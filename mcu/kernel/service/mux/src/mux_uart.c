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
#include "hal_uart.h"
#include "hal_uart_internal.h"
#include "syslog.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_spm.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
static sleep_management_lock_request_t uart_lock_sleep[HAL_UART_MAX] = {SLEEP_LOCK_UART0, SLEEP_LOCK_UART1, SLEEP_LOCK_UART2};
static volatile bool mux_uart_lock_sleep_handle[HAL_UART_MAX] = {false, false, false};
static hal_uart_port_t uart_log_running_port = HAL_UART_MAX;
#endif/*HAL_SLEEP_MANAGER_ENABLED*/

/*Static global variables for CONNECT and DISCONNECT service*/
static volatile uint32_t g_uart_record_wptr_legnth[3] = {0};
static volatile bool g_uart_on_off_status[3] = {true, true, true};
extern UART_REGISTER_T *const g_uart_regbase[];

#ifndef MTK_SINGLE_CPU_ENV  /*multi core user*/
static volatile mux_port_config_t *g_uart_port_configs = (volatile mux_port_config_t *)HW_SYSRAM_PRIVATE_MEMORY_MUX_VAR_PORT_START; //80 * N
#endif

/*MUX operate DMA hardware pointer begin*/
ATTR_TEXT_IN_TCM uint32_t port_mux_uart_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    return uart_get_hw_rptr(port_index, is_rx);
}

ATTR_TEXT_IN_TCM uint32_t port_mux_uart_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    return uart_get_hw_wptr(port_index, is_rx);
}

/**When Receive data, MUX will call this API to move Rx Ring buffer Read point.
    for MUX, firstly ,should copy data out. Then call this API to move Rx Ring buffer Read point. **/
void port_mux_uart_set_rx_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    uart_set_sw_move_byte(port_index, 1, move_bytes);
}

/**When Send data, MUX will call this API to move Tx Ring buffer Write point.
    for MUX, firstly ,should copy data in. Then call this API to move Tx Ring buffer Write point. **/
ATTR_TEXT_IN_TCM void port_mux_uart_set_tx_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    if (g_uart_on_off_status[port_index] == true) {
        uart_set_sw_move_byte(port_index, 0, move_bytes);
        g_uart_record_wptr_legnth[port_index] = 0;
    } else {
        g_uart_record_wptr_legnth[port_index] = move_bytes;
    }
}
/*MUX operate DMA hardware pointer end*/

#ifdef HAL_UART_FEATURE_FLOWCONTROL_CALLBACK
static uint32_t uart_gpt_handle[HAL_UART_MAX] = {0};                                      //set gpt handle
static volatile uint32_t g_uart_get_rptr_when_start_timer[HAL_UART_MAX] = {0};            //record read pointer when user start gpt timer.
static volatile bool g_uart_already_set_gpt_timer[HAL_UART_MAX] = {false, false, false};  //uart set gpt timer when receive flow control
static uint32_t g_uart_blocking_port = HAL_UART_MAX;                                      //indicate which port is blocking by flow control.
#define MAX_FLOW_CONTROL_TIME 5000                                                       //indicate max time that trigger assert more than this time.

void uart_debug_flow_control_callback(void *user_data)
{
    uint32_t  port_index = (uint32_t)user_data;
    uint32_t  hw_tx_rptr;
    UART_REGISTER_T    *uartx = g_uart_regbase[port_index];

    hal_gpt_sw_stop_timer_ms(uart_gpt_handle[port_index]);
    hal_gpt_sw_free_timer(uart_gpt_handle[port_index]);
    g_uart_already_set_gpt_timer[port_index] = false;
    hw_tx_rptr = port_mux_uart_get_hw_rptr(port_index, false);
    MUX_PORT_MSGID_I("UART_flow_control_callback free timer ! port=%d MCR=%08x\r\n", 2, port_index, uartx->MCR_UNION.MCR);

    /*  assert if two condition true:
        1. DMA tx channel read pointer haven't moved during this period.
        2. UART MCR register show that xoff received or RTS signal received.
    */
    if ( (g_uart_get_rptr_when_start_timer[port_index] == hw_tx_rptr) && \
            ((uartx->MCR_UNION.MCR & UART_MCR_XOFF_STATUS_MASK) || uartx->MCR_UNION.MCR & UART_MCR_RTS_MASK) ) {
        MUX_PORT_MSGID_I("UART_flow_control_callback receive xoff more than 5s! port=%d MCR=%08x\r\n", 2, port_index, uartx->MCR_UNION.MCR);
        // hal_gpt_delay_ms(1000);
        g_uart_blocking_port = port_index;
        // assert(0);
    }
}
#endif /*HAL_UART_FEATURE_FLOWCONTROL_CALLBACK*/

#ifdef MTK_CPU_NUMBER_0

static mux_irq_handler_t g_mux_irq_handler;
#define UART_PORT_INDEX_TO_MUX_PORT(port_index) (port_index + MUX_UART_0)

static void mux_uart_callback(hal_uart_callback_event_t event, void *user_data)
{
    uint32_t  port_index = (uint32_t)user_data;
    uint32_t  hw_tx_rptr, hw_tx_wptr;
    uint32_t  mask_irq;


    if (event == HAL_UART_EVENT_READY_TO_WRITE) {
        g_mux_irq_handler(UART_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_WRITE, user_data);
    } else if (event == HAL_UART_EVENT_READY_TO_READ) {
        g_mux_irq_handler(UART_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_READY_TO_READ, user_data);
    } else if (event == HAL_UART_EVENT_WAKEUP_SLEEP) {
        g_mux_irq_handler(UART_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_WAKEUP_FROM_SLEEP, user_data);
    } else if (event == HAL_UART_EVENT_TRANSMISSION_DONE) {
        g_mux_irq_handler(UART_PORT_INDEX_TO_MUX_PORT(port_index), MUX_EVENT_TRANSMISSION_DONE, user_data);
#ifdef HAL_SLEEP_MANAGER_ENABLED

        /* condition of unlock sleep
           1, This hardware uart port has been locked.
           2, DMA hw_write_point equals to hw_read_point that means DMA empty.
           3, mux_uart_lock_sleep_handle is true means this port is locked by mux_uart.
           4, g_uart_record_wptr_legnth must equal to 0 to avoid race condition.
                (i). For connection,g_uart_record_wptr_legnth is always zero,
                (ii).For disconnection,g_uart_record_wptr_legnth record bytes(not zero) that restored in vfifo.
                     but both DMA&UART are empty(IDLE) may lead to enter sleep and reinitialize hardware and lose data.
        */
        if (port_index != uart_log_running_port) {

            port_mux_local_cpu_enter_critical(&mask_irq);   // mask_irq begin
            hw_tx_rptr = port_mux_uart_get_hw_rptr(port_index, false);
            hw_tx_wptr = port_mux_uart_get_hw_wptr(port_index, false);
            if ( (hal_sleep_manager_is_sleep_handle_alive(uart_lock_sleep[port_index]) == true) && \
                    (g_uart_record_wptr_legnth[port_index] == 0) && (hw_tx_rptr == hw_tx_wptr) && \
                    (mux_uart_lock_sleep_handle[port_index] == true) ) {
                hal_sleep_manager_unlock_sleep(uart_lock_sleep[port_index]);
                mux_uart_lock_sleep_handle[port_index] = false;
            }
            port_mux_local_cpu_exit_critical(mask_irq);    // mask_irq end
        }
#endif /*HAL_SLEEP_MANAGER_ENABLED*/
    }
#ifdef HAL_UART_FEATURE_FLOWCONTROL_CALLBACK
    else if (event == HAL_UART_EVENT_SW_FLOW_CTRL) {
        UART_REGISTER_T    *uartx = g_uart_regbase[port_index];
#ifdef HAL_SLEEP_MANAGER_ENABLED
        if (port_index != uart_log_running_port) {
            MUX_PORT_MSGID_I("UART_SW_FLOW_CTRL! port=%d MCR=%08x\r\n", 2, port_index, uartx->MCR_UNION.MCR);
        }
#endif /*HAL_SLEEP_MANAGER_ENABLED*/
        /*Every time flow control occurs, the timer will restart*/
        if (g_uart_already_set_gpt_timer[port_index] == false) {
            MUX_PORT_MSGID_I("UART_SW_FLOW_CTRL start %d(ms) timer! port=%d MCR=%08x\r\n", 3, MAX_FLOW_CONTROL_TIME, port_index, uartx->MCR_UNION.MCR);

            if ( HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&uart_gpt_handle[port_index]) ) {
                return;
            }
            if ( HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_ms(uart_gpt_handle[port_index], MAX_FLOW_CONTROL_TIME, uart_debug_flow_control_callback , (void *)(intptr_t)port_index) ) {
                hal_gpt_sw_free_timer(uart_gpt_handle[port_index]);
                return;
            }
            g_uart_get_rptr_when_start_timer[port_index] = port_mux_uart_get_hw_rptr(port_index, false);
            g_uart_already_set_gpt_timer[port_index] = true;        //set true to control start timer.

        } else if (g_uart_already_set_gpt_timer[port_index] == true) {
            // MUX_PORT_MSGID_I("UART_SW_FLOW_CTRL restart %d(ms) timer! port=%d MCR=%08x\r\n",3,MAX_FLOW_CONTROL_TIME,port_index,uartx->MCR_UNION.MCR);

            hal_gpt_sw_stop_timer_ms(uart_gpt_handle[port_index]);
            hal_gpt_sw_free_timer(uart_gpt_handle[port_index]);

            if ( HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&uart_gpt_handle[port_index]) ) {
                return;
            }
            if ( HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_ms(uart_gpt_handle[port_index], MAX_FLOW_CONTROL_TIME, uart_debug_flow_control_callback , (void *)(intptr_t)port_index) ) {
                hal_gpt_sw_free_timer(uart_gpt_handle[port_index]);
                return;
            }
            g_uart_get_rptr_when_start_timer[port_index] = port_mux_uart_get_hw_rptr(port_index, false);
        }

    } else if (event == HAL_UART_EVENT_HW_FLOW_CTRL) {
        UART_REGISTER_T    *uartx = g_uart_regbase[port_index];
        /*Every time flow control occurs, the timer will restart*/
        if (g_uart_already_set_gpt_timer[port_index] == false) {
            MUX_PORT_MSGID_I("UART_HW_FLOW_CTRL start %d(ms) timer! port=%d MCR=%08x\r\n", 3, MAX_FLOW_CONTROL_TIME, port_index, uartx->MCR_UNION.MCR);
            if ( HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&uart_gpt_handle[port_index]) ) {
                return;
            }
            if ( HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_ms(uart_gpt_handle[port_index], MAX_FLOW_CONTROL_TIME, uart_debug_flow_control_callback , (void *)(intptr_t)port_index) ) {
                hal_gpt_sw_free_timer(uart_gpt_handle[port_index]);
                return;
            }
            g_uart_get_rptr_when_start_timer[port_index] = port_mux_uart_get_hw_rptr(port_index, false);
            g_uart_already_set_gpt_timer[port_index] = true;        //set true to control start timer.

        } else if (g_uart_already_set_gpt_timer[port_index] == true) {
            MUX_PORT_MSGID_I("UART_HW_FLOW_CTRL restart %d(ms) timer! port=%d MCR=%08x\r\n", 3, MAX_FLOW_CONTROL_TIME, port_index, uartx->MCR_UNION.MCR);
            hal_gpt_sw_stop_timer_ms(uart_gpt_handle[port_index]);
            hal_gpt_sw_free_timer(uart_gpt_handle[port_index]);

            if ( HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&uart_gpt_handle[port_index]) ) {
                return;
            }
            if ( HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_ms(uart_gpt_handle[port_index], MAX_FLOW_CONTROL_TIME, uart_debug_flow_control_callback , (void *)(intptr_t)port_index) ) {
                hal_gpt_sw_free_timer(uart_gpt_handle[port_index]);
                return;
            }
            g_uart_get_rptr_when_start_timer[port_index] = port_mux_uart_get_hw_rptr(port_index, false);
        }
    }
#endif /*HAL_UART_FEATURE_FLOWCONTROL_CALLBACK*/
}

mux_status_t port_mux_uart_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    hal_uart_config_t uart_config;
    hal_uart_dma_config_t dma_config;

    g_mux_irq_handler = irq_handler;

    uart_config.baudrate     = p_setting->p_user_setting->dev_setting.uart.uart_config.baudrate;    //CONFIG_SYSLOG_BAUDRATE;
    uart_config.parity       = p_setting->p_user_setting->dev_setting.uart.uart_config.parity;      //HAL_UART_PARITY_NONE;
    uart_config.stop_bit     = p_setting->p_user_setting->dev_setting.uart.uart_config.stop_bit;    //HAL_UART_STOP_BIT_1;
    uart_config.word_length  = p_setting->p_user_setting->dev_setting.uart.uart_config.word_length; //HAL_UART_WORD_LENGTH_8;

    dma_config.send_vfifo_buffer      = (uint8_t *)(uint32_t *)p_setting->tx_buf_addr;
    dma_config.send_vfifo_buffer_size = p_setting->tx_buf_size;
    dma_config.receive_vfifo_buffer   = (uint8_t *)(uint32_t *)p_setting->rx_buf_addr;
    dma_config.receive_vfifo_buffer_size = p_setting->rx_buf_size;
    dma_config.send_vfifo_threshold_size = dma_config.send_vfifo_buffer_size / 8;
    dma_config.receive_vfifo_threshold_size   = dma_config.receive_vfifo_buffer_size / 2;
    dma_config.receive_vfifo_alert_size       = 12;

    hal_uart_deinit(port_index);
    /*assign logging port and record in uart driver*/
    extern mux_port_t query_syslog_port(void);
    mux_port_t syslog_port_is_runing = query_syslog_port();
    uart_backup_log_para_to_share_buf(syslog_port_is_runing);
    if (HAL_UART_STATUS_OK != hal_uart_init(port_index, &uart_config)) {
        return MUX_STATUS_ERROR_INIT_FAIL;
    }

    if (HAL_UART_STATUS_OK != hal_uart_set_dma(port_index, &dma_config)) {
        return MUX_STATUS_ERROR_INIT_FAIL;
    }

    if (p_setting->p_user_setting->dev_setting.uart.flowcontrol_type == MUX_UART_NONE_FLOWCONTROL) {
        hal_uart_disable_flowcontrol(port_index);
    } else if (p_setting->p_user_setting->dev_setting.uart.flowcontrol_type == MUX_UART_SW_FLOWCONTROL) {
        hal_uart_set_software_flowcontrol(port_index, 0x11, 0x13, 0x77);
    } else if (p_setting->p_user_setting->dev_setting.uart.flowcontrol_type == MUX_UART_HW_FLOWCONTROL) {
        hal_uart_set_hardware_flowcontrol(port_index);
    } else {
        assert(0);
    }

    if (HAL_UART_STATUS_OK != hal_uart_register_callback(port_index, mux_uart_callback, (void *)(intptr_t)port_index)) {
        return MUX_STATUS_ERROR_INIT_FAIL;
    }
    /*In signal core ,this function restore logging port parameter in uart driver.
      In multi core,  this function restore hardware(DMA&UART) parameter of logging port to share buffer*/
    uart_backup_log_para_to_share_buf(syslog_port_is_runing);
    uart_log_running_port = syslog_port_is_runing;

    return MUX_STATUS_OK;
}

mux_status_t port_mux_uart_deinit(uint8_t port_index)
{
    if (HAL_UART_STATUS_OK != hal_uart_deinit(port_index)) {
        return MUX_STATUS_ERROR_DEINIT_FAIL;
    } else {
        return MUX_STATUS_OK;
    }
}

void port_mux_uart_exception_init(uint8_t port_index)
{
    hal_uart_config_t uart_config;

    hal_uart_deinit(port_index);
    uart_config.baudrate = CONFIG_SYSLOG_BAUDRATE;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    hal_uart_init(port_index, &uart_config);
#ifndef MTK_DEBUG_PLAIN_LOG_ENABLE
    hal_uart_set_software_flowcontrol(port_index, 0x11, 0x13, 0x77);
#endif
}

void port_mux_uart_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    uart_exception_send_polling(port_index, buffer, size);
}

bool port_mux_uart_buf_is_full(uint8_t port_index, bool is_rx)
{
    return uart_get_buf_full_status(port_index, is_rx);
}

#endif /* MTK_CPU_NUMBER_0 */

void port_mux_uart_phase1_send(uint8_t port_index)
{
#ifdef HAL_SLEEP_MANAGER_ENABLED
    /*if the port is not for logging, mux_uart will lock sleep until data transfer done.
       as far as logging port, SPM just waits all system log sent out(TX empty) then power off infra_domain.

       condition of tx lock sleep:
       1, Nobody lock sleep for this hardware port.
       2, because hal uart interface may lock sleep using uart's handle,so it is necessary to judge true or false of mux_uart_lock_sleep_handle.
    */
    if (port_index != uart_log_running_port) {
        uint32_t mask_irq_phase1;
        port_mux_local_cpu_enter_critical(&mask_irq_phase1);   // mask_irq begin  these run 15 us.
        if ( (hal_sleep_manager_is_sleep_handle_alive(uart_lock_sleep[port_index]) == false) && (mux_uart_lock_sleep_handle[port_index] == false)) {
            mux_uart_lock_sleep_handle[port_index] = true;
            port_mux_local_cpu_exit_critical(mask_irq_phase1);     // mask_irq end  these run 15 us.
            hal_sleep_manager_lock_sleep(uart_lock_sleep[port_index]);
        } else {
            port_mux_local_cpu_exit_critical(mask_irq_phase1);     // mask_irq end  these run 15 us.
        }

    }
#else
    PORT_MUX_UNUSED(port_index);
#endif
    return;
}


void port_mux_uart_phase2_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void port_mux_uart_clear_fifo(uint8_t port_index)
{
    if ( HAL_UART_STATUS_OK != uart_clear_vfifo_and_fifo(port_index)) {
        MUX_PORT_MSGID_I("port_mux_uart_clear_fifo_failed!\r\n", 0);
    }
}

mux_status_t port_mux_uart_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    uint32_t mask;
    UART_REGISTER_T    *uartx = g_uart_regbase[port_index];

    switch (command) {
    case MUX_CMD_CONNECT: {

#ifndef MTK_SINGLE_CPU_ENV  /*multi core user*/
        volatile mux_port_config_t *log_x = &g_uart_port_configs[port_index];
        /*this port_is_connect will restore in share buffer,if dsp is existed,dsp could know currently status*/
        log_x->port_is_connect = MUX_PORT_CONNECT;
#endif

        MUX_PORT_MSGID_I("port_mux_uart_control()--> CONNECT ! record_size = %d\r\n", 1, g_uart_record_wptr_legnth[port_index]);
        hal_nvic_save_and_set_interrupt_mask(&mask);
        g_uart_on_off_status[port_index] = true;
        if (g_uart_record_wptr_legnth[port_index] != 0) {
            port_mux_uart_set_tx_hw_wptr(port_index, g_uart_record_wptr_legnth[port_index]);
        }
        g_uart_record_wptr_legnth[port_index] = 0;
        hal_nvic_restore_interrupt_mask(mask);
        return MUX_STATUS_OK;
    }
    break;

    case MUX_CMD_DISCONNECT: {

        g_uart_on_off_status[port_index] = false;
        MUX_PORT_MSGID_I("port_mux_uart_control()--> DISCONNECT !\r\n", 0);

#ifndef MTK_SINGLE_CPU_ENV  /*multi core user*/
        volatile mux_port_config_t *log_x = &g_uart_port_configs[port_index];
        /*this port_is_connect will restore in share buffer,if dsp is existed,dsp could know currently status*/
        log_x->port_is_connect = MUX_PORT_DISCONNECT;
#endif

        return MUX_STATUS_OK;
    }
    break;

    case MUX_CMD_CLEAN: {
        MUX_PORT_MSGID_I("port_mux_uart mcr_status %08x\r\n", 1, uartx->MCR_UNION.MCR);
        port_mux_uart_clear_fifo(port_index);
        MUX_PORT_MSGID_I("port_mux_uart_control()--> CLEAN !\r\n", 0);
        return MUX_STATUS_OK;
    }
    break;

    default: {
        MUX_PORT_MSGID_I("MUX UART error control cmd:0x%x\r\n", 1, command);
    }
    break;
    }
    return MUX_STATUS_ERROR;
}

port_mux_device_ops_t g_port_mux_uart_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_uart_normal_init,
    port_mux_uart_deinit,
    port_mux_uart_exception_init,
    port_mux_uart_exception_send,
    port_mux_uart_buf_is_full,
#endif
    port_mux_uart_get_hw_rptr,
    port_mux_uart_set_rx_hw_rptr,
    port_mux_uart_get_hw_wptr,
    port_mux_uart_set_tx_hw_wptr,
    port_mux_uart_phase1_send,
    port_mux_uart_phase2_send,
    port_mux_uart_control,
    NULL,
    NULL,
};



