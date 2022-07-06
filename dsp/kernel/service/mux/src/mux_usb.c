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

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "mux.h"
#include "mux_port_device.h"

#ifdef MTK_SINGLE_CPU_ENV
virtual_read_write_point_t g_mux_usb_r_w_point[2];
static volatile uint32_t mux_usb_update_hw_wptr[2];
#else
#include "hal_resource_assignment.h"
static volatile virtual_read_write_point_t *g_mux_usb_r_w_point = (volatile virtual_read_write_point_t *)HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_USB_VAR_START;
static volatile uint32_t *mux_usb_update_hw_wptr = (volatile uint32_t *)(HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_USB_VAR_START + sizeof(virtual_read_write_point_t) * 2);
#endif

#ifdef MTK_CPU_NUMBER_0

#define USB_PORT_INDEX_TO_MUX_PORT(port_index) (port_index + MUX_USB_COM_1)

mux_irq_handler_t g_mux_usb_callback;
serial_port_handle_t serial_port_usb_handle[2];

static void port_mux_usb_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes);
static void port_mux_usb_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes);


void port_mux_usb_normal_init(uint8_t port_index, uint32_t share_buffer_addr, uint32_t share_buffer_size, mux_irq_handler_t irq_handler)
{
    serial_port_status_t serial_port_status;
    serial_port_open_para_t serial_port_usb_config;
    mux_common_device_r_w_point_init((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index],p_setting);

    g_mux_usb_callback = irq_handler;
    serial_port_usb_config.callback = mux_usb_callback;
#ifdef MTK_PORT_SERVICE_SLIM_ENABLE
    serial_port_usb_config.tx_buffer_size = 1024;
    serial_port_usb_config.rx_buffer_size = 1024;
#endif

    serial_port_status = serial_port_open(port_index+SERIAL_PORT_DEV_USB_COM1, &serial_port_usb_config, &serial_port_usb_handle[port_index]);
    if(serial_port_status == SERIAL_PORT_STATUS_DEV_NOT_READY) {
        serial_port_close(serial_port_usb_handle[port_index]);
    }
    if(serial_port_status != SERIAL_PORT_STATUS_OK){
        return MUX_STATUS_ERROR;
    }
    mux_driver_debug_for_check((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index]);

    return MUX_STATUS_OK;
}

mux_status_t port_mux_usb_deinit(uint8_t port_index)
{
    serial_port_status_t status;

    mux_driver_debug_for_check((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index]);

    status = serial_port_close(serial_port_usb_handle[port_index]);
    if(status != SERIAL_PORT_STATUS_OK) {
        return MUX_STATUS_ERROR_DEINIT_FAIL;
    }
    return MUX_STATUS_OK;
}

void port_mux_usb_exception_init(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
}

void port_mux_usb_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    usb_mux_dump_data(port_index, buffer, size);
}

bool port_mux_usb_buf_is_full(uint8_t port_index, bool is_rx)
{
    return mux_common_device_buf_is_full((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index],is_rx);
}

#endif /* MTK_CPU_NUMBER_0 */

uint32_t port_mux_usb_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_rptr((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index],is_rx);
}

uint32_t port_mux_usb_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_wptr((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index],is_rx);
}

/*
    port_mux_usb_set_tx_hw_wptr: for Rx- MUX driver read data.
*/
void port_mux_usb_set_rx_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_rptr((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index],move_bytes);
}

void port_mux_usb_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_wptr_internal_use((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index],move_bytes);
}

/*port_mux_usb_set_tx_hw_rptr_internal_use: for Tx-HW send data from Tx buff
    This function only need by USB / I2C slave / SPI slave.
    As UART with VFIFO, HW will move Rx write point.
    But for  USB / I2C slave / SPI slave, mux_xxx driver should to do this.
*/
void port_mux_usb_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_tx_hw_rptr_internal_use((virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index],move_bytes);
}


/*
    port_mux_usb_set_tx_hw_wptr: for Tx- MUX driver send data.
*/
void port_mux_usb_set_tx_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    virtual_read_write_point_t *p = (virtual_read_write_point_t*)&g_mux_usb_r_w_point[port_index];
    mux_common_device_set_tx_hw_wptr(p, move_bytes);
    mux_usb_update_hw_wptr[port_index] = 1;
}

void port_mux_usb_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
}

void port_mux_usb_phase2_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
}

mux_status_t port_mux_usb_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    PORT_MUX_UNUSED(port_index);
    PORT_MUX_UNUSED(command);
    PORT_MUX_UNUSED(para);
    return MUX_STATUS_ERROR;
}

port_mux_device_ops_t g_port_mux_usb_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_usb_normal_init,
    port_mux_usb_deinit,
    port_mux_usb_exception_init,
    port_mux_usb_exception_send,
    port_mux_usb_buf_is_full,
#endif /* MTK_CPU_NUMBER_0 */
    port_mux_usb_get_hw_rptr,
    port_mux_usb_set_rx_hw_rptr,
    port_mux_usb_get_hw_wptr,
    port_mux_usb_set_tx_hw_wptr,
    port_mux_usb_phase1_send,
    port_mux_usb_phase2_send,
    port_mux_usb_control,
};

