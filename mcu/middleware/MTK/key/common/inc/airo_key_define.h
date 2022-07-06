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

#ifndef __AIRO_KEY_DEFINE_H__
#define __AIRO_KEY_DEFINE_H__

#ifdef MTK_EINT_KEY_ENABLE
#include "bsp_eint_key_custom.h"
#include "airo_eint_key.h"
#endif

#include "hal_keypad_table.h"
#include "hal_feature_config.h"
#include "airo_key_config.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define DEBUG_AIRO_TIMER_LOG
//#define DEBUG_AIRO_STATE_LOG
//#define DEBUG_AIRO_FLOW_LOG
#define DEBUG_AIRO_BASIC_FLOW_LOG
// #define DEBUG_AIRO_PR_TIME_LOG
//#define DEBUG_AIRO_PERFORMANCE_LOG

#ifdef AIRO_KEY_FEATRURE_POWERKEY
#define AIRO_KEY_POWERKEY_NUMBER     (1)
#else
#define AIRO_KEY_POWERKEY_NUMBER     (0)
#endif

#ifdef MTK_KEYPAD_ENABLE
#define AIRO_KEY_KEYPAD_NUMBER       (32)
#else
#define AIRO_KEY_KEYPAD_NUMBER       (0)
#endif

#ifdef HAL_CAPTOUCH_MODULE_ENABLED
#ifdef HAL_CPT_FEATURE_4CH
#ifdef AIRO_KEY_CAPTOUCH_NUMBER_USER_CONFIG
#define AIRO_KEY_CAPTOUCH_NUMBER     AIRO_KEY_CAPTOUCH_NUMBER_USER_CONFIG
#else
#define AIRO_KEY_CAPTOUCH_NUMBER     (4)
#endif //AIRO_KEY_CAPTOUCH_NUMBER_USER_CONFIG
#else
#ifdef AIRO_KEY_CAPTOUCH_NUMBER_USER_CONFIG
#define AIRO_KEY_CAPTOUCH_NUMBER     AIRO_KEY_CAPTOUCH_NUMBER_USER_CONFIG
#else
#define AIRO_KEY_CAPTOUCH_NUMBER     (8)
#endif //AIRO_KEY_CAPTOUCH_NUMBER_USER_CONFIG
#endif
#else
#define AIRO_KEY_CAPTOUCH_NUMBER     (0)
#endif

#ifdef MTK_EINT_KEY_ENABLE
#ifdef AIRO_EINT_KEY_NUMBER_USER_CONFIG
#define AIRO_EINT_KEY_NUMBER         (AIRO_EINT_KEY_NUMBER_USER_CONFIG)
#else
#define AIRO_EINT_KEY_NUMBER         (BSP_EINT_KEY_NUMBER)
#endif //AIRO_EINT_KEY_NUMBER_USER_CONFIG
#else
#define AIRO_EINT_KEY_NUMBER         (0)
#endif

#ifdef AIR_PSENSOR_KEY_ENABLE
#define AIRO_PSENSOR_KEY_NUMBER         (1)
#else
#define AIRO_PSENSOR_KEY_NUMBER         (0)
#endif

#ifdef AIR_BSP_INEAR_ENABLE
#define AIRO_EKY_INEAR_TOUCH_NUMBER  (2)
#else
#define AIRO_EKY_INEAR_TOUCH_NUMBER  (0)
#endif
#define AIRO_KEY_STATE_SIZE          (AIRO_KEY_KEYPAD_NUMBER + \
                                      AIRO_KEY_POWERKEY_NUMBER + \
                                      AIRO_EINT_KEY_NUMBER + \
                                      AIRO_KEY_CAPTOUCH_NUMBER + \
                                      AIRO_PSENSOR_KEY_NUMBER + \
                                      AIRO_EKY_INEAR_TOUCH_NUMBER)

#define AIRO_KEY_SUPPORT_NUMBER      AIRO_KEY_STATE_SIZE


#ifdef __cplusplus
}
#endif

#endif /* __AIRO_KEY_DEFINE_H__ */
