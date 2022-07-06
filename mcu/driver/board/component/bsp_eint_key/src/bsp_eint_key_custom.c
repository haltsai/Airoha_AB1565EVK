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
#include "bsp_eint_key.h"
#include "bsp_eint_key_normal_gpio.h"
#include "bsp_eint_key_rtc_gpio.h"
#include "hal_gpio.h"
#include "hal_log.h"
#include "assert.h"

#ifdef AIR_BSP_EINT_KEY_ACTIVE_LEVEL_CONFIGURABLE
#define BSP_EINTKEY_DECLARE_ACTIVE_LEVEL(num)\
    extern const unsigned char BSP_EINTKEY##num##_ACTIVE_LEVEL

#define BSP_EINTKEY_SET_ACTIVE_LEVEL(new_driver, level)\
do { \
    if (level == 0xFF) {\
        LOG_MSGID_E(eint_key, "[eint_key]eint key is configured to disable pull by EPT on GPIO page, please check. key_data=%d \r\n", 1, new_driver->driver.key_data);\
        assert(0);\
    }\
    new_driver->driver.active_level = level;\
} while (0)
#else
#define BSP_EINTKEY_DECLARE_ACTIVE_LEVEL(num)
#define BSP_EINTKEY_SET_ACTIVE_LEVEL(new_driver, level)
#endif

#define BSP_EINTKEY_DECLARE_EXTERN_VAR(num)\
    extern const char BSP_EINTKEY##num##_PIN;\
    extern const char BSP_EINTKEY##num##_PIN_M_EINT;\
    extern const unsigned char BSP_EINTKEY##num##_EINT;\
    BSP_EINTKEY_DECLARE_ACTIVE_LEVEL(num)

typedef union {
    bsp_eint_key_normal_gpio_driver_t normal;
#ifdef AIR_BSP_EINT_KEY_RTC_GPIO_ENABLE
    bsp_eint_key_rtc_gpio_driver_t rtc;
#endif
} bsp_eint_key_gpio_driver_obj_t;

static bsp_eint_key_gpio_driver_obj_t bsp_eint_key_gpio_driver_buf[BSP_EINT_KEY_NUMBER];

#ifdef AIR_BSP_EINT_KEY_RTC_GPIO_ENABLE
extern void bsp_eint_key_rtc_gpio_driver_new(bsp_eint_key_rtc_gpio_driver_t *new_driver, const char gpio_port);
#define BSP_EINTKEY_RTC_GPIO_DRIVER_INIT(key_index)\
            bsp_eint_key_rtc_gpio_driver_t *new_driver = &bsp_eint_key_gpio_driver_buf[key_index].rtc;\
            bsp_eint_key_rtc_gpio_driver_new(new_driver, BSP_EINTKEY##key_index##_PIN);\
            new_driver->driver.key_data = BSP_EINT_KEY_DATA##key_index;\
            BSP_EINTKEY_SET_ACTIVE_LEVEL(new_driver, BSP_EINTKEY##key_index##_ACTIVE_LEVEL);

#else
#define BSP_EINTKEY_RTC_GPIO_DRIVER_INIT(key_index)
#endif

extern void bsp_eint_key_normal_gpio_driver_new(
    bsp_eint_key_normal_gpio_driver_t *new_driver, const char gpio_port, const char eint_mode, unsigned char eint_number);
#define BSP_EINTKEY_DRIVER_INIT(key_index, available_key_nums)\
{\
    if (BSP_EINTKEY##key_index##_PIN != 0xff) {\
        if (BSP_EINTKEY##key_index##_EINT != 0xff) {\
            bsp_eint_key_normal_gpio_driver_t *new_driver = &bsp_eint_key_gpio_driver_buf[key_index].normal;\
            bsp_eint_key_normal_gpio_driver_new(new_driver, BSP_EINTKEY##key_index##_PIN, BSP_EINTKEY##key_index##_PIN_M_EINT, BSP_EINTKEY##key_index##_EINT);\
            new_driver->gpio_port = BSP_EINTKEY##key_index##_PIN;\
            new_driver->driver.key_data = BSP_EINT_KEY_DATA##key_index;\
            BSP_EINTKEY_SET_ACTIVE_LEVEL(new_driver, BSP_EINTKEY##key_index##_ACTIVE_LEVEL);\
        } else {\
            BSP_EINTKEY_RTC_GPIO_DRIVER_INIT(key_index);\
        }\
        available_key_nums++;\
    }\
}

/* Declaring the extern variable for eintkey.
The number of extern variable groups are decided by the possiable maximum value of BSP_EINT_KEY_NUMBER. */
BSP_EINTKEY_DECLARE_EXTERN_VAR(0);
BSP_EINTKEY_DECLARE_EXTERN_VAR(1);
BSP_EINTKEY_DECLARE_EXTERN_VAR(2);
BSP_EINTKEY_DECLARE_EXTERN_VAR(3);
BSP_EINTKEY_DECLARE_EXTERN_VAR(4);

/* config the GPIO port, eint mode, eint number, key data, active level */

void bsp_eint_key_custom_init(void)
{
    uint32_t available_key_nums = 0;

    BSP_EINTKEY_DRIVER_INIT(0, available_key_nums);
    BSP_EINTKEY_DRIVER_INIT(1, available_key_nums);
    BSP_EINTKEY_DRIVER_INIT(2, available_key_nums);
    BSP_EINTKEY_DRIVER_INIT(3, available_key_nums);
    BSP_EINTKEY_DRIVER_INIT(4, available_key_nums);

    if (0 == available_key_nums) {
        LOG_MSGID_E(eint_key, "[eint_key]eint key pin has not been configured by ept tool on GPIO page, please check BSP_EINTKEYx_PIN\r\n", 0);
        assert(0);
    }
}
