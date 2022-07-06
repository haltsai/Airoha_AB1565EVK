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
#include "dsp_feature_interface.h"
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_callback.h"
#include "dsp_audio_process.h"
#include "dsp_memory.h"
/**
 *
 *  Definition
 *
 */
#define FEA_SINK_AUDIO_RECOVER              (FALSE)
#define SINK_AUDIO_RECOVER_DEBUG            (FALSE)
#define SINK_AUDIO_STEP_OF_MUTED_SAMPLES    (20000)
#define SINK_AUDIO_MAX_PENALTY_MULTIPLIER   (5)


/**
 *
 *  Type Definition
 *
 */
typedef struct SINK_AUDIO_MUTE_RECOVER_CTRL_s
{
    U32 UnmuteSamples;
    U32 SuccessiveMutePenalty;
    U32 CodecMultiplier;
    BOOL SourceMuted;
    BOOL SinkRecoverMute;
    U16 DebugIndex;
} SINK_AUDIO_MUTE_RECOVER_CTRL_t;


/**
 *
 *  Buffer & Control
 *
 */
SINK_AUDIO_MUTE_RECOVER_CTRL_t AudioSinkMuteRecoverCtrl;


/**
 *
 * External Symbols
 *
 */


/**
 *
 * Function Prototype
 *
 */
VOID Mute_Smooth_NotifySourceMute (U32 Samples);
VOID Mute_Smooth_NotifySourceUnmute (U32 Samples);


/**
 * stream_function_mute_smooth_initialize
 *
 * This function is used to init memory space for mute smother process
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_function_mute_smooth_initialize (void *para)
{
    UNUSED(para);

    AudioSinkMuteRecoverCtrl.UnmuteSamples = 0;
    AudioSinkMuteRecoverCtrl.SuccessiveMutePenalty = 0;
    AudioSinkMuteRecoverCtrl.SinkRecoverMute = FALSE;
    AudioSinkMuteRecoverCtrl.SourceMuted = TRUE;

    return FALSE;
}

/**
 * stream_function_mute_smooth_process
 *
 * The main process for mute smoother
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_mute_smooth_process (void *para)
{
    U16 Length     = (U16)stream_function_get_output_size(para);
    U8* InBufLeft  = stream_function_get_1st_inout_buffer(para);
    U8* InBufRight = stream_function_get_2nd_inout_buffer(para);
    U16 Samples    = 0;

    if (stream_function_get_output_resolution(para) == RESOLUTION_16BIT)
    {
        Samples = Length/sizeof(U16);
    }
    else if (stream_function_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        Samples = Length/sizeof(U32);
    }

    if (stream_codec_get_mute_flag(para) == FALSE)
    {
        Mute_Smooth_NotifySourceUnmute(Samples);
    }
    else
    {
        Mute_Smooth_NotifySourceMute(Samples);
    }

    if (AudioSinkMuteRecoverCtrl.SinkRecoverMute)
    {
        memset(InBufLeft, 0, Length);
        if (InBufRight != NULL) {
            memset(InBufRight, 0, Length);
        }
    }

    return FALSE;
}


VOID Mute_Smooth_NotifySourceMute (U32 Samples)
{
    UNUSED(Samples);

    if (!AudioSinkMuteRecoverCtrl.SourceMuted)
    {
        AudioSinkMuteRecoverCtrl.SourceMuted = TRUE;

        if (AudioSinkMuteRecoverCtrl.UnmuteSamples < SINK_AUDIO_STEP_OF_MUTED_SAMPLES * SINK_AUDIO_MAX_PENALTY_MULTIPLIER)
        {
            AudioSinkMuteRecoverCtrl.SuccessiveMutePenalty
                = (AudioSinkMuteRecoverCtrl.SuccessiveMutePenalty + 1 > SINK_AUDIO_MAX_PENALTY_MULTIPLIER)
                    ? SINK_AUDIO_MAX_PENALTY_MULTIPLIER
                        : AudioSinkMuteRecoverCtrl.SuccessiveMutePenalty + 1;
            AudioSinkMuteRecoverCtrl.SinkRecoverMute = TRUE;
        }
    }
}


VOID Mute_Smooth_NotifySourceUnmute (U32 Samples)
{
    if (AudioSinkMuteRecoverCtrl.SourceMuted)
    {
        AudioSinkMuteRecoverCtrl.SourceMuted = FALSE;
        AudioSinkMuteRecoverCtrl.UnmuteSamples = 0;
    }

    AudioSinkMuteRecoverCtrl.UnmuteSamples += Samples;

    if (AudioSinkMuteRecoverCtrl.SinkRecoverMute)
    {
        if (AudioSinkMuteRecoverCtrl.UnmuteSamples > SINK_AUDIO_STEP_OF_MUTED_SAMPLES * (AudioSinkMuteRecoverCtrl.SuccessiveMutePenalty + 1))
        {
            AudioSinkMuteRecoverCtrl.SinkRecoverMute = FALSE;
            AudioSinkMuteRecoverCtrl.SuccessiveMutePenalty = 0;
        }
    }
}


