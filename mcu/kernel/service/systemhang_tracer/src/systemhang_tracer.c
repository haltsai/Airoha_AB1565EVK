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

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
/* Includes ------------------------------------------------------------------*/
#include "systemhang_tracer.h"
#include "hal_rtc.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "memory_attribute.h"
#include "hal_wdt.h"
#include "hal_gpt.h"
#include "hal_nvic.h"
#include "FreeRTOS.h"
#include "task.h"
#include "exception_handler.h"

#ifdef MTK_SYSTEM_HANG_CHECK_ENABLE
    #error "Please disable MTK_SYSTEM_HANG_CHECK_ENABLE feature when use MTK_SYSTEM_HANG_TRACER_ENABLE feature"
#endif /* MTK_SYSTEM_HANG_CHECK_ENABLE */

#ifndef HAL_WDT_MODULE_ENABLED
    #error "Please enable wdt module when use MTK_SYSTEM_HANG_TRACER_ENABLE"
#endif

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
#include "FreeRTOSConfig.h"
#include "hal_feature_config.h"

#if (configUSE_IDLE_HOOK == 0)
    #error "Please enable IDLE Hook when use MTK_SYSTEM_HANG_TRACER_ENABLE O1"
#endif

#if (configUSE_TICKLESS_IDLE != 0)
    // #error "Please disable sleep feature when use MTK_SYSTEM_HANG_TRACER_ENABLE O1"
#endif

#ifndef HAL_WDT_MODULE_ENABLED
    #error "Please enable wdt module when use MTK_SYSTEM_HANG_TRACER_ENABLE O1"
#endif
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
#include "FreeRTOSConfig.h"
#include "hal_feature_config.h"

#if (configUSE_IDLE_HOOK == 0)
    #error "Please enable IDLE Hook when use MTK_SYSTEM_HANG_TRACER_ENABLE O2"
#endif

#if (configUSE_TICKLESS_IDLE != 0)
    // #error "Please disable sleep feature when use MTK_SYSTEM_HANG_TRACER_ENABLE O2"
#endif

#ifndef HAL_WDT_MODULE_ENABLED
    #error "Please enable wdt module when use MTK_SYSTEM_HANG_TRACER_ENABLE O2"
#endif

#ifndef HAL_RTC_MODULE_ENABLED
    #error "Please enable rtc module when use MTK_SYSTEM_HANG_TRACER_ENABLE O2"
#endif
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */


/* Private define ------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ATTR_ZIDATA_IN_TCM uint32_t systemhang_wdt_count = 0;
ATTR_ZIDATA_IN_TCM uint32_t systemhang_wdt_timeout = 0;
ATTR_ZIDATA_IN_TCM uint32_t systemhang_wdt_timestamp = 0;

ATTR_ZIDATA_IN_TCM systemhang_user_config_t systemhang_current_config;
ATTR_ZIDATA_IN_TCM systemhang_user_config_t systemhang_user_config[SYSTEMHANG_USER_CONFIG_COUNT_TOTAL];

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
ATTR_ZIDATA_IN_TCM uint32_t systemhang_isr_count = 0;
ATTR_ZIDATA_IN_TCM uint32_t systemhang_isr_tracer[SYSTEMHANG_PENDSV_COUNT_TOTAL] = {0};
ATTR_ZIDATA_IN_TCM uint32_t systemhang_isr_time1[SYSTEMHANG_PENDSV_COUNT_TOTAL] = {0};
ATTR_ZIDATA_IN_TCM uint32_t systemhang_isr_time2[SYSTEMHANG_PENDSV_COUNT_TOTAL] = {0};

ATTR_ZIDATA_IN_TCM uint32_t systemhang_pendsv_count = 0;
ATTR_ZIDATA_IN_TCM uint32_t systemhang_pendsv_tracer[SYSTEMHANG_PENDSV_COUNT_TOTAL] = {0};
ATTR_ZIDATA_IN_TCM uint32_t systemhang_pendsv_nextpc[SYSTEMHANG_PENDSV_COUNT_TOTAL] = {0};
ATTR_ZIDATA_IN_TCM uint32_t systemhang_pendsv_lr[SYSTEMHANG_PENDSV_COUNT_TOTAL] = {0};
ATTR_ZIDATA_IN_TCM uint32_t systemhang_pendsv_time[SYSTEMHANG_PENDSV_COUNT_TOTAL] = {0};
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
ATTR_ZIDATA_IN_TCM uint32_t systemhang_set_tag_safe = 0;
char systemhang_tags[SYSTEMHANG_RETENTION_INFO_TOTAL];
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */

extern volatile exception_config_mode_t exception_config_mode;

/* Private functions ---------------------------------------------------------*/
// #ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
void systemhang_wdt_timeout_handle(hal_wdt_reset_status_t wdt_reset_status)
{
    systemhang_wdt_count++;
    if(systemhang_wdt_count == 12)
    {
        /* assert 0 to trigger exception hanling flow */
        assert(0);
    }
    else
    {
        systemhang_wdt_config(systemhang_wdt_timeout);
    }
}
// #endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

ATTR_TEXT_IN_TCM void systemhang_wdt_feed_in_task_switch(void)
{
    uint32_t saved_mask = 0;
    uint32_t duration = 0;


    /* feed wdt */
    hal_wdt_feed(HAL_WDT_FEED_MAGIC);
	
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (systemhang_current_config.used != 0)
    {
        /* if there are user config, check if this task switch is safe */
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &systemhang_wdt_timestamp);
        hal_gpt_get_duration_count(systemhang_current_config.timestamp, systemhang_wdt_timestamp, &duration);
        if (duration > systemhang_current_config.duration)
        {
            /* there is a configurable timeout */
            assert(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
}

ATTR_TEXT_IN_TCM void systemhang_set_safe_duration(uint32_t user_id, uint32_t safe_sec)
{
    uint32_t saved_mask = 0;
    uint32_t i = 0;
    uint32_t id = 0;
    uint32_t duration = 0;
    uint32_t remaining_duration_min = 0;

    /* check if user_id is ok */
    if ((user_id >= SYSTEMHANG_USER_CONFIG_COUNT_TOTAL) || (safe_sec > 36*60*60))
    {
        assert(0);
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    systemhang_wdt_count = 0;

    /* update user configurations */
    systemhang_user_config[user_id].used = TRUE;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &systemhang_user_config[user_id].timestamp);
    extern uint32_t gpt_convert_ms_to_32k_count(uint32_t ms);
    systemhang_user_config[user_id].duration = gpt_convert_ms_to_32k_count(safe_sec * 1000);

    /* check current timestamp if is safe */
    if (systemhang_current_config.used != 0)
    {
        hal_gpt_get_duration_count(systemhang_current_config.timestamp, systemhang_user_config[user_id].timestamp, &duration);
        if (duration > systemhang_current_config.duration)
        {
            /* enter here means the current configuration has been expired */
            assert(0);
        }
    }

    /* find out the earliest expired configuration */
    id = user_id;
    remaining_duration_min = systemhang_user_config[user_id].duration;
    for (i = 0; i < SYSTEMHANG_USER_CONFIG_COUNT_TOTAL; i++)
    {
        if (systemhang_user_config[i].used)
        {
            /* calculate each configuration's elapsed time */
            /* systemhang_user_config[user_id].timestamp is the latest timestamp */
            /* all systemhang_user_config[i].timestamp should be ahead of systemhang_user_config[user_id].timestamp */
            hal_gpt_get_duration_count(systemhang_user_config[i].timestamp, systemhang_user_config[user_id].timestamp, &duration);
            if (duration >= systemhang_user_config[i].duration)
            {
                /* enter here means the current timestamp has been expired than this configuration */
                assert(0);
            }

            /* calculate each configuration's remaining duration */
            duration = systemhang_user_config[i].duration - duration;
            if(duration < remaining_duration_min)
            {
                /* enter here means this configuration's remaining duration is less than the current chosen configuration */
                /* so choose this configuration as the current chosen configuration */
                remaining_duration_min = duration;
                id = i;
            }
        }
    }

    /* the current chosen configuration will be update into the current configuration */
    systemhang_current_config.timestamp = systemhang_user_config[id].timestamp;
    systemhang_current_config.duration  = systemhang_user_config[id].duration;
    systemhang_current_config.used = TRUE;

    hal_nvic_restore_interrupt_mask(saved_mask);
}

void systemhang_wdt_config(uint32_t timeout_sec)
{
#if !defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) && !defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2) && !defined(MTK_SYSTEM_HANG_TRACER_ENABLE_MP)

	hal_wdt_config_t wdt_init;

	if (exception_config_mode.exception_mode_t.wdt_reset_mode == TRUE)
    	wdt_init.mode = HAL_WDT_MODE_RESET;
	else
		wdt_init.mode = HAL_WDT_MODE_INTERRUPT;	
	
    wdt_init.seconds = timeout_sec;

    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    hal_wdt_deinit();
    hal_wdt_init(&wdt_init);
	
	if (exception_config_mode.exception_mode_t.wdt_reset_mode == FALSE)
    hal_wdt_register_callback(systemhang_wdt_timeout_handle);

	hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif /* !defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) && !defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2) && !defined(MTK_SYSTEM_HANG_TRACER_ENABLE_MP) */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
    hal_wdt_config_t wdt_init;
    wdt_init.mode = HAL_WDT_MODE_INTERRUPT;
    wdt_init.seconds = timeout_sec;

    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    hal_wdt_init(&wdt_init);
    hal_wdt_register_callback(systemhang_wdt_timeout_handle);
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
    hal_wdt_config_t wdt_init;
    wdt_init.mode = HAL_WDT_MODE_RESET;
    wdt_init.seconds = timeout_sec;

    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    hal_wdt_init(&wdt_init);
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_MP
    hal_wdt_config_t wdt_init;
    wdt_init.mode = HAL_WDT_MODE_RESET;
    wdt_init.seconds = timeout_sec;

    hal_wdt_disable(HAL_WDT_DISABLE_MAGIC);
    hal_wdt_deinit();
    hal_wdt_init(&wdt_init);
    hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */
}

void systemhang_exception_enter_trace(void)
{
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
    char exception_tag[SYSTEMHANG_RETENTION_INFO_TOTAL - 1] = {0,'e','x','p'};
    hal_rtc_set_data(HAL_RTC_BACKUP_BYTE_NUM_MAX - SYSTEMHANG_RETENTION_INFO_TOTAL + 1,
                     (const char *)exception_tag,
                     SYSTEMHANG_RETENTION_INFO_TOTAL - 1);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */
}

ATTR_TEXT_IN_TCM void systemhang_interrupt_enter_trace(uint32_t irq_num, systemhang_tracer_t *tracer)
{
#if defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2)
    uint32_t saved_mask = 0;
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
    uint32_t tracer_offest = 0;
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if(irq_num != SYSTEMHANG_OS_INT_NUMBER)
    {
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
        tracer_offest = systemhang_isr_count % SYSTEMHANG_ISR_COUNT_TOTAL;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &systemhang_isr_time1[tracer_offest]);
        systemhang_isr_tracer[tracer_offest] = irq_num;
        systemhang_isr_count += 1;
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
        char current_irq_num[SYSTEMHANG_RETENTION_INFO_TOTAL - 1] = {0,0,0,0};

        current_irq_num[0] = (char)irq_num;
        hal_rtc_get_data(HAL_RTC_BACKUP_BYTE_NUM_MAX - SYSTEMHANG_RETENTION_INFO_TOTAL + 1,
                         tracer->retentionstatus_backup,
                         SYSTEMHANG_RETENTION_INFO_TOTAL - 1);
        hal_rtc_set_data(HAL_RTC_BACKUP_BYTE_NUM_MAX - SYSTEMHANG_RETENTION_INFO_TOTAL + 1,
                         (const char *)current_irq_num,
                         SYSTEMHANG_RETENTION_INFO_TOTAL - 1);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 || MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */
}

ATTR_TEXT_IN_TCM void systemhang_interrupt_exit_trace(uint32_t irq_num, systemhang_tracer_t *tracer)
{
#if defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2)
    uint32_t saved_mask = 0;
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
    uint32_t tracer_offest = 0;
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if(irq_num != SYSTEMHANG_OS_INT_NUMBER)
    {
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
        tracer_offest = (systemhang_isr_count - 1) % SYSTEMHANG_ISR_COUNT_TOTAL;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &systemhang_isr_time2[tracer_offest]);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
        hal_rtc_set_data(HAL_RTC_BACKUP_BYTE_NUM_MAX - SYSTEMHANG_RETENTION_INFO_TOTAL + 1,
                         (const char *)(tracer->retentionstatus_backup),
                         SYSTEMHANG_RETENTION_INFO_TOTAL - 1);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 || MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */
}

ATTR_TEXT_IN_TCM void systemhang_task_enter_trace(void)
{
#if defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2)
    uint32_t saved_mask = 0;
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
    uint32_t tracer_offest = 0;
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O1
    tracer_offest = systemhang_pendsv_count % SYSTEMHANG_PENDSV_COUNT_TOTAL;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &systemhang_pendsv_time[tracer_offest]);
    systemhang_pendsv_tracer[tracer_offest] = (uint32_t)(xTaskGetCurrentTaskHandle());
    systemhang_pendsv_count += 1;
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
    hal_rtc_set_data(HAL_RTC_BACKUP_BYTE_NUM_MAX - SYSTEMHANG_RETENTION_INFO_TOTAL + 1,
                     (const char *)(pcTaskGetTaskName(NULL)),
                     SYSTEMHANG_RETENTION_INFO_TOTAL - 1);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */

    hal_nvic_restore_interrupt_mask(saved_mask);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 || MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */
}

ATTR_TEXT_IN_TCM void systemhang_task_exit_trace(uint32_t *task_psp)
{
#if defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1)
    uint32_t saved_mask = 0;
    uint32_t tracer_offest = 0;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if(systemhang_pendsv_count > 0)
    {
        tracer_offest = (systemhang_pendsv_count - 1) % SYSTEMHANG_PENDSV_COUNT_TOTAL;
        /* task_psp is PSP after hw push r0 - r3, r12 , lr, return address and xPSR */
        systemhang_pendsv_lr[tracer_offest]     = *(task_psp + 5);
        systemhang_pendsv_nextpc[tracer_offest] = *(task_psp + 6);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O1 */
}

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
void systemhang_retention_status_output(void)
{
    uint32_t i = 0;
    char data = 0;

    printf("[system hang]retention status:");
    for(i = 0; i < SYSTEMHANG_RETENTION_INFO_TOTAL; i++)
    {
        hal_rtc_get_data((i + HAL_RTC_BACKUP_BYTE_NUM_MAX - SYSTEMHANG_RETENTION_INFO_TOTAL),
                         (char *)&data,
                         1);
        printf(" 0x%x ", (int)data);
        systemhang_tags[i] = data;
    }
    printf("\r\n");
}

void systemhang_retention_status_init(void)
{
    char data[SYSTEMHANG_RETENTION_INFO_TOTAL] = {'@',0,0,0,0};

    hal_rtc_set_data(HAL_RTC_BACKUP_BYTE_NUM_MAX - SYSTEMHANG_RETENTION_INFO_TOTAL,
                     (const char *)data,
                     5);

    systemhang_set_tag_safe = 0xdeadbeef;
}

void systemhang_retention_status_get_tag(char *data, uint32_t length)
{
    uint32_t i = 0;

    for(i = 0; (i < length) && (i < SYSTEMHANG_RETENTION_INFO_TOTAL); i++)
    {
        *(data + i) = systemhang_tags[i];
    }
}

ATTR_TEXT_IN_TCM void systemhang_retention_status_set_tag(char tag)
{
    char data = tag;
    uint32_t saved_mask = 0;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if ((systemhang_set_tag_safe != 0) && (systemhang_set_tag_safe != 0xdeadbeef))
    {
        assert(0);
    }

    if (systemhang_set_tag_safe != 0)
    {
        hal_rtc_set_data(HAL_RTC_BACKUP_BYTE_NUM_MAX - SYSTEMHANG_RETENTION_INFO_TOTAL,
                        (const char *)&data,
                        1);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
}
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */

void systemhang_tracer_init(uint32_t timeout_sec)
{
    /* parameter check */
    if((timeout_sec <= 10) || (timeout_sec > HAL_WDT_MAX_TIMEOUT_VALUE))
    {
        assert(0);
    }

    systemhang_wdt_timeout = timeout_sec;
    systemhang_wdt_config(systemhang_wdt_timeout);

#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE_O2
    systemhang_retention_status_output();
    systemhang_retention_status_init();
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE_O2 */

    printf("[system hang tracer]enable,timeout:%ds\r\n",(int)timeout_sec); 
}

#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
