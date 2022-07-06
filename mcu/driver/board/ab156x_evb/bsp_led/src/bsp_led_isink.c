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
#include <assert.h>
#include "hal.h"
#include "bsp_led_isink.h"

#ifdef HAL_ISINK_MODULE_ENABLED

//#define BSP_LED_SET_ALWAYSON_TO_REG_MODE
#define BSP_LED_SW_CALI_FEATURE
/////////////////////////////////////////Macro Defined//////////////////////////////////////////////
#define BSP_T1_T2_REPEAT_CALI_TM   10000/* unit:ms  */

#define BSP_MS_TO_TICK(tick)      (tick * 32.768f)
#define BSP_LED_T0_TO_MS(channel) (g_led_priv_info[channel].led_cfg.t0 * g_led_priv_info[channel].led_cfg.time_unit)
#define BSP_LED_T1_TO_MS(channel) (g_led_priv_info[channel].led_cfg.t1 * g_led_priv_info[channel].led_cfg.time_unit)
#define BSP_LED_T2_TO_MS(channel) (g_led_priv_info[channel].led_cfg.t2 * g_led_priv_info[channel].led_cfg.time_unit)
#define BSP_LED_T3_TO_MS(channel) (g_led_priv_info[channel].led_cfg.t3 * g_led_priv_info[channel].led_cfg.time_unit)
#define BSP_LED_T1_T2_WITH_REPEAT_TO_MS(channel) (g_led_priv_info[channel].t1t2_cycle * g_led_priv_info[channel].led_cfg.repeat_t1t2)
#define BSP_LED_EXT_LOOP_MS(channel) (BSP_LED_T1_T2_WITH_REPEAT_TO_MS(channel) +BSP_LED_T3_TO_MS(channel) )

/////////////////////////////////////////Function Declare//////////////////////////////////////////////
extern hal_isink_status_t   hal_isink_start(uint32_t channel);
extern hal_isink_status_t   hal_isink_stop(uint32_t channel);
extern void                 bsp_led_process(bsp_led_channel_t channel, bsp_led_action_t action);
extern hal_isink_status_t   hal_isink_enable_pwm_mode(uint32_t channel, uint32_t cycle_ms, uint32_t duty_persent);


////////////////////////////////////Private Variable Define//////////////////////////////////////////////
static  bsp_isink_private_info_t            g_led_priv_info[BSP_LED_CHANNEL_MAX] = {{0}, {0}};
static  const uint16_t                      g_led_isink_tm_tb[] = {123, 338, 523, 707, 926, 1107, 1291, 1507, 1691, 1876, 2091, 2276, 2460, 2676, 2860, 3075};
/*
    Comments:
     Why led_table[BSP_LED_STATE_IDLE][BSP_LED_ACTION_OF_STOP] == BSP_LED_STATE_IDLE?
     Why led_table[BSP_LED_STATE_IDLE][BSP_LED_ACTION_OF_TIMEOUT] == BSP_LED_STATE_IDLE?
             Just for void race condition, because bsp_led_enable() and bsp_led_disable() can be called by any thread, and maybe interrupt led_process() which executed by the GPT IRQ thread.
*/
static  uint32_t                            g_led_state_table[BSP_LED_STATE_MAX][BSP_LED_ACTION_MAX] = {
    /*BSP_LED_ACTION_OF_START, BSP_LED_ACTION_OF_TIMEOUT, BSP_LED_ACTION_OF_STOP,  BSP_LED_ACTION_OF_CALI_TIMEOUT*/
    /*BSP_LED_STATE_IDLE */     {BSP_LED_STATE_T0,         BSP_LED_STATE_IDLE,    BSP_LED_STATE_IDLE,          0xFFFFFFF8},
    /*BSP_LED_STATE_T0 */       {0xFFFFFFF5,               BSP_LED_STATE_T1T2_RPT, BSP_LED_STATE_IDLE,          0xFFFFFFFa},
    /*BSP_LED_STATE_T1T2_RPT */ {0xFFFFFFF6,               BSP_LED_STATE_T3,      BSP_LED_STATE_IDLE,          BSP_LED_STATE_T1T2_RPT},
    /*BSP_LED_STATE_T3 */       {0xFFFFFFF7,               BSP_LED_STATE_T1T2_RPT, BSP_LED_STATE_IDLE,          0xFFFFFFFc},
};


/* ////////////////////////////////////////Private function///////////////////////////////////////////// */
static  uint32_t    led_isink_get_time_sel(uint32_t time, int mul)
{
    uint32_t i   = 0;
    uint32_t tmp = 0;
    uint32_t res = 0;
    uint32_t map_sz = 0;

    map_sz = sizeof(g_led_isink_tm_tb) / sizeof(uint16_t);
    tmp = abs(time - g_led_isink_tm_tb[0] * mul);
    for (i = 1; i < map_sz; i++) {
        if (tmp < (res = abs(time - g_led_isink_tm_tb[i] * mul))) {
            return i - 1;
        }
        tmp = res;
    }
    return i - 1;
}

static  void        isink_hw_config(uint8_t ledx, bsp_isink_private_info_t *pinfo, uint32_t coef)
{
#define     BSP_LED_BRIGHTNESS_SCALE    10

    hal_isink_config_t           isink_cfg;

    if (pinfo == NULL) {
        log_bsp_led_info("[bsp][led][isink] led%d hw config: para is null", 1, ledx);
        return;
    }
    hal_isink_init(ledx);
    if (pinfo->mode == BSP_LED_BLINK_MODE) {
        uint32_t hi_tm_ms = 0;
        uint32_t lo_tm_ms = 0;

        /* if led is off mode, not config isink */
        if (BSP_LED_T1_TO_MS(ledx) == 0) {
            log_bsp_led_info("[bsp][led][isink] led%d blink: always off", 1, ledx);
            return;
        } else if (BSP_LED_T2_TO_MS(ledx) == 0) {
            log_bsp_led_info("[bsp][led][isink] led%d blink: always on", 1, ledx);
#ifdef BSP_LED_SET_ALWAYSON_TO_REG_MODE
            isink_cfg.mode = HAL_ISINK_MODE_REGISTER;
            isink_cfg.config.register_mode.current       = pinfo->led_cfg.brightness / 51;
            isink_cfg.config.register_mode.enable_double = false;
#else
            uint32_t temp = 0;

            temp = (pinfo->led_cfg.brightness * BSP_LED_BRIGHTNESS_SCALE) / 255;
            temp = (temp == 0) ? 1 : temp;
            isink_cfg.mode = HAL_ISINK_MODE_PWM;
            isink_cfg.config.pwm_mode.hi_level_time = temp;
            isink_cfg.config.pwm_mode.lo_level_time = BSP_LED_BRIGHTNESS_SCALE - temp;
            isink_cfg.config.pwm_mode.blink_nums    = 0;
            isink_cfg.config.pwm_mode.idle_time     = 0;
#endif
        } else {
            hi_tm_ms = BSP_LED_T1_TO_MS(ledx);
            lo_tm_ms = BSP_LED_T2_TO_MS(ledx);
            pinfo->t1t2_cycle = hi_tm_ms + lo_tm_ms;

            isink_cfg.mode = HAL_ISINK_MODE_PWM;
            isink_cfg.config.pwm_mode.hi_level_time = hi_tm_ms;
            isink_cfg.config.pwm_mode.lo_level_time = lo_tm_ms;
            isink_cfg.config.pwm_mode.blink_nums    = 0;
            isink_cfg.config.pwm_mode.idle_time     = 0;
            log_bsp_led_info("[bsp][led][isink] led%d blink: t1t2 cycle %dms(t1 %d + t2 %d)\r\n", 4,
                             ledx,
                             pinfo->t1t2_cycle,
                             hi_tm_ms,
                             lo_tm_ms);
        }
    } else if (pinfo->mode == BSP_LED_BREATH_MODE) {
        uint32_t                ton, toff;
        if (BSP_LED_T1_TO_MS(ledx) == 0) {
            log_bsp_led_info("[bsp][led][isink] led%d breath: always off", 1, ledx);
            return;
        } else if (BSP_LED_T2_TO_MS(ledx) == 0) {
            log_bsp_led_info("[bsp][led][isink] led%d breath: always on", 1, ledx);
#ifdef BSP_LED_SET_ALWAYSON_TO_REG_MODE
            isink_cfg.mode = HAL_ISINK_MODE_REGISTER;
            isink_cfg.config.register_mode.current       = pinfo->led_cfg.brightness / 51;
            isink_cfg.config.register_mode.enable_double = false;
#else
            uint32_t temp = 0;

            temp = (pinfo->led_cfg.brightness * BSP_LED_BRIGHTNESS_SCALE) / 255;
            temp = (temp == 0) ? 1 : temp;
            isink_cfg.mode = HAL_ISINK_MODE_PWM;
            isink_cfg.config.pwm_mode.hi_level_time = temp;
            isink_cfg.config.pwm_mode.lo_level_time = BSP_LED_BRIGHTNESS_SCALE - temp;
            isink_cfg.config.pwm_mode.blink_nums    = 0;
            isink_cfg.config.pwm_mode.idle_time     = 0;
#endif

        } else {
            ton  = (BSP_LED_T1_TO_MS(ledx) > 246) ? (BSP_LED_T1_TO_MS(ledx) - 246) : 0;
            toff = (BSP_LED_T2_TO_MS(ledx) > 246) ? (BSP_LED_T2_TO_MS(ledx) - 246) : 0;

            isink_cfg.mode = HAL_ISINK_MODE_BREATH;
            isink_cfg.config.breath_mode.darker_to_lighter_time1 = 0;
            isink_cfg.config.breath_mode.darker_to_lighter_time2 = 0;
            isink_cfg.config.breath_mode.lighter_to_darker_time1 = 0;
            isink_cfg.config.breath_mode.lighter_to_darker_time2 = 0;
            isink_cfg.config.breath_mode.lightest_time           = led_isink_get_time_sel(ton,  1);
            isink_cfg.config.breath_mode.darkest_time            = led_isink_get_time_sel(toff, 2);

            pinfo->t1t2_cycle =     g_led_isink_tm_tb[isink_cfg.config.breath_mode.darker_to_lighter_time1] + \
                                    g_led_isink_tm_tb[isink_cfg.config.breath_mode.darker_to_lighter_time2] + \
                                    g_led_isink_tm_tb[isink_cfg.config.breath_mode.lightest_time] * 1 +         \
                                    g_led_isink_tm_tb[isink_cfg.config.breath_mode.lighter_to_darker_time1] + \
                                    g_led_isink_tm_tb[isink_cfg.config.breath_mode.lighter_to_darker_time2] + \
                                    g_led_isink_tm_tb[isink_cfg.config.breath_mode.darkest_time] * 2;
            log_bsp_led_warn("[bsp][led][isink] led%d breath: t1t2 cycle %dms\r\n", 2, ledx, pinfo->t1t2_cycle);
        }
    }
    hal_isink_configure(ledx , &isink_cfg);
}

static  void        bsp_led_gpt_callback(void *args)
{
    bsp_isink_private_info_t  *pinfo = (bsp_isink_private_info_t *)args;
    //log_bsp_led_warn("[bsp][led][isink] led%d sw timer timeout\r\n", 1, pinfo->led_id);
    bsp_led_process(pinfo->led_id, pinfo->next_action);
}

#ifdef BSP_LED_SW_CALI_FEATURE
/* led_sw_cali_coefficient = 100, means no need SW Cali.
    when 0< Cali<99, means need cali,
    there are two point to do led_sw_cali_coefficient calculate:
        1. The first time to start, just to to cali init and cali the timer of syn_tm_ms.
        2. When start T0, means a new loop, to cali next total loop*/
static  void        bsp_led_sw_cali_init(bsp_led_channel_t channel, uint32_t dly_sta_tm)
{
    bsp_isink_private_info_t    *pinfo = &g_led_priv_info[channel];
    uint32_t temp_32K_tick;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &temp_32K_tick);
    pinfo->sw_cali.expected_expire_tick    = temp_32K_tick + BSP_MS_TO_TICK(dly_sta_tm);// next expected tick when start T0
    pinfo->sw_cali.led_sw_cali_coefficient = 100;

}
static  void        bsp_led_sw_cali(bsp_led_channel_t channel, uint32_t tw_cyc)
{
    bsp_isink_private_info_t    *pinfo = &g_led_priv_info[channel];
    uint32_t temp_32K_tick;
    uint32_t ext_loop_tick;
    int      temp_diff_32K_tick;

    ext_loop_tick = (uint32_t)BSP_MS_TO_TICK(tw_cyc);
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,  &temp_32K_tick);
    temp_diff_32K_tick = temp_32K_tick - pinfo->sw_cali.expected_expire_tick;
    pinfo->sw_cali.led_sw_cali_coefficient = 100;
    if (temp_diff_32K_tick > 0) {
        if (temp_diff_32K_tick <  ext_loop_tick) {
            pinfo->sw_cali.led_sw_cali_coefficient = ((ext_loop_tick - temp_diff_32K_tick) * 100.0f) / ext_loop_tick;
        }
    }
    if (pinfo->sw_cali.led_sw_cali_coefficient < 90) {
        pinfo->sw_cali.led_sw_cali_coefficient = 90;
    }
    log_bsp_led_info("[bsp][led][isink] sw cali: coef %d, timer delay %d = curr_tick(%d)-(expect_tick)%d, next tick %d", 5,
                     (uint32_t)pinfo->sw_cali.led_sw_cali_coefficient,
                     (int32_t)temp_diff_32K_tick,
                     (uint32_t)temp_32K_tick,
                     (uint32_t)pinfo->sw_cali.expected_expire_tick,
                     (uint32_t)ext_loop_tick
                    );
    pinfo->sw_cali.expected_expire_tick += ext_loop_tick;
}
#endif

void  bsp_led_process(bsp_led_channel_t channel, bsp_led_action_t action)
{
    uint32_t    start_timer_continue = 0xFFFF;
    uint32_t    temp_timer_ms = 0;
    bsp_isink_private_info_t  *pinfo = &g_led_priv_info[channel];


    //bsp_led_print_cfg(pinfo->led_id ,&(pinfo->led_cfg));
    pinfo->cur_state = g_led_state_table[pinfo->cur_state][action];
    //log_bsp_led_info("[bsp][led][isink] led process:led %x, cur_sta:%d, next_action:%d\r\n", 3, channel, pinfo->cur_state, action);
    switch (pinfo->cur_state) {
        case BSP_LED_STATE_IDLE: {
            log_bsp_led_info("[bsp][led][isink] led process: will in idle\r\n" , 0);
            hal_isink_stop(channel);
            start_timer_continue = 0xFFFFFFFF;
            pinfo->next_action = BSP_LED_ACTION_MAX;
        }
        break;

        case BSP_LED_STATE_T0: {
            log_bsp_led_info("[bsp][led][isink] led process: start t0\r\n" , 0);
            start_timer_continue = BSP_LED_T0_TO_MS(channel) + pinfo->led_cfg.sync_time;
            pinfo->next_action = BSP_LED_ACTION_OF_TIMEOUT;
#ifdef BSP_LED_SW_CALI_FEATURE
            bsp_led_sw_cali_init(channel, BSP_LED_T0_TO_MS(channel) + pinfo->led_cfg.sync_time);
#endif
        }
        break;

        case BSP_LED_STATE_T1T2_RPT: {
            log_bsp_led_info("[bsp][led][isink] led process: start t1t2 repeat\r\n", 0);
            start_timer_continue = 0xFFFFFFFF;
            if (BSP_LED_T2_TO_MS(channel) == 0) {
                log_bsp_led_info("[bsp][led][isink] led process: always on\r\n" , 0);
            } else if (pinfo->led_cfg.repeat_t1t2 == 0xFFFF) {
#ifdef BSP_LED_SW_CALI_FEATURE
                start_timer_continue = ((BSP_T1_T2_REPEAT_CALI_TM / pinfo->t1t2_cycle) + 1) * pinfo->t1t2_cycle;
                bsp_led_sw_cali(channel, start_timer_continue);
                pinfo->next_action   = BSP_LED_ACTION_OF_CALI_TIMEOUT;
                hal_isink_stop(channel);
#endif
                log_bsp_led_info("[bsp][led][isink] led process: t1t2 always repeat\r\n" , 0);
                hal_isink_start(channel);
            } else {
#ifdef BSP_LED_SW_CALI_FEATURE
                //isink_hw_config(pinfo->led_id, &pinfo->led_cfg, pinfo->sw_cali.led_sw_cali_coefficient);
                bsp_led_sw_cali(channel, BSP_LED_EXT_LOOP_MS(channel));
#endif
                start_timer_continue = BSP_LED_T1_T2_WITH_REPEAT_TO_MS(channel);
                pinfo->next_action = BSP_LED_ACTION_OF_TIMEOUT;
                /* process ext loop */
                if (pinfo->led_cfg.repeat_ext != 0xFFFF) {
                    pinfo->led_cfg.repeat_ext--;
                    if (pinfo->led_cfg.repeat_ext == 0) {
                        bsp_led_process(channel, BSP_LED_ACTION_OF_STOP);
                        log_bsp_led_info("[bsp][led][isink] led process: end ext loop\r\n" , 0);
                        if (pinfo->led_cfg.call_back != NULL) {
                            pinfo->led_cfg.call_back(pinfo->led_id, pinfo->led_cfg.user_data);
                        }
                        return;
                    }
                }
            }
            hal_isink_start(channel);
        }
        break;

        case BSP_LED_STATE_T3: {
            log_bsp_led_info("[bsp][led][isink] led process: start t3\r\n" , 0);
            hal_isink_stop(channel);
            start_timer_continue = BSP_LED_T3_TO_MS(channel);
            pinfo->next_action = BSP_LED_ACTION_OF_TIMEOUT;
        }
        break;

        default: {
            hal_isink_stop(channel);
            start_timer_continue = 0xFFFFFFFF;
            log_bsp_led_info("[bsp][led][isink] led process: error state %d", 1, pinfo->cur_state);
        }
        break;
    }
    /* whether start or stop timer */
    if (start_timer_continue != 0xFFFFFFFF) {
        hal_gpt_status_t    gpt_sta;
#ifdef BSP_LED_SW_CALI_FEATURE
        if (pinfo->sw_cali.led_sw_cali_coefficient != 0) {
            temp_timer_ms = (start_timer_continue * pinfo->sw_cali.led_sw_cali_coefficient) / 100;
        }
#else
        temp_timer_ms = start_timer_continue;
#endif
        log_bsp_led_info("[bsp][led][isink] led process: start timer(%x) %d ms, after cali %d ms", 3,
                         (uint32_t)pinfo->gpt_handle,
                         (uint32_t)start_timer_continue,
                         (uint32_t)temp_timer_ms);
        gpt_sta = hal_gpt_sw_start_timer_ms(pinfo->gpt_handle, temp_timer_ms, bsp_led_gpt_callback, (void *)pinfo);
        if (HAL_GPT_STATUS_OK != gpt_sta) {
            log_bsp_led_error("[bsp][led][isink] led process: start sw timer(%x) fail(%d)", 2, pinfo->gpt_handle, gpt_sta);
        }
    }

    else {
        hal_gpt_status_t    gpt_sta;

        gpt_sta = hal_gpt_sw_stop_timer_ms(pinfo->gpt_handle);
        if (HAL_GPT_STATUS_OK != gpt_sta) {
            log_bsp_led_error("[bsp][led][isink] led process: stop sw timer fail(%d)", 1, gpt_sta);
        }
    }
}


/////////////////////////////////////////public function//////////////////////////////////////////////
bsp_led_status_t    bsp_led_isink_init(uint8_t ledx, bsp_led_mode_t mode)
{
    static bool                 initialized = false;
    bsp_isink_private_info_t    *pinfo = &g_led_priv_info[ledx];

    log_bsp_led_info("[bsp][led][isink] led%d init", 1, ledx);
    /* initial global var */
    if (initialized == false) {
        memset(g_led_priv_info, 0, sizeof(g_led_priv_info));
        initialized = true;
    }
    /* check para */
    if (ledx >= BSP_LED_CHANNEL_MAX) {
        log_bsp_led_error("[bsp][led][isink] led %d init fail, chnl err\r\n ", 1, ledx);
        return BSP_LED_STATUS_ERROR_CHANNEL;
    }
    /* initial global var */
    pinfo->led_id = ledx;
    pinfo->sw_cali.led_sw_cali_coefficient = 100;
    pinfo->mode        = mode;
    pinfo->cur_state   = 0;
    pinfo->next_action = 0;

    if (pinfo->gpt_handle == 0) {
        if (HAL_GPT_STATUS_OK != hal_gpt_sw_get_timer(&pinfo->gpt_handle)) {
            log_bsp_led_error("[bsp][led][isink] led%d init fail, get sw timer failed.\r\n", 1, ledx);
            return BSP_LED_STATUS_ERROR;
        }
        log_bsp_led_info("[bsp][led][isink] led%d init: get sw gpt hdl(0x%x)", 2, ledx, pinfo->gpt_handle);
    }
    hal_isink_init(ledx);
    return BSP_LED_STATUS_OK;
}


bsp_led_status_t    bsp_led_isink_deinit(uint8_t ledx)
{
    bsp_isink_private_info_t    *pinfo = NULL;

    log_bsp_led_info("[bsp][led][isink] led%d deinit", 1, ledx);
    if (ledx >= BSP_LED_CHANNEL_MAX) {
        log_bsp_led_error("[bsp][led][isink] led%d deinit fail, chnl err\r\n ", 1, ledx);
        return BSP_LED_STATUS_ERROR_CHANNEL;
    }
    pinfo = &g_led_priv_info[ledx];
    pinfo->led_id      = ledx;
    pinfo->cur_state   = 0;
    pinfo->next_action = 0;
    //hal_gpt_sw_free_timer(pinfo->gpt_handle);
    //pinfo->gpt_handle = 0;
    hal_gpt_sw_stop_timer_ms(pinfo->gpt_handle);
    hal_isink_deinit(ledx);
    return BSP_LED_STATUS_OK;
}


bsp_led_status_t    bsp_led_isink_config(uint8_t ledx, bsp_led_config_t *cfg)
{
    bsp_isink_private_info_t    *pinfo =  &g_led_priv_info[ledx];
    log_bsp_led_info("[bsp][led][isink] led%d config(sync %d)\r\n", 2, ledx, cfg->sync_time);
    if (ledx >= BSP_LED_CHANNEL_MAX) {
        log_bsp_led_error("[bsp][led][isink] led%d config fail, chnl error\r\n ", 1, ledx);
        return BSP_LED_STATUS_ERROR_CHANNEL;
    }
    if (cfg == NULL) {
        log_bsp_led_error("[bsp][led][isink] led%d config fail, para is null\r\n ", 1, ledx);
        return BSP_LED_STATUS_ERROR_INVALID_PARAMETER;
    }
    pinfo->led_cfg = *cfg;
    if (BSP_LED_T1_TO_MS(ledx) == 0) {
        log_bsp_led_error("[bsp][led][isink] led %d config fail, t1 == 0\r\n ", 1, ledx);
        return BSP_LED_STATUS_OK;
    }
    if (pinfo->led_cfg.repeat_t1t2 == 0) {
        pinfo->led_cfg.repeat_t1t2 = 0xFFFF;
    }
    if (pinfo->led_cfg.repeat_ext == 0) {
        pinfo->led_cfg.repeat_ext  = 0xFFFF;
    }
    isink_hw_config(ledx, pinfo, 100);
    return BSP_LED_STATUS_OK;
}

bsp_led_status_t    bsp_led_isink_start(uint8_t ledx)
{
    bsp_isink_private_info_t    *pinfo =  &g_led_priv_info[ledx];

    log_bsp_led_info("[bsp][led][isink] led%d isink start\r\n", 1, ledx);
    if (ledx >= BSP_LED_CHANNEL_MAX) {
        log_bsp_led_error("[bsp][led][isink] led%d start fail, chnl err\r\n", 1, ledx);
        return BSP_LED_STATUS_ERROR_CHANNEL;
    }
    if (BSP_LED_T1_TO_MS(ledx) == 0) {
        log_bsp_led_error("[bsp][led][isink] led%d no start, t1 == 0\r\n ", 1, ledx);
        return BSP_LED_STATUS_OK;
    }
    pinfo->cur_state = BSP_LED_STATE_IDLE;
    bsp_led_process(ledx, BSP_LED_ACTION_OF_START);
    return BSP_LED_STATUS_OK;
}

bsp_led_status_t    bsp_led_isink_stop(uint8_t ledx)
{
    log_bsp_led_info("[bsp][led][isink] led%d stop\r\n", 1, ledx);
    if (ledx >= BSP_LED_CHANNEL_MAX) {
        log_bsp_led_error("[bsp][led][isink] led%d stop fail, chnl err\r\n", 1, ledx);
        return BSP_LED_STATUS_ERROR_CHANNEL;
    }
    if (BSP_LED_T1_TO_MS(ledx) == 0) {
        log_bsp_led_error("[bsp][led][isink] led%d no stop, t1 == 0\r\n ", 1, ledx);
        return BSP_LED_STATUS_OK;
    }
    bsp_led_process(ledx, BSP_LED_ACTION_OF_STOP);
    return BSP_LED_STATUS_OK;
}

int                 bsp_led_isink_ioctl(uint32_t ledx, uint32_t cmd, uint32_t option)
{
    return 0;
}
#endif
