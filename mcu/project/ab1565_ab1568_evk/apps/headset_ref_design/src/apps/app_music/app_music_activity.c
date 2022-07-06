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
 * File: app_music_activity.c
 *
 * Description:
 * This file is the activity to handle the key action or sink service event when music is playing.
 * When the music starts playing, the app_music_idle_activity start this activity to handle the a2dp
 * state changed events and UI events of music.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "app_music_activity.h"
#include "app_music_utils.h"
#include "apps_debug.h"
#include "bt_sink_srv.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"

#include "apps_config_key_remapper.h"
#include "apps_events_key_event.h"
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
#include "bt_ull_service.h"
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_idle_activity.h"
#endif
#ifdef AIR_ROTARY_ENCODER_ENABLE
#include "bt_sink_srv_a2dp.h"
#endif

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif

#ifdef AIR_LE_AUDIO_ENABLE
#include "app_le_audio_aird_client.h"
#include "app_le_audio_bis.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_le.h"
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le_cap_stream.h"
#endif

#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
#include "app_dongle_service.h"
#endif
#include "apps_dongle_sync_event.h"

static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" create  current activity : %x", 1, (uint32_t)self);
            if (extra_data) {
                self->local_context = extra_data;
                apps_music_local_context_t *local_context = (apps_music_local_context_t *)self->local_context;
                local_context->avrcp_op_sta = AVRCP_OPERATION_STA_IDLE;
            }
            /* Trigger MMI updating. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                NULL, 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" destroy", 0);
            /* This activity would be finished by self when the music stop playing, so trigger MMI updating. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                NULL, 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool app_music_proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    apps_config_key_action_t action = KEY_ACTION_INVALID;
    apps_music_local_context_t *local_ctx = (apps_music_local_context_t *)self->local_context;

    if (local_ctx) {
        action = app_music_utils_proc_key_events(self, event_id, extra_data, data_len);
    }

    if (action != KEY_ACTION_INVALID) {
        if (action == KEY_AVRCP_PLAY) {
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI "music_playing=0x%x", 1, local_ctx->music_playing);
        }
        ret = true;
    }

    return ret;
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
#ifdef AIR_LE_AUDIO_ENABLE
static bool app_music_rotary_set_le_audio_volume(apps_config_key_action_t key_action, uint32_t rotary_data)
{
    app_le_audio_aird_client_info_t *p_info;
    bt_handle_t handle;
    bool ret = false;

    if ((BT_HANDLE_INVALID != (handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established()))
        && (NULL != (p_info = app_le_audio_aird_client_get_info(handle)))
        && (APP_LE_AUDIO_AIRD_MODE_SUPPORT_HID_CALL == p_info->mode)) {

        app_le_audio_aird_action_set_streaming_volume_t param;
        bt_status_t status;

        APPS_LOG_MSGID_I(APP_MUSIC_ACTI" [LEA][CIS] app_music_rotary_set_le_audio_volume, action:0x%04X handle:%x",
                2, key_action, handle);

        param.streaming_interface = APP_LE_AUDIO_AIRD_STREAMING_INTERFACE_SPEAKER;
        param.streaming_port      = APP_LE_AUDIO_AIRD_STREAMING_PORT_0;
        param.channel             = APP_LE_AUDIO_AIRD_CHANNEL_DUAL;
        param.volume              = rotary_data;   /* The rotary value. */

        if (KEY_VOICE_UP == key_action) {
            status = app_le_audio_aird_client_send_action(handle,
                                                        APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_UP,
                                                        &param,
                                                        sizeof(app_le_audio_aird_action_set_streaming_volume_t));
        } else if (KEY_VOICE_DN == key_action) {
            status = app_le_audio_aird_client_send_action(handle,
                                                        APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_DOWN,
                                                        &param,
                                                        sizeof(app_le_audio_aird_action_set_streaming_volume_t));
        }
        ret = true;
    }
    return ret;
}
#endif

static bool app_music_proc_rotary_event_group(
    struct _ui_shell_activity *self,
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
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            bt_ull_streaming_info_t info = {0};
            bt_ull_streaming_t streaming = {
                .streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER,
                .port = 0,
            };

            if ((BT_STATUS_SUCCESS == bt_ull_get_streaming_info(streaming,&info)) && info.is_playing) {
#ifdef AIR_MS_GIP_ENABLE
                if (app_dongle_service_get_dongle_mode() == APP_DONGLE_SERVICE_DONGLE_MODE_XBOX) {
                    APPS_LOG_MSGID_I("app_music_proc_rotary_event_group route to dongle", 0);
                    apps_dongle_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, key_action);
                    break;
                }
#endif
                bt_status_t bt_status;
                bt_ull_volume_t volume_param;
                volume_param.streaming.streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER;
                volume_param.streaming.port = 0;
                volume_param.channel = BT_ULL_VOLUME_CHANNEL_DUEL;
                volume_param.volume = rotary_data; /* The delta value. */
                if (KEY_VOICE_UP == key_action) {
                    volume_param.action = BT_ULL_VOLUME_ACTION_SET_UP;
                } else {
                    volume_param.action = BT_ULL_VOLUME_ACTION_SET_DOWN;
                }
                bt_status = bt_ull_action(BT_ULL_ACTION_SET_STREAMING_VOLUME, &volume_param, sizeof(volume_param));
                APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_rotary_event_group, ull delat volume : %d, result: %d",
                        2, rotary_data, bt_status);
            }
            else
#endif
            {
#ifdef AIR_LE_AUDIO_ENABLE
                if (app_music_rotary_set_le_audio_volume(key_action, rotary_data)) {
                    APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_rotary_event_group, set le audio volume success!", 0);
                } else
#endif
                {
                    uint8_t volume = bt_sink_srv_get_volume(NULL, BT_SINK_SRV_VOLUME_A2DP);
                    if (KEY_VOICE_UP == key_action) {
                        if (volume + rotary_data < bt_sink_srv_ami_get_a2dp_max_volume_level()) {
                            volume += rotary_data;
                        } else {
                            volume = bt_sink_srv_ami_get_a2dp_max_volume_level();
                        }
                    } else {
                        if (volume > rotary_data) {
                            volume -= rotary_data;
                        } else {
                            volume = 0;
                        }
                    }
                    APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_rotary_event_group, music volume : %d", 1, volume);
                    bt_sink_srv_send_action(BT_SINK_SRV_ACTION_SET_VOLUME, &volume);
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

static bool app_music_proc_bt_event_when_playing(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    apps_music_local_context_t *local_context = (apps_music_local_context_t *)self->local_context;
    if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
        bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;

        APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_bt_event:param->pre=0x%x, param->now=0x%x, is playing=0x%x",
                3, param->previous, param->current, local_context->music_playing);

        /* Finish current activity when the a2dp streaming stops. */
        if ((param->previous == BT_SINK_SRV_STATE_STREAMING) && (param->current != BT_SINK_SRV_STATE_STREAMING)) {
            local_context->music_playing = false;
            local_context->isAutoPaused = false;
            ui_shell_finish_activity(self, self);
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_bt_event:ui_shell_finish_activity, current activity:0x%x, isAutoPaused=%d",
                    2, (uint32_t)self, local_context->isAutoPaused);
        }
    }
#if defined(MTK_IN_EAR_FEATURE_ENABLE)
    else if (event_id == BT_SINK_SRV_EVENT_AVRCP_STATUS_CHANGE) {
        bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)extra_data;
        bt_avrcp_status_t avrcp_status = event->avrcp_status_change.avrcp_status;
        APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_bt_event: avrcp_status=%d", 1, avrcp_status);
        /* Finish current activity when the avrcp stopped or paused. */
        if (BT_AVRCP_STATUS_PLAY_PAUSED == avrcp_status || BT_AVRCP_STATUS_PLAY_STOPPED == avrcp_status) {
            if (0 == memcmp(local_context->activate_con_addr, event->avrcp_status_change.address, sizeof(bt_bd_addr_t))){
                APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_bt_event: avrcp_addr-%02x:%02x:%02x:%02x:%02x:%02x",
                        6, event->avrcp_status_change.address[5], event->avrcp_status_change.address[4], event->avrcp_status_change.address[3],
                        event->avrcp_status_change.address[2], event->avrcp_status_change.address[1], event->avrcp_status_change.address[0]);
                local_context->music_playing = false;
                local_context->isAutoPaused = false;
                ui_shell_finish_activity(self, self);
                APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_bt_event:ui_shell_finish_activity, current activity:0x%x, avrcp_status=%d",
                        2, (uint32_t)self, avrcp_status);
            }
        }
    }
#endif
    return false;
}

#ifdef MTK_IN_EAR_FEATURE_ENABLE
/**
* @brief      This function is used to stop music when the earbud was taken out of the ear.
* @param[in]  self, the context pointer of the activity.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_music_check_and_end_music(struct _ui_shell_activity *self, void *extra_data)
{
    apps_music_local_context_t *ctx = (apps_music_local_context_t *)self->local_context;
    app_in_ear_sta_info_t *sta_info = (app_in_ear_sta_info_t *)extra_data;
    uint8_t temp_music_in_ear_config = app_music_get_in_ear_control_state();

#ifdef MTK_AWS_MCE_ENABLE
    APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_check_and_end_music, cur=%d, pre=%d, isAutoPaused=%d, music_playing=%d isPartnerCharging=%d, isPartnerConnected=%d, in_ear_config=%d",
            7, sta_info->current, sta_info->previous, ctx->isAutoPaused, ctx->music_playing, ctx->isPartnerCharging, ctx->isPartnerConnected, temp_music_in_ear_config);
#else
    APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_check_and_end_music, cur=%d, pre=%d, isAutoPaused=%d. in_ear_config=%d",
            4, sta_info->current, sta_info->previous, ctx->isAutoPaused, temp_music_in_ear_config);
#endif

#ifdef IN_EAR_FEATURE_AIROHA
    /* Stop the music when left and right earbuds are taken out of the ear. */
    if (sta_info->previous != APP_IN_EAR_STA_BOTH_OUT
        && sta_info->current == APP_IN_EAR_STA_BOTH_OUT
        && !ctx->isAutoPaused
        && ctx->music_playing)
#else
    /* Stop the music when at least one earbud is taken out of the ear. */
    if (sta_info->current != APP_IN_EAR_STA_BOTH_IN
        && sta_info->previous != APP_IN_EAR_STA_BOTH_OUT
        && !ctx->isAutoPaused
        && ctx->music_playing)
#endif
    {
        if ((APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME == temp_music_in_ear_config)
                || (APP_MUSIC_IN_EAR_ONLY_AUTO_PAUSE == temp_music_in_ear_config)) {

            bt_status_t bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PAUSE, NULL);
            if (bt_status != BT_STATUS_SUCCESS) {
                APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_check_and_end_music app_do_music_action fail, bt_status: 0x%x, isAutoPaused: %d", 2,
                        bt_status, ctx->isAutoPaused);
            } else {
                ctx->isAutoPaused = true;
                //ctx->music_playing = false;
                ui_shell_finish_activity(self,self);
                APPS_LOG_MSGID_I(APP_MUSIC_ACTI" ui_shell_finish_activity auto pause music, current activity : %x, isAutoPaused: %d",
                        2, (uint32_t)self, ctx->isAutoPaused);
            }
        }
    }

    /*
     * while the phone paused music, the sink server will report this event with at least 3 seconds delay.
     * so, it's possiable that this activity will receive the music playing request before the sink event.
     * In this case, may means that the old Agent send pause action and finished the shell activity, but
     * the new Agent not recv the SINK_SRV event about the music paused status.
     */
    if (sta_info->previous != APP_IN_EAR_STA_BOTH_IN && sta_info->current != APP_IN_EAR_STA_BOTH_OUT) {
        APPS_LOG_MSGID_I(APP_MUSIC_ACTI"app_music_check_and_end_music: do music playing action while the music playing flag is true, music_palying:%d, isAutoPaused:%d",
                         2, ctx->music_playing, ctx->isAutoPaused);
        if (!ctx->music_playing && ctx->isAutoPaused) {
            if (APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME == temp_music_in_ear_config) {
                bt_status_t bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PLAY, NULL);
                if (bt_status != BT_STATUS_SUCCESS) {
                    APPS_LOG_MSGID_I(APP_MUSIC_ACTI", app_do_music_action fail, bt_status: 0x%x, isAutoPaused: %d", 2,
                                     bt_status, ctx->isAutoPaused);
                } else {
                    ctx->isAutoPaused = false;
                    ctx->music_playing = true;
                }
            }
        }
    }

    return false;
}

#endif /*MTK_IN_EAR_FEATURE_ENABLE*/

static bool app_music_proc_apps_internal_events(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN:
            /* Not need to display LED under A2DP playing music state, so dispatch the event to next activity to display LED */
            break;
        case APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE: {
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI", [UPDATE MMI].", 0);
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            bt_ull_streaming_info_t info = {0};
            bt_ull_streaming_t streaming = {
                .streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER,
                .port = 0,
            };

            if ((BT_STATUS_SUCCESS == bt_ull_get_streaming_info(streaming,&info)) && info.is_playing) {
                apps_config_key_set_mmi_state(APP_ULTRA_LOW_LATENCY_PLAYING);
            } else
#endif
            {
                apps_config_key_set_mmi_state(APP_A2DP_PLAYING);
            }
            ret = true;
        }
            break;

#ifdef MTK_IN_EAR_FEATURE_ENABLE
        case APPS_EVENTS_INTERACTION_IN_EAR_UPDATE_STA: {
            /* The event come from in ear detection app. */
            app_in_ear_sta_info_t *sta_info = (app_in_ear_sta_info_t *)extra_data;
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI", [IN_EAR_UPDATE] %d->%d", 2, sta_info->previous, sta_info->current);
            if (sta_info->previous != sta_info->current) {
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            bt_ull_streaming_info_t info = {0};
            bt_ull_streaming_t streaming = {
                .streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER,
                .port = 0,
            };

            if ((BT_STATUS_SUCCESS == bt_ull_get_streaming_info(streaming,&info)) && info.is_playing) {
                APPS_LOG_MSGID_I(APP_MUSIC_ACTI"[IN_EAR_UPDATE] ull do not update music", 0);
            } else
#endif
                ret = app_music_check_and_end_music(self, extra_data);
            }
            break;
        }
#endif
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        case APPS_EVENTS_INTERACTION_RHO_END:
        case APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT: {
            apps_music_local_context_t *local_context = (apps_music_local_context_t *)self->local_context;
            app_rho_result_t rho_result = (app_rho_result_t)extra_data;
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" RHO done - rho_result=%d, music_playing=%d",
                    2, rho_result, local_context->music_playing);
            if (APP_RHO_RESULT_SUCCESS == rho_result) {
                if (!local_context->music_playing
#ifdef MTK_IN_EAR_FEATURE_ENABLE
                        || APP_IN_EAR_STA_BOTH_OUT == app_in_ear_get_state()
#endif
                )
                {
                    ui_shell_finish_activity(self, self);
                }
            }
            break;
        }
#endif
        default:
            //APPS_LOG_MSGID_I(APP_MUSIC_ACTI" not supported event id = 0x%x", 1, event_id);
            break;
    }

    return ret;
}


bool app_music_activity_proc(
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
            ret = app_music_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef AIR_ROTARY_ENCODER_ENABLE
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER: {
            /* Rotary encoder event. */
            ret = app_music_proc_rotary_event_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* BT_SINK events, indicates the state of music. */
            ret = app_music_proc_bt_event_when_playing(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            /* Interaction events. */
            ret = app_music_proc_apps_internal_events(self, event_id, extra_data, data_len);
            break;
        }
        default:
            break;
    }
    return ret;
}


