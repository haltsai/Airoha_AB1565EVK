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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM4F port.
 *----------------------------------------------------------*/

#include "FreeRTOS.h"
#include "port_tick.h"
#include "hal_clock.h"
#include "hal_nvic.h"
#include "timers.h"
#include "task.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "hal_resource_assignment.h"

#if configUSE_TICKLESS_IDLE == 2
#include "task.h"
#include "memory_attribute.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#include "core_cm4.h"
#include "hal_rtc.h"
#include "hal_dvfs.h"
#include "hal_eint.h"
#include "hal_wdt.h"
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
#include "systemhang_tracer.h"
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
#include "assert.h"
#endif

#define MaximumIdleTime 10  //ms
#define DEEP_SLEEP_HW_WAKEUP_TIME 2
#define DEEP_SLEEP_SW_BACKUP_RESTORE_TIME 2
#define REMAINDER_LIMIT 100*1000    //100ms
#define SYSTEM_HANG_CHECK_LATENCY_TIME 1

//#define TICKLESS_DEEBUG_ENABLE
#ifdef  TICKLESS_DEEBUG_ENABLE
#define log_debug_tickless(_message,...) log_hal_info(_message, ##__VA_ARGS__)
#else
#define log_debug_tickless(_message,...)
#endif

#if configUSE_TICKLESS_IDLE != 0
/*
 * The number of OS GPT increments that make up one tick period.
 */
static uint32_t ulTimerCountsForOneTick = 0;

/*
 * The maximum number of tick periods that can be suppressed is limited by the
 * 32 bit resolution of the OS GPT timer.
 */
static uint32_t xMaximumPossibleSuppressedTicks = 0;
#endif

volatile uint32_t systick_change_period = 0;
volatile uint32_t tick_remainder = 0;

float RTC_Freq = 32.768f; /* RTC 32.768KHz Freq*/
#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
extern uint32_t eint_get_status(void);
uint32_t wakeup_eint;
#endif

static uint32_t count_idle_time_us;
static uint32_t count_sleep_time_us;
static bool count_idle = FALSE;
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------------------------
#include "hal_nvic.h"
#include <assert.h>
//#include "mt2822.h"

GPT_REGISTER_T *os_gpt0 = OS_GPT0;
OS_GPT_REGISTER_GLOABL_T *os_gpt_glb = OS_GPTGLB;
#define OST_COUNT_CLEAR         0x1
#define OST_COUNT_START         0x1
#define OST_IRQ_ENABLE          0x1
#define OST_IRQ_FLAG_ACK        0x1
#define OST_CLOCK_32KHZ         (0x10)
#define OST_CLOCK_13MHZ         (0x00)

typedef enum {
    OS_GPT_TIMER_TYPE_ONE_SHOT = 0,                /**< Set the GPT oneshot mode.  */
    OS_GPT_TIMER_TYPE_REPEAT   = 1                 /**< Set the GPT repeat  mode.  */
} os_gpt_timer_type_t;

typedef enum {
    OS_GPT_CLOCK_SOURCE_32K = 0,            /**< Set the GPT clock source to 32kHz, 1 tick = 1/32768 seconds. */
    OS_GPT_CLOCK_SOURCE_1M  = 1             /**< Set the GPT clock source to 1MHz, 1 tick = 1 microsecond.*/
} os_gpt_clock_source_t;

void os_gpt_interrupt_handle(hal_nvic_irq_t irq_number);
void os_gpt_init(void *callback)
{
    hal_nvic_disable_irq(OS_GPT_IRQn);
    hal_nvic_register_isr_handler(OS_GPT_IRQn, os_gpt_interrupt_handle);
}

void os_gpt_start(uint32_t time_out_us , os_gpt_timer_type_t timer_type , os_gpt_clock_source_t clock_source , float rtc_freq)
{
    os_gpt_timer_type_t ost_type = timer_type;
    os_gpt_clock_source_t ost_source = clock_source;
    uint32_t time_out_us_reload;

    time_out_us_reload = (ost_type == OS_GPT_TIMER_TYPE_ONE_SHOT)? time_out_us : (time_out_us - 1);

    if ((time_out_us_reload / 1000) > HAL_GPT_MAXIMUM_MS_TIMER_TIME) {
        assert(0);
    }
    //  os_gpt0->GPT_IRQ_ACK = 0x1;                   //clear interrupt status
    os_gpt0->GPT_CON_UNION.GPT_CON |= (1 << 16);  // disabled the clock source

    /* set 13 divided with 13M source */
    if (ost_source == OS_GPT_CLOCK_SOURCE_32K) {       //set clock source
        os_gpt0->GPT_CON_UNION.GPT_CON |= (1 << 16);   // disable clock before config
        os_gpt0->GPT_CLK = 0x10;      //set 32k divide 1
        os_gpt0->GPT_CON_UNION.GPT_CON &= ~(1 << 16);   // enable clock

        os_gpt0->GPT_COMPARE = (uint32_t)(((float)(time_out_us_reload/1000)) *rtc_freq); // calculate the 32k count.

        os_gpt0->GPT_CLR      = 0x1;                    // clear the count
        while (os_gpt_glb->OS_GPT_CLRSTA & (1<<0));
    } else {
        os_gpt0->GPT_CON_UNION.GPT_CON |= (1 << 16);   // disable clock before config
        os_gpt0->GPT_CLK = 0xc;     //set  13M divide 13
        os_gpt0->GPT_CON_UNION.GPT_CON &= ~(1 << 16);   // enable clock

        os_gpt0->GPT_COMPARE = time_out_us_reload;
        os_gpt0->GPT_CLR      = 0x1;                    // clear the count
        while (os_gpt_glb->OS_GPT_CLRSTA & (1<<0));
        (void)rtc_freq;
    }

    while (os_gpt_glb->OS_GPT_WCOMPSTA & (1<<0));

    if (ost_type == OS_GPT_TIMER_TYPE_ONE_SHOT) {
        os_gpt0->GPT_CON_UNION.GPT_CON &= ~(3 << 8);    //set one-shot mode
        os_gpt_glb->OS_GPT_IRQMSK_CM4 &= 0x2;           //CM4 IRQ enable
        os_gpt_glb->OS_GPT_WAKEUPMSK_CM4 &= 0x2;        //CM4 wakeup enable
    } else {
        os_gpt0->GPT_CON_UNION.GPT_CON |= 0x101;        // set repeat mode
        os_gpt_glb->OS_GPT_IRQMSK_CM4 &= 0x2;           //CM4 IRQ enable
        os_gpt_glb->OS_GPT_WAKEUPMSK_CM4 &= 0x2;      //CM4 wakeup enable
    }

    /* enable IRQ */
    os_gpt0->GPT_IRQ_EN = 0x1;

    /* register and enable IRQ */
    hal_nvic_enable_irq(OS_GPT_IRQn);

    os_gpt0->GPT_CON_UNION.GPT_CON |= 0x01;
    return;
}

uint32_t os_gpt_stop()
{
    /*diable interrupt*/
    os_gpt0->GPT_IRQ_EN = ~OST_IRQ_ENABLE  ;
    /* stop timer */
    os_gpt0->GPT_CON_UNION.GPT_CON_CELLS.EN &= ~OST_COUNT_START;
    os_gpt0->GPT_IRQ_ACK = OST_IRQ_FLAG_ACK ;                    /* clean interrupt status */
    os_gpt0->GPT_CON_UNION.GPT_CON = 0;            /* disable timer     */

    NVIC_DisableIRQ(OS_GPT_IRQn);
    NVIC_ClearPendingIRQ(OS_GPT_IRQn);

    return os_gpt0->GPT_COUNT;
}

extern void xPortSysTickHandler(void);
void os_gpt_interrupt_handle(hal_nvic_irq_t irq_number)
{
    os_gpt0->GPT_IRQ_ACK = 0x01;

    if (systick_change_period != 0) {
        systick_change_period = 0;
        os_gpt_start(portTICK_PERIOD_MS * 1000 , OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);
    }
    /* Run FreeRTOS tick handler*/
    xPortSysTickHandler();
}

#if configUSE_TICKLESS_IDLE == 2
ATTR_TEXT_IN_TCM void tickless_handler(uint32_t xExpectedIdleTime)
{
    static long unsigned int after, idle = 0;
    static uint32_t ulCompleteTickPeriods, before_idle_time, remain, ulAST_Reload_ms, last_remain;
    static uint32_t after_sleep_time, before_sleep_time, sleep_time = 0;
    static uint32_t before_time[2], after_time[2], duration_time[2];
    static float calculation;

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __asm volatile("cpsid i");

#ifdef SLEEP_MANAGEMENT_CM4_SLEEP_DISABLE
    hal_wdt_feed(HAL_WDT_FEED_MAGIC);

    __asm volatile("dsb");
    __asm volatile("wfi");
    __asm volatile("isb");

    if (count_idle) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&after);

        if (after >= before_idle_time) {
            idle = after - before_idle_time;
        } else {
            idle = after + (0xFFFFFFFF - before_idle_time);
        }
        count_idle_time_us += idle;
    }
    __asm volatile("cpsie i");
    return;
#else
    if ((hal_sleep_manager_is_sleep_handle_alive(SLEEP_LOCK_BT_CONTROLLER) == true) || (hal_sleep_manager_is_sleep_handle_alive(SLEEP_LOCK_BT_CONTROLLER_A2DP) == true)) {

        hal_wdt_feed(HAL_WDT_FEED_MAGIC);

        __asm volatile("dsb");
        __asm volatile("wfi");
        __asm volatile("isb");

        if (count_idle) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&after);

            if (after >= before_idle_time) {
                idle = after - before_idle_time;
            } else {
                idle = after + (0xFFFFFFFF - before_idle_time);
            }
            count_idle_time_us += idle;
        }
        __asm volatile("cpsie i");
        return;
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&before_time[0]);

    if (systick_change_period != 0) {
        systick_change_period = 0;
        tick_remainder += os_gpt_stop();
        tick_remainder += last_remain;
    } else {
        tick_remainder += os_gpt_stop();
    }

    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        systick_change_period = 0;
        /* Restart OS GPT. */
        os_gpt_start(portTICK_PERIOD_MS * 1000 , OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);

        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile("cpsie i");
        return;
    } else {

        if ((xExpectedIdleTime > (MaximumIdleTime / (1000 / configTICK_RATE_HZ))) && (hal_sleep_manager_is_sleep_locked() == 0)) {

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
			/* make sure the sleep time does not overflow the wdt limitation. */
			uint32_t sleep_time_sec = ((xExpectedIdleTime) / configTICK_RATE_HZ);
			if (sleep_time_sec > (HAL_WDT_MAX_TIMEOUT_VALUE - 10))
			{
				sleep_time_sec = (HAL_WDT_MAX_TIMEOUT_VALUE - 10);
				xExpectedIdleTime = sleep_time_sec * configTICK_RATE_HZ;
				/* maybe xExpectedIdleTime is still larger than OS GPT reload value, it will be cut again */
				/* in any case, wdt timeout value must be larger than sleep time */
			}

            /* in here, sleep_time_sec is between 0 and HAL_WDT_MAX_TIMEOUT_VALUE - 10 */
            extern void hal_wdt_enter_sleep(uint32_t seconds);

            /* disable wdt and config wdt into reset mode */
            hal_wdt_enter_sleep(sleep_time_sec + 10);
            /* after here, wdt is in reset mode for prevent sleep flow hang */
#endif 

			/* Tickless SLEEP mode*/
            /* Calculate total idle time to ms */
            ulAST_Reload_ms = (xExpectedIdleTime - 1) * (1000 / configTICK_RATE_HZ);
            ulAST_Reload_ms = ulAST_Reload_ms - DEEP_SLEEP_SW_BACKUP_RESTORE_TIME - DEEP_SLEEP_HW_WAKEUP_TIME;

            os_gpt_start(ulAST_Reload_ms * 1000 , OS_GPT_TIMER_TYPE_ONE_SHOT , OS_GPT_CLOCK_SOURCE_32K , RTC_Freq);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &before_sleep_time);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&after_time[0]);

            /* Enter Sleep mode */
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);

			
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&before_time[1]);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &after_sleep_time);
            /* stop GPT */
            os_gpt_stop();
            hal_gpt_get_duration_count(before_sleep_time, after_sleep_time, &sleep_time);

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
            /* restore wdt status to the configuration before sleep */
            extern void hal_wdt_exit_sleep(void);
            hal_wdt_exit_sleep();
            /* update safe duration */
            systemhang_set_safe_duration(SYSTEMHANG_USER_CONFIG_COUNT_TOTAL - 1, 60*20);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */

            sleep_time *= 1000;
            calculation = (float)sleep_time;
            calculation /= RTC_Freq;/*us*/

            sleep_time = (uint32_t)calculation;
            ulCompleteTickPeriods = sleep_time / 1000;
            remain = sleep_time % (portTICK_PERIOD_MS * 1000);
            tick_remainder += remain;
        } else {
            /* Tickless IDLE mode */
            /* Make sure the OS GPT reload value does not overflow the counter. */
            if (xExpectedIdleTime > (xMaximumPossibleSuppressedTicks)) {
                xExpectedIdleTime = (xMaximumPossibleSuppressedTicks);
            }

             /* Enter IDLE mode */
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
             /* make sure the IDLE time does not overflow the wdt timeout value. */
             uint32_t sleep_time_sec = ((xExpectedIdleTime) / configTICK_RATE_HZ);
             extern uint32_t systemhang_wdt_timeout;
             if (sleep_time_sec > (systemhang_wdt_timeout - 10))
             {
                 sleep_time_sec = (systemhang_wdt_timeout - 10);
                 xExpectedIdleTime = sleep_time_sec * configTICK_RATE_HZ;
                /* maybe xExpectedIdleTime is still larger than OS GPT reload value, it will be cut again */
                /* in any case, wdt timeout value must be larger than IDLE time */
              }
              /* feed wdt to keep time for idle sleep */
              systemhang_wdt_feed_in_task_switch();
             /* after here, wdt is in interrupt mode for prevent sleep(IDLE) flow hang */
#endif 

            ulAST_Reload_ms = ((xExpectedIdleTime - 1UL) * (1000 / configTICK_RATE_HZ));

            os_gpt_start(ulAST_Reload_ms * 1000 , OS_GPT_TIMER_TYPE_ONE_SHOT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &before_sleep_time);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&after_time[0]);

            /* Enter Idle mode */
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_IDLE);

            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&before_time[1]);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &after_sleep_time);
            /* stop GPT and get 1MHz tick count*/
            os_gpt_stop();
            hal_gpt_get_duration_count(before_sleep_time, after_sleep_time, &sleep_time);
            calculation = (float)sleep_time;

            sleep_time = (uint32_t)calculation;
            ulCompleteTickPeriods = sleep_time / 1000;
            remain = sleep_time % (portTICK_PERIOD_MS * 1000);

            tick_remainder += remain;

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
            /* update safe duration */
            systemhang_set_safe_duration(SYSTEMHANG_USER_CONFIG_COUNT_TOTAL - 1, 60*20);
            /* feed wdt */
            systemhang_wdt_feed_in_task_switch();
#endif 
        }

        hal_gpt_get_duration_count(before_time[0], after_time[0], &duration_time[0]);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&after_time[1]);
        hal_gpt_get_duration_count(before_time[1], after_time[1], &duration_time[1]);

        tick_remainder += duration_time[0] + duration_time[1];

        /* Limit OS Tick Compensation Value */
        if (ulCompleteTickPeriods >= (xExpectedIdleTime)) {
            ulCompleteTickPeriods = xExpectedIdleTime;
        } else {
            if (tick_remainder >= (1000 * (1000 / configTICK_RATE_HZ))) {
                tick_remainder -= (1000 * (1000 / configTICK_RATE_HZ));
                ulCompleteTickPeriods++;
            }
        }

        if (tick_remainder > 100) {
            systick_change_period = 1;
            last_remain = (tick_remainder % (portTICK_PERIOD_MS * 1000));
            tick_remainder -= last_remain;
            remain = (portTICK_PERIOD_MS * 1000) - last_remain;
            os_gpt_start(remain, OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);
        } else {
            systick_change_period = 0;
            os_gpt_start(portTICK_PERIOD_MS * 1000, OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);
        }

        if (tick_remainder >= REMAINDER_LIMIT) {
            tick_remainder = REMAINDER_LIMIT;
        }

        vTaskStepTick(ulCompleteTickPeriods);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
        wakeup_eint = eint_get_status();
#endif

        /* Re-enable interrupts */
        __asm volatile("cpsie i");

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#ifdef  SLEEP_MANAGEMENT_DEBUG_SLEEP_WAKEUP_LOG_ENABLE
        sleep_management_dump_wakeup_source(sleep_management_status.wakeup_source, wakeup_eint);
#endif
#endif
    }
#endif
}
#endif


#if configUSE_TICKLESS_IDLE == 2
#if !defined(MTK_OS_CPU_UTILIZATION_ENABLE) || defined(SLEEP_MANAGEMENT_DEBUG_ENABLE)
TimerHandle_t xTimerofTest;
void log_cup_resource_callback(TimerHandle_t pxTimer);
#endif

void vPortSetupTimerInterrupt(void)
{
    uint32_t freq_32K;

    //get rtc real frequency from share memory
    freq_32K = *((volatile uint32_t *)HW_SYSRAM_PRIVATE_MEMORY_RTC_FREQ_START);
    
    //printf("\r\ncm4 hal_rtc_get_f32k_frequency:%d\r\n", freq_32K);
    RTC_Freq = ((float)freq_32K) / 1000;

    os_gpt_init(os_gpt_interrupt_handle);
    os_gpt_start(portTICK_PERIOD_MS * 1000 , OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);

#if configUSE_TICKLESS_IDLE != 0
    /* Calculate the constants required to configure the tick interrupt. */
    {
        /* OS GPT one count equal 1us */
        ulTimerCountsForOneTick = (1000000 / configTICK_RATE_HZ);
        /* OS GPT is 32 bits timer */
        xMaximumPossibleSuppressedTicks = 0xFFFFFFFF / ulTimerCountsForOneTick;
    }
#endif /* configUSE_TICKLESS_IDLE  != 0*/
#if !defined(MTK_OS_CPU_UTILIZATION_ENABLE) || defined(SLEEP_MANAGEMENT_DEBUG_ENABLE)
//   xTimerofTest = xTimerCreate("TimerofTest", (5 * 1000 / portTICK_PERIOD_MS), pdTRUE, NULL, log_cup_resource_callback);
//   xTimerStart(xTimerofTest, 0);
#endif /* !defined(MTK_OS_CPU_UTILIZATION_ENABLE) || defined(SLEEP_MANAGEMENT_DEBUG_ENABLE) */
}
#endif

#if configUSE_TICKLESS_IDLE == 2
void log_cup_resource_callback(TimerHandle_t pxTimer)
{
#ifndef MTK_OS_CPU_UTILIZATION_ENABLE
    TaskStatus_t *pxTaskStatusArray;
    UBaseType_t uxArraySize, x;
    uint32_t ulTotalTime;
    static uint32_t ulTotalTime_now = 0, ulTotalTime_last = 0;
    float Percentage;

    /* Optionally do something if the pxTimer parameter is NULL. */
    configASSERT(pxTimer);

    uxArraySize = uxTaskGetNumberOfTasks();

    pxTaskStatusArray = pvPortMalloc(uxArraySize * sizeof(TaskStatus_t));

    uxArraySize = uxTaskGetSystemState(pxTaskStatusArray, uxArraySize, &ulTotalTime);

    ulTotalTime_now = ulTotalTime;

    if (ulTotalTime_now > ulTotalTime_last) {
        ulTotalTime = ulTotalTime_now - ulTotalTime_last;
    } else {
        ulTotalTime = ulTotalTime_now;
    }
    ulTotalTime_last = ulTotalTime_now;

    if (ulTotalTime > 0UL) {
        log_hal_info("ulTotalTimeL:%lu\r\n", ulTotalTime);
        log_hal_info("CM4:%lu\r\n", hal_dvfs_get_cpu_frequency());
        log_hal_info("----------------------CM4 Dump OS Task Info-----------------------------\r\n");
        for (x = 0; x < uxArraySize; x++) {
            Percentage = ((float)pxTaskStatusArray[x].ulRunTimeCounter) / ((float)ulTotalTime);
            log_hal_info("Task[%s] State[%lu] Percentage[%lu.%lu] MinStack[%lu] RunTime[%lu]\r\n"
                         , pxTaskStatusArray[x].pcTaskName
                         , pxTaskStatusArray[x].eCurrentState
                         , (uint32_t)(Percentage * 100)
                         , ((uint32_t)(Percentage * 1000)) % 10
                         , pxTaskStatusArray[x].usStackHighWaterMark
                         , pxTaskStatusArray[x].ulRunTimeCounter
                        );
        }
        log_hal_info("----------------------------------------------------------------------\r\n");
        vPortFree(pxTaskStatusArray);
    }
    vTaskClearTaskRunTimeCounter();
#endif /* MTK_OS_CPU_UTILIZATION_ENABLE */
#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
    sleep_management_debug_dump_lock_sleep_time();
#endif
}
#endif

#if configUSE_TICKLESS_IDLE == 2
uint32_t get_count_sleep_time_us()
{
    return count_sleep_time_us;
}

uint32_t get_count_idle_time_us()
{
    return count_idle_time_us;
}

void tickless_start_count_idle_ratio()
{
    count_idle = TRUE;
    count_sleep_time_us = 0;
    count_idle_time_us = 0;
}

void tickless_stop_count_idle_ratio()
{
    count_idle = FALSE;
}
#endif
