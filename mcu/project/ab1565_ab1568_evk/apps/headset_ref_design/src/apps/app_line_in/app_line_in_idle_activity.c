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
 * File: app_line_in_idle_activity.c
 *
 * Description:
 * This file is the activity to process the play or stop of line-in. When line-in is plugin,
 * this activity will play line-in audio, and stop line-in audio when plugout.
 */

#include "app_line_in_utils.h"
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined (AIR_USB_AUDIO_OUT_ENABLE)
#include "app_usb_audio_idle_activity.h"
#endif

app_audio_path_t current_audio_path_value = APP_AUDIO_PATH_UNKNOWN;

static void line_in_sp_app_pull_request(uint8_t *current_audio_path);
static void line_in_sp_app_control_request(uint8_t new_audio_path, uint8_t *control_result);
static void line_in_sp_app_push_response(uint8_t status);

static bool line_in_with_rtc_pin = false;
static uint8_t s_line_in_type = 0;

#define APPS_EVENTS_INTERACTION_LINE_IN_PLUG_IN 1
#define APPS_EVENTS_INTERACTION_LINE_IN_PLUG_OUT 2

/*========================================================================================*/
/*                          APP INTERNAL FUNCTIONS                                        */
/*========================================================================================*/
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"
static app_power_saving_target_mode_t line_in_get_power_saving_target_mode(void)
{
    if (!line_in_with_rtc_pin) {
        hal_gpio_data_t current_gpio_status = 0;
        hal_gpio_get_input(LINE_IN_DETECT_PORT, &current_gpio_status);
        LINE_IN_LOG_I("power saving callback, gpio=%d",1,current_gpio_status);
        if (APP_AUDIO_PATH_LINE_IN == current_audio_path_value && LINE_IN_ALREADY_PLUG_IN == current_gpio_status) {
            return APP_POWER_SAVING_TARGET_MODE_NORMAL;
        } else {
            return APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
        }
    } else {
        bool rtc_level = HAL_RTC_GPIO_DATA_LOW;
        hal_rtc_gpio_get_input(LINE_IN_DETECT_PORT, &rtc_level);
        LINE_IN_LOG_I("power saving callback, rtc gpio=%d",1,rtc_level);
        if (rtc_level == LINE_IN_ALREADY_PLUG_IN) {
            return APP_POWER_SAVING_TARGET_MODE_NORMAL;
        } else {
            return APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
        }
    }
}
#endif

void app_list_in_broadcast_audio_path(app_audio_path_t path) {
    /* broadcast current audio path*/
    app_audio_path_t *a_path = (app_audio_path_t *)pvPortMalloc(sizeof(app_audio_path_t));
    if (a_path == NULL) {
        LINE_IN_LOG_I("malloc mem failed.", 0);
        return;
    }
    *a_path = path;
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH,
                        APPS_EVENTS_INTERACTION_SWITCH_AUDIO_PATH,
                        (void*)a_path, sizeof(app_audio_path_t), NULL, 0);
}

void app_line_in_switch(app_audio_path_t path)
{
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH,
                        APPS_EVENTS_INTERACTION_LINE_IN_CTRL,
                        (void*)path, 0, NULL, 0);
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
    if (path == APP_AUDIO_PATH_LINE_IN) {
        /* In order to avoid the system enter sleep state. */
        app_power_save_utils_notify_mode_changed(true, line_in_get_power_saving_target_mode);
    }
#endif
}

/**
 * @brief Send broadcast of line in is plugin or plugout.
 * Use the system UI shell to handle the audio path switch operation.
 *
 * @param       from_isr, indicates whether this function is called in the interrupt service function.
 * @param       plugin, line in is plug in or not.
 * @return      None.
 */
static void broadcast_line_in_plug_state(bool from_isr, bool plug_in)
{
    ui_shell_send_event(from_isr, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_LINE_IN_PLUG_STATE,
                        (void *)plug_in, 0, NULL, 0);
}

/**
 * @brief Store the current audio path into the NVDM.
 * NVDM group : common.
 * NVDM data item : audio_path.
 *
 * @return      None.
 */
static void store_current_audio_path()
{
    nvdm_status_t status = 0;
    uint8_t buf[2] = {0};
    uint32_t buf_len = 2;
    sprintf((char *)buf, "%d", current_audio_path_value);

    status = nvdm_write_data_item(NVDM_GROUP_COMMON, NVDM_COMMON_ITEM_AUDIO_PATH, NVDM_DATA_ITEM_TYPE_RAW_DATA, buf, buf_len);

    LINE_IN_LOG_I("Store audio path (%d) result : %d\n", 2, current_audio_path_value, status);
}

static void switch_audio_path(app_audio_path_t path, bool storage)
{
    LINE_IN_LOG_I("switch audio path: %d to %d\n", 2, current_audio_path_value, path);
    current_audio_path_value = path;
    if (storage) {
        store_current_audio_path();
    }
}

/**
 * @brief Init the current audio path according to the NVDM data and GPIO status.
 *
 * 1. Read configuration from NVDM.
 * 2. If read value is error (not exist), if the GPIO (line-in) is plugin, set current audio path to be line-in model.
 *    Otherwise, set to be BT.
 * 3. If read value is OK, set to be the saved value.
 */
static void init_audio_path()
{
    nvdm_status_t read_status = 0;
    uint8_t buffer[2] = {0};
    hal_gpio_data_t current_gpio_status = 0;
    bool rtc_level = false;
    app_audio_path_t temp_path = APP_AUDIO_PATH_UNKNOWN;

    uint32_t read_len = sizeof(buffer);

    /* Read the configuration from the NVDM which stored the last configured audio path. */
    read_status = nvdm_read_data_item(NVDM_GROUP_COMMON, NVDM_COMMON_ITEM_AUDIO_PATH, buffer, &read_len);

    LINE_IN_LOG_I("init_audio_path -> read common/audio_path from NVDM result : %d", 1, read_status);

    if (read_status != NVDM_STATUS_OK) {
        temp_path = APP_AUDIO_PATH_LINE_IN;
    } else {
        /* Set the current audio path to be the saved one. */
        LINE_IN_LOG_I("init_audio_path -> read length : %d, value : %d %d", 3, read_len, buffer[0], buffer[1]);
        temp_path = atoi((char *)buffer);
    }

    if (temp_path != APP_AUDIO_PATH_BT) {
        /**
         * If the stored configuration is NONE:
         * Check the current line-in GPIO value, if the line-in already plug-in, switch current audio path to be line-in mode.
         * Otherwise, switch to current audio path to be BT mode.
         */
        if (!line_in_with_rtc_pin) {
            hal_gpio_get_input(LINE_IN_DETECT_PORT, &current_gpio_status);
            if (current_gpio_status == LINE_IN_ALREADY_PLUG_IN) {
                temp_path = APP_AUDIO_PATH_LINE_IN;
            } else {
                temp_path = APP_AUDIO_PATH_BT;
            }
        } else {
            hal_rtc_gpio_get_input(LINE_IN_DETECT_PORT, &rtc_level);
            /* Base on hardware design, high level means line in pluged in. */
            if ((hal_rtc_gpio_data_t)rtc_level == (hal_rtc_gpio_data_t)LINE_IN_ALREADY_PLUG_IN) {
                temp_path = APP_AUDIO_PATH_LINE_IN;
            } else {
                temp_path = APP_AUDIO_PATH_BT;
            }
        }
        LINE_IN_LOG_I("read path from gpio : %d", 1, temp_path == APP_AUDIO_PATH_LINE_IN);
        broadcast_line_in_plug_state(false, temp_path == APP_AUDIO_PATH_LINE_IN ? true : false);
    }

    switch_audio_path(temp_path, false);
    app_line_in_switch(temp_path);

    LINE_IN_LOG_I("init_audio_path -> init current audio path to be : %d", 1, current_audio_path_value);
}

/**
 * @brief The line-in plug-in or plug-out callback function.
 *
 * @param user_data, the user data.
 *
 */
static void line_in_detect_callback(void *user_data)
{
    if (!line_in_with_rtc_pin) {
        hal_gpio_data_t current_gpio_status = 0;
        hal_eint_mask(LINE_IN_DETECT_EINT_NUM);
        hal_gpio_get_input(LINE_IN_DETECT_PORT, &current_gpio_status);
        LINE_IN_LOG_I("detect_callback -> detect callback with value : %d", 1, current_gpio_status);

        ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH,
                            current_gpio_status == LINE_IN_ALREADY_PLUG_IN ? APPS_EVENTS_INTERACTION_LINE_IN_PLUG_IN : APPS_EVENTS_INTERACTION_LINE_IN_PLUG_OUT,
                            NULL, 0, NULL, 0);

        hal_eint_unmask(LINE_IN_DETECT_EINT_NUM);
    } else {
        /* If the RTC pin used for line in, need debounce. */
        ui_shell_remove_event(EVENT_GROUP_SWITCH_AUDIO_PATH, APPS_EVENTS_INTERACTION_LINE_IN_RTC_PLUG_EV);
        ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH,
                            APPS_EVENTS_INTERACTION_LINE_IN_RTC_PLUG_EV,
                            NULL, 0, NULL, 300);
    }
}

/**
 * @brief Init the external interrupt to check the line-in plug-in or not.
 * @return      None.
 */
static void init_line_in_detect_interrupt()
{
    if (!line_in_with_rtc_pin) {
        hal_eint_config_t config;

        /* For falling and rising detect. */
        config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
        config.debounce_time = 300;

        hal_gpio_init(LINE_IN_DETECT_PORT);
        hal_eint_mask(LINE_IN_DETECT_EINT_NUM);
        if (hal_eint_init(LINE_IN_DETECT_EINT_NUM, &config) != HAL_EINT_STATUS_OK) {
            LINE_IN_LOG_E("ini_detect_interrupt -> init eint failed", 0);
            hal_eint_unmask(LINE_IN_DETECT_EINT_NUM);
            return;
        }

        if (hal_eint_register_callback(LINE_IN_DETECT_EINT_NUM, line_in_detect_callback, NULL) != HAL_EINT_STATUS_OK) {
            LINE_IN_LOG_E("ini_detect_interrupt -> eint register callback failed", 0);
            hal_eint_unmask(LINE_IN_DETECT_EINT_NUM);
            hal_eint_deinit(LINE_IN_DETECT_EINT_NUM);
            return;
        }

        hal_eint_unmask(LINE_IN_DETECT_EINT_NUM);
    }else {
        hal_rtc_gpio_config_t rtc_pin_config;
        hal_rtc_eint_config_t rtc_eint_config;
        bool rtc_level = false;

        rtc_pin_config.rtc_gpio = LINE_IN_DETECT_PORT;
        rtc_pin_config.is_analog = false;
        rtc_pin_config.is_input = true;
        rtc_pin_config.is_pull_up = true;
        rtc_pin_config.is_pull_down = false;
        hal_rtc_gpio_init(&rtc_pin_config);
        hal_rtc_gpio_get_input(LINE_IN_DETECT_PORT, &rtc_level);

        rtc_eint_config.is_enable_debounce = true;
        rtc_eint_config.rtc_gpio = LINE_IN_DETECT_PORT;
        rtc_eint_config.is_enable_rtc_eint = true;
        rtc_eint_config.is_falling_edge_active = rtc_level;
        hal_rtc_eint_init(&rtc_eint_config);

        hal_rtc_eint_register_callback(LINE_IN_DETECT_PORT, line_in_detect_callback, NULL);
    }
}

/*========================================================================================*/
/*                          SP APP RACE CMD HANDLER                                       */
/*========================================================================================*/
/**
 * @brief Handle the SP APP RACE CMD of the pull request.
 *
 * @param current_audio_path Output for current audio path.
 */
static void line_in_sp_app_pull_request(uint8_t *current_audio_path)
{
    if (current_audio_path == NULL) {
        LINE_IN_LOG_E("pull_request -> current audio path is NULL", 0);
        return;
    }
    if (current_audio_path_value == APP_AUDIO_PATH_UNKNOWN) {
        *current_audio_path = APP_AUDIO_PATH_BT;
        LINE_IN_LOG_E("pull_request -> current is unknown audio path, return BT audio path", 0);
        return;
    }
    *current_audio_path = current_audio_path_value;
}

/**
 * @brief Handle the SP APP RACE CMD of the control request.
 *
 * @param new_audio_path    The new audio path.
 * @param control_result    The set result.
 * @return      None.
 */
static void line_in_sp_app_control_request(uint8_t new_audio_path, uint8_t *control_result)
{
    app_audio_path_t path = (app_audio_path_t)new_audio_path;
    if (control_result == NULL) {
        LINE_IN_LOG_E("control_request -> control result pointer is NULL", 0);
        return;
    }
    if (new_audio_path != APP_AUDIO_PATH_BT && new_audio_path != APP_AUDIO_PATH_LINE_IN) {
        LINE_IN_LOG_E("control_request -> unknown new audio path : %d", 1, new_audio_path);
        return;
    }

    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH, APPS_EVENTS_INTERACTION_AUDIO_PATH_UI_CTRL,
                        (void*)path, 0, NULL, 0);
    *control_result = 0;
}

/**
 * @brief Handle the SP APP RACE CMD of the push response.
 *
 * @param status The push response result.
 * @return      None.
 */
static void line_in_sp_app_push_response(uint8_t status)
{
    LINE_IN_LOG_I("push_response -> push result : %d", 1, status);
}

/**
 * @brief Init the line-in callback which used to receive the RACE CMD data.
 *  ONLY be called when system bootup.
 * @return      None.
 */
static void init_line_in_sp_app_callback()
{
    line_in_app_callback_t callback;
    callback.control_request = line_in_sp_app_control_request;
    callback.pull_request = line_in_sp_app_pull_request;
    callback.push_response = line_in_sp_app_push_response;
    race_cmd_hostaudio_set_app_line_in_callback(&callback);
}

/*========================================================================================*/
/*                          APP LINE HANDLE FUNCTIONS                                     */
/*========================================================================================*/
/**
 * @brief Handle the system event from UI shell.
 *
 * @param event_id      Which event ID from the system.
 * @param extra_data    The extra data with the event ID.
 * @param data_len      The extra data length.
 * @return              If return true, the current event cannot be handled by the next activity.
 */
static bool app_line_in_handle_system_event(uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = true;
    uint32_t nvkey_size = sizeof(uint8_t);

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            LINE_IN_LOG_I("app_line_in_idle_activity CREATE!!!", 0);
#if defined(AIR_LINE_IN_MIX_ENABLE) || defined(AIR_LINE_OUT_ENABLE)
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
            app_line_in_out_audio_source_ctrl_init();
#endif
#endif
            /**
             * TODO
             * 1. read the configuration from NVDM.
             * 2. configure the GPIO to register the line-in HW operation.
             * 3. register the BT operation/race command.
             * 4. register the callback to the line-in playback middleware.
             * 5. read the line-in GPIO value.
             */
            if (LINE_IN_DETECT_PORT != 0xFF && LINE_IN_DETECT_EINT_NUM == 0xff) {
                /**
                 * If line in detect pin is a RTC pin, should call different API to operate GPIO.
                 *
                 * Notice!
                 * Base 65_evk default HW design, BSP_LINE_IN_SWITCH_PIN is required.
                 * Base on others HW design, BSP_LINE_IN_SWITCH_PIN may not required.
                 * For customer HW design, these logic must be update to check that whether the RTC pin was used for Line in!!!
                 */
                line_in_with_rtc_pin = true;
            }
            nvkey_status_t sta = nvkey_read_data((uint16_t)NVKEYID_APP_LINE_IN_SCENARIO_TYPE, &s_line_in_type, &nvkey_size);
#if !defined(AIR_LINE_IN_MIX_ENABLE) && !defined(AIR_LINE_OUT_ENABLE)
            /* If only line in enabled, force set type to common. */
            s_line_in_type = LINE_IN_SCENARIO_TYPE_COMMON;
#endif
            LINE_IN_LOG_I("get line in scenario type 0x%x, ret %d", 2, s_line_in_type, sta);
#ifdef AIR_LINE_IN_MIX_ENABLE
            if (s_line_in_type == LINE_IN_SCENARIO_TYPE_MIX) {
                app_line_in_mix_init();
            } else {
            }
#else
#endif
#ifdef AIR_LINE_OUT_ENABLE
            if (s_line_in_type == LINE_IN_SCENARIO_TYPE_MIX) {
                app_line_out_init();
            }
#endif
            init_line_in_detect_interrupt();
            init_line_in_sp_app_callback();
            init_audio_path();
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            app_power_save_utils_register_get_mode_callback(line_in_get_power_saving_target_mode);
#endif
            }
            break;

        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            LINE_IN_LOG_I("app_line_in_idle_activity RESUME !!!", 0);
            break;

        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            LINE_IN_LOG_I("app_line_in_idle_activity PAUSE !!!", 0);
            break;

        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            LINE_IN_LOG_I("app_line_in_idle_activity DESTROY !!!", 0);
            break;
    }
    return ret;
}

/**
 * @brief Handle the EINT key event.
 *
 * @param event_id      The event ID of the key.
 * @param extra_data    The extra data of the event.
 * @param data_len      The extra data length.
 * @return              If return true, the current key event cannot be handled by the next activity.
 */
static bool app_line_in_handle_key_event(uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    /**
     * TODO
     * Need to switch the line-in state according to the current status.
     * If current is line-in, switch to BT mode.
     * If current is BT, switch to line-in mode.
     * ATTENTION:
     * Need to notify SP application that mode has been changed.
     */
    uint16_t action_id = 0;
    app_audio_path_t path = APP_AUDIO_PATH_UNKNOWN;

    action_id = *(uint16_t *)extra_data;
    if (action_id == KEY_LINE_IN_SWITCH) {
        LINE_IN_LOG_I("key_event -> switch the audio path : (current %d)", 1, current_audio_path_value);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH, APPS_EVENTS_INTERACTION_AUDIO_PATH_UI_CTRL,
                        (void*)path, 0, NULL, 0);
        return true;
    } else if (action_id == KEY_MUTE_MIC) {
        LINE_IN_LOG_I("line out mute, start sta=%d", 1, app_line_out_is_open());
#ifdef AIR_LINE_OUT_ENABLE
        app_line_out_set_mute_unmute();
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
static bool app_line_in_handle_rotary_event(
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
#ifdef AIR_LINE_IN_MIX_ENABLE
            if (current_audio_path_value == APP_AUDIO_PATH_LINE_IN) {
                uint8_t volume = app_line_in_get_volume();
                if (KEY_VOICE_UP == key_action) {
                    if (volume + rotary_data < bt_sink_srv_ami_get_lineIN_max_volume_level()) {
                        volume += rotary_data;
                    } else {
                        volume = bt_sink_srv_ami_get_lineIN_max_volume_level();
                    }
                } else {
                    if (volume > rotary_data) {
                        volume -= rotary_data;
                    } else {
                        volume = 0;
                    }
                }
                LINE_IN_LOG_I("rotary change volume to %d", 1, volume);
                /* TO-DO: support line in change volume. */
                app_line_in_set_volume(volume);
            }
#endif
            break;
        }
        default:
            break;
    }
    return ret;
}
#endif

/**
 * @brief Handle the line in about event.
 *
 * @param event_id      The event ID of the key.
 * @param extra_data    The extra data of the event.
 * @param data_len      The extra data length.
 * @return              If return true, the current key event cannot be handled by the next activity.
 */
static bool app_line_in_handle_line_in_event(
        ui_shell_activity_t *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    switch (event_id) {
        /* Start or stop Line in. */
        case APPS_EVENTS_INTERACTION_LINE_IN_CTRL: {
            app_audio_path_t audio_path = (app_audio_path_t)extra_data;
#ifdef AIR_LINE_IN_MIX_ENABLE
            if (s_line_in_type == LINE_IN_SCENARIO_TYPE_MIX) {
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
                if (audio_path == APP_AUDIO_PATH_LINE_IN) {
                    app_line_in_request_source();
                } else {
                    app_line_in_mix_start_stop(audio_path);
                }
#else
                app_line_in_mix_start_stop(audio_path);
#endif
            } else {
                app_line_in_start_stop(audio_path);
            }
#else
#ifdef APPS_LINE_IN_SUPPORT
            app_line_in_start_stop(audio_path);
#endif
#endif
#ifdef AIR_LINE_OUT_ENABLE
            if (s_line_in_type == LINE_IN_SCENARIO_TYPE_MIX) {
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
                if (audio_path == APP_AUDIO_PATH_LINE_IN) {
                    app_line_out_request_source();
                } else {
                    app_line_out_start_stop(audio_path);
                }
#else
                app_line_out_start_stop(audio_path);
#endif
            }
#endif
            app_list_in_broadcast_audio_path(audio_path);
        }
        break;
        /* Line in pluged in or pluged out. */
        case APPS_EVENTS_INTERACTION_LINE_IN_PLUG_IN:
        case APPS_EVENTS_INTERACTION_LINE_IN_PLUG_OUT:{
            /* JACK sta: line in plug in. */
            int8_t jack_sta = event_id == APPS_EVENTS_INTERACTION_LINE_IN_PLUG_IN ? 1 : 0;
            app_audio_path_t path = event_id == APPS_EVENTS_INTERACTION_LINE_IN_PLUG_IN ? APP_AUDIO_PATH_LINE_IN : APP_AUDIO_PATH_BT;
            broadcast_line_in_plug_state(false, jack_sta == 1 ? true : false);
            switch_audio_path(path, false);
            app_race_send_notify(APPS_RACE_CMD_CONFIG_TYPE_LINE_IN_JACK_STA, &jack_sta, sizeof(uint8_t));
            app_line_in_switch(path);
#ifdef AIR_LINE_OUT_ENABLE
            app_line_out_set_unmute();
#endif
        }
        break;
        /* Switch audio path through key pad or phone's app. */
        case APPS_EVENTS_INTERACTION_AUDIO_PATH_UI_CTRL: {
            app_audio_path_t new_path = (app_audio_path_t)extra_data;
            LINE_IN_LOG_I("audio path ctrl: %d", 1, new_path);
            if (new_path == APP_AUDIO_PATH_UNKNOWN) {
                new_path = current_audio_path_value == APP_AUDIO_PATH_BT ? APP_AUDIO_PATH_LINE_IN : APP_AUDIO_PATH_BT;
            }

            switch_audio_path(new_path, true);
            app_line_in_switch(new_path);
        }
        break;
        case APPS_EVENTS_INTERACTION_AM_EVENT: {
            vendor_se_event_t am_event = (vendor_se_event_t)extra_data;
            app_line_in_handle_am_event(am_event);
        }
        break;
        case APPS_EVENTS_INTERACTION_LINE_IN_RTC_PLUG_EV: {
            bool rtc_level = 0;
            hal_rtc_eint_config_t rtc_eint_config;
            rtc_eint_config.is_enable_debounce = true;
            rtc_eint_config.rtc_gpio = LINE_IN_DETECT_PORT;
            rtc_eint_config.is_enable_rtc_eint = true;

            hal_rtc_gpio_get_input(LINE_IN_DETECT_PORT, &rtc_level);
            rtc_eint_config.is_falling_edge_active = rtc_level; //changer trigger edge
            hal_rtc_eint_init(&rtc_eint_config);
            hal_rtc_eint_register_callback(LINE_IN_DETECT_PORT, line_in_detect_callback, NULL);

            LINE_IN_LOG_I("get line in sta(RTC): %d", 1, rtc_level);
            ui_shell_send_event(true, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_SWITCH_AUDIO_PATH,
                            rtc_level == LINE_IN_ALREADY_PLUG_IN ? APPS_EVENTS_INTERACTION_LINE_IN_PLUG_IN : APPS_EVENTS_INTERACTION_LINE_IN_PLUG_OUT,
                            NULL, 0, NULL, 0);
        }
        break;
#if defined(AIR_LINE_IN_MIX_ENABLE) || defined(AIR_LINE_OUT_ENABLE)
        case APPS_EVENTS_INTERACTION_LINE_IN_TRANSMITTER: {
            uint16_t shell_event = (uint16_t)(uint32_t)extra_data;
            app_line_in_transmitter_event(shell_event);
            break;
        }
#endif
#if defined(AIR_LINE_IN_MIX_ENABLE) || defined(AIR_LINE_OUT_ENABLE)
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        case APPS_EVENTS_INTERACTION_AUDIO_SRC_CTRL_EV: {
            uint32_t shell_event = (uint32_t)extra_data;
            app_line_in_audio_source_event(shell_event);
            break;
        }
#endif
#endif
        break;
    }
    return false;
}

/*========================================================================================*/
/*                          IDLE ACTIVITY PRO FUNCTION                                    */
/*========================================================================================*/
bool app_line_in_idle_activity_proc(struct _ui_shell_activity *self,
                                    uint32_t event_group,
                                    uint32_t event_id,
                                    void *extra_data,
                                    size_t data_len)
{
    bool ret = false;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            /* ui_shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = app_line_in_handle_system_event(event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_KEY:
            /* key event. */
            ret = app_line_in_handle_key_event(event_id, extra_data, data_len);
            break;
#ifdef AIR_ROTARY_ENCODER_ENABLE
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER:
            /**< group for rotary encoder events */
            ret = app_line_in_handle_rotary_event(self, event_id, extra_data, data_len);
            break;
#endif
        case EVENT_GROUP_SWITCH_AUDIO_PATH: {
            ret = app_line_in_handle_line_in_event(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            /* interaction events. */
            if (event_id == APPS_EVENTS_INTERACTION_POWER_OFF) {
                /* Close line-in audio before system power off. */
                current_audio_path_value = APP_AUDIO_PATH_BT;
                app_line_in_switch(APP_AUDIO_PATH_BT);
            }
            break;
        case EVENT_GROUP_UI_SHELL_BT_SINK:
            if (s_line_in_type == LINE_IN_SCENARIO_TYPE_MIX) {
                app_line_in_handle_sink_event(event_id, extra_data, data_len);
            }
            break;
#if defined(AIR_USB_AUDIO_MIX_ENABLE) || defined (AIR_USB_AUDIO_OUT_ENABLE)
        case EVENT_GROUP_SWITCH_USB_AUDIO:
            if (event_id == APPS_EVENTS_INTERACTION_USB_AUDIO_TRANSMITTER) {
                uint32_t event = (uint32_t) extra_data;
                audio_transmitter_event_t ev = event & 0xff;
                LINE_IN_LOG_I("try to resume line in after USB in stopped: %d", 1, event);
                if (ev == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS && (ev & 0xff00) <= 0x200) {
                    if (current_audio_path_value == APP_AUDIO_PATH_LINE_IN) {
                        app_line_in_switch(APP_AUDIO_PATH_LINE_IN);
                    }
                }
            }
            break;
#endif
        case EVENT_GROUP_UI_SHELL_AMI_VENDOR: {
            vendor_se_event_t am_event = event_id;
            app_line_in_handle_am_event(am_event);
            break;
        }
    }

    return ret;
}

app_audio_path_t app_line_in_activity_get_current_audio_path()
{
    uint32_t mask;
    app_audio_path_t cur_path;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    cur_path = current_audio_path_value;
    hal_nvic_restore_interrupt_mask(mask);

    return cur_path;
}

bool app_line_in_is_plug_in()
{
    bool plug_in = false;

    if (!line_in_with_rtc_pin) {
        hal_gpio_data_t current_gpio_status = 0;
        hal_gpio_get_input(LINE_IN_DETECT_PORT, &current_gpio_status);
        plug_in = current_gpio_status == LINE_IN_ALREADY_PLUG_IN;
    } else {
        bool rtc_level = false;
        hal_rtc_gpio_get_input(LINE_IN_DETECT_PORT, &rtc_level);
        plug_in = ((hal_rtc_gpio_data_t)rtc_level == (hal_rtc_gpio_data_t)LINE_IN_ALREADY_PLUG_IN);
    }

    LINE_IN_LOG_I("get line in plug sta: %d", 1, plug_in);
    return plug_in;
}

