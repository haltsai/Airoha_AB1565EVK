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

#include "app_swift_pair_idle_activity.h"
#include "multi_ble_adv_manager.h"
#include "apps_events_event_group.h"
#include "apps_debug.h"
#include "apps_events_interaction_event.h"
#include "bt_device_manager.h"
#include "bt_customer_config.h"
#include "bt_hci.h"
#include "bt_gap.h"
#include "bt_gap_le.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#include "apps_events_bt_event.h"
#include "bt_app_common.h"
#include "assert.h"

#define LOG_TAG "[SWIFT_PAIR]"

#define APP_SWIFT_PAIR_TIMEOUT              (30 * 1000)
#define BLE_AD_TYPE_COMPLETE_LOCAL_NAME     0x09 //EIR_TYPE_COMPLETE_LOCAL_NAME 0x09

typedef struct {
        bool started;
} app_swift_context_t;

static app_swift_context_t g_app_swift_ctx = {0};

static void app_swift_pair_get_adv_data(bt_gap_le_set_ext_advertising_data_t *data)
{
    bt_bd_addr_t *addr = NULL;
    uint8_t name_length = 0;
    uint8_t max_name_len = 31 - 15;
    const bt_gap_config_t *cust_config = NULL;

    uint8_t data_context[31] = { 23, //Len
            0xFF, 0x06, 0x00, //Company <0x0006> Micorsoft
            0x03, 0x01,  //Type: Swift Pair Beacon
            0x80, // RSSI
            0x33, 0x33, 0x33, 0xFF, 0xFF, 0xFF,  //BD Addr
            0x18, 0x04, 0x20, // Cod[3] http://bluetooth-pentest.narod.ru/software/bluetooth_class_of_device-service_generator.html
            'A', 'B', '1','5','6','5','-','-' // Display Name
          };

    if (data == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG" get_adv_data data NULL", 0);
        assert(0);
    }

    addr = bt_device_manager_get_local_address();
    if (addr == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG" get_adv_data addr NULL", 0);
        assert(0);
    }
    /* little endian. */
    memcpy(&data_context[7], addr, 6);

    cust_config = bt_customer_config_get_gap_config();
    if (cust_config != NULL) {
        name_length = strlen(cust_config->device_name);
        if (name_length > max_name_len) {
            APPS_LOG_MSGID_W(LOG_TAG" name length too long %d", 1, name_length);
            name_length = max_name_len;
        }
        memcpy(&data_context[16], cust_config->device_name, name_length);
        data_context[0] = 15 + name_length;
    }

    APPS_LOG_MSGID_I(LOG_TAG" get adv data, addr %x:%x:%x:%x:%x:%x, len %d", 7,
            *addr[0], *addr[1], *addr[2], *addr[3], *addr[4], *addr[5], data_context[0]);

    data->data_length = data_context[0] + 1;
    data->fragment_preference = 0x00;

    memcpy(data->data, data_context, data->data_length);
}

static void app_swift_pair_get_adv_param(bt_hci_le_set_ext_advertising_parameters_t *adv_param)
{
    if (adv_param == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG" get_adv_param param NULL", 0);
        assert(0);
    }

    bt_app_common_generate_default_adv_data(adv_param, NULL, NULL, NULL, 0);

    adv_param->primary_advertising_interval_min = 0x0040;
    adv_param->primary_advertising_interval_max = 0x0040;

    memset(&(adv_param->peer_address), 0, sizeof(bt_addr_t));
}

static void app_swift_pair_get_scan_rsp(bt_gap_le_set_ext_scan_response_data_t *scan_rsp)
{
    uint8_t rsp[7] = {6, BLE_AD_TYPE_COMPLETE_LOCAL_NAME, 0x00, '-', 'M', 'S', 'P'};

    if (scan_rsp == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG" get_scan_rsp rsp NULL", 0);
        assert(0);
    }

    scan_rsp->data_length = 7;
    scan_rsp->fragment_preference = 0x00;

    memcpy(scan_rsp->data, rsp, scan_rsp->data_length);
}

static uint32_t app_swift_pair_adv_info(multi_ble_adv_info_t *adv_info)
{
    if (adv_info == NULL) {
        return 0;
    }
    if (adv_info->adv_data) {
        app_swift_pair_get_adv_data(adv_info->adv_data);
    }
    if (adv_info->adv_param) {
        app_swift_pair_get_adv_param(adv_info->adv_param);
    }
    if (adv_info->scan_rsp) {
        app_swift_pair_get_scan_rsp(adv_info->scan_rsp);
    }
    return 0;
}


bool app_swift_pair_start_adv(void)
{
    if (g_app_swift_ctx.started) {
        APPS_LOG_MSGID_W(LOG_TAG" already started",0);
        return true;
    }

    APPS_LOG_MSGID_I(LOG_TAG" start adv",0);

    if (multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_SWIFT_PAIR, app_swift_pair_adv_info, 1)) {
        /* Delay event timer. */
        g_app_swift_ctx.started = true;
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                APPS_EVENTS_INTERACTION_SWIFT_PAIR_TO, NULL, 0, NULL, APP_SWIFT_PAIR_TIMEOUT);
        multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_SWIFT_PAIR);
        return true;
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" add adv fail",0);
        return false;
    }
}

static bool app_swift_pair_stop_adv(void)
{
    if (!g_app_swift_ctx.started) {
        APPS_LOG_MSGID_W(LOG_TAG" not started",0);
        return true;
    }

    APPS_LOG_MSGID_I(LOG_TAG" stop adv",0);
    g_app_swift_ctx.started = false;
    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_SWIFT_PAIR, app_swift_pair_adv_info);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_SWIFT_PAIR);
    return true;
}

static bool app_swift_pair_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(LOG_TAG" create", 0);
            app_swift_pair_start_adv();
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool app_swift_pair_proc_interaction_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    switch (event_id) {
        case APPS_EVENTS_INTERACTION_SWIFT_PAIR_TO: {
            ret = true;
            APPS_LOG_MSGID_I(LOG_TAG" pairing timeout", 0);
            app_swift_pair_stop_adv();
            break;
        }
        default:
            break;
    }

    return ret;
}

static bool app_swift_pair_proc_bt_cm_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* Stop pair when connected. */
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == remote_update) {
                APPS_LOG_MSGID_E(LOG_TAG",BT_CM_EVENT_REMOTE_INFO_UPDATE param null", 0);
                break;
            }
#ifdef MTK_AWS_MCE_ENABLE
            bt_event_suffix_data_t *suffix_data = get_bt_event_suffix_data(extra_data, sizeof(bt_cm_remote_info_update_ind_t));
            role = suffix_data->aws_role;
#endif
#ifdef MTK_AWS_MCE_ENABLE
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role || BT_AWS_MCE_ROLE_FOLLOWER_1 == role)
#endif
            {
                if ((remote_update->connected_service & ~remote_update->pre_connected_service)
                    & (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL))) {
                    APPS_LOG_MSGID_I(LOG_TAG" Remote connected.", 0);
                    app_swift_pair_stop_adv();
                }
            }
            break;
        }
        default:
            break;
    }

    return ret;
}

bool app_swift_pair_idle_activity_proc(struct _ui_shell_activity *self,
                                         uint32_t event_group,
                                         uint32_t event_id,
                                         void *extra_data,
                                         size_t data_len)
{
    bool ret = false;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = app_swift_pair_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            ret = app_swift_pair_proc_interaction_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            ret = app_swift_pair_proc_bt_cm_group(self, event_id, extra_data, data_len);
            break;
        }
        default:
            break;
    }
    return ret;
}
