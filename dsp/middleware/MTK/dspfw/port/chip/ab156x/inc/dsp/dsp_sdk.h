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

#ifndef _DSP_SDK_H_
#define _DSP_SDK_H_

#include "types.h"
#include "transform.h"
#include <stdbool.h>


/**
 * @addtogroup Middleware
 * @{
 * @addtogroup Stream
 * @{
 * This section introduces the DSP customization APIs including terms and acronyms,
 * details on how to use this APIs, feature table, enums, structures and functions.
 * The stream is data flow.
 * The DSP process is applied to stream by feature list.\n
 * The method of algorithm integration is shown in \ref STREAM_INTERFACE.
 *
 */



/******************************************************************************
 * CONSTANT DEFINITIONS
 ******************************************************************************/



/******************************************************************************
 * DSP Command Structure
 ******************************************************************************/
/**
 * @addtogroup Typedef
 * @{
 */

 /** @brief This structure defines the entry of the stream feature. */
typedef bool (*stream_feature_entry_t)   (void *para);
/** @brief This structure defines the entry of the stream feature codec. */
typedef stream_feature_entry_t stream_feature_codec_entry_t;
/** @brief This structure defines the entry of the stream feature function. */
typedef stream_feature_entry_t stream_feature_function_entry_t;
/** @brief This structure defines the entry of the PIC function. */
typedef void (*stream_feature_ctrl_entry)   (void);

/**
 * @}
 */


/**
 * @addtogroup Struct
 * @{
 */

/** This structure defines the configuration of the stream feature table. */
typedef struct stream_feature_s {
    uint32_t   feature_type   : 8;              /**< The feature Type.  */
    uint32_t   memory_size    : 24;             /**< The instance memory(working buffer) size reserved for feature usage. */
    uint32_t   codec_output_size;               /**< The codec output size for streaming buffer allocate, Set 0 for functions. */
    stream_feature_entry_t initialize_entry;    /**< The Initializing entry for procedure. */
    stream_feature_entry_t process_entry;       /**< The Processing entry for procedure. */
}
/** @brief This structure defines the stream feature. */
stream_feature_t,
/** @brief This structure defines the stream feature pointer. */
*stream_feature_ptr_t,
/** @brief This structure defines the stream feature table. */
stream_feature_table_t,
/** @brief This structure defines the stream feature codec. */
stream_feature_codec_t,
/** @brief This structure defines the stream feature function. */
stream_feature_function_t;
/**
 * @}
 */


/**
 * @addtogroup Enum
 * @{
 */

/** This enum defines the types of audio features.*/
typedef enum
{
    DSP_DECODER_TYPE = 0,
    CODEC_PCM_COPY = DSP_DECODER_TYPE,          /**< Default type is null codec. For PCM input stream that do not require codec. */

    CODEC_DECODER_CVSD      = 0x01,             /**< decoder type: CVSD.  */
    CODEC_DECODER_MSBC      = 0x02,             /**< decoder type: MSBC. */
    CODEC_DECODER_SBC       = 0x03,             /**< decoder type: SBC. */
    CODEC_DECODER_AAC       = 0x04,             /**< decoder type: AAC. */
    CODEC_DECODER_MP3       = 0x05,             /**< decoder type: Reserved. */
    CODEC_DECODER_EC        = 0x06,             /**< decoder type: Reserved. */
    CODEC_DECODER_UART      = 0x07,             /**< decoder type: Reserved. */
    CODEC_DECODER_UART16BIT = 0x08,             /**< decoder type: Reserved. */
    CODEC_DECODER_CELT_HD   = 0x09,             /**< decoder type: Reserved. */
    CODEC_DECODER_VENDOR    = 0x0A,             /**< decoder type: Vendor defined. */
    CODEC_DECODER_VENDOR_1  = 0x0B,             /**< decoder type: Vendor 1 defined. */
    CODEC_DECODER_VENDOR_2  = 0x0C,             /**< decoder type: Vendor 2 defined. */
    CODEC_DECODER_VENDOR_3  = 0x0D,             /**< decoder type: Vendor 3 defined. */
    CODEC_DECODER_LC3       = 0x0E,             /**< decoder type: LC3. */
    CODEC_DECODER_SAMPLE,                       /**< decoder type: example for customer. */

    DSP_ENCODER_TYPE = 0x10,
    CODEC_ENCODER_CVSD = DSP_ENCODER_TYPE,      /**< encoder type: CVSD. */
    CODEC_ENCODER_MSBC      = 0x11,             /**< encoder type: MSBC. */
    CODEC_ENCODER_OPUS      = 0x12,             /**< encoder type: OPUS. */
    CODEC_ENCODER_LC3       = 0x13,             /**< encoder type: LC3. */
    CODEC_ENCODER_LC3_BRANCH= 0x14,             /**< encoder type: LC3. branch a new stream to encode */
    CODEC_ENCODER_SAMPLE,                       /**< encoder type: example for customer. */

    DSP_FUNC_TYPE = 0x20,
    DSP_SRC = DSP_FUNC_TYPE-1,
    FUNC_END = DSP_FUNC_TYPE,                   /**< Basic FUNCTION type: using to identify end of feature list. */
    FUNC_RX_WB_DRC          = 0x21,             /**< SPEECH PROCESS FUNCTION type: DRC for RX WB. */
    FUNC_RX_NB_DRC          = 0x22,             /**< SPEECH PROCESS FUNCTION type: DRC for RX NB. */
    FUNC_TX_WB_DRC          = 0x23,             /**< SPEECH PROCESS FUNCTION type: DRC for TX WB. */
    FUNC_TX_NB_DRC          = 0x24,             /**< SPEECH PROCESS FUNCTION type: DRC for TX NB. */
    FUNC_RX_NR              = 0x25,             /**< SPEECH PROCESS FUNCTION type: NR for RX. */
    FUNC_TX_NR              = 0x26,             /**< SPEECH PROCESS FUNCTION type: NR for TX. */
    FUNC_CLK_SKEW_UL        = 0x27,             /**< SPEECH PROCESS FUNCTION type: CLK_SKEW for UL. */
    FUNC_CLK_SKEW_DL        = 0x28,             /**< SPEECH PROCESS FUNCTION type: CLK_SKEW for DL. */
    FUNC_MIC_SW_GAIN        = 0x29,             /**< SPEECH PROCESS FUNCTION type: SW gain for UL. */
    FUNC_PLC                = 0x2A,             /**< SPEECH PROCESS FUNCTION type: PLC. */
    FUNC_CLK_SKEW_HFP_DL    = 0x2B,             /**< SPEECH PROCESS FUNCTION type: CLK_SKEW for ESCO DL. */
    FUNC_PROC_SIZE_CONV     = 0x2C,             /**< Basic FUNCTION type: size conversion. */
    FUNC_JOINT              = 0x2D,             /**< Basic FUNCTION type: Reserved. */
    FUNC_BRANCH             = 0x2E,             /**< Basic FUNCTION type: Reserved. */
    FUNC_MUTE               = 0x2F,             /**< Basic FUNCTION type: Reserved. */
    FUNC_DRC                = 0x30,             /**< Basic FUNCTION type: DRC. */
    FUNC_PEQ                = 0x31,             /**< Basic FUNCTION type: PEQ. */
    FUNC_LPF                = 0x32,             /**< Basic FUNCTION type: Reserved. */
    FUNC_CH_SEL             = 0x33,             /**< Basic FUNCTION type: CH SEL. */
    FUNC_MUTE_SMOOTHER      = 0x34,             /**< Basic FUNCTION type: mute smoother. */
    FUNC_PEQ2               = 0x35,             /**< Basic FUNCTION type: PEQ2. */
    FUNC_DRC2               = 0x36,             /**< Basic FUNCTION type: DRC2. */
    FUNC_CH_SEL_HFP         = 0x37,             /**< Basic FUNCTION type: CH SEL. for HFP */
    FUNC_LEAKAGE_COMPENSATION= 0x38,             /**< Basic FUNCTION type: LEAKAGE_COMPENSATION */
    FUNC_WWE_PREPROC        = 0x3A,             /**< Basic FUNCTION type: WWE Pre-processing */
    FUNC_WWE_PROC           = 0x3B,             /**< Basic FUNCTION type: WWE processing */
    FUNC_RX_WB_AGC          = 0x3C,             /**< Basic FUNCTION type: AGC for RX WB. for HFP */
    FUNC_RX_NB_AGC          = 0x3D,             /**< Basic FUNCTION type: AGC for RX NB. for HFP */
    FUNC_TX_AGC             = 0x3E,             /**< Basic FUNCTION type: AGC for TX. for HFP */
    FUNC_GSENSOR_MOTION_DETECT = 0x3F,          /**< Basic FUNCTION type: GSENSOR moton detect*/
    FUNC_AUDIO_PLC          = 0x40,             /**< Basic FUNCTION type: AUDIO PLC. */
    FUNC_INS                = 0x41,             /**< Basic FUNCTION type: Idle Noise Suppression */
    FUNC_PEQ_INSTANT        = 0x42,             /**< Basic FUNCTION type: PEQ. */
    FUNC_FIXED_SW_GAIN      = 0x43,             /**< Special FUNCTION type: Fixed SW gain function for Special Function*/
    FUNC_FUNCTION_A         = 0x44,             /**< Special FUNCTION type: Special Function A for Microphone*/
    FUNC_FUNCTION_B         = 0x45,             /**< Special FUNCTION type: Special Function B for Microphone*/
    FUNC_FUNCTION_C         = 0x46,             /**< Special FUNCTION type: Special Function C for Microphone*/
    FUNC_FUNCTION_D         = 0x47,             /**< Special FUNCTION type: Special Function D for Sensor*/
    FUNC_FUNCTION_E         = 0x48,             /**< Special FUNCTION type: Special Function E for Sensor*/
    FUNC_FUNCTION_F         = 0x49,             /**< Special FUNCTION type: Special Function F for Sensor*/
    FUNC_FUNCTION_G         = 0x4A,             /**< Special FUNCTION type: Special Function G for Sensor/Microphone*/
    FUNC_FUNCTION_H         = 0x4B,             /**< Special FUNCTION type: Special Function H for Microphone*/
    FUNC_TX_EQ              = 0x4C,             /**< Basic FUNCTION type: Tx EQ*/
    FUNC_USER_TRIGGER_FF_SZ = 0x4D,             /**< Basic FUNCTION type: User Triggered adaptive FF*/
    FUNC_USER_TRIGGER_FF_PZ = 0x4E,             /**< Basic FUNCTION type: User Triggered adaptive FF*/
    FUNC_USER_TRIGGER_FF_PZ_FIR = 0x4F,             /**< Basic FUNCTION type: User Triggered adaptive FF*/
    FUNC_AUDIO_LOOPBACK_TEST= 0x50,             /**< Basic FUNCTION type: Audio Loopback Test for Flicker Noise*/
    FUNC_DRC3               = 0x51,             /**< Basic FUNCTION type: DRC3. */
    FUNC_SW_SRC             = 0x52,             /**< Basic FUNCTION type: Software Sample Rate Converters. */
    FUNC_SW_CLK_SKEW        = 0x53,             /**< Basic FUNCTION type: Software clock skew. */
    FUNC_SW_GAIN            = 0x54,             /**< Basic FUNCTION type: Software gain. */
    FUNC_SW_MIXER           = 0x55,             /**< Basic FUNCTION type: Software mixer. */
    FUNC_SW_BUFFER          = 0x56,             /**< Basic FUNCTION type: Software buffer. */
    FUNC_CLK_SKEW_BLE_MUSIC_DL = 0x57,          /**< Basic FUNCTION type: CLK_SKEW for BLE Music DL. */
    FUNC_WIND_DETECT        = 0x58,             /**< Basic FUNCTION type: wind detection. */
    FUNC_CH_SEL_VP          = 0x59,             /**< Basic FUNCTION type: CH SEL for VP. */
    FUNC_PEQ3               = 0x5A,             /**< Basic FUNCTION type: PEQ3. */
    FUNC_AFC                = 0x5B,             /**< Basic FUNCTION type: Acoustic feedback cancellation. */
    FUNC_LD_NR              = 0x5C,             /**< Basic FUNCTION type: Low delay noise reduction. */
    FUNC_AT_AGC             = 0x5D,             /**< Basic FUNCTION type: Advance PassThru auto gain control. */
    FUNC_PEQ4               = 0x5E,             /**< Basic FUNCTION type: PEQ4. */
    FUNC_DNN_NR             = 0x5F,             /**< Special FUNCTION type: DNN NR >*/
    FUNC_EC120              = 0x60,             /**< SPEECH PROCESS FUNCTION type: EC120. >*/
    FUNC_SRC_FIXED_RATIO    = 0x61,             /**< Basic FUNCTION type: up/down sampler. */
    FUNC_USER_UNAWARE       = 0x62,             /**< Basic FUNCTION type: user unaware. */
    FUNC_ENVIRONMENT_DETECTION = 0x63,          /**< Basic FUNCTION type: Environment Detection. */
    FUNC_ECNR_PREV_PROCESS = 0x64,              /**< Basic FUNCTION type: Previous process of ECNR. */
    FUNC_ECNR_POST_PROCESS = 0x65,              /**< Basic FUNCTION type: Post process of ECNR. */
    FUNC_SAMPLE,                                /**< FUNCTION type: example for customer. */

    DSP_FEATURE_MAX_NUM,
}
/** @brief This structure defines the stream feature type. */
stream_feature_type_t,
/** @brief This structure defines the stream feature type pointer. */
*stream_feature_type_ptr_t,
/** @brief This structure defines the stream feature list. */
stream_feature_list_t,
/** @brief This structure defines the stream feature list pointer. */
*stream_feature_list_ptr_t;


/** @brief This enum defines the types of codec configuration.*/
typedef enum {
    CONFIG_DECODER,    /**< To configure the decoder. */
    CONFIG_ENCODER,    /**< To configure the encoder. */
} stream_feature_configure_codec_t;

/** @brief This enum defines the sampling rate.*/
typedef enum
{
    FS_RATE_8K          = 8,    /**< 8kHz sampling rate. */
    FS_RATE_16K         = 16,   /**< 16kHz sampling rate. */
    FS_RATE_24K         = 24,   /**< 24kHz sampling rate. */
    FS_RATE_32K         = 32,   /**< 32kHz sampling rate. */
    FS_RATE_44_1K       = 44,   /**< 44.1kHz sampling rate. */
    FS_RATE_48K         = 48,   /**< 48kHz sampling rate. */
    FS_RATE_88_2K       = 88,   /**< 88.2kHz sampling rate. */
    FS_RATE_96K         = 96,   /**< 96kHz sampling rate. */
    FS_RATE_192K        = 192,  /**< 192kHz sampling rate. */
} stream_samplerate_t;

/** @brief This enum defines the bit resolution of audio samples.*/
typedef enum
{
    RESOLUTION_16BIT    = 0,    /**< 16 bits per sample. */
    RESOLUTION_32BIT    = 1,    /**< 32 bits per sample. */
}stream_resolution_t;

/** @brief This enum defines the audio stream sampling rate conversion ratio.*/
typedef enum
{
    UPSAMPLE_BY1        = 0x01,    /**< SRC ratio is 1x. */
    UPSAMPLE_BY2        = 0x02,    /**< SRC ratio is 2x. */
    UPSAMPLE_BY3        = 0x03,    /**< SRC ratio is 3x. */
    UPSAMPLE_BY4        = 0x04,    /**< SRC ratio is 4x. */
    UPSAMPLE_BY6        = 0x06,    /**< SRC ratio is 6x. */
    UPSAMPLE_BY12       = 0x0C,    /**< SRC ratio is 12x. */

    DOWNSAMPLE_BY1      = 0xFF,    /**< SRC ratio is 1x. */
    DOWNSAMPLE_BY2      = 0xFE,    /**< SRC ratio is 1/2x. */
    DOWNSAMPLE_BY3      = 0xFD,    /**< SRC ratio is 1/3x. */
    DOWNSAMPLE_BY4      = 0xFC,    /**< SRC ratio is 1/4x. */
    DOWNSAMPLE_BY6      = 0xFA,    /**< SRC ratio is 1/6x. */
    DOWNSAMPLE_BY12     = 0xF4,    /**< SRC ratio is 1/12x. */

}stream_feature_convert_samplerate_t;

/**
 * @addtogroup Struct
 * @{
 */

/** This structure defines the open & close feature control entry. */
typedef struct
{
    stream_feature_ctrl_entry open_entry;       /**< The open entry for this feature.  */
    stream_feature_ctrl_entry close_entry;      /**< The close entry for this feature.  */
}
/** @brief This structure defines the stream feature control. */
stream_feature_ctrl_entry_t,
/** @brief This structure defines the stream feature control pointer. */
*stream_feature_ctrl_entry_ptr_t;
/**
 * @}
 */

/** @brief This enum defines the bit rate of the encoded data.*/
typedef enum dsp_enc_bitrate_e
{
    ENCODER_BITRATE_16KBPS  = 16, /**< 16kbps encoder bitrate. */
    ENCODER_BITRATE_32KBPS  = 32, /**< 32kbps encoder bitrate. */
    ENCODER_BITRATE_64KBPS  = 64, /**< 64kbps encoder bitrate. */
    ENCODER_BITRATE_MAX     = 0xFFFFFFFF,
} DSP_ENC_BITRATE_t;

/**
 * @}
 */


/******************************************************************************
 * External Global Variables
 ******************************************************************************/

EXTERN stream_feature_table_t stream_feature_table[];
/**<
 * Declare feature table for stream.
 * @section STREAM_FEATURETABLE Feature Table
 *

 * | feature_type                 | memory_size                                | codec_output_size         | initialize_entry                               | process_entry                                |
 * |------------------------------|--------------------------------------------|---------------------------|------------------------------------------------|----------------------------------------------|
 * |\b CODEC_PCM_COPY |                                                      0 |                      2048 |                     stream_pcm_copy_initialize |                        stream_pcm_copy_process
 * |\b CODEC_DECODER_CVSD |                           DSP_CVSD_DECODER_MEMSIZE | 480+DSP_SIZE_FOR_CLK_SKEW |           stream_codec_decoder_cvsd_initialize |              stream_codec_decoder_cvsd_process
 * |\b CODEC_DECODER_MSBC |                           DSP_MSBC_DECODER_MEMSIZE | 480+DSP_SIZE_FOR_CLK_SKEW |           stream_codec_decoder_msbc_initialize |              stream_codec_decoder_msbc_process
 * |\b CODEC_DECODER_SBC |                               DSP_SBC_CODEC_MEMSIZE |4096+DSP_SIZE_FOR_CLK_SKEW |            stream_codec_decoder_sbc_initialize |               stream_codec_decoder_sbc_process
 * |\b CODEC_DECODER_AAC |                             DSP_AAC_DECODER_MEMSIZE |4096+DSP_SIZE_FOR_CLK_SKEW |            stream_codec_decoder_aac_initialize |               stream_codec_decoder_aac_process
 * |\b CODEC_DECODER_MP3 |                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b CODEC_DECODER_EC |                                                    0 |                      0xFF |                                           NULL |                                           NULL
 * |\b CODEC_DECODER_UART |                                                  0 |                      0xFF |                                           NULL |                                           NULL
 * |\b CODEC_DECODER_UART16BIT |                                             0 |                      0xFF |                                           NULL |                                           NULL
 * |\b CODEC_DECODER_CELT_HD |                                               0 |                      0xFF |                                           NULL |                                           NULL
 * |\b CODEC_DECODER_VENDOR |                               VENDOR_HANDLE_SIZE |4096+DSP_SIZE_FOR_CLK_SKEW |         STREAM_CODEC_DECODER_VENDOR_INITIALIZE |            STREAM_CODEC_DECODER_VENDOR_PROCESS
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b CODEC_ENCODER_CVSD |                           DSP_CVSD_ENCODER_MEMSIZE |                       240 |           stream_codec_encoder_cvsd_initialize |              stream_codec_encoder_cvsd_process
 * |\b CODEC_ENCODER_MSBC |                           DSP_MSBC_ENCODER_MEMSIZE |                       240 |           stream_codec_encoder_msbc_initialize |              stream_codec_encoder_msbc_process
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b 0 |                                                                   0 |                      0xFF |                                           NULL |                                           NULL
 * |\b DSP_SRC |                                               DSP_SRC_MEMSIZE |                         0 |                 stream_function_src_initialize |                    stream_function_src_process
 * |\b FUNC_END |                                                            0 |                         0 |                 stream_function_end_initialize |                    stream_function_end_process
 * |\b FUNC_RX_WB_DRC |                                     DSP_DRC_VO_MEMSIZE |                         0 |     stream_function_drc_voice_rx_wb_initialize |           stream_function_drc_voice_rx_process
 * |\b FUNC_RX_NB_DRC |                                     DSP_DRC_VO_MEMSIZE |                         0 |     stream_function_drc_voice_rx_nb_initialize |           stream_function_drc_voice_rx_process
 * |\b FUNC_TX_WB_DRC |                                     DSP_DRC_VO_MEMSIZE |                         0 |     stream_function_drc_voice_tx_wb_initialize |           stream_function_drc_voice_tx_process
 * |\b FUNC_TX_NB_DRC |                                     DSP_DRC_VO_MEMSIZE |                         0 |     stream_function_drc_voice_tx_nb_initialize |           stream_function_drc_voice_tx_process
 * |\b FUNC_RX_NR |                                                          0 |                         0 |              stream_function_aec_nr_initialize |                     stream_function_nr_process
 * |\b FUNC_TX_NR |                                                          0 |                         0 |              stream_function_aec_nr_initialize |                    stream_function_aec_process
 * |\b FUNC_CLK_SKEW_UL |    DSP_CLK_SKEW_MEMSIZE*3+DSP_CLK_SKEW_TEMP_BUF_SIZE |                         0 |   stream_function_clock_skew_uplink_initialize |      stream_function_clock_skew_uplink_process
 * |\b FUNC_CLK_SKEW_DL |    DSP_CLK_SKEW_MEMSIZE*4+DSP_CLK_SKEW_TEMP_BUF_SIZE |                         0 | stream_function_clock_skew_downlink_initialize |    stream_function_clock_skew_downlink_process
 * |\b FUNC_MIC_SW_GAIN |                                                    0 |                         0 |                stream_function_gain_initialize |                   stream_function_gain_process
 * |\b FUNC_PLC |                                        DSP_VOICE_PLC_MEMSIZE |                         0 |                 stream_function_plc_initialize |                    stream_function_plc_process
 * |\b FUNC_CLK_SKEW_HFP_DL |DSP_CLK_SKEW_MEMSIZE*2+DSP_CLK_SKEW_TEMP_BUF_SIZE |                         0 | stream_function_clock_skew_downlink_initialize |stream_function_clock_skew_hfp_downlink_process
 * |\b FUNC_PROC_SIZE_CONV |                                                 0 |                         0 |      stream_function_size_converter_initialize |         stream_function_size_converter_process
 * |\b FUNC_JOINT |                                                          0 |                         0 |                                           NULL |                                           NULL
 * |\b FUNC_BRANCH |                                                         0 |                         0 |                                           NULL |                                           NULL
 * |\b FUNC_MUTE |                                                           0 |                         0 |                                           NULL |                                           NULL
 * |\b FUNC_DRC |                                           DSP_DRC_AU_MEMSIZE |                         0 |           stream_function_drc_audio_initialize |              stream_function_drc_audio_process
 * |\b FUNC_PEQ |                                              DSP_PEQ_MEMSIZE |                         0 |                 stream_function_peq_initialize |                    stream_function_peq_process
 * |\b FUNC_LPF |                                                            0 |                         0 |                                           NULL |                                           NULL
 * |\b FUNC_CH_SEL |                                                         0 |                         0 |    stream_function_channel_selector_initialize |       stream_function_channel_selector_process
 * |\b FUNC_MUTE_SMOOTHER |                                                  0 |                         0 |         stream_function_mute_smooth_initialize |            stream_function_mute_smooth_process
*/


EXTERN stream_feature_list_t stream_feature_list_a2dp[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for A2DP.\n
 *  { \n
 *      #CODEC_DECODER_SBC, \n
 *      #FUNC_MUTE_SMOOTHER, \n
 *      #FUNC_PEQ, \n
 *      #FUNC_DRC, \n
 *      #FUNC_CLK_SKEW_DL, \n
 *      #FUNC_CH_SEL, \n
 *      #FUNC_END, \n
 *  };
*/

EXTERN stream_feature_list_t stream_feature_list_vend_a2dp[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for A2DP.\n
 *  { \n
 *      #CODEC_DECODER_VENDOR, \n
 *      #FUNC_CH_SEL, \n
 *      #FUNC_END, \n
 *  };
*/


EXTERN stream_feature_list_t stream_feature_list_hfp_uplink[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for HFP uplink.\n
 *  { \n
 *      #CODEC_PCM_COPY, \n
 *      #FUNC_MIC_SW_GAIN, \n
 *      #FUNC_CLK_SKEW_UL, \n
 *      #FUNC_TX_NR, \n
 *      #FUNC_TX_WB_DRC, \n
 *      #CODEC_ENCODER_MSBC, \n
 *      #FUNC_END, \n
 *  };
*/

EXTERN stream_feature_list_t stream_feature_list_hfp_downlink[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for HFP downlink.\n
 *  {\n
 *      #CODEC_DECODER_MSBC, \n
 *      #FUNC_PLC, \n
 *      #FUNC_RX_NR,\n
 *      #FUNC_RX_WB_DRC,\n
 *      #FUNC_CLK_SKEW_HFP_DL,\n
 *      #FUNC_END,\n
 *  };
*/

#ifdef AIR_HFP_DNN_PATH_ENABLE
EXTERN stream_feature_list_t stream_feature_list_hfp_uplink_dnn[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for HFP uplink DNN path.\n
 *  { \n
 *      #CODEC_PCM_COPY, \n
 *      #FUNC_MIC_SW_GAIN, \n
 *      #FUNC_END, \n
 *  };
*/
#endif

EXTERN stream_feature_list_t stream_feature_list_playback[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for CM4 playback.\n
 *  {\n
 *      #CODEC_PCM_COPY,\n
 *      #FUNC_MIC_SW_GAIN,\n
 *      #FUNC_END,\n
 *  };
*/

EXTERN stream_feature_list_t stream_feature_list_mic_record[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for mic record.\n
 *  {\n
 *      #CODEC_PCM_COPY,\n
 *      #FUNC_MIC_SW_GAIN,\n
 *      #FUNC_END,\n
 *  };
*/

EXTERN stream_feature_list_t stream_feature_list_mic_record_airdump[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for mic record.\n
 *  {\n
 *      #CODEC_PCM_COPY,\n
 *      #FUNC_END,\n
 *  };
*/

EXTERN stream_feature_list_t stream_feature_list_wwe_mic_record[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for mic record.\n
 *  {\n
 *      #CODEC_PCM_COPY,\n
 *      #FUNC_MIC_SW_GAIN,\n
 *      #FUNC_WWE_PREPROC,
 *      #FUNC_WWE_PROC,
 *      #FUNC_END,\n
 *  };
*/

EXTERN stream_feature_list_t stream_feature_list_prompt[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for prompt.\n
 *  {\n
 *      #CODEC_PCM_COPY,\n
 *      #FUNC_END,\n
 *  };
*/

#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
EXTERN stream_feature_list_t stream_feature_list_prompt_dummy_source[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for prompt.\n
 *  {\n
 *      #CODEC_PCM_COPY,\n
 *      #FUNC_END,\n
 *  };
*/
#endif

EXTERN stream_feature_list_t stream_feature_list_linein[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for linein playback.\n
 *  {\n
        #CODEC_PCM_COPY,\n
        #FUNC_END,\n
 *  };
*/

#ifdef MTK_LEAKAGE_DETECTION_ENABLE
EXTERN stream_feature_list_t stream_feature_list_leakage_compensation[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for leakage compensation.\n
 *  {\n
        #CODEC_PCM_COPY,\n
        #FUNC_LEAKAGE_COMPENSATION
        #FUNC_END,\n
 *  };
*/
#endif

#ifdef MTK_USER_TRIGGER_FF_ENABLE
EXTERN stream_feature_list_t stream_feature_list_user_trigger_adaptive_ff_sz[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for user trigger FF.\n
 *  {\n
        #CODEC_PCM_COPY,\n
        #FUNC_USER_TRIGGER_FF_SZ,\n
        #FUNC_END,\n
 *  };
*/
EXTERN stream_feature_list_t stream_feature_list_user_trigger_adaptive_ff_pz[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for user trigger FF.\n
 *  {\n
    #CODEC_PCM_COPY,\n
    #FUNC_USER_TRIGGER_FF_PZ,\n
    #FUNC_END,\n
 *  };
*/
EXTERN stream_feature_list_t stream_feature_list_user_trigger_adaptive_ff_pz_fir[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for user trigger FF.\n
 *  {\n
#CODEC_PCM_COPY,\n
#FUNC_USER_TRIGGER_FF_PZ_FIR,\n
#FUNC_END,\n
 *  };
*/

#endif

#ifdef MTK_SENSOR_SOURCE_ENABLE
EXTERN stream_feature_list_t AudioFeatureList_GSensorMotionDetect[];
EXTERN stream_feature_list_t AudioFeatureList_GSensorMotionDetect_virtual[];
#endif
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for gsensor motion detect.\n
 *  {\n
        #FUNC_GSENSOR_MOTION_DETECT,\n
        #FUNC_END,\n
 *  };
*/

#ifdef AIR_BT_CODEC_BLE_ENABLED
EXTERN stream_feature_list_t AudioFeatureList_BLE_Call_UL[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for le audio uplink.\n
 *  {\n
        #CODEC_PCM_COPY,\n
        #FUNC_MIC_SW_GAIN, \n
        #CODEC_ENCODER_LC3, \n
        #FUNC_END,\n
 *  };
*/

EXTERN stream_feature_list_t AudioFeatureList_BLE_Music_DL[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for le audio downlink.\n
 *  {\n
        #CODEC_DECODER_LC3,\n
        #FUNC_CLK_SKEW_ESCO_DL,\n
        #FUNC_END,\n
 *  };
*/

EXTERN stream_feature_list_t AudioFeatureList_BLE_Call_DL[];
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for le audio downlink.\n
 *  {\n
        #CODEC_DECODER_LC3,\n
        #FUNC_CLK_SKEW_ESCO_DL,\n
        #FUNC_END,\n
 *  };
*/
#endif

#if defined(MTK_ANC_SURROUND_MONITOR_ENABLE)
EXTERN stream_feature_list_t stream_featuremulti_anc_monitor[];
#endif
/**<
 *  Declare all of feature list at dsp_sdk.c \n
 *  Each feature list must begin with \b codec and end with #FUNC_END.
 *  Stream feature list for gsensor motion detect.\n
 *  {\n
        #CODEC_PCM_COPY,\n
        #FUNC_END,\n
 *  };
*/

/******************************************************************************
 * External Functions
 ******************************************************************************/


/*!
  @brief Initialize customer's feature.
*/
void dsp_sdk_initialize(void);


/*!
  @brief Add feature to feature table.
  @param[in] feature_ptr is a pointer to corresponding feature pointer.
  @return TRUE when operation is success.
  @return FALSE when operation is failed.
*/
bool dsp_sdk_add_feature_table(stream_feature_ptr_t feature_ptr);


/*!
  @brief Add feature entry to feature table.
  @param[in] feature_ptr is a pointer to corresponding feature pointer.
  @param[in] feature_entry_ptr is a pointer to corresponding feature control pointer.
  @return TRUE when operation is success.
  @return FALSE when operation is failed.
*/
bool dsp_sdk_add_feature_ctrl(stream_feature_ptr_t feature_ptr, stream_feature_ctrl_entry_ptr_t feature_entry_ptr);


/*!
  @brief Configure Feature List Codec Type.
  @param[in] list is a pointer to corresponding feature type list.
  @param[in] codec_type is to identity corresponding codec type.
  @param[in] configure_codec is to identity corresponding codec configuration.
*/
void stream_feature_configure_type(stream_feature_list_ptr_t list, stream_feature_type_t codec_type, stream_feature_configure_codec_t configure_codec);


/*!
  @brief Configure Feature List Codec Output size.
  @param[in] list is a pointer to corresponding feature type list.
  @param[in] size is to identify the codec size.
  @param[in] configure_codec is to identify the codec's configuration.
*/
void stream_feature_configure_codec_output_size(stream_feature_list_ptr_t list, uint32_t size, stream_feature_configure_codec_t configure_codec);


/*!
  @brief Configure Feature List Codec Output Resolution.
  @param[in] list is a pointer to corresponding feature type list.
  @param[in] resolution is to identify the codec's resolution.
  @param[in] configure_codec is to identify the codec's configuration.
*/
void stream_feature_configure_resolution(stream_feature_list_ptr_t list, uint8_t resolution, stream_feature_configure_codec_t configure_codec);


/*!
  @brief Configure Feature List SRC
  @param[in] list is a pointer to corresponding feature type list.
  @param[in] in_res is the resolution of SRC's input.
  @param[in] out_res is the resolution of SRC's output.
  @param[in] src_out_sampling is sampling rate of SRC's output.
  @param[in] src_out_length is length of of SRC's output.
*/
void stream_feature_configure_src(stream_feature_list_ptr_t list, stream_resolution_t in_res, stream_resolution_t out_res, uint32_t src_out_sampling, uint32_t src_out_length);


/*!
  @brief reinitialize stream when an error occurs during the procedure.
  @param[in] para is a pointer to corresponding feature.
*/
void stream_feature_reinitialize(void *para);

/**
 * @defgroup STREAM_INTERFACE Interface
 * @{
 * This section introduces the APIs for algorithm integration.
 * - \b Introduction \n
 * The feature algorithm is separate to the codec and function. Each type uses different APIs. \n
 * The method of creating customer's codec is introduced in \ref STREAM_CODEC, and creating customer's function is introduced in \ref STREAM_FUNCTION.
 * - \b Stream \b interface \b architecture \n
 * The stream interface architecture is shown below:\n
 * @image html stream_interface_architecture.png
 */

/**
 * @defgroup STREAM_CODEC Codec
 * @{
 * This section introduces the DSP customization APIs for codecs.
 *
 *
 * @section STREAM_HOWTO_CREATE_CODEC How to create codec
 * The user must complete the following procedure to create a customized codec. For example:
 * - \b Step \b 1. Prepare entry of codec:\n
 *     1. Initialization\n
 *     2. Process\n
 *     .
 * The entry follows #stream_feature_entry_t.\n
 * The stream provides #stream_feature_reinitialize() to do initialization again when an error occurs, so there is no need to provide de-initialization api.\n
 * Note: The dynamic memory allocation is not allowed in procedure. The working buffer pointer get from #stream_codec_get_workingbuffer().
 * @code
 * typedef struct stream_codec_sample_instance_u
 * {
 *     uint8_t scratch_memory[128];
 *     uint8_t output[512];
 *     bool memory_check;
 *     bool reset_check;
 * } stream_codec_sample_instance_t, *stream_codec_sample_instance_ptr_t;
 *
 *
 * bool stream_codec_decoder_sample_initialize(void *para)
 * {
 *     stream_codec_sample_instance_ptr_t codec_sample_pointer;
 *
 *     //Get working buffer pointer
 *     codec_sample_pointer = (stream_codec_sample_instance_ptr_t)stream_codec_get_workingbuffer(para);
 *
 *     //Do initialize
 *     codec_sample_pointer->memory_check = true;
 *     codec_sample_pointer->reset_check = false;
 *     memset(codec_sample_pointer->scratch_memory, 0xFF, 128);
 *
 *     //return 0 when the initialization process is successful.
 *     return 0;
 * }
 *
 *
 * bool stream_codec_decoder_sample_process(void *para)
 * {
 *     stream_codec_sample_instance_ptr_t codec_sample_pointer;
 *     uint8_t *pattern_input_pointer;
 *     int16_t *output_l_pointer, *output_r_pointer;
 *     uint32_t input_length, output_length;
 *
 *     //Get working buffer pointer, stream buffer pointer, and length
 *     codec_sample_pointer = (stream_codec_sample_instance_ptr_t)stream_codec_get_workingbuffer(para);
 *     pattern_input_pointer = stream_codec_get_input_buffer(para, 1);
 *     output_l_pointer = stream_codec_get_output_buffer(para, 1);
 *     output_r_pointer = stream_codec_get_output_buffer(para, 2);
 *     input_length = stream_codec_get_input_size(para);
 *     output_length = stream_codec_get_output_size(para);
 *
 *
 *     //Call decoder
 *     //output sample rate : 16kHz
 *     //output resolution  : 16-bit
 *     output_length = codec_decoder_sample_api(pattern_input_pointer, output_l_pointer, output_r_pointer, input_length);
 *
 *     //Check decoder output
 *     if (output_length == 0) {
 *        //Do reinitialize when an error occurs.
 *        stream_feature_reinitialize(para);
 *     }
 *
 *     //Check expected resolution
 *     if (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT) {
 *        dsp_converter_16bit_to_32bit((int32_t*)output_l_pointer, (int16_t*)output_l_pointer, output_length/sizeof(int16_t));
 *        dsp_converter_16bit_to_32bit((int32_t*)output_r_pointer, (int16_t*)output_r_pointer, output_length/sizeof(int16_t));
 *        output_length = output_length*2;
 *     }
 *
 *     //Modify stream buffering format
 *     stream_codec_modify_output_samplingrate(para, FS_RATE_16K);
 *     stream_codec_modify_output_size(para, output_length);
 *     stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
 *
 *     //return 0 when the process is successful.
 *     return 0;
 * }
 * @endcode
 *
 * - \b Step \b 2. Add new feature type in #stream_feature_type_t.
 * @code
 * CODEC_DECODER_CELT_HD   = 0x09,             // decoder type: Reserved.
 * CODEC_DECODER_VENDOR    = 0x0A,             // decoder type: Vendor defined.
 * CODEC_DECODER_SAMPLE,                       // decoder type: example for the customer.
 * @endcode
 *
 * - \b Step \b 3. Define new feature codec, The structure folllows #stream_feature_t.\n
 * Note: The \ref stream_feature_t.memory_size is working buffer size.
 * @code
 * #define CODEC_SAMPLE_MEMORY_SIZE        sizeof(stream_codec_sample_instance_t)
 * #define CODEC_OUTPUT_SIZE               1024
 *
 * //                                                             feature_type,                memory_size, codec_output_size,                       initialize_entry,                       process_entry
 * stream_feature_codec_t stream_feature_sample_codec = { CODEC_DECODER_SAMPLE,   CODEC_SAMPLE_MEMORY_SIZE, CODEC_OUTPUT_SIZE, stream_codec_decoder_sample_initialize, stream_codec_decoder_sample_process};
 * @endcode
 *
 * - \b Step \b 4. Call #dsp_sdk_add_feature_table() to add to #stream_feature_table. \n
 * The sample code at #dsp_sdk_initialize().
 * @code
 * dsp_sdk_add_feature_table(&stream_feature_sample_codec);
 * @endcode
 *
 * - \b Step \b 5. Add feature type to feature list. The new codec is then available with the stream process.  \n
 * The sample code is replaced codec of prompt.
 * @code
 * stream_feature_list_t stream_feature_list_prompt[] =
 * {
 *     CODEC_DECODER_SAMPLE,       //New decoder
 *     FUNC_END,
 * };
 * @endcode
 */

/*!
  @brief Get corresponding feature type of this codec.
  @param[in] para a pointer to corresponding stream.
  @return codec's feature type.
*/
uint8_t stream_codec_get_type(void *para);

/*!
  @brief Get corresponding DSP task ID of this codec.
  @param[in] para a pointer to corresponding stream.
  @return DSP task ID.
*/
void* stream_codec_get_task(void *para);

/*!
  @brief Get the channel number of the stream's input in this codec.
  @param[in] para a pointer to corresponding stream.
  @return channel number.
*/
uint8_t stream_codec_get_input_channel_number(void* para);

/*!
  @brief Get the sampling rate of the stream's input in this codec.
  @param[in] para a pointer to corresponding codec.
  @return sampling rate.
*/
uint8_t stream_codec_get_input_samplingrate(void* para);

/*!
  @brief Get corresponding size of the input stream for this codec.
  @param[in] para a pointer to corresponding codec.
  @return stream size.
*/
uint16_t stream_codec_get_input_size(void* para);

/*!
  @brief Get corresponding input stream size pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream size pointer.
*/
uint16_t* stream_codec_get_input_size_pointer(void *para);

/*!
  @brief Modify input stream size for this codec.
  @param[in] para a pointer to corresponding codec.
  @param[in] in_size input parameter for changing stream size.
  @return TRUE if the size is smaller or equal to allocated memory.
  @return FALSE if the size is lager than allocated memory.
*/
bool stream_codec_modify_input_size(void* para, uint16_t in_size);

/*!
  @brief Get the first input stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream pointer.
*/
void* stream_codec_get_1st_input_buffer(void* para);

/*!
  @brief Get the second input stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream pointer.
*/
void* stream_codec_get_2nd_input_buffer(void* para);

/*!
  @brief Get the third input stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream pointer.
*/
void* stream_codec_get_3rd_input_buffer(void* para);

/*!
  @brief Get the fourth input stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream pointer.
*/
void* stream_codec_get_4th_input_buffer(void* para);

/*!
  @brief Get specific number of stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @param[in] channel channel number.
  @return Sream pointer.
*/
void* stream_codec_get_input_buffer(void* para, uint32_t channel);

/*!
  @brief Get the channel number of the stream's output in this codec.
  @param[in] para a pointer to corresponding stream.
  @return channel number.
*/
uint8_t stream_codec_get_output_channel_number(void* para);

/*!
  @brief Get the sampling rate of the stream's output in this codec.
  @param[in] para a pointer to corresponding codec.
  @return sampling rate.
*/
uint8_t stream_codec_get_output_samplerate(void* para);

/*!
  @brief Get the pointer to the sampling rate of the stream's output in this codec.
  @param[in] para a pointer to corresponding codec.
  @return the pointer of sampling rate.
*/
uint8_t* stream_codec_get_output_samplerate_pointer(void *para);

/*!
  @brief Modify the sampling rate of output stream for this codec.
  @param[in] para a pointer to corresponding codec.
  @param[in] sampling_rate input parameter for changing sampling rate.
  @return TRUE
*/
bool stream_codec_modify_output_samplingrate(void* para, uint8_t sampling_rate);

/*!
  @brief Get corresponding size of the output stream for this codec.
  @param[in] para a pointer to corresponding codec.
  @return stream size.
*/
uint16_t stream_codec_get_output_size(void* para);

/*!
  @brief Get corresponding output stream's size pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream size pointer.
*/
uint16_t* stream_codec_get_output_size_pointer(void *para);

/*!
  @brief Modify output stream size for this codec.
  @param[in] para a pointer to corresponding codec.
  @param[in] out_size input parameter for changing stream size.
  @return TRUE if the size is smaller or equal to allocated memory
  @return FALSE if the size is lager than allocated memory.
*/
bool stream_codec_modify_output_size(void* para, uint16_t out_size);

/*!
  @brief Get 1st output stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream pointer.
*/
void* stream_codec_get_1st_output_buffer(void* para);

/*!
  @brief Get 2nd output stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream pointer.
*/
void* stream_codec_get_2nd_output_buffer(void* para);

/*!
  @brief Get 3rd output stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream pointer.
*/
void* stream_codec_get_3rd_output_buffer(void* para);

/*!
  @brief Get 4th output stream pointer for this codec.
  @param[in] para a pointer to corresponding codec.
  @return Sream pointer.
*/
void* stream_codec_get_4th_output_buffer(void* para);

/*!
  @brief Get the specified output stream pointer with channel number for this codec.
  @param[in] para a pointer to corresponding codec.
  @param[in] channel channel number.
  @return Sream pointer.
*/
void* stream_codec_get_output_buffer(void* para, uint32_t channel);

/*!
  @brief Get the pointer of the codec's memory instance.
  @param[in] para a pointer to corresponding codec.
  @return memory pointer.
*/
void* stream_codec_get_workingbuffer(void* para);

/*!
  @brief Get the resolution of the codec's input.
  @param[in] para a pointer to corresponding codec.
  @return resolution. (stream_resolution_t)
*/
uint8_t stream_codec_get_input_resolution(void* para);

/*!
  @brief Modify the resolution of codec process.
  @param[in] para a pointer to corresponding codec.
  @param[in] resolution resolution. (value is defined in stream_resolution_t)
  @return TRUE if it is successful.
  @return FALSE if it fails
*/
bool stream_codec_modify_resolution(void* para, uint8_t resolution);

/*!
  @brief Get the resolution of codec process.
  @param[in] para a pointer to corresponding codec.
  @return the resolution of codec process.
*/
uint8_t stream_codec_get_resolution(void* para);

/*!
  @brief Get the resolution of codec output.
  @param[in] para a pointer to corresponding codec.
  @return the resolution of codec output.
*/
uint8_t stream_codec_get_output_resolution(void* para);

/*!
  @brief Get the numbering of codec of feature list.
  @param[in] para a pointer to corresponding codec.
  @return the numbering.
*/
uint32_t stream_codec_get_numbering(void *para);

/*!
  @brief Get status of mute flag.
  @param[in] para a pointer to corresponding codec.
  @return the status of mute flag.
*/
uint32_t stream_codec_get_mute_flag(void *para);

/**
 * @}
 */



/**
 * @defgroup STREAM_FUNCTION Function
 * @{
 * This section introduces the DSP customization APIs for functions.
 *
 *
 * @section STREAM_HOWTO_CREATE_FUNCTION How to create function
 * The user must complete the following procedure to create a customized function. For example:
 * - \b Step \b 1. Prepare entry of function:\n
 *     1. Initialization\n
 *     2. Process\n
 *     .
 * The entry follows #stream_feature_entry_t.\n
 * The stream provides #stream_feature_reinitialize() to do initialization again when an error occurs, so there is no need to provide de-initialization api.\n
 * Note: The dynamic memory allocation is not allowed in procedure. The working buffer pointer get from #stream_function_get_working_buffer().
 * @code
 * typedef struct stream_function_sample_instance_u
 * {
 *     uint32_t coefficient_size;
 *     uint8_t filter[32];
 *     int16_t buffer_l[512];
 *     int16_t buffer_r[512];
 * } stream_function_sample_instance_t, *stream_function_sample_instance_ptr_t;
 *
 *
 * bool stream_function_sample_initialize(void *para)
 * {
 *     stream_function_sample_instance_ptr_t function_sample_pointer;
 *
 *     //Get working buffer pointer
 *     function_sample_pointer = (stream_function_sample_instance_ptr_t)stream_function_get_working_buffer(para);
 *
 *     //Do initialize
 *     memcpy(function_sample_pointer->filter, coefficient_table_16k, 13*sizeof(uint16_t));
 *     function_sample_pointer->coefficient_size = 13;
 *
 *     //return 0 when the initialization process is successful.
 *     return 0;
 * }
 *
 *
 * bool stream_function_sample_process(void *para)
 * {
 *     int16_t *l_pointer, *r_pointer;
 *     uint32_t frame_length;
 *     stream_function_sample_instance_ptr_t function_sample_pointer;
 *
 *     //Get working buffer pointer, stream buffer pointer, and length
 *     function_sample_pointer = (stream_function_sample_instance_ptr_t)stream_function_get_working_buffer(para);
 *     l_pointer = stream_function_get_inout_buffer(para, 1);
 *     r_pointer = stream_function_get_inout_buffer(para, 2);
 *     frame_length = stream_function_get_output_size(para);
 *
 *     //Call function API
 *     function_sample_api(function_sample_pointer,
 *                         l_pointer,
 *                         r_pointer,
 *                         function_sample_pointer->buffer_l,
 *                         function_sample_pointer->buffer_r,
 *                         frame_length);
 *
 *
 *     //return 0 when the process is successful.
 *     return 0;
 * }
 * @endcode
 *
 * - \b Step \b 2. Add new feature type in #stream_feature_type_t.
 * @code
 * FUNC_CH_SEL             = 0x33,             //< Basic FUNCTION type: CH SEL.
 * FUNC_MUTE_SMOOTHER      = 0x34,             //< Basic FUNCTION type: mute smoother.
 * FUNC_SAMPLE,                                //< FUNCTION type: example for the customer.
 * @endcode
 *
 * - \b Step \b 3. Define new feature function. The structure folllows #stream_feature_t.\n
 * Note: The \ref stream_feature_t.memory_size is working buffer size.
 * @code
 * #define FUNCTION_SAMPLE_MEMORY_SIZE     sizeof(stream_function_sample_instance_t)
 *
 * //                                                            feature_type,                 memory_size,   0(Must equal to zero),                  initialize_entry,                  process_entry
 * stream_feature_function_t  stream_feature_sample_function = {  FUNC_SAMPLE, FUNCTION_SAMPLE_MEMORY_SIZE,                       0, stream_function_sample_initialize, stream_function_sample_process};
 * @endcode
 *
 * - \b Step \b 4. Call #dsp_sdk_add_feature_table() to add to #stream_feature_table. \n
 * the sample code at #dsp_sdk_initialize()
 * @code
 * dsp_sdk_add_feature_table(&stream_feature_sample_function);
 * @endcode
 *
 * - \b Step \b 5. Add feature type to feature list. The effect of the new function is available to the stream process. \n
 * The sample code is added to HFP downlink.
 * @code
 * stream_feature_list_t stream_feature_list_hfp_downlink[] =
 * {
 *     CODEC_DECODER_MSBC,
 *     FUNC_PLC,
 *     FUNC_RX_NR,
 *     FUNC_RX_WB_DRC,
 *     FUNC_SAMPLE,                // Add New feature type
 *     FUNC_CLK_SKEW_HFP_DL,
 *     FUNC_END,
 * };
 * @endcode
 */


/*!
  @brief Get the Type of function.
  @param[in] para a pointer to corresponding function.
  @return the function's feature type.
*/
uint8_t stream_function_get_type(void *para);

/*!
  @brief Get the Type of decoder with respect the feature list.
  @param[in] para a pointer to corresponding function.
  @return the function's feature type.
*/
uint8_t stream_function_get_decoder_type(void *para);

/*!
  @brief Get the OS Task handle of function.
  @param[in] para a pointer to corresponding function.
  @return Task handle.
*/
void* stream_function_get_task(void *para);

/*!
  @brief Get the channel number of the stream in this function.
  @param[in] para a pointer to corresponding function.
  @return channel number.
*/
uint8_t stream_function_get_channel_number(void* para);

/*!
  @brief Get the sampling rate of the stream in this function.
  @param[in] para a pointer to corresponding function.
  @return sampling rate.
*/
uint8_t stream_function_get_samplingrate(void* para);

/*!
  @brief Get the size of the stream in this function.
  @param[in] para a pointer to corresponding function.
  @return the size of the stream.
*/
uint16_t stream_function_get_output_size(void* para);

/*!
  @brief Modify the sampling conversion ratio of the stream in this function.
  @param[in] para a pointer to corresponding function.
  @param[in] updown_rate identify updown ratio. (defined in #stream_feature_convert_samplerate_t)
  @return TRUE if it is successful.
  @return FALSE if it fails.
*/
bool stream_function_modify_samplingrateconversion_ratio(void* para, stream_feature_convert_samplerate_t updown_rate);


/*!
  @brief Get the device channel number to be output in the end.
  @param[in] para a pointer to corresponding function.
  @return channel number.
*/
uint8_t stream_function_get_device_channel_number(void* para);

/*!
  @brief Get the sofware defined channel number to be handled in callback.
  @param[in] para a pointer to corresponding function.
  @return channel number.
*/
uint8_t stream_function_get_software_channel_number(void* para);

/*!
  @brief Modify the size of the stream in this function.
  @param[in] para a pointer to corresponding function.
  @param[in] size identify size for modification.
  @return TRUE if it is successful.
  @return FALSE if it fails.
*/
bool stream_function_modify_output_size(void* para, uint16_t size);

/*!
  @brief Get 1st stream pointer for this function.
  @param[in] para a pointer to corresponding function.
  @return Sream pointer.
*/
void* stream_function_get_1st_inout_buffer(void* para);

/*!
  @brief Get 2nd stream pointer for this function.
  @param[in] para a pointer to corresponding function.
  @return Sream pointer.
*/
void* stream_function_get_2nd_inout_buffer(void* para);

/*!
  @brief Get 3rd stream pointer for this function.
  @param[in] para a pointer to corresponding function.
  @return Sream pointer.
*/
void* stream_function_get_3rd_inout_buffer(void* para);

/*!
  @brief Get 4th stream pointer for this function.
  @param[in] para a pointer to corresponding function.
  @return Sream pointer.
*/
void* stream_function_get_4th_inout_buffer(void* para);

/*!
  @brief Get 5th stream pointer for this function.
  @param[in] para a pointer to corresponding function.
  @return Sream pointer.
*/
void* stream_function_get_5th_inout_buffer(void* para);

/*!
  @brief Get 6th stream pointer for this function.
  @param[in] para a pointer to corresponding function.
  @return Sream pointer.
*/
void* stream_function_get_6th_inout_buffer(void* para);

/*!
  @brief Get the specific stream pointer for this function.
  @param[in] para a pointer to corresponding function.
  @param[in] channel channel number.
  @return Sream pointer.
*/
void* stream_function_get_inout_buffer(void* para, uint32_t channel);

/*!
  @brief Force stream re-enter even when stream input size is 0
  @param[in] para a pointer to corresponding function.
  @return memory pointer.
*/
bool stream_function_reenter_stream(void* para);

/*!
  @brief Get the pointer of the function's memory instance.
  @param[in] para a pointer to corresponding function.
  @return memory pointer.
*/
void* stream_function_get_working_buffer(void* para);

/*!
  @brief Modify the resolution of function process.
  @param[in] para a pointer to corresponding function.
  @param[in] resolution resolution. (defined in #stream_resolution_t)
  @return TRUE if it is successful.
  @return FALSE if it fails.
*/
bool stream_function_modify_output_resolution(void* para, uint8_t resolution);

/*!
  @brief Get the resolution of function process.
  @param[in] para a pointer to corresponding function.
  @return resolution. (defined in #stream_resolution_t)
*/
uint8_t stream_function_get_output_resolution(void* para);

/*!
  @brief Get the resolution of function's stream output.
  @param[in] para a pointer to corresponding function.
  @return resolution. (defined in #stream_resolution_t)
*/
uint8_t stream_function_get_sink_output_resolution(void* para);

/*!
  @brief Get the numbering of function process of feature list.
  @param[in] para a pointer to corresponding function.
  @return the numbering.
*/
uint32_t stream_function_get_numbering(void *para);

/**
 * @}
 */
/**
 * @}
 */


/**
 * @}
 * @}
 */

#endif /* _DSP_SDK_H_ */

