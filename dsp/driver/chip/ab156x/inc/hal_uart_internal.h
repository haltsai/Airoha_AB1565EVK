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

#include "hal_pdma_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

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
} uart_backup_para_for_log_t;

vdma_channel_t uart_port_to_dma_channel(hal_uart_port_t uart_port, int32_t is_rx);

uint32_t uart_get_hw_rptr(hal_uart_port_t uart_port, int32_t is_rx);
hal_uart_status_t uart_set_sw_move_byte(hal_uart_port_t uart_port, int32_t is_rx, uint16_t sw_move_byte);
uint32_t uart_get_hw_wptr(hal_uart_port_t uart_port, int32_t is_rx);
uint32_t uart_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size);

#ifdef __cplusplus
}
#endif

#endif

#endif

