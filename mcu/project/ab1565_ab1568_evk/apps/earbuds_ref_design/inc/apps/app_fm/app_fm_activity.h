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
 * File: app_fm_activity.h
 *
 * Description: This file defines the interface of app_fm_activity.c.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifndef __APP_FIND_ME_ACTIVITY_H__
#define __APP_FIND_ME_ACTIVITY_H__

#include "bt_sink_srv.h"
#include "ui_shell_manager.h"
#include "ui_shell_activity.h"
#include "FreeRTOS.h"
#include "apps_debug.h"
#include "apps_config_event_list.h"
#include "apps_config_state_list.h"
#include "airo_key_event.h"
#include "apps_events_interaction_event.h"
#include "apps_events_key_event.h"


/**
 *  @brief This structure defines the parameter to trigger find me.
 */
typedef struct {
    uint8_t blink;                /**<  1 means need light up LED, 0 means no need LED. */
    uint8_t tone;                 /**<  1 means need play ringtone, 0 means no need ringtone. */
    uint16_t duration_seconds;    /**<  Duration time(seconds) to keep LED/ringtone, 0 means use default value. */
} app_find_me_param_struct;

/**
 *  @brief This structure defines the data format of find me notification.
 */
typedef struct {
    uint8_t blink;                /**<  1 means need light up LED, 0 means no need LED. */
    uint8_t tone;                 /**<  1 means need play ringtone, 0 means no need ringtone. */
} app_find_me_notify_state_t;


/**
 *  @brief This structure defines the local context of app_fm_activity.
 */
typedef struct {
    uint8_t blink;                   /**<  1 means current find me cmd need light up LED, 0 means no need LED. */
    uint8_t tone;                    /**<  1 means current find me cmd need play ringtone, 0 means no need ringtone. */
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    uint8_t peer_blink;
    uint8_t peer_tone;
#endif
    uint16_t duration_seconds;       /**<  Duration time(seconds) of current findme cmd, 0 means use default value. */
    uint8_t count;                   /**<  Record the loop count of ringtone playback. */
    bt_sink_srv_state_t sink_state;  /**<  Record the sink_state. */
} app_find_me_context_t;


/**
 *  @brief This enum defines the events of group EVENT_GROUP_UI_SHELL_FINDME.
 */
typedef enum {
    APP_FIND_ME_EVENT_ID_TRIGGER = 0,    /**<  The event to trigger find me, send by race cmd or other app. */
} app_find_me_event_id_t;

typedef enum {
    FIND_ME_LEFT = 1,          /**< Left do find me. */
    FIND_ME_RIGHT,             /**< Right do find me. */
    FIND_ME_LEFT_RIGHT,        /**< Both of left and right do find me. */
} app_find_me_config_t;

/**
 * @brief      The find me state change, only called by find me app.
 * @param[in]  context, the context the find me app.
 * @param[in]  context, if need aws sync.
 */
void app_find_me_notify_state_change(app_find_me_context_t *context, bool need_aws_sync);

/**
 * @brief      The Fine me action function.
 * @param[in]  enable, true is start find me, false is stop find me.
 * @param[in]  config, FIND_ME_LEFT is left do find me action, FIND_ME_RIGHT is right,
 *                     FIND_ME_LEFT_RIGHT is left and right.
 * @return     If return true, do find me success.
 * @note:      Only can call this function at agent side or partner side.
 *             Headset has only FIND_ME_LEFT config.
 */
bool app_find_me_do_find_me_action(bool enable, app_find_me_config_t config);

/**
* @brief      This function is the interface of the app_fm_activity, and is only called by ui_shell when events are sent.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_group, the current event group to be handled.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_find_me_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);

/**
* @brief      This function is to get the find me app context.
* @return     If return true, the context of find me app.
*/
const app_find_me_context_t *app_find_me_idle_activity_get_context(void);

#endif


