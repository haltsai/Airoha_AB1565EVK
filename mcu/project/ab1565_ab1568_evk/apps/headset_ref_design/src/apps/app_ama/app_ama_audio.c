/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#include "record_control.h"
#include "bt_sink_srv_ami.h"
#include "app_ama_audio.h"
#include "FreeRTOS.h"
#include "memory_attribute.h"
// Modify for 2822
//#include "hal_dvfs_internal.h"
#include "hal_platform.h"
#include "audio_codec_manager.h"
#include "audio_codec_opus_encoder.h"
#include "BtAma.h"
#include "app_ama_activity.h"
#include "timers.h"

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
#include "audio_source_control.h"
#include "audio_src_srv_resource_manager_config.h"
#include "audio_src_srv.h"
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

// #include "audio_dump.h"

#ifdef MTK_AMA_ENABLE


/**
 * @brief When received the PCM data from DSP
 *
 */
#define AMA_TASK_EVENT_AUDIO_STREAM_IN                  1

/**
 * @brief Start to run audio recorder and opus encoding
 *
 */
#define AMA_TASK_EVENT_AUDIO_START                      2
/**
 * @brief When start audio recorder done
 *
 */
#define AMA_TASK_EVENT_AUDIO_START_DONE                 3

/**
 * @brief Stop the audio recorder and opus encoding
 *
 */
#define AMA_TASK_EVENT_AUDIO_STOP                       4

/**
 * @brief When audio recorder execute action done
 *
 */
#define AMA_TASK_EVENT_AUDIO_ACTION_DONE                5
/**
 * @brief Send the audio data to SP
 *
 */
#define AMA_TASK_EVENT_AUDIO_SEND_DATA                  6

/**
 * @brief Restart the audio recorder and opus encoding
 *
 */
#define AMA_TASK_EVENT_AUDIO_RESTART                    7

/**
 * @brief When the Wake Word Detected.
 *
 */
#define AMA_TASK_EVENT_AUDIO_WWD_NOTIFICATION           8

/**
 * @brief When audio recorder need to clear all data
 *
 */
#define AMA_TASK_EVENT_AUDIO_DATA_ABORT_NOTIFICATION    9

/**
 * @brief Audio recorder stopped by other application (eg : HFP)
 *
 */
#define AMA_TASK_EVENT_AUDIO_STOP_BY_IND                10

/**
 * @brief When side tone disable finished
 */
#define AMA_TASK_EVENT_SIDE_TONE_STOPPED                11

/**
 * @brief When side tone enable succeed
 */
#define AMA_TASK_EVENT_SIDE_TONE_STARTED                12

/**
 * @brief Audio source compete finished
 */
#define AMA_TASK_EVENT_SOURCE_CONTROL_RESULT            13

/**
 * @brief When recorder has been released
 */
#define AMA_TASK_EVENT_AUDIO_RELEASED                   14

/**
 * @brief The opus codec buffer size for the library
 *
 */
#define AMA_CODEC_BUF_LEN                               15184

/**
 * @brief The audio queue depth
 *
 */
#define AMA_TASK_QUEUE_SIZE                             40
#define AMA_TASK_QUEUE_ITEM_SIZE                        sizeof(ama_msg_item_t)


#define AMA_AM_STATE_IDLE                               0
#define AMA_AM_STATE_STARTING                           1
#define AMA_AM_STATE_STOPING                            2
#define AMA_AM_STATE_STARTED                            3
#define AMA_AM_INVALID_ID                               0

/**
 * @brief The opus encode manager ring buffer cout, the ring buffer size should be 50 * opus output length for one frame.
 *
 */
#define AMA_OPUS_CODEC_RING_BUFFER_COUNT              20

/**
 * @brief The audio queue message item.
 *
 */
typedef struct {
    uint8_t event_id;
    uint8_t *data;
} ama_msg_item_t;

/**
 * @brief The audio recorder / opus information struct.
 *
 */
typedef struct {
    bool                            info_sending_data;
    bool                            info_need_force_stop_recorder;
    bool                            info_need_force_restart_recorder;

    uint8_t                         info_compress_ratio;
    uint8_t                         info_state;
    record_id_t                     info_recorder_id;

    uint32_t                        info_pcm_length_in_one_frame;
    uint32_t                        info_encode_output_len;
    uint32_t                        info_codec_ring_buffer_len;
    audio_codec_manager_handle_t    info_codec_manager_handle;

    on_recorder_stop_record         info_recorder_stopped_callback;
    on_recorder_released            info_recorder_released_callback;

    uint8_t                         *info_codec_ring_buffer;
    uint8_t                         *info_pcm_read_buffer;

    uint8_t                         info_codec_buffer[AMA_CODEC_BUF_LEN];
    wwe_mode_t                      info_restart_mode;
    wwe_mode_t                      info_current_mode;
    am_vendor_se_id_t               info_am_se_id;
    bool                            info_am_stop_by_ld; /* For AMA recording stopped by LD */
    bool                            info_is_suspended;
    bool                            info_is_restart;

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
    void                            *info_audio_source_control_handle;
    bool                            info_audio_source_requested;
    bool                            info_audio_source_released;
    bool                            info_audio_source_suspend;
    bool                            info_audio_source_transmitter_started;
    bool                            info_audio_source_transmitter_stopped;
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

#ifdef AIR_AMA_SIDETONE_ENABLE
    bool                            info_side_tone_enabled;
    bool                            info_side_tone_stopped;
    bool                            info_side_tone_started;
#endif /* AIR_AMA_SIDETONE_ENABLE */

    bool                            info_dvfs_locked;
    bool                            info_is_data_notified;
    bool                            info_is_timer_start_ok; /** Check reading audio data timer start succeed or not */

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    uint16_t                        info_wwd_preroll_recommend_len;
    uint16_t                        info_wwd_preroll_padding_data_len;

    uint32_t                        info_wwd_dummy_read_length;
    uint32_t                        info_wwd_dummy_read_index;

    uint32_t                        info_wwd_flash_address;
    uint32_t                        info_wwd_length;

    on_wwd_trigger                  info_wwd_start_speech_callback;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
} ama_audio_context_t;

ama_audio_context_t ama_audio_context;

TimerHandle_t reading_timer_handle;

static void         ama_audio_control_ccni_callback(hal_audio_event_t event, void *data);
static void         ama_audio_codec_manager_callback(audio_codec_manager_handle_t handle, audio_codec_manager_event_t event, void *user_data);
static void         ama_audio_am_callback(bt_sink_srv_am_id_t aud_id,
                                            bt_sink_srv_am_cb_msg_class_t msg_id,
                                            bt_sink_srv_am_cb_sub_msg_t sub_msg,
                                            void *parm);
static int8_t       ama_audio_queue_send(QueueHandle_t q_id, void *data);
//static uint16_t     ama_audio_get_item_num(QueueHandle_t q_id);
static bool         ama_audio_recorder_stop(void);

static bool         ama_audio_recorder_start(wwe_mode_t mode);

static QueueHandle_t ama_audio_task_queue = NULL;

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
static void app_ama_audio_start_request_control(bool request);
static void app_ama_audio_execute_transmitter(bool start);
static void app_ama_audio_handle_control_request_result(uint16_t result);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

/**
 * @brief Audio data reading timer callback handler
 *
 * @param xTimer
 */
static void ama_audio_timer_callback_handler(TimerHandle_t xTimer)
{
    if ((xTimer == reading_timer_handle) && (ama_audio_context.info_sending_data == true)) {
        ama_msg_item_t event;
        event.data = NULL;
        event.event_id = AMA_TASK_EVENT_AUDIO_STREAM_IN;
        ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &event);
    }
}

/**
 * @brief Audio data reading timer start
 * Which use to reduce the power consumption.
 */
static void ama_audio_timer_start()
{
    if ((reading_timer_handle != NULL) && (ama_audio_context.info_is_timer_start_ok == false)) {
        if (xTimerStart(reading_timer_handle, 0) == pdPASS) {
            ama_audio_context.info_is_timer_start_ok = true;
        } else {
            ama_audio_context.info_is_timer_start_ok = false;
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [ama_audio_timer_start] Start timer failed", 0);
        }
    }
}

/**
 * @brief If the flow execute finished, stop timer.
 */
static void ama_audio_timer_stop()
{
    if ((reading_timer_handle != NULL) && (ama_audio_context.info_is_timer_start_ok == true)) {
        ama_audio_context.info_is_timer_start_ok = false;
        if (xTimerStop(reading_timer_handle, 0) != pdPASS) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [ama_audio_timer_stop] Stop timer failed", 0);
        }
    }
}

static void ama_audio_queue_reset(QueueHandle_t q_id)
{
    xQueueReset(q_id);
}

static int8_t ama_audio_queue_send(QueueHandle_t q_id, void *data)
{
    BaseType_t ret = -1;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (0 == q_id) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_queue_send FAILED (q_id is 0)", 0);
        return -1;
    }

    if (0 == HAL_NVIC_QUERY_EXCEPTION_NUMBER) {
        ret = xQueueSend(q_id, data, 0);
    } else {
        ret = xQueueSendFromISR(q_id, data, &xHigherPriorityTaskWoken);

        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    if (pdFAIL != ret) {
        return 0;
    } else {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_queue_send FAILED (queue full): false %d, ret:%d", 2, pdFAIL, ret);
        return -1;
    }
}

int8_t ama_audio_queue_send_front(QueueHandle_t q_id, void *data)
{
    BaseType_t ret = -1;
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    if (q_id == 0) {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", ama_audio_queue_send_front FAILED (q_id is 0)", 0);
        return -1;
    }

    if (0 == HAL_NVIC_QUERY_EXCEPTION_NUMBER) {
        ret = xQueueSendToFront(q_id, data, 0);
    } else {
        ret = xQueueSendToFrontFromISR(q_id, data, &xHigherPriorityTaskWoken);

        if (xHigherPriorityTaskWoken) {
            portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
        }
    }

    if (pdFAIL != ret) {
        return 0;
    } else {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", ama_audio_queue_send_front FAILED (queue full): false %d, ret:%d", 2, pdFAIL, ret);
        return -1;
    }
}

static void ama_send_audio_data(void)
{
    uint8_t *voice_data = NULL;
    uint8_t frameSize = 160;
    AMAStatus status;

    if (AMA_AM_STATE_STARTED != ama_audio_context.info_state || ama_audio_context.info_sending_data == false) {
        /**
         * @brief If current is stopped, return
         *
         */
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data; send failed, state error , state : %d, sending data : %d",
                                2, ama_audio_context.info_state, ama_audio_context.info_sending_data);
        return;
    }

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    if (ama_audio_context.info_wwd_preroll_padding_data_len > 0) {
        /**
         * @brief If the preroll padding data is not NULL, need to send the padding data of preroll firstly.
         *
         */
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data; Start to send preroll padding data : %d", 1, ama_audio_context.info_wwd_preroll_padding_data_len);

        while (ama_audio_context.info_wwd_preroll_padding_data_len > 0) {
            voice_data = AMA_Target_VoiceStreamSinkClaim(frameSize);
            if (voice_data != NULL) {
                if (ama_audio_context.info_wwd_preroll_padding_data_len > frameSize) {
                    memset(voice_data, 0, sizeof(uint8_t) * frameSize);
                    ama_audio_context.info_wwd_preroll_padding_data_len -= frameSize;
                } else {
                    memset(voice_data, 0, sizeof(uint8_t) * ama_audio_context.info_wwd_preroll_padding_data_len);
                    ama_audio_context.info_wwd_preroll_padding_data_len = 0;
                }
                status = AMA_Target_VoiceStreamSinkFlush();

                if (status != AMA_STATUS_OK) {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data; Send padding data, Packet send failed", 1, status);
                }
            } else {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data; Send padding data, Claim voice data buffer failed", 0);
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                app_ama_audio_start_request_control(false);
                app_ama_audio_execute_transmitter(false);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                ama_audio_recorder_stop();
                break;
            }
        }
    } else {
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
        /**
         * @brief Read the opus encoder length which stored the encoded data.
         *
         */
        //uint16_t current_opus_length = audio_opus_encoder_get_payload_length();
        uint32_t current_opus_length = audio_codec_buffer_mode_get_output_data_length(ama_audio_context.info_codec_manager_handle);

        uint32_t count = current_opus_length / frameSize;
        uint32_t index = 0;

        if (count == 0) {
            return;
        }

        for (index = 0; index < count; index ++) {
            voice_data = AMA_Target_VoiceStreamSinkClaim(frameSize);
            if (voice_data == NULL) {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data; Send voice data, Claim voice data buffer failed", 0);
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                app_ama_audio_start_request_control(false);
                app_ama_audio_execute_transmitter(false);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                ama_audio_recorder_stop();
                return;
            }
            uint8_t *ring_buf_address = NULL;
            uint32_t ring_buf_length = 0;
            uint16_t read_length = 0;

            while (ring_buf_length < frameSize) {
                audio_codec_manager_status_t st = audio_codec_buffer_mode_get_read_information(ama_audio_context.info_codec_manager_handle,
                                                                                                &ring_buf_address,
                                                                                                &ring_buf_length);
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data, get read information length : %d", 1, ring_buf_length);

                if (st != AUDIO_CODEC_MANAGER_SUCCESS) {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data, read failed from codec, %d", 1, st);
                    return;
                }

                if (read_length == 0 && ring_buf_length >= frameSize) {
                    /**
                     * @brief If the read data length bigger than the expected data length
                     * Only copy the data of the frame size
                     */
                    memcpy(voice_data, ring_buf_address, frameSize);
                    audio_codec_buffer_mode_read_done(ama_audio_context.info_codec_manager_handle, frameSize);
                    read_length += frameSize;
                    // LOG_AUDIO_DUMP(ring_buf_address, frameSize, VOICE_TX_MIC_3);
                    break;
                } else {
                    /**
                     * @brief If the read length smaller than the expected data length, and read more than 1 times.
                     *
                     */
                    if (ring_buf_length >= (frameSize - read_length)) {
                        memcpy(voice_data + read_length, ring_buf_address, frameSize - read_length);
                        audio_codec_buffer_mode_read_done(ama_audio_context.info_codec_manager_handle, frameSize - read_length);
                        read_length += (frameSize - read_length);
                        // LOG_AUDIO_DUMP(ring_buf_address, (frameSize - read_length), VOICE_TX_MIC_3);
                        break;
                    } else {
                        memcpy(voice_data + read_length, ring_buf_address, ring_buf_length);
                        audio_codec_buffer_mode_read_done(ama_audio_context.info_codec_manager_handle, ring_buf_length);
                        // LOG_AUDIO_DUMP(ring_buf_address, ring_buf_length, VOICE_TX_MIC_3);
                    }

                    read_length += ring_buf_length;
                }
            }

            if (read_length == frameSize) {
                status = AMA_Target_VoiceStreamSinkFlush();
                if (status != AMA_STATUS_OK) {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data, Send audio data, Packet send failed", 1, status);
                }
            } else {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_send_audio_data, Send voice data, opus encode read failed (length not equal), %d, %d", 2, read_length, frameSize);
                AMA_Target_VoiceStreamSinkFree();
            }
        }
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    }
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
}

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
static void ama_handle_wwd_notification(uint32_t information_data)
{
    if (ama_audio_context.info_sending_data == true) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Ama WWD already enabled, break", 0);
        return;
    }
    if (AMA_AM_STATE_STARTED != ama_audio_context.info_state) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Current state is %d", 1, ama_audio_context.info_state);
        return;
    }

    ama_audio_context.info_sending_data = true;

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] AMA_TASK_EVENT_AUDIO_WWD_NOTIFICATION", 0);
    /**
     * @brief
     * The received information from DSP which including the wake word length and the total length of the DSP buffer
     * bit16 ~ bit31 is the total length
     * bit0 ~ bit15 is the wake word length
     */
    /**
     * @brief
     * The total length
     */
    uint16_t total_length = (information_data & 0xFFFF0000) >> 16;
    /**
     * @brief
     * The wake word length
     */
    uint16_t wake_word_length = (information_data & 0x0000FFFF);
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Information length : 0x%x (%d), wake_word_length : 0x%x(%d), total_length : 0x%x(%d)",
                        6, information_data, information_data, wake_word_length, wake_word_length, total_length, total_length);

    /**
     * @brief
     * The PCM total_length and wake_word_length are 320 multiple, not pcm_bytes_per_one_opus(640) multiple.
     * Must cut them to pcm_bytes_per_one_opus(640) multiple in order to whole opus encode.
     */
    uint16_t pcm_bytes_per_one_opus = ama_audio_context.info_pcm_length_in_one_frame;
    total_length -= (total_length % pcm_bytes_per_one_opus);
    wake_word_length -= (wake_word_length % pcm_bytes_per_one_opus);
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] pcm_bytes_per_one_opus=%d wake_word_length=%d, total_length=%d",
                            3, pcm_bytes_per_one_opus, wake_word_length, total_length);

    /**
     * @brief
     * The current length of the opus ring buffer
     * Which maybe contains the wake word and the preroll
     */
    uint32_t current_opus_payload_length = audio_codec_buffer_mode_get_output_data_length(ama_audio_context.info_codec_manager_handle);//audio_opus_encoder_get_payload_length();

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Current opus ring buffer length : %d", 1, current_opus_payload_length);

    /**
     * @brief
     * The opus preroll length which received
     */
    uint16_t opus_preroll_length = 0;

    if (wake_word_length >= total_length) {
        /**
         * @brief If the wake word length is bigger than the total length of the DSP buffer
         * Means that the wake word has been already sent to CM4 (Partly). So the CM4 ring buffer contains the
         * preroll audio data (opus encoded) and the wake word (opus encoded)
         *
         * So the preroll audio data is : current opus payload length, minus the received wake word opus length
         */
        uint16_t opus_received_ww_length = (wake_word_length - total_length) / ama_audio_context.info_compress_ratio;//OPUS_COMPRESS_RATIO_32KBPS;
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] The received opus wake word length (which stored in the CM4 codec ring buffer): %d", 1, opus_received_ww_length);
        if (current_opus_payload_length < opus_received_ww_length) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] <ERROR> The current opus length < opus received wake word length", 0);
            // assert(false);
            /**
             * @brief Fix issue : BTA-8896
             * If the DSP send the error length information, need restart the audio recorder.
             * Actually, this should not happen.
             */
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] (ERROR INFORMATION) Restart the audio recorder, flash address : 0x%08x, length : 0x%08x", 2,
                        ama_audio_context.info_wwd_flash_address, ama_audio_context.info_wwd_length);
            if (ama_audio_context.info_wwd_flash_address != 0x00 && ama_audio_context.info_wwd_length != 0x00) {
                ama_audio_restart(WWE_MODE_AMA, ama_audio_context.info_wwd_flash_address, ama_audio_context.info_wwd_length);
            }
            return;
        }
        opus_preroll_length = current_opus_payload_length - opus_received_ww_length;
    } else {
        /**
         * @brief If the wake word length is smaller than the total length of the DSP buffer
         * Means that the wake word are all stored in the DSP buffer, and the DSP buffer also contains some preroll data (PCM)
         *
         * So the total preroll audio data is : the preroll data in the DSP buffer (PCM), add the stored bytes in the CM4 ring buffer (opus encoded)
         */
        uint16_t opus_receiving_preroll_length = (total_length - wake_word_length) / ama_audio_context.info_compress_ratio;
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] The receiving opus preroll length : %d", 1, opus_receiving_preroll_length);
        opus_preroll_length = opus_receiving_preroll_length + current_opus_payload_length;
    }
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] The opus length : %d, required preroll length : %d", 2, opus_preroll_length, ama_audio_context.info_wwd_preroll_recommend_len);

    uint32_t stop_index = AMA_AUDIO_WWD_START_INDEX + (wake_word_length / 2);
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Wake word stop index : %d", 1, stop_index);

#ifdef AIR_AMA_SIDETONE_ENABLE
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] side_tone_enabled : %d", 1, ama_audio_context.info_side_tone_enabled);
    if (ama_audio_context.info_side_tone_enabled == false) {
#ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
        bt_sink_srv_am_result_t ami_result = am_audio_side_tone_enable();
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Enable Side_tone result : %d", 1, ami_result);
        if (ami_result == AUD_EXECUTION_SUCCESS) {
#else
        audio_source_control_result_t result = audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                                                        AUDIO_SOURCE_CONTROL_CMD_START_SIDETONE,
                                                                        AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION][Audio Source Control] Enable Side_tone result : %d", 1, result);
        if (result == AUDIO_SOURCE_CONTROL_RESULT_OK) {
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
            ama_audio_context.info_side_tone_enabled = true;
            ama_audio_context.info_side_tone_stopped = false;
            ama_audio_context.info_side_tone_started = false;
        }
    }
#endif /* AIR_AMA_SIDETONE_ENABLE */

    if (ama_audio_context.info_wwd_start_speech_callback != NULL) {
        bool ret = ama_audio_context.info_wwd_start_speech_callback(stop_index);
        if (ret != true) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Start speech failed", 0);
            ama_audio_context.info_sending_data = false;

            /**
             * @brief If start speech failed, need restart the recorder for next round.
             */
            if (ama_audio_context.info_wwd_flash_address != 0x00 && ama_audio_context.info_wwd_length != 0x00) {
                ama_audio_restart(WWE_MODE_AMA, ama_audio_context.info_wwd_flash_address, ama_audio_context.info_wwd_length);
            }

            return;
        } else {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Start speech succeed", 0);
        }
    } else {
        /**
         * @brief If the application layer callback function is NULL,
         * Cannot start speech.
         */
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Failed to start speech, cause the callback function is NULL", 0);
        ama_audio_stop();
        ama_audio_context.info_sending_data = false;
        return;
    }

    /**
     * @brief
     * If the opus preroll length is smaller than the required length, means need to padding the silence data in the forground.
     * If the opus preroll length is bigger than the required length, means need to remove the header of the preroll data that stored in the CM4 ring buffer.
     */
    if (opus_preroll_length < ama_audio_context.info_wwd_preroll_recommend_len) {
        ama_audio_context.info_wwd_preroll_padding_data_len = ama_audio_context.info_wwd_preroll_recommend_len - opus_preroll_length;
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Preroll padding data length : %d", 1, ama_audio_context.info_wwd_preroll_padding_data_len);
    } else if (opus_preroll_length > ama_audio_context.info_wwd_preroll_recommend_len) {
        /**
         * @brief Fix issue : BTA-8640
         * Store the dummy read PCM data length
         */
        ama_audio_context.info_wwd_dummy_read_length = (opus_preroll_length - ama_audio_context.info_wwd_preroll_recommend_len) * ama_audio_context.info_compress_ratio;
        ama_audio_context.info_wwd_dummy_read_index = 0;
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Dummy read PCM total length : %d", 1, ama_audio_context.info_wwd_dummy_read_length);
        if (current_opus_payload_length > opus_preroll_length) {
            /**
             * @brief If the opus ring buffer length is bigger than the opus preroll length, then need
             * read the data.
             *
             */
            uint16_t length = opus_preroll_length - ama_audio_context.info_wwd_preroll_recommend_len;
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Dummy read OPUS total length : %d", 1, length);

            audio_codec_buffer_mode_read_done(ama_audio_context.info_codec_manager_handle, length);
        }
    } else {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Opus preroll length == required length", 0);
    }

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] ama_audio_context.info_dvfs_locked : %d", 1, ama_audio_context.info_dvfs_locked);
    /**
     * @brief Lock DVFS when wake word detected.
     */
    if (ama_audio_context.info_dvfs_locked == false) {
        hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_LOCK);
        ama_audio_context.info_dvfs_locked = true;
    }

    ama_msg_item_t event;
    event.event_id = AMA_TASK_EVENT_AUDIO_SEND_DATA;
    event.data = NULL;
    ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &event);

    /**
     * @brief Start a timer to read audio data (PCM) from DSP.
     */
    ama_audio_timer_start();
    // APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][TASK] AMA_TASK_EVENT_AUDIO_WWD_NOTIFICATION, send AMA_TASK_EVENT_AUDIO_SEND_DATA", 0);
}
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */

/**
 * @brief Restart the audio recorder for WWD
 *
 * @param mode
 * @return true
 * @return false
 */
static bool ama_audio_recorder_wwd_restart()
{
    switch (ama_audio_context.info_state) {
        case AMA_AM_STATE_IDLE: {
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_wwd_restart, IDLE state, restart mode : %d, restart : %d, requested : %d, released : %d, started : %d, stopped : %d", 6,
                                ama_audio_context.info_restart_mode,
                                ama_audio_context.info_is_restart,
                                ama_audio_context.info_audio_source_requested,
                                ama_audio_context.info_audio_source_released,
                                ama_audio_context.info_audio_source_transmitter_started,
                                ama_audio_context.info_audio_source_transmitter_stopped);

            if ((ama_audio_context.info_audio_source_requested == false)
                    && (ama_audio_context.info_audio_source_released == true)) {
                ama_audio_context.info_is_restart = true;
                app_ama_audio_start_request_control(true);
            } else {
                if ((ama_audio_context.info_audio_source_transmitter_started == false)
                        && (ama_audio_context.info_audio_source_transmitter_stopped == true)) {
                    ama_audio_context.info_is_restart = true;
                    app_ama_audio_execute_transmitter(false);
                } else {
                    ama_audio_recorder_start(ama_audio_context.info_restart_mode);
                }
            }
            return true;
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
            return ama_audio_recorder_start(ama_audio_context.info_restart_mode);
        }

        case AMA_AM_STATE_STARTING: {
            return false;
        }

        case AMA_AM_STATE_STOPING: {
            ama_audio_context.info_is_restart = true;
            return false;
        }

        case AMA_AM_STATE_STARTED: {
            ama_audio_context.info_is_restart = true;
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
            app_ama_audio_start_request_control(false);
            app_ama_audio_execute_transmitter(false);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
            return ama_audio_recorder_stop();
        }

        default:
            return false;
    }
}

void ama_audio_restart(wwe_mode_t wwe_mode, uint32_t wwe_flash_address, uint32_t wwe_length)
{
    if (wwe_mode == WWE_MODE_AMA && (wwe_length == 0 || wwe_flash_address == 0)) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO ", ama_audio_restart, mode is AMA, but length/address is 0", 0);
        return;
    }

    APPS_LOG_MSGID_I(APP_AMA_AUDIO ", ama_audio_restart, wwe_mode : %d, flash_address : 0x%08x, length : 0x%08x",
                        3, wwe_mode, wwe_flash_address, wwe_length);

    ama_msg_item_t ama_msg_item_event;
    ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_RESTART;
    ama_msg_item_event.data = NULL;

    ama_audio_context.info_restart_mode = wwe_mode;

    if (wwe_mode == WWE_MODE_AMA) {
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
        ama_audio_context.info_wwd_flash_address = wwe_flash_address;
        ama_audio_context.info_wwd_length = wwe_length;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
    }

    if (ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event) == 0) {
        //ama_wwe_running = true;
    } else {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO ", ama_audio_restart, restart failed <queue send failed>", 0);
        ama_audio_context.info_need_force_restart_recorder = true;
    }
}

/**
 * @brief Init the opus codec
 *
 * @return true
 * @return false
 */
static bool ama_audio_init_opus(void)
{
    /**
     * @brief For new audio codec manager API implementation
     *
     */
    ama_audio_context.info_codec_ring_buffer_len = 0;
    ama_audio_context.info_compress_ratio = 0;
    ama_audio_context.info_encode_output_len = 0;
    ama_audio_context.info_pcm_length_in_one_frame = 0;

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    ama_audio_context.info_wwd_preroll_padding_data_len = 0;
    ama_audio_context.info_wwd_preroll_recommend_len = 0;

    ama_audio_context.info_wwd_dummy_read_index = 0;
    ama_audio_context.info_wwd_dummy_read_length = 0;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */

    audio_codec_manager_status_t st;

    audio_codec_manager_config_t opus_config;
    opus_config.codec_mode = CODEC_MODE_BUFFER;

    opus_config.codec_config.opus_encoder_config.param.channel = OPUS_CODEC_CHANNEL_MONO;
    /* Sample rate */
    opus_config.codec_config.opus_encoder_config.param.samplerate = OPUS_CODEC_SAMPLERATE_16KHZ;
    /* Encode bitrate */
    opus_config.codec_config.opus_encoder_config.param.bitrate = OPUS_CODEC_BITRATE_32KBPS;

    memset(ama_audio_context.info_codec_buffer, 0, AMA_CODEC_BUF_LEN);

    opus_config.codec_config.opus_encoder_config.opus_encoder_working_buffer_ptr = (uint32_t *)ama_audio_context.info_codec_buffer;

    st = audio_codec_open(CODEC_TYPE_OPUS_ENCODE, &opus_config, &ama_audio_context.info_codec_manager_handle);
    if (st != AUDIO_CODEC_MANAGER_SUCCESS) {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", ama_audio_init_opus, open opus codec failed, %d", 1, st);
        goto FAILED;
    }
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init_opus, opus handle : %d", 1, ama_audio_context.info_codec_manager_handle);

    st = audio_codec_get_inout_frame_length(ama_audio_context.info_codec_manager_handle,
                                            &ama_audio_context.info_pcm_length_in_one_frame,
                                            &ama_audio_context.info_encode_output_len);

    if (ama_audio_context.info_pcm_length_in_one_frame == 0
            || ama_audio_context.info_encode_output_len == 0) {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", ama_audio_init_opus, error frame parameter %d %d",
                            2, ama_audio_context.info_pcm_length_in_one_frame, ama_audio_context.info_encode_output_len);
        goto FAILED;
    } else {
        ama_audio_context.info_compress_ratio = ama_audio_context.info_pcm_length_in_one_frame / ama_audio_context.info_encode_output_len;
    }

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    ama_audio_context.info_wwd_preroll_recommend_len = ama_audio_context.info_encode_output_len * 25;
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD] ama_audio_init_opus, Preroll recommend length : %d", 1, ama_audio_context.info_wwd_preroll_recommend_len);
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init_opus, get one frame in length : %d, out length : %d, compress ratio : %d", 3,
                                ama_audio_context.info_pcm_length_in_one_frame,
                                ama_audio_context.info_encode_output_len,
                                ama_audio_context.info_compress_ratio);

    if (ama_audio_context.info_codec_ring_buffer != NULL) {
        vPortFree(ama_audio_context.info_codec_ring_buffer);
        ama_audio_context.info_codec_ring_buffer = NULL;
    }

    ama_audio_context.info_codec_ring_buffer_len = (ama_audio_context.info_encode_output_len * AMA_OPUS_CODEC_RING_BUFFER_COUNT);
    ama_audio_context.info_codec_ring_buffer = (uint8_t *)pvPortMalloc(sizeof(uint8_t) * ama_audio_context.info_codec_ring_buffer_len);

    if (ama_audio_context.info_codec_ring_buffer == NULL) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init_opus, Failed to allocate codec ring buffer", 0);
        goto FAILED;
    }
    memset(ama_audio_context.info_codec_ring_buffer, 0, sizeof(uint8_t) * ama_audio_context.info_codec_ring_buffer_len);

    audio_codec_manager_user_config_t user_config;
    user_config.output_buffer = ama_audio_context.info_codec_ring_buffer;
    user_config.output_length = ama_audio_context.info_codec_ring_buffer_len;
    user_config.callback = ama_audio_codec_manager_callback;
    user_config.user_data = NULL;
    st = audio_codec_buffer_mode_set_config(ama_audio_context.info_codec_manager_handle, &user_config);
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init_opus, set config result : %d", 1, st);

    if (st != AUDIO_CODEC_MANAGER_SUCCESS) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init_opus, Set config failed, close codec", 0);
        goto FAILED;
    }

    if (ama_audio_context.info_pcm_read_buffer != NULL) {
        vPortFree(ama_audio_context.info_pcm_read_buffer);
        ama_audio_context.info_pcm_read_buffer = NULL;
    }

    ama_audio_context.info_pcm_read_buffer = (uint8_t *)pvPortMalloc(sizeof(uint8_t) * ama_audio_context.info_pcm_length_in_one_frame);
    if (ama_audio_context.info_pcm_read_buffer == NULL) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init_opus, Failed to allocate PCM read buffer", 0);
        goto FAILED;
    }
    memset(ama_audio_context.info_pcm_read_buffer, 0, sizeof(uint8_t) * ama_audio_context.info_pcm_length_in_one_frame);

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init_opus, Succeed to init opus codec", 0);

    goto SUCCEED;

FAILED:
    if (ama_audio_context.info_codec_manager_handle != 0) {
        audio_codec_close(ama_audio_context.info_codec_manager_handle);
    }
    if (ama_audio_context.info_codec_ring_buffer != NULL) {
        vPortFree(ama_audio_context.info_codec_ring_buffer);
        ama_audio_context.info_codec_ring_buffer = NULL;
    }
    if (ama_audio_context.info_pcm_read_buffer != NULL) {
        vPortFree(ama_audio_context.info_pcm_read_buffer);
        ama_audio_context.info_pcm_read_buffer = NULL;
    }

    memset(ama_audio_context.info_codec_buffer, 0, AMA_CODEC_BUF_LEN);

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    ama_audio_context.info_wwd_preroll_recommend_len = 0;
    ama_audio_context.info_wwd_preroll_padding_data_len = 0;

    ama_audio_context.info_wwd_dummy_read_index = 0;
    ama_audio_context.info_wwd_dummy_read_length = 0;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */

    ama_audio_context.info_compress_ratio = 0;
    ama_audio_context.info_pcm_length_in_one_frame = 0;
    ama_audio_context.info_codec_ring_buffer_len = 0;

    assert(false);

    return false;

SUCCEED:
    return true;
}

static void ama_audio_deinit(void)
{
    /**
     * @brief If deinit happen, clear all queue message.
     * Fix issue : BTA-6606
     * When deinit, clear the task queue firstly to make sure that the other message can be append succeed.
     */
    // xQueueReset((QueueHandle_t)ama_audio_task_queue);

    record_control_result_t result_record = audio_record_control_deinit(ama_audio_context.info_recorder_id);
    audio_codec_manager_status_t st = AUDIO_CODEC_MANAGER_SUCCESS;

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_deinit, codec manager handle : %d", 1, ama_audio_context.info_codec_manager_handle);
    audio_codec_buffer_mode_clear_output_data(ama_audio_context.info_codec_manager_handle);
    if (ama_audio_context.info_codec_manager_handle != 0) {
        st = audio_codec_close(ama_audio_context.info_codec_manager_handle);
    }

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_deinit, record_deinit : %d, codec close : %d", 2, result_record, st);

#ifdef AIR_AMA_SIDETONE_ENABLE
    /**
     * @brief If start recorder failed with WWE_MODE_NONE, need disable sidetone.
     * Cause side tone enabled while start API called.
     */
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_deinit, side_tone_enabled : %d, current state : %d",
                    2, ama_audio_context.info_side_tone_enabled, ama_audio_context.info_state);
    if ((ama_audio_context.info_side_tone_enabled == true)
            && ((ama_audio_context.info_state == AMA_AM_STATE_STARTING)
                    || (ama_audio_context.info_state == AMA_AM_STATE_STARTED))) {
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
        audio_source_control_result_t result = audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                                                        AUDIO_SOURCE_CONTROL_CMD_STOP_SIDETONE,
                                                                        AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Enable Side_tone result : %d", 1, result);
#else
        bt_sink_srv_am_result_t result = am_audio_side_tone_disable();
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_deinit, Disable side_tone result : %d", 1, result);
    }
#endif /* AIR_AMA_SIDETONE_ENABLE */

    /**
     * @brief If start recorder failed with WWE_MODE_NONE, need unlock dvfs.
     * Cause the dvfs should be locked while start API called.
     */
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_deinit, dvfs_locked : %d", 1, ama_audio_context.info_dvfs_locked);
    if (ama_audio_context.info_dvfs_locked == true) {
        hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_UNLOCK);
        ama_audio_context.info_dvfs_locked = false;
    }

    ama_audio_timer_stop();

    ama_audio_context.info_state = AMA_AM_STATE_IDLE;
    ama_audio_context.info_current_mode = WWE_MODE_NONE;
    ama_audio_context.info_recorder_id = AMA_AM_INVALID_ID;
    ama_audio_context.info_sending_data = false;

    if (ama_audio_context.info_codec_ring_buffer != NULL) {
        vPortFree(ama_audio_context.info_codec_ring_buffer);
        ama_audio_context.info_codec_ring_buffer = NULL;
    }
    if (ama_audio_context.info_pcm_read_buffer != NULL) {
        vPortFree(ama_audio_context.info_pcm_read_buffer);
        ama_audio_context.info_pcm_read_buffer = NULL;
    }

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
    ama_audio_context.info_wwd_preroll_recommend_len = 0;
    ama_audio_context.info_wwd_preroll_padding_data_len = 0;

    ama_audio_context.info_wwd_dummy_read_index = 0;
    ama_audio_context.info_wwd_dummy_read_length = 0;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */

    memset(ama_audio_context.info_codec_buffer, 0, AMA_CODEC_BUF_LEN);

    ama_audio_context.info_compress_ratio = 0;
    ama_audio_context.info_pcm_length_in_one_frame = 0;
    ama_audio_context.info_codec_ring_buffer_len = 0;

    ama_audio_context.info_need_force_stop_recorder = false;
    ama_audio_context.info_is_data_notified = false;
}

static bool ama_audio_recorder_start(wwe_mode_t mode)
{
    record_encoder_cability_t encoder;

    if (AMA_AM_STATE_STOPING == ama_audio_context.info_state) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, recorder is stopping state", 0);
        return false;
    }
    if (AMA_AM_STATE_IDLE != ama_audio_context.info_state) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, Audio recorder already started. state: %d, target mode : %d, current mode : %d",
                3, ama_audio_context.info_state, mode, ama_audio_context.info_current_mode);

        if (mode != ama_audio_context.info_current_mode) {
            /**
             * Fix issue : BTA-8978
             * If start again and the mode is not equal, need restart again.
             */
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, mode not match, need restart the recorder", 0);
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
            ama_audio_restart(mode, ama_audio_context.info_wwd_flash_address, ama_audio_context.info_wwd_length);
#endif
        }

        return false;
    }

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, wwe_mode : %d", 1, mode);

    /* 1. audio record init */
    encoder.wwe_mode = mode;
    encoder.bit_rate = ENCODER_BITRATE_16KBPS;//ENCODER_BITRATE_32KBPS;
    if (encoder.wwe_mode == WWE_MODE_AMA) {
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
        encoder.codec_type = AUDIO_DSP_CODEC_TYPE_PCM_WWE;
        encoder.wwe_language_mode_address = ama_audio_context.info_wwd_flash_address;
        encoder.wwe_language_mode_length = ama_audio_context.info_wwd_length;
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, WWE start with address : 0x%08x, length : 0x%08x", 2, encoder.wwe_language_mode_address, encoder.wwe_language_mode_length);
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
    } else if (mode == WWE_MODE_NONE) {
        encoder.codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
    } else {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, Unsupported wwe mode : %d", 1, mode);
        return false;
    }

    ama_audio_context.info_recorder_id = audio_record_control_enabling_encoder_init(ama_audio_control_ccni_callback, NULL, ama_audio_am_callback, &encoder);
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, Init audio recorder mode : %d, inited ID : %x", 2, encoder.wwe_mode, ama_audio_context.info_recorder_id);

    if (AMA_AM_INVALID_ID == ama_audio_context.info_recorder_id) {
        return false;
    }

    /* 2. audio opus init */
    if (ama_audio_init_opus() == false) {
        record_control_result_t result;

        result = audio_record_control_deinit(ama_audio_context.info_recorder_id);
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", ama_audio_recorder_start, Init opus encoder failed, deinit result: %d", 1, result);

        ama_audio_context.info_recorder_id = AMA_AM_INVALID_ID;
        ama_audio_context.info_state = AMA_AM_STATE_IDLE;
        return false;
    }

    ama_audio_context.info_state  = AMA_AM_STATE_STARTING;
    ama_audio_context.info_current_mode = mode;

    /* 3. audio record start and wait for msg_id = AUD_SINK_OPEN_CODEC */
    record_control_result_t start_result = audio_record_control_start(ama_audio_context.info_recorder_id);
    if (start_result != RECORD_CONTROL_EXECUTION_SUCCESS) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, Audio recorder start failed: %d", 1, start_result);
        ama_audio_deinit();
        return false;
    }

    if (mode == WWE_MODE_NONE) {
        /**
         * @brief If current is not VAD mode, means the provide speech or with not support WWD feature (Tap-To-Talk or Push-To-Talk)
         * Send the audio data directly
         */
        ama_audio_context.info_sending_data = true;

#ifdef AIR_AMA_SIDETONE_ENABLE
        if (ama_audio_context.info_side_tone_enabled == false) {
#ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
            bt_sink_srv_am_result_t ami_result = am_audio_side_tone_enable();
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, Enable Side_tone result : %d", 1, ami_result);

            if (ami_result == AUD_EXECUTION_SUCCESS) {
#else
            audio_source_control_result_t result = audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                                                        AUDIO_SOURCE_CONTROL_CMD_START_SIDETONE,
                                                                        AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][WWD_NOTIFICATION] Enable Side_tone result : %d", 1, result);
            if (result == AUDIO_SOURCE_CONTROL_RESULT_OK) {
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                ama_audio_context.info_side_tone_enabled = true;
                ama_audio_context.info_side_tone_stopped = false;
                ama_audio_context.info_side_tone_started = false;
            }
        }
#endif /* AIR_AMA_SIDETONE_ENABLE */

        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, ama_audio_context.info_dvfs_locked : %d", 1, ama_audio_context.info_dvfs_locked);
        if (ama_audio_context.info_dvfs_locked == false) {
            hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_LOCK);
            ama_audio_context.info_dvfs_locked = true;
        }

        /**
         * @brief Start a timer to read audio data (PCM) from DSP.
         */
        ama_audio_timer_start();
    } else {
        /**
         * @brief If current is VAD mode, and WWD is not detected, set to be false
         */
        ama_audio_context.info_sending_data = false;
    }
    /**
     * @brief Make data notified to be false.
     */
    ama_audio_context.info_is_data_notified = false;

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_start, Audio recorder start succeed id %d", 1, ama_audio_context.info_recorder_id);
    return true;
}

static bool ama_audio_recorder_stop(void)
{
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_stop, Current state : %d", 1, ama_audio_context.info_state);
    if (AMA_AM_STATE_STARTING == ama_audio_context.info_state) {
        return false;
    }

    if (AMA_AM_STATE_STARTED != ama_audio_context.info_state) {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", ama_audio_recorder_stop, Audio recorder already stopped. state %d", 1, ama_audio_context.info_state);
        return false;
    }

    if (AMA_AM_INVALID_ID == ama_audio_context.info_recorder_id) {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", ama_audio_recorder_stop, Audio recorder ID is 0.", 0);
        return false;
    }

#ifdef AIR_AMA_SIDETONE_ENABLE
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_stop, side_tone_enabled : %d, side_tone_started : %d", 2,
                        ama_audio_context.info_side_tone_enabled, ama_audio_context.info_side_tone_started);
    if (ama_audio_context.info_side_tone_enabled == true) {
#ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
        bt_sink_srv_am_result_t ami_result = am_audio_side_tone_disable();
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_stop, Disable side_tone result : %d", 1, ami_result);
#else
        audio_source_control_result_t result = audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                                                        AUDIO_SOURCE_CONTROL_CMD_STOP_SIDETONE,
                                                                        AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_stop, Disable Side_tone result : %d", 1, result);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
        // if (ami_result == AUD_EXECUTION_SUCCESS) {
        //     ama_audio_context.info_side_tone_enabled = false;
        // }
    }
#endif /* AIR_AMA_SIDETONE_ENABLE */

    /**
     * @brief Unlock the frequency
     *
     */
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_stop, ama_audio_context.info_dvfs_locked : %d", 1, ama_audio_context.info_dvfs_locked);
    if (ama_audio_context.info_dvfs_locked == true) {
        hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_UNLOCK);
        ama_audio_context.info_dvfs_locked = false;
    }

    /**
     * @brief Stop the audio data reading timer
     */
    ama_audio_timer_stop();

    ama_audio_context.info_is_data_notified = false;

    bool current_sending_data = ama_audio_context.info_sending_data;

    ama_audio_context.info_sending_data = false;
    ama_audio_context.info_state = AMA_AM_STATE_STOPING;
    record_control_result_t recorder_stop_result = audio_record_control_stop(ama_audio_context.info_recorder_id);
    if (RECORD_CONTROL_EXECUTION_SUCCESS != recorder_stop_result) {
        ama_audio_context.info_state = AMA_AM_STATE_STARTED;
        ama_audio_context.info_sending_data = current_sending_data;
    }

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_recorder_stop, Audio recorder stop_result : %d", 1, recorder_stop_result);

    return true;
}

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
static bool app_ama_audio_restart_checker()
{
    if (ama_audio_context.info_is_restart == false) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_restart_checker, restart is false", 0);
        return false;
    }

    if (ama_audio_context.info_state != AMA_AM_STATE_IDLE) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_restart_checker, current state is not IDLE", 0);
        return false;
    }

    if (ama_audio_context.info_audio_source_released == false) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_restart_checker, audio source has not been release", 0);
        return false;
    }

    if (ama_audio_context.info_audio_source_transmitter_stopped == false) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_restart_checker, transmitter has not been stopped", 0);
        return false;
    }

    bool need_restart = false;

#ifdef AIR_AMA_SIDETONE_ENABLE
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_restart_checker, info_side_tone_enabled : %d, info_side_tone_stopped : %d, info_side_tone_started : %d",
                        3, ama_audio_context.info_side_tone_enabled,
                        ama_audio_context.info_side_tone_stopped,
                        ama_audio_context.info_side_tone_started);

        if ((ama_audio_context.info_side_tone_enabled == true) && (ama_audio_context.info_side_tone_started == true)) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_restart_checker, side tone start succeed, and check stopped value", 0);
            if (ama_audio_context.info_side_tone_stopped == true) {
                /**
                 * @brief If the side tone started and already stopped, need restart recorder
                 */
                need_restart = true;
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_restart_checker, side tone already stopped, mark as true", 0);
            }
        } else {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_restart_checker, side tone is not started, mark as true", 0);
            /**
             * @brief If the side tone is not started, need restart recorder
             */
            need_restart = true;
        }
#else
    need_restart = true;
#endif /* AIR_AMA_SIDETONE_ENABLE */

    return need_restart;
}
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

static void ama_wwe_audio_recorder_action_done(bt_sink_srv_am_cb_sub_msg_t sub_msg)
{
    if (AMA_AM_STATE_STARTING == ama_audio_context.info_state) {
        if (AUD_CMD_COMPLETE == sub_msg) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, Start recorder succeed", 0);
            ama_audio_context.info_state = AMA_AM_STATE_STARTED;

            if (ama_audio_context.info_need_force_stop_recorder == true) {
                /**
                 * @brief Fix issue that send stop event to queue failed.
                 * When start finished, need stop recorder.
                 */
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, need_force_stop is true, stop audio recorder", 0);
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                app_ama_audio_start_request_control(false);
                app_ama_audio_execute_transmitter(false);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                ama_audio_recorder_stop();
                ama_audio_queue_reset((QueueHandle_t)ama_audio_task_queue);
            }
        } else {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, Start recorder failed, error : %d", 1, sub_msg);
            // ama_audio_deinit();

            /**
             * @brief Fix bug when start recorder failed, need notify that the recorder
             * start failed
             */
            ama_msg_item_t ama_msg_item_event;
            ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_STOP_BY_IND;
            ama_audio_queue_send_front((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
            /**
             * @brief If start recorder failed, need release the resource.
             *
             */
            app_ama_audio_start_request_control(false);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
        }
    } else if (AMA_AM_STATE_STOPING == ama_audio_context.info_state) {
        if (AUD_CMD_COMPLETE == sub_msg) {
            ama_audio_deinit();

            bool need_restart = false;
#ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
#ifdef AIR_AMA_SIDETONE_ENABLE
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, is_restart : %d, info_side_tone_enabled : %d, info_side_tone_stopped : %d, info_side_tone_started : %d",
                                4, ama_audio_context.info_is_restart,
                                ama_audio_context.info_side_tone_enabled,
                                ama_audio_context.info_side_tone_stopped,
                                ama_audio_context.info_side_tone_started);

            if (ama_audio_context.info_is_restart == true) {
                if ((ama_audio_context.info_side_tone_enabled == true) && (ama_audio_context.info_side_tone_started == true)) {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, side tone start succeed, and check stopped value", 0);
                    if (ama_audio_context.info_side_tone_stopped == true) {
                        /**
                         * @brief If the side tone started and already stopped, need restart recorder
                         */
                        need_restart = true;
                        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, side tone already stopped, mark as true", 0);
                    }
                } else {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, side tone is not started, mark as true", 0);
                    /**
                     * @brief If the side tone is not started, need restart recorder
                     */
                    need_restart = true;
                }
            } else {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, restart is false, mark as false", 0);
            }
#else
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, is_restart : %d", 1, ama_audio_context.info_is_restart);
            if (ama_audio_context.info_is_restart == true) {
                need_restart = true;
            }
#endif /* AIR_AMA_SIDETONE_ENABLE */

            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, need_restart : %d", 1, need_restart);

            if (need_restart == true) {
                ama_audio_context.info_is_restart = false;
                ama_audio_recorder_start(ama_audio_context.info_restart_mode);
            }
        } else {
            if (ama_audio_context.info_is_restart) {
                ama_audio_context.info_is_restart = false;
            }
            ama_audio_context.info_state = AMA_AM_STATE_STARTED;
        }
#else
            need_restart = app_ama_audio_restart_checker();
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_wwe_audio_recorder_action_done, need_restart : %d", 1, need_restart);
            if (need_restart == true) {
                ama_audio_context.info_is_restart = false;
                app_ama_audio_start_request_control(true);
            }
        }
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
    }
}

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
static void app_ama_audio_start_request_control(bool request)
{
    if (ama_audio_context.info_audio_source_control_handle != NULL) {
        audio_source_control_result_t result = AUDIO_SOURCE_CONTROL_RESULT_OK;

        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_start_request_control, %d - %d - %d", 3,
                        request,
                        ama_audio_context.info_audio_source_requested,
                        ama_audio_context.info_audio_source_released);

        if ((request == true) && (ama_audio_context.info_audio_source_requested == false)) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_start_request_control, request audio source", 0);
            result = audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                                AUDIO_SOURCE_CONTROL_CMD_REQUEST_RES,
                                                AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
            if (result != AUDIO_SOURCE_CONTROL_RESULT_OK) {
                ama_audio_context.info_audio_source_requested = false;
                APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_start_request_control, request control failed with error : %d", 1, result);
            } else {
                ama_audio_context.info_audio_source_requested = true;
                ama_audio_context.info_audio_source_released = false;
            }
        } else if ((request == false) && (ama_audio_context.info_audio_source_released == false)) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_start_request_control, release audio source", 0);
            result = audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                                AUDIO_SOURCE_CONTROL_CMD_RELEASE_RES,
                                                AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
            if (result != AUDIO_SOURCE_CONTROL_RESULT_OK) {
                APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_start_request_control, release control failed with error : %d", 1, result);
            } else {
                // ama_audio_context.info_audio_source_released = true;
            }
        }
    } else {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_start_request_control, audio control handle is NULL", 0);
    }
}

static void app_ama_audio_execute_transmitter(bool start)
{
    if (ama_audio_context.info_audio_source_control_handle != NULL) {
        audio_source_control_result_t result = AUDIO_SOURCE_CONTROL_RESULT_OK;

        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_execute_transmitter, %d - %d - %d", 3,
                        start,
                        ama_audio_context.info_audio_source_transmitter_started,
                        ama_audio_context.info_audio_source_transmitter_stopped);

        if ((start == true) && (ama_audio_context.info_audio_source_transmitter_started == false)) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_execute_transmitter, start transmitter", 0);
            result = audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                                AUDIO_SOURCE_CONTROL_CMD_START_TRANSMITTER,
                                                AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
            if (result != AUDIO_SOURCE_CONTROL_RESULT_OK) {
                ama_audio_context.info_audio_source_transmitter_started = false;
                APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_execute_transmitter, start transmitter failed with : %d", 1, result);
            } else {
                ama_audio_context.info_audio_source_transmitter_started = true;
                ama_audio_context.info_audio_source_transmitter_stopped = false;
            }
        } else if ((start == false) && (ama_audio_context.info_audio_source_transmitter_stopped == false)) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_execute_transmitter, stop transmitter", 0);
            result = audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                                AUDIO_SOURCE_CONTROL_CMD_STOP_TRANSMITTER,
                                                AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
            if (result != AUDIO_SOURCE_CONTROL_RESULT_OK) {
                APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_execute_transmitter, stop transmitter failed with : %d", 1, result);
            } else {
                // ama_audio_context.info_audio_source_transmitter_stopped = true;
            }
        }
    } else {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_execute_transmitter, audio control handle is NULL", 0);
    }
}

static void app_ama_audio_handle_control_request_result(uint16_t result)
{
    uint8_t type = 0x00;
    uint8_t sub_type = 0x00;

    type = (result & 0xFF00) >> 8;
    sub_type = result & 0x00FF;

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, request control result : %d - %d, 0x%04x",
                        3, type, sub_type, result);

    bool need_restart_check = false;

    switch(type) {
        case AUDIO_SOURCE_CONTROL_EVENT_RES_CTRL: {
            if (sub_type == AUDIO_SRC_SRV_EVENT_GIVE_SUCCESS) {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, give source succeed", 0);
                ama_audio_context.info_audio_source_released = true;
                ama_audio_context.info_audio_source_requested = false;

                need_restart_check = true;
            }
            if (sub_type == AUDIO_SRC_SRV_EVENT_SUSPEND) {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, audio source suspended", 0);

                ama_audio_context.info_audio_source_suspend = true;
                app_ama_audio_execute_transmitter(false);
                app_ama_audio_start_request_control(false);
                /**
                 * @brief Add to waiting list
                 */
                audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                        AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST,
                                        AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);

                ama_audio_recorder_stop();

                if (ama_audio_context.info_recorder_stopped_callback != NULL) {
                    ama_audio_context.info_recorder_stopped_callback();
                }
                return;
            }
            if ((sub_type == AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS)
                    || (sub_type == AUDIO_SRC_SRV_EVENT_TAKE_ALREADY)) {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, take source succeed or already take, suspend : %d, released : %d", 2,
                                    ama_audio_context.info_audio_source_suspend,
                                    ama_audio_context.info_audio_source_released);

                if ((ama_audio_context.info_audio_source_released == true)
                        && (ama_audio_context.info_audio_source_suspend == false)) {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, release audio source step, ignore", 0);
                    return;
                }

                ama_audio_context.info_audio_source_requested = true;
                ama_audio_context.info_audio_source_released = false;

                if ((sub_type == AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS) && (ama_audio_context.info_audio_source_suspend == true)) {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, notify source released", 0);

                    /**
                     * @brief Add to waiting list
                     */
                    audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                            AUDIO_SOURCE_CONTROL_CMD_DEL_WAITTING_LIST,
                                            AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);

                    ama_audio_context.info_audio_source_suspend = false;
                    if (ama_audio_context.info_recorder_released_callback != NULL) {
                        ama_audio_context.info_recorder_released_callback();
                    }
                    return;
                }

                if (ama_audio_context.info_audio_source_suspend == false) {
                    app_ama_audio_execute_transmitter(true);
                }
                return;
            }
            if (sub_type == AUDIO_SRC_SRV_EVENT_TAKE_REJECT) {
                APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, take source failed (reject)", 0);
                ama_audio_context.info_audio_source_released = true;
                ama_audio_context.info_audio_source_requested = false;
                return;
            }
        }
        break;

        case AUDIO_SOURCE_CONTROL_EVENT_TRANSMITTER: {
            if (sub_type == AUDIO_TRANSMITTER_EVENT_START_SUCCESS) {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, start transmitter succeed", 0);
                if (ama_audio_context.info_audio_source_transmitter_started == false) {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, release transmitter step, ignore", 0);
                    return;
                }
                ama_audio_context.info_audio_source_transmitter_started = true;
                ama_audio_context.info_audio_source_transmitter_stopped = false;
                if (ama_audio_context.info_restart_mode != WWE_MODE_MAX) {
                    ama_audio_recorder_start(ama_audio_context.info_restart_mode);
                    ama_audio_context.info_restart_mode = WWE_MODE_MAX;
                } else {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, mode after control is MAX (invalid)", 0);
                    app_ama_audio_execute_transmitter(false);
                }
                return;
            }
            if (sub_type == AUDIO_TRANSMITTER_EVENT_START_FAIL) {
                if (ama_audio_context.info_audio_source_transmitter_started == false) {
                    APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, start transmitter not started", 0);
                    return;
                }
                APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, start transmitter failed, release audio source", 0);
                ama_audio_context.info_audio_source_transmitter_started = false;
                ama_audio_context.info_audio_source_transmitter_stopped = true;
                app_ama_audio_start_request_control(false);
                return;
            }
            if (sub_type == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS) {
                need_restart_check = true;
                ama_audio_context.info_audio_source_transmitter_started = false;
                ama_audio_context.info_audio_source_transmitter_stopped = true;
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, stop transmitter succeed, release audio source", 0);
                app_ama_audio_start_request_control(false);
            }
        }
        break;
        default:
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, unknown type to handle", 0);
        break;
    }

    if (need_restart_check == true) {
        bool need_restart = app_ama_audio_restart_checker();

        if (need_restart == true) {
            ama_audio_context.info_is_restart = false;
            app_ama_audio_start_request_control(true);
        }
    }
}
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

/**
 * @brief The audio task
 *
 * @param arg
 */
void ama_port_task_main(void *arg)
{
    ama_msg_item_t ama_msg_item_event;
    BaseType_t ret = -1;

    ama_audio_task_queue = xQueueCreate(AMA_TASK_QUEUE_SIZE, AMA_TASK_QUEUE_ITEM_SIZE);
    reading_timer_handle = xTimerCreate("A_AMA_TIMER", 10 * portTICK_PERIOD_MS, pdTRUE, NULL, ama_audio_timer_callback_handler);

    if (reading_timer_handle == NULL) {
        ama_audio_context.info_is_timer_start_ok = false;
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] Create timer failed", 0);
    }

    /**
     * For dump pcm data
     */
    bool is_wwd_detected = false;

    while (1) {
        ret = xQueueReceive((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event, portMAX_DELAY / portTICK_PERIOD_MS);

        if (ret == pdPASS) {
            // APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] ama_audio_event_id : %d, state : %d, send : %d",
            //                     3, ama_msg_item_event.event_id, ama_audio_context.info_state, ama_audio_context.info_sending_data);

            switch (ama_msg_item_event.event_id) {
                case AMA_TASK_EVENT_AUDIO_WWD_NOTIFICATION: {
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
                    ama_handle_wwd_notification((uint32_t)ama_msg_item_event.data);
                    is_wwd_detected = true;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
                }
                break;

                case AMA_TASK_EVENT_AUDIO_DATA_ABORT_NOTIFICATION: {
                    /**
                     * @brief
                     * Clear the opus data that already encoded.
                     */
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][AUDIO_TASK] AMA_TASK_EVENT_AUDIO_DATA_ABORT_NOTIFICATION, Data abort notification, clear the opus buffer", 0);
                    //audio_opus_encoder_clear_payload();
                    if (ama_audio_context.info_codec_manager_handle != 0) {
                        audio_codec_buffer_mode_clear_output_data(ama_audio_context.info_codec_manager_handle);
                    }

                    /**
                     * @brief Fix issue that DSP recorder execute failed, restart the recorder again for next round.
                     */
                    // APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][AUDIO_TASK] AMA_TASK_EVENT_AUDIO_DATA_ABORT_NOTIFICATION, ### Restart recorder ###", 0);
                    // ama_audio_recorder_wwd_restart();
                }
                break;

                case AMA_TASK_EVENT_AUDIO_RESTART: {
                    ama_audio_recorder_wwd_restart();
                }
                break;

                case AMA_TASK_EVENT_AUDIO_STREAM_IN: {
                    if (ama_audio_context.info_state != AMA_AM_STATE_STARTED) {
                        /**
                         * @brief If current is not started, ignore the event.
                         * Fix issue : BTA-6606
                         */
                        break;
                    }

                    if (ama_audio_context.info_need_force_stop_recorder == true) {
                        /**
                         * @brief If stop request send to queue failed, need force stop the audio recorder happen.
                         * When received the audio data, need stop the audio recorder.
                         */
                        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_STREAM_IN; force_stop is true, need stop it now", 0);
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                        app_ama_audio_start_request_control(false);
                        app_ama_audio_execute_transmitter(false);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                        ama_audio_recorder_stop();
                        ama_audio_queue_reset((QueueHandle_t)ama_audio_task_queue);
                        break;
                    }

                    if (ama_audio_context.info_need_force_restart_recorder == true) {
                        /**
                         * @brief Fix issue, that send the restart event to queue failed.
                         * restart the audio recorder force.
                         */
                        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_STREAM_IN; force_restart is true, need restart it now", 0);
                        ama_audio_recorder_wwd_restart();
                        ama_audio_context.info_need_force_restart_recorder = false;
                        break;
                    }

                    /**
                     * @brief
                     * Encode the PCM audio data to opus format.
                     * TODO if current is ready to send the audio data, need send event to queue to send data to SP.
                     */
                    uint32_t dsp_pcm_size = audio_record_control_get_share_buf_data_byte_count();
                    uint32_t opus_frame_num = dsp_pcm_size / ama_audio_context.info_pcm_length_in_one_frame;
                    uint8_t index;

                    if (ama_audio_context.info_pcm_read_buffer == NULL) {
                        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_STREAM_IN, pcm read buffer is NULL", 0);
                        assert(false);
                    }

                    if (opus_frame_num < 2) {
                        if (ama_audio_context.info_is_timer_start_ok == false) {
                            ama_msg_item_t event;
                            event.data = NULL;
                            event.event_id = AMA_TASK_EVENT_AUDIO_STREAM_IN;
                            ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &event);
                        }
                        break;
                    }

                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_STREAM_IN; Read (PCM data) : %d, opus_frame_num : %d", 2, dsp_pcm_size, opus_frame_num);

                    uint32_t pcm_codec_len = 0;

                    for (index = 0; index < opus_frame_num; index ++) {
                        if (ama_audio_context.info_state == AMA_AM_STATE_IDLE) {
                            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_STREAM_IN; the audio state changed to stopped, break", 0);
                            break;
                        }

                        pcm_codec_len = ama_audio_context.info_pcm_length_in_one_frame;
                        memset(ama_audio_context.info_pcm_read_buffer, 0, sizeof(uint8_t) * ama_audio_context.info_pcm_length_in_one_frame);
                        if (RECORD_CONTROL_EXECUTION_SUCCESS == audio_record_control_read_data(ama_audio_context.info_pcm_read_buffer, ama_audio_context.info_pcm_length_in_one_frame)) {

#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
                            if (ama_audio_context.info_wwd_dummy_read_length != 0 && ama_audio_context.info_wwd_dummy_read_length > ama_audio_context.info_wwd_dummy_read_index) {
                                /**
                                 * @brief Need read the data which is not preroll.
                                 * Fix issue : BTA-8640
                                 * Cause the data from DSP contains x-preroll data + wake word
                                 * Need dummy read the preroll data which over 500ms
                                 */
                                ama_audio_context.info_wwd_dummy_read_index += ama_audio_context.info_pcm_length_in_one_frame;
                                APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_STREAM_IN; dummy read length (index) : %d", 1, ama_audio_context.info_wwd_dummy_read_index);
                                // LOG_AUDIO_DUMP(ama_audio_context.info_pcm_read_buffer, ama_audio_context.info_pcm_length_in_one_frame, 12);
                            } else
#endif
                            {
                                if (is_wwd_detected == true) {
                                    // LOG_AUDIO_DUMP(ama_audio_context.info_pcm_read_buffer, ama_audio_context.info_pcm_length_in_one_frame, 12);
                                }

                                audio_codec_manager_status_t st = audio_codec_buffer_mode_process(ama_audio_context.info_codec_manager_handle,
                                                                                                    ama_audio_context.info_pcm_read_buffer,
                                                                                                    &pcm_codec_len);
                                if (st != AUDIO_CODEC_MANAGER_SUCCESS) {
                                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] opus encode FAILED, result : %d", 1, st);
                                }
                            }
                        }
                    }

                    bool need_send_data = false;
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
                    if ((!((ama_audio_context.info_wwd_dummy_read_length != 0) && (ama_audio_context.info_wwd_dummy_read_length > ama_audio_context.info_wwd_dummy_read_index)))
                            && (ama_audio_context.info_wwd_preroll_padding_data_len > 0)) {
                        need_send_data = true;
                    } else
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
                    {
                        uint32_t current_opus_length = audio_codec_buffer_mode_get_output_data_length(ama_audio_context.info_codec_manager_handle);
                        uint32_t current_mtu_value = app_ama_get_mtu();
                        if ((current_mtu_value > 0) && (current_opus_length >= current_mtu_value)) {
                            /**
                             * @brief If current opus encoded length is bigger than mtu to send the buffer,
                             * need send event to send audio data. otherwise, the codec buffer will be full, cause the audio data
                             * steam_in event is too many and too quick.
                             */
                            need_send_data = true;
                        } else {
                            need_send_data = false;
                        }
                    }

                    if ((ama_audio_context.info_sending_data == true)
                            && (ama_audio_context.info_state == AMA_AM_STATE_STARTED)) {
                        ama_msg_item_t event;
                        event.data = NULL;
                        event.event_id = 0x00;

                        if (need_send_data == true) {
                            event.event_id = AMA_TASK_EVENT_AUDIO_SEND_DATA;
                        } else {
                            if (ama_audio_context.info_is_timer_start_ok == false) {
                                event.event_id = AMA_TASK_EVENT_AUDIO_STREAM_IN;
                            }
                        }
                        if (event.event_id != 0x00) {
                            ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &event);
                        }
                    }
                }
                break;

                case AMA_TASK_EVENT_AUDIO_SEND_DATA: {
                    ama_send_audio_data();

                    if (ama_audio_context.info_is_timer_start_ok == false) {
                        ama_msg_item_t ama_msg_item_event;
                        ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_STREAM_IN;
                        ama_msg_item_event.data = NULL;
                        ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
                    }
                }
                break;

                case AMA_TASK_EVENT_AUDIO_START: {
                    ama_audio_context.info_restart_mode = (wwe_mode_t)ama_msg_item_event.data;
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                    APPS_LOG_MSGID_E(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_START, requested : %d, started : %d",
                                    2,
                                    ama_audio_context.info_audio_source_requested,
                                    ama_audio_context.info_audio_source_transmitter_started);

                    if (ama_audio_context.info_audio_source_requested == true) {
                        if (ama_audio_context.info_audio_source_transmitter_started == true) {
                            ama_audio_recorder_start(ama_audio_context.info_restart_mode);
                            ama_audio_context.info_restart_mode = WWE_MODE_MAX;
                        } else {
                            app_ama_audio_execute_transmitter(true);
                        }
                    } else {
                        app_ama_audio_start_request_control(true);
                    }
#else
                    ama_audio_recorder_start((wwe_mode_t)ama_msg_item_event.data);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                }
                break;

                case AMA_TASK_EVENT_AUDIO_STOP: {
                    is_wwd_detected = false;
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                    app_ama_audio_start_request_control(false);
                    app_ama_audio_execute_transmitter(false);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                    ama_audio_recorder_stop();
                }
                break;

                case AMA_TASK_EVENT_AUDIO_START_DONE: {
                    if (AMA_AM_STATE_STARTING != ama_audio_context.info_state) {
                        APPS_LOG_MSGID_E(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_START_DONE, current is not starting state (Ignore) : %d",
                                    1, ama_audio_context.info_state);
                        break;
                    }

                    if (AUD_CMD_COMPLETE == (uint32_t)ama_msg_item_event.data) {
                        ama_audio_context.info_state = AMA_AM_STATE_STARTED;
                        APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_START_DONE, Start complete, Set state to be started", 0);
                    } else {
                        APPS_LOG_MSGID_E(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_START_DONE, Start failed, deinit", 0);
                        ama_audio_deinit();
                    }
                }
                break;

                case AMA_TASK_EVENT_AUDIO_ACTION_DONE: {
                    ama_wwe_audio_recorder_action_done((bt_sink_srv_am_cb_sub_msg_t)ama_msg_item_event.data);
                }
                break;

                case AMA_TASK_EVENT_AUDIO_STOP_BY_IND: {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_STOP_BY_IND, state : %d, suspended : %d",
                                2, ama_audio_context.info_state, ama_audio_context.info_is_suspended);
                    if ((AMA_AM_STATE_IDLE != ama_audio_context.info_state)
                            && (ama_audio_context.info_is_suspended == false)) {

                        ama_audio_context.info_is_suspended = true;
                        ama_audio_deinit();

                        if (ama_audio_context.info_recorder_stopped_callback != NULL) {
                            ama_audio_context.info_recorder_stopped_callback();
                        }
                    }
                }
                break;

                case AMA_TASK_EVENT_SIDE_TONE_STOPPED: {
#ifdef AIR_AMA_SIDETONE_ENABLE
                    /**
                     * @brief Handle the side tone has been stopped succeed.
                     * Need check whether need restart recorder or not.
                     */
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] SIDE_TONE_STOPPED, is_restart : %d, state : %d, side_tone_enabled : %d, side_tone_stopped : %d",
                                4, ama_audio_context.info_is_restart,
                                ama_audio_context.info_state,
                                ama_audio_context.info_side_tone_enabled,
                                ama_audio_context.info_side_tone_stopped);

                    if ((ama_audio_context.info_side_tone_enabled == true) && (ama_audio_context.info_side_tone_stopped == false)) {
                        ama_audio_context.info_side_tone_stopped = true;
                        ama_audio_context.info_side_tone_started = false;
                        ama_audio_context.info_side_tone_enabled = false;

#ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                        if ((ama_audio_context.info_is_restart == true)
                                && (ama_audio_context.info_state == AMA_AM_STATE_IDLE)) {
                            ama_audio_context.info_is_restart = false;
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
                            ama_audio_start(ama_audio_context.info_restart_mode, ama_audio_context.info_wwd_flash_address, ama_audio_context.info_wwd_length);
#else
                            if (ama_audio_context.info_restart_mode == WWE_MODE_NONE) {
                                ama_audio_start(ama_audio_context.info_restart_mode, 0, 0);
                            }
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
                        }
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                    }
#endif /* AIR_AMA_SIDETONE_ENABLE */
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                    bool need_restart = app_ama_audio_restart_checker();

                    if (need_restart == true) {
                        ama_audio_context.info_is_restart = false;
                        app_ama_audio_start_request_control(true);
                    }
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                }
                break;

                case AMA_TASK_EVENT_SIDE_TONE_STARTED: {
#ifdef AIR_AMA_SIDETONE_ENABLE
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] SIDE_TONE_STARTED, side_tone_enabled : %d",
                                1, ama_audio_context.info_side_tone_enabled);
                    if (ama_audio_context.info_side_tone_enabled == true) {
                        ama_audio_context.info_side_tone_started = true;
                    }
#endif /* AIR_AMA_SIDETONE_ENABLE */
                }
                break;

                case AMA_TASK_EVENT_SOURCE_CONTROL_RESULT: {
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                    app_ama_audio_handle_control_request_result((uint16_t)ama_msg_item_event.data);
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
                }
                break;

                case AMA_TASK_EVENT_AUDIO_RELEASED: {
                    APPS_LOG_MSGID_I(APP_AMA_AUDIO", [AUDIO_TASK] AMA_TASK_EVENT_AUDIO_RELEASED, state : %d, suspended : %d",
                                        2, ama_audio_context.info_state, ama_audio_context.info_is_suspended);
                    if ((AMA_AM_STATE_IDLE == ama_audio_context.info_state)
                            &&(ama_audio_context.info_is_suspended == true)) {

                        ama_audio_context.info_is_suspended = false;
                        if (ama_audio_context.info_recorder_released_callback != NULL) {
                            ama_audio_context.info_recorder_released_callback();
                        }
                    }
                }
                break;

                default:
                    break;
            }
        }
    }
}

static void ama_audio_control_ccni_callback(hal_audio_event_t event, void *data)
{
    ama_msg_item_t ama_msg_item_event;

    // if (ama_audio_context.info_state != AMA_AM_STATE_STARTED) {
    //     APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][RECORDER][CALLBACK] event : %d, recorder not started state (%d), ignore",
    //                             2, event, ama_audio_context.info_state);
    //     return;
    // }

    switch (event) {
        case HAL_AUDIO_EVENT_WWD_NOTIFICATION: {
            ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_WWD_NOTIFICATION;
            ama_msg_item_event.data = (uint8_t *)data;
            ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][RECORDER][CALLBACK] HAL_AUDIO_EVENT_WWD_NOTIFICATION", 0);
        }
        break;

        case HAL_AUDIO_EVENT_DATA_ABORT_NOTIFICATION: {
            ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_DATA_ABORT_NOTIFICATION;
            ama_msg_item_event.data = NULL;
            ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", [WWD][RECORDER][CALLBACK] HAL_AUDIO_EVENT_DATA_ABORT_NOTIFICATION", 0);
        }
        break;

        case HAL_AUDIO_EVENT_DATA_NOTIFICATION: {
            uint32_t dsp_pcm_size = audio_record_control_get_share_buf_data_byte_count();
            uint32_t opus_frame_num = dsp_pcm_size / ama_audio_context.info_pcm_length_in_one_frame;

            if (ama_audio_context.info_is_data_notified == false && opus_frame_num >= 2) {
                ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_STREAM_IN;
                ama_msg_item_event.data = NULL;
                ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);

                ama_audio_context.info_is_data_notified = true;
            }
        }
        break;

        case HAL_AUDIO_EVENT_ERROR:
            break;

        default:
            break;
    }
}

/**
 * @brief The audio codec manager callback handler
 *
 * @param handle
 * @param event
 * @param user_data
 */
static void ama_audio_codec_manager_callback(audio_codec_manager_handle_t handle, audio_codec_manager_event_t event, void *user_data)
{
    if (handle != ama_audio_context.info_codec_manager_handle) {
        return;
    }

    switch (event) {
        case AUDIO_CODEC_MANAGER_EVENT_OUTPUT_BUFFER_FULL: {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_codec_manager_callback, Codec buffer full", 0);
            if (ama_audio_context.info_sending_data == false) {
                uint32_t total_len = audio_codec_buffer_mode_get_output_data_length(handle);
                audio_codec_buffer_mode_read_done(handle, total_len);
            } else {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_codec_manager_callback, Current is sending data (ERROR), maybe error happen", 0);
            }
        }
    }
}

static void ama_audio_am_callback(bt_sink_srv_am_id_t aud_id,
                                    bt_sink_srv_am_cb_msg_class_t msg_id,
                                    bt_sink_srv_am_cb_sub_msg_t sub_msg,
                                    void *parm)
{
    ama_msg_item_t ama_msg_item_event;

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_am_callback id : %d, msg_id : %d, sub_msg : %d, state : %d", 4, aud_id, msg_id, sub_msg, ama_audio_context.info_state);

    switch (msg_id) {
        case AUD_SINK_OPEN_CODEC: {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_am_callback, Recorder open done", 0);
            ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_START_DONE;
            ama_msg_item_event.data = (uint8_t *)sub_msg;
            ama_audio_queue_send_front((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
            break;
        }

        case AUD_SELF_CMD_REQ: {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_am_callback, Recorder audio action done", 0);
            ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_ACTION_DONE;
            ama_msg_item_event.data = (uint8_t *)sub_msg;
            ama_audio_queue_send_front((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
            break;
        }

        case AUD_SUSPEND_BY_IND: {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_am_callback, Recorder suspended by 0x%x", 1, sub_msg);
            if ((AUD_SUSPEND_BY_HFP == sub_msg)
                    || (AUD_SUSPEND_BY_LC == sub_msg)
                    || (AUD_SUSPEND_BY_LE_CALL == sub_msg)) {
                if (AUD_SUSPEND_BY_LC == sub_msg) {
                    ama_audio_context.info_am_stop_by_ld = true;
                }
                ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_STOP_BY_IND;
                ama_audio_queue_send_front((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
            }
            break;
        }

        default:
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", default", 0);
            break;
    }
}

void ama_audio_set_wwd_trigger_callback(on_wwd_trigger trigger_callback)
{
    if (trigger_callback != NULL) {
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
        ama_audio_context.info_wwd_start_speech_callback = trigger_callback;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
    }
}

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
static void app_ama_audio_am_vendor_se_callback_handler(vendor_se_event_t event, void *arg);

void app_ama_audio_mic_resource_request_callback_handler(audio_source_control_event_type type, uint32_t sub_event, void* usr_data)
{
    if ((type != AUDIO_SOURCE_CONTROL_EVENT_RES_CTRL)
            && (type != AUDIO_SOURCE_CONTROL_EVENT_VENDER_SE)
            && (type != AUDIO_SOURCE_CONTROL_EVENT_TRANSMITTER)) {
        APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_mic_resource_request_callback_handler, response type error : %d - %d", 2, type, sub_event);
        return;
    }

    if (type == AUDIO_SOURCE_CONTROL_EVENT_VENDER_SE) {
        app_ama_audio_am_vendor_se_callback_handler(sub_event, NULL);
    } else {
        uint16_t event_data = 0x00;
        ama_msg_item_t ama_msg_item_event;
        ama_msg_item_event.event_id = AMA_TASK_EVENT_SOURCE_CONTROL_RESULT;
        event_data = ((type << 8) & 0xFF00) | (sub_event & 0x00FF);
        ama_msg_item_event.data = (uint16_t *)event_data;
        ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
    }
#if 0
    if (type == AUDIO_SOURCE_CONTROL_EVENT_VENDER_SE) {
        app_ama_audio_am_vendor_se_callback_handler(sub_event, NULL);
    } if (type == AUDIO_SOURCE_CONTROL_EVENT_RES_CTRL) {

        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_mic_resource_request_callback_handler, resource control result : %d", 1, sub_event);

        if ((sub_event == AUDIO_SRC_SRV_EVENT_TAKE_SUCCESS)
                || (sub_event == (uint8_t)AUDIO_SRC_SRV_EVENT_TAKE_ALREADY)) {
            if (ama_audio_context.info_audio_source_suspend == false) {
                audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                            AUDIO_SOURCE_CONTROL_CMD_START_TRANSMITTER,
                                            AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
            }
        } else if ((sub_event == AUDIO_SRC_SRV_EVENT_SUSPEND)
                        || (sub_event == AUDIO_SRC_SRV_EVENT_TAKE_REJECT)) {
            if (sub_event == AUDIO_SRC_SRV_EVENT_SUSPEND) {
                APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, MIC has been suspended, current state : %d", 1, ama_audio_context.info_state);

                audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                            AUDIO_SOURCE_CONTROL_CMD_RELEASE_RES,
                                            AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
            } else {
                APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, take resource failed", 0);
            }
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
            audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                        AUDIO_SOURCE_CONTROL_CMD_ADD_WAITTING_LIST,
                                        AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
        } else if (sub_event == AUDIO_SRC_SRV_EVENT_GIVE_SUCCESS) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, give resource succeed", 0);
            ama_msg_item_event.data = (uint8_t *)sub_event;
            ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
        } else if (sub_event == AUDIO_SRC_SRV_EVENT_GIVE_ERROR) {
            APPS_LOG_MSGID_E(APP_AMA_AUDIO", app_ama_audio_handle_control_request_result, give resource failed", 0);
        }
    } else {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_mic_resource_request_callback_handler, transmitter event : %d", 1, sub_event);
        if (sub_event == AUDIO_TRANSMITTER_EVENT_START_SUCCESS) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_mic_resource_request_callback_handler, start transmitter succeed", 0);
        } else {
            if (sub_event == AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS) {
                APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_mic_resource_request_callback_handler, stop transmitter succeed", 0);
                audio_source_control_cmd(ama_audio_context.info_audio_source_control_handle,
                                            AUDIO_SOURCE_CONTROL_CMD_RELEASE_RES,
                                            AUDIO_SOURCE_CONTROL_CMD_DEST_REMOTE);
            }
        }
    }
#endif
}
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

void app_ama_audio_am_vendor_se_callback_handler(vendor_se_event_t event, void *arg)
{
    // APPS_LOG_MSGID_I(APP_AMA_AUDIO", am_vendor_se_callback, event : %d, stopped by LD ? %d",
    //                 2, event, ama_audio_context.info_am_stop_by_ld);

    ama_msg_item_t ama_msg_item_event;
    ama_msg_item_event.event_id = 0x00;
    ama_msg_item_event.data = NULL;

    if (event == EVENT_RECORD_STOP && ama_audio_context.info_am_stop_by_ld == true) {
        ama_audio_context.info_am_stop_by_ld = false;
        ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_RELEASED;
    }

#ifdef AIR_AMA_SIDETONE_ENABLE
    if (event == EVENT_SIDETONE_STOP) {
        ama_msg_item_event.event_id = AMA_TASK_EVENT_SIDE_TONE_STOPPED;
    }

    if (event == EVENT_SIDETONE_START) {
        ama_msg_item_event.event_id = AMA_TASK_EVENT_SIDE_TONE_STARTED;
    }
#endif /* AIR_AMA_SIDETONE_ENABLE */

    if ((event == EVENT_BLE_STOP)
            || (event == EVENT_HFP_STOP)) {
        ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_RELEASED;
    }

    if (event == EVENT_BLE_START) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", app_ama_audio_am_vendor_se_callback_handler, BLE start event, stop recorder and notify stopped", 0);
        ama_audio_context.info_is_suspended = true;
        ama_audio_context.info_is_restart = false;
        ama_audio_stop();
        if (ama_audio_context.info_recorder_stopped_callback != NULL) {
            ama_audio_context.info_recorder_stopped_callback();
        }
        return;
    }

    if (ama_msg_item_event.event_id != 0x00) {
        ama_audio_queue_send_front((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event);
    }
}

void ama_audio_init(on_recorder_stop_record callback, on_recorder_released released_callback)
{
    static bool is_init_done = false;

    /**
     * @brief Fix issue that API re-enter which will make the register error happen.
     * For vendor se callback, just need register once.
     */
    if (is_init_done == false) {
        ama_audio_context.info_recorder_stopped_callback = callback;
        ama_audio_context.info_recorder_released_callback = released_callback;

        ama_audio_context.info_am_se_id = ami_get_vendor_se_id();
        bt_sink_srv_am_result_t am_se_result = ami_register_vendor_se(ama_audio_context.info_am_se_id, app_ama_audio_am_vendor_se_callback_handler);
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init, am_se_id : %d, am_se_result : %d", 2, ama_audio_context.info_am_se_id, am_se_result);

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
        audio_source_control_cfg_t ama_config = {
            .res_type = AUDIO_SRC_SRV_RESOURCE_TYPE_MIC,
            .req_priority = AUDIO_SRC_SRV_RESOURCE_TYPE_MIC_USER_VA_PRIORITY,
            .usr = AUDIO_SOURCE_CONTROL_USR_AMA,
            .usr_name = "AMA_SOURCE",
            .usr_data = NULL,
            .request_notify = app_ama_audio_mic_resource_request_callback_handler,
        };
        ama_audio_context.info_audio_source_control_handle = audio_source_control_register(&ama_config);

        if (ama_audio_context.info_audio_source_control_handle == NULL) {
            APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init, register MIC audio source control failed", 0);
            assert(false);
        }
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

        is_init_done = true;
    } else {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_init, already init se callback done, id : %d", 1, ama_audio_context.info_am_se_id);
    }
}

void ama_audio_start(wwe_mode_t wwe_mode, uint32_t wwe_flash_address, uint32_t wwe_length)
{
    if (wwe_mode == WWE_MODE_AMA && (wwe_length == 0 || wwe_flash_address == 0)) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_start, mode is AMA, but length/address is 0, address : 0x%08x, length : 0x%08x", 2, wwe_flash_address, wwe_length);
        return;
    }

    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_start ENTER, wwe_mode : %d, flash_address : 0x%08x, length : 0x%08x",
                        3, wwe_mode, wwe_flash_address, wwe_length);

    ama_msg_item_t ama_msg_item_event;
    ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_START;
    ama_msg_item_event.data = (uint8_t *)wwe_mode;

    if (wwe_mode == WWE_MODE_AMA) {
#ifdef AMA_TRIGGER_MODE_WWD_ENABLE
        ama_audio_context.info_wwd_flash_address = wwe_flash_address;
        ama_audio_context.info_wwd_length = wwe_length;
#endif /* AMA_TRIGGER_MODE_WWD_ENABLE */
    }

    if (ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event) != 0) {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_start, start failed <queue send failed>; this should not happen, ID : %d", 1, ama_msg_item_event.event_id);
    }
}

void ama_audio_stop(void)
{
    ama_msg_item_t ama_msg_item_event;
    ama_msg_item_event.event_id = AMA_TASK_EVENT_AUDIO_STOP;
    ama_msg_item_event.data = NULL;
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_stop ENTER", 0);
    if (ama_audio_queue_send((QueueHandle_t)ama_audio_task_queue, &ama_msg_item_event) == 0) {
        //ama_wwe_running = false;
    } else {
        APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_stop, stop failed <queue send failed>, need stop forcely", 0);
        ama_audio_context.info_need_force_stop_recorder = true;
    }
}

bool ama_audio_is_busy()
{
    APPS_LOG_MSGID_I(APP_AMA_AUDIO", ama_audio_is_busy, Current sending data : %d", 1, ama_audio_context.info_sending_data);
    if (ama_audio_context.info_sending_data == true) {
        return true;
    }
    return false;
}

bool ama_audio_is_stopped_by_ld()
{
    return ama_audio_context.info_am_stop_by_ld;
}

bool ama_audio_is_suspended()
{
    if (
        (ama_audio_context.info_am_stop_by_ld == true)
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
        || (ama_audio_context.info_audio_source_suspend == true)
#endif /* AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
        || (ama_audio_context.info_is_suspended == true)
    ) {
        return true;
    }
    return false;
}

#endif

