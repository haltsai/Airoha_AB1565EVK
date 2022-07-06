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
#include <windows.h>

#include "ui_shell_al_memory.h"
#include "ui_shell_al_task.h"

typedef struct {
    void *task_parameter;
    ui_shell_al_task_handler task_method;
}task_handler_t;

CRITICAL_SECTION cs;

uint32_t WINAPI ui_shell_al_pthread_handle_method(LPVOID parameters)
{
    task_handler_t *task = (task_handler_t*)parameters;
    ui_shell_al_task_handler task_method = task->task_method;
    void *task_parameter = task->task_parameter;

    ui_shell_al_free(parameters);

    task_method(task_parameter);

    return 0;
}


ui_shell_al_task_result_t ui_shell_al_create_task(ui_shell_al_task_handler task_method,
                                const char* task_name,
                                uint16_t task_depth,
                                void* task_parameters,
                                uint32_t task_priority,
                                ui_shell_al_task_handle* p_task_handle)
{
    HANDLE handle;
    task_handler_t *task;

    task = (task_handler_t*)ui_shell_al_malloc(sizeof(task_handler_t));
    task->task_parameter = task_parameters;
    task->task_method = task_method;

    handle = CreateThread(NULL, 0, ui_shell_al_pthread_handle_method, (void*)task, 0, NULL);
    CloseHandle(handle);

    return AL_TASK_RESULT_OK;
}

ui_shell_al_task_result_t ui_shell_al_create_task_with_stack(ui_shell_al_task_handler task_method,
                                            const char* task_name,
                                            uint16_t task_depth,
                                            void* task_parameters,
                                            uint32_t task_priority,
                                            ui_shell_al_task_handle* p_task_handle,
                                            uint8_t *stack_buffer,
                                            void *tcb_buffer)
{
    return ui_shell_al_create_task(task_method, task_name, task_depth, task_parameters, task_priority, p_task_handle);
}

void ui_shell_al_delete_task(ui_shell_al_task_handle task_handle)
{

}

uint32_t ui_shell_al_get_task_priority(void *task_handle)
{
    return 0;
}

void ui_shell_al_set_task_priority(void *task_handle, uint32_t priority)
{

}

SemaPhoreHandle_t ui_shell_al_semaphore_create_binary() {
    return (SemaPhoreHandle_t)CreateSemaphore(NULL, 0, 1, NULL);
}

void ui_shell_al_destroy_semaphore(SemaPhoreHandle_t semaphore_handle) {
    CloseHandle(semaphore_handle);
}

uint32_t ui_shell_al_semaphore_take(SemaPhoreHandle_t semaphore_handle) {
    return WaitForSingleObject(semaphore_handle, INFINITE) == WAIT_OBJECT_0 ? 1 : 0;
}

uint32_t ui_shell_al_semaphore_take_in_time(SemaPhoreHandle_t semaphore_handle, uint32_t block_time) {
    return WaitForSingleObject(semaphore_handle, block_time) == WAIT_OBJECT_0 ? 1 : 0;
}

uint32_t ui_shell_al_semaphore_give_from_isr(SemaPhoreHandle_t semaphore_handle) {
    return ReleaseSemaphore(semaphore_handle, 1, NULL);
}

uint32_t ui_shell_al_semaphore_give(SemaPhoreHandle_t semaphore_handle) {
    return ReleaseSemaphore(semaphore_handle, 1, NULL);
}

SemaPhoreHandle_t ui_shell_al_create_mutex()
{
    PRTL_CRITICAL_SECTION cri_section = (PRTL_CRITICAL_SECTION)ui_shell_al_malloc(sizeof(RTL_CRITICAL_SECTION));
    InitializeCriticalSection(cri_section);
    return (void *)cri_section;
}

void ui_shell_al_destroy_mutex(SemaPhoreHandle_t mutex)
{
    //pthread_mutex_destroy(mutex);
    DeleteCriticalSection((PRTL_CRITICAL_SECTION)mutex);
    if (mutex != NULL)
    {
        ui_shell_al_free(mutex);
        mutex = NULL;
    }
}

uint32_t ui_shell_al_mutex_lock(SemaPhoreHandle_t mutex)
{
    //pthread_mutex_lock(mutex);
    EnterCriticalSection((PRTL_CRITICAL_SECTION)mutex);
    return 1;
}

uint32_t ui_shell_al_mutex_unlock(SemaPhoreHandle_t mutex)
{
    //pthread_mutex_unlock(mutex);
    LeaveCriticalSection((PRTL_CRITICAL_SECTION)mutex);
    return 1;
}

uint32_t ui_shell_al_mutex_lock_from_isr(SemaPhoreHandle_t mutex)
{
    //pthread_mutex_lock(mutex);
    EnterCriticalSection((PRTL_CRITICAL_SECTION)mutex);
    return 1;
}

uint32_t ui_shell_al_mutex_unlock_from_isr(SemaPhoreHandle_t mutex)
{
    //pthread_mutex_unlock(mutex);
    LeaveCriticalSection((PRTL_CRITICAL_SECTION)mutex);
    return 1;
}

bool ui_shell_al_synchronized(uint32_t *mask) {
    PRTL_CRITICAL_SECTION cri_section = (PRTL_CRITICAL_SECTION)ui_shell_al_malloc(sizeof(RTL_CRITICAL_SECTION));
    InitializeCriticalSection(cri_section);
    *mask = (uint32_t)cri_section;
    EnterCriticalSection(cri_section);
    return true;
}

bool ui_shell_al_synchronized_end(uint32_t mask) {
    PRTL_CRITICAL_SECTION cri_section = (PRTL_CRITICAL_SECTION)mask;
    LeaveCriticalSection(cri_section);
    DeleteCriticalSection(cri_section);
    ui_shell_al_free(cri_section);
    return true;
}

void ui_shell_al_sleep(uint32_t sleep_time)
{
    Sleep(sleep_time);
}

void ui_shell_al_task_delay_until(uint32_t delay_time)
{
    ui_shell_al_sleep(delay_time);
}

