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
#include "race_cmd_fcd_cmd.h"
#include "stdio.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_bt.h"
#include "race_timer.h"
#include "race_cmd.h"
#include "race_event.h"
#include "FreeRTOS.h"

#ifdef RACE_RELAY_CMD_ENABLE
#include "race_cmd_relay_cmd.h"
#endif

#ifdef RACE_BT_CMD_ENABLE

race_bt_fcd_get_rssi_t g_race_fcd_cntx = {
    .type = 0,
    .rssi_info = {0},
    .rssi_hdl =0,
    .channel = 0,
    };


static void* RACE_CmdHandler_factory_reset(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t reset_id;
    } PACKED CMD;
    CMD *cmd = (CMD *)pCmdMsg;

    RSP *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                    RACE_FCD_FACTORY_RESET_CMD,
                                    sizeof(RSP),
                                    channel_id);
    if (pEvt) {
        switch(cmd->reset_id)
        {
            case FCD_RESET_PAIRDEVICE: {
                //pEvt->status = bt_sink_srv_action_send(BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST, NULL);
                break;
            }
            case FCD_RESET_VPLANG: {
                break;
            }
            case FCD_RESET_DEFAULT_NVKEY: {
                break;
            }
            case FCD_RESET_MCE_SETTING: {
                break;
            }
            case FCD_RESET_DEVICE: {
                break;

            }
        }

        //pEvt->status = TO do clean nvdm
    }
    return pEvt;
}

static void RACE_CmdHandler_sw_reset(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    /*software reset*/
    hal_wdt_status_t ret;
    hal_wdt_config_t wdt_config;

    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = 1;
    hal_wdt_init(&wdt_config);
    ret = hal_wdt_software_reset();
    RACE_LOG_MSGID_I("fota_device_reboot() ret:%d",1, ret);
}

static void* RACE_CmdHandler_sync_info(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role;
#endif

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        bt_bd_addr_t local_addr;
        bt_bd_addr_t peer_addr;
    } PACKED CMD;

    typedef struct
    {
        uint8_t status;
    } PACKED RSP;


    CMD *pCmd = (CMD *)pCmdMsg;

    if (pCmd == NULL) {
        return pCmd;
    }
    RSP *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE,
                                 pCmd->cmdhdr.id,
                                 sizeof(RSP),
                                 channel_id);
#ifdef MTK_AWS_MCE_ENABLE
    if (pCmd->cmdhdr.id == RACE_FCD_IMPORT_MCS_SYNC_AGENT_INFO) {
        aws_role = BT_AWS_MCE_ROLE_AGENT;
    } else {
        aws_role = BT_AWS_MCE_ROLE_PARTNER;
    }
#endif

    if (pEvt != NULL) {
        bt_connection_manager_device_local_info_store_local_address(&(pCmd->local_addr));
#ifdef MTK_AWS_MCE_ENABLE
        bt_connection_manager_device_local_info_store_aws_role(aws_role);
        bt_connection_manager_device_local_info_store_peer_aws_address(&(pCmd->peer_addr));
#endif
    }
    return pEvt;
}

void* RACE_CmdHandler_FCD(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_CmdHandler_FCD, type[0x%X], id[0x%X]", 2, pCmdMsg->hdr.type, pCmdMsg->hdr.id);

    if (pCmdMsg->hdr.type == RACE_TYPE_COMMAND ||
        pCmdMsg->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP)
    {

        switch (pCmdMsg->hdr.id)
        {
            case RACE_FCD_FACTORY_RESET_CMD:
               return  RACE_CmdHandler_factory_reset(pCmdMsg, length, channel_id);
            case RACE_FCD_SW_RESET_CMD:
            {
                RACE_CmdHandler_sw_reset(pCmdMsg, length, channel_id);
                break;
            }
            case RACE_FCD_IMPORT_MCS_SYNC_AGENT_INFO:
            case RACE_FCD_IMPORT_MCS_SYNC_PARTNER_INFO:
            {
                return RACE_CmdHandler_sync_info(pCmdMsg, length, channel_id);
            }
            case RACE_BLUETOOTH_GET_RSSI:
                return RACE_CmdHandler_GET_RSSI(pCmdMsg, length, channel_id);
            default:
                break;
        }
    }
    return NULL;
}


#endif /* RACE_FCD_CMD_ENABLE */

