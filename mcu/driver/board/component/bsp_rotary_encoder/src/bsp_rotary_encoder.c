/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "hal.h"
#include "hal_gpio.h"
#include "hal_eint.h"
#include "hal_gpt.h"
#include "bsp_rotary_encoder.h"
#include "bsp_rotary_encoder_custom.h"

// #define BSP_ROTARY_ENCODER_SINGLE_EDGE_TRIGGER

#define UPDATE_PIN_STATE(driver)\
    ((driver->pinb_config.pin_state << 1) | driver->pina_config.pin_state)

#define REABS(val) ((val>0?val:-(val)))

static void update_pin_state(bsp_rotary_encoder_pin_config_t *pin_config)
{
#ifdef BSP_ROTARY_ENCODER_SINGLE_EDGE_TRIGGER
    pin_config->pin_state = ((pin_config->trigger_mode == HAL_EINT_EDGE_RISING) ? 1 : 0);
#else
    hal_gpio_get_input(pin_config->gpio_port, &pin_config->pin_state);
#endif
}

#define BSP_ROTARY_ENCODER_DEFINE_WEAK_VAR(num, gpio_num_a, gpio_num_b, eint_num_a, eint_num_b, eint_mode_a, eint_mode_b)\
    const char __attribute__((weak)) BSP_ROTARY_ENCODER##num##_PIN_A = gpio_num_a;\
    const char __attribute__((weak)) BSP_ROTARY_ENCODER##num##_PIN_B = gpio_num_b;\
    const char __attribute__((weak)) BSP_ROTARY_ENCODER##num##_PIN_A_M_EINT = eint_mode_a;\
    const char __attribute__((weak)) BSP_ROTARY_ENCODER##num##_PIN_B_M_EINT = eint_mode_b;\
    const unsigned char __attribute__((weak)) BSP_ROTARY_ENCODER##num##_PIN_A_EINT = eint_num_a;\
    const unsigned char __attribute__((weak)) BSP_ROTARY_ENCODER##num##_PIN_B_EINT = eint_num_b

BSP_ROTARY_ENCODER_DEFINE_WEAK_VAR(0, HAL_GPIO_10, HAL_GPIO_11, HAL_EINT_NUMBER_10, HAL_EINT_NUMBER_11, 9, 9);
BSP_ROTARY_ENCODER_DEFINE_WEAK_VAR(1, HAL_GPIO_16, HAL_GPIO_17, HAL_EINT_NUMBER_16, HAL_EINT_NUMBER_17, 9, 9);
BSP_ROTARY_ENCODER_DEFINE_WEAK_VAR(2, HAL_GPIO_8, HAL_GPIO_9, HAL_EINT_NUMBER_8, HAL_EINT_NUMBER_9, 9, 9);

typedef enum
{
    BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_NONE = 0,
    BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_A = 1,
    BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_B = 2,
} bsp_rotary_encoder_last_trigger_pin_t;
//02310231 CW
//13201320 CCW
typedef struct {
    bool is_pina_trigered;
    bool is_pinb_trigered;
    uint32_t max_position;
    uint32_t min_position;
    uint32_t current_position;
    int32_t relative_position;
    uint32_t event_count;
    uint32_t timer_handle;
    uint8_t pc_state;      /**< Including previous and current status of the pinA and pinB. */
    uint8_t dpc_state;      /**< Including double previous and current status of the pinA and pinB. */
    uint8_t pina_debounce_counter;
    uint8_t pinb_debounce_counter;
    bsp_rotary_encoder_event_t direction;
    bsp_rotary_encoder_event_type_t event_type;
    bsp_rotary_encoder_last_trigger_pin_t last_trigger_pin;
    bsp_rotary_encoder_config_t user_config;
    bsp_rotary_encoder_driver_t *driver;
} bsp_rotary_encoder_t;

// static int8_t bsp_rotary_encoder_table[] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};

static bsp_rotary_encoder_t rotary_encoder[BSP_ROTARY_ENCODER_MAX];

static bsp_rotary_encoder_driver_t bsp_rotary_encoder_driver[BSP_ROTARY_ENCODER_MAX];


bsp_rotary_encoder_driver_t * bsp_rotary_encoder_driver_new(bsp_rotary_encoder_port_t port)
{
    bsp_rotary_encoder_driver_t *new_driver;

    if (port >= BSP_ROTARY_ENCODER_MAX) {
        return NULL;
    }
    new_driver = &bsp_rotary_encoder_driver[port];
    rotary_encoder[port].driver = new_driver;
    return new_driver;
}

int32_t bsp_rotary_encoder_get_position(bsp_rotary_encoder_t *re, uint8_t current_state)
{
    uint32_t position = re->current_position;

    re->pc_state <<= 2;
    re->pc_state |= (current_state & 0x3);
    re->pc_state &= 0xF;

    LOG_MSGID_I(common, "[bsp][rotary encoder] prev_curr_state=0x%x current_state=0x%x", 2, re->pc_state, current_state);
    switch(re->pc_state) {
        case 0xd: case 0x4: case 0x2: case 0xb: case 0xe: case 0x8: case 0x1: case 0x7:
            re->dpc_state <<= 4;
            re->dpc_state |= (re->pc_state & 0xF);
            re->dpc_state &= 0xFF;
            switch(re->dpc_state) {
                case 0xe8: case 0x17:
                    if (re->event_type == BSP_ROTARY_ENCODER_RELATIVE_CHANGE) {
                        re->relative_position++;
                    } else {
                        if (position < re->max_position) {
                            re->relative_position++;
                        }
                    }
                    position++;
                    LOG_MSGID_I(common, "[bsp][rotary encoder]  CW dpc_state=0x%x", 1, re->dpc_state);
                    break;
                case 0xd4: case 0x2b:
                    if (re->event_type == BSP_ROTARY_ENCODER_RELATIVE_CHANGE) {
                        re->relative_position--;
                    } else {
                        if (position > re->min_position) {
                            re->relative_position--;
                        }
                    }
                    position--;
                    LOG_MSGID_I(common, "[bsp][rotary encoder] CCW dpc_state=0x%x", 1, re->dpc_state);
                    break;
                default:
                    LOG_MSGID_W(common, "[bsp][rotary encoder] MID dpc_state=0x%x", 1, re->dpc_state);
                    return -1;
            }
            re->current_position = position;
            break;
        default:
            LOG_MSGID_E(common, "[bsp][rotary encoder] error pc_state=0x%x", 1, re->pc_state);
            return -2;
    }
    return 0;
}

static void bsp_rotary_encoder_process_event(bsp_rotary_encoder_t *re)
{
    uint32_t position;

    //reset event counter
    re->event_count = 0;

    if (re->relative_position > 0) {
        re->direction = BSP_ROTARY_ENCODER_EVENT_CW;
    } else if (re->relative_position < 0) {
        re->direction = BSP_ROTARY_ENCODER_EVENT_CCW;
    } else {
        LOG_MSGID_W(common, "[bsp][rotary encoder] no position change", 0);
        return;
    }
    if (re->event_type == BSP_ROTARY_ENCODER_RELATIVE_CHANGE) {
        position = REABS(re->relative_position);
    } else {
        position = re->current_position;
    }
    re->relative_position = 0;

    re->user_config.callback(re->driver->port, re->direction, position, re->user_config.user_data);
}

static void bsp_rotary_encoder_timeout_callback(void *user_data)
{
    LOG_MSGID_I(common, "[bsp][rotary encoder] event timeout", 0);

    bsp_rotary_encoder_process_event((bsp_rotary_encoder_t*)user_data);
}

static void bsp_rotary_encoder_eint_callback(bsp_rotary_encoder_t *re, uint8_t current_state, bsp_rotary_encoder_pin_config_t *pin_config)
{
    hal_gpt_status_t gpt_status;
    bsp_rotary_encoder_config_t *rec = &re->user_config;

#ifdef BSP_ROTARY_ENCODER_SINGLE_EDGE_TRIGGER
    if (pin_config->pin_state == HAL_GPIO_DATA_LOW) {
        pin_config->trigger_mode = HAL_EINT_EDGE_RISING;
        hal_eint_set_trigger_mode(pin_config->eint_number, HAL_EINT_EDGE_RISING);
    }else {
        pin_config->trigger_mode = HAL_EINT_EDGE_FALLING;
        hal_eint_set_trigger_mode(pin_config->eint_number, HAL_EINT_EDGE_FALLING);
    }
#endif

    LOG_MSGID_I(common, "[bsp][rotary encoder] EINT%d trigger, pin_state=%d ", 2,  (int)pin_config->eint_number, (int)pin_config->pin_state);

#ifdef BSP_ROTARY_ENCODER_TIMING_DEBUG_ENABLE
    bsp_rotary_encoder_port_t  port = re->driver->port;
    uint32_t current_timer_count[BSP_ROTARY_ENCODER_MAX];
    static uint32_t last_timer_count[BSP_ROTARY_ENCODER_MAX] = {0};

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timer_count[port]);
    if (last_timer_count[port] != 0) {
        LOG_MSGID_I(common, "[bsp][rotary encoder] event interval time=%dms", 1, (int)(current_timer_count[port] - last_timer_count[port])/1000);
    }
    last_timer_count[port] = current_timer_count[port];
#endif

    if (bsp_rotary_encoder_get_position(re, current_state) < 0) {
        hal_eint_unmask(pin_config->eint_number);
        return;
    }

    LOG_MSGID_I(common, "[bsp][rotary encoder] event_count=%d current_pos=%d", 2, (int)re->event_count, (int)re->current_position);
    if (rec->event_threshold > 0) {
        hal_gpt_sw_stop_timer_ms(re->timer_handle);
        re->event_count++;
        if (re->event_count > rec->event_threshold) {
            bsp_rotary_encoder_process_event(re);
        } else if (rec->event_timeout_ms > 0) {
            gpt_status = hal_gpt_sw_start_timer_ms(re->timer_handle, rec->event_timeout_ms, bsp_rotary_encoder_timeout_callback, (void*)re);
            if (gpt_status != HAL_GPT_STATUS_OK) {
                LOG_MSGID_E(common, "[bsp][rotary encoder] gpt start error:%d", 1, (int)gpt_status);
            }
        }
    } else {
        bsp_rotary_encoder_process_event(re);
    }

    hal_eint_unmask(pin_config->eint_number);
}

static void bsp_rotary_encoder_pina_eint_callback(void *user_data)
{
    bsp_rotary_encoder_driver_t *driver = (bsp_rotary_encoder_driver_t*)user_data;
    bsp_rotary_encoder_pin_config_t *pin_config = &driver->pina_config;
    bsp_rotary_encoder_t *re = &rotary_encoder[driver->port];

    update_pin_state(pin_config);

#ifdef BSP_ROTARY_ENCODER_TIMING_DEBUG_ENABLE
    uint32_t current_timer_count[BSP_ROTARY_ENCODER_MAX];
    static uint32_t last_timer_count[BSP_ROTARY_ENCODER_MAX] = {0};
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timer_count[driver->port]);
    if (last_timer_count[driver->port] != 0) {
        LOG_MSGID_I(common, "[bsp][rotary encoder] A EINT%d interval time=%dms", 2, pin_config->eint_number, (int)(current_timer_count[driver->port] - last_timer_count[driver->port])/1000);
    }
    last_timer_count[driver->port] = current_timer_count[driver->port];
#endif

    re->pinb_debounce_counter = 0;
    if ((re->last_trigger_pin != BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_NONE)
        && (re->last_trigger_pin == BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_A)) {
        re->pina_debounce_counter++;
        if (re->pina_debounce_counter >= 2) {
            LOG_MSGID_W(common, "[bsp][rotary encoder] A EINT%d debounce pin_state=%d", 2, \
                pin_config->eint_number, pin_config->pin_state);
            hal_eint_unmask(pin_config->eint_number);
            return;
        }
    }
    re->last_trigger_pin = BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_A;
    LOG_MSGID_I(common, "[bsp][rotary encoder] A EINT%d debounce counter=%d trigger_mode=%d", 3,\
        pin_config->eint_number, re->pina_debounce_counter, pin_config->trigger_mode);


    bsp_rotary_encoder_eint_callback(re, UPDATE_PIN_STATE(driver), pin_config);
}

static void bsp_rotary_encoder_pinb_eint_callback(void *user_data)
{
    bsp_rotary_encoder_driver_t *driver = (bsp_rotary_encoder_driver_t*)user_data;
    bsp_rotary_encoder_pin_config_t *pin_config = &driver->pinb_config;
    bsp_rotary_encoder_t *re = &rotary_encoder[driver->port];

    update_pin_state(pin_config);

#ifdef BSP_ROTARY_ENCODER_TIMING_DEBUG_ENABLE
    uint32_t current_timer_count[BSP_ROTARY_ENCODER_MAX];
    static uint32_t last_timer_count[BSP_ROTARY_ENCODER_MAX] = {0};
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timer_count[driver->port]);
    if (last_timer_count[driver->port] != 0) {
        LOG_MSGID_I(common, "[bsp][rotary encoder] B EINT%d interval time=%dms", 2, pin_config->eint_number, (int)(current_timer_count[driver->port] - last_timer_count[driver->port])/1000);
    }
    last_timer_count[driver->port] = current_timer_count[driver->port];
#endif

    re->pina_debounce_counter = 0;
    if ((re->last_trigger_pin != BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_NONE)
        && (re->last_trigger_pin == BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_B)) {
        re->pinb_debounce_counter++;
        if (re->pinb_debounce_counter >= 2) {
            LOG_MSGID_W(common, "[bsp][rotary encoder] B EINT%d debounce pin_state=%d", 2, \
                pin_config->eint_number, pin_config->pin_state);
            hal_eint_unmask(pin_config->eint_number);
            return;
        }
    }
    re->last_trigger_pin = BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_B;
    LOG_MSGID_I(common, "[bsp][rotary encoder] B EINT%d debounce counter=%d trigger_mode=%d", 3,\
        pin_config->eint_number, re->pina_debounce_counter, pin_config->trigger_mode);


    bsp_rotary_encoder_eint_callback(re, UPDATE_PIN_STATE(driver), pin_config);
}

bsp_rotary_encoder_status_t bsp_rotary_encoder_init(bsp_rotary_encoder_port_t port, bsp_rotary_encoder_config_t *config)
{
    uint32_t i = 0;
    hal_eint_config_t pina_eint_config;
    hal_eint_config_t pinb_eint_config;
    hal_gpio_pin_t    gpio_port_a;
    hal_gpio_pin_t    gpio_port_b;
    hal_eint_number_t eint_number_a;
    hal_eint_number_t eint_number_b;
    bsp_rotary_encoder_t *re;
    bsp_rotary_encoder_driver_t *driver;

    if (port >= BSP_ROTARY_ENCODER_MAX) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }

    /*param check*/
    if (!config->callback) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] GPIO init param error", 0);
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }

    re = &rotary_encoder[port];
    memset(re, 0, sizeof(bsp_rotary_encoder_t));

    bsp_rotary_encoder_custom_init();

    driver = re->driver;

    i = port;
    // for (i = 0; i < BSP_ROTARY_ENCODER_MAX; i++) {
    if (!driver) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] driver is not ready for port %d, please check EPT config or select the right port", 1, i);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }
    gpio_port_a = driver->pina_config.gpio_port;
    gpio_port_b = driver->pinb_config.gpio_port;
    eint_number_a = driver->pina_config.eint_number;
    eint_number_b = driver->pinb_config.eint_number;
    if ((gpio_port_a == 0xFF) || (gpio_port_b == 0xFF) || (eint_number_a == 0xFF) || (eint_number_b == 0xFF)) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] GPIO or EINT config error%d, please check EPT config or select the right port", 1, i);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }
    #ifndef ROTARY_ENCODER_USED_EPT_CONFIGURATION
    hal_gpio_init(gpio_port_a);
    hal_gpio_init(gpio_port_b);
    hal_pinmux_set_function(gpio_port_a, driver->pina_config.eint_mode);
    hal_pinmux_set_function(gpio_port_b, driver->pinb_config.eint_mode);
    hal_gpio_set_direction(gpio_port_a, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_set_direction(gpio_port_b, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_pull_up(gpio_port_a);
    hal_gpio_pull_up(gpio_port_b);
    #endif
    re->max_position = BSP_ROTARY_ENCODER_MAX_POSITION;
    re->current_position = BSP_ROTARY_ENCODER_DEFAULT_POSITION;
    re->event_type = BSP_ROTARY_ENCODER_RELATIVE_CHANGE;
    re->event_count = 0;
    re->last_trigger_pin = BSP_ROTART_ENCODER_LAST_TRIGGER_PIN_NONE;
    re->pina_debounce_counter = 0;
    re->pinb_debounce_counter = 0;

    /*get the init GPIO status*/
    hal_gpio_get_input(gpio_port_a, &driver->pina_config.pin_state);
    hal_gpio_get_input(gpio_port_b, &driver->pinb_config.pin_state);

#ifdef BSP_ROTARY_ENCODER_SINGLE_EDGE_TRIGGER
    if (driver->pina_config.pin_state == HAL_GPIO_DATA_LOW) {
        driver->pina_config.trigger_mode = HAL_EINT_EDGE_RISING;
        pina_eint_config.trigger_mode = driver->pina_config.trigger_mode;
    }else {
        driver->pina_config.trigger_mode = HAL_EINT_EDGE_FALLING;
        pina_eint_config.trigger_mode = driver->pina_config.trigger_mode;
    }
    pina_eint_config.debounce_time = config->eint_debounce_time_ms; /*Rotary Encoder mini 4ms*/

    if (driver->pinb_config.pin_state == HAL_GPIO_DATA_LOW) {
        driver->pinb_config.trigger_mode = HAL_EINT_EDGE_RISING;
        pinb_eint_config.trigger_mode = driver->pinb_config.trigger_mode;
    }else {
        driver->pinb_config.trigger_mode = HAL_EINT_EDGE_FALLING;
        pinb_eint_config.trigger_mode = driver->pinb_config.trigger_mode;
    }

    pinb_eint_config.debounce_time = config->eint_debounce_time_ms; /*Rotary Encoder mini 4ms*/
#else
    pina_eint_config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
    pina_eint_config.debounce_time = config->eint_debounce_time_ms; /*Rotary Encoder mini 4ms*/
    pinb_eint_config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
    pinb_eint_config.debounce_time = config->eint_debounce_time_ms; /*Rotary Encoder mini 4ms*/
#endif

    re->pc_state = UPDATE_PIN_STATE(driver);
    hal_gpt_sw_get_timer(&re->timer_handle);

    LOG_MSGID_I(common, "initial prev_curr_state=0x%x eint_debounce_time=%dms timeout=%dms threshold=%d", 4, \
        re->pc_state, config->eint_debounce_time_ms, config->event_timeout_ms, config->event_threshold);

    if ((HAL_EINT_STATUS_OK !=  hal_eint_init(eint_number_a, &pina_eint_config)) ||
        (HAL_EINT_STATUS_OK !=  hal_eint_init(eint_number_b, &pinb_eint_config))) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] EINT init fail", 0);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }

    if ((hal_eint_mask(eint_number_a) != HAL_EINT_STATUS_OK) ||
        (hal_eint_mask(eint_number_b) != HAL_EINT_STATUS_OK)) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] EINT mask fail", 0);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }

    if (HAL_EINT_STATUS_OK !=  hal_eint_register_callback(eint_number_a, bsp_rotary_encoder_pina_eint_callback, (void*)driver) ||
        HAL_EINT_STATUS_OK !=  hal_eint_register_callback(eint_number_b, bsp_rotary_encoder_pinb_eint_callback, (void*)driver)) {
            LOG_MSGID_E(common, "[bsp][rotary encoder] EINT register callback fail", 0);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }
    LOG_MSGID_I(common, "[bsp][rotary_encoder]idx=%d: PINA = %d, eint=%d, mode = %d; PINB = %d, eint=%d, mode = %d", 7,\
            i, \
            driver->pina_config.gpio_port, \
            driver->pina_config.eint_number,\
            driver->pina_config.eint_mode, \
            driver->pinb_config.gpio_port, \
            driver->pinb_config.eint_number, \
            driver->pinb_config.eint_mode);

    /*save the config param*/
    re->user_config = *config;
    if ((hal_eint_unmask(eint_number_a) != HAL_EINT_STATUS_OK) ||
        (hal_eint_unmask(eint_number_b) != HAL_EINT_STATUS_OK)) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] EINT unmask fail", 0);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }
    return BSP_ROTARY_ENCODER_STATUS_OK;
}


/**
 * @brief
 *
 * @return bsp_rotary_encoder_status_t
 */
bsp_rotary_encoder_status_t bsp_rotary_encoder_deinit(bsp_rotary_encoder_port_t port)
{
    uint32_t i = 0;
    hal_gpio_pin_t    gpio_port_a;
    hal_gpio_pin_t    gpio_port_b;
    hal_eint_number_t eint_number_a;
    hal_eint_number_t eint_number_b;
    bsp_rotary_encoder_driver_t *driver;

    if (port >= BSP_ROTARY_ENCODER_MAX) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }
    i = port;
    // for (i = 0; i < BSP_ROTARY_ENCODER_MAX; i++) {
    driver = rotary_encoder[i].driver;
    if (!driver) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] driver is not ready for port %d, please check EPT config or select the right port", 1, i);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }
    gpio_port_a = driver->pina_config.gpio_port;
    gpio_port_b = driver->pinb_config.gpio_port;
    eint_number_a = driver->pina_config.eint_number;
    eint_number_b = driver->pinb_config.eint_number;
    if ((gpio_port_a == 0xFF) || (gpio_port_b == 0xFF) || (eint_number_a == 0xFF) || (eint_number_b == 0xFF)) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] GPIO or EINT config error%d, please check EPT config or select the right port", 1, i);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }

    if ((hal_gpio_deinit(gpio_port_a) != HAL_GPIO_STATUS_OK) || (hal_gpio_deinit(gpio_port_b) != HAL_GPIO_STATUS_OK)) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] GPIO init fail", 0);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }
    if ((HAL_EINT_STATUS_OK !=  hal_eint_deinit(eint_number_a)) ||
        (HAL_EINT_STATUS_OK !=  hal_eint_deinit(eint_number_b))) {
        LOG_MSGID_E(common, "[bsp][rotary encoder] EINT init fail", 0);
        return BSP_ROTARY_ENCODER_STATUS_ERROR;
    }
    hal_gpio_disable_pull(gpio_port_a);
    hal_gpio_disable_pull(gpio_port_b);
    hal_gpt_sw_free_timer(rotary_encoder[i].timer_handle);
    // }
    return BSP_ROTARY_ENCODER_STATUS_OK;
}

/**
 * @brief
 *
 * @return bsp_rotary_encoder_status_t
 */
__attribute__((unused)) static bsp_rotary_encoder_status_t bsp_rotary_encoder_enable(bsp_rotary_encoder_port_t port)
{
    uint32_t i = 0;
    hal_eint_number_t eint_number_a;
    hal_eint_number_t eint_number_b;
    bsp_rotary_encoder_driver_t *driver;

    if (port >= BSP_ROTARY_ENCODER_MAX) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }
    i = port;
    // for (i = 0; i < BSP_ROTARY_ENCODER_MAX; i++) {
        driver = rotary_encoder[i].driver;
        if (!driver) {
            LOG_MSGID_E(common, "[bsp][rotary encoder] driver is not ready for port %d, please check EPT config or select the right port", 1, i);
            return BSP_ROTARY_ENCODER_STATUS_ERROR;
        }
        eint_number_a = driver->pina_config.eint_number;
        eint_number_b = driver->pinb_config.eint_number;
        if ((eint_number_a == 0xFF) || (eint_number_b == 0xFF)) {
            LOG_MSGID_E(common, "[bsp][rotary encoder] EINT config error%d, please check EPT config or select the right port", 1, i);
            return BSP_ROTARY_ENCODER_STATUS_ERROR;
        }
        if ((hal_eint_unmask(eint_number_a) != HAL_EINT_STATUS_OK) ||
            (hal_eint_unmask(eint_number_b) != HAL_EINT_STATUS_OK)) {
            LOG_MSGID_E(common, "[bsp][rotary encoder] EINT unmask fail", 0);
            return BSP_ROTARY_ENCODER_STATUS_ERROR;
        }
    // }
    return BSP_ROTARY_ENCODER_STATUS_OK;
}

/**
 * @brief
 *
 * @return bsp_rotary_encoder_status_t
 */
__attribute__((unused)) static bsp_rotary_encoder_status_t bsp_rotary_encoder_disable(bsp_rotary_encoder_port_t port)
{
    uint32_t i = 0;
    hal_eint_number_t eint_number_a;
    hal_eint_number_t eint_number_b;
    bsp_rotary_encoder_driver_t *driver;

    if (port >= BSP_ROTARY_ENCODER_MAX) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }
    i = port;
    // for (i = 0; i < BSP_ROTARY_ENCODER_MAX; i++) {
        driver = rotary_encoder[i].driver;
        if (!driver) {
            LOG_MSGID_E(common, "[bsp][rotary encoder] driver is not ready for port %d, please check EPT config or select the right port", 1, i);
            return BSP_ROTARY_ENCODER_STATUS_ERROR;
        }
        eint_number_a = driver->pina_config.eint_number;
        eint_number_b = driver->pinb_config.eint_number;
        if ((eint_number_a == 0xFF) || (eint_number_b == 0xFF)) {
            LOG_MSGID_E(common, "[bsp][rotary encoder] EINT config error%d, please check EPT config or select the right port", 1, i);
            return BSP_ROTARY_ENCODER_STATUS_ERROR;
        }
        if ((hal_eint_mask(BSP_ROTARY_ENCODER0_PIN_A_EINT) != HAL_EINT_STATUS_OK) ||
            (hal_eint_mask(BSP_ROTARY_ENCODER0_PIN_B_EINT) != HAL_EINT_STATUS_OK)) {
            LOG_MSGID_E(common, "[bsp][rotary encoder] EINT mask fail", 0);
            return BSP_ROTARY_ENCODER_STATUS_ERROR;
        }
    // }
    return BSP_ROTARY_ENCODER_STATUS_OK;
}

__attribute__((unused)) static bsp_rotary_encoder_status_t bsp_rotary_encoder_set_position(bsp_rotary_encoder_port_t port, uint32_t position)
{
    if (port >= BSP_ROTARY_ENCODER_MAX) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }

    if (position > rotary_encoder[port].max_position) {
        rotary_encoder[port].current_position = rotary_encoder[port].max_position;
    } else {
        rotary_encoder[port].current_position = position;
    }
    return BSP_ROTARY_ENCODER_STATUS_OK;
}

bsp_rotary_encoder_status_t bsp_rotary_encoder_change_config(bsp_rotary_encoder_port_t port, uint32_t event_threshold, uint32_t event_timeout_ms)
{
    if (port >= BSP_ROTARY_ENCODER_MAX) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }

    rotary_encoder[port].user_config.event_threshold = event_threshold;
    rotary_encoder[port].user_config.event_timeout_ms = event_timeout_ms;
    return BSP_ROTARY_ENCODER_STATUS_OK;
}

__attribute__((unused)) static bsp_rotary_encoder_status_t bsp_rotary_encoder_set_position_range(bsp_rotary_encoder_port_t port, uint32_t min, uint32_t max)
{
    if (port >= BSP_ROTARY_ENCODER_MAX) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }
    if (min >= max) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }
    if (max > BSP_ROTARY_ENCODER_MAX_POSITION) {
        return BSP_ROTARY_ENCODER_STATUS_ERROR_INVALID_PARAMETER;
    }
    rotary_encoder[port].min_position = min;
    rotary_encoder[port].max_position = max;
    return BSP_ROTARY_ENCODER_STATUS_OK;
}
