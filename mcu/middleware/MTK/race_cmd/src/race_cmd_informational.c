/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
#include "race_cmd_informational.h"
#include "verno.h"
#include "race_xport.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


static void* RACE_READ_SDK_VERSION_HDR(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    RACE_LOG_MSGID_I("RACE_READ_SDK_VERSION_HDR() enter\r\n", 0);

    typedef struct
    {
        uint8_t length_of_read_bytes;
        char ReadData[0];
    }PACKED RACE_READ_SDK_VERSION_EVT_STRU;

    RACE_READ_SDK_VERSION_EVT_STRU* pEvt = NULL;

    pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_RESPONSE, (uint16_t)RACE_READ_SDK_VERSION,
                            (uint16_t)(sizeof(RACE_READ_SDK_VERSION_EVT_STRU) + sizeof(MTK_FW_VERSION)),
                            channel_id);
    if (pEvt) {
        memcpy(&pEvt->ReadData, MTK_FW_VERSION, sizeof(MTK_FW_VERSION));
        pEvt->length_of_read_bytes = sizeof(MTK_FW_VERSION);
    }
    return pEvt;
}
void* RACE_CmdHandler_INFORMATION(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void* ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_INFORMATION, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    switch (pRaceHeaderCmd->hdr.id)
    {
        case RACE_READ_SDK_VERSION :
        {
            ptr = RACE_READ_SDK_VERSION_HDR(pRaceHeaderCmd, channel_id);
        }
        break;

        default:
        {
            while(1);
        }
        break;
    }

    return ptr;
}

