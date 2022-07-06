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

#include <stdint.h>
#include "ble_dis.h"
#include "syslog.h"
#include "FreeRTOS.h"

/* Create the log control block as user wishes. Here we use 'BLE_DIS' as module name.
 * User needs to define their own log control blocks as project needs.
 * Please refer to the log dev guide under /doc folder for more details.
 */
log_create_module(BLE_DIS, PRINT_LEVEL_INFO);

/************************************************
*   Global
*************************************************/

#define BLE_DIS_UUID16                               (0x180A)      /**< 16-bit UUID of Device Information Service. */
#define BLE_DIS_SYSTEM_ID_UUID16                     (0x2A23)
#define BLE_DIS_MODEL_NUMBER_STRING_UUID16           (0x2A24)
#define BLE_DIS_SERIAL_NUMBER_STRING_UUID16          (0x2A25)
#define BLE_DIS_FIRMWARE_REVISION_STRING_UUID16      (0x2A26)
#define BLE_DIS_HARDWARE_REVISION_STRING_UUID16      (0x2A27)
#define BLE_DIS_SOFTWARE_REVISION_STRING_UUID16      (0x2A28)
#define BLE_DIS_MANUFACTURER_NAME_STRING_UUID16      (0x2A29)
#define BLE_DIS_IEEE_DATA_UUID16                     (0x2A2A)
#define BLE_DIS_PNP_ID_UUID16                        (0x2A50)

#define BLE_DIS_SYSTEM_ID_VALUE_HANDLE                    (0x0062)      /**< Attribute Vlaue Hanlde of SYS ID Characteristic. */
#define BLE_DIS_MODEL_NUMBER_STRING_VALUE_HANDLE          (0x0064)
#define BLE_DIS_SERIAL_NUMBER_STRING_VALUE_HANDLE         (0x0066)
#define BLE_DIS_FIRMWARE_REVISION_STRING_VALUE_HANDLE     (0x0068)
#define BLE_DIS_HARDWARE_REVISION_STRING_VALUE_HANDLE     (0x006A)
#define BLE_DIS_SOFTWARE_REVISION_STRING_VALUE_HANDLE     (0x006C)
#define BLE_DIS_MANUFACTURER_NAME_STRING_VALUE_HANDLE     (0x006E)
#define BLE_DIS_IEEE_DATA_VALUE_HANDLE                    (0x0070)
#define BLE_DIS_PNP_ID_VALUE_HANDLE                       (0x0072)

#define BLE_DIS_SYS_ID_LEN (8)
#define BLE_DIS_PNP_ID_LEN (7)

const bt_uuid_t BT_SIG_UUID_SYSTEM_ID = BT_UUID_INIT_WITH_UUID16(BLE_DIS_SYSTEM_ID_UUID16);
const bt_uuid_t BT_SIG_UUID_MODEL_NUMBER_STRING = BT_UUID_INIT_WITH_UUID16(BLE_DIS_MODEL_NUMBER_STRING_UUID16);
const bt_uuid_t BT_SIG_UUID_SERIAL_NUMBER_STRING = BT_UUID_INIT_WITH_UUID16(BLE_DIS_SERIAL_NUMBER_STRING_UUID16);
const bt_uuid_t BT_SIG_UUID_FIRMWARE_REVISION_STRING = BT_UUID_INIT_WITH_UUID16(BLE_DIS_FIRMWARE_REVISION_STRING_UUID16);
const bt_uuid_t BT_SIG_UUID_HARDWARE_REVISION_STRING = BT_UUID_INIT_WITH_UUID16(BLE_DIS_HARDWARE_REVISION_STRING_UUID16);
const bt_uuid_t BT_SIG_UUID_SOFTWARE_REVISION_STRING = BT_UUID_INIT_WITH_UUID16(BLE_DIS_SOFTWARE_REVISION_STRING_UUID16);
const bt_uuid_t BT_SIG_UUID_MANUFACTURER_NAME_STRING = BT_UUID_INIT_WITH_UUID16(BLE_DIS_MANUFACTURER_NAME_STRING_UUID16);
const bt_uuid_t BT_SIG_UUID_IEEE_DATA = BT_UUID_INIT_WITH_UUID16(BLE_DIS_IEEE_DATA_UUID16);
const bt_uuid_t BT_SIG_UUID_PNP_ID = BT_UUID_INIT_WITH_UUID16(BLE_DIS_PNP_ID_UUID16);


/************************************************
*   Utilities
*************************************************/
#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_ble_dis_alert_level_write_callback =_default_ble_dis_alert_level_write_callback")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION) || defined(__CC_ARM)
#pragma weak ble_dis_get_characteristic_value_callback = default_ble_dis_get_characteristic_value_callback
#else
#error "Unsupported Platform"
#endif

bt_status_t default_ble_dis_get_characteristic_value_callback(ble_dis_charc_type_t charc, void *value)
{
    return BT_STATUS_FAIL;
}

static void ble_dis_system_id_encode(uint8_t *p_encoded_buffer, const ble_dis_system_id_t *p_sys_id)
{
    configASSERT(p_sys_id != NULL);
    configASSERT(p_encoded_buffer != NULL);
    p_encoded_buffer[0] = (p_sys_id->manufacturer_id & 0x00000000FF);
    p_encoded_buffer[1] = (p_sys_id->manufacturer_id & 0x000000FF00) >> 8;
    p_encoded_buffer[2] = (p_sys_id->manufacturer_id & 0x0000FF0000) >> 16;
    p_encoded_buffer[3] = (p_sys_id->manufacturer_id & 0x00FF000000) >> 24;
    p_encoded_buffer[4] = (p_sys_id->manufacturer_id & 0xFF00000000) >> 32;
    p_encoded_buffer[5] = (p_sys_id->organizationally_unique_id & 0x00000000FF);
    p_encoded_buffer[6] = (p_sys_id->organizationally_unique_id & 0x000000FF00) >> 8;
    p_encoded_buffer[7] = (p_sys_id->organizationally_unique_id & 0x0000FF0000) >> 16;
}

static uint8_t ble_dis_uint16_encode(uint8_t *p_encoded_data, uint16_t value)
{
    p_encoded_data[0] = (uint8_t) ((value & 0x00FF) >> 0);
    p_encoded_data[1] = (uint8_t) ((value & 0xFF00) >> 8);
    return sizeof(uint16_t);
}

static void ble_dis_pnp_id_encode(uint8_t *p_encoded_buffer, const ble_dis_pnp_id_t *p_pnp_id)
{
    uint8_t len = 0;
    configASSERT(p_pnp_id != NULL);
    configASSERT(p_encoded_buffer != NULL);
    p_encoded_buffer[len++] = p_pnp_id->vendor_id_source;
    len += ble_dis_uint16_encode(&p_encoded_buffer[len], p_pnp_id->vendor_id);
    len += ble_dis_uint16_encode(&p_encoded_buffer[len], p_pnp_id->product_id);
    len += ble_dis_uint16_encode(&p_encoded_buffer[len], p_pnp_id->product_version);
    configASSERT(len == BLE_DIS_PNP_ID_LEN);
}

static uint32_t ble_dis_model_number_string_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_model_number_string_callback, opcode:%d, size:%d\r\n", 2, rw, size);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_string_t str = {0};
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_MODEL_NUMBER, &str)) {
                if (0 != size) {
                    memcpy(data, str.utf8_string + offset, size);
                    return size;
                } else {
                    return str.length;
                }
            }
        }
    }
    return 0;
}

static uint32_t ble_dis_serial_number_string_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_serial_number_string_callback, opcode:%d, size:%d\r\n", 2, rw, size);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_string_t str = {0};
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_SERIAL_NUMBER, &str)) {
                if (0 != size) {
                    memcpy(data, str.utf8_string + offset, size);
                    return size;
                } else {
                    return str.length;
                }
            }
        }
    }
    return 0;
}

static uint32_t ble_dis_firmware_revision_string_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_firmware_revision_string_callback, opcode:%d, size:%d\r\n", 2, rw, size);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_string_t str = {0};
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_FIRMWARE_REVISION, &str)) {
                if (0 != size) {
                    memcpy(data, str.utf8_string + offset, size);
                    return size;
                } else {
                    return str.length;
                }
            }
        }
    }
    return 0;
}

static uint32_t ble_dis_hardware_revision_string_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_hardware_revision_string_callback, opcode:%d, size:%d\r\n", 2, rw, size);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_string_t str = {0};
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_HARDWARE_REVISION, &str)) {
                if (0 != size) {
                    memcpy(data, str.utf8_string + offset, size);
                    return size;
                } else {
                    return str.length;
                }
            }
        }
    }
    return 0;
}

static uint32_t ble_dis_software_revision_string_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_software_revision_string_callback, opcode:%d, size:%d\r\n", 2, rw, size);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_string_t str = {0};
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_SOFTWARE_REVISION, &str)) {
                if (0 != size) {
                    memcpy(data, str.utf8_string + offset, size);
                    return size;
                } else {
                    return str.length;
                }
            }
        }
    }
    return 0;
}

static uint32_t ble_dis_manufacturer_name_string_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_manufacturer_name_string_callback, opcode:%d, size:%d\r\n", 2, rw, size);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_string_t str = {0};
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_MANUFACTURER_NAME, &str)) {
                if (0 != size) {
                    memcpy(data, str.utf8_string + offset, size);
                    return size;
                } else {
                    return str.length;
                }
            }
        }
    }
    return 0;
}

static uint32_t ble_dis_system_id_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_system_id_callback, opcode:%d, size:%d\r\n", 2, rw, size);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_system_id_t sys_id;
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_SYSTEM_ID, &sys_id)) {
                if (0 != size) {
                    uint8_t encoded_sys_id[BLE_DIS_SYS_ID_LEN];
                    ble_dis_system_id_encode(encoded_sys_id, &sys_id);
                    memcpy(data, &encoded_sys_id, BLE_DIS_SYS_ID_LEN);
                    LOG_MSGID_I(BLE_DIS, "read system ID value = %8X\r\n", 1, data);
                }
                return BLE_DIS_SYS_ID_LEN;
            }
        }
    }
    return 0;
}

static uint32_t ble_dis_ieee_data_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_ieee_data_callback, opcode:%d, size:%d offset = %d\r\n", 3, rw, size, offset);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_ieee_data_t ieee_data;
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_IEEE_DATA_LIST, &ieee_data)) {
                if (0 != size) {
                    memcpy(data, ieee_data.data_list + offset, size);
                    return size;
                } else {
                    return ieee_data.list_length;
                }
            }
        }
    }
    return 0;
}

static uint32_t ble_dis_pnp_id_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(BLE_DIS, "ble_dis_pnp_id_callback, opcode:%d, size:%d\r\n", 2, rw, size);
    if (handle > 0) {
        if (rw == BT_GATTS_CALLBACK_READ) {
            ble_dis_pnp_id_t pnp_id;
            if (0 == ble_dis_get_characteristic_value_callback(BLE_DIS_CHARC_PNP_ID, &pnp_id)) {
                if (0 != size) {
                    uint8_t encoded_pnp_id[BLE_DIS_PNP_ID_LEN];
                    ble_dis_pnp_id_encode(encoded_pnp_id, &pnp_id);
                    memcpy(data, &encoded_pnp_id, BLE_DIS_PNP_ID_LEN);
                    LOG_MSGID_I(BLE_DIS, "read pnp ID value = %8X\r\n", 1, data);
                }
                return BLE_DIS_PNP_ID_LEN;
            }
        }
    }
    return 0;
}



BT_GATTS_NEW_PRIMARY_SERVICE_16(ble_dis_primary_service, BLE_DIS_UUID16);
//system_id
BT_GATTS_NEW_CHARC_16(ble_dis_char4_system_id, BT_GATT_CHARC_PROP_READ, BLE_DIS_SYSTEM_ID_VALUE_HANDLE, BLE_DIS_SYSTEM_ID_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_system_id, BT_SIG_UUID_SYSTEM_ID,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_system_id_callback);
//model_number_string
BT_GATTS_NEW_CHARC_16(ble_dis_char4_model_number_string, BT_GATT_CHARC_PROP_READ, BLE_DIS_MODEL_NUMBER_STRING_VALUE_HANDLE, BLE_DIS_MODEL_NUMBER_STRING_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_model_number_string, BT_SIG_UUID_MODEL_NUMBER_STRING,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_model_number_string_callback);
//serial_number_string
BT_GATTS_NEW_CHARC_16(ble_dis_char4_serial_number_string, BT_GATT_CHARC_PROP_READ, BLE_DIS_SERIAL_NUMBER_STRING_VALUE_HANDLE, BLE_DIS_SERIAL_NUMBER_STRING_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_serial_number_string, BT_SIG_UUID_SERIAL_NUMBER_STRING,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_serial_number_string_callback);
//firmware_revision_string
BT_GATTS_NEW_CHARC_16(ble_dis_char4_firmware_revision_string, BT_GATT_CHARC_PROP_READ, BLE_DIS_FIRMWARE_REVISION_STRING_VALUE_HANDLE, BLE_DIS_FIRMWARE_REVISION_STRING_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_firmware_revision_string, BT_SIG_UUID_FIRMWARE_REVISION_STRING,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_firmware_revision_string_callback);

//hardware_revision_string
BT_GATTS_NEW_CHARC_16(ble_dis_char4_hardware_revision_string, BT_GATT_CHARC_PROP_READ, BLE_DIS_HARDWARE_REVISION_STRING_VALUE_HANDLE, BLE_DIS_HARDWARE_REVISION_STRING_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_hardware_revision_string, BT_SIG_UUID_HARDWARE_REVISION_STRING,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_hardware_revision_string_callback);


//software_revision_string
BT_GATTS_NEW_CHARC_16(ble_dis_char4_software_revision_string, BT_GATT_CHARC_PROP_READ, BLE_DIS_SOFTWARE_REVISION_STRING_VALUE_HANDLE, BLE_DIS_SOFTWARE_REVISION_STRING_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_software_revision_string, BT_SIG_UUID_SOFTWARE_REVISION_STRING,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_software_revision_string_callback);

//MANUFACTURER_NAME_STRING manufacturer_name_string
BT_GATTS_NEW_CHARC_16(ble_dis_char4_manufacturer_name_string, BT_GATT_CHARC_PROP_READ, BLE_DIS_MANUFACTURER_NAME_STRING_VALUE_HANDLE, BLE_DIS_MANUFACTURER_NAME_STRING_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_manufacturer_name_string, BT_SIG_UUID_MANUFACTURER_NAME_STRING,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_manufacturer_name_string_callback);
//IEEE
BT_GATTS_NEW_CHARC_16(ble_dis_char4_ieee_data, BT_GATT_CHARC_PROP_READ, BLE_DIS_IEEE_DATA_VALUE_HANDLE, BLE_DIS_IEEE_DATA_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_ieee_data, BT_SIG_UUID_IEEE_DATA,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_ieee_data_callback);
//pnp_id
BT_GATTS_NEW_CHARC_16(ble_dis_char4_pnp_id, BT_GATT_CHARC_PROP_READ, BLE_DIS_PNP_ID_VALUE_HANDLE, BLE_DIS_PNP_ID_UUID16);
BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_dis_pnp_id, BT_SIG_UUID_PNP_ID,
                                  BT_GATTS_REC_PERM_READABLE,
                                  ble_dis_pnp_id_callback);



static const bt_gatts_service_rec_t *ble_dis_service_rec[] = {
    (const bt_gatts_service_rec_t *) &ble_dis_primary_service,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_system_id,
    (const bt_gatts_service_rec_t *) &ble_dis_system_id,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_model_number_string,
    (const bt_gatts_service_rec_t *) &ble_dis_model_number_string,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_serial_number_string,
    (const bt_gatts_service_rec_t *) &ble_dis_serial_number_string,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_firmware_revision_string,
    (const bt_gatts_service_rec_t *) &ble_dis_firmware_revision_string,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_hardware_revision_string,
    (const bt_gatts_service_rec_t *) &ble_dis_hardware_revision_string,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_software_revision_string,
    (const bt_gatts_service_rec_t *) &ble_dis_software_revision_string,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_manufacturer_name_string,
    (const bt_gatts_service_rec_t *) &ble_dis_manufacturer_name_string,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_ieee_data,
    (const bt_gatts_service_rec_t *) &ble_dis_ieee_data,
    (const bt_gatts_service_rec_t *) &ble_dis_char4_pnp_id,
    (const bt_gatts_service_rec_t *) &ble_dis_pnp_id,
};

const bt_gatts_service_t ble_dis_service = {
    .starting_handle = 0x0060,
    .ending_handle = 0x0072,
    .required_encryption_key_size = 0,
    .records = ble_dis_service_rec
};




