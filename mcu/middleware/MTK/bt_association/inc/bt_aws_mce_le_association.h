/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 



#ifndef __BT_AWS_MCE_LE_ASSOCIATION__
#define __BT_AWS_MCE_LE_ASSOCIATION__
#include "stdint.h"
#include "bt_type.h"
#include "bt_gatts.h"
#include "bt_uuid.h"
#include "bt_aws_mce_srv.h"


#define ASSOCIATION_SERVICE_UUID                   \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x01, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define ASSOCIATION_AGENT_ADDR_CHAR_UUID           \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x02, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define ASSOCIATION_KEY_CHAR_UUID           \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x03, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define ASSOCIATION_CLIENT_ADDR_CHAR_UUID           \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x04, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define ASSOCIATION_AUDIO_LAT_CHAR_UUID           \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x05, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define ASSOCIATION_VOICE_LAT_CHAR_UUID           \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x06, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define ASSOCIATION_NUMBER_CHAR_UUID           \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x07, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define ASSOCIATION_CUSTOM_READ_CHAR_UUID           \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x08, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define ASSOCIATION_CUSTOM_WRITE_CHAR_UUID           \
    {0x61, 0x68, 0x6F, 0x72, 0x69, 0x61, 0x11, 0x8F,               \
     0x58, 0x4C, 0x09, 0x10, 0x75, 0x4E, 0x28, 0xA2}

#define BT_MODULE_CUSTOM_LE_ASSOCIATION (BT_MODULE_CUSTOM | 0x100)
#define BT_STATUS_LE_ASSOCIATION_PARAMETER_ERR (BT_MODULE_CUSTOM_LE_ASSOCIATION | 0x0)

#define ASSOCIATION_AGENT_ADDR_CHAR_VALUE_HANDLE 0x0122
#define ASSOCIATION_KEY_CHAR_VALUE_HANDLE 0x0124
#define ASSOCIATION_CLIENT_ADDR_CHAR_VALUE_HANDLE 0x0126
#define ASSOCIATION_AUDIO_LAT_CHAR_VALUE_HANDLE 0x0128
#define ASSOCIATION_VOICE_LAT_CHAR_VALUE_HANDLE 0x012A
#define ASSOCIATION_NUMBER_CHAR_VALUE_HANDLE 0x012C
#define ASSOCIATION_CUSTOM_READ_CHAR_VALUE_HANDLE 0x012E
#define ASSOCIATION_CUSTOM_WRITE_CHAR_VALUE_HANDLE 0x0130



#define BT_AWS_MCE_LE_ASSOCIATION_MAX_NUMBER 8
#define BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN 4

#define BT_AWS_MCE_LE_ASSOCIATION_EVENT_CLIENT_PAIR_IND 0x00
#define BT_AWS_MCE_LE_ASSOCIATION_EVENT_AGENT_PAIR_CNF 0x01
#define BT_AWS_MCE_LE_ASSOCIATION_EVENT_READ_CUSTOM_DATA_CNF 0x02
#define BT_AWS_MCE_LE_ASSOCIATION_EVENT_WRITE_CUSTOM_DATA_CNF 0x03
#define BT_AWS_MCE_LE_ASSOCIATION_EVENT_WRITE_CUSTOM_DATA_IND 0x04

typedef uint8_t bt_aws_mce_le_association_event_t;

#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_AGENT_ADDR 0x00
#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_SECRET_KEY 0x01
#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_ADDR 0x02
#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_AUDIO_LATENCY 0x03
#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_VOICE_LATENCY 0x04
#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_NO 0x05
#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_CUSTOM_READ_DATA 0x06
#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_CUSTOM_WRITE_DATA 0x07
#define BT_AWS_MCE_LE_ASSOCIATION_CHAR_INVALID 0xFF
typedef uint8_t bt_aws_mce_le_association_char_type_t;

#define BT_AWS_MCE_LE_ASSOCIATION_PAIR_MODE_NORMAL 0x0
#define BT_AWS_MCE_LE_ASSOCIATION_PAIR_MODE_NUMBERED 0x1
typedef uint8_t bt_aws_mce_le_association_pair_mode_t;

typedef struct {
    bt_bd_addr_t address;
    uint8_t secret_key[16];
} bt_aws_mce_le_association_agent_info_t;

typedef struct {
    bt_aws_mce_le_association_char_type_t type;
    uint16_t value_handle;
} bt_aws_mce_le_association_char_t;

typedef struct {
    uint8_t char_count;
    bt_aws_mce_le_association_char_t chara[BT_AWS_MCE_LE_ASSOCIATION_MAX_NUMBER];
} bt_aws_mce_le_association_service_t;

typedef struct {
    bt_bd_addr_t address;
    uint16_t audio_latency;
    uint16_t voice_latency;
} bt_aws_mce_le_association_client_info_t;

typedef struct {
    bt_handle_t handle;
    bt_aws_mce_le_association_pair_mode_t mode;
    bt_aws_mce_le_association_service_t *service;
    bt_aws_mce_le_association_client_info_t *client;
} bt_aws_mce_le_association_pair_agent_req_t;


typedef struct {
    bt_handle_t handle;
    uint16_t number;
    bt_aws_mce_le_association_client_info_t info;
} bt_aws_mce_le_association_client_pair_ind_t;

typedef struct {
    bt_handle_t handle;
    uint16_t number;
    bt_aws_mce_le_association_agent_info_t info;
} bt_aws_mce_le_association_agent_pair_cnf_t;

typedef struct {
    uint8_t *data;
    uint8_t len; /*len <= BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN*/
} bt_aws_mce_le_association_custom_data_t;

typedef struct {
    bt_handle_t handle;
    bt_bd_addr_t address;
    bt_aws_mce_le_association_custom_data_t info;
}bt_aws_mce_le_association_read_custom_data_cnf_t;

typedef struct {
    bt_handle_t handle;
}bt_aws_mce_le_association_write_custom_data_cnf_t;

typedef struct {
    bt_handle_t handle;
    bt_bd_addr_t address;
    bt_aws_mce_le_association_custom_data_t info;
}bt_aws_mce_le_association_write_custom_data_ind_t;


extern const bt_gatts_service_t bt_aws_mce_le_association_service;

void bt_aws_mce_le_association_service_info_set(bt_aws_mce_le_association_agent_info_t *agent);

uint16_t bt_aws_mce_le_association_service_get_assign_number(void);

bt_status_t bt_aws_mce_le_association_service_build_advertising_data(void *buffer, uint16_t *buffer_length, bt_firmware_type_t mode);

bt_status_t bt_aws_mce_le_association_client_start_pairing(bt_aws_mce_le_association_pair_agent_req_t *req);

bt_status_t bt_aws_mce_le_association_client_check_adv_data(void *buffer, uint16_t buffer_len, bt_firmware_type_t mode);

bt_status_t bt_aws_mce_le_association_event_handler(bt_msg_type_t msg, bt_status_t status, void *buffer);

void bt_aws_mce_le_association_event_callback(bt_aws_mce_le_association_event_t event, bt_status_t status, void *buffer, uint16_t length);

bt_status_t bt_aws_mce_le_association_read_custom_data(bt_handle_t handle);

bt_status_t bt_aws_mce_le_association_write_custom_data(bt_handle_t handle,  uint8_t *data, uint8_t len);

bool bt_aws_mce_le_association_service_get_custom_data(bt_aws_mce_le_association_custom_data_t* data);



#endif /*  __BT_AWS_MCE_LE_ASSOCIATION__ */

