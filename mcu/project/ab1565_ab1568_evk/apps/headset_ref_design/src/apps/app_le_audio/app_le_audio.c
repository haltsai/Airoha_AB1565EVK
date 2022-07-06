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
#ifdef AIR_LE_AUDIO_ENABLE
#include "app_le_audio.h"
#include "app_le_audio_aird_client.h"

#include "ble_csis.h"
#include "ble_pacs.h"
#include "ble_bass_def.h"
#include "ble_ascs_def.h"
#include "ble_cas_def.h"

#include "bt_type.h"
#include "bt_gap_le_audio.h"

#include "bt_sink_srv.h"
#include "bt_sink_srv_le.h"
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "bt_le_audio_sink.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_srv.h"
#include "apps_aws_sync_event.h"
#endif
#include "apps_debug.h"

#include "multi_ble_adv_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_events_bt_event.h"
#include "apps_config_features_dynamic_setting.h"
#include "app_le_audio_hfp_at_cmd.h"
#include "bt_callback_manager.h"
#include "bt_connection_manager.h"
#include "bt_customer_config.h"
#include "bt_device_manager.h"
#include "bt_init.h"
#include "ui_shell_manager.h"
#include "bt_aws_mce_role_recovery.h"
#include "bt_gattc.h"

#include "bt_le_audio_msglog.h"

#include "nvkey.h"
#include "nvkey_id_list.h"

#include "app_ccp.h"
#include "app_mcp.h"

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"
#endif
#ifdef AIR_MULTI_POINT_ENABLE
#include "app_bt_emp_service.h"
#endif
#ifdef MTK_SMART_CHARGER_ENABLE
#include "app_smcharger_utils.h"
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
#include "app_le_audio_bis.h"
#endif
#if defined(AIR_LE_AUDIO_ENABLE) && (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))
#include "app_bt_conn_manager.h"
#include "bt_sink_srv_ami.h"
#endif


/**************************************************************************************************
* Define
**************************************************************************************************/
#define LOG_TAG     "[LE_AUDIO_ACT]"

/* LE Audio feature state */
#define APP_LE_AUDIO_FEATURE_DISABLE    0
#define APP_LE_AUDIO_FEATURE_ENABLE     1

#define APP_LE_AUDIO_ADV_INTERVAL_MIN_S 0x0020  /* 20 ms */
#define APP_LE_AUDIO_ADV_INTERVAL_MAX_S 0x0030  /* 30 ms */
#define APP_LE_AUDIO_ADV_INTERVAL_MIN_L 0x00A0  /* 100 ms */
#define APP_LE_AUDIO_ADV_INTERVAL_MAX_L 0x00A0  /* 100 ms */

#define APP_LE_AUDIO_RESET_DEVICE_BUSY_TIMEOUT    (1000)  /* unit: ms */

/**************************************************************************************************
* Enum
**************************************************************************************************/
typedef enum {
    APP_LE_AUDIO_LINE_OR_USB_NONE,
    APP_LE_AUDIO_LINE_IN,
    APP_LE_AUDIO_LINE_OUT,
    APP_LE_AUDIO_USB_IN,
    APP_LE_AUDIO_USB_OUT
} app_le_audio_line_or_usb_in_state_t;

/**************************************************************************************************
* Structure
**************************************************************************************************/
typedef struct {
    bt_handle_t handle;
    uint8_t discover_gtbs:1;
    uint8_t discover_gmcs:1;
    uint8_t support_gtbs:1;
    uint8_t support_gmcs:1;
    uint8_t reserved:4;
} app_le_audio_ctrl_t;

/**************************************************************************************************
* Variable
**************************************************************************************************/
#ifdef AIR_LE_AUDIO_CIS_ENABLE
static bool lea_local_cis_connected = FALSE;
#ifdef MTK_AWS_MCE_ENABLE
static bool lea_peer_cis_connected = FALSE;
#endif
#endif

static app_le_audio_line_or_usb_in_state_t s_app_le_audio_line_or_usb_in_state = APP_LE_AUDIO_LINE_OR_USB_NONE;

static uint8_t g_le_audio_feature_state = APP_LE_AUDIO_FEATURE_ENABLE;

static app_le_audio_ctrl_t g_le_audio_ctrl[APP_LE_AUDIO_MAX_LINK_NUM];

#if (APP_LE_AUDIO_VISIBILITY_TYPE == 1)
static bool s_le_audio_adv_visible = FALSE;
#else
static bool s_le_audio_adv_visible = TRUE;
#endif
static bool s_le_audio_adv_enabled = FALSE;
static uint32_t s_le_audio_adv_timeout = APP_LE_AUDIO_ADV_TIME;   /* 0 - always */
static uint16_t s_le_audio_adv_interval_min = APP_LE_AUDIO_ADV_INTERVAL_MIN_S;
static uint16_t s_le_audio_adv_interval_max = APP_LE_AUDIO_ADV_INTERVAL_MAX_S;
extern bool g_le_audio_aird_device_suspend_flag;
static TimerHandle_t g_lea_timer = NULL;

/**************************************************************************************************
* Prototype
**************************************************************************************************/
extern uint8_t bt_le_audio_sink_get_adv_handle(void);

extern bt_status_t bt_gatts_service_set_le_audio_device_name(const uint8_t *device_name, uint16_t length);

extern const uint8_t *bt_gatts_service_get_le_audio_device_name(void);

extern void bt_app_common_sync_le_audio_info(void);

/**************************************************************************************************
* Static Functions
**************************************************************************************************/
static void app_le_audio_get_scan_response(bt_gap_le_set_ext_scan_response_data_t *scan_rsp)
{
    uint16_t device_name_len = 0;

    char device_name[BT_GAP_LE_MAX_DEVICE_NAME_LENGTH] = {0};
    bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();

    snprintf(device_name, BT_GAP_LE_MAX_DEVICE_NAME_LENGTH, "LEA-H_%.2X%.2X%.2X%.2X%.2X%.2X",
             (*local_addr)[5], (*local_addr)[4], (*local_addr)[3],
             (*local_addr)[2], (*local_addr)[1], (*local_addr)[0]);

    device_name_len = strlen((char *)device_name);

    /* scan_rsp: AD_TYPE_NAME_COMPLETE */
    scan_rsp->data[0] = device_name_len + 1;
    scan_rsp->data[1] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
    memcpy(&scan_rsp->data[2], device_name, device_name_len);
    scan_rsp->data_length = device_name_len + 2;

    LE_AUDIO_MSGLOG_I("[APP] get_scan_response, device_name_len:%x", 1, device_name_len);

    /* set GATT GAP service device name */
    bt_gatts_service_set_le_audio_device_name((const uint8_t *)device_name, device_name_len);
}

static uint32_t app_le_audio_get_adv_data(multi_ble_adv_info_t *adv_data)
{
    //LE_AUDIO_MSGLOG_I(LOG_TAG"[APP] app_le_audio_get_adv_data", 0);

    /* SCAN RSP */
    if (NULL != adv_data->scan_rsp) {
#if 0
        app_le_audio_get_scan_response(adv_data->scan_rsp);
#else
        adv_data->scan_rsp->data_length = 0;
#endif
    }

    /* ADV DATA */
    if ((NULL != adv_data->adv_data) && (NULL != adv_data->adv_data->data)) {
        uint16_t sink_conent, source_conent;
        uint8_t rsi[6];
        uint8_t len = 0;

        adv_data->adv_data->data[len] = 2;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_FLAG;
        adv_data->adv_data->data[len + 2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;
        len += 3;

        /* adv_data: RSI */
        adv_data->adv_data->data[len] = 7;
        adv_data->adv_data->data[len + 1] = 0x2E;
        ble_csis_get_rsi(rsi);
        memcpy(&adv_data->adv_data->data[len + 2], rsi, sizeof(rsi));
        len += 8;

        /* adv_data: AD_TYPE_SERVICE_DATA (BAP)*/
        adv_data->adv_data->data[len] = 9;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_SERVICE_DATA;
        adv_data->adv_data->data[len + 2] = (BT_GATT_UUID16_ASCS_SERVICE & 0x00FF);
        adv_data->adv_data->data[len + 3] = ((BT_GATT_UUID16_ASCS_SERVICE & 0xFF00) >> 8);
        adv_data->adv_data->data[len + 4] = ANNOUNCEMENT_TYPE_GENERAL;
        ble_pacs_get_available_audio_contexts(&sink_conent, &source_conent);
        memcpy(&adv_data->adv_data->data[len + 5], &sink_conent, 2);
        memcpy(&adv_data->adv_data->data[len + 7], &source_conent, 2);
        adv_data->adv_data->data[len + 9] = 0x00; /* Length of the Metadata field = 0 */
        len += 10;

        /* adv_data: TX_POWER (BAP)*/
        adv_data->adv_data->data[len] = 2;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_TX_POWER;
        adv_data->adv_data->data[len + 2] = 0x7F;
        len += 3;

        /* adv_data: AD_TYPE_APPEARANCE (TMAP) */
        adv_data->adv_data->data[len] = 3;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_APPEARANCE;
        /* value: 2 bytes */
        adv_data->adv_data->data[len + 2] = 0x41;
        adv_data->adv_data->data[len + 3] = 0x09;
        len += 4;

        /* adv_data: AD_TYPE_SERVICE_DATA (BASS)*/
        adv_data->adv_data->data[len] = 3;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_SERVICE_DATA;
        /* BASS UUID: 2 bytes */
        adv_data->adv_data->data[len + 2] = (BT_SIG_UUID16_BASS & 0x00FF);
        adv_data->adv_data->data[len + 3] = ((BT_SIG_UUID16_BASS & 0xFF00) >> 8);
        len += 4;

        /* adv_data: AD_TYPE_SERVICE_DATA (CAS)*/
        adv_data->adv_data->data[len] = 4;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_SERVICE_DATA;
        /* CAS UUID: 2 bytes */
        adv_data->adv_data->data[len + 2] = (BT_SIG_UUID16_CAS & 0x00FF);
        adv_data->adv_data->data[len + 3] = ((BT_SIG_UUID16_CAS & 0xFF00) >> 8);
        adv_data->adv_data->data[len + 4] = ANNOUNCEMENT_TYPE_GENERAL;
        len += 5;

        uint16_t device_name_len = 0;

        char device_name[BT_GAP_LE_MAX_DEVICE_NAME_LENGTH] = {0};
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();

        snprintf(device_name, BT_GAP_LE_MAX_DEVICE_NAME_LENGTH, "LEA-H_%.2X%.2X%.2X%.2X%.2X%.2X",
                 (*local_addr)[5], (*local_addr)[4], (*local_addr)[3],
                 (*local_addr)[2], (*local_addr)[1], (*local_addr)[0]);

        device_name_len = strlen((char *)device_name);

        /* scan_rsp: AD_TYPE_NAME_COMPLETE*/
        adv_data->adv_data->data[len] = device_name_len + 1;
        adv_data->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
        memcpy(&adv_data->adv_data->data[len + 2] , device_name, device_name_len);

        /*set GASTT GAP service device name*/
        bt_gatts_service_set_le_audio_device_name((const uint8_t *)device_name, device_name_len);

        adv_data->adv_data->data_length = len + 2 + device_name_len;

        LE_AUDIO_MSGLOG_I("[APP] data_length:%d", 1, adv_data->adv_data->data_length);
    }

    if (NULL != adv_data->adv_param) {
        /*
        bt_hci_le_set_ext_advertising_parameters_t ext_adv_param = {
            .advertising_event_properties = BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE | BT_HCI_ADV_EVT_PROPERTIES_MASK_SCANNABLE | BT_HCI_ADV_EVT_PROPERTIES_MASK_LEGACY_PDU,
            .primary_advertising_interval_min = 0x0020,
            .primary_advertising_interval_max = 0x0030,
            .primary_advertising_channel_map = 7,
            .own_address_type = BT_ADDR_RANDOM,
            .advertising_filter_policy = 0,
            .advertising_tx_power = TX_POWER_VALUE,
            .primary_advertising_phy = BT_HCI_LE_ADV_PHY_1M,
            .secondary_advertising_phy = BT_HCI_LE_ADV_PHY_1M,
        };
        */

        adv_data->adv_param->advertising_event_properties = BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE;
                                                  //| BT_HCI_ADV_EVT_PROPERTIES_MASK_SCANNABLE
                                                  //| BT_HCI_ADV_EVT_PROPERTIES_MASK_LEGACY_PDU;
        /* Interval should be no larger than 100ms when discoverable */
        adv_data->adv_param->primary_advertising_interval_min = s_le_audio_adv_interval_min;
        adv_data->adv_param->primary_advertising_interval_max = s_le_audio_adv_interval_max;
        adv_data->adv_param->primary_advertising_channel_map = 0x07;
        adv_data->adv_param->own_address_type = BT_ADDR_RANDOM;
        adv_data->adv_param->advertising_filter_policy = 0;
        adv_data->adv_param->advertising_tx_power = 0x7F;
        adv_data->adv_param->primary_advertising_phy = BT_HCI_LE_ADV_PHY_1M;
        adv_data->adv_param->secondary_advertising_phy = BT_HCI_LE_ADV_PHY_1M;
    }
    return 0;
}

static bool app_le_audio_is_ever_connected(void)
{
    uint32_t size = 0;
    nvkey_status_t status = nvkey_data_item_length(NVKEYID_APP_LE_AUDIO_CONNECTED_FLAG, &size);
    if (status != NVKEY_STATUS_OK || size == 0) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] is_ever_connected, nvkey fail", 0);
        return FALSE;
    }

    uint8_t  data = 0;
    uint32_t data_len = size;
    status = nvkey_read_data(NVKEYID_APP_LE_AUDIO_CONNECTED_FLAG, &data, &data_len);
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] is_ever_connected, status=%d data=%d len=%d", 3,
                        status, data, data_len);
    return (status == NVKEY_STATUS_OK && data == 1 && data_len == 1);
}

static void app_le_audio_set_connected_flag(void)
{
    bool flag = app_le_audio_is_ever_connected();
    if (!flag) {
        uint8_t data = 1;
        uint32_t data_len = 1;
        nvkey_status_t status = nvkey_write_data(NVKEYID_APP_LE_AUDIO_CONNECTED_FLAG, &data, data_len);
        APPS_LOG_MSGID_I(LOG_TAG"[LEA] set_connected_flag, status=%d", 1, status);
    }
}

static bt_status_t app_le_audio_disconnect_by_handle(bt_handle_t handle, bt_hci_disconnect_reason_t reason)
{
    bt_status_t status = 0;
    bt_hci_cmd_disconnect_t disconnect_para;

    disconnect_para.connection_handle = handle;
    disconnect_para.reason = reason;
    status = bt_gap_le_disconnect(&disconnect_para);
    return status;
}

uint8_t app_le_audio_get_link_index(bt_handle_t handle)
{
    uint8_t idx;
    for (idx = 0; idx < APP_LE_AUDIO_MAX_LINK_NUM; idx++) {
        if (handle == g_le_audio_ctrl[idx].handle) {
            break;
        }
    }
    return idx;
}

bt_handle_t app_le_audio_get_handle(uint8_t link_idx)
{
    return g_le_audio_ctrl[link_idx].handle;
}

static uint8_t app_le_audio_get_link_count(void)
{
    uint8_t link_count = 0;
    for (uint8_t idx = 0; idx < APP_LE_AUDIO_MAX_LINK_NUM; idx++) {
        if (BT_HANDLE_INVALID != g_le_audio_ctrl[idx].handle) {
            link_count++;
        }
    }
    return link_count;
}

static void app_le_audio_set_connection(bt_handle_t handle)
{
    for (uint8_t idx = 0; idx < APP_LE_AUDIO_MAX_LINK_NUM; idx++) {
        if (BT_HANDLE_INVALID == g_le_audio_ctrl[idx].handle) {
            memset(&g_le_audio_ctrl[idx], 0, sizeof(app_le_audio_ctrl_t));
            g_le_audio_ctrl[idx].handle = handle;
            break;
        }
    }
}

static void app_le_audio_reset_connection(void)
{
    for (uint8_t idx = 0; idx < APP_LE_AUDIO_MAX_LINK_NUM; idx++) {
        if (BT_HANDLE_INVALID != g_le_audio_ctrl[idx].handle) {
            memset(&g_le_audio_ctrl[idx], 0, sizeof(app_le_audio_ctrl_t));
            g_le_audio_ctrl[idx].handle = BT_HANDLE_INVALID;
        }
        app_le_audio_aird_client_reset_info(idx);
    }
}

#if 0
static bool app_le_audio_is_connected(void)
{
    return (g_le_audio_ctrl.handle != BT_HANDLE_INVALID);
}
#endif

static void app_le_audio_do_start_advertising(uint32_t timeout)
{
    /* Start ADV only in UI task */
    if (g_le_audio_feature_state == APP_LE_AUDIO_FEATURE_DISABLE) {
        APPS_LOG_MSGID_I(LOG_TAG"[LEA] do_start_advertising fail, LC3 disabled", 0);
        return;
    }

    if (apps_config_features_is_mp_test_mode()) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_start_advertising fail, MP TEST Mode", 0);
        return;
    }

#if defined(AIR_LE_AUDIO_ENABLE) && (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))
    if (!app_bt_conn_manager_allow_le_audio()) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_start_advertising fail, app_bt_conn_manager disallow", 0);
        return;
    }
#endif

#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT)
    if (s_app_le_audio_line_or_usb_in_state == APP_LE_AUDIO_LINE_IN
        || s_app_le_audio_line_or_usb_in_state == APP_LE_AUDIO_USB_IN) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_start_advertising fail, USB or Line in.", 0);
        return;
    }
#endif

    if (s_le_audio_adv_enabled) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER);
        if (s_le_audio_adv_timeout > 0 && timeout == 0) {
            s_le_audio_adv_timeout = 0;
        }
        if (s_le_audio_adv_timeout > 0) {
            ui_shell_send_event(FALSE, EVENT_PRIORITY_MIDDLE,
                                       EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                       EVENT_ID_LE_AUDIO_ADV_TIMER,
                                       NULL, 0, NULL, s_le_audio_adv_timeout);
        }
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_start_advertising fail, already enable timeout=%d", 1, s_le_audio_adv_timeout);
        return;
    }

    if (APP_LE_AUDIO_MAX_LINK_NUM == app_le_audio_get_link_count()) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_start_advertising fail, reach max link num", 0);
        return;
    }

    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_le_audio_get_adv_data);
    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_le_audio_get_adv_data, 1);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_NOT_RHO);

    s_le_audio_adv_enabled = TRUE;

#if 0
    if (app_le_audio_is_ever_connected()) {
        /* ADV always on, if LE Audio is ever connected. */
        s_le_audio_adv_timeout = 0;
    } else
#endif
    {
        s_le_audio_adv_timeout = timeout;    /* Enable ADV 2 mins */
    }

    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER);
    if (s_le_audio_adv_timeout > 0) {
        ui_shell_send_event(FALSE, EVENT_PRIORITY_MIDDLE,
                                   EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                   EVENT_ID_LE_AUDIO_ADV_TIMER,
                                   NULL, 0, NULL, s_le_audio_adv_timeout);
    }
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] do_start_advertising, timeout:%d interval:%x %x", 3, s_le_audio_adv_timeout, s_le_audio_adv_interval_min, s_le_audio_adv_interval_max);
}

static void app_le_audio_do_restart_advertising(uint32_t timeout)
{
    /* Start ADV only in UI task */
    if (g_le_audio_feature_state == APP_LE_AUDIO_FEATURE_DISABLE) {
        APPS_LOG_MSGID_I(LOG_TAG"[LEA] do_restart_advertising fail, LC3 disabled", 0);
        return;
    }

    if (apps_config_features_is_mp_test_mode()) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_restart_advertising fail, MP TEST Mode", 0);
        return;
    }

    if (APP_LE_AUDIO_MAX_LINK_NUM == app_le_audio_get_link_count()) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_restart_advertising fail, reach max link num", 0);
        return;
    }

#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT)
    if (s_app_le_audio_line_or_usb_in_state == APP_LE_AUDIO_LINE_IN
        || s_app_le_audio_line_or_usb_in_state == APP_LE_AUDIO_USB_IN) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_restart_advertising fail, USB or Line in.", 0);
        return;
    }
#endif

    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_le_audio_get_adv_data);
    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_le_audio_get_adv_data, 1);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_NOT_RHO);

    s_le_audio_adv_enabled = TRUE;
    s_le_audio_adv_timeout = timeout;

    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER);
    if (s_le_audio_adv_timeout > 0) {
        ui_shell_send_event(FALSE, EVENT_PRIORITY_MIDDLE,
                                   EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                   EVENT_ID_LE_AUDIO_ADV_TIMER,
                                   NULL, 0, NULL, s_le_audio_adv_timeout);
    }
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] do_restart_advertising, timeout=%d interval=0x%08X 0x%08X",
                     3, s_le_audio_adv_timeout, s_le_audio_adv_interval_min, s_le_audio_adv_interval_max);
}

static void app_le_audio_do_stop_advertising(void)
{
    /* Stop ADV only in UI task */

    if (apps_config_features_is_mp_test_mode()) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_stop_advertising fail, MP TEST Mode", 0);
        return;
    }

    if (!s_le_audio_adv_enabled) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_stop_advertising fail, already stopped", 0);
        return;
    }

    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER);

    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_le_audio_get_adv_data);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_NOT_RHO);
    s_le_audio_adv_enabled = FALSE;
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] do_stop_advertising", 0);
}

void app_le_audio_do_update_advertising_param(void)
{
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] do_update_advertising_param", 0);

    if ((!s_le_audio_adv_enabled) || (APP_LE_AUDIO_MAX_LINK_NUM == app_le_audio_get_link_count())) {
        return;
    }

#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT)
    if (s_app_le_audio_line_or_usb_in_state == APP_LE_AUDIO_LINE_IN
        || s_app_le_audio_line_or_usb_in_state == APP_LE_AUDIO_USB_IN) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] do_update_advertising_param fail, USB or Line in.", 0);
        return;
    }
#endif

    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_le_audio_get_adv_data);
    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_NOT_RHO, app_le_audio_get_adv_data, 1);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_NOT_RHO);

    s_le_audio_adv_enabled = TRUE;

    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_ADV_TIMER);
    if (s_le_audio_adv_timeout > 0) {
        ui_shell_send_event(FALSE, EVENT_PRIORITY_MIDDLE,
                                   EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                   EVENT_ID_LE_AUDIO_ADV_TIMER,
                                   NULL, 0, NULL, s_le_audio_adv_timeout);
    }

    APPS_LOG_MSGID_I(LOG_TAG"[LEA] do_start_advertising, timeout:%d interval:%x %x", 3, s_le_audio_adv_timeout, s_le_audio_adv_interval_min, s_le_audio_adv_interval_max);
}

void app_le_audio_start_advertising(uint32_t timeout)
{
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] start_advertising", 0);
    ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
        EVENT_GROUP_UI_SHELL_LE_AUDIO,
        EVENT_ID_LE_AUDIO_START_ADV,
        (void*)timeout, 0, NULL, 0);
}

void app_le_audio_restart_advertising(uint32_t timeout)
{
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] restart_advertising", 0);
    ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
        EVENT_GROUP_UI_SHELL_LE_AUDIO,
        EVENT_ID_LE_AUDIO_RESTART_ADV,
        (void*)timeout, 0, NULL, 0);
}

void app_le_audio_stop_advertising(void)
{
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] stop_advertising", 0);
    ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
        EVENT_GROUP_UI_SHELL_LE_AUDIO,
        EVENT_ID_LE_AUDIO_STOP_ADV,
        NULL, 0, NULL, 0);
}

#ifdef AIR_MULTI_POINT_ENABLE
bool app_le_audio_emp_switch_allow_callback(bool need_enable, bt_bd_addr_t *keep_phone_addr)
{
    bool allow = TRUE;
    if (need_enable) {
        if (APP_LE_AUDIO_FEATURE_ENABLE == g_le_audio_feature_state) {
            allow = FALSE;
        }
    }
    APPS_LOG_MSGID_I(LOG_TAG"[EMP] emp_switch_allow, need_enable=%d allow=%d",
                     2, need_enable, allow);
    return allow;
}
#endif

static bt_status_t app_le_audio_check_profile_supported(bt_handle_t handle)
{
    uint8_t idx = app_le_audio_get_link_index(handle);
    if (idx == APP_LE_AUDIO_MAX_LINK_NUM) {
        return BT_STATUS_FAIL;
    }

    if ((!g_le_audio_ctrl[idx].discover_gtbs) || (!g_le_audio_ctrl[idx].discover_gmcs)) {
        return BT_STATUS_BUSY;
    }

    if ((!g_le_audio_ctrl[idx].support_gtbs) && (!g_le_audio_ctrl[idx].support_gmcs)) {
        LE_AUDIO_MSGLOG_I("[APP] peer device NOT support LE Audio!", 0);
        return BT_STATUS_UNSUPPORTED;
    }

    return BT_STATUS_SUCCESS;
}

static void app_le_audio_discover_ccp_callback(bt_handle_t handle, bool support_gtbs)
{
    uint8_t idx = app_le_audio_get_link_index(handle);
    if (idx == APP_LE_AUDIO_MAX_LINK_NUM) {
        return;
    }

    g_le_audio_ctrl[idx].discover_gtbs = true;
    g_le_audio_ctrl[idx].support_gtbs = (support_gtbs ? true : false);

    if (BT_STATUS_UNSUPPORTED == app_le_audio_check_profile_supported(handle)) {
        //app_le_audio_disconnect_by_handle(handle,BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION);
    }
}

static void app_le_audio_discover_mcp_callback(bt_handle_t handle, bool support_gmcs)
{
    uint8_t idx = app_le_audio_get_link_index(handle);
    if (idx == APP_LE_AUDIO_MAX_LINK_NUM) {
        return;
    }

    g_le_audio_ctrl[idx].discover_gmcs = true;
    g_le_audio_ctrl[idx].support_gmcs = (support_gmcs ? true : false);

    if (BT_STATUS_UNSUPPORTED == app_le_audio_check_profile_supported(handle)) {
        //app_le_audio_disconnect_by_handle(handle,BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION);
    }
}

static bt_status_t app_le_audio_set_sniff(bool enable)
{
    bt_bd_addr_t aws_device;
    bt_status_t status = BT_STATUS_SUCCESS;
    uint32_t num = 0;
    num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &aws_device, 1);

    if (num > 0) {
        bt_gap_connection_handle_t gap_handle = bt_cm_get_gap_handle(aws_device);
        bt_gap_link_policy_setting_t setting;

        LE_AUDIO_MSGLOG_I("[APP] app_le_audio_set_sniff %d, connHdl:%x\r\n", 2, enable, gap_handle);
        if (!enable) {
            status = bt_gap_exit_sniff_mode(gap_handle);
        }

        setting.sniff_mode = enable ? BT_GAP_LINK_POLICY_ENABLE : BT_GAP_LINK_POLICY_DISABLE;
        status = bt_gap_write_link_policy(gap_handle, &setting);
    }
    return status;
}

static bt_status_t app_le_audio_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    switch (msg) {
        /* GAP */
        case BT_GAP_LE_DISCONNECT_IND: {
            bt_hci_evt_disconnect_complete_t *ind = (bt_hci_evt_disconnect_complete_t *)buff;
            LE_AUDIO_MSGLOG_I("[APP] BT_GAP_LE_DISCONNECT_IND, connHdl:%x, reset attribute", 1, ind->connection_handle);
            break;
        }

        case BT_GAP_LE_CIS_ESTABLISHED_IND: {
            bt_gap_le_cis_established_ind_t* ind = (bt_gap_le_cis_established_ind_t*) buff;
            LE_AUDIO_MSGLOG_I("[APP] BT_GAP_LE_CIS_ESTABLISHED_IND, connHdl:%x\r\n", 1, ind->connection_handle);
            //app_le_audio_set_sniff(false);
            break;
        }

        case BT_GAP_LE_CIS_TERMINATED_IND: {
            bt_gap_le_cis_terminated_ind_t* ind = (bt_gap_le_cis_terminated_ind_t*) buff;
            LE_AUDIO_MSGLOG_I("[APP] BT_GAP_LE_CIS_TERMINATED_IND, connHdl:%x\r\n", 1, ind->connection_handle);
            app_le_audio_set_sniff(true);
            break;
        }

        case BT_GAP_LE_BIG_SYNC_LOST_IND: {
            LE_AUDIO_MSGLOG_I("[APP] BT_GAP_LE_BIG_SYNC_LOST_IND", 0);
            break;
        }
        case BT_GAP_LE_ADVERTISING_SET_TERMINATED_IND: {
            bt_gap_le_advertising_set_terminated_ind_t *ind = (bt_gap_le_advertising_set_terminated_ind_t*)buff;
            APPS_LOG_MSGID_I(LOG_TAG" ADV_SET_TERMINATED, handle:%x adv_handle:%x", 2, ind->connection_handle, ind->handle);
            if (ind->handle == bt_le_audio_sink_get_adv_handle()) {
                s_le_audio_adv_enabled = false;
                app_le_audio_set_connection(ind->connection_handle);
                app_le_audio_start_advertising(0);
                app_le_audio_set_connected_flag();
            }
            break;
        }

        case BT_GAP_LE_ENABLE_EXTENDED_ADVERTISING_CNF: {
            bt_gap_le_enable_extended_advertising_cnf_t *cnf = (bt_gap_le_enable_extended_advertising_cnf_t *)buff;
            APPS_LOG_MSGID_I(LOG_TAG" LE_ENABLE_EXTENDED_ADVERTISING_CNF, handle:%x enable:%x", 2, cnf->handle, cnf->enable);
            if (cnf->handle == bt_le_audio_sink_get_adv_handle()) {
                if (cnf->enable) {
                    s_le_audio_adv_enabled = true;
                } else {
                    s_le_audio_adv_enabled = false;
                }

            }
            break;
        }

        case BT_GATTC_DISCOVER_PRIMARY_SERVICE:
        case BT_GATTC_DISCOVER_PRIMARY_SERVICE_BY_UUID:
        case BT_GATTC_FIND_INCLUDED_SERVICES:
        case BT_GATTC_DISCOVER_CHARC:
        case BT_GATTC_DISCOVER_CHARC_DESCRIPTOR:
        case BT_GATTC_READ_CHARC:
        case BT_GATTC_READ_LONG_CHARC:
        case BT_GATTC_READ_USING_CHARC_UUID:
        case BT_GATTC_READ_MULTI_CHARC_VALUES:
        case BT_GATTC_WRITE_CHARC:
        case BT_GATTC_WRITE_LONG_CHARC:
        case BT_GATTC_RELIABLE_WRITE_CHARC:
        case BT_GATTC_CHARC_VALUE_NOTIFICATION: {
            app_le_audio_aird_client_event_handler(msg, status, buff);
            break;
        }
        default:
            break;
    }

    return BT_STATUS_SUCCESS;
}

static bool app_le_audio_proc_bt_cm_group(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
#ifdef MTK_AWS_MCE_ENABLE
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
            bool sync_adv = false;

            if (NULL == remote_update) {
                break;
            }

#ifdef MTK_AWS_MCE_ENABLE
            if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service) &&
                (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                LE_AUDIO_MSGLOG_I(LOG_TAG"[LEA][%02X] AWS Connected", 1, role);
                //app_mps_set_battery_level_by_channel(APP_MPS_CHANNEL_PEER, MPS_BATTERY_LEVEL_VALUE_MAX);
                //app_le_audio_send_battery_level_notification();
                if (BT_AWS_MCE_ROLE_AGENT == role) {
                    #if (APP_LE_AUDIO_VISIBILITY_TYPE == 0)
                        if (!s_le_audio_adv_enabled && (app_le_audio_get_link_count() < APP_LE_AUDIO_MAX_LINK_NUM)) {
                            app_le_audio_do_start_advertising(s_le_audio_adv_timeout);
                        }
                    #endif
                    sync_adv = true;
                }
#ifdef AIR_LE_AUDIO_CIS_ENABLE
                bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                                                       EVENT_ID_LE_AUDIO_CIS_SYNC_STATE,
                                                                       &lea_local_cis_connected,
                                                                       sizeof(uint8_t));
                APPS_LOG_MSGID_I(LOG_TAG"[LEA][LEA_Power_Saving] AWS Connected, sync_cis_connected=%d bt_status=0x%08X",
                                 2, lea_local_cis_connected, bt_status);
#endif
            } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                LE_AUDIO_MSGLOG_I(LOG_TAG"[LEA][%02X][LEA_Power_Saving] AWS Disconnected", 1, role);
                //app_mps_set_battery_level_by_channel(APP_MPS_CHANNEL_PEER, MPS_BATTERY_LEVEL_VALUE_MIN);
                //app_le_audio_send_battery_level_notification();
#ifdef AIR_LE_AUDIO_CIS_ENABLE
                lea_peer_cis_connected = FALSE;
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                app_power_save_utils_notify_mode_changed(FALSE, NULL);
#endif
#endif
            }
#endif

            if (!(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service) &&
                (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                LE_AUDIO_MSGLOG_I(LOG_TAG"[LEA] EDR non-AWS profile connected, addr:%02x:%02x:%02x:%02x:%02x:%02x" , 6,
                                                                            remote_update->address[5],
                                                                            remote_update->address[4],
                                                                            remote_update->address[3],
                                                                            remote_update->address[2],
                                                                            remote_update->address[1],
                                                                            remote_update->address[0]);
                #if (APP_LE_AUDIO_VISIBILITY_TYPE == 1)
                    s_le_audio_adv_visible = true;
                    app_le_audio_do_start_advertising(s_le_audio_adv_timeout);
                    sync_adv = true;
                #endif
            }

            if ((BT_CM_ACL_LINK_CONNECTED <= remote_update->pre_acl_state) &&
                (BT_CM_ACL_LINK_CONNECTED > remote_update->acl_state) &&
                (0 == bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0))) {
#ifdef MTK_AWS_MCE_ENABLE
                if (BT_AWS_MCE_ROLE_AGENT == role) {
                    LE_AUDIO_MSGLOG_I(LOG_TAG"[LEA] SP ACL disconnected, addr:%02x:%02x:%02x:%02x:%02x:%02x" , 6,
                                                                                remote_update->address[5],
                                                                                remote_update->address[4],
                                                                                remote_update->address[3],
                                                                                remote_update->address[2],
                                                                                remote_update->address[1],
                                                                                remote_update->address[0]);

                    #if (APP_LE_AUDIO_VISIBILITY_TYPE == 1)
                        s_le_audio_adv_visible = false;
                        app_le_audio_do_stop_advertising();
                        sync_adv = true;
                    #endif
                }
#endif

            }

#ifdef MTK_AWS_MCE_ENABLE
#if (APP_LE_AUDIO_VISIBILITY_TYPE == 0) || (APP_LE_AUDIO_VISIBILITY_TYPE == 1)
            if ((sync_adv) && (BT_AWS_MCE_ROLE_AGENT == role)) {
                /* sync LE_Audio_ADV */
                app_ble_audio_adv_aws_data_t aws_data = {
                    .timeout = s_le_audio_adv_timeout,
                    .visible = s_le_audio_adv_visible,
                };
                APPS_LOG_MSGID_I(LOG_TAG"[LEA][Agent] send sync ADV, enable:%d timeout:%d visible:%x", 3,
                                                                            s_le_audio_adv_enabled,
                                                                            s_le_audio_adv_timeout,
                                                                            aws_data.visible);
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                                        APPS_EVENTS_INTERACTION_LE_AUDIO_ADV,
                                                                        &aws_data,
                                                                        sizeof(aws_data))) {
                    APPS_LOG_MSGID_E(LOG_TAG"[LEA][Agent] send sync ADV, fail", 0);
                }
            }
#endif
#endif
            break;
        }

        case BT_CM_EVENT_VISIBILITY_STATE_UPDATE: {
            bt_cm_visibility_state_update_ind_t *visible_update = (bt_cm_visibility_state_update_ind_t *)extra_data;
            if (NULL == visible_update) {
                break;
            }
            LE_AUDIO_MSGLOG_I(LOG_TAG"[APP] Trigger advertising visibility status: %d", 1, visible_update->visibility_state);
            break;
        }

        case BT_CM_EVENT_POWER_STATE_UPDATE: {
            bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)extra_data;
            if (NULL == power_update) {
                break;
            }

            if (BT_CM_POWER_STATE_ON != power_update->power_state) {
                LE_AUDIO_MSGLOG_I(LOG_TAG"[APP] stop ble adv caused by BT OFF", 0);
                // reset le audio data
                bt_sink_srv_cap_am_deinit();
            }
        }
            break;

        default:
            break;
    }
    return ret;
}

static bool app_le_audio_proc_bt_dm_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    bt_device_manager_power_event_t evt;
    bt_device_manager_power_status_t status;
    bt_event_get_bt_dm_event_and_status(event_id, &evt, &status);
    switch (evt) {
        case BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE:
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] BT_DM Power ON, status: 0x%x", 1, status);
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                bt_sink_srv_cap_am_init();
                app_le_audio_hfp_at_cmd_register(TRUE);
            }
            break;
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE:
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] BT_DM POWER OFF, status: 0x%x", 1, status);
            if (BT_DEVICE_MANAGER_POWER_RESET_TYPE_NORMAL == status) {
                LE_AUDIO_MSGLOG_I(LOG_TAG"[LEA] stop ble adv caused by BT OFF", 0);
                // reset le audio data
                bt_sink_srv_cap_am_deinit();
                app_le_audio_hfp_at_cmd_register(FALSE);
            }
            break;
        default:
            break;
    }
    return ret;
}

static bool app_le_audio_bt_sink_proc(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    switch (event_id) {
        case LE_SINK_SRV_EVENT_REMOTE_INFO_UPDATE: {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
            bt_le_sink_srv_event_remote_info_update_t *update_ind = (bt_le_sink_srv_event_remote_info_update_t *)extra_data;
            if (update_ind == NULL) {
                break;
            }
            bool old_local_cis_connected = lea_local_cis_connected;
            APPS_LOG_MSGID_I(LOG_TAG"[LEA][LEA_Power_Saving] LEA_BT_SINK, Link=%d->%d srv=%d->%d",
                             4, update_ind->pre_state, update_ind->state,
                             update_ind->pre_connected_service, update_ind->connected_service);
            if (update_ind->pre_state == BT_BLE_LINK_DISCONNECTED
                && update_ind->state == BT_BLE_LINK_CONNECTED) {
                lea_local_cis_connected = TRUE;
            } else if (update_ind->pre_state == BT_BLE_LINK_CONNECTED
                    && update_ind->state == BT_BLE_LINK_DISCONNECTED) {
#ifdef AIR_LE_AUDIO_MULTIPOINT_ENABLE
                bt_handle_t le_handle = bt_sink_srv_cap_check_links_state(BT_SINK_SRV_CAP_STATE_CONNECTED);
                APPS_LOG_MSGID_I(LOG_TAG"[LEA][LEA_Power_Saving] LEA_BT_SINK, Disconnect remain le_handle=0x%04X", 1, le_handle);
                if (BT_HANDLE_INVALID == le_handle) {
                    lea_local_cis_connected = FALSE;
                }
#else
                lea_local_cis_connected = FALSE;
#endif
            }

            APPS_LOG_MSGID_I(LOG_TAG"[LEA][LEA_Power_Saving] LEA_BT_SINK, lea_local_cis_connected=%d->%d",
                             2, old_local_cis_connected, lea_local_cis_connected);
            if (old_local_cis_connected != lea_local_cis_connected) {
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                app_power_save_utils_notify_mode_changed(FALSE, NULL);
#endif
#ifdef MTK_AWS_MCE_ENABLE
                bt_aws_mce_srv_link_type_t aws_link_type = bt_aws_mce_srv_get_link_type();
                bt_status_t bt_status = BT_STATUS_SUCCESS;
                if (aws_link_type != BT_AWS_MCE_SRV_LINK_NONE) {
                    bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                                               EVENT_ID_LE_AUDIO_CIS_SYNC_STATE,
                                                               &lea_local_cis_connected,
                                                               sizeof(uint8_t));
                    APPS_LOG_MSGID_I(LOG_TAG"[LEA][LEA_Power_Saving] LEA_BT_SINK, sync_cis_connected=%d bt_status=0x%08X",
                                     2, lea_local_cis_connected, bt_status);
                }
#endif
#endif
            }
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            else if (lea_local_cis_connected) {
                /* To notify power save refresh time. */
                app_power_save_utils_refresh_waiting_time();
            }
#endif
            break;
        }

        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
            if (param == NULL) {
                break;
            }
            const bt_sink_srv_am_type_t type = bt_sink_srv_ami_get_current_scenario();

            APPS_LOG_MSGID_I(LOG_TAG"[LEA] sink state change, state:%x->%x", 2, param->previous, param->current);

            if ((param->previous != BT_SINK_SRV_STATE_STREAMING) && (param->current == BT_SINK_SRV_STATE_STREAMING)) {
                /* A2DP Streaming start */
                APPS_LOG_MSGID_I(LOG_TAG"[LEA] A2DP Streaming start! type %d", 1, type);
                s_le_audio_adv_interval_min = APP_LE_AUDIO_ADV_INTERVAL_MIN_L;
                s_le_audio_adv_interval_max = APP_LE_AUDIO_ADV_INTERVAL_MAX_L;
                app_le_audio_do_update_advertising_param();

            } else if ((param->previous == BT_SINK_SRV_STATE_STREAMING) && (param->current != BT_SINK_SRV_STATE_STREAMING)) {
                /* A2DP Streaming stop */
                APPS_LOG_MSGID_I(LOG_TAG"[LEA] A2DP Streaming stop! type %d", 1, type);
                s_le_audio_adv_interval_min = APP_LE_AUDIO_ADV_INTERVAL_MIN_S;
                s_le_audio_adv_interval_max = APP_LE_AUDIO_ADV_INTERVAL_MAX_S;
                app_le_audio_do_update_advertising_param();
            }

            break;
        }
        case BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE: {
            bt_sink_srv_sco_state_update_t *esco_state = (bt_sink_srv_sco_state_update_t *)extra_data;
            if (esco_state == NULL) {
                break;
            }
            if (esco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED) {
                /* ESCO connected */
                APPS_LOG_MSGID_I(LOG_TAG"[LEA] eSCO connected", 0);
                s_le_audio_adv_interval_min = APP_LE_AUDIO_ADV_INTERVAL_MIN_L;
                s_le_audio_adv_interval_max = APP_LE_AUDIO_ADV_INTERVAL_MAX_L;
                app_le_audio_do_update_advertising_param();
                //app_le_audio_aird_client_infom_device_busy(true);

            } else if (esco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED) {
                /* ESCO disconnected */
                APPS_LOG_MSGID_I(LOG_TAG"[LEA] eSCO disconnected", 0);
                s_le_audio_adv_interval_min = APP_LE_AUDIO_ADV_INTERVAL_MIN_S;
                s_le_audio_adv_interval_max = APP_LE_AUDIO_ADV_INTERVAL_MAX_S;
                app_le_audio_do_update_advertising_param();
                //app_le_audio_aird_client_infom_device_busy(false);
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

/*
void bt_sink_srv_le_media_state_change_callback(uint16_t event_id, bt_handle_t handle, bool is_resume)
{
    // Use BT_SINK_SRV LE Audio Callback directly, not switch to APP task for low latency
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] le_media_state_change_callback, event_id=0x%04X handle=0x%04X is_resume=%d",
                     3, event_id, handle, is_resume);
    if (event_id == BT_LE_AUDIO_SINK_EVENT_MEDIA_SUSPEND) {
        app_le_audio_aird_client_notify_dongle_media_state(TRUE, handle, NULL, 0);
    } else if (event_id == BT_LE_AUDIO_SINK_EVENT_MEDIA_RESUME) {
        app_le_audio_aird_client_notify_dongle_media_state(FALSE, handle, NULL, 0);
    }
}*/

static void app_le_audio_handle_timeout(TimerHandle_t timer_id)
{
    LE_AUDIO_MSGLOG_I("[LEA] handle_timeout", 0);
    app_le_audio_aird_client_infom_device_busy(false);
}

static void app_le_audio_usb_start_timer(void)
{
    if (!g_lea_timer) {
        g_lea_timer = xTimerCreate("app_lea_timer",
                                            (APP_LE_AUDIO_RESET_DEVICE_BUSY_TIMEOUT * portTICK_PERIOD_MS),
                                            pdFALSE, /* Repeat timer */
                                            NULL,
                                            app_le_audio_handle_timeout);
    }

    if (!g_lea_timer) {
        LE_AUDIO_MSGLOG_I("[LEA] start_timer, create timer failed!", 0);
        return;
    }

    if (pdFALSE == xTimerIsTimerActive(g_lea_timer)) {
        xTimerStart(g_lea_timer, 0);
        LE_AUDIO_MSGLOG_I("[LEA] start_timer, done", 0);
    }
}

static void app_le_audio_usb_stop_timer(void)
{
    if (!g_lea_timer) {
        return;
    }

    if (pdTRUE != xTimerIsTimerActive(g_lea_timer)) {
        return;
    }

    xTimerStop(g_lea_timer, 0);
    LE_AUDIO_MSGLOG_I("[LEA] stop_timer, done", 0);
}

void bt_sink_srv_edr_state_change_callback(bt_sink_srv_state_t previous, bt_sink_srv_state_t now)
{
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] edr_state_change, previous:%x now:%x", 2, previous, now);

    if (((!(previous & BT_SINK_SRV_STATE_INCOMING)) && (now & BT_SINK_SRV_STATE_INCOMING)) ||
        ((!(previous & BT_SINK_SRV_STATE_OUTGOING)) && (now & BT_SINK_SRV_STATE_OUTGOING)) ||
        //((previous & BT_SINK_SRV_STATE_HELD_REMAINING) && (!(now & BT_SINK_SRV_STATE_HELD_REMAINING))) ||
        ((!(previous & BT_SINK_SRV_STATE_ACTIVE)) && (now & BT_SINK_SRV_STATE_ACTIVE)) ||
        ((!(previous & BT_SINK_SRV_STATE_STREAMING)) && (now & BT_SINK_SRV_STATE_STREAMING))) {
        /* HFP incoming call */
        /* HFP outgoing call */
        /* HFP cal unheld */
        /* HFP call active */
        /* A2DP Streaming start */
        app_le_audio_usb_stop_timer();
        app_le_audio_aird_client_infom_device_busy(true);

    } else if (((previous & BT_SINK_SRV_STATE_INCOMING) && (!(now & BT_SINK_SRV_STATE_INCOMING))) ||
                ((previous & BT_SINK_SRV_STATE_OUTGOING) && (!(now & BT_SINK_SRV_STATE_OUTGOING))) ||
                //((!(previous & BT_SINK_SRV_STATE_HELD_REMAINING)) && (now & BT_SINK_SRV_STATE_HELD_REMAINING)) ||
                ((previous & BT_SINK_SRV_STATE_ACTIVE) && (!(now & BT_SINK_SRV_STATE_ACTIVE))) ||
                ((previous & BT_SINK_SRV_STATE_STREAMING) && (!(now & BT_SINK_SRV_STATE_STREAMING)))) {
        /* HFP reject call (incoming) */
        /* HFP reject call (outgoing) */
        /* HFP cal held */
        /* HFP call end */
        /* A2DP Streaming stop */
        app_le_audio_usb_start_timer();
    }
}

static bool app_le_audio_proc_bt_group(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    apps_bt_event_data_t *bt_event_data = (apps_bt_event_data_t *)extra_data;
    if (bt_event_data == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG"[LEA] BT event, bt_event_data is NULL", 0);
        return ret;
    }

    switch (event_id) {
        case BT_POWER_ON_CNF: {
            /* Multi-BLE-ADV will restart LE_Audio ADV if app_le_audio_is_ever_connected() == TRUE */
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] BT_POWER_ON_CNF, adv_enabled=%d", 1, s_le_audio_adv_enabled);
            g_le_audio_aird_device_suspend_flag = false;
            app_le_audio_reset_connection();

#if (APP_LE_AUDIO_VISIBILITY_TYPE == 0) || (APP_LE_AUDIO_VISIBILITY_TYPE == 2)
            if (app_le_audio_is_ever_connected()) {
                app_le_audio_do_start_advertising(s_le_audio_adv_timeout);    /* Enable ADV 2 mins */
                APPS_LOG_MSGID_I(LOG_TAG"[LEA] BT_POWER_ON_CNF, restart BLE Audio ADV", 0);
            }
#endif
            break;
        }
        case BT_POWER_OFF_CNF: {
            /* Multi-BLE-ADV will stop LE_Audio ADV, but keep s_le_audio_adv_enabled */
            APPS_LOG_MSGID_I(LOG_TAG"[LEA][BIS] BT_POWER_OFF_CNF, adv_enabled=%d", 1, s_le_audio_adv_enabled);
            app_le_audio_reset_connection();
            break;
        }
        case BT_GAP_LE_CONNECT_IND: {
            bt_gap_le_connection_ind_t *ind = (bt_gap_le_connection_ind_t *)bt_event_data->buffer;
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] BT_GAP_LE_CONNECT_IND, handle=0x%08X", 1, ind->connection_handle);
            break;
        }
        case BT_GAP_LE_DISCONNECT_IND: {
            bt_hci_evt_disconnect_complete_t *ind = (bt_hci_evt_disconnect_complete_t *)bt_event_data->buffer;
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] BT_GAP_LE_DISCONNECT_IND, handle=0x%08X", 1, ind->connection_handle);
            uint8_t link_idx = app_le_audio_get_link_index(ind->connection_handle);
            if (link_idx < APP_LE_AUDIO_MAX_LINK_NUM) {
                memset(&g_le_audio_ctrl[link_idx], 0, sizeof(app_le_audio_ctrl_t));
                g_le_audio_ctrl[link_idx].handle = BT_HANDLE_INVALID;
		    }
            app_le_audio_aird_client_reset_info(link_idx);

#if (APP_LE_AUDIO_VISIBILITY_TYPE == 2)
            app_le_audio_do_start_advertising(0);
#else
            if (s_le_audio_adv_visible) {
                app_le_audio_do_start_advertising(s_le_audio_adv_timeout);    /* Enable ADV 2 mins */
            }
#endif
            break;
        }
        default:
            break;
    }
    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool app_le_audio_process_aws_data_group(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    uint32_t aws_event_group;
    uint32_t aws_event_id;
    void *p_extra_data = NULL;
    uint32_t extra_data_len = 0;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (!aws_data_ind || aws_data_ind->module_id != BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        return ret;
    }

    apps_aws_sync_event_decode_extra(aws_data_ind, &aws_event_group, &aws_event_id,
                                     &p_extra_data, &extra_data_len);
    if (aws_event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
        && aws_event_id == APPS_EVENTS_INTERACTION_LE_AUDIO_ADV) {
        app_ble_audio_adv_aws_data_t *aws_data = (app_ble_audio_adv_aws_data_t *)p_extra_data;
        if (aws_data != NULL) {
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] recv sync ADV, role:%02X visible:%d enable:%x", 3, role, aws_data->visible, s_le_audio_adv_enabled);
            if (aws_data->visible) {
                APPS_LOG_MSGID_I(LOG_TAG"[LEA] recv sync ADV, start advertising", 0);
                s_le_audio_adv_visible = true;
                app_le_audio_do_start_advertising(aws_data->timeout);
            } else {
                APPS_LOG_MSGID_I(LOG_TAG"[LEA] recv sync ADV, stop advertising", 0);
                s_le_audio_adv_visible = false;
                app_le_audio_do_stop_advertising();
            }
        }
    } else if (aws_event_group == EVENT_GROUP_UI_SHELL_LE_AUDIO
               && aws_event_id == EVENT_ID_LE_AUDIO_CIS_SYNC_STATE) {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
        bool peer_cis_connected = *((bool *)p_extra_data);
        APPS_LOG_MSGID_I(LOG_TAG"[LEA][LEA_Power_Saving] AWS_DATA, peer_cis_connected=%d->%d",
                         2, lea_peer_cis_connected, peer_cis_connected);
        if (peer_cis_connected != lea_peer_cis_connected) {
            lea_peer_cis_connected = peer_cis_connected;
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            app_power_save_utils_notify_mode_changed(FALSE, NULL);
#endif
#endif
        }
    }

    return ret;
}
#endif

#ifdef MTK_SMART_CHARGER_ENABLE
static bool app_le_audio_process_smcharger_group(struct _ui_shell_activity *self,
                                                 uint32_t event_id,
                                                 void *extra_data,
                                                 size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_SMCHARGER_NOTIFY_CHANGED_EVENT: {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            LE_AUDIO_MSGLOG_I("[LEA][HF] SMCharger group, [%02X] event_id=%d", 2, role, event_id);
            if (role == BT_AWS_MCE_ROLE_AGENT) {
                app_le_audio_hfp_send_battery_info();
            }
            break;
        }
        default:
            break;
    }
    return FALSE;
}
#endif

static bool app_le_audio_proc_interaction_event_group(
        ui_shell_activity_t *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;


    switch (event_id) {
#ifdef APPS_LINE_IN_SUPPORT
        case APPS_EVENTS_INTERACTION_LINE_IN_PLUG_STATE: {
            bool plug_in = (bool)extra_data;
            app_le_audio_line_or_usb_in_state_t current_state = plug_in ? APP_LE_AUDIO_LINE_IN : APP_LE_AUDIO_LINE_OUT;
            if (current_state != s_app_le_audio_line_or_usb_in_state) {
                APPS_LOG_MSGID_I(LOG_TAG", receive LINE_IN = %d", 1, plug_in);
                if (APP_LE_AUDIO_LINE_OR_USB_NONE == s_app_le_audio_line_or_usb_in_state) {
                    /* BT should have not enabled. */
                    s_app_le_audio_line_or_usb_in_state = current_state;
                } else {
                    s_app_le_audio_line_or_usb_in_state = current_state;
                    if (plug_in) {
                        app_le_audio_stop_advertising();
                        app_le_audio_disconnect(BT_HCI_STATUS_CONNECTION_TERMINATED_BY_LOCAL_HOST);
                    } else {
                        app_le_audio_start_advertising(0);
                    }
                }
            }
            break;
        }
#endif
#ifdef APPS_USB_AUDIO_SUPPORT
        case APPS_EVENTS_INTERACTION_USB_PLUG_STATE: {
            bool plug_in = (bool)extra_data;
            app_le_audio_line_or_usb_in_state_t current_state = plug_in ? APP_LE_AUDIO_USB_IN : APP_LE_AUDIO_USB_OUT;
            if (current_state != s_app_le_audio_line_or_usb_in_state) {
                APPS_LOG_MSGID_I(LOG_TAG", receive USB AUDIO CHANGE = %d", 1, plug_in);
                s_app_le_audio_line_or_usb_in_state = current_state;
                if (plug_in) {
                    app_le_audio_stop_advertising();
                    app_le_audio_disconnect(BT_HCI_STATUS_CONNECTION_TERMINATED_BY_LOCAL_HOST);
                } else {
                    app_le_audio_start_advertising(0);
                }
            }
            break;
        }
#endif
        default:
            break;
    }

    return ret;
}


static bool app_le_audio_proc(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = TRUE;
    APPS_LOG_MSGID_I(LOG_TAG"[LEA] LE Audio Event ID=%d", 1, event_id);

    switch (event_id) {
        case EVENT_ID_LE_AUDIO_ADV_TIMER: {
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] ADV timeout", 0);
            app_le_audio_do_stop_advertising();
            break;
        }
        case EVENT_ID_LE_AUDIO_START_ADV: {
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] START ADV event", 0);
            app_le_audio_do_start_advertising((uint32_t)extra_data);    /* Enable ADV 2 mins */
            break;
        }
        case EVENT_ID_LE_AUDIO_STOP_ADV: {
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] STOP ADV event", 0);
            app_le_audio_do_stop_advertising();
            break;
        }
        case EVENT_ID_LE_AUDIO_RESTART_ADV: {
            APPS_LOG_MSGID_I(LOG_TAG"[LEA] RESTART ADV event", 0);
            app_le_audio_do_restart_advertising((uint32_t)extra_data);
            break;
        }
        case EVENT_ID_LE_AUDIO_NOTIFY_BATTERY: {
            app_le_audio_hfp_send_battery_info();
            break;
        }
        default:
            break;
    }
    return ret;
}

static void app_le_audio_read_feature_state(void)
{
#ifdef AIR_MULTI_POINT_ENABLE
    nvkey_status_t nvkey_status = NVKEY_STATUS_OK;
    uint32_t size = sizeof(g_le_audio_feature_state);

    /* NVKEYID_BT_LE_AUDIO_ENABLE: (1 bytes) */
    nvkey_status = nvkey_read_data(NVKEYID_BT_LE_AUDIO_ENABLE, &g_le_audio_feature_state, &size);
    if (NVKEY_STATUS_OK != nvkey_status) {
        LE_AUDIO_MSGLOG_I("[APP][state] read NVKEYID_BT_LE_AUDIO_ENABLE, nvkey_status:%x", 1, nvkey_status);

        g_le_audio_feature_state = APP_LE_AUDIO_FEATURE_ENABLE;
        nvkey_status = nvkey_write_data(NVKEYID_BT_LE_AUDIO_ENABLE, &g_le_audio_feature_state, size);
        if (NVKEY_STATUS_OK != nvkey_status) {
            LE_AUDIO_MSGLOG_I("[APP][state] read_feature_state, write nvkey_status:%x", 1, nvkey_status);
        }
    }
#else
    g_le_audio_feature_state = APP_LE_AUDIO_FEATURE_ENABLE;
#endif

    LE_AUDIO_MSGLOG_I("[APP][state] app_le_audio_read_feature_state, enable:%x", 1, g_le_audio_feature_state);
}

static void app_le_audio_set_feature_state(uint8_t enable)
{
    nvkey_status_t nvkey_status = NVKEY_STATUS_OK;
    uint32_t size = sizeof(g_le_audio_feature_state);

    nvkey_status = nvkey_write_data(NVKEYID_BT_LE_AUDIO_ENABLE, &enable, size);
    if (NVKEY_STATUS_OK != nvkey_status) {
        LE_AUDIO_MSGLOG_I("[APP][state] write NVKEYID_BT_LE_AUDIO_ENABLE, nvkey_status:%x", 1, nvkey_status);
    }
    g_le_audio_feature_state = enable;

    LE_AUDIO_MSGLOG_I("[APP][state] app_le_audio_set_feature_state, enable:%x", 1, g_le_audio_feature_state);

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (BT_AWS_MCE_ROLE_AGENT == role) {
        bt_app_common_sync_le_audio_info();
    }
#endif
}

/**************************************************************************************************
* Public function
**************************************************************************************************/
bool app_le_audio_idle_activity_proc(struct _ui_shell_activity *self, uint32_t event_group, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    LE_AUDIO_MSGLOG_I("[APP] APP_LE_AUDIO_IDLE_ACT, event_group : %x, evt: %x", 2, event_group, event_id);

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            ret = app_le_audio_proc_bt_cm_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            ret = app_le_audio_bt_sink_proc(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT: {
            ret = app_le_audio_proc_bt_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef MTK_AWS_MCE_ENABLE
        /* UI Shell BT AWS_DATA events. */
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = app_le_audio_process_aws_data_group(self, event_id, extra_data, data_len);
            break;
#endif
#ifdef MTK_SMART_CHARGER_ENABLE
        /* APP SmartCharger events. */
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE:
            ret = app_le_audio_process_smcharger_group(self, event_id, extra_data, data_len);
            break;
#endif
        case EVENT_GROUP_UI_SHELL_LE_AUDIO: {
            ret = app_le_audio_proc(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER: {
            ret = app_le_audio_proc_bt_dm_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            ret = app_le_audio_proc_interaction_event_group(self, event_id, extra_data, data_len);
            break;
        }
        default:
            break;
    }

#ifdef AIR_LE_AUDIO_BIS_ENABLE
    app_le_audio_bis_proc_ui_shell_event(self, event_group, event_id, extra_data, data_len);
#endif

    LE_AUDIO_MSGLOG_I("[APP] APP_LE_AUDIO_IDLE_ACT, ret : %x", 1, ret);
    return ret;
}

uint8_t app_le_audio_get_feature_state(void)
{
    return g_le_audio_feature_state;
}

void app_le_audio_disconnect(bt_hci_disconnect_reason_t reason)
{
    for (uint8_t idx = 0; idx < APP_LE_AUDIO_MAX_LINK_NUM; idx++) {
        if (BT_HANDLE_INVALID != g_le_audio_ctrl[idx].handle) {
            app_le_audio_disconnect_by_handle(g_le_audio_ctrl[idx].handle, reason);
        }
    }
}

void app_le_audio_enable(bool enable)
{
    LE_AUDIO_MSGLOG_I("[APP] app_le_audio_enable %d", 1, enable);
    if (enable) {
        app_le_audio_set_feature_state(APP_LE_AUDIO_FEATURE_ENABLE);
        if (s_le_audio_adv_visible && (app_le_audio_get_link_count() < APP_LE_AUDIO_MAX_LINK_NUM)) {
            app_le_audio_start_advertising(0);
        }
    } else {
        app_le_audio_set_feature_state(APP_LE_AUDIO_FEATURE_DISABLE);
        // disconnect le audio link
        // ZMITWS01-430: remove disconnection behaviour because the race condition with SP
        app_le_audio_disconnect(BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION);
        // stop adv
        app_le_audio_stop_advertising();
    }
}

void app_le_audio_init(void)
{
    LE_AUDIO_MSGLOG_I("[APP] app_le_audio_init", 0);
#ifdef AIR_LE_AUDIO_MULTIPOINT_ENABLE
    multi_ble_adv_manager_set_le_connection_max_count(MULTI_ADV_INSTANCE_NOT_RHO, APP_LE_AUDIO_MAX_LINK_NUM);
#endif
	app_le_audio_read_feature_state();

    le_sink_srv_init(APP_LE_AUDIO_MAX_LINK_NUM);
    le_audio_set_device_type(LE_AUDIO_DEVICE_TYPE_HEADSET);

    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_GAP | MODULE_MASK_SYSTEM | MODULE_MASK_GATT, (void *)app_le_audio_event_callback);

    app_le_audio_reset_connection();

    g_le_audio_aird_device_suspend_flag = false;
    app_le_audio_aird_client_init();

    app_ccp_init(app_le_audio_discover_ccp_callback);
    app_mcp_init(app_le_audio_discover_mcp_callback);

#ifdef AIR_MULTI_POINT_ENABLE
    app_bt_emp_srv_user_register(APP_BT_EMP_SRV_USER_ID_LEAUDIO, app_le_audio_emp_switch_allow_callback);
#endif

#ifdef AIR_LE_AUDIO_BIS_ENABLE
    app_le_audio_bis_init();
#endif
}

bool app_le_audio_is_connected(void)
{
    bool ret = FALSE;
#ifdef AIR_LE_AUDIO_CIS_ENABLE
    ret = lea_local_cis_connected;
#ifdef MTK_AWS_MCE_ENABLE
    if (!ret) {
        ret = lea_peer_cis_connected;
    }
#endif
#endif
    APPS_LOG_MSGID_I(LOG_TAG"[LEA][LEA_Power_Saving] is_connected=%d", 1, ret);
    return ret;
}

#endif  /* AIR_LE_AUDIO_ENABLE */

