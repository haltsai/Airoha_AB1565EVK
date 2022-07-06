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
#include <stdio.h>
#include <string.h>
#include "source.h"
#include "stream_audio_setting.h"
#include "hal_nvic.h"
#include "FreeRTOS.h"
#include "stream_audio_driver.h"
#include "dsp_callback.h"
#include "dsp_audio_ctrl.h"
#include "hal_audio_afe_control.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_clock.h"
#include "audio_afe_common.h"
#include "dsp_vow.h"
#include "dtm.h"

/******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/
#define FEA_SUPP_DSP_VOW 1
#define SKIP_VOW 0

/******************************************************************************
 * Function Declaration
 ******************************************************************************/
void dsp_vow_isr_handler(void);

/******************************************************************************
 * Variables
 ******************************************************************************/
#if (FEA_SUPP_DSP_VOW)
hal_audio_device_parameter_vow_t *dsp_vow,dsp_vow_ctrl;
hal_audio_device_parameter_t device_handle_in_g;
bool is_enable = false;
#endif
#if (HAL_AUDIO_VOW_DEBUG)
extern HAL_AUDIO_TIMER_HANDLE      vow_debug_timer_handle;
#endif

/******************************************************************************
 * Function
 ******************************************************************************/
 extern void Source_device_set_para(hal_audio_device_parameter_t *device_handle);
void dsp_vow_init(void)
{
#if (FEA_SUPP_DSP_VOW)
    dsp_vow = (hal_audio_device_parameter_vow_t    *)&dsp_vow_ctrl;//pvPortMalloc(sizeof(dsp_vad));
    if (dsp_vow) {
        DSP_MW_LOG_I("DSP VOW init ", 0);
        memset(dsp_vow,0,sizeof(hal_audio_device_parameter_vow_t));
        dsp_vow->audio_device = HAL_AUDIO_CONTROL_DEVICE_VOW;
        dsp_vow->dma_irq_threshold = VOW_SRAM_COPY_SIZE;
        dsp_vow->snr_threshold = 0x7373;
        dsp_vow->alpha_rise = 0x7;
        dsp_vow->noise_ignore_bit = 0xFF000000;
        dsp_vow->dmic_selection = HAL_AUDIO_DMIC_GPIO_DMIC0;
        dsp_vow->suspend_mic = false;
        dsp_vow->input_device = HAL_AUDIO_CONTROL_DEVICE_VOW;
        dsp_vow->mic_selection = HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL;
        dsp_vow->bias_select = HAL_AUDIO_BIAS_SELECT_BIAS0;
        dsp_vow->bias_voltage[0] = HAL_AUDIO_BIAS_VOLTAGE_1_85V;
        dsp_vow->mic_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        dsp_vow->memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
        dsp_vow->adc_parameter.performance = AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE;
        dsp_vow->rate = 16000;
        dsp_vow->vow_detection_done_entry = dsp_vow_isr_handler;
        dsp_vow->vow_mode = AFE_VOW_PHASE0;
        dsp_vow->vow_with_hpf = true;
        #if 0
        hal_audio_irq_parameter_t           register_irq_handler;
        register_irq_handler.audio_irq = HAL_AUDIO_VOW_SNR;
        register_irq_handler.entry = dsp_vow_isr_handler;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&register_irq_handler, HAL_AUDIO_SET_IRQ_HANDLER);
        #endif
    } else {
        DSP_MW_LOG_I("DSP VOW Malloc Fail ", 0);
        OS_ASSERT(0);
    }
#endif
}

void dsp_vow_isr_handler(void)
{
#if (FEA_SUPP_DSP_VOW)
    DSP_MW_LOG_I("DSP VOW detected isr handler suspend_mic %d", 1,dsp_vow->suspend_mic);
    vow_disable(dsp_vow,NULL);
    //vow_enable(dsp_vow,NULL);
#endif
}

int vow_enable(hal_audio_device_parameter_vow_t *vow,SOURCE source)
{
    UNUSED(source);
    UNUSED(vow);
#if (FEA_SUPP_DSP_VOW)
    HAL_AUDIO_ENTER_CRITICAL();//disable irq
    if(is_enable) {
        HAL_AUDIO_EXIT_CRITICAL();//enable irq
        return -1;
    } else {
        is_enable = true;
    }
    HAL_AUDIO_EXIT_CRITICAL();//enable irq
    #if 0
    if(!dsp_vow){
        dsp_vow_init();
    }else{
        dsp_vow = vow;
    }
    #else
    if(!vow){
        dsp_vow_init();
        DSP_MW_LOG_I("VOW set default parameters",0);
    }else{
        dsp_vow = vow;
    }
    #endif
    #if SKIP_VOW
    DTM_enqueue(DTM_EVENT_ID_VOW_DISABLE, 0, false);
    wwe_hwvad_resume();
    DSP_MW_LOG_I("Skip VOW enable",0);
    return 0;
    #endif

    DSP_MW_LOG_I("DSP VOW_enable suspend_mic %d,mic_interface %d,mic_selection %d", 3,dsp_vow->suspend_mic,dsp_vow->mic_interface,dsp_vow->mic_selection);
    printf("DSP VOW_enable suspend_mic %d,mic_interface %d,mic_selection %d\r\n",dsp_vow->suspend_mic,dsp_vow->mic_interface,dsp_vow->mic_selection);
    hal_audio_device_parameter_t device_handle_in;
    hal_audio_get_value_parameter_t get_val_handle;
    hal_audio_set_value_parameter_t handle_set_value;
    memset(&device_handle_in,0,sizeof(device_handle_in));
    get_val_handle.get_control_count.audio_control =  dsp_vow->mic_selection;

    device_handle_in.common.rate = dsp_vow->rate;
    //keep mic bias
    handle_set_value.mic_bias.bias1_2_with_LDO0 = false;
    handle_set_value.mic_bias.bias_select = dsp_vow->bias_select;
    handle_set_value.mic_bias.bias_voltage[0] = dsp_vow->bias_voltage[0];
    handle_set_value.mic_bias.enable = true;
    handle_set_value.mic_bias.with_bias_lowpower = true;
    hal_audio_set_value(&handle_set_value, HAL_AUDIO_SET_DEVICE_MIC_BIAS);

    hal_audio_irq_parameter_t           register_irq_handler;
    register_irq_handler.audio_irq = HAL_AUDIO_VOW_SNR;
    register_irq_handler.entry = dsp_vow->vow_detection_done_entry;
    hal_audio_set_value((hal_audio_set_value_parameter_t *)&register_irq_handler, HAL_AUDIO_SET_IRQ_HANDLER);

    if(!(hal_audio_get_value(&get_val_handle, HAL_AUDIO_GET_CONTROL_COUNT)) && dsp_vow->suspend_mic!=true){
        #if 0
        device_handle_in->common.audio_device = dsp_vow->mic_selection;
        Source_device_set_para(device_handle_in);
        if((dsp_vow->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL){
            device_handle_in->analog_mic.rate = 48000;//AUDIO_SOURCE_DEFAULT_ANALOG_VOICE_RATE;
            device_handle_in->analog_mic.mic_interface = dsp_vow->input_interface;
            device_handle_in->analog_mic.bias_voltage = HAL_AUDIO_BIAS_VOLTAGE_1_85V;
            device_handle_in->analog_mic.bias_select = HAL_AUDIO_BIAS_SELECT_ALL;
            device_handle_in->analog_mic.iir_filter = HAL_AUDIO_UL_IIR_5HZ_AT_48KHZ;
            device_handle_in->analog_mic.with_external_bias = false;
            device_handle_in->analog_mic.with_bias_lowpower = false;
            device_handle_in->analog_mic.bias1_2_with_LDO0 = false;
            device_handle_in->analog_mic.adc_parameter.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;
            device_handle_in->analog_mic.adc_parameter.performance = dsp_vow->performance;
        }
        if((dsp_vow->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL){
            device_handle_in->digital_mic.rate = 48000;//AUDIO_SOURCE_DEFAULT_ANALOG_VOICE_RATE;
            device_handle_in->digital_mic.mic_interface = dsp_vow->input_interface;
            device_handle_in->digital_mic.dmic_selection = dsp_vow->dmic_selection;
            device_handle_in->digital_mic.adc_parameter.performance = dsp_vow->performance;
            device_handle_in->digital_mic.bias_voltage = HAL_AUDIO_BIAS_VOLTAGE_1_85V;
            device_handle_in->digital_mic.bias_select = HAL_AUDIO_BIAS_SELECT_ALL;
            device_handle_in->digital_mic.iir_filter = HAL_AUDIO_UL_IIR_DISABLE;
            device_handle_in->digital_mic.with_external_bias = false;
            device_handle_in->digital_mic.with_bias_lowpower = false;
            device_handle_in->digital_mic.bias1_2_with_LDO0 = false;
        }
        hal_audio_set_device(device_handle_in,dsp_vow->mic_selection, HAL_AUDIO_CONTROL_ON);
        #endif
        hal_audio_set_device((hal_audio_device_parameter_t *)dsp_vow,dsp_vow->input_device, HAL_AUDIO_CONTROL_ON);
    }else{
        if(dsp_vow->suspend_mic){
            device_handle_in.common.audio_device = dsp_vow->mic_selection;
            device_handle_in.common.device_interface = dsp_vow->mic_interface;
            device_handle_in.common.rate = dsp_vow->rate;
            Source_device_set_para(&device_handle_in);
            //Suspend MEM
            hal_audio_trigger_start_parameter_t memory_start_parameter;
            if((dsp_vow->memory_select)!=NULL){
                memory_start_parameter.memory_select = dsp_vow->memory_select;//hal_memory_convert_ul(dsp_vow->memory_select);//HAL_AUDIO_MEMORY_UL_VUL1;
                memory_start_parameter.enable = false;
                hal_audio_set_value((hal_audio_set_value_parameter_t *)&memory_start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
            }

            //Suspend MIC
            DSP_MW_LOG_I("DSP VOW Suspend MIC mic_selection %d,device_interface %d", 2,dsp_vow->mic_selection,device_handle_in.common.device_interface);
            hal_audio_set_device(&device_handle_in,dsp_vow->mic_selection, HAL_AUDIO_CONTROL_OFF);
            //Enable VOW
            if((dsp_vow->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL){
                device_handle_in.digital_mic.adc_parameter.performance = AFE_PEROFRMANCE_LOW_POWER_MODE;
                hal_audio_set_device((hal_audio_device_parameter_t *)dsp_vow,dsp_vow->input_device, HAL_AUDIO_CONTROL_ON);
            }else if((dsp_vow->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL){
                device_handle_in.analog_mic.adc_parameter.performance = AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE;
                hal_audio_set_device((hal_audio_device_parameter_t *)dsp_vow,dsp_vow->input_device, HAL_AUDIO_CONTROL_ON);
            }else{
                HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio VOW Wrong Device :%d !", 1, dsp_vow->mic_selection);
                HAL_AUDIO_ASSERT(false);
            }
        }else{
            if((dsp_vow->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL){
                device_handle_in.analog_mic.adc_parameter.performance = dsp_vow->adc_parameter.performance;
                hal_audio_set_device((hal_audio_device_parameter_t *)dsp_vow,dsp_vow->input_device, HAL_AUDIO_CONTROL_ON);
            }else{
                HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio VOW concurrent mode only for AMIC :%d !", 1, dsp_vow->mic_selection);
                HAL_AUDIO_ASSERT(false);
            }
        }
    }

#else
    UNUSED(vow);
    UNUSED(source);
#endif

#if (HAL_AUDIO_VOW_DEBUG)
    HAL_AUDIO_TIMER_START(vow_debug_timer_handle, HAL_AUDIO_VOW_STABLE_TIMER_MS*10);
#endif
    return 0;
}

int vow_disable(hal_audio_device_parameter_vow_t *vow,SOURCE source)
{
    UNUSED(vow);
    UNUSED(source);

#if (FEA_SUPP_DSP_VOW)
    HAL_AUDIO_ENTER_CRITICAL();//disable irq
    if(is_enable) {
        is_enable = false;
    } else {
        HAL_AUDIO_EXIT_CRITICAL();//enable irq
        return -1;
    }
    HAL_AUDIO_EXIT_CRITICAL();//enable irq
    if(!dsp_vow){
        return true;
    }
    #if SKIP_VOW
    dsp_vow = NULL;
    DSP_MW_LOG_I("Skip VOW Disable",0);
    return 0;
    #endif
    hal_audio_device_parameter_t device_handle_in;
    hal_audio_set_value_parameter_t handle_set_value;
    hal_audio_trigger_start_parameter_t memory_start_parameter;
    memset(&device_handle_in,0,sizeof(device_handle_in));
    DSP_MW_LOG_I("DSP VOW_disable suspend_mic %d,mic_interface %d,mic_selection %d\r\n", 3,dsp_vow->suspend_mic,dsp_vow->mic_interface,dsp_vow->mic_selection);
    if(dsp_vow->suspend_mic){
        if((dsp_vow->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL){
            device_handle_in.digital_mic.adc_parameter.performance = AFE_PEROFRMANCE_LOW_POWER_MODE;
            hal_audio_set_device((hal_audio_device_parameter_t *)dsp_vow,dsp_vow->input_device, HAL_AUDIO_CONTROL_OFF);
        }else if((dsp_vow->mic_selection)&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL){
            device_handle_in.analog_mic.adc_parameter.performance = AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE;
            hal_audio_set_device((hal_audio_device_parameter_t *)dsp_vow,dsp_vow->input_device, HAL_AUDIO_CONTROL_OFF);
        }else{
            HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio VOW Wrong Device :%d !", 1, dsp_vow->mic_selection);
            HAL_AUDIO_ASSERT(false);
        }
        device_handle_in.common.audio_device = dsp_vow->mic_selection;
        device_handle_in.common.device_interface = dsp_vow->mic_interface;
        device_handle_in.common.rate = dsp_vow->rate;
        Source_device_set_para(&device_handle_in);
        //Resume MIC
        DSP_MW_LOG_I("DSP VOW Resume MIC mic_selection %d,device_interface %d",2,dsp_vow->mic_selection,device_handle_in.common.device_interface);
        hal_audio_set_device(&device_handle_in,dsp_vow->mic_selection, HAL_AUDIO_CONTROL_ON);
        //Suspend MEM
        if((dsp_vow->memory_select)!=NULL){
            memory_start_parameter.memory_select = dsp_vow->memory_select;//hal_memory_convert_ul(dsp_vow->memory_select);//HAL_AUDIO_MEMORY_UL_VUL1;
            memory_start_parameter.enable = true;
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&memory_start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        }

    }else{
        hal_audio_set_device((hal_audio_device_parameter_t *)dsp_vow,dsp_vow->input_device, HAL_AUDIO_CONTROL_OFF);
    }
    //Disable keep mic bias
    handle_set_value.mic_bias.bias1_2_with_LDO0 = false;
    handle_set_value.mic_bias.bias_select = dsp_vow->bias_select;
    handle_set_value.mic_bias.bias_voltage[0] = dsp_vow->bias_voltage[0];
    handle_set_value.mic_bias.enable = false;
    handle_set_value.mic_bias.with_bias_lowpower = true;
    hal_audio_set_value(&handle_set_value, HAL_AUDIO_SET_DEVICE_MIC_BIAS);
#else
    UNUSED(vow);
    UNUSED(source);
#endif
    dsp_vow = NULL;
#if (HAL_AUDIO_VOW_DEBUG)
        HAL_AUDIO_TIMER_STOP(vow_debug_timer_handle);
#endif

    return 0;
}

extern hal_audio_vow_control_t      vow_control;
extern afe_t afe;
/*
use vow_sink before vow_enable

*/
int vow_sink(hal_audio_device_parameter_vow_t *vow,SOURCE source){
    UNUSED(source);
    UNUSED(vow);
    hal_audio_memory_parameter_t mem_handle;
    hal_audio_path_parameter_t path_handle;
    hal_audio_control_t memory_interface = HAL_AUDIO_CONTROL_MEMORY_INTERFACE;
    hal_audio_device_parameter_t device_handle_out;
    unsigned int i = 0;
    DSP_MW_LOG_I("DSP vow_sink", 0);
    memset(&mem_handle,0,sizeof(hal_audio_memory_parameter_t));
    memset(&path_handle,0,sizeof(hal_audio_path_parameter_t));
    memset(&device_handle_out,0,sizeof(hal_audio_device_parameter_t));

    mem_handle.audio_path_rate = 16000;
    mem_handle.buffer_length = 32768;//1280;//16K*2ch*2byte*20ms
    mem_handle.buffer_addr = (uint32_t)NULL;
    mem_handle.initial_buffer_offset = 0;
    mem_handle.irq_counter = 320;//16KHz 10ms;
    mem_handle.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
    mem_handle.pcm_format = HAL_AUDIO_PCM_FORMAT_S16_LE;
    mem_handle.pure_agent_with_src = false;
    mem_handle.sync_status = HAL_AUDIO_MEMORY_SYNC_NONE;

    hal_audio_set_memory(&mem_handle, memory_interface, HAL_AUDIO_CONTROL_ON);
    if(mem_handle.buffer_addr != (uint32_t)NULL){
        memset((U8*)mem_handle.buffer_addr,0x0,mem_handle.buffer_length);
    }else{
        DSP_MW_LOG_I("mem_handle.buffer_addr 0x%x\r\n",1,mem_handle.buffer_addr);
    }
    DSP_MW_LOG_I("DSP vow_sink set mem done", 0);
    device_handle_out.common.audio_device = HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER;

    if (device_handle_out.common.audio_device & HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL) {
        device_handle_out.dac.audio_device = HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL;
        device_handle_out.dac.rate = mem_handle.audio_path_rate;
        device_handle_out.dac.dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
        device_handle_out.dac.dc_compensation_value = afe.stream_out.dc_compensation_value;
        device_handle_out.dac.with_high_performance = false;
        device_handle_out.dac.with_phase_inverse = false;
        device_handle_out.dac.with_force_change_rate = false;

    } else if ((device_handle_out.common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER) || (device_handle_out.common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L) || (device_handle_out.common.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
        device_handle_out.i2s_master.rate = mem_handle.audio_path_rate;
        device_handle_out.i2s_master.i2s_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        device_handle_out.i2s_master.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle_out.i2s_master.word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
        device_handle_out.i2s_master.mclk_divider = 2;
        device_handle_out.i2s_master.with_mclk = false;
        device_handle_out.i2s_master.is_low_jitter = false;
        device_handle_out.i2s_master.is_recombinant = false;
    }
    hal_audio_set_device(&device_handle_out, device_handle_out.dac.audio_device, HAL_AUDIO_CONTROL_ON);
    DSP_MW_LOG_I("DSP vow_sink set device done", 0);
    path_handle.audio_path_input_rate = mem_handle.audio_path_rate;//afe_get_audio_device_samplerate(pAudPara->audio_device, pAudPara->audio_interface);
    path_handle.audio_path_output_rate = mem_handle.audio_path_rate;//afe_get_audio_device_samplerate(pAudPara->audio_device, pAudPara->audio_interface);
    path_handle.connection_selection = HAL_AUDIO_INTERCONN_CH01CH02_to_CH01CH02;//pAudPara->stream_channel;
    path_handle.connection_number = 2;

    hal_audio_path_port_parameter_t input_port_parameters, output_port_parameters;
    //input_port_parameters.device_interface = param->in_interface;
    input_port_parameters.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
    output_port_parameters.device_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;

    for (i=0 ; i<path_handle.connection_number ; i++) {
    path_handle.input.interconn_sequence[i]  = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, input_port_parameters, i, true);
    path_handle.output.interconn_sequence[i] = stream_audio_convert_control_to_interconn(device_handle_out.analog_mic.audio_device, output_port_parameters, i, false);
    }
    path_handle.with_hw_gain = false;
    path_handle.with_upwdown_sampler = false;
    hal_audio_set_path(&path_handle, HAL_AUDIO_CONTROL_ON);
    DSP_MW_LOG_I("DSP vow_sink set path done", 0);


    vow_control.u4AFE_MEMIF_BUF_BASE = mem_handle.buffer_addr;//recordBuf;//u4SramPhyBase;
    vow_control.u4AFE_MEMIF_BUF_END  = vow_control.u4AFE_MEMIF_BUF_BASE + (mem_handle.buffer_length - 1);
    vow_control.u4AFE_MEMIF_BUF_RP   = vow_control.u4AFE_MEMIF_BUF_BASE;
    vow_control.u4AFE_MEMIF_BUF_WP   = vow_control.u4AFE_MEMIF_BUF_BASE+ mem_handle.buffer_length/2;
#if 0
        //Sine generator for FGPA verification TEMP!!!
        //hal_audio_sine_generator_parameter_t sine_generator;
        hal_audio_sine_generator_parameter_t sine_generator;
        sine_generator.enable = true;
        sine_generator.rate = mem_handle.audio_path_rate;
        sine_generator.audio_control = device_handle_out.analog_mic.audio_device;
        sine_generator.port_parameter.device_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        sine_generator.is_input_port = false;
        DSP_MW_LOG_I("out audio_controol %d rate %d audio_interface %d sine_generator.rate %d\r\n",4,sine_generator.audio_control,sine_generator.rate,sine_generator.port_parameter.device_interface,sine_generator.rate);
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&sine_generator, HAL_AUDIO_SET_SINE_GENERATOR);
#endif
    return 0;

}

