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

#ifndef __BT_CONNECTION_MANAGER_STATE_MACHINE_EXT_H__
#define __BT_CONNECTION_MANAGER_STATE_MACHINE_EXT_H__

#include "bt_sink_srv.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif

#define BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_DUR_MIN (1500)   /*ms*/
#define BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_DUR_MAX (2500)   /*ms*/
#define BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_SHORT_TIMER_RESOLUTION (300)  /*ms*/
#define BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_DUR_MIN (3000)      /*ms*/
#define BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_DUR_MAX (4000)      /*ms*/
#define BT_CONNECTION_MANAGER_STATE_EXT_ROLE_CHANGE_LONG_TIMER_RESOLUTION (200)    /*ms*/
#ifdef MTK_SMART_CHARGER_ENABLE
#define BT_CONNECTION_MANAGER_STATE_EXT_OLD_ROLE_STANDBY_TIMER (1000)               /*ms*/
#else
#define BT_CONNECTION_MANAGER_STATE_EXT_OLD_ROLE_STANDBY_TIMER (500)                /*ms*/
#endif
#define BT_CONNECTION_MANAGER_STATE_EXT_PARTNER_WAIT_FOR_AGENT_SYNC_TIMER (1100)     /*ms*/


#define BT_CONNECTION_MANAGER_STATE_EXT_STANDBY       0x00
#define BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT     0x01
#define BT_CONNECTION_MANAGER_STATE_EXT_CONNECT       0x02
#define BT_CONNECTION_MANAGER_STATE_EXT_PARTNER       0x03
#define BT_CONNECTION_MANAGER_STATE_EXT_REC_LS        0x04
#define BT_CONNECTION_MANAGER_STATE_EXT_CON_LS        0x05
#define BT_CONNECTION_MANAGER_STATE_EXT_SCAN          0x06
#define BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE       0x07
#define BT_CONNECTION_MANAGER_STATE_EXT_NONE          0xFF
typedef uint8_t bt_connection_manager_state_ext_t;


#define BT_CONNECTION_MANAGER_STATE_EXT_STANDBY_BIT       0x00000001
#define BT_CONNECTION_MANAGER_STATE_EXT_RECONNECT_BIT     0x00000002
#define BT_CONNECTION_MANAGER_STATE_EXT_CONNECT_BIT       0x00000004
#define BT_CONNECTION_MANAGER_STATE_EXT_PARTNER_BIT       0x00000008
#define BT_CONNECTION_MANAGER_STATE_EXT_REC_LS_BIT        0x00000010
#define BT_CONNECTION_MANAGER_STATE_EXT_CON_LS_BIT        0x00000020
#define BT_CONNECTION_MANAGER_STATE_EXT_SCAN_BIT          0x00000040
#define BT_CONNECTION_MANAGER_STATE_EXT_IN_CASE_BIT       0x00000080
#define BT_CONNECTION_MANAGER_STATE_EXT_ALL_BIT           0xFFFFFFFF
typedef uint32_t bt_connection_manager_state_ext_bit_t;


#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_RACE                  0x01
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_REMOTE_STATE          0x02
#define BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_RHO                  0x03
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_RHO                   0x04
#define BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_LRS                  0x05   /*Speical AWS Link Re-setup*/
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_SCAN_TIME_OUT         0x06
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_REC_LS_TIME_OUT       0x07
#define BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_TAKEOUT_FROM_CASE    0x08
#define BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_PUTIN_TO_CASE        0x09
#define BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_AGENT_CONN_SP        0x0A
#define BT_CONNECTION_MANAGER_MISC_FUNC_ACTION_PARTNER_CONN_AGENT   0x0B
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_ON           0x0C
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_BT_POWER_OFF          0x0D
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_ROLE_CHANGE_MANUALLY  0x0E
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_ROLE_STANDBY_TIMEOUT  0x0F
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC 0x10
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC_TIMEOUT  0x11
#define BT_CONNECTION_MANAGER_MISC_FUNC_EVENT_CHECK_LINK_TIMEOUT  0x12
typedef uint8_t bt_connection_manager_misc_func_type_t;

#define BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_NONE            0x00
#define BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_SWITCH     0x01
#define BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_SCAN_TIMEOUT    0x02
#define BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_REC_LS_TIMEOUT  0x03
#define BT_CONNECTION_MANAGER_STATE_BT_REINIT_BY_ROLE_CHANGE_MANUALLY  0x04
typedef uint8_t bt_connection_manager_state_bt_reinit_type_t;

#define BT_CONNECTION_MANAGER_STATE_EXT_TIMER_PARTNER_TO_AGENT 0x00
#define BT_CONNECTION_MANAGER_STATE_EXT_TIMER_AGENT_TO_PARTNER 0x01
#define BT_CONNECTION_MANAGER_STATE_EXT_TIMER_STANDBY_WITH_OLD_ROLE 0x02
#define BT_CONNECTION_MANAGER_STATE_EXT_TIMER_PARTNER_WAIT_FOR_AGENT_SYNC 0x03
#define BT_CONNECTION_MANAGER_STATE_EXT_TIMER_CHECK_LINK_STATE 0x04
typedef uint8_t bt_connection_manager_state_ext_timer_type_t;

#define BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_UNLOCK 0x00
#define BT_CONNECTION_MANAGER_STATE_EXT_ROLE_RECOVERY_LOCK 0x01
typedef uint8_t bt_connection_manager_state_ext_recovery_lock_state_type_t;

typedef struct {
    bt_connection_manager_state_ext_t state;
} bt_connection_manager_misc_func_params_remote_state_t;

typedef struct {
    bt_bd_addr_t agent_addr;
} bt_connection_manager_misc_func_params_lrs_t;

typedef struct {
    bt_bd_addr_t addr;
} bt_connection_manager_misc_func_params_aws_remote_addr_t;

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
typedef struct {
    bt_role_handover_event_t event;
    bt_status_t status;
} bt_connection_manager_misc_func_params_rho_event_t;
#endif

typedef bt_status_t (*bt_connection_manager_state_machine_bt_event_fun_t)(bt_msg_type_t msg, bt_status_t status, void *buffer);
typedef void (*bt_connection_manager_state_machine_race_event_fun_t)(uint16_t id, uint8_t *payload, uint16_t payload_length);
typedef void (*bt_connection_manager_state_machine_enter_fun_t)(bt_connection_manager_state_ext_t from, uint32_t reason, void *user_data);
typedef void (*bt_connection_manager_state_machine_exit_fun_t)(bt_connection_manager_state_ext_t state, uint32_t reason, void *user_data);
typedef void (*bt_connection_manager_state_machine_sink_event_fun_t)(bt_sink_srv_event_t event_id, void *params);
typedef void (*bt_connection_manager_state_machine_misc_fun_t)(bt_connection_manager_misc_func_type_t type, void *params);


typedef struct {
    uint32_t                     allow_list;
    bt_connection_manager_state_machine_enter_fun_t         on_enter;
    bt_connection_manager_state_machine_exit_fun_t          on_exit;
    bt_connection_manager_state_machine_bt_event_fun_t      bt_fun;
    bt_connection_manager_state_machine_sink_event_fun_t    sink_fun;
    bt_connection_manager_state_machine_misc_fun_t          misc_fun;
} bt_connection_manager_state_ext_item_t;

typedef struct {
    const bt_connection_manager_state_ext_item_t     *active_state;
    bt_connection_manager_state_ext_t                    goto_state;
    uint8_t                          active_disconn;
    uint32_t                         switch_reason;
    void                            *switch_data;
} bt_connection_manager_state_machine_context_t;

void bt_connection_manager_state_ext_init(void);
void bt_connection_manager_state_ext_deinit(void);
void bt_connection_manager_state_ext_force_deinit(void);
int32_t bt_connection_manager_state_ext_switch (bt_connection_manager_state_ext_t to, uint32_t reason, void *user_data);
void bt_connection_manager_state_ext_defualt_on_enter(bt_connection_manager_state_ext_t from, uint32_t reason, void *user_data);
void bt_connection_manager_state_ext_defualt_on_exit(bt_connection_manager_state_ext_t state, uint32_t reason, void *user_data);
bt_status_t bt_connection_manager_state_ext_defualt_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer);
void bt_connection_manager_state_ext_defualt_sink_fun(bt_sink_srv_event_t event_id, void *params);
void bt_connection_manager_state_ext_default_misc_fun(bt_connection_manager_misc_func_type_t type, void *params);
void bt_connection_manager_state_ext_in_case_misc_fun(bt_connection_manager_misc_func_type_t type, void *params);
void bt_connection_manager_state_ext_standby_on_exit(bt_connection_manager_state_ext_t state, uint32_t reason, void *user_data);
bt_status_t bt_connection_manager_state_ext_standby_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer);
void bt_connection_manager_state_ext_standby_sink_fun(bt_sink_srv_event_t event_id, void *params);
void bt_connection_manager_state_ext_standby_misc_fun(bt_connection_manager_misc_func_type_t type, void *params);
void bt_connection_manager_state_ext_partner_sink_fun(bt_sink_srv_event_t event_id, void *params);
void bt_connection_manager_state_ext_partner_misc_fun(bt_connection_manager_misc_func_type_t type, void *params);
bt_status_t bt_connection_manager_state_ext_reconnect_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer);
void bt_connection_manager_state_ext_reconnect_sink_fun(bt_sink_srv_event_t event_id, void *params);
bt_status_t bt_connection_manager_state_ext_connect_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer);
void bt_connection_manager_state_ext_connect_sink_fun(bt_sink_srv_event_t event_id, void *params);
void bt_connection_manager_state_ext_connect_misc_fun(bt_connection_manager_misc_func_type_t type, void *params);
void bt_connection_manager_state_ext_rec_ls_on_enter(bt_connection_manager_state_ext_t from, uint32_t reason, void *user_data);
bt_status_t bt_connection_manager_state_ext_rec_ls_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer);
void bt_connection_manager_state_ext_rec_ls_sink_fun(bt_sink_srv_event_t event_id, void *params);
void bt_connection_manager_state_ext_rec_ls_misc_fun(bt_connection_manager_misc_func_type_t type, void *params);
bt_status_t bt_connection_manager_state_ext_conn_ls_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer);
void bt_connection_manager_state_ext_conn_ls_sink_fun(bt_sink_srv_event_t event_id, void *params);
void bt_connection_manager_state_ext_conn_ls_misc_fun(bt_connection_manager_misc_func_type_t type, void *params);
bt_status_t bt_connection_manager_state_ext_scan_bt_fun(bt_msg_type_t msg, bt_status_t status, void *buffer);
void bt_connection_manager_state_ext_scan_sink_fun(bt_sink_srv_event_t event_id, void *params);
void bt_connection_manager_state_ext_scan_misc_fun(bt_connection_manager_misc_func_type_t type, void *params);
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
void bt_connection_manager_state_machine_ext_rho_event_notify(bt_role_handover_event_t event, bt_status_t status);
#endif
void bt_connection_manager_state_machine_ext_sink_event_callback(bt_sink_srv_event_t event_id, void *param, uint32_t param_len);
void bt_connection_manager_state_machine_ext_action_handler(bt_sink_srv_action_t action, void *param);

void bt_connection_manager_state_machine_ext_power_onoff_notify(uint8_t is_on);
void bt_connection_manager_state_machine_ext_stop_role_change();
void bt_connection_manager_state_machine_ext_trigger_role_change(bt_aws_mce_role_t role);
uint8_t bt_connection_manager_state_machine_ext_is_in_role_resetup();
void bt_connection_manager_state_machine_ext_cancel_role_resetup();
void bt_connection_manager_state_machine_ext_agent_sync_notify();

void bt_connection_manager_state_machine_ext_role_recovery_lock(void);
void bt_connection_manager_state_machine_ext_role_recovery_unlock(void);
bt_connection_manager_state_ext_recovery_lock_state_type_t bt_connection_manager_state_machine_ext_get_lock_state(void);

#endif //__BT_CONNECTION_MANAGER_STATE_MACHINE_EXT_H__
#endif //#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE

