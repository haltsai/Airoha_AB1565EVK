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

#include "hal_wdt.h"
#include <stdio.h>

#ifdef HAL_WDT_MODULE_ENABLED

#include "hal_wdt_internal.h"
#include "hal_nvic.h"
#include "hal_gpt.h"
#include "memory_attribute.h"

#define     __HAL_GET_WDT_RETURN_ADDRESS(temp) {     \
    __asm volatile("MOV %0, lr" : "=r"(temp));   \
}


static hal_wdt_callback_t g_user_wdt_callback[2];
ATTR_ZIDATA_IN_TCM static hal_wdt_config_t s_hal_wdt_config_backup[2];
ATTR_ZIDATA_IN_TCM static hal_wdt_callback_t s_hal_wdt_callback_backup[2];
ATTR_ZIDATA_IN_TCM static bool s_hal_wdt_enable_status_backup[2];

static void internal_wdt_isr(void)
{
    uint32_t channel;
    hal_wdt_reset_status_t wdt_reset_status;

    for (channel = 0; channel < 2; channel++) {
        wdt_reset_status = wdt_get_reset_status(channel);
        if ((wdt_clear_irq(channel) == true) && (g_user_wdt_callback[channel] != NULL)) {
            g_user_wdt_callback[channel](wdt_reset_status);
        }
    }
}

hal_wdt_status_t hal_wdt_init(hal_wdt_config_t *wdt_config)
{
    if ((wdt_config == NULL) ||
        (wdt_config->mode > HAL_WDT_MODE_INTERRUPT) ||
        (wdt_config->seconds == 0) ||
        (wdt_config->seconds > HAL_WDT_MAX_TIMEOUT_VALUE)) {
        return HAL_WDT_STATUS_INVALID_PARAMETER;
    }
#ifndef __EXT_BOOTLOADER__
    printf("wdt set mod(%d)/tm(%ds)", (int) wdt_config->mode, (int) wdt_config->seconds);
#endif
    wdt_set_enable(0, false);
    wdt_set_length(0, wdt_config->seconds);
    wdt_set_mode(0, wdt_config->mode);

    if (wdt_config->mode == HAL_WDT_MODE_INTERRUPT) {
        hal_nvic_register_isr_handler(WDT_IRQn, (hal_nvic_isr_t)internal_wdt_isr);
        hal_nvic_enable_irq(WDT_IRQn);
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    /* WDT always did cold reset if DVFS feature is enabled */
    wdt_set_pmu_mask(false);
#endif

    return HAL_WDT_STATUS_OK;
}

hal_wdt_status_t hal_wdt_deinit(void)
{
    wdt_reset_default_value(0);

    g_user_wdt_callback[0] = NULL;

    return HAL_WDT_STATUS_OK;
}

hal_wdt_callback_t hal_wdt_register_callback(hal_wdt_callback_t wdt_callback)
{
    hal_wdt_callback_t curr_wdt_callback;

    if (wdt_callback == NULL) {
        return NULL;
    }

    curr_wdt_callback = g_user_wdt_callback[0];

    g_user_wdt_callback[0] = wdt_callback;

    return curr_wdt_callback;
}

hal_wdt_status_t hal_wdt_feed(uint32_t magic)
{
    if (magic != HAL_WDT_FEED_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    wdt_set_restart(0);

    return HAL_WDT_STATUS_OK;
}

hal_wdt_status_t hal_wdt_software_reset(void)
{
    uint32_t saved_mask;
    uint32_t counter = 0;
    uint32_t lr_reg;

    __HAL_GET_WDT_RETURN_ADDRESS(lr_reg);
#ifdef MTK_BOOTREASON_CHECK_ENABLE
{
    extern void bootreason_set_flag_wdt_sw_reset(void);
    bootreason_set_flag_wdt_sw_reset();
}
#endif
#ifndef __EXT_BOOTLOADER__
    printf("wdt sw rst, caller:0x%x", (unsigned int)lr_reg);
#endif
    hal_gpt_delay_ms(10);
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    wdt_set_mode(0, HAL_WDT_MODE_RESET);/* must config WDT to reset mode */
    wdt_set_pmu_mask(false);            /* always do cold reset */
    wdt_set_sw_rst(0);                  /* it take 2~3tick(32k) & independent of wdt enable bit */
    /* retry 3 times*/
    while(1){
        wdt_dump_register(0);
        hal_gpt_delay_ms(20);
        wdt_set_sw_rst(0);
        if(counter++ > 3){
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);
    return HAL_WDT_STATUS_INVALID_MAGIC;
}

hal_wdt_status_t hal_wdt_enable(uint32_t magic)
{
    if (magic != HAL_WDT_ENABLE_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    wdt_set_enable(0, true);
    wdt_set_restart(0);

    return HAL_WDT_STATUS_OK;
}

hal_wdt_status_t hal_wdt_disable(uint32_t magic)
{
    if (magic != HAL_WDT_DISABLE_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    wdt_set_enable(0, false);

    return HAL_WDT_STATUS_OK;
}

hal_wdt_reset_status_t hal_wdt_get_reset_status(void)
{
    hal_wdt_reset_status_t reset_status;

    reset_status = wdt_get_reset_status(0);

    return reset_status;
}

bool hal_wdt_get_enable_status(void)
{
    bool enable_status;

    enable_status = wdt_get_enable_status(0);

    return enable_status;
}

hal_wdt_mode_t hal_wdt_get_mode(void)
{
    hal_wdt_mode_t wdt_mode;

    wdt_mode = wdt_get_mode_status(0);

    return wdt_mode;
}




void hal_wdt_enter_sleep(uint32_t seconds)
{
    /* if wdt is enabled, backup wdt configuration */
    if (hal_wdt_get_enable_status() == true)
    {
        /* backup wdt configuration*/
        s_hal_wdt_enable_status_backup[0] = true;
        s_hal_wdt_callback_backup[0] = g_user_wdt_callback[0];
        s_hal_wdt_config_backup[0].mode = hal_wdt_get_mode();
        s_hal_wdt_config_backup[0].seconds = ((WDT_REGISTER->WDT_LENGTH) >> 16);
        s_hal_wdt_config_backup[0].seconds = s_hal_wdt_config_backup[0].seconds * WDT_1_TICK_LENGTH / 10000 + 1;
        wdt_set_length(0, seconds);
        wdt_set_mode(  0, HAL_WDT_MODE_RESET);

    } else {
        hal_wdt_config_t wdt_config;

        wdt_config.mode = HAL_WDT_MODE_RESET;
        wdt_config.seconds = seconds;
        hal_wdt_init(&wdt_config);
        hal_wdt_enable(HAL_WDT_ENABLE_MAGIC);
    }
}



void hal_wdt_exit_sleep(void)
{
    if (s_hal_wdt_enable_status_backup[0] == true)
    {
        wdt_set_length(0, s_hal_wdt_config_backup[0].seconds);
        wdt_set_mode(  0, s_hal_wdt_config_backup[0].mode);
    } else {
        wdt_set_enable(0, false);
        wdt_set_length(0, 1000);
    }
}





hal_wdt_status_t hal_wdt_init_ext(hal_wdt_config_t *wdt_config)
{
    if ((wdt_config == NULL) ||
        (wdt_config->mode > HAL_WDT_MODE_INTERRUPT) ||
        (wdt_config->seconds == 0) ||
        (wdt_config->seconds > HAL_WDT_MAX_TIMEOUT_VALUE)) {
        return HAL_WDT_STATUS_INVALID_PARAMETER;
    }
    wdt_set_enable(1, false);
    wdt_set_length(1, wdt_config->seconds);
    wdt_set_mode(1, wdt_config->mode);

    if (wdt_config->mode == HAL_WDT_MODE_INTERRUPT) {
        hal_nvic_register_isr_handler(WDT_IRQn, (hal_nvic_isr_t)internal_wdt_isr);
        hal_nvic_enable_irq(WDT_IRQn);
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    /* WDT always did cold reset if DVFS feature is enabled */
    wdt_set_pmu_mask(false);
#endif

    return HAL_WDT_STATUS_OK;
}

hal_wdt_status_t hal_wdt_deinit_ext(void)
{
    wdt_reset_default_value(1);

    g_user_wdt_callback[1] = NULL;

    return HAL_WDT_STATUS_OK;
}

hal_wdt_callback_t hal_wdt_register_callback_ext(hal_wdt_callback_t wdt_callback)
{
    hal_wdt_callback_t curr_wdt_callback;

    if (wdt_callback == NULL) {
        return NULL;
    }

    curr_wdt_callback = g_user_wdt_callback[1];

    g_user_wdt_callback[1] = wdt_callback;

    return curr_wdt_callback;
}

hal_wdt_status_t hal_wdt_feed_ext(uint32_t magic)
{
    if (magic != HAL_WDT_FEED_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    wdt_set_restart(1);

    return HAL_WDT_STATUS_OK;
}

hal_wdt_status_t hal_wdt_software_reset_ext(void)
{
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

#ifdef MTK_BOOTREASON_CHECK_ENABLE
{
    extern void bootreason_set_flag_wdt_sw_reset(void);
    bootreason_set_flag_wdt_sw_reset();
}
#endif

    wdt_set_enable(1, false);

    wdt_set_length(1, 1);

    wdt_set_mode(1, HAL_WDT_MODE_RESET);

#ifdef HAL_DVFS_MODULE_ENABLED
    /* WDT always did cold reset if DVFS feature is enabled */
    wdt_set_pmu_mask(false);
#endif

    wdt_set_enable(1, true);

    wdt_set_restart(1);

    wdt_set_sw_rst(1);

    while(1);

    hal_nvic_restore_interrupt_mask(saved_mask);

    return HAL_WDT_STATUS_OK;
}

hal_wdt_status_t hal_wdt_enable_ext(uint32_t magic)
{
    if (magic != HAL_WDT_ENABLE_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    wdt_set_enable(1, true);
    wdt_set_restart(1);

    return HAL_WDT_STATUS_OK;
}

hal_wdt_status_t hal_wdt_disable_ext(uint32_t magic)
{
    if (magic != HAL_WDT_DISABLE_MAGIC) {
        return HAL_WDT_STATUS_INVALID_MAGIC;
    }

    wdt_set_enable(1, false);

    return HAL_WDT_STATUS_OK;
}

hal_wdt_reset_status_t hal_wdt_get_reset_status_ext(void)
{
    hal_wdt_reset_status_t reset_status;

    reset_status = wdt_get_reset_status(1);

    return reset_status;
}

bool hal_wdt_get_enable_status_ext(void)
{
    bool enable_status;

    enable_status = wdt_get_enable_status(1);

    return enable_status;
}

hal_wdt_mode_t hal_wdt_get_mode_ext(void)
{
    hal_wdt_mode_t wdt_mode;

    wdt_mode = wdt_get_mode_status(1);

    return wdt_mode;
}

void hal_wdt_enter_sleep_ext(uint32_t seconds)
{
    hal_wdt_config_t wdt_config;

    /* if wdt is enabled, backup wdt configuration */
    if (hal_wdt_get_enable_status_ext() == true)
    {
        /* backup wdt configuration*/
        s_hal_wdt_enable_status_backup[1] = true;
        s_hal_wdt_callback_backup[1] = g_user_wdt_callback[1];
        s_hal_wdt_config_backup[1].mode = hal_wdt_get_mode_ext();
        s_hal_wdt_config_backup[1].seconds = ((WDT_REGISTER->WDT1_LENGTH) >> 16);
        s_hal_wdt_config_backup[1].seconds = s_hal_wdt_config_backup[1].seconds * WDT_1_TICK_LENGTH / 10000 + 1;

        /* disable wdt */
        hal_wdt_disable_ext(HAL_WDT_DISABLE_MAGIC);

        /* deinit wdt */
        hal_wdt_deinit_ext();
    }

    /* change wdt into reset mode */
    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = seconds;
    hal_wdt_init_ext(&wdt_config);

    /* enable wdt */
    hal_wdt_enable_ext(HAL_WDT_ENABLE_MAGIC);
}

void hal_wdt_exit_sleep_ext(void)
{
    /* disable wdt */
    hal_wdt_disable_ext(HAL_WDT_DISABLE_MAGIC);

    /* deinit wdt */
    hal_wdt_deinit_ext();

    /* if wdt is configuread before sleep, restore wdt configuration */
    if (s_hal_wdt_enable_status_backup[1] == true)
    {
        /* restore user's wdt status */
        hal_wdt_init_ext(&s_hal_wdt_config_backup[1]);
        if (s_hal_wdt_config_backup[1].mode == HAL_WDT_MODE_INTERRUPT)
        {
            hal_wdt_register_callback_ext(s_hal_wdt_callback_backup[1]);
        }
        hal_wdt_enable_ext(HAL_WDT_ENABLE_MAGIC);
        s_hal_wdt_enable_status_backup[1] = false;
        /* do not clean them for debugging */
        // s_hal_wdt_callback_backup[1] = NULL;
        // s_hal_wdt_config_backup[1].mode = 0;
        // s_hal_wdt_config_backup[1].seconds = 0;
    }
}


#endif

