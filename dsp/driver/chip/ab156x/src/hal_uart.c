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

#include "hal_uart.h"
#include "hal_uart_internal.h"
#ifdef HAL_UART_MODULE_ENABLED
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_spm.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif
#include "hal_resource_assignment.h"
#include "hal_pdma_internal.h"

#ifdef __cplusplus
extern "C" {
#endif


UART_REGISTER_T *const g_uart_regbase[HAL_UART_MAX] = {UART0, UART1, UART2};
VDMA_REGISTER_T *const g_vdma_regbase[HAL_UART_MAX][2] = {{VDMA_RG_UART0_TX_BASE, VDMA_RG_UART0_RX_BASE},
                                                          {VDMA_RG_UART1_TX_BASE, VDMA_RG_UART1_RX_BASE},
                                                          {VDMA_RG_UART2_TX_BASE, VDMA_RG_UART2_RX_BASE}};


uint32_t uart_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size);

vdma_channel_t  uart_port_to_dma_map[2][3] =
{
    {VDMA_UART0TX, VDMA_UART1TX, VDMA_UART2TX},
    {VDMA_UART0RX, VDMA_UART1RX, VDMA_UART2RX},
};

#define     uart_port_to_dma_channel(uart_port,is_rx)   (uart_port_to_dma_map[is_rx][uart_port])


#ifdef HAL_SLEEP_MANAGER_ENABLED

void uart_backup_all_registers(void)
{

}

extern void mux_restore_callback();
void uart_restore_all_registers(void)
{
    hal_uart_port_t g_uart_port_for_logging = HAL_UART_MAX;
    UART_REGISTER_T *uartx;
    VDMA_REGISTER_T *dma_tx;
    VDMA_REGISTER_T *dma_rx;
    uart_backup_para_for_log_t *log_para;

    log_para = (uart_backup_para_for_log_t *)HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_UART_VAR_START;
    g_uart_port_for_logging = (hal_uart_port_t)log_para->uart_log_port;
    uartx  = g_uart_regbase[g_uart_port_for_logging];
    dma_tx = g_vdma_regbase[g_uart_port_for_logging][0];
    dma_rx = g_vdma_regbase[g_uart_port_for_logging][1];

    // VDMA_CLOCK enable TX and RX chanel UART0
    *(volatile uint32_t *)(0xA0040074) |= 0x3;
    //global top interrupt enable
    *(volatile uint32_t *)(0xA004000C) |= 0x3;

    // VDMA_CLOCK enable TX and RX chanel  UART1/2
    *(volatile uint32_t *)(0xA30C0074) |= 0xf;
    //global top interrupt enable
    *(volatile uint32_t *)(0xA30C000C) |= 0xf;

    while (hal_hw_semaphore_take(HW_SEMAPHORE_SLEEP) != HAL_HW_SEMAPHORE_STATUS_OK);
    if(SPM_INFRA_OFF_FLAG != 0) {
        SPM_INFRA_OFF_FLAG = 0;
        mux_restore_callback(g_uart_port_for_logging);
        /*restore UART register from share memory*/
        uartx->DLM_DLL                        = log_para->uart_rg_DL            ;
        uartx->FCR_UNION.FCR                  = log_para->uart_rg_FCR           ;
        uartx->LCR_UNION.LCR                  = log_para->uart_rg_LCR           ;
        uartx->HIGHSPEED                      = log_para->uart_rg_HIGHSPEED     ;
        uartx->SAMPLE_REG_UNION.SAMPLE_REG    = log_para->uart_rg_SAMPLEREG     ;
        uartx->RATEFIX_UNION.RATEFIX          = log_para->uart_rg_RATEFIX       ;
        uartx->GUARD                          = log_para->uart_rg_GUARD         ;
        uartx->SLEEP_REG                      = log_para->uart_rg_SLEEPREG      ;
        uartx->IER_UNION.IER                  = log_para->uart_rg_IER           ;
        uartx->IER_UNION.IER                 &= (~UART_IER_ETBEI_MASK);
        uartx->RXTRIG                         = log_para->uart_rg_RXTRIG        ;
        uartx->FRACDIV                        = log_para->uart_rg_FRACDIV       ;
        uartx->EFR_UNION.EFR                  = log_para->uart_rg_EFR           ;
        uartx->XON_XOFF_UNION.XON_XOFF        = log_para->uart_rg_XOFF_XON      ;
        uartx->ESCAPE_REG_UNION.ESCAPE_REG    = log_para->uart_rg_ESCAPEREG     ;
        uartx->FCR_UNION.FCR |=  UART_FCR_CLRT_MASK | UART_FCR_CLRR_MASK        ; // clear uart hw fifo
        /*restore DMA TX channel register from share memory*/
        dma_tx->VDMA_ACKINT                  |= VDMA_ACKINT_BIT_MASK            ;
        dma_tx->VDMA_COUNT                    = log_para->vdma_rg_tx_COUNT      ;
        dma_tx->VDMA_CON_UNION.VDMA_CON       = log_para->vdma_rg_tx_CON        ;
        dma_tx->VDMA_PGMADDR                  = log_para->vdma_rg_tx_PGMADDR    ;
        dma_tx->VDMA_ALTLEN                   = log_para->vdma_rg_tx_ALTLEN     ;
        dma_tx->VDMA_FFSIZE                   = log_para->vdma_rg_tx_FFSIZE     ;
        dma_tx->VDMA_START                   |= VDMA_START_BIT_MASK             ;
        /*restore DMA TX channel register from share memory*/
        dma_rx->VDMA_ACKINT                  |= VDMA_ACKINT_BIT_MASK            ;
        dma_rx->VDMA_COUNT                    = log_para->vdma_rg_rx_COUNT      ;
        dma_rx->VDMA_CON_UNION.VDMA_CON       = log_para->vdma_rg_rx_CON        ;
        dma_rx->VDMA_PGMADDR                  = log_para->vdma_rg_rx_PGMADDR    ;
        dma_rx->VDMA_ALTLEN                   = log_para->vdma_rg_rx_ALTLEN     ;
        dma_rx->VDMA_FFSIZE                   = log_para->vdma_rg_rx_FFSIZE     ;
        dma_rx->VDMA_START                   |= VDMA_START_BIT_MASK             ;

        uartx->DMA_CON_UNION.DMA_CON          = log_para->uart_rg_DMACON        ; //enable UART DMA mode after DMA configuration
    }
    while (hal_hw_semaphore_give(HW_SEMAPHORE_SLEEP) != HAL_HW_SEMAPHORE_STATUS_OK);
}
#endif //HAL_SLEEP_MANAGER_ENABLED

uint32_t uart_get_hw_rptr(hal_uart_port_t uart_port, int32_t is_rx)
{
    vdma_channel_t dma_channel;
    uint32_t read_pointer, read_offset;

    uart_backup_para_for_log_t *log_para = (uart_backup_para_for_log_t *)HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_UART_VAR_START;
    dma_channel = uart_port_to_dma_channel(uart_port, is_rx);
    vdma_get_hw_read_point(dma_channel, &read_pointer);
    if (is_rx) {
        read_offset = read_pointer - log_para->vdma_rg_rx_PGMADDR;
    } else {
        read_offset = read_pointer - log_para->vdma_rg_tx_PGMADDR;
    }

    return read_offset;
}

hal_uart_status_t uart_set_sw_move_byte(hal_uart_port_t uart_port, int32_t is_rx, uint16_t sw_move_byte)
{
    vdma_status_t status;
    vdma_channel_t dma_channel;

    dma_channel = uart_port_to_dma_channel(uart_port, is_rx);
    status = vdma_set_sw_move_byte(dma_channel, sw_move_byte);
    if (status != VDMA_OK) {
        return HAL_UART_STATUS_ERROR_PARAMETER;
    }
    return HAL_UART_STATUS_OK;
}

uint32_t uart_get_hw_wptr(hal_uart_port_t uart_port, int32_t is_rx)
{
    vdma_channel_t dma_channel;
    uint32_t write_pointer, write_offset;
    uart_backup_para_for_log_t *log_para = (uart_backup_para_for_log_t *)HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_UART_VAR_START;

    dma_channel = uart_port_to_dma_channel(uart_port, is_rx);
    vdma_get_hw_write_point(dma_channel, &write_pointer);
    if (is_rx) {
        write_offset = write_pointer - log_para->vdma_rg_rx_PGMADDR;
    } else {
        write_offset = write_pointer - log_para->vdma_rg_tx_PGMADDR;
    }

    return write_offset;
}


uint32_t uart_send_polling(hal_uart_port_t uart_port, const uint8_t *data, uint32_t size)
{
    uint32_t i = 0;
    uint32_t LSR;
    UART_REGISTER_T *uartx;
    uartx = g_uart_regbase[uart_port];

    for (i = 0; i < size; i++) {
        while (1) {
            LSR = uartx->LSR;
            if (LSR & UART_LSR_THRE_MASK) {
                uartx->THR = *data;
                break;
            }
        }
        data++;
    }
    return size;
}

#ifdef __cplusplus
}
#endif

#endif



