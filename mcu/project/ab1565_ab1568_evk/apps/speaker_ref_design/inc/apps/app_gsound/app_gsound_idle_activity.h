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


/**
 * File: app_gsound_idle_activity.h
 *
 * Description: This file defines the interface of app_gsound_idle_activity.c.
 */


#ifndef __APP_GSOUND_IDLE_ACTIVITY_H__
#define __APP_GSOUND_IDLE_ACTIVITY_H__

#ifdef GSOUND_LIBRARY_ENABLE

#include "ui_shell_activity.h"
#include "bt_sink_srv.h"

/**
 * @brief GSound events about connection
 */
enum {
    APPS_EVENTS_INTERACTION_GSOUND_ENABLED,
    APPS_EVENTS_INTERACTION_GSOUND_DISABLED,
    APPS_EVENTS_INTERACTION_GSOUND_CONNECTED,
    APPS_EVENTS_INTERACTION_GSOUND_DISCONNECTED,
    APPS_EVENTS_INTERACTION_GSOUND_RHO_PREPARE_READY,
    APPS_EVENTS_INTERACTION_GSOUND_PARTNER_ENABLE,
    APPS_EVENTS_INTERACTION_GSOUND_PARTNER_DISABLE,
    APPS_EVENTS_INTERACTION_GSOUND_POWER_OFF_SYNC,
};

enum {
    APP_GSOUND_BATTERY_INFO_LOCAL_PERCENT,
    APP_GSOUND_BATTERY_INFO_LOCAL_IS_CHARGING,
    APP_GSOUND_BATTERY_INFO_PARTNER_PERCENT,
    APP_GSOUND_BATTERY_INFO_PARTNER_IS_CHARGING,
    APP_GSOUND_BATTERY_INFO_CASE,
};

/**
* @brief      This function is the interface of the app_gsound_idle_activity_proc, and is only called by ui_shell when events are sent.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_group, the current event group to be handled.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 if there extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_gsound_idle_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);

/**
* @brief      This function will process the BT sink action.
* @param[in]  sink_action, the action from BT sink
* @return     True means the action is only used by gsound, it will not be processed by music
*/
bool app_gsound_idle_activity_proc_music_event(bt_sink_srv_action_t sink_action);
uint32_t app_gsound_get_battery_info(uint8_t info_type);

#endif
#endif /* __APP_GSOUND_IDLE_ACTIVITY_H__ */
