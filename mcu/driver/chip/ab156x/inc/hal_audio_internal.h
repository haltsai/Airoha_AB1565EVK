/* Copyright Statement:
 *
 * (C) 2017  Airoha Technology Corp. All rights reserved.
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

#ifndef __HAL_AUDIO_INTERNAL_H__
#define __HAL_AUDIO_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "hal_audio.h"
#include "hal_audio_message_struct.h"

#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#include "hal_dvfs_internal.h"
#endif

#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
#define PACKED __attribute__((packed))
#endif

//==== Definition ====
#define UPPER_BOUND(in,up)      ((in) > (up) ? (up) : (in))
#define LOWER_BOUND(in,lo)      ((in) < (lo) ? (lo) : (in))
#define BOUNDED(in,up,lo)       ((in) > (up) ? (up) : (in) < (lo) ? (lo) : (in))
#define MAXIMUM(a,b)            ((a) > (b) ? (a) : (b))
#define MINIMUM(a,b)            ((a) < (b) ? (a) : (b))
#define FOUR_BYTE_ALIGNED(size) (((size) + 3) & ~0x3)

typedef enum {
#ifdef HAL_DVFS_MODULE_ENABLED
    HAL_AUDIO_DVFS_UNLOCK = HAL_DVFS_UNLOCK,
    HAL_AUDIO_DVFS_LOCK   = HAL_DVFS_LOCK,
#else
    HAL_AUDIO_DVFS_UNLOCK,
    HAL_AUDIO_DVFS_LOCK,
#endif
} hal_audio_dvfs_lock_parameter_t;

typedef enum {
#ifdef HAL_DVFS_MODULE_ENABLED
    HAL_AUDIO_DVFS_DEFAULT_SPEED = HAL_DVFS_HALF_SPEED_52M_W_LDSP,
    HAL_AUDIO_DVFS_MEDIUM_SPEED	 = HAL_DVFS_HALF_SPEED_52M,
    HAL_AUDIO_DVFS_HIGH_SPEED    = HAL_DVFS_FULL_SPEED_104M,
    HAL_AUDIO_DVFS_MAX_SPEED     = HAL_DVFS_HIGH_SPEED_208M,
#else
    HAL_AUDIO_DVFS_DEFAULT_SPEED,
    HAL_AUDIO_DVFS_MEDIUM_SPEED,
    HAL_AUDIO_DVFS_HIGH_SPEED,
    HAL_AUDIO_DVFS_MAX_SPEED,
#endif
} hal_audio_dvfs_speed_t;

#if defined(HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE)
typedef enum {
    INPUT_DIGITAL_GAIN_FOR_DEVICE_0             = 0,            /**< input digital gain for device path.  */
    INPUT_DIGITAL_GAIN_FOR_DEVICE_1             = 1,            /**< input digital gain for device1 path.  */
    INPUT_DIGITAL_GAIN_FOR_DEVICE_2             = 2,            /**< input digital gain for device2 path.  */
    INPUT_DIGITAL_GAIN_FOR_DEVICE_3             = 3,            /**< input digital gain for device3 path.  */
    INPUT_DIGITAL_GAIN_FOR_ECHO_PATH            = 4,            /**< input digital gain for echo path.  */
    INPUT_DIGITAL_GAIN_NUM                      = 5,            /**< Specifies the number of input digital gain .  */
} afe_input_digital_gain_t;

typedef enum {
    INPUT_ANALOG_GAIN_FOR_MIC_L                 = 0,            /**< input analog gain for analog microphone L.  */
    INPUT_ANALOG_GAIN_FOR_MIC_R                 = 1,            /**< input analog gain for analog microphone R.  */
    INPUT_ANALOG_GAIN_NUM                       = 2,            /**< Specifies the number of input analog gain.  */
} afe_input_analog_gain_t;
//== hal_audio.c related ==
typedef struct {
    hal_audio_sampling_rate_t   stream_sampling_rate;                   /**< Specifies the sampling rate of audio data.*/
    hal_audio_bits_per_sample_t stream_bit_rate;                        /**< Specifies the number of bps of audio data.*/
    hal_audio_channel_number_t  stream_channel;                         /**< Specifies the number of channel.*/
    hal_audio_channel_number_t  stream_channel_mode;                    /**< Specifies the mode of channel.*/
    hal_audio_device_t          audio_device;                           /**< Specifies the device.*/
    bool                        mute;                                   /**< Specifies whether the device is mute or not.*/
    uint32_t                    digital_gain_index[INPUT_DIGITAL_GAIN_NUM];   /**< Digital gain index of the audio stream.*/
    uint32_t                    analog_gain_index[INPUT_ANALOG_GAIN_NUM];     /**< Analog gain index of the audio stream.*/
} hal_audio_stream_info_t;
#else
//== hal_audio.c related ==
typedef struct {
    hal_audio_sampling_rate_t   stream_sampling_rate;  /**< Specifies the sampling rate of audio data.*/
    hal_audio_bits_per_sample_t stream_bit_rate;       /**< Specifies the number of bps of audio data.*/
    hal_audio_channel_number_t  stream_channel;        /**< Specifies the number of channel.*/
    hal_audio_channel_number_t  stream_channel_mode;   /**< Specifies the mode of channel.*/
    hal_audio_device_t          audio_device;          /**< Specifies the device.*/
    bool                        mute;                  /**< Specifies whether the device is mute or not.*/
    uint32_t                    digital_gain_index;    /**< Digital gain index of the audio stream.*/
    uint32_t                    analog_gain_index;     /**< Analog gain index of the audio stream.*/
} hal_audio_stream_info_t;
#endif

typedef struct {
    bool                    init;
    void                    *allocated_memory;

    // stream in/out information
    hal_audio_stream_info_t stream_in;
    hal_audio_stream_info_t stream_out;
    hal_audio_stream_info_t stream_out_DL2;
    hal_audio_stream_info_t stream_out_DL3;
} audio_common_t;

typedef struct {
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    /*
        amic type:
            0x0: MEMS,
            0x1: ECM Differential,
            0x2: ECM Single,
    */
    uint8_t amic_type[8];

    /* refer to hal_audio_bias_voltage_t, it depends on the layout of the board */
    uint8_t bias_voltage[5];

    /* refer to hal_audio_bias_selection_t */
    uint8_t bias_select;/***/

    /*
        external bias enable:
            0x0: disable
            0x1: enable
    */
    uint8_t with_external_bias;

    /*
        bias lowpower enable
            0x0: disable
            0x1: enable
    */
    uint8_t with_bias_lowpower;

    /*
        bias 1/2 with LDO 0
            0x0: disable
            0x1: enable
       */
    bool bias1_2_with_LDO0;

    /* refer to hal_audio_dmic_selection_t */
    uint8_t dmic_selection[8];

    /* refer to hal_audio_ul_iir_t */
    uint8_t iir_filter[3];

    /*
        uplink adc mode
            0x0: ACC_10k
            0x1: ACC_20k
            0x2: DCC
    */
    uint8_t ul_adc_mode[8];
#endif

    /* refer to hal_audio_analog_mdoe_t */
    hal_audio_analog_mdoe_t adc_mode;

    /* refer to hal_audio_performance_mode_t */
    hal_audio_performance_mode_t performance;
} hal_audio_mic_config_t;

//== hal_audio_dsp_controller.c related ==
typedef struct {
    uint32_t mState;
    int32_t  mUserCount;
} audiosys_clkmux_control;

typedef enum {
    AUDIO_CLKMUX_BLOCK_INTERNAL_BUS = 0,
    AUDIO_CLKMUX_BLOCK_DOWNLINK_HIRES,
    AUDIO_CLKMUX_BLOCK_UPLINK_HIRES,
    AUDIO_CLKMUX_BLOCK_HW_SRC,
    AUDIO_CLKMUX_BLOCK_NUM_OF_CLKMUX_BLOCK,
} audio_clkmux_block_t;

/** @brief audio structure */
#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
typedef enum {
    AU_DSP_AUDIO      = 0,
    AU_DSP_VOICE      = 1,
    AU_DSP_ANC        = 2,
    AU_DSP_RECORD     = 3,
    AU_DSP_VAD_PHASE0 = 4,
    AU_DSP_VAD_PHASE1 = 5,
    AU_DSP_LINEIN     = 6,
} audio_scenario_sel_t;
typedef enum {
    AU_DSP_CH_LR = 0,       /**< */
    AU_DSP_CH_L,            /**< */
    AU_DSP_CH_R,            /**< */
    AU_DSP_CH_SWAP,         /**< */
    AU_DSP_CH_MIX,
    AU_DSP_CH_MIX_SHIFT,
} audio_channel_sel_t;

typedef enum {
    SDK_NONE   = 0x0000,
    SDK_V1p3   = 0x0001,
    SDK_V1p4   = 0x0002,
} audio_version_t;

typedef struct HAL_AUDIO_CH_SEL_HW_MODE_s
{
    uint8_t                     audioChannelGPIOH;      /**< Channel select when GPIO is high. 0:None, 1:L CH, 2:R CH, 3:Two CH*/
    uint8_t                     audioChannelGPIOL;      /**< Channel select when GPIO is low.  0:None, 1:L CH, 2:R CH, 3:Two CH*/
    uint8_t                     gpioIndex;              /**< GPIO index for audio channel select in HW mode. */
} PACKED HAL_AUDIO_CH_SEL_HW_MODE_t;

typedef struct HAL_AUDIO_CHANNEL_SELECT_s
{
    uint8_t                     modeForAudioChannel;    /**< NVkey_0     Channel select mode. 0:SW_mode, 1: HW_mode */
    uint8_t                     audioChannel;           /**< NVkey_1     Channel select. 0:None, 1:L CH, 2:R CH, 3:Two CH */
    HAL_AUDIO_CH_SEL_HW_MODE_t  hwAudioChannel;         /**< NVkey_2-4  Channel select param in HW_Mode.*/
} PACKED HAL_AUDIO_CHANNEL_SELECT_t;

/*############################################Audio part######################################*/
typedef struct HAL_DSP_PARA_AU_AFE_AUDIO_SCENARIO_s {
    uint8_t Audio_Linein_Input_Path; //0x0: Line-In,0x1: I2S_Master_In,0x2: I2S_Slave_In
    uint8_t Audio_A2DP_Output_Path; //0x0: Analog_SPK_Out_L,0x1: Analog_SPK_Out_R,0x2: Analog_SPK_Out_DUAL, 0x3: I2S_Master_Out,0x4: I2S_Slave_Out
    uint8_t Audio_Linein_Input_I2S_Interface; //0x0: I2S_0,0x1: I2S_1,0x2: I2S_2,0x3: I2S_3,
    uint8_t Audio_A2DP_Output_I2S_Interface; //0x0: I2S_0,0x1: I2S_1,0x2: I2S_2,0x3: I2S_3,
    uint8_t Audio_I2S_CLK_Src_In_Normal; //0x0: DCXO,0x1: APLL,
    uint8_t Audio_Analog_LineIn_Performance_Sel; //x0: Normal_Mode,0x1: High_Performance,0x2: Low_Power_mode,0x3: Ultra_Low_Power_mode,0x4: Super_Ultra_Low_Power_mode,
    /*
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,
        could support MAX 8 MIC bias setting
    */
    uint8_t Audio_LineIn_Bias_Enable;
    uint8_t Audio_LineIn_bias_low_power_Enable;

    uint8_t Audio_Linein_Output_Path;//0x0: Analog_SPK_Out_L,0x1: Analog_SPK_Out_R,0x2: Analog_SPK_Out_DUAL, 0x3: I2S_Master_Out,0x4: I2S_Slave_Out
    uint8_t Audio_Linein_Output_I2S_Interface; //0x0: I2S_0,0x1: I2S_1,0x2: I2S_2,0x3: I2S_3,
    uint8_t Audio_Pure_Linein_enable; //0x0:disable,0x1,enable
    /*
        0, by scenario
        1, fix 48000Hz
        2, fix 96000Hz
    */
    uint8_t Audio_I2S0_Master_Sampling_Rate;
    uint8_t Audio_I2S1_Master_Sampling_Rate;
    uint8_t Audio_I2S2_Master_Sampling_Rate;
    uint8_t Audio_I2S3_Master_Sampling_Rate;

    uint8_t Audio_Reserved[2];
}PACKED HAL_DSP_PARA_AU_AFE_AUDIO_SCENARIO_t;


/*############################################Voice part#############################################*/
typedef struct HAL_DSP_PARA_AU_AFE_VOICE_SCENARIO_s {
    uint8_t Voice_Input_Path; //0x0: Analog Mic,0x1: Digital Mic,0x2: I2S_Master_In,0x3: I2S_Slave_In,0x4: Multiple_Mic_Configuration
    uint8_t Voice_Output_Path; //0x0: Analog_SPK_Out_L,0x1: Analog_SPK_Out_R,0x2: Analog_SPK_Out_DUAL,0x3: I2S_Master_Out,0x4: I2S_Slave_Out
    uint8_t Voice_Input_I2S_Interface; //0x0: I2S_0,0x1: I2S_1,0x2: I2S_2,0x3: I2S_3,
    uint8_t Voice_Output_I2S_Interface; //0x0: I2S_0,0x1: I2S_1,0x2: I2S_2,0x3: I2S_3,
    uint8_t Voice_I2S_CLK_Src_In_Normal; //0x0: DCXO,0x1: APLL,
    uint8_t Voice_Input_DMIC_channel; //0x0: Mic_L,0x1: Mic_R,0x2: Mic_DUAL,0x3: None
    uint8_t Voice_Input_AMIC_channel; //0x0: Mic_L,0x1: Mic_R,0x2: Mic_DUAL,0x3: None
    uint8_t Voice_Analog_ADC_Performance_Sel;//x0: Normal_Mode,0x1: High_Performance,0x2: Low_Power_mode,0x3: Ultra_Low_Power_mode,0x4: Super_Ultra_Low_Power_mode,
    uint8_t Voice_Analog_MIC_Sel; //0x0: 0x0: Analog_Mic0 (AU_VIN0 & AU_VIN1),0x1: Analog_Mic1 (AU_VIN2 & AU_VIN3),0x2: Analog_Mic2 (AU_VIN4 & AU_VIN5),
    uint8_t Voice_Digital_MIC_Sel; //0x0: First_DMIC,0x1: Second_DMIC,0x02:third_DMIC
    /*
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,
        could support MAX 8 MIC bias setting
    */
    uint8_t Voice_MIC_Bias_Enable;
    uint8_t Voice_mic_bias_low_power_Enable;

    uint8_t Voice_Side_Tone_Enable; //0x0: disable,0x1: enable,

     /* 0x00: Analog_Mic0_L,0x01: Analog_Mic0_R,0x02: Analog_Mic1_L,0x03: Analog_Mic1_R,
        0x04: Analog_Mic2_L,0x05: Analog_Mic2_R,0x08: First_DMic_L,0x09: First_DMic_R,
        0x0A: Second_DMic_L,0x0B: Second_DMic_R,0x0C: Third_DMic_L,0x0D: Third_DMic_R,

        0x10: I2S0_Master_L,0x20: I2S0_Master_R,0x30: I2S1_Master_L,0x40: I2S1_Master_R,
        0x50: I2S2_Master_L,0x60: I2S2_Master_R,0x80: I2S0_Slave_L,0x90: I2S0_Slave_R,
        0xA0: I2S1_Slave_L,0xB0: I2S1_Slave_R, 0xC0: I2S2_Slave_L,0xD0: I2S2_Slave_R,
        0xFF: None.
    */
    uint8_t Voice_Multiple_Mic_Main_Input_Select;
    uint8_t Voice_Multiple_Mic_Ref1_Input_Select;
    uint8_t Voice_Multiple_Mic_Ref2_Input_Select;
    uint8_t Voice_Multiple_Mic_Ref3_Input_Select;
    uint8_t Voice_Multiple_Mic_Ref4_Input_Select;
    uint8_t Voice_Multiple_Mic_Ref5_Input_Select;
    /*
        0, by scenario
        1, fix 48000Hz
        2, fix 96000Hz
    */
    uint8_t Voice_I2S0_Master_Sampling_Rate;
    uint8_t Voice_I2S1_Master_Sampling_Rate;
    uint8_t Voice_I2S2_Master_Sampling_Rate;
    uint8_t Voice_I2S3_Master_Sampling_Rate;
    uint8_t Voice_Reserved[2];
}PACKED HAL_DSP_PARA_AU_AFE_VOICE_SCENARIO_t;

/*############################################Record part############################################*/
typedef struct HAL_DSP_PARA_AU_AFE_RECORD_SCENARIO_s {
     /* 0x00: Analog_Mic0_L,0x01: Analog_Mic0_R,0x02: Analog_Mic1_L,0x03: Analog_Mic1_R,
        0x04: Analog_Mic2_L,0x05: Analog_Mic2_R,0x08: First_DMic_L,0x09: First_DMic_R,
        0x0A: Second_DMic_L,0x0B: Second_DMic_R,0x0C: Third_DMic_L,0x0D: Third_DMic_R,

        0x10: I2S0_Master_L,0x20: I2S0_Master_R,0x30: I2S1_Master_L,0x40: I2S1_Master_R,
        0x50: I2S2_Master_L,0x60: I2S2_Master_R,0x80: I2S0_Slave_L,0x90: I2S0_Slave_R,
        0xA0: I2S1_Slave_L,0xB0: I2S1_Slave_R, 0xC0: I2S2_Slave_L,0xD0: I2S2_Slave_R,
        0xFF: None.
    */
    uint8_t Record_Main_Input_Select;
    uint8_t Record_Ref_Input_Select;
    uint8_t Record_Ref2_Input_Select;
    uint8_t Record_Ref3_Input_Select;
    uint8_t Record_Ref4_Input_Select;
    uint8_t Record_Ref5_Input_Select;

    /*
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,
        could support MAX 8 MIC bias setting*/
    uint8_t Record_MIC_Bias_Enable;

    uint8_t Record_Side_Tone_Enable;  //0x0: disable,0x1: enable,
    uint8_t Record_Analog_ADC_Performance_Sel; //x0: Normal_Mode,0x1: High_Performance,0x2: Low_Power_mode,0x3: Ultra_Low_Power_mode,0x4: Super_Ultra_Low_Power_mode,
    uint8_t Record_Reserved[7];
}PACKED HAL_DSP_PARA_AU_AFE_RECORD_SCENARIO_t;

/*############################################VAD part############################################*/
typedef struct HAL_DSP_PARA_AU_AFE_VAD_SCENARIO_s {
    /*  0x00: Analog_Mic0_L,0x01: Analog_Mic0_R,0x02: Analog_Mic1_L,0x03: Analog_Mic1_R,
        0x04: Analog_Mic2_L,0x05: Analog_Mic2_R,0x08: First_DMic_L,0x09: First_DMic_R,
        0x0A: Second_DMic_L,0x0B: Second_DMic_R,0x0C: Third_DMic_L,0x0D: Third_DMic_R,

        0x10: I2S0_Master_L,0x20: I2S0_Master_R,0x30: I2S1_Master_L,0x40: I2S1_Master_R,
        0x50: I2S2_Master_L,0x60: I2S2_Master_R,0x80: I2S0_Slave_L,0x90: I2S0_Slave_R,
        0xA0: I2S1_Slave_L,0xB0: I2S1_Slave_R, 0xC0: I2S2_Slave_L,0xD0: I2S2_Slave_R,
        0xFF: None.
    */
    uint8_t VAD_phase0_Main_Input_Select;
    uint8_t VAD_phase0_Ref_Input_Select;
    uint8_t VAD_phase1_Main_Input_Select;
    uint8_t VAD_phase1_Ref_Input_Select;

    /*
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,
        could support MAX 8 MIC bias setting*/
    uint8_t VAD_MIC_Bias_Enable;

    uint8_t VAD_Side_Tone_Enable; //0x0:disable,0x1:enable,
    uint8_t VAD_Analog_ADC_Performance_Sel; //x0:Normal_Mode,0x1:High_Performance,0x2:Low_Power_mode,0x3:Ultra_Low_Power_mode,0x4:Super_Ultra_Low_Power_mode,
    uint8_t VAD_Reserved[5];
}PACKED HAL_DSP_PARA_AU_AFE_VAD_SCENARIO_t;

typedef struct HAL_DSP_PARA_AU_AFE_DETACH_MIC_SCENARIO_s {
    /*  0x00: Analog_Mic0_L,0x01: Analog_Mic0_R,0x02: Analog_Mic1_L,0x03: Analog_Mic1_R,
        0x04: Analog_Mic2_L,0x05: Analog_Mic2_R,0x08: First_DMic_L,0x09: First_DMic_R,
        0x0A: Second_DMic_L,0x0B: Second_DMic_R,0x0C: Third_DMic_L,0x0D: Third_DMic_R,

        0x10: I2S0_Master_L,0x20: I2S0_Master_R,0x30: I2S1_Master_L,0x40: I2S1_Master_R,
        0x50: I2S2_Master_L,0x60: I2S2_Master_R,0x80: I2S0_Slave_L,0x90: I2S0_Slave_R,
        0xA0: I2S1_Slave_L,0xB0: I2S1_Slave_R, 0xC0: I2S2_Slave_L,0xD0: I2S2_Slave_R,
        0xFF: None.
    */
    uint8_t Detach_MIC_Select;

    /*
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,
        could support MAX 8 MIC bias setting*/
    uint8_t Detach_MIC_Bias_Enable;

    uint8_t Detach_MIC_Side_Tone_Enable; //0x0:disable,0x1:enable,
    uint8_t Detach_MIC_Analog_ADC_Performance_Sel; //x0:Normal_Mode,0x1:High_Performance,0x2:Low_Power_mode,0x3:Ultra_Low_Power_mode,0x4:Super_Ultra_Low_Power_mode,
    uint8_t Detach_MIC_Reserved[4];
}PACKED HAL_DSP_PARA_AU_AFE_DETACH_MIC_SCENARIO_t;

typedef struct HAL_DSP_PARA_AU_AFE_ANC_SCENARIO_s {
    /*
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,
        could support MAX 8 MIC bias setting*/
    uint8_t ANC_MIC_Bias_Enable;

    uint8_t ANC_MIC_Analog_ADC_Performance_Sel; //x0:Normal_Mode,0x1:High_Performance,0x2:Low_Power_mode,0x3:Ultra_Low_Power_mode,0x4:Super_Ultra_Low_Power_mode,
    uint8_t ANC_Reserved[2];
}PACKED HAL_DSP_PARA_AU_AFE_ANC_SCENARIO_t;

typedef struct HAL_DSP_PARA_AU_AFE_PASSTHROUGH_SCENARIO_s {
    /*
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,
        could support MAX 8 MIC bias setting*/
    uint8_t Passthrough_MIC_Bias_Enable;

    uint8_t Passthrough_MIC_Analog_ADC_Performance_Sel; //x0:Normal_Mode,0x1:High_Performance,0x2:Low_Power_mode,0x3:Ultra_Low_Power_mode,0x4:Super_Ultra_Low_Power_mode,
    uint8_t Passthrough_Reserved[2];
}PACKED HAL_DSP_PARA_AU_AFE_PASSTHROUGH_SCENARIO_t;

/*############################################ADC DAC config part####################################*/
typedef struct HAL_DSP_PARA_AU_AFE_ADC_DAC_CONFIG_s {
    uint8_t ADDA_DAC_Output_Channel; // 0x1: Analog_SPK_Out_L, 0x2: Analog_SPK_Out_R,x3: Analog_SPK_Out_DUAL
    uint8_t ADDA_DAC_Mode_Sel; //0x0: Class_G,0x1: Class_AB,0x2: Class_D,
    uint8_t ADDA_DAC_Class_AB_G_Performance_Sel; //0x0: Normal_Mode,0x1: High_Performance,

    /*  0x0: 1.8V,
        0x1: 1.85V,
        0x2: 1.9V,
        0x3: 2.0V,
        0x4: 2.1V,
        0x5: 2.2V,
        0x6: 2.4V,
        0x7: 2.55V,
    */
    uint8_t ADDA_Voice_Bias4_Level;
    uint8_t ADDA_Voice_Bias3_Level;
    uint8_t ADDA_Voice_Bias2_Level;
    uint8_t ADDA_Voice_Bias1_Level;
    uint8_t ADDA_Voice_Bias0_Level;

    /*  Bias 0 1 2 share with one LDO HW
    */
    uint8_t ADDA_Voice_Bias012_share_LDO;


    /*
        0x0: ACC_10k,
        0x1: ACC_20k,
        0x2: DCC,
    */
    uint8_t ADDA_Analog_MIC0_Mode;

    /*  0x0: MEMS,
        0x1: ECM Differential,
        0x2: ECM Single,
    */
    uint8_t ADDA_Analog_MIC_VIN0_PUD;

    uint8_t ADDA_Analog_MIC1_Mode;
    uint8_t ADDA_Analog_MIC_VIN1_PUD;

    uint8_t ADDA_Analog_MIC2_Mode;
    uint8_t ADDA_Analog_MIC_VIN2_PUD;

    uint8_t ADDA_Analog_MIC3_Mode;
    uint8_t ADDA_Analog_MIC_VIN3_PUD;

    uint8_t ADDA_Analog_MIC4_Mode;
    uint8_t ADDA_Analog_MIC_VIN4_PUD;

    uint8_t ADDA_Analog_MIC5_Mode;
    uint8_t ADDA_Analog_MIC_VIN5_PUD;

    /*  Disable_IIR : 0x0,
        IIR_1HZ_AT_16KHZ : 0x1,
        IIR_3HZ_AT_16KHZ : 0x2,
        IIR_8HZ_AT_16KHZ : 0x3,
        IIR_17HZ_AT_16KHZ : 0x4,
        IIR_25HZ_AT_16KHZ : 0x5,
    */
    uint8_t ADDA_Voice_IIR_Filter;

    /* Disable_IIR : 0x0,
       IIR_5HZ_AT_48KHZ : 0x1,
       IIR_10HZ_AT_48KHZ : 0x2,
       IIR_25HZ_AT_48KHZ : 0x3,
       IIR_50HZ_AT_48KHZ : 0x4,
       IIR_75HZ_AT_48KHZ : 0x5,
    */
    uint8_t ADDA_Audio_IIR_Filter;

    uint8_t ADDA_Reserved[6];
}PACKED HAL_DSP_PARA_AU_AFE_ADC_DAC_CONFIG_t;

/*#########################################Digital Mic config part####################################*/
typedef struct HAL_DSP_PARA_AU_AFE_DMIC_CONFIG_s {
    /*  0x0: GPIO_DMIC0,
        0x1: GPIO_DMIC1,
        0x2: ANA_DMIC0,
        0x3: ANA_DMIC1,
        0x4: ANA_DMIC2,
        0x5: ANA_DMIC3,
        0x6: ANA_DMIC4,
        0x7: ANA_DMIC5,
    */
    uint8_t DMIC_First_Digital_MIC_Pin_Sel;
    uint8_t DMIC_Second_Digital_MIC_Pin_Sel;
    uint8_t DMIC_Third_Digital_MIC_Pin_Sel;

    /*
        0x0: AFE_DMIC_CLOCK_3_25M
        0x1: AFE_DMIC_CLOCK_1_625M
        0x2: AFE_DMIC_CLOCK_812_5K
        0x3: AFE_DMIC_CLOCK_406_25K
    */
    uint8_t DMIC_First_Digital_MIC_Clk_Rate;
    uint8_t DMIC_Second_Digital_MIC_Clk_Rate;
    uint8_t DMIC_Third_Digital_MIC_Clk_Rate;

    uint8_t DMIC_Reserved[3];
}PACKED HAL_DSP_PARA_AU_AFE_DMIC_CONFIG_t;

/*#########################################Sidetone config part#######################################*/
typedef struct HAL_DSP_PARA_AU_AFE_SIDETONE_CONFIG_s {
    /*  0x0: Main_Mic,
        0x1: Multiple_Mic_Ref1_Mic,
        0x2: Multiple_Mic_Ref2_Mic,
        0x3: Multiple_Mic_Ref3_Mic,
        0x4: Multiple_Mic_Ref4_Mic,
        0x5: Multiple_Mic_Ref5_Mic,
    */
    uint8_t SideTone_Source_Sel;

    /*  -24 ~ 12??1dB step
    */
    uint8_t SideTone_Gain;

    uint8_t SideTone_Reserved[6];
}PACKED HAL_DSP_PARA_AU_AFE_SIDETONE_CONFIG_t;

/*#######################################I2S master config part#######################################*/
typedef struct HAL_DSP_PARA_AU_AFE_I2SM_CONFIG_s {
    /*  0x0: Format_Right_Justified,
        0x1: Format_Left_Justified,
        0x2: Format_I2S,
    */
    //uint8_t I2S_Master_Format;
    uint8_t I2S0_Master_Format;
    uint8_t I2S1_Master_Format;
    uint8_t I2S2_Master_Format;
    uint8_t I2S3_Master_Format;

    /*  0x0: 16_bit,
        0x1: 32_bit,
    */
    //uint8_t I2S_Master_Word_length;
    uint8_t I2S0_Master_Word_length;
    uint8_t I2S1_Master_Word_length;
    uint8_t I2S2_Master_Word_length;
    uint8_t I2S3_Master_Word_length;

    /*  0x0: disable,
        0x1: enable,
    */
    //uint8_t I2S_Master_tx_swap;


    /*  0x0: disable,
        0x1: enable,
    */
    //uint8_t I2S_Master_rx_swap;

    /*  0x0: disable,
        0x1: enable,
        0x2: keep_clock,
    */
    //uint8_t I2S_Master_mclk;

    /*
        divider_value,
    */
    //uint8_t I2S_Master_mclk_divider;

    uint8_t I2S_Master_Reserved[4];
}HAL_DSP_PARA_AU_AFE_I2SM_CONFIG_t;

/*#######################################I2S Slave config part#######################################*/
typedef struct HAL_DSP_PARA_AU_AFE_I2SS_CONFIG_s {
    /*  0x0: Format_Right_Justified,
        0x1: Format_Left_Justified,
        0x2: Format_I2S,
    */
    uint8_t I2S_Slave_Format;

    /*  0x0: Disable,
        0x1: TDM_2ch,
        0x2: TDM_4ch,
        0x3: TDM_6ch,
        0x4: TDM_8ch,
    */
    uint8_t I2S_Slave_TDM;

    /*  0x0: 16_bit,
        0x1: 32_bit,
    */
    uint8_t I2S_Slave_Word_length;

    /*  0x0: disable,
        0x1: enable,
    */
    uint8_t I2S_Slave_tx_swap;

    /*  0x0: disable,
        0x1: enable,
    */
    uint8_t I2S_Slave_rx_swap;

    uint8_t I2S_Slave_Reserved[7];
}PACKED HAL_DSP_PARA_AU_AFE_I2SS_CONFIG_t;

typedef struct HAL_DSP_PARA_AU_AFE_CTRL_NEW_s {
    /*commmon hw module config*/
    /*############################################ADC DAC config part####################################*/
     HAL_DSP_PARA_AU_AFE_ADC_DAC_CONFIG_t adc_dac_config;
    /*#########################################Digital Mic config part####################################*/
     HAL_DSP_PARA_AU_AFE_DMIC_CONFIG_t dmic_config;
    /*#########################################Sidetone config part#######################################*/
     HAL_DSP_PARA_AU_AFE_SIDETONE_CONFIG_t sidetone_config;
    /*#######################################I2S master config part#######################################*/
     HAL_DSP_PARA_AU_AFE_I2SM_CONFIG_t I2SM_config;
    /*#######################################I2S Slave config part#######################################*/
     HAL_DSP_PARA_AU_AFE_I2SS_CONFIG_t I2SS_config;
     uint8_t test_mode_config[4];
    /*scenario config*/
    /*############################################ANC part#############################################*/
    HAL_DSP_PARA_AU_AFE_ANC_SCENARIO_t anc_scenario;
    /*############################################PassThrough part#############################################*/
    HAL_DSP_PARA_AU_AFE_PASSTHROUGH_SCENARIO_t passthrough_scenario;
    /*############################################Audio part#############################################*/
     HAL_DSP_PARA_AU_AFE_AUDIO_SCENARIO_t audio_scenario;
    /*############################################Voice part#############################################*/
     HAL_DSP_PARA_AU_AFE_VOICE_SCENARIO_t voice_scenario;
    /*############################################Record part############################################*/
     HAL_DSP_PARA_AU_AFE_RECORD_SCENARIO_t record_scenario;
    /*############################################VAD part############################################*/
     HAL_DSP_PARA_AU_AFE_VAD_SCENARIO_t VAD_scenario;
     /*############################################Detach Mic part############################################*/
    HAL_DSP_PARA_AU_AFE_DETACH_MIC_SCENARIO_t detach_mic_scenario;
    uint8_t reserve_scenario[4];
}PACKED HAL_DSP_PARA_AU_AFE_CTRL_t;

#endif

#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_MICROPHONE
typedef struct {
    bool                            is_modified;
    bool                            echo_path_enabled;
    hal_audio_device_t              audio_device;
    hal_audio_device_t              audio_device1;
    hal_audio_device_t              audio_device2;
    hal_audio_device_t              audio_device3;
    hal_audio_interface_t           audio_interface;
    hal_audio_interface_t           audio_interface1;
    hal_audio_interface_t           audio_interface2;
    hal_audio_interface_t           audio_interface3;
} au_afe_multi_input_instance_param_t,*au_afe_multi_input_instance_param_p;
#endif

typedef struct HAL_AUDIO_DVFS_CLK_SELECT_s
{
    uint8_t HFP_DVFS_CLK; /* NVkey_0 0x00:26MHz 0x01:39MHz 0x02:78MHz 0x03:156MHz */
    uint8_t RESERVED_1;   /* NVkey_1 0x00 */
    uint8_t RESERVED_2;   /* NVkey_2 0x00 */
    uint8_t RESERVED_3;   /* NVkey_3 0x00 */
    uint8_t RESERVED_4;   /* NVkey_4 0x00 */
    uint8_t RESERVED_5;   /* NVkey_5 0x00 */
    uint8_t RESERVED_6;   /* NVkey_6 0x00 */
    uint8_t RESERVED_7;   /* NVkey_7 0x00 */
    uint8_t RESERVED_8;   /* NVkey_8 0x00 */
    uint8_t RESERVED_9;   /* NVkey_9 0x00 */
} PACKED HAL_AUDIO_DVFS_CLK_SELECT_t;


//== Message related ==
#define MSG_TYPE_BASE_MASK              0x0F00
#define MSG_TYPE_SHIFT_BIT              8


//== Callback related ==
typedef void (*hal_audio_callback_t)(hal_audio_event_t event, void *user_data);
typedef void (*hal_bt_audio_dl_open_callback_t)(void);
typedef void (*hal_audio_notify_task_callback_t)(void);
typedef void (*hal_audio_task_ms_delay_function_t)(uint32_t ms);

//== Ring buffer ==
typedef struct {
    uint32_t write_pointer;
    uint32_t read_pointer;
    uint32_t buffer_byte_count;
    uint8_t *buffer_base_pointer;
} ring_buffer_information_t;

//== AWS related ==
// ToDo: workaround. Not necessary
#define HAL_AUDIO_AWS_NORMAL        0
#define HAL_AUDIO_AWS_NOT_SUPPORT  -1
#define HAL_AUDIO_AWS_ERROR        -2

typedef enum {
    AWS_CODEC_TYPE_AAC_FORMAT,
    AWS_CODEC_TYPE_SBC_FORMAT,
    AWS_CODEC_TYPE_MP3_FORMAT,
    AWS_CODEC_TYPE_PCM_FORMAT
} aws_codec_type_t;

typedef enum {
    AWS_CLOCK_SKEW_STATUS_IDLE,
    AWS_CLOCK_SKEW_STATUS_BUSY
} aws_clock_skew_status_t;

typedef enum {
    CODEC_AWS_CHECK_CLOCK_SKEW,
    CODEC_AWS_CHECK_UNDERFLOW
} aws_event_t;

typedef void (*aws_callback_t)(aws_event_t event, void *user_data);

//== low jitter mode related ==
typedef enum {
    LOW_JITTER_MODE_LINEIN_PLAYBACK_IN,
    LOW_JITTER_MODE_LINEIN_PLAYBACK_OUT,
    LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_IN,
    LOW_JITTER_MODE_PURE_LINEIN_PLAYBACK_OUT,
    LOW_JITTER_MODE_BT_CODEC_A2DP_OUT,
    LOW_JITTER_MODE_HAL_AUDIO_NOTIFY_ANC_AFE_SETTINGS_OUT,
    LOW_JITTER_MODE_MP3_CODEC_PLAY_AVM_OUT,
    LOW_JITTER_MODE_USB_AUDIO_PLAYBACK_OUT,
} low_jitter_mode_scenario_t;

//==== API ====
void hal_audio_dsp_controller_init(void);
void hal_audio_dsp_controller_deinit(void);

void hal_audio_dsp_controller_send_message(uint16_t message, uint16_t data16, uint32_t data32, bool wait);
void *hal_audio_dsp_controller_put_paramter(const void *p_param_addr, uint32_t param_size, audio_message_type_t msg_type);

void hal_audio_dsp_internalbus_clkmux_control(uint32_t device_rate, bool isEnable);
void hal_audio_dsp_hwsrc_clkmux_control(uint32_t device_rate, bool isEnable);
void hal_audio_dsp_dl_clkmux_control(audio_message_type_t type, hal_audio_device_t out_device, uint32_t device_rate, bool isEnable);
void hal_audio_dsp_ul_clkmux_control(hal_audio_device_t out_device, uint32_t device_rate, bool isEnable);
void hal_audio_i2s_master_in_low_jitter_mode(low_jitter_mode_scenario_t scenario, hal_audio_device_t in_device, uint32_t scenario_device_rate, bool is_low_jitter, bool isEnable);
void hal_audio_i2s_master_out_low_jitter_mode(low_jitter_mode_scenario_t scenario, hal_audio_device_t out_device, uint32_t scenario_device_rate, bool is_low_jitter, bool isEnable);

void hal_audio_set_task_notification_callback(hal_audio_notify_task_callback_t callback);
void hal_audio_dsp_message_process(void);
void hal_audio_set_task_ms_delay_function(hal_audio_task_ms_delay_function_t delay_func);
extern uint32_t hal_audio_dsp2mcu_data_get(void);
uint32_t hal_audio_dsp2mcu_AUDIO_DL_ACL_data_get(void);

#ifdef HAL_AUDIO_SUPPORT_MULTIPLE_STREAM_OUT
uint32_t hal_audio_sampling_rate_enum_to_value(hal_audio_sampling_rate_t hal_audio_sampling_rate_enum);
hal_audio_sampling_rate_t hal_audio_sampling_rate_value_to_enum(uint32_t sample_rate);
hal_audio_status_t hal_audio_translate_mic_config(hal_audio_mic_config_t *mic_config, mcu2dsp_open_stream_in_param_t *stream_in_open_param);
hal_audio_status_t hal_audio_get_mic_config(audio_scenario_sel_t Audio_or_Voice, hal_audio_mic_config_t *mic_config);

/**
 * @brief     Get stream in device, MemInterface, i2s_interface from static HW I/O config table.
 * @param[out] Device is a pointer to the audio Device.
 * @param[out] MemInterface is a pointer to the audio MemInterface.
 * @param[out] i2s_interface is a pointer to the audio i2s_interface.
 * @return     #HAL_AUDIO_STATUS_OK, if OK. #HAL_AUDIO_STATUS_ERROR, if wrong.
 */
hal_audio_status_t hal_audio_get_stream_in_setting_config(audio_scenario_sel_t Audio_or_Voice, mcu2dsp_open_stream_in_param_t *stream_in_open_param);

/**
 * @brief     Get stream out device, MemInterface, i2s_interface from static HW I/O config table.
 * @param[out] Device is a pointer to the audio Device.
 * @param[out] MemInterface is a pointer to the audio MemInterface.
 * @param[out] i2s_interface is a pointer to the audio i2s_interface.
 * @return     #HAL_AUDIO_STATUS_OK, if OK. #HAL_AUDIO_STATUS_ERROR, if wrong.
 */
hal_audio_status_t hal_audio_get_stream_out_setting_config(audio_scenario_sel_t Audio_or_Voice, mcu2dsp_open_stream_out_param_t *stream_out_open_param);
#endif

void hal_audio_set_dvfs_control(hal_audio_dvfs_speed_t DVFS_SPEED, hal_audio_dvfs_lock_parameter_t DVFS_lock);

#ifdef HAL_DVFS_MODULE_ENABLED
void hal_audio_set_dvfs_clk(audio_scenario_sel_t Audio_or_Voice, dvfs_frequency_t *dvfs_clk);
#endif

//== Audio Service ==
void hal_audio_service_hook_callback(audio_message_type_t type, hal_audio_callback_t callback, void *user_data);
void hal_audio_service_unhook_callback(audio_message_type_t type);
hal_audio_device_t hal_audio_convert_mic_config(uint8_t Mic_NVkey);
hal_audio_interface_t hal_audio_convert_linein_interface(uint8_t Mic_NVkey,bool is_input_device);

//== Share buffer ==
n9_dsp_share_info_t *hal_audio_query_bt_audio_dl_share_info(void);
n9_dsp_share_info_t *hal_audio_query_bt_voice_ul_share_info(void);
n9_dsp_share_info_t *hal_audio_query_bt_voice_dl_share_info(void);
n9_dsp_share_info_t *hal_audio_query_ble_audio_ul_share_info(void);
n9_dsp_share_info_t *hal_audio_query_ble_audio_dl_share_info(void);
n9_dsp_share_info_t *hal_audio_query_playback_share_info(void);
n9_dsp_share_info_t *hal_audio_query_record_share_info(void);
#ifdef MTK_BT_SPEAKER_ENABLE
uint8_t *hal_audio_query_fec_share_info(void);
#endif
uint32_t *hal_audio_query_rcdc_share_info(void);
uint32_t *hal_audio_query_ull_rcdc_share_info(void);
uint32_t *hal_audio_query_hfp_air_dump(void);
n9_dsp_audio_sync_info_t *hal_audio_query_audio_sync_info(void);
n9_dsp_share_info_t *hal_audio_query_prompt_share_info(void);
n9_dsp_share_info_t *hal_audio_query_nvkey_parameter_share_info(void);
void hal_audio_reset_share_info(n9_dsp_share_info_t *p_info);
void hal_audio_a2dp_reset_share_info(n9_dsp_share_info_t *p_info);
void hal_audio_set_avm_info(avm_share_buf_info_t *p_info, uint32_t buf_len,  uint16_t blksize, uint32_t sink_latnecy);
void hal_audio_set_hfp_avm_info(avm_share_buf_info_t *p_info, uint32_t buf_len,  uint16_t blksize);
void hal_audio_set_forwarder_addr(avm_share_buf_info_t *p_info, bool isRx);

void hal_audio_set_sysram(void);
uint32_t *hal_audio_query_ltcs_asi_buf(void);
uint32_t *hal_audio_query_ltcs_min_gap_buf(void);
uint32_t *hal_audio_query_ltcs_anchor_info_buf(void);
uint32_t *hal_audio_query_race_cmd_audio_buf(void);
uint32_t *hal_audio_report_bitrate_buf(void);
uint32_t *hal_audio_report_lostnum_buf(void);
uint32_t *hal_audio_afe_dl_buf_report(void);
hal_audio_status_t hal_audio_write_audio_drift_val(int32_t val);
hal_audio_status_t hal_audio_write_audio_anchor_clk(uint32_t val);
hal_audio_status_t hal_audio_write_audio_asi_base(uint32_t val);
hal_audio_status_t hal_audio_write_audio_asi_cur(uint32_t val);

//== Buffer management related ==
uint32_t hal_audio_buf_mgm_get_data_byte_count(n9_dsp_share_info_t *p_info);
uint32_t hal_audio_buf_mgm_get_free_byte_count(n9_dsp_share_info_t *p_info);
void hal_audio_buf_mgm_get_free_buffer(
    n9_dsp_share_info_t *p_info,
    uint8_t **pp_buffer,
    uint32_t *p_byte_count);
void hal_audio_buf_mgm_get_data_buffer(
    n9_dsp_share_info_t *p_info,
    uint8_t **pp_buffer,
    uint32_t *p_byte_count);
void hal_audio_buf_mgm_get_write_data_done(n9_dsp_share_info_t *p_info, uint32_t byte_count);
void hal_audio_buf_mgm_get_read_data_done(n9_dsp_share_info_t *p_info, uint32_t byte_count);

//== Status control ==
void hal_audio_status_set_running_flag(audio_message_type_t type, bool is_running);
void hal_audio_status_set_notify_flag(audio_message_type_t type, bool is_notify);
bool hal_audio_status_query_running_flag(audio_message_type_t type);
bool hal_audio_status_query_running_flag_except(audio_message_type_t type);
bool hal_audio_status_query_notify_flag(audio_message_type_t type);
uint16_t hal_audio_status_query_running_flag_value();

//== Data path ==
hal_audio_status_t hal_audio_write_stream_out_by_type(audio_message_type_t type, const void *buffer, uint32_t size);

//== AM A2DP open callback ==
void hal_audio_am_register_a2dp_open_callback(hal_bt_audio_dl_open_callback_t callback);

//== Speech related parameter ==
void speech_update_common(const uint16_t *common);
void speech_update_nb_param(const uint16_t *param);
void speech_update_wb_param(const uint16_t *param);
void speech_update_nb_fir(const int16_t *in_coeff, const int16_t *out_coeff);
void speech_update_wb_fir(const int16_t *in_coeff, const int16_t *out_coeff);

int32_t audio_update_iir_design(const uint32_t *parameter);

//== Ring buffer operation ==
uint32_t ring_buffer_get_data_byte_count(ring_buffer_information_t *p_info);
uint32_t ring_buffer_get_space_byte_count(ring_buffer_information_t *p_info);
void ring_buffer_get_write_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_get_read_information(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_write_done(ring_buffer_information_t *p_info, uint32_t write_byte_count);
void ring_buffer_read_done(ring_buffer_information_t *p_info, uint32_t read_byte_count);

uint32_t ring_buffer_get_data_byte_count_non_mirroring(ring_buffer_information_t *p_info);
uint32_t ring_buffer_get_space_byte_count_non_mirroring(ring_buffer_information_t *p_info);
void ring_buffer_get_write_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_get_read_information_non_mirroring(ring_buffer_information_t *p_info, uint8_t **pp_buffer, uint32_t *p_byte_count);
void ring_buffer_write_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t write_byte_count);
void ring_buffer_read_done_non_mirroring(ring_buffer_information_t *p_info, uint32_t read_byte_count);
cm4_dsp_audio_sync_request_param_t *hal_audio_query_audio_sync_request_param(void);
//== Time report ==
audio_dsp_a2dp_dl_time_param_t *hal_audio_a2dp_dl_get_time_report(void);

#ifdef AIR_AUDIO_DETACHABLE_MIC_ENABLE

typedef enum {
    VOICE_MIC_TYPE_FIXED = 0,
    VOICE_MIC_TYPE_DETACHABLE = 1,
    VOICE_MIC_TYPE_MAX
} voice_mic_type_t;

voice_mic_type_t hal_audio_query_voice_mic_type(void);

#endif

#ifdef __cplusplus
}
#endif

#endif /*__HAL_AUDIO_INTERNAL_H__ */
