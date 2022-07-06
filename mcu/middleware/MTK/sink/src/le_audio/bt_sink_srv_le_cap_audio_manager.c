/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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




#include "FreeRTOS.h"
#include "bt_type.h"
#include "bt_le_audio_sink.h"
#include "bt_sink_srv_utils.h"
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "bt_sink_srv_le_cap_stream.h"
#include "bt_sink_srv_le_volume.h"
#include "ble_ascs.h"
#include "bt_sink_srv_ami.h"
#include "audio_src_srv.h"
#if defined(MTK_AVM_DIRECT)
#include "bt_avm.h"
#endif
#include "bt_le_audio_msglog.h"


/**************************************************************************************************
* Define
**************************************************************************************************/

#if defined(__AFE_HS_DC_CALIBRATION__)
    #define CAP_AUDIO_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HANDSET
#else
    #define CAP_AUDIO_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HEADSET
#endif
#ifdef MTK_DEVELOPMENT_BOARD_HDK
    #define CAP_AUDIO_INPUT_DEVICE HAL_AUDIO_DEVICE_MAIN_MIC
#else
    #define CAP_AUDIO_INPUT_DEVICE HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC
#endif

#define CAP_INVALID_UINT8   0xFF
#define BROADCAST_MUSIC_RESUME   0

/**************************************************************************************************
* Enum
**************************************************************************************************/
enum
{
    CAP_AM_STATE_OFF,
    CAP_AM_STATE_IDLE,
    CAP_AM_STATE_PREPARE_PLAY,
    CAP_AM_STATE_WAIT_PLAY,
    CAP_AM_STATE_PLAYING,
    CAP_AM_STATE_SUSPENDING,
    CAP_AM_STATE_PREPARE_STOP,
    CAP_AM_STATE_WAIT_STOP,
};

/**************************************************************************************************
* Structure
**************************************************************************************************/
typedef struct
{
    uint8_t state;
    bt_sink_srv_am_id_t cap_aid;
    audio_src_srv_handle_t *p_handle;
    bool stop_needed;
    bool deinit_needed;
    uint8_t current_mode;
    uint8_t sub_state;
}cap_audio_manager_control_t;

/**************************************************************************************************
* Variables
**************************************************************************************************/
static cap_audio_manager_control_t g_cap_am_ctl[CAP_MAX_DEVICE_NUM];
static bt_sink_srv_cap_am_mode g_cap_am_current_mode = CAP_INVALID_UINT8;
static bt_sink_srv_cap_am_mode g_cap_am_reject_mode = CAP_INVALID_UINT8;
static TimerHandle_t g_cap_am_reject_timer = NULL;
static bt_sink_srv_cap_am_mode g_cap_am_restarting_mode = CAP_INVALID_UINT8;

/**************************************************************************************************
* Prototype
**************************************************************************************************/
static void bt_sink_srv_cap_am_para_init(void);
static void bt_sink_srv_cap_am_function_init(void);
static void bt_sink_srv_cap_am_callback_register(void);
static void bt_sink_srv_cap_am_callback_deregister(bt_sink_srv_cap_am_mode mode);
static void bt_sink_srv_cap_am_action_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param);
static void bt_sink_srv_cap_am_play_callback(audio_src_srv_handle_t *handle);
static void bt_sink_srv_cap_am_stop_callback(audio_src_srv_handle_t *handle);
static void bt_sink_srv_cap_am_suspend_callback(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd);
static void bt_sink_srv_cap_am_reject_callback(audio_src_srv_handle_t *handle);
static void bt_sink_srv_cap_am_exception_callback(audio_src_srv_handle_t *handle, int32_t event, void *param);
static void bt_sink_srv_cap_am_send_event_and_set_state(bt_sink_srv_cap_am_mode mode, audio_src_srv_event_t event_id);
static void bt_sink_srv_cap_am_set_state(bt_sink_srv_cap_am_mode mode, uint8_t state);
static uint8_t bt_sink_srv_cap_am_get_state(bt_sink_srv_cap_am_mode mode);
static bt_sink_srv_cap_am_mode bt_sink_srv_cap_am_get_mode_by_audio_id(bt_sink_srv_am_id_t aud_id);
static bt_sink_srv_cap_am_mode bt_sink_srv_cap_am_get_mode_by_audio_handle(audio_src_srv_handle_t *p_handle);
static bool bt_sink_srv_cap_am_reject_timer_start(bt_sink_srv_cap_am_mode mode);
static void bt_sink_srv_cap_am_reject_timer_callback(TimerHandle_t timer_id);
static bool bt_sink_srv_cap_am_reject_timer_stop(void);


/**************************************************************************************************
* Static Functions
**************************************************************************************************/
static void bt_sink_srv_cap_am_para_init(void)
{
#ifdef AIR_BT_CODEC_BLE_ENABLED
    memset(&g_cap_am_ctl, 0, sizeof(g_cap_am_ctl));
    for (uint8_t i = 0; i < CAP_AM_MODE_NUM; i++) {
        g_cap_am_ctl[i].cap_aid = AUD_ID_INVALID;
        g_cap_am_ctl[i].p_handle = audio_src_srv_construct_handle(AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE);
        g_cap_am_ctl[i].p_handle->type = AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE;
        g_cap_am_ctl[i].p_handle->dev_id = 0x0;
        g_cap_am_ctl[i].p_handle->priority = ((i <= CAP_AM_UNICAST_CALL_MODE_1) ? AUDIO_SRC_SRV_PRIORITY_HIGH : AUDIO_SRC_SRV_PRIORITY_NORMAL);
    }
#endif
}

static void bt_sink_srv_cap_am_function_init(void)
{
    for (uint8_t i = 0; i < CAP_AM_MODE_NUM; i++) {
        if (!g_cap_am_ctl[i].p_handle) {
            le_audio_log("[CAP][AM] function init FAIL, mode:%d", 1, i);
            configASSERT(0);
            return;
        }

        g_cap_am_ctl[i].p_handle->play             = bt_sink_srv_cap_am_play_callback;
        g_cap_am_ctl[i].p_handle->stop             = bt_sink_srv_cap_am_stop_callback;
        g_cap_am_ctl[i].p_handle->suspend          = bt_sink_srv_cap_am_suspend_callback;
        g_cap_am_ctl[i].p_handle->reject           = bt_sink_srv_cap_am_reject_callback;
        g_cap_am_ctl[i].p_handle->exception_handle = bt_sink_srv_cap_am_exception_callback;
    }
}

static void bt_sink_srv_cap_am_callback_register(void)
{
    for (uint8_t i = 0; i < CAP_AM_MODE_NUM; i++) {
        if(g_cap_am_ctl[i].cap_aid != AUD_ID_INVALID)
        {
            bt_sink_srv_ami_audio_close(g_cap_am_ctl[i].cap_aid);
        }

        g_cap_am_ctl[i].cap_aid = bt_sink_srv_ami_audio_open(((i <= CAP_AM_UNICAST_CALL_MODE_1) ? AUD_HIGH : AUD_MIDDLE), bt_sink_srv_cap_am_action_callback);
        le_audio_log("[CAP][AM] register callback, index:%d, aid:%d", 2, i, g_cap_am_ctl[i].cap_aid);


        bt_sink_srv_cap_am_send_event_and_set_state(i, AUDIO_SRC_SRV_EVT_READY);
    }
}

static void bt_sink_srv_cap_am_callback_deregister(bt_sink_srv_cap_am_mode mode)
{
    if (g_cap_am_ctl[mode].cap_aid != AUD_ID_INVALID) {
        bt_sink_srv_ami_audio_close(g_cap_am_ctl[mode].cap_aid);
        g_cap_am_ctl[mode].cap_aid = AUD_ID_INVALID;
    }
    audio_src_srv_destruct_handle(g_cap_am_ctl[mode].p_handle);
    le_audio_log("[CAP][AM] deregister callback, mode:%d", 1, mode);
}

static void bt_sink_srv_cap_am_action_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *param)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_mode_by_audio_id(aud_id);
    uint8_t state = bt_sink_srv_cap_am_get_state(mode);
    bt_handle_t conn_handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);

    le_audio_log("[CAP][AM] action callback, mode:%d, msg:%d, sub_msg:%d, state:%d, need stop:%d, need deinit:%d", 6,
        mode, msg_id, sub_msg, state, g_cap_am_ctl[mode].stop_needed, g_cap_am_ctl[mode].deinit_needed);

    bt_sink_srv_mutex_lock();

    switch(msg_id)
    {
        case AUD_SINK_OPEN_CODEC: //DSP on
            le_audio_log("[CAP][AM] AUD_SINK_OPEN_CODEC", 0);

            if (state == CAP_AM_STATE_WAIT_PLAY) {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
                if (mode <= CAP_AM_UNICAST_MUSIC_MODE_1) {
                    /*Unicast*/
                    if (sub_msg == AUD_CMD_FAILURE || g_cap_am_ctl[mode].stop_needed || g_cap_am_ctl[mode].deinit_needed) {
                        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
                        le_audio_log("[CAP][AM] enabling_response first ase_id", 0);
                        bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SINK);

                        if(le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET) {
                            //respond 2nd ASE Characteristic
                            le_audio_log("[CAP][AM] enabling_response second ase_id", 0);
                            bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SINK);
                        }

                        if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                            //respond both ASE Characteristic
                            le_audio_log("[CAP][AM] enabling_response source ase_id", 0);
                            bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SOURCE);
                        }

                    } else if (sub_msg == AUD_HFP_OPEN_CODEC_DONE) {
                            //bt_clock_t play_clk = {0};

                        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PLAYING);

						bt_sink_srv_le_volume_state_t volume_state = bt_sink_srv_le_volume_get_state(conn_handle);
                        bt_sink_srv_ami_audio_set_mute(aud_id, (volume_state.mute ? true : false), STREAM_OUT);

                        bt_sink_srv_cap_stream_set_service_ble_link(conn_handle);
                        g_cap_am_current_mode = mode;

                        if (mode == g_cap_am_restarting_mode) {
                            /* Restart AM success, no need to send ASE notification, just set data path */
                            bt_sink_srv_cap_stream_restarting_complete_response(mode);
                            g_cap_am_restarting_mode = CAP_INVALID_UINT8;
                            break;
                        }

                        //bt_sink_srv_cap_stream_am_timer_start();
                        le_audio_log("[CAP][AM] enabling_response first ase_id", 0);
                        bt_sink_srv_cap_stream_enabling_response(conn_handle, true, AUDIO_DIRECTION_SINK);

                        if(le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET) {
                            //respond 2nd ASE Characteristic
                            le_audio_log("[CAP][AM] enabling_response second ase_id", 0);
                            bt_sink_srv_cap_stream_enabling_response(conn_handle, true, AUDIO_DIRECTION_SINK);
                        }

                        if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                            //respond both ASE Characteristic
                            le_audio_log("[CAP][AM] enabling_response source ase_id", 0);
                            am_audio_side_tone_enable();
                            bt_sink_srv_cap_stream_enabling_response(conn_handle, true, AUDIO_DIRECTION_SOURCE);
                        }
                        bt_sink_srv_cap_stream_set_all_cis_data_path(conn_handle);
                    } else {
                            configASSERT(0);
                    }
                }
#endif

                if (mode == CAP_AM_BROADCAST_MUSIC_MODE) {
                    /*Broadcast*/
                    if (sub_msg == AUD_CMD_FAILURE || g_cap_am_ctl[mode].stop_needed || g_cap_am_ctl[mode].deinit_needed) {
                        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
                        bt_sink_srv_cap_stream_broadcast_enabling_response(false);

                    } else if (sub_msg != AUD_CMD_FAILURE) {
                        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PLAYING);
                        bt_sink_srv_ami_audio_set_mute(aud_id, (bt_sink_srv_cap_stream_is_broadcast_mute() ? true : false), STREAM_OUT);
                        g_cap_am_current_mode = mode;

                        if (mode == g_cap_am_restarting_mode) {
                            /* Restart AM success, no need to create BIG sync, just set data path */
                            bt_sink_srv_cap_stream_restarting_complete_response(mode);
                            g_cap_am_restarting_mode = CAP_INVALID_UINT8;
                            break;
                        }
                        bt_sink_srv_cap_stream_broadcast_enabling_response(true);
                    }
                }
            }
            break;

        case AUD_SELF_CMD_REQ: //DSP off
            le_audio_log("[CAP][AM] AUD_SELF_CMD_REQ", 0);
            bt_sink_srv_ami_audio_set_mute(aud_id, false, STREAM_OUT);

            if (state == CAP_AM_STATE_WAIT_STOP) {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
                if (mode <= CAP_AM_UNICAST_MUSIC_MODE_1) {
                    /*Unicast*/
                    if (sub_msg == AUD_CMD_COMPLETE) {
                        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_READY);
                        bt_sink_srv_cap_stream_clear_service_ble_link(conn_handle);
                        g_cap_am_current_mode = CAP_INVALID_UINT8;


                        if (g_cap_am_ctl[mode].deinit_needed) {
                            g_cap_am_ctl[mode].deinit_needed = false;
                            bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
                            bt_sink_srv_cap_am_callback_deregister(mode);
                        } else if (mode == g_cap_am_restarting_mode) {
                            /* Restart AM without sending ASE notification */
                            bt_sink_srv_cap_am_audio_start(mode);
                            break;
                        }

                        bt_sink_srv_cap_stream_disabling_response_all(conn_handle, true, AUDIO_DIRECTION_SINK);
                        if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                            bt_sink_srv_cap_stream_disabling_response(conn_handle, true, AUDIO_DIRECTION_SOURCE);
                        }

                    } else {
                        bt_sink_srv_cap_stream_disabling_response_all(conn_handle, false, AUDIO_DIRECTION_SINK);

                        if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                            bt_sink_srv_cap_stream_disabling_response(conn_handle, false, AUDIO_DIRECTION_SOURCE);
                        }
                    }
                }
#endif

                if (mode == CAP_AM_BROADCAST_MUSIC_MODE) {
                /*Broadcast*/
                    bt_sink_srv_cap_stream_service_big_t *service_big = bt_sink_srv_cap_stream_get_service_big();
                    bt_sink_srv_ami_audio_set_mute(aud_id, false, STREAM_OUT);
                    bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_READY);
                    g_cap_am_current_mode = CAP_INVALID_UINT8;

                    if (g_cap_am_ctl[mode].deinit_needed) {
                        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
                        bt_sink_srv_cap_am_callback_deregister(mode);
                        g_cap_am_ctl[mode].deinit_needed = false;
                    } else if (mode == g_cap_am_restarting_mode) {
                        /* Restart AM without sending ASE notification */
                        bt_sink_srv_cap_am_audio_start(mode);
                        break;
                    }

                    if ((NULL != service_big) && (service_big->big_handle)) {
                        bt_sink_srv_cap_stream_broadcast_disabling_response(true);
                        bt_sink_srv_cap_stream_clear_service_big(service_big->big_handle);
                    }
                }

            } else if (state == CAP_AM_STATE_SUSPENDING && sub_msg == AUD_CMD_COMPLETE) {

                bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_READY);
                audio_src_srv_add_waiting_list(g_cap_am_ctl[mode].p_handle);
                g_cap_am_current_mode = CAP_INVALID_UINT8;

#ifdef AIR_LE_AUDIO_CIS_ENABLE
                if (mode <= CAP_AM_UNICAST_MUSIC_MODE_1 && conn_handle != BT_HANDLE_INVALID) {
                    bt_sink_srv_cap_stream_disabling_response_all(conn_handle, true, AUDIO_DIRECTION_SINK);

                    if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                        bt_sink_srv_cap_stream_disabling_response(conn_handle, true, AUDIO_DIRECTION_SOURCE);
                    }
                    //bt_sink_srv_cap_stream_clear_service_ble_link(conn_handle);
                }
#endif

                if (mode == CAP_AM_BROADCAST_MUSIC_MODE) {
                    bt_sink_srv_cap_stream_service_big_t *service_big = bt_sink_srv_cap_stream_get_service_big();
                    if ((NULL != service_big) && (service_big->big_handle)) {
                        bt_sink_srv_cap_stream_broadcast_disabling_response(true);
                        bt_sink_srv_cap_stream_clear_service_big(service_big->big_handle);
                    }
                }

            } else if(state == CAP_AM_STATE_WAIT_PLAY && sub_msg == AUD_CMD_COMPLETE) {
                bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_READY);
                g_cap_am_current_mode = CAP_INVALID_UINT8;
                bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
                bt_sink_srv_cap_am_callback_deregister(mode);
            }
            break;

        default:
            break;
    }

    if (mode < CAP_AM_MODE_NUM) {
        g_cap_am_ctl[mode].stop_needed = false;
    }
    bt_sink_srv_mutex_unlock();
}

static void bt_sink_srv_cap_am_play_callback(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_mode_by_audio_handle(handle);
    uint8_t state = bt_sink_srv_cap_am_get_state(mode);
    bt_handle_t conn_handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);

    le_audio_log("[CAP][AM] play callback, mode:%d, state:%d", 3, mode, state);
#ifdef AIR_BT_CODEC_BLE_ENABLED
    if (state == CAP_AM_STATE_SUSPENDING) {
        le_audio_log("[CAP][AM] resume", 0);
        bt_sink_srv_cap_am_set_state(mode, CAP_AM_STATE_PREPARE_STOP);
        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);

        switch (mode) {
            case CAP_AM_UNICAST_MUSIC_MODE_0:
            case CAP_AM_UNICAST_MUSIC_MODE_1: {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
                /*TO DO MCP PLAY*/
                bt_le_audio_sink_action_param_t le_param = {
                    .service_idx = BLE_MCP_GMCS_INDEX,
                };
                bt_le_audio_sink_send_action(conn_handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PLAY, &le_param);
                
                bt_sink_srv_cap_event_media_change_state_t media_state;
                media_state.connect_handle = conn_handle;
                media_state.resume = true;
                le_audio_sink_inform_app(BT_LE_AUDIO_SINK_EVENT_MEDIA_RESUME, &conn_handle);
#endif
                break;
            }
            case CAP_AM_BROADCAST_MUSIC_MODE: {
#if (BROADCAST_MUSIC_RESUME)
                bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
                scan_param.duration = DEFAULT_SCAN_TIMEOUT;
                bt_sink_srv_cap_stream_get_default_bmr_scan_info(&scan_param);
                bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
                break;
#endif
            }

            default:
                break;
        }
        return;
    }

    if(handle->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE && state == CAP_AM_STATE_PREPARE_PLAY)
    {
        bt_sink_srv_cap_am_reject_timer_stop();
        uint8_t codec_id[AUDIO_CODEC_ID_SIZE] = CODEC_ID_LC3;

        bt_sink_srv_am_audio_capability_t aud_cap = {0};
        bt_sink_srv_am_result_t am_result;
        bt_sink_srv_cap_stream_config_info_t config_info = bt_sink_srv_cap_stream_get_config_info(mode, AUDIO_DIRECTION_SINK);
        bt_sink_srv_cap_stream_config_info_t ul_config = {{0}};
        uint32_t context_type = 0;
        le_audio_log("[CAP][AM] config_info [sdu_interval %x %x %x]", 3, config_info.sdu_interval[0], config_info.sdu_interval[1], config_info.sdu_interval[2]);

        if ((config_info.metadata_length == 4) && (NULL != config_info.metadata)) {
            context_type = (config_info.metadata[2])|(config_info.metadata[3] << 8);
        }
        aud_cap.codec.ble_format.ble_codec.context_type = context_type;
        le_audio_log("[CAP][AM] context_type:%d", 1, context_type);

        bt_sink_srv_le_volume_state_t volume_state = {0};

        if (mode == CAP_AM_BROADCAST_MUSIC_MODE) {
            volume_state.volume = bt_sink_srv_cap_stream_get_broadcast_volume();
            volume_state.mute = bt_sink_srv_cap_stream_is_broadcast_mute();
        } else {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
            volume_state = bt_sink_srv_le_volume_get_state(conn_handle);
#endif

            if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                /*Call mode, get configuration for UL*/
                ul_config = bt_sink_srv_cap_stream_get_config_info(mode, AUDIO_DIRECTION_SOURCE);
                uint32_t ul_context_type = 0;

                switch (ul_config.sampling_frequency) {
                case CODEC_CONFIGURATION_SAMPLING_FREQ_8KHZ:
                    //aud_cap.codec.ble_format.ble_codec.sample_rate = SAMPLING_RATE_8K;
                    break;

                case CODEC_CONFIGURATION_SAMPLING_FREQ_16KHZ:
                        aud_cap.codec.ble_format.ble_codec.ul_sample_rate = SAMPLING_RATE_16K;
                    break;

                case CODEC_CONFIGURATION_SAMPLING_FREQ_24KHZ:
                        aud_cap.codec.ble_format.ble_codec.ul_sample_rate = SAMPLING_RATE_24K;
                    break;

                case CODEC_CONFIGURATION_SAMPLING_FREQ_32KHZ:
                        aud_cap.codec.ble_format.ble_codec.ul_sample_rate = SAMPLING_RATE_32K;
                    break;

                case CODEC_CONFIGURATION_SAMPLING_FREQ_44_1KHZ:
                        aud_cap.codec.ble_format.ble_codec.ul_sample_rate = SAMPLING_RATE_44_1K;
                    break;

                case CODEC_CONFIGURATION_SAMPLING_FREQ_48KHZ:
                        aud_cap.codec.ble_format.ble_codec.ul_sample_rate = SAMPLING_RATE_48K;
                    break;

                default:
                    break;
            }
                aud_cap.codec.ble_format.ble_codec.ul_channel_num = CHANNEL_MONO;
                aud_cap.codec.ble_format.ble_codec.ul_frame_payload_length = ul_config.frame_payload_length;
                if ((ul_config.metadata_length == 4) && (NULL != ul_config.metadata)) {
                    ul_context_type = (ul_config.metadata[2])|(ul_config.metadata[3] << 8);
                }
                aud_cap.codec.ble_format.ble_codec.ul_context_type = ul_context_type;
                le_audio_log("[CAP][AM] ul_context_type:%d", 1, ul_context_type);
            }
        }

        switch (config_info.sampling_frequency) {
                    case CODEC_CONFIGURATION_SAMPLING_FREQ_8KHZ:
                        //aud_cap.codec.ble_format.ble_codec.sample_rate = SAMPLING_RATE_8K;
                        break;

                    case CODEC_CONFIGURATION_SAMPLING_FREQ_16KHZ:
                aud_cap.codec.ble_format.ble_codec.sample_rate = SAMPLING_RATE_16K;
                        break;

                    case CODEC_CONFIGURATION_SAMPLING_FREQ_24KHZ:
                aud_cap.codec.ble_format.ble_codec.sample_rate = SAMPLING_RATE_24K;
                        break;

                    case CODEC_CONFIGURATION_SAMPLING_FREQ_32KHZ:
                aud_cap.codec.ble_format.ble_codec.sample_rate = SAMPLING_RATE_32K;
                        break;

                    case CODEC_CONFIGURATION_SAMPLING_FREQ_44_1KHZ:
                aud_cap.codec.ble_format.ble_codec.sample_rate = SAMPLING_RATE_44_1K;
                        break;

                    case CODEC_CONFIGURATION_SAMPLING_FREQ_48KHZ:
                aud_cap.codec.ble_format.ble_codec.sample_rate = SAMPLING_RATE_48K;
                        break;

                    default:
                        break;
                }

        le_audio_log("[CAP][AM] config_info [codec:%X][sampling_freq:%X][frame_payload_length:%04X][volume:%x]", 4,
           config_info.codec[0], config_info.sampling_frequency, config_info.frame_payload_length, volume_state.volume);

        aud_cap.type = BLE;
        if(config_info.codec[0] == codec_id[0])
        {
            aud_cap.codec.ble_format.ble_codec.codec                = CODEC_LC3;
        }

        if(config_info.is_stereo == true)
            aud_cap.codec.ble_format.ble_codec.channel_num = CHANNEL_STEREO;
        else {
            if(le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET)
                aud_cap.codec.ble_format.ble_codec.channel_num = CHANNEL_DUAL_MONO;
            else
                aud_cap.codec.ble_format.ble_codec.channel_num = CHANNEL_MONO;
        }
        le_audio_log("[CAP][AM] play callback, channel_num:%d", 1, aud_cap.codec.ble_format.ble_codec.channel_num);

        aud_cap.codec.ble_format.ble_codec.channel_mode = (mode <= CAP_AM_UNICAST_CALL_MODE_1) ? CHANNEL_MODE_DL_UL_BOTH : CHANNEL_MODE_DL_ONLY;

        if ((config_info.sdu_interval[0] + (config_info.sdu_interval[1] << 8) + (config_info.sdu_interval[2] << 16)) == 0x002710) {
            aud_cap.codec.ble_format.ble_codec.frame_duration = FRAME_MS_10;
        }
        else if((config_info.sdu_interval[0] + (config_info.sdu_interval[1] << 8) + (config_info.sdu_interval[2] << 16)) == 0x001D4C) {
            aud_cap.codec.ble_format.ble_codec.frame_duration = FRAME_MS_7P5;
        }

        aud_cap.codec.ble_format.ble_codec.frame_payload_length = config_info.frame_payload_length;
        aud_cap.codec.ble_format.ble_event = BT_CODEC_MEDIA_REQUEST;
        aud_cap.audio_stream_in.audio_device  = CAP_AUDIO_INPUT_DEVICE;
        aud_cap.audio_stream_in.audio_volume  = AUD_VOL_IN_LEVEL0;
        aud_cap.audio_stream_out.audio_device = CAP_AUDIO_OUTPUT_DEVICE;
        aud_cap.audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)(volume_state.volume);
        aud_cap.audio_stream_out.audio_mute = volume_state.mute;

        bt_sink_srv_cap_am_set_state(mode, CAP_AM_STATE_WAIT_PLAY);

		am_result = bt_sink_srv_ami_audio_play(g_cap_am_ctl[mode].cap_aid, &aud_cap);
        le_audio_log("[CAP][AM] play callback, play result:%d", 1, am_result);
        if(am_result != AUD_EXECUTION_SUCCESS)
        {
            bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
#ifdef AIR_LE_AUDIO_CIS_ENABLE
            bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SINK);
            if(le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET) {
                //respond 2nd ASE Characteristic
                le_audio_log("[CAP][AM] enabling_response second ase_id", 0);
                bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SINK);
            }

            if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                //respond both ASE Characteristic
                le_audio_log("[CAP][AM] enabling_response source ase_id", 0);
                bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SOURCE);
            }
#endif
        }

		//#ifdef AIR_LE_AUDIO_ENABLE
		//bt_sink_srv_cap_am_action_callback(g_cap_am_ctl.cap_aid, AUD_SINK_OPEN_CODEC, AUD_CMD_COMPLETE, NULL);
		//#endif
    } else {
        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
    }
#endif
}

static void bt_sink_srv_cap_am_stop_callback(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_mode_by_audio_handle(handle);
    uint8_t state = bt_sink_srv_cap_am_get_state(mode);

    le_audio_log("[CAP][AM] stop callback, mode:%d, state:%d", 2, mode, state);
#ifdef AIR_BT_CODEC_BLE_ENABLED
    if(handle->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE && state == CAP_AM_STATE_PREPARE_STOP)
    {
        bt_sink_srv_cap_am_set_state(mode, CAP_AM_STATE_WAIT_STOP);

		if (mode <= CAP_AM_UNICAST_CALL_MODE_1) am_audio_side_tone_disable();
        
		if (AUD_EXECUTION_FAIL == bt_sink_srv_ami_audio_stop(g_cap_am_ctl[mode].cap_aid)) {
            /*Audio resource is not in used*/
            bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_READY);
		}

		//#ifdef AIR_LE_AUDIO_ENABLE
		//bt_sink_srv_cap_am_action_callback(g_cap_am_ctl.cap_aid, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
		//#endif
    } else if (state == CAP_AM_STATE_SUSPENDING) {
		if (AUD_EXECUTION_FAIL == bt_sink_srv_ami_audio_stop(g_cap_am_ctl[mode].cap_aid)) {
            /*Audio resource is not in used*/
            bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_READY);
            audio_src_srv_add_waiting_list(g_cap_am_ctl[mode].p_handle);
		}
	}
#endif
}

static void bt_sink_srv_cap_am_suspend_callback(audio_src_srv_handle_t *handle, audio_src_srv_handle_t *int_hd)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_mode_by_audio_handle(handle);
    uint8_t state = bt_sink_srv_cap_am_get_state(mode);

    le_audio_log("[CAP][AM] suspend callback, mode:%d, state:%d, int_priority:%d", 3, mode, state, int_hd->priority);
#ifdef AIR_BT_CODEC_BLE_ENABLED
    if (handle->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE) {
        if (state == CAP_AM_STATE_PLAYING) {

            if (mode == g_cap_am_restarting_mode) {
                /*Restarting is interrupted*/
                g_cap_am_restarting_mode = CAP_INVALID_UINT8;
            }

            if (mode == CAP_AM_BROADCAST_MUSIC_MODE) {
                if (AUDIO_SRC_SRV_PRIORITY_HIGH == int_hd->priority) {
#if (BROADCAST_MUSIC_RESUME)
                bt_sink_srv_cap_am_set_state(mode, CAP_AM_STATE_SUSPENDING);
#endif
                }
                bt_sink_srv_cap_stream_service_big_t *big_info = bt_sink_srv_cap_stream_get_service_big();
                if (NULL != big_info) {
                    ble_bap_remove_bis_data_path(big_info->big_handle, 0x03);
                }
                bt_sink_srv_cap_am_audio_stop(mode);
            } else if (AUDIO_SRC_SRV_PRIORITY_HIGH == int_hd->priority) {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
                if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                    /*Call mode shall not resume*/
                    bt_handle_t conn_handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);
                    bt_sink_srv_cap_stream_release_autonomously(conn_handle, 0xFF, false, 0);
                } else {
                    /*Check currnt media state to judge resuming or not*/
                    bt_handle_t conn_handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);
                    bool resume = (BLE_MCS_MEDIA_STATE_PLAYING == bt_le_audio_sink_media_get_state(conn_handle, BLE_MCP_GMCS_INDEX) ? true : false);
                    if (resume) {
                        /*Set suspending state to resume music after call end*/
                        bt_sink_srv_cap_am_set_state(mode, CAP_AM_STATE_SUSPENDING);
                    }
                    bt_sink_srv_cap_stream_release_autonomously(conn_handle, 0xFF, resume, 0);
                }
            } else {
                /*No need to resume*/
                bt_handle_t conn_handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);
                bt_sink_srv_cap_stream_release_autonomously(conn_handle, 0xFF, false, 0);
#endif
            }

        } else if (state == CAP_AM_STATE_WAIT_PLAY) {
            //bt_handle_t conn_handle = bt_sink_srv_cap_stream_get_service_ble_link();
            bt_sink_srv_cap_am_audio_stop(mode);

            /*if (conn_handle != BT_HANDLE_INVALID) {
                bt_sink_srv_cap_stream_enabling_response(false);
                if (g_cap_am_ctl.current_mode == UNICAST_CALL_MODE) {
                    bt_sink_srv_cap_stream_enabling_response(false);
                }
                bt_sink_srv_cap_stream_clear_service_ble_link(conn_handle);
            }*/
        }
    }
#endif
}

static void bt_sink_srv_cap_am_reject_callback(audio_src_srv_handle_t *handle)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_mode_by_audio_handle(handle);
    uint8_t state = bt_sink_srv_cap_am_get_state(mode);

    const audio_src_srv_handle_t *rej_handle = NULL;
    bt_handle_t conn_handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);

    le_audio_log("[CAP][AM] reject callback, mode:%d, state:%d", 2, mode, state);

#ifdef AIR_BT_CODEC_BLE_ENABLED
    if(handle->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE && state == CAP_AM_STATE_PREPARE_PLAY)
    {
        rej_handle = audio_src_srv_get_runing_pseudo_device();

        le_audio_log("[CAP][AM] reject callback, mode:%d, RejDevId:%d, RejDevType:%d, BleDevId:%d", 4, mode, rej_handle->dev_id, rej_handle->type, handle->dev_id);

        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_READY);

        if (mode == CAP_AM_BROADCAST_MUSIC_MODE) {
            bt_sink_srv_cap_stream_broadcast_enabling_response(false);

        } else if (conn_handle != BT_HANDLE_INVALID) {
            bt_sink_srv_cap_am_set_state(mode, CAP_AM_STATE_PREPARE_PLAY);
            audio_src_srv_add_waiting_list(handle);
            bt_sink_srv_cap_am_reject_timer_start(mode);

#ifdef AIR_LE_AUDIO_CIS_ENABLE
            /*bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SINK);
            if (mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SOURCE);
            }*/
#endif

        }
        //bt_sink_srv_cap_am_send_event_and_set_state(AUDIO_SRC_SRV_EVT_UNAVAILABLE);
    }
#endif
}

static void bt_sink_srv_cap_am_exception_callback(audio_src_srv_handle_t *handle, int32_t event, void *param)
{
    bt_sink_srv_cap_am_mode mode = bt_sink_srv_cap_am_get_mode_by_audio_handle(handle);
    uint8_t state = bt_sink_srv_cap_am_get_state(mode);

    le_audio_log("[CAP][AM] exception callback, mode:%d, state:%d", 2, mode, state);
#ifdef AIR_BT_CODEC_BLE_ENABLED
    if(handle->type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE)
    {

    }
#endif
}

static void bt_sink_srv_cap_am_send_event_and_set_state(bt_sink_srv_cap_am_mode mode, audio_src_srv_event_t event_id)
{
    uint8_t cur_state = bt_sink_srv_cap_am_get_state(mode);

    le_audio_log("[CAP][AM] send to am and set state, mode:%d, event:%d, state:%d", 3, mode, event_id, cur_state);
    switch (event_id) {
        case AUDIO_SRC_SRV_EVT_UNAVAILABLE:
            cur_state = CAP_AM_STATE_OFF;
            break;
        case AUDIO_SRC_SRV_EVT_READY:
            if (cur_state != CAP_AM_STATE_SUSPENDING) {
                cur_state = CAP_AM_STATE_IDLE;
            }
            break;
        case AUDIO_SRC_SRV_EVT_PREPARE_PLAY:
            cur_state = CAP_AM_STATE_PREPARE_PLAY;
            break;
        case AUDIO_SRC_SRV_EVT_PREPARE_STOP:
            if (cur_state != CAP_AM_STATE_SUSPENDING) {
                cur_state = CAP_AM_STATE_PREPARE_STOP;
            }
            break;
        case AUDIO_SRC_SRV_EVT_PLAYING:
            cur_state = CAP_AM_STATE_PLAYING;
            break;

        default:
            break;
    }
    bt_sink_srv_cap_am_set_state(mode, cur_state);

    audio_src_srv_update_state(g_cap_am_ctl[mode].p_handle, event_id);
}

void bt_sink_srv_cap_am_disable_waiting_list(void)
{
    for (uint8_t i = 0; i < CAP_AM_MODE_NUM; i++) {
        if (bt_sink_srv_cap_am_get_state(i) == CAP_AM_STATE_SUSPENDING) {

            le_audio_log("[CAP][AM] disable suspending mode [%d]", 1, i);
            audio_src_srv_del_waiting_list(g_cap_am_ctl[i].p_handle);
        }
    }
}

void bt_sink_srv_cap_am_enable_waiting_list(void)
{
    for (uint8_t i = 0; i < CAP_AM_MODE_NUM; i++) {
        if (bt_sink_srv_cap_am_get_state(i) == CAP_AM_STATE_SUSPENDING) {

            le_audio_log("[CAP][AM] enable suspending mode [%d]", 1, i);
            audio_src_srv_add_waiting_list(g_cap_am_ctl[i].p_handle);
        }
    }
}

static void bt_sink_srv_cap_am_set_state(bt_sink_srv_cap_am_mode mode, uint8_t state)
{
    le_audio_log("[CAP][AM] set state, mode:%d, state:%d", 2, mode, state);
    g_cap_am_ctl[mode].state = state;
}

static uint8_t bt_sink_srv_cap_am_get_state(bt_sink_srv_cap_am_mode mode)
{
    return g_cap_am_ctl[mode].state;
}

static bt_sink_srv_cap_am_mode bt_sink_srv_cap_am_get_mode_by_audio_id(bt_sink_srv_am_id_t aud_id)
{
    for (uint8_t i = 0; i < CAP_AM_MODE_NUM; i++) {
        if (g_cap_am_ctl[i].cap_aid == aud_id) {
            return i;
        }
    }
    return CAP_INVALID_UINT8;
}

static bt_sink_srv_cap_am_mode bt_sink_srv_cap_am_get_mode_by_audio_handle(audio_src_srv_handle_t *p_handle)
{
    for (uint8_t i = 0; i < CAP_AM_MODE_NUM; i++) {
        if (g_cap_am_ctl[i].p_handle == p_handle) {
            return i;
        }
    }
    return CAP_INVALID_UINT8;
}

static bool bt_sink_srv_cap_am_reject_timer_start(bt_sink_srv_cap_am_mode mode)
{
    le_audio_log("[CAP][AM] bt_sink_srv_cap_am_reject_timer_start, mode:%d, g_cap_am_reject_timer:%x", 2, mode, g_cap_am_reject_timer);

    if (g_cap_am_reject_timer) {
        return false;
    } else {
        g_cap_am_reject_timer = xTimerCreate("cap_am_reject_timer", (1500 * portTICK_PERIOD_MS), pdFALSE, NULL, bt_sink_srv_cap_am_reject_timer_callback);
    }

    if (!g_cap_am_reject_timer) {
        return false;
    }

    xTimerStart(g_cap_am_reject_timer, 0);
    g_cap_am_reject_mode = mode;
    return true;
}

static void bt_sink_srv_cap_am_reject_timer_callback(TimerHandle_t timer_id)
{
    uint8_t state;
    bt_handle_t conn_handle;

    le_audio_log("[CAP][AM] reject timer timeout, g_cap_am_reject_mode:%d, g_cap_am_reject_timer:%x, timer_id:%x", 3,
        g_cap_am_reject_mode, g_cap_am_reject_timer, timer_id);

    if (pdPASS == xTimerDelete(g_cap_am_reject_timer, 0)) {
            g_cap_am_reject_timer = NULL;
    }

    if (CAP_INVALID_UINT8 == g_cap_am_reject_mode) {
        return;
    }

    state = bt_sink_srv_cap_am_get_state(g_cap_am_reject_mode);
    conn_handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(g_cap_am_reject_mode);

    le_audio_log("[CAP][AM] reject timer timeout, mode:%d, state:%d", 2, g_cap_am_reject_mode, state);

    if (state == CAP_AM_STATE_PREPARE_PLAY) {
        bt_sink_srv_cap_am_send_event_and_set_state(g_cap_am_reject_mode, AUDIO_SRC_SRV_EVT_READY);

        if (conn_handle != BT_HANDLE_INVALID) {
            bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SINK);

            if(le_audio_get_device_type() == LE_AUDIO_DEVICE_TYPE_HEADSET) {
                //respond 2nd ASE Characteristic
                le_audio_log("[CAP][AM] enabling_response second ase_id", 0);
                bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SINK);
            }

            if (g_cap_am_reject_mode <= CAP_AM_UNICAST_CALL_MODE_1) {
                //respond both ASE Characteristic
                le_audio_log("[CAP][AM] enabling_response source ase_id", 0);
                bt_sink_srv_cap_stream_enabling_response(conn_handle, false, AUDIO_DIRECTION_SOURCE);
            }
        }

        audio_src_srv_del_waiting_list(g_cap_am_ctl[g_cap_am_reject_mode].p_handle);
        g_cap_am_reject_mode = CAP_INVALID_UINT8;
    }
}

static bool bt_sink_srv_cap_am_reject_timer_stop(void)
{
    le_audio_log("[CAP][AM] reject timer stop, g_cap_am_reject_timer:%x", 1, g_cap_am_reject_timer);
    if (NULL == g_cap_am_reject_timer) {
        return true;
    }

    if (pdPASS == xTimerDelete(g_cap_am_reject_timer, 0)) {
        g_cap_am_reject_timer = NULL;
        if (g_cap_am_reject_mode != CAP_INVALID_UINT8) {
            audio_src_srv_del_waiting_list(g_cap_am_ctl[g_cap_am_reject_mode].p_handle);
            g_cap_am_reject_mode = CAP_INVALID_UINT8;
        }
        return true;
    }

    return false;
}


/**************************************************************************************************
* Public Functions
**************************************************************************************************/
void bt_sink_srv_cap_am_init()
{
    le_audio_log("[CAP][AM] init, current mode:%d", 1, g_cap_am_current_mode);

    if(g_cap_am_current_mode == CAP_INVALID_UINT8)
    {
        bt_sink_srv_cap_am_para_init();
        bt_sink_srv_cap_am_function_init();
        bt_sink_srv_cap_am_callback_register();
        //bt_sink_srv_cap_am_send_event_and_set_state(AUDIO_SRC_SRV_EVT_READY);
    }
}

void bt_sink_srv_cap_am_deinit(void)
{
    uint8_t state = 0;
    for (uint8_t mode = 0; mode < CAP_AM_MODE_NUM; mode++) {
        state = bt_sink_srv_cap_am_get_state(mode);
        le_audio_log("[CAP][AM] deinit, mode:%d, state:%d", 2, mode, state);

        if ((g_cap_am_ctl[mode].cap_aid == AUD_ID_INVALID) || (state == CAP_AM_STATE_OFF)) {
            continue;
        }

        if (state == CAP_AM_STATE_PLAYING) {
            g_cap_am_ctl[mode].deinit_needed = true;
            bt_sink_srv_cap_am_audio_stop(mode);
            continue;
        } else if (state == CAP_AM_STATE_IDLE || state == CAP_AM_STATE_SUSPENDING) {
            bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_UNAVAILABLE);
            bt_sink_srv_cap_am_callback_deregister(mode);
        } else if (state == CAP_AM_STATE_PREPARE_PLAY || state == CAP_AM_STATE_WAIT_PLAY ||
            state == CAP_AM_STATE_PREPARE_STOP || state == CAP_AM_STATE_WAIT_STOP) {
            bt_sink_srv_cap_am_reject_timer_stop();
            g_cap_am_ctl[mode].deinit_needed = true;
        }
    }

    ///
    /*uint8_t state = bt_sink_srv_cap_am_get_state();
    le_audio_log("[CAP][AM] deinit, aid:%d, state:%d", 2, g_cap_am_ctl.cap_aid, state);

    if((g_cap_am_ctl.cap_aid == AUD_ID_INVALID) || (state == CAP_AM_STATE_OFF) || (state == CAP_AM_STATE_SUSPENDING))
    {
        return;
    }

    if (state == CAP_AM_STATE_PLAYING && g_cap_am_ctl.current_mode == BROADCAST_MUSIC_MODE && reason == CAP_AM_DEINIT_REASON_LE_DISCONNECT) {
        return;
    }

    if (state == CAP_AM_STATE_PLAYING) {
        g_cap_am_ctl.deinit_needed = true;
        bt_sink_srv_cap_am_audio_stop();
        return;
    } else if (state == CAP_AM_STATE_IDLE) {
        bt_sink_srv_cap_am_send_event_and_set_state(AUDIO_SRC_SRV_EVT_UNAVAILABLE);
        bt_sink_srv_cap_am_callback_deregister();
    } else if (state == CAP_AM_STATE_PREPARE_PLAY || state == CAP_AM_STATE_WAIT_PLAY ||
        state == CAP_AM_STATE_PREPARE_STOP || state == CAP_AM_STATE_WAIT_STOP) {
        g_cap_am_ctl.deinit_needed = true;
    }*/

}

void bt_sink_srv_cap_am_audio_start(bt_sink_srv_cap_am_mode mode)
{
    if (mode >= CAP_AM_MODE_NUM) {
        le_audio_log("[CAP][AM] Invalid mode:%d", 1, mode);
        return;
    }
    uint8_t state = bt_sink_srv_cap_am_get_state(mode);
    le_audio_log("[CAP][AM] audio start, mode:%d, state:%d", 2, mode, state);


    /*switch(mode){
        case UNICAST_MUSIC_MODE:
        case UNICAST_CALL_MODE:
            bt_sink_srv_cap_am_set_sub_state(CAP_AM_SUB_STATE_UNICAST_STREAM_PREPARING);
            break;

        case BROADCAST_MUSIC_MODE:
            bt_sink_srv_cap_am_set_sub_state(CAP_AM_SUB_STATE_BROADCAST_STREAM_PREPARING);
            break;

        default:
            configASSERT(0);
    }*/

    if(state == CAP_AM_STATE_IDLE || state == CAP_AM_STATE_PREPARE_STOP || state == CAP_AM_STATE_WAIT_STOP || state == CAP_AM_STATE_SUSPENDING) {

        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_PLAY);

    }
}

void bt_sink_srv_cap_am_audio_stop(bt_sink_srv_cap_am_mode mode)
{
    if (mode >= CAP_AM_MODE_NUM) {
        le_audio_log("[CAP][AM] Invalid mode:%d", 1, mode);
        return;
    }

    uint8_t state = bt_sink_srv_cap_am_get_state(mode);

    le_audio_log("[CAP][AM] audio stop, mode:%d, state:%d", 2, mode, state);

    if (mode == g_cap_am_restarting_mode) {
        /*Restarting is interrupted*/
        g_cap_am_restarting_mode = CAP_INVALID_UINT8;
    }

    switch (state) {
        case CAP_AM_STATE_PLAYING: {
            bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
            break;
        }

        case CAP_AM_STATE_PREPARE_PLAY:
        case CAP_AM_STATE_WAIT_PLAY: {
            if (mode == g_cap_am_reject_mode) {
                /*Reject timer is proccessing*/
                bt_sink_srv_cap_am_reject_timer_stop();
                bt_sink_srv_cap_am_set_state(mode, CAP_AM_STATE_IDLE);
            } else {
                g_cap_am_ctl[mode].stop_needed = true;
            }
            break;
        }

        case CAP_AM_STATE_SUSPENDING: {
            bt_sink_srv_ami_audio_stop(g_cap_am_ctl[mode].cap_aid);
            break;
        }

        default:
            break;


    }

    /*if(state == CAP_AM_STATE_PLAYING)
    {
        bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
    }
    else if(state == CAP_AM_STATE_PREPARE_PLAY || state == CAP_AM_STATE_WAIT_PLAY ||
        state == CAP_AM_STATE_PREPARE_STOP || state == CAP_AM_STATE_WAIT_STOP)
    {
        g_cap_am_ctl[mode].stop_needed = true;
    }
    else if(state == CAP_AM_STATE_SUSPENDING)
    {
        bt_sink_srv_ami_audio_stop(g_cap_am_ctl[mode].cap_aid);
    }
    else if(state == CAP_AM_STATE_IDLE)
    {
        return;

        bt_handle_t conn_handle = bt_sink_srv_cap_get_ble_link_by_streaming_mode(mode);
        switch (mode) {
            case CAP_AM_UNICAST_CALL_MODE_0:
            case CAP_AM_UNICAST_CALL_MODE_1:
                bt_sink_srv_cap_stream_disabling_response(conn_handle, true, AUDIO_DIRECTION_SINK);
                bt_sink_srv_cap_stream_disabling_response(conn_handle, true, AUDIO_DIRECTION_SOURCE);
                break;

            case CAP_AM_UNICAST_MUSIC_MODE_0:
            case CAP_AM_UNICAST_MUSIC_MODE_1:
                bt_sink_srv_cap_stream_disabling_response(conn_handle, true, AUDIO_DIRECTION_SINK);
                break;

            default:
                break;
        }
    }*/
}

void bt_sink_srv_cap_am_audio_suspend(bt_sink_srv_cap_am_mode mode)
{
    if (mode >= CAP_AM_MODE_NUM) {
        le_audio_log("[CAP][AM] Invalid mode:%d", 1, mode);
        return;
    }

    uint8_t state = bt_sink_srv_cap_am_get_state(mode);

    le_audio_log("[CAP][AM] audio suspend, mode:%d, state:%d", 2, mode, state);

    if (mode == g_cap_am_restarting_mode) {
        /*Restarting is interrupted*/
        g_cap_am_restarting_mode = CAP_INVALID_UINT8;
    }

    if (CAP_AM_STATE_PLAYING == state) {
        bt_sink_srv_cap_am_set_state(mode, CAP_AM_STATE_SUSPENDING);
    } else if (CAP_AM_STATE_PREPARE_PLAY == state || CAP_AM_STATE_WAIT_PLAY == state) {
        g_cap_am_ctl[mode].stop_needed = true;
    }
    bt_sink_srv_cap_am_send_event_and_set_state(mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
}

bool bt_sink_srv_cap_am_audio_restart(void)
{
    le_audio_log("[CAP][AM] audio restart, current current_mode:%d, restarting_mode:%d", 2,
        g_cap_am_current_mode, g_cap_am_restarting_mode);

    if (g_cap_am_current_mode == CAP_INVALID_UINT8 || g_cap_am_restarting_mode != CAP_INVALID_UINT8) {
        return false;
    }

    if (CAP_AM_STATE_PLAYING != bt_sink_srv_cap_am_get_state(g_cap_am_current_mode)) {
        return false;
    }

    g_cap_am_restarting_mode = g_cap_am_current_mode;
    bt_sink_srv_cap_am_send_event_and_set_state(g_cap_am_restarting_mode, AUDIO_SRC_SRV_EVT_PREPARE_STOP);
    return true;
}

int8_t bt_sink_srv_cap_am_get_aid(void)
{
    if (g_cap_am_current_mode < CAP_AM_MODE_NUM) {
        return g_cap_am_ctl[g_cap_am_current_mode].cap_aid;
    } else {
        return AUD_ID_INVALID;
    }
}

bt_sink_srv_cap_am_mode bt_sink_srv_cap_am_get_current_mode(void)
{
    le_audio_log("[CAP][AM] Get current streaming mode:%d", 1, g_cap_am_current_mode);
    return g_cap_am_current_mode;
}

bt_sink_srv_cap_am_mode bt_sink_srv_cap_am_get_restarting_mode(void)
{
    le_audio_log("[CAP][AM] Get restarting streaming mode:%d", 1, g_cap_am_restarting_mode);
    return g_cap_am_restarting_mode;
}


