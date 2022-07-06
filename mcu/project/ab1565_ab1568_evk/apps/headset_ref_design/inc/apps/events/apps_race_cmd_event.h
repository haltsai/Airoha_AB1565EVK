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

#ifndef __APPS_RACE_CMD_EVENT__
#define __APPS_RACE_CMD_EVENT__

#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd.h"
#include "race_event.h"



////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_GET_MMI_STATE      0x2C80
#define RACE_PLAY_VOICE_PROMPT  0x2C81

#define RACE_SET_APP_COMMON_CONFIG  0x2C82
#define RACE_GET_APP_COMMON_CONFIG  0x2C83
#define RACE_SET_LED_PATTERN  0x2C85

typedef enum {
    APPS_RACE_CMD_CONFIG_TYPE_TOUCH_KEY_ENABLE = 0,
    APPS_RACE_CMD_CONFIG_TYPE_POWER_SAVING_CFG = 1,
    APPS_RACE_CMD_CONFIG_TYPE_IN_EAR_MUSIC_CFG = 2,
    APPS_RACE_CMD_CONFIG_TYPE_LINE_IN_JACK_STA = 3,
    APPS_RACE_CMD_CONFIG_TYPE_DETACH_MIC_JACK_STA = 4,
    APPS_RACE_CMD_CONFIG_TYPE_MFI_DEVICE_ID = 5,
    APPS_RACE_CMD_CONFIG_TYPE_SIDETONE_LEVEL = 6,
    APPS_RACE_CMD_CONFIG_TYPE_SIDETONE_ENABLE = 7,
    APPS_RACE_CMD_CONFIG_TYPE_ADVANCED_PASSTHROUGH = 8,
    APPS_RACE_CMD_CONFIG_TYPE_VP_LANGUAGE = 9,
    APPS_RACE_CMD_CONFIG_TYPE_USB_LOGGING_MODE = 0xA,
    APPS_RACE_CMD_CONFIG_TYPE_ULL_MIX_RATIO = 0x0B,
    APPS_RACE_CMD_CONFIG_TYPE_ULL_MIC_VOL = 0x0C,
} apps_race_common_config_type_t;

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/************************************* NOTI Definition End *************************************/


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*!
  @brief initialize app race cmd event module.
*/
void apps_race_cmd_event_init(void);

/**
* @brief      This function send notify to smartphone.
* @param[in]  cfg_type, see APPS_RACE_CMD_CONFIG_TYPE_t for details.
* @param[in]  data, the notify data, you should free the memory after this function return.
* @param[in]  len, the length of notify data.
*/
void app_race_send_notify(uint16_t cfg_type, int8_t *data, uint32_t len);

/**
* @brief      This function send notify to PC the MMI state.
* @param[in]  mmi_state, current MMI state.
*/
void app_race_notify_mmi_state(uint32_t mmi_state);

#endif /* #ifdef MTK_RACE_CMD_ENABLE */

#endif /* __APPS_RACE_CMD_EVENT__ */
