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
#include "bt_aws_mce_role_recovery.h"
#include "bt_device_manager.h"
#include "bt_device_manager_internal.h"
#include "bt_connection_manager_utils.h"
#include "bt_connection_manager_internal.h"
#include "hal_trng.h"
#include "hal_gpt.h"
#include "bt_system.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif

#include "bt_aws_mce_srv.h"


/* agent keep role recovery even if there is no paired device exist */
#define AGENT_KEEP_ROLE_RECOVER  0x01

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
typedef struct {
    bt_role_handover_event_t event;
    bt_status_t status;
} bt_aws_mce_role_recovery_misc_func_params_rho_event_t;
#endif

static bt_aws_mce_role_recovery_state_machine_context_t bt_aws_mce_role_recovery_state_machine_ctx;
static bt_aws_mce_role_recovery_reinit_type_t bt_aws_mce_role_recovery_bt_reinit = BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE;
static uint32_t bt_aws_mce_role_recovery_gpt_start = 0;
static uint32_t bt_aws_mce_role_recovery_gpt_end = 0;
static bt_aws_mce_role_recovery_lock_state_type_t bt_aws_mce_role_recovery_lock_state = 0;
/* only first time partner scan timeout 1~2s */
static bool bt_aws_mce_role_recovery_is_first_scan = false;

static void bt_aws_mce_role_recovery_restart(void);
static void bt_aws_mce_role_recovery_deinit(void);
static bt_status_t bt_aws_mce_role_recovery_handle_callback(bt_cm_event_t event_id, void *params, uint32_t params_len);
static void bt_aws_mce_role_recovery_bt_power_state_notify(uint8_t is_on);
static void bt_aws_mce_role_recovery_action_handler(bt_cm_event_t event, void *param);
static void bt_aws_mce_role_recovery_sink_event_callback(bt_cm_event_t event_id, void *param, uint32_t param_len);
static int32_t bt_aws_mce_role_recovery_state_switch (bt_aws_mce_role_recovery_state_t to, uint32_t reason, void *user_data);
static void bt_aws_mce_role_recovery_state_defualt_on_enter(bt_aws_mce_role_recovery_state_t from, uint32_t reason, void *user_data);
static void bt_aws_mce_role_recovery_state_defualt_on_exit(bt_aws_mce_role_recovery_state_t state, uint32_t reason, void *user_data);
static bt_status_t bt_aws_mce_role_recovery_state_defualt_bt_func(bt_msg_type_t msg, bt_status_t status, void *buffer);
static void bt_aws_mce_role_recovery_state_defualt_sink_func(bt_cm_event_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_in_case_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params);
static void bt_aws_mce_role_recovery_state_standby_on_exit(bt_aws_mce_role_recovery_state_t state, uint32_t reason, void *user_data);
static void bt_aws_mce_role_recovery_state_standby_sink_func(bt_cm_event_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_standby_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params);
static void bt_aws_mce_role_recovery_state_partner_sink_func(bt_cm_event_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_partner_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params);
static void bt_aws_mce_role_recovery_state_reconnect_sink_func(bt_cm_event_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_reconnect_misc_func(bt_aws_mce_role_recovery_misc_func_type_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_connect_sink_func(bt_cm_event_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_connect_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params);
static void bt_aws_mce_role_recovery_state_rec_ls_on_enter(bt_aws_mce_role_recovery_state_t from, uint32_t reason, void *user_data);
static bt_status_t bt_aws_mce_role_recovery_state_rec_ls_bt_func(bt_msg_type_t msg, bt_status_t status, void *buffer);
static void bt_aws_mce_role_recovery_state_rec_ls_sink_func(bt_cm_event_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_rec_ls_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params);
static void bt_aws_mce_role_recovery_state_conn_ls_sink_func(bt_cm_event_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_conn_ls_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params);
static bt_status_t bt_aws_mce_role_recovery_state_scan_bt_func(bt_msg_type_t msg, bt_status_t status, void *buffer);
static void bt_aws_mce_role_recovery_state_scan_sink_func(bt_cm_event_t event_id, void *params);
static void bt_aws_mce_role_recovery_state_scan_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params);

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
static void bt_aws_mce_role_recovery_rho_event_notify(bt_role_handover_event_t event, bt_status_t status);
#endif

static void bt_aws_mce_role_recovery_set_reinit_flag(bt_aws_mce_role_recovery_reinit_type_t type);
static bt_aws_mce_role_recovery_reinit_type_t bt_aws_mce_role_recovery_get_reinit_flag(void);
static const bt_aws_mce_role_recovery_state_machine_item_t* bt_aws_mce_role_recovery_get_active_state(void);

static void bt_aws_mce_role_recovery_start_scan_timer();
static void bt_aws_mce_role_recovery_stop_scan_timer();
static void bt_aws_mce_role_recovery_handle_state_switching(void);
static void bt_aws_mce_role_recovery_start_agent_change_timer();
static void bt_aws_mce_role_recovery_stop_agent_change_timer();
static void bt_aws_mce_role_recovery_start_standby_timer();
static void bt_aws_mce_role_recovery_stop_standby_timer();
static uint8_t bt_aws_mce_role_recovery_is_aws_peer_addr_empty();
static void bt_aws_mce_role_recovery_set_reconnect(bool is_allow, bool is_initiate_connection);



static const bt_aws_mce_role_recovery_state_machine_item_t bt_aws_mce_role_recovery_state_machine_table[] = {
    {/*BT_AWS_MCE_ROLE_RECOVERY_STATE_STANDBY*/
        .allow_list =  BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS_BIT,
        .on_enter = bt_aws_mce_role_recovery_state_defualt_on_enter,
        .on_exit = bt_aws_mce_role_recovery_state_standby_on_exit,
        .bt_fun = bt_aws_mce_role_recovery_state_defualt_bt_func,
        .sink_fun = bt_aws_mce_role_recovery_state_standby_sink_func,
        .misc_fun = bt_aws_mce_role_recovery_state_standby_misc_func,
    },
    {/* BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT */
        .allow_list = BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS_BIT,
        .on_enter = bt_aws_mce_role_recovery_state_defualt_on_enter,
        .on_exit = bt_aws_mce_role_recovery_state_defualt_on_exit,
        .bt_fun = bt_aws_mce_role_recovery_state_defualt_bt_func,
        .sink_fun = bt_aws_mce_role_recovery_state_reconnect_sink_func,
        .misc_fun = bt_aws_mce_role_recovery_state_reconnect_misc_func,
    },
    {/* BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT */
        .allow_list = BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS_BIT,
        .on_enter = bt_aws_mce_role_recovery_state_defualt_on_enter,
        .on_exit = bt_aws_mce_role_recovery_state_defualt_on_exit,
        .bt_fun = bt_aws_mce_role_recovery_state_defualt_bt_func,
        .sink_fun = bt_aws_mce_role_recovery_state_connect_sink_func,
        .misc_fun = bt_aws_mce_role_recovery_state_connect_misc_func,
    },
    {/* BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER */
        .allow_list = BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT_BIT,
        .on_enter = bt_aws_mce_role_recovery_state_defualt_on_enter,
        .on_exit = bt_aws_mce_role_recovery_state_defualt_on_exit,
        .bt_fun = bt_aws_mce_role_recovery_state_defualt_bt_func,
        .sink_fun = bt_aws_mce_role_recovery_state_partner_sink_func,
        .misc_fun = bt_aws_mce_role_recovery_state_partner_misc_func,
    },
    {/* BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS */
        .allow_list = BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS_BIT,
        .on_enter = bt_aws_mce_role_recovery_state_rec_ls_on_enter,
        .on_exit = bt_aws_mce_role_recovery_state_defualt_on_exit,
        .bt_fun = bt_aws_mce_role_recovery_state_rec_ls_bt_func,
        .sink_fun = bt_aws_mce_role_recovery_state_rec_ls_sink_func,
        .misc_fun = bt_aws_mce_role_recovery_state_rec_ls_misc_func,
    },
    {/* BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS */
        .allow_list = BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS_BIT,
        .on_enter = bt_aws_mce_role_recovery_state_defualt_on_enter,
        .on_exit = bt_aws_mce_role_recovery_state_defualt_on_exit,
        .bt_fun = bt_aws_mce_role_recovery_state_defualt_bt_func,
        .sink_fun = bt_aws_mce_role_recovery_state_conn_ls_sink_func,
        .misc_fun = bt_aws_mce_role_recovery_state_conn_ls_misc_func,
    },
    {/* BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN */
        .allow_list = BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER_BIT | BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS_BIT,
        .on_enter = bt_aws_mce_role_recovery_state_defualt_on_enter,
        .on_exit = bt_aws_mce_role_recovery_state_defualt_on_exit,
        .bt_fun = bt_aws_mce_role_recovery_state_scan_bt_func,
        .sink_fun = bt_aws_mce_role_recovery_state_scan_sink_func,
        .misc_fun = bt_aws_mce_role_recovery_state_scan_misc_func,
    },
    {/* BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE */
        .allow_list = BT_AWS_MCE_ROLE_RECOVERY_STATE_ALL_BIT,
        .on_enter = bt_aws_mce_role_recovery_state_defualt_on_enter,
        .on_exit = bt_aws_mce_role_recovery_state_defualt_on_exit,
        .bt_fun = bt_aws_mce_role_recovery_state_defualt_bt_func,
        .sink_fun = bt_aws_mce_role_recovery_state_defualt_sink_func,
        .misc_fun = bt_aws_mce_role_recovery_state_in_case_misc_func,
    },
};

static bt_status_t bt_aws_mce_role_recovery_handle_callback(bt_cm_event_t event_id, void *params, uint32_t params_len)
{
    bt_cmgr_report_id("[BT_CM_SM][D] callback event 0x:%x", 1, event_id);
    switch (event_id) {
        case BT_CM_EVENT_POWER_STATE_UPDATE: {
            bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)params;
            if (NULL == power_update) {
                break;
            }
            bt_cmgr_report_id("[BT_CM_SM][I] callback power_state 0x:%x", 1, power_update->power_state);
            if (BT_CM_POWER_STATE_ON == power_update->power_state) {
                bt_aws_mce_role_recovery_bt_power_state_notify(1);
            } else if (BT_CM_POWER_STATE_OFF == power_update->power_state) {
                bt_aws_mce_role_recovery_bt_power_state_notify(0);
            }
            break;
        }
        case BT_CM_EVENT_VISIBILITY_STATE_UPDATE: {
            bt_cm_visibility_state_update_ind_t *visible_update = (bt_cm_visibility_state_update_ind_t *)params;
            if (NULL == visible_update) {
                break;
            }
            bt_cmgr_report_id("[BT_CM_SM][I] callback visible_update 0x:%x", 1, visible_update->visibility_state);
#if 0
            /* agent enable discovery, should stop role recovery */
            if (visible_update->visibility_state) {
                if(bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT){
                    bt_cmgr_report_id("[BT_CM_SM][I] enable discoverable", 0);
                    bt_aws_mce_role_recovery_stop_agent_change_timer();
                }
            }
#endif
            break;
        }
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)params;
            if (NULL == remote_update) {
                break;
            }
            bt_cmgr_report_id("[BT_CM_SM][I] callback remote_info_update, connected_service = 0x%x, pre_connected_service = 0x%x, acl_state = 0x%x, pre_acl_state = 0x%x", 4,
                remote_update->connected_service, remote_update->pre_connected_service, remote_update->acl_state, remote_update->pre_acl_state);
            bt_device_manager_dump_bt_address(BT_DEVICE_MANAGER_LOCAL_INFO_ADDRESS_AWS_LOCAL, (uint8_t *)(&(remote_update->address)));
            bt_dmgr_report_id("[BT_CM_SM] remote_update->address:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x", 6,
                 remote_update->address[0], remote_update->address[1], remote_update->address[2],
                 remote_update->address[3], remote_update->address[4], remote_update->address[5]);
            bt_aws_mce_role_recovery_sink_event_callback(event_id, remote_update, sizeof(bt_cm_remote_info_update_ind_t));
            break;
        }
        case BT_CM_EVENT_PRE_CONNECT: {
            bt_cmgr_report_id("[BT_CM_SM][I] callback pre connection", 0);
            bt_bd_addr_t *addr = (bt_bd_addr_t *)params;
            if (NULL == addr) {
                break;
            }
            bt_aws_mce_role_recovery_action_handler(BT_CM_EVENT_PRE_CONNECT, addr);
            break;
        }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        case BT_CM_EVENT_RHO_STATE_UPDATE: {
            bt_cm_rho_state_update_ind_t *rho = (bt_cm_rho_state_update_ind_t *)params;
            if (NULL == rho) {
                break;
            }
            bt_aws_mce_role_recovery_rho_event_notify(rho->event, rho->status);
            break;
        }
#endif
        case BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_STARTED: {
            bt_cmgr_report_id("[BT_CM_SM][D] callback air pairing start", 0);
            bt_aws_mce_role_recovery_deinit();
            break;
         }
        case BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE: {
            bt_cmgr_report_id("[BT_CM_SM][D] callback air pairing end", 0);
            bt_aws_mce_role_recovery_restart();
            break;
         }
        case BT_AWS_MCE_SRV_EVENT_ROLE_CHANGED_IND: {
            bt_aws_mce_srv_switch_role_complete_ind_t *role_update = (bt_aws_mce_srv_switch_role_complete_ind_t *)params;
            if (NULL == role_update) {
                break;
            }
            bt_cmgr_report_id("[BT_CM_SM][I] callback role change, result= 0x%x, new role = 0x%x", 2,
                role_update->result, role_update->cur_aws_role);
            bt_aws_mce_role_recovery_sink_event_callback(event_id, role_update, sizeof(bt_aws_mce_srv_switch_role_complete_ind_t));
            break;
         }
        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}


void bt_aws_mce_role_recovery_init(void)
{
    bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE);
    memset(&bt_aws_mce_role_recovery_state_machine_ctx, 0x00, sizeof(bt_aws_mce_role_recovery_state_machine_ctx));
    bt_aws_mce_role_recovery_state_machine_ctx.goto_state = BT_AWS_MCE_ROLE_RECOVERY_STATE_STANDBY;
    bt_aws_mce_role_recovery_state_machine_ctx.active_state = bt_aws_mce_role_recovery_state_machine_table + BT_AWS_MCE_ROLE_RECOVERY_STATE_STANDBY;
    /* role recovery feature default is unlock */
    bt_aws_mce_role_recovery_lock_state = BT_AWS_MCE_ROLE_RECOVERY_UNLOCK;
    bt_status_t ret = bt_cm_register_event_callback(&bt_aws_mce_role_recovery_handle_callback);
    bt_cmgr_report_id("[BT_CM_SM][I] init state ret 0x:%x", 1, ret);
}

void bt_aws_mce_role_recovery_deinit(void)
{
    bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE);
    bt_aws_mce_role_recovery_stop_scan_timer();
    bt_aws_mce_role_recovery_stop_agent_change_timer();
    bt_aws_mce_role_recovery_state_machine_ctx.goto_state = BT_AWS_MCE_ROLE_RECOVERY_STATE_NONE;
    bt_aws_mce_role_recovery_state_machine_ctx.active_state = NULL;
    bt_cmgr_report_id("[BT_CM_SM][I] deinit role recovery.", 0);
}

void bt_aws_mce_role_recovery_restart(void)
{
    bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE);
    bt_aws_mce_role_recovery_stop_scan_timer();
    bt_aws_mce_role_recovery_stop_agent_change_timer();
    bt_aws_mce_role_recovery_state_machine_ctx.goto_state = BT_AWS_MCE_ROLE_RECOVERY_STATE_STANDBY;
    bt_aws_mce_role_recovery_state_machine_ctx.active_state = bt_aws_mce_role_recovery_state_machine_table + BT_AWS_MCE_ROLE_RECOVERY_STATE_STANDBY;
    /* role recovery feature default is unlock */
    bt_aws_mce_role_recovery_lock_state = BT_AWS_MCE_ROLE_RECOVERY_UNLOCK;
    bt_cmgr_report_id("[BT_CM_SM][I] restart role recovery.", 0);
}


void bt_aws_mce_role_recovery_lock(void)
{
    bt_aws_mce_role_recovery_lock_state = BT_AWS_MCE_ROLE_RECOVERY_LOCK;
    bt_cmgr_report_id("[BT_CM_SM][I] lock role recovery", 0);
}

void bt_aws_mce_role_recovery_unlock(void)
{
    bt_aws_mce_role_recovery_lock_state = BT_AWS_MCE_ROLE_RECOVERY_UNLOCK;
    bt_cmgr_report_id("[BT_CM_SM][I] unlock role recovery", 0);
}

bt_aws_mce_role_recovery_lock_state_type_t bt_aws_mce_role_recovery_get_lock_state(void)
{
    bt_cmgr_report_id("[BT_CM_SM][I] role recovery lock state = 0x%x", 1, bt_aws_mce_role_recovery_lock_state);
    return bt_aws_mce_role_recovery_lock_state;
}


static void bt_aws_mce_role_recovery_set_reinit_flag(bt_aws_mce_role_recovery_reinit_type_t type)
{
    bt_cmgr_report_id("[BT_CM_SM][I] old reinit = (0x%x), new reinit = (0x%x)", 2, bt_aws_mce_role_recovery_bt_reinit, type);
    bt_aws_mce_role_recovery_bt_reinit = type;
}

static bt_aws_mce_role_recovery_reinit_type_t bt_aws_mce_role_recovery_get_reinit_flag(void)
{
    bt_cmgr_report_id("[BT_CM_SM][I] current reinit = (0x%x)", 1, bt_aws_mce_role_recovery_bt_reinit);
    return bt_aws_mce_role_recovery_bt_reinit;
}


static const bt_aws_mce_role_recovery_state_machine_item_t* bt_aws_mce_role_recovery_get_active_state(void)
{
    if (bt_aws_mce_role_recovery_state_machine_ctx.active_state) {
        bt_cmgr_report_id("[BT_CM_SM][I] current active state = (0x%x)", 1, (bt_aws_mce_role_recovery_state_machine_ctx.active_state - bt_aws_mce_role_recovery_state_machine_table));
    } else {
        bt_cmgr_report_id("[BT_CM_SM][I] current active state is null", 0);
    }
    return bt_aws_mce_role_recovery_state_machine_ctx.active_state;
}



bt_aws_mce_role_recovery_state_t bt_connection_manager_state_machine_get_state(void)
{
    if (bt_aws_mce_role_recovery_state_machine_ctx.active_state == NULL) {
        return BT_AWS_MCE_ROLE_RECOVERY_STATE_NONE;
    } else {
        return (bt_aws_mce_role_recovery_state_t)(bt_aws_mce_role_recovery_state_machine_ctx.active_state - bt_aws_mce_role_recovery_state_machine_table);
    }
}

int32_t bt_aws_mce_role_recovery_state_switch (bt_aws_mce_role_recovery_state_t to, uint32_t reason, void *user_data)
{
    //check allow list.
    if ((bt_aws_mce_role_recovery_state_machine_ctx.active_state) && ((bt_aws_mce_role_recovery_state_machine_ctx.active_state->allow_list & (1 << to)) == 0)) {
        bt_cmgr_report_id("[BT_CM_SM][E]bt_aws_mce_role_recovery_state_switch (0x%02x -> 0x%02x) return error", 2, bt_connection_manager_state_machine_get_state(), to);
        bt_cm_assert(0 && "role recover state not allow");
        return -1;
    }
    if (bt_aws_mce_role_recovery_state_machine_ctx.goto_state != BT_AWS_MCE_ROLE_RECOVERY_STATE_NONE) {
        bt_cmgr_report_id("[BT_CM_SM][W]bt_aws_mce_role_recovery_state_switch (%d) skip %d", 2, to, bt_aws_mce_role_recovery_state_machine_ctx.goto_state);
    }
    bt_aws_mce_role_recovery_state_machine_ctx.goto_state = to;
    bt_aws_mce_role_recovery_state_machine_ctx.switch_reason = reason;
    bt_aws_mce_role_recovery_state_machine_ctx.switch_data = user_data;

    bt_aws_mce_role_recovery_handle_state_switching();
    return 0;
}

void bt_aws_mce_role_recovery_sink_event_callback(bt_cm_event_t event_id, void *param, uint32_t param_len)
{
    bt_cmgr_report_id("[BT_CM_SM][D] state ext sink callback: event_id(0x%08x), param(0x%08x), len(%d)", 3, event_id, param, param_len);
    bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)param;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            const bt_aws_mce_role_recovery_state_machine_item_t *active_state = bt_aws_mce_role_recovery_get_active_state();
            if (active_state && active_state->sink_fun) {
                active_state->sink_fun(event_id, event);
            }
            break;
        }
        case BT_AWS_MCE_SRV_EVENT_ROLE_CHANGED_IND: {
            const bt_aws_mce_role_recovery_state_machine_item_t *active_state = bt_aws_mce_role_recovery_get_active_state();
            if (active_state && active_state->misc_fun) {
                active_state->misc_fun(BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_CHANGE, param);
            }
            break;
        }
        default:
            break;
    }
}

uint8_t bt_aws_mce_role_recovery_is_in_role_resetup()
{
    if(bt_aws_mce_role_recovery_get_reinit_flag() == BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE) {
        return 0;
    } else {
        return 1;
    }
}

void bt_aws_mce_role_recovery_bt_power_state_notify(uint8_t is_on)
{
    const bt_aws_mce_role_recovery_state_machine_item_t *active_state = bt_aws_mce_role_recovery_get_active_state();
    if (active_state && active_state->misc_fun) {
        bt_aws_mce_role_recovery_misc_func_type_t type = is_on ? BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_ON : BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_OFF;
        active_state->misc_fun(type, NULL);
    }

    /* temp: power off force init aws info */
    if (is_on == 0) {
        bt_device_manager_aws_local_info_init();
    }
}

void bt_aws_mce_role_recovery_stop_role_change()
{
    if (bt_aws_mce_role_recovery_get_reinit_flag() == BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE){
        bt_aws_mce_role_recovery_stop_scan_timer();
        bt_aws_mce_role_recovery_stop_agent_change_timer();
    } else {
        bt_cmgr_report_id("[BT_CM_SM][E] alreay in role change flow, reason(%d)", 1, bt_aws_mce_role_recovery_bt_reinit);
    }

}

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
void bt_aws_mce_role_recovery_rho_event_notify(bt_role_handover_event_t event, bt_status_t status)
{
    bt_cmgr_report_id("[BT_CM_SM][I] rho event notify, type (%d), status(0x%08x)", 2, event, status);
    const bt_aws_mce_role_recovery_state_machine_item_t *active_state = bt_aws_mce_role_recovery_get_active_state();
    if (active_state && active_state->misc_fun) {
        bt_aws_mce_role_recovery_misc_func_params_rho_event_t rho_event;
        rho_event.event = event;
        rho_event.status = status;
        active_state->misc_fun(BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_RHO, &rho_event);
    }
}
#endif

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
void bt_aws_mce_role_recovery_timeout_callback(uint32_t timer_id, uint32_t data)
{
    bt_aws_mce_role_recovery_state_timer_type_t timer = (bt_aws_mce_role_recovery_state_timer_type_t)data;
    bt_aws_mce_role_recovery_misc_func_type_t type = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_aws_mce_role_recovery_gpt_end);
    bt_cmgr_report_id("[BT_CM_SM][I] gpt time diff (%d)", 1, (bt_aws_mce_role_recovery_gpt_end - bt_aws_mce_role_recovery_gpt_start) * 1000/32768);

    switch (timer) {
        case BT_AWS_MCE_ROLE_RECOVERY_TIMER_PARTNER_TO_AGENT: {
            type = BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_SCAN_TIMEOUT;
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_TIMER_AGENT_TO_PARTNER: {
            type = BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_REC_LS_TIMEOUT;
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_TIMER_STANDBY_WITH_OLD_ROLE: {
            type = BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_STANDBY_TIMEOUT;
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_TIMER_WAIT_SYNC_INFO: {
            type = BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC_TIMEOUT;
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_TIMER_CHECK_LINK_STATE: {
            type = BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_CHECK_LINK_TIMEOUT;
            break;
        }
        default:
            break;
    }
    const bt_aws_mce_role_recovery_state_machine_item_t *active_state = bt_aws_mce_role_recovery_get_active_state();
    if (active_state && active_state->misc_fun) {
        active_state->misc_fun(type, NULL);
    }
}
#endif

void bt_aws_mce_role_recovery_start_standby_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    uint32_t time_out = 0;
    bt_cmgr_report_id("[BT_CM_SM][I] start standby timer", 0);
    if(bt_device_manager_aws_local_info_get_real_role() & BT_AWS_MCE_ROLE_AGENT) {
        time_out = BT_AWS_MCE_ROLE_RECOVERY_STANDBY_STATE_TIMER + 650;
        bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_AWS_MCE_ROLE_RECOVERY_TIMER_STANDBY_WITH_OLD_ROLE,
                        time_out, bt_aws_mce_role_recovery_timeout_callback);
    } else {
        time_out = 10;
        bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_AWS_MCE_ROLE_RECOVERY_TIMER_STANDBY_WITH_OLD_ROLE,
                        time_out, bt_aws_mce_role_recovery_timeout_callback);
    }
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_aws_mce_role_recovery_gpt_start);
    bt_cmgr_report_id("[BT_CM_SM][I] start standby timer : %d", 1, time_out);
#endif

}

void bt_aws_mce_role_recovery_stop_standby_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop standby timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif

}

void bt_aws_mce_role_recovery_start_check_link_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    uint32_t timeout = 3000;

    bt_cmgr_report_id("[BT_CM_SM][I] start_check_link_timer: %d ms", 1, timeout);

    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_AWS_MCE_ROLE_RECOVERY_TIMER_CHECK_LINK_STATE,
                    timeout, bt_aws_mce_role_recovery_timeout_callback);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_aws_mce_role_recovery_gpt_start);
#endif
}

void bt_aws_mce_role_recovery_stop_check_link_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop check link timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif
}

bool bt_aws_mce_role_recovery_is_check_link_timer_active()
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

uint32_t bt_aws_mce_role_recovery_random()
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
    return random_seed;
}

uint32_t bt_aws_mce_role_recovery_generate_random(uint32_t min, uint32_t max, uint32_t resolution)
{
    uint32_t random_step = (max - min)/resolution + 1;

    return (min + (bt_aws_mce_role_recovery_random() % random_step) * resolution);
}

void bt_aws_mce_role_recovery_start_scan_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    uint32_t timeout = 0;

    if(bt_device_manager_aws_local_info_get_real_role() & BT_AWS_MCE_ROLE_AGENT){
        if (bt_aws_mce_role_recovery_is_first_scan) {
            bt_aws_mce_role_recovery_is_first_scan = false;
            timeout = bt_aws_mce_role_recovery_generate_random(BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_DUR_MIN,
                        BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_DUR_MAX, BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_RESOLUTION);
        } else {
            timeout = bt_aws_mce_role_recovery_generate_random(BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_DUR_MIN,
                        BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_DUR_MAX, 100);
        }
    } else {
        timeout = bt_aws_mce_role_recovery_generate_random(BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_DUR_MIN,
                    BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_DUR_MAX, BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_RESOLUTION);
    }
    bt_cmgr_report_id("[BT_CM_SM][I] start scan timer: %d ms", 1, timeout);

    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_AWS_MCE_ROLE_RECOVERY_TIMER_PARTNER_TO_AGENT,
                    timeout, bt_aws_mce_role_recovery_timeout_callback);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_aws_mce_role_recovery_gpt_start);

#endif

}

void bt_aws_mce_role_recovery_set_reconnect(bool is_allow, bool is_initiate_connection)
{
    bt_cmgr_report_id("[BT_CM_SM][I] bt_aws_mce_role_recovery_set_reconnect: %d, is_initiate_connection: 0x%x", 2, is_allow, is_initiate_connection);
    if (is_allow) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_ENABLE);
    } else {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_DISABLE);
    }
    bt_cm_reconn_is_allow(is_allow, is_initiate_connection);
}

void bt_aws_mce_role_recovery_stop_scan_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop scan timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif

}

void bt_aws_mce_role_recovery_start_agent_change_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
/*
    uint32_t timeout = 0;
    if(bt_device_manager_aws_local_info_get_real_role() & BT_AWS_MCE_ROLE_AGENT){
        timeout = bt_aws_mce_role_recovery_generate_random(BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_DUR_MIN,
                    BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_DUR_MAX, BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_RESOLUTION);
    } else {
        timeout = bt_aws_mce_role_recovery_generate_random(BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_DUR_MIN,
                    BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_DUR_MAX, BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_RESOLUTION);
    }
    bt_cmgr_report_id("[BT_CM_SM][I] start agent change timer: %d ms", 1, timeout);
*/
    bt_cmgr_report_id("[BT_CM_SM][I] start agent change timer: 8000 ms", 0);
    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_AWS_MCE_ROLE_RECOVERY_TIMER_AGENT_TO_PARTNER,
                8000, bt_aws_mce_role_recovery_timeout_callback);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_aws_mce_role_recovery_gpt_start);

#endif

}

void bt_aws_mce_role_recovery_stop_agent_change_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop agent change timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif

}

void bt_aws_mce_role_recovery_async_timeout_callback(uint32_t timer_id, uint32_t data)
{
    bt_aws_mce_role_t dest_role;
    if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
        dest_role = BT_AWS_MCE_ROLE_PARTNER;
    } else {
        dest_role = BT_AWS_MCE_ROLE_AGENT;
    }
    if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
        bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_SWITCH);
        if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(dest_role)) {
            bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
        }
    }
    bt_cm_reconn_is_allow(true, true);
}

void bt_aws_mce_role_recovery_start_link_conflict_timer()
{
    /* connection conflick wait timer */
    bt_cmgr_report_id("[BT_CM_SM][I] start link conflict waitting timer", 0);
    bt_cm_reconn_is_allow(false, false);
    bt_timer_ext_start(BT_SINK_SRV_CM_STATE_EXT_ASYNC_TIMER_ID, 0,
                    1500, bt_aws_mce_role_recovery_async_timeout_callback);
}

void bt_aws_mce_role_recovery_stop_link_conflict_timer()
{
    bt_cmgr_report_id("[BT_CM_SM][I] stop link conflict waitting timer", 0);
    bt_cm_reconn_is_allow(true, true);
    bt_timer_ext_stop(BT_SINK_SRV_CM_STATE_EXT_ASYNC_TIMER_ID);
}

void bt_aws_mce_role_recovery_start_partner_sync_agent_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] start partner wait for agent sync timer: %d ms", 1, BT_AWS_MCE_ROLE_RECOVERY_WAIT_SYNC_INFO_TIMER);

    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_AWS_MCE_ROLE_RECOVERY_TIMER_WAIT_SYNC_INFO,
                BT_AWS_MCE_ROLE_RECOVERY_WAIT_SYNC_INFO_TIMER, bt_aws_mce_role_recovery_timeout_callback);
#endif

}

void bt_aws_mce_role_recovery_stop_partner_sync_agent_timer()
{
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_cmgr_report_id("[BT_CM_SM][I] stop partner wait for agent sync timer", 0);
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
#endif

}

uint8_t bt_aws_mce_role_recovery_is_partner_connected_with_agent()
{
    if(bt_cm_get_connected_profile_services(*(bt_device_manager_aws_local_info_get_peer_address())) & BT_CM_PROFILE_SERVICE_AWS){
        return 1;
    } else {
        return 0;
    }
}

uint8_t bt_aws_mce_role_recovery_is_agent_connected_with_partner()
{
    bt_aws_mce_agent_state_type_t agent_state = bt_sink_srv_cm_get_aws_link_state();
    if(agent_state == BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        return 1;
    } else {
        return 0;
    }
}

uint8_t bt_aws_mce_role_recovery_agent_connected_with_sp()
{
    bt_bd_addr_t addr_list = {0};
    uint32_t connect_devie_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, &addr_list, 1);

    return connect_devie_num;
}

void bt_aws_mce_role_recovery_action_handler(bt_cm_event_t event, void *param)
{
    const bt_aws_mce_role_recovery_state_machine_item_t *active_state = bt_aws_mce_role_recovery_get_active_state();
    if (active_state && active_state->misc_fun) {
        switch(event){
            case BT_CM_EVENT_PRE_CONNECT: {
                /*start to connect, need  to start role change timer*/
                if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                    if(bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT){
                        //bt_cmgr_report_id("[BT_CM_SM][I] agent connect sp", 0);
                        bt_bd_addr_t *addr = (bt_bd_addr_t *)param;
                        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
                        if (memcmp(addr, local_addr, sizeof(bt_bd_addr_t))) {
                            active_state->misc_fun(BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_CONN_SP, NULL);
                        }
                    } else {
                        //bt_cmgr_report_id("[BT_CM_SM][I] partner connect agent", 0);
                        active_state->misc_fun(BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_CONN_AGENT, NULL);
                    }
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
uint8_t bt_aws_mce_role_recovery_is_aws_peer_addr_empty()
{
    bt_bd_addr_t empty_addr = {0};
    bt_bd_addr_t *remote_addr = bt_device_manager_aws_local_info_get_peer_address();

    if (!memcmp(remote_addr, empty_addr, sizeof(bt_bd_addr_t))) {
        bt_cmgr_report_id("[BT_CM_SM][E] Error! >>>>>>>>>>>>[Attention] AWS peer addr is empty, please config it first for earbuds working well!!!!!!<<<<<<<<<<<<<<<", 0);
        return 1;
    }

    return 0;
}

void  bt_aws_mce_role_recovery_handle_state_switching(void)
{
    bt_aws_mce_role_recovery_state_t from = BT_AWS_MCE_ROLE_RECOVERY_STATE_NONE;
    uint32_t reason = bt_aws_mce_role_recovery_state_machine_ctx.switch_reason;
    void *data = bt_aws_mce_role_recovery_state_machine_ctx.switch_data;
    bt_aws_mce_role_recovery_state_t to = bt_aws_mce_role_recovery_state_machine_ctx.goto_state;
    bt_aws_mce_role_recovery_state_machine_ctx.goto_state = BT_AWS_MCE_ROLE_RECOVERY_STATE_NONE;
    if (bt_aws_mce_role_recovery_state_machine_ctx.active_state) {
        from = bt_connection_manager_state_machine_get_state();
        if (bt_aws_mce_role_recovery_state_machine_ctx.active_state->on_exit) {
            bt_aws_mce_role_recovery_state_machine_ctx.active_state->on_exit(from, reason, data);
        }
    }
    bt_aws_mce_role_recovery_state_machine_ctx.active_state = bt_aws_mce_role_recovery_state_machine_table + to;
    bt_cmgr_report_id("[BT_CM_SM][I]bt_aws_mce_role_recovery_handle_state_switching(%d -> %d) %d", 3, from, to, reason);
    bt_aws_mce_role_recovery_state_machine_ctx.active_state->on_enter(from, reason, data);
}

void bt_aws_mce_role_recovery_state_defualt_on_enter(bt_aws_mce_role_recovery_state_t from, uint32_t reason, void *user_data)
{

}

void bt_aws_mce_role_recovery_state_defualt_on_exit(bt_aws_mce_role_recovery_state_t state, uint32_t reason, void *user_data)
{

}

bt_status_t bt_aws_mce_role_recovery_state_defualt_bt_func(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    //bt_cmgr_report_id("[BT_CM_SM][W]bt_aws_mce_role_recovery_state_defualt_bt_func, msg(0x%08x), status(0x%08x), buffer(0x%08x)", 3, msg, status, buffer);
    return BT_STATUS_SUCCESS;

}

void bt_aws_mce_role_recovery_state_defualt_sink_func(bt_cm_event_t event_id, void *params)
{
    //bt_cmgr_report_id("[BT_CM_SM][W]bt_connection_manager_state_ext_in_case_defualt_fun, event_id(0x%08x), params(0x%08x)", 2, event_id, params);
}

void bt_aws_mce_role_recovery_state_in_case_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params)
{
    switch (type) {
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_OUT_CASE: {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if(role == BT_AWS_MCE_ROLE_AGENT){
                if(bt_aws_mce_role_recovery_is_agent_connected_with_partner()) {
                    if(bt_aws_mce_role_recovery_agent_connected_with_sp()) {
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT, 0, NULL);
                    } else {
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT, 0, NULL);
                    }
                } else {
                    if(bt_aws_mce_role_recovery_agent_connected_with_sp()) {
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS, 0, NULL);
                    } else {
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
                    }
                }
            } else {
                if(bt_aws_mce_role_recovery_is_partner_connected_with_agent()){
                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER, 0, NULL);
                } else {
                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN, 0, NULL);
                    bt_aws_mce_role_recovery_start_scan_timer();
                }
            }
            break;

        }
        default:
            break;
    }
}

void bt_aws_mce_role_recovery_state_standby_on_exit(bt_aws_mce_role_recovery_state_t state, uint32_t reason, void *user_data)
{

}

void bt_aws_mce_role_recovery_state_standby_sink_func(bt_cm_event_t event_id, void *params)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_role_recovery_reinit_type_t reinit = bt_aws_mce_role_recovery_get_reinit_flag();
    bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)params;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* aws profile connected */
            if (!(event->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && (event->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                if ((role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER)) {
                    /* partner attach to agent*/
                    bt_bd_addr_t *agent_addr = bt_device_manager_aws_local_info_get_peer_address();
                    if (!memcmp(&(event->address), agent_addr, sizeof(bt_bd_addr_t))) {
                        bt_cmgr_report_id("[BT_CM_SM][I] standby sink fun: partner attach to agent", 0);
                        /* don't switch state during role recovery is on going */
                        if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == reinit) {
                            bt_aws_mce_role_recovery_stop_standby_timer();
                            /* mt2822 no aws sync packet */
                            //bt_aws_mce_role_recovery_start_partner_sync_agent_timer();
                            bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER, 0, NULL);
                        } else {
                            bt_cmgr_report_id("[BT_CM_SM][I] standby state: is during role switch, ignore", 0);
                        }
                    }
                } else if (role == BT_AWS_MCE_ROLE_AGENT) {
                    bt_cmgr_report_id("[BT_CM_SM][I] standby sink fun: partner is connected", 0);
                    /* don't switch state during role recovery is on going */
                    if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == reinit) {
                        bt_aws_mce_role_recovery_stop_standby_timer();
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT, 0, NULL);
                        /*start reconnect SP*/
                        bt_aws_mce_role_recovery_set_reconnect(true, true);
                    } else {
                        bt_cmgr_report_id("[BT_CM_SM][I] standby state: is during role switch, ignore", 0);
                    }
                }
            } else {
                bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
                if (BT_CM_ACL_LINK_CONNECTED == event->acl_state
                    && BT_CM_ACL_LINK_CONNECTED != event->pre_acl_state
                    && memcmp(&(event->address), local_addr, sizeof(bt_bd_addr_t))) {
                    if ((role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER)) {
                        bt_cmgr_report_id("[BT_CM_SM][I] standby state: sp connected, current role is partner, skip", 0);
                    } else {
                        bt_cmgr_report_id("[BT_CM_SM][I] standby state: sp connected", 0);
                        /* don't switch state during role recovery is on going */
                        if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == reinit) {
                            bt_aws_mce_role_recovery_stop_standby_timer();
                            /* enable page scan after sp connected */
                            bt_aws_mce_role_recovery_set_reconnect(true, false);
                            bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS, 0, NULL);
                        } else {
                            bt_cmgr_report_id("[BT_CM_SM][I] standby state: is during role switch, ignore", 0);
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

void bt_aws_mce_role_recovery_state_standby_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params)
{
    //bt_aws_mce_role_recovery_reinit_type_t reinit = bt_aws_mce_role_recovery_get_reinit_flag();

    switch (type) {
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_STANDBY_TIMEOUT: {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            bt_cmgr_report_id("[BT_CM_SM][I] standby misc fun: role standby time out, role: 0x%x", 1, role);
            if (BT_AWS_MCE_ROLE_NONE == role) {
                bt_cmgr_report_id("[BT_CM_SM][E] local role is None", 0);
                return;
            }

            if (role & BT_AWS_MCE_ROLE_AGENT) {
                if ((bt_device_manager_get_paired_number() != 0) || (0x01 == AGENT_KEEP_ROLE_RECOVER)) {
                    if (BT_AWS_MCE_ROLE_RECOVERY_LOCK == bt_aws_mce_role_recovery_get_lock_state()) {
                        /* role recovert was disallow, then switch to REC_LS for agent role power on */
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
                    } else {
                        if (!bt_aws_mce_role_recovery_is_aws_peer_addr_empty()) {
                            bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_SWITCH);
                            if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_PARTNER)) {
                                bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                            }
                        }
                        bt_aws_mce_role_recovery_is_first_scan = true;
                    }
                    bt_aws_mce_role_recovery_set_reconnect(true,false);
                } else {
                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
                }
            } else {
                if (bt_device_manager_get_paired_number() != 0){
                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN, 0, NULL);
                    bt_aws_mce_role_recovery_start_scan_timer();
                } else {
                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN, 0, NULL);
                }
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_CHANGE: {
            bt_aws_mce_srv_switch_role_complete_ind_t *role_update = (bt_aws_mce_srv_switch_role_complete_ind_t *)params;
            bt_cmgr_report_id("[BT_CM_SM][I] standby misc fun: role change: 0x%x", 1, role_update->cur_aws_role);

            bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE);
            if (BT_AWS_MCE_ROLE_AGENT == role_update->cur_aws_role) {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
            } else {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN, 0, NULL);
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_OFF: {
            bt_cmgr_report_id("[BT_CM_SM][I] standby state: bt power off", 0);
            bt_aws_mce_role_recovery_set_reconnect(true, false);
            /* user request power off, we should return to standy state */
            bt_aws_mce_role_recovery_restart();
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_ON: {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            bt_cmgr_report_id("[BT_CM_SM][I] standby state: bt power on, role: 0x%x", 1, role);
            /* power on, start standby timer */
            bt_aws_mce_role_recovery_start_standby_timer();
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_real_role()
                && !bt_aws_mce_role_recovery_is_aws_peer_addr_empty()) {
                bt_aws_mce_role_recovery_set_reconnect(false, false);
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC_TIMEOUT: {
            if(bt_device_manager_get_paired_number() != 0
                && !bt_aws_mce_role_recovery_is_aws_peer_addr_empty()
                && BT_AWS_MCE_ROLE_RECOVERY_UNLOCK == bt_aws_mce_role_recovery_get_lock_state()){
                bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_SWITCH);
                if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_AGENT)) {
                    bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                }
            } else {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER, 0, NULL);
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC:{
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

            bt_aws_mce_role_recovery_stop_partner_sync_agent_timer();
            if (role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER) {
                bt_cmgr_report_id("[BT_CM_SM][I] standby misc fun: partner receive agent connected sync packet.", 0);
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER, 0, NULL);
            }
            break;
        }
        default:
            break;
    }
}

void bt_aws_mce_role_recovery_state_partner_sink_func(bt_cm_event_t event_id, void *params)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)params;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* aws profile disconnected */
            if (!(event->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && (event->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                if ((role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER)) {
                    bt_cmgr_report_id("[BT_CM_SM][I] partner sink fun: agent lost", 0);
                    bt_bd_addr_t *agent_addr = bt_device_manager_aws_local_info_get_peer_address();
                    if (!memcmp(&(event->address), agent_addr, sizeof(bt_bd_addr_t))) {
                        if (bt_aws_mce_role_recovery_get_reinit_flag() != BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_SWITCH) {
                            bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN, 0, NULL);
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

void bt_aws_mce_role_recovery_state_partner_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params)
{
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    if (type == BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_RHO) {
        bt_aws_mce_role_recovery_misc_func_params_rho_event_t *rho_event = (bt_aws_mce_role_recovery_misc_func_params_rho_event_t *)params;
        if (rho_event->event == BT_ROLE_HANDOVER_COMPLETE_IND) {
            if (rho_event->status == BT_STATUS_SUCCESS) {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT, 0, NULL);
            }  else {
                bt_cmgr_report_id("[BT_CM_SM][I] partner sink fun: rho fail", 0);
            }
        }
    }
#endif
}

void bt_aws_mce_role_recovery_state_reconnect_sink_func(bt_cm_event_t event_id, void *params)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)params;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* aws profile disconnected */
            if ((event->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && !(event->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    bt_cmgr_report_id("[BT_CM_SM][I]reconnect sink fun: partner lost ", 0);
                    if (bt_aws_mce_role_recovery_get_reinit_flag() != BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_SWITCH) {
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
                        bt_aws_mce_role_recovery_start_agent_change_timer();
                    }
                }
            } else {
                bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
                if (BT_CM_ACL_LINK_CONNECTED == event->acl_state
                    && BT_CM_ACL_LINK_CONNECTED != event->pre_acl_state
                    && memcmp(&(event->address), local_addr, sizeof(bt_bd_addr_t))) {
                    bt_cmgr_report_id("[BT_CM_SM][I] reconnect state: sp connected", 0);
                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT, 0, NULL);
                }
            }
            break;
        }
        default:
            break;
    }
}

void bt_aws_mce_role_recovery_state_reconnect_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params)
{
    switch (type) {
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_OFF: {
            /* user request power off, we should return to standy state */
            bt_aws_mce_role_recovery_restart();
            break;
        }
        default:
            break;
    }
}

void bt_aws_mce_role_recovery_state_connect_sink_func(bt_cm_event_t event_id, void *params)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)params;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* aws profile disconnected */
            if ((event->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && !(event->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    /* partner lost*/
                    bt_cmgr_report_id("[BT_CM_SM][I]connect sink fun: partner lost ", 0);
                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS, 0, NULL);
                    /* start a timer to check SP whether disconnect during 3000ms, if yes, then agent wait 5120ms to reconnect SP */
                    bt_aws_mce_role_recovery_start_check_link_timer();
                }
            } else {
                bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
                if (BT_CM_ACL_LINK_DISCONNECTED == event->acl_state
                    && event->pre_acl_state != BT_CM_ACL_LINK_DISCONNECTED
                    && memcmp(&(event->address), local_addr, sizeof(bt_bd_addr_t))) {
                    bt_cmgr_report_id("[BT_CM_SM][I] connect sink fun: sp disconnect agent.", 0);

                    uint32_t connect_devie_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, NULL, 0);
                    bt_cmgr_report_id("[BT_CM_SM][E] connect sink fun: connect_devie_num (%d).", 1, connect_devie_num);
                    /* include agent special link */
                    if (connect_devie_num <= 1) {
                        /*phone disconnenct and partner  will also disconnect*/
                        bt_aws_mce_srv_link_type_t state = bt_aws_mce_srv_get_link_type();
                        bt_cmgr_report_id("[BT_CM_SM][E] connect sink fun: aws link state (%d).", 1, state);
                        if (BT_AWS_MCE_SRV_LINK_NONE == state) {
                            bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
                            bt_aws_mce_role_recovery_start_agent_change_timer();
                        } else {
                            /* partner is also connected */
                            bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT, 0, NULL);
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

void bt_aws_mce_role_recovery_state_connect_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params)
{
    if (type == BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_RHO) {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        bt_aws_mce_role_recovery_misc_func_params_rho_event_t *rho_event = (bt_aws_mce_role_recovery_misc_func_params_rho_event_t *)params;
        if (rho_event->event == BT_ROLE_HANDOVER_COMPLETE_IND) {
            if (rho_event->status == BT_STATUS_SUCCESS) {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER, 0, NULL);
            }  else {
                bt_cmgr_report_id("[BT_CM_SM][I] connect misc fun: rho fail", 0);
            }
        }
#endif
    }
}

void bt_aws_mce_role_recovery_state_rec_ls_on_enter(bt_aws_mce_role_recovery_state_t from, uint32_t reason, void *user_data)
{
    /* temp set role recovery timer to 7s */
    bt_timer_ext_stop(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID);
    bt_timer_ext_start(BT_SINK_SRV_CM_AUTO_ROLE_CHANGE_TIMER_ID, BT_AWS_MCE_ROLE_RECOVERY_TIMER_AGENT_TO_PARTNER,
                7000, bt_aws_mce_role_recovery_timeout_callback);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &bt_aws_mce_role_recovery_gpt_start);
    bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state in, start 7000ms timer to check SP connection", 0);

    return;
}


bt_status_t bt_aws_mce_role_recovery_state_rec_ls_bt_func(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
#if 0
    switch (msg) {
        case BT_GAP_LINK_STATUS_UPDATED_IND: {
            /* phone is connected. */
            bt_gap_link_status_updated_ind_t *param = (bt_gap_link_status_updated_ind_t *)buffer;
            bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
            if (BT_GAP_LINK_STATUS_CONNECTED_0 == param->link_status && memcmp(param->address, local_addr, sizeof(bt_bd_addr_t))) {
                bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: sp connected", 0);
                bt_aws_mce_role_recovery_stop_agent_change_timer();
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS, 0, NULL);
            }
            break;
        }
        default:
            break;
    }
#endif
    return BT_STATUS_SUCCESS;
}

void bt_aws_mce_role_recovery_state_rec_ls_sink_func(bt_cm_event_t event_id, void *params)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_role_recovery_reinit_type_t reinit = bt_aws_mce_role_recovery_get_reinit_flag();
    bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)params;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* aws profile connected */
            if (!(event->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && (event->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    bt_cmgr_report_id("[BT_CM_SM][I]rec_ls sink fun: partner is connected", 0);
                    /* don't switch state during role recovery is on going */
                    if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == reinit) {
                        bt_aws_mce_role_recovery_stop_agent_change_timer();
                        bt_aws_mce_role_recovery_stop_link_conflict_timer();
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT, 0, NULL);
                    } else {
                        bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: is during role switch, ignore", 0);
                    }
                }
            } else {
                bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
                if (BT_CM_ACL_LINK_CONNECTED == event->acl_state
                    && BT_CM_ACL_LINK_CONNECTED != event->pre_acl_state
                    && memcmp(&(event->address), local_addr, sizeof(bt_bd_addr_t))) {
                    bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: sp connected", 0);
                    /* don't switch state during role recovery is on going */
                    if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == reinit) {
                        bt_aws_mce_role_recovery_stop_agent_change_timer();
                        bt_aws_mce_role_recovery_stop_link_conflict_timer();
                        bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS, 0, NULL);
                    } else {
                        bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: is during role switch, ignore", 0);
                    }
                } else if (BT_CM_ACL_LINK_DISCONNECTED == event->acl_state
                    && event->pre_acl_state != BT_CM_ACL_LINK_DISCONNECTED
                    && memcmp(&(event->address), local_addr, sizeof(bt_bd_addr_t))) {
                    bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: sp disconnected by reason = 0x%x", 1, event->reason);
                    if (BT_HCI_STATUS_VENDOR_REMOTE_CONNECTION_EXIST == event->reason) {
                        bt_cmgr_report_id("[BT_CM_SM][I]rec_ls sink fun: connection already existed", 0);
                        bt_aws_mce_role_recovery_stop_agent_change_timer();
                        /* if role recovert was lock, nothing to do. */
                        if (BT_AWS_MCE_ROLE_RECOVERY_UNLOCK == bt_aws_mce_role_recovery_get_lock_state()) {
                            if (!bt_aws_mce_role_recovery_is_aws_peer_addr_empty()) {
                                bt_aws_mce_role_recovery_start_link_conflict_timer();
                            }
                        }
                    } else if (BT_HCI_STATUS_PAGE_TIMEOUT == event->reason
                                || BT_HCI_STATUS_CONNECTION_TIMEOUT == event->reason) {
                        bt_cmgr_report_id("[BT_CM_SM][I]rec_ls sink fun: page sp timeout, will be do role recovery", 0);
                        bt_aws_mce_role_recovery_stop_agent_change_timer();
                        /* switch role due to page timeout */
                        uint32_t paired_num = bt_device_manager_get_paired_number();
                        bt_cmgr_report_id("[BT_CM_SM][I] rec_ls misc fun: paired number(%d)", 1, paired_num);
                        bt_aws_mce_role_t real_role = bt_device_manager_aws_local_info_get_real_role();
                        if (BT_AWS_MCE_ROLE_RECOVERY_UNLOCK == bt_aws_mce_role_recovery_get_lock_state()) {
                            if (paired_num && !bt_aws_mce_role_recovery_is_aws_peer_addr_empty()) {
                                if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                                    bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_REC_LS_TIMEOUT);
                                    if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_PARTNER)) {
                                        bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                                    }
                                }
                            } else if ((real_role & BT_AWS_MCE_ROLE_AGENT) && (0x01 == AGENT_KEEP_ROLE_RECOVER)) {
                                if (!bt_aws_mce_role_recovery_is_aws_peer_addr_empty()) {
                                    if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                                        bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_REC_LS_TIMEOUT);
                                        if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_PARTNER)) {
                                            bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                                        }
                                    }
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
}

void bt_aws_mce_role_recovery_state_rec_ls_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params)
{
    //bt_aws_mce_role_recovery_reinit_type_t reinit = bt_aws_mce_role_recovery_get_reinit_flag();

    switch (type) {
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_REC_LS_TIMEOUT: {
            bt_cmgr_report_id("[BT_CM_SM][I] rec_ls misc fun: connect parnter/sp time out", 0);
            uint32_t paired_num = bt_device_manager_get_paired_number();
            bt_cmgr_report_id("[BT_CM_SM][I] rec_ls misc fun: paired number(%d)", 1, paired_num);
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();

            if (BT_AWS_MCE_ROLE_RECOVERY_UNLOCK == bt_aws_mce_role_recovery_get_lock_state()) {
                if (paired_num && !bt_aws_mce_role_recovery_is_aws_peer_addr_empty()) {
                    if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                        bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_REC_LS_TIMEOUT);
                        if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_PARTNER)) {
                            bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                        }
                    }
                } else if ((role & BT_AWS_MCE_ROLE_AGENT) && (0x01 == AGENT_KEEP_ROLE_RECOVER)) {
                    if (!bt_aws_mce_role_recovery_is_aws_peer_addr_empty()) {
                        if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                            bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_REC_LS_TIMEOUT);
                            if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_PARTNER)) {
                                bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                            }
                        }
                    }
                }
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_CHANGE: {
            bt_aws_mce_srv_switch_role_complete_ind_t *role_update = (bt_aws_mce_srv_switch_role_complete_ind_t *)params;
            bt_cmgr_report_id("[BT_CM_SM][I] rec_ls misc fun: role change: 0x%x", 1, role_update->cur_aws_role);

            bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE);
            if (BT_AWS_MCE_ROLE_AGENT == role_update->cur_aws_role) {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
            } else {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN, 0, NULL);
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_CONN_SP: {
            bt_cmgr_report_id("[BT_CM_SM][I] rec_ls misc fun: agent connect sp", 0);
            bt_aws_mce_role_recovery_start_agent_change_timer();
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_OFF: {
            bt_cmgr_report_id("[BT_CM_SM][I] rec_ls state: bt power off", 0);
            bt_aws_mce_role_recovery_stop_link_conflict_timer();
            /* user request power off, we should return to standy state */
            bt_aws_mce_role_recovery_restart();
            break;
        }
        default:
            break;
    }
}

void bt_aws_mce_role_recovery_state_conn_ls_sink_func(bt_cm_event_t event_id, void *params)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)params;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* aws profile connected */
            if (!(event->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && (event->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    bt_cmgr_report_id("[BT_CM_SM][I]conn ls sink fun: partner is connected", 0);
                    bt_aws_mce_role_recovery_stop_check_link_timer();
                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT, 0, NULL);
                }
            } else {
                bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
                if (BT_CM_ACL_LINK_DISCONNECTED == event->acl_state
                    && event->pre_acl_state != BT_CM_ACL_LINK_DISCONNECTED
                    && memcmp(&(event->address), local_addr, sizeof(bt_bd_addr_t))) {
                    bt_cmgr_report_id("[BT_CM_SM][I] conn_ls state: sp disconnected, error (0x%x)", 1, event->reason);
                    uint32_t connect_devie_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, NULL, 0);
                    bt_cmgr_report_id("[BT_CM_SM][E] conn_ls sink fun: connect_devie_num (%d).", 1, connect_devie_num);
                    /* include agent special link */
                    if (connect_devie_num <= 1) {
                        if(BT_HCI_STATUS_CONNECTION_TERMINATED_BY_LOCAL_HOST == event->reason || BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION == event->reason
                            || (BT_HCI_STATUS_PIN_OR_KEY_MISSING == event->reason && (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_real_role()))) {
                            bt_aws_mce_role_recovery_stop_check_link_timer();
                            bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
                        } else {
                            if (BT_AWS_MCE_ROLE_RECOVERY_UNLOCK == bt_aws_mce_role_recovery_get_lock_state()) {
                                if (bt_aws_mce_role_recovery_is_check_link_timer_active() && BT_HCI_STATUS_VENDOR_REMOTE_CONNECTION_EXIST != event->reason) {
                                    bt_aws_mce_role_recovery_stop_check_link_timer();
                                    bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
                                    bt_aws_mce_role_recovery_start_agent_change_timer();
                                } else {
                                    if(!bt_aws_mce_role_recovery_is_aws_peer_addr_empty()){
                                        if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                                            bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_SWITCH);
                                            if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_PARTNER)) {
                                                bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                                            }
                                        }
                                    }
                                 }
                            } else {
                                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
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
}

void bt_aws_mce_role_recovery_state_conn_ls_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params)
{
    //bt_aws_mce_role_recovery_reinit_type_t reinit = bt_aws_mce_role_recovery_get_reinit_flag();
    switch (type) {
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_OFF: {
            bt_cmgr_report_id("[BT_CM_SM][I] conn_ls state: bt power off", 0);
            /* user request power off, we should return to standy state */
            bt_aws_mce_role_recovery_restart();
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_CHANGE: {
            bt_aws_mce_srv_switch_role_complete_ind_t *role_update = (bt_aws_mce_srv_switch_role_complete_ind_t *)params;
            bt_cmgr_report_id("[BT_CM_SM][I] standby misc fun: role change: 0x%x", 1, role_update->cur_aws_role);

            bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE);
            if (BT_AWS_MCE_ROLE_AGENT == role_update->cur_aws_role) {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
            } else {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN, 0, NULL);
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_CHECK_LINK_TIMEOUT: {
            bt_cmgr_report_id("[BT_CM_SM][I] conn_ls state: check link timeout", 0);
            break;
        }
        default:
            break;
    }
}

bt_status_t bt_aws_mce_role_recovery_state_scan_bt_func(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    switch (msg) {

        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}


void bt_aws_mce_role_recovery_state_scan_sink_func(bt_cm_event_t event_id, void *params)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_role_recovery_reinit_type_t reinit = bt_aws_mce_role_recovery_get_reinit_flag();
    bt_cm_remote_info_update_ind_t *event = (bt_cm_remote_info_update_ind_t *)params;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* aws profile connected */
            if (!(event->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && (event->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                if ((role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER)) {
                    bt_bd_addr_t *agent_addr = bt_device_manager_aws_local_info_get_peer_address();
                    if (!memcmp(&(event->address), agent_addr, sizeof(bt_bd_addr_t))) {
                        bt_cmgr_report_id("[BT_CM_SM][I] scan sink fun: partner attach to agent", 0);
                        /* don't switch state during role recovery is on going */
                        if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == reinit) {
                            bt_aws_mce_role_recovery_stop_scan_timer();
                            /* mt2822 no aws sync packet */
                            //bt_aws_mce_role_recovery_start_partner_sync_agent_timer();
                            bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER, 0, NULL);
                        } else {
                            bt_cmgr_report_id("[BT_CM_SM][I] scan state: is during role switch, ignore", 0);
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

void bt_aws_mce_role_recovery_state_scan_misc_func(bt_aws_mce_role_recovery_misc_func_type_t type, void *params)
{
    //bt_aws_mce_role_recovery_reinit_type_t reinit = bt_aws_mce_role_recovery_get_reinit_flag();
    switch (type) {
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_SCAN_TIMEOUT: {
            bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: connect agent time out", 0);
            uint32_t paired_num = bt_device_manager_get_paired_number();
            bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: paired number(%d)", 1, paired_num);
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();

            if (BT_AWS_MCE_ROLE_RECOVERY_UNLOCK == bt_aws_mce_role_recovery_get_lock_state()) {
                if (!bt_aws_mce_role_recovery_is_aws_peer_addr_empty() && paired_num) {
                    if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                        bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_SCAN_TIMEOUT);
                        if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_AGENT)) {
                            bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                        }
                    }
                } else if ((role & BT_AWS_MCE_ROLE_AGENT) && (0x01 == AGENT_KEEP_ROLE_RECOVER)) {
                    if (!bt_aws_mce_role_recovery_is_aws_peer_addr_empty()) {
                        if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                            bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_SCAN_TIMEOUT);
                            if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_AGENT)) {
                                bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                            }
                        }
                    }
                }
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_CHANGE: {
            bt_aws_mce_srv_switch_role_complete_ind_t *role_update = (bt_aws_mce_srv_switch_role_complete_ind_t *)params;
            bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: role change: 0x%x", 1, role_update->cur_aws_role);

            bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE);
            if (BT_AWS_MCE_ROLE_AGENT == role_update->cur_aws_role) {
                bt_aws_mce_role_recovery_stop_scan_timer();
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS, 0, NULL);
                uint32_t paired_num = bt_device_manager_get_paired_number();
                bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: paired number(%d)", 1, paired_num);
                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_real_role();
                /* if real role is agent && no paired device, continue do role recover */
                if (!paired_num && (role & BT_AWS_MCE_ROLE_AGENT) && (0x01 == AGENT_KEEP_ROLE_RECOVER)) {
                    bt_aws_mce_role_recovery_start_agent_change_timer();
                }
            } else {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN, 0, NULL);
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_CONN_AGENT: {
            bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: parnter connect agent", 0);
            bt_aws_mce_role_recovery_start_scan_timer();
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_OFF: {
            bt_cmgr_report_id("[BT_CM_SM][I] scan state: bt power off", 0);
            /* user request power off, we should return to standy state */
            bt_aws_mce_role_recovery_restart();
            break;
        }

        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC_TIMEOUT: {
            if(bt_device_manager_get_paired_number() != 0
                && !bt_aws_mce_role_recovery_is_aws_peer_addr_empty()
                && BT_AWS_MCE_ROLE_RECOVERY_UNLOCK == bt_aws_mce_role_recovery_get_lock_state()) {
                if (BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE == bt_aws_mce_role_recovery_get_reinit_flag()) {
                    bt_aws_mce_role_recovery_set_reinit_flag(BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_SWITCH);
                    if (BT_STATUS_SUCCESS != bt_aws_mce_srv_switch_role(BT_AWS_MCE_ROLE_AGENT)) {
                        bt_cmgr_report_id("[BT_CM_SM][E] swich role fail !!!", 0);
                    }
                }
            } else {
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER, 0, NULL);
            }
            break;
        }
        case BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC: {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

            bt_aws_mce_role_recovery_stop_partner_sync_agent_timer();
            if (role & BT_AWS_MCE_ROLE_CLINET || role & BT_AWS_MCE_ROLE_PARTNER) {
                bt_cmgr_report_id("[BT_CM_SM][I] scan misc fun: partner receive agent connected sync packet.", 0);
                bt_aws_mce_role_recovery_state_switch(BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER, 0, NULL);
            }
            break;
        }
        default:
            break;
    }
}
#endif //#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
