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
 * File: app_share_activity.c
 *
 * Description:
 * This file is the activity to handle the key action or charger case event after earbuds enter share mode.
 * When enter share mode, the app_share_idle_activity will create this activity.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#ifdef AIR_MCSYNC_SHARE_ENABLE

#include "app_share_utils.h"


#define TAG "app_share_activity "
#define get_local_context app_share_get_local_context


static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    app_share_context_t* ctx = get_local_context();
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            ctx->target_mode = APP_POWER_SAVING_TARGET_MODE_NORMAL;
#ifdef DISABLE_PAGE_SCAN_IN_APPLAYER
            /* Try to disable page scan */
            disable_page_scan();
#endif
            /* Set the led display and pause ADV on follower side. */
            if (!ctx->is_follower) {
                apps_config_set_background_led_pattern(LED_INDEX_INCOMING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOW);
            } else {
                APPS_LOG_MSGID_I(TAG"pause ble adv, current: %d", 1, ctx->ble_adv_paused);
                if (!ctx->ble_adv_paused) {
                    multi_ble_adv_manager_pause_ble_adv();
                    ctx->ble_adv_paused = true;
                }
                apps_config_set_background_led_pattern(LED_INDEX_INCOMING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            }
            APPS_LOG_MSGID_I(TAG"create", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(TAG"destory", 0);
            /* Reset share pairing status */
            app_share_context_t* ctx = get_local_context();
            ctx->share_pairing_result = false;
            APPS_LOG_MSGID_I(TAG"reset share pairing result %x", 1, ctx->share_pairing_result);
            ctx->target_mode = APPS_POWER_SAVING_MODE;
#ifdef AIR_MULTI_POINT_ENABLE
            /* Enable share mode after share mode exited. */
            enable_emp_mode();
#endif
#ifdef DISABLE_PAGE_SCAN_IN_APPLAYER
            /* Try to enable page scan */
            enable_page_scan();
#endif
            /* Notify that the power saving mode changed and update the led display to normal. */
            app_power_save_utils_notify_mode_changed(false, app_share_get_power_saving_target_mode);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0, NULL, 0);
            apps_config_set_vp(VP_INDEX_SUCCESSED, false, 200, VOICE_PROMPT_PRIO_HIGH, false, NULL);
            if (ctx->ble_adv_paused) {
                multi_ble_adv_manager_resume_ble_adv();
                ctx->ble_adv_paused = false;
            }
            if (self->local_context) {
                vPortFree(self->local_context);
            }
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(TAG"resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(TAG"pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(TAG"refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(TAG"result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}


static bool _proc_key_event_group(
        struct _ui_shell_activity *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;
    apps_config_key_action_t action;
    uint8_t key_id;
    airo_key_event_t key_event;
    app_event_key_event_decode(&key_id, &key_event, event_id);

    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }

    switch(action) {
        case KEY_SHARE_MODE_SWITCH: {
            APPS_LOG_MSGID_I(TAG"share mode switch trigger by key.", 0);
            app_share_disable_share_mode(self);
            ret = true;
        }
        break;

        case KEY_SHARE_MODE_FOLLOWER_SWITCH: {
            APPS_LOG_MSGID_I(TAG"share mode follower switch trigger by key.", 0);
            app_share_disable_share_mode(self);
            ret = true;
        }
        break;

        case KEY_AVRCP_BACKWARD:
        case KEY_AVRCP_FORWARD:
        case KEY_AVRCP_FAST_FORWARD_PRESS:
        case KEY_AVRCP_FAST_FORWARD_RELEASE:
        case KEY_AVRCP_FAST_REWIND_PRESS:
        case KEY_AVRCP_FAST_REWIND_RELEASE:
        case KEY_AVRCP_PLAY:
        case KEY_AVRCP_PAUSE:
        case KEY_ACCEPT_CALL:
        case KEY_REJCALL:
        case KEY_ONHOLD_CALL:
        case KEY_END_CALL:
        case KEY_REJCALL_SECOND_PHONE:
        case KEY_SWITCH_AUDIO_PATH:
        case KEY_3WAY_HOLD_ACTIVE_ACCEPT_OTHER: {
            bt_mcsync_share_role_t role = bt_mcsync_share_get_role();
            APPS_LOG_MSGID_I(TAG"recevice key: %d, role:%d.", 2, action, role);
            if ((role & BT_MCSYNC_SHARE_ROLE_SHARE) && (role & BT_MCSYNC_SHARE_ROLE_PARTNER)) {
                /* Notice: only 1 byte could be sent to agent, so the action value must not be bigger than 0xff */
                app_share_send_key_code_to_agent((uint8_t)(0xff & action));
                ret = true;
            }
        }
        break;
#if 1
        case KEY_VOICE_UP:
            app_share_vol_change(ACTION_VOL_UP);
            ret = true;
            break;
        case KEY_VOICE_DN:
            app_share_vol_change(ACTION_VOL_DOWN);
            ret = true;
            break;
#endif
        default: {
            bt_mcsync_share_role_t role;
            role = bt_mcsync_share_get_role();
            /* The common key action is not allowed on follower side. */
            if ((role & BT_MCSYNC_SHARE_ROLE_FOLLOWER) && (action != KEY_POWER_OFF)) {
                ret = true;
            } else {
                ret = false;
            }
        }
        break;
    }

    return ret;
}


static bool _proc_apps_internal_events(
        struct _ui_shell_activity *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;
    app_share_context_t* ctx = get_local_context();

    switch (event_id) {
        case APPS_EVENTS_INTERACTION_SHARE_MODE_STA_UPDATE: {
            app_share_mode_sta_t old_sta = ctx->current_sta;
            app_share_mode_sta_update();
            app_share_mode_sta_t new_sta = ctx->current_sta;
            if (new_sta == BT_MCSYNC_SHARE_STATE_SHARING && old_sta != new_sta) {
                apps_config_set_vp(VP_INDEX_SUCCESSED, false, 200, VOICE_PROMPT_PRIO_HIGH, false, NULL);
            }
            /* If the state become to normal, finish self to exit share mode completely. */
            if (new_sta == BT_MCSYNC_SHARE_STATE_NORMAL) {
                ui_shell_finish_activity(self, self);
            }
            ret = true;
        }
        break;

        /* When power off requested, all app will received this event */
        case APPS_EVENTS_INTERACTION_POWER_OFF: {
            /* Pending the possible RHO until the earbuds exit share mode completely. */
            ctx->rho_status = BT_STATUS_PENDING;
            app_share_disable_share_mode(self);
        }
        break;

        case APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN: {
            /* Workaround: the LED display will update when mmi state changed on follower side. */
            APPS_LOG_MSGID_I(TAG"set the LED parttern to share mode.", 0);
            apps_config_set_background_led_pattern(LED_INDEX_INCOMING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            ret = true;
        }
        break;
        default:
            break;
    }
    return ret;
}


#ifdef MTK_AWS_MCE_ENABLE
static bool _proc_aws_data(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;

        apps_aws_sync_event_decode(aws_data_ind, &event_group, &action);
        if (event_group == EVENT_GROUP_UI_SHELL_KEY) {
            ret = true;
            switch (action) {
                case KEY_SHARE_MODE_SWITCH: {
                    APPS_LOG_MSGID_I(TAG"share mode switch trigger by aws key.", 0);
                    app_share_disable_share_mode(self);
                }
                break;

                case KEY_SHARE_MODE_FOLLOWER_SWITCH: {
                    APPS_LOG_MSGID_I(TAG"share mode follower switch trigger by aws key.", 0);
                    app_share_disable_share_mode(self);
                }
                break;

                default:
                    ret = false;
                    break;
            }
        }
    }

    return ret;
}
#endif


#ifdef MTK_AWS_MCE_ENABLE
#ifdef MTK_SMART_CHARGER_ENABLE
static bool _proc_charger_case_group(
        struct _ui_shell_activity *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT: {
            app_smcharger_public_event_para_t *event_para = (app_smcharger_public_event_para_t *)extra_data;
            /* When put any earbud into charger case, disable share mode and pending possible RHO. */
            if (event_para->action == SMCHARGER_CHARGER_IN_ACTION) {
                app_share_context_t* ctx = get_local_context();
                APPS_LOG_MSGID_I(TAG"charger in.", 0);
                ctx->rho_status = BT_STATUS_PENDING;
                app_share_disable_share_mode(self);
            }
        }
        break;

        default:
            return ret;
    }
    return ret;
}
#endif
#endif


bool app_share_activity(
            struct _ui_shell_activity *self,
            uint32_t event_group,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }

        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            ret = _proc_apps_internal_events(self, event_id, extra_data, data_len);
            break;
        }

#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = _proc_aws_data(self, event_id, extra_data, data_len);
            break;
#endif

#ifdef DISABLE_PAGE_SCAN_IN_APPLAYER
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            /* Check the remote connection link status. */
            if (event_id == BT_CM_EVENT_REMOTE_INFO_UPDATE) {
                bt_mcsync_share_role_t role;
                bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
                role = bt_mcsync_share_get_role();
                if (role & BT_MCSYNC_SHARE_ROLE_FOLLOWER) {
                    return;
                }
                uint32_t conn_devs = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 2);
                /* Enable page scan to allowe the SP connect whil no SP connected. */
                if (conn_devs == 0) {
                    enable_page_scan();
                } else {
                    /* Disable page scan when SP connected. */
                    disable_page_scan();
                }
            }
            break;
#endif

#ifdef MTK_SMART_CHARGER_ENABLE
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE:
            ret = _proc_charger_case_group(self, event_id, extra_data, data_len);
            break;
#else
        case EVENT_GROUP_UI_SHELL_BATTERY: {
            if (event_id == APPS_EVENTS_BATTERY_CHARGER_EXIST_CHANGE) {
                int32_t charger_exist = (int32_t)extra_data;
                /* When put any earbud into charger case, disable share mode and pending possible RHO. */
                if (charger_exist == 1) {
                    app_share_context_t* ctx = get_local_context();
                    APPS_LOG_MSGID_I(TAG"charger exist: %d.", 1, charger_exist);
                    ctx->rho_status = BT_STATUS_PENDING;
                    app_share_disable_share_mode(self);
                }
            }
            break;
        }
        break;

#endif
#if 0
        case EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT: {
            if (event_id == EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT) {
                bt_mcsync_share_role_t role = bt_mcsync_share_get_role();
                if (role & BT_MCSYNC_SHARE_ROLE_AGENT) {
                    app_smcharger_public_event_para_t* data;
                    data = (app_smcharger_public_event_para_t*)extra_data;
                    APPS_LOG_MSGID_I(TAG"charger case key action: %d %d.", 2, data->action, data->data);
                }
            }
            break;
        }
#endif
        default:
            break;
    }
    return ret;
}


#endif

