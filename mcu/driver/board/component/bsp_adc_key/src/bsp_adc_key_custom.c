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

#include "hal_log.h"
#include "bsp_adc_key.h"
#include "bsp_adc_key_custom.h"

extern void bsp_adc_key_driver_init(bsp_adc_key_channel_t key_channel, uint32_t adc_channel, uint32_t pin, uint32_t pinmux);

#define BSP_ADCKEY_DRIVER_INIT(adc_channel, available_key_nums)\
{\
    if (HAL_AUXADC_##adc_channel##_PIN != 0xff) {\
        if (HAL_AUXADC_##adc_channel##_PIN_M_AUXADC##adc_channel != 0xff) {\
            bsp_adc_key_driver_init((bsp_adc_key_channel_t)available_key_nums, adc_channel, HAL_AUXADC_##adc_channel##_PIN, HAL_AUXADC_##adc_channel##_PIN_M_AUXADC##adc_channel);\
            available_key_nums++;\
        }\
    }\
}

#define BSP_ADCKEY_DECLARE_EXTERN_VAR(adc_channel)\
    extern const char HAL_AUXADC_##adc_channel##_PIN;\
    extern const char HAL_AUXADC_##adc_channel##_PIN_M_AUXADC##adc_channel;

/* Declaring the extern variable for adckey.*/
BSP_ADCKEY_DECLARE_EXTERN_VAR(0);
BSP_ADCKEY_DECLARE_EXTERN_VAR(1);
BSP_ADCKEY_DECLARE_EXTERN_VAR(2);
BSP_ADCKEY_DECLARE_EXTERN_VAR(3);
BSP_ADCKEY_DECLARE_EXTERN_VAR(4);
BSP_ADCKEY_DECLARE_EXTERN_VAR(5);
BSP_ADCKEY_DECLARE_EXTERN_VAR(6);
BSP_ADCKEY_DECLARE_EXTERN_VAR(7);

/* config the GPIO port,key data*/
void bsp_adc_key_custom_init(void)
{
    uint32_t available_key_nums = 0;

    BSP_ADCKEY_DRIVER_INIT(0, available_key_nums);
    BSP_ADCKEY_DRIVER_INIT(1, available_key_nums);
    BSP_ADCKEY_DRIVER_INIT(2, available_key_nums);
    BSP_ADCKEY_DRIVER_INIT(3, available_key_nums);
    BSP_ADCKEY_DRIVER_INIT(4, available_key_nums);
    BSP_ADCKEY_DRIVER_INIT(5, available_key_nums);
    BSP_ADCKEY_DRIVER_INIT(6, available_key_nums);
    BSP_ADCKEY_DRIVER_INIT(7, available_key_nums);

    if (0 == available_key_nums) {
        LOG_MSGID_W(adc_key, "[adc_key]adc key pin has not been configured by ept tool on GPIO page, please check HAL_AUXADC_x_PIN\r\n", 0);
        // assert(0);
    }
}
