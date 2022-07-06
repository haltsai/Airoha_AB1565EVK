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
#include "audio_transmitter_control.h"
#include "audio_transmitter_internal.h"
#include "bt_sink_srv_ami.h"


log_create_module(audio_transmitter, PRINT_LEVEL_INFO);

audio_transmitter_control_t g_audio_transmitter_control[AUDIO_TRANSMITTER_MAX];

static void audio_transmitter_AM_callback(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_cb_msg_class_t msg_id,
        bt_sink_srv_am_cb_sub_msg_t sub_msg,
        void *parm)
{
    TRANSMITTER_LOG_I("[am cb]coming to AUDIO_TRANSMITTER am callback. msgid=%d, submsg=%d\r\n", 2, msg_id, sub_msg);
    if (parm != NULL) {
        uint16_t *scenario_and_id = (uint16_t *)parm;
        uint8_t scenario_type = (*scenario_and_id) >> 8;
        uint8_t scenario_sub_id = (uint8_t)(((*scenario_and_id) << 8) >> 8);
        TRANSMITTER_LOG_I("[am cb] scenario = %d, scenario_sub_id=%d", 2, scenario_type, scenario_sub_id);
        uint8_t i;
        audio_transmitter_msg_handler_t msg_handler;
        void *user_data;
        for (i = 0; i < AUDIO_TRANSMITTER_MAX; i++) {
            if ((g_audio_transmitter_control[i].am_id == aud_id)
                    &&(g_audio_transmitter_control[i].config.scenario_type == scenario_type)
                    && (g_audio_transmitter_control[i].config.scenario_sub_id == scenario_sub_id)
                    && (g_audio_transmitter_control[i].state != AUDIO_TRANSMITTER_STATE_CLOSE)) {
                msg_handler = g_audio_transmitter_control[i].config.msg_handler;
                user_data = g_audio_transmitter_control[i].config.user_data;
                break;
            }
        }
        if (i < AUDIO_TRANSMITTER_MAX) {
            switch (msg_id) {
                case AUD_SELF_CMD_REQ:
                    if (sub_msg == AUD_CMD_FAILURE) {
                        TRANSMITTER_LOG_I("[am cb]test_aud_set_play_stream_req_hdlr(), AM_PLAYBACK_REJECT\r\n", 0);
                        if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTING)
                        {
                            g_audio_transmitter_control[i].state = AUDIO_TRANSMITTER_STATE_IDLE;
                        }
                        if (audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE] != NULL) {
                            audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE](scenario_sub_id);
                        }
                        msg_handler(AUDIO_TRANSMITTER_EVENT_START_FAIL, NULL, user_data);
                    } else if (sub_msg == AUD_CMD_COMPLETE) {
                        TRANSMITTER_LOG_I("[am cb]come to am task hdlr am_audio_set_stop(), work,send ccni msg\r\n", 0);
                        if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STOPING)
                        {
                            g_audio_transmitter_control[i].state = AUDIO_TRANSMITTER_STATE_IDLE;
                        }
                        if (audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE] != NULL) {
                            audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE](scenario_sub_id);
                        }
                        msg_handler(AUDIO_TRANSMITTER_EVENT_STOP_SUCCESS, NULL, user_data);
                    }
                    break;
                case AUD_SINK_OPEN_CODEC:
                    if (sub_msg == AUD_CMD_COMPLETE) {
                        TRANSMITTER_LOG_I("[am cb]come to am task hdlr am_audio_set_play(), work,send ccni msg\r\n", 0);
                        if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTING)
                        {
                            g_audio_transmitter_control[i].state = AUDIO_TRANSMITTER_STATE_STARTED;
                        }
                        if (audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_STARTED] != NULL) {
                            audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_STARTED](scenario_sub_id);
                        }
                        msg_handler(AUDIO_TRANSMITTER_EVENT_START_SUCCESS, NULL, user_data);
                    } else if ((sub_msg == AUD_SUSPEND_BY_HFP) || (sub_msg == AUD_SUSPEND_BY_RECORDER) || (sub_msg == AUD_SUSPEND_BY_A2DP) || (sub_msg == AUD_SUSPEND_BY_LE_CALL)) {
                        TRANSMITTER_LOG_I("[am cb]come to am task hdlr am_audio_set_play(), rejected by HFP or RECORDER or A2DP\r\n", 0);
                        if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTING)
                        {
                            g_audio_transmitter_control[i].state = AUDIO_TRANSMITTER_STATE_IDLE;
                        }
                        if (audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE] != NULL) {
                            audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE](scenario_sub_id);
                        }
                        msg_handler((sub_msg == AUD_SUSPEND_BY_HFP) ? AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_HFP :
                                    (sub_msg == AUD_SUSPEND_BY_A2DP) ? AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_A2DP :
                                    (sub_msg == AUD_SUSPEND_BY_LE_CALL) ? AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_LE_CALL :
                                    AUDIO_TRANSMITTER_EVENT_START_REJECT_BY_RECORDER, NULL, user_data);
                    }
                    break;
                case AUD_HAL_EVENT_IND:
                    if (sub_msg == AUD_CMD_COMPLETE) {
                        TRANSMITTER_LOG_I("[am cb]come to am task hdlr audio_set_audio_transmitter_config(), work,send ccni msg\r\n", 0);
                        msg_handler(AUDIO_TRANSMITTER_EVENT_SET_RUNTIME_CONFIG_SUCCESS, NULL, user_data);
                    }
                    break;
                case AUD_SUSPEND_BY_IND:
                    if ((sub_msg == AUD_SUSPEND_BY_HFP) || (sub_msg == AUD_SUSPEND_BY_RECORDER) || (sub_msg == AUD_SUSPEND_BY_A2DP)) {
                        TRANSMITTER_LOG_I("[am cb]come to am task hdlr am_audio_set_play, suspended by HFP or RECORDER or A2DP\r\n", 0);
                        #ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
                        if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTED)
                        #else
                        if ((g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTED) || (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STOPING))
                        #endif
                        {
                            g_audio_transmitter_control[i].state = AUDIO_TRANSMITTER_STATE_IDLE;
                        }
                        if (audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE] != NULL) {
                            audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE](scenario_sub_id);
                        }
                        #ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
                        msg_handler((sub_msg == AUD_SUSPEND_BY_HFP) ? AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_HFP : 
                                    (sub_msg == AUD_SUSPEND_BY_A2DP) ? AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_A2DP : 
                                    (sub_msg == AUD_SUSPEND_BY_LE_CALL) ? AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LE_CALL :
                                    AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_RECORDER, NULL, user_data);
                        #endif
                    } else if ((sub_msg == AUD_SUSPEND_BY_LINE_OUT) || (sub_msg == AUD_SUSPEND_BY_USB_OUT)) {
                        TRANSMITTER_LOG_I("[am cb]come to am task hdlr am_audio_set_play, suspended by HFP or A2DP or Recorder\r\n", 0);
                        if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTED)
                        {
                            g_audio_transmitter_control[i].state = AUDIO_TRANSMITTER_STATE_IDLE;
                        }
                        if (audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE] != NULL) {
                            audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE](scenario_sub_id);
                        }
                        msg_handler((sub_msg == AUD_SUSPEND_BY_LINE_OUT) ? AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_OUT : AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_OUT, NULL, user_data);
                    } else if ((sub_msg == AUD_SUSPEND_BY_LINE_IN) || (sub_msg == AUD_SUSPEND_BY_USB_IN)) {
                        TRANSMITTER_LOG_I("[am cb]come to am task hdlr am_audio_set_play, suspended by LINE_OUT or USB_OUT\r\n", 0);
                        if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTED)
                        {
                            g_audio_transmitter_control[i].state = AUDIO_TRANSMITTER_STATE_IDLE;
                        }
                        if (audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE] != NULL) {
                            audio_transmitter_state_change_handler[scenario_type][AUDIO_TRANSMITTER_STATE_IDLE](scenario_sub_id);
                        }
                        msg_handler((sub_msg == AUD_SUSPEND_BY_LINE_IN) ? AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_LINE_IN : AUDIO_TRANSMITTER_EVENT_SUSPEND_BY_USB_IN, NULL, user_data);
                    }
                    break;
                default:
                    break;
            }
        }
    }

}

audio_transmitter_id_t audio_transmitter_init(audio_transmitter_config_t *config)
{
    audio_transmitter_id_t audio_transmitter_id = 0;
    bt_sink_srv_am_id_t aud_id = 0;
    bt_sink_srv_am_result_t ami_ret;

    if (config->msg_handler == NULL) {
        TRANSMITTER_LOG_E("user haven`t set callback\r\n", 0);
        return -1;
    }
    aud_id = ami_audio_open(AUDIO_TRANSMITTER, (bt_sink_srv_am_notify_callback)audio_transmitter_AM_callback);
    TRANSMITTER_LOG_I("init get am id = %d", 1, aud_id);
    if (aud_id == -1) {
        TRANSMITTER_LOG_E("Can`t get am id\r\n", 0);
        return -1;
    }
    int i;
    vTaskSuspendAll();
    for (i = 0; i < AUDIO_TRANSMITTER_MAX; i++) {
        if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_CLOSE) {
            g_audio_transmitter_control[i].am_id = aud_id;
            memcpy(&(g_audio_transmitter_control[i].config), config, sizeof(audio_transmitter_config_t));
            g_audio_transmitter_control[i].state = AUDIO_TRANSMITTER_STATE_IDLE;
            audio_transmitter_id = i;
            break;
        }
    }
    xTaskResumeAll();
    if (i == AUDIO_TRANSMITTER_MAX) {
        TRANSMITTER_LOG_E("more than MAX audio transmitter channel\r\n", 0);
        for (i = 0; i < AUDIO_TRANSMITTER_MAX; i++)
        {
            TRANSMITTER_LOG_E("already init type:%d, sub_id:%d\r\n", 2, g_audio_transmitter_control[i].config.scenario_type, g_audio_transmitter_control[i].config.scenario_sub_id);
        }
        /* assert more than maximum parallel audio_transmitter, need check AUDIO_TRANSMITTER_MAX number*/
        configASSERT(0);

        if (aud_id != -1) {
            ami_ret = bt_sink_srv_ami_audio_close(aud_id);
            if (ami_ret < 0) {
                TRANSMITTER_LOG_E("can`t close aud_id\r\n", 0);
                configASSERT(0);
            }
        }
        return -1;
    }
    return audio_transmitter_id;
}

audio_transmitter_status_t audio_transmitter_start(audio_transmitter_id_t id)
{
    bt_sink_srv_am_audio_capability_t  aud_cap;
    bt_sink_srv_am_result_t ami_ret;
    bt_sink_srv_am_id_t aud_id;

    memset(&aud_cap, 0, sizeof(bt_sink_srv_am_audio_capability_t));
    aud_cap.type = AUDIO_TRANSMITTER;

    if (g_audio_transmitter_control[id].state == AUDIO_TRANSMITTER_STATE_IDLE) {
        aud_cap.codec.audio_transmitter_format.scenario_type = g_audio_transmitter_control[id].config.scenario_type;
        aud_cap.codec.audio_transmitter_format.scenario_sub_id = g_audio_transmitter_control[id].config.scenario_sub_id;
        aud_id = g_audio_transmitter_control[id].am_id;
    } else {
        TRANSMITTER_LOG_E("state = %d, can`t start.\r\n", 1, g_audio_transmitter_control[id].state);
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }
    if (g_audio_transmitter_control[id].state == AUDIO_TRANSMITTER_STATE_IDLE) {
        g_audio_transmitter_control[id].state = AUDIO_TRANSMITTER_STATE_STARTING;
        if (audio_transmitter_state_change_handler[g_audio_transmitter_control[id].config.scenario_type][AUDIO_TRANSMITTER_STATE_STARTING] != NULL) {
            audio_transmitter_state_change_handler[g_audio_transmitter_control[id].config.scenario_type][AUDIO_TRANSMITTER_STATE_STARTING](g_audio_transmitter_control[id].config.scenario_sub_id);
        }
    }

    ami_ret = ami_audio_play(aud_id, &aud_cap);
    TRANSMITTER_LOG_I("audio transmitter am id = %d, result = %d, when start\r\n", 2, aud_id, ami_ret);

    return (audio_transmitter_status_t)ami_ret;
}

audio_transmitter_status_t audio_transmitter_stop(audio_transmitter_id_t id)
{
    bt_sink_srv_am_result_t ami_ret;
    bt_sink_srv_am_id_t aud_id;
    if (g_audio_transmitter_control[id].state == AUDIO_TRANSMITTER_STATE_STARTED) {
        aud_id = g_audio_transmitter_control[id].am_id;
    } else {
        TRANSMITTER_LOG_E("state = %d, can`t stop.\r\n", 1, g_audio_transmitter_control[id].state);
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }
    if (g_audio_transmitter_control[id].state == AUDIO_TRANSMITTER_STATE_STARTED) {
        g_audio_transmitter_control[id].state = AUDIO_TRANSMITTER_STATE_STOPING;
        if (audio_transmitter_state_change_handler[g_audio_transmitter_control[id].config.scenario_type][AUDIO_TRANSMITTER_STATE_STOPING] != NULL) {
            audio_transmitter_state_change_handler[g_audio_transmitter_control[id].config.scenario_type][AUDIO_TRANSMITTER_STATE_STOPING](g_audio_transmitter_control[id].config.scenario_sub_id);
        }
    }
    ami_ret = ami_audio_stop(aud_id);
    TRANSMITTER_LOG_I("audio transmitter am id = %d, result = %d, when stop\r\n", 2, aud_id, ami_ret);
    return (audio_transmitter_status_t)ami_ret;
}

audio_transmitter_status_t audio_transmitter_deinit(audio_transmitter_id_t id)
{
    bt_sink_srv_am_result_t ami_ret;
    bt_sink_srv_am_id_t aud_id;
    if (g_audio_transmitter_control[id].state == AUDIO_TRANSMITTER_STATE_IDLE) {
        aud_id = g_audio_transmitter_control[id].am_id;
    } else {
        TRANSMITTER_LOG_E("state = %d, can`t deinit.\r\n", 1, g_audio_transmitter_control[id].state);
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }
    ami_ret = bt_sink_srv_ami_audio_close(aud_id);
    if (ami_ret == AUD_EXECUTION_SUCCESS) {
        g_audio_transmitter_control[id].state = AUDIO_TRANSMITTER_STATE_CLOSE;
        if (audio_transmitter_state_change_handler[g_audio_transmitter_control[id].config.scenario_type][AUDIO_TRANSMITTER_STATE_CLOSE] != NULL) {
            audio_transmitter_state_change_handler[g_audio_transmitter_control[id].config.scenario_type][AUDIO_TRANSMITTER_STATE_CLOSE](g_audio_transmitter_control[id].config.scenario_sub_id);
        }
    }
    return (audio_transmitter_status_t)ami_ret;
}

uint32_t audio_transmitter_get_available_data_size(audio_transmitter_id_t id)
{
    n9_dsp_share_info_t *p_info;
    uint8_t *p_source_buf;
    uint32_t buf_size, size = 0;
    uint32_t block_number;
    p_info = g_audio_transmitter_control[id].p_read_info;

    if (g_audio_transmitter_control[id].state != AUDIO_TRANSMITTER_STATE_STARTED) {
        TRANSMITTER_LOG_E("id%d state is %d, not started yet, can`t get availabel data size. \r\n", 2, id, g_audio_transmitter_control[id].state);
    }

    hal_audio_buf_mgm_get_data_buffer(p_info, &p_source_buf, &buf_size);
    //block base
    if (buf_size > 0) {
        block_number = buf_size / p_info->sub_info.block_size;
        for(uint32_t i = 0; i < block_number; i++) {
            size += ((audio_transmitter_block_header_t *)p_source_buf)->data_length;
            p_source_buf += p_info->sub_info.block_size;
        }
    }
    return size;
}

uint32_t audio_transmitter_get_available_data_space(audio_transmitter_id_t id)
{
    n9_dsp_share_info_t *p_info;
    uint8_t *p_source_buf;
    uint32_t space_size, space = 0;
    p_info = g_audio_transmitter_control[id].p_write_info;

    if (g_audio_transmitter_control[id].state != AUDIO_TRANSMITTER_STATE_STARTED) {
        TRANSMITTER_LOG_E("id%d state is %d, not started yet, can`t get availabel data space. \r\n", 2, id, g_audio_transmitter_control[id].state);
    }

    hal_audio_buf_mgm_get_free_buffer(p_info, &p_source_buf, &space_size);
    //block base
    if (space_size > 0) {
        space = (space_size / p_info->sub_info.block_size) * (p_info->sub_info.block_size - sizeof(audio_transmitter_block_header_t));
    }
    return space;
}

audio_transmitter_status_t audio_transmitter_get_read_information(audio_transmitter_id_t id, uint8_t **address, uint32_t *length)
{
    n9_dsp_share_info_t *p_info;
    uint8_t *p_source_buf;
    uint32_t buf_size;
    p_info = g_audio_transmitter_control[id].p_read_info;

    if (g_audio_transmitter_control[id].state != AUDIO_TRANSMITTER_STATE_STARTED) {
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }

    hal_audio_buf_mgm_get_data_buffer(p_info, &p_source_buf, &buf_size);
    //block base
    if (buf_size == 0) {
        *address = 0;
        *length = 0;
    } else {
        *address = p_source_buf + sizeof(audio_transmitter_block_header_t);
        *length = ((audio_transmitter_block_header_t *)p_source_buf)->data_length;
    }
    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

audio_transmitter_status_t audio_transmitter_read_done(audio_transmitter_id_t id, uint32_t length)
{
    n9_dsp_share_info_t *p_info;
    uint8_t *p_source_buf;
    uint32_t buf_size;
    p_info = g_audio_transmitter_control[id].p_read_info;

    if (g_audio_transmitter_control[id].state != AUDIO_TRANSMITTER_STATE_STARTED) {
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }

    hal_audio_buf_mgm_get_data_buffer(p_info, &p_source_buf, &buf_size);
    if (buf_size == 0)
    {
        TRANSMITTER_LOG_E("buffer is empty! can`t read done\r\n", 0);
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }
    //block base
    uint16_t len = ((audio_transmitter_block_header_t *)p_source_buf)->data_length;
    if (length != len) {
        TRANSMITTER_LOG_E("read done fail\r\n", 0);
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    } else {
        len = p_info->sub_info.block_size;
        hal_audio_buf_mgm_get_read_data_done(p_info, len);
    }

    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

audio_transmitter_status_t audio_transmitter_get_write_information(audio_transmitter_id_t id, uint8_t **address, uint32_t *length)
{
    n9_dsp_share_info_t *p_info;
    uint8_t *p_source_buf;
    uint32_t buf_size;
    p_info = g_audio_transmitter_control[id].p_write_info;

    if (g_audio_transmitter_control[id].state != AUDIO_TRANSMITTER_STATE_STARTED) {
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }

    hal_audio_buf_mgm_get_free_buffer(p_info, &p_source_buf, &buf_size);
    //block base
    *address = p_source_buf + sizeof(audio_transmitter_block_header_t);
    uint16_t len = p_info->sub_info.block_size;
    if (buf_size < len) {
        *length = 0;
    } else {
        *length = len - sizeof(audio_transmitter_block_header_t);
    }

    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

audio_transmitter_status_t audio_transmitter_write_done(audio_transmitter_id_t id, uint32_t length)
{
    n9_dsp_share_info_t *p_info;
    uint8_t *p_source_buf;
    uint32_t buf_size;
    p_info = g_audio_transmitter_control[id].p_write_info;

    if (g_audio_transmitter_control[id].state != AUDIO_TRANSMITTER_STATE_STARTED) {
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }

    hal_audio_buf_mgm_get_free_buffer(p_info, &p_source_buf, &buf_size);
    //block base
    uint16_t len = p_info->sub_info.block_size;

    if (buf_size < len) {
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }
    len -= sizeof(audio_transmitter_block_header_t);
    if (length > len) {
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    } else {
        ((audio_transmitter_block_header_t *)p_source_buf)->data_length = length;
        hal_audio_buf_mgm_get_write_data_done(p_info, (len + sizeof(audio_transmitter_block_header_t)));
    }

    return AUDIO_TRANSMITTER_STATUS_SUCCESS;
}

audio_transmitter_status_t audio_transmitter_set_runtime_config(audio_transmitter_id_t id, audio_transmitter_runtime_config_type_t config_type, audio_transmitter_runtime_config_t *runtime_config)
{
    bt_sink_srv_am_audio_capability_t aud_cap;
    bt_sink_srv_am_result_t ami_ret;
    bt_sink_srv_am_id_t aud_id;
    if (g_audio_transmitter_control[id].state == AUDIO_TRANSMITTER_STATE_STARTED) {
        aud_id = g_audio_transmitter_control[id].am_id;
    } else {
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }

    aud_cap.codec.audio_transmitter_format.scenario_runtime_config_type = (uint32_t)config_type;
    if(runtime_config != NULL){
        aud_cap.codec.audio_transmitter_format.scenario_runtime_config = *runtime_config;
    }
    else{
        memset(&(aud_cap.codec.audio_transmitter_format.scenario_runtime_config),0,sizeof(audio_transmitter_runtime_config_t));
    }
    ami_ret = ami_audio_set_audio_transmitter_config(aud_id, &aud_cap);

    return (audio_transmitter_status_t)ami_ret;
}

audio_transmitter_status_t audio_transmitter_get_runtime_config(audio_transmitter_id_t id, audio_transmitter_runtime_config_type_t config_type, audio_transmitter_runtime_config_t *runtime_config)
{
    audio_transmitter_status_t ret = AUDIO_TRANSMITTER_STATUS_FAIL;
    uint8_t scenario_type;
    uint8_t scenario_sub_id;

    if (g_audio_transmitter_control[id].state != AUDIO_TRANSMITTER_STATE_STARTED) {
        return ret;
    }

    scenario_type   = g_audio_transmitter_control[id].config.scenario_type;
    scenario_sub_id = g_audio_transmitter_control[id].config.scenario_sub_id;

    extern audio_transmitter_runtime_config_handler_t audio_transmitter_runtime_config_handler[];
    if (audio_transmitter_runtime_config_handler[scenario_type].get_runtime_config != NULL)
    {
        ret = audio_transmitter_runtime_config_handler[scenario_type].get_runtime_config(scenario_type, scenario_sub_id, config_type, runtime_config);
    }

    return ret;
}

bool audio_transmitter_get_is_running_by_scenario_list(audio_transmitter_scenario_list_t *list, uint8_t list_length)
{
    uint8_t i = 0, j = 0;
    for (i = 0; i < AUDIO_TRANSMITTER_MAX; i++) {
        for (j = 0; j < list_length; j++) {
            if ((g_audio_transmitter_control[i].config.scenario_type == list[j].scenario_type)
                    && (g_audio_transmitter_control[i].config.scenario_sub_id == list[j].scenario_sub_id)) {
                if (g_audio_transmitter_control[i].state == AUDIO_TRANSMITTER_STATE_STARTED){
                    return true;
                }
            }
        }
    }
    return false;
}

int8_t audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_type_t scenario_type, uint8_t scenario_sub_id)
{
    for (uint32_t i = 0; i < AUDIO_TRANSMITTER_MAX; i++) {
        if ((g_audio_transmitter_control[i].config.scenario_type == scenario_type)
                && (g_audio_transmitter_control[i].config.scenario_sub_id == scenario_sub_id)) {
                    return g_audio_transmitter_control[i].am_id;
        }
    }
    return -1;
}

