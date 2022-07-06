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

/*!
 *@file   stream_audio_hardware.c
 *@brief  Defines the hardware control for audio stream
 *
 @verbatim
         * Programmer : SYChiu@airoha.com.tw, Ext.3307
         * Programmer : BrianChen@airoha.com.tw, Ext.2641
         * Programmer : MachiWu@airoha.com.tw, Ext.2673
 @endverbatim
 */

//-
#include "types.h"
#include "audio_config.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "stream_audio_setting.h"
#include "dtm.h"
#include "stream_audio_driver.h"
#include "stream_audio_hardware.h"
#include "hal_audio_afe_control.h"
#include "dsp_audio_ctrl.h"
#ifdef AIR_HFP_DNN_PATH_ENABLE
#include "dsp_scenario.h"
#endif
U16  ADC_SOFTSTART;
EXTERN afe_t afe;


#define AUDIO_AFE_DL_DEFAULT_FRAME_NUM 4
#define AUDIO_AFE_UL_DEFAULT_FRAME_NUM 4
#define AUDIO_AFE_SOURCE_ASRC_BUFFER_SIZE 4096

#ifdef MTK_PROMPT_SOUND_ENABLE
void afe_dl2_interrupt_handler(void);
void stream_audio_src2_interrupt(void);
#endif

void afe_dl1_interrupt_handler(void);
#if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE) || defined (AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE)
    void afe_dl3_interrupt_handler(void);
#endif
#if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE)
void afe_dl12_interrupt_handler(void);
#endif
void stream_audio_srcl_interrupt(void);
void afe_vul1_interrupt_handler(void);
void i2s_slave_ul_interrupt_handler(void);
void i2s_slave_dl_interrupt_handler(void);
#ifdef MTK_TDM_ENABLE
void i2s_slave_ul_tdm_interrupt_handler(void);
void i2s_slave_dl_tdm_interrupt_handler(void);
#endif
void Source_device_set_para(hal_audio_device_parameter_t *device_handle);
hal_audio_bias_selection_t micbias_para_convert(uint32_t  in_misc_parms);

#ifdef AIR_HWSRC_RX_TRACKING_ENABLE
extern bool g_esco_ul_open_flag;
#endif

VOID Sink_Audio_Get_Default_Parameters(SINK sink)
{
    AUDIO_PARAMETER *pAudPara = &sink->param.audio;
    afe_pcm_format_t format;
    uint32_t media_frame_samples;
    //modify for ab1568
    hal_audio_path_parameter_t *path_handle = &sink->param.audio.path_handle;
    hal_audio_memory_parameter_t *mem_handle = &sink->param.audio.mem_handle;
    hal_audio_device_parameter_t *device_handle = &sink->param.audio.device_handle;
    memset(&sink->param.audio.AfeBlkControl, 0, sizeof(afe_block_t));

    format = gAudioCtrl.Afe.AfeDLSetting.format ;
    /* calculate memory size for delay */
    if (format == AFE_PCM_FORMAT_S32_LE ||
        format == AFE_PCM_FORMAT_U32_LE ||
        format == AFE_PCM_FORMAT_S24_LE ||
        format == AFE_PCM_FORMAT_U24_LE)
        pAudPara->format_bytes = 4;
    else
        pAudPara->format_bytes = 2;

    pAudPara->format        = format;

    pAudPara->channel_num = ((sink->param.audio.channel_sel == AUDIO_CHANNEL_A) ||
                             (sink->param.audio.channel_sel == AUDIO_CHANNEL_B) ||
                             (sink->param.audio.channel_sel == AUDIO_CHANNEL_VP))
                                        ? 1 : 2;

    pAudPara->rate          = gAudioCtrl.Afe.AfeDLSetting.rate;
    pAudPara->src_rate      = gAudioCtrl.Afe.AfeDLSetting.src_rate;
    pAudPara->period        = gAudioCtrl.Afe.AfeDLSetting.period;           /* ms, how many period to trigger */

#if 1   // for FPGA early porting
    if (sink->type == SINK_TYPE_VP_AUDIO) {
        pAudPara->sw_channels = pAudPara->channel_num;
        media_frame_samples = Audio_setting->Audio_VP.Frame_Size;
    } else {
        pAudPara->sw_channels = Audio_setting->Audio_sink.Software_Channel_Num;
        media_frame_samples = Audio_setting->Audio_sink.Frame_Size;//Audio_setting->Audio_sink.Frame_Size;//AUDIO_AAC_FRAME_SAMPLES;
    }
#else
    switch (gAudioCtrl.Afe.OperationMode) {
        case AU_AFE_OP_ESCO_VOICE_MODE:
            media_frame_samples = AUDIO_SBC_FRAME_SAMPLES;  // use mSBC (worst case)
            break;
        case AU_AFE_OP_PLAYBACK_MODE:
            media_frame_samples = AUDIO_AAC_FRAME_SAMPLES; // TODO:
            break;
        default:
            media_frame_samples = AUDIO_SBC_FRAME_SAMPLES;
            break;
    }
#endif

#if defined (AIR_ADVANCED_PASSTHROUGH_ENABLE)
    if ((sink->type == SINK_TYPE_AUDIO_DL3) && (Audio_setting->Audio_sink.Buffer_Frame_Num != AUDIO_AFE_DL_DEFAULT_FRAME_NUM) && (Audio_setting->Audio_sink.Buffer_Frame_Num > 0))
    {
        pAudPara->buffer_size   = Audio_setting->Audio_sink.Buffer_Frame_Num * media_frame_samples *
                              pAudPara->channel_num * pAudPara->format_bytes;
    }
    else
#endif
    if (((media_frame_samples == 120) && ( pAudPara->rate == 16000))||((media_frame_samples == 360) && ( pAudPara->rate == 48000)))
    {
        pAudPara->buffer_size   = (AUDIO_AFE_DL_DEFAULT_FRAME_NUM + 4) * media_frame_samples *
                              pAudPara->channel_num * pAudPara->format_bytes;
    }
    else

    {
        pAudPara->buffer_size   = AUDIO_AFE_DL_DEFAULT_FRAME_NUM * media_frame_samples *
                              pAudPara->channel_num * pAudPara->format_bytes;
    }

    pAudPara->AfeBlkControl.u4asrc_buffer_size = pAudPara->buffer_size;

    if (pAudPara->period == 1) {
        /* Gaming Headset Customized Period */
        pAudPara->count         = 120;
    } else {
    pAudPara->count         = (pAudPara->rate * pAudPara->period) / 1000;
    }

    if (pAudPara->count >= pAudPara->buffer_size) {
        pAudPara->count = pAudPara->buffer_size >> 2;
    }
    if (pAudPara->period == 0) {
        pAudPara->count = media_frame_samples;
        pAudPara->period = media_frame_samples / (pAudPara->rate /1000);
    }
    pAudPara->audio_device                   = gAudioCtrl.Afe.AfeDLSetting.audio_device;
#ifdef ENABLE_2A2D_TEST
    pAudPara->audio_device1                  = HAL_AUDIO_CONTROL_NONE;
    pAudPara->audio_device2                  = HAL_AUDIO_CONTROL_NONE;
    pAudPara->audio_device3                  = HAL_AUDIO_CONTROL_NONE;
    pAudPara->audio_device4                  = HAL_AUDIO_CONTROL_NONE;
    pAudPara->audio_device5                  = HAL_AUDIO_CONTROL_NONE;
    pAudPara->audio_device6                  = HAL_AUDIO_CONTROL_NONE;
    pAudPara->audio_device7                  = HAL_AUDIO_CONTROL_NONE;
#endif
    pAudPara->stream_channel                 = gAudioCtrl.Afe.AfeDLSetting.stream_channel;
    pAudPara->memory                         = gAudioCtrl.Afe.AfeDLSetting.memory;
    pAudPara->audio_interface                = gAudioCtrl.Afe.AfeDLSetting.audio_interface;
#ifdef ENABLE_2A2D_TEST
    pAudPara->audio_interface1               = HAL_AUDIO_INTERFACE_NONE;
    pAudPara->audio_interface2               = HAL_AUDIO_INTERFACE_NONE;
    pAudPara->audio_interface3               = HAL_AUDIO_INTERFACE_NONE;
    pAudPara->audio_interface4               = HAL_AUDIO_INTERFACE_NONE;
    pAudPara->audio_interface5               = HAL_AUDIO_INTERFACE_NONE;
    pAudPara->audio_interface6               = HAL_AUDIO_INTERFACE_NONE;
    pAudPara->audio_interface7               = HAL_AUDIO_INTERFACE_NONE;
#endif
    pAudPara->hw_gain                        = gAudioCtrl.Afe.AfeDLSetting.hw_gain;
    pAudPara->echo_reference                 = gAudioCtrl.Afe.AfeDLSetting.echo_reference;
    #ifdef AUTO_ERROR_SUPPRESSION
    pAudPara->misc_parms.I2sClkSourceType    = gAudioCtrl.Afe.AfeDLSetting.misc_parms.I2sClkSourceType;
    pAudPara->misc_parms.MicbiasSourceType   = gAudioCtrl.Afe.AfeDLSetting.misc_parms.MicbiasSourceType;
    #endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
    pAudPara->enable_ul_dnn                  = gAudioCtrl.Afe.AfeDLSetting.enable_ul_dnn;
#endif
    DSP_MW_LOG_I("audio sink default buffer_size:%d, count:%d\r\n", 2, pAudPara->buffer_size, pAudPara->count);
    DSP_MW_LOG_I("audio sink default device:%d, channel:%d, memory:%d, interface:%d rate:%d\r\n", 5, pAudPara->audio_device,
                                                                                    pAudPara->stream_channel,
                                                                                    pAudPara->memory,
                                                                                    pAudPara->audio_interface,
                                                                                    pAudPara->rate);

    //modfiy for ab1568
    pAudPara->with_sink_src = false;//HWSRC Continuous mode
    //for hal_audio_set_memory
    mem_handle->buffer_length = pAudPara->buffer_size;
    mem_handle->memory_select = hal_memory_convert_dl(pAudPara->memory);//modify for ab1568
    mem_handle->irq_counter = pAudPara->count;
    mem_handle->pcm_format = pAudPara->format;
#ifdef ENABLE_HWSRC_CLKSKEW
    mem_handle->asrc_clkskew_mode = Audio_setting->Audio_sink.clkskew_mode;
#endif
#if 1
    if (mem_handle->memory_select == HAL_AUDIO_MEMORY_DL_DL1) {
        hal_audio_set_value_parameter_t handle;
        if(pAudPara->with_sink_src){
            mem_handle->memory_select = HAL_AUDIO_MEMORY_DL_SRC1;
            handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_SRC1;
            handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_DL_SRC1;
            handle.register_irq_handler.entry = stream_audio_srcl_interrupt;
        }else{
            handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_AUDIOSYS;
            handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
            handle.register_irq_handler.entry = afe_dl1_interrupt_handler;
        }
        DSP_MW_LOG_I("DL 1 memory_select %d,audio_irq %d,entry %x\r\n",3,handle.register_irq_handler.memory_select,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
        hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
    } else if (mem_handle->memory_select == HAL_AUDIO_MEMORY_DL_DL2) {
        #ifdef MTK_PROMPT_SOUND_ENABLE
            hal_audio_set_value_parameter_t handle;
            if(pAudPara->with_sink_src){
                mem_handle->memory_select = HAL_AUDIO_MEMORY_DL_SRC2;
                handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_SRC2;
                handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_DL_SRC2;
                handle.register_irq_handler.entry = stream_audio_src2_interrupt;
            }else{
                handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_AUDIOSYS;
                handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_DL_DL2;
                handle.register_irq_handler.entry = afe_dl2_interrupt_handler;
            }
            DSP_MW_LOG_I("DL 2 memory_select %d,audio_irq %d,entry %x\r\n",3,handle.register_irq_handler.memory_select,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
            hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
        #endif
    } else if (mem_handle->memory_select == HAL_AUDIO_MEMORY_DL_DL3) {
        #if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE) || defined (AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE)
        hal_audio_set_value_parameter_t handle;
        handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_AUDIOSYS;
        handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_DL_DL3;
        handle.register_irq_handler.entry = afe_dl3_interrupt_handler;
        DSP_MW_LOG_I("DL 3 memory_select %d,audio_irq %d,entry %x\r\n",3,handle.register_irq_handler.memory_select,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
        hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
        #endif
    } else if (mem_handle->memory_select == HAL_AUDIO_MEMORY_DL_DL12) {
        #if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE)
        hal_audio_set_value_parameter_t handle;
        handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_AUDIOSYS;
        handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_DL_DL12;
        handle.register_irq_handler.entry = afe_dl12_interrupt_handler;
        DSP_MW_LOG_I("DL 12 memory_select %d,audio_irq %d,entry %x\r\n",3,handle.register_irq_handler.memory_select,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
        hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
        #endif
    } else if (mem_handle->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_DMA) {
        hal_audio_set_value_parameter_t handle;
        handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_I2S_SLAVE;
#ifdef MTK_TDM_ENABLE
        handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_DL_SLAVE_DMA;
#endif
        handle.register_irq_handler.entry = i2s_slave_dl_interrupt_handler;
        DSP_MW_LOG_I("DL Slave DMA audio_irq %d,entry %x\r\n",2,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
        hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
    } else if (mem_handle->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_TDM) {
        hal_audio_set_value_parameter_t handle;
        handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_I2S_SLAVE;
#ifdef MTK_TDM_ENABLE
        handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_DL_SLAVE_TDM;
        handle.register_irq_handler.entry = i2s_slave_dl_tdm_interrupt_handler;
#endif
        DSP_MW_LOG_I("DL Slave TDM audio_irq %d,entry %x\r\n",2,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
        hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
    }
    mem_handle->with_mono_channel = (pAudPara->channel_num == 1) ? true : false;

    //for hal_audio_set_path
    if(gAudioCtrl.Afe.AfeDLSetting.with_upwdown_sampler){
        path_handle->with_upwdown_sampler = true;
        path_handle->audio_path_input_rate = gAudioCtrl.Afe.AfeDLSetting.audio_path_input_rate;//afe_get_audio_device_samplerate(pAudPara->audio_device , pAudPara->audio_interface);
        path_handle->audio_path_output_rate = gAudioCtrl.Afe.AfeDLSetting.audio_path_output_rate;//afe_get_audio_device_samplerate(pAudPara->audio_device , pAudPara->audio_interface);
    }else{
        path_handle->with_upwdown_sampler = false;
        path_handle->audio_path_input_rate = pAudPara->rate;//afe_get_audio_device_samplerate(pAudPara->audio_device , pAudPara->audio_interface);
        path_handle->audio_path_output_rate = pAudPara->rate;//afe_get_audio_device_samplerate(pAudPara->audio_device , pAudPara->audio_interface);
    }

    path_handle->connection_number = pAudPara->channel_num;
    if ((sink->type == SINK_TYPE_VP_AUDIO) &&
         (pAudPara->audio_device != HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL)) {
        path_handle->connection_number = 1;
    }

    uint32_t i;
    hal_audio_path_port_parameter_t input_port_parameters, output_port_parameters;
    input_port_parameters.memory_select = mem_handle->memory_select;
    output_port_parameters.device_interface = pAudPara->audio_interface;
#ifdef AIR_HFP_DNN_PATH_ENABLE
    if((pAudPara->enable_ul_dnn == true) && (mem_handle->memory_select == HAL_AUDIO_MEMORY_DL_DL3)) {
        path_handle->connection_number = 1;
        path_handle->input.interconn_sequence[0]  = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, input_port_parameters, 0, false);
        path_handle->output.interconn_sequence[0] = stream_audio_convert_control_to_interconn(pAudPara->audio_device, output_port_parameters, 1, false);
        DSP_MW_LOG_I("[DNN] audio sink default enable_ul_dnn: %d, mem_handle->memory_select:0x%x", 2, pAudPara->enable_ul_dnn, mem_handle->memory_select);
    } else if((pAudPara->enable_ul_dnn == true) && (mem_handle->memory_select == HAL_AUDIO_MEMORY_DL_DL1))  {
        path_handle->connection_number = 1;
        path_handle->input.interconn_sequence[0]  = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, input_port_parameters, 0, false);
        path_handle->output.interconn_sequence[0] = stream_audio_convert_control_to_interconn(pAudPara->audio_device, output_port_parameters, 0, false);
        DSP_MW_LOG_I("[DNN] audio sink default enable_ul_dnn: %d, mem_handle->memory_select:0x%x", 2, pAudPara->enable_ul_dnn, mem_handle->memory_select);
    } else
#endif
{
    for (i=0 ; i<path_handle->connection_number ; i++) {
        path_handle->input.interconn_sequence[i]  = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, input_port_parameters, (mem_handle->with_mono_channel) ? 0: i, false);
        path_handle->output.interconn_sequence[i] = stream_audio_convert_control_to_interconn(pAudPara->audio_device, output_port_parameters, i, false);
    }
}
    path_handle->with_hw_gain = pAudPara->hw_gain;
    path_handle->with_dl_deq_mixer = false;//for anc & deq
    //for hal_audio_set_device
    device_handle->common.audio_device = pAudPara->audio_device;
    if (pAudPara->audio_device & HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL) {
        device_handle->dac.rate = pAudPara->rate;
        device_handle->dac.dac_mode = gAudioCtrl.Afe.AfeDLSetting.adc_mode;//HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
        device_handle->dac.dc_compensation_value = afe.stream_out.dc_compensation_value;
        device_handle->dac.with_high_performance = gAudioCtrl.Afe.AfeDLSetting.performance;
        device_handle->dac.with_phase_inverse = false;
        device_handle->dac.with_force_change_rate = true;
    } else if (pAudPara->audio_device & (HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER | HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L | HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
        device_handle->i2s_master.rate = path_handle->audio_path_output_rate;
        device_handle->i2s_master.i2s_interface = gAudioCtrl.Afe.AfeDLSetting.audio_interface;
        device_handle->i2s_master.i2s_format = gAudioCtrl.Afe.AfeDLSetting.i2s_format;
        device_handle->i2s_master.word_length = gAudioCtrl.Afe.AfeDLSetting.i2s_word_length;
        device_handle->i2s_master.mclk_divider = 2;
        device_handle->i2s_master.with_mclk = false;
        device_handle->i2s_master.is_low_jitter = gAudioCtrl.Afe.AfeDLSetting.is_low_jitter;
        device_handle->i2s_master.is_recombinant = false;
    } else if (pAudPara->audio_device & HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        device_handle->i2s_slave.rate = pAudPara->rate;
        device_handle->i2s_slave.i2s_interface = gAudioCtrl.Afe.AfeDLSetting.audio_interface;
        device_handle->i2s_slave.i2s_format = gAudioCtrl.Afe.AfeDLSetting.i2s_format;
        device_handle->i2s_slave.word_length = gAudioCtrl.Afe.AfeDLSetting.i2s_word_length;
#ifdef MTK_TDM_ENABLE
        device_handle->i2s_slave.tdm_channel = gAudioCtrl.Afe.AfeDLSetting.tdm_channel;
#endif
        device_handle->i2s_slave.memory_select = mem_handle->memory_select;
        if ((device_handle->i2s_slave.memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_DMA) || (device_handle->i2s_slave.memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_TDM)) {
            device_handle->i2s_slave.is_vdma_mode = true;
        } else {
            device_handle->i2s_slave.is_vdma_mode = false;
        }
#ifdef MTK_TDM_ENABLE
        if (device_handle->i2s_slave.memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_TDM) {
            if (device_handle->i2s_slave.i2s_interface == HAL_AUDIO_INTERFACE_1) {
                DSP_MW_LOG_I("[SLAVE TDM] DL I2S Slave0 not support TMD mode and assert", 0);
                platform_assert("[SLAVE TDM] DL I2S Slave0 not support TMD mode and assert",__FILE__,__LINE__);
            }
            if ((device_handle->i2s_slave.tdm_channel != HAL_AUDIO_I2S_TDM_4CH) && (device_handle->i2s_slave.tdm_channel != HAL_AUDIO_I2S_TDM_6CH) && (device_handle->i2s_slave.tdm_channel != HAL_AUDIO_I2S_TDM_8CH)) {
                DSP_MW_LOG_I("[SLAVE TDM] DL I2S Slave tdm channel : %d invalid and assert", 1, device_handle->i2s_slave.tdm_channel);
                platform_assert("[SLAVE TDM] DL I2S Slave tdm channel invalid and assert",__FILE__,__LINE__);
            }
        }
#endif
        #ifdef AIR_HWSRC_TX_TRACKING_ENABLE
        if(pAudPara->audio_interface == HAL_AUDIO_INTERFACE_1)
            mem_handle->src_tracking_clock_source =  HAL_AUDIO_SRC_TRACKING_I2S1;
        else if(pAudPara->audio_interface == HAL_AUDIO_INTERFACE_2)
            mem_handle->src_tracking_clock_source =  HAL_AUDIO_SRC_TRACKING_I2S2;
        else if(pAudPara->audio_interface == HAL_AUDIO_INTERFACE_3)
            mem_handle->src_tracking_clock_source =  HAL_AUDIO_SRC_TRACKING_I2S3;
        else if(pAudPara->audio_interface == HAL_AUDIO_INTERFACE_4)
            mem_handle->src_tracking_clock_source =  HAL_AUDIO_SRC_TRACKING_I2S4;
        DSP_MW_LOG_I("[HWSRC] tx tracking clock_source =  %d\r\n",1,mem_handle->src_tracking_clock_source);
        #endif
    } else if (pAudPara->audio_device & HAL_AUDIO_CONTROL_DEVICE_SPDIF) {
        device_handle->spdif.i2s_setting.rate = pAudPara->rate;
        device_handle->spdif.i2s_setting.i2s_interface = HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        device_handle->spdif.i2s_setting.i2s_format = HAL_AUDIO_I2S_I2S;
        device_handle->spdif.i2s_setting.word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
        device_handle->spdif.i2s_setting.mclk_divider = 2;
        device_handle->spdif.i2s_setting.with_mclk = false;
        device_handle->spdif.i2s_setting.is_low_jitter = false;
        device_handle->spdif.i2s_setting.is_recombinant = false;
    }
    DSP_MW_LOG_I("with_hw_gain %d\r\n",path_handle->with_hw_gain);
#endif
}

hal_audio_bias_selection_t micbias_para_convert(uint32_t  in_misc_parms){
    hal_audio_bias_selection_t bias_selection;
    bias_selection = in_misc_parms >> 20;
    return bias_selection;
}

VOID Source_Audio_Get_Default_Parameters(SOURCE source)
{
    AUDIO_PARAMETER *pAudPara = &source->param.audio;
    afe_pcm_format_t format;
    uint32_t media_frame_samples;
    hal_audio_path_parameter_t *path_handle = &source->param.audio.path_handle;//modify for ab1568
    hal_audio_memory_parameter_t *mem_handle = &source->param.audio.mem_handle;//modify for ab1568
    hal_audio_device_parameter_t *device_handle = &source->param.audio.device_handle;//modify for ab1568
    #ifdef ENABLE_2A2D_TEST
    hal_audio_device_parameter_t *device_handle1 = &source->param.audio.device_handle1;//modify for ab1568
    hal_audio_device_parameter_t *device_handle2 = &source->param.audio.device_handle2;//modify for ab1568
    hal_audio_device_parameter_t *device_handle3 = &source->param.audio.device_handle3;//modify for ab1568
    hal_audio_device_parameter_t *device_handle4 = &source->param.audio.device_handle4;//modify for ab1568
    hal_audio_device_parameter_t *device_handle5 = &source->param.audio.device_handle5;//modify for ab1568
    hal_audio_device_parameter_t *device_handle6 = &source->param.audio.device_handle6;//modify for ab1568
    hal_audio_device_parameter_t *device_handle7 = &source->param.audio.device_handle7;//modify for ab1568
    #endif
    memset(&source->param.audio.AfeBlkControl, 0, sizeof(afe_block_t));

    format = gAudioCtrl.Afe.AfeULSetting.format;
    /* calculate memory size for delay */
    if (format == AFE_PCM_FORMAT_S32_LE ||
        format == AFE_PCM_FORMAT_U32_LE ||
        format == AFE_PCM_FORMAT_S24_LE ||
        format == AFE_PCM_FORMAT_U24_LE)
        pAudPara->format_bytes = 4;
    else
        pAudPara->format_bytes = 2;

    pAudPara->format        = format;

    if((pAudPara->channel_sel == AUDIO_CHANNEL_A) || (pAudPara->channel_sel == AUDIO_CHANNEL_B)) {
        pAudPara->channel_num = 1;
    } else if (pAudPara->channel_sel == AUDIO_CHANNEL_A_AND_B) {
        pAudPara->channel_num = 2;
    } else if (pAudPara->channel_sel == AUDIO_CHANNEL_3ch) {
        pAudPara->channel_num = 3;
    } else if (pAudPara->channel_sel == AUDIO_CHANNEL_4ch) {
        pAudPara->channel_num = 4;
    } else if (pAudPara->channel_sel == AUDIO_CHANNEL_5ch) {
        pAudPara->channel_num = 5;
    } else if (pAudPara->channel_sel == AUDIO_CHANNEL_6ch) {
        pAudPara->channel_num = 6;
#ifdef MTK_TDM_ENABLE
    } else if (pAudPara->channel_sel == AUDIO_CHANNEL_8ch) {
        pAudPara->channel_num = 8;
#endif
    } else {
        pAudPara->channel_num = 2;
    }

    DSP_MW_LOG_I("audio source default channel_num:%d, channel_sel:%d", 2, pAudPara->channel_num, pAudPara->channel_sel);
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    pAudPara->Frame_Size = Audio_setting->Audio_source.Frame_Size;
    pAudPara->Buffer_Frame_Num = Audio_setting->Audio_source.Buffer_Frame_Num;
#endif

   // pAudPara->channel_num   = 1;
    pAudPara->rate          = gAudioCtrl.Afe.AfeULSetting.rate;
    pAudPara->src_rate      = gAudioCtrl.Afe.AfeULSetting.src_rate;
    pAudPara->period        = gAudioCtrl.Afe.AfeULSetting.period;

    // for early porting
    media_frame_samples = Audio_setting->Audio_source.Frame_Size;//AUDIO_AAC_FRAME_SAMPLES;

    uint8_t channel_num     = (pAudPara->channel_num>=2) ? 2 : 1;
    pAudPara->buffer_size   = media_frame_samples*Audio_setting->Audio_source.Buffer_Frame_Num*channel_num*pAudPara->format_bytes;

    pAudPara->AfeBlkControl.u4asrc_buffer_size = AUDIO_AFE_SOURCE_ASRC_BUFFER_SIZE;//AUDIO_AFE_BUFFER_SIZE;//pAudPara->buffer_size;
    #ifdef AIR_I2S_SLAVE_ENABLE
    if (gAudioCtrl.Afe.AfeULSetting.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        pAudPara->AfeBlkControl.u4asrc_buffer_size = pAudPara->buffer_size/2;
    }
    #endif
    //pAudPara->buffer_size   = AUDIO_SOURCE_DEFAULT_FRAME_NUM * media_frame_samples *
    //                          pAudPara->channel_num * pAudPara->format_bytes;
// printf("===> %d %d %d %d = %d\r\n", AUDIO_SOURCE_DEFAULT_FRAME_NUM, media_frame_samples, pAudPara->channel_num,pAudPara->format_bytes,   pAudPara->buffer_size);
    pAudPara->count         = (pAudPara->rate * pAudPara->period) / 1000;
    if (pAudPara->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
        //pAudPara->count         = (pAudPara->src_rate * pAudPara->period) / 1000;
        #endif
    }
    if (pAudPara->count >= pAudPara->buffer_size) {
        pAudPara->count = pAudPara->buffer_size >> 2;
    }
    if (pAudPara->period == 0) {
        pAudPara->count = media_frame_samples;
        pAudPara->period = media_frame_samples / (pAudPara->rate /1000);
    }
    pAudPara->audio_device                   = gAudioCtrl.Afe.AfeULSetting.audio_device;
#ifdef ENABLE_2A2D_TEST
    pAudPara->audio_device1                  = gAudioCtrl.Afe.AfeULSetting.audio_device1;
    pAudPara->audio_device2                  = gAudioCtrl.Afe.AfeULSetting.audio_device2;
    pAudPara->audio_device3                  = gAudioCtrl.Afe.AfeULSetting.audio_device3;
    pAudPara->audio_device4                  = gAudioCtrl.Afe.AfeULSetting.audio_device4;
    pAudPara->audio_device5                  = gAudioCtrl.Afe.AfeULSetting.audio_device5;
    pAudPara->audio_device6                  = gAudioCtrl.Afe.AfeULSetting.audio_device6;
    pAudPara->audio_device7                  = gAudioCtrl.Afe.AfeULSetting.audio_device7;
#endif
    pAudPara->stream_channel                 = gAudioCtrl.Afe.AfeULSetting.stream_channel;
    pAudPara->memory                         = gAudioCtrl.Afe.AfeULSetting.memory;
    pAudPara->audio_interface                = gAudioCtrl.Afe.AfeULSetting.audio_interface;
#ifdef ENABLE_2A2D_TEST
    pAudPara->audio_interface1               = gAudioCtrl.Afe.AfeULSetting.audio_interface1;
    pAudPara->audio_interface2               = gAudioCtrl.Afe.AfeULSetting.audio_interface2;
    pAudPara->audio_interface3               = gAudioCtrl.Afe.AfeULSetting.audio_interface3;
    pAudPara->audio_interface4               = gAudioCtrl.Afe.AfeULSetting.audio_interface4;
    pAudPara->audio_interface5               = gAudioCtrl.Afe.AfeULSetting.audio_interface5;
    pAudPara->audio_interface6               = gAudioCtrl.Afe.AfeULSetting.audio_interface6;
    pAudPara->audio_interface7               = gAudioCtrl.Afe.AfeULSetting.audio_interface7;
#endif
    pAudPara->hw_gain                        = gAudioCtrl.Afe.AfeULSetting.hw_gain;
    pAudPara->echo_reference                 = gAudioCtrl.Afe.AfeULSetting.echo_reference;
    #ifdef AUTO_ERROR_SUPPRESSION
    pAudPara->misc_parms.I2sClkSourceType    = gAudioCtrl.Afe.AfeULSetting.misc_parms.I2sClkSourceType;
    pAudPara->misc_parms.MicbiasSourceType   = gAudioCtrl.Afe.AfeULSetting.misc_parms.MicbiasSourceType;
    #endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
    pAudPara->enable_ul_dnn                  = gAudioCtrl.Afe.AfeULSetting.enable_ul_dnn;
#endif

    pAudPara->with_upwdown_sampler           = gAudioCtrl.Afe.AfeULSetting.with_upwdown_sampler;
    pAudPara->audio_path_input_rate          = gAudioCtrl.Afe.AfeULSetting.audio_path_input_rate;
    pAudPara->audio_path_output_rate         = gAudioCtrl.Afe.AfeULSetting.audio_path_output_rate;
    DSP_MW_LOG_I("audio source default buffer_size:%d, count:%d\r\n", 2, pAudPara->buffer_size, pAudPara->count);
    DSP_MW_LOG_I("audio source default device:%d, channel:%d, memory:%d, interface:%d rate %d\r\n", 5, pAudPara->audio_device,
                                                                                      pAudPara->stream_channel,
                                                                                      pAudPara->memory,
                                                                                      pAudPara->audio_interface,
                                                                                      pAudPara->rate);

    //modify for ab1568
    //for hal_audio_set_memory

    mem_handle->buffer_length = pAudPara->buffer_size;
    mem_handle->memory_select = hal_memory_convert_ul(pAudPara->memory);//modify for ab1568
    if (!(mem_handle->memory_select&(HAL_AUDIO_MEMORY_UL_SLAVE_TDM|HAL_AUDIO_MEMORY_UL_SLAVE_DMA))) {
        if (pAudPara->channel_num >= 3) {
            mem_handle->memory_select |= HAL_AUDIO_MEMORY_UL_VUL2;
        }
        if (pAudPara->channel_num >= 5) {
            mem_handle->memory_select |= HAL_AUDIO_MEMORY_UL_VUL3;
        }
        if (pAudPara->channel_num >= 7) {
            mem_handle->memory_select |= HAL_AUDIO_MEMORY_UL_AWB;
        }
        if (pAudPara->channel_num >= 9) {
            DSP_MW_LOG_W("DSP STREAM: no memory agent for more channels.", 0);
        }
    }
    mem_handle->irq_counter = pAudPara->count;
    mem_handle->pcm_format = pAudPara->format;

#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    if ((source->type >= SOURCE_TYPE_SUBAUDIO_MIN) && (source->type <= SOURCE_TYPE_SUBAUDIO_MAX)) {
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        if (pAudPara->memory == (HAL_AUDIO_MEM_SUB|HAL_AUDIO_MEM4)){
            hal_audio_set_value_parameter_t handle;
            handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_AUDIOSYS;
            handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_UL_AWB;
            handle.register_irq_handler.entry = afe_subsource2_interrupt_handler;
            DSP_MW_LOG_I("DSP AFE Sub-Source2 memory_select 0x%x, entry 0x%x\r\n", 2, handle.register_irq_handler.memory_select, handle.register_irq_handler.entry);
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&handle, HAL_AUDIO_SET_IRQ_HANDLER);
        } else
#endif
        {
            hal_audio_set_value_parameter_t handle;
            handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_AUDIOSYS;
            handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_UL_MASK;
            handle.register_irq_handler.entry = afe_subsource_interrupt_handler;
            DSP_MW_LOG_I("DSP AFE Sub-Source memory_select 0x%x, entry 0x%x\r\n", 2, handle.register_irq_handler.memory_select, handle.register_irq_handler.entry);
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&handle, HAL_AUDIO_SET_IRQ_HANDLER);
        }
    } else
#endif
    {
        if (mem_handle->memory_select&HAL_AUDIO_MEMORY_UL_VUL1) {
            #ifdef AIR_I2S_SLAVE_ENABLE
            if( pAudPara->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE )
            {
                #if 0
                hal_audio_set_value_parameter_t handle;
                handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_I2S_SLAVE;
                //handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
                handle.register_irq_handler.entry = i2s_slave_1_ul_interrupt_handler;
                DSP_MW_LOG_I("[HAS][LINEIN]HAL_AUDIO_MEMORY_UL_VUL1 memory_select %d,audio_irq %d,entry %x\r\n",3,handle.register_irq_handler.memory_select,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
                hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
                #endif
            }else
            #endif
            {
                hal_audio_set_value_parameter_t handle;
                handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_AUDIOSYS;
                handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
                handle.register_irq_handler.entry = afe_vul1_interrupt_handler;
                DSP_MW_LOG_I("memory_select %d,audio_irq %d,entry %x\r\n",3,handle.register_irq_handler.memory_select,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
                hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
            }
        } else if (mem_handle->memory_select == HAL_AUDIO_MEMORY_UL_VUL2) {

        } else if (mem_handle->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_DMA) {
            hal_audio_set_value_parameter_t handle;
            handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_I2S_SLAVE;
#ifdef MTK_TDM_ENABLE
            handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_UL_SLAVE_DMA;
#endif
            handle.register_irq_handler.entry = i2s_slave_ul_interrupt_handler;
            DSP_MW_LOG_I("UL Slave DMA audio_irq %d,entry %x\r\n",2,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);

            hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
        } else if (mem_handle->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_TDM) {
            hal_audio_set_value_parameter_t handle;
            handle.register_irq_handler.audio_irq = HAL_AUDIO_IRQ_I2S_SLAVE;
#ifdef MTK_TDM_ENABLE
            handle.register_irq_handler.memory_select = HAL_AUDIO_MEMORY_UL_SLAVE_TDM;
            handle.register_irq_handler.entry = i2s_slave_ul_tdm_interrupt_handler;
#endif
            DSP_MW_LOG_I("UL Slave TDM audio_irq %d,entry %x\r\n",2,handle.register_irq_handler.audio_irq,handle.register_irq_handler.entry);
            hal_audio_set_value(&handle, HAL_AUDIO_SET_IRQ_HANDLER);
        }
    }
    mem_handle->with_mono_channel = (pAudPara->channel_num == 1) ? true : false;

    //path
    path_handle->audio_path_input_rate = pAudPara->rate;//afe_get_audio_device_samplerate(pAudPara->audio_device, pAudPara->audio_interface);
    path_handle->audio_path_output_rate = pAudPara->rate;//afe_get_audio_device_samplerate(pAudPara->audio_device, pAudPara->audio_interface);
    path_handle->connection_selection = pAudPara->channel_num;//pAudPara->stream_channel;

    //handle.input.parameters.audio_interface;

    //path_handle->input.parameters.audio_interface = pAudPara->memory;//modify for ab1568
    //path_handle->input.port = pAudPara->audio_device;//modify for ab1568
    //path_handle->input.parameters.audio_interface = pAudPara->audio_interface;//modify for ab1568
    //path_handle->output.port = HAL_AUDIO_CONTROL_MEMORY_INTERFACE;//modify for ab1568
    //path_handle->output.parameters.memory_select = HAL_AUDIO_MEMORY_UL_VUL1;//modify for ab1568
    path_handle->connection_number = pAudPara->channel_num;

    uint32_t i;
    hal_audio_path_port_parameter_t input_port_parameters, output_port_parameters;
    input_port_parameters.device_interface = pAudPara->audio_interface;
    output_port_parameters.memory_select = mem_handle->memory_select&(~HAL_AUDIO_MEMORY_UL_AWB2);
    #ifdef ENABLE_2A2D_TEST
    hal_audio_device_t path_audio_device[HAL_AUDIO_PATH_SUPPORT_SEQUENCE] = {pAudPara->audio_device,pAudPara->audio_device1,pAudPara->audio_device2,pAudPara->audio_device3, pAudPara->audio_device4,
                                                                             pAudPara->audio_device5,pAudPara->audio_device6,pAudPara->audio_device7};
    hal_audio_device_interface_t device_interface[HAL_AUDIO_PATH_SUPPORT_SEQUENCE]= {pAudPara->audio_interface,pAudPara->audio_interface1,pAudPara->audio_interface2,pAudPara->audio_interface3,pAudPara->audio_interface4,
                                                                                     pAudPara->audio_interface5,pAudPara->audio_interface6,pAudPara->audio_interface7};
    hal_audio_memory_selection_t memory_select[HAL_AUDIO_PATH_SUPPORT_SEQUENCE]= {HAL_AUDIO_MEMORY_UL_VUL1, HAL_AUDIO_MEMORY_UL_VUL1 ,HAL_AUDIO_MEMORY_UL_VUL2, HAL_AUDIO_MEMORY_UL_VUL2, HAL_AUDIO_MEMORY_UL_VUL3, HAL_AUDIO_MEMORY_UL_VUL3,
            HAL_AUDIO_MEMORY_UL_AWB, HAL_AUDIO_MEMORY_UL_AWB};
    #else
    hal_audio_device_t path_audio_device[HAL_AUDIO_PATH_SUPPORT_SEQUENCE] = {pAudPara->audio_device,pAudPara->audio_device};
    hal_audio_device_interface_t device_interface[HAL_AUDIO_PATH_SUPPORT_SEQUENCE]= {pAudPara->audio_interface,pAudPara->audio_interface};
    hal_audio_memory_selection_t memory_select[HAL_AUDIO_PATH_SUPPORT_SEQUENCE]= {output_port_parameters.memory_select, output_port_parameters.memory_select};
    #endif

#ifdef AIR_HFP_DNN_PATH_ENABLE
    if((pAudPara->enable_ul_dnn == true) && (pAudPara->memory&HAL_AUDIO_MEM_SUB)) {
        path_handle->connection_number = 1;
        input_port_parameters.device_interface = device_interface[0];
        output_port_parameters.memory_select = memory_select[0];
        path_handle->input.interconn_sequence[0]  = stream_audio_convert_control_to_interconn(path_audio_device[0], input_port_parameters, 1, true);
        path_handle->output.interconn_sequence[0] = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, output_port_parameters, 0, true);
        DSP_MW_LOG_I("[DNN] audio source default enable_ul_dnn: %d, pAudPara->memory:0x%x", 2, pAudPara->enable_ul_dnn, pAudPara->memory);
    } else if ((pAudPara->enable_ul_dnn == true) && (pAudPara->memory&HAL_AUDIO_MEM1)){
        path_handle->connection_number = 1;
        input_port_parameters.device_interface = device_interface[0];
        output_port_parameters.memory_select = memory_select[0];
        path_handle->input.interconn_sequence[0]  = stream_audio_convert_control_to_interconn(path_audio_device[0], input_port_parameters, 0, true);
        path_handle->output.interconn_sequence[0] = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, output_port_parameters, 0, true);
        DSP_MW_LOG_I("[DNN] audio source default enable_ul_dnn: %d, pAudPara->memory:0x%x", 2, pAudPara->enable_ul_dnn, pAudPara->memory);
    } else
#endif

{
    for (i=0 ; i<path_handle->connection_number ; i++) {
        input_port_parameters.device_interface = device_interface[i];
        output_port_parameters.memory_select = memory_select[i];
        path_handle->input.interconn_sequence[i]  = stream_audio_convert_control_to_interconn(path_audio_device[i], input_port_parameters, i, true);
        path_handle->output.interconn_sequence[i] = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, output_port_parameters, i, true);
    }

}

    path_handle->with_hw_gain = pAudPara->hw_gain ;
    path_handle->with_upwdown_sampler = false;
    if(pAudPara->with_upwdown_sampler == true && pAudPara->audio_path_input_rate != 0 && pAudPara->audio_path_output_rate != 0
                                              && pAudPara->audio_path_input_rate != pAudPara->audio_path_output_rate)
    {
        path_handle->with_upwdown_sampler   = true;
        path_handle->audio_path_input_rate  = pAudPara->audio_path_input_rate;
        path_handle->audio_path_output_rate = pAudPara->audio_path_output_rate;
    }

#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    //Update memory and path selection for Sub-source
    if ((source->type >= SOURCE_TYPE_SUBAUDIO_MIN) && (source->type <= SOURCE_TYPE_SUBAUDIO_MAX)) {
        bool memory_fined = false;
        mem_handle->memory_select = 0;

        //whether if use same memory interface
        SOURCE_TYPE     search_source_type;
        hal_audio_memory_selection_t memory_assign, memory_occupied = 0;


        for (i=0; i<path_handle->connection_number ; i+=2) {
            memory_fined = false;
            if (path_handle->input.interconn_sequence[i] == (uint8_t)(HAL_AUDIO_INTERCONN_SEQUENCE_DUMMY&0xFF)){
                if (i == 0) {
                     path_handle->connection_number = 0;
                }
                DSP_MW_LOG_I("DSP audio path_handle->input.interconn_sequence[%d]==NULL \n", 1, i);
                break;
            }

            for (search_source_type = SOURCE_TYPE_SUBAUDIO_MIN ; search_source_type<=SOURCE_TYPE_SUBAUDIO_MAX ; search_source_type++) {
                if ((!Source_blks[search_source_type]) || (source->type == search_source_type)) {
                    continue;
                }
#if defined(MTK_MULTI_MIC_STREAM_ENABLE)
                uint32_t        interconn_sequence;
                for (interconn_sequence=0 ; interconn_sequence<HAL_AUDIO_PATH_SUPPORT_SEQUENCE ; interconn_sequence+=2) {
                    if ((path_handle->input.interconn_sequence[i] == Source_blks[search_source_type]->param.audio.path_handle.input.interconn_sequence[interconn_sequence]) &&
                        (path_handle->input.interconn_sequence[i+1] == Source_blks[search_source_type]->param.audio.path_handle.input.interconn_sequence[interconn_sequence+1])) {
                        path_handle->output.interconn_sequence[i] = Source_blks[search_source_type]->param.audio.path_handle.output.interconn_sequence[interconn_sequence];
                        path_handle->output.interconn_sequence[i+1] = Source_blks[search_source_type]->param.audio.path_handle.output.interconn_sequence[interconn_sequence+1];
                        mem_handle->memory_select |= stream_audio_convert_interconn_to_memory(source->param.audio.path_handle.output.interconn_sequence[i]);
                        pAudPara->buffer_size = Source_blks[search_source_type]->param.audio.buffer_size;
                        DSP_MW_LOG_I("audio source buffer find exist memory agent 0x%x  %d\n", 2, mem_handle->memory_select, source->param.audio.path_handle.output.interconn_sequence[i]);
                        search_source_type=SOURCE_TYPE_SUBAUDIO_MAX;
                        memory_fined = true;
                        break;
                    }
                }
#endif
                memory_occupied |= Source_blks[search_source_type]->param.audio.mem_handle.memory_select;
            }

            if (!memory_fined) {
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
                if (pAudPara->memory == (HAL_AUDIO_MEM_SUB|HAL_AUDIO_MEM4)){
                    if (!(memory_occupied&HAL_AUDIO_MEMORY_UL_AWB)) {
                        memory_assign = HAL_AUDIO_MEMORY_UL_AWB;
                    } else {
                        assert(0);
                    }

                } else
#endif
                {
                    if (!(mem_handle->memory_select&HAL_AUDIO_MEMORY_UL_VUL3) && !(memory_occupied&HAL_AUDIO_MEMORY_UL_VUL3)) {
                        memory_assign = HAL_AUDIO_MEMORY_UL_VUL3;
                    } else if (!(memory_occupied&HAL_AUDIO_MEMORY_UL_AWB)){
                        memory_assign = HAL_AUDIO_MEMORY_UL_AWB;
                    } else {
                        assert(0);
                    }
                }
                mem_handle->memory_select |= memory_assign;
                path_handle->output.interconn_sequence[i] = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, (hal_audio_path_port_parameter_t)memory_assign, i, true);
                path_handle->output.interconn_sequence[i+1] = stream_audio_convert_control_to_interconn(HAL_AUDIO_CONTROL_MEMORY_INTERFACE, (hal_audio_path_port_parameter_t)memory_assign, i+1, true);
            }
        }
        DSP_MW_LOG_I("DSP audio sub-source:%d, memory_agent:0x%x \n", 2, source->type,mem_handle->memory_select);
    }
#endif

    if(pAudPara->echo_reference){
        mem_handle->memory_select = mem_handle->memory_select | HAL_AUDIO_MEMORY_UL_AWB2;
    }

    if (mem_handle->memory_select == HAL_AUDIO_MEMORY_UL_AWB2) {
        //Echo path Only
        DSP_MW_LOG_I("DSP audio source echo paht Only \n", 0);
    }

    //for hal_audio_set_device
    if ((pAudPara->audio_device)&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_ANC|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R|HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE)) {
        if (pAudPara->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER) {
            device_handle->common.rate = pAudPara->audio_path_input_rate;
        } else {
            device_handle->common.rate = pAudPara->rate;
        }
        device_handle->common.device_interface = pAudPara->audio_interface;
#ifdef ENABLE_2A2D_TEST
        device_handle1->common.rate = pAudPara->rate;
        device_handle1->common.device_interface = pAudPara->audio_interface1;
        device_handle2->common.rate = pAudPara->rate;
        device_handle2->common.device_interface = pAudPara->audio_interface2;
        device_handle3->common.rate = pAudPara->rate;
        device_handle3->common.device_interface = pAudPara->audio_interface3;
        device_handle4->common.rate = pAudPara->rate;
        device_handle4->common.device_interface = pAudPara->audio_interface4;
        device_handle5->common.rate = pAudPara->rate;
        device_handle5->common.device_interface = pAudPara->audio_interface5;
        device_handle6->common.rate = pAudPara->rate;
        device_handle6->common.device_interface = pAudPara->audio_interface6;
        device_handle7->common.rate = pAudPara->rate;
        device_handle7->common.device_interface = pAudPara->audio_interface7;
#endif
        DSP_MW_LOG_I("set device common.rate %d,source rate %d",2,device_handle->common.rate,pAudPara->rate);
    }

    if (pAudPara->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        device_handle->i2s_slave.memory_select = mem_handle->memory_select;
        #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
        if(g_esco_ul_open_flag){
            mem_handle->irq_counter = pAudPara->count * 3;
        }
        mem_handle->src_rate = gAudioCtrl.Afe.AfeULSetting.src_rate;
        mem_handle->src_buffer_length = pAudPara->buffer_size;
        mem_handle->buffer_length = AUDIO_AFE_SOURCE_ASRC_BUFFER_SIZE;
        if(pAudPara->audio_interface == HAL_AUDIO_INTERFACE_1)
            mem_handle->src_tracking_clock_source =  HAL_AUDIO_SRC_TRACKING_I2S1;
        else if(pAudPara->audio_interface == HAL_AUDIO_INTERFACE_2)
            mem_handle->src_tracking_clock_source =  HAL_AUDIO_SRC_TRACKING_I2S2;
        else if(pAudPara->audio_interface == HAL_AUDIO_INTERFACE_3)
            mem_handle->src_tracking_clock_source =  HAL_AUDIO_SRC_TRACKING_I2S3;
        else if(pAudPara->audio_interface == HAL_AUDIO_INTERFACE_4)
            mem_handle->src_tracking_clock_source =  HAL_AUDIO_SRC_TRACKING_I2S4;
        DSP_MW_LOG_I("[HWSRC]rx tracking clock_source =  %d\r\n",1,mem_handle->src_tracking_clock_source);
        #endif
    }

    device_handle->common.audio_device = pAudPara->audio_device;
#ifdef ENABLE_2A2D_TEST
    device_handle1->common.audio_device = pAudPara->audio_device1;
    device_handle2->common.audio_device = pAudPara->audio_device2;
    device_handle3->common.audio_device = pAudPara->audio_device3;
    device_handle4->common.audio_device = pAudPara->audio_device4;
    device_handle5->common.audio_device = pAudPara->audio_device5;
    device_handle6->common.audio_device = pAudPara->audio_device6;
    device_handle7->common.audio_device = pAudPara->audio_device7;
#endif
    Source_device_set_para(device_handle);
#ifdef ENABLE_2A2D_TEST
    Source_device_set_para(device_handle1);
    Source_device_set_para(device_handle2);
    Source_device_set_para(device_handle3);
    Source_device_set_para(device_handle4);
    Source_device_set_para(device_handle5);
    Source_device_set_para(device_handle6);
    Source_device_set_para(device_handle7);
#endif


}

void Source_device_set_para(hal_audio_device_parameter_t *device_handle){
    uint8_t index = 0;
    if ((device_handle->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL) {
        device_handle->analog_mic.rate = device_handle->common.rate;//AUDIO_SOURCE_DEFAULT_ANALOG_VOICE_RATE;
        device_handle->analog_mic.mic_interface = device_handle->common.device_interface;//HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        device_handle->analog_mic.bias_select = gAudioCtrl.Afe.AfeULSetting.bias_select;

        device_handle->analog_mic.bias_voltage[0] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[0];
        device_handle->analog_mic.bias_voltage[1] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[1];
        device_handle->analog_mic.bias_voltage[2] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[2];
        device_handle->analog_mic.bias_voltage[3] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[3];
        device_handle->analog_mic.bias_voltage[4] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[4];
        DSP_MW_LOG_I("bias_select 0x%x,vol0 0x%x,vol1 0x%x,vol2 0x%x,vol3 0x%x,vol4 0x%x ",6,device_handle->analog_mic.bias_select,
            device_handle->analog_mic.bias_voltage[0],device_handle->analog_mic.bias_voltage[1],
            device_handle->analog_mic.bias_voltage[2],device_handle->analog_mic.bias_voltage[3],
            device_handle->analog_mic.bias_voltage[4]);
        switch(device_handle->common.audio_device){
            case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL:
            case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L:
                index = (2*(device_handle->common.device_interface) - 2);
                device_handle->analog_mic.adc_parameter.adc_mode = gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[index];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R:
                index = (2*(device_handle->common.device_interface) - 1);
                device_handle->analog_mic.adc_parameter.adc_mode = gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[index];
            break;
            default:
                device_handle->analog_mic.adc_parameter.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;
            break;
        }
        DSP_MW_LOG_I("adc_mode 0x%x",1,device_handle->analog_mic.adc_parameter.adc_mode);
        switch(device_handle->common.device_interface){
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1:
                device_handle->analog_mic.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[0];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2:
                device_handle->analog_mic.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[1];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3:
                device_handle->analog_mic.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[2];
            break;
            default:
                 device_handle->analog_mic.iir_filter = HAL_AUDIO_UL_IIR_50HZ_AT_48KHZ;//HAL_AUDIO_UL_IIR_5HZ_AT_48KHZ;
            break;
        }

        device_handle->analog_mic.with_external_bias = gAudioCtrl.Afe.AfeULSetting.with_external_bias;
        device_handle->analog_mic.bias1_2_with_LDO0 = gAudioCtrl.Afe.AfeULSetting.bias1_2_with_LDO0;
        device_handle->analog_mic.with_bias_lowpower = gAudioCtrl.Afe.AfeULSetting.with_bias_lowpower;
        device_handle->analog_mic.adc_parameter.performance = gAudioCtrl.Afe.AfeULSetting.performance;
        DSP_MW_LOG_I("Source ext bis %d, bias1_2_with_LDO0 %d",2,device_handle->analog_mic.with_external_bias,device_handle->analog_mic.bias1_2_with_LDO0);
    } else if ((device_handle->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL) {
        device_handle->linein.rate =  device_handle->common.rate;//AUDIO_SOURCE_DEFAULT_ANALOG_AUDIO_RATE;
        device_handle->linein.bias_select = gAudioCtrl.Afe.AfeULSetting.bias_select;
        device_handle->linein.bias_voltage[0] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[0];
        device_handle->linein.bias_voltage[1] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[1];
        device_handle->linein.bias_voltage[2] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[2];
        device_handle->linein.bias_voltage[3] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[3];
        device_handle->linein.bias_voltage[4] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[4];
        switch(device_handle->common.device_interface){
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1:
                device_handle->linein.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[0];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2:
                device_handle->linein.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[1];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3:
                device_handle->linein.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[2];
            break;
            default:
                 device_handle->linein.iir_filter = HAL_AUDIO_UL_IIR_50HZ_AT_48KHZ;//HAL_AUDIO_UL_IIR_5HZ_AT_48KHZ;
            break;
        }
        switch(device_handle->common.audio_device){
            case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL:
            case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_L:
                index = (2*(device_handle->common.device_interface) - 2);
                device_handle->linein.adc_parameter.adc_mode = gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[index];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_R:
                index = (2*(device_handle->common.device_interface) - 1);
                device_handle->linein.adc_parameter.adc_mode = gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[index];
            break;
            default:
                device_handle->linein.adc_parameter.adc_mode = HAL_AUDIO_ANALOG_INPUT_ACC10K;
            break;
        }

        device_handle->linein.adc_parameter.performance = gAudioCtrl.Afe.AfeULSetting.performance;

    } else if ((device_handle->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL) {
        device_handle->digital_mic.rate = device_handle->common.rate;//AUDIO_SOURCE_DEFAULT_ANALOG_VOICE_RATE;
        device_handle->digital_mic.mic_interface = device_handle->common.device_interface;//HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        switch(device_handle->common.audio_device){
            case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL:
            case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L:
                index = (2*(device_handle->common.device_interface) - 2);
                device_handle->digital_mic.dmic_selection = gAudioCtrl.Afe.AfeULSetting.dmic_selection[index];

            break;
            case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R:
                index = (2*(device_handle->common.device_interface) - 1);
                device_handle->digital_mic.dmic_selection = gAudioCtrl.Afe.AfeULSetting.dmic_selection[index];
            break;
            default:
                device_handle->digital_mic.dmic_selection = HAL_AUDIO_ANALOG_INPUT_ACC10K;
            break;
        }
        DSP_MW_LOG_I("dmic_selection %d",1,device_handle->digital_mic.dmic_selection);
        device_handle->digital_mic.bias_select = gAudioCtrl.Afe.AfeULSetting.bias_select;
        device_handle->digital_mic.bias_voltage[0] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[0];
        device_handle->digital_mic.bias_voltage[1] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[1];
        device_handle->digital_mic.bias_voltage[2] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[2];
        device_handle->digital_mic.bias_voltage[3] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[3];
        device_handle->digital_mic.bias_voltage[4] = gAudioCtrl.Afe.AfeULSetting.bias_voltage[4];
        switch(device_handle->common.device_interface){
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1:
                device_handle->digital_mic.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[0];
                device_handle->digital_mic.dmic_clock_rate = gAudioCtrl.Afe.AfeULSetting.dmic_clock_rate[0];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2:
                device_handle->digital_mic.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[1];
                device_handle->digital_mic.dmic_clock_rate = gAudioCtrl.Afe.AfeULSetting.dmic_clock_rate[1];
            break;
            case HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3:
                device_handle->digital_mic.iir_filter = gAudioCtrl.Afe.AfeULSetting.iir_filter[2];
                device_handle->digital_mic.dmic_clock_rate = gAudioCtrl.Afe.AfeULSetting.dmic_clock_rate[2];
            break;
            default:
                 device_handle->digital_mic.iir_filter = HAL_AUDIO_UL_IIR_DISABLE;
                 device_handle->digital_mic.dmic_clock_rate = 0;/**AFE_DMIC_CLOCK_3_25M*/
            break;
        }

        device_handle->digital_mic.with_external_bias = gAudioCtrl.Afe.AfeULSetting.with_external_bias;
        device_handle->digital_mic.with_bias_lowpower = gAudioCtrl.Afe.AfeULSetting.with_bias_lowpower;
        device_handle->digital_mic.bias1_2_with_LDO0 = gAudioCtrl.Afe.AfeULSetting.bias1_2_with_LDO0;
    } else if ((device_handle->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_VAD) {
        device_handle->vad.rate = AUDIO_SOURCE_DEFAULT_ANALOG_VOICE_RATE;
    } else if ((device_handle->common.audio_device)&(HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER | HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L | HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R)) {
        device_handle->i2s_master.rate = device_handle->common.rate;//48000;
        device_handle->i2s_master.i2s_interface = device_handle->common.device_interface;//HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
        device_handle->i2s_master.i2s_format = gAudioCtrl.Afe.AfeULSetting.i2s_format;
        device_handle->i2s_master.word_length = gAudioCtrl.Afe.AfeULSetting.i2s_word_length;
        device_handle->i2s_master.mclk_divider = 2;
        device_handle->i2s_master.with_mclk = false;
        device_handle->i2s_master.is_low_jitter = gAudioCtrl.Afe.AfeULSetting.is_low_jitter;
        device_handle->i2s_master.is_recombinant = false;
    } else if ((device_handle->common.audio_device)&HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        device_handle->i2s_slave.rate = device_handle->common.rate;//48000;
#ifdef MTK_TDM_ENABLE
        device_handle->i2s_slave.i2s_interface = gAudioCtrl.Afe.AfeULSetting.audio_interface;//HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
#else
        device_handle->i2s_slave.i2s_interface = device_handle->common.device_interface;//HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1;
#endif
        device_handle->i2s_slave.i2s_format = gAudioCtrl.Afe.AfeULSetting.i2s_format;
        device_handle->i2s_slave.word_length = gAudioCtrl.Afe.AfeULSetting.i2s_word_length;
#ifdef MTK_TDM_ENABLE
        device_handle->i2s_slave.tdm_channel = gAudioCtrl.Afe.AfeULSetting.tdm_channel;
#endif
        if ((device_handle->i2s_slave.memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_DMA) || (device_handle->i2s_slave.memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_TDM)) {
            device_handle->i2s_slave.is_vdma_mode = true;
        } else {
#ifdef AIR_I2S_SLAVE_ENABLE
            device_handle->i2s_slave.is_vdma_mode = true;
#else
            device_handle->i2s_slave.is_vdma_mode = false;
#endif
        }
#ifdef MTK_TDM_ENABLE
        if (device_handle->i2s_slave.memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_TDM) {
            if (device_handle->i2s_slave.i2s_interface == HAL_AUDIO_INTERFACE_1) {
                DSP_MW_LOG_I("[SLAVE TDM] UL I2S Slave0 not support TMD mode and assert", 0);
                platform_assert("[SLAVE TDM] UL I2S Slave0 not support TMD mode and assert",__FILE__,__LINE__);
            }
            if ((device_handle->i2s_slave.tdm_channel != HAL_AUDIO_I2S_TDM_4CH) && (device_handle->i2s_slave.tdm_channel != HAL_AUDIO_I2S_TDM_6CH) && (device_handle->i2s_slave.tdm_channel != HAL_AUDIO_I2S_TDM_8CH)) {
                DSP_MW_LOG_I("[SLAVE TDM] UL I2S Slave tdm channel : %d invalid and assert", 1, device_handle->i2s_slave.tdm_channel);
                platform_assert("[SLAVE TDM] UL I2S Slave tdm channel invalid and assert",__FILE__,__LINE__);
            }
        }
#endif
    }
}

VOID Sink_Audio_HW_Init_AFE(SINK sink)
{
    // do .hw_params()
    // according to sink to init the choosen AFE IO block
    // 1) hw_type
    // 2) channel
    // 3) mem allocate

    // TODO: AFE Clock init here <----

    if (audio_ops_probe(sink)) {
        DSP_MW_LOG_I("audio sink type : %d probe error\r\n", 1, sink->type);
    }
    if (audio_ops_hw_params(sink)) {
        DSP_MW_LOG_I("audio sink type : %d setting hw_params error\r\n", 1, sink->type);
    }

    switch (sink->param.audio.HW_type)
    {
        case  AUDIO_HARDWARE_PCM :
            //printf_james("Sink_Audio_HW_Init\r\n");
            break;
        case AUDIO_HARDWARE_I2S_M ://I2S master
            //#warning "To do I2S master interface later"
            break;
        case AUDIO_HARDWARE_I2S_S ://I2S slave
            //#warning "To do I2S slave interface later"
            break;
        default:
            configASSERT(0);
            break;
    }
}

VOID Sink_Audio_HW_Init(SINK sink)
{
    UNUSED(sink);
    // DSP_DRV_oDFE_CLK_INIT();
    // switch (sink->param.audio.HW_type)
    // {
        // case  AUDIO_HARDWARE_PCM :
            // AUDIO_DFE.OUT_SET0.field.ODFE_THRP_RATE_SEL = ODFE_RATE_ALIGN_DAC;
            // Audio_setting->Rate.Sink_Output_Sampling_Rate = FS_RATE_96K;
            // break;
        // case AUDIO_HARDWARE_I2S_M ://I2S master
            // AUDIO_DFE.OUT_SET0.field.ODFE_THRP_RATE_SEL = ODFE_RATE_ALIGN_I2S_MASTER;
            // AUDIO_DFE.OUT_SET0.field.I2S_MASTER_SDO_MUX = 0;
            // break;
        // case AUDIO_HARDWARE_I2S_S ://I2S slave
            // AUDIO_DFE.OUT_SET0.field.ODFE_THRP_RATE_SEL = ODFE_RATE_ALIGN_I2S_SLAVE;
            // AUDIO_DFE.OUT_SET0.field.I2S_SLAVE_SDO_MUX = 0;
            // break;
        // case AUDIO_HARDWARE_SPDIF :
            // AUDIO_DFE.OUT_SET0.field.ODFE_THRP_RATE_SEL = ODFE_RATE_ALIGN_SPDIF_TX;
            // AUDIO_DFE.OUT_SET0.field.SPDIF_TX_MUX = 0;
            // break;
    // }
}


VOID Source_Audio_HW_Init(SOURCE source)
{
    // TODO: AFE Clock init here <----
    if (audio_ops_probe(source))
        DSP_MW_LOG_I("audio source type : %d probe error\r\n", 1, source->type);

    if (audio_ops_hw_params(source))
        DSP_MW_LOG_I("audio source type : %d setting hw_params error\r\n", 1, source->type);

}


VOID AudioSourceHW_Ctrl (SOURCE source, BOOL IsEnabled)
{
    UNUSED(source);
    UNUSED(IsEnabled);
    /*U8 I2S_ModeSel,I2S_SampleRate;
    audio_hardware HW_type = (audio_hardware)source->param.audio.HW_type;

    I2S_ModeSel = (U8)I2S_RX_MODE;
    if (Sink_blks[SINK_TYPE_AUDIO] != NULL)
    {
        I2S_ModeSel = (Sink_blks[SINK_TYPE_AUDIO]->param.audio.HW_type == HW_type)? (U8)I2S_TRX_MODE : (U8)I2S_RX_MODE;
    }
    if (IsEnabled)
    {
        //Enable WADMA
        DSP_DRV_SOURCE_WADMA(source, IsEnabled);

        //Enable iDFE
        if(source->type == SOURCE_TYPE_AUDIO)
        {
            DSP_DRV_iDFE_DEC3_INIT( Audio_setting->Rate.Source_DownSampling_Ratio, source->param.audio.channel_num, Audio_setting->resolution.AudioInRes);
        }

        //Enable audio Interface
        switch (HW_type)
        {
            case AUDIO_HARDWARE_PCM:
                DSP_AD_IN_INIT();
                break;

            case AUDIO_HARDWARE_I2S_M:
                if (Audio_setting->Rate.Source_Input_Sampling_Rate > FS_RATE_48K)
                {
                    I2S_SampleRate = I2S_FS_RATE_96K;
                    Audio_setting->Rate.Source_Input_Sampling_Rate = FS_RATE_96K;
                }
                else
                {
                    I2S_SampleRate = I2S_FS_RATE_48K;
                    Audio_setting->Rate.Source_Input_Sampling_Rate = FS_RATE_48K;
                }
                DSP_DRV_I2S_MS_INIT(I2S_SampleRate ,I2S_WORD_LEN_32BIT,I2S_WORD_LEN_32BIT, I2S_WORD_LEN_32BIT ,I2S_ModeSel);
                break;

            case AUDIO_HARDWARE_I2S_S:
                DSP_DRV_I2S_SL_INIT(I2S_WORD_LEN_32BIT,I2S_WORD_LEN_32BIT,I2S_WORD_LEN_32BIT,I2S_ModeSel);
                break;
            case AUDIO_HARDWARE_SPDIF:
                DSP_DRV_SPDIF_RX_INIT();
                break;
            default:
                break;
        }
    }
    else
    {
        //Disable audio Interface
        switch (HW_type)
        {
            case AUDIO_HARDWARE_PCM:
                DSP_AD_IN_END();
                break;

            case AUDIO_HARDWARE_I2S_M :
                if (I2S.SET0.field.I2S_TR_MODE_CTL == I2S_RX_MODE)
                {
                    DSP_DRV_I2S_MS_END();
                }
                else
                {
                    I2S.SET0.field.I2S_TR_MODE_CTL = I2S_TX_MODE;
                }
                break;

            case AUDIO_HARDWARE_I2S_S :
                if (I2S.SET1.field.I2S_TR_MODE_CTL == I2S_RX_MODE)
                {
                    DSP_DRV_I2S_SL_END();
                }
                else
                {
                    I2S.SET1.field.I2S_TR_MODE_CTL = I2S_TX_MODE;
                }
                break;
            case AUDIO_HARDWARE_SPDIF:
                DSP_DRV_SPDIF_RX_END();
                break;
            default:
                break;
        }

        //Disable iDFE
        if(source->type == SOURCE_TYPE_AUDIO)
        {
            DSP_DRV_iDFE_DEC3_END();
        }

        //Disable WADMA
        DSP_DRV_SOURCE_WADMA(source, IsEnabled);
    }*/
}


VOID AudioSinkHW_Ctrl (SINK sink, BOOL IsEnabled)
{
    UNUSED(sink);
    UNUSED(IsEnabled);
    /*audio_hardware HW_type = sink->param.audio.HW_type;
    U8 I2S_ModeSel,samplingRate;

    I2S_ModeSel = (U8)I2S_TX_MODE;
    if (Source_blks[SOURCE_TYPE_AUDIO] != NULL)
    {
        I2S_ModeSel = (Source_blks[SOURCE_TYPE_AUDIO]->param.audio.HW_type == HW_type)? (U8)I2S_TRX_MODE : (U8)I2S_TX_MODE;
    }

    if (IsEnabled)
    {
        //Enable RADMA
        DSP_DRV_SINK_RADMA(sink, IsEnabled);

        //Enable SRC
        if (sink->type == SINK_TYPE_AUDIO && Audio_setting->Audio_sink.SRC_Out_Enable)
        {
            DSP_DRV_SRC_A_INIT (DSP_FsChange2SRCInRate(Audio_setting->Rate.SRC_Sampling_Rate),
                                DSP_FsChange2SRCOutRate(AudioSinkSamplingRate_Get()),
                                Audio_setting->resolution.SRCInRes,
                                Audio_setting->resolution.AudioOutRes);
        }

        //Enable oDFE
        if(sink->type == SINK_TYPE_AUDIO)
        {
            DSP_DRV_oDFE_INT4_INIT(Audio_setting->Rate.Sink_UpSampling_Ratio,
                                   sink->param.audio.channel_num,
                                   (Audio_setting->Audio_sink.SRC_Out_Enable)^1,
                                   Audio_setting->Audio_sink.CIC_Filter_Enable,
                                   Audio_setting->resolution.AudioOutRes);
        }
        else if (sink->type == SINK_TYPE_VP_AUDIO)
        {

            DSP_DRV_oDFE_INT6_INIT(Audio_setting->Rate.VP_UpSampling_Ratio,
                                   WITH_CIC,
                                   Audio_setting->resolution.AudioOutRes); //Shall able to adjust sampling rate with audio sink
        }

        //Enable audio Interface
        switch (HW_type)
        {
            case AUDIO_HARDWARE_PCM:
                if (AUDIO_CODEC.CTL0.field.EN_AU_DAC_DSM == 0)
                {
                    DSP_DA_OUT_INIT(sink->param.audio.channel_num);
                }
                break;

            case AUDIO_HARDWARE_I2S_M :
                if (Audio_setting->Rate.Sink_Output_Sampling_Rate > FS_RATE_48K)
                {
                    samplingRate = I2S_FS_RATE_96K;
                    Audio_setting->Rate.Sink_Output_Sampling_Rate = FS_RATE_96K;
                }
                else
                {
                    samplingRate = I2S_FS_RATE_48K;
                    Audio_setting->Rate.Sink_Output_Sampling_Rate = FS_RATE_48K;
                }
                DSP_DRV_I2S_MS_INIT(samplingRate ,I2S_WORD_LEN_32BIT,I2S_WORD_LEN_32BIT, I2S_WORD_LEN_32BIT ,I2S_ModeSel);
                break;

            case AUDIO_HARDWARE_I2S_S :
                DSP_DRV_I2S_SL_INIT(I2S_WORD_LEN_32BIT,I2S_WORD_LEN_32BIT,I2S_WORD_LEN_32BIT,I2S_ModeSel);
                break;
            case AUDIO_HARDWARE_SPDIF :
                samplingRate = DSP_ChangeFs2SpdifRate(Audio_setting->Rate.Sink_Output_Sampling_Rate);
                DSP_DRV_SPDIF_TX_INIT(samplingRate);
                break;
            default:
                break;
        }
    }
    else
    {
        //Disable audio Interface
        switch (HW_type)
        {
            case AUDIO_HARDWARE_PCM:
                DSP_DA_OUT_END();
                break;

            case AUDIO_HARDWARE_I2S_M :
                if (I2S.SET0.field.I2S_TR_MODE_CTL == I2S_TX_MODE)
                {
                    DSP_DRV_I2S_MS_END();
                }
                else
                {
                    I2S.SET0.field.I2S_TR_MODE_CTL = I2S_RX_MODE;
                }
                break;

            case AUDIO_HARDWARE_I2S_S :
                if (I2S.SET1.field.I2S_TR_MODE_CTL == I2S_TX_MODE)
                {
                    DSP_DRV_I2S_SL_END();
                }
                else
                {
                    I2S.SET1.field.I2S_TR_MODE_CTL = I2S_RX_MODE;
                }
                break;
            case AUDIO_HARDWARE_SPDIF:
                DSP_DRV_SPDIF_TX_END();
                break;
            default:
                break;
        }

        //Disable oDFE
        if(sink->type == SINK_TYPE_AUDIO)
        {
            DSP_DRV_oDFE_INT4_END();
        }
        else if (sink->type == SINK_TYPE_VP_AUDIO)
        {
            DSP_DRV_oDFE_INT6_END();
        }


        //Disable SRC
        if (sink->type == SINK_TYPE_AUDIO && Audio_setting->Audio_sink.SRC_Out_Enable)
        {
            DSP_DRV_SRC_A_END();
        }

        //Disable RADMA
        DSP_DRV_SINK_RADMA(sink, IsEnabled);
    }

    if (sink->type == SINK_TYPE_AUDIO)
        Audio_setting->Audio_sink.Output_Enable = IsEnabled;
    else if (sink->type == SINK_TYPE_VP_AUDIO)
        Audio_setting->Audio_VP.Output_Enable = IsEnabled;

    #if 0
    if ((Audio_setting->Audio_sink.Output_Enable==FALSE) &&
        (Audio_setting->Audio_VP.Output_Enable==FALSE))
        DSP_DRV_DisableAudioDfeClock();
    #endif*/
}

