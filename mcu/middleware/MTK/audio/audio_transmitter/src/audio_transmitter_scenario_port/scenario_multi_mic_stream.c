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
#include "scenario_multi_mic_stream.h"

/*------------------------------------------------PORT----MTK_MULTI_MIC_STREAM_ENABLE------------------------------------------------------------------*/
#if defined(MTK_MULTI_MIC_STREAM_ENABLE)
void audio_transmitter_multmic_open_playback(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    hal_audio_memory_t memory;


    open_param->param.stream_in  = STREAM_IN_AFE;
    open_param->param.stream_out = STREAM_OUT_VIRTUAL;

    if (config->scenario_config.multi_mic_stream_config.mic_para == NULL) {
        hal_audio_get_stream_in_setting_config(AU_DSP_ANC, &(open_param->stream_in_param));
    }

    open_param->stream_in_param.afe.audio_device    = HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL;
    open_param->stream_in_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
    memory = 0;
    if ((open_param->stream_in_param.afe.audio_device != HAL_AUDIO_DEVICE_NONE) ||
            (open_param->stream_in_param.afe.audio_device1 != HAL_AUDIO_DEVICE_NONE) ||
            (open_param->stream_in_param.afe.audio_device2 != HAL_AUDIO_DEVICE_NONE) ||
            (open_param->stream_in_param.afe.audio_device3 != HAL_AUDIO_DEVICE_NONE)) {
        memory |= HAL_AUDIO_MEM_SUB;
    }
    if (config->scenario_config.multi_mic_stream_config.echo_reference == true) {
        memory |= HAL_AUDIO_MEM3;
    }
    open_param->stream_in_param.afe.memory = memory;
    open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
    open_param->stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
    open_param->stream_in_param.afe.sampling_rate   = 48000;
    open_param->stream_in_param.afe.irq_period      = 0;
    open_param->stream_in_param.afe.frame_size      = 512;
    open_param->stream_in_param.afe.frame_number    = 3;
    open_param->stream_in_param.afe.hw_gain         = false;
    open_param->stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;

#ifdef MTK_MULTI_MIC_STREAM_ENABLE
    open_param->stream_in_param.afe.irq_period = ((uint32_t)config->scenario_config.multi_mic_stream_config.frame_size * 1000) / config->scenario_config.multi_mic_stream_config.sampling_rate;
    open_param->stream_in_param.afe.sampling_rate = config->scenario_config.multi_mic_stream_config.sampling_rate;
    open_param->stream_in_param.afe.frame_size = config->scenario_config.multi_mic_stream_config.frame_size;
    open_param->stream_in_param.afe.frame_number = config->scenario_config.multi_mic_stream_config.frame_number;
    open_param->stream_in_param.afe.format = config->scenario_config.multi_mic_stream_config.format;
    open_param->stream_in_param.afe.audio_device = config->scenario_config.multi_mic_stream_config.mic_configs[0].audio_device;
    open_param->stream_in_param.afe.audio_interface = config->scenario_config.multi_mic_stream_config.mic_configs[0].audio_interface;
#ifdef ENABLE_2A2D_TEST
    open_param->stream_in_param.afe.audio_device1 = config->scenario_config.multi_mic_stream_config.mic_configs[1].audio_device;
    open_param->stream_in_param.afe.audio_interface1 = config->scenario_config.multi_mic_stream_config.mic_configs[1].audio_interface;
    open_param->stream_in_param.afe.audio_device2 = config->scenario_config.multi_mic_stream_config.mic_configs[2].audio_device;
    open_param->stream_in_param.afe.audio_interface2 = config->scenario_config.multi_mic_stream_config.mic_configs[2].audio_interface;
    open_param->stream_in_param.afe.audio_device3 = config->scenario_config.multi_mic_stream_config.mic_configs[3].audio_device;
    open_param->stream_in_param.afe.audio_interface3 = config->scenario_config.multi_mic_stream_config.mic_configs[3].audio_interface;
#endif
#endif

    /* Below parameters are not be used and filled with default value */
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param->stream_in_param.afe.audio_device4 = HAL_AUDIO_DEVICE_NONE;
    open_param->stream_in_param.afe.audio_device5 = HAL_AUDIO_DEVICE_NONE;
    open_param->stream_in_param.afe.audio_device6 = HAL_AUDIO_DEVICE_NONE;
    open_param->stream_in_param.afe.audio_device7 = HAL_AUDIO_DEVICE_NONE;
    open_param->stream_in_param.afe.audio_interface4 = HAL_AUDIO_INTERFACE_NONE;
    open_param->stream_in_param.afe.audio_interface5 = HAL_AUDIO_INTERFACE_NONE;
    open_param->stream_in_param.afe.audio_interface6 = HAL_AUDIO_INTERFACE_NONE;
    open_param->stream_in_param.afe.audio_interface7 = HAL_AUDIO_INTERFACE_NONE;
#endif
    open_param->stream_in_param.afe.sw_channels = 0;
#ifdef ENABLE_HWSRC_CLKSKEW
    open_param->stream_in_param.afe.clkskew_mode = CLK_SKEW_V1;
#endif

    if (config->scenario_config.multi_mic_stream_config.mic_para != NULL) {
        hal_audio_translate_mic_config(config->scenario_config.multi_mic_stream_config.mic_para, &(open_param->stream_in_param));
    }
}

void audio_transmitter_multmic_start_playback(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    start_param->param.stream_in = STREAM_IN_AFE;
    start_param->param.stream_out = STREAM_OUT_VIRTUAL;
}
#endif

