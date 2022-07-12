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
#include "bt_type.h"
#include "bt_system.h"
#include "bt_device_manager_internal.h"
#include "bt_callback_manager.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_utils.h"
#include "bt_connection_manager_internal.h"
#include "bt_device_manager_power.h"
#include "bt_aws_mce_srv.h"
#include "bt_device_manager_test_mode.h"

void default_bt_cm_poweroff_notify()
{
}

void bt_cm_poweroff_notify();

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_cm_prepare_standby_callback=_default_bt_cm_prepare_standby_callback")
#pragma comment(linker, "/alternatename:_bt_cm_poweroff_notify=_default_bt_cm_poweroff_notify")
#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__CC_ARM)
#pragma weak bt_cm_prepare_standby_callback = default_bt_cm_prepare_standby_callback
#pragma weak bt_cm_poweroff_notify = default_bt_cm_poweroff_notify
#else
#error "Unsupported Platform"
#endif

#define BT_CM_POWER_FLAG_FORCE              (0x01)
#define BT_CM_POWER_FLAG_DUT_MODE           (0x02)
//#define BT_CM_POWER_FLAG_DEINIT_PREPAIRE    (0x03)
#define BT_CM_POWER_FLAG_TEST_SYS_OFF       (0x08)
#define BT_CM_POWER_FLAG_TEST_SYS_RESET     (0x10)
typedef uint8_t bt_cm_power_flag_t;

#ifndef TEMP_CLASSIC_BT_OFF

static struct {
    bt_cm_power_state_t cur_state;
    bt_cm_power_state_t target_state;
    bt_cm_power_flag_t  flags;
    void* reset_user_data;
    bt_cm_power_reset_callback_t reset_cb;
} g_bt_cm_power_cnt = {
    .cur_state = BT_CM_POWER_STATE_OFF,
    .target_state = BT_CM_POWER_STATE_OFF,
    .flags = 0,
    .reset_user_data = NULL,
    .reset_cb = NULL
};

static void         bt_cm_power_test_sys_excute()
{
    if (g_bt_cm_power_cnt.flags & BT_CM_POWER_FLAG_TEST_SYS_OFF) {
#ifdef HAL_RTC_FEATURE_RTC_MODE
        if (HAL_RTC_STATUS_ERROR == hal_rtc_enter_rtc_mode()) {
            bt_cm_assert(0 && "Enter RTC mode fail !!!");
        }
#else
#ifdef HAL_PMU_MODULE_ENABLED
        pmu_enter_power_off();
#else
        bt_cm_assert(0 && "No method to do sys power off");
#endif
#endif
    } else if (g_bt_cm_power_cnt.flags & BT_CM_POWER_FLAG_TEST_SYS_RESET) {
#ifdef HAL_WDT_MODULE_ENABLED
        hal_wdt_config_t wdt_config;
        wdt_config.mode = HAL_WDT_MODE_RESET;
        wdt_config.seconds = 1;
        hal_wdt_init(&wdt_config);
        hal_wdt_software_reset();
#else
        bt_cm_assert(0 && "No method to sys power reset");
#endif
    }
}

static bt_status_t  bt_cm_power_sys_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    switch (msg) {
    case BT_POWER_ON_CNF: {
        bt_cmgr_report_id("[BT_CM][POWER][I] BT POWER ON cnf status 0x%x", 1, status);
        bt_cm_power_state_t pre_power_state = g_bt_cm_power_cnt.cur_state;
        g_bt_cm_power_cnt.flags = 0;
        g_bt_cm_power_cnt.cur_state = BT_CM_POWER_STATE_ON;
        if (BT_CM_POWER_STATE_RESETING == pre_power_state) {
            if (NULL != g_bt_cm_power_cnt.reset_cb) {
                g_bt_cm_power_cnt.reset_cb(BT_CM_POWER_RESET_PROGRESS_COMPLETE, g_bt_cm_power_cnt.reset_user_data);
                g_bt_cm_power_cnt.reset_cb = NULL;
                g_bt_cm_power_cnt.reset_user_data = NULL;
            }
        } else {
            bt_cm_power_state_update_ind_t state = {
                .power_state = BT_CM_POWER_STATE_ON
            };
            bt_cm_event_callback(BT_CM_EVENT_POWER_STATE_UPDATE, &state, sizeof(state));
            bt_cm_register_callback_notify(BT_CM_EVENT_POWER_STATE_UPDATE, &state, sizeof(state));
            bt_cm_power_on_cnf(BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS);
        }
        if (BT_CM_POWER_STATE_ON != g_bt_cm_power_cnt.target_state) {
            bt_cm_power_update(NULL);
        }
    }
        break;
    case BT_POWER_OFF_CNF: {
        bt_cmgr_report_id("[BT_CM][POWER][I] BT POWER OFF cnf status 0x%x", 1, status);
        if (g_bt_cm_power_cnt.flags & (BT_CM_POWER_FLAG_TEST_SYS_OFF | BT_CM_POWER_FLAG_TEST_SYS_RESET)) {
            bt_cm_power_test_sys_excute();
        }
        g_bt_cm_power_cnt.flags = 0;
        bt_cm_power_off_cnf(BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS);
        if (BT_CM_POWER_STATE_RESETING == g_bt_cm_power_cnt.cur_state) {
            if (NULL != g_bt_cm_power_cnt.reset_cb) {
                g_bt_cm_power_cnt.reset_cb(BT_CM_POWER_RESET_PROGRESS_MEDIUM, g_bt_cm_power_cnt.reset_user_data);
            }
        } else {
            bt_cm_power_state_update_ind_t state = {
                .power_state = BT_CM_POWER_STATE_OFF
            };
            g_bt_cm_power_cnt.cur_state = BT_CM_POWER_STATE_OFF;
            bt_cm_event_callback(BT_CM_EVENT_POWER_STATE_UPDATE, &state, sizeof(state));
            bt_cm_register_callback_notify(BT_CM_EVENT_POWER_STATE_UPDATE, &state, sizeof(state));
            bt_device_manager_db_flush_all(BT_DEVICE_MANAGER_DB_FLUSH_BLOCK);
            if (BT_CM_POWER_STATE_OFF != g_bt_cm_power_cnt.target_state) {
                bt_cm_power_update(NULL);
            }
        }
    }
        break;
    case BT_DUT_MODE_ACTIVE_IND:
        bt_cmgr_report_id("[BT_CM][POWER][I] DUT mode active status 0x%x", 1, status);
        g_bt_cm_power_cnt.flags |= BT_CM_POWER_FLAG_DUT_MODE;
        break;
    case BT_PANIC:
        bt_cmgr_report_id("[BT_CM][POWER][W] Warnning PANIC event", 0);
        break;
    default:
        bt_cmgr_report_id("[BT_CM][POWER][E] Error SDK sys msg:0x%x", 1, msg);
        break;
    }
    return BT_STATUS_SUCCESS;
}

bt_cm_power_state_t bt_cm_power_get_state()
{
    return g_bt_cm_power_cnt.cur_state;
}

void            bt_cm_power_update(void *params)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][POWER][I] BT power update, state target:0x%x, cur:0x%x, flag:0x%x", 3,
        g_bt_cm_power_cnt.target_state, g_bt_cm_power_cnt.cur_state, g_bt_cm_power_cnt.flags);
    if (BT_CM_POWER_STATE_OFF == g_bt_cm_power_cnt.cur_state && BT_CM_POWER_STATE_ON == g_bt_cm_power_cnt.target_state) {
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
        g_bt_cm_power_cnt.cur_state = BT_CM_POWER_STATE_ON_PENDING;
        if (BT_STATUS_SUCCESS != (ret = bt_power_on((bt_bd_addr_ptr_t)local_addr, NULL))) {
            bt_cmgr_report_id("[BT_CM][POWER][E] BT power on fail, status:0x%x", 1, ret);
            g_bt_cm_power_cnt.cur_state = BT_CM_POWER_STATE_OFF;
            bt_cm_assert(0 && "Power on fail");
        }
    } else if ((BT_CM_POWER_STATE_ON == g_bt_cm_power_cnt.cur_state && BT_CM_POWER_STATE_OFF == g_bt_cm_power_cnt.target_state) ||
        BT_CM_POWER_STATE_RESETING == g_bt_cm_power_cnt.cur_state || BT_CM_POWER_STATE_OFF_PENDING == g_bt_cm_power_cnt.cur_state) {
        if (BT_CM_POWER_STATE_RESETING != g_bt_cm_power_cnt.cur_state) {
            g_bt_cm_power_cnt.cur_state = BT_CM_POWER_STATE_OFF_PENDING;
        }
        if (NULL == params && !(BT_CM_POWER_FLAG_FORCE & g_bt_cm_power_cnt.flags) && BT_STATUS_SUCCESS != bt_cm_prepare_power_deinit(false, BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS)) {
            if (!bt_cm_timer_is_exist(BT_CM_FORCE_POWER_OFF_TIMER_ID)) {
                bt_cm_timer_start(BT_CM_FORCE_POWER_OFF_TIMER_ID, 3000, bt_cm_power_update, (void *)1);
            }
            return;
        }
        bt_cm_timer_stop(BT_CM_FORCE_POWER_OFF_TIMER_ID);
        if (BT_STATUS_SUCCESS == bt_cm_prepare_power_deinit(true, BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS)) {
            return;
        }
        if (BT_STATUS_SUCCESS != bt_cm_prepare_standby_callback()) {
            return;
        }
        if (NULL == params) {
            bt_cm_timer_start(BT_CM_FORCE_POWER_OFF_TIMER_ID, 1, bt_cm_power_update, (void *)1);
        } else if (BT_STATUS_SUCCESS != (ret = (BT_CM_POWER_STATE_RESETING == g_bt_cm_power_cnt.cur_state) ?
            bt_power_reset(BT_POWER_RESET_SW) : bt_power_off())) {
            bt_cmgr_report_id("[BT_CM][POWER][E] BT power off or reset fail, cur state:0x%x, status:0x%x", 2, g_bt_cm_power_cnt.cur_state, ret);
        }
    }
}

bt_status_t     bt_cm_power_active()
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    bt_cmgr_report_id("[BT_CM][POWER][I] BT power on...", 0);
    g_bt_cm_power_cnt.target_state = BT_CM_POWER_STATE_ON;
    if (BT_CM_POWER_STATE_OFF != g_bt_cm_power_cnt.cur_state) {
        bt_cmgr_report_id("[BT_CM][POWER][I] BT is not in POWER OFF state, cur state : %d", 1, g_bt_cm_power_cnt.cur_state);
        return ret;
    }
    bt_cm_power_update(NULL);
    return ret;
}

bt_status_t     bt_cm_power_standby(bool force)
{
    bt_cmgr_report_id("[BT_CM][POWER][I] BT power off force:%d ...", 1, force);
    extern bool bt_avm_allow_poweroff(void *data);
    if (false == bt_avm_allow_poweroff(NULL)) {
        return BT_STATUS_FAIL;
    }
    bt_cm_mutex_lock();
    g_bt_cm_power_cnt.target_state = BT_CM_POWER_STATE_OFF;
    if (true == force || (g_bt_cm_power_cnt.flags & BT_CM_POWER_FLAG_DUT_MODE)) {
        g_bt_cm_power_cnt.flags |= BT_CM_POWER_FLAG_FORCE;
    }
    if (BT_CM_POWER_STATE_ON != g_bt_cm_power_cnt.cur_state) {
        bt_cmgr_report_id("[BT_CM][POWER][W] BT is not in POWER ON state, cur state:%d", 1, g_bt_cm_power_cnt.cur_state);
        bt_cm_mutex_unlock();
        return BT_STATUS_SUCCESS;
    }
    bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
    bt_cm_power_update(NULL);
    bt_cm_mutex_unlock();
    return BT_STATUS_SUCCESS;
}

bt_status_t     bt_cm_power_reset(bool force)
{
    bt_cmgr_report_id("[BT_CM][POWER][I] Bt power reset:%d...", 1, force);
    bt_cm_mutex_lock();
    if (g_bt_cm_power_cnt.cur_state != BT_CM_POWER_STATE_ON ||
        g_bt_cm_power_cnt.target_state != BT_CM_POWER_STATE_ON) {
        bt_cmgr_report_id("[BT_CM][POWER][W] Bt power reset state error, target:%d, cur:%d, flags:0x%x", 3,
            g_bt_cm_power_cnt.target_state, g_bt_cm_power_cnt.cur_state, g_bt_cm_power_cnt.flags);
        bt_cm_mutex_unlock();
        return BT_CM_STATUS_INVALID_STATUS;
    }
    if (true == force) {
        g_bt_cm_power_cnt.flags |= BT_CM_POWER_FLAG_FORCE;
    }
    bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
    g_bt_cm_power_cnt.cur_state = BT_CM_POWER_STATE_RESETING;
    bt_cm_power_update(NULL);
    bt_cm_mutex_unlock();
    return BT_STATUS_SUCCESS;
}

bt_status_t     bt_cm_power_reset_ext(bool force, bt_cm_power_reset_callback_t cb, void* user_data)
{
    bt_cmgr_report_id("[BT_CM][POWER][I] Bt power reset ext", 0);
    bt_cm_mutex_lock();
    if (g_bt_cm_power_cnt.cur_state != BT_CM_POWER_STATE_ON ||
        g_bt_cm_power_cnt.target_state != BT_CM_POWER_STATE_ON) {
        bt_cmgr_report_id("[BT_CM][POWER][W] Bt power reset state error, target:%d, cur:%d, flags:0x%x", 3,
            g_bt_cm_power_cnt.target_state, g_bt_cm_power_cnt.cur_state, g_bt_cm_power_cnt.flags);
        bt_cm_mutex_unlock();
        return BT_CM_STATUS_INVALID_STATUS;
    }
    if (true == force) {
        g_bt_cm_power_cnt.flags |= BT_CM_POWER_FLAG_FORCE;
    }
    bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
    g_bt_cm_power_cnt.cur_state = BT_CM_POWER_STATE_RESETING;
    g_bt_cm_power_cnt.reset_cb = cb;
    g_bt_cm_power_cnt.reset_user_data = user_data;
    bt_cm_power_update(NULL);
    bt_cm_mutex_unlock();
    return BT_STATUS_SUCCESS;
}

void            bt_cm_power_init()
{
    bt_callback_manager_register_callback(bt_callback_type_app_event,
            (uint32_t)MODULE_MASK_SYSTEM, (void *)bt_cm_power_sys_callback);
}

void            bt_cm_power_deinit()
{
    bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)bt_cm_power_sys_callback);
}

//Just for UT/IT test.
void            bt_cm_power_test_sys(bt_cm_power_test_sys_t type)
{
    if (BT_CM_POWER_TEST_SYS_RESET == type) {
        g_bt_cm_power_cnt.flags |= BT_CM_POWER_FLAG_TEST_SYS_RESET;
    } else {
        g_bt_cm_power_cnt.flags |= BT_CM_POWER_FLAG_TEST_SYS_OFF;
    }
    if (BT_CM_POWER_STATE_ON == g_bt_cm_power_cnt.cur_state ||
        BT_CM_POWER_STATE_ON_PENDING == g_bt_cm_power_cnt.cur_state) {
        bt_cm_power_standby(false);
        return;
    } else if (BT_CM_POWER_STATE_OFF == g_bt_cm_power_cnt.cur_state) {
        bt_cm_power_test_sys_excute();
    }
}

void            bt_cm_power_standby_ready()
{
    bt_cmgr_report_id("[BT_CM] Prepare standby ready", 0);
    bt_cm_timer_start(BT_CM_FORCE_POWER_OFF_TIMER_ID, 1, bt_cm_power_update, (void *)1);
}

bt_status_t     default_bt_cm_prepare_standby_callback()
{
    bt_cmgr_report_id("[BT_CM] Prepare standby callback default", 0);
    return BT_STATUS_SUCCESS;
}




#else /*#ifndef TEMP_CLASSIC_BT_OFF*/








static struct {
    bt_cm_power_state_t cur_state;
    bt_cm_power_state_t target_state;
    bt_cm_power_flag_t  flags;
} g_bt_cm_edr_power_cnt = {
    .cur_state = BT_CM_POWER_STATE_OFF,
    .target_state = BT_CM_POWER_STATE_OFF,
    .flags = 0
};

bt_device_manager_power_status_t g_bt_cm_edr_power_status = BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS;

bt_cm_power_state_t bt_cm_power_get_state()
{
    return g_bt_cm_edr_power_cnt.cur_state;
}

void                bt_cm_power_update(void *params)
{
    bt_cmgr_report_id("[BT_CM][POWER][I] BT power update, state target:0x%x, cur:0x%x, flag:0x%x", 3,
        g_bt_cm_edr_power_cnt.target_state, g_bt_cm_edr_power_cnt.cur_state, g_bt_cm_edr_power_cnt.flags);
    if (BT_CM_POWER_STATE_OFF == g_bt_cm_edr_power_cnt.cur_state && BT_CM_POWER_STATE_ON == g_bt_cm_edr_power_cnt.target_state) {
        g_bt_cm_edr_power_cnt.flags = 0;
        g_bt_cm_edr_power_cnt.cur_state = BT_CM_POWER_STATE_ON;
        bt_cmgr_report_id("[BT_CM][POWER][I] BT POWER ON cnf", 0);
        printf("TYM PowerOn E\r\n");
        bt_cm_power_state_update_ind_t state = {
            .power_state = BT_CM_POWER_STATE_ON
        };
        if (BT_DEVICE_MANAGER_POWER_STATE_RESTING != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
            bt_cm_event_callback(BT_CM_EVENT_POWER_STATE_UPDATE, &state, sizeof(state));
        }
        if (BT_DEVICE_MANAGER_POWER_STATUS_ROLE_RECOVERY != g_bt_cm_edr_power_status) {
            bt_cm_register_callback_notify(BT_CM_EVENT_POWER_STATE_UPDATE, &state, sizeof(state));
        }
        bt_cm_power_on_cnf(g_bt_cm_edr_power_status);
        bt_device_manager_dev_set_power_state(BT_DEVICE_TYPE_CLASSIC, BT_DEVICE_MANAGER_POWER_STATE_ACTIVE);
    } else if ((BT_CM_POWER_STATE_ON == g_bt_cm_edr_power_cnt.cur_state && BT_CM_POWER_STATE_OFF == g_bt_cm_edr_power_cnt.target_state) ||
            BT_CM_POWER_STATE_OFF_PENDING == g_bt_cm_edr_power_cnt.cur_state) {
        bt_cm_poweroff_notify();

        g_bt_cm_edr_power_cnt.cur_state = BT_CM_POWER_STATE_OFF_PENDING;
        if (NULL == params && BT_STATUS_SUCCESS != bt_cm_prepare_power_deinit(false, g_bt_cm_edr_power_status)) {
            if (!bt_cm_timer_is_exist(BT_CM_FORCE_POWER_OFF_TIMER_ID)) {
                bt_cm_timer_start(BT_CM_FORCE_POWER_OFF_TIMER_ID, 10000, bt_cm_power_update, (void *)1);
            }
            return;
        }
        bt_cm_timer_stop(BT_CM_FORCE_POWER_OFF_TIMER_ID);
        if (BT_STATUS_SUCCESS == bt_cm_prepare_power_deinit(true, g_bt_cm_edr_power_status)) {
            return;
        }
        if (NULL == params) {
            bt_cm_timer_start(BT_CM_FORCE_POWER_OFF_TIMER_ID, 1, bt_cm_power_update, (void *)1);
            return;
        }
        if (0 != bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, NULL, 0)) {
            bt_cmgr_report_id("[BT_CM][POWER][I] there are still connection exit!!! ", 0);            
            bt_cm_timer_start(BT_CM_FORCE_POWER_OFF_TIMER_ID, 500, bt_cm_power_update, (void *)1);
            return;
        }
        g_bt_cm_edr_power_cnt.flags = 0;
        bt_cmgr_report_id("[BT_CM][POWER][I] BT POWER OFF cnf", 0);
        printf("TYM PowerOff E\r\n");
        bt_cm_power_off_cnf(g_bt_cm_edr_power_status);
        bt_cm_power_state_update_ind_t state = {
            .power_state = BT_CM_POWER_STATE_OFF
        };
        g_bt_cm_edr_power_cnt.cur_state = BT_CM_POWER_STATE_OFF;
        if (BT_DEVICE_MANAGER_POWER_STATE_RESTING != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
            bt_cm_event_callback(BT_CM_EVENT_POWER_STATE_UPDATE, &state, sizeof(state));
        }
        if (BT_DEVICE_MANAGER_POWER_STATUS_ROLE_RECOVERY != g_bt_cm_edr_power_status) {
            bt_cm_register_callback_notify(BT_CM_EVENT_POWER_STATE_UPDATE, &state, sizeof(state));
        }
        bt_device_manager_db_flush_all(BT_DEVICE_MANAGER_DB_FLUSH_BLOCK);
        bt_device_manager_dev_set_power_state(BT_DEVICE_TYPE_CLASSIC, BT_DEVICE_MANAGER_POWER_STATE_STANDBY);
    }
}

static bt_status_t  bt_cm_power_device_manager_callback(bt_device_manager_power_event_t evt, bt_device_manager_power_status_t status, void *data, uint32_t data_length)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    switch (evt) {
    case BT_DEVICE_MANAGER_POWER_EVT_PREPARE_ACTIVE: {
        bt_cmgr_report_id("[BT_CM][POWER][I] Prepare active", 0);
        if (BT_DEVICE_MANAGER_TEST_MODE_NONE == bt_device_manager_get_test_mode()) {
            g_bt_cm_edr_power_status = status;
            g_bt_cm_edr_power_cnt.target_state = BT_CM_POWER_STATE_ON;
            if (BT_CM_POWER_STATE_ON == g_bt_cm_edr_power_cnt.cur_state) {
                bt_cmgr_report_id("[BT_CM][POWER][I] Current power state is power on!", 0);
                return BT_STATUS_SUCCESS;            
            } else if (BT_CM_POWER_STATE_OFF == g_bt_cm_edr_power_cnt.cur_state) {
                bt_cm_timer_start(BT_CM_FORCE_POWER_OFF_TIMER_ID, 1, bt_cm_power_update, NULL);
            }
            return BT_STATUS_PENDING;
        } else {
            return BT_STATUS_SUCCESS;
        }
    }
    case BT_DEVICE_MANAGER_POWER_EVT_PREPARE_STANDBY: {
        bt_cmgr_report_id("[BT_CM][POWER][I] Prepare standby", 0);
        if(BT_CM_POWER_STATE_OFF == g_bt_cm_edr_power_cnt.cur_state) {
            bt_cmgr_report_id("[BT_CM][POWER][I] current power state is power off!", 0);
            return BT_STATUS_SUCCESS;
        }
        bt_cm_mutex_lock();
        g_bt_cm_edr_power_cnt.target_state = BT_CM_POWER_STATE_OFF;
        g_bt_cm_edr_power_status = status;
        if (BT_CM_POWER_STATE_ON == g_bt_cm_edr_power_cnt.cur_state) {
            bt_cmgr_report_id("[BT_CM][POWER][I] Standby reason is 0x%d!", 1, status);
#ifdef MTK_AWS_MCE_ENABLE
            if ((BT_DEVICE_MANAGER_POWER_STATUS_AIR_PAIRING_COMPLETE != status) && (BT_DEVICE_MANAGER_POWER_STATUS_AIR_PAIRING_START != status)) {
                bt_aws_mce_srv_air_pairing_stop();
            }
#endif
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_UNKNOW);
                bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
            } else {
                bt_cm_write_scan_mode_internal(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
            }
            g_bt_cm_edr_power_cnt.cur_state = BT_CM_POWER_STATE_OFF_PENDING;
            bt_cm_timer_start(BT_CM_FORCE_POWER_OFF_TIMER_ID, 1, bt_cm_power_update, NULL);
        }
        bt_cm_mutex_unlock();
        return BT_STATUS_PENDING;
    }
    default:
        break;
    }
    return ret;
}

#if 0
static void         bt_cm_power_test_sys_excute()
{
    return;
}

static bt_status_t  bt_cm_power_sys_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    return BT_STATUS_SUCCESS;
}
#endif

bt_status_t         bt_cm_power_active()
{
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_cm_power_standby(bool force)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_cm_power_reset(bool force)
{
    return BT_STATUS_SUCCESS;
}

bt_status_t         bt_cm_power_reset_ext(bool force, bt_cm_power_reset_callback_t cb, void* user_data)
{
    return BT_STATUS_SUCCESS;
}

void                bt_cm_power_init()
{
    bt_device_manager_dev_register_callback(BT_DEVICE_TYPE_CLASSIC, (bt_device_manager_power_callback_t)bt_cm_power_device_manager_callback);
}

void                bt_cm_power_deinit()
{
    bt_device_manager_dev_register_callback(BT_DEVICE_TYPE_CLASSIC, NULL);
}

//Just for UT/IT test.
void                bt_cm_power_test_sys(bt_cm_power_test_sys_t type)
{
    return;
}

void                bt_cm_power_standby_ready()
{
    return;
}

bt_status_t         default_bt_cm_prepare_standby_callback()
{
    return BT_STATUS_SUCCESS;
}

#endif /*#ifndef TEMP_CLASSIC_BT_OFF*/



