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
 * File: app_line_in_idle_activity.h
 *
 * Description: This file defines the interface of app_line_in_idle_activity.c.
 */


#ifndef __APP_LINE_IN_IDLE_ACTIVITY_H__
#define __APP_LINE_IN_IDLE_ACTIVITY_H__

#if defined(APPS_LINE_IN_SUPPORT) || defined(AIR_LINE_IN_MIX_ENABLE) || defined(AIR_LINE_OUT_ENABLE)

#include "ui_shell_activity.h"

/**
 *  @brief This enumeration defines the audio path.
 */
typedef enum {

    APP_AUDIO_PATH_LINE_IN              = 0x01, /**<  The current audio path is line-in. */
    APP_AUDIO_PATH_BT                   = 0x00, /**<  The line-in audio is not in playing, the default audio path is BT. */
    APP_AUDIO_PATH_UNKNOWN              = 0xFF, /**<  The current audio path is unknown. */

} app_audio_path_t;

enum {
    APPS_EVENTS_INTERACTION_LINE_IN_PLUG_IN         = 0x01, /**<  Indicate that the line in jack inserted. */
    APPS_EVENTS_INTERACTION_LINE_IN_PLUG_OUT        = 0x02, /**<  Indicate that the line in jack uninserted. */
    APPS_EVENTS_INTERACTION_AM_EVENT                = 0x03, /**<  Indicate the state of audio manager updated. */
    APPS_EVENTS_INTERACTION_AUDIO_PATH_UI_CTRL      = 0x04, /**<  The audio path switched by UI. */
    APPS_EVENTS_INTERACTION_LINE_IN_CTRL            = 0x05, /**<  Start or stop the line in function. */
    APPS_EVENTS_INTERACTION_LINE_IN_TRANSMITTER     = 0x06, /**<  Indicate the transmitter event. */
    APPS_EVENTS_INTERACTION_LINE_IN_RTC_PLUG_EV     = 0x07, /**<  Indicate that the line in jack inserted or uninserted. */
    APPS_EVENTS_INTERACTION_AUDIO_SRC_CTRL_EV       = 0x08, /**<  Indicate the audio source event. */
};

/**
* @brief      This function is used to get the audio path of line-in app.
* @return     APP_AUDIO_PATH_LINE_IN, the audio path is line-in, the audio source of the audio manager is occupied by line-in.
*             Others, the audio path is not line-in, the audio source of the audio manager can be used by A2DP and so on.
*/
app_audio_path_t app_line_in_activity_get_current_audio_path();


/**
* @brief      This function is the interface of the app_line_in_idle_activity, and is only called by ui_shell framework when events are sent.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_group, the current event group to be handled.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_line_in_idle_activity_proc(struct _ui_shell_activity *self,
                                    uint32_t event_group,
                                    uint32_t event_id,
                                    void *extra_data,
                                    size_t data_len);

/**
* @brief      This function is used to get the line in plug status.
* @return     If line in plugged in, return true.
*/
bool app_line_in_is_plug_in(void);

/**
* @brief      This function is used to get the line out status.
* @return     If line out open, return true.
*/
bool app_line_out_is_open(void);

#endif /* APPS_LINE_IN_SUPPORT */

#endif /* __APP_LINE_IN_IDLE_ACTIVITY_H__ */

