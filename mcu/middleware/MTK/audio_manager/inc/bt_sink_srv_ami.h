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

#ifndef __AM_INTERFACE_H__
#define __AM_INTERFACE_H__

#if defined(MTK_AUDIO_MP3_ENABLED) || defined(MTK_MP3_DECODER_ENABLED)
//#define __AUDIO_MP3_ENABLE__
#endif

#ifdef MTK_WAV_DECODER_ENABLE
#define __AUDIO_COMMON_CODEC_ENABLE__
#endif

#ifdef MTK_WAV_DECODER_ENABLE
#define __AUDIO_WAV_ENABLE__
#endif

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif


#define __BT_SINK_SRV_ACF_MODE_SUPPORT__
#define MTK_VENDOR_SOUND_EFFECT_ENABLE

#define STANDALONE_TEST 0
//#define WIN32_UT
#ifndef  WIN32_UT
#define RTOS_TIMER
#define HAL_AUDIO_MODULE_ENABLED
#define __AM_DEBUG_INFO__
//#define MTK_EXTERNAL_DSP_NEED_SUPPORT

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
#else
#include <stdlib.h>
#include <corecrt_malloc.h>
#endif

#include "bt_codec.h"
#include "bt_a2dp.h"
#include "bt_hfp.h"
#include "hal_audio.h"
#ifndef MTK_AVM_DIRECT
#include "hal_audio_enhancement.h"
#include "audio_coefficient.h"
#include "audio_nvdm.h"
#endif
#include "bt_type.h"
#include "bt_sink_srv.h"
#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
  #include "anc_control_api.h"
#else
  #include "anc_control.h"
#endif
#endif
#ifdef MTK_RECORD_ENABLE
#include "record_control.h"
#endif
#ifdef MTK_PROMPT_SOUND_ENABLE
#include "prompt_control.h"
#endif

#ifdef __BT_AWS_SUPPORT__
#include "bt_aws.h"
#include "bt_sink_srv_aws.h"
#endif
#ifdef __AUDIO_MP3_ENABLE__
#include "mp3_codec.h"
#endif
#ifdef __AUDIO_COMMON_CODEC_ENABLE__
#include "audio_codec.h"
#endif
//#include "bt_sink_srv_resource.h"
#include "hal_audio_internal.h"
#include "nvdm.h"
#include "bt_sink_srv_audio_setting.h"
#include "nvkey_dspalg.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif
#include "audio_api.h"

#ifndef FALSE
#define FALSE                  0
#endif
#ifndef TRUE
#define TRUE                   1
#endif
#define NOT_REWRITE            0
#define REWRITE                1
#define AM_REGISTER_ID_TOTAL   16
#define DEVICE_IN_LIST        (HAL_AUDIO_DEVICE_MAIN_MIC | HAL_AUDIO_DEVICE_HEADSET_MIC | HAL_AUDIO_DEVICE_LINE_IN | HAL_AUDIO_DEVICE_DUAL_DIGITAL_MIC | HAL_AUDIO_DEVICE_SINGLE_DIGITAL_MIC)
#define DEVICE_OUT_LIST       (HAL_AUDIO_DEVICE_HANDSET | HAL_AUDIO_DEVICE_HANDS_FREE_MONO | HAL_AUDIO_DEVICE_HANDS_FREE_STEREO | HAL_AUDIO_DEVICE_HEADSET | HAL_AUDIO_DEVICE_HEADSET_MONO)
#define DEVICE_LOUDSPEAKER    (HAL_AUDIO_DEVICE_HANDSET | HAL_AUDIO_DEVICE_HANDS_FREE_MONO | HAL_AUDIO_DEVICE_HANDS_FREE_STEREO)
#define DEVICE_EARPHONE       (HAL_AUDIO_DEVICE_HEADSET | HAL_AUDIO_DEVICE_HEADSET_MONO)

#define SPEECH_MODE_HEADSET 0
#define SPEECH_MODE_HANDSFREE 1
#define SPEECH_MODE_HEADSET_DUALMIC 0

#if defined(MTK_PEQ_ENABLE) && defined(MTK_AWS_MCE_ENABLE)
#define SUPPORT_PEQ_NVKEY_UPDATE
#endif
#ifdef MTK_ANC_ENABLE
//#define MTK_POST_PEQ_DEFAULT_ON
//#define MTK_VOICE_ANC_EQ
#endif
/*************************************
*         HAL Struct & Enum
**************************************/
typedef hal_audio_status_t          bt_sink_srv_am_hal_result_t;
typedef hal_audio_event_t           bt_sink_srv_am_event_result_t;
typedef hal_audio_device_t          bt_sink_srv_am_device_set_t;
typedef hal_audio_sampling_rate_t   bt_sink_srv_am_sample_rate_t;
typedef hal_audio_bits_per_sample_t bt_sink_srv_am_bit_per_sample_t;
typedef hal_audio_channel_number_t  bt_sink_srv_am_channel_number_t;
typedef hal_audio_active_type_t     bt_sink_srv_am_active_type_t;
typedef bt_codec_media_event_t      bt_sink_srv_am_bt_event_t;
typedef bt_codec_hfp_audio_t        bt_sink_srv_am_hfp_codec_t;
#ifdef MTK_RECORD_ENABLE
typedef record_encoder_cability_t audio_record_codec_capability_t;
#endif
//typedef bt_codec_a2dp_audio_t       bt_sink_srv_am_a2dp_codec_t;
#ifdef AIR_BT_CODEC_BLE_ENABLED
typedef bt_codec_le_audio_t         bt_sink_srv_am_ble_codec_t;
#endif
typedef int8_t                      bt_sink_srv_am_id_t;
typedef uint8_t                     bt_sink_srv_am_volume_level_t;
typedef uint32_t                    bt_sink_srv_am_a2dp_sink_latency_t;
typedef uint32_t                    bt_sink_srv_am_bt_audio_param_t;


typedef wchar_t bt_sink_srv_file_path_t;
#define FILE_PATH_TYPE_LEN          (2)

#ifndef WIN32_UT
extern xSemaphoreHandle g_xSemaphore_ami;
#endif
extern bt_sink_srv_am_id_t g_aud_id_num;
#define AUD_ID_INVALID  -1

#ifndef __AUDIO_MP3_ENABLE__
/** @brief This enumeration defines the MP3 codec events. */
typedef enum {
    MP3_CODEC_MEDIA_OK = 0,       /**< The codec operation was successful.   */
    MP3_CODEC_MEDIA_REQUEST,      /**< The MP3 codec requested a bitstream. */
    MP3_CODEC_MEDIA_UNDERFLOW,    /**< The bitstream has an underflow. */
    MP3_CODEC_MEDIA_JUMP_FILE_TO, /**< The MP3 codec requested to jump to specific position on the file. The position is jump_file_to_specified_position which is a member of #mp3_media_handle_s. */
    MP3_CODEC_AWS_CHECK_CLOCK_SKEW,
    MP3_CODEC_AWS_CHECK_UNDERFLOW,
    MP3_CODEC_MEDIA_BITSTREAM_END,/**< Tha MP3 codec play constant array end. */
} bt_sink_srv_mp3_codec_event_t;
#endif


#define BT_SINK_SRV_AWS_SKEW_LOOP_COUNT     (100)

#define BT_SINK_SRV_AWS_SKEW_LOOP_1ST_COUNT     (95)


#define BT_SINK_SRV_AM_PCM_RING_BUFFER_SIZE         (1024 * 3)


/**
 * @defgroup am_enum Enum
 * @{
 */

/**
 *  @brief This enum defines audio handler ID state type.
 */
typedef enum {
    ID_CLOSE_STATE   = 0,
    ID_IDLE_STATE    = 1,
    ID_PLAY_STATE    = 2,
    ID_SUSPEND_STATE = 3,
    ID_RESUME_STATE  = 4,
    ID_STOPING_STATE = 5,
} bt_sink_srv_am_reg_id_state_t;

/**
 *  @brief This enum defines audio handler prioirty type.
 */
typedef enum {
    AUD_LOW     = 0,
    AUD_MIDDLE  = 1,
    AUD_HIGH    = 2
} bt_sink_srv_am_priority_t;

/**
 *  @brief This enum defines audio handler instruction type.
 */
typedef enum {
    AUD_SELF_CMD_REQ      = 0,
    AUD_RESUME_IND        = 1,
    AUD_SUSPEND_IND       = 2,
    AUD_SUSPEND_BY_IND    = 3,
    AUD_A2DP_PROC_IND     = 4,
    AUD_STREAM_EVENT_IND  = 5,
    AUD_STREAM_DATA_REQ   = 6,
    AUD_TIMER_IND         = 7,
    AUD_SINK_OPEN_CODEC   = 8,
    AUD_HFP_EVENT_IND     = 9,
    AUD_FILE_OPEN_CODEC   = 10,
    AUD_FILE_PROC_IND     = 11,
    AUD_HAL_EVENT_IND     = 12,
    /* AWS Group */
    AUD_AWS_SINK_OPEN_CODEC      = 30,
    AUD_AWS_A2DP_PROC_IND        = 31,

    AUD_CB_MSG_TOTAL
} bt_sink_srv_am_cb_msg_class_t;

/**
 *  @brief This enum defines audio handler result of instruction type.
 */
typedef enum {
    AUD_EMPTY                          = 0,
    AUD_SINK_PROC_PTR                  = 1,
    AUD_CMD_FAILURE                    = 2,
    AUD_CMD_COMPLETE                   = 3,
    AUD_SUSPEND_BY_NONE                = 4,
    AUD_SUSPEND_BY_PCM                 = 5,
    AUD_SUSPEND_BY_HFP                 = 6,
    AUD_SUSPEND_BY_A2DP                = 7,
    AUD_SUSPEND_BY_FILES               = 8,
    AUD_SUSPEND_BY_AWS                 = 9,
    AUD_FILE_PROC_PTR                  = 10,
    AUD_SUSPEND_BY_LC                  = 11,
    AUD_SUSPEND_BY_LINE_OUT            = 12,
    AUD_SUSPEND_BY_USB_OUT             = 13,
    AUD_SUSPEND_BY_RECORDER            = 14,
    AUD_SUSPEND_BY_LINE_IN             = 15,
    AUD_SUSPEND_BY_USB_IN              = 16,
    AUD_SUSPEND_BY_LE_CALL             = 17,
    AUD_A2DP_CODEC_RESTART             = 30,
    AUD_A2DP_AWS_UNDERFLOW             = 35,
    AUD_A2DP_TIME_REPORT               ,
    AUD_HFP_OPEN_CODEC_DONE            ,
    AUD_FILE_EVENT_BASE                = 45,
    AUD_FILE_EVENT_DATA_REQ            = AUD_FILE_EVENT_BASE + 1,
    AUD_FILE_EVENT_JUMP_INFO,
    AUD_FILE_EVENT_DATA_END,
    AUD_FILE_EVENT_UNDERFLOW,
    AUD_FILE_MP3_BITSTREAM_END,
    AUD_STREAM_EVENT_ERROR             = BT_CODEC_MEDIA_ERROR,
    AUD_STREAM_EVENT_NONE              ,
    AUD_STREAM_EVENT_UNDERFLOW         ,
    AUD_STREAM_EVENT_DATA_REQ          ,
    AUD_STREAM_EVENT_DATA_NOTIFICATION ,
    AUD_STREAM_EVENT_TERMINATED        ,
    AUD_CODEC_MEDIA_AWS_CHECK_UNDERFLOW,
    AUD_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW,
    AUD_RESUME_PLAY_STATE              ,
    AUD_RESUME_IDLE_STATE              ,
    AUD_HFP_PLAY_OK                    = 100,
    AUD_HFP_AVC_PARA_SEND              ,
    AUD_A2DP_LTCS_REPORT               ,
    AUD_A2DP_DL_REINIT_REQUEST         ,
    AUD_A2DP_ACTIVE_LATENCY_REQUEST    ,
    AUD_A2DP_RECONNECT_REQUEST         ,
    AUD_A2DP_UPDATE_BUF_INFO           ,
} bt_sink_srv_am_cb_sub_msg_t;

/**
 *  @brief This enum defines audio layer type.
 */
typedef enum {
    MOD_AMI   = 0,
    MOD_AM    = 1,
    MOD_TMR   = 2,
    MOD_L1SP  = 3
} bt_sink_srv_am_module_t;

/**
 *  @brief This enum defines audio handler message type.
 */
typedef enum {
    MSG_ID_AM_CODE_BEGIN                       = 0,
    MSG_ID_STREAM_OPEN_REQ                     = 1,
    MSG_ID_STREAM_PLAY_REQ                     = 2,
    MSG_ID_STREAM_STOP_REQ                     = 3,
    MSG_ID_STREAM_CLOSE_REQ                    = 4,
    MSG_ID_STREAM_SET_VOLUME_REQ               = 5,
    MSG_ID_STREAM_MUTE_DEVICE_REQ              = 6,
    MSG_ID_STREAM_CONFIG_DEVICE_REQ            = 7,
    MSG_ID_STREAM_READ_WRITE_DATA_REQ          = 8,
    MSG_ID_STREAM_GET_LENGTH_REQ               = 9,
    MSG_ID_MEDIA_A2DP_PROC_CALL_EXT_REQ        = 10,
    MSG_ID_MEDIA_HFP_EVENT_CALL_EXT_REQ        = 11,
    MSG_ID_MEDIA_EVENT_STREAM_OUT_CALL_EXT_REQ = 12,
    MSG_ID_MEDIA_EVENT_STREAM_IN_CALL_EXT_REQ  = 13,
    MSG_ID_TIMER_OUT_CALL_EXT_REQ              = 14,
    MSG_ID_MEDIA_FILE_PROCE_CALL_EXT_REQ       = 15,
    MSG_ID_HAL_EVENT_EXT_REQ                   = 17,
    MSG_ID_AUDIO_SET_PAUSE                     = 18,
    MSG_ID_AUDIO_SET_RESUME                    = 19,
    MSG_ID_AUDIO_SIDE_TONE_ENABLE              = 20,
    MSG_ID_AUDIO_SIDE_TONE_DISABLE             = 21,
    MSG_ID_AUDIO_SET_SIDE_TONE_VOLUME          = 22,
    MSG_ID_AUDIO_DL_SUSPEND                    = 23,
    MSG_ID_AUDIO_DL_RESUME                     = 24,
    MSG_ID_AUDIO_SET_FEATURE                   = 25,

    MSG_ID_AUDIO_UL_SUSPEND                    = 26,
    MSG_ID_AUDIO_UL_RESUME                     = 27,
    MSG_ID_STREAM_PLAY_REQ_TEST                = 28,
    MSG_ID_STREAM_STOP_REQ_TEST                = 29,
    MSG_ID_AUDIO_SET_VENDOR_SE                 = 30,

    /* NWS Group */
    MSG_ID_MEDIA_AWS_A2DP_PROC_CALL_EXT_REQ    = 31,

    MSG_ID_TRANS_SET_CONFIG_REQ  = 32,

    MSG_ID_VOICE_SET_MIC_TYPE_REQ  = 33,
    /* Audio Sync */
    MSG_ID_AUDIO_REQUEST_SYNC                  = 34,
    MSG_ID_AM_CODE_END
} bt_sink_srv_am_msg_id_t;

/**
 *  @brief This enum defines audio handler return result.
 */
typedef enum {
    AUD_EXECUTION_FAIL    = -1,
    AUD_EXECUTION_SUCCESS =  0,
} bt_sink_srv_am_result_t;

/**
 *  @brief This enum defines audio stream type.
 */
typedef enum {
    STREAM_IN    = 0x0001,
    STREAM_OUT   = 0x0002,
    STREAM_OUT_2 = 0x0003,
    STREAM_OUT_3 = 0x0004,
} bt_sink_srv_am_stream_type_t;

/**
 *  @brief This enum defines audio type.
 */
typedef enum {
    PCM          = 0,
    A2DP         = 1,
    HFP          = 2,
    NONE         = 3,
    FILES        = 4,
    AWS          = 5,

    RECORD       = 6,
    VOICE_PROMPT = 7,
    LINE_IN      = 8,
    BLE          = 9,
    USB_AUDIO_IN = 10,
    AUDIO_TRANSMITTER      = 12,
    AM_TYPE_TOTAL
} bt_sink_srv_am_type_t;

typedef enum {
    AM_PLAYBACK_DL1 = 1 << 0,
    AM_PLAYBACK_DL2 = 1 << 1,
    AM_PLAYBACK_UL1 = 1 << 2,
    AM_PLAYBACK_UL2 = 1 << 3,

    AM_PLAYBACK_INDEX_TOTAL = 4,
} am_playback_index_t;

typedef struct {
#if 1
    bt_sink_srv_am_priority_t DL_1;
    bt_sink_srv_am_priority_t DL_2;
    bt_sink_srv_am_priority_t UL_1;
    bt_sink_srv_am_priority_t UL_2;
#else
    bt_sink_srv_am_priority_t playback[AM_PLAYBACK_INDEX_TOTAL];
#endif
} bt_sink_srv_am_priority_table_t;


typedef enum {
    AM_PLAYBACK_REJECT     = -1,
    AM_PLAYBACK_NONE       =  0,
    AM_PLAYBACK_PERMISSION =  1,
} am_playback_arbitration_status_t;

typedef struct {
    am_playback_arbitration_status_t am_arbitration_status;
    uint32_t                         preempt_index;
} am_playback_arbitration_result_t;

/**
 *  @brief This enum defines file type.
 */
typedef enum {
    FILE_NONE   = 0,
    FILE_MP3    = 1,
    FILE_WAV    = 2,

    FILE_TOAL
} am_file_type_t;

/**
 *  @brief This enum defines audio volume type.
 */
typedef enum {
    AUD_VOL_AUDIO   = 0,
    AUD_VOL_SPEECH  = 1,
    AUD_VOL_TYPE
} bt_sink_srv_am_volume_type_t;


/**
 *  @brief This struct defines audio sink status detail info.
 */
typedef enum a2dp_sink_state {
    A2DP_SINK_CODEC_CLOSE  = 0,
    A2DP_SINK_CODEC_OPEN   = 1,
    A2DP_SINK_CODEC_PLAY   = 2,
    A2DP_SINK_CODEC_STOP   = 3,
    A2DP_SINK_CODEC_TOTAL  = 4
} bt_sink_srv_am_sink_state_t;

/**
 *  @brief This struct defines audio file status.
 */
typedef enum {
    FILE_CODEC_CLOSE  = 0,
    FILE_CODEC_OPEN   = 1,
    FILE_CODEC_PLAY   = 2,
    FILE_CODEC_STOP   = 3,
    FILE_CODEC_PAUSE  = 4,

    FILE_CODEC_TOTAL
} bt_sink_srv_am_file_state_t;

/**
 *  @brief This struct defines audio volume-out level info.
 */
typedef enum {
    AUD_VOL_OUT_LEVEL0 = 0,
    AUD_VOL_OUT_LEVEL1 = 1,
    AUD_VOL_OUT_LEVEL2 = 2,
    AUD_VOL_OUT_LEVEL3 = 3,
    AUD_VOL_OUT_LEVEL4 = 4,
    AUD_VOL_OUT_LEVEL5 = 5,
    AUD_VOL_OUT_LEVEL6 = 6,

    AUD_VOL_OUT_LEVEL7,
    AUD_VOL_OUT_LEVEL8,
    AUD_VOL_OUT_LEVEL9,
    AUD_VOL_OUT_LEVEL10,
    AUD_VOL_OUT_LEVEL11,
    AUD_VOL_OUT_LEVEL12,
    AUD_VOL_OUT_LEVEL13,
    AUD_VOL_OUT_LEVEL14,
    AUD_VOL_OUT_LEVEL15,

    AUD_VOL_OUT_MAX
} bt_sink_srv_am_volume_level_out_t;

/**
 *  @brief This struct defines audio volume-in level info.
 */
typedef enum {
    AUD_VOL_IN_LEVEL0  = 0,
    AUD_VOL_IN_MAX     = 1
} bt_sink_srv_am_volume_level_in_t;

/**
 *  @brief This struct defines audio volume-out device info.
 */
typedef enum {
    LOUDSPEAKER_STREAM_OUT  = 0,
    EARPHONE_STREAM_OUT     = 1,
    DEVICE_OUT_MAX          = 2
} bt_sink_srv_am_device_out_t;

/**
 *  @brief This struct defines audio volume-in device info.
 */
typedef enum {
    MICPHONE_STREAM_IN  = 0,
    DEVICE_IN_MAX       = 1
} bt_sink_srv_am_device_in_t;

typedef enum {
    STREAM_OUT_MIX_METHOD_NO_MIX = BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_NO_MIX,
    STREAM_OUT_MIX_METHOD_REPLACE = BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_REPLACE,
    STREAM_OUT_MIX_METHOD_WEIGHTED_MEAN = BT_CODEC_MEDIA_HFP_MIX_STREAM_OUT_METHOD_WEIGHTED_MEAN,

    STREAM_OUT_MIX_METHOD_END
} bt_sink_srv_stream_out_mix_t;

typedef enum {
    BT_SINK_SRV_VOICE_SYS_POWER_ON = 0,
    BT_SINK_SRV_VOICE_SYS_POWER_OFF,
    BT_SINK_SRV_VOICE_BT_POWER_ON,
    BT_SINK_SRV_VOICE_BT_POWER_OFF,
    BT_SINK_SRV_VOICE_BT_CONNECT,
    BT_SINK_SRV_VOICE_BT_DISCONNECT,

    BT_SINK_SRV_VOICE_END
} bt_sink_srv_voice_t;

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
/**
 *  @brief This struct defines audio app callback message ID info.
 */
typedef enum {
    AUDIO_APPS_EVENTS_ANC_STATUS_CHANGE,
    AUDIO_APPS_EVENTS_SAMPLE_RATE_CHANGE,
} audio_sink_srv_am_app_callback_message_t;

/**
 *  @brief This structure defines the AWS codec capability.
 */
typedef struct {
  hal_audio_sampling_rate_t sampling_rate;
  bool Enable;
} I2S_param_for_external_DSP_t;

/**
 *  @brief This struct defines audio codec type detail info.
 */
typedef union audio_app_param {
    I2S_param_for_external_DSP_t   I2S_param;
} audio_app_param_t;
#endif

/**
 *  @brief This struct defines hfp avc sync status
 */
typedef enum {
    AVC_INIT_SYNC = 0,
    AVC_UPDATE_SYNC,
} bt_sink_avc_sync_statys_t;

/**
 *  @brief ANC and Pass-through mode.
 */
typedef enum {
    AUDIO_ANC_CONTROL_NORMAL_MODE        = 0,  /**< .   */
    AUDIO_ANC_CONTROL_ANC_MODE,                /**< .   */
    AUDIO_ANC_CONTROL_PASSTHROUGH_MODE,        /**< .   */
    AUDIO_ANC_CONTROL_PASSTHROUGH_VOICE_MODE,  /**< .   */
} audio_anc_control_mode_t;

/**
 * @}
 */

/**
 * @defgroup am_struct Struct
 * @{
 */

/**
 *  @brief This structure defines the dsp audio dump NVKey.
 */
typedef struct stru_dsp_datadump_para_s
{
    uint32_t Dump_Enable;                /**< @Value 0 @Desc 0:Disable 1:Enable */
    uint32_t Dump_Scenario_Sel;          /**< @Value 0x00000002 @Desc 0:Diaable, 1:I2S_MASTER, 2:I2S_SLAVE, 3:SPDIF, 4:USB, 5:AIR, */
    uint32_t Dump_Mask_0;                /**< @Value 0x00000000 @Desc Mask ID  0-31 */
    uint32_t Dump_Mask_1;                /**< @Value 0x00000000 @Desc Mask ID 32-63*/
    uint32_t Buf_size;                   /**< @Value 0x00001388 @Desc internal buffer size for dump data in */
} PACKED DSP_PARA_DATADUMP_STRU;

#if defined(MTK_AWS_MCE_ENABLE)
typedef uint8_t bt_aws_role_t;

#define BT_AWS_ROLE_SOURCE      0x00                      /**< The AWS role of source. */

#define BT_AWS_ROLE_SINK        0x01                      /**< The AWS role of sink. */

#define BT_AWS_ROLE_NONE        0xFF                      /**< The AWS role of none. */


typedef uint32_t bt_aws_codec_type_t;
#define BT_AWS_CODEC_TYPE_SBC        (0x00)           /**< SBC codec. */
#define BT_AWS_CODEC_TYPE_MP3        (0x01)           /**< MP3 codec. */
#define BT_AWS_CODEC_TYPE_AAC        (0x02)           /**< AAC codec. */
#define BT_AWS_CODEC_TYPE_VENDOR     (0xFF)           /**< VENDOR codec. */

/**
 *  @brief This structure defines the AWS codec capability.
 */
typedef struct {
  bt_aws_codec_type_t type;      /**< Codec type. */
  uint8_t   length;              /**< The length of the following codec. */
  union {
        bt_a2dp_aac_codec_t aac; /**< AAC codec. */
        bt_a2dp_sbc_codec_t sbc; /**< SBC codec. */
  } codec;                       /**< Codec information. */
} bt_aws_codec_capability_t;
#endif

/**
 *  @brief This struct defines audio stream node detail info.
 */
typedef struct audio_stream_node {
    bt_sink_srv_am_sample_rate_t    stream_sample_rate;
    bt_sink_srv_am_bit_per_sample_t stream_bit_rate;
    bt_sink_srv_am_channel_number_t stream_channel;
    void                        *buffer;
    uint32_t                    size;
} bt_sink_srv_am_stream_node_t;

typedef struct {
    bt_a2dp_codec_capability_t codec_cap;  /**< The capabilities of Bluetooth codec */
    bt_a2dp_role_t             role;       /**< The Bluetooth codec roles */
    uint16_t a2dp_mtu;
} bt_sink_srv_am_a2dp_codec_t;

#define BT_A2DP_VENDOR_CODEC_SAMPLE_RATE_MASK       (0x3F)   /**< VENDOR codec SAMPLE_RATE_MASK. */
#define BT_A2DP_VENDOR_CODEC_CHANNEL_MODE_MASK      (0x07)   /**< VENDOR codec CHANNEL_MODE_MASK.*/

#if defined(__BT_AWS_SUPPORT__) || defined(MTK_AWS_MCE_ENABLE)
typedef struct {
    bt_aws_codec_capability_t codec_cap;  /**< The capabilities of Bluetooth codec */
    bt_aws_role_t             role;       /**< The Bluetooth codec roles */
} bt_sink_srv_am_aws_codec_t;
#endif

/**
 *  @brief This struct defines audio pcm format detail info.
 */
typedef struct am_pcm {
    bt_sink_srv_am_stream_node_t    stream;
    bt_sink_srv_am_stream_type_t    in_out;
    bt_sink_srv_am_event_result_t   event;
} bt_sink_srv_am_pcm_format_t;


/**
 *  @brief This struct defines audio a2dp format detail info.
 */
typedef struct am_a2dp {
    bt_sink_srv_am_a2dp_codec_t    a2dp_codec;
    bt_sink_srv_am_bt_event_t      a2dp_event;
} bt_sink_srv_am_a2dp_format_t;


#if defined(__BT_AWS_SUPPORT__) || defined(MTK_AWS_MCE_ENABLE)
/**
 *  @brief This struct defines audio aws format detail info.
 */
typedef struct am_aws {
    bt_sink_srv_am_aws_codec_t    aws_codec;
    bt_sink_srv_am_bt_event_t      aws_event;
} bt_sink_srv_am_aws_format_t;
#endif


/**
 *  @brief This struct defines audio hfp format detail info.
 */
typedef struct am_hfp {
    bt_sink_srv_am_hfp_codec_t     hfp_codec;
    bt_sink_srv_am_bt_event_t      hfp_event;
} bt_sink_srv_am_hfp_format_t;

#ifdef MTK_LINE_IN_ENABLE

/**
 *  @brief This structure defines the Line in codec capability.
 */
typedef struct {
    hal_audio_sampling_rate_t linein_sample_rate;
    hal_audio_device_t in_audio_device;
    hal_audio_device_t out_audio_device;
} audio_line_in_codec_capability_t;

typedef struct {
    audio_line_in_codec_capability_t codec_cap;
} audio_sink_srv_am_line_in_codec_t;

/**
 *  @brief This struct defines audio line_in format detail info.
 */
typedef struct am_line_in {
    audio_sink_srv_am_line_in_codec_t line_in_codec;
} audio_sink_srv_am_line_in_format_t;
#endif

#ifdef MTK_RECORD_ENABLE
typedef struct {
    audio_record_codec_capability_t codec_cap;
} audio_sink_srv_am_record_codec_t;

/**
 *  @brief This struct defines audio record format detail info.
 */
typedef struct am_record {
    audio_sink_srv_am_record_codec_t record_codec;
    void *Reserve_callback;
    void *Reserve_callback_user_data;
} audio_sink_srv_am_record_format_t;
#endif

#ifdef MTK_USB_AUDIO_PLAYBACK_ENABLE

/**
 *  @brief This structure defines the Line in codec capability.
 */
typedef struct {
    hal_audio_sampling_rate_t usb_audio_sample_rate;
    hal_audio_device_t in_audio_device;
    hal_audio_device_t out_audio_device;
} audio_usb_audio_codec_capability_t;

typedef struct {
    audio_usb_audio_codec_capability_t codec_cap;
} audio_sink_srv_am_usb_audio_codec_t;

/**
 *  @brief This struct defines audio usb_audio format detail info.
 */
typedef struct am_usb_audio {
    audio_sink_srv_am_usb_audio_codec_t usb_audio_codec;
} audio_sink_srv_am_usb_audio_format_t;

#endif

#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
#include "audio_transmitter_playback.h"
typedef struct{
    audio_transmitter_scenario_type_t scenario_type;
    uint8_t scenario_sub_id;

    uint32_t scenario_runtime_config_type;
    audio_transmitter_runtime_config_t scenario_runtime_config;
}audio_sink_srv_am_audio_transmitter_format_t;
#endif

#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE

extern voice_mic_type_t current_voice_mic_type;

bt_sink_srv_am_result_t ami_set_voice_mic_type(voice_mic_type_t voice_mic_type);

typedef struct{
    voice_mic_type_t voice_mic_type;
}audio_sink_srv_am_audio_detachable_mic_format_t;

#endif

#ifdef AIR_BT_CODEC_BLE_ENABLED
/**
 *  @brief This struct defines audio ble format detail info.
 */
typedef struct am_ble {
    bt_sink_srv_am_ble_codec_t     ble_codec;
    bt_sink_srv_am_bt_event_t      ble_event;
} bt_sink_srv_am_ble_format_t;
#endif

/**
 *  @brief This struct defines file mp3 event type.
 */
 typedef enum {
    AM_MP3_CODEC_MEDIA_OK = MP3_CODEC_MEDIA_OK,                     /**< The codec operation was successful.   */
    AM_MP3_CODEC_MEDIA_REQUEST = MP3_CODEC_MEDIA_REQUEST,           /**< The MP3 codec requested a bitstream. */
    AM_MP3_CODEC_MEDIA_UNDERFLOW = MP3_CODEC_MEDIA_UNDERFLOW,       /**< The bitstream has an underflow. */
    AM_MP3_CODEC_MEDIA_JUMP_FILE_TO = MP3_CODEC_MEDIA_JUMP_FILE_TO,
    AM_MP3_CODEC_AWS_CHECK_CLOCK_SKEW = MP3_CODEC_AWS_CHECK_CLOCK_SKEW,
    AM_MP3_CODEC_AWS_CHECK_UNDERFLOW = MP3_CODEC_AWS_CHECK_UNDERFLOW,
    AM_MP3_CODEC_MEDIA_BITSTREAM_END = MP3_CODEC_MEDIA_BITSTREAM_END,/**< Tha MP3 codec play constant array end. */

    AM_ME3_CODEC_MEDIA_TOTAL
} am_mp3_event_type_t;

/**
 *  @brief This struct defines file mp3 event struct.
 */
 typedef struct {
    am_mp3_event_type_t event;
    void *param;
} am_mp3_event_t;


/**
 *  @brief This struct defines file mp3 event type.
 */
#ifdef __AUDIO_COMMON_CODEC_ENABLE__
typedef enum {
    AM_AUDIO_CODEC_MEDIA_OK        = AUDIO_CODEC_MEDIA_OK,          /**< The codec operation was successful.   */
    AM_AUDIO_CODEC_MEDIA_REQUEST   = AUDIO_CODEC_MEDIA_REQUEST,     /**< The audio codec requested a bitstream. */
    AM_AUDIO_CODEC_MEDIA_UNDERFLOW = AUDIO_CODEC_MEDIA_UNDERFLOW,   /**< The bitstream has an underflow. */
    AM_AUDIO_CODEC_MEDIA_EVENT_END = AUDIO_CODEC_MEDIA_EVENT_END,   /**< The bitstream had been totally streamed out. */

    AM_AUDIO_CODEC_MEDIA_TOTAL
} am_audio_event_type_t;
#else
typedef enum {
    AM_AUDIO_CODEC_MEDIA_OK        = 0,                             /**< The codec operation was successful.   */
    AM_AUDIO_CODEC_MEDIA_REQUEST   = 1,                             /**< The audio codec requested a bitstream. */
    AM_AUDIO_CODEC_MEDIA_UNDERFLOW = 2,                             /**< The bitstream has an underflow. */
    AM_AUDIO_CODEC_MEDIA_EVENT_END = 3,                             /**< The bitstream had been totally streamed out. */

    AM_AUDIO_CODEC_MEDIA_TOTAL
} am_audio_event_type_t;
#endif


/**
 *  @brief This struct defines file audio event struct.
 */
 typedef struct {
    am_audio_event_type_t event;
    void *param;
} am_audio_event_t;


/**
 *  @brief This struct defines file type event struct.
 */
typedef struct {
    uint8_t type;
    union {
        am_mp3_event_t mp3;
        am_audio_event_t audio;
    } event;
} am_file_event_t;

/**
 *  @brief This struct defines audio local music format detail info.
 */
typedef struct am_files {
    ///bt_sink_srv_file_path_t *path;
    am_file_type_t file_type;
    am_file_event_t file_event;
} bt_sink_srv_am_files_format_t;

/**
 *  @brief This struct defines audio codec type detail info.
 */
typedef union audio_codec {
    bt_sink_srv_am_pcm_format_t   pcm_format;
    bt_sink_srv_am_a2dp_format_t  a2dp_format;
    bt_sink_srv_am_hfp_format_t   hfp_format;
    #ifdef AIR_BT_CODEC_BLE_ENABLED
    bt_sink_srv_am_ble_format_t   ble_format;
    #endif
    bt_sink_srv_am_files_format_t files_format;
    #if defined(__BT_AWS_SUPPORT__) || defined(MTK_AWS_MCE_ENABLE)
    bt_sink_srv_am_aws_format_t   aws_format;
    #endif
    #ifdef MTK_LINE_IN_ENABLE
    audio_sink_srv_am_line_in_format_t line_in_format;
    #endif
    #ifdef MTK_RECORD_ENABLE
    audio_sink_srv_am_record_format_t record_format;
    #endif
    #ifdef MTK_USB_AUDIO_PLAYBACK_ENABLE
    audio_sink_srv_am_usb_audio_format_t usb_audio_format;
    #endif
    #ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    audio_sink_srv_am_audio_transmitter_format_t audio_transmitter_format;
    #endif
    #ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE
    audio_sink_srv_am_audio_detachable_mic_format_t audio_detachable_mic_format;
    #endif
} bt_sink_srv_am_codec_t;


/**
 *  @brief These structs define audio feature (from am to dsp) detail info.
 */
#define FEATURE_NO_NEED_ID    -1

typedef enum {
    DC_COMPENSATION = 1 << 0,
    AM_A2DP_PEQ     = 1 << 1,
    AM_HFP_AVC      = 1 << 2,
    AM_ANC          = 1 << 3,
    AM_VP           = 1 << 4,
    AM_LINEIN_PEQ   = 1 << 6,
    AM_DYNAMIC_CHANGE_DSP_SETTING = 1 << 7,
    AM_ADAPTIVE_FF  = 1 << 8,
    AM_BLE          = 1 << 9,
    AM_AUDIO_LOOPBACK = 1 << 10,
    AM_AUDIO_BT_SET_PLAY_EN = 1 << 11,
    AM_AUDIO_COSYS          = 1 << 12,
    AM_ANC_MONITOR = 1<<13,
} am_feature_type_t;

/** @brief AWS one source or two source */
typedef enum {
    AUDIO_CHANNEL_SELECTION_STEREO = 0, /**< DSP streaming output L and R will be it default. */     /**< AWS Agent and Partner exist. */
    AUDIO_CHANNEL_SELECTION_MONO   = 1, /**< DSP streaming output L and R will be (L+R)/2. */    /**< AWS only Agent exist. */
    AUDIO_CHANNEL_SELECTION_BOTH_L = 2, /**< DSP streaming output both L. */
    AUDIO_CHANNEL_SELECTION_BOTH_R = 3, /**< DSP streaming output both R. */
    AUDIO_CHANNEL_SELECTION_NUM,
} audio_channel_selection_t;

/** @brief Get Audio Channel in DSP */
typedef enum {
    AUDIO_CHANNEL_NONE  = -1, /**< Audio channel none. */
    AUDIO_CHANNEL_L     =  0, /**< Audio channel L. */
    AUDIO_CHANNEL_R     =  1, /**< Audio channel R. */
    AUDIO_CHANNEL_SWAP  =  2, /**< Audio channel SWAP. */
} audio_channel_t;

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#define PEQ_DEFAULT_ENABLE              (0xFF)
#define PEQ_DEFAULT_SOUND_MODE          (1)
#define PEQ_SOUND_MODE_FORCE_DRC        (0xFC)
#define PEQ_SOUND_MODE_UNASSIGNED       (0xFD)
#define PEQ_SOUND_MODE_SAVE             (0xFE)
#define PEQ_SOUND_MODE_REAlTIME         (0xFF)
#define PEQ_ON                          (0xFB)
#define PEQ_FW_LATENCY                  (130)
#ifdef MTK_POST_PEQ_DEFAULT_ON
#define POST_PEQ_DEFAULT_ENABLE         (1)
#define POST_PEQ_DEFAULT_SOUND_MODE     (1)
#else
#define POST_PEQ_DEFAULT_ENABLE         (0)
#define POST_PEQ_DEFAULT_SOUND_MODE     (0)
#endif
#define POST_PEQ_FBANC_SOUND_MODE       (2)
#ifdef MTK_DEQ_ENABLE
#define DEQ_AUDIO_SOUND_MODE            (1)
#define DEQ_AUDIO_NVKEY                 (NVKEY_DSP_PARA_PEQ_COEF_26)
#endif
typedef void(*bt_sink_srv_am_peq_callback)(uint16_t *);
typedef struct am_peq {
    uint8_t enable;
    uint8_t sound_mode;
    uint16_t u2ParamSize;
    uint16_t *pu2Param;
    uint32_t target_bt_clk;
    uint8_t setting_mode;
    uint8_t not_clear_sysram;
    uint8_t phase_id;
    bt_sink_srv_am_peq_callback peq_notify_cb;
} bt_sink_srv_am_peq_param_t;
#endif

#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
typedef struct am_anc {
    audio_anc_control_event_t event;
    audio_anc_control_cap_t   cap;
} bt_sink_srv_am_anc_param_t;
#else
typedef struct am_anc {
    anc_control_event_t event;
    anc_filter_type_t   filter;
    anc_sw_gain_t       sw_gain;
    int16_t             user_runtime_gain;
    uint16_t            param;
    void (*anc_control_callback)(anc_control_event_t event_id, anc_control_result_t result);
} bt_sink_srv_am_anc_param_t;
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
typedef struct am_adaptive_ff {
    anc_fwd_iir_t *cmp_ori_filter;
    anc_fwd_iir_t *cmp_new_filter;
} bt_sink_srv_am_adaptive_ff_param_t;
#endif
#endif
#ifdef MTK_ANC_SURROUND_MONITOR_ENABLE
typedef struct am_anc_monitor{
    uint32_t event; //audio_anc_monitor_event_t
    uint32_t event_type; //audio_anc_monitor_get_info_t, audio_anc_monitor_set_info_t
    uint32_t param;
} bt_sink_srv_am_anc_monitor_param_t;
#endif
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
typedef struct am_cosys {
    am_feature_type_t sub_type;
    uint32_t          cosys_event;
} bt_sink_srv_am_cosys_param_t;
#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
typedef struct am_vp {
    prompt_control_event_t event;
    prompt_control_tone_type_t tone_type;
    uint8_t *tone_buf;
    uint32_t tone_size;
    uint32_t sync_time;
    prompt_control_callback_t vp_end_callback;
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
    prompt_control_dummy_source_param_t dummy_source_param;
#endif
} bt_sink_srv_am_vp_param_t;
#endif

#define AEC_NR_PARAM_TYPE_RX_EQ     (0x1)

#ifdef AIR_BT_CODEC_BLE_ENABLED
typedef enum {
    BLE_FEATURE_EVENT_PLAY_INFO,
} ble_feature_event_t;

typedef struct {
    uint32_t iso_interval; /* Unit with BT clock (312.5us) */
    uint32_t dl_timestamp_clk; /* Unit with BT clock (312.5us), indicate the first anchor of DL */
    uint32_t dl_retransmission_window_clk; /* Unit with BT clock (312.5us), valid bit[27:2] */
    uint16_t dl_timestamp_phase; /* Unit with 0.5us, valid value: 0~2499 */
    uint16_t dl_retransmission_window_phase; /* Unit with 0.5us, valid value: 0~2499 */
    uint8_t  dl_ft;
    uint8_t  dl_packet_counter; /* ISO DL packet counter & 0xFF */
    uint8_t  ul_ft;
    uint8_t  ul_packet_counter; /* ISO UL packet counter & 0xFF */
    uint32_t ul_timestamp; /* Unit with BT clock (312.5us), indicate the first anchor of UL */
    uint32_t ISOAnchorClk;
    uint16_t ISOAnchorPhase;
    uint8_t  DL_AVMIndex;
} ble_init_play_info_t;

typedef struct {
    ble_feature_event_t event;
    union {
        ble_init_play_info_t play_info;
    } param;
} bt_sink_srv_am_ble_param_t;
#endif

#define SIDETONE_GAIN_MAGIC_NUM 0xDEADBEEF

typedef enum {
    SIDETONE_SCENARIO_HFP,
    SIDETONE_SCENARIO_RESERVE,
    SIDETONE_SCENARIO_MAX
} sidetone_scenario_t;

typedef struct {
    sidetone_scenario_t scenario;
    int32_t side_tone_gain;
} bt_sink_srv_am_sidetone_param_t;

typedef union audio_feature_param {
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    bt_sink_srv_am_peq_param_t peq_param;
#endif
#ifdef MTK_ANC_ENABLE
    bt_sink_srv_am_anc_param_t anc_param;
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)||defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    bt_sink_srv_am_cosys_param_t cosys_param;
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    bt_sink_srv_am_adaptive_ff_param_t adaptive_ff_param;
#endif
#endif
#ifdef MTK_PROMPT_SOUND_ENABLE
    bt_sink_srv_am_vp_param_t vp_param;
#endif
    uint32_t avc_vol;
    audio_channel_selection_t channel;
#ifdef AIR_BT_CODEC_BLE_ENABLED
    bt_sink_srv_am_ble_param_t ble_param;
#endif
#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
    audio_dsp_a2dp_dl_play_en_param_t play_en_param;
#endif
    bt_sink_srv_am_sidetone_param_t sidetone_param;
#ifdef MTK_ANC_SURROUND_MONITOR_ENABLE
    bt_sink_srv_am_anc_monitor_param_t anc_monitor_param;
#endif
} bt_sink_srv_audio_feature_param_t;

typedef struct audio_feature {
    am_feature_type_t type_mask;
    bt_sink_srv_audio_feature_param_t feature_param;
} bt_sink_srv_am_feature_t;

/**
 *  @brief This struct defines sink media handle detail info.
 */
typedef struct {
    bt_status_t (*get_write_buffer)(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length);
    bt_status_t (*write_data_done)(bt_sink_srv_am_id_t aud_id, uint32_t length);
    bt_status_t (*finish_write_data)(bt_sink_srv_am_id_t aud_id);
    int32_t (*get_free_space)(bt_sink_srv_am_id_t aud_id);
    void (*reset_share_buffer)(bt_sink_srv_am_id_t aud_id);
    bt_status_t (*play)(bt_sink_srv_am_id_t aud_id);
    bt_status_t (*stop)(bt_sink_srv_am_id_t aud_id);
#if defined(__BT_AWS_SUPPORT__) || defined(MTK_AWS_MCE_ENABLE)
    int32_t (*set_aws_flag)(bt_sink_srv_am_id_t aud_id, bool flag);
    int32_t (*set_aws_initial_sync)(bt_sink_srv_am_id_t aud_id);
    void (*aws_plh_init)(bt_sink_srv_am_id_t aud_id);
    void (*aws_plh_deinit)(void);
#endif
    bt_media_handle_t *med_hd;
} bt_sink_srv_am_media_handle_t;

#ifdef __AUDIO_MP3_ENABLE__
/**
 *  @brief This struct defines mp3 media handle detailed information.
 */
typedef struct {
    void (*get_write_buffer)(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length);                 /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*get_read_buffer)(bt_sink_srv_am_id_t aud_id, uint8_t **buffer, uint32_t *length);                 /**< Get the available length to write into a shared buffer and a pointer to the shared buffer. */
    void (*write_data_done)(bt_sink_srv_am_id_t aud_id, uint32_t length);                                     /**< Update the write pointer to the shared buffer. */
    void (*finish_write_data)(bt_sink_srv_am_id_t aud_id);                                                    /**< Indicate last data transfer. */
    int32_t (*get_data_count)(bt_sink_srv_am_id_t aud_id);                                          /**< Get the data length from the shared buffer. */
    int32_t (*get_free_space)(bt_sink_srv_am_id_t aud_id);                                          /**< Get the available length from the shared buffer. */
    int32_t (*play)(bt_sink_srv_am_id_t aud_id);                                                    /**< The MP3 codec play function. */
    int32_t (*pause)(bt_sink_srv_am_id_t aud_id);                                                   /**< The MP3 codec pause function. */
    int32_t (*resume)(bt_sink_srv_am_id_t aud_id);                                                  /**< The MP3 codec resume function. */
    int32_t (*stop)(bt_sink_srv_am_id_t aud_id);                                                    /**< The MP3 codec stop function. */
    int32_t (*close_codec)(bt_sink_srv_am_id_t aud_id);                                             /**< The MP3 codec close_codec function. */
    int32_t (*skip_id3v2_and_reach_next_frame)(bt_sink_srv_am_id_t aud_id, uint32_t file_size);     /**< Skip id3v2 header and reach next frame */
    int32_t (*set_silence_frame_information)(bt_sink_srv_am_id_t aud_id, silence_frame_information_t *frm_info);    /**< Set the silence frame information. */
    int32_t (*fill_silence_frame)(bt_sink_srv_am_id_t aud_id, uint8_t *buffer);                                     /**< Get the silence frame. */
    int32_t (*get_data_status)(bt_sink_srv_am_id_t aud_id, mp3_codec_data_type_t type, int32_t *status);
    int32_t (*flush)(bt_sink_srv_am_id_t aud_id, int32_t flush_data_flag);

#ifdef __BT_AWS_SUPPORT__
    int32_t (*set_aws_flag)(bt_sink_srv_am_id_t aud_id, bool flag);
    int32_t (*set_aws_initial_sync)(bt_sink_srv_am_id_t aud_id);
    int32_t (*aws_init)(void);
    int32_t (*aws_deinit)(void);
    int32_t (*aws_set_clock_skew_compensation_value)(int32_t sample_count);
    int32_t (*aws_get_clock_skew_status)(int32_t *status);
    int32_t (*aws_set_clock_skew)(bool flag);
#endif /* __BT_AWS_SUPPORT__ */
} bt_sink_srv_am_mp3_media_handle_t;
#endif


/**
 *  @brief This struct defines file media handle detailed information.
 */
typedef struct {
    am_file_type_t type;
    union {
        #ifdef __AUDIO_MP3_ENABLE__
        bt_sink_srv_am_mp3_media_handle_t mp3;
        #endif
    } media_handle;
} bt_sink_srv_am_files_media_handle_t;

/**
 *  @brief This struct defines audio common stream-in info.
 */
typedef struct {
    bt_sink_srv_am_device_set_t        audio_device;
    bt_sink_srv_am_volume_level_in_t   audio_volume;
    bool                           audio_mute;
} bt_sink_srv_am_audio_stream_in_t;

/**
 *  @brief This struct defines audio common stream-out info.
 */
typedef struct {
    bt_sink_srv_am_device_set_t         audio_device;
    bt_sink_srv_am_volume_level_out_t   audio_volume;
    bool                            audio_mute;
} bt_sink_srv_am_audio_stream_out_t;


/**
 *  @brief This struct defines audio capability detail info.
 */
typedef struct audio_capability {
    bt_sink_srv_am_type_t               type;
    bt_sink_srv_am_codec_t              codec;
    bt_sink_srv_am_audio_stream_in_t    audio_stream_in;
    bt_sink_srv_am_audio_stream_out_t   audio_stream_out;
    bt_sink_srv_am_active_type_t        audio_path_type;
    bt_bd_addr_t dev_addr;
} bt_sink_srv_am_audio_capability_t;

/** @brief define audio callback function prototype for notification */
typedef void(*bt_sink_srv_am_notify_callback)(bt_sink_srv_am_id_t aud_id,
        bt_sink_srv_am_cb_msg_class_t msg_id,
        bt_sink_srv_am_cb_sub_msg_t sub_msg,
        void *parm);

/**
 *  @brief This struct defines action sync mechanism between L and R detail info.
 */
typedef enum {
    MCU2DSP_SYNC_REQUEST_START      = 0,
    MCU2DSP_SYNC_REQUEST_STOP       = 1,
    MCU2DSP_SYNC_REQUEST_SET_VOLUME = 2,
    MCU2DSP_SYNC_REQUEST_SET_MUTE   = 3,
    MCU2DSP_SYNC_REQUEST_FADE_OUT,
    MCU2DSP_SYNC_REQUEST_ACTION_MAX = 0xFFFFFFFF
} bt_sink_srv_am_sync_action_type_t;

typedef enum {
    MCU2DSP_SYNC_REQUEST_HFP = 0,
    MCU2DSP_SYNC_REQUEST_A2DP,
    MCU2DSP_SYNC_REQUEST_ANC,
    MCU2DSP_SYNC_REQUEST_VP,
    MCU2DSP_SYNC_REQUEST_SCENARIO_MAX = 0xFFFFFFFF
} bt_sink_srv_am_sync_scenario_type_t;

typedef enum {
    MCU2DSP_SYNC_VOLUME_CHANNEL_L   = 0,
    MCU2DSP_SYNC_VOLUME_CHANNEL_R,
    MCU2DSP_SYNC_VOLUME_CHANNEL_DUAL,
    AUDIO_SYNC_GAIN_SELECT_CHANNEL_MAX = 0xFFFFFFFF
} bt_sink_srv_am_sync_volume_channel_t;

typedef struct {
    uint32_t vol_level;
    bt_sink_srv_am_sync_volume_channel_t channel;
} bt_sink_srv_am_sync_volume_info_t;

typedef struct {
    bt_sink_srv_am_sync_scenario_type_t   sync_scenario_type;
    bt_sink_srv_am_sync_action_type_t     sync_action_type;
    uint32_t                              target_gpt_cnt;
    bt_sink_srv_am_sync_volume_info_t     vol_out;
} bt_sink_srv_am_audio_sync_capability_t;

/**
 *  @brief This struct defines audio handler in the background detail info.
 */
typedef struct am_background {
    bt_sink_srv_am_id_t                 aud_id;
    bt_sink_srv_am_type_t               type;
    bt_sink_srv_am_priority_t           priority;
    bt_sink_srv_am_priority_table_t     priority_table;
    bt_sink_srv_am_stream_type_t        in_out;
    bt_sink_srv_am_audio_stream_in_t    audio_stream_in;
    bt_sink_srv_am_audio_stream_out_t   audio_stream_out;
    bt_sink_srv_am_active_type_t        audio_path_type;
    bt_sink_srv_am_notify_callback      notify_cb;
    uint32_t                        *data_length_ptr;
    struct am_background            *prior;
    struct am_background            *next;
    bt_sink_srv_am_codec_t              local_context;
    bt_sink_srv_am_feature_t            local_feature;
    bt_sink_srv_am_audio_sync_capability_t sync_parm;
} bt_sink_srv_am_background_t;
extern bt_sink_srv_am_background_t *g_prCurrent_player;

/**
 *  @brief This struct defines audio ID detail info.
 */
typedef struct {
    bt_sink_srv_am_reg_id_state_t   use;
    struct am_background        *contain_ptr;
} bt_sink_srv_am_aud_id_type_t;
extern bt_sink_srv_am_aud_id_type_t g_rAm_aud_id[AM_REGISTER_ID_TOTAL];

/**
 *  @brief This struct defines message of audio manager detail info.
 */
typedef struct amm {
    bt_sink_srv_am_cb_msg_class_t   cb_msg_id;
    bt_sink_srv_am_module_t         src_mod_id;      /* Source module ID of the message. */
    bt_sink_srv_am_module_t         dest_mod_id;     /* Destination module ID of the message. */
    bt_sink_srv_am_msg_id_t         msg_id;          /* Message identifier */
    bt_sink_srv_am_background_t     background_info; /* background information*/
} bt_sink_srv_am_amm_struct;
#ifdef WIN32_UT
extern bt_sink_srv_am_amm_struct *g_prAmm_current;
#endif
extern bt_sink_srv_am_amm_struct *ptr_callback_amm;

typedef struct {
    uint32_t time_stamp;
    uint32_t sample;
} bt_sink_srv_am_time_report_t;


/**
 * @brief Define for AWS-MCE MAI event.
 */
#define AMI_EVENT_PEQ_REALTIME      (0x00)
#define AMI_EVENT_PEQ_CHANGE_MODE   (0x01)
#define AMI_EVENT_ANC_SYNC          (0x02)
#define AMI_EVENT_ATTACH_INFO       (0x03)
#define AMI_EVENT_ATTACH_NVDM_INFO  (0x04)
typedef uint8_t ami_event_t;

typedef struct AMI_AWS_MCE_PACKET_HDR_s
{
    ami_event_t ami_event;
    uint8_t numSubPkt;
    uint8_t SubPktId;
} AMI_AWS_MCE_PACKET_HDR_t;

typedef struct AMI_AWS_MCE_PEQ_PACKT_s
{
    uint8_t phase_id;
    uint8_t setting_mode;
    uint32_t target_bt_clk;
    union {
        struct {
            uint8_t enable;
            uint8_t sound_mode;
            uint8_t reserved;
        } peq_data_cm;
        struct {
            uint16_t peq_coef_size;
            uint8_t peq_coef[1];
        } peq_data_rt;
    } peq_data;
} AMI_AWS_MCE_PEQ_PACKT_t;

typedef struct AMI_AWS_MCE_ATTACH_PACKT_s
{
#ifdef MTK_ANC_ENABLE
    uint8_t anc_enable;
    uint8_t anc_filter_type;
    int16_t anc_runtime_gain;
#ifdef MTK_ANC_V2
    uint8_t anc_filter_id;
#endif
#endif
#ifdef MTK_PEQ_ENABLE
    uint8_t a2dp_pre_peq_enable;
    uint8_t a2dp_pre_peq_sound_mode;
    uint8_t a2dp_post_peq_enable;
    uint8_t a2dp_post_peq_sound_mode;
#endif
    uint8_t reserved;
} AMI_AWS_MCE_ATTACH_PACKT_t;

#ifdef MTK_PEQ_ENABLE
#define PEQ_ATTACH_NVKEY_MAX    (16)
typedef struct {
    uint16_t nvkey_id;
    uint16_t nvkey_length;
} ami_aws_mce_attach_peq_nvdm_t;

typedef struct AMI_AWS_MCE_ATTACH_NVDM_PACKT_s
{
    uint8_t peq_nvkey_num;
    uint8_t peq_nvkey_slot_num;
    uint16_t peq_nvkey_mask;
    ami_aws_mce_attach_peq_nvdm_t peq_nvdm[PEQ_ATTACH_NVKEY_MAX];
    uint8_t *nvkey_payload;
} PACKED AMI_AWS_MCE_ATTACH_NVDM_PACKT_t;

typedef struct ami_attach_nvdm_ctrl_s
{
    uint8_t *buffer;
    uint16_t buffer_offset;
    uint8_t total_pkt;
    int8_t  pre_pkt;
} ami_attach_nvdm_ctrl_t;
#endif

typedef struct {
    int16_t gain1_compensation;
    int16_t gain2_compensation;
    uint8_t gain1_sample_per_step;
    uint8_t gain2_sample_per_step;
} ami_audio_volume_parameters_nvdm_t;

/**
 *  @brief This enum defines test mode scenario.
 */
typedef enum {
    SIDETONE_TEST_MODE    = 0,
    TEST_MODE_MAX         = 31,
} bt_sink_srv_am_test_mode_scenario_t;


#ifdef __cplusplus
extern "C" {
#endif

#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
/** @brief  This defines the BLE codec callback function prototype.
 *          The user should register a callback function while opening the BLE codec. Please refer to #bt_codec_ble_open().
 *  @param[in] handle is the media handle of the BLE codec.
 *  @param[in] event_id is the value defined in #bt_msg_type_t. This parameter is given by the driver to notify the user of data flow processing behavior.
 */
typedef void (*am_audio_app_callback_t)(audio_sink_srv_am_app_callback_message_t message, audio_app_param_t param);

bt_sink_srv_am_result_t ami_set_app_notify_callback(am_audio_app_callback_t ami_app_callback);

bt_sink_srv_am_result_t ami_set_afe_param (bt_sink_srv_am_stream_type_t stream_type, hal_audio_sampling_rate_t sampling_rate, bool enable);
#endif

/**
 * @brief                   This function is to get current playing audio device.
 * @param[in]               void
 * @return                  Audio type.
 */
extern bt_sink_srv_am_type_t bt_sink_srv_ami_get_current_scenario();

/**
 * @brief                   This function is to get A2DP max volume level.
 * @param[in]               void
 * @return                  Audio volume level.
 */
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_a2dp_max_volume_level();

/**
 * @brief                   This function is to get A2DP default volume level.
 * @param[in]               void
 * @return                  Audio volume level.
 */
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_a2dp_default_volume_level();

/**
 * @brief                   This function is to get VP max volume level.
 * @param[in]               void
 * @return                  Audio volume level.
 */
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_vp_max_volume_level();

/**
 * @brief                   This function is to get VP default volume level.
 * @param[in]               void
 * @return                  Audio volume level.
 */
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_vp_default_volume_level();

/**
 * @brief                   This function is to get lineIN max volume level.
 * @param[in]               void
 * @return                  Audio volume level.
 */
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_lineIN_max_volume_level();

/**
 * @brief                   This function is to get lineIN default volume level.
 * @param[in]               void
 * @return                  Audio volume level.
 */
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_lineIN_default_volume_level();

#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE) || defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_BLE_AUDIO_DONGLE_ENABLE)
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_usb_voice_sw_max_volume_level();
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_usb_voice_sw_default_volume_level();
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_usb_music_sw_max_volume_level();
extern bt_sink_srv_am_volume_level_t bt_sink_srv_ami_get_usb_music_sw_default_volume_level();
#endif

#ifdef MTK_RECORD_ENABLE
/**
 * @brief                   This function is employed to open a new audio handler to get the registered ID.
 * @param[in] pseudo_device_type      Priority level
 * @param[in] handler       Callback function for A.M. notification
 * @return                  A valid registered ID that is 0~(AM_REGISTER_ID_TOTAL-1) on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_id_t ami_audio_open(bt_sink_srv_am_type_t pseudo_device_type,
                                               bt_sink_srv_am_notify_callback handler);

/**
 * @brief                   This function is employed to play the specified audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] capability_t  Representing the audio content format
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t ami_audio_play(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_audio_capability_t *capability_t);

/**
 * @brief                   This function is employed to stop playing the specified audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t ami_audio_stop(bt_sink_srv_am_id_t aud_id);
#endif

/**
 * @brief                   This function is employed to open a new audio handler to get the registered ID.
 * @param[in] priority      Priority level
 * @param[in] handler       Callback function for A.M. notification
 * @return                  A valid registered ID that is 0~(AM_REGISTER_ID_TOTAL-1) on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_id_t bt_sink_srv_ami_audio_open(bt_sink_srv_am_priority_t priority, bt_sink_srv_am_notify_callback handler);

/**
 * @brief                   This function is employed to play the specified audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] capability_t  Representing the audio content format
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_play(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_audio_capability_t *capability_t);

/**
 * @brief                   This function is employed to stop playing the specified audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_stop(bt_sink_srv_am_id_t aud_id);

/**
 * @brief                   This function is employed to close the opened audio handler.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_close(bt_sink_srv_am_id_t aud_id);

/**
 * @brief                   This function is employed to set audio in/out volume.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] volume        Representing the audio volume level
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_volume(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_volume_level_t volume_level, bt_sink_srv_am_stream_type_t in_out);

/**
 * @brief                   This function is employed to mute on audio stream in/out.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] mute          TRUE/FALSE
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_mute(bt_sink_srv_am_id_t aud_id, bool mute, bt_sink_srv_am_stream_type_t in_out);

/**
 * @brief                   This function is employed to set audio device.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] device        HAL_AUDIO_DEVICE_NONE ~ HAL_AUDIO_DEVICE_LINEIN
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_device(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_device_set_t device);

/**
 * @brief                   This function is employed to write data for palyback / Read data for record.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] buffer        Pointer to user's data buffer for writing or reading usage
 * @param[in] data_count    Output data count for writing; recieve data count for reading
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_continue_stream(bt_sink_srv_am_id_t aud_id, void *buffer, uint32_t data_count);

/**
 * @brief                   This function is employed to query available input/output data length.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[out]data_length   Available input/output data length
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_audio_get_stream_length(bt_sink_srv_am_id_t aud_id, uint32_t *data_length, bt_sink_srv_am_stream_type_t in_out);

/**
 * @brief                   This function is employed to set a2dp sink latency available.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] a2dp_sink_latency        Specified a2dp sink latency 0~(MAX)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_set_a2dp_sink_latency(bt_sink_srv_am_a2dp_sink_latency_t a2dp_sink_latency);

/**
 * @brief                   This function is employed to get a2dp sink latency available.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @return                  bt_sink_srv_am_a2dp_sink_latency_t on failure
 */
extern bt_sink_srv_am_a2dp_sink_latency_t bt_sink_srv_ami_get_a2dp_sink_latency(void);

/**
 * @brief                   This function is employed to set bt_information address to notify DSP.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] bt_inf_address        Specified bt_information address
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t bt_sink_srv_ami_set_bt_inf_address(bt_sink_srv_am_bt_audio_param_t bt_inf_address);

/**
 * @brief                   This function is employed to get bt_information address.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @return                  bt_sink_srv_am_bt_audio_param_t
 */
extern bt_sink_srv_am_bt_audio_param_t bt_sink_srv_ami_get_bt_inf_address(void);

/**
 * @brief                   This function is employed to set audio pause for DSP parameter updating.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_set_pause(void);

/**
 * @brief                   This function is employed to set audio resume which was paused for DSP parameter updating.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_set_resume(void);

/**
 * @brief                   This function is employed to get test mode config.
 * @return                  test_mode_config of nvdm
 */
extern bool am_audio_get_test_mode_config(bt_sink_srv_am_test_mode_scenario_t scenario);

/**
 * @brief                   This function is employed to enable side tone.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_side_tone_enable(void);

/**
 * @brief                   The API is use to change the side tone gain value with db
 *
 * @param                   side_tone_gain
 * @param                   scenario
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
bt_sink_srv_am_result_t am_audio_side_tone_set_volume_by_scenario(sidetone_scenario_t scenario, int32_t side_tone_gain);

/**
 * @brief                   The API is use to get the side tone gain value with db
 *
 * @param                   scenario
 * @return                  side tone gain value with db
 */
int32_t am_audio_side_tone_get_volume_by_scenario(sidetone_scenario_t scenario);

/**
 * @brief                   This function is employed to disable side tone.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_side_tone_disable(void);

/**
 * @brief                   This function is employed to suspend DL 1.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_dl_suspend(void);

/**
 * @brief                   This function is employed to resume DL 1.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_dl_resume(void);

/**
 * @brief                   This function is employed to suspend UL 1.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_ul_suspend(void);

/**
 * @brief                   This function is employed to resume UL 1.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_ul_resume(void);

/**
 * @brief                   This function is employed to set parameters to DSP.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_audio_set_feature(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_feature_t *feature_param);
extern bt_sink_srv_am_result_t am_audio_set_feature_ISR(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_feature_t *feature_param);

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
extern void aud_peq_save_misc_param(void);
extern void aud_peq_save_sound_mode(void);
#ifdef SUPPORT_PEQ_NVKEY_UPDATE
extern void aud_peq_chk_nvkey(uint16_t nvkey_id);
extern uint32_t aud_peq_get_changed_nvkey(uint8_t **packet, uint32_t *total_size);
extern uint32_t aud_peq_save_changed_nvkey(AMI_AWS_MCE_ATTACH_NVDM_PACKT_t *attach_nvdm_pkt, uint32_t data_size);
#endif
extern uint32_t aud_peq_get_sound_mode(bt_sink_srv_am_type_t type, uint8_t *peq_info);
extern int32_t aud_peq_get_total_mode(peq_audio_path_id_t audio_path, uint8_t phase_id);
extern uint8_t aud_peq_get_current_sound_mode(peq_audio_path_id_t audio_path, uint8_t phase_id);
extern uint8_t aud_peq_get_peq_status(peq_audio_path_id_t audio_path, uint8_t phase_id);
#endif

/**
 * @brief                   This function is supply to APP get whuich audio channel in DSP.
 * @return                  AUDIO_CHANNEL_L if L_channel, AUDIO_CHANNEL_R if R_channel, AUDIO_CHANNEL_NONE on other setting.
 */
audio_channel_t ami_get_audio_channel(void);

/**
 * @brief                   This function is supply to APP set whuich audio channel in DSP. (including NV key)
 * @param[in]   set_in_channel         AUDIO_CHANNEL_SWAP to set SWAP_LR Channel.
 * @param[in]   set_out_channel        AUDIO_CHANNEL_L to set Left Channel, AUDIO_CHANNEL_R to set Right Channel.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
bt_sink_srv_am_result_t ami_set_audio_channel(audio_channel_t set_in_channel, audio_channel_t set_out_channel, bool rewrite_nv);

/**
 * @brief                   get audio setting,and adapt audio channel HW mode to SW mode
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
bt_sink_srv_am_result_t ami_audio_setting_init(void);

bt_sink_srv_am_result_t ami_set_audio_device(bt_sink_srv_am_stream_type_t in_out, audio_scenario_sel_t Audio_or_Voice, hal_audio_device_t Device, hal_audio_interface_t i2s_interface, bool rewrite_nv);
typedef struct {
    uint8_t mic0_nvkey;
    uint8_t mic1_nvkey;
    uint8_t mic2_nvkey;
    uint8_t mic3_nvkey;
    uint8_t mic4_nvkey;
    uint8_t mic5_nvkey;
} multic_mic_config_param_t;
bt_sink_srv_am_result_t ami_set_audio_device_extend(bt_sink_srv_am_stream_type_t in_out, audio_scenario_sel_t Audio_or_Voice, multic_mic_config_param_t *mic_config, bool rewrite_nv);
void ami_set_audio_mask (uint32_t flag, bool isSet);

#ifdef MTK_PROMPT_SOUND_ENABLE
/**
 * @brief                   This function is prompt_sound_control ami interface which is used to lead VP request calling API
 *                          or calling task event. (APP VP request >> AM Task event >> MP3 Task event)
 * @param[in]   tone_type        MP3 format or WAV format.
 * @param[in]   *tone_buf        Static voice_prompt_data[]
 * @param[in]   tone_size         Voice_prompt_data size
 * @param[in]   sync_time        Sync time for Agent and Partner VP sync
 * @param[out]  callback           Provide APP a callback while the VP was end.
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_prompt_control_play_sync_tone(prompt_control_tone_type_t tone_type,
                                                            uint8_t *tone_buf,
                                                            uint32_t tone_size,
                                                            uint32_t sync_time,
                                                            prompt_control_callback_t callback);

/**
 * @brief                   This function is prompt_sound_control ami interface which is used to lead VP request calling API
 *                          or calling task event. (APP VP request >> AM Task event >> MP3 Task event)
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
extern bt_sink_srv_am_result_t am_prompt_control_stop_tone(void);
#endif

extern bt_sink_srv_am_result_t am_dynamic_change_channel(audio_channel_selection_t change_channel);

//extern void bt_sink_srv_ami_send_amm(bt_sink_srv_am_module_t dest_id, bt_sink_srv_am_module_t src_id, bt_sink_srv_am_cb_msg_class_t cb_msg_id, bt_sink_srv_am_msg_id_t msg_id, bt_sink_srv_am_background_t *background_info);

extern void ami_hal_audio_status_set_running_flag(audio_message_type_t type, bool is_running);
extern bool ami_hal_audio_status_query_running_flag(audio_message_type_t type);
extern bool ami_hal_audio_status_query_running_flag_by_type_list(audio_message_type_t *type_list, uint32_t type_list_num);

extern void am_hfp_ndvc_sent_avc_vol(bt_sink_avc_sync_statys_t sync_status);
#ifdef MTK_AWS_MCE_ENABLE
extern void am_hfp_ndvc_callback(bt_aws_mce_report_info_t *para);
#endif
extern bool get_audio_dump_info_from_nvdm(DSP_PARA_DATADUMP_STRU *p_DumpInfo);
void am_set_audio_output_volume_parameters_to_nvdm(ami_audio_volume_parameters_nvdm_t *volume_param_ptr);
#ifdef AIR_WIRED_AUDIO_ENABLE
void am_set_audio_output_volume_parameters_2_to_nvdm(ami_audio_volume_parameters_nvdm_t *volume_param_ptr);
#endif
void am_load_audio_output_volume_parameters_from_nvdm(void);

extern void bt_sink_srv_ami_send_amm(bt_sink_srv_am_module_t dest_id,
                                     bt_sink_srv_am_module_t src_id,
                                     bt_sink_srv_am_cb_msg_class_t cb_msg_id,
                                     bt_sink_srv_am_msg_id_t msg_id,
                                     bt_sink_srv_am_background_t *background_info,
                                     uint8_t fromISR,
                                     bt_sink_srv_am_amm_struct *pr_Amm);

#ifdef __BT_SINK_SRV_ACF_MODE_SUPPORT__
void bt_sink_srv_set_acf_mode(uint8_t mode);
#endif
#ifdef MTK_AWS_MCE_ENABLE
extern void bt_aws_mce_report_peq_callback(bt_aws_mce_report_info_t *info);
#endif
extern bt_sink_srv_am_result_t bt_sink_srv_aws_mce_ami_data(ami_event_t ami_event, uint8_t *buffer, uint32_t length, bool urgent);
extern bt_status_t bt_event_ami_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);

extern bt_sink_srv_am_result_t ami_audio_power_off_flow(void);

bt_sink_srv_am_background_t *am_get_aud_by_type(bt_sink_srv_am_type_t type);

void audio_set_anc_compensate(bt_sink_srv_am_type_t type, uint32_t event, bt_sink_srv_am_type_t *cur_type);
#ifdef MTK_ANC_ENABLE
void audio_set_anc_compensate_phase2(bt_sink_srv_am_type_t type, uint32_t event);
#endif

#ifdef MTK_VENDOR_STREAM_OUT_VOLUME_TABLE_ENABLE
bt_sink_srv_am_result_t ami_register_stream_out_volume_table(vol_type_t type, int mode, const uint32_t table[][2], size_t table_num);
bt_sink_srv_am_result_t ami_switch_stream_out_volume_table(vol_type_t type, int mode);
bt_sink_srv_am_result_t ami_get_stream_out_volume(vol_type_t type, uint32_t level, uint32_t *digital_gain, uint32_t *analog_gain);
#endif

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
typedef enum {
    EVENT_SET_VENDOREFFECT = 0,
    EVENT_VOLUME_CHANGE = 1,
    EVENT_A2DP_START = 2,
    EVENT_A2DP_STOP = 3,
    EVENT_HFP_START = 4,
    EVENT_HFP_STOP = 5,
    EVENT_VP_START = 6,
    EVENT_VP_STOP = 7,
    EVENT_LINEINPLAYBACK_START = 8,
    EVENT_LINEINPLAYBACK_STOP = 9,
    EVENT_LINEINPLAYBACK_SUSPEND = 10,
    EVENT_LINEINPLAYBACK_REJECT = 11,
    EVENT_RECORD_START = 12,
    EVENT_RECORD_STOP = 13,
    EVENT_ANC_START = 14,
    EVENT_ANC_STOP = 15,
    EVENT_USB_AUDIO_START = 16,
    EVENT_USB_AUDIO_STOP = 17,
    EVENT_BLE_START = 18,
    EVENT_BLE_STOP = 19,
    EVENT_TRANSMITTER_START = 20,
    EVENT_TRANSMITTER_STOP = 21,
    EVENT_SIDETONE_START = 22,
    EVENT_SIDETONE_STOP = 23
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
    EVENT_AFTER_A2DP_START,
    EVENT_BEFORE_A2DP_STOP,
    EVENT_AFTER_HFP_START,
    EVENT_BEFORE_HFP_STOP,
    EVENT_AFTER_LINEINPLAYBACK_START,
    EVENT_BEFORE_LINEINPLAYBACK_STOP,
    /* aliases of predefined events */
    EVENT_BEFORE_A2DP_START             = EVENT_A2DP_START,
    EVENT_AFTER_A2DP_STOP               = EVENT_A2DP_STOP,
    EVENT_BEFORE_HFP_START              = EVENT_HFP_START,
    EVENT_AFTER_HFP_STOP                = EVENT_HFP_STOP,
    EVENT_BEFORE_LINEINPLAYBACK_START   = EVENT_LINEINPLAYBACK_START,
    EVENT_AFTER_LINEINPLAYBACK_STOP     = EVENT_LINEINPLAYBACK_STOP,
#endif
} vendor_se_event_t;

typedef void (*am_vendor_se_callback_t)(vendor_se_event_t event, void *arg);
typedef int32_t am_vendor_se_id_t;

#define AM_VENDOR_SE_MAX 4

typedef struct {
    bool is_used;
    bool is_vendor_se_set[AM_VENDOR_SE_MAX];
    void *vendor_se_arg[AM_VENDOR_SE_MAX];
    am_vendor_se_callback_t am_vendor_se_callback;
} vendor_se_context_t;

am_vendor_se_id_t ami_get_vendor_se_id(void);
bt_sink_srv_am_result_t ami_register_vendor_se(am_vendor_se_id_t id, am_vendor_se_callback_t callback);
bt_sink_srv_am_result_t ami_set_vendor_se(am_vendor_se_id_t id, void *arg);
bt_sink_srv_am_result_t ami_execute_vendor_se(vendor_se_event_t event);
#endif

#if defined(MTK_AUDIO_TRANSMITTER_ENABLE)
bt_sink_srv_am_result_t ami_audio_set_audio_transmitter_config(bt_sink_srv_am_id_t aud_id, bt_sink_srv_am_audio_capability_t *capability_t);
#endif

#ifdef  AIR_AM_DIRECT_EXEC_ENABLE
void am_direct_exec(bt_sink_srv_am_amm_struct *amm_temp_t);
#endif

#ifdef MTK_ANC_ENABLE
#ifdef MTK_ANC_V2
void anc_port_log_info(int32_t index, const char *message, uint32_t arg_cnt, ...);
void anc_port_log_notice(int32_t index, const char *message, uint32_t arg_cnt, ...);
void anc_port_log_error(int32_t index, const char *message, uint32_t arg_cnt, ...);
void anc_port_log_debug(int32_t index, const char *message, uint32_t arg_cnt, ...);
void anc_port_log_MP(uint8_t index, uint8_t arg_cnt, uint8_t value1, uint8_t value2, uint8_t value3);
#endif
#endif

#ifdef AIR_BT_AUDIO_SYNC_ENABLE
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_request_sync(bt_sink_srv_am_id_t aud_id,
                                                           bt_sink_srv_am_audio_sync_capability_t *sync_capability);
void audio_request_sync_hdlr(bt_sink_srv_am_amm_struct *amm_ptr);
void bt_sink_srv_ami_audio_service_hook_callback(audio_message_type_t type, hal_audio_callback_t callback, void *user_data);
void bt_sink_srv_ami_audio_service_unhook_callback(audio_message_type_t type);
#endif

#ifdef __cplusplus
}
#endif

extern bt_bd_addr_t g_int_dev_addr;

#ifdef AIR_SILENCE_DETECTION_ENABLE
extern void audio_silence_detection_nvdm_ccni(void);
extern void audio_silence_detection_callback_register(silence_detection_callback_t FunPtr);
extern U32 audio_silence_detection_get_detect_time_s(void);
#endif
void audio_driver_configure_set_hold_amp_gpio(uint32_t is_hold);


#ifdef AIR_AUDIO_MIXER_GAIN_ENABLE
/**
 * @brief                   This function is employed to set audio in/out volume.
 *                          About input/output, depend on definition of configuration am_stream_type_t.
 * @param[in] aud_id        Specified audio ID 0~(AM_REGISTER_ID_TOTAL-1)
 * @param[in] volume        Representing the audio volume level
 * @param[in] in_out        STREAM_IN / STREAM_OUT
 * @return                  AUD_EXECUTION_SUCCESS on success or AUD_EXECUTION_FAIL on failure
 */
bt_sink_srv_am_result_t bt_sink_srv_ami_audio_set_mixer_volume(bt_sink_srv_am_volume_level_t volume_level);
#endif

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
typedef struct {
    uint32_t    uplink_bitrate; /* unit: bps, only support 32000 and 50133 */
} ull_misc_nvkey_t;

uint32_t audio_ull_get_uplink_bitrate(void);
#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */

bt_sink_srv_am_result_t bt_sink_srv_ami_audio_power_off_dac_immediately(void);

#endif /*__AM_INTERFACE_H__*/

