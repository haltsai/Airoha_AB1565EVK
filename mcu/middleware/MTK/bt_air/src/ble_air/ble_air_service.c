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

/*****************************************************************************
 *
 *
 * Description:
 * ------------
 * This file implements BLE Air service main function
 *
 ****************************************************************************/

#ifdef MTK_PORT_SERVICE_BT_ENABLE
#include <stdint.h>
#include "bt_gatts.h"
#include "bt_gattc.h"
#include "ble_air_interface.h"
#include "bt_gap_le.h"
#include "bt_uuid.h"
#include "bt_type.h"
#include "bt_callback_manager.h"
#include "ble_air_internal.h"
#ifdef MTK_GATT_OVER_BREDR_ENABLE
#include "gatt_over_bredr_air_internal.h"
#endif
#ifndef WIN32
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#endif
#include "syslog.h"

#ifdef MTK_BLE_GAP_SRV_ENABLE
#include "bt_gap_le_service.h"
#endif
#ifdef MTK_BT_CM_SUPPORT
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#endif
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#endif
#if 0
#ifdef BLE_AIR_LOW_POWER_CONTROL
#include "timers.h"
TimerHandle_t g_xTimer_low_power = NULL;
extern TimerHandle_t ble_air_create_timer(void);
#endif
#endif
log_create_module(AIR, PRINT_LEVEL_INFO);

#define BLE_AIR_SRV_MAX_MTU               512

#define ble_air_hex_dump(_message,...)   LOG_HEXDUMP_I(AIR, (_message), ##__VA_ARGS__)

#define AIR_SERVICE_UUID                \
    {{0x45, 0x4C, 0x42, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
     0x41, 0x03, 0xAB, 0x2D, 0x4D, 0x49, 0x52, 0x50}}


#define AIR_RX_CHAR_UUID                \
    {{0x45, 0x4C, 0x42, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
     0x41, 0x32, 0xAB, 0x2D, 0x52, 0x41, 0x48, 0x43}}



#define AIR_TX_CHAR_UUID                \
    {{0x45, 0x4C, 0x42, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
     0x41, 0x31, 0xAB, 0x2D, 0x52, 0x41, 0x48, 0x43}}


/************************************************
*   Global
*************************************************/

const bt_uuid_t AIR_RX_CHAR_UUID128 = {
    {
        0x45, 0x4C, 0x42, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
        0x41, 0x32, 0xAB, 0x2D, 0x52, 0x41, 0x48, 0x43
    }
};


const bt_uuid_t AIR_TX_CHAR_UUID128 = {
    {
        0x45, 0x4C, 0x42, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
        0x41, 0x31, 0xAB, 0x2D, 0x52, 0x41, 0x48, 0x43
    }
};


typedef struct {
    bool in_use;
    ble_air_common_callback_t callback;
} ble_air_callback_node_t;

static uint8_t g_rx_buffer[BLE_AIR_RECEIVE_BUFFER_SIZE];
static ble_air_cntx_t g_air_cntx[BT_CONNECTION_MAX];
static ble_air_callback_node_t ble_air_cb_list[BLE_AIR_SUPPORT_CB_MAX_NUM];

#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
static ble_air_cntx_t *g_le_air_highlight_context = NULL;
#endif

#define BLE_AIR_LINK_UPDATE_STATE_NONE                              (0x00)
#define BLE_AIR_LINK_UPDATE_STATE_OPTIMIZING                        (0x01)
#define BLE_AIR_LINK_UPDATE_STATE_OPTIMIZED                         (0x02)
#define BLE_AIR_LINK_UPDATE_STATE_OPTIMIZE_REVERT                   (0x03)
static uint8_t link_update_state = BLE_AIR_LINK_UPDATE_STATE_NONE;

/************************************************
*   static utilities
*************************************************/
static uint32_t ble_air_tx_char_cccd_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static uint32_t ble_air_rx_write_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset);
static int32_t ble_air_event_callback(ble_air_event_t event_id, void *param);
static bt_status_t ble_air_check_user(void);
static void ble_air_connection_status_notify(ble_air_common_callback_t callback);
static ble_air_status_t ble_air_cb_register(ble_air_common_callback_t callback);
static ble_air_status_t ble_air_cb_deregister(ble_air_common_callback_t callback);
static void ble_air_init_all_cntx(void);
#if defined(MTK_AWS_MCE_ENABLE) && defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
static void ble_air_reset_connection_cntx_with_rho(void);
static ble_air_cntx_t *ble_air_get_free_conn_cntx(void);
#endif
#ifdef MTK_BLE_GAP_SRV_ENABLE
bt_gap_le_srv_link_attribute_t bt_gap_le_srv_get_link_attribute_by_handle(bt_handle_t handle);
#endif
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce.h"
#include "bt_device_manager.h"
typedef struct {
    bt_aws_mce_role_t air_aws_mce_role;
} ble_air_aws_mce_role_context_t;
static ble_air_aws_mce_role_context_t role_cntx;
#define AIR_AWS_MCE_ROLE_VALUE_HANDLE                (0x0058)   /**< Attribute Vlaue Hanlde of AWS MCE role Characteristic. */
#define AIR_AWS_MCE_ROLE_CHAR_UUID                \
    {{0x45, 0x4C, 0x42, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
     0x41, 0x30, 0xAB, 0x2D, 0x52, 0x41, 0x48, 0x43}}


const bt_uuid_t AIR_AWS_MCE_ROLE_CHAR_UUID128 = {
    {
        0x45, 0x4C, 0x42, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
        0x41, 0x30, 0xAB, 0x2D, 0x52, 0x41, 0x48, 0x43
    }
};

static uint32_t ble_air_aws_mce_role_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(AIR, "[BLE][AIR] aws mce role value, rw is %d, size is %d, offset is %d \r\n", 3, rw, size, offset);
    if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == 0) {
            return sizeof(bt_aws_mce_role_t);
        }
        role_cntx.air_aws_mce_role = bt_device_manager_aws_local_info_get_role();
        LOG_MSGID_I(AIR, "[BLE][AIR] aws mce get role = %02x\r\n", 1, role_cntx.air_aws_mce_role);
        uint8_t *buffer = (uint8_t *)data;
        *buffer = (uint8_t)role_cntx.air_aws_mce_role;
        return sizeof(bt_aws_mce_role_t);
    }
    return 0;
}

static uint32_t ble_air_aws_mce_role_cccd_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(AIR, "[BLE][AIR] aws mce role cccd, rw is %d, size is %d, offset is %d \r\n", 3, rw, size, offset);
    ble_air_cntx_t *temp_cntx = ble_air_get_cntx_by_handle(handle);
    if (rw == BT_GATTS_CALLBACK_READ) {
        if (size == 0) {
            return sizeof(uint16_t);
        }
        uint16_t *buffer = (uint16_t *)data;
        *buffer = (uint16_t)temp_cntx->aws_role_cccd;
        return sizeof(uint16_t);
    } else if (rw == BT_GATTS_CALLBACK_WRITE) {
        temp_cntx->aws_role_cccd = *(uint16_t *)data;
        return sizeof(uint16_t);
    }
    return 0;
}

static bt_status_t ble_air_srv_notification(ble_air_cntx_t *context, uint16_t value_handle, uint8_t *data, uint16_t length)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint8_t *send_buffer = NULL;
    /* notify client. */
    if (context->aws_role_cccd == BLE_AIR_CCCD_NOTIFICATION) {
        send_buffer = (uint8_t *)pvPortMalloc(5 + length);
        if (send_buffer == NULL) {
            LOG_MSGID_I(AIR, "[BLE][AIR] notification alloc buffer fail", 0);
            return BT_STATUS_FAIL;
        }
        bt_gattc_charc_value_notification_indication_t *notification = (bt_gattc_charc_value_notification_indication_t *)send_buffer;
        notification->attribute_value_length = 3 + length;
        notification->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
        notification->att_req.handle = value_handle;
        memcpy(notification->att_req.attribute_value, data, length);
        status = bt_gatts_send_charc_value_notification_indication(context->conn_handle, notification);
        if (status != BT_STATUS_SUCCESS) {
            LOG_MSGID_I(AIR, "[BLE][AIR] notification fail status = %02x", 1, status);
        }
        vPortFree(send_buffer);
    } else {
        LOG_MSGID_I(AIR, "[BLE][AIR] notification cccd disable, cccd = %02x", 1, context->aws_role_cccd);
    }
    return status;
}

bt_status_t ble_air_set_aws_mce_role(bt_aws_mce_role_t role)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    uint32_t i = 0;
    if (role_cntx.air_aws_mce_role != role) {
        LOG_MSGID_I(AIR, "[BLE][AIR] role change, old role = %02x, new role = %02x", 2, role_cntx.air_aws_mce_role, role);
        role_cntx.air_aws_mce_role = role;
        for (i = 0; i < BT_CONNECTION_MAX; i++) {
            LOG_MSGID_I(AIR, "[BLE][AIR] send index:%d role notification", 1, i);
            status = ble_air_srv_notification(&g_air_cntx[i], AIR_AWS_MCE_ROLE_VALUE_HANDLE, &role_cntx.air_aws_mce_role, sizeof(bt_aws_mce_role_t));
        }
    } else {
        LOG_MSGID_I(AIR, "[BLE][AIR] role is not change, role = %02x", 1, role_cntx.air_aws_mce_role);
    }
    return status;
}

static bt_status_t ble_air_cm_event_callback(bt_cm_event_t event_id, void *params, uint32_t params_len)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    switch(event_id) {
#if 0
        case BT_AWS_MCE_SRV_EVENT_ROLE_CHANGED_IND:
        {
            if((params_len == 0) || (params == NULL)) {
                LOG_MSGID_I(AIR, "[BLE][AIR] cm event role change params error", 0);
                return BT_STATUS_FAIL;
            }
            bt_aws_mce_srv_switch_role_complete_ind_t *ind = (bt_aws_mce_srv_switch_role_complete_ind_t *)params;
            LOG_MSGID_I(AIR, "[BLE][AIR] cm event role change result = %02x, role = %02x", 2, ind->result, ind->cur_aws_role);
            ble_air_set_aws_mce_role(ind->cur_aws_role);
        }
        break;
#endif
        case BT_CM_EVENT_REMOTE_INFO_UPDATE:
        {
            bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)params;
            if (!(event->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && (event->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                LOG_MSGID_I(AIR, "[BLE][AIR] cm event aws connect reason = %02x, current role = %02x", 2, event->reason, role);
                ble_air_set_aws_mce_role(role);
            }
        }
        break;
        default:
            break;
    }
    return BT_STATUS_FAIL;
}
#endif


BT_GATTS_NEW_PRIMARY_SERVICE_128(ble_air_primary_service, AIR_SERVICE_UUID);

BT_GATTS_NEW_CHARC_128(ble_air_rx_char,
                       BT_GATT_CHARC_PROP_WRITE_WITHOUT_RSP | BT_GATT_CHARC_PROP_WRITE, AIR_RX_CHAR_VALUE_HANDLE, AIR_RX_CHAR_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_air_rx_char_value, AIR_RX_CHAR_UUID128,
                                  BT_GATTS_REC_PERM_WRITABLE,
                                  ble_air_rx_write_char_callback);

BT_GATTS_NEW_CHARC_128(ble_air_tx_char,
                       BT_GATT_CHARC_PROP_NOTIFY, AIR_TX_CHAR_VALUE_HANDLE, AIR_TX_CHAR_UUID);

BT_GATTS_NEW_CHARC_VALUE_UINT8(ble_air_tx_char_value, AIR_TX_CHAR_UUID128,
                               BT_GATTS_REC_PERM_READABLE, 0);

BT_GATTS_NEW_CLIENT_CHARC_CONFIG(ble_air_tx_client_config,
                                 BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 ble_air_tx_char_cccd_callback);

#ifdef MTK_AWS_MCE_ENABLE
BT_GATTS_NEW_CHARC_128(ble_air_aws_mce_role_char, BT_GATT_CHARC_PROP_READ | BT_GATT_CHARC_PROP_NOTIFY,
                       AIR_AWS_MCE_ROLE_VALUE_HANDLE, AIR_AWS_MCE_ROLE_CHAR_UUID);

BT_GATTS_NEW_CHARC_VALUE_CALLBACK(ble_air_aws_mce_role_value, AIR_AWS_MCE_ROLE_CHAR_UUID128, BT_GATTS_REC_PERM_READABLE,
                                  ble_air_aws_mce_role_char_callback);

BT_GATTS_NEW_CLIENT_CHARC_CONFIG(ble_air_aws_mce_role_client_config, BT_GATTS_REC_PERM_READABLE | BT_GATTS_REC_PERM_WRITABLE,
                                 ble_air_aws_mce_role_cccd_callback);

#endif

static const bt_gatts_service_rec_t *ble_air_service_rec[] = {
    (const bt_gatts_service_rec_t *) &ble_air_primary_service,
    (const bt_gatts_service_rec_t *) &ble_air_rx_char,
    (const bt_gatts_service_rec_t *) &ble_air_rx_char_value,
    (const bt_gatts_service_rec_t *) &ble_air_tx_char,
    (const bt_gatts_service_rec_t *) &ble_air_tx_char_value,
    (const bt_gatts_service_rec_t *) &ble_air_tx_client_config,
#ifdef MTK_AWS_MCE_ENABLE
    (const bt_gatts_service_rec_t *) &ble_air_aws_mce_role_char,
    (const bt_gatts_service_rec_t *) &ble_air_aws_mce_role_value,
    (const bt_gatts_service_rec_t *) &ble_air_aws_mce_role_client_config,
#endif
};

const bt_gatts_service_t ble_air_service = {
    .starting_handle = 0x0051,
#ifdef MTK_AWS_MCE_ENABLE
    .ending_handle = 0x0059,
#else
    .ending_handle = 0x0056,
#endif
    .required_encryption_key_size = 0,
    .records = ble_air_service_rec
};

//Mutex
extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);
#define BLEAIR_MUTEX_LOCK() bt_os_take_stack_mutex()
#define BLEAIR_MUTEX_UNLOCK() bt_os_give_stack_mutex()


#if defined(MTK_AWS_MCE_ENABLE) && defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
#include "bt_role_handover.h"
BT_PACKED (
typedef struct {
    bool                            is_real_connected; //After ACL link connected and first write request had come
    bool                            need_ready2write;
    uint16_t                        notify_enabled;
    uint16_t                        conn_handle;
    bt_bd_addr_t                    peer_addr; /**< Address information of the remote device. */
}) ble_air_rho_cntx_t;

BT_PACKED (
typedef struct {
    uint8_t connected_dev_num;
}) ble_air_rho_header_t;

ble_air_rho_header_t air_rho_header = {0};

static bt_status_t ble_air_rho_allowed_cb(const bt_bd_addr_t *addr)
{
    //App need call ble_air_get_rx_available() to check if any data in RX queue, if have, need get all data, then to allow RHO
    return BT_STATUS_SUCCESS;
}

static uint8_t ble_air_rho_get_data_length_cb(const bt_bd_addr_t *addr)
{
    uint8_t i, counter = 0;
#ifdef AIR_MULTI_POINT_ENABLE
    if (addr != NULL) {
        LOG_MSGID_I(AIR, "[AIR][LE][RHO] get data length addr = NULL for EMP", 0);
        return 0;
    }
#endif
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if ((BT_HANDLE_INVALID != g_air_cntx[i].conn_handle)
#ifdef MTK_BLE_GAP_SRV_ENABLE
                && (BT_GAP_LE_SRV_LINK_ATTRIBUTE_NOT_NEED_RHO != bt_gap_le_srv_get_link_attribute_by_handle(g_air_cntx[i].conn_handle))
#endif
           ) {
            counter++;
        }
    }
    LOG_MSGID_I(AIR, "[AIR][LE][RHO] get length, rho connected num = %d", 1, counter);
    air_rho_header.connected_dev_num = counter;
    if (air_rho_header.connected_dev_num) {
        return (sizeof(ble_air_rho_header_t) + (sizeof(ble_air_callback_node_t) * BLE_AIR_SUPPORT_CB_MAX_NUM)
                + (sizeof(ble_air_rho_cntx_t) * air_rho_header.connected_dev_num));
    }
    return 0;
}

static bt_status_t ble_air_rho_get_data_cb(const bt_bd_addr_t *addr, void *data)
{
    configASSERT((NULL != data));
    uint8_t i, j;
#ifdef AIR_MULTI_POINT_ENABLE
    if (addr != NULL) {
        LOG_MSGID_I(AIR, "[BLE][AIR][RHO] get data addr = NULL for EMP", 0);
        return BT_STATUS_FAIL;
    }
#endif
    if (air_rho_header.connected_dev_num) {
        uint8_t index = 0;
        ble_air_rho_header_t *rho_head = (ble_air_rho_header_t *)data;
        rho_head->connected_dev_num = air_rho_header.connected_dev_num;
        ble_air_callback_node_t *rho_cb = (ble_air_callback_node_t *)(rho_head + 1);
        for (i = 0; i < BLE_AIR_SUPPORT_CB_MAX_NUM; i++) {
            ((ble_air_callback_node_t *)(rho_cb + i))->in_use = ble_air_cb_list[i].in_use;
            ((ble_air_callback_node_t *)(rho_cb + i))->callback = ble_air_cb_list[i].callback;
        }
        ble_air_rho_cntx_t *rho_cntx = (ble_air_rho_cntx_t *)(rho_cb + BLE_AIR_SUPPORT_CB_MAX_NUM);
        for (j = 0; ((j < BT_CONNECTION_MAX) && (index < air_rho_header.connected_dev_num)); j++) {
            if (BT_HANDLE_INVALID != g_air_cntx[j].conn_handle
#ifdef MTK_BLE_GAP_SRV_ENABLE
                    && (bt_gap_le_srv_get_link_attribute_by_handle(g_air_cntx[j].conn_handle) != BT_GAP_LE_SRV_LINK_ATTRIBUTE_NOT_NEED_RHO)
#endif
               ) {
                LOG_MSGID_I(AIR, "[BLE][AIR][RHO] get data, link index = %d", 1, j);
                ((ble_air_rho_cntx_t *)(rho_cntx + index))->conn_handle = g_air_cntx[j].conn_handle;
                ((ble_air_rho_cntx_t *)(rho_cntx + index))->is_real_connected = g_air_cntx[j].is_real_connected;
                ((ble_air_rho_cntx_t *)(rho_cntx + index))->need_ready2write = g_air_cntx[j].need_ready2write;
                ((ble_air_rho_cntx_t *)(rho_cntx + index))->notify_enabled = g_air_cntx[j].notify_enabled;
                memcpy(((ble_air_rho_cntx_t *)(rho_cntx + index))->peer_addr, g_air_cntx[j].peer_addr, sizeof(bt_bd_addr_t));
                index++;
            }
        }
    }
    return BT_STATUS_SUCCESS;
}

static void ble_air_rho_status_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    switch (event) {
    case BT_ROLE_HANDOVER_COMPLETE_IND: {
        if (BT_STATUS_SUCCESS == status) {
            if (BT_AWS_MCE_ROLE_AGENT == role) {
                ble_air_reset_connection_cntx_with_rho();
                air_rho_header.connected_dev_num = 0;
                ble_air_set_aws_mce_role(BT_AWS_MCE_ROLE_PARTNER);
            } else if (BT_AWS_MCE_ROLE_PARTNER == role) {
                ble_air_set_aws_mce_role(BT_AWS_MCE_ROLE_AGENT);
            }
        }
    }
    break;
    default:
        break;
    }
}

static bt_status_t ble_air_rho_update_cb(bt_role_handover_update_info_t *info)
{
    uint8_t i, j;
    if (info && (BT_AWS_MCE_ROLE_PARTNER == info->role)) {
        if ((info->length > 0) && (info->data)) {//copy data to context
            ble_air_rho_header_t *rho_head = (ble_air_rho_header_t *)info->data;
            ble_air_callback_node_t *rho_cb = (ble_air_callback_node_t *)(rho_head + 1);
            for (i = 0; i < BLE_AIR_SUPPORT_CB_MAX_NUM; i++) {
                ble_air_cb_list[i].in_use = ((ble_air_callback_node_t *)(rho_cb + i))->in_use;
                ble_air_cb_list[i].callback = ((ble_air_callback_node_t *)(rho_cb + i))->callback;
            }
            ble_air_rho_cntx_t *rho_cntx = (ble_air_rho_cntx_t *)(rho_cb + BLE_AIR_SUPPORT_CB_MAX_NUM);
            for (j = 0; j < rho_head->connected_dev_num; j++) {
                ble_air_cntx_t *temp_cntx = ble_air_get_free_conn_cntx();
                if (temp_cntx == NULL) {
                    break;
                }
                temp_cntx->conn_handle = ((ble_air_rho_cntx_t *)(rho_cntx + j))->conn_handle;
                temp_cntx->is_real_connected = ((ble_air_rho_cntx_t *)(rho_cntx + j))->is_real_connected;
                temp_cntx->need_ready2write = ((ble_air_rho_cntx_t *)(rho_cntx + j))->need_ready2write;
                temp_cntx->notify_enabled = ((ble_air_rho_cntx_t *)(rho_cntx + j))->notify_enabled;
                memcpy(temp_cntx->peer_addr, ((ble_air_rho_cntx_t *)(rho_cntx + j))->peer_addr, sizeof(bt_bd_addr_t));
#ifdef MTK_BLE_GAP_SRV_ENABLE
                /* Get new handle after RHO */
                bt_handle_t new_handle = bt_gap_le_srv_get_handle_by_old_handle(temp_cntx->conn_handle);
                LOG_MSGID_I(AIR, "[AIR][LE][RHO] rho update index = %d,old_handle = %02x,new_handle = %02x!\r\n", 3, j, temp_cntx->conn_handle, new_handle);
                temp_cntx->conn_handle = new_handle;
#endif
                if (temp_cntx->is_real_connected) {
                    memset(&g_rx_buffer, 0x0, BLE_AIR_RECEIVE_BUFFER_SIZE);
                    temp_cntx->receive_buffer = g_rx_buffer;
                }
            }
        } else {
            //error log
            return BT_STATUS_FAIL;
        }
    }
    return BT_STATUS_SUCCESS;
}

bt_role_handover_callbacks_t ble_air_rho_callbacks = {
    .allowed_cb = ble_air_rho_allowed_cb,/*optional if always allowed*/
    .get_len_cb = ble_air_rho_get_data_length_cb,  /*optional if no RHO data to partner*/
    .get_data_cb = ble_air_rho_get_data_cb,   /*optional if no RHO data to partner*/
    .update_cb = ble_air_rho_update_cb,       /*optional if no RHO data to partner*/
    .status_cb = ble_air_rho_status_cb, /*Mandatory for all users.*/
};

#endif /*__MTK_AWS_MCE_ENABLE__ */



/********************Connection Info**************************/

static bool ble_air_has_real_connected(void)
{
    uint8_t i = 0;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if (g_air_cntx[i].is_real_connected) {
            LOG_MSGID_I(AIR, "[AIR][LE] air has connected index = %d, conenction handle = %02x", 2, i, g_air_cntx[i].conn_handle);
            return true;
        }
    }
    if (i == BT_CONNECTION_MAX) {
        LOG_MSGID_I(AIR, "ble_air_has_real_connected,no real connected link!\r\n", 0);
    }
    return false;
}

uint16_t ble_air_get_real_connected_handle(void)
{
    uint8_t i = 0;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if ((BT_HANDLE_INVALID != g_air_cntx[i].conn_handle) &&
                (g_air_cntx[i].is_real_connected)) {
            return g_air_cntx[i].conn_handle;
        }
    }
    if (i == BT_CONNECTION_MAX) {
        LOG_MSGID_I(AIR, "ble_air_get_cntx_by_handle,not connected!\r\n", 0);
    }
    return BT_HANDLE_INVALID;
}

ble_air_cntx_t *ble_air_get_cntx_by_handle(uint16_t conn_handle)
{
    uint8_t i = 0;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if ((conn_handle != BT_HANDLE_INVALID) && (conn_handle == g_air_cntx[i].conn_handle)) {
            return &(g_air_cntx[i]);
        }
    }
    if (i == BT_CONNECTION_MAX) {
        LOG_MSGID_I(AIR, "ble_air_get_cntx_by_handle,not connected!\r\n", 0);
    }
    return NULL;
}

#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
static bt_status_t ble_air_srv_notify_user(bt_handle_t connection_handle, bt_bd_addr_t *address)
{
    ble_air_connect_t connect_param;
    memset(&connect_param, 0x0, sizeof(ble_air_connect_t));
    connect_param.conn_handle = connection_handle;
    memcpy(&(connect_param.bdaddr), address, BT_BD_ADDR_LEN);
    ble_air_event_callback(BLE_AIR_EVENT_CONNECT_IND, (void *)&connect_param);
    return BT_STATUS_SUCCESS;
}
#endif

#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
static void ble_air_srv_update_highlight_context(ble_air_cntx_t *context)
{
    LOG_MSGID_I(AIR, "[AIR][LE] update highlight context, old highlight:%02x, new_highlight:%02x\r\n", 2, g_le_air_highlight_context, context);
    g_le_air_highlight_context = context;
}

static ble_air_cntx_t *ble_air_srv_get_highlight_context(void)
{
    LOG_MSGID_I(AIR, "[AIR][LE] get highlight context:%02x\r\n", 1, g_le_air_highlight_context);
    return g_le_air_highlight_context;
}

bt_status_t ble_air_srv_switch_link(bt_handle_t connection_handle)
{
    LOG_MSGID_I(AIR, "[AIR][LE] switch link connection handle %02x\r\n", 1, connection_handle);
    ble_air_cntx_t *temp_cntx = ble_air_get_cntx_by_handle(connection_handle);
    if (temp_cntx == NULL) {
        LOG_MSGID_I(AIR, "[AIR][LE] switch link fail, connection handle is invaild = %02x\r\n", 1, connection_handle);
        return BT_STATUS_FAIL;
    }
    temp_cntx->receive_buffer = g_rx_buffer;
    temp_cntx->receive_buffer_length = 0;
    ble_air_srv_update_highlight_context(temp_cntx);
    return ble_air_srv_notify_user(temp_cntx->conn_handle, &temp_cntx->peer_addr);
}

bt_status_t ble_air_link_performace_optimization_internel(bt_handle_t connection_handle)
{
    /* Set LE max data length. */
    bt_hci_cmd_le_set_data_length_t data_length_cmd;
    data_length_cmd.connection_handle = connection_handle;
    data_length_cmd.tx_octets = 0xFB;
    data_length_cmd.tx_time = 0x0848;
    bt_gap_le_update_data_length(&data_length_cmd);
    /*Set LE PHY 2M. */
    bt_hci_le_set_phy_t phy;
    phy.tx = BT_HCI_LE_PHY_MASK_2M;
    phy.rx = BT_HCI_LE_PHY_MASK_2M;
    bt_gap_le_set_phy(connection_handle, &phy);

    return BT_STATUS_SUCCESS;
}
#endif

bt_status_t ble_air_link_performace_optimization_revert(void)
{
    uint32_t i = 0;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if((BT_HANDLE_INVALID != g_air_cntx[i].conn_handle) && (0 != g_air_cntx[i].conn_handle) && 
           (g_air_cntx[i].is_link_optimization) && (g_air_cntx[i].is_real_connected)) {
            g_air_cntx[i].is_link_optimization = false;
            link_update_state = BLE_AIR_LINK_UPDATE_STATE_OPTIMIZE_REVERT;
            /* Update conenction interval. */
            bt_hci_cmd_le_connection_update_t new_param;
            new_param.connection_handle = g_air_cntx[i].conn_handle;
            new_param.conn_interval_min = g_air_cntx[i].revert_interval;
            new_param.conn_interval_max = g_air_cntx[i].revert_interval;
            new_param.conn_latency = 0x00;
            new_param.supervision_timeout =  g_air_cntx[i].revert_supervision_timeout / 10;
            new_param.minimum_ce_length = 0x00;
            new_param.maximum_ce_length = 0x00;
            bt_status_t update_status = bt_gap_le_update_connection_parameter((const bt_hci_cmd_le_connection_update_t *)(&new_param));
            LOG_MSGID_I(AIR, "[BLE][AIR] revert LE connection update interval parameter index = %d, handle = %02x, interval = %02x, latency = %02x,\
                                        supervision_timeout = %02x, ce_length = %02x , status = %02x", 7, i, new_param.connection_handle, new_param.conn_interval_min,\
                                        new_param.conn_latency, new_param.supervision_timeout, new_param.minimum_ce_length, update_status);
            return BT_STATUS_SUCCESS;
        }
    }
    link_update_state = BLE_AIR_LINK_UPDATE_STATE_NONE;
    LOG_MSGID_I(AIR, "[BLE][AIR] all link optimization revert complete\r\n", 0);
    return BT_STATUS_FAIL;
}

static bt_status_t ble_air_link_performace_optimization_by_handle(bt_handle_t connection_handle)
{
    /* Update conenction interval. */
    bt_hci_cmd_le_connection_update_t new_param;
    new_param.connection_handle = connection_handle ;
    new_param.conn_interval_min = 0x0018;
    new_param.conn_interval_max = 0x0018;
    new_param.conn_latency = 0x00;
    new_param.supervision_timeout = 0x1F4;
    new_param.minimum_ce_length = 0x06;
    new_param.maximum_ce_length = 0x06;
    bt_status_t update_status = bt_gap_le_update_connection_parameter((const bt_hci_cmd_le_connection_update_t *)(&new_param));
    LOG_MSGID_I(AIR, "[BLE][AIR] set LE connection update interval parameter by handle = %02x, interval = %02x, latency = %02x,\
                                supervision_timeout = %02x, ce_length = %02x , status = %02x", 6, new_param.connection_handle, new_param.conn_interval_min,\
                                new_param.conn_latency, new_param.supervision_timeout, new_param.minimum_ce_length, update_status);
    return BT_STATUS_SUCCESS;
}

bt_status_t ble_air_link_performace_optimization(void)
{
    uint32_t i = 0;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if((BT_HANDLE_INVALID != g_air_cntx[i].conn_handle) && (0 != g_air_cntx[i].conn_handle) &&
           (!g_air_cntx[i].is_link_optimization) && (g_air_cntx[i].is_real_connected)) {
            link_update_state = BLE_AIR_LINK_UPDATE_STATE_OPTIMIZING;
            g_air_cntx[i].is_link_optimization = true;
            /* Update conenction interval. */
            bt_hci_cmd_le_connection_update_t new_param;
            new_param.connection_handle = g_air_cntx[i].conn_handle;
            new_param.conn_interval_min = 0x0018;
            new_param.conn_interval_max = 0x0018;
            new_param.conn_latency = 0x00;
            new_param.supervision_timeout = 0x1F4;
            new_param.minimum_ce_length = 0x06;
            new_param.maximum_ce_length = 0x06;
            bt_status_t update_status = bt_gap_le_update_connection_parameter((const bt_hci_cmd_le_connection_update_t *)(&new_param));
            LOG_MSGID_I(AIR, "[BLE][AIR] set LE connection update interval parameter index = %d, handle = %02x, interval = %02x, latency = %02x,\
                                        supervision_timeout = %02x, ce_length = %02x , status = %02x", 7, i, new_param.connection_handle, new_param.conn_interval_min,\
                                        new_param.conn_latency, new_param.supervision_timeout, new_param.minimum_ce_length, update_status);
            return BT_STATUS_SUCCESS;
        }
    }
    link_update_state = BLE_AIR_LINK_UPDATE_STATE_OPTIMIZED;
    LOG_MSGID_I(AIR, "[BLE][AIR] all link optimization complete\r\n", 0);
    return BT_STATUS_FAIL;
}

static bt_status_t ble_air_save_connection_info(void *buff)
{
    uint8_t i;
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_gap_le_connection_ind_t *conn_ind = (bt_gap_le_connection_ind_t *)buff;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        /**< first connect, to save connection info. */
        if (BT_HANDLE_INVALID == g_air_cntx[i].conn_handle) {
            g_air_cntx[i].conn_handle = conn_ind->connection_handle;
            memcpy(g_air_cntx[i].peer_addr, conn_ind->peer_addr.addr, sizeof(conn_ind->peer_addr.addr));
#ifdef BLE_AIR_LOW_POWER_CONTROL
            g_air_cntx[i].low_power_t.conn_interval = conn_ind->conn_interval;
#endif
#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
            /* BLE air service client, notify user. */
            g_air_cntx[i].receive_buffer = g_rx_buffer;
            g_air_cntx[i].receive_buffer_length = 0;
            LOG_MSGID_I(AIR, "[AIR][LE] connection context:%02x, connection_handle = %02x", 2, &g_air_cntx[i], g_air_cntx[i].conn_handle);
            if(ble_air_srv_get_highlight_context() == NULL) {
                ble_air_srv_notify_user(g_air_cntx[i].conn_handle, &g_air_cntx[i].peer_addr);
            }
            ble_air_link_performace_optimization_internel(conn_ind->connection_handle);
#endif
            g_air_cntx[i].revert_interval = conn_ind->conn_interval;
            g_air_cntx[i].revert_supervision_timeout = conn_ind->supervision_timeout;
            bt_gatts_set_max_mtu(BLE_AIR_SRV_MAX_MTU);
            break;
            /**< Reconnect. */
        } else if (conn_ind->connection_handle == g_air_cntx[i].conn_handle) {
            LOG_MSGID_I(AIR, "reconnect, connection handle=0x%04x", 1, g_air_cntx[i].conn_handle);
            break;
        }
    }
    if (i == BT_CONNECTION_MAX) {
        LOG_MSGID_I(AIR, "Reach maximum connection, no empty buffer!\r\n", 0);
        status = BT_STATUS_OUT_OF_MEMORY; 
    }
    return status;
}

static bt_status_t ble_air_delete_connection_info(void *buff)
{
    uint8_t i;
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_hci_evt_disconnect_complete_t *disconn_ind;
    LOG_MSGID_I(AIR, "ble_air_delete_connection_info", 0);
    disconn_ind = (bt_hci_evt_disconnect_complete_t *) buff;
    for (i = 0; i < BT_CONNECTION_MAX ; i++) {
        if (disconn_ind->connection_handle == g_air_cntx[i].conn_handle) {
            LOG_MSGID_I(AIR, "[AIR][LE] disconnection context:%02x, connection_handle = %02x", 2, &g_air_cntx[i], g_air_cntx[i].conn_handle);
            memset(&(g_air_cntx[i]), 0, sizeof(ble_air_cntx_t));
            memset(g_air_cntx[i].peer_addr, 0, sizeof(g_air_cntx[i].peer_addr));
            g_air_cntx[i].conn_handle = BT_HANDLE_INVALID;
            g_air_cntx[i].receive_buffer = NULL;
#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
            if(ble_air_srv_get_highlight_context() == &g_air_cntx[i]) {
                ble_air_srv_update_highlight_context(NULL);
            }
#endif
            break;
        }
    }
    if (i == BT_CONNECTION_MAX) {
        LOG_MSGID_I(AIR, "Can not find the connection info to delete!\r\n", 0);
        status = BT_STATUS_FAIL;
    }
    return status;
}

static void ble_air_reset_connection_cntx(void)
{
    uint8_t i;
    LOG_MSGID_I(AIR, "ble_air_reset_connection_cntx", 0);
    for (i = 0; i < BT_CONNECTION_MAX ; i++) {
        memset(&(g_air_cntx[i]), 0, sizeof(ble_air_cntx_t));
        memset(g_air_cntx[i].peer_addr, 0, sizeof(g_air_cntx[i].peer_addr));
        g_air_cntx[i].conn_handle = BT_HANDLE_INVALID;
        g_air_cntx[i].receive_buffer = NULL;
    }
#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
    ble_air_srv_update_highlight_context(NULL);
#endif
}

/**********************************************/

/**
 * @brief Function for handling Client Configuration Characteristisc Descriptor's read and write event.
 *
 * @param[in]   rw                    Flag of Read or Write event.
 * @param[in]   handle                Connection handle.
 * @param[in]   size                  Length of the data.
 * @param[in]   *data                 Data buffer.
 * @param[in]   *data                 Data buffer.
 * @param[in]  offset                 Write or Read offset.
 *
 * @return      Real wrote or read length of the data.
 */
static uint32_t ble_air_tx_char_cccd_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(AIR, "ble_air_tx_char_cccd_callback, handle = %02x, rw is %d, size is %d, offset is %d \r\n", 4, handle, rw, size, offset);
    ble_air_cntx_t *temp_cntx = ble_air_get_cntx_by_handle(handle);
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    gatt_over_bredr_air_cntx_t *gobe_cntx = gatt_over_bredr_air_get_cntx_by_handle(handle);
    if ((handle != BT_HANDLE_INVALID) && (gobe_cntx)) {
        return gatt_over_bredr_air_tx_char_cccd_callback(rw, handle, data, size, offset);
    } else
#endif
        //if ((handle != BT_HANDLE_INVALID) && (handle == g_air_cntx.conn_handle)) {
        if ((handle != BT_HANDLE_INVALID) && (temp_cntx)) {
            /** record for each connection. */
            if (rw == BT_GATTS_CALLBACK_WRITE) {
                if (size != sizeof(uint16_t)) {
                    LOG_MSGID_I(AIR, "[AIR][LE] enable air cccd size error = %02x", 1, size);
                    return 0;
                }
                //g_air_cntx.notify_enabled = *(uint16_t *)data;
                if ((false == ble_air_has_real_connected()) || ((0 == *(uint16_t *)data) && (true == temp_cntx->is_real_connected))) {
                    temp_cntx->notify_enabled = *(uint16_t *)data;
                } else {
                    LOG_MSGID_I(AIR, "[AIR][LE] enable air cccd fail, is real connected = %d", 1, ble_air_has_real_connected());
                    return 0;
                }
                if ((0 == ble_air_check_user()) && (BLE_AIR_CCCD_NOTIFICATION == temp_cntx->notify_enabled)) {
                    ble_air_ready_to_write_t ready_to_write;
                    if (false == temp_cntx->is_real_connected) {
                        ble_air_connect_t connect_param;
                        temp_cntx->is_real_connected = true;
                        memset(&g_rx_buffer, 0x0, BLE_AIR_RECEIVE_BUFFER_SIZE);
                        temp_cntx->receive_buffer = g_rx_buffer;
                        memset(&connect_param, 0x0, sizeof(ble_air_connect_t));
                        connect_param.conn_handle = temp_cntx->conn_handle;
                        memcpy(&(connect_param.bdaddr), &(temp_cntx->peer_addr), BT_BD_ADDR_LEN);
                        LOG_MSGID_I(AIR, "BLE_AIR_EVENT_CONNECT_IND\r\n", 0);
                        ble_air_event_callback(BLE_AIR_EVENT_CONNECT_IND, (void *)&connect_param);
                    }
                    memset(&ready_to_write, 0x0, sizeof(ble_air_ready_to_write_t));
                    ready_to_write.conn_handle = handle;
                    memcpy(&(ready_to_write.bdaddr), &(temp_cntx->peer_addr), BT_BD_ADDR_LEN);
                    ble_air_event_callback(BLE_AIR_EVENT_READY_TO_WRITE_IND, (void *)&ready_to_write);
                } else if ((0 == ble_air_check_user()) && (0 == temp_cntx->notify_enabled))  {
                    if (true == temp_cntx->is_real_connected) {
                        ble_air_disconnect_t disconnect_param;
                        temp_cntx->is_real_connected = false;
                        temp_cntx->receive_buffer = NULL;
                        memset(&g_rx_buffer, 0x0, BLE_AIR_RECEIVE_BUFFER_SIZE);
                        memset(&disconnect_param, 0x0, sizeof(ble_air_disconnect_t));
                        disconnect_param.conn_handle = temp_cntx->conn_handle;
                        memcpy(&(disconnect_param.bdaddr), &(temp_cntx->peer_addr), BT_BD_ADDR_LEN);
                        LOG_MSGID_I(AIR, "disconnec handle=0x%04x, because CCCD Disable\r\n", 1, handle);
                        ble_air_event_callback(BLE_AIR_EVENT_DISCONNECT_IND, (void *)&disconnect_param);
                    }
                }
                LOG_MSGID_I(AIR, "ble_air_tx_char_cccd_callback, data:%d \r\n", 1,  temp_cntx->notify_enabled);
            } else if (rw == BT_GATTS_CALLBACK_READ) {
                if (size != 0) {
                    uint16_t *buf = (uint16_t *) data;
                    *buf = (uint16_t) temp_cntx->notify_enabled;
                    LOG_MSGID_I(AIR, "read cccd value = %d\r\n", 1, *buf);
                }
            }
            return sizeof(uint16_t);
        }
    return 0;
}


/**
 * @brief Function for handling Air Rx Characteristisc's read and write event.
 *
 * @param[in]   rw                    Flag of Read or Write event.
 * @param[in]   handle                Connection handle.
 * @param[in]   size                  Length of the data.
 * @param[in]   *data                 Data buffer.
 * @param[in]   offset                Write or Read offset.
 *
 * @return      Real wrote or read length of the data.
 */
static uint32_t ble_air_rx_write_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(AIR, "ble_air_rx_write_char_callback:rw is %d, size is %d, offset is %d \r\n", 3, rw, size, offset);
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(handle);
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    gatt_over_bredr_air_cntx_t *gobe_cntx = gatt_over_bredr_air_get_cntx_by_handle(handle);
    //TODO:Should record for each connection handle.
    if ((handle != BT_HANDLE_INVALID) && (gobe_cntx) && (gobe_cntx->is_real_connected)) {
        return gatt_over_bredr_air_rx_write_char_callback(rw, handle, data, size, offset);
    } else
#endif
        //TODO:Should record for each connection handle.
        if ((handle != BT_HANDLE_INVALID) && (buffer_t) && (buffer_t->is_real_connected)) {
            if (rw == BT_GATTS_CALLBACK_WRITE) {
                /**remote write & notify app ready to read*/
                LOG_MSGID_I(AIR, "write length= %d\r\n", 1, size);
                if (size > (BLE_AIR_RECEIVE_BUFFER_SIZE - buffer_t->receive_buffer_length)) {
                    LOG_MSGID_I(AIR, "write characteristic: buffer full error!\r\n", 0);
                    return 0; /**means fail, buffer full*/
                }
#ifdef BLE_AIR_LOW_POWER_CONTROL
                ble_air_set_remote_device_type(handle, BLE_AIR_REMOTE_DEVICE_IOS);
                ble_air_update_connection_interval(handle);
#endif
                memcpy((uint8_t *)(buffer_t->receive_buffer + buffer_t->receive_buffer_length), data, size);
                buffer_t->receive_buffer_length += size;
                if ((0 == ble_air_check_user())) {
                    ble_air_ready_to_read_t ready_to_read;
                    memset(&ready_to_read, 0, sizeof(ble_air_ready_to_read_t));
                    ready_to_read.conn_handle = handle;
                    memcpy(&(ready_to_read.bdaddr), &(buffer_t->peer_addr), BT_BD_ADDR_LEN);
                    LOG_MSGID_I(AIR, "write characteristic: write size = %d \r\n", 1, size);
                    ble_air_event_callback(BLE_AIR_EVENT_READY_TO_READ_IND, (void *)&ready_to_read);
                }
                return size;
            }
        } else {
            LOG_MSGID_I(AIR, "ble_air write characteristic handle = %02x!\r\n", 1, handle);
            if (buffer_t != NULL) {
                LOG_MSGID_I(AIR, "ble_air write characteristic is_real_connected = %d!\r\n", 1, buffer_t->is_real_connected);
            }
        }
    return 0;
}

/**
 *  @brief Function for handling the Application's BLE Stack events.
 *
 *  @param[in] msg    Stack event type.
 *  @param[in] *buff  Stack event parameters.
 */
static bt_status_t ble_air_common_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    //LOG_MSGID_I(AIR, "ble_air_on_ble_evt msg = 0x%04x \r\n", 1, msg);
    switch (msg) {
    case BT_GAP_LE_CONNECT_IND: {
        ble_air_save_connection_info(buff);
    }
    break;
    case BT_GAP_LE_DISCONNECT_IND: {
        bt_hci_evt_disconnect_complete_t *connection_ind = (bt_hci_evt_disconnect_complete_t *)buff;
        uint16_t conn_handle = connection_ind->connection_handle;
        ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);
        if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t)) {
            if (0 == ble_air_check_user()
#ifndef AIR_BLE_AUDIO_DONGLE_ENABLE
            && (buffer_t->is_real_connected)
#endif
        ) {
                ble_air_disconnect_t disconnect_param;
                memset(&disconnect_param, 0, sizeof(ble_air_disconnect_t));
                disconnect_param.conn_handle = conn_handle;
                memcpy(&disconnect_param.bdaddr, &(buffer_t->peer_addr), BT_BD_ADDR_LEN);
                LOG_MSGID_I(AIR, "[AIR][LE] notify user disconnect handle=0x%04x", 1, conn_handle);
                ble_air_event_callback(BLE_AIR_EVENT_DISCONNECT_IND, (void *)&disconnect_param);
            }
            ble_air_delete_connection_info(buff);
        }
    }
    break;
    //case BT_GAP_LE_CONNECTION_UPDATE_CNF:
    case BT_GAP_LE_CONNECTION_UPDATE_IND: {
        bt_gap_le_connection_update_ind_t *ind = (bt_gap_le_connection_update_ind_t *)buff;
        uint16_t conn_handle = ind->conn_handle;
        ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);
        if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t)) {
            LOG_MSGID_I(AIR, "CONNECTION UPDATE: event_id = %x, interval = %d\n", 2, msg, ind->conn_interval);
#ifdef BLE_AIR_LOW_POWER_CONTROL
            buffer_t->low_power_t.conn_interval = ind->conn_interval;
            buffer_t->low_power_t.conn_priority = ble_air_get_current_connection_interval(conn_handle, ind->conn_interval);
#endif
            if(link_update_state == BLE_AIR_LINK_UPDATE_STATE_OPTIMIZING) {
                ble_air_link_performace_optimization();
            } else if(link_update_state == BLE_AIR_LINK_UPDATE_STATE_OPTIMIZE_REVERT) {
                ble_air_link_performace_optimization_revert();
            } else {
                if ((buffer_t->is_link_optimization) && (ind->conn_interval != 0x0018)) {
                    ble_air_link_performace_optimization_by_handle(conn_handle);
                }
                buffer_t->revert_interval = ind->conn_interval;
                buffer_t->revert_supervision_timeout = ind->supervision_timeout;
            }
        }
    }
    break;
    case BT_POWER_OFF_CNF: {
        uint16_t conn_handle = ble_air_get_real_connected_handle();
        ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);
        if ((BT_HANDLE_INVALID != conn_handle) && (buffer_t)) {
            ble_air_disconnect_t disconnect_param;
            memset(&disconnect_param, 0, sizeof(ble_air_disconnect_t));
            disconnect_param.conn_handle = conn_handle;
            memcpy(&(disconnect_param.bdaddr), &(buffer_t->peer_addr), BT_BD_ADDR_LEN);
            LOG_MSGID_I(AIR, "BT power off, then disconn handle=0x%04x\r\n", 1, conn_handle);
            ble_air_event_callback(BLE_AIR_EVENT_DISCONNECT_IND, (void *)&disconnect_param);
        }
        ble_air_reset_connection_cntx();
    }
    break;
#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
    case BT_GATTC_CHARC_VALUE_NOTIFICATION:
    {
        bt_gatt_handle_value_notification_t *notification = (bt_gatt_handle_value_notification_t *)buff;
        if(notification->att_rsp->handle == AIR_TX_CHAR_VALUE_HANDLE) {
            uint32_t length = notification->length - 3;
            bt_handle_t connection_handle = notification->connection_handle;
            ble_air_cntx_t *context = ble_air_get_cntx_by_handle(connection_handle);
            if(context == NULL) {
                LOG_MSGID_I(AIR, "[BLE][AIR] receive notification context is NULL\r\n", 0);
                break;
            }

            if(context->receive_buffer == NULL) {
                LOG_MSGID_I(AIR, "[BLE][AIR] receive buffer is NULL\r\n", 0);
                break;
            }

            if (length > (BLE_AIR_RECEIVE_BUFFER_SIZE - context->receive_buffer_length)) {
                LOG_MSGID_I(AIR, "[AIR][LE] receive notification over length, buffer_length = %02x\r\n", 1, context->receive_buffer_length);
                break;
            }
            memcpy((uint8_t *)(context->receive_buffer + context->receive_buffer_length), notification->att_rsp->attribute_value, length);
            context->receive_buffer_length += length;
            if ((0 == ble_air_check_user())) {
                ble_air_ready_to_read_t ready_to_read;
                memset(&ready_to_read, 0, sizeof(ble_air_ready_to_read_t));
                ready_to_read.conn_handle = connection_handle;
                memcpy(&(ready_to_read.bdaddr), &(context->peer_addr), BT_BD_ADDR_LEN);
                ble_air_event_callback(BLE_AIR_EVENT_READY_TO_READ_IND, (void *)&ready_to_read);
            }
        }
    }
    break;
#endif
    default:
        break;
    }
    return BT_STATUS_SUCCESS;
}


static bt_status_t ble_air_event_callback_init(void)
{
    bt_status_t result = bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SYSTEM | MODULE_MASK_GAP | MODULE_MASK_GATT, (void *)ble_air_common_event_handler);
    if (result != BT_STATUS_SUCCESS) {
        LOG_MSGID_I(AIR, "ble_air_event_callback_init fail! \r\n", 0);
        return BT_STATUS_FAIL;
    }
    return result;
}


/************************************************
*   Functions
*************************************************/
/**
 * @brief Function for sending the Air service tx characteristic value.
 *
 * @param[in]   conn_handle                           connection handle.
 *
 * @return      ble_status_t                              0 means success.
 */
static bt_status_t ble_air_service_tx_send(bt_handle_t conn_handle, uint8_t *data, uint32_t length)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint8_t *buf = NULL;
    bt_gattc_charc_value_notification_indication_t *air_noti_rsp;
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);

    buf = (uint8_t *)pvPortMalloc(5 + length);
    if (buf == NULL) {
        LOG_MSGID_I(AIR, "ble_air_service_tx_send fail, OOM!\r\n", 0);
        return status;
    }
    air_noti_rsp = (bt_gattc_charc_value_notification_indication_t *) buf;
    LOG_MSGID_I(AIR, "ble_air_service_tx_send, new RX Char Value is %d\r\n", 1, data[0]);
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t) && (buffer_t->is_real_connected) &&
            (BLE_AIR_CCCD_NOTIFICATION ==  buffer_t->notify_enabled)) {
        air_noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
        air_noti_rsp->att_req.handle = AIR_TX_CHAR_VALUE_HANDLE;
        memcpy((void *)(air_noti_rsp->att_req.attribute_value), data, length);
        air_noti_rsp->attribute_value_length = 3 + length;
        LOG_MSGID_I(AIR, "ble_air_service_notify conn_handle is %x, send data is %d\r\n", 2, conn_handle, data[0]);
        status = bt_gatts_send_charc_value_notification_indication(conn_handle, air_noti_rsp);
    }
    if (buf != NULL) {
        vPortFree(buf);
    }
    return status;
}

/**
 * @brief Function for application to write data to the send buffer.
 */
uint32_t ble_air_write_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size)
{
    BLEAIR_MUTEX_LOCK();
    uint32_t send_size = 0;
    uint32_t mtu_size;
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);

    if (buffer_t == NULL) {
        LOG_MSGID_I(AIR, "[BLE][AIR] client send data get conn cntx fail\r\n", 0);
        BLEAIR_MUTEX_UNLOCK();
        return 0;
    }

    mtu_size = bt_gattc_get_mtu((bt_handle_t)conn_handle);
    LOG_MSGID_I(AIR, "mtu = %d\r\n", 1, mtu_size);
    if ((mtu_size - 3) < size) {
        send_size = mtu_size - 3;
    } else {
        send_size = size;
    }
    if (0 == send_size) {
        LOG_MSGID_I(AIR, "[BLE_AIR] ble_air_send_data send_size is 0!\r\n", 0);
        BLEAIR_MUTEX_UNLOCK();
        return 0;
    }
#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
    bt_status_t status = BT_STATUS_FAIL;
    uint8_t *buf = NULL;
    if (buffer == NULL) {
        LOG_MSGID_I(AIR, "[BLE][AIR] client send data buffer is NULL\r\n", 0);
        BLEAIR_MUTEX_UNLOCK();
        return 0;
    }
    /* As client, use write send data to server. */
    buf = (uint8_t *)pvPortMalloc(3 + send_size);
    if (buf == NULL) {
        LOG_MSGID_I(AIR, "[BLE][AIR] client send data alloc fail\r\n", 0);
        BLEAIR_MUTEX_UNLOCK();
        return 0;
    }
    LOG_MSGID_I(AIR, "[BLE][AIR] client send conenction handle = %02x\r\n", 1, conn_handle);
    static bt_gattc_write_without_rsp_req_t write_req;
    write_req.attribute_value_length = send_size;
    write_req.att_req = (bt_att_write_command_t *)buf;
    write_req.att_req->opcode = BT_ATT_OPCODE_WRITE_COMMAND;
    write_req.att_req->attribute_handle = AIR_RX_CHAR_VALUE_HANDLE;
    memcpy(write_req.att_req->attribute_value, buffer, send_size);
    status = bt_gattc_write_without_rsp(buffer_t->conn_handle, 0, &write_req);
    vPortFree(buf);
    if(status != BT_STATUS_SUCCESS) {
        LOG_MSGID_I(AIR, "[BLE][AIR] client send fail status = %02x\r\n", 1, status);
        BLEAIR_MUTEX_UNLOCK();
        return 0;
    }
    BLEAIR_MUTEX_UNLOCK();
    return send_size;
#else
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t) &&
            (buffer_t->is_real_connected)) {

#ifdef BLE_AIR_LOW_POWER_CONTROL
        ble_air_set_remote_device_type(conn_handle, BLE_AIR_REMOTE_DEVICE_IOS);
        ble_air_update_connection_interval(conn_handle);
#endif
        if (BT_STATUS_SUCCESS == ble_air_service_tx_send(conn_handle, buffer, send_size)) {
            LOG_MSGID_I(AIR, "[BLE_AIR] ble_air_send_data: send_size[%d]\r\n", 1, send_size);
            BLEAIR_MUTEX_UNLOCK();
            return send_size;
        }
    }
#endif
    BLEAIR_MUTEX_UNLOCK();
    return 0;
}

uint32_t ble_air_get_rx_available(uint16_t conn_handle)
{
    BLEAIR_MUTEX_LOCK();
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t) && (buffer_t->is_real_connected)) {
        BLEAIR_MUTEX_UNLOCK();
        return buffer_t->receive_buffer_length;
    }
    BLEAIR_MUTEX_UNLOCK();
    return 0;
}

/**
 * @brief Function for application to read data from the receive buffer.
 */
uint32_t ble_air_read_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size)
{
    BLEAIR_MUTEX_LOCK();
    uint32_t read_size = 0;
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t)
#ifndef AIR_BLE_AUDIO_DONGLE_ENABLE
    && (buffer_t->is_real_connected)
#endif
    ) {
        if (buffer_t->receive_buffer_length > size) {
            read_size = size;
        } else {
            read_size = buffer_t->receive_buffer_length;
        }
        if (0 == read_size) {
            LOG_MSGID_I(AIR, "[BLE_AIR] ble_air_read_data: read buffer is null\r\n", 0);
            BLEAIR_MUTEX_UNLOCK();
            return 0;
        }
        memcpy(buffer, buffer_t->receive_buffer, read_size);
        if (buffer_t->receive_buffer_length > read_size) {
            memmove(buffer_t->receive_buffer, (uint8_t *)(buffer_t->receive_buffer + read_size), (buffer_t->receive_buffer_length - read_size));
            buffer_t->receive_buffer_length -= read_size;
        } else {
            buffer_t->receive_buffer_length = 0;
            memset(&g_rx_buffer, 0x0, BLE_AIR_RECEIVE_BUFFER_SIZE);
        }
        LOG_MSGID_I(AIR, "[BLE_AIR] ble_air_read_data: read_size is [%d]\r\n", 1, read_size);
        BLEAIR_MUTEX_UNLOCK();
        return read_size;
    }
    LOG_MSGID_I(AIR, "[BLE_AIR] ble_air_read_data: conn id error [%d]\r\n", 1, conn_handle);
    BLEAIR_MUTEX_UNLOCK();
    return 0;
}
#if defined(MTK_AWS_MCE_ENABLE) && defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
static ble_air_cntx_t *ble_air_get_free_conn_cntx(void)
{
    uint8_t i = 0;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if ((g_air_cntx[i].conn_handle == 0x0000) || (g_air_cntx[i].conn_handle == BT_HANDLE_INVALID)) {
            LOG_MSGID_I(AIR, "[BLE][AIR] get free conn cntx index = %d", 1, i);
            return &g_air_cntx[i];
        }
    }
    LOG_MSGID_I(AIR, "[BLE][AIR] not have free conn cntx", 0);
    return NULL;
}

static void ble_air_reset_connection_cntx_with_rho(void)
{
    uint8_t i = 0;
    for (i = 0; i < BT_CONNECTION_MAX ; i++) {
#ifdef MTK_BLE_GAP_SRV_ENABLE
        if (bt_gap_le_srv_get_link_attribute_by_handle(g_air_cntx[i].conn_handle) != BT_GAP_LE_SRV_LINK_ATTRIBUTE_NOT_NEED_RHO) {
#endif
            memset(&(g_air_cntx[i]), 0, sizeof(ble_air_cntx_t));
            memset(g_air_cntx[i].peer_addr, 0, sizeof(g_air_cntx[i].peer_addr));
            g_air_cntx[i].conn_handle = BT_HANDLE_INVALID;
            g_air_cntx[i].receive_buffer = NULL;
#ifdef MTK_BLE_GAP_SRV_ENABLE
        }
#endif

    }
}
#endif

static void ble_air_init_all_cntx(void)
{
    ble_air_reset_connection_cntx();
    memset(&g_rx_buffer, 0x0, BLE_AIR_RECEIVE_BUFFER_SIZE);
}


void ble_air_main(void)
{
#if 0
#ifdef BLE_AIR_LOW_POWER_CONTROL
    g_xTimer_low_power = ble_air_create_timer();
#endif
#endif
    ble_air_init_all_cntx();
#if defined(MTK_AWS_MCE_ENABLE) && defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_BLE_AIR, &ble_air_rho_callbacks);
#endif
    ble_air_event_callback_init();
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    gatt_over_bredr_air_event_callback_init();
#endif
#if defined(MTK_BT_CM_SUPPORT) && defined(MTK_AWS_MCE_ENABLE)
    bt_cm_register_event_callback(ble_air_cm_event_callback);
#endif
}

/**
 * @brief Function for application main entry.
 */
ble_air_status_t ble_air_init(ble_air_common_callback_t app_callback)
{
    if (NULL == app_callback) {
        return BLE_AIR_STATUS_INVALID_PARAMETER;
    } else {
        /**Initialize.*/
        LOG_MSGID_I(AIR, "init app_callback=0x%04x", 1, app_callback);
        return ble_air_cb_register(app_callback);
    }
    return BLE_AIR_STATUS_FAIL;
}

ble_air_status_t ble_air_deinit(ble_air_common_callback_t app_callback)
{
    if (NULL == app_callback) {
        return BLE_AIR_STATUS_INVALID_PARAMETER;
    } else {
        /**Initialize.*/
        LOG_MSGID_I(AIR, "deinit app_callback=0x%04x", 1, app_callback);
        return ble_air_cb_deregister(app_callback);
    }
    return BLE_AIR_STATUS_FAIL;
}

static int32_t ble_air_event_callback(ble_air_event_t event_id, void *param)
{
    uint8_t i = 0;
    int32_t ret = 0;
    for (i = 0; i < BLE_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (ble_air_cb_list[i].in_use && ble_air_cb_list[i].callback != NULL) {
            ble_air_cb_list[i].callback(event_id, param);
            ret = 0;
        }
    }
    return ret;
}

static bt_status_t ble_air_check_user(void)
{
    uint8_t i = 0;
    bt_status_t status = 0;
    LOG_MSGID_I(AIR, "ble_air_check_user \r\n", 0);
    for (i = 0; i < BLE_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (ble_air_cb_list[i].in_use) {
            return status;
        }
    }
    if (i == BLE_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(AIR, "not find any users existed!\r\n", 0);
        status = BLE_AIR_STATUS_FAIL;
    }
    return status;
}

static void ble_air_connection_status_notify(ble_air_common_callback_t callback)
{
    uint16_t conn_handle = ble_air_get_real_connected_handle();
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);
    if ((BT_HANDLE_INVALID != conn_handle) && (buffer_t)) {
        ble_air_connect_t connect_param;
        memset(&connect_param, 0x0, sizeof(ble_air_connect_t));
        connect_param.conn_handle = conn_handle;
        memcpy(&(connect_param.bdaddr), &(buffer_t->peer_addr), BT_BD_ADDR_LEN);
        callback(BLE_AIR_EVENT_CONNECT_IND, (void *)&connect_param);
    }
}

static ble_air_status_t ble_air_cb_register(ble_air_common_callback_t callback)
{
    uint8_t i = 0;
    ble_air_status_t status = 0;
    LOG_MSGID_I(AIR, "ble_air_cb_register callback %x\r\n", 1, callback);
    for (i = 0; i < BLE_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (!ble_air_cb_list[i].in_use) {
            ble_air_cb_list[i].callback = callback;
            ble_air_cb_list[i].in_use = true;
            ble_air_connection_status_notify(callback);
            break;
        }
    }
    if (i == BLE_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(AIR, "all are in use, please extend the value of BLE_AIR_SUPPORT_CB_MAX_NUM\r\n", 0);
        status = BLE_AIR_STATUS_FAIL;
    }
    return status;
}

static ble_air_status_t ble_air_cb_deregister(ble_air_common_callback_t callback)
{
    uint8_t i = 0;
    ble_air_status_t status = 0;
    LOG_MSGID_I(SPPAIR, "ble_air_cb_deregister callback %x\r\n", 1, callback);
    for (i = 0; i < BLE_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (ble_air_cb_list[i].in_use && ble_air_cb_list[i].callback == callback) {
            ble_air_cb_list[i].callback = NULL;
            ble_air_cb_list[i].in_use = false;
            break;
        }
    }
    if (i == BLE_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(AIR, "ble_air_cb_deregister delete fail, because of not find the callback\r\n", 0);
        status = BLE_AIR_STATUS_FAIL;
    }
    return status;
}

#endif /*#ifdef MTK_PORT_SERVICE_BT_ENABLE*/
