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

#ifndef __UI_SHELL_MESSAGE_HANDLER_H__
#define __UI_SHELL_MESSAGE_HANDLER_H__

#include <stdbool.h>
#include "ui_shell_message_queue.h"
#include "ui_shell_delay_message_queue.h"
#include "ui_shell_al_task.h"

#define THREAD_STATE_STOPPED                (0)
#define THREAD_STATE_STARTING               (1)
#define THREAD_STATE_RUNNING                (2)
#define THREAD_STATE_STOPPING               (3)

typedef struct _ui_shell_msg_handler {
    //uint8_t (*getPriority)(uint16_t msgId);
    void (*sendMessage)(struct _ui_shell_msg_handler *handler,
            bool from_isr, ui_shell_msg_t *input_msg);
    void (*send_delay_message)(struct _ui_shell_msg_handler *handler, bool from_isr, ui_shell_delay_msg_t *input_msg);
    void (*handleMessage)(struct _ui_shell_msg_handler *handler, ui_shell_msg_t* msg);
    void (*removeMessages)(struct _ui_shell_msg_handler *handler, uint32_t event_group, uint32_t event_id);
    ui_shell_msg_queue_t *m_MessageQueue;
    ui_shell_delay_msg_queue_t *m_DelayMessageQueue;
    const char *thread_name;
#ifdef WIN32
    uint8_t m_thread_run_state;
#endif
    SemaPhoreHandle_t m_semaPhore;
} ui_shell_msg_handler_t;

//protected function
void ui_shell_init_message_handler(ui_shell_msg_handler_t *handler,
        const char *thread_name,
        uint16_t thread_size,
        uint32_t priority);

#ifdef WIN32
void ui_shell_deinit_message_handler(ui_shell_msg_handler_t *handler);
#endif

#endif
