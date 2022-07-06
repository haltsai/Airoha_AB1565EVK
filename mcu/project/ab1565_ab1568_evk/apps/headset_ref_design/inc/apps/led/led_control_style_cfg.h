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

#ifndef  _LED_CONTROL_STYLE_CFG_H_
#define  _LED_CONTROL_STYLE_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif
/**
 * File: led_control_style_cfg.h
 * Description: This file is to read led configure infomation from NVDM.
 * Note: None
 */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hal_platform.h"
#include "hal.h"
#include "syslog.h"


#define     log_led_info(fmt, cnt, ...)     LOG_MSGID_I(led_log, fmt, cnt, ##__VA_ARGS__) /* Redefine led log info api */
#define     log_led_warn(fmt, cnt, ...)     LOG_MSGID_W(led_log, fmt, cnt, ##__VA_ARGS__) /* Redefine led log warning api */
#define     log_led_error(fmt, cnt, ...)    LOG_MSGID_E(led_log, fmt, cnt, ##__VA_ARGS__) /* Redefine led log error api */

#define     LED_SYSTEM_MODE_NORMAL          0 /* Define normal mode,that led will not turn off when system enter sleep */
#define     LED_SYSTEM_MODE_LP_TEST         1 /* Define lowpower test mode for user measure system lowpower */

/**
 * @brief This enum for define status
 */
typedef enum {
    APP_LED_STATUS_OK = 0,     /**< The operation completed successfully. */
    APP_LED_STATUS_ERROR = -1, /**< An error occurred. */
} app_led_status_t;

/**
 * @brief This enum for define led pattern
 */
typedef enum {
    LED_PATTERN_BG = 0, /**< The backgroud pattern */
    LED_PATTERN_FG,     /**< The foregroud pattern */
    LED_PATTERN_FILTER, /**< The filter pattern */
    LED_PATTERN_NONE,   /**< The error pattern */
} led_pattern_type_t;

/**
* @ brief the struct is for led config that stored in NVDM
*/
#pragma  pack(1)
typedef struct {
    uint8_t  time_unit;     /**< t0/t1/t2/t3 time unit(ms) */
    uint8_t  t0;            /**< delay time before start repeat t1t2 */
    uint8_t  t1;            /**< led on time in one cycle */
    uint8_t  t2;            /**< led off time in one cycle */
    uint8_t  t3;            /**< idle time when finish t1t2 repeat times */
    uint16_t  repeat_t1t2;  /**< t1t2  repeat times */
    uint16_t  repeat_ext;   /**< t0~t3 repeat times */
    uint8_t   brightness;   /**< led brightness */
    uint8_t   onoff;        /**< led on off(1:on,0:off) */
} one_led_style_t;
#pragma  pack()


/**
 * @brief This struct for led style config setting which stored in NVDM.
 */
typedef struct {
    uint8_t                  led_num;    /**< led num that user configured by config tool*/
    one_led_style_t         *led_setting;/**< led style setting that user configured by config tool*/
} led_style_config_t;


/**
 * @brief This function initialize the style config which will read config info from NVDM.
 * @return None
 */
void                  led_style_config_init(void);

/**
 * @brief This function de-initialize the style config.
 * @return None
 */
void                  led_style_config_deinit(void);


/**
 * @brief This function get led style configure infomation.
 * @param[in] pattern is the pattern type that LED want to read.
 * @param[in] style_no is the style number that led want to read.
 * @return #Not NULL  means success.
 *         #NULL  means fail.
 */
led_style_config_t   *led_style_config_read(led_pattern_type_t    pattern, uint8_t  style_no);


/**
 * @brief This function get system mode
 * @return #LED_SYSTEM_MODE_NORMAL
 *             #LED_SYSTEM_MODE_LP_TEST
 */
uint8_t               led_style_config_sys_mode(void);


#ifdef __cplusplus
}
#endif
#endif /*END _LED_CONTROL_STYLE_CFG_H_*/


