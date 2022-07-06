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

#ifdef __cplusplus
extern "C" {
#endif


void nvic_clear_all_pending_interrupt(void)
{
    uint8_t i,j;
    uint32_t temp = 0;
    
    for (i = 0; i < (IRQ_NUMBER_MAX/32); i++) {
        NVIC->ICPR[i] = 0xffffffff;
    }

    for (j = 0; j < (IRQ_NUMBER_MAX%32); j++) {
        temp |= 1<<j;
    }

    NVIC->ICPR[i+1] = temp;
}

hal_nvic_status_t nvic_irq_software_trigger(hal_nvic_irq_t irq_number)
{
    if (irq_number < 0 || irq_number >= IRQ_NUMBER_MAX) {
        return HAL_NVIC_STATUS_INVALID_PARAMETER;
    }

    NVIC->STIR = ( irq_number << NVIC_STIR_INTID_Pos ) & NVIC_STIR_INTID_Msk;
    __DSB();
    
    return HAL_NVIC_STATUS_OK;
}

/**
 * @brief This function is used to return the CM4 status. 
 *        The return value if it is not zero indicates that the CM4 exception number.
 *        If you want to get the IRQ number, IRQn = exception number - 16.
 * @return    To indicate whether this function call is successful.
 *            If the return value is not zero, the CM4 is executing excetpion handler;
 *            If the return value is zero, the CM4 is executing normal code.
 */
int32_t hal_nvic_query_exception_number(void)
{   
     return ((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos);
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


