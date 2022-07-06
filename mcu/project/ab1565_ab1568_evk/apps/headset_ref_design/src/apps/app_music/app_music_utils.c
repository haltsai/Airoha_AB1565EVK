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
 * File: app_music_utils.c
 *
 * Description: this file provide common functions for music_app.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "app_music_utils.h"
#include "app_music_activity.h"
#include "app_home_screen_idle_activity.h"
#include "bt_sink_srv_music.h"
#include "bt_device_manager.h"
#include "apps_debug.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_ami.h"
#include "apps_events_key_event.h"
#include "apps_events_interaction_event.h"
#include "apps_events_event_group.h"
#ifdef GSOUND_LIBRARY_ENABLE
#include "app_gsound_idle_activity.h"
#endif
#if defined(MTK_AWS_MCE_ENABLE)
#include "bt_aws_mce_srv.h"
#include "bt_aws_mce_report.h"
#include "apps_aws_sync_event.h"
#endif
#include "bt_connection_manager.h"

#ifdef MTK_SMART_CHARGER_ENABLE
#include "app_smcharger_idle_activity.h"
#else
#include "app_battery_transient_activity.h"
#endif
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
#include "bt_ull_service.h"
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_utils.h"
#include "nvkey_id_list.h"
#include "nvkey.h"
#endif
#ifdef AIR_MCSYNC_SHARE_ENABLE
#include "app_share_idle_activity.h"
#endif
#ifdef AIR_LE_AUDIO_ENABLE
#include "app_le_audio_aird_client.h"
#include "app_le_audio_bis.h"
#include "audio_src_srv.h"
#include "bt_sink_srv_le.h"
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le_cap_stream.h"

extern bt_le_sink_srv_music_active_handle g_music_active_handle;
#endif
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "app_dongle_service.h"
#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */
#include "apps_dongle_sync_event.h"

#ifdef MTK_IN_EAR_FEATURE_ENABLE
uint8_t g_music_in_ear_config = APP_MUSIC_IN_EAR_NONE;        /**<  Record the music in ear config. */
#endif

static bool app_music_do_music_actions(bool from_aws_data, ui_shell_activity_t *self, apps_config_key_action_t action)
{
    bt_sink_srv_action_t sink_action = BT_SINK_SRV_ACTION_NONE;
    apps_music_local_context_t *local_context = (apps_music_local_context_t *)self->local_context;
    bt_status_t bt_status = BT_STATUS_FAIL;
    bool op_valid = false;
    bt_sink_srv_avrcp_operation_state_t op = BT_SINK_SRV_AVRCP_OPERATION_PRESS;

    bool ret = true;

    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_action key_action=0x%x, avrcp_op_sta=%d", 2,
                     action, local_context->avrcp_op_sta);

    /* Map key action to sink service action. */
    switch (action) {
        case KEY_AVRCP_PLAY:
        case KEY_AVRCP_PAUSE: {
            sink_action = BT_SINK_SRV_ACTION_PLAY_PAUSE;
            break;
        }
        case KEY_VOICE_UP: {
            sink_action = BT_SINK_SRV_ACTION_VOLUME_UP;
            break;
        }
        case KEY_VOICE_DN: {
            sink_action = BT_SINK_SRV_ACTION_VOLUME_DOWN;
            break;
        }
        case KEY_AVRCP_BACKWARD: {
            sink_action = BT_SINK_SRV_ACTION_PREV_TRACK;
            break;
        }
        case KEY_AVRCP_FORWARD: {
            sink_action = BT_SINK_SRV_ACTION_NEXT_TRACK;
            break;
        }
        case KEY_AVRCP_FAST_FORWARD_PRESS: {
            if (local_context->avrcp_op_sta == AVRCP_OPERATION_STA_IDLE) {
                op = BT_SINK_SRV_AVRCP_OPERATION_PRESS;
                op_valid = true;
                sink_action = BT_SINK_SRV_ACTION_FAST_FORWARD;
                local_context->avrcp_op_sta = AVRCP_OPERATION_STA_FAST_FORWARD_PRESS;
            } else {
                ret = false;
            }
            break;
        }
        case KEY_AVRCP_FAST_FORWARD_RELEASE: {
            if (local_context->avrcp_op_sta == AVRCP_OPERATION_STA_FAST_FORWARD_PRESS) {
                op = BT_SINK_SRV_AVRCP_OPERATION_RELEASE;
                op_valid = true;
                sink_action = BT_SINK_SRV_ACTION_FAST_FORWARD;
                local_context->avrcp_op_sta = AVRCP_OPERATION_STA_IDLE;
            } else {
                ret = false;
            }
            break;
        }
        case KEY_AVRCP_FAST_REWIND_PRESS: {
            if (local_context->avrcp_op_sta == AVRCP_OPERATION_STA_IDLE) {
                op = BT_SINK_SRV_AVRCP_OPERATION_PRESS;
                op_valid = true;
                sink_action = BT_SINK_SRV_ACTION_REWIND;
                local_context->avrcp_op_sta = AVRCP_OPERATION_STA_FAST_REWIND_PRESS;
            } else {
                ret = false;
            }
            break;
        }
        case KEY_AVRCP_FAST_REWIND_RELEASE: {
            if (local_context->avrcp_op_sta == AVRCP_OPERATION_STA_FAST_REWIND_PRESS) {
                op = BT_SINK_SRV_AVRCP_OPERATION_RELEASE;
                op_valid = true;
                sink_action = BT_SINK_SRV_ACTION_REWIND;
                local_context->avrcp_op_sta = AVRCP_OPERATION_STA_IDLE;
            } else {
                ret = false;
            }
            break;
        }
        default: {
            ret = false;
            break;
        }
    }

    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_actions, ret=0x%x, sink_action=0x%x",
            2, ret, sink_action);

#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
    bt_ull_streaming_info_t info = {0};
    bt_ull_streaming_t streaming = {
        .streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER,
        .port = 0,
    };
    uint32_t a2dp_conn_num = 0;
    uint32_t ull_conn_num = 0;
    a2dp_conn_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK),
                                                NULL, 0);
    ull_conn_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL),
                                                NULL, 0);
    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_actions: a2dp_conn_num=%d, ull_conn_num=%d",
            2, a2dp_conn_num, ull_conn_num);

#ifdef MTK_AWS_MCE_ENABLE
        /* Partner can not send action to ull for control music, so relay to agent.*/
        if ((BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role())
                && (TRUE == app_home_screen_idle_activity_is_aws_connected())
                && (BT_SINK_SRV_ACTION_PLAY_PAUSE == sink_action
                        || BT_SINK_SRV_ACTION_PREV_TRACK == sink_action
                        || BT_SINK_SRV_ACTION_NEXT_TRACK == sink_action
                        || BT_SINK_SRV_ACTION_VOLUME_UP == sink_action
                        || BT_SINK_SRV_ACTION_VOLUME_DOWN == sink_action)) {
            bt_status= apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action);
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_actions: partner send music sink action to agent: result=%d",
                    1, bt_status);
            return ret;
        }
#endif
    if (((BT_STATUS_SUCCESS == bt_ull_get_streaming_info(streaming,&info)) && info.is_playing)
            || (ull_conn_num  != 0 && a2dp_conn_num == 0)) {
        if (BT_SINK_SRV_ACTION_VOLUME_UP == sink_action
                || BT_SINK_SRV_ACTION_VOLUME_DOWN == sink_action) {
#ifdef AIR_MS_GIP_ENABLE
            if (app_dongle_service_get_dongle_mode() == APP_DONGLE_SERVICE_DONGLE_MODE_XBOX) {
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS"app_music_do_music_actions route to dongle", 0);
                apps_dongle_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action);
                return ret;
            }
#endif
            bt_ull_volume_t volume_param;
            volume_param.streaming.streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER;
            volume_param.streaming.port = 0;
            volume_param.channel = BT_ULL_VOLUME_CHANNEL_DUEL;
            volume_param.volume = 1; /* The delta value. */
            if (BT_SINK_SRV_ACTION_VOLUME_UP == sink_action) {
                volume_param.action = BT_ULL_VOLUME_ACTION_SET_UP;
            } else if (BT_SINK_SRV_ACTION_VOLUME_DOWN == sink_action) {
                volume_param.action = BT_ULL_VOLUME_ACTION_SET_DOWN;
            }
            bt_status = bt_ull_action(BT_ULL_ACTION_SET_STREAMING_VOLUME, &volume_param, sizeof(volume_param));
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_actions :Change ull volume: result=%d", 1, bt_status);
            return ret;
        } else if (BT_SINK_SRV_ACTION_PLAY_PAUSE == sink_action
                || BT_SINK_SRV_ACTION_PREV_TRACK == sink_action
                || BT_SINK_SRV_ACTION_NEXT_TRACK == sink_action) {
            bt_ull_usb_hid_control_t control_param;
            if (BT_SINK_SRV_ACTION_PLAY_PAUSE == sink_action) {
                control_param = BT_ULL_USB_HID_PLAY_PAUSE_TOGGLE;
            } else if (BT_SINK_SRV_ACTION_PREV_TRACK == sink_action) {
                control_param = BT_ULL_USB_HID_PREVIOUS_TRACK;
            } else if (BT_SINK_SRV_ACTION_NEXT_TRACK == sink_action) {
                control_param = BT_ULL_USB_HID_NEXT_TRACK;
            }
            bt_status = bt_ull_action(BT_ULL_ACTION_USB_HID_CONTROL, &control_param, sizeof(control_param));
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_actions: Change ull play: result=%d", 1, bt_status);
            return ret;
        }
    }
#endif

#ifdef AIR_LE_AUDIO_BIS_ENABLE
    if (KEY_VOICE_UP == action || KEY_VOICE_DN == action) {
        bool is_bis_streaming = app_le_audio_bis_is_streaming();
        bt_sink_srv_am_volume_level_out_t volume = bt_sink_srv_cap_stream_get_broadcast_volume();
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [LEA][BIS] VOLUME, action=0x%04X from_aws_data=%d is_bis_streaming=%d volume=%d",
                         4, action, from_aws_data, is_bis_streaming, volume);
        if (is_bis_streaming) {
#ifdef MTK_AWS_MCE_ENABLE
            if (!from_aws_data && bt_aws_mce_srv_get_link_type() != BT_AWS_MCE_SRV_LINK_NONE) {
                bt_status = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action);
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [LEA][BIS] VOLUME, bt_status=0x%08X", 1, bt_status);
            }
#endif

            if (KEY_VOICE_UP == action) {
                volume += 1;
                if (volume > AUD_VOL_OUT_LEVEL15) {
                    volume = AUD_VOL_OUT_LEVEL15;
                }
            } else if (KEY_VOICE_DN == action) {
                if (volume == AUD_VOL_OUT_LEVEL0) {
                    // ignore
                } else if (volume >= AUD_VOL_OUT_LEVEL15) {
                    volume = AUD_VOL_OUT_LEVEL14;
                } else {
                    volume -= 1;
                }
            }
            bool success = bt_sink_srv_cap_stream_set_broadcast_volume(volume);
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [LEA][BIS] VOLUME set, volume=%d success=%d",
                             2, volume, success);
            return ret;
        }
    }
#endif

#ifdef AIR_LE_AUDIO_ENABLE
        if ((KEY_VOICE_UP == action) || (KEY_VOICE_DN == action)) {
            app_le_audio_aird_client_info_t *p_info;
            bt_handle_t handle;

            if ((BT_HANDLE_INVALID != (handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established())) &&
                (NULL != (p_info = app_le_audio_aird_client_get_info(handle))) &&
                (APP_LE_AUDIO_AIRD_MODE_SUPPORT_HID_CALL == p_info->mode)) {

                app_le_audio_aird_action_set_streaming_volume_t param;
                bt_status_t status;

                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [LEA][CIS] app_music_do_music_actions, action:0x%04X handle:%x", 2, action, handle);

                param.streaming_interface = APP_LE_AUDIO_AIRD_STREAMING_INTERFACE_SPEAKER;
                param.streaming_port = APP_LE_AUDIO_AIRD_STREAMING_PORT_0;
                param.channel = APP_LE_AUDIO_AIRD_CHANNEL_DUAL;
                param.volume = 1;   /* The delta value. */

                if (KEY_VOICE_UP == action) {
                    status = app_le_audio_aird_client_send_action(handle,
                                                                APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_UP,
                                                                &param,
                                                                sizeof(app_le_audio_aird_action_set_streaming_volume_t));
                } else if (KEY_VOICE_DN == action) {
                    status = app_le_audio_aird_client_send_action(handle,
                                                                APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_DOWN,
                                                                &param,
                                                                sizeof(app_le_audio_aird_action_set_streaming_volume_t));
                }

                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [LEA][CIS] VOLUME set, status:%x", 1, status);
                return ret;
            }
        } else if (KEY_AVRCP_PLAY == action || KEY_AVRCP_PAUSE == action) {
            app_le_audio_aird_client_info_t *p_info;
            bt_handle_t handle;

            handle = g_music_active_handle.handle;
            const audio_src_srv_handle_t *current_device = audio_src_srv_get_runing_pseudo_device();

            if ((BT_HANDLE_INVALID != handle) &&
                (current_device == NULL || (current_device->type != AUDIO_SRC_SRV_PSEUDO_DEVICE_A2DP && current_device->type != AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_A2DP)) &&
                (NULL != (p_info = app_le_audio_aird_client_get_info(handle))) &&
                (APP_LE_AUDIO_AIRD_MODE_SUPPORT_HID_CALL == p_info->mode)) {

                bt_status_t status;

                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [LEA][CIS] app_music_do_music_actions, action:0x%04X handle:%x", 2, action, handle);

                status = app_le_audio_aird_client_send_action(handle,
                                                            APP_LE_AUDIO_AIRD_ACTION_TOGGLE_PLAY,
                                                            NULL,
                                                            0);

                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [LEA][CIS] TOGGLE PLAY, status:%x", 1, status);
                return ret;
            }
        }
#endif

    if (ret) {
#ifdef GSOUND_LIBRARY_ENABLE
        bool gsound_ret = false;
        gsound_ret = app_gsound_idle_activity_proc_music_event(sink_action);
        if (gsound_ret) {
            /* Gsound app may intercept this action, refer to gsound app for more details. */
            ret = false;
        } else
#endif
        {
            if (op_valid) {
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_action, op=0x%x, sink_action=0x%x", 2, op, sink_action);
                bt_status = bt_sink_srv_send_action(sink_action, &op);
            } else {
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_action, op=0x%x, sink_action=0x%x", 2, op, sink_action);
                bt_status = bt_sink_srv_send_action(sink_action, NULL);
            }
            if (bt_status != BT_STATUS_SUCCESS) {
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_action fail, bt_status=0x%x, sink_action=0x%x",
                        2, bt_status, sink_action);
            }
        }
    }
    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_do_music_action ret=%d", 1, ret);
    return ret;
}

apps_config_key_action_t app_music_utils_proc_key_events(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    uint8_t key_id;
    airo_key_event_t key_event;
    bool ret = false;

    /* Decode event_id to key_id and key_event. */
    app_event_key_event_decode(&key_id, &key_event, event_id);

    apps_config_key_action_t action;

    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }

    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" key_id=0x%x, key_event=0x%x, key_action=0x%x",
            3, key_id, key_event, action);

    if (!app_music_get_curr_link_is_connected()) {
        return KEY_ACTION_INVALID;
    }

    if (KEY_ACTION_INVALID != action) {
        ret = app_music_do_music_actions(FALSE, self, action);
    }

    if (ret) {
        return action;
    } else {
        return KEY_ACTION_INVALID;
    }
}

bool app_music_idle_proc_bt_sink_events(ui_shell_activity_t *self, uint32_t event_id,
                                         void *extra_data, size_t data_len)
{
    bool ret = false;
    apps_music_local_context_t *local_context = (apps_music_local_context_t *)self->local_context;
    if (local_context == NULL) {
        return ret;
    }

    if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
        bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *) extra_data;
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_bt_sink_events param->pre=0x%x,  param->now=0x%x, is_playing=%d",
                3, param->previous, param->current, local_context->music_playing);
        /* Try to start app_music_activity when the music starts playing. */
        if ((param->previous != BT_SINK_SRV_STATE_STREAMING) && (param->current == BT_SINK_SRV_STATE_STREAMING)) {
            /* If music_playing is true, it indicates that the app_music_activity already exists. */
            if (!local_context->music_playing) {
                local_context->isAutoPaused = false;
                ui_shell_start_activity(self, app_music_activity_proc, ACTIVITY_PRIORITY_MIDDLE, local_context, 0);
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_bt_sink_events ui_shell_start_activity , current activity : %x, isAutoPaused: %d",
                        2, (uint32_t)self, local_context->isAutoPaused);
                local_context->music_playing = true;
            }
        }
        else if ((param->previous == BT_SINK_SRV_STATE_STREAMING) && (param->current != BT_SINK_SRV_STATE_STREAMING)) {
            local_context->music_playing = false;
        }
    }
#if defined(MTK_IN_EAR_FEATURE_ENABLE)
    else if (event_id == BT_SINK_SRV_EVENT_AVRCP_STATUS_CHANGE) {
        bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)extra_data;
        bt_sink_srv_state_t bt_sink_state = bt_sink_srv_get_state();
        bt_avrcp_status_t avrcp_status = event->avrcp_status_change.avrcp_status;
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_bt_event: avrcp_status=%d, sink_state=%x"
                , 2, avrcp_status, bt_sink_state);
        if (BT_AVRCP_STATUS_PLAY_PLAYING == avrcp_status) {
            memcpy(local_context->activate_con_addr, event->avrcp_status_change.address, sizeof(bt_bd_addr_t));
            APPS_LOG_MSGID_I(APP_MUSIC_ACTI" app_music_proc_bt_event: avrcp_addr-%02x:%02x:%02x:%02x:%02x:%02x",
                    6, event->avrcp_status_change.address[5], event->avrcp_status_change.address[4], event->avrcp_status_change.address[3],
                    event->avrcp_status_change.address[2], event->avrcp_status_change.address[1], event->avrcp_status_change.address[0]);
            if ((!local_context->music_playing) && (bt_sink_state < BT_SINK_SRV_STATE_INCOMING)) {
                local_context->isAutoPaused = false;
                local_context->music_playing = true;
                ui_shell_start_activity(self, app_music_activity_proc, ACTIVITY_PRIORITY_MIDDLE, local_context, 0);
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_bt_sink_events ui_shell_start_activity , current activity : %x, isAutoPaused: %d",
                        2, (uint32_t)self, local_context->isAutoPaused);
            }
        }
        else if ((BT_AVRCP_STATUS_PLAY_PAUSED == avrcp_status) || (BT_AVRCP_STATUS_PLAY_STOPPED == avrcp_status)) {
            if (0 == memcmp(local_context->activate_con_addr, event->avrcp_status_change.address, sizeof(bt_bd_addr_t))){
                local_context->music_playing = false;
            }
        }
    }
#endif
    return ret;
}

bool app_music_idle_proc_bt_cm_events(ui_shell_activity_t *self, uint32_t event_id,
                                     void *extra_data, size_t data_len)
{
    bool ret = false;
#ifdef MTK_AWS_MCE_ENABLE
    apps_music_local_context_t *local_context = (apps_music_local_context_t *)self->local_context;
    bt_aws_mce_role_t role;
    role = bt_device_manager_aws_local_info_get_role();
#endif

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
#ifdef MTK_AWS_MCE_ENABLE
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == local_context || NULL == remote_update) {
                break;
            }

            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
                if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Update the partner connection state when AWS is disconnected. */
                    local_context->isPartnerConnected = false;
                    local_context->isPartnerCharging = false;
                    app_music_checkAudioState(local_context);
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_bt_cm_events, Partner Detached.", 0);
                } else if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Update the partner connection state when AWS is connected. */
                    local_context->isPartnerConnected = true;
#ifdef MTK_IN_EAR_FEATURE_ENABLE
                    app_music_notify_state_to_peer();
#endif
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_bt_cm_events, Partner Attached.", 0);
                }
            } else if (BT_AWS_MCE_ROLE_PARTNER == role) {
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* For partner, set music mixing mode to STEREO when AWS is connected. */
                    am_dynamic_change_channel(AUDIO_CHANNEL_SELECTION_STEREO);
                    local_context->currMixState = MUSIC_STEREO;
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_bt_cm_events set partner as stereo.", 0);
                }
                if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
#ifdef AIR_MCSYNC_SHARE_ENABLE
                    if (app_share_get_pairing_result()) {
                        APPS_LOG_MSGID_I("app_music_activity_proc, music share mode, partner ignore check audio state", 0);
                        return ret;
                    }
#endif
                    am_dynamic_change_channel(AUDIO_CHANNEL_SELECTION_MONO);
                    local_context->currMixState = MUSIC_MONO;
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_bt_cm_events set partner as mono.", 0);
                }
            }
#endif
        }
        break;
        default:
            break;
    }
    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
bool app_music_idle_proc_aws_data_events(ui_shell_activity_t *self, uint32_t event_id,
                                      void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    apps_music_local_context_t *local_context = (apps_music_local_context_t *)self->local_context;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

#ifdef MTK_IN_EAR_FEATURE_ENABLE
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action,
                            &p_extra_data, &extra_data_len);
        if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
            && action == APPS_EVENTS_INTERACTION_SYNC_MUSIC_IN_EAR_CFG) {
            uint8_t music_in_ear_cfg = *(uint8_t *)p_extra_data;
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] received music in ear cfg:music_in_ear_cfg=%d", 1, music_in_ear_cfg);
            app_music_set_in_ear_control(music_in_ear_cfg, false);
        }
    }
#endif

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;
        apps_aws_sync_event_decode(aws_data_ind, &event_group, &action);
        if (event_group == EVENT_GROUP_UI_SHELL_KEY) {
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [LEA][BIS] [%02X] received key action=0x%04X",
                             2, role, action);
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            if (role == BT_AWS_MCE_ROLE_AGENT) {
                ret = app_music_do_music_actions(TRUE, self, action);
            }
#elif defined(AIR_LE_AUDIO_BIS_ENABLE)
            ret = app_music_do_music_actions(TRUE, self, action);
#endif
        }
    }

    uint8_t battery = *(uint8_t *)aws_data_ind->param;
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_BATTERY
        && role == BT_AWS_MCE_ROLE_AGENT) {
        /* Partner's battery app will report this event when AWS is connected. */
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_aws_data_events Partner charge percent=%d.", 1, battery);

        local_context->isPartnerConnected = true;
#ifdef MTK_SMART_CHARGER_ENABLE
        local_context->isPartnerCharging = battery & PARTNER_BATTERY_CHARGING ? true : false;
#else
        local_context->isPartnerCharging = battery & PARTNER_BATTERY_CHARGING ? true : false;
#endif
        /* Update music mixing mode when parter's battery info updated. */
        app_music_checkAudioState(local_context);
    }
    return ret;
}
void app_music_checkAudioState(apps_music_local_context_t *cntx)
{

    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_checkAudioState, currMixState=%d, isPartner_conn=%d, isChagring=%d,isBoothInEar=%d",
               4, cntx->currMixState, cntx->isPartnerConnected, cntx->isPartnerCharging, cntx->isBoothInEar );

#ifdef AIR_MCSYNC_SHARE_ENABLE
    if (app_share_get_pairing_result()) {
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_checkAudioState, music share mode, agent ignore check audio state", 0);
        return;
    }
#endif

#if  defined(MTK_IN_EAR_FEATURE_ENABLE)
    if (cntx->isPartnerConnected && !cntx->isPartnerCharging && cntx->isBoothInEar)
#else
    if (cntx->isPartnerConnected && !cntx->isPartnerCharging)
#endif
    {
        /* Set music mixing mode to STEREO when left and right earbuds are in ear. */
        if (cntx->currMixState != MUSIC_STEREO) {
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_checkAudioState set to stereo", 0);
            am_dynamic_change_channel(AUDIO_CHANNEL_SELECTION_STEREO);
            cntx->currMixState = MUSIC_STEREO;
        }
    } else {
        /* Set music mixing mode to MONO when the partner may not be in the ear. */
        if (cntx->currMixState != MUSIC_MONO) {
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_checkAudioState set to mono", 0);
            am_dynamic_change_channel(AUDIO_CHANNEL_SELECTION_MONO);
            cntx->currMixState = MUSIC_MONO;
        }
    }
}
#endif

#ifdef GSOUND_LIBRARY_ENABLE
bool app_music_idle_proc_gsound_reject_action(ui_shell_activity_t *self, bt_sink_srv_action_t sink_action)
{
    apps_music_local_context_t *local_ctx = (apps_music_local_context_t *)self->local_context;
    bool ret = false;
    bt_status_t bt_status;

    /* Handle the actions that were intercepted by gsound but not processed. */
    switch (sink_action) {
        case BT_SINK_SRV_ACTION_PLAY_PAUSE:
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS", app_music_proc_gsound_reject_action, sink_action : 0x%x", 1, sink_action);
            bt_status = bt_sink_srv_send_action(sink_action, NULL);
            if (bt_status != BT_STATUS_SUCCESS) {
                APPS_LOG_MSGID_W(APP_MUSIC_UTILS", app_music_proc_gsound_reject_action fail, bt_status: 0x%x, sink_action : 0x%x", 2, bt_status, sink_action);
            } else {
                if (local_ctx->music_playing) {
                    /*
                    ui_shell_finish_activity(self, self);
                    APPS_LOG_MSGID_I("ui_shell_finish_activity app_music_activity_proc, current activity : 0x%x", 1, (uint32_t)self);
                    local_ctx->music_playing = false;
                    */
                } else {
                    local_ctx->isAutoPaused = false;
                    ui_shell_start_activity(self, app_music_activity_proc, ACTIVITY_PRIORITY_MIDDLE, local_ctx, 0);
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_proc_gsound_reject_action ui_shell_start_activity app_music_activity_proc, current activity : %x, isAutoPaused: %d", 2,
                                     (uint32_t)self, local_ctx->isAutoPaused);
                    local_ctx->music_playing = true;
                }
            }
            ret = true;
            break;
        case BT_SINK_SRV_ACTION_NEXT_TRACK:
        case BT_SINK_SRV_ACTION_PREV_TRACK:
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS", app_music_proc_gsound_reject_action, sink_action : 0x%x", 1, sink_action);
            bt_status = bt_sink_srv_send_action(sink_action, NULL);
            if (bt_status != BT_STATUS_SUCCESS) {
                APPS_LOG_MSGID_W(APP_MUSIC_UTILS", app_music_proc_gsound_reject_action fail, bt_status: 0x%x, sink_action : 0x%x", 2, bt_status, sink_action);
            }
            ret = true;
            break;
        default:
            break;
    }

    return ret;
}
#endif

#ifdef MTK_IN_EAR_FEATURE_ENABLE
/**
* @brief      This function is used to resume music when the earbud was put into the ear.
* @param[in]  self, the context pointer of the activity.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_music_idle_check_and_start_music(struct _ui_shell_activity *self, void *extra_data)
{
    apps_music_local_context_t *ctx = (apps_music_local_context_t *)self->local_context;
    app_in_ear_sta_info_t *sta_info = (app_in_ear_sta_info_t *)extra_data;
    uint8_t temp_music_in_ear_config = app_music_get_in_ear_control_state();
#ifdef MTK_AWS_MCE_ENABLE
    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_idle_check_and_start_music, cur=%d, pre=%d, isAutoPaused=%d, music_playing=%d isPartnerCharging=%d, isPartnerConnected=%d, in_ear_config=%d",
            7, sta_info->current, sta_info->previous, ctx->isAutoPaused, ctx->music_playing, ctx->isPartnerCharging, ctx->isPartnerConnected, temp_music_in_ear_config);
#else
    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" app_music_idle_check_and_start_music, cur=%d, pre=%d, isAutoPaused=%d. in_ear_config=%d",
            4, sta_info->current, sta_info->previous, ctx->isAutoPaused, temp_music_in_ear_config);
#endif

    /* Resume music when the earbud was put into the ear. */
    if (ctx->isAutoPaused && sta_info->previous != APP_IN_EAR_STA_BOTH_IN && sta_info->current != APP_IN_EAR_STA_BOTH_OUT) {
        if (!ctx->music_playing) {
            if (APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME == temp_music_in_ear_config) {
                bt_status_t bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PLAY, NULL);
                if (bt_status != BT_STATUS_SUCCESS) {
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS", app_do_music_action fail, bt_status: 0x%x", 1, bt_status);
                } else {
                    ctx->isAutoPaused = false;
                    //ui_shell_start_activity(self, app_music_activity_proc, ACTIVITY_PRIORITY_MIDDLE, ctx, 0);
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" ui_shell_start_activity auto resume music, current activity : %x, isAutoPaused: %d",
                            2, (uint32_t)self, ctx->isAutoPaused);
                    //ctx->music_playing = true;
                }
            }
        }
    }

    /*
     * this is a sepcial case. it is means that the old Agent send the play action and start the shell activity, but the new Agent
     * still in pause status because of the SINK_SRV not report the event about the music to play status.
     */
#ifdef IN_EAR_FEATURE_AIROHA
    if (sta_info->previous != APP_IN_EAR_STA_BOTH_OUT && sta_info->current == APP_IN_EAR_STA_BOTH_OUT)
#else
    if (sta_info->current != APP_IN_EAR_STA_BOTH_IN && sta_info->previous != APP_IN_EAR_STA_BOTH_OUT)
#endif
    {
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS"app_music_idle_check_and_start_music recv pause request when it was paused. music_playing:%d", 1, ctx->music_playing);
        if (ctx->music_playing) {
            if ((APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME == temp_music_in_ear_config)
                    || (APP_MUSIC_IN_EAR_ONLY_AUTO_PAUSE == temp_music_in_ear_config)) {
                bt_status_t bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PAUSE, NULL);
                if (bt_status != BT_STATUS_SUCCESS) {
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS", app_do_music_action fail, bt_status: 0x%x", 1, bt_status);
                } else {
                    //ctx->music_playing = false;
                    ctx->isAutoPaused = true;
                }
            }
        }
    }
    return false;
}
#endif /*MTK_IN_EAR_FEATURE_ENABLE*/

bool app_music_idle_proc_apps_internal_events(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_id) {
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        case APPS_EVENTS_INTERACTION_IN_EAR_UPDATE_STA: {
            /* The event come from in ear detection app. */
            app_in_ear_sta_info_t *sta_info = (app_in_ear_sta_info_t *)extra_data;
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS", [IN_EAR_UPDATE] %d->%d", 2, sta_info->previous, sta_info->current);
            if (sta_info->previous != sta_info->current) {
                app_music_in_ear_update_mix_state(self, extra_data);
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
                bt_ull_streaming_info_t info = {0};
                bt_ull_streaming_t streaming = {
                        .streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER,
                        .port = 0,
                };

                if ((BT_STATUS_SUCCESS == bt_ull_get_streaming_info(streaming,&info)) && info.is_playing) {
                    APPS_LOG_MSGID_I(APP_MUSIC_UTILS"[IN_EAR_UPDATE] ull do not update music", 0);
                } else
#endif
                    ret = app_music_idle_check_and_start_music(self, extra_data);
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_IN_EAR_CFG_UPDATE: {
            uint8_t* music_in_ear_cfg = (uint8_t*)extra_data;
            bool setResult = false;
            if (0 == *music_in_ear_cfg) {
                setResult = app_music_set_in_ear_control(APP_MUSIC_IN_EAR_DISABLE, false);
            } else if (1 == *music_in_ear_cfg) {
                setResult = app_music_set_in_ear_control(APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME, false);
            } else {
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] music in ear cfg error: music_in_ear_cfg=%d", 1, *music_in_ear_cfg);
            }

            if (true == setResult) {
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] set in ear control value success: isEnable=%d.",
                        1, *music_in_ear_cfg);
            } else {
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] set in ear control value fail!", 0);
            }
            break;
        }
#endif
#ifdef GSOUND_LIBRARY_ENABLE
        case APPS_EVENTS_INTERACTION_GSOUND_ACTION_REJECTED:
            /* Handle actions related to music control that were intercepted by gsound but not processed. */
            if (!app_music_get_curr_link_is_connected()) {
                break;
            }
            ret = app_music_idle_proc_gsound_reject_action(self, (bt_sink_srv_action_t)extra_data);
            break;
#endif
        default:
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" not supported event id=0x%x", 1, event_id);
            break;
    }

    return ret;
}

#ifdef MTK_IN_EAR_FEATURE_ENABLE
/**
* @brief      This function is used to obtain the status of the function of controlling music through in ear detection.
* @return     Return the configuration of music in ear.
*/
uint8_t app_music_get_in_ear_control_state()
{
    nvkey_status_t status = NVKEY_STATUS_OK;
    uint8_t temp_in_ear_cfg;
    uint32_t data_size = sizeof(uint8_t);
    if (APP_MUSIC_IN_EAR_NONE != g_music_in_ear_config) {
        return g_music_in_ear_config;
    } else {
        status = nvkey_read_data(NVKEYID_APP_IN_EAR_MUSIC_ABILITY, &temp_in_ear_cfg, &data_size);
        if (NVKEY_STATUS_OK == status) {
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] get music in ear cfg success: temp_in_ear_cfg=%d.",1, temp_in_ear_cfg);
        } else if (NVKEY_STATUS_ITEM_NOT_FOUND == status) {
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] get music in ear cfg not found!", 0);
            temp_in_ear_cfg = APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME;
            status = nvkey_write_data(NVKEYID_APP_IN_EAR_MUSIC_ABILITY, &temp_in_ear_cfg, sizeof(uint8_t));
            if (NVKEY_STATUS_OK != status) {
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] get music in ear cfg again fail!", 0);
            } else {
                g_music_in_ear_config = temp_in_ear_cfg;
                APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] get music in ear default cfg success: temp_in_ear_cfg=%d",
                        1, temp_in_ear_cfg);
                return temp_in_ear_cfg;
            }
        }
    }
    return temp_in_ear_cfg;
}


/**
* @brief      This function is used to set the function of controlling music through ear detection.
* @param[in]  music_in_ear_cfg, music in ear configuration.
* @param[in]  isSync, whether sync music_in_ear_cfg to other side. false for RACE CMD, true for the other application.
* @return     If return true, set successfully.
*/
bool app_music_set_in_ear_control(uint8_t music_in_ear_cfg, bool isSync)
{
    bool ret = false;
    nvkey_status_t status = NVKEY_STATUS_OK;
    status = nvkey_write_data(NVKEYID_APP_IN_EAR_MUSIC_ABILITY, &music_in_ear_cfg, sizeof(uint8_t));
    if (status != NVKEY_STATUS_OK) {
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] set in ear control value failed: %d.", 1, status);
        return ret;
    } else {
        ret = true;
        g_music_in_ear_config = music_in_ear_cfg;
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] set in ear control value success: music_in_ear_cfg=%d",1, music_in_ear_cfg);
#ifdef MTK_AWS_MCE_ENABLE
        if ((TRUE == isSync) && (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role())) {
            app_music_notify_state_to_peer();
        }
#endif
    }
    return ret;
}

void app_music_notify_state_to_peer()
{
    uint8_t music_in_ear_cfg = app_music_get_in_ear_control_state();
#ifdef MTK_AWS_MCE_ENABLE
    if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
        bt_status_t send_state = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                APPS_EVENTS_INTERACTION_SYNC_MUSIC_IN_EAR_CFG, &music_in_ear_cfg, sizeof(uint8_t));
        if (BT_STATUS_SUCCESS == send_state) {
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] sync music in ear control success.", 0);
        } else if (BT_STATUS_FAIL == send_state) {
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" [IN EAR] sync music in ear control fail.", 0);
        }
    }
#endif
}

bool app_music_in_ear_update_mix_state(struct _ui_shell_activity *self, void *extra_data)
{
    apps_music_local_context_t *ctx = (apps_music_local_context_t *)self->local_context;
    app_in_ear_sta_info_t *sta_info = (app_in_ear_sta_info_t *)extra_data;
#ifdef MTK_AWS_MCE_ENABLE
    if (sta_info->current != APP_IN_EAR_STA_BOTH_IN) {
        ctx->isBoothInEar = false;
    } else {
        ctx->isBoothInEar = true;
    }
    APPS_LOG_MSGID_I(APP_MUSIC_UTILS" isBoothInEar=%d", 1, ctx->isBoothInEar);
    if (sta_info->current != APP_IN_EAR_STA_BOTH_OUT) {
        app_music_checkAudioState(ctx);
    }
#endif
    return false;
}
#endif  /*MTK_IN_EAR_FEATURE_ENABLE*/

bool app_music_set_volume_value(bool isUp, uint32_t value)
{
    bool ret = FALSE;
    bt_status_t bt_status = BT_STATUS_FAIL;
    uint8_t volume = 0;

    bt_sink_srv_state_t bt_sink_state = bt_sink_srv_get_state();
    if (bt_sink_state >= BT_SINK_SRV_STATE_INCOMING) {
        volume = bt_sink_srv_get_volume(NULL, BT_SINK_SRV_VOLUME_HFP);
        if (isUp == TRUE) {
            if (volume + value < AUD_VOL_OUT_MAX) {
                volume += value;
            } else {
                volume = AUD_VOL_OUT_LEVEL15;
            }
        } else {
            if (volume > value) {
                volume -= value;
            } else {
                volume = 0;
            }
        }
        bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CALL_SET_VOLUME, &volume);
    } else {
        volume = bt_sink_srv_get_volume(NULL, BT_SINK_SRV_VOLUME_A2DP);
        if (isUp == TRUE) {
            if (volume + value < bt_sink_srv_ami_get_a2dp_max_volume_level()) {
                volume += value;
            } else {
                volume = bt_sink_srv_ami_get_a2dp_max_volume_level();
            }
        } else {
            if (volume > value) {
                volume -= value;
            } else {
                volume = 0;
            }
        }
        bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_SET_VOLUME, &volume);
    }

    if (bt_status == BT_STATUS_SUCCESS) {
        ret = TRUE;
    }
    APPS_LOG_MSGID_I(APP_MUSIC_UTILS"app_music_set_volume_value: ret=%d, isUp=%d, value=%d, volume_level=%d, bt_sink_state=%x",
            5, ret, isUp, value, volume, bt_sink_state);
    return ret;
}

bool app_music_get_curr_link_is_connected(void)
{
    bool ret = true;

#ifndef MTK_AWS_MCE_ENABLE
    if (0 == bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL), NULL, 0)) {
#ifdef AIR_LE_AUDIO_ENABLE
        bt_handle_t le_handle = bt_sink_srv_cap_check_links_state(BT_SINK_SRV_CAP_STATE_CONNECTED);
        bool is_bis_streaming = bt_sink_srv_cap_stream_is_broadcast_streaming();
        if (!is_bis_streaming && BT_HANDLE_INVALID == le_handle) {
            APPS_LOG_MSGID_I(APP_MUSIC_UTILS" NOT connected A2DP/ULL/CIS/BIS, ignore KEY Action", 0);
            return false;
        }
#else
        APPS_LOG_MSGID_I(APP_MUSIC_UTILS" NOT connected A2DP or ULL, ignore KEY Action", 0);
        return false;
#endif
    }
#endif

    return ret;
}

