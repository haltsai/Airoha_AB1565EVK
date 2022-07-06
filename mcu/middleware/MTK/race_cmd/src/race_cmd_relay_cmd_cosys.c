/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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
#include "race_cmd_relay_cmd_cosys.h"
#include "race_cmd_bluetooth.h"
#include "bt_connection_manager_internal.h"
#include "stdio.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_bt.h"
#include "race_timer.h"
#include "race_cmd.h"
#include "race_event.h"
#include "race_cmd_co_sys.h"

typedef struct {
    uint8_t channel_id;
    uint8_t relay_type;
    race_pkt_t cmd;
} race_ralay_cosys_packet_t;

bool race_relay_send_cosys(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id, uint8_t relay_type)
{
    race_ralay_cosys_packet_t *relay_data = NULL;
    uint32_t len;
    bool ret = false;

    RACE_LOG_MSGID_I("[relay_cmd] race_relay_send_cosys, race_id 0x%04X, length %d, channel %d, relay_type %d", 4,
            race_pkt->hdr.id, length, channel_id, relay_type);

    if (length != race_pkt->hdr.length + 4) {
        RACE_LOG_MSGID_E("[relay_cmd] race_relay_send_cosys, length(%d) != 4 + hdr.len(%d)", 2, length, race_pkt->hdr.length);
        return false;
    }

    len = length + 2;
    relay_data = (race_ralay_cosys_packet_t *)race_mem_alloc(len);
    if (relay_data == NULL) {
        RACE_LOG_MSGID_E("[relay_cmd] race_relay_send_cosys, alloc fail", 0);
        return false;
    }

    relay_data->channel_id = channel_id;
    relay_data->relay_type = relay_type;
    memcpy(&(relay_data->cmd), race_pkt, length);
    ret = race_cosys_send_data(RACE_COSYS_MODULE_ID_RELAY_CMD, false, (uint8_t *)relay_data, len);
    race_mem_free(relay_data);

    return ret;
}

bool race_relay_send_cosys_to_remote_internal(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id)
{
    return race_relay_send_cosys(race_pkt, length, channel_id, RACE_CMD_RELAY_TO_REMOTE_INTERNAL);
}

static void race_cmd_relay_req_hdlr(race_pkt_t *pMsg, uint8_t channel)
{
    uint8_t channel_id = RACE_CHANNEL_ID_SET_RELAY_CMD_FLAG(channel);
    race_send_pkt_t *pEvt = RACE_CmdHandler(pMsg, channel_id);

    if(pEvt) {
        race_relay_send_cosys(&pEvt->race_data, pEvt->length, channel, RACE_CMD_RSP_FROM_PARTNER);
        race_mem_free(pEvt);
    }
}

static void race_cmd_relay_req_internal_hdlr(race_pkt_t *pMsg, uint8_t channel)
{
    uint8_t channel_id = RACE_CHANNEL_ID_SET_RELAY_CMD_FLAG(channel);
    race_send_pkt_t *pEvt = RACE_CmdHandler(pMsg, channel_id);

    if(pEvt) {
        race_relay_send_cosys(&pEvt->race_data, pEvt->length, channel, RACE_CMD_RSP_FROM_REMOTE_INTERNAL);
        race_mem_free(pEvt);
    }
}

static void race_cmd_relay_rsp_hdlr(race_pkt_t *pMsg, uint8_t channel)
{
    race_status_t ret = RACE_STATUS_OK;
    typedef struct
    {
        uint8_t dst_type;
        uint8_t dst_id;
        race_pkt_t race_cmd_rsp;
    } PACKED RSP;

    RSP *rsp = RACE_ClaimPacketAppID(pMsg->hdr.pktId.field.app_id,
                     RACE_TYPE_NOTIFICATION,
                     RACE_CMDRELAY_PASS_TO_DST,
                     (sizeof(RSP) + pMsg->hdr.length - 2),
                     channel);
    if (rsp != NULL) {
        rsp->dst_type = RACE_RELAY_CHANNEL_TYPE_UART;
        rsp->dst_id = RACE_RELAY_CHANNEL_ID_UART;
        memcpy(&(rsp->race_cmd_rsp.hdr), &(pMsg->hdr), RACE_CMD_HDR_LEN);
        memcpy(rsp->race_cmd_rsp.payload, pMsg->payload, pMsg->hdr.length - 2);
        RACE_LOG_MSGID_I("[relay_cmd][relay_dbg] agent flush relay rsp id: 0x%04X, len:%d\n", 1, pMsg->hdr.id, pMsg->hdr.length);

        if (pMsg->hdr.length < 890) {
            ret = race_flush_packet((void *)rsp, channel);
            if (ret != RACE_STATUS_OK) {
                RACE_LOG_MSGID_E("[relay_cmd] agent flush relay rsp FAIL \n", 0);
            }
        } else {
            race_send_pkt_t* pSndPkt;
            uint32_t port_handle, ret_size, size;
            uint8_t *ptr;
            pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)rsp);
            port_handle = race_get_port_handle_by_channel_id(channel);
            ret_size = race_port_send_data(port_handle, (uint8_t*)&pSndPkt->race_data, pSndPkt->length);

            size = pSndPkt->length;
            ptr = (uint8_t*)&pSndPkt->race_data;
            size -= ret_size;
            ptr += ret_size;
            while(size > 0)
            {
                ret_size = race_port_send_data(port_handle, ptr, size);
                size -= ret_size;
                ptr += ret_size;
            }
            race_mem_free(pSndPkt);
        }
    } else {
        RACE_LOG_MSGID_E("[relay_cmd] race_cmd_relay_rsp_hdlr malloc FAIL \n", 0);
    }
}


static void race_cmd_relay_rsp_internal_hdlr(race_pkt_t *pMsg, uint8_t channel)
{
    /* Don't need send relay header to dest */
    race_status_t ret = RACE_STATUS_OK;
    void *rsp = RACE_ClaimPacketAppID(pMsg->hdr.pktId.field.app_id,
                     pMsg->hdr.type,
                     pMsg->hdr.id,
                     pMsg->hdr.length - 2,
                     channel);

    if (rsp == NULL) {
        RACE_LOG_MSGID_E("[relay_cmd] agent claim relay rsp(internal) FAIL", 0);
        return;
    }

    memcpy(rsp, pMsg->payload, pMsg->hdr.length - 2);

    ret = race_flush_packet(rsp, channel);
    RACE_LOG_MSGID_I("[relay_cmd] agent flush relay rsp internal ret %d", 1, ret);
}

void race_cmd_relay_aws_mce_msg_process(race_general_msg_t *msg)
{
    if (msg == NULL) {
        RACE_LOG_MSGID_E("[relay_cmd] race_relay_process, msg null", 0);
        return;
    }

    race_ralay_cosys_packet_t *packet = (race_ralay_cosys_packet_t *)msg->msg_data;

    if (packet == NULL) {
        RACE_LOG_MSGID_E("[relay_cmd] race_relay_process, data null", 0);
        return;
    }

    RACE_LOG_MSGID_I("[relay_cmd] race_relay_process, race_id 0x%04X, channel %d, type %d, race_len %d", 4,
            packet->cmd.hdr.id, packet->channel_id, packet->relay_type, packet->cmd.hdr.length);

    switch (packet->relay_type) {
        case RACE_CMD_RELAY_FROM_AGENT:
            race_cmd_relay_req_hdlr(&(packet->cmd), packet->channel_id);
            break;
        case RACE_CMD_RSP_FROM_PARTNER:
            race_cmd_relay_rsp_hdlr(&(packet->cmd), packet->channel_id);
            break;
        case RACE_CMD_RELAY_TO_REMOTE_INTERNAL:
            race_cmd_relay_req_internal_hdlr(&(packet->cmd), packet->channel_id);
            break;
        case RACE_CMD_RSP_FROM_REMOTE_INTERNAL:
            race_cmd_relay_rsp_internal_hdlr(&(packet->cmd), packet->channel_id);
            break;
        default:
            break;
    }

    race_mem_free(msg->msg_data);
}

static void* RACE_CmdHandler_relay_query_state(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t channel_type;
        uint8_t channel_id;
    }PACKED RSP;

    RSP *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_CMDRELAY_GET_AVA_DST,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);

    if (pEvt) {
        pEvt->channel_type = RACE_RELAY_CHANNEL_TYPE_UART;
        pEvt->channel_id = RACE_RELAY_CHANNEL_ID_UART;
    }
    return pEvt;
}

static void* RACE_CmdHandler_relay_pass_to_dst(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    uint16_t data_len;

    typedef struct
    {
        RACE_COMMON_HDR_STRU hdr;
        uint8_t dst_type;
        uint8_t dst_id;
        race_pkt_t sub_race_cmd;
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                 RACE_TYPE_RESPONSE,
                                 RACE_CMDRELAY_PASS_TO_DST,
                                 sizeof(RSP),
                                 channel_id);

    //race_recipient_type_enum recipient_type = race_recipient_type_convt(pCmd->recipient);

    if (pEvt != NULL)
    {
        bool ret = FALSE;

        data_len = pCmd->hdr.length - 4;/*race id length 2 bytes + dst_type 1byte + dst_id 1byte*/
        ret = race_relay_send_cosys(&(pCmd->sub_race_cmd), data_len, channel_id, RACE_CMD_RELAY_FROM_AGENT);

        pEvt->status = ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
    return pEvt;
}


void* RACE_CmdHandler_RELAY_RACE_CMD(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_CmdHandler_RELAY_RACE_CMD, type[0x%X], id[0x%X]",2, pCmdMsg->hdr.type, pCmdMsg->hdr.id);

    if (pCmdMsg->hdr.type == RACE_TYPE_COMMAND ||
        pCmdMsg->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP)
    {

        switch (pCmdMsg->hdr.id)
        {
            case RACE_CMDRELAY_GET_AVA_DST:
               return  RACE_CmdHandler_relay_query_state((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);
            case RACE_CMDRELAY_PASS_TO_DST:
                return RACE_CmdHandler_relay_pass_to_dst((PTR_RACE_COMMON_HDR_STRU)pCmdMsg , length, channel_id);
            default:
                break;
        }
    }
    return NULL;
}


static void race_relay_cosys_callback(bool is_critical, uint8_t *buff, uint32_t len)
{
    race_general_msg_t msg_queue_item;
    race_ralay_cosys_packet_t *packet = (race_ralay_cosys_packet_t *)buff;

    if (buff == NULL) {
        RACE_LOG_MSGID_E("race_relay_cosys_callback, buff null", 0);
        return;
    }

    if (len != packet->cmd.hdr.length + 6) {
        RACE_LOG_MSGID_E("race_relay_cosys_callback(), packet->cmd.hdr.length %d, len %d", 2, packet->cmd.hdr.length, len);
        return;
    }

    msg_queue_item.dev_t = SERIAL_PORT_DEV_UNDEFINED;
    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_RELAY_RACE_CMD;
    msg_queue_item.msg_data = race_mem_alloc(len);

    if (msg_queue_item.msg_data == NULL) {
        RACE_LOG_MSGID_E("race_relay_cosys_callback, data malloc fail", 0);
        return;
    }
    memcpy(msg_queue_item.msg_data, buff, len);
    race_send_msg(&msg_queue_item);

    return;
}

void race_relay_cmd_init(void)
{
    race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_RELAY_CMD, race_relay_cosys_callback);
}

