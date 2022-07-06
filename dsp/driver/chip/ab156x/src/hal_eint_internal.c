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

/*************************************************************************
 * Included header files
 *************************************************************************/

#include "hal_eint.h"

#ifdef HAL_EINT_MODULE_ENABLED
#include "hal_eint_internal.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "memory_attribute.h"

#ifdef HAL_TIME_CHECK_ENABLED
#include "hal_time_check.h"
#endif


EINT_REGISTER_T *EINT_REGISTER = (EINT_REGISTER_T *)EINT_BASE;
// ATTR_RWDATA_IN_TCM EINT_SRC_SELECT_REGISTER_T *EINT_SRC_SELECT = (EINT_SRC_SELECT_REGISTER_T *)(TOP_MISC_CFG_BASE+0x500);

eint_function_t eint_function_table[HAL_EINT_NUMBER_MAX];

uint32_t eint_get_status(void)
{
    return (EINT_REGISTER->EINT_STA);
}

uint32_t eint_get_status1(void)
{
    return (EINT_REGISTER->EINT_STA1);
}


void eint_ack_interrupt(uint32_t eint_number)
{
    if (eint_number < EINT_GROUP_MAX_NUMBER) {
        EINT_REGISTER->EITN_INTACK = (1 << eint_number);
    } else {
        EINT_REGISTER->EITN_INTACK1 = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
    }
}

void eint_ack_wakeup_event(uint32_t eint_number)
{
    if (eint_number < EINT_GROUP_MAX_NUMBER) {
        EINT_REGISTER->EINT_EEVTACK = (1 << eint_number);
    } else {
        EINT_REGISTER->EINT_EEVTACK1 = (1 << (eint_number & (EINT_GROUP_MAX_NUMBER - 1)));
    }
}

uint32_t eint_caculate_debounce_time(uint32_t ms)
{
    uint32_t prescaler;
    uint32_t count;

    if (ms == 0) {
        /* set to one 32KHz clock cycle */
        prescaler = EINT_CON_PRESCALER_32KHZ;
        count = 0;
    } else if (ms <= 62) {
        prescaler = EINT_CON_PRESCALER_32KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,32768);
    } else if (ms <= 125) {
        prescaler = EINT_CON_PRESCALER_16KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,16384);
    } else if (ms <= 250) {
        prescaler = EINT_CON_PRESCALER_8KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,8192);
    } else if (ms <= 500) {
        prescaler = EINT_CON_PRESCALER_4KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,4096);
    } else if (ms <= 1000) {
        prescaler = EINT_CON_PRESCALER_2KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,2048);
    } else if (ms <= 2000) {
        prescaler = EINT_CON_PRESCALER_1KHZ;
        count = EINT_TIME_MS_TO_COUNT(ms,1024);
    } else if (ms <= 4000) {
        prescaler = EINT_CON_PRESCALER_512HZ;
        count = EINT_TIME_MS_TO_COUNT(ms,512);
    } else if (ms <= 8000) {
        prescaler = EINT_CON_PRESCALER_256HZ;
        count = EINT_TIME_MS_TO_COUNT(ms,256);
    } else {
        /* set to maximum prescaler/count */
        prescaler = EINT_CON_PRESCALER_256HZ;
        count = EINT_CON_DBC_CNT_MASK;
    }

    if (count > EINT_CON_DBC_CNT_MASK) {
        count = EINT_CON_DBC_CNT_MASK;
    }

    count = (count | EINT_CON_DBC_EN_MASK |
             (EINT_CON_PRESCALER_MASK & (prescaler << EINT_CON_PRESCALER_OFFSET)));
    return count;
}

void hal_eint_isr(hal_nvic_irq_t index)
{
    uint32_t status = 0;
    uint32_t status1 = 0;
    uint32_t eint_index = 0;

    status = eint_get_status();
    status1 = eint_get_status1();
    index = index;

    if (status != 0) {
        for (eint_index = 0; eint_index < EINT_GROUP_MAX_NUMBER; eint_index++) {
            if (status & (1 << eint_index)) {
                if (eint_function_table[eint_index].eint_callback) {
                    hal_eint_mask((hal_eint_number_t)eint_index);
                    eint_ack_interrupt(eint_index);
                    eint_ack_wakeup_event(eint_index);

#ifdef HAL_TIME_CHECK_ISR_ENABLED
                    time_check_start(eint_index + 1000);
#endif
                    eint_function_table[eint_index].eint_callback(eint_function_table[eint_index].user_data);
#ifdef HAL_TIME_CHECK_ISR_ENABLED
                    time_check_end(eint_index + 1000, TIME_CHECK_ISR_TIME, (void *)eint_function_table[eint_index].eint_callback);
#endif
                } else {
                    //log_hal_msgid_error("ERROR: no EINT interrupt handler!\n", 0);
                }
            }
        }
    }

    if (status1 != 0) {
        for (eint_index = EINT_GROUP_MAX_NUMBER; eint_index < HAL_EINT_NUMBER_MAX; eint_index++) {
            if (status1 & (1 << (eint_index & (EINT_GROUP_MAX_NUMBER - 1)))) {
                if (eint_function_table[eint_index].eint_callback) {
                    hal_eint_mask((hal_eint_number_t)eint_index);
                    eint_ack_interrupt(eint_index);
                    eint_ack_wakeup_event(eint_index);

#ifdef HAL_TIME_CHECK_ISR_ENABLED
                    time_check_start(eint_index + 1000);
#endif
                    eint_function_table[eint_index].eint_callback(eint_function_table[eint_index].user_data);
#ifdef HAL_TIME_CHECK_ISR_ENABLED
                    time_check_end(eint_index + 1000, TIME_CHECK_ISR_TIME, (void *)eint_function_table[eint_index].eint_callback);
#endif
                } else {
                    //log_hal_msgid_error("ERROR: no EINT interrupt handler!\n", 0);
                }

            }
        }
    }

}


#endif /* HAL_EINT_MODULE_ENABLED */

