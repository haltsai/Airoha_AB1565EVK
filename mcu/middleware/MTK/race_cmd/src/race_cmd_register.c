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
#ifdef RACE_RG_READ_WRITE_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "syslog.h"
#include "hal.h"
#include "race_util.h"
#include "race_xport.h"
#include "race_lpcomm_aws.h"
#include "race_cmd_bluetooth.h"
#include "race_lpcomm_util.h"
#include "race_lpcomm_trans.h"
#include "race_lpcomm_conn.h"
#include "race_lpcomm_msg_struct.h"
#include "race_noti.h"
#include "race_lpcomm_ps_noti.h"
#include "race_fota_util.h"
#ifdef RACE_CAPTOUCH_CMD_ENABLE
#include "hal_captouch_internal.h"
#endif
#include "ept_keypad_drv.h"
#include "race_cmd_register.h"
#include "timers.h"


////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
uint32_t DRV_2WIRE_Read_default(uint16_t addr)
{
    return 0;
}
void DRV_2WIRE_Write_default(uint16_t addr, uint8_t data)
{
    return;
}
#pragma weak DRV_2WIRE_Read = DRV_2WIRE_Read_default
#pragma weak DRV_2WIRE_Write = DRV_2WIRE_Write_default
extern void DRV_2WIRE_Write(uint16_t Addr, uint8_t Data);
extern uint32_t DRV_2WIRE_Read(uint16_t Addr);


void *race_2wire_register_read(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint16_t rg_addr_low;
        uint16_t rg_addr_high;
    } PACKED CMD;

    typedef struct {
        uint8_t  status;
        uint8_t rg_data;
        uint16_t rg_addr;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_2WIRE_RG_READ,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);

    uint16_t temp_addr;
    temp_addr = ((pCmd->rg_addr_high & 0x00FF)<<8) | ((pCmd->rg_addr_low & 0xFF00)>>8);

    RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (pEvt) {
        pEvt->rg_data   = DRV_2WIRE_Read(temp_addr);
        pEvt->rg_addr   = temp_addr;
        pEvt->status    = RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}


void *race_2wire_register_write(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t rg_addr_high;
        uint8_t  value;
        uint8_t rg_addr_low;
    } PACKED CMD;

    typedef struct {
        uint8_t  status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_2WIRE_RG_WRITE,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);
    uint16_t reg_addr;
    reg_addr = (pCmd->rg_addr_high << 8) | (pCmd->rg_addr_low);

    RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (pEvt) {
        DRV_2WIRE_Write(reg_addr, pCmd->value);
        pEvt->status    = RACE_ERRCODE_SUCCESS;
    }

    return pEvt;
}


void *race_register_read(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint16_t module_id;
        uint32_t rg_addr;
    } PACKED CMD;

    typedef struct {
        uint8_t  status;
        uint16_t module_id;
        uint32_t rg_addr;
        uint32_t rg_data;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_RG_READ,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);
    RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (pEvt) {
        if (pCmd->module_id == 0) { // common register
            pEvt->rg_data   = *(volatile uint32_t *)(pCmd->rg_addr);
            pEvt->rg_addr   = pCmd->rg_addr;
            pEvt->module_id = pCmd->module_id;
            pEvt->status    = RACE_ERRCODE_SUCCESS;
        } else if (pCmd->module_id == RACE_RG_MODULE_CAPTOUCH_ANALOG) { // captouch 3wire analog register
            RACE_LOG_MSGID_I("not support RACE_RG_MODULE_CAPTOUCH_ANALOG,module_id=%d\r\n",1, pCmd->module_id);
        } else {
            RACE_LOG_MSGID_I("module id %d not support yet\r\n",1, pCmd->module_id);
            pEvt->rg_data   = 0xffffffff;
            pEvt->rg_addr   = pCmd->rg_addr;
            pEvt->module_id = pCmd->module_id;
            pEvt->status = RACE_ERRCODE_PARAMETER_ERROR;
        }
    }

    return pEvt;
}


void *race_register_write(ptr_race_pkt_t pCmdMsg, uint8_t channel_id)
{
    typedef struct {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint16_t module_id;
        uint32_t rg_addr;
        uint32_t rg_data;
    } PACKED CMD;

    typedef struct {
        uint8_t  status;
    } PACKED RSP;

    CMD *pCmd = (CMD *)pCmdMsg;
    RSP *pEvt = RACE_ClaimPacketAppID(pCmdMsg->hdr.pktId.field.app_id,
                                      (uint8_t)RACE_TYPE_RESPONSE,
                                      (uint16_t)RACE_RG_WRITE,
                                      (uint16_t)sizeof(RSP),
                                      channel_id);

    RACE_LOG_MSGID_I("channel_id = %x \r\n",1, channel_id);

    if (pEvt) {
        if (pCmd->module_id == 0) { // common register
            *(volatile uint32_t *)(pCmd->rg_addr) = pCmd->rg_data;
            pEvt->status    = RACE_ERRCODE_SUCCESS;
        } else if (pCmd->module_id == RACE_RG_MODULE_CAPTOUCH_ANALOG) { // captouch 3wire analog register
            RACE_LOG_MSGID_I("not support RACE_RG_MODULE_CAPTOUCH_ANALOG,module_id=%d\r\n",1, pCmd->module_id);
        } else {
            RACE_LOG_MSGID_I("module id %d not support yet\r\n",1, pCmd->module_id);
            pEvt->status = RACE_ERRCODE_PARAMETER_ERROR;
        }
    }

    return pEvt;
}


void *RACE_CmdHandler_RG_read_write(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void *ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_RG_read_write, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    if (pRaceHeaderCmd->hdr.type == RACE_TYPE_COMMAND) {
        switch (pRaceHeaderCmd->hdr.id) {
            case RACE_RG_READ : {
                ptr = race_register_read(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_RG_WRITE : {
                ptr = race_register_write(pRaceHeaderCmd, channel_id);
            }
            break;

            default: {
                break;
            }
        }
    }

    return ptr;
}


void *RACE_CmdHandler_2wire_RG_read_write(ptr_race_pkt_t pRaceHeaderCmd, uint16_t length, uint8_t channel_id)
{
    void *ptr = NULL;

    RACE_LOG_MSGID_I("RACE_CmdHandler_2wire_RG_read_write, type[0x%X], race_id[0x%X], channel_id[%d]", 3,
        pRaceHeaderCmd->hdr.type, pRaceHeaderCmd->hdr.id, channel_id);

    if (pRaceHeaderCmd->hdr.type == RACE_TYPE_COMMAND) {
        switch (pRaceHeaderCmd->hdr.id) {
            case RACE_2WIRE_RG_WRITE : {
                ptr = race_2wire_register_write(pRaceHeaderCmd, channel_id);
            }
            break;

            case RACE_2WIRE_RG_READ : {
                ptr = race_2wire_register_read(pRaceHeaderCmd, channel_id);
            }
            break;

            default: {
                break;
            }
        }
    }

    return ptr;
}



#endif /* RACE_CAPTOUCH_CMD_ENABLE */

