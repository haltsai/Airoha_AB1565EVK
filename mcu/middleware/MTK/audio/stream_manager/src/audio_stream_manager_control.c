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
#include "audio_log.h"
#include "audio_codec.h"
#include "bt_sink_srv_am_task.h"
#include "bt_sink_srv_ami.h"
#include "hal_dvfs_internal.h"
#include "hal_audio.h"
#include "hal_gpt.h"
#include "hal_audio_cm4_dsp_message.h"
#include "string.h"
#include "semphr.h"
#include "FreeRTOS.h"

// Debug
//#define AIR_AUDIO_STREAM_MANAGER_LOG_DEBUG_ENABLE
//#define AIR_AUDIO_STREAM_MANAGER_MIPS_DEBUG
#define MSG_MCU2DSP_PROMPT_CLEAR_AUDIO_BUFFER  (0x55U) // only for VP, speed up the open/close flow of VP.
#define AUDIO_STREAM_MANAGER_USER_MAX    (2)
#define AUDIO_STREAM_MANAGER_DSP_PROCESS_FRAME_SIZE (1024)

typedef struct {
    bool is_used;
    audio_stream_manager_handle_t handle;
    void *user_data;
} audio_stream_manager_internal_handle_t; // size:84bytes

#ifdef AIR_AUDIO_STREAM_MANAGER_MIXER_ENABLE
    uint8_t g_stream_manager_mixer_memory_pool[AIR_AUDIO_STREAM_MANAGER_MIXER_MEMORY_POOL_SIZE] = {0};
#endif

/* RAM Memory pool for Codec */
#ifdef AIR_MP3_DECODER_ENABLE
uint8_t g_stream_manager_mp3_memory_pool[AIR_STREAM_MANAGER_MP3_MEMORY_POOL_SIZE_MAX] = {0};
#endif
#ifdef AIR_OPUS_DECODER_ENABLE
uint8_t g_stream_manager_opus_memory_pool[AIR_STREAM_MANAGER_OPUS_MEMORY_POOL_SIZE_MAX] = {0};
#endif
#ifdef MTK_WAV_DECODER_ENABLE
uint8_t g_stream_manager_wav_memory_pool[AIR_STREAM_MANAGER_WAV_MEMORY_POOL_SIZE_MAX] = {0};
#endif

StaticTimer_t g_os_timer_buffer[AUDIO_STREAM_MANAGER_USER_MAX];
audio_stream_manager_internal_handle_t g_audio_stream_manager_handle[AUDIO_STREAM_MANAGER_USER_MAX];
SemaphoreHandle_t g_audio_stream_manager_semaphore_handle = NULL;
static audio_stream_manager_status_t audio_stream_manager_decoder_handler(audio_codec_media_handle_t *handle, void *data);
static void audio_stream_manager_fill_sharebuffer(audio_stream_manager_handle_t *handle);
static audio_stream_manager_status_t audio_stream_manager_mutex_create(void);
static audio_stream_manager_status_t audio_stream_manager_mutex_lock(void);
static audio_stream_manager_status_t audio_stream_manager_mutex_unlock(void);
static void audio_stream_manager_avm_config_start(audio_stream_manager_handle_t *handle);
static void audio_stream_manager_ccni_callback(hal_audio_event_t event, void *data);
static void audio_stream_manager_gpt_timer_callback(TimerHandle_t xTimer);

static audio_codec_manager_status_t audio_stream_manager_cmi_memory_init(audio_stream_manager_handle_t *stream_handle);
static audio_codec_manager_status_t audio_stream_manager_cmi_init(audio_stream_manager_handle_t *stream_handle);
static audio_codec_manager_status_t audio_stream_manager_cmi_process(audio_stream_manager_handle_t *stream_handle);
//static audio_codec_manager_status_t audio_stream_manager_cmi_post_process(audio_stream_manager_handle_t *stream_handle);
static audio_codec_manager_status_t audio_stream_manager_cmi_deinit(audio_stream_manager_handle_t *stream_handle);

extern audio_codec_manager_status_t audio_stream_manager_port_before_init(audio_stream_manager_handle_t *stream_handle);
extern audio_codec_manager_status_t audio_stream_manager_port_after_init(audio_stream_manager_handle_t *stream_handle);
extern audio_codec_manager_status_t audio_stream_manager_port_before_process(audio_stream_manager_handle_t *stream_handle);
extern audio_codec_manager_status_t audio_stream_manager_port_after_process(audio_stream_manager_handle_t *stream_handle);
extern audio_codec_manager_status_t audio_stream_manager_fill_decoder_bs(audio_stream_manager_handle_t *stream_handle, uint32_t *bs_buf_size, uint32_t remain_data);
extern volatile uint8_t g_audio_dl_suspend_by_user;


audio_stream_manager_status_t audio_stream_manager_open(audio_stream_manager_id_t *id, audio_stream_manager_config_t *user_config, audio_stream_manager_callback_t *callback)
{
    *id = -1;
    if ((!callback) || (!user_config)) {
        AUD_LOG_W("[Audio Stream] NOTE: callback ptr or config ptr null", 0);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    if ((user_config->stream_type>= AUDIO_STREAM_MANAGER_TYPE_MAX) || (user_config->stream_type < AUDIO_STREAM_MANAGER_TYPE_MIN)) {
        AUD_LOG_E("[Audio Stream] ERROR: stream user type %d", 1, user_config->stream_type);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    if ((user_config->codec_type>= CODEC_TYPE_MAX) || (user_config->codec_type < CODEC_TYPE_NONE)) {
        AUD_LOG_E("[Audio Stream] ERROR: codec type %d", 1, user_config->codec_type);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    // search unused stream id
    audio_stream_manager_mutex_create();
    audio_stream_manager_mutex_lock();
    uint32_t i = 0;
    for (i = 0; i < AUDIO_STREAM_MANAGER_USER_MAX; i++) {
        if (g_audio_stream_manager_handle[i].is_used == false) {
            break;
        }
    }
    if (i == AUDIO_STREAM_MANAGER_USER_MAX) {
        AUD_LOG_E("[Audio Stream] ERROR: stream user is full", 0);
        audio_stream_manager_mutex_unlock();
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    /* -1-  Init -----------------------------------------------------------------------------------------------------*/
    //AUD_LOG_I("[Audio Stream] sizeof internal handle %d", 1, sizeof(audio_stream_manager_internal_handle_t));
    memset(&g_audio_stream_manager_handle[i], 0, sizeof(audio_stream_manager_internal_handle_t));
    g_audio_stream_manager_handle[i].user_data                           = user_config->user_data;
    audio_stream_manager_handle_t *handle                                = &(g_audio_stream_manager_handle[i].handle);
    handle->src_buffer.buffer_base_pointer = user_config->input_buffer;
    handle->src_buffer.buffer_byte_count   = user_config->input_buffer_size;
    handle->src_buffer.write_pointer       = user_config->input_buffer_size;
    handle->src_buffer.read_pointer        = 0;
    handle->codec_manager_type             = user_config->codec_type;
    handle->app_callback                   = (void *)callback;
    handle->stream_type                    = user_config->stream_type;
    handle->buffer_type                    = user_config->buffer_type;
    handle->os_timer_handle                = NULL;
    handle->os_timer_period                = 0; /* 0ms means that there is no need to create timer */
    handle->eof_flag                       = false;
    handle->decode_end_flag                = false;
    handle->bit_stream_end                 = false;
    handle->handler                        = audio_stream_manager_decoder_handler;
#ifdef AIR_AUDIO_MIXER_SUPPORT_ENABLE
    handle->mixer_track_role               = user_config->mixer_track_role;
#endif
    if (handle->handler) {
        audio_codec_event_register_callback((audio_codec_media_handle_t *)handle, (audio_codec_internal_callback_t)(handle->handler)); // Register to Audio Codec Task
    } else {
        AUD_LOG_E("[Audio Stream] ERROR: no handler function!", 0);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    switch (handle->stream_type) {
        case AUDIO_STREAM_MANAGER_TYPE_VP:
            handle->message_type = AUDIO_MESSAGE_TYPE_PROMPT;
            handle->os_timer_period = 20;
            break;
        default:
            break;
    }
    // OPUS only support ringbuffer
#ifdef AIR_OPUS_DECODER_ENABLE
    if (handle->codec_manager_type == CODEC_TYPE_OPUS_DECODE) {
        handle->buffer_type = AUDIO_STREAM_MANAGER_RING_BUFFER;
    }
#endif
    if (handle->buffer_type == AUDIO_STREAM_MANAGER_LINEAR_BUFFER) {
        handle->dec_in_buf.buffer_base_pointer = handle->src_buffer.buffer_base_pointer;
        handle->dec_in_buf.buffer_byte_count = handle->src_buffer.buffer_byte_count;
        handle->dec_in_buf.read_pointer = 0;
        handle->dec_in_buf.write_pointer = handle->src_buffer.buffer_byte_count;
    }
    // Time-Drivern Model
    handle->os_timer_handle = handle->os_timer_period == 0 ? NULL :
                            xTimerCreateStatic(
                                "audio_timer",
                                pdMS_TO_TICKS(handle->os_timer_period),
                                pdTRUE, /* not one shot timer */
                                (void *) 0,
                                audio_stream_manager_gpt_timer_callback,
                                &(g_os_timer_buffer[i])
                            );
    if ((handle->os_timer_handle == NULL) && (handle->os_timer_period != 0)) {
        /* The timer was not created. */
        AUD_LOG_W("[Audio Stream] os timer create fail.", 0);
    }
    // decoder codec init, NOTE: the example of using handler function
    handle->stream_out = (void *)audio_stream_manager_fill_sharebuffer;
    handle->current_event = AUDIO_CODEC_EVENT_IDLE;
    if (handle->handler((audio_codec_media_handle_t *)handle, NULL) != AUDIO_STREAM_MANAGER_STATUS_OK) {
        goto ERR;
    }
    handle->current_event = AUDIO_CODEC_EVENT_INIT;
    if (handle->handler((audio_codec_media_handle_t *)handle, NULL) != AUDIO_STREAM_MANAGER_STATUS_OK) {
        goto ERR;
    }
#ifdef AIR_AUDIO_MIXER_SUPPORT_ENABLE
    handle->mixer_track_id = audio_mixer_get_track_id(mp3_codec_translate_decoder_ip_sample_rate_index((uint16_t)internal_handle->mp3_handle->sampleRateIndex),
                                                      handle->channel,
                                                      audio_stream_manager_ccni_callback,
                                                      handle,
                                                      handle->mixer_track_role);
    if (handle->mixer_track_id < 0) {
        AUD_LOG_E("[Audio Stream] ERROR: get mixer track id fail, %d", 1, handle->mixer_track_id);
        goto ERR;
    }
#endif
    /* -2-  Pre-decode and Pre-fill Sharebuffer ----------------------------------------------------------------------*/
#ifdef AIR_AUDIO_STREAM_MANAGER_MIPS_DEBUG
    uint32_t curr_cnt1, curr_cnt2, diff_cnt;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt1);
#endif
    handle->current_event = AUDIO_CODEC_EVENT_PRE_PROCESS;
    handle->handler((audio_codec_media_handle_t *)handle, NULL);
#ifdef AIR_AUDIO_STREAM_MANAGER_MIPS_DEBUG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt2);
    hal_gpt_get_duration_count(curr_cnt1, curr_cnt2, &diff_cnt);
    AUD_LOG_I("[Audio Stream] Pre_Process time [%u]us", 1, diff_cnt);
#endif
    *id = i + 1;
    g_audio_stream_manager_handle[i].is_used = true;
    audio_stream_manager_mutex_unlock();
    AUD_LOG_I("[Audio Stream] stream type %d, buffer type %d, data addr 0x%x, audio data length %d, channel %d, sample rate %d", 6,
            handle->codec_manager_type, handle->buffer_type, handle->src_buffer.buffer_base_pointer,
            handle->src_buffer.buffer_byte_count, handle->channel + 1, hal_audio_sampling_rate_enum_to_value(handle->sampling_rate));
    AUD_LOG_I("[Audio Stream] OPEN id[%d]", 1, *id);
    return AUDIO_STREAM_MANAGER_STATUS_OK;
ERR:
    audio_stream_manager_mutex_unlock();
    AUD_LOG_E("[Audio Stream] ERROR: OPEN id[%d] fail", 1, *id);
    return AUDIO_STREAM_MANAGER_STATUS_ERR;
}

audio_stream_manager_status_t audio_stream_manager_start(audio_stream_manager_id_t id)
{
    int32_t index = id - 1;
    /* -1-  Input parameter check  -----------------------------------------------------------------------------------*/
    if ((index < 0) || (index >= AUDIO_STREAM_MANAGER_USER_MAX)) {
        AUD_LOG_E("[Audio Stream] ERROR: start id error %d", 1, id);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    if (g_audio_stream_manager_handle[index].is_used != true) {
        AUD_LOG_E("[Audio Stream] ERROR: start current id is not used", 0);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    audio_stream_manager_handle_t *handle = &(g_audio_stream_manager_handle[index].handle);
    /* -2-  Config DSP Parameters ------------------------------------------------------------------------------------*/
    audio_stream_manager_avm_config_start(handle);
    /* -3-  Special treatment for different stream type --------------------------------------------------------------*/
    if (!handle->aws_sync_request) {
        if (handle->os_timer_handle != NULL) {
            if (xTimerStart(handle->os_timer_handle, 0) != pdPASS) {
                /* The timer could not be set into the Active state. */
                AUD_LOG_E("[Audio Stream] ERROR: start os timer fail", 0);
            }
        }
        if (handle->app_callback) {
            handle->app_callback(AUDIO_CODEC_CB_EVENT_START, NULL);
        }
    }
    AUD_LOG_I("[Audio Stream] START id[%d]", 1, id);
    return AUDIO_STREAM_MANAGER_STATUS_OK;
}

void audio_stream_manager_set_aws(audio_stream_manager_id_t id, bool aws_sync_request, uint32_t aws_sync_time)
{
    int32_t index = id - 1;
    /* -1-  Input parameter check  -----------------------------------------------------------------------------------*/
    if ((index < 0) || (index >= AUDIO_STREAM_MANAGER_USER_MAX)) {
        AUD_LOG_E("[Audio Stream] ERROR: start id error %d", 1, id);
        return;
    }
    if (g_audio_stream_manager_handle[index].is_used != true) {
        AUD_LOG_E("[Audio Stream] ERROR: start current id is not used", 0);
        return;
    }
    audio_stream_manager_handle_t *handle = &(g_audio_stream_manager_handle[index].handle);
    /* -2-  sync parameter config  -----------------------------------------------------------------------------------*/
    uint32_t savedmask;
    hal_nvic_save_and_set_interrupt_mask(&savedmask);
    handle->aws_sync_request = aws_sync_request;
    if (aws_sync_request) {
        handle->aws_sync_time    = aws_sync_time;
    } else {
        handle->aws_sync_time    = 0;
    }
    hal_nvic_restore_interrupt_mask(savedmask);
}

static void audio_stream_manager_fill_sharebuffer(audio_stream_manager_handle_t *handle)
{
    ring_buffer_information_t *p_ring              = &handle->pcm_ring_buf;
    n9_dsp_share_info_t       *p_info              = NULL;
    uint32_t                  dst_cnt              = 0;
    uint32_t                  data_cnt             = 0;
    uint32_t                  used_size            = 0;
    uint8_t                   *stream_out_buf_addr = NULL;
    p_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(handle->message_type);
    if ((!p_ring) || (!p_info)) {
        AUD_LOG_E("[Audio Stream] ERROR: fill sharebuffer buffer ptr == NULL", 0);
        return;
    }
    dst_cnt = hal_audio_buf_mgm_get_free_byte_count(p_info);
    data_cnt = ring_buffer_get_data_byte_count(p_ring);
#ifdef AIR_AUDIO_STREAM_MANAGER_LOG_DEBUG_ENABLE
    AUD_LOG_I("[Audio Stream] stream out, sharebuffer free size = %d, pcm data size = %d", 2, dst_cnt, data_cnt);
#endif
    if ((dst_cnt > 0) && (data_cnt > 0)) { // share buffer has free space and there is still pcm data
        int32_t loop_idx;
        for (loop_idx = 0; loop_idx < 2; loop_idx++) {
            // get sharebuffer free space
            uint32_t stream_out_buf_size  = 0;
            ring_buffer_get_read_information(p_ring, &stream_out_buf_addr, &stream_out_buf_size);
    #if 0
            AUD_LOG_I("[Audio Stream] dsp share buffer free size = %d  glide buffer used = %d", 3, dst_cnt, stream_out_buf_size);
    #endif
            used_size = MINIMUM(stream_out_buf_size, dst_cnt);
            if (used_size > 0) {
                hal_audio_write_stream_out_by_type(handle->message_type, stream_out_buf_addr, used_size);
                // #include "audio_dump.h"
                // LOG_AUDIO_DUMP(stream_out_buf_addr, used_size, PROMPT_VP_OUT);
                dst_cnt -= used_size;
                ring_buffer_read_done(p_ring, used_size);
            }
        }
    } else {
        hal_audio_write_stream_out_by_type(handle->message_type, stream_out_buf_addr, 0); // just ack to avoid dsp risk condition
    }
}

static void audio_stream_manager_avm_config_start(audio_stream_manager_handle_t *handle)
{
    if (!handle) {
        AUD_LOG_E("[Audio Stream] ERROR: start avm handle null!", 0);
        return;
    }
    dsp2mcu_audio_msg_t open_msg = MSG_MCU2DSP_PLAYBACK_OPEN;
    dsp2mcu_audio_msg_t start_msg = MSG_MCU2DSP_PLAYBACK_START;
    if (handle->message_type == AUDIO_MESSAGE_TYPE_PROMPT) {
        open_msg = MSG_MCU2DSP_PROMPT_OPEN;
        start_msg = MSG_MCU2DSP_PROMPT_START;
    } else {
        AUD_LOG_E("[Audio Stream] ERROR: not the specified message!", 0);
        return;
    }
    mcu2dsp_open_param_t open_param;

    memset(&open_param, 0, sizeof(open_param));

    open_param.param.stream_in  = STREAM_IN_VP;
    open_param.param.stream_out = STREAM_OUT_AFE;

    open_param.stream_in_param.playback.bit_type       = HAL_AUDIO_BITS_PER_SAMPLING_16;
    open_param.stream_in_param.playback.sampling_rate  = handle->sampling_rate;  // index
    open_param.stream_in_param.playback.channel_number = handle->channel;
    open_param.stream_in_param.playback.codec_type     = 0;
    open_param.stream_in_param.playback.p_share_info   = (n9_dsp_share_info_t *)hal_audio_query_share_info(handle->message_type);

    hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
    if (handle->message_type == AUDIO_MESSAGE_TYPE_PROMPT) {
        open_param.stream_out_param.afe.memory = HAL_AUDIO_MEM2;
    }
    if (open_param.stream_out_param.afe.audio_device & HAL_AUDIO_DEVICE_DAC_DUAL) {
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_AFE, true);
    }
    open_param.stream_out_param.afe.format                   = AFE_PCM_FORMAT_S16_LE;
    open_param.stream_out_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(handle->sampling_rate);
    open_param.stream_out_param.afe.sampling_rate            = hal_audio_sampling_rate_enum_to_value(handle->sampling_rate);
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
    open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
    open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#endif
#if defined (MTK_FIXED_VP_A2DP_SAMPLING_RATE_TO_48KHZ)
    open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#endif
    open_param.stream_out_param.afe.irq_period   = 5;
    open_param.stream_out_param.afe.frame_size   = 768;
    open_param.stream_out_param.afe.frame_number = 4;
    open_param.stream_out_param.afe.hw_gain      = true;

#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Open i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_MP3_CODEC_PLAY_AVM_OUT, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif

    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PROMPT, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, true);

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
   ami_set_afe_param(STREAM_OUT_2, open_param.stream_out_param.afe.sampling_rate, true);
#endif
    void *p_param_share = hal_audio_dsp_controller_put_paramter(&open_param, sizeof(mcu2dsp_open_param_t), handle->message_type);
    // Notify to do dynamic download. Use async wait.
    hal_audio_dsp_controller_send_message(open_msg, AUDIO_DSP_CODEC_TYPE_PCM, (uint32_t)p_param_share, true);

    // Register callback
    // handle is the second parameter in FUNC: audio_stream_manager_ccni_callback
    hal_audio_service_hook_callback(handle->message_type, audio_stream_manager_ccni_callback, handle);

    // Start playback
    mcu2dsp_start_param_t start_param;
    memset(&start_param, 0, sizeof(start_param));
    // Collect parameters
    start_param.param.stream_in     = STREAM_IN_VP;
    start_param.param.stream_out    = STREAM_OUT_AFE;

    start_param.stream_out_param.afe.aws_flag         = false;

    start_param.stream_out_param.afe.aws_sync_request = handle->aws_sync_request;
    start_param.stream_out_param.afe.aws_sync_time    = handle->aws_sync_time;

    p_param_share = hal_audio_dsp_controller_put_paramter(&start_param, sizeof(mcu2dsp_start_param_t), handle->message_type);
    hal_audio_dsp_controller_send_message(start_msg, 0, (uint32_t)p_param_share, true);
}
static void audio_stream_manager_avm_config_stop(audio_stream_manager_handle_t *handle)
{
    if (!handle) {
        AUD_LOG_E("[Audio Stream] ERROR: stop avm handle null!", 0);
        return;
    }
    dsp2mcu_audio_msg_t stop_msg = MSG_MCU2DSP_PLAYBACK_STOP;
    dsp2mcu_audio_msg_t close_msg = MSG_MCU2DSP_PLAYBACK_CLOSE;
    if (handle->message_type == AUDIO_MESSAGE_TYPE_PROMPT) {
        stop_msg = MSG_MCU2DSP_PROMPT_STOP;
        close_msg = MSG_MCU2DSP_PROMPT_CLOSE;
    } else {
        AUD_LOG_E("[Audio Stream] ERROR: not the specified message!", 0);
        return;
    }
    hal_audio_service_unhook_callback(handle->message_type);
    hal_audio_dsp_controller_send_message(stop_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
    hal_audio_dsp_controller_send_message(close_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Close i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_MP3_CODEC_PLAY_AVM_OUT, 0, 0, 0, false);
#endif
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PROMPT, 0, 0, false);
}

static void audio_stream_manager_ccni_callback(hal_audio_event_t event, void *data)
{
    audio_stream_manager_handle_t *handle     = (audio_stream_manager_handle_t *)data;
    if (!handle) {
        AUD_LOG_E("[Audio Stream] ERROR: ccni callback handle null", 0);
        return;
    }
    switch (event) {
        case HAL_AUDIO_EVENT_UNDERFLOW:
        case HAL_AUDIO_EVENT_DATA_REQUEST:
            if (handle->bit_stream_end == true) {
                break;
            }
            handle->current_event = AUDIO_CODEC_EVENT_PROCESS;
            audio_codec_event_send_from_isr((audio_codec_media_handle_t *)handle, NULL);
            #if defined AIR_MP3_DECODER_ENABLE || defined AIR_OPUS_DECODER_ENABLE
                hal_audio_write_stream_out_by_type(handle->message_type, handle->pcm_ring_buf.buffer_base_pointer, 0); // ack 707
            #endif /* AIR_MP3_DECODER_ENABLE */
            #ifdef AIR_AUDIO_STREAM_MANAGER_LOG_DEBUG_ENABLE
                AUD_LOG_I("[Audio Stream] request data from dsp", 0);
            #endif
            break;
        case HAL_AUDIO_EVENT_END:
            {
                handle->bit_stream_end = true;
                AUD_LOG_I("[Audio Stream] bit_stream_end", 0);
                if (!handle->eof_flag) {
                    AUD_LOG_W("[Audio Stream] Warning: bit_stream_end occurs before eof_flag ", 0);
                }
                if (handle->app_callback) {
                    handle->app_callback(AUDIO_CODEC_CB_EVENT_BITSTEAM_END, NULL);
                }
            }
            break;
        case HAL_AUDIO_EVENT_ERROR:
        case HAL_AUDIO_EVENT_NONE:
            AUD_LOG_E("[Audio Stream] Warning: a strange condition", 0);
            break;
        case HAL_AUDIO_EVENT_DATA_DIRECT: // VP start
            // notify APP user
            AUD_LOG_I("[Audio Stream] stream type %d, trigger", 1, handle->stream_type);
            if (handle->os_timer_handle != NULL) {
                if (xTimerStart(handle->os_timer_handle, 0) != pdPASS) {
                    /* The timer could not be set into the Active state. */
                    AUD_LOG_E("[Audio Stream] ERROR: start os timer fail", 0);
                }
            }
            if (handle->app_callback) {
                handle->app_callback(AUDIO_CODEC_CB_EVENT_START, NULL);
            }
            break;
        default:
            break;
    }
}

audio_stream_manager_status_t audio_stream_manager_stop(audio_stream_manager_id_t id)
{
    int32_t index = id - 1;
    /* -1-  Input parameter check  -----------------------------------------------------------------------------------*/
    if ((index < 0) || (index >= AUDIO_STREAM_MANAGER_USER_MAX)) {
        AUD_LOG_E("[Audio Stream] ERROR: stop id error %d", 1, id);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    if (g_audio_stream_manager_handle[index].is_used != true) {
        AUD_LOG_E("[Audio Stream] ERROR: stop current id is not used", 0);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    audio_stream_manager_handle_t *handle = &(g_audio_stream_manager_handle[index].handle);
    // user stop this stream
    if (handle->bit_stream_end == false) {
        AUD_LOG_I("[Audio Stream] user stop immediately", 0);
        if (handle->message_type == AUDIO_MESSAGE_TYPE_PROMPT) {
            handle->bit_stream_end = true;
            vTaskDelay(50); // prepare total <60ms for HW GAIN ramp down to mute, in order to remove pop noise before turning off DAC.
            // clear share buffer, speed up the process of streaming out non-zero data
            n9_dsp_share_info_t *pInfo = (n9_dsp_share_info_t *)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_PROMPT);
            hal_audio_reset_share_info(pInfo);
            memset((uint32_t *)pInfo->start_addr, 0, pInfo->length);
            // MSG_MCU2DSP_PROMPT_CLEAR_AUDIO_BUFFER is just a special symbol to request vp to clear all audio buffer(src buffer + afe buffer) in dsp side
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PROMPT_CONFIG, MSG_MCU2DSP_PROMPT_CLEAR_AUDIO_BUFFER, 1, false);
            vTaskDelay(10); // prepare 10ms for DMA streaming out all non-zero data before disable DMA, in order to remove pop noise after turning on DAC next time.
            audio_stream_manager_stop(id);
        }
    } else {
        if (handle->message_type == AUDIO_MESSAGE_TYPE_PROMPT) {
            audio_stream_manager_avm_config_stop(handle);
            if(g_audio_dl_suspend_by_user == false) {
                am_audio_dl_resume();
            }
            am_audio_ul_resume();
        }
    }
    handle->current_event = AUDIO_CODEC_EVENT_POST_PROCESS;
    handle->handler((audio_codec_media_handle_t *)handle, NULL);
    AUD_LOG_I("[Audio Stream] STOP id[%d]", 1, id);
    return AUDIO_STREAM_MANAGER_STATUS_OK;
}

audio_stream_manager_status_t audio_stream_manager_close(audio_stream_manager_id_t id)
{
    int32_t index = id - 1;
    /* -1-  Input parameter check  -----------------------------------------------------------------------------------*/
    if ((index < 0) || (index >= AUDIO_STREAM_MANAGER_USER_MAX)) {
        AUD_LOG_E("[Audio Stream] ERROR: close id error %d", 1, id);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    if (g_audio_stream_manager_handle[index].is_used != true) {
        AUD_LOG_E("[Audio Stream] ERROR: close current id is not used", 0);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    AUD_LOG_I("[Audio Stream] CLOSE id[%d]", 1, id);
    audio_stream_manager_handle_t *handle = &(g_audio_stream_manager_handle[index].handle);
    // stop/delete os timer
    if (handle->os_timer_handle != NULL) {
        xTimerStop(handle->os_timer_handle, 0);
        xTimerDelete(handle->os_timer_handle, 0);
        handle->os_timer_handle = NULL;
    }
    handle->current_event = AUDIO_CODEC_EVENT_DEINIT;
    handle->handler((audio_codec_media_handle_t *)handle, NULL);
    audio_codec_event_deregister_callback((audio_codec_media_handle_t *)handle);
    audio_stream_manager_mutex_lock();
    g_audio_stream_manager_handle[index].is_used = false;
    audio_stream_manager_mutex_unlock();
    return AUDIO_STREAM_MANAGER_STATUS_OK;

}
static audio_stream_manager_status_t audio_stream_manager_decoder_handler(audio_codec_media_handle_t *handle, void *data)
{
    (void)(data);
    audio_stream_manager_handle_t *stream_handle = (audio_stream_manager_handle_t *)handle;
    audio_stream_manager_event_t event = AUDIO_CODEC_EVENT_IDLE;
    n9_dsp_share_info_t *p_share_info = NULL;
    uint32_t err_cnt = 0;
    if (!stream_handle) {
        AUD_LOG_E("[Audio Stream] ERROR: decoder handler null!", 0);
        return AUDIO_STREAM_MANAGER_STATUS_ERR;
    }
    event = stream_handle->current_event;
    switch (event) {
        case AUDIO_CODEC_EVENT_IDLE:
            /* decoder input buffer allocate */
            if (audio_stream_manager_cmi_memory_init(stream_handle) != AUDIO_CODEC_MANAGER_SUCCESS) {
                goto HANDLER_ERROR;
            }
            break;
        case AUDIO_CODEC_EVENT_INIT:
            if (audio_stream_manager_cmi_init(stream_handle) != AUDIO_CODEC_MANAGER_SUCCESS) {
                goto HANDLER_ERROR;
            }
            break;
        case AUDIO_CODEC_EVENT_PRE_PROCESS:
        case AUDIO_CODEC_EVENT_PROCESS:
            /* Pre-decode and Pre-fill data into Sharebuffer between DSP and CM4 */
            p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(stream_handle->message_type);
            if (!p_share_info) {
                AUD_LOG_E("[Audio Stream] ERROR: share info ptr == NULL!", 0);
                break;
            }
            // reset sharebuffer information
            if (event == AUDIO_CODEC_EVENT_PRE_PROCESS) {
                hal_audio_reset_share_info(p_share_info);
                p_share_info->bBufferIsFull = 0;
            }
            while (1) {
                if (!stream_handle->decode_end_flag) {
                    if (audio_stream_manager_cmi_process(stream_handle) != AUDIO_CODEC_MANAGER_SUCCESS) {
                        err_cnt++;
                        if (err_cnt > 10) {
                            stream_handle->eof_flag = true;
                            AUD_LOG_E("[Audio Stream] ERROR: process fail so many times %d, abort!", 1, err_cnt);
                        }
                    }
                }
                if (stream_handle->stream_out) {
                    stream_handle->stream_out(handle); // stream out pcm data into sharebuffer
                }
                // ensure the enough data in CM4 Path
                #ifdef AIR_AUDIO_STREAM_MANAGER_LOG_DEBUG_ENABLE
                    AUD_LOG_I("[Audio Stream] sharebuffer_data %d bytes, pcm_ring_buffer data %d bytes", 2, hal_audio_buf_mgm_get_data_byte_count(p_share_info),
                            ring_buffer_get_data_byte_count(&stream_handle->pcm_ring_buf));
                #endif
                // EOF Dectect
                if ((hal_audio_buf_mgm_get_data_byte_count(p_share_info) <= (AUDIO_STREAM_MANAGER_DSP_PROCESS_FRAME_SIZE *
                    (stream_handle->channel + 1))) // DSP Process frame will be (1024*2) when it is stereo.
                    && (ring_buffer_get_data_byte_count(&stream_handle->pcm_ring_buf) == 0)
                    && stream_handle->decode_end_flag) {
                    stream_handle->eof_flag = true;
                    #ifdef AIR_AUDIO_STREAM_MANAGER_LOG_DEBUG_ENABLE
                        AUD_LOG_I("[Audio Stream] set eof flag!", 0);
                    #endif
                }
                if (stream_handle->eof_flag) {
                    if (event == AUDIO_CODEC_EVENT_PROCESS) {
                        // message type
                        if (stream_handle->message_type == AUDIO_MESSAGE_TYPE_PROMPT) {
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PROMPT_CONFIG, AUDIO_PLAYBACK_CONFIG_EOF, 1, false);
                        }
                    }
                    break;
                }
                if (stream_handle->bit_stream_end) { // app layer stops it
                    break;
                }

                if (((hal_audio_buf_mgm_get_free_byte_count(p_share_info) < stream_handle->stream_out_threshold) &&
                    (ring_buffer_get_space_byte_count(&stream_handle->pcm_ring_buf) < stream_handle->stream_out_threshold)) ||
                    /* (hal_audio_buf_mgm_get_free_byte_count(p_share_info) == 0) || */ stream_handle->decode_end_flag) { /* if sharebuffer free space == 0 and
                    sink buffer free space size is not less than threshold */
                    break;
                }
            }
            break;
        case AUDIO_CODEC_EVENT_POST_PROCESS:
            /* do nothing */
            break;
        case AUDIO_CODEC_EVENT_DEINIT:
            if (audio_stream_manager_cmi_deinit(stream_handle) != AUDIO_CODEC_MANAGER_SUCCESS) {
                goto HANDLER_ERROR;
            }
            break;
        default:
            break;
    }
    return AUDIO_STREAM_MANAGER_STATUS_OK;
HANDLER_ERROR:
    return AUDIO_STREAM_MANAGER_STATUS_ERR;
}

static audio_stream_manager_status_t audio_stream_manager_mutex_create(void)
{
    audio_stream_manager_status_t status = AUDIO_STREAM_MANAGER_STATUS_OK;

    if ( g_audio_stream_manager_semaphore_handle == NULL ) {
        //audio_codec_semaphore_handle = xSemaphoreCreateMutex();  /*Old FreeRTOS version work.*/
        vSemaphoreCreateBinary(g_audio_stream_manager_semaphore_handle);      /*In New FreeRTOS version It would assert if Mutex take & give are different task.*/
    }
    else {
        status = AUDIO_STREAM_MANAGER_STATUS_ERR;
    }

    return status;
}

static audio_stream_manager_status_t audio_stream_manager_mutex_lock(void)
{
    audio_stream_manager_status_t status = AUDIO_STREAM_MANAGER_STATUS_OK;

    if ( g_audio_stream_manager_semaphore_handle != NULL ) {
        AUD_LOG_I("[Audio Codec] audio_stream_manager_mutex_lock() +", 0);
        xSemaphoreTake(g_audio_stream_manager_semaphore_handle, portMAX_DELAY);
        AUD_LOG_I("[Audio Codec] audio_stream_manager_mutex_lock() -", 0);
    }
    else {
        status = AUDIO_STREAM_MANAGER_STATUS_ERR;
    }

    return status;
}

static audio_stream_manager_status_t audio_stream_manager_mutex_unlock(void)
{
    audio_stream_manager_status_t status = AUDIO_STREAM_MANAGER_STATUS_OK;

    if ( g_audio_stream_manager_semaphore_handle != NULL ) {
        AUD_LOG_I("[Audio Codec] audio_codec_mutex_unlock()", 0);
        xSemaphoreGive(g_audio_stream_manager_semaphore_handle);
    }
    else {
        status = AUDIO_STREAM_MANAGER_STATUS_ERR;
    }

    return status;
}

static audio_codec_manager_status_t audio_stream_manager_cmi_init(audio_stream_manager_handle_t *stream_handle)
{
    audio_codec_manager_status_t status = AUDIO_CODEC_MANAGER_SUCCESS;
    if (stream_handle == NULL) {
        AUD_LOG_E("[Audio Stream] ERROR: cmi init handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    /* -1-  Codec Manager Paramater Init -----------------------------------------------------------------------------*/
    audio_codec_manager_config_t  codec_manager_config;
    memset(&codec_manager_config, 0, sizeof(audio_codec_manager_config_t));
    codec_manager_config.codec_mode = CODEC_MODE_DERECT;
    /* -2-  Codec Manager Open ---------------------------------------------------------------------------------------*/
    status = audio_codec_open(stream_handle->codec_manager_type, &(codec_manager_config), &(stream_handle->codec_manger_id));
    if (status != AUDIO_CODEC_MANAGER_SUCCESS) {
        AUD_LOG_W("[Audio Stream] Warning: cmi init open codec manager fail, status = %d", 1, status);
        return status;
    }
    stream_handle->codec_manager_config = audio_codec_get_codec_config(stream_handle->codec_manger_id);
    if (!stream_handle->codec_manager_config) {
        AUD_LOG_W("[Audio Stream] Warning: get codec config fail", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    /* -3-  Codec Manager Parse --------------------------------------------------------------------------------------*/
    // 3.1 fill data into codec in buffer
    status = audio_stream_manager_port_before_init(stream_handle);
    if (status != AUDIO_CODEC_MANAGER_SUCCESS) {
        AUD_LOG_W("[Audio Stream] Warning: port init fail", 0);
        return status;
    }
    while (1) {
        uint32_t bs_buffer_size = 0;
        uint32_t dec_buf_wo = 0;
        if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
            audio_stream_manager_fill_decoder_bs(stream_handle, &bs_buffer_size, (uint32_t) -1);
            if (ring_buffer_get_data_byte_count(&(stream_handle->src_buffer))) {
                // [TODO] Notify APP to copy data into stream manager
            }
        }
        dec_buf_wo = stream_handle->dec_in_buf.write_pointer;
        status = audio_codec_parse_config(stream_handle->codec_manger_id, stream_handle->dec_in_buf.buffer_base_pointer,
                    &stream_handle->dec_in_buf.write_pointer, stream_handle->codec_manager_config);
        if ((status == AUDIO_CODEC_MANAGER_ERROR) || (status == AUDIO_CODEC_MANAGER_ERROR_PARAM)) {
            AUD_LOG_W("[Audio Stream] Warning: parse: fail, status = %d", 1, status);
            return status;
        } else if (status == AUDIO_CODEC_MANAGER_SUCCESS) {
            AUD_LOG_I("[Audio Stream] parse: success", 0);
            break;
        }
        if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_LINEAR_BUFFER) {
            AUD_LOG_W("[Audio Stream] Warning: parse: fail", 0); // It means that Codec can't parse anything from the whole file.
            return AUDIO_CODEC_MANAGER_ERROR;
        } else { // drop the useless data
            if (stream_handle->dec_in_buf.write_pointer != 0) {
                memmove(stream_handle->dec_in_buf.buffer_base_pointer,
                            stream_handle->dec_in_buf.buffer_base_pointer + stream_handle->dec_in_buf.write_pointer,
                            dec_buf_wo - stream_handle->dec_in_buf.write_pointer);
            }
            stream_handle->dec_in_buf.write_pointer = dec_buf_wo - stream_handle->dec_in_buf.write_pointer;
        }
    }
    if (stream_handle->codec_manager_config == NULL) {
        AUD_LOG_W("[Audio Stream] Warning: can't get codec_manager_config pointer", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    /* -4-  Special treatment in diferent codec type -----------------------------------------------------------------*/
    status = audio_stream_manager_port_after_init(stream_handle);
    AUD_LOG_I("[Audio Stream] init end, dec_in_buf status base 0x%x ro %d wo %d size %d", 4, stream_handle->dec_in_buf.buffer_base_pointer,
        stream_handle->dec_in_buf.read_pointer, stream_handle->dec_in_buf.write_pointer, stream_handle->dec_in_buf.buffer_byte_count);
    if (status != AUDIO_CODEC_MANAGER_SUCCESS) {
        AUD_LOG_W("[Audio Stream] Warning: port init fail", 0);
        return status;
    }
    return status;
}

static audio_codec_manager_status_t audio_stream_manager_cmi_process(audio_stream_manager_handle_t *stream_handle)
{
    audio_codec_manager_status_t status = AUDIO_CODEC_MANAGER_SUCCESS;
    uint8_t *output_buf = NULL;
    uint32_t output_buf_size = 0;
    if (stream_handle == NULL) {
        AUD_LOG_E("[Audio Stream] ERROR: cmi process handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }

    // 1.special treatment before Decode process
    status = audio_stream_manager_port_before_process(stream_handle);
    if (status != AUDIO_CODEC_MANAGER_SUCCESS) {
        AUD_LOG_W("[Audio Stream] Warning: port process fail", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
#ifdef AIR_AUDIO_STREAM_MANAGER_MIPS_DEBUG
    uint32_t curr_cnt1, curr_cnt2, diff_cnt;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt1);
#endif
    // 2.Decode process
    ring_buffer_get_write_information(&stream_handle->pcm_ring_buf, &output_buf, &output_buf_size);
    if (output_buf == NULL) {
        AUD_LOG_W("[Audio Stream] Warning: get output_buf address fail", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
#ifdef AIR_AUDIO_STREAM_MANAGER_LOG_DEBUG_ENABLE
    AUD_LOG_I("[Audio Stream] [TEST] decode addr[0x%x] wo[%u] ro[%u], out addr[0x%x] size[%u]", 5,
        stream_handle->dec_in_buf.buffer_base_pointer, stream_handle->dec_in_buf.write_pointer, stream_handle->dec_in_buf.read_pointer,
        output_buf, output_buf_size);
#endif
    status = audio_codec_direct_mode_process(stream_handle->codec_manger_id,
                                             stream_handle->dec_in_buf.buffer_base_pointer,
                                             &(stream_handle->dec_in_buf.write_pointer),
                                             output_buf,
                                             &output_buf_size);
    // check status
    if (status == AUDIO_CODEC_MANAGER_STREAM_PROCESS_END) {
        stream_handle->decode_end_flag = true;
        AUD_LOG_I("[Audio Stream] set decode end flag!", 0);
    } else if (status == AUDIO_CODEC_MANAGER_ERROR_LEN) {
        // pass-through
        // AUD_LOG_W("[Audio Stream] process length error!", 0);
    } else if (status < 0) {
        AUD_LOG_W("[Audio Stream] Warning: decode fail, status = %d", 1, status);
        return AUDIO_CODEC_MANAGER_ERROR;
    } else {
        ring_buffer_write_done(&stream_handle->pcm_ring_buf, output_buf_size); // update wo of ringbuffer
    }

    // 3.special treatment after Decode process
    status = audio_stream_manager_port_after_process(stream_handle);
    if (status != AUDIO_CODEC_MANAGER_SUCCESS) {
        AUD_LOG_W("[Audio Stream] Warning: port after process fail", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
#ifdef AIR_AUDIO_STREAM_MANAGER_MIPS_DEBUG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt2);
    hal_gpt_get_duration_count(curr_cnt1, curr_cnt2, &diff_cnt);
    AUD_LOG_I("[Audio Stream] Decode MIPS [%u]us", 1, diff_cnt);
#endif
    return AUDIO_CODEC_MANAGER_SUCCESS;
}

static audio_codec_manager_status_t audio_stream_manager_cmi_deinit(audio_stream_manager_handle_t *stream_handle)
{
    audio_codec_manager_status_t status = AUDIO_CODEC_MANAGER_SUCCESS;
    if (stream_handle == NULL) {
        AUD_LOG_E("[Audio Stream] ERROR: cmi deinit handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    status = audio_codec_close(stream_handle->codec_manger_id);
    if (status != AUDIO_CODEC_MANAGER_SUCCESS) {
        AUD_LOG_W("[Audio Stream] Warning: deinit fail, status = %d", 1, status);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    return AUDIO_CODEC_MANAGER_SUCCESS;
}

// Timer drivern model for Audio codec task
static void audio_stream_manager_gpt_timer_callback(TimerHandle_t xTimer)
{
    audio_stream_manager_handle_t *stream_handle = NULL;
    uint32_t i = 0;
    for (i = 0; i < AUDIO_STREAM_MANAGER_USER_MAX; i++) {
        if (g_audio_stream_manager_handle[i].is_used == true) {
            if (g_audio_stream_manager_handle[i].handle.os_timer_handle == xTimer) {
                break;
            }
        }
    }
    if (i == AUDIO_STREAM_MANAGER_USER_MAX) {
        return;
    }
    #ifdef AIR_AUDIO_STREAM_MANAGER_LOG_DEBUG_ENABLE
        AUD_LOG_I("[Audio Stream] os timer callback id[%d]", 1, i);
    #endif
    stream_handle = &(g_audio_stream_manager_handle[i].handle);
    if (!stream_handle) {
        AUD_LOG_E("[Audio Stream] ERROR: timer callback handle null, %d", 1, i);
        return;
    }
    if ((stream_handle->eof_flag) || (stream_handle->bit_stream_end)) {
        // stop timer
        if (g_audio_stream_manager_handle[i].handle.os_timer_handle != NULL) {
            xTimerStop(xTimer, 0);
            xTimerDelete(xTimer, 0);
            g_audio_stream_manager_handle[i].handle.os_timer_handle = NULL;
        }
    } else {
        stream_handle->current_event = AUDIO_CODEC_EVENT_PROCESS;
        audio_codec_event_send_from_isr((audio_codec_media_handle_t *)stream_handle, NULL);
    }
}

audio_codec_manager_status_t audio_stream_manager_cmi_memory_init(audio_stream_manager_handle_t *stream_handle)
{
    /* 1. input paramter check */
    uint8_t  *memory_pool_addr    = NULL;
    uint32_t memory_pool_size     = 0;
    uint32_t bs_buffer_size       = 0;
    uint32_t pcm_ring_buffer_size = 0;
    if (stream_handle == NULL) {
        AUD_LOG_E("[Audio Stream] ERROR: cmi memory init handle null", 0);
        return AUDIO_CODEC_MANAGER_ERROR;
    }
    switch (stream_handle->codec_manager_type) {
        case CODEC_TYPE_NONE: // PCM
            // memory_pool_addr     = g_stream_manager_mp3_memory_pool;
            // memory_pool_size     = AIR_STREAM_MANAGER_MP3_MEMORY_POOL_SIZE_MAX;
            // bs_buffer_size       = AIR_STREAM_MANAGER_MP3_BS_BUFFER_SIZE;
            // pcm_ring_buffer_size = AIR_STREAM_MANAGER_MP3_PCM_RING_BUFFER_SIZE;
            break;
        #ifdef MTK_WAV_DECODER_ENABLE
            case CODEC_TYPE_WAV_DECODE:
                memory_pool_addr     = g_stream_manager_wav_memory_pool;
                memory_pool_size     = AIR_STREAM_MANAGER_WAV_MEMORY_POOL_SIZE_MAX;
                bs_buffer_size       = AIR_STREAM_MANAGER_WAV_BS_BUFFER_SIZE;
                pcm_ring_buffer_size = AIR_STREAM_MANAGER_WAV_PCM_RING_BUFFER_SIZE;
                break;
        #endif
        #ifdef AIR_OPUS_DECODER_ENABLE
            case CODEC_TYPE_OPUS_DECODE:
                memory_pool_addr     = g_stream_manager_opus_memory_pool;
                memory_pool_size     = AIR_STREAM_MANAGER_OPUS_MEMORY_POOL_SIZE_MAX;
                bs_buffer_size       = AIR_STREAM_MANAGER_OPUS_BS_BUFFER_SIZE;
                pcm_ring_buffer_size = AIR_STREAM_MANAGER_OPUS_PCM_RING_BUFFER_SIZE;
                break;
        #endif
        #ifdef AIR_MP3_DECODER_ENABLE
            case CODEC_TYPE_MP3_DECODE:
                memory_pool_addr     = g_stream_manager_mp3_memory_pool;
                memory_pool_size     = AIR_STREAM_MANAGER_MP3_MEMORY_POOL_SIZE_MAX;
                bs_buffer_size       = AIR_STREAM_MANAGER_MP3_BS_BUFFER_SIZE;
                #ifdef MTK_MP3_STEREO_SUPPORT
                    pcm_ring_buffer_size = AIR_STREAM_MANAGER_MP3_PCM_RING_BUFFER_SIZE*2;
                #else
                    pcm_ring_buffer_size = AIR_STREAM_MANAGER_MP3_PCM_RING_BUFFER_SIZE;
                #endif
                break;
        #endif
        default:
            AUD_LOG_E("[Audio Stream] ERROR: set memory, codec type error %d", 1, stream_handle->codec_manager_type);
            configASSERT(0);
            return AUDIO_CODEC_MANAGER_ERROR;
            break;
    }
    /* 2. clear buffer */
    memset(memory_pool_addr, 0, memory_pool_size);
    AUD_LOG_I("[Audio Stream] set memory success, addr[0x%x] size[%u]", 2, memory_pool_addr, memory_pool_size);
    /* 3. allocate the pcm ring buffer */
    if (stream_handle->buffer_type == AUDIO_STREAM_MANAGER_RING_BUFFER) {
        // remap the address of dec_in_buf (bs_buf)
        stream_handle->dec_in_buf.buffer_base_pointer = memory_pool_addr;
        stream_handle->dec_in_buf.buffer_byte_count = bs_buffer_size;
        stream_handle->dec_in_buf.read_pointer = 0;
        stream_handle->dec_in_buf.write_pointer = 0;
        memory_pool_addr += bs_buffer_size;
        memory_pool_size -= bs_buffer_size;
    }

    stream_handle->pcm_ring_buf.buffer_base_pointer = memory_pool_addr;
    stream_handle->pcm_ring_buf.buffer_byte_count = pcm_ring_buffer_size;
    stream_handle->pcm_ring_buf.read_pointer = 0;
    stream_handle->pcm_ring_buf.write_pointer = 0;
    memory_pool_addr += pcm_ring_buffer_size;
    memory_pool_size -= pcm_ring_buffer_size;
    return AUDIO_CODEC_MANAGER_SUCCESS;
}