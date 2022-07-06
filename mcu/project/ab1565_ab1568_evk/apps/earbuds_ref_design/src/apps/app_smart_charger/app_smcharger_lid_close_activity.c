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
 * File: app_smcharger_lid_close_activity.c
 *
 * Description: This file is the activity to handle action/event in LID_CLOSE state.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for SmartCharger state machine.
 *
 */

#ifdef MTK_SMART_CHARGER_ENABLE

#include "app_smcharger_lid_close_activity.h"
#include "timers.h"

#include "app_smcharger_lid_open_activity.h"
#include "app_smcharger_out_of_case_activity.h"
#include "app_smcharger_off_activity.h"

#define SMCHARGER_LID_CLOSE_ACTIVITY "[SMCharger]lid_close_activity"

/**
* @brief      smcharger_timer_t is used to debounce "device shake in the SmartCharger Lid_close Case".
* default timer 500ms
* smcharger_out_timer_callback_function, device will switch to "out_of_case" state when 500ms smcharger_timer timeout
* smcharger_out_timer_start, start 500ms smcharger_timer if receive charger_out event in the SmartCharger Lid_close Case
* smcharger_out_timer_cancel, stop 500ms smcharger_timer if receive charger_out event then "charger in" within 500ms.
*/
#define SMCHARGER_TIMER_NAME         "smcharger_timer"
#define SMCHARGER_TIMER_INTERVAL     500
typedef struct {
    TimerHandle_t   timer;
    bool            is_timer_start;
} smcharger_timer_t;

static void smcharger_out_timer_callback_function(TimerHandle_t xTimer)
{
    APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", timer callback", 0);
    ui_shell_activity_t *lid_close_activity = (ui_shell_activity_t *)pvTimerGetTimerID(xTimer);
    if (lid_close_activity != NULL) {
        ui_shell_start_activity(lid_close_activity,
                                (ui_shell_proc_event_func_t)app_smcharger_out_of_case_activity_proc,
                                ACTIVITY_PRIORITY_MIDDLE, (void *)NULL, 0);
        ui_shell_finish_activity(lid_close_activity, lid_close_activity);
    }
    xTimerStop(xTimer, 0);
}

static void smcharger_out_timer_start(ui_shell_activity_t *self)
{
    smcharger_timer_t *smcharger_timer = (smcharger_timer_t *)self->local_context;
    if (smcharger_timer != NULL && !smcharger_timer->is_timer_start) {
        APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", out_timer start", 0);
        xTimerStart(smcharger_timer->timer, 0);
        smcharger_timer->is_timer_start = TRUE;
    }
}

static void smcharger_out_timer_cancel(ui_shell_activity_t *self)
{
    smcharger_timer_t *smcharger_timer = (smcharger_timer_t *)self->local_context;
    if (smcharger_timer != NULL && smcharger_timer->is_timer_start) {
        APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", out_timer cancel", 0);
        xTimerStop(smcharger_timer->timer, 0);
        smcharger_timer->is_timer_start = FALSE;
    }
}

static bool smcharger_lid_close_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", create", 0);
            /* malloc and create smcharger_timer_t. */
            self->local_context = pvPortMalloc(sizeof(smcharger_timer_t));
            if (self->local_context != NULL) {
                smcharger_timer_t *smcharger_timer = (smcharger_timer_t *)self->local_context;
                smcharger_timer->timer = xTimerCreate(SMCHARGER_TIMER_NAME,
                                                      SMCHARGER_TIMER_INTERVAL / portTICK_PERIOD_MS,
                                                      pdFALSE, self, smcharger_out_timer_callback_function);
                smcharger_timer->is_timer_start = false;
            } else {
                APPS_LOG_MSGID_E(SMCHARGER_LID_CLOSE_ACTIVITY", malloc fail", 0);
            }
            /* Do LID_CLOSE action - power off BT and mute audio. */
            app_smcharger_state_do_action(STATE_SMCHARGER_LID_CLOSE);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", destroy", 0);
            /* Delete and free smcharger_timer_t. */
            if (self->local_context != NULL) {
                smcharger_timer_t *smcharger_timer = (smcharger_timer_t *)self->local_context;
                xTimerStop(smcharger_timer->timer, 0);
                xTimerDelete(smcharger_timer->timer, 0);
                vPortFree(self->local_context);
            }
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool smcharger_lid_close_proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    return ret;
}

bool app_smcharger_lid_close_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = smcharger_lid_close_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell key events - ignore all key events when device in the SmartCharger case. */
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = smcharger_lid_close_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
        /* APP interaction events - only handle and update led background pattern. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            if (event_id == APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN) {
                APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", INTERACTION group LED_BG_PATTERN", 0);
                ret = app_smcharger_show_led_bg_pattern();
            }
            break;
        }
        /* APP SmartCharger events. */
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE: {
            APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", CHARGER_CASE group, event_id=%d", 1, event_id);
            ret = false;
            if (event_id == EVENT_ID_SMCHARGER_CHARGER_OUT_PATTERN) {
                /* Start 500ms smcharger_timer when receive charger_out event in LID_CLOSE state. */
                smcharger_out_timer_start(self);
            } else if (event_id == EVENT_ID_SMCHARGER_CHARGER_IN_INTERRUPT) {
                /* Stop 500ms smcharger_timer when receive charger_in event within 500ms. */
                smcharger_out_timer_cancel(self);
            }  else if (event_id == EVENT_ID_SMCHARGER_LID_OPEN_PATTERN) {
                /* Switch to lid_open activity when receive lid_open event in LID_CLOSE state. */
                ui_shell_start_activity(self,
                                        (ui_shell_proc_event_func_t)app_smcharger_lid_open_activity_proc,
                                        ACTIVITY_PRIORITY_HIGH, (void *)NULL, 0);
                ui_shell_finish_activity(self, self);
                ret = true;
            } else if (event_id == EVENT_ID_SMCHARGER_CHARGER_OFF_PATTERN) {
                /* Switch to off activity when receive charger_off event in LID_CLOSE state. */
                /* Such as SmartCharger case low battery, or charging completely. */
                smcharger_out_timer_cancel(self);
                ui_shell_start_activity(self,
                                        (ui_shell_proc_event_func_t)app_smcharger_off_activity_proc,
                                        ACTIVITY_PRIORITY_MIDDLE, (void *)NULL, 0);
                ui_shell_finish_activity(self, self);
                ret = true;
            } else if (event_id == EVENT_ID_SMCHARGER_LID_CLOSE_PATTERN) {
                /* Ignore and block it when receive duplicate lid_close event in LID_CLOSE state. */
                APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", recv duplicate lid_close event", 0);
                ret = true;
            } else if (event_id == EVENT_ID_SMCHARGER_CASE_BATTERY_REPORT) {
                app_smcharger_handle_case_battery((int)extra_data);
            } else if (event_id == EVENT_ID_SMCHARGER_CHARGER_KEY_PATTERN) {
                app_smcharger_handle_key_event((uint32_t)extra_data);
                ret = true;
            } else if (event_id == EVENT_ID_SMCHARGER_USER_DATA1_PATTERN
                       || event_id == EVENT_ID_SMCHARGER_USER_DATA2_PATTERN
                       || event_id == EVENT_ID_SMCHARGER_USER_DATA3_PATTERN) {
                app_smcharger_handle_user_data(STATE_SMCHARGER_LID_CLOSE, event_id, (int)extra_data);
                ret = true;
            } else {
                APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", unexpected smcharger event", 0);
            }
            APPS_LOG_MSGID_I(SMCHARGER_LID_CLOSE_ACTIVITY", CHARGER_CASE group, ret=%d", 1, ret);
            break;
        }
        default:
            break;
    }
    return ret;
}

#endif
