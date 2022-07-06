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

#include <string.h>
#include "memory_attribute.h"
#include "hal_pdma_internal.h"
#include "hal_nvic.h"
#include "hal_clock.h"
#ifdef HAL_CACHE_MODULE_ENABLED
#include "hal_cache.h"
#endif
#include "hal_log.h"

#if 0
#ifndef __UBL__
#include <assert.h>
#else
#define assert(expr) log_hal_error("assert\r\n")
#endif
#endif

#define assert(expr)

#ifndef NULL
#define NULL ((void *)(0))
#endif

/* Full DMA channel list table */
typedef enum {
    SLOW_DMA_0_CHANNEL_BEGIN,
    SLOW_DMA_0_CHANNEL_UART0_TX = SLOW_DMA_0_CHANNEL_BEGIN,
    SLOW_DMA_0_CHANNEL_UART0_RX,
    SLOW_DMA_0_CHANNEL_I2C0_TX,
    SLOW_DMA_0_CHANNEL_I2C0_RX,
    SLOW_DMA_0_CHANNEL_END = SLOW_DMA_0_CHANNEL_I2C0_RX,

    SLOW_DMA_1_CHANNEL_BEGIN,
    SLOW_DMA_1_CHANNEL_UART1_TX = SLOW_DMA_1_CHANNEL_BEGIN,
    SLOW_DMA_1_CHANNEL_UART1_RX,
    SLOW_DMA_1_CHANNEL_UART2_TX,
    SLOW_DMA_1_CHANNEL_UART2_RX,
    SLOW_DMA_1_CHANNEL_I2C1_TX,
    SLOW_DMA_1_CHANNEL_I2C1_RX,
    SLOW_DMA_1_CHANNEL_I2C2_TX,
    SLOW_DMA_1_CHANNEL_I2C2_RX,
    SLOW_DMA_1_CHANNEL_END = SLOW_DMA_1_CHANNEL_I2C2_RX,

    I2S_DMA_CHANNEL_BEGIN,
    I2S_DMA_CHANNEL_I2S0_TX = I2S_DMA_CHANNEL_BEGIN,
    I2S_DMA_CHANNEL_I2S0_RX,
    I2S_DMA_CHANNEL_I2S1_TX,
    I2S_DMA_CHANNEL_I2S1_RX,
    I2S_DMA_CHANNEL_I2S2_TX,
    I2S_DMA_CHANNEL_I2S2_RX,
    I2S_DMA_CHANNEL_I2S3_TX,
    I2S_DMA_CHANNEL_I2S3_RX,
    I2S_DMA_CHANNEL_I2S4_TX,
    I2S_DMA_CHANNEL_I2S4_RX,
    I2S_DMA_CHANNEL_I2S5_TX,
    I2S_DMA_CHANNEL_I2S5_RX,
    I2S_DMA_CHANNEL_I2S6_TX,
    I2S_DMA_CHANNEL_I2S6_RX,
    I2S_DMA_CHANNEL_I2S7_TX,
    I2S_DMA_CHANNEL_I2S7_RX,
    I2S_DMA_CHANNEL_END = I2S_DMA_CHANNEL_I2S7_RX,
} dma_channel_t;

static void slow_dma_0_interrupt_hander(hal_nvic_irq_t irq_number);
static void slow_dma_1_interrupt_hander(hal_nvic_irq_t irq_number);
static void i2s_dma_interrupt_hander(hal_nvic_irq_t irq_number);

static void internal_dma_register_irq_callback(dma_channel_t dma_channel)
{
    uint32_t irq_status;
    static bool slow_dma_0_register_done = false;
    static bool slow_dma_1_register_done = false;
    static bool i2s_dma_register_done = false;

    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    if (dma_channel <= SLOW_DMA_0_CHANNEL_END) {
        if (slow_dma_0_register_done == false) {
            hal_nvic_register_isr_handler(SLOW_DMA0_IRQn, slow_dma_0_interrupt_hander);
            hal_nvic_enable_irq(SLOW_DMA0_IRQn);
            slow_dma_0_register_done = true;
        }
    } else if ((dma_channel >= SLOW_DMA_1_CHANNEL_BEGIN) && (dma_channel <= SLOW_DMA_1_CHANNEL_END)) {
        if (slow_dma_1_register_done == false) {
            hal_nvic_register_isr_handler(SLOW_DMA1_IRQn, slow_dma_1_interrupt_hander);
            hal_nvic_enable_irq(SLOW_DMA1_IRQn);
            slow_dma_1_register_done = true;
        }
    } else {
        if (i2s_dma_register_done == false) {
            hal_nvic_register_isr_handler(I2S_SLAVE_IRQn, i2s_dma_interrupt_hander);
            hal_nvic_enable_irq(I2S_SLAVE_IRQn);
            i2s_dma_register_done = true;
        }
    }
    hal_nvic_restore_interrupt_mask(irq_status);
}

static void internal_dma_enable_clock(dma_channel_t dma_channel)
{
    uint32_t domain_offset;
    static bool slow_dma_0_pdn_on = false;
    static bool slow_dma_1_pdn_on = false;
    static bool i2s_dma_pdn_on = false;

    if (dma_channel <= SLOW_DMA_0_CHANNEL_END) {
        domain_offset = dma_channel - SLOW_DMA_0_CHANNEL_BEGIN;
        SLOW_DMA_0_RG_GLB_CLK_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
        if (slow_dma_0_pdn_on == false) {
            ///TODO:: add clock relative code here
            hal_clock_enable(HAL_CLOCK_CG_SLOW_DMA_0);
            //*(volatile uint32_t *)(0xA2030B20) = (1 << 4);
            slow_dma_0_pdn_on = true;
        }
    } else if ((dma_channel >= SLOW_DMA_1_CHANNEL_BEGIN) && (dma_channel <= SLOW_DMA_1_CHANNEL_END)) {
        domain_offset = dma_channel - SLOW_DMA_1_CHANNEL_BEGIN;
        SLOW_DMA_1_RG_GLB_CLK_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
        if (slow_dma_1_pdn_on == false) {
            ///TODO:: add clock relative code here
            hal_clock_enable(HAL_CLOCK_CG_SLOW_DMA_1);
            //*(volatile uint32_t *)(0xA2030B20) = (1 << 4);
            slow_dma_1_pdn_on = true;
        }
    } else {
        domain_offset = dma_channel - I2S_DMA_CHANNEL_BEGIN;
        I2S_DMA_RG_GLB_CLK_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
        if (i2s_dma_pdn_on == false) {
            ///TODO:: add clock relative code here
            //hal_clock_enable();
            //*(volatile uint32_t *)(0xA2030B20) = (1 << 4);
            i2s_dma_pdn_on = true;
        }
    }
}

static void internal_dma_disable_clock(dma_channel_t dma_channel)
{
    uint32_t domain_offset;

    if (dma_channel <= SLOW_DMA_0_CHANNEL_END) {
        domain_offset = dma_channel - SLOW_DMA_0_CHANNEL_BEGIN;
        SLOW_DMA_0_RG_GLB_CLK_CLR = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    } else if ((dma_channel >= SLOW_DMA_1_CHANNEL_BEGIN) && (dma_channel <= SLOW_DMA_1_CHANNEL_END)) {
        domain_offset = dma_channel - SLOW_DMA_1_CHANNEL_BEGIN;
        SLOW_DMA_1_RG_GLB_CLK_CLR = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    } else {
        domain_offset = dma_channel - I2S_DMA_CHANNEL_BEGIN;
        I2S_DMA_RG_GLB_CLK_CLR = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    }
}

static void internal_dma_enable_irq(dma_channel_t dma_channel)
{
    uint32_t domain_offset;

    if (dma_channel <= SLOW_DMA_0_CHANNEL_END) {
        domain_offset = dma_channel - SLOW_DMA_0_CHANNEL_BEGIN;
        SLOW_DMA_0_RG_GLB_CPU0_INT_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    } else if ((dma_channel >= SLOW_DMA_1_CHANNEL_BEGIN) && (dma_channel <= SLOW_DMA_1_CHANNEL_END)) {
        domain_offset = dma_channel - SLOW_DMA_1_CHANNEL_BEGIN;
        SLOW_DMA_1_RG_GLB_CPU0_INT_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    } else {
        domain_offset = dma_channel - I2S_DMA_CHANNEL_BEGIN;
        I2S_DMA_RG_GLB_CPU0_INT_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    }
}

static void internal_dma_disable_irq(dma_channel_t dma_channel)
{
    uint32_t domain_offset;

    if (dma_channel <= SLOW_DMA_0_CHANNEL_END) {
        domain_offset = dma_channel - SLOW_DMA_0_CHANNEL_BEGIN;
        SLOW_DMA_0_RG_GLB_CPU0_INT_CLR = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    } else if ((dma_channel >= SLOW_DMA_1_CHANNEL_BEGIN) && (dma_channel <= SLOW_DMA_1_CHANNEL_END)) {
        domain_offset = dma_channel - SLOW_DMA_1_CHANNEL_BEGIN;
        SLOW_DMA_1_RG_GLB_CPU0_INT_CLR = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    } else {
        domain_offset = dma_channel - I2S_DMA_CHANNEL_BEGIN;
        I2S_DMA_RG_GLB_CPU0_INT_CLR = (DMA_GLB_IRQ_CFG_BIT_MASK(domain_offset));
    }
}

static bool internal_dma_check_idle_status(dma_channel_t dma_channel)
{
    uint32_t domain_offset;

    if (dma_channel <= SLOW_DMA_0_CHANNEL_END) {
        domain_offset = dma_channel - SLOW_DMA_0_CHANNEL_BEGIN;
        if ((SLOW_DMA_0_RG_GLB_STA & DMA_GLB_RUNNING_BIT_MASK(domain_offset)) ||
            (SLOW_DMA_0_RG_GLB_STA & DMA_GLB_IRQ_STA_BIT_MASK(domain_offset))) {
            return false;
        }
    } else if ((dma_channel >= SLOW_DMA_1_CHANNEL_BEGIN) && (dma_channel <= SLOW_DMA_1_CHANNEL_END)) {
        domain_offset = dma_channel - SLOW_DMA_1_CHANNEL_BEGIN;
        if ((SLOW_DMA_1_RG_GLB_STA & DMA_GLB_RUNNING_BIT_MASK(domain_offset)) ||
            (SLOW_DMA_1_RG_GLB_STA & DMA_GLB_IRQ_STA_BIT_MASK(domain_offset))) {
            return false;
        }
    } else {
        domain_offset = dma_channel - I2S_DMA_CHANNEL_BEGIN;
        if ((I2S_DMA_RG_GLB_STA & DMA_GLB_RUNNING_BIT_MASK(domain_offset)) ||
            (I2S_DMA_RG_GLB_STA & DMA_GLB_IRQ_STA_BIT_MASK(domain_offset))) {
            return false;
        }
    }

    return true;
}

static bool internal_dma_check_running_status(dma_channel_t dma_channel)
{
    uint32_t domain_offset;

    if (dma_channel <= SLOW_DMA_0_CHANNEL_END) {
        domain_offset = dma_channel - SLOW_DMA_0_CHANNEL_BEGIN;
        if (SLOW_DMA_0_RG_GLB_STA & DMA_GLB_RUNNING_BIT_MASK(domain_offset)) {
            return true;
        }
    } else if ((dma_channel >= SLOW_DMA_1_CHANNEL_BEGIN) && (dma_channel <= SLOW_DMA_1_CHANNEL_END)) {
        domain_offset = dma_channel - SLOW_DMA_1_CHANNEL_BEGIN;
        if (SLOW_DMA_1_RG_GLB_STA & DMA_GLB_RUNNING_BIT_MASK(domain_offset)) {
            return true;
        }
    } else {
        domain_offset = dma_channel - I2S_DMA_CHANNEL_BEGIN;
        if (I2S_DMA_RG_GLB_STA & DMA_GLB_RUNNING_BIT_MASK(domain_offset)) {
            return true;
        }
    }

    return false;
}

/*************************************************************************************************************/
/************************************peripheral  dma function start  line********************************************/
/*************************************************************************************************************/

#define PDMA_INIT    1
#define PDMA_DEINIT  0

#define PDMA_MAX_WPPT 0xFFFF

/* Map table for pdma_channel_t -> dma_channel_t */
static dma_channel_t g_pdma_channel_map_table[] = {
    SLOW_DMA_0_CHANNEL_I2C0_TX,
    SLOW_DMA_0_CHANNEL_I2C0_RX,
    SLOW_DMA_1_CHANNEL_I2C1_TX,
    SLOW_DMA_1_CHANNEL_I2C1_RX,
    SLOW_DMA_1_CHANNEL_I2C2_TX,
    SLOW_DMA_1_CHANNEL_I2C2_RX,
};

/* PDMA channel base address list */
static PDMA_REGISTER_T *g_pdma_regbase_table[] = {
    PDMA_RG_I2C0_TX_BASE,
    PDMA_RG_I2C0_RX_BASE,
    PDMA_RG_I2C1_TX_BASE,
    PDMA_RG_I2C1_RX_BASE,
    PDMA_RG_I2C2_TX_BASE,
    PDMA_RG_I2C2_RX_BASE,
};

typedef struct {
    pdma_callback_t func;
    void *argument;
} pdma_user_callback_t;

static pdma_user_callback_t g_pdma_callback[PDMA_END_CHANNEL];
static volatile uint8_t g_pdma_init_status[PDMA_END_CHANNEL];

static void internal_pdma_restore_default_value(pdma_channel_t channel)
{
    PDMA_REGISTER_T *pdma;

    pdma = g_pdma_regbase_table[channel];
    pdma->PDMA_START = 0;
    pdma->PDMA_WPPT = 0;
    pdma->PDMA_WPTO = 0;
    pdma->PDMA_COUNT = 0;
    pdma->PDMA_CON_UNION.PDMA_CON = 0;
    pdma->PDMA_LIMITER = 0;
    pdma->PDMA_PGMADDR = 0;
    pdma->PDMA_ACKINT = 0x8000;
}

pdma_status_t pdma_init(pdma_channel_t channel)
{
    uint32_t saved_mask;

    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (g_pdma_init_status[channel] == PDMA_INIT) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return PDMA_ERROR;
    }
    g_pdma_init_status[channel] = PDMA_INIT;
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (internal_dma_check_idle_status(g_pdma_channel_map_table[channel]) == false) {
        assert(0);
        return PDMA_ERROR;
    }

    internal_dma_enable_clock(g_pdma_channel_map_table[channel]);
    internal_pdma_restore_default_value(channel);

    return PDMA_OK;
}

pdma_status_t pdma_deinit(pdma_channel_t channel)
{
    uint32_t saved_mask;

    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (g_pdma_init_status[channel] == PDMA_DEINIT) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return PDMA_ERROR;
    }

    if (internal_dma_check_idle_status(g_pdma_channel_map_table[channel]) == false) {
        assert(0);
        return PDMA_ERROR;
    }

    internal_pdma_restore_default_value(channel);
    internal_dma_disable_irq(g_pdma_channel_map_table[channel]);
    internal_dma_disable_clock(g_pdma_channel_map_table[channel]);

    g_pdma_init_status[channel] = PDMA_DEINIT;
    hal_nvic_restore_interrupt_mask(saved_mask);

    return PDMA_OK;
}

pdma_status_t pdma_set_ring_buffer(pdma_channel_t channel, uint32_t wppt, uint32_t wpto)
{
    PDMA_REGISTER_T *pdma;

    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }
    if (wppt > PDMA_MAX_WPPT) {
        return PDMA_INVALID_PARAMETER;
    }

    pdma = g_pdma_regbase_table[channel];
    pdma->PDMA_WPPT = wppt;
    pdma->PDMA_WPTO = wpto;

    return PDMA_OK;
}

pdma_status_t pdma_configure(pdma_channel_t channel, pdma_config_t *pdma_config)
{
    uint32_t dma_control;
    PDMA_REGISTER_T *pdma;

    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }

    if ((pdma_config == NULL) ||
        (pdma_config->master_type > PDMA_RX_RINGBUFF) ||
        (pdma_config->size > PDMA_WORD)) {
        return PDMA_INVALID_PARAMETER;
    }

    dma_control = 0;
    if (pdma_config->burst_mode == true) {
        dma_control = PDMA_CON_BURST_4_BEAT << PDMA_CON_BURST_OFFSET;
    } else {
        dma_control = PDMA_CON_BURST_SINGLE << PDMA_CON_BURST_OFFSET;
    }
    switch (pdma_config->master_type) {
        case PDMA_TX:
            dma_control |= PDMA_CON_SINC_MASK;
            break;
        case PDMA_RX:
            dma_control |= PDMA_CON_DIR_MASK;
            dma_control |= PDMA_CON_DINC_MASK;
            break;
        case PDMA_TX_RINGBUFF:
            dma_control |= PDMA_CON_SINC_MASK;
            dma_control |= PDMA_CON_WPEN_MASK;
            break;
        case PDMA_RX_RINGBUFF:
            dma_control |= PDMA_CON_DIR_MASK;
            dma_control |= PDMA_CON_SINC_MASK;
            dma_control |= PDMA_CON_WPSD_MASK;
            dma_control |= PDMA_CON_WPEN_MASK;
            break;
        default:
            dma_control |= PDMA_CON_SINC_MASK;
    }
    switch (pdma_config->size) {
        case PDMA_BYTE:
            dma_control |= PDMA_CON_SIZE_BYTE  << PDMA_CON_SIZE_OFFSET;
            break;
        case PDMA_HALF_WORD:
            dma_control |= PDMA_CON_SIZE_HALF_WORD  << PDMA_CON_SIZE_OFFSET;
            break;
        case PDMA_WORD:
            dma_control |= PDMA_CON_SIZE_WORD  << PDMA_CON_SIZE_OFFSET;
            break;
        default:
            dma_control |= PDMA_CON_SIZE_BYTE  << PDMA_CON_SIZE_OFFSET;
    }
    if ((pdma_config->burst_mode == true) && (pdma_config->size == PDMA_BYTE)) {
        dma_control |= PDMA_CON_B2W_MASK;
    }
    dma_control |= PDMA_CON_DREQ_MASK;

    pdma = g_pdma_regbase_table[channel];
    pdma->PDMA_START &= ~PDMA_START_BIT_MASK;
    pdma->PDMA_CON_UNION.PDMA_CON = dma_control;
    pdma->PDMA_COUNT = pdma_config->count;

    return PDMA_OK;
}

pdma_status_t pdma_start_polling(pdma_channel_t channel, uint32_t address)
{
    PDMA_REGISTER_T *pdma;
    pdma_running_status_t running_status;

    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }
    if ((address % 4) > 0) {
        assert(0);
        return PDMA_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable(address)) {
        assert(0);
        return PDMA_INVALID_PARAMETER;
    }
#endif

    pdma = g_pdma_regbase_table[channel];
    pdma->PDMA_PGMADDR = address;
    pdma->PDMA_CON_UNION.PDMA_CON_CELLS.PDMA_ITEN = 0x0;
    pdma->PDMA_START = PDMA_START_BIT_MASK;

    do {
        pdma_get_running_status(channel, &running_status);
    } while (running_status);
    pdma_stop(channel);

    return PDMA_OK;
}

pdma_status_t pdma_start_interrupt(pdma_channel_t channel, uint32_t address)
{
    PDMA_REGISTER_T *pdma;

    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }
    if ((address % 4) > 0) {
        assert(0);
        return PDMA_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable(address)) {
        assert(0);
        return PDMA_INVALID_PARAMETER;
    }
#endif

    pdma = g_pdma_regbase_table[channel];
    pdma->PDMA_PGMADDR = address;
    pdma->PDMA_CON_UNION.PDMA_CON_CELLS.PDMA_ITEN = 0x1;
    pdma->PDMA_START = PDMA_START_BIT_MASK;

    return PDMA_OK;
}

pdma_status_t pdma_stop(pdma_channel_t channel)
{
    PDMA_REGISTER_T *pdma;

    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }

    pdma = g_pdma_regbase_table[channel];
    pdma->PDMA_START &= ~PDMA_START_BIT_MASK;
    pdma->PDMA_ACKINT = PDMA_ACKINT_BIT_MASK;

    return PDMA_OK;
}

pdma_status_t pdma_register_callback(pdma_channel_t channel, pdma_callback_t callback, void *user_data)
{
    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }
    if (callback == NULL) {
        return PDMA_INVALID_PARAMETER;
    }

    g_pdma_callback[channel].func = callback;
    g_pdma_callback[channel].argument  = user_data;

    internal_dma_register_irq_callback(g_pdma_channel_map_table[channel]);
    internal_dma_enable_irq(g_pdma_channel_map_table[channel]);

    return PDMA_OK;
}

pdma_status_t pdma_get_running_status(pdma_channel_t channel, pdma_running_status_t *running_status)
{
    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }
    if (running_status == NULL) {
        return PDMA_INVALID_PARAMETER;
    }

    if (internal_dma_check_running_status(g_pdma_channel_map_table[channel]) == true) {
        *running_status = PDMA_BUSY;
    } else {
        *running_status = PDMA_IDLE;
    }

    return PDMA_OK;
}

pdma_status_t pdma_dump_reg_list(pdma_channel_t channel)
{
    PDMA_REGISTER_T *pdma;

    if (channel >= PDMA_END_CHANNEL) {
        return PDMA_ERROR_CHANNEL;
    }
    (void)pdma;
    pdma = g_pdma_regbase_table[channel];
    log_hal_msgid_info("PDMA channel %d reg list dump:", 1, channel);
    log_hal_msgid_info("PDMA_WPPT = 0x%08x", 1, pdma->PDMA_WPPT);
    log_hal_msgid_info("PDMA_WPTO = 0x%08x", 1, pdma->PDMA_WPTO);
    log_hal_msgid_info("PDMA_COUNT = 0x%08x", 1, pdma->PDMA_COUNT);
    log_hal_msgid_info("PDMA_CON = 0x%08x", 1, pdma->PDMA_CON_UNION.PDMA_CON);
    log_hal_msgid_info("PDMA_START = 0x%08x", 1, pdma->PDMA_START);
    log_hal_msgid_info("PDMA_INTSTA = 0x%08x", 1, pdma->PDMA_INTSTA);
    log_hal_msgid_info("PDMA_RLCT = 0x%08x", 1, pdma->PDMA_RLCT);
    log_hal_msgid_info("PDMA_LIMITER = 0x%08x", 1, pdma->PDMA_LIMITER);
    log_hal_msgid_info("PDMA_PGMADDR = 0x%08x", 1, pdma->PDMA_PGMADDR);

    return PDMA_OK;
}

/*************************************************************************************************************/
/************************************peripheral  dma function end line**********************************************/
/*************************************************************************************************************/



/*************************************************************************************************************/
/************************************virtual fifo dma function start line*********************************************/
/*************************************************************************************************************/

#define VDMA_INIT    1
#define VDMA_DEINIT  0

#define VDMA_MAX_THRESHOLD       0xFFFF
#define VDMA_MAX_ALERT_LENGTH    0x3F

/* Map table for vdma_channel_t -> dma_channel_t */
static dma_channel_t g_vdma_channel_map_table[] = {
    SLOW_DMA_0_CHANNEL_UART0_TX,
    SLOW_DMA_0_CHANNEL_UART0_RX,
    SLOW_DMA_1_CHANNEL_UART1_TX,
    SLOW_DMA_1_CHANNEL_UART1_RX,
    SLOW_DMA_1_CHANNEL_UART2_TX,
    SLOW_DMA_1_CHANNEL_UART2_RX,
    I2S_DMA_CHANNEL_I2S0_TX,
    I2S_DMA_CHANNEL_I2S0_RX,
    I2S_DMA_CHANNEL_I2S1_TX,
    I2S_DMA_CHANNEL_I2S1_RX,
    I2S_DMA_CHANNEL_I2S2_TX,
    I2S_DMA_CHANNEL_I2S2_RX,
    I2S_DMA_CHANNEL_I2S3_TX,
    I2S_DMA_CHANNEL_I2S3_RX,
    I2S_DMA_CHANNEL_I2S4_TX,
    I2S_DMA_CHANNEL_I2S4_RX,
    I2S_DMA_CHANNEL_I2S5_TX,
    I2S_DMA_CHANNEL_I2S5_RX,
    I2S_DMA_CHANNEL_I2S6_TX,
    I2S_DMA_CHANNEL_I2S6_RX,
    I2S_DMA_CHANNEL_I2S7_TX,
    I2S_DMA_CHANNEL_I2S7_RX,
};

/* VDMA channel base address list */
static VDMA_REGISTER_T *g_vdma_regbase_table[] = {
    VDMA_RG_UART0_TX_BASE,
    VDMA_RG_UART0_RX_BASE,
    VDMA_RG_UART1_TX_BASE,
    VDMA_RG_UART1_RX_BASE,
    VDMA_RG_UART2_TX_BASE,
    VDMA_RG_UART2_RX_BASE,
    VDMA_RG_I2S0_TX_BASE,
    VDMA_RG_I2S0_RX_BASE,
    VDMA_RG_I2S1_TX_BASE,
    VDMA_RG_I2S1_RX_BASE,
    VDMA_RG_I2S2_TX_BASE,
    VDMA_RG_I2S2_RX_BASE,
    VDMA_RG_I2S3_TX_BASE,
    VDMA_RG_I2S3_RX_BASE,
    VDMA_RG_I2S4_TX_BASE,
    VDMA_RG_I2S4_RX_BASE,
    VDMA_RG_I2S5_TX_BASE,
    VDMA_RG_I2S5_RX_BASE,
    VDMA_RG_I2S6_TX_BASE,
    VDMA_RG_I2S6_RX_BASE,
    VDMA_RG_I2S7_TX_BASE,
    VDMA_RG_I2S7_RX_BASE,
};

typedef struct {
    vdma_callback_t func;
    void *argument;
} vdma_user_callback_t;

static vdma_user_callback_t g_vdma_callback[VDMA_END_CHANNEL];
static volatile uint8_t g_vdma_init_status[VDMA_END_CHANNEL];

static void internal_vdma_restore_default_value(vdma_channel_t channel)
{
    VDMA_REGISTER_T *vdma;

    vdma = g_vdma_regbase_table[channel];
    vdma->VDMA_START = 0;
    vdma->VDMA_CON_UNION.VDMA_CON = 0;
    vdma->VDMA_COUNT = 0;
    vdma->VDMA_INTSTA = 0;
    vdma->VDMA_PGMADDR = 0;
    vdma->VDMA_ALTLEN = 0;
    vdma->VDMA_FFSIZE = 0;
    vdma->VDMA_LIMITER = 0;
    vdma->VDMA_ACKINT = 0x8000;
}

vdma_status_t vdma_init(vdma_channel_t channel)
{
    uint32_t saved_mask;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (g_vdma_init_status[channel] == VDMA_INIT) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return PDMA_ERROR;
    }
    g_vdma_init_status[channel] = VDMA_INIT;
    hal_nvic_restore_interrupt_mask(saved_mask);

    internal_dma_enable_clock(g_vdma_channel_map_table[channel]);
    internal_vdma_restore_default_value(channel);

    vdma = g_vdma_regbase_table[channel];
    switch (channel) {
        case VDMA_UART0TX:
        case VDMA_UART1TX:
        case VDMA_UART2TX:
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SETTING = 0x1;
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE = 0x0;
            break;
        case VDMA_UART0RX:
        case VDMA_UART1RX:
        case VDMA_UART2RX:
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_DIRECTION = 0x10;
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SETTING = 0x1;
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE = 0x0;
            break;
        case VDMA_I2S0TX:
        case VDMA_I2S1TX:
        case VDMA_I2S2TX:
        case VDMA_I2S3TX:
        case VDMA_I2S4TX:
        case VDMA_I2S5TX:
        case VDMA_I2S6TX:
        case VDMA_I2S7TX:
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SETTING = 0x1;
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE = 0x2;
            break;
        case VDMA_I2S0RX:
        case VDMA_I2S1RX:
        case VDMA_I2S2RX:
        case VDMA_I2S3RX:
        case VDMA_I2S4RX:
        case VDMA_I2S5RX:
        case VDMA_I2S6RX:
        case VDMA_I2S7RX:
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_DIRECTION = 0x10;
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SETTING = 0x1;
            vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE = 0x2;
            break;
        default:
            return VDMA_ERROR;
    }

    return VDMA_OK;
}

vdma_status_t vdma_deinit(vdma_channel_t channel)
{
    uint32_t saved_mask;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (g_vdma_init_status[channel] == VDMA_DEINIT) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return VDMA_ERROR;
    }

    if (internal_dma_check_idle_status(g_vdma_channel_map_table[channel]) == false) {
        assert(0);
        return VDMA_ERROR;
    }

    internal_vdma_restore_default_value(channel);
    internal_dma_disable_irq(g_vdma_channel_map_table[channel]);
    internal_dma_disable_clock(g_vdma_channel_map_table[channel]);

    g_vdma_init_status[channel] = VDMA_DEINIT;
    hal_nvic_restore_interrupt_mask(saved_mask);

    return VDMA_OK;
}

vdma_status_t vdma_start(vdma_channel_t channel)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    internal_dma_check_idle_status(g_vdma_channel_map_table[channel]);

    vdma = g_vdma_regbase_table[channel];
    vdma->VDMA_START = VDMA_START_BIT_MASK;

    return VDMA_OK;
}

vdma_status_t vdma_stop(vdma_channel_t channel)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma = g_vdma_regbase_table[channel];
    vdma->VDMA_START = 0 << VDMA_START_BIT_OFFSET;
    vdma->VDMA_SW_MV_BYTE = vdma->VDMA_FFCNT << vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE;

    return VDMA_OK;
}

vdma_status_t vdma_configure(vdma_channel_t channel, vdma_config_t *vdma_config)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if ((vdma_config == NULL) || (vdma_config->base_address % 4) > 0) {
        assert(0);
        return VDMA_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable(vdma_config->base_address)) {
        assert(0);
        return VDMA_INVALID_PARAMETER;
    }
#endif

    vdma = g_vdma_regbase_table[channel];
    vdma->VDMA_FFSIZE = vdma_config->size;
    vdma->VDMA_PGMADDR = vdma_config->base_address;

    return VDMA_OK;
}

vdma_status_t vdma_enable_interrupt(vdma_channel_t channel)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma = g_vdma_regbase_table[channel];
    vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_ITEN = 0x1;

    return VDMA_OK;
}

vdma_status_t vdma_disable_interrupt(vdma_channel_t channel)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma = g_vdma_regbase_table[channel];
    vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_ITEN = 0x0;

    return VDMA_OK;
}

vdma_status_t vdma_get_interrupt_status(vdma_channel_t channel, uint8_t *is_enabled)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (is_enabled == NULL) {
        return VDMA_INVALID_PARAMETER;
    }

    vdma = g_vdma_regbase_table[channel];
    *is_enabled = vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_ITEN & 0x1;

    return VDMA_OK;
}

vdma_status_t vdma_set_threshold(vdma_channel_t channel, uint32_t threshold)
{
    uint8_t h_size = 0;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma = g_vdma_regbase_table[channel];
    h_size = vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE;
    threshold = threshold >> h_size;
    if (threshold > VDMA_MAX_THRESHOLD) {
        return VDMA_INVALID_PARAMETER;
    }
    vdma->VDMA_COUNT = threshold;

    return VDMA_OK;
}

vdma_status_t vdma_set_alert_length(vdma_channel_t channel, uint32_t alert_length)
{
    uint8_t h_size = 0;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma = g_vdma_regbase_table[channel];
    h_size = vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE;
    alert_length = alert_length >> h_size;
    if (alert_length > VDMA_MAX_ALERT_LENGTH) {
        return VDMA_INVALID_PARAMETER;
    }
    vdma->VDMA_ALTLEN = alert_length;

    return VDMA_OK;
}

vdma_status_t vdma_push_data(vdma_channel_t channel, uint8_t data)
{
    uint8_t *write_address;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma = g_vdma_regbase_table[channel];
    write_address = (uint8_t *)vdma->VDMA_WRPTR;
    *write_address = data;

    vdma_set_sw_move_byte(channel, 1);

    return VDMA_OK;
}

vdma_status_t vdma_push_data_4bytes(vdma_channel_t channel, uint32_t data)
{
    uint32_t *write_address;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma = g_vdma_regbase_table[channel];
    write_address = (uint32_t *)vdma->VDMA_WRPTR;
    *write_address = data;

    vdma_set_sw_move_byte(channel, 4);

    return VDMA_OK;
}

vdma_status_t vdma_push_data_multi_bytes(vdma_channel_t channel, uint8_t *data, uint32_t size)
{
    uint8_t *write_address;
    uint32_t byte_to_bndry;
    uint32_t byte_avail;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (data == NULL) {
        return VDMA_INVALID_PARAMETER;
    }
    vdma = g_vdma_regbase_table[channel];
    byte_avail = vdma->VDMA_BYTE_AVAIL;
    if (size > byte_avail) {
        return VDMA_INVALID_PARAMETER;
    }

    write_address = (uint8_t *)vdma->VDMA_WRPTR;
    byte_to_bndry = vdma->VDMA_BYTE_TO_BNDRY;
    if (size > byte_to_bndry) {
        memcpy(write_address, data, byte_to_bndry);
        write_address = (uint8_t *)vdma->VDMA_PGMADDR;
        memcpy(write_address, (data + byte_to_bndry), (size - byte_to_bndry));
    } else {
        memcpy(write_address, data, size);
    }
    vdma_set_sw_move_byte(channel, size);

    return VDMA_OK;
}

vdma_status_t vdma_pop_data(vdma_channel_t channel, uint8_t *data)
{
    uint8_t *read_address;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (data == NULL) {
        return VDMA_INVALID_PARAMETER;
    }

    vdma = g_vdma_regbase_table[channel];
    read_address = (uint8_t *)vdma->VDMA_RDPTR;
    *data = *read_address ;

    vdma_set_sw_move_byte(channel, 1);

    return VDMA_OK;
}

vdma_status_t vdma_pop_data_4bytes(vdma_channel_t channel, uint32_t *data)
{
    uint32_t *read_address;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (data == NULL) {
        return VDMA_INVALID_PARAMETER;
    }

    vdma = g_vdma_regbase_table[channel];
    read_address = (uint32_t *)vdma->VDMA_RDPTR;
    *data = *read_address;

    vdma_set_sw_move_byte(channel, 4);

    return VDMA_OK;
}

vdma_status_t vdma_pop_data_multi_bytes(vdma_channel_t channel, uint8_t *data, uint32_t size)
{
    uint8_t h_size;
    uint8_t  *read_address;
    uint32_t ready_for_read;
    uint32_t byte_to_bndry;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (data == NULL) {
        return VDMA_INVALID_PARAMETER;
    }
    vdma = g_vdma_regbase_table[channel];
    h_size = vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE;
    ready_for_read = (vdma->VDMA_FFCNT << h_size);
    if (size > ready_for_read) {
        return VDMA_INVALID_PARAMETER;
    }

    byte_to_bndry = vdma->VDMA_BYTE_TO_BNDRY;
    read_address = (uint8_t *)vdma->VDMA_RDPTR;
    if (size > byte_to_bndry) {
        memcpy(data, read_address, byte_to_bndry);
        read_address = (uint8_t *)vdma->VDMA_PGMADDR;
        memcpy((data + byte_to_bndry), read_address, (size - byte_to_bndry));
    } else {
        memcpy(data, read_address, size);
    }

    vdma_set_sw_move_byte(channel, size);

    return VDMA_OK;
}

vdma_status_t vdma_get_available_receive_bytes(vdma_channel_t channel, uint32_t *receive_bytes)
{
    uint8_t h_size;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (receive_bytes == NULL) {
        return VDMA_INVALID_PARAMETER;
    }

    vdma = g_vdma_regbase_table[channel];
    h_size = vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE;
    *receive_bytes = (vdma->VDMA_FFCNT << h_size);

    return VDMA_OK;
}

vdma_status_t vdma_get_available_send_space(vdma_channel_t channel, uint32_t *available_space)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (available_space == NULL) {
        return VDMA_INVALID_PARAMETER;
    }

    vdma = g_vdma_regbase_table[channel];
    *available_space = vdma->VDMA_BYTE_AVAIL;

    return VDMA_OK;
}

ATTR_TEXT_IN_TCM vdma_status_t vdma_get_hw_read_point(vdma_channel_t channel, uint32_t *read_point)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (read_point == NULL) {
        return VDMA_INVALID_PARAMETER;
    }

    vdma = g_vdma_regbase_table[channel];
    *read_point = vdma->VDMA_RDPTR;

    return VDMA_OK;
}

vdma_status_t vdma_set_sw_move_byte(vdma_channel_t channel, uint16_t sw_move_byte)
{
    uint8_t h_size = 0;
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }

    vdma = g_vdma_regbase_table[channel];
    h_size = vdma->VDMA_CON_UNION.VDMA_CON_CELLS.VDMA_SIZE;
    switch (h_size) {
        case 0x2:
            if ((sw_move_byte & 0x3) != 0x0) {
                return VDMA_INVALID_PARAMETER;
            }
            break;
        default:
            break;
    }

    vdma->VDMA_SW_MV_BYTE = (sw_move_byte >> h_size);

    return VDMA_OK;
}

ATTR_TEXT_IN_TCM vdma_status_t vdma_get_hw_write_point(vdma_channel_t channel, uint32_t *write_point)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (write_point == NULL) {
        return VDMA_INVALID_PARAMETER;
    }

    vdma = g_vdma_regbase_table[channel];
    *write_point = vdma->VDMA_WRPTR;

    return VDMA_OK;
}

vdma_status_t vdma_register_callback(vdma_channel_t channel, vdma_callback_t callback, void *user_data)
{
    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    if (callback == NULL) {
        return VDMA_INVALID_PARAMETER;
    }

    g_vdma_callback[channel].func = callback;
    g_vdma_callback[channel].argument = user_data;

    internal_dma_register_irq_callback(g_vdma_channel_map_table[channel]);
    internal_dma_enable_irq(g_vdma_channel_map_table[channel]);

    return VDMA_OK;
}

vdma_status_t vdma_dump_reg_list(vdma_channel_t channel)
{
    VDMA_REGISTER_T *vdma;

    if (channel >= VDMA_END_CHANNEL) {
        return VDMA_ERROR_CHANNEL;
    }
    (void)vdma;
    vdma = g_vdma_regbase_table[channel];
    log_hal_msgid_info("VDMA channel %d reg list dump:", 1, channel);
    log_hal_msgid_info("VDMA_COUNT = 0x%08x", 1, vdma->VDMA_COUNT);
    log_hal_msgid_info("VDMA_CON = 0x%08x", 1, vdma->VDMA_CON_UNION.VDMA_CON);
    log_hal_msgid_info("VDMA_START = 0x%08x", 1, vdma->VDMA_START);
    log_hal_msgid_info("VDMA_INTSTA = 0x%08x", 1, vdma->VDMA_INTSTA);
    log_hal_msgid_info("VDMA_LIMITER = 0x%08x", 1, vdma->VDMA_LIMITER);
    log_hal_msgid_info("VDMA_PGMADDR = 0x%08x", 1, vdma->VDMA_PGMADDR);
    log_hal_msgid_info("VDMA_WRPTR = 0x%08x", 1, vdma->VDMA_WRPTR);
    log_hal_msgid_info("VDMA_RDPTR = 0x%08x", 1, vdma->VDMA_RDPTR);
    log_hal_msgid_info("VDMA_FFCNT = 0x%08x", 1, vdma->VDMA_FFCNT);
    log_hal_msgid_info("VDMA_FFSTA = 0x%08x", 1, vdma->VDMA_FFSTA);
    log_hal_msgid_info("VDMA_ALTLEN = 0x%08x", 1, vdma->VDMA_ALTLEN);
    log_hal_msgid_info("VDMA_FFSIZE = 0x%08x", 1, vdma->VDMA_FFSIZE);
    log_hal_msgid_info("VDMA_BNDRY_ADDR = 0x%08x", 1, vdma->VDMA_BNDRY_ADDR);
    log_hal_msgid_info("VDMA_BYTE_TO_BNDRY = 0x%08x", 1, vdma->VDMA_BYTE_TO_BNDRY);
    log_hal_msgid_info("VDMA_BYTE_AVAIL = 0x%08x", 1, vdma->VDMA_BYTE_AVAIL);

    return VDMA_OK;
}

/*************************************************************************************************************/
/************************************DMA domain IRQ handler start line*********************************************/
/*************************************************************************************************************/


/* Register base address list for MCU DMA domain */
static void *const g_i2s_dma_domain_channel_regbase_table[] = {
    VDMA_RG_I2S0_TX_BASE,
    VDMA_RG_I2S0_RX_BASE,
    VDMA_RG_I2S1_TX_BASE,
    VDMA_RG_I2S1_RX_BASE,
    VDMA_RG_I2S2_TX_BASE,
    VDMA_RG_I2S2_RX_BASE,
    VDMA_RG_I2S3_TX_BASE,
    VDMA_RG_I2S3_RX_BASE,
    VDMA_RG_I2S4_TX_BASE,
    VDMA_RG_I2S4_RX_BASE,
    VDMA_RG_I2S5_TX_BASE,
    VDMA_RG_I2S5_RX_BASE,
    VDMA_RG_I2S6_TX_BASE,
    VDMA_RG_I2S6_RX_BASE,
    VDMA_RG_I2S7_TX_BASE,
    VDMA_RG_I2S7_RX_BASE
};

/* domain channel index list */
typedef enum {
    I2S_DMA_DOMAIN_I2S0_TX = 0,
    I2S_DMA_DOMAIN_I2S0_RX,
    I2S_DMA_DOMAIN_I2S1_TX,
    I2S_DMA_DOMAIN_I2S1_RX,
    I2S_DMA_DOMAIN_I2S2_TX,
    I2S_DMA_DOMAIN_I2S2_RX,
    I2S_DMA_DOMAIN_I2S3_TX,
    I2S_DMA_DOMAIN_I2S3_RX,
    I2S_DMA_DOMAIN_I2S4_TX,
    I2S_DMA_DOMAIN_I2S4_RX,
    I2S_DMA_DOMAIN_I2S5_TX,
    I2S_DMA_DOMAIN_I2S5_RX,
    I2S_DMA_DOMAIN_I2S6_TX,
    I2S_DMA_DOMAIN_I2S6_RX,
    I2S_DMA_DOMAIN_I2S7_TX,
    I2S_DMA_DOMAIN_I2S7_RX,
} i2s_dma_domain_channel_t;

/* Map from domain channel index to vdma_channel_t/pdma_channel_t */
static uint32_t g_i2s_dma_domain_channel_index[] = {
    6,
    7,
    8,
    9,
    10,
    11,
    12,
    13,
    14,
    15,
    16,
    17,
    18,
    19,
    20,
    21
};

void i2s_dma_interrupt_hander(hal_nvic_irq_t irq_number)
{
    void *argument;
    vdma_callback_t vdma_callback;
    i2s_dma_domain_channel_t channel = 0;
    VDMA_REGISTER_T *vdma;
    uint32_t index;

    if (irq_number != I2S_SLAVE_IRQn) {
        assert(0);
    }

    for (channel = I2S_DMA_DOMAIN_I2S0_TX; channel <= I2S_DMA_DOMAIN_I2S7_RX; channel++) {
        index = g_i2s_dma_domain_channel_index[channel];
        if ((g_vdma_init_status[index] == VDMA_INIT) && (I2S_DMA_RG_GLB_STA & DMA_GLB_IRQ_STA_BIT_MASK(channel))) {
            /*clear vdma interrupt*/
            vdma = (VDMA_REGISTER_T *)g_i2s_dma_domain_channel_regbase_table[channel];
            vdma->VDMA_ACKINT = VDMA_ACKINT_BIT_MASK;
            /*call vdma user's callback */
            vdma_callback = g_vdma_callback[index].func;
            argument = g_vdma_callback[index].argument;
            if (vdma_callback != NULL) {
                vdma_callback(VDMA_EVENT_TRANSACTION_SUCCESS, argument);
            } else {
                assert(0);
            }
        }
    }
}

/* Register base address list for SLOW DMA 0 domain */
static void *const g_slow_dma_0_domain_channel_regbase_table[] = {
    VDMA_RG_UART0_TX_BASE,
    VDMA_RG_UART0_RX_BASE,
    PDMA_RG_I2C0_TX_BASE,
    PDMA_RG_I2C0_RX_BASE,
};

/* domain channel index list */
typedef enum {
    SLOW_DMA_0_DOMAIN_UART0_TX = 0,
    SLOW_DMA_0_DOMAIN_UART0_RX,
    SLOW_DMA_0_DOMAIN_I2C0_TX,
    SLOW_DMA_0_DOMAIN_I2C0_RX,
} slow_dma_0_domain_channel_t;

/* Map from domain channel index to vdma_channel_t/pdma_channel_t */
static uint32_t slow_dma_0_domain_channel_index[] = {
    0,
    1,
    0,
    1,
};

void slow_dma_0_interrupt_hander(hal_nvic_irq_t irq_number)
{
    uint32_t index;
    void *argument;
    VDMA_REGISTER_T *vdma;
    PDMA_REGISTER_T *pdma;
    vdma_callback_t vdma_callback;
    pdma_callback_t pdma_callback;
    slow_dma_0_domain_channel_t channel = 0;

    if (irq_number != SLOW_DMA0_IRQn) {
        assert(0);
    }

    for (channel = SLOW_DMA_0_DOMAIN_UART0_TX; channel <= SLOW_DMA_0_DOMAIN_UART0_RX; channel++) {
        index = slow_dma_0_domain_channel_index[channel];
        if ((g_vdma_init_status[index] == VDMA_INIT) && (SLOW_DMA_0_RG_GLB_STA & DMA_GLB_IRQ_STA_BIT_MASK(channel))) {
            /*clear vdma interrupt*/
            vdma = (VDMA_REGISTER_T *)g_slow_dma_0_domain_channel_regbase_table[channel];
            vdma->VDMA_ACKINT = VDMA_ACKINT_BIT_MASK;
            /*call vdma user's callback */
            vdma_callback = g_vdma_callback[index].func;
            argument = g_vdma_callback[index].argument;
            if (vdma_callback != NULL) {
                vdma_callback(VDMA_EVENT_TRANSACTION_SUCCESS, argument);
            } else {
                assert(0);
            }
        }
    }

    for (channel = SLOW_DMA_0_DOMAIN_I2C0_TX; channel <= SLOW_DMA_0_DOMAIN_I2C0_RX; channel++) {
        index = slow_dma_0_domain_channel_index[channel];
        if ((g_pdma_init_status[index] == PDMA_INIT) && (SLOW_DMA_0_RG_GLB_STA & DMA_GLB_IRQ_STA_BIT_MASK(channel))) {
            pdma = (PDMA_REGISTER_T *)g_slow_dma_0_domain_channel_regbase_table[channel];
            /*clear pdma interrupt*/
            pdma->PDMA_ACKINT = PDMA_ACKINT_BIT_MASK;
            pdma->PDMA_START &= ~PDMA_START_BIT_MASK;
            /*call pdma user's callback */
            pdma_callback = g_pdma_callback[index].func;
            argument = g_pdma_callback[index].argument;
            if (pdma_callback != NULL) {
                pdma_callback(PDMA_EVENT_TRANSACTION_SUCCESS, argument);
            } else {
                assert(0);
            }
        }
    }
}

/* Register base address list for SLOW DMA 1 domain */
static void *const g_slow_dma_1_domain_channel_regbase_table[] = {
    VDMA_RG_UART1_TX_BASE,
    VDMA_RG_UART1_RX_BASE,
    VDMA_RG_UART2_TX_BASE,
    VDMA_RG_UART2_RX_BASE,
    PDMA_RG_I2C1_TX_BASE,
    PDMA_RG_I2C1_RX_BASE,
    PDMA_RG_I2C2_TX_BASE,
    PDMA_RG_I2C2_RX_BASE,
};

/* domain channel index list */
typedef enum {
    SLOW_DMA_1_DOMAIN_UART1_TX = 0,
    SLOW_DMA_1_DOMAIN_UART1_RX,
    SLOW_DMA_1_DOMAIN_UART2_TX,
    SLOW_DMA_1_DOMAIN_UART2_RX,
    SLOW_DMA_1_DOMAIN_I2C1_TX,
    SLOW_DMA_1_DOMAIN_I2C1_RX,
    SLOW_DMA_1_DOMAIN_I2C2_TX,
    SLOW_DMA_1_DOMAIN_I2C2_RX,
} slow_dma_1_domain_channel_t;

/* Map from domain channel index to vdma_channel_t/pdma_channel_t */
static uint32_t slow_dma_1_domain_channel_index[] = {
    2,
    3,
    4,
    5,
    2,
    3,
    4,
    5
};

void slow_dma_1_interrupt_hander(hal_nvic_irq_t irq_number)
{
    uint32_t index;
    void *argument;
    VDMA_REGISTER_T *vdma;
    PDMA_REGISTER_T *pdma;
    vdma_callback_t vdma_callback;
    pdma_callback_t pdma_callback;
    slow_dma_1_domain_channel_t channel = 0;

    if (irq_number != SLOW_DMA1_IRQn) {
        assert(0);
    }

    for (channel = SLOW_DMA_1_DOMAIN_UART1_TX; channel <= SLOW_DMA_1_DOMAIN_UART2_RX; channel++) {
        index = slow_dma_1_domain_channel_index[channel];
        if ((g_vdma_init_status[index] == VDMA_INIT) && (SLOW_DMA_1_RG_GLB_STA & DMA_GLB_IRQ_STA_BIT_MASK(channel))) {
            /*clear vdma interrupt*/
            vdma = (VDMA_REGISTER_T *)g_slow_dma_1_domain_channel_regbase_table[channel];
            vdma->VDMA_ACKINT = VDMA_ACKINT_BIT_MASK;
            /*call vdma user's callback */
            vdma_callback = g_vdma_callback[index].func;
            argument = g_vdma_callback[index].argument;
            if (vdma_callback != NULL) {
                vdma_callback(VDMA_EVENT_TRANSACTION_SUCCESS, argument);
            } else {
                assert(0);
            }
        }
    }

    for (channel = SLOW_DMA_1_DOMAIN_I2C1_TX; channel <= SLOW_DMA_1_DOMAIN_I2C2_RX; channel++) {
        index = slow_dma_1_domain_channel_index[channel];
        if ((g_pdma_init_status[index] == PDMA_INIT) && (SLOW_DMA_1_RG_GLB_STA & DMA_GLB_IRQ_STA_BIT_MASK(channel))) {
            pdma = (PDMA_REGISTER_T *)g_slow_dma_1_domain_channel_regbase_table[channel];
            /*clear pdma interrupt*/
            pdma->PDMA_ACKINT = PDMA_ACKINT_BIT_MASK;
            pdma->PDMA_START &= ~PDMA_START_BIT_MASK;
            /*call pdma user's callback */
            pdma_callback = g_pdma_callback[index].func;
            argument = g_pdma_callback[index].argument;
            if (pdma_callback != NULL) {
                pdma_callback(PDMA_EVENT_TRANSACTION_SUCCESS, argument);
            } else {
                assert(0);
            }
        }
    }
}

