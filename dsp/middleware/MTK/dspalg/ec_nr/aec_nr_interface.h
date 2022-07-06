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
#ifndef _AEC_NR_INTERFACE_H_
#define _AEC_NR_INTERFACE_H_

#include "dsp_feature_interface.h"
#include "dsp_para_aec_nr.h"
#include "dsp_para_wb_rx_eq.h"
#include "dsp_para_wb_tx_eq.h"
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
#include "dsp_para_inear_eq.h"
#endif
#if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
#include "dsp_para_ast_eq.h"
#endif
#ifdef MTK_3RD_PARTY_NR
#include "dsp_para_reserved.h"
#if defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
#include "igo_nr.h"
#endif
#endif
#include "dsp_para_nb_rx_eq.h"
#include "dsp_para_nb_tx_eq.h"
#include "dsp_utilities.h"
#include "dsp_share_memory.h"

#define AEC_EXPAND_BUF                (0)
#define AEC_TEST_DEFAULT_PARA       (0)
#define DSP_AEC_OUT_BUF             (480)
#define DSP_AEC_NR_SCRATCH_MEMSIZE  (1) //26000
#define DSP_AEC_NR_MEMSIZE          (sizeof(CPD_EC_NR_INSTANCE))
#if defined(AIR_EC120_ENABLE)
#define DSP_EC_120_MEMSIZE          (sizeof(EC120_INSTANCE))
#endif

#define AEC_NR_PARAM_TYPE_RX_EQ     (0x1)
typedef U16 aec_nr_param_type_t;

EXTERN bool gfgAvcUpdate;
EXTERN bool gfgAvcSend;
EXTERN S16 gi2AvcVol;

typedef enum
{
    avc_vol_dB0 = 8192,
    avc_vol_dB1 = 9192,
    avc_vol_dB2 = 10314,
    avc_vol_dB3 = 11572,
    avc_vol_dB4 = 12984,
    avc_vol_dB5 = 14568,
    avc_vol_dB6 = 16346,
    avc_vol_dB7 = 18340,
    avc_vol_dB8 = 20578,
    avc_vol_dB9 = 23089,
    avc_vol_dB10 = 25906,
    avc_vol_dB11 = 29067,
    avc_vol_dB12 = 32613,
    avc_vol_max  = 32767,
} eAVC_VOL_DB;

typedef struct stru_dsp_aec_nr_nvkey_u
{
    DSP_ALIGN4 DSP_PARA_AEC_NR_STRU AecNr;
    DSP_ALIGN4 DSP_PARA_WB_RX_EQ_STRU RxEq;
    DSP_ALIGN4 DSP_PARA_WB_TX_EQ_STRU TxEq;
    #if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR)
    DSP_ALIGN4 DSP_PARA_InEarEQ_STRU InearEq;
    #endif
    #if defined(MTK_INEAR_ENHANCEMENT) || defined(MTK_DUALMIC_INEAR) || defined(AIR_IGO_NR_PLUS_1_ENABLE)
    DSP_ALIGN4 DSP_PARA_AST_EQ_STRU AstEq;
    #endif
    #ifdef MTK_3RD_PARTY_NR
    DSP_ALIGN4 DSP_PARA_RESERVED_STRU ResKey;
    #endif
} CPD_EC_NR_NVKEY_STATE;

#if (AEC_EXPAND_BUF)
typedef struct AEC_EXP_BUF_s
{
    S16 volatile AecTxId;
    S16 volatile AecRxId;
    DSP_ALIGN8 S8 AEC_TX_IN_L_BUF[480];
    DSP_ALIGN8 S8 AEC_TX_IN_R_BUF[480];
    DSP_ALIGN8 S8 AEC_TX_OUT_BUF[480];
    DSP_ALIGN8 S8 AEC_RX_IN_BUF[480];
    DSP_ALIGN8 S8 AEC_RX_OUT_BUF[480];
    DSP_ALIGN8 S8 AEC_ECHO_BUF[480];
} AEC_EXP_BUF_t, *AEC_EXP_BUF_PTR_t;
#endif

typedef struct stru_dsp_aec_nr_para_u
{
    U32 MemoryCheck;
    U32 deinit_process; // Only deinit AEC
	#ifdef MTK_INEAR_ENHANCEMENT
	S16 prev_g_f_wind;
    S16 prev_mic_status_o;
    #endif
	DSP_ALIGN4 CPD_EC_NR_NVKEY_STATE NvKey;
	#if (AEC_EXPAND_BUF)
	AEC_EXP_BUF_t BufExpansion;
	#endif
	DSP_ALIGN8 U8 AecOut[DSP_AEC_OUT_BUF];
    #if AIR_ULL_VOICE_LOW_LATENCY_ENABLE
    DSP_ALIGN8 U8 AecInTempBuf1[DSP_AEC_OUT_BUF]; // for 7.5ms afe buffering
    DSP_ALIGN8 U8 AecInTempBuf2[DSP_AEC_OUT_BUF]; // for 7.5ms afe buffering
    DSP_ALIGN8 U8 AecInTempBuf3[DSP_AEC_OUT_BUF]; // for 7.5ms afe buffering
    DSP_ALIGN8 U8 AecInTempBufec[DSP_AEC_OUT_BUF]; // for 7.5ms afe buffering
    DSP_ALIGN8 U8 AecOutTempBuf[DSP_AEC_OUT_BUF]; // for 7.5ms afe buffering
    #endif
	DSP_ALIGN8 U8 ScratchMemory[DSP_AEC_NR_SCRATCH_MEMSIZE]; //Set at the end of structure for dynamic size
} CPD_EC_NR_INSTANCE, *CPD_EC_NR_INSTANCE_PTR;

#if defined(AIR_EC120_ENABLE)
typedef struct stru_dsp_ec120_para_u
{
    U32 MemoryCheck;
    DSP_ALIGN4 CPD_EC_NR_NVKEY_STATE NvKey;
    DSP_ALIGN8 U8 AecOut[DSP_AEC_OUT_BUF];
    DSP_ALIGN8 U8 DNN_IN[DSP_AEC_OUT_BUF];
    DSP_ALIGN8 U8 DNN_OUT[DSP_AEC_OUT_BUF];
    DSP_ALIGN8 U8 ScratchMemory[DSP_AEC_NR_SCRATCH_MEMSIZE]; //Set at the end of structure for dynamic size
} EC120_INSTANCE, *EC120_INSTANCE_PTR;
#endif

#ifdef MTK_AIRDUMP_EN
typedef struct{
    bool past_state;
    bool over_halfbuf;
} AIRDUMP_WO_STATE_t;

typedef struct{
    U32 read_offset;
    U32 write_offset;
    U16 length;
    U8  notify_count;
#if defined (MTK_INEAR_ENHANCEMENT) || defined (MTK_DUALMIC_INEAR)
    U8  data[240];
#else
    U8  data[170];
#endif
} AIRDUMPCTRL_t;

typedef struct{
    AIRDUMP_WO_STATE_t  wo_state;
    bool                airdump_en;
} AIRDUMP_DSP_CTRL_t;

EXTERN AIRDUMPCTRL_t * rAirDumpCtrl;
EXTERN AIRDUMP_DSP_CTRL_t rAirDumpDSPCtrl;
#endif

EXTERN void stream_function_aec_nr_deinitialize (bool is_aec_only);
EXTERN bool stream_function_aec_nr_initialize (void *para);
EXTERN bool stream_function_aec_process (void *para);
EXTERN bool stream_function_nr_process (void *para);
EXTERN VOID AEC_NR_GetRefGain (S16* GainAddr);
EXTERN uint32_t AEC_NR_Query_Lib_Version(dsp_alg_lib_type_t type, uint8_t *version);
bool AEC_NR_Check2MicEnabled (void);
EXTERN void AEC_NR_Enable(hal_ccni_message_t msg, hal_ccni_message_t *ack);
#ifdef MTK_AIRDUMP_EN
void AEC_NR_AirDumpEnable (bool state);
void AEC_NR_AirDump (void);
void AirDump_update_writeoffset_share_information (U32 writeoffset);
void AirDump_update_notify_count(U32 writeoffset);
#endif
bool avc_vol_compare(S16 i2PreAvcVol, S16 i2PostAvcVol);
VOID aec_nr_set_param(hal_ccni_message_t msg, hal_ccni_message_t *ack);

#ifdef PRELOADER_ENABLE
BOOL AEC_NR_Open  (VOID* para);
BOOL AEC_NR_Close (VOID* para);
#endif


#if defined(AIR_EC120_ENABLE)
bool EC_120_MemCheck (void);
bool EC_120_MemInit (void* para);
EXTERN bool stream_function_ec_120_initialize (void *para);
EXTERN bool stream_function_ec120_process (void *para);
#endif

#if defined(MTK_3RD_PARTY_NR)
#if defined(AIR_ECNR_PREV_PART_ENABLE) && defined(AIR_EC120_ENABLE)
uint8_t AEC_NR_Get_PostEC_Gain(void);
bool stream_function_ecnr_prev_initialize(void *para);
bool stream_function_ecnr_prev_process(void *para);
#endif
#endif

#ifdef AIR_ECNR_POST_PART_ENABLE
bool stream_function_ecnr_post_initialize(void *para);
bool stream_function_ecnr_post_process(void *para);
#endif

#endif /* _AEC_NR_INTERFACE_H_ */
