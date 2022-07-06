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
#include "race_cmd.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_msg_struct.h"
#include "race_lpcomm_conn.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"

#include "race_cmd_version_code.h"

#ifdef RACE_VERSION_CODE_CMD_ENABLE

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
extern uint32_t version_hw_code_get();
extern void uid_code_get(uint8_t* p_data);
extern uint32_t sub_chip_version_get();

void* race_cmdhdl_version_code_get(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("Race version code race_cmdhdl_version_code_get()!!!",0);     
    typedef struct {
        uint8_t  status;
        uint16_t version_code;
    } PACKED RSP;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_RESPONSE,
                                        RACE_CMD_VERSION_CODE_GET,
                                        sizeof(RSP),
                                        channel_id);
    if (pEvt) {
        pEvt->version_code = version_hw_code_get();
        pEvt->status = 0;
        RACE_LOG_MSGID_I("Race Version code: 0x%x", 1, pEvt->version_code);
    }
    else
    {
        RACE_LOG_MSGID_I("Race version code error!!!",0);
    }
    return (void*)pEvt;
}
void* race_cmdhdl_uid_get(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    uint32_t i;
    RACE_LOG_MSGID_I("Race version code race_cmdhdl_uid_get()!!!",0);  

    typedef struct {
        uint8_t  status;
        uint8_t uid[16];
    } PACKED RSP;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                        RACE_TYPE_RESPONSE,
                                        RACE_CMD_UID_GET,
                                        sizeof(RSP),
                                        channel_id);
    if (pEvt) {
        uid_code_get(pEvt->uid);
        for(i=0;i<16;i+=4)
            RACE_LOG_MSGID_I("Race UID code: 0x%x 0x%x 0x%x 0x%x", 4, pEvt->uid[0+i],pEvt->uid[1+i],pEvt->uid[2+i],pEvt->uid[3+i]);
        pEvt->status = 0;
    }
    else
    {
        RACE_LOG_MSGID_I("Race UID code error!!!",0);
    }
    return (void*)pEvt;
}

void* race_cmdhdl_sub_version_code_get(PTR_RACE_COMMON_HDR_STRU pCmdMsg, uint16_t length, uint8_t channel_id)
{
    typedef struct {
    uint8_t  status;
    uint8_t sub_version_code;
    } PACKED RSP;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->pktId.field.app_id,
                                    RACE_TYPE_RESPONSE,
                                    RACE_CMD_SUB_VERSION_CODE_GET,
                                    sizeof(RSP),
                                    channel_id);
    if(pEvt){
        pEvt->sub_version_code = (uint8_t)sub_chip_version_get();
        pEvt->status = 0;
        RACE_LOG_MSGID_I("Race Version code: 0x%x", 1, pEvt->sub_version_code);

    }else{
        RACE_LOG_MSGID_I("Race sub_chip_version code error!!!",0);
    }
    return (void*)pEvt;
}


void* RACE_CmdHandler_version_code(ptr_race_pkt_t pCmdMsg, uint16_t length, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("pCmdMsg->hdr.id = %d", 1, (int)pCmdMsg->hdr.id);

    switch (pCmdMsg->hdr.id)
    {
        case RACE_CMD_VERSION_CODE_GET: //0x0305
        {
            return race_cmdhdl_version_code_get((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);
        }
        break;

        case RACE_CMD_UID_GET: //0x0306
        {
            return race_cmdhdl_uid_get((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr), length, channel_id);
        }
        break;
        
        case RACE_CMD_SUB_VERSION_CODE_GET: //   0x0307
        {
            return race_cmdhdl_sub_version_code_get((PTR_RACE_COMMON_HDR_STRU)&(pCmdMsg->hdr),length,channel_id);
        }
        default:
             RACE_LOG_MSGID_I("Race RACE_CmdHandler_version_code error!!!",0);
            break;
    }

    return NULL;
}
#endif

