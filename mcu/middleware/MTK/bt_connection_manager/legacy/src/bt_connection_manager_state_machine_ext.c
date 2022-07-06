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

#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
#include <stdio.h>
#include <stdlib.h>
#include "bt_sink_srv.h"
#include "bt_aws_mce.h"
#include "syslog.h"
#include "bt_callback_manager.h"
#include "bt_timer_external.h"
//#include "bt_sink_srv_resource.h"
//#include "bt_sink_srv_utils.h"
#include "bt_device_manager.h"
#include "bt_device_manager_internal.h"
#include "bt_connection_manager_utils.h"
#include "bt_connection_manager_state_machine_ext.h"
#include "bt_connection_manager_internal.h"
#include "hal_trng.h"
#include "hal_gpt.h"
#include "bt_system.h"

#define KEEP_ROLE_RECOVER  0x01


bt_connection_manager_state_machine_context_t bt_connection_manager_state_machine_cntx;
//bt_connection_manager_state_machine_context_t *bt_connection_manager_state_machine_cntx_ptr = &bt_connection_manager_state_machine_cntx;
static bt_connection_manager_state_bt_reinit_type_t bt_connection_manager_state_bt_reinit = BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE;
static uint8_t bt_connection_manager_state_machine_ext_need_start_agent_switch_timer = 0;
static uint32_t bt_connection_manager_state_machine_ext_gpt_start = 0;
static uint32_t bt_connection_manager_state_machine_ext_gpt_end = 0;
static bt_connection_manager_state_ext_recovery_lock_state_type_t bt_connection_manager_state_ext_role_recovery_lock_state = 0;

static void bt_connection_manager_state_machine_ext_set_reinit_flag(bt_connection_manager_state_bt_reinit_type_t type);
static bt_connection_manager_state_bt_reinit_type_t bt_connection_manager_state_machine_ext_get_reinit_flag(void);
static const bt_connection_manager_state_ext_item_t* bt_connection_manager_state_machine_ext_get_active_state(void);

static bt_status_t bt_connection_manager_state_machine_ext_bt_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
void bt_connection_manager_state_machine_ext_start_scan_timer();
void bt_connection_manager_state_machine_ext_stop_scan_timer();
void  bt_connection_manager_state_machine_ext_handle_state_switching(void);
void bt_connection_manager_state_machine_ext_start_agent_change_timer();
void bt_connection_manager_state_machine_ext_stop_agent_change_timer();
void bt_connection_manager_state_machine_ext_start_standby_timer();
void bt_connection_manager_state_machine_ext_random_seed_init();
uint8_t bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty();

extern void bt_connection_manager_connect_ext();

static const bt_connection_manager_state_ext_item_t bt_connection_manager_state_machine_table[] = {
    {/*BT_CONNECTION_MANAGER_STATE_EXT_STANDBY*/
        .allow_list =  BT_CONNECTION_MANAGER_STATE_EXT_SCAN_BIT | BT_CONNECTION_MANAGER_STATE_EXT_PARTNER_BIT | BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT_BIT | BT_CONNECTION_MANAGER_STATE_EXT_CON_LS_BIT | BT_CONNECTION_MANAGER_STATE_EXT_REC_LS_BIT,
        .on_enter = bt_connection_manager_state_ext_defualt_on_enter,
        .on_exit = bt_connection_manager_state_ext_standby_on_exit,
        .bt_fun = bt_connection_manager_state_ext_standby_bt_fun,
        .sink_fun = bt_connection_manager_state_ext_standby_sink_fun,
        .misc_fun = bt_connection_manager_state_ext_standby_misc_fun,
    },
    {/* BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT */
        .allow_list = BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE_BIT | BT_CONNECTION_MANAGER_STATE_EXT_CONNECT_BIT | BT_CONNECTION_MANAGER_STATE_EXT_REC_LS_BIT,
        .on_enter = bt_connection_manager_state_ext_defualt_on_enter,
        .on_exit = bt_connection_manager_state_ext_defualt_on_exit,
        .bt_fun = bt_connection_manager_state_ext_reconnect_bt_fun,
        .sink_fun = bt_connection_manager_state_ext_reconnect_sink_fun,
        .misc_fun = bt_connection_manager_state_ext_default_misc_fun,
    },
    {/* BT_CONNECTION_MANAGER_STATE_EXT_CONNECT */
        .allow_list = BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE_BIT | BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT_BIT | BT_CONNECTION_MANAGER_STATE_EXT_CON_LS_BIT | BT_CONNECTION_MANAGER_STATE_EXT_PARTNER_BIT | BT_CONNECTION_MANAGER_STATE_EXT_REC_LS_BIT,
        .on_enter = bt_connection_manager_state_ext_defualt_on_enter,
        .on_exit = bt_connection_manager_state_ext_defualt_on_exit,
        .bt_fun = bt_connection_manager_state_ext_connect_bt_fun,
        .sink_fun = bt_connection_manager_state_ext_connect_sink_fun,
        .misc_fun = bt_connection_manager_state_ext_connect_misc_fun,
    },
    {/* BT_CONNECTION_MANAGER_STATE_EXT_PARTNER */
        .allow_list = BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE_BIT | BT_CONNECTION_MANAGER_STATE_EXT_SCAN_BIT | BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT_BIT | BT_CONNECTION_MANAGER_STATE_EXT_CONNECT_BIT,
        .on_enter = bt_connection_manager_state_ext_defualt_on_enter,
        .on_exit = bt_connection_manager_state_ext_defualt_on_exit,
        .bt_fun = bt_connection_manager_state_ext_defualt_bt_fun,
        .sink_fun = bt_connection_manager_state_ext_partner_sink_fun,
        .misc_fun = bt_connection_manager_state_ext_partner_misc_fun,
    },
    {/* BT_CONNECTION_MANAGER_STATE_EXT_REC_LS */
        .allow_list = BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE_BIT | BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT_BIT | BT_CONNECTION_MANAGER_STATE_EXT_SCAN_BIT | BT_CONNECTION_MANAGER_STATE_EXT_CON_LS_BIT,
        .on_enter = bt_connection_manager_state_ext_rec_ls_on_enter,
        .on_exit = bt_connection_manager_state_ext_defualt_on_exit,
        .bt_fun = bt_connection_manager_state_ext_rec_ls_bt_fun,
        .sink_fun = bt_connection_manager_state_ext_rec_ls_sink_fun,
        .misc_fun = bt_connection_manager_state_ext_rec_ls_misc_fun,
    },
    {/* BT_CONNECTION_MANAGER_STATE_EXT_CON_LS */
        .allow_list = BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE_BIT | BT_CONNECTION_MANAGER_STATE_EXT_SCAN_BIT | BT_CONNECTION_MANAGER_STATE_EXT_CONNECT_BIT | BT_CONNECTION_MANAGER_STATE_EXT_REC_LS_BIT,
        .on_enter = bt_connection_manager_state_ext_defualt_on_enter,
        .on_exit = bt_connection_manager_state_ext_defualt_on_exit,
        .bt_fun = bt_connection_manager_state_ext_conn_ls_bt_fun,
        .sink_fun = bt_connection_manager_state_ext_conn_ls_sink_fun,
        .misc_fun = bt_connection_manager_state_ext_conn_ls_misc_fun,
    },
    {/* BT_CONNECTION_MANAGER_STATE_EXT_SCAN */
        .allow_list = BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE_BIT | BT_CONNECTION_MANAGER_STATE_EXT_PARTNER_BIT | BT_CONNECTION_MANAGER_STATE_EXT_REC_LS_BIT,
        .on_enter = bt_connection_manager_state_ext_defualt_on_enter,
        .on_exit = bt_connection_manager_state_ext_defualt_on_exit,
        .bt_fun = bt_connection_manager_state_ext_scan_bt_fun,
        .sink_fun = bt_connection_manager_state_ext_scan_sink_fun,
        .misc_fun = bt_connection_manager_state_ext_scan_misc_fun,
    },
    {/* BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE */
        .allow_list = BT_CONNECTION_MANAGER_STATE_EXT_ALL_BIT,
        .on_enter = bt_connection_manager_state_ext_defualt_on_enter,
        .on_exit = bt_connection_manager_state_ext_defualt_on_exit,
        .bt_fun = bt_connection_manager_state_ext_defualt_bt_fun,
        .sink_fun = bt_connection_manager_state_ext_defualt_sink_fun,
        .misc_fun = bt_connection_manager_state_ext_in_case_misc_fun,
    },
};

void bt_connection_manager_state_ext_init(void)
{
    if(bt_connection_manager_state_machine_ext_get_reinit_flag() == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE){
        if(bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()) {
            bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ALL);
        } else {
            bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_NONE);
        }
        bt_connection_manager_state_machine_ext_random_seed_init();
        bt_connection_manager_state_machine_cntx.goto_state = BT_CONNECTION_MANAGER_STATE_EXT_STANDBY;
        bt_connection_manager_state_machine_cntx.active_state = bt_connection_manager_state_machine_table + BT_CONNECTION_MANAGER_STATE_EXT_STANDBY;
        bt_connection_manager_state_machine_ext_start_standby_timer();
        bt_callback_manager_register_callback(bt_callback_type_app_event,
                                              (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_SYSTEM),
                                              (void *)bt_connection_manager_state_machine_ext_bt_event_callback);
        bt_cmgr_report_id("[BT_CM_SM][I] init state:%d.", 1, bt_connection_manager_state_machine_cntx.goto_state);
    }
}

void bt_connection_manager_state_ext_deinit(void)
{
    if(bt_connection_manager_state_machine_ext_get_reinit_flag() == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE) {
        bt_connection_manager_state_machine_ext_stop_scan_timer();
        bt_connection_manager_state_machine_ext_stop_agent_change_timer();
        //if(bt_connection_manager_device_local_info_get_aws_role() & BT_AWS_MCE_ROLE_AGENT) {
            bt_connection_manager_state_machine_cntx.goto_state = BT_CONNECTION_MANAGER_STATE_EXT_NONE;
            bt_connection_manager_state_machine_cntx.active_state = NULL;
        //}
        bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_connection_manager_state_machine_ext_bt_event_callback);
        bt_cmgr_report_id("[BT_CM_SM][I] deinit state:%d.", 1, bt_connection_manager_state_machine_cntx.goto_state);
    }

}
void bt_connection_manager_state_ext_force_deinit(void)
{
    bt_cmgr_report_id("[BT_CM_SM][I] user requet force deinit role recovery", 0);
    bt_connection_manager_state_machine_ext_stop_scan_timer();
    bt_connection_manager_state_machine_cntx.goto_state = BT_CONNECTION_MANAGER_STATE_EXT_NONE;
    bt_connection_manager_state_machine_cntx.active_state = NULL;
    bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_connection_manager_state_machine_ext_bt_event_callback);
    bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE);
}

void bt_connection_manager_state_machine_ext_role_recovery_lock(void)
{
    bt_connection_manager_state_ext_role_recovery_lock_state = BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_LOCK;
    bt_cmgr_report_id("[BT_CM_SM][I] lock role recovery", 0);
}

void bt_connection_manager_state_machine_ext_role_recovery_unlock(void)
{
    bt_connection_manager_state_ext_role_recovery_lock_state = BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_UNLOCK;
    bt_cmgr_report_id("[BT_CM_SM][I] unlock role recovery", 0);
}

bt_connection_manager_state_ext_recovery_lock_state_type_t bt_connection_manager_state_machine_ext_get_lock_state(void)
{
    bt_cmgr_report_id("[BT_CM_SM][I] role recovery lock state = 0x%x", 1, bt_connection_manager_state_ext_role_recovery_lock_state);
    return bt_connection_manager_state_ext_role_recovery_lock_state;
}


static void bt_connection_manager_state_machine_ext_set_reinit_flag(bt_connection_manager_state_bt_reinit_type_t type)
{
    bt_cmgr_report_id("[BT_CM_SM][I] old reinit = (0x%x), new reinit = (0x%x)", 2, bt_connection_manager_state_bt_reinit, type);
    bt_connection_manager_state_bt_reinit = type;
}

static bt_connection_manager_state_bt_reinit_type_t bt_connection_manager_state_machine_ext_get_reinit_flag(void)
{
    bt_cmgr_report_id("[BT_CM_SM][I] current reinit = (0x%x)", 1, bt_connection_manager_state_bt_reinit);
    return bt_connection_manager_state_bt_reinit;
}


static const bt_connection_manager_state_ext_item_t* bt_connection_manager_state_machine_ext_get_active_state(void)
{
    if (bt_connection_manager_state_machine_cntx.active_state) {
        bt_cmgr_report_id("[BT_CM_SM][I] current active state = (0x%x)", 1, (bt_connection_manager_state_machine_cntx.active_state - bt_connection_manager_state_machine_table));
    } else {
        bt_cmgr_report_id("[BT_CM_SM][I] current active state is null", 0);
    }
    return bt_connection_manager_state_machine_cntx.active_state;
}



bt_connection_manager_state_ext_t bt_connection_manager_state_machine_get_state(void)
{
    if (bt_connection_manager_state_machine_cntx.active_state == NULL) {
        return BT_CONNECTION_MANAGER_STATE_EXT_NONE;
    } else {
        return (bt_connection_manager_state_ext_t)(bt_connection_manager_state_machine_cntx.active_state - bt_connection_manager_state_machine_table);
    }
}

int32_t bt_connection_manager_state_ext_switch (bt_connection_manager_state_ext_t to, uint32_t reason, void *user_data)
{
    //check allow list.
    if ((bt_connection_manager_state_machine_cntx.active_state) && ((bt_connection_manager_state_machine_cntx.active_state->allow_list & (1 << to)) == 0)) {
        bt_cmgr_report_id("[BT_CM_SM][E]bt_connection_manager_state_ext_switch (0x%02x -> 0x%02x) return error", 2, bt_connection_manager_state_machine_get_state(), to);
        bt_connection_manager_assert(0);
        return -1;
    }
    if (bt_connection_manager_state_machine_cntx.goto_state != BT_CONNECTION_MANAGER_STATE_EXT_NONE) {
        bt_cmgr_report_id("[BT_CM_SM][W]bt_connection_manager_state_ext_switch (%d) skip %d", 2, to, bt_connection_manager_state_machine_cntx.goto_state);
    }
    bt_connection_manager_state_machine_cntx.goto_state = to;
    bt_connection_manager_state_machine_cntx.switch_reason = reason;
    bt_connection_manager_state_machine_cntx.switch_data = user_data;

    bt_connection_manager_state_machine_ext_handle_state_switching();
    return 0;
}

void bt_connection_manager_state_machine_ext_sink_event_callback(bt_sink_srv_event_t event_id, void *param, uint32_t param_len)
{
    bt_cmgr_report_id("[BT_CM_SM][I] state ext sink callback: event_id(0x%08x), param(0x%08x), len(%d)", 3, event_id, param, param_len);
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)param;

    switch (event_id) {
        case BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE:
        case BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE:
        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE: {
            const bt_connection_manager_state_ext_item_t *active_state = bt_connection_manager_state_machine_ext_get_active_state();
            if (active_state && active_state->sink_fun) {
                active_state->sink_fun(event_id, event);
            }
            break;
        }
        default:
            break;
    }
}

static bt_status_t bt_connection_manager_state_machine_ext_bt_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    switch (msg) {
        case BT_GAP_LINK_STATUS_UPDATED_IND: { //todo , reconnect fail reson.
            //bt_cmgr_report_id("[BT_CM_SM][I] link status changed.", 0);
            const bt_connection_manager_state_ext_item_t *active_state = bt_connection_manager_state_machine_ext_get_active_state();
            if (active_state && active_state->bt_fun) {
                active_state->bt_fun(msg, status, buffer);
            }
            break;
        }
        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

uint8_t bt_connection_manager_state_machine_ext_is_in_role_resetup()
{
    if(bt_connection_manager_state_machine_ext_get_reinit_flag() == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE) {
        return 0;
    } else {
        return 1;
    }
}

void bt_connection_manager_state_machine_ext_cancel_role_resetup()
{
    bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE);
}

void bt_connection_manager_state_machine_ext_power_onoff_notify(uint8_t is_on)
{
    const bt_connection_manager_state_ext_item_t *active_state = bt_connection_manager_state_machine_ext_get_active_state();
    if (active_state && active_state->misc_fun) {
        bt_connection_manager_misc_func_type_t type = is_on ? BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_ON : BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_OFF;
        active_state->misc_fun(type, NULL);
    }

    if (!bt_connection_manager_get_dut_mode()) {
        if(is_on == 0 && bt_connection_manager_state_machine_ext_get_reinit_flag() == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE && !bt_connection_manager_aws_pair_pairing_is_ongoing()){
            bt_device_manager_aws_local_info_init();
            bt_set_local_public_address((bt_bd_addr_ptr_t)bt_connection_manager_device_local_info_get_local_address());
        }
    }
}

void bt_connection_manager_state_machine_ext_stop_role_change()
{
    if (bt_connection_manager_state_machine_ext_get_reinit_flag() == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE){
        bt_connection_manager_state_machine_ext_stop_scan_timer();
        bt_connection_manager_state_machine_ext_stop_agent_change_timer();
    } else {
        bt_cmgr_report_id("[BT_CM_SM][E] alreay in role change flow, reason(%d)", 1, bt_connection_manager_state_bt_reinit);
    }

}

void bt_connection_manager_state_machine_ext_trigger_role_change(bt_aws_mce_role_t role)
{
    if(bt_connection_manager_state_machine_ext_get_reinit_flag() == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE) {
        if(role != bt_connection_manager_device_local_info_get_aws_role()) {
            const bt_connection_manager_state_ext_item_t *active_state = bt_connection_manager_state_machine_ext_get_active_state();
            if (active_state && active_state->misc_fun) {
                active_state->misc_fun(BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_ROLE_CHANGE_MANUALLY, NULL);
            }
        } else {
            bt_cmgr_report_id("[BT_CM_SM][E] alreay in role (%d)", 1, role);
        }
    } else {
        bt_cmgr_report_id("[BT_CM_SM][E] alreay in role change flow, reason(%d)", 1, bt_connection_manager_state_bt_reinit);
    }
}

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
void bt_connection_manager_state_machine_ext_rho_event_notify(bt_role_handover_event_t event, bt_status_t status)
{
    bt_cmgr_report_id("[BT_CM_SM][I] rho event notify, type (%d), status(0x%08x)", 2, event, status);
    const bt_connection_manager_state_ext_item_t *active_state = bt_connection_manager_state_machine_ext_get_active_state();
    if (active_state && active_state->misc_fun) {
        bt_connection_manager_misc_func_params_rho_event_t rho_event;
        rho_event.event = event;
        rho_event.status = status;
        active_state->misc_fun(BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_RHO, &rho_event);
    }
}
#endif

void bt_connection_manager_state_machine_ext_agent_sync_notify()
{
    bt_cmgr_report_id("[BT_CM_SM][I] partner receive agent sync packet after attached.", 0);
    const bt_connection_manager_state_ext_item_t *active_state = bt_connection_manager_state_machine_ext_get_active_state();
    if (active_state && active_state->misc_fun) {
        active_state->misc_fun(BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC, NULL);
    }
}


#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
void bt_connection_manager_state_machine_ext_timeout_callback(uint32_t timer_id, uint32_t data)
{
    bt_connection_manager_state_ext_timer_type_t timer = (bt_connection_manager_state_ext_timer_type_t)data;
    bt_connection_manager_misc_func_type_t type = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_connection_manager_state_machine_ext_gpt_end);
    bt_cmgr_report_id("[BT_CM_SM][I] gpt time diff (%d)", 1, (bt_connection_manager_state_machine_ext_gpt_end - bt_connection_manager_state_machine_ext_gpt_start) * 1000/32768);

    switch (timer) {
        case BT_CONNECTION_MANAGER_STATE_EXT_TIMER_PARTNER_TO_AGENT: {
            type = BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_SCAN_TIME_OUT;
            break;
        }
        case BT_CONNECTION_MANAGER_STATE_EXT_TIMER_AGENT_TO_PARTNER: {
            type = BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_REC_LS_TIME_OUT;
            break;
        }
        case BT_CONNECTION_MANAGER_STATE_EXT_TIMER_STANDBY_WITH_OLD_ROLE: {
            type = BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_ROLE_STANDBY_TIMEOUT;
            break;
        }
        case BT_CONNECTION_MANAGER_STATE_EXT_TIMER_PARTNER_WAIT_FOR_AGENT_SYNC: {
            type = BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC_TIMEOUT;
            break;
        }
        case BT_CONNECTION_MANAGER_STATE_EXT_TIMER_CHECK_LINK_STATE: {
            type = BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_CHECK_LINK_TIMEOUT;
            break;
        }
        default:
            break;
    }
    const bt_connection_manager_state_ext_item_t *active_state = bt_connection_manager_state_machine_ext_get_active_state();
    if (active_state && active_state->misc_fun) {
        active_state->misc_fun(type, NULL);
    }
}
#endif

void bt_connection_manager_state_machine_ext_start_standby_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    uint32_t time_out = 0;

    if(bt_device_manager_aws_local_info_get_real_role() & BT_AWS_MCE_ROLE_AGENT) {
        time_out = BT_CONNECTION_MANAGER_STATE_EXT_OLD_ROLE_STANDBY_TIMER + 650;
        bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_CONNECTION_MANAGER_STATE_EXT_TIMER_STANDBY_WITH_OLD_ROLE,
                        time_out, bt_connection_manager_state_machine_ext_timeout_callback);
    } else {
        time_out = 10;
        bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_CONNECTION_MANAGER_STATE_EXT_TIMER_STANDBY_WITH_OLD_ROLE,
                        time_out, bt_connection_manager_state_machine_ext_timeout_callback);
    }
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_connection_manager_state_machine_ext_gpt_start);
    bt_cmgr_report_id("[BT_CM_SM][I] start standby timer : %d", 1, time_out);

#endif

}

void bt_connection_manager_state_machine_ext_stop_standby_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop standby timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif

}

void bt_connection_manager_state_machine_ext_start_check_link_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    uint32_t timeout = 3000;

    bt_cmgr_report_id("[BT_CM_SM][I] start_check_link_timer: %d ms", 1, timeout);

    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_CONNECTION_MANAGER_STATE_EXT_TIMER_CHECK_LINK_STATE,
                    timeout, bt_connection_manager_state_machine_ext_timeout_callback);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_connection_manager_state_machine_ext_gpt_start);
#endif
}

void bt_connection_manager_state_machine_ext_stop_check_link_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop check link timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif
}

bool bt_connection_manager_state_machine_ext_is_check_link_timer_active()
{
    bool ret = false;
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    if (bt_timer_ext_find(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID)) {
        ret = true;
    }
#endif
    bt_cmgr_report_id("[BT_CM_SM][I] is_check_link_timer_active : %d", 1, ret);
    return ret;
}

void bt_connection_manager_state_machine_ext_random_seed_init()
{
    hal_trng_status_t ret = HAL_TRNG_STATUS_OK;
    uint32_t random_seed;

    ret = hal_trng_init();
    if (HAL_TRNG_STATUS_OK != ret) {
        bt_cmgr_report_id("[BT_CM_SM][E] init random seed--error 1", 0);
    }

    ret = hal_trng_get_generated_random_number(&random_seed);
    if (HAL_TRNG_STATUS_OK != ret) {
        bt_cmgr_report_id("[BT_CM_SM][E] init random seed--error 2", 0);
    }
    hal_trng_deinit();
    srand(random_seed);
}

uint32_t bt_connection_manager_state_machine_ext_generate_random(uint32_t min, uint32_t max, uint32_t resolution)
{
    uint32_t random_step = (max - min)/resolution + 1;

    return (min + (rand() % random_step) * resolution);
}

void bt_connection_manager_state_machine_ext_start_scan_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    uint32_t timeout = 0;

    if(bt_device_manager_aws_local_info_get_real_role() & BT_AWS_MCE_ROLE_AGENT){
        timeout = bt_connection_manager_state_machine_ext_generate_random(BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_DUR_MIN,
                    BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_DUR_MAX, BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_RESOLUTION);
    } else {
        timeout = bt_connection_manager_state_machine_ext_generate_random(BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_DUR_MIN,
                    BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_DUR_MAX, BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_RESOLUTION);
    }
    bt_cmgr_report_id("[BT_CM_SM][I] start scan timer: %d ms", 1, timeout);

    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_CONNECTION_MANAGER_STATE_EXT_TIMER_PARTNER_TO_AGENT,
                    timeout, bt_connection_manager_state_machine_ext_timeout_callback);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_connection_manager_state_machine_ext_gpt_start);

#endif

}

void bt_connection_manager_state_machine_ext_stop_scan_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop scan timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif

}

void bt_connection_manager_state_machine_ext_start_agent_change_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
/*
    uint32_t timeout = 0;
    if(bt_device_manager_aws_local_info_get_real_role() & BT_AWS_MCE_ROLE_AGENT){
        timeout = bt_connection_manager_state_machine_ext_generate_random(BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_DUR_MIN,
                    BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_DUR_MAX, BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_RESOLUTION);
    } else {
        timeout = bt_connection_manager_state_machine_ext_generate_random(BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_DUR_MIN,
                    BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_DUR_MAX, BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_RESOLUTION);
    }
    bt_cmgr_report_id("[BT_CM_SM][I] start agent change timer: %d ms", 1, timeout);
*/
    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_CONNECTION_MANAGER_STATE_EXT_TIMER_AGENT_TO_PARTNER,
                5120, bt_connection_manager_state_machine_ext_timeout_callback);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_connection_manager_state_machine_ext_gpt_start);

#endif

}

void bt_connection_manager_state_machine_ext_stop_agent_change_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop agent change timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif

}


/*can not call bt_connection_manager_power_reset() in bt_app_event_callback(), maybe cause double free issue*/
void bt_connection_manager_state_machine_ext_async_timeout_callback(uint32_t timer_id, uint32_t data)
{
    bt_connection_manager_power_reset(false);
}

void bt_connection_manager_state_machine_ext_start_async_timer()
{
    bt_timer_ext_start(BT_SINK_SRV_CM_STATE_EXT_ASYNC_TIMER_ID, 0,
                    0, bt_connection_manager_state_machine_ext_async_timeout_callback);

}
void bt_connection_manager_state_machine_ext_start_partner_sync_agent_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] start partner wait for agent sync timer: %d ms", 1, BT_CONNECTION_MANAGER_STATE_EXT_PARTNER_WAIT_FOR_AGENT_SYNC_TIMER);

    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_CONNECTION_MANAGER_STATE_EXT_TIMER_PARTNER_WAIT_FOR_AGENT_SYNC,
                BT_CONNECTION_MANAGER_STATE_EXT_PARTNER_WAIT_FOR_AGENT_SYNC_TIMER, bt_connection_manager_state_machine_ext_timeout_callback);
#endif

}

void bt_connection_manager_state_machine_ext_stop_partner_sync_agent_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop partner wait for agent sync timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif

}

uint8_t bt_connection_manager_state_machine_ext_partner_connected_with_agent()
{
    if(bt_sink_srv_cm_get_connected_profiles(bt_connection_manager_device_local_info_get_peer_aws_address()) & BT_SINK_SRV_PROFILE_AWS){
        return 1;
    } else {
        return 0;
    }
}

uint8_t bt_connection_manager_state_machine_ext_agent_connected_with_partner()
{
    bt_aws_mce_agent_state_type_t agent_state = bt_sink_srv_cm_get_aws_link_state();
    if(agent_state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t bt_connection_manager_state_machine_ext_agent_connected_with_sp()
{
    bt_bd_addr_t addr_list = {0};
    uint32_t connect_devie_num = bt_sink_srv_cm_get_connected_device_list(BT_SINK_SRV_PROFILE_NONE, &addr_list, 1);

    return connect_devie_num;
}

void bt_connection_manager_state_machine_ext_action_handler(bt_sink_srv_action_t action, void *param)
{
    const bt_connection_manager_state_ext_item_t *active_state = bt_connection_manager_state_machine_ext_get_active_state();
    if (active_state && active_state->misc_fun) {
        switch(action){
            case BT_SINK_SRV_ACTION_NOTIFY_OUT_OF_CASE: {
                active_state->misc_fun(BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_TAKEOUT_FROM_CASE, NULL);
                break;
            }
            case BT_SINK_SRV_ACTION_NOTIFY_IN_CASE: {
                //bt_connection_manager_state_machine_cntx_ptr->active_state->misc_fun(BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_PUTIN_TO_CASE, NULL);
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE, 0, NULL);
                break;
            }
            case BT_SINK_SRV_ACTION_CONNECT: {
                /*start to connect, need  to start role change timer*/
                if(bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT){
                    //bt_cmgr_report_id("[BT_CM_SM][I] agent connect sp", 0);
                    active_state->misc_fun(BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_AGENT_CONN_SP, NULL);
                } else {
                    //bt_cmgr_report_id("[BT_CM_SM][I] partner connect agent", 0);
                    active_state->misc_fun(BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_PARTNER_CONN_AGENT, NULL);
                }
                break;
            }
            case BT_SINK_SRV_ACTION_DISCOVERABLE: {
                if(bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT){
                    bt_cmgr_report_id("[BT_CM_SM][I] enable discoverable", 0);
                    bt_connection_manager_state_machine_ext_stop_agent_change_timer();
                }
                break;
            }
            default:
                break;
        }
    }
}


/*******************************************************************************
*******************************************************************************
**************************          State Machine              ****************************
*******************************************************************************
*******************************************************************************
********************************************************************************/
uint8_t bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()
{
    bt_bd_addr_t empty_addr = {0};
    bt_bd_addr_t *remote_addr = bt_connection_manager_device_local_info_get_peer_aws_address();

    if (!memcmp(remote_addr, empty_addr, sizeof(bt_bd_addr_t))) {
        bt_cmgr_report_id("[BT_CM_SM][E] Error! >>>>>>>>>>>>[Attention] AWS peer addr is empty, please config it first for earbuds working well!!!!!!<<<<<<<<<<<<<<<", 0);
        return 1;
    }

    return 0;
}

void bt_connection_manager_state_machine_ext_exchange_bt_address()
{
    /*exchange local address and remote address*/
    bt_cmgr_report_id("[BT_CM_SM][I] exchange bt address", 0);

    bt_bd_addr_t local_addr = {0};
    bt_bd_addr_t empty_addr = {0};
    bt_bd_addr_t *remote_addr = bt_connection_manager_device_local_info_get_peer_aws_address();

    bt_connection_manager_memcpy((void *)&local_addr, (void *)bt_connection_manager_device_local_info_get_local_address(), sizeof(bt_bd_addr_t));
    if (memcmp(remote_addr, empty_addr, sizeof(bt_bd_addr_t))) {
        bt_connection_manager_device_local_info_store_local_address(remote_addr);
    } else {
        bt_connection_manager_assert(0 && "Peer address is null, please check the enviroment is setup right: each earbud must set local and peer address." );
    }

    bt_connection_manager_device_local_info_store_peer_aws_address(&local_addr);

    return;
}

void  bt_connection_manager_state_machine_ext_handle_state_switching(void)
{
    bt_connection_manager_state_ext_t from = BT_CONNECTION_MANAGER_STATE_EXT_NONE;
    uint32_t reason = bt_connection_manager_state_machine_cntx.switch_reason;
    void *data = bt_connection_manager_state_machine_cntx.switch_data;
    bt_connection_manager_state_ext_t to = bt_connection_manager_state_machine_cntx.goto_state;
    bt_connection_manager_state_machine_cntx.goto_state = BT_CONNECTION_MANAGER_STATE_EXT_NONE;
    if (bt_connection_manager_state_machine_cntx.active_state) {
        from = bt_connection_manager_state_machine_get_state();
        if (bt_connection_manager_state_machine_cntx.active_state->on_exit) {
            bt_connection_manager_state_machine_cntx.active_state->on_exit(from, reason, data);
        }
    }
    bt_connection_manager_state_machine_cntx.active_state = bt_connection_manager_state_machine_table + to;
    bt_cmgr_report_id("[BT_CM_SM][I]bt_connection_manager_state_machine_ext_handle_state_switching(%d -> %d) %d", 3, from, to, reason);
    bt_connection_manager_state_machine_cntx.active_state->on_enter(from, reason, data);
}

void bt_connection_manager_state_ext_defualt_on_enter(bt_connection_manager_state_ext_t from, uint32_t reason, void *user_data)
{

}

void bt_connection_manager_state_ext_defualt_on_exit(bt_connection_manager_state_ext_t state, uint32_t reason, void *user_data)
{

}

bt_status_t bt_connection_manager_state_ext_defualt_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    //bt_cmgr_report_id("[BT_CM_SM][W]bt_connection_manager_state_ext_defualt_bt_fun, msg(0x%08x), status(0x%08x), buffer(0x%08x)", 3, msg, status, buffer);
    return BT_STATUS_SUCCESS;

}

void bt_connection_manager_state_ext_defualt_sink_fun(bt_sink_srv_event_t event_id, void *params)
{
    //bt_cmgr_report_id("[BT_CM_SM][W]bt_connection_manager_state_ext_in_case_defualt_fun, event_id(0x%08x), params(0x%08x)", 2, event_id, params);
}

void bt_connection_manager_state_ext_default_misc_fun(bt_connection_manager_misc_func_type_t type, void *params)
{
    //bt_cmgr_report_id("[BT_CM_SM][W]bt_connection_manager_state_ext_default_misc_fun, type(%d), params(0x%08x)", 2, type, params);
}

void bt_connection_manager_state_ext_in_case_misc_fun(bt_connection_manager_misc_func_type_t type, void *params)
{
    switch (type) {
        case BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_TAKEOUT_FROM_CASE: {
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            if(role == BT_AWS_MCE_ROLE_AGENT){
                if(bt_connection_manager_state_machine_ext_agent_connected_with_partner()) {
                    if(bt_connection_manager_state_machine_ext_agent_connected_with_sp()) {
                        bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_CONNECT, 0, NULL);
                    } else {
                        bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT, 0, NULL);
                    }
                } else {
                    if(bt_connection_manager_state_machine_ext_agent_connected_with_sp()) {
                        bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_CON_LS, 0, NULL);
                    } else {
                        bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
                    }
                }
            } else {
                if(bt_connection_manager_state_machine_ext_partner_connected_with_agent()){
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_PARTNER, 0, NULL);
                } else {
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_SCAN, 0, NULL);
                    bt_connection_manager_state_machine_ext_start_scan_timer();
                }
            }
            break;

        }
        default:
            break;
    }
}

void bt_connection_manager_state_ext_standby_on_exit(bt_connection_manager_state_ext_t state, uint32_t reason, void *user_data)
{
    bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ALL);
}

bt_status_t bt_connection_manager_state_ext_standby_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();
    switch (msg) {
        case BT_GAP_LINK_STATUS_UPDATED_IND: {
            if ((role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER)) {
                bt_cmgr_report_id("[BT_CM_SM][I] standby bt fun: partner skip gap status", 0);
            } else {
                /* phone is connected. */
                bt_gap_link_status_updated_ind_t *param = (bt_gap_link_status_updated_ind_t *)buffer;
                bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
                if (BT_GAP_LINK_STATUS_CONNECTED_0 <= param->link_status && memcmp(param->address, local_addr, sizeof(bt_bd_addr_t))) {
                    bt_cmgr_report_id("[BT_CM_SM][I] standby state: sp connected", 0);
                    /* reset reconnect flag */
                    bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ALL);
                    if (BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE == reinit) {
                        bt_connection_manager_state_machine_ext_stop_standby_timer();
                        bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_CON_LS, 0, NULL);
                    } else {
                        bt_cmgr_report_id("[BT_CM_SM][I] standby state: is during role switch, ignore", 0);
                    }
                }
            }
            break;
        }
        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

void bt_connection_manager_state_ext_standby_sink_fun(bt_sink_srv_event_t event_id, void *params)
{
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)params;

    switch (event_id) {
        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE: {
            /* partner attach to agent*/
            bt_bd_addr_t *agent_addr = bt_connection_manager_device_local_info_get_peer_aws_address();
            bt_sink_srv_connection_information_t *connection_info = (bt_sink_srv_connection_information_t *)params;
            if ((role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER) && !memcmp(&(connection_info->bt_addr), agent_addr, sizeof(bt_bd_addr_t))
                    && (connection_info->profile_type & BT_SINK_SRV_PROFILE_AWS)) {
                bt_cmgr_report_id("[BT_CM_SM][I] standby sink fun: partner attach to agent", 0);
                if (BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE == reinit) {
                    bt_connection_manager_state_machine_ext_stop_standby_timer();
                    bt_connection_manager_state_machine_ext_start_partner_sync_agent_timer();
                    //bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_PARTNER, 0, NULL);
                } else {
                    bt_cmgr_report_id("[BT_CM_SM][I] standby state: is during role switch, ignore", 0);
                }
            }
            break;
        }
        case BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE: {
            /* agent is attached by partner*/
            if (role == BT_AWS_MCE_ROLE_AGENT && event->aws_state_change.aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                bt_cmgr_report_id("[BT_CM_SM][I]standby sink fun: partner is connected", 0);
                if (BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE == reinit) {
                    bt_connection_manager_state_machine_ext_stop_standby_timer();
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT, 0, NULL);
                    /*start reconnect SP*/
                    bt_connection_manager_connect_ext();
                } else {
                    bt_cmgr_report_id("[BT_CM_SM][I] standby state: is during role switch, ignore", 0);
                }
            }
            break;
        }
        default:
            break;

    }
}

void bt_connection_manager_state_ext_standby_misc_fun(bt_connection_manager_misc_func_type_t type, void *params)
{
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();

    switch (type) {
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_ROLE_STANDBY_TIMEOUT: {
            bt_cmgr_report_id("[BT_CM_SM][I] standby misc fun: role standby time out", 0);
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();
            if(BT_AWS_MCE_ROLE_NONE == role){
                bt_cmgr_report_id("[BT_CM_SM][E] local role is None", 0);
                return;
            }

            if(role & BT_AWS_MCE_ROLE_AGENT){
                if ((bt_device_manager_get_paired_number() != 0) || (0x01 == KEEP_ROLE_RECOVER)) {
                    bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ALL);
                    if (BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_LOCK == bt_connection_manager_state_machine_ext_get_lock_state()) {
                        /* role recovert was disallow, then switch to REC_LS for agent role power on */
                        bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
                    } else {
                        if (!bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()){
                            bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH);
                            bt_connection_manager_power_reset(false);
                        }
                    }
                } else {
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
                }

            } else {
                if(bt_device_manager_get_paired_number() != 0){
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_SCAN, 0, NULL);
                    bt_connection_manager_state_machine_ext_start_scan_timer();
                } else {
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_SCAN, 0, NULL);
                }

            }

            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_OFF: {
            if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH) {
                bt_cmgr_report_id("[BT_CM_SM][I] standby state: bt power off", 0);

                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();

                /*switch local address and peer address*/
                bt_connection_manager_state_machine_ext_exchange_bt_address();

                if (role & BT_AWS_MCE_ROLE_AGENT) {
                    /*switch role to partner*/
                    bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_PARTNER);
                } else {
                    /*switch role to agent*/
                    bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_AGENT);
                }

                /*set BT address to BT stack*/
                bt_set_local_public_address((bt_bd_addr_ptr_t)bt_connection_manager_device_local_info_get_local_address());
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_ON: {
            if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH) {
                bt_cmgr_report_id("[BT_CM_SM][I] standby state: bt power on", 0);

                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();
                bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE);
                if (role & BT_AWS_MCE_ROLE_AGENT) {
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_SCAN, 0, NULL);
                } else {
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
                }
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC_TIMEOUT: {
            if(bt_device_manager_get_paired_number() != 0
                && (BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_UNLOCK == bt_connection_manager_state_machine_ext_get_lock_state())
                && !bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()){
                bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ALL);
                bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH);
                bt_connection_manager_power_reset(true);
            } else {
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_PARTNER, 0, NULL);
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC:{
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

            bt_connection_manager_state_machine_ext_stop_partner_sync_agent_timer();
            if (role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER) {
                bt_cmgr_report_id("[BT_CM_SM][I] standby misc fun: partner receive agent connected sync packet.", 0);
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_PARTNER, 0, NULL);
            }
            break;
        }
        default:
            break;
    }
}

void bt_connection_manager_state_ext_partner_sink_fun(bt_sink_srv_event_t event_id, void *params)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (event_id) {
        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE: {
            /* agent is lost. */
            bt_cmgr_report_id("[BT_CM_SM][I] partner sink fun: agent lost", 0);
            bt_sink_srv_connection_information_t *connection_info = (bt_sink_srv_connection_information_t *)params;
            bt_bd_addr_t *agent_addr = bt_connection_manager_device_local_info_get_peer_aws_address();
            if ((role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER) && !memcmp(&(connection_info->bt_addr), agent_addr, sizeof(bt_bd_addr_t))
                    && !(connection_info->profile_type & BT_SINK_SRV_PROFILE_AWS) && bt_connection_manager_state_machine_ext_get_reinit_flag() != BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH) {
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_SCAN, 0, NULL);
            }
            break;
        }
        default:
            break;

    }
}

void bt_connection_manager_state_ext_partner_misc_fun(bt_connection_manager_misc_func_type_t type, void *params)
{
    if (type == 0xFF) {
        //todo: ROLE switch by connection manger common API/event
        bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT, 0, NULL);
        bt_sink_srv_send_action(BT_SINK_SRV_ACTION_START_RECONNECT, NULL);

    }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    else if (type == BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_RHO) {
        bt_connection_manager_misc_func_params_rho_event_t *rho_event = (bt_connection_manager_misc_func_params_rho_event_t *)params;
        if (rho_event->event == BT_ROLE_HANDOVER_COMPLETE_IND) {
            if (rho_event->status == BT_STATUS_SUCCESS) {
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_CONNECT, 0, NULL);
            }  else {
                bt_cmgr_report_id("[BT_CM_SM][I] partner sink fun: rho fail", 0);
            }
        }
    }
#endif
    else if (type == BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_ROLE_CHANGE_MANUALLY) {
        bt_connection_manager_state_machine_ext_stop_scan_timer();
        if (!bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()){
            bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH);
            bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_SCAN, 0, NULL);
            bt_connection_manager_power_reset(false);
        }
    }
}

bt_status_t bt_connection_manager_state_ext_reconnect_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    switch (msg) {
        case BT_GAP_LINK_STATUS_UPDATED_IND: {
            /* phone is connected. */
            bt_gap_link_status_updated_ind_t *param = (bt_gap_link_status_updated_ind_t *)buffer;
            bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
            if (BT_GAP_LINK_STATUS_CONNECTED_0 <= param->link_status && memcmp(param->address, local_addr, sizeof(bt_bd_addr_t))) {
                bt_cmgr_report_id("[BT_CM_SM][I] reconnect state: sp connected", 0);
                /* reset reconnect flag */
                bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ALL);
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_CONNECT, 0, NULL);
            }
            break;
        }
        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}


void bt_connection_manager_state_ext_reconnect_sink_fun(bt_sink_srv_event_t event_id, void *params)
{
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)params;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (event_id) {
        case BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE: {
            /* partner lost*/
            if (role == BT_AWS_MCE_ROLE_AGENT && event->aws_state_change.aws_state == BT_AWS_MCE_AGENT_STATE_INACTIVE
                && bt_connection_manager_state_machine_ext_get_reinit_flag() != BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH) {
                bt_cmgr_report_id("[BT_CM_SM][I]reconnect sink fun: partner lost ", 0);
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
                bt_connection_manager_state_machine_ext_start_agent_change_timer();
            }
            break;
        }

        default:
            break;

    }
}

bt_status_t bt_connection_manager_state_ext_connect_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_bd_addr_t dev_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER] = {{0}};

    switch (msg) {
        case BT_GAP_LINK_STATUS_UPDATED_IND: {
            /* phone is disconnected. */
            bt_gap_link_status_updated_ind_t *param = (bt_gap_link_status_updated_ind_t *)buffer;
            bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
            if (BT_GAP_LINK_STATUS_DISCONNECTED == param->link_status && memcmp(param->address, local_addr, sizeof(bt_bd_addr_t))) {
                bt_cmgr_report_id("[BT_CM_SM][I] connect bt fun: sp disconnected, error (0x%x)", 1, status);
                uint32_t connected_dev = bt_sink_srv_cm_get_connected_device_list(BT_SINK_SRV_PROFILE_NONE, dev_list, BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
                bt_cmgr_report_id("[BT_CM_SM][E] connect bt fun: connected_dev (%d).", 1, connected_dev);
                if (connected_dev == 0) {
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT, 0, NULL);
                }
            }
            break;
        }
        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

void bt_connection_manager_state_ext_connect_sink_fun(bt_sink_srv_event_t event_id, void *params)
{
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)params;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    //bt_bd_addr_t dev_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER] = {0};

    switch (event_id) {
#if 0
        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE: {
            /* phone is disconnected. */
            bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
            bt_sink_srv_connection_information_t *connection_info = (bt_sink_srv_connection_information_t *)params;
            if (memcmp(&(connection_info->bt_addr), local_addr, sizeof(bt_bd_addr_t))
                    && (connection_info->profile_type == BT_SINK_SRV_PROFILE_NONE)) {
                bt_cmgr_report_id("[BT_CM_SM][I] connect sink fun: sp disconnect agent.", 0);
                /*phone disconnenct and partner  will also disconnect*/
                bt_sink_srv_cm_remote_device_t *device_p = bt_sink_srv_cm_find_device(BT_SINK_SRV_CM_FIND_BY_ADDR, (void *)connection_info->bt_addr);
                if(NULL == device_p) {
                    bt_cmgr_report_id("[BT_CM_SM][E] not find device pointer with addr:%02X-%02X-%02X-%02X-%02X-%02X", 6,
                        connection_info->bt_addr[0], connection_info->bt_addr[1], connection_info->bt_addr[2], connection_info->bt_addr[3], connection_info->bt_addr[4], connection_info->bt_addr[5]);
                    return;
                }
                if (device_p->aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                    bt_cmgr_report_id("[BT_CM_SM][E] connect sink fun: aws state (%d).", 1, device_p->aws_state);
                }
                uint32_t connected_dev = bt_sink_srv_cm_get_connected_device_list(BT_SINK_SRV_PROFILE_NONE, dev_list, BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
                bt_cmgr_report_id("[BT_CM_SM][E] connect sink fun: connected_dev (%d).", 1, connected_dev);
                if (connected_dev <= 1) {
                    //bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
                    //bt_connection_manager_state_machine_ext_start_agent_change_timer();
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT, 0, NULL);
                }
            }
            break;
        }
#endif
        case BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE: {
            /* partner lost*/
            bt_cmgr_report_id("[BT_CM_SM][I]connect sink fun: partner lost ", 0);
            if (role == BT_AWS_MCE_ROLE_AGENT && event->aws_state_change.aws_state == BT_AWS_MCE_AGENT_STATE_INACTIVE) {
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_CON_LS, 0, NULL);
                /* start a timer to check partner whether disconnect during 3000ms, if yes, then agent wait 5120ms to reconnect SP */
                bt_connection_manager_state_machine_ext_start_check_link_timer();
            }
            break;
        }
        default:
            break;

    }
}

void bt_connection_manager_state_ext_connect_misc_fun(bt_connection_manager_misc_func_type_t type, void *params)
{
    if (type == BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_RHO) {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        bt_connection_manager_misc_func_params_rho_event_t *rho_event = (bt_connection_manager_misc_func_params_rho_event_t *)params;
        if (rho_event->event == BT_ROLE_HANDOVER_COMPLETE_IND) {
            if (rho_event->status == BT_STATUS_SUCCESS) {
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_PARTNER, 0, NULL);
            }  else {
                bt_cmgr_report_id("[BT_CM_SM][I] connect misc fun: rho fail", 0);
            }
        }
#endif
    }
}

void bt_connection_manager_state_ext_rec_ls_on_enter(bt_connection_manager_state_ext_t from, uint32_t reason, void *user_data)
{
    /* temp set role recovery timer to 7s */
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_CONNECTION_MANAGER_STATE_EXT_TIMER_AGENT_TO_PARTNER,
                7000, bt_connection_manager_state_machine_ext_timeout_callback);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_connection_manager_state_machine_ext_gpt_start);
    bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state in, start 7000ms timer to check SP connection", 0);

    return;
}


bt_status_t bt_connection_manager_state_ext_rec_ls_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();

    switch (msg) {
        case BT_GAP_LINK_STATUS_UPDATED_IND: {
            /* phone is connected. */
            bt_gap_link_status_updated_ind_t *param = (bt_gap_link_status_updated_ind_t *)buffer;
            bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
            if (BT_GAP_LINK_STATUS_CONNECTED_0 <= param->link_status && memcmp(param->address, local_addr, sizeof(bt_bd_addr_t))) {
                bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: sp connected", 0);
                /* reset reconnect flag */
                bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ALL);
                if (BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE == reinit) {
                    bt_connection_manager_state_machine_ext_stop_agent_change_timer();
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_CON_LS, 0, NULL);
                } else {
                    bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: is during role switch, ignore", 0);
                }
            }
            break;
        }
        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

void bt_connection_manager_state_ext_rec_ls_sink_fun(bt_sink_srv_event_t event_id, void *params)
{
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();

    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)params;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (event_id) {
        case BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE: {
            /* agent is attached by partner*/
            if (role == BT_AWS_MCE_ROLE_AGENT && event->aws_state_change.aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                bt_cmgr_report_id("[BT_CM_SM][I]rec_ls sink fun: partner is connected", 0);
                if (BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE == reinit) {
                    bt_connection_manager_state_machine_ext_stop_agent_change_timer();
                    bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT, 0, NULL);
                } else {
                    bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: is during role switch, ignore", 0);
                }
            }
            break;
        }
        case BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE: {
            bt_sink_srv_profile_connection_state_update_t *profile_state = (bt_sink_srv_profile_connection_state_update_t *)params;
            if (profile_state->state == BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED) {
                if (profile_state->reason == BT_HCI_STATUS_VENDOR_REMOTE_CONNECTION_EXIST ||
                    profile_state->reason == BT_HCI_STATUS_CONNECTION_TIMEOUT ||
                    profile_state->reason == BT_HCI_STATUS_PAGE_TIMEOUT) {
                    if (profile_state->reason == BT_HCI_STATUS_PAGE_TIMEOUT) {
                        bt_cmgr_report_id("[BT_CM_SM][I]rec_ls sink fun: connecting page timeout", 0);
                    } else {
                        bt_cmgr_report_id("[BT_CM_SM][I]rec_ls sink fun: connection already existed", 0);
                    }
                    bt_connection_manager_state_machine_ext_stop_agent_change_timer();
                    if (BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_UNLOCK == bt_connection_manager_state_machine_ext_get_lock_state()) {
                        if(!bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()){
                            bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH);
                            bt_connection_manager_state_machine_ext_start_async_timer();
                        }
                    }
                }
            }
            break;
        }

        default:
            break;

    }
}

void bt_connection_manager_state_ext_rec_ls_misc_fun(bt_connection_manager_misc_func_type_t type, void *params)
{
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();

    switch (type) {
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_REC_LS_TIME_OUT: {
            bt_cmgr_report_id("[BT_CM_SM][I] rec_ls misc fun: connect parnter/sp time out", 0);
            uint32_t paired_num = bt_device_manager_get_paired_number();
            bt_cmgr_report_id("[BT_CM_SM][I] rec_ls misc fun: paired number(%d)", 1, paired_num);
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

            if (BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_UNLOCK == bt_connection_manager_state_machine_ext_get_lock_state()) {
                if (paired_num && !bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()) {
                    bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_REC_LS_TIMEOUT);
                    bt_connection_manager_power_reset(false);
                } else if ((role & BT_AWS_MCE_ROLE_AGENT) && (0x01 == KEEP_ROLE_RECOVER)) {
                    if (!bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()) {
                        bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_REC_LS_TIMEOUT);
                        bt_connection_manager_power_reset(false);
                    }
                }
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_AGENT_CONN_SP: {
            if(bt_connection_manager_state_machine_ext_need_start_agent_switch_timer == 1) {
                bt_cmgr_report_id("[BT_CM_SM][I] rec_ls misc fun: agent connect sp", 0);
                bt_connection_manager_state_machine_ext_need_start_agent_switch_timer = 0;
                bt_connection_manager_state_machine_ext_stop_agent_change_timer();
                bt_connection_manager_state_machine_ext_start_agent_change_timer();
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_OFF: {
            if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH
                || reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_REC_LS_TIMEOUT) {
                bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: bt power off", 0);
                /*switch local(agent) address to peer(partner) address*/
                bt_connection_manager_state_machine_ext_exchange_bt_address();
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if (BT_AWS_MCE_ROLE_AGENT != role) {
                    bt_connection_manager_assert(0 && "role recovery error state with role !")
                }
                /*switch role to agent*/
                bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_PARTNER);

                /*set BT address to BT stack*/
                bt_set_local_public_address((bt_bd_addr_ptr_t)bt_connection_manager_device_local_info_get_local_address());
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_ON: {
            if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH ||
                reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_REC_LS_TIMEOUT) {
                bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: bt power on", 0);
                bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE);
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_SCAN, 0, NULL);
            }
            break;
        }
        default:
            break;
    }
}


bt_status_t bt_connection_manager_state_ext_conn_ls_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_bd_addr_t dev_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER] = {{0}};

    switch (msg) {
        case BT_GAP_LINK_STATUS_UPDATED_IND: {
            bt_gap_link_status_updated_ind_t *param = (bt_gap_link_status_updated_ind_t *)buffer;
            bt_bd_addr_t *local_addr = bt_connection_manager_device_local_info_get_local_address();
            if (BT_GAP_LINK_STATUS_DISCONNECTED == param->link_status && memcmp(param->address, local_addr, sizeof(bt_bd_addr_t))) {
                bt_cmgr_report_id("[BT_CM_SM][I] conn_ls state: sp disconnected, error (0x%x)", 1, status);
                uint32_t connected_dev = bt_sink_srv_cm_get_connected_device_list(BT_SINK_SRV_PROFILE_NONE, dev_list, BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
                bt_cmgr_report_id("[BT_CM_SM][E] conn_ls bt fun: connected_dev (%d).", 1, connected_dev);
                if (connected_dev == 0) {
                    if(BT_HCI_STATUS_CONNECTION_TERMINATED_BY_LOCAL_HOST == status || BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION == status
                        || (BT_HCI_STATUS_PIN_OR_KEY_MISSING == status && (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_real_role()))) {
                        bt_connection_manager_state_machine_ext_stop_check_link_timer();
						bt_connection_manager_allow_power_on_reconnect(BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_PASSIVE);
                        bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
                    } else {
                        if (bt_connection_manager_state_machine_ext_is_check_link_timer_active()) {
                            bt_connection_manager_state_machine_ext_stop_check_link_timer();
                            bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
                            bt_connection_manager_state_machine_ext_start_agent_change_timer();
                        } else {
                            if (BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_UNLOCK == bt_connection_manager_state_machine_ext_get_lock_state()) {
                                if(!bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()){
                                    bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH);
                                    bt_connection_manager_state_machine_ext_start_async_timer();
                                }
                            }
                        }
                    }
                }
            }
            break;
        }

        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

void bt_connection_manager_state_ext_conn_ls_sink_fun(bt_sink_srv_event_t event_id, void *params)
{
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)params;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (event_id) {
        case BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE: {
            /* agent is attached by partner*/
            if (role == BT_AWS_MCE_ROLE_AGENT && event->aws_state_change.aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
                bt_cmgr_report_id("[BT_CM_SM][I]conn ls sink fun: partner is connected", 0);
                bt_connection_manager_state_machine_ext_stop_check_link_timer();
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_CONNECT, 0, NULL);
            }
            break;
        }

        default:
            break;

    }
}

void bt_connection_manager_state_ext_conn_ls_misc_fun(bt_connection_manager_misc_func_type_t type, void *params)
{
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();
    switch (type) {
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_OFF: {
            bt_cmgr_report_id("[BT_CM_SM][I] conn_ls state: bt power off", 0);
            if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH) {
                /*switch local(agent) address to peer(partner) address*/
                bt_connection_manager_state_machine_ext_exchange_bt_address();
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if (BT_AWS_MCE_ROLE_AGENT != role) {
                    bt_connection_manager_assert(0 && "role recovery error state with role !")
                }
                /*switch role to partner*/
                bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_PARTNER);

                /*set BT address to BT stack*/
                bt_set_local_public_address((bt_bd_addr_ptr_t)bt_connection_manager_device_local_info_get_local_address());
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_ON: {
            bt_cmgr_report_id("[BT_CM_SM][I] conn_ls state: bt power on", 0);
            if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH) {
                bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE);
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_SCAN, 0, NULL);
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_CHECK_LINK_TIMEOUT: {
            bt_cmgr_report_id("[BT_CM_SM][I] conn_ls state: check link state timeout", 0);
            break;
        }
        default:
            break;
    }
}

bt_status_t bt_connection_manager_state_ext_scan_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    switch (msg) {

        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}


void bt_connection_manager_state_ext_scan_sink_fun(bt_sink_srv_event_t event_id, void *params)
{
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    switch (event_id) {
        case BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE: {
            /* partner attach to agent*/
            //bt_cmgr_report_id("[BT_CM_SM][I] scan sink fun: partner attach to agent", 0);
            bt_bd_addr_t *agent_addr = bt_connection_manager_device_local_info_get_peer_aws_address();
            bt_sink_srv_connection_information_t *connection_info = (bt_sink_srv_connection_information_t *)params;
            if ((role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER) && !memcmp(&(connection_info->bt_addr), agent_addr, sizeof(bt_bd_addr_t))
                    && (connection_info->profile_type & BT_SINK_SRV_PROFILE_AWS)) {
                bt_cmgr_report_id("[BT_CM_SM][I] scan sink fun: partner attach to agent", 0);

                if (BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE == reinit) {
                    bt_connection_manager_state_machine_ext_stop_scan_timer();
                    bt_connection_manager_state_machine_ext_start_partner_sync_agent_timer();
                    //bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_PARTNER, 0, NULL);
                } else {
                    bt_cmgr_report_id("[BT_CM_SM][I] scan state: is during role switch, ignore", 0);
                }
            }
            break;
        }
        default:
            break;

    }
}

void bt_connection_manager_state_ext_scan_misc_fun(bt_connection_manager_misc_func_type_t type, void *params)
{
    bt_connection_manager_state_bt_reinit_type_t reinit = bt_connection_manager_state_machine_ext_get_reinit_flag();
    switch (type) {
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_SCAN_TIME_OUT: {
            bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: connect agent time out", 0);
            //bt_connection_manager_state_machine_ext_stop_scan_timer();
            uint32_t paired_num = bt_device_manager_get_paired_number();
            bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: paired number(%d)", 1, paired_num);
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();

            if (BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_UNLOCK == bt_connection_manager_state_machine_ext_get_lock_state()) {
                if (!bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty() && paired_num) {
                    bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_SCAN_TIMEOUT);
                    bt_connection_manager_power_reset(false);
                } else if ((role & BT_AWS_MCE_ROLE_AGENT) && (0x01 == KEEP_ROLE_RECOVER)) {
                    if (!bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()) {
                        bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_SCAN_TIMEOUT);
                        bt_connection_manager_power_reset(false);
                    }
                }
            }

            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_PARTNER_CONN_AGENT: {
            bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: parnter connect agent", 0);
            bt_connection_manager_state_machine_ext_stop_scan_timer();
            bt_connection_manager_state_machine_ext_start_scan_timer();
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_OFF: {
            bt_cmgr_report_id("[BT_CM_SM][I] scan state: bt power off", 0);
            if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_SCAN_TIMEOUT
                || reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH
                || reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_CHANGE_MANUALLY) {
                /*switch local(agent) address and remote(partner) address*/
                bt_connection_manager_state_machine_ext_exchange_bt_address();
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if (BT_AWS_MCE_ROLE_PARTNER != role) {
                    bt_connection_manager_assert(0 && "role recovery error state with role !")
                }
                /*switch role to agent*/
                bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_AGENT);

                /*set BT address to BT stack*/
                bt_set_local_public_address((bt_bd_addr_ptr_t)bt_connection_manager_device_local_info_get_local_address());
            }

            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_ON: {
            bt_cmgr_report_id("[BT_CM_SM][I] scan state: bt power on", 0);
            if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_SCAN_TIMEOUT
                || reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH
                || reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_CHANGE_MANUALLY) {
                if (reinit == BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_CHANGE_MANUALLY) {
                    bt_connection_manager_state_machine_ext_need_start_agent_switch_timer = 0;
                } else {
                    bt_connection_manager_state_machine_ext_need_start_agent_switch_timer = 1;
                }
                bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE);
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_REC_LS, 0, NULL);
#if 0
                uint32_t paired_num = bt_device_manager_get_paired_number();
                bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: paired number(%d)", 1, paired_num);
                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();
                /* if real role is agent && no paired device, continue do role recover */
                if (!paired_num && (role & BT_AWS_MCE_ROLE_AGENT) && (0x01 == KEEP_ROLE_RECOVER)) {
                    bt_connection_manager_state_machine_ext_need_start_agent_switch_timer = 0;
                    bt_connection_manager_state_machine_ext_start_agent_change_timer();
                }
#endif
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_ROLE_CHANGE_MANUALLY: {
            bt_connection_manager_state_machine_ext_stop_scan_timer();
            if (!bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()){
                bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_CHANGE_MANUALLY);
                bt_connection_manager_power_reset(false);
	        }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC_TIMEOUT: {
            if(bt_device_manager_get_paired_number() != 0
                && (BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_UNLOCK == bt_connection_manager_state_machine_ext_get_lock_state())
                && !bt_connection_manager_state_machine_ext_is_aws_peer_addr_empty()){
                bt_connection_manager_state_machine_ext_set_reinit_flag(BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH);
                bt_connection_manager_power_reset(true);
            } else {
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_PARTNER, 0, NULL);
            }
            break;
        }
        case BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC: {
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

            bt_connection_manager_state_machine_ext_stop_partner_sync_agent_timer();
            if (role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER) {
                bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: partner receive agent connected sync packet.", 0);
                bt_connection_manager_state_ext_switch(BT_CONNECTION_MANAGER_STATE_EXT_PARTNER, 0, NULL);
            }
            break;
        }
        default:
            break;
    }
}
#endif //#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
