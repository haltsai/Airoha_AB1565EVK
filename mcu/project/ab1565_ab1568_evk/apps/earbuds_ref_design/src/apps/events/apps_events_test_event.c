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

#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "atci.h"
#include "apps_debug.h"
#include "multi_ble_adv_manager.h"
#include "bt_app_common.h"
#include "apps_events_test_event.h"
#ifdef AIR_TILE_ENABLE
#include "app_tile.h"
#endif

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "bt_gap_le.h"

#ifdef AIR_TILE_ENABLE /* Test CMD with tile features. */

#define LOG_TAG                         "[test_event]"

#define APP_CLASSIC_BT_OFF_REQUEST      "CLASSIC_BT_OFF"
#define APP_BT_ON_OFF_REQUEST           "BT_ON_OFF"
#define APP_BLE_START_TEST_ADV          "START_ADV"

#ifdef AIR_TILE_ENABLE
void apps_events_test_event_ble_adv_start(bool enable)
{
    APPS_LOG_MSGID_I(LOG_TAG"apps_events_test_event_ble_adv_start enable = %d", 1, enable);
    if (enable) {
        app_tile_start_advertising(true);
    } else {
        app_tile_stop_advertising();
    }
}

static bool app_test_parse_tile_cmd(char *string)
{
    /* AT+APPTEST=TILE_ADV*/
    if (strstr(string, "AT+APPTEST=TILE_ADV,STOP") != 0) {
        /* stop advertising*/
        app_tile_stop_advertising();
    } else if (strstr(string, "AT+APPTEST=TILE_ADV,START,CON") != 0) {
         /*start connectable advertising */
        app_tile_start_advertising(true);
    } else if (strstr(string, "AT+APPTEST=TILE_ADV,START,SCN") != 0) {
         /*start scannable advertising */
        app_tile_start_advertising(false);
    } else if (strstr(string, "AT+APPTEST=TILE_ADV,ACTIVE,1") != 0) {
        /* simulate tile is activated */
        app_tile_simulte_tile_activation(true);
    } else if (strstr(string, "AT+APPTEST=TILE_ADV,ACTIVE,0") != 0) {
        /* simulate tile is inactivated */
        app_tile_simulte_tile_activation(false);
    } else {
        return false;
    }
    return true;
}
#endif

static atci_status_t _app_test_cmd_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *saveptr;
    char *param1 = NULL;
    char *param2 = NULL;
    ui_shell_status_t ret;

    APPS_LOG_MSGID_I(LOG_TAG"_app_test_cmd_handler mode = %d", 1, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION:
        {        
#ifdef AIR_TILE_ENABLE
            bool result;
            result = app_test_parse_tile_cmd(parse_cmd->string_ptr);//for AT+APPTEST=TILE_ADV,
            if (result) {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                break;
            } else {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
#endif            
            param1 = strtok_r(parse_cmd->string_ptr + parse_cmd->name_len + 1, ",", &saveptr);
            param2 = strtok_r(NULL, ",", &saveptr);
            if (memcmp(APP_CLASSIC_BT_OFF_REQUEST, param1, sizeof(APP_CLASSIC_BT_OFF_REQUEST) - 1) == 0) {
                ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                        EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF,
                        NULL, 0, NULL, 0);
                if (UI_SHELL_STATUS_OK == ret) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (memcmp(APP_BT_ON_OFF_REQUEST, param1, sizeof(APP_BT_ON_OFF_REQUEST) - 1) == 0) {
                int32_t on_off = atoi(param2) == 1;
                ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                        EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT,
                        (void *)on_off, 0, NULL, 0);
                if (UI_SHELL_STATUS_OK == ret) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (memcmp(APP_BLE_START_TEST_ADV, param1, sizeof(APP_BLE_START_TEST_ADV) - 1) == 0) {
                int32_t on_off = atoi(param2) == 1;
                /* Because the atci task is not ui task, send event to do start/stop adv */
                ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                        EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_TEST_BLE_ADV,
                        (void *)on_off, 0, NULL, 0);
                if (UI_SHELL_STATUS_OK == ret) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                APPS_LOG_W(LOG_TAG"_app_test_cmd_handler param 1 = %s", param1);
            }
        }
            break;
        default:
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_cmd_hdlr_item_t app_test_atci_cmd[] = {
    {
        .command_head = "AT+APPTEST",    /**< Test Charger plugin/out */
        .command_hdlr = _app_test_cmd_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

void apps_events_test_event_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(app_test_atci_cmd, sizeof(app_test_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
    APPS_LOG_MSGID_I(LOG_TAG"test_event, atci_register_handler ret = %d", 1, ret);
}
#endif
