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
#include "bt_gap_le.h"
#include "bt_connection_manager_internal.h"
#include "bt_connection_manager_utils.h"
#include "bt_device_manager.h"
#include "bt_connection_manager.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_power.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"

typedef struct {
    bt_cm_profile_service_mask_t    profile_connection_mask;
    uint8_t                         aws_ready;
#ifdef AIR_MULTI_POINT_ENABLE
    bt_cm_connect_t connecting_device;
#endif
} bt_cm_rho_context_t;

bt_cm_rho_prepare_wait_flag_t   g_bt_cm_rho_flags_t;
bt_cm_rho_context_t bt_cm_rho_context;

static bt_status_t  bt_cm_rho_is_allowed(const bt_bd_addr_t *addr)
{
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_cm_profile_service_mask_t connected_mask = bt_cm_get_connected_profile_services((void *)addr);
    if (BT_AWS_MCE_ROLE_AGENT != aws_role) {
        bt_cmgr_report_id("[BT_CM][RHO][W] Not allowed due to role is :0x%02x", 1, aws_role);
        return BT_STATUS_FAIL;
    }
    if (!(connected_mask & (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)))) {
        bt_cmgr_report_id("[BT_CM][RHO][W] Not allowed due to partner not be attched on normal link", 0);    
        return BT_STATUS_FAIL;
    }
    if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
        bt_cmgr_report_id("[BT_CM][RHO][W] Not allowed due to power state is not active", 0);
        return BT_STATUS_FAIL;
    }
    return BT_STATUS_PENDING;
}

static uint8_t      bt_cm_rho_get_length(const bt_bd_addr_t *addr)
{
    return ((NULL == addr) ? 0 : sizeof(bt_cm_rho_context_t));
}

static bt_status_t  bt_cm_rho_get_data(const bt_bd_addr_t *addr, void * data)
{
    bt_cm_rho_context_t     *rho_context = (bt_cm_rho_context_t *)data;
    bt_cm_remote_device_t   *remote_device = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)addr);
    bt_cm_assert(NULL != remote_device && "RHO can't find remote device !!!");
    rho_context->profile_connection_mask = remote_device->link_info.connected_mask;
#ifdef AIR_MULTI_POINT_ENABLE
    bt_cm_memcpy(&(rho_context->connecting_device), &(bt_cm_rho_context.connecting_device), sizeof(bt_cm_connect_t));
#endif
    rho_context->aws_ready = bt_aws_mce_srv_rho_get_aws_ready((void *)addr);
    bt_cm_memset(&bt_cm_rho_context, 0, sizeof(bt_cm_rho_context_t));
    return BT_STATUS_SUCCESS;
}

extern void         bt_cm_list_add(bt_cm_list_t list_type, bt_cm_remote_device_t *device, bt_cm_list_add_t add_type);
extern void         bt_cm_list_remove(bt_cm_list_t list_type, bt_cm_remote_device_t *device);

static bt_status_t  bt_cm_rho_update(bt_role_handover_update_info_t *info)
{
    bt_bd_addr_t            aws_addr = {0};
    if (!bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &aws_addr, 1)) {
        bt_cm_assert(0 && "RHO get data but no aws device exist !!!");
    }
    bt_bd_addr_t            local_addr = {0};
    bt_bd_addr_t            *peer_addr = bt_device_manager_aws_local_info_get_peer_address();
    bt_cm_remote_device_t   *aws_device = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)&aws_addr);
    bt_cm_remote_device_t   *saws_device = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)bt_device_manager_get_local_address());
    bt_cm_assert(NULL != info && (BT_AWS_MCE_ROLE_AGENT == info->role || BT_AWS_MCE_ROLE_PARTNER == info->role));
    bt_cmgr_report_id("[BT_CM][RHO][I] Update role: 0x%02x, addr:0x%x, data length %d, active:%d", 4,
        info->role, *(uint32_t *)(info->addr), info->length, info->is_active);
    bt_cm_assert(NULL != aws_device && NULL != saws_device && "RHO update context no aws device exist");
    bt_cm_memcpy((void*)&local_addr, (void*)bt_device_manager_get_local_address(), sizeof(bt_bd_addr_t));
    saws_device->link_info.local_role = BT_ROLE_SLAVE;
    uint8_t aws_ready = true;
    if (BT_AWS_MCE_ROLE_AGENT == info->role) {
        extern bt_cm_cnt_t     *g_bt_cm_cnt;
        extern bt_cm_config_t  *g_bt_cm_cfg;
        bt_cmgr_report_id("[BT_CM][RHO][I] Agent switch context to partner role", 0);
        bt_cm_memcpy((void*)&aws_device->link_info.addr, &local_addr, sizeof(bt_bd_addr_t));
        aws_device->link_info.connected_mask = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
        aws_device->link_info.local_role = BT_ROLE_MASTER;
        for (uint32_t i = 0; i < g_bt_cm_cfg->max_connection_num; ++i) {
            if (saws_device != &(g_bt_cm_cnt->devices_list[i]) && aws_device != &(g_bt_cm_cnt->devices_list[i])) {
                bt_cm_list_remove(BT_CM_LIST_CONNECTING, &(g_bt_cm_cnt->devices_list[i]));
                bt_cm_list_remove(BT_CM_LIST_CONNECTED, &(g_bt_cm_cnt->devices_list[i]));
                bt_cm_memset(&g_bt_cm_cnt->devices_list[i], 0, sizeof(bt_cm_remote_device_t));
            }
        }
        bt_cm_memcpy((void*)&saws_device->link_info.addr, peer_addr, sizeof(bt_bd_addr_t));
        bt_device_manager_aws_local_info_store_local_address(peer_addr);
        bt_device_manager_aws_local_info_store_peer_address(&local_addr);
        bt_device_manager_aws_local_info_store_role(BT_AWS_MCE_ROLE_AGENT == info->role ?
            BT_AWS_MCE_ROLE_PARTNER : BT_AWS_MCE_ROLE_AGENT);
    } else {
        bt_cm_rho_context_t *rho_context = (bt_cm_rho_context_t *)info->data;
        bt_cm_assert(NULL != rho_context && "RHO update to agent data is null");
        aws_ready = rho_context->aws_ready;
        bt_cmgr_report_id("[BT_CM][RHO][I] Partner switch context to agent role data is : 0x%x, aws_ready : %d", 2, rho_context->profile_connection_mask, aws_ready);
        if (info->is_active) {
            bt_cm_memcpy((void*)&aws_device->link_info.addr, (void*)info->addr, sizeof(bt_bd_addr_t));
            aws_device->link_info.connected_mask = rho_context->profile_connection_mask;
            aws_device->link_info.local_role = BT_ROLE_SLAVE;
            aws_device->link_info.link_state = BT_CM_ACL_LINK_ENCRYPTED;
            bt_cm_memcpy((void*)&saws_device->link_info.addr, peer_addr, sizeof(bt_bd_addr_t));
            bt_device_manager_aws_local_info_store_local_address(peer_addr);
            bt_device_manager_aws_local_info_store_peer_address(&local_addr);
            bt_device_manager_aws_local_info_store_role(BT_AWS_MCE_ROLE_AGENT == info->role ?
                BT_AWS_MCE_ROLE_PARTNER : BT_AWS_MCE_ROLE_AGENT);
        } else {
            bt_bd_addr_t            non_aws_addr = {0};
            bt_cm_remote_device_t   *non_aws_device = bt_cm_find_device(BT_CM_FIND_BY_ADDR, (void *)&non_aws_addr);
            bt_cm_assert(NULL != non_aws_device && "Can't found free device");
            bt_cm_memcpy((void*)&(non_aws_device->link_info.addr), (void*)(info->addr), sizeof(bt_bd_addr_t));
            non_aws_device->link_info.handle = bt_gap_get_handle_by_address((void*)info->addr);
            non_aws_device->link_info.connected_mask = rho_context->profile_connection_mask;
            non_aws_device->link_info.local_role = BT_ROLE_SLAVE;
            non_aws_device->link_info.link_state = BT_CM_ACL_LINK_ENCRYPTED;
            bt_cm_list_add(BT_CM_LIST_CONNECTED, non_aws_device, BT_CM_LIST_ADD_FRONT);
        }
#ifdef AIR_MULTI_POINT_ENABLE
        bt_cm_connect_t remote_device;
        if (rho_context->connecting_device.profile != 0) {
            bt_cm_memcpy(&(remote_device.address), &(rho_context->connecting_device.address), sizeof(bt_bd_addr_t));
            remote_device.profile = rho_context->connecting_device.profile;
            bt_cmgr_report_id("[BT_CM][RHO][I] Partner switch context to agent connect the canceled device", 0);
            bt_cm_connect(&remote_device);
            bt_cm_memset(&bt_cm_rho_context, 0, sizeof(bt_cm_rho_context_t));
        }
#endif
    }
    bt_aws_mce_srv_rho_complete((void *)info->addr, info->is_active, aws_ready);
    return BT_STATUS_SUCCESS;
}

void                bt_cm_rho_gap_event_handle(bt_msg_type_t msg, bt_status_t status, void *buffer)
{    
    bt_cmgr_report_id("[BT_CM][RHO][I] start cm rho gap event handle msg:0x%02x, status:0x%02x, g_bt_cm_rho_flags_t is %d", 3, msg, status, g_bt_cm_rho_flags_t);
    switch (msg) {
        case BT_GAP_WRITE_LINK_POLICY_CNF: {
            g_bt_cm_rho_flags_t &= (~BT_CM_RHO_PREPARE_WAIT_FLAG_LINK_POLICY);
        }
            break;
        case BT_GAP_SNIFF_MODE_CHANGE_IND: {
            //bt_cm_remote_device_t *rem_dev = NULL;
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                bt_gap_sniff_mode_changed_ind_t* ind = (bt_gap_sniff_mode_changed_ind_t *)buffer;
                if (NULL != ind && (BT_GAP_LINK_SNIFF_TYPE_ACTIVE == ind->sniff_status)) {
                    g_bt_cm_rho_flags_t &= (~BT_CM_RHO_PREPARE_WAIT_FLAG_EXIT_SNIFF);
                }
            }
        }
            break;
        case BT_GAP_SET_SCAN_MODE_CNF: {
            g_bt_cm_rho_flags_t &= (~BT_CM_RHO_PREPARE_WAIT_FLAG_SCAN_MODE);
        }
            break;
        default:
            return;
    }    
    bt_cmgr_report_id("[BT_CM][RHO][I] end cm rho gap event handle msg:0x%02x, status:0x%02x, g_bt_cm_rho_flags_t is %d", 3, msg, status, g_bt_cm_rho_flags_t);
    if (0 == g_bt_cm_rho_flags_t && BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
        bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_SINK_CM);
    }
}

static void         bt_cm_rho_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    bt_bd_addr_t                    aws_addr;
    bt_cmgr_report_id("[BT_CM][RHO][I] Status callback role:0x%02x, event:0x%02x, status:0x%x", 3, role, event, status);
    if (!bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &aws_addr, 1)) {
        return;
    }
    bt_gap_link_policy_setting_t    setting;
    bt_cm_remote_device_t *aws_device = bt_cm_find_device(BT_CM_FIND_BY_ADDR, &aws_addr);
    extern bt_cm_cnt_t *g_bt_cm_cnt;
    if (BT_ROLE_HANDOVER_PREPARE_REQ_IND == event && NULL != aws_device) {
        bt_status_t ret_status;
        if (BT_GAP_LINK_SNIFF_TYPE_ACTIVE != aws_device->link_info.sniff_state) {
            ret_status = bt_gap_exit_sniff_mode(aws_device->link_info.handle);
            if (BT_STATUS_PENDING == ret_status || BT_STATUS_SUCCESS == ret_status) {
                g_bt_cm_rho_flags_t |= BT_CM_RHO_PREPARE_WAIT_FLAG_EXIT_SNIFF;
            }
            bt_cmgr_report_id("[BT_CM][RHO][I] Exit sniff mode status 0x%x", 1, ret_status);
        }
        if (NULL != g_bt_cm_cnt && BT_GAP_SCAN_MODE_NOT_ACCESSIBLE != g_bt_cm_cnt->scan_mode) {
            if (BT_STATUS_SUCCESS == (ret_status = bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE))) {
                g_bt_cm_rho_flags_t |= BT_CM_RHO_PREPARE_WAIT_FLAG_SCAN_MODE;
            }
            bt_cmgr_report_id("[BT_CM][RHO][E] Write scan mode status 0x%x", 1, ret_status);
        }
        setting.sniff_mode = BT_GAP_LINK_POLICY_DISABLE;
        if (BT_STATUS_SUCCESS == (ret_status = bt_gap_write_link_policy(aws_device->link_info.handle, &setting))) {
            g_bt_cm_rho_flags_t |= BT_CM_RHO_PREPARE_WAIT_FLAG_LINK_POLICY;
        }
        bt_cmgr_report_id("[BT_CM][RHO][E] Write link policy fail status 0x%x", 1, ret_status);
#ifdef AIR_MULTI_POINT_ENABLE
        bt_cm_remote_device_t *cancel_device = bt_cm_list_get_last(BT_CM_LIST_CONNECTING);
        if (NULL != cancel_device) {
            bt_cm_memcpy(&(bt_cm_rho_context.connecting_device.address), &(cancel_device->link_info.addr), sizeof(bt_bd_addr_t));
            bt_cm_rho_context.connecting_device.profile = (cancel_device->request_connect_mask | cancel_device->link_info.connected_mask | cancel_device->link_info.connecting_mask);
            g_bt_cm_rho_flags_t |= BT_CM_RHO_PREPARE_WAIT_FLAG_CANCEL_CONNECTION;            
            bt_cmgr_report_id("[BT_CM][RHO][I] set cancel connectiong flag and g_bt_cm_rho_flags_t is %d", 1,  g_bt_cm_rho_flags_t);
            bt_cm_cancel_connect(&(bt_cm_rho_context.connecting_device.address));            
        } else
#endif
        if (0 == g_bt_cm_rho_flags_t && BT_ROLE_HANDOVER_STATE_ONGOING == bt_role_handover_get_state()) {
            bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_SINK_CM);
        }
    } else if (BT_ROLE_HANDOVER_COMPLETE_IND == event && NULL != aws_device) {
        if (BT_STATUS_SUCCESS == status) {
            /* Flush nvdm may take a lot of time make RHO latency, so move flush point from RHO update to RHO complete. */
            bt_cmgr_report_id("[BT_CM][RHO][I] Success current role 0x%x", 1, role);
            bt_device_manager_aws_local_info_update();
        }else {
            bt_status_t bt_aws_mce_retry_set_state();
            bt_aws_mce_retry_set_state();
        }
        if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
            setting.sniff_mode = BT_GAP_LINK_POLICY_ENABLE;
            bt_gap_write_link_policy(aws_device->link_info.handle, &setting);
#ifdef AIR_MULTI_POINT_ENABLE
            bt_status_t write_policy_sub_status;
            write_policy_sub_status = bt_gap_write_link_policy(aws_device->link_info.handle, &setting);
            bt_cmgr_report_id("[BT_CM][RHO][I] CM RHO COMPLETE write link policy sub status is 0x%x", 1, write_policy_sub_status);
#endif
            bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_UNKNOW);
        }
        g_bt_cm_cnt->connected_dev_num = 0;
        bt_cm_remote_device_t *device_p = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED];
        while (NULL != device_p) {
            g_bt_cm_cnt->connected_dev_num++;
            device_p = device_p->next[BT_CM_LIST_CONNECTED];
        }
    }
    bt_cm_rho_state_update_ind_t rho_state;
    rho_state.event = event;
    rho_state.status = status;
    bt_cm_register_callback_notify(BT_CM_EVENT_RHO_STATE_UPDATE, &rho_state, sizeof(rho_state));
}

void                bt_cm_rho_init()
{
    bt_role_handover_callbacks_t bt_cm_rho_callback_sets = {
        .allowed_cb =   &bt_cm_rho_is_allowed,
        .get_len_cb =   &bt_cm_rho_get_length,
        .get_data_cb =  &bt_cm_rho_get_data,
        .update_cb =    &bt_cm_rho_update,
        .status_cb =    &bt_cm_rho_status_callback
    };
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_SINK_CM, &bt_cm_rho_callback_sets);
    bt_cm_memset(&bt_cm_rho_context, 0, sizeof(bt_cm_rho_context_t));
    g_bt_cm_rho_flags_t = 0;
}

void                bt_cm_rho_deinit()
{
    bt_role_handover_deregister_callbacks(BT_ROLE_HANDOVER_MODULE_SINK_CM);
}

#endif /* #ifdef SUPPORT_ROLE_HANDOVER_SERVICE */




