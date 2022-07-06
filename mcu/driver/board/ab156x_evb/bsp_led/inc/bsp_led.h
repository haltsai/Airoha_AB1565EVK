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

#ifndef __BSP_LED_H__
#define __BSP_LED_H__
/**
*File: bsp_led.h
*Description: Provide the interface for app user using.
*
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hal.h"


/**
* @addtogroup BSP
* @{
* @addtogroup LED
* @{
* - BSP LED example code. \n
*  - Step 1. Call #bsp_led_init() to initial bsp led driver.
*  - Step 2. Call #bsp_led_config() to config led timing & callback information.
*  - Step 3. Call #bsp_led_start() make led begin ligthen.
*  - Step 4. Call #bsp_led_stop() make led suspend.
*  - Step 5. Call #bsp_led_deinit() deinit bsp led when not using led.
*  - sample code:
*    @code
*       uint8_t             led_id = BSP_LED_CHANNEL_0;
*       bsp_led_config_t    led_cfg;
*       bsp_led_state_t     led_sta;
*       //delay 2s then make led flashing twice, and then
*       memset(&led_cfg, 0 ,sizeof(led_cfg));
*        led_cfg.onoff = 1;      //enable led on
*       led_cfg.time_unit = 100; // time unit for t0/t1/t2/t3(unit:ms)
*       led_cfg.t0 = 20;         //delay start time
*        led_cfg.t1 = 10;        //led on time in one cycle
*        led_cfg.t2 = 10;        //led off time in one cycle
*        led_cfg.repeat_t1t2 = 2;//led repeat t0t1 times. if equal 0, led always repeat t1t2
*        led_cfg.t3 = 200;       //led off time after finish t1t2 repeat
*        led_cfg.repeat_ext  = 0;//ext loop times.if equal 0, led always repeat (t1t2_repeat + t3)
*        led_cfg.brightness  = 50;//brightness(0~255)
*
*       led_sta = bsp_led_init(led_id, BSP_LED_MODE_BLINK); //led have BSP_LED_MODE_BLINK and BSP_LED_MODE_BREATH mode
*       if(led_sta != BSP_LED_STATUS_OK){
*            printf("[test][led] bsp_led_init err %d", led_sta);
*         return;
*        }
*        led_sta = bsp_led_config(led_id, &led_cfg);
*        if(led_sta != BSP_LED_STATUS_OK){
*            printf("[test][led] bsp_led_config err %d", led_sta);
*            return;
*        }
*        led_sta = bsp_led_start(led_id);
*        if(led_sta != BSP_LED_STATUS_OK){
*            printf("[test][led] bsp_led_start err %d", led_sta);
*            return;
*        }
*
*        //if not want to using, need stop & deinit
*        led_sta = bsp_led_stop(led_id);
*        if(led_sta != BSP_LED_STATUS_OK){
*            printf("[test][led] bsp_led_start err %d", led_sta);
*            return;
*        }
*        led_sta = bsp_led_deinit(led_id);
*        if(led_sta != BSP_LED_STATUS_OK){
*            printf("[test][led] bsp_led_start err %d", led_sta);
*            return;
*        }
*    @endcode
*
* @}
* @}
*/
#define log_bsp_led_info(fmt, cnt, ...)  LOG_MSGID_I(bsp_led, fmt, cnt, ##__VA_ARGS__) /* redefine log info for bsp led */
#define log_bsp_led_warn(fmt, cnt, ...)  LOG_MSGID_W(bsp_led, fmt, cnt, ##__VA_ARGS__) /* redefine log warning for bsp led */
#define log_bsp_led_error(fmt, cnt, ...) LOG_MSGID_E(bsp_led, fmt, cnt, ##__VA_ARGS__) /* redefine log error for bsp led */


/**
 * @brief Callback functioon definition to handle the LED completed event.
 *
 */
typedef void (*bsp_led_callback_t)(uint8_t led, void *);

/**
 * @brief bsp led status.
 */
typedef enum {
    BSP_LED_STATUS_ERROR_BUSY          = -5,       /**< The LED HW is busy. */
    BSP_LED_STATUS_ERROR_NO_DEVICE     = -4,       /**< The no LED device. */
    BSP_LED_STATUS_ERROR_CHANNEL       = -3,       /**< The  error channel. */
    BSP_LED_STATUS_ERROR_INVALID_PARAMETER   = -2, /**< Invalid parameter was given. */
    BSP_LED_STATUS_ERROR               = -1,       /**< The bsp led function error occurred. */
    BSP_LED_STATUS_OK   = 0                        /**< The bsp led operation completed successfully. */
} bsp_led_status_t;

/**
 * @brief bsp led mode.
 */
typedef enum {
    BSP_LED_BREATH_MODE = 0,  /**< The LED mode is breath. */
    BSP_LED_BLINK_MODE  = 1,  /**< The LED mode is blink. */
} bsp_led_mode_t;

/**
 */
#pragma  pack(1)
typedef struct {
    uint8_t     time_unit; /**< t0~t3 time unit(ms) */
    uint8_t     t0;        /**< delay start time before led started */
    uint8_t     t1;        /**< led on time in one twinkle cycle */
    uint8_t     t2;        /**< led off time in one twinkle cycle */
    uint8_t     t3;        /**< led idle time after finish t1t2_repeat */
    uint16_t    repeat_t1t2;  /**< t1t2 repeat times */
    uint16_t    repeat_ext;   /**< extern loop repeat times */
    uint8_t     brightness;   /**< led brightness*/
    uint8_t     onoff;        /**< led on off(1:on,0:off) */
    uint8_t     reserv;       /**< reserve */
    uint32_t    sync_time;    /**< sync time for two device */
    void        *user_data;   /**< user data for call_back using */
    bsp_led_callback_t      call_back;/**< call back when led finished extern loop repeat */
} bsp_led_config_t;
#pragma  pack()


/**
 * @brief This function initialize the led driver.
 * @param[in] ledx is a led number id.
 * @param[in] mode is a mode that led display(breath &or blink mode).
 * @return  #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_init(uint8_t ledx, bsp_led_mode_t mode);

/**
 * @brief This function deinit the led driver.
 * @param[in] ledx is a led number id.
 * @return  #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_deinit(uint8_t ledx);

/**
 * @brief This function config led timing and others.
 * @param[in] ledx is a led number id.
 * @param[in] cfg is the led timing setting.
 * @return  #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_config(uint8_t ledx, bsp_led_config_t *cfg);

/**
 * @brief This function start led flashing.
 * @param[in] ledx is a led number id.
 * @return  #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_start(uint8_t ledx);

/**
 * @brief This function will suspend led flashing. user can resume flashing by call bsp_led_start.
 * @param[in] ledx is a led number id.
 * @return  #HAL_RTC_STATUS_OK, the operation completed successfully.
 */
bsp_led_status_t    bsp_led_stop(uint8_t ledx);

/**
 * @brief This function is resevered for furture using, currently not implement.
 * @param[in] ledx is a led number id.
 * @param[in] cmd is cmd that driver using.
 * @param[in] option is a parameter for extend..
 * @return #BSP_LED_STATUS_OK, the operation completed successfully, else failed.
 */
int                 bsp_led_ioctrl(uint32_t ledx, uint32_t cmd, uint32_t option);
#ifdef __cplusplus
}
#endif

#endif



