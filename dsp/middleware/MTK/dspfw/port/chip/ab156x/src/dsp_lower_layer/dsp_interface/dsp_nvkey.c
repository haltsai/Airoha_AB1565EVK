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
#include <stdio.h>
#include <assert.h>
#include "FreeRTOS.h"
#include "audio_nvdm_common.h"
#include "exception_handler.h"
#include "dsp_temp.h"
#include "dsp_audio_msg.h"
#include "task.h"

sysram_info_t     g_sysram_info;
void dsp0_nvkey_init(uint32_t syaram_baddr, uint16_t total_size)
{
    g_sysram_info.base_addr  = (uint8_t *)syaram_baddr; // DSP0 NvKey initializing
    g_sysram_info.total_size = total_size;
};

#define NVDM_ITEM_NULL 0
// mode = c_flash_mode: Read NVDM data from FLASH(nvdm_read_data_item function) and then write to SYSRAM NVDM table.
//        c_sram_mode : Read NVDM data from SRAM                                and then write to SYSRAM NVDM table.
sysram_status_t nat_table_write_audio_nvdm_data(mem_nvdm_info_t mem_nvdm, nvdm_access_mode_t mode)
{
    uint16_t            i, valid, remain_space, chksum;
    uint8_t             *adr_pt = (uint8_t*)g_sysram_info.base_addr;
    uint8_t             *src_pt, *dst_pt;
    nat_nvdm_info_t     *nat_nvdm_info;
    sysram_status_t     status;
    UNUSED(mode);

    for ( i = valid = 0 ; i < NAT_NVDM_ITEM_MAX ; i++, adr_pt += sizeof(nat_nvdm_info) ) {
        nat_nvdm_info = (nat_nvdm_info_t*)adr_pt;
        if (nat_nvdm_info->nvdm_id == NVDM_ITEM_NULL) break;
        if (nat_nvdm_info->nvdm_id == mem_nvdm.nvdm_id) {
            valid = 1;
            break;
        }
    } // for(i)

    if ( (valid == 0) && (i == NAT_NVDM_ITEM_MAX) )
        return NVDM_STATUS_NAT_ITEM_FULL;

    if ( (valid == 1) && (nat_nvdm_info->length != mem_nvdm.length) ) // Updated the existing NVDM data
        return NVDM_STATUS_NAT_INVALID_LENGTH;

    // valid = 0: Add the NVDM Item data
    //         1: Updated the existing NVDM data
    if (valid == 0) {
        remain_space =  NAT_MEM_Size - g_sysram_info.used_size_in_byte;
        if (remain_space < mem_nvdm.length)
            return NVDM_STATUS_NAT_DATA_FULL;

        nat_nvdm_info->offset = g_sysram_info.used_size_in_byte;    // store oftset_in_byte
        g_sysram_info.used_size_in_byte += mem_nvdm.length;
        g_sysram_info.nvdm_num++;
    }

    dst_pt = g_sysram_info.base_addr + nat_nvdm_info->offset;
    //if (mode == c_flash_mode) {
    if (0) {
        //status = flash_memory_read_nvdm_data(mem_nvdm.nvdm_id, dst_pt, &mem_nvdm.length);
        if (status != NVDM_STATUS_NAT_OK)
            return status;

        for ( i = chksum = 0 ; i <mem_nvdm.length ; i++ ) {
            chksum += (uint16_t)(*dst_pt++);
        } // for(i)
    } else {
        src_pt = (uint8_t*) mem_nvdm.mem_pt;
        for ( i = chksum = 0 ; i <mem_nvdm.length ; i++ ) {
            chksum += (uint16_t)(*dst_pt++ = *src_pt++);
        } // for(i)
    }

    nat_nvdm_info->nvdm_id  = mem_nvdm.nvdm_id;
    nat_nvdm_info->length   = mem_nvdm.length;
    nat_nvdm_info->chksum   = chksum;

    return NVDM_STATUS_NAT_OK;
};

sysram_status_t nat_table_read_audio_nvdm_data(mem_nvdm_info_t mem_nvdm, nvdm_access_mode_t mode)
{
    uint16_t            i, valid, chksum;
    uint8_t             *adr_pt = (uint8_t*)g_sysram_info.base_addr;
    uint8_t             *src_pt, *dst_pt;
    nat_nvdm_info_t     *nat_nvdm_info;
    sysram_status_t     status;
    UNUSED(mode);

    for ( i = valid = 0 ; i < NAT_NVDM_ITEM_MAX ; i++, adr_pt += sizeof(nat_nvdm_info_t) ) {
        nat_nvdm_info = (nat_nvdm_info_t*)adr_pt;
        if (nat_nvdm_info->nvdm_id == NVDM_ITEM_NULL) break;
        if (nat_nvdm_info->nvdm_id == mem_nvdm.nvdm_id) {
            valid = 1;
            break;
        }
    } // for(i)

    if (valid == 0) return NVDM_STATUS_NAT_ITEM_NOT_FOUND;

    src_pt = g_sysram_info.base_addr + nat_nvdm_info->offset;

    //if (mode == c_flash_mode) {
    if (0) {

        dst_pt = src_pt;

        for ( i = chksum = 0 ; i <mem_nvdm.length ; i++ ) {
            chksum += (uint16_t)(*dst_pt++);
        } // for(i)

        if (chksum != nat_nvdm_info->chksum)
            return NVDM_STATUS_NAT_INCORRECT_CHECKSUM;
        //status = flash_memory_write_nvdm_data(mem_nvdm.nvdm_id, src_pt, mem_nvdm.length);
        if (status != NVDM_STATUS_NAT_OK)
            return status;
    } else {

        dst_pt = (uint8_t*) mem_nvdm.mem_pt;

        for ( i = chksum = 0 ; i <mem_nvdm.length ; i++ ) {
            chksum += (uint16_t)(*dst_pt++ = *src_pt++);
        } // for(i)
        //printf("dsp keyid: 0x%x, chksum:%d, length:%d", nat_nvdm_info->nvdm_id, chksum, mem_nvdm.length);

        if (chksum != nat_nvdm_info->chksum)
            return NVDM_STATUS_NAT_INCORRECT_CHECKSUM;
    }

    return NVDM_STATUS_NAT_OK;
}

sysram_status_t nvkey_read_full_key(uint16_t key_id, void *ptr, uint16_t length)
{
    mem_nvdm_info_t dsp0_nvdm;
    sysram_status_t status;

    dsp0_nvdm.nvdm_id   = key_id;
    dsp0_nvdm.length    = length;
    dsp0_nvdm.mem_pt    = ptr;
    status = nat_table_read_audio_nvdm_data(dsp0_nvdm, c_sram_mode);

    if (status != NVDM_STATUS_OK) {
        DSP_MW_LOG_E("read nvkey error, nvkey_id:0x%x status:%d", 2, key_id, status);
        if (status == NVDM_STATUS_NAT_ITEM_NOT_FOUND) {
            configASSERT(0);
        } else {
            configASSERT(0);
        }
    }

#if 0
    uint16_t    n, len;
    uint8_t     *data_pt = ptr;
    len = (length > 128) ? 128 : length;
    for( n=0 ; n <len ; n++ ) {
        DSP_MW_LOG_I("%02X", 1, *data_pt++);
    } // for(n)
    DSP_MW_LOG_I("Dump OK\n", 0);
#endif

    return status;
};

sysram_status_t nvkey_write_full_key (uint16_t key_id, void *ptr, uint16_t length)
{
    mem_nvdm_info_t dsp0_nvdm;
    sysram_status_t status;

    dsp0_nvdm.nvdm_id   = key_id;
    dsp0_nvdm.length    = length;
    dsp0_nvdm.mem_pt    = ptr;
    status = nat_table_write_audio_nvdm_data(dsp0_nvdm, c_sram_mode);

    //assert(status == NVDM_STATUS_OK); // Add expection
    if (status != NVDM_STATUS_OK) {
        configASSERT(0);
    }

    return status;
};

bool audio_nvdm_update_status(audio_nvdm_user_t user, audio_nvdm_status_t status)
{
    hal_ccni_message_t msg;

    if ((user >= AUDIO_NVDM_USER_MAX) || (status >= AUDIO_NVDM_STATUS_MAX)) {
        return false;
    }

    DSP_MW_LOG_I("[Audio][NVDM] user %d update status %d", 2, user, status);

    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = (MSG_DSP2MCU_COMMON_UPDATE_AUDIO_NVDM_STATUS << 16) | (user << 8) | status;
    msg.ccni_message[1] = 0;
    while (aud_msg_tx_handler(msg, 0, FALSE) != AUDIO_MSG_STATUS_OK) {
        vTaskDelay(2);
    }

    return true;
}

