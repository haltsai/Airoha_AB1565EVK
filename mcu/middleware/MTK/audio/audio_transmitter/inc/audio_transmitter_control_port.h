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
#ifndef __AUDIO_TRANSMITTER_CONTROL_PORT_H__
#define __AUDIO_TRANSMITTER_CONTROL_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    AUDIO_TRANSMITTER_A2DP_SOURCE,
    AUDIO_TRANSMITTER_GSENSOR,
    AUDIO_TRANSMITTER_MULTI_MIC_STREAM,
    AUDIO_TRANSMITTER_GAMING_MODE,
    AUDIO_TRANSMITTER_ANC_MONITOR_STREAM,
    AUDIO_TRANSMITTER_TEST,
    AUDIO_TRANSMITTER_TDM,
    AUDIO_TRANSMITTER_WIRED_AUDIO,
    AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH,
    AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE,
    AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK,
    AUDIO_TRANSMITTER_SCENARIO_TYPE_MAX
} audio_transmitter_scenario_type_t;

typedef enum {
    AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET = 0,
    AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT,
    AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0,
    AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1,
    AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_LINE_OUT,
    AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_LINE_IN,
    AUDIO_TRANSMITTER_GAMING_MODE_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_gamingmode_t;

typedef enum {
    AUDIO_TRANSMITTER_A2DP_SOURCE_WATCH = 0,
    AUDIO_TRANSMITTER_A2DP_SOURCE_LINEIN,
    AUDIO_TRANSMITTER_A2DP_SOURCE_USB,
    AUDIO_TRANSMITTER_A2DP_SOURCE_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_a2dpsource_t;

typedef enum {
    AUDIO_TRANSMITTER_MULTI_MIC_STREAM_FUNCTION_A = 0,
    AUDIO_TRANSMITTER_MULTI_MIC_STREAM_FUNCTION_B,
    AUDIO_TRANSMITTER_MULTI_MIC_STREAM_FUNCTION_C,
    AUDIO_TRANSMITTER_MULTI_MIC_STREAM_FUNCTION_F,
    AUDIO_TRANSMITTER_MULTI_MIC_STREAM_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_multimic_t;

typedef enum {
    AUDIO_TRANSMITTER_GSENSOR_FUNCTION_D = 0,
    AUDIO_TRANSMITTER_GSENSOR_FUNCTION_F,
    AUDIO_TRANSMITTER_GSENSOR_FUNCTION_G,
    AUDIO_TRANSMITTER_GSENSOR_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_gsensor_t;

typedef enum {
    AUDIO_TRANSMITTER_TEST_AUDIO_LOOPBACK = 0,
    AUDIO_TRANSMITTER_TEST_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_test_t;

typedef enum {
    AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT = 0,
    AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0 = 1,
    AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1 = 2,
    AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_OUT = 3,
    AUDIO_TRANSMITTER_WIRED_AUDIO_LINE_IN = 4,
    AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_MASTER = 5,//I2S2->VUL3->DL3->DAC
    AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_IN_SLAVE = 6,//I2S0->VUL3->DL3->DAC
    AUDIO_TRANSMITTER_WIRED_AUDIO_DUAL_CHIP_LINE_OUT_MASTER = 7,//ADC->VUL1->DL4->I2S0
    AUDIO_TRANSMITTER_WIRED_AUDIO_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_wiredaudio_t;

typedef enum {
    AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID = 0,
    AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_advanced_passthrough_t;

typedef enum {
    AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT = 0,
    AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0,
    AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1,
    AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_LINE_OUT,
    AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_LINE_IN,
    AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_bleaudiodongle_t;

typedef enum {
    AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_I2S0_TO_DAC = 0,
    AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_ADC_TO_I2S0,
    AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_LINEIN_TO_I2S2,
    AUDIO_TRANSMITTER_AUDIO_HW_LOOPBACK_SUB_ID_MAX
} audio_transmitter_scenario_sub_id_audio_hw_loopback_t;
/*****************************scenario config************************************************/

#if defined (MTK_MULTI_MIC_STREAM_ENABLE)
#include "hal_audio_internal.h"
#define MAX_MULTI_MIC_STREAM_NUMBER 4
typedef struct {
    hal_audio_device_t audio_device;
    hal_audio_interface_t audio_interface;
} audio_mic_channel_config_t;
typedef struct {
    audio_mic_channel_config_t mic_configs[MAX_MULTI_MIC_STREAM_NUMBER];
    hal_audio_mic_config_t *mic_para;
    uint32_t sampling_rate;
    uint16_t frame_size;
    uint8_t frame_number;
    hal_audio_format_t format;
    bool echo_reference;
} audio_transmitter_multi_mic_stream_config_t;
#endif


#if defined (MTK_AUDIO_LOOPBACK_TEST_ENABLE)
typedef struct {
    hal_audio_device_t audio_device;
    hal_audio_interface_t audio_interface;
} audio_transmitter_audio_loopback_test_config_t;
#endif

typedef union {
#if defined (MTK_AUDIO_LOOPBACK_TEST_ENABLE)
        audio_transmitter_audio_loopback_test_config_t    audio_loopback_test_config;
#endif
    uint32_t reserved;
} audio_transmitter_test_config_t;

#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
typedef struct {
    audio_dsp_codec_type_t codec_type;
    audio_codec_param_t codec_param;
} voice_headset_config_t;
typedef struct {
    audio_dsp_codec_type_t codec_type;
    audio_codec_param_t codec_param;
    audio_dsp_codec_type_t  usb_type;
    audio_codec_param_t     usb_param;
} music_dongle_config_t;
typedef struct {
    audio_dsp_codec_type_t codec_type;
    audio_codec_param_t codec_param;
    audio_dsp_codec_type_t  usb_type;
    audio_codec_param_t     usb_param;
} voice_dongle_config_t;
typedef struct {
    union {
        voice_headset_config_t voice_headset_config;
        music_dongle_config_t music_dongle_config;
        voice_dongle_config_t voice_dongle_config;
    };
    //uint32_t reserved;
} audio_transmitter_gaming_mode_config_t;
#endif

#if defined (AIR_BLE_AUDIO_DONGLE_ENABLE)
typedef struct {
    bool                    test_mode_enable;
    uint32_t                period;
    uint32_t                channel_enable;
    audio_dsp_codec_type_t codec_type;
    audio_codec_param_t codec_param;
    audio_dsp_codec_type_t  usb_type;
    audio_codec_param_t     usb_param;
} music_ble_audio_dongle_config_t;

typedef struct {
    bool                    test_mode_enable;
    uint32_t                period;
    uint32_t                channel_enable;
    audio_dsp_codec_type_t codec_type;
    audio_codec_param_t codec_param;
    audio_dsp_codec_type_t  usb_type;
    audio_codec_param_t     usb_param;
} voice_ble_audio_dongle_config_t;

typedef struct {
    union {
        music_ble_audio_dongle_config_t music_ble_audio_dongle_config;
        voice_ble_audio_dongle_config_t voice_ble_audio_dongle_config;
    };
    //uint32_t reserved;
} audio_transmitter_ble_audio_dongle_config_t;
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

#if defined (AIR_AUDIO_I2S_SLAVE_TDM_ENABLE)
typedef struct {
    hal_audio_memory_t in_memory;
    hal_audio_memory_t out_memory;
    hal_audio_interface_t in_interface;
    hal_audio_interface_t out_interface;
} audio_transmitter_tdm_config_t;
#endif

#if defined (AIR_WIRED_AUDIO_ENABLE)
typedef struct {
    audio_dsp_codec_type_t codec_type;
    audio_codec_param_t codec_param;
    audio_dsp_codec_type_t  usb_codec_type;
    audio_codec_param_t     usb_codec_param;
} usb_out_config_t;
typedef struct {
    union {
        usb_out_config_t usb_out_config;
    };
} audio_transmitter_wired_audio_config_t;

#endif

typedef struct {
    union {

#if defined (MTK_MULTI_MIC_STREAM_ENABLE)
        audio_transmitter_multi_mic_stream_config_t    multi_mic_stream_config;
#endif
#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
        audio_transmitter_gaming_mode_config_t    gaming_mode_config;
#endif
        audio_transmitter_test_config_t    audio_transmitter_test_config;
#if defined (AIR_AUDIO_I2S_SLAVE_TDM_ENABLE)
        audio_transmitter_tdm_config_t    tdm_config;
#endif
#if defined (AIR_BLE_AUDIO_DONGLE_ENABLE)
        audio_transmitter_ble_audio_dongle_config_t    ble_audio_dongle_config;
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
#if defined (AIR_WIRED_AUDIO_ENABLE)
        audio_transmitter_wired_audio_config_t wired_audio_config;
#endif
        uint32_t reserved;
    };
} scenario_config_t;

/*****************************scenario runtime config************************************************/
#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
typedef enum {
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_L = 0,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_R,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_VOICE_DUL,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_L,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_R,
    GAMING_MODE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL,
    GAMING_MODE_CONFIG_OP_MUSIC_MIX,
    GAMING_MODE_CONFIG_OP_MUSIC_UNMIX,
    GAMING_MODE_CONFIG_OP_LATENCY_SWITCH,
    GAMING_MODE_CONFIG_OP_MAX
} gaming_mode_runtime_config_operation_t;

typedef struct {
    uint8_t vol_level_l;
    uint8_t vol_level_r;
    uint8_t vol_ratio;
} vol_level_t;
typedef struct {
    union{
        vol_level_t vol_level;
        int8_t dl_mixer_id;
        uint32_t latency_us;
    };
} audio_transmitter_gaming_mode_runtime_config_t;
#endif

#if defined (AIR_WIRED_AUDIO_ENABLE)
typedef enum {
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_L = 0,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_R,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_VOICE_DUL,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_L,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_R,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_DUL,
    WIRED_AUDIO_CONFIG_OP_MUSIC_MIX,
    WIRED_AUDIO_CONFIG_OP_MUSIC_UNMIX,
    WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_LINEIN,
    WIRED_AUDIO_CONFIG_OP_MAX
} wired_audio_runtime_config_operation_t;

typedef struct {
    uint8_t vol_level_l;
    uint8_t vol_level_r;
    uint8_t vol_ratio;
} wired_audio_vol_level_t;
typedef struct {
    union{
        wired_audio_vol_level_t vol_level;
        int8_t dl_mixer_id;
    };
} audio_transmitter_wired_audio_runtime_config_t;
#endif

#if defined (AIR_BLE_AUDIO_DONGLE_ENABLE)
typedef enum {
    BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_L = 0,
    BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_R,
    BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_VOICE_DUL,
    BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_L,
    BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_R,
    BLE_AUDIO_DONGLE_CONFIG_OP_VOL_LEVEL_MUSIC_DUL,
    BLE_AUDIO_DONGLE_CONFIG_OP_MUSIC_MIX,
    BLE_AUDIO_DONGLE_CONFIG_OP_MUSIC_UNMIX,
    BLE_AUDIO_DONGLE_CONFIG_OP_LATENCY_SWITCH,
    BLE_AUDIO_DONGLE_CONFIG_OP_GET_DL_CH1_SHARE_INFO,
    BLE_AUDIO_DONGLE_CONFIG_OP_GET_DL_CH2_SHARE_INFO,
    BLE_AUDIO_DONGLE_CONFIG_OP_GET_UL_CH1_SHARE_INFO,
    BLE_AUDIO_DONGLE_CONFIG_OP_GET_UL_CH2_SHARE_INFO,
    BLE_AUDIO_DONGLE_CONFIG_OP_SET_UL_CH1_INPUT_SOURCE,
    BLE_AUDIO_DONGLE_CONFIG_OP_SET_UL_CH2_INPUT_SOURCE,
    BLE_AUDIO_DONGLE_CONFIG_OP_MAX
} ble_audio_dongle_runtime_config_operation_t;

typedef struct {
    uint8_t vol_level_l;
    uint8_t vol_level_r;
    uint8_t vol_ratio;
} vol_level_t;
typedef struct {
    union{
        vol_level_t vol_level;
        int8_t dl_mixer_id;
        uint32_t latency_us;
        void *share_info;
        uint32_t channel_enable;
    };
} audio_transmitter_ble_audio_dongle_runtime_config_t;
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */

typedef uint32_t audio_transmitter_runtime_config_type_t;
typedef struct {
    union {
#if defined (AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
        audio_transmitter_gaming_mode_runtime_config_t gaming_mode_runtime_config;
#endif
#if defined (AIR_WIRED_AUDIO_ENABLE)
        audio_transmitter_wired_audio_runtime_config_t wired_audio_runtime_config;
#endif
#if defined (AIR_BLE_AUDIO_DONGLE_ENABLE)
        audio_transmitter_ble_audio_dongle_runtime_config_t ble_audio_dongle_runtime_config;
#endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
        uint32_t reserved;
    };
} audio_transmitter_runtime_config_t;

typedef int8_t (*get_runtime_config_t)(uint8_t scenario_type, uint8_t scenario_sub_id, audio_transmitter_runtime_config_type_t runtime_config_type, audio_transmitter_runtime_config_t *runtime_config);
typedef struct {
    get_runtime_config_t get_runtime_config;
} audio_transmitter_runtime_config_handler_t;

typedef void (*state_change_handler_t)(uint8_t scenario_sub_id);
extern state_change_handler_t audio_transmitter_state_change_handler[AUDIO_TRANSMITTER_SCENARIO_TYPE_MAX][5];//AUDIO_TRANSMITTER_STATE_MAX = 5


#ifdef __cplusplus
}
#endif

#endif/*__AUDIO_TRANSMITTER_CONTROL_PORT_H__*/
