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

#include "scenario_gaming_mode.h"
#include "audio_transmitter_playback_port.h"
#include "hal_audio.h"
#ifdef MTK_AUDIO_DUMP_ENABLE
#include "audio_dump.h"
#endif
/*------------------------------------------------PORT----AIR_BT_ULTRA_LOW_LATENCY_ENABLE------------------------------------------------------------------*/
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)

#define GAMING_MODE_MUSIC_DONGLE_DEBUG_LANTENCY      1
#ifdef HAL_DVFS_MODULE_ENABLED
static dvfs_frequency_t g_audio_transmitter_dvfs_clk = HAL_DVFS_HALF_SPEED_52M;
#endif
#if defined(MTK_3RD_PARTY_NR)
#include "hal_dvfs_internal.h"
#endif

uint32_t usb_first_in_flag_0 = 0;
uint32_t usb_first_in_flag_1 = 0;
uint16_t gaming_mode_latency_debug_0 = 0;
uint16_t gaming_mode_latency_debug_1 = 0;

#include "usbaudio_drv.h"
static audio_transmitter_block_header_t usb_stream_header = {0, 0};

#define DONGLE_PAYLOAD_SIZE_PCM 192//1ms for 48K/16bit/Stereo
#define DONGLE_PAYLOAD_SIZE_ENCODED 84//2.5ms for 48K/16bit/Stereo/opus + uint32 crc32

#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
#define NVKEYID_F50C            "F50C"
static const uint8_t NVKEY_F50C[] =
{
    0x01, 0x00, 0x00, 0x00, 0x67, 0xF3, 0xFF, 0xFF, 0xC8, 0x00, 0x00, 0x00, 0x67, 0xF3, 0xFF, 0xFF, 0xD0, 0x07, 0x00, 0x00, 0x18, 0xFC, 0xFF, 0xFF, 0x19, 0x00, 0x00, 0x00, 0xE7, 0xFF, 0xFF, 0xFF, 0x91, 0x87, 0x08, 0x00, 0xCB, 0xBF, 0x04, 0x00, 0x90, 0x87, 0x08, 0x00, 0xE8, 0x9F, 0xB8, 0xFF, 0x25, 0x29, 0x32, 0x00, 0x2E, 0xC5, 0x42, 0x00, 0x5F, 0xC7, 0xF3, 0xFF, 0x2D, 0xC5, 0x42, 0x00, 0x29, 0x00, 0xB8, 0xFF, 0x29, 0x36, 0x64, 0x00, 0x00, 0x00, 0x40, 0x00, 0xC9, 0x7E, 0x74, 0x00, 0xCF, 0x84, 0x8B, 0xFF, 0xC8, 0x7E, 0x74, 0x00, 0xA6, 0x23, 0x88, 0xFF, 0x71, 0xA4, 0x70, 0x00, 0xD0, 0xEA, 0x7E, 0x00, 0x16, 0x29, 0x81, 0xFF, 0xCF, 0xEA, 0x7E, 0x00, 0xDF, 0x73, 0x81, 0xFF, 0x85, 0x91, 0x7D, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x1E, 0x00, 0x06, 0x00, 0xB0, 0x0A, 0x0C, 0x00, 0x00, 0x00, 0x00,
};
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
static void usb_audio_rx_cb_gaming_dongle_0(void)
{
    static uint32_t previous_count = 0;
    uint32_t gpt_count, duration_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    n9_dsp_share_info_t *p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_RECEIVE_FROM_MCU_0);

    uint32_t buf_size = 0;
    uint8_t *p_source_buf = NULL;

    uint32_t available_data_size = USB_Audio_Get_Len_Received_Data(0);

    if (usb_first_in_flag_0 == 0) {
        USB_Audio_Rx_Buffer_Drop_Bytes(0, available_data_size);
        usb_first_in_flag_0 = 1;
        previous_count = gpt_count;
        return;
    }

    hal_gpt_get_duration_count(previous_count, gpt_count, &duration_count);
    if (duration_count > 2000) {
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 0]usb_audio_rx_cb_gaming_dongle_0 duration = %d", 1, duration_count);
    }
    previous_count = gpt_count;

    if (available_data_size > DONGLE_PAYLOAD_SIZE_PCM) {
        log_hal_msgid_info("[USB_RX_DEBUG 0]Too much data in USB buffer %d\r\n", 1, available_data_size);
    }

    while (available_data_size >= DONGLE_PAYLOAD_SIZE_PCM) {
        hal_audio_buf_mgm_get_free_buffer(p_dsp_info, &p_source_buf, &buf_size);
        if (buf_size < (DONGLE_PAYLOAD_SIZE_PCM + BLK_HEADER_SIZE)) {
            TRANSMITTER_LOG_I("[USB_RX_DEBUG 0]Not enough share buffer space", 0);
            //configASSERT(0);
            break;
        }

        usb_stream_header.sequence_number++;
        usb_stream_header.data_length = DONGLE_PAYLOAD_SIZE_PCM;

        memcpy(p_source_buf, &usb_stream_header, BLK_HEADER_SIZE);
        USB_Audio_Read_Data(0, p_source_buf + BLK_HEADER_SIZE, DONGLE_PAYLOAD_SIZE_PCM);
        #ifdef MTK_AUDIO_DUMP_ENABLE
        //LOG_AUDIO_DUMP(p_source_buf + BLK_HEADER_SIZE, DONGLE_PAYLOAD_SIZE_PCM, SOURCE_IN4);
        #endif
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

        if (gaming_mode_latency_debug_0)
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
    uint32_t gpt_count, duration_count;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);

    n9_dsp_share_info_t *p_dsp_info = (n9_dsp_share_info_t *)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_RECEIVE_FROM_MCU_1);

    uint32_t buf_size = 0;
    uint8_t *p_source_buf = NULL;

    uint32_t available_data_size = USB_Audio_Get_Len_Received_Data(1);

    if (usb_first_in_flag_1 == 0) {
        USB_Audio_Rx_Buffer_Drop_Bytes(1, available_data_size);
        usb_first_in_flag_1 = 1;
        previous_count = gpt_count;
        return;
    }

    hal_gpt_get_duration_count(previous_count, gpt_count, &duration_count);
    if (duration_count > 2000) {
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 1]usb_audio_rx_cb_gaming_dongle_1 duration = %d", 1, duration_count);
    }
    previous_count = gpt_count;

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
        #ifdef MTK_AUDIO_DUMP_ENABLE
        //LOG_AUDIO_DUMP(p_source_buf + BLK_HEADER_SIZE, DONGLE_PAYLOAD_SIZE_PCM, SOURCE_IN5);
        #endif
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

        if (gaming_mode_latency_debug_1)
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

void gaming_mode_open_playback(audio_transmitter_config_t *config, mcu2dsp_open_param_t *open_param)
{
    memset(&usb_stream_header, 0, sizeof(audio_transmitter_block_header_t));
    if (config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET) {

        sysram_status_t     status;
        DSP_FEATURE_TYPE_LIST AudioFeatureList_GameVoiceHead[2] = {
            FUNC_GAMING_HEADSET,
            FUNC_END,
        };
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
        voice_mic_type_t mic_cur_type = hal_audio_query_voice_mic_type();
        TRANSMITTER_LOG_I("GamingVoiceHead mic_cur_type: 0x%x", 1, mic_cur_type);
        if(mic_cur_type == VOICE_MIC_TYPE_FIXED) {
            AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_HEADSET;
        } else if(mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) {
            AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_BOOM_MIC;
        } else {
            TRANSMITTER_LOG_E("GamingVoiceHead no this mic type - (%d)\r\n", 1, mic_cur_type);
            configASSERT(0);
        }
#endif

        /* reset share buffer before put parameters*/
        audio_nvdm_reset_sysram();
        status = audio_nvdm_set_feature(2, AudioFeatureList_GameVoiceHead);
        if (status != NVDM_STATUS_NAT_OK) {
            TRANSMITTER_LOG_E("GamingVoiceHead is failed to set parameters to share memory - err(%d)\r\n", 1, status);
            configASSERT(0);
        }

        open_param->param.stream_in = STREAM_IN_AFE;
        hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param->stream_in_param);
        //open_param->stream_in_param.afe.audio_device    = HAL_AUDIO_DEVICE_MAIN_MIC_L;
        //open_param->stream_in_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
        open_param->stream_in_param.afe.memory          = HAL_AUDIO_MEM1 | HAL_AUDIO_MEM3; //mic in & echo ref
        //open_param->stream_in_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
        open_param->stream_in_param.afe.format          = AFE_PCM_FORMAT_S16_LE;
        #ifdef AIR_UL_FIX_SAMPLING_RATE_48K
        open_param->stream_in_param.afe.sampling_rate   = 48000;
        #else
        open_param->stream_in_param.afe.sampling_rate   = 16000;
        #endif
#if defined(AIR_ULL_VOICE_LOW_LATENCY_ENABLE)
        open_param->stream_in_param.afe.irq_period      = 7.5;
        #ifdef AIR_UL_FIX_SAMPLING_RATE_48K
        open_param->stream_in_param.afe.frame_size      = 360;
        #else
        open_param->stream_in_param.afe.frame_size      = 120;
        #endif
#else
        open_param->stream_in_param.afe.irq_period      = 15;
        #ifdef AIR_UL_FIX_SAMPLING_RATE_48K
        open_param->stream_in_param.afe.frame_size      = 720;
        #else
        open_param->stream_in_param.afe.frame_size      = 240;
        #endif
#endif
        open_param->stream_in_param.afe.frame_number    = 6;
        open_param->stream_in_param.afe.hw_gain         = false;
        open_param->stream_in_param.afe.misc_parms      = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_2p4v;
        if(open_param->stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER){
            //for ULL UL Downsample
            open_param->stream_in_param.afe.with_upwdown_sampler     = true;
            open_param->stream_in_param.afe.audio_path_input_rate    = HAL_AUDIO_FIXED_AFE_SAMPLE_RATE;
            open_param->stream_in_param.afe.audio_path_output_rate   = open_param->stream_in_param.afe.sampling_rate;
        }

        open_param->param.stream_out = STREAM_OUT_AUDIO_TRANSMITTER;
        open_param->stream_out_param.data_ul.scenario_type = config->scenario_type;
        open_param->stream_out_param.data_ul.scenario_sub_id = config->scenario_sub_id;
        open_param->stream_out_param.data_ul.data_notification_frequency = 0;

        if ((config->scenario_config.gaming_mode_config.voice_headset_config.codec_param.opus.bit_rate != 32000) && (config->scenario_config.gaming_mode_config.voice_headset_config.codec_param.opus.bit_rate != 50133)) {
            TRANSMITTER_LOG_E("Error codec bitrate, %u\r\n", 1, config->scenario_config.gaming_mode_config.voice_headset_config.codec_param.opus.bit_rate);
            configASSERT(0);
        }
        if (config->scenario_config.gaming_mode_config.voice_headset_config.codec_param.opus.bit_rate != 32000) {
            open_param->stream_out_param.data_ul.max_payload_size = 52; // header:4 + payload:30
        } else {
            open_param->stream_out_param.data_ul.max_payload_size = 36; // header:4 + payload:47
        }
        open_param->stream_out_param.data_ul.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_BT_SEND_TO_AIR);
        //hal_audio_reset_share_info(open_param->stream_out_param.data_ul.p_share_info);
        open_param->stream_out_param.data_ul.p_share_info->read_offset = 0;
        open_param->stream_out_param.data_ul.p_share_info->write_offset = 0;
        open_param->stream_out_param.data_ul.p_share_info->bBufferIsFull = false;

        if (config->scenario_config.gaming_mode_config.voice_headset_config.codec_param.opus.bit_rate != 32000) {
            open_param->stream_out_param.data_ul.p_share_info->sub_info.block_size = 52; // header:4 + payload:47
            open_param->stream_out_param.data_ul.p_share_info->sub_info.block_num = open_param->stream_out_param.data_ul.p_share_info->length / 52;
        } else {
            open_param->stream_out_param.data_ul.p_share_info->sub_info.block_size = 36; // header:4 + payload:47
            open_param->stream_out_param.data_ul.p_share_info->sub_info.block_num = open_param->stream_out_param.data_ul.p_share_info->length / 36;
        }
        open_param->stream_out_param.data_ul.p_share_info->length = open_param->stream_out_param.data_ul.p_share_info->sub_info.block_size * open_param->stream_out_param.data_ul.p_share_info->sub_info.block_num;
        //memset((void *)open_param->stream_out_param.data_ul.p_share_info->start_addr, 0, open_param->stream_out_param.data_ul.p_share_info->length);

    } else if ((config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) || (config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1)) {
        #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
        if (config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)
        {
            /* prepare game chat balance feature's NVKEY */
            sysram_status_t status;
            nvdm_status_t nvdm_status;
            DSP_FEATURE_TYPE_LIST AudioFeatureList_GameChatBalance[] = {
                FUNC_GAME_CHAT_VOLUME_SMART_BALANCE,
                FUNC_END,
            };

            /* reset share buffer before put parameters*/
            audio_nvdm_reset_sysram();
            status = NVDM_STATUS_ERROR;
            while (status != NVDM_STATUS_NAT_OK)
            {
                /* set NVKEYs that the usb chat stream uses into the share buffer */
                status = audio_nvdm_set_feature(sizeof(AudioFeatureList_GameChatBalance)/sizeof(DSP_FEATURE_TYPE_LIST), AudioFeatureList_GameChatBalance);
                if (status != NVDM_STATUS_NAT_OK)
                {
                    audio_src_srv_report("[Game/Chat Balance] failed to set parameters to share memory - err(%d)\r\n", 1, status);

                    /* workaround for that maybe this NVKEY_F50C is not exsited after the FOTA */
                    if ((nvdm_status_t)status == NVDM_STATUS_ITEM_NOT_FOUND)
                    {
                        audio_src_srv_report("[Game/Chat Balance] set default parameters into the NVKEY\r\n", 0);
                        nvdm_status = nvdm_write_data_item("AB15", NVKEYID_F50C, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)NVKEY_F50C, sizeof(NVKEY_F50C));
                        if (nvdm_status != NVDM_STATUS_OK)
                        {
                            audio_src_srv_report("[Game/Chat Balance] failed to set default parameters into the NVKEY - err(%d)\r\n", 1, nvdm_status);
                            configASSERT(0);
                        }
                    }
                    else
                    {
                        configASSERT(0);
                    }
                }
            }
        }
        #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

        open_param->param.stream_in = STREAM_IN_AUDIO_TRANSMITTER;
        open_param->stream_in_param.data_dl.scenario_type = config->scenario_type;
        open_param->stream_in_param.data_dl.scenario_sub_id = config->scenario_sub_id;
        if(config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) {
            open_param->stream_in_param.data_dl.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_RECEIVE_FROM_MCU_0);
        } else {
            open_param->stream_in_param.data_dl.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_RECEIVE_FROM_MCU_1);
        }
        open_param->stream_in_param.data_dl.data_notification_frequency = 0;
        open_param->stream_in_param.data_dl.max_payload_size = DONGLE_PAYLOAD_SIZE_PCM;
        open_param->stream_in_param.data_dl.scenario_param.gaming_mode_param.period = 2500;
        open_param->stream_in_param.data_dl.scenario_param.gaming_mode_param.codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
        open_param->stream_in_param.data_dl.p_share_info->read_offset = 0;
        open_param->stream_in_param.data_dl.p_share_info->write_offset = 0;
        open_param->stream_in_param.data_dl.p_share_info->bBufferIsFull = false;
        audio_transmitter_modify_share_info_by_block(open_param->stream_in_param.data_dl.p_share_info, DONGLE_PAYLOAD_SIZE_PCM);
        usb_first_in_flag_0 = 0;
        usb_first_in_flag_1 = 0;

        uint32_t gain = audio_get_gain_out_in_dB(0, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
        uint32_t gain_default=gain;
        uint8_t volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_L;
        if(volume_level <= bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
            gain = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
        }
        open_param->stream_in_param.data_dl.scenario_param.gaming_mode_param.gain_default_L = gain;
        gain = gain_default;
        volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_R;
        if(volume_level <= bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
            gain = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
        }
        open_param->stream_in_param.data_dl.scenario_param.gaming_mode_param.gain_default_R = gain;

        /*data write by usb*/
        //write_info = open_param->stream_in_param.data_dl.p_share_info;
        open_param->param.stream_out = STREAM_OUT_BT_COMMON;
        open_param->stream_out_param.bt_common.scenario_type = config->scenario_type;
        open_param->stream_out_param.bt_common.scenario_sub_id = config->scenario_sub_id;
        open_param->stream_out_param.bt_common.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_BT_SEND_TO_AIR);
        open_param->stream_out_param.bt_common.share_info_type = SHARE_BUFFER_INFO_TYPE;
        open_param->stream_out_param.bt_common.data_notification_frequency = 1;
        open_param->stream_out_param.bt_common.max_payload_size = DONGLE_PAYLOAD_SIZE_ENCODED;
        open_param->stream_out_param.bt_common.scenario_param.gaming_mode_param.codec_type = AUDIO_DSP_CODEC_TYPE_OPUS;
        open_param->stream_out_param.bt_common.p_share_info->read_offset = 0;
        open_param->stream_out_param.bt_common.p_share_info->write_offset = 0;
        open_param->stream_out_param.bt_common.p_share_info->bBufferIsFull = false;
        audio_transmitter_modify_share_info_by_block(open_param->stream_out_param.bt_common.p_share_info, DONGLE_PAYLOAD_SIZE_ENCODED);

        /*share buffer modify by bt host*/
        //audio_transmitter_modify_share_info_by_block(open_param->stream_out_param.data_ul.p_share_info, 240);
        /*data read by bt controller*/
        //read_info = open_param->stream_out_param.data_ul.p_share_info;
    } else if (config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT) {
#ifdef AIR_ECNR_POST_PART_ENABLE
        sysram_status_t status;
        DSP_FEATURE_TYPE_LIST AudioFeatureList_GameModeUsbDongle[] = {
            FUNC_TX_POST_ECNR,
            FUNC_END,
        };
        audio_nvdm_reset_sysram();
        status = audio_nvdm_set_feature(sizeof(AudioFeatureList_GameModeUsbDongle)/sizeof(DSP_FEATURE_TYPE_LIST), AudioFeatureList_GameModeUsbDongle);
        if (status != NVDM_STATUS_NAT_OK) {
            audio_src_srv_report("[Game mode music dongle] failed to set parameters to share memory - err(%d)\r\n", 1, status);
            configASSERT(0);
        }
        audio_src_srv_report("[Game mode music dongle] load ecnr nvkey done", 0);
#endif

        open_param->param.stream_in = STREAM_IN_BT_COMMON;
        open_param->stream_in_param.bt_common.scenario_type = config->scenario_type;
        open_param->stream_in_param.bt_common.scenario_sub_id = config->scenario_sub_id;
        open_param->stream_in_param.bt_common.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_BT_RECEIVE_FROM_AIR);
        open_param->stream_in_param.bt_common.share_info_type = AVM_SHARE_BUF_INFO_TYPE;
        open_param->stream_in_param.bt_common.data_notification_frequency = 0;
        open_param->stream_in_param.bt_common.max_payload_size = 240;
        open_param->stream_in_param.bt_common.scenario_param.gaming_mode_param.period = 7500;
        open_param->stream_in_param.bt_common.scenario_param.gaming_mode_param.codec_type = AUDIO_DSP_CODEC_TYPE_OPUS;
        //open_param->stream_in_param.bt_common.scenario_param.gaming_mode_param.codec_param.opus.sample_rate = 0x8;
        if ((config->scenario_config.gaming_mode_config.voice_dongle_config.codec_param.opus.bit_rate != 32000) && (config->scenario_config.gaming_mode_config.voice_dongle_config.codec_param.opus.bit_rate != 50133))
        {
            TRANSMITTER_LOG_E("Error codec bitrate, %u\r\n", 1, config->scenario_config.gaming_mode_config.voice_dongle_config.codec_param.opus.bit_rate);
            configASSERT(0);
        }
        open_param->stream_in_param.bt_common.scenario_param.gaming_mode_param.codec_param.opus.bit_rate = config->scenario_config.gaming_mode_config.voice_dongle_config.codec_param.opus.bit_rate;
        //open_param->stream_in_param.bt_common.scenario_param.gaming_mode_param.codec_param.opus.bit_rate = 0x0;
        //open_param->stream_in_param.bt_common.scenario_param.gaming_mode_param.codec_param.opus.channel_mode = 0x8;
        /*share buffer modify by bt host, AVM buffer info*/
        //audio_transmitter_modify_share_info_by_block(open_param->stream_in_param.bt_common.p_share_info, 240);
        /*data write by bt controller*/
        //write_info = open_param->stream_in_param.bt_common.p_share_info;
        uint32_t gain = audio_get_gain_in_in_dB(0, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
        uint32_t gain_default=gain;
        uint8_t volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_L;
        if(volume_level <= bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
            gain = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
        }
        open_param->stream_in_param.bt_common.scenario_param.gaming_mode_param.gain_default_L = gain;
        gain = gain_default;
        volume_level = 0;//config->scenario_config.gaming_mode_config.voice_dongle_config.volume_level_default_R;
        if(volume_level <= bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
            gain = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
        }
        open_param->stream_in_param.bt_common.scenario_param.gaming_mode_param.gain_default_R = gain;

        open_param->param.stream_out = STREAM_OUT_AUDIO_TRANSMITTER;
        open_param->stream_out_param.data_ul.scenario_type = config->scenario_type;
        open_param->stream_out_param.data_ul.scenario_sub_id = config->scenario_sub_id;
        open_param->stream_out_param.data_ul.p_share_info = hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_SEND_TO_MCU);
        open_param->stream_out_param.data_ul.data_notification_frequency = 0;
        open_param->stream_out_param.data_ul.max_payload_size = 1440;
        open_param->stream_out_param.data_ul.scenario_param.gaming_mode_param.codec_type = AUDIO_DSP_CODEC_TYPE_PCM;
        if ((config->scenario_config.gaming_mode_config.voice_dongle_config.usb_param.pcm.sample_rate != 16000) && (config->scenario_config.gaming_mode_config.voice_dongle_config.usb_param.pcm.sample_rate != 48000))
        {
            TRANSMITTER_LOG_E("Error usb sample rate, %u\r\n", 1, config->scenario_config.gaming_mode_config.voice_dongle_config.usb_param.pcm.sample_rate);
            configASSERT(0);
        }
        open_param->stream_out_param.data_ul.scenario_param.gaming_mode_param.codec_param.pcm.sample_rate = config->scenario_config.gaming_mode_config.voice_dongle_config.usb_param.pcm.sample_rate;
        //hal_audio_reset_share_info(open_param->stream_out_param.data_ul.p_share_info);
        //memset((void *)open_param->stream_out_param.data_ul.p_share_info->start_addr, 0, open_param->stream_out_param.data_ul.p_share_info->length);
        open_param->stream_out_param.data_ul.p_share_info->read_offset = 0;
        open_param->stream_out_param.data_ul.p_share_info->write_offset = 0;
        open_param->stream_out_param.data_ul.p_share_info->bBufferIsFull = false;
        /*share buffer modify by transmitter, n9_dsp buffer info*/
        audio_transmitter_modify_share_info_by_block(open_param->stream_out_param.data_ul.p_share_info, open_param->stream_out_param.data_ul.max_payload_size);
        /*data read by bt bt host, read by transmitter read API*/
        //*read_info = open_param->stream_out_param.data_ul.p_share_info;

    } else {
        TRANSMITTER_LOG_E("not in gaming mode scenario sub id list\r\n", 0);
        configASSERT(0);
    }
}
void gaming_mode_start_playback(audio_transmitter_config_t *config, mcu2dsp_start_param_t *start_param)
{
    if (config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET) {
        start_param->param.stream_in = STREAM_IN_AFE;
        start_param->stream_in_param.afe.aws_flag = true;
    } else if ((config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) || (config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1)) {
        start_param->param.stream_in = STREAM_IN_AUDIO_TRANSMITTER;
    } else if (config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT) {
        start_param->param.stream_in = STREAM_IN_BT_COMMON;
    }
    start_param->param.stream_out = STREAM_OUT_AUDIO_TRANSMITTER;
}

typedef struct {
    uint32_t vol_gain_1;
    uint32_t vol_gain_2;
} vol_gain_t;
typedef struct {
    uint8_t scenario_type;
    uint8_t scenario_sub_id;
} dl_mixer_param_t;


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



audio_transmitter_status_t gaming_mode_set_runtime_config_playback(audio_transmitter_config_t *config, audio_transmitter_runtime_config_type_t runtime_config_type, audio_transmitter_runtime_config_t *runtime_config, mcu2dsp_audio_transmitter_runtime_config_param_t *runtime_config_param){
    uint32_t operation = runtime_config_type;//runtime_config->gaming_mode_runtime_config.gaming_mode_runtime_config_operation;
    if(config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT){
        if((operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_L) || (operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_R)
        || (operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_DUL)){
            vol_gain_t gain;
            uint8_t volume_level;
            if((operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_L)|| (operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_DUL)){
                volume_level = runtime_config->gaming_mode_runtime_config.vol_level.vol_level_l;
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_1 = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
            }
            if((operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_R)|| (operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_DUL)){
                volume_level = runtime_config->gaming_mode_runtime_config.vol_level.vol_level_r;
                if(volume_level > bt_sink_srv_ami_get_usb_voice_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_2 = audio_get_gain_in_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_VOICE_SW_OUT);
            }
            runtime_config_param->config_operation = operation;
            memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
            TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d.",
                                6 ,config->scenario_sub_id, operation, runtime_config->gaming_mode_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->gaming_mode_runtime_config.vol_level.vol_level_r, gain.vol_gain_2);
            return AUDIO_TRANSMITTER_STATUS_SUCCESS;

        }
        else if(operation == GAMING_MODE_CONFIG_OP_LATENCY_SWITCH){
            uint32_t latency_us = runtime_config->gaming_mode_runtime_config.latency_us;
            TRANSMITTER_LOG_I("scenario_sub_id =%d: latency switch %d.", 2 ,config->scenario_sub_id, latency_us);
            runtime_config_param->config_operation = operation;
            memcpy(runtime_config_param->config_param, &latency_us, sizeof(uint32_t));
            return AUDIO_TRANSMITTER_STATUS_SUCCESS;
        }
        else{
            TRANSMITTER_LOG_E("operation %d can not do in GAMING_MODE_VOICE_DONGLE_USB_OUT. ",1,operation);
            configASSERT(0);
        }
    }
    else if((config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) || (config->scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1)){
        if(operation == GAMING_MODE_CONFIG_OP_MUSIC_MIX) {
            TRANSMITTER_LOG_I("scenario_sub_id =%d: MUSIC_MIX id %d.",2 ,config->scenario_sub_id, runtime_config->gaming_mode_runtime_config.dl_mixer_id);
            runtime_config_param->config_operation = operation;
            dl_mixer_param_t dl_mixer;
            dl_mixer.scenario_type = g_audio_transmitter_control[runtime_config->gaming_mode_runtime_config.dl_mixer_id].config.scenario_type;
            dl_mixer.scenario_sub_id = g_audio_transmitter_control[runtime_config->gaming_mode_runtime_config.dl_mixer_id].config.scenario_sub_id;
            memcpy(runtime_config_param->config_param, &dl_mixer, sizeof(dl_mixer_param_t));
            return AUDIO_TRANSMITTER_STATUS_SUCCESS;
        }
        else if(operation == GAMING_MODE_CONFIG_OP_MUSIC_UNMIX){
            TRANSMITTER_LOG_I("scenario_sub_id =%d: MUSIC_UNMIX.", 1 ,config->scenario_sub_id);
            runtime_config_param->config_operation = operation;
            return AUDIO_TRANSMITTER_STATUS_SUCCESS;
        }
        else if((operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_L) || (operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_R)
        || (operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL)){
            vol_gain_t gain;
            uint8_t volume_level;
            uint8_t volume_ratio;
            volume_ratio = runtime_config->gaming_mode_runtime_config.vol_level.vol_ratio;

            if(volume_ratio > 100) {
                TRANSMITTER_LOG_E("Volume ratio should between 0 and 100, volume_ratio = \r\n", 1,volume_ratio);
                volume_ratio = 100;
            }

            if((operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_L)|| (operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL)){
                volume_level = runtime_config->gaming_mode_runtime_config.vol_level.vol_level_l;
                if(volume_level > bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_music_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_1 = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
                gain.vol_gain_1 += gain_compensation_table[volume_ratio/GAIN_COMPENSATION_STEP];
            }
            if((operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_R)|| (operation == GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL)){
                volume_level = runtime_config->gaming_mode_runtime_config.vol_level.vol_level_r;
                if(volume_level > bt_sink_srv_ami_get_usb_music_sw_max_volume_level()){
                    volume_level = bt_sink_srv_ami_get_usb_music_sw_max_volume_level();
                    TRANSMITTER_LOG_E("set L volume %d level more than max level %d\r\n", 2,volume_level,bt_sink_srv_ami_get_usb_music_sw_max_volume_level());
                }
                gain.vol_gain_2 = audio_get_gain_out_in_dB(volume_level, GAIN_DIGITAL, VOL_USB_AUDIO_SW_IN);
                gain.vol_gain_2 += gain_compensation_table[volume_ratio/GAIN_COMPENSATION_STEP];
            }
            runtime_config_param->config_operation = operation;
            memcpy(runtime_config_param->config_param, &gain, sizeof(vol_gain_t));
            TRANSMITTER_LOG_I("scenario_sub_id =%d: operation %d L:volume level %d gain=%d R:volume level %d gain=%d volume_ratio = %d.",
                                7 ,config->scenario_sub_id, operation, runtime_config->gaming_mode_runtime_config.vol_level.vol_level_l,gain.vol_gain_1,runtime_config->gaming_mode_runtime_config.vol_level.vol_level_r, gain.vol_gain_2, volume_ratio);
            return AUDIO_TRANSMITTER_STATUS_SUCCESS;
        }
        else{
            TRANSMITTER_LOG_E("operation %d can not do in GAMING_MODE_MUSIC_DONGLE_USB_IN. ",1,operation);
            configASSERT(0);
        }
    }
    return AUDIO_TRANSMITTER_STATUS_FAIL;
}

audio_transmitter_status_t gaming_mode_get_runtime_config(uint8_t scenario_type, uint8_t scenario_sub_id, audio_transmitter_runtime_config_type_t runtime_config_type, audio_transmitter_runtime_config_t *runtime_config){
    if(scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT){
        return AUDIO_TRANSMITTER_STATUS_SUCCESS;
    }
    return AUDIO_TRANSMITTER_STATUS_FAIL;
}

void gaming_mode_state_started_handler(uint8_t scenario_sub_id)
{
    if (scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) {
        usb_first_in_flag_0 = 0;
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
        USB_Audio_Register_Rx_Callback(0, usb_audio_rx_cb_gaming_dongle_0);
#endif
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 0]register usb_audio_rx_cb 0", 0);
    } else if (scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1) {
        usb_first_in_flag_1 = 0;
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
        USB_Audio_Register_Rx_Callback(1, usb_audio_rx_cb_gaming_dongle_1);
#endif
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 1]register usb_audio_rx_cb 1", 0);
    }
}
void gaming_mode_state_idle_handler(uint8_t scenario_sub_id)
{
    if (scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) {
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
        USB_Audio_Register_Rx_Callback(0, NULL);
#endif
        usb_first_in_flag_0 = 0;
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 0]Unregister usb_audio_rx_cb 0", 0);
    } else if (scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1) {
#if defined(MTK_USB_DEMO_ENABLED) && (defined(MTK_USB_AUDIO_V1_ENABLE) || defined(MTK_USB_AUDIO_V2_ENABLE))
        USB_Audio_Register_Rx_Callback(1, NULL);
#endif
        usb_first_in_flag_1 = 0;
        TRANSMITTER_LOG_I("[USB_RX_DEBUG 1]Unregister usb_audio_rx_cb 1", 0);
    }
}

void gaming_mode_state_starting_handler(uint8_t scenario_sub_id)
{
    if (scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET) {
        #ifndef AIR_ECNR_PREV_PART_ENABLE
        #if defined(MTK_3RD_PARTY_NR)
        if(g_audio_transmitter_dvfs_clk < HAL_DVFS_HIGH_SPEED_208M) {
            g_audio_transmitter_dvfs_clk = HAL_DVFS_HIGH_SPEED_208M;
        }
        hal_dvfs_lock_control(g_audio_transmitter_dvfs_clk, HAL_DVFS_LOCK);
        TRANSMITTER_LOG_I("GamingVoiceHead lock dvfs: 0x%x", 1, HAL_DVFS_HIGH_SPEED_208M);
        #endif /*defined(MTK_3RD_PARTY_NR)*/
        #endif /*defined(AIR_ECNR_PREV_PART_ENABLE)*/
    }
}

audio_transmitter_status_t gaming_mode_stop_handler(uint16_t scenario_sub_id)
{
    if(scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
    {
        if(g_audio_transmitter_dvfs_clk != HAL_DVFS_HALF_SPEED_52M)
        {
            hal_dvfs_lock_control(g_audio_transmitter_dvfs_clk, HAL_DVFS_UNLOCK);
            TRANSMITTER_LOG_I("[audio_transmitter][stop] dvfs unlock from:%d", 1, g_audio_transmitter_dvfs_clk);
        }
        return AUDIO_TRANSMITTER_STATUS_SUCCESS;
    } else {
        return AUDIO_TRANSMITTER_STATUS_FAIL;
    }
}

void gaming_mode_state_stoping_handler(uint8_t scenario_sub_id)
{
    if (scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET) {
        #ifndef AIR_ECNR_PREV_PART_ENABLE
        #if defined(MTK_3RD_PARTY_NR)
        if(g_audio_transmitter_dvfs_clk != HAL_DVFS_HALF_SPEED_52M)
        {
            hal_dvfs_lock_control(g_audio_transmitter_dvfs_clk, HAL_DVFS_UNLOCK);
            TRANSMITTER_LOG_I("GamingVoiceHead dvfs unlock from:%d", 1, g_audio_transmitter_dvfs_clk);
        }
        #endif /*defined(MTK_3RD_PARTY_NR)*/
        #endif /*defined(AIR_ECNR_PREV_PART_ENABLE)*/
    }
}

#endif

