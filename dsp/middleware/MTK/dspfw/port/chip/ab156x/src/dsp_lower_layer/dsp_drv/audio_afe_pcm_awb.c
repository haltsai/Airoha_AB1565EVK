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

extern afe_stream_channel_t connect_type[2][2];
extern bool CM4_Record_air_dump;
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
extern bool CM4_Record_leakage_enable;
#endif
#if 0
static void awb_global_var_init(SOURCE source)
{
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;
    memset(afe_block, 0, sizeof(afe_block_t));
}
#endif

static int32_t pcm_awb_probe(SOURCE source)
{
    //awb_global_var_init(source);
    UNUSED(source);
    return 0;
}

static int32_t pcm_awb_start(SOURCE source)
{
    AUDIO_PARAMETER *runtime = &source->param.audio;
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;
    UNUSED(runtime);
    UNUSED(afe_block);
    DSP_MW_LOG_I("pcm_awb_start enter", 0);
    #if 0//modify for ab1568

    if (runtime->format == AFE_PCM_FORMAT_S32_LE || runtime->format == AFE_PCM_FORMAT_U32_LE) {
        afe_set_memif_fetch_format_per_sample(AUDIO_DIGITAL_BLOCK_MEM_AWB, AFE_WLEN_32_BIT_ALIGN_24BIT_DATA_8BIT_0);
        hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_24BIT, AUDIO_AFE_IO_BLOCK_MEM_AWB);
    } else {
        afe_set_memif_fetch_format_per_sample(AUDIO_AFE_IO_BLOCK_MEM_AWB, AFE_WLEN_16_BIT);
        hal_audio_afe_set_connection_format(AFE_OUTPUT_DATA_FORMAT_16BIT, AUDIO_AFE_IO_BLOCK_MEM_AWB);
    }

    afe_set_samplerate(AUDIO_DIGITAL_BLOCK_MEM_AWB, runtime->rate);
    afe_set_channels(AUDIO_DIGITAL_BLOCK_MEM_AWB, runtime->channel_num);

    if (!source->param.audio.AfeBlkControl.u4awsflag) {
        afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_AWB, true, true);
    }
    DSP_MW_LOG_I("pcm_awb_start exit", 0);
    UNUSED(afe_block);
    #endif
    return 0;
}

static int32_t pcm_awb_stop(SOURCE source)
{
    UNUSED(source);
    #if 0//modify for ab1568
    DSP_MW_LOG_I("pcm_awb_stop enter", 0);

    afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_AWB, true, false);

    DSP_MW_LOG_I("pcm_awb_stop exit", 0);
    UNUSED(source);
    #endif
    return 0;
}

static int32_t pcm_awb_hw_params(SOURCE source)
{
    UNUSED(source);
    return 0;
}

static int32_t pcm_awb_open(SOURCE source)
{
    UNUSED(source);
    #if 0//modify for ab1568
    int ret = 0;
    AUDIO_PARAMETER *runtime = &source->param.audio;
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;

    DSP_MW_LOG_I("pcm_awb_open enter", 0);

    /* Setting the loopback from dl1 to awb here */
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    if(CM4_Record_leakage_enable) {
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_CONNECT, STREAM_M_AFE_M, AUDIO_AFE_IO_BLOCK_HW_GAIN1_OUT_CH1, AUDIO_AFE_IO_BLOCK_MEM_AWB_CH1);
    } else {
#endif
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_CONNECT, STREAM_M_AFE_M, AUDIO_AFE_IO_BLOCK_MEM_DL1_CH1, AUDIO_AFE_IO_BLOCK_MEM_AWB_CH1);
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    }
#endif

    if(CM4_Record_air_dump
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    || CM4_Record_leakage_enable
#endif
    ) {
        /* Setting the loopback from dl2 to awb here */
        DSP_MW_LOG_I("pcm_awb_open, set connection HW_GAIN2 -> AWB", 0);
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_CONNECT, STREAM_M_AFE_M, AUDIO_AFE_IO_BLOCK_HW_GAIN2_OUT_CH1, AUDIO_AFE_IO_BLOCK_MEM_AWB_CH1);
    }

    DSP_MW_LOG_I("pcm_awb_open exit", 0);
    UNUSED(ret);
    UNUSED(runtime);
    UNUSED(afe_block);
    #endif
    return 0;
}

static int32_t pcm_awb_close(SOURCE source)
{
    UNUSED(source);
    DSP_MW_LOG_I("pcm_awb_close enter", 0);
    #if 0//modify for ab1568
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    if(CM4_Record_leakage_enable) {
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_DISCONNECT, STREAM_M_AFE_M, AUDIO_AFE_IO_BLOCK_HW_GAIN1_OUT_CH1, AUDIO_AFE_IO_BLOCK_MEM_AWB_CH1);
    } else {
#endif
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_DISCONNECT, STREAM_M_AFE_M, AUDIO_AFE_IO_BLOCK_MEM_DL1_CH1, AUDIO_AFE_IO_BLOCK_MEM_AWB_CH1);
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    }
#endif

    if(CM4_Record_air_dump
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    || CM4_Record_leakage_enable
#endif
    ) {
        hal_audio_afe_set_intf_connection_state(AUDIO_INTERCONNECTION_DISCONNECT, STREAM_M_AFE_M, AUDIO_AFE_IO_BLOCK_HW_GAIN2_OUT_CH1, AUDIO_AFE_IO_BLOCK_MEM_AWB_CH1);
    }

    DSP_MW_LOG_I("pcm_awb_close exit", 0);
    UNUSED(source);
    #endif
    return 0;
}

static int32_t pcm_awb_trigger(SOURCE source, int cmd)
{
    switch (cmd) {
        case AFE_PCM_TRIGGER_START:
            return pcm_awb_start(source);
            break;
        case AFE_PCM_TRIGGER_STOP:
            return pcm_awb_stop(source);
            break;
        case AFE_PCM_TRIGGER_RESUME:
            return pcm_awb_open(source);
            break;
        case AFE_PCM_TRIGGER_SUSPEND:
            return pcm_awb_close(source);
            break;
        default:
            break;
    }
    return -1;
}

static int32_t pcm_awb_copy(SOURCE source, void *dst, uint32_t count)
{
#if 0
    //copy the AFE src streambuffer to sink streambuffer
    if (Source_Audio_ReadAudioBuffer(source, dst , count) == false) {
        return -1;
    }
#endif
    UNUSED(source);
    UNUSED(dst);
    UNUSED(count);
    return 0;
}

audio_source_pcm_ops_t afe_platform_awb_ops = {
    .probe      = pcm_awb_probe,
    .open       = pcm_awb_open,
    .close      = pcm_awb_close,
    .hw_params  = pcm_awb_hw_params,
    .trigger    = pcm_awb_trigger,
    .copy       = pcm_awb_copy,
};

