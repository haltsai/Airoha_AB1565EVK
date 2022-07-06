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

#ifndef __BT_DEVICE_MANAGER_TEST_MODE_H__
#define __BT_DEVICE_MANAGER_TEST_MODE_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothService_DM Test Mode
 * @{
 * This section provides APIs to manage Bluetooth device test mode.
 *
 * @section device_manager_test_mode_api_usage How to use this module
 * - Step 1. Initialize the device manager and register event notify callback at boot up.
 *  - Sample Code:
 *     @code
 *               bt_device_manager_init();
 *               bt_device_manager_test_mode_notify_callback_t cb;
 *               bt_device_manager_test_mode_register_callback(cb);
 *     @endcode
 * - Step 2. Use device manager test mode API to get the device test mode.
 *  - Sample code:
 *     @code
 *               bt_device_manager_test_mode_t mode;
 *               mode = bt_device_manager_get_test_mode();
 *               printf("bt device mode: %d", mode);
 *     @endcode
 * - Step 3. Use device manager test mode API to set the device test mode.
 *  - Sample code:
 *     @code
 *               bt_device_manager_test_mode_t mode = BT_DEVICE_MANAGER_TEST_MODE_RELAY;
 *               bt_device_manager_set_test_mode(mode);
 *     @endcode
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bt_type.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @defgroup BT_Device_Manager_Service_define Define
 * @{
 * Define Bluetooth device manager test mode service data types and values.
 */

#define BT_DEVICE_MANAGER_TEST_MODE_NONE              0x00    /**< This mode is the default mode when the Bluetooth power on. */
#define BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX           0x01    /**< This mode is used to test the normal function of the device with the DUT mode of Bluetooth controller enabled. */
#define BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY          0x02    /**< This mode is used for Bluetooth controller RF test. */
#define BT_DEVICE_MANAGER_TEST_MODE_RELAY             0x03    /**< This mode is used for Bluetooth controller debug. In this mode, Bluetooth device connects with the debug tool via UART or USB. Bluetooth host receives HCI CMD or ACL raw data from the debug tool, and then relays them to the Bluetooth controller. The Bluetooth host will also send the HCI EVT or ACL raw data that are feedbacked from the Bluetooth controller back to the debug tool. */
#define BT_DEVICE_MANAGER_TEST_MODE_COMMAND           0x04    /**< This mode is used for Bluetooth controller signal test. In this mode, Bluetooth device connects with the signal test tool via UART or USB. The signal test tool sends the signal test HCI CMD raw data to Bluetooth host (such as TX/RX). Then Bluetooth host will pass those HCI CMD through to Bluetooth controller. */
typedef uint8_t bt_device_manager_test_mode_t;                /**< The device test mode. */

#define BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DISABLED            0x00    /**< The DUT mode is disabled. */
#define BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED     0x01    /**< The DUT mix mode is enabled. */
#define BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_ONLY_ENABLED    0x02    /**< The DUT only mode is enabled. */
#define BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_ACTIVE              0x03    /**< The DUT mode is active. When the Bluetooth device is connected by the RF tester, if Bluetooth device receives the LMP test command from the RF tester over the air interface, the DUT mode of the device will become active. For more information please refer to <a href="https://www.bluetooth.org/DocMan/handlers/DownloadDoc.ashx?doc_id=286439&_ga=1.241778370.1195131350.1437993589">Bluetooth core specification version 4.2 [Vol 3, Part D]</a> */
typedef uint8_t bt_device_manager_test_mode_dut_state_t; /**< The state of DUT mode. */

#define BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND        0x00    /**< BT device manager test mode changed event. */
typedef uint8_t bt_device_manager_test_mode_event_t;          /**< The type of test mode change event. */

/**
 *  @brief This structure defines the #BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND.
 */
typedef struct {
    bt_device_manager_test_mode_t mode;                /**< The mode changes to. */
    bt_device_manager_test_mode_dut_state_t dut_state; /**< State of DUT mode, only valid in #BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX and #BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY. */
    bt_status_t status;                                /**< Test mode change status. */
} bt_device_manager_test_mode_changed_ind_t;

/**
 *  @brief Define the callback type of mode change handler for bt device manager test mode.
 */
typedef void (*bt_device_manager_test_mode_notify_callback_t)(bt_device_manager_test_mode_event_t event_id, void *param);

/**
 * @}
 */

/**
 * @brief   This function used to register mode change event notify callback into bt device manager test mode.
 * @param[in] callback is the callback sets registered to bt device manager test mode.
 * @return        #BT_STATUS_SUCCESS, execute success.
 *                #BT_STATUS_FAIL, the parameter passed is invalid or this callback has been registered before.
 */

bt_status_t bt_device_manager_test_mode_register_callback(bt_device_manager_test_mode_notify_callback_t callback);

/**
 * @brief   This function used to deregister mode change event notify callback from bt device manager test mode.
 * @param[in] callback is the callback registered to bt device manager test mode.
 * @return        #BT_STATUS_SUCCESS, execute success.
 *                #BT_STATUS_FAIL, the parameter passed is invalid or this callback has not been registered before.
 */

bt_status_t bt_device_manager_test_mode_deregister_callback(bt_device_manager_test_mode_notify_callback_t callback);

/**
 * @brief     This function is used to set the device to test mode.
 * @param[in]     mode is the mode to set to.
 * @return        #BT_STATUS_SUCCESS, execute success.
 *                #BT_STATUS_FAIL, if an error occurred.
 *                #BT_STATUS_PENDING, the operation is ongoing.
 */
bt_status_t bt_device_manager_set_test_mode(bt_device_manager_test_mode_t mode);

/**
 * @brief         This function is used to get the Bluetooth device test mode.
 * @return         #BT_DEVICE_MANAGER_TEST_MODE_NONE, this mode is the default mode when the Bluetooth powers on.
 *                 #BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX, this mode is default mode with DUT enabled.
 *                 #BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY, this mode is pure DUT enabled mode. The device is waiting for RF tester connection.
 *                 #BT_DEVICE_MANAGER_TEST_MODE_RELAY, this mode is used for Bluetooth controller debugging. The firmware should forbid entering RTC mode, and power saving mode.
 *                 #BT_DEVICE_MANAGER_TEST_MODE_COMMAND, this mode is used for the Bluetooth controller signal test. The firmware should forbid entering RTC mode, and power saving mode.
 */
bt_device_manager_test_mode_t bt_device_manager_get_test_mode(void);

/**
 * @brief         This function is used to get the state of both #BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX and #BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY.
 * @return         #BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DISABLED, DUT mode is disabled.
 *                 #BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED, DUT mix mode is enabled. The behavior of the device is the same as in #BT_DEVICE_MANAGER_TEST_MODE_NONE.
 *                 #BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_ONLY_ENABLED, DUT only mode is enabled. The device is waiting for RF tester connection, the firmware should disable below functions:
 *                 1.BT connection manager:
 *                   (1)Disable the role recovery.
 *                   (2)Forbid to reconnect devices that connected before.
 *                   (3)Close the AWS LS.
 *                 2.BT Application:
 *                   (1)Forbid to close the visibility of Bluetooth automatically.
 *                   (2)Forbid to enter power saving mode.
 *                   (3)Forbid to enter RTC mode.
 *                   (4)Disable BLE advertising.
 *                   (5)Disable air pairing.
 *                 #BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_ACTIVE, DUT mode is active. The device has connected with the RF tester, the behavior of device is the same as in #BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_ONLY_ENABLED.
 */
bt_device_manager_test_mode_dut_state_t bt_device_manager_test_mode_get_dut_state(void);

/**
 * @}
 */
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* __BT_DEVICE_MANAGER_TEST_MODE_H__ */

