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

/**
 * File: app_battery_transient_activity.h
 *
 * Description: This file defines the interface of app_battery_transient_activity.c.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#ifndef __APP_BATTERY_TRANSIENT_ACTIVITY_H__
#define __APP_BATTERY_TRANSIENT_ACTIVITY_H__

#include "ui_shell_activity.h"

#include "apps_events_battery_event.h"
#include "bt_sink_srv.h"

/**
 *  @brief This enum defines the states of battery_app.
 */
typedef enum {
    APP_BATTERY_STATE_SHUTDOWN,         /**<  Shutdown state for LOW VOLTAGE, the device will power off if not charging. */
    APP_BATTERY_STATE_LOW_CAP,          /**<  Low battery state, the device will play VP "low battery, please charge!". */
    APP_BATTERY_STATE_IDLE,             /**<  Idle state, not charging (out of case). */
    APP_BATTERY_STATE_FULL,             /**<  Full battery state, not charging (out of case). */
    APP_BATTERY_STATE_CHARGING,         /**<  Charging state. */
    APP_BATTERY_STATE_CHARGING_FULL,    /**<  Charging full state. */
    APP_BATTERY_STATE_THR               /**<  High temperature state. */
} app_battery_state_t;

/**
 *  @brief This structure defines the context of Battery APP.
 */
typedef struct _local_context_type {
    uint8_t battery_percent;            /**<  The battery percent of current device. */
    uint8_t partner_battery_percent;    /**<  For agent, partner battery. For partner, invalid value. */
    int32_t charging_state;             /**<  Charging state in battery_management. */
    int32_t charger_exist_state;        /**<  Charger_exist_state in battery_management. */
    battery_event_shutdown_state_t shutdown_state;      /**<  Shutdown_state in battery_management. */
    bt_aws_mce_agent_state_type_t aws_state;            /**<  BT AWS connection state. */
    bt_sink_srv_state_t sink_state;                     /**<  BT Sink service state. */
    app_battery_state_t state;                          /**<  The states of battery. */
} battery_local_context_type_t;

#define PARTNER_BATTERY_INVALID             (0xFF)  /* Invalid battery percent. */
#define PARTNER_BATTERY_CHARGING            (0x80)  /* Bit mask of battery charging status. */

/**
* @brief      This function is the interface of the app_battery_transient_activity, and is only called by ui_shell when events are sent.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_group, the current event group to be handled.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_battery_transient_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);

#endif /* __APP_BATTERY_TRANSIENT_ACTIVITY_H__ */
