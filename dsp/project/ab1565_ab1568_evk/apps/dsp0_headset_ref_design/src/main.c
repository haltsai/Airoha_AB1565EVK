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
 
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#include "hal.h"
#include "sys_init.h"
#include "syslog.h"

#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include "timers.h"

#include "memory_map.h"
#include "string.h"


extern void DSP_Init(void);


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


#if( configUSE_MALLOC_FAILED_HOOK == 1 )
void vApplicationMallocFailedHook(void *pucPoolBuffer)
{
    #ifdef MTK_SUPPORT_HEAP_DEBUG
    extern uint8_t ucHeap[];
    extern void mp_dump_status(void *pucPoolBuffer);
    if((uint32_t)pucPoolBuffer == (uint32_t)ucHeap) {
        /* dump all heap blocks */
        //mp_dump_status(&ucHeap);

        configASSERT(0);
    }
    #else
    (void)pucPoolBuffer;
    #endif /* MTK_SUPPORT_HEAP_DEBUG */
}
#endif

#define mainCHECK_DELAY ( ( portTickType ) 1000 / portTICK_RATE_MS )//delay 1000ms

#ifdef FREERTOS_TEST
/**
* @brief       Task main function
* @param[in]   pvParameters: Pointer that will be used as the parameter for the task being created.
* @return      None.
*/
static void vTestTask(void *pvParameters)
{
    uint32_t idx = (int)pvParameters;
    portTickType xLastExecutionTime, xDelayTime;

    xLastExecutionTime = xTaskGetTickCount();
    xDelayTime = (1 << idx) * mainCHECK_DELAY;

    while (1) {
        vTaskDelayUntil(&xLastExecutionTime, xDelayTime);
        LOG_MSGID_I(common,"Hello World from task %d at %d tick", 2, idx, xTaskGetTickCount());
    }
}


/**
* @brief       Audio Task
* @param[in]   pvParameters: Pointer that will be used as the parameter for the task being created.
* @return      None.
*/
static void vAudioTask(void *pvParameters)
{
    /* DSP0 reset DSP1 */
#ifdef FLASH_BOOTING
    LOG_MSGID_I(common,"#### DSP0 Begin to Reset DSP1... ####\r\n",0);
    hal_dsp_core_reset(HAL_CORE_DSP1, DSP1_BASE);
#endif
    while(1){
        vTaskDelete(NULL);
    }
}
#endif

//#define FREERTOS_TEST
#ifdef FREERTOS_TEST
#include "FreeRTOS_test.h"
void vApplicationTickHook(void)
{
    vFreeRTOS_TestISR();
}
#endif /* FREERTOS_TEST */

#if( configUSE_IDLE_HOOK == 1 )
void vApplicationIdleHook(void)
{
    static uint32_t last_count = 0;
    uint32_t current_count = 0, time_count = 0;

    /* every >1s to dump system info */
    {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &current_count);
        hal_gpt_get_duration_count(last_count, current_count, &time_count);
        if(time_count > 32768 * 1){
            #ifdef AIR_CPU_MCPS_PRIORING_ENABLE
            extern void osStatusInfo(void);
            osStatusInfo();
            #else
            extern void osDumpHeapInfo(void);
            osDumpHeapInfo();
            #endif /* AIR_CPU_MCPS_PRIORING_ENABLE */

            last_count = current_count;
        }
    }
}
#endif /* configUSE_IDLE_HOOK == 1 */
/**
 * The entry of C code
 */

unsigned int i2, j2, k2;

//void __attribute__((optimize("O0")))
// void testloop(void)
// {
    // vTaskDelay(1000 / portTICK_PERIOD_MS);
    // while(1){
        // vTaskDelay(20 / portTICK_PERIOD_MS);
        // printf("rx state = %d\r\n", eTaskGetState(aud_cm4_msg_rx_handle));
    // }
// }
#include "hal_ccni.h"
// void _cm4_test(hal_ccni_message_t msg, hal_ccni_message_t *ack)
// {
    // printf("cb msg[0] = 0x%04x, msg[1] = 0x%04x\r\n", msg.ccni_message[0], msg.ccni_message[1]);
// }

/**
 * The entry of C code
 */
int main(void)
{
    system_init();

    LOG_MSGID_I(common,"DSP start..\r\n",0);
#ifdef FREERTOS_TEST
    LOG_MSGID_I(common,"start to run freertos test.\r\n",0);
    xTaskCreate(vMainTestTask, FREERTOS_EXAMPLE_TASK_NAME, FREERTOS_EXAMPLE_TASK_STACKSIZE / sizeof(portSTACK_TYPE), NULL, FREERTOS_EXAMPLE_TASK_PRIO, NULL);
#else
    /* Create 4 tasks*/
    for (uint32_t idx = 0; idx < 4; idx++) {
        //xTaskCreate(vTestTask, FREERTOS_EXAMPLE_TASK_NAME, FREERTOS_EXAMPLE_TASK_STACKSIZE / sizeof(StackType_t), (void *)idx, FREERTOS_EXAMPLE_TASK_PRIO, NULL);
    }
#endif /* FREERTOS_TEST */

    //aud_cm4_msg_rx_register_callback(0x0200, _cm4_test, 0);
    //aud_cm4_msg_rx_register_callback(0x0202, _cm4_test, 0);
    //aud_cm4_msg_init();

    /* Create a demo audio task */
    // xTaskCreate(vAudioTask, FREERTOS_EXAMPLE_TASK_NAME, FREERTOS_EXAMPLE_TASK_STACKSIZE / sizeof(StackType_t), NULL, FREERTOS_EXAMPLE_TASK_PRIO, NULL);
    //xTaskCreate(_james_test, "james test", FREERTOS_EXAMPLE_TASK_STACKSIZE / sizeof(StackType_t), NULL, 7, NULL);
    DSP_Init();
    LOG_MSGID_I(common,"create multi task done.",0);

    /* Call this function to indicate the system initialize done. */
    //move "write core status as ACTIVE" to middleware aud_msg_init for ccni ready.
    //hal_core_status_write(HAL_CORE_DSP0, HAL_CORE_ACTIVE);
    //LOG_MSGID_I(common,"hal_core_status_write done.", 0);

#ifdef HAL_TIME_CHECK_ENABLED
        extern exception_config_mode_t exception_config_mode;
        memcpy(&exception_config_mode, (uint8_t *)(HW_SYSRAM_PRIVATE_MEMORY_EXCEPTION_START + HW_SYSRAM_PRIVATE_MEMORY_EXCEPTION_LEN - sizeof(exception_config_mode_t)), sizeof(exception_config_mode_t));
        if (exception_config_mode.exception_mode_t.mask_irq_check_assert) {
            hal_time_check_enable();
        }
#endif

    vTaskStartScheduler();
    /* If all is well, the scheduler will now be running, and the following line
    will never be reached.  If the following line does execute, then there was
    insufficient FreeRTOS heap memory available for the idle and/or timer tasks
    to be created.  See the memory management section on the FreeRTOS web site
    for more details. */
    for (;;);
}

