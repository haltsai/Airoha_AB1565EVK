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

#include <string.h>
#include <assert.h>
#include "hal_gpt.h"
#include "mux.h"
#include "mux_port_device.h"
#include "memory_attribute.h"

#ifndef MTK_SINGLE_CPU_ENV
#include "hal_hw_semaphore.h"
#include "hal_resource_assignment.h"
#endif

extern port_mux_device_ops_t g_port_mux_uart_ops;
extern port_mux_device_ops_t g_port_mux_airapp_ops;
#if defined(MTK_USB_DEMO_ENABLED) && defined(HAL_USB_MODULE_ENABLED) && defined(MTK_USB_ENABLED)
extern port_mux_device_ops_t g_port_mux_usb_ops;
#endif
#if defined(MTK_MUX_AWS_MCE_ENABLE)
extern port_mux_device_ops_t g_port_mux_aws_mce_ops;
#endif
#if defined(MTK_MUX_BT_ENABLE)
extern port_mux_device_ops_t g_port_mux_bt_ops;
#endif
#if defined(MTK_IAP2_VIA_MUX_ENABLE)
extern port_mux_device_ops_t g_mux_iap2_ops;
#endif
#if defined(HAL_I2C_SLAVE_MODULE_ENABLED) && defined(AG3335)
extern port_mux_device_ops_t g_port_mux_i2c_slave_ops;
#endif
#if defined(HAL_SPI_SLAVE_MODULE_ENABLED) && defined(AG3335)
extern port_mux_device_ops_t g_port_mux_spi_slave_ops;
#endif
#if defined(AIR_RACE_SCRIPT_ENABLE)
extern port_mux_device_ops_t g_port_mux_pseudo_ops;
#endif

/* memory malloc and free */
#ifdef MTK_CPU_NUMBER_0
void *port_mux_malloc(uint32_t size)
{
    return malloc(size);
}

void port_mux_free(void *pdata)
{
    free(pdata);
}
#endif

ATTR_TEXT_IN_TCM void port_mux_local_cpu_enter_critical(uint32_t *per_cpu_irq_mask)
{
    hal_nvic_save_and_set_interrupt_mask(per_cpu_irq_mask);
}

ATTR_TEXT_IN_TCM void port_mux_local_cpu_exit_critical(uint32_t per_cpu_irq_mask)
{
    hal_nvic_restore_interrupt_mask(per_cpu_irq_mask);
}

void port_mux_cross_cpu_enter_critical(void)
{
#ifndef MTK_SINGLE_CPU_ENV
    while (hal_hw_semaphore_take(HW_SEMAPHORE_MUX) != HAL_HW_SEMAPHORE_STATUS_OK);
#endif
}

void port_mux_cross_cpu_exit_critical(void)
{
#ifndef MTK_SINGLE_CPU_ENV
    while (hal_hw_semaphore_give(HW_SEMAPHORE_MUX) != HAL_HW_SEMAPHORE_STATUS_OK);
#endif
}

ATTR_TEXT_IN_TCM void port_mux_cross_local_enter_critical(uint32_t *per_cpu_irq_mask)
{
#ifndef MTK_SINGLE_CPU_ENV
    while (1) {
        hal_nvic_save_and_set_interrupt_mask(per_cpu_irq_mask);
        if (hal_hw_semaphore_take(HW_SEMAPHORE_MUX) == HAL_HW_SEMAPHORE_STATUS_OK) {
            break ;
        }
        hal_nvic_restore_interrupt_mask(*per_cpu_irq_mask);
    }
#else
    hal_nvic_save_and_set_interrupt_mask(per_cpu_irq_mask);
#endif
}

ATTR_TEXT_IN_TCM void port_mux_cross_local_exit_critical(uint32_t per_cpu_irq_mask)
{
#ifndef MTK_SINGLE_CPU_ENV
    if (hal_hw_semaphore_give(HW_SEMAPHORE_MUX) != HAL_HW_SEMAPHORE_STATUS_OK) {
        assert(0);
    }
    hal_nvic_restore_interrupt_mask(per_cpu_irq_mask);
#else
    hal_nvic_restore_interrupt_mask(per_cpu_irq_mask);
#endif
}


/* GPT timer */
uint32_t port_mux_get_current_timestamp(void)
{
    uint32_t count = 0;
    uint64_t count64 = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &count);
    count64 = ((uint64_t)count) * 1000 / 32768;
    return (unsigned int)count64;
}


#include "hal_feature_config.h"
port_mux_device_ops_t *g_mux_device_ops_array[] = {
#if defined(HAL_UART_MODULE_ENABLED)
    &g_port_mux_uart_ops,
#else
    NULL,
#endif
#if defined(MTK_USB_DEMO_ENABLED) && defined(HAL_USB_MODULE_ENABLED) && defined(MTK_USB_ENABLED)
    &g_port_mux_usb_ops,
#else
    NULL,
#endif
#if defined(HAL_I2C_SLAVE_MODULE_ENABLED) && defined(AG3335)
    &g_port_mux_i2c_slave_ops,
#else
    NULL,
#endif
#if defined(HAL_SPI_SLAVE_MODULE_ENABLED) && defined(AG3335)
    &g_port_mux_spi_slave_ops,
#else
    NULL,
#endif
    &g_port_mux_airapp_ops,
#if defined(MTK_MUX_BT_ENABLE)
    &g_port_mux_bt_ops,
#else
    NULL,
#endif
#if defined(MTK_MUX_AWS_MCE_ENABLE)
    &g_port_mux_aws_mce_ops,
#else
    NULL,
#endif
#if defined(MTK_IAP2_VIA_MUX_ENABLE)
    &g_mux_iap2_ops,
#else
    NULL,
#endif
#if defined(AIR_RACE_SCRIPT_ENABLE)
    &g_port_mux_pseudo_ops,
#else
    NULL,
#endif
};

