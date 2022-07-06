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
#include "audio_transmitter_playback_port.h"
#include "audio_log.h"
#include "FreeRTOS.h"
#include "bt_sink_srv_ami.h"

#include "audio_nvdm_coef.h"
#include "audio_nvdm_common.h"
#include "hal_audio_message_struct.h"

#ifdef AIR_AUDIO_I2S_SLAVE_TDM_ENABLE
#if defined(MTK_AVM_DIRECT)
extern HAL_DSP_PARA_AU_AFE_CTRL_t audio_nvdm_HW_config;
#endif
#endif
void audio_transmitter_reset_share_info_by_block(n9_dsp_share_info_t *p_share_info, uint32_t buffer_start_address, uint16_t buffer_length, uint32_t max_payload_size)
{
    p_share_info->start_addr = buffer_start_address;
    p_share_info->sub_info.block_size = (max_payload_size + BLK_HEADER_SIZE + 3)/4*4;  //4B align
    p_share_info->sub_info.block_num = buffer_length / p_share_info->sub_info.block_size;
    p_share_info->length = p_share_info->sub_info.block_size * p_share_info->sub_info.block_num;
    p_share_info->read_offset = 0;
    p_share_info->write_offset = 0;
    p_share_info->bBufferIsFull = 0;
}
void audio_transmitter_modify_share_info_by_block(n9_dsp_share_info_t *p_share_info, uint32_t max_payload_size)
{

    p_share_info->sub_info.block_size = (max_payload_size + BLK_HEADER_SIZE + 3) / 4 * 4; //4B align
    p_share_info->sub_info.block_num = p_share_info->length / p_share_info->sub_info.block_size;
    p_share_info->length = p_share_info->sub_info.block_size * p_share_info->sub_info.block_num;
}

/*------------------------------------------------PORT----AIR_AUDIO_I2S_SLAVE_TDM_ENABLE------------------------------------------------------------------*/
#if defined(AIR_AUDIO_I2S_SLAVE_TDM_ENABLE)
void audio_transmitter_tdm_open_playback(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    open_param->param.stream_in  = STREAM_IN_AFE;
    open_param->param.stream_out = STREAM_OUT_AFE;

    hal_audio_get_stream_in_setting_config (AU_DSP_VOICE, &open_param->stream_in_param);
    hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &open_param->stream_out_param);

    open_param->stream_in_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_SLAVE;
    open_param->stream_in_param.afe.memory = config->scenario_config.tdm_config.in_memory;//HAL_AUDIO_MEM1
    open_param->stream_in_param.afe.audio_interface = config->scenario_config.tdm_config.in_interface;//HAL_AUDIO_INTERFACE_1
    open_param->stream_in_param.afe.format = AFE_PCM_FORMAT_S32_LE;
    open_param->stream_in_param.afe.sampling_rate = 48000;
    open_param->stream_in_param.afe.irq_period = 0;
    open_param->stream_in_param.afe.frame_size = 32;
    open_param->stream_in_param.afe.frame_number = 4;
    open_param->stream_in_param.afe.hw_gain = false;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    //I2S Slave parameters
    open_param->stream_in_param.afe.i2s_format = HAL_AUDIO_I2S_I2S;
    open_param->stream_in_param.afe.i2s_word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
    open_param->stream_in_param.afe.i2S_Slave_TDM = HAL_AUDIO_I2S_TDM_8CH;
#endif
    open_param->stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_SLAVE;
    open_param->stream_out_param.afe.memory = config->scenario_config.tdm_config.out_memory;//HAL_AUDIO_MEM1
    open_param->stream_out_param.afe.audio_interface = config->scenario_config.tdm_config.out_interface;//HAL_AUDIO_INTERFACE_1
    open_param->stream_out_param.afe.format = AFE_PCM_FORMAT_S32_LE;
    open_param->stream_out_param.afe.stream_out_sampling_rate = 48000;
    open_param->stream_out_param.afe.sampling_rate = 48000;
    open_param->stream_out_param.afe.irq_period = 0;
    open_param->stream_out_param.afe.frame_size = 32;
    open_param->stream_out_param.afe.frame_number = 4;
    open_param->stream_out_param.afe.hw_gain = true;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    //I2S Slave parameters
    open_param->stream_out_param.afe.i2s_format = HAL_AUDIO_I2S_I2S;
    open_param->stream_out_param.afe.i2s_word_length = HAL_AUDIO_I2S_WORD_LENGTH_32BIT;
    open_param->stream_out_param.afe.i2S_Slave_TDM = HAL_AUDIO_I2S_TDM_2CH;
#endif

    open_param->stream_in_param.afe.ul_adc_mode[0] = HAL_AUDIO_ANALOG_INPUT_ACC10K;
    open_param->stream_in_param.afe.performance = AFE_PEROFRMANCE_HIGH_MODE;
#ifdef ENABLE_HWSRC_CLKSKEW
    open_param->stream_out_param.afe.clkskew_mode = CLK_SKEW_V1;
#endif
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    open_param->stream_out_param.afe.performance = AFE_PEROFRMANCE_NORMAL_MODE;
#endif
}

void audio_transmitter_tdm_start_playback(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    start_param->param.stream_in = STREAM_IN_AFE;
    start_param->param.stream_out = STREAM_OUT_AFE;
    memset((void *)&start_param->stream_in_param, 0, sizeof(mcu2dsp_start_stream_in_param_t));
    memset((void *)&start_param->stream_out_param, 0, sizeof(mcu2dsp_start_stream_out_param_t));
}
#endif

/*---------------------------------------------------------------------------------------------------------------------------*/
audio_transmitter_playback_func_t audio_transmitter_playback_func[AUDIO_TRANSMITTER_SCENARIO_TYPE_MAX] = {
    //open_playback_t,                                    start_playback_t,                set_runtime_config_playback
    {NULL, NULL, NULL}, //AUDIO_TRANSMITTER_A2DP_SOURCE
#if defined(MTK_SENSOR_SOURCE_ENABLE)
    {audio_transmitter_gsensor_open_playback,     audio_transmitter_gsensor_start_playback, NULL},
#else
    {NULL, NULL, NULL}, //AUDIO_TRANSMITTER_GSENSOR
#endif
#if defined(MTK_MULTI_MIC_STREAM_ENABLE)
    {audio_transmitter_multmic_open_playback,     audio_transmitter_multmic_start_playback, NULL},
#else
    {NULL, NULL, NULL}, //AUDIO_TRANSMITTER_MULTI_MIC_STREAM
#endif
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
    {gaming_mode_open_playback,     gaming_mode_start_playback,    gaming_mode_set_runtime_config_playback},
#else
    {NULL, NULL, NULL},//AUDIO_TRANSMITTER_GAMING_MODE
#endif
#if defined (MTK_ANC_SURROUND_MONITOR_ENABLE)
    {audio_transmitter_anc_monitor_open_playback,  audio_transmitter_anc_monitor_start_playback, NULL},
#else
    {NULL, NULL, NULL}, //AUDIO_TRANSMITTER_ANC_MONITOR_STREAM
#endif
    {audio_transmitter_test_open_playback, audio_transmitter_test_start_playback, NULL},//AUDIO_TRANSMITTER_TEST
#if defined (AIR_AUDIO_I2S_SLAVE_TDM_ENABLE)
    {audio_transmitter_tdm_open_playback,  audio_transmitter_tdm_start_playback, NULL},
#else
    {NULL, NULL, NULL}, //AUDIO_TRANSMITTER_TDM
#endif
#if defined (AIR_WIRED_AUDIO_ENABLE)
    {audio_transmitter_wired_audio_open_playback,  audio_transmitter_wired_audio_start_playback, wired_audio_set_runtime_config_playback},
#else
    {NULL, NULL, NULL}, //AUDIO_TRANSMITTER_ANC_MONITOR_STREAM
#endif
#if defined (AIR_ADVANCED_PASSTHROUGH_ENABLE)
    {advanced_passthrough_open_playback,  advanced_passthrough_start_playback, advanced_passthrough_set_runtime_config_playback},
#else
    {NULL, NULL, NULL},
#endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    {ble_audio_dongle_open_playback,     ble_audio_dongle_start_playback,    ble_audio_dongle_set_runtime_config_playback},
#else
    {NULL, NULL, NULL},//AUDIO_TRANSMITTER_BLE_AUDIO_SOURCE_DONGLE
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
#if defined (AIR_AUDIO_HW_LOOPBACK_ENABLE)
    {audio_hw_loopback_open_playback,    audio_hw_loopback_start_playback,   audio_hw_loopback_set_runtime_config_playback},
#else
    {NULL, NULL, NULL},//AUDIO_TRANSMITTER_AUDIO_HW_LOOKBACK
#endif /* AIR_AUDIO_HW_LOOPBACK_ENABLE */
};

/*---------------------------------------------------------------------------------------------------------------------------*/
audio_transmitter_runtime_config_handler_t audio_transmitter_runtime_config_handler[AUDIO_TRANSMITTER_SCENARIO_TYPE_MAX] = {
    //set_runtime_config             get_runtime_config
    { NULL},
    { NULL},
    { NULL},
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
    { gaming_mode_get_runtime_config},
#else
    { NULL},
#endif
    { NULL},
    { NULL},
    { NULL},
#if defined (AIR_WIRED_AUDIO_ENABLE)
    { NULL},
#else
    { NULL},
#endif
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    { advanced_passthrough_get_runtime_config},
#else
    { NULL},
#endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    { ble_audio_dongle_get_runtime_config},
#else
    { NULL},
#endif /* defined(AIR_BLE_AUDIO_DONGLE_ENABLE) */
#if defined (AIR_AUDIO_HW_LOOPBACK_ENABLE)
     {NULL},
#else
    { NULL},
#endif /* defined(AIR_BLE_AUDIO_DONGLE_ENABLE) */
};

/*---------------------------------------------------------------------------------------------------------------------------*/
state_change_handler_t audio_transmitter_state_change_handler[AUDIO_TRANSMITTER_SCENARIO_TYPE_MAX][AUDIO_TRANSMITTER_STATE_MAX] = {
    //AUDIO_TRANSMITTER_STATE_CLOSE   AUDIO_TRANSMITTER_STATE_IDLE   AUDIO_TRANSMITTER_STATE_STARTING   AUDIO_TRANSMITTER_STATE_STARTED   AUDIO_TRANSMITTER_STATE_STOPING = 4,
    {NULL, NULL, NULL, NULL, NULL},//AUDIO_TRANSMITTER_A2DP_SOURCE
    {NULL, NULL, NULL, NULL, NULL},//AUDIO_TRANSMITTER_GSENSOR
    {NULL, NULL, NULL, NULL, NULL},//AUDIO_TRANSMITTER_MULTI_MIC_STREAM
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
    {NULL, gaming_mode_state_idle_handler, gaming_mode_state_starting_handler, gaming_mode_state_started_handler, gaming_mode_state_stoping_handler},
#else
    {NULL, NULL, NULL, NULL, NULL},
#endif
    {NULL, NULL, NULL, NULL, NULL},//AUDIO_TRANSMITTER_ANC_MONITOR_STREAM
    {NULL, NULL, NULL, NULL, NULL},//AUDIO_TRANSMITTER_TEST
	{NULL, NULL, NULL, NULL, NULL},//AUDIO_TRANSMITTER_TDM
#if defined(AIR_WIRED_AUDIO_ENABLE)
    {NULL, wired_audio_state_idle_handler, NULL, wired_audio_state_started_handler, NULL},
#else
    {NULL, NULL, NULL, NULL, NULL},
#endif
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    {NULL, advanced_passthrough_state_idle_handler, NULL, advanced_passthrough_state_started_handler, NULL},
#else
    {NULL, NULL, NULL, NULL, NULL},
#endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    {NULL, ble_audio_dongle_state_idle_handler, NULL, ble_audio_dongle_state_started_handler, NULL},
#else
    {NULL, NULL, NULL, NULL, NULL},//AUDIO_TRANSMITTER_BLE_AUDIO_SOURCE_DONGLE
#endif /* defined(AIR_BLE_AUDIO_DONGLE_ENABLE) */
#if defined (AIR_AUDIO_HW_LOOPBACK_ENABLE)
     {NULL, NULL, NULL, NULL, NULL},
#else
     {NULL, NULL, NULL, NULL, NULL},
#endif /* AIR_AUDIO_HW_LOOPBACK_ENABLE */
};


