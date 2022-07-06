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

#include "hal_nvic.h"
#include "hal_gpt.h"
#ifdef HAL_NVIC_MODULE_ENABLED
#include "hal_nvic_internal.h"
//#include "hal_flash_disk_internal.h"
#include "memory_attribute.h"
#include "hal_log.h"
#include <xtensa/hal.h>
#include <xtensa/xtruntime.h>
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif /* MTK_SWLA_ENABLE */
#ifdef HAL_TIME_CHECK_ENABLED
#include "hal_time_check.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif


typedef struct {
    void (*nvic_callback)(hal_nvic_irq_t irq_number);
} nvic_function_t;


ATTR_ZIDATA_IN_DRAM nvic_function_t nvic_function_table[IRQ_NUMBER_MAX];

hal_nvic_status_t hal_nvic_init(void)
{
    static bool init = false;

    if (init == false) {
        xthal_set_intclear(0xffffffff);
        init = true;
    }

    nvic_irq_execution_number = -1;

    return HAL_NVIC_STATUS_OK;
}

hal_nvic_status_t hal_nvic_enable_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;
    uint32_t temp;

    if (irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        temp = xthal_get_intenable();
        temp |= 1 << irq_number;
        xthal_set_intenable(temp);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_disable_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;
    uint32_t temp;

    if (irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        temp = xthal_get_intenable();
        temp &= ~(1 << irq_number);
        xthal_set_intenable(temp);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

/*
uint32_t hal_nvic_get_pending_irq(hal_nvic_irq_t irq_number)
{
    uint32_t ret = 0xFF;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        return ret;
    } else {
        ret = NVIC_GetPendingIRQ(irq_number);
    }

    return ret;
}

hal_nvic_status_t hal_nvic_set_pending_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_SetPendingIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_clear_pending_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_ClearPendingIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_set_priority(hal_nvic_irq_t irq_number, uint32_t priority)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_SetPriority(irq_number, priority);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

uint32_t hal_nvic_get_priority(hal_nvic_irq_t irq_number)
{
    uint32_t ret = 0xff;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        return ret;
    } else {
        ret = NVIC_GetPriority(irq_number);
    }

    return ret;
}

*/

ATTR_TEXT_IN_IRAM void isrC_main()
{
    uint32_t irq_state;
    uint32_t i;
    uint32_t int_en;
    uint32_t shift;
    uint32_t int_level;
    uint32_t backup;
    const  uint32_t sw_irq[4][2] = {{I2S_SLAVE_IRQn, SW3_IRQn},
        {MCU_DMA_IRQn, BT_AUTX_IRQn},
        {ANC_IRQn, CM4_IRQn},
        {0, SW0_IRQn}
    };

    backup    = nvic_irq_execution_number;
    irq_state = xthal_get_interrupt();
    int_en    = xthal_get_intenable();
    int_level = (uint32_t)XTOS_SET_INTLEVEL(4); // to get intlevel
    XTOS_RESTORE_INTLEVEL(int_level);           // restore intlevel

    int_level = int_level & 0xf;

    for (i = sw_irq[int_level - 1][0]; i <= sw_irq[int_level - 1][1]; i++) {
        shift = 1 << i;
        nvic_irq_execution_number = i;
        if ((irq_state & shift) && (int_en & shift)) {
            if (nvic_function_table[i].nvic_callback != NULL) {
#ifdef MTK_SWLA_ENABLE
                SLA_RamLogging((uint32_t)(IRQ_START | i));
#endif /* MTK_SWLA_ENABLE */

                xthal_set_intclear(shift);
#ifdef HAL_TIME_CHECK_ENABLED
                time_check_start(i);
#endif
                nvic_function_table[i].nvic_callback((hal_nvic_irq_t)i);
#ifdef HAL_TIME_CHECK_ENABLED
                time_check_end(i, TIME_CHECK_ISR_TIME, nvic_function_table[i].nvic_callback);
#endif
#ifdef MTK_SWLA_ENABLE
                SLA_RamLogging((uint32_t)IRQ_END);
#endif /* MTK_SWLA_ENABLE */

            } else {
                log_hal_msgid_info("irq[%d] has no callback!!!\r\n", 1, (int)i);
            }
        }
    }
    nvic_irq_execution_number = backup;
}

hal_nvic_status_t hal_nvic_register_isr_handler(hal_nvic_irq_t irq_number, hal_nvic_isr_t callback)
{
    if (irq_number >= IRQ_NUMBER_MAX || callback == NULL) {
        return HAL_NVIC_STATUS_INVALID_PARAMETER;
    }

    xthal_set_intclear(1 << irq_number);
    nvic_function_table[irq_number].nvic_callback = callback;

    return HAL_NVIC_STATUS_OK;
}


hal_nvic_status_t hal_nvic_irq_software_trigger(hal_nvic_irq_t irq_number)
{
    if (irq_number >= IRQ_NUMBER_MAX) {
        return HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
    }

    nvic_irq_software_trigger(irq_number);

    return HAL_NVIC_STATUS_OK;
}


ATTR_TEXT_IN_IRAM hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask(uint32_t *mask)
{
    *mask = (uint32_t)XTOS_SET_INTLEVEL(4);
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    if ((*mask & 0xf) == 0) {
        hal_time_check_disable_irq_start((uint32_t)__builtin_return_address(0));
    }
#endif

    return HAL_NVIC_STATUS_OK;
}

ATTR_TEXT_IN_IRAM hal_nvic_status_t hal_nvic_restore_interrupt_mask(uint32_t mask)
{
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    if ((mask & 0xf) == 0) {
        if (false == hal_time_check_disable_irq_end(TIME_CHECK_DISABLE_IRQ_TIME, xLinkRegAddr)) {
            log_hal_msgid_info("#####xLinkRegAddr = 0x%x \r\n", 1, (int)xLinkRegAddr);
        }
    }
#endif

    XTOS_RESTORE_INTLEVEL(mask);

    return HAL_NVIC_STATUS_OK;
}

ATTR_TEXT_IN_IRAM hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask_special(uint32_t *mask)
{
    *mask = (uint32_t)XTOS_SET_INTLEVEL(4);
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    if ((*mask & 0xf) == 0) {
        hal_time_check_disable_irq_start((uint32_t)__builtin_return_address(0));
    }
#endif
    return HAL_NVIC_STATUS_OK;
}

ATTR_TEXT_IN_IRAM hal_nvic_status_t hal_nvic_restore_interrupt_mask_special(uint32_t mask)
{
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    uint32_t xLinkRegAddr = (uint32_t)__builtin_return_address(0);
    if ((mask & 0xf) == 0) {
        if (false == hal_time_check_disable_irq_end(0xffffffff, xLinkRegAddr)) {
            log_hal_msgid_info("#####xLinkRegAddr = 0x%x \r\n", 1, (int)xLinkRegAddr);
        }
    }
#endif

    XTOS_RESTORE_INTLEVEL(mask);

    return HAL_NVIC_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_NVIC_MODULE_ENABLED */

