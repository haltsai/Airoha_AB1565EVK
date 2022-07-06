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

#include "hal_platform.h"
#ifndef __HAL_PWM_INTERNAL_H__
#define __HAL_PWM_INTERNAL_H__

#include "hal_nvic_internal.h"
#ifdef HAL_PWM_MODULE_ENABLED

//#define PWM_DEBUG_ENABLE

#define PWM_BASE_ADDR_ARRAY         {PWM0, PWM1, PWM2, PWM3, PWM4}
#define PWM_CLOCK_CG_ARRAY          {HAL_CLOCK_CG_PWM0, HAL_CLOCK_CG_PWM1, HAL_CLOCK_CG_PWM2, HAL_CLOCK_CG_PWM3, HAL_CLOCK_CG_PWM4}

#define PWM_CLOCK_32K               (32768)
#define PWM_CLOCK_13M               (13000000)
#define PWM_CLOCK_24M               (24000000)
#define PWM_CLOCK_26M               (26000000)
#define PWM_CLOCK_40M               (40000000)
#define PWM_CLOCK_41M               (41600000)
#define PWM_CLOCK_48M               (48000000)

#define PWM_MAX_COUNT               (0x1fff)
#define PWM_INIT                    (1)
#define PWM_DEINIT                  (0)

#define PWM_STATE_INITIALIZED_OFS   (0)
#define PWM_STATE_SET_FREQ_OFS      (1)
#define PWM_STATE_SET_DUTY_OFS      (2)
#define PWM_STATE_START_OFS         (3)

#define PWM_STATE_INIT_MASK         (1<<PWM_STATE_INITIALIZED_OFS)
#define PWM_STATE_SET_FREQ_MASK     (1<<PWM_STATE_SET_FREQ_OFS)
#define PWM_STATE_SET_DUTY_MASK     (1<<PWM_STATE_SET_DUTY_OFS)
#define PWM_STATE_START_MASK        (1<<PWM_STATE_START_OFS)

#ifdef PWM_DEBUG_ENABLE
#define log_pwm_info(_message, cnt, ...)      log_hal_msgid_info(_message,cnt,    ##__VA_ARGS__)
#define log_pwm_warn(_message, cnt, ...)      log_hal_msgid_warning(_message,cnt, ##__VA_ARGS__)
#define log_pwm_error(_message,cnt, ...)      log_hal_msgid_error(_message,cnt,   ##__VA_ARGS__)
#else
#define log_pwm_info(...)
#define log_pwm_warn(...)
#define log_pwm_error(...)
#endif

#endif /*HAL_PWM_MODULE_ENABLED*/
#endif

