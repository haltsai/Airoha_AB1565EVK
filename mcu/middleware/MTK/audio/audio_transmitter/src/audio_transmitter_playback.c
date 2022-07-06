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
#include "hal_platform.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_audio_message_struct.h"
#include "audio_transmitter_playback.h"
#include "audio_transmitter_internal.h"
#include "audio_transmitter_playback_port.h"
#include "bt_sink_srv_ami.h"
#include "audio_log.h"


static uint16_t g_uplink_set_running_flag_cnt = 0;
static uint16_t g_uplink_callback_regist_cnt = 0;
static audio_transmitter_scenario_type_t *g_isr_scenario; //add for isr callback

static void audio_transmitter_isr_handler(hal_audio_event_t event, void *data)
{
    uint32_t *cb_data = (uint32_t *)data;
    uint16_t scenario_and_id = (uint16_t)cb_data[0];
    uint8_t scenario_type = (scenario_and_id) >> 8;
    uint8_t scenario_sub_id = (uint8_t)(((scenario_and_id) << 8) >> 8);
    int8_t i;
    audio_transmitter_msg_handler_t msg_handler;
    void *user_data;

    for (i = 0; i < AUDIO_TRANSMITTER_MAX; i++) {
        if (g_audio_transmitter_control[i].config.scenario_type == scenario_type
                && g_audio_transmitter_control[i].config.scenario_sub_id == scenario_sub_id
                && g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTED) {
            msg_handler = g_audio_transmitter_control[i].config.msg_handler;
            user_data = g_audio_transmitter_control[i].config.user_data;
            break;
        }
    }
    if (i < AUDIO_TRANSMITTER_MAX) {
        switch (event) {
            case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
                msg_handler(AUDIO_TRANSMITTER_EVENT_DATA_NOTIFICATION, (void *)&cb_data[1], user_data);
                break;
            case HAL_AUDIO_EVENT_DATA_DIRECT:
                msg_handler(AUDIO_TRANSMITTER_EVENT_DATA_DIRECT, (void *)&cb_data[1], user_data);
                break;
            default:
                break;
        }
    }
}

audio_transmitter_status_t audio_transmitter_playback_open(uint16_t scenario_and_id)
{
    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER;
    mcu2dsp_audio_msg_t open_msg = MSG_MCU2DSP_AUDIO_TRANSMITTER_OPEN;
    void *p_param_share;
    mcu2dsp_open_param_t open_param;
    int i;
    audio_transmitter_config_t  config;
    n9_dsp_share_info_t **read_info, **write_info;

    uint8_t scenario_type = scenario_and_id >> 8;
    uint8_t scenario_sub_id = (uint8_t)((scenario_and_id << 8) >> 8);
    for (i = 0; i < AUDIO_TRANSMITTER_MAX; i++) {
        if (g_audio_transmitter_control[i].config.scenario_type == scenario_type
                && g_audio_transmitter_control[i].config.scenario_sub_id == scenario_sub_id) {
            config = g_audio_transmitter_control[i].config;
            read_info = &g_audio_transmitter_control[i].p_read_info;
            write_info = &g_audio_transmitter_control[i].p_write_info;
            break;
        }
    }
    if (i == AUDIO_TRANSMITTER_MAX) {
        TRANSMITTER_LOG_E("not init yet\r\n", 0);
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }

    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));

    if (audio_transmitter_playback_func[scenario_type].open_playback != NULL) {
        audio_transmitter_playback_func[scenario_type].open_playback(&config, &open_param);
        if(open_param.param.stream_in == STREAM_IN_AUDIO_TRANSMITTER){
            *write_info = open_param.stream_in_param.data_dl.p_share_info;
        }
        if(open_param.param.stream_out == STREAM_OUT_AUDIO_TRANSMITTER){
            *read_info = open_param.stream_out_param.data_ul.p_share_info;
        }
    }

    if (g_uplink_set_running_flag_cnt == 0) {
        bool is_running = hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER);
        if (is_running) {
            // Reentry: don't allow multiple playback
        } else {
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER, true);//hal_audio_status_set_running_flag
        }
    }
    g_uplink_set_running_flag_cnt++;

    if (g_uplink_callback_regist_cnt == 0) {
        hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER, audio_transmitter_isr_handler, g_isr_scenario);//user_data???挂msg notification cb，注意type
    }

    g_uplink_callback_regist_cnt++;

    p_param_share = hal_audio_dsp_controller_put_paramter(&open_param, sizeof(mcu2dsp_open_param_t), msg_type);
    hal_audio_dsp_controller_send_message(open_msg, scenario_and_id, (uint32_t)p_param_share, true);

    TRANSMITTER_LOG_I("transmitter play back open\r\n", 0);

    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

audio_transmitter_status_t audio_transmitter_playback_start(uint16_t scenario_and_id)
{
    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER;
    mcu2dsp_audio_msg_t start_msg = MSG_MCU2DSP_AUDIO_TRANSMITTER_START;
    void *p_param_share;
    mcu2dsp_start_param_t start_param;
    int i;
    audio_transmitter_config_t  config;

    uint8_t scenario_type = scenario_and_id >> 8;
    uint8_t scenario_sub_id = (uint8_t)((scenario_and_id << 8) >> 8);
    for (i = 0; i < AUDIO_TRANSMITTER_MAX; i++) {
        if (g_audio_transmitter_control[i].config.scenario_type == scenario_type
                && g_audio_transmitter_control[i].config.scenario_sub_id == scenario_sub_id) {
            config = g_audio_transmitter_control[i].config;
            break;
        }
    }
    if (i == AUDIO_TRANSMITTER_MAX) {
        TRANSMITTER_LOG_E("not init yet\r\n", 0);
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }

    memset(&start_param, 0, sizeof(mcu2dsp_start_param_t));

    if (audio_transmitter_playback_func[scenario_type].start_playback != NULL) {
        audio_transmitter_playback_func[scenario_type].start_playback(&config, &start_param);
    }

    p_param_share = hal_audio_dsp_controller_put_paramter(&start_param, sizeof(mcu2dsp_start_param_t), msg_type);
    hal_audio_dsp_controller_send_message(start_msg, scenario_and_id, (uint32_t)p_param_share, true);

    TRANSMITTER_LOG_I("transmitter play back start\r\n", 0);
    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

audio_transmitter_status_t audio_transmitter_playback_stop(uint16_t scenario_and_id)
{
    mcu2dsp_audio_msg_t stop_msg = MSG_MCU2DSP_AUDIO_TRANSMITTER_STOP;

    hal_audio_dsp_controller_send_message(stop_msg, scenario_and_id, 0, true);

    TRANSMITTER_LOG_I("transmitter play back stop\r\n", 0);
    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

audio_transmitter_status_t audio_transmitter_playback_close(uint16_t scenario_and_id)
{
    mcu2dsp_audio_msg_t close_msg = MSG_MCU2DSP_AUDIO_TRANSMITTER_CLOSE;

    hal_audio_dsp_controller_send_message(close_msg, scenario_and_id, 0, true);

    g_uplink_set_running_flag_cnt--;
    if (g_uplink_set_running_flag_cnt == 0) {
        if (hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER)) {
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER, false);
        }
    }

    g_uplink_callback_regist_cnt--;
    if (g_uplink_callback_regist_cnt == 0) {
        hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER);
    }

    TRANSMITTER_LOG_I("transmitter play back close\r\n", 0);
    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

audio_transmitter_status_t audio_transmitter_playback_set_runtime_config(uint16_t scenario_and_id, audio_transmitter_runtime_config_type_t runtime_config_type, audio_transmitter_runtime_config_t *runtime_config)
{
    mcu2dsp_audio_msg_t set_runtime_config_msg = MSG_MCU2DSP_AUDIO_TRANSMITTER_CONFIG;
    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_AUDIO_TRANSMITTER;
    void *p_param_share;

    mcu2dsp_audio_transmitter_runtime_config_param_t runtime_config_param;
    int i;
    audio_transmitter_config_t  *config;

    uint8_t scenario_type = scenario_and_id >> 8;
    uint8_t scenario_sub_id = (uint8_t)((scenario_and_id << 8) >> 8);
    for (i = 0; i < AUDIO_TRANSMITTER_MAX; i++) {
        if (g_audio_transmitter_control[i].config.scenario_type == scenario_type
                && g_audio_transmitter_control[i].config.scenario_sub_id == scenario_sub_id
                && g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTED) {
            config = &g_audio_transmitter_control[i].config;
            break;
        }
    }
    if (i == AUDIO_TRANSMITTER_MAX) {
        TRANSMITTER_LOG_I("not init yet\r\n", 0);
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }

    audio_transmitter_status_t status = AUDIO_TRANSMITTER_STATUS_FAIL;
    if (audio_transmitter_playback_func[scenario_type].set_runtime_config_playback != NULL) {
        status = audio_transmitter_playback_func[scenario_type].set_runtime_config_playback(config, runtime_config_type, runtime_config, &runtime_config_param);
    }
    if(status == AUDIO_TRANSMITTER_STATUS_SUCCESS) {
        p_param_share = hal_audio_dsp_controller_put_paramter(&runtime_config_param, sizeof(mcu2dsp_audio_transmitter_runtime_config_param_t), msg_type);
        hal_audio_dsp_controller_send_message(set_runtime_config_msg, scenario_and_id, (uint32_t)p_param_share, true);
    }

    TRANSMITTER_LOG_I("transmitter play back set runtime config\r\n", 0);
    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

