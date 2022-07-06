/* Copyright Statement:
 *
 * (C) 2020 Airoha Technology Corp. All rights reserved.
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
 * This file implements GATT_OVER_BREDR Air service main function
 *
 ****************************************************************************/

#ifdef MTK_PORT_SERVICE_BT_ENABLE
#ifdef MTK_GATT_OVER_BREDR_ENABLE
#include <stdint.h>
#include "bt_gatts.h"
#include "bt_gattc.h"
#include "bt_gatt_over_bredr.h"
#include "gatt_over_bredr_air.h"
#include "gatt_over_bredr_air_internal.h"
//#include "bt_gap_le.h"
//#include "bt_uuid.h"
#include "bt_type.h"
#include "bt_callback_manager.h"
#include "ble_air_internal.h"
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
#include "bt_role_handover.h"
#endif

#ifndef WIN32
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#endif
#include "syslog.h"

#ifdef MTK_BT_CM_SUPPORT
#include "bt_connection_manager.h"
#endif

log_create_module(GOBEAIR, PRINT_LEVEL_INFO);

extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);

#define GOBEAIR_MUTEX_LOCK() bt_os_take_stack_mutex()
#define GOBEAIR_MUTEX_UNLOCK() bt_os_give_stack_mutex()

static uint8_t g_receive_buffer[GATT_OVER_BREDR_AIR_RECEIVE_BUFFER_SIZE];
static gatt_over_bredr_air_cntx_t g_gobe_air_cntx[BT_CONNECTION_MAX];
static gatt_over_bredr_air_callback_node_t gatt_over_bredr_air_cb_list[GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM];

/************************************************
*   static utilities
*************************************************/
static void gatt_over_bredr_air_event_callback(gatt_over_bredr_air_event_t event_id, void *param);
static gatt_over_bredr_air_status_t gatt_over_bredr_air_check_user(void);
static void gatt_over_bredr_air_connection_status_notify(gatt_over_bredr_air_common_callback_t callback);
static gatt_over_bredr_air_status_t gatt_over_bredr_air_cb_register(gatt_over_bredr_air_common_callback_t callback);
static gatt_over_bredr_air_status_t gatt_over_bredr_air_cb_deregister(gatt_over_bredr_air_common_callback_t callback);
static uint16_t gatt_over_bredr_air_get_real_connected_handle(void);
static void gatt_over_bredr_air_clear_all_connection_info(void);


/********************Connection Info**************************/
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE) && defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
BT_PACKED (
typedef struct {
    bool     is_real_connected; //After ACL link connected and first write request had come
    uint16_t notify_enabled;
    uint16_t conn_handle;
    uint16_t max_packet_size;
})gatt_over_bredr_air_rho_context_t;

static bt_status_t gatt_over_bredr_air_rho_allowed_cb(const bt_bd_addr_t *addr)
{
    return BT_STATUS_SUCCESS;
}
static uint8_t gatt_over_bredr_air_rho_get_data_length_cb(const bt_bd_addr_t *addr)
{
    uint8_t i = 0;
    uint8_t rho_size = 0;
    if(addr == NULL) {
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho get data length addr is NULL\r\n", 0);
        return rho_size;
    }
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho get data length index = %d\r\n", 1, i);
        if ((0 != g_gobe_air_cntx[i].conn_handle) && (g_gobe_air_cntx[i].conn_handle != BT_HANDLE_INVALID) &&
                (0 == memcmp((g_gobe_air_cntx[i].peer_addr), addr, BT_BD_ADDR_LEN))) {
            rho_size += sizeof(gatt_over_bredr_air_rho_context_t);
        }
    }
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho get data length = %d\r\n", 1, rho_size);
    return rho_size;
}
static bt_status_t gatt_over_bredr_air_rho_get_data_cb(const bt_bd_addr_t *addr, void *data)
{
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho get data", 0);
    uint8_t i = 0;
    uint8_t index = 0;
    if (data == NULL) {
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho get data == NULL", 0);
        return BT_STATUS_FAIL;
    }
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if ((0 != g_gobe_air_cntx[i].conn_handle) && (g_gobe_air_cntx[i].conn_handle != BT_HANDLE_INVALID) &&
                (0 == memcmp((g_gobe_air_cntx[i].peer_addr), addr, BT_BD_ADDR_LEN)))  {
            gatt_over_bredr_air_rho_context_t *context = (gatt_over_bredr_air_rho_context_t *)(data + index * sizeof(gatt_over_bredr_air_rho_context_t));
            LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho get data index = %d", 1, index);
            context->is_real_connected = g_gobe_air_cntx[i].is_real_connected;
            context->conn_handle = g_gobe_air_cntx[i].conn_handle;
            context->notify_enabled = g_gobe_air_cntx[i].notify_enabled;
            context->max_packet_size = g_gobe_air_cntx[i].max_packet_size;
            index++;
        }
    }
    return BT_STATUS_SUCCESS;
}
static bt_status_t gatt_over_bredr_air_rho_update_cb(bt_role_handover_update_info_t *info)
{
    if (info && info->length && (BT_AWS_MCE_ROLE_PARTNER == info->role)) {
        uint8_t context_num = 0;
        uint8_t i = 0;
        if (info == NULL) {
            LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho update context info == NULL", 0);
            return BT_STATUS_FAIL;
        }
        context_num = info->length / sizeof(gatt_over_bredr_air_rho_context_t);
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho update context length = %d,num = %d", 2, info->length, context_num);
        if (context_num > 0) {
            //update context
            for (i = 0; i < context_num; i++) {
                gatt_over_bredr_air_rho_context_t *context = (gatt_over_bredr_air_rho_context_t *)(info->data + i * sizeof(gatt_over_bredr_air_rho_context_t));
                g_gobe_air_cntx[i].is_real_connected = context->is_real_connected;
                g_gobe_air_cntx[i].notify_enabled = context->notify_enabled;
                g_gobe_air_cntx[i].conn_handle = (uint16_t)info->profile_info->gatt_handle;
                g_gobe_air_cntx[i].max_packet_size = context->max_packet_size;
                LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air rho update new hci handle = %02x", 1, (uint16_t)info->profile_info->gatt_handle);
                /* notify user new connection handle. */
                gatt_over_bredr_air_connect_t connect_param;
                memset(&connect_param, 0x0, sizeof(gatt_over_bredr_air_connect_t));
                connect_param.conn_handle = g_gobe_air_cntx[i].conn_handle;
                memcpy(&(connect_param.bdaddr), &(g_gobe_air_cntx[i].peer_addr), BT_BD_ADDR_LEN);
                connect_param.max_packet_length = g_gobe_air_cntx[i].max_packet_size;
                gatt_over_bredr_air_event_callback(GATT_OVER_BREDR_AIR_EVENT_CONNECT_IND, (void *)&connect_param);
                if (g_gobe_air_cntx[i].is_real_connected) {
                    memset(&g_receive_buffer, 0x0, GATT_OVER_BREDR_AIR_RECEIVE_BUFFER_SIZE);
                    g_gobe_air_cntx[i].receive_buffer = g_receive_buffer;
                }
            }
        }
    }
    return BT_STATUS_SUCCESS;
}
static void gatt_over_bredr_air_rho_status_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    switch (event) {
        case BT_ROLE_HANDOVER_COMPLETE_IND: {
            if ((BT_AWS_MCE_ROLE_AGENT == role) && (BT_STATUS_SUCCESS == status)) {
                gatt_over_bredr_air_clear_all_connection_info();
            }
        }
        break;
        default:
            break;
    }
}

bt_role_handover_callbacks_t gatt_over_bredr_air_rho_callbacks = {
    .allowed_cb = gatt_over_bredr_air_rho_allowed_cb,             /*optional if always allowed*/
    .get_len_cb = gatt_over_bredr_air_rho_get_data_length_cb,     /*optional if no RHO data to partner*/
    .get_data_cb = gatt_over_bredr_air_rho_get_data_cb,           /*optional if no RHO data to partner*/
    .update_cb = gatt_over_bredr_air_rho_update_cb,               /*optional if no RHO data to partner*/
    .status_cb = gatt_over_bredr_air_rho_status_cb,               /*Mandatory for all users.*/
};
#elif defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE) && !defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
#include "bt_role_handover.h"
static bt_status_t gatt_over_bredr_air_rho_allowed_cb(const bt_bd_addr_t *addr)
{
    uint16_t conn_handle = gatt_over_bredr_air_get_real_connected_handle();
    if (BT_HANDLE_INVALID != conn_handle) {
        gatt_over_bredr_air_cntx_t *buff_t = gatt_over_bredr_air_get_cntx_by_handle(conn_handle);
        if ((buff_t) && (0 == memcmp((buff_t->peer_addr), addr, BT_BD_ADDR_LEN))) {//addr is the real connected device
            return BT_STATUS_PENDING;
        }
    }
    return BT_STATUS_SUCCESS;
}

static void gatt_over_bredr_air_rho_status_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    return ;
}

bt_role_handover_callbacks_t gatt_over_bredr_air_rho_callbacks = {
    .allowed_cb = gatt_over_bredr_air_rho_allowed_cb,/*optional if always allowed*/
    .get_len_cb = NULL,  /*optional if no RHO data to partner*/
    .get_data_cb = NULL,   /*optional if no RHO data to partner*/
    .update_cb = NULL,       /*optional if no RHO data to partner*/
    .status_cb = gatt_over_bredr_air_rho_status_cb, /*Mandatory for all users.*/
};
#endif /*__MTK_AWS_MCE_ENABLE__ */

/* service UUID*/
#define GOBE_SDP_SERVICE_UUID         0x50, 0x52, 0x49, 0x4D, 0X2D, 0xAB, 0x03, 0x41, 0x69, 0x72, 0x6F, 0x68, 0x61, 0x42, 0x4C, 0x45

/* GATT SDP record for Andriod discovery service. */
static const uint8_t bt_gobe_service_class_list[] = {
    BT_GATT_SDP_ATTRIBUTE_UUID_LENGTH_128,                               /* Service UUID type is 128bit. */
    BT_GATT_SDP_ATTRIBUTE_UUID_128(GOBE_SDP_SERVICE_UUID)
};

static uint8_t bt_gobe_service_desc_list[] = {
    BT_GATT_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(0x0051, 0x0056)            /* Service start handle and end handle*/
};

static const uint8_t bt_gobe_service_browse_group[] = {
    BT_GATT_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
};

static const bt_sdps_attribute_t bt_gobe_attributes[] = {
    BT_GATT_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(bt_gobe_service_class_list),
    BT_GATT_SDP_ATTRIBUTE_ID_PROTOCOL_DESC_LIST(bt_gobe_service_desc_list),
    BT_GATT_SDP_ATTRIBUTE_ID_BROWSE_GROUP_LIST(bt_gobe_service_browse_group),
};

static const bt_sdps_record_t bt_gobe_record = {
    .attribute_list_length = sizeof(bt_gobe_attributes),
    .attribute_list = bt_gobe_attributes,
};

static gatt_over_bredr_air_status_t gatt_over_bredr_air_check_user(void)
{
    uint8_t i;
    bt_status_t status = 0;
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_check_user \r\n", 0);
    for (i = 0; i < GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (gatt_over_bredr_air_cb_list[i].in_use) {
            return status;
        }
    }
    if (i == GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(GOBEAIR, "not find any users existed!\r\n", 0);
        status = GATT_OVER_BREDR_AIR_STATUS_FAIL;
    }
    return status;
}

static uint16_t gatt_over_bredr_air_get_real_connected_handle(void)
{
    uint8_t i;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if ((BT_HANDLE_INVALID != g_gobe_air_cntx[i].conn_handle) &&
                (g_gobe_air_cntx[i].is_real_connected)) {
            return g_gobe_air_cntx[i].conn_handle;
        }
    }
    if (i == BT_CONNECTION_MAX) {
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_get_real_connected_handle,no connection!\r\n", 0);
    }
    return BT_HANDLE_INVALID;
}

gatt_over_bredr_air_cntx_t *gatt_over_bredr_air_get_cntx_by_handle(uint16_t conn_handle)
{
    uint8_t i;
    for (i = 0; i < BT_CONNECTION_MAX; i++) {
        if ((conn_handle != BT_HANDLE_INVALID) && (conn_handle == g_gobe_air_cntx[i].conn_handle)) {
            return &(g_gobe_air_cntx[i]);
        }
    }
    if (i == BT_CONNECTION_MAX) {
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_get_cntx_by_handle,not connected!\r\n", 0);
    }
    return NULL;
}

static bt_status_t gatt_over_bredr_air_save_connection_info(void *buff)
{
    uint8_t i;
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_gatt_over_bredr_connect_cnf_t *conn_ind = (bt_gatt_over_bredr_connect_cnf_t *)buff;
    if (gatt_over_bredr_air_get_cntx_by_handle(conn_ind->connection_handle)) {/**< Reconnect. */
        return status;
    } else {
        for (i = 0; i < BT_CONNECTION_MAX; i++) {
            /**< first connect, to save connection info. */
            if (BT_HANDLE_INVALID == g_gobe_air_cntx[i].conn_handle) {
                g_gobe_air_cntx[i].conn_handle = conn_ind->connection_handle;
                memcpy(g_gobe_air_cntx[i].peer_addr, conn_ind->address, BT_BD_ADDR_LEN);
                g_gobe_air_cntx[i].max_packet_size = conn_ind->remote_rx_mtu;
                LOG_MSGID_I(GOBEAIR, "connection handle=0x%04x,remote_rx_mtu = %d", 2, g_gobe_air_cntx[i].conn_handle, g_gobe_air_cntx[i].max_packet_size);
                break;
            }
        }
        if (i == BT_CONNECTION_MAX) {
            LOG_MSGID_I(GOBEAIR, "Reach maximum connection, no empty buffer!\r\n", 0);
            status = BT_STATUS_OUT_OF_MEMORY;
        }
        return status;
    }
}

static bt_status_t gatt_over_bredr_air_delete_connection_info(void *buff)
{
    uint8_t i;
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_gatt_over_bredr_disconnect_ind_t *disconn_ind;
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_delete_connection_info", 0);
    disconn_ind = (bt_gatt_over_bredr_disconnect_ind_t *) buff;
    for (i = 0; i < BT_CONNECTION_MAX ; i++) {
        if (disconn_ind->connection_handle == g_gobe_air_cntx[i].conn_handle) {
            memset(&(g_gobe_air_cntx[i]), 0, sizeof(gatt_over_bredr_air_cntx_t));
            memset(g_gobe_air_cntx[i].peer_addr, 0, sizeof(g_gobe_air_cntx[i].peer_addr));
            g_gobe_air_cntx[i].conn_handle = BT_HANDLE_INVALID;
            g_gobe_air_cntx[i].receive_buffer = NULL;
            break;
        }
    }
    if (i == BT_CONNECTION_MAX) {
        LOG_MSGID_I(GOBEAIR, "Can not find the connection info to delete!\r\n", 0);
        status = BT_STATUS_FAIL;
    }
    return status;
}

static void gatt_over_bredr_air_clear_all_connection_info(void)
{
    uint8_t i;
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_clear_all_connection_info", 0);
    for (i = 0; i < BT_CONNECTION_MAX ; i++) {
        memset(&(g_gobe_air_cntx[i]), 0, sizeof(gatt_over_bredr_air_cntx_t));
        memset(g_gobe_air_cntx[i].peer_addr, 0, sizeof(g_gobe_air_cntx[i].peer_addr));
        g_gobe_air_cntx[i].conn_handle = BT_HANDLE_INVALID;
        g_gobe_air_cntx[i].receive_buffer = NULL;
    }
}

static void gatt_over_bredr_air_event_callback(gatt_over_bredr_air_event_t event_id, void *param)
{
    uint8_t i;
    for (i = 0; i < GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (gatt_over_bredr_air_cb_list[i].in_use && (NULL != gatt_over_bredr_air_cb_list[i].callback)) {
            gatt_over_bredr_air_cb_list[i].callback(event_id, param);
        }
    }
}


/************************************************
*   Functions
*************************************************/
/**
 * @brief Function for sending the Air service tx characteristic value.
 *
 * @param[in]   conn_handle                           connection handle.
 *
 * @return      gatt_over_bredr_status_t                              0 means success.
 */
static bt_status_t gatt_over_bredr_air_service_tx_send(bt_handle_t conn_handle, uint8_t *data, uint32_t length)
{
    bt_status_t status = BT_STATUS_FAIL;
    uint8_t *buf = NULL;
    bt_gattc_charc_value_notification_indication_t *air_noti_rsp;
    gatt_over_bredr_air_cntx_t *buffer_t = gatt_over_bredr_air_get_cntx_by_handle(conn_handle);
    buf = (uint8_t *)pvPortMalloc(5 + length);// 3 is the length of opcode and handle
    if (buf == NULL) {
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_service_tx_send fail, OOM!\r\n", 0);
        return status;
    }
    air_noti_rsp = (bt_gattc_charc_value_notification_indication_t *) buf;
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t) && (buffer_t->is_real_connected) &&
            (BLE_AIR_CCCD_NOTIFICATION ==  buffer_t->notify_enabled)) {
        air_noti_rsp->att_req.opcode = BT_ATT_OPCODE_HANDLE_VALUE_NOTIFICATION;
        air_noti_rsp->att_req.handle = AIR_TX_CHAR_VALUE_HANDLE;
        memcpy((void *)(air_noti_rsp->att_req.attribute_value), data, length);
        air_noti_rsp->attribute_value_length = 3 + length;
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_service_notify conn_handle is %x, send data is %d\r\n", 2, conn_handle, data[0]);
        status = bt_gatts_send_charc_value_notification_indication(conn_handle, air_noti_rsp);
    }
    if (buf) {
        vPortFree(buf);
    }
    return status;
}

static void gatt_over_bredr_air_connection_status_notify(gatt_over_bredr_air_common_callback_t callback)
{
    uint16_t conn_handle = gatt_over_bredr_air_get_real_connected_handle();
    gatt_over_bredr_air_cntx_t *buffer_t = gatt_over_bredr_air_get_cntx_by_handle(conn_handle);
    if ((BT_HANDLE_INVALID != conn_handle) && (buffer_t)) {
        gatt_over_bredr_air_connect_t connect_param;
        memset(&connect_param, 0x0, sizeof(gatt_over_bredr_air_connect_t));
        connect_param.conn_handle = conn_handle;
        memcpy(&(connect_param.bdaddr), &(buffer_t->peer_addr), BT_BD_ADDR_LEN);
        connect_param.max_packet_length = buffer_t->max_packet_size;
        callback(GATT_OVER_BREDR_AIR_EVENT_CONNECT_IND, (void *)&connect_param);
    }
}

/**
 *  @brief Function for handling the Application's GATT_OVER_BREDR Stack events.
 *
 *  @param[in] msg    Stack event type.
 *  @param[in] *buff  Stack event parameters.
 */
static bt_status_t gatt_over_bredr_air_common_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    bt_status_t sta = BT_STATUS_SUCCESS;
    switch (msg) {
        case BT_GATT_OVER_BREDR_CONNECT_CNF: {/* transport connection is established. */
            bt_gatt_over_bredr_connect_cnf_t *conn_cnf_p = (bt_gatt_over_bredr_connect_cnf_t *)buff;
            gatt_over_bredr_air_save_connection_info(buff);
#ifdef MTK_BT_CM_SUPPORT
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_GATT_OVER_BREDR_AIR, conn_cnf_p->address, BT_CM_PROFILE_SERVICE_STATE_CONNECTED, status);
#endif
        }
        break;
        case BT_GATT_OVER_BREDR_DISCONNECT_IND: {
            bt_gatt_over_bredr_disconnect_ind_t *connection_ind = (bt_gatt_over_bredr_disconnect_ind_t *)buff;
            gatt_over_bredr_air_cntx_t *buffer_t = gatt_over_bredr_air_get_cntx_by_handle(connection_ind->connection_handle);
            if ((connection_ind->connection_handle != BT_HANDLE_INVALID) && (buffer_t)) {
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE) && !defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
                if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
                    bt_status_t sta = bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_GATT_OVER_BREDR_AIR);
                    LOG_MSGID_I(GOBEAIR, "RHO disconnected status: 0x%4x\r\n", 1, sta);
                }
#endif
                if (0 == gatt_over_bredr_air_check_user() && (buffer_t->is_real_connected)) {
                    gatt_over_bredr_air_disconnect_t disconnect_param;
                    memset(&disconnect_param, 0, sizeof(gatt_over_bredr_air_disconnect_t));
                    disconnect_param.conn_handle = connection_ind->connection_handle;
                    memcpy(&disconnect_param.bdaddr, &(buffer_t->peer_addr), BT_BD_ADDR_LEN);
                    LOG_MSGID_I(GOBEAIR, "disconnec handle=0x%04x\r\n", 1, connection_ind->connection_handle);
                    gatt_over_bredr_air_event_callback(GATT_OVER_BREDR_AIR_EVENT_DISCONNECT_IND, (void *)&disconnect_param);
                }
                gatt_over_bredr_air_delete_connection_info(buff);
            }
        }
        break;
        case BT_POWER_OFF_CNF: {
            uint16_t conn_handle = gatt_over_bredr_air_get_real_connected_handle();
            gatt_over_bredr_air_cntx_t *buffer_t = gatt_over_bredr_air_get_cntx_by_handle(conn_handle);
            if ((BT_HANDLE_INVALID != conn_handle) && (buffer_t)) {
                gatt_over_bredr_air_disconnect_t disconnect_param;
                memset(&disconnect_param, 0, sizeof(gatt_over_bredr_air_disconnect_t));
                disconnect_param.conn_handle = conn_handle;
                memcpy(&(disconnect_param.bdaddr), &(buffer_t->peer_addr), BT_BD_ADDR_LEN);
                LOG_MSGID_I(GOBEAIR, "BT power off, then disconn handle=0x%04x\r\n", 1, conn_handle);
                gatt_over_bredr_air_event_callback(GATT_OVER_BREDR_AIR_EVENT_DISCONNECT_IND, (void *)&disconnect_param);
            }
            gatt_over_bredr_air_clear_all_connection_info();
        }
        break;
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

static gatt_over_bredr_air_status_t gatt_over_bredr_air_cb_register(gatt_over_bredr_air_common_callback_t callback)
{
    uint8_t i = 0;
    gatt_over_bredr_air_status_t status = 0;
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_cb_register callback %x\r\n", 1, callback);
    for (i = 0; i < GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (!(gatt_over_bredr_air_cb_list[i].in_use)) {
            gatt_over_bredr_air_cb_list[i].callback = callback;
            gatt_over_bredr_air_cb_list[i].in_use = true;
            gatt_over_bredr_air_connection_status_notify(callback);
            break;
        }
    }
    if (i == GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(GOBEAIR, "all are in use, please extend the value of GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM\r\n", 0);
        status = GATT_OVER_BREDR_AIR_STATUS_FAIL;
    }
    return status;
}

static gatt_over_bredr_air_status_t gatt_over_bredr_air_cb_deregister(gatt_over_bredr_air_common_callback_t callback)
{
    uint8_t i = 0;
    gatt_over_bredr_air_status_t status = 0;
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_cb_deregister callback %x\r\n", 1, callback);
    for (i = 0; i < GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (gatt_over_bredr_air_cb_list[i].in_use && gatt_over_bredr_air_cb_list[i].callback == callback) {
            gatt_over_bredr_air_cb_list[i].callback = NULL;
            gatt_over_bredr_air_cb_list[i].in_use = false;
            break;
        }
    }
    if (i == GATT_OVER_BREDR_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_cb_deregister delete fail, because of not find the callback\r\n", 0);
        status = GATT_OVER_BREDR_AIR_STATUS_FAIL;
    }
    return status;
}

gatt_over_bredr_air_status_t gatt_over_bredr_air_init(gatt_over_bredr_air_common_callback_t app_callback)
{
    GOBEAIR_MUTEX_LOCK();
    gatt_over_bredr_air_status_t status;
    if (NULL == app_callback) {
        status = GATT_OVER_BREDR_AIR_STATUS_INVALID_PARAMETER;
    } else {
        /**Initialize.*/
        LOG_MSGID_I(GOBEAIR, "GOBE init app_callback=0x%04x", 1, app_callback);
        status = gatt_over_bredr_air_cb_register(app_callback);
    }
    GOBEAIR_MUTEX_UNLOCK();
    return status;
}

gatt_over_bredr_air_status_t gatt_over_bredr_air_deinit(gatt_over_bredr_air_common_callback_t app_callback)
{
    GOBEAIR_MUTEX_LOCK();
    gatt_over_bredr_air_status_t status;
    if (NULL == app_callback) {
        status = GATT_OVER_BREDR_AIR_STATUS_INVALID_PARAMETER;
    } else {
        /**Initialize.*/
        LOG_MSGID_I(GOBEAIR, "GOBE deinit app_callback=0x%04x", 1, app_callback);
        status = gatt_over_bredr_air_cb_deregister(app_callback);
    }
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE) && !defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
    uint16_t conn_handle = gatt_over_bredr_air_get_real_connected_handle();
    if (0 != gatt_over_bredr_air_check_user() && (BT_HANDLE_INVALID != conn_handle)) {
        bt_gatt_over_bredr_disconnect(conn_handle);
    }
#endif
    GOBEAIR_MUTEX_UNLOCK();
    return status;
}
#ifdef MTK_BT_CM_SUPPORT
bt_status_t gatt_over_bredr_air_cm_callback_handler(bt_cm_profile_service_handle_t type, void *data)
{
    LOG_MSGID_I(GOBEAIR, "CMCB,gatt_over_bredr_air callback type = %d \r\n", 1, type);
    switch (type) {
        case BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT: {
            bt_bd_addr_t *address = (bt_bd_addr_t *)data;
            uint8_t i = 0;
            for (i = 0; i < BT_CONNECTION_MAX; i++) {
                if ((g_gobe_air_cntx[i].conn_handle != BT_HANDLE_INVALID) && (0 != g_gobe_air_cntx[i].conn_handle)) {
                    LOG_MSGID_I(GOBEAIR, "CMCB,gatt_over_bredr_air disconnect! index = %d\r\n", 1, index);
                    bt_gatt_over_bredr_disconnect(g_gobe_air_cntx[i].conn_handle);
                }
            }
        }
        break;
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}
#endif

static void gatt_over_bredr_air_init_all_cntx(void)
{
    gatt_over_bredr_air_clear_all_connection_info();
    memset(&g_receive_buffer, 0x0, GATT_OVER_BREDR_AIR_RECEIVE_BUFFER_SIZE);
}

bt_status_t gatt_over_bredr_air_event_callback_init(void)
{
    GOBEAIR_MUTEX_LOCK();
    bt_status_t result = bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SYSTEM | MODULE_MASK_GATT, (void *)gatt_over_bredr_air_common_event_handler);
    if (result != BT_STATUS_SUCCESS) {
        LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_event_callback_init fail! \r\n", 0);
        GOBEAIR_MUTEX_UNLOCK();
        return BT_STATUS_FAIL;
    }
    gatt_over_bredr_air_init_all_cntx();
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
    result += bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_GATT_OVER_BREDR_AIR, &gatt_over_bredr_air_rho_callbacks);
#endif
#ifdef MTK_BT_CM_SUPPORT
    bt_cm_profile_service_register(BT_CM_PROFILE_SERVICE_GATT_OVER_BREDR_AIR, gatt_over_bredr_air_cm_callback_handler);
#endif
    bt_callback_manager_add_sdp_customized_record(&bt_gobe_record);
    GOBEAIR_MUTEX_UNLOCK();
    return result;
}


/**
 * @brief Function for application to write data to the send buffer.
 */
uint32_t gatt_over_bredr_air_write_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size)
{
    GOBEAIR_MUTEX_LOCK();
    gatt_over_bredr_air_cntx_t *buffer_t = gatt_over_bredr_air_get_cntx_by_handle(conn_handle);
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t) && (buffer_t->is_real_connected)) {
        uint32_t send_size = (buffer_t->max_packet_size < size) ? buffer_t->max_packet_size : size;
        LOG_MSGID_I(GOBEAIR, "mtu = %d\r\n", 1, buffer_t->max_packet_size);
        if (0 == send_size) {
            LOG_MSGID_I(GOBEAIR, "[GATT_OVER_BREDR_AIR] gatt_over_bredr_air_send_data send_size is 0!\r\n", 0);
            GOBEAIR_MUTEX_UNLOCK();
            return 0;
        }
        if (BT_STATUS_SUCCESS == gatt_over_bredr_air_service_tx_send(conn_handle, buffer, send_size)) {
            LOG_MSGID_I(AIR, "[GATT_OVER_BREDR_AIR] gatt_over_bredr_air_send_data: send_size[%d]\r\n", 1, send_size);
            GOBEAIR_MUTEX_UNLOCK();
            return send_size;
        }
    }
    GOBEAIR_MUTEX_UNLOCK();
    return 0;
}


/**
 * @brief Function for application to read data from the receive buffer.
 */
uint32_t gatt_over_bredr_air_read_data(uint16_t conn_handle, uint8_t *buffer, uint32_t size)
{
    GOBEAIR_MUTEX_LOCK();
    uint32_t read_size = 0;
    gatt_over_bredr_air_cntx_t *buffer_t = gatt_over_bredr_air_get_cntx_by_handle(conn_handle);
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t) && (buffer_t->is_real_connected)) {
        if (buffer_t->receive_buffer_length > size) {
            read_size = size;
        } else {
            read_size = buffer_t->receive_buffer_length;
        }
        if (0 == read_size) {
            LOG_MSGID_I(GOBEAIR, "[GATT_OVER_BREDR_AIR] gatt_over_bredr_air_read_data: read buffer is null\r\n", 0);
            GOBEAIR_MUTEX_UNLOCK();
            return 0;
        }
        memcpy(buffer, buffer_t->receive_buffer, read_size);
        if (buffer_t->receive_buffer_length > read_size) {
            memmove(buffer_t->receive_buffer, (uint8_t *)(buffer_t->receive_buffer + read_size), (buffer_t->receive_buffer_length - read_size));
            buffer_t->receive_buffer_length -= read_size;
        } else {
            buffer_t->receive_buffer_length = 0;
            memset(&g_receive_buffer, 0, GATT_OVER_BREDR_AIR_RECEIVE_BUFFER_SIZE);
        }
        LOG_MSGID_I(GOBEAIR, "[GATT_OVER_BREDR_AIR] gatt_over_bredr_air_read_data: read_size is [%d]\r\n", 1, read_size);
        GOBEAIR_MUTEX_UNLOCK();
        return read_size;
    }
    LOG_MSGID_I(GOBEAIR, "[GATT_OVER_BREDR_AIR] gatt_over_bredr_air_read_data: conn id error [%d]\r\n", 1, conn_handle);
    GOBEAIR_MUTEX_UNLOCK();
    return 0;
}

uint32_t gatt_over_bredr_air_get_rx_available(uint16_t conn_handle)
{
    GOBEAIR_MUTEX_LOCK();
    uint32_t rx_available_len = 0;
    gatt_over_bredr_air_cntx_t *buffer_t = gatt_over_bredr_air_get_cntx_by_handle(conn_handle);
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t) && (buffer_t->is_real_connected)) {
        rx_available_len = buffer_t->receive_buffer_length;
    }
    GOBEAIR_MUTEX_UNLOCK();
    return rx_available_len;
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
uint32_t gatt_over_bredr_air_tx_char_cccd_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(AIR, "gatt_over_bredr_air_tx_char_cccd_callback, rw is %d, size is %d, offset is %d \r\n", 3, rw, size, offset);
    gatt_over_bredr_air_cntx_t *temp_cntx = gatt_over_bredr_air_get_cntx_by_handle(handle);
    if ((handle != BT_HANDLE_INVALID) && (temp_cntx)) {
        /** record for each connection. */
        if (rw == BT_GATTS_CALLBACK_WRITE) {
            if (size != sizeof(uint16_t)) { //Size check
                return 0;
            }
            if ((BT_HANDLE_INVALID == gatt_over_bredr_air_get_real_connected_handle()) ||
                    ((0 == *(uint16_t *)data) && (true == temp_cntx->is_real_connected))) {
                temp_cntx->notify_enabled = *(uint16_t *)data;
            } else {
                return 0;
            }
            if ((false == temp_cntx->is_real_connected) && (BLE_AIR_CCCD_NOTIFICATION == temp_cntx->notify_enabled)) {
                gatt_over_bredr_air_ready_to_write_t ready_to_write;
                temp_cntx->is_real_connected = true;
                memset(&g_receive_buffer, 0, GATT_OVER_BREDR_AIR_RECEIVE_BUFFER_SIZE);
                temp_cntx->receive_buffer = g_receive_buffer;
                if (0 == gatt_over_bredr_air_check_user()) {
                    gatt_over_bredr_air_connect_t connect_param;
                    memset(&connect_param, 0x0, sizeof(gatt_over_bredr_air_connect_t));
                    connect_param.conn_handle = temp_cntx->conn_handle;
                    memcpy(&(connect_param.bdaddr), &(temp_cntx->peer_addr), BT_BD_ADDR_LEN);
                    connect_param.max_packet_length = temp_cntx->max_packet_size;
                    LOG_MSGID_I(GOBEAIR, "GATT_OVER_BREDR_AIR_EVENT_CONNECT_IND\r\n", 0);
                    gatt_over_bredr_air_event_callback(GATT_OVER_BREDR_AIR_EVENT_CONNECT_IND, (void *)&connect_param);
                    memset(&ready_to_write, 0x0, sizeof(ble_air_ready_to_write_t));
                    ready_to_write.conn_handle = handle;
                    memcpy(&(ready_to_write.bdaddr), &(temp_cntx->peer_addr), BT_BD_ADDR_LEN);
                    gatt_over_bredr_air_event_callback(GATT_OVER_BREDR_AIR_EVENT_READY_TO_WRITE_IND, (void *)&ready_to_write);
                }
            } else if ((true == temp_cntx->is_real_connected) && (0 == temp_cntx->notify_enabled))  {
                temp_cntx->is_real_connected = false;
                temp_cntx->receive_buffer = NULL;
                memset(&g_receive_buffer, 0, GATT_OVER_BREDR_AIR_RECEIVE_BUFFER_SIZE);
                if (0 == gatt_over_bredr_air_check_user()) {
                    gatt_over_bredr_air_disconnect_t disconnect_param;
                    memset(&disconnect_param, 0x0, sizeof(gatt_over_bredr_air_disconnect_t));
                    disconnect_param.conn_handle = temp_cntx->conn_handle;
                    memcpy(&(disconnect_param.bdaddr), &(temp_cntx->peer_addr), BT_BD_ADDR_LEN);
                    LOG_MSGID_I(GOBEAIR, "disconnec handle=0x%04x, because CCCD Disable\r\n", 1, handle);
                    gatt_over_bredr_air_event_callback(GATT_OVER_BREDR_AIR_EVENT_DISCONNECT_IND, (void *)&disconnect_param);
                }
            }
            LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_tx_char_cccd_callback, data:%d \r\n", 1,  temp_cntx->notify_enabled);
        } else if (rw == BT_GATTS_CALLBACK_READ) {
            if (size != 0) {
                uint16_t *buf = (uint16_t *) data;
                *buf = (uint16_t) temp_cntx->notify_enabled;
                LOG_MSGID_I(GOBEAIR, "read cccd value = %d\r\n", 1, *buf);
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
uint32_t gatt_over_bredr_air_rx_write_char_callback(const uint8_t rw, uint16_t handle, void *data, uint16_t size, uint16_t offset)
{
    LOG_MSGID_I(AIR, "gobe_air_rx_write_char_callback:rw is %d, size is %d, offset is %d \r\n", 3, rw, size, offset);
    gatt_over_bredr_air_cntx_t *buffer_t = gatt_over_bredr_air_get_cntx_by_handle(handle);
    //TODO:Should record for each connection handle.
    if ((handle != BT_HANDLE_INVALID) && (buffer_t) && (buffer_t->is_real_connected)) {
        if (rw == BT_GATTS_CALLBACK_WRITE) {
            /**remote write & notify app ready to read*/
            LOG_MSGID_I(GOBEAIR, "write length= %d\r\n", 1, size);
            if (size > (GATT_OVER_BREDR_AIR_RECEIVE_BUFFER_SIZE - buffer_t->receive_buffer_length)) {
                LOG_MSGID_I(GOBEAIR, "write characteristic: buffer full error!\r\n", 0);
                return 0; /**means fail, buffer full*/
            }
            memcpy((uint8_t *)(buffer_t->receive_buffer + buffer_t->receive_buffer_length), data, size);
            buffer_t->receive_buffer_length += size;
            if ((0 == gatt_over_bredr_air_check_user())) {
                gatt_over_bredr_air_ready_to_read_t ready_to_read;
                memset(&ready_to_read, 0, sizeof(gatt_over_bredr_air_ready_to_read_t));
                ready_to_read.conn_handle = handle;
                memcpy(&(ready_to_read.bdaddr), &(buffer_t->peer_addr), BT_BD_ADDR_LEN);
                LOG_MSGID_I(GOBEAIR, "write characteristic: write size = %d \r\n", 1, size);
                gatt_over_bredr_air_event_callback(GATT_OVER_BREDR_AIR_EVENT_READY_TO_READ_IND, (void *)&ready_to_read);
            }
            return size;
        }
    }
    return 0;
}

gatt_over_bredr_air_status_t gatt_over_bredr_air_connect(const bt_bd_addr_t *address)
{
    GOBEAIR_MUTEX_LOCK();
    gatt_over_bredr_air_status_t status = 0;
    configASSERT(address && "NULL address");
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
    if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
        LOG_MSGID_I(GOBEAIR, "RHO ongoing, please don't connect spp!\r\n", 0);
        GOBEAIR_MUTEX_UNLOCK();
        return GATT_OVER_BREDR_AIR_STATUS_FAIL;
    }
#endif
    status = bt_gatt_over_bredr_connect(address);
    if (0 != status) {
        status = GATT_OVER_BREDR_AIR_STATUS_FAIL;
    }
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_connect, status: 0x%x\r\n", 1, status);
    GOBEAIR_MUTEX_UNLOCK();
    return status;
}

gatt_over_bredr_air_status_t gatt_over_bredr_air_disconnect(uint16_t handle)
{
    GOBEAIR_MUTEX_LOCK();
    gatt_over_bredr_air_status_t status = 0;
    LOG_MSGID_I(GOBEAIR, "gatt_over_bredr_air_disconnect\r\n", 0);
    if ((BT_HANDLE_INVALID != handle) && (gatt_over_bredr_air_get_cntx_by_handle(handle))) {
        status = bt_gatt_over_bredr_disconnect(handle);
    } else {
        GOBEAIR_MUTEX_UNLOCK();
        return GATT_OVER_BREDR_AIR_STATUS_INVALID_PARAMETER;
    }
    if (0 != status) {
        status = GATT_OVER_BREDR_AIR_STATUS_FAIL;
    }
    GOBEAIR_MUTEX_UNLOCK();
    return status;
}


#endif /*MTK_GATT_OVER_BREDR_ENABLE*/
#endif /*#ifdef MTK_PORT_SERVICE_BT_ENABLE*/





