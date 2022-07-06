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

/* System Header file */
#include "FreeRTOS.h"
#include "task.h"
#include "hal_uart.h"
#include "syslog.h"
#include "atci_adapter.h"
#include "atci_main.h"
#include "timers.h"
#include "semphr.h"

#include "at_command.h"
#include "memory_attribute.h"
#include <string.h>
#include <stdio.h>
#include <assert.h>


#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
#include "mux.h"
#include "mux_port_device.h"
#include "serial_port_assignment.h"
#include "usb_case.h"
#endif

log_create_module(atci, PRINT_LEVEL_INFO);
#if 1
#define LOGE(fmt,arg...)   LOG_E(atci, "ATCI main: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(atci, "ATCI main: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(atci ,"ATCI main: "fmt,##arg)

#define LOGMSGIDE(fmt,arg...)   LOG_MSGID_E(atci ,"ATCI main: "fmt,##arg)
#define LOGMSGIDW(fmt,arg...)   LOG_MSGID_W(atci ,"ATCI main: "fmt,##arg)
#define LOGMSGIDI(fmt,arg...)   LOG_MSGID_I(atci ,"ATCI main: "fmt,##arg)

#else
#define LOGE(fmt,arg...)   printf("ATCI main:"fmt,##arg)
#define LOGW(fmt,arg...)   printf("ATCI main:"fmt,##arg)
#define LOGI(fmt,arg...)   printf("ATCI main:"fmt,##arg)
#endif

#define ATCI_SERIAL_PORT_DATA_CALLBACK   "atci_serial_port_data_callback"
#define ATCI_SERIAL_PORT_INIT   "atci_serial_port_init"
#define ATCI_SERIAL_PORT_DEINIT   "atci_serial_port_deinit"
#define ATCI_SERIAL_PORT_READ_DATA   "atci_serial_port_read_data"
#define ATCI_SERIAL_PORT_SEND_DATA   "atci_serial_port_send_data"
#define ATCI_SERIAL_PORT_SWITCH   "atci_serial_port_switch"
#define ATCI_UART_IRQ "atci_uart_irq"
#define ATCI_UART_INIT "atci_uart_init"
#define ATCI_LOCAL_INIT "atci_local_init"
#define ATCI_INTT_INT "atci_init_int"
#define ATCI_SEND_DATA_INT "atci_send_data_int"
#define ATCI_SEND_DATA "atci_send_data"
#define ATCI_READ_DATA "atci_read_data"
#define ATCI_PROCESSING "atci_processing"
#define ATCI_DATA_PROCESSING_FOR_DEINIT "atci_data_processing_for_deinit"
#define ATCI_DEINIT "atci_deinit"

#define _unused(x) ((void)x)

/* ATCI main body related */
// g_atci_local_startup_flag = 0 means atci deinit; 1 means atci can work normal; 2 means in mode switching; 3 means atci just do local init.
uint8_t  g_atci_local_startup_flag = 0;
uint32_t g_atcmd_table_semaphore = NULL;
uint32_t g_atci_handler_semaphore = NULL;
uint32_t g_atci_processing_semaphore = NULL;

uint32_t g_atci_input_command_queue = NULL;
uint32_t g_atci_resonse_command_queue = NULL;
uint32_t g_atci_data_cached_queue = NULL;
uint32_t g_atci_registered_table_number = 0;
uint32_t atci_input_commad_in_processing = 0;

atci_cmd_hdlr_table_t g_atci_cm4_general_hdlr_tables[ATCI_MAX_GNENERAL_TABLE_NUM] = {{0}};

//static atci_send_input_cmd_msg_t   g_input_command = {{0}};
static atci_send_input_cmd_msg_t   *g_input_command = NULL;

hal_uart_port_t g_atci_uart_port = HAL_UART_0;
hal_uart_port_t g_atci_uart_port_default = HAL_UART_0;

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)

serial_port_dev_t g_serial_port = SERIAL_PORT_DEV_UART_1;
serial_port_dev_t g_switched_serial_port = SERIAL_PORT_DEV_UART_1;
//serial_port_handle_t g_serial_port_handle = 0;
serial_port_handle_t switched_serial_port_handle = 0;

#else
#ifndef MTK_ATCI_VIA_MUX
typedef struct {
    char    uart_rx_buff[ATCI_UART_RX_FIFO_BUFFER_SIZE];
    char    uart_tx_buff[ATCI_UART_TX_FIFO_BUFFER_SIZE];
} atci_uart_cntx_st;


ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN static atci_uart_cntx_st  g_atci_uart_cntx;
#endif
#endif

uint32_t g_atci_port_handle = 0;

// for send data cached
uint32_t g_send_data_cache_left_len = 0;
uint32_t g_send_data_cache_len = 0;
uint8_t *g_send_data_cache = NULL;

//for weakup sleep
static uint8_t atci_uart_rx_sleep_handle = 0xFF;
static TimerHandle_t atci_sleep_lock_timer = NULL;

atci_status_t atci_init_int(void);
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
atci_status_t atci_read_mux_data(uint32_t data_len, mux_handle_t handle);
#else
atci_status_t atci_read_data(void);
#endif
atci_status_t atci_port_deinit(uint32_t port);
atci_status_t atci_data_processing_for_deinit(void);
atci_status_t atci_vaild_port_check(uint32_t port);
static void atci_start_sleep_lock_timer(void);
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE) && defined(MTK_USB_DEMO_ENABLED)
void atci_mux_usb_plug(bool plug_in);
#endif

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)

void atci_serial_port_data_callback(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
{
    atci_general_msg_t msg_queue_item;
    //uint16_t		   queue_msg_num = 0;

    //LOGW("atci_serial_port_data_callback, event = %d, atci status = %d\r\n", event, g_atci_local_startup_flag);

    switch(event) {
        case SERIAL_PORT_EVENT_READY_TO_READ: {
            LOGMSGIDI(ATCI_SERIAL_PORT_DATA_CALLBACK",read event\r\n", 0);

            if((g_atci_local_startup_flag == 3) &&
                    ((g_serial_port == SERIAL_PORT_DEV_USB_COM1) || (g_serial_port == SERIAL_PORT_DEV_USB_COM2))) {
                atci_init_int();
            }

            if (atci_check_startup_finish()) {
                LOGMSGIDI(ATCI_SERIAL_PORT_DATA_CALLBACK", 1, read event\r\n", 0);
                //queue_msg_num = atci_queue_get_item_num(g_atci_input_command_queue);

                //if (queue_msg_num >= ATCI_LOCAL_QUEUE_LENGTH)
                //	LOGW("atci_serial_port_data_callback, input read queue full (len: %d)\r\n", queue_msg_num);

                msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_SEND_CMD_IND;
                msg_queue_item.msg_data = NULL;
                atci_queue_send(g_atci_input_command_queue, (void *)&msg_queue_item);
                atci_semaphore_give(g_atci_handler_semaphore);
            }

            break;
        }

        case SERIAL_PORT_EVENT_READY_TO_WRITE: {
            if (atci_check_startup_finish() && g_send_data_cache_left_len) {
                //queue_msg_num = atci_queue_get_item_num(g_atci_data_cached_queue);

                //if (queue_msg_num >= ATCI_LOCAL_QUEUE_LENGTH)
                //	LOGW("atci_serial_port_data_callback, cache data queue full (len: %d)\r\n", queue_msg_num);

                LOGMSGIDI(ATCI_SERIAL_PORT_DATA_CALLBACK ",write event\r\n", 0);
            }
            break;
        }
        case SERIAL_PORT_EVENT_WAKEUP_SLEEP: {
            #ifdef HAL_SLEEP_MANAGER_ENABLED
            LOGMSGIDI("wakeup sleep event!\r\n", 0);
            msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_WAKEUP_SLEEP_EVENT_NOTIFY_IND;
            atci_queue_send(g_atci_data_cached_queue, (void *)&msg_queue_item);
            atci_semaphore_give(g_atci_handler_semaphore);
            #endif
            break;
        }
#if 0
        case SERIAL_PORT_EVENT_BTSPP_CONNECTION: {
            LOGMSGIDW("atci_serial_port_data_callback, btspp connection event(parameter:%d)\r\n",1, parameter);

            if(parameter == BT_STATUS_SUCCESS) {
                // connect success
                if(g_atci_local_startup_flag == 2) {
                    // close serial port
                    serial_port_close(g_atci_port_handle);
                    g_atci_port_handle = switched_serial_port_handle;
                    g_atci_local_startup_flag = 1;
                    g_serial_port = g_switched_serial_port;
                    LOGMSGIDW("atci_serial_port_data_callback() switch to BTSPP success\r\n", 0);
                    return;
                } else if(g_atci_local_startup_flag == 3) {
                    atci_init_int();
                }
            } else {
                // connect fail
                if(g_atci_local_startup_flag == 2) {
                    // keep old serial port
                    switched_serial_port_handle = 0;
                    g_atci_local_startup_flag = 1;
                    g_switched_serial_port = SERIAL_PORT_DEV_UART_TYPE_BEGIN;
                    LOGMSGIDW("atci_serial_port_data_callback() switch to BTSPP fail\r\n", 0);
                    return;
                } else if(g_atci_local_startup_flag == 3) {
                    serial_port_close(g_atci_port_handle);
                }
            }

            break;
        }

        case SERIAL_PORT_EVENT_BTSPP_DISCONNECTION: {
            LOGMSGIDW("atci_serial_port_data_callback, btspp disconnection event (atci status:%d)\r\n",1, g_atci_local_startup_flag);

            if (g_atci_local_startup_flag == 1) {
                atci_deinit((hal_uart_port_t)g_atci_port_handle);
            } else if(g_atci_local_startup_flag == 2) {
                g_atci_local_startup_flag = 1;
                g_serial_port = g_switched_serial_port;
            }
            break;
        }
#endif
        case SERIAL_PORT_EVENT_USB_CONNECTION: {
            LOGMSGIDI(ATCI_SERIAL_PORT_DATA_CALLBACK", usb connection event\r\n", 0);

            if(g_atci_local_startup_flag == 2) {
                // close serial port
                serial_port_close(g_atci_port_handle);
                g_atci_port_handle = switched_serial_port_handle;
                g_atci_local_startup_flag = 1;
                g_serial_port = g_switched_serial_port;
                LOGMSGIDI(ATCI_SERIAL_PORT_DATA_CALLBACK", switch to usb success\r\n", 0);
                return;
            } else if(g_atci_local_startup_flag == 3) {
                atci_init_int();
            }

            break;
        }
        case SERIAL_PORT_EVENT_USB_DISCONNECTION: {
            LOGMSGIDI(ATCI_SERIAL_PORT_DATA_CALLBACK", usb disconnection event\r\n", 0);
            //atci_deinit_ext();
	    g_atci_local_startup_flag = 3;
            break;
        }

        default:
            break;
    }

}

atci_status_t atci_serial_port_init(serial_port_dev_t port)
{
    atci_status_t ret = ATCI_STATUS_OK;
    serial_port_status_t status;

    serial_port_open_para_t serial_port_open_para;

    LOGMSGIDI(ATCI_SERIAL_PORT_INIT", enter,select port:%d \r\n",1, port);

    if (g_atci_local_startup_flag == 1 || g_atci_local_startup_flag == 2) {
        LOGMSGIDW(ATCI_SERIAL_PORT_INIT",already inited \r\n", 0);
        return ATCI_STATUS_OK;
    }

    g_serial_port = port;
    g_atci_uart_port = (hal_uart_port_t)port;

    // register the serial user event callback
    serial_port_open_para.callback = atci_serial_port_data_callback;
    #ifdef MTK_PORT_SERVICE_SLIM_ENABLE
    serial_port_open_para.tx_buffer_size = ATCI_UART_RX_FIFO_BUFFER_SIZE;
    serial_port_open_para.rx_buffer_size = ATCI_TX_BUFFER_SIZE;
    #endif
    // for serial port configuration
    status = serial_port_open(port, &serial_port_open_para, &g_atci_port_handle);
    LOGMSGIDW(ATCI_SERIAL_PORT_INIT",serial_port_open ret=%d \r\n",1, status);

    switch (status) {
        case SERIAL_PORT_STATUS_DEV_NOT_READY:
            /* for usb */
            break;

        case SERIAL_PORT_STATUS_OK:
            /* port service ready */
            ret = atci_init_int();
            break;

//       case SERIAL_PORT_STATUS_PORT_PENDING: /* for bt spp */
        /* wait for bp-spp connection ready before using it */
//           break;

        case SERIAL_PORT_STATUS_BUSY:
        case SERIAL_PORT_STATUS_UNINITIALIZED:
        case SERIAL_PORT_STATUS_UNSUPPORTED:
        case SERIAL_PORT_STATUS_INVALID_DEVICE:
        case SERIAL_PORT_STATUS_INVALID_PARAMETER:
        case SERIAL_PORT_STATUS_FAIL:
        default :
            /* not recoverable */
            ret = ATCI_STATUS_ERROR;
            break;
    }

    return ret;
}

static atci_status_t atci_serial_port_deinit(serial_port_handle_t handle)
{
    atci_status_t ret = ATCI_STATUS_ERROR;
    serial_port_status_t status;

    status = serial_port_close(handle);
    if (status != SERIAL_PORT_STATUS_OK) {
        LOGMSGIDE(ATCI_SERIAL_PORT_DEINIT", fail \r\n", 0);
        ret = ATCI_STATUS_ERROR;
    } else {
        ret = ATCI_STATUS_OK;
        LOGMSGIDI(ATCI_SERIAL_PORT_DEINIT",success \r\n", 0);
    }

    return ret;
}

uint32_t atci_serial_port_read_data(serial_port_handle_t handle, uint8_t *buf, uint32_t buf_len)
{
    serial_port_read_data_t read_data;
    serial_port_status_t status = SERIAL_PORT_STATUS_FAIL;
    uint32_t data_len = 0;

    read_data.buffer = (uint8_t *)buf;
    read_data.size = buf_len;

    status = serial_port_control(g_atci_port_handle, SERIAL_PORT_CMD_READ_DATA, (serial_port_ctrl_para_t *)&read_data);
    LOGMSGIDI(ATCI_SERIAL_PORT_READ_DATA",serial_port_control read ,status=%d\r\n",1,status);
    if (status != SERIAL_PORT_STATUS_OK) {
        LOGMSGIDE(ATCI_SERIAL_PORT_READ_DATA", serial_port_control read fail \r\n", 0);
        return data_len;
    }

    data_len = read_data.ret_size;

    return data_len;
}

uint32_t atci_serial_port_send_data(uint32_t port, uint8_t *buf, uint32_t buf_len)
{
    serial_port_write_data_t send_data;
    uint32_t data_len = 0;
    serial_port_status_t status = SERIAL_PORT_STATUS_OK;

    if(port == 0) {
        return data_len;
    }

    send_data.data = buf;
    send_data.size = buf_len;

    while(send_data.size > 0) {
        status = serial_port_control(port, SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t *)&send_data);
        data_len += send_data.ret_size;
        send_data.size -= send_data.ret_size;
        send_data.data += send_data.ret_size;

        if (status != SERIAL_PORT_STATUS_OK) {
            LOGMSGIDE(ATCI_SERIAL_PORT_SEND_DATA",fail\r\n", 0);
            return data_len;
        }
    }

    return data_len;

}

void atci_serial_port_switch(serial_port_dev_t device)
{
    LOGMSGIDI(ATCI_SERIAL_PORT_SWITCH",enter:%d\r\n",1, device);

    serial_port_status_t status;
    serial_port_open_para_t serial_port_open_para;

    if(device == g_serial_port) {
        LOGMSGIDW(ATCI_SERIAL_PORT_SWITCH", switch port == current port \r\n", 0);
        return;
    }

    g_switched_serial_port = device;

    g_atci_local_startup_flag = 2;

    atci_data_processing_for_deinit();

    // register the serial user event callback
    serial_port_open_para.callback = atci_serial_port_data_callback;
    #ifdef MTK_PORT_SERVICE_SLIM_ENABLE
    serial_port_open_para.tx_buffer_size = ATCI_UART_RX_FIFO_BUFFER_SIZE;
    serial_port_open_para.rx_buffer_size = ATCI_TX_BUFFER_SIZE;
    #endif
    // for serial port configuration
    status = serial_port_open(device, &serial_port_open_para, &switched_serial_port_handle);
    LOGMSGIDI(ATCI_SERIAL_PORT_SWITCH", serial_port_open ret=%d \r\n",1, status);

    switch (status) {
        case SERIAL_PORT_STATUS_DEV_NOT_READY:
            break;

        case SERIAL_PORT_STATUS_OK:
            // close serial port
            status = serial_port_close(g_atci_port_handle);
            g_atci_port_handle = switched_serial_port_handle;
#if 0
            if((g_serial_port >= SERIAL_PORT_DEV_BTSPP_SERVER1) && (g_serial_port <= SERIAL_PORT_DEV_BTSPP_CLIENT2)) {
                g_atci_local_startup_flag = 2;
            } else
#endif
            {
                g_serial_port = g_switched_serial_port;
                g_atci_local_startup_flag = 1;
            }
            LOGMSGIDI(ATCI_SERIAL_PORT_SWITCH",success atci_status:%d \r\n",1, g_atci_local_startup_flag);
            break;

//       case SERIAL_PORT_STATUS_PORT_PENDING: /* for bt spp */
        /* wait for bp-spp connection ready before using it */
        //          break;

        case SERIAL_PORT_STATUS_BUSY:
        case SERIAL_PORT_STATUS_UNINITIALIZED:
        case SERIAL_PORT_STATUS_UNSUPPORTED:
        case SERIAL_PORT_STATUS_INVALID_DEVICE:
        case SERIAL_PORT_STATUS_INVALID_PARAMETER:
        case SERIAL_PORT_STATUS_FAIL:
        default :
            /* not recoverable */
            g_atci_local_startup_flag = 1;
            LOGMSGIDW(ATCI_SERIAL_PORT_SWITCH",fail because the port is busy \r\n", 0);
            break;
    }

    LOGMSGIDI("atci_serial_port_switch end\r\n", 0);
}

atci_status_t atci_init_ext(serial_port_dev_t port)
{
    atci_status_t ret = ATCI_STATUS_ERROR;
    ret = atci_serial_port_init(port);
    return ret;
}

#else
#ifndef MTK_ATCI_VIA_MUX
void atci_uart_irq(hal_uart_callback_event_t event, void *parameter)
{
    atci_general_msg_t msg_queue_item;

    //LOGW("atci_uart_irq, hal_uart_cb_event: %d \r\n", event);

    if (g_atci_port_handle >= HAL_UART_MAX) {

        LOGMSGIDW(ATCI_UART_IRQ", invaild uart port:%d\r\n",1, g_atci_port_handle);
        return;
    }

    if (HAL_UART_EVENT_READY_TO_READ == event) {
        LOGMSGIDI(ATCI_UART_IRQ", read event\r\n", 0);
        if (atci_check_startup_finish()) {
            msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_SEND_CMD_IND;
            msg_queue_item.msg_data = NULL;
            atci_queue_send(g_atci_input_command_queue, (void *)&msg_queue_item);
            atci_semaphore_give(g_atci_handler_semaphore);
        }
    } else if (HAL_UART_EVENT_READY_TO_WRITE == event) {
        if (atci_check_startup_finish() && g_send_data_cache_left_len) {
            LOGMSGIDI(ATCI_UART_IRQ"write event\r\n", 0);
            msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_WRITE_CMD_IND;

            atci_queue_send(g_atci_data_cached_queue, (void *)&msg_queue_item);
            atci_semaphore_give(g_atci_handler_semaphore);
        }
    }
    #ifdef HAL_SLEEP_MANAGER_ENABLED
    else if(HAL_UART_EVENT_WAKEUP_SLEEP == event)
    {
        LOGMSGIDI("wakeup sleep event!\r\n", 0);
        msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_WAKEUP_SLEEP_EVENT_NOTIFY_IND;
        atci_queue_send(g_atci_data_cached_queue, (void *)&msg_queue_item);
        atci_semaphore_give(g_atci_handler_semaphore);
    }
    #endif
}

atci_status_t atci_uart_init(hal_uart_port_t port)
{
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;
    atci_status_t ret = ATCI_STATUS_ERROR;

    LOGMSGIDI(ATCI_UART_INIT",port=%d \r\n",1, port);
    if(port >= HAL_UART_MAX) {
        return ATCI_STATUS_ERROR;
    }

    /* Configure UART PORT */
    uart_config.baudrate = HAL_UART_BAUDRATE_115200;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;

    g_atci_port_handle = port;
    g_atci_uart_port = port;

    if (HAL_UART_STATUS_OK != hal_uart_init(port, &uart_config)) {
        // log: uart init fail
        LOGMSGIDE(ATCI_UART_INIT", fail\r\n", 0);
        ret = ATCI_STATUS_ERROR;
    } else {
        // log: uart init success
        LOGMSGIDI(ATCI_UART_INIT", success \n", 0);
        ret = ATCI_STATUS_OK;
        dma_config.receive_vfifo_alert_size = ATCI_UART_RX_FIFO_ALERT_SIZE;
        dma_config.receive_vfifo_buffer = (uint8_t *)g_atci_uart_cntx.uart_rx_buff;
        dma_config.receive_vfifo_buffer_size = ATCI_UART_RX_FIFO_BUFFER_SIZE;
        dma_config.receive_vfifo_threshold_size = ATCI_UART_RX_FIFO_THRESHOLD_SIZE;
        dma_config.send_vfifo_buffer = (uint8_t *)g_atci_uart_cntx.uart_tx_buff;
        dma_config.send_vfifo_buffer_size = ATCI_UART_TX_FIFO_BUFFER_SIZE;
        dma_config.send_vfifo_threshold_size = ATCI_UART_TX_FIFO_THRESHOLD_SIZE;

        hal_uart_set_dma(port, &dma_config);
        hal_uart_register_callback(port, atci_uart_irq, NULL);
    }

    return ret;
}


static atci_status_t atci_uart_deinit(hal_uart_port_t port)
{
    hal_uart_status_t uart_ret = HAL_UART_STATUS_ERROR;
    atci_status_t ret = ATCI_STATUS_ERROR;

    uart_ret = hal_uart_deinit(port);

    LOGMSGIDI("atci_uart_deinit() uart_ret = %d \r\n",1, uart_ret);

    if (uart_ret == HAL_UART_STATUS_OK) {

        ret = ATCI_STATUS_OK;
    }

    return ret;
}


uint32_t atci_uart_read_data(hal_uart_port_t uart_index, uint8_t *buf, uint32_t buf_len)
{
#define EARLY_ACCESS_7687EVB_TEST
#ifdef EARLY_ACCESS_7687EVB_TEST
    uint32_t available_data_len = 0;
    available_data_len = hal_uart_get_available_receive_bytes(uart_index);
    if (available_data_len > buf_len) {
        available_data_len = buf_len;
    }
    hal_uart_receive_dma(uart_index, (uint8_t *)buf, available_data_len);
    return available_data_len;
#else
    return hal_uart_receive_dma(uart_index, (uint8_t *)buf, (uint32_t) buf_len);
#endif
}
#endif

uint32_t atci_uart_send_data(hal_uart_port_t port, uint8_t *buf, uint32_t buf_len)
{
    uint32_t ret_len = 0;

    ret_len = hal_uart_send_dma(port, buf, buf_len);
    return ret_len;
}

#endif

uint8_t atci_check_startup_finish(void)
{
    uint8_t is_finish = 0;
    if(g_atci_local_startup_flag == 1) {
        is_finish = 1;
    }
    return is_finish;
}


/* ATCI main body related */
atci_status_t atci_local_init(void)
{
    atci_status_t ret =  ATCI_STATUS_ERROR;
    /* Init Mutex */

    LOGMSGIDI(ATCI_LOCAL_INIT", enter flag:%d \r\n",1, g_atci_local_startup_flag);

    if (g_atci_local_startup_flag != 0) {

        return ATCI_STATUS_OK;
    }

    if (NULL == (void *)g_atcmd_table_semaphore) {
        g_atcmd_table_semaphore = (uint32_t)xSemaphoreCreateBinary();
        LOGMSGIDI(ATCI_LOCAL_INIT", init smp:0x%08x ",1, g_atcmd_table_semaphore);
    	if (NULL == (void *)g_atcmd_table_semaphore) {
    		return ret;
    	}
    }

    if (NULL == (void *)g_atci_handler_semaphore) {

        g_atci_handler_semaphore = atci_semaphore_create(10, 0);
        if (NULL == (void *)g_atci_handler_semaphore) {
            return ret;
        }
    }

    /*    g_atci_registered_table_number = 0;
        for ( i = 0; i < ATCI_MAX_GNENERAL_TABLE_NUM; i++) {
            g_atci_cm4_general_hdlr_tables[i].item_table = NULL;
            g_atci_cm4_general_hdlr_tables[i].item_table_size = 0;
        }
    */
    /* Init Queue */
    if (NULL == (void *)g_atci_input_command_queue) {
        g_atci_input_command_queue = atci_queue_create(ATCI_LOCAL_QUEUE_LENGTH,sizeof(atci_general_msg_t));
        if (NULL == (void *)g_atci_input_command_queue) {
            return ret;
        }
    }

    if (NULL == (void *)g_atci_resonse_command_queue) {
        g_atci_resonse_command_queue = atci_queue_create(ATCI_LOCAL_QUEUE_LENGTH,sizeof(atci_general_msg_t));
        if (NULL == (void *)g_atci_resonse_command_queue) {
            return ret;
        }
    }

    if (NULL == (void *)g_atci_data_cached_queue) {
        g_atci_data_cached_queue = atci_queue_create(ATCI_LOCAL_QUEUE_LENGTH,sizeof(atci_general_msg_t));
        if (NULL == (void *)g_atci_data_cached_queue) {
            return ret;
        }
    }

    atci_input_commad_in_processing = 0;

    g_atci_local_startup_flag = 3;

    if ( NULL != (void *)g_atci_processing_semaphore ) {
        atci_semaphore_give(g_atci_processing_semaphore);
    }

    ret =  ATCI_STATUS_OK;
    LOGMSGIDI(ATCI_LOCAL_INIT", success \r\n", 0);
    return ret;
}

atci_status_t atci_init_int(void)
{
    atci_status_t ret = ATCI_STATUS_OK;

    LOGMSGIDI("atci_init_int, enter\r\n", 0);

    //  ret = atci_local_init();

    // if (ret != ATCI_STATUS_OK) {
    //     ret = atci_port_deinit(g_atci_port_handle);
    //     return ret;
    //  }

    if (at_command_init() == ATCI_STATUS_OK) {

        LOGMSGIDI(ATCI_INTT_INT", success \r\n", 0);
    } else {

        ret = ATCI_STATUS_ERROR;
        LOGMSGIDE(ATCI_INTT_INT", fail \r\n", 0);
    }
    g_atci_local_startup_flag = 1;
    return ret;
}

#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
atci_mux_t g_atci_mux[ATCI_MAX_MUX_PORT];
int g_atci_mux_index_inuse = 0xFF;

void mux_atci_callback(mux_handle_t handle, mux_event_t event,uint32_t data_len,void*user_data);

static bool atci_mux_find_index_by_handle(mux_handle_t handle, int *index_p)
{
    int i = 0;

    for (i = 0; i < ATCI_MAX_MUX_PORT; i ++) {
        if (g_atci_mux[i].handle == handle) {
            if (g_atci_mux[i].type == ATCI_MUX_UNUSE){
                LOGMSGIDE("atci_mux_find_index_by_handle, unuse index %d\r\n", 1, i);
                return false;
            }
            *index_p = i;
            LOGMSGIDI("atci_mux_find_index_by_handle handle %d index %d\r\n", 2, handle, i);
            return true;
        }
    }

    LOGMSGIDE("atci_mux_find_index_by_handle, not found %d\r\n", 1, handle);
    return false;
}

static void race_tx_no_packed_callback(mux_handle_t handle, const mux_buffer_t payload[], uint32_t buffers_counter, mux_buffer_t *head, mux_buffer_t *tail, void *user_data)
{
    head->p_buf = NULL;
    tail->p_buf = NULL;
    head->buf_size = 0;
    tail->buf_size = 0;
}

static void race_rx_no_packed_callback(mux_handle_t *handle, mux_buffer_t buffers[], uint32_t buffers_counter, uint32_t *consume_len, uint32_t *package_len, void *user_data)
{
    uint32_t total_size, i;
    mux_handle_t p_handle;

    *package_len = 0;
    *consume_len = 0;
    total_size = 0;

    for (i = 0; i < buffers_counter; i++) {
        total_size += buffers[i].buf_size;
    }

    *package_len = total_size;
    if(mux_query_user_handle(g_atci_mux[1].port, "ATCI", &p_handle) == MUX_STATUS_OK){
        /* race command */
        *handle = p_handle;
        *consume_len = 0;
    } else {
        *handle = (mux_handle_t)NULL;
        *consume_len = total_size;
        return;
    }
}

static mux_status_t atci_mux_init_mono_port(mux_port_t port)
{
    mux_status_t status = MUX_STATUS_ERROR;
    mux_port_setting_t atci_setting;
    mux_protocol_t protocol_callback = {0};

    /* default atci mono port setting */
    #ifdef AG3335
            atci_setting.tx_buffer_size = 1024;
            atci_setting.rx_buffer_size = 1024;
    #else
            atci_setting.tx_buffer_size = 256;
            atci_setting.rx_buffer_size = 256;
    #endif

    atci_setting.dev_setting.uart.uart_config.baudrate    = CONFIG_ATCI_BAUDRATE;
    atci_setting.dev_setting.uart.uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    atci_setting.dev_setting.uart.uart_config.stop_bit    = HAL_UART_STOP_BIT_1;
    atci_setting.dev_setting.uart.uart_config.parity      = HAL_UART_PARITY_NONE;
    atci_setting.dev_setting.uart.flowcontrol_type        = MUX_UART_NONE_FLOWCONTROL;

    protocol_callback.rx_protocol_callback = race_rx_no_packed_callback;
    protocol_callback.tx_protocol_callback = race_tx_no_packed_callback;
    protocol_callback.user_data = NULL;

    g_atci_mux[1].port = port;
    status = mux_query_port_setting_from_nvdm(g_atci_mux[1].port, &atci_setting);
    if (status != MUX_STATUS_OK) {
        mux_save_port_setting_to_nvdm(g_atci_mux[1].port, &atci_setting);
    }

    if ((g_atci_mux[1].port >= MUX_USB_BEGIN) && (g_atci_mux[1].port <= MUX_USB_END)) {
        g_atci_mux[1].type = ATCI_MUX_MONO_USB;
    } else {
        g_atci_mux[1].type = ATCI_MUX_MONO_UART;
    }

    status = mux_init(g_atci_mux[1].port, &atci_setting, &protocol_callback);
    LOGMSGIDI("atci_mux_init init mono mux port %d status %d\r\n", 2, g_atci_mux[1].port, status);
    if (status != MUX_STATUS_OK) {
        return ATCI_STATUS_ERROR;
    }
    mux_open_save_to_nvdm(g_atci_mux[1].port, "ATCI");
    status = mux_open(g_atci_mux[1].port, "ATCI", (mux_handle_t*)&(g_atci_mux[1].handle), mux_atci_callback, NULL);
    LOGMSGIDI("atci_mux_init open mono mux port %d status %d\r\n", 2, g_atci_mux[1].port, status);

    if (status == MUX_STATUS_OK) {
        g_atci_mux_index_inuse = 1;
    }

    return status;
}

#ifdef MTK_USB_DEMO_ENABLED
void atci_mux_usb_callback(void *param)
{
    bool plug_in = (bool)param;
    atci_general_msg_t msg_queue_item;

    if (plug_in) {
        msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_USB_PLUG_IN;
        atci_queue_send(g_atci_data_cached_queue, (void *)&msg_queue_item);
        atci_semaphore_give(g_atci_handler_semaphore);
    } else {
        msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_USB_PLUG_OUT;
        atci_queue_send(g_atci_data_cached_queue, (void *)&msg_queue_item);
        atci_semaphore_give(g_atci_handler_semaphore);
    }
}

void atci_mux_usb_plug(bool plug_in)
{
    mux_status_t status = MUX_STATUS_ERROR;

    if (plug_in) {
        LOGMSGIDI("atci_mux_usb_plug plug in\r\n", 0);
        status = atci_mux_init_mono_port(g_atci_mux[1].port);
    } else {
        LOGMSGIDI("atci_mux_usb_plug plug out\r\n", 0);
        status = mux_close(g_atci_mux[1].handle);
        LOGMSGIDI("atci_mux_usb_plug mux_close stat %d\r\n", 1, status);
        if (status == MUX_STATUS_OK) {
            status = mux_deinit(g_atci_mux[1].port);
            LOGMSGIDI("atci_mux_usb_plug mux_deinit stat %d\r\n", 1, status);
            g_atci_mux_index_inuse = 0;
        }
    }
}
#endif

/* Currently only support 1-wire share port. */
atci_status_t atci_mux_port_reinit(mux_port_t port, bool is_share, bool is_1wire)
{
    mux_status_t status = MUX_STATUS_ERROR;
    LOGMSGIDI("atci_mux_share_reinit port %d, now share port %d", 2, port, g_atci_mux[0].port);

    if (g_atci_mux[0].type == ATCI_MUX_SHARE) {
        status = mux_close(g_atci_mux[0].handle);
        LOGMSGIDI("atci_mux_share_reinit mux_close stat %d", 1, status);
    }

    g_atci_mux[0].port = port;
    status = mux_open(g_atci_mux[0].port, "ATCI", (mux_handle_t*)&(g_atci_mux[0].handle), mux_atci_callback, NULL);
    LOGMSGIDI("atci_mux_share_reinit mux_open stat %d", 1, status);
    if (is_share && is_1wire) {
        g_atci_mux[0].type = ATCI_MUX_SHARE_1WIRE;
    } else if (is_share) {
        g_atci_mux[0].type = ATCI_MUX_SHARE;
    }
    g_atci_mux_index_inuse = 0;
    return ATCI_STATUS_OK;
}


atci_status_t atci_mux_port_resume(bool is_share, bool is_1wire)
{
    mux_status_t status = MUX_STATUS_ERROR;
    LOGMSGIDI("atci_mux_share_resume now share port %d", 1, g_atci_mux[0].port);

    if (g_atci_mux[0].type != ATCI_MUX_UNUSE) {
        status = mux_close(g_atci_mux[0].handle);
        LOGMSGIDI("atci_mux_share_resume mux_close stat %d", 1, status);
        g_atci_mux[0].type = ATCI_MUX_UNUSE;
        g_atci_mux[0].handle = 0;
        g_atci_mux_index_inuse = 1;
    }

    mux_port_buffer_t query_port_buffer;
    query_port_buffer.count = 0;
    status = mux_query_port_numbers_from_nvdm("RACE_CMD", (mux_port_buffer_t*)&query_port_buffer);
    if ((query_port_buffer.count == 1) && (status == MUX_STATUS_OK)) {
        if(query_port_buffer.buf[0] != MUX_AIRAPP_0) { /* syslog via bt, not for atci*/
            g_atci_mux[0].port = query_port_buffer.buf[0];
            status = mux_open(g_atci_mux[0].port, "ATCI", (mux_handle_t*)&(g_atci_mux[0].handle), mux_atci_callback, NULL);
            LOGMSGIDI("atci_mux_share_resume open share mux port %d status %d\r\n", 2, g_atci_mux[0].port, status);
            g_atci_mux[0].type = ATCI_MUX_SHARE;
            g_atci_mux_index_inuse = 0;
        } else {
            LOGMSGIDI("atci_mux_share_resume syslog port MUX_AIRAPP_0\r\n", 0);
        }
    } else {
        LOGMSGIDI("atci_mux_share_resume query syslog port fail\r\n", 0);
    }

    return ATCI_STATUS_OK;
}

static atci_status_t atci_mux_init(mux_port_t port)
{
    mux_status_t mux_status = MUX_STATUS_ERROR;
    mux_status_t mono_status = MUX_STATUS_ERROR;
    mux_port_t atci_default_port;
    mux_port_buffer_t query_port_buffer;

    memset(&g_atci_mux, 0, sizeof(atci_mux_t) * ATCI_MAX_MUX_PORT);

    /* Share with syslog port */
    query_port_buffer.count = 0;

#ifndef AG3335
#ifdef MTK_USB_AUDIO_HID_ENABLE
    mux_status = mux_query_port_numbers_from_nvdm("SYSLOG", (mux_port_buffer_t*)&query_port_buffer);
#else
    mux_status = mux_query_port_numbers_from_nvdm("RACE_CMD", (mux_port_buffer_t*)&query_port_buffer);
#endif
#else
    mux_status = mux_query_port_numbers_from_nvdm("SYSLOG", (mux_port_buffer_t*)&query_port_buffer);
#endif

    if ((query_port_buffer.count == 1) && (mux_status == MUX_STATUS_OK)) {
        if(query_port_buffer.buf[0] != MUX_AIRAPP_0) { /* syslog via bt, not for atci*/
            g_atci_mux[0].port = query_port_buffer.buf[0];
            mux_status = mux_open(g_atci_mux[0].port, "ATCI", (mux_handle_t*)&(g_atci_mux[0].handle), mux_atci_callback, NULL);
            LOGMSGIDI("atci_mux_init open share mux port %d status %d\r\n", 2, g_atci_mux[0].port, mux_status);
            g_atci_mux[0].type = ATCI_MUX_SHARE;
            g_atci_mux_index_inuse = 0;
        } else {
            LOGMSGIDI("atci_mux_init syslog port MUX_AIRAPP_0\r\n", 0);
        }
    } else {
        LOGMSGIDI("atci_mux_init query syslog port fail\r\n", 0);
    }

    /* load atci default port */
    query_port_buffer.count = 0;
    mux_status = mux_query_port_numbers_from_nvdm("ATCI", (mux_port_buffer_t*)&query_port_buffer);
    if ((mux_status == MUX_STATUS_OK) && (query_port_buffer.count == 1)) {
        atci_default_port = query_port_buffer.buf[0];
    } else {
        atci_default_port = CONFIG_ATCI_PORT;
    }

    /* atci through mono port */
    if(g_atci_mux[0].port != atci_default_port) {
        mono_status = atci_mux_init_mono_port(atci_default_port);
        if (g_atci_mux[1].type == ATCI_MUX_MONO_USB) {
#ifdef MTK_USB_DEMO_ENABLED
            LOGMSGIDI("atci_mux_init register usb callback\r\n", 0);
            usb_case_register_atci_callback(atci_mux_usb_callback);
#endif
        }
    } else {
        /* Only a share port for atci */
        LOGMSGIDI("atci_mux_init syslog port == CONFIG_ATCI_PORT\r\n", 0);
    }

    if ((mux_status == MUX_STATUS_OK) || (mono_status == MUX_STATUS_OK)) {
        LOGMSGIDI("atci_mux_init mux_open success \r\n", 0);
        atci_init_int();
    } else {
        assert(0);
    }

    return ATCI_STATUS_OK;
}

#endif

atci_status_t atci_init(uint16_t port)
{
    atci_status_t ret = ATCI_STATUS_ERROR;

    ret = atci_local_init();

    if (ret != ATCI_STATUS_OK) {
        // ret = atci_port_deinit(g_atci_port_handle);
        return ret;
    }
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
    ret = atci_mux_init((mux_port_t)port);
#elif defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    ret = atci_serial_port_init((serial_port_dev_t)port);
#else
    ret = atci_uart_init((hal_uart_port_t)port);
    if(ret == ATCI_STATUS_OK) {
        ret = atci_init_int();
    }
#endif

    return ret;
}

uint32_t atci_port_send_data(uint32_t port, uint8_t *buf, uint32_t buf_len)
{
    uint32_t data_len = 0;

#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
{
    mux_buffer_t pdata;
    mux_status_t result;
    uint32_t send_len;
    mux_port_t port_in_use;
    mux_ctrl_para_t mux_para;

    if (g_atci_mux_index_inuse < ATCI_MAX_MUX_PORT) {
        if (g_atci_mux[g_atci_mux_index_inuse].type == ATCI_MUX_UNUSE) {
            LOGMSGIDW("atci_port_send_data, g_atci_mux_index_inuse %d is UNUSE\r\n", 1, g_atci_mux_index_inuse);
            return 0;
        } else {
            port_in_use = g_atci_mux[g_atci_mux_index_inuse].port;
        }
    } else {
        LOGMSGIDE("atci_port_send_data, g_atci_mux_index_inuse error:%d\r\n", 1, g_atci_mux_index_inuse);
        return 0;
    }

    pdata.p_buf = buf;
    while (data_len < buf_len) {
        result = mux_control(port_in_use, MUX_CMD_GET_TX_AVAIL, &mux_para);
        if(MUX_STATUS_OK != result) {
            LOGMSGIDE("atci_port_send_data,mux_control fail:%d\r\n", 1, result);
            return data_len;
        }
        if (buf_len - data_len > mux_para.mux_get_tx_avail.ret_size) {
            pdata.buf_size = mux_para.mux_get_tx_avail.ret_size;
        } else {
            pdata.buf_size = buf_len - data_len;
        }
#ifdef MTK_SMART_CHARGER_1WIRE_RACE_ENABLE
        if (g_atci_mux[g_atci_mux_index_inuse].type == ATCI_MUX_SHARE_1WIRE) {
            LOGMSGIDI("atci_port_send_data, set pinmux for 1-wire tx", 0);
            hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_UART1_TXD);
            hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_GPIO5);
        }
#endif
        result = mux_tx(g_atci_mux[g_atci_mux_index_inuse].handle, &pdata,1, &send_len);
        if(MUX_STATUS_OK != result) {
            LOGMSGIDE("atci_port_send_data,fail:%d\r\n", 1, result);
            return data_len;
        }
        data_len += send_len;
        pdata.p_buf = buf + data_len;
    }
    return data_len;
}

#elif defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    data_len = atci_serial_port_send_data(port, buf, buf_len);
#else
    data_len = atci_uart_send_data((hal_uart_port_t)port, buf, buf_len);
#endif

    return data_len;
}

uint32_t atci_port_read_data(uint32_t port, uint8_t *buf, uint32_t buf_len)
{
    uint32_t data_len = 0;
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
    mux_buffer_t buffer;
    mux_status_t result;

    buffer.p_buf = buf;
    buffer.buf_size = buf_len;
    
    result = mux_rx(port, &buffer, &data_len);
    //data_len = mux_rx(MUX_USER_ATCI, buf, buf_len);
    if (MUX_STATUS_OK != result) {
        LOGMSGIDI("atci_port_read_data done, ret %d", 1, result);
        return 0;
    }

#elif defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    data_len = atci_serial_port_read_data(port, buf, buf_len);
#else
    data_len = atci_uart_read_data((hal_uart_port_t)port, buf, buf_len);
#endif

    return data_len;
}

atci_status_t atci_send_data_int(uint8_t *data, uint32_t data_len)
{
    atci_status_t ret = ATCI_STATUS_OK;
    uint32_t sent_len = 0;

    if(atci_vaild_port_check(g_atci_port_handle) == ATCI_STATUS_ERROR) {
        return ATCI_STATUS_ERROR;
    }

    sent_len = atci_port_send_data(g_atci_port_handle, data, data_len);

    if (sent_len == 0) {
        ret = ATCI_STATUS_ERROR;
        LOGMSGIDE(ATCI_SEND_DATA_INT",fail\r\n", 0);
    }

    LOGMSGIDI(ATCI_SEND_DATA_INT", send data len:%x\r\n",1, sent_len);
    return ret;
}

atci_status_t atci_send_data(uint8_t *data, uint32_t data_len)
{
    uint8_t *send_data = NULL;
    uint32_t send_len = 0;
    uint32_t sent_len = 0;
    atci_status_t ret = ATCI_STATUS_OK;

    LOGMSGIDI(ATCI_SEND_DATA", enter, cache_left_len:%x \r\n",1, g_send_data_cache_left_len);
#ifndef MTK_MUX_ENABLE
    if(atci_vaild_port_check(g_atci_port_handle) == ATCI_STATUS_ERROR) {
        LOGMSGIDE(ATCI_SEND_DATA", fail,because g_atci_port_handle is invalid:%d \r\n",1, g_atci_port_handle);
        return ATCI_STATUS_ERROR;
    }
#endif
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
    /*If there's data cache(that must be cache for mono port), but now data sending to share port, just send data now. */
    if(g_send_data_cache_left_len == 0 || (data && g_atci_mux_index_inuse == 0)) {
        LOGMSGIDI(ATCI_SEND_DATA", g_atci_mux_index_inuse is 0 and data not null", 0);
#else
    if (g_send_data_cache_left_len == 0) {
#endif
        send_data = data;
        send_len = data_len;
    } else if ((data == NULL) && (g_send_data_cache_left_len > 0)) {

        if(g_send_data_cache != NULL) {
            send_data = &g_send_data_cache[g_send_data_cache_len - g_send_data_cache_left_len];
            send_len = g_send_data_cache_left_len;
        } else {
            return  ATCI_STATUS_ERROR;
        }
    } else {
        return	ATCI_STATUS_ERROR;
    }

    // send data by port
    sent_len = atci_port_send_data(g_atci_port_handle, send_data, send_len);
    if (sent_len == 0) {
        ret = ATCI_STATUS_ERROR;
        LOGMSGIDE(ATCI_SEND_DATA", fail\r\n", 0);
    }

    LOGMSGIDI(ATCI_SEND_DATA", sending data(len:%x), sent data(len:%x)\r\n",2, send_len, sent_len);

#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
    /* For share port(log port), there'll be no ready_to_write event, so no chance to retry, no need cache. */
    if(sent_len >= send_len || g_atci_mux_index_inuse == 0) {
        LOGMSGIDI(ATCI_SEND_DATA", g_atci_mux_index_inuse is 0", 0);
#else
    if(sent_len >= send_len) {
#endif
        // send all the input data
        if(g_send_data_cache != NULL) {
            atci_mem_free(g_send_data_cache);
            g_send_data_cache = NULL;
            LOGMSGIDI(ATCI_SEND_DATA", atci_mem_free\r\n", 0);
        }

        g_send_data_cache_len = 0;
        g_send_data_cache_left_len = 0;

        if(atci_input_commad_in_processing == 3) {
            atci_input_commad_in_processing = 0;
            LOGMSGIDI(ATCI_SEND_DATA", send AT cmd response done\r\n", 0);
        } else {
            //LOGMSGIDI(ATCI_SEND_DATA", send data done\r\n", 0);
        }

        return ATCI_STATUS_OK;
    } else {
        // cache the left sending data
        if(g_send_data_cache_len == 0) {

            g_send_data_cache_len = send_len - sent_len;

            if(g_send_data_cache == NULL) {
                g_send_data_cache = (uint8_t *)atci_mem_alloc(g_send_data_cache_len);
                if(g_send_data_cache == NULL) {
                    LOGMSGIDW(ATCI_SEND_DATA", alloc memory fail, drop the cached data(data_len:%x)\r\n",1, g_send_data_cache_left_len);
                    g_send_data_cache_len = 0;
                    g_send_data_cache_left_len = 0;
                    return ATCI_STATUS_ERROR;
                }
            }

            memset(g_send_data_cache, 0, g_send_data_cache_len);
            memcpy(g_send_data_cache, &send_data[sent_len], g_send_data_cache_len);
        }

        g_send_data_cache_left_len = send_len - sent_len;

        LOGMSGIDI(ATCI_SEND_DATA", cached data(data_len:%d)\r\n",1, g_send_data_cache_left_len);
    }

    return ret;
}

#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
uint32_t g_data_len;

atci_status_t atci_read_mux_data(uint32_t data_len, mux_handle_t handle)
{
    atci_status_t ret = ATCI_STATUS_OK;
    //atci_send_input_cmd_msg_t input_data;
    //atci_response_t   response_1 = {{0}};
    uint32_t  length = 0;
    uint32_t find_valid_command = 0;
    uint32_t  read_data_len = 0;

    LOGMSGIDI("atci_read_mux_data, enter,g_atci_port_handle:%x, data_len = %d, g_data_len = %d\r\n", 3, g_atci_port_handle, data_len, g_data_len);

    if (data_len >= ATCI_UART_RX_FIFO_BUFFER_SIZE) {
        assert(0);
    }

#ifndef MTK_MUX_ENABLE
    if(atci_vaild_port_check(handle) == ATCI_STATUS_ERROR) {
        return ATCI_STATUS_ERROR;
    }
#endif

    //for slim
    if(g_input_command == NULL) {
        g_input_command = (atci_send_input_cmd_msg_t *)atci_mem_alloc(sizeof(atci_send_input_cmd_msg_t));
        LOGMSGIDW("atci_read_mux_dataalloc mem to get data from port\r\n", 0);
        if(g_input_command == NULL) {
            LOGMSGIDW("atci_read_mux_data alloc mem fail\r\n", 0);
            return ATCI_STATUS_ERROR;
        }
        memset(g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));
    }

    // read data from port
    do {
        read_data_len = atci_port_read_data(handle, (uint8_t *)(&g_input_command->input_buf[g_input_command->input_len]),
                ATCI_UART_RX_FIFO_BUFFER_SIZE - g_input_command->input_len);

        if (read_data_len > 0) {
            uint32_t flags;
            hal_nvic_save_and_set_interrupt_mask(&flags);
            g_data_len -= read_data_len;
            hal_nvic_restore_interrupt_mask(flags);
        }

        if((read_data_len > 0) && (atci_input_commad_in_processing == 0)) {
            /* append current receive data to the input buffer*/
            if((read_data_len + g_input_command->input_len) < ATCI_UART_RX_FIFO_BUFFER_SIZE) {
                //memcpy(&g_input_command->input_buf[g_input_command->input_len], (char *)input_data, read_data_len);
                g_input_command->input_len += read_data_len;
                //LOGW("current input cmd (len:%d):%s\r\n", g_input_command->input_len, g_input_command.input_buf);
            } else {
                LOGMSGIDW("atci_read_mux_data atci_port_read_data len is bigger than %d\r\n", 1, ATCI_UART_RX_FIFO_BUFFER_SIZE);
            }
            LOGMSGIDI("atci_read_mux_data atci_port_read_data read_data_len %d, total len %d\r\n", 2, read_data_len, g_input_command->input_len);
        }
    } while(read_data_len);

    // drop data if cmd is not processed
    if (atci_input_commad_in_processing != 0) {
        atci_send_data((uint8_t *)"ERROR\r\n", strlen("ERROR\r\n"));
        LOGMSGIDW("atci_read_mux_data drop this cmd, need wait the response of previous AT cmd \r\n", 0);
        return ATCI_STATUS_ERROR;
    }

    // user should know the read_data size before reading
    //read_data_len = atci_port_read_data(handle, (uint8_t*)(g_input_command->input_buf + g_input_command->input_len), (ATCI_UART_RX_FIFO_BUFFER_SIZE - g_input_command->input_len));
    //g_input_command->input_len += read_data_len;
    if (g_input_command->input_len == 0) {
        LOGMSGIDW("atci_read_mux_data fail \r\n", 0);
        atci_mem_free(g_input_command);
        g_input_command = NULL;
        return ATCI_STATUS_ERROR;
    }

    if ((g_input_command->input_len) < ATCI_UART_RX_FIFO_BUFFER_SIZE) {
        // just use a golbal buffer (g_input_command) to receive data because the mem alloc API can't be used in hal irq
        LOGW("%s current input cmd (len:%d):%s\r\n", ATCI_READ_DATA, g_input_command->input_len, g_input_command->input_buf);
    } else {
        /*invild command, input cmd is larger then max len, need drop this cmd*/
        atci_input_commad_in_processing = 0;
        atci_mem_free(g_input_command);
        g_input_command = NULL;
        atci_send_data((uint8_t *)"ERROR\r\n", strlen("ERROR\r\n"));
        LOGMSGIDW("atci_read_mux_data fail, input AT cmd length is larger than %d \r\n", 1, ATCI_UART_RX_FIFO_BUFFER_SIZE);
        g_data_len = 0;
        return ATCI_STATUS_ERROR;
    }

    /* find '\0'/CR/LF to be the end of AT command */
    for ( length = 0; length < g_input_command->input_len; length++) {
        if (('\0' == g_input_command->input_buf[length]) ||
                (ATCI_CR_C == g_input_command->input_buf[length]) ||
                (ATCI_LF_C == g_input_command->input_buf[length])) {
            find_valid_command = 1;
            break;
        }
    }

    if(find_valid_command == 1) {
        /* find "AT" to be the beginning of AT command */
        if (('A' == g_input_command->input_buf[0] && 'T' == g_input_command->input_buf[1]) ||
                ('a' == g_input_command->input_buf[0] && 't' == g_input_command->input_buf[1])) {

            atci_input_commad_in_processing = 1;
        } else {

            atci_input_commad_in_processing = 0;
            atci_mem_free(g_input_command);
            g_input_command = NULL;
            atci_send_data((uint8_t *)"ERROR\r\n", strlen("ERROR\r\n"));
            LOGMSGIDW("atci_read_mux_data fail, receive data is not AT cmd type, drop this cmd! \r\n", 0);
            return ATCI_STATUS_ERROR;
        }
    }
    return ret;
}
#else
atci_status_t atci_read_data(void)
{
    atci_status_t ret = ATCI_STATUS_OK;
    //atci_send_input_cmd_msg_t input_data;
    //atci_response_t   response_1 = {{0}};
    uint32_t  length = 0;
    uint32_t find_valid_command = 0;
    uint8_t  *input_data = NULL;
    uint32_t  input_data_len = 100;
    uint32_t  read_data_len = 0;

    LOGMSGIDI(ATCI_READ_DATA", enter,g_atci_port_handle:%x\r\n",1, g_atci_port_handle);
    
#ifndef MTK_MUX_ENABLE
    if(atci_vaild_port_check(g_atci_port_handle) == ATCI_STATUS_ERROR) {
        return ATCI_STATUS_ERROR;
    }
#endif
    if(input_data == NULL) {
        input_data = (uint8_t *)atci_mem_alloc(input_data_len);
        memset(input_data, 0, input_data_len);
        if(input_data == NULL) {
            LOGMSGIDW(ATCI_READ_DATA", alloc mem fail.drop data \r\n", 0);
            return ATCI_STATUS_ERROR;
        }
    }

    //for slim
    if(g_input_command == NULL) {
        g_input_command = (atci_send_input_cmd_msg_t *)atci_mem_alloc(sizeof(atci_send_input_cmd_msg_t));
        LOGMSGIDW(ATCI_READ_DATA", alloc mem to get data from port\r\n", 0);
        if(g_input_command == NULL) {
            atci_mem_free(input_data);
            LOGMSGIDW(ATCI_READ_DATA", alloc mem fail\r\n", 0);
            return ATCI_STATUS_ERROR;
        }
        memset(g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));
    }

    // read data from port
    do {
        read_data_len = atci_port_read_data(g_atci_port_handle, (uint8_t *)input_data , input_data_len);

        if((read_data_len > 0) && (atci_input_commad_in_processing == 0)) {
            /* append current receive data to the input buffer*/
            if((read_data_len + g_input_command->input_len) < ATCI_UART_RX_FIFO_BUFFER_SIZE) {
                memcpy(&g_input_command->input_buf[g_input_command->input_len], (char *)input_data, read_data_len);
                g_input_command->input_len += read_data_len;
                //LOGW("current input cmd (len:%d):%s\r\n", g_input_command->input_len, g_input_command.input_buf);
            } else {
                LOGMSGIDW(ATCI_READ_DATA", atci_port_read_data len is bigger than %d\r\n",1, ATCI_UART_RX_FIFO_BUFFER_SIZE);
            }
        }
        memset(input_data, 0, input_data_len);
    } while(read_data_len);

    atci_mem_free(input_data);
    input_data = NULL;

    // drop data if cmd is not processed
    if (atci_input_commad_in_processing != 0) {
        atci_send_data((uint8_t *)"ERROR\r\n", strlen("ERROR\r\n"));
        LOGMSGIDW(ATCI_READ_DATA", drop this cmd, need wait the response of previous AT cmd \r\n", 0);
        return ATCI_STATUS_ERROR;
    }

    // user should know the read_data size before reading
    //read_data_len = atci_port_read_data(g_atci_port_handle, (uint8_t*)(g_input_command->input_buf + g_input_command->input_len), (ATCI_UART_RX_FIFO_BUFFER_SIZE - g_input_command->input_len));
    //g_input_command->input_len += read_data_len;
    if (g_input_command->input_len == 0) {
        LOGMSGIDW(ATCI_READ_DATA", fail \r\n", 0);
        atci_mem_free(g_input_command);
        g_input_command = NULL;
        return ATCI_STATUS_ERROR;
    }

    if ((g_input_command->input_len) < ATCI_UART_RX_FIFO_BUFFER_SIZE) {
        // just use a golbal buffer (g_input_command) to receive data because the mem alloc API can't be used in hal irq
        LOGW(ATCI_READ_DATA",current input cmd (len:%d):%s\r\n", g_input_command->input_len, g_input_command->input_buf);

    } else {
        /*invild command, input cmd is larger then max len, need drop this cmd*/
        atci_input_commad_in_processing = 0;
        atci_mem_free(g_input_command);
        g_input_command = NULL;
        atci_send_data((uint8_t *)"ERROR\r\n", strlen("ERROR\r\n"));
        LOGMSGIDW(ATCI_READ_DATA", fail, input AT cmd length is larger than %d \r\n",1, ATCI_UART_RX_FIFO_BUFFER_SIZE);
        return ATCI_STATUS_ERROR;
    }

    /* find '\0'/CR/LF to be the end of AT command */
    for ( length = 0; length < g_input_command->input_len; length++) {
        if (('\0' == g_input_command->input_buf[length]) ||
                (ATCI_CR_C == g_input_command->input_buf[length]) ||
                (ATCI_LF_C == g_input_command->input_buf[length])) {
            find_valid_command = 1;
            break;
        }
    }

    if(find_valid_command == 1) {
        /* find "AT" to be the beginning of AT command */
        if (('A' == g_input_command->input_buf[0] && 'T' == g_input_command->input_buf[1]) ||
                ('a' == g_input_command->input_buf[0] && 't' == g_input_command->input_buf[1])) {

            atci_input_commad_in_processing = 1;
        } else {

            atci_input_commad_in_processing = 0;
            atci_mem_free(g_input_command);
            g_input_command = NULL;
            atci_send_data((uint8_t *)"ERROR\r\n", strlen("ERROR\r\n"));
            LOGMSGIDW(ATCI_READ_DATA", fail, receive data is not AT cmd type, drop this cmd! \r\n", 0);
            return ATCI_STATUS_ERROR;
        }
    }
    return ret;
}
#endif


// processing the read/send data
/* This function can't be reentry. It should be executed completely and than invoke this function again. */
void atci_processing(void)
{
    /* Queue number */
    uint16_t            common_queue_msg_num = 0;
    uint16_t            notify_queue_msg_num = 0;
    uint16_t            cached_queue_msg_num = 0;
    atci_general_msg_t msg_queue_data;
    uint32_t data_len = 0;
    bool atcmd_switch_racecmd_flag = false;

    atci_semaphore_give(g_atcmd_table_semaphore);

    if (1) {// (g_atci_local_startup_flag == 0) {

        if (NULL == (void *)g_atci_processing_semaphore) {

            g_atci_processing_semaphore = atci_semaphore_create(10, 0);
            if (NULL == (void *)g_atci_processing_semaphore) {
                LOGMSGIDE(ATCI_PROCESSING", create mutex fail\r\n", 0);
            }
        }
    }

    LOGMSGIDI(ATCI_PROCESSING",enter\r\n", 0);

    while (1) {
        //printf("murder task ATCI\n");
        if (( NULL != (void *)g_atci_handler_semaphore )&& (g_atci_local_startup_flag != 0)) {
            LOGMSGIDI(ATCI_PROCESSING", take AT handler mutex \r\n", 0);
            atci_semaphore_take(g_atci_handler_semaphore);
        } else if ( (NULL != (void *)g_atci_processing_semaphore) && (g_atci_local_startup_flag == 0)) {
            LOGMSGIDI(ATCI_PROCESSING",take protect mutex \r\n", 0);
            atci_semaphore_take(g_atci_processing_semaphore);
        }

        if(g_atci_local_startup_flag == 2) {
            continue;
        }

        common_queue_msg_num = atci_queue_get_item_num(g_atci_input_command_queue);
        /*
         * We are handling AT command one by one. Need to wait the previous AT command response and then handle the next one.
         * atci_input_commad_in_processing means the response of AT command was resonse or not.
         * When atci_input_commad_in_processing = 0 means we could receiveing AT comman
         * atci_input_commad_in_processing = 1 means receiving vaild AT command.
         * atci_input_commad_in_processing = 2 means parsing the  AT command.
         * atci_input_commad_in_processing = 3 means handling the AT command response.
         */

        if (common_queue_msg_num > 0) {
            LOGMSGIDI(ATCI_PROCESSING", commad_in_processing(%x), input_queue_msg_num(%d)\r\n",2, atci_input_commad_in_processing, common_queue_msg_num);

            atci_queue_receive_no_wait(g_atci_input_command_queue,&msg_queue_data);

            LOGMSGIDI(ATCI_PROCESSING", msg_id(%x)\\r\n",1, msg_queue_data.msg_id);

            /* message handling */
            if (MSG_ID_ATCI_LOCAL_SEND_CMD_IND == msg_queue_data.msg_id) {
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
                uint32_t data_len;
                mux_handle_t handle;

                data_len = (uint32_t)msg_queue_data.msg_data;
                handle = g_atci_mux[msg_queue_data.mux_index].handle;
                g_atci_mux_index_inuse = msg_queue_data.mux_index;
                LOGMSGIDI(ATCI_PROCESSING" msg_data(%d) handle(%d)\r\n", 2, data_len, handle);
                if (g_data_len == 0) {
                    LOGMSGIDW(ATCI_PROCESSING" g_data_len == 0", 0);
                } else {
                    atci_read_mux_data(data_len, handle);
                }
#else
                atci_read_data();
#endif
                atci_start_sleep_lock_timer();
                if (atci_input_commad_in_processing == 1) {

                    /* handling someone AT command. Need to wait the response and then could handle the next AT command */
                    LOGMSGIDI(ATCI_PROCESSING", handle AT input\r\n", 0);
                    atci_input_commad_in_processing = 2;

                    atci_input_command_handler(g_input_command);
                    /* g_input_command is available now */
                    //memset(&g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));
                    atci_mem_free(g_input_command);
                    g_input_command = NULL;
                }
            }
            common_queue_msg_num--;
        }

        if(g_atci_local_startup_flag == 2) {
            continue;
        }

        cached_queue_msg_num = atci_queue_get_item_num(g_atci_data_cached_queue);

        if(cached_queue_msg_num >0) {
            LOGMSGIDI(ATCI_PROCESSING", cached_queue_msg_num(%d)\r\n",1, cached_queue_msg_num);
        }

        while (cached_queue_msg_num > 0) {

            atci_queue_receive_no_wait(g_atci_data_cached_queue, &msg_queue_data);
            if (msg_queue_data.msg_id == MSG_ID_ATCI_LOCAL_WRITE_CMD_IND) {
                // send the cache output data
                LOGMSGIDI(ATCI_PROCESSING", g_send_data_cache_left_len(%x)\r\n",1, g_send_data_cache_left_len);
                if (g_send_data_cache_left_len > 0) {
                    atci_send_data(NULL, 0);
                }
            }
#ifdef HAL_SLEEP_MANAGER_ENABLED
            if(msg_queue_data.msg_id == MSG_ID_ATCI_LOCAL_WAKEUP_SLEEP_EVENT_NOTIFY_IND)
            {
                atci_start_sleep_lock_timer();
            }
#endif
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE) && defined(MTK_USB_DEMO_ENABLED)
            if (msg_queue_data.msg_id == MSG_ID_ATCI_LOCAL_USB_PLUG_IN) {
                atci_mux_usb_plug(true);
            } else if(msg_queue_data.msg_id == MSG_ID_ATCI_LOCAL_USB_PLUG_OUT) {
                atci_mux_usb_plug(false);
            }
#endif
            cached_queue_msg_num --;
        }

        if(g_atci_local_startup_flag == 2) {
            continue;
        }

        notify_queue_msg_num = atci_queue_get_item_num(g_atci_resonse_command_queue);
        if(notify_queue_msg_num >0) {
            //LOGMSGIDI(ATCI_PROCESSING", notify_queue_msg_num(%d), cache_len(%x)\r\n",2, notify_queue_msg_num, g_send_data_cache_left_len);
        }

        while ((g_send_data_cache_left_len == 0 ) && (notify_queue_msg_num > 0)) {
            atci_queue_receive_no_wait(g_atci_resonse_command_queue,&msg_queue_data);

            if ((MSG_ID_ATCI_LOCAL_RSP_NOTIFY_IND == msg_queue_data.msg_id) && (atci_input_commad_in_processing == 2)) {
                /* the response resonse of handling AT command is finished . allow to handle the next AT command */
                atci_input_commad_in_processing = 3;
                LOGMSGIDI(ATCI_PROCESSING", handle AT response\r\n", 0);
            } else if (MSG_ID_ATCI_LOCAL_URC_NOTIFY_IND == msg_queue_data.msg_id) {
                LOGMSGIDI(ATCI_PROCESSING", handle AT urc \r\n", 0);
            }
            else if(MSG_ID_ATCI_LOCAL_ATCMD_SW_RACECMD_NOTIFY_IND == msg_queue_data.msg_id)
            {
                atcmd_switch_racecmd_flag = true;
                notify_queue_msg_num--;
                continue;
            }
            /* the message handling. Sending response or URC to UART */
            memcpy(&data_len, msg_queue_data.msg_data, sizeof(uint16_t));
            atci_send_data((uint8_t *)(msg_queue_data.msg_data + sizeof(uint16_t)), data_len);
            /* free the message content */
            atci_mem_free(msg_queue_data.msg_data);
            msg_queue_data.msg_data = NULL;
            msg_queue_data.msg_id = MSG_ID_ATCI_END;

            //LOGMSGIDI(ATCI_PROCESSING", handle AT send cmd done\r\n", 0);

            notify_queue_msg_num--;
        }
        if(atcmd_switch_racecmd_flag)
        {
            /*at cmd switch race cmd,return atci processing*/
            LOGMSGIDI("atci_processing() finish, at cmd switch to race cmd.\r\n", 0);
            //break;
        }
    }
}

atci_status_t atci_data_processing_for_deinit(void)
{
    /* Queue number */
    uint16_t            cached_queue_msg_num = 0;
    uint16_t            notify_queue_msg_num = 0;
    atci_general_msg_t msg_queue_data;
    atci_status_t ret =  ATCI_STATUS_ERROR;
    uint32_t data_len = 0;

    cached_queue_msg_num = atci_queue_get_item_num(g_atci_data_cached_queue);

    if(cached_queue_msg_num >0) {
        LOGMSGIDI(ATCI_DATA_PROCESSING_FOR_DEINIT", cached_queue_msg_num(%d)\r\n",1, cached_queue_msg_num);
    }

    while (cached_queue_msg_num > 0) {

        atci_queue_receive_no_wait(g_atci_data_cached_queue, &msg_queue_data);
        if (msg_queue_data.msg_id == MSG_ID_ATCI_LOCAL_WRITE_CMD_IND) {
            // send the cache output data
            if (g_send_data_cache_left_len > 0) {
                atci_send_data(NULL, 0);
            }
        }
        cached_queue_msg_num --;
    }

    notify_queue_msg_num = atci_queue_get_item_num(g_atci_resonse_command_queue);
    if(notify_queue_msg_num >0) {
        LOGMSGIDI(ATCI_DATA_PROCESSING_FOR_DEINIT", notify_queue_msg_num(%d)\r\n",1,notify_queue_msg_num);
    }

    while ((g_send_data_cache_left_len == 0 ) && (notify_queue_msg_num > 0)) {
        int32_t ret = atci_queue_receive_no_wait(g_atci_resonse_command_queue,&msg_queue_data);
        if (0 == ret) {
            LOGMSGIDE("atci_processing() get response queue data fail\r\n", 0);
            return ATCI_STATUS_ERROR;
        }

        if ((MSG_ID_ATCI_LOCAL_RSP_NOTIFY_IND == msg_queue_data.msg_id) && (atci_input_commad_in_processing == 2)) {
            /* the response resonse of handling AT command is finished . allow to handle the next AT command */
            atci_input_commad_in_processing = 3;
            LOGMSGIDI(ATCI_DATA_PROCESSING_FOR_DEINIT", handle AT response\r\n", 0);
        } else if (MSG_ID_ATCI_LOCAL_URC_NOTIFY_IND == msg_queue_data.msg_id) {
            LOGMSGIDI(ATCI_DATA_PROCESSING_FOR_DEINIT", handle AT urc \r\n", 0);
        }
        /* the message handling. Sending response or URC to UART */
        memcpy(&data_len, msg_queue_data.msg_data, sizeof(uint16_t));
        atci_send_data((uint8_t *)(msg_queue_data.msg_data + sizeof(uint16_t)), data_len);
        /* free the message content */
        atci_mem_free(msg_queue_data.msg_data);
        msg_queue_data.msg_data = NULL;
        msg_queue_data.msg_id = MSG_ID_ATCI_END;

        LOGMSGIDI(ATCI_DATA_PROCESSING_FOR_DEINIT", handle AT send cmd done\r\n", 0);

        notify_queue_msg_num--;

    }
    #if defined(MTK_RACE_CMD_ENABLE) && defined(MTK_SWITCH_TO_RACE_COMMAND_ENABLE)
    /*notify atci processing return*/
    msg_queue_data.msg_id = MSG_ID_ATCI_LOCAL_ATCMD_SW_RACECMD_NOTIFY_IND;
    msg_queue_data.msg_data = NULL;
    atci_queue_send(g_atci_resonse_command_queue, (void*)&msg_queue_data);
    atci_semaphore_give(g_atci_handler_semaphore);
    #endif
    ret = ATCI_STATUS_OK;

    return ret;
}

atci_status_t atci_vaild_port_check(uint32_t port)
{
    atci_status_t ret = ATCI_STATUS_ERROR;

#if defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    if( port != 0) {
        ret = ATCI_STATUS_OK;
    }
#else
    if( port < HAL_UART_MAX) {
        ret = ATCI_STATUS_OK;
    }
#endif

    return ret;
}


atci_status_t atci_port_deinit(uint32_t port)
{
#ifdef MTK_MUX_ENABLE
    return ATCI_STATUS_OK;
#elif defined(MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)
    return atci_serial_port_deinit(port);
#elif !defined(MTK_ATCI_VIA_MUX)
    return atci_uart_deinit((hal_uart_port_t)port);
#endif

}

atci_status_t atci_local_deinit(void)
{
    atci_status_t ret =  ATCI_STATUS_ERROR;
    uint16_t i;

    g_atci_local_startup_flag = 0;
    ret = atci_data_processing_for_deinit();

    atci_input_commad_in_processing = 0;

    g_atci_registered_table_number = 0;
    for ( i = 0; i < ATCI_MAX_GNENERAL_TABLE_NUM; i++) {
        g_atci_cm4_general_hdlr_tables[i].item_table = NULL;
        g_atci_cm4_general_hdlr_tables[i].item_table_size = 0;
    }

    ///memset(g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));
    atci_mem_free(g_input_command);
    g_input_command = NULL;

    ret = ATCI_STATUS_OK;

    return ret;
}

#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
atci_status_t atci_mux_deinit(hal_uart_port_t port)
{
    mux_status_t status;
    mux_handle_t deinit_handle;
    int atci_mux_index;

    if (MUX_STATUS_OK != mux_query_user_handle((mux_port_t)port, "ATCI", &deinit_handle)) {
        LOGMSGIDE(ATCI_DEINIT", mux_query_user_handle fail\r\n", 0);
        return ATCI_STATUS_ERROR;
    }

    if (!atci_mux_find_index_by_handle(deinit_handle, &atci_mux_index)) {
        LOGMSGIDE(ATCI_DEINIT", atci_mux_find_index_by_handle fail\r\n", 0);
        return ATCI_STATUS_ERROR;
    } else {
        g_atci_mux[atci_mux_index].type = ATCI_MUX_UNUSE;
        g_atci_mux[atci_mux_index].handle = 0xFF;
    }

    status = mux_close(deinit_handle);
    if(status != MUX_STATUS_OK) {
        LOGMSGIDE(ATCI_DEINIT", mux_close status(%d)\r\n", 1, status);
        return ATCI_STATUS_ERROR;
    } else {
        LOGMSGIDI(ATCI_DEINIT" success, port(%d), handle(%x)\r\n", 2, port, deinit_handle);
        return ATCI_STATUS_OK;
    }
}

#endif

atci_status_t atci_deinit(hal_uart_port_t port)
{
    /* Init */
    atci_status_t ret = ATCI_STATUS_ERROR;

    LOGMSGIDI(ATCI_DEINIT", enter (flag:%d)\r\n",1, g_atci_local_startup_flag);

    if (g_atci_local_startup_flag == 0) {
        return ret;
    }

    ret = atci_local_deinit();

    // close serial port
    LOGMSGIDE("atci_deinit %x \r\n",1,g_atci_port_handle);
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
    ret = atci_mux_deinit(port);
#else
    ret = atci_port_deinit(g_atci_port_handle);
#endif
    if (ret != ATCI_STATUS_OK) {
        LOGMSGIDE(ATCI_DEINIT", fail \r\n", 0);
    } else {
        LOGMSGIDI(ATCI_DEINIT", success \r\n", 0);
    }

    return ret;
}


atci_status_t atci_deinit_keep_table(hal_uart_port_t port)
{
    /* Init */
    atci_status_t ret = ATCI_STATUS_ERROR;

    LOGMSGIDI("atci_deinit_keep_table, enter (flag:%d)\r\n",1, g_atci_local_startup_flag);

    if (g_atci_local_startup_flag == 0) {
        return ret;
    }

    g_atci_local_startup_flag = 0;
    atci_data_processing_for_deinit();

    atci_input_commad_in_processing = 0;

    ///memset(g_input_command, 0, sizeof(atci_send_input_cmd_msg_t));
    atci_mem_free(g_input_command);
    g_input_command = NULL;

    // close serial port
    LOGMSGIDE("atci_deinit_keep_table %x \r\n",1,g_atci_port_handle);
    ret = atci_port_deinit(g_atci_port_handle);
    if (ret != ATCI_STATUS_OK) {
        LOGMSGIDE("atci_deinit_keep_table, fail \r\n", 0);
    } else {
        LOGMSGIDI("atci_deinit_keep_table, success \r\n", 0);
    }

    return ret;
}

void atci_task()
{
    while(1)
    {
      atci_processing();
    }
}

static void atci_sleep_lock_timer_callback(TimerHandle_t xtimer)
{
    if(hal_sleep_manager_is_sleep_handle_alive(atci_uart_rx_sleep_handle) == true)
    {
        hal_sleep_manager_unlock_sleep(atci_uart_rx_sleep_handle);
        LOGMSGIDI("atci sleep unlock\r\n", 0);
    }
}
static void atci_start_sleep_lock_timer(void)
{
    if(atci_uart_rx_sleep_handle == 0xFF)
    {
        atci_uart_rx_sleep_handle = hal_sleep_manager_set_sleep_handle("atci");
        LOGMSGIDI("create atci sleep lock handler success %d \r\n",1, atci_uart_rx_sleep_handle);
    }
    if(atci_sleep_lock_timer == NULL)
    {
        atci_sleep_lock_timer = xTimerCreate("sleep lock timer", (10 * 1000 / portTICK_PERIOD_MS), pdFALSE, NULL, atci_sleep_lock_timer_callback);
        if(atci_sleep_lock_timer == NULL)
        {
            if(hal_sleep_manager_is_sleep_handle_alive(atci_uart_rx_sleep_handle) == true)
            {
                hal_sleep_manager_unlock_sleep(atci_uart_rx_sleep_handle);
            }
            LOGMSGIDI("create atci sleep lock timer fail\r\n", 0);
            return;
        }
        LOGMSGIDI("create atci sleep lock timer success\r\n", 0);
    }
    if(xTimerIsTimerActive(atci_sleep_lock_timer) == pdFALSE)
    {
        if(hal_sleep_manager_is_sleep_handle_alive(atci_uart_rx_sleep_handle) == true)
        {
            hal_sleep_manager_unlock_sleep(atci_uart_rx_sleep_handle);
        }
        hal_sleep_manager_lock_sleep(atci_uart_rx_sleep_handle);
        xTimerStart(atci_sleep_lock_timer, 0);
        LOGMSGIDI("atci sleep lock\r\n", 0);
    } else {
        LOGMSGIDI("atci restart sleep lock timer\r\n", 0);
        xTimerStop(atci_sleep_lock_timer, 0);
        xTimerStart(atci_sleep_lock_timer, 0);
    }
}

#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
// mux limitaiton: data_len should send to atci_read_data, the data should be taken at one time
//uint32_t g_data_len;

void mux_atci_callback(mux_handle_t handle, mux_event_t event,uint32_t data_len,void*user_data)
{
    atci_general_msg_t msg_queue_item;

    switch (event) {
        case MUX_EVENT_READY_TO_READ:
        {
            if (atci_check_startup_finish()) {
                LOGMSGIDI(ATCI_SERIAL_PORT_DATA_CALLBACK" 1, read event, data_len %d, g_data_len %d", 2, data_len, g_data_len);
                int mux_index;
                if (!atci_mux_find_index_by_handle(handle, &mux_index)) {
                    LOGMSGIDI("handle wrong!\r\n", 0);
                    return;
                }
                g_data_len += data_len;
                    msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_SEND_CMD_IND;
                    msg_queue_item.msg_data = (void *)data_len;
                    msg_queue_item.mux_index = mux_index;
                    atci_queue_send(g_atci_input_command_queue, (void *)&msg_queue_item);
                atci_semaphore_give(g_atci_handler_semaphore);
            }
            break;
        }
        case MUX_EVENT_READY_TO_WRITE:
        {
            /*TO do */
            break;
        }
        case MUX_EVENT_WAKEUP_FROM_SLEEP: {
            #ifdef HAL_SLEEP_MANAGER_ENABLED
            LOGMSGIDI("wakeup sleep event!\r\n", 0);
            msg_queue_item.msg_id = MSG_ID_ATCI_LOCAL_WAKEUP_SLEEP_EVENT_NOTIFY_IND;
            atci_queue_send(g_atci_data_cached_queue, (void *)&msg_queue_item);
            atci_semaphore_give(g_atci_handler_semaphore);
            #endif
            break;
        }
    }
}
#endif

