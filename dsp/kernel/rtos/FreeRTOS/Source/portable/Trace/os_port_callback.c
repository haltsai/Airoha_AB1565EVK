/*
 * FreeRTOS Kernel V10.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */


#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "os_port_callback.h"
#include "hal_gpt.h"
#include "hal_time_check.h"
#include "syslog.h"
#include "swla.h"
uint32_t runtime_counter_base = 0;

void vConfigureTimerForRunTimeStats(void)
{
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &runtime_counter_base);
}

uint32_t ulGetRunTimeCounterValue(void)
{
    uint32_t cur_count, duration;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &cur_count);

    hal_gpt_get_duration_count(runtime_counter_base, cur_count, &duration);

    return duration;
}

void osDumpHeapInfo(void)
{
    uint32_t mini_heap_free_size = xPortGetMinimumEverFreeHeapSize();

    /* dump heap mini-free size */
    LOG_MSGID_I(common, "The max free block size is [%d] byte, current heap free size is [%d] byte, Minimum heap Size is [%d] byte\n", 3
                                                                                            , xPortGetMaxFreeBlockSize()
                                                                                            , xPortGetFreeHeapSize()
                                                                                            , mini_heap_free_size);

    /* warning if lower than watermark */
    if(mini_heap_free_size < 2*1024 ){
        LOG_MSGID_W(common, "The heap margin is not enough, the mini-free is [%d] byte", 1, mini_heap_free_size);
    }
}

#ifdef AIR_CPU_MCPS_PRIORING_ENABLE
extern task_profiling_type_t port_xTaskRuntimeCounter[];
void osDumpTaskInfo(TaskHandle_t xTask)
{
    TaskStatus_t taskStatus;

    vTaskGetInfo( xTask, &taskStatus, pdTRUE, eInvalid);
    LOG_I(common, "Task[0x%x][%s] MinStack[%d] State[%d]\n"
                                                    , taskStatus.xHandle
                                                    , taskStatus.pcTaskName
                                                    , taskStatus.usStackHighWaterMark
                                                    , taskStatus.eCurrentState
                                                    );
}

void osStatusInfo(void)
{
    uint32_t i;
    for(i = 0; i < config_MAX_NUM_OF_TASK; i++)
    {
        if(port_xTaskRuntimeCounter[i].xTaskHandle != NULL)
        {
	        osDumpTaskInfo(port_xTaskRuntimeCounter[i].xTaskHandle);
        }
    }

    osDumpHeapInfo();
}
#include "hal_gpt_internal.h"
ATTR_TEXT_IN_IRAM void osDumpTaskCounter(void)
{
    hal_gpt_status_t gpt_ret;
    uint32_t active_tasks = 0;
    uint32_t cur_dump_time;
    task_profiling_type_t task_counter[config_MAX_NUM_OF_TASK];
    uint32_t irq_time[MAX_INTERRUPT_LEVEL];
    uint32_t mask;
    uint32_t i,j,duration;
    static uint32_t last_dump_time = 0;
    uint32_t freq;

    //SLA_CustomLogging("osp", SA_START);

    /* re-start the profiling timer */
    extern uint32_t os_profiling_gpt_handle;
    gpt_ret = hal_gpt_sw_start_timer_ms(os_profiling_gpt_handle, OS_PROFILING_GPT_PERIOD_MS, (hal_gpt_callback_t)osDumpTaskCounter, NULL);
    if(HAL_GPT_STATUS_OK != gpt_ret){
        LOG_MSGID_E(common, "os profiling timer is not re-started:%d", 1, gpt_ret);
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    {
        /* update irq interrupted time */
        vTaskUpdateCurrentTaskRunTimeCounter();

        /* record isr runtime */
        extern uint32_t irq_execution_time_us[];
        for(uint32_t i=0; i<MAX_INTERRUPT_LEVEL; i++){
            irq_time[i] = irq_execution_time_us[i];
            irq_execution_time_us[i] = 0;
        }
    }
    hal_nvic_restore_interrupt_mask(mask);

    /* update profiling time */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &cur_dump_time);
    hal_gpt_get_duration_count(last_dump_time, cur_dump_time, &duration);
    freq = *((volatile uint32_t *)HW_SYSRAM_PRIVATE_MEMORY_RTC_FREQ_START);

    LOG_MSGID_I(common, "[unit: 32k gpt count]last_dump_time: %d current_dump_time: %d, duration: %d, 32k clk: %d\n", 4, last_dump_time,cur_dump_time,duration,freq);
    last_dump_time = cur_dump_time;

    /* get and clear task execution counter */
    for(i = 0; i < config_MAX_NUM_OF_TASK; i++)
    {
        if(port_xTaskRuntimeCounter[i].xTaskHandle != NULL)
        {
            task_counter[i].xTaskHandle = port_xTaskRuntimeCounter[i].xTaskHandle;
            //task_counter[i].xRuntimeCount = (uint32_t) ((float)xTaskGetTaskRunTimeCounter(port_xTaskRuntimeCounter[i].xTaskHandle) * 30.5); /* 1/32768 = 30.517578125 us*/
            task_counter[i].xRuntimeCount = (uint32_t) (xTaskGetTaskRunTimeCounter(port_xTaskRuntimeCounter[i].xTaskHandle) * 305 / 10); /* 1/32768 = 30.517578125 us*/
            vTaskClearTaskRunTimeCounter(port_xTaskRuntimeCounter[i].xTaskHandle);
            active_tasks++;
        }
    }

    /* dump task execution counter info */
    for(j = 0; j < active_tasks; j += 3){
        if((j + 2) < active_tasks) {
            LOG_MSGID_I(common, "Task[0x%x]:RunTime[%8d us]  Task[0x%x]:RunTime[%8d us]  Task[0x%x]:RunTime[%8d us]\n",6,
                                    task_counter[j+0].xTaskHandle,
                                    task_counter[j+0].xRuntimeCount,
                                    task_counter[j+1].xTaskHandle,
                                    task_counter[j+1].xRuntimeCount,
                                    task_counter[j+2].xTaskHandle,
                                    task_counter[j+2].xRuntimeCount);
        } else if((j + 1) < active_tasks){
            LOG_MSGID_I(common, "Task[0x%x]:RunTime[%8d us]  Task[0x%x]:RunTime[%8d us]\n",4,
                                    task_counter[j+0].xTaskHandle,
                                    task_counter[j+0].xRuntimeCount,
                                    task_counter[j+1].xTaskHandle,
                                    task_counter[j+1].xRuntimeCount);
        } else {
            LOG_MSGID_I(common, "Task[0x%x]:RunTime[%8d us]\n",2,
                                    task_counter[j+0].xTaskHandle,
                                    task_counter[j+0].xRuntimeCount);
        }
    }

    #if 0
    /* dump low power time */
    extern uint32_t count_idle_time;
    extern uint32_t count_sleep_time;
    LOG_MSGID_I(common, "Idle time: [%8d], Sleep time: [%8d]\r\n", 2, count_idle_time, count_sleep_time);
    count_idle_time = 0;
    count_sleep_time = 0;
    #endif

    /* dump ISR execution counter info */
    extern uint32_t total_irq_frag_time_us;
    LOG_MSGID_I(common, "total %d irq level: Level1 ISR[%8d us] Level2 ISR[%8d us] Level3 ISR[%8d us] Level4 ISR[%8d us] irq frag[%8d us]\n", 6
                                                                            , MAX_INTERRUPT_LEVEL
                                                                            , irq_time[0]
                                                                            , irq_time[1]
                                                                            , irq_time[2]
                                                                            , irq_time[3]
                                                                            , total_irq_frag_time_us
                                                                            );
    total_irq_frag_time_us = 0x0;
    //SLA_CustomLogging("osp", SA_STOP);
}
#endif /* AIR_CPU_MCPS_PRIORING_ENABLE */
