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
 
#ifndef _DSP_DRV_AFE_HAL_H_
#define _DSP_DRV_AFE_HAL_H_

#include "types.h"


/******************************************************************************
 * Type Definitions
 ******************************************************************************/
typedef enum AU_SIG_CTRL_e
{
    DISABLE_AUDIO_CTRL     = 0,
    ENABLE_AUDIO_CTRL      = 1,
} AU_SIG_CTRL_t;

typedef enum AU_MIC_LINE_CTRL_e
{
    AU_IN_SEL_LINE          = 0,
    AU_IN_SEL_MIC           = 1,
} AU_MIC_LINE_CTRL_t;


typedef enum AU_POWER_ON_SEQ_SEL_e
{
    SEQ_BY_TCON             = 0,
    SEQ_BY_3WIRE            = 1,
} AU_POWER_ON_SEQ_SEL_t;


typedef enum AU_GAIN_CTRL_SEL_e
{
    GAIN_CTRL_BY_CSR        = 0,
    GAIN_CTRL_BY_3WIRE      = 1,
} AU_GAIN_CTRL_SEL_t;


typedef enum AU_RAMP_VAL_e
{
    RAMP_VAL_52_MS          = 0,
    RAMP_VAL_103_MS         = 1,
    RAMP_VAL_154_MS         = 2,
    RAMP_VAL_205_MS         = 3,    /* Default */
    RAMP_VAL_257_MS         = 4,
    RAMP_VAL_308_MS         = 5,
    RAMP_VAL_359_MS         = 6,
    RAMP_VAL_410_MS         = 7,
    RAMP_VAL_461_MS         = 8,
    RAMP_VAL_513_MS         = 9,
    RAMP_VAL_564_MS         = 10,
    RAMP_VAL_615_MS         = 11,
    RAMP_VAL_666_MS         = 12,
    RAMP_VAL_717_MS         = 13,
    RAMP_VAL_769_MS         = 14,
} AU_RAMP_VAL_t;


typedef enum AU_IN_TCON_SIG_e
{
    DISABLE_AU_IN_SUBBLOCK  = 0,
    ENABLE_AU_IN_SUBBLOCK   = 1,
} AU_IN_TCON_SIG_t;


typedef enum AU_ADC_3RD_STAGE_e
{
    VOICE_2ND_SD_ADC        = 0,
    MUSIC_3RD_SD_ADC        = 1,
} AU_ADC_3RD_STAGE_t;

typedef enum AU_BANDGAP_OUT_VOL_e
{
    OUT_VOL_DEC_30_P        = 0,
    OUT_VOL_DEC_25_P        = 1,
    OUT_VOL_DEC_20_P        = 2,
    OUT_VOL_DEC_15_P        = 3,
    OUT_VOL_DEC_10_P        = 4,
    OUT_VOL_DEC_5_P         = 5,
    OUT_VOL_FIX             = 6,
    OUT_VOL_INC_5_P         = 7,
} AU_BANDGAP_OUT_VOL_t;

typedef enum AU_MIC_BIAS_MODE_e
{
    MIC_BIAS_BYPASS_MODE    = 0,
    MIC_BIAS_NORMAL_MODE    = 1,
} AU_MIC_BIAS_MODE_t;

typedef enum AU_MIC_BIAS_VOL_e
{
    MIC_BIAS_VOL_1_48       = 0,
    MIC_BIAS_VOL_1_59       = 0,
    MIC_BIAS_VOL_1_72       = 0,
    MIC_BIAS_VOL_1_87       = 0,
    MIC_BIAS_VOL_1_96       = 0,
    MIC_BIAS_VOL_2_06       = 0,
    MIC_BIAS_VOL_2_16       = 0,
    MIC_BIAS_VOL_2_28       = 0,
    MIC_BIAS_VOL_2_42       = 0,
    MIC_BIAS_VOL_2_49       = 0,
    MIC_BIAS_VOL_2_56       = 0,
    MIC_BIAS_VOL_2_65       = 0,
    MIC_BIAS_VOL_2_74       = 0,
    MIC_BIAS_VOL_2_83       = 0,
    MIC_BIAS_VOL_2_93       = 0,
    MIC_BIAS_VOL_3_00       = 0,
} AU_MIC_BIAS_VOL_t;

typedef enum AU_BIAS_CUR_e
{
    MIC_BIAS_CUR_2_50       = 0,
    MIC_BIAS_CUR_3_75       = 1,
    MIC_BIAS_CUR_5_00       = 2,
    MIC_BIAS_CUR_6_25       = 3,
} AU_BIAS_CUR_t;

typedef enum AU_MIC_GAIN_e
{
    MIC_GAIN_0dB            = 0,
    MIC_GAIN_3dB            = 1,
    MIC_GAIN_6dB            = 2,
    MIC_GAIN_9dB            = 3,
    MIC_GAIN_12dB           = 4,
    MIC_GAIN_15dB           = 5,
    MIC_GAIN_18dB           = 6,
    MIC_GAIN_21dB           = 7,
    MIC_GAIN_21_dB          = 8,
    MIC_GAIN_24dB           = 9,
    MIC_GAIN_27dB           = 10,
    MIC_GAIN_30dB           = 11,
    MIC_GAIN_33dB           = 12,
    MIC_GAIN_36dB           = 13,
    MIC_GAIN_39dB           = 14,
    MIC_GAIN_42dB           = 15,
} AU_MIC_GAIN_t;


typedef enum AU_LINE_GAIN_e
{
    LINE_GAIN_DEC_1_93dB     = 0,
    LINE_GAIN_DEC_1_41dB     = 1,
    LINE_GAIN_DEC_0_92dB     = 2,
    LINE_GAIN_DEC_0_45dB     = 3,
    LINE_GAIN_FIX            = 4,
    LINE_GAIN_INC_0_42dB     = 5,
    LINE_GAIN_INC_0_83dB     = 6,
    LINE_GAIN_INC_1_58dB     = 7,
    LINE_GAIN_INC_1_94dB     = 8,
    LINE_GAIN_INC_2_61dB     = 9,
    LINE_GAIN_INC_2_92dB     = 10,
    LINE_GAIN_INC_3_52dB     = 11,
    LINE_GAIN_INC_4_08dB     = 12,
} AU_LINE_GAIN_t;


typedef enum AU_ADC_IN_SEL_e
{
    AU_ADC_NO_INPUT             = 0,
    AU_ADC_INPUT_FROM_MIC_AMP   = 1,
    AU_ADC_INPUT_FROM_LINE      = 2,
    AU_ADC_INPUT_FROM_DIR_MIC   = 4,
} AU_ADC_IN_SEL_t;

typedef enum AU_EAR_BIAS_CUR_e
{
    EAR_BIAS_CUR_2_50       = 0,
    EAR_BIAS_CUR_5_00       = 1,
    EAR_BIAS_CUR_7_50       = 2,
    EAR_BIAS_CUR_10_00      = 3,
} AU_EAR_BIAS_CUR_t;


typedef enum AU_OUT_STAT_CTRL_e
{
    AU_OUT_SPK_NORMAL       = 0,
    AU_OUT_SPK_TO_VCM       = 1,
    AU_OUT_SPK_UNUSED       = 2,
    AU_OUT_SPK_TO_VCC_SPK   = 3,
} AU_OUT_STAT_CTRL_t;


typedef enum AU_ADC_CUR_e
{
    AU_ADC_CUR_2_50         = 0,
    AU_ADC_CUR_3_75         = 1,
    AU_ADC_CUR_5_00         = 2,
    AU_ADC_CUR_6_25         = 3,
} AU_ADC_CUR_t;


typedef enum AU_ADC_POWER_MODE_e
{
    AU_ADC_NORMAL_SNR_MODE  = 0,
    AU_ADC_HIGH_SNR_MODE    = 1,
} AU_ADC_POWER_MODE_t;


typedef enum AU_ADC_LV_MODE_e
{
    AU_ADC_3_0_V_MODE       = 0,
    AU_ADC_1_5_V_MODE       = 1,
} AU_ADC_LV_MODE_t;

typedef enum AU_ADC_BIAS_CUR_e
{
    ADC_BIAS_CUR_2_50       = 0,
    ADC_BIAS_CUR_5_00       = 1,
    ADC_BIAS_CUR_7_50       = 2,
    ADC_BIAS_CUR_10_00      = 3,
} AU_ADC_BIAS_CUR_t;


typedef enum AU_DAC_DSM_IN_GAIN_e
{
    DAC_DSM_IN_GAIN_1_00    = 0,
    DAC_DSM_IN_GAIN_0_50    = 1,
    DAC_DSM_IN_GAIN_0_25    = 2,
    DAC_DSM_IN_GAIN_0_125   = 3,
} AU_DAC_DSM_IN_GAIN_t;


typedef enum AU_DAC_IN_RF_MODE_e
{
    DAC_DISABLE_MODE            = 0,
    DAC_EXTERNAL_GPIO_MODE      = 1,
    DAC_INTERNAL_TONE_MODE      = 2,
    DAC_INTERNAL_SILENCE_MODE   = 3,
} AU_DAC_IN_RF_MODE_t;

typedef enum AU_DAC_DSM_DYNAMIC_RANGE_e
{
    DAC_DSM_DYNAMIC_RANGE_SMALL     = 0,
    DAC_DSM_DYNAMIC_RANGE_NORMAL    = 1,
    DAC_DSM_DYNAMIC_RANGE_LARGE     = 2,
    DAC_DSM_DYNAMIC_RANGE_UNDEFINED = 3,
} AU_DAC_DSM_DYNAMIC_RANGE_t;


typedef enum AU_DAC_DITHER_MODE_e
{
    AU_DAC_DITHER_MODE_0_0625   = 0,
    AU_DAC_DITHER_MODE_0_125    = 1,
    AU_DAC_DITHER_MODE_0_250    = 2,
    AU_DAC_DITHER_MODE_0_500    = 3,
} AU_DAC_DITHER_MODE_t;


typedef enum AU_DAC_DSM_MODE_e
{
    AU_DAC_DSM_MODE_CHEBY_15K   = 0,
    AU_DAC_DSM_MODE_CHEBY_10K   = 1,
    AU_DAC_DSM_MODE_CHEBY_7K    = 2,
    AU_DAC_DSM_MODE_BUTTERWORTH = 3,
} AU_DAC_DSM_MODE_t;


typedef enum AU_DAC_DEM_MODE_e
{
    AU_DAC_DEM_MODE_PART_13_UES = 0,
    AU_DAC_DEM_MODE_PART_14_UES = 1,
    AU_DAC_DEM_MODE_PART_15_UES = 2,
    AU_DAC_DEM_MODE_COVENTIONAL = 3,
} AU_DAC_DEM_MODE_t;

typedef enum AU_MXT_TO_DAC_CLK_RATE_e
{
    AU_MXT_TO_DAC_CLK_7_2_MHZ   = 0,
    AU_MXT_TO_DAC_CLK_4_8_MHZ   = 1,
    AU_MXT_TO_DAC_CLK_3_6_MHZ   = 2,
    AU_MXT_TO_DAC_CLK_2_4_MHZ   = 3,
} AU_MXT_TO_DAC_CLK_RATE_t;


typedef enum AU_DAC_DSM_GAIN_e
{
    AU_DAC_DSM_GAIN_0_9375      = 0,
    AU_DAC_DSM_GAIN_0_875       = 1,
    AU_DAC_DSM_GAIN_0_8125      = 2,
    AU_DAC_DSM_GAIN_0_75        = 3,
    AU_DAC_DSM_GAIN_0_625       = 4,
} AU_DAC_DSM_GAIN_t;


typedef enum AU_OUT_ENABLE_SIG_VOL_e
{
    AU_OUT_1_8_V                = 0,
    AU_OUT_3_0_V                = 1,
} AU_OUT_ENABLE_SIG_VOL_t;


typedef enum AU_OUT_MODE_e
{
    AU_OUT_DIFFERENTIAL_MODE        = 0,
    AU_OUT_DIFFERENTIAL_MODE_       = 1,
    AU_OUT_SINGLE_END_MODE          = 2,
    AU_OUT_SINGLE_END_MODE_LRVCM    = 3,
} AU_OUT_MODE_t;


typedef enum AU_EAR_DRV_CURR_e
{
    AU_EAR_DRV_CURRENT_LOW          = 0,
    AU_EAR_DRV_CURRENT_HIGH         = 1,
} AU_EAR_DRV_CURR_t;


typedef enum AU_VAD_MIC_SEL_e
{
    AU_VAD_BY_MAIN_MIC          	= 0,
    AU_VAD_BY_ANC_MIC         		= 1,
} AU_VAD_MIC_SEL_t;


typedef enum AU_IDAC_RAMP_TIME_e
{
    AU_IDAC_RAMP_TIME_4MS           = 0,
    AU_IDAC_RAMP_TIME_8MS           = 1,
    AU_IDAC_RAMP_TIME_16MS          = 2,
    AU_IDAC_RAMP_TIME_32MS          = 3,
} AU_IDAC_RAMP_TIME_t;


typedef enum AU_AGC_CLK_DIVIDER_e
{
    AU_AGC_CLK_DIV_TIME_UNIT_0_25MS = 0,
    AU_AGC_CLK_DIV_TIME_UNIT_0_50MS = 1,
    AU_AGC_CLK_DIV_TIME_UNIT_1_00MS = 2,
    AU_AGC_CLK_DIV_TIME_UNIT_2_00MS = 3,
} AU_AGC_CLK_DIVIDER_t;


typedef enum AU_AGC_GAIN_STEP_e
{
    AU_AGC_GAIN_STEP_0_375DB_       = 0,
    AU_AGC_GAIN_STEP_0_375DB        = 1,
    AU_AGC_GAIN_STEP_0_750DB        = 2,
    AU_AGC_GAIN_STEP_1_125DB        = 3,
} AU_AGC_GAIN_STEP_t;


typedef enum AU_AGC_RAMP_DELAY_e
{
    AU_AGC_RAMP_DELAY_2000TU        = 0,
    AU_AGC_RAMP_DELAY_4000TU        = 1,
    AU_AGC_RAMP_DELAY_8000TU        = 2,
    AU_AGC_RAMP_DELAY_16000TU       = 3,
} AU_AGC_RAMP_DELAY_t;


typedef enum AU_AGC_ATTACK_TIME_e
{
    AU_AGC_ATTACK_TIME_2TU          = 0,
    AU_AGC_ATTACK_TIME_5TU          = 1,
    AU_AGC_ATTACK_TIME_10TU         = 2,
    AU_AGC_ATTACK_TIME_20TU         = 3,
    AU_AGC_ATTACK_TIME_40TU         = 4,
    AU_AGC_ATTACK_TIME_80TU         = 5,
    AU_AGC_ATTACK_TIME_160TU        = 6,
    AU_AGC_ATTACK_TIME_250TU        = 7,
} AU_AGC_ATTACK_TIME_t;


typedef enum AU_AGC_RELEASE_TIME_e
{
    AU_AGC_RELEASE_TIME_2TU         = 0,
    AU_AGC_RELEASE_TIME_4TU         = 1,
    AU_AGC_RELEASE_TIME_8TU         = 2,
    AU_AGC_RELEASE_TIME_16TU        = 3,
    AU_AGC_RELEASE_TIME_64TU        = 4,
    AU_AGC_RELEASE_TIME_128TU       = 5,
    AU_AGC_RELEASE_TIME_256TU       = 6,
    AU_AGC_RELEASE_TIME_512TU       = 7,
} AU_AGC_RELEASE_TIME_t;


typedef enum AU_AGC_DEBOUNCE_TIME_e
{
    AU_AGC_DEBOUNCE_TIME_0TU        = 0,
    AU_AGC_DEBOUNCE_TIME_2TU        = 1,
    AU_AGC_DEBOUNCE_TIME_4TU        = 2,
    AU_AGC_DEBOUNCE_TIME_8TU        = 3,
    AU_AGC_DEBOUNCE_TIME_32TU       = 4,
    AU_AGC_DEBOUNCE_TIME_64TU       = 5,
    AU_AGC_DEBOUNCE_TIME_128TU      = 6,
    AU_AGC_DEBOUNCE_TIME_256TU      = 7,
} AU_AGC_DEBOUNCE_TIME_t;


typedef enum AU_AGC_DEC_SETTLING_TIME_e
{
    AU_AGC_SETTLING_TIME_2TU        = 0,
    AU_AGC_SETTLING_TIME_5TU        = 1,
    AU_AGC_SETTLING_TIME_10TU       = 2,
    AU_AGC_SETTLING_TIME_20TU       = 3,
} AU_AGC_DEC_SETTLING_TIME_t;


typedef enum AU_POWER_SWITCH_MODE_e
{
    AU_POWER_SWITCH_AUTO_MODE       = 0,
    AU_POWER_SWITCH_MANUAL_MODE     = 1,
} AU_POWER_SWITCH_MODE_t;


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
EXTERN VOID DSP_DRV_SelectMicOrLine (AU_MIC_LINE_CTRL_t AudioInSelL, AU_MIC_LINE_CTRL_t AudioInSelR);
EXTERN VOID DSP_DRV_3WireValIntoBitField (U8 Addr, U16 Val, U32 BitLeng, U32 BitOffset);

EXTERN VOID DSP_DRV_EnableMicBias1 (AU_SIG_CTRL_t Val); //0x23
EXTERN VOID DSP_DRV_EnableANCMicBias1 (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetANCEarGain (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableANC_DCOC (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectANC_DCOCL (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectANC_DCOCR (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableANCLimiter (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableANCMute (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableManualEarPullLowMode (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectEarPullLowCtrl (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectShortOutputToVCM (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectMuteEarOnCtrl (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectManulModeInHZFunc (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableSwVrefTest (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectAuADC_VCM (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectANCIBiasLimiter(U16 Val);
EXTERN VOID DSP_DRV_SelectANCIBias_Limiter(U16 Val);
EXTERN VOID DSP_DRV_SelectANCIBias_DCOC(U16 Val);
EXTERN VOID DSP_DRV_SelectANCMicBias1_ROUT(U16 Val);
EXTERN VOID DSP_DRV_SelectMicBias1_ROUT(U16 Val);
EXTERN VOID DSP_DRV_SelectANCMicBias1Mode(U16 Val);
EXTERN VOID DSP_DRV_SelectMicBias1Mode(U16 Val);
EXTERN VOID DSP_DRV_SelectANCMicBiasV1(U16 Val);
EXTERN VOID DSP_DRV_SelectMicBiasV1(U16 Val);
EXTERN VOID DSP_DRV_SelectDepopCtrl(U16 Val);
EXTERN VOID DSP_DRV_LimitHighBound_R(U16 Val);
EXTERN VOID DSP_DRV_LimitLowBound_R(U16 Val);
EXTERN VOID DSP_DRV_LimitHighBound_L(U16 Val);
EXTERN VOID DSP_DRV_LimitLowBound_L(U16 Val);
EXTERN VOID DSP_DRV_CalibrateNegativeDCOffsetANC_R(U16 Val);
EXTERN VOID DSP_DRV_CalibratePositiveDCOffsetANC_R(U16 Val);
EXTERN VOID DSP_DRV_CalibrateNegativeDCOffsetANC_L(U16 Val);
EXTERN VOID DSP_DRV_CalibratePositiveDCOffsetANC_L(U16 Val);
EXTERN VOID DSP_DRV_InitialAuADCDsmIntx(U32 Val);
EXTERN VOID DSP_DRV_EnableDsmInt1OnInitVal(U32 Val);
EXTERN VOID DSP_DRV_EnableDsmInt2OnInitVal(U32 Val);
EXTERN VOID DSP_DRV_EnableDsmInt3OnInitVal(U32 Val);

EXTERN VOID DSP_DRV_SetVADOffset (U16 Val); //0x4A

EXTERN VOID DSP_DRV_SelectDepopAnaIBIAS (U16 Val); //0x57
EXTERN VOID DSP_DRV_SelectDepopAnaCAP (U16 Val);
EXTERN VOID DSP_DRV_SelectPowerOnSequence (AU_POWER_ON_SEQ_SEL_t Val);
EXTERN VOID DSP_DRV_SelectGainCtrl (AU_GAIN_CTRL_SEL_t Val);
EXTERN VOID DSP_DRV_SelectAnalogDacRamp (AU_RAMP_VAL_t Val);
EXTERN VOID DSP_DRV_ControlAuInTconSig (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_ControlAuOutTconSig (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableVCMBufOut (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAudioBandGap (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableMicBias (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableANCMicBias (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableVCMLine (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableVPRAuDAC (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableVCMEAR (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAuIBIASTop (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableMic_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableMic_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableLine_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableLine_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAuADC_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAuADC_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_Sel3rdAuADC_L (AU_ADC_3RD_STAGE_t Val);
EXTERN VOID DSP_DRV_Sel3rdAuADC_R (AU_ADC_3RD_STAGE_t Val);
EXTERN VOID DSP_DRV_EnableAuDAC_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAuDAC_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAuEAR_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAuEAR_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableGPIOAnc_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableGPIOAnc_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableMicAmpAnc_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableMicAmpAnc_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectBandGapOutVoltage (AU_BANDGAP_OUT_VOL_t Val);
EXTERN VOID DSP_DRV_SelectMicBiasMode (AU_MIC_BIAS_MODE_t Val);
EXTERN VOID DSP_DRV_SelectMicBiasAncMode (AU_MIC_BIAS_MODE_t Val);
EXTERN VOID DSP_DRV_SelectMicBiasV (AU_MIC_BIAS_VOL_t Val);
EXTERN VOID DSP_DRV_SelectMicBiasAncV (AU_MIC_BIAS_VOL_t Val);
EXTERN VOID DSP_DRV_SelectVcmMic (U16 Val);
EXTERN VOID DSP_DRV_SelectVcmMicX (U16 Val);
EXTERN VOID DSP_DRV_SelectVcmLine (U16 Val);
EXTERN VOID DSP_DRV_SelectVrpAuDAC (U16 Val);
EXTERN VOID DSP_DRV_EnableTestVcmEAR (U16 Val);
EXTERN VOID DSP_DRV_SelectVcmEAR (U16 Val);
EXTERN VOID DSP_DRV_SelectAuRefBypass (U16 Val);
EXTERN VOID DSP_DRV_EnableAuRefBypass (U16 Val);
EXTERN VOID DSP_DRV_EnableAuRefShort (U16 Val);
EXTERN VOID DSP_DRV_SetMicBiasCurrent (AU_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetLineVcmBufBiasCurrent (AU_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetLineOpBiasCurrent_L (AU_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetLineOpBiasCurrent_R (AU_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetAuADCRefCurrent_L (AU_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetAuADCRefCurrent_R (AU_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetAuDACRefCurrent (AU_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetEarAmpVcmCurrent (AU_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetMicGain_L (AU_MIC_GAIN_t Val);
EXTERN VOID DSP_DRV_SetMicGain_R (AU_MIC_GAIN_t Val);
EXTERN VOID DSP_DRV_SetLineGain_L (AU_LINE_GAIN_t Val);
EXTERN VOID DSP_DRV_SetLineGain_R (AU_LINE_GAIN_t Val);
EXTERN VOID DSP_DRV_SelectAuADCInput_L (AU_ADC_IN_SEL_t Val);
EXTERN VOID DSP_DRV_SelectAuADCInput_R (AU_ADC_IN_SEL_t Val);
EXTERN VOID DSP_DRV_SelectEARBiasCurrent (AU_EAR_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_ControlAuADCDitherLvl (U16 Val);
EXTERN VOID DSP_DRV_EnableAuADCDither (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetAuADCDitherLength (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_ResetAuADCDither (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_InvertAuADCDitherClk (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_ControlAudioOutputStatus (AU_OUT_STAT_CTRL_t Val);
EXTERN VOID DSP_DRV_SelAuADCOp1BiasCurrent (AU_ADC_CUR_t Val);
EXTERN VOID DSP_DRV_SelAuADCOp2BiasCurrent (AU_ADC_CUR_t Val);
EXTERN VOID DSP_DRV_SelAuADCOp3BiasCurrent (AU_ADC_CUR_t Val);
EXTERN VOID DSP_DRV_SelAuADCQuant1BiasCurrent (AU_ADC_CUR_t Val);
EXTERN VOID DSP_DRV_SelAuADCQuant2BiasCurrent (AU_ADC_CUR_t Val);
EXTERN VOID DSP_DRV_SelectAuADCPowerMode_L (AU_ADC_POWER_MODE_t Val);
EXTERN VOID DSP_DRV_SelectAuADCPowerMode_R (AU_ADC_POWER_MODE_t Val);
EXTERN VOID DSP_DRV_SelectAuADCLvMode (AU_ADC_LV_MODE_t Val);
EXTERN VOID DSP_DRV_SelectAuADCData (AU_ADC_LV_MODE_t Val);
EXTERN VOID DSP_DRV_SelectAuADCBiasCurrent (AU_ADC_BIAS_CUR_t Val);
EXTERN VOID DSP_DRV_SetAuDacDsmInGain (AU_DAC_DSM_IN_GAIN_t Val);
EXTERN VOID DSP_DRV_SetAuDacInRfMode (AU_DAC_IN_RF_MODE_t Val);
EXTERN VOID DSP_DRV_SetAuDacDsmDynamicRange (AU_DAC_DSM_DYNAMIC_RANGE_t Val);
EXTERN VOID DSP_DRV_SetAuDacDitherMode (AU_DAC_DITHER_MODE_t Val);
EXTERN VOID DSP_DRV_SetAuDacDsmMode (AU_DAC_DSM_MODE_t Val);
EXTERN VOID DSP_DRV_SetAuDacDemMode (AU_DAC_DEM_MODE_t Val);
EXTERN VOID DSP_DRV_EnableAuDacDem (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAuDacDsmDither (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectMxtToAuDacClockRate (AU_MXT_TO_DAC_CLK_RATE_t Val);
EXTERN VOID DSP_DRV_SelectAuAdcDsmGain (AU_DAC_DSM_GAIN_t Val);
EXTERN VOID DSP_DRV_SelectAuOutVoltage (AU_OUT_ENABLE_SIG_VOL_t Val);
EXTERN VOID DSP_DRV_EnableLineEAR_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableLineEAR_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAncEAR_L (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAncEAR_R (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetAuOutputMode (AU_OUT_MODE_t Val);
EXTERN VOID DSP_DRV_EnableLpModeEar (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableEarSwCs (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableDiv2CsEar (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectEarDriveStandbyCurrent (AU_EAR_DRV_CURR_t Val);
EXTERN VOID DSP_DRV_SetAncGain_L (U16 Val);
EXTERN VOID DSP_DRV_SetAncGain_R (U16 Val);
EXTERN VOID DSP_DRV_SelectMicForVad (AU_VAD_MIC_SEL_t Val);
EXTERN VOID DSP_DRV_SetVadMicGain (AU_VAD_MIC_SEL_t Val);
EXTERN VOID DSP_DRV_SetAuIDac (U16 Val);

EXTERN VOID DSP_DRV_SetAuIDacT8 (AU_IDAC_RAMP_TIME_t Val); //0x73
EXTERN VOID DSP_DRV_SetAuIDacT7 (AU_IDAC_RAMP_TIME_t Val);
EXTERN VOID DSP_DRV_SetAuIDacT6 (AU_IDAC_RAMP_TIME_t Val);
EXTERN VOID DSP_DRV_SetAuIDacT5 (AU_IDAC_RAMP_TIME_t Val);
EXTERN VOID DSP_DRV_SetAuIDacT4 (AU_IDAC_RAMP_TIME_t Val);
EXTERN VOID DSP_DRV_SetAuIDacT3 (AU_IDAC_RAMP_TIME_t Val);
EXTERN VOID DSP_DRV_SetAuIDacT2 (AU_IDAC_RAMP_TIME_t Val);
EXTERN VOID DSP_DRV_SetAuIDacT1 (AU_IDAC_RAMP_TIME_t Val);
EXTERN VOID DSP_DRV_EnableGcMinEar (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableGcMinEarPop (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectAncLineIn (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectAgcDbgPortAsLeft (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAgcMonitorMode (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SyncAgcLRGainCtrl (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectAgcGainCtrl (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnableAgcGainCtrl (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectAgcRampDirection (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetAgcClockFree (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectAgcMode (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetAgcClockDivider (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetAgcClockDivider (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_ChangePhaseToLimitDownGain (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_ChangePhaseToLimitUpGain (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetDecreaseGainDebounceMode (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetIncreaseGainDebounceMode (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SetAgcDecreaseGainStep (AU_AGC_GAIN_STEP_t Val);
EXTERN VOID DSP_DRV_SetAgcIncreaseGainStep (AU_AGC_GAIN_STEP_t Val);
EXTERN VOID DSP_DRV_SetAgcSoftStartStopGainStep (AU_AGC_GAIN_STEP_t Val);
EXTERN VOID DSP_DRV_SetAgcSoftStartStopGainRampDelay (AU_AGC_RAMP_DELAY_t Val);
EXTERN VOID DSP_DRV_SetAgcAttackTime (AU_AGC_ATTACK_TIME_t Val);
EXTERN VOID DSP_DRV_SetAgcReleaseTime (AU_AGC_RELEASE_TIME_t Val);
EXTERN VOID DSP_DRV_SetAgcGainDecreaseDebounceTime (AU_AGC_DEBOUNCE_TIME_t Val);
EXTERN VOID DSP_DRV_SetAgcGainDecreaseSettlingTime (AU_AGC_DEC_SETTLING_TIME_t Val);
EXTERN VOID DSP_DRV_SetAgcGainIncreaseDebounceTime (AU_AGC_DEBOUNCE_TIME_t Val);
EXTERN VOID DSP_DRV_SetAgcGainIncreaseSettlingTime (AU_AGC_DEC_SETTLING_TIME_t Val);

EXTERN VOID DSP_DRV_SetEarGain (S32 Val); //0x78
EXTERN VOID DSP_DRV_SetDepopGain (S32 Val); //0x79

EXTERN VOID DSP_DRV_EnablePowerSwitchInDeepSleepMode (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnablePowerSwitchInSleepMode (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_EnablePowerSwitchManualControl (AU_SIG_CTRL_t Val);
EXTERN VOID DSP_DRV_SelectPowerSwitchMode (AU_POWER_SWITCH_MODE_t Val);
EXTERN VOID DSP_DRV_Enable_AU_LSH_L2H_V33 (AU_SIG_CTRL_t Val);


/******************************************************************************
 * Inline Functions
 ******************************************************************************/




#endif /* _DSP_DRV_AFE_HAL_H_ */

