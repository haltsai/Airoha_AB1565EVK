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
#ifdef RACE_CTRL_BASEBAND_CMD_ENABLE
#include "race_xport.h"
#include "race_cmd_ctrl_baseband.h"


////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
/**
 * RACE_CTRL_BASEBAND_GET_POWER_MODE_HDL
 *
 * Control baseband GET_POWER_MOD COMMAND Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_CTRL_BASEBAND_GET_POWER_MODE_HDL(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct
	{
		uint8_t status;
        uint8_t power_mode; // 0-nomal mode; 1-low power mode
	}PACKED RSP;

	RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

	RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_CTRL_BASEBAND_GET_POWER_MODE,
                                 (uint16_t)sizeof(RSP),
                                 channel_id);

    if (pEvt != NULL)
    {
        // TODO: get current power mode
        pEvt->power_mode = 1;
        pEvt->status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

	return pEvt;
}



/**
 * RACE_CTRL_BASEBAND_GET_POWER_MODE_HDL
 *
 * Control baseband GET_POWER_MOD COMMAND Handler
 *
 * @pCmdMsg : pointer of ptr_race_pkt_t
 *
 */
void* RACE_CTRL_BASEBAND_SWITCH_POWER_MODE_HDL(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    /*typedef struct
	{
		RACE_COMMON_HDR_STRU Hdr;
        uint8_t power_mode; // 0-nomal mode; 1-low power mode
	}PACKED CMD;*/

    typedef struct
	{
		uint8_t status;
	}PACKED RSP;

	RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (!pCmdMsg)
    {
        return NULL;
    }

	RSP* pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                 (uint16_t)RACE_CTRL_BASEBAND_SWITCH_POWER_MODE,
                                 (uint16_t)sizeof(RSP),
                                 channel_id);
    if (pEvt != NULL)
    {
        // TODO: set power mode according to pCmd->power_mode
        pEvt->status = (uint8_t)RACE_ERRCODE_SUCCESS;
    }

	return pEvt;
}


void* RACE_CmdHandler_CTRL_BASEBAND(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void* ptr = NULL;

	RACE_LOG_MSGID_I("pRaceHeaderCmd->hdr.id = %d \r\n",1, (int)pRaceHeaderCmd->hdr.id);
	
    switch (pRaceHeaderCmd->hdr.id)
    {
        case RACE_CTRL_BASEBAND_GET_POWER_MODE :
        {
            ptr = RACE_CTRL_BASEBAND_GET_POWER_MODE_HDL(pRaceHeaderCmd, channel_id);
        }
        break;

        case RACE_CTRL_BASEBAND_SWITCH_POWER_MODE :
        {
            ptr = RACE_CTRL_BASEBAND_SWITCH_POWER_MODE_HDL(pRaceHeaderCmd, channel_id);
        }
        break;

        default:
            break;
    }
	
    return ptr;
}

#endif /* RACE_CTRL_BASEBAND_CMD_ENABLE */

