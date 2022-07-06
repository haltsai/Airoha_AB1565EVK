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


#include "race_cmd_find_me.h"
#include "stdio.h"
#include "race_lpcomm_aws.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_cmd_fota.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_conn.h"
#include "race_lpcomm_msg_struct.h"
#include "race_noti.h"
#include "race_bt.h"
#include "race_lpcomm_ps_noti.h"
#include "race_timer.h"
#include "race_lpcomm_packet.h"
#include "race_cmd.h"
#include "bt_sink_srv_ami.h"
#include "race_event_internal.h"


#ifdef RACE_FIND_ME_ENABLE
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t is_blink;
        uint8_t is_tone;
        uint8_t recipient;
    } PACKED CMD;


#define RACE_CMD_GET_AGENT 0
#define RACE_CMD_GET_PARTER 1
#define RACE_CMD_GET_ALL 2

#ifndef UNUSED
#define UNUSED(x)  ((void)(x))
#endif

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
static race_find_me_by_method_t s_fm_method_cntx = {0};
#endif


static void* RACE_CmdHandler_FIND_ME_by_cmd(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id);

static void* RACE_CmdHandler_FIND_ME_query_state(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id);

static int32_t race_cmd_find_me_agent(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);

#ifdef RACE_LPCOMM_ENABLE
static int32_t race_cmd_find_me_partener(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id);
#endif

uint8_t default_find_me_get_device_role(int32_t capacity)
{
    return 0;
}

void* RACE_CmdHandler_FIND_ME(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);
    RACE_LOG_MSGID_I("RACE_CmdHandler_FIND_ME, type[0x%X], id[0x%X]", 2, pCmdMsg->hdr.type, pCmdMsg->hdr.id);

    if (pCmdMsg->hdr.type == RACE_TYPE_COMMAND ||
        pCmdMsg->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP)
    {

        switch (pCmdMsg->hdr.id)
        {
            case RACE_FIND_ME_QUERY_STATE:
               return  RACE_CmdHandler_FIND_ME_query_state((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);
            case RACE_CMD_FIND_ME:
                return RACE_CmdHandler_FIND_ME_by_cmd(pCmdMsg, length, channel_id);
            default:
                break;
        }



    }
    return NULL;
}


static void* RACE_CmdHandler_FIND_ME_query_state(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
        uint8_t cli_in_existence;
    }PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_FIND_ME_QUERY_STATE,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

    RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);
    if (pEvt)
    {
#ifndef RACE_LPCOMM_ENABLE /*dis single & dual*/
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else

#ifdef RACE_AWS_ENABLE
        pEvt->cli_in_existence = race_lpcomm_is_attached(RACE_LPCOMM_DEFAULT_DEVICE_ID);
        RACE_LOG_MSGID_I("[FIND_ME] in_existence= %x \r\n",1, pEvt->cli_in_existence);

        ret = RACE_ERRCODE_SUCCESS;
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_AWS_ENABLE */
#endif /* RACE_LPCOMM_ENABLE */

        pEvt->status = ret;
    }

    return pEvt;
}


static int32_t race_cmd_find_me_agent(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;

    race_find_me_noti_struct *noti = NULL;

    /* A1. Execute the cmd. */
    if (pCmd == NULL)
    {
        RACE_LOG_MSGID_I("[FIND_ME]cmd is null\r\n", 0);
        return ret;
    }
    ret =  race_event_send_find_me_event(pCmd->is_blink, pCmd->is_tone);

    RACE_LOG_MSGID_I("[FIND_ME]ret = %x \r\n",1, ret);

    if (RACE_ERRCODE_SUCCESS == ret)
    {
      /* A2. Create the noti(to Smart Phone). */
      noti = (void *)RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                           RACE_TYPE_NOTIFICATION,
                                           RACE_CMD_FIND_ME,
                                           sizeof(race_find_me_noti_struct),
                                           channel_id);
      if (noti)
      {
          /* A3. Set the noti parameters with the cmd results.  */
          noti->status = ret;
          noti->recipient = pCmd->recipient;

          /* A4. Send the noti. */
          ret = race_noti_send(noti, channel_id, TRUE);


          if (RACE_ERRCODE_SUCCESS != ret)
          {
              /* A5. Free the noti if needed. */
              RACE_FreePacket(noti);
              noti = NULL;
          }
      }
      else
      {
          ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
      }
    }
    RACE_LOG_MSGID_I("[FIND_ME]ret = %x \r\n",1, ret);
    return ret;
}


#ifdef RACE_LPCOMM_ENABLE
static int32_t race_cmd_find_me_partener(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
#ifdef RACE_LPCOMM_ENABLE

    race_find_me_noti_struct *noti = NULL;
#ifdef RACE_AWS_ENABLE
    race_lpcomm_find_me_req_struct req = {0};
#endif

    /* process_id must be initialized to 0. */
    uint16_t process_id = 0;
    bool dual_cmd = FALSE;
    bool noti_sent = FALSE;
#endif

#ifndef RACE_LPCOMM_ENABLE
    ret = RACE_ERRCODE_NOT_SUPPORT;
#else
    if (pCmd->recipient == RACE_CMD_GET_ALL) /*2 agent & partner, need to send twice. */
    dual_cmd = TRUE;
    /* 1. Create Noti */
    ret = race_lpcomm_ps_noti_create((void **)&noti,
                                     &process_id,
                                     RACE_CMD_FIND_ME,
                                     pCmdMsg->hdr.pktId.field.app_id,
                                     dual_cmd,
                                     sizeof(race_find_me_noti_struct),
                                     channel_id);
    RACE_LOG_MSGID_I("[FIND_ME]find_me_patener:id = %x\r\n",1, process_id);

    if (RACE_ERRCODE_SUCCESS == ret)
    {
        /* 2. Store the const parameters in noti. */
        noti->recipient = pCmd->recipient;
        if (pCmd->recipient == RACE_CMD_GET_ALL) {
        noti->status = race_event_send_find_me_event(pCmd->is_blink, pCmd->is_tone);
        /*save const*/
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                            process_id,
                                            channel_id,
                                            noti->status,
                                            RACE_LPCOMM_ROLE_AGENT,
                                            TRUE);
        }
        RACE_LOG_MSGID_I("[FIND_ME]find_me_patener: ret = %d, recipient = %x, status = %x\r\n",3, ret , noti->recipient, noti->status);
#ifdef RACE_AWS_ENABLE
        /* C3. Send the req to the peer */
        req.is_tone = pCmd->is_tone;
        req.is_blink = pCmd->is_blink;
        RACE_LOG_MSGID_I("[FIND_ME]find_me_patener: is_tone = %x, is_blink = %x\r\n",2, req.is_tone, req.is_blink);
        ret = race_lpcomm_send_race_cmd_req_to_peer((uint8_t *)&req,
                                           sizeof(race_lpcomm_find_me_req_struct),
                                           RACE_LPCOMM_ROLE_AGENT,
                                           RACE_CMD_FIND_ME,
                                           pCmdMsg->hdr.pktId.field.app_id,
                                           channel_id,
                                           process_id,
                                           RACE_LPCOMM_TRANS_METHOD_AWS,
                                           RACE_LPCOMM_DEFAULT_DEVICE_ID);
        RACE_LOG_MSGID_I("[FIND_ME]find_me_patener: ret = %x\r\n",1, ret);

        if (RACE_ERRCODE_SUCCESS != ret)
        {
            /* C4. Free the noti if needed */
            race_lpcomm_ps_noti_free(process_id);
            noti = NULL;
        }
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_AWS_ENABLE */
    }
#endif
    return ret;
}
#endif


static void* RACE_CmdHandler_FIND_ME_by_cmd(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    UNUSED(length);
    audio_channel_t role;

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;
    CMD *pCmd = (CMD *)pCmdMsg;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_CMD_FIND_ME,
                                      sizeof(RSP),
                                      channel_id);

    //race_recipient_type_enum recipient_type = race_recipient_type_convt(pCmd->recipient);
    RACE_LOG_MSGID_I("[FIND_ME] app_id = %x, is_blink = %x, is_tone = %x\r\n",3, pCmdMsg->hdr.pktId.field.app_id, pCmd->is_blink, pCmd->is_tone);


    if (pEvt != NULL)
    {
        role = ami_get_audio_channel();
        RACE_LOG_MSGID_I("[FIND_ME]recipient = %x, role = %x\r\n",2, pCmd->recipient, role);
        if (role == AUDIO_CHANNEL_NONE)
        {
            pEvt->status = race_cmd_find_me_agent(pCmdMsg, channel_id);

        } else {
            if (pCmd->recipient == role) { /*0: means left, 1: means right*/
                pEvt->status = race_cmd_find_me_agent(pCmdMsg, channel_id);
            } else {
#ifdef RACE_LPCOMM_ENABLE
                bool cli_in = race_lpcomm_is_attached(RACE_LPCOMM_DEFAULT_DEVICE_ID);
                if (cli_in) {
                    pEvt->status = race_cmd_find_me_partener(pCmdMsg, length, channel_id);
                } else {
                    pEvt->status = RACE_ERRCODE_FAIL;

                }
#else
                pEvt->status = RACE_ERRCODE_FAIL;
#endif
            }
        }
        RACE_LOG_MSGID_I("[FIND_ME]status = %x\r\n",1, pEvt->status);
    }
    return pEvt;
}


#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
static bt_status_t find_me_role_handover_service_allowed(const bt_bd_addr_t *addr)
{
    //ce_status_t status = RACE_STATUS_ERROR;
    bt_status_t status = BT_STATUS_PENDING;

    RACE_LOG_MSGID_I("[FIND_ME]service_allowed: by_spp = %x, by_ble = %x\r\n",2, s_fm_method_cntx.by_spp, s_fm_method_cntx.by_ble);

    if (!s_fm_method_cntx.by_spp && !s_fm_method_cntx.by_ble) {
        status = BT_STATUS_SUCCESS;
    }
    RACE_LOG_MSGID_I("[FIND_ME]service_allowed: pending\r\n", 0);
    return status;
}


static void race_cmd_find_me_ready()
{
    RACE_LOG_MSGID_I("[FIND_ME]find_me_ready: ok\r\n", 0);
    bt_status_t status = bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_FIND_ME);
    s_fm_method_cntx.by_spp = 0;
    s_fm_method_cntx.by_ble = 0;
    RACE_LOG_MSGID_I("[FIND_ME]find_me_ready: status = %x\r\n",1, status);
}


static void find_me_role_handover_result_callback(const bt_bd_addr_t *addr,
                                                                    bt_aws_mce_role_t role,
                                                                    bt_role_handover_event_t event,
                                                                    bt_status_t status)
{
    race_status_t ret = RACE_STATUS_ERROR;
    RACE_LOG_MSGID_I("[FIND_ME]handover_result_callback event= %x, role = %x\r\n",2, event, role);

    switch (event) {
        //todo, if ADV is stopped or connection is disc due to RHO, do we need to start again in agent part after RHO success.
        case BT_ROLE_HANDOVER_PREPARE_REQ_IND:
        {
            RACE_LOG_MSGID_I("[FIND_ME]handover_result_callback: spp = %x, ble = %x\r\n",2, s_fm_method_cntx.by_spp, s_fm_method_cntx.by_ble);
            if (role == BT_AWS_MCE_ROLE_AGENT) {
#ifndef RACE_RHO_WITHOUT_SMARTPHONE_DISCONNECT_ENABLE
                if (s_fm_method_cntx.by_spp) {
                    ret = race_serial_port_close(RACE_SERIAL_PORT_TYPE_SPP);
                } else if (s_fm_method_cntx.by_ble) {
                    ret = race_serial_port_close(RACE_SERIAL_PORT_TYPE_BLE);
                }
                RACE_LOG_MSGID_I("[FIND_ME]close_port: status = %x\r\n",1, status);
#else
                ret = RACE_STATUS_OK;
#endif
                if (ret == RACE_STATUS_OK) {
                    race_cmd_find_me_ready();
                }
            }
            break;
        }
        case BT_ROLE_HANDOVER_COMPLETE_IND:
        {
#ifndef RACE_RHO_WITHOUT_SMARTPHONE_DISCONNECT_ENABLE
            race_status_t ret = race_serial_port_open(RACE_SERIAL_PORT_TYPE_SPP, SERIAL_PORT_DEV_BT_SPP);
            RACE_LOG_MSGID_I("[FIND_ME]handover_end spp : ret = %x\r\n",1, ret);
            ret = race_serial_port_open(RACE_SERIAL_PORT_TYPE_BLE,
                                  SERIAL_PORT_DEV_BT_LE);
            RACE_LOG_MSGID_I("[FIND_ME]handover_end ble: ret = %x\r\n",1, ret);
#endif
            break;
        }

    }
}


void race_cmd_find_me_init()
{
    bt_role_handover_callbacks_t cb = {
        find_me_role_handover_service_allowed,
        NULL,
        NULL,
        NULL,
        find_me_role_handover_result_callback,
    };

    bt_status_t ret = bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_FIND_ME, &cb);
    RACE_LOG_MSGID_I("[FIND_ME]init: status = %x\r\n",1, ret);
}


void race_cmd_find_me_deinit()
{
    bt_status_t ret = bt_role_handover_deregister_callbacks(BT_ROLE_HANDOVER_MODULE_FIND_ME);
    RACE_LOG_MSGID_I("[FIND_ME]deinit: status = %x\r\n",1, ret);
}


void race_cmd_set_find_me_trans_method(race_event_type_enum event_type)
{
    RACE_LOG_MSGID_I("[FIND_ME]set find me trans_method:event_type = %x\r\n",1, event_type);

    switch(event_type) {
        case RACE_EVENT_TYPE_CONN_BLE_CONNECT:
        {
            s_fm_method_cntx.by_ble = 1;
            break;
        }
        case RACE_EVENT_TYPE_CONN_SPP_CONNECT:{
            s_fm_method_cntx.by_spp = 1;
            break;
        }
    }
}


#if 0

void race_cmd_find_me_noti_disconn(race_event_type_enum event_type)
{
    RACE_LOG_MSGID_I("[FIND_ME]race_cmd_find_me_noti_disconn:event_type = %x, ble = %x, spp = %x\r\n",3, event_type, s_fm_method_cntx.by_ble, s_fm_method_cntx.by_spp);

    if (!s_fm_method_cntx.by_ble && !s_fm_method_cntx.by_spp) {
        return;
    }
    if (event_type == RACE_EVENT_TYPE_CONN_BLE_DISCONNECT ||
        event_type == RACE_EVENT_TYPE_CONN_SPP_DISCONNECT) {
        race_cmd_find_me_ready();
    }

}
#endif
#endif /*SUPPORT_ROLE_HANDOVER_SERVICE*/
#endif

