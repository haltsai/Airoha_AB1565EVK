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
#ifdef HAL_PWM_MODULE_ENABLED

#include "hal_pwm.h"
#include "hal_pwm_internal.h"

#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager_platform.h"
#endif

#ifdef HAL_CLOCK_MODULE_ENABLED
#include "hal_clock.h"
#include "hal_log.h"
#define   PWM_CLOCK_MUX_ENABLED
#define   PWM_CLOCK_ENABLED
#endif

#define     HAL_PWM_MAX         HAL_PWM_MAX_CHANNEL

#ifdef PWM_CLOCK_MUX_ENABLED
extern  uint8_t                     clock_mux_cur_sel(clock_mux_sel_id mux_id);
extern  hal_clock_status_t          clock_mux_sel(clock_mux_sel_id mux_id, uint32_t mux_sel);
static const clock_mux_sel_id       pwm_clk_mux[HAL_PWM_MAX]    = {CLK_PWM0_SEL, CLK_PWM1_SEL, CLK_PWM2_SEL, CLK_PWM3_SEL, CLK_PWM4_SEL};
#else
#define   clock_mux_cur_sel(mux_id)         0
#define   clock_mux_sel(mux_id, mux_sel)
#endif

#ifdef PWM_CLOCK_ENABLED
static const    hal_clock_cg_id     pwm_pdn[HAL_PWM_MAX]        = { HAL_CLOCK_CG_PWM0, HAL_CLOCK_CG_PWM1, HAL_CLOCK_CG_PWM2, HAL_CLOCK_CG_PWM3, HAL_CLOCK_CG_PWM4 };
#endif

static volatile PWM_REGISTER_T     *pwm[HAL_PWM_MAX]            = { PWM0, PWM1, PWM2, PWM3, PWM4};
static          uint32_t            pwm_run_state[HAL_PWM_MAX]  = {0};



#ifdef HAL_PWM_FEATURE_SLEEP_SUPPORTED

static  PWM_REGISTER_T              pwm_backup_register[HAL_PWM_MAX];
static  void    pwm_suspend_callback()
{
    uint32_t    i = 0;

    for (i = 0; i < HAL_PWM_MAX; i++) {
        pwm_backup_register[i].PWM_CTRL       = pwm[i]->PWM_CTRL;
        pwm_backup_register[i].PWM_COUNT      = pwm[i]->PWM_COUNT;
        pwm_backup_register[i].PWM_THRESH     = pwm[i]->PWM_THRESH;
        pwm_backup_register[i].PWM_THRESH_DOWN = pwm[i]->PWM_THRESH_DOWN;
    }
}

static void     pwm_resume_callback()
{
    uint32_t    i = 0;

    for (i = 0; i < HAL_PWM_MAX; i++) {
        pwm[i]->PWM_COUNT        = pwm_backup_register[i].PWM_COUNT;
        pwm[i]->PWM_THRESH       = pwm_backup_register[i].PWM_THRESH;
        pwm[i]->PWM_THRESH_DOWN  = pwm_backup_register[i].PWM_THRESH_DOWN;
        pwm[i]->PWM_CTRL         = pwm_backup_register[i].PWM_CTRL;
    }
}
#endif

hal_pwm_status_t    hal_pwm_init(hal_pwm_channel_t pwm_channel, hal_pwm_source_clock_t source_clock)
{
    hal_clock_status_t  status;
    /*check parameter*/
    if (pwm_channel  >= HAL_PWM_MAX) {
        log_pwm_error("[pwm] ch%d is invalid!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }

    if (source_clock >= HAL_PWM_CLOCK_MAX) {
        log_pwm_error("[pwm] ch%d source clk is invalid!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    if (pwm_run_state[pwm_channel] & PWM_STATE_INIT_MASK) {
        log_pwm_error("[pwm] ch%d had been initialized!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }

    switch (source_clock) {
            /*For 32kHz Clock*/
#ifdef HAL_PWM_CLOCK_32K_SUPPORTED
        case HAL_PWM_CLOCK_32KHZ: {
            status = clock_mux_sel(pwm_clk_mux[pwm_channel], 2);
        }
        break;
#endif
            /*For 24MHz Clock*/
#ifdef HAL_PWM_CLOCK_24M_SUPPORTED
        case HAL_PWM_CLOCK_24MHZ: {
            source_clock = clock_mux_cur_sel(CLK_26M_SEL);
            status = clock_mux_sel(pwm_clk_mux[pwm_channel], source_clock);
        }
        break;
#endif
            /*For 26MHz Clock*/
#ifdef HAL_PWM_CLOCK_26M_SUPPORTED
        case HAL_PWM_CLOCK_26MHZ: {
            source_clock = clock_mux_cur_sel(CLK_26M_SEL);
            status = clock_mux_sel(pwm_clk_mux[pwm_channel], source_clock);
        }
        break;
#endif
            /*For 40MHz Clock*/
#ifdef HAL_PWM_CLOCK_40M_SUPPORTED
        case HAL_PWM_CLOCK_40MHZ: {
            status = clock_mux_sel(pwm_clk_mux[pwm_channel], 3);
        }
        break;
#endif
            /*For 41MHz Clock*/
#ifdef HAL_PWM_CLOCK_41M_SUPPORTED
        case HAL_PWM_CLOCK_41MHZ: {
            status = clock_mux_sel(pwm_clk_mux[pwm_channel], 3);
        }
        break;
#endif
            /*For 48MHz Clock*/
#ifdef HAL_PWM_CLOCK_48M_SUPPORTED
        case HAL_PWM_CLOCK_48MHZ: {
            status = clock_mux_sel(pwm_clk_mux[pwm_channel], 3);
        }
        break;
#endif
        default: {
            return HAL_PWM_STATUS_INVALID_PARAMETER;
        };
    }
    if (status != HAL_CLOCK_STATUS_OK) {
        log_pwm_error("[pwm] ch%d clk mux sel fail(%d)!\r\n", 2, pwm_channel, status);
        return HAL_PWM_STATUS_ERROR;
    }
    /*initial pwm register*/
    pwm[pwm_channel]->PWM_CTRL   = 0;
    pwm[pwm_channel]->PWM_COUNT  = 0;
    pwm[pwm_channel]->PWM_THRESH = 0;
    pwm[pwm_channel]->PWM_THRESH_DOWN = 0;
    pwm[pwm_channel]->PWM_CTRL  |= ((source_clock & 0x03) << PWM_CLK_SEL_OFFSET);

#ifdef HAL_PWM_FEATURE_SLEEP_SUPPORTED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_PWM, pwm_suspend_callback, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_PWM,  pwm_resume_callback,  NULL);
#endif
    pwm_run_state[pwm_channel] |= PWM_STATE_INIT_MASK;
    return HAL_PWM_STATUS_OK;
}


hal_pwm_status_t    hal_pwm_deinit(hal_pwm_channel_t pwm_channel)
{
    /*check parameter*/
    if (pwm_channel >= HAL_PWM_MAX) {
        log_pwm_error("[pwm] ch%d is invalid!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    hal_pwm_stop(pwm_channel);
    pwm_run_state[pwm_channel] = 0;
    return HAL_PWM_STATUS_OK;
}


hal_pwm_status_t    hal_pwm_set_frequency(hal_pwm_channel_t pwm_channel, uint32_t frequency, uint32_t *total_count)
{
    uint32_t    clock_div = 0;
    uint32_t    clock_src = 0;
    uint32_t    clock_pwm = 0;
    uint32_t    control   = 0;
    uint32_t    min_frequency = 0;
    uint32_t    temp      = 0;

    /*Check parameter*/
    if (pwm_channel >= HAL_PWM_MAX || total_count == NULL) {
        log_pwm_error("[pwm] ch%d args is invalid!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }
    if ((pwm_run_state[pwm_channel] & PWM_STATE_INIT_MASK) == 0) {
        log_pwm_error("[pwm] ch%d is not init!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }

    /*Get pwm source clock*/
    control   = pwm[pwm_channel]->PWM_CTRL;
    clock_div = 1 << (control & PWM_CLK_DIV_MASK);
    temp      = (control & PWM_CLK_SEL_MASK) >> PWM_CLK_SEL_OFFSET;
    switch (temp) {
#ifdef HAL_PWM_CLOCK_32K_SUPPORTED
        case HAL_PWM_CLOCK_32KHZ:
            clock_src = PWM_CLOCK_32K;
            break;
#endif
#ifdef HAL_PWM_CLOCK_24M_SUPPORTED
        case HAL_PWM_CLOCK_24MHZ:
            clock_src = PWM_CLOCK_24M;
            break;
#endif
#ifdef HAL_PWM_CLOCK_26M_SUPPORTED
        case HAL_PWM_CLOCK_26MHZ:
            clock_src = PWM_CLOCK_26M;
            break;
#endif
#ifdef HAL_PWM_CLOCK_40M_SUPPORTED
        case HAL_PWM_CLOCK_40MHZ:
            clock_src = PWM_CLOCK_40M;
            break;
#endif
#ifdef HAL_PWM_CLOCK_41M_SUPPORTED
        case HAL_PWM_CLOCK_41MHZ:
            clock_src = PWM_CLOCK_41M;
            break;
#endif
#ifdef HAL_PWM_CLOCK_48M_SUPPORTED
        case HAL_PWM_CLOCK_48MHZ:
            clock_src = PWM_CLOCK_48M;
            break;
#endif
        default: {
            log_pwm_error("[pwm] ch%d clock source invalid(%d)!\r\n", 2, pwm_channel, temp);
            return HAL_PWM_STATUS_INVALID_FREQUENCY;
        }
    }
    clock_pwm     = clock_src / clock_div;
    min_frequency = clock_pwm / PWM_MAX_COUNT;
    if (frequency < min_frequency) {
        log_pwm_error("[pwm] ch%d freq(%dHz) smaller than hw support(hw min freq %dHz)!\r\n", 3, pwm_channel, frequency, min_frequency);
        return HAL_PWM_STATUS_INVALID_FREQUENCY;
    } else {
        temp = (clock_pwm / frequency);
    }

    if (temp > 0) {
        pwm[pwm_channel]->PWM_COUNT = temp - 1;
        *total_count = temp;
        pwm_run_state[pwm_channel] |=  PWM_STATE_SET_FREQ_MASK;
        return HAL_PWM_STATUS_OK;
    } else {
        log_pwm_error("[pwm] ch%d freq(%dHz) bigger than hw support(hw min freq %dHz)!\r\n", 3, pwm_channel, frequency, clock_pwm);
        return HAL_PWM_STATUS_INVALID_FREQUENCY;
    }
}


hal_pwm_status_t    hal_pwm_set_duty_cycle(hal_pwm_channel_t pwm_channel, uint32_t duty_cycle)
{
    uint32_t    temp  = 0;

    if (pwm_channel >= HAL_PWM_MAX) {
        log_pwm_error("[pwm] ch%d is invalid!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    if ((pwm_run_state[pwm_channel] & PWM_STATE_INIT_MASK) == 0) {
        log_pwm_error("[pwm] ch%d is not init!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }
    temp = pwm[pwm_channel]->PWM_COUNT;
    pwm[pwm_channel]->PWM_THRESH_DOWN = 0;

    if (0 == duty_cycle) {
        pwm[pwm_channel]->PWM_THRESH_DOWN = 1;
        pwm[pwm_channel]->PWM_THRESH      = 0;
    } else if (duty_cycle > temp) {
        pwm[pwm_channel]->PWM_THRESH = temp;
    } else {
        pwm[pwm_channel]->PWM_THRESH = duty_cycle - 1;
    }
    pwm_run_state[pwm_channel] |= PWM_STATE_SET_DUTY_MASK;
    return HAL_PWM_STATUS_OK;
}


hal_pwm_status_t    hal_pwm_start(hal_pwm_channel_t pwm_channel)
{
    if (pwm_channel >= HAL_PWM_MAX) {
        log_pwm_error("[pwm] ch%d is invalid!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    if ((pwm_run_state[pwm_channel] & PWM_STATE_INIT_MASK) == 0) {
        log_pwm_error("[pwm] ch%d is not init!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }
    if ((pwm_run_state[pwm_channel] & PWM_STATE_SET_FREQ_MASK) == 0) {
        log_pwm_error("[pwm] ch%d not set freq", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }
    if ((pwm_run_state[pwm_channel] & PWM_STATE_SET_DUTY_MASK) == 0) {
        log_pwm_error("[pwm] ch%d not set duty!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }
    if (pwm_run_state[pwm_channel] & PWM_STATE_START_MASK) {
        log_pwm_error("[pwm] ch%d is busy!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }
    /*enable pwm channel clock*/
#ifdef PWM_CLOCK_ENABLED
    if (HAL_CLOCK_STATUS_ERROR == hal_clock_enable(pwm_pdn[pwm_channel])) {
        log_pwm_error("[pwm] ch%d enable clock failed!\r\n", 1, pwm_channel);
        return  HAL_PWM_STATUS_ERROR;
    }
#endif
    pwm_run_state[pwm_channel] |= PWM_STATE_START_MASK;
    return HAL_PWM_STATUS_OK;
}


hal_pwm_status_t    hal_pwm_stop(hal_pwm_channel_t pwm_channel)
{
    hal_pwm_status_t    result = HAL_PWM_STATUS_OK;

    if (pwm_channel >= HAL_PWM_MAX) {
        log_pwm_error("[pwm] ch%d is invalid!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }

    if ((pwm_run_state[pwm_channel] & PWM_STATE_INIT_MASK) == 0) {
        log_pwm_error("[pwm] ch%d is not init!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }

    /*disable pwm channel clock*/
#ifdef PWM_CLOCK_ENABLED
    if (HAL_CLOCK_STATUS_ERROR == hal_clock_disable(pwm_pdn[pwm_channel])) {
        result = HAL_PWM_STATUS_ERROR;
    }
#endif
    pwm_run_state[pwm_channel] &= ~PWM_STATE_START_MASK;
    return result;
}


hal_pwm_status_t    hal_pwm_get_frequency(hal_pwm_channel_t pwm_channel, uint32_t *frequency)
{
    uint8_t    clock_div = 0;
    uint32_t   clock_src = 0;
    uint32_t   clock_pwm = 0;
    uint32_t   control   = 0;
    uint32_t   temp      = 0;

    /*Check parameter*/
    if (pwm_channel >= HAL_PWM_MAX || frequency == NULL) {
        log_pwm_error("[pwm] ch%d args is invalid\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }
    if ((pwm_run_state[pwm_channel] & PWM_STATE_SET_FREQ_MASK) == 0) {
        log_pwm_error("[pwm] ch%d frequency not set!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }

    /*Get clock division*/
    control   = pwm[pwm_channel]->PWM_CTRL;
    temp      = (control & PWM_CLK_DIV_MASK) >> PWM_CLK_DIV_OFFSET;
    switch (temp) {
        case 0:
            clock_div = 1;
            break;
        case HAL_PWM_CLOCK_DIVISION_2:
            clock_div = 2;
            break;
        case HAL_PWM_CLOCK_DIVISION_4:
            clock_div = 4;
            break;
        case HAL_PWM_CLOCK_DIVISION_8:
            clock_div = 8;
            break;
        default:
            clock_div = 1;
            break;
    }

    /*Get clock source*/
    temp      = (control & PWM_CLK_SEL_MASK) >> PWM_CLK_SEL_OFFSET;
    switch (temp) {
#ifdef HAL_PWM_CLOCK_32K_SUPPORTED
        case HAL_PWM_CLOCK_32KHZ:
            clock_src = PWM_CLOCK_32K;
            break;
#endif
#ifdef HAL_PWM_CLOCK_24M_SUPPORTED
        case HAL_PWM_CLOCK_24MHZ:
            clock_src = PWM_CLOCK_24M;
            break;
#endif

#ifdef HAL_PWM_CLOCK_26M_SUPPORTED
        case HAL_PWM_CLOCK_26MHZ:
            clock_src = PWM_CLOCK_26M;
            break;
#endif
#ifdef HAL_PWM_CLOCK_40M_SUPPORTED
        case HAL_PWM_CLOCK_40MHZ:
            clock_src = PWM_CLOCK_40M;
            break;
#endif
#ifdef HAL_PWM_CLOCK_41M_SUPPORTED
        case HAL_PWM_CLOCK_41MHZ:
            clock_src = PWM_CLOCK_41M;
            break;
#endif
#ifdef HAL_PWM_CLOCK_48M_SUPPORTED
        case HAL_PWM_CLOCK_48MHZ:
            clock_src = PWM_CLOCK_48M;
            break;
#endif
    }
    clock_pwm = clock_src / clock_div;

    //Cal frequency
    temp      = pwm[pwm_channel]->PWM_COUNT + 1;
    *frequency = clock_pwm / temp;

    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t    hal_pwm_get_duty_cycle(hal_pwm_channel_t pwm_channel, uint32_t *duty_cycle)
{

    /*Check parameter*/
    if (pwm_channel >= HAL_PWM_MAX || duty_cycle == NULL) {
        log_pwm_error("[pwm] ch%d args is invalid\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }
    if ((pwm_run_state[pwm_channel] & PWM_STATE_SET_DUTY_MASK) == 0) {
        log_pwm_error("[pwm] ch%d duty not set!\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }
    *duty_cycle = pwm[pwm_channel]->PWM_THRESH;

    return HAL_PWM_STATUS_OK;
}



hal_pwm_status_t    hal_pwm_get_running_status(hal_pwm_channel_t pwm_channel, hal_pwm_running_status_t *runing_status)
{
    if (pwm_channel >= HAL_PWM_MAX || runing_status == NULL) {
        log_pwm_error("[pwm] ch%d args is invalid\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }

    if (pwm_run_state[pwm_channel] & PWM_STATE_START_MASK) {
        *runing_status = HAL_PWM_BUSY;
    } else {
        *runing_status = HAL_PWM_IDLE;
    }
    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t    hal_pwm_set_advanced_config(hal_pwm_channel_t pwm_channel, hal_pwm_advanced_config_t advanced_config)
{

    if (pwm_channel >= HAL_PWM_MAX) {
        log_pwm_error("[pwm] ch%d is invalid\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }
    if (pwm_run_state[pwm_channel] & PWM_STATE_START_MASK) {
        log_pwm_error("[pwm] ch%d is busy\r\n", 1, pwm_channel);
        return HAL_PWM_STATUS_ERROR;
    }

    if (advanced_config >= HAL_PWM_CLOCK_DIVISION_MAX) {
        return HAL_PWM_STATUS_INVALID_PARAMETER;
    }
    pwm[pwm_channel]->PWM_CTRL &= ~(PWM_CLK_DIV_MASK);
    pwm[pwm_channel]->PWM_CTRL |= advanced_config;

    return HAL_PWM_STATUS_OK;
}

hal_pwm_status_t  hal_pwm_set_frequency_and_duty(hal_pwm_channel_t pwm_channel, hal_pwm_source_clock_t clk, uint32_t frequency, float duty_ratio)
{
    uint32_t   clk_div = 0;
    uint32_t   pwm_count = 0;
    uint32_t   pwm_count_rg = 0;
    uint32_t   clk_div_rg = 0;
    uint32_t   duty_cycle = 0;
    uint32_t   pwm_freq = 0;
    uint32_t   temp  = 0;
    uint32_t   temp1 = 0;
    uint32_t   temp2 = 0;
    uint32_t   temp3 = 0;

    /*Check parameter*/
    if (pwm_channel >= HAL_PWM_MAX_CHANNEL) {
        log_hal_msgid_error("channel is invalid!\r\n", 0);
        return HAL_PWM_STATUS_ERROR_CHANNEL;
    }

    switch(clk) {
        case HAL_PWM_CLOCK_32KHZ:
            pwm_freq = 32000;
        break;

        case HAL_PWM_CLOCK_26MHZ:
            pwm_freq = 26000000;
        break;

        case HAL_PWM_CLOCK_26MHZ_OSC:
            pwm_freq = 26000000;
        break;

        case HAL_PWM_CLOCK_41MHZ:
            pwm_freq = 41000000;
        break;
    }

    for(clk_div = 0; clk_div <= 3; clk_div++) {
        /* Calculate the frequency division factor */
        temp1 = 1 << clk_div;
        temp3 = temp1 * frequency;
        pwm_count = pwm_freq / temp3;
        temp2 = pwm_freq % temp3;
        if(temp2 >= temp3/2) {
            pwm_count++;
        }

        if( pwm_count <= PWM_MAX_COUNT) {
            pwm_count_rg =  pwm_count - 1;
            clk_div_rg = clk_div;
            break;
        }
    }

    if(clk_div > 3) {
      log_hal_msgid_error("frequency is invalid!\r\n", 0);
      return HAL_PWM_STATUS_INVALID_FREQUENCY;
    }

    /*set clock div*/
    pwm[pwm_channel]->PWM_CTRL &= ~(PWM_CLK_DIV_MASK);
    pwm[pwm_channel]->PWM_CTRL |= clk_div_rg;

    /*set pwm counter*/
    pwm[pwm_channel]->PWM_COUNT = pwm_count_rg;

    /*set duty cycle*/
    duty_cycle = (uint32_t) (pwm_count*duty_ratio);
    temp = pwm[pwm_channel]->PWM_COUNT;
    pwm[pwm_channel]->PWM_THRESH_DOWN = 0;

    /* duty_ratio =0 */
    if (0 == duty_cycle) {
        pwm[pwm_channel]->PWM_THRESH_DOWN = 1;
        pwm[pwm_channel]->PWM_THRESH      = 0;
    } else if (duty_cycle > temp) {    /* duty_ratio =1 */
        pwm[pwm_channel]->PWM_THRESH = temp;
    } else {
        pwm[pwm_channel]->PWM_THRESH = duty_cycle-1;
    }

    return HAL_PWM_STATUS_OK;
}


#endif


