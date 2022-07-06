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

#ifndef __BT_RACE_H__
#define __BT_RACE_H__


#include "race_cmd_feature.h"
#ifdef RACE_BT_CMD_ENABLE
#include "bt_type.h"
#include "race_cmd.h"
#include "race_cmd_relay_cmd.h"
#include "race_noti.h"
#include "race_xport.h"
#include "race_cmd_fcd_cmd.h"

#ifdef RACE_BT_EVENT_MSG_HDL
typedef struct
{
    bt_msg_type_t msg;
    bt_status_t status;
    uint8_t buff_len;
    uint8_t buff[0];
}race_bt_event_msg_info_struct;
#endif


#ifdef RACE_BT_EVENT_REGISTER_ENABLE
typedef enum
{
    RACE_BT_EVENT_TYPE_HFP_DISC = 0x01,
    RACE_BT_EVENT_TYPE_A2DP_DISC = 0x02
}race_bt_event_type_enum;


typedef enum
{
    RACE_BT_EVENT_REASON_EVENT_OCCUR = 0x01,
    RACE_BT_EVENT_REASON_DEREGISTER = 0x02
}race_bt_event_reason_enum;


typedef void (*race_bt_event_hdl)(uint8_t event,  uint8_t reason, void *user_data);

bool race_bt_event_is_valid_register_id(uint32_t register_id);

uint32_t race_bt_event_register(uint8_t event, race_bt_event_hdl hdl, void *user_data);

void race_bt_event_deregister(uint32_t register_id);
#endif /* RACE_BT_EVENT_REGISTER_ENABLE */

#ifdef RACE_BT_EVENT_MSG_HDL
void race_bt_event_ind_msg_process(race_bt_event_msg_info_struct *msg_info);
#endif

void race_bt_init(void);

void race_bt_deinit(void);

bt_bd_addr_t* race_bt_get_sp_bd_addr(void);

void race_bt_set_ble_conn_hdl(bt_handle_t conn_hdl);

bt_handle_t race_bt_get_ble_conn_hdl(void);

void race_bt_set_a2dp_conn_hdl(bt_handle_t conn_hdl);

bt_handle_t race_bt_get_a2dp_conn_hdl(void);

void race_bt_set_hfp_conn_hdl(bt_handle_t conn_hdl);

bt_handle_t race_bt_get_hfp_conn_hdl(void);

bt_status_t race_bt_event_process(bt_msg_type_t msg_type,
                                         bt_status_t status,
                                         void *buff);
extern bt_handle_t bt_gap_le_srv_get_handle_by_old_handle(bt_handle_t old_handle);

#ifdef MTK_AWS_MCE_ENABLE
void race_bt_notify_aws_state(bool aws_state);
#endif
#endif /* RACE_BT_CMD_ENABLE */
#endif /* __BT_RACE_H__ */

