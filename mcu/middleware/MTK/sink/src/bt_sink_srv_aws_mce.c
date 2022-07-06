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
 
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif /* MTK_NVDM_ENABLE */
#include "bt_sink_srv_aws_mce.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_common.h"
#include "bt_connection_manager_internal.h"
#include "bt_sink_srv_state_notify.h"
#include "bt_avm.h"
#ifdef BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT
#include "bt_role_handover.h"
#include "bt_device_manager.h"
#endif /* BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT */

extern bt_sink_srv_aws_mce_context_t* bt_sink_srv_aws_mce_get_context_by_handle(uint32_t aws_handle);
extern void bt_sink_srv_cm_aws_mce_state_notify(bt_bd_addr_t *addr, bt_aws_mce_agent_state_type_t state);

extern bt_status_t bt_sink_srv_aws_mce_music_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
extern bt_status_t bt_sink_srv_aws_mce_call_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
extern void* bt_sink_srv_aws_mce_get_music_module(uint8_t device_idx);
extern void* bt_sink_srv_aws_mce_get_call_module(uint8_t device_idx);

#ifdef BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT
static bt_status_t bt_sink_srv_aws_mce_status_callback(const bt_bd_addr_t *address, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status);
#endif /* BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT */

bt_status_t default_bt_sink_srv_aws_mce_music_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t default_bt_sink_srv_aws_mce_call_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return BT_STATUS_SUCCESS;
}

void* default_bt_sink_srv_aws_mce_get_music_module(uint8_t device_idx)
{
    return NULL;
}
void* default_bt_sink_srv_aws_mce_get_call_module(uint8_t device_idx)
{
    return NULL;
}

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_sink_srv_aws_mce_music_callback=_default_bt_sink_srv_aws_mce_music_callback")
#pragma comment(linker, "/alternatename:_bt_sink_srv_aws_mce_call_callback=_default_bt_sink_srv_aws_mce_call_callback")
#pragma comment(linker, "/alternatename:_bt_sink_srv_aws_mce_get_music_module=_default_bt_sink_srv_aws_mce_get_music_module")
#pragma comment(linker, "/alternatename:_bt_sink_srv_aws_mce_get_call_module=_default_bt_sink_srv_aws_mce_get_call_module")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_sink_srv_aws_mce_music_callback = default_bt_sink_srv_aws_mce_music_callback
#pragma weak bt_sink_srv_aws_mce_call_callback = default_bt_sink_srv_aws_mce_call_callback
#pragma weak bt_sink_srv_aws_mce_get_music_module = default_bt_sink_srv_aws_mce_get_music_module
#pragma weak bt_sink_srv_aws_mce_get_call_module = default_bt_sink_srv_aws_mce_get_call_module
#else
#error "Unsupported Platform"
#endif

static bt_sink_srv_aws_mce_context_t bt_sink_srv_aws_mce_cntx[BT_SINK_SRV_AWS_MCE_CONNECTION_NUM];

static const bt_sink_srv_aws_mce_cb_t aws_mce_cb[BT_SINK_SRV_AWS_MCE_MODULE_INDEX_NUM] = 
{
    {
        bt_sink_srv_aws_mce_music_callback
    },
    {
        bt_sink_srv_aws_mce_call_callback
    }
};

void* bt_sink_srv_cm_get_aws_info(uint8_t device_idx)
{
    return &bt_sink_srv_aws_mce_cntx[device_idx];
}

static bt_sink_srv_aws_mce_context_t *bt_sink_srv_aws_mce_get_free_context(void)
{
    bt_sink_srv_aws_mce_context_t *context = NULL;

    for (uint8_t i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM;i++)
    {
        if (bt_sink_srv_aws_mce_cntx[i].aws_handle == BT_AWS_MCE_INVALID_HANDLE)
        {
            context = &bt_sink_srv_aws_mce_cntx[i];
            break;
        }
    }

    bt_sink_srv_report_id("[SINK][AWS_MCE]Get free context, context:0x%x", 1, context);
    return context;
}

static bt_sink_srv_aws_mce_context_t *bt_sink_srv_aws_mce_get_context_by_address(const bt_bd_addr_t *address)
{
    bt_sink_srv_aws_mce_context_t *context = NULL;
    
    if (address == NULL)
    {
        bt_sink_srv_report_id("[SINK][AWS_MCE]Get context by address, INVALID address!", 0);
        return NULL;
    }

    for (uint8_t i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM; i++)
    {
        if ((bt_sink_srv_aws_mce_cntx[i].aws_handle != BT_AWS_MCE_INVALID_HANDLE) &&
            (bt_sink_srv_memcmp(address, bt_aws_mce_get_bd_addr_by_handle(bt_sink_srv_aws_mce_cntx[i].aws_handle), sizeof(bt_bd_addr_t)) == 0))
        {
            context = &bt_sink_srv_aws_mce_cntx[i];
            break;
        }
    }

    bt_sink_srv_report_id("[SINK][AWS_MCE]Get context by address, context:0x%x", 1, context);
    return context;
}

void* bt_sink_srv_aws_mce_get_module_info(bt_bd_addr_t *address, bt_sink_srv_aws_mce_module_index_t module)
{
    void *module_info = NULL;
    //bt_sink_srv_aws_mce_context_t* aws_cntx = bt_sink_srv_cm_get_profile_info(address, BT_SINK_SRV_PROFILE_AWS);
    bt_sink_srv_aws_mce_context_t *aws_cntx = bt_sink_srv_aws_mce_get_context_by_address((const bt_bd_addr_t *)address);
    if (aws_cntx) {
        if (module < BT_SINK_SRV_AWS_MCE_MODULE_INDEX_NUM) {
            module_info = aws_cntx->module_info[module];
        }
    }
    bt_sink_srv_report_id("[SINK][AWS_MCE] Get module info:0x%x", 1, module_info);
    return module_info;
}

void* bt_sink_srv_aws_mce_get_module_info_by_handle(uint32_t aws_handle, bt_sink_srv_aws_mce_module_index_t module)
{
    void *module_info = NULL;
    bt_sink_srv_aws_mce_context_t *aws_cntx = bt_sink_srv_aws_mce_get_context_by_handle(aws_handle);
    if (aws_cntx) {
        if (module < BT_SINK_SRV_AWS_MCE_MODULE_INDEX_NUM) {
            module_info = aws_cntx->module_info[module];
        }
    }
    bt_sink_srv_report_id("[SINK][AWS_MCE] Get module info:0x%x", 1, module_info);
    return module_info;
}

bt_aws_mce_agent_state_type_t bt_sink_srv_aws_mce_get_aws_state_by_handle(uint32_t aws_handle)
{
    bt_aws_mce_agent_state_type_t state = BT_AWS_MCE_AGENT_STATE_NONE;
    bt_sink_srv_aws_mce_context_t *aws_cntx = bt_sink_srv_aws_mce_get_context_by_handle(aws_handle);
    if (aws_cntx) {
        state = aws_cntx->state;
    }
    bt_sink_srv_report_id("[SINK][AWS_MCE] Get aws state:0x%x", 1, state);
    return state;
}

bt_bd_addr_t *bt_sink_srv_aws_mce_get_module_address(void* module_info)
{
    bt_bd_addr_t *addr = NULL;
    if (NULL != module_info) {
        bt_sink_srv_aws_mce_context_t* aws_cntx = NULL;
        uint8_t i, j;
        // search module aws connection
        for (i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM; ++i) {
            // search module info
            for (j = 0; j < BT_SINK_SRV_AWS_MCE_MODULE_INDEX_NUM; ++j) {
                if (NULL != bt_sink_srv_aws_mce_cntx[i].module_info[j]
                && module_info == bt_sink_srv_aws_mce_cntx[i].module_info[j]){
                    aws_cntx = &bt_sink_srv_aws_mce_cntx[i];
                    break;
                }
            }
            if (aws_cntx) {
                //addr = bt_sink_srv_cm_get_address_by_profile_info((void*)aws_cntx);
                addr = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(aws_cntx->aws_handle);
                break;
            }
        }
    }
    bt_sink_srv_report_id("[SINK][AWS_MCE] get addr:0x%x", 1, addr);
    return addr;
}

uint32_t bt_sink_srv_aws_mce_get_handle(bt_bd_addr_t *bt_addr)
{
    uint32_t aws_handle = 0;
    //bt_sink_srv_aws_mce_context_t* aws_cntx =
    //(bt_sink_srv_aws_mce_context_t* )bt_sink_srv_cm_get_profile_info(bt_addr, BT_SINK_SRV_PROFILE_AWS);
    bt_sink_srv_aws_mce_context_t *aws_cntx = bt_sink_srv_aws_mce_get_context_by_address((const bt_bd_addr_t *)bt_addr);
    if (aws_cntx) {
        aws_handle = aws_cntx->aws_handle;
    }
    bt_sink_srv_report_id("[SINK][AWS_MCE] Get aws handle:0x%x", 1, aws_handle);
    return aws_handle;
}

bt_bd_addr_t* bt_sink_srv_aws_mce_get_address(uint32_t aws_handle)
{
    bt_bd_addr_t* addr_p = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(aws_handle);
#if 0
    bt_sink_srv_aws_mce_context_t *aws_cntx = bt_sink_srv_aws_mce_get_context_by_handle(aws_handle);
    if (aws_cntx) {
        addr_p = bt_sink_srv_cm_get_address_by_profile_info((void *)aws_cntx);
    }
#endif
    bt_sink_srv_report_id("[SINK][AWS_MCE]Get addr:0x%x", 1, addr_p);
    if (addr_p) {
        bt_sink_srv_report_id("[SINK][AWS_MCE]Get addr:%02x:%02x:%02x:%02x:%02x:%02x", 6,
            (*addr_p)[5],(*addr_p)[4], (*addr_p)[3],(*addr_p)[2],(*addr_p)[1],(*addr_p)[0]);
    }
    return addr_p;
}

void bt_sink_srv_aws_mce_notify_state_change(bt_bd_addr_t *addr, bt_aws_mce_agent_state_type_t state) 
{
    bt_sink_srv_event_param_t *event = bt_sink_srv_memory_alloc(sizeof(*event));
    if (event) {
        bt_sink_srv_report_id("[SINK][AWS_MCE]Notify state to application:0x%x", 1, state);
        bt_sink_srv_memcpy(&(event->aws_state_change.bt_addr), (void *)addr, sizeof(bt_bd_addr_t));
        event->aws_state_change.aws_state = state;
        bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE, (void *)event, sizeof(*event));
        bt_sink_srv_memory_free(event);
    } else {
        bt_sink_srv_report_id("[SINK][AWS_MCE]Not enough heap memory.", 0);
    }
}

#if 0
uint32_t bt_sink_srv_aws_mce_get_module_gap_handle(void* module_info)
{
    uint32_t gap_handle = 0;
    if (NULL != module_info) {
        bt_sink_srv_aws_mce_context_t* aws_cntx = NULL;
        uint8_t i, j;
        // search module aws connection
        for (i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM; ++i) {
            // search module info
            for (j = 0; j < BT_SINK_SRV_AWS_MCE_MODULE_INDEX_NUM; ++j) {
                if (NULL != bt_sink_srv_aws_mce_cntx[i].module_info[j]
                && module_info == bt_sink_srv_aws_mce_cntx[i].module_info[j]){
                    aws_cntx = &bt_sink_srv_aws_mce_cntx[i];
                    break;
                }
            }
            if (aws_cntx) {
                gap_handle =  bt_sink_srv_cm_get_handle_by_profile_info((void*)aws_cntx);
                break;
            }
        }
    }
    bt_sink_srv_report_id("[SINK][AWS_MCE] get gap handle:0x%x", 1, gap_handle);
    return gap_handle;
}
#endif

void bt_sink_srv_aws_mce_init()
{
    int i = 0;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    for(i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM; i++) {
        bt_sink_srv_aws_mce_cntx[i].aws_handle = 0;
        //Init agent aws state is BT_AWS_MCE_AGENT_STATE_INACTIVE.
        if (BT_AWS_MCE_ROLE_AGENT == role) {
            bt_sink_srv_aws_mce_cntx[i].state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
        } else {
            bt_sink_srv_aws_mce_cntx[i].state = BT_AWS_MCE_AGENT_STATE_NONE;
        }
        bt_sink_srv_aws_mce_cntx[i].module_info[BT_SINK_SRV_AWS_MCE_MODULE_MUSIC] = bt_sink_srv_aws_mce_get_music_module(i);
        bt_sink_srv_aws_mce_cntx[i].module_info[BT_SINK_SRV_AWS_MCE_MODULE_CALL] = bt_sink_srv_aws_mce_get_call_module(i);
    }
#ifdef BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT
    bt_role_handover_callbacks_t callbacks = {
        .status_cb = (bt_role_handover_status_callback_t)bt_sink_srv_aws_mce_status_callback
    };
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_AWS_MCE_SRV, &callbacks);
#endif /* BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT */
}

void bt_sink_srv_aws_mce_deinit()
{
    uint32_t i = 0;
    for(i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM; i++) {
        bt_sink_srv_aws_mce_cntx[i].aws_handle = 0;
    }
#ifdef BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT
    bt_role_handover_deregister_callbacks(BT_ROLE_HANDOVER_MODULE_AWS_MCE_SRV);
#endif /* BT_SINK_SRV_AWS_MCE_IN_MULTI_POINT */
}

static void bt_sink_srv_aws_mce_reset_state(bt_aws_mce_agent_state_type_t state)
{
    bt_sink_srv_report_id("[SINK][AWS_MCE]Reset state, state:0x%x", 1, state);
    uint32_t i = 0;
    for(i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM; i++) {
        bt_sink_srv_aws_mce_cntx[i].state = state;
    }
}

bt_sink_srv_aws_mce_context_t* bt_sink_srv_aws_mce_get_context_by_handle(uint32_t aws_handle)
{
    bt_sink_srv_aws_mce_context_t* p_context = NULL;
    int i = 0;
    for( i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM; i++) {
        if (bt_sink_srv_aws_mce_cntx[i].aws_handle == aws_handle) {
            p_context = &bt_sink_srv_aws_mce_cntx[i];
            break;
        }
    }
    
    bt_sink_srv_report_id("[SINK][AWS_MCE]Get handle:0x%x, context:0x%x, index:%d", 3, aws_handle, p_context, i);
    return p_context;
}

static void bt_sink_srv_aws_mce_reset_context_by_handle(uint32_t aws_handle)
{
    bt_sink_srv_aws_mce_context_t* p_context = NULL;
    bt_sink_srv_report_id("[SINK][AWS_MCE]Reset context, handle:0x%x,", 1, aws_handle);
    p_context = bt_sink_srv_aws_mce_get_context_by_handle(aws_handle);
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (p_context){
        p_context->aws_handle = 0;
        if (BT_AWS_MCE_ROLE_AGENT == role) {
            p_context->state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
        } else {
            p_context->state = BT_AWS_MCE_AGENT_STATE_NONE;
        }
        p_context->conn_state = BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED;
    }
    return;
}

static void bt_sink_srv_aws_mce_notify_all(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    for(uint8_t i = 0; i < sizeof(aws_mce_cb) /sizeof(bt_sink_srv_aws_mce_cb_t); i++) {
        if (aws_mce_cb[i].aws_callback != NULL) {
            aws_mce_cb[i].aws_callback(msg, status, buffer);
        }
    }    
}

bt_status_t bt_sink_srv_aws_mce_common_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_sink_srv_report_id("[SINK][AWS_MCE]Common callback:%x, %x", 2, msg, status);
    switch (msg) {
        case BT_AWS_MCE_CONNECTED:{
            bt_aws_mce_connected_t *conn = (bt_aws_mce_connected_t *)buffer;
            if (status == BT_STATUS_SUCCESS) {
                //bt_sink_srv_aws_mce_context_t* aws_cntx = 
                    //bt_sink_srv_cm_get_profile_info(conn->address, BT_SINK_SRV_PROFILE_AWS);
                bt_sink_srv_aws_mce_context_t *aws_cntx = bt_sink_srv_aws_mce_get_context_by_handle(conn->handle);

                if (aws_cntx == NULL) {
                    aws_cntx = bt_sink_srv_aws_mce_get_free_context();
                }

                if (aws_cntx != NULL) {
                    aws_cntx->aws_handle = conn->handle;
                    aws_cntx->conn_state = BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED;
                #ifndef MTK_BT_CM_SUPPORT
                    bt_sink_srv_cm_profile_status_notify(conn->address, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED, status);
                #endif
                    bt_sink_srv_aws_mce_notify_all(msg, status, buffer);
                } else {
                    bt_sink_srv_report_id("[SINK][AWS_MCE] No free aws context.", 0);
                }
            }
        }
        break;
    
        case  BT_AWS_MCE_DISCONNECTED:{
            bt_aws_mce_disconnected_t *disconn = (bt_aws_mce_disconnected_t *)buffer;
            bt_sink_srv_aws_mce_notify_all(msg, status, buffer);
            bt_sink_srv_aws_mce_context_t* context = bt_sink_srv_aws_mce_get_context_by_handle(disconn->handle);
            if (context) {
            #ifndef MTK_BT_CM_SUPPORT
                //bt_bd_addr_t *addr = bt_sink_srv_cm_get_address_by_profile_info((void*)context);
                bt_bd_addr_t *addr = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(context->aws_handle);
                bt_sink_srv_cm_profile_status_notify(addr, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, status);
            #endif
                bt_sink_srv_aws_mce_reset_context_by_handle(disconn->handle);
            }
        }
        break;
        
        case  BT_AWS_MCE_STATE_CHANGED_IND: {
            bt_aws_mce_state_change_ind_t *state_change = (bt_aws_mce_state_change_ind_t *)buffer;
            bt_sink_srv_aws_mce_context_t* context = bt_sink_srv_aws_mce_get_context_by_handle(state_change->handle);
            if (context) {
                //bt_bd_addr_t *addr = bt_sink_srv_cm_get_address_by_profile_info((void*)context);
                bt_bd_addr_t *addr = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(context->aws_handle);
                bt_aws_mce_agent_state_type_t previous = context->state;
                bt_sink_srv_report_id("[SINK][AWS_MCE] state: previous:0x%x, new:0x%x.", 2, previous, state_change->state);
                //Special callback for connection manager.
            #ifndef MTK_BT_CM_SUPPORT
                bt_sink_srv_cm_aws_mce_state_notify(addr, state_change->state);
            #endif
                // RHO case: Agent -> Parnter.
                if (previous == BT_AWS_MCE_AGENT_STATE_ATTACHED && state_change->state == BT_AWS_MCE_AGENT_STATE_NONE) {
                    bt_sink_srv_aws_mce_reset_state(BT_AWS_MCE_AGENT_STATE_NONE);
                // RHO case: Parnter -> Agent.
                } else if (previous == BT_AWS_MCE_AGENT_STATE_NONE && state_change->state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                    bt_sink_srv_aws_mce_reset_state(BT_AWS_MCE_AGENT_STATE_INACTIVE);
                    context->state = state_change->state;
                // Normal case: Only Agent, no need notify to uplayer.
                } else if (state_change->state == BT_AWS_MCE_AGENT_STATE_CONNECTABLE) { 
                // Normal case: Only Agent, Parnter attached or dettached.
                    context->state = state_change->state;
                } else {
                    context->state = state_change->state;
                    bt_sink_srv_aws_mce_notify_all(msg, status, buffer);
                    bt_sink_srv_aws_mce_notify_state_change(addr, state_change->state);
                }
            } else {
                bt_sink_srv_report_id("[SINK][AWS_MCE] Can't fine the context, handle:0x%x.", 1, state_change->handle);
            }
        }
        break;

        case BT_AWS_MCE_INFOMATION_PACKET_IND: {
            bt_aws_mce_information_ind_t* if_packet = (bt_aws_mce_information_ind_t*)buffer;
            if (if_packet->packet.type == BT_AWS_MCE_INFORMATION_A2DP) {
                aws_mce_cb[BT_SINK_SRV_AWS_MCE_MODULE_MUSIC].aws_callback(msg, status,buffer);
            } else if (if_packet->packet.type == BT_AWS_MCE_INFORMATION_SCO) {
                aws_mce_cb[BT_SINK_SRV_AWS_MCE_MODULE_CALL].aws_callback(msg, status,buffer);
            } else {
                bt_sink_srv_report_id("[SINK][AWS_MCE] Not support the type:0x%x now!", 1, msg, status);
            }
        }
        break; 
        
        /*Other event to notify uplayer directly.*/
        case BT_AVM_DECODE_NOTIFICATION_IND:
        case BT_AVM_MEDIA_DATA_RECEIVED_IND:
        case BT_AWS_MCE_CALL_AUDIO_CONNECTED:
        case BT_AWS_MCE_CALL_AUDIO_DISCONNECTED:
        case BT_AWS_MCE_MODE_CHANGED:{
            bt_sink_srv_aws_mce_notify_all(msg, status, buffer);
        }
        break;
   
        default: {
           bt_sink_srv_report_id("[SINK][AWS_MCE] Common callback, exception event: 0x%x", 1, msg);
        }
        break;
           
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_sink_srv_aws_mce_action_handler(bt_sink_srv_action_t action, void *param)
{
    bt_status_t result = BT_STATUS_SUCCESS;
    bt_sink_srv_aws_mce_context_t *bt_sink_srv_aws_context_p = NULL;

    bt_sink_srv_report_id("[SINK][AWS_MCE]Action:%x", 1, action);
    switch(action) {
        case BT_SINK_SRV_ACTION_PROFILE_INIT: {
            bt_sink_srv_aws_mce_init();
        }
        break;
        
        case BT_SINK_SRV_ACTION_PROFILE_DEINIT: {
            bt_sink_srv_aws_mce_deinit();
        }
        break;
        
        case BT_SINK_SRV_ACTION_PROFILE_CONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)param;
            if (action_param->profile_connection_mask & BT_SINK_SRV_PROFILE_AWS) {
                //bt_sink_srv_aws_context_p = bt_sink_srv_cm_get_profile_info(&action_param->address, BT_SINK_SRV_PROFILE_AWS);
                bt_sink_srv_aws_context_p = bt_sink_srv_aws_mce_get_free_context();
                if (NULL != bt_sink_srv_aws_context_p) {
                    bt_status_t status = bt_aws_mce_connect(&bt_sink_srv_aws_context_p->aws_handle, (const bt_bd_addr_t *)&action_param->address);
                    if (status == BT_STATUS_SUCCESS){
                    #ifndef MTK_BT_CM_SUPPORT
                        bt_sink_srv_cm_profile_status_notify(&action_param->address, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING, status);
                    #endif
                    } else {
                        bt_sink_srv_report_id("[SINK][AWS_MCE]Connect aws connection failed:0x%x", 1, status);
                    }
                }
            }
        }
        break;
        
        case BT_SINK_SRV_ACTION_PROFILE_DISCONNECT: {
            bt_sink_srv_profile_connection_action_t *action_param = (bt_sink_srv_profile_connection_action_t *)param;
            if (action_param->profile_connection_mask & BT_SINK_SRV_PROFILE_AWS) {
                //bt_sink_srv_aws_context_p = bt_sink_srv_cm_get_profile_info(&action_param->address, BT_SINK_SRV_PROFILE_AWS);
                bt_sink_srv_aws_context_p = bt_sink_srv_aws_mce_get_context_by_address((const bt_bd_addr_t *)&action_param->address);
                if (NULL != bt_sink_srv_aws_context_p && bt_sink_srv_aws_context_p->aws_handle != BT_AWS_MCE_INVALID_HANDLE) {
                    bt_status_t status = bt_aws_mce_disconnect( bt_sink_srv_aws_context_p->aws_handle);
                    if (status == BT_STATUS_SUCCESS){
                    #ifndef MTK_BT_CM_SUPPORT
                        bt_sink_srv_cm_profile_status_notify(&action_param->address, BT_SINK_SRV_PROFILE_AWS, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTING, status);
                    #endif
                    } else {
                        bt_sink_srv_report_id("[SINK][AWS_MCE]Disconnect aws connection failed:0x%x", 1, status);
                    }
                }
            }
        }
        break;
    }
    return result;
}

#ifdef BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT
static void bt_sink_srv_aws_mce_reset_normal_link_context(void)
{
    bt_bd_addr_t *local_address = bt_device_manager_aws_local_info_get_local_address();

    for (uint32_t i = 0; i < BT_SINK_SRV_AWS_MCE_CONNECTION_NUM; i++) {
        if (bt_sink_srv_aws_mce_cntx[i].aws_handle != BT_AWS_MCE_INVALID_HANDLE) {
            bt_bd_addr_t *address = (bt_bd_addr_t *)bt_aws_mce_get_bd_addr_by_handle(
                bt_sink_srv_aws_mce_cntx[i].aws_handle);

            if ((address != NULL) && (BT_SINK_SRV_AWS_MCE_IS_SPECIAL_LINK(local_address, address))) {
                bt_sink_srv_report_id("[SINK][AWS_MCE]context[%d] is special link", 1, i);
                continue;
            }

            /* Reset normal link context. */
            bt_sink_srv_aws_mce_reset_context_by_handle(bt_sink_srv_aws_mce_cntx[i].aws_handle);
        }
    }
}

static void bt_sink_srv_aws_mce_update_agent(void)
{
    bt_bd_addr_t *new_agent_address = bt_device_manager_aws_local_info_get_peer_address();

    /* 1. Destory all normal link context. */
    bt_sink_srv_aws_mce_reset_normal_link_context();

    /* 2. Create normal link context to Agent. */
    bt_sink_srv_aws_mce_context_t *context = bt_sink_srv_aws_mce_get_free_context();
    bt_sink_srv_assert((context != NULL) && "Can not alloc a Sink AWS MCE context!");

    context->aws_handle = bt_aws_mce_query_handle_by_address(
        BT_MODULE_AWS_MCE,
        (const bt_bd_addr_t *)new_agent_address);
    context->conn_state = BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED;
    context->state = BT_AWS_MCE_AGENT_STATE_NONE;
}

static void bt_sink_srv_aws_mce_update_partner(const bt_bd_addr_t *active_address)
{
    bt_bd_addr_t connected_address[BT_SINK_SRV_AWS_MCE_CONNECTION_NUM] = {{0}};

    /* 1. Destory all normal link context. */
    bt_sink_srv_aws_mce_reset_normal_link_context();

    /* 2. Create normal link context to SP. */
    uint32_t connected_number = bt_cm_get_connected_devices(
        ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS),
        connected_address,
        BT_SINK_SRV_AWS_MCE_CONNECTION_NUM);

    for (uint32_t i = 0; i < connected_number; i++) {
        bt_sink_srv_aws_mce_context_t *context = bt_sink_srv_aws_mce_get_free_context();
        bt_sink_srv_assert((context != NULL) && "Can not alloc a Sink AWS MCE context!");

        context->aws_handle = bt_aws_mce_query_handle_by_address(
            BT_MODULE_AWS_MCE,
            (const bt_bd_addr_t *)&connected_address[i]);
        context->conn_state = BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED;

        if (bt_sink_srv_memcmp(&connected_address[i], active_address, sizeof(bt_bd_addr_t)) == 0) {
            context->state = BT_AWS_MCE_AGENT_STATE_ATTACHED;
        } else {
            context->state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
        }
    }
}

static bt_status_t bt_sink_srv_aws_mce_status_callback(const bt_bd_addr_t *address, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    bt_sink_srv_report_id("[SINK][AWS_MCE]status callback, role:0x%x status:0x%x", 2, role, status);

    switch (event) {
        case BT_ROLE_HANDOVER_START_IND: {
            break;
        }

        case BT_ROLE_HANDOVER_PREPARE_REQ_IND: {
            break;
        }

        case BT_ROLE_HANDOVER_COMPLETE_IND: {
            if (status == BT_STATUS_SUCCESS) {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    bt_sink_srv_aws_mce_update_agent();
                } else {
                    bt_sink_srv_aws_mce_update_partner(address);
                }
            }
            break;
        }

        default: {
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}
#endif /* BT_SINK_SRV_AWS_MCE_IN_MULTIPOINT */

