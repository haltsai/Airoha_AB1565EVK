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
 * This file is the activity to handle the key action or sink service event when call is active.
 * When the call is active or there is a call coming in, the app_hfp_idle_activity start this
 * activity to handle the call state change events and UI events of HFP.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "app_hfp_activity.h"
#include "apps_events_event_group.h"
#include "apps_events_key_event.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_key_remapper.h"
#include "bt_aws_mce_srv.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_ami.h"

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#include "bt_device_manager.h"

#define TWC_INCOMING_CALL_VP_TIMER_INTERVAL   (4 * 1000)

static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI", create : %x", 1, extra_data);
            self->local_context = extra_data;
            /* LED will display when call incoming or call is active, so trigger MMI updating. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI", destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {

            APPS_LOG_MSGID_I(APP_HFP_ACTI", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {

            APPS_LOG_MSGID_I(APP_HFP_ACTI", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {

            APPS_LOG_MSGID_I(APP_HFP_ACTI", result", 0);
            break;
        }
        default:
            ret = false;
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
    hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
    if (hfp_context == NULL) {
        APPS_LOG_MSGID_I(APP_HFP_ACTI", key_event_group: hfp_context is null", 0);
        return ret;
    }
    bt_sink_srv_state_t pre = hfp_context->pre_state;
    bt_sink_srv_state_t cur = hfp_context->curr_state;

    APPS_LOG_MSGID_I(APP_HFP_ACTI", key_event_group: pre = %x, curr = %x ", 2, pre, cur);

    ret = app_hfp_proc_key_event(cur, event_id, extra_data, data_len);

    return ret;
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
static bool app_hfp_proc_rotary_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    bsp_rotary_encoder_port_t port;
    bsp_rotary_encoder_event_t event;
    uint32_t rotary_data;
    if (!extra_data) {
        return ret;
    }
    apps_config_key_action_t key_action = *(uint16_t *)extra_data;
    app_event_rotary_event_decode(&port, &event, &rotary_data, event_id);
    switch (key_action) {
        case KEY_VOICE_UP:
        case KEY_VOICE_DN: {
            uint32_t volume = bt_sink_srv_get_volume(NULL, BT_SINK_SRV_VOLUME_HFP);
            if (0xFFFFFFFF == volume) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI"app_hfp_proc_rotary_event_group, get hfp volume : %x", 1, volume);
            } else {
                if (KEY_VOICE_UP == key_action) {
                    if (volume + rotary_data <= AUD_VOL_OUT_MAX) {
                        volume += rotary_data;
                    } else {
                        volume = AUD_VOL_OUT_MAX - 1;
                    }
                } else {
                    if (volume > rotary_data) {
                        volume -= rotary_data;
                    } else {
                        volume = 0;
                    }
                }
                APPS_LOG_MSGID_I(APP_HFP_ACTI"app_hfp_proc_rotary_event_group, hfp target volume : %d", 1, volume);
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CALL_SET_VOLUME, &volume);
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

bool app_hfp_proc_bt_hfp_event(ui_shell_activity_t *self,
                                uint32_t event_group,
                                uint32_t event_id,
                                void *extra_data,
                                size_t data_len)
{
    bool ret = false;
    APPS_LOG_MSGID_I(APP_HFP_ACTI", event_id : %x", 1, event_id);

    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
            if (param == NULL) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI", state_change :param is null ", 0);
                return ret;
            }
            APPS_LOG_MSGID_I(APP_HFP_ACTI", state_change: param_previous: %x, param_now: %x", 2, param->previous, param->current);
            /* Stop ringtone of incoming call and clear vp state when call up or hung up. */
            if (param->previous == BT_SINK_SRV_STATE_INCOMING && param->current != BT_SINK_SRV_STATE_INCOMING) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI", state_change: stop_vp:", 0);
                apps_config_stop_voice(true, 200, true);
                app_hfp_set_incoming_call_vp_flag(0);
            }

#ifdef AIR_MULTI_POINT_ENABLE
            if (param->previous == BT_SINK_SRV_STATE_TWC_INCOMING && param->current != BT_SINK_SRV_STATE_TWC_INCOMING) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI", state_change: stop_vp:", 0);
                apps_config_stop_voice(true, 200, true);
                /* remove event of notifying next round twc incoming call vp loop. */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP);
                app_hfp_set_incoming_call_vp_flag(0);
            }
#endif

            /* Check the call status, if there are no HFP related states, finish current activity. */
            if (param->current <= BT_SINK_SRV_STATE_STREAMING) {
                if (param->previous == BT_SINK_SRV_STATE_ACTIVE || param->previous == BT_SINK_SRV_STATE_HELD_REMAINING
                    || param->previous == BT_SINK_SRV_STATE_HELD_ACTIVE || param->previous == BT_SINK_SRV_STATE_MULTIPARTY) {
                    /* Call is end */
#ifdef MTK_AWS_MCE_ENABLE
                    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
                    if (BT_AWS_MCE_ROLE_PARTNER == role || BT_AWS_MCE_ROLE_CLINET == role) {
                        if (bt_aws_mce_srv_get_link_type() == BT_AWS_MCE_SRV_LINK_NORMAL) {
                        }
                    } else
#endif
                    {
                        if (0 != bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP), NULL, 0)) {
                            ;
                        }
                    }
                }
                ui_shell_finish_activity(self, self);

            } else {
                apps_config_state_t state = app_get_config_status_by_state(param->current);
                APPS_LOG_MSGID_I(APP_HFP_ACTI", state_change:is_hfp: %x", 1, state);
                if (state != APP_TOTAL_STATE_NO) {
                    APPS_LOG_MSGID_I(APP_HFP_ACTI", enter self", 0);
                    ((hfp_context_t *)(self->local_context))->pre_state = param->previous;
                    ((hfp_context_t *)(self->local_context))->curr_state = param->current;
                }
            }

            /* Udpate LED display and MMI state when sink service state changed. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
            break;
        }
        case BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE: {
            break;
        }
        case BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION: {
#ifdef AIR_MULTI_POINT_ENABLE
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            bt_sink_srv_caller_information_t *call_info = (bt_sink_srv_caller_information_t *)extra_data;
            APPS_LOG_I("Caller info: number:%s, name:%s", call_info->number, call_info->name);
            APPS_LOG_MSGID_I("Caller info incoming address: %02X:%02X:%02X:%02X:%02X:%02X", 6, call_info->address[5],
                    call_info->address[4], call_info->address[3], call_info->address[2], call_info->address[1], call_info->address[0]);
            APPS_LOG_MSGID_I("Caller info context address: %02X:%02X:%02X:%02X:%02X:%02X", 6, hfp_context->activate_con_addr[5],
                                hfp_context->activate_con_addr[4], hfp_context->activate_con_addr[3], hfp_context->activate_con_addr[2],
                                hfp_context->activate_con_addr[1], hfp_context->activate_con_addr[0]);
            if (0 != memcmp(call_info->address, hfp_context->activate_con_addr, sizeof(bt_bd_addr_t))) {
                APPS_LOG_MSGID_I("Remote address don't common", 0);
                if (hfp_context->curr_state == BT_SINK_SRV_STATE_TWC_INCOMING
                        && (hfp_context->pre_state != BT_SINK_SRV_STATE_TWC_INCOMING)) {
                    APPS_LOG_MSGID_I("TWC incoming call VP", 0);
                    app_hfp_set_incoming_call_vp_flag(1);
#ifdef MTK_AWS_MCE_ENABLE
                    apps_config_set_voice(VP_INDEX_TWC_INCOMING_CALL, true, 6000, VOICE_PROMPT_PRIO_ULTRA, true, true, NULL);
#else
                    apps_config_set_vp(VP_INDEX_TWC_INCOMING_CALL, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    /* send a delay event as a timer to loop twc incoming call playback. */
                    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP, NULL, 0,
                            NULL, TWC_INCOMING_CALL_VP_TIMER_INTERVAL);
#endif
                }
            }
#endif
            break;
        }
        case BT_SINK_SRV_EVENT_HF_MISSED_CALL: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI", event_id : %d", 1, event_id);
            bt_sink_srv_caller_information_t *call_info = (bt_sink_srv_caller_information_t *)extra_data;
            APPS_LOG_I("Caller info: number:%s, name:%s", call_info->number, call_info->name);
            break;
        }
        case BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED: {
            bt_sink_srv_voice_recognition_status_t *voice_re = (bt_sink_srv_voice_recognition_status_t *)extra_data;
            APPS_LOG_MSGID_I("Voice_recogn: enable:%d", 1, voice_re->enable);
            break;
        }
        case BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION: {
            break;
        }
        case BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE: {
            /* Update esco connection state. */
            APPS_LOG_MSGID_I(APP_HFP_ACTI" SCO_STATE", 0);
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            bt_sink_srv_sco_state_update_t *sco_state = (bt_sink_srv_sco_state_update_t *)extra_data;
            /*Store the first connected esco addr for playing twc incoming call vp*/
            if (!hfp_context->esco_connected) {
                memcpy(hfp_context->activate_con_addr, sco_state->address, sizeof(bt_bd_addr_t));
                APPS_LOG_MSGID_I("Caller SCO address: %02X:%02X:%02X:%02X:%02X:%02X", 6, hfp_context->activate_con_addr[5],
                        hfp_context->activate_con_addr[4], hfp_context->activate_con_addr[3], hfp_context->activate_con_addr[2],
                        hfp_context->activate_con_addr[1], hfp_context->activate_con_addr[0]);
            }
            if (sco_state != NULL) {
                hfp_context->esco_connected = (sco_state->state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED) ? true : false;
                APPS_LOG_MSGID_I(APP_HFP_ACTI", SCO_STATE: %d", 1, sco_state->state);
            }

            break;
        }
        case BT_SINK_SRV_EVENT_HF_RING_IND: {
            /* Play the ringtone of incoming call when the smartphone request to play ringtone. */
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            APPS_LOG_MSGID_I(APP_HFP_ACTI" [RING IND] pre_state: %x, cur_state : %x", 2, hfp_context->pre_state, hfp_context->curr_state);
            if (hfp_context->curr_state == BT_SINK_SRV_STATE_INCOMING && hfp_context->pre_state != BT_SINK_SRV_STATE_TWC_INCOMING) {
                app_hfp_set_incoming_call_vp_flag(1);
                apps_config_set_voice(VP_INDEX_INCOMING_CALL, true, 600, VOICE_PROMPT_PRIO_ULTRA, true, true, NULL);
            }
            break;
        }
        default: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI", hfp_event_proc:Don't care", 0);
            ret = false;
            break;
        }
    }
    APPS_LOG_MSGID_I(APP_HFP_ACTI", hfp_event_proc:ret = %x", 1, ret);
    return ret;
}

static bool app_hfp_proc_bt_cm_event(ui_shell_activity_t *self,
                                     uint32_t event_id,
                                     void *extra_data,
                                     size_t data_len)
{
    bool ret = false;
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
#ifdef AIR_MULTI_POINT_ENABLE
            APPS_LOG_MSGID_I(APP_HFP_ACTI", hfp_event_proc:Don't care when open emp feature", 0);
#else
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            /* Check the HFP connection state, if the HFP is no longer connected, finish current activity. */
            hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
            if (remote_update
                && remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                && !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP))) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI", hfp_event_proc:Don't care", 0);
                apps_config_stop_voice(true, 200, true);
                app_hfp_set_incoming_call_vp_flag(0);
                hfp_context->hfp_connected = false;
            }
#endif
        }
        break;
        default:
            break;

    }
    return ret;
}


bool app_hfp_activity_proc(
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
            /* Key event. */
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef AIR_ROTARY_ENCODER_ENABLE
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER: {
            /* Rotary encoder events. */
            ret = app_hfp_proc_rotary_event_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* BT_SINK events, indicates the state of HFP. */
            ret = app_hfp_proc_bt_hfp_event(self, event_group, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            /* Event come from bt connection manager, indicates the connection state of HFP. */
            ret = app_hfp_proc_bt_cm_event(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            apps_config_state_t app_state;
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);

            APPS_LOG_MSGID_I(APP_HFP_ACTI",APP_INTERACTION: event_id : %d", 1, event_id);
            if (hfp_context) {
                app_state = app_get_config_status_by_state(hfp_context->curr_state);
                if (event_id == APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN) {
                    /* Update LED display when the state of HFP updated. */
                    APPS_LOG_MSGID_I(APP_HFP_ACTI",APP_INTERACTION, state : %d, app_state = %d", 2, hfp_context->curr_state, app_state);
                    ret = app_hfp_update_led_bg_pattern(self, hfp_context->curr_state,hfp_context->pre_state);
                } else if (APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE == event_id) {
                    if (APP_TOTAL_STATE_NO != app_state) {
                        apps_config_key_set_mmi_state(app_state);
                        ret = true;
                    } else {
                        ret = false;
                    }
                }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
                else if (APPS_EVENTS_INTERACTION_RHO_END == event_id || APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT == event_id) {
                    app_rho_result_t rho_ret = (app_rho_result_t)extra_data;

                    APPS_LOG_MSGID_I(APP_HFP_ACTI", RHO result : 0x%x, cur_state : 0x%x, pre_state : 0x%x", 3, rho_ret, hfp_context->curr_state, hfp_context->pre_state);
                    uint8_t flag = app_hfp_get_incoming_call_vp_flag();
                    if (!flag) {
                        break;
                    }
                    if (APP_RHO_RESULT_SUCCESS == rho_ret) {
                        /* Play ringtone after RHO. */
                        if (BT_SINK_SRV_STATE_INCOMING == hfp_context->curr_state && hfp_context->pre_state != BT_SINK_SRV_STATE_TWC_INCOMING) {
                            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
                            APPS_LOG_MSGID_I(APP_HFP_ACTI", curr RHO: 0x%x", 1, role);
                            if (BT_AWS_MCE_ROLE_AGENT == role) {
                                apps_config_set_voice(VP_INDEX_INCOMING_CALL, true, 600, VOICE_PROMPT_PRIO_ULTRA, true, true, NULL);
                            } else if (BT_AWS_MCE_ROLE_PARTNER == role) {
                                apps_config_set_voice(VP_INDEX_INCOMING_CALL, false, 600, VOICE_PROMPT_PRIO_ULTRA, true, false, NULL);
                            }
                        }
                    }
                }
#endif
#ifdef AIR_MULTI_POINT_ENABLE
                else if (TWC_INCOMING_CALL_VP_TIMER_INTERVAL == event_id) {
                    apps_config_set_vp(VP_INDEX_TWC_INCOMING_CALL, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    /* send a delay event as a timer to loop twc incoming call vp playback. */
                    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP, NULL, 0,
                                NULL, TWC_INCOMING_CALL_VP_TIMER_INTERVAL);
                }
#endif
            }
            break;
        }
        default:
            break;
    }
    APPS_LOG_MSGID_I(APP_HFP_ACTI", connected_activity_proc: ret = %d", 1, ret);
    return ret;
}

