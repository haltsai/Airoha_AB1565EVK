/* Copyright Statement:
 *
 * (C) 2005-2017  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

/* Includes ------------------------------------------------------------------*/
#ifdef MTK_PROMPT_SOUND_ENABLE

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_audio_setting.h"
#include "bt_sink_srv_ami.h"
#if defined(MTK_AVM_DIRECT)
    #include "hal_audio_cm4_dsp_message.h"
    #include "hal_audio_internal.h"
    #include "hal_dvfs_internal.h"
#else
    #include "hal_audio_internal_service.h"
#endif

#include "prompt_control.h"

#include "bt_sink_srv_am_task.h"
#ifdef AIR_STREAM_MANAGER_ENABLE
    #include "audio_stream_manager.h"
#endif

#define UNUSED(x)  ((void)(x))

log_create_module(VPC, PRINT_LEVEL_INFO);

/********************************************************
 * Macro & Define
 *
 ********************************************************/
#define PROMPT_CONTROL_MAIN_TRACK_GAIN    0x2000 /*weight 0.25*/
#define PROMPT_CONTROL_SIDE_TRACK_GAIN    0x8000 /*weight 1.0 */

#ifdef MTK_AVM_DIRECT
#ifdef MTK_PROMPT_SOUND_USING_CONFIG_DEFAULT_GAIN_LEVEL
#define PROMPT_CONTROL_DEFAULT_GAIN_LEVEL bt_sink_srv_ami_get_vp_default_volume_level()
#else
#define PROMPT_CONTROL_DEFAULT_GAIN_LEVEL 11
#endif
#endif

#ifdef  HAL_DVFS_MODULE_ENABLED
#ifdef HAL_DVFS_416M_SOURCE
#define VP_DVFS_DEFAULT_SPEED  HAL_DVFS_HIGH_SPEED_208M
#elif defined(HAL_DVFS_312M_SOURCE)
#define VP_DVFS_DEFAULT_SPEED  DVFS_78M_SPEED
#endif
#endif
/********************************************************
 * Global variable
 *
 ********************************************************/
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
static uint8_t    g_VP_dummy_source_gain_dB = 0;
SemaphoreHandle_t g_VP_dummy_source_semaphore_handle = NULL;
static prompt_control_dummy_source_handle_t g_VP_dummy_source_handle;
#endif /* AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE */

prompt_control_callback_t  g_app_callback = NULL;
prompt_control_callback_t  g_app_internal_callback = NULL;

#ifdef MTK_AVM_DIRECT
static bool       g_prompt_mute_lock = false;
#ifndef MTK_PROMPT_SOUND_USING_CONFIG_DEFAULT_GAIN_LEVEL
static uint8_t    g_prompt_gain_level = PROMPT_CONTROL_DEFAULT_GAIN_LEVEL;
#endif
SemaphoreHandle_t g_prompt_semaphore_handle = NULL;
#endif
extern volatile uint8_t g_audio_dl_suspend_by_user;

/********************************************************
 * Function
 *
 ********************************************************/

#ifdef MTK_AVM_DIRECT
prompt_control_status_t prompt_control_mutex_lock(void)
{
    prompt_control_status_t status = PROMPT_CONTROL_RETURN_OK;
    if ( g_prompt_semaphore_handle == NULL ) {
       g_prompt_semaphore_handle = xSemaphoreCreateMutex();
    }
    if ( g_prompt_semaphore_handle != NULL ) {
        LOG_MSGID_I(VPC, "[VPC] prompt_control_mutex_lock() +\r\n", 0);
        xSemaphoreTake(g_prompt_semaphore_handle, portMAX_DELAY);
        LOG_MSGID_I(VPC, "[VPC] prompt_control_mutex_lock() -\r\n", 0);
    }
    else {
        status = PROMPT_CONTROL_RETURN_ERROR;
    }

    return status;
}

prompt_control_status_t prompt_control_mutex_unlock(void)
{
    prompt_control_status_t status = PROMPT_CONTROL_RETURN_OK;

    if ( g_prompt_semaphore_handle != NULL ) {
        LOG_MSGID_I(VPC, "[VPC] prompt_control_mutex_unlock()\r\n", 0);
        xSemaphoreGive(g_prompt_semaphore_handle);
    }
    else {
        status = PROMPT_CONTROL_RETURN_ERROR;
    }

    return status;
}
#endif


#if !defined(MTK_AVM_DIRECT)
static void prompt_control_set_mixer_volume(void)
{
#if defined(MTK_AVM_DIRECT)
    uint32_t data32 = (mixer_main_track_gain<<16) | mixer_side_track_gain;

    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PROMPT_SET_VOLUME, 0, data32, false);
#else
    audio_mixer_status_t status;

    status = audio_mixer_set_volume(mixer_main_track_gain, mixer_side_track_gain);
    if (status != AUDIO_MIXER_STATUS_OK) {
        LOG_MSGID_I(VPC, "[VPC]Set mixer volume failed : status=%d\r\n",1, (int32_t)status);
    }
#endif
}
#endif //!MTK_AVM_DIRECT

#ifdef MTK_AVM_DIRECT
void prompt_control_set_volume(void)
{
    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    bt_sink_srv_audio_setting_vol_info_t vol_info;
    bt_sink_srv_audio_setting_vol_t vol;
    bt_sink_srv_am_type_t current_audio_type;
    uint32_t digital = 0;
    uint32_t analog = 0;

    vol_info.type = VOL_VP;
#ifndef MTK_PROMPT_SOUND_USING_CONFIG_DEFAULT_GAIN_LEVEL
    vol_info.vol_info.def_vol_info.lev = g_prompt_gain_level;
#else
    vol_info.vol_info.def_vol_info.lev = PROMPT_CONTROL_DEFAULT_GAIN_LEVEL;
#endif
    memset(&vol, 0, sizeof(bt_sink_srv_audio_setting_vol_t));

    bt_sink_srv_audio_setting_get_vol(&vol_info, &vol);
    digital = vol.vol.vp_vol.vol.digital;
    analog = vol.vol.vp_vol.vol.analog_L;

    current_audio_type = bt_sink_srv_ami_get_current_scenario();
    if(current_audio_type != NONE) {
        analog = 0x00007FFF; //don't care
    }

    hal_audio_set_stream_out_dl2_volume(digital, analog);

    LOG_MSGID_I(VPC, "[VPC]Set Prompt Volume Scenario:%d Level[%d] D_Gain:0x%x A_Gain:0x%x\n",4,
        current_audio_type, vol_info.vol_info.def_vol_info.lev, digital, analog);
}
#endif

#ifdef MTK_AVM_DIRECT
void prompt_control_set_mute(bool mute)
{
    bt_sink_srv_am_result_t ret = 0;
    g_prompt_mute_lock = mute;
    ret = bt_sink_srv_ami_audio_set_mute(FEATURE_NO_NEED_ID, mute, STREAM_OUT_2);
    LOG_MSGID_I(VPC, "[VPC]prompt_control_set_mute(0x%x), ret:0x%x", 2, mute, ret);
    UNUSED(ret);
}
#endif


#ifdef MTK_AVM_DIRECT
uint8_t prompt_control_get_level()
{
#ifndef MTK_PROMPT_SOUND_USING_CONFIG_DEFAULT_GAIN_LEVEL
    LOG_MSGID_I(VPC, "[VPC]get level:%d\n",1, g_prompt_gain_level);
    return g_prompt_gain_level;
#else
    LOG_MSGID_I(VPC, "[VPC]get level:%d\n",1, PROMPT_CONTROL_DEFAULT_GAIN_LEVEL);
    return PROMPT_CONTROL_DEFAULT_GAIN_LEVEL;
#endif
}

#ifndef MTK_PROMPT_SOUND_USING_CONFIG_DEFAULT_GAIN_LEVEL
void prompt_control_set_level(uint8_t vol_level)
{
    LOG_MSGID_I(VPC, "[VPC]set level:%d\n",1, vol_level);
    g_prompt_gain_level = vol_level;
}
#endif
#endif
static int g_vp_codec_id = -1;
void prompt_control_stop_tone_active(void);
void prompt_control_gpt_callback(void *user_data);
#ifdef AIR_STREAM_MANAGER_ENABLE
    bool prompt_control_play_codec(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, uint32_t sync_time, prompt_control_callback_t callback);
#endif
bool prompt_control_play_tone_internal(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, uint32_t sync_time, prompt_control_callback_t callback)
{
    bool ret = true;
    g_app_callback = callback;
#ifdef HAL_DVFS_MODULE_ENABLED
    hal_dvfs_lock_control(VP_DVFS_DEFAULT_SPEED, HAL_DVFS_LOCK);
#endif
    ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, true);
    ami_set_audio_mask(AM_TASK_MASK_VP_HAPPENING, true);
    if(!g_prompt_mute_lock){
        // if(AUD_EXECUTION_FAIL == bt_sink_srv_ami_audio_set_mute(FEATURE_NO_NEED_ID, false, STREAM_OUT_2)){
        //     LOG_MSGID_I(VPC, "[VPC]set un-mute fail\n",0);
        // }
        #if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
            hal_audio_mute_stream_out(false, HAL_AUDIO_STREAM_OUT2);
        #else
            hal_audio_mute_stream_out(false);
        #endif
    }
#ifdef AIR_STREAM_MANAGER_ENABLE
    if (prompt_control_play_codec(tone_type, tone_buf, tone_size, sync_time, callback) == false) {
        LOG_MSGID_I(VPC, "[VPC]play codec fail\n",0);
        #ifdef HAL_DVFS_MODULE_ENABLED
            hal_dvfs_lock_control(VP_DVFS_DEFAULT_SPEED, HAL_DVFS_UNLOCK);
        #endif
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, false);
        ami_set_audio_mask(AM_TASK_MASK_VP_HAPPENING, false);
        ret = false;
        if(!g_prompt_mute_lock){
            // if(AUD_EXECUTION_FAIL == bt_sink_srv_ami_audio_set_mute(FEATURE_NO_NEED_ID, true, STREAM_OUT_2)){
            //     LOG_MSGID_I(VPC, "[VPC]set un-mute fail\n",0);
            // }
            #if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
                hal_audio_mute_stream_out(true, HAL_AUDIO_STREAM_OUT2);
            #else
                hal_audio_mute_stream_out(true);
            #endif
        }
    }
#endif
    return ret;
}

#ifdef AIR_STREAM_MANAGER_ENABLE
bool prompt_control_stop_codec(void);
audio_stream_manager_callback_t* prompt_control_codec_callback(audio_stream_manager_callback_event_t event_id, void *user_data)
{
    switch (event_id) {
        case AUDIO_CODEC_CB_EVENT_BITSTEAM_END:
            // prompt_control_stop_codec();
            prompt_control_stop_tone();
            break;
        case AUDIO_CODEC_CB_EVENT_START:
            if (g_app_callback) {
                g_app_callback(PROMPT_CONTROL_MEDIA_PLAY);
            }
            break;
        default:
            break;
    }
    return 0;
}
bool prompt_control_play_codec(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, uint32_t sync_time, prompt_control_callback_t callback)
{
    (void) callback;
    prompt_control_mutex_lock();
    if (g_vp_codec_id != -1) {
        LOG_MSGID_I(VPC, "[VPC] vp is already exit, now stop current vp\n",0);
        prompt_control_mutex_unlock();
        prompt_control_stop_codec();
        prompt_control_mutex_lock();
    }
    audio_stream_manager_config_t user_config;
    memset(&user_config, 0, sizeof(audio_stream_manager_config_t));
    user_config.input_buffer = tone_buf;
    user_config.input_buffer_size = tone_size;
    user_config.stream_type = AUDIO_STREAM_MANAGER_TYPE_VP;
    switch (tone_type) {
        case VPC_WAV:
            #ifdef MTK_WAV_DECODER_ENABLE
                user_config.codec_type = CODEC_TYPE_WAV_DECODE;
            #endif
            break;
        case VPC_MP3:
            #ifdef AIR_MP3_DECODER_ENABLE
                user_config.codec_type = CODEC_TYPE_MP3_DECODE;
            #endif
            break;
        case VPC_OPUS:
            #ifdef AIR_OPUS_DECODER_ENABLE
                user_config.codec_type = CODEC_TYPE_OPUS_DECODE;
            #endif
            break;
        default:
            LOG_MSGID_I(VPC, "[VPC] codec type is not support\n",0);
            prompt_control_mutex_unlock();
            return false;
    }
    if (user_config.codec_type == 0) {
        LOG_MSGID_I(VPC, "[VPC] codec type is not found, %d", 1, tone_type);
        configASSERT(0);
    }
#ifdef AIR_PROMPT_SOUND_LINERBUFFER_ENABLE
    user_config.buffer_type = AUDIO_STREAM_MANAGER_LINEAR_BUFFER;
#else
    user_config.buffer_type = AUDIO_STREAM_MANAGER_RING_BUFFER;
#endif
    user_config.user_data = NULL;
    if (audio_stream_manager_open((audio_stream_manager_id_t *)&g_vp_codec_id, &user_config, (audio_stream_manager_callback_t *)prompt_control_codec_callback) !=
        AUDIO_STREAM_MANAGER_STATUS_OK) {
        g_vp_codec_id = -1;
        prompt_control_mutex_unlock();
        LOG_MSGID_I(VPC, "[VPC] open codec manager fail, stop vp", 0);
        return false;
    }
#ifdef MTK_AVM_DIRECT
    prompt_control_set_volume();
#endif
    // sync time detection
    if(sync_time != 0) {
        // critical region
        int32_t delta_cnt = 0;
        uint32_t curr_cnt = 0;
        uint32_t savedmask = 0;
        hal_nvic_save_and_set_interrupt_mask(&savedmask);
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &curr_cnt);
        delta_cnt = sync_time - curr_cnt;
        hal_nvic_restore_interrupt_mask(savedmask);
        LOG_MSGID_I(VPC, "[VPC][SYNC] current gpt cnt = %u, target cnt = %u, delta_cnt = %d", 3, curr_cnt, sync_time, delta_cnt);
        if (delta_cnt < 0) {
            LOG_MSGID_I(VPC, "[VPC][SYNC] Warning: Sync gpt count is invalid\n", 0);
            audio_stream_manager_set_aws(g_vp_codec_id, false, sync_time); // trigger VP immediately
        } else if (delta_cnt <= 80000) {
            LOG_MSGID_I(VPC, "[VPC][SYNC] Warning: delta_cnt is less than 80ms\n", 0);
            audio_stream_manager_set_aws(g_vp_codec_id, true, sync_time);
        } else {
            audio_stream_manager_set_aws(g_vp_codec_id, true, sync_time);
        }
        // LOG_MSGID_I(VPC, "[VPC][SYNC]mix play internal type=%d, buf=%x, size=%d\n", 3, tone_type, tone_buf, tone_size);
    } else {
        audio_stream_manager_set_aws(g_vp_codec_id, false, sync_time);
    }
    audio_stream_manager_start(g_vp_codec_id);
    prompt_control_mutex_unlock();
    return true;
}

bool prompt_control_stop_codec(void)
{
    prompt_control_mutex_lock();
    if (g_vp_codec_id == -1) {
        LOG_MSGID_I(VPC, "[VPC] vp is not exit", 0);
        prompt_control_mutex_unlock();
        return false;
    }
    audio_stream_manager_stop(g_vp_codec_id);
    if (g_app_callback) {
        g_app_callback(PROMPT_CONTROL_MEDIA_END);
    }
    audio_stream_manager_close(g_vp_codec_id);
    g_vp_codec_id = -1;
    prompt_control_mutex_unlock();
    return true;
}
#endif
bool prompt_control_play_sync_tone(prompt_control_tone_type_t tone_type, uint8_t *tone_buf, uint32_t tone_size, uint32_t sync_time, prompt_control_callback_t callback)
{
    bool ret = false;
#if defined(MTK_AVM_DIRECT)
    audio_codec_mutex_lock();
#endif
    //ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, true);
#if defined(__HAL_AUDIO_AWS_SUPPORT__) && !defined(MTK_AVM_DIRECT)
    audio_service_aws_skip_clock_skew(1);
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */
    am_prompt_control_play_sync_tone(tone_type, tone_buf, tone_size, sync_time, callback);
#if defined(MTK_AVM_DIRECT)
    audio_codec_mutex_unlock();
#endif
    return ret;
}
#endif

void prompt_control_stop_tone_internal(void)
{
    if(!g_prompt_mute_lock){
        // if(AUD_EXECUTION_FAIL == bt_sink_srv_ami_audio_set_mute(FEATURE_NO_NEED_ID, true, STREAM_OUT_2)){
        //     LOG_MSGID_I(VPC, "[VPC]set mute fail\n",0);
        // }
        #if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
            hal_audio_mute_stream_out(true, HAL_AUDIO_STREAM_OUT2);
        #else
            hal_audio_mute_stream_out(true);
        #endif
    }
#ifdef AIR_STREAM_MANAGER_ENABLE
    if (prompt_control_stop_codec() == false) {
        return;
    }
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
    hal_dvfs_lock_control(VP_DVFS_DEFAULT_SPEED, HAL_DVFS_UNLOCK);
#endif
    ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, false);
    ami_set_audio_mask(AM_TASK_MASK_VP_HAPPENING, false);
}

void prompt_control_stop_tone(void)
{
    LOG_MSGID_I(VPC, "[VPC]stop codec internal done", 0);
    audio_codec_mutex_lock();
#if defined(__HAL_AUDIO_AWS_SUPPORT__) && !defined(MTK_AVM_DIRECT)
    audio_service_aws_skip_clock_skew(0);
#endif  /* __HAL_AUDIO_AWS_SUPPORT__ */
    am_prompt_control_stop_tone();
    audio_codec_mutex_unlock();
}

#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
void prompt_control_dummy_source_set_volume(uint8_t vol_lev)
{
    g_VP_dummy_source_gain_dB = vol_lev;

    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
    bt_sink_srv_am_type_t current_audio_type;
    uint32_t digital = 0;   //default: 0dB;
    uint32_t analog = 400;  //Align DL1 analog gain.
    if(vol_lev >> 7){
        digital   = (0xFFFFFF00 | vol_lev) * 100;
    } else {
        digital   = 0;  //MAX 0dB;
    }

    current_audio_type = bt_sink_srv_ami_get_current_scenario();
    if(current_audio_type != NONE) {
        analog = 0x00007FFF; //don't care
    }

    hal_audio_set_stream_out_dl3_volume(digital, analog);

    LOG_MSGID_I(VPC, "[VP/DmySrc]Set Prompt Volume Scenario:%d Level[%d] D_Gain:0x%x A_Gain:0x%x\n",4,
        current_audio_type, vol_lev, digital, analog);
    #endif
}

uint8_t prompt_control_dummy_source_get_volume_level(void)
{
    LOG_MSGID_I(VPC, "[VP/DmySrc]get level:%d\n",1, g_VP_dummy_source_gain_dB);
    return g_VP_dummy_source_gain_dB;
}

bool prompt_control_dummy_source_query_state(void)
{
    if(g_VP_dummy_source_handle.state == PROMPT_CONTROL_DUMMY_SOURCE_PLAY_STATE){
        return true;
    } else {
        return false;
    }
}

prompt_control_status_t prompt_control_dummy_source_change_feature(prompt_control_dummy_source_param_t param)
{
    prompt_control_status_t ret = PROMPT_CONTROL_RETURN_ERROR;

#if defined(MTK_AVM_DIRECT)
    prompt_control_mutex_lock();
#endif

    LOG_MSGID_I(VPC, "[VP/DmySrc]Change feature index=%d, mode=%d, vol_lev=%d\n", 3, param.index, param.mode, param.vol_lev);

    if(!prompt_control_dummy_source_query_state()){
        LOG_MSGID_I(VPC, "[VP/DmySrc]Change feature fail. VP_DmySrc not exist.\n", 0);
        prompt_control_mutex_unlock();
        return ret;
    } else {
        uint32_t dummy_source_param;
        dummy_source_param = (param.index << 16) | (param.mode & 0xFFFF);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_CHANGE_FEATURE, 0, (uint32_t)dummy_source_param, false);
        ret = PROMPT_CONTROL_RETURN_OK;
    }

#if defined(MTK_AVM_DIRECT)
    prompt_control_mutex_unlock();
#endif
    return ret;
}

prompt_control_status_t prompt_control_dummy_source_start_internal(prompt_control_dummy_source_param_t param)
{
    prompt_control_status_t ret = PROMPT_CONTROL_RETURN_OK;
    prompt_control_dummy_source_set_volume(param.vol_lev);

    /*Open, Start.*/
    dsp2mcu_audio_msg_t open_msg = MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_OPEN;
    dsp2mcu_audio_msg_t start_msg = MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_START;
    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PROMPT_DUMMY_SOURCE;

    void *p_param_share;
    // Collect parameters
    mcu2dsp_open_param_t open_param;
    open_param.param.stream_in = STREAM_IN_VP_DUMMY_SOURCE;
    open_param.param.stream_out = STREAM_OUT_AFE;

    open_param.stream_in_param.playback.bit_type = HAL_AUDIO_BITS_PER_SAMPLING_24; /*DSP Source 32 resolution.*/
    open_param.stream_in_param.playback.sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
    open_param.stream_in_param.playback.channel_number = HAL_AUDIO_MONO;
    open_param.stream_in_param.playback.codec_type = 0;  //not use
    open_param.stream_in_param.playback.dsp_local_data_index  = param.index;
    open_param.stream_in_param.playback.dsp_local_stream_mode = param.mode;
#if 0
    open_param.stream_out_param.afe.audio_device    = HAL_MT2811_AUDIO_DEVICE_DAC_DUAL;
    open_param.stream_out_param.afe.stream_channel  = HAL_MT2811_AUDIO_DIRECT;
    open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
#else
    hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
    if(open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param.stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
    } else {
        open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
    }
#endif
    open_param.stream_out_param.afe.memory    = HAL_AUDIO_MEM3;
    open_param.stream_out_param.afe.format    = AFE_PCM_FORMAT_S32_LE;    /*DSP Sink 32bit resolution.*/
    if(open_param.stream_out_param.afe.format == AFE_PCM_FORMAT_S32_LE){
        open_param.stream_out_param.afe.irq_period    = 5;
        open_param.stream_out_param.afe.frame_size    = 256;
        open_param.stream_out_param.afe.frame_number  = 4;
    }else{
        open_param.stream_out_param.afe.irq_period    = 10;
        open_param.stream_out_param.afe.frame_size    = 512;
        open_param.stream_out_param.afe.frame_number  = 4;
        //512(frame size) / 48k(Sampling rate) = 10(irq_period)
    }
    #ifdef AIR_FIXED_SUB_DL_HIGH_RES_ENABLE
        hal_audio_dsp_dl_clkmux_control(msg_type, open_param.stream_out_param.afe.audio_device, 96000, true);
    #else
        hal_audio_dsp_dl_clkmux_control(msg_type, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, true);
    #endif
    open_param.stream_out_param.afe.stream_out_sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;  //asrc in
    open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;             //asrc out
    open_param.stream_out_param.afe.hw_gain       = true;
    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), msg_type);
    // Notify to do dynamic download. Use async wait.
    hal_audio_dsp_controller_send_message(open_msg, AUDIO_DSP_CODEC_TYPE_PCM, (uint32_t)p_param_share, true);


    // Register callback
    //hal_audio_service_hook_callback(msg_type, mp3_codec_pcm_out_isr_callback, handle);
    // Start playback
    mcu2dsp_start_param_t start_param;
    // Collect parameters
    start_param.param.stream_in  = STREAM_IN_VP_DUMMY_SOURCE;
    start_param.param.stream_out = STREAM_OUT_AFE;
    start_param.stream_out_param.afe.aws_flag  = false;
    start_param.stream_out_param.afe.aws_sync_request = false;
    start_param.stream_out_param.afe.aws_sync_time	= 0;
    p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), msg_type);
    hal_audio_dsp_controller_send_message(start_msg, 0, (uint32_t)p_param_share, true);
    g_VP_dummy_source_handle.state = PROMPT_CONTROL_DUMMY_SOURCE_PLAY_STATE;

    return ret;
}

prompt_control_status_t prompt_control_dummy_source_start(prompt_control_dummy_source_param_t param)
{
    prompt_control_status_t ret = PROMPT_CONTROL_RETURN_ERROR;

#if defined(MTK_AVM_DIRECT)
    prompt_control_mutex_lock();
#endif

    LOG_MSGID_I(VPC, "[VP/DmySrc]Start play index=%d, mode=%d, vol_lev=%d\n", 3, param.index, param.mode, param.vol_lev);

    if(prompt_control_dummy_source_query_state()){
        LOG_MSGID_I(VPC, "[VP/DmySrc]Start play drop. VP_DmySrc have exist.\n", 0);
        prompt_control_mutex_unlock();
        return ret;
    }
    ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT_DUMMY_SOURCE, true);

    if (AUD_EXECUTION_FAIL == bt_sink_srv_ami_audio_set_mute(FEATURE_NO_NEED_ID, false, STREAM_OUT_3)) {
        LOG_MSGID_I(VPC, "[VP/DmySrc]set un-mute fail\n",0);
    }
    if (HFP == bt_sink_srv_ami_get_current_scenario()) { // dj couldn't play with esco
        LOG_MSGID_I(VPC, "[VP/DmySrc]Start play fail. HFP Scenario exist.\n", 0);
        prompt_control_mutex_unlock();
        return ret;
    }
    // } else if(prompt_control_query_state()){ //TODO
    //     LOG_MSGID_I(VPC, "[VP/DmySrc]Start play fail. Prompt Sound exist.\n", 0);
    //     prompt_control_mutex_unlock();
    //     return ret;
    // }
#if 1
    bt_sink_srv_am_feature_t am_feature;
    memset(&am_feature, 0, sizeof(bt_sink_srv_am_feature_t));
    am_feature.type_mask                    = AM_VP;
    am_feature.feature_param.vp_param.event = PROMPT_CONTROL_DUMMY_SOURCE_START;
    am_feature.feature_param.vp_param.dummy_source_param.index = param.index;
    am_feature.feature_param.vp_param.dummy_source_param.mode  = param.mode;
    am_feature.feature_param.vp_param.dummy_source_param.vol_lev  = param.vol_lev;
    ret = am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
#else
    ret = prompt_control_dummy_source_start_internal(param);
#endif

#if defined(MTK_AVM_DIRECT)
    prompt_control_mutex_unlock();
#endif
    return ret;
}

void prompt_control_dummy_source_stop_internal(void)
{
    dsp2mcu_audio_msg_t stop_msg = MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_STOP;
    dsp2mcu_audio_msg_t close_msg = MSG_MCU2DSP_PROMPT_DUMMY_SOURCE_CLOSE;
    //audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PROMPT_DUMMY_SOURCE;
    // Notify to stop
    hal_audio_dsp_controller_send_message(stop_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);

    // Unregister callback
    //hal_audio_service_unhook_callback(msg_type);

    // Notify to release dynamic download
    hal_audio_dsp_controller_send_message(close_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PROMPT_DUMMY_SOURCE, 0, 0, false);
    g_VP_dummy_source_handle.state = PROMPT_CONTROL_DUMMY_SOURCE_CLOSE_STATE;

    ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT_DUMMY_SOURCE, false);
}

void prompt_control_dummy_source_stop(void)
{
    prompt_control_mutex_lock();
    LOG_MSGID_I(VPC, "[VP/DmySrc]stop VP dummy source.\n",0);
    if(!prompt_control_dummy_source_query_state()){
        LOG_MSGID_I(VPC, "[VP/DmySrc]Stop VP dummy source drop. Not exist.\n", 0);
        prompt_control_mutex_unlock();
        return;
    }
    if (AUD_EXECUTION_FAIL == bt_sink_srv_ami_audio_set_mute(FEATURE_NO_NEED_ID, true, STREAM_OUT_3)) {
        LOG_MSGID_I(VPC, "[VP/DmySrc]set mute fail\n",0);
    }
#if 1
    bt_sink_srv_am_feature_t am_feature;
    memset(&am_feature, 0, sizeof(bt_sink_srv_am_feature_t));
    am_feature.type_mask                    = AM_VP;
    am_feature.feature_param.vp_param.event = PROMPT_CONTROL_DUMMY_SOURCE_STOP;
    am_audio_set_feature(FEATURE_NO_NEED_ID, &am_feature);
#else
    prompt_control_dummy_source_stop_internal();
#endif
    prompt_control_mutex_unlock();
}

#endif /*AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE*/

#endif

