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

#ifdef MTK_EINT_KEY_ENABLE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "hal_log.h"
#include "hal_gpt.h"

#include "bsp_eint_key_custom.h"
#include "airo_key_event_internal.h"
#include "bsp_eint_key.h"

static void airo_eint_key_callback(bsp_eint_key_event_t event, uint8_t key_data, void *user_data)
{
    uint32_t time;
    airo_key_mapping_event_t key_event;


    if ((event == BSP_EINT_KEY_RELEASE) || (event == BSP_EINT_KEY_PRESS)) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time);
        key_event.state      = event;
        key_event.key_data   = key_data;
        key_event.time_stamp = time;
        airo_key_process_key(&key_event, AIRO_KEY_EINT_KEY);
    }
}

bool airo_eint_key_init(void)
{
    bsp_eint_key_config_t key_config;
    uint32_t debounce_time;


    key_config.longpress_time = 0;
    key_config.repeat_time    = 0;

    if (bsp_eint_key_init(&key_config) != true) {
        return false;
    }

    /*set debouce as 5ms, more sensitive*/

    debounce_time = 5;

    bsp_eint_key_set_debounce_time(debounce_time);

    /*register normoal keypad callback*/
    bsp_eint_key_register_callback(airo_eint_key_callback, NULL);

    bsp_eint_key_enable();

    return true;
}

#ifdef AIRO_KEY_FEATRURE_EINTKEY_POWERON_PRESS_EVENT
void airo_eint_key_poweron_press_event_simulation(void)
{
    bsp_eint_key_pressed_key_event_simulation();
}
#endif

#endif /*MTK_EINT_KEY_ENABLE*/

