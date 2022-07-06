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
 
#ifndef __BLE_DIS_H__
#define __BLE_DIS_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_DIS BLE DIS
 * @{
 * This section defines the database of Immediate Alert Service and manages the write event of Alert Level Characteristic from a peer device.
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                  |
 * |------------------------------|-------------------------------------------------------------------------|
 * |\b DIS                        | Immediate Alert Service. This service exposes a control point to allow a peer device to cause the device to immediately alert. |
 *
 * @section ble_dis_api_usage How to use this module
 * - Step 1. Add DIS service to the application's GATT service database. And then return the bt_gatts_service_t pointer to GATTS stack by implementing the user-defined callback @ref bt_get_gatt_server(). 
 *  - Sample Code:
 *     @code
 *               // The GATTS collects all services.
 *               const bt_gatts_service_t * ble_gatt_server[] = {
 *                   &ble_gap_service,  // handle range: 0x0001 to 0x0009.
 *                   &ble_gatt_service, // handle range: 0x0011 to 0x0015.
 *                   &ble_dis_service,  // handle range: 0x0060 to 0x0072.
 *                   NULL
 *                };
 *                const bt_gatts_service_t** bt_get_gatt_server()
 *                { 
 *                    return ble_gatt_server;
 *                }

 *     @endcode
 * - Step 2. Implement the user-defined callback @ref ble_dis_alert_level_write_callback() to listen to the write event of the Alert Level Characteristic.
 *  - Sample Code:
 *     @code
 *               #define MANUFACTURER_NAME               "MEDIATEK" 
 *
 *              bt_status_t ble_dis_get_characteristic_value_callback(ble_dis_charc_type_t charc, void *value)
 *              {
 *	               bt_status_t status = BT_STATUS_FAIL;
 *            	 switch (charc) {
 *		             case BLE_DIS_CHARC_MANUFACTURER_NAME: { 
 *			            if (value) {
 *				         ble_dis_string_t *buffer = (ble_dis_string_t *)value;
 *				         buffer->length = (uint16_t)strlen(MANUFACTURER_NAME);
 *				         buffer->string = (uint8_t *)MANUFACTURER_NAME;
 *			            } 
 *			           break;
 *		             }
 *		             default:
 *			           break;
 *	              }
 *
 *	              return status;
 *            }
 *     @endcode
 */

#include "bt_type.h"
#include "bt_system.h"
#include "bt_gatt.h"
#include "bt_gatts.h"
#include "bt_gap_le.h"
#include "bt_platform.h"


BT_EXTERN_C_BEGIN



/** @UTF-8 string data type.
 * @the data structure for characteristic type BLE_DIS_CHARC_MANUFACTURER_NAME, BLE_DIS_CHARC_MODEL_NUMBER, BLE_DIS_CHARC_SERIAL_NUMBER, 
 * @BLE_DIS_CHARC_HARDWARE_REVISION, BLE_DIS_CHARC_FIRMWARE_REVISION, BLE_DIS_CHARC_SOFTWARE_REVISION
 * @note the type only hold a pointer to the string data.(not the actual data.) 
*/
typedef struct
{
	uint8_t *utf8_string;
	uint16_t length;
} ble_dis_string_t;

/**
 * @brief DIS service system id characteristic data structure.
 */
typedef struct
{
	uint64_t manufacturer_id;  /**< Only 5 LSOs shall be used. */
	uint32_t organizationally_unique_id;  /**< Only 3 LSOs shall be used. */
} ble_dis_system_id_t;

/**
 * @brief DIS service IEEE data characteristic data structure.
 */
typedef struct
{
	uint8_t *data_list;
	uint8_t list_length;
} ble_dis_ieee_data_t;

/**
 * @brief DIS service PnP id characteristic data structure.
 */
typedef struct
{
	uint8_t vendor_id_source;
	uint16_t vendor_id;
	uint16_t product_id;
	uint16_t product_version;
} ble_dis_pnp_id_t;

/**
 * @brief An DIS Service. 
 */
extern const bt_gatts_service_t ble_dis_service;


/**
 * @brief DIS service characteristic type.
 */
typedef enum {
    BLE_DIS_CHARC_MANUFACTURER_NAME, 
    BLE_DIS_CHARC_MODEL_NUMBER,
    BLE_DIS_CHARC_SERIAL_NUMBER,
    BLE_DIS_CHARC_HARDWARE_REVISION,
	BLE_DIS_CHARC_FIRMWARE_REVISION,
	BLE_DIS_CHARC_SOFTWARE_REVISION,
    BLE_DIS_CHARC_SYSTEM_ID,
    BLE_DIS_CHARC_IEEE_DATA_LIST,
    BLE_DIS_CHARC_PNP_ID,
} ble_dis_charc_type_t;

/**
 * @brief   This function is a static callback for the application to listen to the DIS event. Provide a user-defined callback.
 * @param[in] characteristic     is the callback characteristic type.
 * @param[in] value     is the payload of the callback message, need user to fill the content.
 * @return            The status of this operation returned from the callback. BT_STATUS_SUCCESS, if user suport the characteristic. otherwise, BT_STATUS_FAIL.
 */

bt_status_t ble_dis_get_characteristic_value_callback(ble_dis_charc_type_t charc, void *value);

BT_EXTERN_C_END

/**
 * @}
 * @}
 */

#endif /*__BT_BATTERY_SERVICE_H__*/
