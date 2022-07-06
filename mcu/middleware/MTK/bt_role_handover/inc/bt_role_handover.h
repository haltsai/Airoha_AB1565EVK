/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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


#ifndef __BT_ROLE_HANDOVER_H__
#define __BT_ROLE_HANDOVER_H__

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_RHO BT Role Handover
 * @{
 * This section defines the Bluetooth role handover API to manage all Bluetooth role handover users. The module depends on the BT AWS_MCE module.
 * @{
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b RHO                        | Role Handover.                                                         |
 *
 * @section bt_role_handover_api_usage How to use this module
 *
 * - A user that meets any of the following conditions must register the RHO module.
 *  - 1) It triggers the RHO procedure.
 *  - 2) It uses BLE or SPP because the RHO module needs these modules to disconnect the BLE link, stop advertising or
 *        disconnect the SPP connection during RHO due to RHO's limitation.
 *  - 3) It wants to decide whether RHO can be allowed in its scenario.
 *  - 4) It cares about the RHO results.
 * - The RHO module retrys after 100ms when AWS_MCE return BT_STATUS_BUSY. To guarante that the RHO data is real-time,
 *   The RHO module gets the RHO data again. Please make sure the data is up-to-date when the RHO module calls the get data callback function.
 *  - Example code:
 *   - 1. Add your own module type in this file .
 *     @code
 *     #define BT_ROLE_HANDOVER_MODULE_MY_APP        (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 4)
 *     @endcode
 *   - 2. Define the necessary callbacks and register them to the RHO module.
 *     @code
 *     static bool my_app_is_ongoing = false;
 *     void my_app_init(void)
 *     {
 *     #if defined(MTK_AWS_MCE_ENABLE)
 *         static void my_app_callback_register(void);
 *         my_app_callback_register();
 *     #endif
 *     }
 *
 *     void my_app_deinit(void)
 *     {
 *     #if defined(MTK_AWS_MCE_ENABLE)
 *         bt_role_handover_deregister_callbacks(BT_ROLE_HANDOVER_MODULE_MY_APP);
 *     #endif
 *     }
 *
 *     void my_app_start(void)
 *     {
 *         if (bt_role_handover_get_state() == BT_ROLE_HANDOVER_STATE_ONGOING)
 *         {
 *             return;
 *         }
 *         my_app_is_ongoing = true;
 *     }
 *
 *     void my_app_stop(void)
 *     {
 *         my_app_is_ongoing = false;
 *     }
 *
 *     #if defined(MTK_AWS_MCE_ENABLE)
 *     BT_PACKED(
 *     typedef struct {
 *         bool my_app_restart_flag;
 *     })my_app_rho_data_t;
 *
 *     static bool my_app_restart_flag = false;
 *     bt_status_t my_app_role_handover_allow_execution(const bt_bd_addr_t *addr)
 *     {
 *         if (my_app_is_ongoing) {
 *             my_app_restart_flag = true;
 *             return BT_STATUS_PENDING;
 *         } else {
 *             return BT_STATUS_SUCCESS;
 *         }
 *     }
 *
 *     uint8_t my_app_role_handover_get_length(const bt_bd_addr_t *addr)
 *     {
 *         if (my_app_restart_flag) {
 *             return sizeof(my_app_rho_data_t);
 *         } else {
 *             return 0;
 *         }
 *     }
 *
 *     bt_status_t my_app_role_handover_get_data(const bt_bd_addr_t *addr, void * data)
 *     {
 *         if (my_app_restart_flag) {
 *             my_app_rho_data_t *data_p = (my_app_rho_data_t *)data;
 *             data_p->my_app_restart_flag = my_app_restart_flag;
 *         }
 *         return BT_STATUS_SUCCESS;
 *     }
 *
 *     bt_status_t my_app_role_handover_update(bt_role_handover_update_info_t *info)
 *     {
 *         if (info->role == BT_AWS_MCE_ROLE_PARTNER) {
 *             my_app_rho_data_t *data_p = (my_app_rho_data_t *)info->data;
 *             my_app_restart_flag = data_p->my_app_restart_flag;
 *         }
 *         return BT_STATUS_SUCCESS;
 *     }
 *
 *     void my_app_role_handover_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
 *     {
 *         LOG_I(BT_APP, "RHO status_callback role 0x%x, event %d, status 0x%x, app_ongoing %d, app_restart_flag %d",
 *             role, event, status, my_app_is_ongoing, my_app_restart_flag);
 *         switch (event) {
 *             case BT_ROLE_HANDOVER_PREPARE_REQ_IND:
 *             {
 *                 if (role == BT_AWS_MCE_ROLE_AGENT && my_app_is_ongoing) {
 *                     my_app_stop();
 *                     bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_MY_APP);
 *                 }
 *                 break;
 *             }
 *             case BT_ROLE_HANDOVER_COMPLETE_IND:
 *             {
 *                 if (status == BT_STATUS_SUCCESS && my_app_adv_restart_flag && role == BT_AWS_MCE_ROLE_PARTNER) {
 *                     my_app_start();
 *                 }
 *                 if (status != BT_STATUS_SUCCESS) {
 *                     printf("RHO fail 0x%x, please check error code to get the details\n", status);
 *                 }
 *
 *                 my_app_restart_flag = false;
 *                 break;
 *             }
 *         }
 *     }
 *
 *     static void my_app_callback_register(void)
 *     {
 *         bt_role_handover_callbacks_t callbacks = {0};
 *         callbacks.allowed_cb = my_app_role_handover_allow_execution;
 *         callbacks.get_len_cb = my_app_role_handover_get_length;
 *         callbacks.get_data_cb = my_app_role_handover_get_data;
 *         callbacks.update_cb = my_app_role_handover_update;
 *         callbacks.status_cb = my_app_role_handover_status_callback;
 *         bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_MY_APP, &callbacks);
 *     }
 *     #endif
 *     @endcode
 *   - 3. Trigger RHO.
 *     @code
 *     bt_status_t status = bt_role_handover_start();
 *     if (status != BT_STATUS_SUCCESS) {
 *         printf("RHO fail 0x%x, please check error code to get the details\n", status);
 *     }
 *     @endcode
 *      */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "bt_type.h"
#include "bt_aws_mce.h"
#include "bt_custom_type.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @defgroup BluetoothServices_RHO_define Define
 * @{
 * Define the RHO module types.
 */

/**
 * @brief Define the module type to register.
 * For the middleware modules, please define the module type by using #BT_ROLE_HANDOVER_MODULE_START and offset.
 * For the applications, please define the module type by using #BT_ROLE_HANDOVER_MODULE_CUSTOM_START and offset.
 * The RHO module callbacks to these users by the ascending order, except when the RHO module updates the data by the
 * callback #bt_role_handover_status_callback_t in Agent part. If there is dependency between the modules in the same group,
 * please obey the lower layer first rule and adjust the define order.
 */
#define BT_ROLE_HANDOVER_MODULE_START           (0x0)                                       /**< The start value of the RHO module type. */
#define BT_ROLE_HANDOVER_MODULE_RANGE           (0x20)                                      /**< The maximum supported number of the RHO module type. */
#define BT_ROLE_HANDOVER_MODULE_RHO_SRV         (BT_ROLE_HANDOVER_MODULE_START)             /**< The module type of RHO Service. */
#define BT_ROLE_HANDOVER_MODULE_EXTERNAL_TIMER  (BT_ROLE_HANDOVER_MODULE_START + 1)         /**< The module type of BT external time. */
#define BT_ROLE_HANDOVER_MODULE_SINK_CM         (BT_ROLE_HANDOVER_MODULE_START + 2)         /**< The module type of sink connection manager. */
#define BT_ROLE_HANDOVER_MODULE_AWS_MCE_SRV     (BT_ROLE_HANDOVER_MODULE_START + 3)         /**< The module type of AWS MCE Service. */
#define BT_ROLE_HANDOVER_MODULE_SINK_MUSIC      (BT_ROLE_HANDOVER_MODULE_START + 4)         /**< The module type of sink music. */
#define BT_ROLE_HANDOVER_MODULE_SINK_CALL       (BT_ROLE_HANDOVER_MODULE_START + 5)         /**< The module type of sink call. */
#define BT_ROLE_HANDOVER_MODULE_RACE_CMD        (BT_ROLE_HANDOVER_MODULE_START + 6)         /**< The module type of RACE command. */
#define BT_ROLE_HANDOVER_MODULE_FOTA            (BT_ROLE_HANDOVER_MODULE_START + 7)         /**< The module type of FOTA. */
#define BT_ROLE_HANDOVER_MODULE_FIND_ME         (BT_ROLE_HANDOVER_MODULE_START + 8)         /**< The module type of Find Me. */
#define BT_ROLE_HANDOVER_MODULE_BT_AIR          (BT_ROLE_HANDOVER_MODULE_START + 9)         /**< The module type of BT air. */
#define BT_ROLE_HANDOVER_MODULE_GSOUND          (BT_ROLE_HANDOVER_MODULE_START + 10)        /**< The module type of Gsound. */
#define BT_ROLE_HANDOVER_MODULE_AMA             (BT_ROLE_HANDOVER_MODULE_START + 11)        /**< The module type of AMA. */
#define BT_ROLE_HANDOVER_MODULE_VA_XIAOWEI      (BT_ROLE_HANDOVER_MODULE_START + 12)        /**< The module type of VA xiaowei. */
#define BT_ROLE_HANDOVER_MODULE_IAP2            (BT_ROLE_HANDOVER_MODULE_START + 13)        /**< The module type of iAP2. */
#define BT_ROLE_HANDOVER_MODULE_VA_XIAOAI       (BT_ROLE_HANDOVER_MODULE_START + 14)        /**< The module type of VA xiaoai. */
#define BT_ROLE_HANDOVER_MODULE_FAST_PAIR       (BT_ROLE_HANDOVER_MODULE_START + 15)        /**< The module type of fast pair. */
#define BT_ROLE_HANDOVER_MODULE_AIRUPDATE       (BT_ROLE_HANDOVER_MODULE_START + 16)        /**< The module type of Airupdate. */
#define BT_ROLE_HANDOVER_MODULE_BLE_AIR         (BT_ROLE_HANDOVER_MODULE_START + 17)        /**< The module type of BLE air. */
#define BT_ROLE_HANDOVER_MODULE_BLE_SRV         (BT_ROLE_HANDOVER_MODULE_START + 18)        /**< The module type of BT GAP LE SRV. */
#define BT_ROLE_HANDOVER_MODULE_BLE_DM          (BT_ROLE_HANDOVER_MODULE_START + 19)        /**< The module type of BLE Device Manager. */
#define BT_ROLE_HANDOVER_MODULE_PORT_SERVICE_BT (BT_ROLE_HANDOVER_MODULE_START + 20)        /**< The module type of port service bt */
#define BT_ROLE_HANDOVER_MODULE_GATT_SERVICE    (BT_ROLE_HANDOVER_MODULE_START + 21)        /**< The module type of GATT Database */
#define BT_ROLE_HANDOVER_MODULE_GATT_OVER_BREDR_AIR (BT_ROLE_HANDOVER_MODULE_START + 22)    /**< The module type of GATT OVER BR/EDR air. */
#define BT_ROLE_HANDOVER_MODULE_AMS             (BT_ROLE_HANDOVER_MODULE_START + 23)        /**< The module type of BLE AMS. */
#define BT_ROLE_HANDOVER_MODULE_ULL             (BT_ROLE_HANDOVER_MODULE_START + 24)        /**< The module type of ULL. */
#define BT_ROLE_HANDOVER_MODULE_MS_TEAMS        (BT_ROLE_HANDOVER_MODULE_START + 25)        /**< The module type of ms teams. */
#define BT_ROLE_HANDOVER_MODULE_MS_CFU          (BT_ROLE_HANDOVER_MODULE_START + 26)        /**< The module type of ms cfu. */
#define BT_ROLE_HANDOVER_MODULE_RACE_BT         (BT_ROLE_HANDOVER_MODULE_START + 27)        /**< The module type of RACE BT. */

#define BT_ROLE_HANDOVER_MODULE_CUSTOM_START    (BT_ROLE_HANDOVER_MODULE_START + BT_ROLE_HANDOVER_MODULE_RANGE) /**< The start value of the customized RHO module type. */
#define BT_ROLE_HANDOVER_MODULE_CUSTOM_RANGE    (0x10)                                          /**< The maximum supported number of the RHO module type. */
#define BT_ROLE_HANDOVER_MODULE_CHARGER_CASE    (BT_ROLE_HANDOVER_MODULE_CUSTOM_START)          /**< The module type of charger case. 0x10*/
#define BT_ROLE_HANDOVER_MODULE_BATTERY         (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 1)      /**< The module type of battery. */
#define BT_ROLE_HANDOVER_MODULE_BT_CMD          (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 2)      /**< The module type of BT command. */
#define BT_ROLE_HANDOVER_MODULE_BLE_APP         (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 3)      /**< The module type of BLE application. */
#define BT_ROLE_HANDOVER_MODULE_RACE_APP        (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 4)      /**< The module type of BLE application. */
#define BT_ROLE_HANDOVER_MODULE_VP_APP          (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 5)      /**< The module type of vp application. */
#define BT_ROLE_HANDOVER_MODULE_IN_EAR          (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 6)      /**< The module type of in-ear detection. */
#define BT_ROLE_HANDOVER_MODULE_MUSIC           (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 7)      /**< The module type of music application. */
#define BT_ROLE_HANDOVER_MODULE_L2CAP_DEMO_PROFILE  (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 8)  /**< The module type of l2cap demo profile application. */
#define BT_ROLE_HANDOVER_MODULE_GATT_DISCOVERY  (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 9)      /**< The module type of GATT discovery. */
#define BT_ROLE_HANDOVER_MODULE_ANCS_APP        (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 10)     /**< The module type of BLE ancs application. */
#define BT_ROLE_HANDOVER_MODULE_TILE_APP        (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 11)     /**< The module type of Tile application. */
#define BT_ROLE_HANDOVER_MODULE_SHARE_APP       (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 12)     /**< The module type of share app */
#define BT_ROLE_HANDOVER_MODULE_APP_MULTI_ADV   (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 13)     /**< The module type of Multi ADV. */
#define BT_ROLE_HANDOVER_MODULE_HID_APP         (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 14)     /**< The module type of HID app. */
#define BT_ROLE_HANDOVER_MODULE_ADVANCE_PT_APP  (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + 15)     /**< The module type of Advance Passthrough app. */
#define BT_ROLE_HANDOVER_MODULE_MAX             (BT_ROLE_HANDOVER_MODULE_CUSTOM_START + BT_ROLE_HANDOVER_MODULE_CUSTOM_RANGE) /**< The maximum module type. */
typedef uint8_t bt_role_handover_module_type_t; /**< Type definition of the module. */


/**
 * @brief Define the events reported to the user.
 */
#define BT_ROLE_HANDOVER_START_IND       (0x00)    /**< This event indicates that someone triggers the RHO procedure.*/
#define BT_ROLE_HANDOVER_PREPARE_REQ_IND (0x01)    /**< It is for Agent only. This event indicates that the user in the Agent role can start to prepare for RHO procedure because all users are allowed to execute RHO, for example, disconnect the BLE link or SPP connection.*/
#define BT_ROLE_HANDOVER_COMPLETE_IND    (0x02)    /**< This event indicates that the RHO procedure is completed.*/
typedef uint8_t bt_role_handover_event_t;  /**< Type definition of the event. */

/**
 * @brief Define the status for RHO. It is the customized status of the BT status #bt_status_t for the RHO module.
 */
#define BT_STATUS_ROLE_HANDOVER_NOT_AGENT                      (BT_MODULE_CUSTOM_ROLE_HANDOVER | (BT_MODULE_CUSTOM_MASK & 0x00))  /**< RHO is not allowed when the local device is not Agent. */
#define BT_STATUS_ROLE_HANDOVER_ONGOING                        (BT_MODULE_CUSTOM_ROLE_HANDOVER | (BT_MODULE_CUSTOM_MASK & 0x01))  /**< RHO is not allowed when RHO is already ongoing. */
#define BT_STATUS_ROLE_HANDOVER_AWS_CONNECTION_NOT_FOUND       (BT_MODULE_CUSTOM_ROLE_HANDOVER | (BT_MODULE_CUSTOM_MASK & 0x02))  /**< RHO is not allowed when AWS_MCE connection cannot be found. */
#define BT_STATUS_ROLE_HANDOVER_LINK_NUM_EXCEED                (BT_MODULE_CUSTOM_ROLE_HANDOVER | (BT_MODULE_CUSTOM_MASK & 0x03))  /**< RHO is not allowed when AWS MCE connection excceed than max. */

/**
 * @brief Define the state for RHO procedure.
 */
#define BT_ROLE_HANDOVER_STATE_IDLE    (0x00)  /**< This state indicates that the RHO module is in idle state.*/
#define BT_ROLE_HANDOVER_STATE_ONGOING (0x01)  /**< This state indicates that the RHO procedure is ongoing.*/
typedef uint8_t bt_role_handover_state_t;      /**< Type definition of the state. */
/**
 * @}
 */


 /**
 * @defgroup Bluetoothbt_RHO_struct Struct
 * @{
 */

/**
 *  @brief This structure defines the parameters when RHO notify the users to update their context.
 */
typedef struct {
    const bt_bd_addr_t *addr;    /**< The peer address before RHO. In Agent part, it is the phone's address. In Partner part, it is Agent's address. */
    bt_aws_mce_role_t role;      /**< The role before RHO. In Agent part, it is Agent role. In Partner part, it is Partner role.*/
    const bt_aws_mce_role_handover_profile_t *profile_info; /**< The new profile information after RHO. It is NULL in new Partner part.*/
    const void * data;           /**< The RHO data buffer pointer for the specific user. It is NULL in new Partner part. The buffer pointer is not guaranteed to be four-byte aligned.*/
    uint8_t length;              /**< The RHO data length for the specific user. It is 0 in new Partner part.*/
    bool is_active;              /**< The Partner attach state of this address. */
}bt_role_handover_update_info_t;


/**
 * @}
 */


/**
 * @addtogroup BluetoothServices_RHO_define Define
 * @{
 * Define the RHO types.
 */

/**
 * @brief Define the callback functions to register.
 */

/**
 * @brief     This function is a callback to ask if RHO is allowed.
 * @param[in] addr          is the peer address before RHO. In Agent part, it is the phone's address. In Partner part, it is Agent's address.
 * @return                  #BT_STATUS_SUCCESS, if RHO is allowed.
 *                          #BT_STATUS_FAIL, if RHO is not allowed.
 *                          #BT_STATUS_PENDING, if RHO is allowed before something must first be prepared.
 *                          The users should start to prepare when they receive the event #BT_ROLE_HANDOVER_PREPARE_REQ_IND.
 */
typedef bt_status_t (*bt_role_handover_allow_execution_callback_t)(const bt_bd_addr_t *addr);

/**
 * @brief     This function is a callback to get RHO data length of this user.
 * @param[in] addr          is the peer address before RHO. In Agent part, it is the phone's address. In Partner part, it is Agent's address.
 * @return                  #BT_STATUS_SUCCESS, the operation completed successfully.
 */
typedef uint8_t (*bt_role_handover_get_data_length_callback_t)(const bt_bd_addr_t *addr);

/**
 * @brief     This function is a callback to get RHO data of this user.
 * @param[in] addr          is the peer address before RHO. In Agent part, it is the phone's address. In Partner part, it is Agent's address.
 * @param[in] data          is the data buffer pointer allocated by RHO. The user needs to copy the RHO data into this buffer. Suggest to use the PACK to define the data structure.
 * @return                  #BT_STATUS_SUCCESS, the operation completed successfully.
 */
typedef bt_status_t (*bt_role_handover_get_data_callback_t)(const bt_bd_addr_t *addr, void * data);

/**
 * @brief     This function is a callback to the user to update the context.
 * @param[in] info          is information that is necessary for the user to update the context. It is NULL for Agent part.
 * @return                  #BT_STATUS_SUCCESS, the operation completed successfully.
 */
typedef bt_status_t (*bt_role_handover_update_callback_t)(bt_role_handover_update_info_t *info);

/**
 * @brief     This function is to notify the user of any RHO events.
 * @param[in] addr          is the peer address before RHO. In Agent part, it is the phone's address. In Partner part, it is Agent's address.
 * @param[in] role          is the role before RHO. In Agent part, it is Agent role. In Partner part, it is Partner role.
 * @param[in] event         is the event for the RHO phases.
 * @param[in] status        is the status of the RHO phases.
 * @return                  #BT_STATUS_SUCCESS, the operation completed successfully.
 */
typedef void (*bt_role_handover_status_callback_t)(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status);

/**
 * @}
 */


 /**
 * @addtogroup Bluetoothbt_RHO_struct Struct
 * @{
 */

/**
 *  @brief This structure defines the callback functions for the user to register.
 */
typedef struct {
    bt_role_handover_allow_execution_callback_t allowed_cb; /**< This can be NULL if the user always allows to execute RHO at any time.*/
    bt_role_handover_get_data_length_callback_t get_len_cb; /**< This can be NULL if the user does not have any RHO data to send from Agent to Partner at any time.*/
    bt_role_handover_get_data_callback_t get_data_cb;       /**< This can be NULL if the user does not have any RHO data to send from Agent to Partner at any time.*/
    bt_role_handover_update_callback_t update_cb;           /**< This can be NULL if the user does not have any RHO data to send from Agent to Partner at any time and must update the context with provided information.*/
    bt_role_handover_status_callback_t status_cb;  /**< This is mandatory for all users so that all users can be notified of RHO events.*/
}bt_role_handover_callbacks_t;


/**
 * @}
 */

/**
 * @brief     This function registers a callback set to the RHO module. It is suggested to call this function when the module is initialized.
 * @param[in] type          is the module type to register.
 * @param[in] callbacks     is a member of the callback functions to register. These callback functions are invoked by the RHO module when the RHO procedure is ongoing.
 * @return                  #BT_STATUS_SUCCESS, if the operation is successful.
 *                          #BT_STATUS_FAIL, if the module type is incorrect or already registered.
 */
bt_status_t bt_role_handover_register_callbacks(bt_role_handover_module_type_t type, bt_role_handover_callbacks_t *callbacks);




/**
 * @brief     This function deregisters the callback from the RHO module. It is suggested to call this function when the module is deinitialized.
 * @param[in] type          is the module type to unregister.
 * @return                  #BT_STATUS_SUCCESS, if the operation is successful.
 *                          #BT_STATUS_FAIL, if the module type is incorrect or unregistered.
 */
bt_status_t bt_role_handover_deregister_callbacks(bt_role_handover_module_type_t type);



/**
 * @brief     This function is for triggering the RHO procedure.
 * @return             #BT_STATUS_SUCCESS, if the operation is successful.
 *                     #BT_STATUS_ROLE_HANDOVER_AWS_CONNECTION_NOT_FOUND, if no AWS MCE connection can be found.
 *                     #BT_STATUS_ROLE_HANDOVER_NOT_AGENT, if the current device is not an Agent.
 *                     #BT_STATUS_ROLE_HANDOVER_ONGOING, if RHO is already in process.
 *                     For other error codes in the RHO procedure, please refer to other definitions of bt_status_t.
 */
bt_status_t bt_role_handover_start(void);


/**
 * @brief     This function is to reply to the RHO module that the module, who returned BT_STATUS_PENDING in #bt_role_handover_allow_execution_callback_t function, is ready to execute RHO now .
 * @param[in] type   is the module type.
 * @return             #BT_STATUS_SUCCESS, if the operation is successful.
 *                     #BT_STATUS_FAIL, if the module type cannot be found.
 */
bt_status_t bt_role_handover_reply_prepare_request(bt_role_handover_module_type_t type);


/**
 * @brief     This function is to get the current state of the RHO module.
 * @return             #bt_role_handover_state_t,  the current state of the RHO module.
 */
bt_role_handover_state_t bt_role_handover_get_state(void);


#ifdef __cplusplus
}
#endif

/**
 * @}
 * @}
 * @}
 */


#endif /* __BT_ROLE_HANDOVER_H__ */

