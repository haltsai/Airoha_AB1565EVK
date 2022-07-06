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
 * File: app_hfp_activity.c
 *
 * Description:
 * This file is the activity to handle the key action or sink service event when call is not active.
 * This activity will start the app_hfp_activity when the call is active or there is a call coming
 * in. In addition, this activity handle the key action of the voice assistant, and report battery
 * level to smartphone.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "app_hfp_idle_activity.h"
#include "apps_config_event_list.h"
#include "apps_aws_sync_event.h"
#include "battery_management.h"
#include "battery_management_core.h"
#include "apps_events_battery_event.h"
#include "apps_events_event_group.h"
#include "apps_config_key_remapper.h"
#include "apps_events_key_event.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_ami.h"

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif

#define APP_SIDE_TONE_VOLUME_MIN_LEVEL  (-100)  /* The side tone min volume level. */
#define APP_SIDE_TONE_VOLUME_MAX_LEVEL  (24)    /* The side tone max volume level. */
#define APP_SIDE_TONE_CHANGE_LEVEL_PRE_STEP  (3)    /* When press key once, how much increase or decrease. */
static int32_t s_current_side_tone_volume;

static hfp_context_t app_hfp_context;   /* The variable records context */

#ifdef MTK_AWS_MCE_ENABLE
bool app_hfp_aws_data_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;

        apps_aws_sync_event_decode(aws_data_ind, &event_group, &action);
        APPS_LOG_MSGID_I("Received action = %x to paly reject call, role = %x, event_group = %x", 3, action, role, event_group);
        if (event_group == EVENT_GROUP_UI_SHELL_KEY && action == KEY_REJCALL) {
            /* The VP of call rejected only prompt in Agent. If partner rejected a call, this key action will sync to Agent. */
            if (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE) {
                apps_config_stop_voice(true, 200, true);
                app_hfp_set_incoming_call_vp_flag(0);
                apps_config_set_vp(VP_INDEX_CALL_REJECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
        } else if (event_group == EVENT_GROUP_UI_SHELL_KEY && action == KEY_WAKE_UP_VOICE_ASSISTANT) {
            /* Start voice assistant when receiving the key action from the partner. */
            if (role == BT_AWS_MCE_ROLE_AGENT) {
                hfp_context_t *local_context = (hfp_context_t *)self->local_context;
                bool active = !local_context->voice_assistant;
                if (BT_STATUS_SUCCESS == bt_sink_srv_send_action(BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE, &active)) {
                    local_context->voice_assistant = active;
                } else {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", Agent send BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE fail", 0);
                }
            }
        }
    }

    return ret;
}
#endif

static void app_report_battery_to_remote(int32_t bat_val, int32_t pre_val)
{
    /* Need transfer, bt sink action level is 0~9. */
    bat_val = bat_val / 10;
    if (bat_val == 10) {
        /* bal_val is 100. */
        bat_val = 9;
    }

    pre_val = pre_val / 10;
    if (pre_val == 10) {
        /* bal_val is 100. */
        pre_val = 9;
    }

    /* Report current battery level to remote device, such as smartphone. */
    APPS_LOG_MSGID_I(APP_HFP_ACTI", bat_val : %d, pre_val : %d", 2, bat_val, pre_val);
    if (pre_val != bat_val) {
        bt_status_t status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_REPORT_BATTERY, &bat_val);
        APPS_LOG_MSGID_I(APP_HFP_ACTI", status : %x", 1, status);
    }
}

bool app_hfp_idle_start_hfp_activity(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
    if (param == NULL) {
        return ret;
    }
    APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", hfp_state: param->now = %x, param->pre = %x", 2, param->current, param->previous);

    hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
    if (hfp_context == NULL) {
        return ret;
    }

    /* Update context of hfp app, and start the app_hfp_acitvity. */

    hfp_context->pre_state = param->previous;
    hfp_context->curr_state = param->current;

    apps_config_state_t pre_state = app_get_config_status_by_state(param->previous);
    apps_config_state_t state = app_get_config_status_by_state(param->current);

    APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", hfp_activity: state = %x", 1, state);
    if (APP_TOTAL_STATE_NO == pre_state && state != APP_TOTAL_STATE_NO) {
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", hfp_activity: pre_state = %x, curr_state = %x", 2, hfp_context->pre_state, hfp_context->curr_state);
        ui_shell_start_activity(NULL, app_hfp_activity_proc, ACTIVITY_PRIORITY_MIDDLE, (void *)hfp_context, 0);
    }
    return false;
}


static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", create", 0);
            self->local_context = &app_hfp_context;
            memset(self->local_context, 0, sizeof(hfp_context_t));
            hfp_context_t *local_context = (hfp_context_t *)self->local_context;
            local_context->esco_connected = false;
            s_current_side_tone_volume = APP_SIDE_TONE_VOLUME_MIN_LEVEL;
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}


static bool _proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    hfp_context_t *local_context = (hfp_context_t *)self->local_context;
    uint8_t key_id;
    airo_key_event_t key_event;

#ifdef MTK_BT_SPEAKER_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (role == BT_AWS_MCE_ROLE_PARTNER || role == BT_AWS_MCE_ROLE_CLIENT) {
        return ret;
    }
#endif

    app_event_key_event_decode(&key_id, &key_event, event_id);

    apps_config_key_action_t action;
    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }

    switch (action) {
        case KEY_WAKE_UP_VOICE_ASSISTANT_NOTIFY:
            /* Only notify user long press time is up. */
#if !defined(MTK_BT_SPEAKER_ENABLE)
            apps_config_set_vp(VP_INDEX_PRESS, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
#endif
            ret = true;
            break;
        case KEY_WAKE_UP_VOICE_ASSISTANT:
        case KEY_WAKE_UP_VOICE_ASSISTANT_CONFIRM:
            /* If user release the key after long press, trigger voice assistant on agent side. */
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, KEY_WAKE_UP_VOICE_ASSISTANT)) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", Partner send KEY_WAKE_UP_VOICE_ASSISTANT aws to agent failed", 0);
                    if (KEY_WAKE_UP_VOICE_ASSISTANT == action) {
                        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    }
                }
            } else
#endif
            {
                bool active = !local_context->voice_assistant;
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", Agent set voice_recognition to %d", 1, active);
                if (BT_STATUS_SUCCESS == bt_sink_srv_send_action(BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE, &active)) {
                    local_context->voice_assistant = active;
                } else {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", Agent send BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE fail", 0);
                }
            }
            ret = true;
            break;
        case KEY_REDIAL_LAST_CALL:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", receive KEY_REDIAL_LAST_CALL", 0);
            apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            bt_sink_srv_send_action(BT_SINK_SRV_ACTION_DIAL_LAST, NULL);
            ret = true;
            break;
        /* Adjust voice assistant volume.
         * This event could not be handled in app_hfp_activity, because only esco state is updated for voice assistant.
         */
        case KEY_VOICE_UP:
            if (local_context->esco_connected) {
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CALL_VOLUME_UP, NULL);
            }
            break;
        case KEY_VOICE_DN:
            if (local_context->esco_connected) {
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN, NULL);
            }
            break;
    }

    return ret;
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
static bool app_hfp_idle_proc_rotary_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    bsp_rotary_encoder_port_t port;
    bsp_rotary_encoder_event_t event;
    uint32_t rotary_data;
    hfp_context_t *local_context = (hfp_context_t *)self->local_context;
    if (!extra_data) {
        return ret;
    }
    apps_config_key_action_t key_action = *(uint16_t *)extra_data;
    app_event_rotary_event_decode(&port, &event, &rotary_data, event_id);
    switch (key_action) {
        case KEY_VOICE_UP:
        case KEY_VOICE_DN: {
            if (local_context->esco_connected) {
                uint8_t volume = bt_sink_srv_get_volume(NULL, BT_SINK_SRV_VOLUME_HFP);
                if (KEY_VOICE_UP == key_action) {
                    if (volume + rotary_data < 15) {
                        volume += rotary_data;
                    } else {
                        volume = 15;
                    }
                } else {
                    if (volume > rotary_data) {
                        volume -= rotary_data;
                    } else {
                        volume = 0;
                    }
                }
                APPS_LOG_MSGID_I(APP_HFP_ACTI"app_hfp_idle_proc_rotary_event_group, hpf volume : %d", 1, volume);
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CALL_SET_VOLUME, &volume);
                ret = true;
            }
            break;
            case KEY_AUDIO_SIDE_TONE_VOLUME_UP:
            case KEY_AUDIO_SIDE_TONE_VOLUME_DOWN: {
                int32_t target_side_tone = 0;
                if (KEY_AUDIO_SIDE_TONE_VOLUME_UP == key_action) {
                    target_side_tone = s_current_side_tone_volume + rotary_data * APP_SIDE_TONE_CHANGE_LEVEL_PRE_STEP;
                } else {
                    target_side_tone = s_current_side_tone_volume - rotary_data * APP_SIDE_TONE_CHANGE_LEVEL_PRE_STEP;
                }
                /* Check the range. */
                if (target_side_tone < APP_SIDE_TONE_VOLUME_MIN_LEVEL) {
                    target_side_tone = APP_SIDE_TONE_VOLUME_MIN_LEVEL;
                } else if (target_side_tone > APP_SIDE_TONE_VOLUME_MAX_LEVEL) {
                    target_side_tone = APP_SIDE_TONE_VOLUME_MAX_LEVEL;
                }
                if (target_side_tone == s_current_side_tone_volume) {
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                } else {
                    bt_sink_srv_am_result_t am_result;
                    am_result = am_audio_side_tone_set_volume(target_side_tone);
                    APPS_LOG_MSGID_I(APP_HFP_ACTI": set side tone level %d -> %d, status: 0x%x", 3,
                        s_current_side_tone_volume, target_side_tone, am_result);
                    if (AUD_EXECUTION_SUCCESS == am_result) {
                        s_current_side_tone_volume = target_side_tone;
                    }
                }
                ret = true;
                break;
            }
        }
        default:
            break;
    }

    return ret;
}
#endif

static bool app_hfp_idle_proc_bt_cm_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
            if (remote_update && hfp_context) {
                if (!(remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP))
                    && remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)) {
                    /* Report battery level to remote device when HFP connected. */
                    int32_t bat_val = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
                    hfp_context->battery_level = bat_val;
                    hfp_context->hfp_connected = true;
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", bat_val: %d", 1, bat_val);
                    app_report_battery_to_remote(hfp_context->battery_level, 0xFFFF);
                } else if (remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                           && !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP))) {
                    /* Update the state of voice assistant when HFP disconnected. */
                    hfp_context->voice_assistant = false;
                    hfp_context->hfp_connected = false;
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", Set voice_recognition to false when HFP disconnected", 0);
                }
#ifdef MTK_AWS_MCE_ENABLE
                if (BT_AWS_MCE_ROLE_PARTNER == role || BT_AWS_MCE_ROLE_CLINET == role) {
                    if (remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)
                        && !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                        /* Update the state of voice assistant for partner when aws disconnected. */
                        hfp_context->voice_assistant = false;
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", Partner disconnect", 0);
                    }
                }
#endif
            }
        }
        break;
        default:
            break;
    }

    return ret;
}

bool app_hfp_idle_activity_proc(
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
        case EVENT_GROUP_UI_SHELL_KEY: {
            /* key event. */
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef AIR_ROTARY_ENCODER_ENABLE
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER: {
            /* Rotary encoder events. */
            ret = app_hfp_idle_proc_rotary_event_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* BT_SINK events, indicates the state of voice assistant or esco connection. */
            /* APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", BT_SINK : %x", 1, event_id); */
            if (BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED == event_id) {
                /* Update the state of voice assistant by BT_SINK event. */
                hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
                bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)extra_data;
                hfp_context->voice_assistant = event->voice_recognition.enable;
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", voice_recognition changed : %d", 1, hfp_context->voice_assistant);
            } else if (BT_SINK_SRV_EVENT_STATE_CHANGE == event_id) {
                ret = app_hfp_idle_start_hfp_activity(self, event_id, extra_data, data_len);
            } else if (BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE == event_id) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI" SCO_STATE", 0);
                bt_sink_srv_sco_state_update_t *sco_state = (bt_sink_srv_sco_state_update_t *)extra_data;
                if (sco_state) {
                    /* Update esco connection state. */
                    hfp_context_t *ctx = (hfp_context_t *)(self->local_context);
                    ctx->esco_connected = sco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED ? true : false;
                    APPS_LOG_MSGID_I(APP_HFP_ACTI", SCO_STATE: %d", 1, sco_state->state);
                }
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            /* Event come from bt connection manager, indicates the connection state of HFP or AWS. */
            ret = app_hfp_idle_proc_bt_cm_event_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BATTERY: {
            /* When battery level changed, report it to remote device. */
            if (event_id ==  APPS_EVENTS_BATTERY_PERCENT_CHANGE) {
                hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
                if (hfp_context == NULL) {
                    return ret;
                }
                int32_t pre_battery = hfp_context->battery_level;
                int32_t curr_battery = (int32_t)extra_data;
#ifdef MTK_AWS_MCE_ENABLE
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", current_role=%d, pre_bat: %d, curr_bat = %d, cur_state = %x",
                        4, role, pre_battery, curr_battery, hfp_context->curr_state);
#else
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", pre_bat: %d, curr_bat = %d, cur_state = %x",
                        3, pre_battery, curr_battery, hfp_context->curr_state);
#endif
                if (hfp_context->hfp_connected && curr_battery != pre_battery) {
#ifdef MTK_AWS_MCE_ENABLE
                    if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
                    {
                        app_report_battery_to_remote(curr_battery, pre_battery);
                        hfp_context->battery_level = curr_battery;
                    }
                }
            }
            break;
        }

        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            /* Report battery level to remote device after RHO completed for new agent. */
            if (APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT == event_id) {
                app_rho_result_t rho_ret = (app_rho_result_t)extra_data;
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", rho_ret=%d", 1, rho_ret);
                if (APP_RHO_RESULT_SUCCESS == rho_ret) {
                    hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
                    int32_t bat_val = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
                    hfp_context->battery_level = bat_val;
                    app_report_battery_to_remote(hfp_context->battery_level, 0xFFFF);
                }
            }
#endif
            break;
        }

#if defined(MTK_AWS_MCE_ENABLE)
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            /* Handle the key event sync from the partner side. */
            ret = app_hfp_aws_data_proc(self, event_id, extra_data, data_len);
            break;
#endif
        default:
            break;
    }

    /* APPS_LOG_MSGID_I(UI_SHELL_IDLE_HFP_ACTIVITY", ret : %x", 1, ret); */
    return ret;
}

