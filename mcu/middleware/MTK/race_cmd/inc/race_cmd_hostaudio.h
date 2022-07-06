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


#ifndef __RACE_CMD_HOSTAUDIO_H__
#define __RACE_CMD_HOSTAUDIO_H__


#include "race_cmd_feature.h"
#ifdef RACE_HOSTAUDIO_CMD_ENABLE
#include "stdint.h"

#include "race_cmd.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_HOSTAUDIO_BEGIN_ID               0x0900

#define RACE_MMI_SET_ENUM                     0x0900
#define RACE_MMI_GET_ENUM                     0x0901
#define RACE_HOSTAUDIO_PEQ_SAVE_STATUS        0x09FD

#define RACE_HOSTAUDIO_END_ID                 0x09FF

////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RACE_MMI_MODULE_PEQ_GROUP_ID        (0)
#define RACE_MMI_MODULE_VP_ONOFF            (1)
#define RACE_MMI_MODULE_VP_LANGUAGE         (2)
#define RACE_MMI_MODULE_VP_GET              (3)
#define RACE_MMI_MODULE_VP_SET              (4)
#define RACE_MMI_MODULE_ANC_STATUS          (5)
#define RACE_MMI_MODULE_GAME_MODE           (6)
#define RACE_MMI_MODULE_PASSTHRU_GAIN       (7)
#define RACE_MMI_MODULE_LINEIN_PEQ_GROUP_ID (11)
#define RACE_MMI_MODULE_SHARE_MODE          (12)
#define RACE_MMI_MODULE_RHO_DONE            (20)

#define AUDIO_PATH_LINE_IN                  (0x01)
#define AUDIO_PATH_BT                       (0x00)

/**
 * Add for line-in feature
 */
#ifdef APPS_LINE_IN_SUPPORT
#define RACE_MMI_MODULE_AUDIO_PATH          (0x0A)
#endif /* APPS_LINE_IN_SUPPORT */

typedef uint16_t race_mmi_module_t;

typedef struct
{
    RACE_COMMON_HDR_STRU Hdr;
    race_mmi_module_t module;
}PACKED RACE_MMI_GET_ENUM_CMD_STRU;

typedef struct
{
    race_mmi_module_t module;
    uint8_t status;
    uint8_t data[1];
}PACKED RACE_MMI_GET_ENUM_EVT_STRU;

typedef struct
{
    race_mmi_module_t module;
    uint8_t parameters[1];
}PACKED RACE_MMI_COSYS_RELAY_SET_ENUM_CMD_STRU;

////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*!
  @brief Process HOSTAUDIO related RACE commands.

  @param pRaceHeaderCmd This parameter represents the raw data such as "05 5A...".
  @param Lenth Total bytes of this RACE command.
  @param channel_id Channel identifier
*/
void* RACE_CmdHandler_HOSTAUDIO(ptr_race_pkt_t pRaceHeaderCmd, uint16_t Length, uint8_t channel_id);

/**
 * Add for line_in feature
 */
#ifdef APPS_LINE_IN_SUPPORT

typedef struct {

    void (*pull_request)(uint8_t *current_audio_path);
    void (*control_request)(uint8_t new_audio_path, uint8_t *control_result);
    void (*push_response)(uint8_t status);

} line_in_app_callback_t;

void race_cmd_hostaudio_set_app_line_in_callback(line_in_app_callback_t *callback);

void race_cmd_hostaudio_notify_app_audio_path_change(uint8_t new_audio_path);

#endif /* APPS_LINE_IN_SUPPORT */

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
bool race_cmd_hostaudio_cosys_relay_set_cmd(race_mmi_module_t module, uint8_t parameters);
void race_cmd_hostaudio_cosys_relay_set_cmd_callback(bool is_critical, uint8_t *buff, uint32_t len);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE   AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE*/

#endif /* RACE_HOSTAUDIO_CMD_ENABLE */
#endif /* __RACE_CMD_HOSTAUDIO_H__ */

