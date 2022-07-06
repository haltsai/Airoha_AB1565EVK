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
 
#include "hal_flash.h"
#ifdef HAL_FLASH_MODULE_ENABLED
#include "memory_attribute.h"
#include "hal_flash_custom_memorydevice.h"
#include "hal_flash_drvflash.h"
#include "hal_flash_mtd_internal.h"
#include "ab156x.h"
#include "hal_flash_general_types.h"
#include "hal_flash_opt.h"
#if defined(__SERIAL_FLASH_EN__)
#include "hal_flash_mtd_sf_dal.h"
#endif
#include "hal_gpt.h"
#include "hal_log.h"




ATTR_TEXT_IN_SYSRAM void *BlockAddress(void *DriverData, uint32_t BlockIndex)
{
    NOR_Flash_MTD_Data *D = DriverData;
    uint32_t addr  = 0;
    uint32_t region = 0, BlockUpperBound = 0;

    BlockUpperBound = D->RegionInfo[0].RegionBlocks;
    while (BlockUpperBound <= BlockIndex) {
        addr += D->RegionInfo[region].RegionBlocks * D->RegionInfo[region].BlockSize;
        region++;
        BlockUpperBound += D->RegionInfo[region].RegionBlocks;
    }
    addr += (BlockIndex - (BlockUpperBound - D->RegionInfo[region].RegionBlocks)) * D->RegionInfo[region].BlockSize;

    return (D->BaseAddr + addr);
}


/*-----------------------------------*/
#define INVALID_BLOCK_ENTRY (0xFFFFFFFF)

bool NOR_FLASH_BUSY = false;
bool NOR_FLASH_SUSPENDED = false;

ATTR_TEXT_IN_SYSRAM static uint32_t LookupAddress(void *DriveData, uint32_t BlockIndex)
{
    NOR_Flash_MTD_Data *D = DriveData;
    uint32_t  addr = 0;
    static uint32_t replaced = 0;
    static uint32_t look_up_hit = 0;
    static uint32_t look_up_miss = 0;
    if ( D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BlockIndex != INVALID_BLOCK_ENTRY ) {
        if (D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BlockIndex != BlockIndex) {
            if (D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BlockIndex != INVALID_BLOCK_ENTRY) {
                if (D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BlockIndex != BlockIndex) {
                    look_up_miss++;
                    if ( replaced & (SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1))) ) {
                        D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BlockIndex = BlockIndex;
                        addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BaseAddress = (uint32_t)BlockAddress(D, BlockIndex);
                        replaced &= ~(SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1))); /* zero first */
                    } else {
                        D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BlockIndex = BlockIndex;
                        addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BaseAddress = (uint32_t)BlockAddress(D, BlockIndex);
                        replaced |= SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1)); /* one first */
                    }
                } else {
                    look_up_hit++;
                    addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BaseAddress;
                    replaced &= ~(SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1))); /* zero first */
                }
            } else {
                look_up_miss++;
                D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BlockIndex = BlockIndex;
                addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][1].BaseAddress = (uint32_t)BlockAddress(D, BlockIndex);
                replaced &= ~(SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1))); /* zero first */
            }
        } else {
            look_up_hit++;
            addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BaseAddress;
            replaced |= SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1)); /* one first */
        }
    } else {
        look_up_miss++;
        D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BlockIndex = BlockIndex;
        addr = D->lookup_tbl[BlockIndex & (LOOKUP_TABLE_SIZE - 1)][0].BaseAddress = (uint32_t)BlockAddress(D, BlockIndex);
        replaced |= SFC_GENERIC_1_BIT_OFFSET << (BlockIndex & (LOOKUP_TABLE_SIZE - 1)); /* one first */
    }
    return addr;
}
ATTR_TEXT_IN_SYSRAM void *MapWindow(void *DriveData, uint32_t BlockIndex, uint32_t WindowIndex)
{
    NOR_Flash_MTD_Data *D = DriveData;

    /* MapWindow: window index greater zero */
    if (WindowIndex != 0) {
        log_hal_msgid_error("MapWindow error WindowIndex = %d \r\n", 1, WindowIndex);
    }
    D->CurrAddr = (void *)LookupAddress(D, BlockIndex);

    return D->CurrAddr;
}

ATTR_TEXT_IN_SYSRAM uint32_t BlockSize(void *DriveData, uint32_t BlockIndex)
{
    NOR_Flash_MTD_Data *D = DriveData;
    uint32_t region = 0, BlockUpperBound = 0;

    BlockUpperBound = D->RegionInfo[region].RegionBlocks;
    while (BlockUpperBound <= BlockIndex) {
        region++;
        BlockUpperBound += D->RegionInfo[region].RegionBlocks;
    }

    return (D->RegionInfo[region].BlockSize);
}

ATTR_TEXT_IN_SYSRAM uint32_t BlockIndex(void *DriveData, uint32_t blk_addr)
{
    NOR_Flash_MTD_Data *D = DriveData;
    uint32_t     region = 0, BlockUpperAddr = 0;
    uint32_t     blk_idx = 0;

    BlockUpperAddr = D->RegionInfo[region].RegionBlocks * D->RegionInfo[region].BlockSize;

    while ( (BlockUpperAddr < blk_addr) && (D->RegionInfo[region].BlockSize) ) {
        blk_idx += D->RegionInfo[region].RegionBlocks;
        blk_addr -= BlockUpperAddr;
        region++;
        BlockUpperAddr = D->RegionInfo[region].RegionBlocks * D->RegionInfo[region].BlockSize;
    }

    if (BlockUpperAddr >= blk_addr) {
        blk_idx += blk_addr / D->RegionInfo[region].BlockSize;
    } else {
        blk_idx = INVALID_BLOCK_INDEX;
    }
    return blk_idx;
}



#if defined(__SERIAL_FLASH__)

uint32_t get_ROM_BASE(void)
{
#if (defined(__FUE__) && defined(__FOTA_DM__)) || defined(__UBL__)
    return 0;
#else

    return (uint32_t)(SFC_GENERIC_FLASH_BANK_MASK);

#endif
}

uint32_t get_NOR_FLASH_BASE_ADDRESS(void)
{
    return NOR_FLASH_BASE_ADDRESS;
}


uint32_t get_Dual_NOR_FLASH_SIZE(void)
{
    return 0;
}


uint32_t get_NOR_ALLOCATED_FAT_SPACE(void)
{
    return NOR_ALLOCATED_FAT_SPACE;
}



#endif //__SERIAL_FLASH__ 


#ifdef __SERIAL_FLASH__

ATTR_TEXT_IN_RAM void DelayAWhile_UST(uint32_t delay)
{
#if defined(__UBL__)
    hal_gpt_delay_us(delay);
#else

    uint32_t i;
    for( i = 0; i < 10000; i++) {
        if (i == 9999) return;
    }
#endif
}

#endif /* __AMD_SERIES_NOR__ || __SERIAL_FLASH__ */


#endif //#ifdef HAL_FLASH_MODULE_ENABLED


