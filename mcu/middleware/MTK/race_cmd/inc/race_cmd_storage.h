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
 
#ifndef __RACE_CMD_STORAGE_H__
#define __RACE_CMD_STORAGE_H__


#include "race_cmd_feature.h"
#ifdef RACE_STORAGE_CMD_ENABLE
#include "stdint.h"
#include "race_cmd.h"
#include "race_storage_util.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_STORAGE_WRITE_BYTE                    0x0400
#define RACE_STORAGE_WRITE_PAGE                    0x0402
#define RACE_STORAGE_READ_PAGE                     0x0403
#define RACE_STORAGE_ERASE_PARTITION               0x0404
#define RACE_STORAGE_LOCK_UNLOCK                   0x0430
#define RACE_STORAGE_GET_PARTITION_SHA256          0x0431
#define RACE_STORAGE_DUAL_DEVICES_ERASE_PARTITION  0x0432
#define RACE_STORAGE_GET_4K_ERASED_STATUS          0x0433


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


typedef struct
{
    uint8_t status;
    uint8_t storage_type;
    uint8_t agent_or_partner;
    uint32_t partition_address;
    uint32_t partition_length;
    uint8_t sha256[RACE_STORAGE_SHA256_SIZE];
} PACKED race_storage_get_partition_sha256_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t storage_type;
    uint8_t agent_or_partner;    
    uint32_t partition_address;
    uint32_t partition_length;    
    uint16_t erase_status_size;
    uint8_t erase_status[0];
}PACKED race_storage_get_4k_erased_status_noti_struct;

typedef struct
{
    uint8_t status;
    uint8_t storage_type;
    uint8_t agent_or_partner;    
    uint8_t lock_or_unlock;
}PACKED race_storage_lock_unlock_noti_struct;


typedef struct
{
    uint8_t status;
    uint8_t storage_type;    
    uint32_t partition_length;
    uint32_t partition_address;
} PACKED race_storage_erase_partition_noti_struct;


typedef struct
{
    uint8_t   status;
    uint8_t   agent_storage_type;
	uint32_t  agent_length;
	uint32_t  agent_storage_address;
	uint8_t   partner_storage_type;
	uint32_t  partner_length;
	uint32_t  partner_storage_address;
} PACKED race_storage_dual_device_erase_partition_noti_struct;

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*!
  @brief Process FLASH related RACE commands.
  @param pRaceHeaderCmd This parameter represents the raw data such as "05 5A...".
  @param Lenth Total bytes of this RACE command.
  @param channel_id Channel identifier
*/
void* race_cmdhdl_storage(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id);


bool race_storage_data_check(PTR_RACE_COMMON_HDR_STRU pCmdMsg);


#endif /* RACE_STORAGE_CMD_ENABLE */
#endif /* __RACE_CMD_STORAGE_H__ */

