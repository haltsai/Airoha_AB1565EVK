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

#include "audio_codec_manager.h"
#include "FreeRTOS.h"
#include "task.h"
#include "audio_log.h"
#include "exception_handler.h"
#include "hal_audio_internal.h"


typedef struct audio_codec_manager_internal_handle_s {
    bool is_used;
    audio_codec_manager_type_t audio_codec_manager_type;
    audio_codec_manager_config_t audio_codec_manager_config;

    ring_buffer_information_t output_ring_buffer;
    audio_codec_manager_callback_t callback;
    void *user_data;
    uint32_t oneframe_input_byte_length;
    uint32_t oneframe_output_byte_length;
} audio_codec_manager_internal_handle_t;

#define CODEC_HANDLE_MAX  3
static audio_codec_manager_internal_handle_t g_audio_codec_manager_internal_handle[CODEC_HANDLE_MAX];


typedef int32_t (*get_working_buffer_length_t)(void *codec_config, uint32_t *length);
typedef int32_t (*get_inout_frame_length_t)(void *codec_config, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length);
typedef int32_t (*init_t)(void *codec_config);
typedef int32_t (*process_direct_t)(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length);
typedef int32_t (*deinit_t)(void);
typedef int32_t (*parse_t)(void *codec_config, uint8_t *input_buffer, uint32_t *input_byte_length);

typedef struct {
    get_working_buffer_length_t get_working_buffer_length;
    get_inout_frame_length_t get_inout_frame_length;
    init_t init;
    process_direct_t process_direct;
    deinit_t deinit;
    parse_t parse;
} audio_codec_manager_func_t;

audio_codec_manager_func_t audio_codec_manager_func[CODEC_TYPE_MAX] = {
    //get_working_buffer_length,                         get_inout_frame_length,                          init,                          process_direct,                          deinit                              parse
    {audio_codec_none_get_working_buffer_length,         audio_codec_none_get_inout_frame_length,         audio_codec_none_init,         audio_codec_none_process_direct,         audio_codec_none_deinit,            NULL},
#ifdef MTK_OPUS_ENCODER_ENABLE
    {audio_codec_opus_encoder_get_working_buffer_length, audio_codec_opus_encoder_get_inout_frame_length, audio_codec_opus_encoder_init, audio_codec_opus_encoder_process_direct, audio_codec_opus_encoder_deinit,    NULL},
#endif
#ifdef MTK_SBC_ENCODER_ENABLE
    {audio_codec_sbc_encoder_get_working_buffer_length,  audio_codec_sbc_encoder_get_inout_frame_length,  audio_codec_sbc_encoder_init,  audio_codec_sbc_encoder_process_direct,  audio_codec_sbc_encoder_deinit,     NULL},
#endif
#ifdef MTK_WAV_DECODER_ENABLE
    {audio_codec_wav_decoder_get_working_buffer_length,  audio_codec_wav_decoder_get_inout_frame_length,  audio_codec_wav_decoder_init,  audio_codec_wav_decoder_process_direct,  audio_codec_wav_decoder_deinit,     audio_codec_wav_decoder_direct_parse},
#endif
#ifdef AIR_MP3_DECODER_ENABLE
    {audio_codec_mp3_decoder_get_working_buffer_length,  audio_codec_mp3_decoder_get_inout_frame_length,  audio_codec_mp3_decoder_init,  audio_codec_mp3_decoder_process_direct,  audio_codec_mp3_decoder_deinit,     audio_codec_mp3_decoder_direct_parse},
#endif
#ifdef AIR_OPUS_DECODER_ENABLE
    {audio_codec_opus_decoder_get_working_buffer_length,  audio_codec_opus_decoder_get_inout_frame_length,  audio_codec_opus_decoder_init,  audio_codec_opus_decoder_process_direct,  audio_codec_opus_decoder_deinit,     audio_codec_opus_decoder_direct_parse},
#endif
};



audio_codec_manager_status_t audio_codec_get_working_buffer_length(audio_codec_manager_type_t type, audio_codec_manager_config_t *config, uint32_t *working_buffer_length)
{
    audio_codec_manager_status_t st;
    if (type >= CODEC_TYPE_MAX) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    uint32_t len;
    st = audio_codec_manager_func[type].get_working_buffer_length(&config->codec_config, &len);
    *working_buffer_length = (len + 3) / 4;
    return st;
}

audio_codec_manager_status_t audio_codec_get_inout_frame_length(audio_codec_manager_handle_t handle, uint32_t *oneframe_input_byte_length, uint32_t *oneframe_output_byte_length)
{
    audio_codec_manager_status_t st = AUDIO_CODEC_MANAGER_SUCCESS;
    uint32_t id = handle - 1;
    if (handle == (uint32_t)NULL) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if (g_audio_codec_manager_internal_handle[id].is_used == false) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    *oneframe_input_byte_length = g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length;
    *oneframe_output_byte_length = g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length;
    //audio_codec_status_t st = audio_codec_func[type][GET_INOUT_FRAME_LENGTH](g_audio_codec_internal_handle[id].audio_codec_config->codec_config, oneframe_input_byte_length, oneframe_output_byte_length);
    return st;
}

audio_codec_manager_status_t audio_codec_open(audio_codec_manager_type_t type, audio_codec_manager_config_t *config, audio_codec_manager_handle_t *handle)
{
    audio_codec_manager_status_t st;
    if (type >= CODEC_TYPE_MAX) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    vTaskSuspendAll();
    uint32_t id;
    for (id = 0; id < CODEC_HANDLE_MAX; id++) {
        if (g_audio_codec_manager_internal_handle[id].is_used == false) {
            break;
        }
    }

    if (id == CODEC_HANDLE_MAX) {
        xTaskResumeAll();
        AUD_LOG_I("no more audio codec handle can be used\r\n", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    st = audio_codec_manager_func[type].init(&config->codec_config);
    if (st !=  AUDIO_CODEC_MANAGER_SUCCESS) {
        xTaskResumeAll();
        return st;
    }

    memset(&g_audio_codec_manager_internal_handle[id], 0, sizeof(audio_codec_manager_internal_handle_t));
    g_audio_codec_manager_internal_handle[id].is_used = true;
    g_audio_codec_manager_internal_handle[id].audio_codec_manager_type = type;
    g_audio_codec_manager_internal_handle[id].audio_codec_manager_config = *config;
    st = audio_codec_manager_func[type].get_inout_frame_length(&config->codec_config, &g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length, &g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length);
    *handle = id + 1;
    xTaskResumeAll();
    AUD_LOG_I("audio codec create success,handle=%x\r\n", 1, *handle);
    return AUDIO_CODEC_MANAGER_SUCCESS;
}

audio_codec_manager_status_t audio_codec_direct_mode_process(audio_codec_manager_handle_t handle, uint8_t *input_buffer, uint32_t *input_byte_length, uint8_t *output_buffer, uint32_t *output_byte_length)
{

    audio_codec_manager_status_t st;
    uint32_t id = handle - 1;
    uint32_t type = g_audio_codec_manager_internal_handle[id].audio_codec_manager_type;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((g_audio_codec_manager_internal_handle[id].is_used == false) || (g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_mode != CODEC_MODE_DERECT)) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    uint32_t in_len = *input_byte_length;
    uint32_t in_len_already = 0;
    uint32_t out_len = *output_byte_length;
    uint32_t out_len_already = 0;
    uint32_t in_one_len = g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length;
    uint32_t out_one_len = g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length;
    bool ignore_frame_flag = false;
    if ((in_one_len == 0) && (out_one_len == 0)) {
        ignore_frame_flag = true;
    }
    if (ignore_frame_flag) {
        st = audio_codec_manager_func[type].process_direct(&g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_config, input_buffer, input_byte_length, output_buffer, output_byte_length);
        if (st != AUDIO_CODEC_MANAGER_SUCCESS) {
            // return st;
        }
    } else {
        while ((in_len >= in_one_len) && ((out_len_already + out_one_len) <= out_len)) {
            uint32_t in_process_len = in_one_len;
            uint32_t out_process_len = out_one_len;
            st = audio_codec_manager_func[type].process_direct(&g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_config, input_buffer, &in_process_len, output_buffer, &out_process_len);
            if (st != AUDIO_CODEC_MANAGER_SUCCESS) {
                return st;
            }
            in_len -= in_one_len;
            in_len_already += in_one_len;
            out_len_already += out_one_len;
            input_buffer += in_one_len;
            output_buffer += out_one_len;
        }
        *input_byte_length = in_len_already;
        *output_byte_length = out_len_already;
    }
    return st;
}

audio_codec_manager_status_t audio_codec_direct_mode_set_codec_config(audio_codec_manager_handle_t handle, audio_codec_manager_config_t *codec_config)
{
    audio_codec_manager_status_t st;
    uint32_t id = handle - 1;
    uint32_t type = g_audio_codec_manager_internal_handle[id].audio_codec_manager_type;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((g_audio_codec_manager_internal_handle[id].is_used == false) || (g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_mode != CODEC_MODE_DERECT)) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if (codec_config->codec_mode != CODEC_MODE_DERECT) {
        return AUDIO_CODEC_MANAGER_ERROR_PARAM;
    }
    st = audio_codec_manager_func[type].get_inout_frame_length(&codec_config->codec_config, &g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length, &g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length);
    if (st == 0) {
        g_audio_codec_manager_internal_handle[id].audio_codec_manager_config = *codec_config;
        return AUDIO_CODEC_MANAGER_SUCCESS;
    } else {
        return AUDIO_CODEC_MANAGER_ERROR_PARAM;
    }
}

audio_codec_manager_status_t audio_codec_buffer_mode_set_config(audio_codec_manager_handle_t handle, audio_codec_manager_user_config_t *user_config)
{
    uint32_t id = handle - 1;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((g_audio_codec_manager_internal_handle[id].is_used == false) || (g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_mode != CODEC_MODE_BUFFER)) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((user_config->output_buffer == NULL) ||
            (user_config->output_length < g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length) ||
            (user_config->output_length % g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length != 0)) {
        return AUDIO_CODEC_MANAGER_ERROR_PARAM;
    }
    if (user_config->callback == NULL) {
        AUD_LOG_I("codec buffer mode must regist user callback.\r\n", 0);
        configASSERT(0);
    }
    g_audio_codec_manager_internal_handle[id].output_ring_buffer.buffer_base_pointer = user_config->output_buffer;
    g_audio_codec_manager_internal_handle[id].output_ring_buffer.buffer_byte_count = user_config->output_length;
    g_audio_codec_manager_internal_handle[id].callback = user_config->callback;
    g_audio_codec_manager_internal_handle[id].user_data = user_config->user_data;

    return AUDIO_CODEC_MANAGER_SUCCESS;
}


audio_codec_manager_status_t audio_codec_buffer_mode_process(audio_codec_manager_handle_t handle, uint8_t *input_buffer, uint32_t *input_byte_length)
{
    audio_codec_manager_status_t st;
    uint32_t id = handle - 1;
    uint32_t type = g_audio_codec_manager_internal_handle[id].audio_codec_manager_type;

    uint32_t in_len = *input_byte_length;
    uint8_t *in_p = input_buffer;
    uint8_t *pp_buffer = NULL;
    uint32_t p_byte_count = 0;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((g_audio_codec_manager_internal_handle[id].is_used == false) || (g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_mode != CODEC_MODE_BUFFER)) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if (g_audio_codec_manager_internal_handle[id].callback == NULL) {
        AUD_LOG_I("codec buffer mode must regist user callback.\r\n", 0);
        configASSERT(0);
    }
    bool ignore_frame_flag = false;
    if ((g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length == 0) &&
        (g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length == 0)) {
        ignore_frame_flag = true;
    }
    if (ignore_frame_flag) {
    } else {
        while (in_len >= g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length) {
            uint32_t out_len = ring_buffer_get_space_byte_count(&g_audio_codec_manager_internal_handle[id].output_ring_buffer);
            if ((out_len < g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length) && (g_audio_codec_manager_internal_handle[id].callback != NULL)) {
                g_audio_codec_manager_internal_handle[id].callback(handle, AUDIO_CODEC_MANAGER_EVENT_OUTPUT_BUFFER_FULL, g_audio_codec_manager_internal_handle[id].user_data);
            }
            out_len = ring_buffer_get_space_byte_count(&g_audio_codec_manager_internal_handle[id].output_ring_buffer);
            if (out_len < g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length) {
                //uint32_t read_len = g_audio_codec_internal_handle[id].oneframe_output_byte_length - out_len;
                //ring_buffer_read_done(&g_audio_codec_internal_handle[id].output_ring_buffer, read_len);
                *input_byte_length -= in_len;
                return AUDIO_CODEC_MANAGER_SUCCESS;
            }

            ring_buffer_get_write_information(&g_audio_codec_manager_internal_handle[id].output_ring_buffer, &pp_buffer, &p_byte_count);
            uint32_t codec_input_length = g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length;
            uint32_t codec_output_length = g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length;

            st = audio_codec_manager_func[type].process_direct(&g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_config, in_p, &codec_input_length, pp_buffer, &codec_output_length);
            if (st != AUDIO_CODEC_MANAGER_SUCCESS) {
                AUD_LOG_I("unfixbal error.\r\n", 0);
                configASSERT(0);
            }
            if (codec_output_length != g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length) {
                AUD_LOG_I("packet size is not the expected value.\r\n", 0);
                configASSERT(0);
            }

            ring_buffer_write_done(&g_audio_codec_manager_internal_handle[id].output_ring_buffer, g_audio_codec_manager_internal_handle[id].oneframe_output_byte_length);

            in_p += g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length;
            in_len -= g_audio_codec_manager_internal_handle[id].oneframe_input_byte_length;
        }
        *input_byte_length -= in_len;
    }

    return AUDIO_CODEC_MANAGER_SUCCESS;
}

uint32_t audio_codec_buffer_mode_get_output_data_length(audio_codec_manager_handle_t handle)
{
    uint32_t id = handle - 1;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((g_audio_codec_manager_internal_handle[id].is_used == false) || (g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_mode != CODEC_MODE_BUFFER)) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    return ring_buffer_get_data_byte_count(&g_audio_codec_manager_internal_handle[id].output_ring_buffer);
}

audio_codec_manager_status_t audio_codec_buffer_mode_get_read_information(audio_codec_manager_handle_t handle, uint8_t **address, uint32_t *length)
{
    uint32_t id = handle - 1;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((g_audio_codec_manager_internal_handle[id].is_used == false) || (g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_mode != CODEC_MODE_BUFFER)) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    uint8_t *pp_buffer = NULL;
    ring_buffer_get_read_information(&g_audio_codec_manager_internal_handle[id].output_ring_buffer, &pp_buffer, length);
    *address = pp_buffer;
    return AUDIO_CODEC_MANAGER_SUCCESS;
}

audio_codec_manager_status_t audio_codec_buffer_mode_read_done(audio_codec_manager_handle_t handle, uint32_t length)
{
    uint32_t id = handle - 1;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((g_audio_codec_manager_internal_handle[id].is_used == false) || (g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_mode != CODEC_MODE_BUFFER)) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    uint32_t total_data_len = ring_buffer_get_data_byte_count(&g_audio_codec_manager_internal_handle[id].output_ring_buffer);
    if (total_data_len < length) {
        return AUDIO_CODEC_MANAGER_ERROR_LEN;
    } else {
        ring_buffer_read_done(&g_audio_codec_manager_internal_handle[id].output_ring_buffer, length);
        return AUDIO_CODEC_MANAGER_SUCCESS;
    }
}

audio_codec_manager_status_t audio_codec_buffer_mode_clear_output_data(audio_codec_manager_handle_t handle)
{
    uint32_t id = handle - 1;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    if ((g_audio_codec_manager_internal_handle[id].is_used == false) || (g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_mode != CODEC_MODE_BUFFER)) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    g_audio_codec_manager_internal_handle[id].output_ring_buffer.read_pointer = 0;
    g_audio_codec_manager_internal_handle[id].output_ring_buffer.write_pointer = 0;

    return AUDIO_CODEC_MANAGER_SUCCESS;
}


audio_codec_manager_status_t audio_codec_close(audio_codec_manager_handle_t handle)
{
    audio_codec_manager_status_t st;
    uint32_t id = handle - 1;

    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    vTaskSuspendAll();
    uint32_t type = g_audio_codec_manager_internal_handle[id].audio_codec_manager_type;

    if (g_audio_codec_manager_internal_handle[id].is_used == false) {
        xTaskResumeAll();
        AUD_LOG_I("codec haven`t opened", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    st = audio_codec_manager_func[type].deinit();
    if (st !=  AUDIO_CODEC_MANAGER_SUCCESS) {
        AUD_LOG_I("codec can`t deinit", 0);
        xTaskResumeAll();
        return st;
    }

    memset(&g_audio_codec_manager_internal_handle[id], 0 , sizeof(audio_codec_manager_internal_handle_t));
    g_audio_codec_manager_internal_handle[id].is_used = false;
    xTaskResumeAll();

    AUD_LOG_I("close codec handle=%x", 1, handle);
    handle = 0;
    return AUDIO_CODEC_MANAGER_SUCCESS;
}

audio_codec_manager_status_t audio_codec_parse_config(audio_codec_manager_handle_t handle, uint8_t *input_buffer, uint32_t *input_byte_length, audio_codec_manager_config_t *config)
{
    uint32_t id = handle - 1;
    audio_codec_manager_status_t st;
    if (handle == 0) {
        return AUDIO_CODEC_MANAGER_ERROR_PARAM;
    }
    if (g_audio_codec_manager_internal_handle[id].is_used == false) {
        return AUDIO_CODEC_MANAGER_ERROR_PARAM;
    }
    uint32_t type = g_audio_codec_manager_internal_handle[id].audio_codec_manager_type;
    if (audio_codec_manager_func[type].parse == NULL) {
        AUD_LOG_I("no parse function ptr.\r\n", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    st = audio_codec_manager_func[type].parse(&g_audio_codec_manager_internal_handle[id].audio_codec_manager_config.codec_config,
                                                input_buffer, input_byte_length);
    if (st == AUDIO_CODEC_MANAGER_SUCCESS) {
        // config = &(g_audio_codec_manager_internal_handle[id].audio_codec_manager_config);
    }
    return st;
}

audio_codec_manager_config_t *audio_codec_get_codec_config(audio_codec_manager_handle_t handle)
{
    uint32_t id = handle - 1;
    if (handle == 0) {
        return NULL;
    }
    if (g_audio_codec_manager_internal_handle[id].is_used == false) {
        return NULL;
    }
    return  &(g_audio_codec_manager_internal_handle[id].audio_codec_manager_config);
}

