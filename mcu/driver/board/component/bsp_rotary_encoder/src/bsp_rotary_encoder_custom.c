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

#include "bsp_rotary_encoder_custom.h"
#include "hal_gpio.h"
#include "hal_log.h"
#include "assert.h"

#define BSP_ROTARY_ENCODER_DECLARE_EXTERN_VAR(num)\
    extern const char BSP_ROTARY_ENCODER##num##_PIN_A;\
    extern const char BSP_ROTARY_ENCODER##num##_PIN_B;\
    extern const char BSP_ROTARY_ENCODER##num##_PIN_A_M_EINT;\
    extern const char BSP_ROTARY_ENCODER##num##_PIN_B_M_EINT;\
    extern const unsigned char BSP_ROTARY_ENCODER##num##_PIN_A_EINT;\
    extern const unsigned char BSP_ROTARY_ENCODER##num##_PIN_B_EINT

extern bsp_rotary_encoder_driver_t * bsp_rotary_encoder_driver_new(bsp_rotary_encoder_port_t port);
#define BSP_ROTARY_ENCODER_DRIVER_INIT(index, available_device_nums)\
{\
    if ((BSP_ROTARY_ENCODER##index##_PIN_A != 0xff) && (BSP_ROTARY_ENCODER##index##_PIN_B != 0xff)\
      &&(BSP_ROTARY_ENCODER##index##_PIN_A_EINT != 0xff) && (BSP_ROTARY_ENCODER##index##_PIN_B_EINT != 0xff)) {\
        bsp_rotary_encoder_driver_t *new_driver = bsp_rotary_encoder_driver_new(index);\
        if (!new_driver)  {\
            LOG_MSGID_E(common, "[bsp][rotary_encoder]rotary encoder new driver fail\r\n", 0);\
            assert(0);\
        }\
        new_driver->pina_config.gpio_port = BSP_ROTARY_ENCODER##index##_PIN_A;\
        new_driver->pinb_config.gpio_port = BSP_ROTARY_ENCODER##index##_PIN_B;\
        new_driver->pina_config.eint_mode = BSP_ROTARY_ENCODER##index##_PIN_A_M_EINT;\
        new_driver->pinb_config.eint_mode = BSP_ROTARY_ENCODER##index##_PIN_B_M_EINT;\
        new_driver->pina_config.eint_number = BSP_ROTARY_ENCODER##index##_PIN_A_EINT;\
        new_driver->pinb_config.eint_number = BSP_ROTARY_ENCODER##index##_PIN_B_EINT;\
        new_driver->port = index;\
        available_device_nums++;\
    }\
}

BSP_ROTARY_ENCODER_DECLARE_EXTERN_VAR(0);
BSP_ROTARY_ENCODER_DECLARE_EXTERN_VAR(1);
BSP_ROTARY_ENCODER_DECLARE_EXTERN_VAR(2);

void bsp_rotary_encoder_custom_init(void)
{
    uint32_t available_device_nums = 0;

    BSP_ROTARY_ENCODER_DRIVER_INIT(0, available_device_nums);
    BSP_ROTARY_ENCODER_DRIVER_INIT(1, available_device_nums);
    BSP_ROTARY_ENCODER_DRIVER_INIT(2, available_device_nums);

    if (0 == available_device_nums) {
        LOG_MSGID_E(common, "[bsp][rotary_encoder]rotary encoder pin or eint has not been configured by ept tool on GPIO page\r\n", 0);
    }
}

