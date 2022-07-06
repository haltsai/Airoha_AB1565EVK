
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
 * File: app_va_xiaowei_transient_activity.c
 *
 * Description:
 * This file is Xiaowei transient activity. The app_va_xiaowei_activity will start this
 * activity when Xiaowei is recording.
 *
 */


#include "app_va_xiaowei_transient_activity.h"
#include "apps_events_interaction_event.h"
#include "apps_events_event_group.h"
#include "apps_config_state_list.h"
#include "xiaowei.h"
#include "apps_debug.h"

#define APP_VA_TRANS_XIAOWEI_PREFIX "[VA_XIAOWEI] VA_XIAOWEI_TRANSIENT_ACTIVITY"


bool va_xiaowei_app_transient_activity_proc_ui_shell_group(ui_shell_activity_t *self,
                                                           uint32_t event_id,
                                                           void *extra_data,
                                                           size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
            APPS_LOG_MSGID_I(APP_VA_TRANS_XIAOWEI_PREFIX", Transient activity CREATE", 0);
            /* Update MMI state to APP_STATE_VA when Xiaowei is recording. */
            ui_shell_send_event(false,
                                EVENT_PRIORITY_HIGH,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE,
                                NULL,
                                0,
                                NULL,
                                0);
            break;

        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            APPS_LOG_MSGID_I(APP_VA_TRANS_XIAOWEI_PREFIX", Transient activity DESTROY", 0);
            ui_shell_send_event(false,
                                EVENT_PRIORITY_HIGH,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE,
                                NULL,
                                0,
                                NULL,
                                0);
            break;

        default:
            break;
    }
    return true;
}

bool app_va_xiaowei_transient_activity_proc(struct _ui_shell_activity *self,
                                            uint32_t event_group,
                                            uint32_t event_id,
                                            void *extra_data,
                                            size_t data_len)
{

    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = va_xiaowei_app_transient_activity_proc_ui_shell_group(self, event_id, extra_data, data_len);
        }
        break;

        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            if (event_id == APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE) {
                APPS_LOG_MSGID_I(APP_VA_TRANS_XIAOWEI_PREFIX", Configure MMI state to be VA (%d)", 1, APP_STATE_VA);
                apps_config_key_set_mmi_state(APP_STATE_VA);
                ret = true;
            }
        }
        break;

        case EVENT_GROUP_UI_SHELL_VA_XIAOWEI: {
            /* When the status is ready, need finish the activity. */
            if (event_id == XIAOWEI_STATUS_READY) {
                ui_shell_finish_activity(self, self);
                ret = false;
            }
        }
        break;
    }

    return ret;
}

