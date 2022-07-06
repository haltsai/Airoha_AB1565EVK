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

#if configUSE_TICKLESS_IDLE == 2
#include "task.h"
#include "memory_attribute.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#include "core_cm4.h"
#include "hal_gpt.h"
#include "hal_rtc.h"
#include "hal_log.h"
#include "hal_eint.h"
#include "hal_wdt.h"
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
#include "systemhang_tracer.h"
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
#endif

#define MaximumIdleTime 10  //ms
#define DEEP_SLEEP_HW_WAKEUP_TIME 2
#define DEEP_SLEEP_SW_BACKUP_RESTORE_TIME 2

//#define TICKLESS_DEEBUG_ENABLE
#ifdef  TICKLESS_DEEBUG_ENABLE
#define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug(_message,...)
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

#if configUSE_TICKLESS_IDLE == 2
//static void get_rtc_real_clock_freq(void);
TimerHandle_t timestamp_timer = NULL;
float RTC_Freq = 32.768; /* RTC 32.768KHz Freq*/
static long unsigned int before_idle_time, before_sleep_time;

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
extern uint32_t eint_get_status(void);
uint32_t wakeup_eint;
#endif

uint8_t tickless_sleep_manager_handle;

void os_gpt0_pause(void);
void os_gpt0_resume(bool update, uint32_t new_compare);

void doIdelSystickCalibration(uint32_t maxSystickCompensation)
{
    static long unsigned int after_idle_time, sleep_time = 0;
    static uint32_t ulCompleteTickPeriods;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&after_idle_time);

    if (after_idle_time >= before_idle_time) {
        sleep_time = after_idle_time - before_idle_time;
    } else {
        sleep_time = after_idle_time + (0xFFFFFFFF - before_idle_time);
    }

    ulCompleteTickPeriods = (sleep_time / 1000) / (1000 / configTICK_RATE_HZ);

    /* Limit OS Tick Compensation Value */
    if (ulCompleteTickPeriods >= (maxSystickCompensation)) {
        ulCompleteTickPeriods = maxSystickCompensation;
    }

    vTaskStepTick(ulCompleteTickPeriods);
    return;
}

void doSleepSystickCalibration(uint32_t maxSystickCompensation)
{
    static uint32_t ulCompleteTickPeriods;
    static long unsigned int after_sleep_time, sleep_time = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &after_sleep_time);

    if (after_sleep_time >= before_sleep_time) {
        sleep_time = after_sleep_time - before_sleep_time;
    } else {
        sleep_time = after_sleep_time + (0xFFFFFFFF - before_sleep_time);
    }

    ulCompleteTickPeriods = ((unsigned int)(((float)sleep_time) / RTC_Freq)) / ((1000 / configTICK_RATE_HZ));

    /* Limit OS Tick Compensation Value */
    if (ulCompleteTickPeriods > (maxSystickCompensation - 1)) {
        ulCompleteTickPeriods = maxSystickCompensation - 1;
    }

    vTaskStepTick(ulCompleteTickPeriods);

    log_debug("nowCount=%u\r\n", (unsigned int)nowCount);
    log_debug("nowTick=%u\r\n", (unsigned int)nowTick);
    log_debug("CTP=%u\r\n", (unsigned int)ulCompleteTickPeriods);
    return;
}

void AST_vPortSuppressTicksAndSleep(TickType_t xExpectedIdleTime)
{
    volatile static unsigned int ulAST_Reload_ms;

    __asm volatile("cpsid i");

    /* Calculate total idle time to ms */
    ulAST_Reload_ms = (xExpectedIdleTime - 1) * (1000 / configTICK_RATE_HZ);
    ulAST_Reload_ms = ulAST_Reload_ms - DEEP_SLEEP_SW_BACKUP_RESTORE_TIME - DEEP_SLEEP_HW_WAKEUP_TIME;

    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        /* Restart OS GPT. */
        os_gpt0_resume(false, 0);

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
        /* restore wdt status to the configuration before sleep */
        extern void hal_wdt_exit_sleep(void);
        hal_wdt_exit_sleep();
        systemhang_set_safe_duration(SYSTEMHANG_USER_CONFIG_COUNT_TOTAL - 1, 60*20);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */

        /* Re-enable interrupts */
        __asm volatile("cpsie i");
        return;
    } else {
        /* Enter Sleep mode */
        if (ulAST_Reload_ms > 0) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &before_sleep_time);
            hal_sleep_manager_set_sleep_time((uint32_t)ulAST_Reload_ms);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_SLEEP);
        }

        /* Calculate and Calibration Sleep Time to OS Tick */
        doSleepSystickCalibration(xExpectedIdleTime);

        /* Restart OS GPT. */
        os_gpt0_resume(false, 0);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
        wakeup_eint = eint_get_status();
#endif

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
        /* restore wdt status to the configuration before sleep */
        extern void hal_wdt_exit_sleep(void);
        hal_wdt_exit_sleep();
        systemhang_set_safe_duration(SYSTEMHANG_USER_CONFIG_COUNT_TOTAL - 1, 60*20);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */

        /* Re-enable interrupts */
        __asm volatile("cpsie i");

        sleep_management_dump_debug_log(SLEEP_MANAGEMENT_DEBUG_LOG_DUMP);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#ifdef  SLEEP_MANAGEMENT_DEBUG_SLEEP_WAKEUP_LOG_ENABLE
        sleep_management_dump_wakeup_source(sleep_management_status.wakeup_source, wakeup_eint);
#endif
#endif
        log_debug("\r\nEIT=%u\r\n", (unsigned int)xExpectedIdleTime);
        log_debug("RL=%u\r\n", (unsigned int)ulAST_Reload_ms);
    }
}

void tickless_handler(uint32_t xExpectedIdleTime)
{
    static long unsigned int ulReloadValue;

    /* Enter a critical section but don't use the taskENTER_CRITICAL()
    method as that will mask interrupts that should exit sleep mode. */
    __asm volatile("cpsid i");

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
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */

    /* Stop the OS GPT momentarily.  */
    os_gpt0_pause();

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&before_idle_time);

    if ((xExpectedIdleTime > (MaximumIdleTime / (1000 / configTICK_RATE_HZ))) && (hal_sleep_manager_is_sleep_locked() == 0)) {
        AST_vPortSuppressTicksAndSleep(xExpectedIdleTime);
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
        /* after here, wdt has been restored to the configuration before sleep */
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
        return;
    }

    /* Make sure the OS GPT reload value does not overflow the counter. */
    if (xExpectedIdleTime > (xMaximumPossibleSuppressedTicks)) {
        xExpectedIdleTime = (xMaximumPossibleSuppressedTicks);
    }

    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() == eAbortSleep) {
        /* Restart OS GPT. */
        os_gpt0_resume(false, 0);

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
        /* restore wdt status to the configuration before sleep */
        extern void hal_wdt_exit_sleep(void);
        hal_wdt_exit_sleep();
        systemhang_set_safe_duration(SYSTEMHANG_USER_CONFIG_COUNT_TOTAL - 1, 60*20);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */

        /* Re-enable interrupts - see comments above the cpsid instruction()
        above. */
        __asm volatile("cpsie i");
    } else {
        ulReloadValue = ((xExpectedIdleTime - 1UL) * (1000 / configTICK_RATE_HZ));
        /* Enter Idle mode */
        if (ulReloadValue > 0) {
            hal_sleep_manager_set_sleep_time((uint32_t)ulReloadValue);
            hal_sleep_manager_enter_sleep_mode(HAL_SLEEP_MODE_IDLE);
        }

        /* Calculate and Calibration Idle Time to OS Tick */
        doIdelSystickCalibration(xExpectedIdleTime);

        /* Restart OS GPT. */
        os_gpt0_resume(false, 0);

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
        /* restore wdt status to the configuration before sleep */
        extern void hal_wdt_exit_sleep(void);
        hal_wdt_exit_sleep();
        systemhang_set_safe_duration(SYSTEMHANG_USER_CONFIG_COUNT_TOTAL - 1, 60*20);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */

        /* Re-enable interrupts - see comments above the cpsid instruction() above. */
        __asm volatile("cpsie i");

        log_debug("\r\nST_CPT=%u\r\n", (unsigned int)xExpectedIdleTime);
    }
}

#if 0
static void get_rtc_real_clock_freq(void)
{
#define windows_cnt 1999
    /* waiting for RTC Driver ready */

    uint32_t rtc_count, now = 0, last = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &last);
    rtc_count = f32k_measure_count(1, 1, windows_cnt);

    /* use 26MHz xtal */
    RTC_Freq = (float)rtc_count;
    RTC_Freq = (26000000.f / RTC_Freq) * (windows_cnt + 1.f);
    RTC_Freq /= 1000;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &now);

#if 0
    printf("***************************************************\r\n");
    printf("f32k_measure_count:%d,%d,%d\r\n", (unsigned int)(RTC_Freq * 100000), rtc_count, now - last);
    printf("***************************************************\r\n");
#endif
}
#endif
#endif

GPT_REGISTER_T *os_gpt0 = OS_GPT0;
OS_GPT_REGISTER_GLOABL_T *os_gpt_glb = OS_GPTGLB;
bool reset_gpt_to_systick = false;

extern void xPortSysTickHandler(void);
void os_gpt_interrupt_handle(hal_nvic_irq_t irq_number)
{
    os_gpt0->GPT_IRQ_ACK = 0x01;

    /* Run FreeRTOS tick handler*/
    xPortSysTickHandler();
}

void os_gpt_init(uint32_t ms)
{
    /* set 13 divided with 13M source */
    os_gpt0->GPT_CON_UNION.GPT_CON |= (1 << 16);   // disable clock before config
    os_gpt0->GPT_CLK = 0xc;
    os_gpt0->GPT_CON_UNION.GPT_CON &= ~(1 << 16);   // enable clock
    os_gpt0->GPT_COMPARE = ms * 1000;

    /* clear */
    os_gpt0->GPT_CLR = 0x01;
    while (os_gpt0->GPT_COUNT);

    /* enable IRQ */
    os_gpt0->GPT_IRQ_EN = 0x1;
    /* enable GPT0 clk and repeat mode and enable GPT0 */
    os_gpt0->GPT_CON_UNION.GPT_CON |= 0x101;

    /* register and enable IRQ */
    hal_nvic_register_isr_handler(OS_GPT_IRQn, (hal_nvic_isr_t)os_gpt_interrupt_handle);
    NVIC_EnableIRQ(OS_GPT_IRQn);
    os_gpt_glb->OS_GPT_IRQMSK &= 0x2;
    //os_gpt_glb->OS_GPT_WAKEUPMSK &= 0x2;  // mask as system will dead when boot-up, must unmask after sleep<->wake is ok
}

void os_gpt0_pause(void)
{
    os_gpt0->GPT_CON_UNION.GPT_CON &= 0xFFFFFFFE;
}

void os_gpt0_resume(bool update, uint32_t new_compare)
{
    if (update) {
        reset_gpt_to_systick = true;
        os_gpt0->GPT_COMPARE = new_compare;
    }
    os_gpt0->GPT_CON_UNION.GPT_CON |= 0x00000001;
}

void vPortSetupTimerInterrupt(void)
{
#ifdef TEMP_FPGA_ML1S_HACKS
    /* Tick has to be 20 times slower when using slow bitfiles */
    extern bool use_slow_bitfile;
    if (use_slow_bitfile == true) {
        os_gpt_init(portTICK_PERIOD_MS * 20);    /* 1tick = 1ms */
    } else
#endif //TEMP_FPGA_ML1S_HACKS
        os_gpt_init(portTICK_PERIOD_MS); /* 1tick = 1ms */

#if configUSE_TICKLESS_IDLE != 0
    /* Calculate the constants required to configure the tick interrupt. */
    {
        /* OS GPT one count equal 1us */
        ulTimerCountsForOneTick = (1000000 / configTICK_RATE_HZ);
        /* OS GPT is 32 bits timer */
        xMaximumPossibleSuppressedTicks = 0xFFFFFFFF / ulTimerCountsForOneTick;
    }
#endif /* configUSE_TICKLESS_IDLE  != 0*/
}
