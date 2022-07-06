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

#ifndef _DSP_SHARE_MEMORY_H_
#define _DSP_SHARE_MEMORY_H_

#include "types.h"
#include "dsp_para_cpd.h"
#include "voice_plc_interface.h"
#include "audio_config.h"

typedef enum {
    DSP_ALG_LIB_TYPE_IGO,
    DSP_ALG_LIB_TYPE_MAX,
} dsp_alg_lib_type_t;

typedef struct DSP_ALG_SCO_MODE_s
{
	SCO_CODEC 	Tx;
	SCO_CODEC 	Rx;
} DSP_ALG_SCO_MODE_t;

typedef struct DSP_SHARE_MEM_s
{
	BOOL 				AecReady;
	ECNR_OUT 			AecNr;
	U8                  PostEC_Gain;
	DSP_ALG_SCO_MODE_t	EscoMode;
    int                 ECNR_Pitch_Last;
} DSP_SHARE_MEM_t;

EXTERN VOID DSP_ALG_UpdateEscoTxMode (SCO_CODEC mode);
EXTERN VOID DSP_ALG_UpdateEscoRxMode (SCO_CODEC mode);
#ifdef MTK_BT_A2DP_ENABLE
EXTERN VOID DSP_UpdateA2DPCodec (U8 codec_type);
#endif

EXTERN DSP_SHARE_MEM_t gDspAlgParameter;
EXTERN U8 gDspA2DPCodec;

#endif /* _DSP_SHARE_MEMORY_H_ */
