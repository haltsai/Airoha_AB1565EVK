/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
 /* Airoha restricted information */

/**
 * File: app_ms_teams_activity.h
 *
 * Description: This file defines the interface of app_ms_teams_activity.c.
 *
 */

#ifndef __APP_MS_TEAMS_UTILS_H__
#define __APP_MS_TEAMS_UTILS_H__

#include "ui_shell_activity.h"
#include "ms_teams.h"
#include "apps_config_event_list.h"
#include "apps_config_state_list.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "FreeRTOS.h"
#include "airo_key_event.h"
#include "apps_events_key_event.h"
#include "bt_device_manager.h"
#include "apps_aws_sync_event.h"
#include "apps_events_interaction_event.h"
#include "apps_config_key_remapper.h"
#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_dongle_sync_event.h"

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

typedef struct {
    uint8_t type;
    uint8_t data[0];
} PACKED ms_teams_race_pkg_t;

/**
* @brief      This function is used to set the mmi state by the ms teams call state.
* @param[in]  ev, ms teams call state
* @return     None
*/
void app_ms_teams_set_mmi_state(ms_teams_call_state_sub_event_t ev);

/**
* @brief      This function is used to init ms teams core.
* @return     None
*/
void app_ms_teams_init(void);

/**
* @brief      Check that whether the dongle connected.
* @return     True means the dongle connected.
*/
bool app_ms_teams_is_dongle_connected(void);

#if 0
/**
* @brief      This function is used to send data to dongle.
* @param[in]  type, the type item.
* @param[in]  data, the data append.
* @param[in]  data_len, the length of data.
* @return     None
*/
void app_ms_teams_send_race_data_to_dongle(uint8_t type, uint8_t *data, uint32_t data_len);

/**
* @brief      This function is used to register event callback on race command.
* @return     None
*/
void app_ms_teams_event_init(void);
#endif
#endif


