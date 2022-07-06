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
 * CONTAINED IN AIROHA SOFTWARE. AIROHA SHALL ALSO NOT BE RESPONSIGATT_OVER_BREDR FOR ANY AIROHA
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND AIROHA'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO AIROHA SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT AIROHA'S OPTION, TO REVISE OR REPLACE AIROHA SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * AIROHA FOR SUCH AIROHA SOFTWARE AT ISSUE.
 */

#ifndef __GATT_OVER_BREDR_AIR_H__
#define __GATT_OVER_BREDR_AIR_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_GOBEAIR BT GOBEAIR
 * @{
 * The GATT_OVER_BREDRAIR is a Bluetooth service which can help the AIR application to communicate with the peer devices by GATT over BR/EDR.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b UUID                      | A Universally Unique Identifier. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">Wikipedia</a>. |
 *
 * @section gatt_over_bredr_air_api_usage How to use this module
 * - Step 1. Call #gatt_over_bredr_air_main() to initialize the GOBEAIR module. It is better to invoke this function during the system bootup and it should be called only one time in a project.
 *  - Sample Code:
 *     @code
 *                //call the enter function of GOBEAIR on the initialization procedure of the project.
 *                gatt_over_bredr_air_main();
 *     @endcode
 * - Step 2. Call #gatt_over_bredr_air_init() to register an event handler of your application to listen to the events defined by #gatt_over_bredr_air_event_t.
 *  - Sample Code:
 *     @code
 *                gatt_over_bredr_air_init(serial_port_gatt_over_bredr_air_event_callback);
 *                static void serial_port_gatt_over_bredr_air_event_callback(gatt_over_bredr_air_event_t event_id, void *param) {
 *                    case GATT_OVER_BREDR_AIR_EVENT_CONNECT_IND: {
 *                        // Save the connection info.
 *                        break;
 *                    }
 *                    case GATT_OVER_BREDR_AIR_EVENT_DISCONNECT_IND: {
 *                        // Delete the connection info.
 *                        break;
 *                    }
 *                    case GATT_OVER_BREDR_AIR_EVENT_READY_TO_READ_IND: {
 *                        // Read the data.
 *                        break;
 *                    }
 *                    case GATT_OVER_BREDR_AIR_EVENT_READY_TO_WRITE_IND: {
 *                        // Send the data.
 *                        break;
 *                    }
 *                }
 *     @endcode
 * - Step 3. Call #gatt_over_bredr_air_write_data() and #gatt_over_bredr_air_read_data() to send data and read data.
 *  - Sample Code:
 *     @code
 *                real_send_size = gatt_over_bredr_air_write_data(conn_handle, write_data, write_data_length);
 *                real_read_size = gatt_over_bredr_air_read_data(conn_handle, read_data, read_data_length);
 *     @endcode
 */

#include <stdint.h>
#include <stdbool.h>
#include "bt_type.h"
#include "bt_gatts.h"

/**
 * @defgroup GOBE_AIR_define Define
 * @{
 * This section defines the macros for the GOBE_AIR.
 */

/**
 * @brief The maximum number of GATT_OVER_BREDR connections that the GOBE_AIR service can support.
 */
#ifndef BT_CONNECTION_MAX
#define BT_CONNECTION_MAX    (4)
#endif

/**
 * @brief The deault value of the GOBE_AIR service RX buffer.
 */
#define GATT_OVER_BREDR_AIR_RECEIVE_BUFFER_SIZE       (1024 * 2)

/**
 * @brief The maximum number of upper users that the GATT_OVER_BREDR_AIR service can support.
 */
#define GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM (1)

/**
 * @brief The event type of GATT_OVER_BREDR_AIR.
 */
#define GATT_OVER_BREDR_AIR_EVENT_CONNECT_IND          0  /**< This event is generated when GATT_OVER_BREDR AIR Service is connected. */
#define GATT_OVER_BREDR_AIR_EVENT_DISCONNECT_IND       1  /**< This event is generated when GATT_OVER_BREDR AIR Service is disconnected. */
#define GATT_OVER_BREDR_AIR_EVENT_READY_TO_READ_IND    2  /**< This event is generated when RX data is received. */
#define GATT_OVER_BREDR_AIR_EVENT_READY_TO_WRITE_IND   3  /**< This event is generated when TX characteristic is configured. */
typedef uint16_t gatt_over_bredr_air_event_t;        /**< The event type of GATT_OVER_BREDR AIR Service. */

/**
 * @brief The status type of GATT_OVER_BREDR_AIR.
 */
#define GATT_OVER_BREDR_AIR_STATUS_OK                    0x00   /**< The GATT_OVER_BREDR AIR Service function is executed successfully. */
#define GATT_OVER_BREDR_AIR_STATUS_FAIL                 -0x01   /**< The GATT_OVER_BREDR AIR Service function failed. */
#define GATT_OVER_BREDR_AIR_STATUS_INVALID_PARAMETER    -0x02   /**< The GATT_OVER_BREDR AIR Service parameters are invalid. */
#define GATT_OVER_BREDR_AIR_STATUS_UNINITIALIZED        -0x03   /**< The GATT_OVER_BREDR AIR Service device is uninitialized. */
#define GATT_OVER_BREDR_AIR_STATUS_BUSY                 -0x04   /**< The GATT_OVER_BREDR AIR Service device is busy. */
typedef int32_t gatt_over_bredr_air_status_t; /**< The status type of GATT_OVER_BREDR AIR Service. */

/**
 * @}
 */

/**
 * @defgroup GATT_OVER_BREDR_AIR_struct Struct
 * @{
 * This section defines the structures for the GATT_OVER_BREDR_AIR.
 */

/** @brief This structure defines #GATT_OVER_BREDR_AIR_EVENT_CONNECT_IND. */
typedef struct {
    bt_bd_addr_t   bdaddr;          /**< The GATT_OVER_BREDR address of the remote device. */
    uint16_t       conn_handle;     /**< The handle of the current GATT_OVER_BREDR connection. */
    uint16_t       max_packet_length;   /**< The maximum length of a TX packet of the current GATT_OVER_BREDR connection. */
} gatt_over_bredr_air_connect_t;

/** @brief This structure defines #GATT_OVER_BREDR_AIR_EVENT_DISCONNECT_IND. */
typedef struct {
    bt_bd_addr_t   bdaddr;          /**< The GATT_OVER_BREDR address of the remote device. */
    uint16_t       conn_handle;     /**< The handle of the current GATT_OVER_BREDR connection. */
} gatt_over_bredr_air_disconnect_t;

/** @brief This structure defines #GATT_OVER_BREDR_AIR_EVENT_READY_TO_READ_IND. */
typedef struct {
    bt_bd_addr_t    bdaddr;         /**< The GATT_OVER_BREDR address of the remote device. */
    uint16_t        conn_handle;    /**< The handle of the current GATT_OVER_BREDR connection. */
} gatt_over_bredr_air_ready_to_read_t;

/** @brief This structure defines #GATT_OVER_BREDR_AIR_EVENT_READY_TO_WRITE_IND. */
typedef struct {
    bt_bd_addr_t    bdaddr;         /**< The GATT_OVER_BREDR address of the remote device. */
    uint16_t        conn_handle;    /**< The handle of the current GATT_OVER_BREDR connection. */
} gatt_over_bredr_air_ready_to_write_t;


/**
 *  @brief GATT_OVER_BREDR_AIR event handler type.
 */
typedef void (*gatt_over_bredr_air_common_callback_t)(gatt_over_bredr_air_event_t event, void *callback_param);

/**
 * @}
 */

BT_EXTERN_C_BEGIN

/**
 * @brief   This function is for the upper user to register an event handler to handle #gatt_over_bredr_air_event_t.
 * @param[in] app_callback      is a pointer for the callback function to be registered.
 * @return              #GATT_OVER_BREDR_AIR_STATUS_SUCCESS, if the operation is successful.
 *                      #GATT_OVER_BREDR_AIR_STATUS_FAIL, if the operation failed.
 */
gatt_over_bredr_air_status_t gatt_over_bredr_air_init(gatt_over_bredr_air_common_callback_t app_callback);

/**
 * @brief     This function is for the upper user to deregister the callback from the GATT_OVER_BREDR_AIR module.
 * @param[in] app_callback      is a pointer to the callback function to be deregistered.
 * @return               #GATT_OVER_BREDR_AIR_STATUS_SUCCESS, if the operation is successful.
 *                       #GATT_OVER_BREDR_AIR_STATUS_FAIL, if the operation failed.
 */
gatt_over_bredr_air_status_t gatt_over_bredr_air_deinit(gatt_over_bredr_air_common_callback_t app_callback);

/**
 * @brief    This function is for the upper user to read data from the cache buffer of the GATT_OVER_BREDR_AIR.
 * @param[in] conn_handle         is the handle of the current connection.
 * @param[out] buffer             is a pointer of the buffer to save the data.
 * @param[in] size                is the length of the buffer that saved the data.
 * @return                        the length of data which has been read successfully.
 */
uint32_t gatt_over_bredr_air_read_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size);

/**
 * @brief     This function is for the upper user to send data.
 * @param[in] conn_handle         is the handle of the current connection.
 * @param[in] buffer              is a pointer to the packet to be sent.
 * @param[in] size                is the length of data which has been sent successfully.
 * @return                        the length of data which has been sent successfully.
 */
uint32_t gatt_over_bredr_air_write_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size);

/**
 *  @brief    This function is for the upper user to get the availagatt_over_bredr space of the RX buffer.
*  @param[in] conn_handle         is the handle of the current connection.
*  @return                        is the length of available space.
*/
uint32_t gatt_over_bredr_air_get_rx_available(uint16_t conn_handle);


BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif /*__GATT_OVER_BREDR_AIR_H__*/

