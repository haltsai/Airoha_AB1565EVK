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



#include "FreeRTOS.h"
#include "hal_gpt.h"
#include "hal_ccni.h"
#include "pka_porting_layer.h"
#include "semphr.h"
#include "task.h"
#include "timers.h"
#include "hal_nvic_internal.h"
#include "hal_nvic.h"
#include "memory_attribute.h"
#include "task_def.h"
#include "uECC.h"
#include "avm_external.h"
#include "hal_dvfs.h"
#include "hal_platform.h"
#include "syslog.h"

#if !defined(MBEDTLS_CONFIG_FILE)^M
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include "mbedtls/md.h"
#include "mbedtls/sha256.h"

#include "hal_ccni.h"
#include "hal_ccni_config.h"
#ifdef HAL_AUDIO_MODULE_ENABLED
#include "hal_audio_internal.h"
#endif

#include "hal_rtc_internal.h"
#include "hal_dwt.h"

ATTR_TEXT_IN_TCM char* pka_os_get_task_name(void)
{
    return pcTaskGetTaskName(NULL);
}

ATTR_TEXT_IN_TCM void pka_os_task_resume(void* taskHandle)
{
    vTaskResume(taskHandle);
}


ATTR_TEXT_IN_TCM uint32_t  pka_os_task_resume_from_isr(void* taskHandle)
{
    return xTaskResumeFromISR(taskHandle);
}

ATTR_TEXT_IN_TCM void pka_os_yield_from_isr(void)
{
    portYIELD_FROM_ISR(true);
}

ATTR_TEXT_IN_TCM uint32_t pka_os_is_run_in_isr(void)
{
    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > 0) {
        /* is ISR context */
        return true;
    } else {
        /* is Task context */
        return false;
    }
}


ATTR_TEXT_IN_TCM void *pka_os_get_timer_id( void * xTimer )
{
    return pvTimerGetTimerID(xTimer);
}


ATTR_TEXT_IN_TCM void pka_os_cancel_timer( void * xTimer )
{
    xTimerDelete(xTimer, 0);
}


ATTR_TEXT_IN_TCM void* pka_os_create_timer( const char * const pcTimerName, const uint32_t  xTimerPeriodInTicks, const uint32_t  uxAutoReload, void * const pvTimerID, TimerCallbackFunction_t pxCallbackFunction )
{

    return xTimerCreate(pcTimerName, xTimerPeriodInTicks, uxAutoReload, pvTimerID, pxCallbackFunction);

}

ATTR_TEXT_IN_TCM void pka_os_start_timer( void * xTimer )
{
    xTimerStart(xTimer, 0);
}



ATTR_TEXT_IN_TCM void pka_os_task_suspend(void* taskHandle)
{
    vTaskSuspend(taskHandle);
}

ATTR_TEXT_IN_TCM void pka_os_task_notify_wait(uint32_t  ulBitsToClearOnEntry, uint32_t  ulBitsToClearOnExit, uint32_t  *pulNotificationValue)
{
    xTaskNotifyWait( ulBitsToClearOnEntry, ulBitsToClearOnExit, pulNotificationValue, portMAX_DELAY);
}

ATTR_TEXT_IN_TCM void pka_os_task_notify_from_isr( void* xTaskToNotify, uint32_t  ulValue, xNotifyAction eAction, BaseType_t *pxHigherPriorityTaskWoken )
{
    xTaskNotifyFromISR(xTaskToNotify, ulValue, eAction,pxHigherPriorityTaskWoken);
}

ATTR_TEXT_IN_TCM void pka_os_task_notify( void* xTaskToNotify, uint32_t  ulValue, xNotifyAction eAction)
{
    xTaskNotify(xTaskToNotify, ulValue, eAction);
}



void* pka_os_semaphore_init()
{
    return xSemaphoreCreateRecursiveMutex();
}



ATTR_TEXT_IN_TCM void pka_os_semaphore_take(void* pSemaphore)
{
    if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }

 //   if(port_OsStart)
    {
        xSemaphoreTakeRecursive( pSemaphore, portMAX_DELAY );
    }
}

ATTR_TEXT_IN_TCM void pka_os_semaphore_give(void* pSemaphore)
{
    if(xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED) {
        return;
    }


    //if(port_OsStart)
    {
        xSemaphoreGiveRecursive( pSemaphore );
    }
}


void pka_os_task_create(TaskFunction_t fTaskEntry, const char * const taskName, uint32_t  stackSize, void * const pParameters, uint8_t  taskPriority, void* taskHandle)
{
    xTaskCreate( fTaskEntry, taskName, stackSize, pParameters, taskPriority, taskHandle );

}


ATTR_TEXT_IN_TCM uint32_t  pka_os_get_interrupt_mask(void)
{
    uint32_t  nvic_mask;
    hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    return nvic_mask;
}

ATTR_TEXT_IN_TCM void pka_os_restore_interrupt_mask(uint32_t  nvic_mask)
{
    hal_nvic_restore_interrupt_mask(nvic_mask);
}

ATTR_TEXT_IN_TCM uint32_t pka_gpt_get_current_time(void)
{
    uint32_t cur_time;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &cur_time);

    return cur_time;
}

void pka_os_register_bt_interrupt(bt_isr_t intr_handler)
{
    hal_nvic_register_isr_handler(BT_IRQn, (hal_nvic_isr_t)intr_handler);
}

void pka_os_register_bt_timer_interrupt(bt_isr_t intr_handler)
{
    hal_nvic_register_isr_handler(BT_TIMER_IRQn, (hal_nvic_isr_t)intr_handler);
}

void pka_os_enable_bt_and_timer_interrupt()
{
    hal_nvic_enable_irq(BT_IRQn);
    hal_nvic_enable_irq(BT_TIMER_IRQn);
}

void pka_os_disable_bt_and_timer_interrupt()
{
    hal_nvic_disable_irq(BT_IRQn);
    hal_nvic_disable_irq(BT_TIMER_IRQn);
}

uint32_t pka_os_get_lm_task_priority(void)
{
    return TASK_PRIORITY_SOFT_REALTIME;
}

uint32_t pka_os_get_lc_task_priority(void)
{
    return TASK_PRIORITY_BT_CMD_TASK;
}
uint32_t pka_os_get_lc_process_task_priority(void)
{
    return TASK_PRIORITY_BT_ROUTINE_TASK;
}


volatile uint32_t nvic_iser0 = 0;
volatile uint32_t nvic_iser1 = 0;
#define NVIC_BT_IRQS_MASK (0x0f)
#define NVIC_32_BIT_REG_MASK (0x1f)

/*Disable all irq except BT irq.*/
ATTR_TEXT_IN_TCM void pka_disable_all_irq_except_bt(void)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    nvic_iser0 = NVIC->ISER[0];
    nvic_iser1 = NVIC->ISER[1];

	NVIC->ICER[0] = nvic_iser0;
	NVIC->ICER[1] = nvic_iser1 & (~(NVIC_BT_IRQS_MASK << (BT_IRQn & NVIC_32_BIT_REG_MASK)));
    hal_nvic_restore_interrupt_mask(mask);
}

ATTR_TEXT_IN_TCM void pka_restore_all_irq_except_bt(void)
{
    uint32_t mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
	NVIC->ISER[0] = nvic_iser0;
	NVIC->ISER[1] = NVIC->ISER[1] | (nvic_iser1 & (~(NVIC_BT_IRQS_MASK << (BT_IRQn & NVIC_32_BIT_REG_MASK))));
	nvic_iser0 = 0;
	nvic_iser1 = 0;
    hal_nvic_restore_interrupt_mask(mask);


}

int uECC_random(uint8_t *dest, unsigned size)
{
    uint32_t random_seed, i;
    uint32_t words = size/4;
    uint32_t remain = (size - (words * 4));
    for (i = 0; i < words; i++)
    {
        if (hal_trng_get_generated_random_number(&random_seed) == HAL_TRNG_STATUS_OK)
        {
            *((uint32_t*)dest + i) = random_seed;
        }
        else
        {
            return 0;
        }
    }

    if (hal_trng_get_generated_random_number(&random_seed) == HAL_TRNG_STATUS_OK)
    {
        for (i = 0; i < remain; i++)
        {
            dest[4 * words + i] = (random_seed >> (8 * i)) & 0xFF;
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

void pka_uECC_p192_compute_public_key(uint8_t *privatekey, uint8_t *publickey)
{
    //using this API to generate random private key and public key
    uECC_set_rng(&uECC_random);
    uECC_make_key(publickey, privatekey, uECC_secp192r1());
}

void pka_uECC_p192_shared_secret(uint8_t * remotePublicKey, uint8_t *privatekey, uint8_t *DHKey)
{
    /* big endian input */
    uECC_shared_secret(remotePublicKey,privatekey, DHKey, uECC_secp192r1());
}

int8_t pka_hal_gpt_delay_us( uint32_t us)
{
    return hal_gpt_delay_us(us);
}



uint32_t pka_hal_gpt_get_free_run_count_1M(void)
{
    uint32_t count = 0;
    int32_t ret = 0;

    ret = (int32_t)hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &count);

    if (ret < 0)
    {
        return 0;
    }
    else
    {
        return count;
    }
}


void pka_uECC_p256_compute_public_key(uint8_t *privatekey, uint8_t *publickey)
{
    //using this API to generate random private key and public key
    uECC_set_rng(&uECC_random);
    uECC_make_key(publickey, privatekey, uECC_secp256r1());
}

void pka_uECC_p256_shared_secret(uint8_t * remotePublicKey, uint8_t *privatekey, uint8_t *DHKey)
{
    /* big endian input */
    uECC_shared_secret(remotePublicKey,privatekey, DHKey, uECC_secp256r1());
}

void pka_external_hmac_sha256(uint8_t *input, uint8_t ilen, uint8_t *key, uint8_t keylen, uint8_t *output)
{
#ifdef MBEDTLS_CONFIG_FILE
    /* big endian input */
    mbedtls_md_hmac(mbedtls_md_info_from_type(MBEDTLS_MD_SHA256),
            key, keylen, input, ilen, output);
#endif
}

void pka_external_sha256(uint8_t *input, uint8_t ilen, uint8_t *output)
{
#ifdef MBEDTLS_CONFIG_FILE
    mbedtls_sha256_ret( input, ilen, output, 0);
#endif
}

void pka_dvfs_lock_control_SpeedUpTo208M(uint8_t lock)
{
    hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M,lock);;
}

void pka_dvfs_lock_control_SpeedUpTo104M(uint8_t lock)
{
    hal_dvfs_lock_control(HAL_DVFS_FULL_SPEED_104M,lock);;
}

void pka_hal_nvic_save_and_set_interrupt_mask_special(uint32_t* mask)
{
    hal_nvic_save_and_set_interrupt_mask_special(mask);
}

void pka_hal_nvic_restore_interrupt_mask_special(uint32_t mask)
{
    hal_nvic_restore_interrupt_mask_special(mask);
}

void pka_llcp_print(uint8_t* lmp_log_buffer_array,uint32_t* lmp_log_buffer_length_array,uint32_t ret_len)
{
    LOG_TLVDUMP_I(common, LOG_TYPE_BT_LMP_LLCP_DATA, lmp_log_buffer_array, lmp_log_buffer_length_array, ret_len);
}

#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "bt_sink_srv_ami.h"



uint32_t pka_Send_LEAudio_PlayInfo_ToDSP(uint32_t Address)
{
    bt_sink_srv_am_feature_t feature_param;
    ble_init_play_info_t *play_info;

    LOG_MSGID_I(common, "[BLE] pka_Send_LEAudio_PlayInfo_ToDSP enter", 0);

    play_info = (ble_init_play_info_t *)Address;
    LOG_MSGID_I(common, "[BLE] play_info->iso_interval %d", 1, play_info->iso_interval);
    LOG_MSGID_I(common, "[BLE] play_info->dl_timestamp_clk %d", 1, play_info->dl_timestamp_clk);
    LOG_MSGID_I(common, "[BLE] play_info->dl_timestamp_phase %d", 1, play_info->dl_timestamp_phase);
    LOG_MSGID_I(common, "[BLE] play_info->dl_retransmission_window_clk %d", 1, play_info->dl_retransmission_window_clk);
    LOG_MSGID_I(common, "[BLE] play_info->dl_retransmission_window_phase %d", 1, play_info->dl_retransmission_window_phase);
    LOG_MSGID_I(common, "[BLE] play_info->dl_ft %d", 1, play_info->dl_ft);
    LOG_MSGID_I(common, "[BLE] play_info->dl_packet_counter %d", 1, play_info->dl_packet_counter);
    LOG_MSGID_I(common, "[BLE] play_info->ul_timestamp %d", 1, play_info->ul_timestamp);
    LOG_MSGID_I(common, "[BLE] play_info->ul_ft %d", 1, play_info->ul_ft);
    LOG_MSGID_I(common, "[BLE] play_info->ul_packet_counter %d", 1, play_info->ul_packet_counter);

    feature_param.type_mask = AM_BLE;
    feature_param.feature_param.ble_param.event = BLE_FEATURE_EVENT_PLAY_INFO;
    feature_param.feature_param.ble_param.param.play_info = *(ble_init_play_info_t *)Address;

    return am_audio_set_feature_ISR(FEATURE_NO_NEED_ID, &feature_param);
}
#else
uint32_t pka_Send_LEAudio_PlayInfo_ToDSP(uint32_t Address)
{
    return 0xFF;
}
#endif //end of #ifdef AIR_BT_CODEC_BLE_ENABLED

ATTR_TEXT_IN_TCM uint32_t pka_hal_ccni_mask_event(uint32_t event)
{
    return hal_ccni_mask_event(event);
}

ATTR_TEXT_IN_TCM uint32_t pka_hal_ccni_clear_event(uint32_t event)
{
    return hal_ccni_clear_event(event);
}

ATTR_TEXT_IN_TCM uint32_t pka_hal_ccni_unmask_event(uint32_t event)
{
    return hal_ccni_unmask_event(event);
}

ATTR_TEXT_IN_TCM uint32_t pka_hal_ccni_set_event(uint32_t event, hal_ccni_message_t *message)
{
    return hal_ccni_set_event(CCNI_EVENT_SRC_CM4|CCNI_EVENT_DST_DSP0|event,message);
}

#if (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
extern void LC_ULL_DataEncodeRdy (uint32_t event, uint32_t *pMsg);

ATTR_TEXT_IN_TCM void bt_controller_receive_ull_encode_started_msg_from_dsp0(hal_ccni_event_t event, void *msg)
{
    LC_ULL_DataEncodeRdy(event,msg);
}

#include "bt_sink_srv_ami.h"

ATTR_TEXT_IN_TCM  void pka_set_ull_play_en(uint32_t seq_no, uint32_t bt_clk)
{
    bt_sink_srv_am_feature_t feature_param;
    feature_param.type_mask = AM_AUDIO_BT_SET_PLAY_EN;
    feature_param.feature_param.play_en_param.sequence_number= seq_no;
    feature_param.feature_param.play_en_param.bt_clock = bt_clk;
    am_audio_set_feature_ISR(FEATURE_NO_NEED_ID,&feature_param);

    //hal_audio_set_a2dp_play_en(seq_no, bt_clk);
}

#endif

#include "hal_hw_semaphore.h"
#define HwSemRetryTimes 50000

volatile uint32_t int_mask;
ATTR_TEXT_IN_TCM  void pka_HWSemaphoreTake(void)
{
    // uint32_t int_mask;
    uint32_t take_times = 0;

    /* Add hw semaphore to avoid multi-core access */
    while( ++take_times ) {
        hal_nvic_save_and_set_interrupt_mask((uint32_t*)&int_mask);
        if (HAL_HW_SEMAPHORE_STATUS_OK == hal_hw_semaphore_take(4)) {
            break;
        }

        if( take_times > HwSemRetryTimes ) {
            hal_nvic_restore_interrupt_mask(int_mask);
            //error handling
            // printf("%s : Can not take HW Semaphore\r\n", __func__);
            configASSERT(0);
        }
        hal_nvic_restore_interrupt_mask(int_mask);
        // vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

ATTR_TEXT_IN_TCM  void pka_HWSemaphoreGive(void)
{
    // uint32_t int_mask;
    if ( HAL_HW_SEMAPHORE_STATUS_OK == hal_hw_semaphore_give(4) ) {
        hal_nvic_restore_interrupt_mask(int_mask);
    } else {
        hal_nvic_restore_interrupt_mask(int_mask);

        //error handling
        // printf("%s : Can not give HW Semaphore\r\n", __func__);
        configASSERT(0);
    }
}
void pka_rtc_switch_to_dcxo(bool enable)
{
        hal_rtc_switch_to_dcxo(HAL_RTC_CLOCK_USER_CONSYS, enable);
}

/*
 *   Input : Set addr_base which you want to monitor
 *         : Set NULL to disable to monitor
 */
void pka_dwt_requsest_watchpoint(uint32_t addr_base)
{
    hal_dwt_request_watchpoint(HAL_DWT_1, addr_base, 2, WDE_DATA_WO);
}
