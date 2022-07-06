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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hal.h"
#include "bsp_led.h"
#include "bsp_led_isink.h"


log_create_module(bsp_led, PRINT_LEVEL_INFO);
/* ***************************************************************************
 *                     Public API For Bsp LED Driver
 *
 * ****************************************************************************
 */
bsp_led_status_t    bsp_led_init(uint8_t ledx, bsp_led_mode_t mode)
{
#if defined(HAL_ISINK_MODULE_ENABLED)
    return bsp_led_isink_init(ledx, mode);
#endif
    log_bsp_led_error("[bsp][led] not led hw module\r\n", 0);
    return BSP_LED_STATUS_ERROR;
}


bsp_led_status_t    bsp_led_deinit(uint8_t ledx)
{
#if defined(HAL_ISINK_MODULE_ENABLED)
    return bsp_led_isink_deinit(ledx);
#endif
    return BSP_LED_STATUS_ERROR_NO_DEVICE;
}


bsp_led_status_t    bsp_led_config(uint8_t ledx, bsp_led_config_t *cfg)
{
#if defined(HAL_ISINK_MODULE_ENABLED)
    return bsp_led_isink_config(ledx, cfg);
#endif
    return BSP_LED_STATUS_ERROR_NO_DEVICE;
}


bsp_led_status_t    bsp_led_start(uint8_t ledx)
{
#if defined(HAL_ISINK_MODULE_ENABLED)
    return bsp_led_isink_start(ledx);
#endif
    return BSP_LED_STATUS_ERROR_NO_DEVICE;
}


bsp_led_status_t    bsp_led_stop(uint8_t ledx)
{
#if defined(HAL_ISINK_MODULE_ENABLED)
    return bsp_led_isink_stop(ledx);
#endif
    return BSP_LED_STATUS_ERROR_NO_DEVICE;
}


int  bsp_led_ioctrl(uint32_t ledx, uint32_t cmd, uint32_t option)
{
#if defined(HAL_ISINK_MODULE_ENABLED)
    return bsp_led_isink_ioctrl(ledx, cmd, option);
#endif
    return BSP_LED_STATUS_ERROR_NO_DEVICE;
}

