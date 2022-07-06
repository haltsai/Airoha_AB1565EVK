/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
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

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include "task_def.h"
#include "bt_sink_srv_am_task.h"
#include "bt_sink_srv_ami.h"
//#include "bt_sink_srv_media_mgr.h"

#include "bt_sink_srv_audio_tunning.h"
#include "bt_sink_srv_audio_setting.h"
#include "bt_connection_manager_internal.h"
#include "FreeRTOS.h"
#ifdef __BT_AWS_SUPPORT__
#include "bt_aws.h"
#include "bt_codec.h"
#include "bt_sink_srv_audio_sync.h"
#include "bt_sink_srv_music.h"
#include "bt_sink_srv_aws.h"
#endif /* __BT_AWS_SUPPORT__ */
#ifdef MTK_BT_A2DP_VENDOR_ENABLE
#include "bt_sink_srv_music.h"
#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "bt_le_audio_def.h"
#endif

#ifdef __BT_SINK_AUDIO_TUNING__
#include "audio_dsp_fd216_db_to_gain_value_mapping_table.h"
#endif /* __BT_SINK_AUDIO_TUNING__ */
#if PRODUCT_VERSION == 2533
#include "external_dsp_application.h"
#endif

#ifdef __AM_DEBUG_INFO__
#include "bt_sink_srv_utils.h"
#endif

#if defined(MTK_LINEIN_PLAYBACK_ENABLE)
#include "linein_playback.h"
#endif

#ifdef MTK_LINE_IN_ENABLE
#include "audio_sink_srv_line_in.h"
#endif

#include "bt_sink_srv_audio_setting.h"

#ifdef MTK_PROMPT_SOUND_ENABLE
#include "prompt_control.h"
#endif

#if defined(MTK_AVM_DIRECT)
#include "audio_src_srv_internal.h"
#include "hal_adc.h"
#include "hal_gpio.h"
#include "hal_audio_internal.h"
#include "hal_audio_cm4_dsp_message.h"
#include "hal_resource_assignment.h"
#endif

#include "audio_nvdm.h"
#include "audio_nvdm_common.h"
#include "audio_log.h"
#include "nvkey.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_callback_manager.h"
#endif

#ifdef MTK_USB_AUDIO_PLAYBACK_ENABLE
#include "usb_audio_playback.h"
#endif

#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
#include "audio_transmitter_playback.h"
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
#include "bt_ull_service.h"
#endif /* defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE) */
#endif

#ifdef MTK_USER_TRIGGER_FF_ENABLE
#include "user_trigger_adaptive_ff.h"
#endif


#if defined (MTK_AUDIO_TRANSMITTER_ENABLE) && defined (MTK_ANC_SURROUND_MONITOR_ENABLE)
#include "anc_monitor.h"
#endif
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    #include "audio_transmitter_internal.h"
#if defined(AIR_WIRED_AUDIO_ENABLE)
#include "scenario_wired_audio.h"
#define LINE_IN_USB_IN_EXCLUSIVE_WITH_RECORD_ENABLE
#endif
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
#include "race_cmd_co_sys.h"
#include "race_cmd_dsprealtime.h"
#include "race_cmd_hostaudio.h"
extern race_dsp_realtime_callback_t  g_race_cosys_dsp_realtime_callback;
extern void RACE_DSPREALTIME_callback(uint32_t event_id);
extern void RACE_DSPREALTIME_COSYS_GET_PARAM(am_feature_type_t audio_feature, void *data);
#endif

#ifndef _UNUSED
#define _UNUSED(x)  ((void)(x))
#endif

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

const uint32_t g_volume_out_config[DEVICE_OUT_MAX][AUD_VOL_TYPE][AUD_VOL_OUT_MAX][2] =
{
    {   {{0xFFFFFC18, 0x044C}, {0xFFFFFCE0, 0x044C}, {0xFFFFFDA8, 0x044C}, {0xFFFFFE70, 0x044C}, {0xFFFFFF38, 0x044C}, {0x00000000, 0x044C}, {0x00000000, 0x0384}},
        {{0xFFFFFC18, 0x044C}, {0xFFFFFCE0, 0x044C}, {0xFFFFFDA8, 0x044C}, {0xFFFFFE70, 0x044C}, {0xFFFFFF38, 0x044C}, {0x00000000, 0x044C}, {0x00000000, 0x0384}}
    },
    {   {{0xFFFFF768, 0x0000}, {0xFFFFF8F8, 0x0000}, {0xFFFFFA88, 0x0000}, {0xFFFFFC18, 0x0000}, {0xFFFFFDA8, 0x0000}, {0xFFFFFF38, 0x0000}, {0x00000000, 0x00C8}},
        {{0xFFFFF768, 0x0000}, {0xFFFFF8F8, 0x0000}, {0xFFFFFA88, 0x0000}, {0xFFFFFC18, 0x0000}, {0xFFFFFDA8, 0x0000}, {0xFFFFFF38, 0x0000}, {0x00000000, 0x00C8}}
    }
};
const uint32_t g_volume_in_config[DEVICE_IN_MAX][AUD_VOL_IN_MAX][2] = {{{0x00000000, 0x0258}}};

bt_sink_srv_am_amm_struct *ptr_callback_amm = NULL;
bt_sink_srv_am_amm_struct *ptr_isr_callback_amm = NULL;
bt_sink_srv_am_aud_id_type_t g_rAm_aud_id[AM_REGISTER_ID_TOTAL];
static bt_sink_srv_am_background_t *g_prCurrent_resumer = NULL;
static uint8_t *g_bBT_Ringbuf = NULL;
#ifdef RTOS_TIMER
static TimerHandle_t g_xTimer_am;
static uint16_t g_lExpire_count = 0;
#endif
static bt_sink_srv_am_background_t g_rBackground_head = {0};
static bt_sink_srv_am_sink_state_t g_rSink_state = A2DP_SINK_CODEC_CLOSE;
static bt_sink_srv_am_media_handle_t g_prHfp_sink_event_handle;
bt_media_handle_t *g_prHfp_media_handle = NULL;
static bt_sink_srv_am_media_handle_t g_prA2dp_sink_event_handle;
bt_media_handle_t *g_prA2dp_sink_handle = NULL;
bt_sink_srv_am_background_t *g_prCurrent_playback[AM_PLAYBACK_INDEX_TOTAL] = {NULL, NULL, NULL, NULL};
#ifdef MTK_LINE_IN_ENABLE
audio_sink_srv_am_line_in_codec_t g_prLineIN_sink_handle;
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
static bt_sink_srv_am_media_handle_t g_prBle_sink_event_handle;
bt_media_handle_t *g_prBle_media_handle = NULL;
#endif

#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
#ifdef MTK_ANC_ENABLE
#ifndef MTK_ANC_V2
extern anc_control_t g_anc_control;
static bool g_am_anc_flag = false;
#endif
#endif
#endif
static uint16_t *aud_memory = NULL;
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
static uint8_t g_bt_sink_srv_am_ring_buffer[AM_RING_BUFFER_SIZE];
#endif

int32_t g_test_fix_warning = 0;

#if defined(MTK_AVM_DIRECT)
volatile uint32_t g_am_task_mask = 0;
extern HAL_AUDIO_CHANNEL_SELECT_t audio_Channel_Select;
extern HAL_DSP_PARA_AU_AFE_CTRL_t audio_nvdm_HW_config;
extern HAL_AUDIO_DVFS_CLK_SELECT_t audio_nvdm_dvfs_config;
xSemaphoreHandle g_xSemaphore_Audio = NULL;
extern const audio_version_t SW_version;
extern volatile audio_version_t nvdm_version;
#endif
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
static peq_handle_t g_peq_handle;
static uint16_t g_nvkey_change_mask;
#endif
#ifdef MTK_RECORD_ENABLE
extern uint16_t g_stream_in_code_type;//modify for opus
extern encoder_bitrate_t g_bit_rate;
extern uint16_t g_wwe_mode;
#endif

extern int g_side_tone_gain_common;
extern int g_side_tone_gain_hfp;
uint8_t *pFIRBuf = NULL;


/* external dsp*/
//external_dsp_sending_path_register_value_t external_dsp_sending_path_register_value;

/* External DSP*/
//#if PRODUCT_VERSION == 2533
#if 0
external_dsp_sending_path_register_value_t external_dsp_sending_path_register_value;
#endif

/* files media handle */
static bt_sink_srv_am_files_media_handle_t g_am_files_media_handle;
static bt_sink_srv_am_file_state_t g_am_file_state = FILE_CODEC_CLOSE;

/* mp3 */
#ifdef __AUDIO_MP3_ENABLE__
static mp3_codec_media_handle_t *g_am_mp3_media_handle = NULL;
#endif

/* audio codec */
#ifdef __AUDIO_COMMON_CODEC_ENABLE__
static audio_codec_media_handle_t *g_am_audio_media_handle = NULL;
#endif

#ifdef __BT_AWS_SUPPORT__
static bt_aws_codec_type_t g_aws_codec_type = 0;
static uint16_t g_aws_sample_rate = 0;
static uint32_t g_aws_skew_loop_count = 0;
#endif /* __BT_AWS_SUPPORT__ */
static uint32_t g_a2dp_underflow_loop_count = 0;
#ifdef __BT_AWS_SUPPORT__
static uint32_t g_aws_underflow_loop_count = 0;
#endif /* __BT_AWS_SUPPORT__ */
#define BT_SINK_SRV_AM_MAX_UNDERFLOW_COUNT          (1)

static bool g_am_volume_enable = false;
static void am_receive_msg(bt_sink_srv_am_amm_struct *amm_ptr);
static void bt_codec_am_a2dp_sink_open(bt_sink_srv_am_a2dp_codec_t *a2dp_codec_t);
static bt_status_t bt_codec_am_a2dp_sink_play(bt_sink_srv_am_id_t aud_id);
static bt_status_t bt_codec_am_a2dp_sink_stop(bt_sink_srv_am_id_t aud_id);
static bt_status_t bt_codec_am_a2dp_sink_close(void);
static void bt_codec_am_hfp_open(bt_sink_srv_am_hfp_codec_t *hfp_codec_t);
static bt_status_t bt_codec_am_hfp_stop(void);
#ifdef AIR_BT_CODEC_BLE_ENABLED
static void bt_codec_am_ble_open(bt_sink_srv_am_ble_codec_t *hfp_codec_t);
static bt_status_t bt_codec_am_ble_stop(void);
#endif

static void am_files_codec_open(bt_sink_srv_am_files_format_t *files_format);

static void am_files_codec_close(void);

#ifdef __BT_AWS_SUPPORT__
static int32_t bt_a2dp_aws_set_flag(bt_sink_srv_am_id_t aud_id, bool flag);
static int32_t bt_a2dp_aws_set_initial_sync(bt_sink_srv_am_id_t aud_id);
static void bt_a2dp_aws_plh_init(bt_sink_srv_am_id_t aud_id);
static void bt_a2dp_aws_plh_deinit(void);
static void aud_process_aws_a2dp_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static int32_t bt_mp3_aws_set_flag(bt_sink_srv_am_id_t aud_id, bool flag);
static int32_t bt_mp3_aws_set_initial_sync(bt_sink_srv_am_id_t aud_id);
static int32_t bt_mp3_aws_init();
static int32_t bt_mp3_aws_deinit();
static int32_t bt_mp3_aws_set_clock_skew_compensation_value(int32_t sample_count);
static int32_t bt_mp3_aws_get_clock_skew_status(int32_t *status);
#endif /* __BT_AWS_SUPPORT__ */

#ifdef __AUDIO_MP3_ENABLE__
static void am_mp3_get_write_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer,
    uint32_t *length);

static void am_mp3_get_read_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer,
    uint32_t *length);

static int32_t am_mp3_get_data_count(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_get_free_space(bt_sink_srv_am_id_t aud_id);

static void am_mp3_write_data_done(bt_sink_srv_am_id_t aud_id, uint32_t length);

static void am_mp3_finish_write_data(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_play(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_pause(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_resume(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_stop(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_close_codec(bt_sink_srv_am_id_t aud_id);

static int32_t am_mp3_skip_id3v2_and_reach_next_frame(bt_sink_srv_am_id_t aud_id,
    uint32_t file_size);

static int32_t am_mp3_set_silence_frame_information(bt_sink_srv_am_id_t aud_id, silence_frame_information_t *frm_info);

static int32_t am_mp3_fill_silence_frame(bt_sink_srv_am_id_t aud_id, uint8_t *buffer);

static int32_t am_mp3_get_data_status(bt_sink_srv_am_id_t aud_id,
        mp3_codec_data_type_t type, int32_t *status);

static int32_t am_mp3_flush(bt_sink_srv_am_id_t aud_id, int32_t flush_data_flag);

static void am_files_mp3_callback(mp3_codec_media_handle_t *hdl, mp3_codec_event_t event);
#endif /* __AUDIO_MP3_ENABLE__ */

#ifdef __AUDIO_COMMON_CODEC_ENABLE__
static void am_audio_get_write_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length);
static void am_audio_write_data_done(bt_sink_srv_am_id_t aud_id, uint32_t length);
static void am_audio_finish_write_data(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_get_data_count(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_get_free_space(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_play(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_pause(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_resume(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_stop(bt_sink_srv_am_id_t aud_id);
static int32_t am_audio_close_codec(bt_sink_srv_am_id_t aud_id);
//static void aud_process_audio_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static void am_files_audio_callback(audio_codec_media_handle_t *hdl, audio_codec_event_t event);
#endif

#ifdef MTK_AUDIO_MP3_ENABLED
static void aud_process_mp3_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
#endif

static void audio_set_resume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static void audio_set_pause_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);

void audio_side_tone_enable_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static void audio_side_tone_disable_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
void audio_side_tone_set_volume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);

static void audio_dl_suspend_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static void audio_dl_resume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static void audio_ul_suspend_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
static void audio_ul_resume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);

extern void BT_A2DP_CONVERT_SBC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec);

extern void BT_A2DP_CONVERT_AAC_CODEC(bt_codec_capability_t *dst_codec,
                               bt_a2dp_codec_capability_t *src_codec);

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
extern void RACE_DSPREALTIME_ANC_PASSTHRU_RELAY_COSYS_CALLBACK(bool is_critical, uint8_t *buff, uint32_t len);
#endif

#ifndef MTK_DEBUG_LEVEL_INFO
extern int32_t g_test_fix_warning;
#endif


#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
void bt_sink_srv_am_set_volume(bt_sink_srv_am_stream_type_t in_out, bt_sink_srv_audio_setting_vol_info_t *vol_info);
#endif

#ifdef __BT_AWS_SUPPORT__
extern void bt_sink_srv_fetch_bt_offset();
#endif

#if defined(MTK_AVM_DIRECT)
static void audio_set_feature_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
bt_sink_srv_am_result_t audio_nvdm_configure_init(void);
#if defined(MTK_AMP_DC_COMPENSATION_ENABLE)
static uint16_t get_dc_compensation_value();
static void sort(unsigned int *p, unsigned int entries);
#endif
#endif

void audio_message_audio_handler(hal_audio_event_t event, void *data);

#if defined(AIR_WIRED_AUDIO_ENABLE)
static void audio_transmitter_wired_audio_stop_close_usb_in(uint16_t scenario_and_id);
#endif /* AIR_WIRED_AUDIO_ENABLE */

#ifdef MTK_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
extern prompt_control_status_t prompt_control_dummy_source_start_internal(prompt_control_dummy_source_param_t param);
extern void prompt_control_dummy_source_stop_internal(void);
#endif /* VP Dummy Sourece */

#ifndef WIN32_UT
/*****************************************************************************
 * FUNCTION
 *  am_task_create
 * DESCRIPTION
 *  This function is used to create a task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void am_task_create(void)
{
    xTaskCreate(am_task_main,
                AM_TASK_NAME,
                AM_TASK_STACKSIZE,
                NULL,
                AM_TASK_PRIO,
                NULL);
}
#endif

bt_sink_srv_am_background_t *am_get_aud_by_type(bt_sink_srv_am_type_t type)
{
    bt_sink_srv_am_background_t *prTarget_player = NULL;
    uint32_t i;
    for(i = 0; i < AM_REGISTER_ID_TOTAL; i++)
    {
        if(g_rAm_aud_id[i].contain_ptr && g_rAm_aud_id[i].contain_ptr->type == type)
        {
            prTarget_player = g_rAm_aud_id[i].contain_ptr;
            break;
        }
    }
    return prTarget_player;
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_play
 * DESCRIPTION
 *  This function is used to play the audio handler.
 * PARAMETERS
 *  background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
bt_sink_srv_am_hal_result_t aud_set_volume_level(bt_sink_srv_am_stream_type_t in_out,
        bt_sink_srv_am_volume_type_t vol_type,
        bt_sink_srv_am_device_set_t device,
        bt_sink_srv_am_volume_level_t level)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_hal_result_t eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_srv_am_device_out_t dev_out;
    bt_sink_srv_am_device_in_t dev_in;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_VOLUME_CHANGE);
    #endif

    audio_src_srv_report("[Sink][AM]in_out:%d, level:%d, device:0x%x, vol_type:%d, g_am_volume_enable:%d",5, in_out, level, device, vol_type, g_am_volume_enable);

    if (in_out == STREAM_OUT) {
        if (level < AUD_VOL_OUT_MAX) {
            if (device & DEVICE_LOUDSPEAKER) {
                //device = LOUDSPEAKER_STREAM_OUT;
                dev_out = LOUDSPEAKER_STREAM_OUT;
                audio_src_srv_report("[Sink][AM]dev_out:%d, digital:0x%x, analog:0x%x",3, dev_out, g_volume_out_config[dev_out][vol_type][level][0], g_volume_out_config[dev_out][vol_type][level][1]);
                if (g_am_volume_enable) {
                    ;
                } else {
                    eResult = hal_audio_set_stream_out_volume(g_volume_out_config[dev_out][vol_type][level][0], g_volume_out_config[dev_out][vol_type][level][1]);
                }
            } else if (device & DEVICE_EARPHONE) {
                //device = EARPHONE_STREAM_OUT;
                dev_out = EARPHONE_STREAM_OUT;
                audio_src_srv_report("[Sink][AM]dev_out:%d, digital:0x%x, analog:0x%x",3, dev_out, g_volume_out_config[dev_out][vol_type][level][0], g_volume_out_config[dev_out][vol_type][level][1]);
                if (g_am_volume_enable) {
                    ;
                } else {
                    eResult = hal_audio_set_stream_out_volume(g_volume_out_config[dev_out][vol_type][level][0], g_volume_out_config[dev_out][vol_type][level][1]);
                }
            }
        }
    } else if (in_out == STREAM_IN) {
        if (level < AUD_VOL_IN_MAX) {
            if (device & DEVICE_IN_LIST) {
                //device = MICPHONE_STREAM_IN;
                dev_in = MICPHONE_STREAM_IN;
                if (g_am_volume_enable) {
                    ;
                } else {
                    eResult = hal_audio_set_stream_in_volume(g_volume_in_config[dev_in][level][0], g_volume_in_config[dev_in][level][1]);
                }
            }
        }
    }

    if (eResult == HAL_AUDIO_STATUS_OK) {
        return HAL_AUDIO_STATUS_OK;
    } else {
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM][ERROR] Device and Stream type/Vol-level Not Matched", 0);
#endif
        return HAL_AUDIO_STATUS_ERROR;
    }
}

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
static void aud_peq_init(void)
{
    uint32_t length = sizeof(peq_nvdm_misc_param_t);
    peq_nvdm_misc_param_t payload;
    sysram_status_t status;
    memset(&payload, 0, sizeof(peq_nvdm_misc_param_t));
    memset(&g_peq_handle, 0, sizeof(peq_handle_t));
    g_nvkey_change_mask = 0;
    status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_PEQ_MISC_PARA, (uint8_t *)&payload, &length);
    if ((status != NVDM_STATUS_NAT_OK) || (length != sizeof(peq_nvdm_misc_param_t))) {
        audio_src_srv_err("[Sink][AM]Read PEQ misc param error, error(%d), length:%d %d\n",3,status,length,sizeof(peq_nvdm_misc_param_t));
    } else {
        g_peq_handle.a2dp_pre_peq_enable        = payload.a2dp_pre_peq_enable;
        g_peq_handle.a2dp_pre_peq_sound_mode    = payload.a2dp_pre_peq_sound_mode;
        g_peq_handle.a2dp_post_peq_enable       = payload.a2dp_post_peq_enable;
        g_peq_handle.a2dp_post_peq_sound_mode   = payload.a2dp_post_peq_sound_mode;
        g_peq_handle.linein_pre_peq_enable      = payload.linein_pre_peq_enable;
        g_peq_handle.linein_pre_peq_sound_mode  = payload.linein_pre_peq_sound_mode;
        g_peq_handle.linein_post_peq_enable     = payload.linein_post_peq_enable;
        g_peq_handle.linein_post_peq_sound_mode = payload.linein_post_peq_sound_mode;
        g_peq_handle.advanced_passthrough_pre_peq_enable      = payload.advanced_passthrough_pre_peq_enable;
        g_peq_handle.advanced_passthrough_pre_peq_sound_mode  = payload.advanced_passthrough_pre_peq_sound_mode;
        g_peq_handle.advanced_passthrough_post_peq_enable     = payload.advanced_passthrough_post_peq_enable;
        g_peq_handle.advanced_passthrough_post_peq_sound_mode = payload.advanced_passthrough_post_peq_sound_mode;
        g_nvkey_change_mask = payload.nvkey_change_mask;
    }
}

void aud_peq_save_misc_param(void)
{
    peq_nvdm_misc_param_t payload;
    payload.a2dp_pre_peq_enable        = g_peq_handle.a2dp_pre_peq_enable;
    payload.a2dp_pre_peq_sound_mode    = g_peq_handle.a2dp_pre_peq_sound_mode;
    payload.a2dp_post_peq_enable       = g_peq_handle.a2dp_post_peq_enable;
    payload.a2dp_post_peq_sound_mode   = g_peq_handle.a2dp_post_peq_sound_mode;
    payload.linein_pre_peq_enable      = g_peq_handle.linein_pre_peq_enable;
    payload.linein_pre_peq_sound_mode  = g_peq_handle.linein_pre_peq_sound_mode;
    payload.linein_post_peq_enable     = g_peq_handle.linein_post_peq_enable;
    payload.linein_post_peq_sound_mode = g_peq_handle.linein_post_peq_sound_mode;
    payload.advanced_passthrough_pre_peq_enable      = g_peq_handle.advanced_passthrough_pre_peq_enable;
    payload.advanced_passthrough_pre_peq_sound_mode  = g_peq_handle.advanced_passthrough_pre_peq_sound_mode;
    payload.advanced_passthrough_post_peq_enable     = g_peq_handle.advanced_passthrough_post_peq_enable;
    payload.advanced_passthrough_post_peq_sound_mode = g_peq_handle.advanced_passthrough_post_peq_sound_mode;
    payload.nvkey_change_mask = g_nvkey_change_mask;
    if (flash_memory_write_nvdm_data(NVKEY_DSP_PARA_PEQ_MISC_PARA, (uint8_t *)&payload, sizeof(peq_nvdm_misc_param_t)) != NVDM_STATUS_NAT_OK) {
        audio_src_srv_err("[Sink][AM]Write PEQ misc param error\n",0);
    }
}

void aud_peq_save_sound_mode(void)
{
    peq_nvdm_misc_param_t payload;
    payload.a2dp_pre_peq_enable        = g_peq_handle.a2dp_pre_peq_enable;
    payload.a2dp_pre_peq_sound_mode    = g_peq_handle.a2dp_pre_peq_sound_mode;
    payload.a2dp_post_peq_enable       = g_peq_handle.a2dp_post_peq_enable;
    payload.a2dp_post_peq_sound_mode   = g_peq_handle.a2dp_post_peq_sound_mode;
    payload.linein_pre_peq_enable      = g_peq_handle.linein_pre_peq_enable;
    payload.linein_pre_peq_sound_mode  = g_peq_handle.linein_pre_peq_sound_mode;
    payload.linein_post_peq_enable     = g_peq_handle.linein_post_peq_enable;
    payload.linein_post_peq_sound_mode = g_peq_handle.linein_post_peq_sound_mode;
    payload.advanced_passthrough_pre_peq_enable      = g_peq_handle.advanced_passthrough_pre_peq_enable;
    payload.advanced_passthrough_pre_peq_sound_mode  = g_peq_handle.advanced_passthrough_pre_peq_sound_mode;
    payload.advanced_passthrough_post_peq_enable     = g_peq_handle.advanced_passthrough_post_peq_enable;
    payload.advanced_passthrough_post_peq_sound_mode = g_peq_handle.advanced_passthrough_post_peq_sound_mode;
    payload.nvkey_change_mask = g_nvkey_change_mask;
    if (flash_memory_write_nvdm_data(NVKEY_DSP_PARA_PEQ_MISC_PARA, (uint8_t *)&payload, sizeof(peq_nvdm_misc_param_t)) != NVDM_STATUS_NAT_OK) {
        audio_src_srv_err("[Sink][AM]Write PEQ sound mode error\n",0);
    }
    audio_src_srv_report("[Sink][AM] PEQ sound mode write nvkey succeed", 0);
}

static sysram_status_t aud_peq_put_nvkey(uint8_t *pDstBuf, uint32_t *buffer_size, uint16_t keyid, uint8_t *pSrcBuf)
{
    sysram_status_t status;
    uint16_t chksum = 0;
    nat_nvdm_info_t *p_nat_nvdm_info;
    uint8_t *pCoef;
    int32_t j;

    if(pDstBuf == NULL || buffer_size == NULL || keyid == 0) {
        return NVDM_STATUS_ERROR;
    }

    p_nat_nvdm_info = (nat_nvdm_info_t *)pDstBuf;
    p_nat_nvdm_info->offset = sizeof(nat_nvdm_info_t);
    *buffer_size -= sizeof(nat_nvdm_info_t);

    pCoef = pDstBuf + p_nat_nvdm_info->offset;
    if (pSrcBuf != NULL) {
        memcpy(pCoef, pSrcBuf, *buffer_size);
    } else {
        status = flash_memory_read_nvdm_data(keyid, pCoef, buffer_size);
        if (status != NVDM_STATUS_NAT_OK) {
            return status;
        }
    }

    for (j = chksum = 0 ; j < *buffer_size ; j++) {
        chksum += (uint16_t)(*pCoef++);
    }

    p_nat_nvdm_info->nvdm_id = keyid;
    p_nat_nvdm_info->length = *buffer_size;
    p_nat_nvdm_info->chksum = chksum;

    //audio_src_srv_report("[Sink][AM] aud_peq_put_nvkey [DstBuf]0x%x [SrcBuf]0x%x [keyid]0x%x [length]%d [chksum]%d [offset]%d\n", 6, pDstBuf, pSrcBuf, p_nat_nvdm_info->nvdm_id, p_nat_nvdm_info->length, p_nat_nvdm_info->chksum, p_nat_nvdm_info->offset);
    return NVDM_STATUS_NAT_OK;
}

int32_t aud_peq_realtime_update(mcu2dsp_peq_param_t *peq_param, bt_sink_srv_am_peq_param_t *ami_peq_param, peq_audio_path_id_t audio_path)
{
    sysram_status_t status = NVDM_STATUS_NAT_OK;
    uint32_t nvkey_buf_len = ((uint32_t)ami_peq_param->u2ParamSize) + sizeof(nat_nvdm_info_t);
    uint8_t *nvkey_buf = NULL;

    if(ami_peq_param->u2ParamSize == 0 || ami_peq_param->pu2Param == NULL)
    {
        audio_src_srv_report("[Sink][AM]Set realtime PEQ error, invalid param for realtime mode: 0x%x 0x%x\n",2,ami_peq_param->u2ParamSize, ami_peq_param->pu2Param);
        audio_src_srv_report("[AM]invalid PEQ param for realtime mode", 0);
        configASSERT(0);
        return -1;
    }

    nvkey_buf = (uint8_t *)pvPortMallocNC(nvkey_buf_len);
    if(nvkey_buf != NULL){
        memset(nvkey_buf, 0, nvkey_buf_len);
        status = aud_peq_put_nvkey(nvkey_buf, &nvkey_buf_len, NVKEY_DSP_PARA_PEQ, (uint8_t *)ami_peq_param->pu2Param);
        if(status == NVDM_STATUS_NAT_OK) {
            void *p_param_share;
            peq_param->peq_nvkey_id = NVKEY_DSP_PARA_PEQ;
            peq_param->drc_enable = 1;
            peq_param->setting_mode = ami_peq_param->setting_mode;
            peq_param->target_bt_clk = ami_peq_param->target_bt_clk;
            peq_param->phase_id = ami_peq_param->phase_id;
            peq_param->nvkey_addr = (uint8_t *)hal_memview_cm4_to_infrasys((uint32_t)nvkey_buf);

            audio_src_srv_report("[Sink][AM]Set PEQ realtime update phase_id:%d nvkeyID:0x%x setting_mode:%d target_bt_clk:%d\n",4,peq_param->phase_id,peq_param->peq_nvkey_id,peq_param->setting_mode,peq_param->target_bt_clk);
            p_param_share = hal_audio_dsp_controller_put_paramter(peq_param, sizeof(mcu2dsp_peq_param_t), AUDIO_MESSAGE_TYPE_COMMON);

            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_PEQ_SET_PARAM, audio_path, (uint32_t)p_param_share, true);
            LOG_W(MPLOG, "PEQ phase:%d enable:%d realtime", peq_param->phase_id, (peq_param->peq_nvkey_id == 0) ? 0 : 1);
        } else {
            audio_src_srv_err("[Sink][AM]Set PEQ param error, write to sysram error for realtime mode: %d\n",1,status);
        }
    } else {
        audio_src_srv_err("[Sink][AM]Set realtime PEQ error, allocate NC memory fail, length:0x%x\n",1,nvkey_buf_len);
        status = -1;
    }

    if(ami_peq_param->peq_notify_cb != NULL) {
        ami_peq_param->peq_notify_cb(ami_peq_param->pu2Param);
    }

    vPortFree(ami_peq_param->pu2Param);
    ami_peq_param->pu2Param = NULL;
    if (nvkey_buf != NULL) {
        vPortFreeNC(nvkey_buf);
    }

    return status;
}

static peq_audio_path_id_t aud_get_peq_audio_path(bt_sink_srv_am_type_t type)
{
    peq_audio_path_id_t peq_audio_path = PEQ_AUDIO_PATH_A2DP;
    switch (type)
    {
        case A2DP:
        case AWS:
            peq_audio_path = PEQ_AUDIO_PATH_A2DP;
            break;
        default:
            break;
    }
    return peq_audio_path;
}

static uint32_t aud_peq_query_full_set_size(peq_audio_path_id_t audio_path, uint16_t *nvkey_id, uint32_t *nvkey_size)
{
    sysram_status_t status = NVDM_STATUS_NAT_OK;
    peq_audio_path_table_t *audio_path_tbl;
    uint32_t audio_path_tbl_size;
    uint32_t full_set_size;
    uint16_t keyid = 0;
    uint32_t i;
    uint32_t ret = -1;

#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
    if (audio_path == PEQ_AUDIO_PATH_ADVANCED_PASSTHROUGH) {
        keyid = NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_PRE_PEQ_TABLE;
        status = flash_memory_query_nvdm_data_length(keyid, &full_set_size);
        if(status == NVDM_STATUS_NAT_OK) {
            if (nvkey_id != NULL) {
                *nvkey_id = keyid;
            }
            if (nvkey_size != NULL) {
                *nvkey_size = full_set_size;
            }
            ret = 0;
        } else {
            audio_src_srv_err("[Sink][AM]Query PEQ full set size error, error:%d, keyid:0x%x\n",2,status,keyid);
        }
        return ret;
    }
#endif
    /*Get audio path table*/
    status = flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_PEQ, &audio_path_tbl_size);
    if(status != NVDM_STATUS_NAT_OK) {
        audio_src_srv_err("[Sink][AM]Query PEQ audio table size error, error:%d, keyid:0x%x\n",2,status,NVKEY_DSP_PARA_PEQ);
        return -1;
    }
    audio_path_tbl = (peq_audio_path_table_t *)pvPortMalloc(audio_path_tbl_size);
    if (audio_path_tbl != NULL) {
        status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_PEQ, (uint8_t *)audio_path_tbl, &audio_path_tbl_size);
        if(status == NVDM_STATUS_NAT_OK) {
            /*Get peq full set nvkey and its size for audio_path*/
            for(i=0; i<audio_path_tbl->numOfAudioPath; i++)
            {
                if(audio_path_tbl->audioPathList[i].audioPathID == audio_path) {
                    keyid = audio_path_tbl->audioPathList[i].nvkeyID;
                    break;
                }
            }
            if (keyid != 0) {
                status = flash_memory_query_nvdm_data_length(keyid, &full_set_size);
                if(status == NVDM_STATUS_NAT_OK) {
                    if (nvkey_id != NULL) {
                        *nvkey_id = keyid;
                    }
                    if (nvkey_size != NULL) {
                        *nvkey_size = full_set_size;
                    }
                    ret = 0;
                } else {
                    audio_src_srv_err("[Sink][AM]Query PEQ full set size error, error:%d, keyid:0x%x\n",2,status,keyid);
                }
            } else {
                audio_src_srv_err("[Sink][AM]Parse PEQ nvkey error, can't find audio path:%d in keyid:0x%x\n",2,audio_path,NVKEY_DSP_PARA_PEQ);
            }
        } else {
            audio_src_srv_err("[Sink][AM]Parse PEQ nvkey error, read nvdm from flash error:%d, keyid:0x%x\n",2,status,NVKEY_DSP_PARA_PEQ);
        }
        vPortFree(audio_path_tbl);
    } else {
        audio_src_srv_err("[Sink][AM]Query PEQ full set size malloc fail, size:%d\n",1,audio_path_tbl_size);
    }
    return ret;
}

static int32_t aud_peq_find_sound_mode(peq_audio_path_id_t audio_path, bt_sink_srv_am_peq_param_t *ami_peq_param)
{
    peq_full_set_t *full_set = NULL;
    peq_single_set_t *single_set;
    uint32_t i, j;
    uint16_t target_nvkey_id = ami_peq_param->u2ParamSize;
    uint16_t full_set_nvkey_id;
    uint32_t full_set_size;

    if (aud_peq_query_full_set_size(audio_path, &full_set_nvkey_id, &full_set_size) == 0) {
        full_set = (peq_full_set_t *)pvPortMalloc(full_set_size);
        if(full_set == NULL) {
            audio_src_srv_err("[Sink][AM]Find PEQ sound mode error, pvPortMalloc failed.\n", 0);
            return -1;
        } else {
            sysram_status_t status = flash_memory_read_nvdm_data(full_set_nvkey_id, (uint8_t *)full_set, &full_set_size);
            if(status != NVDM_STATUS_NAT_OK) {
                vPortFree(full_set);
                audio_src_srv_err("[Sink][AM]set PEQ param error, read nvdm from flash error:%d, keyid:0x%x\n",2,status,full_set_nvkey_id);
                return -1;
            }
        }
    } else {
        return -1;
    }

    if(full_set != NULL)
    {
        single_set = (peq_single_set_t *)full_set->setList;
        for(i = 0; i < full_set->numOfSet; i++)
        {
            peq_single_phase_t *single_phase = single_set->phaseList;
            for(j = 0; j < single_set->numOfPhase; j++)
            {
                if((single_phase->phaseID == ami_peq_param->phase_id) && (single_phase->nvkeyID == target_nvkey_id))
                {
                    vPortFree(full_set);
                    ami_peq_param->sound_mode = (1+i);
                    return 0;
                }
            }
            single_set = (peq_single_set_t *)((uint8_t *)single_set + (sizeof(peq_single_phase_t)*single_set->numOfPhase + 2));
        }
        vPortFree(full_set);
        audio_src_srv_err("[Sink][AM]Find PEQ sound mode error, can't find nvkey id:%d in audio path:%d\n",2,target_nvkey_id,audio_path);
    }

    audio_src_srv_err("[Sink][AM]Find PEQ sound mode error, no such nvkey id: 0x%x\n",1,target_nvkey_id);
    return -1;
}

static int32_t aud_set_peq_param(peq_audio_path_id_t audio_path, bt_sink_srv_am_peq_param_t *ami_peq_param)
{
    sysram_status_t status = NVDM_STATUS_NAT_OK;
    uint8_t sound_mode = ami_peq_param->sound_mode;
    mcu2dsp_peq_param_t peq_param;
    void *p_param_share;
    uint32_t nvkey_buf_len = 0;
    uint8_t *nvkey_buf = NULL;

    memset(&peq_param, 0, sizeof(mcu2dsp_peq_param_t));

    peq_param.drc_force_disable = 0;
    if (sound_mode == PEQ_SOUND_MODE_FORCE_DRC)
    {
        if (ami_peq_param->enable == 0) {
            peq_param.peq_nvkey_id = 0;
            peq_param.drc_enable = 0;
            peq_param.setting_mode = ami_peq_param->setting_mode;
            peq_param.target_bt_clk = ami_peq_param->target_bt_clk;
            peq_param.phase_id = ami_peq_param->phase_id;
            peq_param.drc_force_disable = 1;
        } else {
            sound_mode = PEQ_SOUND_MODE_UNASSIGNED;
        }
    }

    if (ami_peq_param->enable == 0)
    {
        peq_param.peq_nvkey_id = 0;
        peq_param.drc_enable = 0;
        peq_param.setting_mode = ami_peq_param->setting_mode;
        peq_param.target_bt_clk = ami_peq_param->target_bt_clk;
        peq_param.phase_id = ami_peq_param->phase_id;
    }
    else if (sound_mode == PEQ_SOUND_MODE_REAlTIME)
    {
        return aud_peq_realtime_update(&peq_param, ami_peq_param, audio_path);
    }
    else if (sound_mode == PEQ_SOUND_MODE_SAVE)
    {
        return aud_peq_find_sound_mode(audio_path, ami_peq_param);
    }
    else
    {
        peq_full_set_t *full_set = NULL;
        peq_single_set_t *single_set;
        uint16_t keyid = 0;
        uint16_t full_set_nvkey_id;
        uint32_t full_set_size;
        uint32_t i;

#ifdef MTK_DEQ_ENABLE
        if (ami_peq_param->phase_id == 2) {
            if (sound_mode == DEQ_AUDIO_SOUND_MODE) {
                keyid = DEQ_AUDIO_NVKEY;
            } else {
                audio_src_srv_err("[Sink][AM]Un-supported sound mode :%d for DEQ.\n", 1, sound_mode);
                status = -1;
                goto __EXIT;
            }
        } else
#endif
        {
            if(sound_mode == PEQ_SOUND_MODE_UNASSIGNED) {
                sound_mode = (ami_peq_param->phase_id == 0) ? g_peq_handle.a2dp_pre_peq_sound_mode : g_peq_handle.a2dp_post_peq_sound_mode;
                ami_peq_param->sound_mode = sound_mode;
            }

            if (aud_peq_query_full_set_size(audio_path, &full_set_nvkey_id, &full_set_size) == 0) {
                full_set = (peq_full_set_t *)pvPortMalloc(full_set_size);
                if(full_set == NULL) {
                    audio_src_srv_err("[Sink][AM]set PEQ param error, pvPortMalloc failed with size:%d\n", 1, full_set_size);
                    status = -1;
                    goto __EXIT;
                }
                status = flash_memory_read_nvdm_data(full_set_nvkey_id, (uint8_t *)full_set, &full_set_size);
                if(status != NVDM_STATUS_NAT_OK) {
                    audio_src_srv_err("[Sink][AM]set PEQ param error, read nvdm from flash error:%d, keyid:0x%x\n",2,status,full_set_nvkey_id);
                    status = -1;
                    goto __EXIT;
                }
            } else {
                status = -1;
                goto __EXIT;
            }

            if(sound_mode > full_set->numOfSet)
            {
                audio_src_srv_err("[Sink][AM]Set PEQ param error, invalid sound mode:%d %d\n",2,sound_mode,full_set->numOfSet);
                status = -1;
                goto __EXIT;
            }

            single_set = (peq_single_set_t *)full_set->setList;
            i = sound_mode-1;
            while(i != 0)
            {
                single_set = (peq_single_set_t *)((uint8_t *)single_set + (sizeof(peq_single_phase_t)*single_set->numOfPhase + 2));
                i--;
            }

            keyid = 0;
            for(i=0; i<single_set->numOfPhase; i++)
            {
                if(single_set->phaseList[i].phaseID == ami_peq_param->phase_id) {
                    keyid = single_set->phaseList[i].nvkeyID;
                    break;
                }
            }
            if(keyid == 0)
            {
                audio_src_srv_err("[Sink][AM]Set PEQ param error, can't find phase_id(%d) error, keyid:0x%x\n",2,ami_peq_param->phase_id,keyid);
                status = -1;
                goto __EXIT;
            }
        }

        status = flash_memory_query_nvdm_data_length(keyid, &nvkey_buf_len);
        if (status != NVDM_STATUS_NAT_OK) {
            audio_src_srv_err("[Sink][AM]Set PEQ param error, query nvkey length error:%d, keyid:0x%x\n",2,status,keyid);
            goto __EXIT;
        }
        nvkey_buf_len += sizeof(nat_nvdm_info_t);
        nvkey_buf = (uint8_t *)pvPortMallocNC(nvkey_buf_len);
        if(nvkey_buf != NULL){
            memset(nvkey_buf, 0, nvkey_buf_len);
        } else {
            audio_src_srv_err("[Sink][AM]Set PEQ param error, allocate NC memory fail, keyid:0x%x, length:0x%x\n",2,keyid,nvkey_buf_len);
            status = -1;
            goto __EXIT;
        }

        status = aud_peq_put_nvkey(nvkey_buf, &nvkey_buf_len, keyid, NULL);
        if(status != NVDM_STATUS_NAT_OK)
        {
            audio_src_srv_err("[Sink][AM]Set PEQ param error, write to sysram error:%d, keyid:0x%x\n",2,status,keyid);
            goto __EXIT;
        }

        peq_param.peq_nvkey_id = keyid;
        peq_param.drc_enable = 1;
        peq_param.setting_mode = ami_peq_param->setting_mode;
        peq_param.target_bt_clk = ami_peq_param->target_bt_clk;
        peq_param.phase_id = ami_peq_param->phase_id;
        peq_param.nvkey_addr = (uint8_t *)hal_memview_cm4_to_infrasys((uint32_t)nvkey_buf);

__EXIT:
        if (full_set != NULL) {
            vPortFree(full_set);
        }
        if(status != 0) {
            if (nvkey_buf != NULL) {
                vPortFreeNC(nvkey_buf);
            }
            return status;
        }
    }

    audio_src_srv_report("[Sink][AM]Set PEQ phase_id:%d enable:%d, sound mode:%d, nvkeyID:%d setting_mode:%d target_bt_clk:%d\n",6,ami_peq_param->phase_id,ami_peq_param->enable,sound_mode,peq_param.peq_nvkey_id,ami_peq_param->setting_mode,ami_peq_param->target_bt_clk);
    p_param_share = hal_audio_dsp_controller_put_paramter( &peq_param, sizeof(mcu2dsp_peq_param_t), AUDIO_MESSAGE_TYPE_COMMON);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_PEQ_SET_PARAM, audio_path, (uint32_t)p_param_share, true);
    LOG_W(MPLOG, "PEQ phase:%d enable:%d nvkey:0x%x", peq_param.phase_id, (peq_param.peq_nvkey_id == 0) ? 0 : 1, peq_param.peq_nvkey_id);

    if (nvkey_buf != NULL) {
        vPortFreeNC(nvkey_buf);
    }
    return status;
}

int32_t aud_set_special_peq_param(uint8_t special_peq_audio_id, uint16_t keyid)
{
    sysram_status_t status = NVDM_STATUS_NAT_OK;
    mcu2dsp_peq_param_t peq_param;
    void *p_param_share;
    uint32_t nvkey_buf_len = 0;
    uint8_t *nvkey_buf = NULL;

    memset(&peq_param, 0, sizeof(mcu2dsp_peq_param_t));

    status = flash_memory_query_nvdm_data_length(keyid, &nvkey_buf_len);
    if (status != NVDM_STATUS_NAT_OK) {
        audio_src_srv_err("[Sink][AM]Set PEQ param error, query nvkey length error:%d, keyid:0x%x\n",2,status,keyid);
        goto __EXIT;
    }
    nvkey_buf_len += sizeof(nat_nvdm_info_t);
    nvkey_buf = (uint8_t *)pvPortMallocNC(nvkey_buf_len);
    if(nvkey_buf != NULL){
        memset(nvkey_buf, 0, nvkey_buf_len);
    } else {
        audio_src_srv_err("[Sink][AM]Set PEQ param error, allocate NC memory fail, keyid:0x%x, length:0x%x\n",2,keyid,nvkey_buf_len);
        status = -1;
        goto __EXIT;
    }

    status = aud_peq_put_nvkey(nvkey_buf, &nvkey_buf_len, keyid, NULL);
    if(status != NVDM_STATUS_NAT_OK)
    {
        audio_src_srv_err("[Sink][AM]Set PEQ param error, write to sysram error:%d, keyid:0x%x\n",2,status,keyid);
        goto __EXIT;
    }

    peq_param.peq_nvkey_id = keyid;
    peq_param.drc_enable = 1;
    peq_param.setting_mode = PEQ_DIRECT;
    peq_param.target_bt_clk = 0;
    peq_param.phase_id = 0;
    peq_param.nvkey_addr = (uint8_t *)hal_memview_cm4_to_infrasys((uint32_t)nvkey_buf);

__EXIT:
    if(status != 0) {
        if (nvkey_buf != NULL) {
            vPortFreeNC(nvkey_buf);
        }
        return status;
    }

    audio_src_srv_report("[Sink][AM]Set PEQ phase_id:%d nvkeyID:%d \n",2,peq_param.phase_id,peq_param.peq_nvkey_id);
    p_param_share = hal_audio_dsp_controller_put_paramter( &peq_param, sizeof(mcu2dsp_peq_param_t), AUDIO_MESSAGE_TYPE_COMMON);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_PEQ_SET_PARAM, 3, (uint32_t)p_param_share, true);
    LOG_W(MPLOG, "PEQ phase:%d enable:%d nvkey:0x%x", peq_param.phase_id, (peq_param.peq_nvkey_id == 0) ? 0 : 1, peq_param.peq_nvkey_id);

    if (nvkey_buf != NULL) {
        vPortFreeNC(nvkey_buf);
    }
    return status;
}


#ifdef SUPPORT_PEQ_NVKEY_UPDATE
static uint16_t aud_get_nvkey_id_by_bit(uint32_t bit)
{
    if (bit < 7) {
        return (NVKEY_DSP_PARA_PEQ + bit);
    } else if (bit < 12) {
        return (NVKEY_DSP_PARA_PEQ_COEF_29 + bit - 8);
    } else if (bit < 16) {
        return (NVKEY_PEQ_UI_DATA_01 + bit - 12);
    } else {
        return 0;
    }
}

static uint32_t aud_get_bit_by_nvkey_id(uint16_t nvkey_id)
{
    if ((nvkey_id >= NVKEY_DSP_PARA_PEQ) && (nvkey_id <= NVKEY_DSP_PARA_PEQ_PATH_5)) {
        return (nvkey_id - NVKEY_DSP_PARA_PEQ);
    } else if ((nvkey_id >= NVKEY_DSP_PARA_PEQ_COEF_29) && (nvkey_id <= NVKEY_DSP_PARA_PEQ_COEF_32)) {
        return (nvkey_id - NVKEY_DSP_PARA_PEQ_COEF_29 + 8);
    } else if ((nvkey_id >= NVKEY_PEQ_UI_DATA_01) && (nvkey_id <= NVKEY_PEQ_UI_DATA_04)) {
        return (nvkey_id - NVKEY_PEQ_UI_DATA_01 + 12);
    } else {
        return 0xFFFFFFFF;
    }
}

void aud_peq_chk_nvkey(uint16_t nvkey_id)
{
    if ((bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) && (bt_sink_srv_cm_get_special_aws_device() != NULL) && (bt_sink_srv_cm_get_aws_link_state() != BT_AWS_MCE_AGENT_STATE_ATTACHED)) {
        uint32_t changed_bit = aud_get_bit_by_nvkey_id(nvkey_id);
        if (changed_bit != 0xFFFFFFFF) {
            g_nvkey_change_mask |= (1 << changed_bit);
        }
    }
}

uint32_t aud_peq_get_changed_nvkey(uint8_t **packet, uint32_t *total_size)
{
    AMI_AWS_MCE_ATTACH_NVDM_PACKT_t *attach_nvdm_pkt;
    ami_aws_mce_attach_peq_nvdm_t *peq_nvdm;
    uint32_t temp_mask = (uint32_t)g_nvkey_change_mask;
    uint32_t bit = 0;
    uint32_t num = 0;
    uint16_t nvkey_array[16];
    uint32_t length;
    uint8_t *payload;

    *packet = NULL;
    *total_size = 0;

    while (temp_mask) {
        if (temp_mask & (1 << bit)) {
            temp_mask &= (~(1 << bit));
            nvkey_array[num] = aud_get_nvkey_id_by_bit(bit);
            if(nvkey_array[num] != 0) {
                num++;
            }
        }
        bit++;
    }
    if (num == 0) {
        attach_nvdm_pkt = (AMI_AWS_MCE_ATTACH_NVDM_PACKT_t *)pvPortMalloc(sizeof(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t));
        if (attach_nvdm_pkt) {
            memset(attach_nvdm_pkt, 0, sizeof(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t));
            *packet = (uint8_t *)attach_nvdm_pkt;
            *total_size = sizeof(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t);
        }
        return 0;
    }

    temp_mask = sizeof(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t);
    for (bit = 0; bit < num; bit++)
    {
        if (flash_memory_query_nvdm_data_length(nvkey_array[bit], &length) == NVDM_STATUS_NAT_OK) {
            temp_mask += length;
        } else {
            audio_src_srv_err("[mce_ami] aud_peq_get_changed_nvkey error for get nvdm length : nvkey:0x%x \n",1,nvkey_array[bit]);
            nvkey_array[bit]= 0;
        }
    }

    payload = (uint8_t *)pvPortMalloc(temp_mask);
    if (payload) {
        attach_nvdm_pkt = (AMI_AWS_MCE_ATTACH_NVDM_PACKT_t *)payload;
        payload = (uint8_t *)attach_nvdm_pkt + sizeof(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t);
        temp_mask -= sizeof(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t);
        *packet = (uint8_t *)attach_nvdm_pkt;
        *total_size = sizeof(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t);
        attach_nvdm_pkt->peq_nvkey_num = 0;
        attach_nvdm_pkt->peq_nvkey_slot_num = num;
        attach_nvdm_pkt->peq_nvkey_mask = g_nvkey_change_mask;
        for (bit = 0; bit < num; bit++) {
            if (nvkey_array[bit] != 0) {
                length = temp_mask;
                if (flash_memory_read_nvdm_data(nvkey_array[bit], payload, &length) == NVDM_STATUS_NAT_OK) {
                    peq_nvdm = &attach_nvdm_pkt->peq_nvdm[attach_nvdm_pkt->peq_nvkey_num];
                    peq_nvdm->nvkey_id = nvkey_array[bit];
                    peq_nvdm->nvkey_length = (uint16_t)length;
                    attach_nvdm_pkt->peq_nvkey_num++;
                    payload += length;
                    temp_mask -= length;
                    *total_size += length;
                } else {
                    audio_src_srv_report("[mce_ami] get changed nvkey, read nvdm error, keyid:0x%x buffer_remain_size:%d \n",2,nvkey_array[bit],length);
                }
            }
        }
        audio_src_srv_report("[mce_ami] send attach nvdm info: peq mask:%x/%x %d %d\n",4,attach_nvdm_pkt->peq_nvkey_mask,g_nvkey_change_mask,attach_nvdm_pkt->peq_nvkey_num,*total_size);
        g_nvkey_change_mask = 0;
        return 0;
    }

    audio_src_srv_err("[mce_ami] aud_peq_get_changed_nvkey error for malloc, size:%d\n",1,temp_mask);
    return -1;
}

uint32_t aud_peq_save_changed_nvkey(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t *attach_nvdm_pkt, uint32_t data_size)
{
    ami_aws_mce_attach_peq_nvdm_t *peq_nvdm;
    uint32_t num;
    uint8_t *payload;
    uint32_t used_size;
    uint32_t i;

    if ((attach_nvdm_pkt == NULL) || (data_size == 0) || (attach_nvdm_pkt->peq_nvkey_num > PEQ_ATTACH_NVKEY_MAX) || (attach_nvdm_pkt->peq_nvkey_slot_num > PEQ_ATTACH_NVKEY_MAX)) {
        audio_src_srv_err("[mce_ami] aud_peq_save_changed_nvkey error for abnormal param\n",0);
        return -1;
    }

    num = attach_nvdm_pkt->peq_nvkey_num;
    used_size = sizeof(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t);
    payload = (uint8_t *)attach_nvdm_pkt + used_size;

    for (i = 0; i < num; i++)
    {
        peq_nvdm = &attach_nvdm_pkt->peq_nvdm[i];
        if (used_size + peq_nvdm->nvkey_length > data_size) {
            audio_src_srv_err("[mce_ami] aud_peq_save_changed_nvkey error for abnormal data size:%d, used size:%d, next_nvkey_length:%d\n",3,data_size,used_size,peq_nvdm->nvkey_length);
            return -1;
        }
        if (flash_memory_write_nvdm_data(peq_nvdm->nvkey_id, payload, peq_nvdm->nvkey_length) != NVDM_STATUS_NAT_OK) {
            audio_src_srv_err("[mce_ami] aud_peq_save_changed_nvkey error for save nvkey:0x%x length:%d\n",2,peq_nvdm->nvkey_id,peq_nvdm->nvkey_length);
        } else {
            uint32_t changed_bit = aud_get_bit_by_nvkey_id(peq_nvdm->nvkey_id);
            audio_src_srv_report("[mce_ami] update nvkey:0x%x length:%d success\n",2,peq_nvdm->nvkey_id,peq_nvdm->nvkey_length);
            if (changed_bit != 0xFFFFFFFF) {
                g_nvkey_change_mask &= (~(1 << changed_bit));
            }
        }
        used_size += peq_nvdm->nvkey_length;
        payload += peq_nvdm->nvkey_length;
    }
    if (num > 0) {
        return 1;
    }
    return 0;
}
#endif

uint32_t aud_peq_get_sound_mode(bt_sink_srv_am_type_t type, uint8_t *peq_info)
{
    uint32_t ret = -1;
    if ((type == A2DP) || (type == AWS)) {
        peq_info[0] = g_peq_handle.a2dp_pre_peq_enable;
        peq_info[1] = g_peq_handle.a2dp_pre_peq_sound_mode;
        peq_info[2] = g_peq_handle.a2dp_post_peq_enable;
        peq_info[3] = g_peq_handle.a2dp_post_peq_sound_mode;
        ret = 0;
    } else if (type == LINE_IN) {
        peq_info[0] = g_peq_handle.linein_pre_peq_enable;
        peq_info[1] = g_peq_handle.linein_pre_peq_sound_mode;
        peq_info[2] = g_peq_handle.linein_post_peq_enable;
        peq_info[3] = g_peq_handle.linein_post_peq_sound_mode;
        ret = 0;
    }
    return ret;
}

uint32_t aud_peq_reinit_nvdm(void)
{
    #define CHECK_KEEP(nvkey_id) ((nvkey_id >= NVKEY_DSP_PARA_PEQ_PATH_0) && (nvkey_id < NVKEY_DSP_PARA_PEQ_COEF_29)) ? 1 : 0
    sysram_status_t status;
    uint32_t ret = -1;
    uint8_t T_NVDM_F232[] =
    {
        0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x01,0x01,0x01,0x00,0x01,0x00,0x00,
    };
    peq_full_set_t *full_set = NULL, *full_set2 = NULL;
    peq_single_set_t *single_set, *single_set2;
    peq_single_phase_t *single_phase, *single_phase2;
    uint32_t i, j;
    uint16_t full_set_nvkey_id;
    uint32_t full_set_size;

    status = flash_memory_write_nvdm_data(NVKEY_DSP_PARA_PEQ_MISC_PARA, (uint8_t *)T_NVDM_F232, sizeof(T_NVDM_F232));
    if(status != NVDM_STATUS_NAT_OK) {
        audio_src_srv_err("[Sink][AM] peq failed to reinit 0x%x from nvdm - err(%d)\r\n", 2, NVKEY_DSP_PARA_PEQ_MISC_PARA,status);
    }

    if (aud_peq_query_full_set_size(PEQ_AUDIO_PATH_A2DP, &full_set_nvkey_id, &full_set_size) == 0) {
        full_set = (peq_full_set_t *)pvPortMalloc(full_set_size);
        full_set2 = (peq_full_set_t*)pvPortMalloc(full_set_size);
        if((full_set == NULL) || (full_set2 == NULL)) {
            audio_src_srv_err("[Sink][AM] aud_peq_reinit_nvdm, pvPortMalloc failed.\n", 0);
            goto __END;
        } else {
            memset(full_set, 0, full_set_size);
            memset(full_set2, 0, full_set_size);
            status = flash_memory_read_nvdm_data(full_set_nvkey_id, (uint8_t *)full_set, &full_set_size);
            if(status != NVDM_STATUS_NAT_OK) {
                audio_src_srv_err("[Sink][AM]aud_peq_reinit_nvdm, read nvdm from flash error:%d, keyid:0x%x\n",2,status,full_set_nvkey_id);
                goto __END;
            }
        }
    } else {
        goto __END;
    }

    single_set = &full_set->setList[0];
    single_set2 = &full_set2->setList[0];
    full_set2->numOfSet = 0;
    full_set_size = sizeof(uint16_t);
    for(i = 0; i < full_set->numOfSet; i++)
    {
        single_phase = &single_set->phaseList[0];
        single_phase2 = &single_set2->phaseList[0];
        single_set2->numOfPhase = 0;
        for(j = 0; j < single_set->numOfPhase; j++)
        {
            if (CHECK_KEEP(single_phase->nvkeyID)) {
                single_phase2->phaseID = single_phase->phaseID;
                single_phase2->nvkeyID = single_phase->nvkeyID;
                single_set2->numOfPhase++;
                single_phase2++;
                full_set_size += sizeof(peq_single_phase_t);
            }
            single_phase++;
        }
        if (single_set2->numOfPhase > 0) {
            full_set2->numOfSet++;
            full_set_size += sizeof(uint16_t);
        } else {
            break;
        }
        single_set = (peq_single_set_t *)((uint32_t)single_set + sizeof(peq_single_phase_t)*single_set->numOfPhase + sizeof(uint16_t));
        single_set2 = (peq_single_set_t *)((uint32_t)single_set2 + sizeof(peq_single_phase_t)*single_set2->numOfPhase + sizeof(uint16_t));
    }

    status = flash_memory_write_nvdm_data(full_set_nvkey_id, (uint8_t *)full_set2, full_set_size);
    if(status != NVDM_STATUS_NAT_OK) {
        audio_src_srv_err("[Sink][AM] peq failed to reinit 0x%x from nvdm - err(%d)\r\n", 2, full_set_nvkey_id,status);
        goto __END;
    }

    ret = 0;

__END:
    vPortFree(full_set);
    vPortFree(full_set2);
    return ret;
}

uint8_t aud_peq_get_peq_status(peq_audio_path_id_t audio_path, uint8_t phase_id)
{
    uint8_t ret = -1;
    if (audio_path == PEQ_AUDIO_PATH_A2DP) {
        if(phase_id == 0) {
            ret = g_peq_handle.a2dp_pre_peq_enable;
            return ret;
        }
       else if(phase_id == 1) {
            ret = g_peq_handle.a2dp_post_peq_enable;
            return ret;
        }
    } else if (audio_path == PEQ_AUDIO_PATH_LINEIN) {
        if(phase_id == 0) {
            ret = g_peq_handle.linein_pre_peq_enable;
            return ret;
        }
        else if(phase_id == 1) {
            ret = g_peq_handle.linein_post_peq_enable;
            return ret;
        }
    }
    audio_src_srv_err("[Sink][AM]aud_peq_get_peq_status error\n",0);
    return ret;
}

uint8_t aud_peq_get_current_sound_mode(peq_audio_path_id_t audio_path, uint8_t phase_id)
{
    uint8_t ret = -1;
    if (audio_path == PEQ_AUDIO_PATH_A2DP) {
        if(phase_id == 0) {
            ret = g_peq_handle.a2dp_pre_peq_sound_mode;
            return ret;
        }
       else if(phase_id == 1) {
            ret = g_peq_handle.a2dp_post_peq_sound_mode;
            return ret;
        }
    } else if (audio_path == PEQ_AUDIO_PATH_LINEIN) {
        if(phase_id == 0) {
            ret = g_peq_handle.linein_pre_peq_sound_mode;
            return ret;
        }
        else if(phase_id == 1) {
            ret = g_peq_handle.linein_post_peq_sound_mode;
            return ret;
        }
    }
    audio_src_srv_err("[Sink][AM]aud_peq_get_current_sound_mode error\n",0);
    return ret;
}

int32_t aud_peq_get_total_mode(peq_audio_path_id_t audio_path, uint8_t phase_id)
{
    int32_t total_mode = -1;
    if (audio_path == PEQ_AUDIO_PATH_A2DP) {
        peq_full_set_t *full_set = NULL;
        uint16_t full_set_nvkey_id;
        uint32_t full_set_size;
        if (aud_peq_query_full_set_size(audio_path, &full_set_nvkey_id, &full_set_size) == 0) {
            full_set = (peq_full_set_t *)pvPortMalloc(full_set_size);
            if(full_set == NULL) {
                audio_src_srv_err("[Sink][AM]aud_peq_get_total_mode error, pvPortMalloc failed.\n", 0);
                return -1;
            } else {
                sysram_status_t status = flash_memory_read_nvdm_data(full_set_nvkey_id, (uint8_t *)full_set, &full_set_size);
                if(status == NVDM_STATUS_NAT_OK) {
                    uint32_t i,j;
                    uint16_t *p_cur = (uint16_t *)&full_set->setList[0];
                    uint16_t phase_num;
                    total_mode = 0;
                    for (i = 0; i < full_set->numOfSet; i++) {
                        phase_num = *p_cur;
                        p_cur++;
                        for (j = 0; j < phase_num; j++) {
                            if (*p_cur == phase_id) {
                                total_mode++;
                            }
                            p_cur+=2;
                        }
                    }
                } else {
                    audio_src_srv_err("[Sink][AM]aud_peq_get_total_mode error, read nvdm from flash error:%d, keyid:0x%x\n",2,status,full_set_nvkey_id);
                }
            }
        }
        if(full_set != NULL) {
            vPortFree(full_set);
        }
    }
    audio_src_srv_report("[Sink][AM]aud_peq_get_total_mode, auido_path:%d phase_id:%d total sound mode:%d\n",3,audio_path,phase_id,total_mode);
    return total_mode;
}

#endif

static void aud_prepare_a2dp_nvkey(bt_sink_srv_am_feature_t *local_feature)
{
#if defined(MTK_AVM_DIRECT)
    sysram_status_t status;
    DSP_FEATURE_TYPE_LIST AudioFeatureList_A2DP[] =
    {
#ifdef MTK_PEQ_ENABLE
        FUNC_PEQ_A2DP,
#endif
        FUNC_END,
    };
#ifdef MTK_PEQ_ENABLE
    bt_sink_srv_am_peq_param_t am_peq_param;
#endif

#ifdef MTK_PEQ_ENABLE
    memset(&am_peq_param, 0, sizeof(bt_sink_srv_am_peq_param_t));
    /* set pre PEQ*/
    am_peq_param.phase_id = 0;
    am_peq_param.enable = g_peq_handle.a2dp_pre_peq_enable;
    am_peq_param.sound_mode = g_peq_handle.a2dp_pre_peq_sound_mode;
    aud_set_peq_param(PEQ_AUDIO_PATH_A2DP, &am_peq_param);
#ifndef MTK_ANC_ENABLE
    /* set post PEQ*/
    am_peq_param.phase_id = 1;
    am_peq_param.enable = g_peq_handle.a2dp_post_peq_enable;
    am_peq_param.sound_mode = g_peq_handle.a2dp_post_peq_sound_mode;
    aud_set_peq_param(PEQ_AUDIO_PATH_A2DP, &am_peq_param);
#endif
#endif
    audio_set_anc_compensate(A2DP, 0, NULL);
#ifdef MTK_ANC_ENABLE
    audio_set_anc_compensate_phase2(A2DP, 0);
#endif
    audio_nvdm_reset_sysram();
    status = audio_nvdm_set_feature(sizeof(AudioFeatureList_A2DP)/sizeof(DSP_FEATURE_TYPE_LIST), AudioFeatureList_A2DP);
    if (status != NVDM_STATUS_NAT_OK){
        audio_src_srv_err("[Sink][AM] A2DP open is failed to set nvkey to share memory - err(%d)\r\n", 1, status);
        audio_src_srv_report("[Sink][AM] A2DP open is failed to set nvkey to share memory", 0);
        configASSERT(0);
    }
#if (defined(MTK_ANC_ENABLE) && defined(MTK_LEAKAGE_DETECTION_ENABLE) && (MTK_AP_DEFAULT_TYPE == 2))
#ifndef MTK_ANC_V2
    //workaround for A3 leakage detection
    if (audio_anc_leakage_compensation_get_status()) {
        uint32_t positive_gain[4], positive_gain_size = 16;
        mem_nvdm_info_t mem_nvdm;
        status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_POSITIVE_GAIN, &positive_gain[0], &positive_gain_size);
        if (status != NVDM_STATUS_NAT_OK){
            audio_src_srv_err("[Sink][AM] A2DP open is failed to read NVKEY_DSP_PARA_POSITIVE_GAIN - err(%d)\r\n", 1, status);
            audio_src_srv_report("[Sink][AM] A2DP open is failed to read NVKEY_DSP_PARA_POSITIVE_GAIN", 0);
            configASSERT(0);
        }
        positive_gain[2] += 12; //+6dB
        mem_nvdm.nvdm_id = NVKEY_DSP_PARA_POSITIVE_GAIN;
        mem_nvdm.length = positive_gain_size;
        mem_nvdm.mem_pt = (const void *)&positive_gain[0];
        status = nat_table_write_audio_nvdm_data(mem_nvdm, c_sram_mode);
        if (status != NVDM_STATUS_NAT_OK){
            audio_src_srv_err("[Sink][AM] A2DP open is failed to set NVKEY_DSP_PARA_POSITIVE_GAIN - err(%d)\r\n", 1, status);
            audio_src_srv_report("[Sink][AM] A2DP open is failed to set NVKEY_DSP_PARA_POSITIVE_GAIN", 0);
            configASSERT(0);
        }
    }
#endif
#endif
#endif
}

#if defined(MTK_LINEIN_PEQ_ENABLE) || defined(MTK_LINEIN_INS_ENABLE) || defined(AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE)
#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
#define NVKEYID_F50C            "F50C"
static const uint8_t NVKEY_F50C[] =
{
    0x01, 0x00, 0x00, 0x00, 0x67, 0xF3, 0xFF, 0xFF, 0xC8, 0x00, 0x00, 0x00, 0x67, 0xF3, 0xFF, 0xFF, 0xD0, 0x07, 0x00, 0x00, 0x18, 0xFC, 0xFF, 0xFF, 0x19, 0x00, 0x00, 0x00, 0xE7, 0xFF, 0xFF, 0xFF, 0x91, 0x87, 0x08, 0x00, 0xCB, 0xBF, 0x04, 0x00, 0x90, 0x87, 0x08, 0x00, 0xE8, 0x9F, 0xB8, 0xFF, 0x25, 0x29, 0x32, 0x00, 0x2E, 0xC5, 0x42, 0x00, 0x5F, 0xC7, 0xF3, 0xFF, 0x2D, 0xC5, 0x42, 0x00, 0x29, 0x00, 0xB8, 0xFF, 0x29, 0x36, 0x64, 0x00, 0x00, 0x00, 0x40, 0x00, 0xC9, 0x7E, 0x74, 0x00, 0xCF, 0x84, 0x8B, 0xFF, 0xC8, 0x7E, 0x74, 0x00, 0xA6, 0x23, 0x88, 0xFF, 0x71, 0xA4, 0x70, 0x00, 0xD0, 0xEA, 0x7E, 0x00, 0x16, 0x29, 0x81, 0xFF, 0xCF, 0xEA, 0x7E, 0x00, 0xDF, 0x73, 0x81, 0xFF, 0x85, 0x91, 0x7D, 0x00, 0x00, 0x00, 0x40, 0x00, 0x00, 0x1E, 0x00, 0x06, 0x00, 0xB0, 0x0A, 0x0C, 0x00, 0x00, 0x00, 0x00,
};
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */
static void aud_prepare_linein_nvkey(bt_sink_srv_am_feature_t *local_feature)
{
#if defined(MTK_AVM_DIRECT)
    sysram_status_t status;
    DSP_FEATURE_TYPE_LIST AudioFeatureList_LINEIN[] =
    {
#ifdef MTK_LINEIN_INS_ENABLE
        FUNC_INS,
#endif
#ifdef MTK_LINEIN_PEQ_ENABLE
        FUNC_PEQ_LINEIN,
#endif
#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
        FUNC_GAME_CHAT_VOLUME_SMART_BALANCE,
#endif
        FUNC_END,
    };
#ifdef MTK_LINEIN_PEQ_ENABLE
    bt_sink_srv_am_peq_param_t am_peq_param;
#endif

#ifdef MTK_LINEIN_PEQ_ENABLE
    memset(&am_peq_param, 0, sizeof(bt_sink_srv_am_peq_param_t));
    /* set pre PEQ*/
    am_peq_param.phase_id = 0;
    am_peq_param.enable = g_peq_handle.linein_pre_peq_enable;
    am_peq_param.sound_mode = g_peq_handle.linein_pre_peq_sound_mode;
    aud_set_peq_param(PEQ_AUDIO_PATH_LINEIN, &am_peq_param);
#endif
    audio_nvdm_reset_sysram();
    // status = audio_nvdm_set_feature(sizeof(AudioFeatureList_LINEIN)/sizeof(DSP_FEATURE_TYPE_LIST), AudioFeatureList_LINEIN);
    // if (status != NVDM_STATUS_NAT_OK){
    //     audio_src_srv_err("[Sink][AM] LINEIN open is failed to set nvkey to share memory - err(%d)\r\n", 1, status);
    //     audio_src_srv_report("[Sink][AM] LINEIN open is failed to set nvkey to share memory", 0);
    //     configASSERT(0);
    // }
    status = NVDM_STATUS_ERROR;
    while (status != NVDM_STATUS_NAT_OK)
    {
        /* set NVKEYs that the usb chat stream uses into the share buffer */
        status = audio_nvdm_set_feature(sizeof(AudioFeatureList_LINEIN)/sizeof(DSP_FEATURE_TYPE_LIST), AudioFeatureList_LINEIN);
        if (status != NVDM_STATUS_NAT_OK)
        {
            audio_src_srv_err("[Sink][AM] LINEIN open is failed to set parameters to share memory - err(%d)\r\n", 1, status);

            /* workaround for that maybe this NVKEY_F50C is not exsited after the FOTA */
            if ((nvdm_status_t)status == NVDM_STATUS_ITEM_NOT_FOUND)
            {
                #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
                audio_src_srv_report("[Sink][AM] LINEIN open is set default parameters into the NVKEY\r\n", 0);
                nvdm_status_t nvdm_status = nvdm_write_data_item("AB15", NVKEYID_F50C, NVDM_DATA_ITEM_TYPE_RAW_DATA, (const uint8_t *)NVKEY_F50C, sizeof(NVKEY_F50C));
                if (nvdm_status != NVDM_STATUS_OK)
                {
                    audio_src_srv_report("[Sink][AM] LINEIN open is failed to set default parameters into the NVKEY - err(%d)\r\n", 1, nvdm_status);
                    configASSERT(0);
                }
                #else
                configASSERT(0);
                #endif
            }
            else
            {
                configASSERT(0);
            }
        }
    }
#endif
}
#endif

#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
static void aud_prepare_hearing_aid_nvkey(bt_sink_srv_am_feature_t *local_feature)
{
    bt_sink_srv_am_peq_param_t am_peq_param;
    nvdm_status_t nvdm_status = NVDM_STATUS_OK;
    uint32_t item_size = 0;
    bool afc_enable = false;
    uint8_t *nvkey_buf;

    /* set special PEQ */
    nvdm_status = nvkey_data_item_length(NVKEYID_DSP_PARA_AFC, &item_size);
    if((nvdm_status != NVDM_STATUS_OK) || (item_size == 0)){
        audio_src_srv_report("[Hearing-aid[Warning]NVDM AFC Fail. Status:%d Len:%u\n", 2, nvdm_status, item_size);
        afc_enable = false;
    }
    else
    {
        nvkey_buf = (uint8_t *)pvPortMallocNC(item_size);
        nvdm_status = nvkey_read_data(NVKEYID_DSP_PARA_AFC, nvkey_buf, &item_size);
        if(nvdm_status != NVDM_STATUS_OK)
        {
            audio_src_srv_report("[Hearing-aid[Warning]NVDM AFC Read Fail. Status:%d Len:%u\n", 2, nvdm_status, item_size);
            afc_enable = false;
        }
        else
        {
            if (*nvkey_buf == 0)
            {
                afc_enable = false;
                audio_src_srv_report("[Hearing-aid]NVDM AFC disable.", 0);
            }
            else
            {
                afc_enable = true;
                audio_src_srv_report("[Hearing-aid]NVDM AFC enable.", 0);
            }
        }
        vPortFreeNC(nvkey_buf);
    }
    if (afc_enable != false)
    {
        /* enable special PEQ */
        aud_set_special_peq_param(3, NVKEYID_DSP_PARA_ADVANCED_PASSTHROUGH_SPEACIL_PEQ);
    }
    else
    {
        /* disable special PEQ */
        aud_set_special_peq_param(3, 0);
    }

    memset(&am_peq_param, 0, sizeof(bt_sink_srv_am_peq_param_t));
    /* set pre PEQ*/
    am_peq_param.phase_id = 0;
    am_peq_param.enable     = g_peq_handle.advanced_passthrough_pre_peq_enable;
    am_peq_param.sound_mode = g_peq_handle.advanced_passthrough_pre_peq_sound_mode;
    aud_set_peq_param(PEQ_AUDIO_PATH_ADVANCED_PASSTHROUGH, &am_peq_param);
}
#endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

void aud_stream_out_callback(bt_sink_srv_am_event_result_t event, void *user_data);

void aud_stream_in_callback(bt_sink_srv_am_event_result_t event, void *user_data);
#ifdef MTK_RECORD_ENABLE
static void am_audio_record_sink_play(bt_sink_srv_am_background_t *background_ptr);
static void am_audio_record_sink_stop(void);
#endif

static void aud_dl_control(bool isCodecOpen);
static void aud_ul_control(bool isCodecOpen);
/*****************************************************************************
 * FUNCTION
 *  am_audio_set_play
 * DESCRIPTION
 *  This function is used to play the audio handler.
 * PARAMETERS
 *  background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/

static void am_audio_set_play(bt_sink_srv_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_hal_result_t        eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_srv_am_id_t                bAud_id = background_ptr->aud_id;
    bt_sink_srv_am_audio_stream_out_t  *stream_out = &(background_ptr->audio_stream_out);
    bt_sink_srv_am_audio_stream_in_t   *stream_in = &(background_ptr->audio_stream_in);
    bt_sink_srv_am_stream_node_t       *pcm_stream_node = NULL;
    bt_sink_srv_am_stream_type_t       eIn_out;
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
    bt_sink_srv_audio_tunning_context_t *aud_tunning_p = NULL;
#endif

#ifdef __BT_AWS_SUPPORT__
    bt_aws_role_t aws_role = BT_AWS_ROLE_NONE;
    bt_aws_codec_type_t aws_codec = 0;
    bt_sink_srv_am_a2dp_codec_t am_a2dp_codec = {{0}};
#endif /* __BT_AWS_SUPPORT__ */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    audio_src_srv_report("[Sink][AM]Set Play: type:%d\r\n",1, background_ptr->type);
    if (background_ptr->type != RECORD){
#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
        // Temp solution for audio transmitter, the UL cases should bypass the flag setting
        if(background_ptr->type != AUDIO_TRANSMITTER) {
            ami_set_audio_mask(AM_TASK_MASK_DL1_HAPPENING, true);
        }
#else
        ami_set_audio_mask(AM_TASK_MASK_DL1_HAPPENING, true);
#endif
    }

    if (ptr_callback_amm == NULL) {
        ptr_callback_amm = (bt_sink_srv_am_amm_struct *)pvPortMalloc(sizeof(bt_sink_srv_am_amm_struct));
    }
    if (background_ptr->type == NONE) {
        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
        background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    } else if (background_ptr->type == PCM) {
        pcm_stream_node = &(background_ptr->local_context.pcm_format.stream);
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            audio_src_srv_report("[Sink][AM]register_stream_out_callback: 0x%x\r\n",1, aud_stream_out_callback);
#if defined(MTK_AVM_DIRECT)
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PLAYBACK, true);
#endif
            hal_audio_register_stream_out_callback(aud_stream_out_callback, NULL);
            hal_audio_set_stream_out_sampling_rate(pcm_stream_node->stream_sample_rate);
            hal_audio_set_stream_out_channel_number(pcm_stream_node->stream_channel);
            hal_audio_set_stream_out_device(stream_out->audio_device);
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_PCM;
                vol_info.vol_info.pcm_vol_info.dev = stream_out->audio_device;
                vol_info.vol_info.pcm_vol_info.lev = stream_out->audio_volume;
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
            hal_audio_mute_stream_out(stream_out->audio_mute, HAL_AUDIO_STREAM_OUT1);
#else
            hal_audio_mute_stream_out(stream_out->audio_mute);
#endif
            eResult = hal_audio_write_stream_out(pcm_stream_node->buffer, pcm_stream_node->size);
        } else if (eIn_out == STREAM_IN) {
            #ifndef MTK_AM_NOT_SUPPORT_STREAM_IN
            audio_src_srv_report("[Sink][AM]register_stream_in_callback: 0x%x\r\n",1, aud_stream_in_callback);
#if defined(MTK_AVM_DIRECT)
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true);
#endif
            hal_audio_register_stream_in_callback(aud_stream_in_callback, NULL);
            hal_audio_set_stream_in_sampling_rate(pcm_stream_node->stream_sample_rate);
            hal_audio_set_stream_in_channel_number(pcm_stream_node->stream_channel);
            hal_audio_set_stream_in_device(stream_in->audio_device);
            /* should add this case ?? */
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
            hal_audio_mute_stream_in(stream_in->audio_mute);
            eResult = hal_audio_read_stream_in(pcm_stream_node->buffer, pcm_stream_node->size);
            #endif
        }
        if (eResult == HAL_AUDIO_STATUS_ERROR) {
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
        } else {
            if (eIn_out == STREAM_OUT) {
                hal_audio_start_stream_out(background_ptr->audio_path_type);
            } else if (eIn_out == STREAM_IN) {
                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true);
                hal_audio_start_stream_in(background_ptr->audio_path_type);
            }
            g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
        }
    } else if ( (background_ptr->type == A2DP)
#if defined(MTK_AWS_MCE_ENABLE)
                || (background_ptr->type == AWS)
#endif
              )
    {
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_A2DP_START);
#endif
#if defined(MTK_AVM_DIRECT)
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, true);
#endif

#if defined(AIR_WIRED_AUDIO_ENABLE)

    uint16_t scenario_and_id;

    audio_transmitter_scenario_list_t audio_transmitter_scenario_list[]  =  {
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT},
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT}
    };

    if(true == audio_transmitter_get_is_running_by_scenario_list(audio_transmitter_scenario_list, sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t))) {
        for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
            if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list[i], 1)) {
                scenario_and_id = (audio_transmitter_scenario_list[i].scenario_type << 8) | audio_transmitter_scenario_list[i].scenario_sub_id;
                audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is running, suspended by A2DP\n", 1, scenario_and_id);
                audio_transmitter_playback_stop(scenario_and_id);
                audio_transmitter_playback_close(scenario_and_id);

                bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list[i].scenario_type, audio_transmitter_scenario_list[i].scenario_sub_id);
                if(am_id != -1) {
                    if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
                        g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
                    }
                    if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
                        g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_A2DP, &scenario_and_id);
                    }
                } else {
                    audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
                    configASSERT(0);
                }
                #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
                #endif
                break;
            }
        }
    }
#endif
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_A2DP;
            vol_info.vol_info.a2dp_vol_info.dev = stream_out->audio_device;
            vol_info.vol_info.a2dp_vol_info.lev = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        hal_audio_set_stream_in_device(stream_in->audio_device);
        aud_prepare_a2dp_nvkey(&(background_ptr->local_feature));
        bt_codec_am_a2dp_sink_open(&(background_ptr->local_context.a2dp_format.a2dp_codec));

    } else if (background_ptr->type == BLE) {
#ifdef AIR_BT_CODEC_BLE_ENABLED
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_BLE_START);
#endif
#if defined(MTK_AVM_DIRECT)
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
#ifdef MTK_ANC_ENABLE
#ifndef MTK_ANC_V2
        if(g_anc_control.enable){
            g_am_anc_flag = true;
            anc_off(g_anc_control.anc_off_callback, 0, 0);
            #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_ANC_STOP);
            #endif
        }
#endif
#endif
#endif
    if(background_ptr->local_context.ble_format.ble_codec.channel_mode != CHANNEL_MODE_DL_ONLY) {
        if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)){
        #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_RECORD_STOP);
        #endif
        #ifdef MTK_RECORD_ENABLE
            am_audio_record_sink_stop();
        #else
            hal_audio_stop_stream_in();
        #endif
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
            if(g_prCurrent_playback[2] != NULL){
                g_prCurrent_playback[2]->notify_cb(g_prCurrent_playback[2]->aud_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_LE_CALL, NULL);
                g_rAm_aud_id[g_prCurrent_playback[2]->aud_id].use = ID_IDLE_STATE;
                g_prCurrent_playback[2] = NULL;
                hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_RECORD);
            }
        }
    }
            //Check MIC user with transmitter
#if defined(AIR_WIRED_AUDIO_ENABLE)
    uint16_t scenario_and_id;
    audio_transmitter_scenario_list_t audio_transmitter_scenario_list[]  =  {
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT},
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE},
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT}
    };
    if(true == audio_transmitter_get_is_running_by_scenario_list(audio_transmitter_scenario_list, sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t))) {
        for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
            if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list[i], 1)) {
                scenario_and_id = (audio_transmitter_scenario_list[i].scenario_type << 8) | audio_transmitter_scenario_list[i].scenario_sub_id;
                audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is running, suspended by LE AUDIO CALL\n", 1, scenario_and_id);
                audio_transmitter_playback_stop(scenario_and_id);
                audio_transmitter_playback_close(scenario_and_id);

                bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list[i].scenario_type, audio_transmitter_scenario_list[i].scenario_sub_id);
                if(am_id != -1) {
                    if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
                        g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
                    }
                    if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
                        g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_LE_CALL, &scenario_and_id);
                    }
                } else {
                    audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
                    configASSERT(0);
                }
                #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
                #endif
                break;
            }
        }
    }
#endif
    if (background_ptr->local_context.ble_format.ble_codec.channel_mode == CHANNEL_MODE_DL_ONLY) {
        //use AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL for DL only case
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL, true);
    } else {
        //use AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL for DL/UL both case
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL, true);
    }

#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            if(background_ptr->local_context.ble_format.ble_codec.channel_mode != CHANNEL_MODE_DL_ONLY) {
                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_CVSD;
                vol_info.vol_info.hfp_vol_info.dev_in = stream_in->audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = stream_out->audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = stream_in->audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = stream_out->audio_volume;
            } else {
                vol_info.type = VOL_A2DP;
                vol_info.vol_info.a2dp_vol_info.dev = stream_out->audio_device;
                vol_info.vol_info.a2dp_vol_info.lev = stream_out->audio_volume;
            }
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
        }
#else
#ifdef MTK_VENDOR_VOLUME_TABLE_ENABLE
        background_ptr->vol_type = VOL_HFP;
#endif
        aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
#endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */

        audio_src_srv_report("LE audio stream_out->audio_mute = %d\r\n", 1, stream_out->audio_mute);

#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
        hal_audio_mute_stream_out(stream_out->audio_mute, HAL_AUDIO_STREAM_OUT1);
#else
        hal_audio_mute_stream_out(stream_out->audio_mute);
#endif

#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_CVSD;
            vol_info.vol_info.hfp_vol_info.dev_in = stream_in->audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = stream_out->audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = stream_in->audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
#else
#ifdef MTK_VENDOR_VOLUME_TABLE_ENABLE
        background_ptr->vol_type = VOL_HFP;
#endif
        aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
#endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
#endif
        if (background_ptr->local_context.ble_format.ble_codec.channel_mode == CHANNEL_MODE_DL_ONLY) {
            aud_prepare_a2dp_nvkey(&(background_ptr->local_feature));
        } else if (background_ptr->local_context.ble_format.ble_codec.channel_mode == CHANNEL_MODE_DL_UL_BOTH) {
            if(background_ptr->local_context.ble_format.ble_codec.context_type == AUDIO_CONTENT_TYPE_CONVERSATIONAL){
                DSP_FEATURE_TYPE_LIST AudioFeatureList_LEAudioCall[2] =
                {
                    FUNC_RX_NR,
                    FUNC_END,
                };
                /* reset share buffer before put parameters*/
                audio_nvdm_reset_sysram();
                sysram_status_t status = audio_nvdm_set_feature(2, AudioFeatureList_LEAudioCall);
                if (status != NVDM_STATUS_NAT_OK){
                    audio_src_srv_report("LE audio call failed to set parameters to share memory - err(%d)\r\n", 1, status);
                    configASSERT(0);
                }
         }else{
             DSP_FEATURE_TYPE_LIST AudioFeatureList_LEAudioCall[3] =
             {
                 FUNC_TX_NR_v2,
#ifdef MTK_PEQ_ENABLE
                 FUNC_PEQ_A2DP,
#endif
                 FUNC_END,
             };
                /* reset share buffer before put parameters*/
                audio_nvdm_reset_sysram();
                sysram_status_t status = audio_nvdm_set_feature(2, AudioFeatureList_LEAudioCall);
                if (status != NVDM_STATUS_NAT_OK){
                    audio_src_srv_report("LE audio call failed to set parameters to share memory - err(%d)\r\n", 1, status);
                    configASSERT(0);
                }
            }

            /* reset share buffer before put parameters*/
            /*audio_nvdm_reset_sysram();
            sysram_status_t status = audio_nvdm_set_feature(2, AudioFeatureList_LEAudioCall);
            if (status != NVDM_STATUS_NAT_OK){
                audio_src_srv_report("LE audio call failed to set parameters to share memory - err(%d)\r\n", 1, status);
                configASSERT(0);
            }*/
            audio_src_srv_report("LE audio call load nvdm ok\r\n", 0);
        }

        bt_codec_am_ble_open(&(background_ptr->local_context.ble_format.ble_codec));
#endif

    } else if (background_ptr->type == HFP) {
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_HFP_START);
#endif
#if defined(MTK_AVM_DIRECT)
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
#ifdef MTK_ANC_ENABLE
#ifndef MTK_ANC_V2
        if(g_anc_control.enable){
            g_am_anc_flag = true;
            anc_off(g_anc_control.anc_off_callback, 0, 0);
            #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_ANC_STOP);
            #endif
        }
#endif
#endif
#endif
        ami_set_audio_mask(AM_TASK_MASK_UL1_HAPPENING, true);
        if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)){
        #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_RECORD_STOP);
        #endif
        #ifdef MTK_RECORD_ENABLE
            am_audio_record_sink_stop();
        #else
            hal_audio_stop_stream_in();
        #endif
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
            if(g_prCurrent_playback[2] != NULL){
                g_prCurrent_playback[2]->notify_cb(g_prCurrent_playback[2]->aud_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_HFP, NULL);
                g_rAm_aud_id[g_prCurrent_playback[2]->aud_id].use = ID_IDLE_STATE;
                g_prCurrent_playback[2] = NULL;
                hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_RECORD);
            }
        }

//Check MIC user with transmitter
#if defined(AIR_WIRED_AUDIO_ENABLE)

    uint16_t scenario_and_id;

    audio_transmitter_scenario_list_t audio_transmitter_scenario_list[]  =  {
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT},
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE},
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT}
    };

    if(true == audio_transmitter_get_is_running_by_scenario_list(audio_transmitter_scenario_list, sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t))) {
        for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
            if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list[i], 1)) {
                scenario_and_id = (audio_transmitter_scenario_list[i].scenario_type << 8) | audio_transmitter_scenario_list[i].scenario_sub_id;
                audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is running, suspended by HFP\n", 1, scenario_and_id);
                audio_transmitter_playback_stop(scenario_and_id);
                audio_transmitter_playback_close(scenario_and_id);

                bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list[i].scenario_type, audio_transmitter_scenario_list[i].scenario_sub_id);
                if(am_id != -1) {
                    if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
                        g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
                    }
                    if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
                        g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_HFP, &scenario_and_id);
                    }
                } else {
                    audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
                    configASSERT(0);
                }
                #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
                #endif
                break;
            }
        }
    }
#endif

        if(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE) {
            uint32_t sidetone_gain;
            #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                if(audio_nvdm_HW_config.Voice_Sidetone_Gain >> 7){
                    sidetone_gain   = (0xFFFFFF00 | audio_nvdm_HW_config.Voice_Sidetone_Gain) * 100;      /*Default: -100 /100 = -1dB*/
                } else {
                    sidetone_gain   = (audio_nvdm_HW_config.Voice_Sidetone_Gain) * 100;
                }
            #else
                if (audio_nvdm_HW_config.sidetone_config.SideTone_Gain >> 7) {
                    sidetone_gain   = (0xFFFFFF00 | audio_nvdm_HW_config.sidetone_config.SideTone_Gain) * 100;      /*Default: -100 /100 = -1dB*/
                } else {
                    sidetone_gain   = (audio_nvdm_HW_config.sidetone_config.SideTone_Gain) * 100;
                }
            #endif
            sidetone_gain = (g_side_tone_gain_hfp == SIDETONE_GAIN_MAGIC_NUM) ? g_side_tone_gain_hfp : sidetone_gain;
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_SET_VOLUME, 0, sidetone_gain, false);
        }
        #ifdef MTK_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
            if(prompt_control_dummy_source_query_state()){
                audio_src_srv_report("[VP/DmySrc][Sink][AM HFP] HFP enter, stop DJ mode firstly, won't resume.", 0);
                hal_audio_mute_stream_out(true, HAL_AUDIO_STREAM_OUT3); // mute HW GAIN 3 to reduce pop noise
                prompt_control_dummy_source_stop_internal();
            }
        #endif /* VP Dummy Source */
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BT_VOICE_UL, true);
        audio_src_srv_report("[Sink][AM HFP] hfp_codec: %d, stream_out_device %d, stream_out_vol %d",3,
                     background_ptr->local_context.hfp_format.hfp_codec.type,
                     stream_out->audio_device,
                     stream_out->audio_volume);
#endif
        if (background_ptr->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD) {
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
#else
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_8KHZ);
#endif
        } else if (background_ptr->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_MSBC) {
            hal_audio_set_stream_out_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
            hal_audio_set_stream_in_sampling_rate(HAL_AUDIO_SAMPLING_RATE_16KHZ);
        }
        hal_audio_set_stream_out_channel_number(HAL_AUDIO_MONO);
        hal_audio_set_stream_in_channel_number(HAL_AUDIO_MONO);
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = background_ptr->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = stream_in->audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = stream_out->audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = stream_in->audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = background_ptr->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = stream_in->audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = stream_out->audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = stream_in->audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        hal_audio_set_stream_out_device(stream_out->audio_device);
        hal_audio_set_stream_in_device(stream_in->audio_device);

#ifdef __BT_SINK_SRV_AUDIO_TUNING__
        aud_tunning_p = bt_sink_srv_audio_tunning_get_context();

        if ((aud_tunning_p->flag & TUNNING_FLAG_INIT) &&
                (aud_tunning_p->flag & TUNNING_FLAG_ON)) {
            bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_HF, TUNNING_TYPE_VOL);
            bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_HF, TUNNING_TYPE_DEV);
        }
#endif /* __BT_SINK_SRV_AUDIO_TUNING__ */

        bt_codec_am_hfp_open(&(background_ptr->local_context.hfp_format.hfp_codec));

#ifdef MTK_PROMPT_SOUND_ENABLE
        //bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HFP_ON, NULL);
#endif
    }
#if !defined(MTK_AWS_MCE_ENABLE)
    else if (background_ptr->type == AWS) {
#if defined(MTK_AVM_DIRECT)
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, true);
#endif

        #ifdef __BT_AWS_SUPPORT__
        aws_role = background_ptr->local_context.aws_format.aws_codec.role;
        aws_codec = background_ptr->local_context.aws_format.aws_codec.codec_cap.type;
        /* AWS A2DP */
        if ((aws_role == BT_AWS_ROLE_SINK) &&
            ((aws_codec == BT_AWS_CODEC_TYPE_SBC) || (aws_codec == BT_AWS_CODEC_TYPE_AAC) || (aws_codec == BT_AWS_CODEC_TYPE_VENDOR))) {
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_A2DP;
                vol_info.vol_info.a2dp_vol_info.dev = stream_out->audio_device;
                vol_info.vol_info.a2dp_vol_info.lev = stream_out->audio_volume;
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
            hal_audio_set_stream_in_device(stream_in->audio_device);
            am_a2dp_codec.role = BT_A2DP_SINK;
            if (aws_codec == BT_AWS_CODEC_TYPE_SBC) {
                am_a2dp_codec.codec_cap.type = BT_A2DP_CODEC_SBC;
            } else if (aws_codec == BT_AWS_CODEC_TYPE_AAC) {
                am_a2dp_codec.codec_cap.type = BT_A2DP_CODEC_AAC;
            } else if (aws_codec == BT_AWS_CODEC_TYPE_VENDOR) {
                am_a2dp_codec.codec_cap.type = BT_A2DP_CODEC_VENDOR;
            } else {
                audio_src_srv_report("[Sink][AM][ERROR] Not Support A2DP AWS Codec.", 0);
                audio_src_srv_report("(AM)Not Support A2DP AWS Codec.", 0);
                configASSERT(0);
            }
            am_a2dp_codec.codec_cap.sep_type = BT_A2DP_SINK;
            am_a2dp_codec.codec_cap.length = background_ptr->local_context.aws_format.aws_codec.codec_cap.length;
            if(am_a2dp_codec.codec_cap.length > sizeof(bt_sink_srv_am_codec_t)) {
                am_a2dp_codec.codec_cap.length = sizeof(bt_sink_srv_am_codec_t);
                audio_src_srv_report("A2DP codec length error, please check.", 0);
            }
            memcpy(&(am_a2dp_codec.codec_cap.codec),
                    &(background_ptr->local_context.aws_format.aws_codec.codec_cap.codec),
                    am_a2dp_codec.codec_cap.length);

            aud_prepare_a2dp_nvkey(&(background_ptr->local_feature));
            bt_codec_am_a2dp_sink_open(&am_a2dp_codec);
        } else if ((aws_role == BT_AWS_ROLE_SINK) &&
            (aws_codec == BT_AWS_CODEC_TYPE_MP3)) {
            bt_sink_srv_am_files_format_t file_fmt;
            file_fmt.file_type = FILE_MP3;
            am_files_codec_open(&file_fmt); // need to adjust yiquan
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
            hal_audio_set_stream_in_device(stream_in->audio_device);
            hal_audio_set_stream_out_device(stream_out->audio_device);
        }
        #endif
    }
#endif //#if !defined(MTK_AWS_MCE_ENABLE)
    else if (background_ptr->type == FILES) {
#if defined(MTK_AVM_DIRECT)
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PLAYBACK, true);
#endif
        am_files_codec_open(&(background_ptr->local_context.files_format));
        hal_audio_set_stream_out_device(stream_out->audio_device);
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_MP3;
            vol_info.vol_info.mp3_vol_info.dev = stream_out->audio_device;
            vol_info.vol_info.mp3_vol_info.lev = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
    }
    else if (background_ptr->type == RECORD) {
#ifdef MTK_RECORD_ENABLE
        if((g_prHfp_media_handle == NULL) && (hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL) != true)){
            #ifdef MTK_LINE_IN_ENABLE
            if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
                audio_src_srv_report("[Sink][AM][PLAY] Line-in stopped by record.", 0);
                #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                #ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
                ami_execute_vendor_se(EVENT_BEFORE_LINEINPLAYBACK_STOP);
                #endif
                #endif
                audio_linein_playback_stop();
                aud_dl_control(false);
                audio_linein_playback_close();
                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, false);
                #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                ami_execute_vendor_se(EVENT_LINEINPLAYBACK_SUSPEND);
                ami_execute_vendor_se(EVENT_LINEINPLAYBACK_STOP);
                #endif
            }
            #endif

#ifdef AIR_WIRED_AUDIO_ENABLE
            audio_transmitter_scenario_list_t audio_transmitter_scenario_list[]  =  {
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER},
                #ifdef LINE_IN_USB_IN_EXCLUSIVE_WITH_RECORD_ENABLE
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1},
                #endif
            };

            /* For line-in/USB-in/line-out/usb-out, will be suspended by record */
            for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
                if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list[i], 1)) {
                    uint16_t current_scenario_and_id = (audio_transmitter_scenario_list[i].scenario_type << 8) | audio_transmitter_scenario_list[i].scenario_sub_id;
                    audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is running, suspended by record\n", 1, current_scenario_and_id);
                    #ifdef LINE_IN_USB_IN_EXCLUSIVE_WITH_RECORD_ENABLE
                    if ((audio_transmitter_scenario_list[i].scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)
                        ||(audio_transmitter_scenario_list[i].scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)) {
                        audio_transmitter_wired_audio_stop_close_usb_in(current_scenario_and_id);
                    }
                    else
                    #endif
                    {
                        audio_transmitter_playback_stop(current_scenario_and_id);
                        audio_transmitter_playback_close(current_scenario_and_id);
                    }

                    bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list[i].scenario_type, audio_transmitter_scenario_list[i].scenario_sub_id);
                    if(am_id != -1) {
                        if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
                            g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
                        }
                        if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
                            g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_RECORDER, &current_scenario_and_id);
                        }
                    } else {
                        audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
                        configASSERT(0);
                    }

                    #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                    ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
                    #endif
                }
            }
#endif
            #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_RECORD_START);
            #endif
            ami_set_audio_mask(AM_TASK_MASK_UL1_HAPPENING, true);
#if defined(MTK_AVM_DIRECT)
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, true);
#endif
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
            bt_sink_srv_audio_setting_vol_info_t vol_info = {0};

            vol_info.type = VOL_VC;
            //vol_info.vol_info.vc_vol_info.dev_in = stream_out->audio_device;
            //vol_info.vol_info.vc_vol_info.lev_in = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
            }
            #else
            hal_audio_set_stream_in_volume(3000, 0);
            #endif
            g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
            if(background_ptr->local_context.record_format.Reserve_callback != NULL){
                hal_audio_register_stream_in_callback(background_ptr->local_context.record_format.Reserve_callback, background_ptr->local_context.record_format.Reserve_callback_user_data);
            } else {
                audio_src_srv_report("[Sink][AM][ERROR] Record CCNI NULL", 0);
            }
            am_audio_record_sink_play(background_ptr);
        } else {
            /*Notify Record user HFP is happening, the request of open record resource will be rejected.*/
        }
#endif
    } else if (background_ptr->type == LINE_IN) {
#ifdef MTK_LINE_IN_ENABLE
        if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)){
            #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_LINEINPLAYBACK_REJECT);
            #endif
            audio_src_srv_report("[Sink][AM][PLAY] Line-in rejected by record.", 0);
            g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;//ToDO
        } else {
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_LINEINPLAYBACK_START);
#endif
#if defined(MTK_AVM_DIRECT)
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, true);
        if(background_ptr->local_context.line_in_format.line_in_codec.codec_cap.out_audio_device & HAL_AUDIO_DEVICE_DAC_DUAL) {
            audio_message_audio_handler(HAL_AUDIO_EVENT_DATA_REQUEST, NULL);
        }
#endif
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_LINE_IN;
            vol_info.vol_info.lineIN_vol_info.dev_in  = stream_in->audio_device;
            vol_info.vol_info.lineIN_vol_info.dev_out = stream_out->audio_device;
            vol_info.vol_info.lineIN_vol_info.lev_in  = stream_in->audio_volume;
            vol_info.vol_info.lineIN_vol_info.lev_out = stream_out->audio_volume;
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            bt_sink_srv_am_set_volume(STREAM_IN,  &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
        aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        audio_src_srv_report("[Sink][AM][PLAY] Line-in play.", 0);
#if defined(MTK_LINEIN_PEQ_ENABLE) || defined(MTK_LINEIN_INS_ENABLE)
        aud_prepare_linein_nvkey(&(background_ptr->local_feature));
#endif
        audio_linein_playback_open(background_ptr->local_context.line_in_format.line_in_codec.codec_cap.linein_sample_rate,
                                   background_ptr->local_context.line_in_format.line_in_codec.codec_cap.in_audio_device,
                                   background_ptr->local_context.line_in_format.line_in_codec.codec_cap.out_audio_device);
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, NULL);
#if 0
        g_prLineIN_sink_handle = (audio_sink_srv_am_line_in_codec_t *)pvPortMalloc(sizeof(audio_sink_srv_am_line_in_codec_t));
        if (g_prLineIN_sink_handle == NULL) {
            audio_src_srv_report("[Sink][AM][PLAY] Line-in play: allocate fail\r\n", 0);
        }
        memcpy(&(g_prLineIN_sink_handle), &(background_ptr->local_context.line_in_format.line_in_codec), sizeof(audio_sink_srv_am_line_in_codec_t));
#else
        g_prLineIN_sink_handle.codec_cap.linein_sample_rate = background_ptr->local_context.line_in_format.line_in_codec.codec_cap.linein_sample_rate;
        g_prLineIN_sink_handle.codec_cap.in_audio_device    = background_ptr->local_context.line_in_format.line_in_codec.codec_cap.in_audio_device;
        g_prLineIN_sink_handle.codec_cap.out_audio_device   = background_ptr->local_context.line_in_format.line_in_codec.codec_cap.out_audio_device;
#endif
        aud_dl_control(true);
        audio_linein_playback_start();
        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;//ToDO
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
        ami_execute_vendor_se(EVENT_AFTER_LINEINPLAYBACK_START);
#endif
#endif
        }
#endif
    } else if (background_ptr->type == USB_AUDIO_IN) {
#ifdef MTK_USB_AUDIO_PLAYBACK_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_USB_AUDIO_START);
#endif
        audio_src_srv_report("[Sink][AM][PLAY]USB_AUDIO play.", 0);
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_USB_AUDIO_IN;
            vol_info.vol_info.usb_audio_vol_info.dev = stream_out->audio_device;
            vol_info.vol_info.usb_audio_vol_info.lev = stream_out->audio_volume;

            audio_src_srv_report("[Sink][AM][PLAY]USB_AUDIO: audio_volume = %d.", 1, stream_out->audio_volume);
            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
        }
        #else
        aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */

        aud_prepare_a2dp_nvkey(&(background_ptr->local_feature));

        audio_usb_audio_playback_open();
        aud_dl_control(true);
        audio_usb_audio_playback_start();
        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;//ToDO
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, NULL);
#endif
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    } else if(background_ptr->type == AUDIO_TRANSMITTER){
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_TRANSMITTER_START);
#endif
        uint16_t scenario_and_id = ((background_ptr->local_context.audio_transmitter_format.scenario_type) << 8) + background_ptr->local_context.audio_transmitter_format.scenario_sub_id;
        audio_src_srv_report("[Sink][AM][transmitter]playback_open/start scenario_and_id = %x", 1, scenario_and_id);

#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
        if(background_ptr->local_context.audio_transmitter_format.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE) {
            if(ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)) {
                audio_src_srv_report("record is running when play AUDIO_TRANSMITTER_GAMING_MODE.", 0);
                configASSERT(0);
            }
            if(ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BT_VOICE_UL) || ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BT_VOICE_DL)) {
                audio_src_srv_report("HFP is running when play AUDIO_TRANSMITTER_GAMING_MODE.", 0);
                configASSERT(0);
            }
        }
#endif
#if defined(AIR_WIRED_AUDIO_ENABLE)
        audio_transmitter_scenario_list_t audio_transmitter_scenario_list[]  =  {
            {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT},
            {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT},
            {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER},//workaround
        };
        /*
        *  1. Line-out/USB-out exclusive with recorder, the recorder has a higher priority
        *  2. Line-out/USB-out exclusive with HFP, the HFP has a higher priority
        *  3. Line-out/USB-out exclusive with each other, the later one has a higher priority
        *  4. Line-out/USB-out exclusive with A2DP
        *  5. Dual chip workaround: HW loopback MIC has a higher priority than Line out
        */
       //Workaround for dual chip HFP/Line-out cohenrency case!!
       if(scenario_and_id == ((AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK << 8) + AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_ADC_TO_I2S0)) {

            /* For line-out/USB-out, the later one has a higher priority than the former one */
            for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
                if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list[i], 1)) {
                    uint16_t current_scenario_and_id = (audio_transmitter_scenario_list[i].scenario_type << 8) | audio_transmitter_scenario_list[i].scenario_sub_id;
                    audio_src_srv_report("[Sink][AM][transmitter]current_scenario_and_id = %x is running, suspended by scenario_and_id = %x\n", 2, current_scenario_and_id, scenario_and_id);

                    if((g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE)
                    && (!(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_WAITING_STOP)))
                    {
                        audio_src_srv_report("[Sink][AM][transmitter]current_scenario_and_id = %x is running, stop sidetone by am\n", 1, current_scenario_and_id);
                        audio_side_tone_disable_hdlr(NULL);
                    }
                    audio_transmitter_playback_stop(current_scenario_and_id);
                    audio_transmitter_playback_close(current_scenario_and_id);

                    bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list[i].scenario_type, audio_transmitter_scenario_list[i].scenario_sub_id);
                    if(am_id != -1) {
                        if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
                            g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
                        }
                        if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
                            g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_HFP, &current_scenario_and_id);
                        }
                    } else {
                        audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
                        configASSERT(0);
                    }

                    #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                    ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
                    #endif
                    break;
                }
            }
        /*
        *  1. Line-in/USB-in exclusive with recorder, the recorder has a higher priority
        *  2. Line-in/USB-in exvlusive with each other, the later one has a higher priority
        */
        } else if((scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT)) || \
           (scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT))) {
            /* recorder has a higher priority than Line-out/USB-out */
            if (ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)) {
                audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is rejected by record\n", 1, scenario_and_id);
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SINK_OPEN_CODEC, AUD_SUSPEND_BY_RECORDER, &scenario_and_id);
                return;
            }
            /* HFP has a higher priority than Line-out/USB-out */
            if (ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BT_VOICE_UL) || ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BT_VOICE_DL)) {
                audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is rejected by HFP\n", 1, scenario_and_id);
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SINK_OPEN_CODEC, AUD_SUSPEND_BY_HFP, &scenario_and_id);
                return;
            }
            /* LE CALL has a higher priority than Line-out/USB-out */
            audio_src_srv_report("[Sink][AM][transmitter]get le audio running flag %d %d\n", 2, ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL),ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL));
            if (ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL)||ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL)) {
                audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is rejected by LE_CALL\n", 1, scenario_and_id);
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SINK_OPEN_CODEC, AUD_SUSPEND_BY_LE_CALL, &scenario_and_id);
                return;
            }
            /* For line-out/USB-out, the later one has a higher priority than the former one */
            for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
                if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list[i], 1)) {
                    uint16_t current_scenario_and_id = (audio_transmitter_scenario_list[i].scenario_type << 8) | audio_transmitter_scenario_list[i].scenario_sub_id;
                    audio_src_srv_report("[Sink][AM][transmitter]current_scenario_and_id = %x is running, suspended by scenario_and_id = %x\n", 2, current_scenario_and_id, scenario_and_id);
                    audio_transmitter_playback_stop(current_scenario_and_id);
                    audio_transmitter_playback_close(current_scenario_and_id);

                    bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list[i].scenario_type, audio_transmitter_scenario_list[i].scenario_sub_id);
                    if(am_id != -1) {
                        if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
                            g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
                        }
                        if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
                            g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, (((scenario_and_id & 0xFF) == AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT) || ((scenario_and_id & 0xFF) == AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER)) ? AUD_SUSPEND_BY_LINE_OUT : AUD_SUSPEND_BY_USB_OUT, &current_scenario_and_id);
                        }
                    } else {
                        audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
                        configASSERT(0);
                    }

                    #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                    ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
                    #endif
                    break;
                }
            }

            /* Line-out/USB-out exclusive with A2DP */
            if(ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL) == true) {
                audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is rejected by A2DP\n", 1, scenario_and_id);
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SINK_OPEN_CODEC, AUD_SUSPEND_BY_A2DP, &scenario_and_id);
                return;
            }
        /*
        *  1. Line-in/USB-in exclusive with recorder, the recorder has a higher priority
        *  2. Line-in/USB-in exvlusive with each other, the later one has a higher priority
        */
        } else if((scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN)) || \
           (scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER)) || \
           (scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE)) || \
           (scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)) || \
           (scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))) {

            audio_transmitter_scenario_list_t audio_transmitter_scenario_list_usb_in[]  =  {
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1}
            };

            audio_transmitter_scenario_list_t audio_transmitter_scenario_list_line_in[]  =  {
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER},
                {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE}
            };
            #ifdef LINE_IN_USB_IN_EXCLUSIVE_WITH_RECORD_ENABLE
            /* recorder has a higher priority than Line-in/USB-in */
            if (ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)) {
                audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is rejected by record\n", 1, scenario_and_id);
                //configASSERT(0);
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SINK_OPEN_CODEC, AUD_SUSPEND_BY_RECORDER, &scenario_and_id);
                return;
            }
            #endif
            /* For line-in/USB-in, the later one has a higher priority than the former one */
            /* Line-in preempt USB-in */
            if((scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN)) || \
               (scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER)) || \
               (scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE))) {
                for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list_usb_in)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
                    if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list_usb_in[i], 1)) {
                        uint16_t current_scenario_and_id = (audio_transmitter_scenario_list_usb_in[i].scenario_type << 8) | audio_transmitter_scenario_list_usb_in[i].scenario_sub_id;
                        audio_src_srv_report("[Sink][AM][transmitter]current_scenario_and_id = %x is running, suspended by scenario_and_id = %x\n", 2, current_scenario_and_id, scenario_and_id);
                        audio_transmitter_wired_audio_stop_close_usb_in(current_scenario_and_id);
                        bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list_usb_in[i].scenario_type, audio_transmitter_scenario_list_usb_in[i].scenario_sub_id);
                        if(am_id != -1) {
                            if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
                                g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
                            }
                            if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
                                g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_LINE_IN, &current_scenario_and_id);
                            }
                        } else {
                            audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
                            configASSERT(0);
                        }
                        #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                        ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
                        #endif
                    }
                }
                /* Line-in pre-setting before play */
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                bt_sink_srv_audio_setting_vol_info_t vol_info;
                vol_info.type = VOL_LINE_IN_DL3;
                vol_info.vol_info.lineIN_vol_info.dev_in  = AUDIO_SINK_SRV_LINE_IN_INPUT_DEVICE;
                vol_info.vol_info.lineIN_vol_info.dev_out = AUDIO_SINK_SRV_LINE_IN_OUTPUT_DEVICE;
                vol_info.vol_info.lineIN_vol_info.lev_in  = AUD_VOL_IN_LEVEL0;//stream_in->audio_volume;
                vol_info.vol_info.lineIN_vol_info.lev_out = AUD_VOL_OUT_LEVEL0;//stream_out->audio_volume;
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                bt_sink_srv_am_set_volume(STREAM_IN,  &vol_info);
                #else
                aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
                aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                #if defined(MTK_LINEIN_PEQ_ENABLE) || defined(MTK_LINEIN_INS_ENABLE)
                            aud_prepare_linein_nvkey(&(background_ptr->local_feature));
                #endif
            } else {
            /* USB-in preempt Line-in */
                for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list_line_in)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
                    if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list_line_in[i], 1)) {
                        uint16_t current_scenario_and_id = (audio_transmitter_scenario_list_line_in[i].scenario_type << 8) | audio_transmitter_scenario_list_line_in[i].scenario_sub_id;
                        audio_src_srv_report("[Sink][AM][transmitter]current_scenario_and_id = %x is running, suspended by scenario_and_id = %x\n", 2, current_scenario_and_id, scenario_and_id);
                        audio_transmitter_playback_stop(current_scenario_and_id);
                        audio_transmitter_playback_close(current_scenario_and_id);
                        bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list_line_in[i].scenario_type, audio_transmitter_scenario_list_line_in[i].scenario_sub_id);
                        if(am_id != -1) {
                            if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
                                g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
                            }
                            if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
                                g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_USB_IN, &current_scenario_and_id);
                            }
                        } else {
                            audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
                            configASSERT(0);
                        }
                        #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                        ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
                        #endif
                    }
                }
                /* USB-in pre-setting before play */
                wired_audio_add_usb_audio_start_number();
                if(wired_audio_get_usb_audio_start_number() == 1){
                    wired_audio_set_usb_use_afe_subid(background_ptr->local_context.audio_transmitter_format.scenario_sub_id);
                }
                else if(wired_audio_get_usb_audio_start_number() == 2){
                    if(wired_audio_get_usb_use_afe_subid() == background_ptr->local_context.audio_transmitter_format.scenario_sub_id){
                        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
                        //bypass
                        //audio_transmitter_playback_open(scenario_and_id);
                        //audio_transmitter_playback_start(scenario_and_id);
                        background_ptr->notify_cb(background_ptr->aud_id, AUD_SINK_OPEN_CODEC, AUD_CMD_COMPLETE, &scenario_and_id);
                        return;
                    }
                }
                else if(wired_audio_get_usb_audio_start_number() >2){
                    audio_src_srv_report("too much wire usb audio set play", 0);
                    configASSERT(0);
                }
                //reuse line in level
                bt_sink_srv_audio_setting_vol_info_t vol_info;
                vol_info.type = VOL_LINE_IN_DL3;
                vol_info.vol_info.lineIN_vol_info.dev_in  = AUDIO_SINK_SRV_LINE_IN_INPUT_DEVICE;
                vol_info.vol_info.lineIN_vol_info.dev_out = AUDIO_SINK_SRV_LINE_IN_OUTPUT_DEVICE;
                vol_info.vol_info.lineIN_vol_info.lev_in  = AUD_VOL_IN_LEVEL0;
                vol_info.vol_info.lineIN_vol_info.lev_out = AUD_VOL_OUT_LEVEL15;
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                #if defined(MTK_LINEIN_PEQ_ENABLE) || defined(MTK_LINEIN_INS_ENABLE)
                if(wired_audio_get_usb_audio_start_number() != 2) {
                    aud_prepare_linein_nvkey(&(background_ptr->local_feature));
                }
                #endif
            }
        }

#endif

#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
        if((background_ptr->local_context.audio_transmitter_format.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE)
        &&(background_ptr->local_context.audio_transmitter_format.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)){
            bt_sink_srv_audio_setting_vol_info_t vol_info;
            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_MSBC;
            vol_info.vol_info.hfp_vol_info.dev_in = HAL_AUDIO_DEVICE_MAIN_MIC;
            vol_info.vol_info.hfp_vol_info.dev_out = HAL_AUDIO_DEVICE_HEADSET;
            vol_info.vol_info.hfp_vol_info.lev_in = 0;
            vol_info.vol_info.hfp_vol_info.lev_out = 0;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
#endif

        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        if((background_ptr->local_context.audio_transmitter_format.scenario_type == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH)
        &&(background_ptr->local_context.audio_transmitter_format.scenario_sub_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID)){
            bt_sink_srv_audio_setting_vol_info_t vol_info;
            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_MSBC;
            vol_info.vol_info.hfp_vol_info.dev_in = HAL_AUDIO_DEVICE_MAIN_MIC;
            vol_info.vol_info.hfp_vol_info.dev_out = HAL_AUDIO_DEVICE_HEADSET;
            vol_info.vol_info.hfp_vol_info.lev_in = 0;
            vol_info.vol_info.hfp_vol_info.lev_out = 0;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);

            aud_prepare_hearing_aid_nvkey(&(background_ptr->local_feature));
        }
        #endif /* AIR_ADVANCED_PASSTHROUGH_ENABLE */

        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;

        audio_transmitter_playback_open(scenario_and_id);
        audio_transmitter_playback_start(scenario_and_id);
        background_ptr->notify_cb(background_ptr->aud_id, AUD_SINK_OPEN_CODEC, AUD_CMD_COMPLETE, &scenario_and_id);

#endif
    } else {
        /* trace error */
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_suspend
 * DESCRIPTION
 *  This function set to SUSPEND state when be interrupt by others.
 * PARAMETERS
 *  lead             [IN]
 *  background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_set_suspend(bt_sink_srv_am_type_t lead, bt_sink_srv_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_cb_sub_msg_t   msg_id = AUD_SUSPEND_BY_NONE;
    bt_sink_srv_am_id_t             bAud_id = background_ptr->aud_id;
    bt_sink_srv_am_stream_type_t    eIn_out;
    void *parm = NULL;
    #ifdef __BT_AWS_SUPPORT__
    bt_aws_role_t aws_role = BT_AWS_ROLE_NONE;
    bt_aws_codec_type_t aws_codec = 0;
    #endif /* __BT_AWS_SUPPORT__ */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (lead) {
        case NONE:
            msg_id = AUD_SUSPEND_BY_NONE;
            break;
        case PCM:
            msg_id = AUD_SUSPEND_BY_PCM;
            break;
        case A2DP:
            msg_id = AUD_SUSPEND_BY_A2DP;
            break;
        case HFP:
            msg_id = AUD_SUSPEND_BY_HFP;
            break;
        case FILES:
            msg_id = AUD_SUSPEND_BY_FILES;
            break;
        case AWS:
            msg_id = AUD_SUSPEND_BY_AWS;
            break;
        default:
            break;
    }

    if (background_ptr->type == NONE) {

    } else if (background_ptr->type == PCM) {
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            hal_audio_stop_stream_out();
        } else if (eIn_out == STREAM_IN) {
            hal_audio_stop_stream_in();
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
        }
    } else if ( (background_ptr->type == A2DP)
#if defined(MTK_AWS_MCE_ENABLE)
                || (background_ptr->type == AWS)
#endif
              )
    {
        if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
            if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                #ifdef __AM_DEBUG_INFO__
                audio_src_srv_report("[Sink][AM][ERROR] Suspend A2DP", 0);
                #endif
                return;
            }
        } else if (g_rSink_state == A2DP_SINK_CODEC_PLAY) {
            if ((bt_codec_am_a2dp_sink_stop(background_ptr->aud_id) == BT_CODEC_MEDIA_STATUS_ERROR) ||
                    (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR)) {
                #ifdef __AM_DEBUG_INFO__
                audio_src_srv_report("[Sink][AM][ERROR] Suspend A2DP", 0);
                #endif
                return;
            }
        }
        #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_A2DP_STOP);
        #endif
    } else if (background_ptr->type == HFP) {
        if (bt_codec_am_hfp_stop() == BT_CODEC_MEDIA_STATUS_ERROR) {
#ifdef __AM_DEBUG_INFO__
            audio_src_srv_report("[Sink][AM][ERROR] Suspend HFP", 0);
#endif
            return;
        }
        #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_HFP_STOP);
        #endif
    }
#if !defined(MTK_AWS_MCE_ENABLE)
    else if (background_ptr->type == AWS) {
        #ifdef __BT_AWS_SUPPORT__
        aws_role = background_ptr->local_context.aws_format.aws_codec.role;
        aws_codec = background_ptr->local_context.aws_format.aws_codec.codec_cap.type;
        /* AWS A2DP */
        if ((aws_role == BT_AWS_ROLE_SINK) &&
            ((aws_codec == BT_AWS_CODEC_TYPE_SBC) || (aws_codec == BT_AWS_CODEC_TYPE_AAC) || (aws_codec == BT_AWS_CODEC_TYPE_VENDOR))) {
            if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
                if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                    #ifdef __AM_DEBUG_INFO__
                    audio_src_srv_report("[Sink][AM][ERROR][AWS]Suspend A2DP 1", 0);
                    #endif
                    return;
                }
            } else if (g_rSink_state == A2DP_SINK_CODEC_PLAY) {
                if ((bt_codec_am_a2dp_sink_stop(background_ptr->aud_id) == BT_CODEC_MEDIA_STATUS_ERROR) ||
                        (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR)) {
                    #ifdef __AM_DEBUG_INFO__
                    audio_src_srv_report("[Sink][AM][ERROR][AWS]Suspend A2DP 2", 0);
                    #endif
                    return;
                }
            }
        } else if ((aws_role == BT_AWS_ROLE_SINK) && (aws_codec == BT_AWS_CODEC_TYPE_MP3)) {
            am_files_codec_close();
        }
        #endif /* __BT_AWS_SUPPORT__ */
    }
#endif // #if !defined(MTK_AWS_MCE_ENABLE)
    else if (background_ptr->type == FILES) {
        am_files_codec_close();
    }

    g_rAm_aud_id[bAud_id].use = ID_SUSPEND_STATE;
    if (AUD_SUSPEND_BY_NONE == msg_id || AUD_SUSPEND_BY_HFP == msg_id) {
        parm = (void *) &g_int_dev_addr;
    }

    background_ptr->notify_cb(background_ptr->aud_id, AUD_SUSPEND_BY_IND, msg_id, parm);
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_search_suspended
 * DESCRIPTION
 *  This function is used to find the highest suspend event to resume it when no player.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_search_suspended(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *recoder_current_ptr = NULL;
    bt_sink_srv_am_id_t bAud_id;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    recoder_current_ptr = g_rBackground_head.next;
    while (recoder_current_ptr != NULL) {
        bAud_id = recoder_current_ptr->aud_id;
        if (g_rAm_aud_id[bAud_id].use == ID_SUSPEND_STATE) {
            g_rAm_aud_id[bAud_id].use = ID_RESUME_STATE;
            g_prCurrent_resumer = recoder_current_ptr;
#ifdef RTOS_TIMER
            xTimerStart(g_xTimer_am, 0);
#endif
            recoder_current_ptr->notify_cb(recoder_current_ptr->aud_id, AUD_RESUME_IND, AUD_RESUME_IDLE_STATE, NULL);
            break;
        }
        recoder_current_ptr = recoder_current_ptr->next;
    }
}

/*****************************************************************************
 * FUNCTION
 *  am_audio_set_stop
 * DESCRIPTION
 *  This function is used to stop audio handler.
 * PARAMETERS
 *  background_ptr   [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void am_audio_set_stop(bt_sink_srv_am_background_t *background_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_stream_type_t    eIn_out;
    #ifdef __BT_AWS_SUPPORT__
    bt_aws_role_t aws_role = BT_AWS_ROLE_NONE;
    bt_aws_codec_type_t aws_codec = 0;
    #endif /* __BT_AWS_SUPPORT__ */

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (background_ptr->type == NONE) {
    } else if (background_ptr->type == PCM) {
        eIn_out = background_ptr->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            hal_audio_stop_stream_out();
#if defined(MTK_AVM_DIRECT)
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PLAYBACK, false);
#endif
        } else if (eIn_out == STREAM_IN) {
            hal_audio_stop_stream_in();
#if defined(MTK_AVM_DIRECT)
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
#endif
        }
    } else if ( (background_ptr->type == A2DP)
#if defined(MTK_AWS_MCE_ENABLE)
                || (background_ptr->type == AWS)
#endif
              )
    {
        audio_src_srv_report("[Sink][AM]A2DP sink state: %d",1, g_rSink_state);
        if (g_rSink_state == A2DP_SINK_CODEC_STOP) {
            if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                #ifdef __AM_DEBUG_INFO__
                audio_src_srv_report("[Sink][AM][ERROR] A2DP codec close fail", 0);
                #endif
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                return;
            }
        } else if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
            if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                #ifdef __AM_DEBUG_INFO__
                audio_src_srv_report("[Sink][AM][ERROR] A2DP codec close fail", 0);
                #endif
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                return;
            }
        } else if (g_rSink_state != A2DP_SINK_CODEC_CLOSE) {
            #ifdef __AM_DEBUG_INFO__
            audio_src_srv_report("[Sink][AM][ERROR] A2DP codec NOT CLOSE", 0);
            #endif
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
            return;
        }
#if defined(MTK_AVM_DIRECT)
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, false);
#endif
            #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_A2DP_STOP);
            #endif
// #if defined(AIR_WIRED_AUDIO_ENABLE)

//     uint16_t scenario_and_id;

//     audio_transmitter_scenario_list_t audio_transmitter_scenario_list[]  =  {
//         {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT},
//         {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT}
//     };

//     if(g_am_task_mask & AM_TASK_LINE_OUT_SUSPEND) {
//         for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
//             if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list[i], 1)) {
//                 #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
//                 ami_execute_vendor_se(EVENT_TRANSMITTER_START);
//                 #endif
//                 scenario_and_id = (audio_transmitter_scenario_list[i].scenario_type << 8) | audio_transmitter_scenario_list[i].scenario_sub_id;
//                 audio_src_srv_report("[Sink][AM][transmitter]scenario_and_id = %x is running, resumed by A2DP\n", 1, scenario_and_id);
//                 audio_transmitter_playback_open(scenario_and_id);
//                 audio_transmitter_playback_start(scenario_and_id);
//                 ami_set_audio_mask(AM_TASK_LINE_OUT_SUSPEND, false);
//                 break;
//                 // bt_sink_srv_am_id_t am_id = audio_transmitter_get_am_id_by_scenario(audio_transmitter_scenario_list[i].scenario_type, audio_transmitter_scenario_list[i].scenario_sub_id);
//                 // if(am_id != -1) {
//                 //     if (g_rAm_aud_id[am_id].use != ID_CLOSE_STATE) {
//                 //         g_rAm_aud_id[am_id].use = ID_IDLE_STATE;
//                 //     }
//                 //     if(g_rAm_aud_id[am_id].contain_ptr->notify_cb != NULL) {
//                 //         g_rAm_aud_id[am_id].contain_ptr->notify_cb(am_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_A2DP, &scenario_and_id);
//                 //     }
//                 // } else {
//                 //     audio_src_srv_report("[Sink][AM][transmitter]No am id for the running scenario, abnormal!\r\n", 0);
//                 //     configASSERT(0);
//                 // }
//             }
//         }
//     }
// #endif
    } else if (background_ptr->type == BLE) {
#ifdef AIR_BT_CODEC_BLE_ENABLED
        if (bt_codec_am_ble_stop() == BT_CODEC_MEDIA_STATUS_ERROR) {
#ifdef __AM_DEBUG_INFO__
            audio_src_srv_report("[Sink][AM][ERROR] Close BLE", 0);
#endif
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
            return;
        }
            //use AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL for DL only case
        if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL)) {
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL, false);
        } else {
            //use AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL for DL/UL both case
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BLE_AUDIO_UL, false);
        }
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_BLE_STOP);
#endif
#endif //#ifdef AIR_BT_CODEC_BLE_ENABLED
    } else if (background_ptr->type == HFP) {
        if (bt_codec_am_hfp_stop() == BT_CODEC_MEDIA_STATUS_ERROR) {
#ifdef __AM_DEBUG_INFO__
            audio_src_srv_report("[Sink][AM][ERROR] Close HFP", 0);
#endif
            background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
            return;
        }
#if defined(MTK_AVM_DIRECT)
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BT_VOICE_UL, false);
#endif
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
#ifdef MTK_ANC_ENABLE
#ifndef MTK_ANC_V2
            if(g_am_anc_flag){
                g_am_anc_flag = false;
                #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                    ami_execute_vendor_se(EVENT_ANC_START);
                #endif
                if(MACRO_ANC_CHK_PASSTHRU(g_anc_control.cur_filter_type) == true){
                    anc_on(g_anc_control.cur_filter_type, g_anc_control.passthru_runtime_gain, g_anc_control.anc_on_callback, 0);
                } else {
                    anc_on(g_anc_control.cur_filter_type, g_anc_control.anc_runtime_gain, g_anc_control.anc_on_callback, 0);
                }
            }
#endif
#endif
#endif
#ifdef MTK_PROMPT_SOUND_ENABLE
        //bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_HFP_OFF, NULL);
#endif
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_HFP_STOP);
#endif
    }
#if !defined(MTK_AWS_MCE_ENABLE)
    else if (background_ptr->type == AWS){
        #ifdef __BT_AWS_SUPPORT__
        aws_role = background_ptr->local_context.aws_format.aws_codec.role;
        aws_codec = background_ptr->local_context.aws_format.aws_codec.codec_cap.type;
        /* AWS A2DP */
        if ((aws_role == BT_AWS_ROLE_SINK) &&
            ((aws_codec == BT_AWS_CODEC_TYPE_SBC) || (aws_codec == BT_AWS_CODEC_TYPE_AAC) || (aws_codec == BT_AWS_CODEC_TYPE_VENDOR))) {
            audio_src_srv_report("[Sink][AM][AWS]A2DP sink state: %d",1, g_rSink_state);
            if (g_rSink_state == A2DP_SINK_CODEC_STOP) {
                if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                    #ifdef __AM_DEBUG_INFO__
                    audio_src_srv_report("[Sink][AM][ERROR][AWS]A2DP codec close fail", 0);
                    #endif
                    background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                    return;
                }
            } else if (g_rSink_state == A2DP_SINK_CODEC_OPEN) {
                if (bt_codec_am_a2dp_sink_close() == BT_CODEC_MEDIA_STATUS_ERROR) {
                    #ifdef __AM_DEBUG_INFO__
                    audio_src_srv_report("[Sink][AM][ERROR][AWS]A2DP codec close fail", 0);
                    #endif
                    background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                    return;
                }
            } else if (g_rSink_state != A2DP_SINK_CODEC_CLOSE) {
                #ifdef __AM_DEBUG_INFO__
                audio_src_srv_report("[Sink][AM][ERROR][AWS]A2DP codec NOT CLOSE", 0);
                #endif
                background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
                return;
            }
        }
#if defined(MTK_AVM_DIRECT)
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, false);
#endif
        #endif /* __BT_AWS_SUPPORT__ */
    }
#endif // #if !defined(MTK_AWS_MCE_ENABLE)
    else if (background_ptr->type == FILES) {
        am_files_codec_close();
#if defined(MTK_AVM_DIRECT)
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_PLAYBACK, false);
#endif
        background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_FILE_EVENT_DATA_END, NULL);
    }
    else if (background_ptr->type == RECORD) {
#ifdef MTK_RECORD_ENABLE
    if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)){
        am_audio_record_sink_stop();
        if(background_ptr->local_context.record_format.Reserve_callback != NULL){
            hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_RECORD);
        }
#if defined(MTK_AVM_DIRECT)
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
#endif
        ami_set_audio_mask(AM_TASK_MASK_UL1_HAPPENING, false);
        if(g_prCurrent_playback[2] != NULL){
            g_prCurrent_playback[2] = NULL;
        }
        #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_RECORD_STOP);
        #endif
    } else {
        audio_src_srv_report("[Sink][AM][RECORD] record is stopped by other scenarios already.", 0);
    }
#endif /*MTK_RECORD_ENABLE*/
    } else if (background_ptr->type == LINE_IN) {
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
        ami_execute_vendor_se(EVENT_BEFORE_LINEINPLAYBACK_STOP);
#endif
#endif
#ifdef MTK_LINE_IN_ENABLE
#if defined(MTK_AVM_DIRECT)
        if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
            audio_src_srv_report("[Sink][AM][PLAY] Line-in stop.", 0);
            audio_linein_playback_stop();
            aud_dl_control(false);
            audio_linein_playback_close();
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_LINEIN, false);
            #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            ami_execute_vendor_se(EVENT_LINEINPLAYBACK_STOP);
            #endif
        } else {
            audio_src_srv_report("[Sink][AM][PLAY] Line-in stopped already.", 0);
        }
#endif
#if 0
        if(g_prLineIN_sink_handle != NULL){
            vPortFree(g_prLineIN_sink_handle);
            g_prLineIN_sink_handle = NULL;
        }
#endif
#endif
    }else if (background_ptr->type == USB_AUDIO_IN) {
#ifdef MTK_USB_AUDIO_PLAYBACK_ENABLE
        audio_src_srv_report("[Sink][AM][PLAY]USB_AUDIO stop.", 0);
        audio_usb_audio_playback_stop();
        aud_dl_control(false);
        audio_usb_audio_playback_close();
#endif
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_USB_AUDIO_STOP);
#endif
    }
#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
        else if(background_ptr->type == AUDIO_TRANSMITTER){
        uint16_t scenario_and_id = ((background_ptr->local_context.audio_transmitter_format.scenario_type)<<8) + background_ptr->local_context.audio_transmitter_format.scenario_sub_id;
        audio_src_srv_report("[Sink][AM][transmitter]playback_stop/close scenario_and_id = %x",1,scenario_and_id);
        #if defined(AIR_WIRED_AUDIO_ENABLE)
        if((scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0))
        || (scenario_and_id == ((AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))){
            audio_transmitter_wired_audio_stop_close_usb_in(scenario_and_id);
        }
        else
        #endif /* AIR_WIRED_AUDIO_ENABLE */
        {
            audio_transmitter_playback_stop(scenario_and_id);
            audio_transmitter_playback_close(scenario_and_id);
        }
        background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, &scenario_and_id);
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_TRANSMITTER_STOP);
#endif
    }
#endif

    if(g_rAm_aud_id[background_ptr->aud_id].use != ID_CLOSE_STATE){
        g_rAm_aud_id[background_ptr->aud_id].use = ID_IDLE_STATE;
    }
    background_ptr->notify_cb(background_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);

    #if defined(MTK_AVM_DIRECT)
    if (background_ptr->type != RECORD){
#if defined (MTK_AUDIO_TRANSMITTER_ENABLE)
        // Temp solution for audio transmitter, the UL cases should bypass the flag setting
        if(background_ptr->type == AUDIO_TRANSMITTER) {
            return;
        }
#endif
        ami_set_audio_mask(AM_TASK_MASK_DL1_HAPPENING, false);
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
        ami_set_afe_param(STREAM_OUT, 0, false);
#endif
    }
    #endif
}

/*****************************************************************************
 * FUNCTION
 *  aud_initial
 * DESCRIPTION
 *
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_sink_srv_am_hal_result_t
 *****************************************************************************/
static void aud_initial(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    memset(&g_rAm_aud_id[0], 0, sizeof(bt_sink_srv_am_aud_id_type_t)*AM_REGISTER_ID_TOTAL);
    memset(&g_prA2dp_sink_event_handle, 0, sizeof(g_prA2dp_sink_event_handle));

    hal_audio_set_sysram();
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    aud_peq_init();
#endif
#ifdef MTK_AWS_MCE_ENABLE
    if (bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_PEQ, bt_aws_mce_report_peq_callback) != BT_STATUS_SUCCESS)
    {
        audio_src_srv_err("peq failed to register aws mce report callback\r\n", 0);
    }
#endif

}

/*****************************************************************************
 * FUNCTION
 *  aud_set_open_stream_req_hdlr
 * DESCRIPTION
 *  Get register ID
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_open_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *Background_ptr = NULL;
    bt_sink_srv_am_background_t *recoder_current_ptr = NULL, *recoder_previous_ptr = NULL;
    bt_sink_srv_am_hal_result_t eResult = HAL_AUDIO_STATUS_ERROR;
    uint32_t g_AudMem_size = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_aud_id_num == 1) {
        eResult = hal_audio_get_memory_size(&g_AudMem_size);
        if (eResult == HAL_AUDIO_STATUS_OK) {
            if (!aud_memory) {
#ifndef WIN32_UT
                aud_memory = (uint16_t *)pvPortMalloc(g_AudMem_size);
#else
                aud_memory = (uint16_t *)malloc(g_AudMem_size);
#endif
            }
            hal_audio_set_memory(aud_memory);
        } else {
#ifdef __AM_DEBUG_INFO__
            audio_src_srv_report("[Sink][AM][ERROR] Get memoey size", 0);
#endif
        }
    }

#ifndef WIN32_UT
    Background_ptr = (bt_sink_srv_am_background_t *)pvPortMalloc(sizeof(bt_sink_srv_am_background_t));
#else
    Background_ptr = (bt_sink_srv_am_background_t *)malloc(sizeof(bt_sink_srv_am_background_t));
#endif
    if (Background_ptr) {
    Background_ptr->aud_id = bAud_id;
    Background_ptr->type = am_background_temp->type;
    Background_ptr->priority = am_background_temp->priority;
    Background_ptr->priority_table = am_background_temp->priority_table;
    Background_ptr->notify_cb = am_background_temp->notify_cb;
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    memset(&Background_ptr->local_feature.feature_param.peq_param, 0, sizeof(bt_sink_srv_am_peq_param_t));
    Background_ptr->local_feature.feature_param.peq_param.enable = PEQ_DEFAULT_ENABLE;
    Background_ptr->local_feature.feature_param.peq_param.sound_mode = PEQ_DEFAULT_SOUND_MODE;
    Background_ptr->local_feature.feature_param.peq_param.setting_mode = PEQ_DIRECT;
#endif

    recoder_previous_ptr = &g_rBackground_head;
    recoder_current_ptr = g_rBackground_head.next;
    while ((recoder_current_ptr != NULL) && (recoder_current_ptr->priority > Background_ptr->priority)) {
        recoder_previous_ptr = recoder_current_ptr;
        recoder_current_ptr = recoder_current_ptr->next;
    }
    Background_ptr->prior = recoder_previous_ptr;
    Background_ptr->next = recoder_current_ptr;
    recoder_previous_ptr->next = Background_ptr;
    if (recoder_current_ptr != NULL) {
        recoder_current_ptr->prior = Background_ptr;
    }
    g_rAm_aud_id[bAud_id].contain_ptr = Background_ptr;
    //Background_ptr->notify_cb(am_background_temp->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    }

    //printf("[AudM]open_hdr-id: %d, num: %d, use: %d, b_ptr: 0x%x\n", bAud_id,
        //g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)Background_ptr);
    audio_src_srv_report("[AudM]open_hdr-id: %d, num: %d, use: %d, b_ptr: 0x%x",4, bAud_id,
        g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)Background_ptr);
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_play_stream_req_hdlr
 * DESCRIPTION
 *  Start to play the specified audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_play_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t         bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;
    bt_sink_srv_am_background_t *recoder_high_t = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_resumer != NULL) {
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_rAm_aud_id[g_prCurrent_resumer->aud_id].use = ID_IDLE_STATE;
        g_prCurrent_resumer = NULL;
    }

    //printf("[AudM]play_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x\n",
        //bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);
    audio_src_srv_report("[AudM]play_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x",4,
        bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);

    if (recoder_current_ptr) {
    recoder_current_ptr->type = am_background_temp->type;
    recoder_current_ptr->audio_path_type = am_background_temp->audio_path_type;
    memcpy(&(recoder_current_ptr->local_context), &(am_background_temp->local_context), sizeof(bt_sink_srv_am_codec_t));
    memcpy(&(recoder_current_ptr->audio_stream_in), &(am_background_temp->audio_stream_in), sizeof(bt_sink_srv_am_audio_stream_in_t));
    memcpy(&(recoder_current_ptr->audio_stream_out), &(am_background_temp->audio_stream_out), sizeof(bt_sink_srv_am_audio_stream_out_t));
    /* Find which is high priority */
    recoder_high_t = g_rBackground_head.next;
    while ((recoder_high_t != NULL) && (recoder_high_t->priority > recoder_current_ptr->priority)) {
        recoder_high_t = recoder_high_t->next;
    }
    if (recoder_high_t && (recoder_high_t->aud_id != bAud_id)) {
        if (recoder_current_ptr->next != NULL) {
            recoder_current_ptr->next->prior = recoder_current_ptr->prior;
        }
        recoder_current_ptr->prior->next = recoder_current_ptr->next;
        recoder_high_t->prior->next = recoder_current_ptr;
        recoder_current_ptr->prior = recoder_high_t->prior;
        recoder_high_t->prior = recoder_current_ptr;
        recoder_current_ptr->next = recoder_high_t;
    }

#ifdef MTK_PROMPT_SOUND_ENABLE
    //prompt_control_stop_tone();
    // bt_sink_srv_event_callback(BT_SINK_SRV_EVENT_MIXER_TONE_STOP, NULL);
#endif
    audio_src_srv_report("[AudM]paly stream req: g_prCurrent_player:0x%x",1, g_prCurrent_player);
    if (g_prCurrent_player != NULL) {
        if (recoder_current_ptr->priority < g_prCurrent_player->priority) {
            g_rAm_aud_id[bAud_id].use = ID_SUSPEND_STATE;
            audio_src_srv_report("[AudM]paly stream req: suspend, In_p:%d, Current_p:%d",2, recoder_current_ptr->priority, g_prCurrent_player->priority);
            recoder_current_ptr->notify_cb(bAud_id, AUD_SUSPEND_IND, AUD_EMPTY, NULL);
        } else {
            am_audio_set_suspend(recoder_current_ptr->type, g_prCurrent_player);
            g_prCurrent_player = recoder_current_ptr;
            am_audio_set_play(g_prCurrent_player);
        }
    } else {
        g_prCurrent_player = recoder_current_ptr;
        am_audio_set_play(g_prCurrent_player);
    }
    }
}

/*****************************************************************************
 * FUNCTION
 *
 *
 *
 *****************************************************************************/
#ifdef MTK_RECORD_ENABLE

static void aud_prepare_wwe_para(uint32_t language_mode_address, uint32_t language_mode_length)
{
    uint32_t nvkey_vad_length_common, nvkey_vad_length_para;

    n9_dsp_share_info_t *n9_dsp_share_info = hal_audio_query_record_share_info();
    mcu2dsp_vad_param_p mcu2dsp_vad_param = (mcu2dsp_vad_param_p)(n9_dsp_share_info->start_addr);

    if(NVDM_STATUS_NAT_OK == flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_VAD_COMMON, &nvkey_vad_length_common)) {
        flash_memory_read_nvdm_data(NVKEY_DSP_PARA_VAD_COMMON, (uint8_t *)&mcu2dsp_vad_param->vad_nvkey_common, &nvkey_vad_length_common);
    } else {
        audio_src_srv_err("[VAD_NVKEY]Failded to Read VAD Para, Para ID = [0x%04x]", 1, NVKEY_DSP_PARA_VAD_COMMON);
    }

    if(NVDM_STATUS_NAT_OK == flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_VAD_1MIC_V_MODE, &nvkey_vad_length_para)) {
        flash_memory_read_nvdm_data(NVKEY_DSP_PARA_VAD_1MIC_V_MODE, (uint8_t *)&mcu2dsp_vad_param->vad_nvkey_1mic_v_mode, &nvkey_vad_length_para);
    } else {
        audio_src_srv_err("[VAD_NVKEY]Failded to Read VAD Para, Para ID = [0x%04x]", 1, NVKEY_DSP_PARA_VAD_1MIC_V_MODE);
    }

    if(NVDM_STATUS_NAT_OK == flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_VAD_1MIC_C_MODE, &nvkey_vad_length_para)) {
        flash_memory_read_nvdm_data(NVKEY_DSP_PARA_VAD_1MIC_C_MODE, (uint8_t *)&mcu2dsp_vad_param->vad_nvkey_1mic_c_mode, &nvkey_vad_length_para);
    } else {
        audio_src_srv_err("[VAD_NVKEY]Failded to Read VAD Para, Para ID = [0x%04x]", 1, NVKEY_DSP_PARA_VAD_1MIC_C_MODE);
    }

    if(NVDM_STATUS_NAT_OK == flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_VAD_2MIC_V_MODE, &nvkey_vad_length_para)) {
        flash_memory_read_nvdm_data(NVKEY_DSP_PARA_VAD_2MIC_V_MODE, (uint8_t *)&mcu2dsp_vad_param->vad_nvkey_2mic_v_mode, &nvkey_vad_length_para);
    } else {
        audio_src_srv_err("[VAD_NVKEY]Failded to Read VAD Para, Para ID = [0x%04x]", 1, NVKEY_DSP_PARA_VAD_2MIC_V_MODE);
    }
    if(NVDM_STATUS_NAT_OK == flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_VAD_2MIC_C_MODE, &nvkey_vad_length_para)) {
        flash_memory_read_nvdm_data(NVKEY_DSP_PARA_VAD_2MIC_C_MODE, (uint8_t *)&mcu2dsp_vad_param->vad_nvkey_2mic_c_mode, &nvkey_vad_length_para);
    } else {
        audio_src_srv_err("[VAD_NVKEY]Failded to Read VAD Para, Para ID = [0x%04x]", 1, NVKEY_DSP_PARA_VAD_2MIC_C_MODE);
    }

    mcu2dsp_vad_param->language_mode_address = language_mode_address;
    mcu2dsp_vad_param->language_mode_length = language_mode_length;

    audio_src_srv_err("[VAD_NVKEY]language_mode_address = [0x%08x], language_mode_length = [0x%08x]", 2, language_mode_address, language_mode_length);

    /*read vow config param*/
    if (NVDM_STATUS_NAT_OK == flash_memory_query_nvdm_data_length(NVKEYID_DSP_FW_PARA_VOW_PARAMETERS, &nvkey_vad_length_common)) {
        flash_memory_read_nvdm_data(NVKEYID_DSP_FW_PARA_VOW_PARAMETERS, (uint8_t *)&mcu2dsp_vad_param->vow_setting, &nvkey_vad_length_common);
    }else{
        audio_src_srv_err("[VAD_NVKEY]Failded to Read VOW Para, Para ID = [0x%04x]", 1, NVKEYID_DSP_FW_PARA_VOW_PARAMETERS);
    }
    /*vow mic config*/
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        mcu2dsp_vad_param->vow_setting.main_mic = (uint16_t)hal_audio_convert_mic_config(audio_nvdm_HW_config.VAD_scenario.VAD_phase0_Main_Input_Select);
        mcu2dsp_vad_param->vow_setting.ref_mic  = (uint16_t)hal_audio_convert_mic_config(audio_nvdm_HW_config.VAD_scenario.VAD_phase0_Ref_Input_Select);
        /*all vow mic use same adda setting*/
        mcu2dsp_vad_param->adda_analog_mic_mode = audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC0_Mode;
        audio_src_srv_report("[VAD_NVKEY]main_mic = %d,ref_mic = %d",2,mcu2dsp_vad_param->vow_setting.main_mic,mcu2dsp_vad_param->vow_setting.ref_mic);
        audio_src_srv_report("[VAD_VOW]adda_analog_mic_mode = %d",1,mcu2dsp_vad_param->adda_analog_mic_mode);
    #endif
}

static void am_audio_record_sink_play(bt_sink_srv_am_background_t *background_ptr){
    hal_audio_status_t result;
    g_stream_in_code_type = background_ptr->local_context.record_format.record_codec.codec_cap.codec_type;
    g_bit_rate = background_ptr->local_context.record_format.record_codec.codec_cap.bit_rate;
    g_wwe_mode = background_ptr->local_context.record_format.record_codec.codec_cap.wwe_mode;
    uint32_t language_mode_address =  background_ptr->local_context.record_format.record_codec.codec_cap.wwe_language_mode_address;
    uint32_t language_mode_length =  background_ptr->local_context.record_format.record_codec.codec_cap.wwe_language_mode_length;
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_IN, HAL_AUDIO_SAMPLING_RATE_16KHZ, true);
#endif

    if(g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_ANC_LC) {
        #ifdef MTK_LEAKAGE_DETECTION_ENABLE
        audio_src_srv_report("[RECORD_LC]hal_audio_start_stream_in_leakage_compensation, codec_type:0x%x\r\n", 1, g_stream_in_code_type);
        result = hal_audio_start_stream_in_leakage_compensation();
        #else
        result = HAL_AUDIO_STATUS_INVALID_PARAMETER;
        #endif
#if defined(MTK_USER_TRIGGER_FF_ENABLE) & defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2)
    } else if (g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_SZ) {
        audio_src_srv_report("[user_trigger_ff]hal_audio_start_stream_in_user_trigger_adaptive_ff, codec_type:0x%x\r\n", 1, g_stream_in_code_type);
        result = hal_audio_start_stream_in_user_trigger_adaptive_ff(2);

    } else if (g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ) {
        audio_src_srv_report("[user_trigger_ff]hal_audio_start_stream_in_user_trigger_adaptive_ff, codec_type:0x%x\r\n", 1, g_stream_in_code_type);
        result = hal_audio_start_stream_in_user_trigger_adaptive_ff(1);

    } else if (g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ_FIR) {
        audio_src_srv_report("[user_trigger_ff]hal_audio_start_stream_in_user_trigger_adaptive_ff, codec_type:0x%x\r\n", 1, g_stream_in_code_type);
        result = hal_audio_start_stream_in_user_trigger_adaptive_ff(0);
#endif
    } else {
        #ifdef MTK_USER_TRIGGER_FF_ENABLE
        #ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
        if(g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF) {
            audio_anc_user_trigger_ff_write_Sz_coef_share_buff();
        }
        #endif
        #endif
        audio_src_srv_report("hal_audio_start_stream_in, codec_type:0x%x\r\n", 1, g_stream_in_code_type);
        #ifdef MTK_USB_AUDIO_RECORD_ENABLE
        result = hal_audio_start_stream_in_usb(HAL_AUDIO_RECORD_VOICE);
        #else
        if(g_wwe_mode != WWE_MODE_NONE) {
            aud_prepare_wwe_para(language_mode_address, language_mode_length);
        }
        audio_src_srv_report("hal_audio_start_stream_in, codec_type:0x%x\r\n", 1, g_stream_in_code_type);
        result = hal_audio_start_stream_in(HAL_AUDIO_RECORD_VOICE);
        #endif
    }

    if(result == HAL_AUDIO_STATUS_OK){
        g_prCurrent_playback[2]->notify_cb(g_prCurrent_playback[2]->aud_id, AUD_SINK_OPEN_CODEC, AUD_CMD_COMPLETE, NULL);
    } else {
        g_prCurrent_playback[2]->notify_cb(g_prCurrent_playback[2]->aud_id, AUD_SINK_OPEN_CODEC, AUD_CMD_FAILURE, NULL);
    }
    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if((((audio_nvdm_HW_config.Voice_InputDev & 0xC0) >> 6) == 0x02) && (((audio_nvdm_HW_config.Voice_OutputDev & 0xF0) >> 4) == 0x03)){
    #else
        if (((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x02) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03)) ||
            ((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x04) && (audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select <= 0x60) && (audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select >= 0x10) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03))) {
    #endif
        aud_ul_control(true);
    }
}
static void am_audio_record_sink_stop(void){
    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if((((audio_nvdm_HW_config.Voice_InputDev & 0xC0) >> 6) == 0x02) && (((audio_nvdm_HW_config.Voice_OutputDev & 0xF0) >> 4) == 0x03)){
    #else
        if (((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x02) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03)) ||
            ((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x04) && (audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select <= 0x60) && (audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select >= 0x10) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03))) {
    #endif
        aud_ul_control(false);
    }
    if(g_stream_in_code_type == AUDIO_DSP_CODEC_TYPE_ANC_LC) {
        #ifdef MTK_LEAKAGE_DETECTION_ENABLE
        hal_audio_stop_stream_in_leakage_compensation();
        #endif
    } else {
        hal_audio_stop_stream_in();
    }
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
    ami_set_afe_param(STREAM_IN, HAL_AUDIO_SAMPLING_RATE_16KHZ, false);
#endif
}
#endif /*MTK_RECORD_ENABLE*/
uint32_t am_audio_check_pri_table(uint16_t index,
                                         bt_sink_srv_am_priority_table_t *ori_pri_table,
                                         bt_sink_srv_am_priority_table_t *cur_pri_table){
    uint32_t higher_table = 0x0;
#if 1
    if(index == 0){
        if(ori_pri_table->DL_1 < cur_pri_table->DL_1){
            higher_table |= AM_PLAYBACK_DL1;
        }
    }else if(index == 1){
        if(ori_pri_table->DL_2 < cur_pri_table->DL_2){
            higher_table |= AM_PLAYBACK_DL2;
        }
    }else if(index == 2){
        if(ori_pri_table->UL_1 < cur_pri_table->UL_1){
            higher_table |= AM_PLAYBACK_UL1;
        }
    }else if(index == 3){
        if(ori_pri_table->UL_2 < cur_pri_table->UL_2){
            higher_table |= AM_PLAYBACK_UL2;
        }
    }
#else
    if(ori_pri_table + index < cur_pri_table + index){
        higher_table |= 1 << index;
    }
#endif
    audio_src_srv_report("[AudM] check_pri_table index(%d) preempt (0x%x)",2, index, higher_table );
    return higher_table;
}

am_playback_arbitration_result_t am_audio_get_permission_request(bt_sink_srv_am_background_t *request) {
    am_playback_arbitration_result_t result;
    result.am_arbitration_status = AM_PLAYBACK_REJECT;
    result.preempt_index = 0x0;
    bt_sink_srv_am_priority_table_t pri_table_NULL = {AUD_LOW    , AUD_LOW  , AUD_LOW    , AUD_LOW};
    for (uint16_t i = 0; i < AM_PLAYBACK_INDEX_TOTAL; i++){
        if(g_prCurrent_playback[i]!=NULL){
            if(am_audio_check_pri_table(i, &g_prCurrent_playback[i]->priority_table, &request->priority_table) != 0x0){
                result.preempt_index |= 1 << i;
           }
        }
        else if(am_audio_check_pri_table(i, &pri_table_NULL, &request->priority_table) != 0x0){
            result.preempt_index |= 1 << i;
        }
    }
    if(result.preempt_index != 0x0){
        result.am_arbitration_status = AM_PLAYBACK_PERMISSION;
    }
    if(g_prHfp_media_handle != NULL){
        result.am_arbitration_status = AM_PLAYBACK_REJECT;
    }
    return result;
}

static void test_aud_set_play_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t         bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;
    //bt_sink_srv_am_background_t *recoder_high_t = NULL;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if 0
    if (g_prCurrent_resumer != NULL) {
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_rAm_aud_id[g_prCurrent_resumer->aud_id].use = ID_IDLE_STATE;
        g_prCurrent_resumer = NULL;
    }
#endif
    audio_src_srv_report("[AudM]TEST_play_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x",4,
        bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);

    if (recoder_current_ptr) {
        recoder_current_ptr->type = am_background_temp->type;
        recoder_current_ptr->audio_path_type = am_background_temp->audio_path_type;
        memcpy(&(recoder_current_ptr->local_context), &(am_background_temp->local_context), sizeof(bt_sink_srv_am_codec_t));
        memcpy(&(recoder_current_ptr->audio_stream_in), &(am_background_temp->audio_stream_in), sizeof(bt_sink_srv_am_audio_stream_in_t));
        memcpy(&(recoder_current_ptr->audio_stream_out), &(am_background_temp->audio_stream_out), sizeof(bt_sink_srv_am_audio_stream_out_t));

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
        //bypass permission check
        if(recoder_current_ptr->type == AUDIO_TRANSMITTER){
            am_audio_set_play(recoder_current_ptr);
        }
        else{
#endif
    /*----------------------------------------------------------------*/
    /* Check Permission                                                      */
    /*----------------------------------------------------------------*/
        am_playback_arbitration_result_t am_arbitration;
        am_arbitration = am_audio_get_permission_request(recoder_current_ptr);
        audio_src_srv_report("[AudM] get_permission_request status(%d) preempt_index (0x%x)",2, am_arbitration.am_arbitration_status, am_arbitration.preempt_index);
        if(am_arbitration.am_arbitration_status == AM_PLAYBACK_REJECT){
            #ifdef MTK_RECORD_ENABLE
            if((recoder_current_ptr->type == RECORD) && ((recoder_current_ptr->local_context.record_format.record_codec.codec_cap.codec_type == AUDIO_DSP_CODEC_TYPE_ANC_LC)
            #ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
            || (recoder_current_ptr->local_context.record_format.record_codec.codec_cap.codec_type == AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ)
            || (recoder_current_ptr->local_context.record_format.record_codec.codec_cap.codec_type == AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ_FIR)
            || (recoder_current_ptr->local_context.record_format.record_codec.codec_cap.codec_type == AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_SZ)
            #endif
            )
            && (g_prCurrent_playback[2] != NULL)) {
                audio_src_srv_report("[AudM] Leakage detection will replace normal record",0);
                hal_audio_stop_stream_in();
                #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                ami_execute_vendor_se(EVENT_RECORD_STOP);
                #endif
                 g_prCurrent_playback[2]->notify_cb(g_prCurrent_playback[2]->aud_id, AUD_SUSPEND_BY_IND, AUD_SUSPEND_BY_LC, NULL);
                g_rAm_aud_id[g_prCurrent_playback[2]->aud_id].use = ID_IDLE_STATE;
                g_prCurrent_playback[2] = recoder_current_ptr;
                audio_src_srv_report("[AudM] g_prCurrent_playback[2](0x%x) g_rAm_aud_id(0x%x)",2, g_prCurrent_playback[2], &g_rAm_aud_id[bAud_id].contain_ptr);
                am_audio_set_play(recoder_current_ptr);
            } else
            #endif
            {
                /*NOTIFY Playback play request was fail. */
                recoder_current_ptr->notify_cb(recoder_current_ptr->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
            }
        } else if (am_arbitration.am_arbitration_status == AM_PLAYBACK_PERMISSION) {
            /*Use preempt_index to close playback which have exit*/
            if((am_arbitration.preempt_index & AM_PLAYBACK_DL1)) {
                if(g_prCurrent_playback[0] != NULL){
                    am_audio_set_stop(g_prCurrent_playback[0]);
                }
                g_prCurrent_playback[0] = recoder_current_ptr;
            }
            if((am_arbitration.preempt_index & AM_PLAYBACK_DL2)) {
                if(g_prCurrent_playback[1] != NULL){
                    am_audio_set_stop(g_prCurrent_playback[1]);
                }
                g_prCurrent_playback[1] = recoder_current_ptr;
            }
            if((am_arbitration.preempt_index & AM_PLAYBACK_UL1)) {
                if(g_prCurrent_playback[2] != NULL){
                    am_audio_set_stop(g_prCurrent_playback[2]);
                }
                g_prCurrent_playback[2] = recoder_current_ptr;
                audio_src_srv_report("[AudM] g_prCurrent_playback[2](0x%x) g_rAm_aud_id(0x%x)",2, g_prCurrent_playback[2], &g_rAm_aud_id[bAud_id].contain_ptr);
            }
            if((am_arbitration.preempt_index & AM_PLAYBACK_UL2)) {
                if(g_prCurrent_playback[3] != NULL){
                    am_audio_set_stop(g_prCurrent_playback[3]);
                }
                g_prCurrent_playback[3] = recoder_current_ptr;
            }
            am_audio_set_play(recoder_current_ptr);
        } else {
        /*ERROR*/
        }
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
        }
#endif
#if 0
    /* Find which is high priority */
    recoder_high_t = g_rBackground_head.next;
    while ((recoder_high_t != NULL) && (recoder_high_t->priority > recoder_current_ptr->priority)) {
        recoder_high_t = recoder_high_t->next;
    }
    if (recoder_high_t && (recoder_high_t->aud_id != bAud_id)) {
        if (recoder_current_ptr->next != NULL) {
            recoder_current_ptr->next->prior = recoder_current_ptr->prior;
        }
        recoder_current_ptr->prior->next = recoder_current_ptr->next;
        recoder_high_t->prior->next = recoder_current_ptr;
        recoder_current_ptr->prior = recoder_high_t->prior;
        recoder_high_t->prior = recoder_current_ptr;
        recoder_current_ptr->next = recoder_high_t;
    }
    audio_src_srv_report("[AudM]paly stream req: g_prCurrent_player:0x%x",1, g_prCurrent_player);
    if (g_prCurrent_player != NULL) {
        if (recoder_current_ptr->priority < g_prCurrent_player->priority) {
            g_rAm_aud_id[bAud_id].use = ID_SUSPEND_STATE;
            audio_src_srv_report("[AudM]paly stream req: suspend, In_p:%d, Current_p:%d",2, recoder_current_ptr->priority, g_prCurrent_player->priority);
            recoder_current_ptr->notify_cb(bAud_id, AUD_SUSPEND_IND, AUD_EMPTY, NULL);
        } else {
            am_audio_set_suspend(recoder_current_ptr->type, g_prCurrent_player);
            g_prCurrent_player = recoder_current_ptr;
            am_audio_set_play(g_prCurrent_player);
        }
    } else {
        g_prCurrent_player = recoder_current_ptr;
        am_audio_set_play(g_prCurrent_player);
    }
#endif
    }
}

static void test_aud_set_stop_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]stop_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x\n",
        //bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);

    audio_src_srv_report("[AudM]Test_stop_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x",4,
        bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);
    if(g_rAm_aud_id[bAud_id].use == ID_IDLE_STATE) {
        audio_src_srv_report("[AudM]Test_stop_hdr-id, state idle, no need to do anything",0);
        return;
    }

    //g_prCurrent_player = NULL;
    am_audio_set_stop(recoder_current_ptr);
    am_audio_search_suspended();
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_stop_stream_req_hdlr
 * DESCRIPTION
 *  Stop playing the specified audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_stop_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    //printf("[AudM]stop_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x\n",
        //bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);

    audio_src_srv_report("[AudM]stop_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x",4,
        bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);

    g_prCurrent_player = NULL;
    am_audio_set_stop(recoder_current_ptr);
    am_audio_search_suspended();
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_close_stream_req_hdlr
 * DESCRIPTION
 *  Close the opening audio handler.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_close_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_rAm_aud_id[bAud_id].use == ID_RESUME_STATE) {
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_prCurrent_resumer = NULL;
    }

    //printf("[AudM]close_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x\n",
        //bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);
    audio_src_srv_report("[AudM]close_hdr-id: %d, num: %d, use: %d, c_ptr: 0x%x",4,
        bAud_id, g_aud_id_num, g_rAm_aud_id[bAud_id].use, (unsigned int)recoder_current_ptr);
    if(g_rAm_aud_id[bAud_id].use > ID_IDLE_STATE) {
        audio_src_srv_report("[AudM]close_hdr-id: %d, !!!!!!State abnormal, need to check!!!!!",1,bAud_id);
    }
    if (recoder_current_ptr) {
    g_rAm_aud_id[bAud_id].contain_ptr = NULL;
    //recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ, AUD_CMD_COMPLETE, NULL);
    if (recoder_current_ptr->next != NULL) {
        recoder_current_ptr->next->prior = recoder_current_ptr->prior;
    }
    if (recoder_current_ptr->prior != NULL) {
        recoder_current_ptr->prior->next = recoder_current_ptr->next;
    }
#ifndef WIN32_UT
    vPortFree(recoder_current_ptr);
#else
    free(recoder_current_ptr);
#endif

    if (g_aud_id_num == 1) {
        if (aud_memory != NULL) {
#ifndef WIN32_UT
            vPortFree(aud_memory);
#else
            free(aud_memory);
#endif
            aud_memory = NULL;
        }
    }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_set_volume_stream_out_req_hdlr
 * DESCRIPTION
 *  Set audio in/out volume.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_set_volume_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_volume_level_t level;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (!g_prCurrent_player) {
        return ;
    }

    if (am_background_temp->in_out == STREAM_OUT) {
        level = (bt_sink_srv_am_volume_level_t)am_background_temp->audio_stream_out.audio_volume;
        g_prCurrent_player->audio_stream_out.audio_volume = (bt_sink_srv_am_volume_level_out_t)level;
        if (g_prCurrent_player->type == HFP) {
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, g_prCurrent_player->audio_stream_out.audio_device, level);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
#ifdef AIR_BT_CODEC_BLE_ENABLED
            } else if(g_prCurrent_player->type == BLE){
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

                if(g_prCurrent_player->local_context.ble_format.ble_codec.channel_mode != CHANNEL_MODE_DL_ONLY) {
                    vol_info.type = VOL_HFP;
                    vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_CVSD;
                    vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                    vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                    vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                } else {
                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                }
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            #ifdef MTK_VENDOR_VOLUME_TABLE_ENABLE
            g_prCurrent_player->vol_type = VOL_HFP;
            #endif
            aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, g_prCurrent_player->audio_stream_out.audio_device, level);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
#endif
        } else {
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                if (g_prCurrent_player->type == PCM) {
                    vol_info.type = VOL_PCM;
                    vol_info.vol_info.pcm_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.pcm_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else if (g_prCurrent_player->type == A2DP) {
                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else if (g_prCurrent_player->type == FILES) {
                    vol_info.type = VOL_MP3;
                    vol_info.vol_info.mp3_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.mp3_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else if (g_prCurrent_player->type == AWS) {
                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else if(g_prCurrent_player->type == LINE_IN){
                #ifdef MTK_LINE_IN_ENABLE
                    vol_info.type = VOL_LINE_IN;
                    vol_info.vol_info.lineIN_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                    vol_info.vol_info.lineIN_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.lineIN_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                    vol_info.vol_info.lineIN_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                #endif
                } else if (g_prCurrent_player->type == USB_AUDIO_IN) {
                    vol_info.type = VOL_USB_AUDIO_IN;
                    vol_info.vol_info.usb_audio_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.usb_audio_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                } else {
                    // Fix Coverity issue
                    vol_info.type = VOL_DEF;
                    vol_info.vol_info.def_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.def_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;

                    audio_src_srv_report("[AudM]volume. Unknown type: %d\n",1, g_prCurrent_player->type);
                }

                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
            aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_out.audio_device, level);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        }
    } else if (am_background_temp->in_out == STREAM_IN) {
        if (g_prCurrent_player->type == HFP) {
            level = (bt_sink_srv_am_volume_level_t)am_background_temp->audio_stream_in.audio_volume;
            g_prCurrent_player->audio_stream_in.audio_volume = (bt_sink_srv_am_volume_level_in_t)level;
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
        #else
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_in.audio_device, level);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
#ifdef AIR_BT_CODEC_BLE_ENABLED
        } else if(g_prCurrent_player->type == BLE){
            level = (bt_sink_srv_am_volume_level_t)am_background_temp->audio_stream_in.audio_volume;
            g_prCurrent_player->audio_stream_in.audio_volume = (bt_sink_srv_am_volume_level_in_t)level;
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_CVSD;
            vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
        #else
            #ifdef MTK_VENDOR_VOLUME_TABLE_ENABLE
            g_prCurrent_player->vol_type = VOL_HFP;
            #endif
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_in.audio_device, level);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
#endif
        } else if(g_prCurrent_player->type == LINE_IN){
        #ifdef MTK_LINE_IN_ENABLE
            level = (bt_sink_srv_am_volume_level_t)am_background_temp->audio_stream_in.audio_volume;
            g_prCurrent_player->audio_stream_in.audio_volume = (bt_sink_srv_am_volume_level_in_t)level;
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;
            vol_info.type = VOL_LINE_IN;
            vol_info.vol_info.lineIN_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
            vol_info.vol_info.lineIN_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
            vol_info.vol_info.lineIN_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
            vol_info.vol_info.lineIN_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
        }
        #else
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_in.audio_device, level);
        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
        #endif /* MTK_LINE_IN_ENABLE */
        } else {
            level = (bt_sink_srv_am_volume_level_t)am_background_temp->audio_stream_in.audio_volume;
            g_prCurrent_player->audio_stream_in.audio_volume = (bt_sink_srv_am_volume_level_in_t)level;
            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, g_prCurrent_player->audio_stream_in.audio_device, level);
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_mute_device_stream_req_hdlr
 * DESCRIPTION
 *  Mute audio input/output device.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_mute_device_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    bool err_flag = false;
    bool mute = false;
    hal_audio_hw_stream_out_index_t hw_gain_index = HAL_AUDIO_STREAM_OUT_ALL; // default mute all
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (am_background_temp->in_out) {
        case STREAM_IN:
            /* code */
            if ((am_background_temp->aud_id != FEATURE_NO_NEED_ID) && (g_prCurrent_player)) {
                g_prCurrent_player->audio_stream_in.audio_mute = am_background_temp->audio_stream_in.audio_mute;
            }
            mute = am_background_temp->audio_stream_in.audio_mute;
            // hw_gain_index = HAL_AUDIO_STREAM_OUT1;
            if ((g_prCurrent_player != NULL) && ((g_prCurrent_player->type == HFP)||(g_prCurrent_player->type == BLE))) {
                hal_audio_mute_stream_in_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, mute);
            } else {
                hal_audio_mute_stream_in(mute); // Actually, mute stream_in won't work.
            }
            return;
        case STREAM_OUT:   // DL1
        case STREAM_OUT_2: // DL2 (VP)
        case STREAM_OUT_3: // DL3
            if ((am_background_temp->aud_id != FEATURE_NO_NEED_ID) && (g_prCurrent_player)) {
                g_prCurrent_player->audio_stream_out.audio_mute = am_background_temp->audio_stream_out.audio_mute;;
            }
            mute = am_background_temp->audio_stream_out.audio_mute;
            hw_gain_index = (am_background_temp->in_out == STREAM_OUT) ? HAL_AUDIO_STREAM_OUT1 :
                    ((am_background_temp->in_out == STREAM_OUT_2) ? HAL_AUDIO_STREAM_OUT2 : HAL_AUDIO_STREAM_OUT3);
            break;
        default:
            err_flag = true;
            break;
    }
    if (err_flag) {
        audio_src_srv_report("[Sink][AM] aud_mute_device_stream_req_hdlr error, in_out = %d", 1, am_background_temp->in_out);
        return;
        }
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
    hal_audio_mute_stream_out(mute, hw_gain_index);
#else
    hal_audio_mute_stream_out(mute);
#endif
}

/*****************************************************************************
 * FUNCTION
 *  aud_config_device_stream_req_hdlr
 * DESCRIPTION
 *  Set audio input/output device.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_config_device_stream_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        g_prCurrent_player->audio_stream_out.audio_device = am_background_temp->audio_stream_out.audio_device;
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
        hal_audio_mute_stream_out(TRUE, HAL_AUDIO_STREAM_OUT1);
#else
        hal_audio_mute_stream_out(TRUE);
#endif
        hal_audio_set_stream_out_device(g_prCurrent_player->audio_stream_out.audio_device);
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT)
        hal_audio_mute_stream_out(FALSE, HAL_AUDIO_STREAM_OUT1);
#else
        hal_audio_mute_stream_out(FALSE);
#endif
    } else if (am_background_temp->in_out == STREAM_IN) {
        g_prCurrent_player->audio_stream_in.audio_device = am_background_temp->audio_stream_in.audio_device;
        hal_audio_mute_stream_in(TRUE);
        hal_audio_set_stream_in_device(g_prCurrent_player->audio_stream_out.audio_device);
        hal_audio_mute_stream_in(FALSE);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_rw_stream_data_req_hdlr
 * DESCRIPTION
 *  Write data to audio output for palyback / Read data to audio input for record.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_rw_stream_data_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t     *am_background_temp = &(amm_ptr->background_info);
    uint8_t                     bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_hal_result_t     eResult = HAL_AUDIO_STATUS_ERROR;
    bt_sink_srv_am_stream_type_t    eIn_out;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_rAm_aud_id[bAud_id].use == ID_RESUME_STATE) {
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM] Resume to play", 0);
#endif
#ifdef RTOS_TIMER
        xTimerStop(g_xTimer_am, 20);
        g_lExpire_count = 0;
#endif
        g_prCurrent_player = g_prCurrent_resumer;
        am_audio_set_play(g_prCurrent_player);
        g_rAm_aud_id[bAud_id].use = ID_PLAY_STATE;
        g_prCurrent_resumer = NULL;
    } else {
#ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM] Continue playing", 0);
#endif
        eIn_out = g_prCurrent_player->local_context.pcm_format.in_out;
        if (eIn_out == STREAM_OUT) {
            eResult = hal_audio_write_stream_out(am_background_temp->local_context.pcm_format.stream.buffer,
                                                 am_background_temp->local_context.pcm_format.stream.size);
            if (eResult == HAL_AUDIO_STATUS_OK) {
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_STREAM_DATA_REQ, AUD_CMD_COMPLETE, NULL);
                return;
            }
        } else if (eIn_out == STREAM_IN) {
            eResult = hal_audio_read_stream_in(am_background_temp->local_context.pcm_format.stream.buffer,
                                               am_background_temp->local_context.pcm_format.stream.size);
            if (eResult == HAL_AUDIO_STATUS_OK) {
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_STREAM_DATA_REQ, AUD_CMD_COMPLETE, NULL);
                return;
            }
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_STREAM_DATA_REQ, AUD_CMD_FAILURE, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_query_stream_len_req_hdlr
 * DESCRIPTION
 *  Query available input/output data length.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_query_stream_len_req_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_background_t     *am_background_temp = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t             bAud_id = am_background_temp->aud_id;
    bt_sink_srv_am_background_t     *recoder_current_ptr = g_rAm_aud_id[bAud_id].contain_ptr;
    bt_sink_srv_am_hal_result_t     eResult = HAL_AUDIO_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (am_background_temp->in_out == STREAM_OUT) {
        eResult = hal_audio_get_stream_out_sample_count(am_background_temp->data_length_ptr);
    } else if (am_background_temp->in_out == STREAM_IN) {
        eResult = hal_audio_get_stream_in_sample_count(am_background_temp->data_length_ptr);
    }
    /* CallBack to AP */
    if (eResult == HAL_AUDIO_STATUS_OK) {
        recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ,  AUD_CMD_COMPLETE, NULL);
    } else {
        recoder_current_ptr->notify_cb(bAud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_bt_codec_a2dp_callback
 * DESCRIPTION
 *  This function is used to send L1Audio events to A.M.
 * PARAMETERS
 *  handle           [IN]
 *  event_id         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_bt_codec_a2dp_callback(bt_media_handle_t *handle, bt_sink_srv_am_bt_event_t event_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player->type == A2DP) {
        #ifdef __BT_AWS_SUPPORT__
        if (event_id == BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW) {
            g_aws_skew_loop_count++;
            if (g_aws_skew_loop_count >= BT_SINK_SRV_AWS_SKEW_LOOP_COUNT) {
                g_aws_skew_loop_count = 0;
                g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                                         MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                                         g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
            }
        } else if (event_id == BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW) {
           if (g_aws_skew_loop_count == BT_SINK_SRV_AWS_SKEW_LOOP_1ST_COUNT) {
               g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                                         MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                                         g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
           }
        } else {
            g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
            bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                                     MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                                     g_prCurrent_player,
                                     TRUE, ptr_callback_amm);
        }
        #else
        g_prCurrent_player->local_context.a2dp_format.a2dp_event = event_id;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_A2DP_PROC_IND,
                                 MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ,
                                 g_prCurrent_player,
                                 TRUE, ptr_callback_amm);
        #endif /* __BT_AWS_SUPPORT__ */
    } else if (g_prCurrent_player->type == AWS) {
        #ifdef __BT_AWS_SUPPORT__
        g_prCurrent_player->local_context.aws_format.aws_event = event_id;
        if (event_id == BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW) {
            g_aws_skew_loop_count++;
            if (g_aws_skew_loop_count >= BT_SINK_SRV_AWS_SKEW_LOOP_COUNT) {
                g_aws_skew_loop_count = 0;
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_AWS_A2DP_PROC_IND,
                                         MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ,
                                         g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
            }
        } else if (event_id == BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW) {
            if (g_aws_skew_loop_count == BT_SINK_SRV_AWS_SKEW_LOOP_1ST_COUNT) {
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_AWS_A2DP_PROC_IND,
                                         MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ,
                                         g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
           }
        } else {
            bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_AWS_A2DP_PROC_IND,
                                     MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ,
                                     g_prCurrent_player,
                                     TRUE, ptr_callback_amm);
        }
        #elif defined(MTK_AWS_MCE_ENABLE)
        g_prCurrent_player->local_context.aws_format.aws_event = event_id;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_AWS_A2DP_PROC_IND,
                                 MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ,
                                 g_prCurrent_player,
                                 TRUE, ptr_callback_amm);
        #endif /* __BT_AWS_SUPPORT__ */
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_bt_codec_hfp_callback
 * DESCRIPTION
 *  This function is used to send L1Audio events to A.M.
 * PARAMETERS
 *  handle           [IN]
 *  event_id         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_bt_codec_hfp_callback(bt_media_handle_t *handle, bt_sink_srv_am_bt_event_t event_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(event_id == BT_CODEC_MEDIA_HFP_AVC_PARA_SEND){
       if(bt_connection_manager_device_local_info_get_aws_role()==BT_AWS_MCE_ROLE_AGENT){
           //printf("[NDVC] It's Agent, send avc_vol");
           if (g_prCurrent_player->type == HFP) {
               g_prCurrent_player->local_context.hfp_format.hfp_event = event_id;
#if 0
               bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_HFP_EVENT_IND,
                                       MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ,
                                       g_prCurrent_player,
                                       TRUE, ptr_callback_amm);
#else
          am_hfp_ndvc_sent_avc_vol(AVC_UPDATE_SYNC);
#endif
           }
       }else{
           //printf("[NDVC] It's Partner, don't send avc_vol");
       }
    }
}

#ifdef AIR_BT_CODEC_BLE_ENABLED
/*****************************************************************************
 * FUNCTION
 *  aud_bt_codec_ble_callback
 * DESCRIPTION
 *  This function is used to send LeAudio events to A.M.
 * PARAMETERS
 *  handle           [IN]
 *  event_id         [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_bt_codec_ble_callback(bt_media_handle_t *handle, bt_sink_srv_am_bt_event_t event_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#if 0

#endif
}
#endif

/*****************************************************************************
 * FUNCTION
 *  aud_stream_out_callback
 * DESCRIPTION
 *  This callback function is notified A.M. for stream-out by AUD HISR.
 * PARAMETERS
 *  event            [OUT]
 *  user_data        [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_stream_out_callback(bt_sink_srv_am_event_result_t event, void *user_data)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player == NULL) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        g_prCurrent_player->local_context.pcm_format.event = event;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_STREAM_EVENT_IND,
                                 MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ,
                                 g_prCurrent_player,
                                 TRUE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_stream_in_callback
 * DESCRIPTION
 *  This callback function is notified A.M. for stream-in by AUD HISR.
 * PARAMETERS
 *  event            [OUT]
 *  user_data        [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_stream_in_callback(bt_sink_srv_am_event_result_t event, void *user_data)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prCurrent_player == NULL) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        g_prCurrent_player->local_context.pcm_format.event = event;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_STREAM_EVENT_IND,
                                 MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ,
                                 g_prCurrent_player,
                                 TRUE, ptr_callback_amm);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_process_a2dp_callback_hdlr
 * DESCRIPTION
 *  This function is used to handle A2DP process callback.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_process_a2dp_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_bt_event_t event_id = amm_ptr->background_info.local_context.a2dp_format.a2dp_event;
    //bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
#endif
    bt_sink_srv_am_device_set_t dev;
    bt_sink_srv_am_volume_level_out_t vol;
    uint32_t digital_vol = 0x7FFF, analog_vol = 0x0002;
#ifdef __BT_AWS_SUPPORT__
    uint32_t sampling_rate = 44100;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        g_prA2dp_sink_handle->process(g_prA2dp_sink_handle, event_id);

        switch (event_id) {
            case BT_CODEC_MEDIA_ERROR: {
#ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                //g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                //g_rSink_state = A2DP_SINK_CODEC_STOP;
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    ;
                }
                if (g_am_volume_enable) {
                    audio_src_srv_report("[sink][AM]-d_gain: 0x%x, a_gain: 0x%x",2, digital_vol, analog_vol);
                } else {
                    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                    {
                        bt_sink_srv_audio_setting_vol_info_t vol_info;

                        vol_info.type = VOL_A2DP;
                        vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                        vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                        bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                    }
                    #else
                    hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                    #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                }
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_CODEC_RESTART, NULL);
#else
                g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                g_rSink_state = A2DP_SINK_CODEC_STOP;
                g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    ;
                }
                if (g_am_volume_enable) {
                    ;
                } else {
                    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                    {
                        bt_sink_srv_audio_setting_vol_info_t vol_info;

                        vol_info.type = VOL_A2DP;
                        vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                        vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                        bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                    }
                    #else
                    hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                    #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                }

                //eResult = g_prA2dp_sink_handle->play(g_prA2dp_sink_handle);
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_CODEC_RESTART, NULL);
                if (eResult == BT_CODEC_MEDIA_STATUS_ERROR) {
                    audio_src_srv_report("[sink][AM] Error", 0);
                }
                audio_src_srv_report("[sink][AM]-restart play", 0);
#endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
                break;
            }

            #ifdef __BT_AWS_SUPPORT__
            case BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW: {
                bt_sink_srv_fetch_bt_offset();
                break;
            }

            case BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW: {
                if (BT_AWS_CODEC_TYPE_SBC == g_aws_codec_type) {
                    switch (g_aws_sample_rate) {
                        case 8:
                            sampling_rate = 16000;
                            break;
                        case 4:
                            sampling_rate = 32000;
                            break;
                        case 2:
                            sampling_rate = 44100;
                            break;
                        case 1:
                            sampling_rate = 48000;
                            break;

                        default:
                            sampling_rate = 44100;
                            break;
                    }
                } else if (BT_AWS_CODEC_TYPE_AAC == g_aws_codec_type){
                    switch (g_aws_sample_rate) {
                        case 0x800:
                            sampling_rate = 8000;
                            break;
                        case 0x400:
                            sampling_rate = 11025;
                            break;
                        case 0x200:
                            sampling_rate = 12000;
                            break;
                        case 0x100:
                            sampling_rate = 16000;
                            break;
                        case 0x80:
                            sampling_rate = 22050;
                            break;
                        case 0x40:
                            sampling_rate = 24000;
                            break;
                        case 0x20:
                            sampling_rate = 32000;
                            break;
                        case 0x10:
                            sampling_rate = 44100;
                            break;
                        case 0x8:
                            sampling_rate = 48000;
                            break;
                        default:
                            sampling_rate = 44100;
                            break;
                    }
                } else if (BT_AWS_CODEC_TYPE_VENDOR == g_aws_codec_type){
                    switch (g_aws_sample_rate) {
                        case 0x20:
                            sampling_rate = 44100;
                            break;
                        case 0x10:
                            sampling_rate = 48000;
                            break;
                        case 0x08:
                            sampling_rate = 88200;
                            break;
                        case 0x04:
                            sampling_rate = 96000;
                            break;
                        case 0x02:
                            sampling_rate = 176400;
                            break;
                        case 0x01:
                            sampling_rate = 192000;
                            break;
                        default:
                            sampling_rate = 44100;
                            break;
                    }
                }
                bt_sink_srv_audio_sync_calc_t audio_calc;
                audio_calc.media_handle= g_prA2dp_sink_handle;
                audio_calc.sampling_rate = sampling_rate;
                audio_calc.type = g_aws_codec_type;
                bt_sink_srv_audio_clock_calibrate(&audio_calc);
                break;
            }
            #endif /* __BT_AWS_SUPPORT__ */

            case BT_CODEC_MEDIA_UNDERFLOW: {
                g_a2dp_underflow_loop_count++;
                if (g_a2dp_underflow_loop_count >= BT_SINK_SRV_AM_MAX_UNDERFLOW_COUNT) {
                    g_a2dp_underflow_loop_count = 0;
                    g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_AWS_UNDERFLOW, NULL);
                }
                break;
            }

#if defined(MTK_AVM_DIRECT)
            case BT_CODEC_MEDIA_TIME_REPORT: {
                audio_dsp_a2dp_dl_time_param_t *p_param = hal_audio_a2dp_dl_get_time_report();
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_TIME_REPORT, p_param);
                return;  // avoid to callback twice
            }

            case BT_CODEC_MEDIA_LTCS_DATA_REPORT: {
                audio_dsp_a2dp_ltcs_report_param_t param;
                param.p_ltcs_asi_buf = hal_audio_query_ltcs_asi_buf();
                param.p_ltcs_min_gap_buf = hal_audio_query_ltcs_min_gap_buf();
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_LTCS_REPORT, &param);
                return;
            }

            case BT_CODEC_MEDIA_AUDIO_DL_REINIT_REQUEST: {
                uint32_t param_reinit = hal_audio_dsp2mcu_data_get();
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_DL_REINIT_REQUEST, &param_reinit);
                break;
            }

            case BT_CODEC_MEDIA_AUDIO_DL_ALC_REQUEST: {
                uint32_t ALC_latency = hal_audio_dsp2mcu_AUDIO_DL_ACL_data_get();
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_ACTIVE_LATENCY_REQUEST, &ALC_latency);
                return;
            }
#endif

            default:
                break;
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, (bt_sink_srv_am_cb_sub_msg_t)event_id, NULL);
        _UNUSED(digital_vol);
        _UNUSED(analog_vol);
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_process_hfp_callback_hdlr
 * DESCRIPTION
 *  This function is used to inform HF AP about hf error event.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_process_hfp_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_bt_event_t event_id = amm_ptr->background_info.local_context.hfp_format.hfp_event;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        if (event_id == BT_CODEC_MEDIA_ERROR) {
            bt_codec_am_hfp_stop();
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_HFP_EVENT_IND, (bt_sink_srv_am_cb_sub_msg_t)event_id, NULL);
    }
#if 0
    if(g_prCurrent_player != NULL){
        if (g_prCurrent_player->type == HFP){
            if (event_id == BT_CODEC_MEDIA_HFP_AVC_PARA_SEND) {
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, (bt_sink_srv_am_cb_sub_msg_t)AUD_HFP_AVC_PARA_SEND, NULL);
            }
        }
    } else {
        audio_src_srv_report("[Sink][AM]HFP Callback event(%d) error. Current_player NULL", 1, event_id);
    }
#endif
}

/*****************************************************************************
 * FUNCTION
 *  aud_event_stream_callback_hdlr
 * DESCRIPTION
 *  This function is used to handle stream event callback.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_event_stream_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_event_result_t eEvent = amm_ptr->background_info.local_context.pcm_format.event;
    bt_sink_srv_am_id_t bAud_id = amm_ptr->background_info.aud_id;
    bt_sink_srv_am_stream_type_t    eIn_out;
    uint32_t                    data_length = 0;
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if ((g_prCurrent_player == NULL) || (bAud_id != g_prCurrent_player->aud_id)) {
        return;
    }
    if (g_prCurrent_player->type == PCM) {
        eIn_out = g_prCurrent_player->local_context.pcm_format.in_out;
        if (eEvent == HAL_AUDIO_EVENT_UNDERFLOW) {
            if (eIn_out == STREAM_OUT) {
                hal_audio_stop_stream_out();
            } else if (eIn_out == STREAM_IN) {
                hal_audio_stop_stream_in();
                ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_RECORD, false);
            }
            g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_IDLE_STATE;
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_UNDERFLOW, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_DATA_REQUEST) {
            if (eIn_out == STREAM_OUT) {
                hal_audio_get_stream_out_sample_count(&data_length);
            } else if (eIn_out == STREAM_IN) {
                hal_audio_get_stream_in_sample_count(&data_length);
            }
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_DATA_REQ, &data_length);
        } else if (eEvent == HAL_AUDIO_EVENT_ERROR) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_ERROR, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_NONE) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_UNDERFLOW, NULL);
        } else if (eEvent == HAL_AUDIO_EVENT_DATA_NOTIFICATION) {
            g_prCurrent_player->notify_cb(bAud_id, AUD_STREAM_EVENT_IND, AUD_STREAM_EVENT_DATA_NOTIFICATION, NULL);
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_process_dsp_nvdm_setting
 * DESCRIPTION
 *  This function is used to read setting value from nvram and set to dsp
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
#if (PRODUCT_VERSION == 2533)
static void aud_process_dsp_nvdm_setting(void)
{
    int i;

    if (external_dsp_is_power_on()) {
        const audio_eaps_t *am_speech_eaps = audio_nvdm_get_global_eaps_address();
        uint32_t download_index = 0;
        uint16_t download_value = 0;

        //audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        audio_src_srv_report("[Sink][AM][EXTDSP]--cur: 0x%08x, type: 0x%08x, c_type: %d",3,
            g_prCurrent_player, g_prCurrent_player->type, g_prCurrent_player->local_context.hfp_format.hfp_codec.type);

        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_CUSTOMER_REGISTER_AMOUNT; i++) {
            if (g_prCurrent_player &&
               (g_prCurrent_player->type == HFP) &&
               (BT_HFP_CODEC_TYPE_MSBC == g_prCurrent_player->local_context.hfp_format.hfp_codec.type)) {
                if(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag) {
                    download_index = (uint32_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value;
                    download_value = (uint16_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value;
                    external_dsp_write_parameter(download_index, download_value);
                    audio_src_srv_report("[Sink][AM][EXTDSP]WB nvdm setting:i=%d, download_index=%d, download_value=0x%x",3, i, download_index, download_value);
                    vTaskDelay(20);
                }
            } else if (g_prCurrent_player &&
                      (g_prCurrent_player->type == HFP) &&
                      (BT_HFP_CODEC_TYPE_CVSD == g_prCurrent_player->local_context.hfp_format.hfp_codec.type)) {
                if(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].need_to_download_flag) {
                    download_index = (uint32_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].index_value;
                    download_value = (uint16_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.external_dsp_sending_path_register_info[i].register_value;
                    external_dsp_write_parameter(download_index, download_value);
                    audio_src_srv_report("[Sink][AM][EXTDSP]NB nvdm setting:i=%d, download_index=%d, download_value=0x%x",3, i, download_index, download_value);
                    vTaskDelay(20);
                }
            } else {
                audio_src_srv_report("[Sink][AM][EXTDSP] error", 0);
            }
        }
        //vPortFree(am_speech_eaps);


/*
        // get audio data
        audio_nvdm_get_external_dsp_register_value_by_memcpy(DSP_REGISTER_PROFILE_BT, &external_dsp_sending_path_register_value);

        // write to dsp
        for (i = 0; i < EXTERNAL_DSP_REGISTER_SENDING_PATH_INDEX_AMOUNT; i++) {
            if(external_dsp_sending_path_register_value.need_to_download_index[i]) {
                external_dsp_write_parameter(i, external_dsp_sending_path_register_value.external_dsp_register_value[i]);
                audio_src_srv_report("[Sink][AM][EXTDSP]nvdm setting:%d=>0x%x",2, i, external_dsp_sending_path_register_value.external_dsp_register_value[i]);
                vTaskDelay(20);
            }
        }
*/
    } else {
        audio_src_srv_report("[Sink][AM][EXTDSP][ERROR] DSP power off", 0);
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  bt_get_write_buffer_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "get_write_buffer" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  buffer           [OUT]
 *  length           [OUT]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_get_write_buffer_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->get_write_buffer(g_prA2dp_sink_handle, buffer, length);
        return BT_CODEC_MEDIA_STATUS_OK;
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_write_data_done_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "write data done" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_write_data_done_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id, uint32_t length)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->write_data_done(g_prA2dp_sink_handle, length);
        return BT_CODEC_MEDIA_STATUS_OK;
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_finish_write_data_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "finish_write_data" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
static bt_status_t bt_finish_write_data_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->finish_write_data(g_prA2dp_sink_handle);
        return BT_CODEC_MEDIA_STATUS_OK;
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_get_free_space_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "get_free_space" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 *  length           [IN]
 * RETURNS
 *  int32_t
 *****************************************************************************/
static int32_t bt_get_free_space_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id)
{
    if (aud_id == g_prCurrent_player->aud_id) {
        return g_prA2dp_sink_handle->get_free_space(g_prA2dp_sink_handle);
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  bt_reset_share_buffer_a2dp_sink_codec
 * DESCRIPTION
 *  This function is used to process "reset_share_buffer" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void bt_reset_share_buffer_a2dp_sink_codec(bt_sink_srv_am_id_t aud_id)
{
    if (aud_id == g_prCurrent_player->aud_id) {
        g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
    }
}


#if defined(MTK_AVM_DIRECT)
/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_open_callback
 * DESCRIPTION
 *  This function is used to callback after A2DP sink open for asynchronous notification.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
void aud_process_a2dp_open_callback_hdlr(void)
{
    g_rSink_state = A2DP_SINK_CODEC_OPEN;
    g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
    if (g_prCurrent_player->type == A2DP) {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, &g_prA2dp_sink_event_handle);
    } else if (g_prCurrent_player->type == AWS) {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, &g_prA2dp_sink_event_handle);
    } else {
        /* trace error */
    }
}

static void bt_codec_am_a2dp_sink_open_callback(void)
{
    aud_process_a2dp_open_callback_hdlr();
}

#endif

/*****************************************************************************
 * FUNCTION
 *  aud_dl_suspend
 * DESCRIPTION
 *  This function is used to suspend DL 1
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_dl_suspend(void)
{
    #if defined(MTK_AVM_DIRECT)
    if(!(g_am_task_mask & AM_TASK_MASK_DL_SUSPEND))
    {
        audio_src_srv_report("[Sink][AM]aud_dl_suspend\n", 0);
        ami_set_audio_mask(AM_TASK_MASK_DL_SUSPEND, true);
        if((g_prCurrent_player->type == A2DP) || (g_prCurrent_player->type == AWS))
        {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_SUSPEND, 0, 0, true);
        }
        else if((g_prCurrent_player->type == HFP) || (g_prCurrent_player->type == PCM))
        {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_SUSPEND, 0, 0, true);
        }
        else if(g_prCurrent_player->type == LINE_IN)
        {
        #ifdef MTK_LINE_IN_ENABLE
            #if defined(MTK_PURE_LINEIN_PLAYBACK_ENABLE)
                pure_linein_playback_close();
            #else
                #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    if (0x01 == audio_nvdm_HW_config.audio_scenario.Audio_Pure_Linein_enable) {
                        pure_linein_playback_close();
                    }else {
                        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_SUSPEND, 0, 0, true);
                    }
                #else
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_SUSPEND, 0, 0, true);
                #endif
            #endif
        #endif
        } else if(g_prCurrent_player->type == USB_AUDIO_IN)
        {
        #ifdef MTK_USB_AUDIO_PLAYBACK_ENABLE
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PLAYBACK_SUSPEND, 0, 0, true);
        #endif
        } else if(g_prCurrent_player->type == BLE)
        {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_DL_SUSPEND, 0, 0, true);
        }
    }
    else
    {
        audio_src_srv_report("[Sink][AM]aud_dl_suspend Fail: Already suspended\n", 0);
    }
    #endif
}

static void aud_ul_suspend(void)
{
    #if defined(MTK_AVM_DIRECT)
    if(!(g_am_task_mask & AM_TASK_MASK_UL_SUSPEND))
    {
        audio_src_srv_report("[Sink][AM]aud_ul_suspend\n", 0);
        ami_set_audio_mask(AM_TASK_MASK_UL_SUSPEND, true);
        if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD))
        {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_SUSPEND, 0, 0, true);
        }
        else if(g_prCurrent_player->type == HFP)
        {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_SUSPEND, 0, 0, true);
        }
    }
    else
    {
        audio_src_srv_report("[Sink][AM]aud_ul_suspend Fail: Already suspended\n", 0);
    }
    #endif
}

extern audio_common_t audio_common;
/*****************************************************************************
 * FUNCTION
 *  aud_dl_resume
 * DESCRIPTION
 *  This function is used to resume DL 1
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_dl_resume(void)
{
    #if defined(MTK_AVM_DIRECT)
    if(g_am_task_mask & AM_TASK_MASK_DL_SUSPEND)
    {
        audio_src_srv_report("[Sink][AM]aud_dl_resume\n", 0);
        ami_set_audio_mask(AM_TASK_MASK_DL_SUSPEND, false);
/*Change dl1 resume control for g_prCurrent_player = NULL*/
        if (g_prCurrent_player != NULL)
        {
            #ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            bt_sink_srv_am_audio_stream_out_t  *stream_out = &(g_prCurrent_player->audio_stream_out);
            bt_sink_srv_am_audio_stream_in_t   *stream_in = &(g_prCurrent_player->audio_stream_in);
            #endif
            bt_sink_srv_audio_setting_vol_info_t vol_info;
            if((g_prCurrent_player->type == A2DP) || (g_prCurrent_player->type == AWS))
            {
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                }
                #else
                aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */

                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_RESUME, 0, 0, true);
            }
            else if((g_prCurrent_player->type == HFP) || (g_prCurrent_player->type == PCM))
            {
                if(g_prCurrent_player->type == HFP)
                {
                    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                    {
                    vol_info.type = VOL_HFP;
                    vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                    vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                    vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                    vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                    }
                    #else
                    aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
                    #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                }
                else if(g_prCurrent_player->type == PCM)
                {
                    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                    {
                    vol_info.type = VOL_PCM;
                    vol_info.vol_info.pcm_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.pcm_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                    }
                    #else
                    aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
                    #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                }

                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_RESUME, 0, 0, true);
            }
            else if(g_prCurrent_player->type == LINE_IN)
            {
            #ifdef MTK_LINE_IN_ENABLE
            #if defined(MTK_PURE_LINEIN_PLAYBACK_ENABLE)
                pure_linein_playback_open(g_prCurrent_player->local_context.line_in_format.line_in_codec.codec_cap.linein_sample_rate,
                                   g_prCurrent_player->local_context.line_in_format.line_in_codec.codec_cap.in_audio_device,
                                   g_prCurrent_player->local_context.line_in_format.line_in_codec.codec_cap.out_audio_device);
            #else
                #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    if (0x01 == audio_nvdm_HW_config.audio_scenario.Audio_Pure_Linein_enable) {
                        pure_linein_playback_open(g_prCurrent_player->local_context.line_in_format.line_in_codec.codec_cap.linein_sample_rate,
                                    g_prCurrent_player->local_context.line_in_format.line_in_codec.codec_cap.in_audio_device,
                                    g_prCurrent_player->local_context.line_in_format.line_in_codec.codec_cap.out_audio_device);
                    }else {
                        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                        {
                            bt_sink_srv_audio_setting_vol_info_t vol_info;

                            vol_info.type = VOL_LINE_IN;
                            vol_info.vol_info.lineIN_vol_info.dev_in  = g_prCurrent_player->audio_stream_in.audio_device;
                            vol_info.vol_info.lineIN_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                            vol_info.vol_info.lineIN_vol_info.lev_in  = g_prCurrent_player->audio_stream_in.audio_volume;
                            vol_info.vol_info.lineIN_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                            bt_sink_srv_am_set_volume(STREAM_IN,  &vol_info);
                        }
                        #else
                            aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
                            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
                        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_RESUME, 0, 0, true);
                    }
                #else
                    #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                        {
                            bt_sink_srv_audio_setting_vol_info_t vol_info;

                            vol_info.type = VOL_LINE_IN;
                            vol_info.vol_info.lineIN_vol_info.dev_in  = g_prCurrent_player->audio_stream_in.audio_device;
                            vol_info.vol_info.lineIN_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                            vol_info.vol_info.lineIN_vol_info.lev_in  = g_prCurrent_player->audio_stream_in.audio_volume;
                            vol_info.vol_info.lineIN_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                            bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                            bt_sink_srv_am_set_volume(STREAM_IN,  &vol_info);
                        }
                        #else
                            aud_set_volume_level(STREAM_OUT, AUD_VOL_SPEECH, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
                            aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
                        #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_RESUME, 0, 0, true);
                #endif
            #endif
            #endif
            }else if(g_prCurrent_player->type == USB_AUDIO_IN)
            {
            #ifdef MTK_USB_AUDIO_PLAYBACK_ENABLE
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_USB_AUDIO_IN;
                vol_info.vol_info.usb_audio_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.usb_audio_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;

                audio_src_srv_report("[Sink][AM][PLAY]USB_AUDIO: audio_volume = %d.", 1, g_prCurrent_player->audio_stream_out.audio_volume);
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
            }
            #else
                aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
            #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_PLAYBACK_RESUME, 0, 0, true);
            #endif
            } else if(g_prCurrent_player->type == BLE) {
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                }
                #else
                aud_set_volume_level(STREAM_OUT, AUD_VOL_AUDIO, stream_out->audio_device, (bt_sink_srv_am_volume_level_t)stream_out->audio_volume);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_DL_RESUME, 0, 0, true);
            }
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
            ami_set_afe_param(STREAM_OUT, audio_common.stream_out.stream_sampling_rate, true);
#endif
        }
        else
        {
            if (g_prHfp_media_handle != NULL){
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_RESUME, 0, 0, true);
            }
            else if (g_prA2dp_sink_handle != NULL){
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_RESUME, 0, 0, true);
            }
#ifdef MTK_LINE_IN_ENABLE
            else if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_LINEIN)){
            #if defined(MTK_PURE_LINEIN_PLAYBACK_ENABLE)
                pure_linein_playback_open(g_prLineIN_sink_handle.codec_cap.linein_sample_rate, g_prLineIN_sink_handle.codec_cap.in_audio_device, g_prLineIN_sink_handle.codec_cap.out_audio_device);
            #else
                #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                    if (0x01 == audio_nvdm_HW_config.audio_scenario.Audio_Pure_Linein_enable) {
                        pure_linein_playback_open(g_prLineIN_sink_handle.codec_cap.linein_sample_rate, g_prLineIN_sink_handle.codec_cap.in_audio_device, g_prLineIN_sink_handle.codec_cap.out_audio_device);
                    }else {
                        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_RESUME, 0, 0, true);
                    }
                #else
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_LINEIN_PLAYBACK_RESUME, 0, 0, true);
                #endif
            #endif
            }
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
            else if(g_prBle_media_handle != NULL) {
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_DL_RESUME, 0, 0, true);
            }
#endif
            else{
                audio_src_srv_report("[Sink][AM]aud_dl_resume Fail: HFP & A2DP & BLE handler was NULL.\n", 0);
            }
        }
    }
    else
    {
        audio_src_srv_report("[Sink][AM]aud_dl_resume Fail: Have not suspended\n", 0);
    }
    #endif
}

static void aud_ul_resume(void)
{
    #if defined(MTK_AVM_DIRECT)
    if(g_am_task_mask & AM_TASK_MASK_UL_SUSPEND)
    {
        audio_src_srv_report("[Sink][AM]aud_ul_resume\n", 0);
        ami_set_audio_mask(AM_TASK_MASK_UL_SUSPEND, false);
/*Change dl1 resume control for g_prCurrent_player = NULL*/
        if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD))
        {
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_RESUME, 0, 0, true);
        }
        else if (g_prCurrent_player != NULL)
        {
            #ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            bt_sink_srv_am_audio_stream_in_t   *stream_in = &(g_prCurrent_player->audio_stream_in);
            #endif
            bt_sink_srv_audio_setting_vol_info_t vol_info;
            if(g_prCurrent_player->type == HFP)
            {
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
                }
                #else
                aud_set_volume_level(STREAM_IN, AUD_VOL_AUDIO, stream_in->audio_device, (bt_sink_srv_am_volume_level_t)stream_in->audio_volume);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_RESUME, 0, 0, true);
            }
        }
        else
        {
            if (g_prHfp_media_handle != NULL){
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_RESUME, 0, 0, true);
            } else {
                audio_src_srv_report("[Sink][AM]aud_ul_resume Fail: HFP handler was NULL.\n", 0);
            }
        }
    }
    else
    {
        audio_src_srv_report("[Sink][AM]aud_ul_resume Fail: Have not suspended\n", 0);
    }
    #endif
}

/*****************************************************************************
 * FUNCTION
 *  aud_dl_control
 * DESCRIPTION
 *  This function is used to contorl DL 1 suspend / resume when side tone enable / disable
 * PARAMETERS
 *  isCodecOpen     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_dl_control(bool isCodecOpen)
{
    if((g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE)
        || (g_am_task_mask & AM_TASK_MASK_VP_HAPPENING)
        || (g_am_task_mask & AM_TASK_MASK_DL1_HAPPENING))
    {
        audio_src_srv_report("[Sink][AM]aud_dl_control: g_am_task_mask = 0x%08x", 1, g_am_task_mask);
        if(isCodecOpen)
        {
#ifndef FIXED_SAMPLING_RATE_TO_48KHZ
            if(((g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE) && !((g_prCurrent_player->type == HFP) || (g_prCurrent_player->type == PCM))))
            {
                aud_dl_suspend();
            } else
#endif
            if(g_am_task_mask & AM_TASK_MASK_VP_HAPPENING) {
                aud_dl_suspend();
            }
        }
        else
        {
#ifndef FIXED_SAMPLING_RATE_TO_48KHZ
            if(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE)
            {
                aud_dl_resume();
            } else
#endif
            if(g_am_task_mask & AM_TASK_MASK_DL1_HAPPENING) {
                aud_dl_resume();
            }
        }
    }
}

static void aud_ul_control(bool isCodecOpen)
{
    if((g_am_task_mask & AM_TASK_MASK_VP_HAPPENING)
        || (g_am_task_mask & AM_TASK_MASK_UL1_HAPPENING))
    {
        audio_src_srv_report("[Sink][AM]aud_ul_control: g_am_task_mask = 0x%08x", 1, g_am_task_mask);
        if(isCodecOpen)
        {
            if(g_prCurrent_player && (g_prCurrent_player->type == HFP)
            && (g_am_task_mask & AM_TASK_MASK_VP_HAPPENING))
            {
                aud_ul_suspend();
            }
        }
        else
        {
            if(g_am_task_mask & AM_TASK_MASK_UL1_HAPPENING)
            {
                aud_ul_resume();
            }
        }
    }
}

/*****************************************************************************
 * FUNCTION
 *  aud_side_tone_control
 * DESCRIPTION
 *  This function is used to contorl DL 1 suspend / resume when side tone enable / disable
 * PARAMETERS
 *  isEnable        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
#ifndef FIXED_SAMPLING_RATE_TO_48KHZ

static void aud_side_tone_control(bool isEnable)
{
    if(g_prCurrent_player && (g_prCurrent_player->type != NONE))
    {
        if((g_prCurrent_player->type == HFP) || (g_prCurrent_player->type == PCM))
        {
        }
        else
        {
            if(isEnable)
            {
                aud_dl_suspend();
            }
            else
            {
                if(!(g_am_task_mask & AM_TASK_MASK_VP_HAPPENING)){
                    aud_dl_resume();
                }
            }
        }
    }
}

#endif

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_open
 * DESCRIPTION
 *  This function is used to open codec for A2DP sink by BT APP
 * PARAMETERS
 *  a2dp_codec_t     [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_codec_am_a2dp_sink_open(bt_sink_srv_am_a2dp_codec_t *a2dp_codec_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_codec_a2dp_audio_t a2dp_codec = {{0}};

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    switch (a2dp_codec_t->role) {
        case BT_A2DP_SINK:
            a2dp_codec.role = a2dp_codec_t->role;
            if (a2dp_codec_t->codec_cap.type == BT_A2DP_CODEC_SBC) {
                BT_A2DP_CONVERT_SBC_CODEC(&(a2dp_codec.codec_cap), &(a2dp_codec_t->codec_cap));
                LOG_W(MPLOG,"[AM]Open A2DP SBC codec.", 0);
                #ifdef __BT_AWS_SUPPORT__
                g_aws_codec_type = BT_AWS_CODEC_TYPE_SBC;
                g_aws_sample_rate = a2dp_codec.codec_cap.codec.sbc.sample_rate;
                #endif /* __BT_AWS_SUPPORT__ */
            } else if (a2dp_codec_t->codec_cap.type == BT_A2DP_CODEC_AAC) {
                BT_A2DP_CONVERT_AAC_CODEC(&(a2dp_codec.codec_cap), &(a2dp_codec_t->codec_cap));
                LOG_W(MPLOG,"[AM]Open A2DP AAC codec.", 0);
                #ifdef __BT_AWS_SUPPORT__
                g_aws_codec_type = BT_AWS_CODEC_TYPE_AAC;
                g_aws_sample_rate = a2dp_codec.codec_cap.codec.aac.sample_rate;
                #endif /* __BT_AWS_SUPPORT__ */
            } else if (a2dp_codec_t->codec_cap.type == BT_A2DP_CODEC_VENDOR) {
                #if defined (MTK_BT_A2DP_VENDOR_ENABLE)
                BT_A2DP_CONVERT_VENDOR_CODEC(&(a2dp_codec.codec_cap), &(a2dp_codec_t->codec_cap));
                LOG_W(MPLOG,"[AM]Open A2DP VENDOR codec.", 0);
                #ifdef __BT_AWS_SUPPORT__
                g_aws_codec_type = BT_AWS_CODEC_TYPE_VENDOR;
                g_aws_sample_rate = a2dp_codec.codec_cap.codec.vendor.sample_rate;
                #endif /* __BT_AWS_SUPPORT__ */
                #else
                audio_src_srv_report("[sink][AM]a2dp sink open Vendor not open", 0);
                LOG_W(MPLOG,"[AM][error]Open A2DP VENDOR codec error(define not open).", 0);
                #endif
            }else if (a2dp_codec_t->codec_cap.type == 0x65) { //BT_A2DP_CODEC_AIRO_CELT=0x65
                //BT_A2DP_CONVERT_AIRO_CELT_CODEC(&(a2dp_codec.codec_cap), &(a2dp_codec_t->codec_cap));
                a2dp_codec.codec_cap.type = 0x65;//BT_A2DP_CODEC_AIRO_CELT=0x65;
                LOG_W(MPLOG,"[AM]Open A2DP AIRO_CELT codec.", 0);
                #ifdef __BT_AWS_SUPPORT__  //TODO
                g_aws_codec_type = BT_AWS_CODEC_TYPE_SBC;
                g_aws_sample_rate = a2dp_codec.codec_cap.codec.sbc.sample_rate;
                #endif /* __BT_AWS_SUPPORT__ */
            } else {
                audio_src_srv_report("[sink][AM][error]a2dp sink open codec error codec not support.", 0);
                LOG_W(MPLOG,"[AM][error]Open A2DP codec error(not support).", 0);
            }
#if defined(MTK_AVM_DIRECT)
            hal_audio_am_register_a2dp_open_callback(bt_codec_am_a2dp_sink_open_callback);
#endif

            g_prA2dp_sink_handle = bt_codec_a2dp_open(aud_bt_codec_a2dp_callback, &a2dp_codec);
            audio_src_srv_report("[sink][AM]a2dp sink open: g_prA2dp_sink_handle:0x%x",1, g_prA2dp_sink_handle);
            bt_sink_srv_assert(g_prA2dp_sink_handle);
#if defined(MTK_AVM_DIRECT)
            aud_dl_control(true);
#endif
            break;

        default:
            audio_src_srv_report("[sink][AM]a2dp sink open: undefined role:%d",1, a2dp_codec_t->role);
            break;
    }
    if (g_bBT_Ringbuf == NULL) {
#ifndef WIN32_UT
        //g_bBT_Ringbuf = (uint8_t *)pvPortMalloc(AM_RING_BUFFER_SIZE * sizeof(uint8_t));
        g_bBT_Ringbuf = NULL;
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
        g_bBT_Ringbuf = g_bt_sink_srv_am_ring_buffer;
#endif
#else
        g_bBT_Ringbuf = (uint8_t *)malloc(AM_RING_BUFFER_SIZE * sizeof(uint8_t));
#endif
    }
#ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
    g_prA2dp_sink_handle->set_get_data_function(g_prA2dp_sink_handle, bt_sink_srv_media_mgr_read_data);
    g_prA2dp_sink_handle->set_get_data_count_function(g_prA2dp_sink_handle, bt_sink_srv_media_mgr_get_data_count);
#else
    g_prA2dp_sink_handle->set_buffer(g_prA2dp_sink_handle, g_bBT_Ringbuf, AM_RING_BUFFER_SIZE);
#endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
    g_prA2dp_sink_event_handle.get_write_buffer = bt_get_write_buffer_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.write_data_done = bt_write_data_done_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.finish_write_data = bt_finish_write_data_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.get_free_space = bt_get_free_space_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.reset_share_buffer = bt_reset_share_buffer_a2dp_sink_codec;
    g_prA2dp_sink_event_handle.play = bt_codec_am_a2dp_sink_play;
    g_prA2dp_sink_event_handle.stop = bt_codec_am_a2dp_sink_stop;
    g_prA2dp_sink_event_handle.med_hd = g_prA2dp_sink_handle;
    #ifdef __BT_AWS_SUPPORT__
    g_prA2dp_sink_event_handle.set_aws_flag = bt_a2dp_aws_set_flag;
    g_prA2dp_sink_event_handle.set_aws_initial_sync = bt_a2dp_aws_set_initial_sync;
    g_prA2dp_sink_event_handle.aws_plh_init = bt_a2dp_aws_plh_init;
    g_prA2dp_sink_event_handle.aws_plh_deinit = bt_a2dp_aws_plh_deinit;
    #endif /* __BT_AWS_SUPPORT__ */
#if defined(MTK_AVM_DIRECT)
    // callback will be done when DSP ack open command.
#else
    g_rSink_state = A2DP_SINK_CODEC_OPEN;
    g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
    audio_src_srv_report("[sink][AM]a2dp sink open: type:%d, handle:0x%x",2, g_prCurrent_player->type, &g_prA2dp_sink_event_handle);
    if (g_prCurrent_player->type == A2DP) {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, &g_prA2dp_sink_event_handle);
    } else if (g_prCurrent_player->type == AWS) {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_SINK_OPEN_CODEC, AUD_SINK_PROC_PTR, &g_prA2dp_sink_event_handle);
    } else {
        /* trace error */
        audio_src_srv_report("[sink][AM]a2dp sink open error", 0);
    }
#endif

#ifdef __BT_SINK_SRV_ACF_MODE_SUPPORT__
    bt_sink_srv_set_acf_mode(0);
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_play
 * DESCRIPTION
 *  This function is used to process "play" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_a2dp_sink_play(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
    bt_sink_srv_am_device_set_t dev;
#if 0
    bt_sink_srv_am_volume_level_out_t vol;
    uint32_t digital_vol = 0x7FFF, analog_vol = 0x0002;
#endif
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
    bt_sink_srv_audio_tunning_context_t *aud_tunning_p = NULL;
#endif

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(g_prCurrent_player == NULL) {
        audio_src_srv_report("[sink][AM]sink_play--aud_id: %d, g_prCurrent_player is NULL, return error", 1, aud_id);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    audio_src_srv_report("[sink][AM]sink_play--aud_id: %d, aid: %d, g_prCurrent_player: 0x%x, g_prA2dp_sink_handle: 0x%x",4,
        aud_id, g_prCurrent_player->aud_id, g_prCurrent_player, g_prA2dp_sink_handle);

    if ((g_prCurrent_player != NULL) && (aud_id == g_prCurrent_player->aud_id)) {
        if (g_prA2dp_sink_handle != NULL) {
            dev = g_prCurrent_player->audio_stream_out.audio_device;
            //vol = g_prCurrent_player->audio_stream_out.audio_volume;
            hal_audio_set_stream_out_device(dev);
            //To save set volume time. BT sink must set volume before playing.
            /*if (dev & DEVICE_LOUDSPEAKER) {
                digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
            } else if (dev & DEVICE_EARPHONE) {
                digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
            } else {
                ;
            }
            if (g_am_volume_enable) {
                audio_src_srv_report("[sink][AM]play-d_gain: 0x%x, a_gain: 0x%x",2, digital_vol, analog_vol);
            } else {
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                    bt_sink_srv_audio_setting_vol_info_t vol_info;

                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                }
                #else
                hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                #endif //__BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            }*/
#ifdef __BT_SINK_SRV_AUDIO_TUNING__
            aud_tunning_p = bt_sink_srv_audio_tunning_get_context();
            if ((aud_tunning_p->flag & TUNNING_FLAG_INIT) &&
                    (aud_tunning_p->flag & TUNNING_FLAG_ON)) {
                bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_A2DP, TUNNING_TYPE_VOL);
                bt_sink_srv_audio_tunning_update(TUNNING_SCENARIO_A2DP, TUNNING_TYPE_DEV);
            }
#endif /* __BT_SINK_SRV_AUDIO_TUNING__ */
            eResult = g_prA2dp_sink_handle->play(g_prA2dp_sink_handle);
            if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
                g_rSink_state = A2DP_SINK_CODEC_PLAY;
                return BT_CODEC_MEDIA_STATUS_OK;
            }
        }
    }

    audio_src_srv_report("[sink][AM]sink_play--aud_id: %d, aid: %d, g_prCurrent_player: 0x%x, g_prA2dp_sink_handle: 0x%x",4,
        aud_id, g_prCurrent_player->aud_id, g_prCurrent_player, g_prA2dp_sink_handle);

    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_stop
 * DESCRIPTION
 *  This function is used to process "play" for A2DP sink by BT APP.
 * PARAMETERS
 *  aud_id           [IN]
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_a2dp_sink_stop(bt_sink_srv_am_id_t aud_id)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (aud_id == g_prCurrent_player->aud_id) {
        if (g_prA2dp_sink_handle != NULL) {
            eResult = g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
            if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
#endif
                g_rSink_state = A2DP_SINK_CODEC_STOP;
                return BT_CODEC_MEDIA_STATUS_OK;
            }
        }
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_a2dp_sink_close
 * DESCRIPTION
 *  This function is used to process "close" for A2DP sink by BT APP.
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
extern uint32_t g_Rdebug_a2dp_ltcs_last_time;
bt_status_t bt_codec_am_a2dp_sink_close(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        #if defined(MTK_AVM_DIRECT)
        aud_dl_control(false);
        #endif

        eResult = bt_codec_a2dp_close(g_prA2dp_sink_handle);
        /* for A2DP LTCS debug*/
        g_Rdebug_a2dp_ltcs_last_time = 0;

        if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
            g_prA2dp_sink_handle = NULL;
            g_rSink_state = A2DP_SINK_CODEC_CLOSE;
            if (g_bBT_Ringbuf != NULL) {
#ifndef WIN32_UT
                //vPortFree(g_bBT_Ringbuf);
#ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                bt_sink_srv_memset(g_bt_sink_srv_am_ring_buffer, 0, sizeof(g_bt_sink_srv_am_ring_buffer));
#endif
                g_bBT_Ringbuf = NULL;
#else
                free(g_bBT_Ringbuf);
                g_bBT_Ringbuf = NULL;
#endif
            }
            return BT_CODEC_MEDIA_STATUS_OK;
        }
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}


#ifdef __BT_AWS_SUPPORT__
/**
 * @brief     This function sets the advanced wireless stereo flag.
 * @param[in] aud_id is audio ID.
 * @param[in] flag is used to determine to turn on or turn off AWS mechanism. 1: turn on AWS, 0: turn off AWS.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
static int32_t bt_a2dp_aws_set_flag(bt_sink_srv_am_id_t aud_id, bool flag)
{
    int32_t ret = 0;

    audio_src_srv_report("[sink][AM]aws_set_flag--aud_id: %d, aid: %d, flag: %d",3,
        aud_id, g_prCurrent_player->aud_id, flag);

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = bt_codec_a2dp_aws_set_flag(g_prA2dp_sink_handle, flag);
    }

    return ret;
}


/**
 * @brief     This function sets the advanced wireless stereo initial synchronization.
 * @param[in] aud_id is audio ID.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
static int32_t bt_a2dp_aws_set_initial_sync(bt_sink_srv_am_id_t aud_id)
{
    int32_t ret = AM_ERR_FAIL_1ST;

    if (aud_id == g_prCurrent_player->aud_id) {
        //hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_HIGH);
        ret = bt_codec_a2dp_aws_set_initial_sync(g_prA2dp_sink_handle);
        //hal_gpio_set_output(HAL_GPIO_12, HAL_GPIO_DATA_LOW);
    }

    return ret;
}


static void bt_a2dp_aws_plh_init(bt_sink_srv_am_id_t aud_id)
{
    if (aud_id == g_prCurrent_player->aud_id) {
        bt_a2dp_plh_init(g_prA2dp_sink_handle);
    }
}


static void bt_a2dp_aws_plh_deinit(void)
{
    bt_a2dp_plh_deinit();
}


#ifdef __AUDIO_MP3_ENABLE__
// for AWS check Rance.
/**
 * @brief     This function sets the advanced wireless stereo flag.
 * @param[in] aud_id is audio ID.
 * @param[in] flag is used to determine to turn on or turn off AWS mechanism. 1: turn on AWS, 0: turn off AWS.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
static int32_t bt_mp3_aws_set_flag(bt_sink_srv_am_id_t aud_id, bool flag)
{
    int32_t ret = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = mp3_codec_aws_set_flag(g_am_mp3_media_handle, flag);
    }

    return ret;
}


/**
 * @brief     This function sets the advanced wireless stereo initial synchronization.
 * @param[in] aud_id is audio ID.
 * @return    If the operation completed successfully, the return value is #BT_CODEC_MEDIA_STATUS_OK, otherwise the return value is #BT_CODEC_MEDIA_STATUS_ERROR.
 */
static int32_t bt_mp3_aws_set_initial_sync(bt_sink_srv_am_id_t aud_id)
{
    int32_t ret = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = mp3_codec_aws_set_initial_sync(g_am_mp3_media_handle);
    }

    return ret;
}

static int32_t bt_mp3_aws_init()
{
    int32_t ret = 0;

    ret = g_am_mp3_media_handle->aws_init(g_am_mp3_media_handle);
    return ret;
}


static int32_t bt_mp3_aws_deinit()
{
    int32_t ret = 0;

    ret = g_am_mp3_media_handle->aws_deinit(g_am_mp3_media_handle);
    return ret;
}

static int32_t bt_mp3_aws_set_clock_skew_compensation_value(int32_t sample_count)
{
    int32_t ret = 0;

    ret =g_am_mp3_media_handle->aws_set_clock_skew_compensation_value(g_am_mp3_media_handle, sample_count);
    return ret;
}

static int32_t bt_mp3_aws_get_clock_skew_status(int32_t *status)
{
    int32_t ret = 0;

    ret = g_am_mp3_media_handle->aws_get_clock_skew_status(g_am_mp3_media_handle, status);
    return ret;
}

static int32_t bt_mp3_aws_set_clock_skew(bool flag)
{
    int32_t ret = 0;

    ret = g_am_mp3_media_handle->aws_set_clock_skew(g_am_mp3_media_handle, flag);
    return ret;
}
#endif // __AUDIO_MP3_ENABLE__


void bt_sink_srv_skew_test(const char *value)
{
    int32_t sample_count = 0;

    sample_count = atoi(value);

    LOG_I("[Sink][AM]skew_test--value: %s, count: %d", value, sample_count);

    if (g_prA2dp_sink_handle) {
        bt_codec_a2dp_aws_set_clock_skew_compensation_value(g_prA2dp_sink_handle, sample_count);
    }
}


#endif /* __BT_AWS_SUPPORT__ */

#if defined(__BT_AWS_SUPPORT__) || defined(MTK_AWS_MCE_ENABLE)


/*****************************************************************************
 * FUNCTION
 *  aud_process_aws_a2dp_callback_hdlr
 * DESCRIPTION
 *  This function is used to handle A2DP process callback.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void aud_process_aws_a2dp_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_bt_event_t event_id = amm_ptr->background_info.local_context.aws_format.aws_event;
    #ifndef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
    #endif
    bt_sink_srv_am_device_set_t dev;
    bt_sink_srv_am_volume_level_out_t vol;
    uint32_t digital_vol = 0x7FFF, analog_vol = 0x0002;
#ifdef __BT_AWS_SUPPORT__
    uint32_t sampling_rate = 44100;
#endif
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (g_prA2dp_sink_handle != NULL) {
        g_prA2dp_sink_handle->process(g_prA2dp_sink_handle, event_id);

        switch (event_id) {
            case BT_CODEC_MEDIA_ERROR: {
                #ifdef __BT_SINK_SRV_AM_MED_LIST_SUPPORT__
                //g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                //g_rSink_state = A2DP_SINK_CODEC_STOP;
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    audio_src_srv_report("[sink][AM]aws-d_gain: 0x%x, a_gain: 0x%x",2, digital_vol, analog_vol);
                }
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                    bt_sink_srv_audio_setting_vol_info_t vol_info;

                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                }
                #else
                hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_A2DP_PROC_IND, AUD_A2DP_CODEC_RESTART, NULL);
                #else
                g_prA2dp_sink_handle->stop(g_prA2dp_sink_handle);
                g_rSink_state = A2DP_SINK_CODEC_STOP;
                g_prA2dp_sink_handle->reset_share_buffer(g_prA2dp_sink_handle);
                dev = g_prCurrent_player->audio_stream_out.audio_device;
                vol = g_prCurrent_player->audio_stream_out.audio_volume;
                hal_audio_set_stream_out_device(dev);
                if (dev & DEVICE_LOUDSPEAKER) {
                    digital_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[LOUDSPEAKER_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else if (dev & DEVICE_EARPHONE) {
                    digital_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][0];
                    analog_vol = g_volume_out_config[EARPHONE_STREAM_OUT][AUD_VOL_AUDIO][vol][1];
                } else {
                    ;
                }
                #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
                {
                    bt_sink_srv_audio_setting_vol_info_t vol_info;

                    vol_info.type = VOL_A2DP;
                    vol_info.vol_info.a2dp_vol_info.dev = g_prCurrent_player->audio_stream_out.audio_device;
                    vol_info.vol_info.a2dp_vol_info.lev = g_prCurrent_player->audio_stream_out.audio_volume;
                    bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                }
                #else
                hal_audio_set_stream_out_volume(digital_vol, analog_vol);
                #endif /* __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__ */

                //eResult = g_prA2dp_sink_handle->play(g_prA2dp_sink_handle);
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_A2DP_PROC_IND, AUD_A2DP_CODEC_RESTART, NULL);
                if (eResult == BT_CODEC_MEDIA_STATUS_ERROR) {
                    audio_src_srv_report("[sink][AM] Error", 0);
                }
                audio_src_srv_report("[sink][AM]-restart play", 0);
                #endif /* __BT_SINK_SRV_AM_MED_LIST_SUPPORT__ */
                break;
            }
#ifdef __BT_AWS_SUPPORT__
            case BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW: {
                #ifndef MTK_AVM_DIRECT
                bt_sink_srv_fetch_bt_offset();
                #endif
                break;
            }

            case BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW: {
                if (BT_AWS_CODEC_TYPE_SBC == g_aws_codec_type) {
                    switch (g_aws_sample_rate) {
                        case 8:
                            sampling_rate = 16000;
                            break;
                        case 4:
                            sampling_rate = 32000;
                            break;
                        case 2:
                            sampling_rate = 44100;
                            break;
                        case 1:
                            sampling_rate = 48000;
                            break;

                        default:
                            sampling_rate = 44100;
                            break;
                    }
                } else if (BT_AWS_CODEC_TYPE_AAC == g_aws_codec_type){
                    switch (g_aws_sample_rate) {
                        case 0x800:
                            sampling_rate = 8000;
                            break;
                        case 0x400:
                            sampling_rate = 11025;
                            break;
                        case 0x200:
                            sampling_rate = 12000;
                            break;
                        case 0x100:
                            sampling_rate = 16000;
                            break;
                        case 0x80:
                            sampling_rate = 22050;
                            break;
                        case 0x40:
                            sampling_rate = 24000;
                            break;
                        case 0x20:
                            sampling_rate = 32000;
                            break;
                        case 0x10:
                            sampling_rate = 44100;
                            break;
                        case 0x8:
                            sampling_rate = 48000;
                            break;
                        default:
                            sampling_rate = 44100;
                            break;
                    }
                } else if (BT_AWS_CODEC_TYPE_VENDOR == g_aws_codec_type){
                    switch (g_aws_sample_rate) {
                        case 0x20:
                            sampling_rate = 44100;
                            break;
                        case 0x10:
                            sampling_rate = 48000;
                            break;
                        case 0x08:
                            sampling_rate = 88200;
                            break;
                        case 0x04:
                            sampling_rate = 96000;
                            break;
                        case 0x02:
                            sampling_rate = 176400;
                            break;
                        case 0x01:
                            sampling_rate = 192000;
                            break;
                        default:
                            sampling_rate = 44100;
                            break;
                    }
                }
                /* myler */
                #ifdef __BT_AWS_SUPPORT__
                bt_sink_srv_audio_sync_calc_t audio_calc;
                audio_calc.media_handle= g_prA2dp_sink_handle;
                audio_calc.sampling_rate = sampling_rate;
                audio_calc.type = g_aws_codec_type;
                bt_sink_srv_audio_clock_calibrate(&audio_calc);
                #endif
                break;
            }

            case BT_CODEC_MEDIA_UNDERFLOW: {
                #ifdef __BT_AWS_SUPPORT__
                g_aws_underflow_loop_count++;
                if (g_aws_underflow_loop_count >= BT_SINK_SRV_AM_MAX_UNDERFLOW_COUNT) {
                    g_aws_underflow_loop_count = 0;
                    g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_A2DP_PROC_IND, AUD_A2DP_AWS_UNDERFLOW, NULL);
                }
                #endif
                break;
            }
#endif
#if defined(MTK_AVM_DIRECT)
            case BT_CODEC_MEDIA_LTCS_DATA_REPORT: {
                audio_dsp_a2dp_ltcs_report_param_t param;
                param.p_ltcs_asi_buf = hal_audio_query_ltcs_asi_buf();
                param.p_ltcs_min_gap_buf = hal_audio_query_ltcs_min_gap_buf();
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_LTCS_REPORT, &param);
                break;
            }

            case BT_CODEC_MEDIA_LTCS_DATA_TIMEOUT: {
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_RECONNECT_REQUEST, NULL);
                return;
            }

            case BT_CODEC_MEDIA_AUDIO_DL_REINIT_REQUEST: {
                uint32_t param_reinit = hal_audio_dsp2mcu_data_get();
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_DL_REINIT_REQUEST, &param_reinit);
                break;
            }
#endif
            default:
                break;
        }
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_AWS_A2DP_PROC_IND, (bt_sink_srv_am_cb_sub_msg_t)event_id, NULL);
        _UNUSED(digital_vol);
        _UNUSED(analog_vol);
    }
}
#endif /* defined(__BT_AWS_SUPPORT__) || defined(MTK_AWS_MCE_ENABLE) */


#ifdef __BT_SINK_AUDIO_TUNING__
#define BT_SINK_AM_SIDE_TONE_DB_MAX 31
void bt_codec_am_speech_nvdm_changed_callback(void *data)
{
    audio_eaps_t *am_speech_eaps;

#if PRODUCT_VERSION == 2533
    aud_process_dsp_nvdm_setting();

    am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    audio_src_srv_report("[Sink][AM] nvdm changed, g_prCurrent_player:0x%x",1, g_prCurrent_player);

    if (am_speech_eaps != NULL) {
        audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        #ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__

        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_mode_parameter);
        #endif
        if (NULL != g_prCurrent_player && HFP == g_prCurrent_player->type) {
            int32_t stream_out_db = 0, stream_in_db = 0;
            int32_t analog_gain_in_db = 0, digital_gain_in_db = 0;
            int16_t temp_int16 = 0;
            uint32_t side_tone_db = 0;
            bool result = false;
            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                speech_set_enhancement(false);
                bt_sink_srv_audio_setting_update_voice_fillter_setting(&vol_info, am_speech_eaps);
                speech_set_enhancement(true);
            }
            #endif

            audio_src_srv_report("[Sink][AM] type:0x%x, out:%d, in:%d",3,
                               g_prCurrent_player->local_context.hfp_format.hfp_codec.type,
                               g_prCurrent_player->audio_stream_out.audio_volume,
                               g_prCurrent_player->audio_stream_in.audio_volume);

            if (BT_HFP_CODEC_TYPE_CVSD == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.stream_out_gain[g_prCurrent_player->audio_stream_out.audio_volume];
                stream_out_db = (int32_t)temp_int16;
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.stream_in_gain[g_prCurrent_player->audio_stream_in.audio_volume];
                stream_in_db = (int32_t)temp_int16;
                side_tone_db = (uint32_t)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_gain.sidetone_gain;
                result = true;
            } else if (BT_HFP_CODEC_TYPE_MSBC == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.stream_out_gain[g_prCurrent_player->audio_stream_out.audio_volume];
                stream_out_db = (int32_t)temp_int16;
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.stream_in_gain[g_prCurrent_player->audio_stream_in.audio_volume];
                stream_in_db = (int32_t)temp_int16;
                side_tone_db = (uint32_t)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_gain.sidetone_gain;
                result = true;
            }

            audio_src_srv_report("[Sink][AM] out_db:%d, in_db:%d, side_tone:%d",3, stream_out_db, stream_in_db, side_tone_db);

            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                audio_src_srv_report("[Sink][AM]2533 update gain", 0);
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
            }
            #endif

            if (result) {
                audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_out_db, &analog_gain_in_db, &digital_gain_in_db);
                digital_gain_in_db *= 100;
                analog_gain_in_db *= 100;
                audio_src_srv_report("[Sink][AM] stream out, digital:0x%x, analog:0x%x",2, digital_gain_in_db, analog_gain_in_db);
#ifndef __BT_SINK_SRV_AUDIO_TUNING__
                hal_audio_set_stream_out_volume((uint32_t)digital_gain_in_db, (uint32_t)analog_gain_in_db);
#endif
                voice_uplink_dmic_db_transfer_to_analog_gain_and_digital_gain(stream_in_db, &analog_gain_in_db, &digital_gain_in_db);
                digital_gain_in_db *= 100;
                analog_gain_in_db *= 100;
                audio_src_srv_report("[Sink][AM] stream in, digital:0x%x, analog:0x%x",2, digital_gain_in_db, analog_gain_in_db);
#ifndef __BT_SINK_SRV_AUDIO_TUNING__
                hal_audio_set_stream_in_volume((uint32_t)digital_gain_in_db, (uint32_t)analog_gain_in_db);
#endif

                if (BT_SINK_AM_SIDE_TONE_DB_MAX >= side_tone_db) {
                    audio_src_srv_report("[Sink][AM] side_tone_db:0x%x",1, side_tone_db);
                    speech_set_sidetone_volume(side_tone_db);
                }
            }
        }

        vPortFree(am_speech_eaps);
    }

#else

    am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    audio_src_srv_report("[Sink][AM] nvdm changed, g_prCurrent_player:0x%x",1, g_prCurrent_player);

    if (am_speech_eaps != NULL) {
        audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        #ifndef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        speech_set_enhancement(false);
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_mode_parameter);
        speech_set_enhancement(true);
        #endif

        if (NULL != g_prCurrent_player && HFP == g_prCurrent_player->type) {
            int32_t stream_out_db = 0, stream_in_db = 0;
            int32_t analog_gain_in_db = 0, digital_gain_in_db = 0;
            int16_t temp_int16 = 0;
            uint32_t side_tone_db = 0;
            bool result = false;

            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                speech_set_enhancement(false);
                bt_sink_srv_audio_setting_update_voice_fillter_setting(&vol_info, am_speech_eaps);
                speech_set_enhancement(true);
            }
            #endif

            audio_src_srv_report("[Sink][AM] type:0x%x, out:%d, in:%d",3,
                               g_prCurrent_player->local_context.hfp_format.hfp_codec.type,
                               g_prCurrent_player->audio_stream_out.audio_volume,
                               g_prCurrent_player->audio_stream_in.audio_volume);

            if (BT_HFP_CODEC_TYPE_CVSD == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_gain.stream_out_gain[g_prCurrent_player->audio_stream_out.audio_volume];
                stream_out_db = (int32_t)temp_int16;
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_gain.stream_in_gain[g_prCurrent_player->audio_stream_in.audio_volume];
                stream_in_db = (int32_t)temp_int16;
                side_tone_db = (uint32_t)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_gain.sidetone_gain;
                result = true;
            } else if (BT_HFP_CODEC_TYPE_MSBC == g_prCurrent_player->local_context.hfp_format.hfp_codec.type) {
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_gain.stream_out_gain[g_prCurrent_player->audio_stream_out.audio_volume];
                stream_out_db = (int32_t)temp_int16;
                temp_int16 = (int16_t)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_gain.stream_in_gain[g_prCurrent_player->audio_stream_in.audio_volume];
                stream_in_db = (int32_t)temp_int16;
                side_tone_db = (uint32_t)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_gain.sidetone_gain;
                result = true;
            }

            audio_src_srv_report("[Sink][AM] out_db:%d, in_db:%d, side_tone:%d",3, stream_out_db, stream_in_db, side_tone_db);

            #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
            {
                bt_sink_srv_audio_setting_vol_info_t vol_info;

                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
                vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
                audio_src_srv_report("[Sink][AM]2523 update gain", 0);
                bt_sink_srv_am_set_volume(STREAM_OUT, &vol_info);
                bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
            }
            #endif

            if (result) {
                audio_downlink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_out_db, &analog_gain_in_db, &digital_gain_in_db);
                digital_gain_in_db *= 100;
                analog_gain_in_db *= 100;
                audio_src_srv_report("[Sink][AM] stream out, digital:0x%x, analog:0x%x",2, digital_gain_in_db, analog_gain_in_db);
#ifndef __BT_SINK_SRV_AUDIO_TUNING__
                hal_audio_set_stream_out_volume((uint32_t)digital_gain_in_db, (uint32_t)analog_gain_in_db);
#endif
                voice_uplink_amp_db_transfer_to_analog_gain_and_digital_gain(stream_in_db, &analog_gain_in_db, &digital_gain_in_db);
                digital_gain_in_db *= 100;
                analog_gain_in_db *= 100;
                audio_src_srv_report("[Sink][AM] stream in, digital:0x%x, analog:0x%x",2, digital_gain_in_db, analog_gain_in_db);
#ifndef __BT_SINK_SRV_AUDIO_TUNING__
                hal_audio_set_stream_in_volume((uint32_t)digital_gain_in_db, (uint32_t)analog_gain_in_db);
#endif

                if (BT_SINK_AM_SIDE_TONE_DB_MAX >= side_tone_db) {
                    audio_src_srv_report("[Sink][AM] side_tone_db:0x%x",1, side_tone_db);
                    speech_set_sidetone_volume(side_tone_db);
                }
            }
        }

        vPortFree(am_speech_eaps);
    }
#endif
}
#endif /* __BT_SINK_AUDIO_TUNING__ */


/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_hfp_open
 * DESCRIPTION
 *  This function is used to open HFP codec.
 * PARAMETERS
 *  hfp_codec        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_codec_am_hfp_open(bt_sink_srv_am_hfp_codec_t *hfp_codec_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
    const audio_eaps_t *am_speech_eaps = (const audio_eaps_t *) audio_nvdm_get_global_eaps_address();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if(hfp_codec_t->type == BT_HFP_CODEC_TYPE_CVSD){
        LOG_W(MPLOG,"[AM]Open HFP CVSD codec.", 0);
    } else if (hfp_codec_t->type == BT_HFP_CODEC_TYPE_MSBC){
        LOG_W(MPLOG,"[AM]Open HFP MSBC codec.", 0);
    } else {
        LOG_W(MPLOG,"[AM][error]Open HFP codec error(not support).", 0);
        configASSERT(0);
    }
    g_prHfp_media_handle = bt_codec_hfp_open(aud_bt_codec_hfp_callback, hfp_codec_t);

#if defined(MTK_AVM_DIRECT)
    // For BT to analyze duration
    g_prHfp_sink_event_handle.med_hd = g_prHfp_media_handle;
    g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_HFP_OPEN_CODEC_DONE, &g_prHfp_sink_event_handle);
#endif

#if PRODUCT_VERSION != 2533
    //am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));
    if (am_speech_eaps != NULL) {
        //audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);

        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
            bt_sink_srv_audio_setting_update_voice_fillter_setting(&vol_info, am_speech_eaps);
        }
        #else
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_parameter[SPEECH_MODE_HEADSET].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir((int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_in_fir_coefficient,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_parameter[SPEECH_MODE_HEADSET].voice_wb_enhancement_parameter.wb_mode_parameter);
        #endif
        //vPortFree(am_speech_eaps);
    }

#else

    //am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));
    if (am_speech_eaps != NULL) {
        //audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);
        #ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        {
            bt_sink_srv_audio_setting_vol_info_t vol_info;

            vol_info.type = VOL_HFP;
            vol_info.vol_info.hfp_vol_info.codec = g_prCurrent_player->local_context.hfp_format.hfp_codec.type;
            vol_info.vol_info.hfp_vol_info.dev_in = g_prCurrent_player->audio_stream_in.audio_device;
            vol_info.vol_info.hfp_vol_info.dev_out = g_prCurrent_player->audio_stream_out.audio_device;
            vol_info.vol_info.hfp_vol_info.lev_in = g_prCurrent_player->audio_stream_in.audio_volume;
            vol_info.vol_info.hfp_vol_info.lev_out = g_prCurrent_player->audio_stream_out.audio_volume;
            bt_sink_srv_audio_setting_update_voice_fillter_setting(&vol_info, am_speech_eaps);
        }
        #else
        speech_update_common(am_speech_eaps->speech_common_parameter.speech_common_parameter);
        speech_update_nb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_stream_out_fir_coefficient);
        speech_update_nb_param(am_speech_eaps->voice_parameter.voice_nb_band.voice_nb_parameter_with_external_dsp[0].voice_nb_enhancement_parameter.nb_mode_parameter);
        speech_update_wb_fir(NULL,
                             (int16_t *)am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_stream_out_fir_coefficient);
        speech_update_wb_param(am_speech_eaps->voice_parameter.voice_wb_band.voice_wb_parameter_with_external_dsp[0].voice_wb_enhancement_parameter.wb_mode_parameter);
        #endif
        //vPortFree(am_speech_eaps);
    }

#endif

#ifdef __BT_SINK_AUDIO_TUNING__
    audio_nvdm_register_eaps_is_changed_callback((audio_nvdm_callback_id *)g_prHfp_media_handle, bt_codec_am_speech_nvdm_changed_callback, NULL);
    audio_src_srv_report("[sink][AM]eaps_reg_hf--cid: 0x%08x", 1, g_prHfp_media_handle);
#endif /* __BT_SINK_AUDIO_TUNING__ */

    eResult = g_prHfp_media_handle->play(g_prHfp_media_handle);
    if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
        g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_HFP_PLAY_OK, NULL);
    } else {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SELF_CMD_REQ, AUD_CMD_FAILURE, NULL);
    }
#if defined(MTK_AVM_DIRECT)
        aud_dl_control(true);
        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
            if((((audio_nvdm_HW_config.Voice_InputDev & 0xC0) >> 6) == 0x02) && (((audio_nvdm_HW_config.Voice_OutputDev & 0xF0) >> 4) == 0x03)){
        #else
            if (((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x02) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03)) ||
            ((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x04) && (audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select <= 0x60) && (audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select >= 0x10) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03))) {
        #endif
            aud_ul_control(true);
        }
#endif

#if PRODUCT_VERSION == 2533
    aud_process_dsp_nvdm_setting(); // must put after play
#endif
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_hfp_stop
 * DESCRIPTION
 *  This function is used to stop HFP codec.
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_hfp_stop(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __BT_SINK_AUDIO_TUNING__
    audio_nvdm_unregister_eaps_is_changed_callback((audio_nvdm_callback_id *)g_prHfp_media_handle);
#endif /* __BT_SINK_AUDIO_TUNING__ */
    eResult = g_prHfp_media_handle->stop(g_prHfp_media_handle);
    if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
        #if defined(MTK_AVM_DIRECT)
            aud_dl_control(false);
            #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                if((((audio_nvdm_HW_config.Voice_InputDev & 0xC0) >> 6) == 0x02) && (((audio_nvdm_HW_config.Voice_OutputDev & 0xF0) >> 4) == 0x03)){
            #else
                if (((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x02) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03)) ||
            ((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x04) && (audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select <= 0x60) && (audio_nvdm_HW_config.voice_scenario.Voice_Multiple_Mic_Main_Input_Select >= 0x10) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03))) {
            #endif
                aud_ul_control(false);
            }
        #endif
        if (bt_codec_hfp_close(g_prHfp_media_handle) == BT_CODEC_MEDIA_STATUS_OK) {
            g_prHfp_media_handle = NULL;
            /*Close sidetone if there is sidetone exit.*/
            if(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE){
                audio_side_tone_disable_hdlr(NULL);
            }
            ami_set_audio_mask(AM_TASK_MASK_UL1_HAPPENING, false);
            return BT_CODEC_MEDIA_STATUS_OK;
        }
    }
    ami_set_audio_mask(AM_TASK_MASK_UL1_HAPPENING, false);
    return BT_CODEC_MEDIA_STATUS_ERROR;
}

#ifdef AIR_BT_CODEC_BLE_ENABLED
void BT_BLE_CONVERT_LC3_CODEC(bt_codec_le_audio_t *dst_codec,
                               bt_sink_srv_am_ble_codec_t *src_codec)
{
    dst_codec->codec = BT_CODEC_TYPE_LE_AUDIO_LC3;
    switch(src_codec->channel_mode){
        case CHANNEL_MODE_DL_ONLY:
        case CHANNEL_MODE_DL_UL_BOTH:
            dst_codec->channel_mode = src_codec->channel_mode;
            audio_src_srv_report("[sink][am][med]CONVERT_LC3 channel_mode(%d)", 1, src_codec->channel_mode);
    }
    switch(src_codec->sample_rate){
        case SAMPLING_RATE_16K:
            dst_codec->sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
        break;
        case SAMPLING_RATE_24K:
            dst_codec->sample_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
        break;
        case SAMPLING_RATE_32K:
            dst_codec->sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
        break;
        case SAMPLING_RATE_44_1K:
            dst_codec->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
        break;
        case SAMPLING_RATE_48K:
            dst_codec->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
        break;
        default:
            audio_src_srv_report("[sink][am][error]CONVERT_LC3 sampling_frequency not support(%d)", 1, src_codec->sample_rate);
        break;
    }
    switch(src_codec->ul_sample_rate){
        case SAMPLING_RATE_16K:
            dst_codec->ul_sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
        break;
        case SAMPLING_RATE_24K:
            dst_codec->ul_sample_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
        break;
        case SAMPLING_RATE_32K:
            dst_codec->ul_sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
        break;
        case SAMPLING_RATE_44_1K:
            dst_codec->ul_sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
        break;
        case SAMPLING_RATE_48K:
            dst_codec->ul_sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
        break;
        default:
            audio_src_srv_report("[sink][am][error]CONVERT_LC3 ul_sampling_frequency not support(%d)", 1, src_codec->ul_sample_rate);
        break;
    }
    switch(src_codec->channel_num){
        case CHANNEL_MONO:
        case CHANNEL_STEREO:
        case CHANNEL_DUAL_MONO:
            dst_codec->channel_num = src_codec->channel_num;
        break;
        default:
            audio_src_srv_report("[sink][am][error]CONVERT_LC3 channel_num not support(%d)", 1, src_codec->channel_num);
        break;
    }
    switch(src_codec->ul_channel_num){
        case CHANNEL_MONO:
        case CHANNEL_STEREO:
        case CHANNEL_DUAL_MONO:
            dst_codec->ul_channel_num = src_codec->ul_channel_num;
        break;
        default:
            audio_src_srv_report("[sink][am][error]CONVERT_LC3 ul_channel_num not support(%d)", 1, src_codec->ul_channel_num);
        break;
    }
    dst_codec->frame_duration = src_codec->frame_duration;
    dst_codec->context_type = src_codec->context_type;
    dst_codec->ul_context_type = src_codec->ul_context_type;
    dst_codec->frame_payload_length = src_codec->frame_payload_length;
    dst_codec->ul_frame_payload_length = src_codec->ul_frame_payload_length;
    audio_src_srv_report("[sink][am][med]CONVERT_LC3--channel_mode: %d, frame_ms: %d,[DL] context_type: %d, channel_num: %d, sampling_frequency: %d, frame_payload_length: %d", 6,
                       dst_codec->channel_mode, dst_codec->frame_duration, dst_codec->context_type,
                       dst_codec->channel_num, dst_codec->sample_rate, dst_codec->frame_payload_length);
    audio_src_srv_report("[sink][am][med]CONVERT_LC3--[UL] context_type: %d, channel_num: %d, sampling_frequency: %d, frame_payload_length: %d", 4,
                       dst_codec->ul_context_type, dst_codec->ul_channel_num,
                       dst_codec->ul_sample_rate, dst_codec->ul_frame_payload_length);

}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_ble_open
 * DESCRIPTION
 *  This function is used to open BLE codec.
 * PARAMETERS
 *  hfp_codec        [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void bt_codec_am_ble_open(bt_sink_srv_am_ble_codec_t *ble_codec_t)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_ble_codec_t ble_codec = {0};
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;
    //const audio_eaps_t *am_speech_eaps = (const audio_eaps_t *) audio_nvdm_get_global_eaps_address();

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    if (ble_codec_t->codec == CODEC_LC3) {
        BT_BLE_CONVERT_LC3_CODEC(&(ble_codec), ble_codec_t);
    } else {
        audio_src_srv_report("[sink][AM]ble open codec fail--not support codec: %d", 1, ble_codec_t->codec);
    }
    g_prBle_media_handle = bt_codec_le_audio_open(aud_bt_codec_ble_callback, &ble_codec);

    aud_dl_control(true);
#if defined(MTK_AVM_DIRECT)
    // For BT to analyze duration
    g_prBle_sink_event_handle.med_hd = g_prBle_media_handle;
#endif
    eResult = g_prBle_media_handle->play(g_prBle_media_handle);
    if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
        g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_HFP_OPEN_CODEC_DONE, &g_prBle_sink_event_handle);
    } else {
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_SINK_OPEN_CODEC, AUD_CMD_FAILURE, &g_prBle_sink_event_handle);
    }
}

/*****************************************************************************
 * FUNCTION
 *  bt_codec_am_ble_stop
 * DESCRIPTION
 *  This function is used to stop BLE codec.
 * PARAMETERS
 *  void
 * RETURNS
 *  bt_status_t
 *****************************************************************************/
bt_status_t bt_codec_am_ble_stop(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_status_t eResult = BT_CODEC_MEDIA_STATUS_ERROR;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    eResult = g_prBle_media_handle->stop(g_prBle_media_handle);
    if (eResult == BT_CODEC_MEDIA_STATUS_OK) {
        #if defined(MTK_AVM_DIRECT)
            //aud_dl_control(false);
            //aud_ul_control(false);
        #endif
        if (bt_codec_le_audio_close(g_prBle_media_handle) == BT_CODEC_MEDIA_STATUS_OK) {
            g_prBle_media_handle = NULL;
            /*Close sidetone if there is sidetone exit.*/
#if 0
            if(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE){
                audio_side_tone_disable_hdlr(NULL);
            }
#endif
            return BT_CODEC_MEDIA_STATUS_OK;
        }
    }
    return BT_CODEC_MEDIA_STATUS_ERROR;
}
#endif

#ifdef RTOS_TIMER
/*****************************************************************************
 * FUNCTION
 *  aud_timer_callback
 * DESCRIPTION
 *  This callback function is used to notify A.M. when the timer expires on timer service task.
 * PARAMETERS
 *  pxTimer          [OUT]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_timer_callback(TimerHandle_t pxTimer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t lTimer_ID;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __AM_DEBUG_INFO__
    //audio_src_srv_report("[Sink][AM] Timer tick", 0);
#endif
    configASSERT(pxTimer);
    lTimer_ID = (uint32_t)pvTimerGetTimerID(pxTimer);
    if (lTimer_ID == AM_TIMER_ID) {
        g_lExpire_count++;
        if (g_lExpire_count == AM_EXPIRE_TIMER_MAX) {
            xTimerStop(pxTimer, 0);
            g_lExpire_count = 0;
            bt_sink_srv_ami_send_amm(MOD_AM, MOD_TMR, AUD_TIMER_IND,
                                     MSG_ID_TIMER_OUT_CALL_EXT_REQ,
                                     g_prCurrent_resumer,
                                     FALSE, NULL);
        }
    }
}
#endif

/*****************************************************************************
 * FUNCTION
 *  aud_timer_out_callback_hdlr
 * DESCRIPTION
 *  This function is used to handle timer callback on A.M. task.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void aud_timer_out_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    g_rAm_aud_id[g_prCurrent_resumer->aud_id].use = ID_IDLE_STATE;
    g_prCurrent_resumer->notify_cb(g_prCurrent_resumer->aud_id, AUD_RESUME_IND, AUD_RESUME_IDLE_STATE, NULL);
    g_prCurrent_resumer = NULL;
    am_audio_search_suspended();
}

#if defined(MTK_AVM_DIRECT)
/*****************************************************************************
 * FUNCTION
 *  audio_event_task_notification
 * DESCRIPTION
 *  HAL audio use the function to notify AM task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
static void audio_event_task_notification(void)
{
    bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_HAL_EVENT_IND,
                             MSG_ID_HAL_EVENT_EXT_REQ,
                             g_prCurrent_player,
                             TRUE, ptr_isr_callback_amm);
}


/*****************************************************************************
 * FUNCTION
 *  audio_task_delay_ms_func
 * DESCRIPTION
 *  Provide task delay to HAL.
 *  HAL need to wait for CCNI or DSP ack.
 *  Task delay is better than GPT delay.
 * PARAMETERS
 *  ms_duration      [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void audio_task_delay_ms_func(uint32_t ms_duration)
{
    vTaskDelay(ms_duration / portTICK_RATE_MS);
}

#endif

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
void audio_set_vendor_se_hdlr(bt_sink_srv_am_amm_struct *amm_pt)
{
    ami_execute_vendor_se(EVENT_SET_VENDOREFFECT);
    return;
}
#endif

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
static void audio_set_audio_transmitter_config(bt_sink_srv_am_amm_struct *amm_pt){
    bt_sink_srv_am_background_t *background_ptr = &(amm_pt->background_info);
    if(background_ptr->type == AUDIO_TRANSMITTER){
        uint16_t scenario_and_id = ((background_ptr->local_context.audio_transmitter_format.scenario_type)<<8) + background_ptr->local_context.audio_transmitter_format.scenario_sub_id;
        audio_src_srv_report("[Sink][AM][transmitter]playback_set_runtime_config, scenario_and_id = %x,",1,scenario_and_id);
        audio_transmitter_playback_set_runtime_config(scenario_and_id, (background_ptr->local_context.audio_transmitter_format.scenario_runtime_config_type), &(background_ptr->local_context.audio_transmitter_format.scenario_runtime_config));
        background_ptr->notify_cb(background_ptr->aud_id, AUD_HAL_EVENT_IND, AUD_CMD_COMPLETE, &scenario_and_id);
    }
}
#endif

#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
static void audio_set_voice_mic_type_hdlr(bt_sink_srv_am_amm_struct *amm_pt){
    bt_sink_srv_am_background_t *background_ptr = &(amm_pt->background_info);
    voice_mic_type_t voice_mic_type = background_ptr->local_context.audio_detachable_mic_format.voice_mic_type;
    uint32_t mic_used_flag;

    mcu2dsp_open_param_t open_param;

    audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr\n", 0);

    mic_used_flag = 0;

    //Restart sidetone if sidetone is enabled
    if((g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE)
    && (!(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_WAITING_STOP)))
    {
        audio_side_tone_disable_hdlr(NULL);
        audio_side_tone_enable_hdlr(NULL);
    }

    //AEC_NR start/stop
    if(voice_mic_type == VOICE_MIC_TYPE_FIXED) {
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AEC_NR_EN, 0, 1, false);
    } else {
        //hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AEC_NR_EN, 0, 0, false);
    }


    memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
    hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &open_param.stream_in_param);
    void *p_param_share = hal_audio_dsp_controller_put_paramter(&open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_VOICE_DL);

    //Check MIC user with transmitter
    #ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    #include "audio_transmitter_internal.h"

    _UNUSED(g_audio_transmitter_control);

    uint16_t scenario_and_id;

    audio_transmitter_scenario_list_t audio_transmitter_scenario_list[]  =  {
        {AUDIO_TRANSMITTER_GAMING_MODE, AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET},
#ifdef AIR_WIRED_AUDIO_ENABLE
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT},
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT},
        {AUDIO_TRANSMITTER_WIRED_AUDIO, AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER},
#endif
    };

    if(true == audio_transmitter_get_is_running_by_scenario_list(audio_transmitter_scenario_list, sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t))) {
        for(uint32_t i = 0; i < sizeof(audio_transmitter_scenario_list)/sizeof(audio_transmitter_scenario_list_t); i++ ) {
            if(true == audio_transmitter_get_is_running_by_scenario_list(&audio_transmitter_scenario_list[i], 1)) {
                mic_used_flag = 1;
                scenario_and_id = (audio_transmitter_scenario_list[i].scenario_type << 8) | audio_transmitter_scenario_list[i].scenario_sub_id;
                audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: \
                scenario_and_id = %x is running, change mic to %d\n", 2, scenario_and_id, voice_mic_type);

                //SUSPEND TRANSMITTER
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_AUDIO_TRANSMITTER_SUSPEND, scenario_and_id, 0, true);

                //DSP NVKEY PARA
                sysram_status_t status;
                DSP_FEATURE_TYPE_LIST AudioFeatureList_GameVoiceHead[2] = {
                     FUNC_GAMING_HEADSET,
                     FUNC_END,
                };
                if(voice_mic_type == VOICE_MIC_TYPE_FIXED) {
                    AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_HEADSET;
                } else if(voice_mic_type == VOICE_MIC_TYPE_DETACHABLE) {
                    AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_BOOM_MIC;
                }
                /* reset share buffer before put parameters*/
                audio_nvdm_reset_sysram();
                status = audio_nvdm_set_feature(2, AudioFeatureList_GameVoiceHead);
                if (status != NVDM_STATUS_NAT_OK) {
                    audio_src_srv_report("[DETACHABLE_MIC][Transmitter] failed to set parameters to share memory - err(%d)\r\n", 1, status);
                }

                // update MIC gain settings
                bt_sink_srv_audio_setting_vol_info_t vol_info;
                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_MSBC;
                vol_info.vol_info.hfp_vol_info.dev_in = HAL_AUDIO_DEVICE_MAIN_MIC;
                vol_info.vol_info.hfp_vol_info.dev_out = HAL_AUDIO_DEVICE_HEADSET;
                vol_info.vol_info.hfp_vol_info.lev_in = 0;
                vol_info.vol_info.hfp_vol_info.lev_out = 0;
                bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);

                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_STREAM_DEINIT, 0, 0, false);

                //RESUME TRANSMITTER
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_AUDIO_TRANSMITTER_RESUME, scenario_and_id, (uint32_t)p_param_share, true);
                break;
            }
        }
    } else {
        audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: [transmitter]no player use MIC currently, do nothing\n", 0);
    }
    #endif//MTK_AUDIO_TRANSMITTER_ENABLE

    //Check MIC user record
    if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD)) {
        audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: current player is record\n", 0);
        if(mic_used_flag) {
            audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: unexpected!! MIC should not has multiple user!!\n", 0);
            //configASSERT(0);
        } else {
            mic_used_flag = 2;
            bt_sink_srv_audio_setting_vol_info_t vol_info = {0};

            // update MIC gain settings
            vol_info.type = VOL_VC;
            bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);

            //SUSPEND RECORD
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_SUSPEND, 0, 0, true);

            //RESUME RECORD
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_RESUME, 0, (uint32_t)p_param_share, true);
        }
    }

    //Check MIC user HFP
    if(g_prCurrent_player == NULL) {
        audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: no current player\n", 0);
        return;
    } else {
        audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: g_prCurrent_player = 0x%08x, type = %d\n", 2, g_prCurrent_player, g_prCurrent_player->type);
        if (HFP == g_prCurrent_player->type) {
            audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: current player is HFP\n", 0);
            if(mic_used_flag) {
                audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: unexpected!! MIC should not has multiple user!!\n", 0);
                //configASSERT(0);
            } else {
                mic_used_flag = 3;
                //SUSPEND HFP
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_SUSPEND, 0, 0, true);

                //DSP NVKEY PARA
                sysram_status_t status;
                DSP_FEATURE_TYPE_LIST AudioFeatureList_WBeSCO[2] = {
                     FUNC_RX_NR,
                     FUNC_END,
                };
                if((voice_mic_type == VOICE_MIC_TYPE_FIXED) && (g_prCurrent_player->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD)) {
                    AudioFeatureList_WBeSCO[0] = FUNC_TX_NR;
                } else if((voice_mic_type == VOICE_MIC_TYPE_DETACHABLE) && (g_prCurrent_player->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD)) {
                    AudioFeatureList_WBeSCO[0] = FUNC_NB_BOOM_MIC;
                } else if((voice_mic_type == VOICE_MIC_TYPE_DETACHABLE) && (g_prCurrent_player->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_MSBC)) {
                    AudioFeatureList_WBeSCO[0] = FUNC_WB_BOOM_MIC;
                }
                /* reset share buffer before put parameters*/
                audio_nvdm_reset_sysram();
                status = audio_nvdm_set_feature(2, AudioFeatureList_WBeSCO);
                if (status != NVDM_STATUS_NAT_OK) {
                    audio_src_srv_report("[DETACHABLE_MIC][HFP] failed to set parameters to share memory - err(%d)\r\n", 1, status);
                }

                // update MIC gain settings
                bt_sink_srv_audio_setting_vol_info_t vol_info;
                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_MSBC;
                vol_info.vol_info.hfp_vol_info.dev_in = HAL_AUDIO_DEVICE_MAIN_MIC;
                vol_info.vol_info.hfp_vol_info.dev_out = HAL_AUDIO_DEVICE_HEADSET;
                vol_info.vol_info.hfp_vol_info.lev_in = 0;
                vol_info.vol_info.hfp_vol_info.lev_out = 0;
                bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);

                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_STREAM_DEINIT, 0, 0, false);
                //RESUME HFP with new stream in setting of the detachable mic
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_UL_RESUME, 0, (uint32_t)p_param_share, true);
            }
        } else {
            audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: current player not use MIC\n", 2, g_prCurrent_player, g_prCurrent_player->type);
        }
    }
    audio_src_srv_report("[DETACHABLE_MIC][Sink][AM]audio_set_voice_mic_type_hdlr: current MIC user is %d\n", 1, mic_used_flag);
}
#endif

/*****************************************************************************
 * FUNCTION
 *  am_task_main
 * DESCRIPTION
 *  This function is a main message handler on A.M. task.
 * PARAMETERS
 *  void
 * RETURNS
 *  void
 *****************************************************************************/
#ifdef WIN32_UT
void am_task_main(void)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32_t bAud_timer_id = AM_TIMER_ID;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    hal_audio_register_stream_out_callback(aud_stream_out_callback, NULL);
    #ifndef MTK_AM_NOT_SUPPORT_STREAM_IN
    hal_audio_register_stream_in_callback(aud_stream_in_callback, NULL);
    #endif
    aud_initial();
    while (1) {
        am_receive_msg(g_prAmm_current);
        free(g_prAmm_current);
        if (g_prCurrent_player != NULL) {
            audio_src_srv_report("\tHave player: ID=%d\n", 1, g_prCurrent_player->aud_id);
        } else if (g_prCurrent_resumer != NULL) {
            audio_src_srv_report("\tHave resumer: ID=%d\n", 1, g_prCurrent_resumer->aud_id);
        }
        break;
    }
}
#else
QueueHandle_t g_xQueue_am;
/*g_audio_nvdm_init_flg is use for check if audio nvkey have init done*/
bool g_audio_nvdm_init_flg = false;
extern void bt_sink_srv_audio_setting_init_vol_level(void);

#ifdef AIR_ESCO_VOICE_SPE_MULTI_MODE_ENABLE
extern SemaphoreHandle_t g_xSemaphore_hfp_nvkey_change_notify;
extern void audio_voice_change_mode_notify_callback(audio_nvdm_user_t user, audio_nvdm_status_t status, void *user_data);
#endif

void am_task_main(void *arg)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    bt_sink_srv_am_amm_struct *amm_temp_t = NULL;
    uint32_t bAud_timer_id = AM_TIMER_ID;
    DSP_PARA_DATADUMP_STRU DumpInfo;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
#ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM] Start AM_Task", 0);
#endif
    g_xQueue_am = xQueueCreate(AM_QUEUE_NUMBER, sizeof(bt_sink_srv_am_amm_struct *));

    if (g_xQueue_am != 0) {
        g_xSemaphore_ami = xSemaphoreCreateMutex();
        g_xSemaphore_Audio = xSemaphoreCreateMutex();
        g_xTimer_am = xTimerCreate("Timer",
                                   (AM_TIMER_PERIOD / portTICK_PERIOD_MS),
                                   pdTRUE,
                                   (void *)bAud_timer_id,
                                   aud_timer_callback);
        hal_audio_register_stream_out_callback(aud_stream_out_callback, NULL);
        #ifndef MTK_AM_NOT_SUPPORT_STREAM_IN
        hal_audio_register_stream_in_callback(aud_stream_in_callback, NULL);
        #endif
        aud_initial();
    #ifdef MTK_RECORD_ENABLE
        record_init();
    #endif

#if defined(MTK_AVM_DIRECT)
        hal_audio_set_task_notification_callback(audio_event_task_notification);
        hal_audio_set_task_ms_delay_function(audio_task_delay_ms_func);
        if (ptr_callback_amm == NULL) {
            ptr_callback_amm = (bt_sink_srv_am_amm_struct *)pvPortMalloc(sizeof(bt_sink_srv_am_amm_struct));
            if(ptr_callback_amm == NULL){
                audio_src_srv_report("[AM][ERROR]Init audio ptr_cb fail.",0);
                audio_src_srv_report("[AM][ERROR]Init audio ptr_cb fail.", 0);
                configASSERT(0);
            }
        }
        if (ptr_isr_callback_amm == NULL) {
            ptr_isr_callback_amm = (bt_sink_srv_am_amm_struct *)pvPortMalloc(sizeof(bt_sink_srv_am_amm_struct));
            if(ptr_isr_callback_amm == NULL){
                audio_src_srv_report("[AM][ERROR]Init audio ptr_isr_cb fail.",0);
                audio_src_srv_report("[AM][ERROR]Init audio ptr_isr_cb fail.", 0);
                configASSERT(0);
            }
        }

        if(AUD_EXECUTION_FAIL == audio_nvdm_configure_init()){
            audio_src_srv_report("[AM][ERROR]Init audio configure from NVDM.",0);
        }
        am_dynamic_change_channel(AUDIO_CHANNEL_SELECTION_STEREO); //Default
#endif

#if defined(MTK_AMP_DC_COMPENSATION_ENABLE)
        bt_sink_srv_am_feature_t feature_param;
        feature_param.type_mask = DC_COMPENSATION;
        am_audio_set_feature(FEATURE_NO_NEED_ID,&feature_param);
#endif

        //Register audio callback
        hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_AFE, audio_message_audio_handler, NULL);

#ifdef AIR_ESCO_VOICE_SPE_MULTI_MODE_ENABLE
        //Register hfp nvkey real time change process callback
        g_xSemaphore_hfp_nvkey_change_notify = xSemaphoreCreateBinary();
        audio_nvdm_register_user_callback(AUDIO_NVDM_USER_HFP, audio_voice_change_mode_notify_callback, NULL);
#endif

        am_load_audio_output_volume_parameters_from_nvdm();

        get_audio_dump_info_from_nvdm(&DumpInfo);
#ifdef MTK_AUDIO_DUMP_BY_SPDIF_ENABLE
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_SPDIF_DUMP, true);
        hal_audio_apll_enable(true, 96000); // SPDIF need APLL
#endif
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AUDIO_DUMP_MASK, 0, DumpInfo.Dump_Mask_0, false);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AUDIO_DUMP_MASK, 1, DumpInfo.Dump_Mask_1, false);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AUDIO_DUMP_MASK, 2, DumpInfo.Dump_Scenario_Sel, false);

#ifdef AIR_SILENCE_DETECTION_ENABLE
        audio_silence_detection_nvdm_ccni();
#endif

#ifdef __BT_SINK_AUDIO_TUNING__
        bt_sink_hf_audio_tuning_atci_init();
        bt_sink_srv_audio_tunning_init();
#endif /* __BT_SINK_AUDIO_TUNING__ */
#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
        bt_sink_srv_audio_setting_init();
        bt_sink_srv_audio_setting_init_vol_level();
#endif /* bt_sink_srv_audio_setting_init */

#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
        audio_anc_control_init();
        #ifdef MTK_USER_TRIGGER_FF_ENABLE
        audio_user_trigger_adaptive_ff_initialize();
        #endif
#if defined (MTK_AUDIO_TRANSMITTER_ENABLE) && defined (MTK_ANC_SURROUND_MONITOR_ENABLE)
        audio_anc_monitor_anc_init();
        audio_anc_control_register_callback((audio_anc_control_callback_t) audio_anc_monitor_anc_callback, AUDIO_ANC_CONTROL_EVENT_ON | AUDIO_ANC_CONTROL_EVENT_OFF | AUDIO_ANC_CONTROL_EVENT_SUSPEND_NOTIFY | AUDIO_ANC_CONTROL_EVENT_RESUME_NOTIFY, AUDIO_ANC_CONTROL_CB_LEVEL_ALL);
#endif
#else
        anc_init();
#endif
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_ANC_PASSTHROUGH, RACE_DSPREALTIME_ANC_PASSTHRU_RELAY_COSYS_CALLBACK);
    #ifdef RACE_DSP_REALTIME_CMD_ENABLE
        race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_DSP_COMMON, race_dsprealtime_cosys_relay_callback);
    #endif
    #ifdef RACE_HOSTAUDIO_CMD_ENABLE
        race_cosys_register_data_callback(RACE_COSYS_MODULE_ID_HOST_AUDIO_SET, race_cmd_hostaudio_cosys_relay_set_cmd_callback);
    #endif
    #if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
        g_race_cosys_dsp_realtime_callback = RACE_DSPREALTIME_callback;
    #endif
#endif
#ifdef MTK_AWS_MCE_ENABLE
        bt_callback_manager_register_callback(bt_callback_type_app_event, (uint32_t)MODULE_MASK_AWS_MCE, (void *)bt_event_ami_callback);
#endif

        while (1) {
            if (xQueueReceive(g_xQueue_am, &amm_temp_t, portMAX_DELAY)) {
                am_receive_msg((bt_sink_srv_am_amm_struct *)amm_temp_t);
                if ( (amm_temp_t != ptr_callback_amm) && (amm_temp_t != ptr_isr_callback_amm)) {
                    vPortFree(amm_temp_t);
                }
            }
        }
    }
#ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM][ERROR] Start AM_Task Error", 0);
#endif
}
#endif /* WIN32_UT */

/*****************************************************************************
 * FUNCTION
 *  am_receive_msg
 * DESCRIPTION
 *  This function is a main message dispatching function of A.M.
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
void am_receive_msg(bt_sink_srv_am_amm_struct *amm_ptr)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    /*
    #ifdef __AM_DEBUG_INFO__
     audio_src_srv_report("[Sink][AM] Received Message", 0);
     audio_src_srv_report("[Sink][AM] Aud ID: %d, Type: %d, Priority: %d, Vol_D: %d, Vol_A: %d, SR: %d",6,
                     amm_ptr->background_info.aud_id,
                     amm_ptr->background_info.type,
                     amm_ptr->background_info.priority,
                     amm_ptr->background_info.audio_stream_out.audio_volume.digital_gain_index,
                     amm_ptr->background_info.audio_stream_out.audio_volume.analog_gain_index,
                     amm_ptr->background_info.local_context.pcm_format.stream.stream_sample_rate
                   );
    #endif
    */
    switch (amm_ptr->msg_id) {
        case MSG_ID_STREAM_OPEN_REQ:
            aud_set_open_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_PLAY_REQ:
            aud_set_play_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_PLAY_REQ_TEST:
            test_aud_set_play_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_STOP_REQ:
            aud_set_stop_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_STOP_REQ_TEST:
            test_aud_set_stop_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_CLOSE_REQ:
            aud_set_close_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_SET_VOLUME_REQ:
            aud_set_volume_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_MUTE_DEVICE_REQ:
            aud_mute_device_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_CONFIG_DEVICE_REQ:
            aud_config_device_stream_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_READ_WRITE_DATA_REQ:
            aud_rw_stream_data_req_hdlr(amm_ptr);
            break;
        case MSG_ID_STREAM_GET_LENGTH_REQ:
            aud_query_stream_len_req_hdlr(amm_ptr);
            break;

        case MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ:
            aud_process_a2dp_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ:
            aud_process_hfp_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ:
            aud_event_stream_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ:
            aud_event_stream_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_TIMER_OUT_CALL_EXT_REQ:
            aud_timer_out_callback_hdlr(amm_ptr);
            break;
        case MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ:
            #if defined(__BT_AWS_SUPPORT__) || defined(MTK_AWS_MCE_ENABLE)
            aud_process_aws_a2dp_callback_hdlr(amm_ptr);
            #endif
            break;
        case MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ:
#ifdef MTK_AUDIO_MP3_ENABLED
            aud_process_mp3_callback_hdlr(amm_ptr);
#endif
            break;

#if defined(MTK_AVM_DIRECT)
        case MSG_ID_HAL_EVENT_EXT_REQ:
            hal_audio_dsp_message_process();
            break;
#endif

        case MSG_ID_AUDIO_SET_PAUSE:
            audio_set_pause_hdlr(amm_ptr);
            break;
        case MSG_ID_AUDIO_SET_RESUME:
            audio_set_resume_hdlr(amm_ptr);
            break;

        case MSG_ID_AUDIO_SIDE_TONE_ENABLE:
            audio_side_tone_enable_hdlr(amm_ptr);
            break;
        case MSG_ID_AUDIO_SIDE_TONE_DISABLE:
            audio_side_tone_disable_hdlr(amm_ptr);
            break;
        case MSG_ID_AUDIO_SET_SIDE_TONE_VOLUME:
            audio_side_tone_set_volume_hdlr(amm_ptr);
            break;
        case MSG_ID_AUDIO_DL_SUSPEND:
            audio_dl_suspend_hdlr(amm_ptr);
            break;
        case MSG_ID_AUDIO_DL_RESUME:
            audio_dl_resume_hdlr(amm_ptr);
            break;
        case MSG_ID_AUDIO_UL_SUSPEND:
            audio_ul_suspend_hdlr(amm_ptr);
            break;
        case MSG_ID_AUDIO_UL_RESUME:
            audio_ul_resume_hdlr(amm_ptr);
            break;

        case MSG_ID_AUDIO_SET_FEATURE:
            audio_set_feature_hdlr(amm_ptr);
            break;

#if defined(MTK_VENDOR_SOUND_EFFECT_ENABLE)
        case MSG_ID_AUDIO_SET_VENDOR_SE:
            audio_set_vendor_se_hdlr(amm_ptr);
            break;
#endif
#if defined(MTK_AUDIO_TRANSMITTER_ENABLE)
        case MSG_ID_TRANS_SET_CONFIG_REQ:
            audio_set_audio_transmitter_config(amm_ptr);
            break;
#endif
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    case MSG_ID_VOICE_SET_MIC_TYPE_REQ:
        audio_set_voice_mic_type_hdlr(amm_ptr);
        break;
#endif
#ifdef AIR_BT_AUDIO_SYNC_ENABLE
        case MSG_ID_AUDIO_REQUEST_SYNC:
            audio_request_sync_hdlr(amm_ptr);
            break;
#endif
        default:
            break;
    }

#if 0
    audio_src_srv_report("[Sink][AM] Background Message", 0);
    if (g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr != NULL) {
        audio_src_srv_report("[Sink][AM] Aud ID: %d, State: %d, Type: %d, Priority: %d, Vol: %d, pcm_SR: %d, a2dp_SR: %d",7,
                           amm_ptr->background_info.aud_id,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].use,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->type,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->priority,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->audio_stream_out.audio_volume,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->local_context.pcm_format.stream.stream_sample_rate,
                           g_rAm_aud_id[amm_ptr->background_info.aud_id].contain_ptr->local_context.a2dp_format.a2dp_codec.codec_cap.codec.aac.sample_rate
                          );
    }
#endif

}

#ifdef AIR_AM_DIRECT_EXEC_ENABLE

/*
 * Execute AM func directry/immediately
 */
void am_direct_exec(bt_sink_srv_am_amm_struct *amm_temp_t)
{
    am_receive_msg((bt_sink_srv_am_amm_struct *)amm_temp_t);
    if ( (amm_temp_t != ptr_callback_amm) && (amm_temp_t != ptr_isr_callback_amm)) {
            vPortFree(amm_temp_t);
    }
}
#endif

#if STANDALONE_TEST
/* Hal API func */
#ifndef WIN32_UT
hal_audio_status_t hal_audio_init(void)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_deinit(void)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_out_sampling_rate(hal_audio_sampling_rate_t sample_rate)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_out_channel_number(hal_audio_channel_number_t channel_number)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_start_stream_out(hal_audio_active_type_t active_type)
{
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_stop_stream_out(void)
{}

hal_audio_status_t hal_audio_set_stream_out_volume(uint32_t digital_volume_index, uint32_t analog_volume_index)
{
#ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM] Digital: %04x, Analog: %04x",2, digital_volume_index, analog_volume_index);
#endif
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_mute_stream_out(bool mute)
{}

hal_audio_status_t hal_audio_set_stream_out_device(hal_audio_device_t device)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_write_stream_out(const void *buffer, uint32_t size)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_get_stream_out_sample_count(uint32_t *sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_register_stream_out_callback(hal_audio_stream_out_callback_t callback, void *user_data)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_in_sampling_rate(hal_audio_sampling_rate_t sample_rate)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_set_stream_in_channel_number(hal_audio_channel_number_t channel_number)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_start_stream_in(hal_audio_active_type_t active_type)
{
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_stop_stream_in(void)
{}

hal_audio_status_t hal_audio_set_stream_in_volume(uint32_t digital_volume_index, uint32_t analog_volume_index)
{
#ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM] Digital: %04x, Analog: %04x",2, digital_volume_index, analog_volume_index);
#endif
    return HAL_AUDIO_STATUS_OK;
}

void hal_audio_mute_stream_in(bool mute)
{}

hal_audio_status_t hal_audio_set_stream_in_device(hal_audio_device_t device)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_read_stream_in(void *buffer, uint32_t sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_get_stream_in_sample_count(uint32_t *sample_count)
{
    return HAL_AUDIO_STATUS_OK;
}

hal_audio_status_t hal_audio_register_stream_in_callback(hal_audio_stream_in_callback_t callback, void *user_data)
{
    return HAL_AUDIO_STATUS_OK;
}


/* BT API func */
bt_media_handle_t *temp_t = NULL;
bt_media_handle_t *hf_temp_t = NULL;

void bt_codec_a2dp_set_buffer(bt_media_handle_t *handle, uint8_t  *buffer, uint32_t  length)
{

}

void bt_codec_a2dp_get_write_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    *length = 500;
}

void bt_codec_a2dp_reset_buffer(bt_media_handle_t *handle)
{

}

bt_status_t bt_codec_a2dp_play(bt_media_handle_t *handle)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_status_t bt_codec_a2dp_stop(bt_media_handle_t *handle)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_media_handle_t *bt_codec_a2dp_open(bt_codec_a2dp_callback_t callback, const bt_sink_srv_am_a2dp_codec_t *param)
{
    temp_t = (bt_media_handle_t *)pvPortMalloc(sizeof(bt_media_handle_t));
    temp_t->set_buffer         = bt_codec_a2dp_set_buffer;
    temp_t->get_write_buffer   = bt_codec_a2dp_get_write_buffer;
    temp_t->reset_share_buffer = bt_codec_a2dp_reset_buffer;
    temp_t->play               = bt_codec_a2dp_play;
    temp_t->stop               = bt_codec_a2dp_stop;
    return (bt_media_handle_t *)temp_t;
}

bt_status_t bt_codec_a2dp_close(bt_media_handle_t *handle)
{
    vPortFree(temp_t);
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_status_t bt_codec_hfp_play(bt_media_handle_t *handle)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_status_t bt_codec_hfp_stop(bt_media_handle_t *handle)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_media_handle_t *bt_codec_hfp_open(bt_codec_hfp_callback_t callback, const bt_sink_srv_am_hfp_codec_t *param)
{
    hf_temp_t = (bt_media_handle_t *)pvPortMalloc(sizeof(bt_media_handle_t));
    hf_temp_t->play = bt_codec_hfp_play;
    hf_temp_t->stop = bt_codec_hfp_stop;
    return (bt_media_handle_t *)hf_temp_t;
}

bt_status_t bt_codec_hfp_close(bt_media_handle_t *handle)
{
    vPortFree(hf_temp_t);
    return BT_STATUS_SUCCESS;
}
#endif /*#ifndef WIN32_UT*/
#endif

#ifdef __AUDIO_MP3_ENABLE__
extern mp3_codec_media_handle_t *mp3_dsp_codec_open(mp3_codec_callback_t mp3_codec_callback);
#endif
static void am_files_codec_open(bt_sink_srv_am_files_format_t *files_format)
{
    am_file_type_t type = FILE_NONE;
    //int32_t len = 0;
    //bt_sink_srv_file_path_t *path = NULL;
    int32_t ret = 0, err = 0;

    #if 0
    #if FILE_PATH_TYPE_LEN == 1
    path = files_format->path;
    len = strlen(path);
    if (strncmp(&path[len - 4], ".mp3", 4) == 0) {
        type = FILE_MP3;
    } else if (strncmp(&path[len - 4], ".wav", 4) == 0) {
        type = FILE_WAV;
    } else {
        type = FILE_NONE;
    }
    #else
    path = (wchar_t *)files_format->path;
    len = wcslen(path);
    if (wcsncmp(&path[len - 4], L".mp3", 4) == 0) {
        type = FILE_MP3;
    } else if (wcsncmp(&path[len - 4], L".wav", 4) == 0) {
        type = FILE_WAV;
    } else {
        type = FILE_NONE;
    }
    #endif
    #endif
    type = files_format->file_type;
    switch (type) {
        case FILE_MP3: {
        #ifdef __AUDIO_MP3_ENABLE__
            #ifdef __BT_AWS_SUPPORT__
            g_aws_codec_type = BT_AWS_CODEC_TYPE_MP3;
            #endif
            // codec already opened
            if (g_am_mp3_media_handle) {
                g_prCurrent_player->local_context.files_format.file_event.type = FILE_MP3;
                ret = AM_ERR_SUCCESS_1ST;
                break;
            }
#ifdef __AUDIO_MP3_ENABLE__
#if defined(MTK_AVM_DIRECT)
            g_am_mp3_media_handle = (mp3_codec_media_handle_t *) mp3_dsp_codec_open(am_files_mp3_callback);
#else
            g_am_mp3_media_handle = (mp3_codec_media_handle_t *) mp3_codec_open(am_files_mp3_callback);
#endif
#endif
            if (g_am_mp3_media_handle) {
                /* fill mp3 media handle */
                memset(&g_am_files_media_handle, 0x00, sizeof(bt_sink_srv_am_files_media_handle_t));
                g_am_files_media_handle.type = FILE_MP3;
                g_am_files_media_handle.media_handle.mp3.get_write_buffer = am_mp3_get_write_buffer;
                g_am_files_media_handle.media_handle.mp3.get_read_buffer = am_mp3_get_read_buffer;
                g_am_files_media_handle.media_handle.mp3.get_data_count = am_mp3_get_data_count;
                g_am_files_media_handle.media_handle.mp3.get_free_space = am_mp3_get_free_space;
                g_am_files_media_handle.media_handle.mp3.write_data_done = am_mp3_write_data_done;
                g_am_files_media_handle.media_handle.mp3.finish_write_data = am_mp3_finish_write_data;
                g_am_files_media_handle.media_handle.mp3.play = am_mp3_play;
                g_am_files_media_handle.media_handle.mp3.pause = am_mp3_pause;
                g_am_files_media_handle.media_handle.mp3.resume = am_mp3_resume;
                g_am_files_media_handle.media_handle.mp3.stop = am_mp3_stop;
                g_am_files_media_handle.media_handle.mp3.close_codec = am_mp3_close_codec;
                g_am_files_media_handle.media_handle.mp3.skip_id3v2_and_reach_next_frame = am_mp3_skip_id3v2_and_reach_next_frame;
                g_am_files_media_handle.media_handle.mp3.set_silence_frame_information = am_mp3_set_silence_frame_information;
                g_am_files_media_handle.media_handle.mp3.fill_silence_frame = am_mp3_fill_silence_frame;
                g_am_files_media_handle.media_handle.mp3.get_data_status = am_mp3_get_data_status;
                g_am_files_media_handle.media_handle.mp3.flush = am_mp3_flush;

                // aws + mp3
        #ifdef __BT_AWS_SUPPORT__
                g_am_files_media_handle.media_handle.mp3.set_aws_flag = bt_mp3_aws_set_flag;
                g_am_files_media_handle.media_handle.mp3.set_aws_initial_sync =
                    bt_mp3_aws_set_initial_sync;
                g_am_files_media_handle.media_handle.mp3.aws_init = bt_mp3_aws_init;
                g_am_files_media_handle.media_handle.mp3.aws_deinit = bt_mp3_aws_deinit;
                g_am_files_media_handle.media_handle.mp3.aws_set_clock_skew_compensation_value =
                    bt_mp3_aws_set_clock_skew_compensation_value;
                g_am_files_media_handle.media_handle.mp3.aws_get_clock_skew_status =
                    bt_mp3_aws_get_clock_skew_status;
                g_am_files_media_handle.media_handle.mp3.aws_set_clock_skew =
                    bt_mp3_aws_set_clock_skew;
        #endif
                g_prCurrent_player->local_context.files_format.file_event.type = FILE_MP3;
#if defined(MTK_AUDIO_MIXER_SUPPORT)
                mp3_codec_set_memory2(g_am_mp3_media_handle);
#else
                mp3_codec_set_memory2();
#endif

                ret = AM_ERR_SUCCESS_1ST;
            } else {
                ret = AM_ERR_FAIL_1ST;
            }
        #endif /* __AUDIO_MP3_ENABLE__ */
            break;
        }

        case FILE_WAV: {
        #ifdef __AUDIO_WAV_ENABLE__
            // codec already opened
            #ifdef __AUDIO_MP3_ENABLE__
                if (g_am_audio_media_handle) {
                    g_prCurrent_player->local_context.files_format.file_event.type = FILE_WAV;
                    ret = AM_ERR_SUCCESS_1ST;
                    break;
                }
                g_am_audio_media_handle = audio_codec_wav_codec_open(am_files_audio_callback, NULL);
                if (g_am_audio_media_handle) {
                    memset(&g_am_files_media_handle, 0x00, sizeof(bt_sink_srv_am_files_media_handle_t));
                    g_am_files_media_handle.type = FILE_WAV;
                    g_am_files_media_handle.media_handle.mp3.get_write_buffer                = am_audio_get_write_buffer;
                    g_am_files_media_handle.media_handle.mp3.write_data_done                 = am_audio_write_data_done;
                    g_am_files_media_handle.media_handle.mp3.finish_write_data               = am_audio_finish_write_data;
                    g_am_files_media_handle.media_handle.mp3.get_data_count                  = am_audio_get_data_count;
                    g_am_files_media_handle.media_handle.mp3.get_free_space                  = am_audio_get_free_space;
                    g_am_files_media_handle.media_handle.mp3.play                            = am_audio_play;
                    g_am_files_media_handle.media_handle.mp3.pause                           = am_audio_pause;
                    g_am_files_media_handle.media_handle.mp3.resume                          = am_audio_resume;
                    g_am_files_media_handle.media_handle.mp3.stop                            = am_audio_stop;
                    g_am_files_media_handle.media_handle.mp3.close_codec                     = am_audio_close_codec;

                    ret = AM_ERR_SUCCESS_1ST;
                } else {
                    ret = AM_ERR_FAIL_1ST;
                }
            #endif /* __AUDIO_MP3_ENABLE__ */
        #endif /* __AUDIO_WAV_ENABLE__ */
            break;
        }

        default:
            break;
    }

    if (ret > AM_ERR_SUCCESS_OK) {
        g_am_file_state = FILE_CODEC_OPEN;
        g_rAm_aud_id[g_prCurrent_player->aud_id].use = ID_PLAY_STATE;
        g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id,
            AUD_FILE_OPEN_CODEC, AUD_FILE_PROC_PTR, &g_am_files_media_handle);

        #ifdef __BT_SINK_SRV_ACF_MODE_SUPPORT__
        bt_sink_srv_set_acf_mode(1);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM]files_codec_open--type: %d, ret: %d, err: %d",3, type, ret, err);
    #endif
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static void am_files_codec_close(void)
{
    int32_t ret = AM_ERR_FAIL_7TH, err = AM_ERR_FAIL_7TH;
    am_file_type_t type = FILE_NONE;

    type = g_am_files_media_handle.type;

    switch (type) {
        case FILE_MP3: {
            #ifdef __AUDIO_MP3_ENABLE__
            if (g_am_mp3_media_handle) {
                if ((g_am_file_state == FILE_CODEC_PLAY) || (g_am_file_state == FILE_CODEC_PAUSE)) {
                    err = g_am_mp3_media_handle->stop(g_am_mp3_media_handle);
                }
                ret = g_am_mp3_media_handle->close_codec(g_am_mp3_media_handle);
                g_am_mp3_media_handle = NULL;
                g_am_file_state = FILE_CODEC_CLOSE;
            } else {
                audio_src_srv_report("[Sink][AM]files_codec_close--empty hd", 0);
            }
            #endif
            break;
        }

        case FILE_WAV: {
            #ifdef __AUDIO_WAV_ENABLE__
            if (g_am_audio_media_handle) {
                if (g_am_file_state == FILE_CODEC_PLAY) {
                    err = g_am_audio_media_handle->stop(g_am_audio_media_handle);
                }
                ret = audio_codec_wav_codec_close(g_am_audio_media_handle);
                g_am_audio_media_handle = NULL;
                g_am_file_state = FILE_CODEC_CLOSE;
            } else {
                audio_src_srv_report("[Sink][AM]files_codec_close--empty hd", 0);
            }
            #endif /*__AUDIO_WAV_ENABLE__*/
            break;
        }

        default:
            break;
    }

    audio_src_srv_report("[Sink][AM]files_codec_close--type: %d, ret: %d, err: %d, f_state: %d",4, type, ret, err, g_am_file_state);
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = ret;
    //g_test_fix_warning = err;
    _UNUSED(ret);
    #endif
}


#ifdef __AUDIO_MP3_ENABLE__
static void am_files_mp3_callback(mp3_codec_media_handle_t *hdl, mp3_codec_event_t event)
{
    if (g_prCurrent_player->type == FILES || g_prCurrent_player->type == AWS) {
        if (FILE_MP3 == g_prCurrent_player->local_context.files_format.file_event.type) {
            bt_sink_srv_am_background_t background_info;
            memcpy(&background_info, g_prCurrent_player, sizeof(bt_sink_srv_am_background_t));
            background_info.local_context.files_format.file_event.event.mp3.event = (am_mp3_event_type_t)event;
            //g_prCurrent_player->local_context.files_format.file_event.event.mp3.event = (am_mp3_event_type_t)event;
#ifdef __BT_AWS_SUPPORT__
            if (event == MP3_CODEC_AWS_CHECK_CLOCK_SKEW) {
                g_aws_skew_loop_count++;
                if (g_aws_skew_loop_count >= BT_SINK_SRV_AWS_SKEW_LOOP_COUNT) {
                    // send_amm used the same pointer to store event, take care dont  overlap it
                    bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_FILE_PROC_IND,
                                         MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ,
                                         &background_info, //g_prCurrent_player,
                                         TRUE, ptr_callback_amm);
                }
            } else if (event == MP3_CODEC_AWS_CHECK_UNDERFLOW) {
            } else
#endif
            {
                bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_FILE_PROC_IND,
                                 MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ, &background_info, // g_prCurrent_player,
                                 FALSE, NULL);
            }
        }
    } else {
        audio_src_srv_report("[Sink[AM]]am_files_mp3_callback type %d event %d",2, g_prCurrent_player->type, g_prCurrent_player->local_context.files_format.file_event.type);
    }
}


static void am_mp3_get_write_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->get_write_buffer(g_am_mp3_media_handle, buffer, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]get_write_buffer--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}

static void am_mp3_get_read_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->get_read_buffer(g_am_mp3_media_handle, buffer, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]get_write_buffer--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}

static int32_t am_mp3_get_data_count(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    int32_t ret = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = g_am_mp3_media_handle->get_data_count(g_am_mp3_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]get_data_count--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    _UNUSED(err);
    return ret;
}

static int32_t am_mp3_get_free_space(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    int32_t ret = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        ret = g_am_mp3_media_handle->get_free_space(g_am_mp3_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]get_free_space--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    _UNUSED(err);
    return ret;
}


static void am_mp3_write_data_done(bt_sink_srv_am_id_t aud_id, uint32_t length)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->write_data_done(g_am_mp3_media_handle, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]write_data_done--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static void am_mp3_finish_write_data(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->finish_write_data(g_am_mp3_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]finish_write_data--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static int32_t am_mp3_play(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->play(g_am_mp3_media_handle);
        g_am_file_state = FILE_CODEC_PLAY;
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]mp3_play--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_pause(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->pause(g_am_mp3_media_handle);
        g_am_file_state = FILE_CODEC_PAUSE;
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]mp3_pause--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_resume(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->resume(g_am_mp3_media_handle);
        g_am_file_state = FILE_CODEC_PLAY;
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]mp3_resume--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_stop(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->stop(g_am_mp3_media_handle);
        g_am_file_state = FILE_CODEC_STOP;
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]mp3_stop--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_close_codec(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->close_codec(g_am_mp3_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]mp3_close_codec--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}


static int32_t am_mp3_skip_id3v2_and_reach_next_frame(bt_sink_srv_am_id_t aud_id,
    uint32_t file_size)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        err = g_am_mp3_media_handle->skip_id3v2_and_reach_next_frame(g_am_mp3_media_handle, file_size);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]skip_id3v2--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM]skip_id3v2(e)--err: %d, size: %d",2, err, file_size);
    #endif

    return err;
}


static int32_t am_mp3_set_silence_frame_information(bt_sink_srv_am_id_t aud_id, silence_frame_information_t *frm_info)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        audio_src_srv_report("[Sink][AM]set_silence_frame_information--frame %x size %d",2, frm_info->frame, frm_info->frame_size);
        err = g_am_mp3_media_handle->set_silence_frame_information(g_am_mp3_media_handle, frm_info);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]set_silence_frame_information--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM]set_silence_frame_information(e)--err: %d",1, err);
    #endif

    return err;
}


static int32_t am_mp3_fill_silence_frame(bt_sink_srv_am_id_t aud_id, uint8_t *buffer)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->fill_silence_frame(g_am_mp3_media_handle, buffer);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]fill_silence_frame--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM]fill_silence_frame--err(e)--err: %d",1, err);
    #endif
    return err;
}


static int32_t am_mp3_get_data_status(bt_sink_srv_am_id_t aud_id,
        mp3_codec_data_type_t type, int32_t *status)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->get_data_status(g_am_mp3_media_handle, type, status);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]fill_silence_frame--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM]fill_silence_frame--err(e)--err: %d",1, err);
    #endif
    return err;
}


static int32_t am_mp3_flush(bt_sink_srv_am_id_t aud_id, int32_t flush_data_flag)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_mp3_media_handle->flush(g_am_mp3_media_handle, flush_data_flag);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]am_mp3_flush--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    #ifdef __AM_DEBUG_INFO__
    audio_src_srv_report("[Sink][AM]am_mp3_flush--err(e)--err: %d",1, err);
    #endif
    return err;
}

#endif /* __AUDIO_MP3_ENABLE__*/

#ifdef MTK_AUDIO_MP3_ENABLED
static void aud_process_mp3_callback_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    am_file_event_t file_event;
#if defined(MTK_AUDIO_MP3_ENABLED)
    uint32_t offset = 0;
#endif
    bt_sink_srv_am_cb_sub_msg_t cb_sub_msg = AUD_FILE_EVENT_BASE;
    am_mp3_event_type_t event_id = amm_ptr->background_info.local_context.files_format.file_event.event.mp3.event;

    file_event.type = amm_ptr->background_info.local_context.files_format.file_type;
    file_event.event.mp3.event = event_id;

#if defined(MTK_AUDIO_MP3_ENABLED)
    if ( FILE_MP3 == file_event.type ) {
        switch (event_id) {
            case AM_MP3_CODEC_MEDIA_REQUEST: {
                cb_sub_msg = AUD_FILE_EVENT_DATA_REQ;
                break;
            }
            case AM_MP3_CODEC_MEDIA_UNDERFLOW: {
                cb_sub_msg = AUD_FILE_EVENT_UNDERFLOW;
                break;
            }

            case AM_MP3_CODEC_MEDIA_JUMP_FILE_TO: {
                offset = g_am_mp3_media_handle->jump_file_to_specified_position;
                file_event.event.mp3.param = (void *)offset;
                cb_sub_msg = AUD_FILE_EVENT_JUMP_INFO;
                break;
            }

            case AM_MP3_CODEC_AWS_CHECK_UNDERFLOW: {
                break;
            }

            case AM_MP3_CODEC_MEDIA_BITSTREAM_END: {
                cb_sub_msg = AUD_FILE_MP3_BITSTREAM_END;
                break;
            }
    #ifdef __BT_AWS_SUPPORT__
            case AM_MP3_CODEC_AWS_CHECK_CLOCK_SKEW: {
                //bt_sink_srv_audio_clock_calibrate(&audio_calc);
                cb_sub_msg = AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW;
                g_aws_skew_loop_count = 0;
                break;
            }
    #endif

            default:
                break;
        }
    } else
#endif
    {
        switch (event_id) {
            case AM_AUDIO_CODEC_MEDIA_REQUEST: {
                cb_sub_msg = AUD_FILE_EVENT_DATA_REQ;
            } break;
            case AM_AUDIO_CODEC_MEDIA_UNDERFLOW: {
                cb_sub_msg = AUD_FILE_EVENT_UNDERFLOW;
            } break;
            case AM_AUDIO_CODEC_MEDIA_EVENT_END: {
                cb_sub_msg = AUD_FILE_EVENT_DATA_END;
            } break;
            default:
                break;
        }
    }

    g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_FILE_PROC_IND, cb_sub_msg, &file_event);
}
#endif //MTK_AUDIO_MP3_ENABLED

static void audio_set_pause_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    //send DSP mute message
    audio_src_srv_report("[Sink][AM]audio_set_pause_hdlr\n", 0);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_MUTE_OUTPUT_DEVICE, 0, (HAL_AUDIO_STREAM_OUT1<<16)|1, false);
}

static void audio_set_resume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    uint32_t feature_number;
    bt_sink_srv_am_background_t *am_background_temp = &(amm_ptr->background_info);
    //send DSP deinit message
    audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr with current am type: %d\n", 1, am_background_temp->type);

    switch(am_background_temp->type)
    {
        case A2DP:
        case AWS:
        case USB_AUDIO_IN:
        {
            #ifdef MTK_PEQ_ENABLE
            bt_sink_srv_am_peq_param_t ami_peq_param;
            sysram_status_t status;
            DSP_FEATURE_TYPE_LIST AudioFeatureList_A2DP[3] =
            {
                FUNC_PEQ_A2DP,
                FUNC_END,
            };
            feature_number = 2;
            /* Help to load UL relative nvkey list if ULL is ongoing. */
            if ((g_prCurrent_player->type == A2DP) && (g_prCurrent_player->local_context.a2dp_format.a2dp_codec.codec_cap.type == 0x65)) {
                audio_src_srv_report("GamingVoiceHead UL side load nvkey", 0);
                AudioFeatureList_A2DP[1] = FUNC_GAMING_HEADSET;
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
                voice_mic_type_t mic_cur_type = hal_audio_query_voice_mic_type();
                audio_src_srv_report("GamingVoiceHead mic_cur_type: 0x%x", 1, mic_cur_type);
                if(mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) {
                    AudioFeatureList_A2DP[1] = FUNC_GAMING_BOOM_MIC;
                }
#endif
                AudioFeatureList_A2DP[2] = FUNC_END;
                feature_number = 3;
            }
            memset(&ami_peq_param, 0, sizeof(bt_sink_srv_am_peq_param_t));
            audio_nvdm_reset_sysram();
            status = audio_nvdm_set_feature(feature_number, AudioFeatureList_A2DP);
            if (status != NVDM_STATUS_NAT_OK){
                audio_src_srv_report("A2DP set resume is failed to set parameters to share memory - err(%d)\r\n", 1, status);
            }
            /* set pre PEQ*/
            ami_peq_param.phase_id = 0;
            ami_peq_param.enable = g_peq_handle.a2dp_pre_peq_enable;
            ami_peq_param.sound_mode = g_peq_handle.a2dp_pre_peq_sound_mode;
            ami_peq_param.setting_mode = PEQ_DIRECT;
            aud_set_peq_param(aud_get_peq_audio_path(g_prCurrent_player->type), &ami_peq_param);
            /* set post PEQ*/
            ami_peq_param.phase_id = 1;
            ami_peq_param.enable = g_peq_handle.a2dp_post_peq_enable;
            ami_peq_param.sound_mode = g_peq_handle.a2dp_post_peq_sound_mode;
            ami_peq_param.setting_mode = PEQ_DIRECT;
            aud_set_peq_param(aud_get_peq_audio_path(g_prCurrent_player->type), &ami_peq_param);
            #endif
            break;
        }
        case LINE_IN:
        {
            #if defined(MTK_LINEIN_PEQ_ENABLE) || defined(MTK_LINEIN_INS_ENABLE)
            bt_sink_srv_am_peq_param_t ami_peq_param;
            sysram_status_t status;
            DSP_FEATURE_TYPE_LIST AudioFeatureList_LINEIN[] =
            {
#ifdef MTK_LINEIN_INS_ENABLE
                FUNC_INS,
#endif
#ifdef MTK_LINEIN_PEQ_ENABLE
                FUNC_PEQ_LINEIN,
#endif
                FUNC_END,
            };
            memset(&ami_peq_param, 0, sizeof(bt_sink_srv_am_peq_param_t));
            audio_nvdm_reset_sysram();
            status = audio_nvdm_set_feature(sizeof(AudioFeatureList_LINEIN)/sizeof(DSP_FEATURE_TYPE_LIST), AudioFeatureList_LINEIN);
            if (status != NVDM_STATUS_NAT_OK){
                audio_src_srv_report("LINEIN set resume is failed to set parameters to share memory - err(%d)\r\n", 1, status);
            }
            /* set pre PEQ*/
            ami_peq_param.phase_id = 0;
            ami_peq_param.enable = g_peq_handle.linein_pre_peq_enable;
            ami_peq_param.sound_mode = g_peq_handle.linein_pre_peq_sound_mode;
            ami_peq_param.setting_mode = PEQ_DIRECT;
            aud_set_peq_param(aud_get_peq_audio_path(g_prCurrent_player->type), &ami_peq_param);
            #endif
            break;
        }
        case HFP:
        {
            sysram_status_t status;
            DSP_FEATURE_TYPE_LIST AudioFeatureList_WBeSCO[2] =
            {
                 FUNC_RX_NR,
                 FUNC_END,
            };
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
            voice_mic_type_t mic_cur_type = hal_audio_query_voice_mic_type();
            if((mic_cur_type == VOICE_MIC_TYPE_FIXED) && (g_prCurrent_player->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD)) {
                AudioFeatureList_WBeSCO[0] = FUNC_TX_NR;
                audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr eSCO NB mode (CVSD) fixed mic", 0);
            } else if((mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) && (g_prCurrent_player->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD)) {
                AudioFeatureList_WBeSCO[0] = FUNC_NB_BOOM_MIC;
                audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr eSCO NB mode (CVSD) detachable mic", 0);
            } else if((mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) && (g_prCurrent_player->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_MSBC)) {
                AudioFeatureList_WBeSCO[0] = FUNC_WB_BOOM_MIC;
                audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr eSCO WB mode (mSBC) detachable mic", 0);
            }
#else
            if (g_prCurrent_player->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD) {
                AudioFeatureList_WBeSCO[0] = FUNC_TX_NR;
                audio_src_srv_report("eSCO NB mode (CVSD)", 0);
                #if defined(MTK_ANC_ENABLE) && defined(AIR_ESCO_VOICE_SPE_MULTI_MODE_ENABLE)
                    /*config the nvkey base on ANC mode*/
                    hfp_dsp_nvkey_replace(BT_HFP_CODEC_TYPE_CVSD,AudioFeatureList_WBeSCO);
                #endif
            } else {
                audio_src_srv_report("eSCO WB mode (mSBC)", 0);
                #if defined(MTK_ANC_ENABLE) && defined(AIR_ESCO_VOICE_SPE_MULTI_MODE_ENABLE)
                    /*config the nvkey base on ANC mode*/
                    hfp_dsp_nvkey_replace(BT_HFP_CODEC_TYPE_MSBC,AudioFeatureList_WBeSCO);
                #endif
            }
#endif
            /* reset share buffer before put parameters*/
            audio_nvdm_reset_sysram();
            status = audio_nvdm_set_feature(2, AudioFeatureList_WBeSCO);
            if (status != NVDM_STATUS_NAT_OK){
               audio_src_srv_err("eSCO set resume is failed to set parameters to share memory - err(%d)\r\n", 1, status);
            }
            break;
        }
        case AUDIO_TRANSMITTER:
        {
            #if defined (MTK_AUDIO_TRANSMITTER_ENABLE)

            #ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
            audio_transmitter_scenario_list_t voice_headset_scenario_list[]  =
                {{AUDIO_TRANSMITTER_GAMING_MODE, AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET}};
            if(audio_transmitter_get_is_running_by_scenario_list(&voice_headset_scenario_list[0], 1) == true){
                voice_mic_type_t mic_cur_type = hal_audio_query_voice_mic_type();

                //DSP NVKEY PARA
                sysram_status_t status;
                DSP_FEATURE_TYPE_LIST AudioFeatureList_GameVoiceHead[2] = {
                    FUNC_GAMING_HEADSET,
                    FUNC_END,
                };
                if(mic_cur_type == VOICE_MIC_TYPE_FIXED) {
                    AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_HEADSET;
                    audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr audio transmitter fixed mic", 0);
                } else if(mic_cur_type == VOICE_MIC_TYPE_DETACHABLE) {
                    AudioFeatureList_GameVoiceHead[0] = FUNC_GAMING_BOOM_MIC;
                    audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr audio transmitter detachable mic", 0);
                }
                /* reset share buffer before put parameters*/
                audio_nvdm_reset_sysram();
                status = audio_nvdm_set_feature(2, AudioFeatureList_GameVoiceHead);
                if (status != NVDM_STATUS_NAT_OK) {
                    audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr audio transmitter set resume failed to set parameters to share memory - err(%d)\r\n", 1, status);
                }
            }
            #endif
            #ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
            audio_transmitter_scenario_list_t voice_dongle_usb_out_scenario_list[]  =
                {{AUDIO_TRANSMITTER_GAMING_MODE, AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT}};
            if(audio_transmitter_get_is_running_by_scenario_list(&voice_dongle_usb_out_scenario_list[0], 1) == true){
                bt_ull_streaming_t stream;
                stream.streaming_interface = BT_ULL_STREAMING_INTERFACE_MICROPHONE;
                stream.port = 0;
                /* reopen audio stream for loading new settings */
                /* stop stream */
                audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr stop ULL uplink stream", 0);
                bt_ull_action(BT_ULL_ACTION_STOP_STREAMING, &stream, sizeof(bt_ull_streaming_t));
                /* delay 100ms */
                vTaskDelay(100 / portTICK_RATE_MS);
                /* start stream */
                bt_ull_action(BT_ULL_ACTION_START_STREAMING, &stream, sizeof(bt_ull_streaming_t));
                audio_src_srv_report("[Sink][AM]audio_set_resume_hdlr start ULL uplink stream", 0);
            }
            #endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */
            #endif /* MTK_AUDIO_TRANSMITTER_ENABLE */
        }

        default:
            break;
    }

    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_STREAM_DEINIT, 0, 0, false);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_MUTE_OUTPUT_DEVICE, 0, ((HAL_AUDIO_STREAM_OUT_ALL<<16) | 0), false);
    if(g_prCurrent_player != NULL){
        am_background_temp->in_out = STREAM_IN;
        am_background_temp->audio_stream_in.audio_mute = false;
        am_background_temp->audio_stream_in.audio_volume = g_prCurrent_player->audio_stream_in.audio_volume;;
        aud_set_volume_stream_req_hdlr(amm_ptr);
        am_background_temp->in_out = STREAM_OUT;
        am_background_temp->audio_stream_out.audio_mute = false;
        am_background_temp->audio_stream_out.audio_volume = g_prCurrent_player->audio_stream_out.audio_volume;;
        aud_set_volume_stream_req_hdlr(amm_ptr);
    }
}

static void audio_side_tone_callback(hal_audio_event_t event, void *data)
{
    if(event == HAL_AUDIO_EVENT_END){
    /*SideTone Stop ack.*/
        ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_ENABLE, false);
        ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_WAITING_STOP, false);
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
        ami_set_afe_param(STREAM_OUT, HAL_AUDIO_SAMPLING_RATE_16KHZ, false);
#endif
#ifndef FIXED_SAMPLING_RATE_TO_48KHZ
        aud_side_tone_control(false);
#endif
#if defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
        hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_SIDETONE, 0, 0, false);
#endif
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_SIDETONE, false);

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_SIDETONE_STOP);
#endif

        if(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_REQUEST){
            audio_side_tone_enable_hdlr(NULL);
            ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_REQUEST, false);
        }

    }
}

void audio_side_tone_enable_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    #if defined(MTK_AVM_DIRECT)
    mcu2dsp_sidetone_param_t sidetone;
    void *p_param_share;
    static mcu2dsp_open_stream_in_param_t in_device;
    static mcu2dsp_open_stream_out_param_t out_device;

    memset(&in_device,0,sizeof(in_device));
    memset(&out_device,0,sizeof(out_device));
    memset(&sidetone,0,sizeof(sidetone));

    audio_src_srv_report("[Sink][AM]audio_side_tone_enable_hdlr\n", 0);
    if( (g_am_task_mask & AM_TASK_MASK_VP_HAPPENING)
        || (g_am_task_mask & AM_TASK_MASK_SIDE_TONE_WAITING_STOP)){
        audio_src_srv_report("[Sink][AM]side tone enable Fail: VP happen first.\n", 0);
        ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_REQUEST, true);
    } else {
        if(!(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE)) {
            #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
                if((audio_nvdm_HW_config.Voice_Sidetone_EN == false) && (g_prHfp_media_handle != NULL)){
            #else
                //if ((audio_nvdm_HW_config.voice_scenario.Voice_Side_Tone_Enable == 0) && (g_prHfp_media_handle != NULL)) {
                if (audio_nvdm_HW_config.voice_scenario.Voice_Side_Tone_Enable == 0) {
            #endif
                audio_src_srv_report("[Sink][AM]side tone enable Fail: HFP sidetone disable.\n", 0);
                return;
            }
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
        ami_execute_vendor_se(EVENT_SIDETONE_START);
#endif
            ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_ENABLE, true);
#ifndef FIXED_SAMPLING_RATE_TO_48KHZ
            aud_side_tone_control(true);
#endif
            ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_SIDETONE, true);
            hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_SIDETONE, audio_side_tone_callback, NULL);
#if 0
            sidetone.out_device                      = HAL_AUDIO_DEVICE_DAC_DUAL;
            sidetone.out_interface                   = HAL_AUDIO_INTERFACE_NONE;
            sidetone.out_misc_parms                  = DOWNLINK_PERFORMANCE_NORMAL;
            sidetone.in_device                       = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
            sidetone.in_interface                    = HAL_AUDIO_INTERFACE_1;
            sidetone.in_channel                      = HAL_AUDIO_DIRECT;
            sidetone.in_misc_parms                   = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
            sidetone.sample_rate                     = 16000;
#else
            hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &out_device); /*Expect stream out channel is default*/
            hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &in_device);   /*Sidetone.channel should be mic channel setting.*/
            sidetone.out_device                      = out_device.afe.audio_device;
            sidetone.out_interface                   = out_device.afe.audio_interface;
            sidetone.out_misc_parms                  = out_device.afe.misc_parms;
            sidetone.in_device                       = in_device.afe.audio_device;
            sidetone.in_interface                    = in_device.afe.audio_interface;
            sidetone.in_channel                      = in_device.afe.stream_channel;
            sidetone.in_misc_parms                   = in_device.afe.misc_parms;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE//workaround, use this option to make sure the setting is only for AB156X and later series
            sidetone.adc_mode                        = in_device.afe.adc_mode;
#endif
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
            sidetone.sample_rate                     = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
            sidetone.sample_rate                     = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#else
            sidetone.sample_rate                     = 16000;
#endif
#endif
            sysram_status_t nvdm_status;
            uint32_t nvkey_length = 0;//31*sizeof(uint16_t);

            nvdm_status = flash_memory_query_nvdm_data_length(NVKEY_DSP_PARA_SIDETONE_FIR_COEFFICIENT,&nvkey_length);
            if (nvdm_status || !nvkey_length) {
                 audio_src_srv_report("[audio_side_tone_enable_hdlr] Read Nvkey length Fail id:0x%x, status:%d ", 2, NVKEY_DSP_PARA_SIDETONE_FIR_COEFFICIENT, nvdm_status);
            }
            pFIRBuf = pvPortMallocNC(nvkey_length);
            if(pFIRBuf){
                nvdm_status = flash_memory_read_nvdm_data(NVKEY_DSP_PARA_SIDETONE_FIR_COEFFICIENT, (uint8_t *)pFIRBuf, &nvkey_length);
                if (nvdm_status || !nvkey_length) {
                    audio_src_srv_report("[audio_side_tone_enable_hdlr] Read Nvkey length Fail id:0x%x, status:%d ", 2, NVKEY_DSP_PARA_SIDETONE_FIR_COEFFICIENT, nvdm_status);
                }
            }else{
               audio_src_srv_report("[audio_side_tone_enable_hdlr] fail to malloc pFIRBuf",0);
            }

            sidetone.FIR_nvdm_param =(uint16_t *)hal_memview_cm4_to_dsp0((uint32_t)pFIRBuf);
            //audio_src_srv_report("[M DEBUG][audio_side_tone_enable_hdlr] tableSize %d FIR_nvdm_param 0x%x",2,nvkey_length,sidetone.FIR_nvdm_param);

#ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
            if(g_prHfp_media_handle != NULL){
                //HFP sidetone gain
                if(audio_nvdm_HW_config.Voice_Sidetone_Gain >> 7){
                    sidetone.gain   = (0xFFFFFF00 | audio_nvdm_HW_config.Voice_Sidetone_Gain) * 100;      /*Default: -100 /100 = -1dB*/
                } else {
                    sidetone.gain   = (audio_nvdm_HW_config.Voice_Sidetone_Gain) * 100;
                }
            } else {
                //Others sidetone gain
                if(audio_nvdm_HW_config.Reserve_Sidetone_Gain >> 7){
                    sidetone.gain   = (0xFFFFFF00 | audio_nvdm_HW_config.Reserve_Sidetone_Gain) * 100;      /*Default: 800 /100 = 8dB change to -100 /100 = -1dB. Because record mic would sync analog gain to eSCO.*/
                } else {
                    sidetone.gain   = (audio_nvdm_HW_config.Reserve_Sidetone_Gain) * 100;
                }
            }
#else
            if(g_prHfp_media_handle != NULL){
                //HFP sidetone gain
                if (audio_nvdm_HW_config.sidetone_config.SideTone_Gain >> 7) {
                    sidetone.gain   = (0xFFFFFF00 | audio_nvdm_HW_config.sidetone_config.SideTone_Gain) * 100;      /*Default: -100 /100 = -1dB*/
                } else {
                    sidetone.gain   = (audio_nvdm_HW_config.sidetone_config.SideTone_Gain) * 100;
                }

                if(g_side_tone_gain_hfp == SIDETONE_GAIN_MAGIC_NUM) {
                    g_side_tone_gain_hfp = sidetone.gain/100;
                } else {
                    sidetone.gain = g_side_tone_gain_hfp * 100;
                    audio_src_srv_report("[Sink][AM]User defined side tone gain value for HFP: %d\n", 1, g_side_tone_gain_hfp);
                }
            }else {
                //Others sidetone gain,reserve config same as HFP sidetone gain val
                audio_nvdm_HW_config.sidetone_config.SideTone_Reserved[0] = audio_nvdm_HW_config.sidetone_config.SideTone_Gain;
                if(audio_nvdm_HW_config.sidetone_config.SideTone_Reserved[0] >> 7){
                    sidetone.gain   = (0xFFFFFF00 | audio_nvdm_HW_config.sidetone_config.SideTone_Reserved[0]) * 100;      /*Default: 800 /100 = 8dB change to -100 /100 = -1dB. Because record mic would sync analog gain to eSCO.*/
                } else {
                    sidetone.gain   = (audio_nvdm_HW_config.sidetone_config.SideTone_Reserved[0]) * 100;
                }
                if(g_side_tone_gain_common == SIDETONE_GAIN_MAGIC_NUM) {
                    g_side_tone_gain_common = sidetone.gain/100;
                } else {
                    sidetone.gain = g_side_tone_gain_common * 100;
                    audio_src_srv_report("[Sink][AM]User defined side tone gain value for none-HFP: %d\n", 1, g_side_tone_gain_common);
                }
            }

            bt_sink_srv_audio_setting_vol_info_t vol_info = {0};

            if(hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_RECORD) || (g_prHfp_media_handle != NULL)) {
                ;
            } else {
                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = BT_HFP_CODEC_TYPE_MSBC;
                vol_info.vol_info.hfp_vol_info.dev_in = HAL_AUDIO_DEVICE_MAIN_MIC;
                vol_info.vol_info.hfp_vol_info.dev_out = HAL_AUDIO_DEVICE_HEADSET;
                vol_info.vol_info.hfp_vol_info.lev_in = 0;
                vol_info.vol_info.hfp_vol_info.lev_out = 0;
                bt_sink_srv_am_set_volume(STREAM_IN, &vol_info);
            }
#endif
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
            ami_set_afe_param(STREAM_OUT, HAL_AUDIO_SAMPLING_RATE_16KHZ, true);
#endif
#if defined(AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
            hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_SIDETONE, sidetone.out_device, sidetone.sample_rate, true);
#endif
            p_param_share = hal_audio_dsp_controller_put_paramter( &sidetone, sizeof(mcu2dsp_sidetone_param_t), AUDIO_MESSAGE_TYPE_SIDETONE);
            audio_src_srv_report("[Sink][AM]side tone enable:side tone gain = %d", 1,sidetone.gain/100);
            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_START, 0, (uint32_t)p_param_share, true);   //Sidetone start ack will ack first, and then begin ramp up.
        } else {
            audio_src_srv_report("[Sink][AM]side tone enable Fail: Already enabled\n", 0);
        }
    }
    #endif
}

void audio_side_tone_set_volume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    uint32_t sidetone_gain = 0;
    sidetone_scenario_t scenario;

    sidetone_gain   = amm_ptr->background_info.local_feature.feature_param.sidetone_param.side_tone_gain;
    scenario = amm_ptr->background_info.local_feature.feature_param.sidetone_param.scenario;
    audio_src_srv_report("[Sink][AM]side tone set volume:sidetone_gain = %d, scenario = %d",2,sidetone_gain, (int)scenario);
    if(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE) {
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_SET_VOLUME, 0, sidetone_gain * 100, false);
    }else {
        audio_src_srv_err("[Sink][AM]side tone set volume error,side tone not enable!",0);
    }
    //record the sidetone gain no matter the sidetone is enabled or not
    if(amm_ptr->background_info.local_feature.feature_param.sidetone_param.scenario == SIDETONE_SCENARIO_HFP) {
        g_side_tone_gain_hfp = sidetone_gain;
        g_side_tone_gain_common = g_side_tone_gain_hfp;//Temp, currently all the sidetone is set by type HFP
    } else {
        g_side_tone_gain_common = sidetone_gain;
    }
}

static void audio_side_tone_disable_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    #if defined(MTK_AVM_DIRECT)
    mcu2dsp_sidetone_param_t sidetone = {0};
    void *p_param_share;
    mcu2dsp_open_stream_in_param_t in_device;
    mcu2dsp_open_stream_out_param_t out_device;

    memset(&out_device,0,sizeof(out_device));
    memset(&in_device,0,sizeof(in_device));

    audio_src_srv_report("[Sink][AM]audio_side_tone_disable_hdlr\n", 0);
    ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_REQUEST, false);
    if((g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE)
        && (!(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_WAITING_STOP)))
    {
#if 0
        sidetone.out_device                      = HAL_AUDIO_DEVICE_DAC_DUAL;
        sidetone.out_interface                   = HAL_AUDIO_INTERFACE_NONE;
        sidetone.out_misc_parms                  = DOWNLINK_PERFORMANCE_NORMAL;
        sidetone.in_device                       = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
        sidetone.in_interface                    = HAL_AUDIO_INTERFACE_1;
        sidetone.in_channel                      = HAL_AUDIO_DIRECT;
        sidetone.in_misc_parms                   = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
        sidetone.sample_rate                     = 16000;
#else
        hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &out_device); /*Expect stream out channel is default*/
        hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &in_device);   /*Sidetone.channel should be mic channel setting.*/
        sidetone.out_device                      = out_device.afe.audio_device;
        sidetone.out_interface                   = out_device.afe.audio_interface;
        sidetone.out_misc_parms                  = out_device.afe.misc_parms;
        sidetone.in_device                       = in_device.afe.audio_device;
        sidetone.in_interface                    = in_device.afe.audio_interface;
        sidetone.in_channel                      = in_device.afe.stream_channel;
        sidetone.in_misc_parms                   = in_device.afe.misc_parms;
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
        sidetone.sample_rate                     = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
        sidetone.sample_rate                     = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#else
        sidetone.sample_rate                     = 16000;
#endif
#endif
        sidetone.gain           = 0;
        p_param_share = hal_audio_dsp_controller_put_paramter( &sidetone, sizeof(mcu2dsp_sidetone_param_t), AUDIO_MESSAGE_TYPE_SIDETONE);
        audio_src_srv_report("[Sink][AM]side tone disable\n", 0);
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_STOP, 0, (uint32_t)p_param_share, false);
        //Not wait Sidetone stop ack, because sidetone ramp down would block AM task.
        ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_WAITING_STOP, true);
        vPortFreeNC(pFIRBuf);
#if 0
        ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_ENABLE, false);
        aud_side_tone_control(false);
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_SIDETONE, false);
#endif
    }
    else
    {
        audio_src_srv_report("[Sink][AM]side tone disable Fail: Have not enabled\n", 0);
    }
    #endif
}

static void audio_dl_suspend_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    #if defined(MTK_AVM_DIRECT)
    audio_src_srv_report("[Sink][AM]audio_dl_suspend_hdlr\n", 0);
    aud_dl_suspend();
    #endif
}

static void audio_dl_resume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    #if defined(MTK_AVM_DIRECT)
    audio_src_srv_report("[Sink][AM]audio_dl_resume_hdlr\n", 0);
    if(!(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_ENABLE)){
        aud_dl_resume();
    } else {
        if(g_prCurrent_player != NULL){
            if(g_prCurrent_player->type == HFP){
                aud_dl_resume();
                audio_src_srv_report("[Sink][AM]audio_dl_resume_hdlr eSCO happening.\n", 0);
            }
        }
    #ifndef FIXED_SAMPLING_RATE_TO_48KHZ
        audio_src_srv_report("[Sink][AM]audio_dl_resume_hdlr fail, sidetone still exist\n", 0);
    #else
        aud_dl_resume();
        audio_src_srv_report("[Sink][AM]audio_dl_resume_hdlr pass, not suspended by sidetone\n", 0);
    #endif
    }
    if(g_am_task_mask & AM_TASK_MASK_SIDE_TONE_REQUEST){
        audio_side_tone_enable_hdlr(NULL);
        ami_set_audio_mask(AM_TASK_MASK_SIDE_TONE_REQUEST, false);
    }
    #endif
}

static void audio_ul_suspend_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    #if defined(MTK_AVM_DIRECT)
    audio_src_srv_report("[Sink][AM]audio_ul_suspend_hdlr\n", 0);
    aud_ul_suspend();
    #endif
}

static void audio_ul_resume_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    #if defined(MTK_AVM_DIRECT)
    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if((((audio_nvdm_HW_config.Voice_InputDev & 0xC0) >> 6) == 0x02) && (((audio_nvdm_HW_config.Voice_OutputDev & 0xF0) >> 4) == 0x03)){
    #else
        if ((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x02) && (audio_nvdm_HW_config.voice_scenario.Voice_Output_Path == 0x03)) {
    #endif
        audio_src_srv_report("[Sink][AM]audio_ul_resume_hdlr\n", 0);
        aud_ul_resume();
    } else {
        audio_src_srv_report("[Sink][AM]audio_ul_resume_hdlr. Not use I2S_Master, No need resume UL.\n", 0);
    }
    #endif
}

void audio_set_anc_compensate(bt_sink_srv_am_type_t type, uint32_t event, bt_sink_srv_am_type_t *cur_type)
{
    uint32_t voice_eq_nvkey_is_ready = 0;
#ifdef MTK_VOICE_ANC_EQ
    uint8_t *nvkey_buf = NULL;
#endif
#ifdef MTK_ANC_ENABLE
  #ifndef MTK_ANC_V2
    uint32_t on_event = ANC_CONTROL_EVENT_ON;
  #else
    uint32_t on_event = AUDIO_ANC_CONTROL_EVENT_ON;
  #endif
#else
    uint32_t on_event = 1 << 0;
#endif

    if (type == HFP) {
        voice_eq_nvkey_is_ready = 1;
    } else if (type == NONE) {
        type = (g_prCurrent_player != NULL) ? g_prCurrent_player->type : NONE;
    }

    if (event == 0) {
        uint8_t anc_enable = 0;
    #ifdef MTK_ANC_ENABLE
      #ifndef MTK_ANC_V2
        uint8_t hybrid_enable;
        anc_get_status(&anc_enable, NULL, &hybrid_enable);
        if ((anc_enable > 0) && (hybrid_enable > 0)) {
            event = ANC_CONTROL_EVENT_ON;
        } else {
            event = ANC_CONTROL_EVENT_OFF;
        }
      #else
        audio_anc_control_get_status(&anc_enable, NULL, NULL, NULL, NULL, NULL);
        if (anc_enable > 0) {
            event = AUDIO_ANC_CONTROL_EVENT_ON;
        } else {
            event = AUDIO_ANC_CONTROL_EVENT_OFF;
        }
      #endif
    #else
      #if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        RACE_DSPREALTIME_COSYS_GET_PARAM(AM_ANC, &anc_enable);
        if (anc_enable > 0) {
            event = 1 << 0;
        } else {
            event = 1 << 1;
        }
      #else
        _UNUSED(anc_enable);
        event = 1 << 1;
      #endif
    #endif
    }

    if (cur_type != NULL) {
        *cur_type = type;
    }
    switch (type)
    {
        case A2DP:
        case AWS:
        {
#ifdef MTK_PEQ_ENABLE
            bt_sink_srv_am_peq_param_t peq_param;
            memset(&peq_param, 0, sizeof(bt_sink_srv_am_peq_param_t));
            /* post PEQ */
            peq_param.phase_id = 1;
          #ifdef MTK_ANC_ENABLE
            #ifndef MTK_ANC_V2
            if (event == on_event) {
                peq_param.enable = 1;
                peq_param.sound_mode = POST_PEQ_FBANC_SOUND_MODE;
            } else {
                peq_param.enable = POST_PEQ_DEFAULT_ENABLE;
                peq_param.sound_mode = POST_PEQ_DEFAULT_SOUND_MODE;
            }
            aud_set_peq_param(aud_get_peq_audio_path(A2DP), &peq_param);
            g_peq_handle.a2dp_post_peq_enable = peq_param.enable;
            if (peq_param.enable) {
                g_peq_handle.a2dp_post_peq_sound_mode = peq_param.sound_mode;
            }
            #else
            if (event == on_event) {
                peq_param.enable = g_peq_handle.a2dp_post_peq_enable;
                peq_param.sound_mode = POST_PEQ_FBANC_SOUND_MODE;
            } else {
                peq_param.enable = g_peq_handle.a2dp_post_peq_enable;
                peq_param.sound_mode = g_peq_handle.a2dp_post_peq_sound_mode;
            }
            aud_set_peq_param(aud_get_peq_audio_path(A2DP), &peq_param);
            #endif
          #else
            if (event == on_event) {
                peq_param.enable = g_peq_handle.a2dp_post_peq_enable;
                peq_param.sound_mode = POST_PEQ_FBANC_SOUND_MODE;
            } else {
                peq_param.enable = g_peq_handle.a2dp_post_peq_enable;
                peq_param.sound_mode = g_peq_handle.a2dp_post_peq_sound_mode;
            }
            aud_set_peq_param(aud_get_peq_audio_path(A2DP), &peq_param);
          #endif
#endif
            break;
        }
        case HFP:
        {
#ifdef MTK_VOICE_ANC_EQ
            /* Voice receiving EQ */
            if (voice_eq_nvkey_is_ready == 0) {
                mem_nvdm_info_t flash_nvdm;
                sysram_status_t status;
                uint16_t nvkey1, nvkey2, target_nvkey = 0;
                if (g_prCurrent_player->local_context.hfp_format.hfp_codec.type == BT_HFP_CODEC_TYPE_CVSD) {
                    nvkey1 = NVKEY_DSP_PARA_NB_RX_EQ;
                    nvkey2 = NVKEY_DSP_PARA_NB_RX_EQ_2ND;
                    if (event == on_event) {
                        flash_nvdm.nvdm_id = NVKEY_DSP_PARA_NB_RX_EQ_2ND;
                    } else {
                        flash_nvdm.nvdm_id = NVKEY_DSP_PARA_NB_RX_EQ;
                    }
                } else {
                    nvkey1 = NVKEY_DSP_PARA_WB_RX_EQ;
                    nvkey2 = NVKEY_DSP_PARA_WB_RX_EQ_2ND;
                    if (event == on_event) {
                        flash_nvdm.nvdm_id = NVKEY_DSP_PARA_WB_RX_EQ_2ND;
                    } else {
                        flash_nvdm.nvdm_id = NVKEY_DSP_PARA_WB_RX_EQ;
                    }
                }
                flash_nvdm.length = 0; //dummy
                status = flash_memory_query_nvdm_data_length(flash_nvdm.nvdm_id, &flash_nvdm.length);
                if (status != NVDM_STATUS_NAT_OK) {
                    audio_src_srv_err("change RX EQ error: query nvkey length error status:%d ", 1, status);
                    break;
                } else {
                    target_nvkey = flash_nvdm.nvdm_id;
                }
                nvkey_buf = (uint8_t *)pvPortMalloc(flash_nvdm.length);
                if (nvkey_buf == NULL) {
                    audio_src_srv_err("change RX EQ error: malloc fail size:%d ", 1, flash_nvdm.length);
                    break;
                }
                status = flash_memory_read_nvdm_data(flash_nvdm.nvdm_id, nvkey_buf, &flash_nvdm.length);
                if (status != NVDM_STATUS_NAT_OK) {
                    audio_src_srv_err("change RX EQ error: read nvkey error status:%d ", 1, status);
                    break;
                }
                flash_nvdm.mem_pt = nvkey_buf;
                flash_nvdm.nvdm_id = nvkey1;
                status = nat_table_write_audio_nvdm_data(flash_nvdm, c_sram_mode);
                if (status != NVDM_STATUS_NAT_OK) {
                    flash_nvdm.nvdm_id = nvkey2;
                    status = nat_table_write_audio_nvdm_data(flash_nvdm, c_sram_mode);
                }
                if (status != NVDM_STATUS_NAT_OK) {
                    audio_src_srv_err("set HFP AEC NR error, param_type: AEC_NR_PARAM_TYPE_RX_EQ, status:%d", 1, status);
                } else {
                    audio_src_srv_report("set HFP AEC NR success, param_type: AEC_NR_PARAM_TYPE_RX_EQ, nvkey:0x%x", 1, target_nvkey);
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AEC_NR_SET_PARAM, AEC_NR_PARAM_TYPE_RX_EQ, 0, true);
                }
            }
#else
            _UNUSED(voice_eq_nvkey_is_ready);
#endif
            break;
        }
        case NONE:
            break;
    }
#ifdef MTK_VOICE_ANC_EQ
    if (nvkey_buf != NULL) {
        vPortFree(nvkey_buf);
    }
#endif
    _UNUSED(on_event);
}

#ifdef MTK_ANC_ENABLE
void audio_set_anc_compensate_phase2(bt_sink_srv_am_type_t type, uint32_t event)
{
    _UNUSED(type);
    _UNUSED(event);
#ifndef MTK_ANC_V2
#ifdef MTK_DEQ_ENABLE
    bt_sink_srv_am_peq_param_t peq_param;
    memset(&peq_param, 0, sizeof(bt_sink_srv_am_peq_param_t));
    if (type == NONE) {
        type = (g_prCurrent_player != NULL) ? g_prCurrent_player->type : NONE;
    }
    if ((type != A2DP) && (type != AWS)) { //DEQ only used in A2DP case now.
        return;
    }
    if (event == 0) {
        uint8_t anc_enable, hybrid_enable;
        anc_get_status(&anc_enable, NULL, &hybrid_enable);
        if ((anc_enable > 0) && (hybrid_enable > 0)) {
            event = ANC_CONTROL_EVENT_ON;
        } else {
            event = ANC_CONTROL_EVENT_OFF;
        }
    }
    peq_param.phase_id = 2;
    if (event == ANC_CONTROL_EVENT_OFF) {
        peq_param.enable = 0;
    } else if (event == ANC_CONTROL_EVENT_ON) {
        peq_param.enable = anc_get_deq_enable();
        peq_param.sound_mode = DEQ_AUDIO_SOUND_MODE;
        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_DEQ_SET_PARAM, 0, anc_get_deq_param(), true);
    }
    aud_set_peq_param(aud_get_peq_audio_path(A2DP), &peq_param);
#endif
#else
    //No need to use PEQ as DEQ.
#endif
}

#endif

/*****************************************************************************
 * FUNCTION
 *  audio_set_feature_hdlr
 * DESCRIPTION
 *  This function is an interface that can set parameters to DSP or set feature control through AM task
 * PARAMETERS
 *  amm_ptr          [IN]
 * RETURNS
 *  void
 *****************************************************************************/
static void audio_set_feature_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    bt_sink_srv_am_feature_t *ami_feature = &amm_ptr->background_info.local_feature;
    am_feature_type_t feature_type, feature_type_mask;
    uint8_t shift = 0;

    if(amm_ptr->background_info.aud_id == FEATURE_NO_NEED_ID){
    } else {
        if(g_prCurrent_player == NULL) {
            audio_src_srv_report("[Sink][AM]audio_set_param_hdlr error, no current player\n", 0);
            return;
        } else if (amm_ptr->background_info.type != g_prCurrent_player->type) {
            audio_src_srv_report("[Sink][AM]audio_set_param_hdlr error, player changed (%d -> %d)\n",2,amm_ptr->background_info.type,g_prCurrent_player->type);
            return;
        }
    }
    audio_src_srv_report("[Sink][AM]audio_set_param_hdlr with feature type %d\n",1, ami_feature->type_mask);

    feature_type_mask = ami_feature->type_mask;

    do {
        feature_type = feature_type_mask & (1 << shift++);
        feature_type_mask &= (~feature_type);
        if(feature_type == 0)
            continue;
        switch(feature_type)
        {
#ifdef MTK_PEQ_ENABLE
            case AM_A2DP_PEQ: {
                if (ami_feature->feature_param.peq_param.not_clear_sysram == 0) {
                    //audio_nvdm_reset_sysram();
                }
#ifdef AIR_PEQ_WRITE_NVKEY_ENABLE
                if(ami_feature->feature_param.peq_param.sound_mode == (aud_peq_get_total_mode(PEQ_AUDIO_PATH_A2DP,0)+1) || ami_feature->feature_param.peq_param.sound_mode == PEQ_ON){
                    ami_feature->feature_param.peq_param.sound_mode = g_peq_handle.a2dp_pre_peq_sound_mode;
                    audio_src_srv_report("[Sink][AM]PEQ ON,SOUND MODE: %d\n",1, ami_feature->feature_param.peq_param.sound_mode);
                }
#endif
                if (aud_set_peq_param(PEQ_AUDIO_PATH_A2DP, &ami_feature->feature_param.peq_param) == 0)
                {
                    if (ami_feature->feature_param.peq_param.sound_mode != PEQ_SOUND_MODE_REAlTIME) {
                        if (ami_feature->feature_param.peq_param.phase_id == 0) {
                            g_peq_handle.a2dp_pre_peq_enable = ami_feature->feature_param.peq_param.enable;
                            if (ami_feature->feature_param.peq_param.enable) {
                                g_peq_handle.a2dp_pre_peq_sound_mode = ami_feature->feature_param.peq_param.sound_mode;
                            }
                        } else if (ami_feature->feature_param.peq_param.phase_id == 1) {
                            g_peq_handle.a2dp_post_peq_enable = ami_feature->feature_param.peq_param.enable;
                            if (ami_feature->feature_param.peq_param.enable) {
                                g_peq_handle.a2dp_post_peq_sound_mode = ami_feature->feature_param.peq_param.sound_mode;
                            }
                        }
#ifdef AIR_PEQ_WRITE_NVKEY_ENABLE
                        aud_peq_save_sound_mode();
#endif
                    }

                    audio_src_srv_report("[Sink][AM]Set A2DP PEQ param finish, phase_id:%d not_clear_sysram:%d\n",2,ami_feature->feature_param.peq_param.phase_id,ami_feature->feature_param.peq_param.not_clear_sysram);
                }
                break;
            }
#endif
#ifdef MTK_LINEIN_PEQ_ENABLE
            case AM_LINEIN_PEQ: {
                if (ami_feature->feature_param.peq_param.not_clear_sysram == 0) {
                    //audio_nvdm_reset_sysram();
                }
                if (aud_set_peq_param(PEQ_AUDIO_PATH_LINEIN, &ami_feature->feature_param.peq_param) == 0)
                {
                    if (ami_feature->feature_param.peq_param.sound_mode != PEQ_SOUND_MODE_REAlTIME) {
                        if (ami_feature->feature_param.peq_param.phase_id == 0) {
                            g_peq_handle.linein_pre_peq_enable = ami_feature->feature_param.peq_param.enable;
                            if (ami_feature->feature_param.peq_param.enable) {
                                g_peq_handle.linein_pre_peq_sound_mode = ami_feature->feature_param.peq_param.sound_mode;
                            }
                        } else if (ami_feature->feature_param.peq_param.phase_id == 1) {
                            g_peq_handle.linein_post_peq_enable = ami_feature->feature_param.peq_param.enable;
                            if (ami_feature->feature_param.peq_param.enable) {
                                g_peq_handle.linein_post_peq_sound_mode = ami_feature->feature_param.peq_param.sound_mode;
                            }
                        }
                    }
                    audio_src_srv_report("[Sink][AM]Set LINEIN PEQ param finish, phase_id:%d not_clear_sysram:%d\n",2,ami_feature->feature_param.peq_param.phase_id,ami_feature->feature_param.peq_param.not_clear_sysram);
                }
                break;
            }
#endif
#if defined(MTK_AMP_DC_COMPENSATION_ENABLE)
            case DC_COMPENSATION: {
                audio_src_srv_report("[Sink][AM] Init: DC Compensation\n", 0);
                uint16_t dc_compensation_value = 0xffff;
                mcu2dsp_open_param_t open_param;
                void *p_param_share;
                memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
                hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
                ami_hal_audio_status_set_running_flag(1, 1);
                // if(dc_compensation_value == 0) {
                // Open Amp
                //get DAC Class G/Class AB type and send to DSP with start & stop msg
                p_param_share = hal_audio_dsp_controller_put_paramter(&open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_COMMON);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_DC_COMPENSATION_START, 0, (uint32_t)p_param_share, true);

                // Calculate dc compensation value
                dc_compensation_value = get_dc_compensation_value();
                // Close Amp and send dc compensation value to DSP
                memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
                hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
                #ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
                //no delay flag for dual chip slave side
                open_param.stream_out_param.afe.misc_parms = true; // // just reuse this parameter to disable AMP Delay 2s
                #endif
                p_param_share = hal_audio_dsp_controller_put_paramter(&open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_COMMON);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_DC_COMPENSATION_STOP, dc_compensation_value, (uint32_t)p_param_share, true);
                ami_hal_audio_status_set_running_flag(1, 0);
                audio_src_srv_report("[Sink][AM] Init Finish: DC Compensation, cal value 0x%x\n", 1, dc_compensation_value);
                break;
            }
#endif

#ifdef MTK_AWS_MCE_ENABLE
            case AM_HFP_AVC: {
                //audio_src_srv_report("[NDVC] ami_feature->feature_param.avc_vol: %d", 1, ami_feature->feature_param.avc_vol);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_VOICE_DL_AVC_PARA_SEND, 0, (uint32_t)ami_feature->feature_param.avc_vol, false);
                break;
            }
#endif
#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
            case AM_ANC:{
                audio_anc_control_result_t anc_ret = AUDIO_ANC_CONTROL_EXECUTION_SUCCESS;
                audio_src_srv_report("[Sink][AM]audio_set_param_hdlr with event(%d)\n", 1, ami_feature->feature_param.anc_param.event);
                if(ami_feature->feature_param.anc_param.event == AUDIO_ANC_CONTROL_EVENT_ON){
                    #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                    ami_execute_vendor_se(EVENT_ANC_START);
                    #endif
                    anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_AM, ami_feature->feature_param.anc_param.event, &ami_feature->feature_param.anc_param.cap);
                }else if (ami_feature->feature_param.anc_param.event == AUDIO_ANC_CONTROL_EVENT_OFF){
                    anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_AM, ami_feature->feature_param.anc_param.event, &ami_feature->feature_param.anc_param.cap);
                    #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                    ami_execute_vendor_se(EVENT_ANC_STOP);
                    #endif
                }else if (ami_feature->feature_param.anc_param.event == AUDIO_ANC_CONTROL_EVENT_COPY_FILTER){
                    anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_AM, ami_feature->feature_param.anc_param.event, &ami_feature->feature_param.anc_param.cap);
                }else if (ami_feature->feature_param.anc_param.event == AUDIO_ANC_CONTROL_EVENT_SET_REG){
                    anc_ret = audio_anc_control_command_handler(AUDIO_ANC_CONTROL_SOURCE_FROM_AM, ami_feature->feature_param.anc_param.event, &ami_feature->feature_param.anc_param.cap);
                }
                audio_src_srv_report("[Sink][AM]audio_set_param_hdlr with ret(%d) filter_id(%d) type(%d) filter_mask(0x%x) sram_bank(%d)\n", 5,
                          anc_ret, ami_feature->feature_param.anc_param.cap.filter_cap.filter_id, ami_feature->feature_param.anc_param.cap.filter_cap.type,
                          ami_feature->feature_param.anc_param.cap.filter_cap.filter_mask, ami_feature->feature_param.anc_param.cap.filter_cap.sram_bank);
                break;
            }
#else
            case AM_ANC:{
                if(ami_feature->feature_param.anc_param.event == ANC_CONTROL_EVENT_ON){
                    anc_on(ami_feature->feature_param.anc_param.filter, ami_feature->feature_param.anc_param.user_runtime_gain, ami_feature->feature_param.anc_param.anc_control_callback, (uint8_t)ami_feature->feature_param.anc_param.param);
                }else if (ami_feature->feature_param.anc_param.event == ANC_CONTROL_EVENT_OFF){
                    anc_off(ami_feature->feature_param.anc_param.anc_control_callback, 0, (uint8_t)ami_feature->feature_param.anc_param.param);
                }else if (ami_feature->feature_param.anc_param.event == ANC_CONTROL_EVENT_SET_VOLUME){
                    anc_set_volume(&ami_feature->feature_param.anc_param.sw_gain);
                }else if (ami_feature->feature_param.anc_param.event == ANC_CONTROL_EVENT_SET_RUNTIME_VOLUME){
                    anc_set_runtime_volume(ami_feature->feature_param.anc_param.user_runtime_gain);
                }else if (ami_feature->feature_param.anc_param.event == ANC_CONTROL_EVENT_SET_ATTACH_FUNC){
                    anc_set_attach_enable((uint8_t)ami_feature->feature_param.anc_param.param);
                }
                break;
            }
#endif
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
            case AM_AUDIO_COSYS:{
                if(ami_feature->feature_param.cosys_param.sub_type == AM_ANC){
                    audio_set_anc_compensate(NONE, ami_feature->feature_param.cosys_param.cosys_event, NULL);
                }
                break;
            }
#endif
#ifdef MTK_PROMPT_SOUND_ENABLE
            case AM_VP: {
                if(ami_feature->feature_param.vp_param.event == PROMPT_CONTROL_MEDIA_PLAY){
                    #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                    ami_execute_vendor_se(EVENT_VP_START);
                    #endif
                    #if 0
                    prompt_control_play_tone(ami_feature->feature_param.vp_param.tone_type,
                                             ami_feature->feature_param.vp_param.tone_buf,
                                             ami_feature->feature_param.vp_param.tone_size,
                                             ami_feature->feature_param.vp_param.vp_end_callback);
                    #else
                    bool ret= false;
                    ret = prompt_control_play_tone_internal(ami_feature->feature_param.vp_param.tone_type,
                                                            ami_feature->feature_param.vp_param.tone_buf,
                                                            ami_feature->feature_param.vp_param.tone_size,
                                                            ami_feature->feature_param.vp_param.sync_time,
                                                            ami_feature->feature_param.vp_param.vp_end_callback);
                    _UNUSED(ret);
                    #endif
                }else if (ami_feature->feature_param.vp_param.event == PROMPT_CONTROL_MEDIA_STOP){
                    prompt_control_stop_tone_internal();
                    #ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                    ami_execute_vendor_se(EVENT_VP_STOP);
                    #endif
            #ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
                } else if (ami_feature->feature_param.vp_param.event == PROMPT_CONTROL_DUMMY_SOURCE_START) {
                    audio_src_srv_report("[AM_VP][DummySource]index(%d) mode(%d) vol_lev(%d)", 3, ami_feature->feature_param.vp_param.dummy_source_param.index,
                                        ami_feature->feature_param.vp_param.dummy_source_param.mode, ami_feature->feature_param.vp_param.dummy_source_param.vol_lev);
                    prompt_control_dummy_source_start_internal(ami_feature->feature_param.vp_param.dummy_source_param);
                }else if (ami_feature->feature_param.vp_param.event == PROMPT_CONTROL_DUMMY_SOURCE_STOP) {
                    prompt_control_dummy_source_stop_internal();
            #endif
                }else{
                    audio_src_srv_report("AM_VP Event error", 0);
                }
                break;
            }
#endif
            case AM_DYNAMIC_CHANGE_DSP_SETTING: {
                if(ami_feature->feature_param.channel == AUDIO_CHANNEL_SELECTION_STEREO){
                    hal_gpio_status_t status;
                    hal_gpio_data_t channel_gpio_data = HAL_GPIO_DATA_LOW;
                    uint8_t channel_temp;
                    //Audio Channel selection setting
                    if(audio_Channel_Select.modeForAudioChannel){
                        //HW_mode
                        status = hal_gpio_get_input((hal_gpio_pin_t)audio_Channel_Select.hwAudioChannel.gpioIndex, &channel_gpio_data);
                        if (status == HAL_GPIO_STATUS_OK) {
                            if (channel_gpio_data == HAL_GPIO_DATA_HIGH) {
                                channel_temp = (audio_Channel_Select.hwAudioChannel.audioChannelGPIOH & 0x0F);
                            } else {
                                channel_temp = (audio_Channel_Select.hwAudioChannel.audioChannelGPIOL & 0x0F);
                            }
                        } else {
                            channel_temp = AU_DSP_CH_LR; //default.
                            audio_src_srv_report("Get Stream in channel setting false with HW_mode.", 0);
                        }
                    } else {
                        //SW_mode
                        channel_temp = (audio_Channel_Select.audioChannel & 0x0F);
                    }
                    switch(channel_temp)
                    {
                        case AU_DSP_CH_LR: {
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 0, (uint32_t)AUDIO_DSP_CHANNEL_SELECTION_STEREO, false);
                            break;
                        }
                        case AU_DSP_CH_L: {
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 0, (uint32_t)AUDIO_DSP_CHANNEL_SELECTION_BOTH_L, false);
                            break;
                        }
                        case AU_DSP_CH_R: {
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 0, (uint32_t)AUDIO_DSP_CHANNEL_SELECTION_BOTH_R, false);
                            break;
                        }
                        default: {
                            hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 0, (uint32_t)AUDIO_DSP_CHANNEL_SELECTION_STEREO, false);
                            break;
                        }
                    }
                } else if(ami_feature->feature_param.channel == AUDIO_CHANNEL_SELECTION_MONO){
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 0, (uint32_t)AUDIO_DSP_CHANNEL_SELECTION_MONO, false);
                } else if(ami_feature->feature_param.channel == AUDIO_CHANNEL_SELECTION_BOTH_L){
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 0, (uint32_t)AUDIO_DSP_CHANNEL_SELECTION_BOTH_L, false);
                } else if(ami_feature->feature_param.channel == AUDIO_CHANNEL_SELECTION_BOTH_R){
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_CHANGE_DSP_CHANEL, 0, (uint32_t)AUDIO_DSP_CHANNEL_SELECTION_BOTH_R, false);
                }
                break;
            }
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
            case AM_ADAPTIVE_FF:{

                n9_dsp_share_info_t *p_share_info = hal_audio_query_record_share_info();
                hal_audio_reset_share_info(p_share_info);
                U8* write_ptr = (U8*)p_share_info->start_addr;
                U32 copy_length = sizeof(anc_fwd_iir_t);

                if ((ami_feature->feature_param.adaptive_ff_param.cmp_new_filter) && (ami_feature->feature_param.adaptive_ff_param.cmp_ori_filter)) {
                    memcpy(write_ptr, (U8*)ami_feature->feature_param.adaptive_ff_param.cmp_ori_filter, copy_length);
                    memcpy(write_ptr + copy_length, (U8*)ami_feature->feature_param.adaptive_ff_param.cmp_new_filter, copy_length);
                    p_share_info->write_offset += (2 * copy_length);
                    audio_src_srv_report("[Sink][AM][user_trigger_ff]cpy ori/new filter to share buff, write_ptr=0x%x, write_offset:%d, read_offset:%d, length:%d", 4, (U32)write_ptr, p_share_info->write_offset, p_share_info->read_offset, p_share_info->length);
                }
                audio_src_srv_report("[Sink][AM][user_trigger_ff]notify DSP to cmp filter\n", 0);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_RECORD_USER_TRIGGER_FF_CMP_FILTER, 0, 0, true);
                break;
            }
#else
            case AM_ADAPTIVE_FF:{
                audio_src_srv_report("[Sink][AM][user_trigger_ff]Set param\n", 0);
                audio_user_trigger_adaptive_ff_set_parameters();
                break;
            }
#endif
#endif

#if defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
            case AM_ANC_MONITOR:{
                switch(ami_feature->feature_param.anc_monitor_param.event)
                {
                    case AUDIO_ANC_MONITOR_SET_INFO:{
                        audio_src_srv_report("[Sink][AM][ANC_Monitor]Set info, type:%d, para:%d \n", 2, ami_feature->feature_param.anc_monitor_param.event_type, ami_feature->feature_param.anc_monitor_param.param);
                        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AUDIO_ANC_ADAPTIVE_SET,
                                              ami_feature->feature_param.anc_monitor_param.event_type,
                                              ami_feature->feature_param.anc_monitor_param.param,
                                              true);
                        break;
                    }
                    case AUDIO_ANC_MONITOR_GET_INFO:{
                        extern bool g_adaptive_ANC_getinfo_DSP_write_done;
                        audio_src_srv_report("[Sink][AM][ANC_Monitor]Get info, share_buffer_addr 0x%x\n", 1, ami_feature->feature_param.anc_monitor_param.param);

                        //ami_feature->feature_param.anc_monitor_param.param = ANC_MONITOR_INFO_BASE; //share buffer addr
                        hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AUDIO_ANC_ADAPTIVE_GET,
                                              ami_feature->feature_param.anc_monitor_param.event_type,
                                              ami_feature->feature_param.anc_monitor_param.param,
                                              true);
                        g_adaptive_ANC_getinfo_DSP_write_done = true;
                        break;
                    }
                    default:{
                        audio_src_srv_report("[Sink][AM][ANC_Monitor]Wrong event\n", 0);
                        break;
                    }
                }
            }
#endif


#ifdef AIR_BT_CODEC_BLE_ENABLED
            case AM_BLE: {
                void *p_param_share;

                audio_src_srv_report("[Sink][AM][ble] ble feature event %d", 1, ami_feature->feature_param.ble_param.event);
                if (ami_feature->feature_param.ble_param.event == BLE_FEATURE_EVENT_PLAY_INFO) {
                    p_param_share = hal_audio_dsp_controller_put_paramter(&(ami_feature->feature_param.ble_param.param.play_info), sizeof(ble_init_play_info_t), AUDIO_MESSAGE_TYPE_BLE_AUDIO_DL);
                    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BLE_AUDIO_INIT_PLAY_INFO, 0, (uint32_t)p_param_share, false);
                }
                break;
            }
#endif
#if defined (MTK_AUDIO_LOOPBACK_TEST_ENABLE)
            case AM_AUDIO_LOOPBACK: {
                n9_dsp_share_info_t *share_info = hal_audio_query_bt_audio_dl_share_info();
                memset(share_info, 0, sizeof(uint32_t) * 4);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_AUDIO_LOOPBACK_TEST, 0, (uint32_t)share_info, false);
                break;
            }
#endif
#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
            case AM_AUDIO_BT_SET_PLAY_EN: {
                hal_audio_set_a2dp_play_en(ami_feature->feature_param.play_en_param.sequence_number,ami_feature->feature_param.play_en_param.bt_clock);
                break;
            }
#endif
            default:
                break;
        }
    } while(feature_type_mask != 0);
}

#ifdef __AUDIO_COMMON_CODEC_ENABLE__

static void am_files_audio_callback(audio_codec_media_handle_t *hdl, audio_codec_event_t event)
{
    if (g_prCurrent_player->type == FILES) {
        g_prCurrent_player->local_context.files_format.file_event.event.audio.event = event;
        bt_sink_srv_ami_send_amm(MOD_AM, MOD_L1SP, AUD_FILE_PROC_IND,
                             MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ, g_prCurrent_player,
                             TRUE, ptr_callback_amm);
    }
}

static void am_audio_get_write_buffer(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_audio_media_handle->get_write_buffer(g_am_audio_media_handle, buffer, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]get_write_buffer--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static void am_audio_write_data_done(bt_sink_srv_am_id_t aud_id, uint32_t length)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_audio_media_handle->write_data_done(g_am_audio_media_handle, length);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]write_data_done--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}


static void am_audio_finish_write_data(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_audio_media_handle->finish_write_data(g_am_audio_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]finish_write_data--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif
}

static int32_t am_audio_get_data_count(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        return g_am_audio_media_handle->get_data_count(g_am_audio_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]get_data_count--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif

    return 0;
}

static int32_t am_audio_get_free_space(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;
    //am_file_type_t type = g_prCurrent_player->local_context.files_format.file_event.type;

    if (aud_id == g_prCurrent_player->aud_id) {
        return g_am_audio_media_handle->get_free_space(g_am_audio_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]get_data_count--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }
    #ifndef MTK_DEBUG_LEVEL_INFO
    g_test_fix_warning = err;
    #endif

    return 0;
}

#define _AM_AUDIO_PROCESS_(_action_,aud_id,_file_state_) { \
    int32_t err = 0; \
    if (aud_id == g_prCurrent_player->aud_id) {\
        g_am_audio_media_handle->_action_(g_am_audio_media_handle);\
        g_am_file_state = FILE_CODEC_##_file_state_; \
    } else { \
        err = AM_ERR_FAIL_1ST; \
    } \
    return err; \
}

static int32_t am_audio_play(bt_sink_srv_am_id_t aud_id) {
    // _AM_AUDIO_PROCESS_(play,aud_id,PLAY);
    int32_t err = 0;
    if (aud_id == g_prCurrent_player->aud_id) {
        g_am_audio_media_handle->play(g_am_audio_media_handle);
        g_am_file_state = FILE_CODEC_PLAY;
    } else {
        err = AM_ERR_FAIL_1ST;
    }
    return err;
}

static int32_t am_audio_pause(bt_sink_srv_am_id_t aud_id) {
    _AM_AUDIO_PROCESS_(pause,aud_id,PAUSE);
}

static int32_t am_audio_resume(bt_sink_srv_am_id_t aud_id) {
    _AM_AUDIO_PROCESS_(resume,aud_id,PLAY);
}

static int32_t am_audio_stop(bt_sink_srv_am_id_t aud_id) {
    _AM_AUDIO_PROCESS_(stop,aud_id,STOP);
}

#undef _AM_AUDIO_PROCESS_

static int32_t am_audio_close_codec(bt_sink_srv_am_id_t aud_id)
{
    int32_t err = 0;

    if (aud_id == g_prCurrent_player->aud_id) {
        audio_codec_wav_codec_close(g_am_audio_media_handle);
    } else {
        err = AM_ERR_FAIL_1ST;
        #ifdef __AM_DEBUG_INFO__
        audio_src_srv_report("[Sink][AM]audio_close_codec--err: %d, c_aid: %d, aid: %d",3,
            err, g_prCurrent_player->aud_id, aud_id);
        #endif
    }

    return err;
}

#endif /*__AUDIO_COMMON_CODEC_ENABLE__*/

void bt_sink_srv_am_set_volume_change(bool enable)
{
    g_am_volume_enable = enable;
    audio_src_srv_report("[AudM]set_volume_change-enalbe: %d\n", 1, enable);
}


#ifdef __BT_SINK_SRV_ACF_MODE_SUPPORT__

extern int32_t audio_update_iir_design(const uint32_t *parameter);

/*
    A2DP ACF_MODE1 index 0
    LOCAL PLAYER(MP3) ACF_MODE2 index 1
*/

void bt_sink_srv_set_acf_mode(uint8_t mode)
{
    int32_t i = 0, j = 0, m = 0, n = 0, k = 0;
    const audio_eaps_t *am_speech_eaps = audio_nvdm_get_global_eaps_address();;
    int32_t ret = 0;

    //am_speech_eaps = (audio_eaps_t *)pvPortMalloc(sizeof(audio_eaps_t));

    if (am_speech_eaps) {
        //audio_nvdm_get_eaps_data_by_memcpy(am_speech_eaps);
        uint32_t g_sink_srv_audio_iir_filter_array[AMOUNT_OF_AUDIO_FILTERS * EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE];

        m = AMOUNT_OF_AUDIO_FILTERS;
        n = EAPS_AUDIO_IIR_FILTER_PARAMETER_SIZE;
        k = 0;
        for (i = 0; i < m; ++i) {
            for (j = 0; j < n; ++j) {
                g_sink_srv_audio_iir_filter_array[k++] = am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[j];
            }
            audio_src_srv_report("[Sink][AM]acf--i: %d--0x%x, 0x%x, 0x%x, 0x%x, 0x%x",6, i,
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[0],
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[1],
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[2],
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[3],
                am_speech_eaps->audio_parameter.audio_post_processing_parameter[mode].audio_post_processing_compensation_filter[i].audio_iir_design_parameter[4]);
        }

        audio_update_iir_design((const uint32_t *)g_sink_srv_audio_iir_filter_array);
    } else {
        ret = AM_ERR_FAIL_1ST;
    }

    //if (am_speech_eaps != NULL) {
    //    vPortFree(am_speech_eaps);
    //}

    audio_src_srv_report("[Sink][AM]acf--ret: %d, mode: %d, k: %d",3, ret, mode, k);
    _UNUSED(ret);
}
#endif


#ifdef __BT_SINK_SRV_AUDIO_SETTING_SUPPORT__
void bt_sink_srv_am_set_volume(bt_sink_srv_am_stream_type_t in_out, bt_sink_srv_audio_setting_vol_info_t *vol_info)
{
#ifdef MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_ENABLE

uint32_t digital_gain, analog_gain;

if(in_out == STREAM_OUT) {

    vol_type_t type = vol_info->type;
    uint32_t level = AUD_VOL_OUT_LEVEL10;
    if(type == VOL_ANC){
        type = VOL_HFP;
    }
    if(g_prCurrent_player!=NULL){
        level = g_prCurrent_player->audio_stream_out.audio_volume;
    }

    bt_sink_srv_am_result_t ami_result = ami_get_stream_out_volume(type, level, &digital_gain, &analog_gain);

    if(ami_result != AUD_EXECUTION_SUCCESS) {
        audio_src_srv_report("[Sink][AM]bt_sink_srv_am_set_volume: ami_get_stream_out_volume fail!", 0);
        return;
    } else {
        hal_audio_set_stream_out_volume(digital_gain, analog_gain);
        return;
    }
}

#endif

    uint32_t analog = 0;
    bt_sink_srv_audio_setting_vol_t vol;
    vol_type_t type;
    uint32_t digital = 0;
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
    bt_sink_audio_setting_multi_vol_config_t *multi_vol_config=NULL;
    multi_vol_config = (bt_sink_audio_setting_multi_vol_config_t *)pvPortMalloc(sizeof(bt_sink_audio_setting_multi_vol_config_t));
    if (multi_vol_config == NULL) {
        audio_src_srv_err("[Sink][Setting] multi_vol_config malloc error", 0);
        configASSERT(0);
        return;
    }
#endif
#else
    #if defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
        uint32_t digital_Ref1 = 0;
        uint32_t digital_Ref2 = 0;
        uint32_t digital_Ref3 = 0;
        uint32_t digital_EchoREF = 0;
        uint32_t analog_R = 0;
    #endif
#endif

    vol_type_t remember_type = VOL_A2DP;
    if (vol_info->type == VOL_LINE_IN_DL3)
    {
        remember_type = VOL_LINE_IN_DL3;
        vol_info->type = VOL_LINE_IN;
    }

    memset(&vol, 0, sizeof(bt_sink_srv_audio_setting_vol_t));
    bt_sink_srv_audio_setting_get_vol(vol_info, &vol);
    type = vol.type;
    switch (type) {
        case VOL_A2DP: {
            digital = vol.vol.a2dp_vol.vol.digital;
            analog = vol.vol.a2dp_vol.vol.analog_L;
            break;
        }

        case VOL_USB_AUDIO_IN: {
            digital = vol.vol.usb_audio_vol.vol.digital;
            analog = vol.vol.usb_audio_vol.vol.analog_L;
            break;
        }

        case VOL_ANC:
        case VOL_HFP: {
            if (STREAM_IN == in_out) {
                analog   = vol.vol.hfp_vol.vol_in.analog_L;
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
                /*when enable MTK_AUDIO_GAIN_SETTING_ENHANCE feature option*/
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
                multi_vol_config->digital_MIC0_L = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC0_L_Digital_Vol;
                multi_vol_config->digital_MIC0_R = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC0_R_Digital_Vol;
                multi_vol_config->digital_MIC1_L = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC1_L_Digital_Vol;
                multi_vol_config->digital_MIC1_R = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC1_R_Digital_Vol;
                multi_vol_config->digital_MIC2_L = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC2_L_Digital_Vol;
                multi_vol_config->digital_MIC2_R = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC2_R_Digital_Vol;
                multi_vol_config->digital_I2S0_L = vol.vol.hfp_vol.vol_multiMIC_in.digital_I2S0_L_Digital_Vol;
                multi_vol_config->digital_I2S0_R = vol.vol.hfp_vol.vol_multiMIC_in.digital_I2S0_R_Digital_Vol;
                multi_vol_config->digital_I2S1_L = vol.vol.hfp_vol.vol_multiMIC_in.digital_I2S1_L_Digital_Vol;
                multi_vol_config->digital_I2S1_R = vol.vol.hfp_vol.vol_multiMIC_in.digital_I2S1_R_Digital_Vol;
                multi_vol_config->digital_I2S2_L = vol.vol.hfp_vol.vol_multiMIC_in.digital_I2S2_L_Digital_Vol;
                multi_vol_config->digital_I2S2_R = vol.vol.hfp_vol.vol_multiMIC_in.digital_I2S2_R_Digital_Vol;
                multi_vol_config->digital_LINEIN_L = vol.vol.hfp_vol.vol_multiMIC_in.digital_LINEIN_L_Digital_Vol;
                multi_vol_config->digital_LINEIN_R = vol.vol.hfp_vol.vol_multiMIC_in.digital_LINEIN_R_Digital_Vol;
                multi_vol_config->digital_Echo     = vol.vol.hfp_vol.vol_multiMIC_in.digital_Echo_Reference_Vol;
                multi_vol_config->digital_MIC0_L_func = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC0_L_Func_Digital_Vol;
                multi_vol_config->digital_MIC0_R_func = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC0_R_Func_Digital_Vol;
                multi_vol_config->digital_MIC1_L_func = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC1_L_Func_Digital_Vol;
                multi_vol_config->digital_MIC1_R_func = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC1_R_Func_Digital_Vol;
                multi_vol_config->digital_MIC2_L_func = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC2_L_Func_Digital_Vol;
                multi_vol_config->digital_MIC2_R_func = vol.vol.hfp_vol.vol_multiMIC_in.digital_MIC2_R_Func_Digital_Vol;
                multi_vol_config->digital_Echo_func = vol.vol.hfp_vol.vol_multiMIC_in.digital_Echo_Func_Reference_Vol;
                multi_vol_config->analog_R = vol.vol.hfp_vol.vol_in.analog_R;
                multi_vol_config->analog_MIC2 = vol.vol.hfp_vol.vol_in.analog_MIC2;
                multi_vol_config->analog_MIC3 = vol.vol.hfp_vol.vol_in.analog_MIC3;
                multi_vol_config->analog_MIC4 = vol.vol.hfp_vol.vol_in.analog_MIC4;
                multi_vol_config->analog_MIC5 = vol.vol.hfp_vol.vol_in.analog_MIC5;

#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
                bt_sink_srv_audio_setting_detach_mic_gain_config(&analog, &digital, multi_vol_config);
#endif
#else
                digital = vol.vol.hfp_vol.vol_in.digital;
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
                bt_sink_srv_audio_setting_detach_mic_gain_config(&analog, &digital, NULL);
#endif
#endif
#else
                digital = vol.vol.hfp_vol.vol_in.digital;
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
                digital_Ref1    = vol.vol.hfp_vol.vol_multiMIC_in.digital_Ref1;
                digital_Ref2    = vol.vol.hfp_vol.vol_multiMIC_in.digital_Ref2;
                digital_Ref3    = vol.vol.hfp_vol.vol_multiMIC_in.digital_RESERVE;
                digital_EchoREF = vol.vol.hfp_vol.vol_multiMIC_in.digital_Echo;
                analog_R = vol.vol.hfp_vol.vol_in.analog_R;
#endif
#endif
            } else if (STREAM_OUT == in_out) {
                analog = vol.vol.hfp_vol.vol_out.analog_L;
                digital = vol.vol.hfp_vol.vol_out.digital;
            }
            break;
        }

        case VOL_PCM: {
            digital = vol.vol.pcm_vol.vol.digital;
            analog = vol.vol.pcm_vol.vol.analog_L;
            break;
        }

        case VOL_MP3: {
            digital = vol.vol.mp3_vol.vol.digital;
            analog = vol.vol.mp3_vol.vol.analog_L;
            break;
        }

        case VOL_DEF: {
            digital = vol.vol.def_vol.vol.digital;
            analog = vol.vol.def_vol.vol.analog_L;
            break;
        }

        case VOL_VC: {
            analog  = vol.vol.vc_vol.vol.analog_L;
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
            /*when enable MTK_AUDIO_GAIN_SETTING_ENHANCE feature option*/
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
            multi_vol_config->digital_MIC0_L = vol.vol.vc_vol.vol_multiMIC.digital_MIC0_L_Digital_Vol;
            multi_vol_config->digital_MIC0_R = vol.vol.vc_vol.vol_multiMIC.digital_MIC0_R_Digital_Vol;
            multi_vol_config->digital_MIC1_L = vol.vol.vc_vol.vol_multiMIC.digital_MIC1_L_Digital_Vol;
            multi_vol_config->digital_MIC1_R = vol.vol.vc_vol.vol_multiMIC.digital_MIC1_R_Digital_Vol;
            multi_vol_config->digital_MIC2_L = vol.vol.vc_vol.vol_multiMIC.digital_MIC2_L_Digital_Vol;
            multi_vol_config->digital_MIC2_R = vol.vol.vc_vol.vol_multiMIC.digital_MIC2_R_Digital_Vol;
            multi_vol_config->digital_I2S0_L = vol.vol.vc_vol.vol_multiMIC.digital_I2S0_L_Digital_Vol;
            multi_vol_config->digital_I2S0_R = vol.vol.vc_vol.vol_multiMIC.digital_I2S0_R_Digital_Vol;
            multi_vol_config->digital_I2S1_L = vol.vol.vc_vol.vol_multiMIC.digital_I2S1_L_Digital_Vol;
            multi_vol_config->digital_I2S1_R = vol.vol.vc_vol.vol_multiMIC.digital_I2S1_R_Digital_Vol;
            multi_vol_config->digital_I2S2_L = vol.vol.vc_vol.vol_multiMIC.digital_I2S2_L_Digital_Vol;
            multi_vol_config->digital_I2S2_R = vol.vol.vc_vol.vol_multiMIC.digital_I2S2_R_Digital_Vol;
            multi_vol_config->digital_LINEIN_L = vol.vol.vc_vol.vol_multiMIC.digital_LINEIN_L_Digital_Vol;
            multi_vol_config->digital_LINEIN_R = vol.vol.vc_vol.vol_multiMIC.digital_LINEIN_R_Digital_Vol;
            multi_vol_config->digital_Echo     = vol.vol.vc_vol.vol_multiMIC.digital_Echo_Reference_Vol;
            multi_vol_config->digital_MIC0_L_func = vol.vol.vc_vol.vol_multiMIC.digital_MIC0_L_Func_Digital_Vol;
            multi_vol_config->digital_MIC0_R_func = vol.vol.vc_vol.vol_multiMIC.digital_MIC0_R_Func_Digital_Vol;
            multi_vol_config->digital_MIC1_L_func = vol.vol.vc_vol.vol_multiMIC.digital_MIC1_L_Func_Digital_Vol;
            multi_vol_config->digital_MIC1_R_func = vol.vol.vc_vol.vol_multiMIC.digital_MIC1_R_Func_Digital_Vol;
            multi_vol_config->digital_MIC2_L_func = vol.vol.vc_vol.vol_multiMIC.digital_MIC2_L_Func_Digital_Vol;
            multi_vol_config->digital_MIC2_R_func = vol.vol.vc_vol.vol_multiMIC.digital_MIC2_R_Func_Digital_Vol;
            multi_vol_config->digital_Echo_func = vol.vol.vc_vol.vol_multiMIC.digital_Echo_Func_Reference_Vol;
            multi_vol_config->analog_R = vol.vol.vc_vol.vol.analog_R;
            multi_vol_config->analog_MIC2 = vol.vol.vc_vol.vol.analog_MIC2;
            multi_vol_config->analog_MIC3 = vol.vol.vc_vol.vol.analog_MIC3;
            multi_vol_config->analog_MIC4 = vol.vol.vc_vol.vol.analog_MIC4;
            multi_vol_config->analog_MIC5 = vol.vol.vc_vol.vol.analog_MIC5;
#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
            bt_sink_srv_audio_setting_detach_mic_gain_config(&analog, &digital, multi_vol_config);
#endif
#else
            digital = vol.vol.vc_vol.vol.digital;

#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
            bt_sink_srv_audio_setting_detach_mic_gain_config(&analog, &digital, NULL);
#endif
#endif
#else
            digital = vol.vol.vc_vol.vol.digital;
#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
            digital_Ref1 = vol.vol.vc_vol.vol_multiMIC.digital_Ref1;
            analog_R     = vol.vol.vc_vol.vol.analog_R;
#endif
#endif
            break;
        }

        case VOL_LINE_IN: {
        #if defined  (MTK_LINE_IN_ENABLE) || (AIR_WIRED_AUDIO_ENABLE)
            if (STREAM_IN == in_out) {
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
                analog   = vol.vol.lineIN_vol.vol_in.analog_L;
                multi_vol_config->analog_R = vol.vol.lineIN_vol.vol_in.analog_R;
#else
                analog = vol.vol.lineIN_vol.vol_in.analog_L;
#endif
                digital = vol.vol.lineIN_vol.vol_in.digital;
#else
                digital = vol.vol.lineIN_vol.vol_in.digital;
                analog = vol.vol.lineIN_vol.vol_in.analog_L;
#endif
            } else if (STREAM_OUT == in_out) {
                digital = vol.vol.lineIN_vol.vol_out.digital;
                analog = vol.vol.lineIN_vol.vol_out.analog_L;
            }
        #endif
            break;
        }

        default:
            break;
    }

    if (g_am_volume_enable) {
        ;
    } else {
        if (STREAM_IN == in_out) {
    #if !defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
        hal_audio_set_stream_in_volume(digital,analog);
        audio_src_srv_report("[Sink][Setting]AM set vol-- analog gain = %d,digital_MIC0_L gain = %d", 2, analog,digital);
    #else
        #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
            if(((type == VOL_HFP) && (((audio_nvdm_HW_config.Voice_InputDev & 0xC0) >> 6) == 0x03)) || (type == VOL_VC) || (type == VOL_ANC)){
        #else
            if(((type == VOL_HFP) && ((audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x04) || (audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x03) || (audio_nvdm_HW_config.voice_scenario.Voice_Input_Path == 0x02))) || (type == VOL_VC) || (type == VOL_ANC)){
        #endif
                /*digital gain setting to DSP side*/
                if(type != VOL_ANC){
                #ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
                    if (type == VOL_HFP) {
                        /*multi mic gain*/
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC0_L, multi_vol_config->digital_MIC0_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D0_D1);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC1_L, multi_vol_config->digital_MIC1_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D2_D3);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC2_L, multi_vol_config->digital_MIC2_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D4_D5);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_I2S0_L, multi_vol_config->digital_I2S0_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D6_D7);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_I2S1_L, multi_vol_config->digital_I2S1_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D8_D9);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_I2S2_L, multi_vol_config->digital_I2S2_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D10_D11);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_LINEIN_L, multi_vol_config->digital_LINEIN_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D12_D13);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_Echo, 0x7FFF, HAL_AUDIO_INPUT_GAIN_SELECTION_D14);
                        /*mic functions gain*/
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC0_L_func, multi_vol_config->digital_MIC0_R_func, HAL_AUDIO_INPUT_GAIN_SELECTION_D18_19);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC1_L_func, multi_vol_config->digital_MIC1_R_func, HAL_AUDIO_INPUT_GAIN_SELECTION_D20_21);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC2_L_func, multi_vol_config->digital_MIC2_R_func, HAL_AUDIO_INPUT_GAIN_SELECTION_D22_23);
                        hal_audio_set_stream_in_volume_for_multiple_microphone_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_Echo_func, 0x7FFF, HAL_AUDIO_INPUT_GAIN_SELECTION_D24_25);
                    } else {
                        /*multi mic gain*/
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_MIC0_L, multi_vol_config->digital_MIC0_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D0_D1);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_MIC1_L, multi_vol_config->digital_MIC1_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D2_D3);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_MIC2_L, multi_vol_config->digital_MIC2_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D4_D5);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_I2S0_L, multi_vol_config->digital_I2S0_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D6_D7);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_I2S1_L, multi_vol_config->digital_I2S1_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D8_D9);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_I2S2_L, multi_vol_config->digital_I2S2_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D10_D11);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_LINEIN_L, multi_vol_config->digital_LINEIN_R, HAL_AUDIO_INPUT_GAIN_SELECTION_D12_D13);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_Echo, 0x7FFF, HAL_AUDIO_INPUT_GAIN_SELECTION_D14);
                        /*mic functions gain*/
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_MIC0_L_func, multi_vol_config->digital_MIC0_R_func, HAL_AUDIO_INPUT_GAIN_SELECTION_D18_19);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_MIC1_L_func, multi_vol_config->digital_MIC1_R_func, HAL_AUDIO_INPUT_GAIN_SELECTION_D20_21);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_MIC2_L_func, multi_vol_config->digital_MIC2_R_func, HAL_AUDIO_INPUT_GAIN_SELECTION_D22_23);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->digital_Echo_func, 0x7FFF, HAL_AUDIO_INPUT_GAIN_SELECTION_D24_25);
                    }
                    /*debug log*/
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_MIC0_L gain = %d,digital_MIC0_R gain = %d", 2, multi_vol_config->digital_MIC0_L, multi_vol_config->digital_MIC0_R);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_MIC1_L gain = %d,digital_MIC1_R gain = %d", 2, multi_vol_config->digital_MIC1_L, multi_vol_config->digital_MIC1_R);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_MIC2_L gain = %d,digital_MIC2_R gain = %d", 2, multi_vol_config->digital_MIC2_L, multi_vol_config->digital_MIC2_R);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_I2S0_L gain = %d,digital_I2S0_R gain = %d", 2, multi_vol_config->digital_I2S0_L, multi_vol_config->digital_I2S0_R);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_I2S1_L gain = %d,digital_I2S1_R gain = %d", 2, multi_vol_config->digital_I2S1_L, multi_vol_config->digital_I2S1_R);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_I2S2_L gain = %d,digital_I2S2_R gain = %d", 2, multi_vol_config->digital_I2S2_L, multi_vol_config->digital_I2S2_R);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_LINEIN_L gain = %d,digital_LINEIN_R gain = %d", 2, multi_vol_config->digital_LINEIN_L, multi_vol_config->digital_LINEIN_R);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_Echo gain = %d", 1, multi_vol_config->digital_Echo);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_MIC0_L_func gain = %d,digital_MIC0_R_func gain = %d", 2, multi_vol_config->digital_MIC0_L_func, multi_vol_config->digital_MIC0_R_func);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_MIC1_L_func gain = %d,digital_MIC1_R_func gain = %d", 2, multi_vol_config->digital_MIC1_L_func, multi_vol_config->digital_MIC1_R_func);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_MIC2_L_func gain = %d,digital_MIC2_R_func gain = %d", 2, multi_vol_config->digital_MIC2_L_func, multi_vol_config->digital_MIC2_R_func);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP multi mic digital_Echo_func gain = %d", 1, multi_vol_config->digital_Echo_func);
#else
                    hal_audio_set_stream_in_volume_for_multiple_microphone(digital, digital_Ref1, HAL_AUDIO_INPUT_GAIN_SELECTION_D0_D1);
                    if(type == VOL_HFP){
                        hal_audio_set_stream_in_volume_for_multiple_microphone(digital_Ref2, digital_Ref3, HAL_AUDIO_INPUT_GAIN_SELECTION_D2_D3);
                        hal_audio_set_stream_in_volume_for_multiple_microphone(digital_EchoREF, digital_EchoREF, HAL_AUDIO_INPUT_GAIN_SELECTION_D4);
                    }
                #endif
                }

                /*analog gain setting to DSP side*/
                if (type != VOL_ANC) {
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
                    hal_audio_set_stream_in_volume_for_multiple_microphone(analog, multi_vol_config->analog_R, HAL_AUDIO_INPUT_GAIN_SELECTION_A0_A1);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- analog gain = %d,analog_R gain = %d", 2, analog, multi_vol_config->analog_R);
                    hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->analog_MIC2, multi_vol_config->analog_MIC3, HAL_AUDIO_INPUT_GAIN_SELECTION_A2_A3);
                    hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->analog_MIC4, multi_vol_config->analog_MIC5, HAL_AUDIO_INPUT_GAIN_SELECTION_A4_A5);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- analog_MIC2 gain = %d,analog_MIC3 gain = %d,analog_MIC4 gain = %d,analog_MIC5 gain = %d", 4, multi_vol_config->analog_MIC2, multi_vol_config->analog_MIC3, multi_vol_config->analog_MIC4, multi_vol_config->analog_MIC5);
#else
                    hal_audio_set_stream_in_volume_for_multiple_microphone(analog, analog_R, HAL_AUDIO_INPUT_GAIN_SELECTION_A0_A1);
#endif
                } else {
                    mcu2dsp_open_stream_in_param_t  in_device;
                    memset(&in_device,0,sizeof(in_device));
                    hal_audio_get_stream_in_setting_config (AU_DSP_ANC, &in_device);
                    uint32_t interface_mask = 0;
#ifdef ENABLE_2A2D_TEST
                    interface_mask = in_device.afe.audio_interface | in_device.afe.audio_interface1 | in_device.afe.audio_interface2 | in_device.afe.audio_interface3;
#else
                    interface_mask = in_device.afe.audio_interface;
#endif
                    if (interface_mask & HAL_AUDIO_INTERFACE_1) {
                        #ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
                            hal_audio_set_stream_in_volume_for_multiple_microphone(analog, multi_vol_config->analog_R, HAL_AUDIO_INPUT_GAIN_SELECTION_A0_A1);
                            audio_src_srv_report("[Sink][Setting]AM set vol-- analog gain = %d,analog_R gain = %d", 2, analog, multi_vol_config->analog_R);
                        #else
                            hal_audio_set_stream_in_volume_for_multiple_microphone(analog, analog_R, HAL_AUDIO_INPUT_GAIN_SELECTION_A0_A1);
                            audio_src_srv_report("[Sink][Setting]AM set vol-- analog gain = %d,analog_R gain = %d", 2, analog, analog_R);
                        #endif
                    }
#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
                    if (interface_mask & HAL_AUDIO_INTERFACE_2) {
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->analog_MIC2, multi_vol_config->analog_MIC3, HAL_AUDIO_INPUT_GAIN_SELECTION_A2_A3);
                    }
                    if (interface_mask & HAL_AUDIO_INTERFACE_3) {
                        hal_audio_set_stream_in_volume_for_multiple_microphone(multi_vol_config->analog_MIC4, multi_vol_config->analog_MIC5, HAL_AUDIO_INPUT_GAIN_SELECTION_A4_A5);
                    }
                    audio_src_srv_report("[Sink][Setting]AM set vol-- analog_MIC2 gain = %d,analog_MIC3 gain = %d,analog_MIC4 gain = %d,analog_MIC5 gain = %d", 4, multi_vol_config->analog_MIC2, multi_vol_config->analog_MIC3, multi_vol_config->analog_MIC4, multi_vol_config->analog_MIC5);
#endif
                }
            } else {
            #ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
                if (type == VOL_LINE_IN) {
                    /*Line in L R analog gain*/
                    hal_audio_set_stream_in_volume_for_multiple_microphone(analog, multi_vol_config->analog_R, HAL_AUDIO_INPUT_GAIN_SELECTION_A0_A1);
                    hal_audio_set_stream_in_volume_for_multiple_microphone(digital, 0x7FFF, HAL_AUDIO_INPUT_GAIN_SELECTION_D0_D1);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- Line in analog gain = %d,analog_R gain = %d", 2, analog, multi_vol_config->analog_R);
                } else if (type == VOL_HFP) {
                    #ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
                    audio_src_srv_report("[Sink][Setting]hal_audio_query_voice_mic_type() = %d", 1, hal_audio_query_voice_mic_type());
                    if (hal_audio_query_voice_mic_type() == VOICE_MIC_TYPE_DETACHABLE) {
                        hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC0_L, analog);
                        audio_src_srv_report("[Sink][Setting]AM set vol-- detachable MIC analog gain = %d,digital_MIC0_L gain = %d", 2, analog, multi_vol_config->digital_MIC0_L);

                        #ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
                        #ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
                        vPortFree(multi_vol_config);
                        #endif
                        #endif
                        return;
                    }
                    #endif
                    /*HFP with 1 mic HWIO config*/
                    switch(audio_nvdm_HW_config.voice_scenario.Voice_Input_Path) {
                        case 0x00: //0x0: Analog Mic
                            switch(audio_nvdm_HW_config.voice_scenario.Voice_Analog_MIC_Sel) {
                                case 0x00: //mic 0
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC0_L, analog);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one mic0 analog gain = %d,digital gain = %d", 2, analog, multi_vol_config->digital_MIC0_L);
                                    break;
                                case 0x01: //mic 1
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC0_R, multi_vol_config->analog_R);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one mic1 analog gain = %d,digital gain = %d", 2, multi_vol_config->analog_R, multi_vol_config->digital_MIC0_R);
                                    break;
                                case 0x02: //mic 2
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC1_L, multi_vol_config->analog_MIC2);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one mic2 analog gain = %d,digital gain = %d", 2, multi_vol_config->analog_MIC2, multi_vol_config->digital_MIC1_L);
                                    break;
                                case 0x03: //mic 3
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC1_R, multi_vol_config->analog_MIC3);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one mic3 analog gain = %d,digital gain = %d", 2, multi_vol_config->analog_MIC3, multi_vol_config->digital_MIC1_R);
                                    break;
                                case 0x04: //mic 4
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC2_L, multi_vol_config->analog_MIC4);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one mic4 analog gain = %d,digital gain = %d", 2, multi_vol_config->analog_MIC4, multi_vol_config->digital_MIC2_L);
                                    break;
                                case 0x05: //mic 5
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC2_R, multi_vol_config->analog_MIC5);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one mic5 analog gain = %d,digital gain = %d", 2, multi_vol_config->analog_MIC5, multi_vol_config->digital_MIC2_R);
                                    break;
                            }
                            break;
                        case 0x01: //0x1: Digital Mic
                            switch(audio_nvdm_HW_config.voice_scenario.Voice_Digital_MIC_Sel) {
                                case 0x08: //mic 0
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC0_L, analog);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one mic8 analog gain = %d,digital gain = %d", 2, analog, multi_vol_config->digital_MIC0_L);
                                    break;
                                case 0x09: //mic 1
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC0_R, analog);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one mic9 analog gain = %d,digital gain = %d", 2, analog, multi_vol_config->digital_MIC0_R);
                                    break;
                                case 0x0A: //mic 2
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC1_L, analog);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one micA analog gain = %d,digital gain = %d", 2, analog, multi_vol_config->digital_MIC1_L);
                                    break;
                                case 0x0B: //mic 3
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC1_R, analog);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one micB analog gain = %d,digital gain = %d", 2, analog, multi_vol_config->digital_MIC1_R);
                                    break;
                                case 0x0C: //mic 4
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC2_L, analog);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one micC analog gain = %d,digital gain = %d", 2, analog, multi_vol_config->digital_MIC2_L);
                                    break;
                                case 0x0D: //mic 5
                                    hal_audio_set_stream_in_volume_by_scenario(HAL_AUDIO_STREAM_IN_SCENARIO_HFP, multi_vol_config->digital_MIC2_R, analog);
                                    audio_src_srv_report("[Sink][Setting]AM set vol-- HFP one micD analog gain = %d,digital gain = %d", 2, analog, multi_vol_config->digital_MIC2_R);
                                    break;
                            }
                            break;
                    }
                }else {
                    hal_audio_set_stream_in_volume(multi_vol_config->digital_MIC0_L, analog);
                    audio_src_srv_report("[Sink][Setting]AM set vol-- other analog gain = %d,digital_MIC0_L gain = %d", 2, analog, multi_vol_config->digital_MIC0_L);
                }
            #else
                hal_audio_set_stream_in_volume(digital, analog);
            #endif
            }
    #endif
        } else if (STREAM_OUT == in_out) {
            if(remember_type == VOL_LINE_IN_DL3){
                #ifdef AIR_WIRED_AUDIO_ENABLE
                hal_audio_set_stream_out_dl3_volume(digital, analog);
                #endif
            }
            else{
                hal_audio_set_stream_out_volume(digital, analog);
            }
        }
    }

#ifdef MTK_AUDIO_GAIN_SETTING_ENHANCE
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
    vPortFree(multi_vol_config);
#endif
#endif

    audio_src_srv_report("[Sink][Setting]AM set vol--in_out: %d, d: 0x%x, a: 0x%x, type1: %d, type2: %d", 5,
                         in_out, digital, analog, vol_info->type, type);
}

#endif

/**
 * @brief      Init HW I/O config by reading NV key into static table.
 * @return     #HAL_AUDIO_STATUS_OK, if OK. #HAL_AUDIO_STATUS_ERROR, if wrong.
 * @note      This function has SW_Mode and HW_Mode(//TODO).
 */
#if defined(MTK_AVM_DIRECT)
audio_version_t audio_nvdm_check_version(uint32_t HWIO_Configure_Size)
{
    audio_version_t local_version;
    switch(HWIO_Configure_Size){
        case 0x1D:
            local_version = SDK_V1p4;
            break;
        case 0x09:
        default:
            local_version = SDK_V1p3;
            break;
    }
    return local_version;
}

#ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
bt_sink_srv_am_result_t audio_nvdm_configure_init_mapping(audio_version_t FW_version,audio_version_t key_version)
{
    if(FW_version == key_version){
        //No need to mapping
        audio_src_srv_report("[AM]audio_nvdm_configure_init_mapping no need to mapping.version(%d)\n", 1, FW_version);
        return AUD_EXECUTION_SUCCESS;
    }
    if(FW_version == SDK_V1p4){
        switch(key_version){
            case SDK_V1p3:{
            audio_src_srv_report("[AM]audio_nvdm_configure_init_mapping version(%d)(%d)\n", 2, FW_version, key_version);
                mcu2dsp_open_param_t eSCO_open_param;
                memset(&eSCO_open_param,0,sizeof(eSCO_open_param));
                hal_audio_get_stream_out_setting_config(AU_DSP_VOICE, &eSCO_open_param.stream_out_param);
                hal_audio_get_stream_in_setting_config(AU_DSP_VOICE, &eSCO_open_param.stream_in_param);
                if(eSCO_open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_DAC_L){
                    audio_nvdm_HW_config.ANC_OutputDev = 0x00;
                } else if (eSCO_open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_DAC_R){
                    audio_nvdm_HW_config.ANC_OutputDev = 0x10;
                } else if (eSCO_open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_DAC_DUAL){
                    audio_nvdm_HW_config.ANC_OutputDev = 0x20;
                } else {
                    audio_nvdm_HW_config.ANC_OutputDev = 0x00;
                }
                if((eSCO_open_param.stream_in_param.afe.audio_device & HAL_AUDIO_DEVICE_MAIN_MIC_DUAL) != 0){
                        audio_nvdm_HW_config.MIC_Select_ANC_FF = 0x00;
                        audio_nvdm_HW_config.MIC_Select_ANC_FB = 0x01;
                    if(eSCO_open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_L){
                        audio_nvdm_HW_config.MIC_Select_Record_Main = 0x00;
                        audio_nvdm_HW_config.MIC_Select_Record_Ref  = 0xff;
                    } else if(eSCO_open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_MAIN_MIC_R){
                        audio_nvdm_HW_config.MIC_Select_Record_Main = 0x01;
                        audio_nvdm_HW_config.MIC_Select_Record_Ref  = 0xff;
                    } else {
                        audio_nvdm_HW_config.MIC_Select_Record_Main = 0x00;
                        audio_nvdm_HW_config.MIC_Select_Record_Ref = 0x01;
                    }
                }else if((eSCO_open_param.stream_in_param.afe.audio_device & HAL_AUDIO_DEVICE_DIGITAL_MIC_DUAL) != 0){
                        audio_nvdm_HW_config.MIC_Select_ANC_FF = 0x02;
                        audio_nvdm_HW_config.MIC_Select_ANC_FB = 0x03;
                    if(eSCO_open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_DIGITAL_MIC_L){
                        audio_nvdm_HW_config.MIC_Select_Record_Main = 0x02;
                        audio_nvdm_HW_config.MIC_Select_Record_Ref  = 0xff;
                    } else if(eSCO_open_param.stream_in_param.afe.audio_device == HAL_AUDIO_DEVICE_DIGITAL_MIC_R){
                        audio_nvdm_HW_config.MIC_Select_Record_Main = 0x03;
                        audio_nvdm_HW_config.MIC_Select_Record_Ref  = 0xff;
                    } else {
                        audio_nvdm_HW_config.MIC_Select_Record_Main = 0x02;
                        audio_nvdm_HW_config.MIC_Select_Record_Ref  = 0x03;
                    }
                }
            }
            default:
                break;
        }
    }
    return AUD_EXECUTION_SUCCESS;
}
#endif

bt_sink_srv_am_result_t audio_nvdm_configure_init(void)
{
    bt_sink_srv_am_result_t status = AUD_EXECUTION_SUCCESS;
    nvdm_status_t nvdm_status = NVDM_STATUS_OK;
    uint32_t tableSize = 0;

    if (g_audio_nvdm_init_flg == true) {
        return AUD_EXECUTION_SUCCESS;
    }

    //Audio HW Configure..  /*HFP sidetone, gain and AMIC DCC, ACC*/
    nvdm_status = nvkey_data_item_length(NVKEYID_APP_AUDIO_HW_IO_CONFIGURE, &tableSize);
    if(nvdm_status || !tableSize){
        audio_src_srv_report("[AM][Warning]NVDM HWIO_config_init Fail. Status:%d Len:%u\n", 2, nvdm_status, tableSize);
        status = AUD_EXECUTION_FAIL;
    }
    nvdm_version = audio_nvdm_check_version(tableSize);
    if(nvdm_version == SDK_NONE){
        audio_src_srv_report("[AM][ERROR]NVDM HWIO_config_init SDK Version fail.\n",0);
        audio_src_srv_report("NVDM HWIO_config_init SDK Version not match.", 0);
        configASSERT(0);
    }
    if(tableSize != sizeof(audio_nvdm_HW_config)){
        audio_src_srv_report("[AM][Warning]NVDM HWIO_config_init size not match. NVDM tableSize (%u) != (%d)\n", 2, tableSize, sizeof(audio_nvdm_HW_config));
        status = AUD_EXECUTION_FAIL;
    }
    nvdm_status = nvkey_read_data(NVKEYID_APP_AUDIO_HW_IO_CONFIGURE, (uint8_t *)&audio_nvdm_HW_config, &tableSize);
    if(nvdm_status){
        audio_src_srv_report("[AM][Warning]NVDM HWIO_config_init Fail 2. Status:%d pNvdmMp3Vol:0x%x\n", 2, nvdm_status, (unsigned int)&audio_nvdm_HW_config);
        status = AUD_EXECUTION_FAIL;
    }
    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        audio_nvdm_configure_init_mapping(SW_version,nvdm_version);
    #endif

    //Audio Channel Selection from NV key.
    nvdm_status = nvkey_data_item_length(NVKEYID_APP_AUDIO_CHANNEL, &tableSize);
    if(nvdm_status || !tableSize){
        audio_src_srv_report("[AM][Warning]NVDM Channel_selection_init Fail. Status:%u Len:%lu\n", 2, nvdm_status, tableSize);
        status = AUD_EXECUTION_FAIL;
    }
    if(tableSize != sizeof(audio_Channel_Select)){
        audio_src_srv_report("[AM][Warning]NVDM Channel_selection_init size not match. NVDM tableSize (%lu) != (%d)\n", 2, tableSize, sizeof(audio_Channel_Select));
        status = AUD_EXECUTION_FAIL;
    }
    nvdm_status = nvkey_read_data(NVKEYID_APP_AUDIO_CHANNEL, (uint8_t *)&audio_Channel_Select, &tableSize);
    if(nvdm_status){
        audio_src_srv_report("[AM][Warning]NVDM Channel_selection_init Fail 2. Status:%d pNvdmMp3Vol:0x%x\n", 2, nvdm_status, (unsigned int)&audio_Channel_Select);
        status = AUD_EXECUTION_FAIL;
    }
    //AMIC ACC DCC Setting
    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if((audio_nvdm_HW_config.UL_AFE_PARA & 0x02) != 0){
    #else
        if ((audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC0_Mode != 0x02) ||
            (audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC1_Mode != 0x02) ||
            (audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC2_Mode != 0x02) ||
            (audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC3_Mode != 0x02) ||
            (audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC4_Mode != 0x02) ||
            (audio_nvdm_HW_config.adc_dac_config.ADDA_Analog_MIC5_Mode != 0x02)) {
    #endif
        //AMIC ACC mode
        *((volatile uint32_t*)(0xA2120B04)) &= 0xFFFFFF87;
        *((volatile uint32_t*)(0xA2120B04)) |= 0x10;
        audio_src_srv_report("AMIC change to ACC mode", 0);
    }
    //ADC, DAC Configure (NM HP mode)
    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if((audio_nvdm_HW_config.UL_AFE_PARA & 0x01) != 0){
    #else
        if ((audio_nvdm_HW_config.audio_scenario.Audio_Analog_LineIn_Performance_Sel == 0x01) ||
            (audio_nvdm_HW_config.voice_scenario.Voice_Analog_ADC_Performance_Sel == 0x01) ||
            (audio_nvdm_HW_config.record_scenario.Record_Analog_ADC_Performance_Sel == 0x01)) {
    #endif
        //ADC HP
        *((volatile uint32_t*)(0xA2120B04)) |= 0x200;
        audio_src_srv_report("UL ADC change to HP mode", 0);
    } else {
        //ADC NM
        *((volatile uint32_t*)(0xA2120B04)) &= 0xFFFFFDFF;
    }
    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if((audio_nvdm_HW_config.DL_AFE_PARA & 0x01) != 0){
    #else
        if (audio_nvdm_HW_config.adc_dac_config.ADDA_DAC_Class_AB_G_Performance_Sel != 0x00) {
    #endif
        //DAC HP
        *((volatile uint32_t*)(0xA2120B04)) |= 0x100;
        audio_src_srv_report("DL DAC change to HP mode", 0);
    } else {
        //DAC NM
        *((volatile uint32_t*)(0xA2120B04)) &= 0xFFFFFEFF;
    }

    //DVFS Clk Selection from NVKEY
    nvdm_status = nvkey_data_item_length(NVKEYID_AUDIO_DVFS_CLK_SETTING, &tableSize);

    if(nvdm_status || !tableSize){
        audio_src_srv_report("[AM][Warning]NVDM DVFS_CLK_SEL Fail. Status:%d Len:%u\n", 2, nvdm_status, tableSize);
        status = AUD_EXECUTION_FAIL;
    }
    if(tableSize != sizeof(audio_nvdm_dvfs_config)){
        audio_src_srv_report("[AM][Warning]NVDM HWIO_config_init size not match. NVDM tableSize (%u) != (%d)\n", 2, tableSize, sizeof(audio_nvdm_dvfs_config));
        status = AUD_EXECUTION_FAIL;
    }

    nvdm_status = nvkey_read_data(NVKEYID_AUDIO_DVFS_CLK_SETTING, (uint8_t *)&audio_nvdm_dvfs_config, &tableSize);

    if(nvdm_status){
        audio_src_srv_report("[AM][Warning]NVDM DVFS_CLK_SEL Fail 2. Status:%d pNvdmMp3Vol:0x%x\n", 2, nvdm_status, (unsigned int)&audio_nvdm_dvfs_config);
        status = AUD_EXECUTION_FAIL;
    }
    #ifndef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        audio_src_srv_report("HFP Sidetone_EN(%d) HFP_Sidetone_gain(0x%x) G_Sidetone_gain(0x%x) UL_AFE_PARA(0x%x) DL_AFE_PARA(0x%x)", 5, audio_nvdm_HW_config.Voice_Sidetone_EN, audio_nvdm_HW_config.Voice_Sidetone_Gain, audio_nvdm_HW_config.Reserve_Sidetone_Gain, audio_nvdm_HW_config.UL_AFE_PARA, audio_nvdm_HW_config.DL_AFE_PARA);
        audio_src_srv_report("CH_sel mode(%d) sw_channel(%d) hw_H(%d) hw_L(%d) hw_index(%d)", 5, audio_Channel_Select.modeForAudioChannel, audio_Channel_Select.audioChannel, audio_Channel_Select.hwAudioChannel.audioChannelGPIOH, audio_Channel_Select.hwAudioChannel.audioChannelGPIOL, audio_Channel_Select.hwAudioChannel.gpioIndex);
        audio_src_srv_report("HW Config Audio_in device(0x%x) Audio_out device(0x%x) Voice_in device(0x%x) Voice_out device(0x%x)", 4, audio_nvdm_HW_config.Audio_InputDev, audio_nvdm_HW_config.Audio_OutputDev, audio_nvdm_HW_config.Voice_InputDev, audio_nvdm_HW_config.Voice_OutputDev);
        audio_src_srv_report("HW Config Micbias Audio(0x%x) Voice(0x%x) Reserve(0x%x)", 3, audio_nvdm_HW_config.MICBIAS_PARA_Audio, audio_nvdm_HW_config.MICBIAS_PARA_Voice, audio_nvdm_HW_config.MICBIAS_PARA_RESERVE);
        audio_src_srv_report("HW Config I2S Audio(0x%x) Voice(0x%x) Reserve(0x%x)", 3, audio_nvdm_HW_config.I2S_PARA_Audio, audio_nvdm_HW_config.I2S_PARA_Voice, audio_nvdm_HW_config.I2S_PARA_RESERVE);
        audio_src_srv_report("HW Config PARA AMIC_PARA(0x%x) DMIC_PARA(0x%x) LOOPBACK_RATE(0x%x) LOOPBACK_PARA(0x%x)", 4, audio_nvdm_HW_config.AMIC_PARA, audio_nvdm_HW_config.DMIC_PARA, audio_nvdm_HW_config.LOOPBACK_RATE, audio_nvdm_HW_config.LOOPBACK_PARA);
        audio_src_srv_report("HW Config mic MIC_Select_Main(0x%x) MIC_Select_Ref(0x%x) MIC_Select_Ref_2(0x%x) MIC_Select_Ref_3(0x%x)", 4, audio_nvdm_HW_config.MIC_Select_Main, audio_nvdm_HW_config.MIC_Select_Ref, audio_nvdm_HW_config.MIC_Select_Ref_2, audio_nvdm_HW_config.MIC_Select_Ref_3);
        audio_src_srv_report("HW Config mic MIC_Select_ANC_FF(0x%x) MIC_Select_ANC_FB(0x%x) ANC_OutputDev(0x%x) ", 3, audio_nvdm_HW_config.MIC_Select_ANC_FF, audio_nvdm_HW_config.MIC_Select_ANC_FB, audio_nvdm_HW_config.ANC_OutputDev);
        audio_src_srv_report("HFP_DVFS_CLK(0x%x) ", 1, audio_nvdm_dvfs_config.HFP_DVFS_CLK);
    #endif

    /*set audio nvdm read done flg*/
    g_audio_nvdm_init_flg = true;

    return status;
}

#if defined(MTK_AMP_DC_COMPENSATION_ENABLE)
static void sort(unsigned int *p, unsigned int entries)
{
    unsigned int i, j, swap;
    unsigned int swapped = 0;

    if (entries == 0) return;

    for (i=0; i < (entries-1); i++) {
        for (j = 0; j < (entries-1)-i; j++) {
            if (p[j] > p[j+1]) {
                swap = p[j];
                p[j] = p[j+1];
                p[j+1] = swap;
                swapped = 1;
            }
        }
        if (!swapped) {
            break;
        }
    }
}

#define ReadREG(_addr)          (*(volatile uint32_t *)(_addr))
#define AFE_READ(addr)          *((volatile uint32_t *)(addr))
#define AFE_WRITE(addr, val)    *((volatile uint32_t *)(addr)) = val
#define AFE_SET_REG(addr, val, msk)  AFE_WRITE((addr), ((AFE_READ(addr) & (~(msk))) | ((val) & (msk))))
#define setting_count       (100)
#define abandon_head_count  (5)
#define abandon_tail_count  (5)
#define shrinkvalue         (0.125893)

static uint16_t get_dc_compensation_value() {
    bool SignBitL = 0, SignBitR = 0;
    uint8_t count = 0, loop = 4, TrimL = 0, TrimR = 0, FineTrimL = 0, FineTrimR = 0;
    int channel_sel[4]={0x00000071, 0x00000074, 0x00000072, 0x00000073};//Trimming buffer mux selection with trimming buffer gain 18dB
    int SUM_HPLP = 0, SUM_HPLN = 0, SUM_HPRP = 0, SUM_HPRN = 0;
    uint16_t total=0;
#ifdef DC_COMPENSATION_PARA_ENABLE
    double fAVG_HPLP = 0, fAVG_HPLN = 0, fAVG_HPRP = 0, fAVG_HPRN = 0, fValueL = 0, fValueR = 0, TrimStep = 0.18, FineTrimStep = 0.05, fcount = 0;
#else
    double fAVG_HPLP = 0, fAVG_HPLN = 0, fAVG_HPRP = 0, fAVG_HPRN = 0, fValueL = 0, fValueR = 0, TrimStep = 0.15, FineTrimStep = 0.04, fcount = 0;
#endif
    unsigned int **data = (unsigned int **)pvPortMalloc(2 * sizeof(unsigned int *));
    if(data == NULL)
    {
        audio_src_srv_report("DC compensation error, pvPortMalloc **data failed.", 0);
        audio_src_srv_report("DC compensation error, pvPortMalloc **data failed.", 0);
        configASSERT(0);
        return 1;
    }
    for (count = 0; count < 2; count++){

        data[count] = pvPortMalloc(setting_count * sizeof(unsigned int));

        if(data[count] == NULL)
        {
            audio_src_srv_report("DC compensation error, pvPortMalloc data[count] failed.", 0);
            audio_src_srv_report("DC compensation error, pvPortMalloc data[count] failed.", 0);
            vPortFree(data);
            configASSERT(0);
            return 1;
        }
    }

    hal_adc_init();

    //Auxadc measure value, trimming buffer mux selection 0x71:LP, 0x74:LN, 0x72:RP, 0x73:RN
    for (count = 0; count < setting_count; count++) {
        for (loop = 0; loop < 2; loop++) {
#ifdef DC_COMPENSATION_PARA_ENABLE
            AFE_SET_REG (0xA2070118,  channel_sel[loop],  0xffffffff);
#else
            AFE_SET_REG (0xA2070218,  channel_sel[loop],  0xffffffff);
#endif
            ADC->AUXADC_CON1 = 0;
            //hal_gpt_delay_us(10);
            ADC->AUXADC_CON1 = (1<<8);
            // Wait until the module status is idle
            while (ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.ADC_STAT & AUXADC_CON3_ADC_STA_MASK);
#ifdef DC_COMPENSATION_PARA_ENABLE
            //hal_gpt_delay_us(10);
            data[loop][count] = ReadREG(0xA30B0030);
            //printf("Ldata[%d][%d]:0x%x RG(0xA30B0004):0x%x RG(0xA207012C):0x%x\r\n",loop,count,data[loop][count], ReadREG(0xA30B0004),ReadREG(0xA207012C));
#else
            data[loop][count] = ReadREG(0xA0170030);
#endif

        }
    }

    //Abandon critical value and calculate sum of ordered data to get average value
    for (loop = 0; loop < 2; loop++) {
        sort(&data[loop][0], setting_count);
        if (loop == 0) {
            for (count = 0+abandon_head_count; count < setting_count-abandon_tail_count; count++) {
                SUM_HPLP = SUM_HPLP + data[loop][count];
            }
            fAVG_HPLP = (float)SUM_HPLP / (float)(setting_count-abandon_head_count-abandon_tail_count);
        }
        if (loop == 1) {
            for (count = 0+abandon_head_count; count < setting_count-abandon_tail_count; count++) {
                SUM_HPLN = SUM_HPLN + data[loop][count];
            }
            fAVG_HPLN = (float)SUM_HPLN / (float)(setting_count-abandon_head_count-abandon_tail_count);
        }
    }

    //Auxadc measure value, trimming buffer mux selection 0x71:LP, 0x74:LN, 0x72:RP, 0x73:RN
    for (count = 0; count < setting_count; count++) {
        for (loop = 0; loop < 2; loop++) {
#ifdef DC_COMPENSATION_PARA_ENABLE
            AFE_SET_REG (0xA2070118,  channel_sel[loop+2],  0xffffffff);
#else
            AFE_SET_REG (0xA2070218,  channel_sel[loop+2],  0xffffffff);
#endif
            ADC->AUXADC_CON1 = 0;
            //hal_gpt_delay_us(10);
            ADC->AUXADC_CON1 = (1<<8);
            // Wait until the module status is idle
            while (ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.ADC_STAT & AUXADC_CON3_ADC_STA_MASK);
#ifdef DC_COMPENSATION_PARA_ENABLE
            //hal_gpt_delay_us(10);
            data[loop][count] = ReadREG(0xA30B0030);
            //printf("Rdata[%d][%d]:0x%x RG(0xA30B0004):0x%x RG(0xA207012C):0x%x\r\n",loop,count,data[loop][count], ReadREG(0xA30B0004),ReadREG(0xA207012C));
#else
            data[loop][count] = ReadREG(0xA0170030);
#endif


        }
    }

    //Abandon critical value and calculate sum of ordered data to get average value
    for (loop = 0; loop < 2; loop++) {
        sort(&data[loop][0], setting_count);
        if (loop == 0) {
            for (count = 0+abandon_head_count; count < setting_count-abandon_tail_count; count++) {
                SUM_HPRP = SUM_HPRP + data[loop][count];
            }
            fAVG_HPRP = (float)SUM_HPRP / (float)(setting_count-abandon_head_count-abandon_tail_count);
        }
        if (loop == 1) {
            for (count = 0+abandon_head_count; count < setting_count-abandon_tail_count; count++) {
                SUM_HPRN = SUM_HPRN + data[loop][count];
            }
            fAVG_HPRN = (float)SUM_HPRN / (float)(setting_count-abandon_head_count-abandon_tail_count);
        }
    }

    hal_adc_deinit();

    for (count = 0; count < 2; count++){
        vPortFree(data[count]);
    }
    vPortFree(data);

    fValueL = fAVG_HPLP - fAVG_HPLN;
    fValueR = fAVG_HPRP - fAVG_HPRN;
    //printf("fValueL 0x%x fValueR 0x%x fAVG_HPLP 0x%x fAVG_HPLN 0x%x fAVG_HPRP 0x%x fAVG_HPRN 0x%x\r\n",fValueL,fValueR,fAVG_HPLP,fAVG_HPLN,fAVG_HPRP,fAVG_HPRN);
    //printf("fValueL = LP - LN = %f - %f = %f\r\n",fAVG_HPLP,fAVG_HPLN,fValueL);
    //printf("fValueR = RP - RN = %f - %f = %f\r\n",fAVG_HPRP,fAVG_HPRN,fValueR);
    audio_src_srv_report("fValueL = LP - LN = %f - %f = %f", 3,fAVG_HPLP,fAVG_HPLN,fValueL);
    audio_src_srv_report("fValueR = RP - RN = %f - %f = %f", 3,fAVG_HPRP,fAVG_HPRN,fValueR);

    //Auxadc reference voltage from 0~1.4V, express with 12bit
    fValueL = 1400 * fValueL / 4096; // mV (w 18dB)
    fValueR = 1400 * fValueR / 4096; // mV (w 18dB)

    //Without trimming buffer gain 18dB
    fValueL = fValueL * shrinkvalue; // mV (wo 18dB)
    fValueR = fValueR * shrinkvalue; // mV (wo 18dB)

    SignBitL = (fValueL > 0) ? 1 : 0;
    SignBitR = (fValueR > 0) ? 1 : 0;

    if (SignBitL == 1) {
        for (fcount = fValueL; fcount >= TrimStep; fcount -= TrimStep) {
                fValueL -= TrimStep;
                TrimL += 1;
        }
        for (fcount = fValueL; fcount >= FineTrimStep; fcount -= FineTrimStep) {
                fValueL -= FineTrimStep;
                FineTrimL += 1;
        }
    } else {
        for (fcount = fValueL; fcount <= (-TrimStep); fcount += TrimStep) {
                fValueL += TrimStep;
                TrimL += 1;
        }
        for (fcount = fValueL; fcount <= (-FineTrimStep); fcount += FineTrimStep) {
                fValueL += FineTrimStep;
                FineTrimL += 1;
        }
    }

    if(SignBitR == 1) {
        for (fcount = fValueR; fcount >= TrimStep; fcount -= TrimStep) {
                fValueR -= TrimStep;
                TrimR += 1;
        }
        for (fcount = fValueR; fcount >= FineTrimStep; fcount -= FineTrimStep) {
                fValueR -= FineTrimStep;
                FineTrimR += 1;
        }
    } else {
        for (fcount = fValueR; fcount <= (-TrimStep); fcount += TrimStep) {
                fValueR += TrimStep;
                TrimR += 1;
        }
        for (fcount = fValueR; fcount <= (-FineTrimStep); fcount += FineTrimStep) {
                fValueR += FineTrimStep;
                FineTrimR += 1;
        }
    }

    total = SignBitR<<15 | FineTrimR<<13 | SignBitR<<12 | TrimR<<8 | SignBitL<<7 | FineTrimL<<5 | SignBitL<<4 | TrimL<<0;
    //printf("total 0x%x\r\n",total);
    return total;
}
#endif //MTK_AMP_DC_COMPENSATION_ENABLE
#endif //MTK_AVM_DIRECT



void audio_message_audio_handler(hal_audio_event_t event, void *data)
{
    audio_src_srv_report("dsp amp mcu callback :%d", 1, event);
    _UNUSED(data);
    if (event == HAL_AUDIO_EVENT_DATA_REQUEST) {
    #ifdef AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ
        bool set_dl_clkmux = ami_hal_audio_status_query_running_flag(AUDIO_MESSAGE_TYPE_AFE)?false:true;
    #endif
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_AFE, true);
    #ifdef AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ
        if(set_dl_clkmux){
            hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_AFE, HAL_AUDIO_DEVICE_DAC_DUAL, HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE, true);
            audio_src_srv_report("[hi-res]Set AFE count", 0);
        }
    #endif
    } else {
    #ifdef AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ
        hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_AFE, 0, 0, false);
    #endif
        ami_hal_audio_status_set_running_flag(AUDIO_MESSAGE_TYPE_AFE, false);
    }
}

#if defined(AIR_WIRED_AUDIO_ENABLE)
static void audio_transmitter_wired_audio_stop_close_usb_in(uint16_t scenario_and_id){
    if(wired_audio_get_usb_audio_start_number() > 1){
        if (wired_audio_get_usb_use_afe_subid() == (scenario_and_id & 0xff)) {
            /* not close because afe must open */
        }
        else {
            audio_transmitter_playback_stop(scenario_and_id);
            audio_transmitter_playback_close(scenario_and_id);
        }
    }
    else if(wired_audio_get_usb_audio_start_number() == 1) {
        if (wired_audio_get_usb_use_afe_subid() != (scenario_and_id & 0xff)) {
            uint16_t scenario_and_id_0 = (AUDIO_TRANSMITTER_WIRED_AUDIO << 8) + wired_audio_get_usb_use_afe_subid();
            audio_transmitter_playback_stop(scenario_and_id_0);
            audio_transmitter_playback_close(scenario_and_id_0);
            audio_transmitter_playback_stop(scenario_and_id);
            audio_transmitter_playback_close(scenario_and_id);
            wired_audio_set_usb_use_afe_subid(0);
        }
        else {
            audio_transmitter_playback_stop(scenario_and_id);
            audio_transmitter_playback_close(scenario_and_id);
        }
    }
    else{
        audio_src_srv_report("wired audio set stop error, start_number:%d", 1,wired_audio_get_usb_audio_start_number());
        configASSERT(0);
    }
    wired_audio_minus_usb_audio_start_number();
}
#endif /* AIR_WIRED_AUDIO_ENABLE */

#ifdef AIR_BT_AUDIO_SYNC_ENABLE
void audio_request_sync_hdlr(bt_sink_srv_am_amm_struct *amm_ptr)
{
    bt_sink_srv_am_background_t         *background_info = &(amm_ptr->background_info);
    bt_sink_srv_am_id_t                 aud_id           = background_info->aud_id;
    cm4_dsp_audio_sync_request_param_t *info = hal_audio_query_audio_sync_request_param();
    uint32_t enter_cnt = 0; // function enter gpt count
    uint32_t exit_cnt = 0; // function exit gpt count
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &enter_cnt);
    uint16_t sub_id = background_info->sync_parm.sync_scenario_type << 8 | background_info->sync_parm.sync_action_type;
    // convert volume level into digital/analog gain
    bt_sink_srv_audio_setting_vol_info_t vol_info = {0};
    bt_sink_srv_audio_setting_vol_t vol = {0};
    memset(info, 0, sizeof(cm4_dsp_audio_sync_request_param_t)); // init share info
    // check current pseudo device
    if (background_info->sync_parm.sync_action_type == MCU2DSP_SYNC_REQUEST_SET_VOLUME) {
        switch (background_info->sync_parm.sync_scenario_type) // scenario type [HFP/A2DP/ANC/VP]
        {
            case MCU2DSP_SYNC_REQUEST_HFP:
                vol_info.type = VOL_HFP;
                vol_info.vol_info.hfp_vol_info.codec = g_rAm_aud_id[aud_id].contain_ptr->local_context.hfp_format.hfp_codec.type;
                vol_info.vol_info.hfp_vol_info.dev_out = g_rAm_aud_id[aud_id].contain_ptr->audio_stream_out.audio_device;
                g_rAm_aud_id[aud_id].contain_ptr->audio_stream_out.audio_volume = background_info->sync_parm.vol_out.vol_level; // update
                vol_info.vol_info.hfp_vol_info.lev_out = g_rAm_aud_id[aud_id].contain_ptr->audio_stream_out.audio_volume;
                vol_info.vol_info.hfp_vol_info.dev_in = g_rAm_aud_id[aud_id].contain_ptr->audio_stream_in.audio_device;
                vol_info.vol_info.hfp_vol_info.lev_in = g_rAm_aud_id[aud_id].contain_ptr->audio_stream_in.audio_volume;
                bt_sink_srv_audio_setting_get_vol(&vol_info, &vol);
                info->vol_gain_info.gain = vol.vol.hfp_vol.vol_out.digital;
                info->vol_gain_info.gain_select = background_info->sync_parm.vol_out.channel;
                break;
            case MCU2DSP_SYNC_REQUEST_A2DP:
                vol_info.type = VOL_A2DP;
                vol_info.vol_info.a2dp_vol_info.dev = g_rAm_aud_id[aud_id].contain_ptr->audio_stream_out.audio_device;
                g_rAm_aud_id[aud_id].contain_ptr->audio_stream_out.audio_volume = background_info->sync_parm.vol_out.vol_level;
                vol_info.vol_info.a2dp_vol_info.lev = g_rAm_aud_id[aud_id].contain_ptr->audio_stream_out.audio_volume;
                bt_sink_srv_audio_setting_get_vol(&vol_info, &vol);
                info->vol_gain_info.gain = vol.vol.a2dp_vol.vol.digital;
                info->vol_gain_info.gain_select = background_info->sync_parm.vol_out.channel;
                break;
            case MCU2DSP_SYNC_REQUEST_ANC: // differ from others
                // [TODO]

                break;
            case MCU2DSP_SYNC_REQUEST_VP:
                vol_info.type = VOL_VP;
                // VP no need audio_device parameter
                vol_info.vol_info.vp_vol_info.lev = background_info->sync_parm.vol_out.vol_level;
                bt_sink_srv_audio_setting_get_vol(&vol_info, &vol);
                info->vol_gain_info.gain = vol.vol.vp_vol.vol.digital;
                info->vol_gain_info.gain_select = background_info->sync_parm.vol_out.channel;
                break;
            default:
                // error happen
                audio_src_srv_report("[DSP SYNC] am type error [%d]", (uint32_t)background_info->sync_parm.sync_scenario_type);
                return;
                break;
        }
    }

    info->gpt_count = background_info->sync_parm.target_gpt_cnt;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &exit_cnt);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_AUDIO_SYNC_REQUEST, sub_id, (uint32_t)info, true);
    audio_src_srv_report("[DSP SYNC] cm4 send info: type [%d], t_cnt = %u, out_level = %d, channel = %d, out_gain = 0x%x", 5,
                            background_info->sync_parm.sync_scenario_type, info->gpt_count, background_info->sync_parm.vol_out.vol_level,
                            info->vol_gain_info.gain_select, info->vol_gain_info.gain);
    audio_src_srv_report("[DSP SYNC] cm4 send info: enter_cnt = %u, exit_cnt = %u", 2, enter_cnt, exit_cnt);
}
#endif /* AIR_BT_AUDIO_SYNC_ENABLE */
