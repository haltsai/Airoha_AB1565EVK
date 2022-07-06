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


#define _BL_FOTA_FLASH_CTRL_C_
#include "bl_fota_flash_ctrl.h"
#include "bl_fota_def.h"
#include "fota_util.h"
#include "fota_flash.h"
#include "bsp_flash.h"

#define DFU_DATA_SIZE_4KB           0x00001000
#define DFU_DATA_SIZE_32KB          0x00008000
#define DFU_DATA_SIZE_64KB          0x00010000

#define DFU_ERASE_FLASE_4KB_BOUNDARY_MASK 0xFFF
#define DFU_ERASE_FLASE_32KB_BOUNDARY_MASK 0x7FFF
#define DFU_ERASE_FLASE_64KB_BOUNDARY_MASK  0xFFFF

#ifdef DFU_VERIFY_VALUE
U8 gCmpBuffer[FLASH_PAGE_SIZE] ALIGN(64);
#endif


DFU_ERRCODE bl_fota_flash_verify_erase(U32 addr, U32 size)
{
#ifdef DFU_VERIFY_VALUE

 U32 i;
 U16 j;
 for (i = 0; i < (U16)(size >> 8); i++)
 {
  if (bsp_flash_read(addr + (U32)(i<<8) , gCmpBuffer, 256) != BSP_FLASH_STATUS_OK)
  {
   return DFU_ERR_FLASH_CTRL_NVRAM_PAGE_READ_FAIL;
  }
  for (j = 0; j < FLASH_PAGE_SIZE; j++)
  {
   if (*(gCmpBuffer + j) != 0xFF)
   {
    return DFU_ERR_FLASH_CTRL_VERIFY_ERASE_FAIL;
   }
  }
 }
#else
 UNUSED(addr);
 UNUSED(size);
#endif
 return DFU_ERR_SUCCESS;
}

DFU_ERRCODE bl_fota_flash_verify_data(U32 addr, U8* pBuffer, U32 size)
{
#ifdef DFU_VERIFY_VALUE
    U16 i = 0;
    while (size >= FLASH_PAGE_SIZE)
    {
        if (bsp_flash_read(addr, gCmpBuffer, 256) != BSP_FLASH_STATUS_OK)
        {
            return DFU_ERR_FLASH_CTRL_NVRAM_PAGE_READ_FAIL;
        }
        if (memcmp(pBuffer + i * FLASH_PAGE_SIZE, gCmpBuffer, FLASH_PAGE_SIZE))
        {
            return DFU_ERR_FLASH_CTRL_VERIFY_DATA_FAIL;
        }
        i++;
        size -= FLASH_PAGE_SIZE;
        addr += FLASH_PAGE_SIZE;
    }
    if (size > 0)
    {
        if (bsp_flash_read(addr, gCmpBuffer, size) != BSP_FLASH_STATUS_OK)
        {
            return DFU_ERR_FLASH_CTRL_NVRAM_PAGE_READ_FAIL;
        }
        if (memcmp(pBuffer + i * FLASH_PAGE_SIZE, gCmpBuffer, size))
        {
            return DFU_ERR_FLASH_CTRL_VERIFY_DATA_FAIL;
        }
    }
#else
    UNUSED(addr);
    UNUSED(pBuffer);
    UNUSED(size);
#endif
    return DFU_ERR_SUCCESS;
}

DFU_ERRCODE bl_fota_flash_erase(U32 startEraseAddr, U32 eraseSize)
{
#if 1
    FOTA_LOG_I("startEraseAddr=%x eraseSize=%x",startEraseAddr,eraseSize);
    hal_flash_block_t block_type;
    U32 eraseLength;

    while (eraseSize)
    {
        block_type = HAL_FLASH_BLOCK_4K;
        eraseLength = DFU_DATA_SIZE_4KB;

        if (eraseSize < DFU_DATA_SIZE_4KB)
        {
            FOTA_LOG_W("Not 4K align with eraseSize:%d left.", eraseSize);
            eraseSize = DFU_DATA_SIZE_4KB;
        }

        if (eraseSize >= DFU_DATA_SIZE_64KB)
        {
            if(startEraseAddr % DFU_DATA_SIZE_64KB == 0)
            {
                block_type = HAL_FLASH_BLOCK_64K;
                eraseLength = DFU_DATA_SIZE_64KB;
            }
            else if (startEraseAddr % DFU_DATA_SIZE_32KB == 0)
            {
                block_type = HAL_FLASH_BLOCK_32K;
                eraseLength = DFU_DATA_SIZE_32KB;
            }
        }
        else if (eraseSize >= DFU_DATA_SIZE_32KB)
        {
            if(startEraseAddr % DFU_DATA_SIZE_32KB == 0)
            {
                block_type = HAL_FLASH_BLOCK_32K;
                eraseLength = DFU_DATA_SIZE_32KB;
            }
        }

        if (bsp_flash_erase(startEraseAddr, block_type) != FLASH_SUCCESS)
        {
            return DFU_ERR_FLASH_CTRL_4K_ERASE_FAIL;
        }

        startEraseAddr += eraseLength;
        eraseSize      -= eraseLength;
    }
    return DFU_ERR_SUCCESS;
#else

    U32 eraseLength;
    hal_flash_block_t block_type;

    while (eraseSize)
    {
        block_type = HAL_FLASH_BLOCK_4K;
        eraseLength = DFU_DATA_SIZE_4KB;

        if (eraseSize < DFU_DATA_SIZE_4KB)
        {
            FOTA_LOG_W("Not 4K align with eraseSize:%d left.", eraseSize);
            eraseSize = DFU_DATA_SIZE_4KB;
        }

        if (eraseSize >= DFU_DATA_SIZE_64KB)
        {
            if(startEraseAddr % DFU_DATA_SIZE_64KB == 0)
            {
                block_type = HAL_FLASH_BLOCK_64K;
                eraseLength = DFU_DATA_SIZE_64KB;
            }
            else if (startEraseAddr % DFU_DATA_SIZE_32KB == 0)
            {
                block_type = HAL_FLASH_BLOCK_32K;
                eraseLength = DFU_DATA_SIZE_32KB;
            }
        }
        else if (eraseSize >= DFU_DATA_SIZE_32KB)
        {
            if(startEraseAddr % DFU_DATA_SIZE_32KB == 0)
            {
                block_type = HAL_FLASH_BLOCK_32K;
                eraseLength = DFU_DATA_SIZE_32KB;
            }
        }

        if (fota_flash_erase(startEraseAddr, block_type, FOTA_FLASH_TYPE) != FLASH_SUCCESS)
        {
            return DFU_ERR_FLASH_CTRL_4K_ERASE_FAIL;
        }

        FOTA_LOG_E("startEraseAddr=%x eraseLength=%d",startEraseAddr,eraseLength);
        startEraseAddr += eraseLength;
        eraseSize      -= eraseLength;
    }
    return DFU_ERR_SUCCESS;
#endif
}


