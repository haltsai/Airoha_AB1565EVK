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
 * File: app_smcharger_out_of_case_activity.c
 *
 * Description: This file is the activity to handle action/event in OUT_OF_CASE state.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for SmartCharger state machine.
 *
 */


#ifdef MTK_SMART_CHARGER_ENABLE

#include "app_smcharger_off_activity.h"

#include "app_smcharger_lid_open_activity.h"

#define SMCHARGER_OUT_OF_CASE_ACTIVITY "[SMCharger]out_of_case_activity"

static bool smcharger_out_of_case_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", create", 0);
            /* Power_off device or play "low battery" VP according current app_smcharger_battery_state_t. */
            bool need_power_off = false;
            app_smcharger_context_t *smcharger_ctx = app_smcharger_get_context();
            if (smcharger_ctx != NULL) {
                if (smcharger_ctx->battery_state == SMCHARGER_BATTERY_STATE_SHUTDOWN) {
                    APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", ShutDown in create", 0);
                    need_power_off = true;
                    app_smcharger_power_off(TRUE);
                } else if (smcharger_ctx->battery_state == SMCHARGER_BATTERY_STATE_LOW_CAP) {
                    APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", LowCap in create", 0);
                    apps_config_set_vp(VP_INDEX_LOW_BATTERY, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                }
            }

            if (!need_power_off) {
                /* Do OUT_OF_CASE action - Power on BT and unmute Audio. */
                app_smcharger_state_do_action(STATE_SMCHARGER_OUT_OF_CASE);
            }
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool smcharger_out_of_case_proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    return ret;
}

bool app_smcharger_out_of_case_activity_proc(
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
            ret = smcharger_out_of_case_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell key events - ignore all key events when device in the SmartCharger case. */
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = smcharger_out_of_case_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
        /* APP interaction events - only handle and update led background pattern. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            if (event_id == APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN) {
                APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", INTERACTION group LED_BG_PATTERN", 0);
                ret = app_smcharger_show_led_bg_pattern();
            }
            break;
        }
        /* APP SmartCharger events. */
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE: {
            APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", CHARGER_CASE group, event_id=%d", 1, event_id);
            ret = false;
            if (event_id == EVENT_ID_SMCHARGER_CHARGER_IN_INTERRUPT) {
                /* Switch to lid_open activity when receive charger_in event in OUT_OF_CASE state. */
                ui_shell_start_activity(self,
                                        (ui_shell_proc_event_func_t)app_smcharger_lid_open_activity_proc,
                                        ACTIVITY_PRIORITY_HIGH, (void *)NULL, 0);
                ui_shell_finish_activity(self, self);
            } else {
                APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", unexpected smcharger event", 0);
            }
            APPS_LOG_MSGID_I(SMCHARGER_OUT_OF_CASE_ACTIVITY", CHARGER_CASE group, ret=%d", 1, ret);
            break;
        }
        default:
            break;
    }
    return ret;
}

#endif
