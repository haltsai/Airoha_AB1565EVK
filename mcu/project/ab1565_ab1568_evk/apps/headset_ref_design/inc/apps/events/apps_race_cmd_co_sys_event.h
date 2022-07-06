/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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
 
#ifndef __APPS_RACE_CMD_CO_SYS_EVENT__
#define __APPS_RACE_CMD_CO_SYS_EVENT__

#ifdef AIR_RACE_CO_SYS_ENABLE
#include "race_cmd_co_sys.h"


////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

typedef enum {
    APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_GET_TIMESTAMP,
    APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_POWER_OFF_READY,
    APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_REBOOT_READY,
    APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_UPDATE_MMI_STATE,
    APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_UPDATE_BG_LED,
    APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_UPDATE_FG_LED,
    APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_PLAY_VP,
    APPS_RECE_CMD_CO_SYS_DUAL_CHIP_EVENT_SILENCE_DETECT,
    APPS_RECE_CMD_CO_SYS_DUAL_CHIP_EVENT_SLAVE_POWER_ON,
    APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_SYNC_KEY_TABLE,
} apps_race_cmd_co_sys_dual_chip_event_t;

typedef struct {
    uint32_t group_id;
    uint32_t event_id;
    uint32_t data_len;
    uint8_t data[0];
} apps_race_cmd_co_sys_common_cmd_buf_t;

typedef struct {
    apps_race_cmd_co_sys_dual_chip_event_t event_id;
    uint32_t data_len;
    uint8_t data[0];
} apps_race_cmd_co_sys_dual_chip_cmd_buf_t;

typedef struct {
    uint8_t index;
    uint8_t need_sync;
    uint16_t fg_timeout;
} apps_race_cmd_co_sys_led_pattern_format_t;

typedef struct {
    uint8_t call_voice;
    uint8_t is_start;
    uint8_t index;
    uint8_t level;
    uint8_t is_ringtone;
    uint8_t is_repeat;
} apps_race_cmd_co_sys_vp_format_t;


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/************************************* NOTI Definition End *************************************/


////////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*!
  @brief initialize app co-system race cmd event module.
*/
void apps_race_cmd_co_sys_event_init(void);

/**
* @brief      This function send notify to the other side of dual chip project.
* @param[in]  event_group, event group id, must be in apps_event_group_t.
* @param[in]  event_id, the event id of the event.
* @param[in]  extra_data, the extra data sending with the event.
* @param[in]  extra_data_len, the length of data.
*/
void app_race_cmd_co_sys_send_event(uint32_t event_group, uint32_t event_id, void *extra_data, uint32_t extra_data_len, bool is_critical);


#endif /* #ifdef AIR_RACE_CO_SYS_ENABLE */

#endif /* __APPS_RACE_CMD_CO_SYS_EVENT__ */
