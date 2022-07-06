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

#include "airo_powerkey.h"

#ifdef AIRO_KEY_FEATRURE_POWERKEY



#include "ept_keypad_drv.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "airo_key_event_internal.h"
#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
#include "hal_pmu.h"
#include "hal_pmu_mt6388_platform.h"
#endif

#if  (PRODUCT_VERSION == 1565)
#include "hal_pmu_ab2565_platform.h"
#endif

#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1568)
#include "hal_pmu.h"
#include "hal_pmu_ab2568_platform.h"
#endif

extern pmu_operate_status_t pmu_pwrkey_normal_key_init(pmu_pwrkey_config_t *config);

#if defined(AB1565)
static bool is_send_simulation_press_event = false;
#endif

static void airo_powerkey_send_state(airo_key_driven_t state)
{
    uint32_t time;
    airo_key_mapping_event_t key_event;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time);
    key_event.state      = state;
    key_event.key_data   = POWERKEY_POSITION;
    key_event.time_stamp = time;
    LOG_MSGID_I(common, "[powerkey] send powerkey state:%d\r\n", 1, (uint32_t)state);

    airo_key_process_key(&key_event, AIRO_KEY_POWERKEY);
}


static bool airo_powerkey_is_pressed(void)
{
#if (defined(AB155X) || defined(AM255X))
    return pmu_get_register_value_2byte_mt6388(PMU_PWRKEY_VAL_ADDR, PMU_PWRKEY_VAL_MASK, PMU_PWRKEY_VAL_SHIFT) ? false : true;
#endif

#if (defined(MT2822S) || defined(MT2822A) || defined(AB1568))
    return pmu_get_register_value(PMU_PSSTS2, PMU_PWRKEY_VAL_MASK, PMU_PWRKEY_VAL_SHIFT) ? false : true;
#endif
#if defined(AB1565)
    return pmu_get_pwrkey_state();
#endif
}

#if defined(AB1565)
static void airo_powerkey_callback1(void)
{
#ifdef AIRO_KEY_FEATRURE_POWERKEY_POWERON_PRESS_EVENT
    /* The first press event on boot procedure will be issued after the scheduler is ready,
       this may be delayed by the initialization flow, so, the first press event is issued
       by airo_powerkey_poweron_press_event_simulation() function when the powerkey is checked
       at the press status.
    */
    static bool is_first_power_on = true;

    if (is_first_power_on && is_send_simulation_press_event) {
        is_first_power_on = false;
        LOG_MSGID_I(common, "[powerkey] press event ignore\r\n", 0);
    } else {
        airo_powerkey_send_state(AIRO_KEY_DRIVEN_PRESS);
    }
#else
    airo_powerkey_send_state(AIRO_KEY_DRIVEN_PRESS);
#endif
}

static void airo_powerkey_callback2(void)
{
    airo_powerkey_send_state(AIRO_KEY_DRIVEN_RELEASE);
}
#else

static void airo_powerkey_check_and_send_state(airo_key_driven_t state)
{
    static airo_key_driven_t last_key_state = AIRO_KEY_DRIVEN_RELEASE;
    airo_key_driven_t extra_state;

#ifndef AIRO_KEY_FEATRURE_POWERKEY_POWERON_PRESS_EVENT
    static bool is_first_power_on = true;
    /* If the powerkey power on press event is not necessary, the step of adding event should
        be skiped when the first event is received. */
    if (!is_first_power_on) {
#endif
        if (last_key_state == state) {
            if (last_key_state == AIRO_KEY_DRIVEN_RELEASE) {
                extra_state = AIRO_KEY_DRIVEN_PRESS;
            } else {
                extra_state = AIRO_KEY_DRIVEN_RELEASE;
            }
            LOG_MSGID_I(common, "[powerkey] event missing : %d\r\n", 1, (uint32_t)extra_state);
            airo_powerkey_send_state(extra_state);
        }
#ifndef AIRO_KEY_FEATRURE_POWERKEY_POWERON_PRESS_EVENT
    } else {
        is_first_power_on = false;
    }
#endif

    airo_powerkey_send_state(state);
    last_key_state = state;
}

static inline airo_key_driven_t airo_powerkey_get_status(void)
{
    return (airo_powerkey_is_pressed() == true) ? AIRO_KEY_DRIVEN_PRESS : AIRO_KEY_DRIVEN_RELEASE;
}

static void airo_powerkey_callback(void)
{
    airo_powerkey_check_and_send_state(airo_powerkey_get_status());
}
static void airo_powerkey_callback_r(void)
{
    // nothing to do now
}

#endif

bool airo_powerkey_init(void)
{
    pmu_pwrkey_config_t config;
    pmu_operate_status_t ret;

#if defined(AB1565)
    config.callback1  = airo_powerkey_callback1;
    config.user_data1 = NULL;
    config.callback2  = airo_powerkey_callback2;
    config.user_data2 = NULL;
#else
    config.callback1  = airo_powerkey_callback;
    config.user_data1 = NULL;
    config.callback2  = airo_powerkey_callback_r;
    config.user_data2 = NULL;
#endif
    ret = pmu_pwrkey_normal_key_init(&config);
    if (ret != PMU_OK) {
        LOG_MSGID_I(common, "[powerkey] powerkey init fail\r\n", 0);
        return false;
    } else {
        LOG_MSGID_I(common, "[powerkey] powerkey init done\r\n", 0);
        return true;
    }
}


void airo_powerkey_poweron_press_event_simulation(void)
{
    if (airo_powerkey_is_pressed()) {
        LOG_MSGID_I(common, "[powerkey] press simulation\r\n", 0);
#if !defined(AB1565)
        airo_powerkey_check_and_send_state(AIRO_KEY_DRIVEN_PRESS);
#else
        is_send_simulation_press_event = true;
        airo_powerkey_send_state(AIRO_KEY_DRIVEN_PRESS);
#endif
    }
}
#endif /*AIRO_KEY_FEATRURE_POWERKEY*/

