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

#ifndef __AUDIO_NVDM_COEEF_H__
#define __AUDIO_NVDM_COEEF_H__

#include "types.h"
#include "nvkey_dspalg.h"
#include "nvkey_dspfw.h"

#define NVKEY_CM4_COMMON	0xbeef
#define NVKEY_END			0xffff

typedef enum	// from 1530 DSP_SDK.h
{
    DSP_DECODER_TYPE = 0,
    CODEC_NULL = DSP_DECODER_TYPE,
    CODEC_DECODER_VP,
    CODEC_DECODER_RT,
    CODEC_DECODER_SBC,
    CODEC_DECODER_AAC,
    CODEC_DECODER_MP3,
    CODEC_DECODER_MSBC,
    CODEC_DECODER_EC,
    CODEC_DECODER_UART,
    CODEC_DECODER_UART16BIT,
    CODEC_DECODER_CELT_HD,
    CODEC_DECODER_CVSD,
    CODEC_DECODER_SAMPLE,

    DSP_ENCODER_TYPE = 0x20,
    CODEC_ENCODER_SBC = DSP_ENCODER_TYPE,
    CODEC_ENCODER_MSBC,
    CODEC_ENCODER_SB_WF,
	CODEC_ENCODER_CVSD,

    DSP_FUNC_TYPE = 0x40,
    FUNC_END = DSP_FUNC_TYPE,
    FUNC_JOINT,
    FUNC_BRANCH,
    FUNC_MUTE,
    FUNC_PEQ_A2DP,
    FUNC_PEQ_LINEIN,
    FUNC_LPF,
    FUNC_CH_SEL,
    FUNC_SOUND_EFFECT,
    FUNC_PLC,
    FUNC_RX_NR,
    FUNC_TX_NR,
    FUNC_RX_WB_CPD,
    FUNC_RX_NB_CPD,
    FUNC_TX_WB_CPD,
    FUNC_TX_NB_CPD,
    FUNC_VC = 0x50,
    FUNC_VP_CPD,
    FUNC_DUMP_STREAM1,
    FUNC_DUMP_STREAM2,
    FUNC_PEQ2,
    FUNC_ANC1,
    FUNC_ANC2,
    FUNC_ANC3,
    FUNC_ANC4,
    FUNC_ANC5,
    FUNC_ANC6,
    FUNC_ANC7,
    FUNC_ANC_1,
    FUNC_ANC_2,
    FUNC_ANC_3,
    FUNC_ANC_4,
    FUNC_PASSTHRU_1,
    FUNC_PASSTHRU_2,
    FUNC_PASSTHRU_3,
    FUNC_VOICE_WB_ANC,
    FUNC_VOICE_NB_ANC,
    FUNC_INS,
    FUNC_ADAPTIVE_FF,
    FUNC_GAMING_HEADSET,
    FUNC_GAMING_BOOM_MIC,
    FUNC_WB_BOOM_MIC,
    FUNC_NB_BOOM_MIC,
    FUNC_HEARING_AID,
    FUNC_GAME_CHAT_VOLUME_SMART_BALANCE,
    FUNC_TX_NR_v2,
    FUNC_TX_POST_ECNR,
    FUNC_SAMPLE,

    DSP_FEATURE_MAX_NUM = 0x70,
    DSP_SRC = DSP_FUNC_TYPE-1,

} DSP_FEATURE_TYPE, DSP_FEATURE_TYPE_LIST, *DSP_FEATURE_TYPE_PTR, *DSP_FEATURE_TYPE_LIST_PTR;

typedef enum
{
	NVDM_UT_FAIL = 0,
	NVDM_UT_PASS
} NVDM_UT;

// ======================================================================================================
// ======================================================================================================
/*
#ifdef WIN32
	#define PACKED
#else
	#define PACKED __attribute__((__packed__))
#endif
*/
#endif  /* __AUDIO_NVDM_COEEF_H__ */
