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
 * File: app_smcharger_utils.c
 *
 * Description: This file provides many utility function for SmartCharger APP.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for SmartCharger state machine.
 *
 */


#ifdef MTK_SMART_CHARGER_ENABLE

/* Enable SmartCharger APP AT-CMD for test. */
#define MTK_SMART_CHARGER_TEST

#include "app_smcharger_utils.h"
#include "apps_aws_sync_event.h"
#include "apps_config_event_list.h"
#include "apps_events_key_event.h"
#include "app_home_screen_idle_activity.h"
#include "battery_management.h"
#include "hal_pmu.h"
#include "prompt_control.h"
#include "smartcharger.h"
#include "smartcase.h"
#include "bt_power_on_config.h"
#include "bt_sink_srv_ami.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "timers.h"

#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif

#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_le_audio_util.h"
#include "bt_sink_srv_le_volume.h"
#endif

#ifdef MTK_VA_XIAOAI_ENABLE
#include "xiaoai.h"
#endif
#ifdef AIR_TILE_ENABLE
#include "app_tile.h"
#include "app_bt_state_service.h"
#endif

/* SmartCharger APP must use Battery Manager module. */
#if !defined(MTK_BATTERY_MANAGEMENT_ENABLE)
#error "For SmartCharger feature, please enable MTK_BATTERY_MANAGEMENT_ENABLE"
#endif
/* SmartCharger APP must use AWS_MCE module. */
#if !defined(MTK_AWS_MCE_ENABLE)
#error "For SmartCharger feature, please enable MTK_AWS_MCE_ENABLE"
#endif

#define SMCHARGER_UTILS "[SMCharger]Util"

#ifdef MTK_SMART_CHARGER_TEST
#include "atci.h"

/* Define SmartCharger APP AT-CMD structure. */
static atci_status_t smcharger_atci_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_cmd_hdlr_item_t smcharger_atci_cmd[] = {
    {
        .command_head = "AT+SMC",
        .command_hdlr = smcharger_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};
#endif

/* SmartCharger APP global context. */
static app_smcharger_context_t *g_smcharger_context = NULL;
static app_smcharger_action_status_t app_smcharger_switch_bt(bool on);

#define SMCHARGER_OUT_TIMER_NAME                "out_timer"
#define SMCHARGER_OUT_TIMER_INTERVAL            3000
static  TimerHandle_t                           g_smcharger_out_timer;

/**
* @brief      This function is send internal event for SmartCharger state machine.
* @param[in]  event_id, SmartCharger APP event.
* @param[in]  data, SmartCharger APP data pointer.
* @param[in]  data_len, data length.
* @return     If return true, the event send successfully, otherwise failure.
*/
bool app_smcharger_ui_shell_event(int event_id, void *data, size_t data_len)
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", send smcharger event_id=%d", 1, event_id);
    ui_shell_status_t ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_CHARGER_CASE,
                                                event_id, data, data_len, NULL, 0);
    if (ret != UI_SHELL_STATUS_OK) {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS", send smcharger event fail", 0);
    }
    return (ret == UI_SHELL_STATUS_OK);
}


static void app_smcharger_out_timer_callback(TimerHandle_t xTimer)
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS" out_timer_callback", 0);
    app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_CHARGER_OUT_PATTERN, NULL, 0);
}

static void app_smcharger_out_timer_start()
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS" out_timer_start, timer=0x%08X", 1, g_smcharger_out_timer);
    if (g_smcharger_out_timer == NULL) {
        g_smcharger_out_timer = xTimerCreate(SMCHARGER_OUT_TIMER_NAME,
                                             SMCHARGER_OUT_TIMER_INTERVAL / portTICK_PERIOD_MS,
                                             pdFALSE, NULL, app_smcharger_out_timer_callback);
    }
    if (g_smcharger_out_timer != NULL) {
        xTimerStart(g_smcharger_out_timer, 0);
        APPS_LOG_MSGID_I(SMCHARGER_UTILS" out_timer_start, done", 0);
    }
}

static void app_smcharger_out_timer_stop()
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS" out_timer_stop, timer=0x%08X", 1, g_smcharger_out_timer);
    if (g_smcharger_out_timer != NULL
        && xTimerIsTimerActive(g_smcharger_out_timer) == pdTRUE) {
        xTimerStop(g_smcharger_out_timer, 0);
        xTimerDelete(g_smcharger_out_timer, 0);
        g_smcharger_out_timer = NULL;
        APPS_LOG_MSGID_I(SMCHARGER_UTILS" out_timer_stop, done", 0);
    }
}

/**
* @brief      SmartCharger APP send public event for other APP.
* @param[in]  action, SmartCharger action.
* @param[in]  data, SmartCharger action data.
*/
static void app_smcharger_ui_shell_public_event(app_smcharger_action_t action, uint8_t data)
{
    app_smcharger_public_event_para_t *event_para = (app_smcharger_public_event_para_t *)
                                                    pvPortMalloc(sizeof(app_smcharger_public_event_para_t));
    if (event_para == NULL) {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS" shell_public_event, malloc fail", 0);
        return;
    }

    event_para->action = action;
    event_para->data = data;
    bool ret = app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT,
                                            (void *)event_para, sizeof(app_smcharger_public_event_para_t));
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", shell_public_event action=%d data=%d ret=%d", 3,
                     action, data, ret);

    // Feature: First, turn off BT Classic when LID_OFF action
#if 1
    if (action == SMCHARGER_CLOSE_LID_ACTION) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                              APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT);
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF, NULL, 0, NULL, 0);
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", shell_public_event CLOSE -> Classic BT OFF", 0);
    } else if (action == SMCHARGER_OPEN_LID_ACTION) {
        app_smcharger_switch_bt(TRUE);
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", shell_public_event OPEN -> BT ON", 0);
    }
#endif
}

static const char *app_smcharger_state_string(int state)
{
    const char *state_str = "NULL";
    switch (state) {
        case STATE_SMCHARGER_STARTUP:
            state_str = "STARTUP";
            break;
        case STATE_SMCHARGER_LID_CLOSE:
            state_str = "LID_CLOSE";
            break;
        case STATE_SMCHARGER_LID_OPEN:
            state_str = "LID_OPEN";
            break;
        case STATE_SMCHARGER_OUT_OF_CASE:
            state_str = "OUT_OF_CASE";
            break;
        case STATE_SMCHARGER_OFF:
            state_str = "OFF";
            break;
    }
    return state_str;
}

#define SMCHARGER_DATA_MASK_B01        0x03     /* 0b0000 0011. */
#define SMCHARGER_DATA_MASK_B234       0x1C     /* 0b0001 1100. */
#define SMCHARGER_DATA_MASK_B567       0xE0     /* 0b1110 0000. */

/**
* @brief      SmartCharger APP handle case battery report.
* @param[in]  pattern_data, data from SmartCharger case.
*/
static void app_smcharger_handle_case_battery_report(uint32_t pattern_data)
{
    /* Check "case_battery_percent" data mask. */
#if 0
    uint8_t mask = SMCHARGER_DATA_MASK_B01 + SMCHARGER_DATA_MASK_B234;
#else
    uint8_t mask = SMCHARGER_DATA_MASK_B01 + SMCHARGER_DATA_MASK_B234 + SMCHARGER_DATA_MASK_B567;
#endif
    if ((pattern_data & ~mask) != 0 || pattern_data == 0) {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS", [DRV]callback, error case_battery=%d", 1, pattern_data);
        return;
    }
    // case_battery_percent include bit7(charging_flag)
    // cannot be 0, because case low battery will send charger_off -> earbuds power off
    uint8_t case_battery_percent = pattern_data;
#if 0
    /* "case_battery_percent" should be 100 if pattern_data=31 (0b11111). */
    if (case_battery_percent == 31) {
        case_battery_percent = 100;
    } else {
        case_battery_percent = (uint8_t)(case_battery_percent * 3.22);
    }
#endif
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", [DRV]callback, case_battery_percent %d->%d",
                     2,
                     (g_smcharger_context != NULL ? g_smcharger_context->case_battery_percent : 0),
                     case_battery_percent);

    /* Update SmartCharger APP context->case_battery_percent and send internal event. */
    if (g_smcharger_context != NULL
        && g_smcharger_context->case_battery_percent != case_battery_percent
        && (case_battery_percent <= 100 || (case_battery_percent > 128 && case_battery_percent <= 228))) {
        g_smcharger_context->case_battery_percent = case_battery_percent;
        app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_CASE_BATTERY_REPORT, (void *)(uint32_t)case_battery_percent, 0);
    }
}

/**
* @brief      SmartCharger middleware callback function.
* @param[in]  drv_event, SmartCharger middleware event, see middleware/MTK/smart_charger/inc/smartcharger.h.
* @param[in]  pattern_data, data from SmartCharger case.
* @param[in]  data_len, pattern_data length.
*/
static void app_smcharger_driver_callback(uint8_t drv_event, uint32_t pattern_data, uint16_t data_len)
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", [DRV]callback drv_event=%d pattern_data=%d data_len=%d",
                     3, drv_event, pattern_data, data_len);
    bool need_send_event = FALSE;
    uint8_t user_data = 0;
    uint8_t mask = 0;
    int event_id = EVENT_ID_SMCHARGER_NONE;
    switch (drv_event) {
        /* SmartCharger middleware LID_OPEN Command. */
        case DRV_CHARGER_EVENT_LID_OPEN_PATTERN: {
            event_id = EVENT_ID_SMCHARGER_LID_OPEN_PATTERN;
            if (g_smcharger_context != NULL && g_smcharger_context->smcharger_state == STATE_SMCHARGER_LID_OPEN) {
                APPS_LOG_MSGID_I(SMCHARGER_UTILS", [DRV]callback, ignore LID_OPEN Pattern in LID_OPEN State", 0);
                /* For close_lid then open quickly, or device shake in the SmartCharger case (closed). */
                if (g_smcharger_context->case_lid_close_flag) {
                    APPS_LOG_MSGID_I(SMCHARGER_UTILS", [DRV]callback, ignore LID_OPEN, still send OPEN_LID_ACTION", 0);
                    app_smcharger_ui_shell_public_event(SMCHARGER_OPEN_LID_ACTION, 0);
                }
            } else {
                need_send_event = TRUE;
            }
            if (data_len > 0) {
                app_smcharger_handle_case_battery_report(pattern_data);
            }
            if (g_smcharger_context != NULL) {
                g_smcharger_context->case_lid_close_flag = FALSE;
            }
            break;
        }
        /* SmartCharger middleware LID_CLOSE Command - The LID_CLOSE command is sent after USER_DATA1 command and wait 3 sec. */
        case DRV_CHARGER_EVENT_LID_CLOSE_PATTERN: {
            app_smcharger_out_timer_stop();

            event_id = EVENT_ID_SMCHARGER_LID_CLOSE_PATTERN;
            if (g_smcharger_context != NULL && g_smcharger_context->smcharger_state == STATE_SMCHARGER_LID_CLOSE) {
                APPS_LOG_MSGID_I(SMCHARGER_UTILS", [DRV]callback, ignore LID_CLOSE Pattern in LID_CLOSE State", 0);
            } else {
                need_send_event = TRUE;
            }
            break;
        }
        /* SmartCharger middleware CHARGER_OFF Command. */
        case DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN: {
            app_smcharger_out_timer_stop();

            event_id = EVENT_ID_SMCHARGER_CHARGER_OFF_PATTERN;
            if (g_smcharger_context != NULL && g_smcharger_context->smcharger_state == STATE_SMCHARGER_OFF) {
                APPS_LOG_MSGID_I(SMCHARGER_UTILS", [DRV]callback, ignore OFF Pattern in OFF State", 0);
            } else {
                need_send_event = TRUE;
            }
            if (data_len > 0) {
                app_smcharger_handle_case_battery_report(pattern_data);
            }
            break;
        }
        /* SmartCharger middleware CHARGER_OUT Command. */
        case DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN: {
            if (g_smcharger_context != NULL
                && g_smcharger_context->smcharger_state == STATE_SMCHARGER_LID_OPEN
                && g_smcharger_context->case_lid_close_flag) {
                app_smcharger_out_timer_start();
                return;
            }

            event_id = EVENT_ID_SMCHARGER_CHARGER_OUT_PATTERN;
            need_send_event = TRUE;
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_CHARGER_CASE, event_id);
            break;
        }
        /* SmartCharger middleware CHARGER_KEY Command. */
        case DRV_CHARGER_EVENT_CHARGER_KEY_PATTERN: {
            app_smcharger_out_timer_stop();

            event_id = EVENT_ID_SMCHARGER_CHARGER_KEY_PATTERN;
            /* For Airoha 1-wire UART SmartCharger, pattern_data is 0 if long press the SmartCharger key 3 sec. */
            /* Pattern_data is 1 if long press the SmartCharger key 10 sec. */
            if (data_len == 8 && pattern_data <= 255) {
                user_data = pattern_data;
                need_send_event = TRUE;
            } else {
                APPS_LOG_MSGID_E(SMCHARGER_UTILS", [DRV]callback, error CHARGER_KEY data", 0);
            }
            break;
        }
        /* SmartCharger middleware USER1 data Command - start to close lid. */
        case DRV_CHARGER_EVENT_USER1_PATTERN: {
            event_id = EVENT_ID_SMCHARGER_USER_DATA1_PATTERN;
            mask = SMCHARGER_DATA_MASK_B01;
            if ((pattern_data & ~mask) == 0) {
                user_data = pattern_data;
                need_send_event = TRUE;
            } else {
                APPS_LOG_MSGID_E(SMCHARGER_UTILS", [DRV]callback, error USER_DATA1 data", 0);
            }
            if (g_smcharger_context != NULL) {
                g_smcharger_context->case_lid_close_flag = TRUE;
            }
            break;
        }
        /* SmartCharger middleware USER2 data Command. */
        case DRV_CHARGER_EVENT_USER2_PATTERN: {
            event_id = EVENT_ID_SMCHARGER_USER_DATA2_PATTERN;
            mask = SMCHARGER_DATA_MASK_B234 + SMCHARGER_DATA_MASK_B567;
            if ((pattern_data & ~mask) == 0) {
                user_data = pattern_data;
                need_send_event = TRUE;
            } else {
                APPS_LOG_MSGID_E(SMCHARGER_UTILS", [DRV]callback, error USER_DATA2 data", 0);
            }
            break;
        }
        /* SmartCharger middleware USER3 data Command. */
        case DRV_CHARGER_EVENT_USER3_PATTERN: {
            event_id = EVENT_ID_SMCHARGER_USER_DATA3_PATTERN;
            mask = SMCHARGER_DATA_MASK_B01 + SMCHARGER_DATA_MASK_B234 + SMCHARGER_DATA_MASK_B567;
            if ((pattern_data & ~mask) == 0) {
                user_data = pattern_data;
                need_send_event = TRUE;
            } else {
                APPS_LOG_MSGID_E(SMCHARGER_UTILS", [DRV]callback, error USER_DATA3 data", 0);
            }
            break;
        }
        /* SmartCharger middleware CHARGER_IN Command, i.e. battery_manager charger_exist is 1. */
        case DRV_CHARGER_EVENT_CHARGER_IN_PATTERN: {
            if (g_smcharger_context != NULL
                && g_smcharger_context->smcharger_state == STATE_SMCHARGER_LID_OPEN
                && g_smcharger_context->case_lid_close_flag) {
                app_smcharger_out_timer_stop();
                return;
            }

            event_id = EVENT_ID_SMCHARGER_CHARGER_IN_INTERRUPT;
            need_send_event = TRUE;
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_CHARGER_CASE, event_id);
            break;
        }
        default: {
            APPS_LOG_MSGID_E(SMCHARGER_UTILS", [DRV]callback, error event", 0);
            break;
        }
    }

    /* Need to send internal event when need_send_event is 1. */
    if (need_send_event) {
        app_smcharger_ui_shell_event(event_id, (void *)(uint32_t)user_data, 0);
    }
}

/**
* @brief      SmartCharger APP check system boot reason and send event.
*/
static app_smcharger_action_status_t app_smcharger_handle_system_boot()
{
    extern uint8_t pmu_get_power_on_reason();
    int32_t charger_exitst = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
    uint8_t reason = pmu_get_power_on_reason();
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", handle_system_boot reason=0x%02X charger_exitst=%d", 2, reason, charger_exitst);
    if (charger_exitst) {
        /* Send CHARGER_IN_BOOT event if bit[2] is 1 (STS_CHRIN). */
        app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_CHARGER_IN_BOOT, NULL, 0);
    } else {
        /* Send POWER_KEY_BOOT event if bit[0] is 1 (STS_PWRKEY) or other. */
        app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_POWER_KEY_BOOT, NULL, 0);
        /* Play "power_on" VP and foreground led pattern if device power on in out_of_case state. */
        apps_config_set_vp(VP_INDEX_POWER_ON, FALSE, 0, VOICE_PROMPT_PRIO_HIGH, FALSE, NULL);
        apps_config_set_foreground_led_pattern(LED_INDEX_POWER_ON, 30, FALSE);
    }
    return APP_SMCHARGER_OK;
}

static void app_smcharger_send_key_action(uint16_t action)
{
    uint16_t *key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
    if (key_action != NULL) {
        *key_action = action;
        ui_shell_status_t ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                                                    EVENT_GROUP_UI_SHELL_KEY,
                                                    INVALID_KEY_EVENT_ID,
                                                    key_action, sizeof(uint16_t),
                                                    NULL, 0);
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", send_key_action %d ret=%d", 2, action, ret);
    } else {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS", send_key_action malloc fail", 0);
    }
}

/**
* @brief      SmartCharger APP enable BT discoverable mode via SmartCharger Case Key
*/
static void app_smcharger_bt_enter_discoverable()
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", [%02X] bt_enter_discoverable", 1, role);
    if (role == BT_AWS_MCE_ROLE_AGENT) {
        app_smcharger_send_key_action(KEY_DISCOVERABLE);
    }
}

/**
* @brief      This function is used to clear and enter discoverable.
* @param[in]  step, SmartCharger BT clear step.
*/
void app_smcharger_bt_clear_enter_discoverable(app_smcharger_clear_bt_step_t step)
{
    if (g_smcharger_context == NULL || !g_smcharger_context->bt_clear_ongoing) {
        return;
    }

    bt_cm_power_state_t power_state = bt_cm_power_get_state();
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", BT_CLEAR power_state=%d step=%d",
                     2, power_state, step);

    if (step == APP_SMCHARGER_REQUEST_BT_OFF) {
        if (power_state == BT_CM_POWER_STATE_OFF) {
            step = APP_SMCHARGER_REQUEST_BT_CLEAR_ALL;
        } else if (power_state == BT_CM_POWER_STATE_OFF_PENDING) {
            step = APP_SMCHARGER_REQUEST_NONE;
        }
    }

    if (step == APP_SMCHARGER_REQUEST_NONE) {
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", BT_CLEAR - no action", 0);
    } else if (step == APP_SMCHARGER_REQUEST_BT_OFF) {
        app_smcharger_switch_bt(FALSE);
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", BT_CLEAR - OFF action", 0);
    } else if (step == APP_SMCHARGER_REQUEST_BT_CLEAR_ALL) {
        bt_status_t status = bt_device_manager_unpair_all();
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", BT_CLEAR - unpair_all=%d", 1, status);
        if (status == BT_STATUS_SUCCESS) {
            app_smcharger_bt_clear_enter_discoverable(APP_SMCHARGER_REQUEST_BT_ON);
        } else {
            APPS_LOG_MSGID_E(SMCHARGER_UTILS", BT_CLEAR error", 0);
            g_smcharger_context->bt_clear_ongoing = FALSE;
        }
    } else if (step == APP_SMCHARGER_REQUEST_BT_ON) {
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", BT_CLEAR - ON action", 0);
        app_smcharger_switch_bt(TRUE);
    } else if (step == APP_SMCHARGER_REQUEST_BT_DISCOVERABLE) {
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", BT_CLEAR - discoverable action", 0);
        app_smcharger_bt_enter_discoverable();
        g_smcharger_context->bt_clear_ongoing = FALSE;
    }
}

/**
* @brief      SmartCharger APP enable BT air pairing mode via SmartCharger Case Key.
*/
static void app_smcharger_bt_air_pairing()
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", [%02X] bt_air_pairing", 1, role);
    app_smcharger_send_key_action(KEY_AIR_PAIRING);
}

/**
* @brief      SmartCharger APP power on/off BT.
* @param[in]  on, TRUE - power on BT, FALSE - power off BT.
* @return     If return APP_SMCHARGER_OK, run successfully.
*/
static app_smcharger_action_status_t app_smcharger_switch_bt(bool on)
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", switch_bt on=%d", 1, on);

    uint32_t delay_ms = 0;
#if defined(MTK_VA_XIAOAI_ENABLE) || defined(__BT_FAST_PAIR_ENABLE__)
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                          APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT);
    if (on) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                              APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF);
    } else {
        delay_ms = 7 * 1000;
    }
#endif

    ui_shell_status_t ret = UI_SHELL_STATUS_OK;
    if (BT_POWER_ON_NORMAL == bt_power_on_get_config_type()
        || BT_POWER_ON_DUT == bt_power_on_get_config_type()) {
#ifdef AIR_TILE_ENABLE
        /* If tile is activated, earbuds should enter classic bt mode */
        if (on) {
            ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                  EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                  APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT,
                                  (void *)on, 0, NULL, delay_ms);
        } else {
            if (!app_tile_tmd_is_active()) {
                ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                  EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                  APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT,
                                  (void *)on, 0, NULL, delay_ms);
            } else {
                ret = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF, NULL, 0, NULL, 0);
                /* Notify fast pair to stop ble adv after 3 sec */
                ret = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_STOP_FAST_PAIR_ADV, NULL, 0, NULL, 3000);
            }
        }
#else
        /* Only send event to HomeScreen APP, no need to care BT current state. */
        ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                  EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                  APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT,
                                  (void *)on, 0, NULL, delay_ms);
#endif
    }
    return (ret == UI_SHELL_STATUS_OK ? APP_SMCHARGER_OK : APP_SMCHARGER_FAILURE);
}

/**
* @brief      SmartCharger APP mute/unmute audio, include BT Audio and VP.
* @param[in]  is_mute, TRUE - mute audio, FALSE - unmute audio.
* @return     If return APP_SMCHARGER_OK, run successfully.
*/
static app_smcharger_action_status_t app_smcharger_mute_audio(bool is_mute)
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", mute_audio mute=%d", 1, is_mute);
    bt_status_t status = bt_sink_srv_music_set_mute(is_mute);
#ifdef AIR_LE_AUDIO_ENABLE
    bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, is_mute);
#endif
    prompt_control_set_mute(is_mute);
    return (status == BT_STATUS_SUCCESS ? APP_SMCHARGER_OK : APP_SMCHARGER_FAILURE);
}

/**
* @brief      SmartCharger APP send public_event with action.
* @param[in]  old_state, old SmartCharger APP state.
* @param[in]  state, new SmartCharger APP state.
*/
static void app_smcharger_notify_state_to_app(int old_state, int state)
{
    uint32_t action = 0xFF;
    if (old_state == STATE_SMCHARGER_OUT_OF_CASE && state == STATE_SMCHARGER_LID_OPEN) {
        action = SMCHARGER_CHARGER_IN_ACTION;
    } else if (old_state == STATE_SMCHARGER_LID_CLOSE && state == STATE_SMCHARGER_OFF) {
        if (g_smcharger_context != NULL && g_smcharger_context->battery_percent == 100) {
            action = SMCHARGER_CHARGER_COMPLETE_ACTION;
        }
    } else if (state == STATE_SMCHARGER_LID_OPEN) {
        /* Two case: 1. LID_CLOSE switch to LID_OPEN. */
        /* 2. STARTUP switch to LID_OPEN (power_off device then charger in, device reboot in the lid_open case, open lid after device power_off in lid_close case). */
        action = SMCHARGER_OPEN_LID_ACTION;
    } else if (old_state == STATE_SMCHARGER_STARTUP && state == STATE_SMCHARGER_LID_CLOSE) {
        /* STARTUP->LID_CLOSE for "device boot slowly then close lid quickly". */
        action = SMCHARGER_CLOSE_LID_ACTION;
    } else if (old_state == STATE_SMCHARGER_LID_OPEN && state == STATE_SMCHARGER_LID_CLOSE) {
        action = SMCHARGER_CLOSE_LID_COMPLETE_ACTION;
    } else if (state == STATE_SMCHARGER_OUT_OF_CASE) {
        action = SMCHARGER_CHARGER_OUT_ACTION;
    }
    if (action != 0xFF) {
        app_smcharger_ui_shell_public_event(action, 0);
    }
}

app_smcharger_action_status_t app_smcharger_power_off(bool normal_off)
{
    /* Enter app_smcharger_power_off with following conditions
       1. app_smcharger_idle_activity.c detect battery state is SMCHARGER_BATTERY_STATE_SHUTDOWN
       2. app_smcharger_out_of_case_activity.c detect battery state is SMCHARGER_BATTERY_STATE_SHUTDOWN
       3. app_smcharger_utils.c detect smart charger state is STATE_SMCHARGER_OFF
     */
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", power_off %d", 1, normal_off);
    if (normal_off) {
        /* LOW_VOLTAGE power off outside of SmartCharger case, play "power off" VP and foreground led pattern. */
        apps_config_set_vp(VP_INDEX_POWER_OFF, FALSE, 0, VOICE_PROMPT_PRIO_EXTREME, TRUE, app_home_screen_power_off_vp_play_callback);
        apps_config_set_foreground_led_pattern(LED_INDEX_POWER_OFF, 30, FALSE);
    }
#ifdef AIR_TILE_ENABLE
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;
    if (normal_off) {
        /* for normall_off is low battery state, tile should stop ble adv and enter power off mode */
        ret = ui_shell_send_event(FALSE,
                                  EVENT_PRIORITY_HIGNEST,
                                  EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                  (normal_off ? APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF
                                  : APPS_EVENTS_INTERACTION_REQUEST_IMMEDIATELY_POWER_OFF),
                                  NULL, 0, NULL, 0);
    } else {
        /* smart charger state is STATE_SMCHARGER_OFF, it should be classic bt off state */
        const app_bt_state_service_status_t* bt_state_srv_status = app_bt_connection_service_get_current_status();
        if (bt_state_srv_status != NULL) {
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", current power state %d", 1, bt_state_srv_status->current_power_state);
        }
    }
#else
    ui_shell_status_t ret = ui_shell_send_event(FALSE,
                                                EVENT_PRIORITY_HIGNEST,
                                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                (normal_off ? APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF
                                                 : APPS_EVENTS_INTERACTION_REQUEST_IMMEDIATELY_POWER_OFF),
                                                NULL, 0, NULL, 0);
#endif
    return (ret == UI_SHELL_STATUS_OK ? APP_SMCHARGER_OK : APP_SMCHARGER_FAILURE);
}

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
/**
* @brief      need_power_saving when SmartCharger APP enable, two cases:
*             1. Single device (AWS no connected) + out_of_case state
*             2. Double device (AWS connected) + Agent + both out_of_case state
* @return     If return APP_POWER_SAVING_TARGET_MODE_BT_OFF, need to power_saving via power off device,
*             otherwise no need to power_saving.
*/
app_power_saving_target_mode_t app_smcharger_get_power_saving_target_mode(void)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bool need_power_saving = FALSE;
    if (g_smcharger_context != NULL) {
        if (g_smcharger_context->aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED
            && g_smcharger_context->smcharger_state == STATE_SMCHARGER_OUT_OF_CASE) {
            need_power_saving = TRUE;
        } else if (g_smcharger_context->aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED
                   && role == BT_AWS_MCE_ROLE_AGENT
                   && g_smcharger_context->smcharger_state == STATE_SMCHARGER_OUT_OF_CASE
                   && g_smcharger_context->peer_smcharger_state == STATE_SMCHARGER_OUT_OF_CASE) {
            need_power_saving = TRUE;
        }
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", [0x%02X] aws=0x%02X need_power_saving=%d",
                         3, role, g_smcharger_context->aws_state, need_power_saving);
    }

    if (need_power_saving) {
        return APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
    } else {
        return APP_POWER_SAVING_TARGET_MODE_NORMAL;
    }
}
#endif

app_smcharger_action_status_t app_smcharger_state_do_action(int state)
{
    if (g_smcharger_context == NULL) {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS", app_smcharger do_action fail, invalid context", 0);
        return APP_SMCHARGER_FAILURE;
    }
    int old_state = g_smcharger_context->smcharger_state;

    /* SmartCharger APP send IN_OUT_EVENT. */
    if (state == STATE_SMCHARGER_OUT_OF_CASE
            || (old_state != STATE_SMCHARGER_LID_CLOSE && state == STATE_SMCHARGER_LID_OPEN)) {
        app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_IN_OUT_EVENT, NULL, 0);
    }

    g_smcharger_context->smcharger_state = state;
    /* Print state switch log for Auto-Test. */
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", app_smcharger %d -> %d action", 2, old_state, state);

    /* Sync smcharger_state to peer. */
    if (state == STATE_SMCHARGER_STARTUP
            || state == STATE_SMCHARGER_OUT_OF_CASE
            || state == STATE_SMCHARGER_LID_OPEN
            || state == STATE_SMCHARGER_LID_CLOSE
            || state == STATE_SMCHARGER_OFF) {
        app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_SYNC_STATE, NULL, 0);
    }

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
    /* Notify Power_saving APP mode changed, then determine "is_need_power_saving". */
    if (state == STATE_SMCHARGER_OUT_OF_CASE
        || state == STATE_SMCHARGER_LID_OPEN
        || state == STATE_SMCHARGER_LID_CLOSE) {
        app_power_save_utils_notify_mode_changed(FALSE, app_smcharger_get_power_saving_target_mode);
    }
#endif

    /* SmartCharger APP send public_event with action. */
    app_smcharger_notify_state_to_app(old_state, state);

    /* See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for SmartCharger state machine. */
    app_smcharger_action_status_t status = APP_SMCHARGER_OK;
    switch (state) {
        case STATE_SMCHARGER_STARTUP: {
            status += app_smcharger_handle_system_boot();
            break;
        }
        case STATE_SMCHARGER_LID_CLOSE: {
            status += app_smcharger_mute_audio(TRUE);
            status += app_smcharger_switch_bt(FALSE);
            break;
        }
        case STATE_SMCHARGER_LID_OPEN: {
            if (old_state == STATE_SMCHARGER_STARTUP) {
                if (BT_POWER_ON_DUT == bt_power_on_get_config_type()) {
                    APPS_LOG_MSGID_I(SMCHARGER_UTILS", BT_POWER_ON_DUT, No switch bt on", 0);
                } else {
                    status += app_smcharger_switch_bt(TRUE);
                }
            } else if (old_state == STATE_SMCHARGER_LID_CLOSE) {
                status += app_smcharger_switch_bt(TRUE);
            }
            status += app_smcharger_mute_audio(TRUE);
#ifdef MTK_ANC_ENABLE
            /* Suspend ANC when device in the SmartCharger case. */
            bool anc_ret = app_anc_service_suspend();
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", audio_anc_suspend ret=%d", 1, anc_ret);
#endif
            break;
        }
        case STATE_SMCHARGER_OUT_OF_CASE: {
            status += app_smcharger_switch_bt(TRUE);
            status += app_smcharger_mute_audio(FALSE);
#ifdef MTK_ANC_ENABLE
            /* Resume ANC when device outside of the SmartCharger case. */
            bool anc_ret = app_anc_service_resume();
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", audio_anc_resume ret=%d", 1, anc_ret);
#endif
            break;
        }
        case STATE_SMCHARGER_OFF: {
            status += app_smcharger_power_off(FALSE);
            break;
        }
        default: {
            configASSERT(0);
            break;
        }
    }
    if (status != APP_SMCHARGER_OK) {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS", app_smcharger do_action fail %d", 1, status);
    }
    return status;
}

/**
* @brief      This function is init SmartCharger APP.
*/
void app_smcharger_init()
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", app_smcharger_init", 0);
    /* Register callback to SmartCharger middleware. */
    DrvCharger_RegisterSmartCase(app_smcharger_driver_callback);
#ifdef MTK_SMART_CHARGER_TEST
    /* Register AT CMD handler for SmartCharger Test. */
    atci_status_t ret = atci_register_handler(smcharger_atci_cmd, sizeof(smcharger_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", app_smcharger_init atci ret=%d", 1, ret);
#endif

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
    /* Register callback for power_saving. */
    app_power_save_utils_register_get_mode_callback(app_smcharger_get_power_saving_target_mode);
#endif
}

void app_smcharger_set_context(app_smcharger_context_t *smcharger_ctx)
{
    g_smcharger_context = smcharger_ctx;
}

app_smcharger_context_t *app_smcharger_get_context()
{
    return g_smcharger_context;
}

bool app_smcharger_show_led_bg_pattern()
{
    app_smcharger_battery_state_t battery_state = (g_smcharger_context != NULL ?
                                         g_smcharger_context->battery_state : SMCHARGER_BATTERY_STATE_IDLE);
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", show BG_LED battery_state=%d", 1, battery_state);
    bool ret = FALSE;
    /* Update background led pattern according to app_battery_state. */
    switch (battery_state) {
        case SMCHARGER_BATTERY_STATE_IDLE:
        case SMCHARGER_BATTERY_STATE_FULL:
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", show BG_LED none", 0);
            ret = FALSE;
            break;
        case SMCHARGER_BATTERY_STATE_CHARGING:
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", show BG_LED CHARGING", 0);
            apps_config_set_background_led_pattern(LED_INDEX_CHARGING, FALSE, APPS_CONFIG_LED_AWS_SYNC_PRIO_HIGH);
            ret = TRUE;
            break;
        case SMCHARGER_BATTERY_STATE_CHARGING_FULL:
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", IDLE BG_LED for CHARGING_FULL", 0);
            apps_config_set_background_led_pattern(LED_INDEX_IDLE, FALSE, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOWEST);
            ret = TRUE;
            break;
        case SMCHARGER_BATTERY_STATE_LOW_CAP:
        case SMCHARGER_BATTERY_STATE_SHUTDOWN:
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", show BG_LED LOW_BATTERY/SHUTDOWN", 0);
            apps_config_set_background_led_pattern(LED_INDEX_LOW_BATTERY, FALSE, APPS_CONFIG_LED_AWS_SYNC_PRIO_HIGH);
            ret = TRUE;
            break;
        case SMCHARGER_BATTERY_STATE_THR:
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", show error BG_LED SMCHARGER_BATTERY_STATE_THR", 0);
            apps_config_set_background_led_pattern(LED_INDEX_CHARGING_ERROR, FALSE, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOW);
            ret = TRUE;
            break;
    }
    return ret;
}

void app_smcharger_handle_case_battery(uint8_t case_battery)
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", handle_case_battery %d", 1, case_battery);
    if (case_battery <= 100
            || (case_battery >= 128 && case_battery <= 228)) {
        /* Use SmartCharger Case battery_percent, such as send it via BLE ADV (XiaoAI MIUI Quick_Connect, Fast Pairing). */
        app_smcharger_ui_shell_public_event(SMCHARGER_CASE_BATTERY_REPORT_ACTION, case_battery);
    } else {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS", handle_case_battery invalid case_battery", 0);
    }
}

void app_smcharger_handle_key_event(uint32_t key_value)
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", handle_key_event key=%d", 1, key_value);
    if (g_smcharger_context != NULL && key_value <= 255
            && (g_smcharger_context->smcharger_state == STATE_SMCHARGER_LID_OPEN
                || g_smcharger_context->smcharger_state == STATE_SMCHARGER_LID_CLOSE)) {
        // Default Key function must be both LID_OPEN
        APPS_LOG_MSGID_I(SMCHARGER_UTILS", handle_key_event state=%d peer=%d aws=%d", 3,
                         g_smcharger_context->smcharger_state,
                         g_smcharger_context->peer_smcharger_state,
                         g_smcharger_context->aws_state);
        if (g_smcharger_context->smcharger_state == STATE_SMCHARGER_LID_OPEN
                && key_value == APP_SMCHARGER_KEY_BT_AIR_PAIRING) {
            app_smcharger_bt_air_pairing();
        } else if (g_smcharger_context->smcharger_state == STATE_SMCHARGER_LID_OPEN
                && g_smcharger_context->peer_smcharger_state == STATE_SMCHARGER_LID_OPEN
                && g_smcharger_context->aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
            if (key_value == APP_SMCHARGER_KEY_BT_DISCOVERABLE) {
                app_smcharger_bt_enter_discoverable();
            } else if (key_value == APP_SMCHARGER_KEY_BT_CLEAR) {
                g_smcharger_context->bt_clear_ongoing = TRUE;
                app_smcharger_bt_clear_enter_discoverable(APP_SMCHARGER_REQUEST_BT_OFF);
            }
        } else {
            APPS_LOG_MSGID_E(SMCHARGER_UTILS", handle_key_event - No both LID_OPEN/AWS", 0);
        }

        app_smcharger_ui_shell_public_event(SMCHARGER_CHARGER_KEY_ACTION, key_value);
    } else {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS", handle_key_event invalid key_value", 0);
    }
}

void app_smcharger_handle_user_data(int state, int event_id, uint8_t user_data)
{
    APPS_LOG_MSGID_I(SMCHARGER_UTILS", handle_user_data state=%d event_id=%d user_data=0x%02X",
                     3, state, event_id, user_data);

    /* Send SMCHARGER_CLOSE_LID_ACTION to notify other APP when received USER_DATA1 command. */
    if (state == STATE_SMCHARGER_LID_OPEN && event_id == EVENT_ID_SMCHARGER_USER_DATA1_PATTERN) {
        app_smcharger_ui_shell_public_event(SMCHARGER_CLOSE_LID_ACTION, 0);
    }

    if ((state == STATE_SMCHARGER_LID_OPEN || state == STATE_SMCHARGER_LID_CLOSE)
        && (event_id == EVENT_ID_SMCHARGER_USER_DATA1_PATTERN
            || event_id == EVENT_ID_SMCHARGER_USER_DATA2_PATTERN
            || event_id == EVENT_ID_SMCHARGER_USER_DATA3_PATTERN)) {
        /* Use USER_DATA1/2/3 according to Case State & user_data. */
        if (event_id == EVENT_ID_SMCHARGER_USER_DATA1_PATTERN) {
            app_smcharger_ui_shell_public_event(SMCHARGER_USER_DATA1_ACTION, user_data);
        } else if (event_id == EVENT_ID_SMCHARGER_USER_DATA2_PATTERN) {
            app_smcharger_ui_shell_public_event(SMCHARGER_USER_DATA2_ACTION, user_data);
        } else if (event_id == EVENT_ID_SMCHARGER_USER_DATA3_PATTERN) {
            app_smcharger_ui_shell_public_event(SMCHARGER_USER_DATA3_ACTION, user_data);
        }
    } else {
        APPS_LOG_MSGID_E(SMCHARGER_UTILS", handle_user_data invalid key_value", 0);
    }
}

void app_smcharger_update_case_battery_nvkey(uint8_t case_battery)
{
    uint32_t size = sizeof(uint8_t);
    nvkey_status_t status = nvkey_write_data(NVKEYID_APP_CASE_BATTERY_INFO,
                                             (const uint8_t *)&case_battery, size);
    APPS_LOG_MSGID_I(SMCHARGER_UTILS" update_case_battery_nvkey, case_battery=%d status=%d",
                     2, case_battery, status);

    // Notify other APP when Agent smcharger_case_battery changed
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (role == BT_AWS_MCE_ROLE_AGENT) {
        app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_CHANGED_EVENT, NULL, 0);
    }
}

/*
 * @brief   SmartCharger APP UT Code by using AT CMD
 * AT+SMC=pwr_key_boot/chg_in_boot/chg_in/chg_out/chg_off/lid_close/lid_open
 */
#ifdef MTK_SMART_CHARGER_TEST

char smcharger_context_str[200] = {0};

static void smcharger_atci_get_context_str(uint8_t *response_buf)
{
    memset(response_buf, 0, ATCI_UART_TX_FIFO_BUFFER_SIZE);
    snprintf((char *)response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
             "role=0x%02X battery_state=%d own=%s[peer=%s] battery_percent=%d[peer-%d] aws=%02X\r\n",
             bt_device_manager_aws_local_info_get_role(),
             g_smcharger_context->battery_state,
             app_smcharger_state_string(g_smcharger_context->smcharger_state),
             app_smcharger_state_string(g_smcharger_context->peer_smcharger_state),
             (int)g_smcharger_context->battery_percent,
             (int)g_smcharger_context->peer_battery_percent,
             g_smcharger_context->aws_state);
}

static atci_status_t smcharger_atci_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            char *atcmd = parse_cmd->string_ptr + parse_cmd->name_len + 1;
            char cmd[20] = {0};
            uint8_t data = 0;
            memcpy(cmd, atcmd, strlen(atcmd) - 2);
            int event_id = EVENT_ID_SMCHARGER_NONE;
            if (strstr(cmd, "pwr_key_boot") != NULL) {
                event_id = EVENT_ID_SMCHARGER_POWER_KEY_BOOT;
            } else if (strstr(cmd, "chg_in_boot") != NULL) {
                event_id = EVENT_ID_SMCHARGER_CHARGER_IN_BOOT;
            } else if (strstr(cmd, "chg_in") != NULL) {
                event_id = EVENT_ID_SMCHARGER_CHARGER_IN_INTERRUPT;
            } else if (strstr(cmd, "chg_out") != NULL) {
                event_id = EVENT_ID_SMCHARGER_CHARGER_OUT_PATTERN;
            } else if (strstr(cmd, "chg_off") != NULL) {
                event_id = EVENT_ID_SMCHARGER_CHARGER_OFF_PATTERN;
                data = 0;    /* battery level report. */
            } else if (strstr(cmd, "key") != NULL) {
                event_id = EVENT_ID_SMCHARGER_CHARGER_KEY_PATTERN;
                data = 0;     /* key value. */
                int x = 0;
                sscanf(cmd, "key,%d", &x);
                data = (uint8_t)x;
            } else if (strstr(cmd, "lid_close") != NULL) {
                event_id = EVENT_ID_SMCHARGER_USER_DATA1_PATTERN;
                ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_CHARGER_CASE,
                                    EVENT_ID_SMCHARGER_LID_CLOSE_PATTERN, NULL, 0, NULL, 3 * 1000);
            } else if (strstr(cmd, "lid_open") != NULL) {
                event_id = EVENT_ID_SMCHARGER_LID_OPEN_PATTERN;
                data = 80;    /* Battery level report. */
            } else if (strstr(cmd, "user_data1") != NULL) {
                event_id = EVENT_ID_SMCHARGER_USER_DATA1_PATTERN;
                data = 0x11;
            } else if (strstr(cmd, "user_data2") != NULL) {
                event_id = EVENT_ID_SMCHARGER_USER_DATA2_PATTERN;
                data = 0x22;
            } else if (strstr(cmd, "user_data3") != NULL) {
                event_id = EVENT_ID_SMCHARGER_USER_DATA3_PATTERN;
                data = 0x33;
            } else if (strstr(cmd, "status") != NULL) {
                smcharger_atci_get_context_str(response.response_buf);
                break;
            } else if (strstr(cmd, "case_bat") != NULL) {
                int x = 0;
                sscanf(cmd, "case_bat,%d", &x);
                app_smcharger_handle_case_battery_report(x);
            } else {
                APPS_LOG_MSGID_E(SMCHARGER_UTILS", invalid SMCharger AT-CMD", 0);
            }
            memset(response.response_buf, 0, ATCI_UART_TX_FIFO_BUFFER_SIZE);
            snprintf((char *)response.response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, "OK - %s\r\n", atcmd);
            app_smcharger_ui_shell_event(event_id, (void *)(uint32_t)data, 0);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}
#endif

/**================================================================================*/
/**                           PUBLIC API for other APP                             */
/**================================================================================*/

void app_smcharger_update_bat()
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (role == BT_AWS_MCE_ROLE_AGENT && g_smcharger_context != NULL) {
        unsigned char partner_battery_percent = g_smcharger_context->peer_battery_percent;
        unsigned char left_bat = 0;
        unsigned char right_bat = 0;
        audio_channel_t channel = ami_get_audio_channel();
        if (AUDIO_CHANNEL_L == channel) {
            left_bat = g_smcharger_context->battery_percent;
            right_bat = partner_battery_percent;
        } else if (AUDIO_CHANNEL_R == channel) {
            right_bat = g_smcharger_context->battery_percent;
            left_bat = partner_battery_percent;
        } else {
            APPS_LOG_MSGID_I(SMCHARGER_UTILS", [XIAOAI_VA] invalid L/R Channel", 0);
        }

        APPS_LOG_MSGID_I(SMCHARGER_UTILS", [XIAOAI_VA] left=%d right=%d case=%d",
                         3, left_bat, right_bat, g_smcharger_context->case_battery_percent);
#ifdef MTK_VA_XIAOAI_ENABLE
        /* Call XiaoAI API to set battery. */
        xiaoai_set_battery(g_smcharger_context->battery_percent,
                           partner_battery_percent,
                           g_smcharger_context->case_battery_percent,
                           left_bat,
                           right_bat);
#endif
    }
}

app_smcharger_in_out_t app_smcharger_is_charging()
{
    app_smcharger_in_out_t result = APP_SMCHARGER_NONE;
    if (g_smcharger_context != NULL) {
        uint8_t state = (uint8_t)g_smcharger_context->smcharger_state;
        if (state == STATE_SMCHARGER_LID_OPEN
                || state == STATE_SMCHARGER_LID_CLOSE
                || state == STATE_SMCHARGER_OFF) {
            result = APP_SMCHARGER_IN;
        } else if (state == STATE_SMCHARGER_OUT_OF_CASE) {
            result = APP_SMCHARGER_OUT;
        }
    }
    return result;
}

app_smcharger_in_out_t app_smcharger_peer_is_charging()
{
    app_smcharger_in_out_t result = APP_SMCHARGER_NONE;
    if (g_smcharger_context != NULL) {
        uint8_t state = (uint8_t)g_smcharger_context->peer_smcharger_state;
        if (state == STATE_SMCHARGER_LID_OPEN
                || state == STATE_SMCHARGER_LID_CLOSE
                || state == STATE_SMCHARGER_OFF) {
            result = APP_SMCHARGER_IN;
        } else if (state == STATE_SMCHARGER_OUT_OF_CASE) {
            result = APP_SMCHARGER_OUT;
        }
    }
    return result;
}

uint8_t app_smcharger_read_case_battery_nvkey()
{
    uint8_t case_battery = 0;
    uint32_t size = sizeof(uint8_t);
    nvkey_status_t status = nvkey_read_data(NVKEYID_APP_CASE_BATTERY_INFO, (uint8_t *)&case_battery, &size);
    APPS_LOG_MSGID_I(SMCHARGER_UTILS" read_case_battery_nvkey, read status=%d", 1, status);
    if (status == NVKEY_STATUS_ITEM_NOT_FOUND) {
        size = sizeof(uint8_t);
        status = nvkey_write_data(NVKEYID_APP_CASE_BATTERY_INFO, (const uint8_t *)&case_battery, size);
        APPS_LOG_MSGID_I(SMCHARGER_UTILS" read_case_battery_nvkey, write status=%d", 1, status);
    }
    return case_battery;
}

void app_smcharger_get_battery_percent(uint8_t *own_battery, uint8_t *peer_battery, uint8_t *case_battery)
{
    if (own_battery != NULL) {
        *own_battery = g_smcharger_context->battery_percent;
    }
    if (peer_battery != NULL) {
        *peer_battery = g_smcharger_context->peer_battery_percent;
    }
    if (case_battery != NULL) {
        *case_battery = g_smcharger_context->case_battery_percent;
    }
}

#endif
