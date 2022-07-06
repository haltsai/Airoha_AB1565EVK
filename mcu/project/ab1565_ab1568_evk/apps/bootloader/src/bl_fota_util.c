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


#define _BL_FOTA_UTIL_C_

#include "fota_flash.h"
#include "fota_util.h"
#include "bl_fota_util.h"
#include "bl_fota_def.h"
#include "bsp_flash.h"
#include "fota_multi_info.h"

U32 g_fota_data_end_address = 0;

VOID bl_gen_each_4k_target_addr_table()
{
    U32 count, offset;

    for (U32 i = 0; i < g_number_of_movers; ++i)
    {
        count = g_movers[i].length / 0x1000 + (g_movers[i].length % 0x1000 ? 1 : 0);
        FOTA_LOG_I("length:%d, count:%d", g_movers[i].length, count);
        offset = 0;

#ifdef  FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
        if(g_movers[i].destination_address == 0)
        {
            is_fota_partition_upgrade = TRUE;
        }
#endif
        for (U32 j = 0; j < count; ++j)
        {
            g_fota_data_4k_target_table[g_fota_data_4k_target_table_size] = g_movers[i].destination_address + offset;
            offset += 0x1000;

            ++g_fota_data_4k_target_table_size;
        }

        FOTA_LOG_I("g_fota_data_4k_target_table[%d]-addr:%x, g_fota_data_4k_target_table[%d]-addr:%x",
                    g_fota_data_4k_target_table_size - count,
                    g_fota_data_4k_target_table[g_fota_data_4k_target_table_size - count],
                    g_fota_data_4k_target_table_size - 1,
                    g_fota_data_4k_target_table[g_fota_data_4k_target_table_size - 1]);
    }
}


bool bl_fota_is_mover_info_valid(void)
{
    U32 tmp_addr_dst = 0, len = 0, remainder = 0, tmp_addr_src = 0;
    int i, j;

    /* destination_address Bubble sort */
    for (i = g_number_of_movers; i > 1; i--)
    {
        for (j = 1; j < i; j++)
        {
            if (g_movers[j - 1].destination_address > g_movers[j].destination_address)
            {
                tmp_addr_src = g_movers[j - 1].source_address;
                tmp_addr_dst = g_movers[j - 1].destination_address;
                len = g_movers[j - 1].length;

                g_movers[j - 1].source_address = g_movers[j].source_address;
                g_movers[j - 1].destination_address = g_movers[j].destination_address;
                g_movers[j - 1].length = g_movers[j].length;

                g_movers[j].source_address = tmp_addr_src;
                g_movers[j].destination_address = tmp_addr_dst;
                g_movers[j].length = len;
            }
        }
    }

    /* Check destination_address overlap and address range. */
    for (i = g_number_of_movers - 1; i > 0; i--)
    {
        //len = ((g_movers[i - 1].length >> 12) << 12);
        len = g_movers[i - 1].length & 0xFFFFF000;
        remainder = g_movers[i - 1].length > len ? 0x1000 : 0;
        len = len + remainder;
        if (g_movers[i - 1].destination_address + len > g_movers[i].destination_address ||
            !fota_flash_is_addr_range_valid(g_movers[i - 1].destination_address, len))
        {
            FOTA_LOG_E("destination_address check failed. i:%d", i - 1);
            return FALSE;
        }
    }

    len = g_movers[g_number_of_movers - 1].length & 0xFFFFF000;
    remainder = g_movers[g_number_of_movers - 1].length > len ? 0x1000 : 0;
    len = len + remainder;
    if (!fota_flash_is_addr_range_valid(g_movers[g_number_of_movers - 1].destination_address, len))
    {
        FOTA_LOG_E("destination_address check failed. i:%d", g_number_of_movers - 1);
        return FALSE;
    }

    /* source_address Bubble sort */
    for (i = g_number_of_movers; i > 1; i--)
    {
        for (j = 1; j < i; j++)
        {
            if (g_movers[j - 1].source_address > g_movers[j].source_address)
            {
                tmp_addr_src = g_movers[j - 1].source_address;
                tmp_addr_dst = g_movers[j - 1].destination_address;
                len = g_movers[j - 1].length;

                g_movers[j - 1].source_address = g_movers[j].source_address;
                g_movers[j - 1].destination_address = g_movers[j].destination_address;
                g_movers[j - 1].length = g_movers[j].length;

                g_movers[j].source_address = tmp_addr_src;
                g_movers[j].destination_address = tmp_addr_dst;
                g_movers[j].length = len;
            }
        }
    }

    if (None == g_compression_type)
    {
        /* Check source_address overlap and address range. */
        if (g_movers[0].source_address < g_fota_data_start_address - g_fota_partition_start_address ||
        (g_movers[g_number_of_movers - 1].source_address + g_movers[g_number_of_movers - 1].length >
         g_fota_data_start_address - g_fota_partition_start_address + g_fota_data_length))
        {
            FOTA_LOG_E("source_address check failed. i:%d", g_number_of_movers - 1);
            return FALSE;
        }

        for (i = g_number_of_movers - 1; i > 0; i--)
        {
            if (g_movers[i - 1].source_address + g_movers[i - 1].length > g_movers[i].source_address)
            {
                FOTA_LOG_E("source_address check failed. i:%d", i - 1);
                return FALSE;
            }
        }
    }

    FOTA_LOG_I("mover info is valid");

    return TRUE;
}


BOOL bl_fota_init(VOID)
{
    FotaStorageType storage_type;
    uint32_t length;

    if (fota_flash_get_fota_partition_info(&storage_type, (uint32_t *)&g_fota_partition_start_address, &length) != FOTA_ERRCODE_SUCCESS)
    {
        return FALSE;
    }

    if( storage_type == ExternalFlash)
    {
#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED
        g_fota_partition_start_address |= SPI_SERIAL_FLASH_ADDRESS;
#else
        FOTA_LOG_E("External flash is not supported!");
        return FALSE;
#endif
    }
    FOTA_LOG_I("g_fota_partition_start_address[%x]",g_fota_partition_start_address);


    U32 curr_addr = g_fota_partition_start_address + SIGNATURE_SIZE; // skip signature

    /* fota length do not include last 4K byte */
    g_fota_partition_length = length - 0x1000;

    U16 tlv_type;
    U16 tlv_length;
#define TLV_BUFFER_SIZE 256
    U8 temp[TLV_BUFFER_SIZE];

    bool is_basic_info_found   = FALSE;
    bool is_mover_info_found   = FALSE;
    bool is_version_info_found = FALSE;
    bool is_nvdm_change_found = FALSE;

    while (bsp_flash_read(curr_addr, (U8*)&tlv_type, 2) == BSP_FLASH_STATUS_OK &&
           tlv_type != INVALID_TLV_VALUE)
    {
        curr_addr += 2; // point to length
        if (bsp_flash_read(curr_addr, (U8*)&tlv_length, 2) != BSP_FLASH_STATUS_OK)
        {
            FOTA_LOG_E("Read record length in FOTA header failed.");
            return FALSE;
        }

        curr_addr += 2; // point to data
        FOTA_LOG_I("tlv_type:%x, tlv_length:%d", tlv_type, tlv_length);

        if (tlv_type == FOTA_HEADER_TLV_BASIC_INFO)
        {
            FOTA_LOG_I("TLV_BASIC_INFO process");
            if (bsp_flash_read(curr_addr, &temp[0], tlv_length) != BSP_FLASH_STATUS_OK)
            {
                FOTA_LOG_E("read FOTA basic info. failed.");
                return FALSE;
            }
            FOTA_BASIC_INFO * p_basic_info = (FOTA_BASIC_INFO *)temp;
            g_compression_type = p_basic_info->compression_type; // record compresstion type for further using
            // TODO: is the fota_data_start_address a relative address?
            g_fota_data_start_address = g_fota_partition_start_address +p_basic_info->fota_data_start_address;
            g_fota_data_length = p_basic_info->fota_data_length;
            g_fota_data_end_address = g_fota_data_start_address + g_fota_data_length;

            if (!fota_flash_is_addr_range_valid(g_fota_data_start_address, g_fota_data_length))
            {
                return FALSE;
            }
            is_basic_info_found = TRUE;
        }
        else if (tlv_type == FOTA_HEADER_TLV_MOVER_INFO)
        {
            FOTA_MOVER_INFO * p_mover_info = NULL;

            FOTA_LOG_I("TLV_MOVER_INFO process");
            if (bsp_flash_read(curr_addr, &temp[0], sizeof(FOTA_MOVER_INFO)) != BSP_FLASH_STATUS_OK)
            {
                FOTA_LOG_E("read FOTA mover info. failed.");
                return FALSE;
            }
            p_mover_info = (FOTA_MOVER_INFO *)temp;
            g_number_of_movers = p_mover_info->number_of_movers;

            if (tlv_length != sizeof(p_mover_info->number_of_movers) + g_number_of_movers * sizeof(FOTA_MOVER_TRIPLET))
            {
                FOTA_LOG_E("wrong tlv_length:%d, should be=%d", tlv_length,
                            sizeof(p_mover_info->number_of_movers) + g_number_of_movers * sizeof(FOTA_MOVER_TRIPLET));
                return FALSE;
            }
            if (bsp_flash_read(curr_addr + sizeof(FOTA_MOVER_INFO), (U8 *)(&g_movers[0]), tlv_length - sizeof(FOTA_MOVER_INFO)) != BSP_FLASH_STATUS_OK)
            {
                FOTA_LOG_E("read FOTA movers failed.");
                return FALSE;
            }

            is_mover_info_found = TRUE;
        }
        else if (tlv_type == FOTA_HEADER_TLV_INTEGRITY_VERIFY_INFO)
        {
            U32 sha_number;

            FOTA_LOG_I("sha info process");

            if (bsp_flash_read(curr_addr, (uint8_t*)&sha_number, sizeof(int)) != BSP_FLASH_STATUS_OK)
            {
                FOTA_LOG_E("read sha info info. failed.");
                return FALSE;
            }

            if(g_number_of_movers != sha_number)
            {
                FOTA_LOG_E("g_number_of_movers:%x, sha_number:%x");
                return FALSE;
            }

            g_sha_info_start_address = curr_addr + 4;

            FOTA_LOG_I("g_sha_info_start_address:%x",g_sha_info_start_address);

            g_is_sha_info_found = TRUE;
        }
        else if (tlv_type == FOTA_HEADER_TLV_VERSION_INFO)
        {
            U32 tmpLength = tlv_length;

            FOTA_LOG_I("fota version process");

            if (tlv_length > FOTA_VERSION_MAX_SIZE)
            {
                FOTA_LOG_E("fota version size is not correct: lentgh = %d",tlv_length);
                tmpLength = FOTA_VERSION_MAX_SIZE - 1;
            }

            if (bsp_flash_read(curr_addr, version, tmpLength) != BSP_FLASH_STATUS_OK) {
                return FOTA_ERRCODE_FAIL;
            }

            versionLength = tmpLength;

            version[tmpLength] = '\0';

            FOTA_LOG_I("fota version :%s",version);

            is_version_info_found = TRUE;
        }
        else if (tlv_type == 0xF0)
        {
            U32 tmpLength = tlv_length;
            U32 data = 0;

            FOTA_LOG_I("fota nvdm incompatible flag found. tmpLength:%d", tmpLength);

            if (bsp_flash_read(curr_addr,  (uint8_t*)&data, tmpLength) != BSP_FLASH_STATUS_OK) {
                return FOTA_ERRCODE_FAIL;
            }

            FOTA_LOG_I("fota nvdm incompatible data = %x",data);

            if(data == 0x1)
            {
                fota_nvdm_incompatible_flag_set();
            }

            is_nvdm_change_found = TRUE;
        }
        else
        {
            FOTA_LOG_W("Unknown FOTA TLV type value: %x", tlv_type);
        }
        curr_addr += tlv_length; // point to the next record
    }

    FOTA_LOG_I("is_basic_info_found:%x, is_mover_info_found:%x is_sha_info_found:%x is_version_info_found:%x is_nvdm_change_found:%x",
                is_basic_info_found,
                is_mover_info_found,
                g_is_sha_info_found,
                is_version_info_found,
                is_nvdm_change_found);

    return is_basic_info_found && is_mover_info_found;
}

