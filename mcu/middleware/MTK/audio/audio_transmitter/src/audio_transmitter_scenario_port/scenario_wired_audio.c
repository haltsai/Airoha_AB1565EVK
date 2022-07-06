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

#if defined (AIR_WIRED_AUDIO_ENABLE)
#include "scenario_wired_audio.h"
#include "audio_transmitter_playback_port.h"
#include "bsp_audio_ext_codec.h"

static uint8_t is_ext_dac_initialized = false;

#if defined(MTK_AVM_DIRECT)
extern HAL_DSP_PARA_AU_AFE_CTRL_t audio_nvdm_HW_config;
#endif

#define DONGLE_PAYLOAD_SIZE_PCM 192//1ms for 48K/16bit/Stereo
//#define DONGLE_PAYLOAD_SIZE_ENCODED 80//2.5ms for 48K/16bit/Stereo/opus

uint32_t usb_audio_first_in_flag_0 = 0;
uint32_t usb_audio_first_in_flag_1 = 0;
uint16_t usb_audio_latency_debug_0 = 0;
uint16_t usb_audio_latency_debug_1 = 0;
uint8_t  usb_audio_out_started_flag = 0;


int8_t usb_audio_start_number = 0;
uint8_t usb_audio_use_afe_subid = 0;
int8_t wired_audio_get_usb_audio_start_number(void){
    TRANSMITTER_LOG_E("usb_audio_start_number = %d, get", 1, usb_audio_start_number);
    return usb_audio_start_number;
}
void wired_audio_add_usb_audio_start_number(void){
    usb_audio_start_number++;
    TRANSMITTER_LOG_E("usb_audio_start_number = %d, ++", 1, usb_audio_start_number);
}
void wired_audio_minus_usb_audio_start_number(void){
    usb_audio_start_number--;
    TRANSMITTER_LOG_E("usb_audio_start_number = %d, --", 1, usb_audio_start_number);
}
uint8_t wired_audio_get_usb_use_afe_subid(void){
    TRANSMITTER_LOG_E("usb_audio_use_afe_subid = %d, get", 1, usb_audio_use_afe_subid);
    return usb_audio_use_afe_subid;
}
void wired_audio_set_usb_use_afe_subid(uint8_t sub_id){
    usb_audio_use_afe_subid = sub_id;
    TRANSMITTER_LOG_E("usb_audio_use_afe_subid = %d, set", 1, usb_audio_use_afe_subid);
}

#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
#include "usbaudio_drv.h"
static audio_transmitter_block_header_t usb_stream_header = {0, 0};

static void usb_audio_rx_cb_gaming_dongle_0(void)
{
    static uint32_t previous_count = 0;
    uint32_t gpt_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    if ((gpt_count - previous_count) > 2000) {
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 0]usb_audio_rx_cb_0 duration = %d", 1, gpt_count - previous_count);
    }

    previous_count = gpt_count;

    n9_dsp_share_info_t *p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_WIRED_AUDIO_DSP_RECEIVE_FROM_MCU_0);

    uint32_t buf_size = 0;
    uint8_t *p_source_buf = NULL;

    uint32_t available_data_size = USB_Audio_Get_Len_Received_Data(0);

    if (usb_audio_first_in_flag_0 == 0) {
        USB_Audio_Rx_Buffer_Drop_Bytes(0, available_data_size);
        usb_audio_first_in_flag_0 = 1;
    }

    if (available_data_size > DONGLE_PAYLOAD_SIZE_PCM) {
        log_hal_msgid_info("[USB_RX_DEBUG 0]Too much data in USB buffer %d\r\n", 1, available_data_size);
    }

    while (available_data_size >= DONGLE_PAYLOAD_SIZE_PCM) {
        hal_audio_buf_mgm_get_free_buffer(p_dsp_info, &p_source_buf, &buf_size);
        if (buf_size < (DONGLE_PAYLOAD_SIZE_PCM + BLK_HEADER_SIZE)) {
            TRANSMITTER_LOG_I("[USB_RX_DEBUG 0]Not enough share buffer space, buf_size, buf_size:%d, remiansize:%d, ro:%d, wo:%d, is_full:%d",
            5, buf_size,DONGLE_PAYLOAD_SIZE_PCM + BLK_HEADER_SIZE, p_dsp_info->read_offset, p_dsp_info->write_offset,p_dsp_info->bBufferIsFull);
            //configASSERT(0);
            break;
        }

        usb_stream_header.sequence_number++;
        usb_stream_header.data_length = DONGLE_PAYLOAD_SIZE_PCM;

        memcpy(p_source_buf, &usb_stream_header, BLK_HEADER_SIZE);
        USB_Audio_Read_Data(0, p_source_buf + BLK_HEADER_SIZE, DONGLE_PAYLOAD_SIZE_PCM);
        //LOG_AUDIO_DUMP(p_source_buf + BLK_HEADER_SIZE, DONGLE_PAYLOAD_SIZE_PCM, 13);
        hal_audio_buf_mgm_get_write_data_done(p_dsp_info, DONGLE_PAYLOAD_SIZE_PCM + BLK_HEADER_SIZE);

#if 0
        hal_ccni_message_t ccni_msg;
        ccni_msg.ccni_message[0] = usb_stream_header.sequence_number;
        ccni_msg.ccni_message[1] = 0;
        for (uint32_t i = 0 ; (hal_ccni_set_event(CCNI_CM4_TO_DSP0_EVENT2, &ccni_msg)) != HAL_CCNI_STATUS_OK ; i++) {
            if ((i % 1000) == 0) {
                log_hal_msgid_info("[USB_RX_DEBUG]Send message waiting %d\r\n", 1, (int)i);
            }
        }
#endif

#if GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY
        static uint32_t last_level = 0;
        static int16_t last_sample = 0x0;
        int16_t *start_address = NULL;
        uint32_t current_level = 0;
        uint32_t i;

        if (usb_audio_latency_debug_0)
        {
            current_level = last_level;
            start_address = (int16_t *)(p_source_buf + BLK_HEADER_SIZE);
            if ((*(start_address + 2 * 0) > last_sample) &&
                    (*(start_address + 2 * 0) - last_sample > 40000)) {
                current_level = 1;
            } else if ((*(start_address + 2 * 0) < last_sample) &&
                    (last_sample - * (start_address + 2 * 0) > 40000)) {
                current_level = 0;
            }
            for (i = 0; i < DONGLE_PAYLOAD_SIZE_PCM / 4 - 1; i++) {
                if ((*(start_address + 2 * i) > *(start_address + 2 * (i + 1))) &&
                        (*(start_address + 2 * i) - * (start_address + 2 * (i + 1)) > 40000)) {
                    current_level = 0;
                    break;
                } else if ((*(start_address + 2 * i) < * (start_address + 2 * (i + 1))) &&
                        (*(start_address + 2 * (i + 1)) - * (start_address + 2 * i) > 40000)) {
                    current_level = 1;
                    break;
                } else {
                }
            }
            last_sample = *(start_address + 2 * (DONGLE_PAYLOAD_SIZE_PCM / 4 - 1));
            if (current_level != last_level) {
                hal_gpio_set_output(HAL_GPIO_13, current_level);
                last_level = current_level;
            }
        }
#endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY */
        available_data_size = USB_Audio_Get_Len_Received_Data(0);
    }
}


static void usb_audio_rx_cb_gaming_dongle_1(void)
{
    static uint32_t previous_count = 0;
    uint32_t gpt_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    if ((gpt_count - previous_count) > 2000) {
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 1]usb_audio_rx_cb_1 duration = %d", 1, gpt_count - previous_count);
    }

    previous_count = gpt_count;

    n9_dsp_share_info_t *p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_WIRED_AUDIO_DSP_RECEIVE_FROM_MCU_1);

    uint32_t buf_size = 0;
    uint8_t *p_source_buf = NULL;

    uint32_t available_data_size = USB_Audio_Get_Len_Received_Data(1);

    if (usb_audio_first_in_flag_1 == 0) {
        USB_Audio_Rx_Buffer_Drop_Bytes(1, available_data_size);
        usb_audio_first_in_flag_1 = 1;
    }

    if (available_data_size > DONGLE_PAYLOAD_SIZE_PCM) {
        log_hal_msgid_info("[USB_RX_DEBUG 1]Too much data in USB buffer %d\r\n", 1, available_data_size);
    }

    while (available_data_size >= DONGLE_PAYLOAD_SIZE_PCM) {
        hal_audio_buf_mgm_get_free_buffer(p_dsp_info, &p_source_buf, &buf_size);
        if (buf_size < (DONGLE_PAYLOAD_SIZE_PCM + BLK_HEADER_SIZE)) {
            TRANSMITTER_LOG_I("[USB_RX_DEBUG 1]Not enough share buffer space", 0);
            break;
        }

        usb_stream_header.sequence_number++;
        usb_stream_header.data_length = DONGLE_PAYLOAD_SIZE_PCM;

        memcpy(p_source_buf, &usb_stream_header, BLK_HEADER_SIZE);
        USB_Audio_Read_Data(1, p_source_buf + BLK_HEADER_SIZE, DONGLE_PAYLOAD_SIZE_PCM);
        //LOG_AUDIO_DUMP(p_source_buf + BLK_HEADER_SIZE, DONGLE_PAYLOAD_SIZE_PCM, 13);
        hal_audio_buf_mgm_get_write_data_done(p_dsp_info, DONGLE_PAYLOAD_SIZE_PCM + BLK_HEADER_SIZE);

#if 0
        hal_ccni_message_t ccni_msg;
        ccni_msg.ccni_message[0] = usb_stream_header.sequence_number;
        ccni_msg.ccni_message[1] = 0;
        for (uint32_t i = 0 ; (hal_ccni_set_event(CCNI_CM4_TO_DSP0_EVENT2, &ccni_msg)) != HAL_CCNI_STATUS_OK ; i++) {
            if ((i % 1000) == 0) {
                log_hal_msgid_info("[USB_RX_DEBUG]Send message waiting %d\r\n", 1, (int)i);
            }
        }
#endif

#if GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY
        static uint32_t last_level = 0;
        static int16_t last_sample = 0x0;
        int16_t *start_address = NULL;
        uint32_t current_level = 0;
        uint32_t i;

        if (usb_audio_latency_debug_1)
        {
            current_level = last_level;
            start_address = (int16_t *)(p_source_buf + BLK_HEADER_SIZE);
            if ((*(start_address + 2 * 0) > last_sample) &&
                    (*(start_address + 2 * 0) - last_sample > 40000)) {
                current_level = 1;
            } else if ((*(start_address + 2 * 0) < last_sample) &&
                    (last_sample - * (start_address + 2 * 0) > 40000)) {
                current_level = 0;
            }
            for (i = 0; i < DONGLE_PAYLOAD_SIZE_PCM / 4 - 1; i++) {
                if ((*(start_address + 2 * i) > *(start_address + 2 * (i + 1))) &&
                        (*(start_address + 2 * i) - * (start_address + 2 * (i + 1)) > 40000)) {
                    current_level = 0;
                    break;
                } else if ((*(start_address + 2 * i) < * (start_address + 2 * (i + 1))) &&
                        (*(start_address + 2 * (i + 1)) - * (start_address + 2 * i) > 40000)) {
                    current_level = 1;
                    break;
                } else {
                }
            }
            last_sample = *(start_address + 2 * (DONGLE_PAYLOAD_SIZE_PCM / 4 - 1));
            if (current_level != last_level) {
                hal_gpio_set_output(HAL_GPIO_13, current_level);
                last_level = current_level;
            }
        }
#endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY */
        available_data_size = USB_Audio_Get_Len_Received_Data(1);
    }
}

#endif

/*------------------------------------------------PORT----AUDIO_TRANSMITTER_WIRED_AUDIO------------------------------------------------------------------*/
void audio_transmitter_wired_audio_open_playback(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    mcu2dsp_open_param_t open_param_performance;
    memset(&open_param_performance,0,sizeof(mcu2dsp_open_param_t));
    hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param_performance.stream_out_param);

    if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT){
        sysram_status_t     status;
        DSP_FEATURE_TYPE_LIST AudioFeatureList_GameVoiceHead[2] = {
            FUNC_GAMING_HEADSET,
            FUNC_END,
        };
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
        voice_mic_type_t mic_cur_type = hal_audio_query_voice_mic_type();
        TRANSMITTER_LOG_I("[WIRED_AUDIO_USB_OUT]Wired line out mic_cur_type: 0x%x", 1, mic_cur_type);
        if(mic_cur_type == VOICE_MIC_TYPE_FIXED) {
            AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_HEADSET;
        } else if(mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) {
            AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_BOOM_MIC;
        } else {
            TRANSMITTER_LOG_E("[WIRED_AUDIO_USB_OUT]Wired line out no this mic type - (%d)\r\n", 1, mic_cur_type);
            configASSERT(0);
        }
#endif

        /* reset share buffer before put parameters*/
        audio_nvdm_reset_sysram();
        status = audio_nvdm_set_feature(2, AudioFeatureList_GameVoiceHead);
        if (status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("[WIRED_AUDIO_USB_OUT]Wired line out is failed to set parameters to share memory - err(%d)\r\n", 1, status);
            configASSERT(0);
        }

        open_param->param.stream_in = STREAM_IN_AFE;
        hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param->stream_in_param);
        //open_param->stream_in_param.afe.audio_device    = HAL_AUDIO_DEVICE_MAIN_MIC_L;
        open_param->stream_in_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
        open_param->stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3; //mic in & echo ref
        //open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        open_param->stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
        open_param->stream_in_param.afe.sampling_rate   = 16000;
        open_param->stream_in_param.afe.irq_period      = 15;
        open_param->stream_in_param.afe.frame_size      = 240;
        open_param->stream_in_param.afe.frame_number    = 6;
        open_param->stream_in_param.afe.hw_gain         = false;
        //open_param->stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;

        open_param->param.stream_out = STREAM_OUT_AUDIO_TRANSMITTER;
        open_param->stream_out_param.data_ul.scenario_type = config->scenario_type;
        open_param->stream_out_param.data_ul.scenario_sub_id = config->scenario_sub_id;
        open_param->stream_out_param.data_ul.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_SEND_TO_MCU);
        open_param->stream_out_param.data_ul.data_notification_frequency = 0;
        open_param->stream_out_param.data_ul.max_payload_size = 1440;
        open_param->stream_out_param.data_ul.scenario_param.wired_audio_param.codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
        if ((config->scenario_config.wired_audio_config.usb_out_config.usb_codec_param.pcm.sample_rate != 16000) && (config->scenario_config.wired_audio_config.usb_out_config.usb_codec_param.pcm.sample_rate != 48000))
        {
            TRANSMITTER_LOG_E("[WIRED_AUDIO_USB_OUT]Error usb sample rate, %u\r\n", 1, config->scenario_config.wired_audio_config.usb_out_config.usb_codec_param.pcm.sample_rate);
            //configASSERT(0);
            config->scenario_config.wired_audio_config.usb_out_config.usb_codec_param.pcm.sample_rate = 16000;
        }
        TRANSMITTER_LOG_I("[WIRED_AUDIO_USB_OUT]USB out sample rate = %d", 1, config->scenario_config.wired_audio_config.usb_out_config.usb_codec_param.pcm.sample_rate);
        open_param->stream_out_param.data_ul.scenario_param.wired_audio_param.codec_param.pcm.sample_rate = config->scenario_config.wired_audio_config.usb_out_config.usb_codec_param.pcm.sample_rate;
        open_param->stream_out_param.data_ul.p_share_info->read_offset = 0;
        open_param->stream_out_param.data_ul.p_share_info->write_offset = 0;
        open_param->stream_out_param.data_ul.p_share_info->bBufferIsFull = false;
        /*share buffer modify by transmitter, n9_dsp buffer info*/
        audio_transmitter_modify_share_info_by_block(open_param->stream_out_param.data_ul.p_share_info, open_param->stream_out_param.data_ul.max_payload_size);
        /*data read by bt bt host, read by transmitter read API*/
        //*read_info = open_param->stream_out_param.data_ul.p_share_info;
    }
    else if((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)){
        open_param->param.stream_in = STREAM_IN_AUDIO_TRANSMITTER;
        open_param->param.stream_out = STREAM_OUT_AFE;
        open_param->stream_in_param.data_dl.scenario_type = config->scenario_type;
        open_param->stream_in_param.data_dl.scenario_sub_id = config->scenario_sub_id;
        //reuse dongle sharebuffer
        if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) {
            open_param->stream_in_param.data_dl.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_WIRED_AUDIO_DSP_RECEIVE_FROM_MCU_0);
        } else {
            open_param->stream_in_param.data_dl.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_WIRED_AUDIO_DSP_RECEIVE_FROM_MCU_1);
        }

        if(wired_audio_get_usb_audio_start_number() > 1){
            open_param->param.stream_out = STREAM_OUT_VIRTUAL;
        }
        open_param->stream_in_param.data_dl.data_notification_frequency = 0;
        open_param->stream_in_param.data_dl.max_payload_size = DONGLE_PAYLOAD_SIZE_PCM;
        //reuse gaming_mode_param
        open_param->stream_in_param.data_dl.scenario_param.wired_audio_param.codec_type = AUDIO_DSP_CODEC_TYPE_PCM;

        audio_transmitter_reset_share_info_by_block(open_param->stream_in_param.data_dl.p_share_info,
        open_param->stream_in_param.data_dl.p_share_info->start_addr,
        open_param->stream_in_param.data_dl.p_share_info->length,
        DONGLE_PAYLOAD_SIZE_PCM);

        usb_audio_first_in_flag_0 = 0;
        usb_audio_first_in_flag_1 = 0;

        //set volume
        uint32_t gain = audio_get_gain_out_in_dB(0, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
        uint32_t gain_default=gain;
        uint8_t volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_L;
        if(volume_level <= bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
            gain = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
        }
        open_param->stream_in_param.data_dl.scenario_param.wired_audio_param.gain_default_L = gain;
        gain = gain_default;
        volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_R;
        if(volume_level <= bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
            gain = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
        }
        open_param->stream_in_param.data_dl.scenario_param.wired_audio_param.gain_default_R = gain;

        //set STREAM OUT PARAM
        hal_audio_sampling_rate_t linein_sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
        hal_audio_device_t out_audio_device = HAL_AUDIO_DEVICE_DAC_DUAL;
        hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param->stream_out_param);
        TRANSMITTER_LOG_I("[WIRED_AUDIO_USB_IN_0]Open out_device(0x%x), channel(%d), interface(%d)", 3, open_param->stream_out_param.afe.audio_device, open_param->stream_out_param.afe.stream_channel, open_param->stream_out_param.afe.audio_interface);

        open_param->stream_out_param.afe.audio_device = out_audio_device;
        open_param->stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
        open_param->stream_out_param.afe.memory = HAL_AUDIO_MEM3;
        open_param->stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        open_param->stream_out_param.afe.format = AFE_PCM_FORMAT_S16_LE;//AFE_PCM_FORMAT_S32_LE;
        open_param->stream_out_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
        open_param->stream_out_param.afe.sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
        //open_param->stream_out_param.afe.irq_period = 512 / (hal_audio_sampling_rate_enum_to_value(linein_sample_rate)/1000);
        open_param->stream_out_param.afe.irq_period = 10;
        open_param->stream_out_param.afe.frame_size = 512;
        open_param->stream_out_param.afe.frame_number = 2;
        open_param->stream_out_param.afe.hw_gain = true;
#ifdef ENABLE_HWSRC_CLKSKEW
        open_param->stream_out_param.afe.clkskew_mode = CLK_SKEW_V1;
#endif

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        // open_param->stream_out_param.afe.performance = AFE_PEROFRMANCE_NORMAL_MODE;
        // switch(audio_nvdm_HW_config.adc_dac_config.ADDA_DAC_Mode_Sel) {
        //     case 0x00: //Class G
        //     case 0x01: //Class AB
        //         open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
        //         break;
        //     case 0x02: //Class D
        //         open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSD;
        //         break;
        //     default:
        //         open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
        //         break;
        // }

        // //I2S Slave parameters
        // open_param->stream_out_param.afe.i2s_format = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Format;
        // open_param->stream_out_param.afe.i2s_word_length = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Word_length;
#endif
        if(open_param->stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
            open_param->stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        } else {
            open_param->stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
        }

        hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_PLAYBACK, out_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), true);

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
        ami_set_afe_param(STREAM_OUT, linein_sample_rate, true);
#endif

    }
    else if((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)){
        sysram_status_t     status;
        DSP_FEATURE_TYPE_LIST AudioFeatureList_GameVoiceHead[2] = {
            FUNC_GAMING_HEADSET,
            FUNC_END,
        };
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
        voice_mic_type_t mic_cur_type = hal_audio_query_voice_mic_type();
        TRANSMITTER_LOG_I("[WIRED_AUDIO_LINE_OUT]Wired line out mic_cur_type: 0x%x", 1, mic_cur_type);
        if(mic_cur_type == VOICE_MIC_TYPE_FIXED) {
            AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_HEADSET;
        } else if(mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) {
            AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_BOOM_MIC;
        } else {
            TRANSMITTER_LOG_E("[WIRED_AUDIO_LINE_OUT]Wired line out no this mic type - (%d)\r\n", 1, mic_cur_type);
            configASSERT(0);
        }
#endif

        /* reset share buffer before put parameters*/
        audio_nvdm_reset_sysram();
        status = audio_nvdm_set_feature(2, AudioFeatureList_GameVoiceHead);
        if (status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("[WIRED_AUDIO_LINE_OUT]Wired line out is failed to set parameters to share memory - err(%d)\r\n", 1, status);
            configASSERT(0);
        }

        open_param->param.stream_in = STREAM_IN_AFE;
        hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param->stream_in_param);
        //open_param->stream_in_param.afe.audio_device    = HAL_AUDIO_DEVICE_MAIN_MIC_L;
        open_param->stream_in_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
        open_param->stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3; //mic in & echo ref

        //open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        open_param->stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
        open_param->stream_in_param.afe.sampling_rate   = 16000;
        open_param->stream_in_param.afe.irq_period      = 15;
#ifdef AIR_ULL_VOICE_LOW_LATENCY_ENABLE
        open_param->stream_in_param.afe.frame_size      = 120;
#else
        open_param->stream_in_param.afe.frame_size      = 240;
#endif
        open_param->stream_in_param.afe.frame_number    = 6;
        open_param->stream_in_param.afe.hw_gain         = false;
        //open_param->stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;

        open_param->param.stream_out = STREAM_OUT_AFE;

#if 0
        open_param->stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_DAC_DUAL;//use DAC DUAL for temp test
        open_param->stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
#else

        open_param->stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;//use I2S MASTER
        open_param->stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;//I2S 0
        if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT){
            bsp_audio_ext_codec_dac_init(BSP_AUDIO_EXT_CODEC_TYPE_DA7212, NULL);
            bsp_audio_ext_codec_dac_enable(BSP_AUDIO_EXT_CODEC_TYPE_DA7212);
            is_ext_dac_initialized = true;
        }
#endif
        open_param->stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
        open_param->stream_out_param.afe.memory = HAL_AUDIO_MEM1;
        open_param->stream_out_param.afe.format = AFE_PCM_FORMAT_S16_LE;
        open_param->stream_out_param.afe.stream_out_sampling_rate = 16000;
        open_param->stream_out_param.afe.sampling_rate = 16000;
        //open_param->stream_out_param.afe.irq_period = 512 / (hal_audio_sampling_rate_enum_to_value(linein_sample_rate)/1000);
        open_param->stream_out_param.afe.irq_period = 0;
        open_param->stream_out_param.afe.frame_size = 240;
        open_param->stream_out_param.afe.frame_number = 4;
        open_param->stream_out_param.afe.hw_gain = false;
    if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER){
        open_param->stream_out_param.afe.with_upwdown_sampler     = true;
        open_param->stream_out_param.afe.audio_path_input_rate    = 16000;
        open_param->stream_out_param.afe.audio_path_output_rate   = 48000;
        open_param->stream_out_param.afe.memory = HAL_AUDIO_MEM4;
        open_param->stream_out_param.afe.audio_device = HAL_AUDIO_DEVICE_I2S_MASTER;
    }
//#ifdef ENABLE_HWSRC_CLKSKEW
//        open_param->stream_out_param.afe.clkskew_mode = CLK_SKEW_V1;
//#endif

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        open_param->stream_out_param.afe.performance = open_param_performance.stream_out_param.afe.performance;
        switch(audio_nvdm_HW_config.adc_dac_config.ADDA_DAC_Mode_Sel) {
            case 0x00: //Class G
                open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSG;
                break;
            case 0x01: //Class AB
                open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
                break;
            case 0x02: //Class D
                open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSD;
                break;
            default:
                open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
                break;
        }

        //I2S Slave parameters
        open_param->stream_out_param.afe.i2s_format = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Format;
        open_param->stream_out_param.afe.i2s_word_length = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Word_length;
#endif
        if(open_param->stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
            open_param->stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
            //if (linein_sample_rate > HAL_AUDIO_SAMPLING_RATE_48KHZ) {
            //    open_param->stream_out_param.afe.misc_parms  = I2S_CLK_SOURCE_APLL;
            //}
        } else {
            open_param->stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
        }
    }
    else if((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE)){

        hal_audio_sampling_rate_t linein_sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
        hal_audio_device_t in_audio_device = HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL;
        hal_audio_device_t out_audio_device = HAL_AUDIO_DEVICE_DAC_DUAL;
        if((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE)){
            in_audio_device = HAL_AUDIO_DEVICE_I2S_SLAVE;
        }
        TRANSMITTER_LOG_I("enter linein_playback_open, sample_rate=%d, in_audio_device=0x%x, out_audio_device=0x%x\n", 3, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), in_audio_device, out_audio_device);

        open_param->param.stream_in = STREAM_IN_AFE;
        open_param->param.stream_out = STREAM_OUT_AFE;

        open_param->stream_in_param.afe.audio_device = in_audio_device;
#ifdef ENABLE_2A2D_TEST
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        open_param->stream_in_param.afe.audio_device1 = in_audio_device;
#endif
#endif
        open_param->stream_in_param.afe.stream_channel = HAL_AUDIO_DIRECT;
        open_param->stream_in_param.afe.memory = HAL_AUDIO_MEM_SUB ;
        if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER){
                open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_3;
        }else{
                open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        }
#ifdef ENABLE_2A2D_TEST
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER){
                open_param->stream_in_param.afe.audio_interface1 = HAL_AUDIO_INTERFACE_3;
        }else{
                open_param->stream_in_param.afe.audio_interface1 = HAL_AUDIO_INTERFACE_1;
        }
#endif
#endif
        open_param->stream_in_param.afe.format = AFE_PCM_FORMAT_S32_LE;
        open_param->stream_in_param.afe.sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
        if((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE)){
            open_param->stream_in_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
        }
        //open_param->stream_in_param.afe.irq_period = 512 / (hal_audio_sampling_rate_enum_to_value(linein_sample_rate)/1000);
        open_param->stream_in_param.afe.irq_period = 0;
        open_param->stream_in_param.afe.frame_size = 256;
        open_param->stream_in_param.afe.frame_number = 2;
        open_param->stream_in_param.afe.hw_gain = false;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        //Linein parameters
        open_param->stream_in_param.afe.performance = AFE_PEROFRMANCE_NORMAL_MODE;
        open_param->stream_in_param.afe.iir_filter[0] = HAL_AUDIO_UL_IIR_DISABLE;
        open_param->stream_in_param.afe.ul_adc_mode[0] = audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC0_Mode;

        //I2S Slave parameters
        open_param->stream_in_param.afe.i2s_format = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Format;
        open_param->stream_in_param.afe.i2s_word_length = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Word_length;
#endif

        if(open_param->stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
            open_param->stream_in_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        } else {
            if (in_audio_device & HAL_AUDIO_DEVICE_LINEINPLAYBACK_DUAL) {
                open_param->stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;
            } else {
                open_param->stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
            }
        }

        open_param->stream_out_param.afe.audio_device = out_audio_device;
        open_param->stream_out_param.afe.stream_channel = HAL_AUDIO_DIRECT;
        open_param->stream_out_param.afe.memory = HAL_AUDIO_MEM3;
        open_param->stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        open_param->stream_out_param.afe.format = AFE_PCM_FORMAT_S32_LE;
        open_param->stream_out_param.afe.stream_out_sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
        open_param->stream_out_param.afe.sampling_rate = hal_audio_sampling_rate_enum_to_value(linein_sample_rate);
        //open_param->stream_out_param.afe.irq_period = 512 / (hal_audio_sampling_rate_enum_to_value(linein_sample_rate)/1000);
        open_param->stream_out_param.afe.irq_period = 0;
        open_param->stream_out_param.afe.frame_size = 256;
        open_param->stream_out_param.afe.frame_number = 2;
        if((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE)){
            open_param->stream_out_param.afe.hw_gain = false;
            open_param->stream_in_param.afe.frame_size = 128;
            open_param->stream_out_param.afe.frame_size = 128;
        }else{
        open_param->stream_out_param.afe.hw_gain = true;
        }
#ifdef ENABLE_HWSRC_CLKSKEW
        open_param->stream_out_param.afe.clkskew_mode = CLK_SKEW_V1;
#endif

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        open_param->stream_out_param.afe.performance = open_param_performance.stream_out_param.afe.performance;
        switch(audio_nvdm_HW_config.adc_dac_config.ADDA_DAC_Mode_Sel) {
            case 0x00: //Class G
                open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSG;
                break;
            case 0x01: //Class AB
                open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
                break;
            case 0x02: //Class D
                open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSD;
                break;
            default:
                open_param->stream_out_param.afe.dl_dac_mode = HAL_AUDIO_ANALOG_OUTPUT_CLASSAB;
                break;
        }

        //I2S Slave parameters
        open_param->stream_out_param.afe.i2s_format = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Format;
        open_param->stream_out_param.afe.i2s_word_length = audio_nvdm_HW_config.I2SS_config.I2S_Slave_Word_length;
#endif
        if(open_param->stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
            open_param->stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_DCXO;
        } else {
            open_param->stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
        }

        // Open uplink high res
        hal_audio_dsp_ul_clkmux_control(in_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), true);
        // Open downlink high res
        hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_LINEIN, out_audio_device, hal_audio_sampling_rate_enum_to_value(linein_sample_rate), true);

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
        ami_set_afe_param(STREAM_OUT, linein_sample_rate, true);
#endif

    }

}

void audio_transmitter_wired_audio_start_playback(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT){
        start_param->param.stream_in = STREAM_IN_AFE;
        start_param->param.stream_out = STREAM_OUT_AUDIO_TRANSMITTER;
    }
    else if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0){
        start_param->param.stream_in = STREAM_IN_AUDIO_TRANSMITTER;
        start_param->param.stream_out = STREAM_OUT_AFE;
    }
    else if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1){
        start_param->param.stream_in = STREAM_IN_AUDIO_TRANSMITTER;
        start_param->param.stream_out = STREAM_OUT_AFE;
    }
    else if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT){
        start_param->param.stream_in = STREAM_IN_AFE;
        start_param->param.stream_out = STREAM_OUT_AFE;
    }
    else if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER){
        start_param->param.stream_in = STREAM_IN_AFE;
        start_param->param.stream_out = STREAM_OUT_AFE;
    }
    else if(config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN){
        start_param->param.stream_in = STREAM_IN_AFE;
        start_param->param.stream_out = STREAM_OUT_AFE;
    }
    else if((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE)){
        start_param->param.stream_in = STREAM_IN_AFE;
        start_param->param.stream_out = STREAM_OUT_AFE;
    }
}

typedef struct {
    uint32_t vol_gain_1;
    uint32_t vol_gain_2;
} vol_gain_t;


#define GAIN_COMPENSATION_STEP 10
const static int16_t gain_compensation_table[GAIN_COMPENSATION_STEP + 1] =
{
    /*
    Ratio |    db    | Compensation
    0%    |  -60db   | 0xE890
    10%   |  -20db   | 0xF830
    20%   | -13.98db | 0xFA8B
    30%   | -10.46db | 0xFBEB
    40%   |  -7.96db | 0xFCE5
    50%   |  -6.02db | 0xFDA6
    60%   |  -4.44db | 0xFE45
    70%   |  -3.1db  | 0xFECB
    80%   |  -1.94db | 0xFF3F
    90%   |  -0.92db | 0xFFA5
    100%  |     0db  | 0
    */
    0xE890,
    0xF830,
    0xFA8B,
    0xFBEB,
    0xFCE5,
    0xFDA6,
    0xFE45,
    0xFECB,
    0xFF3F,
    0xFFA5,
    0x0
};

extern void bt_sink_srv_am_set_volume(bt_sink_srv_am_stream_type_t in_out, bt_sink_srv_audio_setting_vol_info_t *vol_info);
audio_transmitter_status_t wired_audio_set_runtime_config_playback(audio_transmitter_config_t *config, audio_transmitter_runtime_config_type_t runtime_config_type, audio_transmitter_runtime_config_t *runtime_config, mcu2dsp_audio_transmitter_runtime_config_param_t *runtime_config_param){
    uint32_t operation = runtime_config_type;
    TRANSMITTER_LOG_E("operation %d level \r\n", 1,operation);
    if (0) {
        /* code */
    }
    else if ((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)) {
        if((operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_L) || (operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_R)
        || (operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_DUL)){
            vol_gain_t gain = {0,0};
            uint8_t volume_level;
            uint8_t volume_ratio;
            volume_ratio = runtime_config->wired_audio_runtime_config.vol_level.vol_ratio;

            if(volume_ratio > 100) {
                TRANSMITTER_LOG_E("Volume ratio should between 0 and 100, volume_ratio = \r\n", 1,volume_ratio);
                volume_ratio = 100;
            }
            if((operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_L)|| (operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_DUL)){
                volume_level = runtime_config->wired_audio_runtime_config.vol_level.vol_level_l;
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_1 = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
                gain.vol_gain_1 += gain_compensation_table[volume_ratio/GAIN_COMPENSATION_STEP];
            }
            if((operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_R)|| (operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_DUL)){
                volume_level = runtime_config->wired_audio_runtime_config.vol_level.vol_level_r;
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set R volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_2 = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
                gain.vol_gain_2 += gain_compensation_table[volume_ratio/GAIN_COMPENSATION_STEP];
            }
            runtime_config_param->config_operation = operation;
            memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
            TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d volume_ratio = %d.",
                                7 ,config->scenario_sub_id, operation, runtime_config->wired_audio_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->wired_audio_runtime_config.vol_level.vol_level_r, gain.vol_gain_2, volume_ratio);
            return AUDIO_TRANSMITTER_STATUS_SUCCESS;

        }
    }
    else if ((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE)) {
        if(operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_LINEIN){
            uint8_t volume_level = runtime_config->wired_audio_runtime_config.vol_level.vol_level_l;
            TRANSMITTER_LOG_I("set volume %d level \r\n", 1,volume_level);
            if(volume_level > bt_sink_srv_ami_get_lineIN_max_volume_level()){
                volume_level = bt_sink_srv_ami_get_lineIN_max_volume_level();
                TRANSMITTER_LOG_E("set volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
            }

#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
#ifdef MTK_DEVELOPMENT_BOARD_HDK
    #define AUDIO_SINK_SRV_LINE_IN_INPUT_DEVICE HAL_AUDIO_DEVICE_MAIN_MIC
#else
    #define AUDIO_SINK_SRV_LINE_IN_INPUT_DEVICE HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC
#endif
#if defined(__AFE_HS_DC_CALIBRATION__)
    #define AUDIO_SINK_SRV_LINE_IN_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HANDSET
#else
    #define AUDIO_SINK_SRV_LINE_IN_OUTPUT_DEVICE HAL_AUDIO_DEVICE_HEADSET
#endif
            bt_sink_srv_audio_setting_vol_info_t vol_info;
            vol_info.type = VOL_LINE_IN;
            vol_info.vol_info.lineIN_vol_info.dev_in  = AUDIO_SINK_SRV_LINE_IN_INPUT_DEVICE;
            vol_info.vol_info.lineIN_vol_info.dev_out = AUDIO_SINK_SRV_LINE_IN_OUTPUT_DEVICE;
            vol_info.vol_info.lineIN_vol_info.lev_in  = 1;
            vol_info.vol_info.lineIN_vol_info.lev_out = volume_level;

            bt_sink_srv_audio_setting_vol_t vol;
            bt_sink_srv_audio_setting_get_vol(&vol_info, &vol);
            uint32_t digital = 0;
            uint32_t analog = 0;
            digital = vol.vol.lineIN_vol.vol_out.digital;
            analog = vol.vol.lineIN_vol.vol_out.analog_L;
            TRANSMITTER_LOG_E("set volume digital %x  analog %x \r\n", 2,digital,analog);
            hal_audio_set_stream_out_dl3_volume(digital, analog);
#else
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, out_audio_device, volume_level);
#endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
TRANSMITTER_LOG_E("2 set volume %d level \r\n", 1,volume_level);
            return AUDIO_TRANSMITTER_STATUS_FAIL; //not send runtime config msg to dsp!!
        }
        else{
            TRANSMITTER_LOG_E("operation %d can not do in AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN. ",1,operation);
            configASSERT(0);
        }
    } else if((config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) || (config->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)) {
        if((operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_L) || (operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_R)
        || (operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_DUL)){
            vol_gain_t gain;
            uint8_t volume_level;
            if((operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_L)|| (operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_DUL)){
                volume_level = runtime_config->wired_audio_runtime_config.vol_level.vol_level_l;
                TRANSMITTER_LOG_I("set L volume level %d  \r\n", 1,volume_level);
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume level more than max level %d\r\n", 1, bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_1 = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
            }
            if((operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_R)|| (operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_DUL)){
                volume_level = runtime_config->wired_audio_runtime_config.vol_level.vol_level_r;
                TRANSMITTER_LOG_I("set R volume level %d  \r\n", 1,volume_level);
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set R volume level more than max level %d\r\n", 1,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_2 = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
            }
            runtime_config_param->config_operation = operation;
            memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
            TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d.",
                                6 ,config->scenario_sub_id, operation, runtime_config->wired_audio_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->wired_audio_runtime_config.vol_level.vol_level_r, gain.vol_gain_2);
            return AUDIO_TRANSMITTER_STATUS_SUCCESS;
        }
        else{
            TRANSMITTER_LOG_E("operation %d can not do in AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT/AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT/AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER. ",1,operation);
            configASSERT(0);
        }
    }
    return AUDIO_TRANSMITTER_STATUS_FAIL;
}

void wired_audio_state_started_handler(uint8_t scenario_sub_id)
{
    if (scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) {
        usb_audio_first_in_flag_0 = 0;
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
        USB_Audio_Register_Rx_Callback(0, usb_audio_rx_cb_gaming_dongle_0);
#endif
        hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_LOCK);
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 0]register usb_audio_rx_cb", 0);
    }
    else if (scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1) {
        usb_audio_first_in_flag_1 = 0;
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
        USB_Audio_Register_Rx_Callback(1, usb_audio_rx_cb_gaming_dongle_1);
#endif
        hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_LOCK);
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 1]register usb_audio_rx_cb", 0);
    } else if ((scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || (scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) || (scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)) {
#if defined(MTK_3RD_PARTY_NR)
        usb_audio_out_started_flag = 1;
        hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_LOCK);
#endif
    }
}
void wired_audio_state_idle_handler(uint8_t scenario_sub_id)
{
    if (scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) {
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
        USB_Audio_Register_Rx_Callback(0, NULL);
#endif
        usb_audio_first_in_flag_0 = 0;
        hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_UNLOCK);
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 0]Unregister usb_audio_rx_cb", 0);
    } else if (scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1) {
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
        USB_Audio_Register_Rx_Callback(1, NULL);
#endif
        usb_audio_first_in_flag_1 = 0;
        hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_UNLOCK);
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 1]Unregister usb_audio_rx_cb", 0);
    } else if ((scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || (scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) || (scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)) {
#if defined(MTK_3RD_PARTY_NR)
        if(usb_audio_out_started_flag == 1) {
            hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_UNLOCK);
            usb_audio_out_started_flag = 0;
        }
#endif
        TRANSMITTER_LOG_I("[wired_audio]: wired_audio_state_idle_handler for AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT", 0);
        if(is_ext_dac_initialized == true) {
            bsp_audio_ext_codec_dac_disable(BSP_AUDIO_EXT_CODEC_TYPE_DA7212);
            bsp_audio_ext_codec_dac_deinit(BSP_AUDIO_EXT_CODEC_TYPE_DA7212);
            is_ext_dac_initialized = false;
        }
    }
}

#endif
