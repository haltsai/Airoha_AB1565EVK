/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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

#include "bt_sink_srv_hf.h"
#include "bt_connection_manager_internal.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_common.h"
#include "bt_sink_srv_pbapc.h"
#include "bt_sink_srv_call_pseudo_dev.h"
#include "bt_sink_srv_hsp.h"
#include "bt_sink_srv_aws_mce_call.h"
#include "bt_device_manager_internal.h"
#include "bt_sink_srv_aws_mce.h"
#include "bt_sink_srv_call_pseudo_dev_mgr.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_gap.h"
#include "bt_gap_le.h"

#define BT_SINK_SRV_ACUQA_VENDER_ID     ((uint16_t)0x0047)
#define BT_SINK_SRV_ACUQA_PRODUCT_ID    ((uint16_t)0xf000)

#ifdef MTK_BLE_GAP_SRV_ENABLE
#include "bt_gap_le_service.h"
#endif

#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_le_audio_sink.h"
#include "bt_sink_srv_le_cap_stream.h"
#endif

bool g_sink_srv_hf_pts_on;
bt_sink_srv_hf_context_t g_sink_srv_hf_context[BT_SINK_SRV_HF_LINK_NUM];
bt_sink_srv_hf_context_t *g_sink_srv_hf_missed_call_device_p;
bt_sink_srv_hf_context_t *g_sink_srv_hf_last_device_p;
bt_hfp_audio_codec_type_t g_sink_srv_hf_audio_codec = (bt_hfp_audio_codec_type_t)(BT_HFP_CODEC_TYPE_CVSD | BT_HFP_CODEC_TYPE_MSBC);
uint32_t g_sink_srv_hf_delay_for_acqua = 0;
#if defined( __BT_SINK_SRV_DEBUG_INFO__ )
const static char *g_sink_hf_event_string[] = {
    "BT_HFP_SLC_CONNECTING_IND",            /**< This event indicates when service level connection is connecting. */
    "BT_HFP_SLC_CONNECTED_IND",             /**< This event indicates when service level connection has been connected. */
    "BT_HFP_CONNECT_REQUEST_IND",           /**< This event indicates when AG want to connect with HF. */
    "BT_HFP_DISCONNECT_IND",                /**< This event indicates RFCOMM link has been disconnected. */
    "BT_HFP_AUDIO_CONNECT_IND",             /**< This event indicates the SCO has been connected. */
    "BT_HFP_AUDIO_DISCONNECT_IND",          /**< This event indicates the SCO has been disconnected. */
    "BT_HFP_BRSF_FEATURES_IND",             /**< This event indicates the AG supported features. */
    "BT_HFP_CALL_HOLD_FEATURES_IND",        /**< This event indicates the call held actions which AG supports. */
    "BT_HFP_CIEV_CALL_SETUP_IND",           /**< This event indicates the call setup status when changed. */
    "BT_HFP_CIEV_CALL_IND",                 /**< This event indicates the call status when changed. */
    "BT_HFP_CIEV_CALL_HOLD_IND",            /**< This event indicates call hold status when changed. */
    "BT_HFP_CIEV_SERVICE_IND",              /**< This event indicates service roam status when changed. */
    "BT_HFP_CIEV_SIGNAL_IND",               /**< This event indicates the signal value when changed. */
    "BT_HFP_CIEV_ROAMING_IND",              /**< This event indicates the roam status when changed. */
    "BT_HFP_CIEV_BATTCHG_IND",              /**< This event indicates the battery value when changed. */
    "BT_HFP_RING_IND",                      /**< This event indicates the ring has happened. */
    "BT_HFP_CALL_WAITING_IND",              /**< This event indicates the waiting call information when get the waiting call. */
    "BT_HFP_CALLER_ID_IND",                 /**< This event indicates the caller information when get incoming call. */
    "BT_HFP_CURRENT_CALLS_IND",             /**< This event indicates the current call information when query current calls. */
    "BT_HFP_VOICE_RECOGNITION_IND",         /**< This event indicates the voice recognition feature has changed. */
    "BT_HFP_VOLUME_SYNC_SPEAKER_GAIN_IND",  /**< This event indicates the value of remote speaker volume when changed. */
    "BT_HFP_VOLUME_SYNC_MIC_GAIN_IND",      /**< This event indicates the value of remote microphone volume when changed. */
    "BT_HFP_IN_BAND_RING_IND",              /**< This event indicates the in band ring feature has changed. */
    "BT_HFP_ACTION_CMD_CNF",
    "BT_HFP_AUDIO_STATUS_CHANGE_IND",
    "BT_HFP_CUSTOM_COMMAND_RESULT_IND"
};

const static char *g_sink_hf_call_state[] = {
    "IDLE",
    NULL,
    NULL,
    NULL,
    "INCOMING",
    "OUTGOING",
    "ACTIVE",
    "TWC_INCOMING",
    "TWC_OUTGOING",
    "HELD_ACTIVE",
    "HELD_REMAINING",
    "MULTIPARTY"
};
#endif /*defined( __BT_SINK_SRV_DEBUG_INFO__ ) */

const bt_sink_srv_hf_custom_cmd_t g_bt_sink_hf_custom_cmd_list[] =
{
    {BT_HFP_CUSTOM_CMD_TYPE_XAPL, (uint8_t *)"+XAPL"},
    {BT_HFP_CUSTOM_CMD_TYPE_APLSIRI,(uint8_t *)"+APLSIRI"}
};

static bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_context_by_device(void *device)
{
    bt_sink_srv_hf_context_t * hfp_context = NULL;
    uint8_t i = 0;
    for(i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
        if (g_sink_srv_hf_context[i].device == device) {
            hfp_context = &g_sink_srv_hf_context[i];
            break;
        }
    }
    bt_sink_srv_report_id("[CALL][HF]Get context, context[%d] = 0x%0x", 2, i, hfp_context);
    return hfp_context;
}

void bt_sink_srv_hf_reset_by_device(void* dev)
{
    bt_sink_srv_report_id("[CALL][HF]Reset by device:%x", 1, dev);
    uint8_t i = 0;
    for(i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
        if (g_sink_srv_hf_context[i].device == dev) {
            g_sink_srv_hf_context[i].is_used = false;
            bt_sink_srv_memset(&g_sink_srv_hf_context[i].link, 0, sizeof(bt_sink_srv_hf_link_context_t));
            g_sink_srv_hf_context[i].device = NULL;
            return;
        }
    }
    return;
}

void bt_sink_srv_hf_update_last_context(bt_sink_srv_hf_context_t *context, bool is_add)
{
    if (is_add) {
        g_sink_srv_hf_last_device_p = context;
    } else {
        if (g_sink_srv_hf_last_device_p == context) {
            g_sink_srv_hf_last_device_p = NULL;
            uint32_t i = 0;
            for(i = 0; i < BT_SINK_SRV_CM_MAX_DEVICE_NUMBER; i++) {
                bt_sink_srv_hf_context_t *new_context = &g_sink_srv_hf_context[i];
                if (new_context != context && bt_sink_srv_hf_check_is_connected_by_context(new_context)) {
                    g_sink_srv_hf_last_device_p = new_context;
                    break;
                }
            }
        }
    }
    bt_sink_srv_report_id("[CALL][HF]update context, is add:%d, 0x%x", 2, is_add, g_sink_srv_hf_last_device_p);
}

#ifdef MTK_BT_CM_SUPPORT
static void bt_sink_srv_hf_disconnect(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p)
{
    bt_status_t status = bt_hfp_disconnect(bt_sink_srv_hf_context_p->link.handle);

    if ((status == BT_STATUS_SUCCESS) && (bt_sink_srv_hf_context_p->link.handle != 0)) {
        if (bt_sink_srv_call_psd_is_connecting(bt_sink_srv_hf_context_p->device)) {
            bt_bd_addr_t remote_address;
            memcpy(remote_address,bt_sink_srv_hf_context_p->link.address,sizeof(bt_bd_addr_t));
            bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_LINK_DISCONNECTED, NULL);
            bt_sink_srv_cm_profile_status_notify(&remote_address, BT_SINK_SRV_PROFILE_HFP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, BT_STATUS_SUCCESS);
        } else {
            bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_DISCONNECT_LINK_REQ, NULL);
        }
    } else {
        bt_sink_srv_report_id("[CALL][HF]Disconnect hfp connection failed:0x%x", 1, status);
    }
}
#endif

static void bt_sink_srv_hf_custom_cmd_parse(
                uint8_t *at_cmd,
                uint16_t cmd_len,
                bt_sink_srv_hf_custom_cmd_result_t *at_result)
{
    uint8_t i = 0;
    uint8_t custom_cmd_num = sizeof(g_bt_sink_hf_custom_cmd_list)/sizeof(bt_sink_srv_hf_custom_cmd_t);
    bt_sink_srv_report_id("[CALL][HF]custom length:%d", 1, cmd_len);
    const bt_sink_srv_hf_custom_cmd_t *curr_cmd;
    for (i = 0; i < custom_cmd_num; i++) {
        curr_cmd = &g_bt_sink_hf_custom_cmd_list[i];
        if (0 == bt_sink_srv_memcmp(at_cmd, curr_cmd->command,(uint32_t)strlen((const char *)curr_cmd->command))){
            uint8_t curr_pos = (uint8_t)strlen((const char *)curr_cmd->command);
            while (at_cmd[curr_pos] == ' ' || at_cmd[curr_pos] == ':' || at_cmd[curr_pos] == '=') {
                curr_pos++;
            }
            at_result->cmd_type = curr_cmd->type;
            switch (at_result->cmd_type) {
                case BT_HFP_CUSTOM_CMD_TYPE_APLSIRI: {
                    bt_sink_srv_report_id("[CALL][HF]curr_pos:%d", 1, curr_pos);
                    at_result->param.aplsiri.result = bt_sink_srv_util_atoi(at_cmd + curr_pos, 1);
                }
                break;
                default: {

                }
                break;
            }
            break;
        }
    }
    return;
}

static void bt_sink_srv_hf_custom_cmd_notify(
                uint32_t handle,
                bt_sink_srv_hf_custom_cmd_result_t *at_result)
{
    bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
    bt_bd_addr_t* addr = bt_hfp_get_bd_addr_by_handle(handle);
    if (NULL != event) {
        switch (at_result->cmd_type) {
              case BT_HFP_CUSTOM_CMD_TYPE_APLSIRI: {
                bt_sink_srv_memcpy((void *)&event->siri.address, (void *)addr, sizeof(bt_bd_addr_t));
                event->siri.state = at_result->param.aplsiri.result;
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SIRI_STATE_UPDATE, event, sizeof(*event));
              }
              break;
              default: {

              }
              break;
          }
        bt_sink_srv_memory_free(event);
    }
    return;
}

void bt_sink_srv_hf_pseudo_dev_callback(
    bt_sink_srv_call_pseudo_dev_event_t event_id, void *device, void *params)
{
    bt_sink_srv_report_id("[CALL][HF]PSD_CB, event_id:0x%x, device:0x%x, params:0x%x", 3, event_id, device, params);
    bt_sink_srv_assert(device);
    switch(event_id) {
        case BT_SINK_SRV_CALL_PSD_EVENT_GET_CALL_STATE: {
            bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_device(device);
            if (hf_context) {
                bt_sink_srv_hf_call_state_t *call_state = (bt_sink_srv_hf_call_state_t *)params;
                *call_state = hf_context->link.call_state;
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_GET_SCO_STATE: {
            bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_device(device);
            if (hf_context) {
                bt_sink_srv_sco_connection_state_t *sco_state = (bt_sink_srv_sco_connection_state_t *)params;
                bt_sink_srv_report_id("[CALL][HF]PSD_CB, flag:0x%x", 1, hf_context->link.flag);
                if (((hf_context->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) != 0) &&
                    ((hf_context->link.flag & BT_SINK_SRV_HF_FLAG_DISCONNECT_HFP) == 0)) {
                    *sco_state = BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED;
                } else {
                    *sco_state = BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED;
                }
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_REJECT: {
            bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_device(device);
            if (hf_context) {
                bt_sink_srv_report_id("[CALL][HF]PSD_CB,call state:0x%x", 1, hf_context->link.call_state);
                if (hf_context->link.call_state == BT_SINK_SRV_HF_CALL_STATE_ACTIVE) {
                    bt_sink_srv_hf_hold_device(hf_context);
                } else {
                    bt_sink_srv_report_id("[CALL][HF]PSD_CB, to do more case!", 0);
                }
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_SUSPEND: {
            bt_sink_srv_call_psd_state_event_notify(device, BT_SINK_SRV_CALL_EVENT_SUSPEND_REQ, NULL);
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_ACTIVATE_SCO: {
            bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_device(device);
            hf_context->link.flag |= BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
            bt_sink_srv_hf_set_audio_status(hf_context->link.handle, BT_HFP_AUDIO_STATUS_ACTIVE);
            if ((hf_context->link.flag & BT_SINK_SRV_HF_FLAG_TWC_RINGING) &&
                (hf_context->link.call_state == BT_SINK_SRV_HF_CALL_STATE_INCOMING)) {
                hf_context->link.flag |= BT_SINK_SRV_HF_FLAG_RINGING;
                hf_context->link.flag &= ~BT_SINK_SRV_HF_FLAG_TWC_RINGING;
                bt_sink_srv_event_hf_twc_ring_ind_t twc_ring_ind = {0};
                twc_ring_ind.play_vp = false;
                bt_sink_srv_memcpy(twc_ring_ind.address, hf_context->link.address, sizeof(bt_bd_addr_t));
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_TWC_RING_IND, &twc_ring_ind, sizeof(bt_sink_srv_event_hf_twc_ring_ind_t));
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_DEACTIVATE_SCO: {
            bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_device(device);
            hf_context->link.flag &= ~BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
            bt_sink_srv_hf_set_audio_status(hf_context->link.handle, BT_HFP_AUDIO_STATUS_INACTIVE);
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_IS_HIGHLIGHT: {
            bool *is_highlight = (bool *)params;
            bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_device(device);
            bt_sink_srv_hf_context_t *highlight_device = bt_sink_srv_hf_get_highlight_device();

            if (hf_context == highlight_device) {
                *is_highlight = true;
            } else {
                *is_highlight = false;
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_IS_SCO_ACTIVATED: {
            bool *is_activated = (bool *)params;
            bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_device(device);

            if (hf_context != NULL) {
                if ((hf_context->link.flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE) != 0) {
                    *is_activated = true;
                } else {
                    *is_activated = false;
                }
            }
        }
        break;

        case BT_SINK_SRV_CALL_PSD_EVENT_DEINIT: {
            bt_sink_srv_hf_reset_by_device(device);
            bt_sink_srv_call_psd_free_device(device);
            bt_sink_srv_hf_set_hsp_flag(true);
        }
        break;

        default:
        break;
    }
}

static void bt_sink_srv_hf_send_aws_call_info(bt_bd_addr_t *remote_address, bt_sink_srv_aws_mce_call_update_info_t* call_info)
{
#ifdef __MTK_AWS_MCE_ENABLE__
    bt_sink_srv_aws_mce_call_send_call_info(remote_address, call_info);
#endif
}

static bt_sink_srv_aws_mce_call_state_t bt_sink_srv_hf_transfer_to_aws_call_state(bt_sink_srv_state_t hf_call)
{
    bt_sink_srv_aws_mce_call_state_t state = BT_SINK_SRV_AWS_MCE_CALL_STATE_IDLE;
#ifdef __MTK_AWS_MCE_ENABLE__
    state = bt_sink_srv_aws_mce_call_transfer_hf_call_state(hf_call);
#endif
    return state;
}

static bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_context_by_handle(uint32_t handle)
{
    uint8_t i = 0;
    for(i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
        if (g_sink_srv_hf_context[i].link.handle == handle) {
            return &g_sink_srv_hf_context[i];
        }
    }
    return NULL;
}

bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_context_by_flag(bt_sink_srv_hf_flag_t flag)
{
    for (uint8_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
        if ((g_sink_srv_hf_context[i].link.flag & flag) == flag) {
            return &g_sink_srv_hf_context[i];
        }
    }

    return NULL;
}

uint32_t bt_sink_srv_hf_get_connected_sco_count(void)
{
    uint32_t count = 0;

    for (uint32_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
        if ((g_sink_srv_hf_context[i].link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) != 0) {
            count++;
        }
    }

    bt_sink_srv_report_id("[CALL][HF]Get connected SCO count:%d", 1, count);
    return count;
}

bool bt_sink_srv_hf_check_is_connected(bt_bd_addr_t* addr)
{
    bool result = false;
    bt_sink_srv_hf_context_t *hf_context = bt_sink_srv_hf_get_context_by_address(addr);
    if (hf_context) {
       result = bt_sink_srv_call_psd_is_ready(hf_context->device);
    }
    bt_sink_srv_report_id("[CALL][HF]is connected:%d", 1, result);
    return result;
}

bool bt_sink_srv_hf_check_is_connected_by_context(bt_sink_srv_hf_context_t* context)
{
    bool result = false;
    if (context && context->link.handle && context->device) {
       result = bt_sink_srv_call_psd_is_ready(context->device);
    }
    bt_sink_srv_report_id("[CALL][HF] context is connected:%d", 1, result);
    return result;
}

void bt_sink_srv_hf_set_hsp_flag(bool enable)
{
    bt_sink_srv_report_id("[CALL][HF]Enable HSP:%d", 1, enable);
#if defined(MTK_BT_HSP_ENABLE) && defined(MTK_MULTI_POINT_HEADSET)
    //bt_hsp_enable_service_record(enable);
#endif
}

static void bt_sink_srv_hf_volume_change_notify(bt_bd_addr_t *address, bt_sink_srv_call_audio_volume_t local_volume)
{
    bt_sink_srv_report_id("[CALL][HF]volume:%d", 1, local_volume);
    bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));

    if (NULL != event) {
        bt_sink_srv_memcpy((void *)&event->call_volume.address, (void *)address, sizeof(bt_bd_addr_t));
        event->call_volume.current_volume = (uint8_t)local_volume;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE, event, sizeof(*event));
        bt_sink_srv_memory_free(event);
    }
    bt_sink_srv_aws_mce_call_update_info_t call_info = {0,{0}};
    call_info.mask = BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME;
    call_info.data.volume = (uint8_t)local_volume;
    bt_sink_srv_hf_send_aws_call_info(address, &call_info);
    return;
}

static void bt_sink_srv_hf_sync_speaker_gain_ind_handler(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p, uint8_t volume)
{
    bt_sink_srv_call_audio_volume_t local_volume = bt_sink_srv_call_audio_volume_bt_to_local(volume);
    bt_sink_srv_call_psd_set_speaker_volume(bt_sink_srv_hf_context_p->device, local_volume);
    bt_bd_addr_t *address_p = &bt_sink_srv_hf_context_p->link.address;
    bt_sink_srv_hf_stored_data_t stored_data;
    stored_data.speaker_volume = (uint8_t)local_volume | BT_SINK_SRV_HF_VOLUME_MASK;
    bt_sink_srv_hf_set_nvdm_data(address_p, &stored_data, sizeof(stored_data));
    bt_sink_srv_hf_volume_change_notify(address_p, local_volume);
    return;
}

bool bt_sink_srv_hf_get_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size)
{
    bool result = false;
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
    // Warnning: Due to the task stack limite, record should not increase more than 100 bytes
    bt_device_manager_db_remote_profile_info_t record = {{0}};
    if (NULL != bt_addr && NULL != data_p &&
            BT_STATUS_SUCCESS == bt_device_manager_remote_find_profile_info((*bt_addr), &record)) {
        bt_sink_srv_memcpy(data_p, record.hfp_info, size);
        result = true;
    }
#endif
    return result;
}

bool bt_sink_srv_hf_set_nvdm_data(bt_bd_addr_t *bt_addr, void *data_p, uint32_t size)
{
    bool result = false;
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
    // Warnning: Due to the task stack limite, record should not increase more than 100 bytes
    bt_device_manager_db_remote_profile_info_t record = {{0}};
    if (NULL != bt_addr && NULL != data_p) {
        bt_device_manager_remote_find_profile_info((*bt_addr), &record);
        bt_sink_srv_memcpy(record.hfp_info, data_p, size);
        result = bt_device_manager_remote_update_profile_info((*bt_addr), &record);
    }
#endif
    return result;
}


static void bt_sink_srv_hf_init_speaker_volume(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p)
{
    bt_sink_srv_hf_stored_data_t stored_data;
    bt_bd_addr_t *address_p = &bt_sink_srv_hf_context_p->link.address;

    bt_sink_srv_assert(address_p && "Get null address from sink cm!");
    bt_sink_srv_report_id("[CALL][HF]Init volume, addr:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 6,
             (*address_p)[5], (*address_p)[4], (*address_p)[3],(*address_p)[2], (*address_p)[1], (*address_p)[0]);

    if (bt_sink_srv_hf_get_nvdm_data(address_p, &stored_data, sizeof(stored_data))) {
        bt_sink_srv_report_id("[CALL][HF]Real NVDM volume is 0x%02x", 1, stored_data.speaker_volume);
        if (((stored_data.speaker_volume & ~BT_SINK_SRV_HF_VOLUME_MASK) > BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME) ||
            ((stored_data.speaker_volume & BT_SINK_SRV_HF_VOLUME_MASK) != BT_SINK_SRV_HF_VOLUME_MASK)) {
            bt_sink_srv_report_id("[CALL][HF]Get volume invalid, set default volume.", 0);
            stored_data.speaker_volume = BT_SINK_SRV_CALL_AUDIO_DEFAULT_VOLUME | BT_SINK_SRV_HF_VOLUME_MASK;
            bt_sink_srv_hf_set_nvdm_data(address_p, &stored_data, sizeof(stored_data));
        }
    } else {
        bt_sink_srv_report_id("[CALL][HF]Get volume failed, set default volume.", 0);
        stored_data.speaker_volume = BT_SINK_SRV_CALL_AUDIO_DEFAULT_VOLUME | BT_SINK_SRV_HF_VOLUME_MASK;
        bt_sink_srv_hf_set_nvdm_data(address_p, &stored_data, sizeof(stored_data));
    }

    bt_sink_srv_call_psd_init_speaker_volume(bt_sink_srv_hf_context_p->device, stored_data.speaker_volume & ~BT_SINK_SRV_HF_VOLUME_MASK);

    return;
}

bool bt_sink_srv_hf_volume_change_handler(bt_sink_srv_call_audio_volume_act_t vol_act, bool min_max)
{
    bool result = false;
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_highlight_device();

    if (bt_sink_srv_hf_context_p == NULL) {
        bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_flag(BT_SINK_SRV_HF_FLAG_SCO_CREATED | BT_SINK_SRV_HF_FLAG_SCO_ACTIVE);
    }

    if (bt_sink_srv_hf_context_p) {
        bt_sink_srv_call_audio_volume_t local_volume = bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hf_context_p->device);

        if (BT_SINK_SRV_CALL_AUDIO_VOL_ACT_UP == vol_act) {
            if (min_max) {
                local_volume = BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME;
            } else {
                if (BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME > local_volume) {
                    local_volume++;
                }
            }
        } else if (BT_SINK_SRV_CALL_AUDIO_VOL_ACT_DOWN == vol_act) {
            if (min_max) {
                local_volume = BT_SINK_SRV_CALL_AUDIO_MIN_VOLUME;
            } else {
                if (BT_SINK_SRV_CALL_AUDIO_MIN_VOLUME < local_volume) {
                    local_volume--;
                }
            }
        }

        bt_sink_srv_call_psd_set_speaker_volume(bt_sink_srv_hf_context_p->device, local_volume);

        bt_bd_addr_t *address_p = &bt_sink_srv_hf_context_p->link.address;
        bt_sink_srv_hf_volume_change_notify(address_p, local_volume);
        bt_sink_srv_hf_stored_data_t stored_data;
        stored_data.speaker_volume = (uint8_t)local_volume | BT_SINK_SRV_HF_VOLUME_MASK;
        bt_sink_srv_hf_set_nvdm_data(address_p, &stored_data, sizeof(stored_data));

        char buffer[BT_SINK_SRV_HF_CMD_LENGTH];
        snprintf((char *)buffer, BT_SINK_SRV_HF_CMD_LENGTH, "AT+VGS=%d",
                  bt_sink_srv_call_audio_volume_local_to_bt(local_volume));
        BT_SINK_SRV_HF_SYNC_SPEAKER_GAIN(bt_sink_srv_hf_context_p->link.handle, buffer);
    }
    return result;
}

bool bt_sink_srv_hf_mic_volume_change_handler(bt_sink_srv_call_audio_volume_act_t vol_act, bool min_max)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_highlight_device();

    if (bt_sink_srv_hf_context_p) {
        bt_sink_srv_call_audio_volume_t local_volume = bt_sink_srv_call_psd_get_mic_volume(bt_sink_srv_hf_context_p->device);

        if (BT_SINK_SRV_CALL_AUDIO_VOL_ACT_UP == vol_act) {
            if (min_max) {
                local_volume = BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME;
            } else {
                if (BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME > local_volume) {
                    local_volume++;
                }
            }
        } else if (BT_SINK_SRV_CALL_AUDIO_VOL_ACT_DOWN == vol_act) {
            if (min_max) {
                local_volume = BT_SINK_SRV_CALL_AUDIO_MIN_VOLUME;
            } else {
                if (BT_SINK_SRV_CALL_AUDIO_MIN_VOLUME < local_volume) {
                    local_volume--;
                }
            }
        }

        bt_sink_srv_call_psd_set_mic_volume(bt_sink_srv_hf_context_p->device, local_volume);

        char buffer[BT_SINK_SRV_HF_CMD_LENGTH] = {0};
        snprintf(buffer, BT_SINK_SRV_HF_CMD_LENGTH, "AT+VGM=%d",
                bt_sink_srv_call_audio_volume_local_to_bt(local_volume));
        BT_SINK_SRV_HF_SYNC_MIC_GAIN(bt_sink_srv_hf_context_p->link.handle, buffer);
    }

    return true;
}

bool bt_sink_srv_hf_set_speaker_volume_handler(bt_sink_srv_call_audio_volume_t volume)
{
    bt_sink_srv_hf_context_t *context = bt_sink_srv_hf_get_highlight_device();

    if ((context == NULL) || (volume > BT_SINK_SRV_CALL_AUDIO_MAX_VOLUME)) {
        return false;
    }

    bt_sink_srv_call_psd_set_speaker_volume(context->device, volume);

    bt_bd_addr_t *address = &(context->link.address);
    bt_sink_srv_hf_stored_data_t stored_data = {.speaker_volume = (uint8_t)volume | BT_SINK_SRV_HF_VOLUME_MASK};
    bt_sink_srv_hf_volume_change_notify(address, volume);
    bt_sink_srv_hf_set_nvdm_data(address, &stored_data, sizeof(stored_data));

    char buffer[BT_SINK_SRV_HF_CMD_LENGTH] = {0};
    snprintf(buffer, BT_SINK_SRV_HF_CMD_LENGTH, "AT+VGS=%d", bt_sink_srv_call_audio_volume_local_to_bt(volume));
    BT_SINK_SRV_HF_SYNC_SPEAKER_GAIN(context->link.handle, buffer);

    return true;
}

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
void bt_sink_srv_hf_set_volume_timeout_handler(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_hf_context_t *hf_context = (bt_sink_srv_hf_context_t *)data;
    bt_sink_srv_report_id("[CALL][HF]Set volume timeout, hf_context:0x%x", 1, hf_context);

    if ((hf_context != NULL) &&
        ((bt_sink_srv_call_psd_is_connecting(hf_context->device)) ||
         (bt_sink_srv_hf_check_is_connected_by_context(hf_context)))) {
        bt_sink_srv_report_id("[CALL][HF]Set volume timeout, set_volume:%d", 1, hf_context->set_volume);
        bt_sink_srv_hf_sync_speaker_gain_ind_handler(hf_context, hf_context->set_volume);
    }
}
#endif /* MTK_BT_TIMER_EXTERNAL_ENABLE */

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
void bt_sink_srv_hf_disable_sniff_timeout_handler(uint32_t timer_id, uint32_t data)
{
    bt_sink_srv_report_id("[CALL][HF]Disable sniff timeout", 0);

    if (bt_sink_srv_hf_get_connected_sco_count() == 1) {
        bt_gap_link_policy_setting_t setting = {BT_GAP_LINK_POLICY_DISABLE};
        for (uint32_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
            bt_gap_connection_handle_t gap_handle = bt_cm_get_gap_handle(g_sink_srv_hf_context[i].link.address);
            if ((gap_handle != 0) && (!(g_sink_srv_hf_context[i].link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED))) {
                bt_gap_exit_sniff_mode(gap_handle);
                if (bt_gap_write_link_policy(gap_handle, &setting) == BT_STATUS_SUCCESS) {
                    g_sink_srv_hf_context[i].link.flag |= BT_SINK_SRV_HF_FLAG_DISABLE_SNIFF;
                }
            }
        }
    }
}
#endif /* MTK_BT_TIMER_EXTERNAL_ENABLE */

void bt_sink_srv_hf_call_state_change(bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p,
                                      bt_sink_srv_hf_call_state_t previous_state,
                                      bt_sink_srv_hf_call_state_t new_state)
{
    if (NULL == bt_sink_srv_hf_context_p) {
        bt_sink_srv_report_id("[CALL][HF]hfp context is null.", 0);
        return;
    }

#ifdef __BT_SINK_SRV_DEBUG_INFO__
    uint32_t index, previous = 0, now = 0;
    for (index = 3; index < 11; index++) {
        if ((1 << index) & previous_state) {
            previous = index;
        } else if ((1 << index) & new_state) {
            now = index;
        }
    }

    bt_sink_srv_report("[CALL][HF]Call state change, handle:0x%x, prev:%s, new:%s",
                       bt_sink_srv_hf_context_p->link.handle,
                       g_sink_hf_call_state[previous],
                       g_sink_hf_call_state[now]);
#endif /* __BT_SINK_SRV_DEBUG_INFO__ */


    bt_sink_srv_hf_link_context_t *link_cntx = &bt_sink_srv_hf_context_p->link;
    if (link_cntx == NULL) {
        bt_sink_srv_report_id("[CALL][HF]hfp link context is null.", 0);
        return;
    }
    bt_bd_addr_t* address_p = bt_hfp_get_bd_addr_by_handle(link_cntx->handle);
    bt_sink_srv_aws_mce_call_update_info_t call_info;
    bt_sink_srv_memset((void *)&call_info, 0, sizeof(bt_sink_srv_aws_mce_call_update_info_t));
    bt_sink_srv_aws_mce_call_state_t aws_call_state = bt_sink_srv_hf_transfer_to_aws_call_state(new_state);
    call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_STATE |
                      BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME);
    call_info.data.call_state = aws_call_state;
    call_info.data.volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hf_context_p->device);
    if (new_state != BT_SINK_SRV_HF_CALL_STATE_INCOMING) {
        if (link_cntx->flag&BT_SINK_SRV_HF_FLAG_RINGING) {
            link_cntx->flag &= ~BT_SINK_SRV_HF_FLAG_RINGING;
        } else {
            call_info.mask |= BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND;
            call_info.data.is_ring = 0;
        }
        if (link_cntx->flag & BT_SINK_SRV_HF_FLAG_TWC_RINGING) {
            link_cntx->flag &= ~BT_SINK_SRV_HF_FLAG_TWC_RINGING;
        }
    }
    bt_sink_srv_hf_send_aws_call_info(address_p, &call_info);

    if (BT_SINK_SRV_HF_CALL_STATE_IDLE != previous_state &&
        BT_SINK_SRV_HF_CALL_STATE_IDLE == new_state) {
        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_CALL_END_IND, NULL);
#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
        if(bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_HF_SWITCH) {
            bt_sink_srv_hf_context_p->link.flag &= ~BT_SINK_SRV_HF_FLAG_HF_SWITCH;
        }
#endif
#ifdef AIR_LE_AUDIO_ENABLE
    bt_gap_link_policy_setting_t setting = {BT_GAP_LINK_POLICY_ENABLE};
    bt_gap_connection_handle_t gap_handle = bt_cm_get_gap_handle(bt_sink_srv_hf_context_p->link.address);
    bt_status_t status = bt_gap_write_link_policy(gap_handle, &setting);
    bt_sink_srv_report_id("[CALL][HF]Call state change, enable Sniff status:0x%x", 1, status);
#endif
    } else if (BT_SINK_SRV_HF_CALL_STATE_IDLE == previous_state &&
             BT_SINK_SRV_HF_CALL_STATE_IDLE != new_state) {
#ifdef AIR_LE_AUDIO_ENABLE
#ifdef AIR_LE_AUDIO_CIS_ENABLE
    bt_handle_t le_handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established();
    bt_sink_srv_report_id("[CALL][HF]Call state change, le_handle:0x%x", 1, le_handle);
    if (le_handle != BT_HANDLE_INVALID) {
        bool resume = (BLE_MCS_MEDIA_STATE_PLAYING == bt_le_audio_sink_media_get_state(le_handle, BLE_MCP_GMCS_INDEX) ? true : false);
        bt_sink_srv_cap_stream_release_autonomously(le_handle, 0xFF, resume, 0);
    }
#endif

    bt_gap_link_policy_setting_t setting = {BT_GAP_LINK_POLICY_DISABLE};
    bt_gap_connection_handle_t gap_handle = bt_cm_get_gap_handle(bt_sink_srv_hf_context_p->link.address);
    bt_status_t status = bt_gap_write_link_policy(gap_handle, &setting);
    bt_sink_srv_report_id("[CALL][HF]Call state change, disable Sniff status:0x%x", 1, status);
#endif
        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_CALL_START_IND, NULL);
    } else if (BT_SINK_SRV_HF_CALL_STATE_INCOMING == previous_state &&
                BT_SINK_SRV_HF_CALL_STATE_ACTIVE == new_state &&
                 !(link_cntx->flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED)) {
        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_STOP_RING, NULL);
    }

    if (BT_SINK_SRV_HF_CALL_STATE_INCOMING == previous_state
            && BT_SINK_SRV_HF_CALL_STATE_IDLE == new_state
            && (!(link_cntx->flag & BT_SINK_SRV_HF_FLAG_USER_REJECT))
            && bt_sink_srv_strlen((char *)link_cntx->caller.number) > 0) {
        bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));;

        g_sink_srv_hf_missed_call_device_p = bt_sink_srv_hf_context_p;
        bt_sink_srv_memcpy((void *)&event->caller_info,
                               (void *)&link_cntx->caller,
                               sizeof(bt_sink_srv_caller_information_t));
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_MISSED_CALL, event, sizeof(*event));
        bt_sink_srv_memory_free(event);
    }

    if (BT_SINK_SRV_HF_CALL_STATE_IDLE == new_state) {
        link_cntx->flag &= (~BT_SINK_SRV_HF_FLAG_USER_REJECT);
    }
}

static void bt_sink_srv_hf_get_name_callback(bt_bd_addr_t *address,
        bt_sink_srv_pbapc_record_t *record,
        void *user_data)
{
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)user_data;

    if (bt_sink_srv_strlen((char *)record->name) > 0) {
        bt_sink_srv_memcpy((void *)event->caller_info.name, (void *)record->name, BT_SINK_SRV_MAX_NAME);
    }

    bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_address(address);

    if (NULL != bt_sink_srv_hf_context_p) {
        bt_sink_srv_memcpy((void *)&bt_sink_srv_hf_context_p->link.caller,
                           (void *)&event->caller_info,
                           sizeof(bt_sink_srv_caller_information_t));
    }
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION, event, sizeof(*event));
    bt_sink_srv_memory_free(event);
}

static void bt_sink_srv_hf_sco_state_change_notify(bt_sink_srv_hf_context_t *hf_context, bt_sink_srv_sco_connection_state_t state)
{
    bt_sink_srv_report_id("[CALL][HF]sco state:0x%x", 1, state);

    /* Using stack due to heap fragment */
    bt_sink_srv_sco_state_update_t update = {{0}};
    bt_bd_addr_t *address_p = &hf_context->link.address;

    /* Notify to cm the esco state. */
#ifndef MTK_BT_CM_SUPPORT
    bt_sink_srv_cm_esco_state_notify(address_p, state);
#endif
    if (BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED == state) {
        if (hf_context->link.flag&BT_SINK_SRV_HF_FLAG_RINGING) {
            hf_context->link.flag &= ~BT_SINK_SRV_HF_FLAG_RINGING;
        }
        if (hf_context->link.flag & BT_SINK_SRV_HF_FLAG_TWC_RINGING) {
            hf_context->link.flag &= ~BT_SINK_SRV_HF_FLAG_TWC_RINGING;
        }
    }

    update.state = state;
    update.codec = bt_sink_srv_call_psd_get_codec_type(hf_context->device);
    bt_sink_srv_memcpy(&update.address, address_p, sizeof(bt_bd_addr_t));
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE, &update, sizeof(bt_sink_srv_sco_state_update_t));
}

static void bt_sink_srv_hf_ring_ind_notify(bt_sink_srv_hf_context_t *hf_context)
{
    bt_sink_srv_report_id("[CALL][HF]ring ind.", 0);
    bt_bd_addr_t *address_p = &hf_context->link.address;
    bt_sink_srv_event_param_t *event = NULL;
    event = bt_sink_srv_memory_alloc(sizeof(*event));
    bt_sink_srv_memcpy(&event->ring_ind.address, address_p, sizeof(*address_p));
    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_RING_IND, event, sizeof(*event));
    bt_sink_srv_memory_free(event);
    return;
}

#ifdef MTK_BLE_GAP_SRV_ENABLE
void bt_sink_srv_hf_le_event_callback(bt_gap_le_srv_event_t event, void *data)
{
    return;
}
#endif

bt_status_t bt_sink_srv_hf_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_bd_addr_t *address_p = NULL;
    bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;

#ifdef __BT_SINK_SRV_DEBUG_INFO__
    if (msg >= BT_HFP_SLC_CONNECTING_IND && msg <= BT_HFP_AUDIO_STATUS_CHANGE_IND) {
        bt_sink_srv_report("[CALL][HF]SDK msg:%s", g_sink_hf_event_string[msg - BT_HFP_SLC_CONNECTING_IND]);
    } else {
        bt_sink_srv_report_id("[CALL][HF]SDK msg:0x%x", 1, msg);
    }
#else
    bt_sink_srv_report_id("[CALL][HF]SDK msg:0x%x, status:0x%x", 2, msg, status);
#endif /* __BT_SINK_SRV_DEBUG_INFO__ */

    switch (msg) {
        case BT_HFP_SLC_CONNECTING_IND: {
            bt_sink_srv_report_id("[CALL][HF] Task delay %d(ms) for ACQUA test", 1, g_sink_srv_hf_delay_for_acqua);
            if (g_sink_srv_hf_delay_for_acqua > 0) {
                vTaskDelay(g_sink_srv_hf_delay_for_acqua / portTICK_PERIOD_MS);
            }
            bt_hfp_slc_connecting_ind_t *message = buffer;
            bt_sink_srv_report_id("[CALL][HF]Connecting, handle:0x%x", 1, message->handle);
            address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
            bt_sink_srv_cm_profile_status_notify(address_p, BT_SINK_SRV_PROFILE_NONE, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED, status);
        }
        break;

        case BT_HFP_CONNECT_REQUEST_IND: {
            bt_hfp_connect_request_ind_t *message = (bt_hfp_connect_request_ind_t *)buffer;
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_alloc_free_context(message->address);
            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_context_p->link.handle = message->handle;
                bt_status_t result = bt_hfp_connect_response(message->handle, true);
                if (BT_STATUS_SUCCESS == result) {
                    bt_sink_srv_hf_context_p->is_used = true;
                    bt_sink_srv_memcpy(&bt_sink_srv_hf_context_p->link.address, message->address, sizeof(bt_bd_addr_t));
                    bt_sink_srv_hf_set_hsp_flag(false);
                    bt_sink_srv_hf_context_p->device = bt_sink_srv_call_psd_alloc_device(message->address, bt_sink_srv_hf_pseudo_dev_callback);
                    bt_sink_srv_assert(bt_sink_srv_hf_context_p->device);
                    bt_sink_srv_hf_init_speaker_volume(bt_sink_srv_hf_context_p);
                    bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ_IND, NULL);
                } else {
                    bt_sink_srv_hf_context_p->link.handle = 0;
                    bt_sink_srv_report_id("[CALL][HF]Accept hfp connection failed:0x%x", 1, status);
                }
            } else {
                bt_sink_srv_report_id("[CALL][HF]Can't find the context, addr:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 6,
                                (*message->address)[5], (*message->address)[4], (*message->address)[3],
                                (*message->address)[2], (*message->address)[1], (*message->address)[0]);
            }
        }
        break;

        case BT_HFP_SLC_CONNECTED_IND: {
            bt_device_manager_db_remote_pnp_info_t pnp_info = {0};
            bt_hfp_slc_connected_ind_t *message = (bt_hfp_slc_connected_ind_t *)buffer;
            const bt_sink_feature_config_t *config_p = bt_sink_srv_get_features_config();
            address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
            bt_sink_srv_report_id("[CALL][HF]Connected, handle:0x%x", 1, message->handle);
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_update_last_context(bt_sink_srv_hf_context_p, true);

                /* Send APL Command */
                const bt_sink_srv_hf_custom_command_xapl_params_t *apple_features = bt_sink_srv_get_hfp_custom_command_xapl_params();
                bt_sink_srv_report_id("[CALL][HF]Apple specific features:0x%x", 1, apple_features->features);
                if (apple_features->features != BT_SINK_SRV_HF_CUSTOM_FEATURE_NONE) {
                    bt_sink_srv_hf_enable_apl_custom_commands(bt_sink_srv_hf_context_p->link.handle, apple_features);
                }

                /* Connect pbapc if need */
                if (BT_SINK_CONFIGURABLE_FEATURE_AUTO_CONNECT_PBAPC & config_p->features) {
                    bt_sink_srv_pbapc_connect(address_p);
                }

                /* Sync to the remote device */
                bt_sink_srv_call_audio_volume_t volume = AUD_VOL_OUT_LEVEL0;
            #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
                bt_timer_ext_t *volume_timer = bt_timer_ext_find(BT_SINK_SRV_HF_SET_VOLUME_TIMER_ID);
                if ((volume_timer != NULL) && (volume_timer->data == (uint32_t)bt_sink_srv_hf_context_p)) {
                    volume = (bt_sink_srv_call_audio_volume_t)bt_sink_srv_hf_context_p->set_volume;
                } else {
                    volume = bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hf_context_p->device);
                }
            #else
                volume = bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hf_context_p->device);
            #endif
                char cmd_buffer[BT_SINK_SRV_HF_CMD_LENGTH];
                snprintf((char *)cmd_buffer, BT_SINK_SRV_HF_CMD_LENGTH, "AT+VGS=%d", (uint8_t)volume);
                BT_SINK_SRV_HF_SYNC_SPEAKER_GAIN(bt_sink_srv_hf_context_p->link.handle, cmd_buffer);

                /* Run pesudo state machine */
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_LINK_CONNECTED, NULL);
                /* Update hfp call info to partner */
                bt_sink_srv_aws_mce_call_update_info_t call_info = {0, {0}};
                call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_STATE |
                                  BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME |
                                  BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND);
                call_info.data.call_state = bt_sink_srv_hf_transfer_to_aws_call_state(bt_sink_srv_hf_context_p->link.call_state);
                call_info.data.volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hf_context_p->device);
                call_info.data.is_ring = 0;
                bt_sink_srv_hf_send_aws_call_info(address_p, &call_info);

                /* Notify to Sink CM */
                bt_sink_srv_cm_profile_status_notify(address_p, BT_SINK_SRV_PROFILE_HFP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED, status);

                /* For ACUQA */
                if (bt_device_manager_remote_find_pnp_info(*address_p, &pnp_info) == BT_STATUS_SUCCESS) {
                    bt_sink_srv_report_id("[CALL][HF]common callback, find vender_id:0x%x product_id:0x%x", 2, pnp_info.vender_id, pnp_info.product_id);
                    if ((pnp_info.vender_id == BT_SINK_SRV_ACUQA_VENDER_ID) &&
                        (pnp_info.product_id == BT_SINK_SRV_ACUQA_PRODUCT_ID)) {
                        /* 1. Enable eSCO + Sniff mode */
                        uint8_t enable = 1;
                        bt_hci_send_vendor_cmd(0xFC10, &enable, sizeof(enable));

                        /* 2. Set Sniff parameter */
                        bt_gap_default_sniff_params_t sniff_param = {
                            .max_sniff_interval = 0x0090,
                            .min_sniff_interval = 0x0090,
                            .sniff_attempt = 0x0002,
                            .sniff_timeout = 0x0001
                        };
                        bt_gap_set_default_sniff_parameters(&sniff_param);

                        /* 3. Disable Page Scan */
                        bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);

                        /* 4. Diable Advertising */
                    #ifdef MTK_BLE_GAP_SRV_ENABLE
                        bt_gap_le_srv_disable_ble(bt_sink_srv_hf_le_event_callback);
                    #else
                        bt_hci_cmd_le_set_advertising_enable_t adv_enable = {
                            .advertising_enable = BT_HCI_DISABLE
                        };
                        bt_gap_le_set_advertising(&adv_enable, NULL, NULL, NULL);
                    #endif
                    }
                } else {
                    bt_sink_srv_report_id("[CALL][HF]common callback, find PNP info FAILED!", 0);
                }
            }
        }
        break;

        case BT_HFP_DISCONNECT_IND: {
            bt_device_manager_db_remote_pnp_info_t pnp_info = {0};
            bt_hfp_disconnect_ind_t *message = (bt_hfp_disconnect_ind_t *)buffer;
            bt_sink_srv_report_id("[CALL][HF]DISCONNECT_IND, handle:0x%x", 1, message->handle);
            address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
            /* bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle); */
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_address(address_p);

            if (bt_sink_srv_hf_context_p) {
                bt_bd_addr_t addr = {0};
                bt_sink_srv_memcpy(&addr, &bt_sink_srv_hf_context_p->link.address, sizeof(bt_bd_addr_t));
                bt_sink_srv_hf_update_last_context(bt_sink_srv_hf_context_p, false);
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_LINK_DISCONNECTED, NULL);
                /* Work around  for the case: the hfp was disconnected firstly, then esco was disconnected. */
                if (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE) {
                    bt_sink_srv_report_id("[CALL][HF]HFP was disconnected but SCO still existed!", 0);
                    bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_SCO_ACTIVE);
                    bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_SCO_CREATED);
                }

                bt_sink_srv_hf_context_p->link.call_state = BT_SINK_SRV_HF_CALL_STATE_IDLE;
                bt_sink_srv_aws_mce_call_update_info_t call_info;
                    call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_STATE |
                                      BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_SCO_STATUS |
                                      BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND);
                    call_info.data.sco_state = BT_SINK_SRV_AWS_MCE_SCO_STATE_DISCONNECTED;
                    call_info.data.is_ring = 0;
                    call_info.data.call_state = bt_sink_srv_hf_transfer_to_aws_call_state(BT_SINK_SRV_HF_CALL_STATE_IDLE);

                bt_sink_srv_hf_send_aws_call_info(&addr, &call_info);
                bt_sink_srv_cm_profile_status_notify(&addr, BT_SINK_SRV_PROFILE_HFP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, status);

                if (bt_sink_srv_hf_get_highlight_device() == bt_sink_srv_hf_context_p) {
                    bt_sink_srv_hf_mp_state_change(bt_sink_srv_hf_context_p);
                }

            #ifdef MTK_BT_HSP_ENABLE
                if (status == BT_STATUS_UNSUPPORTED) {
                    bt_sink_srv_hsp_connect(&addr);
                }
            #endif /*MTK_BT_HSP_ENABLE*/

                /* For ACUQA */
                if (bt_device_manager_remote_find_pnp_info(*address_p, &pnp_info) == BT_STATUS_SUCCESS) {
                    bt_sink_srv_report_id("[CALL][HF]common callback, found vender_id:0x%x product_id:0x%x", 2, pnp_info.vender_id, pnp_info.product_id);
                    if ((pnp_info.vender_id == BT_SINK_SRV_ACUQA_VENDER_ID) &&
                        (pnp_info.product_id == BT_SINK_SRV_ACUQA_PRODUCT_ID)) {
                        /* 1. Enable Page Scan */
                        bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
                    }
                }

            #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
                bt_timer_ext_t *vr_timer = bt_timer_ext_find(BT_SINK_SRV_HF_TRIGGER_VR_TIMER_ID);
                if (vr_timer != NULL && vr_timer->data == (uint32_t)bt_sink_srv_hf_context_p) {
                    bt_timer_ext_stop(BT_SINK_SRV_HF_TRIGGER_VR_TIMER_ID);
                }
            #endif /* MTK_BT_TIMER_EXTERNAL_ENABLE */
            }
        }
        break;

        case BT_HFP_BRSF_FEATURES_IND: {
            bt_hfp_feature_ind_t *message = (bt_hfp_feature_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_context_p->link.ag_featues = message->ag_feature;
            }
        }
        break;

        case BT_HFP_CALL_HOLD_FEATURES_IND: {
            bt_hfp_hold_feature_ind_t *message = (bt_hfp_hold_feature_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_context_p->link.ag_chld_feature = message->feature;
            }
        }
        break;

        case BT_HFP_AUDIO_CONNECT_IND: {
            bt_hfp_audio_connect_ind_t *message = (bt_hfp_audio_connect_ind_t *)buffer;

            bt_sink_srv_report_id("[CALL][HF]AUDIO_CONNECT_IND, codec:%02x", 1, message->codec);
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_call_psd_set_codec_type(bt_sink_srv_hf_context_p->device, message->codec);
                bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_SCO_CREATED;
            #ifndef MTK_BT_CM_SUPPORT
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_SCO_CONNECTED, NULL);
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATE_REQ, NULL);
            #endif
                bt_sink_srv_hf_sco_state_change_notify(bt_sink_srv_hf_context_p, BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED);
            #ifdef MTK_BT_CM_SUPPORT
                bt_role_t role = BT_ROLE_MASTER;
                bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
                bt_gap_connection_handle_t gap_handle = bt_cm_get_gap_handle(bt_sink_srv_hf_context_p->link.address);

                if (!g_sink_srv_hf_pts_on) {
                    if ((aws_role == BT_AWS_MCE_ROLE_AGENT)
                        && (bt_gap_get_role_sync(gap_handle, &role) == BT_STATUS_SUCCESS) && (role != BT_ROLE_SLAVE)) {
                        bt_sink_srv_hf_switch_audio_path();
                        bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_RECONNECT_SCO;
                    } else {
                        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_SCO_CONNECTED, NULL);
                        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATE_REQ, NULL);
                    }
                }
            #endif
            #ifdef AIR_MULTI_POINT_ENABLE
                /* Disable Page Scan when 2 SCO connected. */
                if (bt_sink_srv_hf_get_connected_sco_count() > 1) {
                    //bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_NOT_ACCESSIBLE);
                #ifdef MTK_BT_CM_SUPPORT
                    bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_DISABLE);
                #endif
                }
                if (bt_sink_srv_hf_get_connected_sco_count() == 1) {
                    bt_gap_link_policy_setting_t setting = {BT_GAP_LINK_POLICY_DISABLE};
                    for (uint32_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
                        bt_gap_connection_handle_t gap_handle = bt_cm_get_gap_handle(g_sink_srv_hf_context[i].link.address);
                        if ((gap_handle != 0) && (!(g_sink_srv_hf_context[i].link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED))) {
                            bt_gap_exit_sniff_mode(gap_handle);
                            if (bt_gap_write_link_policy(gap_handle, &setting) == BT_STATUS_SUCCESS) {
                                g_sink_srv_hf_context[i].link.flag |= BT_SINK_SRV_HF_FLAG_DISABLE_SNIFF;
                            } else {
                                bt_timer_ext_start(BT_SINK_SRV_HF_DISABLE_SNIFF_TIMER_ID, 0, 100, bt_sink_srv_hf_disable_sniff_timeout_handler);
                                break;
                            }
                        }
                    }
                }
                if (bt_sink_srv_hf_get_highlight_device() == NULL) {
                    bt_sink_srv_hf_set_highlight_device(bt_sink_srv_hf_context_p);
                }
            #endif /* AIR_MULTI_POINT_ENABLE */
            }
        }
        break;

        case BT_HFP_AUDIO_STATUS_CHANGE_IND: {
            bt_hfp_audio_status_change_ind_t *message = (bt_hfp_audio_status_change_ind_t *)buffer;

            bt_sink_srv_report_id("[CALL][HF]audio changed, handle:0x%x, status:0x%x", 2, message->handle, message->audio_status);

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                if (BT_HFP_AUDIO_STATUS_ACTIVE == message->audio_status) {
                    //send if packet to remote
                    address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
                    bt_sink_srv_aws_mce_call_update_info_t call_info;
                        call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_SCO_STATUS |
                        BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME |
                        BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND);
                        call_info.data.sco_state = BT_SINK_SRV_AWS_MCE_SCO_STATE_CONNECTED;
                        call_info.data.volume = bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hf_context_p->device);
                        call_info.data.is_ring = 0;
                     bt_sink_srv_hf_send_aws_call_info(address_p, &call_info);

                    bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device,
                        BT_SINK_SRV_CALL_EVENT_SCO_ACTIVATED, NULL);
                } else {
                    bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_SCO_DEACTIVATED, NULL);
                }
            }
        }
        break;

        case BT_HFP_AUDIO_DISCONNECT_IND: {
            bt_hfp_audio_disconnect_ind_t *message = (bt_hfp_audio_disconnect_ind_t *)buffer;
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);
            if (bt_sink_srv_hf_context_p) {
            /* Still need to sync eSCO state with Agent side AWS MCE CALL in EMP project. */
            #ifndef AIR_MULTI_POINT_ENABLE
                if (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE) {
            #endif
                    address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
                    bt_sink_srv_aws_mce_call_update_info_t call_info;
                        call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_SCO_STATUS |
                        BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME |
                        BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND);
                        call_info.data.sco_state = BT_SINK_SRV_AWS_MCE_SCO_STATE_DISCONNECTED;
                        call_info.data.volume = bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hf_context_p->device);
                        call_info.data.is_ring = 0;
                    bt_sink_srv_hf_send_aws_call_info(address_p, &call_info);
            #ifndef AIR_MULTI_POINT_ENABLE
                }
            #endif
                bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_SCO_ACTIVE);
                bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_SCO_CREATED);
                bt_sink_srv_hf_sco_state_change_notify(bt_sink_srv_hf_context_p, BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED);
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_SCO_DISCONNECTED, NULL);
                bt_sink_srv_call_psd_set_codec_type(bt_sink_srv_hf_context_p->device, BT_HFP_CODEC_TYPE_NONE);
            #ifdef MTK_BT_CM_SUPPORT
                bt_role_t role = BT_ROLE_MASTER;
                bt_gap_connection_handle_t gap_handle = bt_cm_get_gap_handle(bt_sink_srv_hf_context_p->link.address);
                if ((bt_gap_get_role_sync(gap_handle, &role) == BT_STATUS_SUCCESS) && (role == BT_ROLE_SLAVE)) {
                    if ((bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_RECONNECT_SCO) != 0) {
                        bt_sink_srv_hf_switch_audio_path();
                        bt_sink_srv_hf_context_p->link.flag &= ~BT_SINK_SRV_HF_FLAG_RECONNECT_SCO;
                    }
                }

                if ((bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_DISCONNECT_HFP) != 0) {
                    bt_sink_srv_hf_context_p->link.flag &= ~BT_SINK_SRV_HF_FLAG_DISCONNECT_HFP;
                    bt_sink_srv_hf_disconnect(bt_sink_srv_hf_context_p);
                }
            #endif
            #ifdef AIR_MULTI_POINT_ENABLE
                /* Enable Page Scan when only 1 SCO connected. */
                if (bt_sink_srv_hf_get_connected_sco_count() <= 1) {
                    //bt_gap_set_scan_mode(BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
                #ifdef MTK_BT_CM_SUPPORT
                    bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_ENABLE);
                #endif
                }
                if (bt_sink_srv_hf_get_connected_sco_count() == 0) {
                    bt_gap_link_policy_setting_t setting = {BT_GAP_LINK_POLICY_ENABLE};
                    for (uint32_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
                        bt_gap_connection_handle_t gap_handle
                            = bt_cm_get_gap_handle(g_sink_srv_hf_context[i].link.address);
                        if ((gap_handle != 0) && (g_sink_srv_hf_context[i].link.flag & BT_SINK_SRV_HF_FLAG_DISABLE_SNIFF)) {
                            g_sink_srv_hf_context[i].link.flag &= ~BT_SINK_SRV_HF_FLAG_DISABLE_SNIFF;
                            bt_gap_write_link_policy(gap_handle, &setting);
                        }
                    }
                }
                if ((bt_sink_srv_hf_context_p == bt_sink_srv_hf_get_highlight_device()) &&
                    (bt_sink_srv_hf_context_p->link.call_state == BT_SINK_SRV_HF_CALL_STATE_IDLE)) {
                    for (uint32_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
                        bt_sink_srv_hf_context_t *context = &g_sink_srv_hf_context[i];
                        if ((context->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) &&
                            (context->link.call_state != BT_SINK_SRV_HF_CALL_STATE_IDLE)) {
                            bt_sink_srv_hf_set_highlight_device(context);
                        }
                    }
                }
            #endif /* AIR_MULTI_POINT_ENABLE */
            }
        }
        break;

        case BT_HFP_CIEV_CALL_SETUP_IND: {
            bt_hfp_call_setup_ind_t *callsetup = (bt_hfp_call_setup_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(callsetup->handle);

            if (NULL != bt_sink_srv_hf_context_p) {
                bt_sink_srv_hf_handle_setup_ind(bt_sink_srv_hf_context_p, callsetup->state);
                if (BT_HFP_CIEV_CALL_SETUP_STATE_INCOMING == callsetup->state) {
                    bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_QUERY_NAME;
                }

            #ifdef MTK_BT_CM_SUPPORT
                /* IoT case: using one SCO on two call indicators, active SCO again. */
                bt_sink_srv_report_id("[CALL][HF] CALL_SETUP_IND, flag:0x%x", 1, bt_sink_srv_hf_context_p->link.flag);

                if ((callsetup->state != BT_HFP_CIEV_CALL_SETUP_STATE_NONE) &&
                    (!bt_sink_srv_call_psd_is_playing(bt_sink_srv_hf_context_p->device)) &&
                    ((bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_SCO_ACTIVE) != 0) &&
                    ((bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) != 0)) {
                    bt_sink_srv_hf_context_p->link.flag &= ~BT_SINK_SRV_HF_FLAG_SCO_ACTIVE;
                    bt_sink_srv_hf_mp_set_sco(bt_sink_srv_hf_get_highlight_device(), bt_sink_srv_hf_context_p);
                }
            #endif
            }
        }
        break;

        case BT_HFP_CIEV_CALL_IND: {
            bt_hfp_call_ind_t *call = (bt_hfp_call_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(call->handle);

            bt_sink_srv_hf_handle_call_ind((bt_sink_srv_hf_context_t *)bt_sink_srv_hf_context_p, call->state);
        }
        break;

        case BT_HFP_CIEV_CALL_HOLD_IND: {
            bt_hfp_call_hold_ind_t *callheld = (bt_hfp_call_hold_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(callheld->handle);

            bt_sink_srv_hf_handle_held_ind(bt_sink_srv_hf_context_p, callheld->state);
        }
        break;

        case BT_HFP_CURRENT_CALLS_IND: {
            bt_hfp_call_list_ind_t *message = (bt_hfp_call_list_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);

            bt_sink_srv_hf_handle_call_info_ind((bt_sink_srv_hf_context_t *)bt_sink_srv_hf_context_p, message);

            if (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_LIST) {
                bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
                address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
                if (NULL != event) {
                    bt_sink_srv_memcpy(&event->call_list.address, address_p, sizeof(bt_bd_addr_t));
                    event->call_list.index = message->index;
                    event->call_list.director = message->director;
                    event->call_list.state = message->state;
                    event->call_list.mode = message->mode;
                    event->call_list.multiple_party = message->multiple_party;
                    event->call_list.num_size = message->num_size;
                    event->call_list.final = false;
                    bt_sink_srv_memcpy(event->call_list.number, message->number, BT_SINK_SRV_MAX_PHONE_NUMBER);

                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION, event, sizeof(*event));
                    bt_sink_srv_memory_free(event);
                }
            }
        }
        break;

        case BT_HFP_IN_BAND_RING_IND: {
            bt_hfp_in_band_ring_status_ind_t *message = (bt_hfp_in_band_ring_status_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);

            if (bt_sink_srv_hf_context_p) {
                if (message->enable) {
                    bt_sink_srv_hf_context_p->link.ag_featues |= BT_HFP_AG_FEATURE_IN_BAND_RING;
                } else {
                    bt_sink_srv_hf_context_p->link.ag_featues &= (~BT_HFP_AG_FEATURE_IN_BAND_RING);
                }
            }
        }
        break;

        case BT_HFP_RING_IND: {
            bt_hfp_ring_ind_t *ringtone = (bt_hfp_ring_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(ringtone->handle);
            if (!(bt_sink_srv_hf_context_p->link.ag_featues & BT_HFP_AG_FEATURE_IN_BAND_RING)) {
                if (bt_sink_srv_hf_context_p == bt_sink_srv_hf_get_highlight_device()) {
                  #ifdef BT_SINK_ENABLE_CALL_LOCAL_RINGTONE
                    if (bt_sink_srv_hf_context_p->link.call_state <= BT_SINK_SRV_HF_CALL_STATE_INCOMING) {
                        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_RING_IND, NULL);
                        bt_bd_addr_t* address_p = bt_hfp_get_bd_addr_by_handle(ringtone->handle);
                        bt_sink_srv_aws_mce_call_update_info_t call_info;
                        call_info.mask = (BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_VOLUME |
                                          BT_SINK_SRV_AWS_MCE_CALL_IFORMATION_RING_IND);
                        call_info.data.volume = (uint8_t)bt_sink_srv_call_psd_get_speaker_volume(bt_sink_srv_hf_context_p->device);
                        call_info.data.is_ring = 1;
                        bt_sink_srv_hf_send_aws_call_info(address_p, &call_info);
                    }
                 #else/*BT_SINK_ENABLE_CALL_LOCAL_RINGTONE*/
                    /*SP IOT case: if the RING_IND comes before SLC_CONNECTED_IND, discard the RIND_IND.*/
                    if (!bt_sink_srv_hf_check_is_connected_by_context(bt_sink_srv_hf_context_p)) {
                        bt_sink_srv_report_id("[CALL][HF]Discard the RING IND.", 0);
                        break;
                    }

                    if (!(bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_RINGING)) {
                        bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_RINGING;
                        bt_sink_srv_hf_ring_ind_notify(bt_sink_srv_hf_context_p);
                    }
                 #endif/*BT_SINK_ENABLE_CALL_LOCAL_RINGTONE*/
                } else {
                    if (!(bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_TWC_RINGING)) {
                        bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_TWC_RINGING;
                        bt_sink_srv_event_hf_twc_ring_ind_t twc_ring_ind = {0};
                        twc_ring_ind.play_vp = true;
                        bt_sink_srv_memcpy(twc_ring_ind.address, bt_sink_srv_hf_context_p->link.address, sizeof(bt_bd_addr_t));
                        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_TWC_RING_IND, &twc_ring_ind, sizeof(bt_sink_srv_event_hf_twc_ring_ind_t));
                    }
                }
            }
        }
        break;

        case BT_HFP_CALLER_ID_IND: {
            bt_hfp_caller_id_ind_t *message = (bt_hfp_caller_id_ind_t *)buffer;

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);

            bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
            address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
            bt_sink_srv_memcpy(&event->caller_info.address, address_p, sizeof(bt_bd_addr_t));
            event->caller_info.num_size = message->num_size;
            event->caller_info.type = message->type;
            event->caller_info.waiting = false;
            bt_sink_srv_memcpy(event->caller_info.number, message->number, BT_SINK_SRV_MAX_PHONE_NUMBER);

            if ((bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_NAME)
                    && (bt_sink_srv_pbapc_get_name_from_remote(address_p,
                            message->number,
                            (void *)event,
                            bt_sink_srv_hf_get_name_callback) == BT_STATUS_SUCCESS)) {
                bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_QUERY_NAME);
            } else {
                bt_sink_srv_pbapc_record_t *pbapc_record =
                    (bt_sink_srv_pbapc_record_t *)bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_pbapc_record_t));

                bt_sink_srv_memset((void *)pbapc_record, 0, sizeof(bt_sink_srv_pbapc_record_t));
                bt_sink_srv_memcpy((void *)pbapc_record->number, (void *)message->number, BT_SINK_SRV_MAX_PHONE_NUMBER);

                if (BT_STATUS_SUCCESS == bt_sink_srv_pbapc_get_name_from_local(address_p, pbapc_record)) {
                    bt_sink_srv_memcpy(event->caller_info.name,
                                       pbapc_record->name,
                                       BT_SINK_SRV_MAX_NAME);
                }
                bt_sink_srv_memory_free(pbapc_record);
                bt_sink_srv_memcpy((void *)&bt_sink_srv_hf_context_p->link.caller,
                                   (void *)&event->caller_info,
                                   sizeof(bt_sink_srv_caller_information_t));
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION, event, sizeof(*event));
                bt_sink_srv_memory_free(event);
            }
        }
        break;

        case BT_HFP_CALL_WAITING_IND: {
            bt_hfp_call_waiting_ind_t *message = (bt_hfp_call_waiting_ind_t *)buffer;
            bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));

            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);
            address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
            bt_sink_srv_memcpy(&event->caller_info.address, address_p, sizeof(bt_bd_addr_t));
            event->caller_info.num_size = message->num_size;
            event->caller_info.type = message->type;
            event->caller_info.waiting = true;
            bt_sink_srv_memcpy(event->caller_info.number, message->number, BT_SINK_SRV_MAX_PHONE_NUMBER);

            if ((bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_NAME)
                    && (bt_sink_srv_pbapc_get_name_from_remote(address_p,
                            message->number,
                            (void *)event,
                            bt_sink_srv_hf_get_name_callback) == BT_STATUS_SUCCESS)) {
                bt_sink_srv_hf_context_p->link.flag &= (~BT_SINK_SRV_HF_FLAG_QUERY_NAME);
            } else {
                bt_sink_srv_pbapc_record_t *pbapc_record =
                    (bt_sink_srv_pbapc_record_t *)bt_sink_srv_memory_alloc(sizeof(bt_sink_srv_pbapc_record_t));

                bt_sink_srv_memset((void *)pbapc_record, 0, sizeof(bt_sink_srv_pbapc_record_t));
                bt_sink_srv_memcpy((void *)pbapc_record->number, (void *)message->number, BT_SINK_SRV_MAX_PHONE_NUMBER);

                if (BT_STATUS_SUCCESS == bt_sink_srv_pbapc_get_name_from_local(address_p, pbapc_record)) {
                    bt_sink_srv_memcpy(event->caller_info.name,
                                       pbapc_record->name,
                                       BT_SINK_SRV_MAX_NAME);
                }
                bt_sink_srv_memory_free(pbapc_record);
                bt_sink_srv_memcpy((void *)&bt_sink_srv_hf_context_p->link.caller,
                                   (void *)&event->caller_info,
                                   sizeof(bt_sink_srv_caller_information_t));
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION, event, sizeof(*event));
                bt_sink_srv_memory_free(event);
            }
        }
        break;

        case BT_HFP_VOLUME_SYNC_SPEAKER_GAIN_IND: {
            bt_hfp_volume_sync_speaker_gain_ind_t *message = (bt_hfp_volume_sync_speaker_gain_ind_t *)buffer;
            bt_sink_srv_report_id("[CALL][HF]volume:%d", 1, message->data);
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);
        #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
            bt_sink_srv_hf_context_p->set_volume = message->data;
            if (bt_timer_ext_find(BT_SINK_SRV_HF_SET_VOLUME_TIMER_ID) == NULL) {
                bt_timer_ext_start(
                    BT_SINK_SRV_HF_SET_VOLUME_TIMER_ID,
                    (uint32_t)bt_sink_srv_hf_context_p,
                    BT_SINK_SRV_HF_SET_VOLUME_TIMER_DUR,
                    bt_sink_srv_hf_set_volume_timeout_handler);
            }
        #else
            bt_sink_srv_hf_sync_speaker_gain_ind_handler(bt_sink_srv_hf_context_p, message->data);
        #endif /* MTK_BT_TIMER_EXTERNAL_ENABLE */
        }
        break;

        case BT_HFP_ACTION_CMD_CNF: {
            bt_hfp_action_cnf_t *message = (bt_hfp_action_cnf_t *)buffer;
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);
            if (NULL != bt_sink_srv_hf_context_p && (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_QUERY_LIST)) {
                bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
                address_p = &bt_sink_srv_hf_context_p->link.address;

                bt_sink_srv_hf_context_p->link.flag &= (~(BT_SINK_SRV_HF_FLAG_QUERY_LIST));
                if (NULL != event) {
                    bt_sink_srv_memcpy(&event->call_list.address, address_p, sizeof(bt_bd_addr_t));
                    event->call_list.final = true;

                    bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION, event, sizeof(*event));
                    bt_sink_srv_memory_free(event);
                }
            }
        }
        break;

        case BT_HFP_VOICE_RECOGNITION_IND: {
            bt_hfp_voice_recognition_ind_t *message = (bt_hfp_voice_recognition_ind_t *)buffer;
            bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));

            if (NULL != event) {
                address_p = bt_hfp_get_bd_addr_by_handle(message->handle);
                bt_sink_srv_memcpy(&event->voice_recognition.address, address_p, sizeof(bt_bd_addr_t));
                event->voice_recognition.enable = message->enable;
                bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED, event, sizeof(*event));
                bt_sink_srv_memory_free(event);
            }

        #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
            bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_handle(message->handle);
            bt_timer_ext_t *vr_timer = bt_timer_ext_find(BT_SINK_SRV_HF_TRIGGER_VR_TIMER_ID);
            if (!message->enable) {
                if (vr_timer != NULL && vr_timer->data == (uint32_t)bt_sink_srv_hf_context_p) {
                    bt_timer_ext_stop(BT_SINK_SRV_HF_TRIGGER_VR_TIMER_ID);
                }
            }
        #endif /* MTK_BT_TIMER_EXTERNAL_ENABLE */
        }
        break;

        case BT_HFP_CUSTOM_COMMAND_RESULT_IND: {
            bt_hfp_custom_command_result_ind_t *message = (bt_hfp_custom_command_result_ind_t *)buffer;
            bt_sink_srv_report("[CALL][HF]custom result:%s", message->result);

            bt_sink_srv_hf_custom_cmd_result_t at_result;
            bt_sink_srv_memset(&at_result, 0, sizeof(bt_sink_srv_hf_custom_cmd_result_t));
            bt_sink_srv_hf_custom_cmd_parse((uint8_t*)message->result, bt_sink_srv_strlen((char*)message->result), &at_result);
            bt_sink_srv_hf_custom_cmd_notify(message->handle, &at_result);

        }
        break;

        default:
        break;
    }
    return BT_STATUS_SUCCESS;
}

#ifdef MTK_BT_CM_SUPPORT
bt_status_t bt_sink_srv_hf_gap_callback(bt_msg_type_t msg, bt_status_t status, void *parameter)
{
    if (msg == BT_GAP_ROLE_CHANGED_IND) {
        bt_sink_srv_hf_context_t *context = bt_sink_srv_hf_get_highlight_device();
        bt_sink_srv_report_id("[CALL][HF][GAP] ROLE_CHANGED_IND, context:0x%x", 1, context);

        if (context != NULL) {
            bt_sink_srv_report_id("[CALL][HF][GAP] ROLE_CHANGED_IND, flag:0x%x", 1, context->link.flag);

            if (((context->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) == 0) &&
                ((context->link.flag & BT_SINK_SRV_HF_FLAG_RECONNECT_SCO) != 0)) {
                bt_sink_srv_hf_switch_audio_path();
                context->link.flag &= ~BT_SINK_SRV_HF_FLAG_RECONNECT_SCO;
            }
        }
    }

    return BT_STATUS_SUCCESS;
}
#endif

bt_status_t bt_sink_srv_hf_action_handler(bt_sink_srv_action_t action, void *parameters)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    //bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = NULL;

    //bt_sink_srv_report_id("[CALL][HF]Action:%x", 1, action);

    switch (action) {
#ifndef MTK_BT_CM_SUPPORT
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_memset(g_sink_srv_hf_context, 0, sizeof(g_sink_srv_hf_context));
            bt_sink_srv_hf_reset_highlight_device();
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            bt_sink_srv_memset(g_sink_srv_hf_context, 0, sizeof(g_sink_srv_hf_context));
            bt_sink_srv_hf_reset_highlight_device();
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)parameters;
            if (action_param->profile_connection_mask & BT_SINK_SRV_PROFILE_HFP) {
                //bt_sink_srv_hf_context_p = bt_sink_srv_cm_get_profile_info(&action_param->address, BT_SINK_SRV_PROFILE_HFP);
                bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = bt_sink_srv_hf_alloc_free_context(&action_param->address);
                if (NULL != bt_sink_srv_hf_context_p) {
                    uint32_t handle = 0;
                    bt_status_t status = bt_hfp_connect(&handle, &action_param->address);
                    if (status == BT_STATUS_SUCCESS) {
                        bt_sink_srv_hf_context_p->is_used = true;
                        bt_sink_srv_hf_context_p->link.handle = handle;
                        bt_sink_srv_memcpy(&bt_sink_srv_hf_context_p->link.address, &action_param->address, sizeof(bt_bd_addr_t));
                        bt_sink_srv_hf_set_hsp_flag(false);
                        bt_sink_srv_hf_context_p->device = bt_sink_srv_call_psd_alloc_device(&action_param->address, bt_sink_srv_hf_pseudo_dev_callback);
                        bt_sink_srv_assert(bt_sink_srv_hf_context_p->device);
                        bt_sink_srv_hf_init_speaker_volume(bt_sink_srv_hf_context_p);
                        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ, NULL);
                    } else {
                        bt_sink_srv_report_id("[CALL][HF]Connect hfp connection failed:0x%x", 1, status);
                        bt_sink_srv_cm_profile_status_notify(&action_param->address, BT_SINK_SRV_PROFILE_HFP, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, status);
                    }
                }
            }
        }
        break;

        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)parameters;
            if (action_param->profile_connection_mask & BT_SINK_SRV_PROFILE_HFP) {
                //bt_sink_srv_hf_context_p = bt_sink_srv_cm_get_profile_info(&action_param->address, BT_SINK_SRV_PROFILE_HFP);
                bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_address(&action_param->address);
                if (bt_sink_srv_hf_context_p && bt_sink_srv_hf_context_p->link.handle) {
                    if (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) {
                        bt_hfp_audio_transfer(bt_sink_srv_hf_context_p->link.handle, BT_HFP_AUDIO_TO_AG);
                    }
                    bt_status_t status = bt_hfp_disconnect(bt_sink_srv_hf_context_p->link.handle);
                    if (status == BT_STATUS_SUCCESS) {
                        bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_DISCONNECT_LINK_REQ, NULL);
                    } else {
                        bt_sink_srv_report_id("[CALL][HF]Disconnect hfp connection failed:0x%x", 1, status);
                    }
                }
            }
        }
        break;
#endif

        /* User Event */
        case BT_SINK_SRV_ACTION_ANSWER: {
            bt_sink_srv_hf_answer_call(true);
        }
        break;

        case BT_SINK_SRV_ACTION_REJECT: {
            bt_sink_srv_hf_answer_call(false);
        }
        break;

        case BT_SINK_SRV_ACTION_HANG_UP: {
            bt_sink_srv_hf_terminate_call();
        }
        break;

        case BT_SINK_SRV_ACTION_DIAL_NUMBER: {
            bt_sink_srv_hf_dial_number((char *)parameters);
        }
        break;

        case BT_SINK_SRV_ACTION_DIAL_LAST: {
            bt_sink_srv_hf_dial_last();
        }
        break;

        case BT_SINK_SRV_ACTION_DIAL_MISSED: {
            bt_sink_srv_hf_dial_missed();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD: {
            bt_sink_srv_hf_release_all_held_call();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER: {
            bt_sink_srv_hf_release_all_active_accept_others();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER: {
            bt_sink_srv_hf_hold_all_active_accept_others();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION: {
            bt_sink_srv_hf_add_held_to_conversation();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER: {
            bt_sink_srv_hf_explicit_call_transfer();
        }
        break;

        case BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE: {
            bt_sink_srv_hf_voice_recognition_activate(*((bool *)parameters));
        }
        break;

        case BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH: {
            bt_sink_srv_hf_switch_audio_path();
        }
        break;

        case BT_SINK_SRV_ACTION_SWITCH_AUDIO_DEVICE: {
            bt_sink_srv_hf_switch_audio_device();
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL: {
            bt_sink_srv_hf_release_special(*((uint8_t *)parameters));
        }
        break;

        case BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL: {
            bt_sink_srv_hf_hold_special(*((uint8_t *)parameters));
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_VOLUME_UP: {
            if (bt_sink_srv_hf_volume_change_handler(BT_SINK_SRV_CALL_AUDIO_VOL_ACT_UP, false)) {
            }
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN: {
            if (bt_sink_srv_hf_volume_change_handler(BT_SINK_SRV_CALL_AUDIO_VOL_ACT_DOWN, false)) {
            }
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_VOLUME_MAX: {
            if (bt_sink_srv_hf_volume_change_handler(BT_SINK_SRV_CALL_AUDIO_VOL_ACT_UP, true)) {
            }
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_VOLUME_MIN: {
            if (bt_sink_srv_hf_volume_change_handler(BT_SINK_SRV_CALL_AUDIO_VOL_ACT_DOWN, true)) {
            }
        }
        break;

        case BT_SINK_SRV_ACTION_CALL_SET_VOLUME: {
            bt_sink_srv_hf_set_speaker_volume_handler(*((uint8_t *)parameters));
        }
        break;

        case BT_SINK_SRV_ACTION_QUERY_CALL_LIST: {
            bt_sink_srv_hf_query_call_list((bt_bd_addr_t *)parameters);
        }
        break;

        case BT_SINK_SRV_ACTION_DTMF: {
            bt_sink_srv_hf_send_dtmf((bt_sink_srv_send_dtmf_t *)parameters);
        }
        break;

        case BT_SINK_SRV_ACTION_REPORT_BATTERY: {
            const bt_sink_srv_hf_custom_command_xapl_params_t *params = bt_sink_srv_get_hfp_custom_command_xapl_params();
            if (params->features & BT_SINK_SRV_HF_CUSTOM_FEATURE_BATTERY_REPORT) {
                if(parameters) {
                    bt_sink_srv_hf_apl_report_battery(*((uint8_t *)parameters));
                } else {
                    bt_sink_srv_report_id("[CALL][HF]Null battery paramters!", 0);
                }
            } else {
                bt_sink_srv_report_id("[CALL][HF]Not support battery report!", 0);
            }
        }
        break;

        case BT_SINK_SRV_ACTION_HF_GET_SIRI_STATE: {
            bt_sink_srv_hf_apl_siri();
        }
        break;

        case BT_SINK_SRV_ACTION_HF_ECNR_ACTIVATE: {
            bt_sink_srv_hf_ncer_activate(*((bool *)parameters));
        }
        break;

        default:
            bt_sink_srv_report_id("[CALL][HF]Unexcepted action:0x%x", 1, action);
        break;
    }
    return result;
}

bt_status_t bt_sink_srv_hf_get_init_params(bt_hfp_init_param_t *param)
{
    // for low power test, add cmd to modify hf audio codec
    param->supported_codecs = g_sink_srv_hf_audio_codec;
    param->indicators.service = BT_HFP_INDICATOR_OFF;
    param->indicators.signal = BT_HFP_INDICATOR_OFF;
    param->indicators.roaming = BT_HFP_INDICATOR_OFF;
    param->indicators.battery = BT_HFP_INDICATOR_OFF;
    param->support_features = (bt_hfp_init_support_feature_t)(BT_HFP_INIT_SUPPORT_3_WAY | BT_HFP_INIT_SUPPORT_CODEC_NEG);

    param->disable_nrec = true;
    param->enable_call_waiting = true;
    param->enable_cli = true;
    return BT_STATUS_SUCCESS;
}

void* bt_sink_srv_cm_get_hf_info(uint8_t device_idx)
{
    return &g_sink_srv_hf_context[device_idx];
}

bt_sink_srv_hf_context_t *bt_sink_srv_hf_alloc_free_context(bt_bd_addr_t *address)
{
    bt_sink_srv_hf_context_t *context = bt_sink_srv_hf_get_context_by_address(address);

    if (context == NULL) {
        for (uint8_t i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
            if (!g_sink_srv_hf_context[i].is_used) {
                context = &g_sink_srv_hf_context[i];
                break;
            }
        }
    }

    bt_sink_srv_report_id("[CALL][HF] alloc free context: 0x%x", 1, context);
    return context;
}

bt_sink_srv_hf_context_t *bt_sink_srv_hf_get_context_by_address(bt_bd_addr_t *address)
{
    if (address == NULL) {
        return NULL;
    }

    for (uint32_t idx = 0; idx < BT_SINK_SRV_HF_LINK_NUM; idx++) {
        if (g_sink_srv_hf_context[idx].is_used &&
            (!bt_sink_srv_memcmp(&g_sink_srv_hf_context[idx].link.address, address, sizeof(bt_bd_addr_t)))){
            return &g_sink_srv_hf_context[idx];
        }
    }

    bt_sink_srv_report_id("[CALL][HF] not found HF context", 0);
    return NULL;
}

uint32_t bt_sink_hf_get_connected_device_list(bt_bd_addr_t *addr_list)
{
    uint8_t idx = 0;
    uint32_t count = 0;
    bt_sink_srv_assert(addr_list);
    for (; idx < BT_SINK_SRV_HF_LINK_NUM; idx++) {
        if ((g_sink_srv_hf_context[idx].is_used) && (bt_sink_srv_call_psd_is_ready(g_sink_srv_hf_context[idx].device))) {
            bt_sink_srv_memcpy(&addr_list[count], &g_sink_srv_hf_context[idx].link.address, sizeof(bt_bd_addr_t));
            count++;
        }
    }
    bt_sink_srv_report_id("[CALL][HF] current connected hf device = %d", 1, count);
    return count;
}

bt_status_t bt_sink_srv_hf_get_speaker_volume(bt_bd_addr_t *address, uint32_t *volume)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (role)
    {
        case BT_AWS_MCE_ROLE_NONE:
        case BT_AWS_MCE_ROLE_AGENT:
        {
            bt_sink_srv_hf_context_t *context
                = bt_sink_srv_hf_get_context_by_flag(BT_SINK_SRV_HF_FLAG_SCO_CREATED | BT_SINK_SRV_HF_FLAG_SCO_ACTIVE);

            if (context != NULL) {
                *volume = bt_sink_srv_call_psd_get_speaker_volume(context->device);
                return BT_STATUS_SUCCESS;
            }

            break;
        }

        case BT_AWS_MCE_ROLE_PARTNER:
        {
        #ifdef MTK_AWS_MCE_ENABLE
            bt_sink_srv_aws_mce_call_context_t *context
                = bt_sink_srv_aws_mce_call_get_context_by_sco_state(BT_SINK_SRV_AWS_MCE_SCO_STATE_CONNECTED);

            if (context != NULL) {
                *volume = bt_sink_srv_aws_mce_call_get_speaker_volume(context);
                return BT_STATUS_SUCCESS;
            }
        #endif
        }

        default:
        {
            break;
        }
    }

    return BT_STATUS_FAIL;
}

#ifdef MTK_BT_CM_SUPPORT
bt_status_t  bt_sink_srv_hf_cm_callback_handler(bt_cm_profile_service_handle_t type, void *data)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    uint8_t *address = NULL;
    bt_sink_srv_report_id("[CALL][HF] cm_callback_handler type:0x%02x", 1, type);
    switch (type) {
    case BT_CM_PROFILE_SERVICE_HANDLE_POWER_ON:
        //bt_sink_srv_assert(data);
        //uint32_t count = *(uint32_t *)data;
        bt_sink_srv_call_psd_init();
        bt_sink_srv_call_audio_init();
        bt_sink_srv_memset(g_sink_srv_hf_context, 0, sizeof(g_sink_srv_hf_context));
        bt_sink_srv_hf_reset_highlight_device();
    #ifdef MTK_AWS_MCE_ENABLE
        bt_sink_srv_aws_mce_init();
    #endif
        bt_sink_srv_aws_mce_call_action_handler(BT_SINK_SRV_ACTION_PROFILE_INIT, NULL);
    #if defined(MTK_BT_SPEAKER_ENABLE) && defined(BT_SINK_HFP_CONNECTION_IN_MULTILINK)
        bt_cm_register_event_callback(bt_sink_srv_hf_cm_event_callback);
    #endif
        break;
    case BT_CM_PROFILE_SERVICE_HANDLE_POWER_OFF:
        bt_sink_srv_call_psd_deinit();
        bt_sink_srv_call_audio_deinit();
        bt_sink_srv_memset(g_sink_srv_hf_context, 0, sizeof(g_sink_srv_hf_context));
        bt_sink_srv_hf_reset_highlight_device();
    #ifdef MTK_AWS_MCE_ENABLE
        bt_sink_srv_aws_mce_deinit();
    #endif
        bt_sink_srv_aws_mce_call_action_handler(BT_SINK_SRV_ACTION_PROFILE_DEINIT, NULL);
        break;
    case BT_CM_PROFILE_SERVICE_HANDLE_CONNECT: {
        bt_sink_srv_mutex_lock();
        address = (uint8_t *)data;
        bt_sink_srv_assert(address);
        bt_sink_srv_report_id("[CALL][HF] connect HFP addr :0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", 6, address[0],address[1],address[2],
            address[3],address[4],address[5]);
        bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = bt_sink_srv_hf_alloc_free_context((bt_bd_addr_t *)address);
        if (NULL != bt_sink_srv_hf_context_p) {
            uint32_t handle = 0;
            status = bt_hfp_connect(&handle, (bt_bd_addr_t *)address);
            if (status == BT_STATUS_SUCCESS) {
                bt_sink_srv_hf_context_p->is_used = true;
                bt_sink_srv_hf_context_p->link.handle = handle;
                bt_sink_srv_memcpy(&bt_sink_srv_hf_context_p->link.address, address, sizeof(bt_bd_addr_t));
                bt_sink_srv_hf_set_hsp_flag(false);
                bt_sink_srv_hf_context_p->device = bt_sink_srv_call_psd_alloc_device((bt_bd_addr_t *)address, bt_sink_srv_hf_pseudo_dev_callback);
                bt_sink_srv_assert(bt_sink_srv_hf_context_p->device);
                bt_sink_srv_hf_init_speaker_volume(bt_sink_srv_hf_context_p);
                bt_sink_srv_call_psd_state_event_notify(bt_sink_srv_hf_context_p->device, BT_SINK_SRV_CALL_EVENT_CONNECT_LINK_REQ, NULL);
            } else {
                bt_sink_srv_report_id("[CALL][HF]Connect hfp connection failed:0x%x", 1, status);
            }
        }
        bt_sink_srv_mutex_unlock();
        break;
    }
    case BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT: {
        bt_sink_srv_mutex_lock();
        address = (uint8_t *)data;
        bt_sink_srv_assert(address);
        bt_sink_srv_report_id("[CALL][HF] disconnect HFP addr :0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", 6, address[0],address[1],address[2],
            address[3],address[4],address[5]);
        bt_sink_srv_hf_context_t *bt_sink_srv_hf_context_p = bt_sink_srv_hf_get_context_by_address((bt_bd_addr_t *)address);
        if (bt_sink_srv_hf_context_p && bt_sink_srv_hf_context_p->link.handle) {
            if (bt_sink_srv_hf_context_p->link.flag & BT_SINK_SRV_HF_FLAG_SCO_CREATED) {
                bt_sink_srv_hf_context_p->link.flag |= BT_SINK_SRV_HF_FLAG_DISCONNECT_HFP;
                bt_hfp_audio_transfer(bt_sink_srv_hf_context_p->link.handle, BT_HFP_AUDIO_TO_AG);
            } else {
                bt_sink_srv_hf_disconnect(bt_sink_srv_hf_context_p);
            }
        }
        bt_sink_srv_mutex_unlock();
        break;
    }
    default:
        break;
    }
    return status;
}

#if defined(MTK_BT_SPEAKER_ENABLE) && defined(BT_SINK_HFP_CONNECTION_IN_MULTILINK)
bt_status_t bt_sink_srv_hf_cm_event_callback(bt_cm_event_t event_id, void *params, uint32_t params_len)
{
    switch (event_id)
    {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *update_ind = (bt_cm_remote_info_update_ind_t *)params;

            if ((params == NULL) || (params_len != sizeof(bt_cm_remote_info_update_ind_t))) {
                return BT_STATUS_FAIL;
            }

            if (((update_ind->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)) != 0) &&
                ((update_ind->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)) == 0)) {
                bt_sink_srv_report_id("[CALL][HF][CM]event_callback, HFP connected.", 0);
                bt_hfp_enable_service_record(false);
                bt_hsp_enable_service_record(false);
                bt_bd_addr_t connected_address[BT_SINK_SRV_HF_LINK_NUM] = {{0}};
                uint32_t connected_number = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK), connected_address, BT_SINK_SRV_HF_LINK_NUM);
                bt_sink_srv_report_id("[CALL][HF][CM]event_callback, Connected number:%x", 1, connected_number);
                if (connected_number > 1) {
                    for (uint32_t i = 0; i < connected_number; i++) {
                        bt_sink_srv_hf_cm_callback_handler(BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT, &connected_address[i]);
                    }
                }
            } else if (((update_ind->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)) != 0) &&
                ((update_ind->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)) == 0)) {
                bt_bd_addr_t connected_address[BT_SINK_SRV_HF_LINK_NUM] = {{0}};
                uint32_t connected_number = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK), connected_address, BT_SINK_SRV_HF_LINK_NUM);
                bt_sink_srv_report_id("[CALL][HF][CM]event_callback, A2DP connected. Connected number:%x", 1, connected_number);
                if (connected_number > 1) {
                    for (uint32_t i = 0; i < connected_number; i++) {
                        bt_sink_srv_hf_cm_callback_handler(BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT, &connected_address[i]);
                    }
                }
            } else if (((update_ind->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)) == 0) &&
                ((update_ind->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)) != 0)) {
                bt_bd_addr_t connected_address[BT_SINK_SRV_HF_LINK_NUM] = {{0}};
                uint32_t connected_number = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK), connected_address, BT_SINK_SRV_HF_LINK_NUM);
                bt_sink_srv_report_id("[CALL][HF][CM]event_callback, A2DP disconnected. Connected number:%d", 1, connected_number);
                if (connected_number == 0) {
                    bt_hfp_enable_service_record(true);
                    bt_hsp_enable_service_record(false);
                } else if ((connected_number == 1) && (bt_sink_srv_hf_get_context_by_address(&connected_address[0]) == NULL)) {
                    bt_hfp_enable_service_record(true);
                    bt_hsp_enable_service_record(false);
                    bt_sink_srv_hf_cm_callback_handler(BT_CM_PROFILE_SERVICE_HANDLE_CONNECT, &connected_address[0]);
                }
            }
        }
        break;

        default: {
        }
        break;
    }

    return BT_STATUS_SUCCESS;
}
#endif /* MTK_BT_SPEAKER_ENABLE */
#endif

#ifdef AIR_FEATURE_SINK_AUDIO_SWITCH_SUPPORT
extern uint8_t bt_avm_esco_stream_control(bool value);
bt_status_t bt_sink_srv_hf_switch(bool value)
{
    bt_sink_srv_hf_context_t *hfp_context = NULL;
    uint8_t avm_status = 0;
    uint8_t i = 0;
    if(value) {
        hfp_context = bt_sink_srv_hf_get_context_by_flag(BT_SINK_SRV_HF_FLAG_HF_SWITCH);
        if(hfp_context == NULL) {
            bt_sink_srv_report_id("[CALL][HF] there is no stop HF in front", 0);
            return BT_STATUS_FAIL;
        }
        hfp_context->link.flag &= ~BT_SINK_SRV_HF_FLAG_HF_SWITCH;
        if(!bt_sink_srv_call_psd_is_playing_idle(hfp_context->device)) {
            bt_sink_srv_report_id("[CALL][HF] hfp_context is not palying idle", 0);
            return BT_STATUS_FAIL;
        }
        bt_sink_srv_call_psd_state_event_notify(hfp_context->device,BT_SINK_SRV_CALL_EVENT_HF_SWITCH_START,NULL);
    } else {
        for(i = 0; i < BT_SINK_SRV_HF_LINK_NUM; i++) {
            if (bt_sink_srv_call_psd_is_playing(g_sink_srv_hf_context[i].device)) {
                hfp_context = &g_sink_srv_hf_context[i];
                bt_sink_srv_report_id("[CALL][HF] there is palying device = %02x",1,hfp_context->device);
                break;
            }
        }
        if(hfp_context == NULL) {
            bt_sink_srv_report_id("[CALL][HF] there is no palying device", 0);
            return BT_STATUS_FAIL;
        }
        if(!(hfp_context->link.flag & BT_SINK_SRV_HF_FLAG_HF_SWITCH)) {
            /* Codec stop,sub->state:palying_idle. */
            hfp_context->link.flag |= BT_SINK_SRV_HF_FLAG_HF_SWITCH;
            bt_sink_srv_call_psd_state_event_notify(hfp_context->device,BT_SINK_SRV_CALL_EVENT_HF_SWITCH_STOP,NULL);
        } else {
            bt_sink_srv_report_id("[CALL][HF] already stop", 0);
            return BT_STATUS_FAIL;
        }
    }
    avm_status = bt_avm_esco_stream_control(value);
    bt_sink_srv_report_id("[CALL][HF] avm esco stream control status = %d", 1,avm_status);
#ifdef __MTK_AWS_MCE_ENABLE__
    bt_sink_srv_aws_mce_call_sync_hf_switch(value);
#endif
    return BT_STATUS_SUCCESS;
}
#endif
