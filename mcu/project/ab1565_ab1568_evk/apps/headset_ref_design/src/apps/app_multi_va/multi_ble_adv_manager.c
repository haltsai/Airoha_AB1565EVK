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
 * File: multi_ble_adv_manager.c
 *
 * Description: This file provides ability to process multi BLE adv. It supports multi instance and support multi adv
 * for every instance.
 * Note: Considering this file is a class, the adv instance means the instance of this class.
 *       The added adv means the file which implements the get_ble_adv_data_func_t and add it in a adv instance.
 *       The middleware code also support multi instance, use an instance id to indicate which instance in middleware.
 *
 */

#include "multi_ble_adv_manager.h"
#include "multi_va_manager.h"
#include "multi_va_event_id.h"
#include "bt_app_common.h"
#include "bt_sink_srv.h"
#include "bt_device_manager.h"
#include "bt_device_manager_test_mode.h"
#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_bt_event.h"
#include "apps_events_interaction_event.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "app_rho_idle_activity.h"
#endif
#include "bt_device_manager_le.h"
#include "bt_gap_le_service.h"
#include "nvdm.h"
#include "nvdm_id_list.h"
#include "apps_debug.h"
#include "apps_aws_sync_event.h"
#include "FreeRTOS.h"

#define LOG_TAG     "[MULTI_ADV]"   /* Log tag. */

#define MAX_BLE_DATA                (4)                     /* Maximum BLE adv count for 1 adv instance. */
#define MAX_LE_CONNECTION_COUNT     (3)                     /* Controller resource is limited, only support 3 connection. */
#define SWITCH_ADV_TYPE_MS          (200)                   /* The switching period if added more than 1 adv in a instance. */
#define ADV_POWER_SAVING_TIMEOUT    (30 * 1000)             /* The timeout for increasing the adv interval to save power. */

/**
 *  @brief This enum defines the state of the adv.
 */
typedef enum {
    MULTI_ADV_STATE_STOPPED      = 0,   /**< Stopped state. */
    MULTI_ADV_STATE_STOPPING,           /**< Temporay state of target is stopped but has not called stopping function. */
    MULTI_ADV_STATE_STARTING,           /**< Temporay state of target is started but has not called starting function. */
    MULTI_ADV_STATE_STARTED             /**< Started state. */
} multi_adv_state_t;

/**
 *  @brief This enum defines the state of adv interval mode.
 */
typedef enum {
    MULTI_ADV_INTERVAL_NORMAL,          /**< Not power saving mode. */
    MULTI_ADV_INTERVAL_POWER_SAVING,    /**< Power saving mode, the interval should be larger than normal. */
} multi_adv_s_interval_mode_t;

/**
 *  @brief This struct defines the context for 1 adv instance. 1 adv instance use 1 separeted context.
 */
typedef struct {
    multi_adv_s_interval_mode_t m_interval_mode;    /**< The interval mode, refer to multi_adv_s_interval_mode_t. */
    uint8_t m_adv_instance;                         /**< The instance id get from middleware. */
    uint8_t m_max_connection_count;                 /**< The maximum count of le connection. */
    bt_handle_t m_connection_handle[MULTI_ADV_MAX_CONN_COUNT];  /**< The connection handle. */
    bt_bd_addr_t m_ble_addr;                        /**< The BLE random address. */
#ifdef MTK_AWS_MCE_ENABLE
    bool m_conn_handle_not_rho;                     /**< The connection of this instance is BT_GAP_LE_SRV_LINK_TYPE_CUSTOMIZE, the connection will not switch to peer when RHO. */
#endif
    bt_addr_type_t m_adv_addr_type;                 /**< The adv address type. */

    multi_adv_state_t m_current_adv_state;          /**< The current adv state. */
    multi_adv_state_t m_target_adv_state;           /**< The target adv state. */
    bool m_need_update_adv_param;                   /**< The flag to update adv parameter. Normally set when added or removed adv. */
    bool m_connection_count_limit;                  /**< The flag to indicate the ble adv is effected by connection count limitation */
    bool m_support_multi_data;                      /**< The flag to indicate if this instance support multi adv data and switch the data cyclically. */
    uint16_t m_last_adv_interval;                   /**< The adv interval when last time update adv parameter. */

    get_ble_adv_data_func_t m_adv_func_array[MAX_BLE_DATA]; /**< The array to recored added adv. */
    uint8_t m_adv_weight[MAX_BLE_DATA];             /**< The adv weight of the corresponding adv function. value must be 1 ~ 255. */
    uint8_t m_current_adv_repeat_times;             /**< Increased by 1 every time the adv data not change. when it > m_adv_weight[N], need change to next adv data. */
    bt_gap_le_srv_get_adv_data_cb_t m_get_adv_cb;           /**< The callback function need to register to middleware. */
    uint32_t m_current_adv_id;  /**< Current id in m_adv_func_array, it's useful when switch adv data. */
    uint32_t m_adv_func_count;  /**< The count of added adv in m_adv_func_array. */
    bool m_adv_changed;         /**< The flag to record has added or removed adv after last notify_ble_adv_data_changed(), if it's false, the next notify_ble_adv_data_changed() will be ignored. */
} multi_adv_data_t;

/**
 *  @brief This struct defines data format of aws sync when RHO.
 */
typedef struct {
    bt_handle_t s_connection_handle;    /**< Connection handle, because device will not disconnect when RHO, need send connection handle to partner.  */
    bool le_connected;                  /**< The flag if BLE is connected.  */
} multi_adv_sync_conn_data_t;

/* The array of multi_ble_adv_manager instance. */
static multi_adv_data_t s_multi_adv_data[MULTI_ADV_INSTANCE_MAX_COUNT];

uint8_t s_pause_adv = 0;                        /* The pause flag, normally set for testing. */
bool s_ever_send_le_addr_to_partner = false;    /* The flag means has ever send BLE random address to partner. */
ble_disconnected_callback_t s_disconnected_cb = NULL;   /* The flag means has ever send BLE random address to partner. */

#if (defined(AIR_LE_AUDIO_ENABLE))
extern bt_status_t bt_le_audio_sink_set_adv_handle(uint8_t adv_handle);
#endif

/**
 * @brief      This function store BLE random address.
 */
static void multi_ble_adv_store_ble_addr(void)
{
    multi_adv_instance_t instance;
    uint8_t temp_buffer[sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT];
    /* Append all of the address of multi_ble_adv_manager instances into a buffer. */
    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        memcpy(temp_buffer + sizeof(bt_bd_addr_t) * instance,
               s_multi_adv_data[instance].m_ble_addr, sizeof(bt_bd_addr_t));
    }
    nvdm_write_data_item(NVDM_GROUP_MMI_APP, NVDM_MULTI_ADV_LE_ADDR, NVDM_DATA_ITEM_TYPE_RAW_DATA,
                         temp_buffer, sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT);
#ifdef AIR_LE_AUDIO_ENABLE
        bt_app_common_store_local_random_address(&(s_multi_adv_data[MULTI_ADV_INSTANCE_NOT_RHO].m_ble_addr));
#endif
}

/**
 * @brief      This function is get the next added adv and change the m_current_adv_id.
 * @param[in]  instance, the instance id of multi_ble_adv_manager.
 * @param[in]  change_id, false means just get the adv information to check; True means real use the adv data to start adv.
 * @return     The callback function of get adv information.
 */
static get_ble_adv_data_func_t multi_ble_adv_manager_get_adv_data(multi_adv_instance_t instance, bool change_id)
{
    get_ble_adv_data_func_t temp_func = NULL;
    size_t i;
    int start_id;
    uint32_t next_adv_id;
    if (0 == s_multi_adv_data[instance].m_adv_func_count) {
        APPS_LOG_MSGID_E(LOG_TAG"[%d], get_adv_data, no adv data", 1, instance);
        return NULL;
    } else if (s_multi_adv_data[instance].m_adv_func_count == 1
        || s_multi_adv_data[instance].m_current_adv_repeat_times < s_multi_adv_data[instance].m_adv_weight[s_multi_adv_data[instance].m_current_adv_id]) {
        start_id = s_multi_adv_data[instance].m_current_adv_id; /* Search from current id. Probely it is the same one. */
    } else {
        start_id = s_multi_adv_data[instance].m_current_adv_id + 1; /* Search from next id. */
    }
    for (i = 0; i < MAX_BLE_DATA; i++) {
        next_adv_id = (start_id + i) % MAX_BLE_DATA;
        temp_func = s_multi_adv_data[instance].m_adv_func_array[next_adv_id];
        if (temp_func) {
            if (change_id) {
                /* Update the m_current_adv_id */
                if (s_multi_adv_data[instance].m_current_adv_id != next_adv_id) {
                    s_multi_adv_data[instance].m_current_adv_id = next_adv_id;
                    s_multi_adv_data[instance].m_current_adv_repeat_times = 0;
                }
                if (s_multi_adv_data[instance].m_adv_func_count > 1) {
                    s_multi_adv_data[instance].m_current_adv_repeat_times++;
                }
            }
            break;
        }
    }

    APPS_LOG_MSGID_I(LOG_TAG"[%d], get_adv_data, info[%d]func(%x)", 3,
                     instance, s_multi_adv_data[instance].m_current_adv_id, temp_func);
    return temp_func;
}

/**
 * @brief      This function is called when middleware callback to APP to get adv data. It will fill the adv data which is callback from middleware.
 * @param[in]  instance, the instance id of multi_ble_adv_manager.
 * @param[in]  op, the operation type, refer to bt_gap_le_srv_adv_data_op_t.
 * @param[out] data, the pointer to the adv data need be filled.
 * @return     The callback function of get adv information.
 */
uint8_t multi_adv_manager_get_adv_data_cb(multi_adv_instance_t instance, bt_gap_le_srv_adv_data_op_t op, void *data)
{
    bt_hci_le_set_ext_advertising_parameters_t *p_temp_adv_param = NULL;
    bt_gap_le_set_ext_advertising_data_t *p_temp_adv_data = NULL;
    bt_gap_le_set_ext_scan_response_data_t *p_temp_scan_resp = NULL;
    uint32_t get_adv_info_result = 0;
    if (NULL == data) {
        APPS_LOG_MSGID_E(LOG_TAG"[%d], get_adv_data_cb, data is NULL", 1, instance);
        return BT_GAP_LE_ADV_NOTHING_GEN;
    }
    /* Get the get_adv_data callback from multi_ble_adv_manager instance. */
    get_ble_adv_data_func_t temp_func = multi_ble_adv_manager_get_adv_data(instance, true);
    if (!temp_func) {
        APPS_LOG_MSGID_E(LOG_TAG"[%d], get_adv_data_cb, no tem_func", 1, instance);
        return BT_GAP_LE_ADV_NOTHING_GEN;
    }
    if (BT_GAP_LE_SRV_ADV_DATA_OP_CONFIG == op) {
        /* When the op is BT_GAP_LE_SRV_ADV_DATA_OP_CONFIG, data is the pointer to a bt_gap_le_srv_adv_config_info_t. */
        bt_gap_le_srv_adv_config_info_t *config_info = (bt_gap_le_srv_adv_config_info_t *)data;
        multi_ble_adv_info_t adv_info = {
            .adv_param = &(config_info->adv_param),
            .adv_data = &(config_info->adv_data),
            .scan_rsp = &(config_info->scan_rsp)
        };
        get_adv_info_result = temp_func(&adv_info);
        /* generate default value depends on the get_adv_info_result value. */
        if (get_adv_info_result & MULTI_BLE_ADV_NEED_GEN_ADV_PARAM) {
            p_temp_adv_param = &(config_info->adv_param);
        }
        if (get_adv_info_result & MULTI_BLE_ADV_NEED_GEN_ADV_DATA) {
            p_temp_adv_data = &(config_info->adv_data);
        }
        if (get_adv_info_result & MULTI_BLE_ADV_NEED_GEN_SCAN_RSP) {
            p_temp_scan_resp = &(config_info->scan_rsp);
        }
        bt_app_common_generate_default_adv_data(
            p_temp_adv_param, p_temp_adv_data, p_temp_scan_resp,
            NULL, 0);

        /* If the get_adv_data callback doesn't set adv interval, use default interval */
        if (adv_info.adv_param->primary_advertising_interval_max == 0) {
            if (MULTI_ADV_INTERVAL_NORMAL == s_multi_adv_data[instance].m_interval_mode) {
                adv_info.adv_param->primary_advertising_interval_max = 0xA0;
                adv_info.adv_param->primary_advertising_interval_min = 0xA0;
            } else {
                adv_info.adv_param->primary_advertising_interval_max = 0x29C;
                adv_info.adv_param->primary_advertising_interval_min = 0x29C;
            }
        }
        s_multi_adv_data[instance].m_adv_addr_type = config_info->adv_param.own_address_type;

        /* Check if there is connection on this address. If yes, the adv type should not be connectable. */
        bt_addr_t bt_addr = {
            .type = BT_ADDR_RANDOM,
        };
        if (BT_ADDR_PUBLIC == s_multi_adv_data[instance].m_adv_addr_type
            || BT_ADDR_PUBLIC_IDENTITY == s_multi_adv_data[instance].m_adv_addr_type) {
            bt_addr.type = BT_ADDR_PUBLIC;
            memcpy(bt_addr.addr, bt_device_manager_get_local_address(), sizeof(bt_addr.addr));
        } else {
            bt_addr.type = BT_ADDR_RANDOM;
            memcpy(bt_addr.addr, s_multi_adv_data[instance].m_ble_addr, sizeof(bt_addr.addr));
        }

        bt_handle_t temp_handle_list[MULTI_ADV_MAX_CONN_COUNT];
        uint8_t temp_count = MULTI_ADV_MAX_CONN_COUNT;
        bt_gap_le_srv_error_t temp_err = bt_gap_le_srv_get_conn_handle_by_addr_ext(temp_handle_list, &temp_count, &bt_addr, true);
        if (BT_GAP_LE_SRV_SUCCESS == temp_err && temp_count >= s_multi_adv_data[instance].m_max_connection_count) {
            APPS_LOG_MSGID_I(LOG_TAG"[%d], get_adv_data_cb, already connected, temp_count = %d", 2,
                             instance, temp_count);
            adv_info.adv_param->advertising_event_properties &= ~BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE;
        } else if (bt_gap_le_srv_get_connected_dev_num() >= MAX_LE_CONNECTION_COUNT) {
            /* Controller resource is limted, so only support 3 connection. */
            adv_info.adv_param->advertising_event_properties &= ~BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE;
            APPS_LOG_MSGID_I(LOG_TAG"[%d], get_adv_data_cb, total connection count >= MAX_LE_CONNECTION_COUNT", 1, instance);
            s_multi_adv_data[instance].m_connection_count_limit = true;
        }
        return BT_GAP_LE_ADV_PARAM_GEN | BT_GAP_LE_ADV_DATA_GEN | (adv_info.scan_rsp->data_length > 0 ? BT_GAP_LE_ADV_SCAN_RSP_GEN : 0);
    } else if (BT_GAP_LE_SRV_ADV_DATA_OP_UPDATE == op) {
        /* When the op is BT_GAP_LE_SRV_ADV_DATA_OP_UPDATE, data is the pointer to a bt_gap_le_srv_adv_update_info_t. */
        bt_gap_le_srv_adv_update_info_t *update_info = (bt_gap_le_srv_adv_update_info_t *)data;
        /* The update_info not contents adv parameter. */
        multi_ble_adv_info_t adv_info = {
            .adv_param = NULL,
            .adv_data = &(update_info->adv_data),
            .scan_rsp = &(update_info->scan_rsp)
        };
        get_adv_info_result = temp_func(&adv_info);
        if (get_adv_info_result & MULTI_BLE_ADV_NEED_GEN_ADV_DATA) {
            p_temp_adv_data = &(update_info->adv_data);
        }
        if (get_adv_info_result & MULTI_BLE_ADV_NEED_GEN_SCAN_RSP) {
            p_temp_scan_resp = &(update_info->scan_rsp);
        }
        bt_app_common_generate_default_adv_data(
            p_temp_adv_param, p_temp_adv_data, p_temp_scan_resp,
            NULL, 0);
        return BT_GAP_LE_ADV_DATA_GEN | (adv_info.scan_rsp->data_length > 0 ? BT_GAP_LE_ADV_SCAN_RSP_GEN : 0);
    } else {
        return BT_GAP_LE_ADV_NOTHING_GEN;
    }
}

/**
 * @brief      This is the implementation of bt_gap_le_srv_get_adv_data_cb_t for multi_ble_adv_manager instance id is MULTI_ADV_INSTANCE_DEFAULT.
 * @param[in]  op, the operation type, refer to bt_gap_le_srv_adv_data_op_t.
 * @param[out] data, the pointer to the adv data need be filled.
 * @return     The callback function of get adv information.
 */
uint8_t multi_adv_manager_get_default_adv_data_cb(bt_gap_le_srv_adv_data_op_t op, void *data)
{
    return multi_adv_manager_get_adv_data_cb(MULTI_ADV_INSTANCE_DEFAULT, op, data);
}

#ifdef __BT_FAST_PAIR_ENABLE__
/**
 * @brief      This is the implementation of bt_gap_le_srv_get_adv_data_cb_t for multi_ble_adv_manager instance id is MULTI_ADV_INSTANCE_FAST_PAIR.
 * @param[in]  op, the operation type, refer to bt_gap_le_srv_adv_data_op_t.
 * @param[out] data, the pointer to the adv data need be filled.
 * @return     The callback function of get adv information.
 */
uint8_t multi_adv_manager_get_fast_pair_adv_data_cb(bt_gap_le_srv_adv_data_op_t op, void *data)
{
    return multi_adv_manager_get_adv_data_cb(MULTI_ADV_INSTANCE_FAST_PAIR, op, data);
}
#endif

#ifdef MTK_VA_XIAOAI_ENABLE
/**
 * @brief      This is the implementation of bt_gap_le_srv_get_adv_data_cb_t for multi_ble_adv_manager instance id is MULTI_ADV_INSTANCE_FAST_PAIR.
 * @param[in]  op, the operation type, refer to bt_gap_le_srv_adv_data_op_t.
 * @param[out] data, the pointer to the adv data need be filled.
 * @return     The callback function of get adv information.
 */
uint8_t multi_adv_manager_get_xiaoai_adv_data_cb(bt_gap_le_srv_adv_data_op_t op, void *data)
{
    return multi_adv_manager_get_adv_data_cb(MULTI_ADV_INSTANCE_XIAOAI, op, data);
}
#endif

#ifdef AIR_LE_AUDIO_ENABLE
/**
 * @brief      This is the implementation of bt_gap_le_srv_get_adv_data_cb_t for multi_ble_adv_manager instance id is MULTI_ADV_INSTANCE_NOT_RHO.
 * @param[in]  op, the operation type, refer to bt_gap_le_srv_adv_data_op_t.
 * @param[out] data, the pointer to the adv data need be filled.
 * @return     The callback function of get adv information.
 */
uint8_t multi_adv_manager_get_not_rho_adv_data_cb(bt_gap_le_srv_adv_data_op_t op, void *data)
{
    return multi_adv_manager_get_adv_data_cb(MULTI_ADV_INSTANCE_NOT_RHO, op, data);
}
#endif

#ifdef AIR_TILE_ENABLE
/**
 * @brief      This is the implementation of bt_gap_le_srv_get_adv_data_cb_t for multi_ble_adv_manager instance id is MULTI_ADV_INSTANCE_TILE.
 * @param[in]  op, the operation type, refer to bt_gap_le_srv_adv_data_op_t.
 * @param[out] data, the pointer to the adv data need be filled.
 * @return     The callback function of get adv information.
 */
uint8_t multi_adv_manager_get_tile_adv_data_cb(bt_gap_le_srv_adv_data_op_t op, void *data)
{
    return multi_adv_manager_get_adv_data_cb(MULTI_ADV_INSTANCE_TILE, op, data);
}
#endif

#ifdef AIR_SWIFT_PAIR_ENABLE
/**
 * @brief      This is the implementation of bt_gap_le_srv_get_adv_data_cb_t for multi_ble_adv_manager instance id is MULTI_ADV_INSTANCE_SWIFT_PAIR.
 * @param[in]  op, the operation type, refer to bt_gap_le_srv_adv_data_op_t.
 * @param[out] data, the pointer to the adv data need be filled.
 * @return     The callback function of get adv information.
 */
uint8_t multi_adv_manager_get_swift_pair_adv_data_cb(bt_gap_le_srv_adv_data_op_t op, void *data)
{
    return multi_adv_manager_get_adv_data_cb(MULTI_ADV_INSTANCE_SWIFT_PAIR, op, data);
}
#endif

/**
 * @brief      This function trigger starting ble adv.
 * @param[in]  instance, the instance id of multi_ble_adv_manager.
 */
static void multi_ble_adv_manager_do_start_ble_adv(multi_adv_instance_t instance)
{
    bt_hci_le_set_ext_advertising_parameters_t default_adv_param;
    bt_hci_le_set_ext_advertising_parameters_t *temp_adv_param = NULL;
    uint8_t ble_adv_instance = 0;
    /* Get adv state from middleware. */
    if (s_multi_adv_data[instance].m_adv_instance == 0) {
        bt_gap_le_srv_error_t le_err = bt_gap_le_srv_get_available_instance(&ble_adv_instance);
        APPS_LOG_MSGID_I(LOG_TAG"[%d], do_start_ble_adv get instance:%d: le_err %x", 3, instance, ble_adv_instance, le_err);
        if (le_err != BT_GAP_LE_SRV_SUCCESS) {
            APPS_LOG_MSGID_E(LOG_TAG"[%d], do_start_ble_adv get instance failed", 0);
            return;
        } else {
            for (uint8_t i = 0; i < MULTI_ADV_INSTANCE_MAX_COUNT; i++) {
                if (s_multi_adv_data[i].m_adv_instance == ble_adv_instance) {
                    APPS_LOG_MSGID_E(LOG_TAG"[%d], the ble_adv_instance is duplicate at [%d]", 2, instance, i);
                    s_multi_adv_data[i].m_adv_instance = 0;
#if (defined(AIR_LE_AUDIO_ENABLE))
                    if (i == MULTI_ADV_INSTANCE_NOT_RHO) {
                        bt_le_audio_sink_set_adv_handle(s_multi_adv_data[MULTI_ADV_INSTANCE_NOT_RHO].m_adv_instance);
                    }
#endif
                }
            }
        }
    } else {
        ble_adv_instance = s_multi_adv_data[instance].m_adv_instance;
    }
    bt_gap_le_srv_adv_state_t status = bt_gap_le_srv_get_adv_state(ble_adv_instance);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    uint8_t conn_id;
    bool still_connectable = false;
    for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id++) {
        if (s_multi_adv_data[instance].m_connection_handle[conn_id] == 0xFFFF) {
            still_connectable = true;
            break;
        }
    }

    if ((s_multi_adv_data[instance].m_target_adv_state == MULTI_ADV_STATE_STARTED && !s_pause_adv)
        && (BT_CM_LE_ADV_STATE_TYPE_STARTED == status || s_multi_adv_data[instance].m_support_multi_data || still_connectable || s_multi_adv_data[instance].m_adv_func_count > 1)
#ifdef MTK_AWS_MCE_ENABLE
        && (s_multi_adv_data[instance].m_conn_handle_not_rho || BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
    ) {
        /* If added multi adv in one instance, use the smallest interval in all adv data. */
        bool need_change_because_interval = false; /* When current adv interval is smaller than last, set to true. */
        bt_gap_le_srv_error_t bt_status;
        get_ble_adv_data_func_t ble_adv_func = multi_ble_adv_manager_get_adv_data(instance, false);
        if (BT_CM_LE_ADV_STATE_TYPE_STOPPED == status || BT_CM_LE_ADV_STATE_TYPE_STARTED == status
            || BT_CM_LE_ADV_STATE_TYPE_REMOVED == status) {
            multi_ble_adv_info_t adv_info = {
                &default_adv_param, NULL, NULL
            };
            memset(&default_adv_param, 0, sizeof(default_adv_param));

            if (ble_adv_func) {
                uint32_t gen_ret = ble_adv_func(&adv_info);
                uint16_t temp_interval = 0;

                if (gen_ret & MULTI_BLE_ADV_NEED_GEN_ADV_PARAM)  {
                    APPS_LOG_MSGID_I(LOG_TAG"[%d], adv_info.adv_param need gen default", 1, instance);
                    temp_adv_param = adv_info.adv_param;
                }
                bt_app_common_generate_default_adv_data(
                    temp_adv_param, NULL, NULL, NULL, 0);
                if (adv_info.adv_param->primary_advertising_interval_max == 0) {
                    if (MULTI_ADV_INTERVAL_NORMAL == s_multi_adv_data[instance].m_interval_mode) {
                        temp_interval = 0xA0;
                    } else {
                        temp_interval = 0x29C;
                    }
                } else {
                    temp_interval = adv_info.adv_param->primary_advertising_interval_max;
                }
                if (s_multi_adv_data[instance].m_last_adv_interval > temp_interval) {
                    need_change_because_interval = true;
                }
            }
            /* If already started and need change the adv parameter, must stop first. */
            if (BT_CM_LE_ADV_STATE_TYPE_STARTED == status
                && (need_change_because_interval || s_multi_adv_data[instance].m_need_update_adv_param)) {
                bt_status = bt_gap_le_srv_stop_adv(s_multi_adv_data[instance].m_adv_instance);
                /* If BLE adv is started, stop it first */
                if (bt_status == BT_GAP_LE_SRV_SUCCESS) {
                    APPS_LOG_MSGID_I(LOG_TAG"[%d], start adv, Successed to stop BLE adv:%d, %d", 3,
                                     instance, need_change_because_interval, s_multi_adv_data[instance].m_need_update_adv_param);
                    if (s_multi_adv_data[instance].m_adv_func_count > 0) {
                        s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STARTING;
                    } else {
                        s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
                    }
                } else {
                    APPS_LOG_MSGID_W(LOG_TAG"[%d], start adv, Fail to stop BLE adv", 1, instance);
                }
            } else {
                if (ble_adv_func) {
                    if (adv_info.adv_param->primary_advertising_interval_max == 0) {
                    } else {
                        if (s_multi_adv_data[instance].m_adv_func_count == 1) {
                            /* When current adv is fixed interval, not necessary to change the adv interval */
                            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                  MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);
                        }
                    }
                    if (BT_CM_LE_ADV_STATE_TYPE_STOPPED == status || BT_CM_LE_ADV_STATE_TYPE_REMOVED == status) {
                        /* If not started, start ble adv */
                        bt_gap_le_srv_adv_time_params_t time_param = {
                            .duration = 0,
                            .max_ext_advertising_evts = 0
                        };
                        s_multi_adv_data[instance].m_last_adv_interval = default_adv_param.primary_advertising_interval_max;
                        if (s_multi_adv_data[instance].m_adv_instance == 0) {
                            s_multi_adv_data[instance].m_adv_instance = ble_adv_instance;
                        }
#if (defined(AIR_LE_AUDIO_ENABLE))
                        if (instance == MULTI_ADV_INSTANCE_NOT_RHO) {
                            bt_le_audio_sink_set_adv_handle(s_multi_adv_data[MULTI_ADV_INSTANCE_NOT_RHO].m_adv_instance);
                        }
#endif
                        bt_status = bt_gap_le_srv_start_adv(ble_adv_instance,
                                                            s_multi_adv_data[instance].m_ble_addr,
                                                            &time_param, s_multi_adv_data[instance].m_get_adv_cb,
                                                            apps_bt_events_le_service_callback);
                        s_multi_adv_data[instance].m_need_update_adv_param = false;
                        APPS_LOG_MSGID_I(LOG_TAG"[%d], bt_gap_le_srv_start_adv status:%x", 2, instance, bt_status);
                    } else {
                        /* If has already started, update adv data and scan response. */
                        adv_info.adv_param = NULL;
                        bt_status = bt_gap_le_srv_update_adv(s_multi_adv_data[instance].m_adv_instance);
                        APPS_LOG_MSGID_I(LOG_TAG"[%d], bt_gap_le_srv_update_adv status:%x", 2, instance, bt_status);
                        if (s_multi_adv_data[instance].m_adv_func_count > 1) {
                            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                  MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + instance);
                            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                                                EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + instance,
                                                NULL, 0, NULL, SWITCH_ADV_TYPE_MS);
                        }
                    }
                    s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STARTED;
                } else {
                    APPS_LOG_MSGID_W(LOG_TAG"[%d], Cannot start adv, because ble_adv_info is NULL", 1, instance);
                    s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                          MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);
                }
            }
        } else {
            /* Need do start, but current status is busy, waiting busy status is finished and then start again. */
            APPS_LOG_MSGID_W(LOG_TAG"[%d], start adv, start BLE adv in busy status:%d", 2, instance, status);
            s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STARTING;
        }
    } else {
        /* Not agent or target state is not started. */
        s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
        APPS_LOG_MSGID_W(LOG_TAG"[%d], start adv, No need start, current target is : %d, status: %d, still_connectable: %d", 4, instance, s_multi_adv_data[instance].m_target_adv_state, status, still_connectable);
    }
}

/**
 * @brief      This function trigger stopping ble adv.
 * @param[in]  instance, the instance id of multi_ble_adv_manager.
 */
static void multi_ble_adv_manager_do_stop_ble_adv(multi_adv_instance_t instance)
{
    /* Get adv state from middleware. */
    bt_gap_le_srv_adv_state_t status = bt_gap_le_srv_get_adv_state(s_multi_adv_data[instance].m_adv_instance);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    if ((s_multi_adv_data[instance].m_target_adv_state == MULTI_ADV_STATE_STOPPED || s_pause_adv)
#ifdef MTK_AWS_MCE_ENABLE
        && (s_multi_adv_data[instance].m_conn_handle_not_rho || BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)   /* for partner, stop ADV */
#endif
       ) {
        if (BT_CM_LE_ADV_STATE_TYPE_STARTED == status) {
            /* If BLE adv is started, stop it. */
            bt_gap_le_srv_error_t bt_status = bt_gap_le_srv_stop_adv(s_multi_adv_data[instance].m_adv_instance);
            if (bt_status == BT_GAP_LE_SRV_SUCCESS) {
                APPS_LOG_MSGID_I(LOG_TAG"[%d], stop adv, Successed to stop BLE adv", 1, instance);
            } else {
                APPS_LOG_MSGID_W(LOG_TAG"[%d], stop adv, Fail to stop BLE adv", 1, instance);
            }
            s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
        } else if (BT_CM_LE_ADV_STATE_TYPE_STOPPED == status || BT_CM_LE_ADV_STATE_TYPE_REMOVED == status) {
            APPS_LOG_MSGID_W(LOG_TAG"[%d], stop adv, already stopped", 1, instance);
            s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
        } else {
            APPS_LOG_MSGID_W(LOG_TAG"[%d], stop adv, stop BLE adv in busy status", 1, instance);
            /* May be stopped by other app, so wait for the CNF to do next step. */
            s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPING;
        }
    } else {
        /* Not agent or target state is not stopped. */
        s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
        APPS_LOG_MSGID_W(LOG_TAG"[%d], stop adv, fail, current target is : %d", 2, instance, s_multi_adv_data[instance].m_target_adv_state);
    }
}

/**
 * @brief      This function start the multi_ble_adv_manger instance. normally called when BT enabled.
 * @param[in]  instance, the instance id of multi_ble_adv_manager.
 */
static void multi_ble_adv_manager_start_ble_adv_by_instance(multi_adv_instance_t instance)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    s_multi_adv_data[instance].m_target_adv_state = MULTI_ADV_STATE_STARTED;
    s_multi_adv_data[instance].m_interval_mode = MULTI_ADV_INTERVAL_NORMAL;
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                          MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);

#ifdef MTK_AWS_MCE_ENABLE
    if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
    {
        s_multi_adv_data[instance].m_last_adv_interval = 0xFFFF;
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                            EVENT_GROUP_UI_SHELL_MULTI_VA,
                            MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance,
                            NULL, 0, NULL, ADV_POWER_SAVING_TIMEOUT);
    }

    multi_ble_adv_manager_do_start_ble_adv(instance);
}

void multi_ble_adv_manager_start_ble_adv(void)
{
    multi_adv_instance_t instance;
    if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
        && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
            && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
        APPS_LOG_MSGID_I(LOG_TAG", Cannot start_ble_adv when test mode", 0);
        return;
    }
    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        /* Get le service instance, every multi_ble_adv_manager has one le service instance. */
        multi_ble_adv_manager_start_ble_adv_by_instance(instance);
    }
}

void multi_ble_adv_manager_stop_ble_adv(void)
{
    multi_adv_instance_t instance;
    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        s_multi_adv_data[instance].m_target_adv_state = MULTI_ADV_STATE_STOPPED;
        s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                              MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                              MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + instance);
        multi_ble_adv_manager_do_stop_ble_adv(instance);
        s_multi_adv_data[instance].m_adv_instance = 0;
    }
#if (defined(AIR_LE_AUDIO_ENABLE))
    bt_le_audio_sink_set_adv_handle(s_multi_adv_data[MULTI_ADV_INSTANCE_NOT_RHO].m_adv_instance);
#endif
}

multi_adv_pause_result_t multi_ble_adv_manager_pause_ble_adv(void)
{
    multi_adv_pause_result_t ret = MULTI_ADV_PAUSE_RESULT_HAVE_STOPPED;
    bt_gap_le_srv_adv_state_t adv_state;
    multi_adv_instance_t instance;
    s_pause_adv++;

    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        adv_state = bt_gap_le_srv_get_adv_state(s_multi_adv_data[instance].m_adv_instance);
        APPS_LOG_MSGID_I(LOG_TAG"[%d], pause adv, current = %d, state = %d, s_pause_adv = %d", 4,
                         instance, s_multi_adv_data[instance].m_current_adv_state, adv_state, s_pause_adv);
        if (s_multi_adv_data[instance].m_current_adv_state == MULTI_ADV_STATE_STOPPED
            && (BT_CM_LE_ADV_STATE_TYPE_STOPPED == adv_state || BT_CM_LE_ADV_STATE_TYPE_REMOVED == adv_state)) {

        } else {
            if (s_pause_adv <= 1) {
                multi_ble_adv_manager_do_stop_ble_adv(instance);
            }
            ret = MULTI_ADV_PAUSE_RESULT_WAITING;
        }
    }
    return ret;
}

void multi_ble_adv_manager_resume_ble_adv(void)
{
    bt_gap_le_srv_adv_state_t adv_state;
    multi_adv_instance_t instance;
    if (s_pause_adv) {
        s_pause_adv--;
    }

    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        adv_state = bt_gap_le_srv_get_adv_state(s_multi_adv_data[instance].m_adv_instance);
        APPS_LOG_MSGID_I(LOG_TAG"[%d], resume adv, current = %d, state = %d, s_pause_adv = %d", 4,
                         instance, s_multi_adv_data[instance].m_current_adv_state, adv_state, s_pause_adv);
        if (s_pause_adv == 0) {
            multi_ble_adv_manager_do_start_ble_adv(instance);
        }
    }
}

bool multi_ble_adv_manager_add_ble_adv(multi_adv_instance_t instance, get_ble_adv_data_func_t get_ble_adv_func, uint8_t adv_weight)
{
    size_t i;
    size_t duplicate_id = 0xFF;
    size_t new_id = 0xFF;
    bool ret = true;
    if (adv_weight == 0) {
        /* The adv weight must be >= 1. */
        adv_weight = 1;
    }

    for (i = 0; i < MAX_BLE_DATA; i++) {
        if (NULL == s_multi_adv_data[instance].m_adv_func_array[i] && new_id >= MAX_BLE_DATA) {
            new_id = i;
        } else if (s_multi_adv_data[instance].m_adv_func_array[i] == get_ble_adv_func) {
            duplicate_id = i;
            break;
        }
    }

    if (duplicate_id < MAX_BLE_DATA) {
        APPS_LOG_MSGID_I(LOG_TAG"[%d], add_ble_adv is duplicate at [%d] for %x, weight: %d",
                         4, instance, duplicate_id, get_ble_adv_func, adv_weight);
        s_multi_adv_data[instance].m_adv_weight[duplicate_id] = adv_weight;
    } else if (new_id < MAX_BLE_DATA) {
        APPS_LOG_MSGID_I(LOG_TAG"[%d], add_ble_adv new at [%d] for %x, weight: %d",
                         4, instance, new_id, get_ble_adv_func, adv_weight);
        s_multi_adv_data[instance].m_adv_func_array[new_id] = get_ble_adv_func;
        s_multi_adv_data[instance].m_adv_weight[new_id] = adv_weight;
        s_multi_adv_data[instance].m_adv_func_count++;
        /* Set the m_adv_changed when add successfully. */
        s_multi_adv_data[instance].m_adv_changed = true;
    } else {
        APPS_LOG_MSGID_E(LOG_TAG"add_ble_adv fail, current s_adv_func_count = %d",
                         1, s_multi_adv_data[instance].m_adv_func_count);
        ret = false;
    }

    return ret;
}

bool multi_ble_adv_manager_remove_ble_adv(multi_adv_instance_t instance, get_ble_adv_data_func_t get_ble_adv_func)
{
    size_t i;
    bool ret = false;
    for (i = 0; i < MAX_BLE_DATA; i++) {
        if (s_multi_adv_data[instance].m_adv_func_array[i] == get_ble_adv_func) {
            s_multi_adv_data[instance].m_adv_func_array[i] = NULL;
            s_multi_adv_data[instance].m_adv_weight[i] = 0;
            s_multi_adv_data[instance].m_adv_func_count--;
            ret = true;
            /* Set the m_adv_changed when remove successfully. */
            s_multi_adv_data[instance].m_adv_changed = true;
            APPS_LOG_MSGID_I(LOG_TAG"[%d], remove_ble_adv success, id = %d for %x, s_adv_func_count = %d",
                             4, instance, i, get_ble_adv_func, s_multi_adv_data[instance].m_adv_func_count);
            break;
        }
    }

    if (!ret) {
        APPS_LOG_MSGID_E(LOG_TAG"[%d], remove_ble_adv fail, current s_adv_func_count = %d",
                         2, instance, s_multi_adv_data[instance].m_adv_func_count);
    }

    return ret;
}

void multi_ble_adv_manager_notify_ble_adv_data_changed(multi_adv_instance_t instance)
{
    /* If current have not started the multi_ble_adv_manager, ignore the notify. */
    if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state) {
        /* If the m_adv_changed is false, ignore the notify. */
        if (!s_multi_adv_data[instance].m_adv_changed) {
            APPS_LOG_MSGID_I(LOG_TAG "[%d], notify_ble_adv_data_changed but not changed", 1, instance);
            return;
        }
        s_multi_adv_data[instance].m_need_update_adv_param = true;
        s_multi_adv_data[instance].m_last_adv_interval = 0xFFFF;
        s_multi_adv_data[instance].m_current_adv_repeat_times = 0;
#ifdef MTK_AWS_MCE_ENABLE
        bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
        if (s_multi_adv_data[instance].m_conn_handle_not_rho || BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
        {
            s_multi_adv_data[instance].m_interval_mode = MULTI_ADV_INTERVAL_NORMAL;
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                  MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);

            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                                EVENT_GROUP_UI_SHELL_MULTI_VA,
                                MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance,
                                NULL, 0, NULL, ADV_POWER_SAVING_TIMEOUT);
            multi_ble_adv_manager_do_start_ble_adv(instance);
        }
        s_multi_adv_data[instance].m_adv_changed = false;
    }
}

void multi_ble_adv_manager_bt_event_proc(uint32_t event_id, void *extra_data, size_t data_len)
{
    apps_bt_event_data_t *bt_event_data = (apps_bt_event_data_t *)extra_data;

    if (!bt_event_data) {
        APPS_LOG_MSGID_E(LOG_TAG"BT event, bt_event_data is NULL", 0);
        return;
    }
    switch (event_id) {
        case BT_GAP_LE_DISCONNECT_IND: {
            /* Restart adv when BLE disconnected. */
            multi_adv_instance_t instance;
            uint8_t conn_id;
            bt_gap_le_disconnect_ind_t *disc_ind = (bt_gap_le_disconnect_ind_t *)(bt_event_data->buffer);
            bool have_ble_connection = false;
            if (!disc_ind) {
                APPS_LOG_MSGID_E(LOG_TAG"BLE disconnected, disc_ind is NULL", 0);
                break;
            }
#if 0/* #ifdef MTK_AWS_MCE_ENABLE */
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
            {
                /* Use for loop to find which multi_ble_adv_manager instance. */
                for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                    for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id ++) {
                        if (disc_ind->connection_handle == s_multi_adv_data[instance].m_connection_handle[conn_id]) {
                            APPS_LOG_MSGID_I(LOG_TAG "[%d], BLE disconnected, s_target_adv_state = %d", 2,
                                             instance, s_multi_adv_data[instance].m_target_adv_state);
                            s_multi_adv_data[instance].m_connection_handle[conn_id] = 0xFFFF;
                            /* When connected status, the adv parameter is non-connectable, so must restart to update parameter. */
                            s_multi_adv_data[instance].m_need_update_adv_param = true;
                            if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state) {
                                s_multi_adv_data[instance].m_interval_mode = MULTI_ADV_INTERVAL_NORMAL;
                                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                      MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);
                                ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                                                    EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                    MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance,
                                                    NULL, 0, NULL, ADV_POWER_SAVING_TIMEOUT);
                                multi_ble_adv_manager_do_start_ble_adv(instance);
                            }
                            break;
                        }
                    }
                    if (conn_id < s_multi_adv_data[instance].m_max_connection_count) {
                        break;
                    } else if (s_multi_adv_data[instance].m_connection_count_limit) {
                        /* When le disconnected, recover the adv which are effected by connection count limitation. */
                        s_multi_adv_data[instance].m_need_update_adv_param = true;
                        s_multi_adv_data[instance].m_connection_count_limit = false;
                        if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state) {
                            s_multi_adv_data[instance].m_interval_mode = MULTI_ADV_INTERVAL_NORMAL;
                            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                  MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);
                            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                                                EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance,
                                                NULL, 0, NULL, ADV_POWER_SAVING_TIMEOUT);
                            multi_ble_adv_manager_do_start_ble_adv(instance);
                        }
                    }
                }
                if (instance >= MULTI_ADV_INSTANCE_MAX_COUNT) {
                    APPS_LOG_MSGID_I(LOG_TAG"BLE disconnected, but connection_handle not match: %d", 1, disc_ind->connection_handle);
                }

                /* Notify bt cm when all ble connection are disconnected to do BT power off. */
                if (s_disconnected_cb) {
                    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT && !have_ble_connection; instance++) {
                        for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id ++) {
                            if (s_multi_adv_data[instance].m_connection_handle[conn_id] != 0xFFFF) {
                                have_ble_connection = true;
                                break;
                            }
                        }
                    }
                    if (!have_ble_connection) {
                        s_disconnected_cb();
                    }
                }
            }
#if 0/* #ifdef MTK_AWS_MCE_ENABLE */
            else {
                APPS_LOG_MSGID_I(LOG_TAG"Now should not restart ble adv: current role = %x", 1,
                                 bt_device_manager_aws_local_info_get_role());
            }
#endif
            break;
        }
        case BT_GAP_LE_CONNECT_IND: {
            bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)(bt_event_data->buffer);
            multi_adv_instance_t instance;
            uint8_t conn_id;
            if (!connection_ind) {
                APPS_LOG_MSGID_E(LOG_TAG"BLE connected, connection_ind is NULL", 0);
                break;
            }
            /* Use for loop to find which multi_ble_adv_manager instance. */
            for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id ++) {
                    if (connection_ind->connection_handle == s_multi_adv_data[instance].m_connection_handle[conn_id]) {
                        APPS_LOG_MSGID_I(LOG_TAG "[%d], BLE connected[conn_id:%d], s_target_adv_state = %d", 3,
                                         instance, conn_id, s_multi_adv_data[instance].m_target_adv_state);
                        break;
                    }
                }
                if (conn_id < s_multi_adv_data[instance].m_max_connection_count) {
                    break;
                }
            }
            if (instance >= MULTI_ADV_INSTANCE_MAX_COUNT) {
                APPS_LOG_MSGID_I(LOG_TAG"BLE connected, but connection_handle not match: %d", 1, connection_ind->connection_handle);
            }
            break;
        }
        default:
            break;
    }
}

void multi_ble_adv_manager_multi_va_proc(uint32_t event_id, void *extra_data, size_t data_len)
{
    multi_adv_instance_t instance = MULTI_ADV_INSTANCE_MAX_COUNT;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    if (event_id >= MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL
        && event_id <= MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL_END) {
        instance = event_id - MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL;
        event_id = MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL;
    } else if (event_id >= MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE
               && event_id <= MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE_END) {
        instance = event_id - MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE;
        event_id = MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE;
    }

    switch (event_id) {
        case MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL:
            APPS_LOG_MSGID_I(LOG_TAG"[%d] MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL", 1, instance);
            if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state
#ifdef MTK_AWS_MCE_ENABLE
                && (s_multi_adv_data[instance].m_conn_handle_not_rho || BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
               ) {
                s_multi_adv_data[instance].m_interval_mode = MULTI_ADV_INTERVAL_POWER_SAVING;
                s_multi_adv_data[instance].m_need_update_adv_param = true; /* Set flag to update parameter. */
                if (s_multi_adv_data[instance].m_adv_func_count > 1) {
                    APPS_LOG_MSGID_I(LOG_TAG"[%d], MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL, have multi adv, wait for it switch", 1, instance);
                } else {
                    multi_ble_adv_manager_do_start_ble_adv(instance);
                }
            } else {
                APPS_LOG_MSGID_E(LOG_TAG"[%d], MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL, why s_target_adv_state is not started", 1, instance);
            }
            break;
        case MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE:
            APPS_LOG_MSGID_I(LOG_TAG"[%d], MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE", 1, instance);
            if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state
#ifdef MTK_AWS_MCE_ENABLE
                && (s_multi_adv_data[instance].m_conn_handle_not_rho || BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
               ) {
                multi_ble_adv_manager_do_start_ble_adv(instance);
            } else {
                APPS_LOG_MSGID_E(LOG_TAG"[%d], MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE, why s_target_adv_state is not started", 1, instance);
            }
            break;
        default:
            break;
    }
}

void multi_ble_adv_manager_interaction_proc(uint32_t event_id, void *extra_data, size_t data_len)
{
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    multi_adv_instance_t instance;
    switch (event_id) {
        case APPS_EVENTS_INTERACTION_RHO_STARTED: {
            bt_aws_mce_role_t role = (uint32_t)extra_data;
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
                APPS_LOG_MSGID_I(LOG_TAG"agent role switch, stop ble adv", 0);
                for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                    s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
                    multi_ble_adv_manager_do_stop_ble_adv(instance);
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                          MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                          MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + instance);
                }
            }
        }
            break;
        case APPS_EVENTS_INTERACTION_RHO_END: {
            app_rho_result_t status = (app_rho_result_t)extra_data;
            if (APP_RHO_RESULT_SUCCESS == status) {
                /* multi_adv_sync_conn_data_t *p_sync_conn_data = NULL; */
                APPS_LOG_MSGID_I(LOG_TAG"agent -> partner, stop ble adv", 0);
                for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                    s_multi_adv_data[instance].m_current_adv_state = MULTI_ADV_STATE_STOPPED;
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                          MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                          MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + instance);
                    /*
                    p_sync_conn_data = (multi_adv_sync_conn_data_t *)(send_data + sizeof(multi_adv_sync_conn_data_t) * instance);
                    p_sync_conn_data->s_connection_handle = s_multi_adv_data[instance].m_connection_handle;
                    */
                    if (s_multi_adv_data[instance].m_conn_handle_not_rho) {
                        if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state) {
                            multi_ble_adv_manager_start_ble_adv_by_instance(instance);
                        }
                    } else {
                        uint8_t conn_id = 0;
                        for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id++) {
                            s_multi_adv_data[instance].m_connection_handle[conn_id] = 0xFFFF;
                        }
                    }
                }
                /* apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_MULTI_VA,
                        MULTI_VA_EVENT_SYNC_BLE_CONN_STATE, send_data, sizeof(send_data));
                        */
            } else {
                APPS_LOG_MSGID_E(LOG_TAG"agent rho failed, restart ble adv", 0);
                for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                    if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state) {
                        multi_ble_adv_manager_start_ble_adv_by_instance(instance);
                    }
                }
            }
        }
            break;
        case APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT: {
            app_rho_result_t status = (app_rho_result_t)extra_data;
            bt_addr_t bt_addr = {
                .type = BT_ADDR_RANDOM,
            };
            if (APP_RHO_RESULT_SUCCESS == status) {
                APPS_LOG_MSGID_I(LOG_TAG"partner -> agent, start ble adv", 0);
                for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                    if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state) {
                        if (s_multi_adv_data[instance].m_conn_handle_not_rho) {
                            APPS_LOG_MSGID_I(LOG_TAG"[%d], this connection not RHO, handle = %x", 2, instance, s_multi_adv_data[instance].m_connection_handle[0]);
                        } else {
                            if (BT_ADDR_PUBLIC == s_multi_adv_data[instance].m_adv_addr_type
                                || BT_ADDR_PUBLIC_IDENTITY == s_multi_adv_data[instance].m_adv_addr_type) {
                                bt_addr.type = BT_ADDR_PUBLIC;
                                memcpy(bt_addr.addr, bt_device_manager_get_local_address(), sizeof(bt_addr.addr));
                            } else {
                                bt_addr.type = BT_ADDR_RANDOM;
                                memcpy(bt_addr.addr, s_multi_adv_data[instance].m_ble_addr, sizeof(bt_addr.addr));
                            }
                            bt_handle_t temp_handle_list[MULTI_ADV_MAX_CONN_COUNT];
                            uint8_t temp_count = MULTI_ADV_MAX_CONN_COUNT;
                            uint8_t conn_id;
                            bt_gap_le_srv_error_t temp_err = bt_gap_le_srv_get_conn_handle_by_addr_ext(temp_handle_list, &temp_count, &bt_addr, true);
                            for (conn_id = 0; BT_GAP_LE_SRV_SUCCESS == temp_err && conn_id < temp_count; conn_id++) {
                                if (temp_handle_list[conn_id] < 0xFFFF && conn_id < s_multi_adv_data[instance].m_max_connection_count) {
                                    s_multi_adv_data[instance].m_connection_handle[conn_id] = temp_handle_list[conn_id];
                                    APPS_LOG_MSGID_I(LOG_TAG"[%d], get ble connected, handle = %x", 2, instance, temp_handle_list[conn_id]);
                                } else {
                                    APPS_LOG_MSGID_W(LOG_TAG"[%d], get ble connected more than max, handle[%d] = %x", 3, instance, conn_id, temp_handle_list[conn_id]);
                                }
                            }
                        }
                        multi_ble_adv_manager_start_ble_adv_by_instance(instance);
                    }
                }
            } else {
                APPS_LOG_MSGID_E(LOG_TAG"partner rho failed, ignore", 0);
            }
        }
            break;
        default:
            break;
    }
#endif
}

bool multi_ble_adv_manager_le_service_proc(uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case BT_GAP_LE_SRV_EVENT_ADV_COMPLETE: {
            bt_gap_le_srv_adv_complete_t *adv_complete = (bt_gap_le_srv_adv_complete_t *)extra_data;
            multi_adv_instance_t instance;
            uint8_t conn_id;
            for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                if (adv_complete->instance == s_multi_adv_data[instance].m_adv_instance) {
                    break;
                }
            }
            if (instance >= MULTI_ADV_INSTANCE_MAX_COUNT) {
                APPS_LOG_MSGID_I(LOG_TAG"BT_GAP_LE_SRV_EVENT_ADV_COMPLETE adv_evt not match instance(%d), event:%x", 2, adv_complete->instance, adv_complete->adv_evt);
                return ret;
            }
            APPS_LOG_MSGID_I(LOG_TAG"[%d], BT_GAP_LE_SRV_EVENT_ADV_COMPLETE adv_evt:%x", 2, instance, adv_complete->adv_evt);

            if (adv_complete->conn_handle && BT_GAP_LE_SRV_ADV_STOPPED == adv_complete->adv_evt) {
                for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id++) {
                    if (s_multi_adv_data[instance].m_connection_handle[conn_id] == 0xFFFF) {
                        s_multi_adv_data[instance].m_connection_handle[conn_id] = adv_complete->conn_handle;
                        APPS_LOG_MSGID_I(LOG_TAG"[%d], BT_GAP_LE_SRV_EVENT_ADV_COMPLETE conn_handle[%d]:%x", 3, instance, conn_id, adv_complete->conn_handle);
                        break;
                    }
                }
                if (conn_id >= s_multi_adv_data[instance].m_max_connection_count) {
                    APPS_LOG_MSGID_W(LOG_TAG"[%d], BT_GAP_LE_SRV_EVENT_ADV_COMPLETE too many conn_handle: %x", 2, instance, adv_complete->conn_handle);
                }
            }
            switch (adv_complete->adv_evt) {
                case BT_GAP_LE_SRV_ADV_STARTED:
                case BT_GAP_LE_SRV_ADV_STOPPED:
                    if (adv_complete->conn_handle && BT_GAP_LE_SRV_ADV_STOPPED == adv_complete->adv_evt) {
                        /* When BLE connected, middleware will stop adv automatically. */
                        APPS_LOG_MSGID_I(LOG_TAG"[%d], BT_GAP_LE_SRV_EVENT_ADV_COMPLETE ADV stopped means le connected.", 1, instance);
                        bool still_connectable = false;
                        for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id++) {
                            if (s_multi_adv_data[instance].m_connection_handle[conn_id] == 0xFFFF) {
                                still_connectable = true;
                                break;
                            }
                        }
                        if (still_connectable) {
                            multi_ble_adv_manager_do_start_ble_adv(instance);
                        }
                    }
                    /* If have unfinished action, need do it. */
                    if (MULTI_ADV_STATE_STOPPING == s_multi_adv_data[instance].m_current_adv_state
                        || MULTI_ADV_STATE_STARTING == s_multi_adv_data[instance].m_current_adv_state) {
                        if (MULTI_ADV_STATE_STARTING == s_multi_adv_data[instance].m_current_adv_state || MULTI_ADV_STATE_STOPPING == s_multi_adv_data[instance].m_current_adv_state) {
                            APPS_LOG_MSGID_I(LOG_TAG "[%d], BT_GAP_LE_SRV_EVENT_ADV_COMPLETE and need start adv:%d", 2, instance, s_multi_adv_data[instance].m_target_adv_state);
                            if (s_multi_adv_data[instance].m_target_adv_state) {
                                multi_ble_adv_manager_do_start_ble_adv(instance);
                            } else {
                                multi_ble_adv_manager_do_stop_ble_adv(instance);
                            }
                        }
                    } else {
                        /* Send events when adv started successfully when added multi adv in an instance. */
                        if (BT_GAP_LE_SRV_ADV_STARTED == adv_complete->adv_evt
                            && s_multi_adv_data[instance].m_adv_func_count > 1) {
                            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                  MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + instance);
                            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                                                EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + instance,
                                                NULL, 0, NULL, SWITCH_ADV_TYPE_MS);
                        }

                        /* When result is BT_GAP_LE_SRV_ERROR_CONN_LIMIT_EXCEEDED, need restart adv. */
                        if (BT_GAP_LE_SRV_ADV_STOPPED == adv_complete->adv_evt
                            && BT_GAP_LE_SRV_ERROR_CONN_LIMIT_EXCEEDED == adv_complete->result) {
                            APPS_LOG_MSGID_I(LOG_TAG"[%d], stop when BT_GAP_LE_SRV_ERROR_CONN_LIMIT_EXCEEDED", 1, instance);
                            multi_ble_adv_manager_do_start_ble_adv(instance);
                        }
                    }
                    break;
                case BT_GAP_LE_SRV_ADV_UPDATED:

                    break;
                case BT_GAP_LE_SRV_ADV_FORCE_RESTART: {
                    bool still_connectable = false;
                    for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id++) {
                        if (s_multi_adv_data[instance].m_connection_handle[conn_id] == 0xFFFF) {
                            still_connectable = true;
                            break;
                        }
                    }
                    if (s_multi_adv_data[instance].m_target_adv_state) {
                        if (!still_connectable
                            && s_multi_adv_data[instance].m_adv_func_count <= 1
                            && !s_multi_adv_data[instance].m_support_multi_data) {
                            APPS_LOG_MSGID_I(LOG_TAG"[%d], BT_GAP_LE_SRV_ADV_FORCE_RESTART but already connected.", 1, instance);
                        } else {
                            APPS_LOG_MSGID_I(LOG_TAG"[%d], BT_GAP_LE_SRV_ADV_FORCE_RESTART restart adv.", 1, instance);
                            multi_ble_adv_manager_do_start_ble_adv(instance);
                        }
                    }
                    break;
                }
            }
            break;
        }
        case BT_GAP_LE_SRV_EVENT_CONN_UPDATED: {

            break;
        }

    }
    return ret;
}

#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
void multi_ble_adv_manager_sync_ble_addr(void)
{
    multi_adv_instance_t instance;
    uint8_t send_data[sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT];
    if (s_ever_send_le_addr_to_partner) {
        APPS_LOG_MSGID_I(LOG_TAG"Already synced ble addr to partner", 0);
    }
    /* Append all address of instances in the send_data buffer. */
    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        memcpy(send_data + sizeof(bt_bd_addr_t) * instance, s_multi_adv_data[instance].m_ble_addr, sizeof(bt_bd_addr_t));
    }
    if (BT_STATUS_SUCCESS == apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                            MULTI_VA_EVENT_SYNC_BLE_ADDRESS, send_data, sizeof(send_data))) {
        s_ever_send_le_addr_to_partner = true;
    }
}
#endif

bool multi_ble_adv_manager_get_random_addr_and_adv_handle(multi_adv_instance_t instance,
                                                          bt_bd_addr_t *random_addr,
                                                          bt_gap_le_advertising_handle_t *p_handle)
{
    if (instance < MULTI_ADV_INSTANCE_MAX_COUNT) {
        if (p_handle) {
            *p_handle = s_multi_adv_data[instance].m_adv_instance; /* The instance value is the same as adv handle */
        }
        if (random_addr) {
            memcpy(random_addr, s_multi_adv_data[instance].m_ble_addr, sizeof(bt_bd_addr_t));
        }
        return true;
    } else {
        return false;
    }

}

#ifdef MTK_AWS_MCE_ENABLE
bool multi_ble_adv_manager_aws_data_proc(
    uint32_t unused_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        multi_adv_instance_t instance;
        uint32_t event_group;
        uint32_t event_id;
        void *aws_extra_data;
        uint32_t extra_len;
        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event_id, &aws_extra_data, &extra_len);

        switch (event_group) {
            case EVENT_GROUP_UI_SHELL_MULTI_VA:
                switch (event_id) {
                    case MULTI_VA_EVENT_SYNC_BLE_ADDRESS: {
                        /* Partner received the BLE address from agent */
                        bt_bd_addr_ptr_t received_addr;
                        bool need_restore = false;
                        bool need_sync_paired_addr = false;
                        configASSERT(extra_len == sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT);
                        for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                            received_addr = (bt_bd_addr_ptr_t)((uint8_t *)aws_extra_data + sizeof(bt_bd_addr_t) * instance);
                            APPS_LOG_MSGID_I(LOG_TAG"[%d/%d], MULTI_VA_EVENT_SYNC_BLE_ADDRESS [%02X:%02X:%02X:%02X:%02X:%02X], local [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", 14,
                                             instance,
                                             s_multi_adv_data[instance].m_conn_handle_not_rho,
                                             received_addr[0], received_addr[1], received_addr[2],
                                             received_addr[3], received_addr[4], received_addr[5],
                                             s_multi_adv_data[instance].m_ble_addr[0],
                                             s_multi_adv_data[instance].m_ble_addr[1],
                                             s_multi_adv_data[instance].m_ble_addr[2],
                                             s_multi_adv_data[instance].m_ble_addr[3],
                                             s_multi_adv_data[instance].m_ble_addr[4],
                                             s_multi_adv_data[instance].m_ble_addr[5]);

                            if (!s_multi_adv_data[instance].m_conn_handle_not_rho) {
                                if (memcmp(received_addr, s_multi_adv_data[instance].m_ble_addr, sizeof(bt_bd_addr_t)) != 0) {
                                    APPS_LOG_MSGID_I(LOG_TAG"[%d], Need Update received addr", 1, instance);
                                    memcpy(s_multi_adv_data[instance].m_ble_addr, received_addr, sizeof(bt_bd_addr_t));
                                    need_restore = true;
                                }
                            } else if (memcmp(received_addr, s_multi_adv_data[instance].m_ble_addr, sizeof(bt_bd_addr_t)) == 0) {
                                /* To avoid the partner address is the same as agent. When it's not_rho formart. */
                                uint8_t conn_id;
                                bt_app_common_generate_random_address(s_multi_adv_data[instance].m_ble_addr);
                                APPS_LOG_MSGID_I(LOG_TAG"[%d], regen s_ble_addr [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", 7,
                                                 instance,
                                                 s_multi_adv_data[instance].m_ble_addr[0],
                                                 s_multi_adv_data[instance].m_ble_addr[1],
                                                 s_multi_adv_data[instance].m_ble_addr[2],
                                                 s_multi_adv_data[instance].m_ble_addr[3],
                                                 s_multi_adv_data[instance].m_ble_addr[4],
                                                 s_multi_adv_data[instance].m_ble_addr[5]);
                                for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id++) {
                                    s_multi_adv_data[instance].m_connection_handle[conn_id] = 0xFFFF;
                                }
                                need_restore = true;
                                need_sync_paired_addr = true;
                            }
                            /*
                            if (MULTI_ADV_STATE_STARTED == s_multi_adv_data[instance].m_target_adv_state) {
                                APPS_LOG_MSGID_I(LOG_TAG"[%d], Restart adv after RHO", 1, instance);
                                multi_ble_adv_manager_start_ble_adv_by_instance(instance);
                            }
                            */
                        }
                        if (need_restore) {
                            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                                                EVENT_GROUP_UI_SHELL_MULTI_VA,
                                                MULTI_VA_EVENT_NOTIFY_BLE_ADDR_CHANGED,
                                                NULL, 0, NULL, 0);
                            multi_ble_adv_store_ble_addr();
                        }
                        if (need_sync_paired_addr) {
#ifdef AIR_LE_AUDIO_ENABLE
                            bt_app_common_sync_random_addr();
#endif
                        }
                        s_ever_send_le_addr_to_partner = true;
                        ret = true;
                        break;
                    }
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
    return ret;
}
#endif

bt_status_t multi_ble_adv_manager_disconnect_ble(ble_disconnected_callback_t callback)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_status_t disconnect_ret = BT_STATUS_SUCCESS;
    multi_adv_instance_t instance;
    uint8_t conn_id;
    bt_hci_cmd_disconnect_t disconnect_para = {
        .connection_handle = 0xFFFF,
        .reason = BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION,
    };
    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        for (conn_id = 0; conn_id < s_multi_adv_data[instance].m_max_connection_count; conn_id ++) {
            if (s_multi_adv_data[instance].m_connection_handle[conn_id] < 0xFFFF) {
                disconnect_para.connection_handle = s_multi_adv_data[instance].m_connection_handle[conn_id];
                disconnect_ret = bt_gap_le_disconnect(&disconnect_para);
                if (BT_STATUS_SUCCESS == disconnect_ret) {
                    ret = BT_STATUS_PENDING;
                    s_disconnected_cb = callback;
                } else {
                    /* Since the disconnect failed, should not effect power off flow. */
                    s_multi_adv_data[instance].m_connection_handle[conn_id] = 0xFFFF;
                }
                APPS_LOG_MSGID_I(LOG_TAG"[%d], Disconnect BLE: handle %x, result: %x", 3, instance, disconnect_para.connection_handle, disconnect_ret);
            }
        }
    }

    return ret;
}

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
void multi_ble_rho_status_callback(const bt_bd_addr_t *addr,
                                   bt_aws_mce_role_t role,
                                   bt_role_handover_event_t event,
                                   bt_status_t status)
{
    /* To avoid timing issue, when callback in BT task, remove the multi va events */
    multi_adv_instance_t instance;
    if (BT_ROLE_HANDOVER_START_IND == event) {
        for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                  MULTI_VA_EVENT_CHANGE_BLE_ADV_INTERVAL + instance);
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_MULTI_VA,
                                  MULTI_VA_EVENT_CHANGE_BLE_ADV_TYPE + instance);
        }
    }
}
#endif

void multi_ble_adv_manager_set_support_multi_adv_data(multi_adv_instance_t instance)
{
    if (instance < MULTI_ADV_INSTANCE_MAX_COUNT) {
        s_multi_adv_data[instance].m_support_multi_data = true;
        APPS_LOG_MSGID_I(LOG_TAG"[%d], set_support_multi_adv_data", 1, instance);
    }
}

void multi_ble_adv_manager_set_le_connection_max_count(multi_adv_instance_t instance, uint8_t max_count)
{
    if (instance < MULTI_ADV_INSTANCE_MAX_COUNT) {
        configASSERT(max_count <= MULTI_ADV_MAX_CONN_COUNT);
        s_multi_adv_data[instance].m_max_connection_count = max_count;
        APPS_LOG_MSGID_I(LOG_TAG "[%d], set_le_connection_max_count = %d", 2, instance, max_count);
    }
}

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
static bt_gap_le_srv_link_attribute_t multi_ble_adv_get_link_attribute_callback(const bt_addr_t *local_addr)
{
    bt_gap_le_srv_link_attribute_t attribute = BT_GAP_LE_SRV_LINK_ATTRIBUTE_NEED_RHO;
    multi_adv_instance_t instance;
    bt_bd_addr_t *real_used_bt_addr;
    for(instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        if (BT_ADDR_PUBLIC == s_multi_adv_data[instance].m_adv_addr_type
            || BT_ADDR_PUBLIC_IDENTITY == s_multi_adv_data[instance].m_adv_addr_type) {
            real_used_bt_addr = bt_device_manager_get_local_address();
        } else {
            real_used_bt_addr = &s_multi_adv_data[instance].m_ble_addr;
        }
        if (memcmp(local_addr->addr, real_used_bt_addr, sizeof(bt_bd_addr_t)) == 0) {
            if (s_multi_adv_data[instance].m_conn_handle_not_rho) {
                attribute = BT_GAP_LE_SRV_LINK_ATTRIBUTE_NOT_NEED_RHO;
            }
            break;
        }
    }
    return attribute;
}

uint8_t multi_ble_adv_rho_get_data_len_callback(const bt_bd_addr_t *addr)
{
    uint8_t length = sizeof(bt_addr_type_t) * MULTI_ADV_INSTANCE_MAX_COUNT;
    return length;
}

bt_status_t multi_ble_adv_rho_get_data_callback(const bt_bd_addr_t *addr, void * data)
{
    uint32_t instance;
    uint32_t data_shift = 0;
    if (!data) {
        return BT_STATUS_FAIL;
    }

    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        memcpy((uint8_t *)data + data_shift, &s_multi_adv_data[instance].m_adv_addr_type, sizeof(bt_addr_type_t));
        data_shift += sizeof(bt_addr_type_t);
        APPS_LOG_MSGID_I(LOG_TAG"multi_ble_adv_rho_get_data_callback of [%d] is %d", 2,
            instance, s_multi_adv_data[instance].m_adv_addr_type);
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t multi_ble_adv_rho_update_callback(bt_role_handover_update_info_t *info)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    uint32_t instance;
    uint32_t data_shift = 0;
    uint8_t *data;
    if (info && BT_AWS_MCE_ROLE_PARTNER == info->role && info->data) {
        if (info->length != sizeof(bt_addr_type_t) * MULTI_ADV_INSTANCE_MAX_COUNT) {
            APPS_LOG_MSGID_I(LOG_TAG"multi_ble_adv_rho_update_callback, data_len not correct", 0);
        } else {
            data = (uint8_t *)info->data;
            for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
                memcpy(&s_multi_adv_data[instance].m_adv_addr_type, data + data_shift, sizeof(bt_addr_type_t));
                data_shift += sizeof(bt_addr_type_t);
                APPS_LOG_MSGID_I(LOG_TAG"multi_ble_adv_rho_update_callback of [%d] is %d", 2,
                    instance, s_multi_adv_data[instance].m_adv_addr_type);
            }
        }
    }
    return ret;
}

void multi_ble_adv_rho_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{

}
#endif

void multi_ble_adv_manager_init(void)
{
    multi_adv_instance_t instance;
    uint32_t nvdm_size = sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT;
    uint32_t count = 0;
    uint32_t conn_id;
    uint8_t temp_addr[sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT] = {
        0
    };

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    bt_gap_le_srv_get_link_attribute_register(multi_ble_adv_get_link_attribute_callback);
#endif
    /* Read BLE random address from NVDM. If read fail, generate it and store to NVDM. */
    if (NVDM_STATUS_OK != nvdm_read_data_item(NVDM_GROUP_MMI_APP, NVDM_MULTI_ADV_LE_ADDR,
                                              temp_addr, &nvdm_size)) {
        for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
            bt_app_common_generate_random_address(s_multi_adv_data[instance].m_ble_addr);
            APPS_LOG_MSGID_I(LOG_TAG"[%d], s_ble_addr [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", 7,
                             instance,
                             s_multi_adv_data[instance].m_ble_addr[0],
                             s_multi_adv_data[instance].m_ble_addr[1],
                             s_multi_adv_data[instance].m_ble_addr[2],
                             s_multi_adv_data[instance].m_ble_addr[3],
                             s_multi_adv_data[instance].m_ble_addr[4],
                             s_multi_adv_data[instance].m_ble_addr[5]);
        }
        multi_ble_adv_store_ble_addr();
    } else {
        count = (nvdm_size/sizeof(bt_bd_addr_t));
        count = count > MULTI_ADV_INSTANCE_MAX_COUNT ? MULTI_ADV_INSTANCE_MAX_COUNT : count;
        for (instance = 0; instance < count; instance++) {
            memcpy(s_multi_adv_data[instance].m_ble_addr,
                   temp_addr + sizeof(bt_bd_addr_t) * instance,
                   sizeof(bt_bd_addr_t));
            APPS_LOG_MSGID_I(LOG_TAG"[%d], s_ble_addr [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", 7,
                             instance,
                             s_multi_adv_data[instance].m_ble_addr[0],
                             s_multi_adv_data[instance].m_ble_addr[1],
                             s_multi_adv_data[instance].m_ble_addr[2],
                             s_multi_adv_data[instance].m_ble_addr[3],
                             s_multi_adv_data[instance].m_ble_addr[4],
                             s_multi_adv_data[instance].m_ble_addr[5]);
        }

        if (count < MULTI_ADV_INSTANCE_MAX_COUNT) {
            while (instance < MULTI_ADV_INSTANCE_MAX_COUNT) {
                bt_app_common_generate_random_address(s_multi_adv_data[instance].m_ble_addr);
                APPS_LOG_MSGID_I(LOG_TAG"[%d], s_ble_addr [%02X:%02X:%02X:%02X:%02X:%02X]\r\n", 7,
                                 instance,
                                 s_multi_adv_data[instance].m_ble_addr[0],
                                 s_multi_adv_data[instance].m_ble_addr[1],
                                 s_multi_adv_data[instance].m_ble_addr[2],
                                 s_multi_adv_data[instance].m_ble_addr[3],
                                 s_multi_adv_data[instance].m_ble_addr[4],
                                 s_multi_adv_data[instance].m_ble_addr[5]);
                instance++;
            }
            multi_ble_adv_store_ble_addr();
        }
    }
    for (instance = 0; instance < MULTI_ADV_INSTANCE_MAX_COUNT; instance++) {
        s_multi_adv_data[instance].m_adv_addr_type = BT_ADDR_RANDOM; /* Most feature use random. */
        if (s_multi_adv_data[instance].m_max_connection_count == 0) {
            /* Default support 1 connection for 1 instance. */
            s_multi_adv_data[instance].m_max_connection_count = 1;
        }
        for (conn_id = 0; conn_id < MULTI_ADV_MAX_CONN_COUNT; conn_id++) {
            s_multi_adv_data[instance].m_connection_handle[conn_id] = 0xFFFF;
        }
        s_multi_adv_data[instance].m_adv_instance = 0;
    }
    /* Initialize get_adv_cb for every instance */
    s_multi_adv_data[MULTI_ADV_INSTANCE_DEFAULT].m_get_adv_cb = multi_adv_manager_get_default_adv_data_cb;
#ifdef __BT_FAST_PAIR_ENABLE__
    s_multi_adv_data[MULTI_ADV_INSTANCE_FAST_PAIR].m_get_adv_cb = multi_adv_manager_get_fast_pair_adv_data_cb;
#endif
#ifdef MTK_VA_XIAOAI_ENABLE
    s_multi_adv_data[MULTI_ADV_INSTANCE_XIAOAI].m_get_adv_cb = multi_adv_manager_get_xiaoai_adv_data_cb;
#endif
#if defined(AIR_LE_AUDIO_ENABLE)
    s_multi_adv_data[MULTI_ADV_INSTANCE_NOT_RHO].m_get_adv_cb = multi_adv_manager_get_not_rho_adv_data_cb;
#if defined(MTK_AWS_MCE_ENABLE)
    s_multi_adv_data[MULTI_ADV_INSTANCE_NOT_RHO].m_conn_handle_not_rho = true;
#endif
#ifdef AIR_SWIFT_PAIR_ENABLE
    s_multi_adv_data[MULTI_ADV_INSTANCE_SWIFT_PAIR].m_get_adv_cb = multi_adv_manager_get_swift_pair_adv_data_cb;
#endif
#endif
#ifdef AIR_TILE_ENABLE
    s_multi_adv_data[MULTI_ADV_INSTANCE_TILE].m_get_adv_cb = multi_adv_manager_get_tile_adv_data_cb;
#ifdef MTK_AWS_MCE_ENABLE
    s_multi_adv_data[MULTI_ADV_INSTANCE_TILE].m_conn_handle_not_rho = true;
#endif
#endif
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    bt_role_handover_callbacks_t role_callbacks = {
        NULL,
        multi_ble_adv_rho_get_data_len_callback,
        multi_ble_adv_rho_get_data_callback,
        multi_ble_adv_rho_update_callback,
        multi_ble_adv_rho_status_callback,
    };
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_APP_MULTI_ADV, &role_callbacks);
#endif
}
