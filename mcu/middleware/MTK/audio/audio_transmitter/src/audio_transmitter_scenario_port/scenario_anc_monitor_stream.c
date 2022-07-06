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
#include "stdbool.h"
#include "stddef.h"
#include "stdint.h"


#include "audio_transmitter_control.h"

#include "scenario_anc_monitor_stream.h"


#if defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
/*------------------------------------------------PORT----MTK_ANC_SURROUND_MONITOR_ENABLE------------------------------------------------------------------*/


void audio_transmitter_anc_monitor_open_playback(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    memset((void *)&open_param->stream_in_param, 0, sizeof(mcu2dsp_open_stream_in_param_t));
    memset((void *)&open_param->stream_out_param, 0, sizeof(mcu2dsp_open_stream_out_param_t));

    hal_audio_get_stream_in_setting_config (AU_DSP_ANC, &open_param->stream_in_param);
    hal_audio_get_stream_out_setting_config(AU_DSP_ANC, &open_param->stream_out_param);
    open_param->param.stream_in  = STREAM_IN_AFE;
    open_param->param.stream_out = STREAM_OUT_VIRTUAL;// STREAM_OUT_AFE;//STREAM_OUT_VIRTUAL;

#if 0
    open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
#ifdef ENABLE_2A2D_TEST
    open_param->stream_in_param.afe.audio_interface1 = HAL_AUDIO_INTERFACE_1;
    open_param->stream_in_param.afe.audio_interface2 = 0;
    open_param->stream_in_param.afe.audio_interface3 = 0;
#else
    open_param->stream_in_param.afe.audio_device |= open_param->stream_in_param.afe.audio_device1;
#endif
    open_param->stream_in_param.afe.audio_device    = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
#endif
    open_param->stream_in_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
    open_param->stream_in_param.afe.memory = HAL_AUDIO_MEM_SUB;
    open_param->stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
    #if defined(AIR_UL_FIX_SAMPLING_RATE_48K)
    open_param->stream_in_param.afe.sampling_rate   = 48000;
    open_param->stream_in_param.afe.frame_size      = 240;
    #else
    open_param->stream_in_param.afe.sampling_rate   = 16000;
    open_param->stream_in_param.afe.frame_size      = 80;
    #endif
    open_param->stream_in_param.afe.irq_period      = 0;
    open_param->stream_in_param.afe.frame_number    = 4;
    open_param->stream_in_param.afe.hw_gain         = false;

    TRANSMITTER_LOG_I("audio_transmitter_anc_monitor_open_playback in_device:0x%x out_device:0x%x", 2, open_param->stream_in_param.afe.audio_device, open_param->stream_out_param.afe.audio_device);
}

void audio_transmitter_anc_monitor_start_playback(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    start_param->param.stream_in = STREAM_IN_AFE;
    start_param->param.stream_out = STREAM_OUT_VIRTUAL;//STREAM_OUT_AFE;//STREAM_OUT_VIRTUAL;

    memset((void *)&start_param->stream_in_param, 0, sizeof(mcu2dsp_start_stream_in_param_t));
    memset((void *)&start_param->stream_out_param, 0, sizeof(mcu2dsp_start_stream_out_param_t));
    TRANSMITTER_LOG_I("audio_transmitter_anc_monitor_start_playback", 0);
}

#endif
