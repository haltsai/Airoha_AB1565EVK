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
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce.h"
#include "bt_aws_mce_srv.h"
#endif
#include "bt_device_manager.h"
#include "bt_device_manager_internal.h"
#include "bt_connection_manager_utils.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"

bt_bd_addr_t*   bt_sink_srv_cm_last_connected_device()
{
    return bt_cm_get_last_connected_device();
}


static bt_cm_profile_service_mask_t
                bt_cm_porting_convert_profile_mask(bt_sink_srv_profile_type_t profile)
{
    bt_cm_profile_service_mask_t profile_mask = BT_CM_PROFILE_SERVICE_MASK_NONE;
    if (BT_SINK_SRV_PROFILE_HFP & profile) {
        profile_mask |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP);
    }
    if (BT_SINK_SRV_PROFILE_A2DP_SINK & profile) {
        profile_mask |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK);
    }
    if (BT_SINK_SRV_PROFILE_AVRCP & profile) {
        profile_mask |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP);
    }
    if (BT_SINK_SRV_PROFILE_PBAPC & profile) {
        profile_mask |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_PBAPC);
    }
    if (BT_SINK_SRV_PROFILE_HSP & profile) {
        profile_mask |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HSP);
    }
    if (BT_SINK_SRV_PROFILE_AWS & profile) {
        profile_mask |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
    }
    return profile_mask;
}

static bt_cm_profile_service_t
                bt_cm_porting_convert_profile_type(bt_sink_srv_profile_type_t profile)
{
    bt_cm_profile_service_t profile_type = BT_CM_PROFILE_SERVICE_NONE;
    if (BT_SINK_SRV_PROFILE_HFP & profile) {
        profile_type = BT_CM_PROFILE_SERVICE_HFP;
    } else if (BT_SINK_SRV_PROFILE_A2DP_SINK & profile) {
        profile_type = BT_CM_PROFILE_SERVICE_A2DP_SINK;
    } else if (BT_SINK_SRV_PROFILE_AVRCP & profile) {
        profile_type = BT_CM_PROFILE_SERVICE_AVRCP;
    } else if (BT_SINK_SRV_PROFILE_PBAPC & profile) {
        profile_type = BT_CM_PROFILE_SERVICE_PBAPC;
    } else if (BT_SINK_SRV_PROFILE_HSP & profile) {
        profile_type = BT_CM_PROFILE_SERVICE_HSP;
    } else if (BT_SINK_SRV_PROFILE_AWS & profile) {
        profile_type = BT_CM_PROFILE_SERVICE_AWS;
    }
    return profile_type;
}

uint32_t        bt_sink_srv_cm_get_connected_device(bt_sink_srv_profile_type_t profile, bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER])
{
    bt_cm_profile_service_mask_t profile_mask = bt_cm_porting_convert_profile_mask(profile);
    return bt_cm_get_connected_devices(profile_mask, addr_list, 0xFF);
}

uint32_t        bt_sink_srv_cm_get_connected_device_list(bt_sink_srv_profile_type_t profile, bt_bd_addr_t *addr_list, uint32_t list_num)
{
    bt_cm_profile_service_mask_t profile_mask = bt_cm_porting_convert_profile_mask(profile);
    return bt_cm_get_connected_devices(profile_mask, addr_list, list_num);
}

bt_sink_srv_profile_type_t
                bt_sink_srv_cm_get_connected_profiles(bt_bd_addr_t *address)
{
    bt_sink_srv_profile_type_t profile_type = BT_SINK_SRV_PROFILE_NONE;
    bt_cm_profile_service_mask_t profile_mask = bt_cm_get_connected_profile_services(*address);
    if (profile_mask & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)) {
        profile_type |= BT_SINK_SRV_PROFILE_HFP;
    }
    if (profile_mask & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)) {
        profile_type |= BT_SINK_SRV_PROFILE_A2DP_SINK;
    }
    if (profile_mask & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP)) {
        profile_type |= BT_SINK_SRV_PROFILE_AVRCP;
    }
    if (profile_mask & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_PBAPC)) {
        profile_type |= BT_SINK_SRV_PROFILE_PBAPC;
    }        
    if (profile_mask & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HSP)) {
        profile_type |= BT_SINK_SRV_PROFILE_HSP;
    }
    if (profile_mask & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)) {
        profile_type |= BT_SINK_SRV_PROFILE_AWS;
    }
    return profile_type;    
}

bt_gap_connection_handle_t
                bt_sink_srv_cm_get_gap_handle(bt_bd_addr_t *address_p)
{
    return bt_cm_get_gap_handle(*address_p);
}

void            bt_sink_srv_cm_profile_status_notify(bt_bd_addr_t *addr, bt_sink_srv_profile_type_t profile, bt_sink_srv_profile_connection_state_t state, bt_status_t reason)
{
    bt_cm_profile_service_t profile_type = bt_cm_porting_convert_profile_type(profile);
    bt_cm_profile_service_state_t new_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
    if (BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED == state) {
        new_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
    } else if (BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING == state) {
        new_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTING;
    } else if (BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED == state) {
        new_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
    } else if (BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTING == state) {
        new_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTING;
    } 
    bt_cm_profile_service_status_notify(profile_type, *addr, new_state, reason);
}

void            bt_connection_manager_device_local_info_store_local_address(bt_bd_addr_t *addr)
{
    bt_device_manager_store_local_address_internal(addr);
}

bt_bd_addr_t*   bt_connection_manager_device_local_info_get_local_address(void)
{
    return bt_device_manager_get_local_address();
}

bt_bd_addr_t*   bt_connection_manager_device_local_info_get_peer_aws_address(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    return bt_device_manager_aws_local_info_get_peer_address();
#else
    return NULL;
#endif
}

void            bt_connection_manager_device_local_info_store_peer_aws_address(bt_bd_addr_t *addr)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_device_manager_aws_local_info_store_peer_address(addr);
#endif
}

bt_aws_mce_role_t 
                bt_connection_manager_device_local_info_get_aws_role(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    return bt_device_manager_aws_local_info_get_role();
#else
    return BT_AWS_MCE_ROLE_NONE;
#endif
}

void            bt_connection_manager_device_local_info_store_aws_role(bt_aws_mce_role_t aws_role)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_device_manager_aws_local_info_store_role(aws_role);
#endif
}

void            bt_connection_manager_write_scan_enable_mode(bt_gap_scan_mode_t mode)
{
    if (BT_GAP_SCAN_MODE_DISCOVERABLE_ONLY == mode) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_ENABLE, BT_CM_COMMON_TYPE_DISABLE);
    } else if (BT_GAP_SCAN_MODE_NOT_ACCESSIBLE == mode) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
    } else if (BT_GAP_SCAN_MODE_GENERAL_ACCESSIBLE == mode) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_ENABLE, BT_CM_COMMON_TYPE_ENABLE);
    } else if (BT_GAP_SCAN_MODE_CONNECTABLE_ONLY) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_ENABLE);
    }
}

bt_cm_power_state_t
                bt_connection_manager_power_get_state()
{
    return bt_cm_power_get_state();    
}

#ifdef MTK_AWS_MCE_ENABLE
void            bt_connection_manager_device_local_info_store_aws_ls_enable(bt_connection_manager_aws_ls_enable_t ls_enable)
{

    bt_device_manager_aws_local_info_store_ls_enable(ls_enable);
}

bt_connection_manager_aws_ls_enable_t
                bt_connection_manager_device_local_info_get_aws_ls_enable()
{
    return bt_device_manager_aws_local_info_get_ls_enable();
}

void            bt_sink_srv_cm_ls_enable(bool enable)
{
    bt_aws_mce_srv_set_aws_disable(!enable);
}

void*           bt_sink_srv_cm_get_special_aws_device()
{
    return (void *)(0x01U);
}

#if 0
bt_aws_mce_agent_state_type_t
                bt_sink_srv_aws_mce_get_aws_state_by_handle(uint32_t aws_handle)
{
    bt_cm_remote_device_t *rem_dev = bt_cm_find_device(BT_CM_FIND_BY_AWS_HANDLE, &aws_handle);
    if (NULL != rem_dev) {
        bt_cm_profile_service_state_t profile_state = rem_dev->aws_dev.aws_state;
        if (BT_CM_PROFILE_SERVICE_STATE_CONNECTED == profile_state) {
            return BT_AWS_MCE_AGENT_STATE_ATTACHED;
        }
    }
    return BT_AWS_MCE_AGENT_STATE_NONE;
}
#endif

bt_bd_addr_t*   bt_sink_srv_cm_get_aws_connected_device()
{
    extern bt_cm_cnt_t *g_bt_cm_cnt;
    if (NULL == g_bt_cm_cnt) {
        return NULL;
    }
    bt_cm_remote_device_t *device_p = g_bt_cm_cnt->handle_list[BT_CM_LIST_CONNECTED];
    while (NULL != device_p) {
        if (0 != (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & device_p->link_info.connected_mask)) {
            bt_cmgr_report_id("[BT_CM][ADAPT] Get aws connected device 0x%x", 1, *(uint32_t *)(device_p->link_info.addr));
            return &(device_p->link_info.addr);
        }
        device_p = device_p->next[BT_CM_LIST_CONNECTED];
    }
    bt_cmgr_report_id("[BT_CM][ADAPT][W] Can't get aws connected device", 0);
    return NULL;
}

#if 0
uint32_t        bt_sink_srv_aws_mce_get_handle(bt_bd_addr_t *bt_addr)
{
    return bt_aws_mce_srv_get_aws_handle(bt_addr);
}
#endif

bt_connection_manager_aws_link_type_t
                bt_connection_manager_get_aws_link_type(void)
{
    bt_aws_mce_srv_link_type_t aws_type = bt_aws_mce_srv_get_link_type();
    if (BT_AWS_MCE_SRV_LINK_NONE == aws_type) {
        return BT_CONNECTION_MANAGER_AWS_LINK_NONE;
    } else if (BT_AWS_MCE_SRV_LINK_SPECIAL== aws_type) {
        return BT_CONNECTION_MANAGER_AWS_LINK_SPECIAL;
    } else if (BT_AWS_MCE_SRV_LINK_NORMAL == aws_type) {
        return BT_CONNECTION_MANAGER_AWS_LINK_NORMAL;
    }
    return BT_CONNECTION_MANAGER_AWS_LINK_NONE;
}

bt_aws_mce_agent_state_type_t
                bt_sink_srv_cm_get_aws_link_state(void)
{
    bt_aws_mce_srv_link_type_t aws_type = bt_aws_mce_srv_get_link_type();
    if (BT_AWS_MCE_SRV_LINK_NONE == aws_type) {
        return BT_AWS_MCE_AGENT_STATE_INACTIVE;
    } else {
        return BT_AWS_MCE_AGENT_STATE_ATTACHED;
    }
    return BT_AWS_MCE_AGENT_STATE_INACTIVE;
}
#endif

