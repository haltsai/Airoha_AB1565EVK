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

#include "syslog_port_device.h"

#ifdef MTK_ONLINE_LOG_TO_APK_ENABLE

#include <string.h>

#include "hal_resource_assignment.h"
#ifndef PORT_SYSLOG_SINGLE_CPU_ENV
#include "hal_hw_semaphore.h"
#endif
#include "race_xport.h"
#include "hal_hw_semaphore.h"
#include "hal_nvic.h"


typedef struct {
    uint32_t share_buffer_addr;     /* Start address of share VFIFO buffer, view with CPU 0, other CPU should do remap */
    uint32_t share_buffer_size;     /* Size of share VFIFO buffer */
    uint32_t share_buffer_wptr;     /* Current write position in share VFIFO buffer, view with relative address */
    uint32_t share_buffer_rptr;     /* write position  */
    uint32_t log_on_flag;
    uint32_t log_finished;
} online_log_share_variable_t;

#ifdef PORT_SYSLOG_SINGLE_CPU_ENV
static online_log_share_variable_t online_log_share_variable;
static volatile online_log_share_variable_t *g_online_log_share_variable = &online_log_share_variable;
//volatile uint32_t *online_log_flag;
#else
static volatile online_log_share_variable_t *g_online_log_share_variable = (volatile online_log_share_variable_t *)HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_ONLINE_START;
volatile uint32_t *online_log_flag;
volatile uint32_t *g_syslog_send_packet_ok;    //only can be change by MCU
#endif

//#define ONLINE_LOG_HEADER    0x4
//refere to PORT_SYSLOG_MAX_ONE_LOG_SIZE + ONLINE_LOG_HEADER
//#define SYSLOG_MAX_ONE_LOG_SIZE (256 +64)
//#define MAX_ONLINE_LOG_ITEM     (2)
//#define MAX_LOG_BUFFER          (SYSLOG_MAX_ONE_LOG_SIZE * MAX_ONLINE_LOG_ITEM)
//ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static uint8_t g_syslog_buffer[MAX_LOG_BUFFER];
//#define ONLINE_LOG_MTU_MAX_SIZE   (740)

/* Memory remap across CPUs (primary CPU remap to other CPU) */
#ifdef PORT_SYSLOG_SINGLE_CPU_ENV
#define memory_remap_primary_cpu_to_current_cpu(address) address
#else
#if defined(MTK_CPU_NUMBER_1)
#define memory_remap_primary_cpu_to_current_cpu(address) hal_memview_cm4_to_dsp0(address)
#elif defined(MTK_CPU_NUMBER_2)
#define memory_remap_primary_cpu_to_current_cpu(address) hal_memview_cm4_to_dsp1(address)
#else
#define memory_remap_primary_cpu_to_current_cpu(address) address
#endif
#endif

#ifndef PORT_SYSLOG_SINGLE_CPU_ENV
#define DUMP_SEMAPHORE_TAKE() while(hal_hw_semaphore_take(HW_SEMAPHORE_SYSLOG_WRAP_LAYER) != HAL_HW_SEMAPHORE_STATUS_OK)
#define DUMP_SEMAPHORE_GIVE() while(hal_hw_semaphore_give(HW_SEMAPHORE_SYSLOG_WRAP_LAYER) != HAL_HW_SEMAPHORE_STATUS_OK)
#else
#define DUMP_SEMAPHORE_TAKE()
#define DUMP_SEMAPHORE_GIVE()
#endif

//typedef struct{
//    uint16_t data_length;
//    uint16_t left_log_size;
//    uint32_t rev;
//    uint8_t *log_pdata;
//}online_log_data_t;

//permit to send only when the last packet had finished
//static online_log_data_t p_online_log_msg;

#ifdef MTK_CPU_NUMBER_0

typedef int32_t (*log_command_handler_t)(uint8_t sub_type, bool need_reset, uint32_t payload_len, uint8_t *respond_buffer);
extern log_command_handler_t g_log_command_handlers[];

#ifdef MTK_CPU_NUMBER_0
static void *port_syslog_malloc(uint32_t size)
{
    return malloc(size);
}

static void port_syslog_free(void *pdata)
{
    free(pdata);
}
#endif

uint32_t need_send_msg(void)
{
    uint32_t status = 0;
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (g_online_log_share_variable->share_buffer_wptr != g_online_log_share_variable->share_buffer_rptr) {
        status = 1;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return status;
}


void port_syslog_online_bt_early_init(uint8_t port_index, hal_uart_baudrate_t baudrate)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(baudrate);
}

void port_syslog_online_bt_early_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);
    g_online_log_share_variable->share_buffer_addr = 0;
    g_online_log_share_variable->share_buffer_wptr = 0;
    g_online_log_share_variable->share_buffer_size = 0;
    g_online_log_share_variable->share_buffer_rptr = 0;
    g_online_log_share_variable->log_on_flag = 0;
    g_online_log_share_variable->log_finished = 0;
    online_log_flag = &g_online_log_share_variable->log_on_flag;
    g_syslog_send_packet_ok = &g_online_log_share_variable->log_finished;
}

void port_syslog_online_bt_exception_init(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
}

void port_syslog_online_bt_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);
}

void port_syslog_online_bt_post_init(uint8_t port_index, uint32_t share_buffer_addr, uint32_t share_buffer_size, pc_rx_handler_t rx_handler)
{
    //exception_config_type exception_config;

    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(share_buffer_addr);
    PORT_SYSLOG_UNUSED(share_buffer_size);
    PORT_SYSLOG_UNUSED(rx_handler);

    g_online_log_share_variable->share_buffer_addr = share_buffer_addr;
    g_online_log_share_variable->share_buffer_size = share_buffer_size;
    g_online_log_share_variable->share_buffer_wptr = 0;
    g_online_log_share_variable->share_buffer_rptr = 0;
    g_online_log_share_variable->log_on_flag = 0;
    g_online_log_share_variable->log_finished = 0;
}

static uint8_t pc_rx_data[8];
static volatile uint8_t g_pc_rx_data_len;
static volatile uint8_t g_pc_rx_data_consume_len;

uint32_t port_syslog_online_bt_receive_data(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    if ((g_pc_rx_data_consume_len + size) <= g_pc_rx_data_len) {
        memcpy(buffer, &pc_rx_data[g_pc_rx_data_consume_len], size);
        g_pc_rx_data_consume_len += size;
    } else {
        assert(0);
    }
    return size;
}

void online_log_rx_handler_post(uint8_t *pbuf)
{
    port_syslog_free(pbuf);
}

//this api is only handle type = 0
//more information please refer to designdocument PC-> target
//code refer to pc_tool_command_handler
#define MAX_URGENT_LOG_LENGTH   256
uint8_t *online_log_rx_handler(uint8_t port_index, uint8_t *data, uint32_t length, uint32_t *user_len)   //length is pc sent data len
{
    uint8_t *pc_command_response_buf;
    int32_t pc_command_response_len;
    uint8_t sub_type;
    int32_t cmd_len;
    bool need_reset;

    if (length < 7) {
        return NULL;
    }

    cmd_len = length;
    sub_type = data[4];
    if (data[5] == 0xFF) {  //CMD_INDEX_RESET_FLAG_CLEAR
        need_reset = false;
    } else if (data[5] == 0x0) {  //CMD_INDEX_RESET_FLAG_SET
        need_reset = true;
    } else {
        return NULL;
    }

    cmd_len -= 6;
    if (cmd_len > 0) {
        memcpy(&pc_rx_data[0], &data[6], cmd_len);
        g_pc_rx_data_len = cmd_len;
        //g_pc_rx_data_left_len = g_pc_rx_data_len;
        g_pc_rx_data_consume_len = 0;
    }

    pc_command_response_buf = port_syslog_malloc(MAX_URGENT_LOG_LENGTH);
    pc_command_response_len = g_log_command_handlers[sub_type](sub_type, need_reset, cmd_len, pc_command_response_buf);

    if (pc_command_response_len < 0) {
        port_syslog_free(pc_command_response_buf);
        return NULL;
    }

    /* If command is invalid or abnormal status detect, respond fail package. */
    if (pc_command_response_len == 0) {
        *user_len = 0;
        return NULL;
    }

    *user_len = pc_command_response_len;
    return pc_command_response_buf;
}

void online_log_race_sent_data(uint32_t *curr_log_addr, uint32_t *curr_log_size)
{
    uint32_t mask;
    uint32_t log_length;

    hal_nvic_save_and_set_interrupt_mask(&mask);
#ifndef PORT_SYSLOG_SINGLE_CPU_ENV
    while (hal_hw_semaphore_take(HW_SEMAPHORE_SYSLOG) != HAL_HW_SEMAPHORE_STATUS_OK);
#endif

    if (g_online_log_share_variable->share_buffer_wptr >= g_online_log_share_variable->share_buffer_rptr) {
        log_length = g_online_log_share_variable->share_buffer_wptr - g_online_log_share_variable->share_buffer_rptr;
    } else {
        //log_length = g_online_log_share_variable->share_buffer_wptr +
        log_length = g_online_log_share_variable->share_buffer_size - g_online_log_share_variable->share_buffer_rptr;
    }
    *curr_log_addr = g_online_log_share_variable->share_buffer_addr + g_online_log_share_variable->share_buffer_rptr;
    *curr_log_size = log_length;

#ifndef PORT_SYSLOG_SINGLE_CPU_ENV
    while (hal_hw_semaphore_give(HW_SEMAPHORE_SYSLOG) != HAL_HW_SEMAPHORE_STATUS_OK);
#endif
    hal_nvic_restore_interrupt_mask(mask);
}

void online_log_race_update_readp(uint32_t read_length)
{
    //only CM4 updates read point
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
#ifndef PORT_SYSLOG_SINGLE_CPU_ENV
    while (hal_hw_semaphore_take(HW_SEMAPHORE_SYSLOG) != HAL_HW_SEMAPHORE_STATUS_OK);
#endif
    if ((g_online_log_share_variable->share_buffer_rptr + read_length) >= g_online_log_share_variable->share_buffer_size) {
        g_online_log_share_variable->share_buffer_rptr = read_length -
                                                         (g_online_log_share_variable->share_buffer_size - g_online_log_share_variable->share_buffer_rptr);
    } else {
        g_online_log_share_variable->share_buffer_rptr += read_length;
    }
#ifndef PORT_SYSLOG_SINGLE_CPU_ENV
    while (hal_hw_semaphore_give(HW_SEMAPHORE_SYSLOG) != HAL_HW_SEMAPHORE_STATUS_OK);
#endif
    hal_nvic_restore_interrupt_mask(mask);
}
#endif

void port_syslog_device_get_setting(log_port_type_t *port_type, uint8_t *port_index)
{
    *port_type = LOG_PORT_TYPE_ONLINE_BT;
    *port_index = 0;
    g_online_log_share_variable->log_on_flag = 0;
}


uint32_t port_syslog_online_bt_get_hw_rptr(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
    //if (g_online_log_share_variable->log_on_flag == 0) {
    //    g_online_log_share_variable->share_buffer_rptr = g_online_log_share_variable->share_buffer_wptr;
    //}
    return g_online_log_share_variable->share_buffer_rptr;
}

uint32_t port_syslog_online_bt_get_hw_wptr(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
    return g_online_log_share_variable->share_buffer_wptr;
}

void port_syslog_online_bt_set_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    uint32_t curr_log_size;
    PORT_SYSLOG_UNUSED(port_index);

    if (*online_log_flag == 0) {
        if ((g_online_log_share_variable->share_buffer_wptr + move_bytes) >= g_online_log_share_variable->share_buffer_size) {
            curr_log_size = g_online_log_share_variable->share_buffer_size - g_online_log_share_variable->share_buffer_wptr;
            g_online_log_share_variable->share_buffer_wptr = move_bytes - curr_log_size;
            g_online_log_share_variable->share_buffer_rptr = g_online_log_share_variable->share_buffer_wptr;
        } else {
            g_online_log_share_variable->share_buffer_wptr += move_bytes;
            g_online_log_share_variable->share_buffer_rptr = g_online_log_share_variable->share_buffer_wptr;
        }
    } else {
        if ((g_online_log_share_variable->share_buffer_wptr + move_bytes) >= g_online_log_share_variable->share_buffer_size) {
            curr_log_size = g_online_log_share_variable->share_buffer_size - g_online_log_share_variable->share_buffer_wptr;
            g_online_log_share_variable->share_buffer_wptr = move_bytes - curr_log_size;
            //read point will be move on race task when log had sent finished
            //g_online_log_share_variable->share_buffer_rptr = g_online_log_share_variable->share_buffer_wptr;
        } else {
            g_online_log_share_variable->share_buffer_wptr += move_bytes;
            //read point will be move on race task when log had sent finished
            //g_online_log_share_variable->share_buffer_rptr = g_online_log_share_variable->share_buffer_wptr;
        }
    }
}

#else  //MTK_ONLINE_LOG_TO_APK_ENABLE

#ifdef MTK_CPU_NUMBER_0
void port_syslog_online_bt_early_init(uint8_t port_index, hal_uart_baudrate_t baudrate)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(baudrate);
}

void port_syslog_online_bt_early_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);
}

void port_syslog_online_bt_exception_init(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
}

void port_syslog_online_bt_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);
}

void port_syslog_online_bt_post_init(uint8_t port_index, uint32_t share_buffer_addr, uint32_t share_buffer_size, pc_rx_handler_t rx_handler)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(share_buffer_addr);
    PORT_SYSLOG_UNUSED(share_buffer_size);
    PORT_SYSLOG_UNUSED(rx_handler);
}

uint32_t port_syslog_online_bt_receive_data(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(buffer);
    PORT_SYSLOG_UNUSED(size);

    return 0;
}

#endif  //MTK_CPU_NUMBER_0

uint32_t port_syslog_online_bt_get_hw_rptr(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
    return 0;
}

uint32_t port_syslog_online_bt_get_hw_wptr(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
    return 0;
}

void port_syslog_online_bt_set_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    PORT_SYSLOG_UNUSED(port_index);
    PORT_SYSLOG_UNUSED(move_bytes);
}

#endif  //MTK_ONLINE_LOG_TO_APK_ENABLE

void port_syslog_online_bt_init_hook(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
}

void port_syslog_online_bt_logging_hook(uint8_t port_index)
{
    PORT_SYSLOG_UNUSED(port_index);
}

port_syslog_device_ops_t g_port_syslog_online_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_syslog_online_bt_early_init,
    port_syslog_online_bt_early_send,
    port_syslog_online_bt_post_init,
    port_syslog_online_bt_exception_init,
    port_syslog_online_bt_exception_send,
    port_syslog_online_bt_receive_data,
#endif
    port_syslog_online_bt_init_hook,
    port_syslog_online_bt_logging_hook,
    port_syslog_online_bt_get_hw_rptr,
    port_syslog_online_bt_get_hw_wptr,
    port_syslog_online_bt_set_hw_wptr,
};

