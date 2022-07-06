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
 * File: bt_gattc_discovery.c
 *
 * Description: The file is discovery GATT service.
 *
 */
#include "bt_gattc_discovery.h"
#include "bt_callback_manager.h"
#include "syslog.h"
#include "bt_init.h"

log_create_module(BT_GATTC, PRINT_LEVEL_INFO);

/**************************Macro and Global*************************/
#define SRV_DISC_INVALID_HANDLE     0x0000   /**< The invalid handle. */
#define SRV_DISC_START_HANDLE  0x0001   /* The start handle value used during service discovery. */
#define SRV_DISC_END_HANDLE    0xFFFF   /* The start handle value used during service discovery. */
#define BT_GATTC_DISCOVERY_MAX_USERS BT_GATTC_DISCOVERY_MAX_SRVS  /* The maximum number of users/registrations allowed by this module. */

#define SRV_DISC_SINGLE_SERVICE_INSTANCE    1

#define BT_GATT_DISCOVERY_FIND_INCLUDED_SERVICES_OFFSET         2
#define BT_GATT_DISCOVERY_HANDLE_VALUE_PAIR_UUID_16BITS_SIZE    8
#define BT_GATT_DISCOVERY_HANDLE_VALUE_PAIR_UUID_32BITS_SIZE    10
#define BT_GATT_DISCOVERY_HANDLE_VALUE_PAIR_UUID_128BITS_SIZE   22
#define BT_GATT_DISCOVERY_START_HANDLE_DATA_OFFSET              2
#define BT_GATT_DISCOVERY_END_HANDLE_DATA_OFFSET                4
#define BT_GATT_DISCOVERY_UUID_DATA_OFFSET                      6
#define BT_GATT_DISCOVERY_ATTR_HANDLE_LEN                       2
#define BT_GATT_DISCOVERY_UUID_16BTS_SIZE                       2
#define BT_GATT_DISCOVERY_UUID_32BTS_SIZE                       4
#define BT_GATT_DISCOVERY_UUID_128BTS_SIZE                      16

static bt_gattc_registered_handlers g_registered_handlers[BT_GATTC_DISCOVERY_MAX_USERS];   /* The discovery GATT service storage variable. */
static bt_gattc_pending_user_events g_pending_user_evts[BT_GATTC_DISCOVERY_MAX_USERS];     /* The discovery event structure for user. */
static uint32_t g_pending_usr_evt_index;    /* The index to the pending user event array, pointing to the last added pending user event. */
static uint32_t g_num_of_handlers_reg;      /* The number of handlers registered with the GATTC discovery module. */
static uint32_t g_num_of_discovered;        /* The total number of service discoveries (successful or unsuccessful) made since initialization. */
static bool     g_initialized = false;      /* This variable indicates if the module is initialized or not. */
static bt_gattc_discovery_t g_gattc_discovery = {0};    /* The discovery maintain variables used by discovery at a time. */

static uint32_t g_num_discovered_success;    /* The total number of service discoveries (only successful) made since initialization. */
static uint32_t g_retry_discovering_times;   /* The times of discovery retry. */
static bool g_need_restart_discovery = false;       /* This variable indicates whether discovery needs to be rediscovered. */

static bt_handle_t g_pending_handle_list[BT_LE_CONNECTION_NUM];
/************************************************
*   Utilities
*************************************************/
static void bt_gattc_discovery_input_buffer_reset(bt_gattc_discovery_service_t *buffer);
static bt_status_t bt_gattc_discovery_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);
static bt_status_t bt_gattc_discovery_start_discover_service(uint16_t conn_handle, ble_uuid_t *srv_uuid);
static void bt_gattc_discovery_primary_service_cnf(bt_status_t status, void *buff);
static void bt_gattc_discovery_parse_primary_service(bt_gattc_find_by_type_value_rsp_t *rsp, bt_gattc_discovery_service_t *data);
static bt_status_t bt_gattc_discovery_start_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);
static void bt_gattc_discovery_charactiristics_cnf(bt_status_t status, void *buff);
static void bt_gattc_discovery_parse_characteristic(bt_gattc_read_by_type_rsp_t *data, bt_gattc_discovery_service_t *service);
static void bt_gattc_discovery_parse_char_command_data(bt_gattc_discovery_characteristic_t *char_info, uint8_t *attribute_data_list);
static bt_status_t bt_gattc_discovery_all_descriptor(uint16_t conn_handle, bt_gattc_discovery_service_t *service, uint8_t index);
static bt_status_t bt_gattc_discovery_descriptor_of_characteristic(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);
static void bt_gattc_discovery_descriptors_cnf(bt_status_t status, void *buff);
static void bt_gattc_discovery_parse_descriptor(bt_gattc_find_info_rsp_t *data, bt_gattc_discovery_characteristic_t *chara);
static void bt_gattc_discovery_discover_service_complete(void);
static void bt_gattc_discovery_disconnect_error_evt_trigger(void);
static void bt_gattc_discovery_result_evt_trigger(bool is_complete, int32_t err_code);
static void bt_gattc_discovery_pending_user_evts_send(void);
static bt_gattc_discovery_status_t bt_gattc_discovery_registered_handler_set(ble_uuid_t *srv_uuid, bt_gattc_discovery_service_t *srv_info,
                                             const bt_gattc_discovery_event_handler_t evt_handler);
static void bt_gattc_discovery_find_included_services_cnf(bt_status_t status, bt_gattc_read_by_type_rsp_t *p_rsp);
static bt_status_t bt_gattc_discovery_find_included_service(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle);

/************************************************
*   Utilities
*************************************************/
static bool _bt_gattc_add_pending_handle_list(bt_handle_t handle)
{
    uint8_t i = BT_LE_CONNECTION_NUM;

    while (i != 0) {
        i--;
        if (handle == g_pending_handle_list[i]) {
            return true;
        }
    }

    i = BT_LE_CONNECTION_NUM;
    while (i != 0) {
        i--;
        if (BT_HANDLE_INVALID == g_pending_handle_list[i]) {
            g_pending_handle_list[i] = handle;
            return true;
        }
    }
    return false;
}

static void _bt_gattc_remove_pending_handle_list(bt_handle_t handle)
{
    uint8_t i = BT_LE_CONNECTION_NUM;

    while (i != 0) {
        i--;
        if (handle == g_pending_handle_list[i]) {
            g_pending_handle_list[i] = BT_HANDLE_INVALID;
        }
    }
}


static void _bt_gattc_discover_characteristic(uint16_t conn_handle, bt_gattc_discovery_service_t *pDiscovery)
{
    bt_status_t sta;

    sta = bt_gattc_discovery_start_discover_characteristics(conn_handle, pDiscovery->start_handle, pDiscovery->end_handle);
    if (BT_STATUS_SUCCESS != sta) {
        /**< Error with discovering the service. Indicate the error to the registered user application. */
        bt_gattc_discovery_result_evt_trigger(false, sta);

        #ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
            bt_gattc_discovery_discover_service_complete();
        #endif
    }
}

static void _bt_gattc_discover_descriptor(uint16_t conn_handle, bt_gattc_discovery_service_t *pDiscovery)
{
    bt_status_t sta;

    sta = bt_gattc_discovery_all_descriptor(conn_handle, pDiscovery, 0);
    if (BT_STATUS_SUCCESS != sta) {
        /**< Error with discovering the service. Indicate the error to the registered user application. */
        bt_gattc_discovery_result_evt_trigger(false, sta);

        #ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
            bt_gattc_discovery_discover_service_complete();
        #endif
    }
}

static void bt_gattc_discovery_input_buffer_service_reset(bt_gattc_discovery_service_t *buffer)
{
    buffer->start_handle = 0x00;
    buffer->end_handle = 0x00;
    buffer->char_count_found = 0;
    memset(&(buffer->service_uuid), 0x00, sizeof(ble_uuid_t));
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_input_buffer_service_reset char_count is %d\r\n", 1, buffer->characteristic_count);
    for(int i = 0; i < buffer->characteristic_count; i++) {
        (buffer->charateristics + i)->handle = 0x00;
        (buffer->charateristics + i)->value_handle = 0x00;
        (buffer->charateristics + i)->property = 0x00;
        (buffer->charateristics + i)->descr_count_found = 0;
        memset(&((buffer->charateristics + i)->char_uuid), 0x00, sizeof(ble_uuid_t));
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_input_buffer_service_reset descr_count is %d\r\n", 1, (buffer->charateristics + i)->descriptor_count);
        for(int j = 0; j < (buffer->charateristics + i)->descriptor_count; j++) {
            ((buffer->charateristics + i)->descriptor + j)->handle = 0x00;
            memset(&(((buffer->charateristics + i)->descriptor + j)->descriptor_uuid), 0x00, sizeof(ble_uuid_t));
        }
    }
}

static void bt_gattc_discovery_input_buffer_reset(bt_gattc_discovery_service_t *buffer)
{
    if (buffer->service_instance != NULL) {
        vPortFree(buffer->service_instance);
        buffer->service_instance = NULL;
    }
    memset(&(buffer->service_uuid), 0x00, sizeof(ble_uuid_t));

    bt_gattc_discovery_input_buffer_service_reset(buffer);
}

static bt_status_t bt_gattc_discovery_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
#ifndef AIR_LE_AUDIO_ENABLE
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_event_callback: status(0x%04x), msg(0x%04x)", 2, status, msg);
#endif

    if (!g_initialized)
    {
        return BT_STATUS_SUCCESS;
    }
    switch (msg) {
    case BT_GAP_LE_DISCONNECT_IND: {
        /* LE disconnected complete. */
        bt_gap_le_disconnect_ind_t *dis_ind = (bt_gap_le_disconnect_ind_t *)buff;
        _bt_gattc_remove_pending_handle_list(dis_ind->connection_handle);
        if (g_gattc_discovery.conn_handle == dis_ind->connection_handle) {
            if (g_gattc_discovery.discovery_in_progress) {
                bt_gattc_discovery_disconnect_error_evt_trigger();
                /*
                g_num_of_discovered = 0;
                memset(&g_gattc_discovery, 0x00, sizeof(bt_gattc_discovery_t));
                */
                g_gattc_discovery.discovery_in_progress = false;
            }
        }
        break;
    }
    case BT_GATTC_DISCOVER_PRIMARY_SERVICE_BY_UUID: {
        /* GATTC discovery primary service by uuid response event. */
        bt_gattc_discovery_primary_service_cnf(status, buff);
        break;
    }
        case BT_GATTC_FIND_INCLUDED_SERVICES: {
            bt_gattc_discovery_find_included_services_cnf(status, (bt_gattc_read_by_type_rsp_t *)buff);
            break;
        }
    case BT_GATTC_DISCOVER_CHARC: {
        /* GATTC discovery charc response event. */
        bt_gattc_discovery_charactiristics_cnf(status, buff);
        break;
    }
    case BT_GATTC_DISCOVER_CHARC_DESCRIPTOR: {
        /* GATTC discovery charc descriptor response event. */
        bt_gattc_discovery_descriptors_cnf(status, buff);
        break;
    }
    default:
        break;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_gattc_discovery_init(void)
{
    uint8_t i = BT_LE_CONNECTION_NUM;

    if (g_initialized) {
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_init fail, because it has been initialized by others! \r\n", 0);
        return BT_STATUS_FAIL;
    }
    g_num_discovered_success = 0;
    g_retry_discovering_times = 0;
    g_need_restart_discovery = false;
    g_num_of_handlers_reg      = 0;
    g_initialized              = true;
    g_num_of_discovered        = 0;
    g_pending_usr_evt_index    = 0;
    memset(&g_gattc_discovery, 0x00, sizeof(bt_gattc_discovery_t));
    g_gattc_discovery.discovery_in_progress = false;

    while (i != 0) {
        i--;
        g_pending_handle_list[i] = BT_HANDLE_INVALID;
    }

    return bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_GATT, (void *)bt_gattc_discovery_event_callback);
}

#if 0
static bt_status_t bt_gattc_discovery_deinit(void)
{
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_deinit: enter\r\n", 0);
    g_num_of_handlers_reg      = 0;
    g_initialized              = false;
    g_num_of_discovered        = 0;
    g_pending_usr_evt_index    = 0;
    memset(&g_gattc_discovery, 0x00, sizeof(bt_gattc_discovery_t));
    g_gattc_discovery.discovery_in_progress = false;
    return bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_gattc_discovery_event_callback);
}
#endif

bt_gattc_discovery_status_t bt_gattc_discovery_service_register(ble_uuid_t *uuid, bt_gattc_discovery_service_t *srv_info, const bt_gattc_discovery_event_handler_t handler)
{
    if ((uuid == NULL) || (srv_info == NULL) || (handler == NULL)) {
        /* Have pointer is NULL. */
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_service_register fail, errcode is Input parameters error\r\n", 0);
        return BT_GATTC_DISCOVERY_STATUS_INVALID_PARAMS;
    }
    if (!g_initialized) {
        /* Please call bt_gattc_discovery_init first. */
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_service_register fail, errcode is invalid state\r\n", 0);
        return BT_GATTC_DISCOVERY_STATUS_INVALID_STATE;
    }
    if (g_num_of_handlers_reg == BT_GATTC_DISCOVERY_MAX_USERS) {
        /* The limit on the maximum number of supported users has been reached. Return error. */
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_service_register fail, errcode is user buffer OOM\r\n", 0);
        return BT_GATTC_DISCOVERY_STATUS_OUT_OF_MEMORY;
    }
    if (g_gattc_discovery.discovery_in_progress) {
        /* A discovery is already in progress. Please register again later. */
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_service_register fail, errcode is now in discovering\r\n", 0);
        return BT_GATTC_DISCOVERY_STATUS_BUSY;
    }
    return bt_gattc_discovery_registered_handler_set(uuid, srv_info, handler);
}

static bt_gattc_discovery_status_t bt_gattc_discovery_registered_handler_set(ble_uuid_t *srv_uuid, bt_gattc_discovery_service_t *srv_info,
        const bt_gattc_discovery_event_handler_t evt_handler)
{
    if (g_num_of_handlers_reg < BT_GATTC_DISCOVERY_MAX_USERS) {
        memset(&(g_registered_handlers[g_num_of_handlers_reg].service_uuid), 0x00, sizeof(ble_uuid_t));
        g_registered_handlers[g_num_of_handlers_reg].service_uuid.type = srv_uuid->type;
        if (BLE_UUID_TYPE_16BIT == g_registered_handlers[g_num_of_handlers_reg].service_uuid.type) {
            /* Register uuid is 16 bit. */
            g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid16 = srv_uuid->uuid.uuid16;
            LOG_MSGID_I(BT_GATTC, "uuid16 = 0x%04x\r\n", 1, g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid16);
        } else if (BLE_UUID_TYPE_32BIT == g_registered_handlers[g_num_of_handlers_reg].service_uuid.type) {
            /* Register uuid is 32 bit. */
            g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid16 = srv_uuid->uuid.uuid32;
            LOG_MSGID_I(BT_GATTC, "uuid32 = 0x%04x\r\n", 1, g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid32);
        } else if (BLE_UUID_TYPE_128BIT == g_registered_handlers[g_num_of_handlers_reg].service_uuid.type) {
            /* Register uuid is 128 bit. */
            memcpy(g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid, srv_uuid->uuid.uuid, 16);
            for (int i = 15; i >= 0; i--) {
                LOG_MSGID_I(BT_GATTC, "0x%02x", 1, g_registered_handlers[g_num_of_handlers_reg].service_uuid.uuid.uuid[i]);
            }
            LOG_MSGID_I(BT_GATTC, "\n", 0);
        } else {
            LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_registered_handler_set error uuid! \r\n", 0);
            return BT_GATTC_DISCOVERY_STATUS_INVALID_UUID;
        }
        g_registered_handlers[g_num_of_handlers_reg].service_info = srv_info;
        g_registered_handlers[g_num_of_handlers_reg].event_handler = evt_handler;
        g_num_of_handlers_reg++;
        return BT_GATTC_DISCOVERY_STATUS_SUCCESS;
    } else {
        return BT_GATTC_DISCOVERY_STATUS_OUT_OF_MEMORY;
    }
}

void bt_gattc_reset_before_discovery_restart(void)
{
    uint32_t i;
    g_num_discovered_success = 0;
    for (i = 0; i < g_num_of_handlers_reg; i++) {
        memset(&(g_pending_user_evts[i]), 0x00, sizeof(bt_gattc_pending_user_events));
    }
    if (i == g_num_of_handlers_reg) {
        g_pending_usr_evt_index = 0;
        LOG_MSGID_I(BT_GATTC, "bt_gattc_reset_before_discovery_restart: done\r\n", 0);
    }
}

static bt_gattc_discovery_status_t bt_gattc_discovery_restart(uint16_t conn_handle)
{
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_restart: conn_handle is 0x%04x\r\n", 1, conn_handle);
    return bt_gattc_discovery_start(conn_handle);
}

bt_gattc_discovery_status_t bt_gattc_discovery_get_connect_handle(uint16_t *conn_handle)
{
    if (BT_HANDLE_INVALID == g_gattc_discovery.conn_handle) {
        return BT_GATTC_DISCOVERY_STATUS_FAIL;
    }
    *conn_handle = g_gattc_discovery.conn_handle;
    return BT_GATTC_DISCOVERY_STATUS_SUCCESS;
}

bt_gattc_discovery_status_t bt_gattc_discovery_start(uint16_t conn_handle)
{
    int32_t err_code;
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_start: conn_handle is 0x%04x\r\n", 1, conn_handle);
    if (!g_initialized) {
        /* Please call bt_gattc_discovery_init first. */
        return BT_GATTC_DISCOVERY_STATUS_INVALID_STATE;
    }
    if (g_num_of_handlers_reg == 0) {
        /* No user modules were registered. There are no services to discover. */
        return BT_GATTC_DISCOVERY_STATUS_INVALID_STATE;
    }
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_start discovery_in_progress is %d\r\n", 1, g_gattc_discovery.discovery_in_progress);
    if (g_gattc_discovery.discovery_in_progress) {
         if (conn_handle == g_gattc_discovery.conn_handle) {
            LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_start already start", 0);
            return BT_GATTC_DISCOVERY_STATUS_SUCCESS;
        }
        _bt_gattc_add_pending_handle_list(conn_handle);

        /* Have a user modules call this api. */
        return BT_GATTC_DISCOVERY_STATUS_BUSY;
    }
    bt_gattc_discovery_service_t *p_srv_being_discovered;
    g_num_of_discovered = 0;
    g_pending_usr_evt_index   = 0;
    g_num_discovered_success = 0;
    g_gattc_discovery.curr_srv_ind = 0;
    g_gattc_discovery.conn_handle  = conn_handle;
    g_gattc_discovery.curr_char_ind = 0;
    g_gattc_discovery.char_count = 0;
    g_gattc_discovery.descr_count = 0;
    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
    bt_gattc_discovery_input_buffer_reset(p_srv_being_discovered);
    p_srv_being_discovered->service_uuid.type = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type;
    if (BLE_UUID_TYPE_16BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
        /* Register uuid is 16 bit. */
        p_srv_being_discovered->service_uuid.uuid.uuid16 = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid16;
        LOG_MSGID_I(BT_GATTC, "Starting discovery of service with UUID16 0x%04x for Connection handle %d\r\n", 2,
                    p_srv_being_discovered->service_uuid.uuid.uuid16, g_gattc_discovery.conn_handle);
    } else if (BLE_UUID_TYPE_32BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
        /* Register uuid is 32 bit. */
        p_srv_being_discovered->service_uuid.uuid.uuid32 = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid32;
        LOG_MSGID_I(BT_GATTC, "Starting discovery of service with UUID32 0x%08x for Connection handle %d\r\n", 2,
                    p_srv_being_discovered->service_uuid.uuid.uuid32, g_gattc_discovery.conn_handle);
    } else if (BLE_UUID_TYPE_128BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
        /* Register uuid is 128 bit. */
        memcpy(p_srv_being_discovered->service_uuid.uuid.uuid, g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid, 16);
        for (int i = 15; i >= 0; i--) {
            LOG_MSGID_I(BT_GATTC, "0x%02x", 1, p_srv_being_discovered->service_uuid.uuid.uuid[i]);
        }
        LOG_MSGID_I(BT_GATTC, "\n", 0);
        LOG_MSGID_I(BT_GATTC, "Starting discovery of service with UUID128 for Connection handle %d\r\n", 1, g_gattc_discovery.conn_handle);
    }
    if (p_srv_being_discovered->is_included_definition) {
        err_code = bt_gattc_discovery_find_included_service(g_gattc_discovery.conn_handle, p_srv_being_discovered->discovery_start_handle, p_srv_being_discovered->discovery_end_handle);
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_find_included_service err_code:0x%x", 1, err_code);
    } else {
        err_code = bt_gattc_discovery_start_discover_service(g_gattc_discovery.conn_handle, &(p_srv_being_discovered->service_uuid));
    }

    if (err_code != BT_STATUS_SUCCESS) {
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_start_discover_service: err_code = 0x%04x\r\n", 1, err_code);
        return BT_GATTC_DISCOVERY_STATUS_FAIL;
    }
    g_gattc_discovery.discovery_in_progress = true;
    return BT_GATTC_DISCOVERY_STATUS_SUCCESS;
}

static bt_status_t bt_gattc_discovery_start_discover_service(uint16_t conn_handle, ble_uuid_t *srv_uuid)
{
    uint8_t buff[40];
    bt_gattc_discover_primary_service_by_uuid_req_t req;
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_start_discover_service: conn_handle is 0x%04x\r\n", 1, conn_handle);
    req.att_req = (bt_att_find_by_type_value_req_t *)buff;
    if (BLE_UUID_TYPE_16BIT == srv_uuid->type) {
        req.attribute_value_length = 2;
        memcpy(req.att_req->attribute_value, &(srv_uuid->uuid.uuid16), 2);
    } else if (BLE_UUID_TYPE_32BIT == srv_uuid->type) {
        req.attribute_value_length = 4;
        memcpy(req.att_req->attribute_value, &(srv_uuid->uuid.uuid32), 4);
    } else if (BLE_UUID_TYPE_128BIT == srv_uuid->type) {
        req.attribute_value_length = 16;
        memcpy(req.att_req->attribute_value, &(srv_uuid->uuid.uuid), 16);
    }
    req.att_req->opcode = BT_ATT_OPCODE_FIND_BY_TYPE_VALUE_REQUEST;
    req.att_req->starting_handle = SRV_DISC_START_HANDLE;
    req.att_req->ending_handle = SRV_DISC_END_HANDLE;
    req.att_req->uuid = BT_GATT_UUID16_PRIMARY_SERVICE;
    return bt_gattc_discover_primary_service_by_uuid(conn_handle, &req);
}

static bt_status_t bt_gattc_discovery_find_included_service(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle)
{
    if (SRV_DISC_INVALID_HANDLE == start_handle || SRV_DISC_INVALID_HANDLE == end_handle) {
        return BT_STATUS_FAIL;
    }

    g_gattc_discovery.curr_char_ind = 0;
    g_gattc_discovery.char_count = 0;
    g_gattc_discovery.descr_count = 0;
    bt_gattc_discovery_input_buffer_service_reset(g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info);

    BT_GATTC_NEW_FIND_INCLUDED_SERVICE_REQ(req, start_handle, end_handle);
    return bt_gattc_find_included_services(conn_handle, &req);
}

static void bt_gattc_discovery_primary_service_cnf(bt_status_t status, void *buff)
{
    uint16_t conn_handle = 0;
    bt_gattc_discovery_service_t *p_srv_being_discovered;

    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_primary_service_cnf: status = %d\r\n", 1, status);
    if (buff == NULL) {
        LOG_MSGID_I(BT_GATTC, "DiscoveryServiceCnf buff NULL!\r\n", 0);
        return;
    }

    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;

    if ((status != BT_STATUS_SUCCESS) && (status != BT_ATT_ERRCODE_CONTINUE)) {

        bt_gattc_error_rsp_t *p_event_data = (bt_gattc_error_rsp_t *)buff;
        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
        }

        if (status != BT_ATT_ERRCODE_ATTRIBUTE_NOT_FOUND) {
            /**< Trigger Service Not Found event to the application. */
            bt_gattc_discovery_result_evt_trigger(false, BT_GATTC_DISCOVERY_ERROR_SERVICE_NOT_FOUND);

            #ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
                bt_gattc_discovery_discover_service_complete();
            #endif
            return;
        }

        /* status == BT_ATT_ERRCODE_ATTRIBUTE_NOT_FOUND */
        if (p_srv_being_discovered->start_handle < p_srv_being_discovered->end_handle) {
            _bt_gattc_discover_characteristic(conn_handle, p_srv_being_discovered);

        } else {
            /**< Trigger Service Not Found event to the application. */
            bt_gattc_discovery_result_evt_trigger(false, BT_GATTC_DISCOVERY_ERROR_SERVICE_NOT_FOUND);

            #ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
                bt_gattc_discovery_discover_service_complete();
            #endif
        }

    } else {

        bt_gattc_find_by_type_value_rsp_t *p_event_data = (bt_gattc_find_by_type_value_rsp_t *)buff;
        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
        }

        if (p_event_data->att_rsp) {
            bt_gattc_discovery_parse_primary_service(p_event_data, p_srv_being_discovered);
        }

        if (status == BT_ATT_ERRCODE_CONTINUE) {
            return;
        }

        /* status == BT_STATUS_SUCCESS */
        if (p_srv_being_discovered->characteristic_count) {
            _bt_gattc_discover_characteristic(conn_handle, p_srv_being_discovered);

        }  else {
            /**< Trigger Service Not Found event to the application. */
            bt_gattc_discovery_result_evt_trigger(true, 0);

            #ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
                bt_gattc_discovery_discover_service_complete();
            #endif

        }
    }
}

static void bt_gattc_discovery_parse_included_service(bt_gattc_read_by_type_rsp_t *p_rsp, bt_gattc_discovery_service_t *data, ble_uuid_t *service_uuid)
{
    uint32_t num_of_data;
    uint32_t uuid_size;
    uint32_t pair_len;
    ble_uuid_type_t type;;
    uint8_t *attribute_data_list;

#ifdef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
    bt_gattc_discovery_service_instance_t *p_instance = NULL;
    uint32_t instance_num;
#endif

    pair_len = p_rsp->att_rsp->length;
    if (pair_len < BT_GATT_DISCOVERY_HANDLE_VALUE_PAIR_UUID_16BITS_SIZE) {
        return;
    }
    uuid_size = pair_len - BT_GATT_DISCOVERY_UUID_DATA_OFFSET;

    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_parse_included_service uuid_size: %d\r\n", 1, uuid_size);

    if (BT_GATT_DISCOVERY_UUID_16BTS_SIZE == uuid_size) {
        type = BLE_UUID_TYPE_16BIT;
    } else if (BT_GATT_DISCOVERY_UUID_32BTS_SIZE == uuid_size) {
        type = BLE_UUID_TYPE_32BIT;
    } else if (BT_GATT_DISCOVERY_UUID_128BTS_SIZE == uuid_size) {
        type = BLE_UUID_TYPE_128BIT;
    } else {
        return;
    }

    attribute_data_list = p_rsp->att_rsp->attribute_data_list;
    num_of_data = (p_rsp->length - BT_GATT_DISCOVERY_FIND_INCLUDED_SERVICES_OFFSET) / pair_len;

    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_parse_included_service serviceNum: %d\r\n", 1, num_of_data);

#ifdef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
    if ((num_of_data != SRV_DISC_SINGLE_SERVICE_INSTANCE) && (data->service_instance == NULL)) {

        /* Multi-instance service discovery */
        if (NULL != (p_instance = (bt_gattc_discovery_service_instance_t *)pvPortMalloc(sizeof(bt_gattc_discovery_service_instance_t)))) {
            memset(p_instance, 0, sizeof(bt_gattc_discovery_service_instance_t));
            data->service_instance = p_instance;
        }
    }

    if (NULL != (p_instance = data->service_instance)) {

        /* Multi-instance service discovery */
        instance_num = (BT_GATTC_DISCOVERY_MAX_SRVS_INSTANCE - p_instance->instance_count);
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_parse_included_service multi count:%d, free_record:%d num_of_data:%d\r\n", 3, p_instance->instance_count, instance_num, num_of_data);

        if (instance_num > num_of_data) {
            instance_num = num_of_data;
        }

        while (instance_num > 0) {
            instance_num--;
            if (!memcmp(&service_uuid->uuid, attribute_data_list + BT_GATT_DISCOVERY_UUID_DATA_OFFSET, uuid_size) && service_uuid->type == type) {
                memcpy(&(p_instance->start_handle[p_instance->instance_count]), attribute_data_list + BT_GATT_DISCOVERY_START_HANDLE_DATA_OFFSET, BT_GATT_DISCOVERY_ATTR_HANDLE_LEN);
                memcpy(&(p_instance->end_handle[p_instance->instance_count]), attribute_data_list +  BT_GATT_DISCOVERY_END_HANDLE_DATA_OFFSET, BT_GATT_DISCOVERY_ATTR_HANDLE_LEN);
                LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_parse_included_service multi[%d] startHdl: 0x%04x, endHdl: 0x%04x \r\n", 3, p_instance->instance_count, p_instance->start_handle[p_instance->instance_count], p_instance->end_handle[p_instance->instance_count]);
                if (SRV_DISC_INVALID_HANDLE == data->end_handle) {
                    data->end_handle = SRV_DISC_END_HANDLE; /* PTS BUG */
                }
                p_instance->instance_count++;
            }
            attribute_data_list += pair_len;
        }

        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_parse_included_service multi instance_count:%d, instance_num:%d\r\n", 2, p_instance->instance_count, instance_num);

        data->start_handle = p_instance->start_handle[0];
        data->end_handle = p_instance->end_handle[0];

    } else
#endif
    {
        while (num_of_data > 0) {
            num_of_data--;
            LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_parse_included_service service uuid:%d 0x%04x", 2, service_uuid->type, service_uuid->uuid.uuid16);
            if (!memcmp(&service_uuid->uuid, attribute_data_list + BT_GATT_DISCOVERY_UUID_DATA_OFFSET, uuid_size) && service_uuid->type == type) {
                memcpy(&data->start_handle, attribute_data_list + BT_GATT_DISCOVERY_START_HANDLE_DATA_OFFSET, BT_GATT_DISCOVERY_ATTR_HANDLE_LEN);
                memcpy(&data->end_handle, attribute_data_list + BT_GATT_DISCOVERY_END_HANDLE_DATA_OFFSET, BT_GATT_DISCOVERY_ATTR_HANDLE_LEN);
                LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_parse_included_service startHdl: 0x%04x, endHdl: 0x%04x \r\n", 2, data->start_handle, data->end_handle);
                if (SRV_DISC_INVALID_HANDLE == data->end_handle) {
                    data->end_handle = SRV_DISC_END_HANDLE; /* PTS BUG */
                }
            }
            attribute_data_list += pair_len;
        }
    }
}

static void bt_gattc_discovery_find_included_services_cnf(bt_status_t status, bt_gattc_read_by_type_rsp_t *p_rsp)
{
    uint16_t conn_handle = 0;
    bt_gattc_discovery_service_t *p_srv_being_discovered;

    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_find_included_services_cnf status: 0x%x p_rsp:%x\r\n", 2, status, (uint32_t)(uint8_t *)p_rsp);

    if (NULL == p_rsp) {
        return;
    }

    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;

    if ((status != BT_STATUS_SUCCESS) && (status != BT_ATT_ERRCODE_CONTINUE)) {
        bt_gattc_error_rsp_t *p_event_data = (bt_gattc_error_rsp_t *)p_rsp;

        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
        }

        if (status != BT_ATT_ERRCODE_ATTRIBUTE_NOT_FOUND) {
            /**< Trigger Service Not Found event to the application. */
            bt_gattc_discovery_result_evt_trigger(false, BT_GATTC_DISCOVERY_ERROR_SERVICE_NOT_FOUND);

#ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
            bt_gattc_discovery_discover_service_complete();
#endif
            return;
        }

        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_find_included_services_cnf status:0x%x, discovered start_hdle:0x%x end_hdle:0x%x \r\n", 3, status, p_srv_being_discovered->start_handle, p_srv_being_discovered->end_handle);

        /* status == BT_ATT_ERRCODE_ATTRIBUTE_NOT_FOUND */
        if (p_srv_being_discovered->start_handle < p_srv_being_discovered->end_handle) {
            _bt_gattc_discover_characteristic(conn_handle, p_srv_being_discovered);
            return;
        } else {
            /**< Trigger Service Not Found event to the application. */
            bt_gattc_discovery_result_evt_trigger(false, BT_GATTC_DISCOVERY_ERROR_SERVICE_NOT_FOUND);
#ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
            bt_gattc_discovery_discover_service_complete();
#endif
            return;
        }
    } else {
        conn_handle = p_rsp->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
        }

        if (p_rsp->att_rsp) {
            bt_gattc_discovery_parse_included_service(p_rsp, p_srv_being_discovered, &g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid);
        }

        if (status == BT_ATT_ERRCODE_CONTINUE) {
            return;
        }

        /* status == BT_STATUS_SUCCESS */
        if (p_srv_being_discovered->characteristic_count) {
            _bt_gattc_discover_characteristic(conn_handle, p_srv_being_discovered);
        } else {
            /**< Trigger Service Not Found event to the application. */
            bt_gattc_discovery_result_evt_trigger(true, 0);

#ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
            bt_gattc_discovery_discover_service_complete();
#endif
        }
    }
}

static void bt_gattc_discovery_parse_primary_service(bt_gattc_find_by_type_value_rsp_t *rsp, bt_gattc_discovery_service_t *data)
{
#ifdef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
    bt_gattc_discovery_service_instance_t *p_instance = NULL;
#endif
    uint8_t *attribute_data_list = rsp->att_rsp->handles_info_list;
    uint8_t num_of_data = (rsp->length - 1) / 4;
    uint8_t i;
#ifdef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
    uint8_t instance_num;
#endif

    LOG_MSGID_I(BT_GATTC, "DiscoveryServiceParse serviceNum: %d\r\n", 1, num_of_data);

#ifdef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
    if ((num_of_data != SRV_DISC_SINGLE_SERVICE_INSTANCE) && (data->service_instance == NULL)) {

        /* Multi-instance service discovery */
        if (NULL != (p_instance = (bt_gattc_discovery_service_instance_t*)pvPortMalloc(sizeof(bt_gattc_discovery_service_instance_t)))) {
            memset(p_instance, 0, sizeof(bt_gattc_discovery_service_instance_t));
            data->service_instance = p_instance;
        }
    }

    if (NULL != (p_instance = data->service_instance)) {

        /* Multi-instance service discovery */
        instance_num = (BT_GATTC_DISCOVERY_MAX_SRVS_INSTANCE - p_instance->instance_count);
        LOG_MSGID_I(BT_GATTC, "DiscoveryServiceParse multi count:%d, free_record:%d\r\n", 2, p_instance->instance_count, instance_num);

        if (instance_num < num_of_data) {
            instance_num = BT_GATTC_DISCOVERY_MAX_SRVS_INSTANCE;
        } else {
            instance_num = (p_instance->instance_count+num_of_data);
        }

        for (i = p_instance->instance_count; i < instance_num; i++) {
            memcpy(&(p_instance->start_handle[i]), attribute_data_list + i * 4, 2);
            memcpy(&(p_instance->end_handle[i]), attribute_data_list +  i * 4 + 2, 2);
            LOG_MSGID_I(BT_GATTC, "DiscoveryServiceParse multi[%d] startHdl: 0x%04x, endHdl: 0x%04x \r\n", 3, i, p_instance->start_handle[i], p_instance->end_handle[i]);
        }

        p_instance->instance_count = instance_num;
        LOG_MSGID_I(BT_GATTC, "DiscoveryServiceParse multi instance_count:%d, instance_num:%d\r\n", 2, p_instance->instance_count, instance_num);

        data->start_handle = p_instance->start_handle[0];
        data->end_handle = p_instance->end_handle[0];

    } else
#endif
    {
    for (i = 0 ; i < num_of_data; i++) {
        memcpy(&(data->start_handle), attribute_data_list + i * 4, sizeof(uint16_t));
        memcpy(&(data->end_handle), attribute_data_list +  i * 4 + 2, sizeof(uint16_t));
        LOG_MSGID_I(BT_GATTC, "starting_handle = 0x%04x, end_handle = 0x%04x \r\n", 2, data->start_handle, data->end_handle);
    }

    }
}

/**@brief     Function for performing characteristic discovery.
 *
 * @param[in] g_gattc_discovery Pointer to the DB discovery structure.
 *
 * @return    NRF_SUCCESS if the SoftDevice was successfully requested to perform the characteristic
 *            discovery. Otherwise an error code. This function returns the error code returned
 *            by the SoftDevice API @ref sd_ble_gattc_characteristics_discover.
 */
static bt_status_t bt_gattc_discovery_start_discover_characteristics(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle)
{
    bt_status_t status;
    bt_gattc_discover_charc_req_t req;
    LOG_MSGID_I(BT_GATTC, "discover_char_start: handle range: start_handle = 0x%04x, end_handle = 0x%04x\r\n", 2, start_handle, end_handle);
    uint16_t uuid_16 = BT_GATT_UUID16_CHARC;
    req.opcode = BT_ATT_OPCODE_READ_BY_TYPE_REQUEST;
    req.starting_handle = start_handle;
    req.ending_handle = end_handle;
    bt_uuid_load(&(req.type), (void *)&uuid_16, 2);
    status = bt_gattc_discover_charc(conn_handle, &req);
    LOG_MSGID_I(BT_GATTC, "dis_char end: status = %d", 1, status);
    return status;
}

static void bt_gattc_discovery_charactiristics_cnf(bt_status_t status, void *buff)
{
    uint16_t conn_handle;
    bt_gattc_discovery_service_t *p_srv_being_discovered;

    LOG_MSGID_I(BT_GATTC, "discover_charactiristics_cnf: status = %d\r\n", 1, status);

    if (buff == NULL) {
        return;
    }

    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;

    if ((status != BT_STATUS_SUCCESS) && (status != BT_ATT_ERRCODE_CONTINUE)) {

        bt_gattc_error_rsp_t *p_event_data = (bt_gattc_error_rsp_t *)buff;

        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
    }

        LOG_MSGID_I(BT_GATTC, "disc_char_cnf: not found char_num = %d\r\n", 1, g_gattc_discovery.char_count);
        /**< The previous characteristic discovery resulted in no characteristics.
        Descriptor discovery should be performed. */
        _bt_gattc_discover_descriptor(conn_handle, p_srv_being_discovered);

    } else {

        bt_gattc_read_by_type_rsp_t *p_event_data = (bt_gattc_read_by_type_rsp_t *)buff;

        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
        }

        if (p_event_data->att_rsp) {
            bt_gattc_discovery_parse_characteristic(p_event_data, p_srv_being_discovered);
        }

        if (status == BT_ATT_ERRCODE_CONTINUE) {
            return;
        }

        /* status == BT_STATUS_SUCCESS */
        if (p_srv_being_discovered->characteristic_count) {
            _bt_gattc_discover_descriptor(conn_handle, p_srv_being_discovered);

        } else {
            /**< Trigger Service Not Found event to the application. */
            bt_gattc_discovery_result_evt_trigger(true, 0);
            #ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
                bt_gattc_discovery_discover_service_complete();
            #endif
        }
    }
}

static void bt_gattc_discovery_parse_characteristic(bt_gattc_read_by_type_rsp_t *data, bt_gattc_discovery_service_t *service)
{
    bt_gattc_read_by_type_rsp_t *rsp = (bt_gattc_read_by_type_rsp_t *)data;
    bt_gattc_discovery_characteristic_t *char_info;
    uint8_t *attribute_data_list = rsp->att_rsp->attribute_data_list;
    uint8_t num_of_data = (rsp->length - 2) / rsp->att_rsp->length;
    g_gattc_discovery.char_count += num_of_data;
    LOG_MSGID_I(BT_GATTC, "ble_gattc_parse_characteristic : char_num = %d, char_count = %d, real char_count = %d\r\n", 3, num_of_data,
                service->characteristic_count, g_gattc_discovery.char_count);
    if (service->characteristic_count < g_gattc_discovery.char_count) {
        num_of_data = service->characteristic_count - (g_gattc_discovery.char_count - num_of_data);
        g_gattc_discovery.char_count = service->characteristic_count;
    }
    if (rsp->att_rsp->length < 20) {
        /* 16+2 + 1 +1,uuid is 16 bit. */
        for (int i = 0 ; i < num_of_data; i++) {
            char_info = service->charateristics + i + service->char_count_found;
            bt_gattc_discovery_parse_char_command_data(char_info, (rsp->att_rsp->attribute_data_list + i * rsp->att_rsp->length));
            char_info->char_uuid.type = BLE_UUID_TYPE_16BIT;
            memcpy(&(char_info->char_uuid.uuid.uuid16), attribute_data_list + i * rsp->att_rsp->length + 5, 2);
            LOG_MSGID_I(BT_GATTC, "char_index = %d, char_uuid16 = 0x%04x\r\n", 2, i, char_info->char_uuid.uuid.uuid16);
        }
    } else {
        /* uuid is 128 bit. */
        for (int j = 0 ; j < num_of_data; j++) {
            char_info = service->charateristics + j + service->char_count_found;
            bt_gattc_discovery_parse_char_command_data(char_info, (rsp->att_rsp->attribute_data_list + j * rsp->att_rsp->length));
            char_info->char_uuid.type = BLE_UUID_TYPE_128BIT;
            memcpy(&(char_info->char_uuid.uuid.uuid), attribute_data_list + j * rsp->att_rsp->length + 5, 16);
            LOG_MSGID_I(BT_GATTC, "char_uuid1128 is\r\n", 0);
            for (int i = 15; i >= 0; i--) {
                LOG_MSGID_I(BT_GATTC, "0x%02x", 1, char_info->char_uuid.uuid.uuid[i]);
            }
            LOG_MSGID_I(BT_GATTC, "\n", 0);
        }
    }
    service->char_count_found = g_gattc_discovery.char_count;
    LOG_MSGID_I(BT_GATTC, "ble_gattc_parse_characteristic : char_count_found = %d\r\n", 1, service->char_count_found);
}

static void bt_gattc_discovery_parse_char_command_data(bt_gattc_discovery_characteristic_t *char_info, uint8_t *attribute_data_list)
{
    if (attribute_data_list) {
        memcpy(&(char_info->handle), attribute_data_list, 2);
        memcpy(&(char_info->property), attribute_data_list + 2, 1);
        memcpy(&(char_info->value_handle), attribute_data_list + 3, 2);
        LOG_MSGID_I(BT_GATTC, "char_handle = 0x%04x, value_handle = 0x%04x, property = %d\r\n", 3, char_info->handle,
                    char_info->value_handle, char_info->property);
    }
}

static bt_status_t bt_gattc_discovery_all_descriptor(uint16_t conn_handle, bt_gattc_discovery_service_t *service, uint8_t index)
{
    bt_gattc_discovery_characteristic_t *cur_char, *next_char;
    bt_status_t status = 0;
    uint8_t char_num;
    char_num = g_gattc_discovery.char_count;
    g_gattc_discovery.curr_char_ind = index;
    g_gattc_discovery.descr_count = 0;
    if (index <= (char_num - 1)) {
        uint16_t start_handle;
        uint16_t end_handle;
        cur_char = service->charateristics + index;
        start_handle = cur_char->value_handle + 1;
        LOG_MSGID_I(BT_GATTC, "[Gattc]ble_gattc_find_all_descriptor char_index = %d, char_num = %d\r\n", 2, index, char_num);
        if (0 == cur_char->descriptor_count) {
            status = bt_gattc_discovery_all_descriptor(conn_handle, service, index + 1);
            return status;
        }
        /* This one might be the last characteristic in service. */
        if (index == char_num - 1) {
            end_handle = service->end_handle;
        } else {
            next_char = service->charateristics + index + 1;
            end_handle = next_char->handle - 1;
        }
        LOG_MSGID_I(BT_GATTC, "[Gattc]ble_gattc_find_all_descriptor start_handle = 0x%04x, end_handle =0x%04x\r\n", 2, start_handle, end_handle);
        if (start_handle <= end_handle) {
            status = bt_gattc_discovery_descriptor_of_characteristic(conn_handle, start_handle, end_handle);
        } else {
            status = bt_gattc_discovery_all_descriptor(conn_handle, service, index + 1);
        }
    } else {/*all descriptor is complete done */
        LOG_MSGID_I(BT_GATTC, "[Gattc]ble_gattc_find_all_descriptor_of_char done, to discover next service!\r\n", 0);
        bt_gattc_discovery_result_evt_trigger(true, 0);
        #ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
            bt_gattc_discovery_discover_service_complete();
        #endif
    }
    return status;
}

static bt_status_t bt_gattc_discovery_descriptor_of_characteristic(uint16_t conn_handle, uint16_t start_handle, uint16_t end_handle)
{
    bt_status_t status;
    bt_gattc_discover_charc_descriptor_req_t req;
    LOG_MSGID_I(BT_GATTC, "discovery_descriptor_start: start_handle = 0x%04x, end_handle = 0x%04x\r\n", 2, start_handle, end_handle);
    req.opcode = BT_ATT_OPCODE_FIND_INFORMATION_REQUEST;
    req.starting_handle = start_handle;
    req.ending_handle = end_handle;
    status = bt_gattc_discover_charc_descriptor(conn_handle, &req);
    LOG_MSGID_I(BT_GATTC, "dis_descr end: status = %d\r\n", 1, status);
    return status;
}

static void bt_gattc_discovery_descriptors_cnf(bt_status_t status, void *buff)
{
    uint16_t conn_handle;
    bt_gattc_discovery_characteristic_t *chara = NULL;
    bt_gattc_discovery_service_t *service;
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_descriptors_cnf status = %d\r\n", 1, status);

    if (buff == NULL) {
        return;
    }

    service = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
    if (service == NULL) {
        return;
    }
    if (g_gattc_discovery.curr_char_ind < g_gattc_discovery.char_count) {
        chara = service->charateristics + g_gattc_discovery.curr_char_ind;
    }
    g_gattc_discovery.curr_char_ind++;
    if ((status != BT_STATUS_SUCCESS) && (status != BT_ATT_ERRCODE_CONTINUE)) {
        bt_gattc_error_rsp_t *p_event_data = (bt_gattc_error_rsp_t *)buff;

        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
        }
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_descriptors_cnf: fail:index = %d\r\n", 1, g_gattc_discovery.curr_char_ind);
        bt_gattc_discovery_all_descriptor(conn_handle, service, g_gattc_discovery.curr_char_ind);;
    } else {
        bt_gattc_find_info_rsp_t *p_event_data = (bt_gattc_find_info_rsp_t *)buff;

        conn_handle = p_event_data->connection_handle;
        if (g_gattc_discovery.conn_handle != conn_handle) {
            return;
        }
        if ((status == BT_ATT_ERRCODE_CONTINUE) && (chara)) {
            bt_gattc_discovery_parse_descriptor(p_event_data, chara);
        } else if (status == BT_STATUS_SUCCESS) {
            if ((p_event_data->att_rsp) && (chara)) {
                bt_gattc_discovery_parse_descriptor(p_event_data, chara);
            }
            LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_descriptors_cnf: succsee:index = %d\r\n", 1, g_gattc_discovery.curr_char_ind);
            bt_gattc_discovery_all_descriptor(conn_handle, service, g_gattc_discovery.curr_char_ind);
        }
    }
}

static void bt_gattc_discovery_parse_descriptor(bt_gattc_find_info_rsp_t *data, bt_gattc_discovery_characteristic_t *chara)
{
    uint8_t format = 0;
    uint8_t attribute_length = 0;
    uint8_t i;
    bt_gattc_find_info_rsp_t rsp = *data;
    bt_gattc_discovery_descriptor_t *descr_info;
    if (rsp.att_rsp) {
        format = rsp.att_rsp->format;
    }
    if (format == 0x02) {
        attribute_length = 18;
    } else {
        attribute_length = 4;
    }
    uint8_t num_of_attribute = (rsp.length - 2) / attribute_length;
    g_gattc_discovery.descr_count += num_of_attribute;
    LOG_MSGID_I(BT_GATTC, "descr_count = %d, num is = %d, real num is %d\r\n", 3, chara->descriptor_count, num_of_attribute, g_gattc_discovery.descr_count);
    if (chara->descriptor_count < g_gattc_discovery.descr_count) {
        num_of_attribute = chara->descriptor_count - (g_gattc_discovery.descr_count - num_of_attribute);
        g_gattc_discovery.descr_count = chara->descriptor_count;
    }
    for (i = 0; i < num_of_attribute; i++) {
        descr_info = chara->descriptor + i + chara->descr_count_found;
        if (format == 0x02) {
            /* uuid 128 */
            memcpy(&(descr_info->handle), rsp.att_rsp->info_data + i * attribute_length, 2);
            descr_info->descriptor_uuid.type = BLE_UUID_TYPE_128BIT;
            memcpy(&(descr_info->descriptor_uuid.uuid.uuid), rsp.att_rsp->info_data + i * attribute_length + 2, 16);
        } else {
            /* uuid 16 */
            memcpy(&(descr_info->handle), rsp.att_rsp->info_data + i * attribute_length, 2);
            descr_info->descriptor_uuid.type = BLE_UUID_TYPE_16BIT;
            memcpy(&(descr_info->descriptor_uuid.uuid.uuid16), rsp.att_rsp->info_data + i * attribute_length + 2, 2);
            LOG_MSGID_I(BT_GATTC, "descr_uuid16 = 0x%04x\r\n", 1, descr_info->descriptor_uuid.uuid.uuid16);
        }
        LOG_MSGID_I(BT_GATTC, "attribute handle = 0x%04x\r\n", 1, descr_info->handle);
    }
    chara->descr_count_found = g_gattc_discovery.descr_count;
    LOG_MSGID_I(BT_GATTC, "descr_count_found %d\r\n", 1, chara->descr_count_found);
}

/**@brief     Function for handling service discovery completion.
 *
 * @details   This function will be used to determine if there are more services to be discovered,
 *            and if so, initiate the discovery of the next service.
 *
 * @param[in] g_gattc_discovery pointer to the DB discovery structure.
 */
static void bt_gattc_discovery_discover_service_complete(void)
{
    uint32_t err_code;
    g_num_of_discovered++;
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_discover_service_complete: g_num_of_discovered = %d\r\n", 1, g_num_of_discovered);
    /* Check if more services need to be discovered. */
    if (g_num_of_discovered < g_num_of_handlers_reg) {
        bt_gattc_discovery_service_t *p_srv_being_discovered;
        /* Reset the current service information since a new service discovery is about to start. */
        g_gattc_discovery.curr_char_ind = 0;
        g_gattc_discovery.char_count = 0;
        g_gattc_discovery.descr_count = 0;
        /* Initiate discovery of the next service. */
        g_gattc_discovery.curr_srv_ind++;
        p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_discover_service_complete char_count = %d\r\n", 1, p_srv_being_discovered->characteristic_count);
        bt_gattc_discovery_input_buffer_reset(p_srv_being_discovered);
        p_srv_being_discovered->service_uuid.type = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type;
        if (BLE_UUID_TYPE_16BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
            p_srv_being_discovered->service_uuid.uuid.uuid16 = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid16;
            LOG_MSGID_I(BT_GATTC, "Starting discovery of service with UUID16 0x%04x for Connection handle %d\r\n", 2,
                        p_srv_being_discovered->service_uuid.uuid.uuid16, g_gattc_discovery.conn_handle);
        } else if (BLE_UUID_TYPE_32BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
            p_srv_being_discovered->service_uuid.uuid.uuid32 = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid32;
            LOG_MSGID_I(BT_GATTC, "Starting discovery of service with UUID32 0x%08x for Connection handle %d\r\n", 2,
                        p_srv_being_discovered->service_uuid.uuid.uuid32, g_gattc_discovery.conn_handle);
        } else if (BLE_UUID_TYPE_128BIT == g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.type) {
            memcpy(p_srv_being_discovered->service_uuid.uuid.uuid, g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_uuid.uuid.uuid, 16);
            LOG_MSGID_I(BT_GATTC, "Starting discovery of service with UUID128 for Connection handle %d\r\n", 1, g_gattc_discovery.conn_handle);
            for (int i = 15; i >= 0; i--) {
                LOG_MSGID_I(BT_GATTC, "0x%02x", 1, p_srv_being_discovered->service_uuid.uuid.uuid[i]);
            }
            LOG_MSGID_I(BT_GATTC, "\n", 0);
        }
        if (p_srv_being_discovered->is_included_definition) {
            err_code = bt_gattc_discovery_find_included_service(g_gattc_discovery.conn_handle, p_srv_being_discovered->discovery_start_handle, p_srv_being_discovered->discovery_end_handle);
        } else {
            err_code = bt_gattc_discovery_start_discover_service(g_gattc_discovery.conn_handle, &(p_srv_being_discovered->service_uuid));
        }
        if (err_code != BT_STATUS_SUCCESS) {
            bt_gattc_discovery_result_evt_trigger(false, err_code);
#ifndef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
            bt_gattc_discovery_discover_service_complete();
#endif
        }
    } else {
        /* No more service discovery is needed. */
        g_gattc_discovery.discovery_in_progress = false;
        LOG_MSGID_I(BT_GATTC, "all services discovery complete!, discovery_in_progress is %d\r\n", 1, g_gattc_discovery.discovery_in_progress);

        {
            uint8_t i = BT_LE_CONNECTION_NUM;
            bt_handle_t handle;

            while (i != 0) {
                i--;
                if (BT_HANDLE_INVALID != (handle = g_pending_handle_list[i])) {
                    g_pending_handle_list[i] = BT_HANDLE_INVALID;
                    bt_gattc_discovery_start(handle);
                    return;
                }
            }
        }

        if (g_need_restart_discovery) {
            bt_gattc_reset_before_discovery_restart();
            g_retry_discovering_times++;
            bt_gattc_discovery_restart(g_gattc_discovery.conn_handle);
        }
    }
}


/**@brief     Function for triggering a discovery complete or indicating error event to the
 *            application.
 *
 * @details   This function will fetch the event handler based on the UUID of the service being
 *            discovered. (The event handler is registered by the application beforehand).
 *            The error code is added to the pending events together with the event handler.
 *            If no event handler was found, then this function will do nothing.
 *
 * @param[in] is_complete   is to indicate if the service was found at the peer.
 * @param[in] err_code       Error code that should be provided to the application.
 *
 */
static void bt_gattc_discovery_result_evt_trigger(bool is_complete, int32_t err_code)
{
    bt_gattc_discovery_event_handler_t p_evt_handler;
    bt_gattc_discovery_service_t *p_srv_being_discovered;
    bt_gattc_pending_user_events *p_event;
#ifdef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
    bt_gattc_discovery_service_instance_t *p_instance;
#endif

    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;
    p_evt_handler = g_registered_handlers[g_gattc_discovery.curr_srv_ind].event_handler;
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_result_evt_trigger, is_complete= %d, error_code = %d\r\n", 2, is_complete, err_code);

    if (p_evt_handler == NULL) {
        return;
    }

    if (g_pending_usr_evt_index >= BT_GATTC_DISCOVERY_MAX_USERS) {
        /**< Too many events pending. Do nothing. (Ideally this should not happen.) */
        return;
    }

    p_event = &g_pending_user_evts[g_pending_usr_evt_index];

    /**< Insert an event into the pending event list. */
    p_event->event.conn_handle = g_gattc_discovery.conn_handle;
    p_event->event_handler = p_evt_handler;
    p_event->event.last_instance = true;

#ifdef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
    if (NULL != (p_instance = p_srv_being_discovered->service_instance)) {
        /* Multi-instance service discovery */
        LOG_MSGID_I(BT_GATTC, "DiscoveryEvtTrigger instanceProcessed: %d, instanceCount: %d \r\n", 2, p_instance->instance_processed, p_instance->instance_count);
        p_instance->instance_processed++;
        if (p_instance->instance_processed < p_instance->instance_count) {
            p_event->event.last_instance = false;

        } else {
            vPortFree(p_instance);
            p_srv_being_discovered->service_instance = NULL;
        }
    }

    if (is_complete) {
        p_event->event.event_type = BT_GATTC_DISCOVERY_EVENT_COMPLETE;
        p_event->event.params.discovered_db = p_srv_being_discovered;

    } else {
        p_event->event.event_type = BT_GATTC_DISCOVERY_EVENT_FAIL;
        p_event->event.params.error_code = err_code;
    }

    p_event->event_handler(&(p_event->event));
    memset(p_event, 0x00, sizeof(bt_gattc_pending_user_events));

#else

    if (is_complete) {
        p_event->event.event_type = BT_GATTC_DISCOVERY_EVENT_COMPLETE;
        p_event->event.params.discovered_db = p_srv_being_discovered;
        g_num_discovered_success++;

    } else {
        p_event->event.event_type = BT_GATTC_DISCOVERY_EVENT_FAIL;
        p_event->event.params.error_code = err_code;
    }

    g_pending_usr_evt_index++;

    if (g_pending_usr_evt_index == g_num_of_handlers_reg) {
        if ((g_num_discovered_success == g_num_of_handlers_reg) || (g_retry_discovering_times == BT_GATTC_RETRY_DISCOVERY_MAX_TIMES)) {
            /**< All registered modules have pending events. Send all pending events to the user modules. */
            bt_gattc_discovery_pending_user_evts_send();

        } else {
            /* re-start discovery. */
            g_need_restart_discovery = true;

        }
    }
#endif

}

static void bt_gattc_discovery_disconnect_error_evt_trigger(void)
{
    int8_t i = 0;
    bt_gattc_discovery_event_handler_t p_evt_handler;
    bt_gattc_discovery_service_t       *p_srv_being_discovered;
    if (g_num_of_discovered <= g_num_of_handlers_reg) {
        for (i = g_num_of_discovered; i < g_num_of_handlers_reg; i++) {
            p_evt_handler = g_registered_handlers[i].event_handler;
            p_srv_being_discovered = g_registered_handlers[i].service_info;
            bt_gattc_discovery_input_buffer_reset(p_srv_being_discovered);
            /* Insert an event into the pending event list. */
            g_pending_user_evts[i].event.conn_handle = g_gattc_discovery.conn_handle;
            g_pending_user_evts[i].event_handler = p_evt_handler;
            g_pending_user_evts[i].event.event_type = BT_GATTC_DISCOVERY_EVENT_FAIL;
            g_pending_user_evts[i].event.params.error_code = BT_GATTC_DISCOVERY_ERROR_UNEXPECT_DISCONNECT;
        }
        if (i == g_num_of_handlers_reg) {
            /* All registered modules have pending events. Send all pending events to the user modules. */
            bt_gattc_discovery_pending_user_evts_send();
        }
    }
}

/**@brief Function for sending all pending discovery events to the corresponding user modules.
 */
static void bt_gattc_discovery_pending_user_evts_send(void)
{
    uint32_t i;
    LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_pending_user_evts_send: enter\r\n", 0);

    g_retry_discovering_times = 0;
    g_need_restart_discovery = false;

    for (i = 0; i < g_num_of_handlers_reg; i++) {
        /**< Pass the event to the corresponding event handler. */
        if (NULL != g_pending_user_evts[i].event_handler) {
            g_pending_user_evts[i].event_handler(&(g_pending_user_evts[i].event));
            memset(&(g_pending_user_evts[i]), 0x00, sizeof(bt_gattc_pending_user_events));
        }
    }

    if (i == g_num_of_handlers_reg) {
        LOG_MSGID_I(BT_GATTC, "bt_gattc_discovery_pending_user_evts_send: notify all registered users success\r\n", 0);
        g_pending_usr_evt_index = 0;
    }
}

void bt_gattc_discovery_event_response(void)
{
#ifdef APPS_MULTI_INSTANCE_SERVICE_DISCOVERY_ENABLE
    bt_gattc_discovery_service_t *p_srv_being_discovered;
    bt_gattc_discovery_service_instance_t *p_instance;

    /* Multi-instance service discovery */
    p_srv_being_discovered = g_registered_handlers[g_gattc_discovery.curr_srv_ind].service_info;

    if (NULL == (p_instance = p_srv_being_discovered->service_instance)) {
        /* re-discovery inlcuded service in new attribute handge range */
        if (p_srv_being_discovered->is_included_definition && (BT_STATUS_SUCCESS == bt_gattc_discovery_find_included_service(g_gattc_discovery.conn_handle, p_srv_being_discovered->discovery_start_handle, p_srv_being_discovered->discovery_end_handle))) {
            return;
        }
        bt_gattc_discovery_discover_service_complete();
        return;
    }

    LOG_MSGID_I(BT_GATTC, "DiscoveryEvtResponse process:%d, count:%d\r\n", 2, p_instance->instance_processed, p_instance->instance_count);
    if (p_instance->instance_processed < p_instance->instance_count) {

        g_gattc_discovery.curr_char_ind = 0;
        g_gattc_discovery.char_count = 0;
        g_gattc_discovery.descr_count = 0;
        bt_gattc_discovery_input_buffer_service_reset(p_srv_being_discovered);
        p_srv_being_discovered->start_handle = p_instance->start_handle[p_instance->instance_processed];
        p_srv_being_discovered->end_handle = p_instance->end_handle[p_instance->instance_processed];

        LOG_MSGID_I(BT_GATTC, "DiscoveryEvtResponse startHdl:%x, endHdl:%x\r\n", 2, p_srv_being_discovered->start_handle, p_srv_being_discovered->end_handle);

        if (p_srv_being_discovered->characteristic_count) {
            _bt_gattc_discover_characteristic(g_gattc_discovery.conn_handle, p_srv_being_discovered);
            return;
        }
    }
#endif
}

#ifdef BT_ROLE_HANDOVER_WITH_SPP_BLE
#include "bt_role_handover.h"

static bt_status_t bt_gattc_discovery_rho_is_allowed(const bt_bd_addr_t *addr)
{
    //rho cannot be performed at discovery
    if(g_gattc_discovery.discovery_in_progress) {
        LOG_MSGID_I(BT_GATTC,"bt gattc discovery rho discovery is progress",0);
        return BT_STATUS_BUSY;
    }
    return BT_STATUS_SUCCESS;
}
static void bt_gattc_discovery_rho_status_notify(const bt_bd_addr_t *addr, bt_aws_mce_role_t role,
                                           bt_role_handover_event_t event, bt_status_t status)
{
    switch(event) {
        case BT_ROLE_HANDOVER_COMPLETE_IND:
        {
            LOG_MSGID_I(BT_GATTC,"bt gattc discovery rho complete",0);
        }
        break;
        default:
           break;
    }
}
static bt_role_handover_callbacks_t role_cb = {
    &bt_gattc_discovery_rho_is_allowed,
    NULL,
    NULL,
    NULL,
    &bt_gattc_discovery_rho_status_notify
};

static bt_status_t bt_gattc_discovery_rho_init(void)
{
    LOG_MSGID_I(BT_GATTC,"bt gattc discovery rho register",0);
    bt_status_t status = bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_GATT_DISCOVERY,&role_cb);
    if(status != BT_STATUS_SUCCESS) {
        LOG_MSGID_I(BT_GATTC,"bt gattc discovery rho register fail = %02x",1,status);
    }
    return BT_STATUS_SUCCESS;
}
#endif



