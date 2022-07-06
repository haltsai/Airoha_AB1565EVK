
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

/**
 * File: app_service.c
 *
 * Description: This file is the activity to handle common event.
 *
 */

#include "app_dongle_service.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_config_event_list.h"
#include "apps_dongle_sync_event.h"
#include "apps_debug.h"
#include "apps_events_battery_event.h"
#include "bt_connection_manager.h"
#include "battery_management.h"
#include "battery_management_core.h"


#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE

#define TAG "[APP SERVICE]"

#define APP_SERVICE_CALLBACK_TABLE_COUNT        3

typedef struct {
    bool is_used;
    app_dongle_service_callback_t callback_table;
} app_srv_callback_table_t;

typedef struct {
    bool                        ctx_is_dongle_connected;
    bool                        ctx_is_handshake_done;
    app_dongle_service_dongle_mode_t       ctx_dongle_mode;
    uint32_t                    ctx_fw_version;
    app_srv_callback_table_t    ctx_callback[APP_SERVICE_CALLBACK_TABLE_COUNT];
} app_service_context_t;

static app_service_context_t s_srv_ctx;

static void app_srv_ctx_reset() {
    s_srv_ctx.ctx_is_dongle_connected = false;
    s_srv_ctx.ctx_is_handshake_done = false;
    s_srv_ctx.ctx_dongle_mode = APP_DONGLE_SERVICE_DONGLE_MODE_NONE;
    s_srv_ctx.ctx_fw_version = 0;
    //memset(&s_srv_ctx, 0, sizeof(app_service_context_t));
}

static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(TAG" create", 0);
            app_srv_ctx_reset();
        }
        break;
        default:
            break;
    }
    return ret;
}

static bool _proc_bt_cm_event(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    static bt_bd_addr_t dongle_address;

    if (event_id == BT_CM_EVENT_REMOTE_INFO_UPDATE) {
        /* Record dongle address */
        bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;

        if ((!(remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL)))
                && (remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL))) {

            memcpy(dongle_address, remote_update->address, sizeof(bt_bd_addr_t));

            APPS_LOG_MSGID_I(TAG" Connected dongle with address : 0x%02X:%02X:%02X:%02X:%02X:%02X. Send Hello", 6,
                dongle_address[0], dongle_address[1],
                dongle_address[2], dongle_address[3],
                dongle_address[4], dongle_address[5]);

            s_srv_ctx.ctx_is_dongle_connected = true;

            apps_dongle_sync_event_send(EVENT_GROUP_UI_SHELL_APP_SERVICE, APP_DONGLE_SERVICE_HEADSET_EVENT_HANDSHAKE_START);

            /**
             * @brief Send the HELLO message in a loop in case of dongle connected, but
             * send the HELLO message failed.
             */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_SERVICE,
                                        APP_DONGLE_SERVICE_HEADSET_EVENT_HANDSHAKE_START, NULL, 0,
                                        NULL, 100);

        } else if ((!(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL)))
                && (remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL))) {

            if (0 == memcmp(dongle_address, remote_update->address, sizeof(bt_bd_addr_t))) {
                APPS_LOG_MSGID_I(TAG" Disconnected with dongle", 0);
                app_srv_ctx_reset();

                memset(dongle_address, 0, sizeof(bt_bd_addr_t));

                uint8_t callback_index = 0;
                for (callback_index = 0; callback_index < APP_SERVICE_CALLBACK_TABLE_COUNT; callback_index ++) {
                    if ((s_srv_ctx.ctx_callback[callback_index].is_used == true)
                            && (s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_disconnected != NULL)) {
                        s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_disconnected();
                    }
                }
            }
        }
    }
    return false;
}

static void app_srv_send_fw_version()
{
    uint8_t version[APP_DONGLE_SERVICE_HEADSET_FW_VERSION_BUF_LEN] = {0x00};
    version[0] = (s_srv_ctx.ctx_fw_version & 0x000000FF);
    version[1] = (s_srv_ctx.ctx_fw_version & 0x0000FF00) >> 8;
    version[2] = (s_srv_ctx.ctx_fw_version & 0x00FF0000) >> 16;
    version[3] = (s_srv_ctx.ctx_fw_version & 0xFF000000) >> 24;

    APPS_LOG_MSGID_I(TAG" Send FW version : 0x%x, version 0x%02x %02x %02x %02x", 5,
                s_srv_ctx.ctx_fw_version, version[0], version[1], version[2], version[3]);

    apps_dongle_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_SERVICE,
                                    APP_DONGLE_SERVICE_HEADSET_EVENT_VERSION,
                                    version,
                                    APP_DONGLE_SERVICE_HEADSET_FW_VERSION_BUF_LEN);
}

static void app_srv_send_charger_state()
{
    int32_t charging_status = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE);
    bool charging = false;
    if (charging_status != CHARGER_STATE_CHR_OFF) {
        charging = true;
    }
    APPS_LOG_MSGID_I(TAG" Send charging status : %d, %d", 2, charging_status, charging);

    apps_dongle_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_SERVICE,
                                    APP_DONGLE_SERVICE_HEADSET_EVENT_CHARGING_STATE,
                                    (void*)&charging,
                                    sizeof(bool));
}

static void app_srv_send_battery_level()
{
    int32_t battery_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    APPS_LOG_MSGID_I(TAG" Send battery percent : %d", 1, battery_percent);

    apps_dongle_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_SERVICE,
                                    APP_DONGLE_SERVICE_HEADSET_EVENT_BATTERY_LEVEL,
                                    (void*)&battery_percent,
                                    sizeof(int32_t));
}

static bool _proc_bt_ull_data_event(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;
    uint8_t callback_index = 0;

    apps_dongle_event_sync_info_t *pkg = (apps_dongle_event_sync_info_t*)extra_data;
    if (pkg->event_group == EVENT_GROUP_UI_SHELL_APP_SERVICE) {

        if (s_srv_ctx.ctx_is_dongle_connected == false) {
            APPS_LOG_MSGID_E(TAG" ULL Data event, dongle is not connected", 0);
            return false;
        }

        switch (pkg->event_id) {
            case APP_DONGLE_SERVICE_DONGLE_EVENT_HANDSHAKE_START_ACK: {
                APPS_LOG_MSGID_I(TAG" Dongle Response Handshake Start ACK", 0);

                app_srv_send_fw_version();
                app_srv_send_charger_state();
                app_srv_send_battery_level();

                /**
                 * @brief When received the handshake start ACK message, need remove the event.
                 */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_SERVICE, APP_DONGLE_SERVICE_HEADSET_EVENT_HANDSHAKE_START);

                /**
                 * @brief When send all of the init information, send the handshake done event.
                 */
                apps_dongle_sync_event_send(EVENT_GROUP_UI_SHELL_APP_SERVICE, APP_DONGLE_SERVICE_HEADSET_EVENT_HADNSHAKE_DONE);
            }
            break;

            case APP_DONGLE_SERVICE_DONGLE_EVENT_HANDSHAKE_DONE_ACK: {
                APPS_LOG_MSGID_I(TAG" Dongle Response Handshake Done ACK", 0);
                s_srv_ctx.ctx_is_handshake_done = true;

                for (callback_index = 0; callback_index < APP_SERVICE_CALLBACK_TABLE_COUNT; callback_index ++) {
                    if ((s_srv_ctx.ctx_callback[callback_index].is_used == true)
                            && (s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_connected != NULL)) {
                        s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_connected();
                    }
                }
            }
            break;

            case APP_DONGLE_SERVICE_DONGLE_EVENT_MODE_UPDATE: {

                if (s_srv_ctx.ctx_is_handshake_done == false) {
                    APPS_LOG_MSGID_I(TAG" [DONGLE_MODE_UPDATE] The Headset set is not handshake done with dongle", 0);
                    break;
                }

                s_srv_ctx.ctx_dongle_mode = *((app_dongle_service_dongle_mode_t*)&(pkg->data));
                APPS_LOG_MSGID_I(TAG" Dongle Report MODE : %d", 1, s_srv_ctx.ctx_dongle_mode);

                for (callback_index = 0; callback_index < APP_SERVICE_CALLBACK_TABLE_COUNT; callback_index ++) {
                    if ((s_srv_ctx.ctx_callback[callback_index].is_used == true)
                            && (s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_mode_changed != NULL)) {
                        s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_mode_changed(s_srv_ctx.ctx_dongle_mode);
                    }
                }
            }
            break;

            case APP_DONGLE_SERVICE_DONGLE_EVENT_STATE_OFF: {
                if (s_srv_ctx.ctx_is_handshake_done == false) {
                    APPS_LOG_MSGID_I(TAG" [DONGLE_STATE_OFF] The Headset set is not handshake done with dongle", 0);
                    break;
                }
                /**
                 * @brief Need handle the dongle state off event
                 * Maybe headset should power off
                 */
                for (callback_index = 0; callback_index < APP_SERVICE_CALLBACK_TABLE_COUNT; callback_index ++) {
                    if ((s_srv_ctx.ctx_callback[callback_index].is_used == true)
                            && (s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_off != NULL)) {
                        s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_off();
                    }
                }
            }
            break;

            case APP_DONGLE_SERVICE_DONGLE_EVENT_STATE_RESET: {
                if (s_srv_ctx.ctx_is_handshake_done == false) {
                    APPS_LOG_MSGID_I(TAG" [DONGLE_STATE_RESET] The Headset set is not handshake done with dongle", 0);
                    break;
                }
                /**
                 * @brief Need handle the dongel state reset event
                 * Maybe reconnect
                 */
                for (callback_index = 0; callback_index < APP_SERVICE_CALLBACK_TABLE_COUNT; callback_index ++) {
                    if ((s_srv_ctx.ctx_callback[callback_index].is_used == true)
                            && (s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_reset != NULL)) {
                        s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_reset();
                    }
                }
            }
            break;

            case APP_DONGLE_SERVICE_DONGLE_EVENT_VOLUME_STATE_UPDATE: {
                if (s_srv_ctx.ctx_is_handshake_done == false) {
                    APPS_LOG_MSGID_I(TAG" [DONGLE_VOLUME_STATE_UPDATE] The Headset set is not handshake done with dongle", 0);
                    break;
                }
                app_srv_volume_change_t *volume_status = (app_srv_volume_change_t* )(pkg->data);
                APPS_LOG_MSGID_I(TAG" Dongle Report Volume Status, new volume 0x%04x, changed : 0x%02x", 2,
                                    volume_status->new_volume, volume_status->which_changed);
                for (callback_index = 0; callback_index < APP_SERVICE_CALLBACK_TABLE_COUNT; callback_index ++) {
                    if ((s_srv_ctx.ctx_callback[callback_index].is_used == true)
                            && (s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_volume_status_changed != NULL)) {
                        s_srv_ctx.ctx_callback[callback_index].callback_table.dongle_volume_status_changed(volume_status);
                    }
                }
            }
            break;

            default:
            break;
        }
    }

    return ret;
}

static bool _proc_battery_data_event(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case APPS_EVENTS_BATTERY_PERCENT_CHANGE: {
            if (s_srv_ctx.ctx_is_handshake_done == false) {
                APPS_LOG_MSGID_I(TAG"_proc_battery_data_event_proc_battery_data_event [BATTERY_CHANGE] The Headset set is not handshake done with dongle", 0);
                break;
            }
            app_srv_send_battery_level();
        }
        break;

        case APPS_EVENTS_BATTERY_CHARGER_STATE_CHANGE:{
            if (s_srv_ctx.ctx_is_handshake_done == false) {
                APPS_LOG_MSGID_I(TAG"_proc_battery_data_event [CHARGER_STATE_CHANGE] The Headset set is not handshake done with dongle", 0);
                break;
            }
            app_srv_send_charger_state();
        }
        break;
    }

    return false;
}

bool app_dongle_service_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            /* UI Shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            ret = _proc_bt_cm_event(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_DONGLE_DATA: {
            ret = _proc_bt_ull_data_event(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BATTERY: {
            ret = _proc_battery_data_event(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_SERVICE: {
            if (event_id == APP_DONGLE_SERVICE_HEADSET_EVENT_HANDSHAKE_START) {
                apps_dongle_sync_event_send(EVENT_GROUP_UI_SHELL_APP_SERVICE, APP_DONGLE_SERVICE_HEADSET_EVENT_HANDSHAKE_START);

                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_SERVICE,
                                        APP_DONGLE_SERVICE_HEADSET_EVENT_HANDSHAKE_START, NULL, 0,
                                        NULL, 100);
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            if (event_id == APPS_EVENTS_INTERACTION_POWER_OFF) {
                apps_dongle_sync_event_send(EVENT_GROUP_UI_SHELL_APP_SERVICE, APP_DONGLE_SERVICE_HEADSET_EVENT_POWER_OFF);
            }
        }
        break;
        default: {
            break;
        }
    }

    return ret;
}

void app_dongle_service_set_fw_version(uint32_t fw_version)
{
    s_srv_ctx.ctx_fw_version = fw_version;
}

bool app_dongle_service_is_dongle_connected() {
    return s_srv_ctx.ctx_is_dongle_connected && s_srv_ctx.ctx_is_handshake_done;
}

app_dongle_service_dongle_mode_t app_dongle_service_get_dongle_mode() {
    return s_srv_ctx.ctx_dongle_mode;
}

void app_dongle_service_register_callback(app_dongle_service_callback_t callback_table)
{
    uint8_t index = 0;
    bool register_succeed = false;

    for (index = 0; index < APP_SERVICE_CALLBACK_TABLE_COUNT; index ++) {
        if (s_srv_ctx.ctx_callback[index].is_used == false) {
            memcpy(&(s_srv_ctx.ctx_callback[index].callback_table), &callback_table, sizeof(app_dongle_service_callback_t));
            s_srv_ctx.ctx_callback[index].is_used = true;

            register_succeed = true;
            break;
        }
    }

    if (register_succeed == false) {
        APPS_LOG_MSGID_I(TAG" app_dongle_service_register_callback, register failed, all callback table has been used", 0);
    }
}

#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */

