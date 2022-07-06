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
#include "audio_stream_manager.h"
#ifdef MTK_WAV_DECODER_ENABLE
    #include "wav_codec_exp.h"
#endif
#include "audio_log.h"
#include "string.h"

//#define AIR_AUDIO_STREAM_MANAGER_CMI_LOG_DEBUG_ENABLE
#ifdef AIR_AUDIO_STREAM_MANAGER_CMI_LOG_DEBUG_ENABLE
    uint32_t g_pattern_size = 0;
#endif
audio_codec_manager_status_t audio_stream_manager_fill_decoder_bs(audio_stream_manager_handle_t *stream_handle, uint32_t *bs_buf_size, uint32_t remain_data);
// some special treatment after codec manager open
audio_codec_manager_status_t audio_stream_manager_port_before_init(audio_stream_manager_handle_t *stream_handle)
{
    /* 1. input paramter check */
    if (!stream_handle) {
        AUD_LOG_E("[AUDIO CODEC PORT] ERROR: port before init handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
#ifdef AIR_AUDIO_STREAM_MANAGER_CMI_LOG_DEBUG_ENABLE
    g_pattern_size = 0;
#endif
    /* 2. set the memory base address to codec manager */
    switch (stream_handle->codec_manager_type) {
        #ifdef MTK_WAV_DECODER_ENABLE
            case CODEC_TYPE_WAV_DECODE:
                {
                    wav_decoder_config_t *wav_handle = &(stream_handle->codec_manager_config->codec_config.wav_decoder_config);
                    wav_handle->memory_pool_base_addr = stream_handle->pcm_ring_buf.buffer_base_pointer + stream_handle->pcm_ring_buf.buffer_byte_count;
                    wav_handle->memory_pool_size      = AIR_STREAM_MANAGER_WAV_MEMORY_POOL_SIZE_MAX -
                                                        AIR_STREAM_MANAGER_WAV_BS_BUFFER_SIZE -
                                                        stream_handle->pcm_ring_buf.buffer_byte_count;
                    AUD_LOG_I("[AUDIO CODEC PORT] Codec internal buffer addr 0x%x size %u", 2, wav_handle->memory_pool_base_addr, wav_handle->memory_pool_size);
                }
                break;
        #endif
        #ifdef AIR_OPUS_DECODER_ENABLE
            case CODEC_TYPE_OPUS_DECODE:
                {
                    opus_decoder_config_t *opus_handle = &(stream_handle->codec_manager_config->codec_config.opus_decoder_config);
                    opus_handle->memory_pool_base_addr = stream_handle->pcm_ring_buf.buffer_base_pointer + stream_handle->pcm_ring_buf.buffer_byte_count;
                    opus_handle->memory_pool_size      =    AIR_STREAM_MANAGER_OPUS_MEMORY_POOL_SIZE_MAX -
                                                            AIR_STREAM_MANAGER_OPUS_BS_BUFFER_SIZE -
                                                            stream_handle->pcm_ring_buf.buffer_byte_count;
                    AUD_LOG_I("[AUDIO CODEC PORT] Codec internal buffer addr 0x%x size %u", 2, opus_handle->memory_pool_base_addr, opus_handle->memory_pool_size);
                    // OPUS only support ringbuffer
                    stream_handle->buffer_type = AUDIO_STREAM_MANAGER_RING_BUFFER;
                }
                break;
        #endif
        #ifdef AIR_MP3_DECODER_ENABLE
            case CODEC_TYPE_MP3_DECODE:
                {
                    mp3_decoder_config_t *mp3_handle = &(stream_handle->codec_manager_config->codec_config.mp3_decoder_config);
                    mp3_handle->memory_pool_base_addr = stream_handle->pcm_ring_buf.buffer_base_pointer + stream_handle->pcm_ring_buf.buffer_byte_count;
                    mp3_handle->memory_pool_size      = AIR_STREAM_MANAGER_MP3_MEMORY_POOL_SIZE_MAX -
                                                        AIR_STREAM_MANAGER_MP3_BS_BUFFER_SIZE -
                                                        stream_handle->pcm_ring_buf.buffer_byte_count;
                    AUD_LOG_I("[AUDIO CODEC PORT] Codec internal buffer addr 0x%x size %u", 2, mp3_handle->memory_pool_base_addr, mp3_handle->memory_pool_size);
                }
                break;
        #endif
        default:
            AUD_LOG_W("[AUDIO CODEC PORT] Warning: port before init codec type error %d", 1, stream_handle->codec_manager_type);
            return AUDIO_CODEC_MANAGER_ERROR;
            break;
    }
    return AUDIO_CODEC_MANAGER_SUCCESS;
}
audio_codec_manager_status_t audio_stream_manager_port_after_init(audio_stream_manager_handle_t *stream_handle)
{
    /* 1. input paramter check */
    if (!stream_handle) {
        AUD_LOG_E("[AUDIO CODEC PORT] ERROR: port after init handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    switch (stream_handle->codec_manager_type) {
        #ifdef MTK_WAV_DECODER_ENABLE
            case CODEC_TYPE_WAV_DECODE:
                {
                    wav_decoder_config_t *wav_handle = &(stream_handle->codec_manager_config->codec_config.wav_decoder_config);
                    //stream_handle->src_buffer.read_pointer = stream_handle->codec_manager_config->codec_config.wav_decoder_config.wav_handle->data_offset; // parse success and adjust the read pointer of src buffer.
                    stream_handle->sampling_rate = wav_handle->sampling_rate;
                    stream_handle->channel = wav_handle->channel;
                    stream_handle->stream_out_threshold = stream_handle->pcm_ring_buf.buffer_byte_count;
                    //if it is adpcm, you should transmit the whole file to decoder, not the data chunk.
                    //if ((wav_handle->wav_handle->format == WAV_FORMAT_ALAW) || (wav_handle->wav_handle->format == WAV_FORMAT_ULAW)) {
                        if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
                            // check mem handle size, avoid negative value
                            stream_handle->dec_in_buf.read_pointer = (wav_handle->decoder_input_buf_ro > stream_handle->dec_in_buf.buffer_byte_count) ?
                                stream_handle->dec_in_buf.buffer_byte_count : wav_handle->decoder_input_buf_ro; // drop useless data
                            memmove(stream_handle->dec_in_buf.buffer_base_pointer, \
                                    stream_handle->dec_in_buf.buffer_base_pointer + stream_handle->dec_in_buf.read_pointer, \
                                    stream_handle->dec_in_buf.buffer_byte_count - stream_handle->dec_in_buf.read_pointer);
                            stream_handle->dec_in_buf.write_pointer = stream_handle->dec_in_buf.buffer_byte_count - stream_handle->dec_in_buf.read_pointer;
                        } else {
                            stream_handle->dec_in_buf.buffer_base_pointer += wav_handle->decoder_input_buf_ro;
                            stream_handle->dec_in_buf.buffer_byte_count -= wav_handle->decoder_input_buf_ro;
                            if (stream_handle->dec_in_buf.buffer_byte_count >= 1024) {  // 1024 is the wav decoder input frame size!
                                                                                        // NOTE: If we don't set the max input frame size, decode may fail.
                                stream_handle->dec_in_buf.write_pointer = 1024;
                            } else {
                                stream_handle->dec_in_buf.write_pointer = stream_handle->dec_in_buf.buffer_byte_count; // remain pattern size is less than 1024B
                            }
                        }
                        wav_handle->decoder_input_buf_ro = 0;
                        stream_handle->dec_in_buf.read_pointer = 0;
                    //}
                }
                break;
        #endif
        #ifdef AIR_OPUS_DECODER_ENABLE
            case CODEC_TYPE_OPUS_DECODE:
                {
                    opus_decoder_config_t *opus_handle = &(stream_handle->codec_manager_config->codec_config.opus_decoder_config);
                    stream_handle->sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
                    stream_handle->channel = HAL_AUDIO_MONO;
                    stream_handle->stream_out_threshold = AIR_OPUS_FrameeSize * 2;

                    if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
                        // check mem handle size, avoid negative value
                        stream_handle->dec_in_buf.read_pointer = (opus_handle->decoder_input_buf_ro > stream_handle->dec_in_buf.buffer_byte_count) ?
                            stream_handle->dec_in_buf.buffer_byte_count : opus_handle->decoder_input_buf_ro; // drop useless data
                        memmove(stream_handle->dec_in_buf.buffer_base_pointer, \
                                stream_handle->dec_in_buf.buffer_base_pointer + stream_handle->dec_in_buf.read_pointer, \
                                stream_handle->dec_in_buf.buffer_byte_count - stream_handle->dec_in_buf.read_pointer);
                        stream_handle->dec_in_buf.write_pointer = stream_handle->dec_in_buf.buffer_byte_count - stream_handle->dec_in_buf.read_pointer;
                    } else {
                        stream_handle->dec_in_buf.buffer_base_pointer += opus_handle->decoder_input_buf_ro;
                        stream_handle->dec_in_buf.buffer_byte_count -= opus_handle->decoder_input_buf_ro;
                        if (stream_handle->dec_in_buf.buffer_byte_count >= AIR_OPUS_BufInSize) {  // 1024 is the wav decoder input frame size!
                                                                                    // NOTE: If we don't set the max input frame size, decode may fail.
                            stream_handle->dec_in_buf.write_pointer = AIR_OPUS_BufInSize;
                        } else {
                            stream_handle->dec_in_buf.write_pointer = stream_handle->dec_in_buf.buffer_byte_count; // remain pattern size is less than 1024B
                        }
                    }
                    opus_handle->decoder_input_buf_ro = 0;
                    stream_handle->dec_in_buf.read_pointer = 0;
                }
                break;
        #endif
        #ifdef AIR_MP3_DECODER_ENABLE
            case CODEC_TYPE_MP3_DECODE:
                {
                    mp3_decoder_config_t *mp3_handle = &(stream_handle->codec_manager_config->codec_config.mp3_decoder_config);
                    stream_handle->sampling_rate =  mp3_handle->sampling_rate;
                    stream_handle->channel = mp3_handle->channel;
                    stream_handle->stream_out_threshold = stream_handle->pcm_ring_buf.buffer_byte_count;
                }
                break;
        #endif
        default:
            AUD_LOG_W("[AUDIO CODEC PORT] Warning: port after init codec type error %d", 1, stream_handle->codec_manager_type);
            return AUDIO_CODEC_MANAGER_ERROR;
            break;
    }
    stream_handle->dec_in_buf.read_pointer = 0;
    return AUDIO_CODEC_MANAGER_SUCCESS;
}

// @return: AUDIO_CODEC_MANAGER_ERROR Fail
//          AUDIO_CODEC_MANAGER_ERROR_LEN input or output buf size is not enough, no need to decode
audio_codec_manager_status_t audio_stream_manager_port_before_process(audio_stream_manager_handle_t *stream_handle)
{
    uint32_t bs_buf_size = 0;
    /* 1. input paramter check */
    if (!stream_handle) {
        AUD_LOG_E("[AUDIO CODEC PORT] ERROR: port before process handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
        if (!stream_handle->decode_end_flag) {
            audio_stream_manager_fill_decoder_bs(stream_handle, &bs_buf_size, -1);
        }
        stream_handle->dec_in_buf.write_pointer = bs_buf_size;
    }
    switch (stream_handle->codec_manager_type) {
        #ifdef MTK_WAV_DECODER_ENABLE
            case CODEC_TYPE_WAV_DECODE:
            {
                wav_decoder_config_t *wav_handle = &(stream_handle->codec_manager_config->codec_config.wav_decoder_config);
                if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
                    wav_handle->decoder_input_buf_ro = 0;
                } else {
                    // wav_handle->decoder_input_buf_ro = stream_handle->dec_in_buf.read_pointer; // update input buffer ro/wo
                    stream_handle->dec_in_buf.buffer_base_pointer += wav_handle->decoder_input_buf_ro;
                    stream_handle->dec_in_buf.buffer_byte_count -= wav_handle->decoder_input_buf_ro;
                    if (stream_handle->dec_in_buf.buffer_byte_count >= 1024) {  // 1024 is the wav decoder input frame size!
                                                                                // NOTE: If we don't set the max input frame size, decode may fail.
                        stream_handle->dec_in_buf.write_pointer = 1024;
                    } else {
                        stream_handle->dec_in_buf.write_pointer = stream_handle->dec_in_buf.buffer_byte_count; // remain pattern size is less than 1024B
                    }
                    wav_handle->decoder_input_buf_ro = 0;
                    stream_handle->dec_in_buf.read_pointer = 0;
                }
                // Special Case: If pcm ring buffer is empty, we should reset [ro][wo] to 0!!!
                // It is used to avoid wav decoder output size is not enough.
                if (ring_buffer_get_data_byte_count(&stream_handle->pcm_ring_buf) == 0) {
                    stream_handle->pcm_ring_buf.read_pointer = 0;
                    stream_handle->pcm_ring_buf.write_pointer = 0;
                }
                break;
            }
        #endif
        #ifdef AIR_OPUS_DECODER_ENABLE
            case CODEC_TYPE_OPUS_DECODE:
            {
                opus_decoder_config_t *opus_handle = &(stream_handle->codec_manager_config->codec_config.opus_decoder_config);
                if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
                    opus_handle->decoder_input_buf_ro = 0;
                } else {
                    stream_handle->dec_in_buf.buffer_base_pointer += opus_handle->decoder_input_buf_ro;
                    stream_handle->dec_in_buf.buffer_byte_count -= opus_handle->decoder_input_buf_ro;
                    if (stream_handle->dec_in_buf.buffer_byte_count >= AIR_OPUS_BufInSize) {  // 1024 is the wav decoder input frame size!
                                                                                // NOTE: If we don't set the max input frame size, decode may fail.
                        stream_handle->dec_in_buf.write_pointer = AIR_OPUS_BufInSize;
                    } else {
                        stream_handle->dec_in_buf.write_pointer = stream_handle->dec_in_buf.buffer_byte_count; // remain pattern size is less than 1024B
                    }
                    opus_handle->decoder_input_buf_ro = 0;
                    stream_handle->dec_in_buf.read_pointer = 0;
                }

                break;
            }
        #endif
        #ifdef AIR_MP3_DECODER_ENABLE
            case CODEC_TYPE_MP3_DECODE:
            {
                mp3_decoder_config_t *mp3_handle = &(stream_handle->codec_manager_config->codec_config.mp3_decoder_config);
                if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
                    mp3_handle->decoder_input_buf_ro = 0;
                } else {
                    mp3_handle->decoder_input_buf_ro = stream_handle->dec_in_buf.read_pointer; // update input buffer ro/wo
                }
                break;
            }
        #endif
        default:
            AUD_LOG_W("[AUDIO CODEC PORT] Warning: port before process codec type error %d", 1, stream_handle->codec_manager_type);
            return AUDIO_CODEC_MANAGER_ERROR;
            break;
    }
    if ((ring_buffer_get_data_byte_count(&(stream_handle->src_buffer)) < 1024) && (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER)) {
        // [TODO] Notify APP to copy data into stream manager
    }
    return AUDIO_CODEC_MANAGER_SUCCESS;
}

audio_codec_manager_status_t audio_stream_manager_port_after_process(audio_stream_manager_handle_t *stream_handle)
{
    uint32_t dec_in_buf_ro = 0;
    /* 1. input paramter check */
    if (!stream_handle) {
        AUD_LOG_E("[AUDIO CODEC PORT] ERROR: port after process handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    /* 2. update the decoder in buffer read pointer to stream manager */
    switch (stream_handle->codec_manager_type) {
        #ifdef MTK_WAV_DECODER_ENABLE
            case CODEC_TYPE_WAV_DECODE:
            {
                wav_decoder_config_t *wav_handle = &(stream_handle->codec_manager_config->codec_config.wav_decoder_config);
                dec_in_buf_ro = wav_handle->decoder_input_buf_ro;
                break;
            }
        #endif
        #ifdef AIR_OPUS_DECODER_ENABLE
            case CODEC_TYPE_OPUS_DECODE:
            {
                opus_decoder_config_t *opus_handle = &(stream_handle->codec_manager_config->codec_config.opus_decoder_config);
                dec_in_buf_ro = opus_handle->decoder_input_buf_ro;
                break;
            }
        #endif
        #ifdef AIR_MP3_DECODER_ENABLE
            case CODEC_TYPE_MP3_DECODE:
            {
                mp3_decoder_config_t *mp3_handle = &(stream_handle->codec_manager_config->codec_config.mp3_decoder_config);
                dec_in_buf_ro                = mp3_handle->decoder_input_buf_ro;
                stream_handle->sampling_rate = mp3_handle->sampling_rate;
                stream_handle->channel       = mp3_handle->channel;
                break;
            }
        #endif
        default:
            AUD_LOG_W("[AUDIO CODEC PORT] Warning: port after process codec type error %d", 1, stream_handle->codec_manager_type);
            return AUDIO_CODEC_MANAGER_ERROR;
            break;
    }
    /* 3. modify the input buffer */
    if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
        if (stream_handle->dec_in_buf.write_pointer < dec_in_buf_ro) {
            dec_in_buf_ro = stream_handle->dec_in_buf.write_pointer; // avoid negative copy size
        }
        memmove(stream_handle->dec_in_buf.buffer_base_pointer,
                    stream_handle->dec_in_buf.buffer_base_pointer + dec_in_buf_ro,
                    stream_handle->dec_in_buf.write_pointer - dec_in_buf_ro);
        stream_handle->dec_in_buf.write_pointer -= dec_in_buf_ro;
    } else {
        stream_handle->dec_in_buf.read_pointer = dec_in_buf_ro;
    }
    return AUDIO_CODEC_MANAGER_SUCCESS;
}
// fill data from source buffer into decode input buffer.
audio_codec_manager_status_t audio_stream_manager_fill_decoder_bs(audio_stream_manager_handle_t *stream_handle, uint32_t *bs_buf_size, uint32_t remain_data)
{
    /* 1. input paramter check */
    if (!stream_handle) {
        AUD_LOG_E("[AUDIO CODEC PORT] ERROR: fill bs handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    /* share ring buffer -> decoder_input_buf */
    ring_buffer_information_t *glide_buffer   = &(stream_handle->src_buffer);
    uint32_t                  data_cnt        = ring_buffer_get_data_byte_count(glide_buffer);
    uint32_t                  decoder_in_size = stream_handle->dec_in_buf.buffer_byte_count - stream_handle->dec_in_buf.write_pointer;
    uint32_t                  decoder_in_wo   = stream_handle->dec_in_buf.write_pointer;
    uint8_t                   *p_dst          = (uint8_t *)(stream_handle->dec_in_buf.buffer_base_pointer + stream_handle->dec_in_buf.write_pointer);
    switch (stream_handle->codec_manager_type) {
        #ifdef MTK_WAV_DECODER_ENABLE
            case CODEC_TYPE_WAV_DECODE:
                {
                    wav_decoder_config_t *wav_handle = &(stream_handle->codec_manager_config->codec_config.wav_decoder_config);
                }
                break;
        #endif
        #ifdef AIR_OPUS_DECODER_ENABLE
            case CODEC_TYPE_OPUS_DECODE:
                {
                    opus_decoder_config_t *opus_handle = &(stream_handle->codec_manager_config->codec_config.opus_decoder_config);
                }
                break;
        #endif
        #ifdef AIR_MP3_DECODER_ENABLE
            case CODEC_TYPE_MP3_DECODE:
                {
                    // do nothing
                }
                break;
        #endif
        default:
            AUD_LOG_W("[AUDIO CODEC PORT] Warning: fill bs codec type error %d", 1, stream_handle->codec_manager_type);
            return AUDIO_CODEC_MANAGER_ERROR;
            break;
    }
    data_cnt = MINIMUM(remain_data, data_cnt);
    data_cnt = MINIMUM(data_cnt, decoder_in_size);
    #ifdef AIR_AUDIO_STREAM_MANAGER_CMI_LOG_DEBUG_ENABLE
        AUD_LOG_I("[AUDIO CODEC PORT] fill data: decoder_in_size %d, src_size %d, remain size %d", 3, decoder_in_size,
            data_cnt, remain_data);
    #endif
    if (data_cnt != 0) {
        *bs_buf_size = 0;
        int32_t  loop_idx    = 0;
        uint32_t dst_cnt     = data_cnt;
        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            uint8_t *addr = 0;
            uint32_t size = 0;
            uint32_t read_cnt = 0;
            ring_buffer_get_read_information(glide_buffer, &addr, &size);
            read_cnt = MINIMUM(dst_cnt, size);
            memcpy(p_dst, addr, read_cnt);
            p_dst += read_cnt;
            dst_cnt -= read_cnt;
            ring_buffer_read_done(glide_buffer, read_cnt);
            #ifdef AIR_AUDIO_STREAM_MANAGER_CMI_LOG_DEBUG_ENABLE
                g_pattern_size += read_cnt;
            #endif
        }
        *bs_buf_size += (decoder_in_wo + data_cnt - dst_cnt); // bs_buff_size means handle->decoder_input_buf  write_index
        decoder_in_wo = *bs_buf_size; // update the bs_buf_wo
        if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
            stream_handle->dec_in_buf.write_pointer = decoder_in_wo;
        }
        #ifdef AIR_AUDIO_STREAM_MANAGER_CMI_LOG_DEBUG_ENABLE
            AUD_LOG_I("[AUDIO CODEC PORT] [DEBUG] fill_bs total copy size %u dec_in_buf wo %d", 2, g_pattern_size,
                stream_handle->dec_in_buf.write_pointer);
        #endif
    } else {
        *bs_buf_size = decoder_in_wo;
        #ifdef AIR_AUDIO_STREAM_MANAGER_CMI_LOG_DEBUG_ENABLE
            AUD_LOG_W("[AUDIO CODEC PORT] Warning: share ring buffer underflow or no remain bs, data byte = %d, remain_data = %d dec_in_buf wo %d",
                3, data_cnt, remain_data, decoder_in_wo);
        #endif
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    return AUDIO_CODEC_MANAGER_SUCCESS;
}
