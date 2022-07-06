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
 * File: apps_events_key_event.c
 *
 * Description: This file defines callback of key and send events to APPs
 *
 */

#include "airo_key_event.h"
#include "apps_events_key_event.h"
#include "ui_shell_manager.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_config_event_list.h"
#include "atci.h"
#include "FreeRTOS.h"
#include <stdlib.h>
#include <string.h>

#define LOG_TAG     "[app_key_event]"       /* Log tag */

static bool s_press_from_power_on = false;  /* If user press the key from power on, need do special things. */
static int32_t s_press_event_counter = 0;   /* When power on reason is press key, count the times of press event. */

extern uint8_t pmu_get_power_on_reason();

/**
 * @brief      The implementation of the key callback.
 * @param[in]  event, the key event type, refer to airo_key_event_t.
 * @param[in]  key_data, means key id, ex. DEVICE_KEY_POWER or EINT_KEY_0.
 * @param[in]  user_data, The same user_data input in airo_key_register_callback()
 */
static void _apps_key_airo_key_callback(airo_key_event_t event, uint8_t key_data, void *user_data)
{
    APPS_LOG_MSGID_I(LOG_TAG"key event  = 0x%x, key_data = 0x%x", 2, event, key_data);
    uint16_t *p_key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
    ui_shell_status_t status;

#if !defined(AIR_SLIDE_POWER_KEY_ENABLE)
    /* Ignore the key events before the second press event happen. */
    if (s_press_from_power_on && AIRO_KEY_PRESS == event) {
        s_press_event_counter--;
        s_press_from_power_on = s_press_event_counter <= 0 ? false : true;
        APPS_LOG_MSGID_I(LOG_TAG"key press from power on end", 0);
    }
#endif

    if (p_key_action) {
        /* Set the s_press_from_power_on flag into the extra_data, app_preproc_activity.c will use it. */
        *p_key_action = s_press_from_power_on;
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        /* If the feature enabled, EINT_KEY_2 indicates in ear detection event. */
        if ((key_data == EINT_KEY_2) && (event == AIRO_KEY_PRESS || event == AIRO_KEY_RELEASE)) {
            bool *isInEar = (bool *)pvPortMalloc(sizeof(bool));
            if (isInEar != NULL) {
                *isInEar = (event == AIRO_KEY_PRESS) ? true : false;
                status = ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_UPDATE_IN_EAR_STA_EFFECT);
                if (UI_SHELL_STATUS_OK != status) {
                    APPS_LOG_MSGID_I(LOG_TAG"touch sensor event remove failed., err = %d", 1, status);
                }

                status = ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                             APPS_EVENTS_INTERACTION_UPDATE_IN_EAR_STA_EFFECT, (void *)isInEar, sizeof(bool),
                                             NULL, 500);
                if (UI_SHELL_STATUS_OK != status) {
                    vPortFree(isInEar);
                    APPS_LOG_MSGID_I(LOG_TAG"touch sensor event send failed., err = %d", 1, status);
                }
            } else {
                APPS_LOG_MSGID_E(LOG_TAG"touch sensor malloc fail.", 0);
            }

        }
#endif
        /* Use one uint32_t as the event_id parameter to contains key_data and event. */
        status = ui_shell_send_event(true, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                                     (key_data & 0xFF) | ((event & 0xFF) << 8),
                                     p_key_action, sizeof(uint16_t), NULL, 0);
        if (UI_SHELL_STATUS_OK != status) {
            vPortFree(p_key_action);
#if !defined(AIR_SLIDE_POWER_KEY_ENABLE)
            if (UI_SHELL_STATUS_INVALID_STATE == status) {
                s_press_from_power_on = true;
            }
#endif
            APPS_LOG_MSGID_I(LOG_TAG"key press from power on, err = %d", 1, status);
        }
    } else {
        APPS_LOG_MSGID_E(LOG_TAG"key callback malloc fail", 0);
    }
}


#ifdef AIR_ROTARY_ENCODER_ENABLE
static void app_event_key_rotary_encoder_callback(bsp_rotary_encoder_port_t port, bsp_rotary_encoder_event_t event, uint32_t event_data, void *user_data)
{
    APPS_LOG_MSGID_I(LOG_TAG"rotary port  = 0x%x, event = 0x%x, value = %d", 3, port, event, event_data);
    uint16_t *p_key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
    if (p_key_action) {
        ui_shell_send_event(true, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_ROTARY_ENCODER,
                            (port & 0xFF) | ((event & 0xFF) << 8) | ((event_data & 0xFFFF) << 16),
                            p_key_action, sizeof(uint16_t), NULL, 0);
    }
}
#endif

/**
 * @brief      The ATCI cmd handler, refer to at_cmd_hdlr_fp.
 * @param[in]  parse_cmd, The value is defined in #atci_parse_cmd_param_t. This parameter is given by the ATCI
 *             parser to indicate the input command data to be transferred to the command handler.
 * @return     ATCI_STATUS_OK means success, otherwise means fail.
 */
static atci_status_t _key_action_cmd_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *param1 = NULL;
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    APPS_LOG_MSGID_I(LOG_TAG"_key_action_cmd_handler mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            param1 = parse_cmd->string_ptr + parse_cmd->name_len + 1;
            uint16_t *p_key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
            char *end = NULL;
            if (p_key_action) {
                *p_key_action = strtol(param1, &end, 16);
                APPS_LOG_MSGID_I(LOG_TAG"send simulate key action = 0x%x", 1, *p_key_action);
                /* The extra_data of the event is key action. */
                ui_shell_send_event(false,
                                    EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                                    INVALID_KEY_EVENT_ID,
                                    p_key_action, sizeof(uint16_t), NULL, 0);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }
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

#ifdef AIR_ROTARY_ENCODER_ENABLE
/**
 * @brief      The ATCI cmd handler, refer to at_cmd_hdlr_fp.
 * @param[in]  parse_cmd, The value is defined in #atci_parse_cmd_param_t. This parameter is given by the ATCI
 *             parser to indicate the input command data to be transferred to the command handler.
 * @return     ATCI_STATUS_OK means success, otherwise means fail.
 */
static atci_status_t _rotary_simulator_cmd_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *saveptr;
    char *param1 = NULL;
    char *param2 = NULL;
    char *param3 = NULL;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            param1 = strtok_r(parse_cmd->string_ptr + parse_cmd->name_len + 1, ",\r\n", &saveptr);
            param2 = strtok_r(NULL, ",\r\n", &saveptr);
            param3 = strtok_r(NULL, ",\r\n", &saveptr);
            if (param1 && param2 && param3) {
                bsp_rotary_encoder_port_t port;
                bsp_rotary_encoder_event_t event;
                uint32_t event_data;
                port = atoi(param1);
                event = atoi(param2);
                event_data = atoi(param3);
                app_event_key_rotary_encoder_callback(port, event, event_data, NULL);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
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

/* The atci command table. */
static atci_cmd_hdlr_item_t simulate_key_atci_cmd[] = {
    {
        .command_head = "AT+APPKEYACTION",    /**< Simulate key event */
        .command_hdlr = _key_action_cmd_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#ifdef AIR_ROTARY_ENCODER_ENABLE
    {
        .command_head = "AT+APP_ROTARY_EVENT",  /**< Simulate rotary event */
        .command_hdlr = _rotary_simulator_cmd_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#endif
};

#ifdef AIR_ROTARY_ENCODER_ENABLE
static const uint8_t rotary_array[] = {
    BSP_ROTARY_ENCODER_0,
    BSP_ROTARY_ENCODER_1,
};

static void apps_event_rotary_init(void)
{
    uint32_t i;
    bsp_rotary_encoder_status_t status;
    bsp_rotary_encoder_port_t port;
    bsp_rotary_encoder_config_t config = {
        .eint_debounce_time_ms = 10,
        .event_threshold = 1,
        .event_timeout_ms = 100,
        .callback = app_event_key_rotary_encoder_callback,
        .user_data = NULL,
    };

    for (i = 0; i < sizeof(rotary_array) / sizeof(uint8_t); i++) {
        port = rotary_array[i];
        status = bsp_rotary_encoder_init(port, &config);
        if (status != BSP_ROTARY_ENCODER_STATUS_OK) {
            APPS_LOG_MSGID_E("Init port:%d fail: %d", 2, port, status);
        }
    }
}
#endif

void apps_event_key_event_init(void)
{
    bool status;
    status = airo_key_event_init();
    if (status != true) {
        APPS_LOG_MSGID_E("sct key init fail, status:%d", 1, status);
    }
#if !defined(AIR_SLIDE_POWER_KEY_ENABLE)
    uint8_t reason = pmu_get_power_on_reason();
    if (reason & 0x01) {
        /* Power on reason is pressed power key, set the flag. */
        s_press_from_power_on = pmu_get_pwrkey_state() == 1 ? true : false;
        s_press_event_counter = s_press_from_power_on ? 2 : 0;
    }
    APPS_LOG_MSGID_I("inti key power on by reason : %x", 1, reason);
#endif
    /* Register key callback. */
    status = airo_key_register_callback(_apps_key_airo_key_callback, NULL);
    if (status != true) {
        APPS_LOG_MSGID_E("register callback fail, status:%d", 1, status);
    }

#ifdef AIR_ROTARY_ENCODER_ENABLE
    apps_event_rotary_init();
#endif

    atci_register_handler(simulate_key_atci_cmd, sizeof(simulate_key_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
}

void app_event_key_event_decode(uint8_t *key_id, airo_key_event_t *key_event, uint32_t event_id)
{
    if (key_id) {
        *key_id = event_id & 0xFF;
    }
    if (key_event) {
        *key_event = (event_id >> 8) & 0xFF;
    }
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
void app_event_rotary_event_decode(bsp_rotary_encoder_port_t *p_port, bsp_rotary_encoder_event_t *p_event, uint32_t *p_rotary_data, uint32_t event_id)
{
    if (p_port) {
        *p_port = event_id & 0xFF;
    }
    if (p_event) {
        *p_event = (event_id >> 8) & 0xFF;
    }
    if (p_rotary_data) {
        *p_rotary_data = (event_id >> 16) & 0xFFFF;
    }
}
#endif

