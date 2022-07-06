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

#include "race_cmd_feature.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_util.h"
#include "race_lpcomm_recv.h"
#include "race_lpcomm_agent.h"
#include "race_lpcomm_partner.h"


RACE_ERRCODE race_lpcomm_data_recv_msg_process(race_general_msg_t *msg)
{
    race_lpcomm_packet_struct *packet = NULL;
    race_lpcomm_role_enum role = RACE_LPCOMM_ROLE_NONE;

    if (!msg || !msg->msg_data)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    packet = (race_lpcomm_packet_struct *)msg->msg_data;

    RACE_LOG_MSGID_I("packet_type:%x, cmd_id:%x, channel_id:%d, process_id:%d, trans_method:%d, payload_len:%d, dev_id:%d",7,
               packet->packet_type,
               packet->cmd_id,
               packet->channel_id,
               packet->process_id,
               packet->trans_method,
               packet->payload_len,
               msg->dev_t);
    /* If REQ or RSP is received before RHO and processed after RHO, it will be dispatched to the wrong role handler. However, sender role can fix this.
     * Therefore, event if RACE_AWS_ENABLE is enabled, as long as RACE_LPCOMM_SENDER_ROLE_ENABLE is enabled, use the sender role to dispatch
     * the packets.
     */
#ifndef RACE_LPCOMM_SENDER_ROLE_ENABLE
    role = race_lpcomm_role_get(packet->trans_method);
#else
    role = RACE_LPCOMM_PACKET_GET_SENDER_ROLE(packet->packet_type);
    /* Convert the sender role to the receiver role. */
    if (RACE_LPCOMM_ROLE_AGENT == role)
    {
        role = RACE_LPCOMM_ROLE_PARTNER;
    }
    else if (RACE_LPCOMM_ROLE_PARTNER == role)
    {
        role = RACE_LPCOMM_ROLE_AGENT;
    }

    packet->packet_type = RACE_LPCOMM_PACKET_GET_PACKET_TYPE(packet->packet_type);
#endif

    if (RACE_LPCOMM_ROLE_AGENT == role)
    {
        race_lpcomm_agent_data_recv_hdl(packet, msg->dev_t);
    }
    else if (RACE_LPCOMM_ROLE_PARTNER == role)
    {
        race_lpcomm_partner_data_recv_hdl(packet, msg->dev_t);
    }
    else
    {
        RACE_LOG_MSGID_W("unknown role:%d", 1, role);
    }

    return RACE_ERRCODE_SUCCESS;
}

#endif /* RACE_LPCOMM_ENABLE */

