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

#include "audio_codec_wav_decoder.h"
#include "string.h"
#include "audio_log.h"
#include "hal_audio_internal.h"
/* ------------------------ Memory Allocate -------------------------- */
static int32_t audio_codec_wav_decoder_buffer_init(void *codec_config);
int32_t audio_codec_wav_decoder_get_working_buffer_length(const void *codec_config, uint32_t *length)
{
    wav_decoder_config_t *handle = (wav_decoder_config_t *)codec_config;

    /* 1. input paramter check */
    if (handle == NULL) {
        AUD_LOG_W("[WAV DEC] wav decoder config pointer error\r\n", 0);
        return WAV_CODEC_ERROR;
    }
    handle->decoder_input_buf_size = FOUR_BYTE_ALIGNED(handle->decoder_input_buf_size);
    if (handle->decoder_input_buf_size < 256) {
        AUD_LOG_W("[WAV DEC] wav decoder input buffer size is to small %d byte, error\r\n", 1, handle->decoder_input_buf_size);
        return WAV_CODEC_ERROR;
    }

    /* 2. Get Wav buffer size */
    if (WAV_GetDecBufferSize(handle->decoder_input_buf_size , &(handle->decoder_output_buf_size), &(handle->decoder_working_buf_size)) != WAV_SUCCESS) {
        AUD_LOG_W("[WAV DEC] wav decoder input buffer size is legal, error\r\n", 0);
        return WAV_CODEC_ERROR;
    }
    handle->decoder_output_buf_size  = FOUR_BYTE_ALIGNED(handle->decoder_output_buf_size);
    handle->decoder_working_buf_size = FOUR_BYTE_ALIGNED(handle->decoder_working_buf_size);
    AUD_LOG_I("[WAV DEC] wav decoder: input buffer size %d, output buffer size %d, working buffer size %d\r\n", 3,
        handle->decoder_input_buf_size , handle->decoder_output_buf_size, handle->decoder_working_buf_size);
    *length = handle->decoder_working_buf_size;
    return WAV_CODEC_SUCCESS;
}

int32_t audio_codec_wav_decoder_init(void *codec_config)
{
    wav_decoder_config_t *handle = (wav_decoder_config_t *)codec_config;
    /* 1. input paramter check */
    if (handle == NULL) {
        AUD_LOG_W("[WAV DEC] wav decoder config pointer error\r\n", 0);
        return WAV_CODEC_ERROR;
    }
    if (handle->is_inited) {
        AUD_LOG_W("[WAV DEC] wav decoder already init\r\n", 0);
        return WAV_CODEC_ERROR;
    }
    handle->is_parsed       = false;
    handle->parse_size      = 0;
    handle->is_inited       = false;
    handle->decoder_input_buf_ro = 0;
    return WAV_CODEC_SUCCESS;
}

int32_t audio_codec_wav_decoder_process_direct(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length)
{
    wav_decoder_config_t *handle = (wav_decoder_config_t *)codec_config;
    /* 1. input paramter check */
    if (handle == NULL) {
        AUD_LOG_W("[WAV DEC] wav decoder config pointer error\r\n", 0);
        return WAV_CODEC_ERROR;
    }
    if (handle->is_inited == false) {
        AUD_LOG_W("[WAV DEC] wav decoder is not inited\r\n", 0);
        return WAV_CODEC_ERROR;
    }
    WAV_DEC_HDL *wav_handle   = handle->wav_handle;
    uint32_t    bs_buf_size   = *input_byte_length;
    uint32_t    consumedBytes = 0;
    uint32_t    output_sample_size  = 0;
    /* -2- decode bs_buffer into pcm_buffer */
    if (handle->decoder_output_buf_size > *output_byte_length) {
        //AUD_LOG_W("[WAV DEC] output size is not enough.", 0);
        return WAV_CODEC_LEN_ERROR;
    }
    if (bs_buf_size <= handle->decoder_input_buf_ro) { // No pattern need to be decoded
        AUD_LOG_W("[WAV DEC] bs_buf_size <= handle->decoder_input_buf_ro, decode end", 0);
        return WAV_CODEC_END;
    }
    // WAV_Decode for ALAW ULAW, output_sample_size = 1024, consumedBytes = 1024
    //            for ms ADPCM,  output_sample_size = 2024, consumedBytes = 1024
    //            for ima ADPCM, output_sample_size = 2304, consumedBytes = 1024
    //            for pure PCM,  output_sample_size = 512,  consumedBytes = 1024
    // AUD_LOG_I("[TEST]: handle 0x%x, in_ptr %x, in_size %d, pcm 0x%x, o_ptr 0x%x", 5, wav_handle, input_buffer + handle->decoder_input_buf_ro,
    //                 bs_buf_size, &output_sample_size, output_buffer);
    consumedBytes = WAV_Decode( /* in  */ wav_handle,    /* in  */ input_buffer + handle->decoder_input_buf_ro, /* in */ bs_buf_size,
                                /* out */ output_buffer, /* out */ (unsigned int *)(&output_sample_size)); // decoder_output_buf_offset = 0 才会进, 所以pcm buffer 必然是空的
    if (consumedBytes == 0) {
        AUD_LOG_W("[WAV DEC] WAV_Decode Warning: consumedBytes = 0.", 0);
        return WAV_CODEC_ERROR;
    }
    output_sample_size <<= 1; // outputSamples [o]  output samples counts(1 sample = 2bytes), so need to multiply 2.
    *output_byte_length = output_sample_size;
    handle->decoder_input_buf_ro += consumedBytes;
    AUD_LOG_I("[WAV DEC] WAV_Decode : [input_wav_size] = %d, [output_pcm_size] = %x, [output_total_pcm_size] = %x, consumed_wav_Bytes = %x", 4,
                    bs_buf_size, output_sample_size, *output_byte_length, consumedBytes);
    return WAV_CODEC_SUCCESS;
}

int32_t audio_codec_wav_decoder_get_inout_frame_length(void *codec_config, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length)
{
    wav_decoder_config_t *handle = (wav_decoder_config_t *)codec_config;
    if (handle == NULL) {
        AUD_LOG_W("[WAV DEC] wav decoder config pointer error\r\n", 0);
        return WAV_CODEC_ERROR;
    }
    *oneframe_input_byte_length = 0;
    *oneframe_output_byte_length = 0;
    return WAV_CODEC_SUCCESS;
}

int32_t audio_codec_wav_decoder_direct_parse(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length)
{
    wav_decoder_config_t *handle = (wav_decoder_config_t *)codec_config;
    if (handle == NULL) {
        AUD_LOG_W("[WAV DEC] wav decoder config pointer error\r\n", 0);
        return WAV_CODEC_ERROR;
    }
    WAV_STATUS decode_status       = WAV_SUCCESS;
    uint32_t   data_cnt            = 0;
    uint32_t   consumedBytes       = 0;
    uint32_t   bs_buf_size         = *input_byte_length;
    if (!handle->is_inited) {
        if (audio_codec_wav_decoder_buffer_init(handle) != WAV_CODEC_SUCCESS) {
            AUD_LOG_W("[WAV DEC] wav decoder buffer init fail\r\n", 0);
            return WAV_CODEC_ERROR;
        }
        /* 3. Init Decoder */
        handle->wav_handle = WAV_InitDecoder(handle->decoder_working_buf);
        if (handle->wav_handle == NULL) {
            AUD_LOG_W("[WAV DEC] WAV_InitDecoder error\r\n", 0);
            return WAV_CODEC_ERROR;
        }
        handle->is_inited = true;
        AUD_LOG_I("[WAV DEC] Init success\r\n", 0);
    }
    /* 2. Init Buffer Space */
    if (handle->is_parsed == false) {
        AUD_LOG_I("[WAV DEC] bs_buf_size %d.", 1, bs_buf_size);
        decode_status = WAV_ParseHeader(handle->wav_handle, input_buffer, bs_buf_size, (unsigned int *)(&consumedBytes));
        // update decoder_input_buf
        handle->parse_size  += consumedBytes;
        handle->decoder_input_buf_ro += consumedBytes;
        if (decode_status == WAV_SUCCESS) {
            /* adjust input share ring buffer read pointer */
            // config channel_num and sample_rate
            hal_audio_channel_number_t channel_num = (handle->wav_handle->channel_num == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO;
            handle->channel = channel_num;
            handle->sampling_rate = hal_audio_sampling_rate_value_to_enum(handle->wav_handle->sample_rate);
            AUD_LOG_I("[WAV DEC] WAV PARSE: handle 0x%x, channel_number = %d, sample_rate = %d, total audio data = %d bytes, container = %d, format = %d, data_offset = %d", 7,
                        handle->wav_handle, handle->wav_handle->channel_num, handle->wav_handle->sample_rate, handle->wav_handle->data_chunk_length,
                        handle->wav_handle->container, handle->wav_handle->format, handle->wav_handle->data_offset);
            // initialize some parameters
            handle->is_parsed = true;
            handle->remain_bs = handle->wav_handle->data_chunk_length;
            //handle->wav_handle->data_offset = 0; // reset
            AUD_LOG_E("[WAV DEC] handle %x parsed %d", 2, handle, handle->is_parsed);
            return WAV_CODEC_SUCCESS;
        } else if (decode_status == WAV_REFILL_INBUF) {
            /* do nothing */
            AUD_LOG_I("[WAV DEC] Parser refill data.", 0);
            // update decoder_input_buf
            *input_byte_length = bs_buf_size - consumedBytes;
            return WAV_CODEC_LEN_ERROR;
        } else {
            AUD_LOG_E("[WAV DEC] Unsupported File or Bad File.", 0);
            return WAV_CODEC_ERROR;
        }
    }

    return WAV_CODEC_LEN_ERROR; // continue to parse
}

static int32_t audio_codec_wav_decoder_buffer_init(void *codec_config)
{
    uint32_t int_buf_size = 0;
    /* -1-  Codec Manager Paramater Init ----------------------------------------------------------------------------*/
    AUD_LOG_I("[WAV DEC] wav_codec_buffer_init", 0);
    wav_decoder_config_t *handle = (wav_decoder_config_t *)codec_config;
    handle->decoder_input_buf_size = 1024;
    /* -2-  Codec Manager Get buffer size ---------------------------------------------------------------------------*/
    if (audio_codec_wav_decoder_get_working_buffer_length(handle, &int_buf_size) != WAV_CODEC_SUCCESS) {
        AUD_LOG_E("[WAV DEC] ERROR: get working buffer length fail.", 0);
        return WAV_CODEC_ERROR;
    }
    /* -3-  Buffer Init ---------------------------------------------------------------------------------------------*/
    if (int_buf_size > handle->memory_pool_size) {
        AUD_LOG_E("[WAV DEC] ERROR: memory pool is not enough. %d > %d", 2, int_buf_size, handle->memory_pool_size);
        return WAV_CODEC_ERROR;
    }
    memset(handle->memory_pool_base_addr, 0, handle->memory_pool_size);
    handle->decoder_working_buf = handle->memory_pool_base_addr;
    handle->decoder_working_buf_size = handle->memory_pool_size;
    return WAV_CODEC_SUCCESS;
}

int32_t audio_codec_wav_decoder_deinit(void)
{
    return WAV_CODEC_SUCCESS;
}