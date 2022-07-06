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

#include "hal_cache.h"

#ifdef HAL_CACHE_MODULE_ENABLED

#include "hal_cache_internal.h"
#include "hal_nvic.h"
// #include "hal_log.h"
// #include "assert.h"
// #include "memory_attribute.h"
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>

#ifdef __cplusplus
extern "C" {
#endif


hal_cache_status_t hal_cache_init(void)
{
    hal_cache_region_t region;
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Check module status */
    if (g_cache_status != CACHE_IDLE) {
        /* Restore the previous status of interrupt */
        hal_nvic_restore_interrupt_mask(irq_flag);
        return HAL_CACHE_STATUS_ERROR;
    }

    /* Flush and invalidate all cache lines before use, whether CACHE is enabled or not */
    xthal_dcache_all_writeback_inv();
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();
    /* Disable dCACHE */
    xthal_dcache_set_ways(0);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();

    /* Invalidate all cache lines before use, whether CACHE is enabled or not */
    xthal_icache_all_invalidate();
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();
    /* Disable iCACHE */
    xthal_icache_set_ways(0);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();

    /* Update the global variable */
    g_cache_con.w = 0x11111111 * XCHAL_CA_BYPASS;

    /* Set CACHE region registers to default value */
    xthal_set_cacheattr(g_cache_con.w);

    /* Update the global variable */
    for (region = HAL_CACHE_REGION_0; region < HAL_CACHE_REGION_MAX; region ++) {
        /* Update the global variable */
        g_cache_entry[region].cache_entry_n.w = 0;
        g_cache_entry[region].cache_end_entry_n.w = 0;
    }

    /* Update the global variable*/
    g_cache_region_en = 0;

    /* Change status to INIT */
    g_cache_status = CACHE_INIT;

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);
    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_deinit(void)
{
    hal_cache_region_t region;
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Check module status */
    if (g_cache_status == CACHE_ENABLE) {
        /* Restore the previous status of interrupt */
        hal_nvic_restore_interrupt_mask(irq_flag);
        return HAL_CACHE_STATUS_ERROR;
    }

    /* Invalidate all cache lines */
    // xthal_icache_all_invalidate();
    // xthal_dcache_all_invalidate();

    /* Disable iCACHE */
    xthal_icache_set_ways(0);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();
    /* Disable dCACHE */
    xthal_dcache_set_ways(0);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();

    /* Update the global variable */
    g_cache_con.w = 0x11111111 * XCHAL_CA_BYPASS;

    /* Set CACHE region registers to default value */
    xthal_set_cacheattr(g_cache_con.w);

    /* Update the global variable */
    for (region = HAL_CACHE_REGION_0; region < HAL_CACHE_REGION_MAX; region ++) {
        /* Update the global variable */
        g_cache_entry[region].cache_entry_n.w = 0;
        g_cache_entry[region].cache_end_entry_n.w = 0;
    }

    /* Update the global variable*/
    g_cache_region_en = 0;

    /* Change status to idle */
    g_cache_status = CACHE_IDLE;

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);
    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_enable(void)
{
    uint32_t irq_flag;
    hal_cache_region_t region;
    uint32_t offset;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* CACHE should not be idle */
    if (g_cache_status == CACHE_IDLE) {
        /* Restore the previous status of interrupt */
        hal_nvic_restore_interrupt_mask(irq_flag);
        return HAL_CACHE_STATUS_ERROR;
    }

    /* Set CACHE region registers to configured value */
    g_cache_con.w = 0x11111111 * XCHAL_CA_BYPASS;
    for (region = HAL_CACHE_REGION_0; region < HAL_CACHE_REGION_MAX; region ++) {
        if(g_cache_region_en & (0x1 << region))
        {
            offset = g_cache_entry[region].cache_entry_n.w >> 29;
            g_cache_con.w = (g_cache_con.w & (~(0xf << (offset * 4)))) | (XCHAL_CA_WRITEBACK << (offset * 4));
        }
    }
    xthal_set_cacheattr(g_cache_con.w);

    /* Enable dCACHE */
    xthal_dcache_set_ways(4);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();
    /* Enable iCACHE */
    xthal_icache_set_ways(4);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();

    /* Change status to enable */
    g_cache_status = CACHE_ENABLE;

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);
    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_disable(void)
{
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* CACHE should not be idle */
    if (g_cache_status == CACHE_IDLE) {
        /* Restore the previous status of interrupt */
        hal_nvic_restore_interrupt_mask(irq_flag);
        return HAL_CACHE_STATUS_ERROR;
    }
    /* If CACHE is enabled, flush and invalidate all cache lines */
    else if (g_cache_status == CACHE_ENABLE) {
        /* this function will flush and invalidate all cache lines */
        hal_cache_invalidate_all_cache_lines();
    }

    /* Disable dCACHE */
    xthal_dcache_set_ways(0);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();
    /* Disable iCACHE */
    xthal_icache_set_ways(0);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();

    /* Change status to enable */
    g_cache_status = CACHE_DISABLE;

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);
    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_region_enable(hal_cache_region_t region)
{
    uint32_t irq_flag;

    /* Region is invalid */
    if (region >= HAL_CACHE_REGION_MAX) {
        return HAL_CACHE_STATUS_ERROR_REGION;
    }

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* CACHE should be init */
    //if (g_cache_status != CACHE_INIT) {
        /* Restore the previous status of interrupt */
        //hal_nvic_restore_interrupt_mask(irq_flag);
        //return HAL_CACHE_STATUS_ERROR;
    //}

    /* The region address must be 512MB aligned */
    if (g_cache_entry[region].cache_entry_n.w & (MTK_CACHE_REGION_SIZE_UNIT - 1)) {
        /* Restore the previous status of interrupt */
        hal_nvic_restore_interrupt_mask(irq_flag);
        return HAL_CACHE_STATUS_ERROR_REGION_ADDRESS;
    }

    /* The region size must be 512MB */
    if ((g_cache_entry[region].cache_end_entry_n.w - g_cache_entry[region].cache_entry_n.w) != (MTK_CACHE_REGION_SIZE_UNIT)) {
        /* Restore the previous status of interrupt */
        hal_nvic_restore_interrupt_mask(irq_flag);
        return HAL_CACHE_STATUS_ERROR_REGION_SIZE;
    }

    /* Enable this region */
    g_cache_region_en |= (1 << region);

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);
    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_region_disable(hal_cache_region_t region)
{
    uint32_t irq_flag;

    /* Region is invalid */
    if (region >= HAL_CACHE_REGION_MAX) {
        return HAL_CACHE_STATUS_ERROR_REGION;
    }

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* CACHE should be init */
    if (g_cache_status != CACHE_INIT) {
        /* Restore the previous status of interrupt */
        hal_nvic_restore_interrupt_mask(irq_flag);
        return HAL_CACHE_STATUS_ERROR;
    }

    /* Disable this region */
    g_cache_region_en &= ~(1 << region);

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);
    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_region_config(hal_cache_region_t region, const hal_cache_region_config_t *region_config)
{
    uint32_t irq_flag;

    /* Region is invalid */
    if (region >= HAL_CACHE_REGION_MAX) {
        return HAL_CACHE_STATUS_ERROR_REGION;
    }

    /* Parameter check */
    if (region_config == NULL) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* The region address must be 512MB aligned */
    if (region_config->cache_region_address & (MTK_CACHE_REGION_SIZE_UNIT - 1)) {
        return HAL_CACHE_STATUS_ERROR_REGION_ADDRESS;
    }

    /* The region size must be 512MB */
    if (region_config->cache_region_size != (MTK_CACHE_REGION_SIZE_UNIT)) {
        return HAL_CACHE_STATUS_ERROR_REGION_SIZE;
    }

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* CACHE should be init */
//    if (g_cache_status != CACHE_INIT) {
        /* Restore the previous status of interrupt */
//        hal_nvic_restore_interrupt_mask(irq_flag);
//        return HAL_CACHE_STATUS_ERROR;
//    }

    /* Update the global variable */
    g_cache_entry[region].cache_entry_n.w = region_config->cache_region_address;
    g_cache_entry[region].cache_end_entry_n.w = region_config->cache_region_address + region_config->cache_region_size;

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);
    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_set_size(hal_cache_size_t cache_size)
{
    (void)cache_size;
    /* CACHE size is invalid */
    /*  DSP site no need this jugdement,because DSP cache is fixed.ex I_cache = 32KB, D_cache = 8KB.
    if (cache_size != HAL_CACHE_SIZE_16KB) {
        return HAL_CACHE_STATUS_ERROR_CACHE_SIZE;
    }
    */
    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_one_cache_line(uint32_t address)
{
    uint32_t irq_flag;

    /* Make sure address is cache line size aligned */
    if (address & (HAL_CACHE_LINE_SIZE - 1)) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }
    /* In order to prevent race condition, interrupt should be disabled when operate cache */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Invalidate dCACHE line by address */
    xthal_dcache_line_invalidate((void *) address);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();
    /* Invalidate iCACHE line by address */
    xthal_icache_line_invalidate((void *) address);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_one_i_cache_line(uint32_t address)
{
    uint32_t irq_flag;

    /* Make sure address is cache line size aligned */
    if (address & (HAL_I_CACHE_LINE_SIZE - 1)) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }
    /* In order to prevent race condition, interrupt should be disabled when opearte cache */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Invalidate iCACHE line by address */
    xthal_icache_line_invalidate((void *) address);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_one_d_cache_line(uint32_t address)
{
    uint32_t irq_flag;

    /* Make sure address is cache line size aligned */
    if (address & (HAL_D_CACHE_LINE_SIZE - 1)) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }
    /* In order to prevent race condition, interrupt should be disabled when opearte cache */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Invalidate dCACHE line by address */
    xthal_dcache_line_invalidate((void *) address);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_multiple_cache_lines(uint32_t address, uint32_t length)
{
    uint32_t irq_flag;

    /* Make sure address and length are both cache line size aligned */
    if ((address & (HAL_CACHE_LINE_SIZE - 1)) || (length & (HAL_CACHE_LINE_SIZE - 1))) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* In order to prevent race condition, interrupt should be disabled when opearte cache */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Invalidate dCACHE lines by address and length */
    xthal_dcache_region_invalidate((void *)address, length);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();
    /* Invalidate iCACHE lines by address and length */
    xthal_icache_region_invalidate((void *)address, length);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_multiple_i_cache_lines(uint32_t address, uint32_t length)
{
    uint32_t irq_flag;

    /* Make sure address and length are both cache line size aligned */
    if ((address & (HAL_I_CACHE_LINE_SIZE - 1)) || (length & (HAL_I_CACHE_LINE_SIZE - 1))) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* In order to prevent race condition, interrupt should be disabled when opearte cache */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Invalidate iCACHE lines by address and length */
    xthal_icache_region_invalidate((void *)address, length);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_multiple_d_cache_lines(uint32_t address, uint32_t length)
{
    uint32_t irq_flag;

    /* Make sure address and length are both cache line size aligned */
    if ((address & (HAL_D_CACHE_LINE_SIZE - 1)) || (length & (HAL_D_CACHE_LINE_SIZE - 1))) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* In order to prevent race condition, interrupt should be disabled when opearte cache */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Invalidate iCACHE lines by address and length */
    xthal_dcache_region_invalidate((void *)address, length);
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_invalidate_all_cache_lines(void)
{
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when opearte cache */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Flush and Invalidate all dCACHE lines */
    xthal_dcache_all_writeback_inv();
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();
    /* Invalidate all iCACHE lines */
    xthal_icache_all_invalidate();
    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_icache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_flush_one_cache_line(uint32_t address)
{
    uint32_t irq_flag;

    /* Make sure address is cache line size aligned */
    if (address & (HAL_D_CACHE_LINE_SIZE - 1)) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* Interrupt is masked to make sure flush or invalidate operation can not be interrupted */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Flush CACHE line by address */
    xthal_dcache_line_writeback((void *) address);

    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_flush_multiple_cache_lines(uint32_t address, uint32_t length)
{
    uint32_t irq_flag;

    /* Make sure address and length are both cache line size aligned */
    if ((address & (HAL_D_CACHE_LINE_SIZE - 1)) || (length & (HAL_D_CACHE_LINE_SIZE - 1))) {
        return HAL_CACHE_STATUS_INVALID_PARAMETER;
    }

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Flush and Invalidate CACHE lines by address and length */
    xthal_dcache_region_writeback((void *)address, length);

    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

hal_cache_status_t hal_cache_flush_all_cache_lines(void)
{
    uint32_t irq_flag;

    /* In order to prevent race condition, interrupt should be disabled when query and update global variable which indicates the module status */
    hal_nvic_save_and_set_interrupt_mask(&irq_flag);

    /* Flush and Invalidate all CACHE lines */
    xthal_dcache_all_writeback();

    /* The sync operation insures any previous cache operations are visible to subsequent code */
    xthal_dcache_sync();

    /* Restore the previous status of interrupt */
    hal_nvic_restore_interrupt_mask(irq_flag);

    return HAL_CACHE_STATUS_OK;
}

bool hal_cache_is_cacheable(uint32_t address)
{
    hal_cache_region_t region;

    /* CACHE is not enable, all memorys are non-cacheable */
    if (g_cache_status != CACHE_ENABLE) {
        return false;
    }
    /* Check if it is in local memory*/
    if(((address >= 0x4500000) && (address < 0x4580000)) ||  /* DRAM */
       ((address >= 0x4580000) && (address < 0x45B0000)))    /* IRAM */
    {
        return false;
    }
    /* CACHE is enabled, compare address with each cacheable region */
    for (region = HAL_CACHE_REGION_0; region < HAL_CACHE_REGION_MAX; region ++) {
        /* Only compare with region that is enabled */
        if (g_cache_region_en & (1 << region)) {
            if ((address >= g_cache_entry[region].cache_entry_n.w) && (address < g_cache_entry[region].cache_end_entry_n.w)) {
                return true;
            }
        }
    }
    return false;
}

#ifdef HAL_CACHE_REGION_CONVERT

uint32_t hal_cache_cacheable_to_noncacheable(uint32_t cacheable_address)
{
    return cacheable_address | 0x80000000 ;
}

uint32_t hal_cache_noncacheable_to_cacheable(uint32_t noncacheable_address)
{
    return noncacheable_address & (~0x80000000) ;
}


#endif /* HAL_CACHE_REGION_CONVERT */


#ifdef __cplusplus
}
#endif

#endif /* HAL_CACHE_MODULE_ENABLED */

