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

#include "hal_time_check.h"
#include "hal.h"
#include "assert.h"
#include "memory_attribute.h"
#include "hal_log.h"
#include "hal_gpt_internal.h"
#include "hal_clock_internal.h"

#ifdef HAL_TIME_CHECK_ENABLED
#include "exception_handler.h"

ATTR_ZIDATA_IN_TCM bool is_time_check_enabled;

#ifdef HAL_TIME_CHECK_ISR_ENABLED

typedef struct {
    uint32_t irq_number;
    uint32_t execution_time;
    void    *callback;          //recording the callback for debugging
} time_check_t;

ATTR_ZIDATA_IN_TCM time_check_t time_check_buffer[IRQ_NUMBER_MAX];
ATTR_ZIDATA_IN_TCM int32_t  time_check_buffer_index;
ATTR_ZIDATA_IN_TCM uint32_t last_timestamp;

ATTR_TEXT_IN_TCM void time_check_start(uint32_t irq_number)
{
    uint32_t current_timestamp;
    uint32_t temp_duration_us;
    uint32_t mask = __get_BASEPRI();

    if (is_time_check_enabled == false) {
        return;
    }
    __set_BASEPRI(((1 << (8 - __NVIC_PRIO_BITS)) & 0xFF));//disable all IRQ
    __DMB();
    __ISB();

    current_timestamp = GPT(HAL_GPT_US_PORT)->GPT_COUNT;
    if (time_check_buffer_index != -1) { // nested
        if (current_timestamp > last_timestamp) {
            temp_duration_us = current_timestamp - last_timestamp;
        } else {
            temp_duration_us = (0xffffffff - (last_timestamp - current_timestamp)) + 1;
        }
        time_check_buffer[time_check_buffer_index].execution_time += temp_duration_us;
    }
    time_check_buffer_index++;
    time_check_buffer[time_check_buffer_index].execution_time = 0;
    time_check_buffer[time_check_buffer_index].irq_number = irq_number;
    last_timestamp = current_timestamp;

    __set_BASEPRI(mask); //enable all IRQ
    __DMB();
    __ISB();

    return ;
}

ATTR_TEXT_IN_TCM void time_check_end(uint32_t irq_number, uint32_t limter_us, void *callback)
{
    uint32_t current_timestamp;
    uint32_t temp_duration_us;
    uint32_t mask = __get_BASEPRI();

    if (is_time_check_enabled == false) {
        return;
    }
    __set_BASEPRI(((1 << (8 - __NVIC_PRIO_BITS)) & 0xFF));//disable all IRQ
    __DMB();
    __ISB();

    if ((time_check_buffer_index == -1) || (time_check_buffer[time_check_buffer_index].irq_number != irq_number)) {
#ifdef ENABLE_IRQ_HANDLER_CHECK_ASSERT
        log_hal_msgid_error("time_check_buffer_index=%d, irq_number = %d not match %d\r\n", 3, time_check_buffer_index, time_check_buffer[time_check_buffer_index].irq_number, irq_number);
        assert(0);//start and end are not match!!!
#else
        log_hal_msgid_error("time_check_buffer_index=%d, irq_number = %d not match %d\r\n", 3, time_check_buffer_index, time_check_buffer[time_check_buffer_index].irq_number, irq_number);
        return;
#endif
    }

    current_timestamp = GPT(HAL_GPT_US_PORT)->GPT_COUNT;
    if (current_timestamp > last_timestamp) {
        temp_duration_us = current_timestamp - last_timestamp;
    } else {
        temp_duration_us = (0xffffffff - (last_timestamp - current_timestamp)) + 1;
    }

    temp_duration_us += time_check_buffer[time_check_buffer_index].execution_time ;

    time_check_buffer_index--;
    last_timestamp = current_timestamp;
    __set_BASEPRI(mask); //enable all IRQ
    __DMB();
    __ISB();

    if (temp_duration_us >= limter_us) {
        //if time over than limter_us£¬ recording the callback user
        time_check_buffer[time_check_buffer_index + 1].callback = callback;
        log_hal_msgid_error("ERROR!!!!!!The IRQ%d execute time too long:%d us, callback addr:0x%x\r\n", 3, (int)irq_number, (int)temp_duration_us, (unsigned int)callback);

#ifdef ENABLE_IRQ_HANDLER_CHECK_ASSERT
        assert(0);
#endif
    }
}

#endif

#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED

ATTR_ZIDATA_IN_TCM uint32_t time_check_disbale_irq_start;
extern uint32_t mask_irq_func;
extern uint32_t backup_mask_irq_time;

ATTR_TEXT_IN_TCM void hal_time_check_disable_irq_start(void)
{
    if (is_time_check_enabled == false) {
        return ;
    }
    time_check_disbale_irq_start = GPT(HAL_GPT_US_PORT)->GPT_COUNT;
    return;
}

ATTR_TEXT_IN_TCM bool hal_time_check_disable_irq_end(uint32_t limter_us)
{
    uint32_t temp_time_end, temp_duration_us;
    uint32_t clock;
    if (is_time_check_enabled == false) {
        return false ;
    }

    temp_time_end = GPT(HAL_GPT_US_PORT)->GPT_COUNT;
    if (temp_time_end > time_check_disbale_irq_start) {
        temp_duration_us = temp_time_end - time_check_disbale_irq_start;
    } else {
        temp_duration_us = (0xffffffff - (time_check_disbale_irq_start - temp_time_end)) + 1;
    }
    backup_mask_irq_time = temp_duration_us;
    if (temp_duration_us >= limter_us) {
        if ((clock = hal_clock_get_freq_meter(_hf_fsys_ck, 100)) > TIME_CHECK_MIN_SYSTEM_FREQ) {
#ifdef ENABLE_REAL_TIME_CHECK_ASSERT
            ram_assert(0);
#endif
        }
        return false;
    }

    return true;
}

#endif

void hal_time_check_enable(void)
{
    uint32_t temp_counter;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &temp_counter);

    if (is_time_check_enabled == false) {
#ifdef HAL_TIME_CHECK_ISR_ENABLED
        time_check_buffer_index = -1;
        time_check_buffer[0].execution_time = 0;
#endif
        is_time_check_enabled = true;
    }
}

void hal_time_check_disable(void)
{

    is_time_check_enabled = false;
}

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif

#endif /* HAL_TIME_CHECK_ENABLED */


