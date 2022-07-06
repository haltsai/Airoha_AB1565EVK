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

#ifdef AIR_BSP_EINT_KEY_RTC_GPIO_ENABLE

#include "FreeRTOS.h"
#include "bsp_eint_key_custom.h"
#include "bsp_eint_key_rtc_gpio.h"
#include "syslog.h"

#define RTC_GPIO_LEVEL_TO_GPIO_DATA(rtc_gpio_level) \
    (rtc_gpio_level ? HAL_GPIO_DATA_HIGH : HAL_GPIO_DATA_LOW)

bool bsp_eint_key_rtc_gpio_enable_core(
    hal_rtc_gpio_t gpio_port, bool is_active_low, bool is_enable)
{
    hal_rtc_eint_config_t rtc_eint_config;
    LOG_MSGID_I(eint_key, "[eint_key][rtc_eint] port=%d is_active_low=%d is_enable=%d\r\n", 3, gpio_port, is_active_low, is_enable);
    rtc_eint_config.rtc_gpio = gpio_port;
    rtc_eint_config.is_enable_rtc_eint = is_enable;
    rtc_eint_config.is_enable_debounce = true;
    rtc_eint_config.is_falling_edge_active = is_active_low;
    return hal_rtc_eint_init(&rtc_eint_config) == HAL_RTC_STATUS_OK;
}

bool bsp_eint_key_rtc_gpio_is_level_changed(
    bsp_eint_key_rtc_gpio_driver_t *driver, bool* current_level)
{
    hal_rtc_gpio_get_input(driver->rtc_gpio_port, current_level);
#ifdef AIR_BSP_EINT_KEY_ACTIVE_LEVEL_CONFIGURABLE
    bool pressed = RTC_GPIO_LEVEL_TO_GPIO_DATA(*current_level) == driver->driver.active_level;
#else
    bool pressed = RTC_GPIO_LEVEL_TO_GPIO_DATA(*current_level) == BSP_EINT_KEY_RTC_GPIO_ACTIVE_LEVEL;
#endif
    if (pressed && driver->driver.key_state != BSP_EINT_KEY_RELEASE)
        return false;
    if (!pressed && driver->driver.key_state ==  BSP_EINT_KEY_RELEASE)
        return false;
    return true;
}

static uint32_t s_debounce_time_msec = DEFAULT_DEBOUNCE_TIME_MSEC;

void bsp_eint_key_rtc_gpio_debounce_timer_callback(TimerHandle_t xTimer)
{
    bsp_eint_key_rtc_gpio_driver_t *rtc_gpio_driver =
        (bsp_eint_key_rtc_gpio_driver_t*)pvTimerGetTimerID(xTimer);

    /*Check if the level is changed*/
    bool current_level = false;
    if (bsp_eint_key_rtc_gpio_is_level_changed(rtc_gpio_driver, &current_level)) {
        /*The level is changed, let's update key_state!*/
        bsp_eint_key_change_key_state((void*)rtc_gpio_driver);
    }

    /*Enable RTC_GPIO EINT*/
    bsp_eint_key_rtc_gpio_enable_core(rtc_gpio_driver->rtc_gpio_port, current_level, true);
    /*Fail safe: Check if the level is changed one more time*/
    hal_eint_mask(HAL_EINT_RTC);
    if (bsp_eint_key_rtc_gpio_is_level_changed(rtc_gpio_driver, &current_level)) {
        hal_eint_unmask(HAL_EINT_RTC);
        LOG_MSGID_I(eint_key, "[eint_key][rtc_eint][debounce timer callback] port=%d current_level=%d, level changed, keep debouncing\r\n", 2, rtc_gpio_driver->rtc_gpio_port, current_level);
        /*To keep debouncing, start debounce timer again!*/
        /*RTC_GPIO EINT will be ignored because rtc_gpio_debouncing is active*/
        BaseType_t xHigherPriorityTaskWoken;
        if ( xTimerStartFromISR( rtc_gpio_driver->debounce_timer,
            &xHigherPriorityTaskWoken ) != pdPASS ) {
            LOG_MSGID_I(eint_key, "[eint_key][rtc_eint][debounce timer callback] port=%d xTimerStartFromISR fail\r\n", 1, rtc_gpio_driver->rtc_gpio_port);
        }
    } else {
        LOG_MSGID_I(eint_key, "[eint_key][rtc_eint][debounce timer callback] port=%d End debouncing, current_level=%d\r\n", 2, rtc_gpio_driver->rtc_gpio_port, current_level);
        /*End debouncing*/
        rtc_gpio_driver->is_debouncing = false;
        hal_eint_unmask(HAL_EINT_RTC);
    }
}

static void bsp_eint_key_rtc_gpio_eint_callback(void *user_data)
{
    bsp_eint_key_rtc_gpio_driver_t *rtc_gpio_driver;
    rtc_gpio_driver = (bsp_eint_key_rtc_gpio_driver_t*)user_data;

    BaseType_t xHigherPriorityTaskWoken;
    LOG_MSGID_I(eint_key, "[eint_key][rtc_eint] callback, port = %d\r\n", 1, rtc_gpio_driver->rtc_gpio_port);
    if (rtc_gpio_driver->is_debouncing) {
        return;
    }
    /*Let's start debouncing*/
    rtc_gpio_driver->is_debouncing = true;
    /*Disable RTC_GPIO EINT*/
	if (!bsp_eint_key_rtc_gpio_enable_core(rtc_gpio_driver->rtc_gpio_port, false, false)) {
		LOG_MSGID_E(eint_key, "[eint_key][rtc_eint] init error\r\n", 0);
        assert(0);
	}

    /*Start debounce timer (also tries to update the period)*/
    if ( xTimerChangePeriodFromISR( rtc_gpio_driver->debounce_timer, s_debounce_time_msec, &xHigherPriorityTaskWoken ) != pdPASS ) {
        LOG_MSGID_E(eint_key, "[eint_key][rtc_eint] xTimerChangePeriodFromISR fail, port = %d\r\n",1,rtc_gpio_driver->rtc_gpio_port);
    }
    if ( xTimerStartFromISR( rtc_gpio_driver->debounce_timer, &xHigherPriorityTaskWoken ) != pdPASS ) {
        LOG_MSGID_E(eint_key, "[eint_key][rtc_eint] xTimerStartFromISR fail, port = %d\r\n",1,rtc_gpio_driver->rtc_gpio_port);
    }
    if (xHigherPriorityTaskWoken != pdFALSE) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(pdTRUE);
    }
}

static bool bsp_eint_key_rtc_gpio_init(bsp_eint_key_gpio_driver_t *driver)
{
    bsp_eint_key_rtc_gpio_driver_t *rtc_gpio_driver = (bsp_eint_key_rtc_gpio_driver_t*)driver;
    if (rtc_gpio_driver->rtc_gpio_port >= HAL_RTC_GPIO_MAX) {
        LOG_MSGID_E(eint_key, "[eint_key] rtc gpio %d is not exist, please check your configuration", 1, rtc_gpio_driver->rtc_gpio_port);
        return false;
    }
#ifndef EINT_KEY_USED_EPT_CONFIGURATION
    hal_rtc_gpio_config_t gpio_config;
    gpio_config.is_analog = false;
    gpio_config.is_input = true;
    gpio_config.rtc_gpio = rtc_gpio_driver->rtc_gpio_port;
#ifdef AIR_BSP_EINT_KEY_ACTIVE_LEVEL_CONFIGURABLE
    if (rtc_gpio_driver->driver.active_level == HAL_GPIO_DATA_LOW) {
#else
    if (BSP_EINT_KEY_RTC_GPIO_ACTIVE_LEVEL == HAL_GPIO_DATA_LOW) {
#endif
        gpio_config.is_pull_up = true;
        gpio_config.is_pull_down = false;
    } else {
        gpio_config.is_pull_up = false;
        gpio_config.is_pull_down = true;
    }
    if (HAL_RTC_STATUS_OK != hal_rtc_gpio_init(&gpio_config)) {
        LOG_MSGID_E(eint_key, "[eint_key] rtc gpio init fail, port = %d\r\n", 1, (int)rtc_gpio_driver->rtc_gpio_port);
        return false;
    }
#endif /* EINT_KEY_USED_EPT_CONFIGURATION */
    /*Prepare RTC_GPIO debounce timer*/
    rtc_gpio_driver->debounce_timer = xTimerCreate(NULL, \
        s_debounce_time_msec/portTICK_PERIOD_MS , \
        pdFALSE, (void*)driver, bsp_eint_key_rtc_gpio_debounce_timer_callback);
    if (rtc_gpio_driver->debounce_timer == NULL) {
        LOG_MSGID_E(eint_key, "[eint_key] create debounce timer for rtc gpio fail, port = %d", 1, (int)rtc_gpio_driver->rtc_gpio_port);
        return false;
    }
    rtc_gpio_driver->is_debouncing = false;
    /*Register RTC_GPIO EINT callback*/
    hal_rtc_eint_register_callback(rtc_gpio_driver->rtc_gpio_port, bsp_eint_key_rtc_gpio_eint_callback, (void*)driver);
    bool level;
    if (HAL_RTC_STATUS_OK == hal_rtc_gpio_get_input(rtc_gpio_driver->rtc_gpio_port, &level)) {
        LOG_MSGID_I(eint_key, "[eint_key][key]RTC_GPIO=%d keydata=%d status=%d", 3, rtc_gpio_driver->rtc_gpio_port, \
        rtc_gpio_driver->driver.key_data, level);
    }
    return true;
}

static bool bsp_eint_key_rtc_gpio_enable(bsp_eint_key_gpio_driver_t *driver)
{
    bsp_eint_key_rtc_gpio_driver_t *real_driver = (bsp_eint_key_rtc_gpio_driver_t*)driver;
    bool current_level;
    if (HAL_RTC_STATUS_OK != hal_rtc_gpio_get_input(real_driver->rtc_gpio_port, &current_level)) {
        return false;
    }
    return bsp_eint_key_rtc_gpio_enable_core(real_driver->rtc_gpio_port, current_level, true);
}

static int bsp_eint_key_rtc_gpio_set_debounce_time_msec(bsp_eint_key_gpio_driver_t *driver, uint32_t debounce_time_msec)
{
    s_debounce_time_msec = debounce_time_msec;
    return 0;
}

static int bsp_eint_key_rtc_gpio_driver_is_active(bsp_eint_key_gpio_driver_t *driver, bool *is_active)
{
    bsp_eint_key_rtc_gpio_driver_t *real_driver = (bsp_eint_key_rtc_gpio_driver_t*)driver;
    bool current_level;
    if (HAL_RTC_STATUS_OK != hal_rtc_gpio_get_input(real_driver->rtc_gpio_port, &current_level)) {
        return -1;
    }
#ifdef AIR_BSP_EINT_KEY_ACTIVE_LEVEL_CONFIGURABLE
    *is_active = current_level == (driver->active_level == HAL_GPIO_DATA_HIGH);
#else
    *is_active = current_level == (BSP_EINT_KEY_RTC_GPIO_ACTIVE_LEVEL == HAL_GPIO_DATA_HIGH);
#endif
    return 0;
}

void bsp_eint_key_rtc_gpio_driver_new(bsp_eint_key_rtc_gpio_driver_t *new_driver, const char gpio_port)
{
    new_driver->rtc_gpio_port = gpio_port;
    new_driver->driver.init = bsp_eint_key_rtc_gpio_init;
    new_driver->driver.enable = bsp_eint_key_rtc_gpio_enable;
    new_driver->driver.is_active = bsp_eint_key_rtc_gpio_driver_is_active;
    new_driver->driver.set_debounce_time_msec = bsp_eint_key_rtc_gpio_set_debounce_time_msec;
    bsp_eint_key_register_gpio_driver((bsp_eint_key_gpio_driver_t*)new_driver);
}

#endif
