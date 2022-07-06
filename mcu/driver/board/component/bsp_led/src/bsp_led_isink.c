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
#include "hal_isink.h"
#include "hal_rtc.h"
#include "bsp_led_isink.h"

#ifdef HAL_ISINK_MODULE_ENABLED

#ifdef HAL_ISINK_FEATURE_HW_PMIC2562
    #define BSP_LED_ISINK_2562
#if defined(AB1565)
	#define BSP_LED_FOR_ADIE32K_VARIATION
#endif
#else
#ifdef HAL_ISINK_FEATURE_ADVANCED_CONFIG
    #define BSP_LED_ISINK_NEW_API
#else
    #define BSP_LED_ISINK_OLD_API
#endif
#endif

//#define BSP_LED_SET_ALWAYSON_TO_REG_MODE
#define BSP_LED_SW_CALI_FEATURE


/////////////////////////////////////////Macro Defined//////////////////////////////////////////////
#define BSP_LED_OP_STATE_IDLE   0
#define BSP_LED_OP_STATE_ACTIVE 1

#define BSP_T1_T2_REPEAT_CALI_TM   0/* unit:ms  */

#define BSP_MS_TO_TICK(tick)      bsp_led_ms_to_32k_tick(tick)
#define BSP_LED_T0_TO_MS(channel) (g_led_priv_info[channel].led_cfg.t0 * g_led_priv_info[channel].led_cfg.time_unit)
#define BSP_LED_T1_TO_MS(channel) (g_led_priv_info[channel].led_cfg.t1 * g_led_priv_info[channel].led_cfg.time_unit)
#define BSP_LED_T2_TO_MS(channel) (g_led_priv_info[channel].led_cfg.t2 * g_led_priv_info[channel].led_cfg.time_unit)
#define BSP_LED_T3_TO_MS(channel) (g_led_priv_info[channel].led_cfg.t3 * g_led_priv_info[channel].led_cfg.time_unit)
#define BSP_LED_T1_T2_WITH_REPEAT_TO_MS(channel) (g_led_priv_info[channel].t1t2_cycle * g_led_priv_info[channel].led_cfg.repeat_t1t2)
#define BSP_LED_EXT_LOOP_MS(channel) (BSP_LED_T1_T2_WITH_REPEAT_TO_MS(channel) +BSP_LED_T3_TO_MS(channel) )

/////////////////////////////////////////Function Declare//////////////////////////////////////////////
extern void                 bsp_led_process(bsp_led_channel_t channel, bsp_led_action_t action);
extern hal_isink_status_t   hal_isink_enable_pwm_mode(uint32_t channel, uint32_t cycle_ms, uint32_t duty_persent);
#ifdef BSP_LED_FOR_ADIE32K_VARIATION
extern void  bsp_led_process_ex(bsp_led_channel_t channel, uint8_t state);
#endif
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

uint32_t bsp_led_ms_to_32k_tick(uint32_t ms)
{
    uint32_t tick_per_second = 32768;

    hal_rtc_get_f32k_frequency(&tick_per_second);
    return (uint32_t)(ms * (tick_per_second / 1000.0f));
}


#if defined(AB1565)

#else
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
#endif

#ifdef BSP_LED_ISINK_2562
static  void   isink_hw_config(uint8_t ledx, bsp_isink_private_info_t *pinfo, uint32_t coef)
{
    hal_isink_config_ext_t  isink_cfg;
    uint32_t t1,t2;

    if (pinfo == NULL) {
        log_bsp_led_info("[bsp][led][isink] led%d hw config: para is null", 1, ledx);
        return;
    }
    memset(&isink_cfg, 0, sizeof(isink_cfg));

    t1 = BSP_LED_T1_TO_MS(ledx);
    t2 = BSP_LED_T2_TO_MS(ledx);
    pinfo->t1t2_cycle = t1 + t2;

    /* init isink led configure */
    isink_cfg.timing.t0 = 0;
    isink_cfg.timing.t3 = 0;
    isink_cfg.brightness= pinfo->led_cfg.brightness;
    if (pinfo->mode == BSP_LED_BLINK_MODE) {
        isink_cfg.timing.t1.t_rising   = 0;
        isink_cfg.timing.t1.t_lightest = t1;
        isink_cfg.timing.t2.t_falling  = 0;
        isink_cfg.timing.t2.t_darkest  = t2;
    } else if(pinfo->mode == BSP_LED_BREATH_MODE) {
        isink_cfg.timing.t1.t_rising   = t1 >> 1;
        isink_cfg.timing.t1.t_lightest = t1 - isink_cfg.timing.t1.t_rising;
        isink_cfg.timing.t2.t_falling  = t2 >> 1;
        isink_cfg.timing.t2.t_darkest  = t2 - isink_cfg.timing.t2.t_falling;
    }
    if(t1 == 0 || t2 == 0) {
        isink_cfg.blink_nums = 0;
    }
#ifdef BSP_LED_SW_CALI_FEATURE
    else {
        isink_cfg.blink_nums = (BSP_T1_T2_REPEAT_CALI_TM/(pinfo->t1t2_cycle)) + 1;
    }
#endif
    hal_isink_init(ledx);
    hal_isink_configure_ext(ledx, &isink_cfg);
}
#endif



#ifdef BSP_LED_ISINK_NEW_API
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

        /* blink mode: always off */
        if (BSP_LED_T1_TO_MS(ledx) == 0) {
            log_bsp_led_info("[bsp][led][isink] led%d blink: always off", 1, ledx);
            return;
        }
        /* blink mode: always on */
        else if (BSP_LED_T2_TO_MS(ledx) == 0) {
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
        }
        /* blink mode: twinkle */
        else {
            hi_tm_ms = BSP_LED_T1_TO_MS(ledx);
            lo_tm_ms = BSP_LED_T2_TO_MS(ledx);
            pinfo->t1t2_cycle = hi_tm_ms + lo_tm_ms;

            isink_cfg.mode = HAL_ISINK_MODE_PWM;
            isink_cfg.config.pwm_mode.hi_level_time = hi_tm_ms;
            isink_cfg.config.pwm_mode.lo_level_time = lo_tm_ms;
            isink_cfg.config.pwm_mode.blink_nums    = pinfo->led_cfg.repeat_t1t2;
            isink_cfg.config.pwm_mode.idle_time     = 0;
            log_bsp_led_info("[bsp][led][isink] led%d blink: t1t2 cycle %dms(t1 %d + t2 %d)\r\n", 4,
                             ledx,
                             pinfo->t1t2_cycle,
                             hi_tm_ms,
                             lo_tm_ms);
        }
    }
    /* bsp led breath mode */
    else if (pinfo->mode == BSP_LED_BREATH_MODE) {
         /* breath mode: always off*/
        if (BSP_LED_T1_TO_MS(ledx) == 0) {
            log_bsp_led_info("[bsp][led][isink] led%d breath: always off", 1, ledx);
            return;
        }
        /* breath mode: always off */
        else if (BSP_LED_T2_TO_MS(ledx) == 0) {
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

        }
        /* breath mode: twinkle */
        else {

#if defined(AB1565)
            isink_cfg.mode = HAL_ISINK_MODE_BREATH;
            isink_cfg.config.pwm_mode.hi_level_time = pinfo->led_cfg.t1 * pinfo->led_cfg.time_unit;
            isink_cfg.config.pwm_mode.lo_level_time = pinfo->led_cfg.t2 * pinfo->led_cfg.time_unit;
            isink_cfg.config.pwm_mode.blink_nums    = pinfo->led_cfg.repeat_t1t2;
            isink_cfg.config.pwm_mode.idle_time     = 0;
            pinfo->t1t2_cycle = isink_cfg.config.pwm_mode.hi_level_time + isink_cfg.config.pwm_mode.lo_level_time;
#else
            uint32_t  ton, toff;

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
#endif
            log_bsp_led_warn("[bsp][led][isink] led%d breath: t1t2 cycle %dms\r\n", 2, ledx, pinfo->t1t2_cycle);
        }
    }
    hal_isink_configure(ledx , &isink_cfg);
}
#endif


#ifdef  BSP_LED_ISINK_OLD_API
static  void        isink_hw_config(bsp_led_channel_t ledx, bsp_isink_private_info_t  *pinfo, uint32_t coef)
{
    if (pinfo == NULL) {
        log_bsp_led_info("[bsp][led][isink] led%d hw config: para is null", 1, ledx);
        return;
    }
    pinfo->req_cali = false;
    if(pinfo->mode == BSP_LED_BLINK_MODE){
        uint32_t freq = 0;
        uint32_t duty = 0;

        /*if led is off mode, not config isink*/
        if(BSP_LED_T1_TO_MS(ledx) == 0){
            log_bsp_led_warn("[bsp][led][isink] led%d blink: alway off\r\n", 1, ledx);
            return;
        } else if(BSP_LED_T2_TO_MS(ledx) == 0){
            freq = 1;
            duty = (pinfo->led_cfg.brightness * 100)/255;
            log_bsp_led_warn("[bsp][led][isink] led%d blink: always on\r\n", 1, ledx);
        } else {
            freq = (BSP_LED_T1_TO_MS(ledx) + BSP_LED_T2_TO_MS(ledx));
            duty = (BSP_LED_T1_TO_MS(ledx) * 100)/(BSP_LED_T1_TO_MS(ledx) + BSP_LED_T2_TO_MS(ledx));
            pinfo->req_cali   = true;
            pinfo->t1t2_cycle = freq;
        }
        hal_isink_set_mode(ledx, HAL_ISINK_MODE_PWM);
        hal_isink_enable_pwm_mode(ledx, freq, duty);
        log_bsp_led_info("[bsp][led][isink] led%d blink: t1t2 cycle %dms, duty %d\r\n", 3, ledx, freq, duty);
    } else {
        hal_isink_breath_mode_t breath_mode;
        uint32_t                ton,toff;

        if(BSP_LED_T1_TO_MS(ledx) == 0){
            log_bsp_led_warn("[bsp][led][isink] led%d breath: alway off\r\n", 1, ledx);
            return;
        } else if(BSP_LED_T2_TO_MS(ledx) == 0){
            uint32_t freq = 0;
            uint32_t duty = 0;
            freq = 1;
            duty = (pinfo->led_cfg.brightness * 100)/255;
            log_bsp_led_warn("[bsp][led][isink] led%d breath: always on(duty %d)\r\n", 2, ledx, duty);
            hal_isink_set_mode(ledx, HAL_ISINK_MODE_PWM);
            hal_isink_enable_pwm_mode(ledx, freq, duty);
        }else {
            ton  = (BSP_LED_T1_TO_MS(ledx) > 246)?(BSP_LED_T1_TO_MS(ledx) - 246):0;
            toff = (BSP_LED_T2_TO_MS(ledx) > 246)?(BSP_LED_T2_TO_MS(ledx) - 246):0;
            ton  = (ton  * pinfo->sw_cali.led_sw_cali_coefficient)/100;
            toff = (toff * pinfo->sw_cali.led_sw_cali_coefficient)/100;
            breath_mode.darker_to_lighter_time1 = 0;
            breath_mode.darker_to_lighter_time2 = 0;
            breath_mode.lighter_to_darker_time1 = 0;
            breath_mode.lighter_to_darker_time2 = 0;
            breath_mode.lightest_time           = led_isink_get_time_sel(ton,  1);
            breath_mode.darkest_time            = led_isink_get_time_sel(toff, 2);

            hal_isink_set_mode(ledx, HAL_ISINK_MODE_BREATH);
            hal_isink_enable_breath_mode(ledx, breath_mode);
            pinfo->t1t2_cycle =     g_led_isink_tm_tb[breath_mode.darker_to_lighter_time1] + \
                                    g_led_isink_tm_tb[breath_mode.darker_to_lighter_time2] + \
                                    g_led_isink_tm_tb[breath_mode.lightest_time]*1 +         \
                                    g_led_isink_tm_tb[breath_mode.lighter_to_darker_time1] + \
                                    g_led_isink_tm_tb[breath_mode.lighter_to_darker_time2] + \
                                    g_led_isink_tm_tb[breath_mode.darkest_time]*2;
            log_bsp_led_warn("[bsp][led][isink] led%d breath: t1t2 cycle %dms\r\n", 2, ledx, pinfo->t1t2_cycle);
        }
    }
}
#endif


/***********************************************************************************************
 * Hardware-independent functions
***********************************************************************************************/

static  void        bsp_led_gpt_callback(void *args)
{
    bsp_isink_private_info_t  *pinfo = (bsp_isink_private_info_t *)args;
    //log_bsp_led_warn("[bsp][led][isink] led%d sw timer timeout\r\n", 1, pinfo->led_id);
#ifdef BSP_LED_FOR_ADIE32K_VARIATION
    bsp_led_process_ex(pinfo->led_id, pinfo->next_action);
#else
    bsp_led_process(pinfo->led_id, pinfo->next_action);
#endif
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
#if 0
    log_bsp_led_info("[bsp][led][isink] sw cali: coef %d, timer delay %d = curr_tick(%d)-(expect_tick)%d, next tick %d", 5,
                     (uint32_t)pinfo->sw_cali.led_sw_cali_coefficient,
                     (int32_t)temp_diff_32K_tick,
                     (uint32_t)temp_32K_tick,
                     (uint32_t)pinfo->sw_cali.expected_expire_tick,
                     (uint32_t)ext_loop_tick
                    );
#endif
    pinfo->sw_cali.expected_expire_tick += ext_loop_tick;
}
#endif

#ifdef BSP_LED_FOR_ADIE32K_VARIATION
void  bsp_led_process_ex(bsp_led_channel_t channel, uint8_t state)
{
    uint32_t            sw_timeout_ms = 0xFFFFFFFF;
    static  uint32_t    num_t1t2_rpt[HAL_ISINK_MAX_CHANNEL] = {0, 0};
    static  uint32_t    num_ext_loop[HAL_ISINK_MAX_CHANNEL] = {0, 0};
    bsp_isink_private_info_t  *pinfo = &g_led_priv_info[channel];
    hal_gpt_status_t           gpt_sta;
    //bsp_led_config_t           led_cfg;
    uint32_t                   t1,t2;
    hal_isink_config_ext_t     isink_cfg;

    t1 = pinfo->led_cfg.t1 * pinfo->led_cfg.time_unit;
    t2 = pinfo->led_cfg.t2 * pinfo->led_cfg.time_unit;
    switch(state)
    {
        case BSP_LED_STATE_IDLE:{
                hal_isink_stop(channel);
                sw_timeout_ms = 0xFFFFFFFF;
                pinfo->next_action = BSP_LED_STATE_IDLE;
            }break;
        case BSP_LED_STATE_TSYNC: {
                sw_timeout_ms         = pinfo->led_cfg.sync_time;
                pinfo->next_action    = BSP_LED_STATE_T0;
                num_ext_loop[channel] = pinfo->led_cfg.repeat_ext;
                hal_isink_stop(channel);
#ifdef BSP_LED_SW_CALI_FEATURE
                bsp_led_sw_cali_init(channel, pinfo->led_cfg.sync_time);
#endif
                //log_bsp_led_info("[bsp][led][isink] led%d : start Tsync (%dms)",2, channel, sw_timeout_ms);
            }break;
        case BSP_LED_STATE_T0: {
                sw_timeout_ms         = BSP_LED_T0_TO_MS(channel);
                pinfo->next_action    = BSP_LED_STATE_T1;
                num_t1t2_rpt[channel] = pinfo->led_cfg.repeat_t1t2;
                hal_isink_stop(channel);
#ifdef BSP_LED_SW_CALI_FEATURE
                bsp_led_sw_cali(channel, sw_timeout_ms);
#endif
                //log_bsp_led_info("[bsp][led][isink] led%d : start T0 (%dms)",2, channel, sw_timeout_ms);
            }break;
        case BSP_LED_STATE_T1: {
                pinfo->next_action = BSP_LED_STATE_T2;
                sw_timeout_ms = t1;
                //log_bsp_led_info("[bsp][led][isink] led%d : start T1 (%dms), mode(%d), brightness(%d)", 4, channel, sw_timeout_ms, pinfo->mode, pinfo->led_cfg.brightness);

                /*config led timing */
                isink_cfg.blink_nums = 0;
                isink_cfg.brightness = pinfo->led_cfg.brightness;
                isink_cfg.timing.t0  = 0;
                isink_cfg.timing.t3  = pinfo->led_cfg.t3 * pinfo->led_cfg.time_unit;
                if(pinfo->mode == BSP_LED_BLINK_MODE){
                    isink_cfg.timing.t1.t_rising  = 0;
                    isink_cfg.timing.t1.t_lightest= t1;

                    isink_cfg.timing.t2.t_falling = 0;
                    isink_cfg.timing.t2.t_darkest = 0;
                } else {
                    isink_cfg.timing.t1.t_rising  = (t1 * 3) / 5;
                    isink_cfg.timing.t1.t_lightest= t1 - isink_cfg.timing.t1.t_rising;

                    isink_cfg.timing.t2.t_falling = 0;
                    isink_cfg.timing.t2.t_darkest = 0;
                }
                hal_isink_stop(channel);
                hal_isink_configure_ext(channel, &isink_cfg);
                hal_isink_start(channel);
#ifdef BSP_LED_SW_CALI_FEATURE
                bsp_led_sw_cali(channel, sw_timeout_ms);
#endif
            }break;
        case BSP_LED_STATE_T2: {
                sw_timeout_ms = BSP_LED_T2_TO_MS(channel);
                if (pinfo->led_cfg.repeat_t1t2 == 0) {
                    pinfo->next_action = BSP_LED_STATE_T1;
                } else {
                    num_t1t2_rpt[channel]--;
                    if(num_t1t2_rpt[channel] != 0) {
                        pinfo->next_action = BSP_LED_STATE_T1;
                    } else {
                        pinfo->next_action = BSP_LED_STATE_T3;
                    }
                }
                if(t2 == 0) {
                    sw_timeout_ms = 0xFFFFFFFF;
                    log_bsp_led_warn("[bsp][led][isink] led%d : always on", 1, channel);
                } else if(pinfo->mode == BSP_LED_BLINK_MODE){
                    hal_isink_stop(channel);
                } else {
                    isink_cfg.blink_nums = 1;
                    isink_cfg.brightness = pinfo->led_cfg.brightness;
                    isink_cfg.timing.t0  = 0;
                    isink_cfg.timing.t3  = pinfo->led_cfg.t3 * pinfo->led_cfg.time_unit;

                    isink_cfg.timing.t1.t_rising  = 0;
                    isink_cfg.timing.t1.t_lightest= 1;

                    isink_cfg.timing.t2.t_falling = (t2 * 3) / 5;
                    isink_cfg.timing.t2.t_darkest = t2 - isink_cfg.timing.t2.t_falling;

                    hal_isink_stop(channel);
                    hal_isink_configure_ext(channel, &isink_cfg);
                    hal_isink_start(channel);
                    //hal_isink_dump(channel);
                }
#ifdef BSP_LED_SW_CALI_FEATURE
                bsp_led_sw_cali(channel, sw_timeout_ms);
#endif
                //log_bsp_led_info("[bsp][led][isink] led%d : start T2 (%dms), mode(%d), rptT1T2(%d)", 4, channel, sw_timeout_ms, pinfo->mode,  num_t1t2_rpt[channel]);
            }break;
        case BSP_LED_STATE_T3: {
                hal_isink_stop(channel);
                sw_timeout_ms      = BSP_LED_T3_TO_MS(channel);
                if (pinfo->led_cfg.repeat_ext == 0){
                    pinfo->next_action = BSP_LED_STATE_T0; /*always repeat ex loop */
                } else {
                    num_ext_loop[channel]--;
                    if(num_ext_loop[channel] != 0) {
                        pinfo->next_action = BSP_LED_STATE_T0;   /* wrap to T0 for next ext loop */
                    } else {
                        pinfo->next_action = BSP_LED_STATE_IDLE; /* wrap to T0 for next ext loop */
                    }
                }
#ifdef BSP_LED_SW_CALI_FEATURE
                bsp_led_sw_cali(channel, sw_timeout_ms);
#endif
                //log_bsp_led_info("[bsp][led][isink] led%d : start T3 (%dms), ext_rpt(%d)",3, channel, sw_timeout_ms, num_ext_loop[channel]);
            }break;
        default: {
            }break;
    }

    if(sw_timeout_ms != 0xFFFFFFFF) {
        sw_timeout_ms = (uint32_t)((sw_timeout_ms * (double)pinfo->sw_cali.led_sw_cali_coefficient) / (double)100.0f);
        log_bsp_led_info("[bsp][led][isink] led%d : state in %d, sw cali time(%d), coef(%d)", 4, channel, state, sw_timeout_ms, (int) pinfo->sw_cali.led_sw_cali_coefficient);
        gpt_sta = hal_gpt_sw_start_timer_ms(pinfo->gpt_handle, sw_timeout_ms, bsp_led_gpt_callback, (void *)pinfo);
        if (HAL_GPT_STATUS_OK != gpt_sta) {
            log_bsp_led_error("[bsp][led][isink] led%d process: start sw timer(%x) fail(%d)", 3, channel, pinfo->gpt_handle, gpt_sta);
        }
    } else {
        gpt_sta = hal_gpt_sw_stop_timer_ms(pinfo->gpt_handle);
        if (HAL_GPT_STATUS_OK != gpt_sta) {
        }
    }
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
            log_bsp_led_info("[bsp][led][isink] led%d process: will in idle\r\n" , 1, channel);
            hal_isink_stop(channel);
            start_timer_continue = 0xFFFFFFFF;
            pinfo->next_action = BSP_LED_ACTION_MAX;
        }
        break;

        case BSP_LED_STATE_T0: {
            log_bsp_led_info("[bsp][led][isink] led%d process: start t0\r\n" , 1, channel);
            start_timer_continue = BSP_LED_T0_TO_MS(channel) + pinfo->led_cfg.sync_time;
            pinfo->next_action = BSP_LED_ACTION_OF_TIMEOUT;
#ifdef BSP_LED_SW_CALI_FEATURE
            bsp_led_sw_cali_init(channel, BSP_LED_T0_TO_MS(channel) + pinfo->led_cfg.sync_time);
#endif
        }
        break;

        case BSP_LED_STATE_T1T2_RPT: {
            log_bsp_led_info("[bsp][led][isink] led%d process: start t1t2 repeat\r\n", 1, channel);
            start_timer_continue = 0xFFFFFFFF;
            if (BSP_LED_T2_TO_MS(channel) == 0) {
                log_bsp_led_info("[bsp][led][isink] led%d process: always on\r\n" , 1, channel);
            } else if (pinfo->led_cfg.repeat_t1t2 == 0xFFFF) {
#ifdef BSP_LED_SW_CALI_FEATURE
                start_timer_continue = ((BSP_T1_T2_REPEAT_CALI_TM / pinfo->t1t2_cycle) + 1) * pinfo->t1t2_cycle;
                bsp_led_sw_cali(channel, start_timer_continue);
                pinfo->next_action   = BSP_LED_ACTION_OF_CALI_TIMEOUT;
                hal_isink_stop(channel);
#endif
                log_bsp_led_info("[bsp][led][isink] led%d process: t1t2 always repeat\r\n" , 1, channel);
                hal_isink_start(channel);
            } else {
                start_timer_continue = BSP_LED_T1_T2_WITH_REPEAT_TO_MS(channel);
                pinfo->next_action = BSP_LED_ACTION_OF_TIMEOUT;
                /* process ext loop */
                if (pinfo->led_cfg.repeat_ext != 0xFFFF) {
                    if (pinfo->led_cfg.repeat_ext == 0) {
                        bsp_led_process(channel, BSP_LED_ACTION_OF_STOP);
                        log_bsp_led_info("[bsp][led][isink] led%d process: end ext loop\r\n" , 1, channel);
                        if (pinfo->led_cfg.call_back != NULL) {
                            pinfo->led_cfg.call_back(pinfo->led_id, pinfo->led_cfg.user_data);
                        }
                        return;
                    }else {
                        pinfo->led_cfg.repeat_ext--;
                    }
                } else { /* if always loop ext, led need cali*/
#ifdef BSP_LED_SW_CALI_FEATURE
                    bsp_led_sw_cali(channel, BSP_LED_EXT_LOOP_MS(channel));
#endif
                    start_timer_continue = BSP_LED_T1_T2_WITH_REPEAT_TO_MS(channel);
                }
            }
            hal_isink_start(channel);
        }
        break;

        case BSP_LED_STATE_T3: {
            log_bsp_led_info("[bsp][led][isink] led%d process: start t3\r\n" , 1, channel);
            hal_isink_stop(channel);
            start_timer_continue = BSP_LED_T3_TO_MS(channel);
            pinfo->next_action = BSP_LED_ACTION_OF_TIMEOUT;
        }
        break;

        default: {
            hal_isink_stop(channel);
            start_timer_continue = 0xFFFFFFFF;
            log_bsp_led_info("[bsp][led][isink] led%d process: error state %d", 1, channel, pinfo->cur_state);
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
        log_bsp_led_info("[bsp][led][isink] led%d process: start timer(%x) %d ms, after cali %d ms", 4,
                          channel,
                         (uint32_t)pinfo->gpt_handle,
                         (uint32_t)start_timer_continue,
                         (uint32_t)temp_timer_ms);
        gpt_sta = hal_gpt_sw_start_timer_ms(pinfo->gpt_handle, temp_timer_ms, bsp_led_gpt_callback, (void *)pinfo);
        if (HAL_GPT_STATUS_OK != gpt_sta) {
            log_bsp_led_error("[bsp][led][isink] led%d process: start sw timer(%x) fail(%d)", 3, channel, pinfo->gpt_handle, gpt_sta);
        }
    }
    else {
        hal_gpt_status_t    gpt_sta;

        gpt_sta = hal_gpt_sw_stop_timer_ms(pinfo->gpt_handle);
        if (HAL_GPT_STATUS_OK != gpt_sta) {
            log_bsp_led_warn("[bsp][led][isink] led%d process: stop sw timer fail(%d)", 2, channel, gpt_sta);
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
        log_bsp_led_error("[bsp][led][isink] led%d init fail, chnl err\r\n ", 1, ledx);
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
        //log_bsp_led_info("[bsp][led][isink] led%d init: get sw gpt hdl(0x%x)", 2, ledx, pinfo->gpt_handle);
    }
    hal_isink_init(ledx);
    pinfo->op_state = BSP_LED_OP_STATE_ACTIVE;
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
    if(pinfo->op_state == BSP_LED_OP_STATE_ACTIVE){
        hal_gpt_sw_stop_timer_ms(pinfo->gpt_handle);
        hal_isink_deinit(ledx);
    }
    pinfo->op_state = BSP_LED_OP_STATE_IDLE;
    return BSP_LED_STATUS_OK;
}


bsp_led_status_t    bsp_led_isink_config(uint8_t ledx, bsp_led_config_t *cfg)
{
    bsp_isink_private_info_t    *pinfo =  &g_led_priv_info[ledx];

    if (cfg == NULL) {
        log_bsp_led_error("[bsp][led][isink] led%d config fail, para is null\r\n ", 1, ledx);
        return BSP_LED_STATUS_ERROR_INVALID_PARAMETER;
    }
    if (ledx >= BSP_LED_CHANNEL_MAX) {
        log_bsp_led_error("[bsp][led][isink] led%d config fail, chnl error\r\n ", 1, ledx);
        return BSP_LED_STATUS_ERROR_CHANNEL;
    }
    log_bsp_led_info("[bsp][led][isink] led%d config(sync %d)\r\n", 2, ledx, cfg->sync_time);
    pinfo->led_cfg = *cfg;
    if (BSP_LED_T1_TO_MS(ledx) == 0) {
        log_bsp_led_warn("[bsp][led][isink] led %d config, t1 is 0\r\n ", 1, ledx);
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
        log_bsp_led_warn("[bsp][led][isink] led%d start, t1 is '0'\r\n ", 1, ledx);
        return BSP_LED_STATUS_OK;
    }
    pinfo->cur_state = BSP_LED_STATE_IDLE;
    log_bsp_led_info("[bsp][led][isink] led%d start, mode is %d\r\n", 2, ledx, pinfo->mode);
#ifdef BSP_LED_FOR_ADIE32K_VARIATION
    bsp_led_process_ex(ledx, BSP_LED_STATE_TSYNC);
#else
    bsp_led_process(ledx, BSP_LED_ACTION_OF_START);
#endif
    return BSP_LED_STATUS_OK;
}

bsp_led_status_t    bsp_led_isink_stop(uint8_t ledx)
{
    //bsp_isink_private_info_t    *pinfo =  &g_led_priv_info[ledx];

    log_bsp_led_info("[bsp][led][isink] led%d stop\r\n", 1, ledx);
    if (ledx >= BSP_LED_CHANNEL_MAX) {
        log_bsp_led_error("[bsp][led][isink] led%d stop fail, chnl err\r\n", 1, ledx);
        return BSP_LED_STATUS_ERROR_CHANNEL;
    }
    if (BSP_LED_T1_TO_MS(ledx) == 0) {
        log_bsp_led_warn("[bsp][led][isink] led%d no stop, t1 == 0\r\n ", 1, ledx);
        hal_isink_stop(ledx);
        return BSP_LED_STATUS_OK;
    }
#ifdef BSP_LED_FOR_ADIE32K_VARIATION
    bsp_led_process_ex(ledx, BSP_LED_STATE_IDLE);
#else
    bsp_led_process(ledx, BSP_LED_ACTION_OF_STOP);
#endif
    return BSP_LED_STATUS_OK;
}

int    bsp_led_isink_ioctl(uint32_t ledx, uint32_t cmd, uint32_t option)
{
    return 0;
}
#endif
