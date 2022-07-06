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
#include "audio_nvdm_common.h"
#ifndef WIN32
    #include "hal_audio_internal.h"
    #include "hal_audio_message_struct.h"
#endif
#include "audio_log.h"

//#define Forced_Write_Default_NVDM

extern const uint8_t T_NVDM_BEEF[2];

sysram_info_t	  g_sysram_info;
dsp_ecnr_alg_mic_type_t gDspEcnrAlg_MicType;

static void audio_nvdm_user_handler(hal_audio_event_t event, void *data);

extern nvdm_status_t nvdm_query_data_item_length(const char *group_name, const char *data_item_name, uint32_t *size);
extern uint32_t DSP0_NVDM_ITEM_MAX;

sysram_status_t audio_dsp0_nvdm_init(void)
{
    uint32_t		i;
    mem_nvdm_info_t mem_nvdm;
    sysram_status_t status = NVDM_STATUS_NAT_OK;
    uint32_t tableSize = 0;
    char id_hex[5];

#ifndef Forced_Write_Default_NVDM
    uint32_t        length;
    uint8_t         cm4_buf[sizeof(T_NVDM_BEEF)];
#endif

    AUD_LOG_I("Audio DSP0 NVDM INIT ... !\n", 0);

#ifdef Forced_Write_Default_NVDM
    if (0) {
#else
    length = sizeof(T_NVDM_BEEF);
    status = flash_memory_read_nvdm_data(NVKEY_CM4_COMMON, (uint8_t *)&cm4_buf, &length);
    if ( (status == NVDM_STATUS_NAT_OK) && (cm4_buf[0] == T_NVDM_BEEF[0]) ) {
#endif
        AUD_LOG_I("There is already audio nvdm data on the FLASH memory, which will be kept the previous nvdm data.\n\n", 0);
    } else {
        AUD_LOG_I("There is no audio nvdm data on the FLASH memory, which writes audio nvdm default table (nvdm_audio_coef_default.h) to FLASH memory.\n\n", 0);
        for (i = 0 ; i < DSP0_NVDM_ITEM_MAX ; i++ ) {
            mem_nvdm = g_nvdm_default_table[i];
            if (mem_nvdm.nvdm_id == NVKEY_END)  break;
            if (mem_nvdm.length	== 0)           continue;
            snprintf(id_hex, sizeof(id_hex),"%X", mem_nvdm.nvdm_id);
            if (NVDM_STATUS_ITEM_NOT_FOUND == nvdm_query_data_item_length("AB15", id_hex, &tableSize)){
                status = flash_memory_write_nvdm_data(mem_nvdm.nvdm_id, (uint8_t*)mem_nvdm.mem_pt, mem_nvdm.length);
                if (status != NVDM_STATUS_NAT_OK) break;
            } else {
                if(tableSize != mem_nvdm.length){
                    AUD_LOG_I("[Audio][NVDM]id:0x%x length not match. Table_size(%d), Nvdm_size(%d)\n\n", 3, mem_nvdm.nvdm_id, mem_nvdm.length, tableSize);
#ifdef MTK_AUDIO_NVDM_AUTO_UPDATE /*nvdm_rom_book.h*/
                    if(tableSize < mem_nvdm.length){
                    /*NVDM old version, Software update.*/
                        if((mem_nvdm.nvdm_id == 0xF28F) || (mem_nvdm.nvdm_id == 0xE033) || (mem_nvdm.nvdm_id == 0xE043)){  //Add check list.
                            uint8_t *nvdm_data_new = NULL;
                            nvdm_data_new = (uint8_t *)pvPortMalloc(mem_nvdm.length);
                            if(nvdm_data_new == NULL){
                                AUD_LOG_E("[Audio][NVDM][ERROR]nvdm_data_new pvPortMalloc fail.", 0);
                            }
                            status = flash_memory_read_nvdm_data(mem_nvdm.nvdm_id,(uint8_t *)nvdm_data_new,&tableSize);
                            if (status != NVDM_STATUS_NAT_OK){
                                AUD_LOG_E("[Audio][NVDM][ERROR]flash_memory_read_nvdm_data fail. Status(%d)", 1, status);
                            }
                            //AUD_LOG_I("[Audio][NVDM]mem_nvdm.mem_pt(0x%x)(0x%x), nvdm_data_new(0x%x)(0x%x)", 4,mem_nvdm.mem_pt, (uint8_t *)(mem_nvdm.mem_pt + tableSize), nvdm_data_new, (uint8_t *)(nvdm_data_new + tableSize));
                            memcpy((uint8_t *)(nvdm_data_new + tableSize), (uint8_t *)(mem_nvdm.mem_pt + tableSize), mem_nvdm.length - tableSize);
                            status = flash_memory_write_nvdm_data(mem_nvdm.nvdm_id, (uint8_t*)nvdm_data_new, mem_nvdm.length);
                            if (status != NVDM_STATUS_NAT_OK){
                                AUD_LOG_E("[Audio][NVDM][ERROR]Write nvdm data fail. Status(%d)", 1, status);
                            }
                            vPortFree(nvdm_data_new);
                        }
                    }
#endif
                }
            }
        } // for(i)

    }

#ifdef WIN32
	audio_nvdm_init_sysram((uint32_t)g_SYS_SRAM, NAT_MEM_Size);	// Initialize the NAT
#else
	n9_dsp_share_info_t *info;
	info = hal_audio_query_nvkey_parameter_share_info();
	//printf("CM4 length = %d Addr = %x\n", info->length, info->start_addr);
	audio_nvdm_init_sysram(info->start_addr, info->length);	// Initialize the NAT
#endif

    /* For event notice from DSP side */
    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_AUDIO_NVDM, audio_nvdm_user_handler, NULL);

	return status;
};

sysram_status_t audio_nvdm_set_feature(uint16_t num_of_features, const DSP_FEATURE_TYPE_LIST *featurerlist)
{
	sysram_status_t	status;

	// According to DSP feature, the related NVKey data of FLASH memory writes to the NAT.
	status = dsp_feature_nvdm_write_sysram(&g_sysram_info, num_of_features, featurerlist);

	return status;
};

sysram_status_t audio_nvdm_set_nvkeys(uint16_t num_of_nvkeys, const DSP_ALG_NVKEY_e *array_of_nvkeys)
{
	uint16_t		i, keyid;
	mem_nvdm_info_t	flash_nvdm;
	sysram_status_t	status = NVDM_STATUS_NAT_OK;

	for ( i = 0 ; i < num_of_nvkeys ; i++ ) {
		keyid = array_of_nvkeys[i];
		if (keyid == NVKEY_END) break;

		if ( (status = nvdm_get_default_info(&flash_nvdm, keyid))  != NVDM_STATUS_NAT_OK)					return status;
		if ( (status = nat_table_write_audio_nvdm_data(flash_nvdm, c_flash_mode)) != NVDM_STATUS_NAT_OK)	return status;
	} // for(i)

	return	status;
};

#define NVDM_ITEM_NULL 0
// mode = c_flash_mode: Read NVDM data from FLASH(nvdm_read_data_item function) and then write to SYSRAM NVDM table.
//		  c_sram_mode :	Read NVDM data from SRAM                                and then write to SYSRAM NVDM table.
sysram_status_t nat_table_write_audio_nvdm_data(mem_nvdm_info_t mem_nvdm, nvdm_access_mode_t mode)
{
    uint16_t			i, valid, chksum;
    uint32_t            remain_space;
    uint8_t	            *adr_pt = (uint8_t*)g_sysram_info.base_addr;
    uint8_t				*src_pt, *dst_pt;
    nat_nvdm_info_t		*nat_nvdm_info;
    sysram_status_t		status;

    for ( i = valid = 0 ; i < NAT_NVDM_ITEM_MAX ; i++, adr_pt += sizeof(nat_nvdm_info_t) ) {
        nat_nvdm_info = (nat_nvdm_info_t*)adr_pt;
        if (nat_nvdm_info->nvdm_id == NVDM_ITEM_NULL) break;
        if (nat_nvdm_info->nvdm_id == mem_nvdm.nvdm_id) {
            remain_space = nat_nvdm_info->length;
            valid = 1;
            break;
        }
    } // for(i)

    if ( (valid == 0) && (i == NAT_NVDM_ITEM_MAX) )
        return NVDM_STATUS_NAT_ITEM_FULL;

    if ( (valid == 1) && (nat_nvdm_info->length != mem_nvdm.length) && (mode != c_flash_mode)) // Updated the existing NVDM data
        return NVDM_STATUS_NAT_INVALID_LENGTH;

    // valid = 0: Add the NVDM Item data
    //	        1: Updated the existing NVDM data
    if (valid == 0) {
        remain_space =  NAT_MEM_Size - g_sysram_info.used_size_in_byte;
        if (remain_space < mem_nvdm.length)
            return NVDM_STATUS_NAT_DATA_FULL;

        nat_nvdm_info->offset = g_sysram_info.used_size_in_byte;	// store oftset_in_byte
    }

    dst_pt = g_sysram_info.base_addr + nat_nvdm_info->offset;
    if (mode == c_flash_mode) {
        status = flash_memory_read_nvdm_data(mem_nvdm.nvdm_id, dst_pt, &remain_space);
        if (status != NVDM_STATUS_NAT_OK)
            return status;

        mem_nvdm.length = remain_space;
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
        mem_nvdm.nvdm_id = nat_table_nvkey_id_replace(mem_nvdm.nvdm_id);
#endif
        if(mem_nvdm.nvdm_id == NVKEY_DSP_PARA_AEC_NR){
            gDspEcnrAlg_MicType = nat_table_get_dsp_ecnr_algorithm_type(dst_pt);
        }

        for ( i = chksum = 0 ; i < mem_nvdm.length ; i++ ) {
            chksum += (uint16_t)(*dst_pt++);
        } // for(i)
    } else {
        src_pt = (uint8_t*) mem_nvdm.mem_pt;
        for ( i = chksum = 0 ; i <mem_nvdm.length ; i++ ) {
            chksum += (uint16_t)(*dst_pt++ = *src_pt++);
        } // for(i)
    }

    if(valid == 0) {
        g_sysram_info.used_size_in_byte += mem_nvdm.length;
        g_sysram_info.nvdm_num++;
    }

    nat_nvdm_info->nvdm_id	= mem_nvdm.nvdm_id;
	nat_nvdm_info->length	= mem_nvdm.length;
	nat_nvdm_info->chksum	= chksum;

	return NVDM_STATUS_NAT_OK;
};

sysram_status_t nat_table_read_audio_nvdm_data(mem_nvdm_info_t mem_nvdm, nvdm_access_mode_t mode)
{
	uint16_t            i, valid, chksum;
	uint8_t             *adr_pt = (uint8_t*)g_sysram_info.base_addr;
	uint8_t             *src_pt, *dst_pt;
	nat_nvdm_info_t     *nat_nvdm_info;
	sysram_status_t     status;

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

	if (mode == c_flash_mode) {

		dst_pt = src_pt;

		for ( i = chksum = 0 ; i <mem_nvdm.length ; i++ ) {
			chksum += (uint16_t)(*dst_pt++);
		} // for(i)

		if (chksum != nat_nvdm_info->chksum)
		    return NVDM_STATUS_NAT_INCORRECT_CHECKSUM;
		status = flash_memory_write_nvdm_data(mem_nvdm.nvdm_id, src_pt, mem_nvdm.length);
		if (status != NVDM_STATUS_NAT_OK)
		    return status;
	} else {

		dst_pt = (uint8_t*) mem_nvdm.mem_pt;

		for ( i = chksum = 0 ; i < mem_nvdm.length ; i++ ) {
			chksum += (uint16_t)(*dst_pt++ = *src_pt++);
		} // for(i)

		if (chksum != nat_nvdm_info->chksum)
		    return NVDM_STATUS_NAT_INCORRECT_CHECKSUM;
	}

	return NVDM_STATUS_NAT_OK;
}

dsp_ecnr_alg_mic_type_t nat_table_get_dsp_ecnr_algorithm_type(uint8_t * dst_pt)
{
#if defined(MTK_INEAR_ENHANCEMENT)
    AUD_LOG_I("[HFP CODEC]: 1 + 1 algorithm is select", 0);
#elif defined(MTK_DUALMIC_INEAR)
    AUD_LOG_I("[HFP CODEC]: 2 + 1 algorithm is select", 0);
#elif defined(MTK_3RD_PARTY_NR)
    AUD_LOG_I("[HFP CODEC]: 3RD PARTY algorithm is select", 0);
#else
    AUD_LOG_I("[HFP CODEC]: 1/2 algorithm is select", 0);
#endif

    if(*(dst_pt +2) == 0x5) {
         AUD_LOG_I("[HFP][AEC_NR NVKEY], It's 2 mic algorithm 0x%x",1, *(dst_pt +2));
         return ECNR_DUAL_MIC;
    } else if (*(dst_pt +2) == 0x3) {
         AUD_LOG_I("[HFP][AEC_NR NVKEY], It's 1 mic algorithm 0x%x",1, *(dst_pt +2));
         return ECNR_SINGLE_MIC;
    } else {
         AUD_LOG_I("[HFP][AEC_NR NVKEY], It's Unkown Mic Setting 0x%x",1, *(dst_pt +2));
         return ECNR_UNKOWN_TYPE;
    }
}

uint16_t nat_table_nvkey_id_replace(uint16_t nvdm_id)
{
    uint16_t replace_nvdm_id;
    if(nvdm_id == NVKEY_DSP_PARA_AEC_NR_BOOMIC){
        replace_nvdm_id = NVKEY_DSP_PARA_AEC_NR;
    } else if (nvdm_id == NVKEY_DSP_PARA_NB_TX_EQ_BOOMIC) {
        replace_nvdm_id = NVKEY_DSP_PARA_NB_TX_EQ;
    } else if (nvdm_id == NVKEY_DSP_PARA_WB_TX_EQ_BOOMIC) {
        replace_nvdm_id = NVKEY_DSP_PARA_WB_TX_EQ;
    } else if (nvdm_id == NVKEY_DSP_PARA_WB_TX_VO_CPD_BOOMIC) {
        replace_nvdm_id = NVKEY_DSP_PARA_WB_TX_VO_CPD;
    } else if (nvdm_id == NVKEY_DSP_PARA_NB_TX_FIR_EQ_BOOMIC) {
        replace_nvdm_id = NVKEY_DSP_PARA_NB_TX_FIR_EQ;
    } else if (nvdm_id == NVKEY_DSP_PARA_WB_TX_FIR_EQ_BOOMIC) {
        replace_nvdm_id = NVKEY_DSP_PARA_WB_TX_FIR_EQ;
    } else {
        replace_nvdm_id = nvdm_id;
    }

    if(replace_nvdm_id != nvdm_id) {
        AUD_LOG_I("nat_table_nvkey_id_replace, mem_nvdm.nvdm_id = 0x%x", 2, nvdm_id, replace_nvdm_id);
    }
    return replace_nvdm_id;
}

typedef struct {
    audio_nvdm_user_callback_t callback;
    void *user_data;
} audio_nvdm_user_info_t;

static audio_nvdm_user_info_t g_audio_nvdm_user_info[AUDIO_NVDM_USER_MAX];

bool audio_nvdm_register_user_callback(audio_nvdm_user_t user, audio_nvdm_user_callback_t callback, void *user_data)
{
    if ((user >= AUDIO_NVDM_USER_MAX) || (callback == NULL)) {
        return false;
    }

    AUD_LOG_I("[Audio][NVDM] user %d register callback 0x%08x", 2, user, callback);

    g_audio_nvdm_user_info[user].callback = callback;
    g_audio_nvdm_user_info[user].user_data = user_data;

    return true;
}

bool audio_nvdm_update_status(audio_nvdm_user_t user, audio_nvdm_status_t status)
{
    if ((user >= AUDIO_NVDM_USER_MAX) || (status >= AUDIO_NVDM_STATUS_MAX)) {
        return false;
    }

    if (g_audio_nvdm_user_info[user].callback == NULL) {
        AUD_LOG_E("[Audio][NVDM] user %d update status %d, call user callback NULL!!", 2, user, status);
        return false;
    }

    AUD_LOG_I("[Audio][NVDM] user %d update status %d, call user callback 0x%08x", 3, user, status, g_audio_nvdm_user_info[user].callback);

    g_audio_nvdm_user_info[user].callback(user, status, g_audio_nvdm_user_info[user].user_data);

    return true;
}

static void audio_nvdm_user_handler(hal_audio_event_t event, void *data)
{
    audio_nvdm_user_t user;
    audio_nvdm_status_t status;

    event = event;

    user = ((*(uint32_t *)data) >> 8) & 0xFF;
    status = (*(uint32_t *)data) & 0xFF;

    if (g_audio_nvdm_user_info[user].callback == NULL) {
        AUD_LOG_E("[Audio][NVDM] user %d update status %d, call user callback NULL!!", 2, user, status);
        return;
    }

    AUD_LOG_I("[Audio][NVDM] user %d update status %d, call user callback 0x%08x", 3, user, status, g_audio_nvdm_user_info[user].callback);

    g_audio_nvdm_user_info[user].callback(user, status, g_audio_nvdm_user_info[user].user_data);
}

