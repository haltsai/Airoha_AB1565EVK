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

#ifndef __SPP_AIR_INTERFACE_H__
#define __SPP_AIR_INTERFACE_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_SPPAIR BT SPPAIR
 * @{
 * The SPPAIR is a Bluetooth service which can help the AIR application to communicate with the peer devices by SPP.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b UUID                      | A Universally Unique Identifier. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">Wikipedia</a>. |
 * |\b SPP                        | Serial Port Profile. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_profiles#Serial_Port_Profile_.28SPP.29">Wikipedia</a>. |
 * |\b SDP                       | Service Discovery Protocol. For more information, please refer to <a href="https://en.wikipedia.org/wiki/List_of_Bluetooth_protocols#Service_discovery_protocol_.28SDP.29">Wikipedia</a>. |
 *
 * @section spp_air_api_usage How to use this module
 * - Step 1. Call #bt_spp_air_main() to initialize the SPPAIR module. It is better to invoke this function during the system bootup and it should be called one time in a project.
 *  - Sample Code:
 *     @code
 *                //call the enter function of SPPAIR on the initialization procedure of the project.
 *                bt_spp_air_main();
 *     @endcode
 * - Step 2. Call #spp_air_init() to register an event handler of your application to listen to the events defined by #spp_air_event_t.
 *  - Sample Code:
 *     @code
 *                spp_air_init(serial_port_spp_air_event_callback);
 *                static void serial_port_spp_air_event_callback(spp_air_event_t event_id, void *param) {
 *                    case SPP_AIR_CONNECT_IND: {
 *                        // Save the connection info.
 *                        break;
 *                    }
 *                    case SPP_AIR_DISCONNECT_IND: {
 *                        // Delete the connection info.
 *                        break;
 *                    }
 *                    case SPP_AIR_RECIEVED_DATA_IND: {
 *                        // Save the data.
 *                        break;
 *                    }
 *                    case SPP_AIR_READY_TO_SEND_IND: {
 *                        // Resend the TX data when OOM.
 *                        break;
 *                    }
 *                }
 *     @endcode
 * - Step 3. Call #spp_air_write_data() and #spp_air_read_data() to send data and read data.
 *  - Sample Code:
 *     @code
 *                real_send_size = spp_air_write_data(conn_handle, write_data, write_data_length);
 *                real_read_size = spp_air_read_data(conn_handle, read_data, read_data_length);
 *     @endcode
 */

#include "bt_type.h"

/**
 * @defgroup SPP_AIR_define Define
 * @{
 * This section defines the macros for the SPP_AIR.
 */

/**
 * @brief An invalid value for the SPP_AIR handle.
 */
#define SPP_AIR_INVALID_HANDLE 0x00000000

/**
 * @brief The maximum number of upper users that the SPP_AIR service can support.
 */
#define SPP_AIR_SUPPORT_CB_MAX_NUM (1)

/**
 * @brief The event type of SPP_AIR.
 */
#define SPP_AIR_CONNECT_IND            0x00 /**< This event is generated when SPP AIR Service is connected. */
#define SPP_AIR_DISCONNECT_IND         0x01 /**< This event is generated when SPP AIR Service is disconnected. */
#define SPP_AIR_RECIEVED_DATA_IND      0x02 /**< This event is generated when data is received from the remote device. */
#define SPP_AIR_READY_TO_SEND_IND      0x03 /**< This event is generated when there is enough space in the TX buffer for sending data. */
typedef uint8_t spp_air_event_t; /**< The event type of SPP AIR Service. */

/**
 * @brief The status type of SPP_AIR.
 */
#define SPP_AIR_STATUS_SUCCESS               0x00   /**< Operation success. */
#define SPP_AIR_STATUS_FAIL                 -0x01   /**< Operation fail. */
#define SPP_AIR_STATUS_INVALID_HANDLE       -0x02   /**< The connection handle is invalid. */
#define SPP_AIR_STATUS_INVALID_UUID         -0x03   /**< The UUID is invalid. */
#define SPP_AIR_STATUS_OUT_OF_MEMORY        -0x04   /**< Not enough memory for the operation. */
#define SPP_AIR_STATUS_TX_NOT_AVAILABLE     -0x05   /**< The packet can not be sent. */
typedef int32_t spp_air_status_t; /**< The status type of SPP AIR Service. */

/**
 * @}
 */


/**
 * @defgroup SPP_AIR_struct Struct
 * @{
 * This section defines the structures for the SPP_AIR.
 */

/**
 * @brief This structure defines #SPP_AIR_CONNECT_IND event.
 */
typedef struct {
    uint32_t handle;              /**< The handle of the current connection. */
    uint16_t max_packet_length;   /**< The maximum length of a TX packet of the current SPP connection. */
    bt_bd_addr_t address;         /**< The Bluetooth address of the remote device. */
} spp_air_connect_ind_t;

/**
  * @brief This structure defines #SPP_AIR_DISCONNECT_IND event.
  */
typedef struct {
    uint32_t handle;              /**< The handle of the current connection. */
} spp_air_disconnect_ind_t;

/**
 * @brief This structure defines #SPP_AIR_READY_TO_SEND_IND event.
 */
typedef struct {
    uint32_t handle;              /**< The handle of the current connection. */
} spp_air_ready_to_send_ind_t;

/**
 * @brief This structure defines #SPP_AIR_RECIEVED_DATA_IND event.
 */
typedef struct {
    uint32_t handle;              /**< The handle of the current connection. */
    uint8_t *packet;              /**< The packet is received from the remote device.*/
    uint16_t packet_length;       /**< The length of the received packet.*/
} spp_air_data_received_ind_t;

/**
 *  @brief SPP_AIR event handler type.
 */
typedef void(* spp_air_notify_callback)(spp_air_event_t event_id, void *param);

/**
 * @}
 */

BT_EXTERN_C_BEGIN

/**
 * @brief   This function is for initializing the SPP_AIR module. It is better to invoke this function during bootup and it should be called one time in a project.
 * @return     None.
 */
void bt_spp_air_main(void);

/**
 * @brief   This function is for the upper user to register an event handler to handle #spp_air_event_t.
 * @param[in] callback      is a pointer for the callback function to be registered.
 * @return              #SPP_AIR_STATUS_SUCCESS, if the operation is successful.
 *                      #SPP_AIR_STATUS_FAIL, if the operation failed.
 */
spp_air_status_t spp_air_init(spp_air_notify_callback callback);


/**
 * @brief     This function is for the upper user to deregister the callback from the SPP_AIR module.
 * @param[in] callback      is a pointer to the callback function to be deregistered.
 * @return                  #SPP_AIR_STATUS_SUCCESS, if the operation is successful.
 *                          #SPP_AIR_STATUS_FAIL, if the operation failed.
 */
spp_air_status_t spp_air_deinit(spp_air_notify_callback callback);

/**
 * @brief     This function is for the spp_air client to initiate a connection to a remote server, the #SPP_AIR_CONNECT_IND event is reported to
 *                              indicate the result of the connection. Note that this API can only be used by an spp_air client.
 * @param[out] handle           is the handle of the current connection.
 * @param[in] address           is the Bluetooth address of the remote device.
 * @param[in] uuid128           is a 128-bit UUID of remote server. If set to NULL, it will use default UUID {0x00, 0x00, 0x00, 0x00,
                                0xDE, 0xCA, 0xFA, 0xDE, 0xDE, 0xCA, 0xDE, 0xAF, 0xDE, 0xCA, 0xCA, 0xFE} to connect remote server.
 * @return                      #SPP_AIR_STATUS_SUCCESS, the operation completed successfully.
 *                              #SPP_AIR_STATUS_FAIL, the operation failed.
 *                              #SPP_AIR_STATUS_INVALID_UUID, the UUID is invalid.
 */
spp_air_status_t spp_air_connect(uint32_t *handle, const bt_bd_addr_t *address, const uint8_t *uuid128);

/**
 * @brief     This function is for the spp_air client to disconnect the existing connection, the #SPP_AIR_DISCONNECT_IND event is reported
 *                      to indicate the result of the disconnection.
 * @param[in] handle    is the handle of the current connection.
 * @return              #SPP_AIR_STATUS_SUCCESS, the operation completed successfully.
 *                      #SPP_AIR_STATUS_FAIL, the operation failed.
 *                      #SPP_AIR_STATUS_INVALID_HANDLE, the handle is invalid.
 */
spp_air_status_t spp_air_disconnect(uint32_t handle);

/**
 * @brief     This function is for the upper user to send data.
 * @param[in] handle         is the handle of the current connection.
 * @param[in] data           is a pointer to the packet to be sent.
 * @param[in] data_size      is the length of the packet to be sent.
 * @return                   the length of data which has been sent successfully.
 */
uint32_t spp_air_write_data(uint32_t handle, uint8_t *data, uint16_t data_size);

/**
 * @brief    This function is for the upper user to read data from the cache buffer of the SPP_AIR.
 * @param[in] handle        is the handle of the current connection.
 * @param[out] data          is a pointer of the buffer to save the data.
 * @param[in] data_size     is the length of the buffer that save the data.
 * @return                  the length of data which has been read successfully.
 */
uint32_t spp_air_read_data(uint32_t handle, uint8_t *data, uint16_t data_size);

/**
 *  @brief    This function is for the upper user to get the available space of the cache buffer of the SPP_AIR.
*  @param[in] handle         is the handle of the current connection.
*  @return                   the length of available space.
*/
uint32_t spp_air_get_rx_available(uint32_t handle);

/**
*  @brief     This function is for the upper user to get the UUID that being used now.
*  @param[out]  uuid128    SPP UUID, 16 byte only.
*  @return                 #SPP_AIR_STATUS_SUCCESS, the operation completed successfully.
*                          #SPP_AIR_STATUS_FAIL, the operation failed.
*                          #SPP_AIR_STATUS_OUT_OF_MEMORY, OOM error.
*/
spp_air_status_t spp_air_get_uuid(uint8_t *uuid128);

/**
*  @brief     this function is for the upper user to set the UUID of SPP_AIR.
*  @param[in] index    an index of NVkey to decide which UUID will be using. 0: standard SPP UUID, 1: SPP AIR UUID(default), 2: customization
*  @return             #SPP_AIR_STATUS_SUCCESS, the operation completed successfully.
*                      #SPP_AIR_STATUS_FAIL, the operation failed.
*                      #SPP_AIR_STATUS_OUT_OF_MEMORY, OOM error.
*/
spp_air_status_t spp_air_set_uuid_index(uint8_t index);


BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif /*__SPP_AIR_INTERFACE_H__*/

