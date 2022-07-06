
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

/**
 * File: app_va_xiaoai_ble_adv.c
 *
 * Description: This file provides XiaoAI BLE ADV API for XiaoAI activity.
 *
 */

#ifdef MTK_VA_XIAOAI_ENABLE

#include "app_va_xiaoai_ble_adv.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "bt_app_common.h"
#include "ui_shell_manager.h"
#include "multi_va_manager.h"


#include "xiaoai.h"

#define APP_VA_XIAOAI_BLEADV_TAG       "[XIAOAI_VA]"

/**----------------------------------------------------------------------------**/
/**                                  Multi-BLE ADV                             **/
/**----------------------------------------------------------------------------**/
static uint16_t g_xiaoai_ble_adv_interval = 62;     /* Default 62 ms. */

static uint32_t app_va_xiaoai_get_ble_adv_cb(multi_ble_adv_info_t *xiaoai_ble_adv_info)
{
    bt_hci_cmd_le_set_advertising_parameters_t adv_param = {0};
    bt_hci_cmd_le_set_advertising_data_t adv_data = {0};
    bt_hci_cmd_le_set_scan_response_data_t scan_rsp = {0};
    xiaoai_set_ble_adv_info(g_xiaoai_ble_adv_interval, &adv_param, &adv_data, &scan_rsp);

    /* for new BLE ADV parameter. */
    if (xiaoai_ble_adv_info->adv_param != NULL) {
        bt_app_common_generate_default_adv_data(xiaoai_ble_adv_info->adv_param, NULL, NULL, NULL, 0);
        xiaoai_ble_adv_info->adv_param->primary_advertising_interval_min = adv_param.advertising_interval_min;
        xiaoai_ble_adv_info->adv_param->primary_advertising_interval_max = adv_param.advertising_interval_max;
        xiaoai_ble_adv_info->adv_param->own_address_type = adv_param.own_address_type;
        if (adv_param.advertising_type == BT_HCI_ADV_TYPE_NON_CONNECTABLE_UNDIRECTED) {
            xiaoai_ble_adv_info->adv_param->advertising_event_properties &= ~BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE;
            APPS_LOG_MSGID_I(APP_VA_XIAOAI_BLEADV_TAG" [BLE_ADV][XIAOAI_ADV] non_connectable", 0);
        }
    }
    /* for new BLE ADV data. */
    if (xiaoai_ble_adv_info->adv_data != NULL) {
        xiaoai_ble_adv_info->adv_data->data_length = adv_data.advertising_data_length;
        memcpy(xiaoai_ble_adv_info->adv_data->data, adv_data.advertising_data, adv_data.advertising_data_length);
        xiaoai_ble_adv_info->adv_data->fragment_preference = 0;
    }
    /* for new BLE ADV scan response. */
    if (xiaoai_ble_adv_info->scan_rsp != NULL) {
        xiaoai_ble_adv_info->scan_rsp->data_length = scan_rsp.scan_response_data_length;
        memcpy(xiaoai_ble_adv_info->scan_rsp->data, scan_rsp.scan_response_data, scan_rsp.scan_response_data_length);
        xiaoai_ble_adv_info->scan_rsp->fragment_preference = 0;
    }
    return 0;
}

void app_va_xiaoai_send_ble_adv_event(bool enable, uint16_t adv_interval_ms, int adv_time_ms)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_BLEADV_TAG" [BLE_ADV] send_ble_adv_event enable=%d adv_interval_ms=%d adv_time_ms=%d",
                     3, enable, adv_interval_ms, adv_time_ms);
    /* Remove current pending XiaoAI BLE ADV event. */
    ui_shell_status_t status = ui_shell_remove_event(EVENT_GROUP_UI_SHELL_XIAOAI,
                                                     XIAOAI_EVENT_START_BLE_ADV_ACTION);
    status += ui_shell_remove_event(EVENT_GROUP_UI_SHELL_XIAOAI,
                                    XIAOAI_EVENT_STOP_BLE_ADV_ACTION);
    if (enable) {
        /* Start BLE ADV with adv_interval_ms after 100ms. */
        status += ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                                      EVENT_GROUP_UI_SHELL_XIAOAI,
                                      XIAOAI_EVENT_START_BLE_ADV_ACTION,
                                      (void *)(uint32_t)adv_interval_ms, 0, NULL, 100);
        if (adv_time_ms > 0) {
            /* Stop BLE ADV after adv_time_ms. */
            status += ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                                          EVENT_GROUP_UI_SHELL_XIAOAI,
                                          XIAOAI_EVENT_STOP_BLE_ADV_ACTION,
                                          NULL, 0, NULL, (uint32_t)adv_time_ms);
        }
    } else {
        /* Stop BLE ADV immediately. */
        status += ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                                      EVENT_GROUP_UI_SHELL_XIAOAI,
                                      XIAOAI_EVENT_STOP_BLE_ADV_ACTION,
                                      NULL, 0, NULL, 0);
    }
    if (status != UI_SHELL_STATUS_OK) {
        APPS_LOG_MSGID_E(APP_VA_XIAOAI_BLEADV_TAG" [BLE_ADV] send_ble_adv_event Error status=%d",
                         1, status);
    }
}

bool app_va_xiaoai_start_ble_adv(bool enable, uint16_t advertising_interval)
{
    bool ret = FALSE;
    if (enable) {
        g_xiaoai_ble_adv_interval = advertising_interval;
        ret = multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_XIAOAI, app_va_xiaoai_get_ble_adv_cb);
        ret += multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_XIAOAI, app_va_xiaoai_get_ble_adv_cb, 1);
    } else {
        g_xiaoai_ble_adv_interval = 0;
        ret = multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_XIAOAI, app_va_xiaoai_get_ble_adv_cb);
    }
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_XIAOAI);
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_BLEADV_TAG" [BLE_ADV] start_ble_adv enable=%d interval=%d ret=%d",
                     3, enable, advertising_interval, ret);
    return ret;
}



/**----------------------------------------------------------------------------**/
/**                         Multi-VA (XiaoAI Not Register)                     **/
/**----------------------------------------------------------------------------**/
#ifdef MTK_VA_XIAOAI_MULTI_VA_ENABLE
static uint32_t app_va_xiaoai_gen_adv_data_info(multi_ble_adv_info_t *adv_info)
{
    app_va_xiaoai_get_ble_adv_cb(adv_info);
    return 0;
}

static multi_va_switch_off_return_t app_va_xiaoai_assistant_switched(bool selected)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_BLEADV_TAG" [Multi-VA] switch selected=%d", 1, selected);
    if (selected) {
        return MULTI_VA_SWITCH_OFF_SET_INACTIVE_DONE;
    } else {
        // disconnect & disable XiaoAI
        return MULTI_VA_SWITCH_OFF_WAIT_INACTIVE;
    }
}

static void app_va_xiaoai_null_init(bool selected)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_BLEADV_TAG" [Multi-VA] init selected=%d", 1, selected);
}

void app_va_xiaoai_multi_support_register(void)
{
    multi_va_manager_callbacks_t callback;
    callback.on_get_ble_adv_data = app_va_xiaoai_gen_adv_data_info;
    callback.on_voice_assistant_type_switch = app_va_xiaoai_assistant_switched;
    callback.voice_assistant_initialize = app_va_xiaoai_null_init;
    multi_voice_assistant_manager_register_instance(MULTI_VA_TYPE_XIAOAI, &callback);
}

void app_va_xiaoai_multi_va_notify_disconnected(bool xiaoai_disable)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_BLEADV_TAG" [Multi-VA] notify_disconnected, xiaoai_disable=%d", 1, xiaoai_disable);
    if (xiaoai_disable) {
        multi_voice_assistant_manager_set_inactive_done(MULTI_VA_TYPE_XIAOAI);
    }
    multi_voice_assistant_manager_notify_va_disconnected(MULTI_VA_TYPE_XIAOAI);
}

void app_va_xiaoai_multi_va_notify_connected(void)
{
    multi_va_type_t old_va_type = multi_va_manager_get_current_va_type();
    APPS_LOG_MSGID_I(APP_VA_XIAOAI_BLEADV_TAG" [Multi-VA] notify_connected, old_va_type=%d", 1, old_va_type);
    multi_voice_assistant_manager_notify_va_connected(MULTI_VA_TYPE_XIAOAI);
}
#endif

#endif /* MTK_VA_XIAOAI_ENABLE */
