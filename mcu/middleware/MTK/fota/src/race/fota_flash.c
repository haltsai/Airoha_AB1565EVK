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

/**
 * File: fota_flash.c
 *
 * Description: This file defines and implements some flash related interfaces.
 *
 * Note: See doc/Airoha_IoT_SDK_Firmware_Update_Developers_Guide.pdf for more detail.
 *
 */


#include "fota_util.h"
#include "fota_multi_info.h"
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
#include "bsp_flash.h"
#include <assert.h>
#endif
#include "fota_flash.h"
#include "fota_multi_info_util.h"
#include "fota_platform.h"


fota_flash_partition_info *g_flash_partition_info = NULL; /* This variable stores the flash partition information. */
FotaStorageType g_flash_storage_type = InternalFlash; /* This variable indicates the flash type of the FOTA partition. */


void fota_flash_set_storage_type(FotaStorageType storage_type)
{
    if (InternalFlash == storage_type ||
        ExternalFlash == storage_type) {
        g_flash_storage_type = storage_type;
    }
}


FotaStorageType fota_flash_get_storage_type(void)
{
    return g_flash_storage_type;
}


void fota_init_flash(void)
{
    /* Initialize the internal flash. */
    fota_flash_init(TRUE);
    /* Initialize the external flash. */
    fota_flash_init(FALSE);

    /* Set the flash partition information. */
    fota_flash_config_init(&g_flash_partition_info);

#if FOTA_STORE_IN_EXTERNAL_FLASH
    fota_flash_set_storage_type(ExternalFlash);
#else
    fota_flash_set_storage_type(InternalFlash);
#endif

    fota_multi_info_sector_init();
}


#ifdef BL_FOTA_ENABLE
bool fota_flash_is_addr_range_valid(uint32_t addr, uint32_t length)
{
    uint32_t i = 0;
#ifdef FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
    fota_flash_partition_info *partition_info = NULL;
    bool recorded_addr_range = FALSE; /* The variable indicates if the address range accesses any partition in g_flash_partition_info. */
#endif

#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
    if (SPI_SERIAL_FLASH_ADDRESS & addr) {
        /* Return true if the address range is in the external flash. */
        if (length) {
            return TRUE;
        }

        return FALSE;
    }
#endif

    if (FLASH_CONFIG_BASE != (addr & 0xFF000000) &&
        0 != (addr & 0xFF000000)) {
        FOTA_LOG_MSGID_W("Invalid address range. addr:%x len:%x", 2, addr, length);
        return FALSE;
    }

    if (FLASH_CONFIG_BASE == (addr & 0xFF000000)) {
        FOTA_LOG_MSGID_I("addr:%x with the address base.", 1, addr);
        addr -= FLASH_CONFIG_BASE;
    }

#ifdef FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
    /* Return false if the address range includes any non-updatable partition. */
#if defined(MTK_SECURE_BOOT_ENABLE)
    partition_info = fota_flash_get_partition_info(FLASH_PARTITION_TYPE_SECURITY_HEADER1);
    if (partition_info &&
        !(addr >= partition_info->LoadAddressLow + partition_info->BinaryLengthLow ||
          addr + length <= partition_info->LoadAddressLow))
    {
        FOTA_LOG_MSGID_W("Invalid address range. addr:%x len:%x", 2, addr, length);
        return FALSE;
    }
#endif

    partition_info = fota_flash_get_partition_info(FLASH_PARTITION_TYPE_BOOTLOADER);
    if (partition_info &&
        !(addr >= partition_info->LoadAddressLow + partition_info->BinaryLengthLow ||
          addr + length <= partition_info->LoadAddressLow))
    {
        FOTA_LOG_MSGID_W("Invalid address range. addr:%x len:%x", 2, addr, length);
        return FALSE;
    }
#endif

    while (g_flash_partition_info &&
           FLASH_PARTITION_TYPE_MAX != g_flash_partition_info[i].partition_type) {
#ifdef FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
        if (!recorded_addr_range &&
            !(addr >= g_flash_partition_info[i].LoadAddressLow + g_flash_partition_info[i].BinaryLengthLow ||
              addr + length <= g_flash_partition_info[i].LoadAddressLow)) {
            recorded_addr_range = TRUE;
        }
#endif

        if (addr >= g_flash_partition_info[i].LoadAddressLow &&
            (addr + length <= g_flash_partition_info[i].LoadAddressLow + g_flash_partition_info[i].BinaryLengthLow)) {
            return TRUE;
        }
        i++;
    }

#ifdef FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
    /* Return true if the address range does not access any partition in g_flash_partition_info. */
    if (!recorded_addr_range) {
        FOTA_LOG_MSGID_W("Not recorded address range. addr:%x len:%x", 2, addr, length);
        return TRUE;
    }
#endif

    FOTA_LOG_MSGID_W("Invalid address range. addr:%x len:%x", 2, addr, length);
    return FALSE;
}
#endif


fota_flash_partition_info *fota_flash_get_partition_info(flash_partition_type_enum partition_type)
{
    uint32_t i = 0;

    while (g_flash_partition_info &&
           FLASH_PARTITION_TYPE_MAX != g_flash_partition_info[i].partition_type &&
           partition_type != g_flash_partition_info[i].partition_type) {
        i++;
    }

    if (g_flash_partition_info && partition_type == g_flash_partition_info[i].partition_type) {
        FOTA_LOG_MSGID_I("Found, partition_type:%d", 1, partition_type);
        return g_flash_partition_info + i;
    }

    FOTA_LOG_MSGID_W("Not Found, partition_type:%d", 1, partition_type);

    return NULL;
}


FOTA_ERRCODE fota_flash_get_fota_partition_info(FotaStorageType *storage_type, uint32_t *fota_address, uint32_t *length)
{
    fota_flash_partition_info *partition_info = NULL;
    flash_partition_type_enum partition_type = FLASH_PARTITION_TYPE_MAX;

    if (!storage_type || !fota_address || !length) {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    *storage_type = fota_flash_get_storage_type();

    if (InternalFlash == *storage_type) {
        partition_type = FLASH_PARTITION_TYPE_FOTA;
    }
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
    else if (ExternalFlash == *storage_type) {
        partition_type = FLASH_PARTITION_TYPE_FOTA_EXT;
    }
#endif
    else {
        return FOTA_ERRCODE_UNSUPPORTED;
    }

    partition_info = fota_flash_get_partition_info(partition_type);
    if (!partition_info) {
        return FOTA_ERRCODE_FAIL;
    }

    *fota_address = partition_info->LoadAddressLow;
    *length = partition_info->BinaryLengthLow;

    return FOTA_ERRCODE_SUCCESS;
}


FOTA_ERRCODE fota_flash_init(bool is_int)
{
    FOTA_ERRCODE ret = FOTA_ERRCODE_FAIL;
    static bool is_hal_inited = FALSE;

    if (is_int) {
        if (!is_hal_inited) {
            ret = HAL_FLASH_STATUS_OK == hal_flash_init() ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
            is_hal_inited = TRUE;
        } else {
            ret = FOTA_ERRCODE_SUCCESS;
        }
    } else {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        /* Do not care about the return value because another task may have initialized the SPI external flash and the SPI external
         * flash only supports the 'init-deinit-init' flow.
         */
        ret = bsp_flash_init();
        FOTA_LOG_MSGID_I("bsp_flash_init() ret:%d", 1, ret);
        ret = FOTA_ERRCODE_SUCCESS;
#else
        ret = FOTA_ERRCODE_UNSUPPORTED;
#endif
    }

    return ret;
}


#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
static bsp_block_size_type_t fota_flash_block_type_cnvt(hal_flash_block_t block_type)
{
    switch (block_type) {
        case HAL_FLASH_BLOCK_4K:
            /* The block type of the 4K size block. */
            return BSP_FLASH_BLOCK_4K;

        case HAL_FLASH_BLOCK_32K:
            /* The block type of the 32K size block. */
            return BSP_FLASH_BLOCK_32K;

        case HAL_FLASH_BLOCK_64K:
            /* The block type of the 64K size block. */
            return BSP_FLASH_BLOCK_64K;

        default:
            assert(0); /* Invalid block_type. There must be something wrong. Assert to debug. */
            break;
    }

    return BSP_FLASH_BLOCK_4K;
}
#endif


FOTA_ERRCODE fota_flash_erase(uint32_t start_address, hal_flash_block_t block_type, bool is_int)
{
    int32_t ret = FOTA_ERRCODE_FAIL;

    if (is_int) {
        ret = hal_flash_erase(start_address, block_type);
        FOTA_LOG_MSGID_I("hal_flash_erase ret:%d", 1, ret);
        return HAL_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
    } else {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        /* Support the address with or without SPI_SERIAL_FLASH_ADDRESS. */
        start_address |= SPI_SERIAL_FLASH_ADDRESS;
        ret = bsp_flash_erase(start_address,
                              fota_flash_block_type_cnvt(block_type));
        FOTA_LOG_MSGID_I("bsp_flash_erase ret:%d start_address:%x", 2, ret, start_address);
        if (BSP_FLASH_STATUS_NOT_INIT == ret) {
            /* Other task may deinitialize the external flash. */
            bsp_flash_init();
            ret = bsp_flash_erase(start_address,
                                  fota_flash_block_type_cnvt(block_type));
        }

        FOTA_LOG_MSGID_I("bsp_flash_erase ret:%d", 1, ret);
        return BSP_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
#else
        return FOTA_ERRCODE_UNSUPPORTED;
#endif
    }
}


FOTA_ERRCODE fota_flash_write(uint32_t start_address, const uint8_t *data, uint32_t length, bool is_int)
{
    int32_t ret = FOTA_ERRCODE_FAIL;

    if (is_int) {
        ret = hal_flash_write(start_address, data, length);
        FOTA_LOG_MSGID_I("hal_flash_write ret:%d", 1, ret);
        return HAL_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
    } else {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        /* Support the address with or without SPI_SERIAL_FLASH_ADDRESS. */
        start_address |= SPI_SERIAL_FLASH_ADDRESS;
        ret = bsp_flash_write(start_address, (uint8_t *)data, length);
        FOTA_LOG_MSGID_I("bsp_flash_write ret:%d start_address:%x length:%d", 3, ret, start_address, length);
        if (BSP_FLASH_STATUS_NOT_INIT == ret) {
            /* Other task may deinitialize the external flash. */
            bsp_flash_init();
            ret = bsp_flash_write(start_address, (uint8_t *)data, length);
        }
        FOTA_LOG_MSGID_I("bsp_flash_write ret:%d", 1, ret);
        return BSP_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
#else
        return FOTA_ERRCODE_UNSUPPORTED;
#endif
    }
}


FOTA_ERRCODE fota_flash_read(uint32_t start_address, uint8_t *buffer, uint32_t length, bool is_int)
{
    int32_t ret = FOTA_ERRCODE_FAIL;


    if (is_int) {
        ret = hal_flash_read(start_address, buffer, length);
        return HAL_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
    } else {
#ifdef FOTA_EXTERNAL_FLASH_SUPPORT
        /* Support the address with or without SPI_SERIAL_FLASH_ADDRESS. */
        start_address |= SPI_SERIAL_FLASH_ADDRESS;
        ret = bsp_flash_read(start_address, buffer, length);
        if (BSP_FLASH_STATUS_NOT_INIT == ret) {
            /* Other task may deinitialize the external flash. */
            bsp_flash_init();
            ret = bsp_flash_read(start_address, buffer, length);
        }

        return BSP_FLASH_STATUS_OK == ret ? FOTA_ERRCODE_SUCCESS : FOTA_ERRCODE_FAIL;
#else
        return FOTA_ERRCODE_UNSUPPORTED;
#endif
    }
}


#ifdef BL_FOTA_ENABLE
/**
 * @brief This function returns the erased status of a specific 4K flash. This function is designed only for the bootloader.
 * @param[out] is_erased The erased status of the specific 4K flash.
 * @param[in] start_address The start address of the specific 4K flash.
 * @param[in] is_int It indicates the flash type of the specific 4K flash. TRUE, the internal flash. FALSE, the external flash.
 * @return FOTA_ERRCODE_SUCCESS, succeed. Other values, fail.
 */
static FOTA_ERRCODE fota_flash_is_4K_block_erased(bool *is_erased, uint32_t start_address, bool is_int)
{
#define FOTA_FLASH_4K_READ_EACH_LEN    (1024)

    uint8_t buffer[FOTA_FLASH_4K_READ_EACH_LEN] = {0};
    FOTA_ERRCODE ret = FOTA_ERRCODE_FAIL;
    uint32_t count = 0, i = 0, j = 0;

    if (start_address & 0xFFF || !is_erased ||
        4096 % FOTA_FLASH_4K_READ_EACH_LEN) {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    *is_erased = FALSE;

    count = 4096 / FOTA_FLASH_4K_READ_EACH_LEN;
    for (i = 0; i < count; i++) {
        ret = fota_flash_read(start_address, buffer, FOTA_FLASH_4K_READ_EACH_LEN, is_int);
        if (FOTA_ERRCODE_SUCCESS != ret) {
            return ret;
        }

        for (j = 0; j < FOTA_FLASH_4K_READ_EACH_LEN; j++) {
            if (0xFF != buffer[j]) {
                /* If there is a byte not equal to 0xFF, the flash is not erased. */
                return FOTA_ERRCODE_SUCCESS;
            }
        }

        start_address += FOTA_FLASH_4K_READ_EACH_LEN;
    }

    *is_erased = TRUE;
    return FOTA_ERRCODE_SUCCESS;
}
#endif


FOTA_ERRCODE fota_flash_get_partition_erase_status(uint8_t *erase_status,
                                                   uint16_t *erase_status_size,
                                                   uint32_t start_address,
                                                   uint32_t length,
                                                   bool is_int)
{
#ifdef BL_FOTA_ENABLE
    uint32_t bit_size = 0, curr_bit_pos = 0;
    bool is_erased = FALSE;
    FOTA_ERRCODE ret = FOTA_ERRCODE_FAIL;
    uint16_t erase_status_size_old = 0;

    FOTA_LOG_MSGID_I("erase_status:%x,erase_status_size:%x,start_address:%x,length:%x,is_int:%d", 5,
                     erase_status,
                     erase_status_size,
                     start_address,
                     length,
                     is_int);

    if (!erase_status_size ||
#ifndef FOTA_ERASE_VERIFY_LEN
        start_address & 0xFFF)
#else
        start_address & 0xFFF || length & 0xFFF)
#endif
    {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    erase_status_size_old = *erase_status_size;

#ifndef FOTA_ERASE_VERIFY_LEN
    bit_size = ((length >> 12) + ((length & 0xFFF) > 0 ? 1 : 0));
#else
    bit_size = length >> 12;
#endif
    *erase_status_size = ((bit_size >> 3) + (bit_size % 8 ? 1 : 0));

    FOTA_LOG_MSGID_I("bit_size:%d, *erase_status_size:%d", 2, bit_size, *erase_status_size);
    if (!erase_status) {
        /* Return erase_status_size only. It is the minimum size of erase_status needed for the same address range. */
        return FOTA_ERRCODE_SUCCESS;
    }

    FOTA_LOG_MSGID_I("erase_status_size_old:%d, *erase_status_size:%d", 2,
                     erase_status_size_old,
                     *erase_status_size);
    if (erase_status_size_old < *erase_status_size) {
        return FOTA_ERRCODE_FAIL;
    }

    memset(erase_status, 0, erase_status_size_old);

    curr_bit_pos = 0;

    while (curr_bit_pos < bit_size) {
        ret = fota_flash_is_4K_block_erased(&is_erased, start_address, is_int);
        if (FOTA_ERRCODE_SUCCESS != ret) {
            return ret;
        }

        /* If the current 4K size flash is erased, set the bit representing it in erase_status to be 1. */
        if (is_erased) {
            erase_status[curr_bit_pos / 8] |= (0x80 >> (curr_bit_pos % 8));
        }

        curr_bit_pos++;
        start_address += 0x1000;
    }

    for (int k = 0; k < *erase_status_size; k++) {
        FOTA_LOG_MSGID_I("erase_status[%d]:%x", 2, k, erase_status[k]);
    }

    return FOTA_ERRCODE_SUCCESS;
#else
    return FOTA_ERRCODE_UNSUPPORTED;
#endif
}

