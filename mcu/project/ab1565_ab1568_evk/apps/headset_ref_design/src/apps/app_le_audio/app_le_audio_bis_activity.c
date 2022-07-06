
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
 * File: app_le_audio_bis_activity.c
 *
 * Description:
 * This file is LE Audio BIS activity.
 * Start the activity when BIS streaming.
 *
 */

#ifdef AIR_LE_AUDIO_BIS_ENABLE

#include "app_le_audio_bis_activity.h"

#include "app_le_audio.h"
#include "apps_events_interaction_event.h"
#include "apps_events_event_group.h"
#include "apps_config_state_list.h"
#include "apps_config_key_remapper.h"
#include "apps_debug.h"
#include "bt_le_audio_util.h"

#include "ui_shell_manager.h"

#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_utils.h"
#endif
#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_le_volume.h"
#endif
#ifdef AIR_ROTARY_ENCODER_ENABLE
#include "apps_events_key_event.h"
#endif

#define APP_LE_AUDIO_BIS_TAG           "[LEA][BIS]"

#ifdef MTK_IN_EAR_FEATURE_ENABLE
static void app_le_audio_bis_mute(bool in_ear)
{
    #ifdef AIR_LE_AUDIO_ENABLE
    if (in_ear) {
        bt_status_t status = bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, FALSE);
        APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" IN_EAR unmute=0x%08X", 1, status);
    } else {
        bt_status_t status = bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, TRUE);
        APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" IN_EAR mute=0x%08X", 1, status);
    }
	#else
		APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" LE Audio is not enabled", 0);
	#endif
}
#endif

static bool app_le_audio_bis_activity_proc_ui_shell_group(ui_shell_activity_t *self,
                                                          uint32_t event_id,
                                                          void *extra_data,
                                                          size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" activity CREATE", 0);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN,
                                NULL, 0, NULL, 0);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE,
                                NULL, 0, NULL, 0);
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            bool in_ear = app_in_ear_get_own_state();
            APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" activity in_ear=%d", 1, in_ear);
            app_le_audio_bis_mute(in_ear);
#endif
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" activity DESTROY", 0);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN,
                                NULL, 0, NULL, 0);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE,
                                NULL, 0, NULL, 0);
            break;
        }
        default:
            break;
    }
    return TRUE;
}

static bool app_le_audio_bis_activity_proc_interaction_group(ui_shell_activity_t *self,
                                                             uint32_t event_id,
                                                             void *extra_data,
                                                             size_t data_len)
{
    bool ret = FALSE;
    switch (event_id) {
        case APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN: {
            // BIS streaming LED pattern
            // APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" LED_BG pattern", 0);
            break;
        }
        case APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE: {
            APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" MMI BIS_PLAYING state", 0);
            apps_config_key_set_mmi_state(APP_LE_AUDIO_BIS_PLAYING);
            ret = TRUE;
            break;
        }
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        case APPS_EVENTS_INTERACTION_UPDATE_IN_EAR_STA_EFFECT: {
            if (extra_data != NULL) {
                bool in_ear = *((bool *)extra_data);
                APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" IN_EAR update state=%d", 1, in_ear);
                app_le_audio_bis_mute(in_ear);
            }
            break;
        }
#endif
    }
    return ret;
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
static bool app_le_audio_bis_activity_proc_rotary_group(ui_shell_activity_t *self,
                                                        uint32_t event_id,
                                                        void *extra_data,
                                                        size_t data_len)
{
    bool ret = FALSE;
    bsp_rotary_encoder_port_t port;
    bsp_rotary_encoder_event_t event;
    uint32_t rotary_data;
    if (extra_data == NULL) {
        APPS_LOG_MSGID_E(APP_LE_AUDIO_BIS_TAG" ROTARY event, NULL extra_data", 0);
        return FALSE;
    }
    apps_config_key_action_t key_action = *(uint16_t *)extra_data;
    app_event_rotary_event_decode(&port, &event, &rotary_data, event_id);

    switch (key_action) {
        case KEY_VOICE_UP:
        case KEY_VOICE_DN: {
            bool is_bis_streaming = app_le_audio_bis_is_streaming();
            bt_sink_srv_am_volume_level_out_t volume = bt_sink_srv_cap_stream_get_broadcast_volume();
            APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" ROTARY event, action=0x%04X is_bis_streaming=%d volume=%d rotary_data=%d",
                             4, key_action, is_bis_streaming, volume, rotary_data);
            if (is_bis_streaming) {
                if (KEY_VOICE_UP == key_action) {
                    volume += rotary_data;
                    if (volume > AUD_VOL_OUT_LEVEL15) {
                        volume = AUD_VOL_OUT_LEVEL15;
                    }
                } else if (KEY_VOICE_DN == key_action) {
                    if (volume == AUD_VOL_OUT_LEVEL0) {
                        // ignore
                    } else {
                        volume -= rotary_data;
                    }
                    if (volume >= AUD_VOL_OUT_LEVEL15) {
                        volume = AUD_VOL_OUT_LEVEL14;
                    }
                }
                bool success = bt_sink_srv_cap_stream_set_broadcast_volume(volume);
                ret = TRUE;
                APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" ROTARY event, volume=%d success=%d",
                                 2, volume, success);
            }
        }
    }
    return ret;
}
#endif

bool app_le_audio_bis_activity_proc(struct _ui_shell_activity *self,
                                            uint32_t event_group,
                                            uint32_t event_id,
                                            void *extra_data,
                                            size_t data_len)
{
    bool ret = FALSE;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = app_le_audio_bis_activity_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            ret = app_le_audio_bis_activity_proc_interaction_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef AIR_ROTARY_ENCODER_ENABLE
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER: {
            ret = app_le_audio_bis_activity_proc_rotary_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_LE_AUDIO: {
            if (event_id == EVENT_ID_LE_AUDIO_BIS_STOP_STREAMING) {
                APPS_LOG_MSGID_I(APP_LE_AUDIO_BIS_TAG" Finish BIS Activity", 0);
                ui_shell_finish_activity(self, self);
            }
            break;
        }
    }
    return ret;
}

#endif /* AIR_LE_AUDIO_BIS_ENABLE */
