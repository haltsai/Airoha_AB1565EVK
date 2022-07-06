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
 * File: app_power_save_idle_activity.c
 *
 * Description: This file could handle BT/Key/Audio event and start "waiting_timeout" activity for Power Saving APP.
 *
 */

#include "app_power_save_utils.h"
#include "apps_events_event_group.h"
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
#include "apps_race_cmd_co_sys_event.h"
#endif
#include "ui_shell_manager.h"
#include "app_power_save_waiting_activity.h"
#include "app_home_screen_idle_activity.h"
#include "apps_config_key_remapper.h"
#include "apps_events_interaction_event.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_config_led_manager.h"
#include "apps_events_key_event.h"
#include "apps_aws_sync_event.h"
#include "apps_events_battery_event.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "apps_customer_config.h"

#if defined(MTK_AWS_MCE_ENABLE)
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_srv.h"
#endif
#include "battery_management.h"
#include "FreeRTOS.h"
#include "apps_debug.h"
#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif
#ifdef AIR_TILE_ENABLE
#include "app_tile.h"
#endif
#include "audio_transmitter_internal.h"
#include "bt_sink_srv_ami.h"

#define LOG_TAG     "[POWER_SAVING][IDLE] "

static app_power_saving_context_t app_power_saving_context; /* The global context variable for Power Saving APP */

static void app_power_saving_idle_state_proc(ui_shell_activity_t *self)
{
    /* Get target_mode and start "waiting_timeout" activity if need to do power saving.*/
    app_power_saving_type_t type = 0;
    app_power_saving_target_mode_t target_mode =
        app_power_save_utils_get_target_mode(self, &type);
    if (APP_POWER_SAVING_TARGET_MODE_NORMAL != target_mode) {
        ((app_power_saving_context_t *)(self->local_context))->app_state = POWER_SAVING_STATE_WAITING_TIMEOUT;
        ((app_power_saving_context_t *)(self->local_context))->waiting_type = type;
        ui_shell_start_activity(
            self, app_power_saving_waiting_timeout_activity_proc,
            ACTIVITY_PRIORITY_LOWEST, self->local_context, 0);
        APPS_LOG_MSGID_I(LOG_TAG"app_power_saving_idle_state_proc need start timer", 0);
    }
}

#ifdef MTK_ANC_ENABLE
static uint8_t s_anc_enable = 0;

static app_power_saving_target_mode_t anc_get_power_saving_target_mode_func(void)
{
    /* Update target_mode according to "s_anc_enable" variable and ANC power saving mode.*/
    app_power_saving_target_mode_t target_mode = APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
    if (s_anc_enable) {
#if (APPS_POWER_SAVING_WHEN_ANC_PASSTHROUGH_ENABLED == APPS_POWER_SAVING_DISABLE_BT)
        target_mode = APP_POWER_SAVING_TARGET_MODE_BT_OFF;
#elif (APPS_POWER_SAVING_WHEN_ANC_PASSTHROUGH_ENABLED == APPS_POWER_SAVING_NONE)
        target_mode = APP_POWER_SAVING_TARGET_MODE_NORMAL;
#endif
    }
    return target_mode;
}
#endif

#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT) || defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
static app_power_saving_target_mode_t audio_out_get_power_saving_target_mode_func(void)
{
    static bool s_audio_working = false;
    bool temp_working = false;
    audio_transmitter_scenario_list_t audio_transmitter_voice_scenario_list[] = {
        {AUDIO_TRANSMITTER_GAMING_MODE, AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET},
#ifdef AIR_WIRED_AUDIO_ENABLE
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT},
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT},
#endif
    };
    temp_working = audio_transmitter_get_is_running_by_scenario_list(
        audio_transmitter_voice_scenario_list,
        sizeof(audio_transmitter_voice_scenario_list) / sizeof(audio_transmitter_scenario_list_t));
    if (s_audio_working != temp_working) {
        s_audio_working = temp_working;
        APPS_LOG_MSGID_I(LOG_TAG"audio_out_get_power_saving_target_mode_func audio_working = %d", 1, s_audio_working);
    }
    if (true == s_audio_working) {
        return APP_POWER_SAVING_TARGET_MODE_NORMAL;
    } else {
        return APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
    }
}
#endif

/******************************************************************************/
/**************************** proc functions **********************************/
/******************************************************************************/
static bool app_power_saving_idle_proc_ui_shell_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = true;
    app_power_saving_context_t *local_context = (app_power_saving_context_t *)self->local_context;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
            APPS_LOG_MSGID_I(LOG_TAG"create", 0);
            /* load the power saving from nvkey. */
            app_power_save_utils_init();
            /* Get context, register ANC callback function, update target_mode.*/
            self->local_context = &app_power_saving_context;
            local_context = (app_power_saving_context_t *)self->local_context;
            local_context->app_state = POWER_SAVING_STATE_IDLE;
#ifdef MTK_ANC_ENABLE
            app_power_save_utils_register_get_mode_callback(anc_get_power_saving_target_mode_func);
#endif
#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT) || defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            app_power_save_utils_register_get_mode_callback(audio_out_get_power_saving_target_mode_func);
#endif
            local_context->bt_sink_srv_state = APP_POWER_SAVING_BT_OFF;
            app_power_saving_idle_state_proc(self);
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

static bool app_power_saving_idle_proc_key_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
#if defined(MTK_AWS_MCE_ENABLE)
    uint8_t key_id;
    airo_key_event_t key_event;

    app_event_key_event_decode(&key_id, &key_event, event_id);

    /* If have any key pressed or release, Partner must notify Agent to refresh timer. */
    if (AIRO_KEY_RELEASE == key_event || AIRO_KEY_PRESS == key_event) {
        if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()
            && BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
            apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_POWER_SAVING, APP_POWER_SAVING_EVENT_REFRESH_TIME);
            APPS_LOG_MSGID_I(LOG_TAG"press key on partner, need notify agent refresh timer", 0);
        }
    }
#endif

    return ret;
}

static bool app_power_saving_idle_bt_cm_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    app_power_saving_context_t *local_context = (app_power_saving_context_t *)self->local_context;
    if (!local_context) {
        return ret;
    }

    if (POWER_SAVING_STATE_IDLE != local_context->app_state) {
        /* Transient ("waiting_timeout" and "timeout") activity has processed the event. */
        return ret;;
    }

    /* Update bt_state and update/process target_mode. */
    bool bt_changed = app_power_save_utils_update_bt_state(self, event_id, extra_data, data_len);
    if (bt_changed) {
        app_power_saving_idle_state_proc(self);
    }
    return ret;
}

#if defined(MTK_AWS_MCE_ENABLE)
static bool app_power_saving_idle_aws_data_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    uint32_t event_group;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        apps_aws_sync_event_decode(aws_data_ind, &event_group, &event_id);
        switch (event_group) {
            /* Partner do power saving action after receiving DISABLE_BT and POWER_OFF_SYSTEM event from Agent. */
            case EVENT_GROUP_UI_SHELL_POWER_SAVING:
                switch (event_id) {
                    case APP_POWER_SAVING_EVENT_DISABLE_BT:
                        APPS_LOG_MSGID_I(LOG_TAG"Received event from Agent to disable BT", 0);
                        if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
                            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT,
                                                (void *)false, 0,
                                                NULL, 0);
                            apps_config_set_foreground_led_pattern(LED_INDEX_POWER_ON, 30, false);
                        }
                        ret = true;
                        break;
                    case APP_POWER_SAVING_EVENT_POWER_OFF_SYSTEM:
                        if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
#ifdef AIR_TILE_ENABLE
                            if (app_tile_tmd_is_active())
                            {
                                APPS_LOG_MSGID_I(LOG_TAG"tile is activated, Received event from Agent to classic BT off", 0);
                                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                    APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF,
                                                    NULL, 0,
                                                    NULL, 0);
                                /* Notify fast pair to stop ble adv after 3 sec */
                                ui_shell_send_event(false, EVENT_PRIORITY_HIGH,
                                                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                    APPS_EVENTS_INTERACTION_STOP_FAST_PAIR_ADV,
                                                    NULL, 0,
                                                    NULL, 3000);
                                apps_config_set_vp(VP_INDEX_POWER_OFF, false, 0, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
                                apps_config_set_foreground_led_pattern(LED_INDEX_POWER_ON, 30, false);
                            }
                            else
#endif
                            {
                                APPS_LOG_MSGID_I(LOG_TAG"Received event from Agent to System off", 0);
                                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                    APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF,
                                                    (void *)false, 0,
                                                    NULL, 0);
                                apps_config_set_vp(VP_INDEX_POWER_OFF, false, 0, VOICE_PROMPT_PRIO_EXTREME, true, app_home_screen_power_off_vp_play_callback);
                                apps_config_set_foreground_led_pattern(LED_INDEX_POWER_ON, 30, false);
                            }
                        }
                        ret = true;
                        break;
                    default:
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

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(AIR_SILENCE_DETECTION_ENABLE)
static bool app_power_saving_idle_dual_chip_cmd_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    switch (event_id) {
        case APPS_RECE_CMD_CO_SYS_DUAL_CHIP_EVENT_SILENCE_DETECT: {
            bool silence = *(bool *)extra_data;
            app_power_save_utils_slave_silence_detect(silence);
            break;
        }
        default:
            break;
    }

    return ret;
}
#endif

static bool app_power_saving_idle_audio_group_proc(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_id) {
#ifdef MTK_ANC_ENABLE
        case AUDIO_ANC_CONTROL_EVENT_ON:
        case AUDIO_ANC_CONTROL_EVENT_OFF:
        case AUDIO_ANC_CONTROL_EVENT_FORCE_OFF: {
            s_anc_enable = app_anc_service_is_enable();
            APPS_LOG_MSGID_I(LOG_TAG" audio event, s_anc_enable=%d", 1, s_anc_enable);
            app_power_save_utils_notify_mode_changed(FALSE, anc_get_power_saving_target_mode_func);
        }
        break;
#endif
        default:
            break;
    }

    return ret;
}

static bool app_power_saving_idle_power_saving_group_proc(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case APP_POWER_SAVING_EVENT_NOTIFY_CHANGE: {
            /* Add new power saving target mode function and update/process target_mode when receive NOTIFY_CHANGE event.*/
            get_power_saving_target_mode_func_t callback_func = (get_power_saving_target_mode_func_t)extra_data;
            if (callback_func) {
                app_power_save_utils_add_new_callback_func(callback_func);
            }
            app_power_saving_idle_state_proc(self);
            ret = true;
            break;
        }
        default:
            break;
    }

    return ret;
}

bool app_power_save_idle_activity_proc(
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
            ret = app_power_saving_idle_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef AIR_SILENCE_DETECTION_ENABLE
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            if (event_id == APPS_EVENTS_INTERACTION_SILENCE_DETECT_CHANGE) {
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_POWER_SAVING,
                        APP_POWER_SAVING_EVENT_NOTIFY_CHANGE, NULL, 0, NULL, 0);
            }
            break;
        }
#endif
        /* UI Shell key events. */
        case EVENT_GROUP_UI_SHELL_KEY:
            ret = app_power_saving_idle_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell new BT_CM events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
#ifdef AIR_LE_AUDIO_ENABLE
        case EVENT_GROUP_UI_SHELL_BT_SINK:
#endif
            ret = app_power_saving_idle_bt_cm_event_proc(self, event_id, extra_data, data_len);
            break;
        /* UI Shell ANC events. */
        case EVENT_GROUP_UI_SHELL_AUDIO_ANC:
            ret = app_power_saving_idle_audio_group_proc(self, event_id, extra_data, data_len);
            break;
        /* UI Shell Power Saving events. */
        case EVENT_GROUP_UI_SHELL_POWER_SAVING:
            ret = app_power_saving_idle_power_saving_group_proc(self, event_id, extra_data, data_len);
            break;
#if defined(MTK_AWS_MCE_ENABLE)
        /* UI Shell AWS data events. */
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = app_power_saving_idle_aws_data_proc(self, event_id, extra_data, data_len);
            break;
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(AIR_SILENCE_DETECTION_ENABLE)
        case EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD:
            ret = app_power_saving_idle_dual_chip_cmd_proc(self, event_id, extra_data, data_len);
            break;
#endif
#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT) || defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
        case EVENT_GROUP_UI_SHELL_AMI_VENDOR: {
            switch (event_id) {
                case EVENT_TRANSMITTER_START:
                case EVENT_TRANSMITTER_STOP:
                    app_power_save_utils_notify_mode_changed(FALSE, audio_out_get_power_saving_target_mode_func);
                    break;
                default:
                    break;
            }
            break;
        }
#endif
        default:
            break;
    }

    return ret;
}
