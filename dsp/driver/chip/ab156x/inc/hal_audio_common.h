/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#ifndef __HAL_COMMON_H__
#define __HAL_COMMON_H__
#include "hal_platform.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include "hal_ccni.h"
#include "types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include "assert.h"
#include "dsp_temp.h"
#include "syslog.h"
//#include "portmacro.h"
//#include "FreeRTOS.h"
//#include "task.h"
extern void vTaskEnterCritical( void );
extern void vTaskExitCritical( void );
extern void hal_audio_enter_criticl_section(void);
extern void hal_audio_exit_criticl_section(void);
#define HAL_AUDIO_ASSERT( x )                               assert( x )
#if 0
#define HAL_AUDIO_ENTER_CRITICAL()                          vTaskEnterCritical()//portENTER_CRITICAL()//OS_ENTER_CRITICAL();
#define HAL_AUDIO_EXIT_CRITICAL()                           vTaskExitCritical()//portEXIT_CRITICAL()//OS_EXIT_CRITICAL();
#else
#define HAL_AUDIO_ENTER_CRITICAL()                          hal_audio_enter_criticl_section()
#define HAL_AUDIO_EXIT_CRITICAL()                           hal_audio_exit_criticl_section()

#endif
#if 0
#define HAL_AUDIO_LOG_ERROR(_message, arg_cnt,...)          logPrint(LOG_DSP, PRINT_LEVEL_ERROR, _message, arg_cnt,##__VA_ARGS__)
#define HAL_AUDIO_LOG_WARNING(_message, arg_cnt,...)        logPrint(LOG_DSP, PRINT_LEVEL_WARNING, _message, arg_cnt,##__VA_ARGS__)
#define HAL_AUDIO_LOG_INFO(_message, arg_cnt,...)           logPrint(LOG_DSP, PRINT_LEVEL_INFO, _message, arg_cnt,##__VA_ARGS__)
#define HAL_AUDIO_LOG_PRINT(_message, arg_cnt,...)          printf(_message, ##__VA_ARGS__)
#else
//const char dsp_hal_info_MemorySramAlloc[]                   = "DSP - Hal Audio Memory SRAM alloc Type:%d, size:%d, block index:%d, Reamin_block:%d";
//const char dsp_hal_info_MemorySramFree[]                    = "DSP - Hal Audio Memory SRAM free  Type:%d, free number:%d, block index:%d, Reamin_block:%d";
//const char dsp_hal_error_SramAllocFail[]                    = "DSP - Error Hal Audio Memory SRAM alloc fail. Type:%d, size:%d, Reamin_block:%d";

#define HAL_AUDIO_LOG_ERROR(_message, arg_cnt,...)          LOG_MSGID_E(hal,_message, arg_cnt, ##__VA_ARGS__)//printf(_message, ##__VA_ARGS__)
#define HAL_AUDIO_LOG_WARNING(_message, arg_cnt,...)        LOG_MSGID_W(hal,_message, arg_cnt, ##__VA_ARGS__)//printf(_message, ##__VA_ARGS__)
#define HAL_AUDIO_LOG_INFO(_message, arg_cnt,...)           LOG_MSGID_I(hal,_message, arg_cnt, ##__VA_ARGS__)//printf(_message, ##__VA_ARGS__)//DSP_MW_LOG_I(_message, arg_cnt, ##__VA_ARGS__) //
#define HAL_AUDIO_LOG_PRINT(_message, arg_cnt,...)          LOG_MSGID_D(hal,_message, arg_cnt, ##__VA_ARGS__)//printf(_message, ##__VA_ARGS__)
#define LOG_PRINT_AUDIO(_message, arg_cnt, ...)             printf(_message, ##__VA_ARGS__)

#endif

#define OS_STRU_SEMAPHORE                                     uint32_t
#define OS_STRU_SEMAPHORE_PTR                                 uint32_t*
#define HAL_AUDIO_SEMAPHORE_INIT(x,y)                          //NULL//xSemaphoreCreateBinary()//OS_Semaphore_Init(x,y)
#define HAL_AUDIO_SEMAPHORE_TAKE(x)                           hal_nvic_save_and_set_interrupt_mask(x)//portNOP()//xSemaphoreTake( x, portMAX_DELAY)//OS_Semaphore_Take(x)
#define HAL_AUDIO_SEMAPHORE_GIVE(x)                           hal_nvic_restore_interrupt_mask(*(x))//portNOP()//xSemaphoreGiveFromISR( x, y )//OS_Semaphore_Give(x)

#include "hal_gpt.h"
#define HAL_AUDIO_DELAY_US(time_us)                         hal_gpt_delay_us(time_us)
#define HAL_AUDIO_DELAY_MS(time_ms)                         hal_gpt_delay_ms(time_ms)
#if 0
#include "timers.h"
extern bool g_run_isr;
#define HAL_AUDIO_TIMER_HANDLE                                                      TimerHandle_t
#define HAL_AUDIO_TIMER_CREATE(timer_name, time_ms, is_periodic, timer_callback)    xTimerCreate(timer_name, pdMS_TO_TICKS(time_ms), is_periodic, 0, timer_callback)
#define HAL_AUDIO_TIMER_START(timer_handler, time_ms)                                                   \
        if (g_run_isr) {                                                                                \
            BaseType_t xHigherPriorityTaskWoken;                                                        \
            xTimerChangePeriodFromISR(timer_handler, pdMS_TO_TICKS(time_ms), &xHigherPriorityTaskWoken);\
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);                                               \
        } else { xTimerChangePeriod(timer_handler, pdMS_TO_TICKS(time_ms), 0); }
#define HAL_AUDIO_TIMER_STOP(timer_handler)                                                             \
        if (g_run_isr) {                                                                                \
            BaseType_t xHigherPriorityTaskWoken;                                                        \
            xTimerStopFromISR(timer_handler, &xHigherPriorityTaskWoken);                                \
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);                                               \
        } else { xTimerStop(timer_handler, 0); }
#endif



#ifdef __cplusplus
}
#endif

#endif /*HAL_AUDIO_MODULE_ENABLED*/
#endif /* __HAL_COMMON_H__ */
