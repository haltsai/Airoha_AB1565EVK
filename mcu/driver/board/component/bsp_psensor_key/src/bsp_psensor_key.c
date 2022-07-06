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

#include <string.h>
#include "hal.h"
#include "assert.h"
#include "bsp_psensor_key.h"
#include "bsp_psensor_key_custom.h"
#include "sy3088.h"


const char __attribute__((weak)) BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN = BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN_NUM;
const char __attribute__((weak)) BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN_M_EINT = BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN_EINT_MODE;
const unsigned char __attribute__((weak)) BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT = BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT_NUM;

#ifdef BSP_PSENSOR_KEY_POWER_PIN_ENABLE
const char __attribute__((weak)) BSP_PSENSOR_KEY_POWER_PIN = BSP_PSENSOR_KEY_POWER_PIN_NUM;
#endif

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static bsp_psensor_key_config_t bsp_psensor_context;


static int32_t platform_i2c_write(uint8_t addr, uint8_t reg_addr, uint8_t* buffer, uint32_t buffer_len)
{
    uint8_t buff[20], i;
    hal_i2c_status_t ret;
    hal_i2c_port_t port;

    buff[0] = reg_addr;
    port = bsp_psensor_context.i2c.port;

    for (i = 0; i < buffer_len; i++) {
        buff[i + 1] = *(buffer + i);
    }
    ret = hal_i2c_master_send_polling(port, addr, buff, buffer_len + 1);
    if (ret != HAL_I2C_STATUS_OK) {
        log_hal_msgid_error("[bsp][psensor] i2c write error, ret=%d, port=%d\r\n", 2, ret, port);
		return -1;
    }

    return 0;
}

static int32_t platform_i2c_read(uint8_t addr, uint8_t reg_addr, uint8_t* buffer, uint32_t buffer_len)
{
    uint8_t buff[20];
    hal_i2c_status_t ret;
    hal_i2c_send_to_receive_config_t config;
    hal_i2c_port_t port;

    buff[0] = reg_addr;

    port = bsp_psensor_context.i2c.port;

    config.slave_address  = addr;
    config.send_data      = buff;
    config.send_length    = 1;
    config.receive_buffer = buffer;
    config.receive_length = buffer_len;

    ret = hal_i2c_master_send_to_receive_polling(port, &config);

    if (ret != HAL_I2C_STATUS_OK) {
        log_hal_msgid_error("[bsp][psensor] i2c read error, ret=%d, port=%d\r\n", 2, ret, port);
        return -2;
    }
    return 0;
}

static void bsp_psensor_platform_eint_init(hal_eint_callback_t callback)
{
    hal_eint_config_t eint_config;

    if ((BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN == 0xFF) || (BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT == 0xFF)) {
        log_hal_msgid_error("[bsp][psensor] BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN or BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT is not configured!\r\n",0);
        return;
    }

#ifndef BSP_PSENSOR_KEY_USED_EPT_CONFIGURATION
    hal_gpio_init(BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN);
    hal_pinmux_set_function(BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN, BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN_M_EINT);
    hal_gpio_set_direction(BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN, HAL_GPIO_DIRECTION_INPUT);
    /*The board has a pull-up resistor, if no external pull-up resistor ,please set to pull up*/
    //hal_gpio_disable_pull(BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN);
    hal_gpio_pull_up(BSP_PSENSOR_KEY_EVENT_NOTIFY_PIN);
#endif

    eint_config.debounce_time = 100;
    eint_config.trigger_mode  = HAL_EINT_EDGE_FALLING;

#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_mask(BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT);
#endif
    hal_eint_init(BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT, &eint_config);
    hal_eint_register_callback(BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT, callback, NULL);
#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT);
#endif
}

static void platform_bsp_psensor_calllback(void *  user_data)
{
    sy3088_status_t status;
    user_data = user_data;

    if (sy3088_get_sensor_status(&status)) {
        if ((BSP_PSENSOR_KEY_NEAR == (bsp_psensor_key_event_t)status) || (BSP_PSENSOR_KEY_FAR == (bsp_psensor_key_event_t)status)) {
            bsp_psensor_context.callback.callback(bsp_psensor_context.key_data, (bsp_psensor_key_event_t)status, bsp_psensor_context.callback.user_data);
        } else {
            assert(0);
            return;
        }
    }

#ifdef HAL_EINT_FEATURE_MASK
    hal_eint_unmask(BSP_PSENSOR_KEY_EVENT_NOTIFY_EINT);
#endif
}

static bool bsp_psensor_platform_i2c_init(psensor_i2c_config_t *i2c_config)
{
    hal_i2c_status_t ret;

#ifndef BSP_PSENSOR_KEY_USED_EPT_CONFIGURATION
    hal_gpio_init(BSP_PSENSOR_KEY_I2C_SCL_PIN);
    hal_gpio_init(BSP_PSENSOR_KEY_I2C_SDA_PIN);
    hal_pinmux_set_function(BSP_PSENSOR_KEY_I2C_SCL_PIN, BSP_PSENSOR_KEY_I2C_SCL_PIN_MODE);
    hal_pinmux_set_function(BSP_PSENSOR_KEY_I2C_SDA_PIN, BSP_PSENSOR_KEY_I2C_SDA_PIN_MODE);
    hal_gpio_pull_up(BSP_PSENSOR_KEY_I2C_SCL_PIN);
    hal_gpio_pull_up(BSP_PSENSOR_KEY_I2C_SDA_PIN);
#endif
    ret = hal_i2c_master_init(i2c_config->port, &i2c_config->conifg);
    if (ret != HAL_I2C_STATUS_OK) {
        log_hal_msgid_info("[bsp][psensor] I2C%d init fail, ret=%d\r\n",2 , i2c_config->port, ret);
        return false;
    } else {
        log_hal_msgid_info("[bsp][psensor] I2C%d init success!\r\n",1 , i2c_config->port);
    }

    return true;
}


bool bsp_psensor_key_init(bsp_psensor_key_config_t *bsp_psensor_config)
{
    if (bsp_psensor_config == NULL) {
        log_hal_msgid_error("[bsp][psensor] the configuration parameter is null\r\n",0);
        return false;
    }
    memcpy(&bsp_psensor_context, bsp_psensor_config, sizeof(bsp_psensor_key_config_t));

#ifdef BSP_PSENSOR_KEY_POWER_PIN_ENABLE
    if (BSP_PSENSOR_KEY_POWER_PIN == 0xFF) {
        log_hal_msgid_error("[bsp][psensor] BSP_PSENSOR_KEY_POWER_PIN is not configured!\r\n",0);
        return false;
    }

    hal_gpio_init(BSP_PSENSOR_KEY_POWER_PIN);
    hal_pinmux_set_function(BSP_PSENSOR_KEY_POWER_PIN, 0);
    hal_gpio_set_direction(BSP_PSENSOR_KEY_POWER_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(BSP_PSENSOR_KEY_POWER_PIN, HAL_GPIO_DATA_HIGH);
#endif
    if (bsp_psensor_platform_i2c_init(&bsp_psensor_config->i2c)) {
        bsp_psensor_platform_eint_init(platform_bsp_psensor_calllback);
        sy3088_init(platform_i2c_write, platform_i2c_read);
    } else {
        return false;
    }
    return true;
}

void bsp_psensor_key_deinit(void)
{
    sy3088_deinit();
    hal_i2c_master_deinit(bsp_psensor_context.i2c.port);
#ifdef BSP_PSENSOR_KEY_POWER_PIN_ENABLE
    hal_pinmux_set_function(BSP_PSENSOR_KEY_POWER_PIN, 0);
    hal_gpio_set_direction(BSP_PSENSOR_KEY_POWER_PIN, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_pull_down(BSP_PSENSOR_KEY_POWER_PIN);
#endif
}

