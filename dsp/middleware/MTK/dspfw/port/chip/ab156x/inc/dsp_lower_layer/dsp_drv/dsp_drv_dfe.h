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

#ifndef _DSP_DRV_DFE_H_
#define _DSP_DRV_DFE_H_

#include "types.h"
#include "sfr_au_src.h"
#include "sfr_au_i2s.h"
#include "dsp_sdk.h"
#include "audio_config.h"


#include "source.h"
#include "sink.h"

/******************************************************************************
 * Constant define
 ******************************************************************************/







/******************************************************************************
 * Enumerations
 ******************************************************************************/
    typedef enum
    {
        ADC_DATAIN          = 0,
        I2S_MS_SDI          = 1,
        I2S_SL_SDI          = 2,
        SPDIF_RX            = 3,
        MINI_DSP_IN         = 4,
    } iDFE_DMA_ID_enum_s;

    typedef enum
    {
        DAC_DATAOUT         = 5,
        I2S_MS_SDO          = 6,
        I2S_SL_SDO          = 7,
        SPDIF_TX            = 8,
        MINI_DSP_OUT        = 9,
    }oDFE_DMA_ID_enum_s;

    typedef enum
    {

        SRC_OUT         = 0,
        ADMA_OUT        = 1,
    }SRC_ADMA_enum_s;



    typedef enum
    {
        WITH_CIC        = 0,
        BYPASS_CIC      = 1,
    }CIC_SEL_enum_s;



    typedef enum
    {
        FROM_AU_DNFILT        = 0,
        FROM_AU_oDFE          = 1,
        AEC_MUTE              = 2,
    }AEC_SOURCE_SEL_enum_s;



    typedef enum
    {
        AEC_IN_RATE_16K             = 0,
        AEC_IN_RATE_32K             = 1,
        AEC_IN_RATE_48K             = 2,
        AEC_IN_RATE_96K             = 3,
        AEC_IN_RATE_192K            = 4,
    }AEC_FS_IN_enum_s;

    typedef enum
    {
        I2S_FS_RATE_48K              = 0,
        I2S_FS_RATE_96K              = 1,
    }I2S_FS_RATE_enum_s;

    typedef enum
    {
        I2S_WORD_LEN_16BIT           = 0,
        I2S_WORD_LEN_24BIT           = 1,
        I2S_WORD_LEN_32BIT           = 2,
    }I2S_WORD_LEN_enum_s;

    typedef enum
    {
        FAST_CLK_FOR_ASIC_144M          = 1,
        FAST_CLK_FOR_FPGA_72M           = 3,

        SPDIF_SAMPLE_RATE_192k          = 0,
        SPDIF_SAMPLE_RATE_96k           = 1,
        SPDIF_SAMPLE_RATE_48k           = 2,
        SPDIF_SAMPLE_RATE_44k           = 3,
        SPDIF_SAMPLE_RATE_32k           = 4,

        PRECISION_OF_DELTA_05           = 0,
        PRECISION_OF_DELTA_025          = 1,
        PRECISION_OF_DELTA_0125         = 2,

        WIDER_TOLERANCE_05UI_to_35UI    = 0,
        WIDER_TOLERANCE_025UI_to_375UI  = 1,

    }SPDIF_RELATED_enum_s;

    typedef enum
    {
        ADMA_CH0                     = 0,
        ADMA_CH1                     = 1,
        ADMA_CH2                     = 2,
        ADMA_CH3                     = 3,
        ADMA_CH4                     = 4,
        SRCA_CH0                     = 5,
        SRCA_CH1                     = 6,
        SRCB_CH0                     = 7,
        SRCB_CH1                     = 8,
        SRCC_CH0                     = 9,
        SRCC_CH1                     = 10,
    }ADMA_CH_NUM_s;

    /*
    typedef struct
    {
    iDFE_DMA_ID_enum_s iDFE_DMA;
    oDFE_DMA_ID_enum_s oDFE_DMA;
    stream_samplerate_t FS_OUT;
    stream_samplerate_t FS_IN;
    SRC_ADMA_enum_s SRC_ADMA;
    CIC_SEL_enum_s CIC_SEL;
    AEC_SOURCE_SEL_enum_s AEC_SOURCE_SEL;
    AEC_FS_IN_enum_s AEC_FS_IN;
    I2S_FS_RATE_enum_s I2S_FS_RATE;
    I2S_WORD_LEN_enum_s I2S_WORD_LEN;
    AU_SRC_FS_OUT SRC_FS_OUT;
    AU_SRC_FS_IN SRC_FS_IN;
    SPDIF_RELATED_enum_s SPDIF_RELATED;
    }DSP_DFE_enum_t;
*/

    typedef struct STREAM_SRC_VDM_s
    {
        SRC_PTR_s VOLATILE  src_ptr;
        VOID*               mem_ptr;
        U32                 enable_flag;
        U16                 original_frame_size;
        U16                 accum_process_size;
    } STREAM_SRC_VDM_t, *STREAM_SRC_VDM_PTR_t;


    typedef struct DSP_DRV_SRC_VDM_INIT_s
    {
        SRC_PTR_s VOLATILE  src_ptr;
        AU_SRC_DRIVING_MODE mode;

        U8*                 radma_buf_addr;
        U32                 radma_buf_size;
        U32                 radma_THD;

        U8*                 wadma_buf_addr;
        U32                 wadma_buf_size;
        U32                 wadma_THD;
        stream_resolution_t    Res_In;
        stream_resolution_t    Res_Out;

        AU_SRC_FS_IN        fs_in;
        AU_SRC_FS_OUT       fs_out;
        void *              task_id;
        U8                  channel_num;

    } DSP_DRV_SRC_VDM_INIT_STRU, *DSP_DRV_SRC_VDM_INIT_STRU_PTR;

/******************************************************************************
 * External Global Variables
 ******************************************************************************/


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/

EXTERN  VOID DSP_DRV_iDFE_DEC3_INIT(stream_feature_convert_samplerate_t DWN_RATIO ,U32 ChannelNum, stream_resolution_t BitRes);
EXTERN  VOID DSP_DRV_iDFE_DEC3_END(VOID);

EXTERN  VOID DSP_DRV_WADMA_INIT(ADMA_CH_NUM_s Channel_sel,U8* Buf_addr, U32 Buf_size, U32 THD_size);
EXTERN  VOID DSP_DRV_WADMA_END(U32 Channel_sel);
EXTERN  VOID DSP_DRV_WADMA_CTRL(ADMA_CH_NUM_s Channel_sel, BOOL IsEnabled);
EXTERN  VOID DSP_DRV_SOURCE_WADMA(SOURCE source, BOOL IsEnabled);

EXTERN  VOID DSP_DRV_oDFE_INT4_INIT(stream_feature_convert_samplerate_t UPS_RATIO, U32 ChannelNum, SRC_ADMA_enum_s SRC_ADMA_SEL, CIC_SEL_enum_s CIC_bypass , stream_resolution_t BitRes);
EXTERN  VOID DSP_DRV_oDFE_INT4_RST(U32 ChannelNum);
EXTERN  VOID DSP_DRV_oDFE_INT4_EN(U32 ChannelNum);
EXTERN  VOID DSP_DRV_oDFE_INT4_END(VOID);

EXTERN  VOID DSP_DRV_RADMA_INIT (ADMA_CH_NUM_s Channel_sel,U8* Buf_addr, U32 Buf_size, U32 THD_size);//Not enable yet
EXTERN  VOID DSP_DRV_RADMA_END(U32 Channel_sel);
EXTERN  VOID DSP_DRV_RADMA_CTRL(ADMA_CH_NUM_s Channel_sel, BOOL IsEnabled);
EXTERN  VOID DSP_DRV_SINK_RADMA(SINK sink, BOOL IsEnabled);


EXTERN  VOID DSP_DRV_iDFE_DEC9_INIT (AEC_SOURCE_SEL_enum_s AEC_Source,AEC_FS_IN_enum_s Fs_in, stream_resolution_t BitRes);
EXTERN  VOID DSP_DRV_iDFE_DEC9_END(VOID);

EXTERN  VOID DSP_DRV_SIDETONE_INIT (U32 Gain_Tune, ADMA_CH_NUM_s TONE_SEL);
EXTERN  VOID DSP_DRV_SIDETONE_END(VOID);
EXTERN  VOID DSP_DRV_LR_Swap(BOOL ctrl);

EXTERN  VOID DSP_DRV_iDFE_DEC9_END(VOID);

EXTERN  VOID DSP_DRV_oDFE_INT6_INIT(stream_feature_convert_samplerate_t UPS_RATIO, CIC_SEL_enum_s CIC_bypass, stream_resolution_t BitRes);
EXTERN  VOID DSP_DRV_oDFE_INT6_END(VOID);

EXTERN  VOID DSP_DRV_SRC_A_INIT (AU_SRC_FS_IN Fs_in, AU_SRC_FS_OUT Fs_out ,stream_resolution_t Res_in, stream_resolution_t Res_out);
EXTERN  VOID DSP_DRV_SRC_INIT (SRC_PTR_s VOLATILE src_ptr, AU_SRC_DRIVING_MODE mode, AU_SRC_CH ch, AU_SRC_FS_IN Fs_in, AU_SRC_FS_OUT Fs_out ,stream_resolution_t Res_in, stream_resolution_t Res_out);

EXTERN  VOID DSP_DRV_SRC_A_END ();
EXTERN  VOID DSP_DRV_SRC_END(SRC_PTR_s src_ptr);

EXTERN  VOID DSP_DRV_SPDIF_TX_INIT(SPDIF_RELATED_enum_s TX_SAMPLE_RATE);
EXTERN  VOID DSP_DRV_SPDIF_TX_END(VOID);
EXTERN  VOID DSP_DRV_SPDIF_RX_INIT(VOID);
EXTERN  VOID DSP_DRV_SPDIF_RX_END(VOID);

EXTERN  VOID DSP_DRV_I2S_MS_INIT(I2S_FS_RATE_enum_s FS_rate,I2S_WORD_LEN_enum_s DATA_WORD_LEN,I2S_WORD_LEN_enum_s TX_WORD_LEN,I2S_WORD_LEN_enum_s RX_WORD_LEN, AU_I2S_MODE INOUT);
EXTERN  VOID DSP_DRV_I2S_SL_INIT(I2S_WORD_LEN_enum_s DATA_WORD_LEN,I2S_WORD_LEN_enum_s TX_WORD_LEN,I2S_WORD_LEN_enum_s RX_WORD_LEN ,AU_I2S_MODE INOUT);
EXTERN  VOID DSP_DRV_I2S_MS_END (VOID);
EXTERN  VOID DSP_DRV_I2S_SL_END (VOID);
EXTERN  VOID DSP_DA_OUT_INIT (U8 Channel);
EXTERN  VOID DSP_AD_IN_INIT(VOID);
EXTERN  VOID DSP_DA_OUT_END (VOID);
EXTERN  VOID DSP_AD_IN_END(VOID);

EXTERN  U8 DSP_UpDownRate2Value(stream_feature_convert_samplerate_t rate);
EXTERN  stream_feature_convert_samplerate_t DSP_UpValue2Rate(U8 value);
EXTERN  stream_feature_convert_samplerate_t DSP_DownValue2Rate(U8 value);

EXTERN  AU_SRC_FS_IN DSP_FsChange2SRCInRate(stream_samplerate_t fs_in);
EXTERN  AU_SRC_FS_OUT DSP_FsChange2SRCOutRate(stream_samplerate_t fs_out);
EXTERN  AU_SRC_SR_OUT DSP_RsChange2SRCOutRs(stream_resolution_t rs_out);

EXTERN  U32 DSP_FsChange2Value(stream_samplerate_t fs_in);
EXTERN  U32 DSP_SRCInRateChange2Value(AU_SRC_FS_IN src_in_rate);
EXTERN  U32 DSP_SRCOutRateChange2Value(AU_SRC_FS_OUT src_out_rate);

EXTERN  SPDIF_RELATED_enum_s DSP_ChangeFs2SpdifRate(stream_samplerate_t fs);
EXTERN  U32 DSP_ChangeSpdifRate2Value(SPDIF_RELATED_enum_s spdif_rate);

EXTERN  AU_SRC_FS_IN DSP_GetSRCInRate(SRC_PTR_s src_ptr);
EXTERN  AU_SRC_FS_OUT DSP_GetSRCOutRate(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCIn1BufPtr(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCIn1NextPtr(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCIn2BufPtr(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCIn2NextPtr(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCInBufPtr(SRC_PTR_s src_ptr, U32 CH);
EXTERN  VOID* DSP_GetSRCInNextPtr(SRC_PTR_s src_ptr, U32 CH);
EXTERN  U32 DSP_GetSRCInReadOffset(SRC_PTR_s src_ptr);
EXTERN  U32 DSP_GetSRCInBufSize(SRC_PTR_s src_ptr);
EXTERN  U32 DSP_GetSRCInFrameSize(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCOut1BufPtr(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCOut1NextPtr(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCOut2BufPtr(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCOut2NextPtr(SRC_PTR_s src_ptr);
EXTERN  VOID* DSP_GetSRCOutBufPtr(SRC_PTR_s src_ptr, U32 CH);
EXTERN  VOID* DSP_GetSRCOutNextPtr(SRC_PTR_s src_ptr, U32 CH);
EXTERN  U32 DSP_GetSRCOutWriteOffset(SRC_PTR_s src_ptr);
EXTERN  U32 DSP_GetSRCOutBufSize(SRC_PTR_s src_ptr);
EXTERN  U32 DSP_GetSRCOutFrameSize(SRC_PTR_s src_ptr);
EXTERN  VOID DSP_SetSRCTrigger(SRC_PTR_s VOLATILE src_ptr);
EXTERN  BOOL DSP_GetSRCStatus(SRC_PTR_s VOLATILE src_ptr);
EXTERN  VOID* DSP_GetAudioInNextPtr(ADMA_CH_NUM_s Channel_sel);
EXTERN  VOID* DSP_GetAudioOutNextPtr(ADMA_CH_NUM_s Channel_sel);
EXTERN  SRC_PTR_s DSP_GET_SRC_SFR(AU_HW_SRC_SEL sel);

EXTERN  VOID DSP_SetSRCCompensation(SRC_PTR_s src_ptr, U32 sign, U32 value);
EXTERN  VOID DSP_SetADCompensation(U32 sign, U32 value);


EXTERN  SRC_PTR_s DSP_DRV_SRC_VDM_INIT(DSP_DRV_SRC_VDM_INIT_STRU_PTR src_setting);
EXTERN  VOID DSP_DRV_SINK_SRC_VDM_PreTrigger(SINK sink, STREAM_SRC_VDM_PTR_t VOLATILE src_vdm);
EXTERN  VOID DSP_DRV_SOURCE_SRC_VDM_PreTrigger(SOURCE source, STREAM_SRC_VDM_PTR_t VOLATILE src_vdm);
EXTERN VOID DSP_DRV_ClearReadAdmaIsrStatus(VOID);
EXTERN VOID DSP_DRV_ClearSrcIsrStatus(VOID);
EXTERN VOID DSP_DRV_ClearVpIsrStatus(VOID);
EXTERN VOID DSP_DRV_ClearWriteAdmaIsrStatus(VOID);

EXTERN VOID DSP_DRV_SelectOdfeClk (AU_ODFE_CLK_GATE_t Clk);
EXTERN VOID DSP_DRV_SelectIdfeClk (AU_IDFE_CLK_GATE_t Clk);
EXTERN VOID DSP_DRV_EnableOdfeClock (VOID);
EXTERN VOID DSP_DRV_DisableOdfeClock (VOID);
EXTERN VOID DSP_DRV_EnableIdfeClock (VOID);
EXTERN VOID DSP_DRV_DisableIdfeClock (VOID);


/******************************************************************************
 * Inline Functions
 ******************************************************************************/
STATIC INLINE VOID DSP_DRV_DisableDFE(VOID)
{
    return;
}



#endif /* _DSP_DRV_DFE_H_ */

