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

#include "ui_shell_stack_message_handler.h"

#include "ui_shell_activity.h"
#include "ui_shell_activity_stack.h"
#include "ui_shell_al_log.h"
#include "ui_shell_al_memory.h"
#include "task_def.h"

static void _handle_message(ui_shell_msg_handler_t *handler, ui_shell_msg_t* msg)
{
    ui_shell_activity_internal_t *activity;

    UI_SHELL_LOG_MSGID_I("handle message group : 0x%x, id : 0x%x, extra_data : 0x%x, data_len : 0x%x, free_func : 0x%x", 5,
            msg->msg_group, msg->msg_id, msg->msg_data, msg->msg_data_len, msg->special_free_extra_func);
    switch (msg->msg_group) {
    case EVENT_GROUP_UI_SHELL_SYSTEM:
        activity = (ui_shell_activity_internal_t *)msg->msg_data2;
        switch (msg->msg_id) {
        case UI_SHELL_INTERNAL_EVENT_SYSTEM_START:
            ui_shell_activity_stack_start();
            break;
#ifdef WIN32
        case UI_SHELL_INTERNAL_EVENT_SYSTEM_STOP:
            ui_shell_activity_stack_clear();
            if (((ui_shell_stack_message_handler_t *)handler)->m_temp_semaPhore) {
                ui_shell_al_semaphore_give(((ui_shell_stack_message_handler_t *)handler)->m_temp_semaPhore);
            }
            break;
#endif
        case UI_SHELL_INTERNAL_EVENT_START_ACTI:
            if (!ui_shell_activity_stack_add(activity, msg->msg_data, msg->msg_data_len)) {
                UI_SHELL_LOG_MSGID_E("add activity failed, need free data in ui shell", 0);
            }
            if (msg->msg_data && msg->msg_data_len) {
                ui_shell_al_free(msg->msg_data);
            }
            break;
        case UI_SHELL_INTERNAL_EVENT_FINISH_ACTI:
            ui_shell_activity_stack_remove(activity);
            break;
        case UI_SHELL_INTERNAL_EVENT_SET_RESULT:
            if (!ui_shell_activity_stack_on_result(activity, msg->msg_data, msg->msg_data_len)) {
                UI_SHELL_LOG_MSGID_E("set result to target activity failed, need free data in ui shell", 0);
            }
            if (msg->msg_data && msg->msg_data_len) {
                ui_shell_al_free(msg->msg_data);
            }
            break;
        case UI_SHELL_INTERNAL_EVENT_TRIGGER_REFRESH:
            ui_shell_activity_stack_refresh_activity(activity);
            break;
        case UI_SHELL_INTERNAL_EVENT_BACK_TO_IDLE:
            ui_shell_activity_stack_go_to_idle();
            break;
        case UI_SHELL_INTERNAL_EVENT_GET_ALLOWN:
            // msg->msg_data is request id, msg->msg_data2 is the activity who sends the request
            ui_shell_activity_stack_request_allowance((uint32_t)msg->msg_data,
                    (ui_shell_activity_internal_t *)msg->msg_data2);
            break;
        case UI_SHELL_INTERNAL_EVENT_ALLOWED:
            ui_shell_activity_stack_allowed((uint32_t)msg->msg_data,
                    (ui_shell_activity_internal_t *)msg->msg_data2);
            break;
        default:
            UI_SHELL_LOG_MSGID_I("No defined msg id : %x", 1, msg->msg_data);
            break;
        }
        break;
    default:
        ui_shell_activity_stack_traverse_stack(
                msg->msg_group,
                msg->msg_id,
                msg->msg_data,
                msg->msg_data_len);
        UI_SHELL_LOG_MSGID_I("handle message group end: 0x%x, id : 0x%x, extra_data : 0x%x, data_len : 0x%x, free_func : 0x%x", 5,
                msg->msg_group, msg->msg_id, msg->msg_data, msg->msg_data_len, msg->special_free_extra_func);
        if (msg->msg_data &&  msg->msg_data_len > 0) {
            if (msg->special_free_extra_func) {
                msg->special_free_extra_func(msg->msg_data);
            }
            ui_shell_al_free(msg->msg_data);
        }
        break;
    }
}

static void ui_shell_stack_message_handler_init(ui_shell_stack_message_handler_t *stack_handler)
{
    ui_shell_init_message_handler((ui_shell_msg_handler_t *)stack_handler, UI_SHELL_TASK_NAME,
            UI_SHELL_TASK_STACKSIZE, UI_SHELL_TASK_PRIO);
    ((ui_shell_msg_handler_t *)stack_handler)->handleMessage = _handle_message;
}

#ifdef WIN32
static void ui_shell_stack_message_handler_deinit(ui_shell_stack_message_handler_t *stack_handler)
{
    ui_shell_msg_t msg;
    ui_shell_init_input_msg(&msg,
        EVENT_PRIORITY_UI_SHELL_SYSTEM,
        EVENT_GROUP_UI_SHELL_SYSTEM,
        UI_SHELL_INTERNAL_EVENT_SYSTEM_STOP,
        NULL, 0, NULL, 0,
        NULL);
    stack_handler->handler.sendMessage(&stack_handler->handler,
            false,
            &msg);

    if (stack_handler->m_temp_semaPhore) {
        ui_shell_al_semaphore_take(stack_handler->m_temp_semaPhore);
    }

    ui_shell_al_destroy_semaphore(stack_handler->m_temp_semaPhore);

    ui_shell_deinit_message_handler((ui_shell_msg_handler_t *)stack_handler);
}
#endif

ui_shell_stack_message_handler_t *ui_shell_stack_message_handler_new(void)
{
    ui_shell_stack_message_handler_t *handler = ui_shell_al_malloc(sizeof(ui_shell_stack_message_handler_t));
    if (handler) {
#ifdef WIN32
        handler->m_temp_semaPhore = ui_shell_al_semaphore_create_binary();
        if (handler->m_temp_semaPhore) {
#endif
            ui_shell_stack_message_handler_init(handler);
#ifdef WIN32
        } else {
            UI_SHELL_LOG_MSGID_E("Create ui_shell_stack_message_handler->m_temp_semaPhore failed", 0);
            ui_shell_al_free(handler);
            handler = NULL;
        }
#endif
    } else {
        UI_SHELL_LOG_MSGID_E("Create ui_shell_stack_message_handler failed", 0);
    }
    return handler;
}

#ifdef WIN32
void ui_shell_stack_message_handler_delete(ui_shell_stack_message_handler_t *stack_handler)
{
    if (stack_handler) {
        ui_shell_stack_message_handler_deinit(stack_handler);
    }
    ui_shell_al_free(stack_handler);
}
#endif
