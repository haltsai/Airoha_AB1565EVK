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

#include "hal_platform.h"
#include "hal_log.h"
#include "smartchargertypes.h"
#include "types.h"


char LibDrv_Charger_SetState[] = "[Charger] Set State: <%s(%d)>";
char LibDrv_SmartCase_SetState[] = "[Smart Case] Set State: <%s(%d)>";




 /*************************************************************************************************
 * Structure
 **************************************************************************************************/
typedef struct CHARGER_SMART_CASE_CTL_STRU
{
    smartcharger_callback_t serverChargerApp;
    smartcharger_callback_t serverSmartCaseApp;
    smartcharger_callback_t serverSmartRsvApp;
    uint8_t chargerState;
    uint8_t smartCaseState;
}CHARGER_SMART_CASE_CTL_STRU;

/*************************************************************************************************
* Variable
**************************************************************************************************/
static CHARGER_SMART_CASE_CTL_STRU gChargerSmartCaseCtl;

/*************************************************************************************************
* Public function
**************************************************************************************************/
void ChargerSmartCase_SetChargerState(uint8_t state)
{
    //log_hal_msgid_info("[Charger] Set State: <%s(%d)>", 2, LibDrv_Charger_StateString[state], state);
    gChargerSmartCaseCtl.chargerState = state;
}

void ChargerSmartCase_SetSmartCaseState(uint8_t state)
{
    //log_hal_msgid_info("[Smart Case] Set State: <%s(%d)>", 2, LibDrv_SmartCase_StateString[state], state);
    gChargerSmartCaseCtl.smartCaseState = state;
}

uint8_t ChargerSmartCase_GetChargerState()
{
    return gChargerSmartCaseCtl.chargerState;
}

uint8_t ChargerSmartCase_GetSmartCaseState()
{
    return gChargerSmartCaseCtl.smartCaseState;
}

void ChargerSmartCase_SetChargerHandle(smartcharger_callback_t app)
{
    gChargerSmartCaseCtl.serverChargerApp = app;
}

void ChargerSmartCase_SetSmartCaseHandle(smartcharger_callback_t app)
{
    gChargerSmartCaseCtl.serverSmartCaseApp = app;
}

void ChargerSmartCase_SetSmartRSVHandle(smartcharger_callback_t app)
{
    gChargerSmartCaseCtl.serverSmartRsvApp = app;
}

smartcharger_callback_t ChargerSmartCase_GetChargerHandle()
{
    return gChargerSmartCaseCtl.serverChargerApp;
}

smartcharger_callback_t ChargerSmartCase_GetSmartCaseHandle()
{
    return gChargerSmartCaseCtl.serverSmartCaseApp;
}

smartcharger_callback_t ChargerSmartCase_GetSmartRsvHandle()
{
    return gChargerSmartCaseCtl.serverSmartRsvApp;
}

