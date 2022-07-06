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
#include "app_hfp_va_activity.h"
#include "app_hfp_utils.h"
#include "apps_config_event_list.h"
#include "apps_aws_sync_event.h"
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
#include "battery_management.h"
#include "battery_management_core.h"
#endif
#include "apps_events_battery_event.h"
#include "apps_events_event_group.h"
#include "apps_config_key_remapper.h"
#include "apps_config_audio_helper.h"
#include "apps_events_key_event.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_ami.h"
#include "nvdm.h"
#include "nvdm_id_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#include "bt_sink_srv.h"
#include "bt_sink_srv_common.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif
#include "app_home_screen_idle_activity.h"
#ifdef AIR_MULTI_POINT_ENABLE
#include "app_bt_emp_service.h"
#endif
#include "atci.h"
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_idle_activity.h"
#include "app_in_ear_utils.h"

#define  HFP_AUTO_ACCEPT_TIMER   (6*1000)
#endif

#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le.h"
#include "bt_le_audio_util.h"
#include "bt_sink_srv_le_volume.h"
#ifdef AIR_LE_AUDIO_MULTIPOINT_ENABLE
#include "bt_sink_srv_le_cap.h"
#endif
#endif

#define APP_SIDE_TONE_VOLUME_MIN_LEVEL  (-100)  /* The side tone min volume level. */
#define APP_SIDE_TONE_VOLUME_MAX_LEVEL  (24)    /* The side tone max volume level. */
#define APP_SIDE_TONE_CHANGE_LEVEL_PRE_STEP  (3)    /* When press key once, how much increase or decrease. */

static hfp_context_t s_app_hfp_context;   /* The variable records context */

#ifdef MTK_AWS_MCE_ENABLE
static bool app_hfp_idle_proc_aws_data(
        ui_shell_activity_t *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        //apps_aws_sync_event_decode(aws_data_ind, &event_group, &action);
        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action,
                        &p_extra_data, &extra_data_len);
        if (event_group == EVENT_GROUP_UI_SHELL_KEY && action == KEY_REJCALL) {
            /* The VP of call rejected only prompt in Agent. If partner rejected a call, this key action will sync to Agent. */
            if (role == BT_AWS_MCE_ROLE_AGENT) {
                app_voice_prompt_set_voice_stopping(true);
                app_hfp_stop_vp();
                apps_config_set_vp(VP_INDEX_CALL_REJECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
        } else if ((event_group == EVENT_GROUP_UI_SHELL_KEY && action == KEY_WAKE_UP_VOICE_ASSISTANT)
                || (event_group == EVENT_GROUP_UI_SHELL_KEY && action == KEY_INTERRUPT_VOICE_ASSISTANT)) {
            /* Start voice assistant when receiving the key action from the partner. */
            if (role == BT_AWS_MCE_ROLE_AGENT) {
                hfp_context_t *local_context = (hfp_context_t *)self->local_context;
                //bool active = !local_context->voice_assistant;
                bool active;
                if (action == KEY_WAKE_UP_VOICE_ASSISTANT) {
                    active = true;
                } else {
                    active = false;
                }
                if (BT_STATUS_SUCCESS == bt_sink_srv_send_action(BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE, &active)) {
                    local_context->voice_assistant = active;
                    if (active) {
                        ui_shell_start_activity(NULL, app_hfp_va_activity_proc, ACTIVITY_PRIORITY_HIGH, (void *)local_context, 0);
                    } else {
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
                    }
                } else {
                    APPS_LOG_MSGID_I(APP_HFP_IDLE", Agent send BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE fail", 0);
                }
            }
        } else if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && action == APPS_EVENTS_INTERACTION_SYNC_INCOMING_CALL_VP) {
            app_hfp_incoming_call_vp_play_status_t vp_play_status = *(app_hfp_incoming_call_vp_play_status_t *)p_extra_data;
            if (BT_AWS_MCE_ROLE_PARTNER == bt_connection_manager_device_local_info_get_aws_role()) {
                APPS_LOG_MSGID_I(APP_HFP_IDLE" [RING_IND] received vp_play_status=%d", 1, vp_play_status);
                app_hfp_set_incoming_call_vp_flag(vp_play_status);
            }
        } else if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && action == APPS_EVENTS_INTERACTION_SYNC_MIC_MUTE_STATUS) {
            bool mute = *(bool *)p_extra_data;
            hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [MUTE] received mic mute_status=%d", 1, mute);
            //if ( hfp_context->esco_connected != 0) {
                hfp_context->mute_mic = mute;
                /* Mute or unmute mic.*/
                bt_sink_srv_set_mute(BT_SINK_SRV_MUTE_MICROPHONE, mute);
#ifdef AIR_LE_AUDIO_ENABLE
                bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, mute);
#endif
#ifdef MTK_AWS_MCE_ENABLE
                if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                    if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
                        /* Play prompt voice.*/
                        apps_config_set_vp(VP_INDEX_SUCCESSED, TRUE, 200, VOICE_PROMPT_PRIO_MEDIUM, FALSE, NULL);
                    } else {
                        apps_config_set_vp(VP_INDEX_SUCCESSED, FALSE, 0, VOICE_PROMPT_PRIO_MEDIUM, FALSE, NULL);
                    }
                }
#endif
        } else if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && action == APPS_EVENTS_INTERACTION_SYNC_VA_STATE) {
            if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()) {
                hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
                bool voice_assistant = *(bool *)p_extra_data;
                APPS_LOG_MSGID_I(APP_HFP_UTILS", Receive voice_assistant from Agent = %d", 1, voice_assistant);
                if (voice_assistant != hfp_context->voice_assistant) {
                    hfp_context->voice_assistant = voice_assistant;
                    if (voice_assistant) {
                        ui_shell_start_activity(NULL, app_hfp_va_activity_proc, ACTIVITY_PRIORITY_HIGH, (void *)hfp_context, 0);
                    } else {
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
                    }
                }
            }
        }
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        else if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && action == APPS_EVENTS_INTERACTION_SYNC_AUTO_ACCEPT_STATUS) {
            uint8_t isAutoaccept = *(uint8_t *)p_extra_data;
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [AUTO ACCEPT] received auto accept config:isAutoaccept=%d", 1, isAutoaccept);
            app_hfp_set_auto_accept_incoming_call(isAutoaccept);

        }
#endif

#if defined(AIR_MULTI_POINT_ENABLE) && defined(MTK_AWS_MCE_ENABLE)
        else if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && action == APPS_EVENTS_INTERACTION_SYNC_HFP_STATE_TO_PARTNER) {
            if (BT_AWS_MCE_ROLE_PARTNER == bt_connection_manager_device_local_info_get_aws_role()) {
                hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
                bt_sink_srv_state_change_t *hfp_state_change = (bt_sink_srv_state_change_t *)p_extra_data;
                if (hfp_context == NULL) {
                    return ret;
                }
#if (defined MTK_PROMPT_SOUND_ENABLE)
                uint16_t curr_vp_index= app_voice_prompt_get_current_index();
                if (VP_INDEX_INCOMING_CALL == curr_vp_index) {
                    if (hfp_state_change->previous == BT_SINK_SRV_STATE_INCOMING
                            && hfp_state_change->current != BT_SINK_SRV_STATE_INCOMING) {

                        APPS_LOG_MSGID_I(APP_HFP_IDLE", partner state_change=%x, vp_index=%d: stop_vp:",
                                2, hfp_state_change->current, curr_vp_index);
                        apps_config_stop_voice(false, 200, true);
                        app_hfp_set_incoming_call_vp_flag(APP_HFP_INCOMING_CALL_VP_STOP);

                    }
                } else if (VP_INDEX_TWC_INCOMING_CALL == curr_vp_index) {
                    if (hfp_state_change->previous == BT_SINK_SRV_STATE_TWC_INCOMING
                            && hfp_state_change->current != BT_SINK_SRV_STATE_TWC_INCOMING) {
                        APPS_LOG_MSGID_I(APP_HFP_IDLE", partner state_change=%x, vp_index=%d: stop_vp:",
                                2, hfp_state_change->current, curr_vp_index);
                        apps_config_stop_voice(false, 200, true);
                        app_hfp_set_incoming_call_vp_flag(APP_HFP_INCOMING_CALL_VP_STOP);
                    }
                }
#endif

                /* Update hfp context.*/
                hfp_context->pre_state = hfp_state_change->previous;
                hfp_context->curr_state = hfp_state_change->current;

                apps_config_state_t state = app_get_config_status_by_state(hfp_state_change->current);
                if (state != APP_TOTAL_STATE_NO) {
                    apps_config_key_set_mmi_state(state);
                    APPS_LOG_MSGID_I(APP_HFP_IDLE", Receive agent send mmi state: %x", 1,state);
                }
                /* Update led BG pattern when sink state changed. */
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0, NULL, 0);
                /*apps_config_state_t app_state = *(apps_config_state_t *)p_extra_data;*/
                APPS_LOG_MSGID_I(APP_HFP_IDLE", receive agent hfp state_change:pre_state: %x,curr_state: %x",
                        2, hfp_state_change->previous,hfp_state_change->current);
                if (state != APP_TOTAL_STATE_NO) {
                    if (false == hfp_context->transient_actived) {
                        hfp_context->transient_actived = true;
                        ui_shell_start_activity(NULL, app_hfp_activity_proc, ACTIVITY_PRIORITY_HIGH, (void *)hfp_context, 0);
                    }
                }
            }
        }
#endif
    }

    return ret;
}
#endif

static bool app_hfp_idle_start_hfp_activity(
        ui_shell_activity_t *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;
    bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
    if (param == NULL) {
        return ret;
    }
    APPS_LOG_MSGID_I(APP_HFP_IDLE", sink_state_change:  param->pre = %x, param->now = %x",
            2, param->previous, param->current);

    hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
    if (hfp_context == NULL) {
        return ret;
    }

    /* Update context of hfp app, and start the app_hfp_acitvity. */
    hfp_context->pre_state = param->previous;
    hfp_context->curr_state = param->current;

#ifdef MTK_AWS_MCE_ENABLE
    if (param->current == BT_SINK_SRV_STATE_INCOMING || param->current == BT_SINK_SRV_STATE_TWC_INCOMING) {
        app_voice_prompt_set_voice_stopping(false);
    }
#endif

    /* Record aws link state for sync incoming call vp when incoming call. */
    if (param->previous != BT_SINK_SRV_STATE_INCOMING && param->current == BT_SINK_SRV_STATE_INCOMING) {
        if (true == app_home_screen_idle_activity_is_aws_connected()) {
            APPS_LOG_MSGID_I(APP_HFP_IDLE" sink_state_change set aws link state into true", 0);
            ((hfp_context_t *)(self->local_context))->aws_link_state = true;
        } else {
            APPS_LOG_MSGID_I(APP_HFP_IDLE" sink_state_change set aws link state into false", 0);
            ((hfp_context_t *)(self->local_context))->aws_link_state = false;
        }
    }
#ifdef MTK_AWS_MCE_ENABLE
    else if (param->previous == BT_SINK_SRV_STATE_INCOMING && param->current < BT_SINK_SRV_STATE_INCOMING) {
        app_voice_prompt_set_voice_stopping(true);
    }
#endif

    apps_config_state_t pre_state = app_get_config_status_by_state(param->previous);
    apps_config_state_t state = app_get_config_status_by_state(param->current);

    APPS_LOG_MSGID_I(APP_HFP_IDLE" state = %x", 1, state);
    if (APP_TOTAL_STATE_NO == pre_state && state != APP_TOTAL_STATE_NO) {
        APPS_LOG_MSGID_I(APP_HFP_IDLE" pre_state = %x, curr_state = %x", 2, hfp_context->pre_state, hfp_context->curr_state);
        if (false == hfp_context->transient_actived) {
            hfp_context->transient_actived = true;
            ui_shell_start_activity(NULL, app_hfp_activity_proc, ACTIVITY_PRIORITY_HIGH, (void *)hfp_context, 0);
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            /* For auto accept incoming call. */
            uint8_t isAutoaccept = app_hfp_is_auto_accept_incoming_call();
            app_in_ear_state_t curr_in_ear_state = app_in_ear_get_state();
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [AUTO ACCEPT]: isAutoaccept=%d curr_in_ear_state=%d",
                    2, isAutoaccept, curr_in_ear_state);
            if ((APP_HFP_AUTO_ACCEPT_ENABLE == isAutoaccept) && (APP_IN_EAR_STA_BOTH_IN == curr_in_ear_state)) {
                APPS_LOG_MSGID_I(APP_HFP_IDLE" [AUTO ACCEPT]: send auto accept incoming call timer.", 0);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_AUTO_ACCEPT_INCOMING_CALL, NULL, 0, NULL, HFP_AUTO_ACCEPT_TIMER);
            }
#endif
        }
    }

    return false;
}

#ifdef AIR_MULTI_POINT_ENABLE
bool app_hfp_emp_switch_allow_callback(bool need_enable, bt_bd_addr_t *keep_phone_addr)
{
    bool allow = TRUE;
    uint32_t connected_phone_num = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0);
    bt_sink_srv_state_t hfp_state = s_app_hfp_context.curr_state;
    if (!need_enable) {
        if (connected_phone_num >= 2 && hfp_state >= BT_SINK_SRV_STATE_INCOMING) {
            // uint8_t *need_keep_phone_addr = *keep_phone_addr;
            // uint8_t *cur_hfp_activate_addr = s_app_hfp_context.activate_con_addr;
            // && memcmp(need_keep_phone_addr, cur_hfp_activate_addr, 6) != 0
            allow = FALSE;
        }
    }
    APPS_LOG_MSGID_I(APP_HFP_IDLE" [EMP] emp_switch_allow, need_enable=%d hfp_state=%d allow=%d",
                     3, need_enable, hfp_state, allow);
    return allow;
}
#endif

static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_HFP_IDLE", create", 0);
            self->local_context = &s_app_hfp_context;
            memset(self->local_context, 0, sizeof(hfp_context_t));
            hfp_context_t *local_context = (hfp_context_t *)self->local_context;
            local_context->esco_connected = 0;
            local_context->transient_actived = 0;
            local_context->aws_link_state = FALSE;
            local_context->mute_mic = FALSE;
            memset(&local_context->activate_con_addr, 0, sizeof(bt_bd_addr_t));
            apps_config_audio_helper_sidetone_init();
#ifdef AIR_MULTI_POINT_ENABLE
            app_bt_emp_srv_user_register(APP_BT_EMP_SRV_USER_ID_HFP, app_hfp_emp_switch_allow_callback);
#endif
#ifdef AIR_APP_HFP_DEBUG_ENABLE
            app_hfp_atci_debug_init();
#endif
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_HFP_IDLE", destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(APP_HFP_IDLE", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(APP_HFP_IDLE", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(APP_HFP_IDLE", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(APP_HFP_IDLE", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

/**
* @brief      This function is used to handle the key action.
* @param[in]  state, the current sink_state.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_hfp_idle_proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    hfp_context_t *local_context = (hfp_context_t *)self->local_context;
    uint8_t key_id;
    airo_key_event_t key_event;

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
            apps_config_set_vp(VP_INDEX_PRESS, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            ret = true;
            break;
        case KEY_WAKE_UP_VOICE_ASSISTANT:
        case KEY_WAKE_UP_VOICE_ASSISTANT_CONFIRM:
        case KEY_INTERRUPT_VOICE_ASSISTANT:
            /* If user release the key after long press, trigger voice assistant on agent side. */
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY,
                        (action == KEY_INTERRUPT_VOICE_ASSISTANT) ? KEY_INTERRUPT_VOICE_ASSISTANT : KEY_WAKE_UP_VOICE_ASSISTANT)) {
                    APPS_LOG_MSGID_I(APP_HFP_IDLE", Partner send KEY_WAKE_UP_VOICE_ASSISTANT aws to agent failed", 0);
                    if (KEY_WAKE_UP_VOICE_ASSISTANT == action) {
                        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    }
                }
            } else
#endif
            {
                //bool active = !local_context->voice_assistant;
                bool active;
                if (action == KEY_INTERRUPT_VOICE_ASSISTANT) {
                    active = false;
                } else {
                    active = true;
                }
                APPS_LOG_MSGID_I(APP_HFP_IDLE", Agent set voice_recognition to %d", 1, active);
                if (BT_STATUS_SUCCESS == bt_sink_srv_send_action(BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE, &active)) {
                    local_context->voice_assistant = active;
                    if (active) {
                        ui_shell_start_activity(NULL, app_hfp_va_activity_proc, ACTIVITY_PRIORITY_HIGH, (void *)local_context, 0);
                    } else {
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
                    }
                } else {
                    APPS_LOG_MSGID_I(APP_HFP_IDLE", Agent send BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE fail", 0);
                }
            }
            ret = true;
            break;
        case KEY_REDIAL_LAST_CALL:
            APPS_LOG_MSGID_I(APP_HFP_IDLE", receive KEY_REDIAL_LAST_CALL", 0);
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
                APPS_LOG_MSGID_I(APP_HFP_IDLE"app_hfp_idle_proc_rotary_event_group, hpf volume : %d", 1, volume);
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CALL_SET_VOLUME, &volume);
                ret = true;
            }
            break;
        }
        case KEY_AUDIO_SIDE_TONE_VOLUME_UP:
        case KEY_AUDIO_SIDE_TONE_VOLUME_DOWN: {
            int32_t target_side_tone = 0;
            int32_t current_side_tone_volume = apps_config_audio_helper_get_sidetone_data()->value;
            if (KEY_AUDIO_SIDE_TONE_VOLUME_UP == key_action) {
                target_side_tone = current_side_tone_volume + rotary_data * APP_SIDE_TONE_CHANGE_LEVEL_PRE_STEP;
            } else {
                target_side_tone = current_side_tone_volume - rotary_data * APP_SIDE_TONE_CHANGE_LEVEL_PRE_STEP;
            }
            /* Check the range. */
            if (target_side_tone < APP_SIDE_TONE_VOLUME_MIN_LEVEL) {
                target_side_tone = APP_SIDE_TONE_VOLUME_MIN_LEVEL;
            } else if (target_side_tone > APP_SIDE_TONE_VOLUME_MAX_LEVEL) {
                target_side_tone = APP_SIDE_TONE_VOLUME_MAX_LEVEL;
            }
            if (target_side_tone == current_side_tone_volume) {
                apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            } else {
                bt_sink_srv_am_result_t am_result;
                am_result = apps_config_audio_helper_set_sidetone_value(target_side_tone);
                APPS_LOG_MSGID_I(APP_HFP_IDLE": set side tone level %d -> %d, status: 0x%x", 3,
                    current_side_tone_volume, target_side_tone, am_result);
                if (AUD_EXECUTION_SUCCESS == am_result) {
                    nvdm_write_data_item(NVDM_GROUP_APP, NVDM_APP_SIDETONE_VALUE, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&target_side_tone, sizeof(target_side_tone));
                }
            }
            ret = true;
            break;
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
                        && (remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP))) {
                    /* Report battery level to remote device when HFP connected. */
                    int32_t bat_val = 100;
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
                    bat_val = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
#endif
                    hfp_context->battery_level = bat_val;
                    hfp_context->hfp_connected = true;
                    APPS_LOG_MSGID_I(APP_HFP_IDLE", bat_val: %d", 1, bat_val);
                    app_hfp_report_battery_to_remote(hfp_context->battery_level, 0xFFFF);
                } else if ((remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP))
                           && !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP))) {
                    /* Update the state of voice assistant when HFP disconnected. */
                    hfp_context->voice_assistant = false;
                    hfp_context->hfp_connected = false;
                    APPS_LOG_MSGID_I(APP_HFP_IDLE", Set voice_recognition to false when HFP disconnected", 0);
                }
#ifdef MTK_AWS_MCE_ENABLE
                if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
                    if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                        && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                        /* Update the partner connection state when AWS is disconnected. */
                        hfp_context->aws_link_state = FALSE;
                        APPS_LOG_MSGID_I(APP_HFP_IDLE" app_hfp_idle_proc_bt_cm_event_group, Partner Detached.", 0);
                    } else if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                               && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                        /* Update the partner connection state when AWS is connected. */
                        hfp_context->aws_link_state = TRUE;
#ifdef MTK_IN_EAR_FEATURE_ENABLE
                        app_hfp_notify_state_to_peer();
#endif
                        APPS_LOG_MSGID_I(APP_HFP_IDLE" app_hfp_idle_proc_bt_cm_event_group, Partner Attached.", 0);
                        /* Stop sync twc or incoming call vp fail because of switch aws link.
                         * Retry to stop sync vp  when aws link connected again. */
#ifdef MTK_PROMPT_SOUND_ENABLE
                        uint8_t curr_vp_index = app_voice_prompt_get_current_index();
                        if (APP_HFP_INCOMING_CALL_VP_SYNC_STOP_FAIL == app_hfp_get_incoming_call_vp_sync_flag()) {
                            if (VP_INDEX_TWC_INCOMING_CALL == curr_vp_index
                                    || VP_INDEX_INCOMING_CALL == curr_vp_index) {
                                APPS_LOG_MSGID_I(APP_HFP_IDLE" [RING_IND][BT_CM]: sync stop twc or incoming call again.", 0);
                                app_voice_prompt_set_voice_stopping(true);
                                app_hfp_stop_vp();
                            }
                        }
#endif
                    }
                } else if (BT_AWS_MCE_ROLE_PARTNER == role || BT_AWS_MCE_ROLE_CLINET == role) {
                    if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                        && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                        /* Update the connection state when AWS is connected. */
                        hfp_context->aws_link_state = TRUE;
                        APPS_LOG_MSGID_I(APP_HFP_IDLE" app_hfp_idle_proc_bt_cm_event_group Partner connected.", 0);
                    }
                    if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                        && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                        /* Update the connection state when AWS is disconnected. */
                        hfp_context->aws_link_state = FALSE;
                        hfp_context->voice_assistant = FALSE;
                        APPS_LOG_MSGID_I(APP_HFP_IDLE" app_hfp_idle_proc_bt_cm_event_group Partner disconnected.", 0);
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

static bool app_hfp_idle_proc_bt_sink_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;

    switch (event_id) {
        case BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED: {
            bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)extra_data;
            hfp_context->voice_assistant = event->voice_recognition.enable;
            APPS_LOG_MSGID_I(APP_HFP_IDLE", voice_recognition changed : %d",
                    1, hfp_context->voice_assistant);
            if (event->voice_recognition.enable) {
                ui_shell_start_activity(NULL, app_hfp_va_activity_proc, ACTIVITY_PRIORITY_HIGH, (void *)hfp_context, 0);
            } else {
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
            }
            break;
        }
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
#if defined(AIR_MULTI_POINT_ENABLE) && defined(AIR_EMP_AUDIO_INTER_STYLE_ENABLE)
        app_hfp_emp_music_process(self, event_id, extra_data, data_len);
#endif
        ret = app_hfp_idle_start_hfp_activity(self, event_id, extra_data, data_len);
            break;
        }
        case BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE: {
            bt_sink_srv_sco_state_update_t *sco_state = (bt_sink_srv_sco_state_update_t *)extra_data;
            if (sco_state != NULL) {
                /* Record the first address of esco. */
                if (hfp_context->esco_connected == 0) {
                    memcpy(hfp_context->activate_con_addr, sco_state->address, sizeof(bt_bd_addr_t));
                    APPS_LOG_MSGID_I(APP_HFP_IDLE" SCO_STATE address: %02X:%02X:%02X:%02X:%02X:%02X", 6, hfp_context->activate_con_addr[5],
                            hfp_context->activate_con_addr[4], hfp_context->activate_con_addr[3], hfp_context->activate_con_addr[2],
                            hfp_context->activate_con_addr[1], hfp_context->activate_con_addr[0]);
                }
                /* Update esco connection state. */
                hfp_context->esco_connected += (sco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED) ? 1 : -1;
                hfp_context->esco_connected = (hfp_context->esco_connected > 0) ? hfp_context->esco_connected : 0;
                /* Resume mic mute status when esco disconnected.*/
                if (hfp_context->esco_connected == 0) {
                    hfp_context->mute_mic = FALSE;
                }
                APPS_LOG_MSGID_I(APP_HFP_IDLE", SCO_STATE: %d--%d", 2, sco_state->state, hfp_context->esco_connected);
            }
            break;
        }

#if defined(AIR_LE_AUDIO_ENABLE) && defined(AIR_LE_AUDIO_CIS_ENABLE)
        case LE_SINK_SRV_EVENT_REMOTE_INFO_UPDATE: {
            bt_le_sink_srv_event_remote_info_update_t *update_ind = (bt_le_sink_srv_event_remote_info_update_t *)extra_data;
            if (update_ind == NULL || hfp_context == NULL) {
                break;
            }
            hfp_context->le_audio_srv = update_ind->connected_service;
            break;
        }
#endif
        default: {
            break;
        }
    }

    return ret;
}

static bool app_hfp_idle_proc_battery_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
    if (hfp_context == NULL) {
        return ret;
    }

    switch (event_id) {
        case APPS_EVENTS_BATTERY_PERCENT_CHANGE: {
            /* Report it to remote device when battery percent changed. */
            int32_t pre_battery = hfp_context->battery_level;
            int32_t curr_battery = (int32_t)extra_data;
#ifdef MTK_AWS_MCE_ENABLE
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            APPS_LOG_MSGID_I(APP_HFP_IDLE", current_role=%d, pre_bat: %d, curr_bat = %d, cur_state = %x",
                    4, role, pre_battery, curr_battery, hfp_context->curr_state);
#else
            APPS_LOG_MSGID_I(APP_HFP_IDLE", pre_bat: %d, curr_bat = %d, cur_state = %x",
                    3, pre_battery, curr_battery, hfp_context->curr_state);
#endif
            if (hfp_context->hfp_connected && curr_battery != pre_battery) {
#ifdef MTK_AWS_MCE_ENABLE
                if (BT_AWS_MCE_ROLE_AGENT == role)
#endif
                {
                    app_hfp_report_battery_to_remote(curr_battery, pre_battery);
                    hfp_context->battery_level = curr_battery;
                }
            }
        break;
        }
        default : {
            break;
        }
    }

    return ret;
}

/**
* @brief      This function is used to handle the app internal events.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_hfp_idle_proc_app_internal_events(
   struct _ui_shell_activity *self,
   uint32_t event_id,
   void *extra_data,
   size_t data_len)
{
   bool ret = false;

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
   switch (event_id) {
       /* The old Agent will switch to new Partner if RHO successfully. */
       case APPS_EVENTS_INTERACTION_RHO_END: {
           app_rho_result_t rho_ret = (app_rho_result_t)extra_data;
           APPS_LOG_MSGID_I(APP_HFP_IDLE", proc_internal_events old agent switch to new partner: rho_ret=%d", 1, rho_ret);
           if (APP_RHO_RESULT_SUCCESS == rho_ret) {
           }
           break;
       }
       /* The old Partner will switch to new Agent if RHO successfully. */
       case APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT: {
           app_rho_result_t rho_ret = (app_rho_result_t)extra_data;
           APPS_LOG_MSGID_I(APP_HFP_IDLE", proc_internal_events old partner switch to new agent: rho_ret=%d", 1, rho_ret);
           if (APP_RHO_RESULT_SUCCESS == rho_ret) {
               hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
               /* Create hfp activity after partner switch to agent during the call actived state. */
               apps_config_state_t pre_state = app_get_config_status_by_state(hfp_context->pre_state);
               apps_config_state_t state = app_get_config_status_by_state(hfp_context->curr_state);
               if (APP_TOTAL_STATE_NO == pre_state && state != APP_TOTAL_STATE_NO) {
                   APPS_LOG_MSGID_I(APP_HFP_IDLE", proc_internal_events start hfp_activity: pre_state = %x, curr_state = %x",
                           2, hfp_context->pre_state, hfp_context->curr_state);
                   if (false == hfp_context->transient_actived) {
                       hfp_context->transient_actived = true;
                       ui_shell_start_activity(NULL, app_hfp_activity_proc, ACTIVITY_PRIORITY_HIGH, (void *)hfp_context, 0);
                   }
               }
               /* Report battery level to remote device after RHO completed for new agent. */
               int32_t bat_val = 100;
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
               bat_val = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
#endif
               hfp_context->battery_level = bat_val;
               app_hfp_report_battery_to_remote(hfp_context->battery_level, 0xFFFF);
           }
           break;
       }
   }
#endif
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
            ret = app_hfp_idle_proc_key_event_group(self, event_id, extra_data, data_len);
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
            /* Bt sink srv event. */
            ret  = app_hfp_idle_proc_bt_sink_event_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            /* Event come from bt connection manager, indicates the connection state of HFP or AWS. */
            ret = app_hfp_idle_proc_bt_cm_event_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BATTERY: {
            /* Battery event proc.*/
            ret  = app_hfp_idle_proc_battery_event_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            ret = app_hfp_idle_proc_app_internal_events(self, event_id, extra_data, data_len);
            break;
        }
#if defined(MTK_AWS_MCE_ENABLE)
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            /* Handle the key event sync from the partner side. */
            ret = app_hfp_idle_proc_aws_data(self, event_id, extra_data, data_len);
            break;
        }
#endif
        default:
            break;
    }

    return ret;
}

/***********************************************************************************************************
 * ************************************APP HFP ATCI DEBUG*********************************************
 * *********************************************************************************************************/
#ifdef AIR_APP_HFP_DEBUG_ENABLE
#ifdef MTK_IN_EAR_FEATURE_ENABLE
static atci_status_t _hfp_atci_update_auto_accept_cfg(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *param = NULL;

    APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] _atci_update_auto_accept_cfg mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_EXECUTION:
        param = parse_cmd->string_ptr + parse_cmd->name_len + 1;
        param = strtok(param, ",\n\r");
        if(0 == memcmp("enable",param,6))
        {
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] enable auto accept incoming call.", 0);
            app_hfp_set_auto_accept_incoming_call(APP_HFP_AUTO_ACCEPT_ENABLE);
        }
        if(0 == memcmp("disable",param,7))
        {
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] disable auto accept incoming call.", 0);
            app_hfp_set_auto_accept_incoming_call(APP_HFP_AUTO_ACCEPT_DISABLE);
        }

        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        break;
    default:
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}
#endif

static atci_status_t _hfp_atci_mute_mic(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *param = NULL;

    APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] _mute_mic mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_EXECUTION:
        param = parse_cmd->string_ptr + parse_cmd->name_len + 1;
        param = strtok(param, ",\n\r");
        if(0 == memcmp("mute",param,4))
        {
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] mute mic.", 0);
            app_hfp_mute_mic(TRUE);
        }
        if(0 == memcmp("unmute",param,6))
        {
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] unmute mic.", 0);
            app_hfp_mute_mic(FALSE);
        }
        if(0 == memcmp("mutes",param,5))
        {
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] mute speaker.", 0);
            bt_sink_srv_set_mute(BT_SINK_SRV_MUTE_SPEAKER, TRUE);
        }
        if(0 == memcmp("unmutes",param,7))
        {
            APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] unmute speaker.", 0);
            bt_sink_srv_set_mute(BT_SINK_SRV_MUTE_SPEAKER, FALSE);
        }

        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        break;
    default:
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_cmd_hdlr_item_t app_hfp_atci_cmd_debug[] = {
#ifdef MTK_IN_EAR_FEATURE_ENABLE
    {
        .command_head = "AT+AUTOACCEPT",    /**< HFP auto accept call debug. */
        .command_hdlr = _hfp_atci_update_auto_accept_cfg,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#endif
    {
        .command_head = "AT+MICMUTE",       /**< HFP mute mic debug. */
        .command_hdlr = _hfp_atci_mute_mic,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

void app_hfp_atci_debug_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(app_hfp_atci_cmd_debug, sizeof(app_hfp_atci_cmd_debug) / sizeof(atci_cmd_hdlr_item_t));
    APPS_LOG_MSGID_I(APP_HFP_IDLE" [HFP ATCI] atci_register_handler register ret = %d", 1, ret);
}
#endif  /* AIR_APP_HFP_DEBUG_ENABLE */
