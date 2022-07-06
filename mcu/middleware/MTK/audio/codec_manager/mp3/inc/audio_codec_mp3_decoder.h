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

#ifndef __AUDIO_CODEC_MP3_ENCODER_H__
#define __AUDIO_CODEC_MP3_ENCODER_H__


#include "hal_audio_internal.h"
#include "mp3dec_exp.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    MP3_DEC_CODEC_PARAM_ERROR = -3,
    MP3_DEC_CODEC_LEN_ERROR = -2,
    MP3_DEC_CODEC_ERROR = -1,
    MP3_DEC_CODEC_SUCCESS = 0,
    MP3_DEC_CODEC_DECODE_END  = 1,
} mp3_codec_result_t;

typedef struct {
    Mp3dec_handle               *mp3_handle;                    /** MP3 codec IP used.  */
    uint8_t                     *memory_pool_base_addr;
    uint32_t                    memory_pool_size;
    uint8_t                     *decoder_working_buf1;          /** MP3 codec IP working buffer1. */
    uint8_t                     *decoder_working_buf2;          /** MP3 codec IP working buffer2. */
    uint8_t                     *decoder_input_buf;             /** bitstream buffer*/
    uint8_t                     *decoder_output_buf;            /** pcm buffer*/
    unsigned int                decoder_input_buf_offset;       /** record consumed data(decoder use) offset*/
    unsigned int                decoder_output_buf_offset;      /** record consumed data(fill to decodec_pcm_ring) offset*/
    unsigned int                decoder_working_buf1_size;      /** MP3 codec IP working buffer1 size. */
    unsigned int                decoder_working_buf2_size;      /** MP3 codec IP working buffer2 size. */
    unsigned int                decoder_input_buf_size;         /** bitstream buffer size*/
    unsigned int                decoder_output_buf_size;        /** pcm buffer size*/
    bool                        is_parsed;                      /* it will set when mp3 header is parsed */
    bool                        is_inited;                      /* it will set when mp3 decoder init */
    hal_audio_sampling_rate_t   sampling_rate;
    hal_audio_channel_number_t  channel;
    uint32_t                    decode_size;
    uint32_t                    decoder_input_buf_ro;

} mp3_decoder_config_t;

int32_t audio_codec_mp3_decoder_get_working_buffer_length(void *codec_config, uint32_t *length);
int32_t audio_codec_mp3_decoder_get_inout_frame_length(void *codec_config, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length);
int32_t audio_codec_mp3_decoder_init(void *codec_config);
int32_t audio_codec_mp3_decoder_process_direct(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length);
int32_t audio_codec_mp3_decoder_deinit(void);
int32_t audio_codec_mp3_decoder_direct_parse(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length);
#ifdef __cplusplus
}
#endif

#endif /* __AUDIO_CODEC_MP3_ENCODER_H__ */