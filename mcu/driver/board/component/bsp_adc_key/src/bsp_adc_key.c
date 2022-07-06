/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include <stdlib.h>
#include <assert.h>
#include "bsp_adc_key.h"
#include "bsp_adc_key_custom.h"

log_create_module(adc_key, PRINT_LEVEL_INFO);

#define BSP_ADC_KEY_INVALID_DATA            0xFFFFFFFF

#define BSP_ADCKEY_DEFINE_WEAK_VAR(num, gpio_num, adc_mode)\
    const char __attribute__((weak)) HAL_AUXADC_##num##_PIN = gpio_num;\
    const char __attribute__((weak)) HAL_AUXADC_##num##_PIN_M_AUXADC##num = adc_mode;\

BSP_ADCKEY_DEFINE_WEAK_VAR(0, 0xFF, 0xFF);
BSP_ADCKEY_DEFINE_WEAK_VAR(1, 0xFF, 0xFF);
BSP_ADCKEY_DEFINE_WEAK_VAR(2, 0xFF, 0xFF);
BSP_ADCKEY_DEFINE_WEAK_VAR(3, 0xFF, 0xFF);
BSP_ADCKEY_DEFINE_WEAK_VAR(4, 0xFF, 0xFF);
BSP_ADCKEY_DEFINE_WEAK_VAR(5, 0xFF, 0xFF);
BSP_ADCKEY_DEFINE_WEAK_VAR(6, 0xFF, 0xFF);
BSP_ADCKEY_DEFINE_WEAK_VAR(7, 0xFF, 0xFF);

typedef struct {
    uint8_t base;
    uint8_t interval;
    uint8_t level_max;
    bool is_checking_reverse;
} bsp_adc_data_config_t;

/** @brief This structure defines key timing configurations. */
typedef struct {
    hal_gpio_pin_t          pin;
    uint8_t                 pinmux;
    bsp_adc_key_channel_t   key_channel;
    hal_adc_channel_t       adc_channel;
    uint32_t                adc_data;
    uint32_t                adc_level;
    uint32_t                timer_handle;
    void                    *timer_callback;//check timeout callback
    void                    *timer_callback_user_data;
    uint32_t                check_interval;//unit:ms
    bool                    relative_value;
    bsp_adc_data_config_t  *adc_data_config;
} bsp_adc_key_settings_t;

typedef struct {
    bool                    has_initiliazed;
    uint8_t                 adc_key_num;
    bsp_adc_key_callback_t  adc_key_callback;//application layer callback
    void                   *user_data;
    bsp_adc_key_settings_t  adc_key_settings[BSP_ADC_KEY_CHANNEL_MAX];
} bsp_adc_key_context_t;


static bsp_adc_data_config_t adc_data_config[BSP_ADC_KEY_CHANNEL_MAX];
static bsp_adc_key_context_t bsp_adc_key_context;

#if 0
#ifdef MTK_CFG_TGT_HW_HORN_Q610
static const uint32_t main_vol_data_tab[ADC_DATA_MAIN_VOL_LEVEL_NUM] = {212, 424, 636, 848, 1060, 1272, 1484, 1696, 1908, 2120, 2332, 2544, 2756, 2968, 3180, 3392, 3604};//0~3820
// base=212 interval=212 total=17
static const uint32_t music_voice_banlance_data_tab[ADC_DATA_MUSIC_VOICE_BALANCE_LEVEL_NUM] = {211, 428, 645, 862, 1079, 1296, 1513, 1730, 1947, 2164, 2381, 2598, 2815, 3032, 3249, 3466, 3683};//0~3900
// base= 211 interval=217 total=17
#else
static const uint32_t main_vol_data_tab[ADC_DATA_MAIN_VOL_LEVEL_NUM] = {3834, 3608, 3382, 3156, 2930, 2704, 2478, 2252, 2026, 1800, 1574, 1348, 1122, 896, 670, 444, 218};//0~4060
// base=218  interval=226
static const uint32_t music_voice_banlance_data_tab[ADC_DATA_MUSIC_VOICE_BALANCE_LEVEL_NUM] = {3863, 3636, 3409, 3182, 2955, 2728, 2501, 2274, 2047, 1820, 1593, 1366, 1139, 912, 685, 458, 231};//0~4090
// base = 231 interval=227
#endif
#endif

static uint32_t bsp_adc_key_search_current_level(bsp_adc_data_config_t *data_config, uint32_t adc_data, uint32_t *target_data)
{
    uint32_t tab_idx = 0;
    uint32_t level;
#if 0
    for (tab_idx = 0; tab_idx < (data_config->level_max - 1); tab_idx++) {
#ifdef MTK_CFG_TGT_HW_HORN_Q610
        if (adc_data <= music_voice_banlance_data_tab[tab_idx])
#else
        if (adc_data > music_voice_banlance_data_tab[tab_idx])
#endif
            break;
    }
    *target_data = music_voice_banlance_data_tab[tab_idx];
#else
    uint32_t level_max = data_config->level_max;
    uint32_t base = data_config->base;
    uint32_t interval = data_config->interval;
    uint32_t is_checking_reverse = data_config->is_checking_reverse;

    if (is_checking_reverse) {
        for (tab_idx = level_max - 1; tab_idx > 0; tab_idx--) {
            if (adc_data > base + interval * tab_idx)
                break;
        }
        level = level_max - 1 - tab_idx;
    } else {
        for (tab_idx = 0; tab_idx < level_max - 1; tab_idx++) {
            if (adc_data <= base + interval * tab_idx)
                break;
        }
        level = tab_idx;
    }
    *target_data = base + interval * tab_idx;
#endif
    return level;
}


static void bsp_adc_key_timeout_handle(void *usr_data)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    uint32_t adc_data = BSP_ADC_KEY_INVALID_DATA;
    uint32_t target_data = BSP_ADC_KEY_INVALID_DATA;
    uint32_t delta_data;
    uint32_t current_level = 0;
    bsp_adc_key_settings_t *adc_key_settings = (bsp_adc_key_settings_t *)usr_data;
    bsp_adc_data_config_t  *data_config = adc_key_settings->adc_data_config;
    bsp_adc_key_event_t event;
    if (HAL_ADC_STATUS_OK != hal_adc_get_data_polling(adc_key_settings->adc_channel, &adc_data)) {
        LOG_MSGID_E(adc_key, "[adc_key] get adc data failed\r\n", 0);
        return;
    }
    if (adc_data != adc_key_settings->adc_data) {
        adc_key_settings->adc_data = adc_data;
        current_level = bsp_adc_key_search_current_level(data_config, adc_data, &target_data);
        if (current_level != adc_key_settings->adc_level) {
            if (adc_key_settings->adc_level == BSP_ADC_KEY_INVALID_DATA \
                    || ((adc_data > target_data) ? (adc_data - target_data > ADC_DATA_DEBOUNCE_LIMITATION) : (target_data - adc_data > ADC_DATA_DEBOUNCE_LIMITATION))) {
                LOG_MSGID_I(adc_key, "[adc_key] current_level=%d app_level=%d target_data=%d\r\n", 3, current_level, adc_key_settings->adc_level, target_data);
                LOG_MSGID_I(adc_key, "[adc_key] Pin%d get adc data=%d setting_adc_data=%d level_max=%d\r\n", 4, adc_key_settings->pin, adc_data, adc_key_settings->adc_data, data_config->level_max);

                if (adc_key_settings->relative_value) {
                    if (adc_key_settings->adc_level > current_level) {
                        LOG_MSGID_I(adc_key, "[adc_key] KEY_VAL_DN", 0);
                        if (adc_key_settings->adc_level == 0xFFFFFFFF) {
                            event = BSP_ADC_KEY_VAL_INIT;
                            delta_data = current_level;
                        } else {
                            event = BSP_ADC_KEY_VAL_DOWN;
                            delta_data = adc_key_settings->adc_level - current_level;
                        }
                    } else {
                        LOG_MSGID_I(adc_key, "[adc_key] KEY_VAL_UP", 0);
                        if (adc_key_settings->adc_level == 0xFFFFFFFF) {
                            event = BSP_ADC_KEY_VAL_INIT;
                            delta_data = current_level;
                        } else {
                            delta_data = current_level - adc_key_settings->adc_level;
                            event = BSP_ADC_KEY_VAL_UP;
                        }
                    }
                    adc_key_settings->adc_level = current_level;
                    ctx->adc_key_callback(adc_key_settings->key_channel, event, delta_data, ctx->user_data);
                } else {
                    ctx->adc_key_callback(adc_key_settings->key_channel, BSP_ADC_KEY_VAL_ABS, current_level, ctx->user_data);
                }
                adc_key_settings->adc_level = current_level;
            }
        }
    }
    // LOG_MSGID_I(adc_key, "[adc_key] timer_handle=%x check_interval=%d\r\n", 2, adc_key_settings->timer_handle, adc_key_settings->check_interval);
    if (HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_ms(adc_key_settings->timer_handle, adc_key_settings->check_interval, \
            adc_key_settings->timer_callback, usr_data)) {
        LOG_MSGID_E(adc_key, "[adc_key] start timer failed\r\n", 0);
    }
}

static bsp_adc_key_settings_t *bsp_adc_key_query_key_settings(bsp_adc_key_channel_t channel)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    bsp_adc_key_settings_t *adc_key_settings = NULL;

    if (channel < ctx->adc_key_num) {
        adc_key_settings =  &ctx->adc_key_settings[channel];
    }

    return adc_key_settings;
}

bsp_adc_key_status_t bsp_adc_key_enable(bsp_adc_key_channel_t channel)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    bsp_adc_key_settings_t *key_settings;

    if (!ctx->has_initiliazed) {
        LOG_MSGID_W(adc_key, "[adc_key] Not initiliazed\r\n", 0);
        return BSP_ADC_KEY_STATUS_NOT_INIT;
    }
    key_settings = bsp_adc_key_query_key_settings(channel);
    if (key_settings == NULL) {
        LOG_MSGID_E(adc_key, "[adc_key] channel %d is not configured\r\n", 1, channel);
        return BSP_ADC_KEY_STATUS_INVALID_CHANNEL;
    }
    LOG_MSGID_I(adc_key, "[adc_key] timer_handle=%x check_interval=%d\r\n", 2, key_settings->timer_handle, key_settings->check_interval);

    if (HAL_GPT_STATUS_OK != hal_gpt_sw_start_timer_ms(key_settings->timer_handle, key_settings->check_interval, \
            key_settings->timer_callback, key_settings)) {
        LOG_MSGID_E(adc_key, "[adc_key] bsp_adc_key_enable channel:%d start timer failed\r\n", 1, channel);
        return BSP_ADC_KEY_STATUS_ERROR;
    }
    return BSP_ADC_KEY_STATUS_OK;
}

bsp_adc_key_status_t bsp_adc_key_disable(bsp_adc_key_channel_t channel)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    bsp_adc_key_settings_t *key_settings;

    if (!ctx->has_initiliazed) {
        LOG_MSGID_E(adc_key, "[adc_key] Not initiliazed\r\n", 0);
        return BSP_ADC_KEY_STATUS_NOT_INIT;
    }
    key_settings = bsp_adc_key_query_key_settings(channel);
    if (key_settings == NULL) {
        LOG_MSGID_E(adc_key, "[adc_key] channel %d is not configured\r\n", 1, channel);
        return BSP_ADC_KEY_STATUS_INVALID_CHANNEL;
    }
    if (HAL_GPT_STATUS_OK != hal_gpt_sw_stop_timer_ms(key_settings->timer_handle)) {
        return BSP_ADC_KEY_STATUS_ERROR;
    }
    return BSP_ADC_KEY_STATUS_OK;
}

bsp_adc_key_status_t bsp_adc_key_get_data(bsp_adc_key_channel_t channel, uint32_t *data)
{
    hal_adc_status_t ret = HAL_ADC_STATUS_ERROR;
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    bsp_adc_key_settings_t *key_settings;

    if (!ctx->has_initiliazed) {
        LOG_MSGID_E(adc_key, "[adc_key] Not initiliazed\r\n", 0);
        return BSP_ADC_KEY_STATUS_NOT_INIT;
    }
    key_settings = bsp_adc_key_query_key_settings(channel);
    if (key_settings == NULL) {
        LOG_MSGID_E(adc_key, "[adc_key] channel %d is not configured\r\n", 1, channel);
        return BSP_ADC_KEY_STATUS_INVALID_CHANNEL;
    }

    ret = hal_adc_get_average_data(key_settings->adc_channel, HAL_ADC_AVERAGE_8, data);
    if (HAL_ADC_STATUS_OK != ret) {
        LOG_MSGID_E(adc_key, "[adc_key] bsp_adc_key_get_data failed\r\n", 0);
        return BSP_ADC_KEY_STATUS_ERROR;
    }

    return BSP_ADC_KEY_STATUS_OK;
}

void bsp_adc_key_driver_init(bsp_adc_key_channel_t key_channel, uint32_t adc_channel, uint32_t pin, uint32_t pinmux)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    bsp_adc_key_settings_t *adc_key_settings;

    LOG_MSGID_I(adc_key, "[adc_key] driver_init key_channel=%d adc_channel=%d pin=%d pinmux=%d\r\n", 4, key_channel, adc_channel, pin, pinmux);

    ctx->adc_key_num = key_channel + 1;
    if (ctx->adc_key_num > BSP_ADC_KEY_CHANNEL_MAX) {
        LOG_MSGID_E(adc_key, "[adc_key] driver_init key channel %d > %d\r\n", 2, ctx->adc_key_num, BSP_ADC_KEY_CHANNEL_MAX);
        return;
    }
    adc_key_settings = &ctx->adc_key_settings[key_channel];
    adc_key_settings->key_channel = key_channel;
    adc_key_settings->adc_channel = adc_channel;
    adc_key_settings->pin = pin;
    adc_key_settings->pinmux = pinmux;
}

bsp_adc_key_status_t bsp_adc_key_init(void)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    bsp_adc_key_settings_t *adc_key_settings;
    uint32_t i;

    if (ctx->has_initiliazed == true) {
        LOG_MSGID_E(adc_key, "[adc_key] Already initiliazed\r\n", 0);
        return BSP_ADC_KEY_STATUS_ERROR;
    }

    bsp_adc_key_custom_init();

    if (ctx->adc_key_num > 0) {
        for (i = 0; i < ctx->adc_key_num; i++) {
            adc_key_settings = &ctx->adc_key_settings[i];
            adc_key_settings->adc_data_config = &adc_data_config[i];
#ifndef BSP_ADC_KEY_USED_EPT_CONFIGURATION
            hal_gpio_init(adc_key_settings->pin);
            hal_pinmux_set_function(adc_key_settings->pin, adc_key_settings->pinmux);
#endif
            if (HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&adc_key_settings->timer_handle)) {
                LOG_MSGID_W(adc_key, "[adc_key] bsp_adc_key_init key_channel: %d get timer handle failed\r\n", 1, adc_key_settings->key_channel);
                return BSP_ADC_KEY_STATUS_ERROR;
            }
            /* Avoid the initial lev or data is 0 */
            adc_key_settings->adc_data = 0xFFFFFFFF;
            adc_key_settings->adc_level = 0xFFFFFFFF;
            adc_key_settings->timer_callback = bsp_adc_key_timeout_handle;
        }
        hal_adc_init();
        ctx->has_initiliazed = true;
    } else {
        LOG_MSGID_E(adc_key, "[adc_key] No ADC key is configured\r\n", 0);
        return BSP_ADC_KEY_STATUS_ERROR;
    }

    return BSP_ADC_KEY_STATUS_OK;
}

bsp_adc_key_status_t bsp_adc_key_set_config(bsp_adc_key_channel_t channel, bsp_adc_key_config_t *config)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    bsp_adc_key_settings_t *key_settings;

    if (!ctx->has_initiliazed) {
        LOG_MSGID_E(adc_key, "[adc_key] Not initiliazed\r\n", 0);
        return BSP_ADC_KEY_STATUS_NOT_INIT;
    }

    key_settings = bsp_adc_key_query_key_settings(channel);
    if (key_settings == NULL) {
        LOG_MSGID_E(adc_key, "[adc_key] channel %d is not configured\r\n", 1, channel);
        return BSP_ADC_KEY_STATUS_INVALID_CHANNEL;
    }
    key_settings->adc_data_config->base = config->adc_step;
    key_settings->adc_data_config->interval = config->adc_step;
    key_settings->adc_data_config->level_max = config->level_max;
    key_settings->adc_data_config->is_checking_reverse = config->checking_reverse;
    key_settings->check_interval = config->adc_check_interval;
    key_settings->relative_value = config->relative_value;
    return BSP_ADC_KEY_STATUS_OK;
}

bsp_adc_key_status_t bsp_adc_key_register_callback(bsp_adc_key_callback_t callback, void *usr_data)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;

    if (!callback) {
        return BSP_ADC_KEY_STATUS_ERROR;
    }
    ctx->adc_key_callback = callback;
    ctx->user_data = usr_data;

    return BSP_ADC_KEY_STATUS_OK;
}

bsp_adc_key_status_t bsp_adc_key_deinit(void)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;

    for (uint8_t i = 0; i < ctx->adc_key_num; i++) {
        hal_gpt_sw_stop_timer_ms(ctx->adc_key_settings[i].timer_handle);
        hal_gpt_sw_free_timer(ctx->adc_key_settings[i].timer_handle);
    }

    ctx->has_initiliazed = false;

    if (HAL_ADC_STATUS_OK != hal_adc_deinit()) {
        return BSP_ADC_KEY_STATUS_ERROR;
    }
    return BSP_ADC_KEY_STATUS_OK;
}

bsp_adc_key_status_t bsp_adc_key_get_level(bsp_adc_key_channel_t channel, uint32_t *level)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;
    bsp_adc_key_settings_t *key_settings;

    if (!ctx->has_initiliazed) {
        LOG_MSGID_E(adc_key, "[adc_key] Not initiliazed\r\n", 0);
        return BSP_ADC_KEY_STATUS_NOT_INIT;
    }
    key_settings = bsp_adc_key_query_key_settings(channel);
    if (key_settings == NULL) {
        LOG_MSGID_E(adc_key, "[adc_key] channel %d is not configured\r\n", 1, channel);
        return BSP_ADC_KEY_STATUS_INVALID_CHANNEL;
    } else {
        *level = key_settings->adc_level;
    }
    return BSP_ADC_KEY_STATUS_OK;
}

bsp_adc_key_status_t bsp_adc_key_get_channel(uint32_t *channel_max)
{
    bsp_adc_key_context_t *ctx = &bsp_adc_key_context;

    if (!ctx->has_initiliazed) {
        LOG_MSGID_E(adc_key, "[adc_key] Not initiliazed\r\n", 0);
        return BSP_ADC_KEY_STATUS_NOT_INIT;
    }

    if (!channel_max) {
        return BSP_ADC_KEY_STATUS_ERROR;
    }
    *channel_max = ctx->adc_key_num;
    return BSP_ADC_KEY_STATUS_OK;
}
