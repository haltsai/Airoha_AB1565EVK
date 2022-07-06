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

#ifndef _RACE_CFU_HANDLER_H_
#define _RACE_CFU_HANDLER_H_

#include "race_cmd_feature.h"
#ifdef RACE_CFU_HANDLER_ENABLE
#include "types.h"
#include "race_cmd.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_packet.h"
#endif
#include "race_event.h"
#include "race_cfu.h"


#ifdef RACE_LPCOMM_ENABLE
typedef struct
{
    race_lpcomm_packet_struct packet;
    U8 device_id;
    U16 packet_id;
} race_cfu_handler_relay_packet_rsp_info_struct;
#endif


void* race_cfu_handler_race_cmd_processer(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id);

#ifdef RACE_LPCOMM_ENABLE
race_cfu_handler_relay_packet_rsp_info_struct *race_cfu_handler_get_relay_packet_rsp_info(void);

void race_cfu_handler_reset_relay_packet_rsp_info(void);
#endif

#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
void race_cfu_handler_stop_clear_cached_info_timer(void);

U8 race_cfu_handler_get_clear_cached_info_timer_id(void);

RACE_ERRCODE race_cfu_handler_set_clear_cached_info_timer_id(U8 timer_id);

void race_cfu_handler_clear_cached_info_timer_hdl(uint8_t id, void *user_data);
#endif

bool race_cfu_handler_wait_rsp(void);

RACE_ERRCODE race_cfu_handler_race_event_processer(int32_t register_id, race_event_type_enum event_type, void *param, void *user_data);

void race_cfu_handler_send_data_msg_processer(race_cfu_packet_struct *packet);

void race_cfu_handler_init(void);

void race_cfu_handler_process_relay_packet_response(race_cfu_packet_struct *packet, bool is_local_rsp);

#endif /* RACE_CFU_HANDLER_ENABLE */
#endif /* _RACE_CFU_HANDLER_H_ */

