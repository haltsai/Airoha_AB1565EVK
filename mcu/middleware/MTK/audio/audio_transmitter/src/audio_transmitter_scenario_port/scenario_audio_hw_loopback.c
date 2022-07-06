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

#if defined AIR_AUDIO_HW_LOOPBACK_ENABLE

/* Includes ------------------------------------------------------------------*/
#include "scenario_audio_hw_loopback.h"
#include "audio_transmitter_control.h"
#include "hal_audio.h"
#include "bt_sink_srv_ami.h"
#include "hal_audio_internal.h"
#include "audio_transmitter_control_port.h"
#include "hal_platform.h"
#include "audio_transmitter_playback_port.h"

extern void bt_sink_srv_am_set_volume(bt_sink_srv_am_stream_type_t in_out, bt_sink_srv_audio_setting_vol_info_t *vol_info);
/* Public functions ----------------------------------------------------------*/
void audio_hw_loopback_open_playback(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    TRANSMITTER_LOG_I("[HW_LOOPBACK] audio_hw_loopback_open_playback", 0);
    hal_audio_device_t in_audio_device = 0;
    hal_audio_device_t out_audio_device = 0;
    if(config->scenario_sub_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_I2S0_TO_DAC){
        in_audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;
        open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        open_param->stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;

        hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param->stream_out_param);
        out_audio_device = open_param->stream_out_param.afe.audio_device;
    }
    else if(config->scenario_sub_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_ADC_TO_I2S0){
        hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param->stream_in_param);
        open_param->stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3;//HAL_AUDIO_MEM3 to enable echo referencr;

        in_audio_device = open_param->stream_in_param.afe.audio_device;
        bt_sink_srv_audio_setting_vol_info_t vol_info;
        vol_info.type = VOL_HFP;
        vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_MSBC;
        vol_info.vol_info.hfp_vol_info.dev_in = HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC;
        vol_info.vol_info.hfp_vol_info.dev_out = HAL_AUDIO_DEVICE_HEADSET;
        vol_info.vol_info.hfp_vol_info.lev_in = 0;
        vol_info.vol_info.hfp_vol_info.lev_out = 0;
        bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);

        out_audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;
        open_param->stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        open_param->stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;
    }
    else if(config->scenario_sub_id == AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_LINEIN_TO_I2S2){
        in_audio_device = HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL;
        open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        open_param->stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL;
        out_audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;
        open_param->stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_3;
        open_param->stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;
    }
    else{
        TRANSMITTER_LOG_I("[HW_LOOPBACK] audio_hw_loopback_open_playback fail not have scenario_sub_id %d", 1, config->scenario_sub_id);
    }

    if(open_param->stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param->stream_in_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
    } else {
        if (in_audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
            open_param->stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;
        } else {
        open_param->stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
        }
    }

    open_param->stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
    open_param->stream_out_param.afe.format = AFE_PCM_FORMAT_S32_LE;
    open_param->stream_out_param.afe.stream_out_sampling_rate = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
    open_param->stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;

    open_param->stream_out_param.afe.hw_gain = false;
    if(open_param->stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
        open_param->stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
    } else {
        open_param->stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
    }

    open_param->stream_out_param.afe.performance = 1;
#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Open i2s master in low jitter mode
    hal_audio_i2s_master_in_low_jitter_mode(LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_IN, in_audio_device, hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ), open_param->stream_in_param.afe.is_low_jitter, true);
    //Open i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_OUT, out_audio_device, open_param->stream_out_param.afe.sampling_rate, open_param->stream_out_param.afe.is_low_jitter, true);
#endif

    // Open uplink high res
    hal_audio_dsp_ul_clkmux_control(in_audio_device, hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ), true);
    // Open downlink high res
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_LINEIN, out_audio_device, open_param->stream_out_param.afe.sampling_rate, true);

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_OUT, HAL_AUDIO_SAMPLING_RATE_48KHZ, true);
#endif
}

void audio_hw_loopback_start_playback(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    TRANSMITTER_LOG_I("[HW_LOOKBACK] audio_hw_loopback_start_playback", 0);
}

audio_transmitter_status_t audio_hw_loopback_set_runtime_config_playback(audio_transmitter_config_t *config, audio_transmitter_runtime_config_type_t runtime_config_type, audio_transmitter_runtime_config_t *runtime_config, mcu2dsp_audio_transmitter_runtime_config_param_t *runtime_config_param)
{
    return AUDIO_TRANSMITTER_STATUS_FAIL;
}

#endif /* AIR_AUDIO_HW_LOOPBACK_ENABLE */
