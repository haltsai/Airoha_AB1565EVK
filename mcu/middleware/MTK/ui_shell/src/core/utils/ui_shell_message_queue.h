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

#ifndef __UI_SHELL_MESSAGE_QUEUE_H__
#define __UI_SHELL_MESSAGE_QUEUE_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "ui_shell_activity.h"

typedef struct _ui_shell_msg {
    uint8_t msg_priority;
    uint32_t msg_group;
    uint32_t msg_id;
    void* msg_data;
    size_t msg_data_len;
    void *msg_data2;
    size_t msg_data_len2;
    void (*special_free_extra_func)(void *extra_data);

    struct _ui_shell_msg* next_msg;
} ui_shell_msg_t;

typedef struct _ui_shell_msg_queue {
    ui_shell_msg_t* mTop;
    ui_shell_msg_t** mRemoveCursor;
    ui_shell_msg_t *mLastOfPriority[ACTIVITY_PRIORITY_HIGHEST + 1];
    uint32_t mLength;
    void* p_mutex;

    void (*pushMessage)(struct _ui_shell_msg_queue *message_queue,
            bool from_isr, ui_shell_msg_t *input_msg);
    ui_shell_msg_t* (*getTopMessage)(struct _ui_shell_msg_queue *message_queue);
    uint8_t (*getLength)(struct _ui_shell_msg_queue *message_queue);
    ui_shell_msg_t* (*removeTopMessage)(struct _ui_shell_msg_queue *message_queue);
    void (*outputMessage)(struct _ui_shell_msg_queue *message_queue);
    void (*removeMessages)(struct _ui_shell_msg_queue *message_queue, uint32_t group, uint32_t id);

} ui_shell_msg_queue_t;

void ui_shell_message_queue_init(ui_shell_msg_queue_t *message_queue);
ui_shell_msg_queue_t *ui_shell_new_message_queue();
void ui_shell_delete_message_queue(ui_shell_msg_queue_t *message_queue);
void ui_shell_init_input_msg(ui_shell_msg_t *input_msg,
        uint8_t priority,
        uint32_t group, uint32_t id,
        void *data, size_t data_len,
        void *data2, size_t data_len2,
        void (*special_free_extra_func)(void *extra_data));

#endif/**__UI_SHELL_MESSAGE_QUEUE_H__**/
