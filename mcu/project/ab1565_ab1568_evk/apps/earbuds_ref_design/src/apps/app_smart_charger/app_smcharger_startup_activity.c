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
 * File: app_smcharger_startup_activity.c
 *
 * Description: This file is the activity to handle action/event in STARTUP state.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for SmartCharger state machine.
 *
 */

#ifdef MTK_SMART_CHARGER_ENABLE

#include "app_smcharger_startup_activity.h"

#include "app_smcharger_lid_open_activity.h"
#include "app_smcharger_lid_close_activity.h"
#include "app_smcharger_out_of_case_activity.h"

#define SMCHARGER_STARTUP_ACTIVITY "[SMCharger]startup_activity"

static bool smcharger_startup_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", create", 0);
            /* Do STARTUP action - handle system boot reason and send event. */
            /* Charger_in boot - send charger_in_boot event and switch to lid_open state. */
            /* Power key boot or other - send power_key_boot event and switch to out_of_case state. */
            app_smcharger_state_do_action(STATE_SMCHARGER_STARTUP);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

bool app_smcharger_startup_activity_proc(
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
            ret = smcharger_startup_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        /* APP SmartCharger events. */
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE: {
            APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", CHARGER_CASE group, event_id=%d", 1, event_id);
            ret = false;
            if (event_id == EVENT_ID_SMCHARGER_POWER_KEY_BOOT) {
                /* Switch to out_of_case activity when receive power_key_boot event in STARTUP state. */
                ui_shell_start_activity(self,
                                        (ui_shell_proc_event_func_t)app_smcharger_out_of_case_activity_proc,
                                        ACTIVITY_PRIORITY_MIDDLE, (void *)NULL, 0);
                ui_shell_finish_activity(self, self);
            } else if (event_id == EVENT_ID_SMCHARGER_CHARGER_IN_BOOT) {
                /* Switch to lid_open activity when receive charger_in_boot event in STARTUP state. */
                ui_shell_start_activity(self,
                                        (ui_shell_proc_event_func_t)app_smcharger_lid_open_activity_proc,
                                        ACTIVITY_PRIORITY_HIGH, (void *)NULL, 0);
                ui_shell_finish_activity(self, self);
            } else if (event_id == EVENT_ID_SMCHARGER_LID_CLOSE_PATTERN) {
                /* Switch to lid_close activity when receive lid_close event in STARTUP state. */
                ui_shell_start_activity(self,
                                        (ui_shell_proc_event_func_t)app_smcharger_lid_close_activity_proc,
                                        ACTIVITY_PRIORITY_HIGH, (void *)NULL, 0);
                ui_shell_finish_activity(self, self);
            } else if (event_id == EVENT_ID_SMCHARGER_CHARGER_OUT_PATTERN) {
                /* Switch to lid_open activity and send charger_out event when receive charger out event in STARTUP state. */
                ui_shell_start_activity(self,
                                        (ui_shell_proc_event_func_t)app_smcharger_lid_open_activity_proc,
                                        ACTIVITY_PRIORITY_HIGH, (void *)SMCHARGER_BOOT_OUT_FLAG, 0);
                ui_shell_finish_activity(self, self);
            } else {
                APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", unexpected smcharger event", 0);
            }
            APPS_LOG_MSGID_I(SMCHARGER_STARTUP_ACTIVITY", CHARGER_CASE group, ret=%d", 1, ret);
            break;
        }
        default:
            break;
    }
    return ret;
}

#endif
