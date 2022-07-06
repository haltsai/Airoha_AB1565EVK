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

#ifndef __BT_DEVICE_MANAGER_POWER_H__
#define __BT_DEVICE_MANAGER_POWER_H__

#include "bt_type.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @brief Define for the bluetooth device type.
 */
#define BT_DEVICE_TYPE_LE                       (0x01)  /**< The bluetooth low energy device. */
#define BT_DEVICE_TYPE_CLASSIC                  (0x02)  /**< The bluetooth classic device. */
typedef uint8_t bt_device_type_t;

/**
 *  @brief Define for the bluetooth power event.
 */
#define BT_DEVICE_MANAGER_POWER_EVT_PREPARE_ACTIVE              (0x01)  /**< The event of prepare active device power. */
#define BT_DEVICE_MANAGER_POWER_EVT_PREPARE_STANDBY             (0x02)  /**< The event of prepare standby device power. */
#define BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE             (0x03)  /**< The event of device power active complete. */
#define BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE            (0x04)  /**< The event of device power standby complete. */
#define BT_DEVICE_MANAGER_POWER_EVT_LE_ACTIVE_COMPLETE          (0x05)  /**< The event of device le power active complete. */
#define BT_DEVICE_MANAGER_POWER_EVT_LE_STANDBY_COMPLETE         (0x06)  /**< The event of device le power standby complete. */
#define BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_ACTIVE_COMPLETE     (0x07)  /**< The event of device classic power active complete. */
#define BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_STANDBY_COMPLETE    (0x08)  /**< The event of device classic power standby complete. */
typedef uint8_t bt_device_manager_power_event_t;

/**
 *  @brief Define for the bluetooth power status.
 */
#define BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS              (0x00)  /**< The device manager power status success. */
#define BT_DEVICE_MANAGER_POWER_STATUS_AIR_PAIRING_START    (0x01)  /**< The device manager power status of air pairing start. */
#define BT_DEVICE_MANAGER_POWER_STATUS_AIR_PAIRING_COMPLETE (0x02)  /**< The device manager power status of air pairing complete. */
#define BT_DEVICE_MANAGER_POWER_STATUS_ROLE_RECOVERY        (0x03)  /**< The device manager power status of role recovery. */
typedef uint8_t bt_device_manager_power_status_t;

/**
 *  @brief Define for the bluetooth power reset type.
 */
#define BT_DEVICE_MANAGER_POWER_RESET_TYPE_NORMAL           (0x00)  /**< The device manager power normal reset. */
#define BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_START    (0x01)  /**< The device manager power reset for air pairing start. */
#define BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_COMPLETE (0x02)  /**< The device manager power reset for air pairing complete. */
#define BT_DEVICE_MANAGER_POWER_RESET_TYPE_ROLE_RECOVERY    (0x03)  /**< The device manager power reset for role recovery. */
typedef uint8_t bt_device_manager_power_reset_t;

/**
 *  @brief Define the callback type of device manager power event handler for bt device manager.
 *  return #BT_STATUS_SUCCESS, if one profile service can deal with this handler.
 *  return #BT_STATUS_FAIL, if one profile service can't deal with this handler.
 */
typedef bt_status_t (*bt_device_manager_power_callback_t)(bt_device_manager_power_event_t evt, bt_device_manager_power_status_t status, void *data, uint32_t data_length);

#define BT_DEVICE_MANAGER_POWER_RESET_PROGRESS_MEDIUM       (0x00)  /**< The progress of power reset in power off complete. */
#define BT_DEVICE_MANAGER_POWER_RESET_PROGRESS_COMPLETE     (0x01)  /**< The progress of power reset in power on complete. */
typedef uint8_t bt_device_manager_power_reset_progress_t; /**< The progress of power reset type. */

/**
 *  @brief Define the callback type of device manager power event handler for bt device manager.
 *  return #BT_STATUS_SUCCESS, if one profile service can deal with this handler.
 *  return #BT_STATUS_FAIL, if one profile service can't deal with this handler.
 */
typedef bt_status_t (*bt_device_manager_power_reset_callback_t)(bt_device_manager_power_reset_progress_t type, void *user_data);

#define BT_DEVICE_MANAGER_POWER_STATE_STANDBY               (0x00)  /**< The device manager power standby state. */
#define BT_DEVICE_MANAGER_POWER_STATE_STANDBY_PENDING       (0x01)  /**< The device manager power standby pending state. */
#define BT_DEVICE_MANAGER_POWER_STATE_ACTIVE                (0x02)  /**< The device manager power active state. */
#define BT_DEVICE_MANAGER_POWER_STATE_ACTIVE_PENDING        (0x03)  /**< The device manager power active pending state. */
#define BT_DEVICE_MANAGER_POWER_STATE_RESTING               (0x04)  /**< The device manager power resting state. */
typedef uint8_t bt_device_manager_power_state_t;

/**
* @brief   This function is used for BLE service and classic BT service to set their power state to device manager module.
* @param[in] type            the type of bluetooth device, please refer to #bt_device_type_t.
* @param[in] power_state the current power state of the indicated device type, please refer to #bt_device_manager_power_state_t.
* @return              #BT_STATUS_SUCCESS, the operation was successful.
*/
bt_status_t bt_device_manager_dev_set_power_state(bt_device_type_t type, bt_device_manager_power_state_t power_state);

/**
* @brief   This function is used for BLE service and classic BT service to register their handle callback.
* @param[in] type      the type of bluetooth device, please refer to #bt_device_type_t.
* @param[in] cb         the callback of prepare standby and active, please refer #bt_device_manager_power_callback_t.
* @return              #BT_STATUS_SUCCESS, the operation was successful.
*/
bt_status_t bt_device_manager_dev_register_callback(bt_device_type_t type, bt_device_manager_power_callback_t cb);

/**
* @brief   This function is used for user to register device manager event callback.
* @param[in] cb         the callback of device manager event, please refer #bt_device_manager_power_callback_t.
* @return              #BT_STATUS_SUCCESS, the operation was successful.
*                           #BT_STATUS_FAIL, if the callback had registered full.
*/
bt_status_t bt_device_manager_register_callback(bt_device_manager_power_callback_t cb);

/**
* @brief   This function is used for user to deregister device manager event callback.
* @param[in] cb         the callback of device manager event, please refer #bt_device_manager_power_callback_t.
* @return              #BT_STATUS_SUCCESS, the operation was successful.
*/
bt_status_t bt_device_manager_deregister_callback(bt_device_manager_power_callback_t cb);

/**
* @brief   This function is used to power active the indicated device type.
* @param[in] type      the type of bluetooth device, please refer to #bt_device_type_t.
* @return              #BT_STATUS_SUCCESS, the operation was successful.
*                           #BT_STATUS_FAIL, if the parameter mistake.
*/
bt_status_t bt_device_manager_power_active(bt_device_type_t type);

/**
* @brief   This function is used to power standby the indicated device type.
* @param[in] type      the type of bluetooth device, please refer to #bt_device_type_t.
* @return              #BT_STATUS_SUCCESS, the operation was successful.
*                           #BT_STATUS_FAIL, if the parameter mistake.
*/
bt_status_t bt_device_manager_power_standby(bt_device_type_t type);

/**
* @brief   This function is used to power reset the bluetooth.
* @param[in] type      the reset type, please refer to #bt_device_manager_power_reset_t.
* @param[in] cb         the reset callback, please refer to #bt_device_manager_power_reset_callback_t.
* @param[in] userdata the user data with the reset callback.
* @return              #BT_STATUS_SUCCESS, the operation was successful.
*                           #BT_STATUS_FAIL, the BLE and classic BT in state of prepare to standby.
*                           #BT_STATUS_PENDING, the power reset is ongoing.
*/
bt_status_t bt_device_manager_power_reset(bt_device_manager_power_reset_t type, bt_device_manager_power_reset_callback_t cb, void* user_data);

/**
* @brief   This function is used to get the power state of indicated bluetooth device type.
* @param[in] type      the type of bluetooth device, please refer to #bt_device_type_t.
* @return                please refer to #bt_device_manager_power_state_t.
*/
bt_device_manager_power_state_t bt_device_manager_power_get_power_state(bt_device_type_t type);

#ifdef __cplusplus
}
#endif

#endif /*__BT_DEVICE_MANAGER_POWER_H__*/


