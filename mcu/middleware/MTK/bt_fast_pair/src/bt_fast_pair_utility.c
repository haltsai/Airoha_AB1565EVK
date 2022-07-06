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

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "syslog.h"
#include "atci.h"
#include "bt_fast_pair.h"
#include "bt_fast_pair_utility.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "task.h"
#include "task_def.h"

log_create_module(BT_FAST_PAIR, PRINT_LEVEL_INFO);
LOG_CONTROL_BLOCK_DECLARE(BT_FAST_PAIR);

static atci_status_t bt_fast_pair_at_cmd_set_battery_value(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_fast_pair_at_cmd_set_silence_mode(atci_parse_cmd_param_t *parse_cmd);

uint32_t spot_counter_save_cycle = SPOT_COUNTER_SAVE_CYCLE_DEF;

void        bt_fast_pair_log_msgid_i(const char *msg, uint32_t arg_cnt, ...)
{
#ifdef MTK_DEBUG_LEVEL_INFO
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(&log_control_block_BT_FAST_PAIR, PRINT_LEVEL_INFO, msg, arg_cnt, ap);
    va_end(ap);
#endif
}

void        bt_fast_pair_log_msgid_w(const char *msg, uint32_t arg_cnt, ...)
{
#ifdef MTK_DEBUG_LEVEL_WARNING
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(&log_control_block_BT_FAST_PAIR, PRINT_LEVEL_WARNING, msg, arg_cnt, ap);
    va_end(ap);
#endif
}

void        bt_fast_pair_log_msgid_e(const char *msg, uint32_t arg_cnt, ...)
{
#ifdef MTK_DEBUG_LEVEL_ERROR
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(&log_control_block_BT_FAST_PAIR, PRINT_LEVEL_ERROR, msg, arg_cnt, ap);
    va_end(ap);
#endif
}

void        bt_fast_pair_log_msgid_d(const char *msg, uint32_t arg_cnt, ...)
{
#ifdef MTK_DEBUG_LEVEL_DEBUG
    va_list ap;
    va_start(ap, arg_cnt);
    log_print_msgid(&log_control_block_BT_FAST_PAIR, PRINT_LEVEL_DEBUG, msg, arg_cnt, ap);
    va_end(ap);
#endif
}

static atci_cmd_hdlr_item_t bt_fast_pair_at_cmd[] = {
    {
        .command_head = "AT+FPSBATTERYSET",
        .command_hdlr = bt_fast_pair_at_cmd_set_battery_value,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+FPSSILENCE",
        .command_hdlr = bt_fast_pair_at_cmd_set_silence_mode,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

#if 0
static atci_status_t 
            bt_app_comm_at_cmd_set_fast_pair_tx_power_level(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            int8_t tx_power_level = atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
            LOG_MSGID_I(BT_APP, "set fast pair tx power level :%d", 1, tx_power_level);
        
            app_fast_pair_set_tx_power_level(tx_power_level);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}
#endif

static atci_status_t
            bt_fast_pair_at_cmd_set_battery_value(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            uint8_t battery_raw[sizeof(bt_fast_pair_battery_t) + (2 * sizeof(bt_fast_pair_battery_item_t))];
            bt_fast_pair_battery_t *battery = (bt_fast_pair_battery_t *)battery_raw;
            battery->component_num = 3;
            battery->ui_show = true;
            battery->remaining_time = 0xFFFF;
            battery->battery[0].charging = 0x01;
            battery->battery[0].battery_value= 67;
            battery->battery[1].charging = 0x00;
            battery->battery[1].battery_value= 70;
            battery->battery[2].charging = 0x00;
            battery->battery[2].battery_value= 50;
            
            bt_fast_pair_update_battery(battery);
            LOG_MSGID_I(BT_APP, "bt_fast_pair_at_cmd_set_battery_value", 0);
        
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            break;        
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t
            bt_fast_pair_at_cmd_set_silence_mode(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            bt_fast_pair_set_silence_mode(NULL, true);
            LOG_MSGID_I(BT_APP, "silence mode", 0);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            break;        
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

void bt_fast_pair_at_cmd_init(void)
{
    atci_register_handler(bt_fast_pair_at_cmd, sizeof(bt_fast_pair_at_cmd) / sizeof(atci_cmd_hdlr_item_t));
}

bt_fast_pair_nvdm_sta_t bt_fast_pair_nvdm_read(const char *group_name, const char *item_name, uint8_t *buf, uint32_t size)
{
    nvdm_status_t status;
    status = nvdm_read_data_item(group_name, item_name, buf, &size);
    if (status == NVDM_STATUS_OK) {
        return BT_FAST_PAIR_NVDM_STA_SUCCESS;
    } else if(status == NVDM_STATUS_ITEM_NOT_FOUND) {
        return BT_FAST_PAIR_NVDM_STA_NOT_FOUND;
    }

    return BT_FAST_PAIR_NVDM_STA_FAIL;
}

bt_fast_pair_nvkey_sta_t bt_fast_pair_nvkey_read(uint16_t id, uint8_t *buf, uint32_t* size)
{
    nvkey_status_t sta = NVKEY_STATUS_OK;
    sta = nvkey_read_data(id, buf, size);
    if (sta == NVKEY_STATUS_OK) {
        return BT_FAST_PAIR_NVKEY_STA_SUCCESS;
    } else if(sta == NVKEY_STATUS_ITEM_NOT_FOUND) {
        return BT_FAST_PAIR_NVKEY_STA_NOT_FOUND;
    }

    return BT_FAST_PAIR_NVKEY_STA_FAIL;
}

bt_fast_pair_nvkey_sta_t bt_fast_pair_nvkey_write(uint16_t id, uint8_t *data, uint32_t size)
{
    nvkey_status_t sta = NVKEY_STATUS_OK;
    sta = nvkey_write_data(id, data, size);
    if (sta == NVKEY_STATUS_OK) {
        return BT_FAST_PAIR_NVKEY_STA_SUCCESS;
    } else if(sta == NVKEY_STATUS_ITEM_NOT_FOUND) {
        return BT_FAST_PAIR_NVKEY_STA_NOT_FOUND;
    }

    return BT_FAST_PAIR_NVKEY_STA_FAIL;
}

void *bt_fast_pair_spot_sys_timer_create(char* timer_name, bool repeat, uint32_t period_ms, void* usr_data, spot_timer_callback cb)
{
    if (timer_name == NULL || strlen(timer_name) == 0) {
        return NULL;
    }

    void *timer_handle = xTimerCreate(timer_name, (TickType_t)(period_ms / portTICK_PERIOD_MS), repeat, usr_data, (void *)cb);

    return timer_handle;
}

bool bt_fast_pair_spot_sys_timer_start(void *timer_handle)
{
    if(xTimerStart(timer_handle, 0) != pdPASS)
    {
        return false;
    }
    return true;
}

bool bt_fast_pair_spot_sys_timer_stop(void *timer_handle)
{
    if(xTimerStop(timer_handle, 0) != pdPASS)
    {
        return false;
    }
    return true;
}

bool bt_fast_pair_spot_sys_timer_delete(void *timer_handle)
{
    if(xTimerDelete(timer_handle, 0) != pdPASS)
    {
        return false;
    }
    return true;
}

uint32_t bt_fast_pair_spot_sys_run_time_second()
{
    uint32_t ret = 0;
    ret = xTaskGetTickCount() / portTICK_PERIOD_MS / 1000;
    return ret;
}

void bt_fast_pair_hex_dump(const char *msg, unsigned char *buf, unsigned int buf_len)
{
    LOG_HEXDUMP_I(BT_FAST_PAIR, msg, buf, buf_len);
}

void bt_fast_pair_task_create(bt_fast_pair_task_t *task)
{
    unsigned long create_result = 0;
    void *task_handle = NULL;

    if (task == NULL) {
        return;
    }

    if (task->function == NULL) {
        return;
    }

    create_result = xTaskCreate(task->function,
                                task->name,
                                task->stack_depth,
                                task->parameter,
                                (task->is_high_priority ? TASK_PRIORITY_HIGH : TASK_PRIORITY_BELOW_NORMAL), // TASK_PRIORITY_NORMAL
                                (TaskHandle_t *)&task_handle);

    if (create_result != pdPASS) {
        return;
    }
}

void bt_fast_pair_task_destroy()
{
    vTaskDelete(NULL);
}

uint32_t bt_fast_pair_task_get_running_ms()
{
    return xTaskGetTickCount() / portTICK_PERIOD_MS;
}
