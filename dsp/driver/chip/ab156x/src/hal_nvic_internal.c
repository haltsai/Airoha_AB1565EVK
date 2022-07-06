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
 
#include "hal_nvic_internal.h"

#ifdef HAL_NVIC_MODULE_ENABLED
#include "hal_nvic.h"
#include "memory_attribute.h"

#ifdef HAL_TIME_CHECK_ENABLED
#include"hal_time_check.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

ATTR_ZIDATA_IN_DRAM int32_t nvic_irq_execution_number;


void nvic_clear_all_pending_interrupt(void)
{
	xthal_set_intclear(0xffffffff);
}


hal_nvic_status_t nvic_irq_software_trigger(hal_nvic_irq_t irq_number)
{
    if (irq_number >= IRQ_NUMBER_MAX) {
        return HAL_NVIC_STATUS_INVALID_PARAMETER;
    }

    xthal_set_intset(1<<irq_number);
    
    return HAL_NVIC_STATUS_OK;
}



/**
 * @brief This function is used to return the DSP status.
 * @return    To indicate whether this function call is successful.
 *            If the return value is not zero, the DSP is executing excetpion handler;
 *            If the return value is less than zero, the DSP is executing normal code.
 */
ATTR_TEXT_IN_IRAM int32_t hal_nvic_query_exception_number(void)
{
    return nvic_irq_execution_number;
}

#if 0

/**
 * @brief This function saves the current IRQ settings in an variable, and then disables the IRQ by setting the IRQ mask.
 *  It should be used in conjunction with #nvic_restore_all_irq() to protect the critical resources.
 *  This function is defined only for MD and OS!!!
 * @param[out] mask is used to store the current IRQ setting, upon the return of this function.
 * @return    IRQ settings.
 */
ATTR_TEXT_IN_TCM uint32_t save_and_set_interrupt_mask(void)
{
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    
    return mask;
}


/**
 * @brief This function restores the IRQ settings as specified in the mask. It should be used in conjunction with
 *  #nvic_save_and_set_all_irq() to protect critical resources.
 *  This function is defined only for MD and OS!!!
 * @param[in] mask is an unsigned integer to specify the IRQ settings.
 * @return    N/A.
 */
ATTR_TEXT_IN_TCM void restore_interrupt_mask(uint32_t mask)
{
    
    __set_PRIMASK(mask);
}

#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_NVIC_MODULE_ENABLED */


