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

#ifndef __HAL_UART_INTERNAL_H__
#define __HAL_UART_INTERNAL_H__

#ifdef HAL_UART_MODULE_ENABLED

#include <assert.h>
#include "hal_pdma_internal.h"
#include "hal_log.h"
#include "hal_uart.h"
#include "ab156x.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UART_INPUT_CLOCK_DCXO 26000000     // Clock frequency used by UART
#define UART_DMA_MAX_SETTING_VALUE (2<<16) // The maximum buffer size that user can set cannot exceed 64K
#ifndef __UBL__
#define UART_ASSERT() assert(0)
#else
#define UART_ASSERT() log_hal_error("assert\r\n")
#endif/*__UBL__*/

typedef enum {
    UART_INTERRUPT_NONE,
    UART_INTERRUPT_RECEIVE_ERROR,
    UART_INTERRUPT_RECEIVE_TIMEOUT,
    UART_INTERRUPT_RECEIVE_BREAK,
    UART_INTERRUPT_RECEIVE_AVAILABLE,
    UART_INTERRUPT_SEND_AVAILABLE,
    UART_INTERRUPT_SOFTWARE_FLOWCONTROL,
    UART_INTERRUPT_HARDWARE_FLOWCONTROL
} uart_interrupt_type_t;

typedef struct {
    bool is_rx;
    hal_uart_port_t uart_port;
} uart_dma_callback_data_t;

typedef enum {
    UART_HWSTATUS_UNINITIALIZED,
    UART_HWSTATUS_POLL_INITIALIZED,
    UART_HWSTATUS_DMA_INITIALIZED,
} uart_hwstatus_t;

typedef struct {
    hal_uart_callback_t func;
    void *arg;
} uart_callback_t;

typedef enum {
    UART_FLOWCONTROL_NONE,
    UART_FLOWCONTROL_SOFTWARE,
    UART_FLOWCONTROL_HARDWARE,
} uart_flowcontrol_t;

typedef struct {
    uint8_t xon;
    uint8_t xoff;
    uint8_t escape_character;
} uart_sw_flowcontrol_config_t;

typedef struct {
    uint32_t uart_log_port;         // log_para 0
    uint32_t uart_baudrate;         // log_para 1
    uint32_t uart_rg_DL;            // log_para 2
    uint32_t uart_rg_IER;           // log_para 3
    uint32_t uart_rg_FCR;           // log_para 4
    uint32_t uart_rg_EFR;           // log_para 5
    uint32_t uart_rg_LCR;           // log_para 6
    uint32_t uart_rg_XOFF_XON;      // log_para 7
    uint32_t uart_rg_HIGHSPEED;     // log_para 8
    uint32_t uart_rg_SAMPLEREG;     // log_para 9
    uint32_t uart_rg_RATEFIX;       // log_para 10
    uint32_t uart_rg_GUARD;         // log_para 11
    uint32_t uart_rg_ESCAPEREG;     // log_para 12
    uint32_t uart_rg_SLEEPREG;      // log_para 13
    uint32_t uart_rg_DMACON;        // log_para 14
    uint32_t uart_rg_RXTRIG;        // log_para 15
    uint32_t uart_rg_FRACDIV;       // log_para 16
    uint32_t vdma_rg_tx_COUNT;      // log_para 17
    uint32_t vdma_rg_tx_CON;        // log_para 18
    uint32_t vdma_rg_tx_PGMADDR;    // log_para 19
    uint32_t vdma_rg_tx_ALTLEN;     // log_para 20
    uint32_t vdma_rg_tx_FFSIZE;     // log_para 21
    uint32_t vdma_rg_rx_COUNT;      // log_para 22
    uint32_t vdma_rg_rx_CON;        // log_para 23
    uint32_t vdma_rg_rx_PGMADDR;    // log_para 24
    uint32_t vdma_rg_rx_ALTLEN;     // log_para 25
    uint32_t vdma_rg_rx_FFSIZE;     // log_para 26
    uint32_t infra_reinit_flag;     // log_para 27
} uart_backup_para_for_log_t;       // address = 0x0425B264

/*internal API for uart to set DMA mode*/
void uart_enable_dma(UART_REGISTER_T *uartx);
void uart_dma_channel_to_callback_data(vdma_channel_t dma_channel, uart_dma_callback_data_t *user_data);
void uart_clear_timeout_interrupt(UART_REGISTER_T *uartx);

/*internal API for uart to set hardware*/
void uart_set_fifo(UART_REGISTER_T *uartx);                                // set hw fifo threadhold.
void uart_purge_fifo(UART_REGISTER_T *uartx, int32_t is_rx);               // clear data in hw fifo.
void uart_reset_default_value(UART_REGISTER_T *uartx);                     // set all register to zero.
void uart_set_format(UART_REGISTER_T *uartx,
                     hal_uart_word_length_t word_length,
                     hal_uart_stop_bit_t stop_bit,
                     hal_uart_parity_t parity);                            // set hw frame format.
void uart_set_timeout_value(UART_REGISTER_T *uartx, uint32_t value);       // set rx fifo timeout time,  the unit of value is ms.
void uart_wait_empty(hal_uart_port_t uart_port);                           // wait hw fifo empty.
void uart_set_baudrate(UART_REGISTER_T *uartx, uint32_t actual_baudrate);  // set uart baudrate.
void uart_set_auto_baudrate(UART_REGISTER_T *uartx, bool is_enable, uint32_t current_baudrate);

/*internal API for uart to query tx/rx is empty*/
bool uart_query_rx_empty(hal_uart_port_t uart_port);    // return true means empty.
bool uart_query_tx_empty(hal_uart_port_t uart_port);    // return true means empty.

/*internal API for uart to set polling mode*/
int uart_probe_char(UART_REGISTER_T *uartx);
void uart_put_char_block(UART_REGISTER_T *uartx, uint8_t byte);
uint8_t uart_get_char_block(UART_REGISTER_T *uartx);
uint32_t uart_get_char_unblocking(UART_REGISTER_T *uartx);

/*internal API for uart to set flow control*/
void uart_set_hardware_flowcontrol(UART_REGISTER_T *uartx);
void uart_set_software_flowcontrol(UART_REGISTER_T *uartx,
                                   uint8_t xon,
                                   uint8_t xoff,
                                   uint8_t escape_character);
void uart_disable_flowcontrol(UART_REGISTER_T *uartx);


#ifdef HAL_SLEEP_MANAGER_ENABLED
void uart_set_sleep_mode(UART_REGISTER_T *uartx);
void uart_set_sleep_idle_fc_mode(UART_REGISTER_T *uartx);
#endif

/*internal API for uart to address interrupt*/
uart_interrupt_type_t uart_query_interrupt_type(UART_REGISTER_T *uartx);
void uart_interrupt_handler(hal_nvic_irq_t irq_number);
int32_t uart_verify_error(UART_REGISTER_T *uartx);
void uart_send_handler(hal_uart_port_t uart_port, bool is_send_complete_trigger);
void uart_receive_handler(hal_uart_port_t uart_port, bool is_timeout);
void uart_error_handler(hal_uart_port_t uart_port);
void uart_transaction_done_handler(hal_uart_port_t uart_port);
void uart_unmask_send_interrupt(UART_REGISTER_T *uartx);
void uart_mask_receive_interrupt(UART_REGISTER_T *uartx);
void uart_unmask_receive_interrupt(UART_REGISTER_T *uartx);

#ifdef HAL_UART_FEATURE_FLOWCONTROL_CALLBACK
void uart_hw_flowcontrol_handler(hal_uart_port_t uart_port);
void uart_sw_flowcontrol_handler(hal_uart_port_t uart_port);
#endif

/*internal API for logging*/

hal_uart_status_t uart_backup_log_para_to_share_buf(hal_uart_port_t uart_port);
hal_uart_status_t uart_backup_register(hal_uart_port_t uart_port);
hal_uart_status_t uart_restore_register(hal_uart_port_t uart_port);
uint32_t uart_get_hw_rptr(hal_uart_port_t uart_port, int32_t is_rx);
uint32_t uart_get_hw_wptr(hal_uart_port_t uart_port, int32_t is_rx);
hal_uart_status_t uart_set_sw_move_byte(hal_uart_port_t uart_port, int32_t is_rx, uint16_t sw_move_byte);
bool uart_get_buf_full_status(hal_uart_port_t uart_port, int32_t is_rx);
hal_uart_status_t  uart_clear_vfifo_and_fifo(hal_uart_port_t uart_port);
void uart_disable_irq(hal_uart_port_t uart_port);
/*internal API for exception handler*/
uint32_t uart_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size);
uint32_t uart_exception_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif
#endif/*HAL_UART_MODULE_ENABLED*/

