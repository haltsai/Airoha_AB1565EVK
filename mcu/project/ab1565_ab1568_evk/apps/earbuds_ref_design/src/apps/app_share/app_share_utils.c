/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
 /* Airoha restricted information */

/**
 * File: app_share_utils.c
 *
 * Description: this file provide common functions for share_app.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#ifdef AIR_MCSYNC_SHARE_ENABLE

#include "app_share_utils.h"

#define TAG "app_share "

static app_share_context_t g_context;

app_share_context_t* app_share_get_local_context()
{
    return &g_context;
}

void app_share_enable_share_mode(struct _ui_shell_activity *self, bool is_follower)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    app_share_context_t* ctx = app_share_get_local_context();

#ifdef AIR_MULTI_POINT_ENABLE
    /* If multi point is enabled, can not enter share mode before the inactivate link disconnected. */
    uint32_t link_nums = check_and_exit_emp_mode();
    if (link_nums == 2) {
        ctx->pending_key_action = is_follower ? KEY_SHARE_MODE_FOLLOWER_SWITCH : KEY_SHARE_MODE_SWITCH;
        return;
    }
    ctx->pending_key_action = 0;
#endif

    ctx->is_follower = is_follower;
    ctx->rho_status = BT_STATUS_FAIL;
    if (is_follower) {
        ret = bt_mcsync_share_enable(BT_MCSYNC_SHARE_ROLE_FOLLOWER);
    } else {
        ret = bt_mcsync_share_enable(BT_MCSYNC_SHARE_ROLE_SHARE);
    }
    if (ret == BT_STATUS_SUCCESS) {
        apps_config_set_vp(VP_INDEX_SUCCESSED, false, 200, VOICE_PROMPT_PRIO_HIGH, false, NULL);
#if 1
        ui_shell_start_activity(self, app_share_activity, ACTIVITY_PRIORITY_HIGH, NULL, 0);
#endif
    } else {
        apps_config_set_vp(VP_INDEX_FAILED, true, 200, VOICE_PROMPT_PRIO_HIGH, false, NULL);
    }
    APPS_LOG_MSGID_I(TAG"start enable share mode, is_follower:%d, ret:%d.", 2, is_follower, ret);
}

void app_share_disable_share_mode(struct _ui_shell_activity *self)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
#if 0
    bt_mcsync_share_role_t role;
#endif

    app_share_context_t* ctx = app_share_get_local_context();

    if (ctx->rho_status != BT_STATUS_PENDING) {
        ctx->rho_status = BT_STATUS_SUCCESS;
    }

    ret = bt_mcsync_share_disable();
#if 0
    role = bt_mcsync_share_get_role();
    if (role & BT_MCSYNC_SHARE_ROLE_AGENT) {
        apps_config_set_vp(VP_INDEX_SUCCESSED, true, 200, VOICE_PROMPT_PRIO_HIGH, false, NULL);
    } else {
        apps_config_set_vp(VP_INDEX_SUCCESSED, false, 200, VOICE_PROMPT_PRIO_HIGH, false, NULL);
    }
#endif
#if 0
    ui_shell_finish_activity(self, self);
#endif
    APPS_LOG_MSGID_I(TAG"disable share mode, is_follower:%d, ret:%d.", 2, ctx->is_follower, ret);
    ctx->is_follower = false;
}


void app_share_mode_sta_update()
{
    app_share_context_t* ctx = NULL;
    bt_mcsync_share_state_t new_sta;

    new_sta = bt_mcsync_share_get_state();
    ctx = app_share_get_local_context();

    APPS_LOG_MSGID_I(TAG"sta update, current sta: %d, new sta: %d.", 2, ctx->current_sta, new_sta);
    if (ctx->current_sta == new_sta) {
        if (new_sta == BT_MCSYNC_SHARE_STATE_NORMAL) {
            ctx->rho_status = BT_STATUS_SUCCESS;
        }
        return;
    }

    ctx->current_sta = new_sta;
    /* Report state to SP. */
    race_share_mode_notify_state_change(new_sta);
    /* If the state of share mode become to normal again, check the pending RHO and restart it. */
    if (new_sta == BT_MCSYNC_SHARE_STATE_NORMAL) {
        if (ctx->rho_pending) {
            ctx->rho_pending = false;
            bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_SHARE_APP);
        }
        ctx->rho_status = BT_STATUS_SUCCESS;
    }
}


app_power_saving_target_mode_t app_share_get_power_saving_target_mode() {
    app_share_context_t* ctx = NULL;
    ctx = app_share_get_local_context();

    return ctx->target_mode;
}

extern uint8_t bt_sink_srv_get_last_music_volume();
extern bt_status_t bt_sink_srv_set_local_volume(uint8_t volume);
void app_share_send_key_code_to_agent(uint8_t key_code) {
    bt_mcsync_share_action_info_t action = {
        BT_MCSYNC_SHARE_ACTION_SYNC_KEY_CODE,
        BT_MCSYNC_SHARE_ACTION_DEST_SHARE_AGENT,
        sizeof(uint8_t),
        &key_code
    };

    APPS_LOG_MSGID_I(TAG"send key: %d to agent.", 1, key_code);
    bt_status_t ret = bt_mcsync_share_send_action(&action);
    if (ret != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_E(TAG"send key code to agent failed:%d.", 1, ret);
    }
}

#define MIN_INTERVAL_TIME 50
void app_share_handle_share_action(bt_mcsync_share_action_info_t* action) {
    static uint32_t last_action_tick = 0;
    uint32_t cur_tick = xTaskGetTickCount();
    if (cur_tick - last_action_tick < MIN_INTERVAL_TIME) {
        return;
    }
    last_action_tick = cur_tick;
    APPS_LOG_MSGID_I(TAG"handle share action: %d %d %d %d.", 4,
        action->action, action->dest, action->length, action->data[0]);
    switch(action->action) {
        case BT_MCSYNC_SHARE_ACTION_SYNC_KEY_CODE: {
            uint16_t *p_key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
            if (p_key_action == NULL) {
                APPS_LOG_MSGID_I(TAG"malloc mem failed.", 0);
                break;
            }
            uint8_t k_action = action->data[0];
            *p_key_action = (uint16_t)(0x00ff & k_action);
            APPS_LOG_MSGID_I(TAG"handle event %d %d.", 2, k_action, *p_key_action);
            ui_shell_status_t status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH,
                            EVENT_GROUP_UI_SHELL_KEY,
                            INVALID_KEY_EVENT_ID,
                            p_key_action, sizeof(uint16_t), NULL, 0);
            if (UI_SHELL_STATUS_OK != status) {
                vPortFree(p_key_action);
                APPS_LOG_MSGID_I(TAG"send partner's key event failed.", 0);
            }
        }
        break;
        case BT_MCSYNC_SHARE_ACTION_SYNC_SOUND_LEVEL: {
            bt_status_t ret = bt_sink_srv_set_local_volume(action->data[0]);
            APPS_LOG_MSGID_I(TAG"set local volume %d, ret:%d.", 2, action->data[0], ret);
        }
        break;
    }
}

void app_share_vol_change(uint8_t vol_action) {
    uint8_t current_volume;
    uint8_t max_volume;
    bt_mcsync_share_action_info_t action = {
        BT_MCSYNC_SHARE_ACTION_SYNC_SOUND_LEVEL,
        BT_MCSYNC_SHARE_ACTION_DEST_SHARE_AGENT,
        sizeof(uint8_t),
        NULL,
    };

    current_volume = bt_sink_srv_get_last_music_volume();
    if (vol_action == ACTION_VOL_UP) {
        current_volume += 1;
    } else if (vol_action == ACTION_VOL_DOWN && current_volume > 0) {
        current_volume -= 1;
    }
    max_volume = bt_sink_srv_ami_get_a2dp_max_volume_level();
    current_volume = current_volume > max_volume ? max_volume : current_volume;
    action.data = &current_volume;

    bt_mcsync_share_role_t role = bt_mcsync_share_get_role();
    if (role & BT_MCSYNC_SHARE_ROLE_FOLLOWER) {
        action.dest = BT_MCSYNC_SHARE_ACTION_DEST_FOLLOWER;
    } else {
        bt_status_t ret = bt_sink_srv_set_local_volume(current_volume);
        APPS_LOG_MSGID_I(TAG"set local volume %d, ret:%d.", 2, current_volume, ret);
        if (role & BT_MCSYNC_SHARE_ROLE_AGENT) {
            action.dest = BT_MCSYNC_SHARE_ACTION_DEST_SHARE_PARTNER;
        } else {
            action.dest = BT_MCSYNC_SHARE_ACTION_DEST_SHARE_AGENT;
        }
    }

    APPS_LOG_MSGID_E(TAG"send vol set action %d to %d.", 2, vol_action, action.dest);
    bt_status_t ret = bt_mcsync_share_send_action(&action);
    if (ret != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_E(TAG"send vol set action failed:%d.", 1, ret);
    }
}

/* The follow define is internal now. */
#define BT_CM_COMMON_TYPE_DISABLE               (0x00)
#define BT_CM_COMMON_TYPE_ENABLE                (0x01)
#define BT_CM_COMMON_TYPE_UNKNOW                (0xFF)
typedef uint8_t bt_cm_common_type_t;
extern void bt_cm_write_scan_mode(bt_cm_common_type_t discoveralbe, bt_cm_common_type_t connectable);


void disable_page_scan() {
#ifdef DISABLE_PAGE_SCAN_IN_APPLAYER
    app_share_context_t* ctx = NULL;
    ctx = app_share_get_local_context();
    APPS_LOG_MSGID_I(TAG"try to disable page scan, cur sta:%d.", 1, ctx->page_scan_off);
    if (!ctx->page_scan_off) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
        ctx->page_scan_off = true;
    }
#else
    bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
#endif
}


void enable_page_scan() {
#ifdef DISABLE_PAGE_SCAN_IN_APPLAYER
    app_share_context_t* ctx = NULL;
    ctx = app_share_get_local_context();
    APPS_LOG_MSGID_I(TAG"try to enable page scan, cur sta:%d.", 1, ctx->page_scan_off);
    if (ctx->page_scan_off) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_ENABLE, BT_CM_COMMON_TYPE_ENABLE);
        ctx->page_scan_off = false;
    }
#else
    bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_ENABLE, BT_CM_COMMON_TYPE_ENABLE);
#endif
}


#ifdef AIR_MULTI_POINT_ENABLE
extern bt_status_t bt_cm_cancel_connect(bt_bd_addr_t *addr);

uint32_t check_and_exit_emp_mode() {
    uint32_t device_count = 0;
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_bd_addr_t r_addr[2];

    APPS_LOG_MSGID_I(TAG"try to exit emp mode.", 0);

    /* disable page scan. */
    disable_page_scan();

    memset(r_addr, 0, sizeof(r_addr));
    /* Get the nums of SP link. */
    device_count = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), r_addr, 2);
    if (device_count == 0) {
        APPS_LOG_MSGID_I(TAG"cur link nums is 0.", 0);
        return 0;
    }

    ret = bt_cm_cancel_connect(NULL);
    if (ret != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_I(TAG"bt cm cancel connect failed: %d.", 1, ret);
    }

    if (device_count == 2) {
        /* If there are already two SP connected, find the inactive SP link and disconnect it. */
        bt_bd_addr_t activate_addr;
        APPS_LOG_MSGID_I(TAG"cur link nums is 2, disconnect the inactivated link.", 0);
        memset(activate_addr, 0, sizeof(activate_addr));
        bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &activate_addr, 1);

        bt_cm_connect_t param;
        param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
        /* The inactive link is the link without AWS. */
        if(memcmp(&activate_addr, &r_addr[0], sizeof(activate_addr)) == 0) {
            memcpy(&param.address, r_addr[1], sizeof(r_addr[1]));
        } else {
            memcpy(&param.address, r_addr[0], sizeof(r_addr[0]));
        }
        ret = bt_cm_disconnect(&param);
        if (ret != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_I(TAG"cur link nums is 2, disconnect the inactivated link failed.", 0);
        }
    }

    return device_count;
}


void enable_emp_mode() {
    APPS_LOG_MSGID_I(TAG"try to enable emp mode.", 0);

    /* Enable page scan, the middleware will enable it. */
    /* enable_page_scan(); */
}
#endif

#endif

