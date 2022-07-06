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

#ifndef __AUDIO_CODEC_OPUS_DECODER_H__
#define __AUDIO_CODEC_OPUS_DECODER_H__

#include "hal_audio_internal.h"
#ifdef __cplusplus
extern "C" {
#endif
typedef enum {
    OPUS_CODEC_DEC_PARAM_ERROR = -3,
    OPUS_CODEC_DEC_LEN_ERROR = -2,
    OPUS_CODEC_DEC_ERROR = -1,
    OPUS_CODEC_DEC_SUCCESS = 0,
    OPUS_CODEC_DEC_END = 1,
} opus_codec_dec_result_t;
typedef enum {
    OGG_packet_ready         = 70,
    OGG_packet_celt_mode     = 71,
    OGG_OK                   = 0,
    OGG_mem_alloc_err        = -11,
    OGG_stream_check_err     = -13,
    OGG_serialno_err         = -14,
    OGG_version_err          = -15,
    OGG_os_lacing_expand_err = -16,
    OGG_os_body_expand_err   = -17,
    OGG_mem_overflow         = -18,
    OGG_page_crc_chksum_err  = -19,
    OGG_page_sequence_err    = -20,
    OPUS_framesize_err       = -21,
    OPUS_channel_err         = -22,
    OPUS_samplerate_err      = -23,
    OPUS_celt_only_err       = -24,
} ogg_status_t;

typedef struct {
    unsigned short byte_read;
    unsigned short byte_consumed;
    unsigned short init_done;
    unsigned short exec_done;
} ogg_interface;

#define OPUS_BufInSize      256
#define OPUS_FrameeSize     960

typedef struct {
    ogg_interface     *ogg_info;
    void*             opus_handle;
    void*             ogg_handle;
    uint8_t                     *memory_pool_base_addr;
    uint32_t                    memory_pool_size;
    char              *decoder_input_buf;
    uint32_t          decoder_input_buf_offset; // the wo of buf_in[]
    uint32_t          decoder_input_buf_size;     /** bitstream buffer size*/
    short             *decoder_output_buf;
    uint32_t          decoder_output_buf_offset; // the wo of buf_out[]
    uint32_t          decoder_output_buf_size;    /** pcm buffer size*/
    uint32_t          ogg_working_buf_size;
    uint8_t           *ogg_working_buf;
    uint32_t          opus_working_buf_size;
    uint8_t           *opus_working_buf;
    bool                        is_inited;                  /* it will set when wav decoder init */
    uint32_t                    decode_size;
    uint32_t                    decoder_input_buf_ro;       /* decoder input buffer read pointer */
} opus_decoder_config_t;

int32_t audio_codec_opus_decoder_get_working_buffer_length(void *codec_config, uint32_t *length);
int32_t audio_codec_opus_decoder_get_inout_frame_length(void *codec_config, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length);
int32_t audio_codec_opus_decoder_init(void *codec_config);
int32_t audio_codec_opus_decoder_process_direct(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length);
int32_t audio_codec_opus_decoder_deinit(void);
int32_t audio_codec_opus_decoder_direct_parse(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length);
#ifdef __cplusplus
}
#endif

#endif//__AUDIO_CODEC_OPUS_DECODER_H__