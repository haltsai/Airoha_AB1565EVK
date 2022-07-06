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

#include "leakage_detection_control.h"
#include "hal_audio.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_audio_internal.h"
#include "audio_nvdm_common.h"
#include "audio_nvdm_coef.h"
#include "nvkey_dspalg.h"
#include "race_cmd_dsprealtime.h"
#include "bt_connection_manager_internal.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_sink_srv_common.h"
#endif

log_create_module(leakage_detection, PRINT_LEVEL_INFO);
#define LEAKAGE_DETECTION_USE_MSGID_LOG
#ifdef LEAKAGE_DETECTION_USE_MSGID_LOG
#define LEAKAGE_DETECTION_LOG_E(fmt,arg...)   LOG_MSGID_E(leakage_detection, fmt,##arg)
#define LEAKAGE_DETECTION_LOG_W(fmt,arg...)   LOG_MSGID_W(leakage_detection, fmt,##arg)
#define LEAKAGE_DETECTION_LOG_I(fmt,arg...)   LOG_MSGID_I(leakage_detection ,fmt,##arg)
#else
#define LEAKAGE_DETECTION_LOG_E(fmt,arg...)   LOG_E(leakage_detection, fmt,##arg)
#define LEAKAGE_DETECTION_LOG_W(fmt,arg...)   LOG_W(leakage_detection, fmt,##arg)
#define LEAKAGE_DETECTION_LOG_I(fmt,arg...)   LOG_I(leakage_detection ,fmt,##arg)
#endif

#ifdef MTK_ANC_V2
audio_anc_leakage_detection_sync_ctrl_t g_leakage_detection_sync_control;
static TimerHandle_t g_LD_timer = NULL;
static audio_anc_leakage_detection_callback_service_t g_LD_cb_service = {{0}};
#endif

uint32_t g_leakage_compensation_record_duration = 6000000;
uint32_t g_leakage_compensation_no_response_duration = 7000000;
extern audio_dsp_leakage_compensation_report_param_t leakage_compensation_info;
S8 g_record_lc_id = 0;
bool g_leakage_compensation_status = 0;
int16_t g_record_LC_data[LD_FRAME_SIZE];
extern uint16_t g_stream_in_code_type;
bool g_leakage_detection_mute_flag = false;
bool g_leakage_detection_prepare_flag = false;
#ifdef MTK_ANC_V2
//extern void anc_set_sync_time(uint16_t sync_time);
//extern audio_anc_control_result_t anc_sync_control(audio_anc_control_event_t event_id, audio_anc_control_cap_t* anc_cap);
//extern void anc_get_status(uint8_t *enable, audio_anc_control_filter_id_t *filter_id, audio_anc_control_type_t *type, int16_t *runtime_gain, uint8_t *support_hybrid, audio_anc_control_misc_t* control_misc);
#endif

/*===================
*                 sync control
====================*/
#ifdef MTK_ANC_V2
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_control_register_callback(audio_anc_leakage_detection_control_callback_t callback, audio_anc_leakage_detection_control_event_t event_mask, audio_anc_leakage_detection_control_callback_level_t level)
{
    U32 i;
    for (i = 0; i < LD_MAX_CALLBACK_NUM; i++)
    {
        if (g_LD_cb_service.callback_list[i] == NULL) {
            g_LD_cb_service.event_mask[i] = event_mask;
            g_LD_cb_service.cb_level[i] = level;
            g_LD_cb_service.callback_list[i] = callback;
            return AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
        }
    }
    return AUDIO_LEAKAGE_DETECTION_EXECUTION_FAIL;
}
#endif
#if 0
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_control_deregister_callback(audio_anc_leakage_detection_control_callback_t callback)
{
    U32 i;
    for (i = 0; i < LD_MAX_CALLBACK_NUM; i++)
    {
        if (g_LD_cb_service.callback_list[i] == callback) {
            g_LD_cb_service.callback_list[i] = NULL;
            g_LD_cb_service.event_mask[i] = 0;
            g_LD_cb_service.cb_level[i] = 0;
            return AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
        }
    }
    return AUDIO_LEAKAGE_DETECTION_EXECUTION_FAIL;
}
#endif

void audio_anc_leakage_detection_set_race_ch_id(uint8_t race_ch_id)
{
    extern uint8_t g_leakage_detection_race_ch_id;
    g_leakage_detection_race_ch_id = race_ch_id;
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]Partner receive race ch id:%d", 1, g_leakage_detection_race_ch_id);
}
#ifdef MTK_ANC_V2
static audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_callback_service(audio_anc_leakage_detection_control_event_t event, audio_anc_leakage_detection_execution_t result)
{
    U32 i;
    if (result == AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
        for (i = 0; i < LD_MAX_CALLBACK_NUM; i++)
        {
            if ((g_LD_cb_service.callback_list[i] != NULL) && ((g_LD_cb_service.event_mask[i] & event) != 0)) {
                g_LD_cb_service.callback_list[i](event, result);
                LEAKAGE_DETECTION_LOG_I("audio_anc_leakage_detection_callback_service %d cb:0x%x", 2, i, g_LD_cb_service.callback_list[i]);
            }
        }
    } else {
        for (i = 0; i < LD_MAX_CALLBACK_NUM; i++)
        {
            if ((g_LD_cb_service.callback_list[i] != NULL) && ((g_LD_cb_service.event_mask[i] & event) != 0) && (g_LD_cb_service.cb_level[i] == AUDIO_LEAKAGE_DETECTION_CONTROL_CB_LEVEL_ALL)) {
                g_LD_cb_service.callback_list[i](event, result);
                LEAKAGE_DETECTION_LOG_I("audio_anc_leakage_detection_callback_service %d cb:0x%x", 2, i, g_LD_cb_service.callback_list[i]);
            }
        }
    }
    return AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
}

static void audio_anc_leakage_detection_timer_callback(TimerHandle_t xTimer)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC Sync] enter LD timer callback event %d\n", 1, g_leakage_detection_sync_control.event_id);
    g_leakage_detection_sync_control.timer_available = 1;
    audio_anc_leakage_detection_control(g_leakage_detection_sync_control.event_id, &g_leakage_detection_sync_control.arg);
}

static audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_timer_service(int32_t time_duration, audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_sync_para_t *arg)
{
    audio_anc_leakage_detection_execution_t ret = AUDIO_LEAKAGE_DETECTION_EXECUTION_FAIL;
    g_leakage_detection_sync_control.event_id = event_id;
    g_leakage_detection_sync_control.arg = *arg;
    if(g_LD_timer) {
        if (time_duration > LD_SYNC_LATENCY) {
                LEAKAGE_DETECTION_LOG_E("[RECORD_LC sync] sync time may wrong(>200ms): %d ms\n",1,time_duration);
                time_duration = LD_SYNC_LATENCY;
        }
        if ((time_duration > 0) && (xTimerChangePeriod(g_LD_timer, (time_duration / portTICK_PERIOD_MS), 0) == pdPASS)) {
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC sync] start anc one-shot timer with duration %d ms\n",1,time_duration);
            g_leakage_detection_sync_control.timer_available = 0;
            ret = AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
        } else {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC sync] change timer period failed\n",0);
            ret = audio_anc_leakage_detection_control(event_id, arg);
        }
    }
    return ret;
}

audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_send_aws_mce_param(audio_anc_leakage_detection_sync_mode_t setting_mode, audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_sync_para_t *arg, uint32_t ch_info)
{
    audio_anc_leakage_detection_execution_t ret = AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
    aws_mce_report_LD_param_t param;
    bt_aws_mce_report_info_t info;
    info.module_id = BT_AWS_MCE_REPORT_MODULE_LEAKAGE_DET;
    info.param_len = sizeof(aws_mce_report_LD_param_t);
    info.param = &param;
    param.event_id = event_id;
    param.arg = *arg;
    param.ch_info = ch_info;
    param.is_sync = (setting_mode == LD_SYNC) ? true : false;
    param.sync_bt_clock = g_leakage_detection_sync_control.sync_time_bt_clock;

    if (param.is_sync) {
        if (bt_aws_mce_report_send_urgent_event(&info) != BT_STATUS_SUCCESS) {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC sync] fail to send aws mce report\n", 0);
            ret = AUDIO_LEAKAGE_DETECTION_EXECUTION_FAIL;
        }
    } else {
        if (bt_aws_mce_report_send_event(&info) != BT_STATUS_SUCCESS) {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC direct] fail to send aws mce report\n", 0);
            ret = AUDIO_LEAKAGE_DETECTION_EXECUTION_FAIL;
        }
    }
    return ret;
}

audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_control(audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_sync_para_t *arg)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC] audio_anc_leakage_detection_control, event:0x%x\n", 1, event_id);
    switch (event_id) {
        case AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_MUTE:
        case AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_UNMUTE:
        {
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_detection_control, mute:%d", 1, arg->extend_param);
            hal_audio_mute_stream_out(arg->extend_param, HAL_AUDIO_STREAM_OUT1);
            audio_anc_leakage_detection_set_mute_status(arg->extend_param);
            audio_anc_leakage_detection_callback_service(event_id, AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS);
            break;
        }

        case AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_START:
        {
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC] anc leakage detection start in audio_anc_leakage_detection_control", 0);
            audio_anc_leakage_compensation_set_status(1);
            anc_leakage_compensation_callback_t callback = (anc_leakage_compensation_callback_t)(arg->extend_param);
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC][debug]callback:0x%x", 1, callback);
            if (callback == NULL) {
                audio_anc_leakage_compensation_start(anc_leakage_detection_racecmd_callback);
            } else {
                audio_anc_leakage_compensation_start((anc_leakage_compensation_callback_t)callback);
            }
            break;
        }

        case AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_STOP:
        {
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC] anc leakage detection stop in audio_anc_leakage_detection_control, enabled:%d",1,audio_anc_leakage_compensation_get_status());
            if (audio_anc_leakage_compensation_get_status() == 1) {
                audio_anc_leakage_compensation_terminate();
            }
            break;
        }

        default:
            return AUDIO_LEAKAGE_DETECTION_EXECUTION_FAIL;
    }
    return AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
}

audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_sync_control(audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_sync_para_t *arg)
{
    audio_anc_leakage_detection_execution_t ret = AUDIO_LEAKAGE_DETECTION_EXECUTION_NONE;
    uint32_t sync_mode = LD_DIRECT;
    if(arg->target_device == TO_ITSELF){
        ret = audio_anc_leakage_detection_control(event_id, arg);
    } else {
        if((bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) && (bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {//Agent (Transmitter)
            sync_mode = LD_SYNC;
        }
        if ((g_leakage_detection_sync_control.sync_time != 0) && (bt_connection_manager_get_aws_link_type() == BT_CONNECTION_MANAGER_AWS_LINK_SPECIAL)) {
            /* R-note: no Smart Phone, BT clock agent and partner un-sync. */
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC] is special link case, agent delay %d ms to apply and tell partner to apply directly\n", 1, LD_SYNC_LATENCY_UNDER_SPECIAL_LINK);
            ret = audio_anc_leakage_detection_send_aws_mce_param(LD_DIRECT, event_id, arg, 0);
            ret |= audio_anc_leakage_detection_timer_service(LD_SYNC_LATENCY_UNDER_SPECIAL_LINK, event_id, arg);
        } else if(sync_mode == LD_DIRECT) {
            ret = audio_anc_leakage_detection_control(event_id, arg);
        } else {
            bt_status_t bt_status;
            bt_clock_t target_clk;
            int32_t duration;
            uint32_t loop = 0;
            if (g_leakage_detection_sync_control.sync_time == 0) {
                ret = audio_anc_leakage_detection_send_aws_mce_param(LD_DIRECT, event_id, arg, 0);
                ret |= audio_anc_leakage_detection_control(event_id, arg);
            } else {
                while (!g_leakage_detection_sync_control.timer_available) {
                    vTaskDelay(pdMS_TO_TICKS(100));
                    loop++;
                    LEAKAGE_DETECTION_LOG_I("[RECORD_LC Sync] wait timer to be available : %d ms !\n", 1, loop*100);
                    if(loop >= 5) {
                        LEAKAGE_DETECTION_LOG_E("[RECORD_LC Sync] timer is unavailable too long !\n", 0);
                        g_leakage_detection_sync_control.timer_available = 1;
                    }
                }
                bt_status = bt_sink_srv_bt_clock_addition(&target_clk, NULL, (uint32_t)g_leakage_detection_sync_control.sync_time*1000);
                if (!g_leakage_detection_sync_control.sync_time) {
                    sync_mode = LD_DIRECT;
                }
                g_leakage_detection_sync_control.sync_time_bt_clock = target_clk;
                ret = audio_anc_leakage_detection_send_aws_mce_param(sync_mode, event_id, arg, 0);
                if ((bt_status == BT_STATUS_SUCCESS) && (bt_sink_srv_bt_clock_get_duration(&target_clk, NULL, &duration) == BT_STATUS_SUCCESS)) {
                    ret |= audio_anc_leakage_detection_timer_service(duration/1000, event_id, arg);
                } else {
                    ret |= audio_anc_leakage_detection_control(event_id, arg);
                }
            }
        }
    }
    return ret;
}

static void bt_aws_mce_report_LD_callback(bt_aws_mce_report_info_t *para)
{
    aws_mce_report_LD_param_t *param = (aws_mce_report_LD_param_t *)para->param;
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]AWS MCE Report: para(0x%x): 0x%x 0x%x 0x%x, event:0x%x\n", 5, para, para->module_id,para->param_len,para->param, param->event_id);
    if ((para->module_id == BT_AWS_MCE_REPORT_MODULE_LEAKAGE_DET) && (para->param != NULL)) {
        if ((param->event_id != AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_RACE_CH_ID)
            && (param->event_id != AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_MUTE)
            && (param->event_id != AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_START)
            && (param->event_id != AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_STOP)
            && (param->event_id != AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_UNMUTE))
        {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC]AWS MCE Report: Wrong event:0x%x\n", 1, param->event_id);
            return;
        }
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

        if (BT_AWS_MCE_ROLE_AGENT == role) /* Agent handle */
        {
            if (param->event_id == AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_START) {
                anc_leakage_detection_racecmd_response((uint16_t)(param->arg.extend_param), 1);
            }
        } else { /* Partner handle */
            if (param->event_id == AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_RACE_CH_ID) {
                audio_anc_leakage_detection_set_race_ch_id((uint8_t)param->arg.extend_param);

            } else {
                if (param->is_sync) {
                    int32_t get_duration;
                    bt_sink_srv_bt_clock_get_duration(&(param->sync_bt_clock), NULL, &get_duration);
                    LEAKAGE_DETECTION_LOG_I("[RECORD_LC Sync] Partner target_clk:0x%x, 0x%x, duration:%d us \n", 3, param->sync_bt_clock.nclk, param->sync_bt_clock.nclk_intra, get_duration);
                    audio_anc_leakage_detection_timer_service((int32_t)get_duration/1000, (audio_anc_leakage_detection_control_event_t)param->event_id, &param->arg);
                } else {
                    audio_anc_leakage_detection_control((audio_anc_leakage_detection_control_event_t)param->event_id, &param->arg);
                }
            }
        }
    }
}
#endif

/*===================
*                 Flow control
====================*/

void audio_anc_set_leakage_compensation_parameters_nvdm(anc_leakage_compensation_parameters_nvdm_t *leakage_compensation_param_ptr)
{
    sysram_status_t status;
    status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_LEAKAGE_COMPENSATION, (uint8_t *)leakage_compensation_param_ptr, sizeof(anc_leakage_compensation_parameters_nvdm_t));
    if (status != NVDM_STATUS_NAT_OK) {
        LEAKAGE_DETECTION_LOG_E("[RECORD_LC]audio_anc_set_leakage_compensation_parameters_nvdm FAIL, status:%d", 1, status);
    }
}

int32_t audio_anc_read_leakage_compensation_parameters_nvdm(void)
{
    uint32_t length = sizeof(anc_leakage_compensation_parameters_nvdm_t);
    anc_leakage_compensation_parameters_nvdm_t leakage_compensation_param;
    anc_leakage_compensation_parameters_mcu2dsp_t leakage_compensation_param_mcu2dsp;
    sysram_status_t status;
    void *p_param_share;

    memset(&leakage_compensation_param, 0, sizeof(anc_leakage_compensation_parameters_nvdm_t));
    memset(&leakage_compensation_param_mcu2dsp, 0, sizeof(anc_leakage_compensation_parameters_mcu2dsp_t));

    status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_LEAKAGE_COMPENSATION, (uint8_t *)&leakage_compensation_param, &length);
    if (status == NVDM_STATUS_NAT_OK) {
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]read nvkey ld_thrd:0x%x, RXIN_TXREF_DELAY:0x%x, DIGITAL_GAIN:0x%x", 3, leakage_compensation_param.ld_thrd, leakage_compensation_param.RXIN_TXREF_DELAY, leakage_compensation_param.DIGITAL_GAIN);
        memcpy(&leakage_compensation_param_mcu2dsp, &leakage_compensation_param, length);
        leakage_compensation_param_mcu2dsp.report_thd = g_leakage_compensation_record_duration;
        leakage_compensation_param_mcu2dsp.no_response_thd = g_leakage_compensation_no_response_duration;
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]send param to dsp, ld_thrd:0x%x, RXIN_TXREF_DELAY:0x%x, DIGITAL_GAIN:0x%x, report_thd:%u, no_response_thd:%u", 5, leakage_compensation_param_mcu2dsp.nvdm_param.ld_thrd, leakage_compensation_param_mcu2dsp.nvdm_param.RXIN_TXREF_DELAY, leakage_compensation_param_mcu2dsp.nvdm_param.DIGITAL_GAIN, leakage_compensation_param_mcu2dsp.report_thd, leakage_compensation_param_mcu2dsp.no_response_thd);

        p_param_share = hal_audio_dsp_controller_put_paramter( &leakage_compensation_param_mcu2dsp, sizeof(anc_leakage_compensation_parameters_mcu2dsp_t), AUDIO_MESSAGE_TYPE_RECORD);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_LC_SET_PARAM, 0, (uint32_t)p_param_share, true);
    } else {
        LEAKAGE_DETECTION_LOG_E("[RECORD_LC]read nvkey id 0x%x fail, status:%d", 2, NVKEY_DSP_PARA_LEAKAGE_COMPENSATION, status);
    }
    return ((int32_t)status);
}

void audio_anc_leakage_compensation_CCNI_callback(hal_audio_event_t event, void *data)
{
    switch(event){
        case HAL_AUDIO_EVENT_ERROR:
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC][CALLBACK]HAL_AUDIO_EVENT_ERROR\r\n", 0);
            break;

        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:

        if(g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_ANC_LC){
            for(uint16_t i=0; i < 4 ; i++)
            {
                if (RECORD_CONTROL_EXECUTION_SUCCESS == audio_record_control_read_data(&g_record_LC_data, (sizeof(int16_t) * LD_FRAME_SIZE))){
                    //LOG_AUDIO_DUMP(g_record_Rdemo_data, sizeof(int16_t) * 128, VOICE_TX_MIC_3);
                }else {
                    LEAKAGE_DETECTION_LOG_I("[RECORD_LC][CALLBACK]read stream in failed\r\n", 0);
                }
            }
        }

            break;
    }
}

void audio_anc_leakage_compensation_AM_notify_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]AM_CB, aud_id:%x, msg_id:%x, sub_msg:%x", aud_id, msg_id, sub_msg);
    if (msg_id == AUD_SINK_OPEN_CODEC) {
        if(sub_msg == AUD_CMD_COMPLETE){
            //Start Record Success
        } else if (sub_msg == AUD_CMD_FAILURE) {
            //Start Record Fail
        } else {
            //[error]
        }
    }else if(msg_id == AUD_SELF_CMD_REQ){
        if(sub_msg == AUD_CMD_COMPLETE){
            //Stop Record Request Success
        } else if (sub_msg == AUD_CMD_FAILURE) {
            //Start Record Request Fail because HFP exist. /*Reject Request.*/
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC]AM_CB, Start Record Request Fail because higher priority scenario exists", 0);
            audio_anc_leakage_compensation_terminate();
        } else {
            //[error]
        }
    }else if(msg_id == AUD_SUSPEND_BY_IND){
        if(sub_msg == AUD_SUSPEND_BY_HFP){
            //Suspend record because HFP interrupt.
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC]AM_CB, AUD_SUSPEND_BY_HFP", 0);
            audio_anc_leakage_compensation_terminate();
        } else {
            //[error]
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC]AM_CB, AUD_SUSPEND_BY_IND", 0);
        }
    }else{
        //[error]
    }

}

void audio_anc_leakage_compensation_cb(uint16_t leakage_status)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC] Warning! Leakage compensation middleware callback", 0);
    audio_anc_leakage_compensation_stop();
}

void audio_anc_leakage_compensation_anc_callback(audio_anc_control_event_t event_id, audio_anc_control_result_t result)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC] ANC callback - event:%d result:%d",2, event_id, result);
#ifndef MTK_ANC_V2
    switch (event_id) {
        case ANC_CONTROL_EVENT_OFF: {
            anc_set_sync_time(1500); //enlarge sync time for avrcp suspend
            anc_control_result_t anc_res = anc_sync_control(ANC_CONTROL_EVENT_LD_START, 0, (anc_control_callback_t)leakage_compensation_info.api_callback);
            anc_set_sync_time(ANC_SYNC_LATENCY);

            if (anc_res != AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) {

                LEAKAGE_DETECTION_LOG_I("[RECORD_LC] anc_sync_control fail:%d",1, anc_res);
                audio_anc_leakage_compensation_terminate();
            } else {
                //open vp after anc off
                if (leakage_compensation_info.vp_start_callback != NULL) {
                    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_compensation_info.vp_start_callback", 0);
                    leakage_compensation_info.vp_start_callback(0);
                } else {
                        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_compensation_info.vp_start_callback == NULL", 0);
                }
            }
            break;
        }

        case ANC_CONTROL_EVENT_ON: {

        }
    }

#else
    switch (event_id) {
        case AUDIO_ANC_CONTROL_EVENT_OFF: {
            audio_anc_leakage_detection_execution_t anc_res;
            anc_res = (audio_anc_leakage_detection_execution_t)audio_anc_control_deregister_callback((audio_anc_control_callback_t) audio_anc_leakage_compensation_anc_callback);
            if (anc_res != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
                LEAKAGE_DETECTION_LOG_E("[RECORD_LC]audio_anc_deregister_callback Fail", 0);
            }

            audio_anc_leakage_detection_sync_para_t arg;
            arg.target_device = TO_BOTH;
            arg.extend_param = 0;
            anc_res = audio_anc_leakage_detection_sync_control(AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_START, &arg);

            if (anc_res != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {

                LEAKAGE_DETECTION_LOG_I("[RECORD_LC] anc_sync_control fail:%d",1, anc_res);
                audio_anc_leakage_compensation_terminate();
            } else {
                //open vp after anc off
                if (leakage_compensation_info.vp_start_callback != NULL) {
                    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_compensation_info.vp_start_callback", 0);
                    leakage_compensation_info.vp_start_callback(0);
                } else {
                        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_compensation_info.vp_start_callback == NULL", 0);
                }
            }
            break;
        }

        case AUDIO_ANC_CONTROL_EVENT_ON: {
            audio_anc_leakage_detection_execution_t anc_res = (audio_anc_leakage_detection_execution_t)audio_anc_control_deregister_callback((audio_anc_control_callback_t) audio_anc_leakage_compensation_anc_callback);
            if (anc_res != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
                LEAKAGE_DETECTION_LOG_E("[RECORD_LC]audio_anc_deregister_callback Fail", 0);
            }
            //unmute dl
            anc_res = audio_anc_leakage_detection_mute_music(false);
            if (anc_res != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
                LEAKAGE_DETECTION_LOG_E("[RECORD_LC]audio_anc_leakage_detection_mute_music return Fail", 0);
            }
        }

    }
#endif

}

void audio_anc_leakage_compensation_start(anc_leakage_compensation_callback_t callback)
{
    int32_t status;

    status = audio_anc_read_leakage_compensation_parameters_nvdm();

    if (1) {//status == 0) {
        if (leakage_compensation_info.enable == 1) {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC]Leakage compensation start error: already enable, STOP first", 0);
            leakage_compensation_info.api_callback = audio_anc_leakage_compensation_cb;
            LEAKAGE_DETECTION_LOG_I("[RECORD_LC][debug]audio_anc_leakage_compensation_start, leakage_compensation_info.api_callback:0x%x", 1, leakage_compensation_info.api_callback);
            audio_anc_leakage_compensation_stop();
        }

        //initialize
        leakage_compensation_info.enable = 1;
        leakage_compensation_info.calibration_status = 0;
        leakage_compensation_info.wz_set = 0;
        leakage_compensation_info.api_callback = (callback == NULL) ? audio_anc_leakage_compensation_cb : callback;
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC][debug]audio_anc_leakage_compensation_start, leakage_compensation_info.api_callback:0x%x, callback:0x%x", 2, leakage_compensation_info.api_callback, callback);
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]Leakage compensation start, g_reocrd_id %d", 1, g_record_lc_id);

        /*Request AM*/
        if(g_record_lc_id == 0){

            record_encoder_cability_t encoder_capability;
            encoder_capability.codec_type = AUDIO_DSP_CODEC_TYPE_ANC_LC;
            encoder_capability.bit_rate = ENCODER_BITRATE_16KBPS;
            encoder_capability.wwe_mode = WWE_MODE_NONE;
            g_record_lc_id = audio_record_control_enabling_encoder_init(audio_anc_leakage_compensation_CCNI_callback, NULL, audio_anc_leakage_compensation_AM_notify_callback,&encoder_capability);
            if (g_record_lc_id < 0) {
                LEAKAGE_DETECTION_LOG_E("[RECORD_LC]Leakage compensation start, can not get g_reocrd_id %d", 1, g_record_lc_id);
                leakage_compensation_info.api_callback(5);
                return;
            } else {
                LEAKAGE_DETECTION_LOG_I("[RECORD_LC]Leakage compensation start, codec_type:%d, g_reocrd_id %d", 2, encoder_capability.codec_type, g_record_lc_id);
            }
        }

        if (audio_record_control_start(g_record_lc_id) != RECORD_CONTROL_EXECUTION_SUCCESS) {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC]Leakage compensation start fail, audio_record_control_start fail", 0);
            audio_anc_leakage_compensation_terminate();
        }
    } else {
        LEAKAGE_DETECTION_LOG_E("[RECORD_LC]Leakage compensation start fail, nvdm_status:%d", 2, status);
        leakage_compensation_info.api_callback(5);
    }
}

void audio_anc_leakage_compensation_stop(void)
{
    leakage_compensation_info.enable = 0;
    leakage_compensation_info.api_callback = audio_anc_leakage_compensation_cb;
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC][debug]audio_anc_leakage_compensation_stop, leakage_compensation_info.api_callback:0x%x\r\n", 1, leakage_compensation_info.api_callback);

    if(g_record_lc_id != 0) {
        audio_record_control_stop(g_record_lc_id);
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_compensation_stop, status:%d, w_set:%d, g_record_lc_id:%d\r\n", 3, leakage_compensation_info.calibration_status, leakage_compensation_info.wz_set, g_record_lc_id);
    }
    audio_anc_leakage_compensation_set_status(0);/*set 0 before turn on ANC*/
    audio_anc_leakage_detection_callback_service(AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_STOP, AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS);
}

void audio_anc_leakage_compensation_set_status(bool status)
{
    g_leakage_compensation_status = status;
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_compensation_set_status = %d", 1, status);
}

bool audio_anc_leakage_compensation_get_status(void)
{
    return g_leakage_compensation_status;
}

void audio_anc_leakage_compensation_terminate(void)
{
    if (leakage_compensation_info.api_callback != NULL) {
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC][debug]audio_anc_leakage_compensation_terminate, leakage_compensation_info.api_callback:0x%x\r\n", 1, leakage_compensation_info.api_callback);
        leakage_compensation_info.api_callback(LD_STATUS_TERMINATE);
    } else {
        audio_anc_leakage_compensation_cb(LD_STATUS_TERMINATE);
    }
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC] terminated by higher priority scenario", 0);

    //unmute music
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC] unmute music in terminal case.", 0);
    hal_audio_mute_stream_out(false, HAL_AUDIO_STREAM_OUT1);
    audio_anc_leakage_detection_set_mute_status(false);
    g_leakage_detection_prepare_flag = false;
}

#ifdef MTK_ANC_V2
void audio_anc_leakage_detection_mute_music_callback(audio_anc_leakage_detection_control_event_t event_id, audio_anc_leakage_detection_execution_t result)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC] mute music callback - event:%d result:%d",2, event_id, result);

    if (event_id == AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_MUTE) {

        LEAKAGE_DETECTION_LOG_I("[RECORD_LC][debug]audio_anc_leakage_detection_mute_music_callback, leakage_compensation_info.api_callback:0x%x", 1, leakage_compensation_info.api_callback);

        if((bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) && (bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {//Agent (Transmitter)

            audio_anc_leakage_detection_execution_t anc_res = audio_anc_leakage_detection_start(leakage_compensation_info.api_callback);
            if (anc_res != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
                LEAKAGE_DETECTION_LOG_E("[RECORD_LC] audio_anc_leakage_detection_start fail:%d",1, anc_res);
                audio_anc_leakage_compensation_terminate();
            }
        }
        g_leakage_detection_prepare_flag = false;
    } else if (event_id == AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_UNMUTE) {
    }
}

audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_mute_music(bool is_mute)
{
    audio_anc_leakage_detection_execution_t anc_res = AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC] audio_anc_leakage_detection_mute_music, mute:%d", 1, is_mute);

    audio_anc_leakage_detection_sync_para_t arg;
    arg.target_device = TO_BOTH;
    arg.extend_param = (uint32_t)is_mute;
    if (is_mute) {
        anc_res = audio_anc_leakage_detection_sync_control(AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_MUTE, &arg);
    } else {
        anc_res = audio_anc_leakage_detection_sync_control(AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_UNMUTE, &arg);
    }

    return anc_res;
}

audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_prepare(anc_leakage_compensation_callback_t callback)
{
    audio_anc_leakage_detection_execution_t anc_res = AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
    if (audio_anc_leakage_compensation_get_status() || audio_anc_leakage_detection_get_mute_status() || g_leakage_detection_prepare_flag) {
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_detection_start, reject due to LD is happenings, g_leakage_compensation_status:%d, mute_flag:%d, prepare_flag:%d", 3, g_leakage_compensation_status, g_leakage_detection_mute_flag, g_leakage_detection_prepare_flag);
        anc_res = AUDIO_LEAKAGE_DETECTION_EXECUTION_NOT_ALLOWED;
        return anc_res;
    }

#ifdef MTK_AWS_MCE_ENABLE
    if((bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) && (bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {//Agent (Transmitter)
        g_leakage_detection_prepare_flag = true;
        leakage_compensation_info.api_callback = (callback == NULL) ? audio_anc_leakage_compensation_cb : callback;
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC][debug]audio_anc_leakage_detection_prepare, leakage_compensation_info.api_callback:0x%x, callback:0x%x", 2, leakage_compensation_info.api_callback, callback);

        anc_res = audio_anc_leakage_detection_mute_music(true);

    } else {
        //need to pairing to start detection
        return AUDIO_ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
#else
    return AUDIO_ANC_CONTROL_EXECUTION_NOT_ALLOWED;
#endif

    return anc_res;
}
#endif
#ifdef MTK_ANC_V2
void audio_anc_leakage_detection_init(void)
{
    memset(&g_leakage_detection_sync_control, 0, sizeof(audio_anc_leakage_detection_sync_ctrl_t));
    g_leakage_detection_sync_control.sync_time = LD_SYNC_LATENCY;
    g_leakage_detection_sync_control.timer_available = 1;

    if (bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_LEAKAGE_DET, bt_aws_mce_report_LD_callback) != BT_STATUS_SUCCESS) {
        LEAKAGE_DETECTION_LOG_E("[RECORD_LC]failed to register aws mce report callback\r\n", 0);
    }
    if(g_LD_timer == NULL) {
        g_LD_timer = xTimerCreate("LDTimer", (LD_SYNC_LATENCY / portTICK_PERIOD_MS), pdFALSE, 0, audio_anc_leakage_detection_timer_callback);
        if(g_LD_timer == NULL) {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC]leakage_detection_init: create timer failed\n", 0);
        }
    }

    audio_anc_leakage_detection_execution_t anc_res = AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
    anc_res |= audio_anc_leakage_detection_control_register_callback((audio_anc_leakage_detection_control_callback_t) audio_anc_leakage_detection_mute_music_callback, AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_UNMUTE | AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_MUSIC_MUTE, AUDIO_LEAKAGE_DETECTION_CONTROL_CB_LEVEL_ALL);
    if (anc_res != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
        LEAKAGE_DETECTION_LOG_E("[RECORD_LC]audio_anc_control_register_callback FAIL", 0);
    }

    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_detection_init done\n", 0);
}
#endif
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_start(anc_leakage_compensation_callback_t callback)
{
    audio_anc_leakage_detection_execution_t anc_res = AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS;
#ifdef MTK_AWS_MCE_ENABLE
    if((bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) && (bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {//Agent (Transmitter)

        //move register callback here because LD start after ANC disable callback
        leakage_compensation_info.api_callback = (callback == NULL) ? audio_anc_leakage_compensation_cb : callback;
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC][debug]audio_anc_leakage_detection_start, leakage_compensation_info.api_callback:0x%x, callback:0x%x", 2, leakage_compensation_info.api_callback, callback);

        /*turn off ANC if needed*/
#ifndef MTK_ANC_V2
        uint8_t anc_status;
        int16_t anc_runtime_gain;
        anc_get_status(&leakage_compensation_info.anc_enable, &leakage_compensation_info.anc_runtime_info, NULL);
        anc_status = (uint8_t)(leakage_compensation_info.anc_runtime_info & ANC_FILTER_TYPE_MASK);
        anc_runtime_gain = (uint16_t)(leakage_compensation_info.anc_runtime_info & 0xFFFF0000);
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_detection_start, anc_enable = %d, anc_status = 0x%x, runtime_gain = %d", 3, leakage_compensation_info.anc_enable, anc_status, anc_runtime_gain);

        if (leakage_compensation_info.anc_enable) {
            audio_anc_disable(audio_anc_leakage_compensation_anc_callback);
            return anc_res;
        } else {
            anc_set_sync_time(1500);; //enlarge sync time for avrcp suspend
            anc_res = anc_sync_control(ANC_CONTROL_EVENT_LD_START, 0, (anc_control_callback_t)callback);
            anc_set_sync_time(ANC_SYNC_LATENCY);
            if (anc_res == AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
                //open vp
                if (leakage_compensation_info.vp_start_callback != NULL) {
                    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_compensation_info.vp_start_callback", 0);
                    leakage_compensation_info.vp_start_callback(0);
                } else {
                    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_compensation_info.vp_start_callback == NULL", 0);
                }
            }
            return anc_res;
        }

#else
        uint8_t                                 anc_enable;
        audio_anc_control_filter_id_t           anc_current_filter_id;
        audio_anc_control_type_t                anc_current_type;
        audio_anc_control_get_status(&anc_enable, &anc_current_filter_id, &anc_current_type, NULL, NULL, NULL);
        leakage_compensation_info.anc_enable = anc_enable;
        leakage_compensation_info.anc_filter_id = (uint8_t)anc_current_filter_id;
        leakage_compensation_info.anc_type = (uint8_t)anc_current_type;
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_detection_start, anc_enable = %d, anc_filter_id = 0x%x, anc_type = %d", 3, leakage_compensation_info.anc_enable, leakage_compensation_info.anc_filter_id, leakage_compensation_info.anc_type);

        if (leakage_compensation_info.anc_enable) {
            anc_res = (audio_anc_leakage_detection_execution_t)audio_anc_control_register_callback((audio_anc_control_callback_t) audio_anc_leakage_compensation_anc_callback, AUDIO_ANC_CONTROL_EVENT_OFF, CB_LEVEL_ALL);
            if (anc_res != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
                LEAKAGE_DETECTION_LOG_E("[RECORD_LC]audio_anc_register_callback Fail", 0);
                return anc_res;
            }
            anc_res = audio_anc_control_disable(NULL);
            return anc_res;
        } else {

                audio_anc_leakage_detection_sync_para_t arg;
                arg.target_device = TO_BOTH;
                arg.extend_param = (uint32_t)callback;
                anc_res = audio_anc_leakage_detection_sync_control(AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_START, &arg);
                if (anc_res == AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
                    //open vp
                    if (leakage_compensation_info.vp_start_callback != NULL) {
                        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_compensation_info.vp_start_callback", 0);
                        leakage_compensation_info.vp_start_callback(0);
                    } else {
                        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]leakage_compensation_info.vp_start_callback == NULL", 0);
                    }
                }
            return anc_res;
        }

#endif

    } else {
        //need to pairing to start detection
        return AUDIO_ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
#else
    return AUDIO_ANC_CONTROL_EXECUTION_NOT_ALLOWED;
#endif
}

audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_stop(void)
{
#ifdef MTK_AWS_MCE_ENABLE
	#ifdef MTK_ANC_V2
    audio_anc_leakage_detection_sync_para_t arg;
    arg.target_device = TO_BOTH;
    arg.extend_param = 0;
    return audio_anc_leakage_detection_sync_control(AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_STOP, &arg);
	#else
    return anc_sync_control(ANC_CONTROL_EVENT_LD_STOP, 0, 0);
    #endif
#else
    return ANC_CONTROL_EXECUTION_NOT_ALLOWED;
#endif
}

void audio_anc_leakage_detection_resume_dl_callback(void)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_detection_resume_dl_callback", 0);
    audio_anc_leakage_detection_resume_anc();
}
audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_resume_dl(void)
{

#ifdef MTK_AWS_MCE_ENABLE
	#ifndef MTK_ANC_V2

    return anc_sync_control(ANC_CONTROL_EVENT_LD_DL_RESUME, 0, (anc_control_callback_t)audio_anc_leakage_detection_resume_dl_callback);

    #else

    audio_anc_leakage_detection_execution_t anc_ret;
    anc_ret = audio_anc_leakage_detection_resume_anc();
    if (anc_ret != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
        //unmute music directly
        audio_anc_leakage_detection_mute_music(false);
    }
    return anc_ret;

    #endif
#else
    return AUDIO_LEAKAGE_DETECTION_EXECUTION_NOT_ALLOWED;
#endif

}

void audio_anc_leakage_detection_register_vp_start_callback(anc_leakage_compensation_callback_t callback)
{
#ifdef MTK_ANC_V2
    audio_anc_leakage_detection_init();
#endif
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_detection_register_vp_start_callback", 0);
    leakage_compensation_info.vp_start_callback = callback;
}

audio_anc_leakage_detection_execution_t audio_anc_leakage_detection_resume_anc(void)
{

    /*turn on ANC if needed*/
#ifndef MTK_ANC_V2
    audio_anc_leakage_detection_execution_t anc_ret = AUDIO_LEAKAGE_DETECTION_EXECUTION_NONE;
    uint8_t anc_status = (uint8_t)(leakage_compensation_info.anc_runtime_info & ANC_FILTER_TYPE_MASK);
    int16_t anc_runtime_gain = (uint16_t)(leakage_compensation_info.anc_runtime_info & 0xFFFF0000);

    if (leakage_compensation_info.anc_enable) {
        anc_ret = (audio_anc_leakage_detection_execution_t)audio_anc_enable(anc_status, anc_runtime_gain, audio_anc_leakage_compensation_anc_callback);
        if (anc_ret != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC]audio_anc_register_callback Fail", 0);
            return anc_ret;
        }
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]recover ANC, ret:%d, filter_type:%d, runtime_gain:%d", 3, anc_ret, anc_status, anc_runtime_gain);
    }

#else
    audio_anc_leakage_detection_execution_t anc_ret = AUDIO_LEAKAGE_DETECTION_EXECUTION_NONE;

    if (leakage_compensation_info.anc_enable) {
        anc_ret = (audio_anc_leakage_detection_execution_t)audio_anc_control_register_callback((audio_anc_control_callback_t) audio_anc_leakage_compensation_anc_callback, AUDIO_ANC_CONTROL_EVENT_ON, CB_LEVEL_ALL);
        if (anc_ret != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC]audio_anc_register_callback Fail", 0);
            //unmute dl
//            audio_anc_leakage_detection_mute_music(false);
            return anc_ret;
        }
        anc_ret = (audio_anc_leakage_detection_execution_t)audio_anc_control_enable(leakage_compensation_info.anc_filter_id, leakage_compensation_info.anc_type, NULL);
        LEAKAGE_DETECTION_LOG_I("[RECORD_LC]recover ANC, ret:%d, anc_filter_id:%d, anc_type:%d", 3, anc_ret, leakage_compensation_info.anc_filter_id, leakage_compensation_info.anc_type);
    }
#endif

    return anc_ret;

}

void audio_anc_leakage_detection_send_aws_mce_race_ch_id(uint8_t race_ch_id)
{
    #ifdef MTK_ANC_V2
        audio_anc_leakage_detection_execution_t res = AUDIO_LEAKAGE_DETECTION_EXECUTION_NONE;
        audio_anc_leakage_detection_sync_para_t arg;
        arg.target_device = TO_THEOTHER;
        arg.extend_param = (uint32_t)race_ch_id;
        res = audio_anc_leakage_detection_send_aws_mce_param(LD_DIRECT, AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_RACE_CH_ID, &arg, 0);
        if (res != AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
            LEAKAGE_DETECTION_LOG_E("[RECORD_LC]send race ch id Fail", 0);
        }
    #else
//        audio_anc_control_result_t anc_ret = AUDIO_ANC_CONTROL_EXECUTION_SUCCESS;

//        audio_anc_control_cap_t anc_cap;
//        anc_cap.target_device = TO_THEOTHER;
//        anc_cap.control_misc.extend_use_parameters = (uint32_t)race_ch_id;
//        //send callback to partner
//        anc_ret = anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_LD_RACE_CH_ID, &anc_cap, 0);
//        if (anc_ret != AUDIO_ANC_CONTROL_EXECUTION_SUCCESS) {
//            LEAKAGE_DETECTION_LOG_E("[RECORD_LC]send race ch id Fail", 0);
//        }
    #endif
}

void audio_anc_leakage_detection_set_mute_status(bool mute)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_detection_set_mute_status, mute:%d\r\n", 1, mute);
    g_leakage_detection_mute_flag = mute;
}

bool audio_anc_leakage_detection_get_mute_status(void)
{
    LEAKAGE_DETECTION_LOG_I("[RECORD_LC]audio_anc_leakage_detection_get_mute_status, mute:%d\r\n", 1, g_leakage_detection_mute_flag);
    return g_leakage_detection_mute_flag;
}

