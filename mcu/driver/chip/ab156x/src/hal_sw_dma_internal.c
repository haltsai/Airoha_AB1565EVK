/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#include "hal_sw_dma.h"

#ifdef HAL_SW_DMA_MODULE_ENABLED

#include <assert.h>
#ifdef HAL_CACHE_MODULE_ENABLED
#include "hal_cache.h"
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif
#include "hal_clock_internal.h"
#include "hal_sw_dma_internal.h"

static GDMA_REGISTER_T *const g_sw_dma_reg = SW_DMA_CHANNEL_REG_BASE;

void sw_dma_clear_irq(void)
{
    g_sw_dma_reg->GDMA_ACKINT = GDMA_ACKINT_BIT_MASK;
    g_sw_dma_reg->GDMA_START = 0;

#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_unlock_sleep(SLEEP_LOCK_DMA);
#endif
}

void sw_dma_init(void)
{
    ///TODO:: modify the clock CG
    hal_clock_enable(HAL_CLOCK_CG_CM4_DMA);
    //*(volatile uint32_t *)(0xA2200320) = 0x01;
    MCU_DMA_RG_GLB_CLK_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(SW_DMA_CHANNEL_NUMBER));
    MCU_DMA_RG_GLB_CPU0_INT_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(SW_DMA_CHANNEL_NUMBER));

    g_sw_dma_reg->GDMA_SRC = 0;
    g_sw_dma_reg->GDMA_DST = 0;
    g_sw_dma_reg->GDMA_WPPT = 0;
    g_sw_dma_reg->GDMA_WPTO = 0;
    g_sw_dma_reg->GDMA_CON_UNION.GDMA_CON = 0;
    g_sw_dma_reg->GDMA_START = 0;
    g_sw_dma_reg->GDMA_LIMITER = 0;
    g_sw_dma_reg->GDMA_ACKINT = GDMA_ACKINT_BIT_MASK;
}

void sw_dma_start_interrupt(hal_sw_dma_config_info_t *info)
{
    MCU_DMA_RG_GLB_CLK_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(SW_DMA_CHANNEL_NUMBER));
    MCU_DMA_RG_GLB_CPU0_INT_SET = (DMA_GLB_IRQ_CFG_BIT_MASK(SW_DMA_CHANNEL_NUMBER));
    g_sw_dma_reg->GDMA_CON_UNION.GDMA_CON = ((info->dma_type & 0x3) | (info->h_size << 8) | (info->transfer_type << 18) | (0x1 << 24));
    g_sw_dma_reg->GDMA_COUNT = (info->length) >> (info->h_size);
    g_sw_dma_reg->GDMA_SRC = info->source_address;
    g_sw_dma_reg->GDMA_DST = info->destination_address;

#ifdef HAL_SLEEP_MANAGER_ENABLED
    hal_sleep_manager_lock_sleep(SLEEP_LOCK_DMA);
#endif

    g_sw_dma_reg->GDMA_START = GDMA_START_BIT_MASK;
}

#endif /*HAL_SW_DMA_MODULE_ENABLED*/

