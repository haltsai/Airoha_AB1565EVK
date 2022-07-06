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

#ifndef _SOURCE_INTER_H__
#define _SOURCE_INTER_H__

/*!
 *@file   Sink_private.h
 *@brief  defines the detail structure of sink
 *
 @verbatim
 @endverbatim
 */
//-
#include "types.h"
#include "stream_config.h"
#include "transform_.h"
#include "source_.h"
#include "common.h"

////////////////////////////////////////////////////////////////////////////////
// CONSTANT DEFINITIONS ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define SOURCE_TYPE_L2CAP_NUM   8
#define SOURCE_TYPE_DSP_NUM     4
#define SOURCE_TYPE_RFCOMM_NUM 2

////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EXTERN SOURCE Source_blks[];
EXTERN U8 MapAddr[];


////////////////////////////////////////////////////////////////////////////////
// TYPE DEFINITIONS ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
typedef BOOL (*source_read_buffer_entry)(SOURCE source, U8 *dst_addr, U32 length);


typedef enum {
    SOURCE_TYPE_HOST,
    SOURCE_TYPE_UART,
    SOURCE_TYPE_SCO,
    SOURCE_TYPE_N9SCO,
    SOURCE_TYPE_DSP_MIN,
    SOURCE_TYPE_DSP_0_AUDIO_PATTERN = SOURCE_TYPE_DSP_MIN,
    SOURCE_TYPE_DSP_AUDIOQ,
    SOURCE_TYPE_DSP_BRANCH,
    SOURCE_TYPE_DSP_BRANCH_0 = SOURCE_TYPE_DSP_BRANCH,
    SOURCE_TYPE_DSP_BRANCH_1,
    SOURCE_TYPE_DSP_BRANCH_MAX = SOURCE_TYPE_DSP_BRANCH_1,
    SOURCE_TYPE_DSP_MAX = SOURCE_TYPE_DSP_MIN + SOURCE_TYPE_DSP_NUM - 1,
    SOURCE_TYPE_USBAUDIOCLASS,
    SOURCE_TYPE_USBCDCCLASS,
    SOURCE_TYPE_RINGTONE,
    SOURCE_TYPE_AUDIO,
#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    SOURCE_TYPE_SUBAUDIO_MIN,
    SOURCE_TYPE_SUBAUDIO_0 = SOURCE_TYPE_SUBAUDIO_MIN,
    SOURCE_TYPE_SUBAUDIO_1,
    SOURCE_TYPE_SUBAUDIO_2,
    SOURCE_TYPE_SUBAUDIO_3,
    SOURCE_TYPE_SUBAUDIO_MAX = SOURCE_TYPE_SUBAUDIO_3,
#elif defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
    SOURCE_TYPE_SUBAUDIO_MIN,
    SOURCE_TYPE_SUBAUDIO_0 = SOURCE_TYPE_SUBAUDIO_MIN,
    SOURCE_TYPE_SUBAUDIO_MAX = SOURCE_TYPE_SUBAUDIO_0,
#endif
    SOURCE_TYPE_FILE,
#ifdef MTK_SENSOR_SOURCE_ENABLE
    SOURCE_TYPE_GSENSOR,
#endif
    SOURCE_TYPE_ATT,
    SOURCE_TYPE_HCI,
    SOURCE_TYPE_A2DP,
    SOURCE_TYPE_MEMORY,
    SOURCE_TYPE_AIRAPP,
    SOURCE_TYPE_N9_A2DP,
    SOURCE_TYPE_CM4_PLAYBACK,
    SOURCE_TYPE_CM4_VP_PLAYBACK,
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
    SOURCE_TYPE_CM4_VP_DUMMY_SOURCE_PLAYBACK,
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
    SOURCE_TYPE_N9BLE,
#endif
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    SOURCE_TYPE_AUDIO_TRANSMITTER_MIN,
    SOURCE_TYPE_AUDIO_TRANSMITTER_0 = SOURCE_TYPE_AUDIO_TRANSMITTER_MIN,
    SOURCE_TYPE_AUDIO_TRANSMITTER_1,
    SOURCE_TYPE_AUDIO_TRANSMITTER_2,
    SOURCE_TYPE_AUDIO_TRANSMITTER_3,
    SOURCE_TYPE_AUDIO_TRANSMITTER_4,
    SOURCE_TYPE_AUDIO_TRANSMITTER_5,
    SOURCE_TYPE_AUDIO_TRANSMITTER_6,
    SOURCE_TYPE_AUDIO_TRANSMITTER_7,
    SOURCE_TYPE_AUDIO_TRANSMITTER_MAX = SOURCE_TYPE_AUDIO_TRANSMITTER_7,
#endif
#ifdef MTK_AUDIO_BT_COMMON_ENABLE
    SOURCE_TYPE_BT_COMMON_MIN,
    SOURCE_TYPE_BT_COMMON_0 = SOURCE_TYPE_BT_COMMON_MIN,
    SOURCE_TYPE_BT_COMMON_1,
    SOURCE_TYPE_BT_COMMON_2,
    SOURCE_TYPE_BT_COMMON_3,
    SOURCE_TYPE_BT_COMMON_4,
    SOURCE_TYPE_BT_COMMON_5,
    SOURCE_TYPE_BT_COMMON_6,
    SOURCE_TYPE_BT_COMMON_7,
    SOURCE_TYPE_BT_COMMON_MAX = SOURCE_TYPE_BT_COMMON_7,
#endif
#ifdef AIR_AUDIO_I2S_SLAVE_TDM_ENABLE
    SOURCE_TYPE_TDMAUDIO,
#endif
    SOURCE_TYPE_AUDIO2,
    SOURCE_TYPE_MAX,
} SOURCE_TYPE;

typedef struct __SOURCE_IF {
    U32  (*SourceSize)(SOURCE source);// How many data in source
    U8*  (*SourceMap)(SOURCE source); // For management mode
    VOID (*SourceDrop)(SOURCE source, U32 amount); // Drop data(also update read offset)
    BOOL (*SourceConfigure)(SOURCE source, stream_config_type type, U32 value); //Configure source para
    BOOL (*SourceClose)(SOURCE source); // Close source (include memory free)
    //Airoha
    BOOL (*SourceReadBuf)(SOURCE source, U8 *dst_addr, U32 length); // Write data into *dst_addr

} SOURCE_IF, * SOURCEIF;

struct __SOURCE {

    SOURCE_TYPE      type;

    TRANSFORM        transform;

    BUFFER_TYPE      buftype;

    TaskHandle_t     taskId;

    STREAM_BUFFER    streamBuffer;

    SOURCE_PARAMETER param;

    SOURCE_IF        sif;
};


#endif
