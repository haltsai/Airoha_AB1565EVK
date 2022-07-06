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

#include "hal.h"
#include "hal_wdt.h"
#include "bt_gap_le.h"
#include "bt_callback_manager.h"
#include "bt_device_manager.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_db.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_utils.h"
//#include "bt_connection_manager_internal.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_srv.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif
#endif
#include "bt_device_manager_power.h"

#define BT_AWS_MCE_SRV_SYNC_PARTNER_ADDR_TO_AGENT   (0x00)
#define BT_AWS_MCE_SRV_SYNC_CONNECTION_CONFIRM      (0x01)
typedef uint8_t bt_aws_mce_srv_sync_t;

#define BT_AWS_MCE_SRV_FLAGS_INITINIG               (0x01)
#define BT_AWS_MCE_SRV_FLAGS_SET_ROLE_PENDING       (0x02)
#define BT_AWS_MCE_SRV_FLAGS_CONNECT_DISABLED       (0x04)
#define BT_AWS_MCE_SRV_FLAGS_SET_CONNECTABLE_FAIL   (0x08)
#define BT_AWS_MCE_SRV_FLAGS_SWITCH_MODE_RESET      (0x10)
#define BT_AWS_MCE_SRV_FLAGS_CASE_PAIRING           (0x20)
typedef uint8_t bt_aws_mce_srv_flags_t;

#define BT_AWS_MCE_SRV_SUPPORT_NUM                  (0x04)

#define BT_AWS_MCE_SRV_FIND_BY_ADDR                 (0x00)
#define BT_AWS_MCE_SRV_FIND_BY_AWS_HANDLE           (0x01)
#define BT_AWS_MCE_SRV_FIND_BY_AWS_STATE            (0x02)
#define BT_AWS_MCE_SRV_FIND_BY_REQ_AWS_STATE        (0x03)
typedef uint8_t bt_aws_mce_srv_find_t;

typedef struct {
    bt_bd_addr_t                    address;
    uint32_t                        aws_handle;
    bt_cm_profile_service_state_t   aws_state;
    bt_cm_profile_service_state_t   req_aws_state;
    uint8_t                         aws_ready;
#ifdef AIR_MULTI_POINT_ENABLE
    bt_cm_aws_link_disconnect_reason_t aws_link_disconnect_reason;       /* notify partner aws link disconnect reason */
#endif
} bt_aws_mce_srv_dev_t;

typedef struct {
    bt_aws_mce_srv_flags_t  flags;
    void* data;
    bt_aws_mce_srv_dev_t    aws_dev[BT_AWS_MCE_SRV_SUPPORT_NUM];
} bt_aws_mce_srv_cnt_t;

static bt_aws_mce_srv_cnt_t g_bt_aws_mce_srv_cnt_t;

static void         bt_aws_mce_srv_state_update(bt_aws_mce_srv_dev_t *rem_dev);

static bt_aws_mce_srv_dev_t *
                    bt_aws_mce_srv_find(bt_aws_mce_srv_find_t find_type, void * param)
{
    bt_aws_mce_srv_dev_t *device_p = NULL;
    for (uint32_t i = 0; i < BT_AWS_MCE_SRV_SUPPORT_NUM; ++i) {
        if ((BT_AWS_MCE_SRV_FIND_BY_ADDR == find_type &&
            bt_cm_memcmp(param, &g_bt_aws_mce_srv_cnt_t.aws_dev[i].address, sizeof(bt_bd_addr_t)) == 0) ||
            (BT_AWS_MCE_SRV_FIND_BY_AWS_HANDLE == find_type &&
            g_bt_aws_mce_srv_cnt_t.aws_dev[i].aws_handle == (uint32_t)param) ||
            (BT_AWS_MCE_SRV_FIND_BY_AWS_STATE == find_type &&
            g_bt_aws_mce_srv_cnt_t.aws_dev[i].aws_state == (bt_cm_profile_service_state_t)(uint32_t)param) ||
            (BT_AWS_MCE_SRV_FIND_BY_REQ_AWS_STATE == find_type &&
            g_bt_aws_mce_srv_cnt_t.aws_dev[i].req_aws_state == (bt_cm_profile_service_state_t)(uint32_t)param)) {
            device_p = &g_bt_aws_mce_srv_cnt_t.aws_dev[i];
            break;
        }
    }
    if (NULL != device_p) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Find success with address:0x%x", 1, *(uint32_t *)&(device_p->address));
    } else {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Can't find device by type %d and param 0x%x", 2, find_type, *(uint32_t *)param);
    }
    return device_p;
}

static void         bt_aws_mce_srv_send_packet(bt_aws_mce_srv_sync_t type, uint32_t data_length, void *data)
{
    bt_status_t status;
    uint32_t report_length = sizeof(bt_aws_mce_report_info_t) + data_length + 1;
    bt_aws_mce_report_info_t *cm_report = bt_cm_memory_alloc(report_length);
    if (NULL == cm_report) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Send AWS packet failed can't allocat buffer.", 0);
        return;
    }
    bt_cm_memset(cm_report, 0 , sizeof(report_length));
    uint8_t *data_payload = ((uint8_t *)cm_report) + sizeof(bt_aws_mce_report_info_t);
    cm_report->module_id = BT_AWS_MCE_REPORT_MODULE_CM;
    cm_report->param_len = report_length - sizeof(bt_aws_mce_report_info_t);
    cm_report->param = data_payload;
    data_payload[0] = type;
    if (0 != data_length) {
        bt_cm_memcpy(data_payload + 1, (void*)data, data_length);
    }
    if (BT_STATUS_SUCCESS != (status = bt_aws_mce_report_send_event(cm_report))) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Send AWS packet failed status 0x%x.", 1, status);
    }
    bt_cm_memory_free(cm_report);
}

static void         bt_aws_mce_srv_packet_callback(bt_aws_mce_report_info_t *para)
{
    if (NULL == para) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] AWS packet para is null.", 0);
        return;
    }
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS packet module_id:0x%x, is_sync:%d, sync_time:%d, param_len:%d.", 4,
        para->module_id, para->is_sync, para->sync_time, para->param_len);
    if (BT_AWS_MCE_REPORT_MODULE_CM != para->module_id) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS packet module is not CM.", 0);
        return;
    }
    bt_aws_mce_srv_sync_t event = ((uint8_t *)para->param)[0];
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS packet event: %d.", 1, event);
    switch(event) {
    case BT_AWS_MCE_SRV_SYNC_PARTNER_ADDR_TO_AGENT: {
        bt_bd_addr_t *partner_addr = (bt_bd_addr_t *)(((uint8_t *)para->param) + 1);
        bt_device_manager_aws_local_info_store_peer_address(partner_addr);
        bt_device_manager_aws_local_info_update();
    }
        break;
    case BT_AWS_MCE_SRV_SYNC_CONNECTION_CONFIRM: {

    }
        break;
    default:
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] AWS packet event error.", 0);
    break;
    }
}

static bt_aws_mce_srv_dev_t *
                    bt_aws_mce_srv_find_req_connected_dev_except(bt_aws_mce_srv_dev_t *rem_dev)
{
    for (uint32_t i = 0; i < BT_AWS_MCE_SRV_SUPPORT_NUM; ++i) {
        if (g_bt_aws_mce_srv_cnt_t.aws_dev[i].req_aws_state == BT_CM_PROFILE_SERVICE_STATE_CONNECTED &&
            (NULL == rem_dev || rem_dev != &g_bt_aws_mce_srv_cnt_t.aws_dev[i])) {
            return &g_bt_aws_mce_srv_cnt_t.aws_dev[i];
        }
    }
    return NULL;
}

static bt_status_t  bt_aws_mce_srv_disconnect_req_connect_dev_except(bt_aws_mce_srv_dev_t *rem_dev)
{
    bt_aws_mce_srv_dev_t *device_p = NULL;
    for (uint32_t i = 0; i < BT_AWS_MCE_SRV_SUPPORT_NUM; ++i) {
        device_p = &g_bt_aws_mce_srv_cnt_t.aws_dev[i];
        if (device_p == rem_dev) {
            continue;
        }
        if (BT_CM_PROFILE_SERVICE_STATE_CONNECTED == device_p->req_aws_state || 
            (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == device_p->req_aws_state && BT_CM_PROFILE_SERVICE_STATE_CONNECTING <= device_p->aws_state)) {
            device_p->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
            if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == device_p->aws_state) {
        #ifdef AIR_MULTI_POINT_ENABLE
                device_p->aws_link_disconnect_reason = BT_CM_AWS_LINK_DISCONNECT_NORMAL;
        #endif
                bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, device_p->address,
                    BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED, BT_STATUS_SUCCESS);
                continue;
            }
        #ifdef AIR_MULTI_POINT_ENABLE
            /* set aws link disconnect reason: SWITCH_LINK */
            device_p->aws_link_disconnect_reason = BT_CM_AWS_LINK_DISCONNECT_BY_SWITCH_LINK;
        #endif
            bt_aws_mce_srv_state_update(device_p);
            return BT_STATUS_SUCCESS;
        }
        if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED != device_p->aws_state) {
             return BT_STATUS_SUCCESS;
        }
    }
    return BT_STATUS_FAIL;
}

static bt_status_t  bt_aws_mce_srv_agent_set_state(bt_aws_mce_srv_dev_t *rem_dev, bool enable)
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_cm_profile_service_state_t aws_state = rem_dev->aws_state;
    bt_cm_link_info_t *link_info = bt_cm_get_link_information(rem_dev->address);
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Agent set AWS state:%d, cur_state:0x%x, aws_handle:0x%x, device:0x%x.",
        4, enable, aws_state, rem_dev->aws_handle, *(uint32_t *)&(rem_dev->address));
    if (NULL != link_info && true == enable && BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == aws_state) {
        if (BT_GAP_LINK_SNIFF_TYPE_ACTIVE != link_info->sniff_state) {
            /* The sp connection link sniff state should be active before enable LS on special link or sp link. */
            bt_status_t status = bt_gap_exit_sniff_mode(link_info->handle);
            bt_cmgr_report_id("[BT_CM][AWS_MCE][W] Exit sniff mode fail status 0x%x.", 1, status);
        }
        if (BT_CM_ACL_LINK_ENCRYPTED > link_info->link_state) {
            /* Wait the encryption complete.  */
            return ret;
        } else if (BT_CM_ROLE_SLAVE != link_info->local_role) {
            /* Wait to set role to slave. */
            bt_cm_switch_role(link_info->addr, BT_ROLE_SLAVE);
            return ret;
        } else if (false == rem_dev->aws_ready) {
            /* Wait aws ready. */
            return ret;
        } else if (BT_STATUS_SUCCESS == bt_aws_mce_srv_disconnect_req_connect_dev_except(rem_dev)) {
            /* Wait other aws profile disconnected. */
            return ret;
        } else if (g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_CONNECT_DISABLED) {
            bt_cmgr_report_id("[BT_CM][AWS_MCE][I] User prohibit aws.", 0);
            return ret;
        }
        bt_cm_power_state_t power_state = BT_CM_POWER_STATE_OFF;
        if(BT_CM_POWER_STATE_ON != (power_state = bt_cm_power_get_state())){
            bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Current Power State: %x ", 1,power_state);
            return ret;
        }
        ret = bt_aws_mce_set_state(rem_dev->aws_handle, BT_AWS_MCE_AGENT_STATE_CONNECTABLE);
        if(BT_STATUS_SUCCESS != ret){
            bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Enable LS failed, make a flag", 0);
            g_bt_aws_mce_srv_cnt_t.flags |= BT_AWS_MCE_SRV_FLAGS_SET_CONNECTABLE_FAIL;
            g_bt_aws_mce_srv_cnt_t.data =  rem_dev;
        }else if(g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_SET_CONNECTABLE_FAIL){
            bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Enable LS success, clean  flag", 0);
            g_bt_aws_mce_srv_cnt_t.flags &= ~BT_AWS_MCE_SRV_FLAGS_SET_CONNECTABLE_FAIL;
            g_bt_aws_mce_srv_cnt_t.data =  NULL;
        }
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] bt_aws_mce_set_state ret: %x", 1, ret);
    } else if (false == enable &&
        (BT_CM_PROFILE_SERVICE_STATE_CONNECTING == aws_state || BT_CM_PROFILE_SERVICE_STATE_CONNECTED == aws_state)) {
        bt_aws_mce_agent_state_type_t bt_aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
#ifdef AIR_MULTI_POINT_ENABLE
        bt_cm_aws_link_disconnect_reason_t dis_reason = rem_dev->aws_link_disconnect_reason;
        dis_reason &= 0x07; /* disconnect reason occupy bit1,bit2,bit3; range : 0 ~ 7 */
        bt_aws_state |= (dis_reason << 1);
#endif
        ret = bt_aws_mce_set_state(rem_dev->aws_handle, bt_aws_state);
    } else {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][W] AWS not in correct state.", 0);
        return ret;
    }
    if (BT_STATUS_SUCCESS != ret) {
        g_bt_aws_mce_srv_cnt_t.flags |= BT_AWS_MCE_SRV_FLAGS_SET_ROLE_PENDING;
        bt_cmgr_report_id("[BT_CM][AWS_MCE][W] AWS Set state fail status:0x%x.", 1, ret);
    }
    return ret;
}

static void         bt_aws_mce_srv_agent_state_machine(bt_aws_mce_srv_dev_t *rem_dev)
{
    bt_status_t status = BT_STATUS_FAIL;
    bt_cm_link_info_t *link_info = bt_cm_get_link_information(rem_dev->address);
    bt_cm_assert(NULL != link_info);
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS state update req:0x%x, now:0x%x, link state:0x%x", 3,
        rem_dev->req_aws_state, rem_dev->aws_state, link_info->link_state);
    if (BT_CM_PROFILE_SERVICE_STATE_CONNECTED == rem_dev->req_aws_state) {
        if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == rem_dev->aws_state) {
            if (BT_CM_ACL_LINK_DISCONNECTED == link_info->link_state ||
                BT_CM_ACL_LINK_PENDING_CONNECT == link_info->link_state ||
                BT_CM_ACL_LINK_CONNECTING == link_info->link_state) {
                status = bt_aws_mce_connect(&(rem_dev->aws_handle), (const bt_bd_addr_t *)&(rem_dev->address));
            } else if (BT_CM_ACL_LINK_CONNECTED <= link_info->link_state) {
                status = bt_aws_mce_srv_agent_set_state(rem_dev, true);
            }
            if (BT_STATUS_SUCCESS != status) {
                bt_cmgr_report_id("[BT_CM][AWS_MCE][E] AWS connect fail status:0x%x, link state:%d.", 2, status, link_info->link_state);
            } else {
                rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTING;
                bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address, rem_dev->aws_state, BT_STATUS_SUCCESS);
            }
        }
    }
    if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == rem_dev->req_aws_state || (g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_CONNECT_DISABLED)) {
#ifdef AIR_MULTI_POINT_ENABLE
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS disconnect reason :0x%x", 1, rem_dev->aws_link_disconnect_reason);
#endif
        if(g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_SET_CONNECTABLE_FAIL){
            bt_cmgr_report_id("[BT_CM][AWS_MCE][E] BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED clean flag", 0);
            g_bt_aws_mce_srv_cnt_t.flags &= ~BT_AWS_MCE_SRV_FLAGS_SET_CONNECTABLE_FAIL;
            g_bt_aws_mce_srv_cnt_t.data =  NULL;
        }
        if ((BT_CM_PROFILE_SERVICE_STATE_CONNECTED == rem_dev->aws_state ||
            BT_CM_PROFILE_SERVICE_STATE_CONNECTING == rem_dev->aws_state) &&
            BT_CM_ACL_LINK_CONNECTED <= link_info->link_state) {
            if (BT_STATUS_SUCCESS != (status = bt_aws_mce_srv_agent_set_state(rem_dev, false))) {
                bt_cmgr_report_id("[BT_CM][AWS_MCE][E] AWS disconnect fail status:0x%x, link state:%d.", 2, status, link_info->link_state);
                return;
            }
            rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTING;
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address, rem_dev->aws_state, BT_STATUS_SUCCESS);
        } else if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == rem_dev->aws_state) {
#ifdef AIR_MULTI_POINT_ENABLE
            rem_dev->aws_link_disconnect_reason = BT_CM_AWS_LINK_DISCONNECT_NORMAL;
#endif
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address, rem_dev->aws_state, BT_STATUS_SUCCESS);
        }
    }
}

extern void         bt_avm_set_wide_band_scan_flag(bool enable);
static void         bt_aws_mce_srv_partner_state_machine(bt_aws_mce_srv_dev_t *rem_dev)
{
    bt_status_t status = BT_STATUS_FAIL;
    if (BT_CM_PROFILE_SERVICE_STATE_CONNECTED == rem_dev->req_aws_state &&
        !(g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_CONNECT_DISABLED)) {
        if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == rem_dev->aws_state) {
            bt_avm_set_wide_band_scan_flag(true);
            if (BT_STATUS_SUCCESS != (status = bt_aws_mce_connect(&(rem_dev->aws_handle), (const bt_bd_addr_t *)&(rem_dev->address)))) {
                bt_avm_set_wide_band_scan_flag(false);
                bt_cmgr_report_id("[BT_CM][AWS_MCE][E] AWS connect fail status:0x%x.", 1, status);
                return;
            }
            rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTING;
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address,
                BT_CM_PROFILE_SERVICE_STATE_CONNECTING, BT_STATUS_SUCCESS);
        }
    } else if (BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED == rem_dev->req_aws_state ||
        (g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_CONNECT_DISABLED)) {
        if (BT_CM_PROFILE_SERVICE_STATE_CONNECTED == rem_dev->aws_state) {
            if (BT_STATUS_SUCCESS != (status = bt_aws_mce_disconnect(rem_dev->aws_handle))) {
                bt_cmgr_report_id("[BT_CM][AWS_MCE][E] AWS disconnect fail status:0x%x.", 1, status);
                return;
            }
            rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTING;
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address, rem_dev->aws_state, BT_STATUS_SUCCESS);
        } else if (BT_CM_PROFILE_SERVICE_STATE_CONNECTING == rem_dev->aws_state) {
            bt_avm_set_wide_band_scan_flag(false);
            rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address, rem_dev->aws_state, BT_STATUS_SUCCESS);
        }
    }
}

static void         bt_aws_mce_srv_client_state_machine(bt_aws_mce_srv_dev_t *rem_dev)
{
    bt_aws_mce_srv_partner_state_machine(rem_dev);
}

static void         bt_aws_mce_srv_state_update(bt_aws_mce_srv_dev_t *rem_dev)
{
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    if (NULL == rem_dev || (g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_INITINIG)) {
        return;
    }
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Aws mce srv state machine device:0x%x, role:0x%x, state:%d, req_state:%d", 4,
        *(uint32_t *)&(rem_dev->address), aws_role, rem_dev->aws_state, rem_dev->req_aws_state);
    if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        bt_aws_mce_srv_agent_state_machine(rem_dev);
    } else if (BT_AWS_MCE_ROLE_PARTNER == aws_role) {
        bt_aws_mce_srv_partner_state_machine(rem_dev);
    } else if (BT_AWS_MCE_ROLE_CLINET == aws_role) {
        bt_aws_mce_srv_client_state_machine(rem_dev);
    }
}

static void         bt_aws_mce_srv_state_change_handle(bt_aws_mce_state_change_ind_t *state, bt_status_t status)
{
    bt_aws_mce_srv_dev_t *rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_AWS_HANDLE, (void *)(state->handle));
    if (NULL == rem_dev) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] AWS state notify device not find by addr.", 0);
        return;
    }
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] State change state:0x%x, status:0x%x.", 2, state->state, status);
    if (BT_AWS_MCE_AGENT_STATE_INACTIVE == (state->state & 0xF0)) {
        rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
        if (rem_dev->aws_state == rem_dev->req_aws_state) {
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address, rem_dev->aws_state, BT_STATUS_SUCCESS);
        }
        bt_aws_mce_srv_dev_t *conn_dev = bt_aws_mce_srv_find_req_connected_dev_except(NULL);
        bt_aws_mce_srv_state_update(conn_dev);
    } else if (BT_AWS_MCE_AGENT_STATE_ATTACHED == (state->state & 0xF0) &&
        /* For RHO case, there will received attached state change, but can't send confirm packet to partner. */
        BT_CM_PROFILE_SERVICE_STATE_CONNECTING == rem_dev->aws_state) {
        rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
        bt_device_manager_aws_local_info_update();
        if (rem_dev->aws_state == rem_dev->req_aws_state) {
            /* Agent need send confirm packet to partner after it received partner connected event. */
            bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Send AWS confirm packet and paired list to parnter.", 0);
            bt_device_manager_remote_aws_sync_to_partner();
            bt_aws_mce_srv_send_packet(BT_AWS_MCE_SRV_SYNC_CONNECTION_CONFIRM, 0, NULL);
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address, rem_dev->aws_state, BT_STATUS_SUCCESS);
        } else {
            bt_aws_mce_srv_state_update(rem_dev);
        }
    } else if (BT_AWS_MCE_AGENT_STATE_CONNECTABLE == (state->state & 0xF0)) {
        rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTING;
        bt_aws_mce_srv_state_update(rem_dev);
    }
}

static void         bt_aws_mce_srv_connected_handle(bt_aws_mce_connected_t *conn, bt_status_t status)
{
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_srv_dev_t *rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, conn->address);
    if (status != BT_STATUS_SUCCESS) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Connected status error :0x%x.", 1, status);
        return;
    }
    if (NULL == (rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, conn->address))) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Need new context.", 0);
        uint8_t temp_addr[6] = {0};
        if (NULL != (rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, &temp_addr))) {
            memcpy(rem_dev->address, conn->address, sizeof(bt_bd_addr_t));
        } else {
            bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Connected aws OOM.", 0);
            return;
        }
    }
    rem_dev->aws_handle = conn->handle;
    bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
    if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
        if (!bt_cm_memcmp(&(rem_dev->address), local_addr, sizeof(bt_bd_addr_t))) {
            rem_dev->aws_ready = true;
        }
        bt_aws_mce_srv_state_update(rem_dev);
    } else if (((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) && !(aws_role & 0x0F)) {
        if (bt_cm_memcmp(&(rem_dev->address), local_addr, sizeof(bt_bd_addr_t))) {
            rem_dev->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
            rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
            bt_device_manager_aws_local_info_update();
            /* Since agent may not know the partner's address, we need sync it to agent. */
            bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Send partner/client address to agent.", 0);
            bt_aws_mce_srv_send_packet(BT_AWS_MCE_SRV_SYNC_PARTNER_ADDR_TO_AGENT, sizeof(bt_bd_addr_t), local_addr);
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address,
                BT_CM_PROFILE_SERVICE_STATE_CONNECTED, BT_STATUS_SUCCESS);
        } else {
            rem_dev->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
            rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
        }
    }
}

static void         bt_aws_mce_srv_disconnected_handle(bt_aws_mce_disconnected_t *disconn, bt_status_t status)
{
    bt_aws_mce_srv_dev_t *rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_AWS_HANDLE, (void *)(disconn->handle));
    if (NULL == rem_dev) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Fail can't find the dev by handle 0x%x.", 1, disconn->handle);
        return;
    }
    bt_cm_profile_service_state_t pre_req_state = rem_dev->req_aws_state; 
    if (BT_CM_PROFILE_SERVICE_STATE_CONNECTED == pre_req_state || BT_CM_PROFILE_SERVICE_STATE_CONNECTING == pre_req_state) {
        bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_AWS, rem_dev->address,
            BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED, status);
    }
    memset(rem_dev, 0, sizeof(bt_aws_mce_srv_dev_t));
}

static bt_status_t  bt_aws_mce_srv_switch_mode_cnf_handle();
static bt_status_t  bt_aws_mce_srv_basic_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS event msg:0x%x, status:0x%x, buff:0x%x.", 3, msg, status, buffer);
    switch (msg) {
    case BT_AWS_MCE_CONNECTED:
        bt_aws_mce_srv_connected_handle(buffer, status);
        break;
    case  BT_AWS_MCE_DISCONNECTED:
        bt_aws_mce_srv_disconnected_handle(buffer, status);
        break;
    case  BT_AWS_MCE_STATE_CHANGED_IND:
        bt_aws_mce_srv_state_change_handle(buffer, status);
        break;
    case BT_AWS_MCE_MODE_CHANGED:
        bt_aws_mce_srv_switch_mode_cnf_handle(buffer, status);
        break;
    case BT_AWS_MCE_AWS_READY_IND: {
        bt_aws_mce_ready_ind_t *ready = buffer;
        bt_aws_mce_srv_dev_t *rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_AWS_HANDLE, (void *)(ready->handle));
        if (NULL != rem_dev) {
            rem_dev->aws_ready = ready->aws_ready;
            bt_aws_mce_srv_state_update(rem_dev);
        }
    }
        break;
    case BT_GAP_WRITE_STORED_LINK_KEY_CNF:
        if (g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_INITINIG) {
            g_bt_aws_mce_srv_cnt_t.flags &= (~BT_AWS_MCE_SRV_FLAGS_INITINIG);
            bt_aws_mce_srv_dev_t *rem_dev = bt_aws_mce_srv_find_req_connected_dev_except(NULL);            
            bt_aws_mce_srv_state_update(rem_dev);
        }
        break;
    case BT_GAP_SET_ROLE_CNF:
        if (g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_SET_ROLE_PENDING) {
            g_bt_aws_mce_srv_cnt_t.flags &= (~BT_AWS_MCE_SRV_FLAGS_SET_ROLE_PENDING);
            bt_aws_mce_srv_dev_t *conn_dev = bt_aws_mce_srv_find_req_connected_dev_except(NULL);
            bt_aws_mce_srv_state_update(conn_dev);
        }
        break;
    case BT_GAP_ROLE_CHANGED_IND: {
        bt_bd_addr_t *rem_addr = NULL;
        bt_aws_mce_srv_dev_t *rem_dev = NULL;
        bt_gap_role_changed_ind_t *role_change = (bt_gap_role_changed_ind_t *)buffer;
        if (NULL != role_change && (role_change->local_role & BT_ROLE_SLAVE) &&
            NULL != (rem_addr = (bt_bd_addr_t *)bt_gap_get_remote_address(role_change->handle)) &&
            NULL != (rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, rem_addr))) {
            bt_aws_mce_srv_state_update(rem_dev);
        }
    }
        break;
    case BT_GAP_SNIFF_MODE_CHANGE_IND: {
        bt_bd_addr_t *rem_addr = NULL;
        bt_aws_mce_srv_dev_t *rem_dev = NULL;
        bt_gap_sniff_mode_changed_ind_t* ind = (bt_gap_sniff_mode_changed_ind_t *)buffer;
        if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role() &&
            NULL != ind && (BT_GAP_LINK_SNIFF_TYPE_ACTIVE == ind->sniff_status) &&
            NULL != (rem_addr = (bt_bd_addr_t *)bt_gap_get_remote_address(ind->handle)) &&
            NULL != (rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, rem_addr))) {
            bt_aws_mce_srv_state_update(rem_dev);
        }
    }
        break;
    default:
        break;
    }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    bt_cm_rho_gap_event_handle(msg, status, buffer);
#endif
    return BT_STATUS_SUCCESS;
}

static bt_status_t  bt_aws_mce_srv_handle_connect_cb(void *data)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Connect AWS profile with data 0x%x.", 1, *(uint32_t *)data);
    bt_aws_mce_srv_dev_t *rem_dev = NULL;
    if (NULL == (rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, data))) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Need new context.", 0);
        uint8_t temp_addr[6] = {0};
        if (NULL != (rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, &temp_addr))) {
            memcpy(rem_dev->address, data, sizeof(bt_bd_addr_t));
        } else {
            bt_cmgr_report_id("[BT_CM][AWS_MCE][E] Connect aws OOM.", 0);
            return BT_STATUS_FAIL;
        }
    }
    if (((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) &&
        BT_CM_PROFILE_SERVICE_STATE_DISCONNECTING == rem_dev->aws_state) {
        return BT_STATUS_FAIL;
    }
    rem_dev->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
    bt_aws_mce_srv_state_update(rem_dev);
    return status;
}

static bt_status_t  bt_aws_mce_srv_handle_disconnect_cb(void *data)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Disconnect AWS prfoile with data 0x%x.", 1, *(uint32_t *)data);
    bt_aws_mce_srv_dev_t *rem_dev = NULL;
    if (NULL == (rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, data))) {
        return BT_STATUS_FAIL;
    }
    if (((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) &&
        BT_CM_PROFILE_SERVICE_STATE_CONNECTING == rem_dev->aws_state) {
        return BT_STATUS_FAIL;
    }
    rem_dev->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
    bt_aws_mce_srv_state_update(rem_dev);
    return status;
}

static struct {
    bt_aws_mce_srv_mode_t   mode;
    bool                    param_invalid;
    bt_aws_mce_srv_mode_switch_t    param;
} g_aws_mce_switch_mode;
static bt_aws_mce_mode_t bt_aws_mce_srv_mode_revert(bt_aws_mce_srv_mode_t mode);
static bt_status_t  bt_aws_mce_srv_handle_power_on_cnf_cb()
{
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_key_t* aws_key = bt_device_manager_aws_local_info_get_key();
    bt_bd_addr_t* aws_addr = NULL;
    bt_aws_mce_mode_t   req_mode = bt_aws_mce_srv_mode_revert(bt_device_manager_aws_local_info_get_mode());
    bt_gap_link_policy_setting_t link_policy;
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS profile service power on init, role:0x%02x req mode0x%02x.", 2, aws_role, req_mode);
    if (BT_AWS_MCE_MODE_DEFAULT != req_mode) {
        link_policy.sniff_mode = BT_GAP_LINK_POLICY_DISABLE;
    } else {
        link_policy.sniff_mode = BT_GAP_LINK_POLICY_ENABLE;;
    }
    bt_gap_write_default_link_policy(&link_policy);
    if (BT_AWS_MCE_ROLE_NONE == aws_role) {
        g_bt_aws_mce_srv_cnt_t.flags &= (~BT_AWS_MCE_SRV_FLAGS_SWITCH_MODE_RESET);
        bt_aws_mce_init_role(BT_AWS_MCE_ROLE_NONE);
        return BT_STATUS_FAIL;
    }
    /* bt_callback_manager_register_callback(bt_callback_type_app_event,
        (uint32_t)(MODULE_MASK_AWS_MCE | MODULE_MASK_GAP), (void *)bt_aws_mce_srv_basic_event_callback); */
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_CM, bt_aws_mce_srv_packet_callback);
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    bt_cm_rho_init();
#endif
    if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        aws_addr = bt_device_manager_aws_local_info_get_peer_address();
    } else {
        aws_addr = bt_device_manager_get_local_address();
    }
    bt_gap_stored_link_key_t key_list;
    bt_cm_memcpy(&(key_list.address), aws_addr, sizeof(bt_bd_addr_t));
    bt_cm_memcpy(&(key_list.key), aws_key, sizeof(bt_key_t));
    bt_gap_write_stored_link_key_param_t key_param = {
        .key_number = 1,
        .key_list = &key_list
    };
    bt_gap_delete_stored_link_key_param_t de_param;
    de_param.delete_all_flag = 0x01;
    if (BT_AWS_MCE_ROLE_PARTNER == aws_role) {
        aws_role |= BT_AWS_MCE_ROLE_CLINET;
    }
    bt_aws_mce_init_role(aws_role);
    if (BT_AWS_MCE_MODE_DEFAULT != req_mode && !(g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_SWITCH_MODE_RESET)) {
        g_aws_mce_switch_mode.mode = bt_device_manager_aws_local_info_get_mode();
        g_aws_mce_switch_mode.param_invalid = false;
        bt_cm_assert(BT_STATUS_SUCCESS == bt_aws_mce_set_mode(req_mode, NULL));
    }
    g_bt_aws_mce_srv_cnt_t.flags &= (~BT_AWS_MCE_SRV_FLAGS_SWITCH_MODE_RESET);
    bt_status_t ret = bt_gap_delete_stored_link_key(&de_param);
    ret = bt_gap_write_stored_link_key(&key_param);
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Write AWS key result:0x%x.", 1, ret);
    bt_cm_assert(BT_STATUS_SUCCESS == ret);
    g_bt_aws_mce_srv_cnt_t.flags |= BT_AWS_MCE_SRV_FLAGS_INITINIG;
    return BT_STATUS_SUCCESS;
}

static bt_status_t  bt_aws_mce_srv_handle_power_off_cnf_cb()
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS profile service power off deinit.", 0);
    bt_aws_mce_srv_flags_t ls_disalbe = (g_bt_aws_mce_srv_cnt_t.flags & (BT_AWS_MCE_SRV_FLAGS_CONNECT_DISABLED | BT_AWS_MCE_SRV_FLAGS_SWITCH_MODE_RESET));
    bt_cm_memset(&g_bt_aws_mce_srv_cnt_t, 0, sizeof(g_bt_aws_mce_srv_cnt_t));
    g_bt_aws_mce_srv_cnt_t.flags = ls_disalbe;
    /* bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_aws_mce_srv_basic_event_callback); */
    bt_aws_mce_report_deregister_callback(BT_AWS_MCE_REPORT_MODULE_CM);
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    bt_cm_rho_deinit();
#endif
    return BT_STATUS_SUCCESS;
}

static bt_status_t  bt_aws_mce_srv_profile_service_handle_cb(bt_cm_profile_service_handle_t type, void *data)
{
    bt_status_t status = BT_STATUS_FAIL;
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Connection manager handle type:0x%02x.", 1, type);
    switch (type) {
        case BT_CM_PROFILE_SERVICE_HANDLE_POWER_ON:
            status = bt_aws_mce_srv_handle_power_on_cnf_cb();
            break;
        case BT_CM_PROFILE_SERVICE_HANDLE_POWER_OFF:
            status = bt_aws_mce_srv_handle_power_off_cnf_cb();
            break;
        case BT_CM_PROFILE_SERVICE_HANDLE_CONNECT:
            status = bt_aws_mce_srv_handle_connect_cb(data);
            break;
        case BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT:
            status = bt_aws_mce_srv_handle_disconnect_cb(data);
            break;
        default:
            break;
    }
    return status;
}

bt_aws_mce_srv_link_type_t
                    bt_aws_mce_srv_get_link_type()
{
    bt_bd_addr_t aws_device;
    if (bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &aws_device, 1)) {
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
        bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
        if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
            if (bt_cm_memcmp(&aws_device, local_addr, sizeof(bt_bd_addr_t))) {
                return BT_AWS_MCE_SRV_LINK_NORMAL;
            } else {
                return BT_AWS_MCE_SRV_LINK_SPECIAL;
            }
        } else if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
            if (bt_cm_get_gap_handle(*local_addr)) {
                return BT_AWS_MCE_SRV_LINK_NORMAL;
            } else {
                return BT_AWS_MCE_SRV_LINK_SPECIAL;
            }
        }
    }
    return BT_AWS_MCE_SRV_LINK_NONE;
}

uint32_t            bt_aws_mce_srv_get_aws_handle(bt_bd_addr_t *addr)
{
    bt_aws_mce_srv_dev_t *aws_device = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, (void *)addr);
    return ((NULL != aws_device) ? aws_device->aws_handle : 0);
}

static bt_status_t  bt_aws_mce_srv_switch_role_reset_callback(bt_cm_power_reset_progress_t type, void *user_data)
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Switch role reset progress: %d, dest role :0x%x", 2, type, user_data);
    bt_aws_mce_role_t dest_role = (bt_aws_mce_role_t)(uint32_t)user_data;
    if (BT_CM_POWER_RESET_PROGRESS_MEDIUM == type) {
        if (bt_device_manager_aws_local_info_get_role() != dest_role) {
        bt_bd_addr_t    local_addr = {0};
        bt_bd_addr_t    *peer_addr = bt_device_manager_aws_local_info_get_peer_address();
        bt_device_manager_aws_local_info_store_role(dest_role);
        bt_cm_memcpy((void*)&local_addr, (void*)bt_device_manager_get_local_address(), sizeof(bt_bd_addr_t));

        bt_device_manager_aws_local_info_store_local_address(peer_addr);
        bt_device_manager_aws_local_info_store_peer_address(&local_addr);
        }
        bt_set_local_public_address((void*)bt_device_manager_get_local_address());
    } else if (BT_CM_POWER_RESET_PROGRESS_COMPLETE == type) {
        bt_aws_mce_srv_switch_role_complete_ind_t result = {
            .result = BT_STATUS_SUCCESS,
            .cur_aws_role = dest_role
        };
        bt_aws_mce_srv_event_callback(BT_AWS_MCE_SRV_EVENT_ROLE_CHANGED_IND, &result, sizeof(result));
        bt_cm_register_callback_notify(BT_AWS_MCE_SRV_EVENT_ROLE_CHANGED_IND, &result, sizeof(result));
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_aws_mce_srv_switch_role(bt_aws_mce_role_t dest_role)
{
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    uint32_t temp_data = dest_role;
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] switch role now:0x%x, dest:0x%x", 2, aws_role, dest_role);
    if (aws_role == dest_role) {
        return BT_STATUS_FAIL;
    }
    return bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_ROLE_RECOVERY, bt_aws_mce_srv_switch_role_reset_callback, (void *)temp_data);
}

bt_status_t         bt_aws_mce_srv_set_aws_disable(bool enable)
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Set aws disable %d", 1, enable);
    bt_cm_mutex_lock();
    if (true == enable) {
        g_bt_aws_mce_srv_cnt_t.flags |= BT_AWS_MCE_SRV_FLAGS_CONNECT_DISABLED;
    } else {
        extern bool bt_avm_allow_poweroff(void *data);
        if (false == bt_avm_allow_poweroff(NULL)) {
            bt_cm_mutex_unlock();
            return BT_STATUS_FAIL;
        }
        g_bt_aws_mce_srv_cnt_t.flags &= (~BT_AWS_MCE_SRV_FLAGS_CONNECT_DISABLED);
    }
    bt_aws_mce_srv_dev_t *rem_dev = bt_aws_mce_srv_find_req_connected_dev_except(NULL);
    bt_aws_mce_srv_state_update(rem_dev);
    bt_cm_mutex_unlock();
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_aws_mce_retry_set_state()
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][E] bt_aws_mce_retry_set_state ", 0);
    if((g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_SET_CONNECTABLE_FAIL) &&
        g_bt_aws_mce_srv_cnt_t.data){
        if(BT_STATUS_SUCCESS == bt_aws_mce_set_state(((bt_aws_mce_srv_dev_t*)(g_bt_aws_mce_srv_cnt_t.data))->aws_handle, BT_AWS_MCE_AGENT_STATE_CONNECTABLE)) {
            bt_cmgr_report_id("[BT_CM][AWS_MCE][E] bt_aws_mce_retry_set_state clean ", 0);
            g_bt_aws_mce_srv_cnt_t.flags &= ~BT_AWS_MCE_SRV_FLAGS_SET_CONNECTABLE_FAIL;
            g_bt_aws_mce_srv_cnt_t.data = NULL;
        }
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] bt_aws_mce_retry_set_state ", 0);
    }

    return BT_STATUS_SUCCESS;
}

uint8_t             bt_aws_mce_srv_rho_get_aws_ready(bt_bd_addr_t *remote_addr)
{
    
    bt_aws_mce_srv_dev_t *aws_device = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, (void *)remote_addr);
    if (NULL == aws_device) {
        return true;
    }
    return aws_device->aws_ready;
}

void                bt_aws_mce_srv_rho_complete(bt_bd_addr_t remote_addr, bool active, uint8_t aws_ready)
{
    bt_aws_mce_role_t aws_role = bt_device_manager_aws_local_info_get_role();
    bt_bd_addr_t *local_addr = bt_device_manager_aws_local_info_get_local_address();
    bt_bd_addr_t *peer_addr = bt_device_manager_aws_local_info_get_peer_address();
    bt_aws_mce_srv_dev_t *specail_aws_device = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, (void *)peer_addr);
    bt_aws_mce_srv_dev_t *normal_aws_device = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_AWS_STATE, (void *)BT_CM_PROFILE_SERVICE_STATE_CONNECTED);
    if (active) {
        bt_cm_assert(NULL != specail_aws_device && NULL != normal_aws_device);
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Aws mce srv rho complete special, role:0x%x, state:%d, req_state:%d remote addr:0x%x", 4,
            aws_role, specail_aws_device->aws_state, specail_aws_device->req_aws_state, *(uint32_t *)remote_addr);
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Aws mce srv rho complete normal, state:%d, req_state:%d", 2,
            normal_aws_device->aws_state, normal_aws_device->req_aws_state);
    }
    if (BT_AWS_MCE_ROLE_AGENT == aws_role) {
        if (active) {
            bt_cm_memcpy(&(specail_aws_device->address), local_addr, sizeof(bt_bd_addr_t));
            specail_aws_device->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
            specail_aws_device->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
            specail_aws_device->aws_ready = true;
            bt_cm_memcpy(&(normal_aws_device->address), remote_addr, sizeof(bt_bd_addr_t));
            normal_aws_device->aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
            normal_aws_device->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
            normal_aws_device->aws_ready = true;
        } else {
            uint8_t temp_addr[6] = {0};
            bt_aws_mce_srv_dev_t *rem_dev;
            if (NULL != (rem_dev = bt_aws_mce_srv_find(BT_AWS_MCE_SRV_FIND_BY_ADDR, &temp_addr))) {
                bt_cm_memcpy(rem_dev->address, remote_addr, sizeof(bt_bd_addr_t));
                rem_dev->aws_handle = bt_aws_mce_query_handle_by_address(BT_MODULE_AWS_MCE, (void *)remote_addr);
                rem_dev->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
                rem_dev->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
                rem_dev->aws_ready = aws_ready;
            }
        }
    } else if ((BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER) & aws_role) {
        for (uint32_t i = 0; i < BT_AWS_MCE_SRV_SUPPORT_NUM; ++i) {
            if (bt_cm_memcmp(peer_addr, &g_bt_aws_mce_srv_cnt_t.aws_dev[i].address, sizeof(bt_bd_addr_t)) &&
                bt_cm_memcmp(remote_addr, &g_bt_aws_mce_srv_cnt_t.aws_dev[i].address, sizeof(bt_bd_addr_t))) {
                bt_cm_memset(&g_bt_aws_mce_srv_cnt_t.aws_dev[i], 0 , sizeof(bt_aws_mce_srv_dev_t));
                break;
            }
        }
        bt_cm_memcpy(&(specail_aws_device->address), local_addr, sizeof(bt_bd_addr_t));
        specail_aws_device->aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
        specail_aws_device->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED;
        bt_cm_memcpy(&(normal_aws_device->address), peer_addr, sizeof(bt_bd_addr_t));
        normal_aws_device->aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
        normal_aws_device->req_aws_state = BT_CM_PROFILE_SERVICE_STATE_CONNECTED;
    }
}

static bt_aws_mce_mode_t bt_aws_mce_srv_mode_revert(bt_aws_mce_srv_mode_t mode)
{
    if (BT_AWS_MCE_SRV_MODE_DOUBLE == mode) {
        return BT_AWS_MCE_MODE_DOUBLE;
    } else if (BT_AWS_MCE_SRV_MODE_BROADCAST == mode) {
        return BT_AWS_MCE_MODE_BROADCAST;
    }
    return BT_AWS_MCE_MODE_DEFAULT;
}

static bt_status_t  bt_aws_mce_srv_switch_mode_cnf_handle(bt_aws_mce_set_mode_cnf_t *mode_cnf, bt_status_t status)
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Switch mode confirm ", 0);
    bt_aws_mce_role_t   pre_role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_srv_mode_t pre_srv_mode = bt_device_manager_aws_local_info_get_mode();
    bt_aws_mce_mode_t   req_mode = bt_aws_mce_srv_mode_revert(g_aws_mce_switch_mode.mode);
    bt_aws_mce_mode_t   pre_mode = mode_cnf->mode;
    bt_aws_mce_srv_mode_changed_ind_t result = {
        .mode = g_aws_mce_switch_mode.mode,
        .pre_mode = pre_srv_mode,
        .role = pre_role,
        .pre_role = pre_role
    };
    if (req_mode != pre_mode) {
        if (BT_AWS_MCE_MODE_DEFAULT == pre_mode) {
            bt_cm_assert(BT_STATUS_SUCCESS == bt_aws_mce_set_mode(req_mode, NULL));
        } else if (BT_AWS_MCE_MODE_PLUS != pre_mode) {
            bt_cm_assert(BT_STATUS_SUCCESS == bt_aws_mce_set_mode(BT_AWS_MCE_MODE_DEFAULT, NULL));
        }
        return BT_STATUS_SUCCESS;
    }
    bt_device_manager_aws_local_info_store_mode(g_aws_mce_switch_mode.mode);
    if (true == g_aws_mce_switch_mode.param_invalid) {
        bt_device_manager_aws_local_info_store_role(g_aws_mce_switch_mode.param.role);
        result.role = g_aws_mce_switch_mode.param.role;
        if (g_aws_mce_switch_mode.param.role & (BT_AWS_MCE_ROLE_PARTNER | BT_AWS_MCE_ROLE_CLINET)) {
            bt_device_manager_aws_local_info_store_peer_address(&(g_aws_mce_switch_mode.param.addr));    
        }
    }
    bt_device_manager_aws_local_info_update();
    bt_aws_mce_srv_event_callback(BT_AWS_MCE_SRV_EVENT_MODE_CHANGED_IND, &result, sizeof(result));
    bt_cm_register_callback_notify(BT_AWS_MCE_SRV_EVENT_MODE_CHANGED_IND, &result, sizeof(result));
    return BT_STATUS_SUCCESS;
}

static bt_status_t  bt_aws_mce_srv_switch_mode_reset_callback(bt_cm_power_reset_progress_t type, void *user_data)
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Switch mode reset progress: %d", 1, type);
    bt_aws_mce_role_t pre_role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_srv_mode_t pre_srv_mode = bt_device_manager_aws_local_info_get_mode();
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Switch mode pre role:0x%x, pre mode:0x%x, dest mode:0x%x", 3, pre_role, pre_srv_mode, g_aws_mce_switch_mode.mode);
    if (BT_CM_POWER_RESET_PROGRESS_COMPLETE == type) {
        bt_aws_mce_set_mode_cnf_t temp_mode;
        temp_mode.mode = bt_aws_mce_srv_mode_revert(pre_srv_mode);
        g_bt_aws_mce_srv_cnt_t.flags |= BT_AWS_MCE_SRV_FLAGS_SWITCH_MODE_RESET;
        bt_aws_mce_srv_switch_mode_cnf_handle(&temp_mode, BT_STATUS_SUCCESS);
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_aws_mce_srv_switch_mode(bt_aws_mce_srv_mode_t mode, bt_aws_mce_srv_mode_switch_t* param)
{
    g_aws_mce_switch_mode.mode = mode;
    if (NULL != param) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Switch mode:%d, role:%d", 2, mode, param->role);
        g_aws_mce_switch_mode.param_invalid = true;
        bt_cm_memcpy(&(g_aws_mce_switch_mode.param), param, sizeof(bt_aws_mce_srv_mode_switch_t));
    } else {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Switch mode:%d", 1, mode);
        g_aws_mce_switch_mode.param_invalid = false;
    }
    return bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_ROLE_RECOVERY, bt_aws_mce_srv_switch_mode_reset_callback, NULL);
}

bt_aws_mce_srv_mode_t
                    bt_aws_mce_srv_get_mode(void)
{
    return bt_device_manager_aws_local_info_get_mode();
}

static bt_aws_mce_srv_case_pairing_t g_aws_mce_case_pairing_cnt;

static bt_status_t  bt_aws_mce_srv_case_pairing_bt_reset_callback(bt_cm_power_reset_progress_t type, void *user_data)
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Case pairing progress: %d", 1, type);
    if (BT_CM_POWER_RESET_PROGRESS_MEDIUM == type) {
        bt_bd_addr_t *aws_fixed_addr = bt_device_manager_aws_local_info_get_fixed_address();
        bt_set_local_public_address((void *)aws_fixed_addr);
        bt_device_manager_aws_local_info_store_local_address(aws_fixed_addr);
        bt_device_manager_aws_local_info_store_peer_address(&(g_aws_mce_case_pairing_cnt.peer_address));
        bt_device_manager_aws_local_info_store_role(g_aws_mce_case_pairing_cnt.dest_role);
        bt_device_manager_aws_local_info_store_key(&(g_aws_mce_case_pairing_cnt.aws_key));
        bt_device_manager_aws_local_info_update();
    } else if (BT_CM_POWER_RESET_PROGRESS_COMPLETE == type) {
        bt_aws_mce_srv_switch_role_complete_ind_t result = {
            .result = BT_STATUS_SUCCESS,
            .cur_aws_role = g_aws_mce_case_pairing_cnt.dest_role
        };
        g_bt_aws_mce_srv_cnt_t.flags &= (~BT_AWS_MCE_SRV_FLAGS_CASE_PAIRING);
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Case pairing complete", 0);
        bt_aws_mce_srv_event_callback(BT_AWS_MCE_SRV_EVENT_CASE_PAIRING_COMPLETE, &result, sizeof(result));
        bt_cm_register_callback_notify(BT_AWS_MCE_SRV_EVENT_CASE_PAIRING_COMPLETE, &result, sizeof(result));
    }
    return BT_STATUS_SUCCESS;
}

static bt_status_t  bt_aws_mce_srv_unpair_bt_reset_callback(bt_cm_power_reset_progress_t type, void *user_data)
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Aws unpair progress: %d", 1, type);
    if (BT_CM_POWER_RESET_PROGRESS_MEDIUM == type) {
        bt_device_manager_aws_reset();
        bt_set_local_public_address((void*)bt_device_manager_get_local_address());
    } else if (BT_CM_POWER_RESET_PROGRESS_COMPLETE == type) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Aws unpair complete", 0);    
        bt_aws_mce_srv_event_callback(BT_AWS_MCE_SRV_EVENT_UNPAIR_COMPLETE, NULL, 0);
        bt_cm_register_callback_notify(BT_AWS_MCE_SRV_EVENT_UNPAIR_COMPLETE, NULL, 0);
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_aws_mce_srv_case_pairing_start(const bt_aws_mce_srv_case_pairing_t *param)
{
    if (NULL == param) {
        return BT_CM_STATUS_INVALID_PARAM;
    }
    if (g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_CASE_PAIRING) {
        return BT_CM_STATUS_PENDING;
    }
    bt_bd_addr_t *aws_fixed_addr = bt_device_manager_aws_local_info_get_fixed_address();
    bt_cm_assert(bt_cm_memcmp(aws_fixed_addr, &(param->peer_address), sizeof(bt_bd_addr_t)) && "Case pairing used the same fixed address!!!");
    g_bt_aws_mce_srv_cnt_t.flags |= BT_AWS_MCE_SRV_FLAGS_CASE_PAIRING;
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Case pairing start dest role 0x%02X", 1, param->dest_role);
    bt_cm_memcpy(&(g_aws_mce_case_pairing_cnt), param, sizeof(*param));
    if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
        bt_aws_mce_srv_case_pairing_bt_reset_callback(BT_CM_POWER_RESET_PROGRESS_MEDIUM, NULL);
        bt_aws_mce_srv_case_pairing_bt_reset_callback(BT_CM_POWER_RESET_PROGRESS_COMPLETE, NULL);
    }
    return bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_COMPLETE, bt_aws_mce_srv_case_pairing_bt_reset_callback, NULL);
}

bt_status_t         bt_aws_mce_srv_unpair()
{
    if (g_bt_aws_mce_srv_cnt_t.flags & BT_AWS_MCE_SRV_FLAGS_CASE_PAIRING) {
        bt_cmgr_report_id("[BT_CM][AWS_MCE][E] In Case pairing state", 0);
        return BT_CM_STATUS_PENDING;
    }
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] Aws unpair ", 0);
    if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
        bt_aws_mce_srv_unpair_bt_reset_callback(BT_CM_POWER_RESET_PROGRESS_MEDIUM, NULL);
        bt_aws_mce_srv_unpair_bt_reset_callback(BT_CM_POWER_RESET_PROGRESS_COMPLETE, NULL);
    }
    return bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_AIR_PAIRING_COMPLETE, bt_aws_mce_srv_unpair_bt_reset_callback, NULL);
}

void                bt_aws_mce_srv_init()
{
    bt_cmgr_report_id("[BT_CM][AWS_MCE][I] AWS profile service init.", 0);
    bt_cm_memset(&g_bt_aws_mce_srv_cnt_t, 0, sizeof(g_bt_aws_mce_srv_cnt_t));
    bt_cm_profile_service_register(BT_CM_PROFILE_SERVICE_AWS, &bt_aws_mce_srv_profile_service_handle_cb);
    bt_callback_manager_register_callback(bt_callback_type_app_event,
        (uint32_t)(MODULE_MASK_AWS_MCE | MODULE_MASK_GAP), (void *)bt_aws_mce_srv_basic_event_callback);
}


