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

#include "hal.h"
#include "FreeRTOS.h"
#include "port_tick.h"
#include "hal_clock.h"
#include "hal_nvic.h"
#include "timers.h"
#include "hal_gpt_internal.h"

#if configUSE_TICKLESS_IDLE == 2
#include "task.h"
#include "memory_attribute.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#include "hal_gpt.h"
#include "hal_log.h"
#include "hal_eint.h"
#endif

#define MaximumIdleTime 20  //ms
#define DEEP_SLEEP_HW_WAKEUP_TIME 1
#define DEEP_SLEEP_SW_BACKUP_RESTORE_TIME 1

//#define TICKLESS_DEEBUG_ENABLE
#ifdef  TICKLESS_DEEBUG_ENABLE
#define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug(_message,...)
#endif

#if configUSE_TICKLESS_IDLE == 2
float RTC_Freq =  32.768; /* RTC 32.768KHz Freq*/
uint32_t nvic_mask;
#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
extern uint32_t eint_get_status(void);
uint32_t wakeup_eint;
#endif

uint32_t before_sleep_time;
extern uint8_t sleep_manager_handle;
GPT_REGISTER_T *os_gpt = GPT7;
bool reset_gpt_to_systick = false;
volatile uint32_t systick_change_period = 0;
uint32_t ostick_remain_us;
uint32_t sw_lanency[6];

void os_gpt_pause(void)
{
    hal_gpt_stop_timer(HAL_GPT_7);
}

void os_gpt_resume(bool update, uint32_t new_compare)
{
    if (update) {
        systick_change_period = 1;
        hal_gpt_start_timer_us(HAL_GPT_7,new_compare,0);
        reset_gpt_to_systick = true;
    }else{
        hal_gpt_start_timer_us(HAL_GPT_7,portTICK_PERIOD_MS*1000,HAL_GPT_TIMER_TYPE_REPEAT);
    }
}

void doSleepSystickCalibration(uint32_t maxSystickCompensation)
{
    static uint32_t ulCompleteTickPeriods,remain_time,after_sleep_time,sleep_time,ost_compare;
    
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &after_sleep_time);

    hal_gpt_get_duration_count(before_sleep_time, after_sleep_time, &sleep_time);

    sleep_time = (unsigned int)((float)(sleep_time*1000) / RTC_Freq);
    sleep_time += ostick_remain_us;

    hal_gpt_get_duration_count(sw_lanency[0], sw_lanency[1], &sw_lanency[2]);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &sw_lanency[4]);
    hal_gpt_get_duration_count(sw_lanency[3], sw_lanency[4], &sw_lanency[5]);

    sleep_time += sw_lanency[2] + sw_lanency[5];

    ulCompleteTickPeriods = (sleep_time / 1000) / ((1000 / configTICK_RATE_HZ));

    /* Limit OS Tick Compensation Value */
    if (ulCompleteTickPeriods > (maxSystickCompensation - 1)) {
        ulCompleteTickPeriods = maxSystickCompensation - 1;
    }

    vTaskStepTick(ulCompleteTickPeriods);    

    remain_time = sleep_time - (ulCompleteTickPeriods * 1000 * (1000 / configTICK_RATE_HZ)); //us

    if((remain_time > 0) && (remain_time < (portTICK_PERIOD_MS*1000))){
        systick_change_period = 1;

        ost_compare = ((portTICK_PERIOD_MS * 1000) - remain_time);
        os_gpt_resume(true, ost_compare);
    }else{
        /* Restart OS GPT. */
        os_gpt_resume(false, 0);
    }

    return;
}

void AST_vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    static volatile unsigned int ulAST_Reload_ms;

    /* Stop the OS GPT momentarily.  */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &sw_lanency[0]);
    os_gpt_pause();
    ostick_remain_us = (portTICK_PERIOD_MS * 1000) - ((os_gpt->GPT_COMPARE - os_gpt->GPT_COUNT));

    /* Calculate total idle time to ms */
    ulAST_Reload_ms = (xExpectedIdleTime - 1) * (1000 / configTICK_RATE_HZ);
    ulAST_Reload_ms -= (ostick_remain_us/1000);
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        /* Restart OS GPT. */
        os_gpt_resume(false, 0);
        return;
    } else {
        /* Enter Sleep mode */
        if (ulAST_Reload_ms > 0) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &sw_lanency[1]);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &before_sleep_time);
            hal_sleep_manager_set_sleep_time((uint32_t)ulAST_Reload_ms);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &sw_lanency[3]);
        }

        /* Calculate and Calibration Sleep Time to OS Tick */
        doSleepSystickCalibration(xExpectedIdleTime);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
        wakeup_eint = eint_get_status();
#endif

        //sleep_management_dump_debug_log(SLEEP_MANAGEMENT_DEBUG_LOG_DUMP);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#ifdef  SLEEP_MANAGEMENT_DEBUG_SLEEP_WAKEUP_LOG_ENABLE
        sleep_management_dump_wakeup_source(sleep_management_dsp_status.wakeup_source, wakeup_eint);
#endif
#endif
        log_debug("\r\nEIT=%u\r\n"  , (unsigned int)xExpectedIdleTime);

        log_debug("RL=%u\r\n"       , (unsigned int)ulAST_Reload_ms);
    }
}

void tickless_handler(uint32_t xExpectedIdleTime)
{
    hal_nvic_save_and_set_interrupt_mask(&nvic_mask);

    if ((xExpectedIdleTime > (MaximumIdleTime / (1000 / configTICK_RATE_HZ))) && (hal_sleep_manager_is_sleep_locked() == 0) && (((*((volatile uint32_t*)(0xA2120B04)) >> 31)&0x01) != 1)) {
        /* Enter a critical section but don't use the taskENTER_CRITICAL()
        method as that will mask interrupts that should exit sleep mode. */

        AST_vPortSuppressTicksAndSleep(xExpectedIdleTime);
    }else {
        /* If a context switch is pending or a task is waiting for the scheduler
        to be unsuspended then abandon the low power entry. */
        if (eTaskConfirmSleepModeStatus() != eAbortSleep) {
            /* Enter Idle mode */
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_IDLE);
        }
        log_debug("\r\nST_CPT=%u\r\n"   ,  (unsigned int)xExpectedIdleTime);
    }

    /* Re-enable interrupts */
    hal_nvic_restore_interrupt_mask(nvic_mask);  
}
#endif

/*
A peripheral General Purpose Timer is used for OS timer, and the interrupt is handled in GPT handler.
*/
extern BaseType_t xPortSysTickHandler( void );
void _frxt_tick_timer_init(void)
{
	hal_gpt_init(HAL_GPT_7);
	hal_gpt_register_callback(HAL_GPT_7,(hal_gpt_callback_t)xPortSysTickHandler,NULL);
	hal_gpt_start_timer_us(HAL_GPT_7,portTICK_PERIOD_MS*1000,HAL_GPT_TIMER_TYPE_REPEAT);
}
