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
#include "hal_resource_assignment.h"
#include <assert.h>

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
#define DEEP_SLEEP_HW_WAKEUP_TIME 2
#define DEEP_SLEEP_SW_BACKUP_RESTORE_TIME 2
#define REMAINDER_LIMIT 100*1000 //100ms

#define SPM_PCM_RESERVE2                           ((volatile uint32_t*)(0xA2110000 + 0x0B04))

float RTC_Freq = 32.768; /* RTC 32.768KHz Freq*/
uint32_t nvic_mask;
#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
//extern uint32_t eint_get_status(void);
uint32_t wakeup_eint;
#endif

volatile uint32_t systick_change_period = 0, systick_change_period_dsp = 0;

//#define TICKLESS_DEEBUG_ENABLE
#ifdef  TICKLESS_DEEBUG_ENABLE
#define log_debug_tickless(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug_tickless(_message,...)
#endif

uint32_t count_idle_time;
uint32_t count_sleep_time;
volatile uint32_t tick_remainder = 0;

#include "hal_nvic.h"
#include <assert.h>
#include <xtensa/hal.h>

GPT_REGISTER_T *os_gpt1 = OS_GPT1;
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
    hal_nvic_register_isr_handler(OS_GPT_IRQn, callback);
}

ATTR_TEXT_IN_IRAM void os_gpt_start(uint32_t time_out_us , os_gpt_timer_type_t timer_type , os_gpt_clock_source_t clock_source , float rtc_freq)
{
    os_gpt_timer_type_t ost_type = timer_type;
    os_gpt_clock_source_t ost_source = clock_source;
    uint32_t time_out_us_reload;

    time_out_us_reload = (ost_type == OS_GPT_TIMER_TYPE_ONE_SHOT)? time_out_us : (time_out_us - 1);

    if ((time_out_us_reload / 1000) > HAL_GPT_MAXIMUM_MS_TIMER_TIME) {
        assert(0);
    }
    //  os_gpt1->GPT_IRQ_ACK = 0x1;                   //clear interrupt status
    os_gpt1->GPT_CON_UNION.GPT_CON |= (1 << 16);  // disabled the clock source

    /* set 13 divided with 13M source */
    if (ost_source == OS_GPT_CLOCK_SOURCE_32K) {       //set clock source
        os_gpt1->GPT_CON_UNION.GPT_CON |= (1 << 16);   // disable clock before config
        os_gpt1->GPT_CLK = 0x10;      //set 32k divide 1
        os_gpt1->GPT_CON_UNION.GPT_CON &= ~(1 << 16);   // enable clock

        os_gpt1->GPT_COMPARE = (uint32_t)(((float)time_out_us_reload/1000.0) *rtc_freq);

        while (os_gpt_glb->OS_GPT_WCOMPSTA & (1<<1));
        os_gpt1->GPT_CLR      = 0x1;                    // clear the count
        while (os_gpt_glb->OS_GPT_CLRSTA & (1<<1));
    } else {
        os_gpt1->GPT_CON_UNION.GPT_CON |= (1 << 16);   // disable clock before config
        os_gpt1->GPT_CLK = 0xc;     //set  13M divide 13
        os_gpt1->GPT_CON_UNION.GPT_CON &= ~(1 << 16);   // enable clock

        os_gpt1->GPT_COMPARE = time_out_us_reload;
        while (os_gpt_glb->OS_GPT_WCOMPSTA & (1<<1));
        os_gpt1->GPT_CLR      = 0x1;                    // clear the count
        while (os_gpt_glb->OS_GPT_CLRSTA & (1<<1));
        (void)rtc_freq;
    }

    if (ost_type == OS_GPT_TIMER_TYPE_ONE_SHOT) {
        os_gpt1->GPT_CON_UNION.GPT_CON &= ~(3 << 8);    //set one-shot mode
        os_gpt_glb->OS_GPT_IRQMSK_DSP &= 0x1;           //DSP IRQ enable
        os_gpt_glb->OS_GPT_WAKEUPMSK_DSP &= 0x1;        //DSP wakeup enable
    } else {
        os_gpt1->GPT_CON_UNION.GPT_CON |= 0x101;        // set repeat mode
        os_gpt_glb->OS_GPT_IRQMSK_DSP &= 0x1;           //DSP IRQ enable
        os_gpt_glb->OS_GPT_WAKEUPMSK_DSP &= 0x1;        //DSP wakeup enable
    }

    /* enable IRQ */
    os_gpt1->GPT_IRQ_EN = 0x1;

    /* register and enable IRQ */
    hal_nvic_enable_irq(OS_GPT_IRQn);

    os_gpt1->GPT_CON_UNION.GPT_CON |= 0x01;
    return;
}

ATTR_TEXT_IN_IRAM uint32_t os_gpt_stop()
{
    /*diable interrupt*/
    os_gpt1->GPT_IRQ_EN = ~OST_IRQ_ENABLE  ;
    /* stop timer */
    os_gpt1->GPT_CON_UNION.GPT_CON_CELLS.EN &= ~OST_COUNT_START;
    os_gpt1->GPT_IRQ_ACK = OST_IRQ_FLAG_ACK ;                    /* clean interrupt status */
    os_gpt1->GPT_CON_UNION.GPT_CON = 0;            /* disable timer     */

    //NVIC_DisableIRQ(OS_GPT_IRQn);
    hal_nvic_disable_irq(OS_GPT_IRQn);
    //NVIC_ClearPendingIRQ(OS_GPT_IRQn);
    xthal_set_intclear((0x1) << OS_GPT_IRQn);

    return os_gpt1->GPT_COUNT;
}

extern BaseType_t xPortSysTickHandler(void);
ATTR_TEXT_IN_IRAM void os_gpt_interrupt_handle(hal_nvic_irq_t irq_number)
{
    (void)irq_number;

    os_gpt1->GPT_IRQ_ACK = 0x01;

    if (systick_change_period_dsp != 0) {
        systick_change_period_dsp = 0;
        os_gpt_start(portTICK_PERIOD_MS * 1000 , OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);
    }
    /* Run FreeRTOS tick handler*/
    xPortSysTickHandler();
}

#if configUSE_TICKLESS_IDLE == 2
ATTR_TEXT_IN_IRAM void tickless_handler(uint32_t xExpectedIdleTime)
{
    static uint32_t after_idle_time;
    static uint32_t ulCompleteTickPeriods, before_idle_time, remain, ulAST_Reload_ms, last_remain;
    static uint32_t after_sleep_time, before_sleep_time, sleep_time = 0;
    static uint32_t before_time[2], after_time[2], duration_time[2];
    static float calculation;

    #ifdef AIR_CPU_MCPS_PRIORING_ENABLE
    hal_gpt_status_t gpt_ret;
    #endif /* AIR_CPU_MCPS_PRIORING_ENABLE */

    hal_nvic_save_and_set_interrupt_mask_special(&nvic_mask);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&before_time[0]);
    
    /* If a context switch is pending or a task is waiting for the scheduler
    to be unsuspended then abandon the low power entry. */
    if (eTaskConfirmSleepModeStatus() != eAbortSleep) {
        if ((xExpectedIdleTime > (MaximumIdleTime / (1000 / configTICK_RATE_HZ))) && (hal_sleep_manager_is_sleep_locked() == 0) && (((*SPM_PCM_RESERVE2 >> 31) & 0x01) != 1)) {
            /* Tickless SLEEP mode*/

            #ifdef AIR_CPU_MCPS_PRIORING_ENABLE
            /* stop the os profiling timer */
            extern uint32_t os_profiling_gpt_handle;
            (void)hal_gpt_sw_stop_timer_ms(os_profiling_gpt_handle);
            #endif /* AIR_CPU_MCPS_PRIORING_ENABLE */

            if (systick_change_period_dsp != 0) {
                systick_change_period_dsp = 0;
                tick_remainder += os_gpt_stop();
                tick_remainder += last_remain;
            } else {
                tick_remainder += os_gpt_stop();
            }
            
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

            sleep_time *= 1000;
            calculation = (float)sleep_time;
            calculation /= RTC_Freq;/*us*/

            sleep_time = (uint32_t)calculation;
            ulCompleteTickPeriods = (sleep_time / 1000) / ((1000 / configTICK_RATE_HZ));
            remain = sleep_time % (portTICK_PERIOD_MS * 1000);
            tick_remainder += remain;

            hal_gpt_get_duration_count(before_time[0], after_time[0], &duration_time[0]);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&after_time[1]);
            hal_gpt_get_duration_count(before_time[1], after_time[1], &duration_time[1]);
            tick_remainder += duration_time[0] + duration_time[1];

            /* Limit OS Tick Compensation Value */
            if (ulCompleteTickPeriods >= (xExpectedIdleTime)) {
                ulCompleteTickPeriods = xExpectedIdleTime;
            } else {
                if (tick_remainder >= (portTICK_PERIOD_MS *1000)) {
                    tick_remainder -= (portTICK_PERIOD_MS *1000);
                    ulCompleteTickPeriods++;
                }
            }

            if (tick_remainder > 1000) {
                systick_change_period_dsp = 1;
                last_remain = (tick_remainder % (portTICK_PERIOD_MS * 1000));
                tick_remainder -= last_remain;
                remain = (portTICK_PERIOD_MS * 1000) - last_remain;
                os_gpt_start(remain, OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);
            } else {
                systick_change_period_dsp = 0;
                os_gpt_start(portTICK_PERIOD_MS * 1000, OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);
            }

            if (tick_remainder >= REMAINDER_LIMIT) {
                tick_remainder = REMAINDER_LIMIT;
            }

            vTaskStepTick(ulCompleteTickPeriods);

            #ifdef AIR_CPU_MCPS_PRIORING_ENABLE
            /* restart the os profiling timer */
            extern uint32_t os_profiling_gpt_handle;
            extern VOID osDumpTaskCounter(VOID);
            gpt_ret = hal_gpt_sw_start_timer_ms(os_profiling_gpt_handle, OS_PROFILING_GPT_PERIOD_MS, (hal_gpt_callback_t)osDumpTaskCounter, NULL);
            if(HAL_GPT_STATUS_OK != gpt_ret){
                LOG_MSGID_E(common, "os profiling timer is not re-started:%d", 1, gpt_ret);
            }
            #endif /* AIR_CPU_MCPS_PRIORING_ENABLE */

            /* Re-enable interrupts */
            hal_nvic_restore_interrupt_mask_special(nvic_mask);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
            //   wakeup_eint = eint_get_status();
#endif
            //sleep_management_dump_debug_log(SLEEP_MANAGEMENT_DEBUG_LOG_DUMP);

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
#ifdef  SLEEP_MANAGEMENT_DEBUG_SLEEP_WAKEUP_LOG_ENABLE
            sleep_management_dump_wakeup_source(sleep_management_dsp_status.wakeup_source, wakeup_eint);
#endif
#endif
            log_debug_tickless("\r\nEIT=%u\r\n"  , (unsigned int)xExpectedIdleTime);
            log_debug_tickless("RL=%u\r\n"       , (unsigned int)ulAST_Reload_ms);
            return;
        } else {
            /* pure IDLE mode*/
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &before_idle_time);
            __asm__ __volatile__(" dsync             \n"
                                 " waiti 0           \n"
                                 " isync             \n");
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &after_idle_time);

            /* Re-enable interrupts */
            hal_nvic_restore_interrupt_mask_special(nvic_mask);

            count_idle_time += (after_idle_time - before_idle_time) / 1000;
            return;
        }
    }
      /* Re-enable interrupts */
    hal_nvic_restore_interrupt_mask_special(nvic_mask);
}
#endif

#ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
TimerHandle_t xTimerofTest;
extern void sleep_management_debug_dump_lock_sleep_time(void);
#endif /* SLEEP_MANAGEMENT_DEBUG_ENABLE */

/*
A peripheral General Purpose Timer is used for OS timer, and the interrupt is handled in GPT handler.
*/
void _frxt_tick_timer_init(void)
{
    uint32_t freq_32K;

    //get rtc real frequency from share memory
    freq_32K = *((volatile uint32_t *)HW_SYSRAM_PRIVATE_MEMORY_RTC_FREQ_START);
    
    //printf("\r\ndsp hal_rtc_get_f32k_frequency:%d\r\n", freq_32K);
    RTC_Freq = ((float)freq_32K) / 1000;
    
    os_gpt_init(os_gpt_interrupt_handle);
    os_gpt_start(portTICK_PERIOD_MS * 1000 , OS_GPT_TIMER_TYPE_REPEAT , OS_GPT_CLOCK_SOURCE_1M , RTC_Freq);

    #ifdef  SLEEP_MANAGEMENT_DEBUG_ENABLE
    xTimerofTest = xTimerCreate("TimerofTest", (5 * 1000 / portTICK_PERIOD_MS), pdTRUE, NULL, sleep_management_debug_dump_lock_sleep_time);
    xTimerStart(xTimerofTest, 0);
    #endif /* SLEEP_MANAGEMENT_DEBUG_ENABLE */
}
