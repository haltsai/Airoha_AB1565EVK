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
 * File: apps_events_battery_event.c
 *
 * Description: This file defines callback of battery and send battery events to APPs
 *
 */

#include "apps_events_event_group.h"
#include "apps_events_battery_event.h"
#include "apps_debug.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "ui_shell_manager.h"
#include "battery_management.h"
#include "battery_management_core.h"
#include <stdlib.h>

/* The timer name of reading battery percentage. */
#define BATTERY_EVENT_TIMER_NAME        "battery_event"
/* The timer id of reading battery percentage. It's useful when multi timer for a same callback. */
#define BATTERY_EVENT_TIMER_ID          0
/* The interval of the timer callback will be called. */
#define BATTERY_EVENT_TIMER_INTERVAL    (60 * 1000)

TimerHandle_t timer = NULL; /* The pointer of the timer instance. */
/* The current battery percent, when the value is not change, don't send event. */
int32_t         s_battary_percent = 0;
/* The current charger status, when the value is not change, don't send event. */
int32_t         s_charging_status = 0;
/* The shutdown state, calculate from voltage. */
battery_event_shutdown_state_t s_shutdown_state = APPS_EVENTS_BATTERY_SHUTDOWN_STATE_NONE;

/**
 * @brief      Calculate current status is shutdown or not.
 * @param[in]  voltage, current battery voltage.
 * @return     Current shutdown state.
 */
battery_event_shutdown_state_t calculate_shutdown_state(int32_t voltage)
{
    battery_basic_data battery_basic_data = battery_management_get_basic_data();
    return (voltage <= battery_basic_data.shutdown_bat) ?
           APPS_EVENTS_BATTERY_SHUTDOWN_STATE_VOLTAGE_LOW
           : APPS_EVENTS_BATTERY_SHUTDOWN_STATE_IDLE;
}

/**
 * @brief      Timer callback to read battery percentage and voltage.
 * @param[in]  xTimer, The timer instance.
 */
static void _timer_callback_function(TimerHandle_t xTimer)
{

    battery_event_shutdown_state_t shutdown_state;
    APPS_LOG_MSGID_I("Check battery timer", 0);
    /* The shutdown state, calculate from voltage. */
    int32_t battery_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    int32_t battery_voltage = battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE);
    /* Send event when value changed. */
    if (s_battary_percent != battery_percent) {
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BATTERY,
                            APPS_EVENTS_BATTERY_PERCENT_CHANGE, (void *)battery_percent, 0, NULL, 0);
        s_battary_percent = battery_percent;
        APPS_LOG_MSGID_I("Send battery percent : %d", 1, battery_percent);
    }
    shutdown_state = calculate_shutdown_state(battery_voltage);
    /* Send event when value changed. */
    if (shutdown_state != s_shutdown_state) {
        APPS_LOG_MSGID_I("Need update shutdown_state : %d, current_voltage = %d", 2, shutdown_state, battery_voltage);
        s_shutdown_state = shutdown_state;
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BATTERY,
                            APPS_EVENTS_BATTERY_SHUTDOWN_STATE_CHANGE, (void *)shutdown_state, 0, NULL, 0);
    }

}

/**
 * @brief      The callback is regitered to battery management module to receive charger state change.
 * @param[in]  event, battery management event, refer to battery_management.h.
 * @param[in]  data, not used.
 */
static void _battery_management_callback(battery_management_event_t event, const void *data)
{
    /* Charging state change */
    if (event == BATTERY_MANAGEMENT_EVENT_CHARGER_STATE_UPDATE) {
        int32_t charging_status = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE);
        if (s_charging_status == charging_status) {
            APPS_LOG_MSGID_I("duplicate charging_status: %d", 1, charging_status);
        } else {
            ui_shell_send_event(true, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BATTERY,
                                APPS_EVENTS_BATTERY_CHARGER_STATE_CHANGE, (void *)charging_status, 0,
                                NULL, 0);
            APPS_LOG_MSGID_I("Send battery charging_status : %d", 1, charging_status);
            s_charging_status = charging_status;
        }
        /* Charger exist change */
    } else if (event == BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE) {
        int32_t charger_exist = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
        ui_shell_send_event(true, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BATTERY,
                            APPS_EVENTS_BATTERY_CHARGER_EXIST_CHANGE, (void *)charger_exist, 0,
                            NULL, 0);
        APPS_LOG_MSGID_I("Send battery charger_exist : %d", 1, charger_exist);
    }
}

#define SUPPORT_ATCI_BAT_SIMULATE   1   /* Enable simulate battery event. */

#ifdef SUPPORT_ATCI_BAT_SIMULATE

#include "atci.h"

/**
 * @brief      The callback for AT CMD "AT+BATCHARGING".
 * @param[in]  parse_cmd, ATCI cmd.
 * @return     ACTI status.
 */
static atci_status_t _battery_atci_update_charging_exist(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    int32_t charging_exist_status = 0;
    APPS_LOG_MSGID_I("_battery_atci_update_charging_exist mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
            charging_exist_status = atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
            APPS_LOG_MSGID_I("Send simulated charging_exist_status = %d", 1, charging_exist_status);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BATTERY,
                                APPS_EVENTS_BATTERY_CHARGER_EXIST_CHANGE, (void *)charging_exist_status, 0,
                                NULL, 0);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

/**
 * @brief      The callback for AT CMD "AT+BATCHARGINGSTATE".
 * @param[in]  parse_cmd, ATCI cmd.
 * @return     ACTI status.
 */
static atci_status_t _battery_atci_update_charging_state(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    int32_t charging_status = 0;
    APPS_LOG_MSGID_I("_battery_atci_update_charging_state mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
            charging_status = atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
            APPS_LOG_MSGID_I("Send simulated charging_exist_status = %d", 1, charging_status);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BATTERY,
                                APPS_EVENTS_BATTERY_CHARGER_STATE_CHANGE, (void *)charging_status, 0,
                                NULL, 0);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

/**
 * @brief      The callback for AT CMD "AT+BATPERCENT".
 * @param[in]  parse_cmd, ATCI cmd.
 * @return     ACTI status.
 */
static atci_status_t _battery_atci_update_battery_percent(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    int32_t percent = 0;
    APPS_LOG_MSGID_I("_battery_atci_update_battery_percent mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
            percent = atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
            APPS_LOG_MSGID_I("Send simulated battery percent = %d", 1, percent);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BATTERY,
                                APPS_EVENTS_BATTERY_PERCENT_CHANGE, (void *)percent, 0,
                                NULL, 0);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

/**
 * @brief      The callback for AT CMD "AT+BATVOLTAGE".
 * @param[in]  parse_cmd, ATCI cmd.
 * @return     ATCI status.
 */
static atci_status_t _battery_atci_update_voltage(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    int32_t voltage = 0;
    battery_event_shutdown_state_t mock_shutdown_state;

    APPS_LOG_MSGID_I("_battery_atci_update_voltage mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
            voltage = atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
            APPS_LOG_MSGID_I("Send simulated battery voltage = %d", 1, voltage);
            mock_shutdown_state = calculate_shutdown_state(voltage);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BATTERY,
                                APPS_EVENTS_BATTERY_SHUTDOWN_STATE_CHANGE, (void *)mock_shutdown_state, 0, NULL, 0);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        default:
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

/* ATCI cmd table */
static atci_cmd_hdlr_item_t battery_simu_atci_cmd[] = {
    {
        .command_head = "AT+BATCHARGING",    /**< Test Charger plugin/out */
        .command_hdlr = _battery_atci_update_charging_exist,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BATCHARGINGSTATE",    /**< Test Charger plugin/out */
        .command_hdlr = _battery_atci_update_charging_state,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BATPERCENT",    /* Test battery percent change*/
        .command_hdlr = _battery_atci_update_battery_percent,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BATVOLTAGE",    /* Test battery percent change*/
        .command_hdlr = _battery_atci_update_voltage,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};
#endif

void apps_events_battery_event_init(void)
{
    atci_status_t ret;
    if (battery_management_register_callback(_battery_management_callback) != BATTERY_MANAGEMENT_STATUS_OK) {
        APPS_LOG_MSGID_E("Cannot register battery callback", 0);
        return;

    }
    /* Because battery management will be init in system_init function
    if (!battery_management_init()) {
        APPS_LOG_MSGID_E("Cannot init battery management", 0);
        return;
    }
    */
    s_charging_status = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE);
    s_battary_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);

    APPS_LOG_MSGID_I("Start percent : %d", 1, s_battary_percent);

#ifdef SUPPORT_ATCI_BAT_SIMULATE
    ret = atci_register_handler(battery_simu_atci_cmd, sizeof(battery_simu_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
    APPS_LOG_MSGID_I("atci_register_handler ret = %d", 1, ret);
#endif


    timer = xTimerCreate(BATTERY_EVENT_TIMER_NAME, BATTERY_EVENT_TIMER_INTERVAL / portTICK_PERIOD_MS, pdTRUE, BATTERY_EVENT_TIMER_ID, _timer_callback_function);

    if (timer) {
        xTimerStart(timer, 0);
    }

}
