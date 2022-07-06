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

#include "audio_codec_mp3_decoder.h"
#include "string.h"
#include "audio_log.h"
#include "audio_dump.h"
#include "FreeRTOS.h"
#include "task.h"

// #define AIR_MP3_DEC_DEBUG
static hal_audio_sampling_rate_t audio_codec_mp3_decoder_convert_ip_sample_rate_index(uint16_t sample_rate_index);
static int32_t audio_codec_mp3_decoder_buffer_init(void *codec_config);

int32_t audio_codec_mp3_decoder_get_working_buffer_length(void *codec_config, uint32_t *length)
{
    mp3_decoder_config_t *handle = (mp3_decoder_config_t *)codec_config;
    if (handle == NULL) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder config pointer error", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    if (handle->is_parsed == false) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder is not parsed", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    // get buffer size
    uint32_t bs_buffer_size = 0;
    if (handle->channel == HAL_AUDIO_MONO) { // mono
        MP3Dec_GetMemSize_Mono((int *)&bs_buffer_size, (int *)&handle->decoder_output_buf_size,
        (int *)&handle->decoder_working_buf1_size, (int *)&handle->decoder_working_buf2_size);
    } else {
        MP3Dec_GetMemSize((int *)&bs_buffer_size, (int *)&handle->decoder_output_buf_size,
        (int *)&handle->decoder_working_buf1_size, (int *)&handle->decoder_working_buf2_size);
    }
    handle->decoder_input_buf_size    = FOUR_BYTE_ALIGNED(bs_buffer_size);
    handle->decoder_output_buf_size   = FOUR_BYTE_ALIGNED(handle->decoder_output_buf_size);
    handle->decoder_working_buf1_size = FOUR_BYTE_ALIGNED(handle->decoder_working_buf1_size);
    handle->decoder_working_buf2_size = FOUR_BYTE_ALIGNED(handle->decoder_working_buf2_size);
#ifdef AIR_MP3_DEC_DEBUG
    AUD_LOG_W("[MP3 DEC] buffer size: dec_in_buffer %d, dec_out_buffer %d, dec_work1_buffer %d, dec_work2_buffer %d", 4,
            handle->decoder_input_buf_size, handle->decoder_output_buf_size, handle->decoder_working_buf1_size,
            handle->decoder_working_buf2_size);
#endif
    // NOTE: length can't be used directly!
    *length = handle->decoder_input_buf_size + handle->decoder_output_buf_size + handle->decoder_working_buf1_size +
              handle->decoder_working_buf2_size;
    return MP3_DEC_CODEC_SUCCESS;
}

int32_t audio_codec_mp3_decoder_get_inout_frame_length(void *codec_config, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length)
{
    mp3_decoder_config_t *handle = (mp3_decoder_config_t *)codec_config;
    if (handle == NULL) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder config pointer error", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    *oneframe_input_byte_length = 0;
    *oneframe_output_byte_length = 0;
    return MP3_DEC_CODEC_SUCCESS;
}
int32_t audio_codec_mp3_decoder_init(void *codec_config)
{
    mp3_decoder_config_t *handle = (mp3_decoder_config_t *)codec_config;
    /* 1. input paramter check */
    if (handle == NULL) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder config pointer error", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    handle->is_parsed       = false;
    handle->decode_size     = 0;
#ifdef AIR_MP3_DEC_DEBUG
    AUD_LOG_I("[MP3 DEC] Init success", 0);
#endif
    return MP3_DEC_CODEC_SUCCESS;
}

int32_t audio_codec_mp3_decoder_process_direct(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length)
{
    mp3_decoder_config_t *handle = (mp3_decoder_config_t *)codec_config;
    /* 1. input paramter check */
    if (handle == NULL) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder config pointer error", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    if (handle->is_inited == false) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder is not inited", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    if (handle->is_parsed == false) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder is not parsed", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    Mp3dec_handle             *mp3_handle         = handle->mp3_handle;
    uint8_t                   *bs_buffer          = input_buffer;
    uint8_t                   *bs_buffer_read_ptr = bs_buffer + handle->decoder_input_buf_ro; // current read
    uint32_t                  bs_buf_size         = *input_byte_length;
    int32_t                   consumedBytes       = 0;
    if ((mp3_handle->PCMSamplesPerCH * mp3_handle->CHNumber * 2) > *output_byte_length) {
        // AUD_LOG_E("[MP3 DEC] output size is larger than container [%u] [%u]", 2, mp3_handle->PCMSamplesPerCH * mp3_handle->CHNumber * 2,
        //     *output_byte_length);
        return MP3_DEC_CODEC_LEN_ERROR;
    }
    if (bs_buf_size <= handle->decoder_input_buf_ro) { // No pattern need to be decoded
        AUD_LOG_W("[MP3 DEC] bs_buf_size <= handle->decoder_input_buf_ro, decode end", 0);
        return MP3_DEC_CODEC_DECODE_END;
    }
    consumedBytes = MP3Dec_Decode(mp3_handle,
                                  output_buffer,
                                  bs_buffer,
                                  bs_buf_size,
                                  bs_buffer_read_ptr);
    *output_byte_length = mp3_handle->PCMSamplesPerCH * mp3_handle->CHNumber * 2;
    if (consumedBytes == 1) { // the last frame
        #ifdef AIR_MP3_DEC_DEBUG
            AUD_LOG_I("[MP3 DEC] decode end success", 0);
        #endif
        return MP3_DEC_CODEC_DECODE_END;
    }  else if (consumedBytes <= 0) {
        // if (consumedBytes == -6) {// means bs_buf_size = 0, no source data }
            AUD_LOG_W("[MP3 DEC] warning: no source data, decode end", 0);
            return MP3_DEC_CODEC_DECODE_END;
    }  else {
        handle->decode_size = mp3_handle->PCMSamplesPerCH * mp3_handle->CHNumber * 2;
        handle->sampling_rate = audio_codec_mp3_decoder_convert_ip_sample_rate_index(mp3_handle->sampleRateIndex);
        handle->decoder_input_buf_ro += consumedBytes; // update new ro
        #ifdef AIR_MP3_DEC_DEBUG
            AUD_LOG_I("[MP3 DEC] MP3 decode, consumeBytes = %d, decode_size = %d, bs_buf_size = %d dec_in_buf_ro %d", 4, consumedBytes,
                handle->decode_size, bs_buf_size, handle->decoder_input_buf_ro);
        #endif
    }
    return MP3_DEC_CODEC_SUCCESS;
}


int32_t audio_codec_mp3_decoder_deinit(void)
{
    return MP3_DEC_CODEC_SUCCESS;
}

int32_t audio_codec_mp3_decoder_direct_parse(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length)
{
    mp3_decoder_config_t *handle = (mp3_decoder_config_t *)codec_config;
    int32_t    status;
    uint32_t   bs_buf_size = *input_byte_length;
    if (handle == NULL) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder config pointer error", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    if (bs_buf_size == 0) {
        AUD_LOG_E("[MP3 DEC] ERROR: parse bs buffer no data", 0);
        return MP3_DEC_CODEC_PARAM_ERROR;
    }
    if (!handle->is_parsed) {
#ifdef AIR_MP3_DEC_DEBUG
        AUD_LOG_I("[MP3 DEC] parse bs buffer size = %d", 1, bs_buf_size);
#endif
        status = MP3Dec_GetChannelNumber(input_buffer, bs_buf_size);
        if ((status == 1) || (status == 2)) {
            AUD_LOG_I("[MP3 DEC] mp3 channel %d", 1, status);
            #ifndef MTK_MP3_STEREO_SUPPORT
                if (status == 2) {
                    AUD_LOG_E("[MP3 DEC] stereo format is not support", 0);
                    configASSERT(0);
                }
            #endif /* MTK_MP3_STEREO_SUPPORT */
            handle->channel = status == 1 ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO;
            handle->is_parsed = true;
            audio_codec_mp3_decoder_buffer_init(handle);
            handle->is_inited = true;
            #ifdef AIR_AUDIO_MIXER_SUPPORT_ENABLE
                MP3Dec_Decode(handle->mp3_handle,
                                NULL,
                                input_buffer,
                                bs_buf_size,
                                input_buffer);
                handle->sampling_rate = audio_codec_mp3_decoder_convert_ip_sample_rate_index(handle->mp3_handle->sampleRateIndex);
            #endif
            handle->decoder_input_buf_ro = 0;
            return MP3_DEC_CODEC_SUCCESS;
        } else {
            *input_byte_length = bs_buf_size - 4; // Avoid missing MP3 Sync Frame
            // handle->decoder_input_buf_ro = bs_buf_size - 4; // update read pointer
            AUD_LOG_W("[MP3 DEC] parse status = %d, refill bs_buffer", 1, status);
            return MP3_DEC_CODEC_LEN_ERROR;
        }
    }
    return MP3_DEC_CODEC_SUCCESS;
}

static int32_t audio_codec_mp3_decoder_buffer_init(void *codec_config)
{
    mp3_decoder_config_t *handle = (mp3_decoder_config_t *)codec_config;
    uint32_t buffer_size_total = 0;
    uint8_t *memory_base = NULL;
    /* 1. input paramter check */
    if (handle == NULL) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder config pointer error", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    if (handle->is_parsed == false) {
        AUD_LOG_W("[MP3 DEC] mp3 decoder is not parsed", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    if (handle->channel == HAL_AUDIO_STEREO) {
        #ifndef MTK_MP3_STEREO_SUPPORT
            AUD_LOG_E("[MP3 DEC] please enable stereo format", 0);
            configASSERT(0);
            return MP3_DEC_CODEC_ERROR;
        #endif
    }
    /* -1-  Get Buffer Size -----------------------------------------------------------------------------------------*/
    audio_codec_mp3_decoder_get_working_buffer_length(handle, &buffer_size_total);
    if (buffer_size_total > handle->memory_pool_size) {
        AUD_LOG_E("[MP3 DEC] mp3 buffer size is not enough %u > %u", 2, buffer_size_total, handle->memory_pool_size);
    }
    /* -2-  alloc memory --------------------------------------------------------------------------------------------*/
    memory_base = handle->memory_pool_base_addr;
    memset(memory_base, 0, handle->memory_pool_size);
    // allocate inter
    handle->decoder_working_buf1 = memory_base;
    memory_base += handle->decoder_working_buf1_size;

    handle->decoder_working_buf2 = memory_base;
    memory_base += handle->decoder_working_buf2_size;
    /* -2-  Codec Manager Paramater Init ----------------------------------------------------------------------------*/
    if(handle->channel == HAL_AUDIO_MONO) {
        handle->mp3_handle = MP3Dec_Init_Mono(handle->decoder_working_buf1, handle->decoder_working_buf2);
    } else {
        handle->mp3_handle = MP3Dec_Init(handle->decoder_working_buf1, handle->decoder_working_buf2);
    }
    if (handle->mp3_handle == NULL) {
        AUD_LOG_E("[MP3 DEC] MP3 DEC IP init failed.", 0);
        return MP3_DEC_CODEC_ERROR;
    }
    AUD_LOG_I("[MP3 DEC] mp3 buffer init done, 0x%x", 1, handle->mp3_handle);
    return MP3_DEC_CODEC_SUCCESS;
    /* -1-  Codec Manager Paramater Init ----------------------------------------------------------------------------*/
}

static hal_audio_sampling_rate_t audio_codec_mp3_decoder_convert_ip_sample_rate_index(uint16_t sample_rate_index)
{

    hal_audio_sampling_rate_t hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;

    switch (sample_rate_index) {
        case 0:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 1:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        case 2:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 3:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_22_05KHZ;
            break;
        case 4:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
            break;
        case 5:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 6:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_11_025KHZ;
            break;
        case 7:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_12KHZ;
            break;
        case 8:
            hal_audio_sampling_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
            break;
    }
    return hal_audio_sampling_rate;
}