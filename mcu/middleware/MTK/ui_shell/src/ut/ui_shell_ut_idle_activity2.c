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

#include "ui_shell_ut_activity2_1.h"
#include "ui_shell_al_log.h"
#include "airo_key_event.h"
#include "ui_shell_al_memory.h"
#include "ui_shell_manager.h"
#include "ut_event_group.h"
#include <string.h>

typedef struct _local_context_type {
    uint32_t test_data2;
} local_context_type_t;

static bool _proc_ui_shell_group(
        struct _ui_shell_activity *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = true; // UI shell internal event must process by this activity, so default is true
    switch (event_id) {
    case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
        UI_SHELL_LOG_MSGID_I("ui_shell_ut_idle_activity1 create", 0);
        self->local_context = ui_shell_al_malloc(sizeof(local_context_type_t));
        if (extra_data) {
            UI_SHELL_LOG_I("extra data for activity1 create : %s", extra_data);
        }
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
        UI_SHELL_LOG_MSGID_I("ui_shell_ut_idle_activity1 destroy", 0);
        if (self->local_context) {
            ui_shell_al_free(self->local_context);
        }
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
        UI_SHELL_LOG_MSGID_I("ui_shell_ut_idle_activity1 resume", 0);
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
        UI_SHELL_LOG_MSGID_I("ui_shell_ut_idle_activity1 pause", 0);
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
        UI_SHELL_LOG_MSGID_I("ui_shell_ut_idle_activity1 refresh", 0);
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
        UI_SHELL_LOG_MSGID_I("ui_shell_ut_idle_activity1 result", 0);
        if (extra_data) {
            UI_SHELL_LOG_I("extra data for activity1 result : %s", extra_data);
        }
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_REQUEST_ALLOWANCE:
    {
        uint32_t request_id = (uint32_t)extra_data;
        UI_SHELL_LOG_MSGID_I("ui_shell_ut_activity2 request allowance : %d", 1, request_id);
        if (ALL_ALLOW_REQUEST_ID == request_id) {
            ret = true;
        }
        else {
            ret = false;
        }
    }
    break;
    default:
        break;
    }
    return ret;
}

static bool _proc_key_event_group(
        struct _ui_shell_activity *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;
    uint32_t key_num = event_id & 0x000000FF;
    uint32_t key_event = ((event_id >> 8) & 0x000000FF);
    switch (key_num) {
    case APP2_CARE_KEY_ID:
        if (key_event == AIRO_KEY_SHORT_CLICK) { // key event
            char *send_data = ui_shell_al_malloc(sizeof("B"));
            ui_shell_al_memcpy(send_data, "B", sizeof("B"));
            if (extra_data && data_len) {
                UI_SHELL_LOG_I("extra data = %s", extra_data);
            }
            ui_shell_start_activity(self, ui_shell_ut_activity2_1_proc, ACTIVITY_PRIORITY_MIDDLE,
                    send_data, sizeof("B"));
            ret = true;
        }
        break;
    case TEST_ALLOW_KEY_ID:
        if (key_event == AIRO_KEY_SLONG && memcmp(extra_data, "2", sizeof("2")) == 0) { // key event
            ui_shell_grant_allowance(self, ALL_PENDING_REQUEST_ID);
            ret = true;
        }
        break;
    default:
        UI_SHELL_LOG_MSGID_I("Doesn't care keyID : %d", 1, event_id & 0x000000FF);
        break;
    }
    return ret;
}

bool ui_shell_ut_idle_activity2_proc(
            struct _ui_shell_activity *self,
            uint32_t event_group,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    switch (event_group) {
    case EVENT_GROUP_UI_SHELL_SYSTEM:
        ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
        break;
    case EVENT_GROUP_UI_SHELL_KEY:
        ret = _proc_key_event_group(self, event_id, extra_data, data_len);
        break;
    default:
        break;
    }
    return ret;
}

