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

#ifdef HAL_NVIC_MODULE_ENABLED
#include "hal_nvic_internal.h"
//#include "hal_flash_disk_internal.h"
#include "memory_attribute.h"
#include "hal_log.h"
#include "hal_gpt.h"
#ifdef MTK_SWLA_ENABLE
#include "swla.h"
#endif /* MTK_SWLA_ENABLE */
#ifdef MTK_SYSTEM_HANG_TRACER_ENABLE
#include "systemhang_tracer.h"
#endif /* MTK_SYSTEM_HANG_TRACER_ENABLE */
#ifdef MTK_MEMORY_MONITOR_ENABLE
#include "memory_monitor.h"
#endif /* MTK_MEMORY_MONITOR_ENABLE */
#ifdef HAL_TIME_CHECK_ENABLED
#include "hal_time_check.h"
#endif
#ifndef __UBL__
#include "assert.h"
#endif

#ifdef HAL_ESC_MODULE_ENABLED
#include "hal_esc_internal.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*nvic_callback)(hal_nvic_irq_t irq_number);
} nvic_function_t;

#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
ATTR_ZIDATA_IN_TCM uint32_t backup_mask_irq_time = 0;
ATTR_ZIDATA_IN_TCM uint32_t mask_irq_func = 0;
#endif

static const uint32_t defualt_irq_priority[IRQ_NUMBER_MAX] = {
    OS_GPT_IRQ_PRIORITY,
    MCU_DMA_IRQ_PRIORITY,
    SLOW_DMA0_IRQ_PRIORITY,
    SLOW_DMA1_IRQ_PRIORITY,
    SPI_MST0_IRQ_PRIORITY,
    SPI_MST1_IRQ_PRIORITY,
    SPI_MST2_IRQ_PRIORITY,
    SPI_SLV_IRQ_PRIORITY,
    SDIO_MST0_IRQ_PRIORITY,
    UART0_IRQ_PRIORITY,
    UART1_IRQ_PRIORITY,
    UART2_IRQ_PRIORITY,
    CRYPTO_IRQ_PRIORITY,
    TRNG_IRQ_PRIORITY,
    I2S_SLAVE_IRQ_PRIORITY,
    I2C0_IRQ_PRIORITY,
    I2C1_IRQ_PRIORITY,
    I2C2_IRQ_PRIORITY,
    I2C_AO_IRQ_PRIORITY,
    RTC_IRQ_PRIORITY,
    GPT_IRQ_PRIORITY,
    SPM_IRQ_PRIORITY,
    WDT_IRQ_PRIORITY,
    EINT_IRQ_PRIORITY,
    SFC_IRQ_PRIORITY,
    ESC_IRQ_PRIORITY,
    USB_IRQ_PRIORITY,
    DPS0_IRQ_PRIORITY,
    CAP_TOUCH_IRQ_PRIORITY,
    AUDIOSYS0_IRQ_PRIORITY,
    AUDIOSYS1_IRQ_PRIORITY,
    AUDIOSYS2_IRQ_PRIORITY,
    ANC_IRQ_PRIORITY,
    VAD_IRQ_PRIORITY,
    BT_IRQ_PRIORITY,
    BT_AURX_IRQ_PRIORITY,
    BT_AUTX_IRQ_PRIORITY,
    BT_TIMER_IRQ_PRIORITY,
    VOW_SNR_IRQ_PRIORITY,
    VOW_FIFO_IRQ_PRIORITY,
    DSP_ERR_IRQ_PRIORITY,
    CM4_reserved0_IRQ_PRIORITY,
};

nvic_function_t nvic_function_table[IRQ_NUMBER_MAX];

typedef struct {
    uint32_t callback_addr;
    uint32_t mask_irq_time;
    uint32_t cur_time;
} hal_nvic_debug_t;

#define HAL_NVIC_DEBUG_NUM (20)
ATTR_ZIDATA_IN_TCM hal_nvic_debug_t nvic_debug[HAL_NVIC_DEBUG_NUM];
ATTR_ZIDATA_IN_TCM uint32_t cur_index = 0;

ATTR_TEXT_IN_TCM void hal_nvic_debug_log_save(uint32_t callback_addr, uint32_t time)
{
    nvic_debug[cur_index].callback_addr = callback_addr;
    nvic_debug[cur_index].mask_irq_time = time;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &(nvic_debug[cur_index].cur_time));
    cur_index ++;
    cur_index = (cur_index < HAL_NVIC_DEBUG_NUM) ? cur_index : 0;
}

hal_nvic_status_t hal_nvic_init(void)
{
    static uint32_t priority_set = 0;
    uint32_t i;

    if (priority_set == 0) {
        /* Set defualt priority only one time */
        for (i = 0; i < IRQ_NUMBER_MAX; i++) {
            NVIC_SetPriority((hal_nvic_irq_t)i, defualt_irq_priority[i]);
        }
        priority_set = 1;
    }
    return HAL_NVIC_STATUS_OK;
}

hal_nvic_status_t hal_nvic_enable_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_EnableIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

hal_nvic_status_t hal_nvic_disable_irq(hal_nvic_irq_t irq_number)
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;
        return status;
    } else {
        NVIC_DisableIRQ(irq_number);
        status = HAL_NVIC_STATUS_OK;
    }

    return status;
}

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

ATTR_TEXT_IN_TCM uint32_t get_current_irq()
{
    return (((SCB->ICSR & SCB_ICSR_VECTACTIVE_Msk) >> SCB_ICSR_VECTACTIVE_Pos) - 16);
}


extern void Flash_ReturnReady(void);
#if defined(HAL_ESC_MODULE_ENABLED) && defined(ESC_FLASH_ENABLE)
extern void esc_flash_return_ready(void);
#endif
ATTR_TEXT_IN_TCM hal_nvic_status_t isrC_main()
{
    hal_nvic_status_t status = HAL_NVIC_STATUS_ERROR;
    hal_nvic_irq_t irq_number;

    Flash_ReturnReady();

#if defined(HAL_ESC_MODULE_ENABLED) && defined(ESC_FLASH_ENABLE)
    esc_flash_return_ready();
#endif

    irq_number = (hal_nvic_irq_t)(get_current_irq());

#ifdef MTK_SWLA_ENABLE_O2
    uart_debug_print_byte(UART_LA_PORT,0x14+irq_number);
#endif 

#if defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2)
    systemhang_tracer_t systemhang_irq_tracer;

    systemhang_interrupt_enter_trace((uint32_t)irq_number, &systemhang_irq_tracer);
#endif /* defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2) */

#ifdef MTK_MEMORY_MONITOR_ENABLE
    memorymonitor_interrupt_enter_trace(irq_number);
#endif /* MTK_MEMORY_MONITOR_ENABLE */

#ifdef MTK_SWLA_ENABLE
    if (irq_number != 0) {
        SLA_RamLogging((uint32_t)(IRQ_START | irq_number));
    }
#endif /* MTK_SWLA_ENABLE */

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX) {
        status = HAL_NVIC_STATUS_ERROR_IRQ_NUMBER;

        #ifdef MTK_SWLA_ENABLE_O2
           uart_debug_print_byte(UART_LA_PORT,'*');
        #endif 

        return status;
    } else if (nvic_function_table[irq_number].nvic_callback == NULL) {
        status = HAL_NVIC_STATUS_ERROR_NO_ISR;
#ifdef READY
        log_hal_msgid_error("ERROR: no IRQ handler! \n", 0);
#endif

        #ifdef MTK_SWLA_ENABLE_O2
            uart_debug_print_byte(UART_LA_PORT,'*');
        #endif

        return status;
    } else {
#ifdef HAL_TIME_CHECK_ISR_ENABLED
        if (irq_number != EINT_IRQn) {
            time_check_start(irq_number);
        }
#endif
        nvic_function_table[irq_number].nvic_callback(irq_number);
        status = HAL_NVIC_STATUS_OK;
#ifdef HAL_TIME_CHECK_ISR_ENABLED
        if (irq_number != EINT_IRQn) {
            time_check_end(irq_number, TIME_CHECK_ISR_TIME, (void *)nvic_function_table[irq_number].nvic_callback);
        }
#endif
    }

#ifdef MTK_SWLA_ENABLE
    if (irq_number != 0) {
        SLA_RamLogging((uint32_t)IRQ_END);
    }
#endif /* MTK_SWLA_ENABLE */

#ifdef MTK_MEMORY_MONITOR_ENABLE
    memorymonitor_interrupt_exit_trace(irq_number);
#endif /* MTK_MEMORY_MONITOR_ENABLE */

#if defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2)
    systemhang_interrupt_exit_trace((uint32_t)irq_number, &systemhang_irq_tracer);
#endif /* defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O1) || defined(MTK_SYSTEM_HANG_TRACER_ENABLE_O2) */

#ifndef __UBL__
    if (__get_BASEPRI() != 0) {
        log_hal_msgid_error("irq_number = %d \n", 1, irq_number);
        assert(0);
    }
#endif

#ifdef MTK_SWLA_ENABLE_O2
    uart_debug_print_byte(UART_LA_PORT,'*');
#endif 

    return status;
}

hal_nvic_status_t hal_nvic_register_isr_handler(hal_nvic_irq_t irq_number, hal_nvic_isr_t callback)
{
    uint32_t mask;

    if (irq_number < (hal_nvic_irq_t)0 || irq_number >= IRQ_NUMBER_MAX || callback == NULL) {
        return HAL_NVIC_STATUS_INVALID_PARAMETER;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);
    NVIC_ClearPendingIRQ(irq_number);
    nvic_function_table[irq_number].nvic_callback = callback;
    hal_nvic_restore_interrupt_mask(mask);

    return HAL_NVIC_STATUS_OK;
}

ATTR_TEXT_IN_TCM hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask(uint32_t *mask)
{
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    uint32_t xLinkRegAddr = 0;
#endif
    uint32_t priority = 1;

    *mask = __get_BASEPRI();
    //set base priority 1, fault can be handled first as priority is default zero.
    __set_BASEPRI(((priority << (8 - __NVIC_PRIO_BITS)) & 0xFF));
    __DMB();
    __ISB();

#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    xLinkRegAddr = (uint32_t)__builtin_return_address(0);

    if ((*mask == 0) && (hal_core_status_read(HAL_CORE_CM4) == HAL_CORE_ACTIVE)) {
        mask_irq_func = xLinkRegAddr;
        hal_time_check_disable_irq_start();
    }
#endif
    return HAL_NVIC_STATUS_OK;
}

ATTR_TEXT_IN_TCM hal_nvic_status_t hal_nvic_restore_interrupt_mask(uint32_t mask)
{
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    if ((mask == 0) && (((1 << (8 - __NVIC_PRIO_BITS)) & 0xFF) == __get_BASEPRI())) {
        if (hal_core_status_read(HAL_CORE_CM4) == HAL_CORE_ACTIVE) {
            hal_time_check_disable_irq_end(TIME_CHECK_DISABLE_IRQ_TIME);
        }
        mask_irq_func = 0x0;
    } else if((mask != 0) && (__get_BASEPRI() == 0)) {
        assert(0);
    }
#endif
    __set_BASEPRI(mask);
    __DMB();
    __ISB();

    return HAL_NVIC_STATUS_OK;
}

ATTR_TEXT_IN_TCM hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask_special(uint32_t *mask)
{
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    uint32_t xLinkRegAddr = 0;
#endif
    uint32_t priority = 1;

    *mask = __get_BASEPRI();
    //set base priority 1, fault can be handled first as priority is default zero.
    __set_BASEPRI(((priority << (8 - __NVIC_PRIO_BITS)) & 0xFF));
    __DMB();
    __ISB();

#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    xLinkRegAddr = (uint32_t)__builtin_return_address(0);

    if ((*mask == 0) && (hal_core_status_read(HAL_CORE_CM4) == HAL_CORE_ACTIVE)) {
        mask_irq_func = xLinkRegAddr;
        hal_time_check_disable_irq_start();
    }
#endif
    return HAL_NVIC_STATUS_OK;
}

ATTR_TEXT_IN_TCM hal_nvic_status_t hal_nvic_restore_interrupt_mask_special(uint32_t mask)
{
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    uint32_t backup_lr = 0;
    uint32_t backup_time = 0;

    if ((mask == 0) && (((1 << (8 - __NVIC_PRIO_BITS)) & 0xFF) == __get_BASEPRI())) {
        if (hal_core_status_read(HAL_CORE_CM4) == HAL_CORE_ACTIVE) {
            backup_lr = mask_irq_func;
            hal_time_check_disable_irq_end(TIME_CHECK_DISABLE_IRQ_TIME_SPECIAL);
        }
        backup_time = backup_mask_irq_time;
        mask_irq_func = 0x0;
    } else if((mask != 0) && (__get_BASEPRI() == 0)) {
        assert(0);
    }
#endif
    __set_BASEPRI(mask);
    __DMB();
    __ISB();
#ifdef HAL_TIME_CHECK_DISABLE_IRQ_ENABLED
    if (backup_time > (TIME_CHECK_DISABLE_IRQ_TIME << 1)) {
        hal_nvic_debug_log_save(backup_lr, backup_time);
    }
#endif

    return HAL_NVIC_STATUS_OK;
}

#ifdef __cplusplus
}
#endif

#endif /* HAL_NVIC_MODULE_ENABLED */

