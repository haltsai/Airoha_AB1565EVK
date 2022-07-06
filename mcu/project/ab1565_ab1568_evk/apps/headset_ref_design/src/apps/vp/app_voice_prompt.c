/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
 *
 * This software/firmware and related documentation ("Airoha Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to Airoha Technology Corp. ("Airoha") and/or its licensors.
 * Without the prior written permission of Airoha and/or its licensors,
 * any reproduction, modification, use or disclosure of Airoha Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) Airoha Software
 * if you have agreed to and been bound by the applicable license agreement with
 * Airoha ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of Airoha Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT AIROHA SOFTWARE RECEIVED FROM AIROHA AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. AIROHA EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES AIROHA PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH AIROHA SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

/**
 * File: app_voice_prompt.c
 *
 * Description: This file provide implementation of voice prompt management and control.
 *
 */

#ifdef MTK_PROMPT_SOUND_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "hal.h"
#include "hal_audio_internal.h"
#include "bt_connection_manager.h"
#include "bt_sink_srv_utils.h"

#include "app_voice_prompt.h"
#include "app_voice_prompt_nvdm.h"
#include "prompt_control.h"
#include "bt_app_common.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif
#include "bt_connection_manager_internal.h"
#include "bt_connection_manager.h"
#include "bt_gap.h"
#include "rofs.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_srv.h"
#include "apps_config_vp_index_list.h"
#endif
#include "bt_callback_manager.h"
#include "bt_device_manager.h"
#if 0
#include "hal_dvfs.h"
#endif
#ifdef AIR_TILE_ENABLE
#include "app_tile.h"
#endif

#define APP_VP_SYNC_DELAY_MIN  400   /* Minimum delay time required by vp sync. */

/**
 *  @brief This enum defines the context of the module.
 */
typedef struct {
    vp_queue vp_list_low;                    /**<  VP queue of priority VOICE_PROMPT_PRIO_LOW. */
    vp_queue vp_list_medium;                 /**<  VP queue of priority VOICE_PROMPT_PRIO_MEDIUM. */
    vp_queue vp_list_high;                   /**<  VP queue of priority VOICE_PROMPT_PRIO_HIGH. */
    vp_queue vp_list_extrem;                 /**<  VP queue of priority VOICE_PROMPT_PRIO_EXTREME. */
    vp_queue vp_list_ultra;                  /**<  VP queue of priority VOICE_PROMPT_PRIO_ULTRA. */
    SemaphoreHandle_t vp_semaphore_handle;   /**<  Semaphore handle to protect the context. */
    uint16_t count;                          /**<  ID for stop procedure. */
    bool isCleanup;                          /**<  If flag set, will empty all VP who are queuing and reject any VP from now on. */
    bool isToneStopping;                     /**<  flag for waiting for confirm after stop a tone. */
#ifndef APP_VP_NOT_DISABLE_SNIFF
    app_vp_bt_state bt_state;                /**<  Current BT state, sniff, exiting sniff and normal. */
#endif
    bool isInit;                             /**<  Flag to keep init VP status. */
    bool isPlaying;                          /**<  Flag to indicate that VP is playing. */
    uint32_t voiceDelay;                     /**<  Keep delay from caller. */
    bool connected_synced;                   /**<  Flag to indicate that CONNECTED VP is synced and not need sync stop. */
    uint8_t slp_handle;                      /**<  Lock sleep handle. */
    bool slp_locked;                         /**<  Flag to indicate whether sleep is locked. */
    bool sync_immediately;                   /**<  Flag to indicate current CONNECTED VP need to be sync immediately without switch task. */
    bool same_level_preempt;                 /**<  Flag to indicate the incoming VP is set to preempt the same level previous VP. */
#if 0
    bool dvfs_locked;                        /**<  Flag to indicate whether DVFS is locked. */
#endif
} app_voice_prompt_context_t;

static volatile app_voice_prompt_context_t vp_ctx = {{0}}; /* Context of the module. */
extern bool g_app_voice_prompt_test_off;                /* Flag to take off VP playback for test. */
volatile bt_clock_t g_bt_clk_play = {0};                /* Target BT clock to play the current VP. */
volatile uint32_t g_gpt_count_play = 0;                 /* Target gpt count to play the current VP. */
volatile uint32_t g_gpt_count_wait_play = 0;            /* Target gpt count to play the next VP when the current VP is stopping. */

log_create_module(VOICE_PROMPT_APP, PRINT_LEVEL_INFO);

/**
* @brief      This function is the callback registered in prompt_control when play tone.
* @param[in]  event_id, indicate play started or play ended.
*/
static void app_voice_prompt_callback(prompt_control_event_t event_id);

/**
* @brief      This function is to get a dedicated priority queue.
* @param[in]  level, priority of queue to be get.
* @return     pointer of the VP queue.
*/
static vp_queue *app_voice_prompt_get_queue_by_prio(app_vp_prio_t level);

/**
* @brief      This function is to get current highest priority queue which is not empty.
* @return     pointer of the current highest VP queue.
*/
static vp_queue *app_voice_prompt_get_current_queue();

/**
* @brief      This function is to stop current VP and play next.
*/
static void app_voice_prompt_stop_and_play_next();

/**
* @brief      This function is to delete an VP item in queue by id.
* @param[in]  id, the VP id to be deleted.
* @return     VOICE_PROMPT_LIST_SUCCESS when successfully deleted.
*/
static app_voice_prompt_list_status_t app_voice_prompt_delete_item_by_id(uint16_t id);

/**
* @brief      This function is to get an VP item in queue by id.
* @param[in]  id, the VP id to be get.
* @param[out] item, output buffer of the target item.
* @return     VOICE_PROMPT_LIST_SUCCESS when the item is found.
*/
static app_voice_prompt_list_status_t app_voice_prompt_get_item_by_id(uint16_t id, DataType *item);

/**
* @brief      This function is to delete all items in a queue except current node.
* @param[in]  queue, queue to be deleted.
* @return     VOICE_PROMPT_LIST_SUCCESS when successfully deleted.
*/
static app_voice_prompt_list_status_t app_voice_prompt_delete_all_item_excp_curr(vp_queue *queue);

#ifdef MTK_AWS_MCE_ENABLE
/**
* @brief      This function is to play a sync VP.
* @param[in]  tone_idx, VP index.
* @param[in]  delay_time, delay time before play.
* @param[in]  level, VP priority.
* @param[in]  isCleanup, flag of whether need clean up all the vp item.
* @param[in]  repeat, flag of whether need loop.
* @param[in]  callback, user callback.
* @return     true when success.
*/
static bool app_voice_prompt_aws_play(uint32_t tone_idx, uint32_t delay_time, app_vp_type type, app_vp_prio_t level, bool isCleanup, bool repeat, app_vp_play_callback_t callback);

/**
* @brief      This function is the callback registered in aws_mce_report.
* @param[in]  info, sync info received from peer.
*/
static void app_voice_prompt_sync_callback(bt_aws_mce_report_info_t *info);

#ifndef APP_VP_NOT_DISABLE_SNIFF
/**
* @brief      This function is to get the addr of the connected remote device.
* @param[out] addr, output buffer of remote addr.
* @return     BT_STATUS_SUCCESS when success.
*/
static bt_status_t app_voice_prompt_get_SP_bd_addr(bt_bd_addr_t *addr);

/**
* @brief      This function is the callback registered to bt_gap.
* @param[in]  msg, msg type of bt_gap.
* @param[in]  status, bt_gap status.
* @param[in]  buffer, extra data buffer.
* @return     BT_STATUS_SUCCESS when success.
*/
static bt_status_t app_voice_gap_evt_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

/**
* @brief      This function is called when BT sniff mode is enabled.
* @param[in]  status, bt_gap status.
* @param[in]  cnf, bt_gap cnf info.
*/
static void app_voice_enable_sniff_cnf(bt_status_t status, bt_gap_write_link_policy_cnf_t *cnf);

/**
* @brief      This function is to control BT sniff mode.
* @param[in]  enable, true to enable, false to disable.
* @return     BT_STATUS_SUCCESS when success.
*/
static bt_status_t app_voice_prompt_enable_sniff(bool enable);
#endif
#endif

#ifndef APP_VP_NOT_DISABLE_SNIFF
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
/**
* @brief      This function is the callback function registered to RHO service, and update sniff status when RHO completes.
* @param[in]  addr, remote device address.
* @param[in]  role, current AWS role.
* @param[in]  event, RHO event type.
* @param[in]  status, BT status.
*/
static void app_voice_prompt_rho_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status);
#endif
#endif

/**
* @brief      This function is to stop a VP.
* @param[in]  id, the VP id to be stopped.
* @param[in]  need_sync, true if the VP to be stopped is a sync VP.
* @param[in]  delay_time, delay time to stop.
* @param[in]  preempted, true if the it's stopped by a higher priority VP.
* @return     true when success.
*/
static bool app_voice_prompt_stop_internal(uint16_t id, bool need_sync, uint32_t delay_time, bool preempted);

/**
* @brief      This function is to stop a voice.
* @param[in]  need_sync, true if the voice to be stopped is synced.
* @param[in]  delay_time, delay time to stop.
* @param[in]  isRingtone, true if it's a ringtone.
* @param[in]  preempted, true if the it's stopped by a higher priority VP.
* @return     true when success.
*/
static bool app_voice_prompt_voice_stop_internal(bool need_sync, uint32_t delay_time, bool isRingtone, bool preempted);

/**
* @brief      This function is to notify user callback when VP started or stopped.
* @param[in]  callback, user callback.
* @param[in]  index, current VP index.
* @param[in]  err, error code.
*/
static inline void app_voice_prompt_noti(app_vp_play_callback_t callback, uint32_t index, vp_err_code err);

/**
* @brief      This function is to send msg to ui_realtime task.
* @param[in]  type, msg type.
* @param[in]  data, extra data.
*/
void app_vp_task_send_msg(app_voice_prompt_event_t type, void *data);

/**
* @brief      This function is to play next VP in queue.
*/
static void app_voice_prompt_play_next();

static void app_vp_mutex_create(void)
{
    if (vp_ctx.vp_semaphore_handle == NULL) {
        vSemaphoreCreateBinary(vp_ctx.vp_semaphore_handle);
    }
}

static void app_vp_mutex_lock(void)
{
    if (vp_ctx.vp_semaphore_handle != NULL) {
        xSemaphoreTake(vp_ctx.vp_semaphore_handle, portMAX_DELAY);
    }
}

static void app_vp_mutex_unlock(void)
{
    if (vp_ctx.vp_semaphore_handle != NULL) {
        /* In new FreeRTOS version, it would assert if mutex is taken and given in different tasks. */
        xSemaphoreGive(vp_ctx.vp_semaphore_handle);
    }
}

static app_voice_prompt_list_status_t app_voice_prompt_insert_item(vp_queue *q, DataType *item)
{
    /* Limit length of every queue to APP_VOICE_PROMPT_QUEUE_MAX_LEN to avoid heap crash. */
    if (q->length >= APP_VOICE_PROMPT_QUEUE_MAX_LEN) {
        app_voice_prompt_msgid_report("app_voice_prompt VP queue full", 0);
        return VOICE_PROMPT_LIST_FAIL;
    }

    vp_node *node = (vp_node *)pvPortMalloc(sizeof(vp_node));
    app_voice_prompt_msgid_report("insert 0x%08x, current queue length %d", 2, node, q->length);
    if (node == NULL) {
        app_voice_prompt_msgid_error("add vp queue node fail-memory malloc fail!", 0);
        return VOICE_PROMPT_LIST_FAIL;
    }
    memcpy(&(node->data), item, sizeof(DataType));
    node->next = NULL;
    if (q->front == NULL) {
        q->front = node;
    }

    if (q->rear == NULL) {
        q->rear = node;
    } else {
        q->rear->next = node;
        q->rear = node;
    }

    /* Increase queue length. */
    q->length ++;

    return VOICE_PROMPT_LIST_SUCCESS;
}

static app_voice_prompt_list_status_t app_voice_prompt_delete_item(vp_queue *q)
{
    vp_node *node;
    if (q == NULL ||  q->front == NULL) {
        return VOICE_PROMPT_LIST_FAIL;
    } else {
        node = q->front;
        if (q->front == q->rear) {
            q->front = NULL;
            q->rear = NULL;
        } else {
            q->front = q->front->next;
        }
        app_voice_prompt_msgid_report("delete 0x%08x", 1, node);
        vPortFree(node);
        q->length --;
    }
    return VOICE_PROMPT_LIST_SUCCESS;
}

static app_voice_prompt_list_status_t app_voice_prompt_delete_all_item(vp_queue *q)
{
    vp_node *curr = NULL, *temp;

    if (q == NULL || q->front == NULL) {
        return VOICE_PROMPT_LIST_FAIL;
    }

    curr = q->front;
    while (curr) {
        temp = curr;
        curr = curr->next;
        app_voice_prompt_msgid_report("delete 0x%08x", 1, temp);
        vPortFree(temp);
    }
    q->front = NULL;
    q->rear = q->front;
    q->length = 0;

    return VOICE_PROMPT_LIST_SUCCESS;
}

static app_voice_prompt_list_status_t app_voice_prompt_delete_all_item_excp_curr(vp_queue *q)
{
    vp_node *curr = NULL, *temp;

    if (q == NULL || q->front == NULL) {
        return VOICE_PROMPT_LIST_FAIL;
    }

    if (q->front == q->rear) {
        return VOICE_PROMPT_LIST_SUCCESS;
    }

    curr = q->front->next;
    while (curr) {
        temp = curr;
        curr = curr->next;
        app_voice_prompt_msgid_report("delete 0x%08x", 1, temp);
        vPortFree(temp);
    }
    q->rear = q->front;
    q->length = 1;

    return VOICE_PROMPT_LIST_SUCCESS;

}

static app_voice_prompt_list_status_t app_voice_prompt_delete_item_by_id(uint16_t id)
{
    vp_queue *queue = NULL;
    vp_node *curr = NULL, *prev = NULL;
    bool found = false;

    for (int i = VOICE_PROMPT_PRIO_EXTREME; i >= VOICE_PROMPT_PRIO_LOW; i--) {
        queue = app_voice_prompt_get_queue_by_prio(i);

        /* Empty queue. */
        if (queue == NULL || queue->front == NULL) {
            continue;
        } else {
            curr = queue->front;
            while (curr) {
                if (curr->data.id == id) {
                    if (curr == queue->rear && curr == queue->front) {
                        /* Just one vp in queue. */
                        queue->front = NULL;
                        queue->rear = NULL;
                    } else {
                        if (prev != NULL) {
                            prev->next = curr->next;
                            if (curr == queue->rear) {
                                /* Last one will be removed, update rear. */
                                queue->rear = prev;
                            }
                        } else {
                            configASSERT(0);
                        }
                    }
                    app_voice_prompt_msgid_report("delete 0x%08x", 1, curr);
                    vPortFree(curr);
                    queue->length --;
                    found = true;
                    break;
                }
                prev = curr;
                curr = curr->next;
            }
        }

        if (found) {
            break;
        }
    }

    return (found ? VOICE_PROMPT_LIST_SUCCESS : VOICE_PROMPT_LIST_FAIL);
}

static app_voice_prompt_list_status_t app_voice_prompt_get_item_by_id(uint16_t id, DataType *item)
{
    vp_queue *queue = NULL;
    vp_node *curr = NULL;
    bool found = false;

    for (int i = VOICE_PROMPT_PRIO_EXTREME; i >= VOICE_PROMPT_PRIO_LOW; i--) {
        queue = app_voice_prompt_get_queue_by_prio(i);

        /* Empty queue. */
        if (queue == NULL || queue->front == NULL) {
            continue;
        } else {
            curr = queue->front;
            while (curr) {
                if (curr->data.id == id) {
                    memcpy(item, &(curr->data), sizeof(DataType));
                    found = true;
                    break;
                }
                curr = curr->next;
            }
        }

        if (found) {
            break;
        }
    }

    return (found ? VOICE_PROMPT_LIST_SUCCESS : VOICE_PROMPT_LIST_FAIL);
}

static app_voice_prompt_list_status_t app_voice_prompt_copy_item(vp_queue *q, DataType *item)
{
    if (q == NULL ||  q->front == NULL) {
        item = NULL;
        return VOICE_PROMPT_LIST_FAIL;
    } else {
        app_voice_prompt_msgid_report("copy 0x%08x", (uint32_t)q->front);
        memcpy(item, &(q->front->data), sizeof(DataType));
    }
    return VOICE_PROMPT_LIST_SUCCESS;
}


static vp_queue *app_voice_prompt_get_queue_by_prio(app_vp_prio_t level)
{
    switch (level) {
        case VOICE_PROMPT_PRIO_LOW:
            return (vp_queue *) & (vp_ctx.vp_list_low);

        case VOICE_PROMPT_PRIO_MEDIUM:
            return (vp_queue *) & (vp_ctx.vp_list_medium);

        case VOICE_PROMPT_PRIO_HIGH:
            return (vp_queue *) & (vp_ctx.vp_list_high);

        case VOICE_PROMPT_PRIO_ULTRA:
            return (vp_queue *) & (vp_ctx.vp_list_ultra);

        case VOICE_PROMPT_PRIO_EXTREME:
            return (vp_queue *) & (vp_ctx.vp_list_extrem);

        default:
            return NULL;
    }
}

static vp_queue *app_voice_prompt_get_current_queue()
{
    vp_queue *queue = NULL;

    /* Get the none-empty highest priority queue. */
    for (int i = VOICE_PROMPT_PRIO_EXTREME; i >= VOICE_PROMPT_PRIO_LOW; i--) {
        queue = app_voice_prompt_get_queue_by_prio(i);

        /* Skip empty queue. */
        if (queue == NULL || queue->front == NULL) {
            continue;
        } else {
            break;
        }
    }

    return queue;
}

#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
static bt_status_t app_voice_prompt_get_SP_bd_addr(bt_bd_addr_t *addr)
{
    /*
    bt_bd_addr_t addr_list[2];
    uint32_t count = 0;
    uint32_t i;

    count = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_NONE, addr_list, sizeof(addr_list) / sizeof(bt_bd_addr_t));

    for (i = 0; i < count; i++) {
        if (memcmp(addr_list[i], bt_device_manager_aws_local_info_get_peer_address(), sizeof(bt_bd_addr_t) != 0)) {
            // Exclude peer address, return the 1st remote device addr.
            memcpy(*addr, addr_list[i], sizeof(bt_bd_addr_t));
            return BT_STATUS_SUCCESS;
        }
    }
*/

    uint32_t num = 0;
    num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), addr, 1);
    uint8_t *addr_log = (uint8_t *)addr;
    app_voice_prompt_msgid_report("Get AWS SP addr num %d, %x:%x:%x:%x:%x:%x", 7, num, *(addr_log + 5), *(addr_log + 4),
            *(addr_log + 3), *(addr_log + 2), *(addr_log + 1), *(addr_log + 0));

    if (num != 1) {
        return BT_STATUS_FAIL;
    } else {
        return BT_STATUS_SUCCESS;
    }
}

static bt_status_t app_voice_prompt_enable_sniff(bool enable)
{
    bt_bd_addr_t addr = {0};
    bt_gap_connection_handle_t conn_handle = NULL;
    bt_status_t ret = BT_STATUS_SUCCESS;

    if (app_voice_prompt_get_SP_bd_addr(&addr) != BT_STATUS_SUCCESS) {
        app_voice_prompt_msgid_error("Error! cann't get sp addr,", 0);
        return BT_STATUS_FAIL;
    }
    conn_handle = bt_cm_get_gap_handle(addr);

    if (0 != conn_handle) {
        bt_gap_link_policy_setting_t setting;
        if (enable) {
            setting.sniff_mode = BT_GAP_LINK_POLICY_ENABLE;
        } else {
            setting.sniff_mode = BT_GAP_LINK_POLICY_DISABLE;
        }
        ret = bt_gap_write_link_policy(conn_handle, &setting);
    } else {
        ret = BT_STATUS_FAIL;
    }

    if (ret != BT_STATUS_SUCCESS) {
        app_voice_prompt_msgid_warning("handle 0x%x, enable(%d), bt sniff fail. ret: 0x%08x", 3, conn_handle, enable, ret);
    } else {
        app_voice_prompt_msgid_warning("handle 0x%x, enable(%d), bt sniff success.", 2, conn_handle, enable);
    }
    return ret;
}

void app_voice_exit_sniff_cnf(bt_status_t status, bt_gap_sniff_mode_changed_ind_t *ind)
{
    app_voice_prompt_list_data_t item = {0};

    app_vp_mutex_lock();

    app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), (DataType *)&item);
    if (ind == NULL || status != BT_STATUS_SUCCESS) {
        /* Exit sniff mode fail, treat as sniff, play anyway. */
        vp_ctx.bt_state = VP_BT_STATE_SNIFF;
    } else {
        if (ind->sniff_status == BT_GAP_LINK_SNIFF_TYPE_ACTIVE) {
            vp_ctx.bt_state = VP_BT_STATE_ACTIVE;
            /* Disable sniff mode while vp is playing. */
            app_voice_prompt_enable_sniff(false);
        } else {
            vp_ctx.bt_state = VP_BT_STATE_SNIFF;
        }
    }
    if (item.id != 0 && item.need_sync) {
        app_voice_prompt_aws_play(item.index, item.delay_time, item.type, item.level, vp_ctx.isCleanup, item.repeat, item.callback);
    }

    app_vp_mutex_unlock();
}

static void app_voice_enable_sniff_cnf(bt_status_t status, bt_gap_write_link_policy_cnf_t *cnf)
{
    app_voice_prompt_msgid_report("sniff status change cnf, status: 0x%x, sniff mode: 0x%x", 2, status, cnf->sniff_mode);
}

static bt_status_t app_voice_gap_evt_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    switch (msg) {
        case BT_GAP_SNIFF_MODE_CHANGE_IND: {
            bt_gap_sniff_mode_changed_ind_t *ind = (bt_gap_sniff_mode_changed_ind_t *)buffer;
            app_voice_prompt_msgid_report("sniff mode change ind status: 0x%x, bt_state: %d, sniff status: %d", 3, status, vp_ctx.bt_state, ind ? ind->sniff_status : 0xff);
            if (vp_ctx.bt_state == VP_BT_STATE_SNIFF_EXITING) {
                app_voice_prompt_sniff_change_t *data = (app_voice_prompt_sniff_change_t *)pvPortMalloc(sizeof(app_voice_prompt_sniff_change_t));
                if (data == NULL) {
                    app_voice_prompt_msgid_report("sniff mode change ind status: data malloc fail", 0);
                    break;
                }
                data->status = status;
                memcpy(&(data->ind), ind, sizeof(bt_gap_sniff_mode_changed_ind_t));
                /* Send msg to ui_realtime task to implement aws_play. */
                app_vp_task_send_msg(VOICE_PROMPT_EVENT_SNIFF_CHANGE, data);
            }
        }
        break;

        case BT_GAP_WRITE_LINK_POLICY_CNF: {
            app_voice_enable_sniff_cnf(status, (bt_gap_write_link_policy_cnf_t *)buffer);
        }
        break;

        default:
            ;
    }

    return BT_STATUS_SUCCESS;
}
#endif
#endif

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#ifndef APP_VP_NOT_DISABLE_SNIFF
static void app_voice_prompt_rho_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    app_voice_prompt_msgid_report("voice prompt rho, role: 0x%x, state: %d, playing: %d", 3, role, vp_ctx.bt_state, vp_ctx.isPlaying);

    if (BT_ROLE_HANDOVER_COMPLETE_IND == event && status == BT_STATUS_SUCCESS && vp_ctx.isPlaying) {
        /* Update sniff status. */
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            vp_ctx.bt_state = VP_BT_STATE_SNIFF;
        } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
            vp_ctx.bt_state = VP_BT_STATE_ACTIVE;
        }
    }
}
#endif
#endif

static bool app_voice_prompt_toneIdx2FileId(uint32_t tone_idx, uint16_t *fileId)
{
    return app_voice_prompt_VpId2FileId(tone_idx, fileId);
}

void app_voice_prompt_init()
{
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#ifndef APP_VP_NOT_DISABLE_SNIFF
    bt_role_handover_callbacks_t role_callbacks = {NULL, NULL, NULL, NULL, app_voice_prompt_rho_cb};
#endif
#endif

    if (vp_ctx.isInit) {
        return;
    }
    vp_ctx.isInit = true;
#ifndef APP_VP_NOT_DISABLE_SNIFF
    vp_ctx.bt_state = VP_BT_STATE_SNIFF;
#endif

    /* Set hal sleep handle, will lock sleep in sync play to ensure dsp get accurate gpt count. */
    vp_ctx.slp_handle = hal_sleep_manager_set_sleep_handle("app_vp");
    /* Init VP file. */
    app_voice_prompt_VpInit();
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#ifndef APP_VP_NOT_DISABLE_SNIFF
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_VP_APP, &role_callbacks);
#endif
#endif
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_VP, app_voice_prompt_sync_callback);
#ifndef APP_VP_NOT_DISABLE_SNIFF
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP, (void *)app_voice_gap_evt_callback);
#endif
#endif
}

static bool app_voice_prompt_local_play(uint32_t tone_idx, uint32_t sync_time, app_vp_play_callback_t callback)
{
    uint8_t *tone_buf = NULL;
    uint32_t tone_size = 0;
    uint16_t file_id = 0;
    bool ret = true;
    ROFS_FILEINFO_T *pMediaFile;
    prompt_control_tone_type_t codec_type = VPC_MP3;

    app_voice_prompt_msgid_report("voice prompt play index: 0x%x, synctime: 0x%x", 2, tone_idx, sync_time);
/*
    if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED) {
        vp_ctx.sync_immediately = false;
    } */

#ifndef MTK_AUDIO_AT_CMD_PROMPT_SOUND_ENABLE
    /* Get ROFS file ID according to tone_index. */
    if (app_voice_prompt_toneIdx2FileId(tone_idx, &file_id)) {
        /* Read VP file in ROFS. */
        pMediaFile = ROFS_fopen((unsigned short)file_id);
        if (pMediaFile) {
            tone_buf = (uint8_t *)ROFS_address(pMediaFile);
            tone_size = (uint32_t) pMediaFile->BasicInfo.ulFileSize;
            if (strstr(pMediaFile->szFileName, ".mp3")) {
                codec_type = VPC_MP3;
            } else if (strstr(pMediaFile->szFileName, ".wav")) {
                codec_type = VPC_WAV;
            } else if (strstr(pMediaFile->szFileName, ".opus")) {
                codec_type = VPC_OPUS;
            }  else {
                app_voice_prompt_report("voice prompt: type wrong", 0);
            }
            app_voice_prompt_report("voice prompt: fn: %s, file id: %d, buf: 0x%08x, size: 0x%x", pMediaFile->szFileName, file_id, tone_buf, tone_size);
        } else {
            app_voice_prompt_msgid_report("voice prompt: file non found with file id: 0x%x", 1, file_id);
            ret = false;
        }
    } else {
        app_voice_prompt_msgid_report("voice prompt: get file id fail with tone id: 0x%x", 1, tone_idx);
        ret = false;
    }

    if (tone_size != 0 && tone_buf != NULL) {
        app_voice_prompt_msgid_report("prompt_control_play_sync_tone +", 0);
        /* Play VP. */
#ifndef MTK_MP3_TASK_DEDICATE
        prompt_control_play_tone(codec_type, tone_buf, tone_size, app_voice_prompt_callback);
#else
        /* Use target gpt_count to play sync VP, g_gpt_count_play = 0 when it's not a sync VP. */
        prompt_control_play_sync_tone(codec_type, tone_buf, tone_size, g_gpt_count_play, app_voice_prompt_callback);
        /* Reset bt_clock and gpt_count. */
        g_bt_clk_play.nclk = 0;
        g_bt_clk_play.nclk_intra = 0;
        g_gpt_count_play = 0;
#endif
        app_voice_prompt_msgid_report("prompt_control_play_sync_tone -", 0);

        vp_node *cur_node;
        vp_queue *cur_queue = app_voice_prompt_get_current_queue();
        if (cur_queue == NULL || cur_queue->front == NULL) {
            app_voice_prompt_msgid_report("local play cur_queue empty", 0);
        } else {
            /* Set play flag to indicate the item is played. */
            cur_node = cur_queue->front;
            app_voice_prompt_report("local play cur_node 0x%08x played", cur_node);
            cur_node->data.played = true;
        }
    }
#endif
    vp_ctx.isPlaying = true;

    if (!ret) {
        /* Current play fail, notify user, remove and play next. */
        app_voice_prompt_list_data_t item = {0};
        app_voice_prompt_noti(callback, tone_idx, VP_ERR_CODE_FILE_NO_FOUND);
        app_voice_prompt_delete_item(app_voice_prompt_get_current_queue());
        app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), (DataType *)&item);
        if (item.id != 0) {
#ifdef MTK_AWS_MCE_ENABLE
            if (item.need_sync) {
                ret = app_voice_prompt_aws_play(item.index, item.delay_time, item.type, item.level, vp_ctx.isCleanup, item.repeat, item.callback);
            } else
#endif
            {
                ret = app_voice_prompt_local_play(item.index, item.delay_time, item.callback);
            }
        }
    }

    app_voice_prompt_msgid_report("voice prompt play result: %d", 1, ret);

    return ret;
}

static bool app_voice_prompt_local_stop(uint32_t id, uint32_t sync_time, app_vp_type type, bool preempted)
{
    app_voice_prompt_list_data_t item = {0};
    app_voice_prompt_list_status_t ret = VOICE_PROMPT_LIST_SUCCESS;

    app_voice_prompt_msgid_report("voice prompt local stop, id: 0x%d, type: 0x%x", 2, id, type);

    app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item);
    if (item.id == 0) {
        return false;
    }

    if (type == VP_TYPE_VP) {
        if (item.id == id) {
            /* Delete item in queue. */
            ret = app_voice_prompt_delete_item(app_voice_prompt_get_queue_by_prio(item.level));
            if (ret == VOICE_PROMPT_LIST_SUCCESS) {
                /* Stop current VP and wait callback to play next one */
                app_voice_prompt_stop_and_play_next();
            }
            /* Notify user preempted. */
            app_voice_prompt_noti(item.callback, item.index, preempted ? VP_ERR_CODE_PREEMPTED : VP_ERR_CODE_STOP);
        } else {
            /* Stopping VP is queuing, just remove from queue. */
            app_voice_prompt_get_item_by_id(id, &item);
            ret = app_voice_prompt_delete_item_by_id(id);
            if (ret == VOICE_PROMPT_LIST_SUCCESS) {
                /* Notify user preempted. */
                app_voice_prompt_noti(item.callback, item.index, preempted ? VP_ERR_CODE_PREEMPTED : VP_ERR_CODE_STOP);
            }
        }
    } else if (type == VP_TYPE_VB || type == VP_TYPE_RT) {
        switch (item.type) {
            case VP_TYPE_RT:
                if (type == VP_TYPE_RT) {
                    /* Current VP is a ringtone and want to stop it, so just stop it and play next */
                    app_voice_prompt_msgid_report("stop curr ringtone", 0);
                    ret = app_voice_prompt_delete_item(app_voice_prompt_get_queue_by_prio(VOICE_PROMPT_PRIO_ULTRA));
                    if (ret == VOICE_PROMPT_LIST_SUCCESS) {
                        app_voice_prompt_stop_and_play_next();
                    }
                    app_voice_prompt_noti(item.callback, item.index, preempted ? VP_ERR_CODE_PREEMPTED : VP_ERR_CODE_STOP);
                } else {
                    /* Current is ringtone but want to stop voice broadcast, that's abnormal, stop fail. */
                    app_voice_prompt_msgid_error("ringtone playing, stop ringtone only!", 0);
                }

                break;
            case VP_TYPE_VB:
                /* Stop voice broadcast/ringtone and play next */
                if (type == VP_TYPE_VB) {
                    app_voice_prompt_msgid_report("stop all voice boardcast", 0);
                    ret = app_voice_prompt_delete_all_item(app_voice_prompt_get_queue_by_prio(VOICE_PROMPT_PRIO_ULTRA));
                    if (ret == VOICE_PROMPT_LIST_SUCCESS) {
                        app_voice_prompt_stop_and_play_next();
                    }
                    app_voice_prompt_noti(item.callback, item.index,  preempted ? VP_ERR_CODE_PREEMPTED : VP_ERR_CODE_STOP);
                } else {
                    app_voice_prompt_msgid_error("vc playing, stop vc only!", 0);
                }
                break;

            case VP_TYPE_VP:
                /* Just remove all ringtone and voice broadcast. */
                app_voice_prompt_msgid_report("higher vp is playing idx: 0x%x, remove vc and ringtone quite", 1, item.index);
                ret = app_voice_prompt_delete_all_item(app_voice_prompt_get_queue_by_prio(VOICE_PROMPT_PRIO_ULTRA));
                app_voice_prompt_noti(item.callback, item.index,  preempted ? VP_ERR_CODE_PREEMPTED : VP_ERR_CODE_STOP);
                break;

            default:
                break;

        }
    }

    if (ret == VOICE_PROMPT_LIST_SUCCESS) {
        app_voice_prompt_msgid_report("voice prompt remove success", 0);
    } else {
        app_voice_prompt_msgid_report("voice prompt remove fail", 0);
    }
    return (ret == VOICE_PROMPT_LIST_SUCCESS) ? true : false;
}

#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
static bt_status_t app_voice_prompt_exit_sniff_mode(bt_bd_addr_t remote_addr)
{
    bt_status_t status;
    bt_gap_connection_handle_t bt_handle = bt_cm_get_gap_handle(remote_addr);

    status = bt_gap_exit_sniff_mode(bt_handle);
    app_voice_prompt_msgid_report("[BT_CM][E] Exit sniff mode fail status %x !!!", 1, status);
    return status;
}
#endif

static void app_voice_prompt_sync_fail(uint32_t tone_idx, app_vp_play_callback_t callback)
{
    if (!callback) {
        return;
    }

    app_voice_prompt_list_data_t item = {0};
    vp_queue *queue = app_voice_prompt_get_current_queue();
    app_voice_prompt_copy_item(queue, (DataType *)&item);

    if (item.index == tone_idx) {
        app_voice_prompt_delete_item(queue);
    }

    app_voice_prompt_noti(callback, tone_idx, VP_ERR_CODE_SYNC_FAIL);
    app_voice_prompt_play_next();
}

static bool app_voice_prompt_aws_play(uint32_t tone_idx, uint32_t delay_time, app_vp_type type, app_vp_prio_t level, bool isCleanup, bool repeat, app_vp_play_callback_t callback)
{
    bool ret = false;
    app_voice_prompt_aws_play_t *extra_data = NULL;

    if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED) {
        /* CONNECTED VP sync requires to send IF packet without switch task to avoid AWS disconnect. */
        app_voice_prompt_aws_play_impl(tone_idx, delay_time, type, level, isCleanup, repeat, callback);
        return true;
    }

#ifndef APP_VP_NOT_DISABLE_SNIFF
    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
        app_voice_prompt_msgid_report("aws play, state: %d", 1, vp_ctx.bt_state);
        if (vp_ctx.bt_state == VP_BT_STATE_SNIFF) {
            bt_bd_addr_t bd_addr = {0};
            bt_status_t status = BT_STATUS_SUCCESS;

            if (app_voice_prompt_get_SP_bd_addr(&bd_addr) != BT_STATUS_SUCCESS) {
                app_voice_prompt_msgid_error("Error! cann't get sp addr,", 0);
                status = BT_STATUS_FAIL;
            } else {
                status = app_voice_prompt_exit_sniff_mode(bd_addr);
            }
            if (status == BT_STATUS_SUCCESS || status == BT_STATUS_PENDING) {
                /* Waiting for exit sniff mode. */
                app_voice_prompt_msgid_report("exiting sniff mode...", 0);
                vp_ctx.bt_state = VP_BT_STATE_SNIFF_EXITING;
                return true;
            } else if (status == BT_CONNECTION_MANAGER_STATUS_STATE_ALREADY_EXIST) {
                vp_ctx.bt_state = VP_BT_STATE_ACTIVE;
                /* Disable sniff mode while VP playing. */
                app_voice_prompt_enable_sniff(false);
            } else {
                app_voice_prompt_msgid_report("exiting sniff mode fail, 0x%x, play anyway", 1, status);
                vp_ctx.bt_state = VP_BT_STATE_SNIFF;
            }
        } else if (vp_ctx.bt_state == VP_BT_STATE_SNIFF_EXITING) {
            /* Has been waiting. */
            app_voice_prompt_msgid_report("bt is exiting sniff mode, waitting", 0);
            return true;
        }
    }
#endif

    /* Send msg to ui_realtime task to send IF packet. */
    extra_data = (app_voice_prompt_aws_play_t *)pvPortMalloc(sizeof(app_voice_prompt_aws_play_t));
    if (extra_data == NULL) {
        app_voice_prompt_msgid_report("aws play extra data malloc fail", 0);
        return ret;
    }

    extra_data->tone_idx = tone_idx;
    extra_data->delay_time = delay_time;
    extra_data->callback = callback;
    extra_data->isCleanup = isCleanup;
    extra_data->level = level;
    extra_data->repeat = repeat;
    extra_data->type = type;
    app_vp_task_send_msg(VOICE_PROMPT_EVENT_AWS_PLAY, extra_data);
    ret = true;

    return ret;
}

void app_voice_prompt_aws_play_impl(uint32_t tone_idx, uint32_t delay_time, app_vp_type type, app_vp_prio_t level, bool isCleanup, bool repeat, app_vp_play_callback_t callback)
{
    app_voice_prompt_connected_vp_t *data = NULL;

    if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED) {
        /* Prepare msg to send to ui_realtime task to local play CONNECTED VP. */
        data = pvPortMalloc(sizeof(app_voice_prompt_connected_vp_t));
        if (data == NULL) {
            app_voice_prompt_msgid_report("app_voice_prompt_play sync connected: data malloc fail", 0);
            return;
        }
        data->cleanup = isCleanup;
        data->level = level;
        data->callback = callback;
    }

    if (delay_time < APP_VP_SYNC_DELAY_MIN) {
        app_voice_prompt_msgid_report("app_voice_prompt_play_impl delay_time too small, enlarge", 0);
        if (type == VP_TYPE_VP) {
            delay_time = APP_VP_SYNC_DELAY_MIN;
        } else {
            delay_time = 600;
        }
    }

    if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
        /* AWS connected, prepare IF packet to sync. */
        bt_aws_mce_report_info_t info;
        uint8_t param[sizeof(app_voice_prompt_play_sync_t) + 1] = {0};
        app_voice_prompt_play_sync_t *sync_context = (app_voice_prompt_play_sync_t *)(param + 1);

        info.module_id = BT_AWS_MCE_REPORT_MODULE_VP;
        info.sync_time = delay_time;
        info.param_len = sizeof(app_voice_prompt_play_sync_t) + 1;
        info.param = (void *)param;

        param[0] = APP_VOICE_PROMPT_SYNC_PLAY;
        sync_context->play_index = tone_idx;
        sync_context->level = level;
        sync_context->isCleanup = isCleanup;
        sync_context->type = type;
        sync_context->repeat = repeat;
        sync_context->callback = callback;
        sync_context->delay_time = delay_time;
        /* Has increased in local, but partner not yet. */
        sync_context->count = vp_ctx.count - 1;

        /* Lock sleep before get bt_clock and gpt_count to ensure accurate. */
        if (!(vp_ctx.slp_locked)) {
            app_voice_prompt_msgid_report("aws play imp, lock slp", 0);
            hal_sleep_manager_lock_sleep(vp_ctx.slp_handle);
            vp_ctx.slp_locked = true;
        }
        /* Calculate target bt_clock to play. */
        bt_sink_srv_bt_clock_addition(&(sync_context->play_time), 0, delay_time * 1000);
        /* Send target bt_clock to partner to implement sync play. */
        memcpy((void *)&g_bt_clk_play, &(sync_context->play_time), sizeof(bt_clock_t));
        app_voice_prompt_msgid_report("aws play imp, bt_clk 0x%08x 0x%08x", 2, g_bt_clk_play.nclk, g_bt_clk_play.nclk_intra);

#if 0
        hal_dvfs_status_t dvfs_ret;
        app_voice_prompt_msgid_report("aws play imp,, dvfs_locked %d", 1, vp_ctx.dvfs_locked);
        if (!(vp_ctx.dvfs_locked)) {
            /* Lock DVFS to high speed during sync. */
            dvfs_ret = hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_LOCK);
            app_voice_prompt_msgid_report("aws play imp,, dvfs_ret %d", 1, dvfs_ret);
            vp_ctx.dvfs_locked = true;
        }
#endif
        if (bt_aws_mce_report_send_urgent_event(&info) != BT_STATUS_SUCCESS) {
            /* Design change: If sync fail and there's a callback, not play local VP and notify user. */
            app_voice_prompt_msgid_report("aws play imp,, send aws report fail", 0);
            app_voice_prompt_sync_fail(tone_idx, callback);
        }

#ifdef APP_VP_GPIO_DEBUG
        hal_gpio_set_output(HAL_GPIO_1, 1);
#endif
        /* Convert target bt_clock to target gpt_count. */
        if (bt_sink_srv_convert_bt_clock_2_gpt_count((const bt_clock_t *)&g_bt_clk_play, (uint32_t *)&g_gpt_count_play) == BT_STATUS_SUCCESS) {
#ifdef APP_VP_GPIO_DEBUG
            hal_gpio_set_output(HAL_GPIO_1, 0);
#endif

            if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED) {
                data->gpt_count = g_gpt_count_play;
            }

            app_voice_prompt_msgid_report("aws play imp, tar_gpt 0x%08x", 1, g_gpt_count_play);
#if 0
            if ((duration > 1000000) || (duration < 0)) {
                app_voice_prompt_msgid_report("aws play imp, gpt duration wrong, play without delay", 0);
                g_gpt_count_play = 0;

                if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED) {
                    data->gpt_count = 0;
                }

            }
#endif
        } else {
#ifdef APP_VP_GPIO_DEBUG
            hal_gpio_set_output(HAL_GPIO_1, 0);
#endif
            app_voice_prompt_msgid_report("aws play imp, bt_clk to gpt fail", 0);
            /* Convert fail due to AWS disconnected, play without delay. */
            g_gpt_count_play = 0;

            if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED) {
                data->gpt_count = 0;
            }

        }

        if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED) {
            vp_ctx.connected_synced = true;
            /* CONNECTED VP cannot play in BT task, send msg to ui_realtime task. */
            app_vp_task_send_msg(VOICE_PROMPT_EVENT_CONNECTED_VP, data);
        } else {
            /* Local play synced VP. */
            app_voice_prompt_local_play(tone_idx, delay_time, callback);
        }

        app_voice_prompt_msgid_report("aws play imp, voice promt delay_time: %d, vp index=%d", 2, delay_time, sync_context->play_index);
    } else {
        app_voice_prompt_msgid_report("aws play imp, voice promt has no connect aws device,delay_time: 0ms", 0);
        /* Design change: If sync fail and there's a callback, not play local VP and notify user. */
        if (callback) {
            app_voice_prompt_sync_fail(tone_idx, callback);
            if (data) {
                vPortFree(data);
            }
        } else {
            /* AWS not connected, just play without delay. */
            if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED) {
                data->gpt_count = 0;
                app_vp_task_send_msg(VOICE_PROMPT_EVENT_CONNECTED_VP, data);
            } else {
                app_voice_prompt_local_play(tone_idx, delay_time, callback);
            }
        }
    }
    vp_ctx.sync_immediately = false;
}

static bool app_voice_prompt_aws_stop(uint16_t id, uint32_t delay_time, app_vp_type type, bool preempted)
{
    bool ret = false;

    if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
        /* AWS connected, sync stop info to partner. */
        bt_clock_t stop_clock = {0};
        bt_aws_mce_report_info_t info;

        uint8_t param[sizeof(app_voice_prompt_stop_sync_t) + 1] = {0};
        app_voice_prompt_stop_sync_t *sync_context = (app_voice_prompt_stop_sync_t *)(param + 1);

        info.module_id = BT_AWS_MCE_REPORT_MODULE_VP;
        info.sync_time = delay_time;
        info.param_len = sizeof(app_voice_prompt_stop_sync_t) + 1;
        info.param = (void *)param;

        param[0] = APP_VOICE_PROMPT_SYNC_STOP;
        sync_context->id = id;
        sync_context->type = type;
        sync_context->preempted = preempted;

        bt_sink_srv_bt_clock_addition(&stop_clock, 0, delay_time * 1000);
        memcpy(&(sync_context->stop_time), &stop_clock, sizeof(bt_clock_t));

        app_voice_prompt_msgid_report("voice promt stop id: 0x%x, type: 0x%x delay_time: %d", 3,
                                      id, type, delay_time);

        if (bt_aws_mce_report_send_urgent_event(&info) != BT_STATUS_SUCCESS) {
            app_voice_prompt_msgid_report("sync stop fail", 0);
        }
        /* Stop in local. */
        ret = app_voice_prompt_local_stop(id, delay_time, type, preempted);
    } else {
        /* AWS not connect, just stop in local. */
        app_voice_prompt_msgid_report("voice promt has no connect aws device,delay_time: 0ms", 0);
        ret = app_voice_prompt_local_stop(id, 0, type, preempted);
    }

    return ret;
}

void app_voice_prompt_synced_play(app_voice_prompt_synced_play_t *play_data)
{
    app_voice_prompt_msgid_report("app_voice_prompt_synced_play, index %d, type %d, gpt 0x%08x", 3,
                                  play_data->play_index, play_data->type, play_data->gpt_count);

    app_voice_prompt_sync_count(play_data->count);

    /* Play when received a sync VP from agent. */
    if (play_data->type == VP_TYPE_VP) {
        app_voice_prompt_play(play_data->play_index, false, 0, play_data->level, play_data->cleanup, NULL, play_data->gpt_count);
    } else if (play_data->type == VP_TYPE_VB || play_data->type == VP_TYPE_RT) {
        app_voice_prompt_voice_play(play_data->play_index, false, play_data->delay_time,
                                    play_data->level, (play_data->type == VP_TYPE_RT) ? true : false, play_data->repeat, NULL, play_data->gpt_count);
    }
}

void app_voice_prompt_synced_stop(app_voice_prompt_synced_stop_t *stop_data)
{
    app_voice_prompt_msgid_report("app_voice_prompt_synced_stop, id %d, type %d, preempted", 3,
                                  stop_data->id, stop_data->type, stop_data->preempted);

    app_voice_prompt_list_data_t item = {0};

    if (stop_data->preempted) {
        app_vp_mutex_lock();
        if (app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item) == VOICE_PROMPT_LIST_FAIL) {
            app_vp_mutex_unlock();
            return;
        }

        if (stop_data->type == VP_TYPE_VP) {
            app_voice_prompt_stop_internal(stop_data->id, false, 0, true);
        } else if (stop_data->type == VP_TYPE_VB || stop_data->type == VP_TYPE_RT) {
            app_voice_prompt_voice_stop_internal(false, 0, (stop_data->type == VP_TYPE_RT) ? true : false, true);
        }

        if (!item.played && item.id == stop_data->id) {
            /* If current item is not start playing, just play next. */
            vp_ctx.isToneStopping = false;
            app_voice_prompt_play_next();
        }
        app_vp_mutex_unlock();
    } else {
        if (stop_data->type == VP_TYPE_VP) {
            app_voice_prompt_stop(stop_data->id, false, 0);
        } else if (stop_data->type == VP_TYPE_VB || stop_data->type == VP_TYPE_RT) {
            app_voice_prompt_voice_stop(false, 0, (stop_data->type == VP_TYPE_RT) ? true : false);
        }
    }
}


void app_voice_prompt_sync_callback(bt_aws_mce_report_info_t *info)
{
    bt_aws_mce_report_module_id_t owner = info->module_id;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

    if ((owner != BT_AWS_MCE_REPORT_MODULE_VP) || ((role != BT_AWS_MCE_ROLE_AGENT) && (role != BT_AWS_MCE_ROLE_PARTNER))) {
        app_voice_prompt_msgid_report("[app_voice_prompt] voice prompt owner not correct: 0x%2x, role 0x%2x", 2, owner, role);
        return;
    }

    app_voice_prompt_play_sync_t *play_packet = (app_voice_prompt_play_sync_t *)(info->param + 1);
    uint8_t *param = (uint8_t *)info->param;
    if (param[0] == APP_VOICE_PROMPT_SYNC_PLAY) {
        /* Received a sync play request. */
        int32_t duration = info->sync_time;
        app_voice_prompt_play_sync_t sync_index;
        app_voice_prompt_synced_play_t *extra_data;

        memcpy(&sync_index, play_packet, sizeof(app_voice_prompt_play_sync_t));
        memcpy((bt_clock_t *)&g_bt_clk_play, &(sync_index.play_time), sizeof(bt_clock_t));

        app_voice_prompt_msgid_report("[app_voice_prompt] play vp, duration: %d, bt_clk (0x%08x 0x%08x)", 3,
                                      duration, g_bt_clk_play.nclk, g_bt_clk_play.nclk_intra);

        extra_data = (app_voice_prompt_synced_play_t *)pvPortMalloc(sizeof(app_voice_prompt_synced_play_t));
        if (extra_data == NULL) {
            app_voice_prompt_msgid_report("[app_voice_prompt] sync callback, play data malloc fail", 0);
            return;
        }
        extra_data->cleanup = sync_index.isCleanup;
        extra_data->count = sync_index.count;
        extra_data->level = sync_index.level;
        extra_data->play_index = sync_index.play_index;
        extra_data->repeat = sync_index.repeat;
        extra_data->type = sync_index.type;
        extra_data->delay_time = sync_index.delay_time;

        /* Lock sleep before play sync VP to ensure DSP get the accurate gpt_count. */
        if (!(vp_ctx.slp_locked)) {
            app_voice_prompt_msgid_report("[app_voice_prompt] sync callback lock slp", 0);
            hal_sleep_manager_lock_sleep(vp_ctx.slp_handle);
            vp_ctx.slp_locked = true;
        }
#ifdef APP_VP_GPIO_DEBUG
        hal_gpio_set_output(HAL_GPIO_1, 1);
#endif
        /* Convert received target bt_clock to target gpt_count. */
        if (bt_sink_srv_convert_bt_clock_2_gpt_count((const bt_clock_t *)&g_bt_clk_play, &(extra_data->gpt_count)) == BT_STATUS_SUCCESS) {
#ifdef APP_VP_GPIO_DEBUG
            hal_gpio_set_output(HAL_GPIO_1, 0);
#endif
            app_voice_prompt_msgid_report("[app_voice_prompt] sync callback, tar_gpt 0x%08x", 1, extra_data->gpt_count);
        } else {
#ifdef APP_VP_GPIO_DEBUG
            hal_gpio_set_output(HAL_GPIO_1, 0);
#endif
            app_voice_prompt_msgid_report("[app_voice_prompt] sync callback, bt_clk to gpt fail", 0);
            extra_data->gpt_count = 0;
            //assert(0);
        }
#if 0
        if ((duration > 1000000) || (duration < 0)) {
            app_voice_prompt_msgid_report("[app_voice_prompt] sync callback, gpt duration wrong, play without delay", 0);
            extra_data->gpt_count = 0;
        }
#endif
        /* Send msg to ui_realtime task to play sync VP. */
        app_vp_task_send_msg(VOICE_PROMPT_EVENT_SYNC_PLAY, extra_data);
    } else if (param[0] == APP_VOICE_PROMPT_SYNC_STOP) {
        /* Received a sync stop request. */
        app_voice_prompt_stop_sync_t sync_index;
        app_voice_prompt_synced_stop_t *extra_data;

        memcpy(&sync_index, play_packet, sizeof(app_voice_prompt_stop_sync_t));

        app_voice_prompt_msgid_report("[app_voice_prompt]  stop, id: 0x%x, type: 0x%x", 2, sync_index.id, sync_index.type);

        extra_data = (app_voice_prompt_synced_stop_t *)pvPortMalloc(sizeof(app_voice_prompt_synced_stop_t));
        if (extra_data == NULL) {
            app_voice_prompt_msgid_report("[app_voice_prompt] sync callback, stop data malloc fail", 0);
            return;
        }
        extra_data->id = sync_index.id;
        extra_data->type = sync_index.type;
        extra_data->preempted = sync_index.preempted;
        /* Send msg to ui_realtime task to stop sync VP. */
        app_vp_task_send_msg(VOICE_PROMPT_EVENT_SYNC_STOP, extra_data);
    } else if (param[0] == APP_VOICE_PROMPT_SYNC_LANG) {
        app_voice_prompt_lang_sync_t sync_index;
        memcpy(&sync_index, play_packet, sizeof(app_voice_prompt_lang_sync_t));
        app_voice_prompt_msgid_report("[app_voice_prompt]  vp lang: 0x%x", 1, sync_index.lang_index);
        app_voice_prompt_setLang(sync_index.lang_index, false);
    }
    return;
}
#endif

static void app_voice_prompt_stop_and_play_next()
{
    /* Stop vp and wait callback to play next one. */
    prompt_control_stop_tone();
    vp_ctx.isToneStopping = true;
}

static void app_voice_prompt_play_next()
{
    vp_queue *queue = NULL;
    app_voice_prompt_list_data_t item = {0};

    app_voice_prompt_msgid_report("app_voice_prompt_play_next\n", 0);

    queue = app_voice_prompt_get_current_queue();

    if (queue != NULL && queue->front) {
        bool isPlayed = false;
        app_voice_prompt_copy_item(queue, (DataType *)&item);
#ifdef MTK_AWS_MCE_ENABLE
        if (item.need_sync) {
            isPlayed = app_voice_prompt_aws_play(item.index, item.delay_time, item.type, item.level, vp_ctx.isCleanup, item.repeat, item.callback);
        } else
#endif
        {
            isPlayed = app_voice_prompt_local_play(item.index, item.delay_time, item.callback);
        }

        if (!isPlayed) {
            app_voice_prompt_msgid_report("next one play fail, have to end vp play", 0);
            vp_ctx.isPlaying = false;
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
            /* VP play end, enable sniff mode. */
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT && vp_ctx.bt_state != VP_BT_STATE_SNIFF) {
                app_voice_prompt_enable_sniff(true);
                vp_ctx.bt_state = VP_BT_STATE_SNIFF;
            }
#endif
#endif
        }
    } else {
        /* Nothing need to play, BT will enter sniff. */
        app_voice_prompt_msgid_report("queue is empty, have to end vp play", 0);
        vp_ctx.isPlaying = false;
        if (vp_ctx.slp_locked) {
            app_voice_prompt_msgid_report("app_voice_prompt_play_next, unlock slp", 0);
            hal_sleep_manager_unlock_sleep(vp_ctx.slp_handle);
            vp_ctx.slp_locked = false;
        }
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT && vp_ctx.bt_state != VP_BT_STATE_SNIFF) {
            app_voice_prompt_enable_sniff(true);
            vp_ctx.bt_state = VP_BT_STATE_SNIFF;
        }
#endif
#endif
    }

}

static void app_voice_prompt_callback(prompt_control_event_t event_id)
{
    vp_queue *queue = NULL;
    app_voice_prompt_list_data_t item = {0};

    app_voice_prompt_msgid_report("voice prompt callback event=%d\n", 1, event_id);
    app_vp_mutex_lock();

    if (event_id == PROMPT_CONTROL_MEDIA_PLAY) {
        /* Current VP has started playback. */
        queue = app_voice_prompt_get_current_queue();
        app_voice_prompt_copy_item(queue, (DataType *)&item);

        app_voice_prompt_msgid_report("voice prompt start play %d idx 0x%x, level: 0x%x\n", 3,
                                      item.type, item.index, item.level);
        /* Just notify user. */
        app_voice_prompt_noti(item.callback, item.index, VP_ERR_CODE_START_PLAY);
        app_vp_mutex_unlock();
        return;
    }

    if (event_id == PROMPT_CONTROL_MEDIA_END) {
#if 0
        /* If DVFS is locked, unlock it since play ended. */
        if (vp_ctx.dvfs_locked) {
            hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_UNLOCK);
            vp_ctx.dvfs_locked = false;
        }
#endif
        queue = app_voice_prompt_get_current_queue();
        app_voice_prompt_copy_item(queue, (DataType *)&item);

        app_voice_prompt_msgid_report("voice prompt play type(%d) idx 0x%x end, level: 0x%x\n", 3,
                                      item.type, item.index, item.level);

        if (vp_ctx.isToneStopping) {
            /* Prev vp has been removed in stop procedure, so no need to remove here. */
            app_voice_prompt_msgid_report("stop success, play next one", 0);
            vp_ctx.isToneStopping = false;
        } else {
#ifdef MTK_AWS_MCE_ENABLE
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if (item.id != 0 && item.type == VP_TYPE_RT && item.repeat
                    && (role == BT_AWS_MCE_ROLE_AGENT)) {
                /* Loop play if ringtone, only agent trigger loop. */
#else
            if (item.id != 0 && item.type == VP_TYPE_RT && item.repeat) {
#endif
                app_voice_prompt_msgid_report("ringtone loop play..... %d, %d", 2, item.delay_time, vp_ctx.voiceDelay);
                if (VP_VOICE_DEFAULT_DELAY == item.delay_time) {
                    /* Update caller delay after first play. */
                    queue->front->data.delay_time = vp_ctx.voiceDelay;
                }
                queue->front->data.played = false;
                vTaskDelay(50 / portTICK_PERIOD_MS);
            } else {
                /* If not loop, notify user. */
                app_voice_prompt_noti(item.callback, item.index,  VP_ERR_CODE_SUCCESS);
                app_voice_prompt_delete_item(queue);
                queue = app_voice_prompt_get_current_queue();
            }
        }

        /* Play next if there is. */
        if (queue != NULL && queue->front) {
            bool isPlayed = false;
            app_voice_prompt_copy_item(queue, (DataType *)&item);
#ifdef MTK_AWS_MCE_ENABLE
            if (item.need_sync) {
                isPlayed = app_voice_prompt_aws_play(item.index, item.delay_time, item.type, item.level, vp_ctx.isCleanup, item.repeat, item.callback);
            } else
#endif
            {
                /* If a sync VP is in queue, need update gpt_count. only occur on partner. */
                if (g_gpt_count_wait_play != 0) {
                    g_gpt_count_play = g_gpt_count_wait_play;
                    g_gpt_count_wait_play = 0;
                }
                isPlayed = app_voice_prompt_local_play(item.index, item.delay_time, item.callback);
            }

            if (!isPlayed) {
                app_voice_prompt_msgid_report("next one play fail, have to end vp play", 0);
                vp_ctx.isPlaying = false;
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
                if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT && vp_ctx.bt_state != VP_BT_STATE_SNIFF) {
                    app_voice_prompt_enable_sniff(true);
                    vp_ctx.bt_state = VP_BT_STATE_SNIFF;
                }
#endif
#endif
            }
        } else {
            /* Nothing need to play, BT will enter sniff mode. */
            app_voice_prompt_msgid_report("queue is empty, have to end vp play", 0);
            vp_ctx.isPlaying = false;
            /* Unlock sleep since VP play ended. */
            if (vp_ctx.slp_locked) {
                app_voice_prompt_msgid_report("prompt callback, slp unlock", 0);
                hal_sleep_manager_unlock_sleep(vp_ctx.slp_handle);
                vp_ctx.slp_locked = false;
            }
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT && vp_ctx.bt_state != VP_BT_STATE_SNIFF) {
                app_voice_prompt_enable_sniff(true);
                vp_ctx.bt_state = VP_BT_STATE_SNIFF;
            }
#endif
#endif
        }

    }

    app_vp_mutex_unlock();
}

static inline void app_voice_prompt_noti(app_vp_play_callback_t callback, uint32_t index, vp_err_code err)
{
    if (callback != NULL) {
        callback(index, err);
    }
    if (VP_ERR_CODE_START_PLAY == err) {
        app_vp_task_send_msg(VOICE_PROMPT_EVENT_PLAY_START, NULL);
    }
}

uint16_t app_voice_prompt_play(uint32_t tone_idx, bool need_sync, uint32_t delay_time, app_vp_prio_t level, bool cleanup, app_vp_play_callback_t callback, uint32_t gpt_count)
{
    app_voice_prompt_list_data_t item, cur_item = {0};
    app_voice_prompt_list_status_t ret = VOICE_PROMPT_LIST_SUCCESS;
    uint16_t id = 0;
    vp_queue *queue = NULL;
    bool stop_success = true;

#ifdef AIR_TILE_ENABLE
    if (app_tile_song_is_in_progress()) {
        return 0;
    }
#endif

#ifdef MTK_AWS_MCE_ENABLE
    if (need_sync && (delay_time < APP_VP_SYNC_DELAY_MIN)) {
        /* Sync VP delay time must be no less than APP_VP_SYNC_DELAY_MIN to avoid expired. */
        app_voice_prompt_msgid_report("incoming sync vp delay_time %d, set to %d", 2, delay_time, APP_VP_SYNC_DELAY_MIN);
        delay_time = APP_VP_SYNC_DELAY_MIN;
    }

    if (vp_ctx.sync_immediately && tone_idx == VP_INDEX_CONNECTED && need_sync) {
        /* CONNECTED VP need to be sync immediately to avoid AWS disconnect. */
        app_voice_prompt_msgid_report("app_voice_prompt_play sync connected VP delay_time %d", 1, delay_time);
        app_voice_prompt_aws_play(tone_idx, delay_time, VP_TYPE_VP, level, cleanup, false, callback);
        return -1;
    }
#endif

    app_vp_mutex_create();

    app_vp_mutex_lock();

    app_voice_prompt_msgid_report("incoming vp gpt_count 0x%08x", 1, gpt_count);
    g_gpt_count_play = gpt_count;

    do {
        /* Voice broadcast and ringtone are invalid in this func, please use app_voice_prompt_voice_play(). */
        if (level == VOICE_PROMPT_PRIO_ULTRA ||
                level < VOICE_PROMPT_PRIO_LOW || level > VOICE_PROMPT_PRIO_EXTREME) {
            app_voice_prompt_msgid_error("Error, invaild level 0x%x, play vp fail.", 1, level);
            break;
        }

        if (g_app_voice_prompt_test_off) {
            app_voice_prompt_msgid_warning("Warning, VP testing runing, skp idx: 0x%x.", 1, tone_idx);
            break;
        }

        queue = app_voice_prompt_get_queue_by_prio(level);

        if (vp_ctx.isCleanup) {
            /* Only power off VP can cleanup. */
            app_voice_prompt_msgid_warning("Warning, power off..., skip idx: 0x%x", 1, tone_idx);
            break;
        }

        /* Get current item. */
        app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &cur_item);
        if (cur_item.id != 0) {
            app_voice_prompt_msgid_report("incoming vp idx: 0x%x, level: 0x%x, current vp: 0x%x, level: 0x%x", 4,
                                          tone_idx, level, cur_item.index, cur_item.level);
        } else {
            app_voice_prompt_msgid_report("incoming vp idx: 0x%x, level: 0x%x", 2, tone_idx, level);
        }
#if 0
        vp_node *node_rear = NULL;
        node_rear = queue->rear;
        if (node_rear && (node_rear->data.index == tone_idx)) {
            app_voice_prompt_msgid_warning("Warning, queue the same vp, skip idx: 0x%x", 1, tone_idx);
            break;
        }
#endif
        /* For uint16_t count overflow. */
        if (vp_ctx.count == 0) {
            vp_ctx.count++;
        }

        if (cleanup) {
            /* Empty all queue. caution: after this, all nodes from queue will be freed. */
            if (level != VOICE_PROMPT_PRIO_EXTREME) {
                app_voice_prompt_msgid_report(" clean up fail due to not extreme level, cur level: 0x%x", 1, level);
                break;
            }
            /* Keep cleanup flag once set. */
            vp_ctx.isCleanup = cleanup;
            for (int i = VOICE_PROMPT_PRIO_EXTREME; i >= VOICE_PROMPT_PRIO_LOW; i--) {
                if (i == cur_item.level) {
                    /* Current item will be delete in stop procedure. */
                    app_voice_prompt_delete_all_item_excp_curr(app_voice_prompt_get_queue_by_prio(cur_item.level));
                } else {
                    app_voice_prompt_delete_all_item(app_voice_prompt_get_queue_by_prio(i));
                }
            }
        }

        item.need_sync = need_sync;
        item.index = tone_idx;
        item.delay_time = delay_time;
        item.id = ++vp_ctx.count;
        item.level = level;
        item.callback = callback;
        item.type = VP_TYPE_VP;
        item.repeat = false;
        item.played = false;

        if (cur_item.id != 0) {
            /* Someone is playing. */

            if (level > cur_item.level || (level == cur_item.level && vp_ctx.same_level_preempt)) {
#ifdef MTK_AWS_MCE_ENABLE
                /* If CONNECTED VP has been synced, the current sync VP will be stop by preempted, so no need sync stop. set flag. */
                if (tone_idx == VP_INDEX_CONNECTED && vp_ctx.connected_synced) {
                    //cur_item.need_sync = false;
                    vp_ctx.connected_synced = false;
                }
#endif

                /* Higher priority VP incoming, stop current. */
                app_voice_prompt_msgid_report("stop current type %d and play idx: 0x%x", 2, cur_item.type, tone_idx);

                if (gpt_count != 0) {
                    /* It's a synced VP, record target gpt_count. */
                    g_gpt_count_wait_play = gpt_count;
                }

                if (cur_item.level == VOICE_PROMPT_PRIO_ULTRA) {
                    /* Stop ringtone and voice broadcast. */
                    stop_success = app_voice_prompt_voice_stop_internal(cur_item.need_sync, cur_item.delay_time, (cur_item.type == VP_TYPE_RT) ? true : false, true);
                } else {
                    stop_success = app_voice_prompt_stop_internal(cur_item.id, cur_item.need_sync, cur_item.delay_time, true);
                }
                ret = app_voice_prompt_insert_item(queue, &item);
                if (VOICE_PROMPT_LIST_SUCCESS != ret) {
                    app_voice_prompt_msgid_report("queuing vp fail, id: 0x%x", 1, tone_idx);
                    break;
                }
                id = item.id;

                /* Play tone if stop fail or current item has not been played. */
                if (!stop_success || (!cur_item.played)) {
                    if (!cur_item.played) {
                        vp_ctx.isToneStopping = false;
                    }
                    g_gpt_count_wait_play = 0;
                    bool isPlayed = false;
                    app_voice_prompt_msgid_report("stop current vp fail, just play", 0);
#ifdef MTK_AWS_MCE_ENABLE
                    if (need_sync) {
                        isPlayed = app_voice_prompt_aws_play(tone_idx, delay_time, VP_TYPE_VP, level, cleanup, item.repeat, item.callback);
                    } else
#endif
                    {
                        isPlayed = app_voice_prompt_local_play(tone_idx, delay_time, item.callback);
                    }
                    if (!isPlayed) {
                        vp_ctx.isPlaying = false;
                        id = 0;
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT && vp_ctx.bt_state != VP_BT_STATE_SNIFF) {
                            app_voice_prompt_enable_sniff(true);
                            vp_ctx.bt_state = VP_BT_STATE_SNIFF;
                        }
#endif
#endif
                    }
                }
            } else {
                /* Priority is lower than current VP, just enqueue and wait. */
                app_voice_prompt_msgid_report("just queue idx: 0x%x", 1, tone_idx);
                ret = app_voice_prompt_insert_item(queue, &item);
                if (VOICE_PROMPT_LIST_SUCCESS != ret) {
                    app_voice_prompt_msgid_report("queuing vp fail, id: 0x%x", 1, tone_idx);
                    break;
                }
                id = item.id;
            }
        } else {
            /* There's no VP playing currently. */
            ret = app_voice_prompt_insert_item(queue, &item);
            if (VOICE_PROMPT_LIST_SUCCESS != ret) {
                app_voice_prompt_msgid_report("queuing vp fail, id: 0x%x", 1, tone_idx);
                break;
            }
            id = item.id;

            if (vp_ctx.isToneStopping) {
                /* Wait to stop successfully, just wait. */
                app_voice_prompt_msgid_report("someone is stopping, skip play now.", 0);
                id = 0;
            } else {
                /* No one playing and no one stopping, just play. */
                bool isPlayed = false;
#ifdef MTK_AWS_MCE_ENABLE
                if (need_sync) {
                    isPlayed = app_voice_prompt_aws_play(tone_idx, delay_time, VP_TYPE_VP, level, cleanup, item.repeat, item.callback);
                } else
#endif
                {
                    isPlayed = app_voice_prompt_local_play(tone_idx, delay_time, item.callback);
                }
                if (!isPlayed) {
                    vp_ctx.isPlaying = false;
                    id = 0;
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
                    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT && vp_ctx.bt_state != VP_BT_STATE_SNIFF) {
                        app_voice_prompt_enable_sniff(true);
                        vp_ctx.bt_state = VP_BT_STATE_SNIFF;
                    }
#endif
#endif
                }

            }
        }
        vp_ctx.same_level_preempt = false;
        app_voice_prompt_msgid_report("play id: 0x%x", 1, id);
    }
    while (0);

    app_vp_mutex_unlock();

    return id;
}

bool app_voice_prompt_stop(uint16_t id, bool need_sync, uint32_t delay_time)
{
    bool ret = false;
    app_voice_prompt_list_data_t item = {0};

    app_vp_mutex_lock();

    if (app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item) == VOICE_PROMPT_LIST_FAIL) {
        app_vp_mutex_unlock();
        return false;
    }
    ret = app_voice_prompt_stop_internal(id, need_sync, delay_time, false);

    if (!item.played && item.id == id) {
        /* Current VP has not been played, so there's no callback to wait, just play next. */
        vp_ctx.isToneStopping = false;
        app_voice_prompt_play_next();
    }

    app_vp_mutex_unlock();

    return ret;
}

static bool app_voice_prompt_stop_internal(uint16_t id, bool need_sync, uint32_t delay_time, bool preempted)
{
    bool ret = false;
    app_voice_prompt_list_data_t item = {0};

    app_voice_prompt_msgid_report("voice prompt stop internal, id: 0x%x, preempt: %d", 2, id, preempted);

    if (id == 0) {
        app_voice_prompt_msgid_warning("voice prompt stop invalid id, stop fail.", 0);
        return true;
    }

    app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item);
    if (item.id == 0 || item.type != VP_TYPE_VP) {
        app_voice_prompt_msgid_warning("voice prompt not vp playing or invaild type", 0);
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (need_sync) {
        ret = app_voice_prompt_aws_stop(id, delay_time, VP_TYPE_VP, preempted);
    } else
#endif
    {
        ret = app_voice_prompt_local_stop(id, delay_time, VP_TYPE_VP, preempted);
    }

    app_voice_prompt_msgid_report("voice prompt stop internal done.", 0);

    return ret;
}

uint16_t app_voice_prompt_voice_play(uint32_t tone_idx, bool need_sync, uint32_t delay_time, app_vp_prio_t level, bool isRingtone, bool repeat, app_vp_play_callback_t callback, uint32_t gpt_count)
{
    uint16_t id = 0;
    vp_queue *queue = NULL;
    app_voice_prompt_list_data_t item, cur_item = {0};
    app_voice_prompt_list_status_t ret = VOICE_PROMPT_LIST_SUCCESS;
    bool stop_success = true;

    app_vp_mutex_create();
    app_vp_mutex_lock();

    app_voice_prompt_msgid_report("incoming voice gpt_count 0x%08x", 1, gpt_count);
    g_gpt_count_play = gpt_count;

    do {
        /* Only voice broadcast and ringtone come here. */
        if (level != VOICE_PROMPT_PRIO_ULTRA) {
            app_voice_prompt_msgid_error("Error, invaild level 0x%x, play voice fail.", 1, level);
            break;
        }

        if (g_app_voice_prompt_test_off) {
            app_voice_prompt_msgid_warning("Warning, vp testing runing, skp idx: 0x%x.", 1, tone_idx);
            break;
        }
        queue = app_voice_prompt_get_queue_by_prio(level);

        if (vp_ctx.isCleanup) {
            /* Only power off VP can cleanup. */
            app_voice_prompt_msgid_warning("Warning, power off..., skip idx: 0x%x", 1, tone_idx);
            break;
        }

        /* For uint16_t count overflow. */
        if (vp_ctx.count == 0) {
            vp_ctx.count++;
        }

        /* Get current. */
        app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &cur_item);
        if (cur_item.id != 0) {
            app_voice_prompt_msgid_report("incoming isRingtone(%d) id: 0x%x, level: 0x%x, current type %d: 0x%x, level: 0x%x",
                                          6, isRingtone, tone_idx, level, cur_item.type, cur_item.index, cur_item.level);

            if (isRingtone && cur_item.type == VP_TYPE_VB) {
                /* If current is VC and ringtone coming, remove all VB. */
                app_voice_prompt_delete_all_item_excp_curr(app_voice_prompt_get_queue_by_prio(cur_item.level));
            } else if (!isRingtone && cur_item.type == VP_TYPE_RT) {
                /* If current is ringtone and VC coming, reject it. */
                app_voice_prompt_msgid_warning("ringtone is playing, reject vb ", 0);
                break;
            } else if (isRingtone && cur_item.type == VP_TYPE_RT && tone_idx == cur_item.index) {
                /* If the same ringtone comes, just update repeat flag and need_sync flag. */
                vp_node *node = app_voice_prompt_get_current_queue()->front;
                node->data.repeat = repeat;
                node->data.need_sync = need_sync;
                app_voice_prompt_msgid_warning("the same ringtone is playing, just update flag", 0);
                break;
            }
        } else {
            app_voice_prompt_msgid_report("incoming isRingtone(%d) id: 0x%x, level: 0x%x", 3, isRingtone, tone_idx, level);
        }

        item.need_sync = need_sync;
        item.index = tone_idx;
        if (repeat && isRingtone) {
            item.delay_time = VP_VOICE_DEFAULT_DELAY;
            vp_ctx.voiceDelay = delay_time;
        } else {
            item.delay_time = delay_time;
        }
        item.id = ++vp_ctx.count;
        item.level = level;
        item.type = (isRingtone ? VP_TYPE_RT : VP_TYPE_VB);
        item.repeat = repeat;
        item.callback = callback;
        item.played = false;

        if (cur_item.id != 0) {
            /* Someone is playing. */
            if (level > cur_item.level) {
                /* Higher priority VP incoming, stop current. */
                app_voice_prompt_msgid_report("stop current type %d: 0x%x than play id: 0x%x", 3, cur_item.type, cur_item.index, tone_idx);
                stop_success = app_voice_prompt_stop_internal(cur_item.id, cur_item.need_sync, cur_item.delay_time, true);
                ret = app_voice_prompt_insert_item(queue, &item);
                if (VOICE_PROMPT_LIST_SUCCESS != ret) {
                    app_voice_prompt_msgid_report("queuing voice fail, id: 0x%x", 1, tone_idx);
                    break;
                }
                id = item.id;

                /* If current VP has not been play, just play since there won't be callback come. */
                if (!stop_success || (!cur_item.played)) {
                    if (!cur_item.played) {
                        vp_ctx.isToneStopping = false;
                    }
                    bool isPlayed = false;
                    app_voice_prompt_msgid_report("stop current rt or vb fail, just play", 0);
#ifdef MTK_AWS_MCE_ENABLE
                    if (need_sync) {
                        isPlayed = app_voice_prompt_aws_play(tone_idx, item.delay_time, isRingtone ? VP_TYPE_RT : VP_TYPE_VB, level, false, repeat, item.callback);
                    } else
#endif
                    {
                        isPlayed = app_voice_prompt_local_play(tone_idx, item.delay_time, item.callback);
                    }
                    if (!isPlayed) {
                        vp_ctx.isPlaying = false;
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT && vp_ctx.bt_state != VP_BT_STATE_SNIFF) {
                            app_voice_prompt_enable_sniff(true);
                            vp_ctx.bt_state = VP_BT_STATE_SNIFF;
                        }
#endif
#endif
                    }

                }
            } else {
                app_voice_prompt_msgid_report("just queue id: 0x%x", 1, tone_idx);
                ret = app_voice_prompt_insert_item(queue, &item);
                if (VOICE_PROMPT_LIST_SUCCESS != ret) {
                    app_voice_prompt_msgid_report("queuing voice fail, id: 0x%x", 1, tone_idx);
                    break;
                }
                id = item.id;
            }
        } else {
            /* No one playing. */
            ret = app_voice_prompt_insert_item(queue, &item);
            if (VOICE_PROMPT_LIST_SUCCESS != ret) {
                app_voice_prompt_msgid_report("queuing voice fail, id: 0x%x", 1, tone_idx);
                break;
            }
            id = item.id;

            if (vp_ctx.isToneStopping) {
                /* Wait to stop successfully, just keep wait. */
                app_voice_prompt_msgid_report("someone is stopping, skip play now.", 0);
            } else {
                /* No one playing and no one stopping, just play. */
                bool isPlayed = false;
#ifdef MTK_AWS_MCE_ENABLE
                if (need_sync) {
                    isPlayed = app_voice_prompt_aws_play(tone_idx, item.delay_time, isRingtone ? VP_TYPE_RT : VP_TYPE_VB, level, false, repeat, item.callback);
                } else
#endif
                {
                    isPlayed = app_voice_prompt_local_play(tone_idx, item.delay_time, item.callback);
                }
                if (!isPlayed) {
                    vp_ctx.isPlaying = false;
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
                    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT && vp_ctx.bt_state != VP_BT_STATE_SNIFF) {
                        app_voice_prompt_enable_sniff(true);
                        vp_ctx.bt_state = VP_BT_STATE_SNIFF;
                    }
#endif
#endif
                }
            }
        }
    } while (0);

    app_vp_mutex_unlock();

    return id;
}

bool app_voice_prompt_voice_stop(bool need_sync, uint32_t delay_time, bool isRingtone)
{
    bool ret = false;
    app_voice_prompt_list_data_t item = {0};

    app_vp_mutex_lock();

    if (app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item) == VOICE_PROMPT_LIST_FAIL) {
        app_vp_mutex_unlock();
        return false;
    }
    ret = app_voice_prompt_voice_stop_internal(need_sync, delay_time, isRingtone, false);
    if (!item.played) {
        /* Current VP has not been played, so there's no callback to wait, just play next. */
        vp_ctx.isToneStopping = false;
        app_voice_prompt_play_next();
    }

    app_vp_mutex_unlock();

    return ret;
}

static bool app_voice_prompt_voice_stop_internal(bool need_sync, uint32_t delay_time, bool isRingtone, bool preempted)
{
    bool ret = false;
    app_voice_prompt_list_data_t item = {0};

    app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item);
    if (item.id != 0 && (item.type == VP_TYPE_VB || item.type == VP_TYPE_RT)) {
        app_voice_prompt_msgid_report("stop internal isRingtone(%d): curr type: %d, idx 0x%x, level: %d, preempt: %d",
                                      5, isRingtone, item.type, item.index, item.level, preempted);
    } else {
        app_voice_prompt_msgid_report("stop internal isRingtone(%d), no one playing or invalid type, curr idx: 0x%x type: %d",
                                      3, isRingtone, item.index, item.type);
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (need_sync && !(vp_ctx.isCleanup)) {
        ret = app_voice_prompt_aws_stop(item.id, delay_time, isRingtone ? VP_TYPE_RT : VP_TYPE_VB, preempted);
    } else
#endif
    {
        ret = app_voice_prompt_local_stop(item.id, delay_time, isRingtone ? VP_TYPE_RT : VP_TYPE_VB, preempted);
    }

    return ret;
}

app_vp_type app_voice_prompt_get_current_type()
{
    app_voice_prompt_list_data_t item = {0};

    app_vp_mutex_lock();
    app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item);
    app_vp_mutex_unlock();

    if (item.id == 0) {
        return VP_TYPE_NONE;
    } else {
        return item.type;
    }
}


uint16_t app_voice_prompt_get_current_index()
{
    app_voice_prompt_list_data_t item = {0};

    app_vp_mutex_lock();
    app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item);
    app_vp_mutex_unlock();

    if (item.id != 0) {
        return item.index;
    } else {
        return 0xFFFF;
    }
}

uint16_t app_voice_prompt_get_current_id()
{
    app_voice_prompt_list_data_t item = {0};
    app_vp_mutex_lock();
    app_voice_prompt_copy_item(app_voice_prompt_get_current_queue(), &item);
    app_vp_mutex_unlock();
    return item.id;
}

void app_voice_prompt_sync_count(uint16_t count)
{
    vp_ctx.count = count;
    app_voice_prompt_msgid_report("current count: 0x%x", 1, count);
}


bool app_voice_prompt_setLang(uint8_t index, bool need_sync)
{
#ifdef MTK_AWS_MCE_ENABLE
    if (need_sync) {
        if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
            bt_aws_mce_report_info_t info;
            uint8_t param[sizeof(app_voice_prompt_lang_sync_t) + 1] = {0};
            app_voice_prompt_lang_sync_t *lang_context = (app_voice_prompt_lang_sync_t *)(param + 1);

            info.module_id = BT_AWS_MCE_REPORT_MODULE_VP;
            info.sync_time = 0;
            info.is_sync = false;
            info.param_len = sizeof(app_voice_prompt_lang_sync_t) + 1;
            info.param = (void *)param;
            param[0] = APP_VOICE_PROMPT_SYNC_LANG;
            lang_context->lang_index = index;
            bt_aws_mce_report_send_event(&info);
        }

        return app_voice_prompt_VPSetLang(index);
    } else
#endif
    {
        return app_voice_prompt_VPSetLang(index);
    }
}

uint8_t app_voice_prompt_getLang()
{
    uint8_t index = 0xFF;

    app_voice_prompt_VPGetCurrentLangId(&index);
    return index;
}


uint16_t app_voice_prompt_getLangCount()
{
    uint16_t count = 0;

    app_voice_prompt_GetSupportLangCnt(&count);
    return count;
}


bool app_voice_prompt_getSupportLang(uint16_t *buffer)
{
    if (buffer == NULL) {
        return false;
    } else {
        return app_voice_prompt_GetSupportLang(buffer);
    }
}

void app_voice_prompt_dump_all()
{
    vp_queue *queue = NULL;
    vp_node *node;
    app_vp_mutex_lock();

    app_voice_prompt_msgid_report("-----------------------dump queuing vp begin-------------------------", 0);
    /* Loop read every queue. */
    for (int i = VOICE_PROMPT_PRIO_EXTREME; i >= VOICE_PROMPT_PRIO_LOW; i--) {
        queue = app_voice_prompt_get_queue_by_prio(i);

        /* Empty queue. */
        if (queue == NULL || queue->front == NULL) {
            continue;
        } else {
            node = queue->front;
            while (node) {
                app_voice_prompt_msgid_report("queue: 0x%08x, id: 0x%x, vp idx: 0x%x, level: %d, type: %d", 5,
                                              queue, node->data.id, node->data.index, node->data.level, node->data.type);
                node = node->next;
            }
        }
    }
    app_voice_prompt_msgid_report("-----------------------dump queuing vp end---------------------------", 0);
    app_vp_mutex_unlock();
}

void app_voice_prompt_set_sync_immediat()
{
    app_vp_type cur_type = app_voice_prompt_get_current_type();
    if (cur_type == VP_TYPE_VP || cur_type == VP_TYPE_NONE) {
        vp_ctx.sync_immediately = true;
    }
}

void app_voice_prompt_set_preempt()
{
    app_vp_type cur_type = app_voice_prompt_get_current_type();
    app_vp_mutex_lock();
    if (cur_type != VP_TYPE_NONE) {
        vp_ctx.same_level_preempt = true;
    }
    app_vp_mutex_unlock();
}

void app_voice_prompt_remote_disconnect()
{
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
    app_vp_task_send_msg(VOICE_PROMPT_EVENT_REMOTE_DISCONNECT, NULL);
#endif
#endif
}

#ifdef MTK_AWS_MCE_ENABLE
void app_voice_prompt_remote_disconnect_impl()
{
    app_vp_mutex_lock();
    if (vp_ctx.bt_state == VP_BT_STATE_SNIFF_EXITING) {
        app_voice_prompt_list_data_t item = {0};
        vp_queue *queue = app_voice_prompt_get_current_queue();
        app_voice_prompt_copy_item(queue, (DataType *)&item);
        app_voice_prompt_msgid_report("app_voice_prompt_remote_disconnect_impl: curr item idx %d, callback %x", 2, item.index, item.callback);
        vp_ctx.bt_state = VP_BT_STATE_SNIFF;
        if (item.callback) {
            app_voice_prompt_sync_fail(item.index, item.callback);
        } else {
            app_voice_prompt_play_next();
        }
    }
    app_vp_mutex_unlock();
}
#endif

QueueHandle_t g_app_vp_queue = NULL;

void app_ui_realtime_task(void *arg)
{
    app_vp_task_msg_t msg;

    app_voice_prompt_msgid_report("app_vp_task: task create", 0);

    g_app_vp_queue = xQueueCreate(20, sizeof(app_vp_task_msg_t));

    if (g_app_vp_queue == NULL) {
        app_voice_prompt_msgid_report("app_vp_task: app_vp_queue create fail", 0);
        return;
    }

    while (1) {
        /* Wait msg, blocking. */
        if (xQueueReceive((QueueHandle_t)g_app_vp_queue, &msg, portMAX_DELAY) == pdFALSE) {
            continue;
        }

        switch (msg.type) {
#ifdef MTK_AWS_MCE_ENABLE
#ifndef APP_VP_NOT_DISABLE_SNIFF
            case VOICE_PROMPT_EVENT_SNIFF_CHANGE: {
                /* Receive when disable sniff success. */
                app_voice_prompt_msgid_report("VOICE_PROMPT_EVENT_SNIFF_CHANGE", 0);
                app_voice_prompt_sniff_change_t *data = (app_voice_prompt_sniff_change_t *)msg.data;
                app_voice_exit_sniff_cnf(data->status, &(data->ind));
                break;
            }
            case VOICE_PROMPT_EVENT_REMOTE_DISCONNECT: {
                app_voice_prompt_msgid_report("VOICE_PROMPT_EVENT_REMOTE_DISCONNECT", 0);
                app_voice_prompt_remote_disconnect_impl();
                break;
            }
#endif
            case VOICE_PROMPT_EVENT_SYNC_PLAY: {
                /* Receive when partner recv a sync VP request from agent. */
                app_voice_prompt_msgid_report("VOICE_PROMPT_EVENT_SYNC_PLAY", 0);
                app_voice_prompt_synced_play_t *data = (app_voice_prompt_synced_play_t *)msg.data;
                app_voice_prompt_synced_play(data);
                break;
            }
            case VOICE_PROMPT_EVENT_SYNC_STOP: {
                /* Receive when partner recv a stop VP request from agent. */
                app_voice_prompt_msgid_report("VOICE_PROMPT_EVENT_SYNC_STOP", 0);
                app_voice_prompt_synced_stop_t *data = (app_voice_prompt_synced_stop_t *)msg.data;
                app_voice_prompt_synced_stop(data);
                break;
            }
#endif
#ifdef MTK_AWS_MCE_ENABLE
            case VOICE_PROMPT_EVENT_CONNECTED_VP: {
                /* Receive when agent sent a sync CONNECTED VP to partner, and need local play. */
                app_voice_prompt_connected_vp_t *data = (app_voice_prompt_connected_vp_t *)msg.data;
                app_voice_prompt_msgid_report("VOICE_PROMPT_EVENT_CONNECTED_VP, gpt 0x%08x", 1, data->gpt_count);
                app_voice_prompt_play(VP_INDEX_CONNECTED, false, 0, data->level, data->cleanup, data->callback, data->gpt_count);
                break;
            }
#endif
            case VOICE_PROMPT_EVENT_LOCAL_PLAY : {
                /* Receive when need local play. */
                app_voice_prompt_local_play_t *data = (app_voice_prompt_local_play_t *)msg.data;
                app_voice_prompt_msgid_report("VOICE_PROMPT_EVENT_LOCAL_PLAY, gpt 0x%08x", 1, data->gpt_count);
                app_vp_mutex_lock();
                g_gpt_count_play = data->gpt_count;
                app_voice_prompt_local_play(data->play_index, 0, data->callback);
                app_vp_mutex_unlock();
                break;
            }
#ifdef MTK_AWS_MCE_ENABLE
            case VOICE_PROMPT_EVENT_AWS_PLAY: {
                /* Receive when need aws play. */
                app_voice_prompt_aws_play_t *data = (app_voice_prompt_aws_play_t *)msg.data;
                app_voice_prompt_msgid_report("VOICE_PROMPT_EVENT_AWS_PLAY, index %d, delay %d", 2, data->tone_idx, data->delay_time);
                app_vp_mutex_lock();
                app_voice_prompt_aws_play_impl(data->tone_idx, data->delay_time, data->type, data->level, data->isCleanup, data->repeat, data->callback);
                app_vp_mutex_unlock();
                break;
            }
#endif
            case VOICE_PROMPT_EVENT_PLAY_START: {
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
                bt_app_common_apply_ultra_low_latency_retry_count();
#endif
                break;
            }
            default:
                app_voice_prompt_msgid_report("app_vp_task, wrong type msg", 0);
                break;
        }

        if (msg.data) {
            vPortFree(msg.data);
        }
    }
}


void app_vp_task_send_msg(app_voice_prompt_event_t type, void *data)
{
    app_vp_task_msg_t msg;

    msg.type = type;
    msg.data = data;

    if (xQueueSend((QueueHandle_t)g_app_vp_queue, &msg, 0) == pdFALSE) {
        app_voice_prompt_msgid_report("app_vp_task_send_msg, fail", 0);
    }
}


#endif /*MTK_PROMPT_SOUND_ENABLE*/

