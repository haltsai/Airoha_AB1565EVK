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

#include "bt_type.h"
#include "bt_sdp.h"
#include "bt_spp.h"
#include "bt_system.h"
#include "syslog.h"
#include "bt_callback_manager.h"
#include "bt_fast_pair_utility.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif
#include "bt_connection_manager.h"

/* Weak symbol declaration */
#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_fast_pair_spp_event_handle_cb=_default_bt_fast_pair_spp_event_handle_cb")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_fast_pair_spp_event_handle_cb = default_bt_fast_pair_spp_event_handle_cb
#else
#error "Unsupported Platform"
#endif

#define bt_fast_pair_log(_message, arg_cnt,...) LOG_MSGID_I(BT_FAST_PAIR, _message, arg_cnt, ##__VA_ARGS__)

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#define FAST_PAIR_SPP_RHO_PENDING                   (0x01)
#endif
typedef uint8_t     bt_fast_pair_spp_flags_t;

#define FAST_PAIR_SPP_UUID                          0xDF,0x21,0xFE,0x2C,0x25,0x15,0x4F,0xDB,0x88,0x86,0xF1,0x2C,0x4D,0x67,0x92,0x7C
#define FAST_PAIR_SPP_SERVER_ID                     (0x17)

#define FAST_PAIR_SPP_MAXIMUM                       (0x03)

typedef struct {
    uint32_t        spp_handle;
    bt_bd_addr_t    address;
} bt_fast_pair_spp_cnt_t;

static const uint8_t g_bt_fast_pair_spp_service_class_id[] =
{
    BT_SPP_SDP_ATTRIBUTE_UUID_LENGTH,
    BT_SPP_SDP_ATTRIBUTE_UUID(FAST_PAIR_SPP_UUID)
};

static const uint8_t g_bt_fast_pair_spp_protocol_descriptor_list[] =
{
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESCRIPTOR(FAST_PAIR_SPP_SERVER_ID)
};

static const uint8_t g_bt_fast_pair_spp_browse_group[] =
{
    BT_SPP_SDP_ATTRIBUTE_PUBLIC_BROWSE_GROUP
};

static const uint8_t g_bt_fast_pair_spp_language[] =
{
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE
};

static const uint8_t g_bt_fast_pair_spp_service_name[] =
{
    BT_SPP_SDP_ATTRIBUTE_SIZE_OF_SERVICE_NAME(10),
    'B', 'T', 'F', 'A', 'S', 'T', 'P', 'A', 'I', 'R'
};

static const bt_sdps_attribute_t g_bt_fast_pair_spp_sdp_attributes[] =
{
    /* Service Class ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_CLASS_ID_LIST(g_bt_fast_pair_spp_service_class_id),
    /* Protocol Descriptor List attribute */
    BT_SPP_SDP_ATTRIBUTE_PROTOCOL_DESC_LIST(g_bt_fast_pair_spp_protocol_descriptor_list),
    /* Public Browse Group Service */
    BT_SPP_SDP_ATTRIBUTE_BROWSE_GROUP_LIST(g_bt_fast_pair_spp_browse_group),
    /* Language Base ID List attribute */
    BT_SPP_SDP_ATTRIBUTE_LANGUAGE_BASE_LIST(g_bt_fast_pair_spp_language),
    /* Serial Port Profile Service Name */
    BT_SPP_SDP_ATTRIBUTE_SERVICE_NAME(g_bt_fast_pair_spp_service_name)
};

static const bt_sdps_record_t g_bt_fast_pair_spp_sdp_record =
{
    .attribute_list_length = sizeof(g_bt_fast_pair_spp_sdp_attributes),
    .attribute_list = g_bt_fast_pair_spp_sdp_attributes
};

static bt_fast_pair_spp_cnt_t g_bt_fast_pair_spp_cnt_t[FAST_PAIR_SPP_MAXIMUM];
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
static bt_fast_pair_spp_flags_t g_bt_fast_pair_spp_flags = 0;
#endif

void                default_bt_fast_pair_spp_event_handle_cb(bt_fast_pair_spp_event_t evt, bt_status_t status,
        bt_bd_addr_t *addr, uint8_t* data, uint16_t data_length)
{
    bt_fast_pair_log("fast pair in default event handle !!!", 0);
}

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
static bt_status_t  bt_fast_pair_spp_aws_mce_rho_is_allowed(const bt_bd_addr_t *addr)
{
#ifndef BT_ROLE_HANDOVER_WITH_SPP_BLE
    for (uint8_t i = 0; i < FAST_PAIR_SPP_MAXIMUM; i++) {
        if (0 != g_bt_fast_pair_spp_cnt_t[i].spp_handle) {
            g_bt_fast_pair_spp_flags |= FAST_PAIR_SPP_RHO_PENDING;
            return BT_STATUS_PENDING;
        }
    }
#endif
    return BT_STATUS_SUCCESS;
}

static void         bt_fast_pair_spp_aws_mce_rho_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role,
        bt_role_handover_event_t event, bt_status_t status)
{
    bt_fast_pair_log("[BT_FAST_PAIR][I] Rho role:0x%x, event:0x%x, status:0x%x, addr:0x%x:%x:%x:%x:%x:%x", 9, role, event, status,
        (*addr)[0], (*addr)[1], (*addr)[2], (*addr)[3], (*addr)[4], (*addr)[5]);
	switch (event) {
        case BT_ROLE_HANDOVER_PREPARE_REQ_IND: {
        #ifndef BT_ROLE_HANDOVER_WITH_SPP_BLE
            uint8_t i = 0;
            for (i = 0; i < FAST_PAIR_SPP_MAXIMUM; i++) {
                if (0 != g_bt_fast_pair_spp_cnt_t[i].spp_handle) {
                    bt_fast_pair_spp_disconnect(NULL);
                    g_bt_fast_pair_spp_flags |= FAST_PAIR_SPP_RHO_PENDING;
                    bt_fast_pair_log("fast pair spp rho pending to disconnect", 0);
                    break;
                }
            }
            if (FAST_PAIR_SPP_MAXIMUM == i && (g_bt_fast_pair_spp_flags & FAST_PAIR_SPP_RHO_PENDING)) {
                g_bt_fast_pair_spp_flags &= (~FAST_PAIR_SPP_RHO_PENDING);
                bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_FAST_PAIR);
            }
        #endif
        }
            break;
        case BT_ROLE_HANDOVER_COMPLETE_IND:
            g_bt_fast_pair_spp_flags &= (~FAST_PAIR_SPP_RHO_PENDING);
            bt_fast_pair_log("[BT_FAST_PAIR][I] Rho complete, role 0x%x, status : %x", 2, role, status);
            if (BT_STATUS_SUCCESS != status) {
                break;
            }
#ifdef BT_ROLE_HANDOVER_WITH_SPP_BLE
            for (uint8_t i = 0; i < FAST_PAIR_SPP_MAXIMUM; i++) {
                g_bt_fast_pair_spp_cnt_t[i].spp_handle = 0;
            }
            if (BT_AWS_MCE_ROLE_PARTNER == role) {
                bt_bd_addr_t devices_list[FAST_PAIR_SPP_MAXIMUM];
                uint32_t device_list_count = FAST_PAIR_SPP_MAXIMUM;
#ifdef MTK_BT_CM_SUPPORT
                device_list_count = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_FAST_PAIR),
                                                                devices_list, device_list_count);
#else
                memcpy(devices_list[0], addr, sizeof(bt_bd_addr_t));
                device_list_count = 1;
#endif
                for (uint8_t i = 0; i < device_list_count; i++) {
                    memcpy(&(g_bt_fast_pair_spp_cnt_t[i].address), devices_list[i], sizeof(bt_bd_addr_t));
                    g_bt_fast_pair_spp_cnt_t[i].spp_handle = bt_spp_get_handle_by_local_server_id((const bt_bd_addr_t *)&devices_list[i], FAST_PAIR_SPP_SERVER_ID);
                    bt_fast_pair_log("[BT_FAST_PAIR][I] RHO complete, get spp_handle: 0x%x", 1, g_bt_fast_pair_spp_cnt_t[i].spp_handle);
                }

            }
#endif
	        break;
	}
}

static bt_status_t  bt_fast_pair_spp_aws_mce_rho_update(bt_role_handover_update_info_t *info)
{
    // consider on fast pair spp firstly, TBD for multiple fast pair spp link.
     bt_fast_pair_log("[BT_FAST_PAIR][I] Spp RHO update current role 0x%x", 1, info->role);

    return BT_STATUS_SUCCESS;
}

static void         bt_fast_pair_spp_aws_mce_init()
{
    bt_role_handover_callbacks_t bt_fast_pair_rho_callback_sets = {
        .allowed_cb =   &bt_fast_pair_spp_aws_mce_rho_is_allowed,
        .get_len_cb =   NULL,
        .get_data_cb =  NULL,
        .update_cb =    &bt_fast_pair_spp_aws_mce_rho_update,
        .status_cb =    &bt_fast_pair_spp_aws_mce_rho_status_callback
    };
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_FAST_PAIR, &bt_fast_pair_rho_callback_sets);
}

static void         bt_fast_pair_spp_aws_mce_deinit()
{
    bt_role_handover_deregister_callbacks(BT_ROLE_HANDOVER_MODULE_FAST_PAIR);
}
#endif

static bt_fast_pair_spp_cnt_t *
                    bt_fast_pair_spp_find_by_handle(uint32_t spp_handle)
{
    for (uint8_t i = 0; i < FAST_PAIR_SPP_MAXIMUM; i++) {
        if (spp_handle == g_bt_fast_pair_spp_cnt_t[i].spp_handle) {
            return &g_bt_fast_pair_spp_cnt_t[i];
        }
    }
    return NULL;
}

static bt_fast_pair_spp_cnt_t *
                    bt_fast_pair_spp_find_by_address(bt_bd_addr_t *address)
{
    for (uint8_t i = 0; i < FAST_PAIR_SPP_MAXIMUM; i++) {
        if (!memcmp(address, &g_bt_fast_pair_spp_cnt_t[i].address, sizeof(bt_bd_addr_t))) {
            return &g_bt_fast_pair_spp_cnt_t[i];
        }
    }
    return NULL;
}

static bt_status_t  bt_fast_pair_spp_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    bt_status_t ret_status = BT_STATUS_SUCCESS;
    bt_fast_pair_log("[BT_FAST_PAIR][I] Spp event id:0x%x, status:0x%x, buffer:0x%x", 3, msg, status, buff);
    switch (msg) {
#ifndef MTK_BT_CM_SUPPORT
    case BT_POWER_ON_CNF: {
        bt_fast_pair_log("[BT_FAST_PAIR][I] Spp register", 0);
        if (BT_STATUS_SUCCESS != (ret_status = bt_callback_manager_add_sdp_customized_record(&g_bt_fast_pair_spp_sdp_record))) {
            bt_fast_pair_log("[BT_FAST_PAIR][I] Spp sdp register fail status 0x%x", 1, ret_status);
        }
    #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        bt_fast_pair_spp_aws_mce_init();
    #endif
        bt_fast_pair_spp_event_handle_cb(BT_FAST_PAIR_SPP_EVENT_BT_POWER_ON, status, NULL, NULL, 0);
    }
        break;
    case BT_POWER_OFF_CNF: {
    #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        bt_fast_pair_spp_aws_mce_deinit();
    #endif
        bt_fast_pair_spp_event_handle_cb(BT_FAST_PAIR_SPP_EVENT_BT_POWER_OFF, status, NULL, NULL, 0);
    }
        break;
#endif
    case BT_SPP_CONNECT_IND: {
        if (NULL == buff) {
            break;
        }
        bt_spp_connect_ind_t *conn_ind_p = (bt_spp_connect_ind_t *)buff;
        bt_fast_pair_log("[BT_FAST_PAIR][I] Spp connected handle:0x%x, local server id:0x%x, addr:0x%x", 3,
            conn_ind_p->handle, conn_ind_p->local_server_id, *(uint32_t *)(conn_ind_p->address));
        if (FAST_PAIR_SPP_SERVER_ID == conn_ind_p->local_server_id && BT_SPP_INVALID_HANDLE != conn_ind_p->handle) {
            bt_fast_pair_spp_cnt_t *spp_dev = bt_fast_pair_spp_find_by_handle(conn_ind_p->handle);
            if (spp_dev == NULL) {
                if ((NULL != (spp_dev = bt_fast_pair_spp_find_by_address(conn_ind_p->address))) ||
                    (NULL != (spp_dev = bt_fast_pair_spp_find_by_handle(0)))) {
                    if (BT_STATUS_SUCCESS == (ret_status = bt_spp_connect_response(conn_ind_p->handle, true))) {
                        memcpy(&(spp_dev->address), conn_ind_p->address, sizeof(bt_bd_addr_t));
                        spp_dev->spp_handle = conn_ind_p->handle;
                        bt_fast_pair_spp_event_handle_cb(BT_FAST_PAIR_SPP_EVENT_CONNECTED, BT_STATUS_SUCCESS, &(spp_dev->address), NULL, 0);
                    #ifdef MTK_BT_CM_SUPPORT
                        bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_CUSTOMIZED_FAST_PAIR, spp_dev->address, BT_CM_PROFILE_SERVICE_STATE_CONNECTED, status);
                    #endif
                    } else {
                        bt_fast_pair_log("[BT_FAST_PAIR][W] Spp rsp fail status 0x%x", 1, ret_status);
                    }
                    break;
                }
                bt_fast_pair_log("[BT_FAST_PAIR][W] Spp reject no resource !!!", 0);
            }
            ret_status = bt_spp_connect_response(conn_ind_p->handle, false);
            bt_fast_pair_log("[BT_FAST_PAIR][W] Spp rsp fail status 0x%x", 1, ret_status);
        }
    }
        break;
    case BT_SPP_DISCONNECT_IND: {
        if (NULL == buff) {
            break;
        }
        bt_spp_disconnect_ind_t *disc_ind_p = (bt_spp_disconnect_ind_t *)buff;
        bt_fast_pair_spp_cnt_t *spp_dev = bt_fast_pair_spp_find_by_handle(disc_ind_p->handle);
        bt_fast_pair_log("[BT_FAST_PAIR][I] Spp disconnected handle:0x%x, find device:0x%x", 2, disc_ind_p->handle, spp_dev);
        if (NULL != spp_dev) {
            uint8_t i = 0;
            spp_dev->spp_handle = 0;
            for (i = 0; i < FAST_PAIR_SPP_MAXIMUM; i++) {
                if (0 != g_bt_fast_pair_spp_cnt_t[i].spp_handle) {
                    break;
                }
            }
        #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            if (FAST_PAIR_SPP_MAXIMUM == i && (g_bt_fast_pair_spp_flags & FAST_PAIR_SPP_RHO_PENDING)) {
                g_bt_fast_pair_spp_flags &= (~FAST_PAIR_SPP_RHO_PENDING);
                bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_FAST_PAIR);
            }
        #endif
            bt_fast_pair_spp_event_handle_cb(BT_FAST_PAIR_SPP_EVENT_DISCONNECTED, BT_STATUS_SUCCESS, &(spp_dev->address), NULL, 0);
        #ifdef MTK_BT_CM_SUPPORT
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_CUSTOMIZED_FAST_PAIR, spp_dev->address, BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED, status);
        #endif
        }
    }
        break;
    case BT_SPP_DATA_RECEIVED_IND: {
        if (NULL == buff) {
            break;
        }
        bt_spp_data_received_ind_t *data_ind_p = (bt_spp_data_received_ind_t *)buff;
        bt_fast_pair_spp_cnt_t *spp_dev = bt_fast_pair_spp_find_by_handle(data_ind_p->handle);
        bt_fast_pair_log("[BT_FAST_PAIR][I] Spp data received handle:0x%x, packet length:%d, packet:0x%x, spp_dev:0x%x",
            4, data_ind_p->handle, data_ind_p->packet_length, data_ind_p->packet, spp_dev);
        if (NULL != spp_dev && NULL != data_ind_p->packet && 0 != data_ind_p->packet_length) {
            bt_fast_pair_spp_event_handle_cb(BT_FAST_PAIR_SPP_EVENT_DATA_IND, BT_STATUS_SUCCESS,
                &(spp_dev->address), data_ind_p->packet, data_ind_p->packet_length);
        }
    }
        break;
    case BT_SPP_READY_TO_SEND_IND: {
        // Todo OOM case.
    }
        break;
    default:
        break;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_fast_pair_spp_data_send(bt_bd_addr_t* addr, uint8_t *data, uint16_t data_length)
{
    bt_status_t ret_status = BT_STATUS_SUCCESS;
    bt_fast_pair_spp_cnt_t *spp_dev = NULL;
    if (NULL != addr) {
        bt_fast_pair_log("[BT_FAST_PAIR][I] Spp send by address 0x%x", 1, *(uint32_t *)addr);
        if ((NULL != (spp_dev = bt_fast_pair_spp_find_by_address(addr))) && (0 != spp_dev->spp_handle)) {
            if (BT_STATUS_SUCCESS == (ret_status = bt_spp_send(spp_dev->spp_handle, (uint8_t *)data, data_length))) {
                return BT_STATUS_SUCCESS;
            }
            bt_fast_pair_log("[BT_FAST_PAIR][W] Spp send failed 0x%x", 1, ret_status);
        }
        bt_fast_pair_log("[BT_FAST_PAIR][W] Spp send found fail can't find device", 0);
        return BT_STATUS_FAIL;
    }
    for (uint8_t i = 0; i < FAST_PAIR_SPP_MAXIMUM; i++) {
        if (0 != g_bt_fast_pair_spp_cnt_t[i].spp_handle) {
            if (BT_STATUS_SUCCESS != (ret_status =
                bt_spp_send(g_bt_fast_pair_spp_cnt_t[i].spp_handle, (uint8_t *)data, data_length))) {
                bt_fast_pair_log("[BT_FAST_PAIR][W] Spp send failed 0x%x", 1, ret_status);
            }
        }
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_fast_pair_spp_disconnect(bt_bd_addr_t* addr)
{
    bt_status_t ret_status = BT_STATUS_SUCCESS;
    bt_fast_pair_spp_cnt_t *spp_dev = NULL;
    if (NULL != addr) {
        bt_fast_pair_log("[BT_FAST_PAIR][I] Spp disconnect by address 0x%x", 1, *(uint32_t *)addr);
        if ((NULL != (spp_dev = bt_fast_pair_spp_find_by_address(addr))) && (0 != spp_dev->spp_handle)) {
            if (BT_STATUS_SUCCESS == (ret_status = bt_spp_disconnect(spp_dev->spp_handle))) {
                return BT_STATUS_SUCCESS;
            }
            bt_fast_pair_log("[BT_FAST_PAIR][W] Spp disconnect failed 0x%x", 1, ret_status);
        }
        bt_fast_pair_log("[BT_FAST_PAIR][W] Spp disconnect found fail", 0);
        return BT_STATUS_FAIL;
    }
    for (uint8_t i = 0; i < FAST_PAIR_SPP_MAXIMUM; i++) {
        if (0 != g_bt_fast_pair_spp_cnt_t[i].spp_handle) {
            if (BT_STATUS_SUCCESS != (ret_status = bt_spp_disconnect(g_bt_fast_pair_spp_cnt_t[i].spp_handle))) {
                bt_fast_pair_log("[BT_FAST_PAIR][I] Spp disconnect failed 0x%x", 1, ret_status);
            }
        }
    }
    return BT_STATUS_SUCCESS;
}

#ifdef MTK_BT_CM_SUPPORT
static bt_status_t  bt_fast_pair_profile_service_handle_cb(bt_cm_profile_service_handle_t type, void *data)
{
    bt_status_t status = BT_STATUS_FAIL;
    bt_fast_pair_log("[BT_FAST_PAIR][I] Fast pair handle cm type:0x%02x.", 1, type);
    switch (type) {
    case BT_CM_PROFILE_SERVICE_HANDLE_POWER_ON:
        bt_fast_pair_log("[BT_FAST_PAIR][I] Spp register", 0);
        if (BT_STATUS_SUCCESS != (status = bt_callback_manager_add_sdp_customized_record(&g_bt_fast_pair_spp_sdp_record))) {
            bt_fast_pair_log("[BT_FAST_PAIR][I] Spp sdp register fail status 0x%x", 1, status);
        }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        bt_fast_pair_spp_aws_mce_init();
#endif
        bt_fast_pair_spp_event_handle_cb(BT_FAST_PAIR_SPP_EVENT_BT_POWER_ON, status, NULL, NULL, 0);
        break;
    case BT_CM_PROFILE_SERVICE_HANDLE_POWER_OFF:
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        bt_fast_pair_spp_aws_mce_deinit();
#endif
        bt_fast_pair_spp_event_handle_cb(BT_FAST_PAIR_SPP_EVENT_BT_POWER_OFF, status, NULL, NULL, 0);
        break;
    case BT_CM_PROFILE_SERVICE_HANDLE_CONNECT:
        bt_fast_pair_log("[BT_FAST_PAIR][W] Fast pair can't support active connect request.", 0);
        break;
    case BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT:
        status = bt_fast_pair_spp_disconnect(data);
        break;
    default:
        break;
    }
    return status;
}
#endif

void                bt_fast_pair_spp_init(void)
{
    bt_status_t status = BT_STATUS_FAIL;
    bt_fast_pair_log("[BT_FAST_PAIR][I] Spp init", 0);
    memset(&g_bt_fast_pair_spp_cnt_t, 0, sizeof(g_bt_fast_pair_spp_cnt_t));
#ifdef MTK_BT_CM_SUPPORT
    status = bt_callback_manager_register_callback(bt_callback_type_app_event,
        (uint32_t)(MODULE_MASK_SPP), (void *)bt_fast_pair_spp_event_callback);
	bt_cm_profile_service_register(BT_CM_PROFILE_SERVICE_CUSTOMIZED_FAST_PAIR,
        &bt_fast_pair_profile_service_handle_cb);
#else
    status = bt_callback_manager_register_callback(bt_callback_type_app_event,
        (uint32_t)(MODULE_MASK_SYSTEM | MODULE_MASK_SPP), (void *)bt_fast_pair_spp_event_callback);
#endif
    bt_fast_pair_log("[BT_FAST_PAIR][I] Spp init result : 0x%x", 1, status);
}

