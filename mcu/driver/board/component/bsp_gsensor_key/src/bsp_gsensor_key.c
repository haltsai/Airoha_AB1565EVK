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

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "hal.h"
#include "bsp_gsensor_key.h"
#include "bsp_gsensor_key_internal.h"

/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static bsp_gsensor_key_config_t gsensor_context;
/* Extern variables ----------------------------------------------------------*/
extern unsigned char BSP_GSENSOR_KEY_EINT;
extern char BSP_GSENSOR_KEY_PIN;
extern char BSP_GSENSOR_KEY_PIN_M_EINT;


static int32_t bsp_platform_write(void *handle, uint8_t reg, uint8_t *buffer, uint16_t size)
{
    uint8_t buff[20], i;
    hal_i2c_status_t ret;
    hal_i2c_port_t *port;
    bsp_gsensor_t *gsensor = (bsp_gsensor_t*)handle;

    buff[0] = reg;
    port = gsensor->handle;

    for (i = 0; i < size; i++) {
        buff[i + 1] = *(buffer + i);
    }
    ret = hal_i2c_master_send_polling(*port, gsensor->i2c_addr>> 1, buff, size + 1);
    if (ret != HAL_I2C_STATUS_OK) {
        log_hal_msgid_info("hal_i2c_master_send_to_receive_polling ret =%d\r\n",1, ret);
    }

    return 0;
}

static int32_t bsp_platform_read(void *handle, uint8_t reg, uint8_t *buffer, uint16_t size)
{
    uint8_t buff[20];
    hal_i2c_status_t ret;
    hal_i2c_send_to_receive_config_t config;
    hal_i2c_port_t *port;
    bsp_gsensor_t *gsensor = (bsp_gsensor_t*)handle;

    buff[0] = reg;

    port = gsensor->handle;

    config.receive_buffer = buffer;
    config.receive_length = size;
    config.slave_address  = gsensor->i2c_addr >> 1;
    config.send_data      = buff;
    config.send_length    = 1;

    ret = hal_i2c_master_send_to_receive_polling(*port, &config);
    if (ret != HAL_I2C_STATUS_OK) {
        log_hal_msgid_info("hal_i2c_master_send_to_receive_polling ret =%d, port=%d\r\n",2, ret, *port);
    }
    return 0;
}

static void bsp_gsensor_call_user_callback(bsp_gsensor_key_event_t event)
{
    gsensor_context.callback.callback(gsensor_context.key_data, event, gsensor_context.callback.user_data);
}

static void bsp_gsensor_calllback(void *user_data)
{
    hal_eint_number_t *port;
    bsp_gsensor_status_t *gsensor_status;
    static uint32_t s_count = 0;
    static uint32_t d_count=0;

    log_hal_msgid_info("enter [%s]\r\n", 1, __func__);

    port = (hal_eint_number_t *)user_data;

    hal_eint_unmask(*port);

    gsensor_status = bsp_gsensor_get_status();

    if (gsensor_status->tap_type.single_tap) {
        bsp_gsensor_call_user_callback(BSP_GSENSOR_KEY_SINGLE_TAP);
        log_hal_msgid_info("single tap = %d\r\n",1, s_count++);
    }

    if (gsensor_status->tap_type.tap_level == BSP_GSENSOR_TAP_LEVEL_2) {
        if (gsensor_status->tap_type.double_tap) {
            bsp_gsensor_call_user_callback(BSP_GSENSOR_KEY_DOUBLE_TAP);
            log_hal_msgid_info("double tap = %d\r\n",1, d_count++);
        }
    }

}


static void bsp_gsensor_platform_eint_init(hal_eint_callback_t callabck)
{
    hal_eint_config_t eint_config;

    //hal_pinmux_set_function(BSP_GSENSOR_KEY_PIN, BSP_GSENSOR_KEY_PIN_M_EINT);
    //hal_gpio_pull_down(BSP_GSENSOR_KEY_PIN);
    //hal_gpio_set_direction(BSP_GSENSOR_KEY_PIN, HAL_GPIO_DIRECTION_INPUT);

    eint_config.debounce_time = 1;
    eint_config.trigger_mode  = HAL_EINT_EDGE_RISING;

    hal_eint_init(BSP_GSENSOR_KEY_EINT, &eint_config);
    hal_eint_register_callback(BSP_GSENSOR_KEY_EINT, callabck, &BSP_GSENSOR_KEY_EINT);
}
static void bsp_gsensor_platform_i2c_init(void)
{
    hal_i2c_status_t ret;

    ret = hal_i2c_master_init(gsensor_context.i2c.port, &gsensor_context.i2c.conifg); // initialize i2c master.
    if (ret != HAL_I2C_STATUS_OK) {
        log_hal_msgid_info("I2C%d initla fail,ret =%d\r\n",2, gsensor_context.i2c.port, ret);
    } else {
        log_hal_msgid_info("I2C%d initla Done!\r\n",1, gsensor_context.i2c.port);
    }
}

void bsp_gsenosr_key_init(bsp_gsensor_key_config_t *gsensor_config)
{
    static bsp_gsensor_t gsensor_instance;

    gsensor_instance.handle = &gsensor_context.i2c.port;
    memcpy(&gsensor_context, gsensor_config, sizeof(bsp_gsensor_key_config_t));

    log_hal_info("[%s] i2c_port=%d, eint_port=%d, eint_pin=%d\r\n", \
                bsp_gsensor_get_type(),\
                 gsensor_context.i2c.port, \
                 BSP_GSENSOR_KEY_EINT, \
                 BSP_GSENSOR_KEY_PIN);

    bsp_gsensor_platform_i2c_init();

    bsp_gsensor_platform_eint_init(bsp_gsensor_calllback);

    bsp_gsensor_init(bsp_platform_write, bsp_platform_read, &gsensor_instance);
    hal_eint_unmask(BSP_GSENSOR_KEY_EINT);

}

