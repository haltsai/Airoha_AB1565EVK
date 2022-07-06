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

#include <stdbool.h>
#include <string.h>
#include "audio_codec_sbc_encoder.h"
#include "audio_log.h"
#include "FreeRTOS.h"

#define SBC_HEADER_SIZE 4
static bool sbcEncoderIsInited = false;
static uint8_t *out_p_skip_header = NULL;
static uint32_t out_len_skip_header;
int32_t audio_codec_sbc_encoder_get_working_buffer_length(void *codec_config, uint32_t *length)
{
    *length = sbc_encoder_get_buffer_size();
    return SBC_CODEC_SUCCESS;
}

sbc_codec_result_t audio_codec_sbc_encoder_get_bitpool_from_bitrate(uint32_t bitrate, uint32_t *bitpool, void *codec_config)
{
    uint32_t frame_length_t = 0;
    sbc_encoder_config_t *cfg = (sbc_encoder_config_t *)codec_config;

    uint8_t nrof_channels = (cfg->param.channel_mode == SBC_ENCODER_MONO) ? 1 : 2;
    uint8_t join = (cfg->param.channel_mode == SBC_ENCODER_JOINT_STEREO) ? 1 : 0;
    uint8_t nrof_subbands = cfg->param.subband_number;
    uint8_t nrof_blocks = cfg->param.block_number;
    uint32_t fs = 0;
    switch (cfg->param.sampling_rate) {
        case SBC_ENCODER_SAMPLING_RATE_16000HZ:
            fs = 16000;
            break;
        case SBC_ENCODER_SAMPLING_RATE_32000HZ:
            fs = 32000;
            break;
        case SBC_ENCODER_SAMPLING_RATE_44100HZ:
            fs = 44100;
            break;
        case SBC_ENCODER_SAMPLING_RATE_48000HZ:
            fs = 48000;
            break;
        default:
            return SBC_CODEC_PARAM_ERROR;
    }

    nrof_channels = (cfg->param.channel_mode == SBC_ENCODER_MONO) ? 1 : 2;
    join = (cfg->param.channel_mode == SBC_ENCODER_JOINT_STEREO) ? 1 : 0;

    frame_length_t = (bitrate * 1000 * nrof_subbands * nrof_blocks) / (8 * fs);

    frame_length_t -= 4 + (4 * nrof_subbands * nrof_channels) / 8;
    if (frame_length_t <= 0) {
        return SBC_CODEC_PARAM_ERROR;
    }
    if (cfg->param.channel_mode == SBC_ENCODER_MONO || cfg->param.channel_mode == SBC_ENCODER_DUAL_CHANNEL) {
        *bitpool = (8 * frame_length_t) / (nrof_blocks * nrof_channels);
    } else if (cfg->param.channel_mode == SBC_ENCODER_STEREO || cfg->param.channel_mode == SBC_ENCODER_JOINT_STEREO) {
        *bitpool = (8 * frame_length_t - join * nrof_subbands) / nrof_blocks;
    } else {
        return SBC_CODEC_PARAM_ERROR;
    }

    if (*bitpool < 2 || *bitpool > 250) {
        return SBC_CODEC_PARAM_ERROR;
    }

    return SBC_CODEC_SUCCESS;
}

int32_t audio_codec_sbc_encoder_get_inout_frame_length(void *codec_config, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length)
{
    uint8_t nrof_channels, nrof_subbands, nrof_blocks, join, bitpool;
    uint32_t frame_length;
    sbc_encoder_config_t *cfg = (sbc_encoder_config_t *)codec_config;

    nrof_channels = (cfg->param.channel_mode == SBC_ENCODER_MONO) ? 1 : 2;
    join = (cfg->param.channel_mode == SBC_ENCODER_JOINT_STEREO) ? 1 : 0;
    nrof_subbands = (cfg->param.subband_number + 1) * 4;
    bitpool = cfg->runtime_param.bitpool_value;
    nrof_blocks = (cfg->param.block_number + 1) * 4;

    *oneframe_input_byte_length = (cfg->param.channel_mode == SBC_ENCODER_MONO) ? 256 : 512; //128sample * 16bit * channels

    if ((cfg->runtime_param.bitpool_value >= 2) && (cfg->runtime_param.bitpool_value <= 250)) {
        frame_length = 4 + (4 * nrof_subbands * nrof_channels) / 8;
        if (cfg->param.channel_mode == SBC_ENCODER_MONO || cfg->param.channel_mode == SBC_ENCODER_DUAL_CHANNEL) {
            frame_length += (nrof_blocks * nrof_channels * bitpool) / 8;
        } else if (cfg->param.channel_mode == SBC_ENCODER_STEREO || cfg->param.channel_mode == SBC_ENCODER_JOINT_STEREO) {
            frame_length += (join * nrof_subbands + nrof_blocks * bitpool) / 8;
        } else {
            return SBC_CODEC_PARAM_ERROR;
        }
        if (cfg->is_skip_header == true) {
            *oneframe_output_byte_length = frame_length - 4;
        } else {
            *oneframe_output_byte_length = frame_length;
        }
    } else {
        return SBC_CODEC_PARAM_ERROR;
    }
    return SBC_CODEC_SUCCESS;
}

int32_t audio_codec_sbc_encoder_init(void *codec_config)
{
    int32_t result = 0;

    if (sbcEncoderIsInited == true) {
        AUD_LOG_I("sbc already inited \r\n", 0);
        return SBC_CODEC_ERROR;
    }
    sbc_encoder_config_t *cfg = (sbc_encoder_config_t *)codec_config;
    if ((cfg->param.allocation_method > 1)
            || (cfg->param.block_number > 3)
            || (cfg->param.channel_mode > 3)
            || (cfg->param.sampling_rate > 3)
            || (cfg->param.subband_number > 1)
            || ((cfg->runtime_param.bitpool_value < 2) || (cfg->runtime_param.bitpool_value > 250))) {
        return SBC_CODEC_PARAM_ERROR;
    }
    result = sbc_encoder_init((void **) & (cfg->sbc_encoder_working_buffer_ptr), (uint8_t *)cfg->sbc_encoder_working_buffer_ptr, &(cfg->param));
    if (result >= 0) {
        if (cfg->is_skip_header == true) {
            uint32_t on_in_len, one_out_len;
            audio_codec_sbc_encoder_get_inout_frame_length(codec_config, &on_in_len, &one_out_len);
            out_len_skip_header = one_out_len + SBC_HEADER_SIZE;
            out_p_skip_header = (uint8_t *)pvPortMalloc(out_len_skip_header);
            if (out_p_skip_header == NULL) {
                AUD_LOG_I("sbc init failed, can not allocate memory for skip header mode\r\n", 0);
                return SBC_CODEC_ERROR;
            }
        }
        sbcEncoderIsInited = true;
        AUD_LOG_I("sbc init success\r\n", 0);
        return SBC_CODEC_SUCCESS;
    }
    AUD_LOG_I("sbc init failed\r\n", 0);
    return SBC_CODEC_ERROR;
}

int32_t audio_codec_sbc_encoder_process_direct(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length)
{
    int32_t result = 0;

    if (sbcEncoderIsInited == false) {
        return SBC_CODEC_ERROR;
    }

    sbc_encoder_config_t *cfg = (sbc_encoder_config_t *)codec_config;
    uint32_t in_cnt = *input_byte_length;

    if (cfg->is_skip_header != true) {
        result = sbc_encoder_process(cfg->sbc_encoder_working_buffer_ptr, (int16_t *)input_buffer, &in_cnt, output_buffer, output_byte_length, &(cfg->runtime_param));
        if (result >= 0) {
            return SBC_CODEC_SUCCESS;
        }
    } else {
        uint32_t out_len = out_len_skip_header;
        result = sbc_encoder_process(cfg->sbc_encoder_working_buffer_ptr, (int16_t *)input_buffer, &in_cnt, out_p_skip_header, &out_len, &(cfg->runtime_param));
        if (result >= 0) {
            *output_byte_length = out_len - SBC_HEADER_SIZE;
            memcpy(output_buffer, out_p_skip_header + SBC_HEADER_SIZE, *output_byte_length);
            return SBC_CODEC_SUCCESS;
        }
    }
    return SBC_CODEC_ERROR;
}
int32_t audio_codec_sbc_encoder_deinit(void)
{
    if (sbcEncoderIsInited == false) {
        AUD_LOG_I("sbc deinit fail\r\n", 0);
        return SBC_CODEC_ERROR;
    } else {
        vPortFree(out_p_skip_header);
        out_p_skip_header = NULL;
        sbcEncoderIsInited = false;
        AUD_LOG_I("sbc deinit success\r\n", 0);
        return SBC_CODEC_SUCCESS;
    }
}