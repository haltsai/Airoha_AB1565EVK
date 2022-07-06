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
#include "bt_device_manager_internal.h"


#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"

#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
#include "bt_connection_manager_state_machine_ext.h"
#endif

extern bt_sink_srv_cm_context_t g_sink_srv_cm_context;
extern bt_bd_addr_t g_sink_srv_cm_last_connected;

static bt_status_t bt_connection_manager_aws_rho_is_allowed(const bt_bd_addr_t *addr)
{
    bt_sink_srv_cm_remote_device_t *aws_device = bt_sink_srv_cm_get_normal_aws_device();
    bt_sink_srv_cm_remote_device_t *saws_device = bt_sink_srv_cm_get_special_aws_device();
    bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
    if (BT_AWS_MCE_ROLE_AGENT != aws_role) {
        bt_cmgr_report_id("[BT_CM][RHO][I] rho not allowed due to role is :0x%02x", 1, aws_role);
        return BT_STATUS_FAIL;
    }
    if (NULL == aws_device || NULL == saws_device) {
        bt_cmgr_report_id("[BT_CM][RHO][I] rho not allowed due to aws is %p, special aws is %p", 2, aws_device, saws_device);
        return BT_STATUS_FAIL;
    }
    if (BT_AWS_MCE_AGENT_STATE_ATTACHED != aws_device->aws_state) {
        bt_cmgr_report_id("[BT_CM][RHO][I] rho not allowed due to partner not be attched", 0);
        return BT_STATUS_FAIL;
    }
    if (NULL != g_sink_srv_cm_context.request_pending) {
        bt_cmgr_report_id("[BT_CM][RHO][I] rho not allowed due to request_pending not done", 0);
        //bt_sink_srv_cm_pending_request_print(g_sink_srv_cm_context.request_pending);
        return BT_STATUS_FAIL;
    }
    if (BT_GAP_LINK_SNIFF_TYPE_ACTIVE != aws_device->sniff_state) {
        bt_status_t status  = bt_gap_exit_sniff_mode(aws_device->handle);
        if (BT_STATUS_SUCCESS != status && BT_STATUS_PENDING != status) {
            bt_cmgr_report_id("[BT_CM][RHO][I] rho not allowed due to exit sniff fail", 0);    
            return BT_STATUS_FAIL;
        }
        return BT_STATUS_PENDING;
    }
    return BT_STATUS_SUCCESS;
}

static uint8_t bt_connection_manager_aws_rho_get_length(const bt_bd_addr_t *addr)
{
    return sizeof(bt_sink_srv_cm_rho_context_t);
}

static bt_status_t bt_connection_manager_aws_rho_get_data(const bt_bd_addr_t *addr, void * data)
{
    bt_sink_srv_cm_rho_context_t *rho_context = (bt_sink_srv_cm_rho_context_t *)data;
    bt_sink_srv_cm_remote_device_t *aws_device = bt_sink_srv_cm_get_normal_aws_device();
    bt_connection_manager_assert(NULL != aws_device && "RHO get data but no aws device exist !!!");
    bt_connection_manager_memcpy((void*)&rho_context->sp_addr, (void*)&aws_device->addr, sizeof(bt_bd_addr_t));
    rho_context->profile_connection_mask = aws_device->profile_connection_mask;

    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_connection_manager_aws_rho_update(bt_role_handover_update_info_t *info)
{
    bt_sink_srv_cm_remote_device_t *aws_device = bt_sink_srv_cm_get_normal_aws_device();
    bt_sink_srv_cm_remote_device_t *saws_device = bt_sink_srv_cm_get_special_aws_device();
    bt_bd_addr_t *peer_addr = bt_connection_manager_device_local_info_get_peer_aws_address();
    bt_bd_addr_t local_addr = {0};
    bt_aws_mce_role_t cur_aws_role = bt_connection_manager_device_local_info_get_aws_role();

    bt_connection_manager_assert(NULL != aws_device && "RHO update context no aws device exist");
    bt_connection_manager_assert(NULL != saws_device && "RHO update context no special aws device exist");
    bt_cmgr_report_id("[BT_CM][I] rho update context current role : %02X", 1, cur_aws_role);
    bt_connection_manager_assert(BT_AWS_MCE_ROLE_AGENT == cur_aws_role || BT_AWS_MCE_ROLE_PARTNER == cur_aws_role);

    bt_connection_manager_memcpy((void*)&local_addr, (void*)bt_connection_manager_device_local_info_get_local_address(), sizeof(bt_bd_addr_t));
    saws_device->profile_connection_mask = BT_SINK_SRV_PROFILE_AWS;
    saws_device->local_role = BT_SINK_SRV_ROLE_SLAVE;
    saws_device->aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
    bt_cmgr_report_id("[BT_CM][RHO][I] bt_sink_srv_cm_role_handover_update role: 0x%02x, data length %d", 2, 
        info->role, info->length);
    if(BT_AWS_MCE_ROLE_AGENT == cur_aws_role) {
        bt_cmgr_report_id("[BT_CM][RHO][I] Agent switch context to partner role", 0);
        bt_connection_manager_memcpy((void*)&aws_device->addr, &local_addr, sizeof(bt_bd_addr_t));
        aws_device->profile_connection_mask = BT_SINK_SRV_CM_PROFILE_AWS_CLIENT;
        aws_device->local_role = BT_SINK_SRV_ROLE_MASTER;
        aws_device->aws_state = BT_AWS_MCE_AGENT_STATE_NONE;
        bt_connection_manager_memcpy(&g_sink_srv_cm_last_connected, &local_addr, sizeof(bt_bd_addr_t));
    } else {
        bt_cmgr_report_id("[BT_CM][RHO][I] Partner switch context to agent role data is : %p", 1, info->data);

        bt_connection_manager_assert(NULL != info->data && "RHO update to agent data is null");
        bt_sink_srv_cm_rho_context_t *rho_context = (bt_sink_srv_cm_rho_context_t *)info->data;
        bt_connection_manager_memcpy((void*)&aws_device->addr, (void*)&rho_context->sp_addr, sizeof(bt_bd_addr_t));
        aws_device->profile_connection_mask = rho_context->profile_connection_mask;
        aws_device->local_role = BT_SINK_SRV_ROLE_SLAVE;
        aws_device->aws_state = BT_AWS_MCE_AGENT_STATE_ATTACHED;
        aws_device->link_state = BT_SINK_SRV_CM_STATE_ENCRYPTED;
        bt_connection_manager_memcpy(&g_sink_srv_cm_last_connected, (void*)&rho_context->sp_addr, sizeof(bt_bd_addr_t));
    }
    bt_connection_manager_memcpy((void*)&saws_device->addr, peer_addr, sizeof(bt_bd_addr_t));
    bt_connection_manager_device_local_info_store_local_address(peer_addr);
    bt_connection_manager_device_local_info_store_peer_aws_address(&local_addr);
    bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_AGENT == cur_aws_role ? 
        BT_AWS_MCE_ROLE_PARTNER : BT_AWS_MCE_ROLE_AGENT);
    return BT_STATUS_SUCCESS;
}

static void bt_connection_manager_aws_rho_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, 
    bt_role_handover_event_t event, bt_status_t status)
{
    bt_gap_link_policy_setting_t setting;
    bt_sink_srv_cm_remote_device_t *aws_device = bt_sink_srv_cm_get_normal_aws_device();
    bt_cmgr_report_id("[BT_CM][RHO][I] bt_sink_srv_cm_role_handover_status_callback role:0x%02x, event:0x%02x, status:0x%x", 3,
        role, event, status);
    if (BT_ROLE_HANDOVER_PREPARE_REQ_IND == event && NULL != aws_device) {
        setting.sniff_mode = BT_GAP_LINK_POLICY_DISABLE;
        bt_status_t ret_status = bt_gap_write_link_policy(aws_device->handle, &setting);
        if (BT_STATUS_SUCCESS != ret_status) {
            bt_cmgr_report_id("[BT_CM][RHO][E] RHO write link policy fail status %x", 1, ret_status);
            return;
        }
        aws_device->flag |= BT_SINK_SRV_CM_REMOTE_FLAG_DISABLE_SNIFF_PENDING;
    } else if (BT_ROLE_HANDOVER_COMPLETE_IND == event && NULL != aws_device) {
        if (BT_STATUS_SUCCESS == status) {
            // Flush nvdm may take a lot of time make RHO latency, so move flush point from RHO update to RHO complete.
            bt_cmgr_report_id("[BT_CM][RHO][I] RHO success current role 0x%x", 1, role);
            bt_device_manager_aws_local_info_update();
        }
        setting.sniff_mode = BT_GAP_LINK_POLICY_ENABLE;
        bt_gap_write_link_policy(aws_device->handle, &setting);
    }

#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
    bt_connection_manager_state_machine_ext_rho_event_notify(event, status);
#endif
}

void bt_connection_manager_aws_rho_init()
{
    bt_role_handover_callbacks_t bt_cm_rho_callback_sets = {
        .allowed_cb = &bt_connection_manager_aws_rho_is_allowed,
        .get_len_cb = &bt_connection_manager_aws_rho_get_length,
        .get_data_cb = &bt_connection_manager_aws_rho_get_data,
        .update_cb = &bt_connection_manager_aws_rho_update,
        .status_cb = &bt_connection_manager_aws_rho_status_callback
    };
    // register RHO callback
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_SINK_CM, &bt_cm_rho_callback_sets);
}

void bt_connection_manager_aws_rho_deinit()
{
    bt_role_handover_deregister_callbacks(BT_ROLE_HANDOVER_MODULE_SINK_CM);
}

#endif /* #ifdef SUPPORT_ROLE_HANDOVER_SERVICE */

