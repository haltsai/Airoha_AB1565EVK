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
#include "race_cmd_relay_cmd.h"
#include "race_cmd_bluetooth.h"
#include "bt_connection_manager_internal.h"
#include "stdio.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_bt.h"
#include "race_timer.h"
#include "race_cmd.h"
#include "race_event.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_report_internal.h"
#endif
#include "race_cmd_dsprealtime.h"


#ifdef MTK_MUX_AWS_MCE_ENABLE
#include "mux.h"
#include "mux_aws_mce.h"
#endif

#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
#include "user_trigger_adaptive_ff.h"
#endif
#ifdef RACE_RELAY_CMD_ENABLE

#define MIN(X, Y) (((X)<(Y))?(X):(Y))

static uint8_t relay_init = 0;

#ifdef MTK_MUX_AWS_MCE_ENABLE
static mux_handle_t g_race_relay_mux_aws_if_handle;
#endif

static race_cmd_ctrl_t peq_relay_cmd_ctrl =
{
    .buffer = NULL,
    .offset = 0,
    .total_pkt = 0,
    .pre_pkt = 0,
};

static race_cmd_dbg_t peq_relay_dbg =
{
    .send_idx = 0,
    .start_chk = 0,
    .recv_idx_cur = 0,
    .recv_idx = {0},
};

static race_cmd_ctrl_t anc_adaptive_check_relay_cmd_ctrl =
{
    .buffer = NULL,
    .offset = 0,
    .total_pkt = 0,
    .pre_pkt = 0,
};

static race_cmd_ctrl_t rssi_relay_cmd_ctrl =
{
    .buffer = NULL,
    .offset = 0,
    .total_pkt = 0,
    .pre_pkt = 0,
};

static void race_cmd_ctrl_deinit(race_cmd_ctrl_t *relay_cmd_ctrl)
{
    relay_cmd_ctrl->buffer = NULL;
    relay_cmd_ctrl->offset = 0;
    relay_cmd_ctrl->total_pkt = 0;
    relay_cmd_ctrl->pre_pkt = 0;
}

#ifdef MTK_MUX_AWS_MCE_ENABLE
typedef struct {
    bt_aws_mce_report_packet_header_t header; /**<  The header of the AWS MCE packet. */
    bt_aws_mce_report_payload_header_t header_payload;/**< The header of no-sync payload packet. */
    race_cmd_aws_mce_packet_hdr_t header_race_cmd;
    uint8_t payload[1];                       /**<  The payload  of the AWS MCE packet. */
} race_aws_mce_packet_t;

bt_status_t bt_send_aws_mce_race_cmd_data(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id, uint8_t type, uint8_t send_idx)
{
    bt_status_t aws_ret = BT_SINK_SRV_STATUS_FAIL;
    uint32_t cur_pkt = 0;
    uint32_t size;
    uint8_t *buffer = (uint8_t *)race_pkt;
    uint32_t num_pkt = (length / (BT_AWS_MCE_MAX_DATA_LENGTH - RACE_AWS_RELAY_CMD_HEADER_LEN)) + 1;
    race_aws_mce_packet_t *aws_mce_pkt=NULL;
    uint32_t data_len;
    mux_status_t mux_status;
    mux_buffer_t mux_buffer;
    uint32_t header_size = sizeof(bt_aws_mce_report_packet_header_t) + sizeof(bt_aws_mce_report_payload_header_t) + sizeof(race_cmd_aws_mce_packet_hdr_t);
    uint32_t tx_avail_len_before, tx_avail_len_after;

    mux_control(MUX_AWS_MCE,MUX_CMD_GET_VIRTUAL_TX_AVAIL_LEN,(mux_ctrl_para_t*)&tx_avail_len_before);

    if ((buffer == NULL) || (length == 0) || (bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_NONE))
    {
        return BT_STATUS_FAIL;
    }

    for(cur_pkt = 0; cur_pkt < num_pkt; cur_pkt++)
    {
        size = MIN(length, BT_AWS_MCE_MAX_DATA_LENGTH - header_size);

        if(!aws_mce_pkt){
            aws_mce_pkt = (race_aws_mce_packet_t*)race_mem_alloc(header_size + size);
            RACE_LOG_MSGID_I("[relay_cmd] size=%d header_size=%d\r\n", 2, size, header_size);
            if(!aws_mce_pkt){
                RACE_LOG_MSGID_I("[relay_cmd] bt_send_aws_mce_race_cmd_data() malloc %d bytes fail\r\n", 1, header_size + size);
                break;
            }
            else{
                RACE_LOG_MSGID_I("[relay_cmd] bt_send_aws_mce_race_cmd_data() malloc %d bytes ok\r\n", 1, header_size + size);
            }
            aws_mce_pkt->header.module_id = BT_AWS_MCE_REPORT_MODULE_RELAY_CMD;
            aws_mce_pkt->header_payload.is_sync = false;
            aws_mce_pkt->header_race_cmd.channel_id = channel_id;
            aws_mce_pkt->header_race_cmd.type = type;
            aws_mce_pkt->header_race_cmd.numSubPkt = num_pkt;
            aws_mce_pkt->header_race_cmd.idx = send_idx;
        }

        aws_mce_pkt->header.payload_length = sizeof(bt_aws_mce_report_payload_header_t) + sizeof(race_cmd_aws_mce_packet_hdr_t) + size;
        aws_mce_pkt->header_payload.len = sizeof(race_cmd_aws_mce_packet_hdr_t) + size;
        aws_mce_pkt->header_race_cmd.SubPktId = cur_pkt;
        memcpy(aws_mce_pkt->payload, buffer, size);// real sub race cmd
        buffer += size;
        length -= size;

        mux_buffer.p_buf = (uint8_t*)aws_mce_pkt;
        mux_buffer.buf_size = header_size + size;
        mux_status = mux_tx(g_race_relay_mux_aws_if_handle, &mux_buffer, 1, &data_len);
        RACE_LOG_MSGID_I("[relay_cmd] send aws mce data: cur_pkt=%d, num=%d, remain_size=%d, send_size=%d, data_len=%d, status=%d\r\n", 6, cur_pkt, num_pkt, length, size, data_len, mux_status);
        if(mux_status == MUX_STATUS_OK){
            aws_ret = BT_STATUS_SUCCESS;
        }
        else{
            aws_ret = BT_STATUS_FAIL;
            break;
        }
    }
    if(aws_mce_pkt){
        race_mem_free(aws_mce_pkt);
    }

    mux_control(MUX_AWS_MCE,MUX_CMD_GET_VIRTUAL_TX_AVAIL_LEN,(mux_ctrl_para_t*)&tx_avail_len_after);
    if(tx_avail_len_before != tx_avail_len_after){
        // Not all transfered, we should clean pending sub-pkts in the tx ring buffer.
        RACE_LOG_MSGID_I("[relay_cmd] bt_send_aws_mce_race_cmd_data() tx_avail_len_before=%d tx_avail_len_after=%d\r\n", 2, tx_avail_len_before, tx_avail_len_after);
        aws_ret = BT_STATUS_FAIL;
        mux_control(MUX_AWS_MCE,MUX_CMD_CLEAN_TX_VIRUTUAL,NULL);
    }

    return aws_ret;
}
#else
bt_status_t bt_send_aws_mce_race_cmd_data(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id, uint8_t type, uint8_t send_idx)
{
    bt_status_t aws_ret = BT_SINK_SRV_STATUS_FAIL;
    race_cmd_aws_mce_packet_hdr_t *header;
    bt_aws_mce_report_info_t info;

    uint32_t header_size = sizeof(bt_aws_mce_report_payload_header_t) + RACE_AWS_RELAY_CMD_HEADER_LEN;
    uint32_t num_pkt = (length / (BT_AWS_MCE_MAX_DATA_LENGTH - header_size)) + 1;
    uint32_t cur_pkt = 0;

    uint32_t size;
    uint8_t *buffer = (uint8_t *)race_pkt;

    if ((buffer == NULL) || (length == 0) || (bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_NONE))
    {
        return BT_STATUS_FAIL;
    }

    for(cur_pkt = 0; cur_pkt < num_pkt; cur_pkt++)
    {
        size = MIN(length, BT_AWS_MCE_MAX_DATA_LENGTH - header_size);
        header = (race_cmd_aws_mce_packet_hdr_t *)race_mem_alloc((size + RACE_AWS_RELAY_CMD_HEADER_LEN + 3) & (~0x3));
        if (header != NULL) {
            header->race_cmd_id = race_pkt->hdr.id;
            header->channel_id = channel_id;
            header->type = type;
            header->numSubPkt = num_pkt;
            header->SubPktId = cur_pkt;
            header->idx = send_idx;

            memcpy((uint8_t *)header + RACE_AWS_RELAY_CMD_HEADER_LEN, buffer, size);// real sub race cmd
            buffer += size;
            length -= size;

            info.module_id = BT_AWS_MCE_REPORT_MODULE_RELAY_CMD;
            info.is_sync = false;
            info.sync_time = 0;
            info.param = (void *)header;
            info.param_len = size + RACE_AWS_RELAY_CMD_HEADER_LEN;

            aws_ret = bt_aws_mce_report_send_event(&info);

            RACE_LOG_MSGID_I("[relay_cmd] send aws mce data: race_cmd_id=0x%x, cur_pkt=%d, num=%d, remain_size=%d, send_size=%d\r\n", 5, header->race_cmd_id, cur_pkt, num_pkt, length, size);
            race_mem_free(header);
        }
    }
    return aws_ret;
}
#endif

static void race_cmd_relay_aws_mce_notify(uint8_t *packet, uint16_t len, uint8_t channel)
{
    race_general_msg_t msg_queue_item;
    msg_queue_item.msg_data = race_mem_alloc(len + 1);
    msg_queue_item.dev_t = SERIAL_PORT_DEV_UNDEFINED;
    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_RELAY_RACE_CMD;
    msg_queue_item.msg_data[0] = (uint8_t)channel;
    memcpy(msg_queue_item.msg_data + 1, packet, len);
    RACE_LOG_MSGID_I("[relay_cmd][relay_dbg] notify 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X 0x%02X\n",6
        ,*(msg_queue_item.msg_data + 1)
        ,*(msg_queue_item.msg_data + 2)
        ,*(msg_queue_item.msg_data + 3)
        ,*(msg_queue_item.msg_data + 4)
        ,*(msg_queue_item.msg_data + 5)
        ,*(msg_queue_item.msg_data + 6));
    race_send_msg(&msg_queue_item);
}

static uint32_t race_cmd_relay_aws_mce_check(race_cmd_ctrl_t *relay_ctrl, race_cmd_aws_mce_packet_t *pkt)
{
    uint32_t b_allow = 1;
    if (pkt->hdr.idx == 0) { //When another device power reset, re-start checking mechanism.
        peq_relay_dbg.start_chk = 0;
        peq_relay_dbg.recv_idx_cur= 0;
    }
    if (peq_relay_dbg.start_chk) {
        uint32_t i;
        for(i = 0; i < RACE_RELAY_PKT_DBG_LEN; i++)
        {
            if(pkt->hdr.idx == peq_relay_dbg.recv_idx[i]) {
                b_allow = 0;
                RACE_LOG_MSGID_E("[relay_cmd][relay_dbg] Repeat IF packet idx: %d\n",1, pkt->hdr.idx);
#if (RACE_DEBUG_PRINT_ENABLE)
                race_dump(relay_ctrl->buffer, RACE_DBG_IF_RELAY);
#endif
                break;
            }
        }
    }
    RACE_LOG_MSGID_I("[relay_cmd][relay_dbg] %d - th relay IF packet \n",1, pkt->hdr.idx);
    peq_relay_dbg.recv_idx[peq_relay_dbg.recv_idx_cur] = pkt->hdr.idx;
    peq_relay_dbg.recv_idx_cur = (peq_relay_dbg.recv_idx_cur + 1) & (RACE_RELAY_PKT_DBG_LEN-1);
    if ((!peq_relay_dbg.start_chk) && (peq_relay_dbg.recv_idx_cur == 0)) {
        peq_relay_dbg.start_chk = 1;
        RACE_LOG_MSGID_I("[relay_cmd][relay_dbg] Start check idx\n",0);
    }
    return b_allow;
}

static void bt_aws_mce_report_relay_cmd_callback(bt_aws_mce_report_info_t *info)
{
    uint8_t num_pkt;
    uint8_t index_pkt;
    uint8_t channel_id;
    race_cmd_ctrl_t *relay_ctrl = NULL;
    race_cmd_aws_mce_packet_t *pkt = (race_cmd_aws_mce_packet_t*)(info->param);
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    if (pkt == NULL) {
       RACE_LOG_MSGID_E("[relay_cmd]data is null", 0);
       return;
    }
    num_pkt = pkt->hdr.numSubPkt;
    index_pkt = pkt->hdr.SubPktId;
    channel_id = pkt->hdr.channel_id;

    if((num_pkt * BT_AWS_MCE_MAX_DATA_LENGTH) > 1024) {
        RACE_LOG_MSGID_E("[relay_cmd] Request size is over 1024 bytes, %d\n",1, num_pkt*BT_AWS_MCE_MAX_DATA_LENGTH);
    }

    if((pkt->hdr.type == RACE_CMD_RELAY_FROM_AGENT) && (role != BT_AWS_MCE_ROLE_PARTNER)) {
        RACE_LOG_MSGID_E("[relay_cmd] receive relay packet from agent, but role:%x\n", 1, role);
    } else if((pkt->hdr.type == RACE_CMD_RSP_FROM_PARTNER) && (role != BT_AWS_MCE_ROLE_AGENT)) {
        RACE_LOG_MSGID_E("[relay_cmd] receive relay packet from partner, but role:%x\n", 1, role);
    }

    if((pkt->hdr.race_cmd_id == RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK) || (pkt->hdr.race_cmd_id == RACE_DSPREALTIME_ANC_ADAPTIVE_RESULT)) {
        relay_ctrl = &anc_adaptive_check_relay_cmd_ctrl;

    } else if(pkt->hdr.race_cmd_id != RACE_BLUETOOTH_GET_RSSI) {
        relay_ctrl = &peq_relay_cmd_ctrl;
    } else {
        relay_ctrl = &rssi_relay_cmd_ctrl;
    }

    switch(pkt->hdr.type)
    {
        case RACE_CMD_RELAY_FROM_AGENT:
        case RACE_CMD_RSP_FROM_PARTNER:
        {
            if(index_pkt == 0)
            {
                relay_ctrl->total_pkt = (uint32_t)num_pkt;
                relay_ctrl->pre_pkt = -1;
                if(relay_ctrl->buffer) {
                    race_mem_free(relay_ctrl->buffer);
                    RACE_LOG_MSGID_W("[relay_cmd] buffer didn't free yet\n", 0);
                }
                relay_ctrl->buffer = (uint8_t*)race_mem_alloc(num_pkt * BT_AWS_MCE_MAX_DATA_LENGTH);
                if(relay_ctrl->buffer == NULL) {
                    RACE_LOG_MSGID_E("[relay_cmd] race_mem_alloc buffer NULL\n", 0);
                }
                relay_ctrl->offset = 0;

            }

            if((index_pkt == (relay_ctrl->pre_pkt + 1)) && (num_pkt == relay_ctrl->total_pkt) && (relay_ctrl->buffer != NULL))
            {
                uint32_t temp_size = (uint32_t)info->param_len - RACE_AWS_RELAY_CMD_HEADER_LEN;
                if ((relay_ctrl->offset + temp_size) <= num_pkt * BT_AWS_MCE_MAX_DATA_LENGTH) {
                memcpy(relay_ctrl->buffer + relay_ctrl->offset, (uint8_t *)&pkt->race_cmd, temp_size);
                }
                relay_ctrl->offset += temp_size;
                relay_ctrl->pre_pkt = index_pkt;
            }
            else
            {
                RACE_LOG_MSGID_E("[relay_cmd] packet header is wrong, (%d/%d), (%d/%d), buffer:0x%08x offset:%d\n",6, index_pkt, num_pkt, relay_ctrl->pre_pkt, relay_ctrl->total_pkt, relay_ctrl->buffer, relay_ctrl->offset);
                //race_mem_free(relay_ctrl->buffer);
                //race_cmd_ctrl_deinit(relay_ctrl);
                break;
            }

            if(((relay_ctrl->pre_pkt + 1) == relay_ctrl->total_pkt) && (relay_ctrl->buffer != NULL))/*means last pkt*/
            {
                if (relay_ctrl != &peq_relay_cmd_ctrl) {
                    race_cmd_relay_aws_mce_notify(relay_ctrl->buffer, relay_ctrl->offset, channel_id);
                } else if (race_cmd_relay_aws_mce_check(relay_ctrl, pkt) == 1) {
                    race_cmd_relay_aws_mce_notify(relay_ctrl->buffer, relay_ctrl->offset, channel_id);
                }
                race_mem_free(relay_ctrl->buffer);
                race_cmd_ctrl_deinit(relay_ctrl);
            }
            break;
        }
    }
}

static void race_cmd_relay_req_hdlr(race_pkt_t *pMsg, uint8_t channel)
{
    bt_status_t ret = BT_SINK_SRV_STATUS_FAIL;
    uint8_t channel_id = RACE_CHANNEL_ID_SET_RELAY_CMD_FLAG(channel);
    race_send_pkt_t *pEvt = RACE_CmdHandler(pMsg, channel_id);
    if(pEvt) {
        ret = bt_send_aws_mce_race_cmd_data(&pEvt->race_data, pEvt->length, channel, RACE_CMD_RSP_FROM_PARTNER,peq_relay_dbg.send_idx);
        if (ret != BT_STATUS_SUCCESS) {
            RACE_LOG_MSGID_I("[relay_cmd] partner send relay req FAIL \n", 0);
        } else {
            peq_relay_dbg.send_idx++;
        }
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

    /* Sometimes Partner send channel_id with relay_flag. */
    channel = RACE_CHANNEL_ID_CLEAR_RELAY_CMD_FLAG(channel);

    RSP *rsp = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                     RACE_CMDRELAY_PASS_TO_DST,
                     (sizeof(RSP) + pMsg->hdr.length - 2),
                     channel);
    if (rsp != NULL) {
        rsp->dst_type = PARTENER_MATCHED_CHANNEL_TYPE;
        rsp->dst_id = PARTENER_MATCHED_CHANNEL_ID;
        memcpy(&(rsp->race_cmd_rsp.hdr), &(pMsg->hdr), RACE_CMD_HDR_LEN);
        memcpy(rsp->race_cmd_rsp.payload, pMsg->payload, pMsg->hdr.length - 2);
        RACE_LOG_MSGID_I("[relay_cmd][relay_dbg] agent flush relay rsp id: 0x%04X, len:%d\n", 1, pMsg->hdr.id, pMsg->hdr.length);

#if 0//def MTK_USER_TRIGGER_ADAPTIVE_FF_V2
        if ((pMsg->hdr.id == RACE_DSPREALTIME_ANC_ADAPTIVE_CHECK) && (pMsg->hdr.type == RACE_TYPE_NOTIFICATION)) {
            adaptive_check_notify_t *payload_hdr = (adaptive_check_notify_t *) pMsg->payload;

            if (payload_hdr->mode == ADAPTIVE_FF_ANC_MODE) {
                race_send_pkt_t* pSndPkt;
                uint32_t port_handle, ret_size, size;
                uint8_t *ptr;
                uint32_t send_res = 0;

                pSndPkt = race_pointer_cnv_pkt_to_send_pkt((void *)rsp);
                port_handle = race_get_port_handle_by_channel_id(channel);

                size = pSndPkt->length;
                ptr = (uint8_t*)&pSndPkt->race_data;

                ret_size = race_port_send_data(port_handle, ptr, size);
                send_res = (ret_size & (1<<31));
                ret_size &= ~(1<<31);
                RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff] (Partner)send_size:%u, send_res:%u, ret_size:%u", 3, size, send_res , ret_size);

                if (ret_size == size) {
                    //transmit success, inform parnter
//                    bt_aws_mce_report_info_t info;
//                    info.module_id = BT_AWS_MCE_REPORT_MODULE_USR_TRIGR_FF;
//                    info.param_len = sizeof(uint32_t);
//                    uint32_t event_id = (1<<0);
//                    info.param = &event_id;
//                    if (bt_aws_mce_report_send_event(&info) != BT_STATUS_SUCCESS) {
//                        RACE_LOG_MSGID_E("[relay_cmd][user_trigger_ff] transmit data from Partner success, but inform Partner fail", 0);
//                    }
                    RACE_LOG_MSGID_I("[relay_cmd][user_trigger_ff] (Partner)Send data to APK", 0);

                } else if (ret_size == 0) {
                    RACE_LOG_MSGID_E("[relay_cmd][user_trigger_ff] (Partner)pkt loss, wait for next time", 0);

                }else {
//                    audio_user_trigger_adaptive_ff_wait_for_sending_parnter_relay_data(pSndPkt, port_handle, ret_size);

                    RACE_LOG_MSGID_E("[relay_cmd][user_trigger_ff] (Partner)remain len:%d, wait for next time", 1, size-ret_size);

//                    bt_aws_mce_report_info_t info;
//                    info.module_id = BT_AWS_MCE_REPORT_MODULE_USR_TRIGR_FF;
//                    info.param_len = sizeof(uint32_t);
//                    uint32_t event_id = USER_TRIGGER_ADAPTIVE_FF_AGENT_SEND_RACE_FAIL;//let partner stop timer
//                    info.param = &event_id;
//                    if (bt_aws_mce_report_send_event(&info) != BT_STATUS_SUCCESS) {
//                        RACE_LOG_MSGID_E("[relay_cmd][user_trigger_ff] transmit data from Partner success, but inform Partner fail", 0);
//                    }
                }
                race_mem_free(pSndPkt);
                return;
            }
        }
#endif

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

void race_cmd_relay_aws_mce_msg_process(race_general_msg_t *msg)
{
    uint8_t channel;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    race_pkt_t *pMsg;

    if ((msg == NULL) || (msg->msg_data == NULL)) {
        return;
    }
    channel = msg->msg_data[0];
    pMsg = (race_pkt_t*)(msg->msg_data + 1);
    RACE_LOG_MSGID_I("[relay_cmd] aws_msg_process: role :%x\r\n", 1, role);
    if (role == BT_AWS_MCE_ROLE_PARTNER) {
        race_cmd_relay_req_hdlr(pMsg, channel);
    } else if (role == BT_AWS_MCE_ROLE_AGENT) {
        race_cmd_relay_rsp_hdlr(pMsg, channel);
    }

    race_mem_free(msg->msg_data);
}

static void* RACE_CmdHandler_relay_query_state(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    bt_aws_mce_agent_state_type_t state;
    bt_aws_mce_role_t role;

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
        role = bt_connection_manager_device_local_info_get_aws_role();
        if ((role == BT_AWS_MCE_ROLE_AGENT) && (bt_sink_srv_cm_get_special_aws_device() != NULL)) {
            state = bt_sink_srv_cm_get_aws_link_state();
            RACE_LOG_MSGID_I("[relay_cmd] role = %x, state = %x \r\n",2, role, state);

            if (state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                pEvt->channel_type = PARTENER_MATCHED_CHANNEL_TYPE;/*means agent is in*/
                pEvt->channel_id = PARTENER_MATCHED_CHANNEL_ID;
            } else {
                pEvt->channel_type = 0;
                pEvt->channel_id = 0;
            }
        } else {
            pEvt->channel_type = 0;
            pEvt->channel_id = 0;
        }
    }
    return pEvt;
}

static void* RACE_CmdHandler_relay_pass_to_dst(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    bt_aws_mce_agent_state_type_t state;
    uint16_t data_len;
    int32_t ret = RACE_ERRCODE_FAIL;
    bt_status_t aws_ret;

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

    RSP *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                 RACE_CMDRELAY_PASS_TO_DST,
                                 sizeof(RSP),
                                 channel_id);

    //race_recipient_type_enum recipient_type = race_recipient_type_convt(pCmd->recipient);

    if (pEvt != NULL)
    {
        if (bt_sink_srv_cm_get_special_aws_device() != NULL) {
            state = bt_sink_srv_cm_get_aws_link_state();
            RACE_LOG_MSGID_I("[relay_cmd]aws_state = %x, len = %d\r\n",2, state, pCmd->hdr.length);
            if (pCmd->dst_type == PARTENER_MATCHED_CHANNEL_TYPE
                && state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                ret = RACE_ERRCODE_SUCCESS;
            }
        }

        if (ret == RACE_ERRCODE_SUCCESS) {
            data_len = pCmd->hdr.length - 4;/*race id length 2 bytes + dst_type 1byte + dst_id 1byte*/
            aws_ret = bt_send_aws_mce_race_cmd_data(&(pCmd->sub_race_cmd), data_len, channel_id, RACE_CMD_RELAY_FROM_AGENT,peq_relay_dbg.send_idx);
            if(aws_ret != BT_STATUS_SUCCESS) {
                ret = RACE_ERRCODE_FAIL;
                RACE_LOG_MSGID_I("[relay_cmd] send aws mce data FAIL \n", 0);
            } else {
                peq_relay_dbg.send_idx++;
                RACE_LOG_MSGID_I("[relay_cmd] send aws mce data SUCCESS \n", 0);
            }
        }

        pEvt->status = ret;

        RACE_LOG_MSGID_I("[relay_cmd]status[0x%X]",1, pEvt->status);
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

#ifdef MTK_MUX_AWS_MCE_ENABLE
static void race_relay_mux_callback(mux_handle_t handle, mux_event_t event,uint32_t data_len,void*user_data)
{
    uint32_t recv_data_len;
    bt_aws_mce_report_info_t info;
    race_aws_mce_packet_t *aws_mce_pkt;
    mux_buffer_t mux_buffer;

    RACE_LOG_MSGID_W("race_relay_mux_callback(), event = %d, data_len = %d\r\n", 2, event, data_len);

    aws_mce_pkt = (race_aws_mce_packet_t*)race_mem_alloc(255);
    mux_buffer.p_buf = (void*)aws_mce_pkt;
    mux_buffer.buf_size = 255;
    mux_rx(handle, &mux_buffer, &recv_data_len);
    RACE_LOG_MSGID_W("race_relay_mux_callback() mux_rx() recv_data_len = %d\r\n",1, recv_data_len);
    info.is_sync = false;
    info.module_id = aws_mce_pkt->header.module_id;
    info.param_len = aws_mce_pkt->header_payload.len;
    info.param = (void*)&(aws_mce_pkt->header_race_cmd);

    if (info.param_len >
            (255 - sizeof(bt_aws_mce_report_packet_header_t) - sizeof(bt_aws_mce_report_payload_header_t) - sizeof(race_cmd_aws_mce_packet_hdr_t))) {
        RACE_LOG_MSGID_E("race_relay_mux_callback(), param_len too long %d", 1, info.param_len);
        return;
    }
    memcpy(info.param, &(aws_mce_pkt->header_race_cmd), info.param_len);
    info.sync_time = 0;
    bt_aws_mce_report_relay_cmd_callback(&info);
    race_mem_free(aws_mce_pkt);

    return;
}
#endif

void race_relay_cmd_init(void)
{
    if (relay_init == 0) {
#ifdef MTK_MUX_AWS_MCE_ENABLE
        mux_status_t mux_status;

        port_mux_aws_mce_init();
        mux_status=mux_open(MUX_AWS_MCE, "mux_aws_mce_relay_cmd", &g_race_relay_mux_aws_if_handle, race_relay_mux_callback, NULL);
        RACE_LOG_MSGID_W("[relay_cmd] race_relay_cmd_init() mux_open() ret=%d \r\n",1, mux_status);
        if(mux_status != MUX_STATUS_OK){
            RACE_LOG_MSGID_E("[relay_cmd] fail to register aws mce report callback",0);
        }
        else{
            relay_init = 1;
        }
#else
        if (bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_RELAY_CMD, bt_aws_mce_report_relay_cmd_callback) != BT_STATUS_SUCCESS) {
            RACE_LOG_MSGID_E("[relay_cmd] fail to register aws mce report callback",0);
        } else {
            relay_init = 1;
        }
#endif
    }
}

#elif defined(RACE_DUMMY_RELAY_CMD_ENABLE) //for not aws-mce project

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
        pEvt->channel_type = 0;
        pEvt->channel_id = 0;
    }
    return pEvt;
}

void* RACE_CmdHandler_RELAY_RACE_CMD(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_CmdHandler_RELAY_RACE_CMD, type[0x%X], id[0x%X]", 2, pCmdMsg->hdr.type, pCmdMsg->hdr.id);

    if (pCmdMsg->hdr.type == RACE_TYPE_COMMAND ||
        pCmdMsg->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP)
    {
        switch (pCmdMsg->hdr.id)
        {
            case RACE_CMDRELAY_GET_AVA_DST:
               return  RACE_CmdHandler_relay_query_state((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);
            default:
                break;
        }
    }
    return NULL;
}
#endif /* RACE_RELAY_CMD_ENABLE */

