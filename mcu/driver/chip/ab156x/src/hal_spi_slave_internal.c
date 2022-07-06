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

#include "hal_spi_slave.h"

#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
#include <string.h>
#include "hal_spi_slave_internal.h"
#include "hal_clock.h"
#include "hal_log.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"

#ifdef HAL_SPI_SLAVE_FEATURE_BYPASS
#ifdef HAL_SPI_MASTER_MODULE_ENABLED
extern uint32_t spi_master_get_status(hal_spi_master_port_t master_port);
#endif
#endif

static SPIS_REGISTER_T *const g_spi_slave_register[HAL_SPI_SLAVE_MAX] = {SPI_SLAVE_0};
static SPIS_PAD_REGISTER_T *const g_spi_slave_pad_register[HAL_SPI_SLAVE_MAX] = {SPI_SLAVE_PAD_0};
#ifdef HAL_SPI_SLAVE_FEATURE_BYPASS
static SPIS_BYPASS_REGISTER_T *const g_spi_slave_bypass_register[HAL_SPI_SLAVE_MAX] = {SPI_SLAVE_BYPASS_0};
#endif
static hal_spi_slave_fsm_status_t g_spi_slave_fsm[MAX_STATUS][MAX_OPERATION_CMD] = {
            /* POWER_OFF_CMD                             POWER_ON_CMD                         CONFIG_READ_CMD                       READ_CMD                               CONFIG_WRITE_CMD                      WRITE_CMD */
/*PWROFF_STA*/ {HAL_SPI_SLAVE_FSM_INVALID_OPERATION,     HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION, HAL_SPI_SLAVE_FSM_INVALID_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION,   HAL_SPI_SLAVE_FSM_INVALID_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION},
/*PWRON_STA */ {HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,     HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION,   HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,  HAL_SPI_SLAVE_FSM_INVALID_OPERATION},
/*CR_STA    */ {HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CR, HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION,   HAL_SPI_SLAVE_FSM_ERROR_CW_AFTER_CR,  HAL_SPI_SLAVE_FSM_ERROR_WRITE_AFTER_CR},
/*CW_STA    */ {HAL_SPI_SLAVE_FSM_ERROR_PWROFF_AFTER_CW, HAL_SPI_SLAVE_FSM_INVALID_OPERATION, HAL_SPI_SLAVE_FSM_ERROR_CR_AFTER_CW,  HAL_SPI_SLAVE_FSM_ERROR_READ_AFTER_CW, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION, HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION}
};
#ifdef HAL_SLEEP_MANAGER_ENABLED
static uint32_t g_spi_slave_ctrl_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_ie_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_tmout_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_pad_dummy_cnt_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_pad_cmd_def0_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_pad_cmd_def1_reg[HAL_SPI_SLAVE_MAX] = {0};
static uint32_t g_spi_slave_pad_cmd_def2_reg[HAL_SPI_SLAVE_MAX] = {0};
static sleep_management_lock_request_t g_spi_slave_sleep_handle[HAL_SPI_SLAVE_MAX] = {SLEEP_LOCK_SPI_SLV};
#endif

uint8_t g_last2now_status[2] = {PWROFF_STA, PWROFF_STA};

static inline void update_fsm_status(hal_spi_slave_transaction_status_t *transaction_status, \
    hal_spi_slave_callback_event_t int_status, spi_slave_fsm_status_t fsm_status, spi_slave_operation_cmd_t current_command) {
   transaction_status->interrupt_status = int_status;
   spi_slave_update_status(fsm_status);
   transaction_status->fsm_status = g_spi_slave_fsm[g_last2now_status[0]][current_command];
}

typedef void (*spi_slave_int_callback_t)(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data);

static void spi_slave_poweron_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;
#ifdef HAL_SLEEP_MANAGER_ENABLED
    /* after receive POWER-ON command, lock sleep */
    hal_sleep_manager_lock_sleep(g_spi_slave_sleep_handle[spi_port]);
#endif
    update_fsm_status(&status, HAL_SPI_SLAVE_EVENT_POWER_ON, PWRON_STA, POWER_ON_CMD);
    /* set slv_on bit here */
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_ON = SPIS_STA_SLV_ON_MASK;
    user_callback(status, user_data);
}

static void spi_slave_poweroff_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;
    update_fsm_status(&status, HAL_SPI_SLAVE_EVENT_POWER_OFF, PWROFF_STA, POWER_OFF_CMD);
    /* clear slv_on bit here */
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_ON &= (~SPIS_STA_SLV_ON_MASK);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    /* after spis de-init done, unlock sleep */
    hal_sleep_manager_unlock_sleep(g_spi_slave_sleep_handle[spi_port]);
#endif
    user_callback(status, user_data);
}

static void spi_slave_read_finish_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;
    update_fsm_status(&status, HAL_SPI_SLAVE_EVENT_RD_FINISH, PWRON_STA, READ_CMD);
    /* clear TX_DMA_SW_READY bit here */
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.TXDMA_SW_RDY = 0;
    user_callback(status, user_data);
}

static void spi_slave_write_finish_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;
    update_fsm_status(&status, HAL_SPI_SLAVE_EVENT_WR_FINISH, PWRON_STA, WRITE_CMD);
    /* clear RX_DMA_SW_READY bit here */
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.RXDMA_SW_RDY = 0;
    user_callback(status, user_data);
}

static void spi_slave_read_config_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;
    update_fsm_status(&status, HAL_SPI_SLAVE_EVENT_CRD_FINISH, CR_STA, CONFIG_READ_CMD);
    user_callback(status, user_data);
}

static void spi_slave_write_config_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;
    update_fsm_status(&status, HAL_SPI_SLAVE_EVENT_CWR_FINISH, CW_STA, CONFIG_WRITE_CMD);
    user_callback(status, user_data);
}

static void spi_slave_error_callback(hal_spi_slave_port_t spi_port)
{
    spi_slave_update_status(PWRON_STA);
    /* clear TX/RX_DMA_SW_READY bit here */
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.TXDMA_SW_RDY = 0;
    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.RXDMA_SW_RDY = 0;
}

static void spi_slave_read_error_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;

    spi_slave_error_callback(spi_port);
    status.interrupt_status = HAL_SPI_SLAVE_EVENT_RD_ERR;
    status.fsm_status = HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION;
    user_callback(status, user_data);
}

static void spi_slave_write_error_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;

    spi_slave_error_callback(spi_port);
    status.interrupt_status = HAL_SPI_SLAVE_EVENT_WR_ERR;
    status.fsm_status = HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION;
    user_callback(status, user_data);
}

static void spi_slave_timeout_error_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status;

    spi_slave_error_callback(spi_port);
    status.interrupt_status = HAL_SPI_SLAVE_EVENT_TIMEOUT_ERR;
    status.fsm_status = HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION;
    user_callback(status, user_data);
}

static void spi_slave_direct_mode_callback(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_event_t event, hal_spi_slave_callback_t user_callback, void *user_data)
{
    hal_spi_slave_transaction_status_t status = {.fsm_status = HAL_SPI_SLAVE_FSM_SUCCESS_OPERATION};
    status.interrupt_status = event;

    switch(event) {
    case HAL_SPI_SLAVE_EVENT_IDLE_TIMEOUT  :
        break;
    case HAL_SPI_SLAVE_EVENT_TX_DMA_EMPTY  :
        /* Only notify once to user, there is free space to send data. */
        g_spi_slave_register[spi_port]->IE &= ~SPIS_IE_TX_DMA_EMPTY_MASK;
        break;
    case HAL_SPI_SLAVE_EVENT_RX_DMA_FULL   :
        break;
    case HAL_SPI_SLAVE_EVENT_RX_OVERRUN:
        break;
    default:
        break;
    }

    if (user_callback) {
        user_callback(status, user_data);
    }
}
static spi_slave_int_callback_t spi_slave_int_callback[] = {
    spi_slave_read_finish_callback,
    spi_slave_write_finish_callback,
    spi_slave_poweroff_callback,
    spi_slave_poweron_callback,
    spi_slave_read_config_callback,
    spi_slave_write_config_callback,
    spi_slave_read_error_callback,
    spi_slave_write_error_callback,
    spi_slave_timeout_error_callback,
};

void spi_slave_lisr(hal_spi_slave_port_t spi_port, hal_spi_slave_callback_t user_callback, void *user_data)
{
    uint32_t irq_status;
    uint32_t shift_h;
    uint32_t shift_l;
    uint32_t i;
    uint32_t irq_event;

    if (g_spi_slave_register[spi_port]->CTRL_UNION.CTRL & SPIS_CTRL_DIRECT_MODE_MASK) {
        irq_status = g_spi_slave_register[spi_port]->INT & g_spi_slave_register[spi_port]->IE & SPIS_INT_DIRECT_MODE_MASK;
        irq_status >>= SPIS_INT_DIRECT_MODE_OFFSET;

        for (i = 0; irq_status; i++) {
            if (irq_status & (1 << i)) {
                irq_event = 1 << ( i + SPIS_INT_DIRECT_MODE_OFFSET);
                spi_slave_direct_mode_callback(spi_port, irq_event, user_callback, user_data);
                g_spi_slave_register[spi_port]->INT &= irq_event;
                irq_status &= ~(1 << i);
            }
        }
    } else {
        irq_status = g_spi_slave_register[spi_port]->INT & g_spi_slave_register[spi_port]->IE & SPIS_INT_MASK;
        /* regroup the priority of interrupts for subsequent processing. */
        shift_h = (irq_status & (SPIS_INT_RD_TRANS_FINISH_MASK | SPIS_INT_WR_TRANS_FINISH_MASK | SPIS_INT_POWER_ON_MASK | SPIS_INT_POWER_OFF_MASK)) >> 2;
        shift_l = (irq_status & (SPIS_INT_RD_CFG_FINISH_MASK | SPIS_INT_WR_CFG_FINISH_MASK)) << 4;
        irq_status = shift_h | shift_l | (irq_status & (SPIS_INT_RD_DATA_ERR_MASK | SPIS_INT_WR_DATA_ERR_MASK | SPIS_INT_TMOUT_ERR_MASK));

        /* because more than one interrupt may be raised at the same time, they must be processed one by one at a specify prority. */
        for (i = 0; irq_status; i++) {
            if (irq_status & (1 << i)) {
                spi_slave_int_callback[i](spi_port, user_callback, user_data);
                if (irq_status & shift_h) {
                    g_spi_slave_register[spi_port]->INT &= (1 << ( i + 2));
                } else if (irq_status & shift_l) {
                    /* As only the shift_l[5:4], shift_h[3:0] and irq_status[8:0] can be 1, when program runs here, the variable
                    i must be greater than 3 and less than 6. but, to avoid the warning from code static analysis tool, we do and 
                    operation with 0x1F to make sure left shifting is not more than 31 bits. */
                    g_spi_slave_register[spi_port]->INT &= (1 << ((i - 4) & 0x1F));
                } else {
                    g_spi_slave_register[spi_port]->INT &= (1 << i);
                }
                irq_status &= ~(1 << i);
            }
        }
    }
}

void spi_slave_init(hal_spi_slave_port_t spi_port, const hal_spi_slave_config_t *spi_config)
{
    /* reset spi slave's status frist */
    // g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_RST = 1;
    // g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_RST = 0;

    /* user configure parameters */
    switch (spi_config->bit_order) {
        case HAL_SPI_SLAVE_LSB_FIRST:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 &= (~(SPIS_CTRL_TXMSBF_MASK | SPIS_CTRL_RXMSBF_MASK));
            break;
        case HAL_SPI_SLAVE_MSB_FIRST:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 |= (SPIS_CTRL_TXMSBF_MASK | SPIS_CTRL_RXMSBF_MASK);
            break;
    }

    switch (spi_config->phase) {
        case HAL_SPI_SLAVE_CLOCK_PHASE0:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 &= (~SPIS_CTRL_CPHA_MASK);
            break;
        case HAL_SPI_SLAVE_CLOCK_PHASE1:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 |= SPIS_CTRL_CPHA_MASK;
            break;
    }

    switch (spi_config->polarity) {
        case HAL_SPI_SLAVE_CLOCK_POLARITY0:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 &= (~SPIS_CTRL_CPOL_MASK);
            break;
        case HAL_SPI_SLAVE_CLOCK_POLARITY1:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 |= SPIS_CTRL_CPOL_MASK;
            break;
    }

    /* timeout threshold */
    g_spi_slave_register[spi_port]->TMOUT_THR = spi_config->timeout_threshold;

    /* enable all interrupt, set four-byte address and size, set sw decode bit */
    g_spi_slave_register[spi_port]->IE |= SPIS_IE_MASK;
    g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL0 |= SPIS_CTRL_SIZE_OF_ADDR_MASK;
    g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.CTRL1 = (SPIS_CTRL_DEC_ADDR_EN_MASK >> 8) | (SPIS_CTRL_SW_RDY_EN_MASK >> 8);
    g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.DIR_MODE = 0;
}

hal_spi_slave_status_t spi_slave_send(hal_spi_slave_port_t spi_port, const uint8_t *data, uint32_t size)
{
    uint32_t config_size = 0;

    /* return HAL_SPI_SLAVE_STATUS_ERROR if config_size isn't equal to size. */
    config_size = g_spi_slave_register[spi_port]->TRANS_LENGTH;
    if (config_size != size) {
        log_hal_msgid_error("[SPIS%d][send]:size error.\r\n",1, spi_port);
        return HAL_SPI_SLAVE_STATUS_ERROR;
    } else {
        /* set src_buffer_addr, buffer_size as size and tx_fifo_ready. */
        g_spi_slave_register[spi_port]->BUFFER_BASE_ADDR = (uint32_t)data;
        g_spi_slave_register[spi_port]->BUFFER_SIZE = size;
        g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.TXDMA_SW_RDY = 1;
    }

    return HAL_SPI_SLAVE_STATUS_OK;
}

#ifdef HAL_SPI_SLAVE_FEATURE_DIRECT_MODE
uint32_t spi_slave_send_vfifo(hal_spi_slave_port_t spi_port, const uint8_t *data, uint32_t size)
{
    uint32_t write_address;
    uint32_t write_length;
    uint32_t bytes_to_boundary;

    if (g_spi_slave_register[spi_port]->TXDMA.BYTE_AVALIABLE == 0) {
        return 0;
    }
    if (g_spi_slave_register[spi_port]->TXDMA.PTR_UNION.PTR_CELLS.WPTR >= g_spi_slave_register[spi_port]->TXDMA.PTR_UNION.PTR_CELLS.RPTR) {
        bytes_to_boundary = g_spi_slave_register[spi_port]->TXDMA.SIZE_UNION.SIZE_CELLS.TX_SIZE + 1 - g_spi_slave_register[spi_port]->TXDMA.PTR_UNION.PTR_CELLS.WPTR;
    } else {
        bytes_to_boundary = g_spi_slave_register[spi_port]->TXDMA.PTR_UNION.PTR_CELLS.RPTR - g_spi_slave_register[spi_port]->TXDMA.PTR_UNION.PTR_CELLS.WPTR;
    }

    write_address = g_spi_slave_register[spi_port]->TXDMA.START_ADDR + g_spi_slave_register[spi_port]->TXDMA.PTR_UNION.PTR_CELLS.WPTR;

    if (size > g_spi_slave_register[spi_port]->TXDMA.BYTE_AVALIABLE) {
        write_length = g_spi_slave_register[spi_port]->TXDMA.BYTE_AVALIABLE;
    } else {
        write_length = size;
    }

    if (write_length > bytes_to_boundary) {
        memcpy((void*)write_address, data, bytes_to_boundary);
        write_address = g_spi_slave_register[spi_port]->TXDMA.START_ADDR;
        memcpy((void*)write_address, data + bytes_to_boundary, write_length - bytes_to_boundary);
    } else {
        memcpy((void*)write_address, data, write_length);
    }

    g_spi_slave_register[spi_port]->TXDMA.SW_MOVE_BYTE = write_length;
    g_spi_slave_register[spi_port]->IE |= SPIS_IE_TX_DMA_EMPTY_MASK;
    g_spi_slave_register[spi_port]->DMA_CTRL_UNION.DMA_CTRL_CELLS.TX_START = 1;

    return write_length;
}

uint32_t spi_slave_receive_vfifo(hal_spi_slave_port_t spi_port, uint8_t *data, uint32_t size)
{
    uint32_t read_address = 0;
    uint32_t read_length = 0;
    uint32_t bytes_to_boundary = 0;

    if (g_spi_slave_register[spi_port]->RXDMA.FIFO_CNT == 0) {
        return 0;
    }
    if (g_spi_slave_register[spi_port]->RXDMA.PTR_UNION.PTR_CELLS.RPTR > g_spi_slave_register[spi_port]->RXDMA.PTR_UNION.PTR_CELLS.WPTR) {
        bytes_to_boundary = g_spi_slave_register[spi_port]->RXDMA.SIZE_UNION.SIZE_CELLS.RX_SIZE + 1 - g_spi_slave_register[spi_port]->RXDMA.PTR_UNION.PTR_CELLS.RPTR;
    } else {
        bytes_to_boundary = g_spi_slave_register[spi_port]->RXDMA.PTR_UNION.PTR_CELLS.WPTR - g_spi_slave_register[spi_port]->RXDMA.PTR_UNION.PTR_CELLS.RPTR;
    }

    read_address = g_spi_slave_register[spi_port]->RXDMA.START_ADDR + g_spi_slave_register[spi_port]->RXDMA.PTR_UNION.PTR_CELLS.RPTR;

    if (size > g_spi_slave_register[spi_port]->RXDMA.FIFO_CNT) {
        read_length = g_spi_slave_register[spi_port]->RXDMA.FIFO_CNT;
    } else {
        read_length = size;
    }

    if (read_length > bytes_to_boundary) {
        memcpy(data, (void*)read_address, bytes_to_boundary);
        read_address = g_spi_slave_register[spi_port]->RXDMA.START_ADDR;
        memcpy(data + bytes_to_boundary, (void*)read_address, read_length - bytes_to_boundary);
    } else {
        memcpy(data, (void*)read_address, read_length);
    }

    g_spi_slave_register[spi_port]->RXDMA.SW_MOVE_BYTE = read_length;

    return read_length;
}

void spi_slave_set_vfifo(hal_spi_slave_port_t spi_port, hal_spi_slave_vfifo_config_t *config)
{
    g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.DIR_MODE = 1;

    /* Disable all interrupts in commond mode. */
    g_spi_slave_register[spi_port]->IE &= ~SPIS_IE_MASK;

    g_spi_slave_register[spi_port]->IE |= SPIS_IE_IDLE_TMOUT_MASK;

    if (config->tx_buffer || config->rx_buffer) {
        if (config->tx_buffer) {
            g_spi_slave_register[spi_port]->TXDMA.START_ADDR = (uint32_t)config->tx_buffer;
            g_spi_slave_register[spi_port]->TXDMA.SIZE_UNION.SIZE_CELLS.TX_SIZE = config->tx_buffer_size - 1;
            g_spi_slave_register[spi_port]->TXDMA.SIZE_UNION.SIZE_CELLS.TX_THRESHOLD = config->tx_threshold;
            g_spi_slave_register[spi_port]->DMA_CTRL_UNION.DMA_CTRL_CELLS.TX_TRANS_TYPE = config->transfer_type;
            g_spi_slave_register[spi_port]->DMA_CTRL_UNION.DMA_CTRL_CELLS.TX_START = 1;
            g_spi_slave_register[spi_port]->IE |= SPIS_IE_TX_DMA_EMPTY_MASK;
        }

        if (config->rx_buffer) {
            g_spi_slave_register[spi_port]->RXDMA.START_ADDR = (uint32_t)config->rx_buffer;
            g_spi_slave_register[spi_port]->RXDMA.SIZE_UNION.SIZE_CELLS.RX_SIZE = config->rx_buffer_size - 1;
            g_spi_slave_register[spi_port]->RXDMA.SIZE_UNION.SIZE_CELLS.RX_THRESHOLD = config->rx_threshold;
            g_spi_slave_register[spi_port]->DMA_CTRL_UNION.DMA_CTRL_CELLS.RX_TRANS_TYPE = config->transfer_type;
            g_spi_slave_register[spi_port]->DMA_CTRL_UNION.DMA_CTRL_CELLS.RX_START = 1;
            g_spi_slave_register[spi_port]->IE |= (SPIS_IE_RX_OVERRUN_MASK | SPIS_IE_RX_DMA_FULL_MASK);
        }
    } else {
        g_spi_slave_register[spi_port]->FIFO_CTRL_UNION.FIFO_CTRL_CELLS.TX_THRESHOLD = config->tx_threshold;
        g_spi_slave_register[spi_port]->FIFO_CTRL_UNION.FIFO_CTRL_CELLS.RX_THRESHOLD = config->rx_threshold;
        g_spi_slave_register[spi_port]->IE |= (SPIS_IE_TX_FIFO_EMPTY_MASK | SPIS_IE_RX_FIFO_FULL_MASK | SPIS_IE_RX_OVERRUN_MASK);
    }
}

uint32_t spi_slave_get_vfifo_available_send_space(hal_spi_slave_port_t spi_port)
{
    return g_spi_slave_register[spi_port]->TXDMA.BYTE_AVALIABLE;
}

uint32_t spi_slave_get_vfifo_available_data_bytes(hal_spi_slave_port_t spi_port)
{
    return g_spi_slave_register[spi_port]->RXDMA.FIFO_CNT;
}
#endif

hal_spi_slave_status_t spi_slave_query_config_info(hal_spi_slave_port_t spi_port, uint32_t *address, uint32_t *length)
{
    if ((g_spi_slave_register[spi_port]->STA_UNION.STA_CELLS.STA & 0xff) != (SPIS_STA_CFG_SUCCESS_MASK | SPIS_STA_SLV_ON_MASK)) {
        return HAL_SPI_SLAVE_STATUS_ERROR;
    }

    *address = g_spi_slave_register[spi_port]->TRANS_ADDR;
    *length = g_spi_slave_register[spi_port]->TRANS_LENGTH;

    return HAL_SPI_SLAVE_STATUS_OK;
}

hal_spi_slave_status_t spi_slave_receive(hal_spi_slave_port_t spi_port, uint8_t *buffer, uint32_t size)
{
    uint32_t config_size = 0;

    /* return HAL_SPI_SLAVE_STATUS_ERROR if config_size isn't equal to size */
    config_size = g_spi_slave_register[spi_port]->TRANS_LENGTH;
    if (config_size != size) {
        log_hal_msgid_error("[SPIS%d][receive]:size error.\r\n",1, spi_port);
        return HAL_SPI_SLAVE_STATUS_ERROR;
    } else {
        /* set src_buffer_addr, buffer_size as size and rx_fifo_ready. */
        g_spi_slave_register[spi_port]->BUFFER_BASE_ADDR = (uint32_t)buffer;
        g_spi_slave_register[spi_port]->BUFFER_SIZE = size;
        g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.RXDMA_SW_RDY = 1;
    }

    return HAL_SPI_SLAVE_STATUS_OK;
}

void spi_slave_set_early_miso(hal_spi_slave_port_t spi_port, hal_spi_slave_early_miso_t early_miso)
{
    switch (early_miso) {
        case HAL_SPI_SLAVE_EARLY_MISO_DISABLE:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.MISO_EARLY_TRANS = 0;
            break;
        case HAL_SPI_SLAVE_EARLY_MISO_ENABLE:
            g_spi_slave_register[spi_port]->CTRL_UNION.CTRL_CELLS.MISO_EARLY_TRANS = 1;
            break;
    }
}

void spi_slave_set_command(hal_spi_slave_port_t spi_port, hal_spi_slave_command_type_t command, uint8_t value)
{
    uint32_t reg;

    switch (command) {
        case HAL_SPI_SLAVE_CMD_WS:
        case HAL_SPI_SLAVE_CMD_RS:
        case HAL_SPI_SLAVE_CMD_POWERON:
        case HAL_SPI_SLAVE_CMD_POWEROFF:
            reg = g_spi_slave_pad_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0;
            break;
        case HAL_SPI_SLAVE_CMD_WR:
        case HAL_SPI_SLAVE_CMD_RD:
        case HAL_SPI_SLAVE_CMD_CW:
        case HAL_SPI_SLAVE_CMD_CR:
            reg = g_spi_slave_pad_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1;
            break;
        case HAL_SPI_SLAVE_CMD_CT:
            reg = g_spi_slave_pad_register[spi_port]->CMD_DEF2_UNION.CMD_DEF2;
            break;
        default:
            reg = g_spi_slave_pad_register[spi_port]->CMD_DEF2_UNION.CMD_DEF2;
            break;
    }

    switch (command) {
        case HAL_SPI_SLAVE_CMD_WS:
            reg &= ~SPIS_CMD_DEF0_WS_MASK;
            reg |= value << SPIS_CMD_DEF0_WS_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0 = reg;
            break;
        case HAL_SPI_SLAVE_CMD_RS:
            reg &= ~SPIS_CMD_DEF0_RS_MASK;
            reg |= value << SPIS_CMD_DEF0_RS_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0 = reg;
            break;
        case HAL_SPI_SLAVE_CMD_WR:
            reg &= ~SPIS_CMD_DEF1_WR_MASK;
            reg |= value << SPIS_CMD_DEF1_WR_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1 = reg;
            break;
        case HAL_SPI_SLAVE_CMD_RD:
            reg &= ~SPIS_CMD_DEF1_RD_MASK;
            reg |= value << SPIS_CMD_DEF1_RD_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1 = reg;
            break;
        case HAL_SPI_SLAVE_CMD_POWEROFF:
            reg &= ~SPIS_CMD_DEF0_POWEROFF_MASK;
            reg |= value << SPIS_CMD_DEF0_POWEROFF_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0 = reg;
            break;
        case HAL_SPI_SLAVE_CMD_POWERON:
            reg &= ~SPIS_CMD_DEF0_POWERON_MASK;
            reg |= value << SPIS_CMD_DEF0_POWERON_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0 = reg;
            break;
        case HAL_SPI_SLAVE_CMD_CW:
            reg &= ~SPIS_CMD_DEF1_CW_MASK;
            reg |= value << SPIS_CMD_DEF1_CW_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1 = reg;
            break;
        case HAL_SPI_SLAVE_CMD_CR:
            reg &= ~SPIS_CMD_DEF1_CR_MASK;
            reg |= value << SPIS_CMD_DEF1_CR_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1 = reg;
            break;
        case HAL_SPI_SLAVE_CMD_CT:
            reg &= ~SPIS_CMD_DEF2_CMD_CT_MASK;
            reg |= value << SPIS_CMD_DEF2_CMD_CT_OFFSET;
            g_spi_slave_pad_register[spi_port]->CMD_DEF2_UNION.CMD_DEF2 = reg;
            break;
    }
}

void spi_slave_reset_default(hal_spi_slave_port_t spi_port)
{
    uint32_t int_status;

    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_RST = 1;

    g_spi_slave_register[spi_port]->CTRL_UNION.CTRL = 0x00000100;
    int_status = g_spi_slave_register[spi_port]->INT;
    int_status = int_status;
    g_spi_slave_register[spi_port]->IE = 0;
    g_spi_slave_register[spi_port]->TMOUT_THR = 0x000000ff;
    g_spi_slave_register[spi_port]->BUFFER_BASE_ADDR = 0;
    g_spi_slave_register[spi_port]->BUFFER_SIZE = 0;
    g_spi_slave_pad_register[spi_port]->PAD_DUMMY_CNT = 0;
    g_spi_slave_pad_register[spi_port]->CMD_DEF0_UNION.CMD_DEF0 = 0x08060402;
    g_spi_slave_pad_register[spi_port]->CMD_DEF1_UNION.CMD_DEF1 = 0x0e810c0a;
    g_spi_slave_pad_register[spi_port]->CMD_DEF2_UNION.CMD_DEF2 = 0x00000010;
    g_spi_slave_pad_register[spi_port]->DLYSEL0_UNION.DLYSEL0 = 0;
    g_spi_slave_pad_register[spi_port]->DLYSEL1_UNION.DLYSEL1 = 0;
    g_spi_slave_pad_register[spi_port]->DLYSEL2_UNION.DLYSEL2 = 0;

    g_spi_slave_register[spi_port]->TRIG_UNION.TRIG_CELLS.SW_ON = 0;
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
void spi_slave_backup_register_callback(void *data)
{
    hal_spi_slave_port_t slave_port;

    for (slave_port = HAL_SPI_SLAVE_0; slave_port < HAL_SPI_SLAVE_MAX; slave_port++) {
        /* backup related spi_slave register values */
        g_spi_slave_ctrl_reg[slave_port] = g_spi_slave_register[slave_port]->CTRL_UNION.CTRL;
        g_spi_slave_ie_reg[slave_port] = g_spi_slave_register[slave_port]->IE;
        g_spi_slave_tmout_reg[slave_port] = g_spi_slave_register[slave_port]->TMOUT_THR;
        g_spi_slave_pad_dummy_cnt_reg[slave_port] = g_spi_slave_pad_register[slave_port]->PAD_DUMMY_CNT;
        g_spi_slave_pad_cmd_def0_reg[slave_port] = g_spi_slave_pad_register[slave_port]->CMD_DEF0_UNION.CMD_DEF0;
        g_spi_slave_pad_cmd_def1_reg[slave_port] = g_spi_slave_pad_register[slave_port]->CMD_DEF1_UNION.CMD_DEF1;
        g_spi_slave_pad_cmd_def2_reg[slave_port] = g_spi_slave_pad_register[slave_port]->CMD_DEF2_UNION.CMD_DEF2;
    }
}

void spi_slave_restore_register_callback(void *data)
{
    hal_spi_slave_port_t slave_port;

    for (slave_port = HAL_SPI_SLAVE_0; slave_port < HAL_SPI_SLAVE_MAX; slave_port++) {
        /* restore related spi_slave register values */
        g_spi_slave_register[slave_port]->CTRL_UNION.CTRL = g_spi_slave_ctrl_reg[slave_port];
        g_spi_slave_register[slave_port]->IE = g_spi_slave_ie_reg[slave_port];
        g_spi_slave_register[slave_port]->TMOUT_THR = g_spi_slave_tmout_reg[slave_port];
        g_spi_slave_pad_register[slave_port]->PAD_DUMMY_CNT = g_spi_slave_pad_dummy_cnt_reg[slave_port];
        g_spi_slave_pad_register[slave_port]->CMD_DEF0_UNION.CMD_DEF0 = g_spi_slave_pad_cmd_def0_reg[slave_port];
        g_spi_slave_pad_register[slave_port]->CMD_DEF1_UNION.CMD_DEF1 = g_spi_slave_pad_cmd_def1_reg[slave_port];
        g_spi_slave_pad_register[slave_port]->CMD_DEF2_UNION.CMD_DEF2 = g_spi_slave_pad_cmd_def2_reg[slave_port];
    }
}
#endif


#ifdef HAL_SPI_SLAVE_FEATURE_BYPASS
hal_spi_slave_status_t spi_slave_config_bypass(hal_spi_slave_port_t spi_port, const hal_spi_slave_bypass_config_t *bypass_config)
{
    uint32_t tmp;

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
    if (0 == spi_master_get_status(bypass_config->port)) {
        return HAL_SPI_SLAVE_STATUS_ERROR_BUSY;
    }
#endif
    tmp = g_spi_slave_bypass_register[spi_port]->BYPASS_CTRL;
    tmp &= ~(SPIS_BYPASS_CS_ENABLE_MASK << SPIS_BYPASS_CS0_ENABLE_OFFSET);
    tmp |= 1 << (SPIS_BYPASS_CS0_ENABLE_OFFSET + bypass_config->cs);

    switch(bypass_config->mode) {
    case HAL_SPI_SLAVE_BYPASS_MODE_W:
        tmp |= SPIS_BYPASS_SIO_ENABLE_MASK;
        break;
    case HAL_SPI_SLAVE_BYPASS_MODE_R:
        tmp &= ~SPIS_BYPASS_SIO_ENABLE_MASK;
        break;
    case HAL_SPI_SLAVE_BYPASS_MODE_RW:
        tmp &= ~SPIS_BYPASS_SIO_ENABLE_MASK;
        tmp |= SPIS_BYPASS_MOSI_ENABLE_MASK;
        break;
    }

    tmp &= ~SPIS_BYPASS_MASTER_SELECT_MASK;
    tmp |= (SPIS_BYPASS_MASTER_SELECT_MASK & ( bypass_config->port << SPIS_BYPASS_MASTER_SELECT_OFFSET));

    g_spi_slave_bypass_register[spi_port]->BYPASS_CTRL = tmp;
    return HAL_SPI_SLAVE_STATUS_OK;
}

void spi_slave_enable_bypass(hal_spi_slave_port_t spi_port)
{
    g_spi_slave_bypass_register[spi_port]->BYPASS_CTRL |= SPIS_BYPASS_ENABLE_MASK;
}

void spi_slave_disable_bypass(hal_spi_slave_port_t spi_port)
{
    g_spi_slave_bypass_register[spi_port]->BYPASS_CTRL &= ~SPIS_BYPASS_ENABLE_MASK;
}
#endif
#endif /*HAL_SPI_SLAVE_MODULE_ENABLED*/

