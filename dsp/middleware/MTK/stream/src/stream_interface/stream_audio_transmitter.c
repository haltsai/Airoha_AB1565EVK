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

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE

/* Includes ------------------------------------------------------------------*/
#include "types.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "dsp_callback.h"
#include "dsp_temp.h"
#include "dsp_dump.h"
#include "dsp_scenario.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_process.h"
#include "stream_audio_transmitter.h"
#include "bt_types.h"
#include "hal_gpt.h"
#include "hal_sleep_manager.h"
#include "memory_attribute.h"
#if defined(AIR_GAMING_MODE_DONGLE_ENABLE) || defined(MTK_GAMING_MODE_HEADSET)
#include "scenario_ull_audio.h"
#include "hal_audio.h"
#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
#ifdef AIR_SOFTWARE_SRC_ENABLE
#include "sw_src_interface.h"
#endif
#ifdef AIR_WIRED_AUDIO_ENABLE
#include "scenario_wired_audio.h"
#ifdef AIR_SOFTWARE_GAIN_ENABLE
sw_gain_port_t * g_linein_sw_gain_port = NULL;
#endif
#ifdef AIR_SOFTWARE_SRC_ENABLE
sw_src_port_t * g_usb_out_src_port = NULL;
#endif
#endif
#ifdef AIR_SOFTWARE_GAIN_ENABLE
#include "sw_gain_interface.h"
#endif

#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
#include "scenario_advanced_passthrough.h"
#endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
#include "scenario_ble_audio.h"
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

#ifdef AIR_FIXED_RATIO_SRC
#include "src_fixed_ratio_interface.h"
src_fixed_ratio_port_t *g_adaptive_anc_smp_port;
src_fixed_ratio_port_t *ull_ul_smp_port;
#endif

#ifdef AIR_CELT_ENC_ENABLE
#include "celt_enc_interface.h"
#endif /* AIR_CELT_ENC_ENABLE */

#ifdef AIR_GAMING_MODE_HEADSET_ECNR_ENABLE
#include "aec_nr_interface.h"
#endif

/* Private define ------------------------------------------------------------*/
#define HW_SEMAPHORE_AUDIO_TRANSMITER_RETRY_TIME 50000

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
ATTR_ZIDATA_IN_DRAM static f_audio_transmitter_ccni_callback_t audio_transmitter_ccni_callback;

/* Public variables ----------------------------------------------------------*/
/* Functions -----------------------------------------------------------------*/
void audio_transmitter_register_isr_handler(f_audio_transmitter_ccni_callback_t callback)
{
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if ((audio_transmitter_ccni_callback == NULL) || (audio_transmitter_ccni_callback == callback))
    {
        audio_transmitter_ccni_callback = callback;
    }
    else
    {
        DSP_MW_LOG_E("[audio transmitter][source]ccni callback is used\r\n", 0);
        configASSERT(0);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
}

void audio_transmitter_unregister_isr_handler(f_audio_transmitter_ccni_callback_t callback)
{
    uint32_t saved_mask;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (audio_transmitter_ccni_callback == callback)
    {
        audio_transmitter_ccni_callback = NULL;
    }

    hal_nvic_restore_interrupt_mask(saved_mask);
}

void port_audio_transmitter_configure_task(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, SOURCE source, SINK sink)
{
    if (0) {
        UNUSED(source);
        UNUSED(sink);
        UNUSED(scenario_id);
        UNUSED(sub_id);
#if defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
    } else if (scenario_id == AUDIO_TRANSMITTER_ANC_MONITOR_STREAM){
        source->taskId = DPR_TASK_ID;
#endif
#if defined(AIR_WIRED_AUDIO_ENABLE)
    } else if ((scenario_id == AUDIO_TRANSMITTER_WIRED_AUDIO) &&
                  ((sub_id.wiredaudio_id==AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) || (sub_id.wiredaudio_id==AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))) {
        source->taskId = DHP_TASK_ID;
#endif
    }

}

void port_audio_transmitter_open(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, mcu2dsp_open_param_p open_param, SOURCE source, SINK sink)
{
    UNUSED(scenario_id);
    UNUSED(sub_id);
    UNUSED(open_param);
    UNUSED(source);
    UNUSED(sink);

    port_audio_transmitter_configure_task(scenario_id, sub_id, source, sink);
    DSP_MW_LOG_I("[audio transmitter][task_config]source taskId: 0x%x, sink taskId:0x%x \r\n", 2, source->taskId, sink->taskid);

    switch(scenario_id)
    {
        #if defined(MTK_GAMING_MODE_HEADSET)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            #if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
                if (sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
                {
                    src_fixed_ratio_config_t ull_ul_smp_config;
                    ull_ul_smp_config.channel_number = 2;
                    ull_ul_smp_config.in_sampling_rate = 48000;
                    ull_ul_smp_config.out_sampling_rate = 16000;
                    ull_ul_smp_config.resolution = RESOLUTION_16BIT;
                    ull_ul_smp_config.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE;
                    ull_ul_smp_config.cvt_num = 1;
                    ull_ul_smp_port = stream_function_src_fixed_ratio_get_port(source);
                    stream_function_src_fixed_ratio_init(ull_ul_smp_port, &ull_ul_smp_config);
                }
            #endif
            #ifdef AIR_CELT_ENC_ENABLE
                if (sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
                {
                    if (sink->param.data_ul.max_payload_size ==  36)
                    {
                        stream_codec_encoder_celt_set_input_frame_size(30);
                        DSP_MW_LOG_I("[audio transmitter][gaming_headset ul] codec frame size, %u\r\n", 1, 30);
                    }
                    else if (sink->param.data_ul.max_payload_size == 52)
                    {
                        stream_codec_encoder_celt_set_input_frame_size(47);
                        DSP_MW_LOG_I("[audio transmitter][gaming_headset ul] codec frame size, %u\r\n", 1, 47);
                    }
                    else
                    {
                        DSP_MW_LOG_E("[audio transmitter][gaming_headset ul] error frame size, %u\r\n", 0);
                        configASSERT(0);
                    }
                }
            #endif
                break;
        #endif
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            if ((sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)||(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                gaming_mode_dongle_dl_init(source, &(open_param->stream_in_param.data_dl));
            }
            else if (sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                gaming_mode_dongle_ul_init(source, sink, &(open_param->stream_out_param.data_ul), &(open_param->stream_in_param.bt_dl));
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_WIRED_AUDIO_ENABLE)
        case AUDIO_TRANSMITTER_WIRED_AUDIO:
            if((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)||(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
            {
                if(open_param->param.stream_out == STREAM_OUT_AFE){
                    source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 = true;
                }
                else{
                    source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 = false;
                }
                #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
                wired_audio_usb_dl_vol_balance_init(source, &(open_param->stream_in_param.data_dl));
                #endif
            }
            break;
        #endif /* AIR_WIRED_AUDIO_ENABLE */

        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        case AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH:
            if(sub_id.advanced_passthrough_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID)
            {
                advanced_passthrough_hearing_aid_features_init(source, sink);
            }
            break;
        #endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            if ((sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0)||(sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ble_audio_dongle_dl_init(source, sink, &(open_param->stream_in_param.data_dl), &(open_param->stream_out_param.bt_ul));
            }
            else if (sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                ble_audio_dongle_ul_init(source, sink, &(open_param->stream_out_param.data_ul), &(open_param->stream_in_param.bt_dl));
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        #if defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_NOISE_GATE_ENABLE)
        case AUDIO_TRANSMITTER_ANC_MONITOR_STREAM:{
                #if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
                src_fixed_ratio_config_t smp_config;
                smp_config.channel_number = 2;
                smp_config.in_sampling_rate = 48000;
                smp_config.out_sampling_rate = 16000;
                smp_config.resolution = RESOLUTION_16BIT;
                smp_config.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE;
                smp_config.cvt_num = 1;
                g_adaptive_anc_smp_port = stream_function_src_fixed_ratio_get_port(source);
                stream_function_src_fixed_ratio_init(g_adaptive_anc_smp_port, &smp_config);
                #endif
            }
            break;
        #endif

        default:
            break;
    }
}

void port_audio_transmitter_start(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, mcu2dsp_start_param_p start_param, SOURCE source, SINK sink)
{
    UNUSED(scenario_id);
    UNUSED(sub_id);
    UNUSED(start_param);
    UNUSED(source);
    UNUSED(sink);

    switch(scenario_id)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            if (sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                gaming_mode_dongle_ul_start(source);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(MTK_GAMING_MODE_HEADSET)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            if(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
            {
                hal_audio_trigger_start_parameter_t start_parameter;
                start_parameter.memory_select = sink->transform->source->param.audio.mem_handle.memory_select;
                start_parameter.enable = true;
                U32 now_gpt_time;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &now_gpt_time);
                DSP_MW_LOG_I("[ULL] UL SET TRIGGER MEM:0x%x audio.rate %d audio.count %d, time:%d", 4, start_parameter.memory_select, sink->transform->source->param.audio.rate, sink->transform->source->param.audio.count, now_gpt_time);
                hal_audio_set_value((hal_audio_set_value_parameter_t *)&start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
            }
            break;
        #endif /* MTK_GAMING_MODE_HEADSET */

        #if defined(AIR_WIRED_AUDIO_ENABLE)
        case AUDIO_TRANSMITTER_WIRED_AUDIO:
            if((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)||(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
            {
                wired_audio_add_usb_audio_start_number();
            } else if((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)) {
                #ifdef AIR_SOFTWARE_GAIN_ENABLE
                //Get SW gain port during transmitter start ccni msg, then init SW gain
                if(g_linein_sw_gain_port == NULL) {

                    g_linein_sw_gain_port = stream_function_sw_gain_get_port(source);

                    /* sw gain config */
                    //int32_t default_gain;
                    sw_gain_config_t default_config;
                    default_config.resolution = RESOLUTION_16BIT;
                    //default_config.target_gain = open_param->scenario_param.gaming_mode_param.gain_default_L / 25;
                    default_config.target_gain = 0;
                    default_config.up_step = 1;
                    default_config.up_samples_per_step = 2;
                    default_config.down_step = -1;
                    default_config.down_samples_per_step = 2;

                    DSP_STREAMING_PARA_PTR ul_stream = DSP_Streaming_Get(source, sink);
                    uint32_t channel_number = stream_function_get_channel_number(&(ul_stream->callback.EntryPara));

                    DSP_MW_LOG_I("[audio_transmitter][SW_GAIN] init, channel_number = %d", 1, channel_number);
                    stream_function_sw_gain_init(g_linein_sw_gain_port, channel_number, &default_config);

                    source->param.audio.lineout_scenario_flag = 1;
                }
                #endif /* AIR_SOFTWARE_GAIN_ENABLE */

                #ifdef AIR_SOFTWARE_SRC_ENABLE
                //Get SW SRC port during transmitter start ccni msg, then init SW SRC
                if((g_usb_out_src_port == NULL) && (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT)) {

                DSP_MW_LOG_E("[audio_transmitter][SW_SRC] init", 0);
                g_usb_out_src_port = stream_function_sw_src_get_port(source);

                sw_src_config_t sw_src_config;
                sw_src_config.mode = SW_SRC_MODE_NORMAL;
                sw_src_config.channel_num = 1;
                sw_src_config.in_res = RESOLUTION_16BIT;
                sw_src_config.in_sampling_rate  = 16000;
                sw_src_config.in_frame_size_max = 240*sizeof(int16_t);
                sw_src_config.out_res           = RESOLUTION_16BIT;
                sw_src_config.out_sampling_rate = sink->param.data_ul.scenario_param.wired_audio_param.codec_param.pcm.sample_rate;
                sw_src_config.out_frame_size_max= sink->param.data_ul.scenario_param.wired_audio_param.codec_param.pcm.sample_rate/16000*240*sizeof(int16_t);

                DSP_MW_LOG_I("audio_transmitter][SW_SRC][scenario %d-%d] %u, %u, %u, %u, %u, %u\r\n", 8,
                sink->param.data_ul.scenario_type,
                sink->param.data_ul.scenario_sub_id,
                sw_src_config.in_res,
                sw_src_config.in_sampling_rate,
                sw_src_config.in_frame_size_max,
                sw_src_config.out_res,
                sw_src_config.out_sampling_rate,
                sw_src_config.out_frame_size_max);

                stream_function_sw_src_init(g_usb_out_src_port, &sw_src_config);

               }
               #endif
            }
            break;
        #endif /* AIR_WIRED_AUDIO_ENABLE */

        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        case AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH:
            if(sub_id.advanced_passthrough_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID)
            {
                advanced_passthrough_hearing_aid_trigger_stream_start(source, sink);
            }
            break;
        #endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            if (sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                ble_audio_dongle_ul_start(source);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            break;
    }
}

void port_audio_transmitter_stop(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, SOURCE source, SINK sink)
{
    UNUSED(scenario_id);
    UNUSED(sub_id);
    UNUSED(source);
    UNUSED(sink);

    switch(scenario_id)
    {
        default:
            break;
    }
}

void port_audio_transmitter_close(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, SOURCE source, SINK sink)
{
    UNUSED(scenario_id);
    UNUSED(sub_id);
    UNUSED(source);
    UNUSED(sink);

    switch(scenario_id)
    {
        #if defined(MTK_GAMING_MODE_HEADSET)
        #if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            if  (sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
                if (ull_ul_smp_port) {
                stream_function_src_fixed_ratio_deinit(ull_ul_smp_port);
                ull_ul_smp_port = NULL;
            }
            break;
        #endif
        #endif
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            if((sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0)||(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                gaming_mode_dongle_dl_deinit(source, sink);
            }
            else if (sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                gaming_mode_dongle_ul_deinit(source, sink);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_WIRED_AUDIO_ENABLE)
        case AUDIO_TRANSMITTER_WIRED_AUDIO:
            if((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)||(sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
            {
                #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
                wired_audio_usb_dl_vol_balance_deinit(source, sink);
                #endif
            } else if((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)) {
                #ifdef AIR_SOFTWARE_GAIN_ENABLE
                if(g_linein_sw_gain_port != NULL) {//deinit SW gain port in transmitter close flow
                    DSP_MW_LOG_I("[audio_transmitter][SW_GAIN] deinit", 0);
                    stream_function_sw_gain_deinit(g_linein_sw_gain_port);
                    g_linein_sw_gain_port = NULL;
                }
                #endif

                #ifdef AIR_SOFTWARE_SRC_ENABLE
                if((g_usb_out_src_port != NULL) && (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT)) {
                    DSP_MW_LOG_E("[audio_transmitter][SW_SRC] deinit", 0);
                    stream_function_sw_src_deinit(g_usb_out_src_port);
                    g_usb_out_src_port = NULL;
                }
                #endif
            }
            break;
        #endif /* AIR_WIRED_AUDIO_ENABLE */

        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        case AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH:
            if(sub_id.advanced_passthrough_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID)
            {
                advanced_passthrough_hearing_aid_features_deinit(source, sink);
            }
            break;
        #endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            if ((sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0)||(sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ble_audio_dongle_dl_deinit(source, sink);
            }
            else if (sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                ble_audio_dongle_ul_deinit(source, sink);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */


        #if defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_NOISE_GATE_ENABLE)
        case AUDIO_TRANSMITTER_ANC_MONITOR_STREAM:
            #if defined(AIR_UL_FIX_SAMPLING_RATE_48K) & defined(AIR_FIXED_RATIO_SRC)
            if (g_adaptive_anc_smp_port) {
                stream_function_src_fixed_ratio_deinit(g_adaptive_anc_smp_port);
                g_adaptive_anc_smp_port = NULL;
            }
            #endif
            break;
        #endif

        default:
            break;
    }
}

/**
 * @brief This function is the ccni callback for triggering stream handling flow.
 *
 * @param event is ccni event.
 * @param msg is the ccni msg.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 void audio_transmitter_source_ccni_handler(hal_ccni_event_t event, void * msg)
{
    hal_ccni_status_t status;
    UNUSED(event);
    UNUSED(msg);

    status = hal_ccni_mask_event(event);
    if(status != HAL_CCNI_STATUS_OK)
        DSP_MW_LOG_E("[audio transmitter][source]ccni mask event: 0x%x something wrong, return is %d\r\n", 2, event, status);

    /* run registered callback */
    if (audio_transmitter_ccni_callback != NULL)
    {
        audio_transmitter_ccni_callback(event, msg);
    }
    else
    {
        DSP_MW_LOG_E("[audio transmitter][source]ccni callback is NULL\r\n", 0);
    }

    status = hal_ccni_clear_event(event);
    if(status != HAL_CCNI_STATUS_OK)
        DSP_MW_LOG_E("CCNI clear event something wrong, return is %d\r\n", 1, status);

    status = hal_ccni_unmask_event(event);
    if(status != HAL_CCNI_STATUS_OK)
        DSP_MW_LOG_E("CM4 CCNI unmask event: 0x%x something wrong, return is %d\r\n", 2, event, status);
}

ATTR_TEXT_IN_IRAM_LEVEL_1 static void hw_semaphore_take_audio_transmitter(uint32_t *hw_semaphore_irq_mask)
{
    uint32_t take_times;

    /* Add hw semaphore to avoid multi-core access */
    take_times = 0;
    while (++take_times) {
        hal_nvic_save_and_set_interrupt_mask(hw_semaphore_irq_mask);
        if (HAL_HW_SEMAPHORE_STATUS_OK == hal_hw_semaphore_take(HW_SEMAPHORE_AUDIO_CM4_DSP0_PLAYBACK)) {
            break;
        }

        if(take_times > HW_SEMAPHORE_AUDIO_TRANSMITER_RETRY_TIME) {
            hal_nvic_restore_interrupt_mask(*hw_semaphore_irq_mask);
            //error handling
            // printf("%s : Can not take HW Semaphore\r\n", __func__);
            configASSERT(0);
        }
        hal_nvic_restore_interrupt_mask(*hw_semaphore_irq_mask);
        // vTaskDelay(10/portTICK_PERIOD_MS);
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_1 static void hw_semaphore_give_audio_transmitter(uint32_t *hw_semaphore_irq_mask)
{
    // uint32_t int_mask;
    if ( HAL_HW_SEMAPHORE_STATUS_OK == hal_hw_semaphore_give(HW_SEMAPHORE_AUDIO_CM4_DSP0_PLAYBACK) ) {
        hal_nvic_restore_interrupt_mask(*hw_semaphore_irq_mask);
    } else {
        hal_nvic_restore_interrupt_mask(*hw_semaphore_irq_mask);

        //error handling
        // printf("%s : Can not give HW Semaphore\r\n", __func__);
        configASSERT(0);
    }
}

/**
 * @brief This function is used to reset the read offset and write offest of the share buffer.
 *
 * @param source is the instance whose the share buffer is reset. It can be NULL.
 * @param sink is the instance whose the share buffer is reset. It can be NULL.
 * @return VOID
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID audio_transmitter_share_information_reset_read_write_offset(SOURCE source, SINK sink)
{
    uint32_t hw_semaphore_irq_mask;

    hw_semaphore_take_audio_transmitter(&hw_semaphore_irq_mask);

    if(source != NULL)
    {
        source->param.data_dl.share_info_base_addr->WriteOffset = 0;
        source->param.data_dl.share_info_base_addr->ReadOffset = 0;
        source->param.data_dl.share_info_base_addr->bBufferIsFull = false;
    }

    if(sink != NULL)
    {
        sink->param.data_ul.share_info_base_addr->WriteOffset = 0;
        sink->param.data_ul.share_info_base_addr->ReadOffset = 0;
        sink->param.data_ul.share_info_base_addr->bBufferIsFull = false;
    }

    hw_semaphore_give_audio_transmitter(&hw_semaphore_irq_mask);
}

/**
 * @brief This function is used to fetch the share buffer information into source or sink.
 *
 * @param source is the instance whose the share buffer info is fetched. It can be NULL.
 * @param sink is the instance whose the share buffer info is fetched. It can be NULL.
 * @return VOID
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID audio_transmitter_share_information_fetch(SOURCE source, SINK sink)
{
    uint32_t hw_semaphore_irq_mask;
    uint32_t i;

    hw_semaphore_take_audio_transmitter(&hw_semaphore_irq_mask);

    if(source != NULL)
    {
        for (i = 0; i < sizeof(SHARE_BUFFER_INFO)/4; i++)
        {
            /* do not warry memory corruption here, becasue sizeof(SHARE_BUFFER_INFO) must be 4*n */
            *((uint32_t *)(&(source->streamBuffer.ShareBufferInfo)) + i) = *((uint32_t *)(source->param.data_dl.share_info_base_addr) + i);
        }
        source->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(source->streamBuffer.ShareBufferInfo.startaddr);
    }

    if(sink != NULL)
    {
        for (i = 0; i < sizeof(SHARE_BUFFER_INFO)/4; i++)
        {
            /* do not warry memory corruption here, becasue sizeof(SHARE_BUFFER_INFO) must be 4*n */
            *((uint32_t *)(&(sink->streamBuffer.ShareBufferInfo)) + i) = *((uint32_t *)(sink->param.data_ul.share_info_base_addr) + i);
        }
        sink->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.ShareBufferInfo.startaddr);
    }

    hw_semaphore_give_audio_transmitter(&hw_semaphore_irq_mask);
}

/**
 * @brief This function is used to update the write offset of the sink's share buffer.
 *
 * @param sink is the instance whose write offset is updated.
 * @param WriteOffset is the new write offset value.
 * @return VOID
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID audio_transmitter_share_information_update_write_offset(SINK sink, U32 WriteOffset)
{
    uint32_t hw_semaphore_irq_mask;

    hw_semaphore_take_audio_transmitter(&hw_semaphore_irq_mask);

    sink->param.data_ul.share_info_base_addr->WriteOffset = WriteOffset;
    if (WriteOffset == sink->param.data_ul.share_info_base_addr->ReadOffset) {
        sink->param.data_ul.share_info_base_addr->bBufferIsFull = 1;
    }

    hw_semaphore_give_audio_transmitter(&hw_semaphore_irq_mask);
}

/**
 * @brief This function is used to update the read offset of the source's share buffer.
 *
 * @param source is the instance whose write offset is updated.
 * @param ReadOffset is the new read offset value.
 * @return VOID
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID audio_transmitter_share_information_update_read_offset(SOURCE source, U32 ReadOffset)
{
    uint32_t hw_semaphore_irq_mask;

    hw_semaphore_take_audio_transmitter(&hw_semaphore_irq_mask);

    source->param.data_dl.share_info_base_addr->ReadOffset = ReadOffset;
    if (source->param.data_dl.share_info_base_addr->bBufferIsFull == 1)
    {
        source->param.data_dl.share_info_base_addr->bBufferIsFull = 0;
    }

    hw_semaphore_give_audio_transmitter(&hw_semaphore_irq_mask);
}

/**
 * @brief This function is used in audio_transmitter_config.
 *
 * @param scenario_id is the scenario id.
 * @param sub_id is the scenario sub id.
 * @param config_param is the confiuration parameters.
 * @return true means the confiuration is done successfully.
 * @return false means the confiuration is done unsuccessfully.
 */
bool port_audio_transmitter_scenario_config(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id, void *config_param)
{
    CONNECTION_IF *application_ptr = NULL;

    UNUSED(config_param);

    extern CONNECTION_IF * port_audio_transmitter_get_connection_if(audio_transmitter_scenario_t scenario_id, audio_transmitter_scenario_sub_id_t sub_id);
    application_ptr = port_audio_transmitter_get_connection_if(scenario_id, sub_id);
    if (application_ptr == NULL)
    {
        DSP_MW_LOG_E("[audio transmitter][config]application_ptr is NULL\r\n", 0);
        return false;
    }
#if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    if (scenario_id == AUDIO_TRANSMITTER_GAMING_MODE)
    {
        if ((sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
            (sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
        {
            SourceConfigure(application_ptr->source, AUDIO_SOURCE_MISC_PARMS, (uint32_t)config_param);
        }
        else if(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
        {
            SourceConfigure(application_ptr->source, AUDIO_SOURCE_MISC_PARMS, (uint32_t)config_param);
        }else if(sub_id.gamingmode_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
        {

        }
    }
#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
#if defined(AIR_WIRED_AUDIO_ENABLE)
    else if (scenario_id == AUDIO_TRANSMITTER_WIRED_AUDIO){
        if((sub_id.test_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) || (sub_id.test_id == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || (sub_id.test_id == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)){ 
            DSP_MW_LOG_I("[wired_audio][config]config_param addr= 0x%08x\r\n", 1, (uint32_t)config_param);
            wired_audio_runtime_config_param_p p_config_param = (wired_audio_runtime_config_param_p)config_param;
            if(application_ptr->source == NULL) {
                DSP_MW_LOG_I("[wired_audio][config]Source is NULL for AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT/AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT/AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER\r\n", 0);
                return true;
            }
            if(p_config_param->config_operation == WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_DUL)
            {
                #ifdef AIR_SOFTWARE_GAIN_ENABLE
                int32_t new_gain;
                sw_gain_port_t *port;
                sw_gain_config_t old_config;
                port = stream_function_sw_gain_get_port(application_ptr->source);
                /* left channel */
                new_gain = p_config_param->vol_level.gain_1;
                stream_function_sw_gain_get_config(port, 1, &old_config);
                DSP_MW_LOG_I("[wired_audio][config]change channel%d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                            1,
                            old_config.target_gain,
                            new_gain);
                stream_function_sw_gain_configure_gain_target(port, 1, new_gain);
                /* right channel */
                new_gain = p_config_param->vol_level.gain_2;
                stream_function_sw_gain_get_config(port, 2, &old_config);
                DSP_MW_LOG_I("[wired_audio][config]change channel%d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                            2,
                            old_config.target_gain,
                            new_gain);
                stream_function_sw_gain_configure_gain_target(port, 2, new_gain);
                #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            }
        }
        else if ((sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) || (sub_id.wiredaudio_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
        {
            wired_audio_handle_t *wire_audio_handle = (wired_audio_handle_t *)(application_ptr->source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle);
            wire_audio_runtime_config(sub_id.wiredaudio_id, config_param, wire_audio_handle);
        }
    }
#endif
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    if (scenario_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE)
    {
        if ((sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
            (sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
        {
            SourceConfigure(application_ptr->source, AUDIO_SOURCE_MISC_PARMS, (uint32_t)config_param);
        }
        else if(sub_id.ble_audio_dongle_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
        {
            SourceConfigure(application_ptr->source, AUDIO_SOURCE_MISC_PARMS, (uint32_t)config_param);
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

    return true;
}

/**
 * @brief This function is used to config audio transmitter source.
 *
 * @param source is the instance who is wanted be configured.
 * @param type is the configure type.
 * @param value is the configure paramters.
 * @return true means the configuration is done successfully.
 * @return false means the configuration is done unsuccessfully.
 */
bool port_audio_transmitter_source_config(SOURCE source, stream_config_type type, U32 value)
{
    bool ret = false;

    UNUSED(type);
    UNUSED(value);

    if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {
        ret = true;
    }
#if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        /* Dongle side, music path, usb in case */
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
        {
            ret = gaming_mode_dongle_dl_config(source, type, value);
        }
    }
#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, music path, usb in case */
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
        {
            ret = ble_audio_dongle_dl_config(source, type, value);
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    else {
        configASSERT(0);
    }

    return ret;
}

/**
 * @brief This function is used to get the available space size of the source based on the speical scenario type.
 *
 * @param source is the instance who is checked its available space size.
 * @param avail_size is the actual available size of the source.
 * @return true means this scenario use special method to get the the available size.
 * @return false means this scenario use common method to get the the available size.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static bool port_audio_transmitter_source_get_avail_size(SOURCE source, uint32_t *avail_size)
{
    bool ret = false;
    *avail_size = 0;

    if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {
        ret = false;
    }
#if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        /* Dongle side, music path, usb in case */
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
        {
            ret = gaming_mode_dongle_dl_source_get_avail_size(source, avail_size);
        }
    }
#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
#if defined (AIR_WIRED_AUDIO_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO) {
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
        {
            ret = wire_audio_source_get_avail_size(source, avail_size);
        }
    }
#endif /*AIR_WIRED_AUDIO_ENABLE*/
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, music path, usb in case */
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
        {
            ret = ble_audio_dongle_dl_source_get_avail_size(source, avail_size);
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    else {
        configASSERT(0);
    }

    return ret;
}

/**
 * @brief This function is used to copy the data from the share buffer of the source based on the speical scenario type.
 *
 * @param source is the instance who owns the share buffer.
 * @param dst_buf is the destination address.
 * @param length is the wanted copy size.
 * @return uint32_t is the actual copy size.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static uint32_t port_audio_transmitter_source_copy_payload(SOURCE source, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length)
{
    UNUSED(src_buf);
    UNUSED(dst_buf);

    if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {

    }
#if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        /* Dongle side, music path, usb in case */
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
        {
            length = gaming_mode_dongle_dl_source_copy_payload(source, src_buf, dst_buf, length);
        }
    }
#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
#if defined (AIR_WIRED_AUDIO_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO) {
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
        {
            length = wire_audio_source_copy_payload(source, src_buf, dst_buf, length);
        }
    }
#endif /* AIR_WIRED_AUDIO_ENABLE */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, music path, usb in case */
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
        {
            length = ble_audio_dongle_dl_source_copy_payload(source, src_buf, dst_buf, length);
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    else {
        configASSERT(0);
    }

    return length;
}

/**
 * @brief This function is used to get the new read offset of the source's share buffer based on the speical scenario type.
 *
 * @param source is the instance who owns the share buffer.
 * @param amount is the least offset size of the read offset.
 * @return uint32_t is the actual new read offset.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static uint32_t port_audio_transmitter_source_get_new_read_offset(SOURCE source, U32 amount)
{
    uint32_t ReadOffset = 0;
    UNUSED(amount);
    UNUSED(ReadOffset);

    switch(source->param.data_dl.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt source out case */
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                ReadOffset = gaming_mode_dongle_dl_source_get_new_read_offset(source, amount);
                return ReadOffset;
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined (AIR_WIRED_AUDIO_ENABLE)
        case AUDIO_TRANSMITTER_WIRED_AUDIO:
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
            {
                ReadOffset = wire_audio_source_get_new_read_offset(source, amount);
                return ReadOffset;
            }
            break;
        #endif /*AIR_WIRED_AUDIO_ENABLE*/

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt source out case */
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ReadOffset = ble_audio_dongle_dl_source_get_new_read_offset(source, amount);
                return ReadOffset;
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            configASSERT(0);
            return 0;
    }

    return 0;
}

/**
 * @brief This function is used to do special thing based on the speical scenario type after the drop is done.
 *
 * @param source is the instance who drop the data.
 * @param amount is the total length has been dropped.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static void port_audio_transmitter_source_drop_postprocess(SOURCE source, uint32_t amount)
{
    UNUSED(amount);

    switch(source->param.data_dl.scenario_type)
    {
        #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_GAMING_MODE:
            /* Dongle side, music path, bt source out case */
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                gaming_mode_dongle_dl_source_drop_postprocess(source, amount);
            }
            break;
        #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */

        #if defined(AIR_WIRED_AUDIO_ENABLE)
        case AUDIO_TRANSMITTER_WIRED_AUDIO:
            /* Dongle side, music path, bt source out case */
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
            {
                wire_audio_source_drop_postprocess(source, amount);
            }
            break;
        #endif /* AIR_WIRED_AUDIO_ENABLE */

        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        case AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE:
            /* Dongle side, music path, bt source out case */
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                ble_audio_dongle_dl_source_drop_postprocess(source, amount);
            }
            break;
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

        default:
            break;
    }
}

/**
 * @brief This function is used to get the new read offset of the source's share buffer based on the speical scenario type.
 *
 * @param source is the instance who owns the share buffer.
 * @param amount is the least offset size of the read offset.
 * @return uint32_t is the actual new read offset.
 */
static bool port_audio_transmitter_source_close(SOURCE source)
{
    bool ret;
    uint32_t saved_mask = 0;

    UNUSED(saved_mask);

    if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {
        ret = true;
    }
#if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        /* Dongle side, music path, usb in case */
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
        {
            ret = gaming_mode_dongle_dl_source_close(source);
        }
    }
#endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
#if defined (AIR_WIRED_AUDIO_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO) {
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
        {
           ret = wire_audio_source_close(source);
        }
    }
#endif /* AIR_WIRED_AUDIO_ENABLE */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, music path, usb in case */
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
            (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
        {
            ret = ble_audio_dongle_dl_source_close(source);
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    else {
        configASSERT(0);
    }

    return ret;
}

/**
 * @brief This function is the implementation about getting the available space size of the source in audio transmitter architecture.
 *
 * @param source the instance who is checked its available space size.
 * @return uint32_t is the actual available size of the source.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 U32 SourceSize_audio_transmitter(SOURCE source)
{
    bool ret;
    uint32_t avail_size = 0;

    audio_transmitter_share_information_fetch(source, NULL);

    ret = port_audio_transmitter_source_get_avail_size(source, &avail_size);
    if (ret == false) {
        avail_size = source->param.data_dl.max_payload_size;
    }

    return avail_size;
}

/**
 * @brief This function is the implementation for management mode of the source in audio transmitter architecture.
 *
 * @param source is the source instance.
 * @return uint8_t* is the return value.
 */
U8 *SourceMap_audio_transmitter(SOURCE source)
{
    UNUSED(source);

    return NULL;
}

/**
 * @brief This function is the implementation for dropping data and updating the read offset in audio transmitter architecture.
 *
 * @param source is the source instance.
 * @param amount the least offset size of the read offset.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID SourceDrop_audio_transmitter(SOURCE source, U32 amount)
{
    uint32_t ReadOffset;

    audio_transmitter_share_information_fetch(source, NULL);
    ReadOffset = port_audio_transmitter_source_get_new_read_offset(source, amount);
    audio_transmitter_share_information_update_read_offset(source, ReadOffset);

    /* do special process after the drop done */
    port_audio_transmitter_source_drop_postprocess(source, amount);
}

/**
 * @brief This function is the implementation for configuring the source in audio transmitter architecture.
 *
 * @param source is the source instance.
 * @param type is the configuration type.
 * @param value is the new value of the configuration type.
 * @return true means the configuration is done successfully.
 * @return false means the configuration is done unsuccessfully.
 */
BOOL SourceConfigure_audio_transmitter(SOURCE source, stream_config_type type, U32 value)
{
    bool ret;

    UNUSED(source);
    UNUSED(type);
    UNUSED(value);

    ret = port_audio_transmitter_source_config(source, type, value);

    return ret;
}

/**
 * @brief This function is the implementation to read data from the source's share buffer in audio transmitter architecture.
 *
 * @param source is the source instance.
 * @param dst_addr is the destination address.
 * @param length is the wanted copy size.
 * @return true means the read operation is done successfully.
 * @return false means the read operation is done unsuccessfully.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL SourceReadBuf_audio_transmitter(SOURCE source, U8 *dst_addr, U32 length)
{
    uint32_t avail_size, payload_size;
    SHARE_BUFFER_INFO_PTR ShareBufferInfo;

    audio_transmitter_share_information_fetch(source, NULL);
    ShareBufferInfo = &(source->streamBuffer.ShareBufferInfo);

    // available size double check
    port_audio_transmitter_source_get_avail_size(source, &avail_size);
    if(length > avail_size)
    {
        DSP_MW_LOG_E("[audio transmitter]: error length %d, avail_size %d", 2, length, avail_size);
        return FALSE;
    }

    /* Copy payload header and payload */
    payload_size = port_audio_transmitter_source_copy_payload(source,
                                                              (uint8_t *)(ShareBufferInfo->startaddr + ShareBufferInfo->ReadOffset),
                                                              dst_addr,
                                                              length);
    if (payload_size > source->param.data_dl.max_payload_size) {
        DSP_MW_LOG_E("[audio transmitter]: error payload size %d, max payload size %d", 2, payload_size, source->param.data_dl.max_payload_size);
        return FALSE;
    }
    source->param.data_dl.frame_size = payload_size;

    return TRUE;
}

/**
 * @brief This function is the implementation to close the source in audio transmitter architecture.
 *
 * @param source is the source instance.
 * @return true means the close operation is done successfully.
 * @return false means the close operation is done unsuccessfully.
 */
BOOL SourceClose_audio_transmitter(SOURCE source)
{
    UNUSED(source);

    return port_audio_transmitter_source_close(source);
}

/**
 * @brief This function is the implementation to initialize the source in audio transmitter architecture.
 *
 * @param source is the source instance.
 */
void SourceInit_audio_transmitter(SOURCE source)
{
    // audio_transmitter_share_information_reset_read_write_offset(source, NULL);

    /* interface init */
    source->sif.SourceSize          = SourceSize_audio_transmitter;
    source->sif.SourceMap           = SourceMap_audio_transmitter;
    source->sif.SourceDrop          = SourceDrop_audio_transmitter;
    source->sif.SourceConfigure     = SourceConfigure_audio_transmitter;
    source->sif.SourceReadBuf       = SourceReadBuf_audio_transmitter;
    source->sif.SourceClose         = SourceClose_audio_transmitter;
}

/*
 * This function is used to copy the data to the share buffer.
 * User MUST NOT cross the max payload size in the block!!!!
 * User should add porting code in this function for the specific scenario.
 */
static uint32_t port_audio_transmitter_sink_copy_payload(SINK sink, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length)
{
    if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {

    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GSENSOR) {
        memcpy(dst_buf,src_buf,length);
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_MULTI_MIC_STREAM) {

#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        /* Dongle side, voice path, usb out case */
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
        {
            #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
            length = gaming_mode_dongle_ul_sink_copy_payload(sink, src_buf, dst_buf, length);
            #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
        } else if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
        {
            SHARE_BUFFER_INFO_PTR ShareBufferInfo;
            audio_transmitter_share_information_fetch(NULL, sink);
            ShareBufferInfo = &(sink->streamBuffer.ShareBufferInfo);
            uint8_t* src_data_buffer;
            uint32_t blk_size = (uint32_t)ShareBufferInfo->sub_info.block_info.blk_size;
            uint32_t total_buffer_size = blk_size * ShareBufferInfo->sub_info.block_info.blk_num;
            uint32_t write_offset = ShareBufferInfo->WriteOffset;

            game_headset_voice_param_t *voice_para = &(sink->param.data_ul.scenario_param.voice_param);
            for(int frame_indx = 0; frame_indx < voice_para->process_frame_num; frame_indx++) {
                src_data_buffer = (U8*)(src_buf + frame_indx*length);
                dst_buf = (uint8_t *)(ShareBufferInfo->startaddr + write_offset);
                // seq. no:    1 byte
                // frame size: 1 byte
                // checksum:   2 byte
                voice_para->seq_num++;
#ifdef AIR_GAMING_MODE_HEADSET_ECNR_ENABLE
                voice_para->frame_size = length + 1;
#else
                voice_para->frame_size = length;
#endif
                voice_para->checksum = 0;
                for(U8 indx = 0; indx < voice_para->frame_size; indx++) {
                voice_para->checksum += *((U8*)(src_data_buffer + indx));
                //printf("[UL] chksum %d %d %d", indx, *((U8*)(src_buf + indx)), voice_para.checksum);
                }
                voice_para->checksum += voice_para->seq_num;
                //printf("[UL] seq:0x%x, frame_size:%d, chksum:0x%x, blk_size:%d", voice_para->seq_num, voice_para->frame_size, voice_para->checksum, blk_size);
                memcpy(dst_buf, voice_para, 4);
                memcpy(dst_buf+4, src_buf + frame_indx*voice_para->frame_size, length);
#ifdef AIR_GAMING_MODE_HEADSET_ECNR_ENABLE
                uint8_t PostEC_Gain = AEC_NR_Get_PostEC_Gain();
                memcpy(dst_buf + 4 + length, &PostEC_Gain, 1);
                DSP_MW_LOG_I("[AEC] send PostEC_Gain %d, %d, %d, %d, %d", 5, PostEC_Gain, dst_buf[0], dst_buf[1], dst_buf[2], dst_buf[3]);
#endif
                #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                LOG_AUDIO_DUMP((U8*)(dst_buf), (U32)length, AUDIO_WOOFER_CPD_OUT);
                #endif

                write_offset = (write_offset + blk_size) % total_buffer_size;

                #ifdef AIR_GAMING_MODE_UPLINK_LANTENCY_DEBUG_ENABLE
                uint32_t bt_clk;
                uint16_t intra_clk;
                extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);
                MCE_GetBtClk((BTCLK *)&bt_clk,(BTPHASE *)&intra_clk,BT_CLK_Offset);
                DSP_MW_LOG_I("[audio transmitter][gaming_headset ul]: sink copy, seq_num:%d, bt_clk:0x%x", 2, voice_para->seq_num, bt_clk);
                #endif /* AIR_GAMING_MODE_UPLINK_LANTENCY_DEBUG_ENABLE */
            }
        }
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
    }
#if defined(AIR_WIRED_AUDIO_ENABLE)
else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO) {
    if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT) {
        TRANSFORM transform =  sink->transform;
        DSP_STREAMING_PARA_PTR stream = NULL;

        //DSP_MW_LOG_E("[audio transmitter]: wired_audio length =  %d", 1, length);
        stream = DSP_Streaming_Get(transform->source, sink);

        if ((length%2) != 0)
        {
            configASSERT(0);
        }
        //  DSP_D2I_BufferCopy_16bit((U16 *)dst_buf,
        //                           stream->callback.EntryPara.out_ptr[0],
        //                           stream->callback.EntryPara.out_ptr[0],
        //                           length / 2);
        memcpy(dst_buf, stream->callback.EntryPara.out_ptr[0], length);
        #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
        LOG_AUDIO_DUMP(dst_buf, length, SOURCE_IN5);
        #endif /* MTK_AUDIO_DUMP_BY_CONFIGTOOL */
        //length = length * 2;
    }
}
#endif
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, voice path, usb out case */
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
        {
            length = ble_audio_dongle_ul_sink_copy_payload(sink, src_buf, dst_buf, length);
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    else {
        configASSERT(0);
    }

    return length;
}

/*
 * This function is used to get the left avail size in the block.
 * If user don't decide this, the default max payload size will be used.
 * User should add porting code in this function for the specific scenario.
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 static bool port_audio_transmitter_sink_get_avail_size(SINK sink, uint32_t *avail_size)
{
    bool ret = false;

    *avail_size = 0;

    if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {
        ret = false;
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GSENSOR) {
        ret = false;
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_MULTI_MIC_STREAM) {
        ret = false;
#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        /* Dongle side, voice path, usb out case */
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
        {
            #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
            ret = gaming_mode_dongle_ul_sink_get_avail_size(sink, avail_size);
            #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
        }else if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
        {
            *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size;
        }
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
    }
#if defined(AIR_WIRED_AUDIO_ENABLE)
else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO) {
        /* Dongle side, voice path, usb out case */
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT)
        {
            if (sink->streamBuffer.ShareBufferInfo.ReadOffset < sink->streamBuffer.ShareBufferInfo.WriteOffset)
            {
                /* normal case */
                *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num
                            - sink->streamBuffer.ShareBufferInfo.WriteOffset
                            + sink->streamBuffer.ShareBufferInfo.ReadOffset;
            }
            else if (sink->streamBuffer.ShareBufferInfo.ReadOffset == sink->streamBuffer.ShareBufferInfo.WriteOffset)
            {
                if(sink->streamBuffer.ShareBufferInfo.bBufferIsFull == true)
                {
                    /* buffer is full, so ReadOffset == WriteOffset */
                    *avail_size = 0;
                }
                else
                {
                    /* buffer is empty, so ReadOffset == WriteOffset */
                    *avail_size = sink->streamBuffer.ShareBufferInfo.length;
                }
            }
            else
            {
                /* buffer wrapper case */
                *avail_size = sink->streamBuffer.ShareBufferInfo.ReadOffset - sink->streamBuffer.ShareBufferInfo.WriteOffset;
            }
            /* change avail_size to max_payload_size if the avail_size is too large */
            if (*avail_size > sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size)
            {
                *avail_size = sink->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size;
            }
            ret = true;
        }
}
#endif
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, voice path, usb out case */
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
        {
            ret = ble_audio_dongle_ul_sink_get_avail_size(sink, avail_size);
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    else {
        configASSERT(0);
    }

    return ret;
}

/*
 * This function is used to query whether the notification should be sent to CM4.
 * If user don't decide this, the default rule will be used to send notification.
 * User should add porting code in this function for the specific scenario.
 */
static bool port_audio_transmitter_sink_query_notification(SINK sink, bool *notification_flag)
{
    bool ret;

    *notification_flag = false;

    if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {
        ret = false;
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GSENSOR) {
        ret = false;
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_MULTI_MIC_STREAM) {
        ret = false;
#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        if(sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET) {
            #if 0
            if(sink->param.data_ul.current_notification_index == 0) {
                /* prepare message */
                hal_ccni_message_t msg;
                msg.ccni_message[0] = AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET;
                msg.ccni_message[1] = 0;
                /* send CCNI to notify controller*/
                hal_ccni_set_event(CCNI_DSP0_TO_CM4_EVENT2, &msg);

                sink->param.data_ul.current_notification_index = 1;
                DSP_MW_LOG_I("[audio transmitter][gaming_headset ul]: send CCNI to notice controller", 0);
            }
            ret = true;
            #endif
            ret = true;
        } else {
            ret = true;
        }
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
    }
#if defined(AIR_WIRED_AUDIO_ENABLE)
    else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO) {
        ret = false;
    }
#endif
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, voice path, usb out case */
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
        {
            ret = true;
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    else {
        configASSERT(0);
    }

    return ret;
}

/*
 * This function is used to query the next write offset.
 * If user don't decide this, the default rule will be apply to update write offset.
 * User should add porting code in this function for the specific scenario.
 */
static bool port_audio_transmitter_sink_query_write_offset(SINK sink, uint32_t *write_offset)
{
#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)

#else
    UNUSED(sink);
    UNUSED(write_offset);
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */

    // *write_offset = 0;

    /* Scenario user should insert relative code flow here */
#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
     if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET) {
            U8 process_frame_num = sink->param.data_ul.scenario_param.voice_param.process_frame_num;
            uint32_t total_buffer_size;
            SHARE_BUFFER_INFO_PTR ShareBufferInfo;
            audio_transmitter_share_information_fetch(NULL, sink);
            ShareBufferInfo = &sink->streamBuffer.ShareBufferInfo;
            total_buffer_size = ShareBufferInfo->sub_info.block_info.blk_size * ShareBufferInfo->sub_info.block_info.blk_num;
            ShareBufferInfo->WriteOffset = (ShareBufferInfo->WriteOffset + ShareBufferInfo->sub_info.block_info.blk_size * process_frame_num) % total_buffer_size;
            sink->param.data_ul.scenario_param.voice_param.ul_process_done = true;
            *write_offset = ShareBufferInfo->WriteOffset;
            uint32_t bt_clk;
            uint16_t intra_clk;
            extern VOID MCE_GetBtClk(BTCLK* pCurrCLK, BTPHASE* pCurrPhase,BT_CLOCK_OFFSET_SCENARIO type);
            MCE_GetBtClk((BTCLK *)&bt_clk,(BTPHASE *)&intra_clk,BT_CLK_Offset);
            uint32_t current_timestamp = 0;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
            //DSP_MW_LOG_I("[audio transmitter][source]: write_offset = %d, %d, %u, %d", 4, *write_offset, bt_clk, current_timestamp, (sink->param.data_ul.scenario_param.voice_param.seq_num&0xff));
            if(sink->param.data_ul.current_notification_index == 0) {
                /* prepare message */
                hal_ccni_message_t msg;
                msg.ccni_message[0] = AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET;
                msg.ccni_message[1] = bt_clk;
                /* send CCNI to notify controller*/
                hal_ccni_set_event(CCNI_DSP0_TO_CM4_EVENT2, &msg);
                sink->param.data_ul.current_notification_index = 1;
                DSP_MW_LOG_I("[audio transmitter][gaming_headset ul]: send CCNI to notice controller, current_notification_index:%d, bt_clk:%d", 2, sink->param.data_ul.current_notification_index, bt_clk);
            }
            #ifdef AIR_GAMING_MODE_UPLINK_LANTENCY_DEBUG_ENABLE
            else
            {
                DSP_MW_LOG_I("[audio transmitter][gaming_headset ul]: write_offset:%d, bt_clk:0x%x", 2, *write_offset, bt_clk);
            }
            #endif /* AIR_GAMING_MODE_UPLINK_LANTENCY_DEBUG_ENABLE */
            return true;
        }
     }
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, voice path, usb out case */
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
        {
            ble_audio_dongle_ul_sink_query_write_offset(sink, write_offset);
            return true;
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    return false;
}

/*
 * This function is used to call scenario relative close operation flow.
 * User should add porting code in this function for the specific scenario.
 */
static bool port_audio_transmitter_sink_close(SINK sink)
{
    bool ret;

    if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE) {
        ret = true;
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GSENSOR) {
        ret = true;
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_MULTI_MIC_STREAM) {
        ret = true;
#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
    } else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
        {
            #if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
            ret = gaming_mode_dongle_ul_sink_close(sink);
            #endif /* AIR_GAMING_MODE_DONGLE_ENABLE */
        }
        else
        {
            ret = true;
        }
#endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
    }
#if defined(AIR_WIRED_AUDIO_ENABLE)
else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO) {
        ret = true;
    }
#endif
#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
    else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE) {
        /* Dongle side, voice path, usb out case */
        if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
        {
            ret = ble_audio_dongle_ul_sink_close(sink);
        }
    }
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
    else {
        configASSERT(0);
    }

    return ret;
}

static void send_data_ready_notification(SINK sink)
{
    hal_ccni_message_t msg;

    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = (MSG_DSP2MCU_AUDIO_TRANSMITTER_DATA_NOTIFY << 16) + (sink->param.data_ul.scenario_type << 8) + sink->param.data_ul.scenario_sub_id;
    aud_msg_tx_handler(msg, 0, FALSE);
}

/* Report how many bytes can successfully be flush into the sink. */
ATTR_TEXT_IN_IRAM_LEVEL_1 U32 SinkSlack_audio_transmitter(SINK sink)
{
    bool ret;
    uint32_t avail_size;

    audio_transmitter_share_information_fetch(NULL, sink);

    if (sink->param.data_ul.is_customize == false)  { //PT: temp for gaming headset uplink
        if (sink->streamBuffer.ShareBufferInfo.bBufferIsFull == true) {
            DSP_MW_LOG_E("[audio transmitter]: query share buffer full", 0);
            configASSERT(0);
            return 0;
        }
    }

    ret = port_audio_transmitter_sink_get_avail_size(sink, &avail_size);
    if (ret == false) {
        avail_size = sink->param.data_ul.max_payload_size;
    }
    return avail_size;
}

/* Map the sink into the address map, returning a pointer, to the first byte in the sink, only the size of sinkclaim is avaliable */
U8* SinkMap_audio_transmitter(SINK sink)
{
    UNUSED(sink);

    return NULL;
}

/* Configure a particular sink. */
BOOL SinkConfigure_audio_transmitter(SINK sink, stream_config_type type, U32 value)
{
    UNUSED(sink);
    UNUSED(type);
    UNUSED(value);

    return true;
}

/* write buffer from source address into sink */
BOOL SinkWriteBuf_audio_transmitter(SINK sink, U8 *src_addr, U32 length)
{
    audio_transmitter_frame_header_t *data_ul_header;
    uint32_t avail_block_num, total_buffer_size, payload_size;
    SHARE_BUFFER_INFO_PTR ShareBufferInfo;

    audio_transmitter_share_information_fetch(NULL, sink);
    ShareBufferInfo = &(sink->streamBuffer.ShareBufferInfo);

    /* Check whether overflow happen */
    if (sink->param.data_ul.is_assembling == false) {
        total_buffer_size = ShareBufferInfo->sub_info.block_info.blk_size * ShareBufferInfo->sub_info.block_info.blk_num;
        if (ShareBufferInfo->bBufferIsFull == true) {
            avail_block_num = 0;
        } else if(ShareBufferInfo->ReadOffset > ShareBufferInfo->WriteOffset) {
            avail_block_num = (ShareBufferInfo->ReadOffset - ShareBufferInfo->WriteOffset) / ShareBufferInfo->sub_info.block_info.blk_size;
        } else {
            avail_block_num = (total_buffer_size - (ShareBufferInfo->WriteOffset - ShareBufferInfo->ReadOffset)) / ShareBufferInfo->sub_info.block_info.blk_size;
        }
        if (avail_block_num == 0) {
            DSP_MW_LOG_E("[audio transmitter]: detect share buffer full", 0);
            return FALSE;
        }
        sink->param.data_ul.is_assembling = true;
    }

    /* Copy payload header and payload */
    if (sink->param.data_ul.is_customize == true) {
        payload_size = port_audio_transmitter_sink_copy_payload(sink, src_addr, (uint8_t *)(ShareBufferInfo->startaddr + ShareBufferInfo->WriteOffset), length);
    } else {
        payload_size = port_audio_transmitter_sink_copy_payload(sink, src_addr, (uint8_t *)(ShareBufferInfo->startaddr + ShareBufferInfo->WriteOffset + sizeof(audio_transmitter_frame_header_t)), length);
    }
    if (payload_size > sink->param.data_ul.max_payload_size) {
        DSP_MW_LOG_E("[audio transmitter]: payload size %d, max payload size %d", 2, payload_size, sink->param.data_ul.max_payload_size);
        return FALSE;
    }
    sink->param.data_ul.frame_size = payload_size;

    /* Copy header */
    if (sink->param.data_ul.frame_size) {
        if (sink->param.data_ul.is_customize == false) {
            data_ul_header = (audio_transmitter_frame_header_t *)(ShareBufferInfo->startaddr + ShareBufferInfo->WriteOffset);
            data_ul_header->seq_num = sink->param.data_ul.seq_num++;
            data_ul_header->payload_len = sink->param.data_ul.frame_size;
            sink->param.data_ul.is_assembling = false;
        }
    }

    return TRUE;
}

/* Flush the indicated number of bytes out of the sink. */
BOOL SinkFlush_audio_transmitter(SINK sink, U32 amount)
{
    bool notification_flag, query_status;
    uint32_t total_buffer_size;
    SHARE_BUFFER_INFO_PTR ShareBufferInfo;

    UNUSED(amount);

    /* If one frame has not fill done, don't notice host */
    if (sink->param.data_ul.frame_size == 0) {
        return FALSE;
    }

    audio_transmitter_share_information_fetch(NULL, sink);
    ShareBufferInfo = &sink->streamBuffer.ShareBufferInfo;
    total_buffer_size = ShareBufferInfo->sub_info.block_info.blk_size * ShareBufferInfo->sub_info.block_info.blk_num;
    if (sink->param.data_ul.is_customize == false) {
        ShareBufferInfo->WriteOffset = (ShareBufferInfo->WriteOffset + ShareBufferInfo->sub_info.block_info.blk_size) % total_buffer_size;
        query_status = true;
    } else {
        query_status = port_audio_transmitter_sink_query_write_offset(sink, &(ShareBufferInfo->WriteOffset));
    }

    if (query_status == true) {
        audio_transmitter_share_information_update_write_offset(sink, ShareBufferInfo->WriteOffset);
    }

    if (port_audio_transmitter_sink_query_notification(sink, &notification_flag) == false) {
        sink->param.data_ul.current_notification_index++;
        if (sink->param.data_ul.current_notification_index >= sink->param.data_ul.data_notification_frequency) {
            sink->param.data_ul.current_notification_index = 0;
            notification_flag = true;
        } else {
            notification_flag = false;
        }
    }
    if (notification_flag == true) {
        send_data_ready_notification(sink);
    }

    return true;
}

/* Request to close the sink */
BOOL SinkClose_audio_transmitter(SINK sink)
{
    UNUSED(sink);

    return port_audio_transmitter_sink_close(sink);
}

void SinkInit_audio_transmitter(SINK sink)
{
    audio_transmitter_share_information_reset_read_write_offset(NULL, sink);

    /* interface init */
    sink->sif.SinkSlack        = SinkSlack_audio_transmitter;
    sink->sif.SinkMap          = SinkMap_audio_transmitter;
    sink->sif.SinkConfigure    = SinkConfigure_audio_transmitter;
    sink->sif.SinkFlush        = SinkFlush_audio_transmitter;
    sink->sif.SinkClose        = SinkClose_audio_transmitter;
    sink->sif.SinkWriteBuf     = SinkWriteBuf_audio_transmitter;
}

#endif

