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

#include "config.h"
//#include "os.h"
#include "common.h"
//#include "Nvkey_DspFw.h"
#include "stream_audio_setting.h"
//#include "nvkey.h"
//#include "nvkey_list.h"

#include "dsp_vad.h"
#include "hal_audio.h"
//#include "dsp_para_afe_vad_nvstruc.h"
#include "stream_audio_setting.h"
#include "stream_audio_driver.h"
//#include "DSP_CommandDispatcher.h"

/******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/



/******************************************************************************
 * Function Declaration
 ******************************************************************************/
void dsp_vad_isr_handler(void);



/******************************************************************************
 * Variables
 ******************************************************************************/
#if (FEA_SUPP_DSP_VAD)
dsp_vad_control_t           *dsp_vad, dsp_vad_ctrl;
dsp_afe_vad_status_t vad_nvkey;
#endif

/******************************************************************************
 * Function
 ******************************************************************************/
void dsp_vad_init(void)
{
#if (FEA_SUPP_DSP_VAD)
    hal_audio_device_parameter_vad_t    *vad_parameter;
    hal_audio_irq_parameter_t           register_irq_handler;
    dsp_afe_vad_status_t *vad_nvkey = &vad_nvkey;

    dsp_vad = (dsp_vad_control_t  *)&dsp_vad_ctrl;//pvPortMalloc(sizeof(dsp_vad));
    if (dsp_vad) {
        LOG_PRINT_AUDIO("DSP VAD dsp_vad_init ", 0);
        //dsp_vad->semaphore_ptr =  &(dsp_vad->semaphore);
        OS_Semaphore_Init(dsp_vad->semaphore_ptr, 1);
        OS_Semaphore_Take(dsp_vad->semaphore_ptr);

        vad_parameter =&dsp_vad->vad_parameter;

        vad_parameter->audio_device = HAL_AUDIO_CONTROL_DEVICE_VAD;
        vad_parameter->bias_select = HAL_AUDIO_BIAS_SELECT_BIAS0;//stream_audio_convert_get_bias_selection(AU_AFE_MIC_BIAS_WWE_SHIFT);
        vad_parameter->bias_voltage[0] = HAL_AUDIO_BIAS_VOLTAGE_1_85V;//stream_audio_convert_get_bias_voltage(vad_parameter->bias_select);
        vad_parameter->with_bias_lowpower = true;

        vad_parameter->threshold_0 = 7;//vad_nvkey->threshold_0;
        vad_parameter->threshold_1 = 0x0200;//vad_nvkey->threshold_1;
        vad_parameter->input_sel = 0;//vad_nvkey->input_sel;
        vad_parameter->amp_gain = 0x10;//vad_nvkey->amp_gain;


        register_irq_handler.audio_irq = HAL_AUDIO_IRQ_VAD;
        register_irq_handler.entry = dsp_vad_isr_handler;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&register_irq_handler, HAL_AUDIO_SET_IRQ_HANDLER);

        dsp_vad->source_type = SOURCE_TYPE_AUDIO;
        dsp_vad->pause_audio_source = false;
        dsp_vad->enable = false;

        OS_Semaphore_Give(dsp_vad->semaphore_ptr);
    } else {
        LOG_PRINT_AUDIO("DSP VAD Malloc Fail ", 0);
        OS_ASSERT(0);
    }
#endif
}

void dsp_vad_close(void)
{
#if (FEA_SUPP_DSP_VAD)
    if(dsp_vad) {
        dsp_vad = 0;
    }
#endif
}

void dsp_vad_detect_handler(void)
{
    dsp_vad_enable(false, true);
}

void dsp_vad_isr_handler(void)
{
    #if (FEA_SUPP_DSP_VAD)
    DSP_CommandVadDetect(Source_blks[dsp_vad->source_type]);
    LOG_PRINT_AUDIO("DSP HWVAD detected isr handler", 0);
    #endif
}

void dsp_vad_enable(bool enable, bool suspend_isr)
{
#if (FEA_SUPP_DSP_VAD)
    hal_audio_trigger_start_parameter_t memory_start_parameter;
    hal_audio_vad_start_parameter_t     vad_start;

    if(!dsp_vad) {
        dsp_vad_init();
    }

    //OS_Semaphore_Take(dsp_vad->semaphore_ptr);
    if (enable ^ dsp_vad->enable) {
        dsp_vad->enable = enable;
        memory_start_parameter.memory_select = HAL_AUDIO_MEMORY_UL_VUL1;//Audio_setting->Audio_sink.memory.memory_select;
        if (enable) {
            //Enable VAD alanog for precharge
            hal_audio_set_device((hal_audio_device_parameter_t *)&dsp_vad->vad_parameter, dsp_vad->vad_parameter.audio_device, HAL_AUDIO_CONTROL_ON);

            //Suspend afe input
            if (Source_blks[dsp_vad->source_type]) {
                if (suspend_isr) {
                    memory_start_parameter.enable = false;
                    hal_audio_set_value((hal_audio_set_value_parameter_t *)&memory_start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
                }
                //hal_audio_set_device(&Audio_setting->Audio_source.device, Audio_setting->Audio_source.device.common.audio_device, HAL_AUDIO_CONTROL_OFF);
                dsp_vad->pause_audio_source = true;
            } else {
                dsp_vad->pause_audio_source = false;
            }

            //Start VAD
            vad_start.enable = true;
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&vad_start, HAL_AUDIO_SET_DEVICE_VAD_START);

        } else {
            //Stop VAD
            vad_start.enable = false;
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&vad_start, HAL_AUDIO_SET_DEVICE_VAD_START);

            //Resume afe input
            if (dsp_vad->pause_audio_source) {
                //hal_audio_set_device(&Audio_setting->Audio_source.device, Audio_setting->Audio_source.device.common.audio_device, HAL_AUDIO_CONTROL_ON);
                if (suspend_isr && Source_blks[dsp_vad->source_type]) {
                    //stream_audio_source_reset_offset(Source_blks[dsp_vad->source_type], 0);
                    memory_start_parameter.enable = true;
                    hal_audio_set_value((hal_audio_set_value_parameter_t *)&memory_start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
                }
                dsp_vad->pause_audio_source = false;
            }

            //Disable VAD alanog
            hal_audio_set_device((hal_audio_device_parameter_t *)&dsp_vad->vad_parameter, dsp_vad->vad_parameter.audio_device, HAL_AUDIO_CONTROL_OFF);
        }
        LOG_PRINT_AUDIO("DSP VAD enable:%d suspend_isr:%d ", 2, enable, suspend_isr);
    }
    //OS_Semaphore_Give(dsp_vad->semaphore_ptr);

#else
    UNUSED(enable);
    UNUSED(suspend_isr);
#endif
}

bool dsp_vad_get_status(void)
{
#if (FEA_SUPP_DSP_VAD)
    return (dsp_vad) && (dsp_vad->enable);
#else
    return false;
#endif
}

