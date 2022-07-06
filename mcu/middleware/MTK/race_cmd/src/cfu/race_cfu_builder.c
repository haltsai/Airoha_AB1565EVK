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


#include "race_cmd_feature.h"
#ifdef RACE_CFU_BUILDER_ENABLE
#include "race_cmd.h"
#include "race_xport.h"
#include "race_cfu.h"
#include "race_cmd_cfu.h"
#include "race_cfu_internal.h"


/* Send the CFU packet from the local CFU device module to the headset/earbuds via race cmd. */
void race_cfu_builder_send_data_msg_processer(race_cfu_packet_struct *packet)
{
    U32 race_cmd_len = 0;
    race_cfu_relay_packet_command_struct *cmd = NULL;

    /* Do not process RSP when it is builder. */
    if (!packet ||
        RACE_CFU_PACKET_TYPE_CMD != packet->packet_type)
    {
        RACE_LOG_MSGID_W("[CFU][RACE] Drop packet. packet_type:%d", 1, packet ? packet->packet_type : RACE_CFU_PACKET_TYPE_MAX);
        return;
    }

    race_cmd_len = sizeof(race_cfu_relay_packet_command_struct) + packet->data_len;

    cmd = RACE_ClaimPacketAppID(0,
                                RACE_TYPE_COMMAND,
                                RACE_CFU_RELAY_PACKET,
                                race_cmd_len,
                                RACE_CFU_BUILDER_HANDLER_CHANNEL_ID);
    if (cmd)
    {
        memcpy(cmd, packet, race_cmd_len);
        /* cmd will be freed in race_flush_packet() */
        race_flush_packet((uint8_t *)cmd, RACE_CFU_BUILDER_HANDLER_CHANNEL_ID);
    }
    else
    {
        RACE_LOG_MSGID_W("[CFU][RACE] No enough memory for relay packet race cmd. len:%d", 1, race_cmd_len);
    }
}


void *race_cfu_builder_relay_packet_cmd_processer(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);
    UNUSED(channel_id);

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        //U8 status;
        race_cfu_packet_struct packet;
    } PACKED RSP;

    RSP *rsp = (RSP *)pCmdMsg;
    race_cfu_receive_callback recv_cb = NULL;

    if (!rsp ||
        RACE_CFU_PACKET_TYPE_RSP != rsp->packet.packet_type)
    {
        RACE_LOG_MSGID_W("[CFU][RACE] Invalid param. packet_type:%d", 1, rsp ? rsp->packet.packet_type : RACE_CFU_PACKET_TYPE_MAX);
        return NULL;
    }

    recv_cb = race_cfu_get_recv_cb();
    if (recv_cb)
    {
        RACE_LOG_MSGID_I("[CFU][RACE] update_mode=%x, packet_type=%x, packet_id=%x, payload_lengeh=%x",
                4, rsp->packet.update_mode, rsp->packet.packet_type,rsp->packet.packet_id,
                rsp->packet.data_len);
        recv_cb(&(rsp->packet));
    }
    else
    {
        RACE_LOG_MSGID_W("[CFU][RACE] fail to notify data to cfu device for recv_cb is null", 0);
    }

    return NULL;
}


void* race_cfu_builder_race_cmd_processer(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    if (pCmdMsg->hdr.type == RACE_TYPE_RESPONSE)
    {
        switch (pCmdMsg->hdr.id)
        {
            case RACE_CFU_RELAY_PACKET:
                return race_cfu_builder_relay_packet_cmd_processer((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            default:
                break;
        }
    }

    return NULL;
}
#endif /* RACE_CFU_BUILDER_ENABLE */

