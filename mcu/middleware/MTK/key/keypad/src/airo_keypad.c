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

#ifdef MTK_KEYPAD_ENABLE
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include <stdlib.h>
#include <assert.h>

#include "hal_log.h"
#include "task_def.h"
#include "hal_gpt.h"

#include "keypad_custom.h"
#include "airo_key_event_internal.h"
#include "airo_keypad.h"

static void airo_keypad_normal_key_callback(void *parameter)
{
    airo_key_send_message(SCT_GET_KEY_DATA_KEYPAD, NULL);
}

#ifdef HAL_KEYPAD_FEATURE_POWERKEY
static void airo_keypad_powerkey_callback(void *parameter)
{
    airo_key_send_message(SCT_GET_KEY_DATA_POWERKEY, NULL);
}
#endif



bool airo_keypad_init(void)
{
    keypad_custom_event_time_t hal_event_time;
    uint32_t hal_debouce_time;

    /*init keypad and powerkey, if no powerkey ignore*/
    if (keypad_custom_init() != true) {
        return false;
    }

    /*set hal driver event as driven signal, 100ms per event*/
    hal_event_time.longpress_time = airo_key_driven_signal_time;
    hal_event_time.repet_time     = airo_key_driven_signal_time;
    keypad_custom_set_event_time(&hal_event_time);

    /*set debouce as 5ms, more sensitive*/
#ifdef __CFW_CONFIG_SUPPORT__
    hal_debouce_time = CFW_CFG_ITEM_VALUE(cfw_airo_key_debounce_time);
#else
    hal_debouce_time = SCT_KEY_DEFAULT_DEBOUNCE_TIME;
#endif
    hal_keypad_set_debounce(&hal_debouce_time);

    /*register normoal keypad callback*/
    hal_keypad_register_callback(airo_keypad_normal_key_callback, NULL);

    /*enable normal keypad driver*/
    hal_keypad_enable();

#ifdef HAL_KEYPAD_FEATURE_POWERKEY
    /*register powerkey callback and enable powerkey*/
    hal_keypad_powerkey_register_callback(airo_keypad_powerkey_callback, NULL);
#endif

    return true;
}



#endif /*MTK_KEYPAD_ENABLE*/

