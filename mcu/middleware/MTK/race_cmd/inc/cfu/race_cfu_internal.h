/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#ifndef _RACE_CFU_INTERNAL_H_
#define _RACE_CFU_INTERNAL_H_

#include "race_cmd_feature.h"
#ifdef RACE_CFU_ENABLE
#include "race.h"
#include "race_cfu.h"


#define RACE_CFU_RESPONSE_REPORT_SIZE (17)  /* report id + report data */
#define RACE_CFU_RESPONSE_REPORT_MAX_SIZE (RACE_CFU_RESPONSE_REPORT_SIZE)


#define RACE_CFU_RESULT_STATUS_LOCAL_DONE (0x01)
#define RACE_CFU_RESULT_STATUS_PEER_DONE (0x02)


#ifdef AIR_LE_AUDIO_ENABLE
#define RACE_CFU_BUILDER_HANDLER_CHANNEL_ID (race_get_port_type_by_channel_id(RACE_SERIAL_PORT_TYPE_BLE))
#else
#define RACE_CFU_BUILDER_HANDLER_CHANNEL_ID (race_get_port_type_by_channel_id(RACE_SERIAL_PORT_TYPE_SPP))
#endif


typedef enum
{
    RACE_CFU_MSG_ID_SEND_DATA,
} race_cfu_msg_id_enum;


typedef race_cfu_packet_struct race_cfu_relay_packet_command_struct;

typedef race_cfu_packet_struct race_cfu_relay_packet_response_struct;

typedef race_cfu_packet_struct race_cfu_relay_packet_lpcomm_req_struct;

typedef struct
{
    U8 status;  /* If it is not RACE_ERRCODE_SUCCESS, it means that it fails to get the rsp. */
    race_cfu_packet_struct packet;
}PACKED race_cfu_relay_packet_lpcomm_rsp_struct;


void race_cfu_msg_processer(race_general_msg_t *msg);

race_cfu_receive_callback race_cfu_get_recv_cb(void);


#endif /* RACE_CFU_ENABLE */
#endif /* _RACE_CFU_INTERNAL_H_ */

