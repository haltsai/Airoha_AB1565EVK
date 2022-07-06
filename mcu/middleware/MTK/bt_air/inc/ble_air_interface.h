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

#ifndef __BLE_AIR_INTERFACE_H__
#define __BLE_AIR_INTERFACE_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_BLEAIR BT BLEAIR
 * @{
 * The BLEAIR is a Bluetooth service which can help the AIR application to communicate with the peer devices by BLE.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b UUID                      | A Universally Unique Identifier. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">Wikipedia</a>. |
 *
 * @section ble_air_api_usage How to use this module
 * - Step 1. Call #ble_air_main() to initialize the BLEAIR module. It is better to invoke this function during the system bootup and it should be called only one time in a project.
 *  - Sample Code:
 *     @code
 *                //call the enter function of BLEAIR on the initialization procedure of the project.
 *                ble_air_main();
 *     @endcode
 * - Step 2. Call #ble_air_init() to register an event handler of your application to listen to the events defined by #ble_air_event_t.
 *  - Sample Code:
 *     @code
 *                ble_air_init(serial_port_ble_air_event_callback);
 *                static void serial_port_ble_air_event_callback(ble_air_event_t event_id, void *param) {
 *                    case BLE_AIR_EVENT_CONNECT_IND: {
 *                        // Save the connection info.
 *                        break;
 *                    }
 *                    case BLE_AIR_EVENT_DISCONNECT_IND: {
 *                        // Delete the connection info.
 *                        break;
 *                    }
 *                    case BLE_AIR_EVENT_READY_TO_READ_IND: {
 *                        // Read the data.
 *                        break;
 *                    }
 *                    case BLE_AIR_EVENT_READY_TO_WRITE_IND: {
 *                        // Send the data.
 *                        break;
 *                    }
 *                }
 *     @endcode
 * - Step 3. Call #ble_air_write_data() and #ble_air_read_data() to send data and read data.
 *  - Sample Code:
 *     @code
 *                real_send_size = ble_air_write_data(conn_handle, write_data, write_data_length);
 *                real_read_size = ble_air_read_data(conn_handle, read_data, read_data_length);
 *     @endcode
 */

#include <stdint.h>
#include <stdbool.h>
#include "bt_type.h"
#include "bt_gatts.h"

/**
 * @defgroup BLE_AIR_define Define
 * @{
 * This section defines the macros for the BLE_AIR.
 */
/**
 * @brief A feature option to enable/disable the low power function of BLE_AIR module.
 */
//#define BLE_AIR_LOW_POWER_CONTROL

/**
 * @brief The maximum number of BLE connections that the BLE_AIR service can support.
 */
#ifndef BT_CONNECTION_MAX
#define BT_CONNECTION_MAX    (4)
#endif

/**
 * @brief The deault value of the BLE_AIR service RX buffer.
 */
#define BLE_AIR_RECEIVE_BUFFER_SIZE       (1024)

/**
 * @brief The maximum number of upper users that the BLE_AIR service can support.
 */
#define BLE_AIR_SUPPORT_CB_MAX_NUM (1)

/**
 * @brief The event type of BLE_AIR.
 */
#define BLE_AIR_EVENT_CONNECT_IND          0  /**< This event is generated when BLE AIR Service is connected. */
#define BLE_AIR_EVENT_DISCONNECT_IND       1  /**< This event is generated when BLE AIR Service is disconnected. */
#define BLE_AIR_EVENT_READY_TO_READ_IND    2  /**< This event is generated when RX data is received. */
#define BLE_AIR_EVENT_READY_TO_WRITE_IND   3  /**< This event is generated when TX characteristic is configured. */
typedef uint16_t ble_air_event_t;        /**< The event type of BLE AIR Service. */

/**
 * @brief The type of the remote device.
 */
#define BLE_AIR_REMOTE_DEVICE_ANDROID      0  /**< The device type of remote device is Android. */
#define BLE_AIR_REMOTE_DEVICE_IOS          1  /**< The device type of remote device is iOS. */
typedef uint8_t ble_air_remote_device_type_t;  /**< The device type of remote device. */


/**
 * @brief The status type of BLE_AIR.
 */
#define BLE_AIR_STATUS_OK                    0x00   /**< The BLE AIR Service function is executed successfully. */
#define BLE_AIR_STATUS_FAIL                 -0x01   /**< The BLE AIR Service function failed. */
#define BLE_AIR_STATUS_INVALID_PARAMETER    -0x02   /**< The BLE AIR Service parameters are invalid. */
#define BLE_AIR_STATUS_UNINITIALIZED        -0x03   /**< The BLE AIR Service device is uninitialized. */
#define BLE_AIR_STATUS_BUSY                 -0x04   /**< The BLE AIR Service device is busy. */
typedef int32_t ble_air_status_t; /**< The status type of BLE AIR Service. */

/**
 * @}
 */

/**
 * @defgroup BLE_AIR_struct Struct
 * @{
 * This section defines the structures for the BLE_AIR.
 */

/** @brief This structure defines #BLE_AIR_EVENT_CONNECT_IND. */
typedef struct {
    bt_bd_addr_t   bdaddr;          /**< The BLE address of the remote device. */
    uint16_t       conn_handle;     /**< The handle of the current BLE connection. */
} ble_air_connect_t;

/** @brief This structure defines #BLE_AIR_EVENT_DISCONNECT_IND. */
typedef struct {
    bt_bd_addr_t   bdaddr;          /**< The BLE address of the remote device. */
    uint16_t       conn_handle;     /**< The handle of the current BLE connection. */
} ble_air_disconnect_t;

/** @brief This structure defines #BLE_AIR_EVENT_READY_TO_READ_IND. */
typedef struct {
    bt_bd_addr_t    bdaddr;         /**< The BLE address of the remote device. */
    uint16_t        conn_handle;    /**< The handle of the current BLE connection. */
} ble_air_ready_to_read_t;

/** @brief This structure defines #BLE_AIR_EVENT_READY_TO_WRITE_IND. */
typedef struct {
    bt_bd_addr_t    bdaddr;         /**< The BLE address of the remote device. */
    uint16_t        conn_handle;    /**< The handle of the current BLE connection. */
} ble_air_ready_to_write_t;


/**
 *  @brief BLE_AIR event handler type.
 */
typedef void (*ble_air_common_callback_t)(ble_air_event_t event, void *callback_param);

/**
 * @}
 */

BT_EXTERN_C_BEGIN

/**
 * @brief   This function is for initializing the BLE_AIR module. It is better to invoke this function during bootup and it should be called one time in a project.
 * @return     None.
 */
void ble_air_main(void);

/**
 * @brief   This function is for the upper user to register an event handler to handle #ble_air_event_t.
 * @param[in] app_callback      is a pointer for the callback function to be registered.
 * @return              #BLE_AIR_STATUS_SUCCESS, if the operation is successful.
 *                      #BLE_AIR_STATUS_FAIL, if the operation failed.
 */
ble_air_status_t ble_air_init(ble_air_common_callback_t app_callback);

/**
 * @brief     This function is for the upper user to deregister the callback from the BLE_AIR module.
 * @param[in] app_callback      is a pointer to the callback function to be deregistered.
 * @return               #BLE_AIR_STATUS_SUCCESS, if the operation is successful.
 *                       #BLE_AIR_STATUS_FAIL, if the operation failed.
 */
ble_air_status_t ble_air_deinit(ble_air_common_callback_t app_callback);

/**
 * @brief    This function is for the upper user to read data from the cache buffer of the BLE_AIR.
 * @param[in] conn_handle         is the handle of the current connection.
 * @param[out] buffer             is a pointer of the buffer to save the data.
 * @param[in] size                is the length of the buffer that saved the data.
 * @return                        the length of data which has been read successfully.
 */
uint32_t ble_air_read_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size);

/**
 * @brief     This function is for the upper user to send data.
 * @param[in] conn_handle         is the handle of the current connection.
 * @param[in] buffer              is a pointer to the packet to be sent.
 * @param[in] size                is the length of data which has been sent successfully.
 * @return                        the length of data which has been sent successfully.
 */
uint32_t ble_air_write_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size);

/**
 *  @brief    This function is for the upper user to get the available space of the RX buffer.
*  @param[in] conn_handle         is the handle of the current connection.
*  @return                        is the length of available space.
*/
uint32_t ble_air_get_rx_available(uint16_t conn_handle);


BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif /*__BLE_AIR_INTERFACE_H__*/
