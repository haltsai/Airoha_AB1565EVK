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

#ifndef __AIRUPDATE_INTERFACE_H__
#define __AIRUPDATE_INTERFACE_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_AIRUPDATE BT AIRUPDATE
 * @{
 * The AIRUPDATE is a Bluetooth service which can help the AIR application to communicate with peer devices by a L2CAP fixed channel.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b UUID                      | A Universally Unique Identifier. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Universally_unique_identifier">Wikipedia</a>. |
 *
 * @section airupdate_api_usage How to use this module
 * - Step 1. Call @ref airupdate_main() to initialize the AIRUPDATE module. It is better to invoke this function during the system bootup and it should be called only one time in a project.
 *  - Sample Code:
 *     @code
 *                //call the enter function of AIRUPDATE on the initialization procedure of the project.
 *                #ifdef MTK_AIRUPDATE_ENABLE
 *                    airupdate_main();
 *                #endif
 *     @endcode
 * - Step 2. Call @ref airupdate_init() to register an event handler of your application to listen to the events defined by #airupdate_event_t.
 *  - Sample Code:
 *     @code
 *                static void serial_port_airupdate_event_callback(airupdate_event_t event_id, void *param);
 *                airupdate_init(serial_port_airupdate_event_callback);
 *                static void serial_port_airupdate_event_callback(airupdate_event_t event_id, void *param) {
 *                    case AIRUPDATE_CONNECT_IND: {
 *                        // Save the connection info.
 *                        break;
 *                    }
 *                    case AIRUPDATE_DISCONNECT_IND: {
 *                        // Delete the connection info.
 *                        break;
 *                    }
 *                    case AIRUPDATE_RECIEVED_DATA_IND: {
 *                        // Save the data.
 *                        break;
 *                    }
 *                    case AIRUPDATE_READY_TO_SEND_IND: {
 *                        // resend the tx data when OOM.
 *                        break;
 *                    }
 *                }
 *     @endcode
 * - Step 3. Call @ref airupdate_write_data() and @ref airupdate_read_data() to send data and read data.
 *  - Sample Code:
 *     @code
 *                uint32_t real_send_size, real_read_size;
 *                real_send_size = airupdate_write_data(conn_handle, write_data, write_data_length);
 *                real_read_size = airupdate_read_data(conn_handle, read_data, read_data_length);
 *     @endcode
 */

//#ifdef MTK_AIRUPDATE_ENABLE
#include "bt_type.h"

/**
 * @defgroup AIRUPDATE_define Define
 * @{
 * This section defines the macros for the AIRUPDATE.
 */

/**
 * @brief An invalid value for the AIRUPDATE handle.
 */
#define AIRUPDATE_INVALID_HANDLE 0x00000000

/**
 * @brief The maximum number of upper users that the AIRUPDATE service can support.
 */
#define AIRUPDATE_SUPPORT_CB_MAX_NUM (1)

/**
 * @brief The event type of AIRUPDATE.
 */
#define AIRUPDATE_CONNECT_IND            0x00 /**< This event is generated when AIRUPDATE Service is connected. */
#define AIRUPDATE_DISCONNECT_IND         0x01 /**< This event is generated when AIRUPDATE Service is disconnected. */
#define AIRUPDATE_RECIEVED_DATA_IND      0x02 /**< This event is generated when data is received from the remote device. */
#define AIRUPDATE_READY_TO_SEND_IND      0x03 /**< This event is generated when there is enough space in the TX buffer for sending data. */
typedef uint8_t airupdate_event_t; /**< The event type of AIRUPDATE Service. */

/**
 * @brief The status type of AIRUPDATE.
 */
#define AIRUPDATE_STATUS_SUCCESS               0x00   /**< Operation is successful. */
#define AIRUPDATE_STATUS_FAIL                 -0x01   /**< Operation failed. */
#define AIRUPDATE_STATUS_INVALID_HANDLE       -0x02   /**< The connection handle is invalid. */
#define AIRUPDATE_STATUS_OUT_OF_MEMORY        -0x03   /**< No Memory for the operation. */
typedef int32_t airupdate_status_t; /**< The status type of AIRUPDATE Service. */
/**
 * @}
 */


/**
 * @defgroup AIRUPDATE_struct Struct
 * @{
 * This section defines the structures for the AIRUPDATE.
 */

/** @brief This structure defines #AIRUPDATE_CONNECT_IND. */
typedef struct {
    uint32_t handle;              /**< The connection handle of the current connection. */
    uint16_t max_packet_length;   /**< The maximum length of a TX packet of the current connection. */
    bt_bd_addr_t address;         /**< The Bluetooth address of the remote device. */
} airupdate_connect_ind_t;

/** @brief This structure defines #AIRUPDATE_DISCONNECT_IND. */
typedef struct {
    uint32_t handle;              /**< The handle of the current connection. */
    uint32_t reason;              /**< The reason for the disconnection. */
} airupdate_disconnect_ind_t;

/** @brief This structure defines #AIRUPDATE_READY_TO_SEND_IND. */
typedef struct {
    uint32_t handle;              /**< The connection handle of the current connection. */
} airupdate_ready_to_send_ind_t;

/** @brief This structure defines #AIRUPDATE_RECIEVED_DATA_IND. */
typedef struct {
    uint32_t handle;              /**< The connection handle of the current connection. */
    uint8_t *packet;              /**< The packet is received from the remote device.*/
    uint16_t packet_length;       /**< The length of the received packet.*/
} airupdate_data_received_ind_t;

/**
 *  @brief AIRUPDATE event handler type.
 */
typedef void(* airupdate_notify_callback)(airupdate_event_t event_id, void *param);

/**
 * @}
 */

BT_EXTERN_C_BEGIN

/**
 * @brief   This function is for initializing the AIRUPDATE module. It is better to invoke this function during bootup and it should be called only one time in a project.
 * @return     None.
 */
void airupdate_main(void);

/**
 * @brief   This function is for the upper user to register an event handler to handle #airupdate_event_t.
 * @param[in] callback      is a pointer for the callback function to be registered.
 * @return                  #AIRUPDATE_STATUS_SUCCESS, if the operation is successful.
 *                          #AIRUPDATE_STATUS_FAIL, if the operation failed.
 */
airupdate_status_t airupdate_init(airupdate_notify_callback callback);

/**
 * @brief     This function is for the upper user to deregister the callback from the AIRUPDATE module.
 * @param[in] callback      is a pointer to the callback function to be deregistered.
 * @return                  #AIRUPDATE_STATUS_SUCCESS, if the operation is successful.
 *                          #AIRUPDATE_STATUS_FAIL, if the operation failed.
 */
airupdate_status_t airupdate_deinit(airupdate_notify_callback callback);

/**
 * @brief     This function is for the upper user to send data.
 * @param[in] handle         is the handle of the current connection.
 * @param[in] data           is a pointer to the packet to be sent.
 * @param[in] data_size      is the length of the packet to be sent.
 * @return                   the length of data which has been sent successfully.
 */
uint32_t airupdate_write_data(uint32_t handle, uint8_t *data, uint16_t data_size);

/**
 * @brief    This function is for the upper user to read data from the cache buffer of the AIRUPDATE.
 * @param[in] handle         is the handle of the current connection.
 * @param[out] data          is a pointer of the buffer to save the data.
 * @param[in] data_size      is the length of the buffer that save the data.
 * @return                   the length of data which has been read successfully.
 */
uint32_t airupdate_read_data(uint32_t handle, uint8_t *data, uint16_t data_size);

/**
 *  @brief    This function is for the upper user to get the available space of the cache buffer of the AIRUPDATE.
*  @param[in] handle         is the handle of the current connection.
*  @return                   the length of available space.
*/
uint32_t airupdate_get_rx_available(uint32_t handle);


BT_EXTERN_C_END
//#endif /*MTK_AIRUPDATE_ENABLE*/

/**
 * @}
 * @}
 */

#endif /*__AIRUPDATE_INTERFACE_H__*/

