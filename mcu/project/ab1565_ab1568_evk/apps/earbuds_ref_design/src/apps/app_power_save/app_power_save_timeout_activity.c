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
 * File: app_power_save_timeout_activity.c
 *
 * Description: This file could do power_saving action (DISABLE_BT or POWER_OFF_SYSTEM) for Power Saving APP.
 *
 */

#include "app_power_save_utils.h"
#include "apps_events_event_group.h"
#include "app_power_save_timeout_activity.h"
#include "app_home_screen_idle_activity.h"
#include "apps_customer_config.h"
#include "apps_events_interaction_event.h"
#include "apps_config_key_remapper.h"
#include "apps_events_bt_event.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_config_led_manager.h"
#include "apps_aws_sync_event.h"
#include "apps_events_battery_event.h"
#include "ui_shell_manager.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "apps_debug.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#endif
#ifdef AIR_TILE_ENABLE
#include "app_tile.h"
#include "app_battery_transient_activity.h"
#endif

#define LOG_TAG     "[POWER_SAVING][TIMEOUT] "

/**
* @brief      This function start power saving flow.
* @param[in]  self, the context of the activity.
* @return     true means trigger power saving success.
*/
static bool app_power_saving_timeout_do_power_saving(ui_shell_activity_t *self)
{
    app_power_saving_context_t *local_context = self->local_context;
    app_power_saving_type_t type = 0;
    app_power_saving_target_mode_t target_mode = app_power_save_utils_get_target_mode(self, &type);

    /* Get target_mode and finish self if power saving target mode is normal. */
    if (APP_POWER_SAVING_TARGET_MODE_NORMAL == target_mode) {
        local_context->app_state = POWER_SAVING_STATE_IDLE;
        ui_shell_finish_activity(self, self);
        APPS_LOG_MSGID_I(LOG_TAG"Not need power saving, return to idle", 0);
        return false;
    }

#if defined(MTK_AWS_MCE_ENABLE)
    /* Send AWS event to notify partner to do power saving action if Agent role and AWS attached. */
    if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()
        && BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
        if (APP_POWER_SAVING_TARGET_MODE_BT_OFF == target_mode) {
            APPS_LOG_MSGID_I(LOG_TAG"Send NEED_DISABLE_BT to partner", 0);
            apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_POWER_SAVING,
                                     APP_POWER_SAVING_EVENT_DISABLE_BT);
        } else if (APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF == target_mode) {
            APPS_LOG_MSGID_I(LOG_TAG"Send NEED_SYSTEM_OFF to partner", 0);
            apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_POWER_SAVING,
                                     APP_POWER_SAVING_EVENT_POWER_OFF_SYSTEM);
        }
    } else
#endif
    {
        /* Partner do power saving action if Partner timeout or Agent timeout firstly. */
        /* Agent do power saving action if AWS inactive. */
        if (APP_POWER_SAVING_TARGET_MODE_BT_OFF == target_mode) {
            APPS_LOG_MSGID_I(LOG_TAG"Send REQUEST_ON_OFF_BT", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT,
                                (void *)false, 0,
                                NULL, 0);
            apps_config_set_foreground_led_pattern(LED_INDEX_POWER_OFF, 30, false);
            /* Play VP when start do power saving */
            apps_config_set_vp(VP_INDEX_POWER_OFF, true, 100, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
            ui_shell_finish_activity(self, self);
        } else if (APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF == target_mode) {
#ifdef AIR_TILE_ENABLE
            if (!app_tile_tmd_is_active() || app_tile_get_battery_state() <= APP_BATTERY_STATE_LOW_CAP)
#endif
            {
                APPS_LOG_MSGID_I(LOG_TAG"Send REQUEST_POWER_OFF", 0);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF,
                                    (void *)false, 0,
                                    NULL, 0);
                apps_config_set_foreground_led_pattern(LED_INDEX_POWER_OFF, 30, false);
                /* Play VP when start do power saving */
                apps_config_set_vp(VP_INDEX_POWER_OFF, true, 100, VOICE_PROMPT_PRIO_EXTREME, true, app_home_screen_power_off_vp_play_callback);
            }
#ifdef AIR_TILE_ENABLE
            else
            {
                APPS_LOG_MSGID_I(LOG_TAG"Send REQUEST_CLASSIC_BT_OFF", 0);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF,
                                    NULL, 0,
                                    NULL, 0);
                /* Notify fast pair to stop ble adv after 3 sec */
                ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_STOP_FAST_PAIR_ADV,
                                    NULL, 0,
                                    NULL, 3000);
                apps_config_set_foreground_led_pattern(LED_INDEX_POWER_OFF, 30, false);
                /* Play VP when start do power saving */
                apps_config_set_vp(VP_INDEX_POWER_OFF, true, 100, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
            }
            ui_shell_finish_activity(self, self);
#endif
        }
    }
    return true;
}

/******************************************************************************/
/**************************** proc functions **********************************/
/******************************************************************************/
static bool app_power_saving_timeout_proc_ui_shell_group(
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
            /* Do power_saving action when Timeout activity init*/
            app_power_saving_timeout_do_power_saving(self);
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

static bool app_power_saving_timeout_bt_cm_event_proc(
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
        app_power_saving_timeout_do_power_saving(self);
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == local_context || NULL == remote_update) {
                break;
            }
#ifdef MTK_AWS_MCE_ENABLE
            /* Agent do power saving action if AWS inactive. */
            if (BT_AWS_MCE_ROLE_AGENT == role
                && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                APPS_LOG_MSGID_I(LOG_TAG"aws inactive", 0);
                app_power_saving_timeout_do_power_saving(self);
            }
#endif
        }
        break;
        default:
            break;
    }

    return ret;
}


static bool app_power_saving_timeout_power_saving_proc(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case APP_POWER_SAVING_EVENT_NOTIFY_CHANGE: {
            /* Add new power saving target mode function and do power saving action when receive NOTIFY_CHANGE event.*/
            get_power_saving_target_mode_func_t callback_func = (get_power_saving_target_mode_func_t)extra_data;
            if (callback_func) {
                app_power_save_utils_add_new_callback_func(callback_func);
            }
            app_power_saving_timeout_do_power_saving(self);
            ret = true;
            break;
        }
        default:
            break;
    }

    return ret;
}

bool app_power_save_timeout_activity_proc(
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
            ret = app_power_saving_timeout_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell new BT_CM events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
#ifdef AIR_LE_AUDIO_ENABLE
        case EVENT_GROUP_UI_SHELL_BT_SINK:
#endif
            ret = app_power_saving_timeout_bt_cm_event_proc(self, event_id, extra_data, data_len);
            break;
        /* UI Shell Power Saving events. */
        case EVENT_GROUP_UI_SHELL_POWER_SAVING:
            ret = app_power_saving_timeout_power_saving_proc(self, event_id, extra_data, data_len);
            break;
        default:
            break;
    }

    return ret;
}
