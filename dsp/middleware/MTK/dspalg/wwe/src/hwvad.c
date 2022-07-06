/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#ifdef MTK_WWE_ENABLE

#include "hwvad.h"
#include "dtm.h"
#include "dsp_vow.h"
#include "dsp_nvkey_vow_para.h"
#include "dsp_scenario.h"

hal_audio_device_parameter_vow_t hwvad_vow_control;
extern volatile wwe_state_t g_wwe_state;
//extern hal_audio_device_parameter_vow_t *dsp_vow;

/*VAD driver*/
void hwvad_dsp_vow_isr_handler(void)
{
    //vow_disable(dsp_vow, NULL);
    DTM_enqueue(DTM_EVENT_ID_VOW_DISABLE, 0, true);
    wwe_hwvad_resume();
}

extern CONNECTION_IF record_if;

HWVAD_STA hwvad_init(hwvad_config_t config)
{
    uint32_t snr_threshold = 0,noise_ignore_bit = 0;
    //uint8_t alpha_rise = 0,enable = 0;

    memset(&hwvad_vow_control, 0, sizeof(hal_audio_device_parameter_vow_t));
    hwvad_vow_control.audio_device = HAL_AUDIO_CONTROL_DEVICE_VOW;
    hwvad_vow_control.dma_irq_threshold = VOW_SRAM_COPY_SIZE;
    #if 0
    hwvad_vow_control.snr_threshold = 0x1313;
    hwvad_vow_control.noise_ignore_bit = 0xF0000000;//0xFF000000 for 1-mic is ok, but for dual mic is not so good, 0xF0000000 is better for both 1-mic and dual-mic
    #else
    switch(config.hwvad_vow_para.snr_threshold) {
        case VOW_SNR_THRESHOLD_0303:
            snr_threshold = 0x0303;
            break;
        case VOW_SNR_THRESHOLD_1313:
            snr_threshold = 0x1313;
            break;
        case VOW_SNR_THRESHOLD_2323:
            snr_threshold = 0x2323;
            break;
        case VOW_SNR_THRESHOLD_3333:
            snr_threshold = 0x3333;
            break;
        case VOW_SNR_THRESHOLD_4343:
            snr_threshold = 0x4343;
            break;
        case VOW_SNR_THRESHOLD_5353:
            snr_threshold = 0x5353;
            break;
        case VOW_SNR_THRESHOLD_6363:
            snr_threshold = 0x6363;
            break;
        case VOW_SNR_THRESHOLD_7373:
            snr_threshold = 0x7373;
            break;
        default:
            DSP_MW_LOG_E("[HWVAD] snr_threshold param wrong!", 0);
            break;

    }
    switch(config.hwvad_vow_para.noise_ignore_bits) {
        case VOW_NOISE_IGNORE_BITS_FFFF0000:
            noise_ignore_bit = 0xFFFF0000;
            break;
        case VOW_NOISE_IGNORE_BITS_FFF00000:
            noise_ignore_bit = 0xFFF00000;
            break;
        case VOW_NOISE_IGNORE_BITS_FF000000:
            noise_ignore_bit = 0xFF000000;
            break;
        case VOW_NOISE_IGNORE_BITS_F0000000:
            noise_ignore_bit = 0xF0000000;
            break;
        default:
            DSP_MW_LOG_E("[HWVAD] noise_ignore_bits param wrong!", 0);
            break;

    }
    hwvad_vow_control.snr_threshold = snr_threshold;
    hwvad_vow_control.noise_ignore_bit = noise_ignore_bit;
    hwvad_vow_control.alpha_rise = config.hwvad_vow_para.alpha_rise;
    #endif
    hwvad_vow_control.dmic_selection = HAL_AUDIO_DMIC_GPIO_DMIC0;
    hwvad_vow_control.suspend_mic = true;
    hwvad_vow_control.input_device = HAL_AUDIO_CONTROL_DEVICE_VOW;
    /*analysis the mic selection*/
    if ((config.hwvad_vow_para.main_mic != HAL_AUDIO_CONTROL_NONE) && (config.hwvad_vow_para.ref_mic != HAL_AUDIO_CONTROL_NONE)) {
        /*dual mic*/
        if (config.hwvad_vow_para.main_mic <= HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL) {
            /*dual analog mic*/
            hwvad_vow_control.mic_selection = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL;
        }else{
            /*dual digial mic*/
            hwvad_vow_control.mic_selection = HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL;
        }
    }else {
        /*single mic*/
        if (config.hwvad_vow_para.main_mic != HAL_AUDIO_CONTROL_NONE) {
            hwvad_vow_control.mic_selection = (hal_audio_control_t)config.hwvad_vow_para.main_mic;
        }else {
            hwvad_vow_control.mic_selection = (hal_audio_control_t)config.hwvad_vow_para.ref_mic;
        }
    }

    hwvad_vow_control.bias_select = HAL_AUDIO_BIAS_SELECT_BIAS0;
    hwvad_vow_control.bias_voltage[0] = HAL_AUDIO_BIAS_VOLTAGE_1_85V;
    //hwvad_vow_control.mic_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
    if(record_if.source!=NULL){
         hwvad_vow_control.mic_interface = record_if.source->param.audio.audio_interface;
         DSP_MW_LOG_I("[HWVAD] record_if %d",1,record_if.source->param.audio.audio_interface);
    }else{
        hwvad_vow_control.mic_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
    }


    hwvad_vow_control.memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
    hwvad_vow_control.adc_parameter.performance = AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE;
    hwvad_vow_control.rate = 16000;
    hwvad_vow_control.vow_detection_done_entry = hwvad_dsp_vow_isr_handler;
    hwvad_vow_control.vow_mode = AFE_VOW_PHASE0;
    hwvad_vow_control.vow_with_hpf = true;
    hwvad_vow_control.adc_parameter.adc_mode = config.adda_analog_mic_mode;

    /*open the vow power by cm4*/
    DSP_MW_LOG_I("[HWVAD] init OK,adda_analog_mic_mode = %d", 1,hwvad_vow_control.adc_parameter.adc_mode);
    return HWVAD_OK;
}

HWVAD_STA hwvad_deinit(void)
{
    memset(&hwvad_vow_control, 0, sizeof(hal_audio_device_parameter_vow_t));
    DSP_MW_LOG_I("[HWVAD]deinit OK", 0);

    return HWVAD_OK;
}

HWVAD_STA hwvad_enable(void)
{
    //vow_enable(&hwvad_vow_control, NULL);
    DTM_enqueue(DTM_EVENT_ID_VOW_ENABLE, 0, false);
    DSP_MW_LOG_I("[HWVAD]enable OK", 0);

    return HWVAD_OK;
}

HWVAD_STA hwvad_disable(void)
{
    vow_disable(&hwvad_vow_control, NULL);
    DSP_MW_LOG_I("[HWVAD]disable OK", 0);

    return HWVAD_OK;
}


#endif
