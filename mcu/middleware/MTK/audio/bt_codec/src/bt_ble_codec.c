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

#ifdef AIR_BT_CODEC_BLE_ENABLED

#include "bt_ble_codec_internal.h"
#include "bt_le_audio_def.h"

#include "hal_gpt.h"
#include "hal_audio.h"
#include "hal_dvfs.h"
#include "task_def.h"

#include "bt_ble_codec_internal.h"
#include "bt_sink_srv_ami.h"
#include "audio_nvdm_common.h"
#include "audio_nvdm_coef.h"

#include "bt_sink_srv_le_cap_audio_manager.h"

#define CODEC_AUDIO_MODE 0x0
#define CODEC_VOICE_MODE 0x1

#if defined(HAL_DVFS_MODULE_ENABLED)
#define BLE_MUSIC_DVFS_SPEED HAL_DVFS_HALF_SPEED_52M
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR) || defined(MTK_3RD_PARTY_NR)
#define BLE_CALL_DVFS_SPEED HAL_DVFS_HIGH_SPEED_208M
#else
#define BLE_CALL_DVFS_SPEED HAL_DVFS_HALF_SPEED_52M
#endif
#endif

log_create_module(BT_BLE_CODEC, PRINT_LEVEL_INFO);

#define BLE_LOG_MSGID_E(fmt, arg...) LOG_MSGID_E(BT_BLE_CODEC, "[BLE] "fmt,##arg)
#define BLE_LOG_MSGID_W(fmt, arg...) LOG_MSGID_W(BT_BLE_CODEC, "[BLE] "fmt,##arg)
#define BLE_LOG_MSGID_I(fmt, arg...) LOG_MSGID_I(BT_BLE_CODEC, "[BLE] "fmt,##arg)
#define BLE_LOG_MSGID_D(fmt, arg...) LOG_MSGID_D(BT_BLE_CODEC, "[BLE] "fmt,##arg)

#define UNUSED(x)  ((void)(x))
#define BLE_CALL_APPLY_EC_NR_EQ

static bool bt_ble_codec_is_running = false;
static bt_ble_codec_internal_handle_t *bt_ble_codec_internal_handle = NULL;


/*********************************************
 *********For BLE sink side code flow**************
 *********************************************/

static bt_codec_media_status_t bt_ble_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    BLE_LOG_MSGID_E("process\r\n", 0);
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

static void bt_set_aws_flag(bt_media_handle_t *handle, bool is_aws)
{
#if defined(MTK_AVM_DIRECT)
bt_ble_codec_internal_handle_t *internal_handle = (bt_ble_codec_internal_handle_t *)handle;

    BLE_LOG_MSGID_I("BLE codec set aws flag:%d\r\n", 1, is_aws);
    internal_handle->aws_flag = is_aws;
#endif
}

static void bt_trigger_mic(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    if(handle) {
        BLE_LOG_MSGID_I("BLE codec Trigger Mic On\r\n", 0);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_PLAY_EN, 0, 0, 0);
    }
#endif
}

static void bt_side_tone_enable(bt_media_handle_t *handle)
{
#if 0
    mcu2dsp_sidetone_param_t sidetone;
    void *p_param_share;

    if(!handle){
        BLE_LOG_MSGID_I("[HFP Codec]SideTone Enable No Handle\n", 0);
        return;
    }

    BLE_LOG_MSGID_I("[HFP Codec]SideTone Enable\n", 0);

    sidetone.in_device                       = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
    sidetone.in_interface                    = HAL_AUDIO_INTERFACE_1;
    sidetone.in_misc_parms                   = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
    sidetone.out_device                      = HAL_AUDIO_DEVICE_DAC_DUAL;
    sidetone.out_interface                   = HAL_AUDIO_INTERFACE_NONE;
    sidetone.out_misc_parms                  = DOWNLINK_PERFORMANCE_NORMAL;
    sidetone.in_channel                      = HAL_AUDIO_DIRECT;
    sidetone.gain                            = 600;
    sidetone.sample_rate                     = 16000;

    p_param_share = hal_audio_dsp_controller_put_paramter( &sidetone, sizeof(mcu2dsp_sidetone_param_t), AUDIO_MESSAGE_TYPE_SIDETONE);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_START, 0, (uint32_t)p_param_share, true);
#else
/****/
#endif
}

static void bt_side_tone_disable(bt_media_handle_t *handle)
{
#if 0
    mcu2dsp_sidetone_param_t sidetone;
    void *p_param_share;

    if(!handle){
        BLE_LOG_MSGID_I("[HFP Codec]SideTone Disable No Handle\n", 0);
        return;
    }

    BLE_LOG_MSGID_I("[HFP Codec]SideTone Disable\n", 0);

    sidetone.in_device                       = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
    sidetone.in_interface                    = HAL_AUDIO_INTERFACE_1;
    sidetone.in_misc_parms                   = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
    sidetone.out_device                      = HAL_AUDIO_DEVICE_DAC_DUAL;
    sidetone.out_interface                   = HAL_AUDIO_INTERFACE_NONE;
    sidetone.out_misc_parms                  = DOWNLINK_PERFORMANCE_NORMAL;
    sidetone.in_channel                      = HAL_AUDIO_DIRECT;
    sidetone.gain           = 0;
    sidetone.sample_rate    = 16000;

    p_param_share = hal_audio_dsp_controller_put_paramter( &sidetone, sizeof(mcu2dsp_sidetone_param_t), AUDIO_MESSAGE_TYPE_SIDETONE);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_STOP, 0, (uint32_t)p_param_share, true);
#else
/****/
#endif
}

static void bt_ble_isr_handler(hal_audio_event_t event, void *data)
{
    bt_ble_codec_internal_handle_t *internal_handle = (bt_ble_codec_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;

#if 0
    if (event == HAL_AUDIO_EVENT_HFP_PARA_SEND) {
        handle->handler(handle, BT_CODEC_MEDIA_HFP_AVC_PARA_SEND);
    }
#endif

    UNUSED(handle);
}

static bt_codec_media_status_t bt_ble_play(bt_media_handle_t *handle)
{
    bt_ble_codec_internal_handle_t *internal_handle = (bt_ble_codec_internal_handle_t *)handle;
    bool aws_flag = internal_handle->aws_flag;
    mcu2dsp_start_param_t start_param;
    void *p_param_share;

    BLE_LOG_MSGID_I("start DL\r\n", 0);

    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL, bt_ble_isr_handler, handle);  //Rdebug Need message
    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL, bt_ble_isr_handler, handle);  //Rdebug Need message

    start_param.param.stream_in = STREAM_IN_BLE;  //Rdebug Need DSP in
    start_param.param.stream_out = STREAM_OUT_AFE;
    start_param.stream_out_param.afe.aws_flag = aws_flag;
    p_param_share = hal_audio_dsp_controller_put_paramter(&start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL);
    if (internal_handle->codec_info.channel_mode != CHANNEL_MODE_UL_ONLY) {
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_DL_START, 0, (uint32_t)p_param_share, true);
    }
    if (internal_handle->codec_info.channel_mode == CHANNEL_MODE_DL_UL_BOTH) {
        BLE_LOG_MSGID_I("start UL\r\n", 0);
        start_param.param.stream_in = STREAM_IN_AFE;
        start_param.param.stream_out = STREAM_OUT_BLE;
        start_param.stream_in_param.afe.aws_flag = aws_flag;
        p_param_share = hal_audio_dsp_controller_put_paramter(&start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_START, 0, (uint32_t)p_param_share, true);
    }

    handle->state = BT_CODEC_STATE_PLAY;

    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_ble_stop(bt_media_handle_t *handle)
{
    bt_ble_codec_internal_handle_t *internal_handle = (bt_ble_codec_internal_handle_t *)handle;
    BLE_LOG_MSGID_I("stop DL\r\n", 0);

    hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL);
    hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL);

    if (internal_handle->codec_info.channel_mode != CHANNEL_MODE_UL_ONLY) {
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_DL_STOP, 0, 0, true);
    }
    if (internal_handle->codec_info.channel_mode != CHANNEL_MODE_DL_ONLY) {
        BLE_LOG_MSGID_I("stop UL\r\n", 0);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_STOP, 0, 0, true);

    }

    handle->state = BT_CODEC_STATE_STOP;

#if defined(HAL_DVFS_MODULE_ENABLED)
    if (internal_handle->codec_info.channel_mode == CHANNEL_MODE_DL_ONLY) {
        if (internal_handle->codec_info.context_type != AUDIO_CONTENT_TYPE_GAME)
        {
            hal_dvfs_lock_control(BLE_MUSIC_DVFS_SPEED, HAL_DVFS_UNLOCK);
        }
        else
        {
            hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_UNLOCK);
        }
    } else {
        hal_dvfs_lock_control(BLE_CALL_DVFS_SPEED, HAL_DVFS_UNLOCK);
    }
    BLE_LOG_MSGID_I("dvfs unlock\r\n", 0);
#endif

    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_ble_internal_open(const bt_codec_le_audio_t *param)
{
    mcu2dsp_open_param_t open_param;
    void *p_param_share;
//    sysram_status_t status;
    bt_codec_type_le_audio_t codec_type = (bt_codec_type_le_audio_t)(param->codec);

#if defined(HAL_DVFS_MODULE_ENABLED)
    if (param->channel_mode == CHANNEL_MODE_DL_ONLY) {
        if (param->context_type != AUDIO_CONTENT_TYPE_GAME)
        {
            hal_dvfs_lock_control(BLE_MUSIC_DVFS_SPEED, HAL_DVFS_LOCK);
        }
        else
        {
            hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_LOCK);
        }
    } else {
        hal_dvfs_lock_control(BLE_CALL_DVFS_SPEED, HAL_DVFS_LOCK);
    }
    BLE_LOG_MSGID_I("dvfs lock\r\n", 0);
#endif
    if (param->channel_mode != CHANNEL_MODE_UL_ONLY){
        memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));

        open_param.param.stream_in  = STREAM_IN_BLE;
        open_param.param.stream_out = STREAM_OUT_AFE;

        open_param.stream_in_param.ble.codec_type     = codec_type;
        open_param.stream_in_param.ble.context_type   = param->context_type;
        open_param.stream_in_param.ble.sampling_frequency = hal_audio_sampling_rate_enum_to_value(param->sample_rate);
        open_param.stream_in_param.ble.channel_num    = param->channel_num; /* channel number */
        if (param->channel_num == CHANNEL_MONO){
            open_param.stream_in_param.ble.channel_num = 1;
        }else{
            open_param.stream_in_param.ble.channel_num = 2;
            if (param->channel_num == CHANNEL_DUAL_MONO)
            {
                open_param.stream_in_param.ble.dual_cis_mode = TRUE;
                open_param.stream_in_param.ble.p_sub_share_info = hal_audio_query_ble_audio_sub_dl_share_info();
            }
        }
        open_param.stream_in_param.ble.channel_mode   = param->channel_mode; /* decide call or music */
        if (param->frame_duration == FRAME_MS_10) {
            open_param.stream_in_param.ble.frame_ms   = 10 * 1000;
        } else if (param->frame_duration == FRAME_MS_7P5) {
            open_param.stream_in_param.ble.frame_ms   = 7500;
        }
        open_param.stream_in_param.ble.frame_payload_length = param->frame_payload_length;
        //open_param.stream_in_param.ble.bit_depth      = HAL_AUDIO_BITS_PER_SAMPLING_16; //:TODO: decided by BT
        //open_param.stream_in_param.ble.bt_inf_address = bt_sink_srv_ami_get_bt_inf_address();
        open_param.stream_in_param.ble.p_share_info     = hal_audio_query_ble_audio_dl_share_info();
        //open_param.stream_in_param.ble.clk_info_address = hal_audio_query_rcdc_share_info();
        //open_param.stream_in_param.ble.p_air_dump_buf =

        hal_audio_reset_share_info(open_param.stream_in_param.ble.p_share_info);
        BLE_LOG_MSGID_I("DL codec_type:%d, context_type:0x%x, sampling_rate:%d, channel_num:%d, channel_mode:%d, frame_duration:%d, frame_payload_length:%d, start_addr:0x%x\r\n", 8,
                         open_param.stream_in_param.ble.codec_type,
                         open_param.stream_in_param.ble.context_type,
                         open_param.stream_in_param.ble.sampling_frequency,
                         open_param.stream_in_param.ble.channel_num,
                         open_param.stream_in_param.ble.channel_mode,
                         open_param.stream_in_param.ble.frame_ms,
                         open_param.stream_in_param.ble.frame_payload_length,
                         open_param.stream_in_param.ble.p_share_info->start_addr);

        if (param->channel_mode == CHANNEL_MODE_DL_UL_BOTH) {
            hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &open_param.stream_out_param);
        } else {
            hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
        }
        BLE_LOG_MSGID_I("out_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.stream_channel, open_param.stream_out_param.afe.audio_interface);

        open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM1;
        if (param->channel_mode == CHANNEL_MODE_DL_UL_BOTH) {
            open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
        } else {
            open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
        }
        open_param.stream_out_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(param->sample_rate);
        open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(param->sample_rate);
        open_param.stream_out_param.afe.irq_period      = 0;  /* do not set irq_period, let audio framework use sample count to decide it*/
        open_param.stream_in_param.ble.decode_mode  = CODEC_AUDIO_MODE;
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
        open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
        open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#endif
        open_param.stream_out_param.afe.frame_size  = (open_param.stream_out_param.afe.sampling_rate/1000) * open_param.stream_in_param.ble.frame_ms/1000;

        /*    Voice DL Part       */
        if (param->channel_mode == CHANNEL_MODE_DL_UL_BOTH) {
            if(open_param.stream_in_param.ble.context_type == AUDIO_CONTENT_TYPE_CONVERSATIONAL){
                open_param.stream_in_param.ble.decode_mode  = CODEC_VOICE_MODE;

#ifdef BLE_CALL_APPLY_EC_NR_EQ
            /*
             * if EC NR feature is enabled in call scenario
             * sample rate and must be 16KHz due to algorithm limitation
             */
                open_param.stream_out_param.afe.stream_out_sampling_rate= hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_16KHZ);
                open_param.stream_out_param.afe.sampling_rate           = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_16KHZ);
#else
            /* no need to adjust afe sampling rate */
#endif
#if defined (AIR_BLE_SWB_ENABLE)
                open_param.stream_out_param.afe.stream_out_sampling_rate= hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_32KHZ);
                open_param.stream_out_param.afe.sampling_rate = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_32KHZ);
#endif
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
                open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
                open_param.stream_out_param.afe.sampling_rate = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#endif

            open_param.stream_out_param.afe.frame_size  = (open_param.stream_out_param.afe.sampling_rate/1000) * open_param.stream_in_param.ble.frame_ms/1000;
        } //else {
            //open_param.stream_in_param.ble.decode_mode  = CODEC_AUDIO_MODE;
            //open_param.stream_out_param.afe.frame_size  = (open_param.stream_out_param.afe.sampling_rate/1000) * open_param.stream_in_param.ble.frame_ms/1000;
        }

        open_param.stream_out_param.afe.frame_number    = 6; /* AFE buffer can cache 6 frame */
        open_param.stream_out_param.afe.hw_gain         = true;
        open_param.stream_out_param.afe.adc_mode        = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
#ifdef ENABLE_HWSRC_CLKSKEW
        if ((param->channel_mode == CHANNEL_MODE_DL_ONLY)&&(param->context_type != AUDIO_CONTENT_TYPE_GAME)) {
            open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V2;
        } else {
            open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
        }
        BLE_LOG_MSGID_I("open_param.stream_out_param.afe.clkskew_mode:%d", 1, open_param.stream_out_param.afe.clkskew_mode);
#endif

        BLE_LOG_MSGID_I("open DL\r\n", 0);
        p_param_share = hal_audio_dsp_controller_put_paramter(&open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL);
        hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, TRUE);
        if (codec_type == BT_CODEC_TYPE_LE_AUDIO_LC3) {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_DL_OPEN, AUDIO_DSP_CODEC_TYPE_LC3, (uint32_t)p_param_share, true);
        }
    }
    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
    /*    Voice UL Part       */
    if (param->channel_mode != CHANNEL_MODE_DL_ONLY) {
        open_param.param.stream_in  = STREAM_IN_AFE;
        open_param.param.stream_out = STREAM_OUT_BLE;

        open_param.stream_out_param.ble.codec_type     = codec_type;
        open_param.stream_out_param.ble.context_type   = param->ul_context_type;
        open_param.stream_out_param.ble.sampling_frequency = hal_audio_sampling_rate_enum_to_value(param->ul_sample_rate);
        open_param.stream_out_param.ble.channel_mode   = param->channel_mode;
        open_param.stream_out_param.ble.frame_ms       = (param->frame_duration == FRAME_MS_10) ? 10000 : 7500;
        open_param.stream_out_param.ble.frame_payload_length = param->ul_frame_payload_length;
        //open_param.stream_out_param.ble.bit_depth      = HAL_AUDIO_BITS_PER_SAMPLING_16; //:TODO: decided by BT
        open_param.stream_out_param.ble.decode_mode    = CODEC_VOICE_MODE; //:TODO: should always keep voice mode??
        open_param.stream_out_param.ble.p_share_info = hal_audio_query_ble_audio_ul_share_info();

        hal_audio_reset_share_info(open_param.stream_out_param.ble.p_share_info);
        BLE_LOG_MSGID_I("UL codec_type:%d, context_type:0x%x, sampling_rate:%d, channel_num:%d, channel_mode:%d, frame_duration:%d, frame_payload_length:%d, start_addr:0x%x\r\n", 8,
                         open_param.stream_out_param.ble.codec_type,
                         open_param.stream_out_param.ble.context_type,
                         open_param.stream_out_param.ble.sampling_frequency,
                         open_param.stream_out_param.ble.channel_num,
                         open_param.stream_out_param.ble.channel_mode,
                         open_param.stream_out_param.ble.frame_ms,
                         open_param.stream_out_param.ble.frame_payload_length,
                         open_param.stream_out_param.ble.p_share_info->start_addr);

        hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param.stream_in_param); //:TODO: should add new setting for BLE audio
        BLE_LOG_MSGID_I("in_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_in_param.afe.audio_device, open_param.stream_in_param.afe.stream_channel, open_param.stream_in_param.afe.audio_interface);

        open_param.stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE; //:TODO: always keep 16bit for audio??
        open_param.stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3;/* HAL_AUDIO_MEM3 to enable echo reference */
        open_param.stream_in_param.afe.sw_channels     = param->ul_channel_num;

#ifdef BLE_CALL_APPLY_EC_NR_EQ
        /*
         * if EC NR feature is enabled in call scenario
         * sample rate and frame interval must be 16KHz/ 15ms due to algorithm limitation
         * */
        open_param.stream_in_param.afe.irq_period      = 15;
        open_param.stream_in_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_16KHZ);
        open_param.stream_in_param.afe.frame_size      = (open_param.stream_in_param.afe.irq_period*open_param.stream_in_param.afe.sampling_rate)/1000;
#else
        open_param.stream_in_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(param->ul_sample_rate);
        if (codec_type == BT_CODEC_TYPE_LE_AUDIO_LC3) {
            if (param->sample_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
                open_param.stream_in_param.afe.irq_period     = 10;
                open_param.stream_in_param.afe.frame_size     = 160;
            } else if (param->sample_rate == HAL_AUDIO_SAMPLING_RATE_24KHZ) {
                open_param.stream_in_param.afe.irq_period     = 10;
                open_param.stream_in_param.afe.frame_size     = 240;
            } else {
                BLE_LOG_MSGID_I("LC3_open test, force 32k sampling rate\r\n", 0);
                open_param.stream_in_param.afe.irq_period     = 10;
                open_param.stream_in_param.afe.frame_size     = 320;
                /*
                 *  following parameter is for 15ms interval with 32K sampling rate.
                 *  up sampling is not needed but SW buffer mechanism should be applied.
                 */
                open_param.stream_in_param.afe.irq_period     = 15;
                open_param.stream_in_param.afe.frame_size     = 480;
            }
        }
#endif
#if defined (AIR_BLE_SWB_ENABLE)
        //BLE_LOG_MSGID_I("[TEST] AIR_BLE_SWB_ENABLE",0);
        open_param.stream_in_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_32KHZ);
        open_param.stream_in_param.afe.frame_size = (open_param.stream_in_param.afe.irq_period*open_param.stream_in_param.afe.sampling_rate)/1000;
#endif
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K)
        if (open_param.stream_in_param.afe.sampling_rate == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
            open_param.stream_in_param.afe.frame_size = 3 * open_param.stream_in_param.afe.frame_size;
        } else if (open_param.stream_in_param.afe.sampling_rate == HAL_AUDIO_SAMPLING_RATE_24KHZ) {
            open_param.stream_in_param.afe.frame_size = 2 * open_param.stream_in_param.afe.frame_size;
        } else {
            open_param.stream_in_param.afe.frame_size = (3 * open_param.stream_in_param.afe.frame_size) / 2;
        }
        open_param.stream_in_param.afe.sampling_rate = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ);
#endif
        open_param.stream_in_param.afe.frame_number    = 4; /* AFE buffer can cache 6 frame */
        open_param.stream_in_param.afe.hw_gain         = false; /* uplink use SW gain only */

        BLE_LOG_MSGID_I("open UL\r\n", 0);
        p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL);
        if (codec_type == BT_CODEC_TYPE_LE_AUDIO_LC3) {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_OPEN, AUDIO_DSP_CODEC_TYPE_LC3, (uint32_t)p_param_share, true);
        }
    }

    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_media_handle_t *bt_codec_le_audio_open(bt_codec_le_audio_callback_t callback, const bt_codec_le_audio_t *param)
{
    bt_media_handle_t *handle;
    bt_ble_codec_internal_handle_t *internal_handle = NULL;

    if ((param == NULL) || (param->codec != BT_CODEC_TYPE_LE_AUDIO_LC3)) {
        return NULL;
    }

    BLE_LOG_MSGID_I("Open codec +++, codec type:%d\n", 1, param->codec);

    internal_handle = (bt_ble_codec_internal_handle_t *)pvPortMalloc(sizeof(bt_ble_codec_internal_handle_t));
    if (internal_handle == NULL) {
        return NULL;
    }

    memset(internal_handle, 0, sizeof(bt_ble_codec_internal_handle_t));
    handle = &internal_handle->handle;
    internal_handle->codec_info = *(bt_codec_le_audio_t *)param;
    handle->play    = bt_ble_play;
    handle->stop    = bt_ble_stop;
    handle->process = bt_ble_process;
    handle->set_aws_flag = bt_set_aws_flag;
    handle->trigger_mic = bt_trigger_mic;
    handle->side_tone_enable = bt_side_tone_enable;
    handle->side_tone_disable = bt_side_tone_disable;

    if (bt_ble_internal_open(param) == BT_CODEC_MEDIA_STATUS_ERROR) {
        BLE_LOG_MSGID_I("bt ble internal open fail\r\n", 0);
        vPortFree(internal_handle);
        return NULL;
    }

    handle->state = BT_CODEC_STATE_READY;
    handle->handler = callback;

    bt_ble_codec_internal_handle = internal_handle;
    bt_ble_codec_is_running = true;

    BLE_LOG_MSGID_I("Open codec ---\r\n", 0);

    return handle;
}

bt_codec_media_status_t bt_codec_le_audio_close (bt_media_handle_t *handle)
{
    bt_ble_codec_internal_handle_t *p_info = bt_ble_codec_internal_handle;

    BLE_LOG_MSGID_I("close codec +++\r\n", 0);

    if ((handle == NULL) || (handle->state != BT_CODEC_STATE_STOP)) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (p_info->codec_info.channel_mode != CHANNEL_MODE_DL_ONLY) {
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_CLOSE, 0, 0, true);
    }
    if (p_info->codec_info.channel_mode != CHANNEL_MODE_UL_ONLY) {
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_DL_CLOSE, 0, 0, true);
    }
    handle->state = BT_CODEC_STATE_IDLE;
    vPortFree(p_info);

    bt_ble_codec_internal_handle = NULL;
    bt_ble_codec_is_running = false;

    BLE_LOG_MSGID_I("close codec ---\r\n", 0);

    return BT_CODEC_MEDIA_STATUS_OK;
}

















/*********************************************
 *********For BLE source side code flow**************
 *********************************************/

#ifdef MTK_LINEIN_PLAYBACK_ENABLE
#include "linein_playback.h"
#endif

#define TASK_LOG_MSGID_CTRL(fmt, arg...)  LOG_MSGID_I(BT_BLE_CODEC, "[BLE CODEC]: "fmt,##arg)

typedef struct {
    uint8_t packet_length;
    uint8_t packet_number_per_interrupt;
    bool two_bytes_dummy_between_packet_flag;
} bt_ble_packet_info_t;

/*******************************************************************************\
| LPK_UL_TO_HP feature                                                                    |
\*******************************************************************************/
//#define LBK_UL_TO_HP_ENABLED
#if defined LBK_UL_TO_HP_ENABLED
uint16_t audio_pcm2way_hp_buffer[320] = {0};
#endif
/*******************************************************************************\
| ROM Tables                                                                    |
\*******************************************************************************/

static const uint32_t bt_ble_rx_packet_valid_mask_table[BT_BLE_PKT_TYPE_IDX_MAX][BT_BLE_HW_SRAM_RX_PKT_CNT] = {
    {0x1 << 0, 0x1 << 0, 0x1 << 1, 0x1 << 1, 0x1 <<  2, 0x1 <<  2}, /* 60 Bytes */
    {0x1 << 0, 0x1 << 1, 0x1 << 2, 0x1 << 3, 0x1 <<  4, 0x1 <<  5}, /* 30 Bytes */
    {0x3 << 0, 0x3 << 1, 0x3 << 3, 0x3 << 4, 0x3 <<  6, 0x3 <<  7}, /* 20 Bytes */
    {0x7 << 0, 0x7 << 3, 0x7 << 6, 0x7 << 9, 0x7 << 12, 0x7 << 15}  /* 10 Bytes */
};

static const uint8_t bt_ble_msbc_header[4] = {0x08, 0x38, 0xc8, 0xf8};  //Rdebug need change


void bt_ble_source_playback(uint16_t num_of_sink, uint16_t frame_length, uint8_t in_sample_rate, uint8_t out_bitrate, uint16_t frame_interval_us)
{
    //in_sample_rate    : KHz
    //out_bitrate       : Kbps
    //frame_interval_us : microseconds

#ifdef MTK_LINEIN_PLAYBACK_ENABLE
    if(num_of_sink == 0xFFFF) {
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_PLAYBACK_DATA_INFO, 0xFFFF, 0, false);
        TASK_LOG_MSGID_CTRL("[le audio MCU] toggle playback source between line_in/1K tone",0);
    } else if(num_of_sink == 0x9999) {
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_PLAYBACK_DATA_INFO, 0x9999, 0, false);
        TASK_LOG_MSGID_CTRL("[le audio MCU] trigger line-in irq start",0);
    } else if(num_of_sink) {


        n9_dsp_share_info_t *p_ul1_share = hal_audio_query_ble_audio_ul_share_info();
        n9_dsp_share_info_t *p_ul2_share = hal_audio_query_ble_audio_dl_share_info();    // use dl share buffer to handle ul data for second sink device
        uint32_t dsp_param = frame_length | (in_sample_rate<<16)| (out_bitrate<<24);

        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_PLAYBACK_DATA_INFO, num_of_sink, dsp_param, false);

        if(num_of_sink >= 1)
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_BUFFER_INFO, 1, (uint32_t)p_ul1_share, false);
        if(num_of_sink >= 2)
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_BUFFER_INFO, 2, (uint32_t)p_ul2_share, false);

        TASK_LOG_MSGID_CTRL("[le audio MCU] playback source ul1: %X ul2: %X",2,p_ul1_share,p_ul2_share);


        hal_audio_sampling_rate_t fs_enum;

        fs_enum = (in_sample_rate == 48) ? HAL_AUDIO_SAMPLING_RATE_48KHZ : \
                  (in_sample_rate == 32) ? HAL_AUDIO_SAMPLING_RATE_32KHZ : \
                  (in_sample_rate == 16) ? HAL_AUDIO_SAMPLING_RATE_16KHZ : -1;

        if(fs_enum != -1) {
            uint32_t frame_sample_count = (in_sample_rate*frame_interval_us)/1000;

            linein_playback_le_audio_open(fs_enum, fs_enum, frame_sample_count);
            linein_playback_start();
//            linein_playback_le_audio_start();
        }  else {
            TASK_LOG_MSGID_CTRL("[le audio MCU] playback source sample rate not supported: %X ",1, in_sample_rate);
        }

    } else {
        TASK_LOG_MSGID_CTRL("[le audio MCU] playback source sink num == 0. Close linein transform",0);
        linein_playback_stop();
        linein_playback_close();
    }
#endif

}

void bt_ble_source_loopback(void)
{
    n9_dsp_share_info_t *p_dl_share;
    n9_dsp_share_info_t *p_ul_share;

    p_dl_share = hal_audio_query_ble_audio_dl_share_info();
    p_ul_share = hal_audio_query_ble_audio_ul_share_info();

    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_DL_BUFFER_INFO, 0x5A5A, (uint32_t)p_dl_share, false);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_UL_BUFFER_INFO, 0x5A5A, (uint32_t)p_ul_share, false);
    TASK_LOG_MSGID_CTRL("[le audio MCU] bt_ble_loopback source",0);

#ifdef MTK_LINEIN_PLAYBACK_ENABLE
    linein_playback_stop();
    linein_playback_close();
#endif
}

void bt_ble_set_shared_memory_information (uint32_t base_address, volatile uint32_t *p_offset_r, volatile uint32_t *p_offset_w, volatile uint32_t *p_control)
{
    bt_ble_codec_internal_handle_t *p_info = bt_ble_codec_internal_handle;
    bt_ble_shared_memory_information_t *p_mem_info = &p_info->mem_info;
    p_mem_info->base_address = base_address;
    p_mem_info->p_offset_r   = p_offset_r;
    p_mem_info->p_offset_w   = p_offset_w;
    p_mem_info->p_control    = p_control;
    return;
}

bool bt_ble_codec_query_is_running(void)
{
    return bt_ble_codec_is_running;
}

hal_audio_sampling_rate_t bt_ble_codec_query_sampling_rate(void)
{
    bt_ble_codec_internal_handle_t *p_info = bt_ble_codec_internal_handle;
    return p_info->codec_info.sample_rate;
}

hal_audio_channel_number_t bt_ble_codec_query_channel_number(void)
{
    return HAL_AUDIO_MONO;
}

#endif

