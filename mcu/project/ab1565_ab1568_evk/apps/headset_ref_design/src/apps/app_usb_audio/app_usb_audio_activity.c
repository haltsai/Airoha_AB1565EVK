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
 /* Airoha restricted information */

#include "usbaudio_drv.h"
#include "app_usb_audio_activity.h"
#include "app_usb_audio_idle_activity.h"
#include "app_usb_audio_utils.h"
#include "apps_config_state_list.h"
#include "apps_events_key_event.h"

static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            USB_AUDIO_LOG_I("activity create", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            USB_AUDIO_LOG_I("activity destory", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                NULL, 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            USB_AUDIO_LOG_I("activity resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            USB_AUDIO_LOG_I("activity pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            USB_AUDIO_LOG_I("activity refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            USB_AUDIO_LOG_I("activity result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

#ifdef AIR_USB_HID_MEDIA_CTRL_ENABLE
static bool _proc_key_event_group(
        struct _ui_shell_activity *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;
    apps_config_key_action_t action;
    uint8_t key_id;
    airo_key_event_t key_event;
    app_event_key_event_decode(&key_id, &key_event, event_id);

    action = apps_config_key_event_remapper_map_action_in_temp_state(key_id, key_event, APP_WIRED_MUSIC_PLAY);

    switch(action) {
        case KEY_AVRCP_BACKWARD:
            USB_AUDIO_LOG_I("activity backward", 0);
            USB_Audio_HID_ScanPreviousTrack();
            ret = true;
            break;
        case KEY_AVRCP_FORWARD:
            USB_AUDIO_LOG_I("activity forward", 0);
            USB_Audio_HID_ScanNextTrack();
            ret = true;
            break;
        case KEY_AVRCP_PLAY:
        case KEY_AVRCP_PAUSE:
            USB_AUDIO_LOG_I("activity play pause", 0);
            USB_Audio_HID_PlayPause();
            ret = true;
            break;
        case KEY_VOICE_UP: {
            apps_config_state_t sta = apps_config_key_get_mmi_state();
            USB_AUDIO_LOG_I("activity volue up, sta=%d", 1, sta);
            USB_Audio_HID_VolumeUp(1);
            break;
        }
        case KEY_VOICE_DN: {
            apps_config_state_t sta = apps_config_key_get_mmi_state();
            USB_AUDIO_LOG_I("activity volue down, sta=%d", 1, sta);
            USB_Audio_HID_VolumeDown(1);
            break;
        }
        break;
    }

    return ret;
}
#endif

bool app_usb_audio_activity(
            struct _ui_shell_activity *self,
            uint32_t event_group,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        #ifdef AIR_USB_HID_MEDIA_CTRL_ENABLE
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
        #endif
        case EVENT_GROUP_SWITCH_USB_AUDIO: {
            if (event_id == APPS_EVENTS_INTERACTION_USB_AUDIO_TRANSMITTER) {
                audio_transmitter_event_t ev = ((uint32_t)extra_data) & 0xff;
                int8_t type = (((uint32_t)extra_data) >> 8) & 0xff;
                if (ev == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS && type <= 1) {
                    USB_AUDIO_LOG_I("finish activity due to AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS received", 0);
                    ui_shell_finish_activity(self, self);
                }
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_AMI_VENDOR: {
            vendor_se_event_t event = event_id;
            if (event == EVENT_RECORD_STOP) {
                USB_AUDIO_LOG_I("finish activity due to EVENT_RECORD_STOP received", 0);
                ui_shell_finish_activity(self, self);
            }
            break;
        }
        default:
            break;
    }
    return ret;
}


