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
#ifdef RACE_BOOTREASON_CMD_ENABLE
#include "race_cmd.h"
#include "race_cmd_bootreason.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_msg_struct.h"
#include "race_lpcomm_conn.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"
#include "bootreason_check.h"
#include "hal_nvic.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void* race_cmdhdl_bootreason_get_reasoninfo(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t  status;
        uint32_t reason;
    } PACKED RSP;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_BOOTREASON_GET_REASONINFO,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt)
    {
        pEvt->reason = 0;
        bootreason_get_reason((bootreason_reason_t *)&pEvt->reason);
        pEvt->status = RACE_ERRCODE_SUCCESS;
    }

    return (void*)pEvt;
}

void* race_cmdhdl_bootreason_get_exceptioninfo(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t  status;
        uint32_t data_length;
        uint32_t data_address;
    } PACKED RSP;

    bootreason_info_t bootreason;
    bootreason_status_t ret;

    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                      RACE_TYPE_RESPONSE,
                                      RACE_BOOTREASON_GET_EXCEPTIONINFO,
                                      sizeof(RSP),
                                      channel_id);
    if (pEvt)
    {
        ret = bootreason_get_info(&bootreason);
        if(ret == BOOTREASON_STATUS_OK)
        {
            pEvt->status        = RACE_ERRCODE_SUCCESS;
            pEvt->data_address  = (uint32_t)bootreason.custom.data;
            pEvt->data_length   = bootreason.custom.len;
        }
        else
        {
            pEvt->status        = RACE_ERRCODE_FAIL;
            pEvt->data_address  = 0;
            pEvt->data_length   = 0;
        }
    }

    return (void*)pEvt;
}

void* RACE_CmdHandler_bootreason(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_CmdHandler_bootreason, pCmdMsg->hdr.id[0x%X]",1, (int)pCmdMsg->hdr.id);

    switch (pCmdMsg->hdr.id)
    {
             case RACE_WDT_TIMEOUT_TEST:
             RACE_LOG_MSGID_I("WDT Timeout Test Start...\r\n",0);
             uint32_t mask;
             hal_nvic_save_and_set_interrupt_mask(&mask);
             while(1);

        case RACE_BOOTREASON_GET_REASONINFO:
            return race_cmdhdl_bootreason_get_reasoninfo((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

        case RACE_BOOTREASON_GET_EXCEPTIONINFO:
            return race_cmdhdl_bootreason_get_exceptioninfo((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);

        default:
            break;
    }

    return NULL;
}

#endif /* RACE_BOOTREASON_CMD_ENABLE */
