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


#include "bt_sink_srv.h"
#include "bt_connection_manager_internal.h"
#include "bt_gap_le.h"
#include "bt_connection_manager_utils.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_device_manager_config.h"
#include "bt_device_manager.h"
#include "hal_wdt.h"
#include "bt_sink_srv_music.h"
#include "hal.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif
#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
#include "bt_aws_mce_role_recovery.h"
#endif
#endif
#include "bt_di.h"
#include "bt_os_layer_api.h"
#include "bt_callback_manager.h"
#include "bt_device_manager_db.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_power.h"

#define BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(CHECK_CONDITION, RET_VALUE, LOG_STRING, ...) \
    if (CHECK_CONDITION) {  \
        bt_cmgr_report_id(LOG_STRING, ##__VA_ARGS__); \
        return (RET_VALUE); \
    }

#define BT_CM_CHECK_RET_WITH_VALUE_NO_LOG(CHECK_CONDITION, RET_VALUE)   \
    if (CHECK_CONDITION) { \
        return (RET_VALUE); \
    }

#define BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(CHECK_CONDITION, LOG_STRING, ...) \
    if (CHECK_CONDITION) {  \
        bt_cmgr_report_id(LOG_STRING, ##__VA_ARGS__); \
        return; \
    }

#define BT_CM_CHECK_RET_NO_VALUE_NO_LOG(CHECK_CONDITION)    \
    if (CHECK_CONDITION) {  \
        return; \
    }

#define BT_CM_LOG_CONFIG_NULL   "[BT_CM][E] CM config is null"
#define BT_CM_LOG_CONTEXT_NULL  "[BT_CM][E] CM context is null"

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_cm_get_takeover_disconnect_device=_default_bt_cm_get_takeover_disconnect_device")
#pragma comment(linker, "/alternatename:_bt_cm_event_callback=_default_bt_cm_event_callback")
#pragma comment(linker, "/alternatename:_bt_cm_get_reconnect_profile=_default_bt_cm_get_reconnect_profile")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_cm_get_takeover_disconnect_device = default_bt_cm_get_takeover_disconnect_device
#pragma weak bt_cm_event_callback = default_bt_cm_event_callback
#pragma weak bt_cm_get_reconnect_profile = default_bt_cm_get_reconnect_profile
#else
#error "Unsupported Platform"
#endif

bt_cm_cnt_t     *g_bt_cm_cnt = NULL;
bt_cm_config_t  *g_bt_cm_cfg = NULL;

static  bt_cm_common_type_t g_bt_cm_user_req_discoverable = BT_CM_COMMON_TYPE_DISABLE;
static  bt_cm_common_type_t g_bt_cm_user_req_connectable  = BT_CM_COMMON_TYPE_ENABLE;

static const uint8_t g_bt_cm_search_di_pattern[] = {
    BT_SDP_ATTRIBUTE_HEADER_8BIT(3),                            /* Data Element Sequence, 9 bytes */
    BT_SDP_UUID_16BIT(BT_DI_SDP_SERVICE_CLASS_PNP_INFORMATION), /* The device Identification UUID in big-endian. */
    0x02, 0x00
};

static const uint8_t g_bt_cm_search_di_attributes[] = {
    0x00, 0x64,                                                 /* 0x0064, max handle for attribute return */
    BT_SDP_ATTRIBUTE_HEADER_8BIT(6),
    BT_SDP_UINT_16BIT(BT_DI_SDP_ATTRIBUTE_VENDOR_ID),
    BT_SDP_UINT_16BIT(BT_DI_SDP_ATTRIBUTE_PRODUCT_ID)
};

static const bt_gap_default_sniff_params_t g_bt_cm_default_sniff_params = {
    .max_sniff_interval = 800,
    .min_sniff_interval = 800,
    .sniff_attempt = 4,
    .sniff_timeout = 1
};

#define BT_CM_ADD_MASK(MASK, POSITION)      ((MASK) |= (0x01U << (POSITION)))
#define BT_CM_REMOVE_MASK(MASK, POSITION)   ((MASK) &= ~(0x01U << (POSITION)))

#define BT_CM_AUTO_RECONNECT_TYPE_POWER_ON  (0x01)
#define BT_CM_AUTO_RECONNECT_TYPE_LINK_LOST (0x02)
typedef uint8_t bt_cm_auto_reconnect_t;

bool bt_cm_is_reconnect_flag = true;

bt_bd_addr_t cancel_connection_addr = {0};
static void         bt_cm_auto_reconnect(bt_cm_auto_reconnect_t reconnect_t);
static void         bt_cm_connection_state_update(void *param);
static bool         bt_cm_is_specail_device(bt_bd_addr_t *address);
uint32_t            bt_cm_get_reconnect_profile(bt_bd_addr_t *addr);

bt_bd_addr_t        *default_bt_cm_get_takeover_disconnect_device()
{
    bt_cmgr_report_id("[BT_CM] Default takeover the firstly connected device", 0);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, NULL, BT_CM_LOG_CONTEXT_NULL, 0);
    bt_cm_remote_device_t *device_p = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTING];
    if (NULL != device_p && BT_CM_ACL_LINK_CONNECTING == device_p->link_info.link_state) {
        return &(device_p->link_info.addr);
    }
    device_p = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED];
    while (NULL != device_p) {
        if (!bt_cm_is_specail_device(&(device_p->link_info.addr))) {
            return &(device_p->link_info.addr);
        }
        device_p = device_p->next[BT_CM_LIST_CONNECTED];
    }
    return NULL;
}

uint32_t            default_bt_cm_get_reconnect_profile(bt_bd_addr_t *addr)
{
    return BT_CM_PROFILE_SERVICE_MASK_ALL;
}

void                default_bt_cm_event_callback(bt_cm_event_t event_id, void *params, uint32_t params_len)
{
    bt_cmgr_report_id("[BT_CM]Not find event handler event_id:%d, param:0x%x", 2, event_id, params);
}

static void         BT_CM_LOG_CONNECTION_STATUS(bt_cm_remote_device_t *device_p)
{
    BT_CM_CHECK_RET_NO_VALUE_NO_LOG(NULL == device_p);
    bt_cmgr_report_id("[BT_CM][I] Device:0x%x, request_conn:0x%04x, request_disconn:0x%04x, "
        "connecting:0x%04x, disconnecting:0x%04x, connected:0x%04x, flags:0x%x, link_state:0x%x", 8,
        *(uint32_t *)(&device_p->link_info.addr), device_p->request_connect_mask, device_p->request_disconnect_mask,
        device_p->link_info.connecting_mask, device_p->link_info.disconnecting_mask, device_p->link_info.connected_mask,
        device_p->flags, device_p->link_info.link_state);
}

void                bt_cm_list_add(bt_cm_list_t list_type, bt_cm_remote_device_t *device, bt_cm_list_add_t add_type)
{
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cnt, BT_CM_LOG_CONTEXT_NULL, 0);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == device, "[BT_CM][E] list remove device is NULL", 0);
    if (BT_CM_LIST_ADD_FRONT == add_type) {
        bt_cm_remote_device_t *temp = device;
        while (NULL != temp->next[list_type]) {
            temp = temp->next[list_type];
        }
        temp->next[list_type] = g_bt_cm_cnt->handle_list[list_type];
        g_bt_cm_cnt->handle_list[list_type] = device;
    } else if (BT_CM_LIST_ADD_BACK == add_type) {
        bt_cm_remote_device_t *temp = (bt_cm_remote_device_t *)&(g_bt_cm_cnt->handle_list);
        while (NULL != temp->next[list_type]) {
            temp = temp->next[list_type];
        }
        temp->next[list_type] = device;
    }
}

void                bt_cm_list_remove(bt_cm_list_t list_type, bt_cm_remote_device_t *device)
{
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cnt, BT_CM_LOG_CONTEXT_NULL, 0);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == device, "[BT_CM][E] list remove device is NULL", 0);
    bt_cm_remote_device_t *temp = (bt_cm_remote_device_t *)&(g_bt_cm_cnt->handle_list);
    while (NULL != temp->next[list_type]) {
        if (temp->next[list_type] == device) {
            temp->next[list_type] = ((bt_cm_remote_device_t *)(temp->next[list_type]))->next[list_type];
            device->next[list_type] = NULL;
            return;
        }
        temp = temp->next[list_type];
    }
}

bt_cm_remote_device_t*
                    bt_cm_list_get_last(bt_cm_list_t list_type)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, NULL, BT_CM_LOG_CONTEXT_NULL, 0);
    bt_cm_remote_device_t *temp = g_bt_cm_cnt->handle_list[list_type];
    while (NULL != temp && NULL != temp->next[list_type]) {
        temp = temp->next[list_type];
    }
    return temp;
}

static bool         bt_cm_list_is_empty(bt_cm_list_t list_type)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, NULL, BT_CM_LOG_CONTEXT_NULL, 0);
    return (NULL == g_bt_cm_cnt->handle_list[list_type]);
}

static void         bt_cm_remote_acl_connected_confirm(bt_cm_remote_device_t *device_p)
{
    bt_device_manager_db_remote_pnp_info_t device_id;
    bt_gap_write_page_scan_activity(1024, 18);
    /* Default set supervision timeout to 5s. */
    bt_gap_write_supervision_timeout(device_p->link_info.handle, 0x1F40);
    if (g_bt_cm_cnt->connected_dev_num >= g_bt_cm_cnt->max_connection_num && true == g_bt_cm_cfg->connection_takeover) {
        /* Do takeover flow. */
        bt_bd_addr_t *disconn_addr = bt_cm_get_takeover_disconnect_device();
        if (NULL == disconn_addr) {
            bt_gap_disconnect(device_p->link_info.handle);
            return;
        } else {
            bt_cm_connect_t disconn_param;
            disconn_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
            bt_cm_memcpy(disconn_param.address, disconn_addr, sizeof(bt_bd_addr_t));
            bt_cm_disconnect(&disconn_param);
        }
    }
    bt_cmgr_report_id("[BT_CM][I] Remote sp connected, addr:0x%x", 1, *(uint32_t *)(&device_p->link_info.addr));
    if (BT_STATUS_SUCCESS != bt_device_manager_remote_find_pnp_info((void *)(device_p->link_info.addr), &device_id)) {
        bt_sdpc_service_request_t sdp_req = {
           .address = (void *)&(device_p->link_info.addr),
           .pattern_length = sizeof(g_bt_cm_search_di_pattern),
           .search_pattern = g_bt_cm_search_di_pattern,
           .user_data = &(device_p->link_info.addr),
        };
        bt_sdpc_search_service(&sdp_req);
    }
}

bt_cm_acl_link_state_t bt_cm_get_link_state(bt_bd_addr_t addr);
static void         bt_cm_handle_link_connected_ind(bt_cm_remote_device_t *device_p, bt_status_t status, bt_gap_link_status_updated_ind_t *param)
{
    bt_cm_remote_info_update_ind_t remote_update = {
        .pre_acl_state = device_p->link_info.link_state,
        .pre_connected_service = device_p->link_info.connected_mask,
        .connected_service = device_p->link_info.connected_mask,
        .reason = BT_STATUS_SUCCESS
    };
    bt_cm_list_remove(BT_CM_LIST_CONNECTING, device_p);
    bt_cm_acl_link_state_t pre_acl_state = device_p->link_info.link_state;
    if (BT_CM_ACL_LINK_DISCONNECTED == pre_acl_state || BT_CM_ACL_LINK_PENDING_CONNECT == pre_acl_state) {
        /* To avoid profile connect conflict, Clear the request connect profile after link connected passive. */
        device_p->request_connect_mask = BT_CM_PROFILE_SERVICE_MASK_NONE;
        device_p->flags |= BT_CM_REMOTE_FLAG_LOCK_DISCONNECT;
    }
    if (BT_GAP_LINK_STATUS_CONNECTED_0 < param->link_status) {
        /* Update the paired list sequence due to encrypted. */
        bt_device_manager_remote_set_seq_num(*((bt_bd_addr_t *)(param->address)), 1);
        device_p->link_info.link_state = BT_CM_ACL_LINK_ENCRYPTED;
        /* If agent encrypted with smartphone, then auto to close discoverable. */
        bt_cm_discoverable(false);
    } else {
        if (bt_cm_is_specail_device((void *)(param->address))) {
        #ifdef MTK_AWS_MCE_ENABLE
            /* For aws can enable, need set special link and partner side link encrypted. */
            device_p->link_info.link_state = BT_CM_ACL_LINK_ENCRYPTED;
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                /* Update scan mode, when special link connected in agent. */
                bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
            } else {
                bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
                /* If partner connected with agent, then auto to close discoverable. */
                bt_cm_discoverable(false);
                if (bt_cm_memcmp(local_addr, param->address, sizeof(bt_bd_addr_t)) && 
                    BT_CM_ACL_LINK_CONNECTED <= bt_cm_get_link_state(*local_addr)) {
                /* If smartphone connected in agent, then clear the partner connecting list. */
                    bt_cm_cancel_connect(NULL);
                }
            }
        #endif
        } else {
            device_p->link_info.link_state = BT_CM_ACL_LINK_CONNECTED;
            bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
        }
    }
    bt_cm_memcpy(&(remote_update.address), (void *)(param->address), sizeof(bt_bd_addr_t));
    remote_update.acl_state = device_p->link_info.link_state;
    bt_cm_event_callback(BT_CM_EVENT_REMOTE_INFO_UPDATE, &remote_update, sizeof(remote_update));
    bt_cm_register_callback_notify(BT_CM_EVENT_REMOTE_INFO_UPDATE, &remote_update, sizeof(remote_update));
    if (BT_CM_ACL_LINK_CONNECTED > pre_acl_state) {
        device_p->link_info.handle = param->handle;
        if(BT_STATUS_CONNECTION_NOT_FOUND == bt_gap_get_role_sync(device_p->link_info.handle, &device_p->link_info.local_role)){
            return;
        }
        bt_cm_memcpy(device_p->link_info.addr, param->address, sizeof(bt_bd_addr_t));
        bt_cm_list_add(BT_CM_LIST_CONNECTED, device_p, BT_CM_LIST_ADD_BACK);
        g_bt_cm_cnt->connected_dev_num++;
        bt_cmgr_report_id("[BT_CM][I] Device connected dev num %d, max %d", 2, g_bt_cm_cnt->connected_dev_num, g_bt_cm_cnt->max_connection_num);
        if (!bt_cm_is_specail_device(&(device_p->link_info.addr))) {
            bt_cm_remote_acl_connected_confirm(device_p);
        } else {
            device_p->flags |= BT_CM_REMOTE_FLAG_LOCK_DISCONNECT;
        }
        bt_cm_timer_start(BT_CM_CONNECTION_TIMER_ID, 10, bt_cm_connection_state_update, NULL);
    }
}

static void         bt_cm_remote_acl_disconnected_confirm(bt_bd_addr_t address, bt_status_t status)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_cmgr_report_id("[BT_CM][I] device manager local role is 0x%x", 1, aws_role);
#else
    bt_aws_mce_role_t aws_role = BT_AWS_MCE_ROLE_NONE;
#endif
    bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
    if (g_bt_cm_cnt->connected_dev_num == (BT_AWS_MCE_ROLE_AGENT == aws_role ? 1 : 0)) {
        bt_gap_write_page_scan_activity(1024, 36);
    }
    if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        bt_cm_remote_device_t *conn_aws_dev = NULL;
        bt_cm_remote_device_t *find_dev = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED];
        /* If one profile connected need select to connect aws profile. */
        while (NULL != find_dev) {
            if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) &
                (find_dev->link_info.connecting_mask | find_dev->link_info.connected_mask | find_dev->request_connect_mask))) {
                conn_aws_dev = NULL;
                break;
            }
            if (find_dev->link_info.link_state > BT_CM_ACL_LINK_CONNECTED &&
                (NULL == conn_aws_dev || !bt_cm_is_specail_device(&(find_dev->link_info.addr)))) {
                conn_aws_dev = find_dev;
            }
            find_dev = find_dev->next[BT_CM_LIST_CONNECTED];
        }
        if (NULL != conn_aws_dev) {
            bt_cmgr_report_id("[BT_CM][I] connect aws device 0x%x", 1, *(uint32_t *)&(conn_aws_dev->link_info.addr));
            bt_cm_connect_t reconn;
            bt_cm_memcpy(reconn.address, conn_aws_dev->link_info.addr, sizeof(bt_bd_addr_t));
            reconn.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
            bt_cm_connect(&reconn);
        }
    }
    if (g_bt_cm_cfg->link_loss_reconnect_profile != BT_CM_PROFILE_SERVICE_MASK_NONE && 
        (BT_HCI_STATUS_PAGE_TIMEOUT == status || BT_HCI_STATUS_CONNECTION_TIMEOUT == status || BT_HCI_STATUS_CONTROLLER_BUSY == status ||
        BT_HCI_STATUS_LMP_RESPONSE_TIMEOUT_OR_LL_RESPONSE_TIMEOUT == status || BT_HCI_STATUS_CONNECTION_LIMIT_EXCEEDED == status ||
        BT_CM_STATUS_ROLE_RECOVERY == status || BT_HCI_STATUS_VENDOR_REMOTE_CONNECTION_EXIST == status)) {
#ifdef AIR_MULTI_POINT_ENABLE
        /* use max_connection_num, connection_takeover and role to confirm emp power reconnec*/
        if ((BT_HCI_STATUS_PAGE_TIMEOUT == status) && (g_bt_cm_cfg->max_connection_num >= (2 + (g_bt_cm_cfg->connection_takeover ? 1 : 0) + (BT_AWS_MCE_ROLE_AGENT == aws_role ? 1 : 0)))) {
            bt_device_manager_remote_set_seq_num(address, 2);
        }
#endif
        if (g_bt_cm_cnt->connected_dev_num >= (g_bt_cm_cnt->max_connection_num - (g_bt_cm_cfg->connection_takeover ? 1 : 0))) {
            return;
        }
        bt_cm_connect_t reconn;
        bt_cm_memcpy(reconn.address, address, sizeof(bt_bd_addr_t));
        uint32_t reconnect_profiles = bt_cm_get_reconnect_profile((void*)address);
        reconn.profile = reconnect_profiles & g_bt_cm_cfg->link_loss_reconnect_profile & ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS));
        bt_cm_connect(&reconn);
    } else {
        if (BT_HCI_STATUS_PIN_OR_KEY_MISSING == status) {
            bt_device_manager_remote_delete_info((void *)address, 0);
        }
        bt_cm_connection_state_update(NULL);
    }
}

static void         bt_cm_handle_link_disconnected_ind(bt_cm_remote_device_t *device_p, bt_status_t status)
{
    bt_cmgr_report_id("[BT_CM][I] Detach single link, reason: 0x%x", 1, status);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == device_p, "[BT_CM][E] Can't find disconnected device", 0);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_cmgr_report_id("[BT_CM][I] device manager local role is 0x%x", 1, aws_role);
#else
    bt_aws_mce_role_t aws_role = BT_AWS_MCE_ROLE_NONE;
#endif
    bt_cm_remote_info_update_ind_t remote_update = {
        .acl_state = BT_CM_ACL_LINK_DISCONNECTED,
        .pre_acl_state = device_p->link_info.link_state,
        .pre_connected_service = device_p->link_info.connected_mask,
        .connected_service = BT_CM_PROFILE_SERVICE_MASK_NONE,
        .reason = status
    };
    bt_cm_list_remove(BT_CM_LIST_CONNECTING, device_p);
    bt_cm_list_remove(BT_CM_LIST_CONNECTED, device_p);
    device_p->link_info.link_state = BT_CM_ACL_LINK_DISCONNECTED;
    bt_cm_memcpy(&(remote_update.address), &(device_p->link_info.addr), sizeof(bt_bd_addr_t));
    if (BT_HCI_STATUS_ACL_CONNECTION_ALREADY_EXISTS == status) {
        /* If two agent occur, the old agent will received disconnect complete with acl already exists. */
        remote_update.reason = BT_HCI_STATUS_VENDOR_REMOTE_CONNECTION_EXIST;
    }
    bt_cm_event_callback(BT_CM_EVENT_REMOTE_INFO_UPDATE, &remote_update, sizeof(remote_update));
    bt_cm_register_callback_notify(BT_CM_EVENT_REMOTE_INFO_UPDATE, &remote_update, sizeof(remote_update));
    if (device_p->flags & BT_CM_REMOTE_FLAG_RESERVE_DUE_TO_ROLE_RECOVERY) {
        status = BT_CM_STATUS_ROLE_RECOVERY;
    }
    bt_cm_memset(device_p, 0, sizeof(*device_p));
#ifdef AIR_MULTI_POINT_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
        bt_cmgr_report_id("[BT_CM][I] Cancel connecting success due to rho", 0);
        g_bt_cm_rho_flags_t &= (~BT_CM_RHO_PREPARE_WAIT_FLAG_CANCEL_CONNECTION);
        bt_cmgr_report_id("[BT_CM][RHO][I] g_bt_cm_rho_flags is 0x%x", 1, g_bt_cm_rho_flags_t);
        bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_SINK_CM);
    }
#endif
#endif
    if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        if (BT_HCI_STATUS_ROLE_SWITCH_PENDING == status || BT_HCI_STATUS_CONNECTION_TIMEOUT == status
            || BT_CM_AWS_LINK_DISCONNECT_REASON_MASK == (status & BT_CM_AWS_LINK_DISCONNECT_REASON_MASK)) {
            bt_cm_connect_t reconn;
            bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
            bt_cm_memcpy(reconn.address, remote_update.address, sizeof(bt_bd_addr_t));
            reconn.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
            bt_cm_connect(&reconn);

            bt_cm_remote_device_t *temp_device = bt_cm_find_device(BT_CM_FIND_BY_ADDR, &(remote_update.address));
            bt_cm_list_remove(BT_CM_LIST_CONNECTING, temp_device);
            bt_cm_list_add(BT_CM_LIST_CONNECTING, temp_device, BT_CM_LIST_ADD_FRONT);

            if (BT_HCI_STATUS_CONNECTION_TIMEOUT == status && bt_cm_memcmp(local_addr, remote_update.address, sizeof(bt_bd_addr_t))) {
                bt_cm_auto_reconnect(BT_CM_AUTO_RECONNECT_TYPE_LINK_LOST);
            }
        } else {
            bt_cm_connection_state_update(NULL);
        }
    } else {
        bt_cm_remote_acl_disconnected_confirm(remote_update.address, status);
        if (remote_update.pre_acl_state > BT_CM_ACL_LINK_CONNECTED &&
            BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
            /* Update the paired list sequence due to disconnected. */
            uint8_t connected_dev = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0);
            bt_device_manager_remote_set_seq_num(remote_update.address, connected_dev + 1);
        }
    }
    if (BT_CM_POWER_STATE_ON != bt_cm_power_get_state()) {
        bt_cm_connection_state_update(NULL);
        bt_cm_power_update(NULL);
    }
}

static bt_bd_addr_t g_bt_cm_delay_reconnect_addr;
static bt_cm_profile_service_mask_t g_bt_cm_delay_reconnect_profile;

static void         bt_cm_delay_reconnect_callback(void *params)
{
    bt_cm_connect_t reconnect;
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)&g_bt_cm_delay_reconnect_addr);
    bt_cm_memcpy(&(reconnect.address), &g_bt_cm_delay_reconnect_addr, sizeof(bt_bd_addr_t));
    reconnect.profile = g_bt_cm_delay_reconnect_profile;
    if (NULL != device_p) {
        reconnect.profile &= ~(device_p->link_info.connected_mask | device_p->link_info.connecting_mask);
    }
    bt_cmgr_report_id("[BT_CM][I] Delay to reconnect the connected failed profile addr:0x%x, profile:0x%x", 2,
        *(uint32_t *)&(reconnect.address), reconnect.profile);
    if (BT_CM_PROFILE_SERVICE_MASK_NONE != reconnect.profile) {
        bt_cm_connect(&reconnect);
    }
}

static void         bt_cm_handle_link_update_ind(bt_status_t status, bt_gap_link_status_updated_ind_t *param)
{
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == param, "[BT_CM][E] Param is null", 0);
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, param->address);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
#else
    bt_aws_mce_role_t aws_role = BT_AWS_MCE_ROLE_NONE;
#endif
    bt_cmgr_report_id("[BT_CM][I] Link update device:0x%x, link status:0x%x, status:0x%x, handle = 0x%x", 4, device_p, param->link_status, status, param->handle);
    if (!bt_cm_memcmp(param->address, &cancel_connection_addr, sizeof(bt_bd_addr_t))) {
        bt_cm_timer_stop(BT_CM_CANCEL_CONNECT_TIMER_ID);
     }
    if (BT_GAP_LINK_STATUS_CONNECTED_0 <= param->link_status) {
        if (BT_STATUS_SUCCESS != status && NULL != device_p) {
            if (BT_CM_ACL_LINK_CONNECTING == device_p->link_info.link_state || BT_CM_ACL_LINK_DISCONNECTING == device_p->link_info.link_state) {
                bt_cm_handle_link_disconnected_ind(device_p, status);
                if (BT_HCI_STATUS_UNKNOWN_CONNECTION_IDENTIFIER == status) {
                    bt_cmgr_report_id("[BT_CM][I] Cancel connect success", 0);
                }
            } else if (BT_CM_ACL_LINK_CONNECTED >= device_p->link_info.link_state) {
                bt_gap_disconnect_with_reason(param->handle, BT_HCI_STATUS_PIN_OR_KEY_MISSING);
            }
        } else if (BT_STATUS_SUCCESS == status) {
            bt_bd_addr_t temp_addr = {0};
            if (NULL != device_p && BT_CM_ACL_LINK_DISCONNECTING == device_p->link_info.link_state) {
                bt_cmgr_report_id("[BT_CM][I] Device is disconnecting state !!! ", 0);
                bt_gap_disconnect(param->handle);
            } else if (NULL == device_p && (NULL == (device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, temp_addr)))) {
                /* If can't find free device context, will need to find connecting device context. */
                if (NULL != (device_p = bt_cm_list_get_last(BT_CM_LIST_CONNECTING))) {
                    bt_cmgr_report_id("[BT_CM][I] Find a connecting device to replace. ", 0);    
                    if (BT_CM_ACL_LINK_CONNECTING == device_p->link_info.link_state) {
                        bt_gap_cancel_connection((const bt_bd_addr_t *)&(device_p->link_info.addr));
                    }
                    bt_cm_list_remove(BT_CM_LIST_CONNECTING, device_p);
                    bt_cm_memset(device_p, 0, sizeof(*device_p));
                    bt_cm_handle_link_connected_ind(device_p, status, param);
                } else {
                    bt_gap_disconnect(param->handle);
                }
            } else {
                if (device_p->link_info.link_state == BT_CM_ACL_LINK_PENDING_CONNECT) {
                    device_p->flags &= ~(BT_CM_REMOTE_FLAG_CONNECT_CONFLICT);
                    bt_cm_memcpy(&g_bt_cm_delay_reconnect_addr, device_p->link_info.addr, sizeof(bt_bd_addr_t));
                    g_bt_cm_delay_reconnect_profile = (device_p->expected_connect_mask & ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)));
                    bt_cm_timer_stop(BT_CM_DELAY_RECONNECT_TIMER_ID);
                    bt_cm_timer_start(BT_CM_DELAY_RECONNECT_TIMER_ID, 3000, bt_cm_delay_reconnect_callback, NULL);
                }
                bt_cm_handle_link_connected_ind(device_p, status, param);
            }
        }
    } else if (BT_GAP_LINK_STATUS_DISCONNECTED == param->link_status) {
        g_bt_cm_cnt->connected_dev_num--;
        bt_cm_handle_link_disconnected_ind(device_p, status);
    } else if (BT_GAP_LINK_STATUS_CONNECTION_FAILED == param->link_status) {
        if (BT_HCI_STATUS_ACL_CONNECTION_ALREADY_EXISTS == status || (BT_HCI_STATUS_VENDOR_REMOTE_CONNECTION_EXIST == status &&
             (bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0) || BT_AWS_MCE_ROLE_NONE == aws_role
        #ifdef MTK_AWS_MCE_ENABLE
              || (BT_AWS_MCE_ROLE_AGENT == aws_role && BT_AWS_MCE_SRV_MODE_NORMAL != bt_aws_mce_srv_get_mode())
        #endif
              ))) {
            /* Connect conflict, wait connect request from controller. */
            if (NULL == device_p) {
                return;
            }
            device_p->link_info.link_state = BT_CM_ACL_LINK_PENDING_CONNECT;
            device_p->flags |= (BT_CM_REMOTE_FLAG_LOCK_DISCONNECT | BT_CM_REMOTE_FLAG_CONNECT_CONFLICT);
            /* Start: Wait 3s to retry connect for connection conflict case. */
            bt_cm_memcpy(&g_bt_cm_delay_reconnect_addr, &(device_p->link_info.addr), sizeof(bt_bd_addr_t));
            g_bt_cm_delay_reconnect_profile = (device_p->expected_connect_mask & ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)));
            bt_cm_timer_stop(BT_CM_DELAY_RECONNECT_TIMER_ID);
            bt_cm_timer_start(BT_CM_DELAY_RECONNECT_TIMER_ID, 3000, bt_cm_delay_reconnect_callback, NULL);
            /* End. */
        } else {
#ifdef AIR_MULTI_POINT_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            if (BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
                bt_cmgr_report_id("[BT_CM][I] Cancel connecting success due to rho", 0);
                bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_SINK_CM);
            }
#endif
#endif
            /* If two agent occurred, the new agent will received connect fail with reason #BT_HCI_STATUS_VENDOR_REMOTE_CONNECTION_EXIST. */
            bt_cm_handle_link_disconnected_ind(device_p, status);
            if (NULL == device_p) {
                bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
            }
        }
    }
}

static void         bt_cm_switch_role_timer_callback(void *params)
{
    bt_cm_remote_device_t *device_p = (bt_cm_remote_device_t *)params;
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cfg, BT_CM_LOG_CONFIG_NULL , 0);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == device_p || (BT_CM_ACL_LINK_CONNECTED > device_p->link_info.link_state),
        "[BT_CM][E] Switch slave timer callback device not connected", 0);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(device_p->link_info.local_role == g_bt_cm_cfg->request_role || BT_CM_ROLE_UNKNOWN == g_bt_cm_cfg->request_role,
        "[BT_CM][I] Switch slave timer callback request role:0x%x match current role 0x%x", 2, g_bt_cm_cfg->request_role, device_p->link_info.local_role);
    if (device_p->retry_times < g_bt_cm_cfg->request_role_retry_times) {
        bt_cm_switch_role(device_p->link_info.addr, g_bt_cm_cfg->request_role);
    } else {
        bt_cmgr_report_id("[BT_CM][E] Switch role retry expired times:%d ", 1, device_p->retry_times);
    }
}

static void         bt_cm_gap_role_event_handler(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_cm_remote_device_t *device_p = NULL;
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cfg, BT_CM_LOG_CONFIG_NULL , 0);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cnt, BT_CM_LOG_CONTEXT_NULL, 0);
    if (BT_GAP_GET_ROLE_CNF == msg) {
        bt_gap_get_role_cnf_t *get_role = (bt_gap_get_role_cnf_t *)buffer;
        BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == (device_p = bt_cm_find_device(BT_CM_FIND_BY_HANDLE, &(get_role->handle))),
            "[BT_CM][E] Get role cnf, can't find device", 0);
        bt_cmgr_report_id("[BT_CM][I] Get role cnf: 0x%x", 1, device_p->link_info.local_role);
        device_p->link_info.local_role = get_role->local_role;
        return;
    } else if (BT_GAP_SET_ROLE_CNF == msg) {
        device_p = bt_cm_find_device(BT_CM_FIND_BY_REMOTE_FLAG, (void *)BT_CM_REMOTE_FLAG_ROLE_SWITCHING);
        if (NULL != device_p) {
            device_p->flags &= (~BT_CM_REMOTE_FLAG_ROLE_SWITCHING);
        }
        BT_CM_CHECK_RET_NO_VALUE_NO_LOG(BT_STATUS_SUCCESS == status);
        bt_cmgr_report_id("[BT_CM][W] Set role fail.", 0);
    } else if (BT_GAP_ROLE_CHANGED_IND == msg) {
        bt_gap_role_changed_ind_t *role_change = (bt_gap_role_changed_ind_t *)buffer;
        BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == role_change || NULL == (device_p = bt_cm_find_device(BT_CM_FIND_BY_HANDLE, &(role_change->handle))),
            "[BT_CM][E] Can't find device by handle", 0);
        bt_cmgr_report_id("[BT_CM][I] Changed:0x%x,local:0x%x", 2, role_change->local_role, device_p->link_info.local_role);
        device_p->link_info.local_role = role_change->local_role;
        BT_CM_CHECK_RET_NO_VALUE_NO_LOG(BT_STATUS_SUCCESS == status);
    }
    bt_cm_timer_start(BT_CM_SWITCH_ROLE_TIMER_ID, 100, bt_cm_switch_role_timer_callback, (void *)device_p);
}

void                bt_cm_switch_role(bt_bd_addr_t address, bt_role_t role)
{
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)address);
    bt_cmgr_report_id("[BT_CM][I] Set address:0x%x, role:0x%x", 2, *(uint32_t*)address, role);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == device_p || (BT_CM_ACL_LINK_CONNECTED > device_p->link_info.link_state),
        "[BT_CM][E] Swtich role can't find connected device", 0);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG((device_p->flags & BT_CM_REMOTE_FLAG_ROLE_SWITCHING),
        "[BT_CM][W] Switch role is ongoing", 0);
    device_p->flags |= BT_CM_REMOTE_FLAG_ROLE_SWITCHING;
    bt_status_t ret = bt_gap_set_role(device_p->link_info.handle, role);
    if (BT_STATUS_SUCCESS != ret) {
        bt_cmgr_report_id("[BT_CM][W] Set role fail:0x%x", 1, ret);
        bt_cm_gap_role_event_handler(BT_GAP_SET_ROLE_CNF, BT_STATUS_FAIL, NULL);
    } else {
        device_p->retry_times++;
    }
}

static void         bt_cm_sdp_search_service_cnf_handle(bt_status_t status, void *buffer)
{
    bt_sdpc_service_cnf_t *service_result = (bt_sdpc_service_cnf_t *)buffer;
    BT_CM_CHECK_RET_NO_VALUE_NO_LOG(NULL == service_result);
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)(service_result->user_data));
    BT_CM_CHECK_RET_NO_VALUE_NO_LOG(NULL == device_p);
    if (0 == service_result->handle_number) {
        bt_device_manager_db_remote_pnp_info_t pnp_info = {
            .vender_id = 0,
            .product_id = 0
        };
        bt_device_manager_remote_update_pnp_info(device_p->link_info.addr, &pnp_info);
        bt_cmgr_report_id("[BT_CM][I] Pnp info serach no result", 0);
    } else {
        uint32_t query_service_handle = bt_sdp_get_32bit(service_result->handle_list);
        bt_sdpc_attribute_request_t request;
        request.address = (void *)&(device_p->link_info.addr);
        request.search_handle = query_service_handle;
        request.attribute_pattern = g_bt_cm_search_di_attributes;
        request.pattern_length = sizeof(g_bt_cm_search_di_attributes);
        request.user_data = &(device_p->link_info.addr);
        bt_status_t return_result = bt_sdpc_search_attribute(&request);
        bt_cmgr_report_id("[BT_CM][I] Pnp info serach attribute result: %d", 1, return_result);
    }
}

static void         bt_cm_sdp_search_attribute_cnf_handle(bt_status_t status, void *buffer)
{
    bt_sdpc_attribute_cnf_t *attr_result = (bt_sdpc_attribute_cnf_t *)buffer;
    uint8_t *parse_result = NULL, *data = NULL;
    uint16_t result_len = 0, data_len = 0;

    bt_device_manager_db_remote_pnp_info_t pnp_info = {
        .vender_id = 0,
        .product_id = 0
    };
    bt_sdpc_parse_attribute(&parse_result, &result_len, BT_DI_SDP_ATTRIBUTE_VENDOR_ID, 0, attr_result->length, attr_result->attribute_data);
    bt_sdpc_parse_next_value(&data, &data_len, parse_result, result_len);
    if (data_len) {
        pnp_info.vender_id = (data[0] << 8 | data[1]);
        bt_cmgr_report_id("[BT_CM][I] vender_id:0x%04x", 1, pnp_info.vender_id);
    }
    bt_sdpc_parse_attribute(&parse_result, &result_len, BT_DI_SDP_ATTRIBUTE_PRODUCT_ID, 0, attr_result->length, attr_result->attribute_data);
    bt_sdpc_parse_next_value(&data, &data_len, parse_result, result_len);
    if (data_len) {
        pnp_info.product_id= (data[0] << 8 | data[1]);
        bt_cmgr_report_id("[BT_CM][I] product_id:0x%04x", 1, pnp_info.product_id);
    }
    bt_device_manager_remote_update_pnp_info((void *)(attr_result->user_data), &pnp_info);
}

static bt_status_t  bt_cm_sdp_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_cmgr_report_id("[BT_CM][I] SDK sdp msg:0x%x, status:0x%x", 2, msg, status);
    switch (msg) {
    case BT_SDPC_SEARCH_SERVICE_CNF: {
        bt_cm_sdp_search_service_cnf_handle(status, buffer);
        break;
    }
    case BT_SDPC_SEARCH_ATTRIBUTE_CNF: {
        bt_cm_sdp_search_attribute_cnf_handle(status, buffer);
        break;
    }
    case BT_SDPC_SEARCH_ERROR: {
        bt_cmgr_report_id("[BT_CM][I] BT_SDPC_SEARCH_ERROR", 0);
        break;
    }
    default:
        bt_cmgr_report_id("[BT_CM][E] Unexcepted msg", 0);
        break;
    }
    return BT_STATUS_SUCCESS;
}

static bt_status_t  bt_cm_gap_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][I] SDK gap msg:0x%x, status:0x%x", 2, msg, status);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cfg, result, BT_CM_LOG_CONFIG_NULL , 0);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, result, BT_CM_LOG_CONTEXT_NULL, 0);

    switch (msg) {
    case BT_GAP_LINK_STATUS_UPDATED_IND: {
        bt_cm_handle_link_update_ind(status, buffer);
        break;
    }
    case BT_GAP_SET_SCAN_MODE_CNF: {
        if (g_bt_cm_cnt->flags & BT_CM_FLAG_PENDING_SET_SCAN_MODE) {
            g_bt_cm_cnt->flags &= (~BT_CM_FLAG_PENDING_SET_SCAN_MODE);
            bt_gap_set_scan_mode(g_bt_cm_cnt->scan_mode);
        }
        break;
    }
    case BT_GAP_GET_ROLE_CNF:
    case BT_GAP_SET_ROLE_CNF:
    case BT_GAP_ROLE_CHANGED_IND: {
        bt_cm_gap_role_event_handler(msg, status, buffer);
        break;
    }
    case BT_GAP_SNIFF_MODE_CHANGE_IND: {
        BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == buffer, result, "[BT_CM][E] Gap buffer is NULL", 0);
        bt_gap_sniff_mode_changed_ind_t* ind = (bt_gap_sniff_mode_changed_ind_t *)buffer;
        bt_cm_remote_device_t *device_p = NULL;
        BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == (device_p = bt_cm_find_device(BT_CM_FIND_BY_HANDLE, &(ind->handle))), result, "[BT_CM][E] Can't find the device", 0);
        if (!bt_cm_is_specail_device(&(device_p->link_info.addr))) {
            device_p->link_info.sniff_state = ind->sniff_status;
        }
        break;
    }
    default:
        //bt_cmgr_report_id("[BT_CM][I] Unexcepted msg", 0);
        break;
    }
    return result;
}

static bt_status_t  bt_cm_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    uint32_t moduel = msg & 0xFF000000;
    switch (moduel) {
    case BT_MODULE_GAP:
        result = bt_cm_gap_event_callback(msg, status, buffer);
        break;
    case BT_MODULE_SDP:
        result = bt_cm_sdp_event_callback(msg, status, buffer);
        break;
    default:
        bt_cmgr_report_id("[BT_CM][E] Not expected bt event module 0x%x", 1, moduel);
        break;
    }
    return result;
}

static uint8_t      bt_cm_get_encrypted_device_number()
{
    uint8_t count = 0;
    bt_cm_remote_device_t *device_p = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED];
    while (NULL != device_p) {
        if (BT_CM_ACL_LINK_CONNECTED < device_p->link_info.link_state && !bt_cm_is_specail_device(&(device_p->link_info.addr))) {
            count++;
        }
        device_p = device_p->next[BT_CM_LIST_CONNECTED];
    }
    return count;
}

static bt_cm_remote_device_t *
                    bt_cm_connection_state_machine(bt_cm_remote_device_t *device_p)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == device_p, NULL, "[BT_CM][E] State machine device is null", 0);
    bt_cm_remote_device_t *next_device = NULL;
    bt_cmgr_report_id("[BT_CM][I] State machine begin link state <<< 0x%x", 1, device_p->link_info.link_state);
    if (BT_CM_ACL_LINK_CONNECTED <= device_p->link_info.link_state) {
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
        next_device = ((NULL != device_p->next[BT_CM_LIST_CONNECTED]) ?
            device_p->next[BT_CM_LIST_CONNECTED] : g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTING]);
        if (!(device_p->flags & BT_CM_REMOTE_FLAG_LOCK_DISCONNECT) && BT_CM_PROFILE_SERVICE_MASK_NONE ==
            (device_p->link_info.connected_mask | device_p->link_info.disconnecting_mask | device_p->link_info.connecting_mask | device_p->request_connect_mask)) {
            bt_cm_remote_device_t *disconn_dev = NULL;
            for (uint32_t i = 0; i < g_bt_cm_cfg->max_connection_num; ++i) {
                if (BT_CM_ACL_LINK_DISCONNECTING <= g_bt_cm_cnt->devices_list[i].link_info.link_state &&
                    BT_CM_ACL_LINK_PENDING_CONNECT != g_bt_cm_cnt->devices_list[i].link_info.link_state && device_p != &g_bt_cm_cnt->devices_list[i]) {
                    disconn_dev = &g_bt_cm_cnt->devices_list[i];
                    break;
                }
            }
            if (bt_cm_memcmp(&(device_p->link_info.addr), local_addr, sizeof(bt_bd_addr_t)) || NULL == disconn_dev) {
                /* For controller's limitation, the link with local address need disconnect at last. */
                if (BT_STATUS_SUCCESS == bt_gap_disconnect(device_p->link_info.handle)) {
                    if (device_p->link_info.link_state > BT_CM_ACL_LINK_CONNECTED &&
                        BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
                        /* Update the paired list sequence due to disconnect. */
                        uint8_t connected_dev = bt_cm_get_encrypted_device_number();
                        bt_device_manager_remote_set_seq_num(device_p->link_info.addr, connected_dev);
                    }
                    device_p->link_info.link_state = BT_CM_ACL_LINK_DISCONNECTING;
                    bt_cm_list_remove(BT_CM_LIST_CONNECTED, device_p);
                }
                next_device = NULL;
            }
        }
    } else if (BT_CM_ACL_LINK_CONNECTING == device_p->link_info.link_state) {
        if ((device_p->request_disconnect_mask & device_p->link_info.connecting_mask) == device_p->link_info.connecting_mask) {
            if (BT_STATUS_SUCCESS == bt_gap_cancel_connection((const bt_bd_addr_t *)&(device_p->link_info.addr))) {
                bt_cm_timer_start(BT_CM_CANCEL_CONNECT_TIMER_ID, 60000, bt_cm_cancel_connect_timeout_callback, NULL);
                bt_cm_memcpy(&cancel_connection_addr, &device_p->link_info.addr, sizeof(bt_bd_addr_t));
                device_p->link_info.link_state = BT_CM_ACL_LINK_DISCONNECTING;
                bt_cm_list_remove(BT_CM_LIST_CONNECTING, device_p);
            }
        }
    } else if (BT_CM_ACL_LINK_PENDING_CONNECT == device_p->link_info.link_state) {
        if (BT_CM_PROFILE_SERVICE_MASK_NONE != device_p->link_info.connecting_mask) {
            device_p->link_info.link_state = BT_CM_ACL_LINK_CONNECTING;
        } else if (!(device_p->flags & BT_CM_REMOTE_FLAG_CONNECT_CONFLICT) && (BT_CM_PROFILE_SERVICE_MASK_NONE == device_p->request_connect_mask)) {
            next_device = device_p->next[BT_CM_LIST_CONNECTING];
            bt_cm_handle_link_disconnected_ind(device_p, BT_HCI_STATUS_CONNECTION_TERMINATED_BY_LOCAL_HOST);
        }
    } else {
        /* There is noting to do for disconnecting and disconnected devices. */
        bt_cmgr_report_id("[BT_CM][E] Current link state 0x%x error", 1, device_p->link_info.link_state);    
        /* bt_cm_assert(0 && "Connection status error"); */
    }
    bt_cmgr_report_id("[BT_CM][I] State machine end link state >>> 0x%x", 1, device_p->link_info.link_state);
    return next_device;
}

static bool         bt_cm_is_specail_device(bt_bd_addr_t *address)
{
    bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
    if (NULL != local_addr && !bt_cm_memcmp(address, local_addr, sizeof(bt_bd_addr_t))) {
        return true;
    }
#ifdef MTK_AWS_MCE_ENABLE
    bt_bd_addr_t *peer_addr = bt_device_manager_aws_local_info_get_peer_address();
    if (NULL != peer_addr && !bt_cm_memcmp(address, peer_addr, sizeof(bt_bd_addr_t))) {
        return true;
    }
#endif
    return false;
}

static void         bt_cm_connection_state_update(void *param)
{
    bt_cmgr_report_id("[BT_CM][I] Connection state update", 0);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cnt, BT_CM_LOG_CONTEXT_NULL, 0);
    /* If #param is valid means only to deal with the device indicated by #param. ohterwise we need deal with all devices in connected or connecting list. */
    bt_cm_remote_device_t *device_p = param;
    bt_cm_profile_service_handle_callback_t profile_cb;
    bt_device_manager_power_state_t power_state = bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
#else
    bt_aws_mce_role_t aws_role = BT_AWS_MCE_ROLE_NONE;
#endif
    device_p = (NULL != device_p ? device_p : g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED]);
    device_p = (NULL != device_p ? device_p : g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTING]);
    while (NULL != device_p) {
        BT_CM_LOG_CONNECTION_STATUS(device_p);
        bool connect_allow = false;
        bt_cm_remote_device_t *temp_dev = NULL;
        bt_cm_profile_service_mask_t profile = 0x01U;
        /* Unlock the disconnect action. */
        if (device_p->flags & BT_CM_REMOTE_FLAG_PENDING_DISCONNECT) {
            device_p->flags &= ~(BT_CM_REMOTE_FLAG_LOCK_DISCONNECT | BT_CM_REMOTE_FLAG_CONNECT_CONFLICT);
            device_p->request_connect_mask = BT_CM_PROFILE_SERVICE_NONE;
        }
        if ((g_bt_cm_cnt->flags & BT_CM_FLAG_INITIATED) &&
            (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == power_state || BT_DEVICE_MANAGER_POWER_STATE_ACTIVE_PENDING == power_state) &&
            (true == bt_cm_is_specail_device(&(device_p->link_info.addr)) || (true == bt_cm_is_reconnect_flag && !((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role))) &&
            (g_bt_cm_cnt->connected_dev_num < (2 + !!(BT_AWS_MCE_ROLE_AGENT == aws_role)) || device_p->link_info.link_state >= BT_CM_ACL_LINK_CONNECTED)) {
            connect_allow = true;;
        }
        for (uint32_t i = 0; i <= BT_CM_PROFILE_SERVICE_MAX; i++) {
            profile = 1 << i;
            profile_cb = g_bt_cm_cnt->profile_service_cb[i];
            if (device_p->request_connect_mask == 0 && device_p->request_disconnect_mask == 0) {
                break;
            }
            if (NULL == profile_cb) {
                device_p->request_connect_mask &= (~profile);
                device_p->request_disconnect_mask &= (~profile);
                continue;
            }
            if (device_p->request_disconnect_mask & profile) {
                if (profile == BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) &&
                    ((device_p->request_disconnect_mask | device_p->link_info.disconnecting_mask) & (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)))) {
                    /* For vp sync, the aws profile need disconnect at last. */
                    continue;
                } else if (BT_STATUS_SUCCESS == profile_cb(BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT, device_p->link_info.addr)) {
                    device_p->request_disconnect_mask &= (~profile);
                    if ((device_p->link_info.connecting_mask | device_p->link_info.connected_mask) & profile) {
                        device_p->link_info.disconnecting_mask |= profile;
                        device_p->link_info.connecting_mask &= (~profile);
                    }
                }
            } else if (true == connect_allow && (device_p->request_connect_mask & profile) && !(device_p->link_info.disconnecting_mask & profile)) {
                if (BT_STATUS_SUCCESS == profile_cb(BT_CM_PROFILE_SERVICE_HANDLE_CONNECT, device_p->link_info.addr)) {
                    device_p->request_connect_mask &= (~profile);
                    if (!(device_p->link_info.connected_mask & profile)) {
                        device_p->link_info.connecting_mask |= profile;
                    }
                    device_p->link_info.disconnecting_mask &= (~profile);
                }
            }
        }
        temp_dev = bt_cm_connection_state_machine(device_p);
        BT_CM_LOG_CONNECTION_STATUS(device_p);
        device_p = temp_dev;
        if (NULL != param) {
            break;
        }
    }
}

bt_status_t             bt_cm_write_scan_mode_internal(bt_cm_common_type_t inquiry_scan, bt_cm_common_type_t page_scan)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, BT_CM_STATUS_FAIL, BT_CM_LOG_CONTEXT_NULL, 0);
    bt_gap_scan_mode_t set_mode = g_bt_cm_cnt->scan_mode;
    bt_gap_scan_mode_t pre_mode = g_bt_cm_cnt->scan_mode;
    bt_cmgr_report_id("[BT_CM][I] Scan mode, old:0x%x, new_inquiry:%d, new_page:%d, flags:0x%x", 4,
        g_bt_cm_cnt->scan_mode, inquiry_scan, page_scan, g_bt_cm_cnt->flags);
#ifdef MTK_AWS_MCE_ENABLE
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & bt_device_manager_aws_local_info_get_role(), ret,
        "[BT_CM][W] Current role can't set scan mode", 0);
#endif
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(BT_CM_POWER_STATE_ON != bt_cm_power_get_state(), ret, "[BT_CM][W] Current state is not power on", 0);
    inquiry_scan = (BT_CM_COMMON_TYPE_UNKNOW != inquiry_scan ? inquiry_scan : g_bt_cm_user_req_discoverable);
    page_scan = (BT_CM_COMMON_TYPE_UNKNOW != page_scan ? page_scan : g_bt_cm_user_req_connectable);
    bt_cmgr_report_id("[BT_CM][I] user request discoverable is 0x%x, user request connectable is 0x%x", 2, g_bt_cm_user_req_discoverable, g_bt_cm_user_req_connectable);	
    if (BT_CM_COMMON_TYPE_DISABLE == inquiry_scan) {
        set_mode &= (~BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY);
    } else if (BT_CM_COMMON_TYPE_ENABLE == inquiry_scan) {
        set_mode |= BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY;
    }
    if (BT_CM_COMMON_TYPE_DISABLE == page_scan) {
        set_mode &= (~BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
    } else if (BT_CM_COMMON_TYPE_ENABLE == page_scan) {
        set_mode |= BT_GAP_SCAN_MODE_CONNECTABLE_ONLY;
    }
    if (
    #ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state() ||
    #endif
        g_bt_cm_cnt->connected_dev_num >= (g_bt_cm_cnt->max_connection_num + !!g_bt_cm_cfg->connection_takeover) ||
        bt_cm_find_device(BT_CM_FIND_BY_LINK_STATE, (void *)BT_CM_ACL_LINK_CONNECTED) ||
        (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC) &&
        BT_DEVICE_MANAGER_POWER_STATE_ACTIVE_PENDING != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC))) {
        /* To avoid IOT issue, If one connection not encrypted, we can't enable page scan. */
        set_mode &= (~BT_GAP_SCAN_MODE_CONNECTABLE_ONLY);
    }
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(pre_mode == set_mode, ret, "[BT_CM][W] Set the same scan mode.", 0);
    g_bt_cm_cnt->scan_mode = set_mode;
    g_bt_cm_cnt->flags &= (~BT_CM_FLAG_PENDING_SET_SCAN_MODE);
    if (BT_STATUS_SUCCESS != (ret = bt_gap_set_scan_mode(set_mode))) {
        g_bt_cm_cnt->flags |= BT_CM_FLAG_PENDING_SET_SCAN_MODE;
        bt_cmgr_report_id("[BT_CM][W] Set scan mode fail staus:%x", 1, ret);
    }
    return BT_STATUS_SUCCESS;
}

void                    bt_cm_write_scan_mode(bt_cm_common_type_t discoveralbe, bt_cm_common_type_t connectable)
{
    bt_cmgr_report_id("[BT_CM][I] User set scan mode inuqiry:%d, page:%d", 2, discoveralbe, connectable);
    if (BT_CM_COMMON_TYPE_UNKNOW != discoveralbe && discoveralbe != g_bt_cm_user_req_discoverable) {
        g_bt_cm_user_req_discoverable = discoveralbe;
        bt_cm_visibility_state_update_ind_t state;
        if (BT_CM_COMMON_TYPE_ENABLE == discoveralbe) {
            state.visibility_state = true;
            bt_cm_event_callback(BT_CM_EVENT_VISIBILITY_STATE_UPDATE, &state, sizeof(state));
            bt_cm_register_callback_notify(BT_CM_EVENT_VISIBILITY_STATE_UPDATE, &state, sizeof(state));
        } else {
            state.visibility_state = false;
            bt_cm_event_callback(BT_CM_EVENT_VISIBILITY_STATE_UPDATE, &state, sizeof(state));
            bt_cm_register_callback_notify(BT_CM_EVENT_VISIBILITY_STATE_UPDATE, &state, sizeof(state));
        }
    }
    if (BT_CM_COMMON_TYPE_UNKNOW != connectable && connectable != g_bt_cm_user_req_connectable) {
        g_bt_cm_user_req_connectable = connectable;
    }
    bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
}

bt_cm_remote_device_t   *bt_cm_find_device(bt_cm_find_t find_type, void *param)
{
    bt_cm_remote_device_t *device_p = NULL;
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == param, NULL, "[BT_CM][E] Find device param is null", 0);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, NULL, BT_CM_LOG_CONTEXT_NULL, 0);
    for (uint32_t i = 0; i < g_bt_cm_cfg->max_connection_num; ++i) {
        if (BT_CM_FIND_BY_HANDLE == find_type &&
            g_bt_cm_cnt->devices_list[i].link_info.handle == *((bt_gap_connection_handle_t *)param)) {
            device_p = &g_bt_cm_cnt->devices_list[i];
            break;
        } else if (BT_CM_FIND_BY_ADDR == find_type &&
            bt_cm_memcmp(param, &g_bt_cm_cnt->devices_list[i].link_info.addr, sizeof(bt_bd_addr_t)) == 0) {
            device_p = &g_bt_cm_cnt->devices_list[i];
            break;
        } else if (BT_CM_FIND_BY_REMOTE_FLAG == find_type &&
            (g_bt_cm_cnt->devices_list[i].flags & ((uint32_t)param))) {
            device_p = &g_bt_cm_cnt->devices_list[i];
            break;
        } else if (BT_CM_FIND_BY_LINK_STATE == find_type &&
            (g_bt_cm_cnt->devices_list[i].link_info.link_state == (uint32_t)param)) {
            device_p = &g_bt_cm_cnt->devices_list[i];
            break;
        } else if (g_bt_cm_cfg->max_connection_num == (i + 1)) {
            bt_cmgr_report_id("[BT_CM][I] Can't find device by type %d", 1, find_type);
        }
    }
    if (NULL != device_p) {
        bt_cmgr_report_id("[BT_CM][I] Find handle:0x%x", 1, device_p->link_info.handle);
    }
    return device_p;
}

static void     bt_cm_notify_power_status(bt_cm_profile_service_handle_t handle)
{
    bt_status_t ret;
    for (uint32_t i = 0; i <= BT_CM_PROFILE_SERVICE_MAX; i++) {
        if (NULL != g_bt_cm_cnt->profile_service_cb[i] &&
            BT_STATUS_SUCCESS != (ret = (g_bt_cm_cnt->profile_service_cb[i])(handle, NULL))) {
            bt_cmgr_report_id("[BT_CM][W] Profile service:%d, power status:%d fail, status:0x%x", 3, i, handle, ret);
        }
    }
}

static void     bt_cm_auto_reconnect(bt_cm_auto_reconnect_t reconnect_t)
{
    bt_cm_connect_t conn_req;
    bt_bd_addr_t*   conn_addr = NULL;
    uint8_t temp_max_connection_num;
    uint8_t recon_num;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_cmgr_report_id("[BT_CM][I] device manager local role is 0x%x", 1, aws_role);
#else
    bt_aws_mce_role_t aws_role = BT_AWS_MCE_ROLE_NONE;
#endif
    temp_max_connection_num = g_bt_cm_cfg->max_connection_num -
            (!!(g_bt_cm_cfg->connection_takeover) + !(BT_AWS_MCE_ROLE_NONE == aws_role));
    for (recon_num = 1; recon_num <= temp_max_connection_num; recon_num++) {
        if (NULL == (conn_addr = bt_device_manager_remote_get_dev_by_seq_num(recon_num))) {
            break;
        }
        bt_cm_memcpy(&(conn_req.address), conn_addr, sizeof(bt_bd_addr_t));
        bt_cmgr_report_id("[BT_CM][I] Reconnect remote cod: 0x%x", 1, bt_device_manager_remote_find_cod((void *)conn_addr));
        if (BT_CM_AUTO_RECONNECT_TYPE_POWER_ON == reconnect_t) {
            conn_req.profile = g_bt_cm_cfg->power_on_reconnect_profile & ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS));
        } else {
            conn_req.profile = g_bt_cm_cfg->link_loss_reconnect_profile & ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS));
        }
        bt_cmgr_report_id("[BT_CM][I] Reconnect remote addr:%02x:%02x:%02x:%02x:%02x:%02x, profile mask:0x%x", 7,
        (*conn_addr)[5], (*conn_addr)[4], (*conn_addr)[3], (*conn_addr)[2], (*conn_addr)[1], (*conn_addr)[0], conn_req.profile);
        bt_cm_connect(&conn_req);       
    }
}

bt_bd_addr_t    *bt_cm_get_last_connected_device(void)
{
    bt_bd_addr_t *last_addr = bt_device_manager_remote_get_dev_by_seq_num(1);
    if (NULL != last_addr) {
        bt_cmgr_report_id("[BT_CM][I] Get last connected device:0x%x", 1, *(uint32_t*)last_addr);
    } else {
        bt_cmgr_report_id("[BT_CM][I] Get last connected device:NULL", 0);
    }
    return last_addr;
}

void            bt_cm_power_on_cnf(bt_device_manager_power_status_t status)
{
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cfg, BT_CM_LOG_CONFIG_NULL , 0);
    bt_cmgr_report_id("[BT_CM][I] BT power on status: 0x%x", 1, status);
    BT_CM_CHECK_RET_NO_VALUE_NO_LOG(BT_DEVICE_MANAGER_POWER_STATUS_AIR_PAIRING_START == status);
    bt_gap_write_page_timeout(g_bt_cm_cfg->page_timeout);
    bt_gap_set_extended_inquiry_response((uint8_t *)(g_bt_cm_cfg->eir_data.data), g_bt_cm_cfg->eir_data.length);
    bt_gap_set_default_sniff_parameters(&g_bt_cm_default_sniff_params);
    bt_cm_notify_power_status(BT_CM_PROFILE_SERVICE_HANDLE_POWER_ON);
    g_bt_cm_cnt->flags |= BT_CM_FLAG_INITIATED;
#ifdef MTK_AWS_MCE_ENABLE
    bt_bd_addr_t*   conn_addr = NULL;
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_cmgr_report_id("[BT_CM][I] Power on confirm local aws role is %d", 1, aws_role);
    /* Power on reconnect flow */
    if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        /* For agent reconnect special aws link. */
        conn_addr = bt_device_manager_get_local_address();
    } else if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        /* For partner reconnect agent. */
        conn_addr = bt_device_manager_aws_local_info_get_peer_address();
    }
    if (NULL != conn_addr) {
        bt_cm_connect_t conn_req;
        bt_cm_remote_device_t *device_p = NULL;
        bt_cm_memcpy(&conn_req.address, conn_addr, sizeof(bt_bd_addr_t));
        conn_req.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
        bt_cm_connect(&conn_req);
        device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, conn_addr);
        bt_cm_list_remove(BT_CM_LIST_CONNECTING, device_p);
        bt_cm_list_add(BT_CM_LIST_CONNECTING, device_p, BT_CM_LIST_ADD_FRONT);
    } else {
        bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
    }
#else
    bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
#endif
    if ((BT_CM_PROFILE_SERVICE_MASK_NONE != g_bt_cm_cfg->power_on_reconnect_profile) &&
        (BT_DEVICE_MANAGER_POWER_STATUS_ROLE_RECOVERY != status)) {
        bt_cm_auto_reconnect(BT_CM_AUTO_RECONNECT_TYPE_POWER_ON);
    }
}

void            bt_cm_power_off_cnf(bt_device_manager_power_status_t status)
{
    bt_cmgr_report_id("[BT_CM][I] BT power off status: 0x%x", 1, status);
    BT_CM_CHECK_RET_NO_VALUE_NO_LOG(BT_DEVICE_MANAGER_POWER_STATUS_AIR_PAIRING_COMPLETE == status);
    bt_cm_notify_power_status(BT_CM_PROFILE_SERVICE_HANDLE_POWER_OFF);
    g_bt_cm_cnt->connected_dev_num = 0;
    g_bt_cm_cnt->scan_mode = 0;
    g_bt_cm_cnt->flags = 0;
    g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED] = NULL;
    if (BT_DEVICE_MANAGER_POWER_STATUS_ROLE_RECOVERY != status) {
        g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTING] = NULL;
        bt_cm_memset(&(g_bt_cm_cnt->devices_list), 0, sizeof(bt_cm_remote_device_t) * g_bt_cm_cnt->devices_buffer_num);
    }
    bt_cm_timer_stop(BT_CM_SWITCH_ROLE_TIMER_ID);
    bt_cm_timer_stop(BT_CM_CONNECTION_TIMER_ID);
    bt_cm_timer_stop(BT_CM_DELAY_RECONNECT_TIMER_ID);
}

void            bt_cm_profile_service_register(bt_cm_profile_service_t profile,
                    bt_cm_profile_service_handle_callback_t cb)
{
    bt_cmgr_report_id("[BT_CM][I] Register profile service callback, profile:%d", 1, profile);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cnt, BT_CM_LOG_CONTEXT_NULL, 0);
    g_bt_cm_cnt->profile_service_cb[profile] = cb;
}

static void     bt_cm_profile_service_mask_update(bt_cm_remote_device_t *device_p,
                    bt_cm_profile_service_t profile, bt_cm_profile_service_state_t state)
{
    if (BT_CM_PROFILE_SERVICE_STATE_CONNECTED == state) {
        BT_CM_ADD_MASK(device_p->link_info.connected_mask, profile);
        BT_CM_REMOVE_MASK(device_p->link_info.connecting_mask, profile);
        BT_CM_REMOVE_MASK(device_p->link_info.disconnecting_mask, profile);
        BT_CM_REMOVE_MASK(device_p->request_connect_mask, profile);
    } else if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == state) {
        BT_CM_REMOVE_MASK(device_p->link_info.connected_mask, profile);
        BT_CM_REMOVE_MASK(device_p->link_info.connecting_mask, profile);
        BT_CM_REMOVE_MASK(device_p->link_info.disconnecting_mask, profile);
        BT_CM_REMOVE_MASK(device_p->request_disconnect_mask, profile);
    } else if (BT_CM_PROFILE_SERVICE_STATE_CONNECTING == state) {
        BT_CM_ADD_MASK(device_p->link_info.connecting_mask, profile);
        BT_CM_REMOVE_MASK(device_p->link_info.connected_mask, profile);
        BT_CM_REMOVE_MASK(device_p->link_info.disconnecting_mask, profile);
        BT_CM_REMOVE_MASK(device_p->request_connect_mask, profile);
    } else if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTING == state) {
        BT_CM_ADD_MASK(device_p->link_info.disconnecting_mask, profile);
        BT_CM_REMOVE_MASK(device_p->link_info.connecting_mask, profile);
        BT_CM_REMOVE_MASK(device_p->request_disconnect_mask, profile);
    } else {
        bt_cmgr_report_id("[BT_CM][W] Unexpected profie state", 0);
    }
}

void            bt_cm_profile_service_status_notify(bt_cm_profile_service_t profile, bt_bd_addr_t addr,
                    bt_cm_profile_service_state_t state, bt_status_t reason)
{
    bt_cmgr_report_id("[BT_CM][I] Profile service status notify, profile:%d, state:0x%02x, reason:0x%x, addr:0x%02x:%02x:%02x:%02x:%02x:%02x", 9,
        profile, state, reason, addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, addr);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == device_p, "[BT_CM][W] Can't find device with profile state", 0);
    bt_cm_remote_info_update_ind_t remote_update = {
        .acl_state = device_p->link_info.link_state,
        .pre_acl_state = device_p->link_info.link_state,
        .pre_connected_service = device_p->link_info.connected_mask,
        .reason = reason
    };
    if (!bt_cm_memcmp(&device_p->link_info.addr, &cancel_connection_addr, sizeof(bt_bd_addr_t))) {
        bt_cm_timer_stop(BT_CM_CANCEL_CONNECT_TIMER_ID);
    }
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();   
    bt_cmgr_report_id("[BT_CM][I] device manager local role is 0x%x", 1, aws_role);
#else
    bt_aws_mce_role_t aws_role = BT_AWS_MCE_ROLE_NONE;
#endif
    BT_CM_LOG_CONNECTION_STATUS(device_p);
    if (BT_CM_PROFILE_SERVICE_NONE != profile) {
        bt_cm_profile_service_mask_update(device_p, profile, state);
    }
    if (BT_CM_PROFILE_SERVICE_STATE_CONNECTED == state) {
        if (BT_CM_POWER_STATE_ON != bt_cm_power_get_state() || (device_p->flags & BT_CM_REMOTE_FLAG_PENDING_DISCONNECT)) {
            if (BT_CM_PROFILE_SERVICE_NONE != profile) {
                BT_CM_ADD_MASK(device_p->request_disconnect_mask, profile);
            }
        } else if (BT_CM_PROFILE_SERVICE_AWS != profile && BT_AWS_MCE_ROLE_AGENT == aws_role) {
            bt_cm_remote_device_t *temp_dev = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED];
            /* If one profile connected need select to connect aws profile. */
            while (NULL != temp_dev) {
                if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & (temp_dev->link_info.connecting_mask | temp_dev->link_info.connected_mask | temp_dev->link_info.disconnecting_mask))) {
                    break;
                }
                temp_dev = temp_dev->next[BT_CM_LIST_CONNECTED];
            }
            if (NULL == temp_dev || (device_p != temp_dev && bt_cm_is_specail_device(&(temp_dev->link_info.addr)))) {
                bt_cm_connect_t reconn;
                bt_cm_memcpy(reconn.address, device_p->link_info.addr, sizeof(bt_bd_addr_t));
                reconn.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
                bt_cm_connect(&reconn);
            } else if (device_p->link_info.local_role != g_bt_cm_cfg->request_role && BT_CM_ROLE_UNKNOWN != g_bt_cm_cfg->request_role) {
                bt_cm_switch_role(device_p->link_info.addr, g_bt_cm_cfg->request_role);
            }
            BT_CM_ADD_MASK(device_p->expected_connect_mask, profile);
        } else if (BT_AWS_MCE_ROLE_NONE == aws_role) {
            if (device_p->link_info.local_role != g_bt_cm_cfg->request_role && BT_CM_ROLE_UNKNOWN != g_bt_cm_cfg->request_role) {
                bt_cm_switch_role(device_p->link_info.addr, g_bt_cm_cfg->request_role);
            }
        }
        if (BT_CM_PROFILE_SERVICE_AWS == profile && BT_AWS_MCE_ROLE_AGENT == aws_role) {
            if (bt_cm_timer_is_exist(BT_CM_DIS_CONN_WAIT_AWS_TIMER_ID)) {                
                bt_cmgr_report_id("[BT_CM][I] AWS CONNECTED UPDATE STATUS", 0);
                bt_cm_timer_stop(BT_CM_DIS_CONN_WAIT_AWS_TIMER_ID);
                bt_cm_connection_state_update(NULL);
            }

        }
    }
    if (BT_CM_PROFILE_SERVICE_NONE != profile && (device_p->link_info.connected_mask != remote_update.pre_connected_service)) {
        remote_update.connected_service = device_p->link_info.connected_mask;
        bt_cm_memcpy(&(remote_update.address), addr, sizeof(bt_bd_addr_t));
        bt_cm_event_callback(BT_CM_EVENT_REMOTE_INFO_UPDATE, &remote_update, sizeof(remote_update));
        bt_cm_register_callback_notify(BT_CM_EVENT_REMOTE_INFO_UPDATE, &remote_update, sizeof(remote_update));
    }
    BT_CM_LOG_CONNECTION_STATUS(device_p);
    if (state == BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED && reason == BT_STATUS_FAIL) {
        bt_cmgr_report_id("[BT_CM][I] connect conflict", 0);
        bt_cm_timer_start(BT_CM_CONNECTION_TIMER_ID, 600, bt_cm_connection_state_update, NULL);    
    } else {
        bt_cm_timer_start(BT_CM_CONNECTION_TIMER_ID, 30, bt_cm_connection_state_update, NULL);
    }
    if ((device_p->link_info.connected_mask | device_p->link_info.connecting_mask | device_p->link_info.disconnecting_mask) == BT_CM_PROFILE_SERVICE_MASK_NONE) {
        bt_cm_power_update(NULL);
    }
}

bt_status_t         bt_cm_discoverable(bool discoverable)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_cm_mutex_lock();
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();    
    bt_cmgr_report_id("[BT_CM][I] device manager local role is 0x%x", 1, aws_role);
    bt_cmgr_report_id("[BT_CM][I] Discoveralbe request %d, cur role 0x%x", 2, discoverable, aws_role);
    if ((aws_role & 0xF) && discoverable) {
        bt_cmgr_report_id("[BT_CM][E] Discoveralbe request %d, cur role(0x%x) forbidden", 2, discoverable, aws_role);
        bt_cm_mutex_unlock();
        return BT_STATUS_FAIL;
    }
    if (((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) && (true == discoverable)) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_ENABLE, BT_CM_COMMON_TYPE_UNKNOW);
        bt_cm_mutex_unlock();
        return bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_AGENT);
    }
#endif
    if (true == discoverable) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_ENABLE, BT_CM_COMMON_TYPE_UNKNOW);
    } else {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_UNKNOW);
    }
    bt_cm_mutex_unlock();
    return ret;
}

bt_gap_connection_handle_t  bt_cm_get_gap_handle(bt_bd_addr_t addr)
{
    bt_gap_connection_handle_t gap_handle = 0;
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)addr);
    if (NULL != device_p) {
        gap_handle =  device_p->link_info.handle;
    }
    bt_cmgr_report_id("[BT_CM][I] Get gap handle:0x%x", 1, gap_handle);
    return gap_handle;
}

uint32_t            bt_cm_get_connected_devices(bt_cm_profile_service_mask_t profiles, bt_bd_addr_t *addr_list, uint32_t list_num)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, 0, BT_CM_LOG_CONTEXT_NULL, 0);
    uint32_t count = 0;
    bt_cm_remote_device_t *device_p = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED];

    while (NULL != device_p) {
        if (BT_CM_PROFILE_SERVICE_MASK_NONE == profiles || 0 != (profiles & device_p->link_info.connected_mask)) {
            count++;
            if (addr_list != NULL && 0 != list_num) {
                bt_cm_memcpy(&(addr_list[count - 1]), device_p->link_info.addr, sizeof(bt_bd_addr_t));
                if (count == list_num) {
                    break;
                }
            }
        }
        device_p = device_p->next[BT_CM_LIST_CONNECTED];
    }
    bt_cmgr_report_id("[BT_CM][I] Connected count:%d", 1, count);
    return count;
}

bt_cm_profile_service_mask_t
                    bt_cm_get_connected_profile_services(bt_bd_addr_t addr)
{
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, addr);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == device_p || BT_CM_ACL_LINK_CONNECTED > device_p->link_info.link_state,
        BT_CM_PROFILE_SERVICE_MASK_NONE, "[BT_CM][E] Remote device not connected", 0);
    return device_p->link_info.connected_mask;
}

bt_cm_profile_service_state_t
                    bt_cm_get_profile_service_state(bt_bd_addr_t addr, bt_cm_profile_service_t profile)
{
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, addr);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == device_p || BT_CM_ACL_LINK_CONNECTED > device_p->link_info.link_state,
        BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED, "[BT_CM][E] Remote device not connected", 0);
    if (device_p->link_info.connected_mask & BT_CM_PROFILE_SERVICE_MASK(profile)) {
        return BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
    }
    return BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
}

bt_cm_link_info_t * bt_cm_get_link_information(bt_bd_addr_t addr)
{
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, addr);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == device_p, NULL, "[BT_CM][E] Remote device can't found", 0);
    return &(device_p->link_info);
}

bt_cm_acl_link_state_t bt_cm_get_link_state(bt_bd_addr_t addr)
{
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, addr);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == device_p, BT_CM_ACL_LINK_DISCONNECTED, "[BT_CM][E] Remote device not found", 0);
    return device_p->link_info.link_state;
}

static bt_status_t  bt_cm_force_disconnect(bt_bd_addr_t * remote_dev)
{
    bt_cm_remote_device_t *device_p = NULL;
    bt_cmgr_report_id("[BT_CM][I] To force disconnect the remote device", 0);
    if (NULL != remote_dev && NULL == (device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)remote_dev))) {
        bt_cmgr_report_id("[BT_CM][E] Can't find the remote device", 0);
        return BT_STATUS_FAIL;
    }
    if (NULL != device_p) {
        if (BT_CM_ACL_LINK_DISCONNECTED != device_p->link_info.link_state) {
            return bt_gap_disconnect_ext(device_p->link_info.handle, 1000);
        }
        return BT_STATUS_FAIL;
    } else {
        bt_bd_addr_t* local_addr = bt_device_manager_get_local_address();
        bt_cm_remote_device_t *special_device_p = NULL;
        for (uint32_t i = 0; i < g_bt_cm_cfg->max_connection_num + 1; ++i) {
            device_p = ((i == g_bt_cm_cfg->max_connection_num) ? special_device_p : &(g_bt_cm_cnt->devices_list[i]));
            if (NULL == special_device_p && NULL != device_p && !bt_cm_memcmp(local_addr, &(device_p->link_info.addr), sizeof(bt_bd_addr_t))) {
                special_device_p = device_p;
                continue;
            }
            if (NULL != device_p && BT_CM_ACL_LINK_DISCONNECTED != device_p->link_info.link_state) {
                if (BT_STATUS_SUCCESS == bt_gap_disconnect_ext(device_p->link_info.handle, 1000)) {
                    return BT_STATUS_SUCCESS;
                }
            }
        }
    }
    return BT_STATUS_FAIL;
}

bt_status_t         bt_cm_prepare_power_deinit(bool force, bt_device_manager_power_status_t reason)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, BT_STATUS_SUCCESS, BT_CM_LOG_CONTEXT_NULL, 0);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cfg, BT_STATUS_SUCCESS, BT_CM_LOG_CONFIG_NULL, 0);
    bool wait_disconnect = false;
    bt_cmgr_report_id("[BT_CM][I] Power deinit force %d", 1, force);
    g_bt_cm_cnt->flags &= (~BT_CM_FLAG_INITIATED);
    if (true == force) {
        return bt_cm_force_disconnect(NULL);
    }
    for (uint32_t i = 0; i < g_bt_cm_cfg->max_connection_num; ++i) {
        if (BT_CM_ACL_LINK_CONNECTING <= g_bt_cm_cnt->devices_list[i].link_info.link_state) {
            if (BT_DEVICE_MANAGER_POWER_STATUS_ROLE_RECOVERY == reason &&
                false == bt_cm_is_specail_device(&(g_bt_cm_cnt->devices_list[i].link_info.addr))) {
                g_bt_cm_cnt->devices_list[i].flags |= BT_CM_REMOTE_FLAG_RESERVE_DUE_TO_ROLE_RECOVERY;
            }
            if (!(g_bt_cm_cnt->devices_list[i].flags & BT_CM_REMOTE_FLAG_PENDING_DISCONNECT)) {
                bt_cm_connect_t disconnect_dev = {
                    .profile = BT_CM_PROFILE_SERVICE_MASK_ALL
                };
                bt_cm_memcpy(&(disconnect_dev.address), &(g_bt_cm_cnt->devices_list[i].link_info.addr), sizeof(bt_bd_addr_t));
                bt_cm_disconnect(&disconnect_dev);
            }
            wait_disconnect = true;
        } else if (BT_CM_ACL_LINK_DISCONNECTING == g_bt_cm_cnt->devices_list[i].link_info.link_state) {
            wait_disconnect = true;
        } else if (g_bt_cm_cnt->devices_list[i].flags & BT_CM_REMOTE_FLAG_PASSIV_CONNECTING) {
            g_bt_cm_cnt->devices_list[i].link_info.link_state = BT_CM_ACL_LINK_DISCONNECTING;
            g_bt_cm_cnt->devices_list[i].flags &= ~(BT_CM_REMOTE_FLAG_PASSIV_CONNECTING);
            wait_disconnect = true;
        }
    }
    if (wait_disconnect == true) {
        return BT_CM_STATUS_PENDING;
    }
    bt_cm_timer_stop(BT_CM_SWITCH_ROLE_TIMER_ID);
    bt_cm_timer_stop(BT_CM_CONNECTION_TIMER_ID);
    bt_cm_timer_stop(BT_CM_DELAY_RECONNECT_TIMER_ID);
    return BT_STATUS_SUCCESS;
}
void bt_cm_aws_connected_callback()
{    
    bt_cmgr_report_id("[BT_CM][I] aws connect timeout after disconn!!", 0);
    bt_cm_connection_state_update(NULL);

    return;
}
bt_status_t         bt_cm_connect(const bt_cm_connect_t *param)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == param, BT_CM_STATUS_INVALID_PARAM, "[BT_CM][E] Connect param is NULL", 0);
    bt_device_manager_power_state_t power_state = bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(BT_DEVICE_MANAGER_POWER_STATE_STANDBY == power_state || BT_DEVICE_MANAGER_POWER_STATE_STANDBY_PENDING == power_state,
        BT_CM_STATUS_FAIL, "[BT_CM][E] Current state not in power on", 0);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(BT_CM_PROFILE_SERVICE_MASK_NONE == param->profile, BT_CM_STATUS_INVALID_PARAM, "[BT_CM][E] Connect profile is None", 0);
    bt_cmgr_report_id("[BT_CM][I] Connect address 0x:%02x:%02x:%02x:%02x:%02x:%02x, request profile 0x%04x", 7,
        param->address[5], param->address[4], param->address[3], param->address[2], param->address[1], param->address[0], param->profile);
    bt_cm_mutex_lock();

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    if ((BT_AWS_MCE_ROLE_PARTNER | BT_AWS_MCE_ROLE_CLINET) & aws_role) {
        bt_bd_addr_t* local_addr = bt_device_manager_get_local_address();
        bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)local_addr);
        if (NULL != device_p && BT_CM_ACL_LINK_CONNECTED <= device_p->link_info.link_state) {
        /* If partner got the state that agent had connected with SP, then partner can't connect any other device. */
            bt_cm_mutex_unlock();
            return BT_STATUS_SUCCESS;
        }
    }
#endif

    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, &(param->address));
    bt_cm_profile_service_mask_t need_excute_profile = param->profile;
    if (NULL == device_p) {
        bt_bd_addr_t temp_addr = {0};
        device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)temp_addr);
        if (NULL == device_p) {
            bt_cm_mutex_unlock();
            bt_cmgr_report_id("[BT_CM][E] Connection already maximum", 0);
            return BT_CM_STATUS_MAX_LINK;
        }
        bt_cm_memset(device_p, 0, sizeof(*device_p));
        bt_cm_memcpy(device_p->link_info.addr, param->address, sizeof(bt_bd_addr_t));
        device_p->link_info.link_state = BT_CM_ACL_LINK_DISCONNECTED;
    }
    if (device_p->flags & BT_CM_REMOTE_FLAG_PENDING_DISCONNECT) {
        bt_cm_mutex_unlock();
        bt_cmgr_report_id("[BT_CM][E] Connection is pending to disconnect", 0);
        return BT_CM_STATUS_INVALID_STATUS;
    }
    BT_CM_LOG_CONNECTION_STATUS(device_p);
    /* notify will do connect */
    bt_cm_register_callback_notify(BT_CM_EVENT_PRE_CONNECT, &(device_p->link_info.addr), sizeof(device_p->link_info.addr));
    /* 1, Remask the connected and connecting profile. */
    need_excute_profile &= (~(device_p->link_info.connected_mask | device_p->link_info.connecting_mask));
	need_excute_profile |= (param->profile & device_p->link_info.disconnecting_mask);
    if (BT_CM_PROFILE_SERVICE_MASK_NONE == need_excute_profile) {
        bt_cm_mutex_unlock();
        bt_cmgr_report_id("[BT_CM][I] All request profile is connecting or connected, connecting:0x%04x, connected:0x%04x",
            2, device_p->link_info.connecting_mask, device_p->link_info.connected_mask);
        return BT_STATUS_SUCCESS;
    }
    /* 2, Cancel the request disconnect mask. */
    device_p->request_disconnect_mask &= (~need_excute_profile);
    /* 3, Add the connect profile to request connect mask. */
    device_p->request_connect_mask |= need_excute_profile;
    /* 4, Add new link request to connecting list. */
    if (device_p->link_info.link_state == BT_CM_ACL_LINK_DISCONNECTED) {
        device_p->link_info.link_state = BT_CM_ACL_LINK_PENDING_CONNECT;
        bt_cm_list_add(BT_CM_LIST_CONNECTING, device_p, BT_CM_LIST_ADD_BACK);
    }
    /* 5, Record the expected connect mask. */
    device_p->expected_connect_mask |= param->profile;
    BT_CM_LOG_CONNECTION_STATUS(device_p);
#ifdef MTK_AWS_MCE_ENABLE
    for (uint32_t i = 0; i < g_bt_cm_cfg->max_connection_num; ++i) {
        if ((g_bt_cm_cnt->devices_list[i].flags & BT_CM_REMOTE_FLAG_PENDING_DISCONNECT) && BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
            bt_cmgr_report_id("[BT_CM][I] there is a disconnecting device, aws will disconnect", 0);
            bt_cm_timer_start(BT_CM_DIS_CONN_WAIT_AWS_TIMER_ID, 3000, bt_cm_aws_connected_callback, NULL); 
        }
    }
#endif 
    bt_cm_timer_start(BT_CM_CONNECTION_TIMER_ID, 1, bt_cm_connection_state_update, NULL);
    bt_cm_mutex_unlock();
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_cm_disconnect(const bt_cm_connect_t *param)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == param, BT_CM_STATUS_INVALID_PARAM, "[BT_CM][E] Disconnect param is NULL", 0);
    bt_cmgr_report_id("[BT_CM][I] Disconnect address 0x:%02x:%02x:%02x:%02x:%02x:%02x, request profile 0x%04x", 7,
        param->address[5], param->address[4], param->address[3], param->address[2], param->address[1], param->address[0], param->profile);

    bt_bd_addr_t temp_addr = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    bt_cm_remote_device_t *device_p = NULL;
    bt_cm_profile_service_mask_t need_excute_profile = param->profile;
    if (bt_cm_memcmp(param->address, temp_addr, sizeof(bt_bd_addr_t))) {
        BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == (device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, &(param->address))),
            BT_CM_STATUS_NOT_FOUND, "[BT_CM][E] Disconnect device not found", 0);
    }
    bt_cm_mutex_lock();
    for (uint32_t i = 0; i < g_bt_cm_cfg->max_connection_num + 1; ++i) {
        if (i > 0) {
            device_p = &g_bt_cm_cnt->devices_list[i - 1];
        }
        if (NULL == device_p) {
            continue;
        }
        if (BT_CM_PROFILE_SERVICE_MASK_ALL == param->profile) {
            device_p->flags |= BT_CM_REMOTE_FLAG_PENDING_DISCONNECT;
        }
        need_excute_profile = param->profile;
        BT_CM_LOG_CONNECTION_STATUS(device_p);
        /* 1, Cancle the request connect mask. */
        device_p->request_connect_mask &= (~need_excute_profile);
        /* 2, Remask the disconnecting profile. */
        need_excute_profile &= (~device_p->link_info.disconnecting_mask);
        /* 3, Remask the profile which is not in connecting or connected status. */
        need_excute_profile &= (device_p->link_info.connecting_mask | device_p->link_info.connected_mask);
        /* 4, Add the disconnect profile to request disconnect mask. */
        device_p->request_disconnect_mask |= need_excute_profile;
        BT_CM_LOG_CONNECTION_STATUS(device_p);
        if (i == 0) {
            break;
        }
    }
    bt_cm_timer_start(BT_CM_CONNECTION_TIMER_ID, 1, bt_cm_connection_state_update, NULL);
    //bt_cm_connection_state_update(NULL);
    bt_cm_mutex_unlock();
    if (bt_cm_list_is_empty(BT_CM_LIST_CONNECTING) && bt_cm_list_is_empty(BT_CM_LIST_CONNECTED)) {
        return BT_CM_STATUS_NOT_FOUND;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_cm_cancel_connect(bt_bd_addr_t *addr)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, BT_STATUS_SUCCESS, BT_CM_LOG_CONTEXT_NULL, 0);
    bt_cmgr_report_id("[BT_CM][I] Cancel connect", 0);
    bt_cm_remote_device_t *device_p = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTING];
    while (NULL != device_p) {
        if (NULL == addr || !bt_cm_memcmp(&(device_p->link_info.addr), addr, sizeof(bt_bd_addr_t))) {
            bt_cm_connect_t disconn_param;
            bt_cm_memcpy(&(disconn_param.address), &(device_p->link_info.addr), sizeof(bt_bd_addr_t));
            disconn_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
            bt_cm_disconnect((void *)&disconn_param);
        }
        device_p = device_p->next[BT_CM_LIST_CONNECTING];
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_cm_disconnect_normal_first()
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, BT_STATUS_SUCCESS, BT_CM_LOG_CONTEXT_NULL, 0);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cfg, BT_STATUS_SUCCESS, BT_CM_LOG_CONFIG_NULL, 0);

    bt_cmgr_report_id("[BT_CM][I] disconnect_normal_first", 0);

    bt_cm_remote_device_t *disconn_dev = NULL;
    bt_bd_addr_t* local_addr = bt_device_manager_get_local_address();
    for (uint32_t i = 0; i < g_bt_cm_cfg->max_connection_num; ++i) {
        if (BT_CM_ACL_LINK_CONNECTED <= g_bt_cm_cnt->devices_list[i].link_info.link_state ||
            BT_CM_ACL_LINK_CONNECTING == g_bt_cm_cnt->devices_list[i].link_info.link_state ||
            BT_CM_ACL_LINK_DISCONNECTING == g_bt_cm_cnt->devices_list[i].link_info.link_state) {
            disconn_dev = &(g_bt_cm_cnt->devices_list[i]);
            if (bt_cm_memcmp(local_addr, &(g_bt_cm_cnt->devices_list[i].link_info.addr), sizeof(bt_bd_addr_t))) {
                break;
            } else {
                continue;
            }
        }
    }
    if (NULL != disconn_dev) {
        bt_cmgr_report_id("[BT_CM][I] disconnect_normal_first block device 0x%x", 1, *(uint32_t *)&(disconn_dev->link_info.addr));
        bt_cm_connect_t disconnect_dev = {
            .profile = BT_CM_PROFILE_SERVICE_MASK_ALL
        };
        bt_cm_memcpy(&(disconnect_dev.address), &(disconn_dev->link_info.addr), sizeof(bt_bd_addr_t));
        bt_cm_disconnect(&disconnect_dev);
        return BT_CM_STATUS_PENDING;
    }

    return BT_STATUS_SUCCESS;
}

uint32_t            bt_cm_get_connecting_devices(bt_cm_profile_service_mask_t profiles, bt_bd_addr_t *addr_list, uint32_t list_num)
{
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, 0, BT_CM_LOG_CONTEXT_NULL, 0);
    uint32_t count = 0;
    bt_cm_remote_device_t *device_p = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTING];

    while (NULL != device_p) {
        if (BT_CM_PROFILE_SERVICE_MASK_NONE == profiles ||
            0 != (profiles & (device_p->request_connect_mask | device_p->link_info.connecting_mask))) {
            count++;
            if (addr_list != NULL && 0 != list_num) {
                bt_cm_memcpy(&(addr_list[count - 1]), device_p->link_info.addr, sizeof(bt_bd_addr_t));
                if (count == list_num) {
                    break;
                }
            }
        }
        device_p = device_p->next[BT_CM_LIST_CONNECTING];
    }
    bt_cmgr_report_id("[BT_CM][I] Connecting count:%d", 1, count);
    return count;
}

bt_status_t         bt_cm_register_event_callback(bt_cm_event_handle_callback_t cb)
{
    uint8_t idx = 0;
    for (idx = 0; idx < BT_CM_MAX_CALLBACK_NUMBER; idx++) {
        if (g_bt_cm_cnt->callback_list[idx] == cb) {
            return BT_STATUS_FAIL;
        }
    }

    for (idx = 0; idx < BT_CM_MAX_CALLBACK_NUMBER; idx++) {
        if (g_bt_cm_cnt->callback_list[idx] == NULL) {
            g_bt_cm_cnt->callback_list[idx] = cb;
            return BT_STATUS_SUCCESS;
        }
    }

    return BT_STATUS_FAIL;
}

void                bt_cm_register_callback_notify(bt_cm_event_t event_id, void *params, uint32_t params_len)
{
    uint8_t idx = 0;
    for (idx = 0; idx < BT_CM_MAX_CALLBACK_NUMBER; idx++) {
        if (g_bt_cm_cnt->callback_list[idx]) {
            g_bt_cm_cnt->callback_list[idx](event_id, params, params_len);
        }
    }
}

void                bt_cm_init(const bt_cm_config_t *config)
{
    bt_cmgr_report_id("[BT_CM][I] Init", 0);
    BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == config, "[BT_CM][E] Config param is null", 0);
    g_bt_cm_cfg = (bt_cm_config_t *)config;
    uint32_t support_device = g_bt_cm_cfg->max_connection_num ? g_bt_cm_cfg->max_connection_num : 1;
    if (NULL != g_bt_cm_cnt && g_bt_cm_cnt->devices_buffer_num < g_bt_cm_cfg->max_connection_num) {
        bt_cm_memory_free(g_bt_cm_cnt);
        g_bt_cm_cnt = NULL;
    }
    if (NULL == g_bt_cm_cnt) {
        g_bt_cm_cnt = bt_cm_memory_alloc(sizeof(bt_cm_cnt_t) +
            ((support_device - 1) * sizeof(bt_cm_remote_device_t)));
        BT_CM_CHECK_RET_NO_VALUE_WITH_LOG(NULL == g_bt_cm_cnt, "[BT_CM][E] Context memory allocate fail", 0);
        bt_cm_memset(g_bt_cm_cnt, 0, sizeof(bt_cm_cnt_t) + ((support_device - 1) * sizeof(bt_cm_remote_device_t)));
        g_bt_cm_cnt->devices_buffer_num = support_device;
    }
    g_bt_cm_cnt->max_connection_num = g_bt_cm_cfg->max_connection_num;
    bt_cm_power_init();
    bt_cm_atci_init();
    bt_cm_timer_init();
    bt_callback_manager_register_callback(bt_callback_type_app_event, (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_SDP),
        (void *)bt_cm_common_callback);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_srv_init();
#endif
}

void                bt_cm_deinit()
{
    bt_cmgr_report_id("[BT_CM][I] Deinit", 0);
    bt_cm_power_deinit();
    bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_cm_common_callback);
}

bt_status_t bt_cm_set_sniff_parameters(const bt_gap_default_sniff_params_t* params)
{
    if(NULL == params){
        return BT_STATUS_FAIL;
    }
    return bt_gap_set_default_sniff_parameters(params);
}

bt_status_t bt_cm_reset_sniff_parameters()
{
    return bt_gap_set_default_sniff_parameters(&g_bt_cm_default_sniff_params);
}

uint32_t    bt_cm_get_disconnected_gpt_count(bt_bd_addr_t addr)
{
    extern uint32_t bt_avm_get_gpt_count(bt_bd_addr_t addr);
    return bt_avm_get_gpt_count(addr);
}


void        bt_cm_clear_disconnected_gpt_count(bt_bd_addr_t addr)
{
    extern void bt_avm_clear_gpt_count(bt_bd_addr_t addr);
    return bt_avm_clear_gpt_count(addr);
}

void        bt_cm_unlock_bt_sleep_by_VP(void)
{
    extern void bt_avm_unlock_bt_sleep(void);
    bt_avm_unlock_bt_sleep();
}

bt_status_t bt_cm_reconn_is_allow(bool is_allow, bool is_initiate_connect)
{
    bt_cm_is_reconnect_flag = is_allow;
    if (is_initiate_connect) {
        bt_cm_timer_start(BT_CM_CONNECTION_TIMER_ID, 1, bt_cm_connection_state_update, NULL);
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_cm_set_max_connection_number(uint8_t number, bt_bd_addr_t *keep_list, uint8_t list_size, bool if_recon)
{
#if 0
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cnt, BT_STATUS_SUCCESS, BT_CM_LOG_CONTEXT_NULL, 0);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(NULL == g_bt_cm_cfg, BT_STATUS_SUCCESS, BT_CM_LOG_CONFIG_NULL, 0);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
#else
    bt_aws_mce_role_t aws_role = BT_AWS_MCE_ROLE_NONE;
#endif
    uint8_t req_num = number + !!(g_bt_cm_cfg->connection_takeover) + !!(BT_AWS_MCE_ROLE_NONE != aws_role);
    bt_cmgr_report_id("[BT_CM][I] Set max connection number:%d, req:%d, cur:%d, list size:%d", 4,
        number, req_num, g_bt_cm_cnt->max_connection_num, list_size);
    BT_CM_CHECK_RET_WITH_VALUE_AND_LOG(req_num > g_bt_cm_cfg->max_connection_num, BT_STATUS_FAIL,
        "[BT_CM][E] Set max connection exceed, max:%d", 1, g_bt_cm_cfg->max_connection_num);
    if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC) ||
        req_num == g_bt_cm_cnt->max_connection_num || (BT_AWS_MCE_ROLE_NONE != aws_role && BT_AWS_MCE_ROLE_AGENT != aws_role)) {
        g_bt_cm_cnt->max_connection_num = req_num;
        return BT_STATUS_SUCCESS;
    }
    if (req_num > g_bt_cm_cnt->max_connection_num) {
        g_bt_cm_cnt->max_connection_num = req_num;
        bt_cm_connect_t remote_device;
        bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
        for (uint8_t index = 1; index <= number; index++) {
            bt_bd_addr_t *remote = bt_device_manager_remote_get_dev_by_seq_num(index);
            if (NULL == remote) {
                break;
            }
            bt_cm_memcpy(&remote_device.address, remote, sizeof(bt_bd_addr_t));
            remote_device.profile = g_bt_cm_cfg->power_on_reconnect_profile & ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS));
            bt_cm_connect(&remote_device);
        }
    } else {
        uint8_t use_number = 0;
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
        for (uint8_t index = 0; index < g_bt_cm_cfg->max_connection_num; index++) {
            if (BT_CM_ACL_LINK_PENDING_CONNECT <= g_bt_cm_cfg->devices_list[index].link_info.link_state) {
                use_number++;
            }
        }
        if (req_num < use_number) {

        }
        for (uint8_t index = 0; index < g_bt_cm_cfg->max_connection_num; index++) {
            if (BT_CM_ACL_LINK_PENDING_CONNECT <= g_bt_cm_cfg->devices_list[index].link_info.link_state) {
                use_number++;
                if (bt_cm_memcmp(local_addr, g_bt_cm_cfg->devices_list[index].link_info.addr, sizeof(bt_bd_addr_t))) {
                    for (uint8_t i = 0; i < list_size; i++) {
                        if (bt_cm_memcmp(keep_list[i], g_bt_cm_cfg->devices_list[index].link_info.addr, sizeof(bt_bd_addr_t))) {

                        }
                    }
                }
            }
            
            if (BT_CM_ACL_LINK_CONNECTED <= g_bt_cm_cnt->devices_list[i].link_info.link_state ||
                BT_CM_ACL_LINK_CONNECTING == g_bt_cm_cnt->devices_list[i].link_info.link_state ||
                BT_CM_ACL_LINK_DISCONNECTING == g_bt_cm_cnt->devices_list[i].link_info.link_state) {
                disconn_dev = &(g_bt_cm_cnt->devices_list[i]);
                if (bt_cm_memcmp(local_addr, &(g_bt_cm_cnt->devices_list[i].link_info.addr), sizeof(bt_bd_addr_t))) {
                    break;
                } else {
                    continue;
                }
            }
         }
    }
            
 #endif
    
    if (number > g_bt_cm_cfg->max_connection_num) {
        bt_cmgr_report_id("[BT_CM][I] set max connection num fail due to number bigger or list_size error", 0);
        return BT_STATUS_FAIL;
    }
    if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
        g_bt_cm_cnt->max_connection_num = number;
        bt_cmgr_report_id("[BT_CM][I] set max connectiOn num due to power on", 0);
        return BT_STATUS_SUCCESS;

    }
    bt_cmgr_report_id("[BT_CM][I] set max connection number to %d,list_size is %d", 2, number, list_size);
    bt_cmgr_report_id("[BT_CM][I] set max connection number keep_list is:%02x:%02x:%02x:%02x:%02x:%02x", 6,
    (*keep_list)[5], (*keep_list)[4], (*keep_list)[3], (*keep_list)[2], (*keep_list)[1], (*keep_list)[0]);
    bt_bd_addr_t *local_addr= bt_device_manager_get_local_address();
    uint8_t discon_num;
    uint8_t discon_count = 1;
    uint8_t keep_num;
    bt_cm_connect_t remote_device;
    uint8_t index;
    bt_bd_addr_t connecting_device;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
#else
    bt_aws_mce_role_t aws_role = BT_AWS_MCE_ROLE_NONE;
#endif
    if (BT_AWS_MCE_ROLE_PARTNER != aws_role) {
        if (g_bt_cm_cnt->max_connection_num > number) {
            if (bt_cm_get_connecting_devices(BT_CM_PROFILE_SERVICE_MASK_ALL, NULL, 0) + bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_ALL, NULL, 0) + 
                (BT_AWS_MCE_ROLE_AGENT == aws_role ? 1 : 0) + (g_bt_cm_cfg->connection_takeover ? 1 : 0)> number) {
                index = (bt_cm_get_connecting_devices(BT_CM_PROFILE_SERVICE_MASK_ALL, NULL, 0) + bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_ALL, NULL, 0) + 
                    (BT_AWS_MCE_ROLE_AGENT == aws_role ? 1 : 0) + (g_bt_cm_cfg->connection_takeover ? 1 : 0)) - number;
                if (0 == list_size) {
                    if (bt_cm_get_connecting_devices(BT_CM_PROFILE_SERVICE_MASK_ALL, &connecting_device, 0)) {
                        bt_cm_cancel_connect(&connecting_device);
                        discon_count++;
                    }
                    for (; discon_count <= index; discon_count++) {
                        bt_cm_memcpy(&remote_device.address, bt_device_manager_remote_get_dev_by_seq_num(1), sizeof(bt_bd_addr_t));
                        remote_device.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                        bt_cm_disconnect(&remote_device);
                    }
                } else {
                    for (keep_num = 1; keep_num <= list_size; keep_num++, keep_list++) {
                        for (discon_num = 0; discon_num < g_bt_cm_cnt->max_connection_num; ++discon_num) {
                            if (BT_CM_ACL_LINK_CONNECTED <= g_bt_cm_cnt->devices_list[discon_num].link_info.link_state ||
                                BT_CM_ACL_LINK_CONNECTING == g_bt_cm_cnt->devices_list[discon_num].link_info.link_state) {
                                if (!bt_cm_memcmp(keep_list, g_bt_cm_cnt->devices_list[discon_num].link_info.addr, sizeof(bt_bd_addr_t)) ||
                                    !bt_cm_memcmp(local_addr, g_bt_cm_cnt->devices_list[discon_num].link_info.addr, sizeof(bt_bd_addr_t))) {
                                    continue;
                                }
                                remote_device.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                                bt_cm_memcpy(&remote_device.address, &(g_bt_cm_cnt->devices_list[discon_num].link_info.addr), sizeof(bt_bd_addr_t));
                                bt_cm_disconnect(&remote_device);
                                bt_cmgr_report_id("[BT_CM][I] disconnect device due to max connection num changed", 0);
                                discon_count++;
                                if (discon_count == index) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
            g_bt_cm_cnt->max_connection_num = number;
            bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW,BT_CM_COMMON_TYPE_ENABLE);
        } else if (g_bt_cm_cnt->max_connection_num < number){
            g_bt_cm_cnt->max_connection_num = number;
            bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW,BT_CM_COMMON_TYPE_ENABLE);
            if (if_recon) {
                bt_bd_addr_t connected_addr;
                bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &connected_addr, 1);
                for (index = 1; index <= 2; index++) {
                    if (NULL == bt_device_manager_remote_get_dev_by_seq_num(index)) {
                        break;
                    } else {
                        if (bt_cm_memcmp(&connected_addr, bt_device_manager_remote_get_dev_by_seq_num(index), sizeof(bt_bd_addr_t))) {
                            bt_cm_memcpy(&remote_device.address, bt_device_manager_remote_get_dev_by_seq_num(index), sizeof(bt_bd_addr_t));
                            remote_device.profile = g_bt_cm_cfg->power_on_reconnect_profile & ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS));
                            bt_cmgr_report_id("[BT_CM][I] set max connection num reconnect addr is:%02x:%02x:%02x:%02x:%02x:%02x", 6,
                                (connected_addr)[5], (connected_addr)[4], (connected_addr)[3],
                                (connected_addr)[2], (connected_addr)[1], (connected_addr)[0]);
                            bt_cm_connect(&remote_device);
                            break;
                        }
                    }
                }
            }
        }
    } else {
        g_bt_cm_cnt->max_connection_num = number;
    }
    return BT_STATUS_SUCCESS;
}

bool bt_cm_is_disconnecting_aws_device(bt_bd_addr_t device_addr)
{
    bt_cm_remote_device_t *device_p = bt_cm_find_device(BT_CM_FIND_BY_ADDR, device_addr);
    if(NULL == device_p){
        return NULL;
    }
    if (device_p->link_info.disconnecting_mask & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)) {
        return true;
    }
    return false;
}

void bt_cm_cancel_connect_timeout_callback(void *param)
{
    bt_cm_assert(0 && "cancel connect timeout!!");
    return;
}
