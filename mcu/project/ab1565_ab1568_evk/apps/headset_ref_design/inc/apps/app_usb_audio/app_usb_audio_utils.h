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
 * File: app_usb_audio_utils.h
 *
 * Description: This file defines the common structure and functions of usb audio app.
 */


#ifndef __APP_USB_AUDIO_UTILS_H__
#define __APP_USB_AUDIO_UTILS_H__

#ifdef APPS_USB_AUDIO_SUPPORT

#include "apps_events_event_group.h"
#include "ui_shell_manager.h"
#include "ui_shell_activity.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_config_event_list.h"
#include "apps_debug.h"
#include "race_cmd_hostaudio.h"
#include "usb_audio_playback.h"
#include "usb_audio_control.h"
#include "apps_config_key_remapper.h"

#define USB_AUDIO_LOG_I(msg, ...)     APPS_LOG_MSGID_I("[USB_AUDIO_MMI]"msg, ##__VA_ARGS__)
#define USB_AUDIO_LOG_E(msg, ...)     APPS_LOG_MSGID_E("[USB_AUDIO_MMI]"msg, ##__VA_ARGS__)
#define USB_AUDIO_LOG_D(msg, ...)     APPS_LOG_MSGID_D("[USB_AUDIO_MMI]"msg, ##__VA_ARGS__)


typedef enum {
    USB_AUDIO_ACTION_OPEN,
    USB_AUDIO_ACTION_CLOSE,
    USB_AUDIO_ACTION_UNKNOWN
} usb_audio_action_t;

#define USB_AUDIO_IN_PORT1  (1 << 0)
#define USB_AUDIO_OUT_PORT1 (1 << 1)
#define USB_AUDIO_IN_PORT2  (1 << 2)
typedef uint8_t usb_audio_port_t;

#define USB_AUDIO_SCENARIO_TYPE_COMMON_USB_IN 1
#define USB_AUDIO_SCENARIO_TYPE_MIX_OUT 0

#define USB_AUDIO_VOLUME_CHANNEL_L 0x1
#define USB_AUDIO_VOLUME_CHANNEL_R 0x2
#define USB_AUDIO_VOLUME_CHANNEL_ALL 0xFF

#define ULL_MIX_RATIO_GAME_MAX_LEVEL    (0)     /* Gaming is 100%, Chat is 0% */
#define ULL_MIX_RATIO_CHAT_MAX_LEVEL    (20)    /* Gaming is 0%, Chat is 100% */
#define ULL_MIX_RATIO_BALANCED_LEVEL    (10)    /* Gaming is 100%, Chat is 100% */

/**
* @brief      This function is used to get the working scenario type.
* @return     current scenaro type.
*/
uint8_t app_usb_audio_scenario_type(void);

/**
* @brief      This function is used to start or stop usb audio.
* @param[in]  action, the action need to send.
* @param[in]  delay_ms, the delay time to sending the action.
* @return     None
*/
void app_usb_audio_action(usb_audio_port_t port, usb_audio_action_t action, uint32_t delay_ms);

/**
* @brief      This function is used to resume usb audio after line in.
* @return     None
*/
void app_usb_audio_resume_after_line_in_common();

/**
* @brief      This function is used to listen the HFP's event and try to resume the usb audio.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handled by the next activity.
*/
bool app_usb_audio_bt_state_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len);

/**
* @brief      This function is used to listen the HFP's event and try to resume the usb audio.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handled by the next activity.
*/
bool app_usb_audio_handle_sink_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len);

/**
* @brief      This function is used to handle the event of usb audio.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handled by the next activity.
*/
bool app_usb_audio_event(ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);

/**
* @brief      This function is used to handle the event of EVENT_GROUP_UI_SHELL_AMI_VENDOR.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handled by the next activity.
*/
bool app_audio_vendor_event(ui_shell_activity_t *self,
                            uint32_t event_id,
                            void *extra_data,
                            size_t data_len);

/**
* @brief      This function is used to init the usb audio mix
* @return     None
*/
void app_usb_audio_mix_init(void);

/**
* @brief      This function is used to init the usb out
* @return     None
*/
void app_usb_out_init(void);

/**
* @brief      This function is used to resume transmitter after line in stopped.
* @return     None
*/
void app_usb_resume_after_line_in(void);

/**
* @brief      This function is used to set the current volume
* @return     None
*/
void app_usb_audio_set_volume(usb_audio_port_t port, uint8_t channel, uint8_t volume, bool mute);

/**
* @brief      This function is used to set the sample rate
* @return     None
*/
void app_usb_audio_set_sampling_rate(usb_audio_port_t port, uint32_t rate);

/**
* @brief      This function is used to update the mix rotatry
* @param[in]  mix_arg, the range is [0,20].
* @return     None
*/
void app_usb_audio_mix_rotatry_change(uint8_t mix_arg);

/**
* @brief      This function is used to update the mix rotatry
* @param[in]  game_ratio, the ratio of game rx, range is [0, 100].
* @param[in]  chart_ratio, the ratio of chart rx, range is [0, 100].
* @return     None
*/
void app_usb_audio_set_mix_ratio(uint8_t game_ratio, uint8_t chart_ratio);

/**
* @brief      This function is used to send usb audio out data.
* @return     None
*/
void usb_audio_tx_ready(void);

/**
* @brief      This function is used to send usb audio out data.
* @return     None
*/
void app_usb_init_am(void);

/**
* @brief      This function is used to set line in running state
* @param[in]  running, the running state of line in.
* @return     None
*/
void app_usb_audio_set_line_in_running_state(bool running);

/**
* @brief      This function is used to get line in running state
* @return     True means line in is running.
*/
bool app_usb_audio_get_line_in_running_state(void);

void app_usb_audio_source_ctrl_init();

#endif /* APPS_USB_AUDIO_SUPPORT */
#endif /* __APP_USB_AUDIO_UTILS_H__ */
