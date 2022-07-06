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
#include <string.h>
#include "audio_nvdm_common.h"
#include "nvkey.h"
#include "audio_log.h"

extern nvdm_status_t nvdm_query_data_item_length(const char *group_name, const char *data_item_name, uint32_t *size);
extern uint32_t DSP0_NVDM_ITEM_MAX;

sysram_status_t audio_nvdm_init_sysram(uint32_t sysram_baddr, uint16_t total_size)
{
	uint8_t		*adr_pt;

	g_sysram_info.base_addr  = (uint8_t *)sysram_baddr;
	g_sysram_info.total_size = total_size;
	g_sysram_info.used_size_in_byte = NAT_Data_Begin_Ptr;
	g_sysram_info.nvdm_num = 0;

	adr_pt = g_sysram_info.base_addr;

	memset(adr_pt, 0,  NAT_MEM_Size);// clear NAT_NVDM_INFO

	return NVDM_STATUS_NAT_OK;
};

sysram_status_t audio_nvdm_reset_sysram(void)
{
	uint8_t		*adr_pt;

	g_sysram_info.used_size_in_byte = NAT_Data_Begin_Ptr;
	g_sysram_info.nvdm_num = 0;

	adr_pt = g_sysram_info.base_addr;

	memset(adr_pt, 0,  NAT_MEM_Size);// clear NAT_NVDM_INFO

	return NVDM_STATUS_NAT_OK;
};

sysram_status_t flash_memory_read_nvdm_data(uint16_t id, uint8_t *data, uint32_t *length)
{
	return nvkey_read_data(id, data, length);
}

sysram_status_t flash_memory_write_nvdm_data(uint16_t id, uint8_t *data, uint32_t length)
{
    return nvkey_write_data(id, data, length);
}

sysram_status_t flash_memory_query_nvdm_data_length(uint16_t id, uint32_t *length)
{
    return nvkey_data_item_length(id, length);
}

sysram_status_t nvdm_get_default_info(mem_nvdm_info_t *mem_nvdm, uint16_t get_id)
{
	uint16_t	i, flag;

	for (i = flag = 0 ; i < DSP0_NVDM_ITEM_MAX ; i++ ) {
		if (get_id == NVKEY_END) break;
		if (g_nvdm_default_table[i].nvdm_id == get_id) {
			mem_nvdm->nvdm_id	= get_id;
			mem_nvdm->length	= g_nvdm_default_table[i].length;
			// mem_nvdm->struct_pt= nvdm_table->g_nvdm_default_table[i].struct_pt;
			flag = 1;
			break;
		}
	} // for(i)

	if (flag == 1) 	return NVDM_STATUS_NAT_OK;
	else			return NVDM_STATUS_NAT_ITEM_NOT_FOUND;
};

sysram_status_t dsp_feature_nvdm_write_sysram(sysram_info_t *sysram_info, uint16_t num_of_features, const DSP_FEATURE_TYPE_LIST *featurerlist)
{
	uint16_t				i, j, k, kpos, flag, keyid, kidbuf[NAT_NVDM_ITEM_MAX+1];
	DSP_FEATURE_TYPE_LIST	feature;
	mem_nvdm_info_t			flash_nvdm;
	sysram_status_t			status = NVDM_STATUS_NAT_OK;

	for ( i=0 ; i <=NAT_NVDM_ITEM_MAX ; i++ ) kidbuf[i] = 0;

	//for ( i = kpos = 0 ; ; i++ ) {
	for ( i = kpos = 0 ; i < num_of_features ; i++ ) {
		feature = featurerlist[i];
		//if (feature == FUNC_END) break;
		for ( j = 0 ; ; j++ ) {
			keyid = g_dsp_feature_table[feature].nvkey_id[j];
			if (keyid == NVKEY_END) break;

			for(k = flag = 0 ; k < kpos ; k++) {
				if (keyid == kidbuf[k]) {
					flag = 1;
					break;
				}
			} // for(k)

			if (flag==1)	continue;
			else 			kidbuf[kpos++] = keyid;

			if ( (status = nvdm_get_default_info(&flash_nvdm, keyid)) != NVDM_STATUS_NAT_OK) {
                AUD_LOG_W("[NVKEY] get default info fail %d", 1, keyid);
                return status;
            }
			if ( (status = nat_table_write_audio_nvdm_data(flash_nvdm, c_flash_mode)) != NVDM_STATUS_NAT_OK) {
                AUD_LOG_W("[NVKEY] read nvkey to audio sysram fail %d", 1, keyid);
                return status;
            }

		} // for(j)

	} // for(i)

	return NVDM_STATUS_NAT_OK;
}
