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

#ifndef __RACE_LPCOMM_MSG_STRUCT__
#define __RACE_LPCOMM_MSG_STRUCT__


#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_storage_util.h"
#include "race_fota_util.h"
#include "race_event_internal.h"


/********************************REQ DEFINE START********************************/
typedef struct
{
    uint8_t fota_mode;
}PACKED race_lpcomm_fota_start_req_struct;

typedef struct
{
    uint16_t fota_state;
}PACKED race_lpcomm_fota_write_state_req_struct;

typedef struct
{
    uint8_t partition_id;
}PACKED race_lpcomm_fota_query_partition_info_req_struct;

typedef struct
{
    uint8_t storage_type;
    uint32_t partition_address;
    uint32_t partition_length;
}PACKED race_lpcomm_storage_get_partition_sha256_req_struct;

typedef struct
{
    uint8_t storage_type;
    uint32_t partition_address;
    uint32_t partition_length;
}PACKED race_lpcomm_storage_get_4k_erase_status_req_struct;


typedef struct
{
    int8_t result;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}PACKED race_lpcomm_fota_stop_result_req_struct;


typedef struct
{
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}PACKED race_lpcomm_fota_stop_query_req_struct;

typedef struct
{
    uint8_t storage_type;
    uint8_t lock_or_unlock;
}PACKED race_lpcomm_storage_lock_unlock_req_struct;

typedef struct
{
    uint8_t storage_type;
    uint32_t partition_address;
    uint32_t partition_length;
}PACKED race_lpcomm_storage_dual_device_erase_partition_req_struct;


#ifdef RACE_FOTA_CMD_ENABLE
typedef struct
{
    uint32_t partition_address;
    uint32_t partition_length;
}PACKED race_lpcomm_flash_dual_devices_partition_erase_req_struct;
#endif


typedef struct
{
    uint16_t agent_fota_state;
}PACKED race_lpcomm_fota_dual_device_query_state_req_struct;


#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
typedef struct
{
    uint8_t storage_type;
}PACKED race_lpcomm_fota_check_integrity_req_struct;
#endif


typedef struct
{
    uint8_t address[RACE_EVENT_REMOTE_DEVICE_ADDRESS_LENGTH];
    uint8_t address_length;
}PACKED race_lpcomm_fota_commit_req_struct;


#ifdef RACE_FIND_ME_ENABLE
typedef struct
{
    uint8_t is_tone;
    uint8_t is_blink;
}PACKED race_lpcomm_find_me_req_struct;
#endif
/********************************REQ DEFINE END********************************/


/********************************RSP DEFINE START********************************/

typedef struct
{
    int8_t status;
}PACKED race_lpcomm_rsp_template_struct;


typedef struct
{
    int8_t status;
    uint16_t req_len;
    uint8_t req[0];
}PACKED race_lpcomm_fake_rsp_struct;


typedef struct
{
    int8_t status;
    uint16_t fota_state;
}PACKED race_lpcomm_fota_query_state_rsp_struct;

typedef race_lpcomm_rsp_template_struct race_lpcomm_fota_write_state_rsp_struct;
typedef race_lpcomm_rsp_template_struct race_lpcomm_fota_new_transaction_rsp_struct;
typedef race_lpcomm_rsp_template_struct race_lpcomm_fota_start_rsp_struct;
typedef race_lpcomm_rsp_template_struct race_lpcomm_fota_commit_rsp_struct;
typedef race_lpcomm_rsp_template_struct race_lpcomm_fota_active_fota_preparation_rsp_struct;

typedef struct
{
    int8_t status;
    int8_t result;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}PACKED race_lpcomm_fota_stop_result_rsp_struct;

typedef struct
{
    int8_t status;
    race_fota_stop_originator_enum originator;
    race_fota_stop_reason_enum reason;
}PACKED race_lpcomm_fota_stop_query_rsp_struct;

typedef race_lpcomm_rsp_template_struct race_lpcomm_fota_ping_rsp_struct;


typedef race_lpcomm_rsp_template_struct race_lpcomm_storage_dual_devices_erase_partition_rsp_struct;
typedef race_lpcomm_rsp_template_struct race_lpcomm_storage_lock_unlock_rsp_struct;

typedef struct
{
    int8_t status;
}PACKED race_lpcomm_bt_role_switch_rsp_struct;

typedef struct
{
    int8_t status;
    uint8_t storage_type;
    uint32_t partition_address;
    uint32_t partition_length;
}PACKED race_lpcomm_fota_query_partition_info_rsp_struct;

typedef struct
{
    int8_t status;
    uint8_t version_len;
    uint8_t version[FOTA_VERSION_MAX_SIZE];
}PACKED race_lpcomm_fota_get_version_rsp_struct;

typedef struct
{
    int8_t status;
    uint8_t sha256[RACE_STORAGE_SHA256_SIZE];
} PACKED race_lpcomm_storage_get_partition_sha256_rsp_struct;

typedef struct
{
    int8_t status;
    uint16_t erase_status_size;
    uint8_t erase_status[0];
} PACKED race_lpcomm_storage_get_4k_erased_status_rsp_struct;


typedef struct
{
    int8_t status;
    uint8_t battery_level;
}PACKED race_lpcomm_bt_get_battery_level_rsp_struct;


#ifdef RACE_FOTA_INTEGRITY_CHECK_ENHANCE_ENABLE
typedef struct
{
    int8_t status;
    uint8_t storage_type;
}PACKED race_lpcomm_fota_check_integrity_rsp_struct;
#endif


typedef race_lpcomm_rsp_template_struct race_lpcomm_fota_sp_ping_rsp_struct;


#ifdef RACE_FIND_ME_ENABLE
typedef struct
{
    int8_t status;
} PACKED race_lpcomm_find_me_rsp_struct;
#endif
/********************************RSP DEFINE END********************************/

#endif /* RACE_LPCOMM_ENABLE */
#endif /* __RACE_LPCOMM_MSG_STRUCT__ */

