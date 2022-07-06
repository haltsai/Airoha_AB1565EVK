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
#include "audio_codec_opus_encoder.h"
#include "opus_encoder_interface.h"
#include "audio_log.h"


static bool opusEncoderIsInited = false;

int32_t audio_codec_opus_encoder_get_working_buffer_length(void *codec_config, uint32_t *length)
{
    *length = (uint32_t) opus_encoder_get_size();
    return OPUS_CODEC_SUCCESS;
}
int32_t audio_codec_opus_encoder_get_inout_frame_length(void *codec_config, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length)
{
    opus_encoder_config_t *cfg = (opus_encoder_config_t *)codec_config;

    if ((cfg->param.channel != OPUS_CODEC_CHANNEL_MONO) || (cfg->param.samplerate != OPUS_CODEC_SAMPLERATE_16KHZ)) {
        return OPUS_CODEC_PARAM_ERROR;
    }
    switch (cfg->param.bitrate) {
        case OPUS_CODEC_BITRATE_16KBPS:
            *oneframe_output_byte_length = 40;
            break;
        case OPUS_CODEC_BITRATE_32KBPS:
            *oneframe_output_byte_length = 80;
            break;
        case OPUS_CODEC_BITRATE_64KBPS:
            *oneframe_output_byte_length = 160;
            break;
        default:
            return OPUS_CODEC_PARAM_ERROR;
    }
    *oneframe_input_byte_length = 640;
    return OPUS_CODEC_SUCCESS;
}

int32_t audio_codec_opus_encoder_init(void *codec_config)
{
    int32_t result = 0;

    if (opusEncoderIsInited == true) {
        AUD_LOG_I("opus already inited \r\n", 0);
        return OPUS_CODEC_ERROR;
    }

    opus_encoder_config_t *cfg = (opus_encoder_config_t *)codec_config;
    if (cfg->param.channel != OPUS_CODEC_CHANNEL_MONO) {
        return OPUS_CODEC_PARAM_ERROR;
    }

    if ((cfg->param.bitrate != OPUS_CODEC_BITRATE_16KBPS) && (cfg->param.bitrate != OPUS_CODEC_BITRATE_32KBPS) && (cfg->param.bitrate != OPUS_CODEC_BITRATE_64KBPS)) {
        return OPUS_CODEC_PARAM_ERROR;
    }

    if (cfg->param.samplerate != OPUS_CODEC_SAMPLERATE_16KHZ) {
        return OPUS_CODEC_PARAM_ERROR;
    }

    result = opus_encoder_init((uint8_t *)cfg->opus_encoder_working_buffer_ptr);
    if (result >= 0) {
        opusEncoderIsInited = true;
        AUD_LOG_I("opus init success\r\n", 0);
        return OPUS_CODEC_SUCCESS;
    }
    AUD_LOG_I("opus init failed\r\n", 0);
    return OPUS_CODEC_ERROR;
}

int32_t audio_codec_opus_encoder_process_direct(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length)
{
    int32_t result = 0;

    if (opusEncoderIsInited == false) {
        return OPUS_CODEC_ERROR;
    }

    opus_encoder_config_t *cfg = (opus_encoder_config_t *)codec_config;
    uint32_t in_cnt = *input_byte_length;
    if (in_cnt != 640) {
        return OPUS_CODEC_PARAM_ERROR;
    }

    result = opus_encoder_process((uint8_t *)cfg->opus_encoder_working_buffer_ptr, output_buffer, (short *)input_buffer, cfg->param.bitrate);
    if (result < 0) {
        return OPUS_CODEC_ERROR;
    }

    switch (cfg->param.bitrate) {
        case OPUS_CODEC_BITRATE_16KBPS:
            *output_byte_length = 40;
            break;
        case OPUS_CODEC_BITRATE_32KBPS:
            *output_byte_length = 80;
            break;
        case OPUS_CODEC_BITRATE_64KBPS:
            *output_byte_length = 160;
            break;
        default:
            return OPUS_CODEC_PARAM_ERROR;
    }
    return OPUS_CODEC_SUCCESS;
}

int32_t audio_codec_opus_encoder_deinit(void)
{
    if (opusEncoderIsInited == false) {
        AUD_LOG_I("opus deinit fail\r\n", 0);
        return OPUS_CODEC_ERROR;
    } else {
        opusEncoderIsInited = false;
        AUD_LOG_I("opus deinit success\r\n", 0);
        return OPUS_CODEC_SUCCESS;
    }
}
