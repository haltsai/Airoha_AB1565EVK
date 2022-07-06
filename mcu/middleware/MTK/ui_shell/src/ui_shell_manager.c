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

#include "ui_shell_manager.h"

#include "ui_shell_activity_stack.h"
#include "ui_shell_activity_heap.h"
#include "ui_shell_stack_message_handler.h"
#include "ui_shell_al_log.h"
#include "ui_shell_al_memory.h"

static ui_shell_stack_message_handler_t *s_message_handler = NULL;

ui_shell_status_t ui_shell_start(void)
{
    ui_shell_msg_t msg;
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    if (!s_message_handler) {
        s_message_handler = ui_shell_stack_message_handler_new();
        if (!s_message_handler) {
            UI_SHELL_LOG_MSGID_E("new ui_shell_stack_message_handler failed", 0);
            return UI_SHELL_STATUS_OUT_OF_RESOURCE;
        }

        ui_shell_init_input_msg(&msg,
                EVENT_PRIORITY_UI_SHELL_SYSTEM,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                UI_SHELL_INTERNAL_EVENT_SYSTEM_START,
                NULL, 0,
                NULL, 0,
                NULL);
        s_message_handler->handler.sendMessage(&s_message_handler->handler,
                false,
                &msg);
    } else {
        UI_SHELL_LOG_MSGID_E("Already started a message handler", 0);
        return UI_SHELL_STATUS_INVALID_STATE;
    }
    return ret;
}

ui_shell_status_t ui_shell_set_pre_proc_func(ui_shell_proc_event_func_t proc_event)
{
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    if (s_message_handler) {
        UI_SHELL_LOG_MSGID_E("UI shell has already started, cannot call ui_shell_set_pre_proc_func", 0);
        return UI_SHELL_STATUS_INVALID_STATE;
    }
    ui_shell_activity_internal_t *activity = ui_shell_activity_heap_get_preproc_item();
    if (activity) {
        activity->activity_type = UI_SHELL_PRE_PROC_ACTIVITY;
        activity->proc_event_func = proc_event;
        activity->priority = ACTIVITY_PRIORITY_HIGHEST; // priority is not important, bacause pre-proc is not in stack
        ui_shell_activity_stack_add(activity, NULL, 0);
    } else {
        UI_SHELL_LOG_MSGID_E("new UI_SHELL_PRE_PROC_ACTIVITY failed", 0);
        ret = UI_SHELL_STATUS_OUT_OF_RESOURCE;
    }

    return ret;
}

ui_shell_status_t ui_shell_start_activity(ui_shell_activity_t *self,
        ui_shell_proc_event_func_t proc_event,
        ui_shell_activity_priority_t priority,
        void *extra_data,
        size_t data_len)
{
    ui_shell_activity_internal_t *activity;
    ui_shell_msg_t msg;
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    if (!s_message_handler && priority > ACTIVITY_PRIORITY_IDLE_TOP) {
        return UI_SHELL_STATUS_INVALID_STATE;
    } else if (!proc_event || priority > ACTIVITY_PRIORITY_HIGHEST) {
        return UI_SHELL_STATUS_INVALID_PARAMETER;
    }

    activity = ui_shell_activity_heap_get_free_item();
    if (activity) {
        if (ACTIVITY_PRIORITY_IDLE_TOP >=  priority) {
            activity->activity_type = UI_SHELL_IDLE_ACTIVITY;
        } else {
            activity->activity_type = UI_SHELL_TRANSIENT_ACTIVITY;
        }
        activity->proc_event_func = proc_event;
        activity->priority = priority;
        activity->started_by = (ui_shell_activity_internal_t *)self;
        if (!s_message_handler) {
            ui_shell_activity_stack_add(activity, NULL, 0);
        } else {
            ui_shell_init_input_msg(&msg,
                    EVENT_PRIORITY_UI_SHELL_SYSTEM,
                    EVENT_GROUP_UI_SHELL_SYSTEM,
                    UI_SHELL_INTERNAL_EVENT_START_ACTI,
                    extra_data, data_len,
                    activity, 0,
                    NULL);
            s_message_handler->handler.sendMessage(&s_message_handler->handler,
                    false,
                    &msg);
        }
    } else {
        UI_SHELL_LOG_MSGID_E("new UI_SHELL_TRANSIENT_ACTIVITY failed", 0);
        ret = UI_SHELL_STATUS_OUT_OF_RESOURCE;
    }

    return ret;
}

ui_shell_status_t ui_shell_finish_activity(ui_shell_activity_t *self, ui_shell_activity_t *target_activity)
{
    ui_shell_msg_t msg;
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    (void)self; // Unused, just simulate our API as a class method
    if (!s_message_handler)
    {
        UI_SHELL_LOG_MSGID_E("Should not call finish activity before ui shell running", 0);
        return UI_SHELL_STATUS_INVALID_STATE;
    }
    if (target_activity && s_message_handler) {
        ui_shell_init_input_msg(&msg,
                EVENT_PRIORITY_UI_SHELL_SYSTEM,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                UI_SHELL_INTERNAL_EVENT_FINISH_ACTI,
                NULL, 0,
                target_activity, 0, // Not neccesary to set data_len, because the length is constant
                NULL);
        s_message_handler->handler.sendMessage(&s_message_handler->handler,
                false,
                &msg);
    } else {
        ret = UI_SHELL_STATUS_INVALID_PARAMETER;
    }

    return ret;
}

ui_shell_status_t ui_shell_back_to_idle(ui_shell_activity_t *self)
{
    ui_shell_msg_t msg;
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    (void)self; // Unused, just simulate our API as a class method
    if (!s_message_handler)
    {
        UI_SHELL_LOG_MSGID_E("Should not call go back to idle before ui shell running", 0);
        return UI_SHELL_STATUS_INVALID_STATE;
    } else {
        ui_shell_init_input_msg(&msg,
                EVENT_PRIORITY_UI_SHELL_SYSTEM,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                UI_SHELL_INTERNAL_EVENT_BACK_TO_IDLE,
                NULL, 0, NULL, 0,
                NULL);
        s_message_handler->handler.sendMessage(&s_message_handler->handler,
                false,
                &msg);
    }

    return ret;
}

ui_shell_status_t ui_shell_set_result(ui_shell_activity_t *self, void *data, size_t data_len)
{
    ui_shell_msg_t msg;
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;
    ui_shell_activity_internal_t *internal_self = (ui_shell_activity_internal_t *)self;

    if (!s_message_handler)
    {
        UI_SHELL_LOG_MSGID_E("Should not call set_result before ui shell running", 0);
        return UI_SHELL_STATUS_INVALID_STATE;
    }

    if (internal_self && internal_self->started_by) {
        ui_shell_init_input_msg(&msg,
                EVENT_PRIORITY_UI_SHELL_SYSTEM,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                UI_SHELL_INTERNAL_EVENT_SET_RESULT,
                data, data_len, internal_self->started_by, 0,
                NULL);
        s_message_handler->handler.sendMessage(&s_message_handler->handler,
                false,
                &msg);
    } else {
        UI_SHELL_LOG_MSGID_E("This activity have no trunk activity", 0);
        ret = UI_SHELL_STATUS_INVALID_PARAMETER;
    }

    return ret;
}

ui_shell_status_t ui_shell_refresh_activity(ui_shell_activity_t *self, ui_shell_activity_t *target)
{
    ui_shell_msg_t msg;
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    if (!s_message_handler) {
        UI_SHELL_LOG_MSGID_E("Should not call ui_shell_trigger_activity_refresh before ui shell running", 0);
        return UI_SHELL_STATUS_INVALID_STATE;
    } else if (!target) {
        return UI_SHELL_STATUS_INVALID_PARAMETER;
    }

    ui_shell_init_input_msg(&msg,
            EVENT_PRIORITY_UI_SHELL_SYSTEM,
            EVENT_GROUP_UI_SHELL_SYSTEM,
            UI_SHELL_INTERNAL_EVENT_TRIGGER_REFRESH,
            NULL, 0, target, 0,
            NULL);
    s_message_handler->handler.sendMessage(&s_message_handler->handler,
            false,
            &msg);

    return ret;
}

ui_shell_status_t ui_shell_request_allowance(
        ui_shell_activity_t *self,
        uint32_t request_id
        )
{
    ui_shell_msg_t msg;
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    if (!s_message_handler) {
        UI_SHELL_LOG_MSGID_E("Should not call ui_shell_trigger_activity_refresh before ui shell running", 0);
        return UI_SHELL_STATUS_INVALID_STATE;
    } else if (!self) {
        return UI_SHELL_STATUS_INVALID_PARAMETER;
    }

    ui_shell_init_input_msg(&msg,
            EVENT_PRIORITY_UI_SHELL_SYSTEM,
            EVENT_GROUP_UI_SHELL_SYSTEM,
            UI_SHELL_INTERNAL_EVENT_GET_ALLOWN,
            (void *)request_id, 0, (void *)self, 0,
            NULL);
    s_message_handler->handler.sendMessage(&s_message_handler->handler,
            false,
            &msg);

    return ret;
}

ui_shell_status_t ui_shell_grant_allowance(ui_shell_activity_t *self, uint32_t request_id)
{
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;
    ui_shell_activity_internal_t *acti_need_notify = NULL;
    ui_shell_msg_t msg;

    if (!s_message_handler) {
        UI_SHELL_LOG_MSGID_E("Should not call ui_shell_grant_allowance before ui shell running", 0);
        return UI_SHELL_STATUS_INVALID_STATE;
    } else if (!self) {
        return UI_SHELL_STATUS_INVALID_PARAMETER;
    }

    acti_need_notify = ui_shell_activity_stack_allow((ui_shell_activity_internal_t *)self, request_id);
    if (acti_need_notify) {
        ui_shell_init_input_msg(&msg,
                EVENT_PRIORITY_UI_SHELL_SYSTEM,
                EVENT_GROUP_UI_SHELL_SYSTEM,
                UI_SHELL_INTERNAL_EVENT_ALLOWED,
                (void *)request_id, 0, (void *)acti_need_notify, 0,
                NULL);
        s_message_handler->handler.sendMessage(&s_message_handler->handler,
                false,
                &msg);
    }

    return ret;
}

ui_shell_status_t ui_shell_send_event(
        bool from_isr,
        ui_shell_event_priority_t priority,
        uint32_t event_group,
        uint32_t event_id,
        void *data, size_t data_len,
        void (*special_free_extra_func)(void),
        uint32_t delay_ms)
{
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    if (!s_message_handler) {
        UI_SHELL_LOG_MSGID_E("Should not call send event before ui shell running", 0);
        ret = UI_SHELL_STATUS_INVALID_STATE;
    } else if (priority > EVENT_PRIORITY_UI_SHELL_SYSTEM) {
        ret = UI_SHELL_STATUS_INVALID_PARAMETER;
    } else {
        UI_SHELL_LOG_MSGID_I("ui_shell_send_event: 0x%x, 0x%x, delay:%d", 3, event_group, event_id, delay_ms);
        if (delay_ms) {
            ui_shell_delay_msg_t msg;
            ui_shell_init_input_delay_msg(&msg,
                    priority,
                    event_group,
                    event_id,
                    data, data_len, NULL, 0,
                    NULL,
                    delay_ms);
            s_message_handler->handler.send_delay_message(&s_message_handler->handler,
                    from_isr,
                    &msg);
        } else {
            ui_shell_msg_t msg;
            ui_shell_init_input_msg(&msg,
                    priority,
                    event_group,
                    event_id,
                    data, data_len, NULL, 0,
                    NULL);
            s_message_handler->handler.sendMessage(&s_message_handler->handler,
                    from_isr,
                    &msg);
        }
    }

    return ret;
}

void ui_shell_send_delay_event(
        bool from_isr,
        ui_shell_event_priority_t priority,
        uint32_t event_group,
        uint32_t event_id,
        void *data, size_t data_len,
        void (*special_free_extra_func)(void),
        uint32_t delay_ms)
{
    ui_shell_delay_msg_t msg;

    if (!s_message_handler) {
        UI_SHELL_LOG_MSGID_E("Should not call send event before ui shell running", 0);
    } else {
        ui_shell_init_input_delay_msg(&msg,
                priority,
                event_group,
                event_id,
                data, data_len, NULL, 0,
                NULL,
                delay_ms);
        s_message_handler->handler.send_delay_message(&s_message_handler->handler,
                from_isr,
                &msg);
    }
}

ui_shell_status_t ui_shell_remove_event(uint32_t event_group,
        uint32_t event_id)
{
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    if (!s_message_handler) {
        UI_SHELL_LOG_MSGID_E("Should not call send event before ui shell running", 0);
        ret = UI_SHELL_STATUS_INVALID_STATE;
    } else {
        UI_SHELL_LOG_MSGID_I("ui_shell_remove_event: 0x%x, 0x%x", 2, event_group, event_id);
        s_message_handler->handler.removeMessages(&s_message_handler->handler, event_group, event_id);
    }

    return ret;
}

ui_shell_status_t ui_shell_finish(void)
{
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;

    if (s_message_handler) {
#ifdef WIN32
        // Real environment may not finish UI shell, use macro to mark these code
        ui_shell_stack_message_handler_delete(s_message_handler);
#endif
        s_message_handler = NULL;
    } else {
        ret = UI_SHELL_STATUS_INVALID_STATE;
    }
    return ret;
}
