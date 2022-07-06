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
 * File: app_usb_audio_idle_activity.c
 *
 * Description:
 * This file is the activity to process the play or stop of USB audio. This app accepts
 * events from the USB host to play or stop USB audio, set/mute/unmute the volume of USB
 * audio. It will also resume USB audio after HFP ends.
 */


#ifdef APPS_USB_AUDIO_SUPPORT

#include "app_usb_audio_utils.h"
#include "usbaudio_drv.h"
#ifdef APPS_LINE_IN_SUPPORT
#include "app_line_in_idle_activity.h"
#endif
#include "apps_events_key_event.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "hal_pinmux_define.h"

#define USB_AUDIO_SPEAKER_INTERFACE 1
#define USB_AUDIO_STOPED_ALTERNATE 0
#define USB_AUDIO_STARTED_ALTERNATE 1

static volatile bool s_usb_plugged_in = false;

static void report_usb_plug_status(bool plug_in) {
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_USB_PLUG_STATE,
                        (void *)plug_in, 0, NULL,0);
}

uint8_t app_usb_audio_scenario_type(void) {
#if !defined(AIR_USB_AUDIO_MIX_ENABLE) && !defined(AIR_USB_AUDIO_OUT_ENABLE)
    return USB_AUDIO_SCENARIO_TYPE_COMMON_USB_IN;
#else
    static uint8_t s_usb_scenario_type = 0xFF;
    if (s_usb_scenario_type == 0xFF) {
        uint32_t nvkey_size = sizeof(uint8_t);
        nvkey_status_t sta = nvkey_read_data((uint16_t)NVKEYID_APP_USB_AUDIO_SCENARIO_TYPE, &s_usb_scenario_type, &nvkey_size);
        if (sta == NVKEY_STATUS_ITEM_NOT_FOUND) {
            s_usb_scenario_type = USB_AUDIO_SCENARIO_TYPE_MIX_OUT;
        }
        USB_AUDIO_LOG_I("get usb audio scenario type 0x%x, ret %d", 2, s_usb_scenario_type, sta);
    }

    return s_usb_scenario_type;
#endif
}

/*========================================================================================*/
/*                               AT COMMOND SUPPORT                                       */
/*========================================================================================*/
#include "atci.h"
#include "stdlib.h"
#include "string.h"
static atci_status_t app_usb_audio_atci_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *param = NULL;
    uint32_t p1 = 0, p2 = 0;

    param = parse_cmd->string_ptr + parse_cmd->name_len + 1;
    param = strtok(param, ",");

    p1 = atoi(param);
    param = strtok(NULL, ",");
    p2 = atoi(param);
    USB_AUDIO_LOG_I("at commnd: %d,%d", 2, p1, p2);
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
    app_usb_audio_set_mix_ratio(p1, p2);
#endif

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_cmd_hdlr_item_t app_usb_audio_atci_cmd[] = {
    {
        .command_head = "AT+USBAUDIORATIO",
        .command_hdlr = app_usb_audio_atci_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

static void app_usb_audio_atci_init() {
    atci_status_t ret;

    ret = atci_register_handler(app_usb_audio_atci_cmd, sizeof(app_usb_audio_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));
    USB_AUDIO_LOG_I("atci register result %d", 1, ret);
}

/*========================================================================================*/
/*                          USB Audio evet callback                                       */
/*========================================================================================*/

/**
 * @brief USB interface callback
 *
 * @param interface_number      1 means speaker interface.
 *                              2 means microphone interface.
 * @param alternate_set         0 means the audio stream stoped.
 *                              1 means the audio stream started.
 * @return
 */
static void usb_audio_setinterface_cb(uint8_t interface_number, uint8_t alternate_set)
{
    usb_audio_action_t action = USB_AUDIO_ACTION_UNKNOWN;
    usb_audio_port_t port = interface_number;

    USB_AUDIO_LOG_I("USB interface callback, i:%d, s: %d", 2, interface_number, alternate_set);
    if (!s_usb_plugged_in) {
        s_usb_plugged_in = true;
        report_usb_plug_status(true);
    }

    if (alternate_set == USB_AUDIO_STOPED_ALTERNATE) {
        action = USB_AUDIO_ACTION_CLOSE;
    } else if (alternate_set == USB_AUDIO_STARTED_ALTERNATE) {
        action = USB_AUDIO_ACTION_OPEN;
    }

    if (action == USB_AUDIO_ACTION_UNKNOWN) {
        return;
    }

    app_usb_audio_action(port, action, 0);
}

/**
 * @brief   Callback when USB unplugged.
 *
 * @return  None.
 */
static void usb_audio_unplug_cb(void)
{
    USB_AUDIO_LOG_I("USB unpluged", 0);
    if (s_usb_plugged_in) {
        usb_audio_port_t port = USB_AUDIO_IN_PORT1;
        if (app_usb_audio_scenario_type() == USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
#ifdef AIR_USB_IN_PORT2_ENABLE
            port |= USB_AUDIO_IN_PORT2;
#endif
#ifdef AIR_USB_AUDIO_OUT_ENABLE
            port |= USB_AUDIO_OUT_PORT1;
#endif
        }

        app_usb_audio_action(port, USB_AUDIO_ACTION_CLOSE, 0);
        report_usb_plug_status(false);
    }
    s_usb_plugged_in = false;
}

static void volumechange_cb(uint8_t ep_number, uint8_t channel, uint32_t _volume)
{
    uint8_t volume = (uint8_t)(_volume & 0xFF);
    USB_AUDIO_LOG_I("usb volumechange e:%d, c:%d, v:%d", 3, ep_number, channel, volume);
    if (app_usb_audio_scenario_type() != USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
        audio_usb_audio_control_set_volume(volume);
        return;
    }
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
    usb_audio_port_t port = USB_AUDIO_IN_PORT1;
    switch (ep_number) {
        case 0x01:
            port = USB_AUDIO_IN_PORT1;
            break;
        case 0x02:
            port = USB_AUDIO_IN_PORT2;
            break;
        case 0x81:
            port = USB_AUDIO_OUT_PORT1;
            break;
    }
    app_usb_audio_set_volume(port, channel, volume, false);
#endif
}

static void mute_cb(uint8_t ep_number, usb_audio_mute_t mute)
{
    USB_AUDIO_LOG_I("usb volumechange e:%d, c:%d", 2, ep_number, mute);
    if (app_usb_audio_scenario_type() != USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
        if (mute != USB_AUDIO_MUTE_OFF) {
            audio_usb_audio_control_mute();
        } else {
            audio_usb_audio_control_unmute();
        }
        return;
    }

#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
    usb_audio_port_t port = USB_AUDIO_IN_PORT1;
    switch (ep_number) {
        case 0x01:
            port = USB_AUDIO_IN_PORT1;
            break;
        case 0x02:
            port = USB_AUDIO_IN_PORT2;
            break;
        case 0x81:
            port = USB_AUDIO_OUT_PORT1;
            break;
    }
    app_usb_audio_set_volume(port, USB_AUDIO_VOLUME_CHANNEL_ALL, 0, mute == USB_AUDIO_MUTE_ON);
#endif
}

#ifdef AIR_USB_AUDIO_OUT_ENABLE
static void usb_audio_sample_rate_cb(uint8_t ep_number, uint32_t sample_rate)
{
    usb_audio_port_t port = USB_AUDIO_OUT_PORT1;
    USB_AUDIO_LOG_I("usb_audio_sample_rate_cb, ep:0x%x, sample_rate:%d", 2, ep_number, sample_rate);

    uint8_t chat_if = 0xFF, gaming_if = 0xFF, mic_if = 0xFF;
    uint8_t chat_ep = 0xFF, gaming_ep = 0xFF, mic_ep = 0xFF;
    USB_Audio_Get_Game_Info(&gaming_if, &gaming_ep);
    USB_Audio_Get_Chat_Info(&chat_if, &chat_ep, &mic_if, &mic_ep);
    if ((0xFF != gaming_ep) || (0xFF != chat_ep) || (0xFF != mic_ep)) {
        if (gaming_ep == ep_number) {
            USB_AUDIO_LOG_I("usb_audio_sample_rate_cb, gaming sampling_rate=%d", 1, sample_rate);
            port = USB_AUDIO_IN_PORT1;
        } else if (chat_ep == ep_number) {
            USB_AUDIO_LOG_I("usb_audio_sample_rate_cb, chat sampling_rate=%d", 1, sample_rate);
            port = USB_AUDIO_IN_PORT2;
        } else if (mic_ep == ep_number) {
            USB_AUDIO_LOG_I("usb_audio_sample_rate_cb, mic sampling_rate=%d", 1, sample_rate);
            port = USB_AUDIO_OUT_PORT1;
        } else {
            USB_AUDIO_LOG_I("usb_audio_sample_rate_cb, unknown type", 0);
            return;
        }
    }
    app_usb_audio_set_sampling_rate(port, sample_rate);
}
#endif

/*========================================================================================*/
/*                          APP LINE HANDLE FUNCTIONS                                     */
/*========================================================================================*/

static void app_usb_audio_init()
{
    /* register default usb audio in */
    USB_Audio_Register_SetInterface_Callback(0, usb_audio_setinterface_cb);
    USB_Audio_Register_Unplug_Callback(0, usb_audio_unplug_cb);
    USB_Audio_Register_VolumeChange_Callback(0, volumechange_cb);
    USB_Audio_Register_Mute_Callback(0, mute_cb);

#ifdef AIR_USB_IN_PORT2_ENABLE
    /* register the second usb audio in. */
    if (app_usb_audio_scenario_type() == USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
        USB_Audio_Register_SetInterface_Callback(1, usb_audio_setinterface_cb);
        USB_Audio_Register_Unplug_Callback(1, usb_audio_unplug_cb);
        USB_Audio_Register_VolumeChange_Callback(1, volumechange_cb);
        USB_Audio_Register_Mute_Callback(1, mute_cb);
    }
#endif

#ifdef AIR_USB_AUDIO_OUT_ENABLE
    if (app_usb_audio_scenario_type() == USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
        USB_Audio_Register_Tx_Callback(0, usb_audio_tx_ready);
        USB_Audio_Register_Mic_SetInterface_Callback(0, usb_audio_setinterface_cb);
        USB_Audio_Register_Mic_SetSamplingRate_Callback(0, usb_audio_sample_rate_cb);
        USB_Audio_Register_Mic_VolumeChange_Callback(0, volumechange_cb);
        USB_Audio_Register_Mic_Mute_Callback(0, mute_cb);
    }
#endif
}

/**
 * @brief Handle the system event from UI shell.
 *
 * @param event_id      Which event ID from the system.
 * @param extra_data    The extra data with the event ID.
 * @param data_len      The extra data length.
 * @return              If return true, the current event cannot be handled by the next activity.
 */
static bool app_usb_audio_handle_system_event(ui_shell_activity_t *self,
                                              uint32_t event_id,
                                              void *extra_data,
                                              size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
            app_usb_audio_source_ctrl_init();
#endif
#endif
            app_usb_audio_init();
            if (app_usb_audio_scenario_type() == USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
#ifdef AIR_USB_AUDIO_MIX_ENABLE
                app_usb_audio_mix_init();
#endif
#ifdef AIR_USB_AUDIO_OUT_ENABLE
                app_usb_out_init();
#endif
            }

            app_usb_audio_atci_init();
            USB_AUDIO_LOG_I("app_usb_audio_idle_activity CREATE.", 0);
            break;
        }
        default:
            break;
    }
    return true;
}

static bool _proc_apps_internal_events(struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    switch (event_id) {
        case APPS_EVENTS_INTERACTION_POWER_OFF: {
            /* Stop USB audio before system power off. */
            USB_AUDIO_LOG_I("USB Audio stop when power off.", 0);
            usb_audio_port_t port = USB_AUDIO_IN_PORT1;
            if (app_usb_audio_scenario_type() == USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
#ifdef AIR_USB_IN_PORT2_ENABLE
                port |= USB_AUDIO_IN_PORT2;
#endif
#ifdef AIR_USB_AUDIO_OUT_ENABLE
                port |= USB_AUDIO_OUT_PORT1;
#endif
            }
            app_usb_audio_action(port, USB_AUDIO_ACTION_CLOSE, 0);
        }
        break;
#ifdef AIR_USB_HID_MEDIA_CTRL_ENABLE
#ifdef GSOUND_LIBRARY_ENABLE
        case APPS_EVENTS_INTERACTION_GSOUND_ACTION_REJECTED: {
            apps_config_state_t sta = apps_config_key_get_mmi_state();
            bool line_in_running = app_usb_audio_get_line_in_running_state();
            USB_AUDIO_LOG_I("USB Audio process play pause due to gsound rejected, line_in_running=%d, mmi_sta=%d", 2, line_in_running, sta);
            if (sta <= APP_CONNECTABLE && !line_in_running) {
                USB_Audio_HID_PlayPause();
            }
        }
        break;
#endif
#endif
    }
    return false;
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
/**
 * @brief Handle the rotary encoder event.
 *
 * @param event_id      The event ID of the key.
 * @param extra_data    The extra data of the event.
 * @param data_len      The extra data length.
 * @return              If return true, the current key event cannot be handled by the next activity.
 */
static bool app_usb_handle_rotary_event(
        ui_shell_activity_t *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
    static uint8_t s_ratio = ULL_MIX_RATIO_BALANCED_LEVEL;
#endif
    bsp_rotary_encoder_port_t port;
    bsp_rotary_encoder_event_t event;
    uint32_t rotary_data;
    if (!extra_data) {
        return ret;
    }
    apps_config_key_action_t key_action = *(uint16_t *)extra_data;
    app_event_rotary_event_decode(&port, &event, &rotary_data, event_id);

    switch (key_action) {
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
        case KEY_AUDIO_MIX_RATIO_GAME_ADD: {
            if (ULL_MIX_RATIO_GAME_MAX_LEVEL + rotary_data <= s_ratio) {
                s_ratio -= rotary_data;
                app_usb_audio_mix_rotatry_change(s_ratio);
            }
            break;
        }
        case KEY_AUDIO_MIX_RATIO_CHAT_ADD: {
            if (s_ratio + rotary_data <= ULL_MIX_RATIO_CHAT_MAX_LEVEL) {
                s_ratio += rotary_data;
                app_usb_audio_mix_rotatry_change(s_ratio);
            }
            break;
        }
#endif
        case KEY_VOICE_UP: {
#ifdef MTK_USB_AUDIO_HID_ENABLE
            USB_Audio_HID_VolumeUp(rotary_data);
#endif
            USB_AUDIO_LOG_I("rotary volume up:%d", 1, rotary_data);
            break;
        }

        case KEY_VOICE_DN: {
#ifdef MTK_USB_AUDIO_HID_ENABLE
            USB_Audio_HID_VolumeDown(rotary_data);
#endif
            USB_AUDIO_LOG_I("rotary volume dn:%d", 1, rotary_data);
            break;
        }
        default:
            break;
    }
    return ret;
}
#endif

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

    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }

    switch(action) {
        case KEY_AVRCP_PLAY:
        case KEY_AVRCP_PAUSE: {
            apps_config_state_t sta = apps_config_key_get_mmi_state();
            bool line_in_running = app_usb_audio_get_line_in_running_state();
            USB_AUDIO_LOG_I("idle activity play pause, mmi_sta = %d, line_in_running=%d", 2, sta, line_in_running);
            if (sta <= APP_CONNECTABLE && !line_in_running) {
                USB_Audio_HID_PlayPause();
                ret = true;
            }
            break;
        }
    }

    return ret;
}
#endif

/*========================================================================================*/
/*                          IDLE ACTIVITY PRO FUNCTION                                    */
/*========================================================================================*/
bool app_usb_audio_idle_activity_proc(struct _ui_shell_activity *self,
                                      uint32_t event_group,
                                      uint32_t event_id,
                                      void *extra_data,
                                      size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            /* ui_shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = app_usb_audio_handle_system_event(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            /* interaction events. */
            ret = _proc_apps_internal_events(self, event_id, extra_data, data_len);
            break;
#ifdef AIR_USB_HID_MEDIA_CTRL_ENABLE
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_BT_SINK:
            /* BT_SINK events, indicates the HFP state. */
            if (app_usb_audio_scenario_type() == USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
#ifdef AIR_USB_AUDIO_OUT_ENABLE
                ret = app_usb_audio_handle_sink_event(self, event_id, extra_data, data_len);
#endif
            } else {
                ret = app_usb_audio_bt_state_event(self, event_id, extra_data, data_len);
            }
            break;

#ifdef AIR_ROTARY_ENCODER_ENABLE
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER:
            /**< group for rotary encoder events */
            ret = app_usb_handle_rotary_event(self, event_id, extra_data, data_len);
            break;
#endif
#ifdef APPS_LINE_IN_SUPPORT
        case EVENT_GROUP_SWITCH_AUDIO_PATH:
            /* Resume usb audio after line in stoped. */
            if (event_id == APPS_EVENTS_INTERACTION_SWITCH_AUDIO_PATH) {
                USB_AUDIO_LOG_I("receive audio path switch ev, event_id: 0x%x, value: 0x%x", 2, event_id, (uint32_t)extra_data);
                app_audio_path_t *a_path = (app_audio_path_t *)extra_data;
                if (*a_path != APP_AUDIO_PATH_LINE_IN) {
                    app_usb_audio_set_line_in_running_state(false);
                    if (app_usb_audio_scenario_type() != USB_AUDIO_SCENARIO_TYPE_MIX_OUT) {
                        app_usb_audio_resume_after_line_in_common();
                    }
                } else if (*a_path == APP_AUDIO_PATH_LINE_IN) {
                    app_usb_audio_set_line_in_running_state(true);
                }
            } else if (event_id == APPS_EVENTS_INTERACTION_LINE_IN_TRANSMITTER) {
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined(AIR_USB_AUDIO_OUT_ENABLE)
                USB_AUDIO_LOG_I("receive audio path switch ev, event_id: 0x%x, value: 0x%x", 2, event_id, (uint32_t)extra_data);
                uint32_t event = (uint32_t) extra_data;
                audio_transmitter_event_t ev = event & 0xff;
                /* Line in stop success -> Line out stop success. */
                if (ev == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS) {
                    app_usb_audio_set_line_in_running_state(false);
                    app_usb_resume_after_line_in();
                } else if (ev == AUDIO_TRANSMITTER_EVENT_START_SUCCESS) {
                    app_usb_audio_set_line_in_running_state(true);
                }
#endif
            }
            break;
#endif
        case EVENT_GROUP_SWITCH_USB_AUDIO:
            ret = app_usb_audio_event(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_AMI_VENDOR: {
            app_audio_vendor_event(self, event_id, extra_data, data_len);
            break;
        }
        default:
            break;
    }

    return ret;
}

#endif /* APPS_USB_AUDIO_SUPPORT */

