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
 
#ifdef MTK_PORT_SERVICE_BT_ENABLE
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

#include "bt_os_layer_api.h"
#include "bt_spp.h"
#include "bt_callback_manager.h"
#include "spp_air.h"
#include "spp_air_interface.h"
#include "syslog.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "bt_uuid.h"
#ifdef MTK_BT_CM_SUPPORT
#include "bt_connection_manager.h"
#endif
log_create_module(SPPAIR, PRINT_LEVEL_INFO);

typedef struct {
    bool in_use;
    spp_air_notify_callback callback;
} spp_air_callback_node_t;

static spp_air_node_t g_spp_air_list;

static spp_air_node_t *g_air_head = NULL;

bt_spp_air_cntx_t air_spp_cntx = {{0}};
static uint8_t const g_air_uuid128_default[16] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
};
static spp_air_callback_node_t spp_air_cb_list[SPP_AIR_SUPPORT_CB_MAX_NUM] = {{0}};

static spp_air_node_t *spp_air_get_head(void);
extern bt_status_t bt_spp_air_sdp_event_callback_register(void);
static uint32_t spp_air_check_node_buffer(uint8_t *data, uint16_t data_size);
static bt_status_t spp_air_check_user(void);
static void spp_air_clear_node_list();
static spp_air_status_t spp_air_get_nvkey_data(uint16_t nvkey, void **data);
static int32_t spp_air_event_callback(spp_air_event_t event_id, void *param);

//static uint32_t spp_air_mutex;

extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);
extern bt_status_t bt_spp_air_sdp_event_update_record(bt_uuid_t *uuid);
#define SPPAIR_MUTEX_LOCK() bt_os_take_stack_mutex()
#define SPPAIR_MUTEX_UNLOCK() bt_os_give_stack_mutex()

#if defined(MTK_AWS_MCE_ENABLE) && defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
#include "bt_role_handover.h"
BT_PACKED (
typedef struct {
    uint16_t server_channel_id;
    uint16_t max_packet_size;
}) spp_air_rho_data_t;

static bt_status_t spp_air_rho_allowed_cb(const bt_bd_addr_t *addr)
{
    //App need call spp_air_get_rx_available() to check if any data in RX queue, if have, need get all data, then to allow RHO
    return BT_STATUS_SUCCESS;
}

static uint8_t spp_air_rho_get_data_length_cb(const bt_bd_addr_t *addr)
{
    if (addr && (true == air_spp_cntx.connected) && 
        (0 == memcmp((air_spp_cntx.bt_addr), addr, BT_BD_ADDR_LEN))) {
        return sizeof(spp_air_rho_data_t);      
    }
    return 0;
}

static bt_status_t spp_air_rho_get_data_cb(const bt_bd_addr_t *addr, void * data)
{
    configASSERT(data);

    if (addr && data && (true == air_spp_cntx.connected) && 
            (0 == memcmp((air_spp_cntx.bt_addr), addr, BT_BD_ADDR_LEN))) {
        spp_air_rho_data_t *rho_data = (spp_air_rho_data_t *)data;
        rho_data->max_packet_size = air_spp_cntx.max_packet_size;
        rho_data->server_channel_id = air_spp_cntx.server_channel_id;
        return BT_STATUS_SUCCESS;
    }
    return BT_STATUS_FAIL;
}

static void spp_air_rho_status_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    switch (event) {
        case BT_ROLE_HANDOVER_COMPLETE_IND: {
            if ((BT_AWS_MCE_ROLE_AGENT == role) && (BT_STATUS_SUCCESS == status)) {
                spp_air_clear_node_list();
                memset(&air_spp_cntx, 0, sizeof(bt_spp_air_cntx_t));
            }
        }
            break;

        default:
            break;
    }
}


static bt_status_t spp_air_rho_update_cb(bt_role_handover_update_info_t *info)
{
    if (info && info->length && (BT_AWS_MCE_ROLE_PARTNER == info->role)) {
        if ((info->length > 0) && (info->data)) {//copy data to context
            spp_air_rho_data_t *rho_data = (spp_air_rho_data_t *)info->data;
            
            memcpy(air_spp_cntx.bt_addr, info->addr, BT_BD_ADDR_LEN);
            air_spp_cntx.connected = true;
            air_spp_cntx.max_packet_size = rho_data->max_packet_size;
            air_spp_cntx.server_channel_id = rho_data->server_channel_id;
            air_spp_cntx.spp_handle = bt_spp_get_handle_by_local_server_id(info->addr, air_spp_cntx.server_channel_id);

            // callback connected to BT port
            spp_air_connect_ind_t ind = {
                .handle = air_spp_cntx.spp_handle,
                .max_packet_length = air_spp_cntx.max_packet_size,
            };

            memcpy(&ind.address, info->addr, sizeof(bt_bd_addr_t));
            spp_air_event_callback(SPP_AIR_CONNECT_IND, &ind);
        } else {
            //error log
            return BT_STATUS_FAIL;
        }
    }
    return BT_STATUS_SUCCESS;
}

bt_role_handover_callbacks_t spp_air_rho_callbacks = {
    .allowed_cb = spp_air_rho_allowed_cb,/*optional if always allowed*/
    .get_len_cb = spp_air_rho_get_data_length_cb,  /*optional if no RHO data to partner*/
    .get_data_cb = spp_air_rho_get_data_cb,   /*optional if no RHO data to partner*/
    .update_cb = spp_air_rho_update_cb,       /*optional if no RHO data to partner*/
    .status_cb = spp_air_rho_status_cb, /*Mandatory for all users.*/
};
#elif defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE) && !defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
#include "bt_role_handover.h"
static bt_status_t spp_air_rho_allowed_cb(const bt_bd_addr_t *addr)
{
    if((true == air_spp_cntx.connected) && (0 == memcmp((air_spp_cntx.bt_addr),addr,BT_BD_ADDR_LEN))) {
        return BT_STATUS_PENDING;
    }
    return BT_STATUS_SUCCESS;
}
static void spp_air_rho_status_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    return;
}
bt_role_handover_callbacks_t spp_air_rho_callbacks = {
    .allowed_cb = spp_air_rho_allowed_cb,/*optional if always allowed*/
    .get_len_cb = NULL,  /*optional if no RHO data to partner*/
    .get_data_cb = NULL,   /*optional if no RHO data to partner*/
    .update_cb = NULL,       /*optional if no RHO data to partner*/
    .status_cb = spp_air_rho_status_cb, /*Mandatory for all users.*/
};
#endif /*__MTK_AWS_MCE_ENABLE__ */

static int32_t spp_air_event_callback(spp_air_event_t event_id, void *param)
{
    uint8_t i = 0;
    int32_t ret = 0;

    //LOG_MSGID_I(SPPAIR, "[iAP2]event-cb  -event_id: %d\n", 1, event_id);
    for (i = 0; i < SPP_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (spp_air_cb_list[i].in_use && spp_air_cb_list[i].callback != NULL) {
            spp_air_cb_list[i].callback(event_id, param);
            ret = 0;
        }
    }
    return ret;
}

static bt_status_t spp_air_event_callback_int(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_MSGID_I(SPPAIR, "spp_air_event_callback_int, msg: 0x%4x, status: 0x%4x\r\n", 2, msg, status); 
    switch (msg) {
        case BT_SPP_CONNECT_IND: {
            bt_spp_connect_ind_t *conn_ind_p = (bt_spp_connect_ind_t *)buff;
            LOG_MSGID_I(SPPAIR, "BT_SPP_CONNECT_IND, handle: 0x%4x, channel_id: %d\r\n", 2, 
                conn_ind_p->handle, conn_ind_p->local_server_id); 

            if (0x15 == conn_ind_p->local_server_id) {
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
                if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
                    bt_status_t sta = bt_spp_connect_response(conn_ind_p->handle, false);
                    LOG_MSGID_I(SPPAIR, "RHO Reject Connected status: 0x%4x\r\n", 1, sta); 
                    return BT_STATUS_SUCCESS;
                }
#endif
                if (true == air_spp_cntx.connected) {
                    bt_status_t sta = bt_spp_connect_response(conn_ind_p->handle, false);
                    LOG_MSGID_E(SPPAIR, "Reject Connected becuase of only support the 1st link, status: 0x%4x\r\n\r\n", 1, sta); 
                    return BT_STATUS_SUCCESS;
                }

                air_spp_cntx.spp_handle = conn_ind_p->handle;
                air_spp_cntx.server_channel_id = conn_ind_p->local_server_id;
                memcpy(air_spp_cntx.bt_addr, conn_ind_p->address, BT_BD_ADDR_LEN);
                bt_spp_connect_response(air_spp_cntx.spp_handle, true);
            } 
        }
        break;
        
        case BT_SPP_CONNECT_CNF: {/* transport connection is established. */
            bt_spp_connect_cnf_t *conn_cnf_p = (bt_spp_connect_cnf_t *)buff;
            if (air_spp_cntx.spp_handle == conn_cnf_p->handle) {
                air_spp_cntx.connected = true;
                air_spp_cntx.server_channel_id = conn_cnf_p->server_id;
                air_spp_cntx.max_packet_size = conn_cnf_p->max_packet_length;
				
                /* notify CM*/
#ifdef MTK_BT_CM_SUPPORT
                bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AIR, air_spp_cntx.bt_addr, BT_CM_PROFILE_SERVICE_STATE_CONNECTED, status);
#endif
                /* notify APP*/
                if (BT_STATUS_SUCCESS == spp_air_check_user()) {
                    spp_air_connect_ind_t connect_param;
                    memset(&connect_param, 0x0, sizeof(spp_air_connect_ind_t));
                    connect_param.handle = air_spp_cntx.spp_handle;
                    connect_param.max_packet_length = air_spp_cntx.max_packet_size;
                    memcpy(&connect_param.address, &air_spp_cntx.bt_addr, BT_BD_ADDR_LEN);
                    spp_air_event_callback(SPP_AIR_CONNECT_IND, (void *)&connect_param);
                }
            }
        }
        break;
        
        case BT_SPP_DISCONNECT_IND: {
            bt_spp_disconnect_ind_t *disc_ind_p = (bt_spp_disconnect_ind_t *)buff;
            if ((true == air_spp_cntx.connected) && (air_spp_cntx.spp_handle == disc_ind_p->handle)) {
             
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE) && !defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
                if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
                    bt_status_t sta = bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_BT_AIR);
                    LOG_MSGID_I(SPPAIR, "RHO disconnected status: 0x%4x\r\n", 1, sta);
                }
#endif
#ifdef MTK_BT_CM_SUPPORT		
                /* notify CM*/
                bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AIR, air_spp_cntx.bt_addr, BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED, status);
#endif
                /* notify APP*/
                if (BT_STATUS_SUCCESS == spp_air_check_user()) {
                    spp_air_disconnect_ind_t disconnect_param;
                    memset(&disconnect_param, 0x0, sizeof(spp_air_disconnect_ind_t));
                    disconnect_param.handle = air_spp_cntx.spp_handle;
                    spp_air_event_callback(SPP_AIR_DISCONNECT_IND, (void *)&disconnect_param);
                }
                spp_air_clear_node_list();
                memset(&air_spp_cntx, 0, sizeof(bt_spp_air_cntx_t));
            } else {
                LOG_MSGID_I(SPPAIR, "BT_SPP_DISCONNECT_IND, Wrong spp handle!\r\n", 0);
            }
        }
        break;

        case BT_SPP_DATA_RECEIVED_IND: {
            bt_spp_data_received_ind_t *data_ind_p = (bt_spp_data_received_ind_t *)buff;
            if ((true == air_spp_cntx.connected) && (air_spp_cntx.spp_handle == data_ind_p->handle)) {
                if (BT_STATUS_SUCCESS == spp_air_check_user()) {
                    spp_air_data_received_ind_t recieve_param;
                    memset(&recieve_param, 0x0, sizeof(spp_air_data_received_ind_t));
                    recieve_param.handle = air_spp_cntx.spp_handle;
                    bt_spp_hold_data(data_ind_p->packet);
                    spp_air_add_node(spp_air_get_head(), data_ind_p->packet, data_ind_p->packet_length);
                    spp_air_event_callback(SPP_AIR_RECIEVED_DATA_IND, (void *)&recieve_param);
                }
            } else {
                LOG_MSGID_I(SPPAIR, "BT_SPP_DATA_RECEIVED_IND, Wrong spp handle!\r\n", 0);
            }
        }
        break;
        
        case BT_SPP_READY_TO_SEND_IND: {
            bt_spp_ready_to_send_ind_t *ready_send_p = (bt_spp_ready_to_send_ind_t *)buff;
            if ((true == air_spp_cntx.connected) && (air_spp_cntx.spp_handle == ready_send_p->handle)) {
                if (BT_STATUS_SUCCESS == spp_air_check_user()) {
                    spp_air_ready_to_send_ind_t ready_send;
                    memset(&ready_send, 0x0, sizeof(spp_air_ready_to_send_ind_t));
                    ready_send.handle = air_spp_cntx.spp_handle;
                    spp_air_event_callback(SPP_AIR_READY_TO_SEND_IND, (void *)&ready_send);
                }
            }
        }
        break;

        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}

static bt_status_t spp_air_check_user(void)
{
    uint8_t i = 0;
    bt_status_t status = 0;
    LOG_MSGID_I(SPPAIR, "spp_air_check_user \r\n", 0);

    for (i = 0; i < SPP_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (spp_air_cb_list[i].in_use) {
            return status;
        }
    }
    if (i == SPP_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(SPPAIR, "not find any users existed!\r\n", 0);
        status = SPP_AIR_STATUS_FAIL;
    }
    return status;
}

static void spp_air_connection_status_notify(spp_air_notify_callback callback)
{
    if (true == air_spp_cntx.connected) {
        spp_air_connect_ind_t connect_param;
        memset(&connect_param, 0x0, sizeof(spp_air_connect_ind_t));
        connect_param.handle = air_spp_cntx.spp_handle;
        connect_param.max_packet_length = air_spp_cntx.max_packet_size;
        memcpy(&connect_param.address, &air_spp_cntx.bt_addr, BT_BD_ADDR_LEN);
        callback(SPP_AIR_CONNECT_IND, (void *)&connect_param);
    }
}

#ifdef MTK_BT_CM_SUPPORT
bt_status_t  bt_spp_air_cm_callback_handler(bt_cm_profile_service_handle_t type, void *data)
{
    LOG_MSGID_I(SPPAIR, "cm_callback_handler type: 0x%02x", 1, type);

    switch (type) {
        case BT_CM_PROFILE_SERVICE_HANDLE_POWER_ON: {
            memset(&air_spp_cntx, 0, sizeof(bt_spp_air_cntx_t));
            if (NULL == g_air_head) {
                g_air_head = spp_air_create_list();
                if (NULL == g_air_head) {
                    LOG_MSGID_I(SPPAIR, "CMCB, spp_air_create_list fail, because of no memory\r\n", 0);
                }
            }
        }
        break;
        case BT_CM_PROFILE_SERVICE_HANDLE_POWER_OFF: {
            spp_air_clear_node_list();
            memset(&air_spp_cntx, 0, sizeof(bt_spp_air_cntx_t));
        }
        break;

        case BT_CM_PROFILE_SERVICE_HANDLE_CONNECT: {
            SPPAIR_MUTEX_LOCK();
            const bt_bd_addr_t *address = (const bt_bd_addr_t *)data;
            bt_uuid_t uuid;
            if (false == air_spp_cntx.connected) {
                bt_status_t ret;
                if(SPP_AIR_STATUS_SUCCESS == spp_air_get_uuid((uint8_t *)&uuid)){
                    ret = bt_spp_connect(&air_spp_cntx.spp_handle, address, (const uint8_t *)&uuid);
                } else {
                    ret = bt_spp_connect(&air_spp_cntx.spp_handle, address, (const uint8_t *)(&g_air_uuid128_default));
                }
                if (BT_STATUS_SUCCESS == ret) {
                    memcpy(air_spp_cntx.bt_addr, address, sizeof(bt_bd_addr_t));
                }
            } else {
                LOG_MSGID_I(SPPAIR, "CMCB, spp air connect fail due to was connected! \r\n", 0);
            }
            SPPAIR_MUTEX_UNLOCK();
        }
        break;

        case BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT: {
            SPPAIR_MUTEX_LOCK();
            bt_bd_addr_t *address = (bt_bd_addr_t *)data;
            if ((0 == memcmp((air_spp_cntx.bt_addr), address, BT_BD_ADDR_LEN))
            && (true == air_spp_cntx.connected)
            && (SPP_AIR_INVALID_HANDLE != air_spp_cntx.spp_handle)) {
                LOG_MSGID_I(SPPAIR, "CMCB, spp air disconnect\r\n", 0);
                bt_spp_disconnect(air_spp_cntx.spp_handle);
            }
            SPPAIR_MUTEX_UNLOCK();
        }
        break;

        default:
        break;
    }
    return BT_STATUS_SUCCESS;
}
#endif

void bt_spp_air_main(void)
{
    spp_air_status_t result = 0;
    memset(&air_spp_cntx, 0, sizeof(bt_spp_air_cntx_t));
    g_air_head = spp_air_create_list();
    if (NULL == g_air_head) {
        LOG_MSGID_I(SPPAIR, "spp_air_create_list fail, because of no memory\r\n", 0);
    }

    /** add serviceRecord. */
    bt_spp_air_sdp_event_callback_register();
    result = bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SPP, (void*)spp_air_event_callback_int);
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
    result += bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_BT_AIR, &spp_air_rho_callbacks);
#endif
#ifdef MTK_BT_CM_SUPPORT
    bt_cm_profile_service_register(BT_CM_PROFILE_SERVICE_AIR, bt_spp_air_cm_callback_handler);
#endif
    LOG_MSGID_I(SPPAIR, "bt_spp_air_main, result: 0x%4x\r\n", 1, result);
}

spp_air_status_t spp_air_init(spp_air_notify_callback callback)
{
    uint8_t i = 0;
    spp_air_status_t status = 0;
    LOG_MSGID_I(SPPAIR, "spp_air_init callback %x\r\n", 1, callback);

    for (i = 0; i < SPP_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (!spp_air_cb_list[i].in_use) {
            spp_air_cb_list[i].callback = callback;
            spp_air_cb_list[i].in_use = true;
            spp_air_connection_status_notify(callback);
            break;
        }
    }
    if (i == SPP_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(SPPAIR, "all are in use, please extend the value of SPP_AIR_SUPPORT_CB_MAX_NUM\r\n", 0);
        status = SPP_AIR_STATUS_FAIL;
    }  
    return status;
}

spp_air_status_t spp_air_deinit(spp_air_notify_callback callback)
{
    uint8_t i = 0;
    spp_air_status_t status = 0;

    LOG_MSGID_I(SPPAIR, "spp_air_deinit callback %x\r\n", 1, callback);
    for (i = 0; i < SPP_AIR_SUPPORT_CB_MAX_NUM; i++) {
        if (spp_air_cb_list[i].in_use && spp_air_cb_list[i].callback == callback) {
            spp_air_cb_list[i].callback = NULL;
            spp_air_cb_list[i].in_use = false;
            break;
        }
    }

    if (i == SPP_AIR_SUPPORT_CB_MAX_NUM) {
        LOG_MSGID_I(SPPAIR, "spp_air_deinit delete fail, because of not find the callback\r\n", 0);
        status = SPP_AIR_STATUS_FAIL;
    }

#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE) && !defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
    if ((SPP_AIR_STATUS_FAIL == spp_air_check_user()) &&
        (SPP_AIR_INVALID_HANDLE != air_spp_cntx.spp_handle)) {
        bt_spp_disconnect(air_spp_cntx.spp_handle);
    }
#endif
    return status;
}

uint32_t spp_air_write_data(uint32_t handle, uint8_t *data, uint16_t data_size)
{
    LOG_MSGID_I(SPPAIR, "spp_air_write_data take mutex\r\n", 0);
    SPPAIR_MUTEX_LOCK();
    LOG_MSGID_I(SPPAIR, "spp_air_write_data incoming\r\n", 0);
    bt_status_t status;
    uint32_t send_size = 0;

    if ((handle != SPP_AIR_INVALID_HANDLE) && (handle == air_spp_cntx.spp_handle)) {
        if (air_spp_cntx.max_packet_size < data_size) {
            send_size = air_spp_cntx.max_packet_size;
        } else {
            send_size = data_size;
        }
        status = bt_spp_send(handle, data, send_size);
        if (BT_STATUS_SUCCESS == status) {
            SPPAIR_MUTEX_UNLOCK();
            LOG_MSGID_I(SPPAIR, "spp_air_write_data: send_size[%d]\r\n", 1, send_size);
            return send_size;
        }
    }
    SPPAIR_MUTEX_UNLOCK();
    LOG_MSGID_I(SPPAIR, "spp_air_send_data, fail!\r\n", 0);
    return 0;
}

uint32_t spp_air_read_data(uint32_t handle, uint8_t *data, uint16_t data_size)
{
    LOG_MSGID_I(SPPAIR, "spp_air_read_data, handle:0x%4x, data: 0x%4x, data_size: %d\r\n", 3, handle, data, data_size); 
    SPPAIR_MUTEX_LOCK();
    uint32_t read_length = 0;

    if ((handle != SPP_AIR_INVALID_HANDLE) && (handle == air_spp_cntx.spp_handle)) {
        read_length = spp_air_check_node_buffer(data, data_size);
        SPPAIR_MUTEX_UNLOCK();
        return read_length;
    } 
    SPPAIR_MUTEX_UNLOCK();
    return 0;
}

uint32_t spp_air_get_rx_available(uint32_t handle)
{
    SPPAIR_MUTEX_LOCK();
    uint32_t rx_remain_len = 0;

    if ((handle != SPP_AIR_INVALID_HANDLE) && (handle == air_spp_cntx.spp_handle)) {
        uint32_t count = spp_air_get_node_length(g_air_head);
        if (0 != spp_air_get_node_length(g_air_head)) {//list is null
            spp_air_node_t *node = spp_air_find_node_by_index(g_air_head, count);
            rx_remain_len = node->packet_length;
            SPPAIR_MUTEX_UNLOCK();
            return rx_remain_len;
        }
    }
    SPPAIR_MUTEX_UNLOCK();
    return 0;
}

static bool spp_air_uuid_is_valid(const uint8_t *uuid, uint16_t uuid_len)
{
    bool ret = false;
    uint8_t i = 0;
    uint8_t const g_air_uuid[16] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
    };
        
    if (uuid_len == sizeof(g_air_uuid)) {
        for (i = 0; i < sizeof(g_air_uuid); i++) {
            if (uuid[i] != g_air_uuid[i])
                break;
        }
        if (i == sizeof(g_air_uuid)) {
            //LOG_MSGID_I(SPPAIR, "[IAP2] spp_air_uuid_is_valid, valid\r\n", 0);
            return true;
        }
    }
    return ret;
}

bt_status_t spp_air_connect_int (
        uint32_t *handle,
        const bt_bd_addr_t *address,
        const uint8_t *uuid128)
{
    bt_status_t status = 0;
    uint8_t addr[6] = {0};

    if ((0 == memcmp((air_spp_cntx.bt_addr), &addr, 6)) ||
        (0 == memcmp(address, &(air_spp_cntx.bt_addr), 6))) {
        status = bt_spp_connect(&air_spp_cntx.spp_handle, address, uuid128);
        if (BT_STATUS_SUCCESS == status) {
            *handle = air_spp_cntx.spp_handle;
        } else {
            *handle = 0;
        }
    } else {
        status = BT_STATUS_FAIL;
        LOG_MSGID_I(SPPAIR, "spp_air_connect_int fail, not first conencted and not reconnect!\r\n", 0);
    }
    LOG_MSGID_I(SPPAIR, "spp_air_connect_int, status: 0x%x\r\n", 1, status);
    return status;
}

spp_air_status_t spp_air_connect(uint32_t *handle, const bt_bd_addr_t *address, const uint8_t *uuid128)
{
    spp_air_status_t status = 0;
    configASSERT(address && "NULL address");

#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
    if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
        LOG_MSGID_I(SPPAIR, "RHO ongoing, please don't connect spp!\r\n", 0);
        return SPP_AIR_STATUS_FAIL;
    }
#endif

    /** using SPP as Transport. */
    if (NULL == uuid128) {
        status = spp_air_connect_int(handle, address, (const uint8_t *)(&g_air_uuid128_default));
    } else if (true == spp_air_uuid_is_valid(uuid128, 16)) {
        status = spp_air_connect_int(handle, address, uuid128);
    } else {
        LOG_MSGID_I(SPPAIR, "spp_air_connect, invlaid uuid!\r\n", 0);
        return SPP_AIR_STATUS_INVALID_UUID;
    }
    LOG_MSGID_I(SPPAIR, "spp_air_connect, status: 0x%x\r\n", 1, status);
    if (0 != status) {
        status = SPP_AIR_STATUS_FAIL;
    }
    return status;
}

spp_air_status_t spp_air_disconnect(uint32_t handle)
{
    spp_air_status_t status = 0;

    LOG_MSGID_I(SPPAIR, "spp_air_disconnect\r\n", 0);
    if ((handle != SPP_AIR_INVALID_HANDLE) && (handle == air_spp_cntx.spp_handle)) {
        status = bt_spp_disconnect(handle);
    } else {
        return SPP_AIR_STATUS_INVALID_HANDLE;
    }
    
    if (0 != status) {
        status = SPP_AIR_STATUS_FAIL;
    }
    return status;
}

static spp_air_status_t spp_air_get_nvkey_data(uint16_t nvkey, void **data)
{
    nvkey_status_t nvStatus = NVKEY_STATUS_OK;
    spp_air_status_t status = SPP_AIR_STATUS_SUCCESS;
    uint32_t size;

    do{
        nvStatus = nvkey_data_item_length(nvkey, &size);
        if(NVKEY_STATUS_OK != nvStatus){
            LOG_MSGID_E(SPPAIR, "read nvkey lenght fail, NVKEY: 0x%x, ret %d", 2, nvkey, nvStatus);
            status = SPP_AIR_STATUS_FAIL;
            break;
        }

        *data = (uint8_t *)pvPortMalloc(size);
        if(NULL == *data){
            LOG_MSGID_E(SPPAIR, "alloc memory fail, size: %d", 1, size);
            status = SPP_AIR_STATUS_OUT_OF_MEMORY;
            break;
        }

        nvStatus = nvkey_read_data(nvkey, (uint8_t *)*data, &size);
        if(NVKEY_STATUS_OK != nvStatus){
            LOG_MSGID_E(SPPAIR, "read nvkey fail, NVKEY: 0x04%x, status: %d", 2, nvkey, nvStatus);
            status = SPP_AIR_STATUS_FAIL;
            break;
        }
    }while(0);

    return status;
}
//********************************************************Node Function****************************************************************************//

/**
 * @brief          This function is for create a list.
 * @param[in]  void.
 * @return       the head of the list.
 */


spp_air_node_t *spp_air_create_list(void)
{
    spp_air_node_t *head;
    head = &g_spp_air_list;
    //head = (spp_air_node_t *)pvPortMalloc(sizeof(spp_air_node_t));
    if (head) {
        head->packet = NULL;
        head->packet_length = 0;
        head->next = NULL;
    }
    return head;
}

/**
 * @brief          This function is for add a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       void.
 */
void spp_air_add_node(spp_air_node_t *head, uint8_t *packet, uint16_t packet_length)
{//insert start
    SPPAIR_MUTEX_LOCK();
    spp_air_node_t *p = (spp_air_node_t*)pvPortMalloc(sizeof(spp_air_node_t));
    if (p) {
        p->packet = packet;
        p->packet_length = packet_length;
        p->next = head->next;
        head->next = p;
    }
    SPPAIR_MUTEX_UNLOCK();
}

/**
 * @brief          This function is for get the list length.
 * @param[in]  head      is the head of the list.
 * @return       the length of the list.
 */
uint32_t spp_air_get_node_length(spp_air_node_t *head)//get list length
{
    uint32_t n=0;
    spp_air_node_t *p;
    p=head->next;
    while(p) {
       n++;
       p=p->next;
    }
    return n;
}

/**
 * @brief          This function is for delete a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       delete success or not.
 */
uint32_t spp_air_delete_node(spp_air_node_t * head, uint8_t *packet, uint16_t packet_length)
{
    spp_air_node_t * p;
    spp_air_node_t * q;
    for(p = head; ((NULL != p) && (NULL != p->next)); p = p->next) {
        if(p->next->packet == packet) {
            q = p->next;
            p->next = q->next;
            LOG_MSGID_I(SPPAIR, "spp_air_delete_node, node: 0x%4x, packet: 0x%4x, pak_len: %d\r\n", 3, q, q->packet, q->packet_length); 
            vPortFree(q);
            return 1;
        }
    }
    return 0; 
}

/**
 * @brief          This function is for find a node into list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       the exit node or not.
 */
uint32_t spp_air_find_node(spp_air_node_t *head, uint8_t *packet, uint16_t packet_length)
{
    SPPAIR_MUTEX_LOCK();
    spp_air_node_t *p = head->next;
    while (p) {
        if(p->packet == packet) {
            SPPAIR_MUTEX_UNLOCK();
            return 1;
        }
        p = p->next;
    }
    SPPAIR_MUTEX_UNLOCK();
    return 0;
}

/**
 * @brief          This function is for add a node by the index of the list.
 * @param[in]  head      is the head of the list.
 * @param[in]  packet   is the data point.
 * @param[in]  packet_length       is the data length.
 * @return       the node.
 */
spp_air_node_t *spp_air_find_node_by_index(spp_air_node_t * head, int index)
{
    int i;
    spp_air_node_t * p;
    p = head->next;
    for(i = 0; i < (index - 1); i++) {
        if (NULL == p) {
            break;
        }
        p = p->next;
    }

    if (NULL == p) {
        LOG_MSGID_I(SPPAIR, "spp_air_find_node_by_index, node not find!\r\n", 0);
        return NULL;
    }

    LOG_MSGID_I(SPPAIR, "spp_air_find_node_by_index, find node: 0x%4x, packet: 0x%4x, pak_len: %d\r\n", 3, p, p->packet, p->packet_length); 
    return p;
}

static spp_air_node_t *spp_air_get_head(void)
{
    return g_air_head;
}

uint32_t spp_air_check_node_buffer(uint8_t *data, uint16_t data_size)
{
    uint32_t r_size = 0;
    uint32_t count = spp_air_get_node_length(g_air_head);
    
    LOG_MSGID_I(SPPAIR, "spp_air_check_node_buffer, count: %d\r\n", 1, count);
    if (count != 0 ) {
        uint8_t *pak;
        uint16_t pak_len;
        
        spp_air_node_t *node = spp_air_find_node_by_index(g_air_head, count);
        if (node) {
            if (data_size > node->packet_length) {
                memcpy(data, node->packet, node->packet_length);
                r_size = node->packet_length;
            } else {
                configASSERT(0 && "enlarge the read buffer");
            }

            pak = node->packet;
            pak_len = node->packet_length;

            LOG_MSGID_I(SPPAIR, "spp_air_check_node_buffer, pak: 0x%4x, pak_len: %d\r\n", 2, pak, pak_len);
            spp_air_delete_node(g_air_head, node->packet, node->packet_length); //deleteElem must before bt_spp_release_data, or the sequence of the packets will be wrong
            if (pak || (pak_len != 0)) {
                bt_spp_release_data(pak);
            }
        }
    }
    return r_size;
}

void spp_air_clear_node_list()
{
    SPPAIR_MUTEX_LOCK();
    uint32_t count = spp_air_get_node_length(g_air_head);
    while(count > 0){
        uint8_t *pak;
        uint16_t pak_len;

        spp_air_node_t *node = spp_air_find_node_by_index(g_air_head, count);
        pak = node->packet;
        pak_len = node->packet_length;
        spp_air_delete_node(g_air_head, node->packet, node->packet_length);

        if (pak || (pak_len != 0)) {
            bt_spp_release_data(pak);
        }
        count --;
    }
    SPPAIR_MUTEX_UNLOCK();
}

spp_air_status_t spp_air_set_uuid_index(uint8_t index)
{
    spp_air_status_t status = SPP_AIR_STATUS_SUCCESS;
    nvkey_status_t nvStatus = NVKEY_STATUS_OK;
    uint8_t *uuidCount = NULL;
    bt_uuid_t *uuidData = NULL;

    LOG_MSGID_I(SPPAIR, "set uuid index %d", 1, index);

    do{
#if defined(MTK_AWS_MCE_ENABLE) && defined (SUPPORT_ROLE_HANDOVER_SERVICE)
        if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()){
            LOG_MSGID_W(SPPAIR, "rho is running, skip", 0);
            status = SPP_AIR_STATUS_FAIL;
            break;
        }
#endif

        status = spp_air_get_nvkey_data(NVKEYID_BT_AIR_SPP_UUID_DATA, (void *)&uuidCount);
        if(SPP_AIR_STATUS_SUCCESS != status){
            break;
        }

        if(*uuidCount <= index){
            LOG_MSGID_E(SPPAIR, "invalid uuid index: %d, it should be less than %d", 2, index, *uuidCount);
            status = SPP_AIR_STATUS_FAIL;
            break;
        }

        nvStatus = nvkey_write_data(NVKEYID_BT_AIR_SPP_UUID_INDEX, &index, sizeof(uint8_t));
        if(NVKEY_STATUS_OK != nvStatus){
            LOG_MSGID_E(SPPAIR, "write nvkey fail, NVKEY: 0x04%x, status: %d", 2, NVKEYID_BT_AIR_SPP_UUID_INDEX, nvStatus);
            status = SPP_AIR_STATUS_FAIL;
            break;
        }
        uuidData = (bt_uuid_t *)(uuidCount+sizeof(uint8_t));

        if(BT_STATUS_FAIL != bt_spp_air_sdp_event_update_record((bt_uuid_t *)(uuidData+index))){
            status = SPP_AIR_STATUS_FAIL;
            break;
        }

    }while(0);

    if(uuidCount){
        vPortFree(uuidCount);
    }

    return status;
}

spp_air_status_t spp_air_get_uuid(uint8_t *uuid128)
{
    spp_air_status_t status = SPP_AIR_STATUS_SUCCESS;
    uint8_t *uuidIdx = NULL;
    uint8_t *uuidCount = NULL;
    bt_uuid_t *uuidData = NULL;

    do{

        if(uuid128 == NULL){
            status = SPP_AIR_STATUS_FAIL;
            break;
        }

        //read uuid index
        status = spp_air_get_nvkey_data(NVKEYID_BT_AIR_SPP_UUID_INDEX,  (void *)&uuidIdx);
        if(SPP_AIR_STATUS_SUCCESS != status){
            break;
        }


        //read uuid data
        status = spp_air_get_nvkey_data(NVKEYID_BT_AIR_SPP_UUID_DATA,  (void *)&uuidCount);
        if(SPP_AIR_STATUS_SUCCESS != status){
            break;
        }

        LOG_MSGID_I(SPPAIR, "target uuid index: %d, total uuid count: %d", 2, *uuidIdx, *uuidCount);
        configASSERT(*uuidIdx < *uuidCount);
        uuidData = (bt_uuid_t *)(uuidCount+sizeof(uint8_t));
        memcpy(uuid128, (void *)(uuidData+*uuidIdx), sizeof(bt_uuid_t));

    }while(0);

    if(uuidIdx){
        vPortFree(uuidIdx);
    }

    if(uuidCount){
        vPortFree(uuidCount);
    }

    return status;
}

#endif



