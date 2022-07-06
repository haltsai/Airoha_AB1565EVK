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
* File: bt_ancs_common.h
*
* Description: The file defines the interface of bt_ancs_common.c.
*
*/
#ifndef __BT_ANCS_COMMON_H__
#define __BT_ANCS_COMMON_H__

#include "bt_type.h"
#include "bt_gattc_discovery.h"


/**
 *  @brief Define ancs event type.
 */
#define BLE_ANCS_EVENT_CONNECTED                0            /* The link connected event. */
#define BLE_ANCS_EVENT_IOS_NOTIFICATION         1            /* The ANCS gatt notification event.*/
#define BLE_ANCS_EVENT_NOTIFICATION_ATTRIBUTE   2            /* The ANCS notification attributes data event. */
#define BLE_ANCS_EVENT_APP_ATTRIBUTE            3            /* The ANCS app attributes data event. */
#define BLE_ANCS_EVENT_REQUEST_COMPLETED        4            /* The ANCS write characteristic response event. */
#define BLE_ANCS_EVENT_DISCONNECTED             5            /* The link disconnected event. */
typedef uint8_t ble_ancs_event_type_t;                       /* The ANCS event type. */


/**
 *  @brief Define bt_ancs_common status.
 */
#define ANCS_COMM_STATUS_READY              0               /* The ANCS common status ready. */
#define ANCS_COMM_STATUS_READ_NOTIF_SOURCE  1               /* The ANCS common read notification source cccd in progress. */
#define ANCS_COMM_STATUS_READ_DATA_SOURCE   2               /* The ANCS common read data source cccd in progress. */
typedef uint8_t ble_ancs_comm_status_t;                     /* The bt_ancs_common status. */


/**
 *  @brief Define ANCS charc type.
 */
#define BLE_ANCS_PROXY_CHARC_TYPE_NOTIFICATION_SOURCE   0   /* The ANCS characteristic notification source. */
#define BLE_ANCS_PROXY_CHARC_TYPE_CONTROL_POINT         1   /* The ANCS characteristic control point. */
#define BLE_ANCS_PROXY_CHARC_TYPE_DATA_SOURCE           2   /* The ANCS characteristic data source. */
typedef uint8_t ble_ancs_proxy_charc_type_t;                /* The ANCS proxy characteristic type. */



/**
 *  @brief Define bt_ancs_common event.
 */
typedef struct {
    ble_ancs_event_type_t evt_type;                         /**< ancs event type. */
    bt_handle_t connection_handle;                          /**< Connection handle. */
    bt_status_t result;                                     /**< event result. */
    union {
        ble_ancs_event_notification_t notification;         /**< general information of a notification. */
        ble_ancs_event_attribute_t attribute;               /**< the notification details. */
    } data;
} ble_ancs_event_t;                                         /**< ANCS event,reported to ble_ancs_app. */


/**
 * @brief               The function is ANCS callback, which receives the data and connect/disconnect the callback.
 * @param[in] p_evt     is the event structure reported to the app,#ble_ancs_event_t.
 * @return    none
 */
typedef void(* ble_ancs_event_handler_t)(ble_ancs_event_t *p_evt);

/**
 *  @brief Define bt_ancs_common content.
 */
typedef struct {
    ble_ancs_comm_status_t status;                                                  /**< bt_ancs_common status. */
    ble_ancs_event_handler_t evt_handler;                                           /**< ancs event callback define. */
    bt_gattc_discovery_service_t service;                                           /**< the ANCS service and the characteristics found during the discovery process. */
    bt_gattc_discovery_characteristic_t charc[BLE_ANCS_MAX_CHARC_NUMBER];           /**< the ANCS characteristic and the descriptors found during the discovery process. */
    bt_gattc_discovery_descriptor_t descrp[BLE_ANCS_MAX_CHARC_NUMBER];              /**< the ANCS descriptor during the discovery process. */
} ble_ancs_srv_content_t;                                                           /**< ANCS server context. */


/**
 * @brief               This function initializes the ancs_app_commom.
 * @param[in] evt_handler   is the callback for the #ble_ancs_event_handler_t.
 * @return    none
 */
void ble_ancs_start(ble_ancs_event_handler_t evt_handler);

/**
 * @brief               This function enable/disable notification source and data source CCCD.
 * @param[in] connection_handle   is the handle of the current connection.
 * @param[in] type                is the ANCS charc type #ble_ancs_proxy_charc_type_t.
 * @param[in] value               is the enable/disable value, 0:disable 1:enable.
 * @return                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                #BT_STATUS_FAIL, the ANCS write cccd fail.
 */
bt_status_t ble_ancs_proxy_write_charc_cccd(bt_handle_t connection_handle, ble_ancs_proxy_charc_type_t type, uint16_t value);

/**
 * @brief               This function write control point charc.
 * @param[in] connection_handle   is the handle of the current connection.
 * @param[in] type                is the ANCS charc type #ble_ancs_proxy_charc_type_t.
 * @param[in] value               is the pointer to the control point data, use control point format.
 * @param[in] length              is the value length.
 * @return                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                #BT_STATUS_FAIL, the ANCS write charc fail.
 */
bt_status_t ble_ancs_proxy_write_charc(bt_handle_t connection_handle, ble_ancs_proxy_charc_type_t type, uint8_t *value, uint16_t length);

/**
 * @brief               This function read notification source and data source CCCD status.
 * @param[in] connection_handle   is the handle of the current connection.
 * @param[in] type                is the ANCS charc type #ble_ancs_proxy_charc_type_t.
 * @return                        #BT_STATUS_SUCCESS, the operation completed successfully.
 *                                #BT_STATUS_FAIL, the ANCS read cccd fail.
 */
bt_status_t ble_ancs_proxy_read_charc_cccd(bt_handle_t connection_handle, ble_ancs_proxy_charc_type_t type);

/**
 * @brief          This function find charc by uuid.
 * @param[in] uuid      is the ANCS charc uuid.
 * @return              #BT_STATUS_SUCCESS, the operation completed successfully.
 *                      #BT_STATUS_FAIL, the ANCS connection is full or ANCS does not support this chip.
 */
bt_gattc_discovery_characteristic_t *ble_ancs_proxy_find_charc_by_uuid(uint8_t *uuid);

#endif