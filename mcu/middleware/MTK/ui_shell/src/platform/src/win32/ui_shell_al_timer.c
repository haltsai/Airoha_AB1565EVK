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

//#include "stdafx.h"
#include <Windows.h>

#include "ui_shell_al_memory.h"
#include "ui_shell_al_task.h"
#include "ui_shell_al_timer.h"

typedef struct {
    char *timer_name;
    uint32_t timer_ms;
    uint32_t auto_reload;
    void *pv_timer_id;
    ui_shell_al_timer_handle_method callback;
    bool timer_start;
    bool timer_delete;
    bool timer_task_created;
} al_timer_thread_t;

static void al_timer_thread(void *arg)
{
    if (arg != NULL) {
        al_timer_thread_t *timer_handler = (al_timer_thread_t *)arg;
        while(!timer_handler->timer_delete) {
            ui_shell_al_sleep(timer_handler->timer_ms);
            if (timer_handler->timer_start) {
                timer_handler->callback(arg);
            }
        }
        ui_shell_al_free(arg);
    }
    ui_shell_al_delete_task(NULL);
}

void* ui_shell_al_timer_create(char *timer_name,
                           uint32_t timer_ms,
                           uint32_t auto_reload,
                           void *pv_timer_id,
                           ui_shell_al_timer_handle_method callback)
{
    al_timer_thread_t *timer_handler = (al_timer_thread_t *)ui_shell_al_malloc(sizeof(al_timer_thread_t));
    timer_handler->timer_name = timer_name;
    timer_handler->timer_ms = timer_ms;
    timer_handler->auto_reload = auto_reload;
    timer_handler->pv_timer_id = pv_timer_id;
    timer_handler->callback = callback;
    timer_handler->timer_start = false;
    timer_handler->timer_delete = false;
    timer_handler->timer_task_created = false;

    return timer_handler;
}


int32_t ui_shell_al_timer_start(void    *timer_handler,
                          uint16_t  time_to_wait)
{
    if (timer_handler != NULL) {
        ((al_timer_thread_t *)timer_handler)->timer_start = true;
        if (((al_timer_thread_t *)timer_handler)->timer_task_created == false) {
            ((al_timer_thread_t *)timer_handler)->timer_task_created = true;
            ui_shell_al_create_task(al_timer_thread, NULL, 1024, timer_handler, 4, NULL);
        }
    }
    return 1;
}

int32_t ui_shell_al_timer_stop(void      *timer_handler,
                         uint16_t    time_to_wait)
{
    // TODO: need mutex
    ((al_timer_thread_t *)timer_handler)->timer_start = false;
    return 1;
}

int32_t ui_shell_al_timer_delete(void    *timer_handler,
                           uint16_t  timer_to_wait)
{
    if (timer_handler != NULL) {
        ((al_timer_thread_t *)timer_handler)->timer_start = false;
        ((al_timer_thread_t *)timer_handler)->timer_task_created = false;
        ((al_timer_thread_t *)timer_handler)->timer_delete = true;
        return 1;
    } else {
        return 0;
    }
}

uint32_t ui_shell_al_get_current_ms()
{
    return GetTickCount();
}

bool ui_shell_al_is_time_larger(uint32_t a, uint32_t b)
{
    return ((a > b) && (a - b < (1 << 31))) || ((b > a) && (b - a > (1 << 31)));
}
