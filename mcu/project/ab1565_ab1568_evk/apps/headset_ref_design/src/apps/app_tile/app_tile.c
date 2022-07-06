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

#ifdef AIR_TILE_ENABLE

#include "FreeRTOS.h"
#include "timers.h"
#include "hal_trng.h"
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#include "nvdm_id_list.h"
#endif

#if defined(MTK_AWS_MCE_ENABLE)
#include "bt_aws_mce_report.h"
#endif
#include "bt_sink_srv.h"
#include "bt_sink_srv_ami.h"
#include "bt_type.h"
#include "apps_debug.h"
#include "bt_hci.h"
#include "bt_gap_le.h"
#include "bt_callback_manager.h"
#include "bt_gap_le_service.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "bt_init.h"
#include "ui_shell_manager.h"
#include "bt_customer_config.h"
#include "multi_ble_adv_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_events_bt_event.h"
#include "app_battery_transient_activity.h"
#include "battery_management.h"
#include "battery_management_core.h"
#include "apps_customer_config.h"
#include "app_tile.h"
#ifdef MTK_SMART_CHARGER_ENABLE
/* MTK_SMART_CHARGER_ENABLE feature is only for earbuds project  */
#include "app_smcharger_utils.h"
#endif

#define TILE_LOG_TAG     "[app_tile] "

#define UUID16_TILE_INACTIVE 0xFEEC
#define UUID16_TILE_ACTIVE   0xFEED

#define APP_TILE_ADV_REMOVED                   0x00
#define APP_TILE_ADV_STARTED                   0x01
#define APP_TILE_ADV_STOPPED                   0x02
#define APP_TILE_ADV_UPDATED                   0x03
typedef uint8_t app_tile_adv_state_t;

#define APP_TILE_MODE_NONE                     0x00
#define APP_TILE_MODE_IDLE_2_STREAMING         0x01
#define APP_TILE_MODE_STREAMING_2_IDLE         0x02
typedef uint8_t app_tile_mode_t;

#define APP_TILE_ADV_NONE                      0x00
#define APP_TILE_ADV_SCANNABLE                 0x01
#define APP_TILE_ADV_CONNECTABLE               0x02
typedef uint8_t app_tile_adv_type_t;

#define TILE_ADV_TIMER                        (2 * 60 * 1000) /* The waiting time (ms) if tile is not activated */

typedef struct {
    uint8_t                         adv_instance;
    app_tile_adv_state_t            adv_state;
    app_tile_adv_type_t             now_adv_type;
    app_tile_adv_type_t             next_adv_type;
    app_tile_mode_t                 next_mode;
    bt_bd_addr_t                    local_random_addr;
    uint16_t                        conn_handle;
    uint8_t                         battery_percent;            /**<  The battery percent of current device. */
    int32_t                         charging_state;             /**<  Charging state in battery_management. */
    int32_t                         charger_exist_state;        /**<  Charger_exist_state in battery_management. */
    battery_event_shutdown_state_t  shutdown_state;             /**<  Shutdown_state in battery_management. */
    app_battery_state_t             battery_state;              /**<  The states of battery. */
} app_tile_local_context_t;

static app_tile_local_context_t g_app_tile_context;   /* The variable records context */

static bool g_app_tile_tmd_is_active_state;

static app_battery_state_t get_battery_state(app_tile_local_context_t *local_ctx)
{
    app_battery_state_t new_state;
    app_battery_state_t old_state = local_ctx->battery_state;
    APPS_LOG_MSGID_I(TILE_LOG_TAG"charger_exist_state %d, charger state %d", 2, local_ctx->charger_exist_state, local_ctx->charging_state);
    if (local_ctx->charger_exist_state) {
        if (local_ctx->charging_state == CHARGER_STATE_CHR_OFF || local_ctx->charging_state == CHARGER_STATE_EOC) {
            new_state = APP_BATTERY_STATE_CHARGING_FULL;
        } else if (local_ctx->charging_state == CHARGER_STATE_THR) {
            new_state = APP_BATTERY_STATE_THR;
        } else {
            new_state = APP_BATTERY_STATE_CHARGING;
        }
    } else {
        if (local_ctx->shutdown_state == APPS_EVENTS_BATTERY_SHUTDOWN_STATE_VOLTAGE_LOW) {
            new_state = APP_BATTERY_STATE_SHUTDOWN;
        } else if (local_ctx->battery_percent < APPS_BATTERY_LOW_THRESHOLD) {
            new_state = APP_BATTERY_STATE_LOW_CAP;
        } else if (local_ctx->battery_percent >= APPS_BATTERY_FULL_THRESHOLD) {
            new_state = APP_BATTERY_STATE_FULL;
        } else {
            new_state = APP_BATTERY_STATE_IDLE;
        }
    }

    APPS_LOG_MSGID_I(TILE_LOG_TAG"old battery state %d, new battery state %d", 2, old_state, new_state);
    return new_state;
}

static void app_tile_dump_adv_complete_info(bt_gap_le_srv_adv_complete_t *adv_cp)
{
    APPS_LOG_MSGID_I(TILE_LOG_TAG"adv cp:[instance:%d, evt:%d, result:%d, adv_events:%d, tx_p:%d, handle:0x%4x]", 6,
                  adv_cp->instance, adv_cp->adv_evt, adv_cp->result,
                  adv_cp->num_ext_adv_events, adv_cp->selected_tx_power,
                  adv_cp->conn_handle);
}

static void app_tile_ble_srv_adv_event_cb(bt_gap_le_srv_adv_complete_t *adv_cp)
{
    APPS_LOG_MSGID_I(TILE_LOG_TAG", adv_cp->instance: %d, g_app_tile_context.adv_instance: %d", 2, adv_cp->instance, g_app_tile_context.adv_instance);
    if (g_app_tile_context.adv_instance != adv_cp->instance) {
        APPS_LOG_MSGID_E(TILE_LOG_TAG"adv evt callback, instance error!", 0);
        return;
    }

    if (0 != adv_cp->result) {
        APPS_LOG_MSGID_E(TILE_LOG_TAG" adv evt callback, result error, result is %d", 1, adv_cp->result);
        return;
    }

    app_tile_dump_adv_complete_info(adv_cp);
    switch (adv_cp->adv_evt) {
        case BT_GAP_LE_SRV_ADV_REMOVED: {
            /* 0x00 */
        }
            break;

        case BT_GAP_LE_SRV_ADV_STARTED:
            /* 0x01 */
            g_app_tile_context.adv_state = APP_TILE_ADV_STARTED;
            break;

        case BT_GAP_LE_SRV_ADV_STOPPED: {
            /* 0x02 */
            g_app_tile_context.adv_state = APP_TILE_ADV_STOPPED;
            g_app_tile_context.conn_handle = (0 != adv_cp->conn_handle ? adv_cp->conn_handle : BT_HANDLE_INVALID);

            APPS_LOG_MSGID_I(TILE_LOG_TAG"adv stopped,now_adv_type is %d, tile next mode is %d, g_app_tile_context.conn_handle: %4x", 3,
                    g_app_tile_context.now_adv_type,
                    g_app_tile_context.next_mode,
                    g_app_tile_context.conn_handle);
            if (APP_TILE_MODE_STREAMING_2_IDLE == g_app_tile_context.next_mode) {
                if (APP_TILE_ADV_SCANNABLE == g_app_tile_context.now_adv_type) {
                    app_tile_start_advertising(true);
                } else if (APP_TILE_ADV_CONNECTABLE == g_app_tile_context.now_adv_type) {
                    app_tile_start_advertising(false);
                }
                APPS_LOG_MSGID_E(TILE_LOG_TAG"stop_unconn_adv,then start conn adv", 0);
                g_app_tile_context.next_mode = APP_TILE_MODE_NONE;
            }
        }
            break;

        case BT_GAP_LE_SRV_ADV_UPDATED:
            /* 0x03 */
            break;

        case BT_GAP_LE_SRV_ADV_FORCE_RESTART: {
            /* 0x04 */
            if (g_app_tile_context.next_adv_type > 0) {
                app_tile_start_advertising(g_app_tile_context.next_adv_type - 1);
                APPS_LOG_MSGID_E(TILE_LOG_TAG"Force restart adv,conn_adv %d", 1, g_app_tile_context.next_adv_type - 1);
            }
        }
            break;

        default :
            break;
    }

}

#if defined(MTK_AWS_MCE_ENABLE)
static bt_status_t app_tile_disconnect(bt_handle_t handle)
{
    bt_status_t status = 0;
    bt_hci_cmd_disconnect_t disconnect_para = {
        .connection_handle = 0x0200,
        .reason = BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION,
    };

    disconnect_para.connection_handle = handle;
    status = bt_gap_le_disconnect(&disconnect_para);
    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_disconnect, conn_handle: 0x%4x, status: 0x%4x", 2, handle, status);
    return status;
}
#endif

#ifdef MTK_BLE_IAS
#if defined(MTK_PROMPT_SOUND_ENABLE)
#include "prompt_control.h"

void app_tile_voice_prompt_callback(prompt_control_event_t event_id)
{
    if (event_id == PROMPT_CONTROL_MEDIA_END) {
        APPS_LOG_MSGID_I(TILE_LOG_TAG"voice prompt stop callback.", 0);
    }
}
#endif

#include "ble_ias.h"
void ble_ias_alert_level_write_callback(uint16_t conn_handle, uint8_t alert_level)
{
    APPS_LOG_MSGID_I(TILE_LOG_TAG"ble_ias_alert_level_write_callback, conn_handle: 0x%4x, alert_level: %d", 2, conn_handle, alert_level);

    if ((conn_handle) && (alert_level)) {
        /**< play tile song  */
        APPS_LOG_MSGID_I(TILE_LOG_TAG"play tile alert song,alert_level: %d", 1, alert_level);
#if defined(MTK_PROMPT_SOUND_ENABLE)
        uint8_t *tone_buf = NULL;
        uint32_t tone_size = 0;
        static const uint8_t voice_prompt_mix_mp3_tone_long[] = {
            #include "48k.mp3.long.hex"
            //#include "48k.mp3.hex"
        };
        tone_buf = (uint8_t *)voice_prompt_mix_mp3_tone_long;
        tone_size = sizeof(voice_prompt_mix_mp3_tone_long);
    #ifndef MTK_MP3_TASK_DEDICATE
        prompt_control_play_tone(VPC_MP3, tone_buf, tone_size, app_tile_voice_prompt_callback);
    #else
        prompt_control_play_sync_tone(VPC_MP3, tone_buf, tone_size, 0, app_tile_voice_prompt_callback);
    #endif
#endif
    }
}
#endif

static uint32_t app_tile_get_adv_data(multi_ble_adv_info_t *adv_info)
{
    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_get_adv_data, next_adv_type %d, tile next_mode %d", 2,
            g_app_tile_context.next_adv_type,
            g_app_tile_context.next_mode);

    /* ADV DATA */
    if ((NULL != adv_info->adv_data) && (NULL != adv_info->adv_data->data)) {
        uint8_t len = 0;
        /* adv_data AD_TYPE_FLAG */
        adv_info->adv_data->data[len] = 2;
        adv_info->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_FLAG;
        adv_info->adv_data->data[len + 2] = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE;
        len += 3;

        /* adv_data BT_GAP_LE_AD_TYPE_16_BIT_UUID_PAapp_tile_get_adv_data, next_adv_typeRT */
        adv_info->adv_data->data[len] = 3;
        adv_info->adv_data->data[len + 1] = BT_GAP_LE_AD_TYPE_16_BIT_UUID_PART;
        /* For tile active/inactive */
        if (app_tile_tmd_is_active()) {
            adv_info->adv_data->data[len + 2] = (UUID16_TILE_ACTIVE & 0x00FF);
            adv_info->adv_data->data[len + 3] = ((UUID16_TILE_ACTIVE & 0xFF00) >> 8);;
        } else {
            adv_info->adv_data->data[len + 2] = (UUID16_TILE_INACTIVE & 0x00FF);
            adv_info->adv_data->data[len + 3] = ((UUID16_TILE_INACTIVE & 0xFF00) >> 8);;
        }
        len += 4;

        adv_info->adv_data->data_length = len;
    }

    /* SCAN RSP */
    if (NULL != adv_info->scan_rsp) {
        int16_t device_name_len = 0;
        char device_name[BT_GAP_LE_MAX_DEVICE_NAME_LENGTH] = {0};
#ifdef MTK_AWS_MCE_ENABLE
        /* Earbuds */
        if (AUDIO_CHANNEL_L == ami_get_audio_channel()) {
            snprintf(device_name, BT_GAP_LE_MAX_DEVICE_NAME_LENGTH, "LE-TILE-L_%02x:%02x:%02x:%02x:%02x:%02x",
                    g_app_tile_context.local_random_addr[0],
                    g_app_tile_context.local_random_addr[1],
                    g_app_tile_context.local_random_addr[2],
                    g_app_tile_context.local_random_addr[3],
                    g_app_tile_context.local_random_addr[4],
                    g_app_tile_context.local_random_addr[5]);
        } else {
            snprintf(device_name, BT_GAP_LE_MAX_DEVICE_NAME_LENGTH, "LE-TILE-R_%02x:%02x:%02x:%02x:%02x:%02x",
                    g_app_tile_context.local_random_addr[0],
                    g_app_tile_context.local_random_addr[1],
                    g_app_tile_context.local_random_addr[2],
                    g_app_tile_context.local_random_addr[3],
                    g_app_tile_context.local_random_addr[4],
                    g_app_tile_context.local_random_addr[5]);
        }
#else
       /* Headset */
       snprintf(device_name, BT_GAP_LE_MAX_DEVICE_NAME_LENGTH, "LE-TILE_%02x:%02x:%02x:%02x:%02x:%02x",
               g_app_tile_context.local_random_addr[0],
               g_app_tile_context.local_random_addr[1],
               g_app_tile_context.local_random_addr[2],
               g_app_tile_context.local_random_addr[3],
               g_app_tile_context.local_random_addr[4],
               g_app_tile_context.local_random_addr[5]);
#endif
        device_name_len = strlen((char *)device_name);

        /* scan_rsp: AD_TYPE_NAME_COMPLETE */
        adv_info->scan_rsp->data[0] = device_name_len + 1;
        adv_info->scan_rsp->data[1] = BT_GAP_LE_AD_TYPE_NAME_COMPLETE;
        memcpy(&adv_info->scan_rsp->data[2], device_name, device_name_len);

        adv_info->scan_rsp->data_length = device_name_len + 2;
    }

    /* ADV PARAMETER */
    if (NULL != adv_info->adv_param) {
        adv_info->adv_param->advertising_event_properties = BT_HCI_ADV_EVT_PROPERTIES_MASK_SCANNABLE | BT_HCI_ADV_EVT_PROPERTIES_MASK_LEGACY_PDU;

       if (APP_TILE_ADV_CONNECTABLE == g_app_tile_context.next_adv_type) {
            adv_info->adv_param->advertising_event_properties |= BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE;
        }

        /* For tile active/inactive */
        if (app_tile_tmd_is_active()) {
            adv_info->adv_param->primary_advertising_interval_min = 3200;
            adv_info->adv_param->primary_advertising_interval_max = 3200;
        } else {
            adv_info->adv_param->primary_advertising_interval_min = 800;
            adv_info->adv_param->primary_advertising_interval_max = 800;
#ifdef ENABLE_TILE
            if (!tile_timer_running(TILE_ADVERTISING_TIMER)) {
               APPS_LOG_MSGID_I(TILE_LOG_TAG"[APP_TILE]Start TILE_ADVERTISING_TIMER", 0);
               /* Start the advertising timeout timer if not already running. */
               tile_timer_start(TILE_ADVERTISING_TIMER, TILE_ADVERTISING_DURATION / 10);
           } else {
               APPS_LOG_MSGID_I(TILE_LOG_TAG"[APP_TILE]Skip TILE_ADVERTISING_TIMER", 0);
           }
#endif
        }
        adv_info->adv_param->primary_advertising_channel_map = 0x07;
        adv_info->adv_param->own_address_type = BT_ADDR_RANDOM;
        adv_info->adv_param->advertising_filter_policy = BT_HCI_ADV_FILTER_ACCEPT_SCAN_CONNECT_FROM_ALL;
        adv_info->adv_param->advertising_tx_power = 0x0;
        adv_info->adv_param->primary_advertising_phy = BT_HCI_LE_ADV_PHY_1M;
        adv_info->adv_param->secondary_advertising_phy = BT_HCI_LE_ADV_PHY_1M;
    }

    g_app_tile_context.now_adv_type = g_app_tile_context.next_adv_type;
    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_get_adv_data, now_adv_type %d", 1, g_app_tile_context.now_adv_type);
    return 0;
}

void app_tile_start_advertising(bool conn_adv)
{
    /**< normal case.*/
    g_app_tile_context.next_adv_type = conn_adv ? APP_TILE_ADV_CONNECTABLE : APP_TILE_ADV_SCANNABLE;
    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_start_advertising, conn_adv: %d", 1, conn_adv);
    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_TILE, app_tile_get_adv_data);
    multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_TILE, app_tile_get_adv_data, 1);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_TILE);
}

void app_tile_stop_advertising(void)
{
    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_stop_advertising", 0);
    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_TILE, app_tile_get_adv_data);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_TILE);
}

/* Initialization on apps_init_applications function */
static bool app_tile_proc_ui_shell_group(
    struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    app_tile_local_context_t *local_ctx = (app_tile_local_context_t *)self->local_context;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
            APPS_LOG_MSGID_I(TILE_LOG_TAG"on create", 0);
            /* Init context of Tile APP. */
            app_tile_set_tile_active_dummy(false);
            self->local_context = &g_app_tile_context;
            memset(self->local_context, 0, sizeof(app_tile_local_context_t));
            local_ctx = (app_tile_local_context_t *)self->local_context;
            local_ctx->next_mode = APP_TILE_MODE_NONE;
            local_ctx->conn_handle = BT_HANDLE_INVALID;
            local_ctx->battery_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
            local_ctx->charging_state = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE);
            local_ctx->charger_exist_state = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
            local_ctx->shutdown_state = calculate_shutdown_state(battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE));
            local_ctx->battery_state = APP_BATTERY_STATE_IDLE;
            local_ctx->battery_state = get_battery_state(local_ctx);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            APPS_LOG_MSGID_I(TILE_LOG_TAG"on destroy", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            APPS_LOG_MSGID_I(TILE_LOG_TAG"on resume", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            APPS_LOG_MSGID_I(TILE_LOG_TAG"on pause", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
            APPS_LOG_MSGID_I(TILE_LOG_TAG"on refresh", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
            APPS_LOG_MSGID_I(TILE_LOG_TAG"on result", 0);
            break;
        default:
            break;
    }
    return ret;
}

static bool app_tile_proc_app_interaction_group(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;

    switch (event_id) {
#ifndef ENABLE_TILE
        case APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT: {
            /* Stop tile adv after 2 mins if tile is not activated */
            app_tile_stop_advertising();
            break;
        }
#endif  // ENABLE_TILE
        default:
            break;
    }

    return ret;
}

static bool app_tile_proc_bt_cm_group(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;

    return ret;

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role;
    role = bt_device_manager_aws_local_info_get_role();
#endif

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == remote_update) {
                break;
            }
        }
            break;

        default:
            break;
    }
}

static bool app_tile_proc_bt_sink_group(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;

    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_proc_bt_sink_group, event_id: 0x%8x", 1, event_id);

    switch (event_id) {
#ifdef MTK_AWS_MCE_ENABLE
        /* Partner disconnect BLE during music and eSCO mode */
        case BT_SINK_SRV_EVENT_LE_DISCONNECT: {
            APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_disconnect, conn_handle is 0x%4x", 1, g_app_tile_context.conn_handle);
            if (BT_HANDLE_INVALID != g_app_tile_context.conn_handle) {
                /* Idle to streaming */
                g_app_tile_context.next_mode = APP_TILE_MODE_IDLE_2_STREAMING;
                bt_status_t status = app_tile_disconnect(g_app_tile_context.conn_handle);
                APPS_LOG_MSGID_I(TILE_LOG_TAG"status 0x%8x", 1, status);
            } else {
                bt_gap_le_srv_adv_state_t adv_state = bt_gap_le_srv_get_adv_state(g_app_tile_context.adv_instance);
                APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_disconnect, conn_handle not found, adv_state %d", 1, adv_state);
                if (BT_CM_LE_ADV_STATE_TYPE_STARTED == adv_state) {
                    if (APP_TILE_ADV_SCANNABLE == g_app_tile_context.now_adv_type) {
                        /* do nothing */
                    } else {
                        /* Streaming to idle */
                        app_tile_stop_advertising();
                        g_app_tile_context.next_mode = APP_TILE_MODE_STREAMING_2_IDLE;
                        APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_stop_conn_adv", 0);
                    }
                } else if ((BT_CM_LE_ADV_STATE_TYPE_STOPPED == adv_state) || (BT_CM_LE_ADV_STATE_TYPE_REMOVED == adv_state)) {
                    app_tile_start_advertising(false);
                    APPS_LOG_MSGID_E(TILE_LOG_TAG"app_tile_start_unconn_adv", 0);
                }
            }
        }
            break;

        /* Partner stop unconnectable adv and start conn adv again */
        case BT_SINK_SRV_EVENT_LE_START_CONNECTABLE_ADV: {
            g_app_tile_context.next_mode = APP_TILE_MODE_STREAMING_2_IDLE;
            bt_gap_le_srv_adv_state_t adv_state = bt_gap_le_srv_get_adv_state(g_app_tile_context.adv_instance);
            if (BT_CM_LE_ADV_STATE_TYPE_STARTED == adv_state) {
                if (APP_TILE_ADV_CONNECTABLE == g_app_tile_context.now_adv_type) {
                    /* do nothing */
                } else {
                    /* Streaming to idle */
                    app_tile_stop_advertising();
                    g_app_tile_context.next_mode = APP_TILE_MODE_STREAMING_2_IDLE;
                    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_stop_unconn_adv", 0);
                }
            } else if ((BT_CM_LE_ADV_STATE_TYPE_STOPPED == adv_state) || (BT_CM_LE_ADV_STATE_TYPE_REMOVED == adv_state)) {
                app_tile_start_advertising(true);
                APPS_LOG_MSGID_E(TILE_LOG_TAG"app_tile_start_conn_adv", 0);
            }
        }
            break;
#endif
        default:
            break;
    }
    return ret;
}

static bool app_tile_proc_bt_group(
            struct _ui_shell_activity *self,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    apps_bt_event_data_t *bt_event_data = (apps_bt_event_data_t *)extra_data;
    if (bt_event_data == NULL) {
        APPS_LOG_MSGID_E(TILE_LOG_TAG"BT event, bt_event_data is NULL", 0);
        return ret;
    }
    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_proc_bt_group, event_id: 0x%8x", 1, event_id);

    switch (event_id) {
        case BT_POWER_ON_CNF: {
            APPS_LOG_MSGID_I(TILE_LOG_TAG"BT POWER ON", 0);
            if (!multi_ble_adv_manager_get_random_addr_and_adv_handle(MULTI_ADV_INSTANCE_TILE, (bt_bd_addr_t*)&g_app_tile_context.local_random_addr, &g_app_tile_context.adv_instance)) {
                APPS_LOG_MSGID_W(TILE_LOG_TAG"sync_random_addr & adv_instance failed", 0);
            }
            APPS_LOG_MSGID_I(TILE_LOG_TAG"random_addr:0x%x:%x:%x:%x:%x:%x", 6,
                    g_app_tile_context.local_random_addr[0],
                    g_app_tile_context.local_random_addr[1],
                    g_app_tile_context.local_random_addr[2],
                    g_app_tile_context.local_random_addr[3],
                    g_app_tile_context.local_random_addr[4],
                    g_app_tile_context.local_random_addr[5]);
#ifdef MTK_SMART_CHARGER_ENABLE
            /* MTK_SMART_CHARGER_ENABLE feature is only for earbuds project */
            if (app_tile_tmd_is_active()) {
                /* Start advertising immediately if Tile is active. */
                app_tile_start_advertising(true);
            } else {
                /* Start shipping mode advertising only if out of case. */
                if (app_smcharger_get_context()->smcharger_state == STATE_SMCHARGER_OUT_OF_CASE) {
                    app_tile_start_advertising(true);
#ifndef ENABLE_TILE
                    /* Start tile adv 2 mins timer if tile is not activated */
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT);
                    ui_shell_send_event(false,
                                    EVENT_PRIORITY_HIGNEST,
                                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT,
                                    NULL, 0,
                                    NULL, TILE_ADV_TIMER);
#endif  // ENABLE_TILE
                }
            }
#else
            app_tile_start_advertising(true);
#ifndef ENABLE_TILE
            if (!app_tile_tmd_is_active()) {
                /* Start tile adv 2 mins timer if tile is not activated */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT);
                ui_shell_send_event(false,
                                EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT,
                                NULL, 0,
                                NULL, TILE_ADV_TIMER);
            }
#endif  // ENABLE_TILE
#endif  // MTK_SMART_CHARGER_ENABLE
        }
            break;

        case BT_POWER_OFF_CNF: {
            APPS_LOG_MSGID_I(TILE_LOG_TAG"BT POWER ON", 0);
        }
            break;

        case BT_GAP_LE_CONNECT_IND: {
            /* 0x1000000a */
            bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)(bt_event_data->buffer);
            APPS_LOG_MSGID_I(TILE_LOG_TAG" connection_ind->local_addr %02x:%02x:%02x:%02x:%02x:%02x", 6,
                    connection_ind->local_addr.addr[0],
                    connection_ind->local_addr.addr[1],
                    connection_ind->local_addr.addr[2],
                    connection_ind->local_addr.addr[3],
                    connection_ind->local_addr.addr[4],
                    connection_ind->local_addr.addr[5]);
            if (0 == memcmp(&(connection_ind->local_addr.addr), &(g_app_tile_context.local_random_addr), sizeof(bt_bd_addr_t))) {
                 g_app_tile_context.conn_handle = connection_ind->connection_handle;
                 APPS_LOG_MSGID_I(TILE_LOG_TAG"BT GAP LE CONNECT IND, conn_handle 0x%4x", 1, g_app_tile_context.conn_handle);
            }
        }
            break;

        case BT_GAP_LE_DISCONNECT_IND: {
            /* 0x1000000d */
            APPS_LOG_MSGID_I(TILE_LOG_TAG"BT GAP LE DISCONNECT IND", 0);
            bt_gap_le_disconnect_ind_t *disc_ind = (bt_gap_le_disconnect_ind_t*)(bt_event_data->buffer);
            if ((disc_ind->connection_handle == g_app_tile_context.conn_handle)) {
                g_app_tile_context.conn_handle = BT_HANDLE_INVALID;
#ifdef MTK_AWS_MCE_ENABLE
                if ((BT_AWS_MCE_ROLE_PARTNER == bt_connection_manager_device_local_info_get_aws_role())) {
                    /* idle to streaming mode  */
                    if (APP_TILE_MODE_IDLE_2_STREAMING == g_app_tile_context.next_mode) {
                        g_app_tile_context.next_mode = APP_TILE_MODE_NONE;
                        /**< start Scannable BLE adv. */
                        app_tile_start_advertising(false);
                        APPS_LOG_MSGID_I(TILE_LOG_TAG"partner generate nonconn adv", 0);                                                                                                }
                }
#endif
            }
        }
            break;

        case BT_GAP_LE_CONNECTION_UPDATE_IND: {
            bt_gap_le_connection_update_ind_t *ind = (bt_gap_le_connection_update_ind_t *)(bt_event_data->buffer);
            uint16_t conn_handle = ind->conn_handle;
            APPS_LOG_MSGID_I(TILE_LOG_TAG"CONNECTION UPDATE: interval = %d\n", 1, conn_handle);
        }
            break;

        case BT_GAP_LE_READ_RSSI_CNF: {
            bt_hci_evt_cc_read_rssi_t *rssi = (bt_hci_evt_cc_read_rssi_t *)(bt_event_data->buffer);
            APPS_LOG_MSGID_I(TILE_LOG_TAG"conn handle %d", 1, rssi->handle);
        }
            break;

        default:
            break;
    }
    return ret;
}

static bool app_tile_proc_bt_dm_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    return ret;
}

static bool app_tile_proc_le_service_proc(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    APPS_LOG_MSGID_I(TILE_LOG_TAG"app_tile_proc_le_service_proc, event_id:%x", 1, event_id);

    switch (event_id) {
        case BT_GAP_LE_SRV_EVENT_ADV_COMPLETE: {
            bt_gap_le_srv_adv_complete_t *adv_complete = (bt_gap_le_srv_adv_complete_t *)extra_data;
            app_tile_ble_srv_adv_event_cb(adv_complete);
        }
            break;

        case BT_GAP_LE_SRV_EVENT_ADV_CLEARED: {
        }
            break;

        case BT_GAP_LE_SRV_EVENT_CONN_CLEARED:
           break;

        case BT_GAP_LE_SRV_EVENT_CONN_UPDATED:
           break;

        case BT_GAP_LE_SRV_EVENT_BLE_DISABLED: {
        }
            break;

        default:
            break;
    }

    return ret;
}

static bool app_tile_proc_battery_event_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    bool update_battery_state = true;
    app_tile_local_context_t *local_ctx = (app_tile_local_context_t *)self->local_context;

    switch (event_id) {
        /* Handle battery_percent changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_PERCENT_CHANGE:
            local_ctx->battery_percent = (int32_t)extra_data;
            APPS_LOG_MSGID_I(TILE_LOG_TAG",[DRV]Current battery_percent=%d", 1, (int32_t)extra_data);
            break;

        /* Handle charger_state changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_CHARGER_STATE_CHANGE:
            local_ctx->charging_state = (int32_t)extra_data;
            APPS_LOG_MSGID_I(TILE_LOG_TAG",[DRV]Current charging_state=%d", 1, local_ctx->charging_state);
            break;

        /* Handle shutdown_state changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_SHUTDOWN_STATE_CHANGE:
            local_ctx->shutdown_state = (battery_event_shutdown_state_t)extra_data;
            APPS_LOG_MSGID_I(TILE_LOG_TAG",[DRV]Current shutdown_state=%d", 1, local_ctx->shutdown_state);
            break;

        default:
            /* Ignore other battery event. */
            update_battery_state = false;
            break;
    }
    if (update_battery_state) {
        local_ctx-> battery_state = get_battery_state(local_ctx);
    }
    return ret;
}

bool app_tile_idle_activity_proc(struct _ui_shell_activity *self, uint32_t event_group, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = app_tile_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;

        /* UI Shell APP_INTERACTION events. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = app_tile_proc_app_interaction_group(self, event_id, extra_data, data_len);
            break;

#ifdef MTK_SMART_CHARGER_ENABLE
        /* MTK_SMART_CHARGER_ENABLE feature is only for earbuds project */
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE: {
            if (event_id == EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT) {
                if (((app_smcharger_public_event_para_t *)extra_data)->action == SMCHARGER_CHARGER_IN_ACTION) {
                    if (!app_tile_tmd_is_active()) {
#ifdef ENABLE_TILE
                        tile_timer_cancel(TILE_ADVERTISING_TIMER);
#else
                        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT);
#endif
                        /* Disable 0xFEEC advertising when in the charger. */
                        app_tile_stop_advertising();
                    }
                } else if (((app_smcharger_public_event_para_t *)extra_data)->action == SMCHARGER_CHARGER_OUT_ACTION) {
                    /* Start advertising when charge is out. */
                    app_tile_start_advertising(true);
#ifndef ENABLE_TILE
                    if (!app_tile_tmd_is_active()) {
                        /* Start tile adv 2 mins timer if tile is not activated */
                        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT);
                        ui_shell_send_event(false,
                                        EVENT_PRIORITY_HIGNEST,
                                        EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT,
                                        NULL, 0,
                                        NULL, TILE_ADV_TIMER);
                     }
#endif  // ENABLE_TILE

                }
            }
        }
            break;
#endif

        /* UI Shell BT Connection Manager events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = app_tile_proc_bt_cm_group(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_BT_SINK:
            ret = app_tile_proc_bt_sink_group(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_BT:
            ret = app_tile_proc_bt_group(self, event_id, extra_data, data_len);
            break;

#ifdef MTK_AWS_MCE_ENABLE
        /* UI Shell BT AWS_DATA events. */
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            break;
#endif
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER:
            ret = app_tile_proc_bt_dm_group(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_LE_SERVICE:
            ret = app_tile_proc_le_service_proc(self, event_id, extra_data, data_len);
            break;

        /* UI Shell battery events - handle battery_change/charger_state/shutdown_state from battery event. */
        case EVENT_GROUP_UI_SHELL_BATTERY:
            ret = app_tile_proc_battery_event_group(self, event_id, extra_data, data_len);
            break;

        default:
            break;
    }
    return ret;
}

bool app_tile_tmd_is_active(void) {
#ifdef ENABLE_TILE
    return tile_tmd_is_active();
#else
    return app_tile_get_tile_active_dummy();
#endif
}

bool app_tile_toa_waiting_authentication(void) {
#ifdef ENABLE_TILE
    return tile_toa_waiting_authentication();
#else
    return false;
#endif
}

void app_tile_toa_allow_association(void) {
#ifdef ENABLE_TILE
    tile_toa_allow_association(true);
#endif
}

bool app_tile_song_is_in_progress(void) {
#ifdef ENABLE_TILE
    return tile_song_is_in_progress();
#else
    return false;
#endif
}

bool app_tile_get_tile_active_dummy(void) {
    return g_app_tile_tmd_is_active_state;
}

void app_tile_set_tile_active_dummy(bool is_active) {
    g_app_tile_tmd_is_active_state = is_active;
    APPS_LOG_MSGID_I(TILE_LOG_TAG"simulate tile active state %d", 1, g_app_tile_tmd_is_active_state);
}

app_battery_state_t app_tile_get_battery_state(void) {
    return g_app_tile_context.battery_state;
}

void app_tile_simulte_tile_activation(bool is_active) {
#ifdef ENABLE_TILE
    APPS_LOG_MSGID_I(TILE_LOG_TAG"project contains tile core library, ignore simulate tile activation", 0);
#else
    APPS_LOG_MSGID_I(TILE_LOG_TAG"simulate tile activation", 0);
    /* simulate tile is activated/inactivated */
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT);
    app_tile_stop_advertising();
    app_tile_set_tile_active_dummy(is_active);
    app_tile_start_advertising(true);
    if (!is_active) {
        ui_shell_send_event(false,
                         EVENT_PRIORITY_HIGNEST,
                         EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                         APPS_EVENTS_INTERACTION_TILE_ADV_TIMEOUT,
                         NULL, 0,
                         NULL, TILE_ADV_TIMER);
    }
#endif
}

#endif  /* AIR_TILE_ENABLE */
