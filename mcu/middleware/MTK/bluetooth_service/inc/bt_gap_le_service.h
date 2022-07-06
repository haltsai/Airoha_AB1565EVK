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

#ifndef __BT_GAP_LE_SERVICE_H__
#define __BT_GAP_LE_SERVICE_H__

/**
  * @addtogroup Bluetooth_Services_Group Bluetooth Services
  * @{
  * @addtogroup BluetoothServices BT GAP LE Service
  * @{
  * This section provides API to manage the BLE Advertising/Connection Information/Connection Parameters.
  *
  * Terms and Acronyms
  * ======
  * |Terms                         |Details                                                                  |
  * |------------------------------|-------------------------------------------------------------------------|
  * |\b GAP                        | Generic Access Profile. This profile defines the generic procedures related to discovery of Bluetooth enabled devices and link management aspects of connecting to the Bluetooth enabled devices. It also defines procedures related to the use of different security levels. |
  * |\b ADV                        | Advertising. A device sends data in either non-connectable undirected or scannable undirected advertising events. |
  * |\b LE                           | Low Energy. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Bluetooth_low_energy">Wikipedia</a>. |
  *
  * @section bt_gap_le_srv_api_usage How to use this module
  * - Step 1. Initialize the BT GAP LE Service.
  *  - Sample Code:
  *     @code
  *                bt_gap_le_srv_init();
  *     @endcode
  * - Step 2. Use BT GAP LE Service APIs to set BLE Advertising.
  *  - Sample code:
  *     @code
  *           static uint8_t bt_gap_le_srv_get_adv_data_cb(bt_gap_le_srv_adv_data_op_t op, void *data)
  *           {
  *                 uint8_t gen_result = 0;
  *                 bt_hci_le_set_ext_advertising_parameters_t ext_adv_para = {
  *                    .advertising_event_properties = BT_HCI_ADV_EVT_PROPERTIES_MASK_SCANNABLE | BT_HCI_ADV_EVT_PROPERTIES_MASK_LEGACY_PDU | BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE,
  *                    .primary_advertising_interval_min = 0x00100,
  *                    .primary_advertising_interval_max = 0x00400,
  *                    .primary_advertising_channel_map = 0x07,
  *                    .own_address_type = BT_ADDR_RANDOM,
  *                    .peer_address = {
  *                        .type = BT_ADDR_PUBLIC,
  *                        .addr = {0},
  *                     },
  *                    .advertising_filter_policy = BT_HCI_ADV_FILTER_ACCEPT_SCAN_CONNECT_FROM_ALL,
  *                    .primary_advertising_phy = BT_HCI_LE_ADV_PHY_1M,
  *                    .secondary_advertising_phy = BT_HCI_LE_ADV_PHY_1M,
  *                    .advertisng_SID = 0x00,
  *                    .scan_request_notify_enable = BT_HCI_DISABLE
  *                };
  *                uint8_t adv_data[15] = {0x0E, 0x09, 'L', 'E', '_', 'E', 'X', 'T', '_', 'A', 'D', 'V', '_'};
  *                uint8_t scan_rsp_data[21] = {0x14, 0x09, 'L', 'E', '_', 'E', 'X', 'T', '_', 'S', 'C', 'A', 'N', '_', 'R', 'E', 'S', 'P', '_'};
  *                adv_data[13] = ((instance >> 4) < 0xA ? (instance >> 4) + 48 : (instance >> 4) + 55);
  *                adv_data[14] = ((instance & 0xF) < 0xA ? (instance & 0xF) + 48 : (instance & 0xF) + 55);
  *                scan_rsp_data[19] = adv_data[13];
  *                scan_rsp_data[20] = adv_data[14];
  *                ext_adv_para.advertisng_SID = instance;
  *
  *                switch (op) {
  *                    case BT_GAP_LE_SRV_ADV_DATA_OP_CONFIG: {
  *                        bt_gap_le_srv_adv_config_info_t *info = (bt_gap_le_srv_adv_config_info_t *)data;
  *                        bt_gap_le_srv_memcpy(&info->adv_param, &ext_adv_para, sizeof(bt_hci_le_set_ext_advertising_parameters_t));
  *
  *                        bt_gap_le_srv_memcpy(info->adv_data.data, &adv_data, 15);
  *                        info->adv_data.data_length = 15;
  *                        bt_gap_le_srv_memcpy(info->scan_rsp.data, &scan_rsp_data, 21);
  *                        info->scan_rsp.data_length = 21;
  *                        gen_result = BT_GAP_LE_ADV_PARAM_GEN | BT_GAP_LE_ADV_DATA_GEN | BT_GAP_LE_ADV_SCAN_RSP_GEN;
  *                    }
  *                        break;
  *
  *                    case BT_GAP_LE_SRV_ADV_DATA_OP_UPDATE: {
  *                        bt_gap_le_srv_adv_update_info_t *info = (bt_gap_le_srv_adv_update_info_t *)data;
  *                        bt_gap_le_srv_generate_adv_data(&adv_fields, info->adv_data.data,(uint32_t *) &(info->adv_data.data_length));
  *                        bt_gap_le_srv_generate_adv_data(&scanrsp_fields, info->scan_rsp.data, (uint32_t *)&(info->scan_rsp.data_length));
  *                        gen_result = BT_GAP_LE_ADV_DATA_GEN | BT_GAP_LE_ADV_SCAN_RSP_GEN;
  *                    }
  *                        break;
  *
  *                    default:
  *                        break;
  *                }
  *                return gen_result;
  *            }
  *            static void bt_gap_le_srv_event_func (bt_gap_le_srv_event_t event, void *data)
  *            {
  *               bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Srv event %d", 1, event);
  *               bt_gap_le_srv_event_ind_t *ind = (bt_gap_le_srv_event_ind_t *)data;
  *               switch (event) {
  *                  case BT_GAP_LE_SRV_EVENT_ADV_COMPLETE: {
  *                      bt_gap_le_srv_dump_adv_complete(&ind->adv_complete);
  *                  }
  *                      break;
  *
  *                  default :
  *                      break;
  *            }
  *            bt_bd_addr_t random_addr = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};
  *            uint8_t instance = 0;
  *            bt_gap_le_srv_error_t err = bt_gap_le_srv_get_available_instance(&instance);
  *            bt_gap_le_srv_start_adv(instance, (bt_bd_addr_ptr_t)&random_addr,NULL,bt_gap_le_srv_get_adv_data_cb,bt_gap_le_srv_event_func);
  *     @endcode
  */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bt_gap_le.h"
#include "bt_hci.h"
#include "bt_type.h"
#include "bt_system.h"
#include "bt_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup BT_GAP_LE_SERVICE_define Define
 * @{
 * Define Bluetooth GAP LE service data types and values.
 */

/**
 * @brief Bluetooth GAP LE service error code.
 */
#define BT_GAP_LE_SRV_ERROR_BASE                       (0x0)                                   /**< General Error Base. */
#define BT_GAP_LE_SRV_SUCCESS                          (BT_GAP_LE_SRV_ERROR_BASE + 0x0)        /**< General Success. */
#define BT_GAP_LE_SRV_ERROR_FAIL                       (BT_GAP_LE_SRV_ERROR_BASE + 0x1)        /**< General Fail. */
#define BT_GAP_LE_SRV_ERROR_BUSY                       (BT_GAP_LE_SRV_ERROR_BASE + 0x2)        /**< General Busy. */
#define BT_GAP_LE_SRV_ERROR_UNSUPPORTED                (BT_GAP_LE_SRV_ERROR_BASE + 0x3)        /**< Not Support. */
#define BT_GAP_LE_SRV_ERROR_INVALID_INSTANCE           (BT_GAP_LE_SRV_ERROR_BASE + 0x4)        /**< The input instance is invalid. */
#define BT_GAP_LE_SRV_ERROR_INVALID_PARAM              (BT_GAP_LE_SRV_ERROR_BASE + 0x5)        /**< Some input parameter is invalid. */
#define BT_GAP_LE_SRV_ERROR_INVALID_LENTH              (BT_GAP_LE_SRV_ERROR_BASE + 0x6)        /**< Data length is invalid. */
#define BT_GAP_LE_SRV_ERROR_INVALID_STATE              (BT_GAP_LE_SRV_ERROR_BASE + 0x7)        /**< State is invalid. */
#define BT_GAP_LE_SRV_ERROR_NOT_FOUND                  (BT_GAP_LE_SRV_ERROR_BASE + 0x8)        /**< General not found. */
#define BT_GAP_LE_SRV_ERROR_NO_MEMORY                  (BT_GAP_LE_SRV_ERROR_BASE + 0x9)        /**< General out-of-memory. */
#define BT_GAP_LE_SRV_ERROR_INSTANCE_USED              (BT_GAP_LE_SRV_ERROR_BASE + 0xA)        /**< ADV instance has been used. */
#define BT_GAP_LE_SRV_ERROR_PENDING                    (BT_GAP_LE_SRV_ERROR_BASE + 0xB)        /**< General Pending. */
#define BT_GAP_LE_SRV_ERROR_CONN_LIMIT_EXCEEDED        (BT_GAP_LE_SRV_ERROR_BASE + 0xC)        /**< The connection limit exceeded,cannot enable connectable ADV. */

typedef int32_t bt_gap_le_srv_error_t;
/**
 * @brief Bluetooth GAP LE service advertising interval value.
 */
#define BLE_GAP_LE_SRV_ADV_FAST_INTERVAL        (0x30)   /** Advertising interval: 30 ms. */
#define BLE_GAP_LE_SRV_ADV_MIDDLE_INTERVAL      (0xA0)   /** Advertising interval: 100 ms. */
#define BLE_GAP_LE_SRV_ADV_SLOW_INTERVAL        (0x29C)  /** Advertising interval: 417.5 ms. */

/**
 * @brief Bluetooth GAP LE service connection interval value.
 */
#define BLE_GAP_LE_SRV_CONN_FAST_INTERVAL      (0x18)    /** Connection interval: 30 ms. */
#define BLE_GAP_LE_SRV_CONN_MIDDLE_INTERVAL    (0x78)    /** Connection interval: 150 ms. */
#define BLE_GAP_LE_SRV_CONN_SLOW_INTERVAL      (0x0120)  /** Connection interval: 360 ms. */
#define BLE_GAP_LE_SRV_CONN_LATENCY_DEFAULT    (0x00)    /** Latency is 0, which means that it will always listen master's packet. */
#define BLE_GAP_LE_SRV_SUP_TIMEOUT_DEFAULT     (0x0258)  /** Supervision Timemout: 6000 ms. */

/**
 * @brief Bluetooth GAP LE service events.
 */
#define BT_GAP_LE_SRV_EVENT_ADV_COMPLETE               0x00   /** The adv operation is completed which includes adv start, stop, remove, update, force restart. */
#define BT_GAP_LE_SRV_EVENT_ADV_CLEARED                0x01   /** All the LE adv has been cleared. */
#define BT_GAP_LE_SRV_EVENT_CONN_CLEARED               0x02   /** All the LE connections has been cleared. */
#define BT_GAP_LE_SRV_EVENT_CONN_UPDATED               0x03   /** The procedure of connection parameters update is completed. */
#define BT_GAP_LE_SRV_EVENT_BLE_DISABLED               0x04   /** The LE has been disabled. */
#define BT_GAP_LE_SRV_EVENT_SCAN_STARTED               0x05   /** The scan is started. */
#define BT_GAP_LE_SRV_EVENT_SCAN_STOPPED               0x06   /** The scan is stopped. */
typedef uint8_t bt_gap_le_srv_event_t;              /**< The type of the BLE events. */

/**
 * @brief Bluetooth GAP LE service advertising state.
 */
#define BT_CM_LE_ADV_STATE_TYPE_REMOVED             (0x00)    /** The adv has been removed. */
#define BT_CM_LE_ADV_STATE_TYPE_REMOVING            (0x01)    /** The adv is being removed. */
#define BT_CM_LE_ADV_STATE_TYPE_STOPPED             (0x02)    /** The adv has been stopped. */
#define BT_CM_LE_ADV_STATE_TYPE_STOPPING            (0x03)    /** The adv is being stopped. */
#define BT_CM_LE_ADV_STATE_TYPE_STARTING            (0x04)    /** The adv is being started. */
#define BT_CM_LE_ADV_STATE_TYPE_STARTED             (0x05)    /** The adv has been started. */
typedef uint8_t bt_gap_le_srv_adv_state_t;

/**
 * @brief Bluetooth GAP LE service advertising events.
 */
#define BT_GAP_LE_SRV_ADV_REMOVED                   0x00      /** The adv remove operation is completed. */
#define BT_GAP_LE_SRV_ADV_STARTED                   0x01      /** The adv start operation is completed. */
#define BT_GAP_LE_SRV_ADV_STOPPED                   0x02      /** The adv stop operation is completed. */
#define BT_GAP_LE_SRV_ADV_UPDATED                   0x03      /** The adv update operation is completed. */
#define BT_GAP_LE_SRV_ADV_FORCE_RESTART             0x04      /** The adv has been stopped because of some special reason. The application need to restart the adv. Now, it's only for command disallowed problem.*/
typedef uint8_t bt_gap_le_srv_adv_event_t;                    /**< The type of the BLE advertising. */

/**
 * @brief Bluetooth GAP LE service get advertising data operation type.
 */
#define BT_GAP_LE_SRV_ADV_DATA_OP_CONFIG                    0x01       /**< The LE adv need to be configured. */
#define BT_GAP_LE_SRV_ADV_DATA_OP_UPDATE                    0x02       /**< The LE adv need to be updated, only update adv data or scan response data. */
typedef uint8_t bt_gap_le_srv_adv_data_op_t;

 /**
  * @brief BT GAP LE service advertising data generated result.
  */
#define BT_GAP_LE_ADV_NOTHING_GEN     (0x0)                /**< Nothing is generated. */
#define BT_GAP_LE_ADV_PARAM_GEN       (0x01 << 0)          /**< The LE advertising parameter is generated. */
#define BT_GAP_LE_ADV_DATA_GEN        (0x01 << 1)          /**< The LE advertising data is generated. */
#define BT_GAP_LE_ADV_SCAN_RSP_GEN    (0x01 << 2)          /**< The LE advertising scan response data is generated. */
typedef uint8_t bt_gap_le_srv_adv_gen_result_t;

/**
  * @brief BT GAP LE service BLE link type.
  */
#define BT_GAP_LE_SRV_LINK_TYPE_NONE          (0x00)       /**< The type of no connection. */
#define BT_GAP_LE_SRV_LINK_TYPE_NORMAL        (0x01)       /**< For a normal BLE link, data can be transferred on this data. */
#define BT_GAP_LE_SRV_LINK_TYPE_CUSTOMIZE     (0x02)       /**< BLE Link with specific operations, such as not allowing other modules to operate this link. */
typedef uint8_t bt_gap_le_srv_link_type_t;

/**
  * @brief BT GAP LE service BLE link attribute.
  */
#define BT_GAP_LE_SRV_LINK_ATTRIBUTE_NEED_RHO          (0x0001)            /**< The BLE link attribute is need to do to RHO. */
#define BT_GAP_LE_SRV_LINK_ATTRIBUTE_NOT_NEED_RHO      (0x0002)            /**< The BLE link attribute is not need to do to RHO. */
typedef uint16_t bt_gap_le_srv_link_attribute_t;
/**
 * @}
 */

/**
 * @defgroup BT_GAP_LE_SERVICE_struct Struct.
 * @{
 * This section defines structures for the BT GAP LE Service.
 */

/**
 * @brief Define for BT GAP LE service configure parameters.
 */
typedef struct {
    uint8_t     max_advertising_num;         /**< The maximum number of LE advertising that the chip can support. */
    uint8_t     max_connection_num;          /**< The maximum number of LE connection that the chip can support. */
} bt_gap_le_srv_config_t;

/**
 *  @brief The data structure for #BT_GAP_LE_SRV_EVENT_BLE_DISABLED and #BT_GAP_LE_SRV_EVENT_ADV_CLEARED.
 * and #BT_GAP_LE_SRV_EVENT_CONN_CLEARED events.
 */
typedef struct {
    bt_gap_le_srv_error_t result;
} bt_gap_le_srv_common_result_t;

/**
 *  @brief The 16-bit or 32-bit or 128-bit UUID structure.
 */
typedef struct {
    union {
        uint16_t uuid16;               /**< 16-bit UUID. */
        uint32_t uuid32;               /**< 32-bit UUID. */
        uint8_t  uuid[16];             /**< An array to store 128-bit UUID. */
    };
} bt_gap_le_srv_uuid_t;

/**
 *  @brief Advertising data field structure.
 */
typedef struct {
    uint8_t flags;                            /**< Advertising data Flags field. */

    const bt_gap_le_srv_uuid_t *uuids16;      /**< List of the 16-bit service class UUIDs. */
    uint8_t num_uuids16;                      /**< The Number of 16-bit service class UUIDS. */
    bool uuids16_is_complete;                 /**< Complete or Incomplete. */

    const bt_gap_le_srv_uuid_t *uuids32;      /**< List of the 32-bit service class UUIDs. */
    uint8_t num_uuids32;                      /**< The Number of 32-bit service class UUIDS.*/
    bool uuids32_is_complete;                 /**< Complete or Incomplete. */

    const bt_gap_le_srv_uuid_t *uuids128;     /**< List of the 128-bit service class UUIDs. */
    uint8_t num_uuids128;                     /**< The Number of 128-bit service class UUIDS. */
    bool uuids128_is_complete;                /**< Complete or Incomplete. */

    const uint8_t *name;                      /**< Local device name field. */
    uint8_t name_len;                         /**< The length of the local name.*/
    bool name_is_complete;                    /**< Shortened or Complete local name. */

    int8_t tx_power_level;                    /**< Transmit power level. */
    bool tx_power_level_is_present;           /**< Transmit power level is present or not. */

    const uint8_t *slave_conn_interval;       /**< Slave connection interval range. */

    const uint8_t *srv_data_uuid16;           /**< Service data with 16-bit UUID. */
    uint8_t srv_data_uuid16_len;              /**< The length of the service data. */

    const uint8_t *public_target_addr;        /**< Public target address. */
    uint8_t num_public_target_addrs;          /**< The Number of Public target address. */

    uint16_t appearance;                      /**< Appearance. */
    bool appearance_is_present;               /**< Appearance is present or not. */

    uint16_t adv_interval;                    /**< Advertising interval. */
    bool adv_interval_is_present;             /**< Advertising interval is present or not. */

    const uint8_t *srv_data_uuid32;           /**< Service data with 32-bit UUID. */
    uint8_t srv_data_uuid32_len;              /**< The length of the service data. */

    const uint8_t *srv_data_uuid128;          /**< Service data with 128-bit UUID. */
    uint8_t srv_data_uuid128_len;             /**<The length of the service data. */

    const uint8_t *manufacturer_data;         /**< Manufacturer specific data. */
    uint8_t manufacturer_data_len;            /**<The length of the manufacturer. */
}bt_gap_le_srv_adv_fields_t;

/**
 *  @brief The data structure for #BT_GAP_LE_SRV_ADV_DATA_OP_CONFIG operation type.
 */
typedef struct {
    bt_hci_le_set_ext_advertising_parameters_t adv_param;   /**<The adv parameters. */
    bt_gap_le_set_ext_advertising_data_t adv_data;          /**<The adv data. */
    bt_gap_le_set_ext_scan_response_data_t scan_rsp;        /**<The scan response data. */
} bt_gap_le_srv_adv_config_info_t;

/**
 *  @brief The data structure for #BT_GAP_LE_SRV_ADV_DATA_OP_UPDATE operation type.
 */
typedef struct {
    bt_gap_le_set_ext_advertising_data_t adv_data;         /**<The adv data. */
    bt_gap_le_set_ext_scan_response_data_t scan_rsp;       /**<The scan response data. */
} bt_gap_le_srv_adv_update_info_t;

/**
 *  @brief Bluetooth device manager LE service advertising time structure.
 */
typedef struct {
    uint16_t        duration;                   /**< Advertising duration. The range is from 0x0000 to 0xFFFF, 0x0000 means continue until set disable. */
    uint8_t         max_ext_advertising_evts;   /**< The maximum number of extended advertising events. 0x00 means no maximum number. */
} bt_gap_le_srv_adv_time_params_t;

/**
 *  @brief The data structure for #BT_GAP_LE_SRV_EVENT_ADV_COMPLETE event.
 */
typedef struct {
    uint8_t adv_evt;                            /**< Indicates the event type of #bt_gap_le_srv_adv_event_t. */
    uint8_t result;                             /**< #bt_gap_le_srv_error_t. */
    uint8_t instance;                           /**< The adv instance. */
    uint16_t conn_handle;                       /**< The handle of the relevant connection, valid only when the adv is connected and then terminated. */
    uint8_t num_ext_adv_events;                 /**< The number of completed extended advertising events.
                                                                                   * This field is only valid if non-zero max_events was passed to
                                                                                   * #bt_gap_le_srv_start_adv() and advertising completed due to duration
                                                                                   * timeout or max events transmitted. */
    int8_t selected_tx_power;                   /**< Current selected Tx power by controller, only for #BT_GAP_LE_SRV_ADV_STARTED. */
} bt_gap_le_srv_adv_complete_t;

/**
 *  @brief Connection parameters structure.
 */
typedef struct {
    uint16_t        conn_interval;       /**< Connection interval in 1.25 ms units, value range: 0x0006 to 0x0C80. */
    uint16_t        conn_latency;        /**< Connection latency in number of connection events, value range: 0x0000 to 0x03E8. */
    uint16_t        supervision_timeout; /**< Supervision timeout in 10 ms units, value range: 0x000A to 0x0C80. */
} bt_gap_le_srv_conn_params_t;

/**
 *  @brief The data structure for #BT_GAP_LE_SRV_EVENT_CONN_UPDATE event.
 */
typedef struct {
    uint8_t result;                     /**< #bt_gap_le_srv_error_t. */
    uint16_t conn_handle;               /**< The handle of the relevant connection. */
    bt_gap_le_srv_conn_params_t params; /**< The parameters of the relevant connection. */
} bt_gap_le_srv_conn_update_t;

/**
 *  @brief Connection infomation structure.
 */
typedef struct {
    bt_role_t         role;            /**< The role of local device. */
    bt_addr_t         local_addr;      /**< The LE address of local device. */
    bt_addr_t         peer_addr;       /**< The LE address of peer device. */
} bt_gap_le_srv_conn_info_t;

/**
 *  @brief The data structure for BT GAP LE Service events.
 */
typedef struct {
    union {
        bt_gap_le_srv_adv_complete_t    adv_complete;   /**< #BT_GAP_LE_SRV_EVENT_ADV_COMPLETE.*/
        bt_gap_le_srv_common_result_t   adv_clear;      /**< #BT_GAP_LE_SRV_EVENT_ADV_CLEARED.*/
        bt_gap_le_srv_common_result_t   conn_clear;     /**< #BT_GAP_LE_SRV_EVENT_CONN_CLEARED.*/
        bt_gap_le_srv_common_result_t   dis_complete;   /**< #BT_GAP_LE_SRV_EVENT_BLE_DISABLED.*/
        bt_gap_le_srv_conn_update_t     conn_update;    /**< #BT_GAP_LE_SRV_EVENT_CONN_UPDATED.*/
    };
} bt_gap_le_srv_event_ind_t;

/**
 *  @brief      LE set extended scan enable command.
 */
typedef bt_hci_cmd_le_set_extended_scan_enable_t bt_gap_le_srv_set_extended_scan_enable_t;

/**
 *  @brief      LE set extended scan parameters command.
 */
typedef bt_hci_le_set_ext_scan_parameters_t bt_gap_le_srv_set_extended_scan_parameters_t;

/**
 *  @brief BT GAP LE service event callback type.
 * @param[in] event    the event type.
 * @param[in] data    the data of the event, #bt_gap_le_srv_event_ind_t.
 * @return    None.
 */
typedef void(*bt_gap_le_srv_event_cb_t)(bt_gap_le_srv_event_t event, void *data);

/**
 * @brief BT GAP LE service advertising data generater callback.
 * @param[in] op    the operation type, configure or update.
 * @param[in] data    the advertising buffer need to be configured or updated, please refer to #bt_gap_le_srv_adv_config_info_t
 *                            or #bt_gap_le_srv_adv_update_info_t.
 * @return    please refer #bt_gap_le_srv_adv_gen_result_t.
 */
typedef uint8_t (*bt_gap_le_srv_get_adv_data_cb_t)(bt_gap_le_srv_adv_data_op_t op, void *data);

 /**
  * @}
  */

/**
 * @brief   This function initializes the BT GAP LE service. It is recommended to call this API once during the bootup.
 * @param[in] config    the configuration to initial BT GAP LE service.
 * @return    None.
 */
void bt_gap_le_srv_init(const bt_gap_le_srv_config_t *config);

/**
 * @brief   This function destroys the BT GAP LE service.
 * @return    None.
 */
void bt_gap_le_srv_deinit(void);

/**
 * @brief   This function is used to clear all BLE working state.
 *            The application will receive the #BT_GAP_LE_SRV_EVENT_BLE_DISABLED event after the operation is completed.
 * @param[in] callback        the callback to get the operation result, #bt_gap_le_srv_event_cb_t.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_BUSY, the LE is being disabled, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_disable_ble(void *callback);

/**
 * @brief   This function is used to generate LE advertising data and scan response data.
 * @param[in] adv_fields     the data to be generated.
 * @param[in, out] data       the buffer for storing the adv data or scan response data.
 * @param[in, out] data_len   the length of the data.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_generate_adv_data(const bt_gap_le_srv_adv_fields_t *adv_fields,
                      uint8_t *data, uint32_t *data_len);

/**
 * @brief   This function is used to parse LE advertising data and scan response data.
 * @param[in] type     the AD type.
 * @param[in] adv_data     the data to be parsed.
 * @param[in, out] length      the length of the adv_data and the length of field_data.
 * @param[in, out] field_data   the data point that want to be parsed.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_parse_field_data_by_ad_type(uint8_t type,
    uint8_t *adv_data, uint32_t *length, uint8_t **field_data);

/**
 * @brief   This function is used to get an available LE advertising instance.
 * @param[out] instance     the adv handle, range from 1 to the max number of LE advertising that the chip can support.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_get_available_instance(uint8_t *instance);

/**
 * @brief   This function is used to start LE advertising.
 *            The application receives the #BT_GAP_LE_SRV_ADV_DATA_OP_CONFIG to generate the data that want to be configured.
 *            The application receives the #BT_GAP_LE_SRV_EVENT_ADV_COMPLETE event after the start operation is completed.
 * @param[in] instance     the adv handle that is got by #bt_gap_le_srv_get_available_instance.
 * @param[in] random_addr     the random address want to be set in the LE advertising.
 * @param[in] time_param     the time parameter.
 * @param[in] adv_data_cb      the callback to get advertising data, #bt_gap_le_srv_get_adv_data_cb_t.
 *                             if you want to restart a stopped adv without any data changing, please set it as NULL;
 * @param[in] adv_evt_cb        the callback to get the advertising operation result, #bt_gap_le_srv_event_cb_t.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_STATE, the state of the advertising with instance is invalid.
 *            #BT_GAP_LE_SRV_ERROR_NO_MEMORY, can not allocate enough memory to save the advertising data.
 *            #BT_GAP_LE_SRV_ERROR_BUSY, the LE advertising with instance is being started.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_INSTANCE, the advertising instance is not in the correct range, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_start_adv(uint8_t instance,
    bt_bd_addr_ptr_t random_addr,
    bt_gap_le_srv_adv_time_params_t *time_param,
    void* adv_data_cb,
    void* adv_evt_cb);

/**
 * @brief   This function is used to stop the LE advertising.
 *            The application receives the #BT_GAP_LE_SRV_EVENT_ADV_COMPLETE event after the stop operation is completed.
 * @param[in] instance     the adv handle that is got by #bt_gap_le_srv_get_available_instance.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_STATE, the state of the advertising with instance is invalid.
 *            #BT_GAP_LE_SRV_ERROR_BUSY, the LE advertising with instance is being stopped.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_INSTANCE, the advertising instance is not in the correct range, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_stop_adv(uint8_t instance);

/**
 * @brief   This function is used to update the LE advertising, only update adv data or scan response data.
 *            The application receives the #BT_GAP_LE_SRV_ADV_DATA_OP_UPDATE to generate the data that want to be updated.
 *            The application receives the #BT_GAP_LE_SRV_EVENT_ADV_COMPLETE event after the update operation completed.
 * @param[in] instance     the adv handle that is got by #bt_gap_le_srv_get_available_instance.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_STATE, the state of the advertising with instance is invalid.
 *            #BT_GAP_LE_SRV_ERROR_NO_MEMORY, can not allocate enough memory to save the advertising data.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_INSTANCE, the advertising instance is not in the correct range, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_update_adv(uint8_t instance);

/**
 * @brief   This function is used to remove the LE advertising with instance.
 *            The application receives the #BT_GAP_LE_SRV_EVENT_ADV_COMPLETE event after the remove operation is completed.
 * @param[in] instance     the adv handle that is got by #bt_gap_le_srv_get_available_instance.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_STATE, the state of the advertising with instance is invalid.
 *            #BT_GAP_LE_SRV_ERROR_BUSY, the LE advertising with instance is being removed.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_INSTANCE, the advertising instance is not in the correct range, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_remove_adv(uint8_t instance);

/**
 * @brief   This function is used to clear all LE advertising, it is recommended to call this API once.
 *            The application receives the #BT_GAP_LE_SRV_EVENT_ADV_CLEARED event after the clear operation is completed.
 * @param[in] callback        the callback to get the clear operation result, @bt_gap_le_srv_event_cb_t.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_BUSY, the LE advertising with instance is being cleared, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_clear_adv(void *callback);


/**
 * @brief   This function is used to get the state of the LE advertising with instance.
 * @param[in] instance     the adv handle that is got by #bt_gap_le_srv_get_available_instance.
 * @return    The current state of the adverting with instance.
 */
bt_gap_le_srv_adv_state_t bt_gap_le_srv_get_adv_state(uint8_t instance);

/**
 * @brief   This function is used to get the local random address.
 * @return   The random address of local device.
 */
const bt_bd_addr_t *bt_gap_le_srv_get_local_random_addr(void);

/**
 * @brief   This function is used to get the connected device number.
 * @return    The number of connected device.
 */
uint8_t bt_gap_le_srv_get_connected_dev_num(void);

/**
 * @brief   This function is used to get the connection information with handle.
 * @param[in] conn_handle     the handle of the current LE connection.
 * @return    connection information.
 */
bt_gap_le_srv_conn_info_t *bt_gap_le_srv_get_conn_info(bt_handle_t conn_handle);

/**
 * @brief   This function is used to get the connection handle with local or peer address.
 * @param[in] addr        the address of local device or peer device.
 * @param[in] is_local_addr        true is local address, false is peer address.
 * @return    the handle of the LE connection. if not be found, 0xFFFF is returned.
 */
uint16_t bt_gap_le_srv_get_conn_handle_by_addr(bt_addr_t *addr, bool is_local_addr);

/**
 * @brief   This function is used to get the connection handle with local or peer address.
 * @param[out] handle_list      is the list of connection handle.
 * @param[in out] count         is the input and output parameter. As an input parameter, it is the length of the list and the maximum number of
 *                                   BLE link that the list can hold. As an output parameter, it is the actual number of the connection handle
 *                                   stored in the list upon the return of this function, which cannot exceed the length of the list.
 * @param[in] addr              the address of local device or peer device.
 * @param[in] is_local_addr     addr is local address if it is true, or the address is peer address.
 * @return                      #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_get_conn_handle_by_addr_ext(bt_handle_t *handle_list, uint8_t *count, bt_addr_t *addr, bool is_local_addr);

/**
 * @brief   This function is used to disconnect and clear all LE connections, it is recommended to call this API once.
 *            The application receives the #BT_GAP_LE_SRV_EVENT_CONN_CLEARED event after the clear operation is completed.
 * @param[in] callback        the callback to get the clear operation result, @bt_gap_le_srv_event_cb_t.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_BUSY, the LE connection is being cleared, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_clear_connections(void *callback);

/**
 * @brief   This function is used to get the connection parameters with handle.
 * @param[in] conn_handle     the handle of the LE connection.
 * @return    The connection parameters of the LE connection.
 */
bt_gap_le_srv_conn_params_t *bt_gap_le_srv_get_current_conn_params(bt_handle_t conn_handle);


/**
 * @brief   This function is used to update the connection parameters with handle.
 *            The application receives the #BT_GAP_LE_SRV_EVENT_CONN_UPDATED event after the clear operation is complete.
 * @param[in] conn_handle     the handle of the LE connection.
 * @param[in] callback        the callback to get the operation result, #bt_gap_le_srv_event_cb_t.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_PARAM, some input parameter is NULL or invalid, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_update_conn_params(bt_handle_t conn_handle, bt_gap_le_srv_conn_params_t *param, void* callback);

/**
 * @brief   BT GAP LE service get BLE link attribute callback.
 * @param[in] local_addr      the address of local device.
 * @return    attribute information.
 */
typedef bt_gap_le_srv_link_attribute_t (*bt_gap_le_srv_get_link_attribute_callback_t)(const bt_addr_t *local_addr);

/**
 * @brief   This function is used to register for the link attribute callback.
 * @param[in] callback        the callback to get the link attribute, #bt_gap_le_srv_get_link_attribute_callback_t.
 * @return    #BT_GAP_LE_SRV_SUCCESS, the operation completed successfully.
 *            #BT_GAP_LE_SRV_ERROR_INVALID_PARAM, some input parameter is NULL or invalid, otherwise it failed.
 */
bt_gap_le_srv_error_t bt_gap_le_srv_get_link_attribute_register(bt_gap_le_srv_get_link_attribute_callback_t callback);

/**
 * @brief   This function is used to get new connection handle by old conenction handle.
 * @param[in] old_handle      the handle of the LE connection.
 * @return    conenction handle.
 */
bt_handle_t bt_gap_le_srv_get_handle_by_old_handle(bt_handle_t old_handle);

/**
 * @brief   This function sets the extended scan. Commands will be sent in the sequence of scan parameter and scan enable.
 *            The stack will not set the scanning parameter if the scan parameter is NULL.
 * @param[in] param        is a pointer to the extended scan paramters, it can be NULL.
 * @param[in] enable       is a switch for the scanner to enable or disable or change the timing of scanning, it can be NULL.
 * @param[in] callback     the callback to get the scan result, #bt_gap_le_srv_event_cb_t.
 * @return       #BT_STATUS_SUCCESS, the operation completed successfully, otherwise it failed.
 *               #BT_STATUS_BUSY, the operation is failed because BT is busy on another operation.
 *               #BT_STATUS_OUT_OF_MEMORY, out of memory.
 */
bt_status_t bt_gap_le_srv_set_extended_scan(bt_gap_le_srv_set_extended_scan_parameters_t *param, bt_gap_le_srv_set_extended_scan_enable_t *enable, void *callback);

#ifdef __cplusplus
 }
#endif

/**
 * @}
 * @}
 */

#endif /* __BT_GAP_LE_SERVICE_H__ */


