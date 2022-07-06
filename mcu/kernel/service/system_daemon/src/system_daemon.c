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
#include <stdint.h>
#include <assert.h>
#include "system_daemon.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "hal.h"
#ifdef SYSTEM_DAEMON_TASK_ENABLE


log_create_module(daemon, PRINT_LEVEL_WARNING);

/* system daemon task is created in port.c
    the task priority: (TASK_PRIORITY_IDLE + 1)
    the stack size is: 2048 byte
*/
TaskHandle_t system_daemon_task_handle = NULL;

/* system daemon task message type */
typedef struct {
    uint32_t tick;                          /**< the tick when receive the message, for debug purpose */
    system_daemon_event_t event_id;         /**< event id to mark the message source */
    const void * pdata;                     /**< user data area */
} system_daemon_queue_t;

/* system daemon task's queue handle*/
static QueueHandle_t system_daemon_queue_handle = NULL;

BaseType_t system_daemon_send_message(system_daemon_event_t event_id, const void *const pitem)
{
    system_daemon_queue_t queue_item;
    BaseType_t ret = pdFALSE;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    assert(system_daemon_queue_handle);

    /* fill in queue message */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(queue_item.tick));
    queue_item.event_id = event_id;
    queue_item.pdata = pitem;

    /* send messge to system daemon task */
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > 0) {
        /* is ISR context */
        ret = xQueueSendFromISR(system_daemon_queue_handle, &queue_item, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    } else {
        /* is Task context */
        ret = xQueueSend(system_daemon_queue_handle, &queue_item, 0);
        if (ret != pdTRUE){
            UBaseType_t avaliable_item_number = uxQueueSpacesAvailable((const QueueHandle_t)system_daemon_queue_handle);
            DAEMON_TASK_LOG_W(daemon, "send msg failed. avaliable item number is %u", 1, avaliable_item_number);
        }
    }

    return ret;
}


/* The initial code of system daemon task. */
void system_daemon_task_init(void)
{
    /* create queue */
    system_daemon_queue_handle = xQueueCreate(SYSTEM_DAEMON_QUEUE_LENGTH, sizeof(system_daemon_queue_t));
    assert(system_daemon_queue_handle);
}

#ifdef MTK_MUX_ENABLE
extern bool g_log_resp_push;
#endif
/* system daemon task's main loop */
void system_daemon_task(void *arg)
{
    system_daemon_queue_t queue_item;

    /* dummy reference */
    (void)system_daemon_task_handle;

    /* task main loop */
    while (1) {
        if (xQueueReceive(system_daemon_queue_handle, &queue_item, portMAX_DELAY)) {
            DAEMON_TASK_LOG_W(daemon, "receive event id: %d", 1, queue_item.event_id);
            switch (queue_item.event_id) {
                #ifdef MTK_NVDM_ENABLE
                case SYSTEM_DAEMON_ID_NVDM: {
                    extern void system_daemon_nvdm_msg_handler(void);
                    /* do nvdm message handling at here */
                    system_daemon_nvdm_msg_handler();
                    break;
                }
                #endif

                #if defined(MTK_NVDM_ENABLE) && defined(MTK_BT_AT_COMMAND_ENABLE)
                case SYSTEM_DAEMON_ID_DUMPALL_NVDM_ITEMS:{
                    extern void system_daemon_dump_all_nvdm_items_msg_handler( void );
                    system_daemon_dump_all_nvdm_items_msg_handler();
                    break;
                }
                #endif /* MTK_NVDM_ENABLE && MTK_BT_AT_COMMAND_ENABLE */

                case SYSTEM_DAEMON_ID_LOGGING_TO_FLASH: {
                    /* do logging to flash message handling at here */
                    //system_daemon_logging_to_flash_msg_handler(queue_item.pdata);

                    #ifdef MTK_MUX_ENABLE
                    if (g_log_resp_push) {
                        DAEMON_TASK_LOG_W(daemon, "log push", 0);
                    }
                    g_log_resp_push = false;
                    #endif					
                    break;
                }

                #if defined(HAL_RTC_MODULE_ENABLED) && defined(HAL_RTC_FEATURE_RTC_MODE)
                case SYSTEM_DAEMON_ID_ENTER_RTC_MODE:{
                    /* enter rtc mode & output warning log if its return NOT OK */
                    DAEMON_TASK_LOG_W(daemon, "enter RTC mode at daemon task", 0);
                    hal_rtc_status_t rtc_status = hal_rtc_enter_rtc_mode();
                    if (rtc_status != HAL_RTC_STATUS_OK){
                        DAEMON_TASK_LOG_W(daemon, "hal_rtc_enter_rtc_mode returned %d", 1, rtc_status);
                    }
                    break;
                }
                #endif /* HAL_RTC_MODULE_ENABLED && HAL_RTC_FEATURE_RTC_MODE */

                #ifdef HAL_WDT_MODULE_ENABLED
                case SYSTEM_DAEMON_ID_SW_RESET:{
                    /* software reset & output warning log if its return NOT OK */
                    DAEMON_TASK_LOG_W(daemon, "WDT SW Reset at daemon task", 0);
                    hal_wdt_status_t wdt_status = hal_wdt_software_reset();
                    if (wdt_status != HAL_WDT_STATUS_OK){
                        DAEMON_TASK_LOG_W(daemon, "hal_wdt_software_reset returned %d", 1, wdt_status);
                    }
                    break;
                }
                #endif /* HAL_WDT_MODULE_ENABLED */

                default: {
                    /* illegal event id, do error handling */
                    assert(0);
                }
            }
        }
    }
}

#endif /* SYSTEM_DAEMON_TASK_ENABLE */
