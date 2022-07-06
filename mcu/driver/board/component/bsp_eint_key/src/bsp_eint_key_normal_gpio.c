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

#include "bsp_eint_key_custom.h"
#include "bsp_eint_key_normal_gpio.h"
#include "syslog.h"

#ifdef EINT_KEY_HAL_MASK
#if PRODUCT_VERSION != 7697
extern void eint_ack_interrupt(uint32_t eint_number);
#endif
#endif /* EINT_KEY_HAL_MASK */

static void bsp_eint_key_normal_gpio_callback(void *user_data)
{
    bsp_eint_key_normal_gpio_driver_t *normal_gpio_driver = (bsp_eint_key_normal_gpio_driver_t*)user_data;
    bsp_eint_key_change_key_state(user_data);
#ifdef EINT_KEY_HAL_MASK
    hal_eint_unmask(normal_gpio_driver->eint_number);
#endif
}

static bool bsp_eint_key_normal_gpio_init(bsp_eint_key_gpio_driver_t *driver)
{
    hal_eint_config_t eint_config;
    hal_gpio_data_t gpio_data;
    bsp_eint_key_normal_gpio_driver_t *normal_gpio_driver = (bsp_eint_key_normal_gpio_driver_t*)driver;
#ifdef EINT_KEY_HAL_MASK
    hal_eint_mask(normal_gpio_driver->eint_number);
#endif
#ifndef EINT_KEY_USED_EPT_CONFIGURATION
    hal_gpio_init(normal_gpio_driver->gpio_port);
    hal_pinmux_set_function(normal_gpio_driver->gpio_port, normal_gpio_driver->eint_mode);
    hal_gpio_set_direction(normal_gpio_driver->gpio_port, HAL_GPIO_DIRECTION_INPUT);

#ifdef AIR_BSP_EINT_KEY_ACTIVE_LEVEL_CONFIGURABLE
    if (normal_gpio_driver->driver.active_level == HAL_GPIO_DATA_LOW) {
#else
    if (BSP_EINT_KEY_NORMAL_GPIO_ACTIVE_LEVEL == HAL_GPIO_DATA_LOW) {
#endif
        hal_gpio_pull_up(normal_gpio_driver->gpio_port);
    } else {
        hal_gpio_pull_down(normal_gpio_driver->gpio_port);
    }
#endif

    eint_config.debounce_time = DEFAULT_DEBOUNCE_TIME_MSEC;
    eint_config.trigger_mode  = HAL_EINT_EDGE_FALLING_AND_RISING;
    hal_eint_init(normal_gpio_driver->eint_number, &eint_config);

    hal_eint_register_callback(normal_gpio_driver->eint_number, (hal_eint_callback_t)bsp_eint_key_normal_gpio_callback, (void*)driver);
    if (HAL_GPIO_STATUS_OK == hal_gpio_get_input(normal_gpio_driver->gpio_port, &gpio_data)) {
        LOG_MSGID_I(eint_key, "[eint_key][key]GPIO=%d eint=%d keydata=%d status=%d", 4, normal_gpio_driver->gpio_port, \
        normal_gpio_driver->eint_number, normal_gpio_driver->driver.key_data, gpio_data);
    }

    return true;
}

static bool bsp_eint_key_normal_gpio_enable(bsp_eint_key_gpio_driver_t *driver)
{
    bsp_eint_key_normal_gpio_driver_t *normal_gpio_driver = (bsp_eint_key_normal_gpio_driver_t*)driver;
#ifdef EINT_KEY_HAL_MASK
#if PRODUCT_VERSION != 7697
    eint_ack_interrupt(normal_gpio_driver->eint_number);
#endif
    if (HAL_EINT_STATUS_OK != hal_eint_unmask(normal_gpio_driver->eint_number)) {
        LOG_MSGID_I(eint_key, "[eint_key]eint number %d error \r\n", 1, normal_gpio_driver->eint_number);
        return false;
    }
#endif
    return true;
}

static int bsp_eint_key_normal_gpio_set_debounce_time_msec(bsp_eint_key_gpio_driver_t *driver, uint32_t debounce_time_msec)
{
    bsp_eint_key_normal_gpio_driver_t *normal_gpio_driver = (bsp_eint_key_normal_gpio_driver_t*)driver;

    if (HAL_EINT_STATUS_OK != hal_eint_set_debounce_time(normal_gpio_driver->eint_number, debounce_time_msec)) {
        LOG_MSGID_I(eint_key, "[eint_key]eint number %d error \r\n", 1, normal_gpio_driver->eint_number);
        return -1;
    }

    return 0;
}

static int bsp_eint_key_normal_gpio_driver_is_active(bsp_eint_key_gpio_driver_t *driver, bool *is_active)
{
    bsp_eint_key_normal_gpio_driver_t *real_driver = (bsp_eint_key_normal_gpio_driver_t*)driver;
    hal_gpio_data_t gpio_data;
    if (HAL_GPIO_STATUS_OK != hal_gpio_get_input(real_driver->gpio_port, &gpio_data)) {
        return -1;
    }
#ifdef AIR_BSP_EINT_KEY_ACTIVE_LEVEL_CONFIGURABLE
    *is_active = gpio_data == driver->active_level;
#else
    *is_active = gpio_data == BSP_EINT_KEY_NORMAL_GPIO_ACTIVE_LEVEL;
#endif
    return 0;
}

void bsp_eint_key_normal_gpio_driver_new(
    bsp_eint_key_normal_gpio_driver_t *new_driver, const char gpio_port, const char eint_mode, unsigned char eint_number)
{
    new_driver->gpio_port = gpio_port;
    new_driver->eint_mode = eint_mode;
    new_driver->eint_number = eint_number;
    new_driver->driver.init = bsp_eint_key_normal_gpio_init;
    new_driver->driver.enable = bsp_eint_key_normal_gpio_enable;
    new_driver->driver.is_active = bsp_eint_key_normal_gpio_driver_is_active;
    new_driver->driver.set_debounce_time_msec = bsp_eint_key_normal_gpio_set_debounce_time_msec;
    bsp_eint_key_register_gpio_driver((bsp_eint_key_gpio_driver_t*)new_driver);
}
