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

#include "mux.h"
#include "mux_port.h"
#include "mux_port_device.h"
#include "mux_pseudo.h"
#include "FreeRTOS.h"
#include "semphr.h"

extern void race_script_pseudo_read_data(uint32_t port_index, pseudo_port_read_data_t *data);
extern void race_script_pseudo_write_rsp(uint32_t port_index, uint32_t length, uint32_t data_ptr);

#ifdef MTK_CPU_NUMBER_0

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
#define MAX_PSEUDO_NUMBER   1
#define PSEUDO_PORT_INDEX_TO_MUX_PORT(port_index) (port_index + MUX_PORT_PSEUDO)

/* Private variables ---------------------------------------------------------*/
SemaphoreHandle_t g_mux_pseudo_Semaphore;
virtual_read_write_point_t g_mux_pseudo_r_w_point[MAX_PSEUDO_NUMBER];
bool g_mux_pseudo_init_status[MAX_PSEUDO_NUMBER] = {false};
mux_irq_handler_t g_mux_pseudo_callback;
static void port_mux_pseudo_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes);

/* Private function ---------------------------------------------------------*/
void mux_pseudo_callback(pseudo_callback_event_t event, void *user_data)
{
    uint32_t  port_index = (uint32_t)user_data;
    uint32_t  move_bytes;
    uint32_t  next_available_block_len, next_free_block_len, per_cpu_irq_mask;

    pseudo_port_read_data_t read_data;
    virtual_read_write_point_t *p;
    p = (virtual_read_write_point_t *)&g_mux_pseudo_r_w_point[port_index];

    switch (event) {
    case PSEUDO_EVENT_READY_TO_WRITE: {
        g_mux_pseudo_callback(PSEUDO_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_WRITE, NULL);
        break;
    }

    case PSEUDO_EVENT_READY_TO_READ: {
        //read_data.buffer = (uint8_t *)port_mux_malloc(p->rx_buff_len);
        read_data.size = p->rx_buff_len;

        race_script_pseudo_read_data(port_index, (pseudo_port_read_data_t *)&read_data);

        if (read_data.ret_size > (p->rx_buff_end - p->rx_buff_start - p->rx_buff_available_len)) {
            //port_mux_free(read_data.buffer);
            LOG_MSGID_E(common, "bt rx buffer not enough to save, len:%d", 1, read_data.ret_size);
            return;
        }

        // Rx buffer have some space to do receive.
        next_free_block_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point, p->rx_buff_write_point, p->rx_buff_end, p->rx_buff_available_len);

        if (next_free_block_len >= read_data.ret_size) {
            memcpy((void *)(p->rx_buff_write_point), read_data.buffer, read_data.ret_size);
        } else {
            memcpy((void *)(p->rx_buff_write_point), read_data.buffer, next_free_block_len);
            memcpy((void *)(p->rx_buff_start), read_data.buffer + next_free_block_len, read_data.ret_size - next_free_block_len);
        }

        MUX_PORT_MSGID_I("[PSEUDO] READY_TO_READ pseudo_port_%d move_bytes=%d", 2, (int)port_index, (int)read_data.ret_size);
        port_mux_pseudo_set_rx_hw_wptr_internal_use(port_index, read_data.ret_size);
        //port_mux_free(read_data.buffer);
        g_mux_pseudo_callback(PSEUDO_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_READ, NULL);
        break;
    }

    case PSEUDO_EVENT_READY_TRANSMISSION_DONE: {
        g_mux_pseudo_callback(PSEUDO_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_TRANSMISSION_DONE, NULL);
        break;
    }
    }
}

static void port_mux_pseudo_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_wptr_internal_use((virtual_read_write_point_t *)&g_mux_pseudo_r_w_point[port_index], move_bytes);
}

/*port_mux_pseudo_set_tx_hw_rptr_internal_use: for Tx-HW send data from Tx buff
    This function only need by USB / I2C slave / SPI slave.
    As UART with VFIFO, HW will move Rx write point.
    But for  USB / I2C slave / SPI slave, mux_xxx driver should to do this.
*/
static void port_mux_pseudo_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_tx_hw_rptr_internal_use((virtual_read_write_point_t *)&g_mux_pseudo_r_w_point[port_index], move_bytes);
}


/* standard function ---------------------------------------------------------*/
mux_status_t port_mux_pseudo_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    if (g_mux_pseudo_Semaphore == NULL) {
        g_mux_pseudo_Semaphore = xSemaphoreCreateMutex();
        configASSERT(g_mux_pseudo_Semaphore != NULL);
    }

    if (g_mux_pseudo_init_status[port_index] == false) {
        virtual_read_write_point_t *p = &g_mux_pseudo_r_w_point[port_index];
        mux_common_device_r_w_point_init(p, p_setting);
        mux_driver_debug_for_check(&g_mux_pseudo_r_w_point[port_index]);
        g_mux_pseudo_callback = irq_handler;
        g_mux_pseudo_init_status[port_index] = true;
        MUX_PORT_MSGID_I("[PSEUDO] pseudo_port init done", 0);
        return MUX_STATUS_OK;
    } else {
        MUX_PORT_MSGID_I("[PSEUDO] pseudo_port_%d already init ever!!!", 1, (int)port_index);
        return MUX_STATUS_ERROR;
    }
}

mux_status_t port_mux_pseudo_deinit(uint8_t port_index)
{
    if (g_mux_pseudo_init_status[port_index] == true) {
        vSemaphoreDelete(g_mux_pseudo_Semaphore);
        virtual_read_write_point_t *p = &g_mux_pseudo_r_w_point[port_index];
        //mux_common_device_r_w_point_init(p,p_setting);
        mux_driver_debug_for_check(&g_mux_pseudo_r_w_point[port_index]);

        g_mux_pseudo_init_status[port_index] = false;
        return MUX_STATUS_OK;
    } else {
        MUX_PORT_MSGID_I("[PSEUDO] pseudo_port_%d never been init!!!", 1, (int)port_index);
        return MUX_STATUS_ERROR;
    }
}


bool port_mux_pseudo_buf_is_full(uint8_t port_index, bool is_rx)
{
    return mux_common_device_buf_is_full((virtual_read_write_point_t *)&g_mux_pseudo_r_w_point[port_index], is_rx);
}

#endif


uint32_t port_mux_pseudo_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_rptr((virtual_read_write_point_t *)&g_mux_pseudo_r_w_point[port_index], is_rx);
}

void port_mux_pseudo_set_rx_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_rptr((virtual_read_write_point_t *)&g_mux_pseudo_r_w_point[port_index], move_bytes);

}

uint32_t port_mux_pseudo_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_wptr((virtual_read_write_point_t *)&g_mux_pseudo_r_w_point[port_index], is_rx);
}

void port_mux_pseudo_set_tx_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_tx_hw_wptr((virtual_read_write_point_t *)&g_mux_pseudo_r_w_point[port_index], move_bytes);
}

void port_mux_pseudo_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void port_mux_pseudo_phase2_send(uint8_t port_index)
{
    uint32_t per_cpu_irq_mask;
    uint32_t send_addr, send_len;
    virtual_read_write_point_t *p = &g_mux_pseudo_r_w_point[port_index];

    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
    mux_driver_debug_for_check(p);
    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);

    xSemaphoreTake(g_mux_pseudo_Semaphore, portMAX_DELAY);
    p->tx_send_is_running = MUX_DEVICE_HW_RUNNING;

    MUX_PORT_MSGID_I("[PSEUDO] Semaphore taken.", 0);
    send_len  = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
    send_addr = p->tx_buff_read_point;

    MUX_PORT_MSGID_I("[PSEUDO] first get_buf_next_available send_len = %d address = %08x ", 2, send_len, send_addr);
    race_script_pseudo_write_rsp(port_index, send_len, send_addr);
    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
    port_mux_pseudo_set_tx_hw_rptr_internal_use(port_index, send_len);
    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);

    if (p->tx_buff_read_point != p->tx_buff_write_point) {
        send_len  = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
        send_addr = p->tx_buff_read_point;
        MUX_PORT_MSGID_I("[PSEUDO] second get_buf_next_available send_len = %d address = %08x ", 2, send_len, send_addr);
        race_script_pseudo_write_rsp(port_index, send_len, send_addr);
        port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
        port_mux_pseudo_set_tx_hw_rptr_internal_use(port_index, send_len);
        port_mux_local_cpu_exit_critical(per_cpu_irq_mask);
    }

    p->tx_send_is_running = MUX_DEVICE_HW_IDLE;
    xSemaphoreGive(g_mux_pseudo_Semaphore);
    MUX_PORT_MSGID_I("[PSEUDO] xSemaphore given", 0);

    return;
}


mux_status_t port_mux_pseudo_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    return MUX_STATUS_ERROR;
}

port_mux_device_ops_t g_port_mux_pseudo_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_pseudo_normal_init,
    port_mux_pseudo_deinit,
    NULL,
    NULL,
    port_mux_pseudo_buf_is_full,
#endif
    port_mux_pseudo_get_hw_rptr,
    port_mux_pseudo_set_rx_hw_rptr,
    port_mux_pseudo_get_hw_wptr,
    port_mux_pseudo_set_tx_hw_wptr,
    port_mux_pseudo_phase1_send,
    port_mux_pseudo_phase2_send,
    port_mux_pseudo_control,
};

