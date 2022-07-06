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

/* Includes ------------------------------------------------------------------*/

#include "syslog.h"
#include "record_control.h"

#include "bt_sink_srv_ami.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_audio_internal.h"

#include "audio_nvdm_common.h"
#include "audio_nvdm_coef.h"
#include "nvkey_dspalg.h"
#include "exception_handler.h"

#include "FreeRTOS.h"

extern bool g_record_airdump;
extern uint16_t g_stream_in_code_type;//modify for opus
extern encoder_bitrate_t g_bit_rate;
extern wwe_mode_t g_wwe_mode;

static uint8_t g_aud_record_id_num = 0;
static record_control_aud_id_type_t g_record_user[AUDIO_RECORD_REGISTER_ID_TOTAL];

#ifdef MTK_AUDIO_DUMP_ENABLE
#include "audio_dump.h"
#endif

log_create_module(record, PRINT_LEVEL_INFO);
#if 0
#define LOGE(fmt,arg...)   LOG_E(record, "Record: "fmt,##arg)
#define LOGW(fmt,arg...)   LOG_W(record, "Record: "fmt,##arg)
#define LOGI(fmt,arg...)   LOG_I(record, "Record: "fmt,##arg)
#else
#define LOGMSGIDE(fmt,arg...)   LOG_MSGID_E(record, "Record: "fmt,##arg)
#define LOGMSGIDW(fmt,arg...)   LOG_MSGID_W(record, "Record: "fmt,##arg)
#define LOGMSGIDI(fmt,arg...)   LOG_MSGID_I(record, "Record: "fmt,##arg)
#endif

#ifdef FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
bool g_dump = false;
static SemaphoreHandle_t g_record_mutex = NULL;
void record_mutex_creat(void)
{
    g_record_mutex = xSemaphoreCreateMutex();
    if (g_record_mutex == NULL) {
        LOGMSGIDE("record_mutex_creat error\r\n", 0);
    }
}
void record_mutex_take(void)
{
    if (g_record_mutex != NULL) {
        if (xSemaphoreTake(g_record_mutex, portMAX_DELAY) == pdFALSE) {
            LOGMSGIDE("record_mutex_take error\r\n", 0);
        }
    } else {
        for (uint16_t i = 1; ; i++) {
            if (g_record_mutex != NULL) {
                if (xSemaphoreTake(g_record_mutex, portMAX_DELAY) == pdFALSE) {
                    LOGMSGIDE("record_mutex_take error\r\n", 0);
                }
                return;
            }
            if (i == 1000) {
                LOGMSGIDE("Record_mutex NULL take time out.\r\n", 0);
                configASSERT(0);
            }
            vTaskDelay(2 / portTICK_RATE_MS);
        }
    }
}
void record_mutex_give(void)
{
    if (g_record_mutex != NULL) {
        if (xSemaphoreGive(g_record_mutex) == pdFALSE) {
            LOGMSGIDE("record_mutex_give error\r\n", 0);
        }
    }
}
#else
static int g_record_mutex = 1;
void record_mutex_creat(void)
{
}
void record_mutex_take(void)
{
}
void record_mutex_give(void)
{
}
#endif

typedef bt_sink_srv_am_notify_callback audio_record_notify_cb;
//typedef void (*audio_record_am_notify_cb)(record_control_event_t event_id);

void record_init(void)
{
    record_mutex_creat();
}

static void record_control_parameter_init(record_id_t aud_id, bt_sink_srv_am_audio_capability_t *audio_capability)
{
    /*To DO Audio Manager Prototype.*/
    if (NULL != audio_capability) {
        audio_capability->type = RECORD;
        for (uint8_t i = 0 ; i < AUDIO_RECORD_REGISTER_ID_TOTAL ; i++) {
            if ((g_record_user[i].am_id == aud_id) && (g_record_user[i].is_used == true)) {
                audio_capability->codec.record_format.record_codec.codec_cap.codec_type = g_record_user[i].encoder_codec.codec_type;
                audio_capability->codec.record_format.record_codec.codec_cap.bit_rate   = g_record_user[i].encoder_codec.bit_rate;
                audio_capability->codec.record_format.record_codec.codec_cap.wwe_mode   = g_record_user[i].encoder_codec.wwe_mode;
                audio_capability->codec.record_format.record_codec.codec_cap.wwe_language_mode_address   = g_record_user[i].encoder_codec.wwe_language_mode_address;
                audio_capability->codec.record_format.record_codec.codec_cap.wwe_language_mode_length   = g_record_user[i].encoder_codec.wwe_language_mode_length;
                audio_capability->codec.record_format.Reserve_callback = g_record_user[i].Reserve_callback;
                audio_capability->codec.record_format.Reserve_callback_user_data = g_record_user[i].Reserve_callback_user_data;
                break;
            }
            if (i == (AUDIO_RECORD_REGISTER_ID_TOTAL - 1) && (g_record_user[i].is_used == true)) {
                audio_capability->codec.record_format.record_codec.codec_cap.codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
                audio_capability->codec.record_format.record_codec.codec_cap.bit_rate   = ENCODER_BITRATE_16KBPS;
                audio_capability->codec.record_format.record_codec.codec_cap.wwe_mode   = WWE_MODE_NONE;
                audio_capability->codec.record_format.Reserve_callback = NULL;
                audio_capability->codec.record_format.Reserve_callback_user_data = NULL;
                LOGMSGIDI("[AMI] record_control_parameter_init fail, user more than %d", 1, AUDIO_RECORD_REGISTER_ID_TOTAL);
            }
        }
        /* LOGMSGIDI ("[RECORD][AUDIO]Init RECORD params, codec_type:%d, bit_rate:0x%x, Reserve_callback: 0x%x",
                            audio_capability->codec.record_format.record_codec.codec_cap.codec_type,
                            audio_capability->codec.record_format.record_codec.codec_cap.bit_rate,
                            audio_capability->codec.record_format.Reserve_callback);*/
    }

}

record_id_t audio_record_control_init(hal_audio_stream_in_callback_t ccni_callback,
                                      void *user_data,
                                      void *cb_handler)
{
    record_mutex_take();
    bt_sink_srv_am_id_t Aud_record_id = 0;
    /*To DO Audio Manager Prototype.*/
    if (g_aud_record_id_num >= AUDIO_RECORD_REGISTER_ID_TOTAL) {
        LOGMSGIDI("[AMI] am_audio_record_init fail user more than %d", 1, AUDIO_RECORD_REGISTER_ID_TOTAL);
        record_mutex_give();
        return RECORD_CONTROL_EXECUTION_FAIL;
    }
    Aud_record_id = ami_audio_open(RECORD, (audio_record_notify_cb)cb_handler);
    if (Aud_record_id < 0) {
        LOGMSGIDI("[AMI] am_audio_record_init fail, can not get am_id %d", 1, Aud_record_id);
        record_mutex_give();
        return Aud_record_id;
    }
    LOGMSGIDI("[AMI] am_audio_record_init", 0);
    int i;
    for (i = 0; i < AUDIO_RECORD_REGISTER_ID_TOTAL; i++) {
        if (g_record_user[i].is_used == false) {
            g_record_user[i].am_id = Aud_record_id;
            g_record_user[i].encoder_codec.codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
            g_record_user[i].encoder_codec.bit_rate   = ENCODER_BITRATE_16KBPS;
            g_record_user[i].encoder_codec.wwe_mode   = WWE_MODE_NONE;
            g_record_user[i].Reserve_callback = ccni_callback;
            g_record_user[i].Reserve_callback_user_data = user_data;
            g_record_user[i].is_used = true;
            g_aud_record_id_num++;
            break;
        }
    }
    record_mutex_give();
    return Aud_record_id;
}
record_id_t audio_record_control_enabling_encoder_init(hal_audio_stream_in_callback_t ccni_callback,
        void *user_data,
        void *cb_handler,
        record_encoder_cability_t *encoder_capability)
{
    record_mutex_take();
    record_id_t Aud_record_id = 0;
    /*To DO Audio Manager Prototype.*/
    if (g_aud_record_id_num >= AUDIO_RECORD_REGISTER_ID_TOTAL) {
        LOGMSGIDI("[AMI] am_audio_record_init fail user more than %d", 1, AUDIO_RECORD_REGISTER_ID_TOTAL);
        record_mutex_give();
        return RECORD_CONTROL_EXECUTION_FAIL;
    }

    if(((encoder_capability->codec_type != AUDIO_DSP_CODEC_TYPE_PCM)
    && (encoder_capability->codec_type != AUDIO_DSP_CODEC_TYPE_OPUS)
    && (encoder_capability->codec_type != AUDIO_DSP_CODEC_TYPE_PCM_WWE)
    && (encoder_capability->codec_type != AUDIO_DSP_CODEC_TYPE_ANC_LC)
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    && (encoder_capability->codec_type != AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF)
#else
    && (encoder_capability->codec_type != AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_SZ)
    && (encoder_capability->codec_type != AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ)
    && (encoder_capability->codec_type != AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ_FIR)
#endif
#endif
    )

    || ((encoder_capability->bit_rate != ENCODER_BITRATE_16KBPS) && (encoder_capability->bit_rate != ENCODER_BITRATE_32KBPS) && (encoder_capability->bit_rate != ENCODER_BITRATE_64KBPS))
    || ((encoder_capability->codec_type == AUDIO_DSP_CODEC_TYPE_PCM_WWE) && (encoder_capability->wwe_mode >= WWE_MODE_MAX))){
        LOGMSGIDI("[AMI] am_audio_record_init fail wrong cability:codec_type=%d, bit_rate=%d, wwe_mode=%d", 3, encoder_capability->codec_type,encoder_capability->bit_rate,encoder_capability->wwe_mode);
        record_mutex_give();
        return RECORD_CONTROL_EXECUTION_FAIL;
    }
    Aud_record_id = ami_audio_open(RECORD, (audio_record_notify_cb)cb_handler);
    if (Aud_record_id < 0) {
        LOGMSGIDI("[AMI] am_audio_record_init fail, can not get am_id %d", 1, Aud_record_id);
        record_mutex_give();
        return Aud_record_id;
    }

    LOGMSGIDI("[AMI] am_audio_record_init with encoder", 0);
    int i;
    for (i = 0; i < AUDIO_RECORD_REGISTER_ID_TOTAL; i++) {
        if (g_record_user[i].is_used == false) {
            g_record_user[i].am_id = Aud_record_id;
            g_record_user[i].encoder_codec.codec_type = encoder_capability->codec_type;
            g_record_user[i].encoder_codec.bit_rate   = encoder_capability->bit_rate;
            if(encoder_capability->codec_type == AUDIO_DSP_CODEC_TYPE_PCM_WWE) {
                g_record_user[i].encoder_codec.codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
                g_record_user[i].encoder_codec.wwe_mode   = encoder_capability->wwe_mode;
                g_record_user[i].encoder_codec.wwe_language_mode_address   = encoder_capability->wwe_language_mode_address;
                g_record_user[i].encoder_codec.wwe_language_mode_length   = encoder_capability->wwe_language_mode_length;
            } else {
                g_record_user[i].encoder_codec.codec_type = encoder_capability->codec_type;
                g_record_user[i].encoder_codec.wwe_mode   = WWE_MODE_NONE;
                g_record_user[i].encoder_codec.wwe_language_mode_address = 0xdeaddead;
                g_record_user[i].encoder_codec.wwe_language_mode_length = 0xdeaddead;
            }
            g_record_user[i].Reserve_callback = ccni_callback;
            g_record_user[i].Reserve_callback_user_data = user_data;
            g_record_user[i].is_used = true;
            g_aud_record_id_num++;
            break;
        }
    }
    record_mutex_give();
    return Aud_record_id;
}


record_control_result_t audio_record_control_deinit(record_id_t aud_id)
{
    record_mutex_take();
    LOGMSGIDI("[AMI] am_audio_record_deinit", 0);
    /*To DO Audio Manager Prototype.*/
    int i;
    for (i = 0; i < AUDIO_RECORD_REGISTER_ID_TOTAL; i++) {
        if ((g_record_user[i].am_id == aud_id) && (g_record_user[i].is_used == true)) {
            bt_sink_srv_am_result_t st = bt_sink_srv_ami_audio_close(aud_id);
            if (st == AUD_EXECUTION_SUCCESS) {
                g_record_user[i].is_used = false;
                g_aud_record_id_num--;
            }
            record_mutex_give();
            return st;
        }
    }
    LOGMSGIDI("[AMI] am_audio_record_deinit fail, the id is not inited", 0);
    record_mutex_give();
    return RECORD_CONTROL_EXECUTION_FAIL;
}

record_control_result_t audio_record_control_start(record_id_t aud_id)
{
    LOGMSGIDI("[AMI] am_audio_record_start", 0);
    bt_sink_srv_am_audio_capability_t  aud_cap;
    record_control_result_t ami_ret;

    memset(&aud_cap, 0, sizeof(bt_sink_srv_am_audio_capability_t));
    /*To DO Audio Manager Prototype.*/
    record_control_parameter_init(aud_id, &aud_cap);
    /* play will trigger the callback that send in the open function */

    ami_ret = ami_audio_play(aud_id, &aud_cap);
    return ami_ret;
}

record_control_result_t audio_record_control_stop(record_id_t aud_id)
{
    LOGMSGIDI("[AMI] am_audio_record_stop", 0);
    record_control_result_t ami_ret;

    /*To DO Audio Manager Prototype.*/
    ami_ret = ami_audio_stop(aud_id);
    return ami_ret;
}

record_control_result_t audio_record_control_read_data(void *buffer, uint32_t size)
{
    LOGMSGIDI("[AMI] am_audio_record_read_data buffer(0x%x), size(%d)", 2, buffer, size);
    record_control_result_t ami_ret;

    ami_ret = hal_audio_read_stream_in(buffer, size);
#ifdef MTK_AUDIO_DUMP_ENABLE
    if (g_dump && (ami_ret == RECORD_CONTROL_EXECUTION_SUCCESS)) {
        LOG_AUDIO_DUMP(buffer, size, VOICE_TX_MIC_3);
#ifdef MTK_AIRDUMP_EN_MIC_RECORD
        //Only For record airdump debug.
        if (g_record_airdump) {
            LOG_AUDIO_DUMP(buffer + size, size, VOICE_TX_REF);
        }
#endif
    }
#endif

    return ami_ret;
}

uint32_t audio_record_control_get_share_buf_data_byte_count(void)
{
    n9_dsp_share_info_t *p_info = hal_audio_query_record_share_info();
    uint32_t data_byte_count;

    // Check data amount
    data_byte_count = hal_audio_buf_mgm_get_data_byte_count(p_info);

    return data_byte_count;


}

record_control_result_t audio_record_control_airdump(bool Enable, uint8_t dump_scenario)
{
    if (Enable) {
        void *p_param_share;
        bool is_running;
        audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_RECORD;
        is_running = hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD);
        if (is_running) {
            // Re-entry: don't allow multiple recording
            //log_hal_msgid_info("Re-entry\r\n", 0);
        } else {
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true);
        }
        // Open playback
        mcu2dsp_open_param_t open_param;
        memset(&open_param,0,sizeof(open_param));
        // Collect parameters
        open_param.param.stream_in = STREAM_IN_AFE;
        open_param.param.stream_out = STREAM_OUT_RECORD;
        hal_audio_get_stream_in_setting_config(AU_DSP_RECORD, &open_param.stream_in_param);
#ifdef ENABLE_2A2D_TEST
        open_param.stream_in_param.afe.audio_device    = HAL_AUDIO_DEVICE_MAIN_MIC_L;
        open_param.stream_in_param.afe.audio_device1   = HAL_AUDIO_DEVICE_MAIN_MIC_R;
#else
        open_param.stream_in_param.afe.audio_device    = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
#endif
        open_param.stream_in_param.afe.memory = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3;
        open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
        open_param.stream_in_param.afe.sampling_rate   = 16000;
        open_param.stream_in_param.afe.irq_period      = 8;
        open_param.stream_in_param.afe.frame_size	   = 128; // Warning: currently fixed @ 480 in DSP
        open_param.stream_in_param.afe.frame_number    = 2;
        open_param.stream_in_param.afe.hw_gain		   = false;
        open_param.stream_out_param.record.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(msg_type);
        open_param.stream_out_param.record.p_share_info->length = open_param.stream_out_param.record.p_share_info->length / 2; /*Temp edit, need change back when stop.*/
        hal_audio_reset_share_info(open_param.stream_out_param.record.p_share_info);
        open_param.stream_out_param.record.frames_per_message = 2; // DSP triggers CCNI message after collecting this value of frames
        open_param.stream_out_param.record.bitrate = ENCODER_BITRATE_16KBPS;

        p_param_share = hal_audio_dsp_controller_put_paramter(&open_param, sizeof(mcu2dsp_open_param_t), msg_type);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_OPEN, 0xFF00 | dump_scenario, (uint32_t)p_param_share, true);

        // Start playback
        mcu2dsp_start_param_t start_param;
        // Collect parameters
        start_param.param.stream_in     = STREAM_IN_AFE;
        start_param.param.stream_out    = STREAM_OUT_RECORD;
        start_param.stream_in_param.afe.aws_flag   =  false;
        p_param_share = hal_audio_dsp_controller_put_paramter(&start_param, sizeof(mcu2dsp_open_param_t), msg_type);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_START, 0, (uint32_t)p_param_share, true);
    } else {
        //ToDo: limit the scope -- treat it as recording
        if (hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)) {
            // Stop recording
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_STOP, 0, 0, true);
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_CLOSE, 0, 0, true);
            // Open playback
            audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_RECORD;
            mcu2dsp_open_param_t open_param;
            open_param.stream_out_param.record.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(msg_type);
            open_param.stream_out_param.record.p_share_info->length = SHARE_BUFFER_RECORD_SIZE; /*Change back Here.*/

            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
        } else {
            LOGMSGIDI("Recording was not existed.", 0);
        }
    }
    return RECORD_CONTROL_EXECUTION_SUCCESS;
}

