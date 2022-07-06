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

#ifndef _CPD_INTERFACE_H_
#define _CPD_INTERFACE_H_

#include "dsp_feature_interface.h"
#include "dsp_para_cpd.h"
#include "dsp_utilities.h"
//#include "Dsp_Para_WB_TX_VO_CPD.h"
//#include "Dsp_Para_NB_TX_VO_CPD.h"
//#include "Dsp_Para_WB_RX_VO_CPD.h"
//#include "Dsp_Para_NB_RX_VO_CPD.h"
//#include "Dsp_Para_VP_CPD.h"
//#include "Dsp_Para_A2DP_AU_CPD.h"
//#include "Dsp_Para_LINE_AU_CPD.h"

#define DSP_DRC_AU_MEMSIZE  (sizeof(AU_CPD_INSTANCE_t))
#define DSP_DRC_VO_MEMSIZE  (sizeof(VO_CPD_INSTANCE_t))
#define CPD_TEST_DEFAULT_PARA       (0)
#define CPD_AU_MAX_USER_NUM         (2)

#if 0
typedef union stru_dsp_vo_cpd_para_u
{
	DSP_PARA_WB_TX_VO_CPD_STRU WbTx;
	DSP_PARA_NB_TX_VO_CPD_STRU NbTx;
	DSP_PARA_WB_RX_VO_CPD_STRU WbRx;
	DSP_PARA_NB_RX_VO_CPD_STRU NbRx;
	DSP_PARA_VP_CPD_STRU Vp;
} CPD_VO_NVKEY_STATE;

typedef union stru_dsp_au_cpd_para_u
{
	DSP_PARA_A2DP_AU_CPD_STRU A2DP;
	DSP_PARA_LINE_AU_CPD_STRU Line;
} CPD_AU_NVKEY_STATE;

typedef struct AU_CPD_INSTANCE_s
{
	DSP_ALIGN8 CPD_AU_NVKEY_STATE NvKey;
	DSP_ALIGN8 COMPANDER_AU_STATE StateL;
	DSP_ALIGN8 COMPANDER_AU_STATE StateH;
	DSP_ALIGN8 COMPANDER_AU_STATE StateF;
	DSP_ALIGN8 DBB_STATE Dbb;
} AU_CPD_INSTANCE_t;

typedef struct VO_CPD_INSTANCE_s
{
	DSP_ALIGN8 CPD_VO_NVKEY_STATE Nvkey;
	DSP_ALIGN8 COMPANDER_VO_STATE State;
} VO_CPD_INSTANCE_t;
#endif

typedef struct positive_gain_nvkey_s // note: used for positive digital gain, can be configured from "DRC makeup gain" or "positive gain"in config tool.
{
    U32 vo_rx_wb_positive_gain;
    U32 vo_rx_nb_positive_gain;
    U32 au_positive_gain;
    U32 linein_positive_gain;
} POSITIVE_GAIN_NVKEY_t;


typedef struct au_cpd_ctrl_s
{
    U8 enable;
    U8 enabled;
    U8 sample_rate;
    U8 trigger_drc_change;
    U8 phase_id;
    U8 force_disable;
    U8 sample_base;
    U8 audio_path;
    U16 peq2_nvkey_id;
	U16 peq2_nvkey_id_backup;
} AU_CPD_CTRL_t;



EXTERN bool stream_function_drc_audio_initialize (void *para);
EXTERN bool stream_function_drc_audio2_initialize (void *para);
EXTERN bool stream_function_drc_audio3_initialize (void *para);
EXTERN bool Audio_PureCPD_Init (void *para);
EXTERN bool stream_function_drc_audio_process (void *para);
EXTERN bool stream_function_drc_audio2_process (void *para);
EXTERN bool stream_function_drc_audio3_process (void *para);
EXTERN void Audio_CPD_Enable (U8 enable, U8 phase_id, U8 force_disable, U8 type, U16 peq_nvkey_id);
EXTERN bool stream_function_drc_voice_tx_wb_initialize (void *para);
EXTERN bool stream_function_drc_voice_tx_nb_initialize (void *para);
EXTERN bool stream_function_drc_voice_rx_wb_initialize (void *para);
EXTERN bool stream_function_drc_voice_rx_nb_initialize (void *para);
EXTERN bool Voice_VP_CPD_Init (void *para);
EXTERN bool stream_function_drc_voice_tx_process (void *para);
EXTERN bool stream_function_drc_voice_rx_process (void *para);
EXTERN bool Voice_RX_NB_CPD_Process (void *para);
EXTERN bool Voice_Prompt_CPD_Process (void *para);

//#ifdef MTK_BT_HFP_SPE_ALG_V2
EXTERN void stream_function_cpd_deinitialize (void);
//#endif

#ifdef PRELOADER_ENABLE
EXTERN bool Voice_CPD_Open  (void *para);
EXTERN bool Voice_CPD_Close (void *para);
EXTERN bool Audio_CPD_Open  (void *para);
EXTERN bool Audio_CPD_Close (void *para);
#endif

#endif /* _CPD_INTERFACE_H_ */
