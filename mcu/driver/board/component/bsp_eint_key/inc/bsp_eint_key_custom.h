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

#ifndef _BSP_EINT_KEY_CUSTOM_H_
#define _BSP_EINT_KEY_CUSTOM_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stdbool.h>
#include "bsp_eint_key.h"
// #include "hal_keypad_table.h"

#define EINT_KEY_USED_EPT_CONFIGURATION
#define EINT_KEY_HAL_MASK

/* Active level means that what level the gpio should be, when the eint key is pressed */
#ifdef BSP_EINT_KEY_NORMAL_GPIO_ACTIVE_HIGH
#define BSP_EINT_KEY_NORMAL_GPIO_ACTIVE_LEVEL  HAL_GPIO_DATA_HIGH
#else
#define BSP_EINT_KEY_NORMAL_GPIO_ACTIVE_LEVEL  HAL_GPIO_DATA_LOW
#endif

#ifdef BSP_EINT_KEY_RTC_GPIO_ACTIVE_HIGH
#define BSP_EINT_KEY_RTC_GPIO_ACTIVE_LEVEL  HAL_GPIO_DATA_HIGH
#else
#define BSP_EINT_KEY_RTC_GPIO_ACTIVE_LEVEL  HAL_GPIO_DATA_LOW
#endif

/**@brief If user need eintkey report long press event and repeat event, uncomment the line below.
 * In airokey module, only need press and releae event.so, eintkey do not need to report other event.
 */
// #define BSP_EINT_KEY_TIMING_ENABLE

/*user could modify for your keydata*/
#define BSP_EINT_KEY_NUMBER             5    /*default supported key numbers*/
#define BSP_EINT_KEY_DATA0              126  /*keyad defined by user, must be 125<key_data<254*/
#define BSP_EINT_KEY_DATA1              127  /*keyad defined by user, must be 125<key_data<254*/
#define BSP_EINT_KEY_DATA2              128  /*keyad defined by user, must be 125<key_data<254*/
#define BSP_EINT_KEY_DATA3              129  /*keyad defined by user, must be 125<key_data<254*/
#define BSP_EINT_KEY_DATA4              130  /*keyad defined by user, must be 125<key_data<254*/

void bsp_eint_key_custom_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _BSP_EINT_KEY_CUSTOM_H_ */

