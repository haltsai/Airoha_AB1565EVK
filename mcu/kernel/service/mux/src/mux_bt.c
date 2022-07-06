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
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO ObtAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIBLE FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "hal_platform.h"
#include "hal_nvic.h"

#include "mux.h"
#include "mux_port_device.h"
#include "serial_port.h"
#include "assert.h"
#include "mux_port.h"

#ifdef MTK_MUX_BT_ENABLE

#define BT_GPT_TIMEOUT 1
#define BT_PORT_INDEX_TO_MUX_PORT(port_index) (port_index + MUX_BT_BEGIN)

mux_irq_handler_t g_mux_bt_callback;
virtual_read_write_point_t g_mux_bt_r_w_point[MUX_BT_END - MUX_BT_BEGIN + 1];
serial_port_handle_t serial_port_bt_handle[MUX_BT_END - MUX_BT_BEGIN + 1]; //MUX_SPP MUX_BLE MUX_AIRUPDATE

#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t x_mux_bt_Semaphore;
static uint32_t g_mux_bt_phase2_send_status;
static uint32_t g_bt_gpt_handler;
static bool g_mux_bt_ponit_init[MUX_BT_END - MUX_BT_BEGIN + 1] = {0};
static mux_port_config_t *p_local_setting[MUX_BT_END - MUX_BT_BEGIN + 1] = {0};

typedef struct {
    mux_status_t tx_status;
    uint16_t     tx_complete_size;
    uint8_t      rx_not_enough_count;
} mux_bt_context_t;
static mux_bt_context_t mux_bt_ctx;
log_create_module(MUX_BT, PRINT_LEVEL_INFO);

static void port_mux_bt_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes);
static void port_mux_bt_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes);
void port_mux_bt_phase2_send(uint8_t port_index);

uint16_t bt_send_data(uint8_t port_index, uint8_t *packet, uint32_t length, volatile uint32_t *sending_point)
{
    serial_port_status_t status;
    serial_port_write_data_t send_data;
    uint32_t per_cpu_irq_mask;
    uint16_t tx_complete_size = 0;
    if (length == 0) {
        return 0;
    }

    send_data.ret_size = 0;
    send_data.data = packet;
    send_data.size = length;

    /* Must be  set the value of tx_sending_read_point firstly!!!!!must before call bt send. */
    do {
        status = serial_port_control(serial_port_bt_handle[port_index], SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t *)&send_data);
        if (status == SERIAL_PORT_STATUS_OK) {
            LOG_MSGID_I(MUX_BT, "[MUX][BT] port service send data success, length = %d", 1, send_data.ret_size);
            tx_complete_size += send_data.ret_size;
            if (tx_complete_size < length) {
                send_data.data = (uint8_t *)(packet + tx_complete_size);
                send_data.size = length - tx_complete_size;
            }
        } else {
            LOG_MSGID_W(MUX_BT, "[MUX][BT] port service send data fail status = %02x", 1, status);
        }
    } while ((status == SERIAL_PORT_STATUS_OK) && (tx_complete_size < length));

    /* Manually update tx read pointer, because no SERIAL_PORT_EVENT_READY_TO_WRITE event received. */
    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
    if (!g_mux_bt_ponit_init[port_index]) {
        port_mux_bt_set_tx_hw_rptr_internal_use(port_index, length);
    }
    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);
    return tx_complete_size;

}

static void mux_bt_callback(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
{
    uint8_t port_index = device - SERIAL_PORT_DEV_BT_TYPE_BEGIN;
    virtual_read_write_point_t *p = &g_mux_bt_r_w_point[port_index];
    uint32_t next_free_block_len;
    serial_port_read_data_t read_data;
    uint32_t per_cpu_irq_mask;
    serial_port_status_t status;
    switch (event) {
    case SERIAL_PORT_EVENT_READY_TO_WRITE:
        break;

    case SERIAL_PORT_EVENT_READY_TO_READ:
        read_data.buffer = (uint8_t *)port_mux_malloc(p->rx_buff_len);
        read_data.size = p->rx_buff_len;

        status = serial_port_control(serial_port_bt_handle[port_index], SERIAL_PORT_CMD_READ_DATA, (serial_port_ctrl_para_t *)&read_data);
        if (status != SERIAL_PORT_STATUS_OK) {
            port_mux_free(read_data.buffer);
            LOG_MSGID_E(MUX_BT, "bt rx port control read data error status[%d] handle[%d],rx_buff_len = %d", 3, status, serial_port_bt_handle[port_index], p->rx_buff_len);
            return;
        }

        if (read_data.ret_size > (p->rx_buff_end - p->rx_buff_start - p->rx_buff_available_len)) {
            port_mux_free(read_data.buffer);
            mux_bt_ctx.rx_not_enough_count++;
            if (mux_bt_ctx.rx_not_enough_count >= 2) {
                p->rx_buff_read_point =  p->rx_buff_start;
                p->rx_buff_write_point =  p->rx_buff_start;
                p->rx_buff_available_len = 0;
                p->rx_receiving_write_point = 0xFFFFFFFF;

                LOG_MSGID_E(MUX_BT, "bt rx buffer not enough to save,rx_read_point = %02x,rx_write_point = %02x", 2, \
                            p->rx_buff_read_point, p->rx_buff_write_point);
            }
            LOG_MSGID_E(MUX_BT, "bt rx buffer not enough to save, len:%d,rx_not_enough_count = %d", 2, read_data.ret_size, mux_bt_ctx.rx_not_enough_count);
            return;
        }
        mux_bt_ctx.rx_not_enough_count = 0;
        // Rx buffer have some space to do receive.
        next_free_block_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point, p->rx_buff_write_point, p->rx_buff_end, p->rx_buff_available_len);

        if (next_free_block_len >= read_data.ret_size) {
            memcpy((void *)(p->rx_buff_write_point), read_data.buffer, read_data.ret_size);
        } else {
            memcpy((void *)(p->rx_buff_write_point), read_data.buffer, next_free_block_len);
            memcpy((void *)(p->rx_buff_start), read_data.buffer + next_free_block_len, read_data.ret_size - next_free_block_len);
        }
        port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
        port_mux_bt_set_rx_hw_wptr_internal_use(port_index, read_data.ret_size);
        mux_driver_debug_for_check(&g_mux_bt_r_w_point[port_index]);
        port_mux_local_cpu_exit_critical(per_cpu_irq_mask);
        port_mux_free(read_data.buffer);
        g_mux_bt_callback(BT_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_READ, parameter);

        break;

    case SERIAL_PORT_EVENT_BT_CONNECTION:
        LOG_MSGID_I(MUX_BT, "event: SERIAL_PORT_EVENT_BT_CONNECTION ", 0);
        g_mux_bt_callback(BT_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_CONNECTION, parameter);
        break;

    case SERIAL_PORT_EVENT_BT_DISCONNECTION:
        LOG_MSGID_I(MUX_BT, "event: SERIAL_PORT_EVENT_BT_DISCONNECTION ", 0);
        g_mux_bt_callback(BT_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_DISCONNECTION, parameter);
        break;
    }
}
mux_status_t port_mux_bt_init(mux_port_t port)
{
    mux_status_t status;
    mux_port_setting_t setting;
    mux_protocol_t mux_bt_pro_callback = {
        .tx_protocol_callback = NULL,
        .rx_protocol_callback = NULL,
        .user_data = NULL
    };

    setting.tx_buffer_size = 512;
    setting.rx_buffer_size = 512;
    status = mux_init(port, &setting, &mux_bt_pro_callback);
    //LOG_MSGID_I(MUX_BT, "port_mux_bt_init, status:%d", 1, status);
    return status;
}


mux_status_t port_mux_bt_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    serial_port_status_t serial_port_status;
    serial_port_open_para_t serial_port_bt_config;
    mux_common_device_r_w_point_init(&g_mux_bt_r_w_point[port_index], p_setting);
    p_local_setting[port_index] = p_setting;
    g_mux_bt_ponit_init[port_index] = true;

    if (x_mux_bt_Semaphore == NULL) {
        x_mux_bt_Semaphore = xSemaphoreCreateMutex();
        configASSERT(x_mux_bt_Semaphore != NULL);
    }

    g_mux_bt_callback = irq_handler;
    serial_port_bt_config.callback = mux_bt_callback;
    mux_bt_ctx.rx_not_enough_count = 0;

    if (port_index == 0) {
        serial_port_bt_config.tx_buffer_size = 1024;
        serial_port_bt_config.rx_buffer_size = 1024;
    } else {
        serial_port_bt_config.tx_buffer_size = 1024;
        serial_port_bt_config.rx_buffer_size = 1024 + 256;
    }

    serial_port_status = serial_port_open(port_index + SERIAL_PORT_DEV_BT_SPP, &serial_port_bt_config, &serial_port_bt_handle[port_index]);
    LOG_MSGID_I(MUX_BT, "bt port open status[%d] port_idenx[%d] port_handle[%d]", 3, serial_port_status, port_index, serial_port_bt_handle[port_index]);
    if (serial_port_status != SERIAL_PORT_STATUS_OK) {
        return MUX_STATUS_ERROR;
    }

    mux_driver_debug_for_check(&g_mux_bt_r_w_point[port_index]);
    return MUX_STATUS_OK;
}

mux_status_t port_mux_bt_deinit(uint8_t port_index)
{
    serial_port_status_t status;

    mux_driver_debug_for_check(&g_mux_bt_r_w_point[port_index]);
    p_local_setting[port_index] = NULL;
    status = serial_port_close(serial_port_bt_handle[port_index]);
    LOG_MSGID_I(MUX_BT, "bt port close status[%d] port_idenx[%d] port_handle[%d]", 3, status, port_index, serial_port_bt_handle[port_index]);
    if (status != SERIAL_PORT_STATUS_OK) {
        return MUX_STATUS_ERROR_DEINIT_FAIL;
    }
    return MUX_STATUS_OK;
}

void port_mux_bt_exception_init(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
}

void port_mux_bt_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    //TODO: need bt replace
    //maybe exception bt disconnect ???
    //bt_mux_dump_data(port_index, buffer, size);
}

bool port_mux_bt_buf_is_full(uint8_t port_index, bool is_rx)
{
    return mux_common_device_buf_is_full(&g_mux_bt_r_w_point[port_index], is_rx);
}

uint32_t port_mux_bt_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_rptr(&g_mux_bt_r_w_point[port_index], is_rx);
}

uint32_t port_mux_bt_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_wptr(&g_mux_bt_r_w_point[port_index], is_rx);
}

void port_mux_bt_set_rx_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_rptr(&g_mux_bt_r_w_point[port_index], move_bytes);
}

static void port_mux_bt_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_wptr_internal_use(&g_mux_bt_r_w_point[port_index], move_bytes);
}

static void port_mux_bt_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_tx_hw_rptr_internal_use(&g_mux_bt_r_w_point[port_index], move_bytes);
}

void port_mux_bt_set_tx_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    virtual_read_write_point_t *p = &g_mux_bt_r_w_point[port_index];

    mux_common_device_set_tx_hw_wptr(p, move_bytes);
}

/* maybe have online log request , user is syslog,
    syslog can use any where.
*/
static void bt_gpt_callback(void *user_data)
{
    port_mux_bt_phase2_send(*(uint8_t *)user_data);
}

void port_mux_bt_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void port_mux_bt_phase2_send(uint8_t port_index)
{
    uint32_t per_cpu_irq_mask;
    uint32_t send_addr;
    virtual_read_write_point_t *p = &g_mux_bt_r_w_point[port_index];
    uint16_t tx_complete_size = 0;
    uint16_t tx_prepare_size = 0;

    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
    mux_driver_debug_for_check(p);
    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);

    g_mux_bt_ponit_init[port_index] = false;
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER == HAL_NVIC_NOT_EXCEPTION) { //xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED
        //LOG_MSGID_I(MUX_BT,"Task thread ", 0);
        /* Task context */
        xSemaphoreTake(x_mux_bt_Semaphore, portMAX_DELAY);
        mux_bt_ctx.tx_status = MUX_STATUS_OK;
        mux_bt_ctx.tx_complete_size = 0;
        g_mux_bt_phase2_send_status = MUX_DEVICE_HW_RUNNING;
        p->tx_send_is_running = MUX_DEVICE_HW_RUNNING;

        LOG_MSGID_I(MUX_BT, "[MUX BT] point:port_index = %d, r_point = %02x, w_point = %02x, s_point = %02x, e_point = %02x, tx_available_len", 6,
                    port_index, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_start, p->tx_buff_end, p->tx_buff_available_len);
        if (p->tx_buff_read_point <= p->tx_buff_write_point) {
            tx_prepare_size = p->tx_buff_write_point - p->tx_buff_read_point;
            uint8_t *data_point = (uint8_t *)(p->tx_buff_read_point);
            tx_complete_size = bt_send_data(port_index, data_point, p->tx_buff_write_point - p->tx_buff_read_point, &p->tx_sending_read_point);// user must update Read_point equle Write_point in this function
            LOG_MSGID_I(MUX_BT, "xSemaphoreGive OK tx complete size = %d,tx_prepare_size = %d\r\n", 2, tx_complete_size, tx_prepare_size);
        } else {
            uint8_t *first_data = (uint8_t *)(p->tx_buff_read_point);
            uint16_t first_data_length = p->tx_buff_end - p->tx_buff_read_point;
            uint8_t *second_data = (uint8_t *)(p->tx_buff_start);
            uint16_t second_data_length = p->tx_buff_write_point - p->tx_buff_start;
            uint8_t *full_buffer = (uint8_t *)pvPortMalloc(first_data_length + second_data_length);
            tx_prepare_size = first_data_length + second_data_length;
            if (full_buffer == NULL) {
                LOG_MSGID_I(MUX_BT, "[MUX BT] alloc full buffer fail ", 0);
                return;
            }
            /* copy data from ring buffer. */
            memcpy(full_buffer, first_data, first_data_length);
            memcpy(full_buffer + first_data_length, second_data, second_data_length);
            tx_complete_size = bt_send_data(port_index, full_buffer, first_data_length + second_data_length, &p->tx_sending_read_point);
            vPortFree(full_buffer);
            LOG_MSGID_I(MUX_BT, "Joining together xSemaphoreGive OK tx complete size = %d,tx_prepare_size = %d\r\n", 2, tx_complete_size, tx_prepare_size);
        }
        p->tx_send_is_running = MUX_DEVICE_HW_IDLE;
        g_mux_bt_phase2_send_status = MUX_DEVICE_HW_IDLE;
        xSemaphoreGive(x_mux_bt_Semaphore);
        if ((tx_complete_size != tx_prepare_size) || (tx_prepare_size == 0)) {
            mux_bt_ctx.tx_status = MUX_STATUS_ERROR;
        }
        mux_bt_ctx.tx_complete_size = tx_complete_size;
    } else {
        /* IRQ context */
        port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
        if (g_mux_bt_phase2_send_status == MUX_DEVICE_HW_IDLE) {
            g_mux_bt_phase2_send_status = MUX_DEVICE_HW_RUNNING;
            p->tx_send_is_running = MUX_DEVICE_HW_RUNNING;
            port_mux_local_cpu_exit_critical(per_cpu_irq_mask);

            mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
            send_addr = p->tx_buff_read_point;
            bt_send_data(port_index, (uint8_t *)send_addr, send_addr, &p->tx_sending_read_point);// user must update Read_point equle Write_point in this function

            p->tx_send_is_running = MUX_DEVICE_HW_IDLE;
            g_mux_bt_phase2_send_status = MUX_DEVICE_HW_IDLE;
        } else {
            port_mux_local_cpu_exit_critical(per_cpu_irq_mask);
            if (p->tx_buff_available_len != 0) {
                hal_gpt_status_t gpt_status = hal_gpt_sw_start_timer_ms ( g_bt_gpt_handler,
                                              BT_GPT_TIMEOUT,
                                              bt_gpt_callback,
                                              (uint8_t *)&port_index); // delay 1ms retry...
                if (gpt_status != HAL_GPT_STATUS_OK) {
                    LOG_MSGID_I(MUX_BT, "[MUX_BT] gpt start timer fail,status = %d", 1, gpt_status);
                }
            }
            return ;
        }
    }
}

mux_status_t port_mux_bt_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    mux_get_trx_status_t *p_mux_trx_status = (mux_get_trx_status_t *)para;
    switch (command) {
    case MUX_CMD_GET_TX_SEND_STATUS: {
        p_mux_trx_status->tx_send_status = mux_bt_ctx.tx_status;
        p_mux_trx_status->transfer_completed_size = mux_bt_ctx.tx_complete_size;
        return MUX_STATUS_OK;
    } break;
    case MUX_CMD_GET_RX_RECEIVED_STATUS: {
        return MUX_STATUS_OK;
    } break;
    case MUX_CMD_CLEAN: {
        LOG_MSGID_I(MUX_BT, "[MUX][BT] init point with port_index:%02x", 1, port_index);
        /* Clear mux bt point by index. */
        if (p_local_setting[port_index] != NULL) {
            g_mux_bt_ponit_init[port_index] = true;
            mux_common_device_r_w_point_init(&g_mux_bt_r_w_point[port_index], p_local_setting[port_index]);
        }
    } break;
    default: break;
    }
    return MUX_STATUS_ERROR;
}
port_mux_device_ops_t g_port_mux_bt_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_bt_normal_init,
    port_mux_bt_deinit,
    port_mux_bt_exception_init,
    port_mux_bt_exception_send,
    port_mux_bt_buf_is_full,
#endif
    port_mux_bt_get_hw_rptr,
    port_mux_bt_set_rx_hw_rptr,
    port_mux_bt_get_hw_wptr,
    port_mux_bt_set_tx_hw_wptr,
    port_mux_bt_phase1_send,
    port_mux_bt_phase2_send,
    port_mux_bt_control,
    NULL,
    NULL,
};
#endif
