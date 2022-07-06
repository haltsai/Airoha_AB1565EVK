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

#ifndef _APP_LED_CONTROL_H_
#define _APP_LED_CONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * File: app_led_control.h
 * Description: This file is for user to control led , which include  read style information from NVDM and provide the pattern & style for user using.
 * Note: None
 */


#include "bsp_led.h"
#include "led_control_style_cfg.h"

/**
* @brief This struct for record users operate history
*/
typedef struct {
    bool                enable;     /**< Enable this pattern */
    led_pattern_type_t  pattern;    /**< Led pattern type */
    uint16_t            style_no;   /**< Led style number */
} led_pattern_record_t;

/**
* @brief This function initializes the app led module.
* @return None
*/
void    app_led_control_init();

/**
* @brief This function de-initializes the app led module.
* @return None
*/
void    app_led_control_deinit();

/**
* @brief This function enable the led according the pattern and style.
* @param[in] pattern is the pattern type that user want to use.
* @param[in] style_no is the style number that user want to use.
* @param[in] force_option is a flag means whether this operation take effect immediatly.
* @param[in] dly_ms is the delay time before led start.
* @return #TRUE means success.
*              #FALSE means error.
*/
bool    app_led_control_enable_1(led_pattern_type_t    pattern, uint8_t  style_no, bool force_option, uint32_t dly_ms);

/**
* @brief This function enable the led according the pattern and style with gpt stamp sync.
* @param[in] pattern is the pattern type that user want to use.
* @param[in] style_no is the style number that user want to use.
* @param[in] force_option is a flag means whether this operation take effect immediatly.
* @param[in] need_sync is a flag whether need gpt stamp sync.
* @oaram[in] gpt_tick is the gpt tick stamp for two device sync.
* @return #TRUE means success.
*              #FALSE means error.
*/
bool    app_led_control_enable_with_sync(led_pattern_type_t    pattern, uint8_t  style_no, bool force_option, bool need_sync, uint32_t gpt_tick);


/**
* @brief This function enable the led according the pattern and style.
* @param[in] pattern is the pattern type that user want to use.
* @param[in] style_no is the style number that user want to use.
* @param[in] force_option is a flag means whether this operation take effect immediatly.
* @return #TRUE means success.
               #FALSE means error.
*/
bool    app_led_control_enable(led_pattern_type_t    pattern, uint8_t  style_no, bool force_option);

/**
* @brief This function disable the led  pattern.
* @param[in] pattern is the pattern type that user want to use.
* @param[in] force_option is a flag means whether this operation take effect immediatly.
* @return None
*/
void    app_led_control_disable(led_pattern_type_t    pattern, bool force_option);

/**
* @brief This function get pattern type that current active.
* @return the led pattern
*/
led_pattern_type_t  app_led_control_get_currentpattern();

#ifdef __cplusplus
}
#endif
#endif /* End _APP_LED_CONTROL_H_ */

