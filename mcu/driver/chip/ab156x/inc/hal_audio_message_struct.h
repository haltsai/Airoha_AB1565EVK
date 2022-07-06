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

#ifndef __HAL_AUDIO_MESSAGE_STRUCT_H__
#define __HAL_AUDIO_MESSAGE_STRUCT_H__

#ifdef __cplusplus
extern "C" {
#endif
#define PACKED  __attribute__((packed))
#include <stdint.h>
#include <stdbool.h>
#include "hal_audio.h"

//--------------------------------------------
// Message queue
//--------------------------------------------
#define AUDIO_MESSAGE_QUEUE_SIZE 32

typedef struct {
    uint16_t message16;
    uint16_t data16;
    uint32_t data32;
} audio_message_element_t;

typedef struct {
    uint32_t                read_index;
    uint32_t                write_index;
    audio_message_element_t message[AUDIO_MESSAGE_QUEUE_SIZE];
} audio_message_queue_t;

//--------------------------------------------
// Share buffer information
//--------------------------------------------
typedef enum {
    SHARE_BUFFER_INFO_INDEX_BT_AUDIO_DL,
    SHARE_BUFFER_INFO_INDEX_BT_VOICE_UL,
    SHARE_BUFFER_INFO_INDEX_BT_VOICE_DL,
#ifdef AIR_BT_CODEC_BLE_ENABLED
    SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_UL,
    SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_DL,
    SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_SUB_UL,
    SHARE_BUFFER_INFO_INDEX_BLE_AUDIO_SUB_DL,
#endif
    SHARE_BUFFER_INFO_INDEX_PROMPT,
    SHARE_BUFFER_INFO_INDEX_RECORD,
    SHARE_BUFFER_INFO_INDEX_RINGTONE,
    SHARE_BUFFER_INFO_INDEX_NVKEY_PARAMETER,
    SHARE_BUFFER_INFO_INDEX_MAX
} n9_dsp_share_info_index_t;

typedef enum {
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_A2DP_SOURCE_UL,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_BT_RECEIVE_FROM_AIR,  //AVM buffer
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_BT_SEND_TO_AIR,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_RECEIVE_FROM_MCU_0,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_RECEIVE_FROM_MCU_1,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_DSP_SEND_TO_MCU,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_WIRED_AUDIO_DSP_RECEIVE_FROM_MCU_0,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_WIRED_AUDIO_DSP_RECEIVE_FROM_MCU_1,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_0,  //AVM buffer
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_RECEIVE_FROM_AIR_1,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_0,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_BT_SEND_TO_AIR_1,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_RECEIVE_FROM_MCU_0,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_RECEIVE_FROM_MCU_1,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_BLE_AUDIO_DONGLE_DSP_SEND_TO_MCU,
    AUDIO_TRANSMITTER_SHARE_INFO_INDEX_MAX
} audio_transmitter_share_info_index_t;

// N9-DSP share information buffer (32 bytes)
typedef struct {
    uint16_t block_size;
    uint16_t block_num;
}sub_info_t;
typedef struct {
    uint32_t start_addr;       // start address of N9-DSP share buffer
    uint32_t read_offset;      // read pointer of N9-DSP share buffer
    uint32_t write_offset;     // write pointer of N9-DSP share buffer
    union{
        uint32_t next;             // next read position in buf for DSP
        sub_info_t sub_info;
    };
    uint32_t sampling_rate;    // for AWS clock skew
    uint16_t length;           // total length of N9-DSP share buffer
    uint8_t  bBufferIsFull;    // buffer full flag, when N9 find there is no free buffer for putting a packet,
                               // set this flag = 1, DSP will reset this flag when data be taken by DSP
    uint8_t  notify_count;     // notify count
    int32_t  drift_comp_val;   // long term drift compensation value
    uint32_t anchor_clk;       // long term drift anchor clk
    uint32_t asi_base;
    uint32_t asi_cur;
} n9_dsp_share_info_t;

typedef struct {
    uint8_t  aws_role;
    uint8_t  info_status;
    uint16_t cur_seqn;
    uint32_t cur_asi;
} n9_dsp_audio_sync_info_t;
typedef enum {
    AUDIO_SYNC_GAIN_SELECT_CHANNEL_L = 0,
    AUDIO_SYNC_GAIN_SELECT_CHANNEL_R,
    AUDIO_SYNC_GAIN_SELECT_CHANNEL_DUAL,
} gain_select_t;

typedef struct {
    uint32_t gain;
    gain_select_t gain_select;
} vol_gain_info_t;

typedef struct {
    uint32_t gpt_count;        // the target gpt timer count of sync
    vol_gain_info_t vol_gain_info; // downlink volume level
} cm4_dsp_audio_sync_request_param_t;

typedef struct {
uint32_t StartAddr;       // start address of share buffer
uint16_t ReadIndex;  // read pointer of share buffer  : DSP monitor
uint16_t WriteIndex; // write pointer of share buffer : Controller monitor
uint32_t SampleRate; // sample rate for clock skew counting
uint16_t MemBlkSize; // block length for each frame
uint16_t MemBlkNum;  // number of block for frame usage
uint32_t DbgInfoAddr; // start address of controller packet address table
uint16_t FrameSampleNum;  // DSP notify audio
uint16_t codec_type;      // Codec information
uint16_t codec_config;    // Codec config information
uint16_t NotifyCount;  // notify count of DSP notify controller not to sleep
uint32_t ForwarderAddr; // forwarder buffer address
uint32_t SinkLatency; // a2dp sink latency
uint32_t reserved[1]; // reserved
} avm_share_buf_info_t,*avm_share_buf_info_ptr;
#define AVM_SHAEE_BUF_INFO

#define SHARE_BUFFER_BT_AUDIO_DL_SIZE      (40*1024)
#ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
#define SHARE_BUFFER_VENDOR_CODEC_SIZE (8*1024)
#endif

#define SHARE_BUFFER_BT_VOICE_UL_SIZE      (480) /* 60*4 */
#define SHARE_BUFFER_BT_VOICE_DL_SIZE      (640) /* 60*8 + 20*8 */
#ifdef AIR_BT_CODEC_BLE_ENABLED
#define SHARE_BUFFER_BLE_AUDIO_UL_SIZE     ((28+156)*6) // (28+156) *6
#define SHARE_BUFFER_BLE_AUDIO_DL_SIZE     ((28+156)*6) //(28+156) *6
#define SHARE_BUFFER_BLE_AUDIO_SUB_UL_SIZE     (0) // so far no sub UL
#define SHARE_BUFFER_BLE_AUDIO_SUB_DL_SIZE     ((28+156)*6) //  (28+156) *6
#endif

#if (defined AIR_MP3_DECODER_ENABLE) && (defined AIR_OPUS_DECODER_ENABLE)
#define SHARE_BUFFER_PROMPT_SIZE           (960*2*6)
#elif (defined AIR_MP3_DECODER_ENABLE)
#define SHARE_BUFFER_PROMPT_SIZE           (3*2304)
#else
#define SHARE_BUFFER_PROMPT_SIZE           (960*2*4)
#endif

#define SHARE_BUFFER_RECORD_SIZE           (2304)
#define SHARE_BUFFER_RINGTONE_SIZE         (2*1024)
#ifdef MTK_BT_SPEAKER_ENABLE
#define SHARE_BUFFER_FEC_SIZE              (10*700)
#endif
#define SHARE_BUFFER_MCU2DSP_PARAMETER_SIZE (2*1024)
#define SHARE_BUFFER_CLK_INFO_SIZE          (8)
#define SHARE_BUFFER_ULL_CLK_INFO_SIZE      (8)
#define SHARE_BUFFER_AIRDUMP_SIZE           (400)
#define SHARE_BUFFER_DSP2MCU_PARAMETER_SIZE (1*1024-SHARE_BUFFER_CLK_INFO_SIZE-SHARE_BUFFER_AIRDUMP_SIZE)
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR) || defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE) || defined(AIR_BLE_AUDIO_DONGLE_ENABLE) || defined(AIR_HFP_DNN_PATH_ENABLE)
#define SHARE_BUFFER_NVKEY_PARAMETER_SIZE   (4*1024+2048)
#else
#define SHARE_BUFFER_NVKEY_PARAMETER_SIZE   (6*1024)
#endif
#define SHARE_BUFFER_AUDIO_SYNC_INFO_SIZE   (sizeof(n9_dsp_audio_sync_info_t))
#define SHARE_BUFFER_RX_AUDIO_FORWARDER_BUF_SIZE (760)
#define SHARE_BUFFER_TX_AUDIO_FORWARDER_BUF_SIZE (720)
#define SHARE_BUFFER_SYSRAM3_RX_AUDIO_FORWARDER_BUF_SIZE (280)
#define SHARE_BUFFER_SYSRAM3_TX_AUDIO_FORWARDER_BUF_SIZE (240)
#define SHARE_BUFFER_SYSRAM3_AUDIO_FORWARDER_BUF_SIZE (SHARE_BUFFER_SYSRAM3_RX_AUDIO_FORWARDER_BUF_SIZE + SHARE_BUFFER_SYSRAM3_TX_AUDIO_FORWARDER_BUF_SIZE)
#define SHARE_BUFFER_GAMING_MODE_SIZE       (10*1024)
#define SHARE_BUFFER_AUDIO_SYNC_REQUEST_PARAM_SIZE  (sizeof(cm4_dsp_audio_sync_request_param_t))
#ifdef AIR_WIRED_AUDIO_ENABLE
#define SHARE_BUFFER_BT_AUDIO_USB_IN_SIZE   (5*1024)
#endif
#define SHARE_BUFFER_RACE_CMD_AUDIO_BUF_SIZE (128)
#if defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
#define SHARE_BUFFER_ANC_MONITOR_INFO_COUNT 5 //max usage: UU gain info = uint32_t *5 = 20B
#endif

typedef struct {
    uint32_t bt_audio_dl[(SHARE_BUFFER_BT_AUDIO_DL_SIZE+3)/4];
    uint32_t bt_voice_ul[(SHARE_BUFFER_BT_VOICE_UL_SIZE+3)/4];
    uint32_t bt_voice_dl[(SHARE_BUFFER_BT_VOICE_DL_SIZE+3)/4];
    uint32_t prompt[(SHARE_BUFFER_PROMPT_SIZE+3)/4];
    uint32_t record[(SHARE_BUFFER_RECORD_SIZE+3)/4];
    uint32_t ringtone[(SHARE_BUFFER_RINGTONE_SIZE+3)/4];
    uint32_t mcu2dsp_param[(SHARE_BUFFER_MCU2DSP_PARAMETER_SIZE+3)/4];
    uint32_t dsp2mcu_param[(SHARE_BUFFER_DSP2MCU_PARAMETER_SIZE+3)/4];
    uint32_t clk_info[(SHARE_BUFFER_CLK_INFO_SIZE+3)/4];
    uint32_t ull_clk_info[(SHARE_BUFFER_ULL_CLK_INFO_SIZE+3)/4];
    uint32_t airdump[(SHARE_BUFFER_AIRDUMP_SIZE+3)/4];
    uint32_t nvkey_param[(SHARE_BUFFER_NVKEY_PARAMETER_SIZE+3)/4];
    uint32_t audio_sync_info[(SHARE_BUFFER_AUDIO_SYNC_INFO_SIZE+3)/4];
    uint32_t rx_audio_forwarder_buf[(SHARE_BUFFER_RX_AUDIO_FORWARDER_BUF_SIZE+3)/4];
    uint32_t tx_audio_forwarder_buf[(SHARE_BUFFER_TX_AUDIO_FORWARDER_BUF_SIZE+3)/4];
    uint32_t audio_sync_request_parm[(SHARE_BUFFER_AUDIO_SYNC_REQUEST_PARAM_SIZE + 3) / 4];
#ifdef AIR_WIRED_AUDIO_ENABLE
    uint32_t usb_in_0[(SHARE_BUFFER_BT_AUDIO_USB_IN_SIZE+3)/4];
    uint32_t usb_in_1[(SHARE_BUFFER_BT_AUDIO_USB_IN_SIZE+3)/4];
#endif
    uint32_t race_cmd_audio_buf[(SHARE_BUFFER_RACE_CMD_AUDIO_BUF_SIZE + 3) / 4];
} audio_share_buffer_t;
n9_dsp_share_info_t *hal_audio_query_audio_transmitter_share_info(audio_transmitter_share_info_index_t index);
#ifdef AVM_SHAEE_BUF_INFO
void hal_audio_set_gaming_mode_avm_info(avm_share_buf_info_t *p_info, uint32_t buf_len,  uint16_t blksize);
#endif
void hal_audio_set_audio_transmitter_share_info(n9_dsp_share_info_t *p_share_info, uint32_t buf_len, uint32_t max_payload_size);

#ifdef AIR_BT_CODEC_BLE_ENABLED
typedef struct {
    uint32_t *ble_audio_ul;
    uint32_t *ble_audio_dl;
    uint32_t *ble_audio_sub_ul;
    uint32_t *ble_audio_sub_dl;
} leaudio_share_buffer_t;
#endif

//--------------------------------------------
// Error report
//--------------------------------------------
typedef enum {
    DSP_ERROR_REPORT_ERROR,
    DSP_ERROR_REPORT_END,
} dsp_error_report_t;

//--------------------------------------------
// Codec type
//--------------------------------------------
typedef enum {
    AUDIO_DSP_CODEC_TYPE_CVSD = 0,
    AUDIO_DSP_CODEC_TYPE_MSBC,
    AUDIO_DSP_CODEC_TYPE_DNN, // for DNN loopback
    AUDIO_DSP_CODEC_TYPE_LC3,

    AUDIO_DSP_CODEC_TYPE_PCM = 0x100,
    AUDIO_DSP_CODEC_TYPE_SBC,
    AUDIO_DSP_CODEC_TYPE_MP3,
    AUDIO_DSP_CODEC_TYPE_AAC,
    AUDIO_DSP_CODEC_TYPE_VENDOR,
    AUDIO_DSP_CODEC_TYPE_OPUS,
    AUDIO_DSP_CODEC_TYPE_ANC_LC, //for leakage compensation
    AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_SZ,
    AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ,
    AUDIO_DSP_CODEC_TYPE_ANC_USER_TRIGGER_FF_PZ_FIR,
    AUDIO_DSP_CODEC_TYPE_PCM_WWE,
} audio_dsp_codec_type_t;

typedef enum {
    WWE_MODE_NONE =    0,
    WWE_MODE_AMA =     1,
    WWE_MODE_GSOUND =   2,
    WWE_MODE_VENDOR1 = 3,
    WWE_MODE_MAX =     4
} wwe_mode_t;

typedef enum
{
    ENCODER_BITRATE_16KBPS  = 16,
    ENCODER_BITRATE_32KBPS  = 32,
    ENCODER_BITRATE_64KBPS  = 64,
    ENCODER_BITRATE_MAX     = 0xFFFFFFFF,
} encoder_bitrate_t;

//--------------------------------------------
// Start Parameters
//--------------------------------------------
typedef struct {
    uint32_t                    start_time_stamp;
    uint32_t                    time_stamp_ratio;
    uint32_t                    start_asi;
    uint32_t                    start_bt_clk;
    uint32_t                    start_bt_intra_clk;
    bool                        content_protection_exist;
    bool                        alc_enable;
    bool                        latency_monitor_enable;
} audio_dsp_a2dp_dl_start_param_t, *audio_dsp_a2dp_dl_start_param_p;

typedef struct {
    hal_audio_bits_per_sample_t bit_type;
    uint32_t                    sampling_rate;
    uint32_t                    channel_number;
} audio_dsp_file_pcm_param_t;

typedef struct {
    n9_dsp_share_info_t         *p_share_info;
    hal_audio_channel_number_t  channel_number;
    hal_audio_bits_per_sample_t bit_type;
    hal_audio_sampling_rate_t   sampling_rate;
    uint8_t                     codec_type;  //KH: should use audio_dsp_codec_type_t
    uint8_t                     dsp_local_data_index;   //VP_dummy_source
    uint8_t                     dsp_local_stream_mode;  //VP_dummy_source
} audio_dsp_playback_info_t, *audio_dsp_playback_info_p;

typedef struct {
    uint32_t                    header;
} audio_dsp_file_mp3_param_t;

typedef struct {
    uint32_t                    header;
} audio_dsp_file_aac_param_t;

typedef struct {
    uint32_t                    header;
} audio_dsp_file_vendor_param_t;

//===CM4 to DSP message structure==
/* Open message member parameter structure */
typedef enum {
    STREAM_IN_AFE  = 0,
    STREAM_IN_HFP,
    STREAM_IN_BLE,
    STREAM_IN_A2DP,
    STREAM_IN_PLAYBACK,
    STREAM_IN_VP,
    STREAM_IN_VP_DUMMY_SOURCE,
    STREAM_IN_GSENSOR,
    STREAM_IN_AUDIO_TRANSMITTER,
    STREAM_IN_BT_COMMON,
    STREAM_IN_DUMMY = 0xFFFFFFFF,
} mcu2dsp_stream_in_selection;

typedef enum {
    STREAM_OUT_AFE  = 0,
    STREAM_OUT_HFP,
    STREAM_OUT_BLE,
    STREAM_OUT_RECORD,
    STREAM_OUT_VIRTUAL,
    STREAM_OUT_AUDIO_TRANSMITTER,
    STREAM_OUT_BT_COMMON,
    STREAM_OUT_DUMMY = 0xFFFFFFFF,
} mcu2dsp_stream_out_selection;

typedef struct {
    mcu2dsp_stream_in_selection     stream_in;
    mcu2dsp_stream_out_selection    stream_out;
    uint32_t                        *Feature;
}  mcu2dsp_param_t, *mcu2dsp_param_p;

#ifdef ENABLE_HWSRC_CLKSKEW
//--------------------------------------------
// CLK SKEW Mode
//--------------------------------------------
typedef enum {
    CLK_SKEW_V1 = 0, /* sw clk skew */
    CLK_SKEW_V2 = 1, /* hwsrc clk skew */
    CLK_SKEW_DUMMY = 0xFFFFFFFF,
} clkskew_mode_t;
#endif

typedef struct {
    hal_audio_device_t               audio_device;
    hal_audio_device_t               audio_device1;
    hal_audio_device_t               audio_device2;
    hal_audio_device_t               audio_device3;
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    hal_audio_device_t               audio_device4;
    hal_audio_device_t               audio_device5;
    hal_audio_device_t               audio_device6;
    hal_audio_device_t               audio_device7;
#endif
    hal_audio_channel_selection_t    stream_channel;
    hal_audio_memory_t                      memory;
    hal_audio_interface_t                   audio_interface;
#ifdef ENABLE_2A2D_TEST
    hal_audio_interface_t                   audio_interface1;
    hal_audio_interface_t                   audio_interface2;
    hal_audio_interface_t                   audio_interface3;
#endif

#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
#ifdef ENABLE_2A2D_TEST
    hal_audio_interface_t                   audio_interface4;
    hal_audio_interface_t                   audio_interface5;
    hal_audio_interface_t                   audio_interface6;
    hal_audio_interface_t                   audio_interface7;
#endif

    /*uplink adc mode:
        0x0: ACC_10k,
        0x1: ACC_20k,
        0x2: DCC,
    */
    uint8_t                                 ul_adc_mode[8];
    /*amic type
        0x0: MEMS,
        0x1: ECM Differential,
        0x2: ECM Single,
    */
    uint8_t                                 amic_type[8];
    /*downlink dac mode
        0x0: Class_G,
        0x1: Class_AB,
        0x2: Class_D,
    */
    uint8_t                                 dl_dac_mode;
    /*  uplink:
        0x0: Normal_Mode,
        0x1: High_Performance,
        0x2: Low_Power_mode,
        0x3: Ultra_Low_Power_mode,
        0x4: Super_Ultra_Low_Power_mode,
        downlink:
        0x0: Class_G,
        0x1: Class_AB,
        0x2: Class_D,
        */
    //uint8_t                                 performance;/**hal_audio_analog_mdoe_t*/

    /*bias voltage,support 5 bia voltage
        0x0: 1.8V,
        0x1: 1.85V,
        0x2: 1.9V,
        0x3: 2.0V,
        0x4: 2.1V,
        0x5: 2.2V,
        0x6: 2.4V,
        0x7: 2.55V */
    uint8_t                                 bias_voltage[5];/**hal_audio_bias_voltage_t*/

    /*bias enable
        bit mask to enable Bias
        8'b 00000001: Bias 0,
        8'b 00000010: Bias 1,
        8'b 00000100: Bias 2,
        8'b 00001000: Bias 3,
        8'b 00010000: Bias 4,*/
    uint8_t                                 bias_select;/**hal_audio_bias_selection_t*/
    /*external bias enable*/
    uint8_t                                 with_external_bias;/**< for with_external_bias */
    /*bias lowpower enable*/
    uint8_t                                 with_bias_lowpower;/**< for with_bias_lowpower */
    /*define if bias1_2 config with LDO0 */
    bool                                    bias1_2_with_LDO0;/**< for bias1_2_with_LDO0 */
    /*DMIC select*/
    uint8_t                                 dmic_selection[8];/**< hal_audio_dmic_selection_t */
    /*iir filter*/
    uint8_t                                 iir_filter[3];/**hal_audio_ul_iir_t*/
    /*I2S format*/
    uint8_t                                 i2s_format;/**< hal_audio_i2s_format_t */
    /*I2S slave TDM*/
    uint8_t                                 i2S_Slave_TDM;
    /*I2S word length*/
    uint8_t                                 i2s_word_length;/**< hal_audio_i2s_word_length_t */
    /*I2S low jitter config
        false, DCXO
        true,  APLL*/
    bool                                    is_low_jitter;
#endif

    hal_audio_format_t                      format;
    uint32_t                                misc_parms;
    uint32_t                                sampling_rate;
    uint32_t                                stream_out_sampling_rate;
    uint16_t                                frame_size;
    uint8_t                                 frame_number;
    float                                   irq_period;
    uint8_t                                 sw_channels;
    bool                                    hw_gain;
    hal_audio_analog_mdoe_t                 adc_mode;
    hal_audio_performance_mode_t            performance;
#ifdef ENABLE_HWSRC_CLKSKEW
    clkskew_mode_t                          clkskew_mode;
#endif
#ifdef AIR_HFP_DNN_PATH_ENABLE
    bool                                    enable_ul_dnn;
#endif
#ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
    /*DMIC clock(reserve)*/
    uint8_t                                 dmic_clock_rate[3]; /*afe_dmic_clock_rate_t*/
#endif
    bool                                    with_upwdown_sampler; /*if need up or down sampler flag*/
    uint32_t                                audio_path_input_rate; /**< for audio_path_input_rate */
    uint32_t                                audio_path_output_rate; /**< for audio_path_output_rate */
#ifdef AIR_HFP_DNN_PATH_ENABLE
    bool                                    enable_ul_dnn;
#endif
} au_afe_open_param_t,*au_afe_open_param_p;

typedef struct {
    uint8_t param[20];//TEMP!! align bt_codec_a2dp_audio_t
} audio_dsp_a2DP_codec_param_t;

typedef struct
{
    uint8_t enable;
} dsp_audio_plc_ctrl_t, *dsp_audio_plc_ctrl_p;

typedef struct {
    audio_dsp_a2DP_codec_param_t    codec_info;
    #ifdef AVM_SHAEE_BUF_INFO
    avm_share_buf_info_t            *p_share_info;
    #else
    n9_dsp_share_info_t             *p_share_info;
    #endif
    uint32_t                        *p_asi_buf;
    uint32_t                        *p_min_gap_buf;
    uint32_t                        *p_current_bit_rate;
    uint32_t                        sink_latency;
    uint32_t                        bt_inf_address;
    #ifdef AVM_SHAEE_BUF_INFO
    uint32_t                        *p_afe_buf_report;
    #else
    uint32_t                        *clk_info_address;
    #endif
    uint32_t                        *p_lostnum_report;
    n9_dsp_audio_sync_info_t        *p_audio_sync_info;
    uint32_t                        *p_pcdc_anchor_info_buf;
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    uint32_t                        *ull_clk_info_address;
#endif
#ifdef MTK_AUDIO_PLC_ENABLE
    dsp_audio_plc_ctrl_t            audio_plc;
#endif
} audio_dsp_a2dp_dl_open_param_t, *audio_dsp_a2dp_dl_open_param_p;

typedef uint8_t audio_dsp_hfp_codec_param_t;

typedef struct {
    audio_dsp_hfp_codec_param_t     codec_type;
    #ifdef AVM_SHAEE_BUF_INFO
    avm_share_buf_info_t            *p_share_info;
    #else
    n9_dsp_share_info_t             *p_share_info;
    #endif
    uint32_t                        bt_inf_address;
    uint32_t                        *clk_info_address;
    uint32_t                        *p_air_dump_buf;
    uint32_t                        *p_rx_audio_forwarder_buf;
    uint32_t                        *p_tx_audio_forwarder_buf;
} audio_dsp_hfp_open_param_t, *audio_dsp_hfp_open_param_p;

typedef uint8_t audio_dsp_ble_codec_param_t;

typedef struct {
    audio_dsp_ble_codec_param_t     codec_type;
    uint8_t  channel_num;
    uint8_t  channel_mode;
    uint16_t frame_payload_length;
    uint32_t frame_ms;
    uint32_t sampling_frequency;
    uint32_t context_type;
    uint8_t  dual_cis_mode;
    uint8_t  decode_mode;
    n9_dsp_share_info_t             *p_share_info;
    n9_dsp_share_info_t             *p_sub_share_info;                   /**< Codec information for second CIS. */
} audio_dsp_ble_open_param_t, *audio_dsp_ble_open_param_p;

typedef struct {
    n9_dsp_share_info_t *p_share_info;
    uint32_t frames_per_message;
    encoder_bitrate_t bitrate;
    bool interleave;
} cm4_record_open_param_t, *cm4_record_open_param_p;

typedef struct {
    uint32_t sample_rate;
    uint32_t channel_mode;
    hal_audio_format_t format;
} audio_codec_pcm_t;

typedef struct {
    uint8_t min_bit_pool;       /**< The minimum bit pool. */
    uint8_t max_bit_pool;       /**< The maximum bit pool. */
    uint8_t block_length;       /**< b0: 16, b1: 12, b2: 8, b3: 4. */
    uint8_t subband_num;        /**< b0: 8, b1: 4. */
    uint8_t alloc_method;       /**< b0: loudness, b1: SNR. */
    uint8_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint8_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
} audio_codec_sbc_t;

typedef struct {
    uint32_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint32_t bit_rate;           /**< b0: 16kbps, b1:32kbps, b2:64kbps. */
    uint32_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
} audio_codec_opus_t;

typedef struct {
    uint32_t sample_rate;        /**< b0: 48000, b1: 44100, b2: 32000, b3: 16000. */
    uint32_t bit_rate;           /**< uint:bps. */
    uint32_t channel_mode;       /**< b0: joint stereo, b1: stereo, b2: dual channel, b3: mono. */
    uint32_t frame_interval;     /**< uint:us. */
    uint32_t frame_size;         /**< uint:Byte. */
} audio_codec_lc3_t;

typedef union {
    audio_codec_pcm_t pcm;
    audio_codec_sbc_t sbc;
    audio_codec_opus_t opus;
    audio_codec_lc3_t lc3;
} audio_codec_param_t;

typedef struct {
    uint32_t codec_type;
    audio_codec_param_t codec_param;
    uint32_t period;
    uint32_t gain_default_L;
    uint32_t gain_default_R;
} gaming_mode_param_t;

typedef gaming_mode_param_t wired_audio_param_t;

typedef struct {
    uint32_t codec_type;
    audio_codec_param_t codec_param;
    uint32_t period;
    uint32_t channel_enable;
    int32_t gain_default_L;
    int32_t gain_default_R;
    uint8_t *share_buffer_channel_1;
    uint8_t *share_buffer_channel_2;
} ble_audio_dongle_param_t;

typedef union {
    uint32_t reserved;
    gaming_mode_param_t gaming_mode_param;
    wired_audio_param_t wired_audio_param;
    ble_audio_dongle_param_t ble_audio_dongle_param;
} audio_transmitter_scenario_open_param_t;

typedef struct{
    uint8_t scenario_type;
    uint8_t scenario_sub_id;
    uint16_t data_notification_frequency;
    uint16_t max_payload_size;
    n9_dsp_share_info_t *p_share_info;
    audio_transmitter_scenario_open_param_t scenario_param;
}audio_transmitter_open_param_t;

typedef enum {
    BUFFER_INFO_TYPE = 0,
    SHARE_BUFFER_INFO_TYPE,
    AVM_SHARE_BUF_INFO_TYPE,
    STREAM_BUFFER_TYPE_MAX = 0xffffffff
} STREAM_BUFFER_TYPE;

typedef audio_transmitter_scenario_open_param_t bt_common_scenario_open_param_t;

typedef struct {
    uint8_t scenario_type;
    uint8_t scenario_sub_id;
    uint16_t data_notification_frequency;
    uint16_t max_payload_size;
    STREAM_BUFFER_TYPE share_info_type;
    n9_dsp_share_info_t *p_share_info;
    bt_common_scenario_open_param_t scenario_param;
} bt_common_open_param_t;

typedef union {
    au_afe_open_param_t             afe;
    audio_dsp_hfp_open_param_t      hfp;
    audio_dsp_ble_open_param_t      ble;
    audio_dsp_a2dp_dl_open_param_t  a2dp;
    audio_dsp_playback_info_t       playback;
    audio_transmitter_open_param_t  data_dl;
    bt_common_open_param_t          bt_common;
} mcu2dsp_open_stream_in_param_t, *mcu2dsp_open_stream_in_param_p;

typedef union {
    au_afe_open_param_t             afe;
    audio_dsp_hfp_open_param_t      hfp;
    audio_dsp_ble_open_param_t      ble;
    cm4_record_open_param_t         record;
    audio_transmitter_open_param_t  data_ul;
    bt_common_open_param_t          bt_common;
} mcu2dsp_open_stream_out_param_t, *mcu2dsp_open_stream_out_param_p;

/* Open message parameter structure */
typedef struct {
    mcu2dsp_param_t                 param;
    mcu2dsp_open_stream_in_param_t  stream_in_param;
    mcu2dsp_open_stream_out_param_t stream_out_param;
} mcu2dsp_open_param_t, *mcu2dsp_open_param_p;


/* Start message member parameter structure */
typedef struct {
    bool                        aws_flag;
    bool                        aws_sync_request;
    uint32_t                    aws_sync_time;
} audio_dsp_afe_start_param_t, *audio_dsp_afe_start_param_p;

typedef union {
    audio_dsp_a2dp_dl_start_param_t a2dp;
    audio_dsp_afe_start_param_t     afe;
} mcu2dsp_start_stream_in_param_t, *mcu2dsp_start_stream_in_param_p;

typedef union {
    audio_dsp_afe_start_param_t     afe;
} mcu2dsp_start_stream_out_param_t, *mcu2dsp_start_stream_out_param_p;

/* Start message parameter structure */
typedef struct {
    mcu2dsp_param_t                     param;
    mcu2dsp_start_stream_in_param_t     stream_in_param;
    mcu2dsp_start_stream_out_param_t    stream_out_param;
} mcu2dsp_start_param_t, *mcu2dsp_start_param_p;

/* SideTone message parameter structure */
typedef struct {
    hal_audio_device_t                      in_device;
    hal_audio_interface_t                   in_interface;
    uint32_t                                in_misc_parms;
    hal_audio_device_t                      out_device;
    hal_audio_interface_t                   out_interface;
    uint32_t                                out_misc_parms;
    hal_audio_channel_selection_t           in_channel; /*HW out channel default R+L*/
    uint32_t                                gain;
    uint32_t                                sample_rate;
    uint16_t                                *FIR_nvdm_param;
    hal_audio_analog_mdoe_t                 adc_mode;
} mcu2dsp_sidetone_param_t, *mcu2dsp_sidetone_param_p;

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#define PEQ_DIRECT      (0)
#define PEQ_SYNC        (1)
typedef struct {
    uint8_t         *nvkey_addr;
    uint16_t        peq_nvkey_id;
    uint8_t         drc_enable;
    uint8_t         setting_mode;
    uint32_t        target_bt_clk;
    uint8_t         phase_id;
    uint8_t         drc_force_disable;
} mcu2dsp_peq_param_t, *mcu2dsp_peq_param_p;
#endif

typedef struct {
    uint32_t config_operation;
    uint8_t config_param[250];
} mcu2dsp_audio_transmitter_runtime_config_param_t;

typedef struct {
    uint8_t  ENABLE;             /**< @Value   0x01 @Desc 1 */
    uint8_t  REVISION;           /**< @Value   0x01 @Desc 1 */
    uint16_t WWE_MODE;           /**< @Value 0x0000 @Desc 1 */
    uint16_t skip_frame_num ;    /**< @Value 0x0000 @Desc 1 */
    uint16_t noisy_thr_h ;       /**< @Value 0x0000 @Desc 1 */
    uint16_t noisy_thr_l ;       /**< @Value 0x0000 @Desc 1 */
    uint16_t noisy_debounce_cnt; /**< @Value 0x0000 @Desc 1 */
    uint16_t silent_thr_h ;      /**< @Value 0x0000 @Desc 1 */
    uint16_t silent_thr_l ;      /**< @Value 0x0000 @Desc 1 */
    uint16_t silent_debounce_cnt;/**< @Value 0x0000 @Desc 1 */
    uint16_t vit_pre ;           /**< @Value 0x0000 @Desc 1 */
    uint16_t RESERVE_1 ;         /**< @Value 0x0000 @Desc 1 */
    uint16_t RESERVE_2 ;         /**< @Value 0x0000 @Desc 1 */
    uint16_t RESERVE_3 ;         /**< @Value 0x0000 @Desc 1 */
    uint16_t RESERVE_4 ;         /**< @Value 0x0000 @Desc 1 */
} DSP_NVKEY_VAD_COMM;

typedef struct {
    int16_t en_vad;                /**< @Value 0x0000 @Desc 1 */
    int16_t gamma;                 /**< @Value 0x0000 @Desc 1 */
    int16_t diff_fac;              /**< @Value 0x0000 @Desc 1 */
    int16_t diff_facb;             /**< @Value 0x0000 @Desc 1 */
    int16_t vad_fac;               /**< @Value 0x0000 @Desc 1 */
    int16_t vad_ph_cc;             /**< @Value 0x0000 @Desc 1 */
    int16_t vad_ph_init;           /**< @Value 0x0000 @Desc 1 */
    int16_t vad_ph_range;          /**< @Value 0x0000 @Desc 1 */
    int16_t vad_corr_th;           /**< @Value 0x0000 @Desc 1 */
    int16_t vad_pitch_th;          /**< @Value 0x0000 @Desc 1 */
    int16_t win_sm_1st;            /**< @Value 0x0000 @Desc 1 */
    int16_t sil_thr   ;            /**< @Value 0x0000 @Desc 1 */
    int16_t sil_thr2  ;            /**< @Value 0x0000 @Desc 1 */
    int16_t fr_thr    ;            /**< @Value 0x0000 @Desc 1 */
    int16_t fr_thr2   ;            /**< @Value 0x0000 @Desc 1 */
    int16_t rec_thr   ;            /**< @Value 0x0000 @Desc 1 */
    int16_t rec_thr2  ;            /**< @Value 0x0000 @Desc 1 */
    int16_t end_thr   ;            /**< @Value 0x0000 @Desc 1 */
    int16_t gar_thr   ;            /**< @Value 0x0000 @Desc 1 */
    int16_t rel_thr   ;            /**< @Value 0x0000 @Desc 1 */
    int16_t ss_thr    ;            /**< @Value 0x0000 @Desc 1 */
    int16_t ms_thr    ;            /**< @Value 0x0000 @Desc 1 */
    int16_t ss_rel_thr;            /**< @Value 0x0000 @Desc 1 */
    int16_t ss_g_thr  ;            /**< @Value 0x0000 @Desc 1 */
    int16_t fa_end_thr;            /**< @Value 0x0000 @Desc 1 */
    int16_t fa_beg_thr;            /**< @Value 0x0000 @Desc 1 */
    int16_t short_thr ;            /**< @Value 0x0000 @Desc 1 */
    int16_t rel_st4_thr;           /**< @Value 0x0000 @Desc 1 */
    int16_t rel_oth_thr;           /**< @Value 0x0000 @Desc 1 */
    int16_t dur_thr   ;            /**< @Value 0x0000 @Desc 1 */
    int16_t max_dur_thr;           /**< @Value 0x0000 @Desc 1 */
    int16_t ms_s_thr  ;            /**< @Value 0x0000 @Desc 1 */
    int16_t vad_leave_th;          /**< @Value 0x0000 @Desc 1 */
    int16_t vad_eng_thr;           /**< @Value 0x0000 @Desc 1 */
    uint16_t RESERVE_A ;            /**< @Value 0x0000 @Desc 1 */
    uint16_t RESERVE_B ;            /**< @Value 0x0000 @Desc 1 */
    uint16_t RESERVE_C ;            /**< @Value 0x0000 @Desc 1 */
    uint16_t RESERVE_D ;            /**< @Value 0x0000 @Desc 1 */
} DSP_NVKEY_VAD_PARA;

typedef struct
{
    /*NVkey_0 0x00:0x0303 0x01:0x1313
              0x02:0x2323 0x03:0x3333
              0x04:0x4343 0x05:0x5353
              0x06:0x6363 0x07:0x7373*/
    uint8_t snr_threshold;
    uint8_t noise_ignore_bits; /*NVkey_1 0x01:0xFFFF0000 0x02:0xFFF00000 0x03:0xFF000000 0x04:0xF0000000 */
    uint8_t alpha_rise;        /*NVkey_3 0x01 - 0x0F */
    uint8_t enable;            /*NVkey_4 0x00:diable 0x01:enable*/
    uint16_t main_mic;         /*NVkey_5 vow use main mic type*/
    uint16_t ref_mic;          /*NVkey_6 vow use ref mic type*/
}DSP_NVKEY_VOW_PARA;

typedef struct {
    DSP_NVKEY_VAD_COMM vad_nvkey_common;
    DSP_NVKEY_VAD_PARA vad_nvkey_1mic_v_mode;
    DSP_NVKEY_VAD_PARA vad_nvkey_1mic_c_mode;
    DSP_NVKEY_VAD_PARA vad_nvkey_2mic_v_mode;
    DSP_NVKEY_VAD_PARA vad_nvkey_2mic_c_mode;
    uint32_t language_mode_address;
    uint32_t language_mode_length;
    DSP_NVKEY_VOW_PARA vow_setting;
    uint8_t adda_analog_mic_mode; /*0x0: ACC_10k, 0x1: ACC_20k,0x2: DCC*/
} mcu2dsp_vad_param_t, *mcu2dsp_vad_param_p;

//--------------------------------------------
// Config structure
//--------------------------------------------
typedef enum {
    AUDIO_PLAYBACK_CONFIG_EOF
} audio_playback_config_index_t;

//--------------------------------------------
// Audio Sync Parameters
//--------------------------------------------
typedef struct {
    uint32_t                    time_stamp;
    uint32_t                    sample;
} audio_dsp_a2dp_dl_time_param_t;

//--------------------------------------------
// ULL Audio PlayEN Parameters
//--------------------------------------------
typedef struct {
    uint32_t sequence_number;
    uint32_t bt_clock;
} audio_dsp_a2dp_dl_play_en_param_t;


//--------------------------------------------
// A2DP LTCS data
//--------------------------------------------
typedef struct {
    uint32_t *p_ltcs_asi_buf;
    uint32_t *p_ltcs_min_gap_buf;
} audio_dsp_a2dp_ltcs_report_param_t;

typedef struct {
    int32_t drift_comp_val; // long term drift compensation value
    uint32_t anchor_clk;    // long term drift anchor clk
    uint32_t asi_base;      // 1st time is play_en asi, later is anchor asi
    uint32_t asi_cur;   // asi base for current play
} ltcs_anchor_info_t;

//--------------------------------------------
// Leakage compensation data
//--------------------------------------------
typedef void (*anc_leakage_compensation_callback_t)(uint16_t leakage_status);

typedef struct {
    uint16_t calibration_status;
    uint16_t wz_set;
    anc_leakage_compensation_callback_t api_callback;
    anc_leakage_compensation_callback_t vp_start_callback;
    uint8_t enable;
    uint8_t anc_enable;
    uint8_t anc_filter_id;
    uint8_t anc_type;
} audio_dsp_leakage_compensation_report_param_t;

#ifdef AIR_SILENCE_DETECTION_ENABLE
typedef struct STRU_SD_PARA_s
{
    int32_t AutoPowerOff_TH_dB;
    uint32_t AutoPowerOff_Time;
    int32_t NLE_TH_dB;
    uint32_t NLE_Time;
    int32_t ExtAmpOff_TH_dB;
    uint32_t ExtAmpOff_Time;
    bool APO_isEnable;
    bool NLE_isEnable;
    bool EAO_isEnable;
}PACKED SD_NVKEY_STATE;

typedef void (*silence_detection_callback_t)(bool silence_flag);
typedef struct {
    silence_detection_callback_t callback;
    SD_NVKEY_STATE NvKey;
} audio_dsp_silence_detection_param_t;
#endif

//--------------------------------------------
// Temp audio data
//--------------------------------------------
#ifdef HAL_AUDIO_ANC_ENABLE
typedef struct {
    au_afe_open_param_t    adc_setting;
    au_afe_open_param_t    dac_setting;
} mcu2dsp_open_adda_param_t, *mcu2dsp_open_adda_param_p;
#endif


//--------------------------------------------
// Audio Driver configuration
//--------------------------------------------
typedef enum {
    AUDIO_DRIVER_SET_HOLD_AMP_GPIO  = 1,

    AUDIO_DRIVER_SET_NUMBER,

} audio_driver_set_info_t;


#ifdef __cplusplus
}
#endif

#endif /*__HAL_AUDIO_MESSAGE_STRUCT_H__ */
