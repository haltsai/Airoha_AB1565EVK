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

#ifndef _STREAM_AUDIO_DRIVER_H_
#define _STREAM_AUDIO_DRIVER_H_

/*!
 *@file   stream_audio_driver.h
 *@brief  defines the setting of audio stream
 *
 @verbatim
         Author : SYChiu   	<SYChiu@airoha.com.tw>
                  BrianChen <BrianChen@airoha.com.tw>
                  MachiWu 	<MachiWu@airoha.com.tw>
 @endverbatim
 */

#include "config.h"
#include "types.h"
#include "source.h"
#include "sink.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "transform_inter.h"
#include "dsp_drv_dfe.h"
#include "stream_config.h"
////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//#define AUTO_RATE_DET

#define OFFSET_OVERFLOW_CHK(preOffset, nowOffset, comparedOffset) (\
                                ((preOffset==nowOffset) && (preOffset==comparedOffset))\
                                    ? TRUE\
                                    : (nowOffset>=preOffset)\
                                        ? (comparedOffset<=nowOffset && comparedOffset>preOffset)\
                                            ? TRUE\
                                            : FALSE\
                                        : (comparedOffset<=nowOffset || comparedOffset>preOffset)\
                                            ? TRUE\
                                            : FALSE)


////////////////////////////////////////////////////////////////////////////////
// Extern Function Declarations ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EXTERN BOOL Stream_Audio_Handler(SOURCE source , SINK sink);
EXTERN VOID Sink_Audio_Path_Init(SINK sink);
EXTERN VOID Sink_Audio_Path_Init_AFE(SINK sink);

EXTERN VOID Sink_Audio_Buffer_Ctrl(SINK sink, BOOL isEnabled);
EXTERN VOID Sink_Audio_BufferInfo_Rst(SINK sink, U32 offset);
EXTERN VOID Sink_Audio_UpdateReadOffset(SINK_TYPE type);
EXTERN VOID Sink_Audio_SubPath_Init(SINK sink);
EXTERN BOOL Sink_Audio_ZeroPadding();
EXTERN BOOL Sink_Audio_ClosureSmooth(SINK sink);
EXTERN VOID Sink_Audio_SRC_Ctrl(SINK sink, BOOL isEnabled, stream_samplerate_t value);
EXTERN VOID Sink_Audio_Triger_SourceSRC(SOURCE source);
EXTERN VOID Sink_Audio_SRC_A_CDM_SamplingRate_Reset(SINK sink, stream_samplerate_t rate);
EXTERN VOID Sink_Audio_ADMAIsrHandler(VOID);
EXTERN VOID Sink_Audio_SRCIsrHandler(VOID);
EXTERN VOID Sink_Audio_VpRtIsrHandler(VOID);
EXTERN VOID SinkBufferUpdateReadPtr(SINK sink, int32_t size);
EXTERN VOID AudioCheckTransformHandle(TRANSFORM transform);
EXTERN BOOL AudioAfeConfiguration(stream_config_type type, U32 value);

EXTERN VOID SourceBufferUpdateWritePtr(SOURCE source, int32_t size);

EXTERN VOID Source_Audio_Pattern_Init(SOURCE source);
EXTERN VOID Source_Audio_Path_Init(SOURCE source);
EXTERN VOID Source_Audio_SRC_Ctrl(SOURCE source, BOOL ctrl, stream_samplerate_t value);
EXTERN VOID Source_Audio_Buffer_Ctrl(SOURCE source, BOOL ctrl);
EXTERN VOID Source_Audio_BufferInfo_Rst(SOURCE source, U32 offset);
EXTERN VOID Source_Audio_Triger_SinkSRC(SINK sink);
EXTERN VOID Source_Audio_IsrHandler(VOID);

EXTERN BOOL Sink_Audio_WriteBuffer (SINK sink, U8 *src_addr, U32 length);
EXTERN BOOL Sink_Audio_FlushBuffer(SINK sink, U32 amount);
EXTERN BOOL Sink_Audio_CloseProcedure(SINK sink);
EXTERN BOOL Sink_Audio_Configuration(SINK sink, stream_config_type type, U32 value);
EXTERN BOOL Source_Audio_ReadAudioBuffer(SOURCE source, U8* dst_addr, U32 length);
EXTERN BOOL Source_Audio_CloseProcedure(SOURCE source);
EXTERN BOOL Source_Audio_Configuration(SOURCE source, stream_config_type type, U32 value);
EXTERN U32  Source_Audio_GetVoicePromptSize(SOURCE source);
EXTERN BOOL Source_Audio_ReadVoicePromptBuffer(SOURCE source, U8 *dst_addr, U32 length);
EXTERN VOID Source_Audio_DropVoicePrompt(SOURCE source, U32 amount);
EXTERN U32  Source_Audio_GetRingtoneSize(SOURCE source);
EXTERN BOOL Source_Audio_ReadRingtoneBuf(SOURCE source, U8 *dst_addr, U32 length);
EXTERN VOID Source_Audio_DropRingtone(SOURCE source, U32 amount);

EXTERN BOOL Sink_AudioQ_Configuration(SINK sink, stream_config_type type, U32 value);
EXTERN BOOL Source_AudioQ_Configuration(SOURCE source, stream_config_type type, U32 value);
EXTERN BOOL Sink_VirtualSink_Configuration(SINK sink, stream_config_type type, U32 value);
EXTERN hal_audio_interconn_selection_t stream_audio_convert_control_to_interconn(hal_audio_control_t audio_control, hal_audio_path_port_parameter_t port_parameter, uint32_t connection_sequence, bool is_input);
EXTERN hal_audio_memory_selection_t hal_memory_convert_dl(hal_audio_memory_t memory);//for compatable ab1552
EXTERN hal_audio_memory_selection_t hal_memory_convert_ul(hal_audio_memory_t memory);//for compatable ab1552

hal_audio_memory_selection_t stream_audio_convert_interconn_to_memory(hal_audio_interconn_selection_t interconn_selection);

EXTERN void Sink_Audio_ExtAmpOff_Control_Init(void);
EXTERN void Sink_Audio_ExtAmpOff_Control_Callback(BOOL SilenceFlag);

#endif /* _STREAM_AUDIO_DRIVER_H_ */

