/* Copyright Statement:
 *
 * (C) 2005-2017 MediaTek Inc. All rights reserved.
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
 * the License Agreement ("Permitted User"). If you are not a Permitted User,
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

#ifndef __HAL_AUDIO_TEST_H__
#define __HAL_AUDIO_TEST_H__

#include "hal_gpt.h"
#include "hal_log.h"
#include "hal_pmu.h"
#include "hal_audio.h"
#include "Audio_FFT.h"
#define FFT_BUFFER_SIZE 256
#define ch_idx                         (smt_curr & SMT_CH_LEFT ? 0:1)

#if ((PRODUCT_VERSION == 1552) || defined(AM255X))
#define KTONE_DL_ON dsp2mcu_audio_msg_t open_msg = MSG_MCU2DSP_PROMPT_OPEN;\
                    dsp2mcu_audio_msg_t start_msg = MSG_MCU2DSP_PROMPT_START;\
                    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PROMPT;\
                    void *p_param_share;\
                    hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, true);\
                    hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true);\
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_OUTPUT_DEVICE_VOLUME, 0, 0, false);\
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 0, 0, false);\
                    mcu2dsp_open_param_t open_param;\
                    open_param.param.stream_in    = STREAM_IN_VP;\
                    open_param.param.stream_out = STREAM_OUT_AFE;\
                    open_param.stream_in_param.playback.bit_type = HAL_AUDIO_BITS_PER_SAMPLING_16;\
                    open_param.stream_in_param.playback.sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;\
                    open_param.stream_in_param.playback.channel_number = HAL_AUDIO_MONO;\
                    open_param.stream_in_param.playback.codec_type = 0;\
                    open_param.stream_in_param.playback.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(msg_type);\
                    hal_audio_reset_share_info( open_param.stream_in_param.playback.p_share_info );\
                    open_param.stream_in_param.playback.p_share_info->sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;\
                    open_param.stream_in_param.playback.p_share_info->bBufferIsFull = 1;\
                    open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL; \
                    open_param.stream_out_param.afe.stream_channel    = HAL_AUDIO_DIRECT;\
                    open_param.stream_out_param.afe.memory            = HAL_AUDIO_MEM2;\
                    open_param.stream_out_param.afe.format            = AFE_PCM_FORMAT_S16_LE;\
                    open_param.stream_out_param.afe.stream_out_sampling_rate    = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ);\
                    open_param.stream_out_param.afe.sampling_rate    = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ);\
                    open_param.stream_out_param.afe.irq_period        = 10;\
                    open_param.stream_out_param.afe.frame_size        = 512;\
                    open_param.stream_out_param.afe.frame_number    = 4;\
                    open_param.stream_out_param.afe.hw_gain         = true;\
                    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), msg_type);\
                    hal_audio_dsp_controller_send_message(open_msg, AUDIO_DSP_CODEC_TYPE_PCM, (uint32_t)p_param_share, true);\
                    mcu2dsp_start_param_t start_param;\
                    start_param.param.stream_in     = STREAM_IN_VP;\
                    start_param.param.stream_out    = STREAM_OUT_AFE;\
                    start_param.stream_out_param.afe.aws_flag          = false;\
                    start_param.stream_out_param.afe.aws_sync_request = true;\
                    start_param.stream_out_param.afe.aws_sync_time      = 1000;\
                    p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), msg_type);\
                    hal_audio_dsp_controller_send_message(start_msg, 0, (uint32_t)p_param_share, true);\
                    *(volatile uint32_t *)0x700001F0 = 0x048C28C2;\
                    *(volatile uint32_t *)0x700001DC = 0x00000024
                    //ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true)

#define KTONE_DL_OFF    dsp2mcu_audio_msg_t stop_msg = MSG_MCU2DSP_PROMPT_STOP;\
                        dsp2mcu_audio_msg_t close_msg = MSG_MCU2DSP_PROMPT_CLOSE;\
                        hal_audio_dsp_controller_send_message(stop_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);\
                        hal_audio_dsp_controller_send_message(close_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);\
                        hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, false);\
                        hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false)
#endif

#if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1565)
#define KTONE_DL_ON dsp2mcu_audio_msg_t open_msg = MSG_MCU2DSP_PROMPT_OPEN;\
                    dsp2mcu_audio_msg_t start_msg = MSG_MCU2DSP_PROMPT_START;\
                    audio_message_type_t msg_type = AUDIO_MESSAGE_TYPE_PROMPT;\
                    void *p_param_share;\
                    hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, true);\
                    hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true);\
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_OUTPUT_DEVICE_VOLUME, 0, 0, false);\
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SET_INPUT_DEVICE_VOLUME, 0, 0, false);\
                    mcu2dsp_open_param_t open_param;\
                    open_param.param.stream_in    = STREAM_IN_VP;\
                    open_param.param.stream_out = STREAM_OUT_AFE;\
                    open_param.stream_in_param.playback.bit_type = HAL_AUDIO_BITS_PER_SAMPLING_16;\
                    open_param.stream_in_param.playback.sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;\
                    open_param.stream_in_param.playback.channel_number = HAL_AUDIO_MONO;\
                    open_param.stream_in_param.playback.codec_type = 0;\
                    open_param.stream_in_param.playback.p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(msg_type);\
                    hal_audio_reset_share_info( open_param.stream_in_param.playback.p_share_info );\
                    open_param.stream_in_param.playback.p_share_info->sampling_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;\
                    open_param.stream_in_param.playback.p_share_info->bBufferIsFull = 1;\
                    open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL; \
                    open_param.stream_out_param.afe.stream_channel    = HAL_AUDIO_DIRECT;\
                    open_param.stream_out_param.afe.memory            = HAL_AUDIO_MEM2;\
                    open_param.stream_out_param.afe.format            = AFE_PCM_FORMAT_S16_LE;\
                    open_param.stream_out_param.afe.stream_out_sampling_rate    = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ);\
                    open_param.stream_out_param.afe.sampling_rate    = hal_audio_sampling_rate_enum_to_value(HAL_AUDIO_SAMPLING_RATE_48KHZ);\
                    open_param.stream_out_param.afe.irq_period        = 10;\
                    open_param.stream_out_param.afe.frame_size        = 512;\
                    open_param.stream_out_param.afe.frame_number    = 4;\
                    open_param.stream_out_param.afe.hw_gain         = true;\
                    p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), msg_type);\
                    hal_audio_dsp_controller_send_message(open_msg, AUDIO_DSP_CODEC_TYPE_PCM, (uint32_t)p_param_share, true);\
                    mcu2dsp_start_param_t start_param;\
                    start_param.param.stream_in     = STREAM_IN_VP;\
                    start_param.param.stream_out    = STREAM_OUT_AFE;\
                    start_param.stream_out_param.afe.aws_flag          = false;\
                    start_param.stream_out_param.afe.aws_sync_request = true;\
                    start_param.stream_out_param.afe.aws_sync_time      = 1000;\
                    p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), msg_type);\
                    hal_audio_dsp_controller_send_message(start_msg, 0, (uint32_t)p_param_share, true);\
                    *(volatile uint32_t *)0x700001F0 = 0x048C28C2;\
                    *(volatile uint32_t *)0x700001DC = 0x00000024
                    //ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true)

#define KTONE_DL_OFF    dsp2mcu_audio_msg_t stop_msg = MSG_MCU2DSP_PROMPT_STOP;\
                        dsp2mcu_audio_msg_t close_msg = MSG_MCU2DSP_PROMPT_CLOSE;\
                        hal_audio_dsp_controller_send_message(stop_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);\
                        hal_audio_dsp_controller_send_message(close_msg, AUDIO_DSP_CODEC_TYPE_PCM, 0, true);\
                        hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PROMPT, false);\
                        hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false)
#endif



static const uint16_t empty_feature[2] = {0x0,0x0};
typedef enum smt_chennel_e{
    SMT_CH_LEFT = 0x1,
    SMT_CH_RIGHT = 0x2,
    SMT_CH_NONE = 0x4,
}smt_ch;

typedef union {
    uint8_t value;
    struct {
        uint8_t curr_ch:4;
        uint8_t curr_mic:4;
    }filed;
}smt_status;

typedef struct {
    kal_uint32 u4Freq_data;
    kal_uint32 u4Mag_data;
    int16_t bitstream_buf[FFT_BUFFER_SIZE];
    int16_t *cpyIdx;
}fft_buf_t ;

typedef enum pass_through_test_mic_type_e{
    PTT_AMIC_ACC = 0,
    PTT_AMIC_DCC = 1,
    PTT_DMIC     = 2,
}pass_through_test_mic_type;

typedef enum pass_through_test_mic_side_e{
    PTT_L = 2,
    PTT_R = 3,
}pass_through_test_mic_side;

typedef enum pass_through_test_freq_e{
    PTT_250HZ  = 1,
    PTT_500HZ  = 2,
    PTT_1000HZ = 4,
    PTT_1500HZ = 6,
    PTT_2000HZ = 8,
    PTT_3000HZ = 12,
    PTT_4000HZ = 16,
    PTT_5000HZ = 20,
    PTT_6000HZ = 24,
    PTT_MUTE = 0x3000000,
}pass_through_test_freq;

typedef enum pass_through_test_return_e{
    PTT_FAIL = 0,
    PTT_SUCCESS = 1,
}pass_through_test_return;

typedef struct {
    kal_uint32 freq_data;
    kal_uint32 mag_data;
    double db_data;
}PTT_u4Freq_Mag_data, *pPTT_u4Freq_Mag_data;


void audio_smt_test(bool enable,smt_ch);
void audio_smt_test_pure_on_off(bool ,smt_ch );
void hal_audio_init_stream_buf(fft_buf_t *);
bool audio_stream_in(bool,smt_ch,fft_buf_t *);
uint32_t pass_through_test(pass_through_test_mic_type mic, pass_through_test_mic_side side, pass_through_test_freq freq, pPTT_u4Freq_Mag_data result);

#endif /*defined(__GNUC__)*/

#ifdef AIR_ATA_TEST_ENABLE
typedef enum ata_test_adc_setting_e{
//based on  HAL_DSP_PARA_AU_AFE_RECORD_SCENARIO_s in hal_audio_internal.h
    AMIC_ADC0 = 0x00,
    AMIC_ADC1 = 0x01,
    AMIC_ADC2 = 0x02,
    AMIC_ADC3 = 0x03,
    AMIC_ADC4 = 0x04,
    AMIC_ADC5 = 0x05,

    DMIC_ADC0 = 0x08,
    DMIC_ADC1 = 0x09,
    DMIC_ADC2 = 0x0A,
    DMIC_ADC3 = 0x0B,
    DMIC_ADC4 = 0x0C,
    DMIC_ADC5 = 0x0D,
}ata_test_adc_setting;


void ata_test_ktone_DL(bool enable);
void ata_test_start_stream_in(bool enable);
void ata_test_set_mic(hal_audio_device_t device, uint32_t adc_id);

#endif
