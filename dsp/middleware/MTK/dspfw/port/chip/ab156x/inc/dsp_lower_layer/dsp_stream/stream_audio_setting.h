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

#ifndef __STREAM_AUDIO_SETTING_H__
#define __STREAM_AUDIO_SETTING_H__

/*!
 *@file   stream_audio_setting.h
 *@brief  defines the setting of audio stream
 *
 @verbatim
         Author : SYChiu       <SYChiu@airoha.com.tw>
                  BrianChen <BrianChen@airoha.com.tw>
                  MachiWu     <MachiWu@airoha.com.tw>
 @endverbatim
 */

#include "config.h"
#include "types.h"
#include "dsp_drv_dfe.h"
#include "hal_audio_afe_define.h"

#define AUDIO_SOURCE_DEFAULT_FRAME_SIZE 480
#define AUDIO_SOURCE_DEFAULT_FRAME_NUM 6
#define AUDIO_SINK_DEFAULT_FRAME_SIZE 480
#define AUDIO_SINK_DEFAULT_FRAME_NUM 8// 4 //8
#define AUDIO_SINK_ZEROPADDING_FRAME_NUM 2
#define AUDIO_SBC_FRAME_SAMPLES         120
#define AUDIO_AAC_FRAME_SAMPLES         1024
#define AUDIO_SOURCE_PREFILL_FRAME_NUM  0

#define AUDIO_SOURCE_DEFAULT_ANALOG_VOICE_RATE    16000
#define AUDIO_SOURCE_DEFAULT_ANALOG_AUDIO_RATE    48000
#define AUDIO_SINK_DEFAULT_OUTPUT_RATE            48000

typedef struct Audio_Fade_Ctrl_u
{
    U16  Target_Level;
    U16  Current_Level;
    S16  Step;
    U16  Resolution;
} PACKED Audio_Fade_Ctrl_t, *Audio_Fade_Ctrl_Ptr;

typedef struct Audio_Source_Config_u
{
    U16  Frame_Size;
    U8   Buffer_Frame_Num;
    BOOL Mute_Flag;
    afe_fetch_format_per_sampel_t FetchFormatPerSample;
#ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
    BOOL Pga_mux;
#endif
} Audio_Source_config_t;

typedef struct Audio_Sink_Config_u
{
    U16  Frame_Size;
    U8   Buffer_Frame_Num;
    U8   Target_Q_Frame_Num;
    BOOL Output_Enable;
    BOOL Mute_Flag;
    BOOL SRC_Out_Enable;
    BOOL Pause_Flag;
    BOOL alc_enable;
    U8   Zero_Padding_Cnt;
    U8   Software_Channel_Num;
    afe_fetch_format_per_sampel_t FetchFormatPerSample;
#ifdef ENABLE_HWSRC_CLKSKEW
    clkskew_mode_t clkskew_mode;
#endif
} Audio_Sink_config_t;

typedef struct Audio_VP_Config_u
{
    U16  Frame_Size;
    U8   Buffer_Frame_Num;
    U8   Target_Q_Frame_Num;
    BOOL Output_Enable;
    BOOL Mute_Flag;
} Audio_VP_config_t;


typedef struct Rate_Config_u
{
    U8 Source_Input_Sampling_Rate;
    U8 Source_DownSampling_Ratio;
    U8 Sink_UpSampling_Ratio;
    U8 Sink_Output_Sampling_Rate;
    U8 SRC_Sampling_Rate;
    U8 VP_UpSampling_Ratio;
} Rate_config_t;

typedef struct Audio_Resolution_u
{
    stream_resolution_t AudioInRes;
    stream_resolution_t AudioOutRes;
    stream_resolution_t SRCInRes;
} Audio_Resolution_t;

typedef struct Audio_Interface_GPIO_u
{
    U8 I2S_Master_Group;
    U8 I2S_Slave_Group;
} Audio_Interface_GPIO_t;


typedef struct Stream_audio_Config_s
{
    Audio_Source_config_t Audio_source;
    Audio_Sink_config_t Audio_sink;
    Audio_VP_config_t Audio_VP;
    Rate_config_t  Rate;
    Audio_Resolution_t resolution;
    Audio_Interface_GPIO_t Audio_interface;
}Stream_audio_Config_t, *Stream_audio_Config_Ptr;


EXTERN Stream_audio_Config_Ptr Audio_setting;
EXTERN VOID Audio_Default_setting_init(VOID);
EXTERN U16 AudioSinkFrameSize_Get(VOID);
EXTERN U8 AudioSinkFrameNum_Get(VOID);
EXTERN stream_samplerate_t AudioSourceSamplingRate_Get(VOID);
EXTERN U16 AudioSourceFrameSize_Get(VOID);
EXTERN U8 AudioSourceFrameNum_Get(VOID);
EXTERN U8 AudioSRCSamplingRate_Get(VOID);
EXTERN U8 AudioSRCSamplingRate_Set(stream_samplerate_t rate);
EXTERN stream_samplerate_t AudioSinkSamplingRate_Get(VOID);
EXTERN stream_samplerate_t AudioVpSinkSamplingRate_Get(VOID);


#endif /* __STREAM_AUDIO_SETTING_H__ */

