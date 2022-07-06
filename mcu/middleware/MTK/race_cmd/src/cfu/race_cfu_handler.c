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
#ifdef RACE_CFU_HANDLER_ENABLE
#include "race_cmd.h"
#include "race_xport.h"
#include "race_timer.h"
#include "race_event.h"
#include "race_cfu.h"
#include "race_cmd_cfu.h"
#include "race_cfu_internal.h"
#include "race_cfu_handler.h"
#ifdef RACE_LPCOMM_ENABLE
#include "race_lpcomm_trans.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_conn.h"
#endif


typedef struct
{
    U16 packet_id; /* The packet_id of the command being processed. */
    race_cfu_udpate_mode_enum update_mode; /* The update_mode of the command being processed. */
    U8 result_status;
    U8 local_result[RACE_CFU_RESPONSE_REPORT_MAX_SIZE];
    U8 peer_result[RACE_CFU_RESPONSE_REPORT_MAX_SIZE];
} race_cfu_handler_relay_packet_cmd_info_struct;


typedef struct
{
    race_cfu_handler_relay_packet_cmd_info_struct relay_packet_cmd_info;
#ifdef RACE_LPCOMM_ENABLE
    race_cfu_handler_relay_packet_rsp_info_struct relay_packet_rsp_info;
#endif
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
    uint8_t clear_cached_info_timer_id;
#endif
} race_cfu_handler_context_struct;


race_cfu_handler_context_struct g_race_cfu_handler_cntx;
race_cfu_handler_context_struct *g_race_cfu_handler_cntx_ptr;


/* clear_cached_info_timer_id starts and stops too frequently. Disable this feature by default. */
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
U8 race_cfu_handler_get_clear_cached_info_timer_id(void)
{
    if (g_race_cfu_handler_cntx_ptr)
    {
        if (race_timer_smart_is_enabled(g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id))
        {
            return g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id;
        }

        g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id = RACE_TIMER_INVALID_TIMER_ID;
    }

    return RACE_TIMER_INVALID_TIMER_ID;
}


RACE_ERRCODE race_cfu_handler_set_clear_cached_info_timer_id(U8 timer_id)
{
    if (g_race_cfu_handler_cntx_ptr)
    {
        g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id = timer_id;

        return RACE_ERRCODE_SUCCESS;
    }

    return RACE_ERRCODE_WRONG_STATE;
}


void race_cfu_handler_stop_clear_cached_info_timer(void)
{
    if (g_race_cfu_handler_cntx_ptr &&
        RACE_TIMER_INVALID_TIMER_ID != g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id)
    {
        race_timer_smart_stop(g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id, NULL);
        g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id = RACE_TIMER_INVALID_TIMER_ID;
    }
}


void race_cfu_handler_clear_cached_info_timer_hdl(uint8_t id, void *user_data)
{
    RACE_LOG_MSGID_I("[CFU][RACE] id:%d user_data:%x clear_cached_info_timer_id:%d rsp packet_id:%d cmd packet_id:%d",5,
                    id,
                    user_data,
                    g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id : RACE_TIMER_INVALID_TIMER_ID,
#ifdef RACE_LPCOMM_ENABLE
                    g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet_id : RACE_CFU_PACKET_ID_INVALID,
#else
                    RACE_CFU_PACKET_ID_INVALID,
#endif
                    g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.packet_id : RACE_CFU_PACKET_ID_INVALID);

    if (!g_race_cfu_handler_cntx_ptr ||
        RACE_TIMER_INVALID_TIMER_ID == g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id ||
        g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id != id)
    {
        return;
    }

    race_cfu_handler_reset_relay_packet_cmd_info();
#ifdef RACE_LPCOMM_ENABLE
    race_cfu_handler_reset_relay_packet_rsp_info();
#endif
    race_cfu_handler_stop_clear_cached_info_timer();
}
#endif


bool race_cfu_handler_wait_rsp(void)
{
/* If the timer is not used, packet_id may be a valid value for ever until the new cmd comes. */
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
    if (g_race_cfu_handler_cntx_ptr &&
        RACE_CFU_PACKET_ID_INVALID != g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.packet_id)
    {
        return TRUE;
    }


#ifdef RACE_LPCOMM_ENABLE
    if (g_race_cfu_handler_cntx_ptr &&
        RACE_CFU_PACKET_ID_INVALID != g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet_id)
    {
        return TRUE;
    }
#endif
#endif

    return FALSE;
}


void race_cfu_handler_reset_relay_packet_cmd_info(void)
{
    RACE_LOG_MSGID_I("[CFU][RACE] race_cfu_handler_reset_relay_packet_cmd_info packet_id:%d", 1,
                     g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.packet_id : RACE_CFU_PACKET_ID_INVALID);

    if (g_race_cfu_handler_cntx_ptr)
    {
        memset(&(g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info), 0, sizeof(race_cfu_handler_relay_packet_cmd_info_struct));
        g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.packet_id = RACE_CFU_PACKET_ID_INVALID;
    }
}


#ifdef RACE_LPCOMM_ENABLE
race_cfu_handler_relay_packet_rsp_info_struct *race_cfu_handler_get_relay_packet_rsp_info(void)
{
    return g_race_cfu_handler_cntx_ptr ? &(g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info) : NULL;
}


void race_cfu_handler_reset_relay_packet_rsp_info(void)
{
    RACE_LOG_MSGID_I("race_cfu_handler_reset_relay_packet_rsp_info packet_id:%d", 1,
                     g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet_id : RACE_CFU_PACKET_ID_INVALID);

    if (g_race_cfu_handler_cntx_ptr)
    {
        memset(&(g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info), 0, sizeof(race_cfu_handler_relay_packet_rsp_info_struct));
        g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet_id = RACE_CFU_PACKET_ID_INVALID;
    }
}
#endif


RACE_ERRCODE race_cfu_handler_race_event_processer(int32_t register_id, race_event_type_enum event_type, void *param, void *user_data)
{
    switch (event_type)
    {
        case RACE_EVENT_TYPE_CONN_SPP_DISCONNECT:
        {
            race_cfu_handler_reset_relay_packet_cmd_info();
#ifdef RACE_LPCOMM_ENABLE
            race_cfu_handler_reset_relay_packet_rsp_info();
#endif
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
            race_cfu_handler_stop_clear_cached_info_timer();
#endif
            break;
        }

#ifdef RACE_LPCOMM_ENABLE
        /* Reset the cached info when RHO occurs no matter what the RHO result is. */
        case RACE_EVENT_TYPE_BT_RHO_PREPARE:
        case RACE_EVENT_TYPE_BT_RHO_RESULT:
        {
            race_cfu_handler_reset_relay_packet_cmd_info();
            race_cfu_handler_reset_relay_packet_rsp_info();
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
            race_cfu_handler_stop_clear_cached_info_timer();
#endif
            break;
        }
#endif

        default:
            break;
    }

    return RACE_ERRCODE_SUCCESS;
}


bool race_cfu_handler_check_cmd_process_finish_status(race_cfu_handler_relay_packet_cmd_info_struct *cmd_info)
{
    if (cmd_info &&
        ((RACE_CFU_UPDATE_MODE_SINGLE_DEVICE == cmd_info->update_mode &&
          RACE_CFU_RESULT_STATUS_LOCAL_DONE & cmd_info->result_status &&
          !(RACE_CFU_RESULT_STATUS_PEER_DONE & cmd_info->result_status))
#ifdef RACE_LPCOMM_ENABLE
         || (RACE_CFU_UPDATE_MODE_DUAL_DEVICE == cmd_info->update_mode &&
             RACE_CFU_RESULT_STATUS_LOCAL_DONE & cmd_info->result_status &&
             RACE_CFU_RESULT_STATUS_PEER_DONE & cmd_info->result_status)
#endif
           ))
    {
        return TRUE;
    }

    return FALSE;
}


/* If status is not RACE_ERRCODE_SUCCESS, it means that at least one rsp is failed to get. In such case, send response to builder with report_count of 0 even
 * if there is one rsp obtained.
 */
void *race_cfu_handler_relay_packet_response_generate(U8 status, race_cfu_handler_relay_packet_cmd_info_struct *cmd_info)
{
    U32 rsp_len = 0, data_len = 1;
    race_cfu_relay_packet_response_struct *rsp = NULL;
    U8 *data = NULL;

    if (!cmd_info ||
        RACE_CFU_UPDATE_MODE_MAX == cmd_info->update_mode ||
        (RACE_ERRCODE_SUCCESS == status &&
         !race_cfu_handler_check_cmd_process_finish_status(cmd_info)))
    {
        return NULL;
    }

    if (RACE_ERRCODE_SUCCESS == status)
    {
        if (RACE_CFU_UPDATE_MODE_SINGLE_DEVICE == cmd_info->update_mode)
        {
            /* report_count + report. */
            data_len = 1 + RACE_CFU_RESPONSE_REPORT_SIZE;
        }
        else if (RACE_CFU_UPDATE_MODE_DUAL_DEVICE == cmd_info->update_mode)
        {
            /* report_count + report + report. */
            data_len = 1 + RACE_CFU_RESPONSE_REPORT_SIZE + RACE_CFU_RESPONSE_REPORT_SIZE;
        }
    }

    rsp_len = sizeof(race_cfu_relay_packet_response_struct) + data_len;

    rsp = RACE_ClaimPacketAppID(0,
                                RACE_TYPE_RESPONSE,
                                RACE_CFU_RELAY_PACKET,
                                rsp_len,
                                RACE_CFU_BUILDER_HANDLER_CHANNEL_ID);
    if (rsp)
    {
        rsp->packet_type = RACE_CFU_PACKET_TYPE_RSP;
        rsp->packet_id = cmd_info->packet_id;
        rsp->update_mode = cmd_info->update_mode;
        rsp->data_len = data_len;
        rsp->data[0] = 0; /* report_count */
        data = &rsp->data[0] + 1;

        /* If status is not RACE_ERRCODE_SUCCESS, report_count will be 0 and no report will be inserted into rsp. */
        if ((RACE_CFU_RESULT_STATUS_LOCAL_DONE & cmd_info->result_status) &&
            data_len >= data - &rsp->data[0] + RACE_CFU_RESPONSE_REPORT_SIZE)
        {
            (rsp->data[0])++;
            memcpy(data, cmd_info->local_result, RACE_CFU_RESPONSE_REPORT_SIZE);
            data += RACE_CFU_RESPONSE_REPORT_SIZE;
        }

        if ((RACE_CFU_RESULT_STATUS_PEER_DONE & cmd_info->result_status) &&
            data_len >= data - &rsp->data[0] + RACE_CFU_RESPONSE_REPORT_SIZE)
        {
            (rsp->data[0])++;
            memcpy(data, cmd_info->peer_result, RACE_CFU_RESPONSE_REPORT_SIZE);
        }
    }
    else
    {
        RACE_LOG_MSGID_W("No enough memory for rsp. rsp_len:%d", 1, rsp_len);
    }

    return rsp;
}


void race_cfu_handler_process_relay_packet_response(race_cfu_packet_struct *packet, bool is_local_rsp)
{
    race_cfu_relay_packet_response_struct *pRsp = NULL;
    RACE_ERRCODE status = RACE_ERRCODE_SUCCESS;

    if (!packet ||
        !g_race_cfu_handler_cntx_ptr ||
        RACE_CFU_PACKET_TYPE_RSP != packet->packet_type ||
        RACE_CFU_PACKET_ID_INVALID == g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.packet_id ||
        packet->packet_id != g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.packet_id ||
        (is_local_rsp && (g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.result_status & RACE_CFU_RESULT_STATUS_LOCAL_DONE)) ||
        (!is_local_rsp && (g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.result_status & RACE_CFU_RESULT_STATUS_PEER_DONE)))
    {
        RACE_LOG_MSGID_W("Drop packet. is_local_rsp:%d packet info:packet_type:%d packet_id:%d data_len:%d. cached info: packet_id%d result_status:%x update_mode:%d", 7,
            is_local_rsp,
            packet ? packet->packet_type : RACE_CFU_PACKET_TYPE_MAX,
            packet ? packet->packet_id : 0,
            packet ? packet->data_len : 0,
            g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.packet_id : 0,
            g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.result_status : 0xFF,
            g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.update_mode : RACE_CFU_UPDATE_MODE_MAX);

        return;
    }

    if (1 + RACE_CFU_RESPONSE_REPORT_SIZE > packet->data_len ||
        0 == packet->data[0])
    {
        /* Invalid rsp or rsp is failed to get. */
        RACE_LOG_MSGID_W("Fail to get rsp. is_local_rsp:%d packet_id:%d result_status:%x", 3,
                         is_local_rsp, packet->packet_id,
                         g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.result_status);
        status = RACE_ERRCODE_FAIL;
    }
    else
    {
        if (is_local_rsp)
        {
            g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.result_status |= RACE_CFU_RESULT_STATUS_LOCAL_DONE;
            memcpy(g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.local_result, &(packet->data[1]), RACE_CFU_RESPONSE_REPORT_SIZE);
        }
        else
        {
            g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.result_status |= RACE_CFU_RESULT_STATUS_PEER_DONE;
            memcpy(g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.peer_result, &(packet->data[1]), RACE_CFU_RESPONSE_REPORT_SIZE);
        }
    }

    if (RACE_ERRCODE_SUCCESS != status ||
        race_cfu_handler_check_cmd_process_finish_status(&g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info))
    {
        pRsp = race_cfu_handler_relay_packet_response_generate(status, &g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info);
        race_cfu_handler_reset_relay_packet_cmd_info();
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
        race_cfu_handler_stop_clear_cached_info_timer();
#endif
    }

    if (pRsp)
    {
        race_flush_packet((uint8_t *)pRsp, RACE_CFU_BUILDER_HANDLER_CHANNEL_ID);
    }
}


void race_cfu_handler_send_data_msg_processer(race_cfu_packet_struct *packet)
{
#ifdef RACE_LPCOMM_ENABLE
    race_lpcomm_role_enum role = race_lpcomm_role_get(RACE_LPCOMM_TRANS_METHOD_AWS);
#endif

#ifdef RACE_LPCOMM_ENABLE
    if (RACE_LPCOMM_ROLE_PARTNER == role)
    {
        race_cfu_relay_packet_lpcomm_rsp_struct *lpcomm_rsp = NULL;
        U32 lpcomm_rsp_len = 0;

        if (!packet ||
            !g_race_cfu_handler_cntx_ptr ||
            RACE_CFU_PACKET_TYPE_RSP != packet->packet_type ||
            RACE_CFU_PACKET_ID_INVALID == g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet_id ||
            g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet_id != packet->packet_id)
        {
            RACE_LOG_MSGID_W("Drop packet. packet_type:%d received packet_id:%d cached packet_id:%d", 3,
                             packet ? packet->packet_type : RACE_CFU_PACKET_TYPE_MAX,
                             packet ? packet->packet_id : 0,
                             g_race_cfu_handler_cntx_ptr ? g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet_id : 0);

            return;
        }

        lpcomm_rsp_len = sizeof(race_cfu_relay_packet_lpcomm_rsp_struct) + packet->data_len;
        lpcomm_rsp = race_mem_alloc(lpcomm_rsp_len);
        if (lpcomm_rsp)
        {
            /* Relay the rsp to the agent. */
            lpcomm_rsp->status = RACE_ERRCODE_SUCCESS;
            memcpy(&(lpcomm_rsp->packet), packet, sizeof(race_cfu_packet_struct) + packet->data_len);
            race_lpcomm_send_race_cmd_rsp_to_peer((uint8_t *)lpcomm_rsp,
                                                  lpcomm_rsp_len,
                                                  RACE_LPCOMM_ROLE_PARTNER,
                                                  g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet.cmd_id,
                                                  g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet.app_id,
                                                  g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet.channel_id,
                                                  g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet.process_id,
                                                  g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.packet.trans_method,
                                                  g_race_cfu_handler_cntx_ptr->relay_packet_rsp_info.device_id);
            race_mem_free(lpcomm_rsp);
        }
        else
        {
            RACE_LOG_MSGID_W("No enough memory for lpcomm_rsp. lpcomm_rsp_len:%d", 1, lpcomm_rsp_len);
        }

#ifdef RACE_LPCOMM_ENABLE
        race_cfu_handler_reset_relay_packet_rsp_info();
#endif
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
        race_cfu_handler_stop_clear_cached_info_timer();
#endif
    }
    else
#endif
    {
        race_cfu_handler_process_relay_packet_response(packet, TRUE);
    }
}


/* Do not support multiple cmds. */
void *race_cfu_handler_relay_packet_cmd_processer(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);

    typedef struct
    {
        RACE_COMMON_HDR_STRU Hdr;
        race_cfu_relay_packet_command_struct packet;
    } PACKED CMD;

    CMD *pCmd = (CMD *)pCmdMsg;
    race_cfu_relay_packet_response_struct *pRsp = NULL;
    race_cfu_udpate_mode_enum update_mode = race_cfu_get_update_mode();
    race_cfu_receive_callback recv_cb = race_cfu_get_recv_cb();
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    if (!pCmd ||
        !recv_cb ||
        RACE_CFU_PACKET_TYPE_CMD != pCmd->packet.packet_type ||
        RACE_CFU_UPDATE_MODE_MAX == update_mode ||
        RACE_CFU_UPDATE_MODE_UNKNOWN == update_mode ||
#ifndef RACE_LPCOMM_ENABLE
        RACE_CFU_UPDATE_MODE_DUAL_DEVICE == pCmd->packet.update_mode ||
#endif
        RACE_CFU_UPDATE_MODE_MAX == pCmd->packet.update_mode)
    {
        RACE_LOG_MSGID_W("Drop packet for invalid param. pCmd:%x local update_mode:%d received update_mode:%d packet_type:%d", 4,
                         pCmd, update_mode,
                         pCmd ? pCmd->packet.update_mode : RACE_CFU_UPDATE_MODE_MAX,
                         pCmd ? pCmd->packet.packet_type : RACE_CFU_PACKET_TYPE_MAX);
        return NULL;
    }

    /* New cmd received. No matter if the udpate_mode matches or not, drop the previous unfinished cmd. */
    race_cfu_handler_reset_relay_packet_cmd_info();

    if (RACE_CFU_UPDATE_MODE_UNKNOWN == pCmd->packet.update_mode ||
        update_mode == pCmd->packet.update_mode)
    {
#ifdef RACE_LPCOMM_ENABLE
        if (RACE_CFU_UPDATE_MODE_SINGLE_DEVICE == update_mode)
        {
            /* Return fail to dongle directly if the update_mode is single_device and the partner is attached. */
            if (race_lpcomm_is_attached(RACE_LPCOMM_DEFAULT_DEVICE_ID))
            {
                RACE_LOG_MSGID_W("Drop packet for partner attached when update_mode is single device.", 0);
                ret = RACE_ERRCODE_NOT_ALLOWED;
                goto end;
            }
        }

        /* Relay the data to the partner. */
        if (RACE_CFU_UPDATE_MODE_DUAL_DEVICE == update_mode)
        {
            U16 process_id = race_gen_process_id();
            race_cfu_relay_packet_lpcomm_req_struct *req = (race_cfu_relay_packet_lpcomm_req_struct *)&(pCmd->packet);

            /* Send the req to the peer */
#ifdef RACE_AWS_ENABLE
            ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)req,
                                                        sizeof(race_cfu_relay_packet_lpcomm_req_struct) + req->data_len,
                                                        RACE_LPCOMM_ROLE_AGENT,
                                                        RACE_CFU_RELAY_PACKET,
                                                        pCmdMsg->pktId.field.app_id,
                                                        channel_id,
                                                        process_id,
                                                        RACE_LPCOMM_TRANS_METHOD_AWS,
                                                        RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_AWS_ENABLE */
        }
#endif

        if (RACE_ERRCODE_SUCCESS != ret)
        {
            goto end;
        }

        /* Notify the local cfu device of the data. */
        recv_cb(&(pCmd->packet));

        /* Cache the info of the new cmd. */
        g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.packet_id = pCmd->packet.packet_id;
        g_race_cfu_handler_cntx_ptr->relay_packet_cmd_info.update_mode = update_mode;
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
        if (RACE_TIMER_INVALID_TIMER_ID == g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id)
        {
            ret = race_timer_smart_start(&g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id,
                                         RACE_TIMER_CFU_CLEAR_CACHED_INFO_DELAY_IN_MS,
                                         race_cfu_handler_clear_cached_info_timer_hdl,
                                         NULL);
        }
        else
        {
            ret = race_timer_smart_reset(g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id);
        }
#endif
    }
    else
    {
        RACE_LOG_MSGID_W("update_mode not match. local:%d remote:%d", 2, update_mode, pCmd->packet.update_mode);
        ret = RACE_ERRCODE_CONFLICT;
    }

end:
    if (RACE_ERRCODE_SUCCESS != ret)
    {
        race_cfu_handler_relay_packet_cmd_info_struct *cmd_info = race_mem_alloc(sizeof(race_cfu_handler_relay_packet_cmd_info_struct));

        if (cmd_info)
        {
            memset(cmd_info, 0, sizeof(race_cfu_handler_relay_packet_cmd_info_struct));
            cmd_info->packet_id = pCmd->packet.packet_id;
            cmd_info->update_mode = update_mode;

            pRsp = race_cfu_handler_relay_packet_response_generate(ret, cmd_info);
            race_mem_free(cmd_info);
        }
        else
        {
            RACE_LOG_MSGID_W("No enough memory for cmd_info.", 0);
        }

        race_cfu_handler_reset_relay_packet_cmd_info();
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
        race_cfu_handler_stop_clear_cached_info_timer();
#endif
    }

    return pRsp;
}


void* race_cfu_handler_race_cmd_processer(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    if (pCmdMsg->hdr.type == RACE_TYPE_COMMAND)
    {
        switch (pCmdMsg->hdr.id)
        {
            case RACE_CFU_RELAY_PACKET:
                return race_cfu_handler_relay_packet_cmd_processer((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

            default:
                break;
        }
    }

    return NULL;
}


void race_cfu_handler_init(void)
{
    if (g_race_cfu_handler_cntx_ptr)
    {
        return;
    }

    g_race_cfu_handler_cntx_ptr = &g_race_cfu_handler_cntx;
#ifdef RACE_CFU_HANDLER_CLEAR_CACHED_INFO_TIMER_ENABLE
    g_race_cfu_handler_cntx_ptr->clear_cached_info_timer_id = RACE_TIMER_INVALID_TIMER_ID;
#endif
    race_cfu_handler_reset_relay_packet_cmd_info();
#ifdef RACE_LPCOMM_ENABLE
    race_cfu_handler_reset_relay_packet_rsp_info();
#endif
}
#endif /* RACE_CFU_HANDLER_ENABLE */

