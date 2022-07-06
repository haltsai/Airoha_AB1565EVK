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

#include "hal_eint.h"
#include "hal_log.h"
#include <string.h>
#ifdef FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "timers.h"
#endif
#include "hal_gpt.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include <stdlib.h>
#include <assert.h>
#include "bsp_eint_key_custom.h"
#include "bsp_eint_key.h"
#include "bsp_eint_key_common.h"


log_create_module(eint_key, PRINT_LEVEL_INFO);

#ifdef AIR_BSP_EINT_KEY_ACTIVE_LEVEL_CONFIGURABLE
#define BSP_EINTKEY_DEFINE_WEAK_ACTIVE_LEVEL(num, active_level)\
    const unsigned char __attribute__((weak)) BSP_EINTKEY##num##_ACTIVE_LEVEL = active_level;
#else
#define BSP_EINTKEY_DEFINE_WEAK_ACTIVE_LEVEL(num, active_level)
#endif

#define BSP_EINTKEY_DEFINE_WEAK_VAR(num, gpio_num, eint_num, eint_mode, active_level)\
    const char __attribute__((weak)) BSP_EINTKEY##num##_PIN = gpio_num;\
    const char __attribute__((weak)) BSP_EINTKEY##num##_PIN_M_EINT = eint_mode;\
    const unsigned char __attribute__((weak)) BSP_EINTKEY##num##_EINT = eint_num;\
    BSP_EINTKEY_DEFINE_WEAK_ACTIVE_LEVEL(num, active_level)

BSP_EINTKEY_DEFINE_WEAK_VAR(0, 0xFF, 0xFF, 0xFF, HAL_GPIO_DATA_LOW);
BSP_EINTKEY_DEFINE_WEAK_VAR(1, 0xFF, 0xFF, 0xFF, HAL_GPIO_DATA_LOW);
BSP_EINTKEY_DEFINE_WEAK_VAR(2, 0xFF, 0xFF, 0xFF, HAL_GPIO_DATA_LOW);
BSP_EINTKEY_DEFINE_WEAK_VAR(3, 0xFF, 0xFF, 0xFF, HAL_GPIO_DATA_LOW);
BSP_EINTKEY_DEFINE_WEAK_VAR(4, 0xFF, 0xFF, 0xFF, HAL_GPIO_DATA_LOW);

typedef struct {
#ifdef BSP_EINT_KEY_TIMING_ENABLE
    uint32_t                    longpress_time;
    uint32_t                    repeat_time;
#ifdef FREERTOS_ENABLE
    TimerHandle_t               rtos_timer[BSP_EINT_KEY_NUMBER];
#else
    uint32_t                    timer_handle[BSP_EINT_KEY_NUMBER];
#endif
#endif
    bool                        has_initiliazed;
    bsp_eint_key_callback_t     callback;
    void                       *user_data;
    bsp_eint_key_event_t       *event;
    bool                        is_init_ready;
    bsp_eint_key_gpio_driver_t *gpio_driver[BSP_EINT_KEY_NUMBER];
} bsp_eint_key_context_t;

bsp_eint_key_context_t eint_key_context;

static uint32_t bsp_eint_key_get_gpio_driver_index(bsp_eint_key_gpio_driver_t *driver)
{
    int i;
    for (i = 0; i < BSP_EINT_KEY_NUMBER; i++) {
        if (driver == eint_key_context.gpio_driver[i]) {
            return i;
        }
    }
    return -1;
}

int bsp_eint_key_register_gpio_driver(bsp_eint_key_gpio_driver_t *new_driver)
{
    static int bsp_eint_key_registered_driver_num = 0;

    if (bsp_eint_key_registered_driver_num < BSP_EINT_KEY_NUMBER) {
        eint_key_context.gpio_driver[bsp_eint_key_registered_driver_num] = new_driver;
        bsp_eint_key_registered_driver_num++;
        return bsp_eint_key_registered_driver_num;
    }
    return -1;
}

#ifdef FREERTOS_ENABLE
#ifdef BSP_EINT_KEY_TIMING_ENABLE
void bsp_eint_key_timer_callback(TimerHandle_t xTimer)
{
    bsp_eint_key_gpio_driver_t *gpio_driver;

    uint32_t   i;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    for (i = 0; i < BSP_EINT_KEY_NUMBER; i++) {
        if (xTimer == eint_key_context.rtos_timer[i]) {
            break;
        }
    }

    if (i >= BSP_EINT_KEY_NUMBER) {
        LOG_MSGID_I(eint_key, "[eint_key]no timer found, xTimer=%d\r\n",1,xTimer);
        return;
    }

    gpio_driver = eint_key_context.gpio_driver[i];

    if (gpio_driver->key_state == BSP_EINT_KEY_PRESS) {
        gpio_driver->key_state =  BSP_EINT_KEY_LONG_PRESS;
    } else {
        gpio_driver->key_state =  BSP_EINT_KEY_REPEAT;
    }

    if ( xTimerChangePeriodFromISR( eint_key_context.rtos_timer[i], eint_key_context.repeat_time/portTICK_PERIOD_MS , &xHigherPriorityTaskWoken ) != pdPASS ) {
        LOG_MSGID_I(eint_key, "[eint_key]repeat_time xTimerChangePeriodFromISR fail, index = %d\r\n",1,i);
    }

    eint_key_context.callback(gpio_driver->key_state, gpio_driver->key_data, &eint_key_context.user_data);
}
#endif

void bsp_eint_key_change_key_state(bsp_eint_key_gpio_driver_t *gpio_driver)
{
    uint32_t index;
    BaseType_t xHigherPriorityTaskWoken;

    index = bsp_eint_key_get_gpio_driver_index(gpio_driver);

    LOG_MSGID_I(eint_key, "[eint_key]enter callback, index=%d key_data=%d\r\n", 3,\
        index, gpio_driver->key_data);

     xHigherPriorityTaskWoken = pdFALSE;

    if (eint_key_context.is_init_ready != true) {
        gpio_driver->key_state = BSP_EINT_KEY_RELEASE;
#ifdef BSP_EINT_KEY_TIMING_ENABLE
        if ( xTimerStopFromISR( eint_key_context.rtos_timer[index], &xHigherPriorityTaskWoken ) != pdPASS ) {
            LOG_MSGID_I(eint_key, "[eint_key]xTimerStopFromISR fail, index = %d is_init_ready\r\n",1,index);
        }
#endif
        return;
    }

    if (gpio_driver->key_state == BSP_EINT_KEY_RELEASE) {
        gpio_driver->key_state = BSP_EINT_KEY_PRESS;
#ifdef BSP_EINT_KEY_TIMING_ENABLE
        if ( xTimerChangePeriodFromISR( eint_key_context.rtos_timer[index], eint_key_context.longpress_time/portTICK_PERIOD_MS, &xHigherPriorityTaskWoken ) != pdPASS ) {
            LOG_MSGID_I(eint_key, "[eint_key]longpress_time xTimerChangePeriodFromISR fail, index = %d\r\n",1,index);
        }

        if ( xTimerStartFromISR( eint_key_context.rtos_timer[index], &xHigherPriorityTaskWoken ) != pdPASS ) {
            LOG_MSGID_I(eint_key, "[eint_key]xTimerStartFromISR fail, index = %d\r\n",1,index);
        }
#endif
    } else {
        gpio_driver->key_state = BSP_EINT_KEY_RELEASE;
#ifdef BSP_EINT_KEY_TIMING_ENABLE
        if ( xTimerStopFromISR( eint_key_context.rtos_timer[index], &xHigherPriorityTaskWoken ) != pdPASS ) {
            LOG_MSGID_I(eint_key, "[eint_key]xTimerStopFromISR fail, index = %d\r\n",1,index);
        }
#endif
    }

    eint_key_context.callback(gpio_driver->key_state, gpio_driver->key_data, eint_key_context.user_data);

    if (xHigherPriorityTaskWoken != pdFALSE) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(pdTRUE);
    }
}

#else
#ifdef BSP_EINT_KEY_TIMING_ENABLE
void bsp_eint_key_timer_callback(void *user_data)
{
    uint32_t index;
    hal_gpt_status_t ret;
    bsp_eint_key_gpio_driver_t *gpio_driver = (bsp_eint_key_gpio_driver_t*)user_data;
    index = bsp_eint_key_get_gpio_driver_index(gpio_driver);

    if (gpio_driver->key_state == BSP_EINT_KEY_PRESS) {
        gpio_driver->key_state =  BSP_EINT_KEY_LONG_PRESS;
    } else {
        gpio_driver->key_state =  BSP_EINT_KEY_REPEAT;
    }

    ret = hal_gpt_sw_start_timer_ms(eint_key_context.timer_handle[index], \
                                    eint_key_context.repeat_time,\
                                    (hal_gpt_callback_t)bsp_eint_key_timer_callback,\
                                    (void*)gpio_driver);
    if (ret != HAL_GPT_STATUS_OK) {
        LOG_MSGID_I(eint_key, "[eint_key]longpress_time start repeat timer fail, index = %d, ret=%d\r\n",2,index,ret);
    }

    eint_key_context.callback(gpio_driver->key_state, gpio_driver->key_data, &eint_key_context.user_data);

}
#endif

void bsp_eint_key_change_key_state(bsp_eint_key_gpio_driver_t *gpio_driver)
{
#ifdef BSP_EINT_KEY_TIMING_ENABLE
    hal_gpt_status_t ret;
    uint32_t index = bsp_eint_key_get_gpio_driver_index(gpio_driver);
#endif

    LOG_MSGID_I(eint_key, "[eint_key]enter callback:%d\r\n", 1,
        bsp_eint_key_get_gpio_driver_index(gpio_driver));

    if (eint_key_context.is_init_ready != true) {
        gpio_driver->key_state = BSP_EINT_KEY_RELEASE;
        return;
    }

    if (gpio_driver->key_state == BSP_EINT_KEY_RELEASE) {
        gpio_driver->key_state = BSP_EINT_KEY_PRESS;

#ifdef BSP_EINT_KEY_TIMING_ENABLE
        ret = hal_gpt_sw_get_timer(&eint_key_context.timer_handle[index]);
        if (ret != HAL_GPT_STATUS_OK) {
            LOG_MSGID_I(eint_key, "[eint_key]longpress_time get timer fail, index = %d, ret=%d\r\n",2,index,ret);
        }

        ret = hal_gpt_sw_start_timer_ms(eint_key_context.timer_handle[index], \
                                        eint_key_context.longpress_time,\
                                        (hal_gpt_callback_t)bsp_eint_key_timer_callback,\
                                        (void*)gpio_driver);
        if (ret != HAL_GPT_STATUS_OK) {
            LOG_MSGID_I(eint_key, "[eint_key]longpress_time start timer fail, index = %d, ret=%d\r\n",2,index,ret);
        }
#endif
    } else {
        gpio_driver->key_state = BSP_EINT_KEY_RELEASE;
#ifdef BSP_EINT_KEY_TIMING_ENABLE
        ret = hal_gpt_sw_stop_timer_ms(eint_key_context.timer_handle[index]);
        if (ret != HAL_GPT_STATUS_OK) {
            LOG_MSGID_I(eint_key, "[eint_key]longpress_time stop timer fail, index = %d, ret=%d\r\n",2,index,ret);
        }

        ret = hal_gpt_sw_free_timer(eint_key_context.timer_handle[index]);
        if (ret != HAL_GPT_STATUS_OK) {
            LOG_MSGID_I(eint_key, "[eint_key]longpress_time free timer fail, index = %d, ret=%d\r\n",2,index,ret);
        }
#endif
    }

    eint_key_context.callback(gpio_driver->key_state, gpio_driver->key_data, eint_key_context.user_data);
}
#endif

bool bsp_eint_key_init(bsp_eint_key_config_t *config)
{
    uint8_t             i;
    uint8_t             count = 0;
    bool is_active;
    bsp_eint_key_gpio_driver_t *gpio_driver;

#ifdef BSP_EINT_KEY_TIMING_ENABLE
    if (config == NULL ) {
        LOG_MSGID_I(eint_key, "[eint_key]config pioter is null\r\n", 0);
        return false;
    }
#endif
    if (eint_key_context.has_initiliazed == true) {
        LOG_MSGID_I(eint_key, "[eint_key]bsp eint key has initiliazed\r\n", 0);
        return false;
    }

    memset(&eint_key_context,  0,  sizeof(bsp_eint_key_context_t));

    bsp_eint_key_custom_init();

#ifdef BSP_EINT_KEY_TIMING_ENABLE
    if (config->longpress_time == 0) {
        eint_key_context.longpress_time = 2000; /*default 2s*/
    } else {
        eint_key_context.longpress_time = config->longpress_time;
    }

    if (config->repeat_time == 0) {
        eint_key_context.repeat_time = 1000;/*default 1s */
    } else {
        eint_key_context.repeat_time = config->repeat_time;
    }
#endif
    for (i = 0; i < BSP_EINT_KEY_NUMBER; i++) {
        gpio_driver = eint_key_context.gpio_driver[i];
        if (!gpio_driver) {
            break;
        }
        if (!gpio_driver->init(gpio_driver)) {
            return false;
        }

        if (gpio_driver->is_active(gpio_driver, &is_active) < 0) {
            continue;
        } else {
            if (is_active)
                gpio_driver->key_state = BSP_EINT_KEY_PRESS;
            else
                gpio_driver->key_state = BSP_EINT_KEY_RELEASE;
        }

        count++;
#if defined(FREERTOS_ENABLE) && defined(BSP_EINT_KEY_TIMING_ENABLE)
        eint_key_context.rtos_timer[i] = xTimerCreate(NULL, \
                                                  eint_key_context.longpress_time/portTICK_PERIOD_MS , \
                                                  pdFALSE, NULL, \
                                                  bsp_eint_key_timer_callback);
#endif
    }

    #if 0
    LOG_MSGID_I(eint_key, "[eint_key]key valid num  = %d",1, count);
    LOG_MSGID_I(eint_key, "[eint_key]longpress time = %d ms",1, eint_key_context.longpress_time);
    LOG_MSGID_I(eint_key, "[eint_key]repeat    time = %d ms",1, eint_key_context.repeat_time);
    #endif
    eint_key_context.has_initiliazed = true;
    return true;
}

bool bsp_eint_key_register_callback(bsp_eint_key_callback_t callback, void *user_data)
{
    if (callback == NULL) {
        LOG_MSGID_I(eint_key, "[eint_key]callback pointer is null\r\n", 0);
        return false;
    }

    eint_key_context.callback  = callback;
    eint_key_context.user_data = user_data;

    return true;
}

bool bsp_eint_key_enable(void)
{
    uint32_t i;

    for (i = 0; i < BSP_EINT_KEY_NUMBER; i++){
        if (!eint_key_context.gpio_driver[i]) {
            break;
        }
        if (eint_key_context.gpio_driver[i]->enable &&
            !eint_key_context.gpio_driver[i]->enable(eint_key_context.gpio_driver[i])) {
            return false;
        }
    }

    eint_key_context.is_init_ready = true;

    return true;

}

bool bsp_eint_key_set_event_time(bsp_eint_key_config_t *config)
{
    if (config == NULL) {
        return false;
    }
#ifdef BSP_EINT_KEY_TIMING_ENABLE
    eint_key_context.longpress_time = config->longpress_time;
    eint_key_context.repeat_time    = config->repeat_time;
#endif
    return true;
}

bool bsp_eint_key_set_debounce_time(uint32_t debounce_time)
{
    uint32_t i;
    bsp_eint_key_gpio_driver_t *gpio_driver;

    for (i = 0; i < BSP_EINT_KEY_NUMBER; i++) {
        if (!eint_key_context.gpio_driver[i]) {
            continue;
        }
        gpio_driver = eint_key_context.gpio_driver[i];
        if (gpio_driver->set_debounce_time_msec(gpio_driver, debounce_time) < 0) {
            return false;
        }
    }
    return true;
}

static bool bsp_eint_key_get_key_pressed(uint8_t *indexes, uint8_t *nums)
{
    bsp_eint_key_gpio_driver_t *gpio_driver;
    uint8_t i;
    uint8_t key_pressed_nums = 0;
    bool is_active;

    if ((NULL == indexes) || (NULL == nums)) {
        return false;
    }

    for (i = 0; i < BSP_EINT_KEY_NUMBER; i++) {
        if (eint_key_context.gpio_driver[i]) {
            gpio_driver = eint_key_context.gpio_driver[i];
            if (gpio_driver->is_active(gpio_driver, &is_active) < 0) {
                continue;
            }
            if (is_active) {
                indexes[key_pressed_nums++] = i;
            }
        }
    }

    if (key_pressed_nums == 0) {
        return false;
    }

    *nums = key_pressed_nums;

    return true;
}

void bsp_eint_key_pressed_key_event_simulation(void)
{
    uint8_t indexes[BSP_EINT_KEY_NUMBER];
    uint8_t keynums = 0;
    uint8_t i;
    bsp_eint_key_gpio_driver_t *gpio_driver;

    if (bsp_eint_key_get_key_pressed(indexes, &keynums)) {
        for (i = 0; i < keynums; i++) {
            LOG_MSGID_I(eint_key, "[eint_key] key%d press simulation\r\n", 1, indexes[i]);
            gpio_driver = eint_key_context.gpio_driver[indexes[i]];
            eint_key_context.callback(gpio_driver->key_state, gpio_driver->key_data, eint_key_context.user_data);
        }
    }
}

int bsp_eint_key_get_status(uint8_t eint_key_data, bsp_eint_key_event_t *status)
{
    uint8_t i;
    bool is_active = false;
    bsp_eint_key_gpio_driver_t *gpio_driver;

    if (!status)
        return -1;

    for (i = 0; i < BSP_EINT_KEY_NUMBER; i++) {
        gpio_driver = eint_key_context.gpio_driver[i];
        if (!gpio_driver)
            continue;
        if (eint_key_data == gpio_driver->key_data) {
            gpio_driver->is_active(gpio_driver, &is_active);
            if (is_active) {
                *status = BSP_EINT_KEY_PRESS;
            } else {
                *status = BSP_EINT_KEY_RELEASE;
            }
            LOG_MSGID_I(eint_key, "[eint_key]key_data = 0x%x, key status=%d \r\n", 2, eint_key_data, *status);
            return 0;
        }
    }
    LOG_MSGID_W(eint_key, "[eint_key]The key is not found, key_data = 0x%x\r\n", 1, eint_key_data);
    return -1;
}
