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

#include "ui_shell_message_handler.h"

#include "ui_shell_al_log.h"
#include "ui_shell_al_memory.h"
#include "ui_shell_al_task.h"
#include "stddef.h"

#define DEFAULT_MESSAGE_HANDLE_TASK_PRIORITY                    (1)

/*
static uint8_t MessageHandler_getPriority(uint16_t msgId)
{
    return 10;
}
*/

static void MessageHandler_sendMessage(ui_shell_msg_handler_t *handler,
        bool from_isr, ui_shell_msg_t *input_msg)
{
    if (!input_msg) {
        return;
    }
    UI_SHELL_LOG_MSGID_D("MessageHandler_sendMessage msg id=%x-%x\n", 2,
            input_msg->msg_group, input_msg->msg_id);

    if (handler != NULL && handler->m_MessageQueue != NULL) {
        handler->m_MessageQueue->pushMessage(handler->m_MessageQueue, from_isr, input_msg);
        if (handler->m_semaPhore) {
            if (from_isr) {
                ui_shell_al_semaphore_give_from_isr(handler->m_semaPhore);
            } else {
                ui_shell_al_semaphore_give(handler->m_semaPhore);
            }
        }
    }
}

static void MessageHandler_sendDelayMessage(ui_shell_msg_handler_t *handler,
        bool from_isr, ui_shell_delay_msg_t *input_msg)
{
    if (!input_msg) {
        return;
    }
    UI_SHELL_LOG_MSGID_I("MessageHandler_sendDelayMessage msg id=%x-%x, act_time = %d\n", 3,
                input_msg->basic_msg.msg_group, input_msg->basic_msg.msg_id, input_msg->msg_act_time);

    if (handler != NULL && handler->m_DelayMessageQueue != NULL) {
        handler->m_DelayMessageQueue->pushDelayMessage(handler->m_DelayMessageQueue, input_msg);
        if (handler->m_semaPhore) {
            if (from_isr) {
                ui_shell_al_semaphore_give_from_isr(handler->m_semaPhore);
            } else {
                ui_shell_al_semaphore_give(handler->m_semaPhore);
            }
        }
    }
}

static void MessageHandler_removeMessages(ui_shell_msg_handler_t *handler, uint32_t event_group, uint32_t event_id)
{
    if (handler) {
        if (handler->m_MessageQueue) {
            handler->m_MessageQueue->removeMessages(handler->m_MessageQueue, event_group, event_id);
        }
        if (handler->m_DelayMessageQueue) {
            handler->m_DelayMessageQueue->parent.removeMessages(&handler->m_DelayMessageQueue->parent, event_group, event_id);
        }
    }
}

static void MessageHandler_handleMessage(ui_shell_msg_handler_t *handler, ui_shell_msg_t* msg)
{

}

static void MessageHandler_threadMethod(void* argc)
{
    ui_shell_msg_handler_t *p_handler = (ui_shell_msg_handler_t *)argc;
    ui_shell_msg_queue_t *queue;
    ui_shell_msg_t* msgToHandle;
    ui_shell_msg_t *delayMessage = NULL;
    ui_shell_msg_t *last_delayMessage = NULL;
    uint32_t next_delay_ms = 0;

    UI_SHELL_LOG_MSGID_I("MessageHandler_threadMethod start\n", 0);

#ifdef WIN32
    if (p_handler != NULL && p_handler->handleMessage && p_handler->m_thread_run_state == THREAD_STATE_STARTING)
#else
    if (p_handler != NULL && p_handler->handleMessage)
#endif
    {
#ifdef WIN32
        p_handler->m_thread_run_state = THREAD_STATE_RUNNING;
#endif
        queue = p_handler->m_MessageQueue;
#ifdef WIN32
        while(queue && p_handler->m_thread_run_state == THREAD_STATE_RUNNING) {
#else
        while (queue) {
#endif
            msgToHandle = queue->removeTopMessage(queue);
            if (msgToHandle) {
                p_handler->handleMessage(p_handler, msgToHandle);
                ui_shell_al_free(msgToHandle);
                continue;
            }
            next_delay_ms = p_handler->m_DelayMessageQueue->getNextTimeout(p_handler->m_DelayMessageQueue);
            if (0xFFFFFFFF == next_delay_ms) {
                if (p_handler->m_semaPhore) {
                    ui_shell_al_semaphore_take(p_handler->m_semaPhore);
                }
            } else if (0 != next_delay_ms) {
                if (p_handler->m_semaPhore) {
                    ui_shell_al_semaphore_take_in_time(p_handler->m_semaPhore, next_delay_ms);
                }
            } else {
                delayMessage = &(p_handler->m_DelayMessageQueue->removeTimeoutMessages(p_handler->m_DelayMessageQueue)->basic_msg);
                while (delayMessage) {
                    p_handler->handleMessage(p_handler, delayMessage);
                    last_delayMessage = delayMessage;
                    delayMessage = delayMessage->next_msg;
                    ui_shell_al_free(last_delayMessage);
                }
            }
            //UI_SHELL_LOG_D("[MessageHandler_threadMethod] delay 2s \n\n");
        }
#ifdef WIN32
        p_handler->m_thread_run_state = THREAD_STATE_STOPPED;
#endif
        UI_SHELL_LOG_MSGID_I("MessageHandler_threadMethod end\n", 0);
    } else {
        UI_SHELL_LOG_MSGID_E("MessageHandler_threadMethod fail\n", 0);
    }
	ui_shell_al_delete_task(NULL);
    //return 0;
}

static void MessageHandler_startThread(ui_shell_msg_handler_t *handler, const char *thread_name, uint16_t thread_size, uint32_t priority)
{
    ui_shell_al_create_task((ui_shell_al_task_handler)MessageHandler_threadMethod, thread_name, thread_size, (void *)handler, priority, NULL);
}

void ui_shell_init_message_handler(ui_shell_msg_handler_t *handler, const char *thread_name, uint16_t thread_size, uint32_t priority)
{
    //handler->getPriority = MessageHandler_getPriority;
    handler->sendMessage = MessageHandler_sendMessage;
    handler->send_delay_message = MessageHandler_sendDelayMessage;
    handler->handleMessage = MessageHandler_handleMessage;
    handler->removeMessages = MessageHandler_removeMessages;

    handler->m_MessageQueue = ui_shell_new_message_queue();
    handler->m_DelayMessageQueue = ui_shell_new_delay_message_queue();
#ifdef WIN32
    handler->m_thread_run_state = THREAD_STATE_STARTING;
#endif
    handler->m_semaPhore = ui_shell_al_semaphore_create_binary();
    MessageHandler_startThread(handler, thread_name, thread_size, priority);
}

#ifdef WIN32
void ui_shell_deinit_message_handler(ui_shell_msg_handler_t *handler)
{
    UI_SHELL_LOG_MSGID_E("before deinit_MessageHandler is called", 0);

    handler->m_thread_run_state = THREAD_STATE_STOPPING;
    ui_shell_al_semaphore_give(handler->m_semaPhore);
    while(handler->m_thread_run_state != THREAD_STATE_STOPPED) {
        ui_shell_al_sleep(1000);
    }
    if (handler->m_semaPhore) {
        ui_shell_al_destroy_semaphore(handler->m_semaPhore);
    }
    UI_SHELL_LOG_MSGID_E("after deinit_MessageHandler is called", 0);
    ui_shell_delete_message_queue(handler->m_MessageQueue);
    ui_shell_delete_delay_message_queue(handler->m_DelayMessageQueue);
}
#endif
