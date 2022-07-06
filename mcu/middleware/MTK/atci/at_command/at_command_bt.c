/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifdef MTK_BT_AT_COMMAND_ENABLE
// For Register AT command handler
#include "at_command.h"
#include "at_command_bt.h"
#include "syslog.h"
#include <FreeRTOS.h>
#include <stdlib.h>

#include "queue.h"
#include "task.h"
#include "timers.h"

//For bt head file
#include "bt_gap.h"
#include "bt_system.h"
#ifdef MTK_PORT_SERVICE_ENABLE
#include "serial_port.h"
#endif
#include "nvdm.h"
#include "nvkey_id_list.h"
#include "nvkey.h"
#include "hal_trng.h"
#include "bt_device_manager_internal.h"
#if defined(MTK_MUX_ENABLE)
#include "mux.h"
#endif
#include "bt_power_on_config.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#endif
#include "bt_os_layer_api.h"
#include "bt_device_manager_test_mode.h"

log_create_module(atci_bt, PRINT_LEVEL_INFO);
atci_bt_context_struct_t atci_bt_cntx;
atci_bt_context_struct_t *atci_bt_cntx_p = &atci_bt_cntx;
static bool dut_mode_enable = false;

#if 1
//extern function
extern bt_status_t bt_driver_power_on(void);
extern bt_status_t bt_driver_power_off(void);
extern uint8_t bt_hci_event_parser(uint8_t *param, uint8_t paramLen, atci_bt_hci_event *hci_event);
extern void bt_hci_log(const uint8_t in, const void * data, const uint32_t data_len);
extern uint8_t bt_driver_send_hci_command(uint16_t opCode, uint8_t cmd_len, uint8_t * cmd);
extern bool bt_driver_register_atci_rx_callback(void * rx_callback);
extern void bt_driver_deinit_atci_mode(void);
extern void bt_hci_cmd_raw_data_send(uint8_t* buffer);
extern bool bt_enable_relay_mode(hal_uart_port_t port);


//static function
static void atci_bt_error_handle(void);
static int8_t atci_bt_str2hex(uint8_t *string, uint32_t string_len, uint8_t data[]);
static int8_t atci_bt_hex2str(uint8_t data[], uint8_t data_len, uint8_t *string, uint32_t string_len);

static int8_t atci_bt_hci_string_to_cmd(uint8_t *string, uint32_t string_len, atci_bt_hci_cmd *hci_cmd);
static int8_t atci_bt_hci_event_to_string(atci_bt_hci_event *hci_event, uint8_t *string, uint32_t string_len);
static void atci_bt_task_init(void);
static void acti_bt_task_deinit(void);
static void atci_bt_cmd_task_main(void *pvParameters);
static void acti_bt_task_msg_handler(at_command_bt_message_struct_t *message);
/*---  Variant ---*/
#define ATCI_COMMAND_TASK_NAME "AT Ehanced"
#define ATCI_COMMAND_TASK_STACK_SIZE 768 //should be fine tune
#define ATCI_COMMAND_TASK_PRIORITY 3 //should be arrange by scenario
#define ATCI_COMMAND_QUEUE_SIZE 5

TaskHandle_t at_command_task_hdlr = NULL;
QueueHandle_t at_command_task_queue = NULL;
static TimerHandle_t event_timer = NULL;
static bool at_command_enter_test_mode = false;

#define STRNCPY(dest, source) strncpy(dest, source, strlen(source)+1);

static const atci_bt_uart_buad_t bt_uart_buad_table[] = {
    {115200, HAL_UART_BAUDRATE_115200},
    {230400, HAL_UART_BAUDRATE_230400},
    {460800, HAL_UART_BAUDRATE_460800},
    {921600, HAL_UART_BAUDRATE_921600},
    {2000000, HAL_UART_BAUDRATE_2000000},
    {3000000, HAL_UART_BAUDRATE_3000000},
    {3200000, HAL_UART_BAUDRATE_3200000},
};

static bool acti_bt_check_event_complete(uint8_t event)
{
    bool is_complete = 0;
    LOG_MSGID_I(atci_bt, "check_event_complete--event = %d\r\n",1, event);
    switch (event) {
        case ATCI_BT_EVENT_INQUIRY_COMPLETE:
        case ATCI_BT_EVENT_CONNECT_COMPLETE:
        case ATCI_BT_EVENT_DISCONNECT_COMPLETE:
        case ATCI_BT_EVENT_AUTH_COMPLETE:
        case ATCI_BT_EVENT_REMOTE_NAME_REQ_COMPLETE:
        case ATCI_BT_EVENT_CHNG_CONN_LINK_KEY_COMPLETE:
        case ATCI_BT_EVENT_MASTER_LINK_KEY_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_FEATURES_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_VERSION_COMPLETE:
        case ATCI_BT_EVENT_QOS_SETUP_COMPLETE:
        case ATCI_BT_EVENT_COMMAND_COMPLETE:
        case ATCI_BT_EVENT_READ_CLOCK_OFFSET_COMPLETE:
        case ATCI_BT_EVENT_FLOW_SPECIFICATION_COMPLETE:
        case ATCI_BT_EVENT_READ_REMOTE_EXT_FEAT_COMPLETE:
        case ATCI_BT_EVENT_SYNC_CONNECT_COMPLETE:
        case ATCI_BT_EVENT_SIMPLE_PAIRING_COMPLETE:
        case ATCI_BT_EVENT_ENHANCED_FLUSH_COMPLETE: {
            is_complete = 1;
            break;
        }
        default:
            break;
    }
    LOG_MSGID_I(atci_bt, "check_event_complete--is_complete = %d\r\n",1, is_complete);
    return is_complete;
}

static void acti_bt_delete_timer()
{
    if (event_timer && (xTimerIsTimerActive(event_timer) != pdFALSE)) {
        LOG_MSGID_I(atci_bt, "event complete: timer is exist\r\n", 0);
        xTimerStop(event_timer, 0);
        xTimerDelete(event_timer, 0);
        event_timer = NULL;
    }
}


void acti_bt_event_timer_handle_func(TimerHandle_t timer_id)
{
    LOG_MSGID_I(atci_bt, "timer_handle:timer out\r\n", 0);

    acti_bt_delete_timer();
    atci_bt_error_handle();

}


static void acti_bt_start_check_complete()
{


    if (event_timer && (xTimerIsTimerActive(event_timer) != pdFALSE)) {
        xTimerStop(event_timer, 0);

    } else {
        LOG_MSGID_I(atci_bt, "timer is not exist\r\n", 0);
        event_timer = xTimerCreate( "acti_bt_timer",
                                    (100000 / portTICK_PERIOD_MS),
                                    pdFALSE,
                                    NULL,
                                    acti_bt_event_timer_handle_func);
    }
    xTimerStart(event_timer, 0);
}

uint8_t bt_hci_event_parser(uint8_t *param, uint8_t paramLen, atci_bt_hci_event *hci_event)
{
    uint8_t data[260];
    uint16_t cmdOpcode;
    hci_event->event = 0;
    hci_event->status = 0;
    hci_event->handle = 0;
    hci_event->parmslen = 0;
    memset(hci_event->parms, 0, 256);

    if (param[1] != (paramLen - 2))
    {
        return 1;
    }
    memcpy(data, param, paramLen);
    hci_event->event = data[0];
    hci_event->parmslen = data[1];
    memcpy(hci_event->parms, &data[2], hci_event->parmslen);

    switch(hci_event->event)
    {
        case ATCI_HCE_INQUIRY_COMPLETE:
        case ATCI_HCE_REMOTE_NAME_REQ_COMPLETE:
        case ATCI_HCE_COMMAND_STATUS:
        case ATCI_HCE_ROLE_CHANGE:
        case ATCI_HCE_SIMPLE_PAIRING_COMPLETE:
            hci_event->status = data[2];
            break;
        case ATCI_HCE_CONNECT_COMPLETE:
        case ATCI_HCE_DISCONNECT_COMPLETE:
        case ATCI_HCE_AUTH_COMPLETE:
        case ATCI_HCE_ENCRYPT_CHNG:
        case ATCI_HCE_CHNG_CONN_LINK_KEY_COMPLETE:
        case ATCI_HCE_MASTER_LINK_KEY_COMPLETE:
        case ATCI_HCE_READ_REMOTE_FEATURES_COMPLETE:
        case ATCI_HCE_READ_REMOTE_VERSION_COMPLETE:
        case ATCI_HCE_QOS_SETUP_COMPLETE:
        case ATCI_HCE_MODE_CHNG:
        case ATCI_HCE_READ_CLOCK_OFFSET_COMPLETE:
        case ATCI_HCE_CONN_PACKET_TYPE_CHNG:
        case ATCI_HCE_FLOW_SPECIFICATION_COMPLETE:
        case ATCI_HCE_READ_REMOTE_EXT_FEAT_COMPLETE:
        case ATCI_HCE_SYNC_CONNECT_COMPLETE:
        case ATCI_HCE_SYNC_CONN_CHANGED:
        case ATCI_HCE_SNIFF_SUBRATING_EVENT:
        case ATCI_HCE_ENCRYPTION_KEY_REFRESH_COMPLETE_EVENT:
            hci_event->status = data[2];
            hci_event->handle = *((uint16_t *)(&(data[3])));
            break;
        case ATCI_HCE_COMMAND_COMPLETE:
            hci_event->status = data[5];
            memcpy(&cmdOpcode, data + 3, 2);
            switch(cmdOpcode)
            {
                case ATCI_HCC_READ_LMP_HANDLE:
                case ATCI_HCC_ROLE_DISCOVERY:
                case ATCI_HCC_READ_LINK_POLICY:
                case ATCI_HCC_WRITE_LINK_POLICY:
                case ATCI_HCC_SNIFF_SUBRATING:
                case ATCI_HCC_FLUSH:
                case ATCI_HCC_READ_AUTO_FLUSH_TIMEOUT:
                case ATCI_HCC_WRITE_AUTO_FLUSH_TIMEOUT:
                case ATCI_HCC_READ_XMIT_POWER_LEVEL:
                case ATCI_HCC_READ_LINK_SUPERV_TIMEOUT:
                case ATCI_HCC_WRITE_LINK_SUPERV_TIMEOUT:
                case ATCI_HCC_READ_FAILED_CONTACT_COUNT:
                case ATCI_HCC_RESET_FAILED_CONTACT_COUNT:
                case ATCI_HCC_GET_LINK_QUALITY:
                case ATCI_HCC_READ_RSSI:
                case ATCI_HCC_READ_AFH_CHANNEL_MAP:
                case ATCI_HCC_READ_CLOCK:
                case ATCI_HCC_LE_READ_CHANNEL_MAP:
                case ATCI_HCC_LE_LONG_TERM_KEY_REQ_REPL:
                case ATCI_HCC_LE_LONG_TERM_KEY_REQ_NEG_REPL:
                    hci_event->handle = *((uint16_t *)(&(data[6])));
                    break;
            }
            break;
        case ATCI_HCE_FLUSH_OCCURRED:
        case ATCI_HCE_MAX_SLOTS_CHNG:
        case ATCI_HCE_QOS_VIOLATION:
        case ATCI_HCE_LINK_SUPERVISION_TIMEOUT_CHNG:
        case ATCI_HCE_ENHANCED_FLUSH_COMPLETE:
            hci_event->handle = *((uint16_t *)(&(data[2])));
            break;
        case ATCI_HCE_LE_META_EVENT:
            switch(data[2])
            {
                case ATCI_HCE_LE_CONNECT_COMPLETE:
                    hci_event->status = data[3];
                    hci_event->handle = *((uint16_t *)(&(data[4])));
                    break;
                case ATCI_HCE_LE_CONNECT_UPDATE_COMPLETE:
                    hci_event->status = data[3];
                    hci_event->handle = *((uint16_t *)(&(data[4])));
                    break;
                case ATCI_HCE_LE_READ_REMOTE_FEATURES_COMPLETE:
                    hci_event->status = data[3];
                    hci_event->handle = *((uint16_t *)(&(data[4])));
                    break;
                case ATCI_HCE_LE_LONG_TERM_KEY_REQUEST_EVENT:
                    hci_event->handle = *((uint16_t *)(&(data[3])));
                    break;
                case ATCI_HCE_LE_ENHANCED_CONNECT_COMPLETE:
                    hci_event->status = data[3];
                    hci_event->handle = *((uint16_t *)(&(data[4])));
                    break;
            }
            break;
        default:
            //Not supported event
            break;
    }

    return 0;
}


/*--- Callback Function ---*/
static void acti_bt_task_msg_handler(at_command_bt_message_struct_t *message)
{
    uint8_t result = 0;
    atci_bt_hci_event hci_event;
    atci_response_t output = {{0}};
    uint8_t string[526] = {0};
    uint8_t *temp_str;

    LOG_MSGID_I(atci_bt, "msglen :0x%x",1, message->param1);
    temp_str = message->param2;
    for (int i = 0 ; i < message->param1; i++) {
       LOG_MSGID_I(atci_bt, "msg_handler--msg:%x",1, *(temp_str + i));
    }

    result = bt_hci_event_parser((uint8_t *)message->param2, message->param1, &hci_event);

    LOG_MSGID_I(atci_bt, "parser result:%d",1, result);

    if (result == 1) {
        atci_bt_error_handle();
    } else {
        result = atci_bt_hci_event_to_string(&hci_event, string, 522);
        LOG_MSGID_I(atci_bt, "event to string result:%d",1, result);
        if (result > 0) {
            bool is_complete;
            snprintf((char *)output.response_buf, sizeof(output.response_buf), "\r\n+EBTSHC:%s\r\n", (char *)string);
            LOG_MSGID_I(atci_bt, "event to string event:%d",1, hci_event.event);
            is_complete = acti_bt_check_event_complete(hci_event.event);
            if (is_complete) {
                LOG_MSGID_I(atci_bt, "event is complete.", 0);
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                acti_bt_delete_timer();
            } else {
                LOG_MSGID_I(atci_bt, "event is status.", 0);
                output.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                acti_bt_start_check_complete();
            }

            output.response_len = strlen((char *)output.response_buf);
        } else {
            STRNCPY((char *)output.response_buf, "Response data error!\n");
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            output.response_len = strlen((char *)output.response_buf);
            acti_bt_delete_timer();
        }
        atci_send_response(&output);
    }

}

void atci_bt_hci_cmd_rx_callback(uint8_t *data, uint32_t dataLen)
{
    at_command_bt_message_struct_t msg = {0, 0, {0}};
    msg.message_id = ATCI_BT_COMMAND_HCI_EVENT;
    msg.param1 = dataLen;
    memcpy(msg.param2, data, dataLen);
    xQueueSendFromISR(at_command_task_queue, &msg, 0);

    return;
}
void bt_driver_deinit_atci_mode_default(void)
{
    return;
}

bool bt_driver_register_atci_rx_callback_default(void * rx_callback)
{
    return true;
}

uint8_t bt_driver_send_hci_command_default(uint16_t opCode, uint8_t cmd_len, uint8_t * cmd)
{
    return 0;
}

bool bt_enable_relay_mode_default(hal_uart_port_t port)
{
    return true;
}

#if _MSC_VER >= 1500
    #pragma comment(linker, 
    "/alternatename:_bt_driver_register_atci_rx_callback=_bt_driver_register_atci_rx_callback_default")
    #pragma comment(linker, 
    "/alternatename:_bt_driver_deinit_atci_mode=_bt_driver_deinit_atci_mode_default")
    #pragma comment(linker, "/alternatename:_bt_driver_send_hci_command=_bt_driver_send_hci_command_default")
    #pragma comment(linker, "/alternatename:_bt_enable_relay_mode=_bt_enable_relay_mode_default")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION)
    #pragma weak bt_driver_register_atci_rx_callback = bt_driver_register_atci_rx_callback_default
    #pragma weak bt_driver_deinit_atci_mode = bt_driver_deinit_atci_mode_default
    #pragma weak bt_driver_send_hci_command = bt_driver_send_hci_command_default
    #pragma weak bt_enable_relay_mode = bt_enable_relay_mode_default
#else
    #error "Unsupported Platform"
#endif

/*--- Command handler Function ---*/
atci_status_t atci_cmd_hdlr_bt_power(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    bt_status_t result = BT_STATUS_FAIL;
    uint8_t register_result;

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { /*AT+EBTPW=<op>*/
            LOG_MSGID_I(atci_bt, "bt_power : %d",1, atci_bt_cntx_p->is_power);
            if (strncmp(parse_cmd->string_ptr, "AT+EBTPW=0", 10) == 0) {
                if (atci_bt_cntx_p->is_power) {
                    result = bt_driver_power_off();
                    bt_driver_deinit_atci_mode();
                    atci_bt_cntx_p->is_register_rx_callback = false;
                    acti_bt_task_deinit();
                    atci_bt_cntx_p->is_power = 0;
                } else {
                    result = BT_STATUS_SUCCESS;
                }
            } else if (strncmp(parse_cmd->string_ptr, "AT+EBTPW=1", 10) == 0) {

                if (!atci_bt_cntx_p->is_power) {
                    result = bt_driver_power_on();
                    if (result == BT_STATUS_SUCCESS) {
                        atci_bt_cntx_p->is_power = 1;
                        register_result = bt_driver_register_atci_rx_callback((void*)atci_bt_hci_cmd_rx_callback);
                        atci_bt_task_init();
                        if (register_result == 0) {
                            atci_bt_cntx_p->is_register_rx_callback = true;
                        } else {
                            atci_bt_cntx_p->is_register_rx_callback = false;
                        }
                    }
                } else {
                    result = BT_STATUS_SUCCESS;
                }
            } else {
                ; //not support
            }

            LOG_MSGID_I(atci_bt, "result: 0x%x",1, result);
            if (result == BT_STATUS_SUCCESS) {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            output.response_len = 0;
        }
        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            STRNCPY((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }

    atci_send_response(&output);
    return ATCI_STATUS_OK;
}

atci_status_t atci_cmd_hdlr_bt_enter_test_mode(atci_parse_cmd_param_t *parse_cmd)
{
    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);
    if (ATCI_CMD_MODE_ACTIVE == parse_cmd->mode) {
        bt_status_t result = BT_STATUS_SUCCESS;
        at_command_enter_test_mode = true;
        result = bt_power_off();
        if (BT_STATUS_SUCCESS != result) {
            atci_response_t output = {{0}};
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            output.response_len = 0;
            atci_send_response(&output);
            at_command_enter_test_mode = false;
        }
    }
    return ATCI_STATUS_OK;
}

static void atci_bt_cmd_task_main(void *pvParameters)
{

    at_command_bt_message_struct_t queue_item;
    //LOG_MSGID_I(atci_bt, "enter task main.", 0);

    at_command_task_queue = xQueueCreate(ATCI_COMMAND_QUEUE_SIZE, sizeof(at_command_bt_message_struct_t));
    while (1) {
        if (xQueueReceive(at_command_task_queue, &queue_item, portMAX_DELAY)) {
            //LOG_MSGID_I(atci_bt, "enter massge handler.", 0);
            acti_bt_task_msg_handler(&queue_item);
        }
    }

}

static void atci_bt_task_init(void)
{
    LOG_MSGID_I(atci_bt, "atci_bt_task_init.", 0);

    xTaskCreate(atci_bt_cmd_task_main, ATCI_COMMAND_TASK_NAME, ATCI_COMMAND_TASK_STACK_SIZE, NULL, ATCI_COMMAND_TASK_PRIORITY, &at_command_task_hdlr);
}

static void acti_bt_task_deinit(void)
{
    if (at_command_task_hdlr) {
        vTaskDelete(at_command_task_hdlr);
        at_command_task_hdlr = NULL;
    }

    if (at_command_task_queue) {
        vQueueDelete(at_command_task_queue);
        at_command_task_queue = NULL;
    }
}

static uint32_t atci_cmd_get_end_symbol_length(uint8_t *string)
{
    uint32_t i = strlen((char *)string);
    uint32_t end_simble_len = 0;

    for(i = i-1; i > 0; i--) {
        if(string[i] == '\r' || string[i] == '\n') {
            end_simble_len++;
        } else {
            break;
        }
    }

    return end_simble_len;
}

atci_status_t atci_cmd_hdlr_bt_send_hci_command(atci_parse_cmd_param_t *parse_cmd)
{
    int8_t result;
    atci_bt_hci_cmd hci_cmd;
    atci_response_t output = {{0}};

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { //AT+EBTSHC=<op>
            LOG_MSGID_I(atci_bt, "cmd_len:%d, name_len:%d, string_len:%d",3, strlen(parse_cmd->string_ptr), parse_cmd->name_len, parse_cmd->string_len);
            uint32_t end_simble_len = atci_cmd_get_end_symbol_length((uint8_t *)(parse_cmd->string_ptr));
            uint32_t op_length = strlen(parse_cmd->string_ptr) - parse_cmd->name_len - 1 - end_simble_len;
            result = atci_bt_hci_string_to_cmd((uint8_t *)parse_cmd->string_ptr + 10, op_length, &hci_cmd);
            LOG_MSGID_I(atci_bt, "string to hci cmd result:%d",1, result);
            if (result > 0) {
                result = bt_driver_send_hci_command(hci_cmd.op_code, hci_cmd.cmd_len, hci_cmd.cmd);
                atci_bt_cntx_p->is_sent_cmd = true;
                LOG_MSGID_I(atci_bt, "result:%d",1, result);
            } else {
                output.response_flag = 0;
                STRNCPY((char *)output.response_buf, "Command Error\n");
                output.response_len = strlen((char *)output.response_buf);
                atci_send_response(&output);
            }
        }

        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            STRNCPY((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
        }
        break;
    }

    return ATCI_STATUS_OK;
}

static int8_t atci_bt_hci_extract_data_from_string(uint8_t *string, uint32_t string_len, uint8_t *data_buf)
{
    int8_t parse_result = 0;
    uint32_t string_pos = 0;
    uint32_t data_pos = 0;

    LOG_I(atci_bt, "command: %s, len:%d", string, string_len);

    while(string_len > string_pos){
		
        if(*(string + string_pos) == ',') {
			string_pos++;
            continue;
		}
		parse_result = atci_bt_str2hex(string + string_pos, 2, data_buf + data_pos);
	    if (parse_result <= 0) {
	        return -1;
	    }
		string_pos += 2;
        data_pos += 1;
	}

    return 1;
}

atci_status_t atci_cmd_hdlr_bt_send_hci_command_to_host(atci_parse_cmd_param_t *parse_cmd)
{
    int8_t result;
    atci_response_t output = {{0}};

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { //AT+EBTSHCD=<op>
            LOG_MSGID_I(atci_bt, "cmd_len:%d, name_len:%d, string_len:%d",3, strlen(parse_cmd->string_ptr), parse_cmd->name_len, parse_cmd->string_len);
            uint32_t end_simble_len = atci_cmd_get_end_symbol_length((uint8_t *)(parse_cmd->string_ptr));
            uint32_t op_length = strlen(parse_cmd->string_ptr) - parse_cmd->name_len - 1 - end_simble_len;
            uint8_t raw_data[259]={0};
            result = atci_bt_hci_extract_data_from_string((uint8_t *)parse_cmd->string_ptr + 11, op_length, raw_data);
            LOG_MSGID_I(atci_bt, "string to hci cmd result:%d",1, result);
            if (result > 0) {
            bt_hci_cmd_raw_data_send(raw_data);
                output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                output.response_len = 0;
                atci_send_response(&output);				
            } else {
                output.response_flag = 0;
                STRNCPY((char *)output.response_buf, "Command Error\n");
                output.response_len = strlen((char *)output.response_buf);
                atci_send_response(&output);
            }
        }

        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            STRNCPY((char *)output.response_buf, "Not Support\n");
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
        }
        break;
    }

    return ATCI_STATUS_OK;
}

/*--- Static Function ---*/
static void atci_bt_error_handle(void)
{
    atci_response_t output;
    LOG_MSGID_I(atci_bt, "exception handle.", 0);

    output.response_len = 0;
    output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
    atci_send_response(&output);
}

static int8_t atci_bt_str2hex(uint8_t *string, uint32_t string_len, uint8_t data[])
{
    uint32_t i;
    uint8_t value = 0;
    uint8_t composed_value = 0;
    LOG_I(atci_bt, "string : %s, string_len : %d", string, string_len);

    for (i = 0; i < string_len; i++) {
        if ('0' <= string[i] && '9' >= string[i]) {
            value = string[i] - '0';
        } else if ('a' <= string[i] && 'f' >= string[i]) {
            value = string[i] - 'a' + 10;
        } else if ('A' <= string[i] && 'F' >= string[i]) {
            value = string[i] - 'A' + 10;
        } else {
            return -1;
        }

        if (i % 2 == 0) {
            composed_value += value * 16;
        } else {
            composed_value += value;
        }

        if (i % 2 == 1) {
            data[(i - 1) / 2] = composed_value;
            composed_value = 0;
        }

    }

    return 1;
}

static int8_t atci_bt_hex2str(uint8_t data[], uint8_t data_len, uint8_t *string, uint32_t string_len)
{
    uint32_t i;
    uint8_t high_8bits_string = 0;
    uint8_t low_8bits_string = 0;

    if ( (2 * data_len) > string_len ) {
        return -1;
    }

    for (i = 0; i < data_len; i++) {
        /*parse high 8 bits value*/
        high_8bits_string = data[i] / 16;
        if (high_8bits_string <= 9) {
            string[2 * i] = high_8bits_string + '0';
        } else if (10 <= high_8bits_string && high_8bits_string <= 15) {
            string[2 * i] = high_8bits_string - 10 + 'A';
        } else {
            return -2;
        }

        /*parse low 8 bits value*/
        low_8bits_string = data[i] % 16;
        if (low_8bits_string <= 9) {
            string[2 * i + 1] = low_8bits_string + '0';
        } else if (10 <= low_8bits_string && low_8bits_string <= 15) {
            string[2 * i + 1] = low_8bits_string - 10 + 'A';
        } else {
            return -3;
        }
    }

    return 1;
}

static int8_t atci_bt_hci_event_to_string(atci_bt_hci_event *hci_event, uint8_t *string, uint32_t string_len)
{
    int8_t parse_result = 0;
    uint8_t data[260];
    uint32_t string_pos = 0;
    uint32_t data_pos = 0;

    LOG_MSGID_I(atci_bt, "event parmslen:%d",1, hci_event->parmslen);

    /*init data[]*/
    data[0] = hci_event->event;
    data[1] = hci_event->status ;
    data[2] = (uint8_t)((hci_event->handle & 0xFF00) >> 8);
    data[3] = (uint8_t)(hci_event->handle & 0x00FF) ;
    data[4] = hci_event->parmslen;
    memcpy(data + 5, hci_event->parms, hci_event->parmslen);

    /*1. parser hci_event->event*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len);
    if (parse_result <= 0 ) {
        return -1;
    }
    string_pos += 2;
    data_pos += 1;
    string[string_pos++] = ',';

    /*2. parser hci_event->status*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -2;
    }
    string_pos += 2;
    data_pos += 1;
    string[string_pos++] = ',';

    /*3. parser hci_event->handle*/
    parse_result = atci_bt_hex2str(data + data_pos, 2, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -3;
    }
    string_pos += 4;
    data_pos += 2;
    string[string_pos++] = ',';

    /*4. parser hci_event->parmslen*/
    parse_result = atci_bt_hex2str(data + data_pos, 1, string + string_pos, string_len - string_pos);
    if (parse_result <= 0 ) {
        return -5;
    }
    string_pos += 2;
    data_pos += 1;

    /*5. parser hci_event->parms[256]*/
    if (hci_event->parmslen > 0) {
        string[string_pos++] = ',';
        parse_result = atci_bt_hex2str(data + data_pos, hci_event->parmslen, string + string_pos, string_len - string_pos);
        if (parse_result <= 0 ) {
            return -6;
        }
        string_pos += 2 * hci_event->parmslen;
        data_pos += hci_event->parmslen;
    }

    string[string_pos] = '\0';

    return 1;
}

static int8_t atci_bt_hci_string_to_cmd(uint8_t *string, uint32_t string_len, atci_bt_hci_cmd *hci_cmd)
{
    int8_t parse_result = 0;
    uint8_t data[259];
    uint32_t string_pos = 0;
    uint32_t data_pos = 0;

    LOG_I(atci_bt, "command: %s, len:%d", string, string_len);

    /*reset hci cmd*/
    hci_cmd->op_code = 0;
    hci_cmd->cmd_len = 0;
    memset(hci_cmd->cmd, 0x0, 256);

    /*parse hci cmd string*/
    /*0401, 02, 1234*/

    /*parse opcode*/
    parse_result = atci_bt_str2hex(string + string_pos, 4, data + data_pos);
    if (parse_result <= 0) {
        return -1;
    }
    string_pos += 4;
    data_pos += 2;

    /*parse 1st ','*/
    if (string[string_pos] != ',') {
        return -2;
    }
    string_pos += 1;

    /*parse cmd length*/
    parse_result = atci_bt_str2hex(string + string_pos, 2, data + data_pos);
    if (parse_result <= 0) {
        return -3;
    }

    if (data[data_pos] == 0) {
        goto exit;
    }

    string_pos += 2;
    data_pos += 1;

    /*parse 2nd ','*/
    if (string[string_pos] != ',') {
        return -4;
    }
    string_pos += 1;

    LOG_I(atci_bt, "string : %s, str_pos:%d, data_pos = %d", string + string_pos, string_pos, data_pos);
    /*parse cmd data*/
    if (data[2] > 0) {
        parse_result = atci_bt_str2hex(string + string_pos, string_len - string_pos, data + data_pos);
        if (parse_result <= 0) {
            return -5;
        }
    }

exit:
    hci_cmd->op_code = ((data[0] << 8) | data[1]);
    hci_cmd->cmd_len = data[2];
    if (hci_cmd->cmd_len > 0) {
        memcpy(hci_cmd->cmd, data + 3, hci_cmd->cmd_len);
    }

    return 1;
}

static hal_uart_baudrate_t atci_bt_string_to_buad(const uint8_t *string)
{
    int32_t num = 0;
    uint32_t i = 0;

    num = atoi((const char *)string);
    for (i = 0;i < sizeof(bt_uart_buad_table)/sizeof(atci_bt_uart_buad_t);i++) {
        if (num == bt_uart_buad_table[i].index) {
            return bt_uart_buad_table[i].buad_rate;
        }
    }
    return HAL_UART_BAUDRATE_MAX;
}
#endif /*#if 0*/

extern bool bt_driver_enter_relay_mode(uint8_t port);
extern void bt_driver_deinit_relay_mode(void);
extern void bt_driver_relay_register_callbacks(void *callback);
extern void bt_driver_handle_relay_port_data(void);

#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_bt_driver_enter_relay_mode=_bt_driver_enter_relay_mode_default")
    #pragma comment(linker, "/alternatename:_bt_driver_deinit_relay_mode=_bt_driver_deinit_relay_mode_default")
    #pragma comment(linker, "/alternatename:_bt_driver_relay_register_callbacks=_bt_driver_relay_register_callbacks_default")
    #pragma comment(linker, "/alternatename:_bt_driver_handle_relay_port_data=_bt_driver_handle_relay_port_data")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION)
    #pragma weak bt_driver_enter_relay_mode = bt_driver_enter_relay_mode_default
    #pragma weak bt_driver_deinit_relay_mode = bt_driver_deinit_relay_mode_default
    #pragma weak bt_driver_relay_register_callbacks = bt_driver_relay_register_callbacks_default
    #pragma weak bt_driver_handle_relay_port_data = bt_driver_handle_relay_port_data_default
#else
    #error "Unsupported Platform"
#endif

bool bt_driver_enter_relay_mode_default(uint8_t port)
{
    return false;
}

void bt_driver_deinit_relay_mode_default()
{
    return;
}

void bt_driver_relay_register_callbacks_default(void *callback)
{
    return;
}

void bt_driver_handle_relay_port_data_default()
{
    return;
}

extern hal_uart_port_t g_atci_uart_port;
extern atci_status_t atci_deinit(hal_uart_port_t port);

static uint8_t atci_bt_relay_port_buad = HAL_UART_BAUDRATE_115200;

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
#include "serial_port.h"
static uint32_t atci_bt_port_service_relay_port_handle = 0;
static uint8_t atci_bt_port_service_usb_init_success = 0;

uint32_t atci_bt_relay_port_service_read_data(uint8_t *buf, uint32_t buf_len, uint32_t offset);
uint32_t atci_bt_relay_port_service_write_data(uint8_t *buf, uint32_t buf_len);
uint8_t atci_bt_relay_port_service_init(uint8_t port);
uint8_t atci_bt_relay_port_service_deinit();

const atci_bt_relay_callbacks atci_bt_relay_cb = {
    .read_data = atci_bt_relay_port_service_read_data,
    .write_data = atci_bt_relay_port_service_write_data,
    .init = atci_bt_relay_port_service_init,
    .deinit = atci_bt_relay_port_service_deinit,
};

uint32_t atci_bt_relay_port_service_read_data(uint8_t *buf, uint32_t buf_len, uint32_t offset)
{
    assert(buf && buf_len && atci_bt_port_service_relay_port_handle);
    serial_port_read_data_t read_data;
    read_data.buffer = (uint8_t*)(buf + offset);
    read_data.size = buf_len - offset;
    bt_os_layer_serial_port_control((uint32_t)atci_bt_port_service_relay_port_handle, (uint8_t)SERIAL_PORT_CMD_READ_DATA, (void*)&read_data);
    return read_data.ret_size;
}

uint32_t atci_bt_relay_port_service_write_data(uint8_t *buf, uint32_t buf_len)
{
    assert(buf && buf_len && atci_bt_port_service_relay_port_handle);
    serial_port_write_data_t write_data;
    write_data.data = buf;
    write_data.size = buf_len;
    bt_os_layer_serial_port_control((uint32_t)atci_bt_port_service_relay_port_handle, (uint8_t)SERIAL_PORT_CMD_WRITE_DATA_BLOCKING, &write_data);
    return write_data.ret_size;
}

void atci_bt_relay_port_service_data_callback(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
{
    //uint32_t current_read_size = 0;
    //serial_port_read_data_t read_data;
    //serial_port_status_t status = SERIAL_PORT_STATUS_FAIL;
    switch(event) 
    {
        case SERIAL_PORT_EVENT_READY_TO_READ: {
            bt_driver_handle_relay_port_data();
            break;
        }
        case SERIAL_PORT_EVENT_BT_CONNECTION: {
            atci_bt_port_service_usb_init_success = 1;
            break;
        }
        case SERIAL_PORT_EVENT_READY_TO_WRITE: {
            break;
        }
        default:
            break;
    }
}

uint8_t atci_bt_relay_port_service_init(uint8_t port)
{
    serial_port_status_t status;
    serial_port_open_para_t serial_port_open_para;
    serial_port_setting_uart_t uart_setting;
    serial_port_setting_uart_t default_uart_setting;
    uint8_t pollPeriod = 1;
    uint32_t timer = 0;
    
    if ((serial_port_dev_t)port >= SERIAL_PORT_DEV_MAX) {
        return SERIAL_PORT_STATUS_FAIL;
    }
    if (g_atci_uart_port == (hal_uart_port_t)port) {
        LOG_MSGID_I(atci_bt, "relay mode use atci port", 0);
        atci_deinit(g_atci_uart_port);
    }

    if (atci_bt_port_service_relay_port_handle) {
        serial_port_status_t status = bt_os_layer_serial_port_close((uint32_t)atci_bt_port_service_relay_port_handle);
        if (status != SERIAL_PORT_STATUS_OK) {
            return SERIAL_PORT_STATUS_OK;
        }
    }
    uart_setting.baudrate = atci_bt_relay_port_buad;
    serial_port_status_t read_ret = serial_port_config_read_dev_setting(port, (serial_port_dev_setting_t *)&default_uart_setting);
    serial_port_config_write_dev_setting(port, (serial_port_dev_setting_t *)&uart_setting);
    // register the serial user event callback
    serial_port_open_para.callback = atci_bt_relay_port_service_data_callback;
    serial_port_open_para.tx_buffer_size = 1024;
    serial_port_open_para.rx_buffer_size = 1024;
    // for serial port configuration
    status = bt_os_layer_serial_port_open((uint16_t)port, (void*)&serial_port_open_para, (uint32_t*)&atci_bt_port_service_relay_port_handle);
    if(status == SERIAL_PORT_STATUS_UNINITIALIZED) {
        status = SERIAL_PORT_STATUS_OK;
        while (!atci_bt_port_service_usb_init_success) {
            bt_os_layer_sleep_task(pollPeriod);
            timer += pollPeriod;
            if (timer >= 1000) {
                status = SERIAL_PORT_STATUS_FAIL;
                break;
            }
        }
    } else if(status == SERIAL_PORT_STATUS_OK) {
        status = SERIAL_PORT_STATUS_OK;
    } else {
        status = SERIAL_PORT_STATUS_FAIL;
    }
    if(read_ret == SERIAL_PORT_STATUS_OK) {
        serial_port_config_write_dev_setting(port, (serial_port_dev_setting_t *)&default_uart_setting);
    }

    return status;
}

uint8_t atci_bt_relay_port_service_deinit()
{
    serial_port_status_t status = SERIAL_PORT_STATUS_OK;
    if (atci_bt_port_service_relay_port_handle) {
        status = serial_port_close(atci_bt_port_service_relay_port_handle);
        atci_bt_port_service_relay_port_handle = 0;
    }
    return (uint8_t)status;
}

#elif defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)

#include "mux.h"
#ifdef MTK_RACE_COMMAND_ENABLE
#include "race_cmd.h"
#include "race_xport.h"
#endif
static mux_handle_t atci_bt_relay_mux_handle = 0;
static mux_port_t atci_bt_relay_mux_port = 0;
static uint8_t atci_bt_relay_is_port_1wire = 0;
uint32_t atci_bt_relay_mux_read_data(uint8_t *buf, uint32_t buf_len, uint32_t offset);
uint32_t atci_bt_relay_mux_send_data(uint8_t *buf, uint32_t buf_len);
uint8_t atci_bt_relay_mux_init(uint8_t port);
uint8_t atci_bt_relay_mux_deinit();

const atci_bt_relay_callbacks atci_bt_relay_cb = {
    .read_data = atci_bt_relay_mux_read_data,
    .write_data = atci_bt_relay_mux_send_data,
    .init = atci_bt_relay_mux_init,
    .deinit = atci_bt_relay_mux_deinit,
};

uint32_t atci_bt_relay_mux_read_data(uint8_t *buf, uint32_t buf_len, uint32_t offset)
{
    uint32_t data_len = 0;
    mux_buffer_t buffer;
    buffer.p_buf = buf + offset;
    buffer.buf_size = buf_len;
    mux_status_t status = MUX_STATUS_OK;
    status = mux_rx(atci_bt_relay_mux_handle, &buffer, &data_len);
    LOG_MSGID_I(atci_bt, "atci bt relay read data status : %d, read_len:%d",2, status, data_len);
    return data_len;
}

uint32_t atci_bt_relay_mux_send_data(uint8_t *buf, uint32_t buf_len)
{
    uint32_t data_len = 0;
    mux_buffer_t pdata;

    pdata.buf_size = buf_len;
    pdata.p_buf = buf;
    mux_status_t status = MUX_STATUS_OK;
    if (atci_bt_relay_is_port_1wire) {
#ifdef MTK_SMART_CHARGER_1WIRE_RACE_ENABLE
        LOG_MSGID_I(atci_bt, "atci bt relay set 1wire tx mode", 0);
        hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_UART1_TXD);
        hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_GPIO5);
#endif
    }
    status = mux_tx(atci_bt_relay_mux_handle, &pdata, 1, &data_len);
    LOG_MSGID_I(atci_bt, "atci bt relay send data status : %d, send_len:%d",2, status, data_len);
    return data_len;
}

void atci_bt_relay_mux_callback(mux_handle_t handle, mux_event_t event, uint32_t data_len, void *user_data)
{
    //printf("enter relay rx callback: %s, event: %d, data_len: %d", __FUNCTION__, event, data_len);
    switch (event) {
        case MUX_EVENT_READY_TO_READ: {
            bt_driver_handle_relay_port_data();
        } break;
        case MUX_EVENT_READY_TO_WRITE: {

        } break;

        default:
        break;
    }
}

void atci_bt_relay_mux_tx_no_packed_callback(mux_handle_t handle, const mux_buffer_t payload[], uint32_t buffers_counter, mux_buffer_t *head, mux_buffer_t *tail, void *user_data)
{
    head->p_buf = NULL;
    tail->p_buf = NULL;
    head->buf_size = 0;
    tail->buf_size = 0;
}

void atci_bt_relay_mux_rx_no_packed_callback(mux_handle_t  *handle, mux_buffer_t buffers[], uint32_t buffers_counter,uint32_t *consume_len, uint32_t *package_len, void *user_data)
{
    uint32_t total_size,i;
    mux_handle_t p_handle;
    *package_len = 0;
    *consume_len = 0;
    total_size = 0;
    for(i = 0;i < buffers_counter;i++) {
        total_size += buffers[i].buf_size;
    }
    *package_len = total_size;
    if(mux_query_user_handle(atci_bt_relay_mux_handle,"HCI_CMD",&p_handle) == MUX_STATUS_OK){
        *handle = p_handle;
        *consume_len = 0;
    }else {
        *handle = 0;
        *consume_len = total_size;
        return;
    }
}

extern void log_global_turn_off(void) ;
extern mux_status_t mux_change_port_setting(mux_port_t port, mux_port_setting_t *setting);
uint8_t atci_bt_relay_mux_init(uint8_t port)
{
    mux_status_t mux_status = MUX_STATUS_ERROR;
    mux_port_setting_t setting;
    setting.tx_buffer_size = 1200;
    setting.rx_buffer_size = 1200;
    setting.dev_setting.uart.uart_config.baudrate = atci_bt_relay_port_buad;
    setting.dev_setting.uart.uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    setting.dev_setting.uart.uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    setting.dev_setting.uart.uart_config.parity = HAL_UART_PARITY_NONE;
    setting.dev_setting.uart.flowcontrol_type = MUX_UART_NONE_FLOWCONTROL;
    mux_protocol_t bt_relay_mux_protocol = {
        .tx_protocol_callback = atci_bt_relay_mux_tx_no_packed_callback,
        .rx_protocol_callback = atci_bt_relay_mux_rx_no_packed_callback,
    };

    mux_handle_t syslog_mux_handle = 0, race_mux_handle = 0;
    mux_status_t syslog_status, race_status;
    syslog_status = mux_query_user_handle(port, "SYSLOG", &syslog_mux_handle);
    race_status = mux_query_user_handle(port, "RACE_CMD", &race_mux_handle);
    if (MUX_STATUS_OK == syslog_status && syslog_mux_handle != 0) {
        /*this port has used by system log, so relay port should coexist with system log port*/
        log_global_turn_off();
        mux_status = mux_change_port_setting(port, &setting);
        LOG_MSGID_I(atci_bt, "mux_change_port_setting status : %d",1, mux_status);
        if (mux_status != MUX_STATUS_OK) {
            return mux_status;
        }
    }
#ifdef MTK_RACE_COMMAND_ENABLE
    else if (MUX_STATUS_OK == race_status && race_mux_handle != 0) {
        if (RACE_SERIAL_PORT_TYPE_1WIRE == race_get_channel_id_by_port_handle(race_mux_handle)) {
            /*this port has used by race cmd with uart 1wire, so relay port should coexist with race cmd port*/
            atci_bt_relay_is_port_1wire = 1;
            LOG_MSGID_I(atci_bt, "bt relay by uart one wire", 0);
        } else if (RACE_SERIAL_PORT_TYPE_USB == race_get_channel_id_by_port_handle(race_mux_handle)) {
            /*this port has used by race cmd with USB, so relay port should coexist with race cmd port*/
            LOG_MSGID_I(atci_bt, "bt relay by USB", 0);
        }
    }
#endif
    else {
        /*bt relay exclusive this port*/
        uint32_t port_user_count = 0;
        mux_status = mux_query_port_user_number(port, &port_user_count);
        if (MUX_STATUS_OK == mux_status && port_user_count > 0) {
            mux_port_assign_t *port_assign = (mux_port_assign_t *)pvPortMalloc(port_user_count * sizeof(mux_port_assign_t));
            if (port_assign == NULL) {
                return MUX_STATUS_ERROR;
            }
            mux_status = mux_query_port_user_name(port, port_assign);
            if (MUX_STATUS_OK != mux_status) {
                vPortFree(port_assign);
                return mux_status;
            }
            /*close mux user in this port first*/
            for (uint32_t i = 0; i < port_user_count; i++) {
                if (0 == strncmp(port_assign[i].name, "ATCI", strlen("ATCI"))) {
                    atci_status_t atci_status = atci_deinit(port);
                    LOG_MSGID_I(atci_bt, "atci_deinit status : %d",1, atci_status);
                    if (atci_status != ATCI_STATUS_OK) {
                        vPortFree(port_assign);
                        return MUX_STATUS_ERROR;
                    }
                }
#ifdef MTK_RACE_COMMAND_ENABLE
                else if (0 == strncmp(port_assign[i].name, "RACE_CMD", strlen("RACE_CMD"))) {
                    race_status_t race_status = race_serial_port_close(race_get_channel_id_by_port_handle(race_mux_handle));
                    LOG_MSGID_I(atci_bt, "race_deinit status : %d",1, race_status);
                    if (race_status != RACE_STATUS_OK) {
                        vPortFree(port_assign);
                        return MUX_STATUS_ERROR;
                    }
                }
#endif
                else {
                    mux_handle_t handle = 0;
                    mux_status = mux_query_user_handle(port, port_assign[i].name, &handle);
                    if (MUX_STATUS_OK == mux_status && handle != 0) {
                        mux_status = mux_close(handle);
                        LOG_MSGID_I(atci_bt, "mux close status : %d, user : %s", 2, mux_status, port_assign[i].name);
                        if (MUX_STATUS_OK != mux_status) {
                            vPortFree(port_assign);
                            return mux_status;
                        }
                    }
                }
            }
            vPortFree(port_assign);

            mux_status = mux_deinit(port);
            LOG_MSGID_I(atci_bt, "mux_deinit status : %d",1, mux_status);
            if (mux_status != MUX_STATUS_OK) {
                return mux_status;
            }
        }
        /*init relay port*/
        mux_status = mux_init(port, &setting, &bt_relay_mux_protocol);
        LOG_MSGID_I(atci_bt, "mux_init status : %d",1, mux_status);
        if (mux_status != MUX_STATUS_OK) {
            return mux_status;
        }
    }
    /*open relay port*/
    atci_bt_relay_mux_port = port;
    mux_status = mux_open(atci_bt_relay_mux_port, "HCI_CMD", &atci_bt_relay_mux_handle, atci_bt_relay_mux_callback, NULL);
    LOG_MSGID_I(atci_bt, "mux_open status : %d",1, mux_status);

    return mux_status;
}

uint8_t atci_bt_relay_mux_deinit()
{
    mux_status_t mux_status = MUX_STATUS_OK;
    if (atci_bt_relay_mux_handle) {
        mux_status = mux_close(atci_bt_relay_mux_handle);
        LOG_MSGID_I(atci_bt,"relay mode mux close status:%d", 1, mux_status);
        mux_status = mux_deinit(atci_bt_relay_mux_port);
        LOG_MSGID_I(atci_bt,"relay mode mux deinit status:%d", 1, mux_status);
        atci_bt_relay_mux_handle = 0;
    }
    return mux_status;
}
#endif

#ifdef ATCI_BT_TEST_MODE_DEBUG_ENABLE
static void atci_bt_relay_test_mode_notify_callback(bt_device_manager_test_mode_event_t event_id, void *param)
{
    bt_device_manager_test_mode_changed_ind_t *ind = (bt_device_manager_test_mode_changed_ind_t *)param;
    if (event_id == BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND) {
        LOG_MSGID_I(atci_bt,"bt_device_manager_test_mode mode change, event_id:%d, mode:%d, dut_state:%d, status:%x", 4, event_id, ind->mode, ind->dut_state, ind->status);
    }
}
#endif

atci_status_t atci_cmd_hdlr_bt_relay(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t *output = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (output == NULL) {
        return ATCI_STATUS_ERROR;
    }
    bool result = false;

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            if (0 == strncmp((char *)(parse_cmd->string_ptr+9),"SBUAD", 5)) {
                /*AT+EBTER=SBUAD,115200*/
                hal_uart_baudrate_t buad;
                buad = atci_bt_string_to_buad((uint8_t *)(parse_cmd->string_ptr + 15));
                if (buad == HAL_UART_BAUDRATE_MAX) {
                    output->response_len = 0;
                    output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    LOG_MSGID_I(atci_bt, "Invalid uart buad", 0);
                    break;
                }
                LOG_MSGID_I(atci_bt, "uart buad = %d\r\n", 1, buad);
                atci_bt_relay_port_buad = buad;
                output->response_len = 0;
                output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == strncmp((char *)(parse_cmd->string_ptr+9),"GBUAD", 5)) {
                /*AT+EBTER=GBUAD*/
                uint32_t i;
                for (i = 0;i < sizeof(bt_uart_buad_table) / sizeof(atci_bt_uart_buad_t);i++) {
                    if (bt_uart_buad_table[i].buad_rate == atci_bt_relay_port_buad) {
                        LOG_MSGID_I(atci_bt, "relay uart buad is = %d", 1, bt_uart_buad_table[i].index);
                        break;
                    }
                }
                if (i < sizeof(bt_uart_buad_table) / sizeof(atci_bt_uart_buad_t)) {
                  snprintf((char *)output->response_buf, sizeof(output->response_buf), "+Get Relay Port Buad:%ld\r\n",bt_uart_buad_table[i].index);
                  output->response_len = strlen((char *)output->response_buf);
                  output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                  output->response_len = 0;
                  output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
                  LOG_MSGID_I(atci_bt, "relay uart buad is invalid", 0);
                }
            } else if (0 == strncmp((char *)(parse_cmd->string_ptr+9),"EXIT", 4)) {
                bt_driver_deinit_relay_mode();
                output->response_len = 0;
                output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            }

#ifdef ATCI_BT_TEST_MODE_DEBUG_ENABLE
            else if (0 == strncmp((char *)(parse_cmd->string_ptr+9),"REG", 3)) {
                bt_device_manager_test_mode_register_callback(atci_bt_relay_test_mode_notify_callback);
                output->response_len = 0;
                output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == strncmp((char *)(parse_cmd->string_ptr+9),"DREG", 4)) {
                bt_device_manager_test_mode_deregister_callback(atci_bt_relay_test_mode_notify_callback);
                output->response_len = 0;
                output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == strncmp((char *)(parse_cmd->string_ptr+9),"GET_MODE", 8)) {
                bt_device_manager_test_mode_dut_state_t dut_state;
                bt_device_manager_test_mode_t test_mode = bt_device_manager_get_test_mode();
                LOG_MSGID_I(atci_bt, "get test mode: %d",1, test_mode);
                dut_state = bt_device_manager_test_mode_get_dut_state();
                LOG_MSGID_I(atci_bt, "get dut state: %d",1, dut_state);
                snprintf((char *)output->response_buf, sizeof(output->response_buf), "+Get TEST MODE:%d, DUT STATE:%d\r\n",test_mode, dut_state);
                output->response_len = strlen((char *)output->response_buf);
                output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == strncmp((char *)(parse_cmd->string_ptr+9),"SET_DUT_MIX", 11)) {
                bt_status_t ret = bt_device_manager_set_test_mode(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX);
                LOG_MSGID_I(atci_bt, "set dut mix result: %x",1, ret);
                snprintf((char *)output->response_buf, sizeof(output->response_buf), "+SET DUT MIX result:%ld\r\n",ret);
                output->response_len = strlen((char *)output->response_buf);
                output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == strncmp((char *)(parse_cmd->string_ptr+9),"SET_DUT_ONLY", 12)) {
                bt_status_t ret = bt_device_manager_set_test_mode(BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY);
                LOG_MSGID_I(atci_bt, "set dut only result: %x",1, ret);
                snprintf((char *)output->response_buf, sizeof(output->response_buf), "+SET DUT ONLY result:%ld\r\n",ret);
                output->response_len = strlen((char *)output->response_buf);
                output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
            }
#endif
            else {
                /*Response for tool at first, because of port deinit*/
                output->response_len = 0;
                output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
                atci_send_response(output);
#ifdef HAL_SLEEP_MANAGER_ENABLED
                /*to prevent mcu go to sleep after enter relay mode*/
                extern uint8_t sleep_manager_handle; 
                hal_sleep_manager_lock_sleep(sleep_manager_handle);
#endif
                uint8_t port = *((uint8_t *)parse_cmd->string_ptr + 9) - '0';
                LOG_MSGID_I(atci_bt, "en relay, port: %d",1, port);
                bt_driver_relay_register_callbacks((void *)&atci_bt_relay_cb);
                result = bt_driver_enter_relay_mode(port);
                LOG_MSGID_I(atci_bt, "en relay result, result: %d",1, result);
                bt_power_on_set_config_type(BT_POWER_ON_RELAY);
                vPortFree(output);
                return ATCI_STATUS_OK;
            }
        }
        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:

        break;
        default: {
            STRNCPY((char *)output->response_buf, "Not Support\n");
            output->response_len = strlen((char *)output->response_buf);
            output->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }

    atci_send_response(output);
    vPortFree(output);
    return ATCI_STATUS_OK;
}



/*---ATA Command handler Function Begain---*/
static void atci_bt_ata_response_result(bool is_success, char* string);
static void atci_bt_ata_cmd_paser(uint8_t *string);
static void atci_bt_ata_power_on_req(void);
static void atci_bt_ata_power_on_cnf(bt_status_t status, void*buf);
static void atci_bt_ata_power_off_req(void);
static void atci_bt_ata_power_off_cnf(bt_status_t status, void*buf);
static void atci_bt_ata_inquiry_ind(bt_status_t status, void*buf);
static void atci_bt_ata_inquiry_complete_ind(bt_status_t status, void*buf);
static void atci_bt_ata_read_remote_name_complete_ind(bt_status_t status, void*buf);
static void atci_bt_ata_search_handler(void);
static void atci_bt_ata_dut_handler(void);
static void atci_bt_ata_tx_config_handler(char* string);
static void atci_bt_ata_dut_config_handler(char *string);

#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_bt_driver_dut_mode_set_dut_address=_bt_driver_dut_mode_set_dut_address_default")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION)
    #pragma weak bt_driver_dut_mode_set_dut_address = bt_driver_dut_mode_set_dut_address_default
#else
    #error "Unsupported Platform"
#endif

void bt_driver_dut_mode_set_dut_address_default(bt_bd_addr_t addr)
{

}

static void atci_bt_ata_response_result(bool is_success, char* string)
{
    atci_response_t output = {{0}};
    LOG_MSGID_I(atci_bt, "response result:%d.",1, is_success);
    if (string != NULL) {
        strncpy((char *)output.response_buf, string, sizeof(output.response_buf) - 1);
    }
    output.response_len = 0;
    if (is_success) {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
    } else {
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }
    atci_send_response(&output);
}

#ifdef MTK_BUILD_SMT_LOAD
static void atci_bt_ata_generate_local_address(bt_bd_addr_t addr)
{
    nvdm_status_t status;
    int8_t i;
    uint32_t random_seed;
    uint32_t size = 12;
    uint8_t buffer[18] = {0};
    uint8_t tmp_buf[3] = {0};
    bt_bd_addr_t tempaddr = {0};
    bt_bd_addr_t *local_addr = NULL;
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
    if (memcmp(addr, tempaddr, sizeof(bt_bd_addr_t)) == 0) {
        LOG_MSGID_I(atci_bt, "[BT]Empty bt address after bt_gap_le_get_local_address()\n", 0);
        LOG_MSGID_I(atci_bt, "[BT]Try to read from NVDM.\n", 0);
        local_addr = bt_device_manager_get_local_address();
        if (!memcmp(local_addr, tempaddr, sizeof(bt_bd_addr_t))) {
            memcpy(addr, local_addr, sizeof(bt_bd_addr_t));
        } else {
            LOG_MSGID_I(atci_bt, "[BT]Failed to Read from NVDM:%d.\n",1, status);
            ret = hal_trng_init();
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_MSGID_I(atci_bt, "[BT]generate_random_address--error 1", 0);
            }
            for (i = 0; i < 30; ++i) {
                ret = hal_trng_get_generated_random_number(&random_seed);
                if (HAL_TRNG_STATUS_OK != ret) {
                    LOG_MSGID_I(atci_bt, "[BT]generate_random_address--error 2", 0);
                }
                LOG_MSGID_I(atci_bt, "[BT]generate_random_address--trn: 0x%x",1, random_seed);
            }
            /* randomly generate address */
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_MSGID_I(atci_bt, "[BT]generate_random_address--error 3", 0);
            }
            LOG_MSGID_I(atci_bt, "[BT]generate_random_address--trn: 0x%x",1, random_seed);
            addr[0] = random_seed & 0xFF;
            addr[1] = (random_seed >> 8) & 0xFF;
            addr[2] = (random_seed >> 16) & 0xFF;
            addr[3] = (random_seed >> 24) & 0xFF;
            ret = hal_trng_get_generated_random_number(&random_seed);
            if (HAL_TRNG_STATUS_OK != ret) {
                LOG_MSGID_I(atci_bt, "[BT]generate_random_address--error 3", 0);
            }
            LOG_MSGID_I(atci_bt, "[BT]generate_random_address--trn: 0x%x",1, random_seed);
            addr[4] = random_seed & 0xFF;
            addr[5] = (random_seed >> 8) & 0xCF;
            hal_trng_deinit();
            bt_device_manager_store_local_address(&addr);
        }
    } else {
        bt_device_manager_store_local_address(&addr);
    }
}
#endif /*MTK_BUILD_SMT_LOAD*/

static void atci_bt_ata_power_on_req(void)
{
    LOG_MSGID_I(atci_bt, "power on req, host power on: %d",1, atci_bt_cntx_p->is_host_power_on);
   if (!atci_bt_cntx_p->is_host_power_on ) {
#ifdef MTK_BUILD_SMT_LOAD
        bt_bd_addr_t local_public_addr = {0};
        atci_bt_ata_generate_local_address(local_public_addr);
        bt_status_t result = bt_power_on(local_public_addr, NULL);
#else /*MTK_BUILD_SMT_LOAD*/
        bt_status_t result = bt_power_on(NULL, NULL);
#endif /*MTK_BUILD_SMT_LOAD*/
        LOG_MSGID_I(atci_bt, "power on req result: 0x%x",1, result);
        if (BT_STATUS_SUCCESS == result ) {
        /*waiting power on cnf event*/
            return;
        } else  if (BT_STATUS_FAIL == result) {
            atci_bt_cntx_p->is_host_power_on = true;
            atci_bt_ata_response_result(true, NULL);
        } else {
            atci_bt_ata_response_result(false, NULL);
        }
    } else {
       atci_bt_ata_response_result(true, NULL);
    }
}

static void atci_bt_ata_power_on_cnf(bt_status_t status, void*buf)
{
    LOG_MSGID_I(atci_bt, "power on cnf result: 0x%x",1, status);
    if (status == BT_STATUS_SUCCESS) {
        atci_bt_cntx_p->is_host_power_on = true;
        atci_bt_ata_response_result(true, NULL);
    } else {
        atci_bt_ata_response_result(false, NULL);
    }
}

static void atci_bt_ata_power_off_req(void)
{
    LOG_MSGID_I(atci_bt, "power off req, host power on: %d",1, atci_bt_cntx_p->is_host_power_on);
    if (!atci_bt_cntx_p->is_host_power_on) {
        atci_bt_ata_response_result(true, NULL);
    } else {
        bt_status_t result = bt_power_off();
          LOG_MSGID_I(atci_bt, "power off req result: 0x%x",1, result);
        if (BT_STATUS_SUCCESS == result) {
             /*waiting power off cnf event*/
             return;
         } else if (BT_STATUS_FAIL == result) {
            atci_bt_cntx_p->is_host_power_on = false;
            atci_bt_ata_response_result(true, NULL);
         } else {
             atci_bt_ata_response_result(false, NULL);
             memset((void *)atci_bt_cntx_p, 0x0, sizeof(atci_bt_context_struct_t));
         }
    }
}

static void atci_bt_ata_power_off_cnf(bt_status_t status, void*buf)
{
    LOG_MSGID_I(atci_bt, "power off cnf result: 0x%x",1, status);
    atci_bt_cntx_p->is_host_power_on = false;
    memset((void *)atci_bt_cntx_p, 0x0, sizeof(atci_bt_context_struct_t));
    if (status == BT_STATUS_SUCCESS) {
        atci_bt_ata_response_result(true, NULL);
    } else {
        atci_bt_ata_response_result(false, NULL);
    }
}

static void atci_bt_ata_inquiry_ind(bt_status_t status, void*buf)
{
    bt_gap_inquiry_ind_t* device = (bt_gap_inquiry_ind_t*) buf;
    LOG_MSGID_I(atci_bt, "discovery result ind: 0x%x",1, status);
    if (status) {
        atci_bt_cntx_p->searched_num++;
        bt_gap_read_remote_name(device->address);
    } else {
        atci_bt_ata_response_result(false, NULL);
    }
}

static void atci_bt_ata_inquiry_complete_ind(bt_status_t status, void*buf)
{
    LOG_MSGID_I(atci_bt, "discovery complete result: 0x%x, searched num: %d",2, status, atci_bt_cntx_p->searched_num);

    atci_bt_cntx_p->is_inquiry_completed = true;
    if (atci_bt_cntx_p->searched_num == 0) {
        atci_bt_ata_response_result(true, NULL);
    }
}

static void atci_bt_ata_read_remote_name_complete_ind(bt_status_t status, void*buf)
{
    uint8_t temp_str[50] = {0};
    bt_bd_addr_t remote_address ={0};
    bt_gap_read_remote_name_complete_ind_t *remote_name_ind = (bt_gap_read_remote_name_complete_ind_t *)buf;
    if ((status != BT_STATUS_SUCCESS) ||(buf == NULL) ) {
        atci_bt_ata_response_result(false, NULL);
        return;
    }
    memcpy((void *)&remote_address, (void *) (remote_name_ind->address), 6);
    atci_bt_cntx_p->searched_num-- ;
    LOG_MSGID_I(atci_bt, "read remote name complete ind: searched num: %d",1, atci_bt_cntx_p->searched_num);
    /*send URC to atci*/
    atci_response_t output = {{0}};
    snprintf((char *)temp_str, sizeof(temp_str), "%s, 0x%.2X%.2X%.2X%.2X%.2X%.2X", remote_name_ind->name,
        remote_address[5], remote_address[4], remote_address[3],
        remote_address[2], remote_address[1], remote_address[0]);
    snprintf ((char *)output.response_buf, sizeof(output.response_buf), "+EBTAT:%s\r\n", (char *)temp_str);
    LOG_I(atci_bt, "response string:%s", output.response_buf);
    output.response_len = strlen((char *)output.response_buf);
    output.response_flag = 0 | ATCI_RESPONSE_FLAG_URC_FORMAT;
    atci_send_response(&output);

    /*send final response to atci*/
    if (atci_bt_cntx_p->is_inquiry_completed && (atci_bt_cntx_p->searched_num == 0)) {
        output.response_len = 0;
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&output);
    }
}

extern bool bt_driver_enter_dut_mode(void);
static void atci_bt_ata_dut_handler(void)
{
    bool result;
    result = bt_driver_enter_dut_mode();
    LOG_MSGID_I(atci_bt, "enter dut result: 0x%x",1, result);
    bt_power_on_set_config_type(BT_POWER_ON_DUT);
    atci_bt_ata_response_result(result, NULL);
}

extern void bt_driver_dut_mode_set_dut_address(bt_bd_addr_t addr);
static void atci_bt_ata_dut_config_handler(char *string)
{
    bt_bd_addr_t dut_addr = {0};
    int8_t result = -1;
    for (uint8_t i = 0;i < 12;i+=2) {
        result = atci_bt_str2hex((uint8_t *)string+i, 2, &dut_addr[i/2]);
        if (result < 0) {
            LOG_MSGID_I(atci_bt, "atci_bt_ata_dut_config_handler, parse bd address fail", 0);
            atci_bt_ata_response_result(false, NULL);
            return;
        }
    }
    LOG_MSGID_I(atci_bt, "atci_bt_ata_dut_config_handler, addr:%x%x%x%x%x%x\r\n",6,dut_addr[0],dut_addr[1],dut_addr[2],dut_addr[3],dut_addr[4],dut_addr[5]);
    bt_driver_dut_mode_set_dut_address(dut_addr);
    atci_bt_ata_response_result(true, NULL);
}

static void atci_bt_ata_tx_config_handler(char* string)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint8_t bt_init_tx, bt_max_tx, le_init_tx, tx_offset, fixed_tx_enable, fixed_tx;
    bt_init_tx = (uint8_t)strtoul(string, NULL, 10);
    le_init_tx = (uint8_t)strtoul(string + 4, NULL, 10);
    bt_max_tx = (uint8_t)strtoul(string + 2, NULL, 10);
    tx_offset = (uint8_t)strtoul(string + 6, NULL, 10);
    fixed_tx_enable = (uint8_t)strtoul(string + 8, NULL, 10);
    fixed_tx = (uint8_t)strtoul(string + 10, NULL, 10);
    
    if ((bt_init_tx | bt_max_tx | le_init_tx | fixed_tx) <= 7) {
        bt_config_tx_power_t tx_cfg = {.bt_init_tx_power_level = bt_init_tx, 
                                       .bt_max_tx_power_level = bt_max_tx, 
                                       .le_init_tx_power_level = le_init_tx,
                                       .tx_power_level_offset = tx_offset,
                                       .fixed_tx_power_enable = fixed_tx_enable,
                                       .fixed_tx_power_level = fixed_tx};
        status = bt_config_tx_power_level(&tx_cfg);
    }
    
    LOG_MSGID_I(atci_bt, "atci_bt_ata_tx_config_handler(%d, %d, %d, %d, %d, %d) status 0x%x",7, 
        bt_init_tx, bt_max_tx, le_init_tx, tx_offset, fixed_tx_enable, fixed_tx, status);
    if (BT_STATUS_SUCCESS == status) {
        atci_bt_ata_response_result(true, NULL);
    } else {
        atci_bt_ata_response_result(false, NULL);
    }
}

static void atci_bt_ata_set_tx_power_gc_handler(char *string)
{
    atci_bt_tx_power_gc_t tx_power_gc = {0XFF,0XFF,0XFF,0XFF};
    nvkey_status_t status = 0;
    uint8_t value[2] = {0};
    if (atci_bt_str2hex((uint8_t *)string, 4, value)) {
        tx_power_gc.br_gc = (value[0] << 8) | value[1];
    }
    if (atci_bt_str2hex((uint8_t *)string+4, 4, value)) {
        tx_power_gc.edr_gc = (value[0] << 8) | value[1];
    }
    if (atci_bt_str2hex((uint8_t *)string+8, 4, value)) {
        tx_power_gc.ble_1m_gc = (value[0] << 8) | value[1];
    }
    if (atci_bt_str2hex((uint8_t *)string+12, 4, value)) {
        tx_power_gc.ble_2m_gc = (value[0] << 8) | value[1];
    }
    if (tx_power_gc.br_gc <= 0x3F && tx_power_gc.edr_gc <= 0x3F && tx_power_gc.ble_1m_gc <= 0x3F && tx_power_gc.ble_2m_gc <= 0x3F) {
        status = nvkey_write_data(NVKEYID_BT_TX_POWER_GC, (uint8_t *)&tx_power_gc, sizeof(atci_bt_tx_power_gc_t));
        LOG_MSGID_I(atci_bt, "set tx power gc(br_gc:%x, edr_gc:%x, ble_1m_gc:%x, ble_2m_gc:%x, status:%d)",5,
        tx_power_gc.br_gc, tx_power_gc.edr_gc, tx_power_gc.ble_1m_gc, tx_power_gc.ble_2m_gc, status);

        if (status == NVKEY_STATUS_OK) {
            atci_bt_ata_response_result(true, NULL);
        } else {
            atci_bt_ata_response_result(false, NULL);
        }
    } else {
        atci_bt_ata_response_result(false, NULL);
    }
}

static void atci_bt_ata_get_tx_power_gc_handler()
{
    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (response == NULL) {
        return;
    }
    nvkey_status_t nvkey_status = NVKEY_STATUS_OK;
    atci_bt_tx_power_gc_t tx_power_gc = {0};
    uint32_t tx_power_gc_size = sizeof(atci_bt_tx_power_gc_t);

    nvkey_status = nvkey_read_data(NVKEYID_BT_TX_POWER_GC, (uint8_t *)&tx_power_gc, &tx_power_gc_size);
    LOG_MSGID_I(atci_bt, "gt tx power gc(br_gc:%x, edr_gc:%x, ble_1m_gc:%x, ble_2m_gc:%x, status:%d)",5,
        tx_power_gc.br_gc, tx_power_gc.edr_gc, tx_power_gc.ble_1m_gc, tx_power_gc.ble_2m_gc, nvkey_status);
    if (nvkey_status == NVKEY_STATUS_OK) {
        snprintf((char *)response->response_buf, sizeof(response->response_buf), "+Get BR_GC:0x%04x, EDR_GC:0x%04x, BLE_1M_GC:0x%04x, BLE_2M_GC:0x%04x\r\n",
                tx_power_gc.br_gc,tx_power_gc.edr_gc,tx_power_gc.ble_1m_gc,tx_power_gc.ble_2m_gc);
        response->response_len = strlen((char *)response->response_buf);
        response->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK; 
        atci_send_response(response);
    } else {
        response->response_len = 0;
        response->response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR; 
        atci_send_response(response);
    }

    vPortFree(response);
}

static void atci_bt_ata_search_handler(void)
{
    bt_status_t result = bt_gap_inquiry(ATCI_BT_ATA_INQUIRY_TIME, ATCI_BT_ATA_INQUIRY_NUM);

    LOG_MSGID_I(atci_bt, "inquiry result: 0x%x",1, result);
    if ((BT_STATUS_SUCCESS == result) ||(BT_STATUS_FAIL == result)) {
        /*waiting inquiry cnf event*/
    } else {
        atci_bt_ata_response_result(false, NULL);
    }
}

#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))

static bool test_mode_flag = false;
bool atci_bt_ata_is_on_test_mode(void)
{
    return test_mode_flag;
}
#endif
static void atci_bt_ata_cmd_paser(uint8_t *string)
{
    LOG_I(atci_bt, "enter parser, command: %s", string);

#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))

    if(strncmp((char *)string, "enter_test_mode", 15) == 0){
        test_mode_flag = true;
        bt_device_manager_set_device_mode(BT_DEVICE_MANAGER_TEST_MODE);
        atci_bt_ata_response_result(true, NULL);
        return;
    } else if (strncmp((char *)string, "exit_test_mode", 14) == 0) {
        test_mode_flag = false;
        bt_device_manager_set_device_mode(BT_DEVICE_MANAGER_NORMAL_MODE);
        atci_bt_ata_response_result(true, NULL);
        return;
    }
#endif
    atci_bt_cntx_p->ata = true;
    if (strncmp((char *)string, "bt_power_on", 11) == 0) {
        /* special patch for MP Tool capability. ATEBT=bt_power_on means exit test mode */
        /* should delete after MP Tool add "enter_test_mode" cmd */
        test_mode_flag = false;
        bt_device_manager_set_device_mode(BT_DEVICE_MANAGER_NORMAL_MODE);
        atci_bt_ata_power_on_req();
    } else if (strncmp((char *)string, "bt_power_off", 12) == 0) {
        /* special patch for MP Tool capability. ATEBT=bt_power_off means enter test mode */
        /* should delete after MP Tool add "enter_test_mode" cmd */
        test_mode_flag = true;
        bt_device_manager_set_device_mode(BT_DEVICE_MANAGER_TEST_MODE);
        atci_bt_cntx_p->is_host_power_on = true;
        atci_bt_ata_power_off_req();
    } else if (strncmp((char *)string, "bt_search", 9) == 0) {
        atci_bt_ata_search_handler();
    } else if (strncmp((char *)string, "dut", 3) == 0) {
        atci_bt_ata_dut_handler();
    } else if (strncmp((char *)string, "set_dut_addr,", 13) == 0) {
        atci_bt_ata_dut_config_handler((char *)string + 13);
    } else if (strncmp((char *)string, "tx_config ", 10) == 0) {
        atci_bt_ata_tx_config_handler((char *)string + 10);
    } else if (strncmp((char *)string, "tx_power_gc,", 12) == 0) {
        atci_bt_ata_set_tx_power_gc_handler((char *)string + 12);
    } else if (strncmp((char *)string, "g_tx_power_gc", 13) == 0) {
        atci_bt_ata_get_tx_power_gc_handler();
    } else {
        atci_bt_ata_response_result(false, NULL);
    }
}

atci_status_t atci_cmd_hdlr_bt_ata(atci_parse_cmd_param_t *parse_cmd)
{

    LOG_I(atci_bt, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /*AT+EBTAT=<op>*/
            atci_bt_ata_cmd_paser((uint8_t *)parse_cmd->string_ptr + 9);
            break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            char * str = "Not Support\n";
            atci_bt_ata_response_result(false, str);
        }
        break;
    }

    return ATCI_STATUS_OK;
}

#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
typedef bt_status_t (*bt_gap_ata_app_event_callback_p)(bt_msg_type_t msg, bt_status_t status, void *buff);
bt_gap_ata_app_event_callback_p app_callback = NULL;

bt_status_t bt_gap_ata_callback_register(void *callback)
{
    bt_status_t ret = BT_STATUS_FAIL;
    if(callback) {
        app_callback = callback;
        ret = BT_STATUS_SUCCESS;
    }
    return ret;
}

void bt_gap_ata_callback_deregister()
{
    app_callback = NULL;
}

#endif

bt_status_t bt_gap_ata_callback(bt_msg_type_t msg, bt_status_t status, void*buf)
{

    LOG_MSGID_I(atci_bt, "receive msg: 0x%x, status:0x%x, ata:%d",3, msg, status, atci_bt_cntx_p->ata);
#if ((PRODUCT_VERSION == 1552) || defined(AM255X) || (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565))
    if(app_callback) {
        app_callback(msg, status, buf);
        return BT_STATUS_SUCCESS;
    }
#endif
    if (BT_POWER_OFF_CNF == msg && true == at_command_enter_test_mode) {
        atci_response_t *output = NULL;
        if ((output = pvPortMalloc(sizeof(atci_response_t))) != NULL) {
            memset(output, 0, sizeof(atci_response_t));
            output->response_flag = (0 | ATCI_RESPONSE_FLAG_APPEND_OK);
            output->response_len = 0;
            atci_send_response(output);
            vPortFree(output);
        } else {
            LOG_MSGID_E(atci_bt, "memory malloced failed", 0); 
        }
        at_command_enter_test_mode = false;
    }

    if (BT_GAP_ENTER_TEST_MODE_CNF == msg) {
        LOG_MSGID_I(atci_bt, "enter dut mode !!!", 0);
        dut_mode_enable = true;
    }

    if (!atci_bt_cntx_p->ata) {
        return BT_STATUS_UNSUPPORTED;
    }

    bt_status_t ret = BT_STATUS_SUCCESS;
    switch (msg) {
        case BT_POWER_ON_CNF:
        {
            atci_bt_ata_power_on_cnf(status, buf);
        }
        break;

        case BT_POWER_OFF_CNF:
        {
            LOG_MSGID_I(atci_bt, "Dut mode flag %d!!!", 1, dut_mode_enable); 
            atci_bt_ata_power_off_cnf(status, buf);
            if (true == dut_mode_enable) {
                ret = BT_STATUS_UNSUPPORTED;
            }
            dut_mode_enable = false;
        }
        break;

        case BT_GAP_ENTER_TEST_MODE_CNF:
        {
            ret = BT_STATUS_UNSUPPORTED;
        }
        break;

        case BT_GAP_INQUIRY_CNF:
        {
            if (status != BT_STATUS_SUCCESS) {
                atci_bt_ata_response_result(false, NULL);
            }
        }
        break;

        case BT_GAP_CANCEL_INQUIRY_CNF:
        {
            if (status != BT_STATUS_SUCCESS) {
                atci_bt_ata_response_result(false, NULL);
            }
        }
        break;

        case BT_GAP_INQUIRY_IND:
        {
            atci_bt_ata_inquiry_ind(status, buf);
        }
        break;

        case BT_GAP_INQUIRY_COMPLETE_IND:
        {
            atci_bt_ata_inquiry_complete_ind(status, buf);
        }
        break;

        case BT_GAP_READ_REMOTE_NAME_CNF:
        {
            if (status != BT_STATUS_SUCCESS) {
                atci_bt_ata_response_result(false, NULL);
            }
        }
        break;

        case BT_GAP_READ_REMOTE_NAME_COMPLETE_IND:
        {
            atci_bt_ata_read_remote_name_complete_ind(status, buf);
        }
        break;
        default:
            break;
    }
    return ret;
}

/*---ATA Command handler Function End---*/

#endif  /*MTK_BT_AT_COMMAND_ENABLE*/

#ifdef __BT_DEBUG__
#include "at_command.h"
#include "at_command_bt.h"
#include "syslog.h"

log_create_module(atci_bt_driver_dump, PRINT_LEVEL_INFO);

extern void bt_driver_dump_log_switch(bool flag);

#if _MSC_VER >= 1500
    #pragma comment(linker, "/alternatename:_bt_driver_dump_log_switch=_bt_driver_dump_log_switch_default")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION)
    #pragma weak bt_driver_dump_log_switch = bt_driver_dump_log_switch_default
#else
    #error "Unsupported Platform"
#endif

void bt_driver_dump_log_switch_default(bool flag)
{
    LOG_MSGID_I(atci_bt_driver_dump, "Not support with no Bluetooth module", 0);
}

atci_status_t atci_cmd_hdlr_bt_enable_driver_dump_log(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};

    LOG_I(atci_bt_driver_dump, "cmd:%s, mode:%d", parse_cmd->string_ptr, parse_cmd->mode);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: { /*AT+EBTEDL=<op>*/
            uint8_t is_enable = *((uint8_t *)parse_cmd->string_ptr + 10) - '0';
            LOG_MSGID_I(atci_bt_driver_dump, "enable:%d",1, is_enable);

            bt_driver_dump_log_switch((bool)is_enable);
            output.response_len = 0;
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        }
        break;

        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
        case ATCI_CMD_MODE_ACTIVE:
        default: {
            strncpy((char *)output.response_buf, "Not Support\n", strlen("Not Support\n")+1);
            output.response_len = strlen((char *)output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }

    atci_send_response(&output);
    return ATCI_STATUS_OK;
}
#endif //__BT_DEBUG__

