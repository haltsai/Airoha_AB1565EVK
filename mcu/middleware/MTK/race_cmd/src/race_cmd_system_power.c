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
#include "race_cmd.h"
#include "race_xport.h"
#include "race_event.h"
#include "race_cmd_system_power.h"
#include "race_event_internal.h"


/***************************************************************/
/*                   Defines                                   */
/***************************************************************/
#define SYS_PWR_MODE_ENTER   0x01


void* RACE_CmdHandler_SYS_PWR(ptr_race_pkt_t pCmdMsg, uint16_t Length, uint8_t channel_id)
{
    typedef struct
    {
        uint8_t status;
    } PACKED RSP;

    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t payload;
    } PACKED CMD;
    CMD *cmd = (CMD *)pCmdMsg;

    RSP *pEvt = RACE_ClaimPacket(RACE_TYPE_RESPONSE, pCmdMsg->hdr.id, sizeof(RSP), channel_id);
    if (pEvt == NULL) {
        RACE_LOG_MSGID_E("response alloc fail", 0);
        return NULL;
    }
    pEvt->status = 0xff;
    RACE_ERRCODE err;
    switch (pCmdMsg->hdr.id)
    {
        case RACE_SYSTEM_PMUOFF_MODE:
            if (cmd->payload == SYS_PWR_MODE_ENTER) {
                RACE_LOG_MSGID_I("race cmd PMU OFF, [0x%X]", 1, pCmdMsg->hdr.id);
                uint8_t *msg_data = pvPortMalloc(sizeof(uint8_t));
                if (msg_data == NULL) {
                    RACE_LOG_MSGID_E("race cmd enter PMUOFF mode, msg_data malloc fail", 0);
                    break;
                }
                memset(msg_data, 0, sizeof(uint8_t));
                *msg_data = RACE_SYSTEM_POWER_PMUOFF;
                err = race_send_event_notify_msg(RACE_EVENT_SYSTEM_POWER, (void *)msg_data);
                if (err == RACE_ERRCODE_SUCCESS) {
                    pEvt->status = 0x00;
                }
            }
            break;
        case RACE_SYSTEM_RTC_MODE:
            if (cmd->payload == SYS_PWR_MODE_ENTER) {
                RACE_LOG_MSGID_I("race cmd enter RTC mode, [0x%X]", 1, pCmdMsg->hdr.id);
                uint8_t *msg_data = pvPortMalloc(sizeof(uint8_t));
                if (msg_data == NULL) {
                    RACE_LOG_MSGID_E("race cmd enter RTC mode, msg_data malloc fail", 0);
                    break;
                }
                memset(msg_data, 0, sizeof(uint8_t));
                *msg_data = RACE_SYSTEM_POWER_RTC;
                err = race_send_event_notify_msg(RACE_EVENT_SYSTEM_POWER, (void *)msg_data);
                if (err == RACE_ERRCODE_SUCCESS) {
                    pEvt->status = 0x00;
                }
            }
            break;
        case RACE_SYSTEM_SLEEP_MODE:
            if (cmd->payload == SYS_PWR_MODE_ENTER) {
                RACE_LOG_MSGID_I("race cmd enter sleep mode, [0x%X]", 1, pCmdMsg->hdr.id);
                uint8_t *msg_data = pvPortMalloc(sizeof(uint8_t));
                if (msg_data == NULL) {
                    RACE_LOG_MSGID_E("race cmd enter SLEEP mode, msg_data malloc fail", 0);
                    break;
                }
                memset(msg_data, 0, sizeof(uint8_t));
                *msg_data = RACE_SYSTEM_POWER_SLEEP;
                err = race_send_event_notify_msg(RACE_EVENT_SYSTEM_POWER, (void *)msg_data);
                if (err == RACE_ERRCODE_SUCCESS) {
                    pEvt->status = 0x00;
                }
            }
            break;
        default:
            RACE_LOG_MSGID_E("unknown race cmd key cmd [0x%X]", 1, pCmdMsg->hdr.id);
            break;
    }

    return pEvt;
}


