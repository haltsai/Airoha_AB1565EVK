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

#ifndef __BT_AWS_MCE_ROLE_RECOVERY_H__
#define __BT_AWS_MCE_ROLE_RECOVERY_H__

#include "bt_sink_srv.h"

#define BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_DUR_MIN (1000)   /*ms*/
#define BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_DUR_MAX (2000)   /*ms*/
#define BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_SHORT_TIMER_RESOLUTION (300)  /*ms*/
#define BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_DUR_MIN (3000)      /*ms*/
#define BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_DUR_MAX (4000)      /*ms*/
#define BT_AWS_MCE_ROLE_RECOVERY_ROLE_CHANGE_LONG_TIMER_RESOLUTION (200)    /*ms*/
#ifdef MTK_SMART_CHARGER_ENABLE
#define BT_AWS_MCE_ROLE_RECOVERY_STANDBY_STATE_TIMER (1000)               /*ms*/
#else
#define BT_AWS_MCE_ROLE_RECOVERY_STANDBY_STATE_TIMER (500)                /*ms*/
#endif
#define BT_AWS_MCE_ROLE_RECOVERY_WAIT_SYNC_INFO_TIMER (500)     /*ms*/


#define BT_AWS_MCE_ROLE_RECOVERY_STATE_STANDBY       0x00
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT     0x01
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT       0x02
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER       0x03
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS        0x04
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS        0x05
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN          0x06
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE       0x07
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_NONE          0xFF
typedef uint8_t bt_aws_mce_role_recovery_state_t;


#define BT_AWS_MCE_ROLE_RECOVERY_STATE_STANDBY_BIT       0x00000001
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_RECONNECT_BIT     0x00000002
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_CONNECT_BIT       0x00000004
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_PARTNER_BIT       0x00000008
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_REC_LS_BIT        0x00000010
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_CON_LS_BIT        0x00000020
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_SCAN_BIT          0x00000040
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_IN_CASE_BIT       0x00000080
#define BT_AWS_MCE_ROLE_RECOVERY_STATE_ALL_BIT           0xFFFFFFFF
typedef uint32_t bt_aws_mce_role_recovery_state_bit_t;


#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_RACE                  0x01
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_REMOTE_STATE          0x02
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_RHO                  0x03
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_RHO                   0x04
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_LRS                  0x05   /*Speical AWS Link Re-setup*/
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_SCAN_TIMEOUT         0x06
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_REC_LS_TIMEOUT       0x07
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_OUT_CASE    0x08
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_IN_CASE        0x09
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_CONN_SP        0x0A
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_CONN_AGENT   0x0B
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_ON           0x0C
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_BT_POWER_OFF          0x0D
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_ACTION_ROLE_CHANGE  0x0E
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_STANDBY_TIMEOUT  0x0F
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC 0x10
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_PARTNER_RECEIVE_AGENT_SYNC_TIMEOUT  0x11
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_CHECK_LINK_TIMEOUT  0x12
#define BT_AWS_MCE_ROLE_RECOVERY_MISC_FUNC_EVENT_ROLE_CHANGE  0x13
typedef uint8_t bt_aws_mce_role_recovery_misc_func_type_t;

#define BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_NONE            0x00
#define BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_SWITCH     0x01
#define BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_SCAN_TIMEOUT    0x02
#define BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_REC_LS_TIMEOUT  0x03
#define BT_AWS_MCE_ROLE_RECOVERY_REINIT_BY_ROLE_CHANGE_MANUALLY  0x04
typedef uint8_t bt_aws_mce_role_recovery_reinit_type_t;

#define BT_AWS_MCE_ROLE_RECOVERY_TIMER_PARTNER_TO_AGENT 0x00
#define BT_AWS_MCE_ROLE_RECOVERY_TIMER_AGENT_TO_PARTNER 0x01
#define BT_AWS_MCE_ROLE_RECOVERY_TIMER_STANDBY_WITH_OLD_ROLE 0x02
#define BT_AWS_MCE_ROLE_RECOVERY_TIMER_WAIT_SYNC_INFO 0x03
#define BT_AWS_MCE_ROLE_RECOVERY_TIMER_CHECK_LINK_STATE 0x04
typedef uint8_t bt_aws_mce_role_recovery_state_timer_type_t;

#define BT_AWS_MCE_ROLE_RECOVERY_UNLOCK 0x00
#define BT_AWS_MCE_ROLE_RECOVERY_LOCK 0x01
typedef uint8_t bt_aws_mce_role_recovery_lock_state_type_t;


typedef struct {
    bt_aws_mce_role_recovery_state_t state;
} bt_aws_mce_role_recovery_misc_func_params_remote_state_t;

typedef struct {
    bt_bd_addr_t agent_addr;
} bt_aws_mce_role_recovery_misc_func_params_lrs_t;

typedef struct {
    bt_bd_addr_t addr;
} bt_aws_mce_role_recovery_misc_func_params_aws_remote_addr_t;

typedef bt_status_t (*bt_aws_mce_role_recovery_state_machine_bt_event_fun_t)(bt_msg_type_t msg, bt_status_t status, void *buffer);
typedef void (*bt_aws_mce_role_recovery_state_machine_race_event_fun_t)(uint16_t id, uint8_t *payload, uint16_t payload_length);
typedef void (*bt_aws_mce_role_recovery_state_machine_enter_fun_t)(bt_aws_mce_role_recovery_state_t from, uint32_t reason, void *user_data);
typedef void (*bt_aws_mce_role_recovery_state_machine_exit_fun_t)(bt_aws_mce_role_recovery_state_t state, uint32_t reason, void *user_data);
typedef void (*bt_aws_mce_role_recovery_state_machine_sink_event_fun_t)(bt_cm_event_t event_id, void *params);
typedef void (*bt_aws_mce_role_recovery_state_machine_misc_fun_t)(bt_aws_mce_role_recovery_misc_func_type_t type, void *params);


typedef struct {
    uint32_t                     allow_list;
    bt_aws_mce_role_recovery_state_machine_enter_fun_t         on_enter;
    bt_aws_mce_role_recovery_state_machine_exit_fun_t          on_exit;
    bt_aws_mce_role_recovery_state_machine_bt_event_fun_t      bt_fun;
    bt_aws_mce_role_recovery_state_machine_sink_event_fun_t    sink_fun;
    bt_aws_mce_role_recovery_state_machine_misc_fun_t          misc_fun;
} bt_aws_mce_role_recovery_state_machine_item_t;

typedef struct {
    const bt_aws_mce_role_recovery_state_machine_item_t     *active_state;
    bt_aws_mce_role_recovery_state_t                    goto_state;
    uint8_t                          active_disconn;
    uint32_t                         switch_reason;
    void                            *switch_data;
} bt_aws_mce_role_recovery_state_machine_context_t;

void bt_aws_mce_role_recovery_init(void);
void bt_aws_mce_role_recovery_lock(void);
void bt_aws_mce_role_recovery_unlock(void);
bt_aws_mce_role_recovery_lock_state_type_t bt_aws_mce_role_recovery_get_lock_state(void);
uint8_t bt_aws_mce_role_recovery_is_in_role_resetup();
#endif //__BT_AWS_MCE_ROLE_RECOVERY_H__
#endif //#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE

