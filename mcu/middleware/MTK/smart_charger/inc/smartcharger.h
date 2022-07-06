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

#ifndef _DRV_SMART_CHARGER_H_
#define _DRV_SMART_CHARGER_H_

//#include "types.h"
#include "hal_platform.h"
#include "smartchargertypes.h"
#include "types.h"

////////////////////////////////////////////////////////////////////////////////
// Type Definitions ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define SMARTCHARGER_GPIO   HAL_GPIO_2

typedef enum {
    HAL_SMART_CHARGER_STATUS_INVALID_PARAMETER = -2,  /**<  An invalid parameter was given. */
    HAL_SMART_CHARGER_STATUS_ERROR             = -1,  /**<  The function call failed. */
    HAL_SMART_CHARGER_STATUS_OK                =  0   /**<  The function call was successful. */
} hal_smart_charger_status_t;

#define DRV_SMARTCHARGER_RESERVED_PATTERN_ENABLE          (true)
#define DRV_SMARTCHARGER_CASE_WORKAROUND_FOR_DELAY_350MS  (true)

////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define DRV_CHARGER_EVENT_LID_OPEN_PATTERN                  1
#define DRV_CHARGER_EVENT_LID_CLOSE_PATTERN                 2
#define DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN               3
#define DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN               4
#if(DRV_SMARTCHARGER_RESERVED_PATTERN_ENABLE)
#define DRV_SMARTCHARGER_EVENT_RESERVE0_PATTERN             5
#define DRV_SMARTCHARGER_EVENT_RESERVE1_PATTERN             6
#define DRV_SMARTCHARGER_EVENT_RESERVE2_PATTERN             7
#define DRV_SMARTCHARGER_EVENT_RESERVE3_PATTERN             8
#define DRV_SMARTCHARGER_EVENT_RESERVE4_PATTERN             9
#endif
#define DRV_SMARTCHARGER_EVENT_CHARGER_IN_INTERRUPT         10
#define DRV_SMARTCHARGER_EVENT_CHARGER_COMPLETE_INTERRUPT   11
#define DRV_SMARTCHARGER_EVENT_CHARGER_RECHARGE_INTERRUPT   12

#define DRV_CHARGER_EVENT_CHARGER_KEY_PATTERN               13
#define DRV_CHARGER_EVENT_USER1_PATTERN                     14
#define DRV_CHARGER_EVENT_USER2_PATTERN                     15
#define DRV_CHARGER_EVENT_USER3_PATTERN                     16
#define DRV_CHARGER_EVENT_CHARGER_IN_PATTERN                17


////////////////////////////////////////////////////////////////////////////////
// FUNCTION PROTOTYPES /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
extern uint32_t  DRV_SmartCharger_MsgHandler(Handler handler, uint16_t id, void *msg, uint32_t ext_id);
extern void DRV_SmartChargerCase_Init(smartcharger_callback_t handler);
extern void DRV_SmartChargerCaseV2_Init(smartcharger_callback_t handler);
extern void DRV_SmartChargerCaseV2_preHandler(void);
extern void DRV_SmartCharger_IntrHandler(uint16_t type);

#if(DRV_SMARTCHARGER_CASE_WORKAROUND_FOR_DELAY_350MS)
extern void DRV_SmartCharger_Handler(uint16_t type);
#else
extern void DRV_SmartCharger_Handler(void* pMsg);
#endif

#endif /* _DRV_SMART_CHARGER_H_ */
