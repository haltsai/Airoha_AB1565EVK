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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hal.h"
#include "bsp_led.h"
#include "led_control_internal.h"

#define     APP_LED_MODE    BSP_LED_BLINK_MODE//BSP_LED_BLINK_MODE or BSP_LED_BREATH_MODE

/*****************************************public function*********************************************/

void    led_style_init()
{
}

void    led_style_deinit()
{
}

app_led_status_t    led_style_enable(uint8_t ledx, one_led_style_t *config, uint32_t syn_tm_ms)
{
    bsp_led_config_t    bsp_cfg;

    if (config == NULL) {
        return APP_LED_STATUS_ERROR;
    }
    /* Init bsp led. */
    if (BSP_LED_STATUS_OK != bsp_led_init(ledx, APP_LED_MODE)) {
        return APP_LED_STATUS_ERROR;
    }

    /* Config bsp led. */
    memcpy(&bsp_cfg, config, sizeof(one_led_style_t));
    bsp_cfg.sync_time = syn_tm_ms;
    bsp_cfg.call_back = NULL;
    bsp_cfg.user_data = NULL;
    if (BSP_LED_STATUS_OK != bsp_led_config(ledx, &bsp_cfg)) {
        return APP_LED_STATUS_ERROR;
    }
    /* Start bsp led. */
    if (BSP_LED_STATUS_OK != bsp_led_start(ledx)) {
        return APP_LED_STATUS_ERROR;
    }
    return APP_LED_STATUS_OK;
}

app_led_status_t    led_style_disable(uint8_t ledx)
{
    /* Stop bsp led. */
    if (BSP_LED_STATUS_OK !=  bsp_led_stop(ledx)) {
        return APP_LED_STATUS_ERROR;
    }
    /* Deinit bsp led. */
    if (BSP_LED_STATUS_OK != bsp_led_deinit(ledx)) {
        return APP_LED_STATUS_ERROR;
    }
    return APP_LED_STATUS_OK;
}



