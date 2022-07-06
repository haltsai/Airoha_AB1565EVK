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
#ifdef RACE_BT_CMD_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "syslog.h"
#include "hal.h"
#include "bt_sink_srv.h"
#include "bt_hci.h"
#include "bt_gap_le.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_lpcomm_aws.h"
#include "race_cmd_bluetooth.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_conn.h"
#include "race_lpcomm_msg_struct.h"
#include "race_bt.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"
#include "race_fota_util.h"
#include "race_fota.h"
#include "race_cmd_fcd_cmd.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_le.h"
#include "bt_device_manager_le_config.h"
#include "bt_device_manager_config.h"
#include "bt_device_manager.h"
#include "bt_device_manager_power.h"
#include "bt_connection_manager_internal.h"
#include "bt_hci.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_audio_internal.h"
#include "bt_sink_srv_a2dp.h"
#include "bt_connection_manager.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

extern race_bt_fcd_get_rssi_t g_race_fcd_cntx;
bt_bd_addr_t set_local_addr;
extern void bt_hci_cmd_raw_data_send(uint8_t* buffer);

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////

#ifdef MTK_AWS_MCE_ENABLE
static bt_status_t race_bt_get_rssi(race_bt_rssi_type_t type, bt_aws_mce_role_t role);
#endif
static RACE_ERRCODE race_bt_send_notify_rssi_msg(void);
static void* RACE_BLUETOOTH_SET_BLE_ADV_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_EDR_INQUERY_PAGE_SCAN_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_DISABLE_WBRSSI_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_SET_SNIFF_PARAM_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_SET_RANDOM_ADDR_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_CONN_BLE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_ENABLE_SNIFF_MODE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_AWS_RESET_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_GET_BD_ADDR_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_GET_EDR_LINK_KEY_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_AWS_ENABLE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_GET_A2DP_CODEC_PARAMETERS_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void *RACE_BULETOOTH_SET_LOCAL_ADDR_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_GET_EDR_CONNECTED_DEV_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);
static void* RACE_BLUETOOTH_AWS_GET_ROLE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id);


//////////////////////////////////////////////////////////////////////////////
void DM2L_SetRaceCmdChannel_default(uint8_t channel_id)
{
    return;
}

bool DM2L_SendAwsRdRfStatusToLc_default()
{
    return 0;
}
#pragma weak DM2L_SetRaceCmdChannel = DM2L_SetRaceCmdChannel_default
#pragma weak DM2L_SendAwsRdRfStatusToLc = DM2L_SendAwsRdRfStatusToLc_default
extern void DM2L_SetRaceCmdChannel(uint8_t channel_id);
extern bool DM2L_SendAwsRdRfStatusToLc(void);


void* RACE_BLUETOOTH_GET_LE_LINK_STATUS_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
	typedef struct
	{
		uint8_t status;
	}PACKED RSP;

	RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                  	  (uint16_t)RACE_BLUETOOTH_GET_LE_LINK_STATUS,
                                  	  (uint16_t)sizeof(RSP),
                                  	  channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

	RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (pEvt)
    {
        uint8_t active_link_num = 0;
        bt_gap_le_connection_information_t conn_info = {0};
        bt_handle_t handle = race_bt_get_ble_conn_hdl();

        /* Multi link is not supported currently.
                * BLE connection for FOTA should not be more than one.
                */
        // TODO: what about ble support multiple links?
        active_link_num = 1;
        ret = bt_gap_le_get_connection_information(handle, &conn_info);
        if (BT_STATUS_SUCCESS == ret)
        {
            race_bt_get_le_link_status_noti_struct *noti = NULL;

            noti = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                         RACE_TYPE_NOTIFICATION,
                                         RACE_BLUETOOTH_GET_LE_LINK_STATUS,
                                         sizeof(race_bt_get_le_link_status_noti_struct) + \
                                         active_link_num * sizeof(race_bt_le_link_status_struct),
                                         channel_id);
            if (noti)
            {
                noti->status = RACE_ERRCODE_SUCCESS;
                noti->active_link_num = active_link_num;
                noti->link_status[0].bd_addr_type = BT_ADDR_RANDOM;
                memcpy(&(noti->link_status[0].bd_addr), &(conn_info.local_addr), BT_BD_ADDR_LEN);

                ret = race_noti_send((void *)noti, channel_id, TRUE);
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    RACE_FreePacket((void *)noti);
                    noti = NULL;
                }
            }
            else
            {
                ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            }
        }
        else
        {
            ret = RACE_ERRCODE_FAIL;
        }

        pEvt->status = ret;
    }

	return pEvt;
}

void* RACE_BLUETOOTH_SET_LE_CONNECTION_PARAMETER_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
	{
	    RACE_COMMON_HDR_STRU cmdhdr;
		uint8_t addr_type;
        uint8_t bd_addr[6];
        uint16_t interval_min;
        uint16_t interval_max;
        uint16_t latency;
        uint16_t timeout;
	}PACKED CMD;

	typedef struct
	{
		uint8_t status;
	}PACKED RSP;

	CMD *pCmd = (CMD *)pCmdMsg;
	RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_BLUETOOTH_SET_LE_CONNECTION_PARAMETER,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

	RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (pEvt)
    {
        bt_hci_cmd_le_connection_update_t param = {0};

        // TODO: get conn_hdl by bd_addr and addr_type for multiple BLE support.
        param.connection_handle = race_bt_get_ble_conn_hdl();
        param.conn_interval_min = pCmd->interval_min;
        param.conn_interval_max = pCmd->interval_max;
        param.conn_latency = pCmd->latency;
        param.supervision_timeout = pCmd->timeout;

        ret = bt_gap_le_update_connection_parameter((const bt_hci_cmd_le_connection_update_t *)&param);
        if (BT_STATUS_SUCCESS == ret)
        {
            race_bt_set_le_conn_param_noti_struct *noti = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                                                                RACE_TYPE_NOTIFICATION,
                                                                                RACE_BLUETOOTH_SET_LE_CONNECTION_PARAMETER,
                                                                                sizeof(race_bt_set_le_conn_param_noti_struct),
                                                                                channel_id);
            if (noti)
            {
                noti->status = RACE_ERRCODE_SUCCESS;
                noti->addr_type = pCmd->addr_type;
                memcpy(&(noti->bd_addr), pCmd->bd_addr, BT_BD_ADDR_LEN);

                ret = race_noti_send((void *)noti, channel_id, TRUE);
                if (RACE_ERRCODE_SUCCESS != ret)
                {
                    RACE_FreePacket((void *)noti);
                }
            }
            else
            {
                ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            }
        }
        else
        {
            ret = RACE_ERRCODE_FAIL;
        }

        pEvt->status = ret;
    }

	return pEvt;
}


void* RACE_BLUETOOTH_GET_CIENT_EXISTENCE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
	typedef struct
	{
		uint8_t status;
        uint8_t cli_in_existence;
	}PACKED RSP;

	RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_BLUETOOTH_GET_CIENT_EXISTENCE,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;

	RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);
    if (pEvt)
    {
#ifndef RACE_LPCOMM_ENABLE
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else

#ifdef RACE_AWS_ENABLE
        pEvt->cli_in_existence = race_lpcomm_is_attached(RACE_LPCOMM_DEFAULT_DEVICE_ID);
        ret = RACE_ERRCODE_SUCCESS;
#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_AWS_ENABLE */
#endif /* RACE_LPCOMM_ENABLE */

        pEvt->status = ret;
    }

	return pEvt;
}


void* RACE_BLUETOOTH_IS_AGENT_RIGHT_DEVICE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
	typedef struct
	{
		uint8_t status;
        uint8_t agent_is_right;
	}PACKED RSP;

	RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

	RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_BLUETOOTH_IS_AGENT_RIGHT_DEVICE,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);

    if (pEvt)
    {
        race_device_role_enum role = RACE_DEVICE_ROLE_MAX;

        role = race_get_device_role();
        if (RACE_DEVICE_ROLE_LEFT == role ||
            RACE_DEVICE_ROLE_RIGHT == role)
        {
            pEvt->status = RACE_ERRCODE_SUCCESS;
            pEvt->agent_is_right = (RACE_DEVICE_ROLE_RIGHT == role);
        }
        else
        {
            pEvt->status = RACE_ERRCODE_FAIL;
        }
    }

	return pEvt;
}


void* RACE_BLUETOOTH_GET_BATTERY_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t agent_or_partner;
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    int32_t ret = RACE_ERRCODE_FAIL;
    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_BLUETOOTH_GET_BATTERY,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt)
    {
#ifndef MTK_BATTERY_MANAGEMENT_ENABLE
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else
        if (!pCmd->agent_or_partner)
        {
            /* Agent */
            race_bt_get_battery_level_noti_struct *noti = NULL;
            /* A1. Execute the cmd. */
            uint8_t battery_level = race_get_battery_level();

            /* A2. Create the noti. */
            noti = (void *)RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                                 RACE_TYPE_NOTIFICATION,
                                                 RACE_BLUETOOTH_GET_BATTERY,
                                                 sizeof(race_bt_get_battery_level_noti_struct),
                                                 channel_id);
            if (noti)
            {
                /* A3. Set the noti parameters with the cmd results.  */
                noti->status = RACE_ERRCODE_SUCCESS;
                noti->agent_or_partner = pCmd->agent_or_partner;
                noti->battery_level = battery_level;

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
        else
        {
            /* Client */
#if defined(RACE_LPCOMM_ENABLE) && defined(RACE_AWS_ENABLE)
            uint16_t process_id = race_gen_process_id();

            /* Send the req to the peer */
#ifdef RACE_AWS_ENABLE
            /* C1. Sent the req to the partner and the noti will be created within the Agent's req_hdl.
                        *       ps_noti is not used because there's no parameter needed to be stored for the final noti.
                        */
            ret = race_lpcomm_send_race_cmd_req_to_peer(NULL,
                                               0,
                                               RACE_LPCOMM_ROLE_AGENT,
                                               RACE_BLUETOOTH_GET_BATTERY,
                                               pCmdMsg->hdr.pktId.field.app_id,
                                               channel_id,
                                               process_id,
                                               RACE_LPCOMM_TRANS_METHOD_AWS,
                                               RACE_LPCOMM_DEFAULT_DEVICE_ID);
#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_AWS_ENABLE */

#else
            ret = RACE_ERRCODE_NOT_SUPPORT;
#endif /* RACE_LPCOMM_ENABLE */
        }
#endif /* MTK_BATTERY_MANAGEMENT_ENABLE */
        pEvt->status = ret;
    }

    return pEvt;
}


void* RACE_BLUETOOTH_ROLE_SWITCH_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_BLUETOOTH_DUAL_ROLE_SWITCH,
                                      sizeof(RSP),
                                      channel_id);
    int32_t ret = RACE_ERRCODE_FAIL;
#ifdef RACE_LPCOMM_ENABLE
    race_bt_dual_role_switch_noti_struct *noti = NULL;
    /* process_id must be initialized to 0. */
    uint16_t process_id = 0;
    bool noti_sent = FALSE;
#endif

    if (pEvt != NULL)
    {
#ifndef RACE_LPCOMM_ENABLE
        ret = RACE_ERRCODE_NOT_SUPPORT;
#else

        /* 1. Create Noti */
        ret = race_lpcomm_ps_noti_create((void **)&noti,
                                         &process_id,
                                         RACE_BLUETOOTH_DUAL_ROLE_SWITCH,
                                         pCmdMsg->hdr.pktId.field.app_id,
                                         TRUE,
                                         sizeof(race_bt_dual_role_switch_noti_struct),
                                         channel_id);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail
            ret = RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            goto exit;
        }

        /* 2. Store the const parameters in noti. */
        // No paraemter to store for this cmd

        /* 3. Send the req to the peer */
#ifdef RACE_AWS_ENABLE
        ret = race_lpcomm_send_race_cmd_req_to_peer(NULL,
                                           0,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           RACE_BLUETOOTH_DUAL_ROLE_SWITCH,
                                           pCmdMsg->hdr.pktId.field.app_id,
                                           channel_id,
                                           process_id,
                                           RACE_LPCOMM_TRANS_METHOD_AWS,
                                           RACE_LPCOMM_DEFAULT_DEVICE_ID);

#else
        ret = RACE_ERRCODE_NOT_SUPPORT;
#endif
        if (RACE_ERRCODE_SUCCESS != ret)
        {
            // 5B fail & free noti
            goto exit;
        }

        /* 5. Execute the race cmd */
        // Nothing to do for this cmd


        /* 6. Update noti with race cmd execution result for the local device. */
        // Nothing to do for this cmd

        /* 7. Update noti status and try to send noti
                 *     (Noti will only be sent when both results are obtained.) */
        ret = race_lpcomm_ps_noti_try_send(&noti_sent,
                                           process_id,
                                           channel_id,
                                           ret,
                                           RACE_LPCOMM_ROLE_AGENT,
                                           TRUE);
exit:
#endif /* RACE_LPCOMM_ENABLE */
        pEvt->status = ret;
        if (RACE_ERRCODE_SUCCESS != ret)
        {
#ifdef RACE_LPCOMM_ENABLE
            /* 8. Free noti if needed */
            /* process_id must be initialized to 0 when defining. */
            race_lpcomm_ps_noti_free(process_id);
#endif
        }
        else
        {
#ifdef RACE_FOTA_CMD_ENABLE
            if (race_fota_is_race_fota_running())
            {
                /* In order to avoid FOTA_STOP at the begining of the RHO */
                race_fota_dl_state_set(RACE_FOTA_DOWNLOAD_STATE_RHOING);
            }
#endif
        }
    }

    return pEvt;
}


void* RACE_CmdHandler_BLUETOOTH(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void* ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_BLUETOOTH, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    if (pRaceHeaderCmd->hdr.type == RACE_TYPE_COMMAND ||
        pRaceHeaderCmd->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP)
    {
        switch (pRaceHeaderCmd->hdr.id)
        {
            case RACE_BLUETOOTH_SET_LE_CONNECTION_PARAMETER :
            {
                ptr = RACE_BLUETOOTH_SET_LE_CONNECTION_PARAMETER_HDR(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_BLUETOOTH_GET_LE_LINK_STATUS :
            {
                ptr = RACE_BLUETOOTH_GET_LE_LINK_STATUS_HDR(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_BLUETOOTH_GET_CIENT_EXISTENCE:
            {
                ptr = RACE_BLUETOOTH_GET_CIENT_EXISTENCE_HDR(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_BLUETOOTH_IS_AGENT_RIGHT_DEVICE:
            {
                ptr = RACE_BLUETOOTH_IS_AGENT_RIGHT_DEVICE_HDR(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_BLUETOOTH_GET_BATTERY:
            {
                ptr = RACE_BLUETOOTH_GET_BATTERY_HDR(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_BLUETOOTH_DUAL_ROLE_SWITCH:
            {
                ptr = RACE_BLUETOOTH_ROLE_SWITCH_HDR(pRaceHeaderCmd, channel_id);
            }
            break;
            case RACE_BLUETOOTH_SET_BLE_ADV:
            {
                ptr = RACE_BLUETOOTH_SET_BLE_ADV_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_EDR_INQUERY_PAGE_SCAN:
            {
                ptr = RACE_BLUETOOTH_EDR_INQUERY_PAGE_SCAN_HDR(pRaceHeaderCmd, channel_id);

                break;
            }

            case RACE_BLUETOOTH_DISABLE_WBRSSI:
            {
                ptr = RACE_BLUETOOTH_DISABLE_WBRSSI_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_SET_SNIFF_PARAM:
            {
                ptr = RACE_BLUETOOTH_SET_SNIFF_PARAM_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_SET_RANDOM_ADDR:
            {
                ptr = RACE_BLUETOOTH_SET_RANDOM_ADDR_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_CONN_BLE_DEV:
            {
                ptr = RACE_BLUETOOTH_CONN_BLE_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_ENABLE_SNIFF_MODE:
            {
                ptr = RACE_BLUETOOTH_ENABLE_SNIFF_MODE_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_AWS_RESET:
            {
                ptr = RACE_BLUETOOTH_AWS_RESET_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_GET_BD_ADDR:
            {
                ptr = RACE_BLUETOOTH_GET_BD_ADDR_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_GET_EDR_LINK_KEY:
            {
                ptr = RACE_BLUETOOTH_GET_EDR_LINK_KEY_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_AWS_ENABLE:
            {
                ptr = RACE_BLUETOOTH_AWS_ENABLE_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_GET_A2DP_CODEC_PARAMETERS:
            {
                ptr = RACE_BLUETOOTH_GET_A2DP_CODEC_PARAMETERS_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_SET_LOCAL_ADDR:
            {
                ptr = RACE_BULETOOTH_SET_LOCAL_ADDR_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_GET_EDR_CONNECTED_DEV:
            {
                ptr = RACE_BLUETOOTH_GET_EDR_CONNECTED_DEV_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            case RACE_BLUETOOTH_AWS_GET_ROLE:
            {
                ptr = RACE_BLUETOOTH_AWS_GET_ROLE_HDR(pRaceHeaderCmd, channel_id);
                break;
            }
            default:
            {
                break;
            }
        }
    }

    return ptr;
}


void* RACE_CmdHandler_GET_RSSI(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    DM2L_SetRaceCmdChannel(channel_id);
    DM2L_SendAwsRdRfStatusToLc();
    return NULL;
}


#ifdef MTK_AWS_MCE_ENABLE
static bt_status_t race_bt_get_rssi(race_bt_rssi_type_t type, bt_aws_mce_role_t role)
{
    bt_bd_addr_t* bt_addr = NULL;
    RACE_LOG_MSGID_I("[GET_RSSI], type = %x, role = %x\r\n",2, type, role);
    if (type == RACE_RSSI_PHONE_WITH_DEVICE) { //device with phone
        bt_addr = race_bt_get_sp_bd_addr();//if agent , user sp address
        if (role == BT_AWS_MCE_ROLE_PARTNER) {
            bt_addr = bt_connection_manager_device_local_info_get_peer_aws_address();//if partner , use agent address
        }
    } else if (type == RACE_RSSI_AGENT_WITH_PARTNER) {//agent with partner£¬ only for agent
        bt_addr  = bt_connection_manager_device_local_info_get_local_address();
    } else if (type == RACE_RSSI_PARTNER_WITH_AGENT) {//partner with agent£¬only for partner
        bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
        bt_sink_srv_profile_type_t pro_type = bt_sink_srv_cm_get_connected_profiles(local_addr);
        RACE_LOG_MSGID_I("[GET_RSSI], type = %x\r\n", 1, pro_type);
        if (pro_type == BT_SINK_SRV_PROFILE_NONE) /*ÓëphoneÎÞÁ¬½Ó*/
        {
            bt_addr = bt_connection_manager_device_local_info_get_peer_aws_address();
        } else { /*Óëphone ÓÐÁ¬½Ó*/
            bt_addr = local_addr;
        }
    }

    bt_gap_connection_handle_t handle = bt_sink_srv_cm_get_gap_handle(bt_addr);
    RACE_LOG_MSGID_I("[GET_RSSI], type = %x, bt_addr = %x\r\n",1,bt_addr[0]);
    bt_status_t  status = bt_gap_read_raw_rssi(handle);
    if (status == BT_STATUS_SUCCESS) {
        g_race_fcd_cntx.type = type;
    }
    RACE_LOG_MSGID_I("[GET_RSSI], status = %x, handle = %x, cntx = %x\r\n",2, status,handle, g_race_fcd_cntx.type);
    return status;
}
#endif

bt_status_t race_bt_set_rssi(void *buff)
{
    bt_status_t status = BT_STATUS_FAIL;
    const bt_gap_read_rssi_cnf_t *raw_rssi = (bt_gap_read_rssi_cnf_t *)buff;
    if (raw_rssi == NULL) {
        return status;
    }
    RACE_LOG_MSGID_I("[SET_RSSI]raw rssi=%ddBm, handle= %x ", 2, raw_rssi->rssi, raw_rssi->handle);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    RACE_LOG_MSGID_I("[SET_RSSI]role = %x, type = %x ", 2, role, g_race_fcd_cntx.type);
    if (role == BT_AWS_MCE_ROLE_PARTNER) {
        if (g_race_fcd_cntx.type == RACE_RSSI_PHONE_WITH_DEVICE) {
            g_race_fcd_cntx.rssi_info.AgRssi = raw_rssi->rssi;
            status = race_bt_get_rssi(RACE_RSSI_PARTNER_WITH_AGENT, role);
            RACE_LOG_MSGID_I("Partner:status", 0, status);
        } else if (g_race_fcd_cntx.type == RACE_RSSI_PARTNER_WITH_AGENT) {
            g_race_fcd_cntx.rssi_info.AgentPartnerRssi = raw_rssi->rssi;
            race_bt_send_notify_rssi_msg();
        }
    } else if (role == BT_AWS_MCE_ROLE_AGENT) {
        bt_aws_mce_agent_state_type_t state;

        if (g_race_fcd_cntx.type == RACE_RSSI_PHONE_WITH_DEVICE) /*only for phone RSSI & agent*/
        {
            g_race_fcd_cntx.rssi_info.AgRssi = raw_rssi->rssi;
            state = bt_sink_srv_cm_get_aws_link_state();//AWS connected
            if (state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                status = race_bt_get_rssi(RACE_RSSI_AGENT_WITH_PARTNER, role);
            } else {
                race_bt_send_notify_rssi_msg();
            }

            RACE_LOG_MSGID_I("[SET_RSSI]GET_AGENT_PARTNER_RSSI ", 0);
        } else if (g_race_fcd_cntx.type == RACE_RSSI_AGENT_WITH_PARTNER) {

            RACE_LOG_MSGID_I("[SET_RSSI]START notify rssi... ", 0);
            g_race_fcd_cntx.rssi_info.AgentPartnerRssi = raw_rssi->rssi;
            race_bt_send_notify_rssi_msg();
        }
    }
#else
    g_race_fcd_cntx.rssi_info.AgRssi = raw_rssi->rssi;

    race_bt_send_notify_rssi_msg();
#endif
    return status;
}

static RACE_ERRCODE race_bt_send_notify_rssi_msg(void)
{
    race_general_msg_t msg_queue_item = {0};
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_GET_RSSI_CMD;
    msg_queue_item.msg_data = NULL;
    ret = race_send_msg(&msg_queue_item);
    RACE_LOG_MSGID_E("[RSSI]send_notify_rssi_msg ret:%x.",1, ret);
    return ret;
}

bt_status_t race_bt_notify_rssi(void)
{
    bt_status_t ret = RACE_ERRCODE_SUCCESS;
    typedef struct {
        uint8_t status;
        race_bt_rssi_info_t info;
    }PACKED RSP;

    uint8_t channel_id = g_race_fcd_cntx.channel;
#ifdef MTK_AWS_MCE_ENABLE

    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
#endif
    RSP *noti = RACE_ClaimPacket(RACE_TYPE_NOTIFICATION,
                        RACE_BLUETOOTH_GET_RSSI,
                        sizeof(RSP),
                        channel_id);

    RACE_LOG_MSGID_I("[notify_rssi]: channel = %x, Noti = %x\r\n",2, channel_id, noti);
    if (noti) {
        noti->status = RACE_ERRCODE_SUCCESS;
        memcpy(&(noti->info), &(g_race_fcd_cntx.rssi_info), sizeof(race_bt_rssi_info_t));

        RACE_LOG_MSGID_I("[notify_rssi]: agentpatenerssi = %x, AgRssi = %x *\r\n",2, noti->info.AgentPartnerRssi,
            noti->info.AgRssi);
#ifdef MTK_AWS_MCE_ENABLE
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            ret = race_noti_send(noti, channel_id, FALSE);
            RACE_LOG_MSGID_I("[notify_rssi]: ret = %x\r\n",1, ret);
            if (RACE_ERRCODE_SUCCESS != ret)
            {
              /* A5. Free the noti if needed. */
              RACE_FreePacket(noti);
              noti = NULL;
            }

        }  else if (role == BT_AWS_MCE_ROLE_PARTNER) {
           race_send_pkt_t *pEvt = (void *)race_pointer_cnv_pkt_to_send_pkt(noti);

            if (pEvt) {
#ifdef RACE_RELAY_CMD_ENABLE
                ret = bt_send_aws_mce_race_cmd_data(&pEvt->race_data, pEvt->length, channel_id, RACE_CMD_RSP_FROM_PARTNER, 0);
#endif
                RACE_LOG_MSGID_I("[notify_rssi] ret = %x \n", 1, ret);
                if (ret != BT_STATUS_SUCCESS) {
                    RACE_LOG_MSGID_I("[notify_rssi] partner send relay req FAIL \n", 0);
                }
                RACE_FreePacket(noti);
                noti = NULL;
            }
        }
#else
        ret = race_noti_send(noti, channel_id, FALSE);
        RACE_LOG_MSGID_I("[notify_rssi]: ret = %x\r\n",1, ret);
        if (RACE_ERRCODE_SUCCESS != ret)
        {
          /* A5. Free the noti if needed. */
          RACE_FreePacket(noti);
          noti = NULL;
        }
#endif
    }

    return ret;
}


static void* RACE_BLUETOOTH_SET_BLE_ADV_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t is_enable;
        uint16_t min_advertise_interval;
        uint16_t max_advertise_interval;
        uint8_t adv_type;
        uint8_t own_addr_type;
        uint8_t advertise_length;
        uint8_t advertise_data[31];
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    bt_status_t ret = BT_STATUS_SUCCESS;
    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_SET_BLE_ADV,
                              (uint16_t)sizeof(RSP),
                              channel_id);
    if (pEvt)
    {
        bt_hci_cmd_le_set_advertising_enable_t le_adv_enable;
        bt_hci_cmd_le_set_advertising_parameters_t le_adv_para = {
            .advertising_interval_min = pCmd->min_advertise_interval,
            .advertising_interval_max = pCmd->max_advertise_interval,
            .advertising_type = pCmd->adv_type,
            .own_address_type         = pCmd->own_addr_type,
            .advertising_channel_map = 7,
            .advertising_filter_policy = 1
        };

        bt_hci_cmd_le_set_advertising_data_t le_adv_data = {0};
        le_adv_data.advertising_data_length =
            pCmd->advertise_length > sizeof(le_adv_data.advertising_data) ? sizeof(le_adv_data.advertising_data) : pCmd->advertise_length;
        memcpy(le_adv_data.advertising_data, pCmd->advertise_data, le_adv_data.advertising_data_length);

        if(!(pCmd->is_enable)) {
            le_adv_enable.advertising_enable = BT_HCI_DISABLE;
            ret = bt_gap_le_set_advertising(&le_adv_enable, NULL, NULL, NULL);
        } else {
            le_adv_enable.advertising_enable = BT_HCI_ENABLE;
            ret = bt_gap_le_set_advertising(&le_adv_enable, &le_adv_para, &le_adv_data, NULL);
        }

        pEvt->status = RACE_ERRCODE_SUCCESS;
    }

    RACE_LOG_MSGID_I("[low_power_test][set_ble_adv]is_enable:0x%02x, min_interval:0x%04x, max_interval:0x%04x, adv_type:%02x, addr_type:0x%02x, adv_len:0x%02x, ret:0x%08x\r\n", 7,
        pCmd->is_enable, pCmd->min_advertise_interval, pCmd->max_advertise_interval, pCmd->adv_type, pCmd->own_addr_type, pCmd->advertise_length, ret);
    /* RACE_LOG_MSGID_I("[low_power_test][set_ble_adv]data[0]:0x%02x, data[%d]:0x%02x, ret:0x%08x, pEvt:0x%08x", 5, pCmd->advertise_data[0], (pCmd->advertise_length-1),
        pCmd->advertise_data[(pCmd->advertise_length-1)], ret, pEvt); */
    return pEvt;
}


static void* RACE_BLUETOOTH_EDR_INQUERY_PAGE_SCAN_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t mode;
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_EDR_INQUERY_PAGE_SCAN,
                              (uint16_t)sizeof(RSP),
                              channel_id);

    if (pEvt)
    {
        bt_connection_manager_write_scan_enable_mode(pCmd->mode);
        pEvt->status = RACE_ERRCODE_SUCCESS;
    }
    RACE_LOG_MSGID_I("[low_power_test][set_inquiry_page_scan]mode:0x%02x, pEvt:0x%08x", 2, pCmd->mode, pEvt);
    return pEvt;
}


static void* RACE_BLUETOOTH_DISABLE_WBRSSI_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_DISABLE_WBRSSI,
                              (uint16_t)sizeof(RSP),
                              channel_id);

    if (pEvt)
    {
        uint8_t raw_data[] = {0x4f, 0xfc, 0x00};
        bt_hci_cmd_raw_data_send(raw_data);
        pEvt->status = RACE_ERRCODE_SUCCESS;
    }
    RACE_LOG_MSGID_I("[low_power_test][Disable_WBRSSI]pEvt:0x%08x", 1, pEvt);
    return pEvt;


}

static void* RACE_BLUETOOTH_SET_SNIFF_PARAM_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint16_t max_sniff_interval;
        uint16_t min_sniff_interval;
        uint16_t sniff_attempt;
        uint16_t sniff_timeout;
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;

    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_SET_SNIFF_PARAM,
                              (uint16_t)sizeof(RSP),
                              channel_id);

    if (pEvt)
    {
        uint8_t set_sniff_raw_data[13];
        uint16_t *connection_handle = (uint16_t *)(set_sniff_raw_data + 3);
        uint16_t *max_sniff_interval = (uint16_t *)(set_sniff_raw_data + 5);
        uint16_t *min_sniff_interval = (uint16_t *)(set_sniff_raw_data + 7);
        uint16_t *sniff_attempt = (uint16_t *)(set_sniff_raw_data +9);
        uint16_t *sniff_timeout = (uint16_t *)(set_sniff_raw_data +11);
        *connection_handle = 0x0032;
        *max_sniff_interval = pCmd->max_sniff_interval;
        *min_sniff_interval = pCmd->min_sniff_interval;
        *sniff_attempt = pCmd->sniff_attempt;
        *sniff_timeout = pCmd->sniff_timeout;
        set_sniff_raw_data[0] = 0x03;
        set_sniff_raw_data[1] = 0x08;
        set_sniff_raw_data[2] = 0x0a;
        bt_hci_cmd_raw_data_send(set_sniff_raw_data);
        *connection_handle = 0x0033;
        bt_hci_cmd_raw_data_send(set_sniff_raw_data);
        *connection_handle = 0x0034;
        bt_hci_cmd_raw_data_send(set_sniff_raw_data);
        pEvt->status = RACE_ERRCODE_SUCCESS;
    }
    RACE_LOG_MSGID_I("[low_power_test][set_sniff_param]max_interval:0x%04x, min_interval:0x%04x, attempt:0x%04x, timeout0x%04x, pEvt:0x%08x", 5,
        pCmd->max_sniff_interval, pCmd->min_sniff_interval, pCmd->sniff_attempt, pCmd->sniff_timeout, pEvt);
    return pEvt;
}


static void* RACE_BLUETOOTH_SET_RANDOM_ADDR_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t addr[6];
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    bt_status_t ret = BT_STATUS_SUCCESS;
    CMD *pCmd = (CMD *)pCmdMsg;

    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_SET_RANDOM_ADDR,
                              (uint16_t)sizeof(RSP),
                              channel_id);

    if (pEvt)
    {
        ret = bt_gap_le_set_random_address((bt_bd_addr_ptr_t)&pCmd->addr);
        pEvt->status = RACE_ERRCODE_SUCCESS;
    }
    RACE_LOG_MSGID_I("[low_power_test][set_random_addr]addr[0]:0x%02x, addr[1]:0x%02x, addr[2]:0x%02x, addr[3]:0x%02x,addr[4]:0x%02x, addr[5]:0x%02x, ret:0x%08x, pEvt:0x%08x",
    8, pCmd->addr[0], pCmd->addr[1], pCmd->addr[2], pCmd->addr[3], pCmd->addr[4], pCmd->addr[5], ret, pEvt);
    return pEvt;

}

static void* RACE_BLUETOOTH_CONN_BLE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t addr[6];
        uint8_t addr_type;
        uint16_t min_connection_interval;
        uint16_t max_connection_interval;
        uint16_t connection_latency;
        uint16_t supervision_timeout;
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    bt_status_t ret = BT_STATUS_SUCCESS;
    CMD *pCmd = (CMD *)pCmdMsg;

    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_CONN_BLE_DEV,
                              (uint16_t)sizeof(RSP),
                              channel_id);
    RACE_LOG_MSGID_I("[low_power_test][conn_ble]addr[0]:0x%02x, addr[1]:0x%02x, addr[2]:0x%02x, addr[3]:0x%02x,addr[4]:0x%02x, addr[5]:0x%02x",
    6, pCmd->addr[0], pCmd->addr[1], pCmd->addr[2], pCmd->addr[3], pCmd->addr[4], pCmd->addr[5]);

    if (pEvt)
    {
        bt_hci_cmd_le_create_connection_t conn_param;
        conn_param.le_scan_interval = 0x10;
        conn_param.le_scan_window = 0x10;
        conn_param.initiator_filter_policy = BT_HCI_CONN_FILTER_ASSIGNED_ADDRESS;
        conn_param.own_address_type = BT_ADDR_PUBLIC;
        conn_param.minimum_ce_length = 0x0000;
        conn_param.maximum_ce_length = 0x0190;
        conn_param.peer_address.type = pCmd->addr_type;
        memcpy(&(conn_param.peer_address.addr), pCmd->addr, sizeof(bt_bd_addr_t));
        conn_param.conn_interval_min = pCmd->min_connection_interval;
        conn_param.conn_interval_max = pCmd->max_connection_interval;
        conn_param.conn_latency = pCmd->connection_latency;
        conn_param.supervision_timeout = pCmd->supervision_timeout;
        ret = bt_gap_le_connect(&conn_param);
        pEvt->status = RACE_ERRCODE_SUCCESS;
    }
    RACE_LOG_MSGID_I("[low_power_test][conn_ble]addr_type:0x%02x, min_conn_interval:0x%04x, max_conn_interval:0x%04x, conn_latency:0x%04x, timeout:0x%04x, ret:0x%08x, pEvt:0x%08x",
        7, pCmd->addr_type, pCmd->min_connection_interval, pCmd->max_connection_interval, pCmd->connection_latency, pCmd->supervision_timeout, ret, pEvt);
    return pEvt;
}

static void* RACE_BLUETOOTH_ENABLE_SNIFF_MODE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t addr[6];
        uint8_t enable;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    bt_status_t ret = BT_STATUS_SUCCESS;
    CMD *pCmd = (CMD *)pCmdMsg;

    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_ENABLE_SNIFF_MODE,
                              (uint16_t)sizeof(RSP),
                              channel_id);
    bt_gap_connection_handle_t conn_handle = bt_sink_srv_cm_get_gap_handle((void*)(pCmd->addr));
    RACE_LOG_MSGID_I("[enable sniff mode][I] Addr[0]:0x%02x, addr[1]:0x%02x, addr[2]:0x%02x, addr[3]:0x%02x,addr[4]:0x%02x, addr[5]:0x%02x handle:0x%x pEvt:0x%x",
        8, pCmd->addr[0], pCmd->addr[1], pCmd->addr[2], pCmd->addr[3], pCmd->addr[4], pCmd->addr[5], conn_handle, pEvt);

    if (NULL == pEvt) {
        RACE_LOG_MSGID_I("[enable sniff mode][E] Allocate evt packet fail", 0);
        return NULL;
    }
    if (0 != conn_handle) {
        bt_gap_link_policy_setting_t setting;
        if (0x00 == pCmd->enable) {
            setting.sniff_mode = BT_GAP_LINK_POLICY_DISABLE;
            ret = bt_gap_exit_sniff_mode(conn_handle);
            RACE_LOG_MSGID_I("[enable sniff mode][I] Exit sniff mode ret 0x%x", 1, ret);
        } else {
            setting.sniff_mode = BT_GAP_LINK_POLICY_ENABLE;
        }
        ret = bt_gap_write_link_policy(conn_handle, &setting);
        RACE_LOG_MSGID_I("[enable sniff mode][I] Enable sniff mode ret 0x%x", 1, ret);
        pEvt->status = RACE_ERRCODE_SUCCESS;
    } else {
        RACE_LOG_MSGID_I("[enable sniff mode][E] Can't find connection by address", 0);
        pEvt->status = RACE_ERRCODE_FAIL;
    }
    return pEvt;
}

static void* RACE_BLUETOOTH_AWS_RESET_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t flag;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;

    RACE_LOG_MSGID_I("[aws reset][I] Aws reset handler data length %d", 1, pCmd->cmdhdr.length);
    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_AWS_RESET,
                              (uint16_t)sizeof(RSP),
                              channel_id);
    if (NULL == pEvt) {
        RACE_LOG_MSGID_E("[AWS reset][E] Allocate evt packet fail", 0);
        return NULL;
    }
    if (pCmd->cmdhdr.length == 2 || (pCmd->flag == 0 || pCmd->flag == 1)) {
        bt_device_manager_remote_delete_info(NULL, 0);
    }
    if (pCmd->cmdhdr.length == 2 || (pCmd->flag == 0 || pCmd->flag == 2)) {
    #ifdef MTK_AWS_MCE_ENABLE
        bt_device_manager_aws_reset();
    #endif
    }
    pEvt->status = RACE_ERRCODE_SUCCESS;
    return pEvt;
}

static void* RACE_BLUETOOTH_GET_BD_ADDR_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t agent_or_partner;
    } PACKED CMD;

    typedef struct {
        uint8_t status;
        uint8_t agent_or_partner;
        bt_bd_addr_t bdAddr;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_BLUETOOTH_GET_BD_ADDR,
                                      sizeof(RSP),
                                      channel_id);
    if (NULL == pEvt) {
        RACE_LOG_MSGID_I("[get bd addr][E] Allocate evt packet fail", 0);
        return NULL;
    }
    CMD *pCmd = (CMD *)pCmdMsg;
    bt_bd_addr_t *addr = bt_device_manager_get_local_address();
#ifdef MTK_AWS_MCE_ENABLE
    if (!pCmd->agent_or_partner
        && (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()
            || BT_AWS_MCE_ROLE_CLINET == bt_device_manager_aws_local_info_get_role())) {
        addr = bt_device_manager_aws_local_info_get_peer_address();
    } else if (pCmd->agent_or_partner
        && BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
        addr = bt_device_manager_aws_local_info_get_peer_address();
    }
#endif
    pEvt->agent_or_partner = pCmd->agent_or_partner;
    if (NULL != addr) {
        memcpy(&(pEvt->bdAddr), addr, BT_BD_ADDR_LEN);
        pEvt->status = RACE_ERRCODE_SUCCESS;
    } else {
        pEvt->status = RACE_ERRCODE_FAIL;
    }
    return pEvt;
}

static void* RACE_BLUETOOTH_GET_EDR_LINK_KEY_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t     key_type;
        uint8_t     addr[6];
        uint8_t     key[16];
    } PACKED Key_Item;

    typedef struct {
        uint8_t     status;
        uint8_t     invalid_num;
        Key_Item    key[1];
    } PACKED RSP;

    uint32_t edr_num = BT_DEVICE_MANAGER_MAX_PAIR_NUM;
    bt_device_manager_paired_infomation_t edr_remote[BT_DEVICE_MANAGER_MAX_PAIR_NUM];
    memset(&edr_remote, 0, sizeof(edr_remote));
    bt_device_manager_remote_get_paired_list(edr_remote, &edr_num);

    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_GET_EDR_LINK_KEY,
                              (uint16_t)(sizeof(RSP) - sizeof(Key_Item) + (edr_num * sizeof(Key_Item))),
                              channel_id);
    if (NULL == pEvt) {
        RACE_LOG_MSGID_I("[get edr link key][E] Allocate evt packet fail", 0);
        return NULL;
    }
    pEvt->status = RACE_ERRCODE_SUCCESS;
    pEvt->invalid_num = edr_num;
    RACE_LOG_MSGID_I("[get edr link key][I] key num %d", 1, edr_num);

    if (edr_num) {
        bt_device_manager_db_remote_paired_info_t paired_info;
        for (uint32_t i = 0; i < edr_num; i++) {
            pEvt->key[i].key_type = 0;
            bt_device_manager_remote_find_paired_info((void *)(edr_remote[i].address), &paired_info);
            memcpy((void*)(pEvt->key[i].key), (void*)(paired_info.paired_key.key), 16);
        }
    }
    return pEvt;
}

static void* RACE_BLUETOOTH_AWS_ENABLE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_BLUETOOTH_AWS_ENABLE,
                                      sizeof(RSP),
                                      channel_id);
    if (NULL == pEvt) {
        RACE_LOG_MSGID_I("[aws enable][E] Allocate evt packet fail", 0);
        return NULL;
    }
#ifdef MTK_AWS_MCE_ENABLE
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t enable;
    } PACKED CMD;

    CMD *pCmd = (CMD *)pCmdMsg;
    if (pCmd->enable) {
        bt_sink_srv_cm_ls_enable(true);
    } else {
        bt_sink_srv_cm_ls_enable(false);
    }
    pEvt->status = RACE_ERRCODE_SUCCESS;
#else
    pEvt->status = RACE_ERRCODE_FAIL;
#endif
    return pEvt;
}

static void* RACE_BLUETOOTH_GET_A2DP_CODEC_PARAMETERS_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    bt_sink_srv_a2dp_basic_config_t* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_GET_A2DP_CODEC_PARAMETERS,
                              (uint16_t)sizeof(bt_sink_srv_a2dp_basic_config_t),
                              channel_id);
    bt_sink_srv_a2dp_get_codec_parameters(pEvt);
    
    return pEvt;
}
bt_status_t bt_race_set_local_addr_callback(bt_device_manager_power_reset_progress_t type, void *user_data)
{
    if (BT_DEVICE_MANAGER_POWER_RESET_PROGRESS_MEDIUM == type) {
        typedef struct {
            uint8_t status;
        }PACKED RSP;
        RACE_LOG_MSGID_I("set local addr callback user data %d", 1, (uint32_t)user_data);
        bt_set_local_public_address((bt_bd_addr_ptr_t)set_local_addr);
        bt_device_manager_store_local_address(&set_local_addr);
        uint8_t app_id = (uint32_t)user_data & 0xFF;
        uint8_t channel_id = (((uint32_t)user_data) >> 8) & 0xFF;
        RSP *pEvt = RACE_ClaimPacketAppID(app_id, RACE_TYPE_NOTIFICATION, RACE_BLUETOOTH_SET_LOCAL_ADDR, sizeof(RSP),channel_id);
        if (pEvt) {
            pEvt->status = RACE_ERRCODE_SUCCESS;
            race_flush_packet((uint8_t *)pEvt, channel_id);
        }
    } else if (BT_DEVICE_MANAGER_POWER_RESET_PROGRESS_COMPLETE == type) {
        
    }
    return BT_STATUS_SUCCESS;
}

static void* RACE_BULETOOTH_SET_LOCAL_ADDR_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t addr[6];
    }PACKED CMD;

    typedef struct
    {
        uint8_t status;
    }PACKED RSP;

    bt_status_t ret = BT_STATUS_SUCCESS;
    CMD *pCmd = (CMD *)pCmdMsg;
    memcpy(&set_local_addr, &pCmd->addr, sizeof(bt_bd_addr_t));
    RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_RESPONSE,
                              (uint16_t)RACE_BLUETOOTH_SET_LOCAL_ADDR,
                              (uint16_t)sizeof(RSP),
                              channel_id);

    if (pEvt) {
         pEvt->status = RACE_ERRCODE_SUCCESS;
         if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
            bt_set_local_public_address((bt_bd_addr_ptr_t)pCmd->addr);
            bt_device_manager_store_local_address(&pCmd->addr);
            RSP* pEvt_notiy = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                              (uint8_t)RACE_TYPE_NOTIFICATION,
                              (uint16_t)RACE_BLUETOOTH_SET_LOCAL_ADDR,
                              (uint16_t)sizeof(RSP),
                              channel_id);
            if(NULL == pEvt_notiy){
                RACE_LOG_MSGID_I("[RACE_BULETOOTH_SET_LOCAL_ADDR_HDR[E] Allocate pEvt_notiy packet fail", 0);
                return pEvt;
            }
            pEvt_notiy->status = RACE_ERRCODE_SUCCESS;
            ret = race_flush_packet((uint8_t *)pEvt_notiy, channel_id);
         } else {
            bt_dmgr_report_id("[BT_DM][I] RESET BY RACE", 0);
            bt_dmgr_report_id("[BT_CM][I] RACE SET LOCAL addr:%02x:%02x:%02x:%02x:%02x:%02x, profile mask:0x%x", 6,
                    (pCmd->addr)[5], (pCmd->addr)[4], (pCmd->addr)[3],
                    (pCmd->addr)[2], (pCmd->addr)[1], (pCmd->addr)[0]);
            uint32_t user_data = (pCmdMsg->hdr.pktId.field.app_id) | (channel_id << 8);
            bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_NORMAL, bt_race_set_local_addr_callback, (void *)user_data);
            pEvt->status = RACE_ERRCODE_SUCCESS;
         }
    }
    RACE_LOG_MSGID_I("[set_local_addr]addr[0]:0x%02x, addr[1]:0x%02x, addr[2]:0x%02x, addr[3]:0x%02x,addr[4]:0x%02x, addr[5]:0x%02x, ret:0x%08x, pEvt:0x%08x",
    8, pCmd->addr[0], pCmd->addr[1], pCmd->addr[2], pCmd->addr[3], pCmd->addr[4], pCmd->addr[5], ret, pEvt);
    return pEvt;
}

static void* RACE_BLUETOOTH_GET_EDR_CONNECTED_DEV_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint32_t profile_mask;
    } PACKED CMD;

    typedef struct {
        uint8_t         status;
        uint8_t         valid_num;
        bt_bd_addr_t    address[1];
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
#ifdef MTK_BT_CM_SUPPORT
	uint32_t connected_num = bt_cm_get_connected_devices(pCmd->profile_mask, NULL, 0);
#else
    uint32_t connected_num = 0;
#endif
    uint32_t rsp_size = sizeof(RSP) - sizeof(bt_bd_addr_t) + (connected_num * sizeof(bt_bd_addr_t));

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_BLUETOOTH_GET_EDR_CONNECTED_DEV,
                                      rsp_size,
                                      channel_id);
    if (NULL == pEvt) {
        RACE_LOG_MSGID_I("[get connected dev][E] Allocate evt packet fail", 0);
        return NULL;
    }
    pEvt->status = RACE_ERRCODE_SUCCESS;
    pEvt->valid_num = connected_num;
    RACE_LOG_MSGID_I("[get connected dev][I] key num %d", 1, connected_num);

#ifdef MTK_BT_CM_SUPPORT
    if (connected_num) {
        bt_cm_get_connected_devices(pCmd->profile_mask, &(pEvt->address[0]), connected_num);
    }
#endif
    return pEvt;
}

static void* RACE_BLUETOOTH_AWS_GET_ROLE_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        uint8_t status;
        bt_aws_mce_role_t role;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_BLUETOOTH_AWS_GET_ROLE,
                                      sizeof(RSP),
                                      channel_id);
    if (NULL == pEvt) {
        RACE_LOG_MSGID_I("[get bd addr][E] Allocate evt packet fail", 0);
        return NULL;
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    pEvt->role = role;
    pEvt->status = RACE_ERRCODE_SUCCESS;
#else
    pEvt->status = RACE_ERRCODE_FAIL;
#endif
    return pEvt;

}

#endif /* RACE_BT_CMD_ENABLE */

