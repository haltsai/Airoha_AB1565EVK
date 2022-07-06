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

#include "hal_captouch.h"

#ifdef HAL_CAPTOUCH_MODULE_ENABLED
#include "hal_captouch_internal.h"
#include "hal_rtc_internal.h"

#include "hal_pmu.h"
#include "hal_gpt.h"
#include <string.h>
#include "hal_nvic.h"

log_create_module(Captouch, PRINT_LEVEL_INFO);
#define UNUSED(p) ((void)(p))

//HW basic configuration
extern uint8_t pmu_get_power_on_reason();
extern GPIO_CFG0_REGISTER_T *gpio_cfg0;
extern hal_captouch_sdwu_nvdm_data sdwu_setting;
extern hal_captouch_eardetect_nvdm_data eardetect_setting;
extern hal_captouch_autosuspend_timeout_nvdm_data autosuspend_timeout_data;
extern uint32_t tune_delay_us, det_delay_us;


extern captouch_ear_detect_t captouch_ear_detect;
extern captouch_autosuspend_timeout_context_t autosuspend_timeout_context;

extern TimerHandle_t captouch_fine_base_delay100ms_timer;
extern TimerHandle_t captouch_earcheck_timer;
extern TimerHandle_t captouch_earcheck_stop_baseK_timer;

extern bool gsend_flag[CAPTOUCH_CHANNEL_MAX];
extern bool gfirstTouch_flag[CAPTOUCH_CHANNEL_MAX];

hal_captouch_status_t hal_captouch_set_avg(hal_captouch_channel_t channel,uint8_t mavg_val, uint8_t avg_val)
{
    if (mavg_val > CAPTOUCH_MAVG_MAX) {
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }
    if (avg_val > CAPTOUCH_AVG_MAX) {
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }
    captouch_channel_sense_control((1<<channel), captouch_disable);
    captouch_set_mavg(channel,mavg_val);
    captouch_set_avg(channel,avg_val);
    captouch_channel_sense_control((1<<channel), captouch_enable);
    return HAL_CAPTOUCH_STATUS_OK;
}

static hal_captouch_status_t hal_captouch_power_on_init(bool is_en)
{
    hal_captouch_status_t ret;

    ret = captouch_rtc_clk_control(is_en);
    captouch_analog_init();
    return ret;
}

static void hal_captouch_init_tune(hal_captouch_config_t *config, bool capcon_state, bool isChargerIn)
{
    uint8_t tune_ch_bit_map;
    uint32_t i;
    hal_captouch_tune_data_t tune_data;

    tune_ch_bit_map = config->channel_bit_map;
    if((config->channel_bit_map & (1<<HAL_CAPTOUCH_CHANNEL_0)) && captouch_get_channel_trigger(0) && capcon_state)
    {
        tune_ch_bit_map = tune_ch_bit_map & (~(1<<HAL_CAPTOUCH_CHANNEL_0) & 0x0F);
    }
    if(tune_ch_bit_map == 0) return;//no channel need tune

    captouch_set_autok_suspend(tune_ch_bit_map, false);
    for(i=HAL_CAPTOUCH_CHANNEL_0;i<HAL_CAPTOUCH_CHANNEL_MAX;i++)
    {
        if (tune_ch_bit_map & (1<<i))
            hal_captouch_set_avg(i, 8, captouch_context.avg_s[i]);
    }
    captouch_get_tune_delay_time(32, 8);
    hal_gpt_delay_us(tune_delay_us); //waiting for stable
    for(i=HAL_CAPTOUCH_CHANNEL_0;i<HAL_CAPTOUCH_CHANNEL_MAX;i++)
    {
        if(tune_ch_bit_map & (1<<i))
            captouch_find_baseline(i);
    }
    for(i=HAL_CAPTOUCH_CHANNEL_0;i<HAL_CAPTOUCH_CHANNEL_MAX;i++)
    {
        if (config->swtune_en && (tune_ch_bit_map & (1<<i)) && (isChargerIn || !capcon_state))
        {
            tune_data.coarse_cap = config->coarse_cap[i];
            hal_captouch_tune_control(i, HAL_CAPTOUCH_TUNE_HW_AUTO, &tune_data);
        }
        if (tune_ch_bit_map & (1<<i))
            hal_captouch_set_avg(i, captouch_context.mavg_r[i], captouch_context.avg_s[i]);
    }
    captouch_set_autok_suspend(config->channel_bit_map, true);
}

static void hal_captouch_manual_mode_control(uint8_t channel, bool isEn)
{
    HAL_CAPTOUCH_LOG_PRINT("captouch_manual_mode_control channel %d, isEn %d, manual fine cap %d", 3, channel, isEn, captouch_fine_cap_from_nvkey(channel));
    if(isEn)
    {
        hal_captouch_set_fine_cap(channel, captouch_fine_cap_from_nvkey(channel));
        captouch_set_control_manual(channel, true);
        captouch_get_tune_delay_time(32, 8);
        captouch_fine_base_delay100ms_timer = xTimerCreate("fine_tune_delay", pdMS_TO_TICKS((tune_delay_us/1000)), pdFALSE, NULL, captouch_fine_tune_delay_handler);
    }
    else
    {
        captouch_set_control_manual(channel, false);
        captouch_set_autok_suspend(captouch_context.used_channel_map, false);
        hal_captouch_set_avg(channel, 8, captouch_context.avg_s[channel]);
        hal_gpt_delay_us(tune_delay_us); //wait fine tune
        captouch_find_baseline(channel);
        hal_captouch_set_avg(channel, captouch_context.mavg_r[channel], captouch_context.avg_s[channel]);
        captouch_set_autok_suspend(captouch_context.used_channel_map, true);
    }
}

hal_captouch_status_t hal_captouch_init(hal_captouch_config_t *config)
{
    uint32_t i;
    uint8_t bitmap;
    uint8_t ear_detect_en;
    hal_captouch_status_t ret;
    bool capcon_state, lpwu_flag = false, isChargerIn, autosuspend_timeout_en;
    //hal_captouch_config_t config_temp;
    if (captouch_context.has_initilized == true) {
        return HAL_CAPTOUCH_STATUS_INITIALIZED;
    }

    if (config->callback.callback == NULL) {
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    if(!captouch_is_feature_enable())
        return HAL_CAPTOUCH_STATUS_ERROR;

    if(!captouch_sdwusetting_from_NVKEY()){
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    capcon_state = rtc_internal_captouch_get_capcon_state();
    ear_detect_en = captouch_eardetect_enable();
    //lpwu_flag = captouch_get_lpwu_int_flag();
    if(capcon_state && (hal_rtc_get_power_on_reason() == RTC_POWERED_BY_CAPTOUCH) && (sdwu_setting.sdwu_en & (1<<HAL_CAPTOUCH_LONGPRESS_WAKEUP)))
	    lpwu_flag = true;
    isChargerIn = captouch_get_charge_in_status();
    autosuspend_timeout_en = captouch_autosuspend_timeout_enable();
    HAL_CAPTOUCH_LOG_PRINT("captouch init capcon_state:%d, lpwu_flag:%d, isChargerIn:%d, auto_tune:%d, ear_detect_ch:%d, autosuspend_timeout_en:%d", 6,
                                                                                                            capcon_state, lpwu_flag, isChargerIn, config->swtune_en, ear_detect_en, autosuspend_timeout_en);

    memset(&captouch_context, 0, sizeof(captouch_context_t));
    memset(&gsend_flag, SEND_PRESS, CAPTOUCH_CHANNEL_MAX);
    memset(&gfirstTouch_flag, 1, CAPTOUCH_CHANNEL_MAX);
    memset(&captouch_ear_detect,0,sizeof(captouch_ear_detect_t));
    memset(&autosuspend_timeout_context ,0 ,sizeof(autosuspend_timeout_context));
    //get ept channel bit map
    captouch_context.used_channel_map = config->channel_bit_map;
    captouch_context.captouch_callback.callback  = config->callback.callback;
    captouch_context.captouch_callback.user_data = config->callback.user_data;
    captouch_context.swDebounceTime = config->swDebounceTime;
#if (PRODUCT_VERSION == 2822)
    captouch_context.swDebounceTime_Rls = config->swDebounceTime;
#endif
    //open captouch annalog clock

    ret = hal_captouch_power_on_init(captouch_enable);
    if (ret != HAL_CAPTOUCH_STATUS_OK) {
        return ret;
    }

    //set threshold and coarse_cap
    for(i=HAL_CAPTOUCH_CHANNEL_0;i<HAL_CAPTOUCH_CHANNEL_MAX;i++) {

        if (captouch_context.used_channel_map & (1<<i)) {
            captouch_context.avg_s[i] = config->avg_s[i];
            captouch_context.mavg_r[i] = config->mavg_r[i];
            config->coarse_cap[i] = captouch_coarsecap_from_nvkey(i);
            ret = hal_captouch_set_threshold(i,config->high_thr[i],config->low_thr[i]);
            if (ret != HAL_CAPTOUCH_STATUS_OK) {
                return ret;
            }
            ret = hal_captouch_set_coarse_cap(i,config->coarse_cap[i]);

            if (ret != HAL_CAPTOUCH_STATUS_OK) {
                return ret;
            }
            ret = hal_captouch_set_avg(i,config->mavg_r[i],config->avg_s[i]);
            if (ret != HAL_CAPTOUCH_STATUS_OK) {
                return ret;
            }
            if(i==HAL_CAPTOUCH_CHANNEL_1) {
                gpio_cfg0->GPIO_G.CLR = (1<<3);
            }
            if(i==HAL_CAPTOUCH_CHANNEL_2) {
                gpio_cfg0->GPIO_G.CLR = (1<<4);
            }
            if(i==HAL_CAPTOUCH_CHANNEL_3) {
                gpio_cfg0->GPIO_G.CLR = (1<<5);
            }
        }
    }
    hal_gpt_delay_us(10);

    //enable manual mode
    if(capcon_state && (captouch_context.used_channel_map & (1<<0)) && !isChargerIn && !config->swtune_en){
        hal_captouch_manual_mode_control(HAL_CAPTOUCH_CHANNEL_0, true);
    }

    captouch_channel_sense_control(captouch_context.used_channel_map, captouch_enable);

    if(ear_detect_en != 0){
        for(int i=HAL_CAPTOUCH_CHANNEL_0;i<HAL_CAPTOUCH_CHANNEL_MAX;i++) {
            if (eardetect_setting.detect_ch & (1<<i)) {
                //hal_captouch_set_threshold(i,255,normal_setting->lowThre[i]);
                captouch_ear_detect.detect_ch= i;
                captouch_context.highThre[i] = config->high_thr[i];
                captouch_context.lowThre[i] = config->low_thr[i];
                captouch_ear_detect.mavg_num = 1<<(eardetect_setting.BaseAvgNum);
                captouch_ear_detect.baseKFrozeTime = eardetect_setting.baseKFrozeTime;
                captouch_ear_detect.stop_avg = true;
            }
        }
    }else {
         captouch_ear_detect.detect_ch = EAR_DETECT_DISABLE;
    }

    bitmap = captouch_context.used_channel_map & (~eardetect_setting.detect_ch);
    captouch_channel_int_control(bitmap, captouch_enable);

#if (PRODUCT_VERSION == 2822 || defined(AB1568))
    pmu_set_register_value(PMU_CAP_LPSD_CON0,0x3,0,0x1);
    pmu_set_register_value(PMU_CAP_LPSD_CON1,0x3,0,0);
#endif

     // auto_suspend time out function enable
    if(autosuspend_timeout_en){
        autosuspend_timeout_context.ch_bitmap = autosuspend_timeout_data.ch_bitmap &(~autosuspend_timeout_data.lpsd_ch_bitmap)\
                                                                                   &(~autosuspend_timeout_data.lpwu_ch_bitmap);

        for (int i =HAL_CAPTOUCH_CHANNEL_0; i<HAL_CAPTOUCH_CHANNEL_MAX;i++){
            if(autosuspend_timeout_data.time[i]!= 0 )
                autosuspend_timeout_context.time[i]= autosuspend_timeout_data.time[i];
        }
    }

    captouch_set_dynamic_threshold(true,true,0x28,0);
    captouch_clk_control(true);
    captouch_get_det_delay_time(config, 32);
    hal_gpt_delay_us(det_delay_us);

    HAL_CAPTOUCH_LOG_PRINT("captouch fine/coarse cal process channel coarse cap:%x, %x, %x, %x", 4, config->coarse_cap[0], config->coarse_cap[1], config->coarse_cap[2], config->coarse_cap[3]);
    hal_captouch_init_tune(config, capcon_state, isChargerIn);

    //disable manual mode
    if(!captouch_get_channel_trigger(HAL_CAPTOUCH_CHANNEL_0) && captouch_get_control_manual_state(HAL_CAPTOUCH_CHANNEL_0))
    {
        hal_captouch_manual_mode_control(HAL_CAPTOUCH_CHANNEL_0, false);
    }
    if((sdwu_setting.sdwu_en & (1<<HAL_CAPTOUCH_LONGPRESS_SHUTDOWN)) && !ear_detect_en && !lpwu_flag) {
        if(autosuspend_timeout_data.lpsd_ch_bitmap){
                captouch_longpress_channel_select_control(HAL_CAPTOUCH_LONGPRESS_SHUTDOWN,\
                                                         autosuspend_timeout_data.lpsd_ch_bitmap,\
                                                         sdwu_setting.sdtime*LONGPRESS_TIME_S );
        }else{
            captouch_longpress_channel_control(HAL_CAPTOUCH_LONGPRESS_SHUTDOWN,sdwu_setting.sdtime*LONGPRESS_TIME_S);
        }
        //pmu_lpsd_rst_init(CAP_LPSD, PMU_ON);
        if(sdwu_setting.sdwu_en & (1<<HAL_CAPTOUCH_LONGPRESS_SHUTDOWN_VBUS)){
            captouch_set_vbus_flag(captouch_get_charge_in_status());
            captouch_set_lpsd_chk_vbus(true);
        }
    }
    if(sdwu_setting.sdwu_en & (1<<HAL_CAPTOUCH_LONGPRESS_WAKEUP)) {
        captouch_longpress_int_control(true);
        captouch_longpress_channel_control(HAL_CAPTOUCH_LONGPRESS_WAKEUP,sdwu_setting.wutime*LONGPRESS_TIME_S);
    }
     captouch_set_autok_suspend(eardetect_setting.detect_ch,false);
    //captouch_set_autok_Nsuspend(captouch_context.used_channel_map,true);

    //register nvic irq handler and enable irq
    captouch_register_nvic_callback();
    captouch_int_control(true);

    //Create SW mode timer for in ear channel

    if( captouch_ear_detect.detect_ch != EAR_DETECT_DISABLE){
        captouch_ear_detect.ear_detect_base[captouch_ear_detect.detect_ch]= captouch_eardetect_base_from_nvkey(captouch_ear_detect.detect_ch);
        captouch_earcheck_timer = xTimerCreate("CaptouchEarCheck", pdMS_TO_TICKS(500), pdTRUE , NULL, captouch_sw_ear_check);

        if(xTimerIsTimerActive( captouch_earcheck_timer ))
            xTimerStop(captouch_earcheck_timer,0);
        xTimerStart( captouch_earcheck_timer,0 );

        if( captouch_ear_detect.baseKFrozeTime != 0 ){
            captouch_earcheck_stop_baseK_timer = xTimerCreate ("CaptouchEarBasekStop", pdMS_TO_TICKS(captouch_ear_detect.baseKFrozeTime*1000),\
                                                                                       pdFALSE,\
                                                                                       NULL,\
                                                                                       captouch_stop_earcheck_basek_timeout);
        }
    }


    captouch_context.has_initilized = true;
    if(lpwu_flag && captouch_get_channel_trigger(0))
    {
        captouch_send_press_msg();
    }
    return HAL_CAPTOUCH_STATUS_OK;

}

hal_captouch_status_t hal_captouch_coarse_cap_tune(hal_captouch_coarse_cal_t* coarse_cal_result)
{
    uint8_t i;
    hal_captouch_tune_data_t tune_data;

    captouch_get_tune_delay_time(32, 8);
    captouch_int_control(false);
    if(sdwu_setting.sdwu_en & (1<<HAL_CAPTOUCH_LONGPRESS_WAKEUP))
        captouch_longpress_int_control(false);

    for(i = HAL_CAPTOUCH_CHANNEL_0; i < HAL_CAPTOUCH_CHANNEL_MAX; i++)
    {
        if(captouch_context.used_channel_map & (1<<i))
        {
            captouch_set_autok_suspend((1<<i), false);
            hal_captouch_set_avg(i, 8, captouch_context.avg_s[i]);
            hal_gpt_delay_us(tune_delay_us); //wait fine tune
            captouch_find_baseline(i);
            tune_data.coarse_cap = captouch_coarsecap_from_nvkey(i);
            coarse_cal_result->coarse_cal[i].result = hal_captouch_tune_control(i, HAL_CAPTOUCH_TUNE_HW_AUTO,&tune_data);
            coarse_cal_result->coarse_cal[i].coarse_value= captouch_get_coarse_cap(i);
            hal_captouch_set_avg(i, captouch_context.mavg_r[i], captouch_context.avg_s[i]);
        }
        else
        {
            coarse_cal_result->coarse_cal[i].result = 0xFF;
            coarse_cal_result->coarse_cal[i].coarse_value = 0xFF;
        }
    }
    captouch_set_autok_suspend(captouch_context.used_channel_map, true);
    hal_gpt_delay_us(200);
    captouch_int_control(true);
    if(sdwu_setting.sdwu_en & (1<<HAL_CAPTOUCH_LONGPRESS_WAKEUP))
        captouch_longpress_int_control(true);

    return HAL_CAPTOUCH_STATUS_OK;
}

hal_captouch_status_t hal_captouch_channel_enable(hal_captouch_channel_t channel)
{
    if (channel>= HAL_CAPTOUCH_CHANNEL_MAX) {

        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    //touch channel enable&aio en
    captouch_channel_sense_control(1<<channel, captouch_enable);

    //enable high and low threhold wakeup&int interrupt
    captouch_channel_int_control(1<<channel, captouch_enable);

    return HAL_CAPTOUCH_STATUS_OK;
}

hal_captouch_status_t hal_captouch_channel_disable(hal_captouch_channel_t channel)
{
    if (channel>= HAL_CAPTOUCH_CHANNEL_MAX) {

        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    //disable chanel sensing
    captouch_channel_sense_control(channel, captouch_disable);

    //mask hign and low threhold wakeup&int interrupt
    captouch_channel_int_control(1<<channel, captouch_disable);

    return HAL_CAPTOUCH_STATUS_OK;
}

hal_captouch_status_t hal_captouch_get_event(hal_captouch_event_t *event)
{
    if (event == NULL) {
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    if (captouch_get_buffer_data_size()<=0) {
        return HAL_CAPTOUCH_STATUS_NO_EVENT;
    }

    captouch_pop_one_event_from_buffer(event);

    return HAL_CAPTOUCH_STATUS_OK;
}


hal_captouch_status_t hal_captouch_translate_channel_to_symbol(hal_captouch_channel_t channel, uint8_t *symbol)
{
    if (channel>= HAL_CAPTOUCH_CHANNEL_MAX) {

        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    *symbol = captouch_mapping_keydata[channel];

    return HAL_CAPTOUCH_STATUS_OK;
}

hal_captouch_status_t hal_captouch_set_threshold(hal_captouch_channel_t channel,int32_t high_thr, int32_t low_thr)
{
    if (channel >= HAL_CAPTOUCH_CHANNEL_MAX) {
        return HAL_CAPTOUCH_STATUS_CHANNEL_ERROR;
    }

    if ((high_thr > 255) || (high_thr < (-256)) ||(low_thr>255) || (low_thr< (-256))) {
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    captouch_set_threshold(channel, high_thr, low_thr);

    return HAL_CAPTOUCH_STATUS_OK;
}

#ifdef HAL_CPT_FEATURE_4CH
hal_captouch_status_t hal_captouch_set_Nthreshold(hal_captouch_channel_t channel,int32_t high_thr, int32_t low_thr)
{
    if (channel >= HAL_CAPTOUCH_CHANNEL_MAX) {
        return HAL_CAPTOUCH_STATUS_CHANNEL_ERROR;
    }

    if ((high_thr > 255) || (high_thr < (-256)) ||(low_thr>255) || (low_thr< (-256))) {
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    captouch_set_nthreshold(channel, high_thr, low_thr);

    return HAL_CAPTOUCH_STATUS_OK;
}
#endif

hal_captouch_status_t hal_captouch_set_fine_cap(hal_captouch_channel_t channel,int32_t fine)
{
    if (channel >= HAL_CAPTOUCH_CHANNEL_MAX) {
        return HAL_CAPTOUCH_STATUS_CHANNEL_ERROR;
    }

    if ((fine > 63) || (fine<(-64))) {
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    captouch_set_fine_cap(channel,(int8_t)fine);
    return HAL_CAPTOUCH_STATUS_OK;
}

hal_captouch_status_t hal_captouch_set_coarse_cap(hal_captouch_channel_t channel, uint32_t coarse)
{
    if (channel >= HAL_CAPTOUCH_CHANNEL_MAX) {
        return HAL_CAPTOUCH_STATUS_CHANNEL_ERROR;
    }

    if (coarse > CAPTOUCH_COARSE_CAP_MAX) {
        return HAL_CAPTOUCH_STATUS_INVALID_PARAMETER;
    }

    captouch_set_coarse_cap(channel,(uint8_t)coarse);

    return HAL_CAPTOUCH_STATUS_OK;
}


hal_captouch_status_t hal_captouch_tune_control(hal_captouch_channel_t channel,hal_captouch_tune_type_t tune_type, hal_captouch_tune_data_t* data)
{
    hal_captouch_status_t ret;
    uint32_t count;
    UNUSED(tune_type);
    ret =0;
    count = 0;
    while(1) {
        ++count;
        ret = captouch_hw_auto_tune(channel,data);
        if(ret)break;
        if(count>=3)break;
    }
    //captouch_write_nvkey(CAPTOUCH_COARSE_CAP_ID,channel,data->coarse_cap);
    return ret;
}


#if 0
hal_captouch_status_t hal_captouch_tune_control(hal_captouch_channel_t channel,hal_captouch_tune_type_t tune_type, hal_captouch_tune_data_t* data)
{
    hal_captouch_status_t ret;

    if (channel >= HAL_CAPTOUCH_CHANNEL_MAX) {
        return HAL_CAPTOUCH_STATUS_CHANNEL_ERROR;
    }

    if (data == NULL) {
        return HAL_CAPTOUCH_STATUS_ERROR;
    }

    if (captouch_context.has_initilized != true) {
        return HAL_CAPTOUCH_STATUS_UNINITIALIZED;
    }

    ret = HAL_CAPTOUCH_STATUS_ERROR;

    hal_nvic_disable_irq(CAP_TOUCH_IRQn);
    captouch_set_autok_suspend(captouch_context.used_channel_map,false);

    //mask interrupt & wakeup
    captouch_channel_int_control(1<<channel, captouch_disable);
    hal_gpt_delay_ms(2);

    captouch_switch_debug_sel(channel);

    if (tune_type == HAL_CAPTOUCH_TUNE_HW_AUTO) {
        if (captouch_hw_auto_tune(channel,data) == true) {
            ret = HAL_CAPTOUCH_STATUS_OK;
        }
    }
    else if(tune_type == HAL_CAPTOUCH_TUNE_SW_AUTO) {
        if (captouch_sw_auto_tune(channel,data) == true) {
            ret = HAL_CAPTOUCH_STATUS_OK;
        }
    }

    captouch_channel_int_control(1<<channel, captouch_enable);
    hal_gpt_delay_ms(2);

    captouch_hif->TOUCH_HIF_CON0.CELLS.HIF_SOFT_RST = 1;
    hal_gpt_delay_ms(3);

    hal_nvic_enable_irq(CAP_TOUCH_IRQn);
    captouch_set_autok_suspend(captouch_context.used_channel_map,true);

    return ret;
}
#endif
hal_captouch_status_t hal_captouch_lowpower_control(hal_captouch_lowpower_type_t lowpower_type)
{
    if (captouch_context.has_initilized != true) {
        return HAL_CAPTOUCH_STATUS_UNINITIALIZED;
    }

    captouch->TOUCH_CON0.CON0 = 0;

    if (HAL_CAPTOUCH_MODE_NORMAL == lowpower_type) {
        captouch_rtc_lpm_control(HAL_CAPTOUCH_MODE_NORMAL);
        captouch->TOUCH_ANACFG0.ANACFG0 = (1<<16) | 0x2e;
        captouch_set_clk(HAL_CAPTOUCH_MODE_NORMAL,HAL_CAPTOUCH_CLK_32K);
        captouch_channel_sense_control(captouch_context.used_channel_map, captouch_enable);
        captouch_clk_control(true);
        captouch_int_control(true);
    }
    else if (HAL_CAPTOUCH_MODE_LOWPOWER == lowpower_type) {
        captouch_rtc_lpm_control(HAL_CAPTOUCH_MODE_LOWPOWER);
        captouch->TOUCH_ANACFG0.ANACFG0 = (1<<16) | 0x3f;
        captouch_set_clk(HAL_CAPTOUCH_MODE_NORMAL,HAL_CAPTOUCH_CLK_4K);
        hal_captouch_set_avg(HAL_CAPTOUCH_CHANNEL_0, 5, 5);
        captouch_clk_control(true);
        captouch_int_control(true);
        hal_gpt_delay_ms(200);
        hal_captouch_set_avg(HAL_CAPTOUCH_CHANNEL_0,captouch_context.mavg_r[0]-3,captouch_context.avg_s[0]);
        captouch_set_autok_suspend(0x1,true);
    }

    return HAL_CAPTOUCH_STATUS_OK;

}

hal_captouch_status_t hal_captouch_deinit(void)
{
    captouch_analog_deinit();
    captouch_rtc_clk_control(captouch_disable);

    //disable channel interrupt
    captouch_channel_int_control(captouch_context.used_channel_map, captouch_disable);

    //disable channel calibration
    captouch_channel_sense_control(captouch_context.used_channel_map, captouch_disable);

    //close the captouch clock
    captouch_clk_control(captouch_disable);

    //disable the captouch inerrupt
    captouch_int_control(captouch_disable);

    captouch_context.has_initilized = false;

    memset(&captouch_context, 0, sizeof(captouch_context_t));

    return HAL_CAPTOUCH_STATUS_OK;
}




#endif //HAL_CAPTOUCH_MODULE_ENABLED

