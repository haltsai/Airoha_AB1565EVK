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

#ifndef __AUDIO_STREAM_MANAGER_H__
#define __AUDIO_STREAM_MANAGER_H__

#include "audio_codec.h"
#include "audio_mixer_internal.h"
#include "hal_audio_internal.h"
#include "hal_platform.h"
#include "FreeRTOS.h"
#ifdef MTK_AUDIO_CODEC_MANAGER_ENABLE
    #include "audio_codec_manager.h"
#endif
#include "timers.h"

#ifdef AIR_AUDIO_STREAM_MANAGER_MIXER_ENABLE
    #define AIR_AUDIO_STREAM_MANAGER_MIXER_MEMORY_POOL_SIZE   (1000)
#endif
#ifdef AIR_MP3_DECODER_ENABLE
    #define AIR_STREAM_MANAGER_MP3_PCM_RING_BUFFER_SIZE (4608)
    #ifndef AIR_PROMPT_SOUND_LINERBUFFER_ENABLE
        #define AIR_STREAM_MANAGER_MP3_BS_BUFFER_SIZE (4096)
    #else
        #define AIR_STREAM_MANAGER_MP3_BS_BUFFER_SIZE (0)
    #endif
    #ifdef MTK_MP3_STEREO_SUPPORT
        #define AIR_STREAM_MANAGER_MP3_MEMORY_POOL_SIZE_MAX   (AIR_STREAM_MANAGER_MP3_BS_BUFFER_SIZE + (AIR_STREAM_MANAGER_MP3_PCM_RING_BUFFER_SIZE*2) + (16268 + 1972)) // 16268: stereo MP3 codec working buffer1 size
                                                                                                                                                                         // 1972: stereo MP3 codec working buffer2 size
    #else
        #define AIR_STREAM_MANAGER_MP3_MEMORY_POOL_SIZE_MAX   (AIR_STREAM_MANAGER_MP3_BS_BUFFER_SIZE + AIR_STREAM_MANAGER_MP3_PCM_RING_BUFFER_SIZE + (11300)) // 11300: mono MP3 codec internal buffer size
    #endif
#endif

#ifdef MTK_WAV_DECODER_ENABLE
    #define AIR_STREAM_MANAGER_WAV_PCM_RING_BUFFER_SIZE (4096)
    #ifndef AIR_PROMPT_SOUND_LINERBUFFER_ENABLE
        #define AIR_STREAM_MANAGER_WAV_BS_BUFFER_SIZE (1024)
    #else
        #define AIR_STREAM_MANAGER_WAV_BS_BUFFER_SIZE (0)
    #endif
    #define AIR_STREAM_MANAGER_WAV_MEMORY_POOL_SIZE_MAX   (AIR_STREAM_MANAGER_WAV_BS_BUFFER_SIZE + AIR_STREAM_MANAGER_WAV_PCM_RING_BUFFER_SIZE + (192))
#endif

#ifdef AIR_OPUS_DECODER_ENABLE
    #define AIR_OPUS_FrameeSize		(960)
    #define AIR_OPUS_BufInSize		(256)
    #define AIR_STREAM_MANAGER_OPUS_PCM_RING_BUFFER_SIZE (AIR_OPUS_FrameeSize*(4))
    #define AIR_STREAM_MANAGER_OPUS_BS_BUFFER_SIZE (AIR_OPUS_BufInSize)
    #define AIR_STREAM_MANAGER_OPUS_MEMORY_POOL_SIZE_MAX   (AIR_STREAM_MANAGER_OPUS_BS_BUFFER_SIZE + AIR_STREAM_MANAGER_OPUS_PCM_RING_BUFFER_SIZE + (15952 + 17660))
#endif
/**
 * @addtogroup Audio
 * @{
 * @addtogroup stream_manager
 * @{
 *
 * The stream_manager is used to create multi audio data streams of different codec types in Cortex-M Core.
 *
 * @section stream_manager_api_usage How to use this module
 *
 * - An example on how to use the stream_manager APIs.
 *  - 1.  Create a new stream:
 *        Use #audio_stream_manager_open() to create.
 *  - 2.  Start the stream process flow:
 *        Use #audio_stream_manager_start() to start.
 *  - 3.  Stop the stream process flow:
 *        Use #audio_stream_manager_stop() to stop.
 *  - 4.  Delete the stream:
 *        Use #audio_stream_manager_close() to close.
 *    - Sample code:
 *     @code
        // 1. create a new stream for vp
        int32_t g_vp_codec_id = -1;
        audio_stream_manager_config_t user_config;
        memset(&user_config, 0, sizeof(audio_stream_manager_config_t));
        user_config.input_buffer = tone_buf;
        user_config.input_buffer_size = tone_size;
        user_config.stream_type = AUDIO_STREAM_MANAGER_TYPE_VP;
        user_config.codec_type = CODEC_TYPE_WAV_DECODE;
        user_config.user_data = NULL;
        if (audio_stream_manager_open((audio_stream_manager_id_t *)&g_vp_codec_id, &user_config, (audio_stream_manager_callback_t *)prompt_control_codec_callback) !=
            AUDIO_STREAM_MANAGER_STATUS_OK) {
            g_vp_codec_id = -1;
            return;
        }

        // 2. is there need set aws sync time
        uint32_t curr_cnt = 0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
        audio_stream_manager_set_aws(g_vp_codec_id, true, curr_cnt + 100000);  // 100000 means 100ms

        // 3. start the stream
        audio_stream_manager_start(g_vp_codec_id);

        // 4. stop the stream
        audio_stream_manager_stop(g_vp_codec_id);

        // 5. close the stream
        audio_stream_manager_close(g_vp_codec_id);

        // 6. reset the id
        g_vp_codec_id = -1;
 *     @endcode
 */

/** @defgroup stream_manager_enum Enum
  * @{
  */

/** @brief Stream Manager Buffer Type. */
typedef enum {
    AUDIO_STREAM_MANAGER_RING_BUFFER = 0,
    AUDIO_STREAM_MANAGER_LINEAR_BUFFER = 1,
} audio_stream_manager_buffer_type_t;

/** @brief Stream Manager Function Return Status. */
typedef enum {
    AUDIO_STREAM_MANAGER_STATUS_ERR = -1,
    AUDIO_STREAM_MANAGER_STATUS_OK  = 0,
} audio_stream_manager_status_t;

/** @brief Stream Manager Stream Type. */
typedef enum {
    AUDIO_STREAM_MANAGER_TYPE_MIN     = 0,
    AUDIO_STREAM_MANAGER_TYPE_VP      = AUDIO_STREAM_MANAGER_TYPE_MIN,
    AUDIO_STREAM_MANAGER_TYPE_MAX     ,
    AUDIO_STREAM_MANAGER_TYPE_FULL    = 0xFFFFFFFF,
} audio_stream_manager_type_t;

/** @brief Stream Manager EVENT Type. */
typedef enum {
    AUDIO_CODEC_EVENT_IDLE = 0,
    AUDIO_CODEC_EVENT_INIT = 1,
    AUDIO_CODEC_EVENT_PRE_PROCESS,
    AUDIO_CODEC_EVENT_PROCESS,
    AUDIO_CODEC_EVENT_POST_PROCESS,
    AUDIO_CODEC_EVENT_DEINIT,
} audio_stream_manager_event_t;

/** @brief Stream Manager Notification To Caller EVENT Type. */
typedef enum {
    AUDIO_CODEC_CB_EVENT_OK = 0,
    AUDIO_CODEC_CB_EVENT_BITSTEAM_END,
    AUDIO_CODEC_CB_EVENT_START,
    AUDIO_CODEC_CB_EVENT_STOP,
} audio_stream_manager_callback_event_t;

/** @brief Stream Manager Config Parameters For Caller. */
typedef struct {
    uint8_t *input_buffer;
    uint32_t input_buffer_size;
    audio_stream_manager_type_t stream_type;
    audio_codec_manager_type_t codec_type;
    audio_stream_manager_buffer_type_t buffer_type; // 1:Source is XIP storage  0:NON-XIP storage
#ifdef AIR_AUDIO_MIXER_SUPPORT_ENABLE
    audio_mixer_role_t           mixer_track_role;
#endif
    void *user_data;
} audio_stream_manager_config_t;

/** @brief Stream Manager Stream ID. */
typedef int32_t audio_stream_manager_id_t;

/** @brief Stream Manager Callback Type For Caller. */
typedef void (*audio_stream_manager_callback_t)(audio_stream_manager_callback_event_t event_id, void *user_data);

/** @brief Stream Manager Handle. */
typedef struct {
    audio_stream_manager_type_t           stream_type;                        // caller sets it, the type of this stream: VP or others.
    audio_stream_manager_event_t          excute_event;                       // used for "handler"
    audio_stream_manager_event_t          current_event;                      // used for "handler"
    ring_buffer_information_t             src_buffer;                         // caller's buffer
    audio_stream_manager_buffer_type_t    buffer_type;                        // caller sets it
                                                                              // 0: liner buffer(XIP storage); 1: ring buffer (Non-XIP storage)
    audio_codec_manager_type_t          codec_manager_type;
    audio_codec_manager_config_t        *codec_manager_config;                // codec handle: wav_codec_handle/mp3_codec_handle/opus_codec_handle
    audio_codec_manager_handle_t        codec_manger_id;
    uint32_t                            stream_out_threshold;
    bool                                eof_flag;                             // the end of audio file
    bool                                decode_end_flag;                      // the end of decoder process
    bool                                bit_stream_end;                       // the end of the whole audio stream

    audio_message_type_t                message_type;                         // caller sets it
    hal_audio_sampling_rate_t           sampling_rate;                        // DSP configuration parameter
    hal_audio_channel_number_t          channel;                              // DSP configuration parameter
    // for Audio sync play
    bool                                aws_sync_request;                     // caller sets it
    uint32_t                            aws_sync_time;                        // caller sets it, the target gpt count to play data

    void (*app_callback)(audio_stream_manager_callback_event_t event_id, void *user_data);
    void (*stream_out)(audio_codec_media_handle_t *handle);
    audio_stream_manager_status_t (*handler)(audio_codec_media_handle_t *handle, void *data);          // event-drivern model
    uint32_t                            os_timer_period;                      // ms
    TimerHandle_t                       os_timer_handle;

    // buffer control
    ring_buffer_information_t           dec_in_buf;
    ring_buffer_information_t           pcm_ring_buf;

    // MIXer
#ifdef AIR_AUDIO_MIXER_SUPPORT_ENABLE
    audio_mixer_track_id_t              mixer_track_id;
    audio_mixer_role_t                  mixer_track_role;
#endif
} audio_stream_manager_handle_t;

/**
  * @}
  */

/**
 * @brief     This function is used to create a new audio stream.
 * @param[in] id is an integer to indicate the audio stream identification number.
 * @param[in] user_config is the input parameters need by stream manager.
 * @param[in] callback is the callback to notify application that the process status of stream manager.
 * @return    Return #AUDIO_STREAM_MANAGER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 */
audio_stream_manager_status_t audio_stream_manager_open(audio_stream_manager_id_t *id, audio_stream_manager_config_t *user_config, audio_stream_manager_callback_t *callback);

/**
 * @brief     This function is used to delete the audio stream.
 * @param[in] id is an integer to indicate the audio stream identification number.
 * @return    Return #AUDIO_STREAM_MANAGER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 */
audio_stream_manager_status_t audio_stream_manager_close(audio_stream_manager_id_t id);

/**
 * @brief     This function is used to start the audio stream.
 * @param[in] id is an integer to indicate the audio stream identification number.
 * @return    Return #AUDIO_STREAM_MANAGER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 */
audio_stream_manager_status_t audio_stream_manager_start(audio_stream_manager_id_t id);

/**
 * @brief     This function is used to stop the audio stream.
 * @param[in] id is an integer to indicate the audio stream identification number.
 * @return    Return #AUDIO_STREAM_MANAGER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 */
audio_stream_manager_status_t audio_stream_manager_stop(audio_stream_manager_id_t id);

/**
 * @brief     This function is used to pause the audio stream playing.
 * @param[in] id is an integer to indicate the audio stream identification number.
 * @return    Return #AUDIO_STREAM_MANAGER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 */
audio_stream_manager_status_t audio_stream_manager_pause(audio_stream_manager_id_t id);

/**
 * @brief     This function is used to continue the audio stream playing.
 * @param[in] id is an integer to indicate the audio stream identification number.
 * @return    Return #AUDIO_STREAM_MANAGER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 */
audio_stream_manager_status_t audio_stream_manager_continue(audio_stream_manager_id_t id);

/**
 * @brief     This function is used to set the delay time of stream playing.
 * @param[in] id is an integer to indicate the audio stream identification number.
 * @param[in] aws_sync_request enable or disable the delay time.
 * @param[in] aws_sync_time is a gpt count of gpt timer(1M CLK source). It is an absolute time!
 * @return    Return #AUDIO_STREAM_MANAGER_STATUS_OK if the operation is successful. Otherwise, error occurs.
 */
void audio_stream_manager_set_aws(audio_stream_manager_id_t id, bool aws_sync_request, uint32_t aws_sync_time);

/**
* @}
* @}
*/
#endif /* __AUDIO_STREAM_MANAGER_H__ */