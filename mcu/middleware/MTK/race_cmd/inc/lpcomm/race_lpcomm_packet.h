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


#ifndef __RACE_LPCOMM_PACKET_H__
#define __RACE_LPCOMM_PACKET_H__

#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_trans.h"
#include "race_lpcomm_util.h"


#define RACE_LPCOMM_PACKET_HEADER_SIZE  (sizeof(race_lpcomm_packet_struct))

/* Reuse the high 4 bits of packet_type for sender role. */
#define RACE_LPCOMM_PACKET_GET_SENDER_ROLE(packet_type)  (((packet_type) & 0xF0) >> 4)
#define RACE_LPCOMM_PACKET_SET_SENDER_ROLE(packet_type, role)  ((packet_type) = ((packet_type) & 0x0F) | ((role) << 4))

#define RACE_LPCOMM_PACKET_GET_PACKET_TYPE(packet_type)  ((packet_type) & 0x0F)
#define RACE_LPCOMM_PACKET_SET_PACKET_TYPE(packet_type, type)  ((packet_type) = ((packet_type) & 0xF0) | ((type) & 0x0F))


typedef enum
{
    RACE_LPCOMM_PACKET_TYPE_NONE,

    RACE_LPCOMM_PACKET_TYPE_RACE_CMD_REQ,
    RACE_LPCOMM_PACKET_TYPE_RACE_CMD_RSP,

    RACE_LPCOMM_PACKET_TYPE_COMMON_REQ,
    RACE_LPCOMM_PACKET_TYPE_COMMON_RSP,

    RACE_LPCOMM_PACKET_TYPE_COMMON_NOTI,

    RACE_LPCOMM_PACKET_TYPE_MAX = 0x0F /* Must not exceed 0x0F */
}race_lpcomm_packet_type_enum;


/* Use common cmd_id when the packet_type is RACE_LPCOMM_PACKET_TYPE_COMMON_X */
typedef enum
{
    RACE_LPCOMM_COMMON_CMD_ID_NONE,

    RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_QUERY,
    RACE_LPCOMM_COMMON_CMD_ID_FOTA_STOP_RESULT,
    RACE_LPCOMM_COMMON_CMD_ID_FOTA_PING,

    RACE_LPCOMM_COMMON_CMD_ID_GET_BATTERY,

    RACE_LPCOMM_COMMON_CMD_ID_MAX = 0xFFFF
}race_lpcomm_common_cmd_id_enum;


typedef struct
{
    uint8_t packet_type; /**< high 4 bits: sender role, race_lpcomm_role_enum; low 4 bits: packet type, race_lpcomm_packet_type_enum. */
    uint16_t cmd_id;
    uint8_t app_id;
    uint8_t channel_id;
    uint16_t process_id;
    uint8_t trans_method;
    uint16_t payload_len;
    uint8_t payload[0];
}PACKED race_lpcomm_packet_struct;


void race_lpcomm_packet_free(uint8_t *packet);

race_lpcomm_packet_struct *race_lpcomm_packet_header_get(uint8_t *packet, race_lpcomm_trans_method_enum method);

uint8_t *race_lpcomm_packet_create(uint8_t *payload,
                                   uint16_t payload_len,
                                   race_lpcomm_role_enum sender_role,
                                   race_lpcomm_packet_type_enum packet_type,
                                   uint16_t cmd_id,
                                   uint8_t app_id,
                                   uint8_t channel_id,
                                   uint16_t process_id,
                                   race_lpcomm_trans_method_enum trans_method);
#endif /* RACE_LPCOMM_ENABLE */
#endif /* __RACE_LPCOMM_PACKET_H__ */

