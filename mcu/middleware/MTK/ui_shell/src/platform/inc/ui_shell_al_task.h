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

#ifndef __UI_SHELL_AL_TASK_H__
#define __UI_SHELL_AL_TASK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// this typedef should be different according to platform
// pThread is typedef void* (*task_handler)(void *);
// The following typedef is FreeRTOS define
typedef void (*ui_shell_al_task_handler)(void *);
typedef void * ui_shell_al_task_handle;
typedef void * SemaPhoreHandle_t;

typedef enum {
    AL_TASK_RESULT_OK               = 0x01,
    AL_TASK_RESULT_FAILURE          = 0x02,
    AL_TASK_RESULT_MEMORY_ERROR     = 0x10
    // maybe some other errors
}ui_shell_al_task_result_t;

typedef enum {
    AL_TASK_RUNNING                 = 0x00,
    AL_TASK_READY                   = 0x01,
    AL_TASK_BLOCKED                 = 0x02,
    AL_TASK_SUSPENDED               = 0x03,
    AL_TASK_DELETED                 = 0x04
}ui_shell_al_task_state_t;

/********************************************************************************
 * Create a task to run
 * @param :
 *      task_method : The method to run the
 ********************************************************************************/
ui_shell_al_task_result_t ui_shell_al_create_task(ui_shell_al_task_handler task_method,
                                const char* task_name,
                                uint16_t task_depth,
                                void* task_parameters,
                                uint32_t task_priority,
                                ui_shell_al_task_handle* p_task_handle);

/********************************************************************************
 * Create a task to run, task stack is the parameter
 * @param :
 *      task_method : The method to run the
 ********************************************************************************/
ui_shell_al_task_result_t ui_shell_al_create_task_with_stack(ui_shell_al_task_handler task_method,
                                            const char* task_name,
                                            uint16_t task_depth,
                                            void* task_parameters,
                                            uint32_t task_priority,
                                            ui_shell_al_task_handle* p_task_handle,
                                            uint8_t *stack_buffer,
                                            void *tcb_buffer);

/************************************************************************
 * Quit the task which you created by ui_shell_al_create_task method
 ************************************************************************/
void ui_shell_al_delete_task(ui_shell_al_task_handle task_handle);

/********************************************************************************
 * Get the task priority according to task_handle
 * 
 * @param : 
 *      task_handle : The task_handle is from ui_shell_al_create_task method
 *******************************************************************************/
uint32_t ui_shell_al_get_task_priority(void* task_handle);


/********************************************************************************
 * Set/Update the task priority
 *
 * @param : 
 *      task_handle : The task_handle is from ui_shell_al_create_task method
 *      task_new_priority : The specific task new priority 
 *******************************************************************************/
void ui_shell_al_set_task_priority(void* task_handle, uint32_t task_new_priority);


/********************************************************************************
 * Get the task state according to task_handle
 *
 * @param : 
 *      task_handle : the task_handle is inited in the ui_shell_al_create_task method
 *
 * @return
 *      return the task state
 *******************************************************************************
ui_shell_al_task_state_t ui_shell_al_get_task_state(void* task_handle);
*/

/********************************************************************************
 * Get the current task handle
 *
 * @return
 *      return the task_handle which inited in the ui_shell_al_create_task method
 *******************************************************************************
void* ui_shell_al_get_current_task_handle();
*/

/********************************************************************************
 * Create a binary semaphore
 ********************************************************************************/
    SemaPhoreHandle_t ui_shell_al_semaphore_create_binary();

/********************************************************************************
 * Destroy a semaphore
 ********************************************************************************/
void ui_shell_al_destroy_semaphore(SemaPhoreHandle_t semaphore_handle);
/********************************************************************************
 *take semaphore
 ********************************************************************************/
uint32_t ui_shell_al_semaphore_take(SemaPhoreHandle_t semaphore_handle);
/********************************************************************************
 *take semaphore in block_time
 ********************************************************************************/
uint32_t ui_shell_al_semaphore_take_in_time(SemaPhoreHandle_t semaphore_handle, uint32_t block_time);
/********************************************************************************
 * give semaphore
 ********************************************************************************/
uint32_t ui_shell_al_semaphore_give(SemaPhoreHandle_t semaphore_handle);

/********************************************************************************
 * give semaphore from isr
 ********************************************************************************/
uint32_t ui_shell_al_semaphore_give_from_isr(SemaPhoreHandle_t semaphore_handle);

/********************************************************************************
 * Create a mutex
 ********************************************************************************/
SemaPhoreHandle_t ui_shell_al_create_mutex();

/********************************************************************************
 * Destroy a mutex
 ********************************************************************************/
void ui_shell_al_destroy_mutex(SemaPhoreHandle_t mutex);

/********************************************************************************
 * Lock begin according to mutex
 ********************************************************************************/
uint32_t ui_shell_al_mutex_lock(SemaPhoreHandle_t mutex);

/********************************************************************************
 * Unlock according to mutex
 ********************************************************************************/
uint32_t ui_shell_al_mutex_unlock(SemaPhoreHandle_t mutex);

/********************************************************************************
 * Lock begin according to mutex from ISR
 ********************************************************************************/
uint32_t ui_shell_al_mutex_lock_from_isr(SemaPhoreHandle_t semaphore_handle);

/********************************************************************************
 * Unlock according to mutex from ISR
 ********************************************************************************/
uint32_t ui_shell_al_mutex_unlock_from_isr(SemaPhoreHandle_t semaphore_handle);

/********************************************************************************
 * Disable all interrupt and task switch
 ********************************************************************************/
bool ui_shell_al_synchronized(uint32_t *mask);

/********************************************************************************
 * The end of disable all interrupt and task switch
 ********************************************************************************/
bool ui_shell_al_synchronized_end(uint32_t mask);

/********************************************************************************
 * Sleep time ms
 ********************************************************************************/
void ui_shell_al_sleep(uint32_t sleep_time);

/********************************************************************************
 * Delay a task until a specified time. This function can be used by periodic tasks to ensure a
 * constant execution frequency.
 ********************************************************************************/
void ui_shell_al_task_delay_until(uint32_t delay_time);

/********************************************************************************
* Creates a new queue instance.  This allocates the storage required by the
 * new queue and returns a handle for the queue.
 *
 * @param uxQueueLength The maximum number of items that the queue can contain.
 *
 * @param uxItemSize The number of bytes each item in the queue will require.
 * Items are queued by copy, not by reference, so this is the number of bytes
 * that will be copied for each posted item.  Each item on the queue must be
 * the same size.
 *
 * @return If the queue is successfully create then a handle to the newly
 * created queue is returned.  If the queue cannot be created then 0 is
 * returned.
 ********************************************************************************/
void* ui_shell_al_create_queue(const uint32_t uxQueueLength,
                           const uint32_t uxItemSize);

/********************************************************************************
* Post an item to the back of a queue.  It is safe to use this function from
 * within an interrupt service routine.
 *
 * Items are queued by copy not reference so it is preferable to only
 * queue small items, especially when called from an ISR.  In most cases
 * it would be preferable to store a pointer to the item being queued.
 *
 * @param xQueue The handle to the queue on which the item is to be posted.
 *
 * @param pvItemToQueue A pointer to the item that is to be placed on the
 * queue.  The size of the items the queue will hold was defined when the
 * queue was created, so this many bytes will be copied from pvItemToQueue
 * into the queue storage area.
 *
 * @param pxHigherPriorityTaskWoken xQueueSendFromISR() will set
 * *pxHigherPriorityTaskWoken to pdTRUE if sending to the queue caused a task
 * to unblock, and the unblocked task has a priority higher than the currently
 * running task.  If xQueueSendFromISR() sets this value to pdTRUE then
 * a context switch should be requested before the interrupt is exited.
 *
 * @return pdTRUE if the data was successfully sent to the queue, otherwise
 * errQUEUE_FULL.
 ********************************************************************************/
void ui_shell_al_send_data_to_queue_from_isr( void* xQueue,
                            const void* const pvItemToQueue,
                            int32_t * const pxHigherPriorityTaskWoken
                            );

/********************************************************************************
* Receive an item from a queue.  The item is received by copy so a buffer of
 * adequate size must be provided.  The number of bytes copied into the buffer
 * was defined when the queue was created.
 *
 * Successfully received items are removed from the queue.
 *
 * This function must not be used in an interrupt service routine.  See
 * xQueueReceiveFromISR for an alternative that can.
 *
 * @param xQueue The handle to the queue from which the item is to be
 * received.
 *
 * @param pvBuffer Pointer to the buffer into which the received item will
 * be copied.
 *
 * @param xTicksToWait The maximum amount of time the task should block
 * waiting for an item to receive should the queue be empty at the time
 * of the call.	 xQueueReceive() will return immediately if xTicksToWait
 * is zero and the queue is empty.  The time is defined in tick periods so the
 * constant portTICK_PERIOD_MS should be used to convert to real time if this is
 * required.
 *
 * @return pdTRUE if an item was successfully received from the queue,
 * otherwise pdFALSE.
 ********************************************************************************/
ui_shell_al_task_result_t ui_shell_al_receive_data_from_queue(void* xQueue,
                             void * const pvBuffer,
                             uint32_t xTicksToWait);


#ifdef __cplusplus
    }
#endif

#endif /* __UI_SHELL_AL_TASK_H__ */
