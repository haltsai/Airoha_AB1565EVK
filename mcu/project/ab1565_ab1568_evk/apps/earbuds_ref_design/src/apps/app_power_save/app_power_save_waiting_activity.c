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
 * File: app_power_save_waiting_activity.c
 *
 * Description: This file could handle BT/Key/AWS data event and start Timeout activity for Power Saving APP.
 *
 */

#include "app_power_save_utils.h"
#include "apps_events_event_group.h"
#include "app_power_save_waiting_activity.h"
#include "apps_customer_config.h"
#include "apps_events_interaction_event.h"
#include "apps_config_key_remapper.h"
#include "app_power_save_timeout_activity.h"
#include "apps_aws_sync_event.h"
#include "apps_events_battery_event.h"
#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le.h"
#ifdef AIR_LE_AUDIO_MULTIPOINT_ENABLE
#include "bt_sink_srv_le_cap.h"
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
#include "bt_sink_srv_le_cap_stream.h"
#endif
#endif

#include "ui_shell_manager.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#if defined(MTK_AWS_MCE_ENABLE)
#include "bt_aws_mce_report.h"
#endif
#include "apps_debug.h"

#define LOG_TAG     "[POWER_SAVING][WAITING] "

static void app_power_saving_waiting_timeout_refresh_timer(struct _ui_shell_activity *self)
{
    /* refresh power saving timer via remove and re-send event. */
    app_power_saving_context_t *local_context = (app_power_saving_context_t *)self->local_context;
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_POWER_SAVING, APP_POWER_SAVING_EVENT_TIMEOUT);
    ui_shell_send_event(false,
                        EVENT_PRIORITY_HIGNEST,
                        EVENT_GROUP_UI_SHELL_POWER_SAVING,
                        APP_POWER_SAVING_EVENT_TIMEOUT,
                        NULL, 0,
                        NULL, app_power_save_utils_get_timeout(local_context->waiting_type));
    APPS_LOG_MSGID_I(LOG_TAG"refresh timer", 0);
}

static void app_power_saving_waiting_timeout_state_proc(ui_shell_activity_t *self)
{
    app_power_saving_context_t *local_context = (app_power_saving_context_t *)self->local_context;
    app_power_saving_type_t type = 0;

    /* Get target_mode and finish self if power saving target mode is normal mode. */
    app_power_saving_target_mode_t target_mode =
        app_power_save_utils_get_target_mode(self, &type);
    if (APP_POWER_SAVING_TARGET_MODE_NORMAL == target_mode) {
        /* Finish and return to idle state. */
        APPS_LOG_MSGID_I(LOG_TAG"Not need power saving, return to idle", 0);
        ui_shell_finish_activity(self, self);
        local_context->app_state = POWER_SAVING_STATE_IDLE;
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_POWER_SAVING, APP_POWER_SAVING_EVENT_TIMEOUT);
    } else if (type != local_context->waiting_type) {
        APPS_LOG_MSGID_I(LOG_TAG"The waiting type is different, refresh", 0);
        local_context->waiting_type = type;
        app_power_saving_waiting_timeout_refresh_timer(self);
    }
}


/******************************************************************************/
/**************************** proc functions **********************************/
/******************************************************************************/
static bool app_power_saving_waiting_timeout_proc_ui_shell_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = true;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
            APPS_LOG_MSGID_I(LOG_TAG"create", 0);
            self->local_context = extra_data;
            /* Start power saving timer via send APP_POWER_SAVING_EVENT_TIMEOUT event with delay. */
            ui_shell_send_event(false,
                                EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_POWER_SAVING,
                                APP_POWER_SAVING_EVENT_TIMEOUT,
                                NULL, 0,
                                NULL, app_power_save_utils_get_timeout(((app_power_saving_context_t *)self->local_context)->waiting_type));
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            APPS_LOG_MSGID_I(LOG_TAG"destroy", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            APPS_LOG_MSGID_I(LOG_TAG"resume", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            APPS_LOG_MSGID_I(LOG_TAG"pause", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
            APPS_LOG_MSGID_I(LOG_TAG"refresh", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
            APPS_LOG_MSGID_I(LOG_TAG"result", 0);
            break;
        default:
            break;
    }
    return ret;
}

static bool app_power_saving_waiting_timeout_proc_key_event_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    /* Refresh power saving timer when received any key event. */
    app_power_saving_waiting_timeout_refresh_timer(self);
    return ret;
}

static bool app_power_saving_waiting_timeout_bt_cm_event_proc(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    app_power_saving_context_t *local_context = (app_power_saving_context_t *)self->local_context;
    if (!local_context) {
        return ret;
    }

    /* Update bt_state and update/process target_mode. */
    bool bt_changed = app_power_save_utils_update_bt_state(self, event_id, extra_data, data_len);
    if (bt_changed) {
        app_power_saving_waiting_timeout_state_proc(self);
    } else if (local_context->bt_sink_srv_state == APP_POWER_SAVING_BT_CONNECTED) {
        /* To support multi point, when the second device connected, need refresh time. */
        if (BT_CM_EVENT_REMOTE_INFO_UPDATE == event_id && extra_data != NULL) {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if ((remote_update->pre_connected_service & (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) == 0
                && (remote_update->connected_service & (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) != 0) {
                app_power_saving_waiting_timeout_refresh_timer(self);
            }
        }
    }
    return ret;
}

#if defined(MTK_AWS_MCE_ENABLE)
static bool app_power_saving_waiting_timeout_aws_data_proc(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    uint32_t event_group;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        apps_aws_sync_event_decode(aws_data_ind, &event_group, &event_id);
        switch (event_group) {
            case EVENT_GROUP_UI_SHELL_POWER_SAVING:
                switch (event_id) {
                    case APP_POWER_SAVING_EVENT_REFRESH_TIME:
                        APPS_LOG_MSGID_I(LOG_TAG"Received event to refresh power saving time", 0);
                        /* Agent refresh power saving timer when Partner received any key event. */
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                            app_power_saving_waiting_timeout_refresh_timer(self);
                        }
                        break;
                }
                break;
            default:
                break;
        }
    }

    return ret;
}
#endif

static bool app_power_saving_waiting_timeout_power_saving_event_proc(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    app_power_saving_context_t *local_context = (app_power_saving_context_t *)self->local_context;

    switch (event_id) {
        case APP_POWER_SAVING_EVENT_TIMEOUT:
            /* Finish and start Timeout activity (do power saving action) when power saving timeout. */
            APPS_LOG_MSGID_I(LOG_TAG"Timeout to do power saving, start timeout activity", 0);
            local_context->app_state = POWER_SAVING_STATE_TIMEOUT;
            ui_shell_start_activity(NULL,
                                    app_power_save_timeout_activity_proc,
                                    ACTIVITY_PRIORITY_LOWEST,
                                    self->local_context, 0);
            ui_shell_finish_activity(self, self);
            break;
        case APP_POWER_SAVING_EVENT_NOTIFY_CHANGE: {
            /* Add new power saving target mode function and update/process target mode when receive NOTIFY_CHANGE event.*/
            get_power_saving_target_mode_func_t callback_func = (get_power_saving_target_mode_func_t)extra_data;
            if (callback_func) {
                app_power_save_utils_add_new_callback_func(callback_func);
            }
            app_power_saving_waiting_timeout_state_proc(self);
            ret = true;
            break;
        }
        case APP_POWER_SAVING_EVENT_REFRESH_TIME: {
            app_power_saving_waiting_timeout_refresh_timer(self);
            break;
        }
        default:
            break;
    }
    return ret;
}

bool app_power_saving_waiting_timeout_activity_proc(
    ui_shell_activity_t *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = app_power_saving_waiting_timeout_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell key events. */
        case EVENT_GROUP_UI_SHELL_KEY:
            ret = app_power_saving_waiting_timeout_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell new BT_CM events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
#ifdef AIR_LE_AUDIO_ENABLE
        case EVENT_GROUP_UI_SHELL_BT_SINK:
#endif
            ret = app_power_saving_waiting_timeout_bt_cm_event_proc(self, event_id, extra_data, data_len);
            break;
#if defined(MTK_AWS_MCE_ENABLE)
        /* UI Shell AWS data events. */
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = app_power_saving_waiting_timeout_aws_data_proc(self, event_id, extra_data, data_len);
            break;
#endif
        /* UI Shell Power Saving events. */
        case EVENT_GROUP_UI_SHELL_POWER_SAVING:
            ret = app_power_saving_waiting_timeout_power_saving_event_proc(self, event_id, extra_data, data_len);
            break;
        default:
            break;
    }
    return ret;
}
