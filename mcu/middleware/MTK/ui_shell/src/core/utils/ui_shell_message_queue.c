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

#include "ui_shell_message_queue.h"

#include <stddef.h>
#include "ui_shell_al_error.h"
#include "ui_shell_al_log.h"
#include "ui_shell_al_memory.h"
#include "ui_shell_al_task.h"
#include "ui_shell_activity.h"

static uint8_t getLength(ui_shell_msg_queue_t *message_queue)
{
    return message_queue->mLength;
}

static ui_shell_msg_t* getTopMessage(ui_shell_msg_queue_t *message_queue)
{
    return message_queue->mTop;
}

static void outputMessage(ui_shell_msg_queue_t *message_queue)
{
    ui_shell_msg_t* msg = message_queue->mTop;

    UI_SHELL_LOG_MSGID_D("[outputMessage] mLength : %d\n", 1, message_queue->mLength);

    while (msg != NULL) {
        msg = msg->next_msg;
    }
}

static ui_shell_msg_t *removeTopMessage(ui_shell_msg_queue_t *message_queue)
{
    ui_shell_msg_t* temp = NULL;
    uint32_t sync_mask;

    UI_SHELL_LOG_MSGID_D("[popMessage] pop-ed message : %d\n", 1, message_queue->mTop ? message_queue->mTop->msg_group : -1);
    ui_shell_al_synchronized(&sync_mask);

    temp = message_queue->mTop;
    if (message_queue->mTop != NULL) {
        if (message_queue->mRemoveCursor == &(message_queue->mTop->next_msg)) {
            message_queue->mRemoveCursor = &message_queue->mTop;
        }
        if (message_queue->mLastOfPriority[message_queue->mTop->msg_priority] == message_queue->mTop) {
            message_queue->mLastOfPriority[message_queue->mTop->msg_priority] = NULL;
        }
        message_queue->mTop = message_queue->mTop->next_msg;
        message_queue->mLength--;
    } else {
        configASSERT((message_queue->mLength == 0 && message_queue->mTop == NULL)
                || (message_queue->mLength != 0 && message_queue->mTop != NULL));
    }
    ui_shell_al_synchronized_end(sync_mask);
    UI_SHELL_LOG_MSGID_D("[popMessage] mLength : %d\n", 1, message_queue->mLength);
    return temp;
}

static void pushMessage(ui_shell_msg_queue_t *message_queue,
        bool from_isr,
        ui_shell_msg_t *input_msg)
{
    ui_shell_msg_t* msg;
    //ui_shell_msg_t* _last = NULL;
    //ui_shell_msg_t* _current;
    uint32_t sync_mask;
    uint32_t i;

    if (input_msg == NULL) {
        return;
    }

    UI_SHELL_LOG_MSGID_D("[pushMessage] push message %x-%x, length : %d\n", 3,
            input_msg->msg_group, input_msg->msg_id, message_queue->mLength);
    msg = (ui_shell_msg_t *)ui_shell_al_malloc(sizeof(ui_shell_msg_t));
    if (msg == NULL) {
        UI_SHELL_LOG_MSGID_E("[pushMessage] push message failed, malloc msg is null", 0);
        return;
    }
    ui_shell_al_memcpy(msg, input_msg, sizeof(ui_shell_msg_t));
    msg->next_msg = NULL;
    // if current queue length is 0, a new message is coming
    ui_shell_al_synchronized(&sync_mask);

    for (i = msg->msg_priority; i <= ACTIVITY_PRIORITY_HIGHEST; i++){
        if (message_queue->mLastOfPriority[i] != NULL) {
            break;
        }
    }
    if (i <= ACTIVITY_PRIORITY_HIGHEST) {
        msg->next_msg = message_queue->mLastOfPriority[i]->next_msg;
        message_queue->mLastOfPriority[i]->next_msg = msg;
    } else {
        msg->next_msg = message_queue->mTop;
        message_queue->mTop = msg;
    }
    message_queue->mLastOfPriority[msg->msg_priority] = msg;
    message_queue->mLength ++;
    configASSERT((message_queue->mLength == 1 && message_queue->mTop == msg)
            || (message_queue->mLength > 1));
    ui_shell_al_synchronized_end(sync_mask);
    UI_SHELL_LOG_MSGID_D("[pushMessage] mLength : %d\n", 1, message_queue->mLength);
}

static void removeMessages(ui_shell_msg_queue_t *message_queue, uint32_t group, uint32_t id)
{
    uint32_t sync_mask;
    ui_shell_msg_t* _last = NULL;
    ui_shell_msg_t* _current;

    if (message_queue) {
        ui_shell_al_synchronized(&sync_mask);
        message_queue->mRemoveCursor = &message_queue->mTop;
        ui_shell_al_synchronized_end(sync_mask);
        for (;;) {
            ui_shell_al_synchronized(&sync_mask);
            if (!message_queue || !message_queue->mRemoveCursor) {
                ui_shell_al_synchronized_end(sync_mask);
                break;
            }
            _current = *message_queue->mRemoveCursor;
            if (_current) {
                if (_current->msg_group == group && _current->msg_id == id) {
                    *message_queue->mRemoveCursor = _current->next_msg;
                    if (_current == message_queue->mLastOfPriority[_current->msg_priority]) {
                        if (message_queue->mRemoveCursor == &message_queue->mTop) {
                            message_queue->mLastOfPriority[_current->msg_priority] = NULL;
                        } else {
                            _last = (ui_shell_msg_t *)((uint8_t *)message_queue->mRemoveCursor
                                    - (size_t)&((ui_shell_msg_t *)0)->next_msg);
                            if (_last->msg_priority == _current->msg_priority) {
                                message_queue->mLastOfPriority[_current->msg_priority] = _last;
                            } else {
                                message_queue->mLastOfPriority[_current->msg_priority] = NULL;
							}
                        }
                    }
                    message_queue->mLength--;
                    configASSERT((message_queue->mLength == 0 && message_queue->mTop == NULL)
                            || (message_queue->mLength != 0 && message_queue->mTop != NULL));
                    ui_shell_al_synchronized_end(sync_mask);
                    // Free Data in current
                    if (_current->msg_data && _current->msg_data_len) {
                        if (_current->special_free_extra_func) {
                            _current->special_free_extra_func(_current->msg_data);
                        }
                        ui_shell_al_free(_current->msg_data);
                    }

                    if (_current->msg_data2 && _current->msg_data_len2) {
                        ui_shell_al_free(_current->msg_data2);
                    }
                    ui_shell_al_free(_current);
                } else {
                    message_queue->mRemoveCursor = &_current->next_msg;
                    ui_shell_al_synchronized_end(sync_mask);
                }
            } else {
                message_queue->mRemoveCursor = NULL;
                ui_shell_al_synchronized_end(sync_mask);
                break;
            }
        }
    }
}

void ui_shell_message_queue_init(ui_shell_msg_queue_t *message_queue)
{
    message_queue->pushMessage = pushMessage;
    message_queue->getTopMessage = getTopMessage;
    message_queue->getLength = getLength;
    message_queue->removeTopMessage = removeTopMessage;
    message_queue->outputMessage = outputMessage;
    message_queue->removeMessages = removeMessages;
    message_queue->p_mutex = ui_shell_al_create_mutex();
}

ui_shell_msg_queue_t *ui_shell_new_message_queue()
{
    ui_shell_msg_queue_t *message_queue = (ui_shell_msg_queue_t *)ui_shell_al_malloc(sizeof(ui_shell_msg_queue_t));
    if (message_queue != NULL) {
        ui_shell_al_memset(message_queue, 0, sizeof(ui_shell_msg_queue_t));
        ui_shell_message_queue_init(message_queue);
    } else {
        UI_SHELL_LOG_MSGID_E("new_message_queue failed, malloc null", 0);
    }
    return message_queue;
}

void ui_shell_delete_message_queue(ui_shell_msg_queue_t *message_queue)
{
    uint32_t sync_mask;

    if (message_queue != NULL) {
        ui_shell_al_synchronized(&sync_mask);
        {
            ui_shell_msg_t* msg = message_queue->mTop;
            ui_shell_msg_t* next_msg = NULL;
            while (msg != NULL && message_queue->mLength != 0) {
                if (msg->msg_data && msg->msg_data_len) {
                    if (msg->special_free_extra_func) {
                        msg->special_free_extra_func(msg->msg_data);
                    }
                    ui_shell_al_free(msg->msg_data);
                }
                if (msg->msg_data2 && msg->msg_data_len2) {
                    ui_shell_al_free(msg->msg_data2);
                }
                next_msg = msg->next_msg;
                ui_shell_al_free(msg);
                msg = next_msg;
                message_queue->mLength--;
            }
        }
        message_queue->mLength = 0;
        message_queue->mTop = NULL;
        message_queue->mRemoveCursor = NULL;
        ui_shell_al_synchronized_end(sync_mask);
        ui_shell_al_destroy_mutex(message_queue->p_mutex);
        ui_shell_al_free(message_queue);
    }
}

void ui_shell_init_input_msg(ui_shell_msg_t *input_msg,
        uint8_t priority,
        uint32_t group, uint32_t id,
        void *data, size_t data_len,
        void *data2, size_t data_len2,
        void (*special_free_extra_func)(void *extra_data))
{
    if (input_msg) {
        input_msg->msg_priority = priority;
        input_msg->msg_group = group;
        input_msg->msg_id = id;
        input_msg->msg_data = data;
        input_msg->msg_data_len = data_len;
        input_msg->msg_data2 = data2;
        input_msg->msg_data_len2 = data_len2;
        input_msg->special_free_extra_func = special_free_extra_func;
        input_msg->next_msg = NULL;
    }
}
