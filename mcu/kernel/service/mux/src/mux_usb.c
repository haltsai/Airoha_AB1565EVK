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

#if defined(MTK_USB_DEMO_ENABLED)

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "hal_usb.h"
#include "mux.h"
#include "mux_port_device.h"
#include "hal_gpt.h"
#include "usb_custom_def.h"
#ifdef HAL_USB_MODULE_ENABLED

#include "hal_platform.h"
#include "hal_nvic.h"

#include "mux.h"
#include "mux_port_device.h"
#include "usbacm_drv.h"
#include "serial_port.h"
#include "assert.h"
#include "mux_port.h"

/* Syslog create module for mux_usb.c */
log_create_module_variant(MUX_USB, DEBUG_LOG_OFF, PRINT_LEVEL_INFO);

#ifdef MTK_SINGLE_CPU_ENV
virtual_read_write_point_t g_mux_usb_r_w_point[2];
static volatile uint32_t mux_usb_update_hw_wptr[2];
static volatile uint32_t *mux_usb_update_hw_wptr = &g_mux_usb_update_hw_wptr[0];
#else
#include "hal_resource_assignment.h"
static volatile virtual_read_write_point_t *g_mux_usb_r_w_point = (volatile virtual_read_write_point_t *)HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_USB_VAR_START;
static volatile uint32_t *mux_usb_update_hw_wptr = (volatile uint32_t *)(HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_USB_VAR_START + sizeof(virtual_read_write_point_t) * 2);
#endif

#ifdef MTK_CPU_NUMBER_0

#define USB_PORT_INDEX_TO_MUX_PORT(port_index) (port_index + MUX_USB_BEGIN)

mux_irq_handler_t g_mux_usb_callback;
serial_port_handle_t serial_port_usb_handle[2];

static void port_mux_usb_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes);
static void port_mux_usb_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes);

#include "FreeRTOS.h"
#include "semphr.h"

SemaphoreHandle_t x_mux_usb_Semaphore[2];

#ifndef MTK_USB_AUDIO_HID_ENABLE

void port_mux_usb_phase2_send(uint8_t port_index);

void usb_send_data(uint8_t port_index, uint32_t addr, uint32_t len, volatile uint32_t *sending_point)
{
    serial_port_status_t status;
    serial_port_write_data_t send_data;
    serial_port_get_write_avail_t usb_tx_avail_len;

    if (len == 0) {
        return;
    }

    if (USB_Get_Device_State() != DEVSTATE_CONFIG) {
        return;
    }

    send_data.data = (uint8_t *)addr;
    send_data.size = len;
    status = serial_port_control(serial_port_usb_handle[port_index], SERIAL_PORT_CMD_GET_WRITE_AVAIL, (serial_port_ctrl_para_t *)&usb_tx_avail_len);
    if (SERIAL_PORT_STATUS_OK != status) {
        return;
    }

    if (usb_tx_avail_len.ret_size < send_data.size) {
        send_data.size = usb_tx_avail_len.ret_size;
    }

    /*Must be  set the value of tx_sending_read_point firstly!!!!!must before call usb send!!!!!*/
    *sending_point = addr + send_data.size;

    status = serial_port_control(serial_port_usb_handle[port_index], SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t *)&send_data);

    if (SERIAL_PORT_STATUS_OK != status) {
        return;
    }
    if (send_data.ret_size != send_data.size) {
        /*this is a risk of race condition, because we need restart transfer on IRQ handle,
                if there have no time to update p->tx_sending_read_point, and the IRQ triggerred quickly, will be enter err status.
                So we must query avali size firstly ,and makesure the all of data must be sent done on one tranfser!!!*/
        assert(0);
    }
}

static bool usb_mux_dump_data(uint8_t usb_port, void *start_address, uint32_t size)
{
    uint32_t Length = 0;
    bool is_unalign_data = false;
    uint32_t data;

    while (size > 0) {
        if (size > 64) {
            Length = 64;
        } else if (size & 0x03) {
            Length = size & (~0x03);
            is_unalign_data = true;
        } else {
            Length = size;
        }

        hal_usb_set_endpoint_tx_ready(usb_port + 1);
        while (!hal_usb_is_endpoint_tx_empty(usb_port + 1));
        hal_usb_write_endpoint_fifo(usb_port + 1, Length, start_address);
        hal_usb_set_endpoint_tx_ready(usb_port + 1);
        while (!hal_usb_is_endpoint_tx_empty(usb_port + 1));

        size = size - Length;
        start_address = start_address + Length;

        if (is_unalign_data) {
            data = *(uint32_t *)(start_address);
            hal_usb_set_endpoint_tx_ready(usb_port + 1);
            while (!hal_usb_is_endpoint_tx_empty(usb_port + 1));
            hal_usb_write_endpoint_fifo(usb_port + 1, size & 0x03, (uint8_t *)&data);
            hal_usb_set_endpoint_tx_ready(usb_port + 1);
            while (!hal_usb_is_endpoint_tx_empty(usb_port + 1));
            size = size - (size & 0x03);
            start_address = start_address + (size & 0x03);
        }
    }
    return true;
}

static void mux_usb_callback(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
{
    uint8_t port_index = device - SERIAL_PORT_DEV_USB_COM1;
    virtual_read_write_point_t *p;
    uint32_t next_available_block_len, next_free_block_len, per_cpu_irq_mask;
    serial_port_read_data_t read_data;
    serial_port_status_t status;
    p = (virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index];

    switch (event) {
    case SERIAL_PORT_EVENT_READY_TO_WRITE:

        port_mux_cross_local_enter_critical(&per_cpu_irq_mask);

        if (p->tx_send_is_running != MUX_DEVICE_HW_RUNNING) {
            port_mux_cross_local_exit_critical(per_cpu_irq_mask);
            return;
        }

        //transfer done , then update tx_buff_read_point
        port_mux_usb_set_tx_hw_rptr_internal_use(port_index, p->tx_sending_read_point - p->tx_buff_read_point);

        next_available_block_len = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
        if (next_available_block_len == 0) {
            p->tx_send_is_running = MUX_DEVICE_HW_IDLE;//change to idle
            port_mux_cross_local_exit_critical(per_cpu_irq_mask);
            return;
        } else {
            p->tx_send_is_running = MUX_DEVICE_HW_RUNNING; //keep running
            //p->tx_sending_read_point = p->tx_buff_read_point + next_available_block_len;
            port_mux_cross_local_exit_critical(per_cpu_irq_mask);
            usb_send_data(port_index, p->tx_buff_read_point, next_available_block_len, &p->tx_sending_read_point);
            return;
        }
        break;

    case SERIAL_PORT_EVENT_READY_TO_READ:
        read_data.buffer = (uint8_t *)port_mux_malloc(p->rx_buff_len);
        read_data.size = p->rx_buff_len;

        status = serial_port_control(serial_port_usb_handle[port_index], SERIAL_PORT_CMD_READ_DATA, (serial_port_ctrl_para_t *)&read_data);
        if (status != SERIAL_PORT_STATUS_OK) {
            port_mux_free(read_data.buffer);
            LOG_MSGID_E(MUX_USB, "bt rx port control read data error status[%d] handle[%d],rx_buff_len = %d", 3, status, serial_port_usb_handle[port_index], p->rx_buff_len);
            return;
        }

        if (read_data.ret_size > (p->rx_buff_end - p->rx_buff_start - p->rx_buff_available_len)) {
            port_mux_free(read_data.buffer);
            LOG_MSGID_E(MUX_USB, "bt rx buffer not enough to save, len : %d", 1, read_data.ret_size);
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
        port_mux_usb_set_rx_hw_wptr_internal_use(port_index, read_data.ret_size);
        port_mux_free(read_data.buffer);
        g_mux_usb_callback(USB_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_READ, parameter);
        break;

    case SERIAL_PORT_EVENT_USB_CONNECTION:
        break;

    case SERIAL_PORT_EVENT_BT_DISCONNECTION:
        break;
    }
}

mux_status_t port_mux_usb_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    serial_port_status_t serial_port_status;
    serial_port_open_para_t serial_port_usb_config;
    mux_common_device_r_w_point_init((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index], p_setting);

    g_mux_usb_callback = irq_handler;
    serial_port_usb_config.callback = mux_usb_callback;
#ifdef MTK_PORT_SERVICE_SLIM_ENABLE
    serial_port_usb_config.tx_buffer_size = 1024;
    serial_port_usb_config.rx_buffer_size = 1024;
#endif

    serial_port_status = serial_port_open(port_index + SERIAL_PORT_DEV_USB_COM1, &serial_port_usb_config, &serial_port_usb_handle[port_index]);
    if (serial_port_status == SERIAL_PORT_STATUS_DEV_NOT_READY) {
    }

    if (x_mux_usb_Semaphore[port_index] == NULL) {
        x_mux_usb_Semaphore[port_index] = xSemaphoreCreateMutex();
        configASSERT(x_mux_usb_Semaphore[port_index] != NULL);
    }

    mux_driver_debug_for_check((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index]);

    return MUX_STATUS_OK;
}

mux_status_t port_mux_usb_deinit(uint8_t port_index)
{
    serial_port_status_t status;

    mux_driver_debug_for_check((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index]);

    status = serial_port_close(serial_port_usb_handle[port_index]);
    if (status != SERIAL_PORT_STATUS_OK) {
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

#else /* MTK_USB_AUDIO_HID_ENABLE */

/* USB MUX port converter */
#define USB_PORT_INDEX_TO_MUX_PORT(port_index) (port_index + MUX_USB_BEGIN)

void port_mux_usb_phase2_send(uint8_t port_index);

void usb_send_data(uint8_t port_index, uint32_t addr, uint32_t len, volatile uint32_t *sending_point)
{
    //serial_port_status_t status;
    //serial_port_write_data_t send_data;
    //serial_port_get_write_avail_t usb_tx_avail_len;
    hal_usb_status_t usb_ret;

    if (len == 0) {
        LOG_MSGID_I(MUX_USB, "mux_usb_callback usb_send_data len == 0", 0);
        return;
    }

    if (USB_Get_Device_State() != DEVSTATE_CONFIG) {
        LOG_MSGID_I(MUX_USB, "mux_usb_callback USB_Get_Device_State() != DEVSTATE_CONFIG", 0);
        return;
    }

    /*Must be  set the value of tx_sending_read_point firstly!!!!!must before call usb send!!!!!*/
    *sending_point = addr + len;

    usb_ret = hal_usb_start_dma_channel(2, HAL_USB_EP_DIRECTION_TX, HAL_USB_EP_TRANSFER_INTR, (void *)addr, len, NULL, false, HAL_USB_DMA1_TYPE);
    if (HAL_USB_STATUS_OK != usb_ret) {
        LOG_MSGID_I(MUX_USB, "hal_usb_start_dma_channel Fail %d", 1, usb_ret);
        return;
    }
}

/*
static bool usb_mux_dump_data(uint8_t usb_port, void *start_address, uint32_t size)
{
    return false;
}*/

#include "usb.h"
#include "usbaudio_drv.h"
#include "hal_usb.h"
#include "serial_port.h"

extern uint8_t usb_rx[USB_AUDIO_HID_REPORT_64BYTES];
extern uint8_t usb_tx[USB_AUDIO_HID_REPORT_64BYTES];
extern uint8_t usb_report_id;
extern uint8_t usb_report_type;
extern mux_usb_hid_callback g_mux_usb_hid_callback;

static uint8_t g_mux_usb_data_flag = 0;

uint8_t usb_mux_get_data_flag(void)
{
    return g_mux_usb_data_flag;
}

void mux_usb_callback(uint8_t usb_port, hid_callback_event_t event, void *parameter)
{
    LOG_MSGID_I(MUX_USB, "mux_usb_callback usb_port[%x] event[%x] usb_report_id[%x]", 3, usb_port, event, usb_report_id);

    uint8_t port_index = usb_port;
    uint8_t usb_cfu_len;
    uint32_t next_available_block_len, next_free_block_len;
    uint32_t per_cpu_irq_mask;

    virtual_read_write_point_t *p;
    p = (virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index];

    switch (event) {
    case HID_EVENT_READY_TO_WRITE:
        /* TEAMS - CFU */
        if (port_index == USB_MUX_PORT1) {
            if (usb_report_type == USB_HID_FEATURE_REPORT_TYPE ||
                    usb_report_id   == USB_CFU_VERSION_FEATURE_REPORT_ID) {
                usb_cfu_len = USB_CFU_VERSION_FEATURE_REPORT_LENGTH;
            } else {
                switch (usb_report_id) {
                case USB_CFU_CONTENT_OUT_REPORT_ID:
                    usb_cfu_len = USB_CFU_CONTENT_OUT_REPORT_LENGTH;
                    break;
                case USB_CFU_OFFER_OUT_REPORT_ID:
                    usb_cfu_len = USB_CFU_OFFER_OUT_REPORT_LENGTH;
                    break;
                default:
                    usb_cfu_len = 0;
                    LOG_MSGID_I(MUX_USB, "CFU Report ID 0x%02X is wrong", 1, usb_report_id);
                    break;
                }
            }

            if (p->tx_send_is_running != MUX_DEVICE_HW_RUNNING) {
                LOG_MSGID_I(MUX_USB, "MUX_DEVICE_HW_RUNNING", 0);
                return;
            }

            next_available_block_len = mux_common_device_get_buf_next_available_block_len(
                                           p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point,
                                           p->tx_buff_end, p->tx_buff_available_len);

            LOG_MSGID_I(MUX_USB, "next_available_block_len[%x] usb_cfu_len[%x]", 2, next_available_block_len, usb_cfu_len);

            /* Set the Maximum data byte for getting HID report */
            if (next_available_block_len > usb_cfu_len) {
                next_available_block_len = usb_cfu_len;
            }

            /* Copy the data from MUX-TX buffer to USB-TX buffer for EP */
            memcpy(&usb_tx[0], (void *)p->tx_buff_read_point, next_available_block_len);

            /* Check report ID */
            if (usb_tx[0] != usb_report_id) {
                LOG_MSGID_E(MUX_USB, "Report ID[%x] is not match usb_report_id[%x]", 2, usb_tx[0], usb_report_id);
            }

            LOG_MSGID_I(MUX_USB, "usb_tx: %x %x %x %x %x %x %x %x", 8, usb_tx[0], usb_tx[1], usb_tx[2], usb_tx[3], usb_tx[4], usb_tx[5], usb_tx[6], usb_tx[7]);

            p->tx_sending_read_point = p->tx_buff_read_point + next_available_block_len;

            /* transfer done , then update tx_buff_read_point */
            port_mux_usb_set_tx_hw_rptr_internal_use(port_index, p->tx_sending_read_point - p->tx_buff_read_point);

            g_mux_usb_callback(USB_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_WRITE, parameter);
            p->tx_send_is_running = MUX_DEVICE_HW_RUNNING;

            LOG_MSGID_I(MUX_USB, "port_index[%x] Index[%x]", 2, port_index, USB_PORT_INDEX_TO_MUX_PORT(port_index));
        }


        /* Get Report shoulde not check the result from which target device, because check point was already in Set Report */
        if (usb_report_id == USB_AUDIO_HID_IN_REPORT_ID) {
            if (p->tx_send_is_running != MUX_DEVICE_HW_RUNNING) {
                LOG_MSGID_I(MUX_USB, "MUX Device HW is running", 0);
                return;
            }

            port_mux_cross_local_enter_critical(&per_cpu_irq_mask);
            next_available_block_len = mux_common_device_get_buf_next_available_block_len(
                                           p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point,
                                           p->tx_buff_end, p->tx_buff_available_len);
            port_mux_cross_local_exit_critical(per_cpu_irq_mask);

            /* Set the Maximum data byte for getting HID report */
            if (next_available_block_len > USB_AUDIO_HID_REPORT_DATA_LENGTH) {
                next_available_block_len = USB_AUDIO_HID_REPORT_DATA_LENGTH;
            }

            /* Copy the data from MUX-TX buffer to USB-TX buffer for EP */
            memcpy(&usb_tx[USB_AUDIO_HID_REPORT_DATA_BEGIN], (void *) p->tx_buff_read_point, next_available_block_len);

            /* Set up : get report ID */
            usb_tx[USB_AUDIO_HID_REPORT_ID] = usb_report_id;

            /* Get target from set report data */
            usb_tx[USB_AUDIO_HID_REPORT_TARGET] = usb_rx[USB_AUDIO_HID_REPORT_TARGET];

            /* Set HID report length when MUX-TX buffer receive data */
            if (next_available_block_len > 0) {
                usb_tx[USB_AUDIO_HID_REPORT_LENGTH] = next_available_block_len;

                LOG_MSGID_I(MUX_USB, "Get Report 0x%X - usb_tx %02X %02X %02X %02X %02X %02X %02X %02X", 9, usb_report_id,
                            usb_tx[0], usb_tx[1], usb_tx[2], usb_tx[3], usb_tx[4], usb_tx[5], usb_tx[6], usb_tx[7]);
            } else {
                usb_tx[USB_AUDIO_HID_REPORT_LENGTH] = USB_AUDIO_HID_REPORT_DATA_NULL;
                LOG_MSGID_I(MUX_USB, "Get Report 0x%X - No data ", 1, usb_report_id);
            }

            /* transfer done , then update tx_buff_read_point */
            port_mux_cross_local_enter_critical(&per_cpu_irq_mask);
            port_mux_usb_set_tx_hw_rptr_internal_use(port_index, next_available_block_len);
            port_mux_cross_local_exit_critical(per_cpu_irq_mask);

            g_mux_usb_callback(USB_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_WRITE, parameter);
            p->tx_send_is_running = MUX_DEVICE_HW_RUNNING;
        } else {
            LOG_MSGID_E(MUX_USB, "Get Report FAIL ! usb_report_id = 0x%X is wrong !", 1, usb_report_id);
            LOG_MSGID_E(MUX_USB, "Get Report - usb_tx %02X %02X %02X %02X %02X %02X %02X %02X", 8,
                        usb_tx[0], usb_tx[1], usb_tx[2], usb_tx[3], usb_tx[4], usb_tx[5], usb_tx[6], usb_tx[7]);
        }
        break;

    case HID_EVENT_READY_TO_READ:
        /* TEAMS - CFU */
        if (port_index == USB_MUX_PORT1) {
            if (usb_report_type == USB_HID_FEATURE_REPORT_TYPE ||
                    usb_report_id   == USB_CFU_VERSION_FEATURE_REPORT_ID) {
                usb_cfu_len = USB_CFU_VERSION_FEATURE_REPORT_LENGTH;
            } else {
                switch (usb_report_id) {
                case USB_CFU_DUMMY_IN_REPORT_ID:
                    usb_cfu_len = USB_CFU_DUMMY_IN_REPORT_LENGTH;
                    break;
                case USB_CFU_CONTENT_IN_REPORT_ID:
                    usb_cfu_len = USB_CFU_CONTENT_IN_REPORT_LENGTH;
                    break;
                case USB_CFU_OFFER_IN_REPORT_ID:
                    usb_cfu_len = USB_CFU_OFFER_IN_REPORT_LENGTH;
                    break;
                default:
                    usb_cfu_len = 0;
                    LOG_MSGID_I(MUX_USB, "CFU Report ID 0x%02X is wrong", 1, usb_report_id);
                    break;
                }
            }

            /* MUX-RX buffer have some space to do receive. */
            next_free_block_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point, p->rx_buff_write_point, p->rx_buff_end, p->rx_buff_available_len);

            LOG_MSGID_I(MUX_USB, "next_free_block_len[%x] usb_cfu_len[%x]", 2, next_free_block_len, usb_cfu_len);

            /* The MUX-RX ring buffer is enough to copy data from USB-RX  */
            if (next_free_block_len >= usb_cfu_len) {
                next_free_block_len = usb_cfu_len;
                memcpy((void *)(p->rx_buff_write_point), &usb_rx[0], next_free_block_len);
                port_mux_usb_set_rx_hw_wptr_internal_use(port_index, next_free_block_len);
            }
            /* The MUX-RX ring buffer is NOT enough to copy data from USB-RX  */
            else {
                memcpy((void *)(p->rx_buff_write_point), &usb_rx[0], next_free_block_len);
                memcpy((void *)(p->rx_buff_start), &usb_rx[0 + next_free_block_len], usb_cfu_len - next_free_block_len);
                port_mux_usb_set_rx_hw_wptr_internal_use(port_index, usb_cfu_len);
            }

            LOG_MSGID_I(MUX_USB, "port_index[%x] Index[%x]", 2, port_index, USB_PORT_INDEX_TO_MUX_PORT(port_index));
            g_mux_usb_callback(USB_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_READ, parameter);
        }


        /* ULL - Race cmd and FW update */
        if (usb_rx[USB_AUDIO_HID_REPORT_ID] == USB_AUDIO_HID_OUT_REPORT_ID) {
            /* Choose the target device */
            if (usb_rx[USB_AUDIO_HID_REPORT_TARGET] == TARGET_LOCAL_DEVICE) {
                g_mux_usb_data_flag = TARGET_LOCAL_DEVICE;    /* Local */
            } else if (usb_rx[USB_AUDIO_HID_REPORT_TARGET] == TARGET_REMOTE_DEVICE) {
                g_mux_usb_data_flag = TARGET_REMOTE_DEVICE;   /* Remote */
            } else if (usb_rx[USB_AUDIO_HID_REPORT_TARGET] == TARGET_EXTERNAL_DEVICE) {
                g_mux_usb_data_flag = TARGET_EXTERNAL_DEVICE; /* External */
            } else {
                LOG_MSGID_E(MUX_USB, "Target device[%x] is not expected", 1, usb_rx[USB_AUDIO_HID_REPORT_TARGET]);
                break;
            }
            LOG_MSGID_I(MUX_USB, "Target device[%x]", 1, usb_rx[USB_AUDIO_HID_REPORT_TARGET]);

            /* MUX-RX buffer have some space to do receive. */
            next_free_block_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point, p->rx_buff_write_point, p->rx_buff_end, p->rx_buff_available_len);

            LOG_MSGID_I(MUX_USB, "Set Report - available len = 0x%X next free block len = 0x%X report data len = 0x%X", 3,
                        p->rx_buff_available_len, next_free_block_len, usb_rx[USB_AUDIO_HID_REPORT_LENGTH]);

            /* The MUX-RX ring buffer is enough to copy data from USB-RX  */
            if (next_free_block_len >= usb_rx[USB_AUDIO_HID_REPORT_LENGTH]) {
                next_free_block_len = usb_rx[USB_AUDIO_HID_REPORT_LENGTH];
                memcpy((void *)(p->rx_buff_write_point), &usb_rx[USB_AUDIO_HID_REPORT_DATA_BEGIN], next_free_block_len);
                port_mux_usb_set_rx_hw_wptr_internal_use(port_index, next_free_block_len);
            }
            /* The MUX-RX ring buffer is NOT enough to copy data from USB-RX  */
            else {
                memcpy((void *)(p->rx_buff_write_point), &usb_rx[USB_AUDIO_HID_REPORT_DATA_BEGIN], next_free_block_len);
                memcpy((void *)(p->rx_buff_start), &usb_rx[USB_AUDIO_HID_REPORT_DATA_BEGIN + next_free_block_len], usb_rx[USB_AUDIO_HID_REPORT_LENGTH] - next_free_block_len);
                port_mux_usb_set_rx_hw_wptr_internal_use(port_index, usb_rx[USB_AUDIO_HID_REPORT_LENGTH]);
            }
            g_mux_usb_callback(USB_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_READ, parameter);
        } else {
            LOG_MSGID_E(MUX_USB, "Set Report FAIL ! REPORT ID 0x%X or Target Device 0x%X is wrong", 2, usb_rx[USB_AUDIO_HID_REPORT_ID], usb_rx[USB_AUDIO_HID_REPORT_TARGET]);
        }
        break;

    case HID_EVENT_USB_CONNECTION:
        break;

    case HID_EVENT_USB_DISCONNECTION:
        break;
    }
}



mux_status_t port_mux_usb_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    LOG_MSGID_I(MUX_USB, "port_mux_usb_normal_init port_index[%x]", 1, port_index);

    //serial_port_status_t serial_port_status;
    mux_common_device_r_w_point_init((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index], p_setting);

    g_mux_usb_callback = irq_handler;

    if (x_mux_usb_Semaphore[port_index] == NULL) {
        x_mux_usb_Semaphore[port_index] = xSemaphoreCreateMutex();
        configASSERT(x_mux_usb_Semaphore[port_index] != NULL);
    }
    g_mux_usb_hid_callback = mux_usb_callback;

    mux_driver_debug_for_check((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index]);

    return MUX_STATUS_OK;
}

mux_status_t port_mux_usb_deinit(uint8_t port_index)
{
    LOG_MSGID_I(MUX_USB, "port_mux_usb_deinit port_index[%x]", 1, port_index);

    //serial_port_status_t status;

    mux_driver_debug_for_check((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index]);

    return MUX_STATUS_OK;
}

/* Exeception log environment setting */
#include "hal_gpt.h"
#define MUX_USB_DEBUG_PORT HAL_UART_0
static bool mux_usb_self_debug_flag = false;
static void mux_usb_uart_printf(const char *message, ...);
extern Usb_Device gUsbDevice;

void port_mux_usb_exception_init(uint8_t port_index)
{
    /* Only for usb exception dump debug */
    hal_uart_config_t uart_config;

    hal_uart_deinit(MUX_USB_DEBUG_PORT);
    uart_config.baudrate = CONFIG_SYSLOG_BAUDRATE;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    hal_uart_init(MUX_USB_DEBUG_PORT, &uart_config);
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
    hal_uart_set_software_flowcontrol(MUX_USB_DEBUG_PORT, 0x11, 0x13, 0x77);
#endif

    mux_usb_self_debug_flag = true;
    /* unused variable */
    PORT_MUX_UNUSED(port_index);
}

static void mux_usb_uart_printf(const char *message, ...)
{
    va_list args;
    int32_t log_size;
    char frame_header[256];
    uint8_t race_header[12];

    if (mux_usb_self_debug_flag == false) {
        return ;
    }

    va_start(args, message);

    log_size = vsnprintf(frame_header, sizeof(frame_header), message, args);
    if (log_size < 0) {
        va_end(args);
        return ;
    }

    if ((uint32_t)log_size >= sizeof(frame_header)) {
        log_size = sizeof(frame_header) - 1;
    }
    va_end(args);

    race_header[0] = 0x05;
    race_header[1] = 0x5D;
    race_header[2] = (uint8_t)((log_size + 2 + 6) & 0xFF);
    race_header[3] = (uint8_t)(((log_size + 2 + 6) >> 8) & 0xFF);
    race_header[4] = (uint8_t)(0x0F10 & 0xFF);
    race_header[5] = (uint8_t)((0x0F10 >> 8) & 0xFF);

    race_header[6]  = 0; // CPU ID = 0
    race_header[7]  = 0;
    race_header[8]  = 0; //timestamp
    race_header[9]  = 0;
    race_header[10] = 0;
    race_header[11] = 0;

    hal_uart_send_polling(MUX_USB_DEBUG_PORT, (uint8_t *)race_header, sizeof(race_header));
    hal_uart_send_polling(MUX_USB_DEBUG_PORT, (uint8_t *)frame_header, log_size);
}

static bool usb_mux_dump_data(uint8_t usb_port, void *start_address, uint32_t size)
{
    uint8_t  dump_data[USB_AUDIO_HID_REPORT_64BYTES] = {0};
    uint32_t Length;

    /* Send exception  log */
    while (size > 0) {

        /* Check the HID report data length is 59 bytes */
        if (size > USB_AUDIO_HID_REPORT_DATA_LENGTH) {
            Length = USB_AUDIO_HID_REPORT_DATA_LENGTH;
        } else {
            Length = size;
        }

        /* Get the USB stdcmd of Set report from EP0 */
        memset(&gUsbDevice.cmd, 0, sizeof(gUsbDevice.cmd));
        hal_usb_read_endpoint_fifo(0, sizeof(gUsbDevice.cmd), &gUsbDevice.cmd);

        /* Check the report ID and send data via USB-HID */
        if ( (gUsbDevice.cmd.wValue & 0x00FF) == USB_AUDIO_HID_IN_REPORT_ID) {

            /* Clear buffer */
            memset(dump_data, 0, sizeof(dump_data));

            /* byte[0]:Get report ID
               byte[1]:Data length
               byte[2]:Target device */
            dump_data[0] = usb_report_id;
            dump_data[1] = Length;
            dump_data[2] = 0;

            /* Get data from exception module */
            memcpy(&dump_data[3], start_address, Length);

            /* Print log for debug */
            mux_usb_uart_printf("size = %x Length = %x", size, Length);
            mux_usb_uart_printf("ReportID[0]=%x Length[1]=%x Target[2]=%x", dump_data[0], dump_data[1], dump_data[2]);
            mux_usb_uart_printf("Data[0]=%x [1]=%x [2]=%x [3]=%x [4]=%x [5]=%x [6]=%x [7]=%x [8]=%x [9]=%x [10]=%x [11]=%x [12]=%x [13]=%x [14]=%x [15]=%x",
                                dump_data[3], dump_data[4], dump_data[5], dump_data[6], dump_data[7], dump_data[8], dump_data[9], dump_data[10], dump_data[11], dump_data[12],
                                dump_data[13], dump_data[14], dump_data[15], dump_data[16], dump_data[17], dump_data[18]);

            /* Prepare data for EP0 */
            USB_Generate_EP0Data(&gUsbDevice.ep0info, &gUsbDevice.cmd, &dump_data, USB_AUDIO_HID_REPORT_62BYTES);
            /* Send data by EP0 */
            USB_Endpoint0_Tx();

            /* Update variable */
            size = size - Length;
            start_address = start_address + Length;

            /* Delay 1ms to void continuous Get Report since Tool's flow */
            hal_gpt_delay_us(1000);
        }
    }

    /* Waiting for exception  log */
    hal_gpt_delay_us(200);

    return true;
}

void port_mux_usb_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    usb_mux_dump_data(port_index, buffer, size);
    PORT_MUX_UNUSED(port_index);
}
#endif


#endif
bool port_mux_usb_buf_is_full(uint8_t port_index, bool is_rx)
{
    return mux_common_device_buf_is_full((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index], is_rx);
}

uint32_t port_mux_usb_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_rptr((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index], is_rx);
}

uint32_t port_mux_usb_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_wptr((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index], is_rx);
}

/*
    port_mux_usb_set_tx_hw_wptr: for Rx- MUX driver read data.
*/
void port_mux_usb_set_rx_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_rptr((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index], move_bytes);
}

static void port_mux_usb_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_wptr_internal_use((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index], move_bytes);
}

/*port_mux_usb_set_tx_hw_rptr_internal_use: for Tx-HW send data from Tx buff
    This function only need by USB / I2C slave / SPI slave.
    As UART with VFIFO, HW will move Rx write point.
    But for  USB / I2C slave / SPI slave, mux_xxx driver should to do this.
*/
static void port_mux_usb_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_tx_hw_rptr_internal_use((virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index], move_bytes);
}


/*
    port_mux_usb_set_tx_hw_wptr: for Tx- MUX driver send data.
*/
void port_mux_usb_set_tx_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    virtual_read_write_point_t *p = (virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index];

    mux_common_device_set_tx_hw_wptr(p, move_bytes);

    if (USB_Get_Device_State() != DEVSTATE_CONFIG) {
        //mux_common_device_set_tx_hw_rptr_internal_use(p, move_bytes);
        p->tx_buff_read_point =  p->tx_buff_write_point;
        p->tx_buff_available_len = 0;
        p->tx_send_is_running = MUX_DEVICE_HW_IDLE;
        return;
    } else {
        p->tx_send_is_running = MUX_DEVICE_HW_RUNNING;
    }
}

void port_mux_usb_clear_rx_buf(uint8_t port_index)
{
    virtual_read_write_point_t *p = (virtual_read_write_point_t *)&g_mux_usb_r_w_point[port_index];

    p->rx_buff_read_point =  p->rx_buff_write_point;
    p->rx_buff_available_len = 0;
    return;
}


void port_mux_usb_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void port_mux_usb_phase2_send(uint8_t port_index)
{
    /* HID request mode */
    PORT_MUX_UNUSED(port_index);
    return ;
}

mux_status_t port_mux_usb_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    uint32_t mask;
    switch (command) {
    case MUX_CMD_CLEAN: {
        port_mux_usb_clear_rx_buf(port_index);
        hal_nvic_save_and_set_interrupt_mask(&mask);
        if (mux_usb_update_hw_wptr[port_index] == 1) {
            mux_usb_update_hw_wptr[port_index] = 0;
            hal_nvic_restore_interrupt_mask(mask);
            /* need remove if use USB_CDC log for DSP log */
            // port_mux_usb_phase2_send(port_index);
        } else {
            hal_nvic_restore_interrupt_mask(mask);
        }
        return MUX_STATUS_OK;
    } break;

    default:  {
    } break;
    }

    return MUX_STATUS_ERROR;
}

port_mux_device_ops_t g_port_mux_usb_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_usb_normal_init,
    port_mux_usb_deinit,
    port_mux_usb_exception_init,
    port_mux_usb_exception_send,
    port_mux_usb_buf_is_full,
#endif
    port_mux_usb_get_hw_rptr,
    port_mux_usb_set_rx_hw_rptr,
    port_mux_usb_get_hw_wptr,
    port_mux_usb_set_tx_hw_wptr,
    port_mux_usb_phase1_send,
    port_mux_usb_phase2_send,
    port_mux_usb_control,
    NULL,
    NULL,
};

#endif
#endif

