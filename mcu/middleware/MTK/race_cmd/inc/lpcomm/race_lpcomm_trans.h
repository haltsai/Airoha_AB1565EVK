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


#ifndef __RACE_LPCOMM_TRANS_H__
#define __RACE_LPCOMM_TRANS_H__

#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "bt_sink_srv.h"
#include "race_cmd.h"
#ifdef RACE_AWS_ENABLE
#include "bt_aws_mce_report.h"
#endif

#ifdef RACE_AWS_ENABLE
#define RACE_LPCOMM_AWS_HEADR_LEN (sizeof(bt_aws_mce_report_info_t))
#endif
#define RACE_LPCOMM_COSYS_HEADR_LEN (0)


typedef void (*race_lpcomm_header_init_func)(uint8_t *packet, uint16_t payload_len);
typedef RACE_ERRCODE (*race_lpcomm_packet_send_func)(uint8_t *packet, uint8_t device_id);


typedef enum
{
    RACE_LPCOMM_TRANS_METHOD_NONE,

    RACE_LPCOMM_TRANS_METHOD_AWS,
    RACE_LPCOMM_TRANS_METHOD_COSYS,

    RACE_LPCOMM_TRANS_METHOD_MAX,
}race_lpcomm_trans_method_enum;


typedef struct
{
    race_lpcomm_trans_method_enum method;
    uint8_t header_len;
    race_lpcomm_header_init_func header_init_func;
    race_lpcomm_packet_send_func send_func;
    bool retry_enable;
}race_lpcomm_trans_info_struct;


race_lpcomm_trans_info_struct *race_lpcomm_trans_find_info(race_lpcomm_trans_method_enum method);

RACE_ERRCODE race_lpcomm_packet_send(uint8_t *packet, race_lpcomm_trans_method_enum method, uint8_t device_id);

RACE_ERRCODE race_lpcomm_packet_send_to_peer(uint8_t *payload,
                                                         uint16_t payload_len,
                                                         uint8_t sender_role, /* race_lpcomm_role_enum */
                                                         uint8_t packet_type, /* race_lpcomm_packet_type_enum */
                                                         uint16_t cmd_id,
                                                         uint8_t app_id,
                                                         uint8_t channel_id,
                                                         uint16_t process_id,
                                                         uint8_t max_retry_time,
                                                         race_lpcomm_trans_method_enum method,
                                                         uint8_t device_id);

RACE_ERRCODE race_lpcomm_send_race_cmd_req_to_peer(uint8_t *req,
                                                     uint16_t req_len,
                                                     uint8_t sender_role, /* race_lpcomm_role_enum */
                                                     uint16_t cmd_id,
                                                     uint8_t app_id,
                                                     uint8_t channel_id,
                                                     uint16_t process_id,
                                                     race_lpcomm_trans_method_enum method,
                                                     uint8_t device_id);

RACE_ERRCODE race_lpcomm_send_race_cmd_rsp_to_peer(uint8_t *rsp,
                                              uint16_t rsp_len,
                                              uint8_t sender_role, /* race_lpcomm_role_enum */
                                              uint16_t cmd_id,
                                              uint8_t app_id,
                                              uint8_t channel_id,
                                              uint16_t process_id,
                                              race_lpcomm_trans_method_enum method,
                                              uint8_t device_id);

RACE_ERRCODE race_lpcomm_send_noti_to_peer(uint8_t *noti,
                                                     uint16_t noti_len,
                                                     uint8_t sender_role, /* race_lpcomm_role_enum */
                                                     uint16_t cmd_id,
                                                     uint8_t app_id,
                                                     uint8_t channel_id,
                                                     race_lpcomm_trans_method_enum method,
                                                     uint8_t device_id);

#endif /* RACE_LPCOMM_ENABLE */
#endif /* __RACE_LPCOMM_TRANS_H__ */

