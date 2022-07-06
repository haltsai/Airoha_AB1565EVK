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


#ifndef __RACE_LPCOMM_RECV_H__
#define __RACE_LPCOMM_RECV_H__


#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_cmd.h"
#include "race_xport.h"
#include "race_lpcomm_packet.h"


typedef RACE_ERRCODE (*race_lpcomm_req_hdl)(race_lpcomm_packet_struct *packet, uint8_t device_id);
/* BEWARE: FAKE RSP may be received the structure of which is race_lpcomm_rsp_template_struct with the status of RACE_ERRCODE_FAIL.
  * Therefore, copy the results into noti, only when the status is RACE_ERRCODE_SUCCESS. Otherwise, invalid results are used.
  */
typedef RACE_ERRCODE (*race_lpcomm_rsp_hdl)(race_lpcomm_packet_struct *packet, uint8_t device_id);
typedef RACE_ERRCODE (*race_lpcomm_noti_hdl)(race_lpcomm_packet_struct *packet, uint8_t device_id);

typedef enum
{
    RACE_LPCOMM_PACKET_CLASS_NONE,

    RACE_LPCOMM_PACKET_CLASS_RACE_CMD,
    RACE_LPCOMM_PACKET_CLASS_COMMON,

    RACE_LPCOMM_PACKET_CLASS_MAX
}race_lpcomm_packet_class_enum;


typedef struct
{
    race_lpcomm_packet_class_enum packet_class;
    uint16_t cmd_id;    /* identify each req/rsp/noti with the usage and the packet_type */
    race_lpcomm_req_hdl req_hdl;
    race_lpcomm_rsp_hdl rsp_hdl;
    race_lpcomm_noti_hdl noti_hdl;
}race_lpcomm_data_recv_hdl_struct;


RACE_ERRCODE race_lpcomm_data_recv_msg_process(race_general_msg_t *msg);

#endif /* RACE_LPCOMM_ENABLE */
#endif /* __RACE_LPCOMM_RECV_H__ */

