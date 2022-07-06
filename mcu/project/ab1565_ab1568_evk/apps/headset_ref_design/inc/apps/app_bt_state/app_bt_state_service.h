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

/**
 * File: app_bt_state_service.h
 *
 * Description: This file defines the interface of app_bt_state_service.c.
 *
 */

#ifndef __APP_BT_STATE_SERVICE_H__
#define __APP_BT_STATE_SERVICE_H__

#include "ui_shell_activity.h"

#define BT_VISIBLE_TIMEOUT_INVALID      (0)                 /* Invalid BT visibility duration. */

/**
 *  @brief This enum defines the BT power state.
 */
typedef enum {
    APP_BT_STATE_POWER_STATE_DISABLED,
    APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLED,
    APP_BT_STATE_POWER_STATE_DISABLING,
    APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLING,
    APP_BT_STATE_POWER_STATE_ENABLING,
    APP_BT_STATE_POWER_STATE_ENABLED,
    APP_BT_STATE_POWER_STATE_NONE_ACTION = 0xFF,
} app_bt_state_service_power_state_t;

/**
 *  @brief This enum defines the states of BT connection.
 */
typedef enum {
    APP_BT_CONNECTION_SERVICE_BT_STATE_BT_OFF,              /**<  BT off. */
    APP_BT_CONNECTION_SERVICE_BT_STATE_DISCONNECTED,        /**<  BT on but ACL (Asynchronous Connectionless) disconnected. */
    APP_BT_CONNECTION_SERVICE_BT_STATE_ACL_CONNECTED,       /**<  BT on and ACL (Asynchronous Connectionless) connected, but no profile connected. */
    APP_BT_CONNECTION_SERVICE_BT_STATE_PROFILE_CONNECTED    /**<  BT on and profile connected. */
} app_bt_state_service_connection_state_t;

#ifdef AIR_LE_AUDIO_ENABLE
/**
 *  @brief This enum defines the states of BLE connection.
 */
typedef enum {
    APP_BLE_AUDIO_STATE_BT_OFF,                             /**<  BT off. */
    APP_BLE_AUDIO_STATE_DISCONNECTED,                       /**<  BLE Audio Disconnected. */
    APP_BLE_AUDIO_STATE_CONNECTED,                          /**<  BLE Audio Connected. */
    APP_BLE_AUDIO_STATE_PROFILE_CONNECTED                   /**<  BLE Audio profile Disconnected. */
} app_ble_audio_state_t;
#endif

/**
 *  @brief This structure defines the status/context of BT state service.
 */
typedef struct {
    app_bt_state_service_connection_state_t connection_state;   /**<  BT connection state. */
#ifdef AIR_LE_AUDIO_ENABLE
    app_ble_audio_state_t                   ble_audio_state;    /**<  BLE Audio connection state. */
#endif
    app_bt_state_service_power_state_t      current_power_state;/**<  Current BT power state. */
    app_bt_state_service_power_state_t      target_power_state; /**<  Target BT power state. */
    bool aws_connected;                                         /**<  BT AWS connection flag. */
    bool bt_visible;                                            /**<  BT visibility state. */
#ifdef MTK_AWS_MCE_ENABLE
    bool in_air_pairing;                                        /**<  BT is in air pairing */
#endif
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    bool in_ull_pairing;                                        /**<  BT is in ULL air pairing */
#endif
} app_bt_state_service_status_t;

/**
* @brief      This function is used to handle events and update BT state after pre-process.
* @param[in]  event_group, the current event group to be handled.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_bt_state_service_process_events(uint32_t event_group,
                                         uint32_t event_id,
                                         void *extra_data,
                                         size_t data_len);

/**
* @brief      This function is used to turn on/off BT.
* @param[in]  on, TRUE - turn on BT, FALSE - turn off BT.
* @param[in]  classic_off, TRUE - only turn off classic BT (keep BLE on) when turn off BT.
* @param[in]  need_do_rho, whether need to trigger RHO before turn off BT.
* @param[in]  for_system_off, turn off BT for system off.
*/
void app_bt_state_service_set_bt_on_off(bool on, bool classic_off, bool need_do_rho, bool for_system_off);

/**
* @brief      This function is used to turn on/off BT visibility.
* @param[in]  enable_visible, TRUE - enable BT visibility, FALSE - disable BT visibility.
* @param[in]  wait_aws_connect, TRUE - must wait AWS connected, then enable BT visibility.
* @param[in]  timeout, BT visibility duration time.
* @return     If return true, the operation is successful.
*/
bool app_bt_state_service_set_bt_visible(bool enable_visible, bool wait_aws_connect, uint32_t timeout);

/**
* @brief      This function is used to get current status of BT state service.
* @return     The status of BT state service.
*/
const app_bt_state_service_status_t *app_bt_connection_service_get_current_status(void);

/**
* @brief      This function is used to get visible status of BT state service.
* @return     The status of BT visible (discoverable mode).
*/
bool app_bt_service_is_visible();

#ifdef MTK_AWS_MCE_ENABLE
/**
* @brief      This function set the flag to indicate air pairing is doing.
* @param[in]  True means doing
*/
void app_bt_state_service_set_air_pairing_doing(bool doing);
#endif

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
/**
* @brief      This function set the flag to indicate ULL air pairing is doing.
* @param[in]  True means doing
*/
void app_bt_state_service_set_ull_air_pairing_doing(bool doing);
#endif

#endif /* __APP_BT_STATE_SERVICE_H__ */
