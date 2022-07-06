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
 
#include "hal_sleep_manager.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_gpt.h"
#include "hal_gpt_internal.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#ifdef HAL_SLEEP_MANAGER_LOCK_SLEEP_CROSS_CORE
#include "hal_ccni.h"
#include "hal_ccni_config.h"
#include "hal_clock.h" 
#include "hal_clock_internal.h"
#endif
#include "assert.h"
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif /* MTK_SWLA_ENABLE */

static uint32_t sleep_sw_gpt_handle;

void hal_sleep_manager_sw_gpt_callback()
{
    
}

uint8_t hal_sleep_manager_set_sleep_handle(const char *handle_name)
{
    uint8_t index;
    index = sleep_management_get_lock_handle(handle_name);
    return index;
}

#ifdef HAL_SLEEP_MANAGER_LOCK_SLEEP_CROSS_CORE
void sleep_lock_dsp1_to_dsp0(hal_ccni_event_t event, void * msg)
{
    uint32_t *pMsg = msg;
    if(event!=CCNI_DSP1_TO_DSP0_LOCK_SLEEP)
    {
        assert(0);
    }

    sleep_management_lock_sleep((uint8_t)pMsg[1],(uint8_t)pMsg[0]);

    hal_ccni_clear_event(CCNI_DSP1_TO_DSP0_LOCK_SLEEP);
    hal_ccni_unmask_event(CCNI_DSP1_TO_DSP0_LOCK_SLEEP); // unmask the event.
}
#endif

hal_sleep_manager_status_t hal_sleep_manager_lock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_LOCK_SLEEP_CROSS_CORE
    //ccci_msg_t ccci_msg_for_lock_sleep;
    volatile hal_ccni_status_t status;
    hal_ccni_message_t info;
    volatile hal_ccni_status_t ret;
    if( (handle_index>=SLEEP_LOCK_DSP0_CROSS_CORE_START) && (handle_index<=SLEEP_LOCK_DSP0_CROSS_CORE_END))
    {
        return HAL_SLEEP_MANAGER_ERROR;
    }
    if( (handle_index>=SLEEP_LOCK_DSP1_CROSS_CORE_START) && (handle_index<=SLEEP_LOCK_DSP1_CROSS_CORE_END))
    {
     #if 0
        ccci_msg_for_lock_sleep.event = CCCI_EVENT_DSP0_TO_DSP1_WAKEUP_AND_LOCK_SLEEP;
        ccci_msg_for_lock_sleep.data = handle_index;
        if(CCCI_STATUS_OK != ccci_send_msg(HAL_CORE_DSP1,ccci_msg_for_lock_sleep,CCCI_SEND_MSG_WAIT_FOR_SEND_RECEIVE_DONE))
        {
            return HAL_SLEEP_MANAGER_ERROR;
        }
     #endif
     info.ccni_message[0] = handle_index;
     info.ccni_message[1] = LOCK_SLEEP;
     do{
         status = hal_ccni_set_event(CCNI_DSP0_TO_DSP1_LOCK_SLEEP,&info);
         if(status == HAL_CCNI_STATUS_BUSY)
         {
            log_hal_msgid_info("CCNI of CCNI_DSP0_TO_DSP1_LOCK_SLEEP busy!!! try again!!!", 0);
            hal_gpt_delay_ms(1);
         }
     }while(status == HAL_CCNI_STATUS_BUSY);
     do{
          ret = hal_ccni_query_event_status(CCNI_DSP0_TO_DSP1_LOCK_SLEEP, (uint32_t*)&status);
          if (HAL_CCNI_STATUS_OK != ret)
          {
                log_hal_msgid_info("CCNI of CCNI_DSP0_TO_DSP1_LOCK_SLEEP lock sleep error!!!", 0);
                return HAL_SLEEP_MANAGER_ERROR;
          }
         if(status == HAL_CCNI_EVENT_STATUS_IDLE)
             break;
         log_hal_msgid_info("CCNI of CCNI_DSP0_TO_DSP1_LOCK_SLEEP busy!!! wait for idle!!!", 0);
         hal_gpt_delay_ms(1);
     }while(1);
     return HAL_SLEEP_MANAGER_OK;
    }
#endif
    sleep_management_lock_sleep(LOCK_SLEEP, handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_unlock_sleep(uint8_t handle_index)
{
#ifdef HAL_SLEEP_MANAGER_LOCK_SLEEP_CROSS_CORE
    //ccci_msg_t ccci_msg_for_lock_sleep;
    volatile hal_ccni_status_t status;
    hal_ccni_message_t info;
    //volatile hal_ccni_status_t ret;
    if( (handle_index>=SLEEP_LOCK_DSP0_CROSS_CORE_START) && (handle_index<=SLEEP_LOCK_DSP0_CROSS_CORE_END))
    {
        return HAL_SLEEP_MANAGER_ERROR;
    }  
    if( (handle_index>=SLEEP_LOCK_DSP1_CROSS_CORE_START) && (handle_index<=SLEEP_LOCK_DSP1_CROSS_CORE_END))
    {
     info.ccni_message[0] = handle_index;
     info.ccni_message[1] = UNLOCK_SLEEP;
     do{
         status = hal_ccni_set_event(CCNI_DSP0_TO_DSP1_LOCK_SLEEP,&info);
         if(status == HAL_CCNI_STATUS_BUSY)
         {
            log_hal_msgid_info("CCNI of CCNI_DSP0_TO_DSP1_LOCK_SLEEP(unlock) busy!!! try again!!!", 0);
             hal_gpt_delay_ms(10);
         }
     }while(status == HAL_CCNI_STATUS_BUSY);
     #if 0
     do{
          ret = hal_ccni_query_event_status(CCNI_DSP0_TO_DSP1_LOCK_SLEEP, &status);
          if (HAL_CCNI_STATUS_OK != ret)
          {
                return HAL_SLEEP_MANAGER_ERROR;
          }
         if(status == HAL_CCNI_EVENT_STATUS_IDLE)
             break;
         log_hal_msgid_info("CCNI of CCNI_DSP0_TO_DSP1_LOCK_SLEEP(unlock) busy!!! wait for idle!!!", 0);
         hal_gpt_delay_ms(10);
     }while(1);
     #endif
     return HAL_SLEEP_MANAGER_OK;
    }
#endif
    sleep_management_lock_sleep(UNLOCK_SLEEP, handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_release_sleep_handle(uint8_t handle_index)
{
    if (hal_sleep_manager_get_lock_status() & (1 << handle_index)) {
        log_hal_msgid_error("handle %d is currently holding a lock, cannot release\n", 1, handle_index);
        return HAL_SLEEP_MANAGER_ERROR;
    }
    sleep_management_release_lock_handle(handle_index);
    return HAL_SLEEP_MANAGER_OK;
}

uint32_t hal_sleep_manager_get_lock_status(void)
{
    return sleep_management_get_lock_sleep_request_info();
}

uint32_t hal_sleep_manager_sleep_driver_dump_handle_name(void)
{
    return sleep_management_get_lock_sleep_handle_list();
}

bool hal_sleep_manager_is_sleep_locked(void)
{
    return sleep_management_check_sleep_locks();
}

bool hal_sleep_manager_is_sleep_handle_alive(uint8_t handle_index)
{
    return sleep_management_check_handle_status(handle_index);
}

#ifdef HAL_SLEEP_MANAGER_SUPPORT_POWER_OFF
void hal_sleep_manager_enter_power_off_mode()
{

}
#endif

hal_sleep_manager_status_t hal_sleep_manager_set_sleep_time(uint32_t sleep_time_ms)
{
    hal_gpt_status_t    ret_status;

    if (sleep_time_ms > HAL_GPT_MAXIMUM_MS_TIMER_TIME) {
        sleep_time_ms = HAL_GPT_MAXIMUM_MS_TIMER_TIME;
    }

    hal_gpt_sw_stop_timer_ms(sleep_sw_gpt_handle);
    ret_status = hal_gpt_sw_start_timer_ms(sleep_sw_gpt_handle, sleep_time_ms, (hal_gpt_callback_t)hal_sleep_manager_sw_gpt_callback, NULL);
    if (ret_status != HAL_GPT_STATUS_OK) {
        log_hal_error("ERROR : Deep Sleep GPT Start Fail %d",ret_status);
        return HAL_SLEEP_MANAGER_ERROR;
    }    

    return HAL_SLEEP_MANAGER_OK;
}

void hal_sleep_manager_enter_sleep_mode(hal_sleep_mode_t mode)
{
    if (mode == HAL_SLEEP_MODE_IDLE) {
        #ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("wfi",SA_START);
        #endif /* MTK_SWLA_ENABLE */

        __asm__ __volatile__ (  " dsync             \n"
                                " waiti 0           \n"
                                " isync             \n");   

        #ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("wfi",SA_STOP);
        #endif /* MTK_SWLA_ENABLE */

    } else if (mode == HAL_SLEEP_MODE_SLEEP) {
        #ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("dpm",SA_START);
        #endif /* MTK_SWLA_ENABLE */

        hal_core_status_write(HAL_CORE_DSP0,HAL_CORE_SLEEP);
        sleep_management_enter_deep_sleep();
        hal_core_status_write(HAL_CORE_DSP0,HAL_CORE_ACTIVE);

        #ifdef MTK_SWLA_ENABLE
        SLA_CustomLogging("dpm",SA_STOP);
        #endif /* MTK_SWLA_ENABLE */
    } 
}

#ifdef HAL_SLEEP_MANAGER_SUPPORT_WAKEUP_SOURCE_CONFIG
hal_sleep_manager_status_t hal_sleep_manager_enable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
{
    spm_unmask_wakeup_source(pin);
    return HAL_SLEEP_MANAGER_OK;
}

hal_sleep_manager_status_t hal_sleep_manager_disable_wakeup_pin(hal_sleep_manager_wakeup_source_t pin)
{
    spm_mask_wakeup_source(pin);
    return HAL_SLEEP_MANAGER_OK;
}
#endif

extern void uart_backup_all_registers();
extern void uart_restore_all_registers();
uint8_t sleep_manager_handle;
hal_sleep_manager_status_t hal_sleep_manager_init()
{
    //log_hal_msgid_info("hal_sleep_manager_init start\n", 0);

    spm_init();
    
    sleep_manager_handle = hal_sleep_manager_set_sleep_handle("slp");
    //hal_sleep_manager_lock_sleep(sleep_manager_handle);
    
    if (hal_gpt_sw_get_timer(&sleep_sw_gpt_handle) != HAL_GPT_STATUS_OK) {
        log_hal_msgid_error("ERROR : Deep Sleep GPT Init Fail", 0);
        return HAL_SLEEP_MANAGER_ERROR;
    }
    sleep_management_dsp_status.wakeup_source = 0;
    sleep_management_dsp_status.abort_sleep = 0;
    sleep_management_dsp_status.pshut_off = 0;
    sleep_management_dsp_status.bootvector_backup = 0;
    sleep_management_dsp_status.bootvector_address = (uint32_t)(&MCU_CFG_PRI->DSP0CFG_BOOT_VECTOR);    
    
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_UART,uart_backup_all_registers,NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_UART,uart_restore_all_registers,NULL);     
    
    return HAL_SLEEP_MANAGER_OK;
}

#endif /* HAL_SLEEP_MANAGER_ENABLED */
