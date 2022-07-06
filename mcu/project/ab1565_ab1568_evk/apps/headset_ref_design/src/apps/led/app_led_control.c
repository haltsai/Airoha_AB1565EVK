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
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hal.h"
#include "led_control_internal.h"
#include "led_control_style_cfg.h"
#include "app_led_control.h"


#define     log_led_app_info(fmt, cnt, ...)     log_led_info(fmt, cnt, ##__VA_ARGS__)
#define     log_led_app_warning(fmt, cnt, ...)  log_led_error(fmt, cnt, ##__VA_ARGS__)
#define     log_led_app_error(fmt, cnt, ...)    log_led_error(fmt, cnt, ##__VA_ARGS__)


#define     GET_CURRENT_PATTERN()               app_led_control_get_active_pattern()

/***************************************** Private variable********************************************/
static  uint32_t                g_immediately_enable = false;
static  led_pattern_record_t    history_record[LED_PATTERN_NONE];
/***************************************** Private function********************************************/
led_pattern_type_t  app_led_control_get_active_pattern();

static  void    led_control_printf(one_led_style_t *p_style)
{
    log_led_app_info("[app][led] led      on/off:%d\r\n", 1, p_style->onoff);
    log_led_app_info("[app][led] t0             :%d\r\n", 1, p_style->t0);
    log_led_app_info("[app][led] t1             :%d\r\n", 1, p_style->t1);
    log_led_app_info("[app][led] t2             :%d\r\n", 1, p_style->t2);
    log_led_app_info("[app][led] t3             :%d\r\n", 1, p_style->t3);
    log_led_app_info("[app][led] time unit      :%d\r\n", 1, p_style->time_unit);
    log_led_app_info("[app][led] t1t2   repeat  :%d\r\n", 1, p_style->repeat_t1t2);
    log_led_app_info("[app][led] extend repeat  :%d\r\n", 1, p_style->repeat_ext);
    log_led_app_info("[app][led] brightness     :%d\r\n", 1, p_style->brightness);
}


static void  led_control_enable(led_style_config_t   *p_style, uint32_t  syn_tm_ms)
{
    uint32_t   i = 0;
    uint8_t    led_pattern = 0;

    if (led_style_config_sys_mode() == LED_SYSTEM_MODE_LP_TEST) {
        log_led_app_error("[app][led] system in lowpower test mode\r\n", 0);
        return;
    }
    if (p_style == NULL) {
        log_led_app_error("[app][led] led_control_enable() input arg is null\r\n", 0);
        return;
    }
    log_led_app_info("[app][led] led nums %d\r\n", 1, p_style->led_num);
    for (i = 0; i < p_style->led_num; i++) {
        led_pattern      = p_style->led_setting[i].onoff;
        log_led_app_info("############# Led %d style setting #############\r\n", 1, i);
        led_control_printf(&p_style->led_setting[i]);
        if (led_pattern == LED_OFF) {
            led_style_disable(i);
        } else if (led_pattern == LED_ON) {
            led_style_disable(i);
            led_style_enable(i, &p_style->led_setting[i], syn_tm_ms);
        }
    }
}

static  void    led_control_disable(uint8_t max_led_num)
{
    uint32_t    i = 0;

    if (led_style_config_sys_mode() == LED_SYSTEM_MODE_LP_TEST) {
        log_led_app_error("[app][led] system in lowpower test mode\r\n", 0);
        return;
    }
    for (i = 0; i < max_led_num; i++) {
        led_style_disable(i);
    }
}

static  bool    led_control_is_need_enable(led_pattern_type_t    pattern, uint8_t style_no)
{
    led_pattern_type_t              curren_pattern;

    curren_pattern = GET_CURRENT_PATTERN();

    if (pattern == curren_pattern && history_record[pattern].enable == true && history_record[pattern].style_no == style_no) {
        return false;
    } else {
        return true;
    }
}


static  bool    led_control_is_twinkle_mode(led_style_config_t *config)
{
    int     i = 0;

    if (config == NULL) {
        return true;
    }
    for (i = 0; i < config->led_num; i++) {
        if (config->led_setting[i].onoff == 1 && config->led_setting[i].t1 > 0 && config->led_setting[i].t2 > 0) {
            return true;
        }
    }
    return false;
}


/***************************************** Public function********************************************/

led_pattern_type_t  app_led_control_get_active_pattern()
{
    if (true == history_record[LED_PATTERN_FILTER].enable) {
        return LED_PATTERN_FILTER;
    } else if (true == history_record[LED_PATTERN_FG].enable) {
        return LED_PATTERN_FG;
    } else if (true == history_record[LED_PATTERN_BG].enable) {
        return LED_PATTERN_BG;
    }
    return LED_PATTERN_NONE;
}


void    app_led_control_init()
{
    memset(history_record, 0, sizeof(history_record));
    led_style_init();
    led_style_config_init();
}

void    app_led_control_deinit()
{
    led_style_deinit();
    led_style_config_deinit();
}


bool    app_led_control_enable_1(led_pattern_type_t    pattern, uint8_t  style_no, bool force_option, uint32_t syn_tm_ms)
{
    led_pattern_type_t              curren_pattern;
    led_style_config_t             *p_style = NULL;
    uint32_t                        irq_status;
    bool                            is_need_enable;

    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    is_need_enable = led_control_is_need_enable(pattern, style_no);

    /* If force is true, setting pattern immediately */
    if (force_option) {
        g_immediately_enable = true;
        hal_nvic_restore_interrupt_mask(irq_status);
        goto led_setting;
    }
    /* Record pattern */
    history_record[pattern].enable  = true;
    history_record[pattern].pattern = pattern;
    history_record[pattern].style_no = style_no;
    hal_nvic_restore_interrupt_mask(irq_status);

    /* Get current exist high priority pattern */
    curren_pattern = GET_CURRENT_PATTERN();

    /* Need change pattern? */
    if (curren_pattern < LED_PATTERN_NONE) {
        if (pattern < curren_pattern) {
            log_led_app_error("[app][led] pattern(%d) lower than current(%d)\r\n", 2, pattern, curren_pattern);
            return false;
        }
    }

led_setting:
    log_led_app_info("[app][led] enable parttern %d, style %d\r\n", 2, pattern, style_no);
    p_style =  led_style_config_read(pattern, style_no);
    if (p_style == NULL) {
        log_led_app_error("[app][led] style not exist in nvdm & rom\r\n", 0);
        return false;
    }
    /* If new pattern equal previou pattern & led is always on or off, then no need update led hw */
    if (is_need_enable || led_control_is_twinkle_mode(p_style)) {
        led_control_enable(p_style, syn_tm_ms);
    }

    return  true;
}



bool    app_led_control_enable(led_pattern_type_t    pattern, uint8_t  style_no, bool force_option)
{
    return app_led_control_enable_1(pattern, style_no, force_option, 0);
}


bool    app_led_control_enable_with_sync(led_pattern_type_t    pattern, uint8_t  style_no, bool force_option, bool need_sync, uint32_t gpt_tick)
{
    uint32_t    tick_curr = 0;
    uint32_t    tick_ofs  = 0;

    if (need_sync) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &tick_curr);
        hal_gpt_get_duration_count(tick_curr, gpt_tick, &tick_ofs);
    }
    return app_led_control_enable_1(pattern, style_no, force_option, (tick_ofs / 1000)); /* (tick_ofs/1000) for convert us to ms */
}


void    app_led_control_disable(led_pattern_type_t    pattern, bool force_option)
{
    led_pattern_type_t  curren_pattern;
    uint32_t            irq_status;
    led_style_config_t    *p_style = NULL;

    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    /* If force_disable is true, need switch pattern */
    if (force_option) {
        g_immediately_enable = false;
    } else {
        /* Check setting pattern priority */
        curren_pattern = GET_CURRENT_PATTERN();
        history_record[pattern].enable = false;
        if (pattern < curren_pattern) {
            hal_nvic_restore_interrupt_mask(irq_status);
            log_led_app_error("[app][led] pattern(%d) priority low than current(%d)\r\n", 2, pattern, curren_pattern);
            return;
        }
    }
    hal_nvic_restore_interrupt_mask(irq_status);

    if (g_immediately_enable) {
        return;
    }
    curren_pattern = GET_CURRENT_PATTERN();
    if (curren_pattern < LED_PATTERN_NONE) {
        /* Get low priorty pattern setting and enable */
        p_style = led_style_config_read(history_record[curren_pattern].pattern, history_record[curren_pattern].style_no);
        led_control_enable(p_style, 0);
        log_led_app_info("[app][led] pattern %d be activated(style %d), by disable pattern %d one\r\n", 3,
                         history_record[curren_pattern].pattern,
                         history_record[curren_pattern].style_no,
                         pattern);
    } else {
        uint8_t led_num = 0;

        log_led_app_info("[app][led] no active pattern, led will be turn off \r\n", 0);
        p_style = led_style_config_read(pattern, 0);
        led_num = (p_style == NULL) ? 2 : (p_style->led_num);
        led_control_disable(led_num);
    }
}



