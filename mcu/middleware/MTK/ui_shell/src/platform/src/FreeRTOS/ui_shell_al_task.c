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

#include "ui_shell_al_task.h"

#include "ui_shell_al_log.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"
#include "task.h"
#include "hal_nvic.h"

#define DEFAULT_MUTEX_WAIT_TIME         (10000)


ui_shell_al_task_result_t ui_shell_al_create_task(ui_shell_al_task_handler task_method,
                                const char* task_name,
                                uint16_t task_depth,
                                void* task_parameters,
                                uint32_t task_priority,
                                ui_shell_al_task_handle* p_task_handle) {
    int32_t result = xTaskCreate((TaskFunction_t)task_method, task_name, task_depth / sizeof(portSTACK_TYPE),
            task_parameters, task_priority, (TaskHandle_t *)p_task_handle);
    if (pdPASS != result)
    {
        return AL_TASK_RESULT_FAILURE;
    }
    return AL_TASK_RESULT_OK;
}

ui_shell_al_task_result_t ui_shell_al_create_task_with_stack(ui_shell_al_task_handler task_method,
                                            const char* task_name,
                                            uint16_t task_depth,
                                            void* task_parameters,
                                            uint32_t task_priority,
                                            ui_shell_al_task_handle* p_task_handle,
                                            uint8_t *stack_buffer,
                                            void *tcb_buffer) {
#if defined(FREERTOS_VERSION) && (FREERTOS_VERSION == V10_AND_LATER)
    int32_t result;

    *p_task_handle = xTaskCreateStatic((TaskFunction_t)task_method, task_name, task_depth / sizeof(portSTACK_TYPE),
            task_parameters, task_priority, (StackType_t *)stack_buffer, (StaticTask_t *)tcb_buffer);

    result = (*p_task_handle != NULL)? pdPASS : pdFAIL;
#else
    (void)tcb_buffer;
    int32_t result = xTaskGenericCreate((TaskFunction_t)task_method, task_name, task_depth / sizeof(portSTACK_TYPE),
            task_parameters, task_priority, (TaskHandle_t *)p_task_handle, (StackType_t *)stack_buffer, NULL);
#endif /* #if (FREERTOS_VERSION == V10_AND_LATER) */
    if (pdPASS != result)
    {
        return AL_TASK_RESULT_FAILURE;
    }
    return AL_TASK_RESULT_OK;
}

void ui_shell_al_delete_task(void* task_handle) {
    vTaskDelete((TaskHandle_t)task_handle);
}

uint32_t ui_shell_al_get_task_priority(void *task_handle)
{
    return (uint32_t)uxTaskPriorityGet((TaskHandle_t)task_handle);
}

void ui_shell_al_set_task_priority(void *task_handle, uint32_t priority)
{
    vTaskPrioritySet(task_handle, priority);
}

SemaPhoreHandle_t ui_shell_al_semaphore_create_binary() {
    return (SemaPhoreHandle_t)xSemaphoreCreateBinary();
}

void ui_shell_al_destroy_semaphore(SemaPhoreHandle_t semaphore_handle) {
    vSemaphoreDelete(semaphore_handle);
}

uint32_t ui_shell_al_semaphore_take(SemaPhoreHandle_t semaphore_handle) {
    return (uint32_t)xSemaphoreTake(semaphore_handle, portMAX_DELAY );
}

uint32_t ui_shell_al_semaphore_take_in_time(SemaPhoreHandle_t semaphore_handle, uint32_t block_time) {
    return (uint32_t)xSemaphoreTake(semaphore_handle, (TickType_t)block_time / portTICK_PERIOD_MS);
}

uint32_t ui_shell_al_semaphore_give_from_isr(SemaPhoreHandle_t semaphore_handle) {
     BaseType_t  xHigherPriorityTaskWoken = pdFALSE;
    return (uint32_t)xSemaphoreGiveFromISR(semaphore_handle,&xHigherPriorityTaskWoken);
}

uint32_t ui_shell_al_semaphore_give(SemaPhoreHandle_t semaphore_handle) {
    return (uint32_t)xSemaphoreGive(semaphore_handle);
}

SemaPhoreHandle_t ui_shell_al_create_mutex() {
    return xSemaphoreCreateMutex();
}

void ui_shell_al_destroy_mutex(SemaPhoreHandle_t semaphore_handle) {
    vSemaphoreDelete(semaphore_handle);
}

uint32_t ui_shell_al_mutex_lock(SemaPhoreHandle_t semaphore_handle) {
    return (uint32_t)xSemaphoreTake(semaphore_handle, portMAX_DELAY );
}

uint32_t ui_shell_al_mutex_unlock(SemaPhoreHandle_t semaphore_handle) {
    return (uint32_t)xSemaphoreGive(semaphore_handle);
}

uint32_t ui_shell_al_mutex_lock_from_isr(SemaPhoreHandle_t semaphore_handle) {
    BaseType_t  xHigherPriorityTaskWoken = pdFALSE;
    return (uint32_t)xSemaphoreTakeFromISR(semaphore_handle,&xHigherPriorityTaskWoken);
}

uint32_t ui_shell_al_mutex_unlock_from_isr(SemaPhoreHandle_t semaphore_handle) {
    BaseType_t  xHigherPriorityTaskWoken = pdFALSE;
    return (uint32_t)xSemaphoreGiveFromISR(semaphore_handle,&xHigherPriorityTaskWoken);
}

bool ui_shell_al_synchronized(uint32_t *mask) {
    hal_nvic_status_t status = hal_nvic_save_and_set_interrupt_mask(mask);
    if (status == HAL_NVIC_STATUS_OK) {
        return true;
    } else {
        return false;
    }
}

bool ui_shell_al_synchronized_end(uint32_t mask) {
    hal_nvic_status_t status = hal_nvic_restore_interrupt_mask(mask);
    if (status == HAL_NVIC_STATUS_OK) {
        return true;
    } else {
        return false;
    }
}

void ui_shell_al_sleep(uint32_t sleep_time) {
    vTaskDelay(sleep_time / portTICK_RATE_MS);
}

void ui_shell_al_task_delay_until(uint32_t delay_time) {
    TickType_t xLastWakeTime;

    xLastWakeTime = xTaskGetTickCount();
    vTaskDelayUntil(&xLastWakeTime, delay_time / portTICK_RATE_MS);
}

void* ui_shell_al_create_queue(const uint32_t uxQueueLength, const uint32_t uxItemSize) {
    return xQueueCreate(uxQueueLength, uxItemSize);
}

void ui_shell_al_delete_queue(void* xQueue){

	vQueueDelete(xQueue);
}

void ui_shell_al_send_data_to_queue_from_isr( void* xQueue,
                            const void* const pvItemToQueue,
                            int32_t * const pxHigherPriorityTaskWoken) {
    xQueueSendFromISR(xQueue, pvItemToQueue, pxHigherPriorityTaskWoken);
}

ui_shell_al_task_result_t ui_shell_al_receive_data_from_queue(void* xQueue, void * const pvBuffer, uint32_t xTicksToWait) {
    if (pdPASS != xQueueReceive(xQueue, pvBuffer, xTicksToWait))
    {
        return AL_TASK_RESULT_FAILURE;
    }
    return AL_TASK_RESULT_OK;
}


