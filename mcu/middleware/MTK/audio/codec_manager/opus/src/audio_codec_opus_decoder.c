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

#include "audio_codec_opus_decoder.h"
#include "audio_log.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"

//#define AIR_OPUS_DEC_LOG_DEBUG_ENABLE
static int32_t audio_codec_opus_decoder_buffer_init(void *codec_config);
#ifdef AIR_OPUS_DEC_LOG_DEBUG_ENABLE
    uint32_t g_opus_in_size = 0;
#endif
static uint8_t* g_ogg_info = NULL;

/* The internal function for OGG Layer */
extern int  OGG_PARSER_INIT(void *handle, ogg_interface **ogg_info);
extern int  OGG_PARSER_PROC(char *buf_in, void *handle);
extern int  OGG_OPUS_VP_DEC(short *buf_ou, void *handle, void *opus_handle, int *framesize);
extern int  OPUS_VP_Init(void *handle);
extern int  get_VP_OGG_memsize(void);
extern int  get_VP_OPUS_memsize(void);


int32_t audio_codec_opus_decoder_get_working_buffer_length(void *codec_config, uint32_t *length)
{
    opus_decoder_config_t *handle = (opus_decoder_config_t *)codec_config;
    uint32_t opus_memory_size = 0;
    uint32_t ogg_memory_size = 0;
    opus_memory_size = get_VP_OPUS_memsize();
    ogg_memory_size = get_VP_OGG_memsize();
    handle->ogg_working_buf_size  = FOUR_BYTE_ALIGNED(ogg_memory_size);
    handle->opus_working_buf_size = FOUR_BYTE_ALIGNED(opus_memory_size);
    *length = handle->ogg_working_buf_size + handle->opus_working_buf_size;
    AUD_LOG_I("[OPUS DEC] ogg internal buf size %u. opus internal buf size %u", 2, handle->ogg_working_buf_size, handle->opus_working_buf_size);
    return OPUS_CODEC_DEC_SUCCESS;
}

int32_t audio_codec_opus_decoder_init(void *codec_config)
{
    opus_decoder_config_t *handle = (opus_decoder_config_t *)codec_config;
    int status;
    uint32_t opus_memory_size = 0;
    uint32_t ogg_memory_size = 0;
    if (handle == NULL) {
        AUD_LOG_W("[OPUS DEC] opus decoder config pointer error", 0);
        return OPUS_CODEC_DEC_ERROR;
    }
    memset(handle, 0, sizeof(opus_decoder_config_t));
    handle->ogg_info = pvPortMalloc(sizeof(ogg_interface));
    memset(handle->ogg_info, 0, sizeof(ogg_interface));
    g_ogg_info = handle->ogg_info;
    handle->is_inited = false;
#ifdef AIR_OPUS_DEC_LOG_DEBUG_ENABLE
    g_opus_in_size = 0;
#endif
    return OPUS_CODEC_DEC_SUCCESS;
}
#include "audio_dump.h"
int32_t audio_codec_opus_decoder_direct_parse(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length)
{
    opus_decoder_config_t *handle = (opus_decoder_config_t *)codec_config;
    int status;
    if (handle == NULL) {
        AUD_LOG_W("[OPUS DEC] opus decoder config pointer error", 0);
        return OPUS_CODEC_DEC_ERROR;
    }
    if (!handle->is_inited) {
        if (audio_codec_opus_decoder_buffer_init(handle) != OPUS_CODEC_DEC_SUCCESS) {
            AUD_LOG_E("[OPUS DEC] opus decoder buffer init fail", 0);
            return OPUS_CODEC_DEC_ERROR;
        }
        status = OPUS_VP_Init(handle->opus_handle);
        if (status != OGG_OK) {
            AUD_LOG_E("[OPUS DEC] failed to create the celt decoder. (status = %d)", 1, status);
            return OPUS_CODEC_DEC_ERROR;
        }
        AUD_LOG_I("[OPUS DEC] OGG INIT1 before consume = %d.", 1, handle->ogg_info->byte_consumed);
        status = OGG_PARSER_INIT(handle->ogg_handle, &handle->ogg_info);
        if (status != OGG_OK) {
            AUD_LOG_E("[OPUS DEC] OGG INIT1 error. (status = %d)", 1, status);
            // TODO deinit
            return OPUS_CODEC_DEC_ERROR;
        }
        AUD_LOG_I("[OPUS DEC] OGG INIT1 consume = %d.", 1, handle->ogg_info->byte_consumed);
        if (handle->ogg_info->byte_consumed > OPUS_BufInSize) {
            AUD_LOG_E("[OPUS DEC] OGG INIT1 error. (byte_consumed = %d)", 1, handle->ogg_info->byte_consumed);
            // TODO deinit
            return OPUS_CODEC_DEC_ERROR;
        }
        handle->decoder_input_buf_ro = handle->ogg_info->byte_consumed; // update input buffer [RO]
        handle->is_inited = true;
        AUD_LOG_I("[OPUS DEC] TEST init byte_consume %d", 1, handle->ogg_info->byte_consumed);
        AUD_LOG_I("[OPUS DEC] Init success", 0);
    }

    // Parse the codec internal input buffer or parse the User's buffer.
    if (handle->ogg_info->byte_consumed != 0) {
        handle->ogg_info->byte_read = *input_byte_length;
        //LOG_AUDIO_DUMP(input_buffer, *input_byte_length, PROMPT_VP_OUT);
        #ifdef AIR_OPUS_DEC_LOG_DEBUG_ENABLE
            g_opus_in_size += *input_byte_length;
        #endif
    }
#ifdef AIR_OPUS_DEC_LOG_DEBUG_ENABLE
    AUD_LOG_I("[OPUS DEC] TEST parse: consume %d input %d g_opus_in_size %d", 3, handle->ogg_info->byte_consumed,
        *input_byte_length, g_opus_in_size);
#endif
    status = OGG_PARSER_PROC(input_buffer, handle->ogg_handle);
    if (status != OGG_OK) {
        AUD_LOG_E("[OPUS DEC] OGG INIT2 error. (status = %d)", 1, status);
        return OPUS_CODEC_DEC_ERROR;
    }
    if (!handle->ogg_info->init_done) {
        *input_byte_length = handle->ogg_info->byte_consumed;
        AUD_LOG_I("[OPUS DEC] OGG INIT2 refill data into opus buf_in, consumed %d", 1, handle->ogg_info->byte_consumed);
        return OPUS_CODEC_DEC_LEN_ERROR;
    }
    AUD_LOG_I("[OPUS DEC] OGG INIT2 init done! byte_consumed %d.", 1, handle->ogg_info->byte_consumed);
    handle->decoder_input_buf_ro = handle->ogg_info->byte_consumed; // update input buffer [RO]
    return OPUS_CODEC_DEC_SUCCESS;
}

int32_t audio_codec_opus_decoder_process_direct(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length)
{
    opus_decoder_config_t *handle = (opus_decoder_config_t *)codec_config;
    int status;
    int      framesize   = 0;
    uint32_t bs_buf_size = *input_byte_length;
    if (handle == NULL) {
        AUD_LOG_W("[OPUS DEC] opus decoder config pointer error", 0);
        return OPUS_CODEC_DEC_ERROR;
    }
    if (*output_byte_length < (OPUS_FrameeSize*2)) {
        // AUD_LOG_W("[OPUS DEC] output size is not enough.", 0);
        return OPUS_CODEC_DEC_LEN_ERROR;
    }
    if (handle->ogg_info->byte_consumed != 0) {
        handle->ogg_info->byte_read = *input_byte_length;
        //LOG_AUDIO_DUMP(input_buffer, *input_byte_length, PROMPT_RT_OUT);
        #ifdef AIR_OPUS_DEC_LOG_DEBUG_ENABLE
            g_opus_in_size += *input_byte_length;
        #endif
    }
#ifdef AIR_OPUS_DEC_LOG_DEBUG_ENABLE
    AUD_LOG_I("[OPUS DEC]  byte_consumed %d byte_read %d g_opus_in_size %d ", 3, handle->ogg_info->byte_consumed,
        handle->ogg_info->byte_read, g_opus_in_size);
#endif
    status = OGG_PARSER_PROC(input_buffer, handle->ogg_handle);
    if (status == OGG_page_crc_chksum_err) {
        AUD_LOG_W("[OPUS DEC] OGG Page CRC CheckSum Error.", 0);
        return OPUS_CODEC_DEC_LEN_ERROR;
    } else if (status == OGG_page_sequence_err) {
        AUD_LOG_W("[OPUS DEC] OGG Page Sequence Error.", 0);
        return OPUS_CODEC_DEC_LEN_ERROR;
    } else if (status < 0) {
        AUD_LOG_W("[OPUS DEC] OGG Parser error.  (status = %d)", 1, status);
        // TODO Deinit
        return OPUS_CODEC_DEC_ERROR;
    } else if (status != OGG_packet_ready) {
        AUD_LOG_W("[OPUS DEC] OGG Parser continue.  (status = %d)", 1, status);
        return OPUS_CODEC_DEC_LEN_ERROR;
    }
    // handle->decoder_input_buf_offset = OPUS_BufInSize - handle->ogg_info->byte_consumed;
    status = OGG_OPUS_VP_DEC((short *)output_buffer, handle->ogg_handle, handle->opus_handle, &framesize);
    if (status < 0) {
        AUD_LOG_E("[OPUS DEC] OGG CELT Decoder error. %d", 1, status);
        return OPUS_CODEC_DEC_ERROR;
    } else if (framesize == 0) {
        AUD_LOG_W("[OPUS DEC] OGG CELT Decoder framesize = 0", 0);
        return OPUS_CODEC_DEC_LEN_ERROR;
    }
    handle->decode_size = framesize * 2;
    *output_byte_length = framesize * 2;
    handle->decoder_input_buf_ro += handle->ogg_info->byte_consumed;
#ifdef AIR_OPUS_DEC_LOG_DEBUG_ENABLE
    AUD_LOG_I("[OPUS DEC] decoder_input_buf_ro %d consume %d.", 2, handle->decoder_input_buf_ro, handle->ogg_info->byte_consumed);
#endif
    if (handle->ogg_info->exec_done) {
        return OPUS_CODEC_DEC_END;
    }
    return OPUS_CODEC_DEC_SUCCESS;
}

int32_t audio_codec_opus_decoder_get_inout_frame_length(void *codec_config, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length)
{
    opus_decoder_config_t *handle = (opus_decoder_config_t *)codec_config;
    if (handle == NULL) {
        AUD_LOG_W("[OPUS DEC] opus decoder config pointer error", 0);
        return OPUS_CODEC_DEC_ERROR;
    }
    *oneframe_input_byte_length = 0;
    *oneframe_output_byte_length = 0;
    return OPUS_CODEC_DEC_SUCCESS;
}

static int32_t audio_codec_opus_decoder_buffer_init(void *codec_config)
{
    opus_decoder_config_t *handle = (opus_decoder_config_t *)codec_config;
    uint32_t buf_size = 0;
    uint8_t *memory_base = NULL;
    if (handle == NULL) {
        AUD_LOG_W("[OPUS DEC] opus decoder config pointer error", 0);
        return OPUS_CODEC_DEC_ERROR;
    }
    /* -1- decode input buffer init ------------------------------------------------------------------------------------- */
    audio_codec_opus_decoder_get_working_buffer_length(handle, &buf_size);
    if (handle->memory_pool_size < buf_size) {
        AUD_LOG_E("[OPUS DEC] opus internal memory is not enough", 0);
        return OPUS_CODEC_DEC_ERROR;
    }

    memory_base = handle->memory_pool_base_addr;
    memset(memory_base, 0, handle->memory_pool_size);

    handle->ogg_working_buf  = memory_base;
    memory_base             += handle->ogg_working_buf_size;

    handle->opus_working_buf  = memory_base;
    memory_base              += handle->opus_working_buf_size;

    handle->opus_handle = handle->opus_working_buf;
    handle->ogg_handle  = handle->ogg_working_buf;
    AUD_LOG_I("[OPUS DEC] buffer init success", 0);
    return OPUS_CODEC_DEC_SUCCESS;
}

int32_t audio_codec_opus_decoder_deinit(void)
{
    vPortFree(g_ogg_info);
    g_ogg_info = NULL;
    return OPUS_CODEC_DEC_SUCCESS;
}
