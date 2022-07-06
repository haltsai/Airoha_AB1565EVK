/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include "bt_type.h"
#include "bt_debug.h"
#include "bt_gap.h"
#include "bt_gap_le.h"
#include "bt_system.h"
#include "bt_os_layer_api.h"
#include "bt_device_manager.h"
#include "bt_device_manager_power.h"
#include "bt_device_manager_internal.h"

#define BT_DM_CB_MAXIMUM_NUM            (10)

#define BT_DM_POWER_FLAG_RESET_PENDING  (0x01)
#define BT_DM_POWER_FLAG_RESET_PHASE_1  (0x02)
#define BT_DM_POWER_FLAG_RESET_PHASE_2  (0x04)
#define BT_DM_POWER_FLAG_HOLD_MODE      (0x08)
typedef uint8_t bt_dm_power_flag_t;

typedef struct {
    bt_device_manager_power_state_t target_state;
    bt_device_manager_power_state_t state;
    bt_dm_power_flag_t              reset_flag;
    bt_device_manager_power_callback_t  cb;
} bt_dm_power_dev_t;

typedef struct {
    bt_device_manager_power_state_t state;
    bt_dm_power_dev_t               dev[2];
    bt_device_manager_power_callback_t  cb_set[BT_DM_CB_MAXIMUM_NUM];
    bt_dm_power_flag_t              reset_flag;
    bt_device_manager_power_reset_t reset_type;
    void                            *reset_user_data;
    bt_device_manager_power_reset_callback_t    reset_cb;
} bt_dm_power_context_t;

static bt_dm_power_context_t g_bt_dm_power_t = {
    .state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY,
    .dev[0].target_state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY,
    .dev[0].state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY,
    .dev[0].cb = NULL,
    .dev[1].target_state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY,
    .dev[1].state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY,
    .dev[1].cb = NULL,
    .reset_flag = 0,
    .reset_type = 0,
    .cb_set = {NULL},
    .reset_user_data = NULL,
    .reset_cb = NULL
};

extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);

static void bt_dm_power_mutex_lock(void)
{
    bt_os_take_stack_mutex();
}

static void bt_dm_power_mutex_unlock(void)
{
    bt_os_give_stack_mutex();
}

static void BT_DM_POWER_DUMP_STATE()
{
    bt_dmgr_report_id("[BT_DM][I] Power state <BT current state:%d flag:0x%x>, \
<BLE target state:%d -- current state:%d flag:0x%x>, <Classic target state:%d -- current state:%d flag:0x%x>", 8,
    g_bt_dm_power_t.state, g_bt_dm_power_t.reset_flag,
    g_bt_dm_power_t.dev[0].target_state, g_bt_dm_power_t.dev[0].state, g_bt_dm_power_t.dev[0].reset_flag,
    g_bt_dm_power_t.dev[1].target_state, g_bt_dm_power_t.dev[1].state, g_bt_dm_power_t.dev[1].reset_flag);
}

static void bt_dm_power_state_update_sub_dev(bt_dm_power_dev_t *dev, bt_device_type_t type)
{
    BT_DM_POWER_DUMP_STATE();
    if (NULL == dev->cb) {
        dev->state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY;
    } else if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == dev->state &&
        (BT_DM_POWER_FLAG_RESET_PHASE_2 == dev->reset_flag || (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == dev->target_state &&
         (!(g_bt_dm_power_t.reset_flag & (BT_DM_POWER_FLAG_RESET_PHASE_1 | BT_DM_POWER_FLAG_RESET_PHASE_2)))))) {
        /* If reset in phase 1 or phase 2 we do not care the target state, just run the reset flow. */
        dev->state = ((BT_STATUS_SUCCESS == dev->cb(BT_DEVICE_MANAGER_POWER_EVT_PREPARE_ACTIVE, g_bt_dm_power_t.reset_type, NULL, 0)) ?
            BT_DEVICE_MANAGER_POWER_STATE_ACTIVE : BT_DEVICE_MANAGER_POWER_STATE_ACTIVE_PENDING);
        for (uint8_t i = 0; (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == dev->state) && (i < BT_DM_CB_MAXIMUM_NUM); i++) {
           if (NULL != g_bt_dm_power_t.cb_set[i]) {
                if (BT_DEVICE_TYPE_LE == type) {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_LE_ACTIVE_COMPLETE, g_bt_dm_power_t.reset_type, NULL, 0); 
                } else {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_ACTIVE_COMPLETE, g_bt_dm_power_t.reset_type, NULL, 0); 
                }
           }
        }
    } else if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == dev->state &&
        (BT_DM_POWER_FLAG_RESET_PHASE_1 == dev->reset_flag || (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == dev->target_state &&
        (!(g_bt_dm_power_t.reset_flag & (BT_DM_POWER_FLAG_RESET_PHASE_1 | BT_DM_POWER_FLAG_RESET_PHASE_2)))))) {
        /* If reset in phase 1 or phase 2 we do not care the target state, just run the reset flow. */
        dev->state = ((BT_STATUS_SUCCESS == dev->cb(BT_DEVICE_MANAGER_POWER_EVT_PREPARE_STANDBY, g_bt_dm_power_t.reset_type, NULL, 0)) ?
            BT_DEVICE_MANAGER_POWER_STATE_STANDBY : BT_DEVICE_MANAGER_POWER_STATE_STANDBY_PENDING);
        for (uint8_t i = 0; (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == dev->state) && (i < BT_DM_CB_MAXIMUM_NUM); i++) {
           if (NULL != g_bt_dm_power_t.cb_set[i]) {
                if (BT_DEVICE_TYPE_LE == type) {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_LE_STANDBY_COMPLETE, g_bt_dm_power_t.reset_type, NULL, 0);
                } else {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_STANDBY_COMPLETE, g_bt_dm_power_t.reset_type, NULL, 0);
                }
           }
        }
    }
    BT_DM_POWER_DUMP_STATE();
}

static void bt_dm_power_reset_state_update()
{
    BT_DM_POWER_DUMP_STATE();
    if (g_bt_dm_power_t.reset_flag == BT_DM_POWER_FLAG_RESET_PENDING) {
        if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[0].target_state &&
            BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[1].target_state) {
            bt_dmgr_report_id("[BT_DM][I] Cancel reset due to power off", 0);
            g_bt_dm_power_t.reset_flag = 0;
            g_bt_dm_power_t.reset_type = 0;
        } else if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.state &&
            g_bt_dm_power_t.dev[0].target_state == g_bt_dm_power_t.dev[0].state &&
            g_bt_dm_power_t.dev[1].target_state == g_bt_dm_power_t.dev[1].state) {
            g_bt_dm_power_t.reset_flag = BT_DM_POWER_FLAG_RESET_PHASE_1;
            if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.dev[0].state) {
                g_bt_dm_power_t.dev[0].reset_flag = BT_DM_POWER_FLAG_RESET_PHASE_1;
            }
            if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.dev[1].state) {
                g_bt_dm_power_t.dev[1].reset_flag = BT_DM_POWER_FLAG_RESET_PHASE_1;
            }
        }
    }
    if (!(g_bt_dm_power_t.reset_flag & (BT_DM_POWER_FLAG_RESET_PHASE_1 | BT_DM_POWER_FLAG_RESET_PHASE_2)) ||
        (BT_DEVICE_MANAGER_POWER_STATE_STANDBY != g_bt_dm_power_t.state && BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != g_bt_dm_power_t.state)) {
        /* Reset flow is pending due to target state != current state. */
        return;
    }
    if(((!g_bt_dm_power_t.dev[0].reset_flag) || (BT_DM_POWER_FLAG_RESET_PHASE_2 == g_bt_dm_power_t.dev[0].reset_flag)) &&
       ((!g_bt_dm_power_t.dev[1].reset_flag) || (BT_DM_POWER_FLAG_RESET_PHASE_2 == g_bt_dm_power_t.dev[1].reset_flag))) {
        if (NULL != g_bt_dm_power_t.reset_cb) {
            g_bt_dm_power_t.reset_cb(BT_DEVICE_MANAGER_POWER_RESET_PROGRESS_COMPLETE, g_bt_dm_power_t.reset_user_data);
            g_bt_dm_power_t.reset_cb = NULL;
        }
        bt_dm_power_state_update_sub_dev(&(g_bt_dm_power_t.dev[0]), BT_DEVICE_TYPE_LE);
        bt_dm_power_state_update_sub_dev(&(g_bt_dm_power_t.dev[1]), BT_DEVICE_TYPE_CLASSIC);
        if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.dev[0].state ||
            BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[0].state) {
            g_bt_dm_power_t.dev[0].reset_flag = 0;
        }
        if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.dev[1].state ||
            BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[1].state) {
            g_bt_dm_power_t.dev[1].reset_flag = 0;
        }
        if (0 == g_bt_dm_power_t.dev[0].reset_flag && 0 == g_bt_dm_power_t.dev[1].reset_flag) {
            for (uint8_t i = 0; i < BT_DM_CB_MAXIMUM_NUM; i++) {
               if (NULL != g_bt_dm_power_t.cb_set[i]) {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE, g_bt_dm_power_t.reset_type, NULL, 0);
               }
            }
            g_bt_dm_power_t.reset_flag = 0;
            g_bt_dm_power_t.reset_type = 0;
        }
        return;
    }
    bt_dm_power_state_update_sub_dev(&(g_bt_dm_power_t.dev[0]), BT_DEVICE_TYPE_LE);
    bt_dm_power_state_update_sub_dev(&(g_bt_dm_power_t.dev[1]), BT_DEVICE_TYPE_CLASSIC);
    if ((BT_DM_POWER_FLAG_RESET_PHASE_1 == g_bt_dm_power_t.dev[0].reset_flag && BT_DEVICE_MANAGER_POWER_STATE_STANDBY != g_bt_dm_power_t.dev[0].state) ||
        (BT_DM_POWER_FLAG_RESET_PHASE_1 == g_bt_dm_power_t.dev[1].reset_flag && BT_DEVICE_MANAGER_POWER_STATE_STANDBY != g_bt_dm_power_t.dev[1].state)) {
        /* Wait both request reset device standby. */
        return;
    }
    if (BT_DEVICE_MANAGER_POWER_RESET_TYPE_NORMAL == g_bt_dm_power_t.reset_type && BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.state &&
        BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[0].state && BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[1].state) {
        /* If reset type is normal, then need do power off in phase 1 */
        bt_device_manager_power_state_t pre_state = g_bt_dm_power_t.state;
        g_bt_dm_power_t.state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY_PENDING;
        if (BT_STATUS_SUCCESS != bt_power_off()) {
            g_bt_dm_power_t.state = pre_state;
        }
        return;
    }
    /* If all request reset bt standby complete, start phase 2. */
    g_bt_dm_power_t.dev[0].reset_flag = (g_bt_dm_power_t.dev[0].reset_flag ? BT_DM_POWER_FLAG_RESET_PHASE_2 : 0);
    g_bt_dm_power_t.dev[1].reset_flag = (g_bt_dm_power_t.dev[1].reset_flag ? BT_DM_POWER_FLAG_RESET_PHASE_2 : 0);
    if (NULL != g_bt_dm_power_t.reset_cb) {
        g_bt_dm_power_t.reset_cb(BT_DEVICE_MANAGER_POWER_RESET_PROGRESS_MEDIUM, g_bt_dm_power_t.reset_user_data);
    }
    for (uint8_t i = 0; i < BT_DM_CB_MAXIMUM_NUM; i++) {
        if (NULL != g_bt_dm_power_t.cb_set[i]) {
            g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE, g_bt_dm_power_t.reset_type, NULL, 0);
        }
    }
    if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.state) {
        /* Need do power on in phase 1 */
        bt_device_manager_power_state_t pre_state = g_bt_dm_power_t.state;
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
        g_bt_dm_power_t.state = BT_DEVICE_MANAGER_POWER_STATE_ACTIVE_PENDING;
        if (BT_STATUS_SUCCESS != bt_power_on((void *)local_addr, NULL)) {
            g_bt_dm_power_t.state = pre_state;
        }
    } else {
        bt_dm_power_reset_state_update();
    }
    BT_DM_POWER_DUMP_STATE();
}

static void bt_dm_power_state_update()
{
    if (g_bt_dm_power_t.reset_flag) {
        bt_dm_power_reset_state_update();
        if (g_bt_dm_power_t.reset_flag & (BT_DM_POWER_FLAG_RESET_PHASE_1 | BT_DM_POWER_FLAG_RESET_PHASE_2)) {
            return;
        }
    }
    BT_DM_POWER_DUMP_STATE();
    if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.state) {
        bt_dm_power_state_update_sub_dev(&(g_bt_dm_power_t.dev[0]), BT_DEVICE_TYPE_LE);
        bt_dm_power_state_update_sub_dev(&(g_bt_dm_power_t.dev[1]), BT_DEVICE_TYPE_CLASSIC);
        if ((BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[0].target_state &&
             g_bt_dm_power_t.dev[0].target_state == g_bt_dm_power_t.dev[0].state) &&
            (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[1].target_state &&
             g_bt_dm_power_t.dev[1].target_state == g_bt_dm_power_t.dev[1].state)) {
            /* If both le and classic BT request standby we should power off the BT. */
            bt_device_manager_power_state_t pre_state = g_bt_dm_power_t.state;
            g_bt_dm_power_t.state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY_PENDING;
            if (BT_STATUS_SUCCESS != bt_power_off()) {
                g_bt_dm_power_t.state = pre_state;
            }
        }
    } else if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.state &&
        (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.dev[0].target_state ||
         BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == g_bt_dm_power_t.dev[1].target_state)) {
        /* If any le or classic BT request active we should power on the BT. */
        bt_device_manager_power_state_t pre_state = g_bt_dm_power_t.state;
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
        g_bt_dm_power_t.state = BT_DEVICE_MANAGER_POWER_STATE_ACTIVE_PENDING;
        if (BT_STATUS_SUCCESS != bt_power_on((void *)local_addr, NULL)) {
            g_bt_dm_power_t.state = pre_state;
        }
    }
    BT_DM_POWER_DUMP_STATE();
}

void        bt_device_manager_power_on_cnf()
{
#ifdef TEMP_CLASSIC_BT_OFF
    bt_dmgr_report_id("[BT_DM][I] Power on cnf !!!", 0);
    g_bt_dm_power_t.state = BT_DEVICE_MANAGER_POWER_STATE_ACTIVE;
    if (g_bt_dm_power_t.reset_flag & (BT_DM_POWER_FLAG_RESET_PHASE_1 | BT_DM_POWER_FLAG_RESET_PHASE_2)) {
        g_bt_dm_power_t.reset_flag = BT_DM_POWER_FLAG_RESET_PHASE_2;
        bt_dm_power_state_update();
    } else {
        bt_dm_power_state_update();
        for (uint8_t i = 0; i < BT_DM_CB_MAXIMUM_NUM; i++) {
           if (NULL != g_bt_dm_power_t.cb_set[i]) {
                g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE, BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS, NULL, 0);
           }
        }
    }
#endif
}

void        bt_device_manager_power_off_cnf()
{
#ifdef TEMP_CLASSIC_BT_OFF
    bt_dmgr_report_id("[BT_DM][I] Power off cnf !!!", 0);
    g_bt_dm_power_t.state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY;
    g_bt_dm_power_t.dev[0].state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY;
    g_bt_dm_power_t.dev[1].state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY;
    bt_dm_power_state_update();
    if (!(g_bt_dm_power_t.reset_flag & (BT_DM_POWER_FLAG_RESET_PHASE_1 | BT_DM_POWER_FLAG_RESET_PHASE_2))) {
        for (uint8_t i = 0; i < BT_DM_CB_MAXIMUM_NUM; i++) {
           if (NULL != g_bt_dm_power_t.cb_set[i]) {
                g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE, BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS, NULL, 0);
           }
        }
    }
#endif
}

bt_status_t bt_device_manager_dev_set_power_state(bt_device_type_t type, bt_device_manager_power_state_t power_state)
{
    bt_dmgr_report_id("[BT_DM][I] Device:%d set power state:%d", 2, type, power_state);
    if (type & BT_DEVICE_TYPE_LE) {
        bt_device_manager_power_status_t power_status = g_bt_dm_power_t.dev[0].reset_flag ? g_bt_dm_power_t.reset_type : BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS;
        g_bt_dm_power_t.dev[0].state = power_state;
        for (uint8_t i = 0; i < BT_DM_CB_MAXIMUM_NUM; i++) {
            if (NULL != g_bt_dm_power_t.cb_set[i]) {
                if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == power_state) {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_LE_ACTIVE_COMPLETE, power_status, NULL, 0);
                } else if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == power_state) {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_LE_STANDBY_COMPLETE, power_status, NULL, 0);
                }
            }
        }
    }
    if (type & BT_DEVICE_TYPE_CLASSIC) {
        bt_device_manager_power_status_t power_status = g_bt_dm_power_t.dev[1].reset_flag ? g_bt_dm_power_t.reset_type : BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS;
        g_bt_dm_power_t.dev[1].state = power_state;
        for (uint8_t i = 0; i < BT_DM_CB_MAXIMUM_NUM; i++) {
            if (NULL != g_bt_dm_power_t.cb_set[i]) {
                if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == power_state) {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_ACTIVE_COMPLETE, power_status, NULL, 0);
                } else if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == power_state) {
                    g_bt_dm_power_t.cb_set[i](BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_STANDBY_COMPLETE, power_status, NULL, 0);
                }
            }
        }
    }
    bt_dm_power_state_update();
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_device_manager_dev_register_callback(bt_device_type_t type, bt_device_manager_power_callback_t cb)
{
    bt_dmgr_report_id("[BT_DM][I] Device:%d register cb:0x%x", 2, type, cb);
    if (type & BT_DEVICE_TYPE_LE) {
        g_bt_dm_power_t.dev[0].cb = cb;
    }
    if (type & BT_DEVICE_TYPE_CLASSIC) {
        g_bt_dm_power_t.dev[1].cb = cb;
    }
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_device_manager_register_callback(bt_device_manager_power_callback_t cb)
{
    uint8_t temp_index = 0xFF;
    bt_dmgr_report_id("[BT_DM][I] User register cb:0x%x", 1, cb);
    for (uint8_t i = 0; i < BT_DM_CB_MAXIMUM_NUM; i++) {
        if (0xFF == temp_index && NULL == g_bt_dm_power_t.cb_set[i]) {
            temp_index = i;
        } else if (cb == g_bt_dm_power_t.cb_set[i]) {
            bt_dmgr_report_id("[BT_DM][W] User register duplicate.", 0);
            return BT_STATUS_SUCCESS;
        }
    }
    if (0xFF == temp_index) {
        bt_dmgr_report_id("[BT_DM][E] User register fail reason:full", 0);
        return BT_STATUS_FAIL;
    }
    g_bt_dm_power_t.cb_set[temp_index] = cb;
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_device_manager_deregister_callback(bt_device_manager_power_callback_t cb)
{
    bt_dmgr_report_id("[BT_DM][I] User deregister cb:0x%x", 1, cb);
    for (uint8_t i = 0; i < BT_DM_CB_MAXIMUM_NUM; i++) {
        if (cb == g_bt_dm_power_t.cb_set[i]) {
            g_bt_dm_power_t.cb_set[i] = NULL;
            return BT_STATUS_SUCCESS;
        }
    }
    bt_dmgr_report_id("[BT_DM][W] User deregister find fail", 0);
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_device_manager_power_active(bt_device_type_t type)
{
    bt_dmgr_report_id("[BT_DM][I] Power active type %d", 1, type);
    if (!(type & (BT_DEVICE_TYPE_LE | BT_DEVICE_TYPE_CLASSIC))) {
        bt_dmgr_report_id("[BT_DM][E] Power active type error", 0);
        return BT_STATUS_FAIL;
    }
    bt_dm_power_mutex_lock();
    if (type & BT_DEVICE_TYPE_LE) {
        g_bt_dm_power_t.dev[0].target_state = BT_DEVICE_MANAGER_POWER_STATE_ACTIVE;
    }
    if (type & BT_DEVICE_TYPE_CLASSIC) {
        g_bt_dm_power_t.dev[1].target_state = BT_DEVICE_MANAGER_POWER_STATE_ACTIVE;
    }
    //if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY != g_bt_dm_power_t.state) {
    if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.state) {
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
        bt_set_local_public_address((void *)local_addr);
    }
    bt_dm_power_state_update();
    bt_dm_power_mutex_unlock();
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_device_manager_power_standby(bt_device_type_t type)
{
    bt_dmgr_report_id("[BT_DM][I] Power standby type %d", 1, type);
#ifdef MTK_BT_CM_SUPPORT
    extern bool bt_avm_allow_poweroff(void *data);
    if (false == bt_avm_allow_poweroff(NULL)) {
        return BT_STATUS_FAIL;
    }
#endif
    bt_dm_power_mutex_lock();
    if (type & BT_DEVICE_TYPE_LE) {
        g_bt_dm_power_t.dev[0].target_state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY;
    }
    if (type & BT_DEVICE_TYPE_CLASSIC) {
        g_bt_dm_power_t.dev[1].target_state = BT_DEVICE_MANAGER_POWER_STATE_STANDBY;
    }
    bt_dm_power_state_update();
    bt_dm_power_mutex_unlock();
    return BT_STATUS_SUCCESS;
}

bt_status_t bt_device_manager_power_reset(bt_device_manager_power_reset_t type, bt_device_manager_power_reset_callback_t cb, void* user_data)
{
    bt_dmgr_report_id("[BT_DM][I] Bt power reset by type %d", 1, type);
    BT_DM_POWER_DUMP_STATE();
    if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[0].target_state &&
        BT_DEVICE_MANAGER_POWER_STATE_STANDBY == g_bt_dm_power_t.dev[1].target_state) {
        return BT_STATUS_FAIL;
    }
    if (g_bt_dm_power_t.reset_flag) {
        return BT_STATUS_PENDING;
    }
    bt_dm_power_mutex_lock();
    g_bt_dm_power_t.reset_cb = cb;
    g_bt_dm_power_t.reset_user_data = user_data;
    g_bt_dm_power_t.reset_type = type;
    g_bt_dm_power_t.reset_flag = BT_DM_POWER_FLAG_RESET_PENDING;
    bt_dm_power_state_update();
    bt_dm_power_mutex_unlock();
    return BT_STATUS_SUCCESS;
}

bt_device_manager_power_state_t bt_device_manager_power_get_power_state(bt_device_type_t type)
{
    bt_dmgr_report_id("[BT_DM][I] Bt power get state by type %d", 1, type);
    BT_DM_POWER_DUMP_STATE();
    if (type == BT_DEVICE_TYPE_LE) {
        return (g_bt_dm_power_t.dev[0].reset_flag ? BT_DEVICE_MANAGER_POWER_STATE_RESTING : g_bt_dm_power_t.dev[0].state);
    }
    return (g_bt_dm_power_t.dev[1].reset_flag ? BT_DEVICE_MANAGER_POWER_STATE_RESTING : g_bt_dm_power_t.dev[1].state);
}

