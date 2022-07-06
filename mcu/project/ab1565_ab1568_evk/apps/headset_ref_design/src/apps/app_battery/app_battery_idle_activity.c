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

/**
 * File: app_battery_idle_activity.c
 *
 * Description: This file could sync BT/Battery state, check RHO for Battery APP.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for Battery APP.
 *
 */


#include <string.h>

#include "app_battery_idle_activity.h"
#include "app_battery_transient_activity.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_features_dynamic_setting.h"
#include "apps_events_interaction_event.h"
#include "apps_events_event_group.h"
#ifdef MTK_USB_DEMO_ENABLED
#include "apps_events_usb_event.h"
#include "usb.h"
#ifdef AB1568
#include "hal_pmu_internal_2568.h"
#include "hal_pmu_charger_2568.h"
#endif
#endif
#include "apps_config_key_remapper.h"
#include "battery_management.h"
#include "battery_management_core.h"
#include "apps_debug.h"
#include "ui_shell_manager.h"
#include "FreeRTOS.h"
#include "bt_power_on_config.h"
#include "bt_device_manager.h"
#include "apps_customer_config.h"
#include "prompt_control.h"
#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif
#if defined(MTK_AWS_MCE_ENABLE)
#include "bt_aws_mce_report.h"
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_idle_activity.h"
#endif

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"
#endif
#ifdef MTK_FOTA_ENABLE
#include "app_fota_idle_activity.h"
#endif
#if defined(APPS_LINE_IN_SUPPORT) && defined (AIR_APP_SYSTEM_ON_BY_LINE_IN_ENABLE)
#include "hal_rtc_internal.h"
#endif
#ifdef AIR_TILE_ENABLE
#include "app_bt_state_service.h"
#include "app_tile.h"
#endif

#define LOG_TAG     "[app_battery]"

static battery_local_context_type_t s_battery_context;  /* The variable records context */

/**
* @brief      Shutdown flow when device low battery.
* @param[in]  self, Battery APP idle ui_shell_activity.
*/
static void _shutdown_when_low_battery(struct _ui_shell_activity *self)
{
    APPS_LOG_MSGID_I(LOG_TAG" _shutdown_when_low_battery", 0);
#ifdef AIR_TILE_ENABLE
    const app_bt_state_service_status_t* bt_state_srv_status = app_bt_connection_service_get_current_status();
    if (bt_state_srv_status != NULL && bt_state_srv_status->current_power_state != APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLED)
#endif
    {
        /* Play low battery VP if BT is not in classic off mode */
        apps_config_set_vp(VP_INDEX_POWER_OFF, false, 0, VOICE_PROMPT_PRIO_EXTREME, true, NULL);
        apps_config_set_foreground_led_pattern(LED_INDEX_POWER_OFF, 30, false);
    }
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF, NULL, 0,
                        NULL, 0);
}

/**
* @brief      Update background LED pattern by starting app_battery_transient_activity.
* @param[in]  self, Battery APP idle ui_shell_activity.
* @param[in]  new_state, new battery APP state.
*/
static void battery_update_led_background_pattern(ui_shell_activity_t *self, app_battery_state_t new_state)
{
    /* Start app_battery_transient_activity when old state doesn't need show BG LED, but new state need. */
    if ((s_battery_context.state > APP_BATTERY_STATE_LOW_CAP && s_battery_context.state < APP_BATTERY_STATE_CHARGING)
        && (new_state <= APP_BATTERY_STATE_LOW_CAP || new_state >= APP_BATTERY_STATE_CHARGING)) {
        APPS_LOG_MSGID_I(LOG_TAG" need start battery transient activity to display BG LED", 0);
        ui_shell_start_activity(self, app_battery_transient_activity_proc, ACTIVITY_PRIORITY_LOW, &s_battery_context, 0);
    }
    /* Send event to notify activity update BG_LED. */
    if (s_battery_context.state != new_state) {
        APPS_LOG_MSGID_I(LOG_TAG" old state = %d, new state = %d", 2, s_battery_context.state, new_state);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                            NULL, 0);
    }
}

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
static app_power_saving_target_mode_t app_battery_get_power_saving_target_mode(void)
{
    app_power_saving_target_mode_t target_mode = APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
    if (s_battery_context.charger_exist_state) {
#ifdef APPS_DISABLE_BT_WHEN_CHARGING
        /* Other APP (not battery APP) will disable BT if support APPS_DISABLE_BT_WHEN_CHARGING. */
        target_mode = APP_POWER_SAVING_TARGET_MODE_NORMAL;
#else
        /* Power_saving_mode is BT_OFF (not power off) when charging in. */
        target_mode = APP_POWER_SAVING_TARGET_MODE_BT_OFF;
#endif
    }
    return target_mode;
}
#endif

/**
* @brief      Get Battery APP state.
* @param[in]  self, Battery APP idle ui_shell_activity.
* @return     Return app_battery_state.
*/
static app_battery_state_t _get_battery_state(struct _ui_shell_activity *self)
{
    app_battery_state_t new_state;

    APPS_LOG_MSGID_I(LOG_TAG" charger_exist_state %d, charger state %d", 3, s_battery_context.charger_exist_state, s_battery_context.charging_state);
    if (s_battery_context.charger_exist_state) {
        if (s_battery_context.charging_state == CHARGER_STATE_CHR_OFF || s_battery_context.charging_state == CHARGER_STATE_EOC) {
            new_state = APP_BATTERY_STATE_CHARGING_FULL;
        } else if (s_battery_context.charging_state == CHARGER_STATE_THR) {
            new_state = APP_BATTERY_STATE_THR;
        } else {
            new_state = APP_BATTERY_STATE_CHARGING;
        }
    } else {
        if (s_battery_context.shutdown_state == APPS_EVENTS_BATTERY_SHUTDOWN_STATE_VOLTAGE_LOW) {
            new_state = APP_BATTERY_STATE_SHUTDOWN;
        } else if (s_battery_context.battery_percent < APPS_BATTERY_LOW_THRESHOLD) {
            new_state = APP_BATTERY_STATE_LOW_CAP;
        } else if (s_battery_context.battery_percent >= APPS_BATTERY_FULL_THRESHOLD) {
            new_state = APP_BATTERY_STATE_FULL;
        } else {
            new_state = APP_BATTERY_STATE_IDLE;
        }
    }
    /* Update BG_LED when Battery APP state changed. */
    battery_update_led_background_pattern(self, new_state);

    return new_state;
}

#ifdef APPS_AUTO_TRIGGER_RHO
/**
* @brief      Battery APP check and do RHO.
*             Pre-condition: Agent & AWS Attached & not OTA ongoing
*             RHO Case: The battery percent of partner is 30% greater than agent (both out_of_case)
* @param[in]  self, Battery APP idle ui_shell_activity.
*/
static void check_and_do_rho(struct _ui_shell_activity *self)
{
    if (s_battery_context.aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        APPS_LOG_MSGID_I(LOG_TAG" AWS not connected, cannot rho", 0);
        goto exit;
    }

#ifdef MTK_FOTA_ENABLE
     bool ota_ongoing = app_fota_get_ota_ongoing();
     if (ota_ongoing) {
         APPS_LOG_MSGID_E(LOG_TAG", check_and_do_rho, OTA ongoing", 0);
         return;
     }
#endif

    if (s_battery_context.state != APP_BATTERY_STATE_SHUTDOWN
        && s_battery_context.state < APP_BATTERY_STATE_CHARGING
        && s_battery_context.partner_battery_percent < PARTNER_BATTERY_CHARGING
        && s_battery_context.battery_percent + APPS_DIFFERENCE_BATTERY_VALUE_FOR_RHO < s_battery_context.partner_battery_percent
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        && app_in_ear_is_out()
#endif
       ) {
        APPS_LOG_MSGID_I(LOG_TAG" after checked need do RHO", 0);
        /* Send TRIGGER_RHO event to notify HomeScreen APP do RHO. */
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_TRIGGER_RHO, NULL, 0,
                            NULL, 0);
    }
exit:
    return;
}
#endif

#if defined(MTK_AWS_MCE_ENABLE)
/**
* @brief      Partner notify battery percent to Agent.
* @param[in]  self, Battery APP idle ui_shell_activity.
*/
static void partner_notify_battery_level_to_agent(struct _ui_shell_activity *self)
{
    if (s_battery_context.aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        APPS_LOG_MSGID_I(LOG_TAG" AWS not connected, cannot send aws data", 0);
    } else {
        uint8_t info_array[sizeof(bt_aws_mce_report_info_t)];
        uint8_t data_array[sizeof(s_battery_context.battery_percent)];
        bt_aws_mce_report_info_t *aws_data = (bt_aws_mce_report_info_t *)&info_array;
        aws_data->module_id = BT_AWS_MCE_REPORT_MODULE_BATTERY;
        aws_data->is_sync = FALSE;
        aws_data->sync_time = 0;
        aws_data->param_len = sizeof(s_battery_context.battery_percent);
        if (APP_BATTERY_STATE_CHARGING > s_battery_context.state) {
            *(data_array) = s_battery_context.battery_percent;
        } else {
            *(data_array) = PARTNER_BATTERY_CHARGING | s_battery_context.battery_percent;
        }
        aws_data->param = (void *)data_array;
        APPS_LOG_MSGID_I(LOG_TAG" Send battery value to agent : %d", 1, *((uint8_t *)aws_data->param));
        bt_aws_mce_report_send_event(aws_data);
    }
}
#endif

static bool _proc_ui_shell_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = true;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
            APPS_LOG_MSGID_I(LOG_TAG" create", 0);
            /* Init Battery APP context. */
            app_battery_state_t temp_state;
            memset(&s_battery_context, 0, sizeof(battery_local_context_type_t));
            s_battery_context.battery_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
            s_battery_context.charging_state = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE);
            s_battery_context.charger_exist_state = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
            s_battery_context.shutdown_state = calculate_shutdown_state(battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE));
            s_battery_context.aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
            s_battery_context.partner_battery_percent = PARTNER_BATTERY_INVALID;
            s_battery_context.state = APP_BATTERY_STATE_IDLE;

            temp_state = _get_battery_state(self);

            if (temp_state == APP_BATTERY_STATE_SHUTDOWN) {
                /* Power off device when battery state is APP_BATTERY_STATE_SHUTDOWN. */
                _shutdown_when_low_battery(self);
            } else {
#if defined(APPS_DISABLE_BT_WHEN_CHARGING)
                if (s_battery_context.charger_exist_state == 0) {
#ifdef MTK_ANC_ENABLE
                    bool anc_ret = app_anc_service_resume();
                    APPS_LOG_MSGID_I(LOG_TAG" audio_anc_resume ret = %d", 1, anc_ret);
#endif
#else   /* #if defined(APPS_DISABLE_BT_WHEN_CHARGING) */
#ifdef MTK_ANC_ENABLE
                bool anc_ret = app_anc_service_resume();
                APPS_LOG_MSGID_I(LOG_TAG" audio_anc_resume ret = %d", 1, anc_ret);
#endif
                uint8_t pmu_reason = pmu_get_power_on_reason();
                APPS_LOG_MSGID_I(LOG_TAG" pmu_reason = 0x%x", 1, pmu_reason);
                if (s_battery_context.charger_exist_state == 0 || (~0x04 & pmu_reason)) {
#endif  /* #if defined(APPS_DISABLE_BT_WHEN_CHARGING) */
#if defined(APPS_LINE_IN_SUPPORT) && defined (AIR_APP_SYSTEM_ON_BY_LINE_IN_ENABLE)
                    int rtc_power_on_reason = hal_rtc_get_power_on_reason();
                    APPS_LOG_MSGID_I(LOG_TAG" rtc_power_on_reason = %d", 1, rtc_power_on_reason);
                    if (!(0x02 & pmu_reason && RTC_POWERED_BY_EINT0 == rtc_power_on_reason))
#endif
                    {
                        if (BT_POWER_ON_NORMAL == bt_power_on_get_config_type() || BT_POWER_ON_DUT == bt_power_on_get_config_type()) {
                            /* Start from power on initialization, we don't need to modify the state for AIR_TILE_ENABLE */
                            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT, (void *)true, 0,
                                            NULL, 0);
                            apps_config_set_vp(VP_INDEX_POWER_ON, false, 0, VOICE_PROMPT_PRIO_HIGH, false, NULL);
                            apps_config_set_foreground_led_pattern(LED_INDEX_POWER_ON, 30, false);
                            if (temp_state == APP_BATTERY_STATE_LOW_CAP) {
                                /* Play low battery VP if BT is not in classic off mode */
                                apps_config_set_vp(VP_INDEX_LOW_BATTERY, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                            }
                        }
                    }
                }
            }

            s_battery_context.state = temp_state;
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            /* Register callback for power_saving. */
            app_power_save_utils_register_get_mode_callback(app_battery_get_power_saving_target_mode);
#endif
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            APPS_LOG_MSGID_I(LOG_TAG" destroy", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            APPS_LOG_MSGID_I(LOG_TAG" resume", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            APPS_LOG_MSGID_I(LOG_TAG" pause", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
            APPS_LOG_MSGID_I(LOG_TAG" refresh", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
            APPS_LOG_MSGID_I(LOG_TAG" result", 0);
            break;
        default:
            break;
    }
    return ret;
}

static bool _proc_battery_event_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    app_battery_state_t bat_state;
    app_battery_state_t old_state;
    switch (event_id) {
        /* Handle battery_percent changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_PERCENT_CHANGE:
            s_battery_context.battery_percent = (int32_t)extra_data;
            APPS_LOG_MSGID_I(LOG_TAG" Current capacity : %d", 1, (int32_t)extra_data);
            break;
        /* Handle charger_state changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_CHARGER_STATE_CHANGE:
            if (CHARGER_STATE_CHR_OFF == (int32_t)extra_data && s_battery_context.charger_exist_state) {
                /* When charger out, to avoid the state become charging full, */
                /* Need keep the state of charging_state util timeout. */
                APPS_LOG_MSGID_I(LOG_TAG" Temporarily keep the charging_state to CHARGER_STATE_CHR_OFF", 0);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                    EVENT_GROUP_UI_SHELL_BATTERY,
                                    APPS_EVENTS_BATTERY_CHARGER_FULL_CHANGE,
                                    NULL, 0, NULL, 2000);
            } else {
                s_battery_context.charging_state = (int32_t)extra_data;
                /* To remove the event sending in case APPS_EVENTS_BATTERY_CHARGER_EXIST_CHANGE. */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_BATTERY,
                                      APPS_EVENTS_BATTERY_CHARGER_FULL_CHANGE);
            }
            APPS_LOG_MSGID_I(LOG_TAG" Current charging_state : %d", 1, s_battery_context.charging_state);
            break;
        /* Handle charger_exist_state changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_CHARGER_EXIST_CHANGE: {
            int32_t old_charger_exist_state = s_battery_context.charger_exist_state;
            s_battery_context.charger_exist_state = (int32_t)extra_data;
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
            if (s_battery_context.charger_exist_state && !old_charger_exist_state) {
                prompt_control_set_mute(true);
            } else if (!s_battery_context.charger_exist_state && old_charger_exist_state) {
                prompt_control_set_mute(false);
            }
#endif
            if (s_battery_context.charger_exist_state && !old_charger_exist_state) {
                if (CHARGER_STATE_CHR_OFF == s_battery_context.charging_state) {
                    /* When charger in, to avoid the state become charging full, */
                    /* Need set charge_state to CHARGER_STATE_FASTCC. */
                    s_battery_context.charging_state = CHARGER_STATE_FASTCC;
                    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                        EVENT_GROUP_UI_SHELL_BATTERY,
                                        APPS_EVENTS_BATTERY_CHARGER_FULL_CHANGE,
                                        NULL, 0, NULL, 2000);
                    APPS_LOG_MSGID_I(LOG_TAG" Charger exist, but need wait APPS_EVENTS_BATTERY_CHARGER_STATE_CHANGE", 0);
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG" Charger exist, And the charging_state = %d", 1, s_battery_context.charging_state);
                }
            } else if (!s_battery_context.charger_exist_state
                       && CHARGER_STATE_CHR_OFF != s_battery_context.charging_state) {
                /* To remove the event sending in case APPS_EVENTS_BATTERY_CHARGER_STATE_CHANGE. */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_BATTERY,
                                      APPS_EVENTS_BATTERY_CHARGER_FULL_CHANGE);
                s_battery_context.charging_state = CHARGER_STATE_CHR_OFF;
                APPS_LOG_MSGID_I(LOG_TAG" Set charging_state to CHARGER_STATE_CHR_OFF when charger out", 0);
            }
            s_battery_context.battery_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
            s_battery_context.shutdown_state = calculate_shutdown_state(battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE));
            APPS_LOG_MSGID_I(LOG_TAG" Current charger_exist_state : %d", 1, s_battery_context.charger_exist_state);

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            app_power_save_utils_notify_mode_changed(FALSE, app_battery_get_power_saving_target_mode);
#endif
            break;
        }
        /* Handle shutdown_state changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_SHUTDOWN_STATE_CHANGE:
            s_battery_context.shutdown_state = (battery_event_shutdown_state_t)extra_data;
            break;
        /* Handle shutdown_state changed, event from battery app. */
        case APPS_EVENTS_BATTERY_CHARGER_FULL_CHANGE:
            APPS_LOG_MSGID_I(LOG_TAG" Received APPS_EVENTS_BATTERY_CHARGER_FULL_CHANGE delay message, set charging_state to CHARGER_STATE_CHR_OFF", 0);
            s_battery_context.charging_state = CHARGER_STATE_CHR_OFF;
            break;
        default:
            APPS_LOG_MSGID_I(LOG_TAG" Doesn't care battery event: %d", 1, event_id);
            break;
    }

    bat_state = _get_battery_state(self);
    old_state = s_battery_context.state;

#ifdef AIR_TILE_ENABLE
    const app_bt_state_service_status_t* bt_state_srv_status = app_bt_connection_service_get_current_status();
#endif
    if (bat_state == APP_BATTERY_STATE_LOW_CAP
#ifdef AIR_TILE_ENABLE
        && bt_state_srv_status != NULL && bt_state_srv_status->current_power_state != APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLED
        /* Play low battery VP if BT is not in classic off mode */
#endif
    ) {
        apps_config_set_vp(VP_INDEX_LOW_BATTERY, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
    } else if (bat_state == old_state) {
        /* Do nothing when battery state not changed. */
    } else if ((bat_state == APP_BATTERY_STATE_SHUTDOWN)
               && old_state != APP_BATTERY_STATE_SHUTDOWN) {
        APPS_LOG_MSGID_I(LOG_TAG" Start Power Off", 0);
    } else if (bat_state == APP_BATTERY_STATE_CHARGING_FULL || old_state == APP_BATTERY_STATE_CHARGING)  {
        apps_config_set_foreground_led_pattern(LED_INDEX_CHARGING_FULL, 50, false);
    }

#ifdef APPS_DISABLE_BT_WHEN_CHARGING
    if (BT_POWER_ON_NORMAL == bt_power_on_get_config_type() || BT_POWER_ON_DUT == bt_power_on_get_config_type()) {
        /* If support charger case, battery APP should suspend ANC and disable BT when charger in. */
        if (APP_BATTERY_STATE_CHARGING <= bat_state
            && APP_BATTERY_STATE_CHARGING > s_battery_context.state) {
#ifdef MTK_ANC_ENABLE
            bool anc_ret = app_anc_service_suspend();
            APPS_LOG_MSGID_I(LOG_TAG" audio_anc_suspend after charger in ret = %d", 1, anc_ret);
#endif
            APPS_LOG_MSGID_I(LOG_TAG" send request to do APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT, enable : %d", 1, false);
            apps_config_set_foreground_led_pattern(LED_INDEX_POWER_OFF, 30, false);
#ifdef AIR_TILE_ENABLE
            if (app_tile_tmd_is_active())
            {
                ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF, NULL, 0, NULL, 0);
                /* Notify fast pair to stop ble adv after 3 sec */
                ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_STOP_FAST_PAIR_ADV, NULL, 0, NULL, 3000);
            }
            else
#endif
            {
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT, (void *)false, 0,
                                NULL, 0);
            }
        } else if (APP_BATTERY_STATE_CHARGING > bat_state
                   && APP_BATTERY_STATE_CHARGING <= s_battery_context.state) {
            /* If support charger case, battery APP should resume ANC and enable BT when charger out. */
#ifdef MTK_ANC_ENABLE
            bool anc_ret = app_anc_service_resume();
            APPS_LOG_MSGID_I(LOG_TAG" audio_anc_resume after charger out ret = %d", 1, anc_ret);
#endif
            apps_config_set_vp(VP_INDEX_POWER_ON, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            apps_config_set_foreground_led_pattern(LED_INDEX_POWER_ON, 30, false);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT, (void *)true, 0,
                                NULL, 0);
        }
    }
#endif

    s_battery_context.state = bat_state;

    if (APP_BATTERY_STATE_SHUTDOWN == s_battery_context.state) {
        /* Power off device when battery state is APP_BATTERY_STATE_SHUTDOWN. */
        _shutdown_when_low_battery(self);
    } else {
#if defined(MTK_AWS_MCE_ENABLE)
        /* For Partner, notify battery level to Agent. For Agent, check and do RHO. */
        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
            partner_notify_battery_level_to_agent(self);
        } else if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
#ifdef APPS_AUTO_TRIGGER_RHO
            if (apps_config_features_is_auto_rho_enabled()) {
                check_and_do_rho(self);
            }
#endif
        }
#endif
    }

    return ret;
}

static bool battery_app_bt_cm_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
#if defined(MTK_AWS_MCE_ENABLE)
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    /* Handle new BT_CM Event. */
    switch (event_id) {
#if defined(MTK_AWS_MCE_ENABLE)
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == remote_update) {
                break;
            }

            if (BT_AWS_MCE_ROLE_PARTNER == role) {
                /* Check Partner AWS connection, then notify battery to Agent. */
                if (!(remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                    && (remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                    s_battery_context.aws_state = BT_AWS_MCE_AGENT_STATE_ATTACHED;
                    partner_notify_battery_level_to_agent(self);
                } else if ((remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                           && !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                    s_battery_context.aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
                }
            } else if (BT_AWS_MCE_ROLE_AGENT == role) {
                /* Check Agent AWS connection and set AWS state. */
                if (!(remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                    && (remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                    s_battery_context.partner_battery_percent = PARTNER_BATTERY_INVALID;
                    s_battery_context.aws_state = BT_AWS_MCE_AGENT_STATE_ATTACHED;
                } else if ((remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                           && !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                    s_battery_context.aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
                }
            }
        }
        break;
#endif
        default:
            break;
    }
    return ret;
}

#if defined(MTK_AWS_MCE_ENABLE)
static bool _proc_aws_report_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    /* Agent received Partner battery via AWS_MCE. */
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_BATTERY
        && bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
        configASSERT(aws_data_ind->param_len == sizeof(s_battery_context.partner_battery_percent));
        memcpy(&s_battery_context.partner_battery_percent, aws_data_ind->param, sizeof(s_battery_context.partner_battery_percent));
        APPS_LOG_MSGID_I(LOG_TAG" Received partner battery = %d", 1, s_battery_context.partner_battery_percent);
#ifdef APPS_AUTO_TRIGGER_RHO
        if (apps_config_features_is_auto_rho_enabled()) {
            /* Check RHO when Agent received Partner battery. */
            check_and_do_rho(self);
        }
#endif
    }
    return ret;
}
#endif

static bool app_battery_proc_usb_audio_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        default:
            break;
    }

    return ret;
}


static bool _app_interaction_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        /* The old Agent will switch to new Partner if RHO successfully. */
        case APPS_EVENTS_INTERACTION_RHO_END:
            APPS_LOG_MSGID_I(LOG_TAG" battery received RHO end", 0);
            {
                app_rho_result_t rho_ret = (app_rho_result_t)extra_data;
                if (APP_RHO_RESULT_SUCCESS == rho_ret) {
                    /* Set Partner battery percent to invalid value when Agent switched to Partner. */
                    s_battery_context.partner_battery_percent = PARTNER_BATTERY_INVALID;
                    APPS_LOG_MSGID_I(LOG_TAG" RHO success", 0);
                }
            }
            break;
#endif
        default:
            APPS_LOG_MSGID_I(LOG_TAG" Not supported event id = %d", 1, event_id);
            break;
    }

    return ret;
}

bool app_battery_idle_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell battery events - handle battery_change/charger_state/shutdown_state from battery event. */
        case EVENT_GROUP_UI_SHELL_BATTERY:
            ret = _proc_battery_event_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell new BT_CM events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = battery_app_bt_cm_event_proc(self, event_id, extra_data, data_len);
            break;
        /* UI Shell APP_INTERACTION events. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = _app_interaction_event_proc(self, event_id, extra_data, data_len);
            break;
#if defined(MTK_AWS_MCE_ENABLE)
        /* UI Shell BT AWS_DATA events. */
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = _proc_aws_report_group(self, event_id, extra_data, data_len);
            break;
#endif
        case EVENT_GROUP_UI_SHELL_USB_AUDIO:
            ret = app_battery_proc_usb_audio_group(self, event_id, extra_data, data_len);
            break;
        default:
            break;
    }
    return ret;
}

