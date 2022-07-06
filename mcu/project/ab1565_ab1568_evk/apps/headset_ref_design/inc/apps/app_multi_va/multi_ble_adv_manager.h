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

/**
 * File: multi_ble_adv_manager.h
 *
 * Description: This file defines the function and types of multi_ble_adv_manager.c.
 *
 */

#ifndef __MULTI_BLE_ADV_MANAGER_H__
#define __MULTI_BLE_ADV_MANAGER_H__

#include "bt_gap_le.h"
#include <stdbool.h>
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif

/**
 *  @brief This enum defines the multi_adv instance ID, the module support multi instance BLE adv.
 */
typedef enum {
    MULTI_ADV_INSTANCE_DEFAULT,     /**< The instance ID for Airoha UT app adv and voice asssitant adv. */
#ifdef __BT_FAST_PAIR_ENABLE__
    MULTI_ADV_INSTANCE_FAST_PAIR,   /**< The instance ID for fast pair. */
#endif
#ifdef AIR_LE_AUDIO_ENABLE
    MULTI_ADV_INSTANCE_NOT_RHO,     /**< The instance ID for which adv need boardcasted in partner and do not switch connection handle when RHO */
#endif
#ifdef AIR_TILE_ENABLE
    MULTI_ADV_INSTANCE_TILE,        /**< The instance ID for which adv need boardcasted in partner and do not switch connection handle when RHO */
#endif
#ifdef AIR_SWIFT_PAIR_ENABLE
    MULTI_ADV_INSTANCE_SWIFT_PAIR,  /**< The instance ID for Swift Pair. */
#endif
    MULTI_ADV_INSTANCE_MAX_COUNT    /**< The count of instance, it's useful to create an array. */
} multi_adv_instance_t;

/**
 * @brief Define the maximum le connection count of this software..
 */
#define MULTI_ADV_MAX_CONN_COUNT    (2)

/**
 *  @brief This structure defines the BLE adv information, it include parameter, data and scan response.
 */
typedef struct {
    bt_hci_le_set_ext_advertising_parameters_t *adv_param;  /**< The BLE adv parameter. */
    bt_gap_le_set_ext_advertising_data_t *adv_data;         /**< The BLE adv data. */
    bt_gap_le_set_ext_scan_response_data_t *scan_rsp;       /**< The BLE adv scan response. */
} multi_ble_adv_info_t;

/**
 *  @brief This enum defines result value of multi_ble_adv_manager_pause_ble_adv().
 */
typedef enum {
    MULTI_ADV_PAUSE_RESULT_HAVE_STOPPED,    /**< Pausing BLE adv is done. */
    MULTI_ADV_PAUSE_RESULT_WAITING,         /**< Pausing BLE adv is successful, but need wait the BT_GAP_LE_SET_ADVERTISING_CNF event. */
} multi_adv_pause_result_t;

/**
 * @brief Define the result value of get_ble_adv_data_func_t.
 */
#define MULTI_BLE_ADV_NEED_GEN_ADV_PARAM    (1)         /**< Need generate a default adv parameter. */
#define MULTI_BLE_ADV_NEED_GEN_ADV_DATA     (1 << 1)    /**< Need generate a default adv data. */
#define MULTI_BLE_ADV_NEED_GEN_SCAN_RSP     (1 << 2)    /**< Need generate a default adv scan response. */

/**
 *  @brief      This function type defines the format of the callback which is implemented to get adv information.
 *  @param[out] adv_info, the implemented function should fill the contains of the parameter adv_info.
 *  @return     The bits of the return value means adv_param, adv_data or scan_rsp need generated by multi_ble_adv_manager.
 */
typedef uint32_t (*get_ble_adv_data_func_t)(multi_ble_adv_info_t *adv_info);

/**
 *  @brief This function type defines the format of the callback which is implemented to receive BLE connection is disconnected.
 */
typedef void (*ble_disconnected_callback_t)(void);

/**
 * @brief      This function start all BLE adv. Normally it should be called when BT power on.
 */
void multi_ble_adv_manager_start_ble_adv(void);

/**
 * @brief      This function stop all BLE adv. Normally it should be called when BT power off.
 */
void multi_ble_adv_manager_stop_ble_adv(void);

/**
 * @brief      This function can pause ble adv. It is for testing mode.
 */
multi_adv_pause_result_t multi_ble_adv_manager_pause_ble_adv(void);

/**
 * @brief      This function can resume ble adv. It is for testing mode.
 */
void multi_ble_adv_manager_resume_ble_adv(void);

/**
 * @brief      This function add an adv. If add more than 1 adv in a instance, the multi_ble_adv_manager will switch them per SWITCH_ADV_TYPE_MS microseconds.
 * @param[in]  instance, the instance id to add in.
 * @param[in]  get_ble_adv_data, the callback function to get adv information. If the function is duplicated, it will be ignored.
 * @param[in]  adv_weight, when multi adv data are sharing an adv instance, the weight of switching adv data.
 * @return     If the count of added adv is larger than MAX_BLE_DATA, return false to means adding fail.
 */
bool multi_ble_adv_manager_add_ble_adv(multi_adv_instance_t instance, get_ble_adv_data_func_t get_ble_adv_data, uint8_t adv_weight);

/**
* @brief      This function remove an added adv.
* @param[in]  instance, the instance id to add in.
* @param[in]  get_ble_adv_data, the callback function to get adv information.
* @return     If the callback function have not be added, return false.
*/
bool multi_ble_adv_manager_remove_ble_adv(multi_adv_instance_t instance, get_ble_adv_data_func_t get_ble_adv_data);

/**
 * @brief      This function notify adding or removing is done.
 * @param[in]  instance, the instance id which has been added or removed adv.
 */
void multi_ble_adv_manager_notify_ble_adv_data_changed(multi_adv_instance_t instance);

/**
 * @brief      This function process EVENT_GROUP_UI_SHELL_BT events.
 * @param[in]  event_id, the event id of received events.
 * @param[in]  extra_data, the extra data of received events.
 * @param[in]  data_len, the extra data length of received events.
 */
void multi_ble_adv_manager_bt_event_proc(uint32_t event_id, void *extra_data, size_t data_len);

/**
 * @brief      This function process EVENT_GROUP_UI_SHELL_MULTI_VA events.
 * @param[in]  event_id, the event id of received events.
 * @param[in]  extra_data, the extra data of received events.
 * @param[in]  data_len, the extra data length of received events.
 */
void multi_ble_adv_manager_multi_va_proc(uint32_t event_id, void *extra_data, size_t data_len);

/**
 * @brief      This function process EVENT_GROUP_UI_SHELL_APP_INTERACTION events.
 * @param[in]  event_id, the event id of received events.
 * @param[in]  extra_data, the extra data of received events.
 * @param[in]  data_len, the extra data length of received events.
 */
void multi_ble_adv_manager_interaction_proc(uint32_t event_id, void *extra_data, size_t data_len);

/**
 * @brief      This function process EVENT_GROUP_UI_SHELL_LE_SERVICE events.
 * @param[in]  event_id, the event id of received events.
 * @param[in]  extra_data, the extra data of received events.
 * @param[in]  data_len, the extra data length of received events.
 */
bool multi_ble_adv_manager_le_service_proc(uint32_t event_id, void *extra_data, size_t data_len);

/**
 * @brief      This function send BLE random address to partner side. Call it when AWS connected.
 */
void multi_ble_adv_manager_sync_ble_addr(void);

/**
 * @brief      This function get the random address and advertising handle.
 * @param[in]  instance, the instance id.
 * @param[out] bt_bd_addr_t, the ble random address of the instance.
 * @param[out] p_handle, the advertising handle of the instance, the function will set it's value.
 * @return     true means successful, else means parameter is not correct.
 */
bool multi_ble_adv_manager_get_random_addr_and_adv_handle(multi_adv_instance_t instance,
                                                          bt_bd_addr_t *random_addr,
                                                          bt_gap_le_advertising_handle_t *p_handle);

#ifdef MTK_AWS_MCE_ENABLE
/**
 * @brief      This function process EVENT_GROUP_UI_SHELL_AWS_DATA events.
 * @param[in]  event_id, the event id of received events.
 * @param[in]  extra_data, the extra data of received events.
 * @param[in]  data_len, the extra data length of received events.
 */
bool multi_ble_adv_manager_aws_data_proc(uint32_t unused_id, void *extra_data, size_t data_len);
#endif

/**
* @brief      This function disconnect all connected BLE devices.
* @param[in]  callback, it will be called when all ble connections are disconnected.
*/
bt_status_t multi_ble_adv_manager_disconnect_ble(ble_disconnected_callback_t callback);

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
/**
 * @brief      This function is to receive the notification of any RHO events.
 * @param[in]  addr, is the peer address before RHO. In Agent part, it is the phone's address. In Partner part, it is Agent's address.
 * @param[in]  role, is the role before RHO. In Agent part, it is Agent role. In Partner part, it is Partner role.
 * @param[in]  event, is the event for the RHO phases.
 * @param[in]  status, is the status of the RHO phases.
 * @return     #BT_STATUS_SUCCESS, the operation completed successfully.
 */
void multi_ble_rho_status_callback(const bt_bd_addr_t *addr,
                                   bt_aws_mce_role_t role,
                                   bt_role_handover_event_t event,
                                   bt_status_t status);
#endif

/**
 * @brief      This function set the flag of supporting multi adv data.
 *             If called this function, user can add more than 1 adv data in this instance.
 * @param[in]  instance, the instance id.
 */
void multi_ble_adv_manager_set_support_multi_adv_data(multi_adv_instance_t instance);

/**
 * @brief      This function set the maximum count of le connection of this adv instance.
 *             The function should only be called when system on.
 * @param[in]  instance, the instance id.
 * @param[in]  max_count, the maximum count of le connetion.
 */
void multi_ble_adv_manager_set_le_connection_max_count(multi_adv_instance_t instance, uint8_t max_count);

/**
 * @brief      This function initialize the multi_ble_adv_manager.
 */
void multi_ble_adv_manager_init(void);

#endif /* __MULTI_BLE_ADV_MANAGER_H__ */
