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

#ifndef __UI_SHELL_STACK_MESSAGE_HANDLER_H__
#define __UI_SHELL_STACK_MESSAGE_HANDLER_H__

#include "ui_shell_message_handler.h"

enum {
    UI_SHELL_INTERNAL_EVENT_SYSTEM_START,
    UI_SHELL_INTERNAL_EVENT_SYSTEM_STOP,
    UI_SHELL_INTERNAL_EVENT_START_ACTI,
    UI_SHELL_INTERNAL_EVENT_FINISH_ACTI,
    UI_SHELL_INTERNAL_EVENT_SET_RESULT,
    UI_SHELL_INTERNAL_EVENT_TRIGGER_REFRESH,
    UI_SHELL_INTERNAL_EVENT_BACK_TO_IDLE,
    UI_SHELL_INTERNAL_EVENT_GET_ALLOWN,
    UI_SHELL_INTERNAL_EVENT_ALLOWED,
};

typedef struct _ui_shell_stack_message_handler {
    ui_shell_msg_handler_t handler;
#ifdef WIN32
    SemaPhoreHandle_t m_temp_semaPhore;
#endif
} ui_shell_stack_message_handler_t;

ui_shell_stack_message_handler_t *ui_shell_stack_message_handler_new(void);

#ifdef WIN32
void ui_shell_stack_message_handler_delete(ui_shell_stack_message_handler_t *stack_handler);
#endif

#endif /* __UI_SHELL_STACK_MESSAGE_HANDLER_H__ */
