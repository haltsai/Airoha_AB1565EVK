/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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
#include "anc_control.h"

#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_common.h"
#include "bt_connection_manager_internal.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_audio_internal.h"
#include "hal_clock.h"
#include "hal_pmu.h"
#include "hal_resource_assignment.h"

#include "audio_nvdm_common.h"
#include "audio_nvdm_coef.h"
#include "nvkey_dspalg.h"
#include "race_cmd_dsprealtime.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif
#include "exception_handler.h"
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#include "hal_dvfs_internal.h"
#endif

#define _UNUSED(x)  ((void)(x))
log_create_module(anc, PRINT_LEVEL_INFO);
#define ANC_USE_MSGID_LOG
#ifdef ANC_USE_MSGID_LOG
#define ANC_LOG_E(fmt,arg...)   LOG_MSGID_E(anc, fmt,##arg)
#define ANC_LOG_W(fmt,arg...)   LOG_MSGID_W(anc, fmt,##arg)
#define ANC_LOG_I(fmt,arg...)   LOG_MSGID_I(anc ,fmt,##arg)
#else
#define ANC_LOG_E(fmt,arg...)   LOG_E(anc, fmt,##arg)
#define ANC_LOG_W(fmt,arg...)   LOG_W(anc, fmt,##arg)
#define ANC_LOG_I(fmt,arg...)   LOG_I(anc ,fmt,##arg)
#endif

#define ANC_CHIP_ID_REG     (0xA2000000)
#define ANC_CHIP_ID_E2E3    (0xCA01)
#define ANC_CHIP_ID_E4      (0xCA02)
#define ANC_EFUSE_REG       (0xA20A0148)
#define ANC_EFUSE_MASK      (0x1 << 1)

#define HOWLING_TOLERANCE_MIN       (2)

extern void ami_hal_audio_status_set_running_flag(audio_message_type_t type, bool is_running);

#ifdef MTK_LEAKAGE_DETECTION_ENABLE
extern bool audio_anc_leakage_compensation_get_status(void);
extern void audio_anc_leakage_compensation_set_status(bool status);
extern void audio_anc_leakage_compensation_start(anc_leakage_compensation_callback_t callback);
extern void audio_anc_leakage_compensation_terminate(void);
#endif


anc_control_t g_anc_control;
static anc_callback_service_t g_anc_cb_service = {{0}};
#ifdef HAL_DVFS_MODULE_ENABLED
static uint32_t g_anc_debug = 0;
#endif

#ifdef FREERTOS_ENABLE
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
static SemaphoreHandle_t g_anc_mutex = NULL;
#ifdef MTK_AWS_MCE_ENABLE
static TimerHandle_t g_anc_timer = NULL;
#endif
#if (ANC_STOP_LATENCY != 0)
static TimerHandle_t g_anc_stop_dly_timer = NULL;
#endif

#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
#include "hal_audio.h"
#include "audio_nvdm_coef.h"

#define UTFF_DEBUG_PRINT

uint8_t g_record_user_trigger_ff_id = 0;
anc_user_trigger_ff_param_t g_user_trigger_ff_info;
bool adaptive_ff_cmp_filter = true;
#endif
#endif



void anc_mutex_creat(void)
{
    g_anc_mutex = xSemaphoreCreateMutex();
    if (g_anc_mutex == NULL) {
        ANC_LOG_E("anc_mutex_creat error\r\n", 0);
    }
}
void anc_mutex_take(void)
{
    if(g_anc_mutex != NULL)
    {
        if (xSemaphoreTake(g_anc_mutex, portMAX_DELAY) == pdFALSE) {
            ANC_LOG_E("anc_mutex_take error\r\n", 0);
        }
    }
}
void anc_mutex_give(void)
{
    if(g_anc_mutex != NULL)
    {
        if (xSemaphoreGive(g_anc_mutex) == pdFALSE) {
            ANC_LOG_E("anc_mutex_give error\r\n", 0);
        }
    }
}
#else
static int g_anc_mutex = 1;
void anc_mutex_creat(void)
{
}
void anc_mutex_take(void)
{
}
void anc_mutex_give(void)
{
}
#endif

void at_anc_callback(anc_control_event_t event_id, anc_control_result_t result)
{
    ANC_LOG_I("Customized ANC callback - event:%d result:%d",2, event_id, result);
    return;
}

#if (ANC_STOP_LATENCY != 0)
static void anc_stop_delay_timer_callback(TimerHandle_t xTimer)
{
    //g_anc_control.enable = 0;
    g_anc_control.waiting_stop_ack = true;
    g_anc_control.stop_dly_timer_available = 1;
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ANC_STOP, 0, 0, false);
    ANC_LOG_I("anc_off and waiting async ack (anc_stop_delay_timer_callback)\n", 0);
}
#endif

#ifdef MTK_AWS_MCE_ENABLE
static void anc_timer_callback(TimerHandle_t xTimer)
{
    ANC_LOG_I("[ANC Sync] enter anc timer callback event %d arg 0x%x\n",2,g_anc_control.event, g_anc_control.arg.val);
    audio_anc_control(g_anc_control.event, g_anc_control.arg.val, g_anc_control.callback);
    g_anc_control.timer_available = 1;
}

static anc_control_result_t anc_timer_service(int32_t time_duration, anc_control_event_t event_id, uint32_t arg, anc_control_callback_t callback)
{
    anc_control_result_t ret = ANC_CONTROL_EXECUTION_FAIL;
    g_anc_control.event = event_id;
    g_anc_control.arg.val = arg;
    g_anc_control.callback = callback;
    if(g_anc_timer) {
        if (time_duration > ANC_SYNC_LATENCY) {
            #ifdef MTK_LEAKAGE_DETECTION_ENABLE
            if (time_duration > 1500) {
            #endif

                ANC_LOG_W("[ANC Sync] sync time may wrong(>500ms): %d ms\n",1,time_duration);
                time_duration = ANC_SYNC_LATENCY;

            #ifdef MTK_LEAKAGE_DETECTION_ENABLE
            }
            #endif
        }
        if ((time_duration > 0) && (xTimerChangePeriod(g_anc_timer, (time_duration / portTICK_PERIOD_MS), 0) == pdPASS)) {
            ANC_LOG_I("[ANC Sync] start anc one-shot timer with duration %d ms\n",1,time_duration);
            g_anc_control.timer_available = 0;
            ret = ANC_CONTROL_EXECUTION_SUCCESS;
        } else {
            ANC_LOG_E("[ANC Sync] change timer period failed\n",0);
            ret = audio_anc_control(event_id, arg, callback);
        }
    }
    return ret;
}

static void bt_aws_mce_report_anc_callback(bt_aws_mce_report_info_t *para)
{
    ANC_LOG_I("AWS MCE Report anc_callback: 0x%x 0x%x 0x%x\n", 3, para->module_id, para->param_len, para->param);
    if (para->module_id == BT_AWS_MCE_REPORT_MODULE_ANC && (para->param != NULL)) {
        aws_mce_report_anc_param_t *anc_param = (aws_mce_report_anc_param_t *)para->param;
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
        if (BT_AWS_MCE_ROLE_AGENT == role) /* Agent handle */
        {
            if (anc_param->event_id == ANC_CONTROL_EVENT_READ_VOLUME_NVDM) {
                race_dsprt_anc_feedback_data(anc_param);
            } else if (anc_param->event_id == ANC_CONTROL_EVENT_ON) {
                ANC_LOG_I("MP_Step__3__ set P anc on filter:0x%x",1,anc_param->arg);
                race_dsprt_anc_notify(ANC_CONTROL_EVENT_ON, 1, anc_param->arg);
            } else if (anc_param->event_id == ANC_CONTROL_EVENT_SET_VOLUME) {
                ANC_LOG_I("MP_Step__5__ set P gain: 0x%x",1,anc_param->arg);
                race_dsprt_anc_notify(ANC_CONTROL_EVENT_SET_VOLUME, 1, anc_param->arg);
            } else if (anc_param->event_id == ANC_CONTROL_EVENT_OFF) {
                ANC_LOG_I("MP_Step__3__ set P anc off",0);
                race_dsprt_anc_notify(ANC_CONTROL_EVENT_OFF, 1, 0);
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
            } else if (anc_param->event_id == ANC_CONTROL_EVENT_LD_START) {
                anc_leakage_detection_racecmd_response((uint16_t)anc_param->arg, 1);
#endif
            }
        } else { /* Partner handle */
            if (anc_param->event_id == ANC_CONTROL_EVENT_READ_VOLUME_NVDM) {
                audio_anc_control((anc_control_event_t)anc_param->event_id, (uint32_t)&anc_param->arg, NULL);
                anc_send_aws_mce_param(ANC_DIRECT, (anc_control_event_t)anc_param->event_id, anc_param->arg, anc_param->ch_info);
            } else if (anc_param->event_id == ANC_CONTROL_EVENT_SET_SYNC_TIME) {
                anc_set_sync_time((uint16_t)anc_param->arg);
            } else {
                if ((anc_param->event_id == ANC_CONTROL_EVENT_ON) || (anc_param->event_id == ANC_CONTROL_EVENT_OFF)) {
                    uint8_t enable = (anc_param->event_id == ANC_CONTROL_EVENT_ON) ? 1 : 0;
                    uint8_t filter_type = (uint8_t)((anc_param->arg >> 16) & 0xFF);
                    int16_t runtime_gain = (int16_t)(anc_param->arg & 0xFFFF);
                    if (audio_anc_backup_while_suspend(enable, filter_type, runtime_gain) == ANC_CONTROL_EXECUTION_CANCELLED) {
                        return;
                    }
                }
                if (anc_param->is_sync) {
                    int32_t get_duration;
                    bt_sink_srv_bt_clock_get_duration(&(anc_param->sync_bt_clock), NULL, &get_duration);
                    ANC_LOG_I("[ANC Sync] Partner target_clk:0x%x, 0x%x, duration:%d us \n", 3, anc_param->sync_bt_clock.nclk, anc_param->sync_bt_clock.nclk_intra, get_duration);
                    anc_timer_service((int32_t)get_duration/1000, (anc_control_event_t)anc_param->event_id, anc_param->arg, NULL);
                } else {
                    audio_anc_control((anc_control_event_t)anc_param->event_id, anc_param->arg, NULL);
                }
            }
        }
    }
}

anc_control_result_t anc_sync_control(anc_control_event_t event, uint32_t arg, anc_control_callback_t callback)
{
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
    if(bt_sink_srv_ami_get_current_scenario() == HFP){
        ANC_LOG_I("anc_sync_control ignore event:%d because HFP happening.", 1, event);
        return ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    if ((g_anc_control.event == ANC_CONTROL_EVENT_LD_START) && ((event & (ANC_CONTROL_EVENT_LD_START | ANC_CONTROL_EVENT_LD_STOP | ANC_CONTROL_EVENT_LD_DL_RESUME)) == 0)) {
        ANC_LOG_I("anc_sync_control ignore event:%d because LD happening.", 1, event);
        return ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
#endif

    anc_control_result_t ret;
    uint32_t sync_mode = ANC_DIRECT;
    if((bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) && (bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {//Agent (Transmitter)
        sync_mode = ANC_SYNC;
    }
    if ((g_anc_control.sync_time != 0) && (bt_connection_manager_get_aws_link_type() == BT_CONNECTION_MANAGER_AWS_LINK_SPECIAL)) {
        ANC_LOG_I("[ANC Sync] is special link case, agent delay %d ms to apply and tell partner to apply directly\n", 1, ANC_SYNC_LATENCY_UNDER_SPECIAL_LINK);
        ret = anc_send_aws_mce_param(ANC_DIRECT, event, arg, 0);
        ret |= anc_timer_service(ANC_SYNC_LATENCY_UNDER_SPECIAL_LINK, event, arg, callback);
    } else if(sync_mode == ANC_DIRECT) {
        ret = audio_anc_control(event, arg, callback);
    } else {
        bt_status_t bt_status;
        bt_clock_t target_clk;
        int32_t duration;
        uint32_t loop = 0;
        if (g_anc_control.sync_time == 0) {
            ret = anc_send_aws_mce_param(ANC_DIRECT, event, arg, 0);
            ret |= audio_anc_control(event, arg, callback);
        } else {
            while (!g_anc_control.timer_available) {
                vTaskDelay(pdMS_TO_TICKS(100));
                loop++;
                ANC_LOG_I("[ANC Sync] wait timer to be available : %d ms !\n", 1, loop*100);
                if(loop >= 6) {
                    ANC_LOG_E("[ANC Sync] timer is unavailable too long !\n", 0);
                    g_anc_control.timer_available = 1;
                }
            }
            bt_status = bt_sink_srv_bt_clock_addition(&target_clk, NULL, (uint32_t)g_anc_control.sync_time*1000);
            if (!g_anc_control.sync_time) {
                sync_mode = ANC_DIRECT;
            }
            g_anc_control.sync_time_bt_clock = target_clk;
            ret = anc_send_aws_mce_param(sync_mode, event, arg, 0);
            if ((bt_status == BT_STATUS_SUCCESS) && (bt_sink_srv_bt_clock_get_duration(&target_clk, NULL, &duration) == BT_STATUS_SUCCESS)) {
                ANC_LOG_I("[ANC Sync] Agent target_clk:0x%x, 0x%x, duration:%dus\n", 3, target_clk.nclk, target_clk.nclk_intra, duration);
                ret |= anc_timer_service(duration/1000, event, arg, callback);
            } else {
                ret |= audio_anc_control(event, arg, callback);
            }
        }
    }
    return ret;
}

anc_control_result_t anc_send_aws_mce_param(anc_setting_mode_t setting_mode, anc_control_event_t event_id, uint32_t arg, uint32_t ch_info)
{
    anc_control_result_t ret = ANC_CONTROL_EXECUTION_SUCCESS;
    aws_mce_report_anc_param_t anc_param;
    bt_aws_mce_report_info_t info;
    info.module_id = BT_AWS_MCE_REPORT_MODULE_ANC;
    info.param_len = sizeof(aws_mce_report_anc_param_t);
    info.param = &anc_param;
    anc_param.event_id = event_id;
    anc_param.arg = arg;
    anc_param.ch_info = ch_info;
    anc_param.is_sync = (setting_mode == ANC_DIRECT) ? false : true;
    anc_param.sync_bt_clock = g_anc_control.sync_time_bt_clock;

    if (anc_param.is_sync) {
        if (bt_aws_mce_report_send_urgent_event(&info) != BT_STATUS_SUCCESS) {
            ANC_LOG_E("[ANC sync] fail to send aws mce report\n", 0);
            ret = ANC_CONTROL_EXECUTION_FAIL;
        }
    } else {
        if (bt_aws_mce_report_send_event(&info) != BT_STATUS_SUCCESS) {
            ANC_LOG_E("[ANC dirt] fail to send aws mce report\n", 0);
            ret = ANC_CONTROL_EXECUTION_FAIL;
        }
    }
    return ret;
}
#endif

void anc_save_misc_param(void)
{
#ifdef MTK_ANC_BACKUP_STATUS_ENABLE
    if (g_anc_control.sync_time != 0) {
        anc_nvdm_misc_param_t payload;
        anc_mutex_take();
        payload.anc_suspend = g_anc_control.suspend & 0x1;
        payload.anc_enable = (payload.anc_suspend) ? g_anc_control.backup_enable : (g_anc_control.enable&0x1);
        payload.anc_filter_type = (uint8_t)g_anc_control.cur_filter_type;
        payload.anc_runtime_gain = g_anc_control.anc_runtime_gain;
        payload.passthru_runtime_gain = g_anc_control.passthru_runtime_gain;
        anc_mutex_give();
        if (flash_memory_write_nvdm_data(NVKEY_DSP_PARA_ANC_MISC_PARA, (uint8_t *)&payload, sizeof(anc_nvdm_misc_param_t)) != NVDM_STATUS_NAT_OK) {
            ANC_LOG_E("Write ANC misc param error\n",0);
        } else {
            ANC_LOG_I("Backup ANC status into NVDM:0x%x, enable:%d, suspend:%d \n", 3, NVKEY_DSP_PARA_ANC_MISC_PARA, payload.anc_enable, payload.anc_suspend);
        }
    } else {
        ANC_LOG_W("It's under MP calibration, don't backup ANC status\n",0);
    }
#endif
}

static sysram_status_t anc_set_coef(uint8_t **pCoefBuf, uint32_t *buffer_size, uint16_t *keyid, uint16_t numKeyid)
{
    sysram_status_t status;
    uint16_t chksum = 0;
    nat_nvdm_info_t *p_nat_nvdm_info;
    uint8_t *pCoef;

    if(pCoefBuf == NULL || buffer_size == NULL || keyid == NULL || numKeyid == 0) {
        return NVDM_STATUS_ERROR;
    }

    for(int i = 0; i < numKeyid; i++)
    {
        uint32_t length = buffer_size[i];

        if(pCoefBuf[i] == NULL) {
            return NVDM_STATUS_ERROR;
        }

        p_nat_nvdm_info = (nat_nvdm_info_t *)(pCoefBuf[i]);
        p_nat_nvdm_info->offset = sizeof(nat_nvdm_info_t);

        pCoef = (uint8_t *)pCoefBuf[i] + p_nat_nvdm_info->offset;
        status = flash_memory_read_nvdm_data(keyid[i], pCoef, &length);
        if (status != NVDM_STATUS_NAT_OK) {
            return status;
        }

        for (int j = chksum = 0 ; j < length ; j++ ) {
            chksum += (uint16_t)(*pCoef++);
        }

        p_nat_nvdm_info->nvdm_id = keyid[i];
        p_nat_nvdm_info->length = length;
        p_nat_nvdm_info->chksum = chksum;

        //ANC_LOG_I("[anc_set_coef] [Buf]0x%x [keyid]0x%x [length]%d [chksum]%d [offset]%d\n", 5, pCoefBuf[i], p_nat_nvdm_info->nvdm_id, p_nat_nvdm_info->length, p_nat_nvdm_info->chksum, p_nat_nvdm_info->offset);
    }
    return NVDM_STATUS_NAT_OK;
}

static void anc_get_calibrate_gain(int16_t *gain_l, int16_t *gain_r)
{
    uint32_t length = sizeof(anc_sw_gain_t);
    anc_sw_gain_t anc_sw_gain;
    sysram_status_t status;
    status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ANC_SW_GAIN, (uint8_t *)&anc_sw_gain, &length);
    if(status != NVDM_STATUS_NAT_OK) {
        ANC_LOG_E("anc_on is failed to read sw gain from nvdm - err(%d)\r\n", 1, status);
        return;
    }
    if (gain_l) {
        *gain_l = anc_sw_gain.gain_index_l;
    }
    if (gain_r) {
        *gain_r = anc_sw_gain.gain_index_r;
    }
}

static void anc_get_analog_setting(uint16_t *in_gain, uint16_t *out_gain)
{
    bt_sink_srv_am_background_t *background;
    vol_type_t type = VOL_HFP;

    background = am_get_aud_by_type(HFP);
    if (background && background->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD) {
        type = VOL_HFP_NB;
    }
    #if defined(__BT_SINK_SRV_AUDIO_SETTING_SUPPORT__) && defined(__GAIN_TABLE_NVDM_DIRECT__)
    if (in_gain) {
        *in_gain = (uint16_t)audio_get_gain_in_in_dB(AUD_VOL_IN_LEVEL0, GAIN_ANALOG, type);
    }
    if (out_gain) {
        *out_gain = (uint16_t)audio_get_gain_out_in_dB(AUD_VOL_OUT_LEVEL10, GAIN_ANALOG, type);
    }
    #endif
}

#if 0
static void anc_set_dual_mic_setting(hal_audio_device_t *device)
{
    if (*device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) {
        *device |= HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
    } else if (*device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) {
        *device |= HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL;
    }
}
#endif

static anc_control_result_t anc_get_nvdm_param(anc_filter_type_t anc_filter_type, anc_nvdm_param_t *nvdm_param)
{
    anc_control_result_t ret = ANC_CONTROL_EXECUTION_FAIL;
    sysram_status_t status;
    uint16_t nvdm_param_keyid = MACRO_ANC_CHK_PASSTHRU(anc_filter_type) ? NVKEY_DSP_PARA_PASS_THRU : NVKEY_DSP_PARA_ANC;
    uint32_t length = sizeof(anc_nvdm_param_t);

    status = flash_memory_read_nvdm_data(nvdm_param_keyid, (uint8_t *)nvdm_param, &length);
    if(status != NVDM_STATUS_NAT_OK) {
        ANC_LOG_E("anc_on is failed to read nvdm_param 0x%x from nvdm - err(%d)\r\n", 2, nvdm_param_keyid, status);
    } else {
        ret = ANC_CONTROL_EXECUTION_SUCCESS;
        ANC_LOG_I("nvdm_param: %d %d %d %d %d %d %d %d %d %d\r\n", 10, nvdm_param->power_detect_enable,nvdm_param->detect_smooth,nvdm_param->high_threshold_in_dbfs,nvdm_param->lower_bound_in_dbfs,nvdm_param->ramp_up_dly_sel,
            nvdm_param->ramp_up_step,nvdm_param->ramp_down_dly_sel,nvdm_param->ramp_down_step,nvdm_param->overflow_check_enable,nvdm_param->overflow_threshold);
    }
    return ret;
}

static void anc_set_current_status(void)
{
    uint32_t length = sizeof(anc_nvdm_misc_param_t);
    anc_nvdm_misc_param_t payload;
    sysram_status_t status;
    status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ANC_MISC_PARA, (uint8_t *)&payload, &length);
    if (status != NVDM_STATUS_NAT_OK) {
        ANC_LOG_E("Read ANC misc param error, error(%d)\n",1, status);
        g_anc_control.enable = 0;
        g_anc_control.cur_filter_type = 1;
        g_anc_control.anc_runtime_gain = 0;
        g_anc_control.passthru_runtime_gain = 0;
        g_anc_control.suspend = 1;
        g_anc_control.backup_enable = 0;
    } else {
        if (length <= 4) {
            payload.passthru_runtime_gain = 0;
            payload.anc_suspend = 1;
        } else if (length <= 6) {
            payload.anc_suspend = 1;
        }
        g_anc_control.enable = 0;
        g_anc_control.cur_filter_type = payload.anc_filter_type;
        g_anc_control.anc_runtime_gain = payload.anc_runtime_gain;
        g_anc_control.passthru_runtime_gain = payload.passthru_runtime_gain;
        g_anc_control.suspend = payload.anc_suspend & 0x1;
        g_anc_control.backup_enable = (g_anc_control.suspend == 1) ? (payload.anc_enable & 0x1) : 0;
    }
    ANC_LOG_I("anc init suspend:%d backup_enable:%d", 2, g_anc_control.suspend, g_anc_control.backup_enable);
}

#ifdef HAL_DVFS_MODULE_ENABLED
static void anc_set_dvfs_control(uint8_t lock)
{
    if (lock) {
        hal_dvfs_lock_control(DVFS_156M_SPEED , HAL_DVFS_LOCK);
        ANC_LOG_I("frequency is risen to 1.3V", 0);
        g_anc_debug++;
    } else {
        if (g_anc_debug > 0) {
            hal_dvfs_lock_control(DVFS_156M_SPEED, HAL_DVFS_UNLOCK);
            ANC_LOG_I("frequency is set back %d", 1, hal_dvfs_get_cpu_frequency());
            g_anc_debug--;
        } else {
            ANC_LOG_E("anc_set_dvfs_control not pair, g_anc_debug:%d", 1, g_anc_debug);
        }
    }
}
#endif

static anc_control_result_t anc_callback_service(anc_control_event_t event, anc_control_result_t result)
{
    U32 i;
    if (result == ANC_CONTROL_EXECUTION_SUCCESS) {
        for (i = 0; i < ANC_MAX_CALLBACK_NUM; i++)
        {
            if ((g_anc_cb_service.callback_list[i] != NULL) && ((g_anc_cb_service.event_mask[i] & event) != 0)) {
                g_anc_cb_service.callback_list[i](event, result);
                ANC_LOG_I("anc_callback_service %d cb:0x%x", 2, i, g_anc_cb_service.callback_list[i]);
            }
        }
    } else {
        for (i = 0; i < ANC_MAX_CALLBACK_NUM; i++)
        {
            if ((g_anc_cb_service.callback_list[i] != NULL) && ((g_anc_cb_service.event_mask[i] & event) != 0) && (g_anc_cb_service.cb_level[i] == CB_LEVEL_ALL)) {
                g_anc_cb_service.callback_list[i](event, result);
                ANC_LOG_I("anc_callback_service %d cb:0x%x", 2, i, g_anc_cb_service.callback_list[i]);
            }
        }
    }
    return ANC_CONTROL_EXECUTION_SUCCESS;
}

int32_t anc_on(anc_filter_type_t anc_filter_type, int16_t anc_runtime_gain, anc_control_callback_t callback, uint8_t from_nvdm)
{
    mcu2dsp_anc_param_t anc_param;
    DSP_FEATURE_TYPE feature;
    void *p_param_share;
    anc_control_event_t event = ANC_CONTROL_EVENT_ON;
    anc_control_result_t anc_result = ANC_CONTROL_EXECUTION_NONE;
    sysram_status_t status;
    int result = 0;
    uint16_t keyid[ANC_COEF_NUM];
    uint32_t buf_size[ANC_COEF_NUM];
    uint8_t *pCoefBuf[ANC_COEF_NUM];
    mcu2dsp_open_stream_in_param_t in_device;
    mcu2dsp_open_stream_out_param_t out_device;
    audio_channel_t audio_channel = ami_get_audio_channel();
    anc_hybrid_cap_t hybrid_cap;
    uint32_t ff_fb_only;//1: ff_only , 2: fb_only
    uint32_t contiue = 0;
    int i;

    memset(&pCoefBuf[0], 0, sizeof(uint8_t *)*ANC_COEF_NUM);
    memset(&out_device,0,sizeof(out_device));
    memset(&in_device,0,sizeof(in_device));

    ANC_LOG_I("anc_on with from_nvdm:%d\n", 1, from_nvdm);
    if (from_nvdm == 1) {
        anc_nvdm_misc_param_t payload;
        uint32_t length = sizeof(anc_nvdm_misc_param_t);
        status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ANC_MISC_PARA, (uint8_t *)&payload, &length);
        if (status != NVDM_STATUS_NAT_OK) {
            ANC_LOG_E("Read ANC misc param error, error(%d)\n",1, status);
            payload.anc_enable = 0;
        }
        payload.anc_enable &= 0x1;
        if (payload.anc_enable == 0) {
            if (callback) {
                callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_CANCELLED);
            }
            _UNUSED(audio_channel);
            return 0;
        } else {
            anc_filter_type = payload.anc_filter_type;
            anc_runtime_gain = MACRO_ANC_CHK_PASSTHRU(anc_filter_type) ? payload.passthru_runtime_gain : payload.anc_runtime_gain;
        }
    }

    anc_mutex_take();
    if (g_anc_control.enable > 0) {
        anc_mutex_give();
        if (g_anc_control.cur_filter_type != anc_filter_type) {
            g_anc_control.request_filter_type = anc_filter_type;
            g_anc_control.request_runtime_gain = anc_runtime_gain;
            g_anc_control.anc_on_callback = callback;
            anc_off(NULL, anc_filter_type, 0);
            anc_result = ANC_CONTROL_EXECUTION_CANCELLED;
            //return 0;
        } else {
            if (anc_runtime_gain != ANC_UNASSIGNED_GAIN) {
                if ((MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == true) && (g_anc_control.passthru_runtime_gain != anc_runtime_gain)) {
                    anc_set_runtime_volume(anc_runtime_gain);
                } else if ((MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == false) && (g_anc_control.anc_runtime_gain != anc_runtime_gain)) {
                    anc_set_runtime_volume(anc_runtime_gain);
                }
            }
            if (callback) {
                callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_SUCCESS);
            }
            event = ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME;
            anc_result = ANC_CONTROL_EXECUTION_SUCCESS;
            //return 0;
        }
    } else {
        if (g_anc_control.waiting_stop_ack || (g_anc_control.stop_dly_timer_available == 0)) {
            if (g_anc_control.anc_on_callback) {
                anc_mutex_give();
                g_anc_control.anc_on_callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_CANCELLED);
                anc_mutex_take();
            }
            g_anc_control.request_filter_type = anc_filter_type;
            g_anc_control.request_runtime_gain = anc_runtime_gain;
            g_anc_control.anc_on_callback = callback;
            anc_mutex_give();
            anc_result = ANC_CONTROL_EXECUTION_CANCELLED;
            //return 0;
        } else {
            g_anc_control.enable = 1;
            anc_mutex_give();
            contiue = 1;
        }
    }

    if (contiue == 1) {

        ff_fb_only = (anc_filter_type & ANC_FF_ONLY_BIT_MASK) ? 1 : (anc_filter_type & ANC_FB_ONLY_BIT_MASK) ? 2 : 0;
        anc_filter_type &= ANC_FILTER_TYPE_MASK;
        if (anc_runtime_gain == (int16_t)ANC_UNASSIGNED_GAIN) {
            anc_runtime_gain = (MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == true) ? g_anc_control.passthru_runtime_gain : g_anc_control.anc_runtime_gain;
        }
        ANC_LOG_I("anc on with filter type: %d, runtime_gain: %d, misc_param: %d\n", 3, anc_filter_type, anc_runtime_gain, ff_fb_only);

        // set runing flag
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_ANC,1);
        hal_clock_enable(HAL_CLOCK_CG_ANC);

        switch (anc_filter_type) {
        case FILTER_1:
            feature = FUNC_ANC1;
            break;
        case FILTER_2:
            feature = FUNC_ANC2;
            break;
        case FILTER_3:
            feature = FUNC_ANC3;
            break;
        case FILTER_4:
            feature = FUNC_ANC4;
            break;
        case FILTER_5:
            feature = FUNC_ANC5;
            break;
        case FILTER_6:
            feature = FUNC_ANC6;
            break;
        case FILTER_7:
            feature = FUNC_ANC7;
            break;
        default:
            ANC_LOG_E("Unvalid filter type\n", 0);
            result = -1;
            goto _ANC_END;
            break;
        }

        for(i = 0; i < ANC_COEF_NUM; i++)
        {
            keyid[i] = g_dsp_feature_table[feature].nvkey_id[i];
            buf_size[i] = sizeof(anc_fwd_iir_t);
            pCoefBuf[i] = pvPortMallocNC(buf_size[i]);
            if(pCoefBuf[i] != NULL){
                memset(pCoefBuf[i], 0, buf_size[i]);
            } else {
                ANC_LOG_E("[anc_set_coef] pvPortMallocNC fail\n", 0);
                result = -1;
                goto _ANC_END;
            }
        }
        status = anc_set_coef((uint8_t **)&pCoefBuf, buf_size, keyid, ANC_COEF_NUM);
        if (status != NVDM_STATUS_NAT_OK){
            ANC_LOG_E("anc_on is failed to set audio feature to nvdm - err(%d)\r\n",1, status);
            result = -1;
            goto _ANC_END;
        }

        anc_result = anc_get_nvdm_param(anc_filter_type, &g_anc_control.nvdm_param);
        if(anc_result != ANC_CONTROL_EXECUTION_SUCCESS) {
            result = -1;
            goto _ANC_END;
        }

        memset(&anc_param, 0, sizeof(mcu2dsp_anc_param_t));
        in_device.afe.audio_device  = HAL_AUDIO_DEVICE_MAIN_MIC_L;
        in_device.afe.audio_device1 = HAL_AUDIO_DEVICE_MAIN_MIC_R;
        in_device.afe.misc_parms = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v | MICBIAS0_AMIC_MEMS | MICBIAS1_AMIC_MEMS | UPLINK_PERFORMANCE_HIGH | AMIC_ACC;
        out_device.afe.audio_device = HAL_AUDIO_DEVICE_DAC_L;
        out_device.afe.misc_parms = DOWNLINK_PERFORMANCE_NORMAL;
        hal_audio_get_stream_in_setting_config(AU_DSP_ANC, &in_device);
        hal_audio_get_stream_out_setting_config(AU_DSP_ANC, &out_device);
        anc_param.device = in_device.afe.audio_device | out_device.afe.audio_device;
        anc_param.in_misc_param = in_device.afe.misc_parms;
        anc_param.out_misc_param = out_device.afe.misc_parms;
        anc_param.keyid_l = keyid[ANC_COEF_L];
        anc_param.keyid_r = keyid[ANC_COEF_R];
        anc_param.p_coef_buf[ANC_COEF_L] = (uint8_t *)hal_memview_cm4_to_infrasys((uint32_t)pCoefBuf[ANC_COEF_L]);
        anc_param.p_coef_buf[ANC_COEF_R] = (uint8_t *)hal_memview_cm4_to_infrasys((uint32_t)pCoefBuf[ANC_COEF_R]);
        anc_get_calibrate_gain(&anc_param.gain_index_l, &anc_param.gain_index_r);

        anc_param.runtime_gain = anc_runtime_gain;
        anc_param.ramp_config_gain = g_anc_control.nvdm_param.ramp_config_gain;
        anc_param.ramp_dly_sel = g_anc_control.nvdm_param.ramp_dly_sel;
        anc_param.adc_chexc = g_anc_control.nvdm_param.adc_swap;
        anc_param.ramp_step = g_anc_control.nvdm_param.ramp_step;
        anc_get_hybrid_capability(&hybrid_cap);
        anc_param.support_hybrid_enable = (hybrid_cap.chip_id == ANC_CHIP_ID_E4) ? 1 : 0;
        anc_param.hybrid_enable = ((anc_param.support_hybrid_enable == 1) && (hybrid_cap.efuse_dis_hybrid == 0) && (hybrid_cap.proj_dis_hybrid == 0) && (MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == false) && (ff_fb_only != 1)) ? 1 : 0;
#ifdef MTK_DEQ_ENABLE
        anc_param.deq_enable = g_anc_control.deq_enable;
#else
        anc_param.deq_enable = 0;
#endif
#ifdef MTK_AWS_MCE_ENABLE
        anc_param.earbuds_ch = (audio_channel == AUDIO_CHANNEL_R) ? 2 : 1;
#else
        anc_param.earbuds_ch = 0;
#endif
        anc_get_analog_setting(&anc_param.in_analog_gain, &anc_param.out_analog_gain);
        anc_param.power_detect_enable    = g_anc_control.nvdm_param.power_detect_enable;
        anc_param.smooth                 = g_anc_control.nvdm_param.detect_smooth;
        anc_param.high_threshold_in_dbfs = g_anc_control.nvdm_param.high_threshold_in_dbfs;
        anc_param.lower_bound_in_dbfs    = g_anc_control.nvdm_param.lower_bound_in_dbfs;
        anc_param.ramp_up_dly_sel        = g_anc_control.nvdm_param.ramp_up_dly_sel;
        anc_param.ramp_up_step           = g_anc_control.nvdm_param.ramp_up_step;
        anc_param.ramp_down_dly_sel      = g_anc_control.nvdm_param.ramp_down_dly_sel;
        anc_param.ramp_down_step         = g_anc_control.nvdm_param.ramp_down_step;
        anc_param.reduce_gain_step       = g_anc_control.nvdm_param.reduce_gain_step;
        anc_param.enlarge_gain_step      = g_anc_control.nvdm_param.enlarge_gain_step;
        anc_param.check_irq_period_in_us = g_anc_control.nvdm_param.check_irq_period_in_us;
        anc_param.overflow_check_enable  = g_anc_control.nvdm_param.overflow_check_enable;
        anc_param.overflow_threshold     = g_anc_control.nvdm_param.overflow_threshold;
        anc_param.reset_delay_in_us      = g_anc_control.nvdm_param.reset_delay_in_us;
#ifdef MTK_ANC_HOWLING_TURN_OFF_ANC
        anc_param.howling_tolerance      = ((g_anc_control.nvdm_param.howling_tolerance < HOWLING_TOLERANCE_MIN) && (g_anc_control.nvdm_param.howling_tolerance != 0)) ? HOWLING_TOLERANCE_MIN : g_anc_control.nvdm_param.howling_tolerance;
#else
        anc_param.howling_tolerance      = 0;
#endif

        if ((in_device.afe.audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_R) || (in_device.afe.audio_device == HAL_AUDIO_DEVICE_DIGITAL_MIC_R)
            || ((anc_param.hybrid_enable == 1) && (in_device.afe.audio_device == HAL_AUDIO_DEVICE_NONE) && ((in_device.afe.audio_device1 == HAL_AUDIO_DEVICE_MAIN_MIC_L) || (in_device.afe.audio_device1 == HAL_AUDIO_DEVICE_DIGITAL_MIC_L)))) {
            anc_param.adc_chexc = 1;
        }
        if (anc_param.hybrid_enable) {
            #ifdef MTK_DEQ_ENABLE
            g_anc_control.deq_digital_gain = -(anc_param.gain_index_l + anc_param.gain_index_r);
            #endif
            audio_set_anc_compensate_phase2(NONE, ANC_CONTROL_EVENT_ON);
            if (ff_fb_only == 2) {
                nat_nvdm_info_t *p_nat_nvdm_info = (nat_nvdm_info_t *)pCoefBuf[ANC_COEF_L];
                uint8_t *pCoef = pCoefBuf[ANC_COEF_L] + p_nat_nvdm_info->offset + sizeof(anc_fwd_iir_t) - sizeof(int16_t);
                memset(pCoefBuf[ANC_COEF_L] + p_nat_nvdm_info->offset, 0, sizeof(anc_fwd_iir_t) - sizeof(int16_t));
                p_nat_nvdm_info->chksum = *(pCoef) + *(pCoef+1);
            }
            anc_param.device |= in_device.afe.audio_device1;
            if (in_device.afe.audio_device == in_device.afe.audio_device1) {
                ANC_LOG_E("hybrid anc, dev:0x%x 0x%x 0x%x, FF/FB can't use the same device\r\n", 3, in_device.afe.audio_device,in_device.afe.audio_device1,out_device.afe.audio_device);
                result = -1;
                goto _ANC_END;
            }
        }
        ANC_LOG_I("hybrid:%d, dev:0x%x 0x%x 0x%x, deq:%d, au_ch:0x%x, ff_fb_only:0x%x\r\n", 7, anc_param.hybrid_enable, in_device.afe.audio_device,in_device.afe.audio_device1,out_device.afe.audio_device,anc_param.deq_enable,audio_channel,ff_fb_only);

#ifdef HAL_DVFS_MODULE_ENABLED
        anc_set_dvfs_control(1);
#endif
#ifdef HAL_PMU_MODULE_ENABLED
        if (MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == false) {
            pmu_set_register_value_2byte_mt6388(0x32, 0xFFFF, 0, 0x2); // set VAUD18 to SW mode
        }
#endif

#ifndef MTK_AWS_MCE_ENABLE
        anc_param.device |= in_device.afe.audio_device1;
#endif

        g_anc_control.cur_filter_type = (ff_fb_only == 1) ? (anc_filter_type | ANC_FF_ONLY_BIT_MASK) : (ff_fb_only == 2) ? (anc_filter_type | ANC_FB_ONLY_BIT_MASK) : anc_filter_type;
        g_anc_control.hybrid_enable = anc_param.hybrid_enable;
        if (MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == true) {
            g_anc_control.passthru_runtime_gain = anc_runtime_gain;
        } else {
            g_anc_control.anc_runtime_gain = anc_runtime_gain;
        }

        p_param_share = hal_audio_dsp_controller_put_paramter(&anc_param, sizeof(mcu2dsp_anc_param_t), AUDIO_MESSAGE_TYPE_ANC);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ANC_START, 0, (uint32_t)p_param_share, true);

        if (MACRO_ANC_CHK_PASSTHRU(anc_filter_type)) {
            LOG_W(MPLOG,"Pass-through: ON , cap: %d",anc_param.support_hybrid_enable);
        } else {
            LOG_W(MPLOG,"ANC: ON , filter: %d mode: %d cap: %d",anc_filter_type&ANC_FILTER_TYPE_MASK,anc_param.hybrid_enable,anc_param.support_hybrid_enable);
        }

        if(callback) {
            callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_SUCCESS);
        }

        if (anc_param.hybrid_enable) {
            audio_set_anc_compensate(NONE, ANC_CONTROL_EVENT_ON, NULL);
        }

        anc_result = ANC_CONTROL_EXECUTION_SUCCESS;
    }

_ANC_END:
    for(int i = 0; i < ANC_COEF_NUM; i++)
    {
        if(pCoefBuf[i] != NULL){
            vPortFreeNC(pCoefBuf[i]);
        }
    }
    if(result != 0)
    {
        anc_mutex_take();
        g_anc_control.enable = 0;
        anc_mutex_give();
        if(callback) {
            callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_FAIL);
        }
        anc_result = ANC_CONTROL_EXECUTION_FAIL;
        if (contiue == 1) {
            // clear runing flag
            hal_clock_disable(HAL_CLOCK_CG_ANC);
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_ANC,0);
        }
    }
    anc_callback_service(event, anc_result);
    _UNUSED(audio_channel);
    return result;
}

void anc_dsp_message_callback(hal_audio_event_t event, void *user_data)
{
    _UNUSED(event);
    uint32_t ccni_msg = (uint32_t)user_data;
    if (ccni_msg == MSG_DSP2MCU_COMMON_ANC_STOP_ACK) {
        ANC_LOG_I("receive anc stop ack", 0);
        g_anc_control.waiting_stop_ack = false;

        if(g_anc_control.anc_off_callback) {
            g_anc_control.anc_off_callback(ANC_CONTROL_EVENT_OFF, ANC_CONTROL_EXECUTION_SUCCESS);
            g_anc_control.anc_off_callback = NULL;
        }

#ifdef HAL_PMU_MODULE_ENABLED
        pmu_set_register_value_2byte_mt6388(0x32, 0xFFFF, 0, 0x3); // set VAUD18 back
#endif

        // set runing flag
        hal_clock_disable(HAL_CLOCK_CG_ANC);
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_ANC,0);

        anc_callback_service(ANC_CONTROL_EVENT_OFF, ANC_CONTROL_EXECUTION_SUCCESS);

        audio_set_anc_compensate_phase2(NONE, ANC_CONTROL_EVENT_OFF);

        LOG_W(MPLOG,"ANC/Pass-thru: OFF");

        if (g_anc_control.request_filter_type != 0) {
            anc_on(g_anc_control.request_filter_type, g_anc_control.request_runtime_gain, g_anc_control.anc_on_callback, 0);
            g_anc_control.anc_on_callback = NULL;
        }
#ifdef HAL_DVFS_MODULE_ENABLED
        anc_set_dvfs_control(0);
#endif
        race_dsprt_anc_notify(ANC_CONTROL_EVENT_OFF, 0, 0);
    } else if (ccni_msg == MSG_DSP2MCU_COMMON_ANC_START_DONE) {
        if (g_anc_control.dsp_message_cb_data == 1) {
            ANC_LOG_I("receive anc howling ccni", 0);
            #ifdef MTK_ANC_HOWLING_TURN_OFF_ANC
            anc_callback_service(ANC_CONTROL_EVENT_HOWLING, ANC_CONTROL_EXECUTION_SUCCESS);
            #endif
        } else {
            ANC_LOG_I("receive anc start done ccni", 0);
#ifdef HAL_DVFS_MODULE_ENABLED
            anc_set_dvfs_control(0);
#endif
            race_dsprt_anc_notify(ANC_CONTROL_EVENT_ON, 0, (g_anc_control.enable > 0) ? (g_anc_control.cur_filter_type) : 0);
        }
    } else if (ccni_msg == MSG_DSP2MCU_COMMON_ANC_SET_VOLUME_ACK) {
        ANC_LOG_I("receive anc set vol done ccni", 0);
        race_dsprt_anc_notify(ANC_CONTROL_EVENT_SET_VOLUME, 0, g_anc_control.dsp_message_cb_data);
    }
}

int32_t anc_off(anc_control_callback_t callback, anc_filter_type_t request_filter_type, uint8_t to_nvdm)
{
    g_anc_control.request_filter_type = request_filter_type;
    if ((g_anc_control.request_filter_type == 0) && (g_anc_control.anc_on_callback)) {
        g_anc_control.anc_on_callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_CANCELLED);
        g_anc_control.anc_on_callback = NULL;
    }
    if (to_nvdm == 1) {
        anc_save_misc_param();
    }

    anc_mutex_take();
    if (g_anc_control.enable == 0) {
        anc_mutex_give();
        if (g_anc_control.waiting_stop_ack || (g_anc_control.stop_dly_timer_available == 0)) {
            if (g_anc_control.anc_off_callback) {
                g_anc_control.anc_off_callback(ANC_CONTROL_EVENT_OFF, ANC_CONTROL_EXECUTION_CANCELLED);
            }
            g_anc_control.anc_off_callback = callback;
        } else {
            if (callback) {
                callback(ANC_CONTROL_EVENT_OFF, ANC_CONTROL_EXECUTION_SUCCESS);
            }
        }
        race_dsprt_anc_notify(ANC_CONTROL_EVENT_OFF, 0, 0);
        return 0;
    } else {
        g_anc_control.enable = 0;
        anc_mutex_give();
    }

#ifdef HAL_DVFS_MODULE_ENABLED
    anc_set_dvfs_control(1);
#endif

    if (g_anc_control.hybrid_enable) {
        bt_sink_srv_am_type_t cur_type = NONE;
        audio_set_anc_compensate(NONE, ANC_CONTROL_EVENT_OFF, &cur_type);
        #if (ANC_STOP_LATENCY != 0)
        if (((cur_type == A2DP) || (cur_type == AWS)) && g_anc_stop_dly_timer && g_anc_control.stop_dly_timer_available) {
            g_anc_control.anc_off_callback = callback;
            if(xTimerChangePeriod(g_anc_stop_dly_timer, (ANC_STOP_LATENCY / portTICK_PERIOD_MS), 0) == pdPASS) {
                g_anc_control.stop_dly_timer_available = 0;
                ANC_LOG_I("anc_off waiting PEQ %d ms\n", 1, ANC_STOP_LATENCY);
                return 0;
            }
        }
        #endif
    }

    ANC_LOG_I("anc_off and waiting async ack\n", 0);
    g_anc_control.waiting_stop_ack = true;
    g_anc_control.anc_off_callback = callback;
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ANC_STOP, 0, 0, false);
    return 0;
}

int32_t anc_set_volume(anc_sw_gain_t *anc_sw_gain)
{
    uint32_t param;
    param = (uint32_t)(((uint32_t)(anc_sw_gain->gain_index_r&0xFFFF)<<16)|(anc_sw_gain->gain_index_l&0xFFFF));
    ANC_LOG_I("ANC_SET_VOL L:%d, R:%d", 2, anc_sw_gain->gain_index_l, anc_sw_gain->gain_index_r);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ANC_SET_VOLUME, 0, param, true);

    return 0;
}

int32_t anc_set_runtime_volume(int16_t anc_runtime_gain)
{
    if (anc_runtime_gain != (int16_t)ANC_UNASSIGNED_GAIN) {
        if (MACRO_ANC_CHK_PASSTHRU(g_anc_control.cur_filter_type) == true) {
            g_anc_control.passthru_runtime_gain = anc_runtime_gain;
        } else {
            g_anc_control.anc_runtime_gain = anc_runtime_gain;
        }
    }
    ANC_LOG_I("ANC_SET_RUNTIME_VOL gain:%d", 1, anc_runtime_gain);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_ANC_SET_VOLUME, 1, anc_runtime_gain, true);

    anc_callback_service(ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME, ANC_CONTROL_EXECUTION_SUCCESS);
    return 0;
}

int32_t anc_read_volume_nvdm(anc_sw_gain_t *anc_sw_gain)
{
    uint32_t length = sizeof(anc_sw_gain_t);
    sysram_status_t status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ANC_SW_GAIN, (uint8_t *)anc_sw_gain, &length);
    if (status != NVDM_STATUS_NAT_OK) {
        ANC_LOG_E("anc failed to read 0x%x from nvdm - err(%d)\r\n", 2, NVKEY_DSP_PARA_ANC_SW_GAIN, status);
        return -1;
    }
    ANC_LOG_I("ANC Read nvdm VOL L:%d, R:%d", 2, anc_sw_gain->gain_index_l, anc_sw_gain->gain_index_r);
    return 0;
}

int32_t anc_write_volume_nvdm(anc_sw_gain_t *anc_sw_gain)
{
    sysram_status_t status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_ANC_SW_GAIN, (uint8_t *)anc_sw_gain, sizeof(anc_sw_gain_t));
    if (status != NVDM_STATUS_NAT_OK) {
        ANC_LOG_E("anc failed to write 0x%x from nvdm - err(%d)\r\n", 2, NVKEY_DSP_PARA_ANC_SW_GAIN, status);
        return -1;
    }
    return 0;
}

int32_t anc_write_filter_volume_nvdm(anc_filter_type_t anc_filter_type, int16_t fiter_uniqu_gain)
{
    uint16_t nvkey_l;
    uint16_t nvkey_r;
    anc_fwd_iir_t *tmp_buffer = NULL;
    uint32_t buf_size, length;
    sysram_status_t status;

    switch (anc_filter_type) {
    case FILTER_1:
    default:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER1;
        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER1;
        break;
    case FILTER_2:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER2;
        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER2;
        break;
    case FILTER_3:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER3;
        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER3;
        break;
    case FILTER_4:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER4;
        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER4;
        break;
    case FILTER_5:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER5;
        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER5;
        break;
    }

    buf_size = sizeof(anc_fwd_iir_t);
    tmp_buffer = (anc_fwd_iir_t *)pvPortMalloc(buf_size);
    if(tmp_buffer != NULL){
        memset(tmp_buffer, 0, buf_size);
        length = buf_size;
        status = flash_memory_read_nvdm_data(nvkey_l, (uint8_t *)tmp_buffer, &length);
        if(status != NVDM_STATUS_NAT_OK) ANC_LOG_E("anc failed to read 0x%x from nvdm - err(%d)\r\n", 2, nvkey_l,status);
        tmp_buffer->filter_unique_gain = fiter_uniqu_gain;
        status = flash_memory_write_nvdm_data(nvkey_l, (uint8_t *)tmp_buffer, length);
        if(status != NVDM_STATUS_NAT_OK) ANC_LOG_E("anc failed to write 0x%x from nvdm - err(%d)\r\n", 2, nvkey_l,status);
        length = buf_size;
        status = flash_memory_read_nvdm_data(nvkey_r, (uint8_t *)tmp_buffer, &length);
        if(status != NVDM_STATUS_NAT_OK) ANC_LOG_E("anc failed to read 0x%x from nvdm - err(%d)\r\n", 2, nvkey_r,status);
        tmp_buffer->filter_unique_gain = fiter_uniqu_gain;
        status = flash_memory_write_nvdm_data(nvkey_r, (uint8_t *)tmp_buffer, length);
        if(status != NVDM_STATUS_NAT_OK) ANC_LOG_E("anc failed to write 0x%x from nvdm - err(%d)\r\n", 2, nvkey_r,status);
        vPortFree((void *)tmp_buffer);
    }

    return 0;
}

uint32_t anc_get_hybrid_capability(anc_hybrid_cap_t *hybrid_cap)
{
    uint32_t ret = 0;
    uint32_t chip_id = *((volatile uint32_t *)(ANC_CHIP_ID_REG));
    uint32_t efuse_val = *((volatile uint32_t *)(ANC_EFUSE_REG));
    hybrid_cap->chip_id = (uint16_t)(chip_id & 0xFFFF);
    hybrid_cap->efuse_dis_hybrid = (efuse_val & ANC_EFUSE_MASK) ? 1 : 0;
#ifdef MTK_HYBRID_ANC_ENABLE
    hybrid_cap->proj_dis_hybrid = 0;
#else
    hybrid_cap->proj_dis_hybrid = 1;
#endif
    ANC_LOG_I("chip_id:%x efuse_dis_hybrid:%d proj_dis_hybrid:%d \r\n", 3, hybrid_cap->chip_id, hybrid_cap->efuse_dis_hybrid, hybrid_cap->proj_dis_hybrid);
    return ret;
}

void anc_get_status(uint8_t *enable, uint32_t *runtime_info, uint8_t *hybrid_enable)
{
    if (enable != NULL) {
        anc_mutex_take();
        *enable = g_anc_control.enable;
        anc_mutex_give();
    }
    if (runtime_info != NULL) {
        if (MACRO_ANC_CHK_PASSTHRU(g_anc_control.cur_filter_type) == true) {
            *runtime_info = ((uint32_t)g_anc_control.passthru_runtime_gain << 16) | ((uint32_t)g_anc_control.cur_filter_type);
        } else {
            *runtime_info = ((uint32_t)g_anc_control.anc_runtime_gain << 16) | ((uint32_t)g_anc_control.cur_filter_type);
        }
    }
    if (hybrid_enable != NULL) {
        *hybrid_enable = g_anc_control.hybrid_enable;
    }
}

int16_t anc_get_runtime_gain(uint8_t get_anc_gain)
{
    if (get_anc_gain) {
        return g_anc_control.anc_runtime_gain;
    } else {
        return g_anc_control.passthru_runtime_gain;
    }
}

void anc_get_attach_enable(uint8_t *enable)
{
    if (enable != NULL) {
        anc_mutex_take();
        *enable = g_anc_control.attach_enable;
        anc_mutex_give();
    }
}

void anc_set_attach_enable(uint8_t enable)
{
    anc_mutex_take();
    g_anc_control.attach_enable = enable;
    anc_mutex_give();
}

void anc_get_backup_status(uint8_t *enable)
{
    if (enable != NULL) {
        *enable = (g_anc_control.suspend == 1) ? g_anc_control.backup_enable : g_anc_control.enable;
    }
}

#ifdef MTK_DEQ_ENABLE
uint32_t anc_get_deq_param(void)
{
    uint32_t deq_param = 0;
    deq_param |= (((uint32_t)g_anc_control.deq_digital_gain) & 0xFFFF);
    deq_param |= (((uint32_t)g_anc_control.deq_delay_samples) << 16);
    deq_param |= (((uint32_t)g_anc_control.deq_misc_param) << 24);
    return deq_param;
}

void anc_set_deq_param(uint8_t delay, int16_t gain, uint8_t phase_inverse)
{
    g_anc_control.deq_delay_samples = delay;
    g_anc_control.deq_digital_gain = gain;
    g_anc_control.deq_misc_param &= (~(0x1<<2));
    g_anc_control.deq_misc_param |= ((phase_inverse & 0x1)<<2);
}

uint8_t anc_get_deq_enable(void)
{
    return g_anc_control.deq_enable;
}

void anc_set_deq_enable(uint8_t deq_enable)
{
    g_anc_control.deq_enable = deq_enable;
}
#endif

uint16_t anc_get_sync_time(void)
{
    return g_anc_control.sync_time;
}

void anc_set_sync_time(uint16_t sync_time)
{
    g_anc_control.sync_time = sync_time;
}

void anc_config(anc_config_cmd_t cmd, uint32_t *arg)
{
    switch(cmd)
    {
        case ANC_CONFIG_RAMP_DLY:
        {
            g_anc_control.nvdm_param.ramp_dly_sel = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_ADC_SWAP:
        {
            g_anc_control.nvdm_param.adc_swap = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_RAMP_STEP:
        {
            g_anc_control.nvdm_param.ramp_step = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_RAMP_CONFIG_GAIN:
        {
            g_anc_control.nvdm_param.ramp_config_gain = (int16_t)((*arg) & 0xFFFF);
            break;
        }
        case ANC_CONFIG_PWR_DET_ENABLE:
        {
            g_anc_control.nvdm_param.power_detect_enable = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_SMOOTH:
        {
            g_anc_control.nvdm_param.detect_smooth = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_HIGH_THRESHOLD_IN_DBFS:
        {
            g_anc_control.nvdm_param.high_threshold_in_dbfs = (int16_t)((*arg) & 0xFFFF);
            break;
        }
        case ANC_CONFIG_LOWER_BOUND:
        {
            g_anc_control.nvdm_param.lower_bound_in_dbfs = (int16_t)((*arg) & 0xFFFF);
            break;
        }
        case ANC_CONFIG_RAMP_UP_DLY:
        {
            g_anc_control.nvdm_param.ramp_up_dly_sel = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_RAMP_UP_STEP:
        {
            g_anc_control.nvdm_param.ramp_up_step = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_RAMP_DOWN_DLY:
        {
            g_anc_control.nvdm_param.ramp_down_dly_sel = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_RAMP_DOWN_STEP:
        {
            g_anc_control.nvdm_param.ramp_down_step = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_OVF_CHECK_ENABLE:
        {
            g_anc_control.nvdm_param.overflow_check_enable = (uint8_t)((*arg) & 0xF);
            break;
        }
        case ANC_CONFIG_OVF_THRESHOLD:
        {
            g_anc_control.nvdm_param.overflow_threshold = (int16_t)((*arg) & 0xFFFF);
            break;
        }
        case ANC_CONFIG_RESTORE_NVDM_PARAM:
        {
            uint16_t nvkeyid = (uint16_t)((*arg) & 0xFFFF);
            sysram_status_t status = flash_memory_write_nvdm_data(nvkeyid, (uint8_t *)&g_anc_control.nvdm_param, sizeof(anc_nvdm_param_t));
            if (status != NVDM_STATUS_NAT_OK) {
                ANC_LOG_E("anc failed to write 0x%x from nvdm - err(%d)\r\n", 2, nvkeyid, status);
            }
            break;
        }
        case ANC_CONFIG_REDUCE_GAIN_STEP:
        {
            g_anc_control.nvdm_param.reduce_gain_step = (uint16_t)((*arg) & 0xFFFF);
            break;
        }
        default:
            break;
    }
}

void anc_init(void)
{
    if(g_anc_mutex == NULL)
    {
        anc_mutex_creat();
    }
    memset(&g_anc_control, 0, sizeof(anc_control_t));
    g_anc_control.attach_enable = 1;
    g_anc_control.sync_time = ANC_SYNC_LATENCY;
    g_anc_control.timer_available = 1;
    g_anc_control.stop_dly_timer_available = 1;
    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_ANC, anc_dsp_message_callback, &g_anc_control.dsp_message_cb_data);
#ifdef MTK_AWS_MCE_ENABLE
#ifdef MTK_DEQ_ENABLE
    g_anc_control.deq_enable = 1;
    g_anc_control.deq_misc_param = (ami_get_audio_channel() == AUDIO_CHANNEL_R) ? 2 : 1;
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_DEQ_SET_PARAM, 1, anc_get_deq_param(), true);
#endif
    if (bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_ANC, bt_aws_mce_report_anc_callback) != BT_STATUS_SUCCESS) {
        ANC_LOG_E("anc failed to register aws mce report callback\r\n", 0);
    }
    if(g_anc_timer == NULL) {
        g_anc_timer = xTimerCreate("AncTimer", (ANC_SYNC_LATENCY / portTICK_PERIOD_MS), pdFALSE, 0, anc_timer_callback);
        if(g_anc_timer == NULL) {
            ANC_LOG_E("anc_init: create timer failed\n", 0);
        }
    }
#endif
#if (ANC_STOP_LATENCY != 0)
    if(g_anc_stop_dly_timer == NULL) {
        g_anc_stop_dly_timer = xTimerCreate("AncStop", (ANC_STOP_LATENCY / portTICK_PERIOD_MS), pdFALSE, 0, anc_stop_delay_timer_callback);
        if(g_anc_stop_dly_timer == NULL) {
            ANC_LOG_E("[ANC] create g_anc_stop_dly_timer failed\n", 0);
        }
    }
#endif
#ifdef MTK_ANC_BACKUP_STATUS_ENABLE
    anc_set_current_status();
#endif
    ANC_LOG_I("[ANC] anc_init() finish\n", 0);
}

anc_control_result_t anc_reinit_nvdm(void)
{
    sysram_status_t status;
    uint8_t T_NVDM_E18C[] =
    {
        0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01
    };
    status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_ANC_MISC_PARA, (uint8_t *)T_NVDM_E18C, sizeof(T_NVDM_E18C));
    if(status != NVDM_STATUS_NAT_OK) {
        ANC_LOG_E("anc failed to reinit 0x%x from nvdm - err(%d)\r\n", 2, NVKEY_DSP_PARA_ANC_MISC_PARA,status);
        return ANC_CONTROL_EXECUTION_FAIL;
    }
    return ANC_CONTROL_EXECUTION_SUCCESS;
}

anc_control_result_t audio_anc_enable(anc_filter_type_t anc_filter_type, int16_t anc_runtime_gain, anc_control_callback_t callback)
{
    uint32_t arg = ((uint32_t)anc_filter_type << 16) | ((uint32_t)anc_runtime_gain & 0xFFFF);
    if (g_anc_control.suspend == 1) {
        g_anc_control.backup_enable = 1;
        g_anc_control.cur_filter_type = anc_filter_type;
        if (anc_runtime_gain != (int16_t)ANC_UNASSIGNED_GAIN) {
            if (MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == true) {
                g_anc_control.passthru_runtime_gain = anc_runtime_gain;
            } else {
                g_anc_control.anc_runtime_gain = anc_runtime_gain;
            }
        }
        if (callback) {
            callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_CANCELLED);
        }
        ANC_LOG_I("audio_anc_enable cancelled due to suspend", 0);
        return ANC_CONTROL_EXECUTION_CANCELLED;
    }
    #ifdef MTK_USER_TRIGGER_FF_ENABLE
    #ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    if (audio_anc_user_trigger_ff_get_status()) {
        ANC_LOG_I("[user_trigger_ff]ANC cannot enable during user trigger adaptive ff", 0);
        return ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
    #endif
    #endif
#ifdef MTK_AWS_MCE_ENABLE
    return anc_sync_control(ANC_CONTROL_EVENT_ON, arg, callback);
#else
    return audio_anc_control(ANC_CONTROL_EVENT_ON, arg, callback);
#endif
}

anc_control_result_t audio_anc_disable(anc_control_callback_t callback)
{
    if (g_anc_control.suspend == 1) {
        g_anc_control.backup_enable = 0;
        if (callback) {
            callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_CANCELLED);
        }
        ANC_LOG_I("audio_anc_disable cancelled due to suspend", 0);
        return ANC_CONTROL_EXECUTION_CANCELLED;
    }
#ifdef MTK_AWS_MCE_ENABLE
    return anc_sync_control(ANC_CONTROL_EVENT_OFF, 0, callback);
#else
    return audio_anc_control(ANC_CONTROL_EVENT_OFF, 0, callback);
#endif
}

anc_control_result_t audio_anc_suspend(anc_control_callback_t callback, bool power_off)
{
    ANC_LOG_I("audio_anc_suspend power_off:%d ,suspend:%d", 2, power_off, g_anc_control.suspend);
    if(!g_anc_control.suspend) {
        g_anc_control.suspend = 1;
        g_anc_control.backup_enable = g_anc_control.enable;
    }
    return audio_anc_control(ANC_CONTROL_EVENT_OFF, power_off, callback);
}

anc_control_result_t audio_anc_set_volume(anc_sw_gain_t anc_sw_gain, anc_to_role_t to_role)
{
#ifdef MTK_AWS_MCE_ENABLE
    if (to_role == TO_AGENT) {
        return audio_anc_control(ANC_CONTROL_EVENT_SET_VOLUME, anc_sw_gain.val, NULL);
    } else if (to_role == TO_PARTNER) {
        if ((bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
            return anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_SET_VOLUME, anc_sw_gain.val, 0);
        } else {
            ANC_LOG_E("set partner volume error : bt power off or aws link state isn't attached", 0);
            return ANC_CONTROL_EXECUTION_FAIL;
        }
    } else {
        return anc_sync_control(ANC_CONTROL_EVENT_SET_VOLUME, anc_sw_gain.val, NULL);
    }
#else
    return audio_anc_control(ANC_CONTROL_EVENT_SET_VOLUME, anc_sw_gain.val, NULL);
#endif
}

anc_control_result_t audio_anc_set_runtime_volume(int16_t anc_runtime_gain)
{
#ifdef MTK_AWS_MCE_ENABLE
    return anc_sync_control(ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME, (uint32_t)anc_runtime_gain, NULL);
#else
    return audio_anc_control(ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME, (uint32_t)anc_runtime_gain, NULL);
#endif
}

anc_control_result_t audio_anc_read_volume_nvdm(anc_sw_gain_t *anc_sw_gain, anc_to_role_t to_role)
{
    if (to_role == TO_PARTNER) {
#ifdef MTK_AWS_MCE_ENABLE
        if ((bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
            return anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_READ_VOLUME_NVDM, 0, ((5 << 8) | 1));
        } else {
            ANC_LOG_E("read partner nvdm error : bt power off or aws link state isn't attached", 0);
            return ANC_CONTROL_EXECUTION_FAIL;
        }
#else
        return ANC_CONTROL_EXECUTION_NONE;
#endif
    } else {
        return anc_read_volume_nvdm(anc_sw_gain);
    }
}

anc_control_result_t audio_anc_write_volume_nvdm(anc_sw_gain_t *anc_sw_gain, anc_to_role_t to_role)
{
    if (to_role == TO_PARTNER) {
#ifdef MTK_AWS_MCE_ENABLE
        if ((bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() == BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
            return anc_send_aws_mce_param(ANC_DIRECT, ANC_CONTROL_EVENT_WRITE_VOLUME_NVDM, (uint32_t)anc_sw_gain->val, 0);
        } else {
            ANC_LOG_E("write partner nvdm error : bt power off or aws link state isn't attached", 0);
            return ANC_CONTROL_EXECUTION_FAIL;
        }
#else
        return ANC_CONTROL_EXECUTION_NONE;
#endif
    } else {
        return anc_write_volume_nvdm(anc_sw_gain);
    }
}

anc_control_result_t audio_anc_resume(anc_control_callback_t callback, bool power_on)
{
    uint32_t arg;
    if (((power_on == false) && (g_anc_control.backup_enable == 0)) || (g_anc_control.suspend == 0)) {
        if (callback) {
            callback(ANC_CONTROL_EVENT_ON, ANC_CONTROL_EXECUTION_CANCELLED);
        }
        g_anc_control.suspend = 0;
        ANC_LOG_I("audio_anc_resume power_on:%d no need to do anc_on, backup_enable:%d suspend:%d", 3, power_on, g_anc_control.backup_enable, g_anc_control.suspend);
        return ANC_CONTROL_EXECUTION_CANCELLED;
    }
    if (MACRO_ANC_CHK_PASSTHRU(g_anc_control.cur_filter_type) == true) {
        arg = ((uint32_t)g_anc_control.cur_filter_type << 16) | ((uint32_t)g_anc_control.passthru_runtime_gain & 0xFFFF);
    } else {
        arg = ((uint32_t)g_anc_control.cur_filter_type << 16) | ((uint32_t)g_anc_control.anc_runtime_gain & 0xFFFF);
    }
    ANC_LOG_I("audio_anc_resume power_on:%d ", 1, power_on);
    g_anc_control.suspend = 0;
    return audio_anc_control(ANC_CONTROL_EVENT_ON, ((power_on?1:0) << 31) | arg, callback);
}

anc_control_result_t audio_anc_control(anc_control_event_t event_id, uint32_t arg, anc_control_callback_t callback)
{
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
    if(bt_sink_srv_ami_get_current_scenario() == HFP){
        ANC_LOG_I("audio_anc_control ignore event:%d because HFP happening.", 1, event_id);
        return ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
#endif
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    if ((event_id == ANC_CONTROL_EVENT_ON) && (audio_anc_leakage_compensation_get_status() == 1)) {
        ANC_LOG_I("audio_anc_control ignore event:%d because LD happening.", 1, event_id);
        return ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
    if (event_id == ANC_CONTROL_EVENT_LD_START) {
        if (bt_sink_srv_ami_get_current_scenario() == A2DP) {
            am_audio_dl_suspend();
        }
        ANC_LOG_I("[RECORD_LC] anc leakage detection start in audio_anc_control",0);
        audio_anc_leakage_compensation_set_status(1);
        if (callback == NULL) {
            audio_anc_leakage_compensation_start(anc_leakage_detection_racecmd_callback);
        } else {
            audio_anc_leakage_compensation_start((anc_leakage_compensation_callback_t)callback);
        }
        return AUD_EXECUTION_SUCCESS;
    } else if (event_id == ANC_CONTROL_EVENT_LD_STOP) {
        ANC_LOG_I("[RECORD_LC] anc leakage detection stop in audio_anc_control, enabled:%d",1,audio_anc_leakage_compensation_get_status());
        if (audio_anc_leakage_compensation_get_status() == 1) {
            audio_anc_leakage_compensation_terminate();
        }
        return AUD_EXECUTION_SUCCESS;
    } else if (event_id == ANC_CONTROL_EVENT_LD_DL_RESUME) {
        ANC_LOG_I("[RECORD_LC] anc leakage detection resume dl",0);
        am_audio_dl_resume();
        g_anc_control.timer_available = 1;
        if (callback) {
            callback(0,0);
        }
        return AUD_EXECUTION_SUCCESS;
    } else if (audio_anc_leakage_compensation_get_status() && (event_id & (ANC_CONTROL_EVENT_ON | ANC_CONTROL_EVENT_OFF))) {
        //disable ANC during leakage detection
        ANC_LOG_I("audio_anc_control ignore event:%d because leakage detection happening.", 1, event_id);
        return ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    if (audio_anc_user_trigger_ff_get_status()) {
        ANC_LOG_I("[user_trigger_ff]ANC cannot enable during user trigger adaptive ff", 0);
        return ANC_CONTROL_EXECUTION_NOT_ALLOWED;
    }
#endif
#endif

    bt_sink_srv_am_feature_t feature_param;
    memset(&feature_param, 0, sizeof(bt_sink_srv_am_feature_t));
    feature_param.type_mask = AM_ANC;
    feature_param.feature_param.anc_param.event = event_id;
    feature_param.feature_param.anc_param.anc_control_callback = callback;
    ANC_LOG_I("audio_anc_control event:%d", 1, event_id);
    switch(event_id)
    {
        case ANC_CONTROL_EVENT_ON: {
            anc_filter_type_t filter_type = (anc_filter_type_t)(arg >> 16);
            int16_t runtime_gain = (int16_t)(arg & 0xFFFF);
            feature_param.feature_param.anc_param.filter = filter_type;
            feature_param.feature_param.anc_param.user_runtime_gain = runtime_gain;
            feature_param.feature_param.anc_param.param = (uint16_t)((arg >> 31) & 0x1);
            break;
        }
        case ANC_CONTROL_EVENT_OFF:
            feature_param.feature_param.anc_param.param = (uint16_t)arg;
            break;
        case ANC_CONTROL_EVENT_SET_VOLUME:
            feature_param.feature_param.anc_param.sw_gain.val = arg;
            break;
        case ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME:
            feature_param.feature_param.anc_param.user_runtime_gain = (int16_t)arg;
            break;
        case ANC_CONTROL_EVENT_READ_VOLUME_NVDM:
            return anc_read_volume_nvdm((anc_sw_gain_t *)arg);
        case ANC_CONTROL_EVENT_WRITE_VOLUME_NVDM: {
            anc_sw_gain_t anc_sw_gain;
            anc_sw_gain.val = arg;
            return anc_write_volume_nvdm(&anc_sw_gain);
        }
        case ANC_CONTROL_EVENT_SET_ATTACH_FUNC:
            feature_param.feature_param.anc_param.param = (uint16_t)arg;
            break;
        default:
            return ANC_CONTROL_EXECUTION_FAIL;
    }
    am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_param);
    return AUD_EXECUTION_SUCCESS;
}

anc_control_result_t audio_anc_register_callback(anc_control_callback_t callback, anc_control_event_t event_mask, anc_callback_level_t level)
{
    U32 i;
    for (i = 0; i < ANC_MAX_CALLBACK_NUM; i++)
    {
        if (g_anc_cb_service.callback_list[i] == NULL) {
            g_anc_cb_service.event_mask[i] = event_mask;
            g_anc_cb_service.cb_level[i] = level;
            g_anc_cb_service.callback_list[i] = callback;
            return ANC_CONTROL_EXECUTION_SUCCESS;
        }
    }
    return ANC_CONTROL_EXECUTION_FAIL;
}

anc_control_result_t audio_anc_deregister_callback(anc_control_callback_t callback)
{
    U32 i;
    for (i = 0; i < ANC_MAX_CALLBACK_NUM; i++)
    {
        if (g_anc_cb_service.callback_list[i] == callback) {
            g_anc_cb_service.callback_list[i] = NULL;
            g_anc_cb_service.event_mask[i] = 0;
            g_anc_cb_service.cb_level[i] = 0;
            return ANC_CONTROL_EXECUTION_SUCCESS;
        }
    }
    return ANC_CONTROL_EXECUTION_FAIL;
}

anc_control_result_t audio_anc_backup_while_suspend(uint8_t anc_enable, uint8_t anc_filter_type, int16_t anc_runtime_gain)
{
    if (g_anc_control.suspend == 1) {
        if (anc_enable == 1) {
            g_anc_control.backup_enable = 1;
            g_anc_control.cur_filter_type = (anc_filter_type_t)anc_filter_type;
            if (anc_runtime_gain != (int16_t)ANC_UNASSIGNED_GAIN) {
                if (MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == true) {
                    g_anc_control.passthru_runtime_gain = anc_runtime_gain;
                } else {
                    g_anc_control.anc_runtime_gain = anc_runtime_gain;
                }
            }
        } else {
            g_anc_control.backup_enable = 0;
        }
        ANC_LOG_I("audio_anc_backup_while_suspend due to suspend, enable:%d filter:%d gain:%d", 3, anc_enable, anc_filter_type, anc_runtime_gain);
        return ANC_CONTROL_EXECUTION_CANCELLED;
    }
    return ANC_CONTROL_EXECUTION_SUCCESS;
}
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
void audio_anc_user_trigger_ff_CCNI_demo_callback(hal_audio_event_t event, void *data)
{
    extern int16_t g_record_Rdemo_data[128];
    extern bool g_dump;
    switch(event){
        case HAL_AUDIO_EVENT_ERROR:
            ANC_LOG_I("[user_trigger_ff][CALLBACK]HAL_AUDIO_EVENT_ERROR\r\n", 0);
            break;

        case HAL_AUDIO_EVENT_DATA_NOTIFICATION:
        if(g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF){
            /*case: Receive report*/
            if (*((uint16_t *)data)) {
                uint16_t amount = *((uint16_t *)data);
                ANC_LOG_I("[user_trigger_ff][CALLBACK]cm4 received report, data_length:%d\r\n", 1, amount);
                n9_dsp_share_info_t *p_info = hal_audio_query_record_share_info();
                uint32_t data_byte_count;

                // Check data amount
                data_byte_count = hal_audio_buf_mgm_get_data_byte_count(p_info);
                ANC_LOG_I("[user_trigger_ff][CALLBACK]data_byte_count=%d, wo %d , ro %d", 3, data_byte_count, p_info->write_offset, p_info->read_offset);

                g_dump = false;
                if (RECORD_CONTROL_EXECUTION_SUCCESS == audio_record_control_read_data(&g_user_trigger_ff_info.report_array, (sizeof(int32_t) * amount))) {
                    ANC_LOG_I("[user_trigger_ff][CALLBACK]status %d", 1, *(g_user_trigger_ff_info.report_array));
                    if (*(g_user_trigger_ff_info.report_array) < ANC_K_STATUS_Compare) { /*return cmd A response: 300 tap result*/
                        if(*(g_user_trigger_ff_info.report_array) == ANC_K_STATUS_RUNNING)
                        {
                            *(g_user_trigger_ff_info.report_array) = ANC_K_STATUS_DONE;
                            ANC_LOG_I("[user_trigger_ff][CALLBACK]status 2->3", 0);
                        }
                        if (g_user_trigger_ff_info.api_callback) {
                            g_user_trigger_ff_info.api_callback(*(g_user_trigger_ff_info.report_array));
                            g_user_trigger_ff_info.api_callback = audio_anc_user_trigger_ff_cb;
                        } else {
                            ANC_LOG_W("[user_trigger_ff][CALLBACK]api_callback == NULL", 0);
                        }

                    } else if (*(g_user_trigger_ff_info.report_array) <= ANC_K_STATUS_UPDATE){/*return comparing filter result*/
//                        if (*(g_user_trigger_ff_info.report_array) == ANC_K_STATUS_UPDATE) {
                            ANC_LOG_I("[user_trigger_ff][CALLBACK]write filter %d to nvdm", 1, FILTER_3);
                            audio_anc_user_trigger_ff_write_filter_nvdm(FILTER_3, g_user_trigger_ff_info.cmp_new_filter);
//                        }
                        if (g_user_trigger_ff_info.api_callback) {
                            g_user_trigger_ff_info.api_callback(*(g_user_trigger_ff_info.report_array));
                            g_user_trigger_ff_info.api_callback = audio_anc_user_trigger_ff_cb;
                        } else {
                            ANC_LOG_W("[user_trigger_ff][CALLBACK]api_callback == NULL", 0);
                        }

                    } else {
                        ANC_LOG_I("[user_trigger_ff][CALLBACK]no race cmd B, stop record", 0);
                        audio_anc_user_trigger_ff_stop();
                        audio_anc_user_trigger_ff_recover_anc(ANC_K_STATUS_KEEP_DEFAULT);
                    }

                } else {
                    ANC_LOG_I("[user_trigger_ff][CALLBACK]read report array failed\r\n", 0);
                }

            /*case: Record*/
            } else {
                for(uint16_t i=0; i < 8 ; i++)
                {
                    if (RECORD_CONTROL_EXECUTION_SUCCESS == audio_record_control_read_data(&g_record_Rdemo_data, (sizeof(int16_t) * 125))){
                        //LOG_AUDIO_DUMP(g_record_Rdemo_data, sizeof(int16_t) * 128, VOICE_TX_MIC_3);
                    }else {
                        ANC_LOG_I("[user_trigger_ff][CALLBACK]read stream in failed\r\n", 0);
                    }
                }
            }
        }

        break;
    }

}

void audio_anc_user_trigger_ff_AM_notify_callback(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_cb_msg_class_t msg_id, bt_sink_srv_am_cb_sub_msg_t sub_msg, void *parm)
{
    ANC_LOG_I("[AT][RECORD]AM_CB, aud_id:%x, msg_id:%x, sub_msg:%x", aud_id, msg_id, sub_msg);
    if (msg_id == AUD_SINK_OPEN_CODEC) {
        if(sub_msg == AUD_CMD_COMPLETE){
            //Start Record Success
        } else if (sub_msg == AUD_CMD_FAILURE) {
            //Start Record Fail
            race_dsprealtime_anc_adaptive_notify_end();
        } else {
            //[error]
        }
    }else if(AUD_SELF_CMD_REQ){
        if(sub_msg == AUD_CMD_COMPLETE){
            //Stop Record Request Success
        } else if (sub_msg == AUD_CMD_FAILURE) {
            //Start Record Request Fail because HFP exist. /*Reject Request.*/
            audio_anc_user_trigger_ff_terminate();
            ANC_LOG_I("[user_trigger_ff]AM_CB, Start Record Request Fail because higher priority scenario exists", 0);
            race_dsprealtime_anc_adaptive_notify_end();
        } else {
            //[error]
        }
    }else if(AUD_SUSPEND_BY_IND){
        if(sub_msg == AUD_SUSPEND_BY_HFP){
            //Suspend record because HFP interrupt.
            ANC_LOG_I("[user_trigger_ff]AM_CB, AUD_SUSPEND_BY_HFP", 0);
            audio_anc_user_trigger_ff_terminate();
            race_dsprealtime_anc_adaptive_notify_end();
        } else {
            //[error]
            ANC_LOG_I("[user_trigger_ff]AM_CB, AUD_SUSPEND_BY_IND", 0);
            audio_anc_user_trigger_ff_terminate();
            race_dsprealtime_anc_adaptive_notify_end();
        }
    }else{
        //[error]
    }
}

int32_t audio_anc_user_trigger_ff_write_filter_nvdm(anc_filter_type_t anc_filter_type, anc_fwd_iir_t *filter)
{
    ANC_LOG_I("[user_trigger_ff]audio_anc_user_trigger_ff_write_filter_nvdm", 0);
    uint16_t nvkey_l;
//    uint16_t nvkey_r;
    anc_fwd_iir_t *tmp_buffer = NULL;
    uint32_t buf_size;
    sysram_status_t status;

    switch (anc_filter_type) {
    case FILTER_1:
    default:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER1;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER1;
        break;
    case FILTER_2:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER2;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER2;
        break;
    case FILTER_3:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER3;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER3;
        break;
    case FILTER_4:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER4;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER4;
        break;
    case FILTER_5:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER5;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER5;
        break;
    }

    buf_size = sizeof(anc_fwd_iir_t);
    tmp_buffer = (anc_fwd_iir_t *)pvPortMalloc(buf_size);
    if(filter != NULL){
        status = flash_memory_write_nvdm_data(nvkey_l, (uint8_t *)filter, buf_size);
        if(status != NVDM_STATUS_NAT_OK) ANC_LOG_E("[user_trigger_ff]anc failed to write 0x%x from nvdm - err(%d)\r\n", 2, nvkey_l,status);

//        status = flash_memory_write_nvdm_data(nvkey_r, (uint8_t *)filter, buf_size);
//        if(status != NVDM_STATUS_NAT_OK) ANC_LOG_E("[user_trigger_ff]anc failed to write 0x%x from nvdm - err(%d)\r\n", 2, nvkey_r,status);
    } else {
        ANC_LOG_E("[user_trigger_ff]audio_anc_user_trigger_ff_write_filter_nvdm, filter == NULL", 0);
    }
    vPortFree((void *)tmp_buffer);

    return 0;
}

int32_t audio_anc_user_trigger_ff_read_filter_nvdm(anc_filter_type_t anc_filter_type)
{
    ANC_LOG_I("[user_trigger_ff]audio_anc_user_trigger_ff_read_filter_nvdm, anc_filter_type:%d", anc_filter_type);
    uint16_t nvkey_l;
//    uint16_t nvkey_r;
    anc_fwd_iir_t *tmp_buffer = NULL;
    uint32_t buf_size;
    sysram_status_t status;

    switch (anc_filter_type) {
    case FILTER_1:
    default:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER1;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER1;
        break;
    case FILTER_2:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER2;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER2;
        break;
    case FILTER_3:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER3;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER3;
        break;
    case FILTER_4:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER4;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER4;
        break;
    case FILTER_5:
        nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER5;
//        nvkey_r = NVKEY_DSP_PARA_ANC_R_FILTER5;
        break;
    }

    buf_size = sizeof(anc_fwd_iir_t);
    tmp_buffer = (anc_fwd_iir_t *)pvPortMalloc(buf_size);

    /*read L filter*/
    status = flash_memory_read_nvdm_data(nvkey_l, (uint8_t *)tmp_buffer, &buf_size);

    if(status != NVDM_STATUS_NAT_OK) {
        ANC_LOG_E("[user_trigger_ff]anc failed to read 0x%x from nvdm - err(%d)\r\n", 2, nvkey_l,status);
    }
    else {
        ANC_LOG_I("[user_trigger_ff]anc read 0x%x from nvdm\r\n", 2, nvkey_l);
        ANC_LOG_I("filter_unique_gain=%d, iir_filter_coef_B00=%d, shift_a=%d, shift_b=%d", 4, tmp_buffer->filter_unique_gain, tmp_buffer->iir_filter_coef_B00, tmp_buffer->shift_a, tmp_buffer->shift_b);
        ANC_LOG_I("iir_filter_coef[64]:", 0);
        for (int i=0; i<64; i+=8) {
            ANC_LOG_I("%d, %d, %d, %d, %d, %d, %d, %d", 8,
                *(tmp_buffer->iir_filter_coef+i), *(tmp_buffer->iir_filter_coef+i+1), *(tmp_buffer->iir_filter_coef+i+2), *(tmp_buffer->iir_filter_coef+i+3),
                *(tmp_buffer->iir_filter_coef+i+4), *(tmp_buffer->iir_filter_coef+i+5), *(tmp_buffer->iir_filter_coef+i+6), *(tmp_buffer->iir_filter_coef+i+7));
        }
    }
    /*read R filter*/
//    status = flash_memory_read_nvdm_data(nvkey_r, (uint8_t *)tmp_buffer, &buf_size);
//    if(status != NVDM_STATUS_NAT_OK) {
//        ANC_LOG_E("[user_trigger_ff]anc failed to read 0x%x from nvdm - err(%d)\r\n", 2, nvkey_r,status);
//    }
//    else {
//        ANC_LOG_I("[user_trigger_ff]anc read 0x%x from nvdm\r\n", 2, nvkey_r);
//        printf("filter_unique_gain=%d, iir_filter_coef_B00=%d, shift_a=%d, shift_b=%d", tmp_buffer->filter_unique_gain, tmp_buffer->iir_filter_coef_B00, tmp_buffer->shift_a, tmp_buffer->shift_b);
//        printf("iir_filter_coef[64]:");
//        for (int i=0; i<64; i+=8) {
//            printf("%d, %d, %d, %d, %d, %d, %d, %d",
//                *(tmp_buffer->iir_filter_coef+i), *(tmp_buffer->iir_filter_coef+i+1), *(tmp_buffer->iir_filter_coef+i+2), *(tmp_buffer->iir_filter_coef+i+3),
//                *(tmp_buffer->iir_filter_coef+i+4), *(tmp_buffer->iir_filter_coef+i+5), *(tmp_buffer->iir_filter_coef+i+6), *(tmp_buffer->iir_filter_coef+i+7));
//        }
//    }


    vPortFree((void *)tmp_buffer);

    return 0;
}

int32_t audio_anc_user_trigger_ff_read_Sz_coef_nvdm(void)
{
    sysram_status_t status;
    anc_user_trigger_adaptive_ff_sz_coef_nvdm_t *read_sz_buff = NULL;
    uint32_t buf_size;

    buf_size = sizeof(anc_user_trigger_adaptive_ff_sz_coef_nvdm_t);
    read_sz_buff = (anc_user_trigger_adaptive_ff_sz_coef_nvdm_t *)pvPortMalloc(buf_size);
    status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_ADAPTIVE_FF_SZ_COEF, (uint8_t *)read_sz_buff, &buf_size);
    if(status != NVDM_STATUS_NAT_OK) {
        ANC_LOG_E("[user_trigger_ff]failed to read Sz coef(0x%x) from nvdm - err(%d)\r\n", 2, NVKEY_DSP_PARA_ADAPTIVE_FF_SZ_COEF,status);
    } else {
        ANC_LOG_I("[user_trigger_ff]read Sz coef array, length: %d\r\n", 1, read_sz_buff->Sz_length);
        ANC_LOG_I("[user_trigger_ff]==========array B==========\r\n", 0);
        for (int i=0; i<10; i++) {
            ANC_LOG_I("row %2d | %10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\r\n", 11, i,
                *(read_sz_buff->utff_sz_coef_array_B+i*10), *(read_sz_buff->utff_sz_coef_array_B+i*10+1), *(read_sz_buff->utff_sz_coef_array_B+i*10+2), *(read_sz_buff->utff_sz_coef_array_B+i*10+3), *(read_sz_buff->utff_sz_coef_array_B+i*10+4),
                *(read_sz_buff->utff_sz_coef_array_B+i*10+5), *(read_sz_buff->utff_sz_coef_array_B+i*10+6), *(read_sz_buff->utff_sz_coef_array_B+i*10+7), *(read_sz_buff->utff_sz_coef_array_B+i*10+8), *(read_sz_buff->utff_sz_coef_array_B+i*10+9));
        }
        ANC_LOG_I("[user_trigger_ff]==========array A==========\r\n", 0);
        for (int i=0; i<10; i++) {
            ANC_LOG_I("row %2d | %10d %10d %10d %10d %10d %10d %10d %10d %10d %10d\r\n", 11, i,
                *(read_sz_buff->utff_sz_coef_array_A+i*10), *(read_sz_buff->utff_sz_coef_array_A+i*10+1), *(read_sz_buff->utff_sz_coef_array_A+i*10+2), *(read_sz_buff->utff_sz_coef_array_A+i*10+3), *(read_sz_buff->utff_sz_coef_array_A+i*10+4),
                *(read_sz_buff->utff_sz_coef_array_A+i*10+5), *(read_sz_buff->utff_sz_coef_array_A+i*10+6), *(read_sz_buff->utff_sz_coef_array_A+i*10+7), *(read_sz_buff->utff_sz_coef_array_A+i*10+8), *(read_sz_buff->utff_sz_coef_array_A+i*10+9));
        }

    }

    vPortFree((void *)read_sz_buff);
    return 0;
}

void audio_anc_user_trigger_ff_write_Sz_coef_share_buff(void)
{
    ANC_LOG_I("[user_trigger_ff]audio_anc_user_trigger_ff_write_Sz_coef_share_buff\r\n", 0);
    sysram_status_t     status;
    DSP_FEATURE_TYPE_LIST AudioFeatureList_adaptive_ff[2] =
    {
        FUNC_ADAPTIVE_FF,
        FUNC_END,
    };

    /* reset share buffer before put parameters*/
    audio_nvdm_reset_sysram();
    status = audio_nvdm_set_feature(2, AudioFeatureList_adaptive_ff);
    if (status != NVDM_STATUS_NAT_OK){
        ANC_LOG_I("[user_trigger_ff]failed to set parameters to share memory - err(%d)\r\n", 1, status);
        platform_assert("[user_trigger_ff]failed to set parameters to share memory", __FILE__, __LINE__);
    }
}

void audio_anc_user_trigger_ff_cb(int32_t Cal_status)
{
    ANC_LOG_I("[user_trigger_ff]audio_anc_user_trigger_ff_cb, Cal_status:%d", 1, Cal_status);

    audio_anc_user_trigger_ff_stop();
    audio_anc_user_trigger_ff_recover_anc(ANC_K_STATUS_KEEP_DEFAULT);
}

void audio_anc_user_trigger_ff_start(anc_user_trigger_ff_callback_t callback)
{
    extern uint8_t g_record_user_trigger_ff_id;
    uint8_t anc_status;
    int16_t anc_runtime_gain;

//    audio_anc_user_trigger_ff_read_Sz_coef_nvdm();

    /*initialize*/
    memset(&g_user_trigger_ff_info, 0, sizeof(g_user_trigger_ff_info));
    ANC_LOG_I("[user_trigger_ff]audio_anc_user_trigger_ff_start", 0);

    /*turn off ANC if needed*/
    anc_get_status(&g_user_trigger_ff_info.anc_enable, &g_user_trigger_ff_info.anc_runtime_info, NULL);
    anc_status = (uint8_t)(g_user_trigger_ff_info.anc_runtime_info & ANC_FILTER_TYPE_MASK);
    anc_runtime_gain = (uint16_t)(g_user_trigger_ff_info.anc_runtime_info & 0xFFFF0000);
    if (g_user_trigger_ff_info.anc_enable) {
        audio_anc_disable(NULL);
        ANC_LOG_I("[user_trigger_ff]g_user_trigger_ff_info.anc_enable = %d, anc_status = 0x%x, runtime_gain = %d", 3, g_user_trigger_ff_info.anc_enable, anc_status, anc_runtime_gain);
    }
    ANC_LOG_I("[user_trigger_ff]g_user_trigger_ff_info.anc_enable = %d, anc_status = 0x%x, runtime_gain = %d", 3, g_user_trigger_ff_info.anc_enable, anc_status, anc_runtime_gain);

    g_user_trigger_ff_info.enable = 1;
    g_user_trigger_ff_info.api_callback = (callback == NULL) ? audio_anc_user_trigger_ff_cb : callback;

    /*Request AM*/
    if (g_record_user_trigger_ff_id == 0) {

        record_encoder_cability_t encoder_capability;
        encoder_capability.codec_type = AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF;
        encoder_capability.bit_rate = ENCODER_BITRATE_16KBPS;
        encoder_capability.wwe_mode = WWE_MODE_NONE;
        g_record_user_trigger_ff_id = audio_record_control_enabling_encoder_init(audio_anc_user_trigger_ff_CCNI_demo_callback, &g_user_trigger_ff_info.dsp_msg_cb_data, audio_anc_user_trigger_ff_AM_notify_callback,&encoder_capability);
        ANC_LOG_I("[user_trigger_ff]codec_type:%d, g_record_user_trigger_ff_id %d, *user_data:0x%x", 3, encoder_capability.codec_type, g_record_user_trigger_ff_id, &g_user_trigger_ff_info.dsp_msg_cb_data);
    }

    audio_record_control_start(g_record_user_trigger_ff_id);
}

void audio_anc_user_trigger_ff_stop(void)
{
    extern uint8_t g_record_user_trigger_ff_id;
//    extern anc_user_trigger_ff_param_t g_user_trigger_ff_info;

    if (g_record_user_trigger_ff_id != 0) {
        audio_record_control_stop(g_record_user_trigger_ff_id);
        ANC_LOG_I("[user_trigger_ff]audio_anc_user_trigger_ff_stop", 0);
    }

    /*initialize*/
    g_user_trigger_ff_info.enable = 0;
    g_user_trigger_ff_info.api_callback = audio_anc_user_trigger_ff_cb;

    /*free memory*/
    if (g_user_trigger_ff_info.cmp_ori_filter) {
        vPortFree((void *)g_user_trigger_ff_info.cmp_ori_filter);
        g_user_trigger_ff_info.cmp_ori_filter = NULL;
    }
    if (g_user_trigger_ff_info.cmp_new_filter) {
        vPortFree((void *)g_user_trigger_ff_info.cmp_new_filter);
        g_user_trigger_ff_info.cmp_new_filter = NULL;
    }
}

void audio_anc_user_trigger_ff_receive_filter(anc_user_trigger_ff_callback_t callback, uint8_t *filter_ptr, uint32_t filter_length)
{
    uint32_t buf_size = sizeof(anc_fwd_iir_t);
    sysram_status_t status;

    /*ori filter nvkey*/
    uint16_t nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER1;
    if (g_user_trigger_ff_info.anc_enable) {
        uint8_t anc_status = (uint8_t)(g_user_trigger_ff_info.anc_runtime_info & ANC_FILTER_TYPE_MASK);
        switch (anc_status) {
            case FILTER_1:
            default:
                nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER1;
                break;
            case FILTER_2:
                nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER2;
                break;
            case FILTER_3:
                nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER3;
                break;
            case FILTER_4:
                nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER4;
                break;
            case FILTER_5:
                nvkey_l = NVKEY_DSP_PARA_ANC_L_FILTER5;
                break;
        }
    }


    if (adaptive_ff_cmp_filter) {
        anc_fwd_iir_t *cmp_ori_filter = (anc_fwd_iir_t *)pvPortMalloc(buf_size);
        status = flash_memory_read_nvdm_data(nvkey_l, (uint8_t *)cmp_ori_filter, &buf_size);
        if(status != NVDM_STATUS_NAT_OK) {
            ANC_LOG_E("[user_trigger_ff]anc failed to read 0x%x from nvdm - err(%d)\r\n", 2, nvkey_l,status);
        }
        else {
            g_user_trigger_ff_info.cmp_ori_filter = cmp_ori_filter;
            ANC_LOG_I("[user_trigger_ff]anc read ori filter 0x%x from nvdm\r\n", 2, nvkey_l);
            #ifdef UTFF_DEBUG_PRINT
            ANC_LOG_I("filter_unique_gain=%d, iir_filter_coef_B00=%d, shift_a=%d, shift_b=%d", 4, cmp_ori_filter->filter_unique_gain, cmp_ori_filter->iir_filter_coef_B00, cmp_ori_filter->shift_a, cmp_ori_filter->shift_b);
            ANC_LOG_I("iir_filter_coef[64]:", 0);
            for (int i=0; i<64; i+=8) {
                ANC_LOG_I("%d, %d, %d, %d, %d, %d, %d, %d", 8,
                    *(cmp_ori_filter->iir_filter_coef+i), *(cmp_ori_filter->iir_filter_coef+i+1), *(cmp_ori_filter->iir_filter_coef+i+2), *(cmp_ori_filter->iir_filter_coef+i+3),
                    *(cmp_ori_filter->iir_filter_coef+i+4), *(cmp_ori_filter->iir_filter_coef+i+5), *(cmp_ori_filter->iir_filter_coef+i+6), *(cmp_ori_filter->iir_filter_coef+i+7));
            }
            #endif
        }


        anc_fwd_iir_t *cmp_new_filter = (anc_fwd_iir_t *)pvPortMalloc(buf_size);
        memcpy(cmp_new_filter, filter_ptr, buf_size);
        g_user_trigger_ff_info.cmp_new_filter = cmp_new_filter;
        ANC_LOG_I("[user_trigger_ff]audio_anc_user_trigger_ff_receive_filter, sizeof(anc_fwd_iir_t)=%d, filter_length=%d", 2, buf_size, filter_length);
        #ifdef UTFF_DEBUG_PRINT
        ANC_LOG_I("filter_unique_gain=%d, iir_filter_coef_B00=%d, shift_a=%d, shift_b=%d", 4, cmp_new_filter->filter_unique_gain, cmp_new_filter->iir_filter_coef_B00, cmp_new_filter->shift_a, cmp_new_filter->shift_b);
        ANC_LOG_I("iir_filter_coef[64]:", 0);
        for (int i=0; i<64; i+=8) {
            ANC_LOG_I("%d, %d, %d, %d, %d, %d, %d, %d", 8,
                *(cmp_new_filter->iir_filter_coef+i), *(cmp_new_filter->iir_filter_coef+i+1), *(cmp_new_filter->iir_filter_coef+i+2), *(cmp_new_filter->iir_filter_coef+i+3),
                *(cmp_new_filter->iir_filter_coef+i+4), *(cmp_new_filter->iir_filter_coef+i+5), *(cmp_new_filter->iir_filter_coef+i+6), *(cmp_new_filter->iir_filter_coef+i+7));
        }
        #endif

        /*notify DSP via AM*/
        bt_sink_srv_am_feature_t feature_param;
        feature_param.type_mask = AM_ADAPTIVE_FF;
        feature_param.feature_param.adaptive_ff_param.cmp_ori_filter = cmp_ori_filter;
        feature_param.feature_param.adaptive_ff_param.cmp_new_filter = cmp_new_filter;
        am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_param);
        g_user_trigger_ff_info.api_callback = (callback == NULL) ? audio_anc_user_trigger_ff_cb : callback;

    } else {

        if (callback)
        {
            callback(ANC_K_STATUS_UPDATE);
        }

    }

}

bool audio_anc_user_trigger_ff_get_status(void)
{
    ANC_LOG_I("[user_trigger_ff]audio_anc_user_trigger_ff_get_status: %d", 1, g_user_trigger_ff_info.enable);
    return g_user_trigger_ff_info.enable;
}

void audio_anc_user_trigger_ff_recover_anc(int32_t Cal_status)
{
    uint8_t anc_filter_type;
    int16_t anc_runtime_gain;
//    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    /*turn on ANC if needed*/
    anc_filter_type = (uint8_t)(g_user_trigger_ff_info.anc_runtime_info & ANC_FILTER_TYPE_MASK);
    anc_runtime_gain = (uint16_t)(g_user_trigger_ff_info.anc_runtime_info & 0xFFFF0000);
    ANC_LOG_I("[user_trigger_ff]recover ANC, anc_enable = %d, anc_filter_type = 0x%x, runtime_gain = %d", 3, g_user_trigger_ff_info.anc_enable, anc_filter_type, anc_runtime_gain);

    if (g_user_trigger_ff_info.anc_enable) {
        //audio_anc_enable
        uint32_t arg;
        if (Cal_status == ANC_K_STATUS_UPDATE) {
            arg = ((uint32_t)FILTER_3 << 16) | ((uint32_t)anc_runtime_gain & 0xFFFF);
        } else {
            arg = ((uint32_t)anc_filter_type << 16) | ((uint32_t)anc_runtime_gain & 0xFFFF);
        }
        if (g_anc_control.suspend == 1) {
            g_anc_control.backup_enable = 1;
            g_anc_control.cur_filter_type = anc_filter_type;
            if (anc_runtime_gain != (int16_t)ANC_UNASSIGNED_GAIN) {
                if (MACRO_ANC_CHK_PASSTHRU(anc_filter_type) == true) {
                    g_anc_control.passthru_runtime_gain = anc_runtime_gain;
                } else {
                    g_anc_control.anc_runtime_gain = anc_runtime_gain;
                }
            }
            ANC_LOG_I("audio_anc_user_trigger_ff_recover_anc cancelled due to suspend", 0);
            return;
        }
        audio_anc_control(ANC_CONTROL_EVENT_ON, arg, NULL);
    }

}

void audio_anc_user_trigger_ff_terminate(void)
{
    if (g_user_trigger_ff_info.api_callback != NULL) {
        g_user_trigger_ff_info.api_callback(ANC_K_STATUS_ABORT);
    } else {
        audio_anc_user_trigger_ff_cb(ANC_K_STATUS_ABORT);
    }
    ANC_LOG_I("[user_trigger_ff] terminated by higher priority scenario", 0);
}
#endif

#endif

