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

#include "config.h"
#include "dsp_feature_interface.h"
#include "dsp_stream_connect.h"
#include "dsp_audio_process.h"
#ifdef AIR_BT_HFP_CVSD_ENABLE
#include "cvsd_enc_interface.h"
#include "cvsd_dec_interface.h"
#endif
#ifdef AIR_BT_HFP_MSBC_ENABLE
#include "msbc_enc_interface.h"
#include "msbc_dec_interface.h"
#endif
#include "dprt_rt.h"
#ifdef MTK_BT_A2DP_SBC_ENABLE
#include "sbc_interface.h"
#endif /* MTK_BT_A2DP_SBC_ENABLE */
#ifdef MTK_BT_A2DP_AAC_ENABLE
#include "aac_dec_interface.h"
#endif /* MTK_BT_A2DP_AAC_ENABLE */
#include "compander_interface.h"
#ifdef AIR_VOICE_NR_ENABLE
#include "aec_nr_interface.h"
#endif
#include "clk_skew.h"
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#include "peq_interface.h"
#endif
#ifdef MTK_BT_AGC_USE_PIC
#include "agc_portable.h"
#endif


#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "lc3_enc_interface.h"
#include "lc3_dec_interface.h"
#endif

#if defined(MTK_3RD_PARTY_NR)
#include "tx_eq_portable.h"
#endif

#if defined(AIR_HFP_DNN_PATH_ENABLE) && defined(AIR_DNN_LIB_ENABLE)
#include "dnn_nr_portable.h"
#endif


#include "dsp_vendor_codec_sdk.h"


#include "ch_select_interface.h"
#include "mute_smooth_interface.h"

#if defined(AIR_ANC_WIND_DETECTION_ENABLE) && defined(AIR_WIND_DETECTION_USE_PIC)
#include "wind_interface.h"
#endif

#if defined(AIR_ANC_USER_UNAWARE_ENABLE) && defined(AIR_USER_UNAWARE_USE_PIC)
#include "user_unaware_interface.h"
#endif

#if defined(AIR_EC120_ENABLE)
#include "ec120_portable.h"
#endif

#if defined(AIR_AFC_ENABLE) && defined(AIR_AFC_USE_PIC)
#include "afc_portable.h"
#endif

#if defined(AIR_LD_NR_ENABLE) && defined(AIR_LD_NR_USE_PIC)
#include "ld_nr_portable.h"
#endif

#if defined(AIR_AT_AGC_ENABLE) && defined(AIR_AT_AGC_USE_PIC)
#include "at_agc_portable.h"
#endif
#include <string.h>


/******************************************************************************
 * Constant Definitions
 ******************************************************************************/



#define FEATURETABLE_DECODER_RESERVED_START_NUM     (CODEC_PCM_COPY)
#define FEATURETABLE_DECODER_RESERVED_END_NUM       (CODEC_PCM_COPY+0x0A)
#define FEATURETABLE_ENCODER_RESERVED_START_NUM     (DSP_ENCODER_TYPE)
#define FEATURETABLE_ENCODER_RESERVED_END_NUM       (DSP_ENCODER_TYPE+0x02)
#define FEATURETABLE_FUNC_RESERVED_START_NUM        (DSP_FUNC_TYPE)
#define FEATURETABLE_FUNC_RESERVED_END_NUM          (DSP_FUNC_TYPE+0x2B)


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
EXTERN VOID DSP_Callback_ChangeStreaming2Deinit(VOID* para);


/******************************************************************************
 * Function Declaration
 ******************************************************************************/
bool stream_pcm_copy_initialize  (void *para);
bool stream_function_end_initialize   (void *para);
bool MuteFunc   (void *para);
bool UpBy2Func  (void *para);
#ifdef PRELOADER_ENABLE
VOID CodecOpen  (VOID);
VOID CodecClose (VOID);
#endif


/******************************************************************************
 * Variables
 ******************************************************************************/
#if 0
/*===========================================================================================*/
// Codec blocks
// CodecOutSize must have value
stream_feature_table_t stream_feature_table[DSP_FEATURE_MAX_NUM] =
{

    /*FeatureType,                                MemSize,                      CodecOutSize,           *InitialEntry,     *ProcessEntry*/

    {CODEC_PCM_COPY,                                        0,                         sizeof(U32)*512,            stream_pcm_copy_initialize,      stream_pcm_copy_process},/*0x00 CODEC_PCM_COPY               */ /* Only copy in_ptr memory to out_ptr memory */
    {CODEC_DECODER_CVSD,         DSP_CVSD_DECODER_MEMSIZE,               480+DSP_SIZE_FOR_CLK_SKEW,    stream_codec_decoder_cvsd_initialize,         stream_codec_decoder_cvsd_process},/*0x01 stream_codec_decoder_cvsd_process,    */   // 120(NB), 240(WB)
    {CODEC_DECODER_MSBC,         DSP_MSBC_DECODER_MEMSIZE,               480+DSP_SIZE_FOR_CLK_SKEW,    stream_codec_decoder_msbc_initialize,         stream_codec_decoder_msbc_process},/*0x02 CODEC_DECODER_MSBC,      */
    {CODEC_DECODER_SBC,             DSP_SBC_CODEC_MEMSIZE,  sizeof(U32)*1024+DSP_SIZE_FOR_CLK_SKEW,     stream_codec_decoder_sbc_initialize,          stream_codec_decoder_sbc_process},/*0x03 CODEC_DECODER_SBC,       */
    {CODEC_DECODER_AAC,           DSP_AAC_DECODER_MEMSIZE,  sizeof(U32)*1024+DSP_SIZE_FOR_CLK_SKEW,     stream_codec_decoder_aac_initialize,          stream_codec_decoder_aac_process},/*0x04 CODEC_DECODER_AAC,       */
    {CODEC_DECODER_MP3,                                 0,                                    0xFF,                 NULL,                 NULL},/*0x05 _Reserved,       */
    {CODEC_DECODER_EC,                                  0,                                    0xFF,                 NULL,                 NULL},/*0x06 _Reserved,       */
    {CODEC_DECODER_UART,                                0,                                    0xFF,                 NULL,                 NULL},/*0x07 _Reserved,       */
    {CODEC_DECODER_UART16BIT,                           0,                                    0xFF,                 NULL,                 NULL},/*0x08 _Reserved,       */
    {CODEC_DECODER_CELT_HD,                             0,                                    0xFF,                 NULL,                 NULL},/*0x09 _Reserved,       */
#ifdef MTK_BT_A2DP_VENDOR_ENABLE
    {CODEC_DECODER_VENDOR,              VENDOR_HANDLE_SIZE,  sizeof(U32)*1024+DSP_SIZE_FOR_CLK_SKEW, STREAM_CODEC_DECODER_VENDOR_INITIALIZE,       STREAM_CODEC_DECODER_VENDOR_PROCESS},/*0x0A _CODEC_DECODER_VENDOR,       */
#else
    {CODEC_DECODER_VENDOR,                              0,                                    0xFF,                 NULL,                 NULL},/*0x0A _CODEC_DECODER_VENDOR,       */
#endif
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x0B _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x0C _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x0D _Reserved,       */
#ifdef AIR_BT_CODEC_BLE_ENABLED
    {CODEC_DECODER_LC3,           DSP_LC3_DECODER_MEMSIZE,               960+DSP_SIZE_FOR_CLK_SKEW,    stream_codec_decoder_lc3_initialize,    stream_codec_decoder_lc3_process},/*0x0E CODEC_DECODER_LC3,       */
#else
    {CODEC_DECODER_LC3,                                 0,                                    0xFF,                 NULL,                 NULL},/*0x0E CODEC_DECODER_LC3,       */
#endif
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x0F _Reserved,       */
    {CODEC_ENCODER_CVSD,         DSP_CVSD_ENCODER_MEMSIZE,                                     240,    stream_codec_encoder_cvsd_initialize,         stream_codec_encoder_cvsd_process},/*0x10 CVSD_Encoder,    */   // 60(NB), 120(WB)
    {CODEC_ENCODER_MSBC,         DSP_MSBC_ENCODER_MEMSIZE,                                     240,    stream_codec_encoder_msbc_initialize,         stream_codec_encoder_msbc_process},/*0x11 CODEC_ENCODER_MSBC,      */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x12 _Reserved,       */
#ifdef AIR_BT_CODEC_BLE_ENABLED
    {CODEC_ENCODER_LC3,           DSP_LC3_ENCODER_MEMSIZE,               960+DSP_SIZE_FOR_CLK_SKEW,    stream_codec_encoder_lc3_initialize,    stream_codec_encoder_lc3_process},/*0x13 CODEC_ENCODER_LC3,       */
#else
    {CODEC_ENCODER_LC3,                                 0,                                    0xFF,                 NULL,                 NULL}, /*0x13 CODEC_ENCODER_LC3,       */
#endif
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x14 _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x15 _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x16 _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x17 _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x18 _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x19 _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x1A _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x1B _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x1C _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x1D _Reserved,       */
    {0,                                                 0,                                    0xFF,                 NULL,                 NULL},/*0x1E _Reserved,       */
    {DSP_SRC,                             DSP_SRC_MEMSIZE,                                       0,         stream_function_src_initialize,      stream_function_src_process},/*0x1F  DSP_SRC,       */
    {FUNC_END,                                          0,                                       0,             stream_function_end_initialize,              stream_function_end_process},/*0x20 FUNC_END,                */

    {FUNC_RX_WB_DRC,                   DSP_DRC_VO_MEMSIZE,                                       0, stream_function_drc_voice_rx_wb_initialize,  stream_function_drc_voice_rx_process},/*0x21 FUNC_RX_WB_DRC,          */
    {FUNC_RX_NB_DRC,                   DSP_DRC_VO_MEMSIZE,                                       0, stream_function_drc_voice_rx_nb_initialize,  stream_function_drc_voice_rx_process},/*0x22 FUNC_RX_NB_DRC,          */
    {FUNC_TX_WB_DRC,                   DSP_DRC_VO_MEMSIZE,                                       0, stream_function_drc_voice_tx_wb_initialize,  stream_function_drc_voice_tx_process},/*0x23 FUNC_TX_WB_DRC,       */
    {FUNC_TX_NB_DRC,                   DSP_DRC_VO_MEMSIZE,                                       0, stream_function_drc_voice_tx_nb_initialize,  stream_function_drc_voice_tx_process},/*0x24 FUNC_TX_NB_DRC,       */
    {FUNC_RX_NR,                                        0,                                       0, stream_function_aec_nr_initialize         ,     stream_function_nr_process},/*0x25 FUNC_RX_NR,                */
    {FUNC_TX_NR,                                        0,                                       0, stream_function_aec_nr_initialize         ,     stream_function_aec_process},/*0x26 FUNC_TX_NR,                */
    {FUNC_CLK_SKEW_UL, DSP_CLK_SKEW_MEMSIZE*3+DSP_CLK_SKEW_TEMP_BUF_SIZE,                        0, stream_function_clock_skew_uplink_initialize  , stream_function_clock_skew_uplink_process},/*0x27 FUNC_CLK_SKEW_UL,        */
    {FUNC_CLK_SKEW_DL, DSP_CLK_SKEW_MEMSIZE*4+DSP_CLK_SKEW_TEMP_BUF_SIZE,                        0, stream_function_clock_skew_downlink_initialize  , stream_function_clock_skew_downlink_process},/*0x28 FUNC_CLK_SKEW_DL,        */

    {FUNC_MIC_SW_GAIN,                                  0,                                       0,             stream_function_gain_initialize,   stream_function_gain_process},/*0x29 FUNC_MIC_SW_GAIN,        */
    {FUNC_PLC,                      DSP_VOICE_PLC_MEMSIZE,                                       0,       stream_function_plc_initialize,     stream_function_plc_process},/*0x2A FUNC_PLC,                */
    {FUNC_CLK_SKEW_HFP_DL, DSP_CLK_SKEW_MEMSIZE*2+DSP_CLK_SKEW_TEMP_BUF_SIZE,                   0, stream_function_clock_skew_downlink_initialize  , stream_function_clock_skew_hfp_downlink_process},/*0x2B FUNC_CLK_SKEW_DL,   */
    {FUNC_PROC_SIZE_CONV,                               0,                                       0, stream_function_size_converter_initialize,      stream_function_size_converter_process},/*0x2C FUNC_PROC_SIZE_CONV     */ /* Convert Processing size to give fixed given size*/
    {FUNC_JOINT,                                        0,                                       0,                 NULL,                 NULL},/*0x2D FUNC_JOINT,       */
    {FUNC_BRANCH,                                       0,                                       0,                 NULL,                 NULL},/*0x2E FUNC_BRANCH,       */
    {FUNC_MUTE,                                         0,                                       0,                 NULL,                 NULL},/*0x2F FUNC_MUTE,       */
    {FUNC_DRC,                         DSP_DRC_AU_MEMSIZE,                                       0,       stream_function_drc_audio_initialize,    stream_function_drc_audio_process},/*0x30 FUNC_DRC,       */
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
    {FUNC_PEQ,                            DSP_PEQ_MEMSIZE,                                       0,             stream_function_peq_initialize,          stream_function_peq_process},/*0x31 FUNC_PEQ,       */
#else
    {FUNC_PEQ,                                          0,                                       0,                 NULL,                 NULL},/*0x31 FUNC_PEQ,       */
#endif
    {FUNC_LPF,                                          0,                                       0,                 NULL,                 NULL},/*0x32 FUNC_LPF,       */
    {FUNC_CH_SEL,                                       0,                                       0,       stream_function_channel_selector_initialize,    stream_function_channel_selector_process},/*0x33 FUNC_CH_SEL,       */
    {FUNC_MUTE_SMOOTHER,                                0,                                       0,     stream_function_mute_smooth_initialize,  stream_function_mute_smooth_process},/*0x34 FUNC_MUTE_SMOOTHER,       */


};


/*===========================================================================================*/
#endif
// Feature List
#ifdef PRELOADER_ENABLE

#if !PIC_LOGPRINT
uint32_t fake_printf( const char * format, ... )
{
    UNUSED(format);
    return 0;
}
#endif

#ifdef MTK_BT_A2DP_MSBC_USE_PIC
extern void msbc_dec_library_load();
extern void msbc_dec_library_unload();
extern void msbc_enc_library_load();
extern void msbc_enc_library_unload();
#endif

#ifdef MTK_BT_A2DP_CVSD_USE_PIC
extern void cvsd_dec_library_load();
extern void cvsd_dec_library_unload();
extern void cvsd_enc_library_load();
extern void cvsd_enc_library_unload();
#endif

#ifdef MTK_BT_A2DP_AAC_USE_PIC
extern void aac_library_load();
extern void aac_library_unload();
#endif

#ifdef MTK_BT_A2DP_SBC_USE_PIC
extern void sbc_library_load();
extern void sbc_library_unload();
#endif

#ifdef MTK_BT_A2DP_CPD_USE_PIC
extern void cpd_library_load();
extern void cpd_library_unload();
#endif

#ifdef MTK_BT_A2DP_ECNR_USE_PIC
extern void ecnr_library_load();
extern void ecnr_library_unload();
#if defined(MTK_3RD_PARTY_NR)
extern void tx_eq_library_load();
extern void tx_eq_library_unload();
#endif
#endif

#ifdef MTK_BT_CLK_SKEW_USE_PIC
extern void clk_skew_library_load();
extern void clk_skew_library_unload();
#endif

#ifdef MTK_BT_PEQ_USE_PIC
extern void peq_library_load();
extern void peq_library_unload();
#endif

#ifdef MTK_PLC_USE_PIC
extern void smart_pitch_plc_library_load();
extern void smart_pitch_plc_library_unload();
#endif

#ifdef MTK_BT_AGC_USE_PIC
extern void agc_library_load();
extern void agc_library_unload();
#endif


#ifdef MTK_WWE_USE_PIC
extern void wwe_library_load();
extern void wwe_library_unload();
#endif


#if defined(AIR_HFP_DNN_PATH_ENABLE) && defined(AIR_DNN_LIB_ENABLE)
extern void dnn_nr_library_load();
extern void dnn_nr_library_unload();
#endif


#if (defined(AIR_BT_CODEC_BLE_ENABLED) || defined(AIR_BT_CODEC_BLE_V2_ENABLED)) && defined(MTK_BT_A2DP_LC3_USE_PIC)
extern void lc3_codec_library_load(void);
extern void lc3_codec_library_unload(void);

extern void lc3i_codec_library_load(void);
extern void lc3i_codec_library_unload(void);
#endif

#ifdef MTK_BT_CELT_USE_PIC
extern void celt_library_load(void);
extern void celt_library_unload(void);
#endif

#ifdef AIR_EC120_ENABLE
extern void ec120_library_load(void);
extern void ec120_library_unload(void);
#endif

#ifdef AIR_ECNR_PREV_PART_USE_PIC_ENABLE
void ecnr_prev_part_library_load(void);
void ecnr_prev_part_library_unload(void);
#endif

#ifdef AIR_ECNR_POST_PART_USE_PIC_ENABLE
extern void ecnr_post_ec_library_load(void);
extern void ecnr_post_ec_library_unload(void);
#endif

stream_feature_ctrl_entry_t DSP_FeatureControl[DSP_FEATURE_MAX_NUM] = {
    {CodecOpen,         CodecClose},        /*0x00 CODEC_PCM_COPY,              */
#ifdef MTK_BT_A2DP_CVSD_USE_PIC
    {cvsd_dec_library_load, cvsd_dec_library_unload},/*0x01 CODEC_DECODER_CVSD,            */
#else
    {NULL, NULL},/*0x01 CODEC_DECODER_CVSD,            */
#endif
#ifdef MTK_BT_A2DP_MSBC_USE_PIC
    {msbc_dec_library_load, msbc_dec_library_unload},/*0x02 CODEC_DECODER_MSBC,      */
#else
    {NULL, NULL},/*0x02 CODEC_DECODER_MSBC,      */
#endif
#ifdef MTK_BT_A2DP_SBC_USE_PIC
    {sbc_library_load,  sbc_library_unload}, /*0x03 CODEC_DECODER_SBC,       */
#else
    {NULL, NULL}, /*0x03 CODEC_DECODER_SBC,       */
#endif
#ifdef MTK_BT_A2DP_AAC_USE_PIC
    {aac_library_load,  aac_library_unload}, /*0x04 CODEC_DECODER_AAC,       */
#else
    {NULL, NULL},/*0x04 CODEC_DECODER_AAC,       */
#endif
    {NULL,              NULL},              /*0x05 _Reserved,               */
    {NULL,              NULL},              /*0x06 _Reserved,               */
    {NULL,              NULL},              /*0x07 _Reserved,               */
    {NULL,              NULL},              /*0x08 _Reserved,               */
#ifdef MTK_BT_CELT_USE_PIC
    {celt_library_load,              celt_library_unload},              /*0x09 CODEC_DECODER_CELT_HD,               */
#else
    {NULL, NULL},/*0x11 CODEC_ENCODER_MSBC,      */
#endif

#ifdef MTK_BT_A2DP_VENDOR_USE_PIC
    {vendor_library_load, vendor_library_unload},/*0x0A CODEC_DECODER_VENDOR,               */
#else
    {NULL, NULL}, /*0x0A CODEC_DECODER_VENDOR,               */
#endif
    {vendor_1_library_load, vendor_1_library_unload},  /*0x0B CODEC_DECODER_VENDOR_1,               */
    {vendor_2_library_load, vendor_2_library_unload},  /*0x0C CODEC_DECODER_VENDOR_2,               */
    {vendor_3_library_load, vendor_3_library_unload},  /*0x0D CODEC_DECODER_VENDOR_3,               */
#if (defined(AIR_BT_CODEC_BLE_ENABLED) && defined(MTK_BT_A2DP_LC3_USE_PIC))
    {lc3i_codec_library_load, lc3i_codec_library_unload},/*0x0E CODEC_DECODER_LC3,      */
#elif (defined(AIR_BT_CODEC_BLE_V2_ENABLED) && defined(MTK_BT_A2DP_LC3_USE_PIC))
    {lc3_codec_library_load, lc3_codec_library_unload},/*0x0E CODEC_DECODER_LC3,      */
#else
    {NULL,              NULL},              /*0x0E CODEC_DECODER_LC3,      */
#endif
    {NULL,              NULL},              /*0x0F _Reserved,               */
#ifdef MTK_BT_A2DP_CVSD_USE_PIC
    {cvsd_enc_library_load, cvsd_enc_library_unload},/*0x10 CVSD_Encoder,            */
#else
    {NULL, NULL},/*0x10 CVSD_Encoder,            */
#endif
#ifdef MTK_BT_A2DP_MSBC_USE_PIC
    {msbc_enc_library_load, msbc_enc_library_unload},/*0x11 CODEC_ENCODER_MSBC,      */
#else
    {NULL, NULL},/*0x11 CODEC_ENCODER_MSBC,      */
#endif
#ifdef MTK_BT_CELT_USE_PIC
    {celt_library_load,              celt_library_unload},              /*0x12 CODEC_ENCODER_OPUS,               */
#else
    {NULL, NULL},/*0x11 CODEC_ENCODER_OPUS,      */
#endif
#if (defined(AIR_BT_CODEC_BLE_ENABLED) || defined(AIR_BT_CODEC_BLE_V2_ENABLED)) && defined(MTK_BT_A2DP_LC3_USE_PIC)
    {lc3i_codec_library_load, lc3i_codec_library_unload},/*0x13 CODEC_ENCODER_LC3,      */
#else
    {NULL,              NULL},              /*0x13 CODEC_ENCODER_LC3,      */
#endif
#if (defined(AIR_BT_CODEC_BLE_ENABLED) || defined(AIR_BT_CODEC_BLE_V2_ENABLED)) && defined(MTK_BT_A2DP_LC3_USE_PIC)
    {lc3_codec_library_load, lc3_codec_library_unload},/*0x14 CODEC_ENCODER_LC3_BRANCH,      */
#else
    {NULL,              NULL},              /*0x14 CODEC_ENCODER_LC3_BRANCH,               */
#endif
    {NULL,              NULL},              /*0x15 _Reserved,               */
    {NULL,              NULL},              /*0x16 _Reserved,               */
    {NULL,              NULL},              /*0x17 _Reserved,               */
    {NULL,              NULL},              /*0x18 _Reserved,               */
    {NULL,              NULL},              /*0x19 _Reserved,               */
    {NULL,              NULL},              /*0x1A _Reserved,               */
    {NULL,              NULL},              /*0x1B _Reserved,               */
    {NULL,              NULL},              /*0x1C _Reserved,               */
    {NULL,              NULL},              /*0x1D _Reserved,               */
    {NULL,              NULL},              /*0x1E _Reserved,               */
    {NULL,              NULL},              /*0x1F _Reserved,               */
    {NULL,              NULL},              /*0x20 FUNC_END,                */
#ifdef MTK_BT_A2DP_CPD_USE_PIC
    {cpd_library_load,    cpd_library_unload},   /*0x21 FUNC_RX_WB_DRC,          */
    {cpd_library_load,    cpd_library_unload},   /*0x22 FUNC_RX_NB_DRC,          */
    {cpd_library_load,    cpd_library_unload},   /*0x23 FUNC_TX_WB_DRC,          */
    {cpd_library_load,    cpd_library_unload},   /*0x24 FUNC_TX_NB_DRC,          */
#else
    {NULL, NULL},/*0x21 FUNC_RX_WB_DRC,          */
    {NULL, NULL},/*0x22 FUNC_RX_NB_DRC,          */
    {NULL, NULL},/*0x23 FUNC_TX_WB_DRC,          */
    {NULL, NULL},/*0x24 FUNC_TX_NB_DRC,          */
#endif
#ifdef MTK_BT_A2DP_ECNR_USE_PIC
    {ecnr_library_load,       ecnr_library_unload},      /*0x25 FUNC_RX_NR,              */
    {ecnr_library_load,       ecnr_library_unload},      /*0x26 FUNC_TX_NR,              */
#else
    {NULL, NULL},/*0x25 FUNC_RX_NR,              */
    {NULL, NULL},/*0x26 FUNC_TX_NR,              */
#endif
#ifdef MTK_BT_CLK_SKEW_USE_PIC
    {clk_skew_library_load,       clk_skew_library_unload},              /*0x27 FUNC_CLK_SKEW_UL,        */
    {clk_skew_library_load,       clk_skew_library_unload},              /*0x28 FUNC_CLK_SKEW_DL,        */
#else
    {NULL,              NULL},              /*0x27 FUNC_CLK_SKEW_UL,        */
    {NULL,              NULL},              /*0x28 FUNC_CLK_SKEW_DL,        */
#endif
    {NULL,              NULL},              /*0x29 FUNC_MIC_SW_GAIN,        */
#ifdef MTK_PLC_USE_PIC
    {smart_pitch_plc_library_load,              smart_pitch_plc_library_unload},              /*0x2A FUNC_PLC,                */
#else
    {NULL,              NULL},              /*0x2A FUNC_PLC,                */
#endif
#ifdef MTK_BT_CLK_SKEW_USE_PIC
    {clk_skew_library_load,       clk_skew_library_unload},              /*0x2B FUNC_CLK_SKEW_HFP_DL,   */
#else
    {NULL,              NULL},              /*0x2B FUNC_CLK_SKEW_HFP_DL,        */
#endif
    {NULL,              NULL},              /*0x2C FUNC_PROC_SIZE_CONV,*/
    {NULL,              NULL},              /*0x2D FUNC_JOINT,          */
    {NULL,              NULL},              /*0x2E FUNC_BRANCH,       */
    {NULL,              NULL},              /*0x2F FUNC_MUTE,           */
#ifdef MTK_BT_A2DP_CPD_USE_PIC
    {cpd_library_load,    cpd_library_unload},  /*0x30 FUNC_DRC,             */
#else
    {NULL,              NULL},                  /*0x30 FUNC_DRC,             */
#endif
#ifdef MTK_BT_PEQ_USE_PIC
    {peq_library_load,  peq_library_unload},/*0x31 FUNC_PEQ,             */
#else
    {NULL,              NULL},              /*0x31 FUNC_PEQ,             */
#endif
    {NULL,              NULL},              /*0x32 FUNC_LPF,*/
    {NULL,              NULL},              /*0x33 FUNC_CH_SEL,          */
    {NULL,              NULL},              /*0x34 FUNC_MUTE_SMOOTHER,       */
#ifdef MTK_BT_PEQ_USE_PIC
    {peq_library_load,  peq_library_unload},/*0x35 FUNC_PEQ2,             */
#else
    {NULL,              NULL},              /*0x35 FUNC_PEQ2,           */
#endif
#ifdef MTK_BT_A2DP_CPD_USE_PIC
    {cpd_library_load,    cpd_library_unload},              /*0x36 FUNC_DRC2,       */
#else
    {NULL,              NULL},                  /*0x36 FUNC_DRC2,             */
#endif
    {NULL,              NULL},              /*0x37 FUNC_CH_SEL,               */
    {NULL,              NULL},              /*0x38 FUNC_LEAKAGE_COMPENSATION,               */
    {NULL,              NULL},              /*0x39 _Reserved,       */
    {NULL,              NULL},              /*0x3A FUNC_WWE_PREPROC,               */
#ifdef MTK_WWE_USE_PIC
    {wwe_library_load,  wwe_library_unload},              /*0x3B FUNC_WWE_PROC,     ,               */
#else
    {NULL,              NULL},              /*0x3B FUNC_WWE_PROC,               */
#endif
#ifdef MTK_BT_AGC_USE_PIC
    {agc_library_load,  agc_library_unload},/*0x3C FUNC_RX_WB_AGC,       */
    {agc_library_load,  agc_library_unload},/*0x3D FUNC_RX_WB_AGC,       */
    {agc_library_load,  agc_library_unload},/*0x3E FUNC_RX_WB_AGC,       */
#else
    {NULL,              NULL},              /*0x3C FUNC_RX_WB_AGC,       */
    {NULL,              NULL},              /*0x3D FUNC_RX_WB_AGC,       */
    {NULL,              NULL},              /*0x3E FUNC_RX_WB_AGC,       */
#endif
    {NULL,              NULL},              /*0x3F FUNC_GSENSOR_MOTION_DETECT,       */
    {NULL,              NULL},              /*0x40 FUNC_AUDIO_PLC,       */
#ifdef MTK_BT_A2DP_CPD_USE_PIC
    {cpd_library_load,    cpd_library_unload},  /*0x41 FUNC_INS,             */
#else
    {NULL,              NULL},                  /*0x41 FUNC_INS,             */
#endif
#ifdef MTK_BT_PEQ_USE_PIC
    {peq_library_load,  peq_library_unload},/*0x42 FUNC_PEQ,             */
#else
    {NULL,              NULL},              /*0x42 FUNC_PEQ,             */
#endif
    {NULL,              NULL},              /*0x43 FUNC_FIXED_SW_GAIN,    */
    {NULL,              NULL},              /*0x44 FUNC_FUNCTION_A,       */
    {NULL,              NULL},              /*0x45 FUNC_FUNCTION_B,       */
    {NULL,              NULL},              /*0x46 FUNC_FUNCTION_C,       */
    {NULL,              NULL},              /*0x47 FUNC_FUNCTION_D,       */
    {NULL,              NULL},              /*0x48 FUNC_FUNCTION_E,       */
    {NULL,              NULL},              /*0x49 FUNC_FUNCTION_F,       */
    {NULL,              NULL},              /*0x4A FUNC_FUNCTION_G,       */
    {NULL,              NULL},              /*0x4B FUNC_FUNCTION_H,       */
#ifdef MTK_3RD_PARTY_NR
    {tx_eq_library_load,  tx_eq_library_unload},/*0x4C FUNC_TX_EQ,             */
#else
    {NULL,              NULL},                  /*0x4C FUNC_TX_EQ,             */
#endif
    {NULL,              NULL},              /*0x4D  FUNC_USER_TRIGGER_FF_SZ,       */
    {NULL,              NULL},              /*0x4E  FUNC_USER_TRIGGER_FF_PZ,       */
    {NULL,              NULL},              /*0x4F  FUNC_USER_TRIGGER_FF_PZ,       */
    {NULL,              NULL},              /*0x50 FUNC_AUDIO_LOOPBACK_TEST,       */

#ifdef MTK_BT_A2DP_CPD_USE_PIC
    {cpd_library_load,    cpd_library_unload},  /*0x51 FUNC_DRC3,             */
#else
    {NULL,              NULL},                  /*0x51 FUNC_DRC3,             */
#endif
    {NULL,              NULL},              /*0x52 FUNCTION type: Software Sample Rate Converters,       */
#ifdef MTK_BT_CLK_SKEW_USE_PIC
    {clk_skew_library_load,       clk_skew_library_unload},              /*0x53 FUNCTION type: Software clock skew,       */
#else
    {NULL,              NULL},              /*0x53 FUNCTION type: Software clock skew,        */
#endif
    {NULL,              NULL},              /*0x54 FUNCTION type: Software GAIN,       */
    {NULL,              NULL},              /*0x55 FUNCTION type: Software MIXER,       */
    {NULL,              NULL},              /*0x56 FUNCTION type: Software BUFFER,       */
#ifdef MTK_BT_CLK_SKEW_USE_PIC
    {clk_skew_library_load,       clk_skew_library_unload},              /*0x57 FUNC_CLK_SKEW_BLE_MUSIC_DL,   */
#else
    {NULL,              NULL},              /*0x57 FUNC_CLK_SKEW_BLE_MUSIC_DL,        */
#endif
#if defined(AIR_ANC_WIND_DETECTION_ENABLE) && defined(AIR_WIND_DETECTION_USE_PIC)
    {wind_library_load,  wind_library_unload},/*0x58 FUNC_WIND_DETECT,    */
#else
    {NULL,              NULL},              /*0x58 FUNC_WIND_DETECT,    */
#endif
	{NULL,              NULL},              /*0x59 FUNC_CH_SEL_VP,    */
#ifdef MTK_BT_PEQ_USE_PIC
    {peq_library_load,  peq_library_unload},/*0x5A FUNC_PEQ3,             */
#else
    {NULL,              NULL},              /*0x5A FUNC_PEQ3,             */
#endif
#ifdef AIR_AFC_USE_PIC
    {afc_library_load,  afc_library_unload},/*0x5B FUNC_AFC,             */
#else
    {NULL,              NULL},              /*0x5B FUNC_AFC,             */
#endif
#ifdef AIR_LD_NR_USE_PIC
    {ld_nr_library_load,  ld_nr_library_unload},/*0x5C FUNC_LD_NR,           */
#else
    {NULL,              NULL},              /*0x5C FUNC_LD_NR,           */
#endif
#ifdef AIR_AT_AGC_USE_PIC
    {at_agc_library_load,  at_agc_library_unload},/*0x5D FUNC_AT_AGC,          */
#else
    {NULL,              NULL},              /*0x5D FUNC_AT_AGC,          */
#endif
#ifdef MTK_BT_PEQ_USE_PIC
    {peq_library_load,  peq_library_unload},/*0x5E FUNC_PEQ4,            */
#else
    {NULL,              NULL},              /*0x5E FUNC_PEQ4,            */
#endif
#if defined(AIR_HFP_DNN_PATH_ENABLE) && defined(AIR_DNN_LIB_ENABLE)
    {dnn_nr_library_load,  dnn_nr_library_unload},/*0x5F FUNC_DNN_NR,             */
#else
    {NULL,              NULL},                  /*0x5F FUNC_DNN_NR,             */
#endif
#if defined(AIR_EC120_ENABLE)
    {ec120_library_load,  ec120_library_unload},/*0x60 FUNC_EC120,             */
#else
    {NULL,              NULL},                  /*0x60 FUNC_EC120,             */
#endif
    {NULL,              NULL},              /*0x61 FUNC_SRC_FIXED_RATIO,    */
#if defined(AIR_ANC_USER_UNAWARE_ENABLE) && defined(AIR_USER_UNAWARE_USE_PIC)
    {unaware_library_load,  unaware_library_unload},/*0x62 FUNC_USER_UNAWARE,    */
#else
    {NULL,              NULL},              /*0x62 FUNC_USER_UNAWARE,    */
#endif
    {NULL,              NULL},              /*0x63 FUNC_ENVIRONMENT_DETECTION,    */
#if defined(AIR_ECNR_PREV_PART_USE_PIC_ENABLE)
    {ecnr_prev_part_library_load, ecnr_prev_part_library_unload},/*0x64 FUNC_ECNR_PREV_PROCESS,             */
#else
    {NULL,              NULL},                                   /*0x64 FUNC_ECNR_PREV_PROCESS,             */
#endif
#if defined(AIR_ECNR_POST_PART_USE_PIC_ENABLE)
    {ecnr_post_ec_library_load, ecnr_post_ec_library_unload},/*0x65 FUNC_ECNR_POST_PROCESS,             */
#else
    {NULL,                                             NULL},/*0x65 FUNC_ECNR_POST_PROCESS,             */
#endif
};
#endif

/*===========================================================================================*/
#if 0

stream_feature_list_t AudioFeatureList_BranchJoint[10] =
{
    CODEC_PCM_COPY,
    //DSP_SRC,
    FUNC_BRANCH,
   // FUNC_MUTE,
    FUNC_JOINT,
    FUNC_END,
};

stream_feature_list_t AudioFeatureList_Source2Sink[10] =
{
    CODEC_PCM_COPY,
    FUNC_END,
};

// stream_feature_list_t AudioFeatureList_RT[10] =
// {
    // CODEC_DECODER_RT,
    // FUNC_END,
// };

stream_feature_list_t AudioFeatureList_CVSD_Source2Sink[10] =
{
    CODEC_ENCODER_CVSD,
    CODEC_DECODER_CVSD,
    FUNC_END,
};

stream_feature_list_t gA2dpAudioRxFeatureList[4] =
{
    CODEC_PCM_COPY,
    // FUNC_PEQ,
    // FUNC_DRC,
    FUNC_END,
};
#endif

////////////////////////////////////////////////////////////////////////////////
// DSP FUNCTION DECLARATIONS ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#if 0
/**
 * @brief  Get DSP_CODEC
 *
 * @param codec_sel  codec selected.
 *
 * @return DSP_CODEC structure pointer .
 */
DSP_CODEC_PTR DSP_CallbackCodec_Get(DSP_CALLBACK_CODEC codec_sel)
{
    DSP_CODEC_PTR codec_ptr;
    switch(codec_sel)
    {
        case DSP_RT_DECODER:
            codec_ptr = (DSP_CODEC_PTR)&RT_CODEC;
            break;
        case DSP_SBC_DECODER:
            codec_ptr = (DSP_CODEC_PTR)&SBCDEC_CODEC;
            break;
        case DSP_MSBC_DECODER:
            codec_ptr = (DSP_CODEC_PTR)&MSBCDEC_CODEC;
            break;
        case DSP_MSBC_ENCODER:
            codec_ptr = (DSP_CODEC_PTR)&MSBCENC_CODEC;
            break;
        case DSP_VP_DECODER:
            codec_ptr = (DSP_CODEC_PTR)&VP_CODEC;
            break;
        case DSP_NULL_CODEC:
        default:
            codec_ptr = (DSP_CODEC_PTR)&NULL_CODEC;
            break;
    }
    return codec_ptr;
}
#endif

/**
 * stream_pcm_copy_initialize
 *
 * Null init entry for feature
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
bool stream_pcm_copy_initialize(void *para)
{
    UNUSED(para);
    stream_codec_modify_output_size(para, stream_codec_get_input_size(para));
    stream_codec_modify_output_samplingrate(para, stream_codec_get_input_samplingrate(para));
    return 0;
}

bool stream_function_end_initialize(void *para)
{
    UNUSED(para);
    return 0;
}

/**
 * stream_function_end_process
 *
 * Null function entry for entry table end
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 bool stream_function_end_process(void *para)
{
    UNUSED(para);
    U32 i;
    if(stream_function_get_sink_output_resolution(para) != stream_function_get_output_resolution(para))
    {
        if(stream_function_get_output_resolution(para)==RESOLUTION_16BIT)
        {
            for (i=0 ; i<stream_function_get_channel_number(para) ; i++)
            {
                dsp_converter_16bit_to_32bit((S32*)stream_function_get_inout_buffer(para, i+1),
                                             (S16*)stream_function_get_inout_buffer(para, i+1),
                                             stream_function_get_output_size(para)/sizeof(S16));
            }
            stream_function_modify_output_size(para, stream_function_get_output_size(para)*2);
            stream_function_modify_output_resolution(para, RESOLUTION_32BIT);
        }
        else
        {
            for (i=0 ; i<stream_function_get_channel_number(para) ; i++)
            {
                dsp_converter_32bit_to_16bit((S16*)stream_function_get_inout_buffer(para, i+1),
                                             (S32*)stream_function_get_inout_buffer(para, i+1),
                                             stream_function_get_output_size(para)/sizeof(S32));
            }
            stream_function_modify_output_size(para, stream_function_get_output_size(para)/2);
            stream_function_modify_output_resolution(para, RESOLUTION_16BIT);
        }
    }
    stream_function_modify_output_resolution(para, stream_function_get_sink_output_resolution(para));
    return 0;
}


bool MuteFunc(void *para)
{
    U32 i, in_ChNum;
    in_ChNum = 0;
    for (i=0 ; i<stream_function_get_channel_number(para) ; i++)
    {
        memset(stream_function_get_inout_buffer(para, i+1),
               0,
               stream_function_get_output_size(para));
    }

    return 0;
}

bool UpBy2Func(void *para)
{
    U32 i, in_ChNum;
    S16 *bufPtr;

    //DRV_USB_CDC_ACM1_Send(stream_function_get_1st_inout_buffer(para), stream_function_get_output_size(para));

    #if 1
    for (in_ChNum=0 ; in_ChNum<stream_function_get_channel_number(para) ; in_ChNum++)
    {
        bufPtr = stream_function_get_inout_buffer(para, in_ChNum+1);

        for(i=(stream_function_get_output_size(para)/2) ; i!=0 ; i--)
        {
            *(bufPtr+2*i-2) = *(bufPtr+i-1);
            *(bufPtr+2*i-1) = *(bufPtr+i-1);
        }

    }
    stream_function_modify_samplingrateconversion_ratio(para, UPSAMPLE_BY2);
    #else
    U16 times = 4;

    for (in_ChNum=0 ; in_ChNum<stream_function_get_channel_number(para) ; in_ChNum++)
    {
        bufPtr = stream_function_get_inout_buffer(para, in_ChNum+1);

        for(i=0 ; i<((stream_function_get_output_size(para)/2)/times) ; i++)
        {
            *(bufPtr+i) = *(bufPtr+times*i);
        }

    }
    stream_function_modify_samplingrateconversion_ratio(para, DOWNSAMPLE_BY4);


    #endif



    return 0;
}

#ifdef PRELOADER_ENABLE
VOID CodecOpen  (VOID)
{
    /* No need PIC, just an example of entry */
    DSP_MW_LOG_D("[PIC] Codec Bypass Open", 0);
    //UNUSED(para);
}

VOID CodecClose (VOID)
{
    DSP_MW_LOG_I("[PIC] Codec Bypass Close", 0);
    //UNUSED(para);
}
#endif

bool dsp_sdk_add_feature_ctrl(stream_feature_ptr_t feature_ptr, stream_feature_ctrl_entry_ptr_t feature_entry_ptr)
{
    stream_feature_type_t feature_type = feature_ptr->feature_type;
    if (((feature_type>FEATURETABLE_DECODER_RESERVED_END_NUM) && (feature_type<FEATURETABLE_ENCODER_RESERVED_START_NUM)) ||
        ((feature_type>FEATURETABLE_ENCODER_RESERVED_END_NUM) && (feature_type<FEATURETABLE_FUNC_RESERVED_START_NUM-1)) ||
        ((feature_type>FEATURETABLE_FUNC_RESERVED_END_NUM) && (feature_type<DSP_FEATURE_MAX_NUM)))
    {
        #ifdef PRELOADER_ENABLE
        DSP_FeatureControl[feature_type].open_entry = feature_entry_ptr->open_entry;
        DSP_FeatureControl[feature_type].close_entry = feature_entry_ptr->close_entry;
        #else
        UNUSED(feature_entry_ptr);
        #endif
        return TRUE;
    }

    return FALSE;
}


bool dsp_sdk_add_feature_table(stream_feature_ptr_t feature_ptr)
{
    stream_feature_type_t feature_type = feature_ptr->feature_type;
    if (((feature_type>FEATURETABLE_DECODER_RESERVED_END_NUM) && (feature_type<FEATURETABLE_ENCODER_RESERVED_START_NUM)) ||
        ((feature_type>FEATURETABLE_ENCODER_RESERVED_END_NUM) && (feature_type<FEATURETABLE_FUNC_RESERVED_START_NUM-1)) ||
        ((feature_type>FEATURETABLE_FUNC_RESERVED_END_NUM) && (feature_type<DSP_FEATURE_MAX_NUM)))
    {
        stream_feature_table[feature_type] = *feature_ptr;
        return TRUE;
    }
    return FALSE;
}

/**
 * stream_feature_configure_type
 *
 * Configure Feature Table Codec
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
void stream_feature_configure_type(stream_feature_list_ptr_t list, stream_feature_type_t codec_type, stream_feature_configure_codec_t configure_codec)
{
    U32 i;
    if ((list==NULL) || !(codec_type<DSP_FUNC_TYPE))
        return;
    if (configure_codec == CONFIG_DECODER)
    {
        *list = codec_type;
    }
    else if (configure_codec == CONFIG_ENCODER)
    {
        for(i=0 ; *(list+i+1)!=FUNC_END ; i++);

        if ((*(list+i)&0xFF)>=DSP_FUNC_TYPE)
        {
            i++;
            *(list+i+1) = *(list+i);
        }
        *(list+i) = codec_type;
    }
}

/**
 * stream_feature_configure_codec_output_size
 *
 * Configure Feature Table Codec out size
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
void stream_feature_configure_codec_output_size(stream_feature_list_ptr_t list, uint32_t size, stream_feature_configure_codec_t configure_codec)
{
    U32 i;
    stream_feature_type_ptr_t codecPtr;

    if (list==NULL || size==0)
        return;
    if (configure_codec == CONFIG_DECODER)
    {
        codecPtr = list;
    }
    else if (configure_codec == CONFIG_ENCODER)
    {
        for(i=0 ; *(list+i+1)!=FUNC_END; i++);
        codecPtr = list+i;
    }
    *codecPtr = ((*codecPtr)&0xFFFF) | (size<<16);
}

/**
 * stream_feature_configure_resolution
 *
 * Configure Feature Table Codec out Resolution
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
void stream_feature_configure_resolution(stream_feature_list_ptr_t list, uint8_t resolution, stream_feature_configure_codec_t configure_codec)
{
    U32 i;
    stream_feature_type_ptr_t codecPtr;

    if (list==NULL || (resolution!=RESOLUTION_16BIT && resolution!=RESOLUTION_32BIT))
        return;
    if (configure_codec == CONFIG_DECODER)
    {
        codecPtr = list;
    }
    else if (configure_codec == CONFIG_ENCODER)
    {
        for(i=0 ; *(list+i+1)!=FUNC_END; i++);
        codecPtr = list+i;
    }
    *codecPtr = ((*codecPtr)&0xFFFF00FF) | (resolution<<8);
}

/**
 * stream_feature_configure_src
 *
 * Configure Feature List SRC
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
void stream_feature_configure_src(stream_feature_list_ptr_t list, stream_resolution_t in_res, stream_resolution_t out_res, uint32_t src_out_sampling, uint32_t src_out_length)
{
    U32 i;
    U16 listNum = 0, srcNum = 0;
    if (list==NULL)
        return;
    for(i=0 ; *(list+i)!=FUNC_END ; i++)
    {
        if ((*(list+i)&0xFF) == DSP_SRC)
        {
            configASSERT(srcNum == 0);
            srcNum = i;
        }
    }
    listNum = i;

    /* Insert DSP_SRC */
    if (srcNum==0)
    {
        for(i=listNum ; i>0 ; i--)
        {
            *(list+i+1) = *(list+i);
        }
        srcNum = 1;
        *(list+srcNum) = DSP_SRC;
    }

    *(list+srcNum) |= ((src_out_length&0x3FFF)<<18) | ((in_res&0x01)<<17) | ((out_res&0x01)<<16) | ((src_out_sampling&0xFF)<<8);
}



/**
 * stream_feature_reinitialize
 *
 * Deinit Feature
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
void stream_feature_reinitialize(void *para)
{
    PL_CRITICAL(DSP_Callback_ChangeStreaming2Deinit, para);
}

ATTR_TEXT_IN_IRAM_LEVEL_2 uint8_t stream_function_get_device_channel_number(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->device_out_channel_num;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 uint8_t stream_function_get_software_channel_number(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->software_handled_channel_num;
}


/**
 * DSP_CodecSeqChk
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL DSP_CodecSeqChk(VOID* para)
{
    #if 0
    configASSERT(((DSP_ENTRY_PARA_PTR)para)->number.field.process_sequence == CODEC_ALLOW_SEQUENCE);
    #else
    if (((DSP_ENTRY_PARA_PTR)para)->number.field.process_sequence == CODEC_ALLOW_SEQUENCE)
    {
        return TRUE;
    }
    else if (((DSP_ENTRY_PARA_PTR)para)->number.field.process_sequence == ((DSP_ENTRY_PARA_PTR)para)->number.field.feature_number-1)
    {
        ((DSP_ENTRY_PARA_PTR)para)->with_encoder = TRUE;
        return FALSE;
    }
    configASSERT(FALSE);
    return FALSE;
    #endif
}

/* Codec APIs */
uint8_t stream_codec_get_type(void *para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    DSP_FEATURE_TABLE_PTR  FeatureTablePtr;

    DSP_CodecSeqChk(para);
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    FeatureTablePtr = (DSP_FEATURE_TABLE_PTR)(stream_ptr->callback.FeatureTablePtr + stream_ptr->callback.EntryPara.number.field.process_sequence-1);
    return FeatureTablePtr->FeatureType;
}


void* stream_codec_get_task(void *para)
{
    DSP_CodecSeqChk(para);
    return ((DSP_ENTRY_PARA_PTR)para)->DSPTask;
}

uint8_t stream_codec_get_input_channel_number(void* para)
{
    return (DSP_CodecSeqChk(para))
               ?((DSP_ENTRY_PARA_PTR)para)->in_channel_num
               :((DSP_ENTRY_PARA_PTR)para)->out_channel_num;
}

uint8_t stream_codec_get_input_samplingrate(void* para)
{
    return (DSP_CodecSeqChk(para))
               ?((DSP_ENTRY_PARA_PTR)para)->in_sampling_rate
               :(((DSP_ENTRY_PARA_PTR) para)->with_src)
                   ?((DSP_ENTRY_PARA_PTR)para)->src_out_sampling_rate
                   :((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate;
}

bool stream_codec_modify_input_size(void* para, uint16_t in_size)
{
    if (DSP_CodecSeqChk(para))
    {
        if (((DSP_ENTRY_PARA_PTR)para)->in_malloc_size >= in_size)
        {
            ((DSP_ENTRY_PARA_PTR)para)->in_size = in_size;
            return TRUE;
        }
    }
    else
    {
        if (((DSP_ENTRY_PARA_PTR)para)->out_malloc_size >= in_size)
        {
            if (((DSP_ENTRY_PARA_PTR) para)->with_src)
                ((DSP_ENTRY_PARA_PTR) para)->src_out_size = in_size;
            else
                ((DSP_ENTRY_PARA_PTR) para)->codec_out_size = in_size;
            return TRUE;

        }
    }
    return FALSE;
}

uint16_t* stream_codec_get_input_size_pointer(void *para)
{
    return (DSP_CodecSeqChk(para))
               ?&(((DSP_ENTRY_PARA_PTR)para)->in_size)
               :(((DSP_ENTRY_PARA_PTR) para)->with_src)
                   ?&(((DSP_ENTRY_PARA_PTR) para)->src_out_size)
                   :&(((DSP_ENTRY_PARA_PTR) para)->codec_out_size);

}

ATTR_TEXT_IN_IRAM_LEVEL_2 uint16_t stream_codec_get_input_size(void* para)
{
    return (DSP_CodecSeqChk(para))
               ?((DSP_ENTRY_PARA_PTR)para)->in_size
               :(((DSP_ENTRY_PARA_PTR) para)->with_src)
                   ?((DSP_ENTRY_PARA_PTR) para)->src_out_size
                   :((DSP_ENTRY_PARA_PTR) para)->codec_out_size;

}

ATTR_TEXT_IN_IRAM_LEVEL_2 void* stream_codec_get_1st_input_buffer(void* para)
{
    return (DSP_CodecSeqChk(para))
               ? (1<=CALLBACK_INPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->in_ptr[0]
                   : NULL
               : (1<=CALLBACK_OUTPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[0]
                   : NULL;
}

void* stream_codec_get_2nd_input_buffer(void* para)
{
    return (DSP_CodecSeqChk(para))
               ? (2<=CALLBACK_INPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->in_ptr[1]
                   : NULL
               : (2<=CALLBACK_OUTPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[1]
                   : NULL;
}

void* stream_codec_get_3rd_input_buffer(void* para)
{
    return (DSP_CodecSeqChk(para))
               ? (3<=CALLBACK_INPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->in_ptr[2]
                   : NULL
               : (3<=CALLBACK_OUTPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[2]
                   : NULL;
}

void* stream_codec_get_4th_input_buffer(void* para)
{
    return (DSP_CodecSeqChk(para))
               ? (4<=CALLBACK_INPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->in_ptr[3]
                   : NULL
               : (4<=CALLBACK_OUTPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[3]
                   : NULL;
}

void* stream_codec_get_input_buffer(void* para, uint32_t channel)
{
    return (DSP_CodecSeqChk(para))
               ? (channel<=CALLBACK_INPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->in_ptr[channel-1]
                   : NULL
               : (channel<=CALLBACK_OUTPUT_PORT_MAX_NUM)
                   ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[channel-1]
                   : NULL;
}

uint8_t stream_codec_get_output_channel_number(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->out_channel_num;
}

uint8_t stream_codec_get_output_samplerate(void* para)
{
    DSP_CodecSeqChk(para);
    return ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate;
}

uint8_t* stream_codec_get_output_samplerate_pointer(void *para)
{
    DSP_CodecSeqChk(para);
    return &(((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate);
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_codec_modify_output_samplingrate(void* para, uint8_t sampling_rate)
{
    DSP_CodecSeqChk(para);
    ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate = sampling_rate;
    return TRUE;
}

uint16_t stream_codec_get_output_size(void* para)
{
    return (DSP_CodecSeqChk(para))
               ?((DSP_ENTRY_PARA_PTR)para)->codec_out_size
               :((DSP_ENTRY_PARA_PTR)para)->encoder_out_size;
}

uint16_t* stream_codec_get_output_size_pointer(void *para)
{
    return (DSP_CodecSeqChk(para))
               ? &(((DSP_ENTRY_PARA_PTR)para)->codec_out_size)
               : &(((DSP_ENTRY_PARA_PTR)para)->encoder_out_size);
}

bool stream_codec_modify_output_size(void* para, uint16_t out_size)
{
    if (((DSP_ENTRY_PARA_PTR)para)->out_malloc_size >= out_size)
    {
        if (DSP_CodecSeqChk(para))
            ((DSP_ENTRY_PARA_PTR)para)->codec_out_size = out_size;
        else
            ((DSP_ENTRY_PARA_PTR)para)->encoder_out_size = out_size;
        return TRUE;
    }
    else
        configASSERT(FALSE);
    return FALSE;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void* stream_codec_get_1st_output_buffer(void* para)
{
    DSP_CodecSeqChk(para);
    return (1<=CALLBACK_OUTPUT_PORT_MAX_NUM)
               ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[0]
               : NULL;
    }

ATTR_TEXT_IN_IRAM_LEVEL_2 void* stream_codec_get_2nd_output_buffer(void* para)
{
    DSP_CodecSeqChk(para);
    return (2<=CALLBACK_OUTPUT_PORT_MAX_NUM)
               ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[1]
               : NULL;
}

void* stream_codec_get_3rd_output_buffer(void* para)
{
    DSP_CodecSeqChk(para);
    return (3<=CALLBACK_OUTPUT_PORT_MAX_NUM)
               ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[2]
               : NULL;
}

void* stream_codec_get_4th_output_buffer(void* para)
{
    DSP_CodecSeqChk(para);
    return (4<=CALLBACK_OUTPUT_PORT_MAX_NUM)
               ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[3]
               : NULL;
}

void* stream_codec_get_output_buffer(void* para, uint32_t channel)
{
    DSP_CodecSeqChk(para);
    return (channel<=CALLBACK_OUTPUT_PORT_MAX_NUM)
               ? ((DSP_ENTRY_PARA_PTR)para)->out_ptr[channel-1]
               : NULL;
}

void* stream_codec_get_workingbuffer(void* para)
{
    DSP_CodecSeqChk(para);
    return ((DSP_ENTRY_PARA_PTR)para)->mem_ptr;
}

uint8_t stream_codec_get_input_resolution(void* para)
{
    return (DSP_CodecSeqChk(para))
               ?((DSP_ENTRY_PARA_PTR)para)->resolution.source_in_res
               :((DSP_ENTRY_PARA_PTR)para)->resolution.process_res;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_codec_modify_resolution(void* para, uint8_t resolution)
{
    DSP_CodecSeqChk(para);
    if ((resolution==RESOLUTION_16BIT) || (resolution==RESOLUTION_32BIT))
    {
        ((DSP_ENTRY_PARA_PTR)para)->resolution.process_res = resolution;
    }
    return (((DSP_ENTRY_PARA_PTR)para)->resolution.process_res == resolution)
               ? TRUE
               : FALSE;
}

uint8_t stream_codec_get_resolution(void* para)
{
    DSP_CodecSeqChk(para);
    return ((DSP_ENTRY_PARA_PTR)para)->resolution.process_res;
}


ATTR_TEXT_IN_IRAM_LEVEL_2 uint8_t stream_codec_get_output_resolution(void* para)
{
    return (DSP_CodecSeqChk(para))
               ?((DSP_ENTRY_PARA_PTR)para)->resolution.feature_res
               :((DSP_ENTRY_PARA_PTR)para)->resolution.sink_out_res;
}

uint32_t stream_codec_get_numbering(void *para)
{
    DSP_CodecSeqChk(para);
    return ((DSP_ENTRY_PARA_PTR)para)->number.reg;
}

uint32_t stream_codec_get_mute_flag(void *para)
{
    return (((DSP_ENTRY_PARA_PTR)para)->bypass_mode == BYPASS_CODEC_MODE);
}



/* Function APIs */
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL DSP_FuncSeqChk(VOID* para)
{
    return ((((DSP_ENTRY_PARA_PTR)para)->with_src != 0) &&
            (((DSP_ENTRY_PARA_PTR)para)->number.field.process_sequence > ((DSP_ENTRY_PARA_PTR)para)->with_src));
}

uint8_t stream_function_get_type(void *para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    DSP_FEATURE_TABLE_PTR  FeatureTablePtr;

    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    FeatureTablePtr = (DSP_FEATURE_TABLE_PTR)(stream_ptr->callback.FeatureTablePtr + stream_ptr->callback.EntryPara.number.field.process_sequence-1);
    return FeatureTablePtr->FeatureType;
}


uint8_t stream_function_get_decoder_type(void *para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    DSP_FEATURE_TABLE_PTR  FeatureTablePtr;

    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    FeatureTablePtr = (DSP_FEATURE_TABLE_PTR)(stream_ptr->callback.FeatureTablePtr);
    return FeatureTablePtr->FeatureType;
}


void* stream_function_get_task(void *para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->DSPTask;
}


ATTR_TEXT_IN_IRAM_LEVEL_2 uint8_t stream_function_get_channel_number(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->out_channel_num;
}

uint8_t stream_function_get_samplingrate(void* para)
{
    return (DSP_FuncSeqChk(para))
               ? ((DSP_ENTRY_PARA_PTR)para)->src_out_sampling_rate
               : ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 uint16_t stream_function_get_output_size(void* para)
{
    return (DSP_FuncSeqChk(para))
               ? ((DSP_ENTRY_PARA_PTR)para)->src_out_size
               : ((DSP_ENTRY_PARA_PTR)para)->codec_out_size;
}

bool stream_function_modify_samplingrateconversion_ratio(void* para, stream_feature_convert_samplerate_t updown_rate)
{
    U16 size, samplingRate;

    size = stream_function_get_output_size(para);
    samplingRate = stream_function_get_samplingrate(para);
    switch(updown_rate)
    {
        case UPSAMPLE_BY2:
            size *= 2;
            samplingRate *= 2;
            break;
        case UPSAMPLE_BY3:
            size *= 3;
            samplingRate *= 3;
            break;
        case UPSAMPLE_BY4:
            size *= 4;
            samplingRate *= 4;
            break;
        case UPSAMPLE_BY6:
            size *= 6;
            samplingRate *= 6;
            break;
        case UPSAMPLE_BY12:
            size *= 12;
            samplingRate *= 12;
            break;

        case DOWNSAMPLE_BY2:
            size /= 2;
            samplingRate /= 2;
            break;
        case DOWNSAMPLE_BY3:
            size /= 3;
            samplingRate /= 3;
            break;
        case DOWNSAMPLE_BY4:
            size /= 4;
            samplingRate /= 4;
            break;
        case DOWNSAMPLE_BY6:
            size /= 6;
            samplingRate /= 6;
            break;
        case DOWNSAMPLE_BY12:
            size /= 12;
            samplingRate /= 12;
            break;
        default:
            break;
    }


    if (size>0 && size<=((DSP_ENTRY_PARA_PTR)para)->out_malloc_size)
    {
        if(DSP_FuncSeqChk(para))
        {
            ((DSP_ENTRY_PARA_PTR)para)->src_out_size            = size;
            ((DSP_ENTRY_PARA_PTR)para)->src_out_sampling_rate   = samplingRate;
        }
        else
        {
            ((DSP_ENTRY_PARA_PTR)para)->codec_out_size          = size;
            ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate = samplingRate;
        }
        return TRUE;
    }
    return FALSE;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_modify_output_size(void* para, uint16_t size)
{
    if (size<=((DSP_ENTRY_PARA_PTR)para)->out_malloc_size)
    {
        if(DSP_FuncSeqChk(para))
        {
            ((DSP_ENTRY_PARA_PTR)para)->src_out_size            = size;
        }
        else
        {
            ((DSP_ENTRY_PARA_PTR)para)->codec_out_size          = size;
        }
        return TRUE;
    }
    return FALSE;
}


ATTR_TEXT_IN_IRAM_LEVEL_2 void* stream_function_get_1st_inout_buffer(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->out_ptr[0];
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void* stream_function_get_2nd_inout_buffer(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->out_ptr[1];
}

void* stream_function_get_3rd_inout_buffer(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->out_ptr[2%CALLBACK_OUTPUT_PORT_MAX_NUM];
}

void* stream_function_get_4th_inout_buffer(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->out_ptr[3%CALLBACK_OUTPUT_PORT_MAX_NUM];
}

void* stream_function_get_5th_inout_buffer(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->out_ptr[4%CALLBACK_OUTPUT_PORT_MAX_NUM];
}

void* stream_function_get_6th_inout_buffer(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->out_ptr[5%CALLBACK_OUTPUT_PORT_MAX_NUM];
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void* stream_function_get_inout_buffer(void* para, uint32_t channel)
{
    if (channel<=CALLBACK_OUTPUT_PORT_MAX_NUM)
        return ((DSP_ENTRY_PARA_PTR)para)->out_ptr[channel-1];
    else
        return NULL;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_reenter_stream(void* para)
{
    ((DSP_ENTRY_PARA_PTR)para)->force_resume = TRUE;
    return TRUE;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 void* stream_function_get_working_buffer(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->mem_ptr;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_modify_output_resolution(void* para, uint8_t resolution)
{
    if ((resolution==RESOLUTION_16BIT) || (resolution==RESOLUTION_32BIT))
    {
        ((DSP_ENTRY_PARA_PTR)para)->resolution.process_res = resolution;
    }
    return (((DSP_ENTRY_PARA_PTR)para)->resolution.process_res == resolution)
               ? TRUE
               : FALSE;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 uint8_t stream_function_get_output_resolution(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->resolution.process_res;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 uint8_t stream_function_get_sink_output_resolution(void* para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->resolution.sink_out_res;
}

uint32_t stream_function_get_numbering(void *para)
{
    return ((DSP_ENTRY_PARA_PTR)para)->number.reg;
}


