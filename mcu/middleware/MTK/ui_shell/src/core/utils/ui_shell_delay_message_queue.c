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

#include "ui_shell_delay_message_queue.h"

#include <stddef.h>
#include "ui_shell_al_error.h"
#include "ui_shell_al_log.h"
#include "ui_shell_al_memory.h"
#include "ui_shell_al_task.h"
#include "ui_shell_al_timer.h"

static ui_shell_delay_msg_t *removeTimeoutMessages(ui_shell_delay_msg_queue_t *message_queue)
{
    ui_shell_delay_msg_t *temp;
    ui_shell_delay_msg_t *last_temp = NULL;
    ui_shell_delay_msg_t *ret_msgs = NULL;
    uint32_t sync_mask;
    uint32_t current_time = ui_shell_al_get_current_ms();
    size_t timeout_msg_count = 0;
    bool need_change_remove_cursor = false;

    UI_SHELL_LOG_MSGID_D("[removeTimeoutMessages] pop-ed message : %d, current_time = %d\n", 2,
            message_queue->parent.mTop ? message_queue->parent.mTop->msg_group : -1,
                    current_time);
    ui_shell_al_synchronized(&sync_mask);

    for (temp = (ui_shell_delay_msg_t *)message_queue->parent.mTop; temp; temp = (ui_shell_delay_msg_t *)temp->basic_msg.next_msg) {
        if (ui_shell_al_is_time_larger(current_time, temp->msg_act_time)) {
            timeout_msg_count++;
            last_temp = temp;
            if (message_queue->parent.mRemoveCursor == &(message_queue->parent.mTop)
                    || (message_queue->parent.mRemoveCursor == &(temp->basic_msg.next_msg))) {
                need_change_remove_cursor = true;
            }
        } else {
            break;
        }
    }

    if (timeout_msg_count > 0) {
        message_queue->parent.mLength -= timeout_msg_count;
        ret_msgs = (ui_shell_delay_msg_t *)message_queue->parent.mTop;
        message_queue->parent.mTop = &temp->basic_msg;
        if (need_change_remove_cursor) {
            message_queue->parent.mRemoveCursor = &(message_queue->parent.mTop);
        }
        if (last_temp) {
            last_temp->basic_msg.next_msg = NULL;
        }
    }
    configASSERT((message_queue->parent.mLength == 0 && message_queue->parent.mTop == NULL)
            || (message_queue->parent.mLength != 0 && message_queue->parent.mTop != NULL));
    ui_shell_al_synchronized_end(sync_mask);
    UI_SHELL_LOG_MSGID_D("[removeTimeoutMessages] mLength : %d\n", 1, message_queue->parent.mLength);
    return ret_msgs;
}

static uint32_t getNextTimeout(ui_shell_delay_msg_queue_t *message_queue)
{
    uint32_t sync_mask;
    uint32_t ret = 0;
    uint32_t current_time = ui_shell_al_get_current_ms();

    ui_shell_al_synchronized(&sync_mask);
    if (message_queue->parent.mTop) {
        if (ui_shell_al_is_time_larger(current_time, ((ui_shell_delay_msg_t *)(message_queue->parent.mTop))->msg_act_time)) {
            ret = 0;
        } else {
            ret = ((ui_shell_delay_msg_t *)(message_queue->parent.mTop))->msg_act_time - current_time;
        }
    } else {
        ret = 0xFFFFFFFF;
    }
    ui_shell_al_synchronized_end(sync_mask);
    return ret;
}

static void pushDelayMessage(ui_shell_delay_msg_queue_t *message_queue,
        ui_shell_delay_msg_t *input_msg)
{
    ui_shell_delay_msg_t* msg;
    ui_shell_delay_msg_t* _last = NULL;
    ui_shell_delay_msg_t* _current;
    uint32_t sync_mask;

    if (input_msg == NULL) {
        return;
    }

    UI_SHELL_LOG_MSGID_D("[pushMessage] push delay message %x-%x, act_time : %d, length : %d\n", 4,
            input_msg->basic_msg.msg_group, input_msg->basic_msg.msg_id, input_msg->msg_act_time, message_queue->parent.mLength);
    msg = (ui_shell_delay_msg_t *)ui_shell_al_malloc(sizeof(ui_shell_delay_msg_t));
    if (msg == NULL) {
        UI_SHELL_LOG_MSGID_E("[pushMessage] push dalay message failed, malloc msg is null", 0);
        return;
    }
    ui_shell_al_memcpy(msg, input_msg, sizeof(ui_shell_delay_msg_t));
    msg->basic_msg.next_msg = NULL;
    // if current queue length is 0, a new message is coming
    ui_shell_al_synchronized(&sync_mask);
    if (message_queue->parent.mTop == NULL) {
        message_queue->parent.mTop = &msg->basic_msg;
    } else {
        _current = (ui_shell_delay_msg_t *)message_queue->parent.mTop;
        while(1) {
            if (_current == NULL || ui_shell_al_is_time_larger(_current->msg_act_time, msg->msg_act_time)) {
                msg->basic_msg.next_msg = &_current->basic_msg;
                if (_last) {
                    _last->basic_msg.next_msg = &msg->basic_msg;
                } else {
                    message_queue->parent.mTop = &msg->basic_msg;
                }
                break;
            } else {
                _last = _current;
                _current = (ui_shell_delay_msg_t *)_current->basic_msg.next_msg;
            }
        }
    }
    message_queue->parent.mLength ++;
    configASSERT((message_queue->parent.mLength == 1 && message_queue->parent.mTop == &msg->basic_msg)
            || (message_queue->parent.mLength > 1));
    ui_shell_al_synchronized_end(sync_mask);
    UI_SHELL_LOG_MSGID_D("[pushDelayMessage] mLength : %d\n", 1, message_queue->parent.mLength);
}

ui_shell_delay_msg_queue_t *ui_shell_new_delay_message_queue()
{
    ui_shell_delay_msg_queue_t *message_queue = (ui_shell_delay_msg_queue_t *)ui_shell_al_malloc(sizeof(ui_shell_delay_msg_queue_t));
    if (message_queue != NULL) {
        ui_shell_al_memset(message_queue, 0, sizeof(ui_shell_delay_msg_queue_t));
        ui_shell_message_queue_init(&message_queue->parent);
        message_queue->pushDelayMessage = pushDelayMessage;
        message_queue->removeTimeoutMessages = removeTimeoutMessages;
        message_queue->getNextTimeout = getNextTimeout;
    } else {
        UI_SHELL_LOG_MSGID_E("new_message_queue failed, malloc null", 0);
    }
    return message_queue;
}

void ui_shell_delete_delay_message_queue(ui_shell_delay_msg_queue_t *message_queue)
{
    ui_shell_delete_message_queue(&message_queue->parent);
}

void ui_shell_init_input_delay_msg(ui_shell_delay_msg_t *input_msg,
        uint8_t priority,
        uint32_t group, uint32_t id,
        void *data, size_t data_len,
        void *data2, size_t data_len2,
        void (*special_free_extra_func)(void *extra_data),
        uint32_t delay_ms)
{
    if (input_msg) {
        ui_shell_init_input_msg(&input_msg->basic_msg, priority, group, id, data, data_len, data2, data_len2, special_free_extra_func);
        input_msg->msg_act_time = ui_shell_al_get_current_ms() + delay_ms;
    }
}
