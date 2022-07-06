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
 * File: app_share_idle_activity.c
 *
 * Description:
 * This file is the activity to handle the key action or race command to enter share mode.
 * When enter share mode is requested, this activity will try to enter share mode and start
 * app_share_activity.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#ifdef AIR_MCSYNC_SHARE_ENABLE

#include "app_share_utils.h"


#define TAG "app_share_idle_activity "
#define get_local_context app_share_get_local_context


static void app_share_mode_trigger_sta_update(){
    ui_shell_status_t status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH,
                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_SHARE_MODE_STA_UPDATE,
                    NULL, 0, NULL, 0);
    if (UI_SHELL_STATUS_OK != status) {
        APPS_LOG_MSGID_I(TAG"send sta update event failed.", 0);
    }
}

/**
* @brief      This function will be called when share mode states updated or some errors happend.
* @param[in]  event, the current event come from middleware
* @param[in]  status, indicate the operation is success of failed.
* @param[in]  data, the pointer of payload.
* @return     Always return BT_STATUS_SUCCESS.
*/
static bt_status_t mcsync_share_event_callback(bt_mcsync_share_event_t event, bt_status_t status, void* data)
{
    bt_mcsync_share_role_t role;

    role = bt_mcsync_share_get_role();
    APPS_LOG_MSGID_I(TAG"mcsync share event callback, event:%d, status:%d, role: %d.", 3, event, status, role);

    switch (event) {
        case BT_MCSYNC_SHARE_EVENT_SHARE_PAIRING_IND:
            if (status == BT_STATUS_SUCCESS) {
                app_share_context_t* ctx = get_local_context();
                ctx->share_pairing_result = true;
                APPS_LOG_MSGID_I(TAG"set share pairing result %d", 1, ctx->share_pairing_result);
            }
            return BT_STATUS_SUCCESS;
            break;
        case BT_MCSYNC_SHARE_EVENT_CONNECTING_IND:
            if (status == BT_STATUS_SUCCESS) {
                app_share_context_t* ctx = get_local_context();
                ctx->share_pairing_result = true;
                APPS_LOG_MSGID_I(TAG"set share pairing result %d", 1, ctx->share_pairing_result);
            }
            break;
        case BT_MCSYNC_SHARE_EVENT_CONNECTED_IND:
            //apps_config_set_vp(VP_INDEX_SUCCESSED, true, 200, VOICE_PROMPT_PRIO_HIGH, false, NULL);
            break;
        case BT_NCSYNC_SHARE_EVENT_EXIT_SHARE_MODE_CNF:
            //apps_config_set_vp(VP_INDEX_FAILED, true, 200, VOICE_PROMPT_PRIO_HIGH, false, NULL);
            break;
        case BT_MCSYNC_SHARE_EVENT_ACTION_IND:
            app_share_handle_share_action((bt_mcsync_share_action_info_t*) data);
            break;
    }    
    app_share_mode_trigger_sta_update();

    return BT_STATUS_SUCCESS;
}


#ifdef MTK_AWS_MCE_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
bt_status_t app_share_handover_allow_execution(const bt_bd_addr_t *addr)
{
    bt_mcsync_share_role_t role;
    role = bt_mcsync_share_get_role();

    if (role & BT_MCSYNC_SHARE_ROLE_PARTNER) {
        return BT_STATUS_SUCCESS;
    }

    /*
     * In normal state, rho is allowed.
     * In share mode state, rho is not allowed.
     * Agent power off or put into charger case, rho will be pending until exit share mode.
     */
    app_share_context_t* ctx = get_local_context();
    bt_status_t sta = ctx->rho_status;
    if (sta == BT_STATUS_PENDING) {
        ctx->rho_pending = true;
    }
    APPS_LOG_MSGID_I(TAG"rho allw execution status: %d.", 1, sta);
    return sta;
}


void app_share_handover_status_callback(const bt_bd_addr_t *addr,
                                   bt_aws_mce_role_t role,
                                   bt_role_handover_event_t event,
                                   bt_status_t status)
{
    app_share_context_t* ctx = get_local_context();
    APPS_LOG_MSGID_I(TAG"rho finished, pending flag %d, event:%d, status: %d.", 3,
                    ctx->rho_pending, event, status);

    if (event == BT_ROLE_HANDOVER_COMPLETE_IND && ctx->rho_pending && status == BT_STATUS_SUCCESS) {
        ctx->rho_pending = false;
        /* When rho completed, the share mode is disabled, the rho is allowed. */
        ctx->rho_status = BT_STATUS_SUCCESS;
    }
    /* app share not care the rho status, because app share would not allowed rho
     * in share mode status. But in the followe case, the share mode will disable
     * and try to trigger rho.
     * 1. The Agent put into charger case.
     * 2. The Agent in low power status.
     * 3. The Agent power off by user key.
     * 4. Other power off case in Agent.
    */
}
#endif
#endif


/**
* @brief      This function will be called when race command about share mode received.
* @param[in]  cmd, the command of share mode control.
* @return     return the command excuete result.
*/
uint8_t share_mode_set_callback(race_share_mode_set_cmd cmd)
{
    bt_mcsync_share_state_t sta;
    uint16_t send_event = 0;

    sta = bt_mcsync_share_get_state();
    APPS_LOG_MSGID_I(TAG"receive share set cmd: %d, current sta: %d.", 2, cmd, sta);

    /* Conver the command to key event. */
    if (cmd == RACE_CMD_SHARE_MODE_SET_ENABLE_SHARE && sta == BT_MCSYNC_SHARE_STATE_NORMAL) {
        send_event = KEY_SHARE_MODE_SWITCH;
    } else if (cmd == RACE_CMD_SHARE_MODE_SET_ENABLE_FOLLOWER && sta == BT_MCSYNC_SHARE_STATE_NORMAL) {
        send_event = KEY_SHARE_MODE_FOLLOWER_SWITCH;
    } else if ((cmd == RACE_CMD_SHARE_MODE_SET_DISABLE) &&
               (sta == BT_MCSYNC_SHARE_STATE_PREPAIRING || sta == BT_MCSYNC_SHARE_STATE_SHARING)) {
        app_share_context_t* ctx = get_local_context();
        send_event = ctx->is_follower ? KEY_SHARE_MODE_FOLLOWER_SWITCH : KEY_SHARE_MODE_SWITCH;
    } else {
        return RACE_ERRCODE_FAIL;
    }

    APPS_LOG_MSGID_I(TAG"send key event:%d.", 1, send_event);
    /* Use the same processing with key action. */
    uint16_t *p_key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
    if (p_key_action == NULL) {
         APPS_LOG_MSGID_I(TAG"malloc mem failed.", 0);
         return RACE_ERRCODE_FAIL;
    }
    *p_key_action = send_event;
    ui_shell_status_t status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH,
                    EVENT_GROUP_UI_SHELL_KEY,
                    INVALID_KEY_EVENT_ID,
                    p_key_action, sizeof(uint16_t), NULL, 0);
    if (UI_SHELL_STATUS_OK != status) {
        vPortFree(p_key_action);
        APPS_LOG_MSGID_I(TAG"send sta update event failed.", 0);
        return RACE_ERRCODE_FAIL;
    }

    return RACE_ERRCODE_SUCCESS;
}


race_share_mode_state_type share_mode_get_state_callback(void)
{
    bt_mcsync_share_state_t sta;

    sta = bt_mcsync_share_get_state();
    APPS_LOG_MSGID_I(TAG"race cmd -> get share mode sta:%d.", 1, sta);

    return (race_share_mode_state_type)sta;
}


static void app_share_mode_init(app_share_context_t* ctx){
    bt_status_t ret = BT_STATUS_SUCCESS;
    ctx->current_sta = BT_MCSYNC_SHARE_STATE_NORMAL;
    ctx->is_follower = false;
    ctx->ble_adv_paused = false;
    ctx->target_mode = APPS_POWER_SAVING_MODE;
#ifdef MTK_AWS_MCE_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    ctx->rho_status = BT_STATUS_SUCCESS;
    ctx->rho_pending = false;
    /* The page scan is on as default. */
#ifdef DISABLE_PAGE_SCAN_IN_APPLAYER
    ctx->page_scan_off = false;
#endif
    bt_role_handover_callbacks_t role_callbacks = {
        app_share_handover_allow_execution,
        NULL,
        NULL,
        NULL,
        app_share_handover_status_callback
    };
    ret = bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_SHARE_APP, &role_callbacks);
    if (ret != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_I(TAG"RHO init failed: %d", 1, ret);
    }
#endif
#endif
    bt_mcsync_share_config_t config = {
        BT_MCSYNC_SHARE_EXIT_TYPE_ALL,
    };
    ret = bt_mcsync_share_init(&config);
    if (ret != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_I(TAG"MCSYNC share init failed.", 0);
    }

    ret = bt_mcsync_share_register_callback(BT_MCSYNC_SHARE_MODULE_SHARE_APP, mcsync_share_event_callback);
    if (ret != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_I(TAG"MCSYNC share register callback failed.", 0);
    }

    /* Init race cmd callback */
    race_share_mode_config race_config = {
        share_mode_set_callback,
        share_mode_get_state_callback
    };
    race_share_mode_register(&race_config);

    /* Init power save callback */
    app_power_save_utils_register_get_mode_callback(app_share_get_power_saving_target_mode);
}


static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            app_share_context_t* ctx = get_local_context();
            /* init */
            self->local_context = ctx;
            memset(ctx, 0, sizeof(app_share_context_t));
            app_share_mode_init(ctx);
            APPS_LOG_MSGID_I(TAG"create", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(TAG"destory", 0);
            bt_mcsync_share_deinit();
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
        return false;
#if 0
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
#endif
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role;
    role = bt_device_manager_aws_local_info_get_role();
    if (role == BT_AWS_MCE_ROLE_PARTNER) {
        if (action == KEY_SHARE_MODE_SWITCH || action == KEY_SHARE_MODE_FOLLOWER_SWITCH) {
            APPS_LOG_MSGID_I(TAG"partner sync key action to agent.", 0);
            bt_status_t ret;
            ret = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action);
            if (BT_STATUS_SUCCESS != ret) {
                APPS_LOG_MSGID_I(TAG"partner sync key action to agent failed: %d.", 1, ret);
            }
            return true;
        }
    }
#endif

    switch(action) {
        case KEY_SHARE_MODE_SWITCH: {
            APPS_LOG_MSGID_I(TAG"share mode switch trigger by key.", 0);
            app_share_enable_share_mode(self, false);
            ret = true;
        }
        break;

        case KEY_SHARE_MODE_FOLLOWER_SWITCH: {
            APPS_LOG_MSGID_I(TAG"share mode follower switch trigger by key.", 0);
            app_share_enable_share_mode(self, true);
            ret = true;
        }
        break;

        default:
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
    bt_aws_mce_role_t role;
    app_share_context_t* ctx = get_local_context();

    switch (event_id) {
        case APPS_EVENTS_INTERACTION_SHARE_MODE_STA_UPDATE: {
            app_share_mode_sta_t old_sta = ctx->current_sta;
            app_share_mode_sta_update();
            app_share_mode_sta_t new_sta = ctx->current_sta;
            role = bt_mcsync_share_get_role();
            if (role & BT_MCSYNC_SHARE_ROLE_FOLLOWER) {
                ctx->is_follower = true;
            } else {
                ctx->is_follower = false;
            }
            if ((old_sta == BT_MCSYNC_SHARE_STATE_NORMAL) &&
                (new_sta == BT_MCSYNC_SHARE_STATE_PREPAIRING || new_sta == BT_MCSYNC_SHARE_STATE_SHARING)) {
                ui_shell_start_activity(self, app_share_activity, ACTIVITY_PRIORITY_HIGH, NULL, 0);
            }
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
                    app_share_enable_share_mode(self, false);
                }
                break;

                case KEY_SHARE_MODE_FOLLOWER_SWITCH: {
                    APPS_LOG_MSGID_I(TAG"share mode follower switch trigger by aws key.", 0);
                    app_share_enable_share_mode(self, true);
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


bool app_share_idle_activity(
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

#ifdef AIR_MULTI_POINT_ENABLE
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            /* Check the remote connection link status. */
            if (event_id == BT_CM_EVENT_REMOTE_INFO_UPDATE) {
                /* Try to enable share mode after inactivate link is disconnected. */
                bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
                if (!remote_update->connected_service) {
                    app_share_context_t* ctx = NULL;
                    ctx = app_share_get_local_context();
                    uint32_t conn_devs = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 2);
                    if (conn_devs == 1 && ctx->pending_key_action != 0 && remote_update->acl_state == BT_CM_ACL_LINK_DISCONNECTED) {
                        APPS_LOG_MSGID_I(TAG"share mode try to enter share mode after inactivate disconnected.", 0);
                        if (ctx->pending_key_action == KEY_SHARE_MODE_SWITCH) {
                            app_share_enable_share_mode(self, false);
                        } else if (ctx->pending_key_action == KEY_SHARE_MODE_FOLLOWER_SWITCH) {
                            app_share_enable_share_mode(self, true);
                        }
                        ctx->pending_key_action = 0;
                    }
                }
            }
            break;
#endif

#ifdef MTK_SMART_CHARGER_ENABLE
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE: {
            if (event_id == EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT) {
                bt_mcsync_share_role_t role = bt_mcsync_share_get_role();
                if (role & BT_MCSYNC_SHARE_ROLE_AGENT) {
                    app_smcharger_public_event_para_t* data;
                    data = (app_smcharger_public_event_para_t*)extra_data;
                    APPS_LOG_MSGID_I(TAG"charger case key action: %d %d.", 2, data->action, data->data);
                    if (data->action == SMCHARGER_CHARGER_KEY_ACTION) {
                        if (data->data == APP_SMCHARGER_KEY_SHARE_MODE_AGENT || data->data == APP_SMCHARGER_KEY_SHARE_MODE_FOLLOWER) {
                            uint16_t send_event = KEY_SHARE_MODE_SWITCH;
                            if (data->data == APP_SMCHARGER_KEY_SHARE_MODE_FOLLOWER) {
                                send_event = KEY_SHARE_MODE_FOLLOWER_SWITCH;
                            }
                            APPS_LOG_MSGID_I(TAG"send key event:%d.", 1, send_event);
                            uint16_t *p_key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
                            if (p_key_action == NULL) {
                                 APPS_LOG_MSGID_I(TAG"malloc mem failed.", 0);
                                 return RACE_ERRCODE_FAIL;
                            }
                            *p_key_action = send_event;
                            ui_shell_status_t status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH,
                                            EVENT_GROUP_UI_SHELL_KEY,
                                            INVALID_KEY_EVENT_ID,
                                            p_key_action, sizeof(uint16_t), NULL, 0);
                            if (UI_SHELL_STATUS_OK != status) {
                                vPortFree(p_key_action);
                                APPS_LOG_MSGID_I(TAG"send sta update event failed.", 0);
                                return RACE_ERRCODE_FAIL;
                            }
                        }
                    }
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


bool app_share_get_pairing_result(void) {
    app_share_context_t* ctx = get_local_context();
    APPS_LOG_MSGID_I(TAG"get share pairing result %x", 1, ctx->share_pairing_result);
    return ctx->share_pairing_result;
}
