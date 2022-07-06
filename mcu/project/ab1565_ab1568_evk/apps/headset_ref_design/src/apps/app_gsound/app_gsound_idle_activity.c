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
 * File: app_gsound_idle_activity.c
 *
 * Description: This file is the activity to handle the action from APP layer.
 */


#ifdef GSOUND_LIBRARY_ENABLE

#include "app_gsound_idle_activity.h"
#include "app_gsound_multi_va.h"
#include "app_gsound_service.h"
#include "app_home_screen_idle_activity.h"
#include "app_bt_conn_componet_in_homescreen.h"
#include "apps_events_key_event.h"
#include "apps_config_event_list.h"
#include "gsound_port_interface.h"
#include "bt_sink_srv.h"
#include "bt_device_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "ui_shell_activity.h"
#include "ui_shell_manager.h"
#include "apps_config_key_remapper.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_aws_sync_event.h"
#include "apps_debug.h"
#include "battery_management.h"
#include "bt_sink_srv_ami.h"
#include "gsound_service.h"
#include "apps_control_touch_key_status.h"
#if defined(MTK_AWS_MCE_ENABLE)
#include "bt_aws_mce_report.h"
#endif
#ifndef MULTI_VA_SUPPORT_COMPETITION
#include "multi_va_manager.h"
#endif
#ifdef MTK_SMART_CHARGER_ENABLE
#include "app_smcharger_idle_activity.h"
#else
#include "app_battery_transient_activity.h"
#endif
#include "app_bt_takeover_service.h"
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_utils.h"
#endif
#ifdef MTK_ANC_ENABLE
#include "anc_control_api.h"
#endif
#include "gsound_debug.h"
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "bt_ull_service.h"
#endif

#define TAG "[GS][APP] "

/**
 * @brief State of GSound key.
 */
enum {
    KEY_NOT_PREPARED, /**< key state none */
    KEY_PREPARED,     /**< key state press */
    KEY_RELEASE,      /**< key state release */
    KEY_PTT,          /**< key state Push To Talk */
};

typedef struct {
    bool local_is_charging;
    bool partner_is_charging;
    uint32_t local_percent;
    uint32_t partner_percent;
    uint32_t case_percent;
} app_gsound_battery_info_t;

static app_gsound_battery_info_t app_gsound_battery_status;

static uint32_t app_gsound_is_key_prepared;
static bool gsound_connected = false;
static bool s_le_streaming = false;

static void app_gsound_idle_activity_update_battery(void)
{
    int32_t capacity, charger_status;
    capacity = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    charger_status = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);

    gsound_port_send_battery_info(charger_status, capacity, NULL);
}

bool app_gsound_takeover_service_allow(const bt_bd_addr_t remote_addr)
{
    bool audio_not_word = gsound_port_audio_check_no_audio_work();
    bool hotword_work = gsound_port_hotword_is_detect();

    if (audio_not_word && !hotword_work) {
        return true;
    }
    return false;
}

extern bt_status_t gsound_port_device_action_state_update(void);
static void app_gsound_set_battery_info(uint8_t info_type, uint32_t data)
{
    GSOUND_LOG_I(TAG"set_battery_info info_type=0x%X, data=0x%X", 2,
                     info_type, data);

    switch (info_type) {
        case APP_GSOUND_BATTERY_INFO_LOCAL_PERCENT:
            app_gsound_battery_status.local_percent = data;
            break;

        case APP_GSOUND_BATTERY_INFO_LOCAL_IS_CHARGING:
            app_gsound_battery_status.local_is_charging = data;
            break;

        case APP_GSOUND_BATTERY_INFO_PARTNER_PERCENT:
            app_gsound_battery_status.partner_percent = data;
            break;

        case APP_GSOUND_BATTERY_INFO_PARTNER_IS_CHARGING:
            app_gsound_battery_status.partner_is_charging = data;
            break;

        case APP_GSOUND_BATTERY_INFO_CASE:
            app_gsound_battery_status.case_percent = data;
            break;

        default:
            return;
    }

    gsound_port_device_action_state_update();
}

static void app_gsound_update_battery_status_init(void)
{
    uint32_t percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
    uint32_t is_charging = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);

    GSOUND_LOG_I(TAG"update_battery_status_init percent=0x%X, is_charging=0x%X", 2,
                     percent, is_charging);

    memset(&app_gsound_battery_status, 0, sizeof(app_gsound_battery_status));
    app_gsound_set_battery_info(APP_GSOUND_BATTERY_INFO_LOCAL_PERCENT, percent);
    app_gsound_set_battery_info(APP_GSOUND_BATTERY_INFO_LOCAL_IS_CHARGING, is_charging);
}

void app_gsound_audio_am_vendor_se_callback_handler(vendor_se_event_t event, void *arg)
{
    if (event == EVENT_BLE_START) {
        s_le_streaming = true;
        GSOUND_LOG_I(TAG"am_vendor_se_callback. s_le_streaming=%d", 1, s_le_streaming);
    } else if (event == EVENT_BLE_STOP) {
        s_le_streaming = false;
        GSOUND_LOG_I(TAG"am_vendor_se_callback. s_le_streaming=%d", 1, s_le_streaming);
    }
}

static bool gsound_idle_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true; // UI shell internal event must process by this activity, so default is true
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            GSOUND_LOG_I(TAG"create", 0);
#ifndef MULTI_VA_SUPPORT_COMPETITION
            if (multi_va_manager_get_current_va_type() == MULTI_VA_TYPE_GSOUND)
#endif
            {
#ifdef GSOUND_SUPPORT_TWS
                GSoundServiceInitAsTws();
#else
                GSoundServiceInitAsStereo();
#endif

#if defined(GSOUND_HOTWORD_ENABLE)
                GSoundServiceInitHotwordExternal();
#endif
            }
            app_gsound_idle_activity_update_battery();
            app_gsound_update_battery_status_init();
            app_bt_takeover_service_user_register(APP_BT_TAKEOVER_ID_BISTO, app_gsound_takeover_service_allow);
            am_vendor_se_id_t am_se_id = ami_get_vendor_se_id();
            bt_sink_srv_am_result_t am_se_result = ami_register_vendor_se(am_se_id, app_gsound_audio_am_vendor_se_callback_handler);
            GSOUND_LOG_I(TAG"ama_audio_init, am_se_id : %d, am_se_result : %d", 2, am_se_id, am_se_result);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            GSOUND_LOG_I(TAG"destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            GSOUND_LOG_I(TAG"resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            GSOUND_LOG_I(TAG"pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            GSOUND_LOG_I(TAG"refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            GSOUND_LOG_I(TAG"result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool app_gsound_is_ull_upstreaming()
{
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    bt_ull_streaming_info_t info = {0};
    bt_status_t ret = BT_STATUS_FAIL;
    bt_ull_streaming_t streaming = {
        .streaming_interface = BT_ULL_STREAMING_INTERFACE_MICROPHONE,
        .port = 0,
    };
    ret = bt_ull_get_streaming_info(streaming, &info);
    GSOUND_LOG_I(TAG"is_ull_upstreaming, result : 0x%x, playing : %d", 2, ret, info.is_playing);
    if (BT_STATUS_SUCCESS == ret) {
        return info.is_playing;
    }
    return false;
#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */
    return false;
}

/*
 * return true means the key event valid on gsound, else invalid
 */
static GSOUND_PORT_ACTION_EVENT app_gsound_component_map_keyevent_action(apps_config_key_action_t key_event)
{
    GSOUND_PORT_ACTION_EVENT action = GSOUND_PORT_ACTION_NON;
    switch (key_event) {
        case KEY_GSOUND_PRESS:
            if (bt_sink_srv_ami_get_current_scenario() != HFP) {
                app_gsound_is_key_prepared = KEY_PREPARED;
                action = (GSOUND_PORT_ACTION_GA_VOICE_PREPARE | GSOUND_PORT_ACTION_GA_FETCH_PREPARE);
            }
            break;
        case KEY_GSOUND_VOICE_QUERY:
            #ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
            if (app_gsound_is_ull_upstreaming()) {
                GSOUND_LOG_I(TAG"ull streaming, do not process key event.", 0);
                action = (GSOUND_PORT_ACTION_GA_VOICE_PTT);
                break;
            }
            #endif
            if (app_gsound_is_key_prepared == KEY_PREPARED) {
                gsound_port_send_action_event((GSOUND_PORT_ACTION_GA_VOICE_PREPARE | GSOUND_PORT_ACTION_GA_FETCH_PREPARE), NULL);
                gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_VOICE_PTT_OR_FETCH | GSOUND_PORT_ACTION_GA_WILL_PAUSE, NULL);
                app_gsound_is_key_prepared = KEY_PTT;
                action = (GSOUND_PORT_ACTION_GA_VOICE_PTT);
            }
            break;
        case KEY_GSOUND_ENDPOINTING:
            if (app_gsound_is_key_prepared == KEY_RELEASE) {
                gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_STOP_ASSISTANT | GSOUND_PORT_ACTION_ENDPOINTING_START | GSOUND_PORT_ACTION_TOGGLE_PLAY_PAUSE, NULL);
                action = (GSOUND_PORT_ACTION_GA_STOP_ASSISTANT | GSOUND_PORT_ACTION_ENDPOINTING_START | GSOUND_PORT_ACTION_TOGGLE_PLAY_PAUSE);
            }
            break;
        case KEY_GSOUND_NOTIFY:
            break;
        case KEY_GSOUND_CANCEL:
        case KEY_AVRCP_PLAY:
        case KEY_AVRCP_PAUSE:
            gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_VOICE_PREPARE | GSOUND_PORT_ACTION_GA_FETCH_PREPARE, NULL);
            gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_VOICE_DONE, NULL);
            gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_WILL_PAUSE, NULL);
            if (gsound_port_send_action_event(GSOUND_PORT_ACTION_TOGGLE_PLAY_PAUSE | GSOUND_PORT_ACTION_GA_STOP_ASSISTANT, NULL) == 0) {
                action = GSOUND_PORT_ACTION_GA_STOP_ASSISTANT;
            }
            break;
        case KEY_GSOUND_RELEASE:
            if (app_gsound_is_key_prepared == KEY_PREPARED) {
                app_gsound_is_key_prepared = KEY_RELEASE;
                action = GSOUND_PORT_ACTION_GA_VOICE_DONE;
            } else if (app_gsound_is_key_prepared == KEY_PTT) {
                gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_VOICE_DONE, NULL);
                app_gsound_is_key_prepared = KEY_NOT_PREPARED;
                action = GSOUND_PORT_ACTION_GA_VOICE_DONE;
            }
            break;
        case KEY_AVRCP_FORWARD: {
            gsound_port_send_action_event((GSOUND_PORT_ACTION_GA_VOICE_PREPARE | GSOUND_PORT_ACTION_GA_FETCH_PREPARE), NULL);
            gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_VOICE_DONE, NULL);
            gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_WILL_PAUSE, NULL);
            if (gsound_port_send_action_event(GSOUND_PORT_ACTION_NEXT_TRACK, NULL) == 0) {
                action = GSOUND_PORT_ACTION_NEXT_TRACK;
            }
            break;
        }
        case KEY_AVRCP_BACKWARD: {
            gsound_port_send_action_event((GSOUND_PORT_ACTION_GA_VOICE_PREPARE | GSOUND_PORT_ACTION_GA_FETCH_PREPARE), NULL);
            gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_VOICE_DONE, NULL);
            gsound_port_send_action_event(GSOUND_PORT_ACTION_GA_WILL_PAUSE, NULL);
            if (gsound_port_send_action_event(GSOUND_PORT_ACTION_PREV_TRACK, NULL) == 0) {
                action = GSOUND_PORT_ACTION_PREV_TRACK;
            }
            break;
        }
        default:
            break;
    }
    return action;
}

/*
 * return true means the event is used by gsound, so it will not be send to next app
 */
static bool app_gsound_component_proc_key_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    uint8_t key_id;
    airo_key_event_t key_event;
    GSOUND_PORT_ACTION_EVENT gsound_event;

    app_event_key_event_decode(&key_id, &key_event, event_id);

    apps_config_key_action_t action;
    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }
    if (s_le_streaming) {
        GSOUND_LOG_I(TAG"s_le_streaming=1", 0);
        if (action != KEY_GSOUND_RELEASE) {
            return false;
        }
    }

    GSOUND_LOG_I(TAG"component_proc_key_event:0x%X,0x%X,0x%X,0x%X,0x%X", 5, event_id, key_id, key_event, action, bt_sink_srv_ami_get_current_scenario());
    if ((action >= KEY_GSOUND_PRESS && action <= KEY_GSOUND_CANCEL) || action == KEY_AVRCP_PLAY || action == KEY_AVRCP_PAUSE || action == KEY_AVRCP_FORWARD || action == KEY_AVRCP_BACKWARD) {
        if(!app_gsound_is_push_to_talk_enabled())
        {
            GSOUND_LOG_I(TAG"component_proc_key_event_group. Key invalid", 0);
            return true;
        }

#ifdef MTK_AWS_MCE_ENABLE
        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
            if (app_home_screen_idle_activity_is_aws_connected()) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action)) {
                    GSOUND_LOG_I(TAG"Partner send gsound action %d aws to agent failed", 1, action);
                }
            } else {
                apps_config_set_vp(VP_INDEX_FAILED, false, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                GSOUND_LOG_I(TAG"Partner aws disconnected for GSOUND action=%d", 1, action);
            }
        } else
#endif
        {
            if ((gsound_event = app_gsound_component_map_keyevent_action(action)) == GSOUND_PORT_ACTION_NON) {
                GSOUND_LOG_E(TAG"gsound_port_send_action_event failed", 0);
                if (KEY_AVRCP_PLAY == action || KEY_AVRCP_PAUSE == action) {
                    gsound_port_action_rejected(GSOUND_PORT_ACTION_TOGGLE_PLAY_PAUSE);
                }
            }
        }

        ret = true;
    }

    return ret;
}

/*
 * return true means the event is used by gsound, so it will not be processed by music
 */
bool app_gsound_idle_activity_proc_music_event(bt_sink_srv_action_t sink_action)
{
    bool ret = false;
    if ((sink_action == BT_SINK_SRV_ACTION_PLAY_PAUSE || sink_action == BT_SINK_SRV_ACTION_NEXT_TRACK || sink_action == BT_SINK_SRV_ACTION_PREV_TRACK)
        && GSOUND_PORT_ACTION_EVENT_MAX != gsound_port_remap_action(sink_action)) {
        if (gsound_connected) {
            GSOUND_LOG_I(TAG"Intercept music key.", 0);
            ret = true;
        }
    }

    return ret;
}

/*
 * return 0 means the action reject handle success, else is error.
 */
uint8_t gsound_port_action_rejected(GSOUND_PORT_ACTION_EVENT action)
{
    uint8_t result = 0;
    bt_sink_srv_action_t sink_action = gsound_port_remap_action_reverse(action);

    if (sink_action != -1) {
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_GSOUND_ACTION_REJECTED, (void *)sink_action, 0,
                            NULL, 0);
    }

    return result;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool gsound_idle_aws_report_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    GSOUND_PORT_ACTION_EVENT gsound_action;

    switch (aws_data_ind->module_id) {
        case BT_AWS_MCE_REPORT_MODULE_APP_ACTION: {
            uint32_t aws_event_group;
            uint32_t aws_event_id;
            uint32_t aws_event_extra_data_len;
            void *aws_event_extra_data;
            apps_aws_sync_event_decode_extra(aws_data_ind, &aws_event_group, &aws_event_id, &aws_event_extra_data, &aws_event_extra_data_len);

            switch (aws_event_group) {
                case EVENT_GROUP_UI_SHELL_KEY:
                    if ((aws_event_id >= KEY_GSOUND_PRESS && aws_event_id <= KEY_GSOUND_CANCEL) ||
                         aws_event_id == KEY_AVRCP_PLAY || aws_event_id == KEY_AVRCP_PAUSE || aws_event_id == KEY_AVRCP_FORWARD || aws_event_id == KEY_AVRCP_BACKWARD) {
                        if ((gsound_action = app_gsound_component_map_keyevent_action(aws_event_id)) == GSOUND_PORT_ACTION_NON) {
                            GSOUND_LOG_E(TAG"gsound_port_send_action_event failed", 0);
                            if (KEY_AVRCP_PLAY == aws_event_id || KEY_AVRCP_PAUSE == aws_event_id) {
                                gsound_port_action_rejected(GSOUND_PORT_ACTION_TOGGLE_PLAY_PAUSE);
                            }
                        }
                        ret = true;
                    }
                    break;

                case EVENT_GROUP_UI_SHELL_GSOUND:{
                    switch(aws_event_id){
                        case APPS_EVENTS_INTERACTION_GSOUND_PARTNER_ENABLE: {
                            GSOUND_LOG_I(TAG"AWS rx GSOUND enable. state=%d", 1, app_gsound_get_enable_state());
                            if(BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role() && APP_GSOUND_DISABLED == app_gsound_get_enable_state()){
                                app_gsound_enable();
                                gsound_connected = true;
                            }
                            break;
                        }

                        case APPS_EVENTS_INTERACTION_GSOUND_PARTNER_DISABLE: {
                            GSOUND_LOG_I(TAG"AWS rx GSOUND disable. state=%d", 1, app_gsound_get_enable_state());
                            if(BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role() && APP_GSOUND_ENABLED == app_gsound_get_enable_state()){
                                app_gsound_disable();
                                gsound_connected = false;
                            }
                            break;
                        }

                        case APPS_EVENTS_INTERACTION_GSOUND_POWER_OFF_SYNC:{
                            GSOUND_LOG_I(TAG"APPS_EVENTS_INTERACTION_GSOUND_POWER_OFF_SYNC", 0);
                            bool *need_rho = (bool*)pvPortMalloc(sizeof(bool));
                            if (need_rho == NULL) {
                                GSOUND_LOG_E(TAG"malloc failed", 0);
                                break;
                            }
                            *need_rho = false;
                            ui_shell_status_t status = ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF, (void *)need_rho, sizeof(bool),
                                    NULL, 0);
                            if (status != UI_SHELL_STATUS_OK) {
                                vPortFree(need_rho);
                                GSOUND_LOG_E(TAG"APPS_EVENTS_INTERACTION_GSOUND_POWER_OFF_SYNC send event failed", 0);
                            }
                            break;
                        }

                        case APPS_EVENTS_INTERACTION_GSOUND_CONNECTED:
                            if(BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()){
                                gsound_connected = true;
                                GSOUND_LOG_I(TAG"aws APPS_EVENTS_INTERACTION_GSOUND_CONNECTED", 0);
                            }
                            break;
                        case APPS_EVENTS_INTERACTION_GSOUND_DISCONNECTED:
                            if(BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()){
                                gsound_connected = false;
                                GSOUND_LOG_I(TAG"aws APPS_EVENTS_INTERACTION_GSOUND_DISCONNECTED", 0);
                            }
                            break;
                        default:
                            break;
                    }
                }

                default:
                    break;
            }

            break;
        }

        case BT_AWS_MCE_REPORT_MODULE_BATTERY: {
            bool partner_is_charging;
            uint32_t partner_percent;

#ifdef MTK_SMART_CHARGER_ENABLE
            app_smcharger_context_t smcharger_context;
            memcpy(&smcharger_context.peer_battery_percent, aws_data_ind->param, sizeof(smcharger_context.peer_battery_percent));
            partner_percent = smcharger_context.peer_battery_percent & (~ PARTNER_BATTERY_CHARGING);
            partner_is_charging = (smcharger_context.peer_battery_percent & PARTNER_BATTERY_CHARGING) ? true : false;
#else
            battery_local_context_type_t battery_context;
            memcpy(&battery_context.partner_battery_percent, aws_data_ind->param, sizeof(battery_context.partner_battery_percent));
            partner_percent = battery_context.partner_battery_percent & (~ PARTNER_BATTERY_CHARGING);
            partner_is_charging = (battery_context.partner_battery_percent & PARTNER_BATTERY_CHARGING) ? true : false;
#endif

            GSOUND_LOG_I(TAG"received partner battery info percent=0x%X, is_charging=0x%X", 2,
                             partner_percent, partner_is_charging);

            app_gsound_set_battery_info(APP_GSOUND_BATTERY_INFO_PARTNER_PERCENT, partner_percent);
            app_gsound_set_battery_info(APP_GSOUND_BATTERY_INFO_PARTNER_IS_CHARGING, partner_is_charging);

            break;
        }
    }

    return ret;
}
#endif

uint32_t app_gsound_get_battery_info(uint8_t info_type)
{
    switch (info_type) {
        case APP_GSOUND_BATTERY_INFO_LOCAL_PERCENT:
            return app_gsound_battery_status.local_percent;
            break;

        case APP_GSOUND_BATTERY_INFO_LOCAL_IS_CHARGING:
            return app_gsound_battery_status.local_is_charging;
            break;

        case APP_GSOUND_BATTERY_INFO_PARTNER_PERCENT:
            return app_gsound_battery_status.partner_percent;
            break;

        case APP_GSOUND_BATTERY_INFO_PARTNER_IS_CHARGING:
            return app_gsound_battery_status.partner_is_charging;
            break;

        case APP_GSOUND_BATTERY_INFO_CASE:
            return app_gsound_battery_status.case_percent;
            break;
    }

    return 0;
}

static bool app_gsound_update_battery_status(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case APPS_EVENTS_BATTERY_PERCENT_CHANGE:
            GSOUND_LOG_I(TAG"update_battery_status percent=0x%X", 1, extra_data);
            app_gsound_set_battery_info(APP_GSOUND_BATTERY_INFO_LOCAL_PERCENT, (uint32_t)extra_data);
            break;
    }

    return false;
}

#ifdef MTK_SMART_CHARGER_ENABLE
static bool app_gsound_proc_charger_case_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT: {
            app_smcharger_public_event_para_t *event_para = (app_smcharger_public_event_para_t *)extra_data;

            switch (event_para->action) {
                case SMCHARGER_CHARGER_IN_ACTION:
                    GSOUND_LOG_I(TAG"SMCHARGER_CHARGER_IN_ACTION", 0);
                    app_gsound_set_battery_info(APP_GSOUND_BATTERY_INFO_LOCAL_IS_CHARGING, true);
                    break;

                case SMCHARGER_CHARGER_OUT_ACTION:
                    GSOUND_LOG_I(TAG"SMCHARGER_CHARGER_OUT_ACTION", 0);
                    app_gsound_set_battery_info(APP_GSOUND_BATTERY_INFO_LOCAL_IS_CHARGING, false);
                    break;

                case SMCHARGER_CASE_BATTERY_REPORT_ACTION:
                    GSOUND_LOG_I(TAG"SMCHARGER_CASE_BATTERY_REPORT_ACTION percent=0x%X", 1, (uint32_t)event_para->data);
                    app_gsound_set_battery_info(APP_GSOUND_BATTERY_INFO_CASE, (uint32_t)event_para->data);
                    break;
            }
        }
        break;
    }
    return ret;
}
#endif

#ifdef MTK_IN_EAR_FEATURE_ENABLE
static GSOUND_PORT_OHD_STATE app_gsound_get_ohd_info() {
    app_in_ear_ohd_state_t sta;

    sta = app_in_ear_get_ohd_state();
    switch(sta) {
        case APP_IN_EAR_OHD_BOTH_DETECTED:
            return GSOUND_PORT_OHD_BOTH_DETECTED;
        case APP_IN_EAR_OHD_DETECTED:
            return GSOUND_PORT_OHD_SINGULAR_DETECTED;
        case APP_IN_EAR_OHD_NONE_DETECTED:
            return GSOUND_PORT_OHD_NONE_DETECTED;
        case APP_IN_EAR_OHD_LEFT_DETECTED:
            return GSOUND_PORT_OHD_LEFT_DETECTED;
        case APP_IN_EAR_OHD_RIGHT_DETECTED:
            return GSOUND_PORT_OHD_RIGHT_DETECTED;
        case APP_IN_EAR_OHD_DISABLED:
            return GSOUND_PORT_OHD_DISABLE;
    }

    return GSOUND_PORT_OHD_DISABLE;
}

bt_status_t gsound_port_ohd_get_status(GSOUND_PORT_OHD_STATE *ohd_status_out)
{
    *ohd_status_out = app_gsound_get_ohd_info();
    GSOUND_LOG_I(TAG"[OHD] default_gsound_port_ohd_get_status=0x%X", 1, *ohd_status_out);
    return BT_STATUS_SUCCESS;
}
#endif

bool app_gsound_idle_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = gsound_idle_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;
            switch (event_id) {
                case APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE: {
                    if (local_ctx->connection_state == BT_SINK_SRV_STATE_POWER_ON) {
                        app_gsound_idle_activity_update_battery();
                    }
                }
                break;
#ifdef MTK_IN_EAR_FEATURE_ENABLE
                case APPS_EVENTS_INTERACTION_IN_EAR_UPDATE_STA: {
                    gsound_port_ohd_set_status(app_gsound_get_ohd_info());
                }
                break;
#endif
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = app_gsound_component_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
#if defined(MTK_AWS_MCE_ENABLE)
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            ret = gsound_idle_aws_report_event_proc(self, event_id, extra_data, data_len);
            break;
        }
#endif

#ifdef MTK_SMART_CHARGER_ENABLE
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE:
            ret = app_gsound_proc_charger_case_group(self, event_id, extra_data, data_len);
            break;
#endif

        case EVENT_GROUP_UI_SHELL_BATTERY: {
            app_gsound_idle_activity_update_battery();
            app_gsound_update_battery_status(self, event_id, extra_data, data_len);
            break;
        }

#ifdef MTK_ANC_ENABLE
        case EVENT_GROUP_UI_SHELL_AUDIO_ANC: {
            if (event_id == AUDIO_ANC_CONTROL_EVENT_ON || event_id == AUDIO_ANC_CONTROL_EVENT_OFF || event_id == AUDIO_ANC_CONTROL_EVENT_SET_REG) {
                gsound_port_device_action_state_update();
            }
            break;
        }
#endif

        case EVENT_GROUP_UI_SHELL_GSOUND: {
            switch (event_id) {
                case APPS_EVENTS_INTERACTION_GSOUND_ENABLED:{
#if defined(MTK_AWS_MCE_ENABLE)
                    uint32_t status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_GSOUND, APPS_EVENTS_INTERACTION_GSOUND_PARTNER_ENABLE);
                    GSOUND_LOG_I(TAG"GSound AWS send Enable status=0x%X", 1, status);
#endif
                    app_gsound_multi_va_notify_connected();
                    gsound_connected = true;
                    break;
                }

                case APPS_EVENTS_INTERACTION_GSOUND_CONNECTED: {
#if defined(MTK_AWS_MCE_ENABLE)
                    uint32_t status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_GSOUND, APPS_EVENTS_INTERACTION_GSOUND_CONNECTED);
                    GSOUND_LOG_I(TAG"GSound AWS send connect status=0x%X", 1, status);
#endif
                    gsound_connected = true;
                    app_gsound_multi_va_notify_connected();
                    break;
                }

                case APPS_EVENTS_INTERACTION_GSOUND_DISABLED: {
#if defined(MTK_AWS_MCE_ENABLE)
                    uint32_t status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_GSOUND, APPS_EVENTS_INTERACTION_GSOUND_PARTNER_DISABLE);
                    GSOUND_LOG_I(TAG"GSound AWS send Disable status=0x%X", 1, status);
#endif
                    app_gsound_multi_va_notify_disconnected(true);
                    gsound_connected = false;
                    break;
                }

                case APPS_EVENTS_INTERACTION_GSOUND_DISCONNECTED: {
#if defined(MTK_AWS_MCE_ENABLE)
                    uint32_t status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_GSOUND, APPS_EVENTS_INTERACTION_GSOUND_DISCONNECTED);
                    GSOUND_LOG_I(TAG"GSound AWS send disconnect status=0x%X", 1, status);
#endif
                    gsound_connected = false;
                    app_gsound_multi_va_notify_disconnected(false);
                    break;
                }

                default:
                    break;
            }

            ret = true;
            break;
        }

        case EVENT_GROUP_UI_SHELL_TOUCH_KEY: {
            switch(event_id)
            {
                case APPS_TOUCH_KEY_STATUS_UPDATE: {
                    gsound_port_device_action_state_update();
                    break;
                }
            }

            break;
        }

#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            if (event_id == BT_CM_EVENT_REMOTE_INFO_UPDATE) {
                if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                    bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
                    if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                           uint32_t status = 0;
                           if (gsound_connected) {
                                status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_GSOUND, APPS_EVENTS_INTERACTION_GSOUND_CONNECTED);
                           } else {
                                status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_GSOUND, APPS_EVENTS_INTERACTION_GSOUND_DISCONNECTED);
                           }
                           GSOUND_LOG_I(TAG"sync connection status=%d to partner when partner connected, result=%d.", 2, gsound_connected, status);
                    }
                }
            }
        }
#endif
        default:
            break;
    }
    return ret;
}

#endif
