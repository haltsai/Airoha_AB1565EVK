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

#ifdef MTK_NVDM_ENABLE
#include "nvkey.h"
#include "nvkey_id_list.h"
#endif /* MTK_NVDM_ENABLE */
#include "bt_callback_manager.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_power.h"
#include "bt_debug.h"
#include "syslog.h"
#include "nvdm_id_list.h"
#include "bt_device_manager_test_mode.h"
#include "bt_system.h"

#if _MSC_VER >= 1500
#pragma comment(linker, "/alternatename:_bt_driver_set_dut_only_enable=_default_bt_driver_set_dut_only_enable")

#elif defined(__GNUC__) || defined(__ICCARM__) || defined(__ARMCC_VERSION)
#pragma weak bt_driver_set_dut_only_enable = default_bt_driver_set_dut_only_enable

#else
#error "Unsupported Platform"
#endif

void default_bt_driver_set_dut_only_enable(bool enalbe)
{

}

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

typedef bool bt_dm_dut_mode_config_t;

typedef struct {
    bool relay_enable;
    uint8_t relay_port;
} PACKED bt_dm_relay_mode_config_t;

typedef struct {
    bool in_use;
    bt_device_manager_test_mode_notify_callback_t cb;
} bt_dm_test_mode_notify_cb_node_t;

#define BT_DM_TEST_MODE_CALLBACK_MAX_NUMBER 0x03

static bt_dm_dut_mode_config_t dut_config = false;
static bt_dm_relay_mode_config_t relay_config = {false, 0};
static bt_device_manager_test_mode_t g_bt_dm_mode = BT_DEVICE_MANAGER_TEST_MODE_NONE;
static bt_device_manager_test_mode_dut_state_t dut_mode_state = BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DISABLED;
static bool bt_dm_test_mode_is_inited = false;
static bool bt_dm_dut_only_enable = false;
static bt_dm_test_mode_notify_cb_node_t cb_table[BT_DM_TEST_MODE_CALLBACK_MAX_NUMBER] = {
    {false, NULL},
    {false, NULL},
    {false, NULL}
};

static bt_status_t bt_device_manager_test_mode_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff);
static bt_status_t bt_device_manager_test_mode_callback_handler(bt_device_manager_test_mode_event_t event_id, void *param);
extern bt_status_t bt_gap_enter_test_mode(void);
extern bool bt_driver_enter_dut_mode(void);

void bt_device_manager_test_mode_init(void)
{
    bt_dmgr_report_id("[BT_DM][I]local info init", 0);
    if (!bt_dm_test_mode_is_inited) {
        nvkey_status_t ret = NVKEY_STATUS_ERROR;
        uint32_t relay_size = sizeof(bt_dm_relay_mode_config_t);
        uint32_t dut_size = sizeof(bt_dm_dut_mode_config_t);
        ret = nvkey_read_data(NVKEYID_BT_RELAY_ENABLE, (uint8_t *)(&relay_config), &relay_size);
        bt_dmgr_report_id("[BT_DM][I] ret:%d, relay_enable:%d, port_number:%d\r\n", 3, ret, relay_config.relay_enable, relay_config.relay_port);
        //bt_device_manager_assert(ret == NVKEY_STATUS_OK);
        ret = nvkey_read_data(NVKEYID_BT_DUT_ENABLE, (uint8_t *)(&dut_config), &dut_size);
        bt_dmgr_report_id("[BT_DM][I] ret:%d, dut_config:%d\r\n", 2, ret, dut_config);
        //bt_device_manager_assert(ret == NVKEY_STATUS_OK);
        if (relay_config.relay_enable && dut_config) {
            bt_device_manager_assert(0 && "enable both dut & relay mode");
        }
        bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SYSTEM, (void *)bt_device_manager_test_mode_event_handler);
        bt_dm_test_mode_is_inited = true;
    }
}

bt_status_t bt_device_manager_test_mode_register_callback(bt_device_manager_test_mode_notify_callback_t callback)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    uint8_t i = 0;
    bt_dmgr_report_id("[BT_DM][I]register test mode nofitfy callback", 0);

    if (callback == NULL) {
        return BT_STATUS_FAIL;
    }
    for (i = 0; i < BT_DM_TEST_MODE_CALLBACK_MAX_NUMBER; i++) {
        if (cb_table[i].in_use == false && cb_table[i].cb == NULL) {
            cb_table[i].in_use = true;
            cb_table[i].cb = callback;
            break;
        }
    }
    if (i == BT_DM_TEST_MODE_CALLBACK_MAX_NUMBER) {
        bt_dmgr_report_id("[BT_DM][I]register test mode nofitfy callback fail", 0);
        status = BT_STATUS_FAIL;
    }

    return status;
}

bt_status_t bt_device_manager_test_mode_deregister_callback(bt_device_manager_test_mode_notify_callback_t callback)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    uint8_t i = 0;
    bt_dmgr_report_id("[BT_DM][I]deregister test mode nofitfy callback", 0);

    if (callback == NULL) {
        return BT_STATUS_FAIL;
    }
    for (i = 0; i < BT_DM_TEST_MODE_CALLBACK_MAX_NUMBER; i++) {
        if (cb_table[i].in_use == true && cb_table[i].cb == callback) {
            cb_table[i].in_use = false;
            cb_table[i].cb = NULL;
            break;
        }
    }
    if (i == BT_DM_TEST_MODE_CALLBACK_MAX_NUMBER) {
        bt_dmgr_report_id("[BT_DM][I]deregister test mode nofitfy callback fail", 0);
        status = BT_STATUS_FAIL;
    }

    return status;
}

static bt_status_t bt_device_manager_test_mode_callback_handler(bt_device_manager_test_mode_event_t event_id, void *param)
{
    bt_status_t status = BT_STATUS_SUCCESS;
    uint8_t i = 0;
    bt_dmgr_report_id("[BT_DM][I]bt_device_manager_test_mode_callback_handler, evt_id=%d", 1, event_id);
    if (param == NULL) {
        return BT_STATUS_FAIL;
    }

    switch (event_id) {
        case BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND: {
            for (i = 0; i < BT_DM_TEST_MODE_CALLBACK_MAX_NUMBER; i++) {
                if (cb_table[i].in_use && cb_table[i].cb != NULL) {
                    cb_table[i].cb(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, param);
                }
            }
            break;
        }
        default: {
            break;
        }
    }
    return status;
}

static void bt_device_manager_test_mode_set_dut_state(bt_device_manager_test_mode_dut_state_t state)
{
    bt_dmgr_report_id("[BT_DM][I]Set dut mode state = %d, last state = %d", 2, state, dut_mode_state);
    dut_mode_state = state;
}

bt_device_manager_test_mode_dut_state_t bt_device_manager_test_mode_get_dut_state(void)
{
    bt_dmgr_report_id("[BT_DM][I]Get dut mode state = %d", 1, dut_mode_state);
    return dut_mode_state;
}

bt_status_t bt_device_manager_set_test_mode(bt_device_manager_test_mode_t mode)
{
    bt_dmgr_report_id("[BT_DM][I]Set device mode = %d, last mode = %d", 2, mode, g_bt_dm_mode);
    bt_status_t status = BT_STATUS_SUCCESS;

    switch (mode) {
        case BT_DEVICE_MANAGER_TEST_MODE_NONE: {
            break;
        }
        case BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX: {
            dut_config = true;
            bt_device_manager_power_state_t classic_power_state = bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC);
            bt_device_manager_power_state_t ble_power_state = bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_LE);
            if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == classic_power_state && BT_DEVICE_MANAGER_POWER_STATE_STANDBY == ble_power_state) {
                status = bt_device_manager_power_active(BT_DEVICE_TYPE_CLASSIC | BT_DEVICE_TYPE_LE);
            } else if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == classic_power_state && BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == ble_power_state) {
                status = bt_device_manager_power_reset(BT_DEVICE_MANAGER_POWER_RESET_TYPE_NORMAL, NULL, NULL);
            } else {
                status = BT_STATUS_FAIL;
                bt_dmgr_report_id("[BT_DM][I]Set device test mode fail, current classic power state:%d, ble power state:%d", 2, classic_power_state, ble_power_state);
            }
            break;
        }
        case BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY: {
            bt_device_manager_power_state_t classic_power_state = bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC);
            bt_device_manager_power_state_t ble_power_state = bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_LE);
            if (BT_DEVICE_MANAGER_POWER_STATE_STANDBY == classic_power_state && BT_DEVICE_MANAGER_POWER_STATE_STANDBY == ble_power_state) {
                bool ret = bt_driver_enter_dut_mode();
                bt_device_manager_assert(ret);
            } else if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == classic_power_state && BT_DEVICE_MANAGER_POWER_STATE_ACTIVE == ble_power_state) {
                bt_dm_dut_only_enable = true;
                status = bt_device_manager_power_standby(BT_DEVICE_TYPE_LE | BT_DEVICE_TYPE_CLASSIC);
            } else {
                status = BT_STATUS_FAIL;
                bt_dmgr_report_id("[BT_DM][I]Set device test mode fail, current classic power state:%d, ble power state:%d", 2, classic_power_state, ble_power_state);
            }
            break;
        }
        case BT_DEVICE_MANAGER_TEST_MODE_RELAY: {
            break;
        }
        case BT_DEVICE_MANAGER_TEST_MODE_COMMAND: {
            break;
        }
        default: {
            break;
        }
    }

    return status;
}

bt_device_manager_test_mode_t bt_device_manager_get_test_mode(void)
{
    bt_dmgr_report_id("[BT_DM][I]Get device mode = %d", 1, g_bt_dm_mode);
    return g_bt_dm_mode;
}

static bt_status_t bt_device_manager_test_mode_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    bt_dmgr_report_id("[BT_DM][I] bt_device_manager_test_mode_event_handler, msg = 0x%8x, status = 0x%08x", 2, msg, status);
    bt_device_manager_test_mode_changed_ind_t mode_change_ind = {0};
    bt_status_t result = BT_STATUS_SUCCESS;
    switch (msg) {
        case BT_TEST_MODE_NONE_IND: {
            g_bt_dm_mode = BT_DEVICE_MANAGER_TEST_MODE_NONE;
            bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DISABLED);
            if (dut_config) {
                dut_config = false;
            }
            memset(&relay_config, 0, sizeof(bt_dm_relay_mode_config_t));
            bt_dm_dut_only_enable = false;
            mode_change_ind.mode = BT_DEVICE_MANAGER_TEST_MODE_NONE;
            bt_device_manager_test_mode_callback_handler(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, (void *)&mode_change_ind);
            break;
        }

        case BT_TEST_MODE_DUT_MIX_ENABLE_IND: {
            if (status == BT_STATUS_SUCCESS) {
                g_bt_dm_mode = BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX;
                bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED);
                mode_change_ind.status = status;
                mode_change_ind.mode = g_bt_dm_mode;
                mode_change_ind.dut_state = bt_device_manager_test_mode_get_dut_state();
                bt_device_manager_test_mode_callback_handler(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, (void *)&mode_change_ind);
            } else {
                bt_dmgr_report_id("[BT_DM][I] bt device manager test mode enable dut mix failed", 0);
            }
            break;
        }

        case BT_TEST_MODE_DUT_ONLY_ENABLE_IND: {
            if (status == BT_STATUS_SUCCESS) {
                g_bt_dm_mode = BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY;
                bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_ONLY_ENABLED);
                mode_change_ind.status = status;
                mode_change_ind.mode = g_bt_dm_mode;
                mode_change_ind.dut_state = bt_device_manager_test_mode_get_dut_state();
                bt_device_manager_test_mode_callback_handler(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, (void *)&mode_change_ind);
            } else {
                bt_dmgr_report_id("[BT_DM][I] bt device manager test mode enable dut only failed", 0);
            }
            break;
        }

        case BT_TEST_MODE_DUT_ACTIVE_IND: {
            bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_ACTIVE);
            mode_change_ind.status = status;
            mode_change_ind.mode = g_bt_dm_mode;
            mode_change_ind.dut_state = bt_device_manager_test_mode_get_dut_state();
            bt_device_manager_test_mode_callback_handler(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, (void *)&mode_change_ind);
            break;
        }

        case BT_TEST_MODE_DUT_INACTIVE_IND: {
            if (BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == g_bt_dm_mode) {
                bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED);
            } else if (BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY == g_bt_dm_mode) {
                bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_ONLY_ENABLED);
            }
            //bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_ACTIVE);
            mode_change_ind.status = status;
            mode_change_ind.mode = g_bt_dm_mode;
            mode_change_ind.dut_state = bt_device_manager_test_mode_get_dut_state();
            bt_device_manager_test_mode_callback_handler(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, (void *)&mode_change_ind);
            break;
        }

        case BT_TEST_MODE_RELAY_ENABLE_IND: {
            g_bt_dm_mode = BT_DEVICE_MANAGER_TEST_MODE_RELAY;
            mode_change_ind.status = status;
            mode_change_ind.mode = g_bt_dm_mode;
            bt_device_manager_test_mode_callback_handler(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, (void *)&mode_change_ind);
            break;
        }

        case BT_TEST_MODE_COMMAND_ENABLE_IND: {
            g_bt_dm_mode = BT_DEVICE_MANAGER_TEST_MODE_COMMAND;
            mode_change_ind.status = status;
            mode_change_ind.mode = g_bt_dm_mode;
            bt_device_manager_test_mode_callback_handler(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, (void *)&mode_change_ind);
            break;
        }

        case BT_POWER_ON_CNF: {
            if (dut_config) {
                result = bt_gap_enter_test_mode();
                g_bt_dm_mode = BT_DEVICE_MANAGER_TEST_MODE_DUT_ONLY;
                bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_ONLY_ENABLED);
            }
            break;
        }

        case BT_POWER_OFF_CNF: {
            if (g_bt_dm_mode != BT_DEVICE_MANAGER_TEST_MODE_NONE) {
                g_bt_dm_mode = BT_DEVICE_MANAGER_TEST_MODE_NONE;
                bt_device_manager_test_mode_set_dut_state(BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DISABLED);
                if (dut_config) {
                    dut_config = false;
                }
                memset(&relay_config, 0, sizeof(bt_dm_relay_mode_config_t));
                mode_change_ind.status = status;
                mode_change_ind.mode = g_bt_dm_mode;
                bt_device_manager_test_mode_callback_handler(BT_DEVICE_MANAGER_TEST_MODE_CHANGED_IND, (void *)&mode_change_ind);
            }
            if (bt_dm_dut_only_enable) {
                bool ret = bt_driver_enter_dut_mode();
                bt_device_manager_assert(ret);
            }
            break;
        }

        default: {
            break;
        }
    }

    return result;
}


