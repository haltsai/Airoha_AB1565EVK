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
#ifndef _NVKEY_DSPALG_H_
#define _NVKEY_DSPALG_H_

#include "types.h"

typedef enum
{
    //NVKEYID_DSP_FW_PARA_DATADUMP    = 0xE001,
    NVKEY_DSP_PARA_DRC              = 0xE100,
    NVKEY_DSP_PARA_VO_CPD_BASE      = 0xE101,
    NVKEY_DSP_PARA_WB_TX_VO_CPD     = 0xE101,
    NVKEY_DSP_PARA_NB_TX_VO_CPD     = 0xE102,
    NVKEY_DSP_PARA_WB_RX_VO_CPD     = 0xE103,
    NVKEY_DSP_PARA_NB_RX_VO_CPD     = 0xE104,
    NVKEY_DSP_PARA_VP_CPD           = 0xE105,
    NVKEY_DSP_PARA_A2DP_AU_CPD      = 0xE106,
    NVKEY_DSP_PARA_LINE_AU_CPD      = 0xE107,
    NVKEY_DSP_PARA_POSITIVE_GAIN    = 0xE109,
    NVKEY_DSP_PARA_INS              = 0xE110,
    NVKEY_DSP_PARA_EFFECT           = 0xE120,
    NVKEY_DSP_PARA_VC               = 0xE130,
    NVKEY_DSP_PARA_PLC              = 0xE140,
    NVKEY_DSP_PARA_AEC_NR           = 0xE150,
    NVKEY_DSP_PARA_WB_RX_EQ         = 0xE161,
    NVKEY_DSP_PARA_WB_TX_EQ         = 0xE162,
    NVKEY_DSP_PARA_NB_RX_EQ         = 0xE163,
    NVKEY_DSP_PARA_NB_TX_EQ         = 0xE164,
    NVKEY_DSP_PARA_WB_RX_EQ_2ND     = 0xE165,
    NVKEY_DSP_PARA_NB_RX_EQ_2ND     = 0xE166,
    NVKEY_DSP_PARA_INEAR_EQ         = 0xE167,
    NVKEY_DSP_PARA_AST_EQ           = 0xE168,
    NVKEY_DSP_PARA_ANC_L_FILTER1    = 0xE180,
    NVKEY_DSP_PARA_ANC_L_FILTER2    = 0xE181,
    NVKEY_DSP_PARA_ANC_L_FILTER3    = 0xE182,
    NVKEY_DSP_PARA_ANC_L_FILTER4    = 0xE183,
    NVKEY_DSP_PARA_ANC_R_FILTER1    = 0xE184,
    NVKEY_DSP_PARA_ANC_R_FILTER2    = 0xE185,
    NVKEY_DSP_PARA_ANC_R_FILTER3    = 0xE186,
    NVKEY_DSP_PARA_ANC_R_FILTER4    = 0xE187,
    NVKEY_DSP_PARA_ANC_L_FILTER5    = 0xE188,
    NVKEY_DSP_PARA_ANC_R_FILTER5    = 0xE189,
    NVKEY_DSP_PARA_ANC_SW_GAIN      = 0xE18A,
    NVKEY_DSP_PARA_ANC              = 0xE18B,
    NVKEY_DSP_PARA_ANC_MISC_PARA    = 0xE18C,
    NVKEY_DSP_PARA_PASS_THRU        = 0xE18D,
    NVKEY_DSP_PARA_LEAKAGE_COMPENSATION    = 0xE18E,
    NVKEY_DSP_PARA_ADAPTIVE_FF_SZ_COEF     = 0xE18F,
    NVKEY_DSP_PARA_ADAPTIVE_FF      = 0xE190,
    NVKEY_DSP_PARA_ANC_L_FILTER6    = 0xE1B8,
    NVKEY_DSP_PARA_ANC_R_FILTER6    = 0xE1B9,
    NVKEY_DSP_PARA_ANC_L_FILTER7    = 0xE1BA,
    NVKEY_DSP_PARA_ANC_R_FILTER7    = 0xE1BB,
    NVKEY_DSP_PARA_PEQ_MISC_PARA    = 0xF232,
    NVKEY_DSP_PARA_PEQ              = 0xF233,
    NVKEY_DSP_PARA_PEQ_PATH_0       = 0xF234,
    NVKEY_DSP_PARA_PEQ_PATH_1       = 0xF235,
    NVKEY_DSP_PARA_PEQ_PATH_2       = 0xF236,
    NVKEY_DSP_PARA_PEQ_PATH_3       = 0xF237,
    NVKEY_DSP_PARA_PEQ_PATH_4       = 0xF238,
    NVKEY_DSP_PARA_PEQ_PATH_5       = 0xF239,
    NVKEY_DSP_PARA_PEQ_COEF_01      = 0xF260,
    NVKEY_DSP_PARA_PEQ_COEF_26      = 0xF279,
    NVKEY_DSP_PARA_PEQ_COEF_29      = 0xF27C,
    NVKEY_DSP_PARA_PEQ_COEF_32      = 0xF27F,
    NVKEY_PEQ_UI_DATA_01            = 0xEF00,
    NVKEY_PEQ_UI_DATA_04            = 0xEF03,
    NVKEY_DSP_PARA_VAD_COMMON       = 0xE172,
    NVKEY_DSP_PARA_VAD_1MIC_V_MODE  = 0xE175,
    NVKEY_DSP_PARA_VAD_1MIC_C_MODE  = 0xE176,
    NVKEY_DSP_PARA_VAD_2MIC_V_MODE  = 0xE177,
    NVKEY_DSP_PARA_VAD_2MIC_C_MODE  = 0xE178
} DSP_ALG_NVKEY_e;

typedef enum
{
    PEQ_AUDIO_PATH_A2DP = 0x0,
    PEQ_AUDIO_PATH_LINEIN,
} peq_audio_path_id_t;

typedef struct {
    uint16_t audioPathID;
    uint16_t nvkeyID;
} peq_audio_path_info_t;

typedef struct {
    uint16_t numOfAudioPath;
    peq_audio_path_info_t audioPathList[1];
} peq_audio_path_table_t;

typedef enum
{
    PEQ_PHASE_ID_0 = 0,
    PEQ_PHASE_ID_1,
} peq_phase_id_t;

typedef struct {
    uint16_t phaseID;
    uint16_t nvkeyID;
} peq_single_phase_t;

typedef struct {
    uint16_t numOfPhase;
    peq_single_phase_t phaseList[1];
} peq_single_set_t;

typedef struct {
    uint16_t numOfSet;
    peq_single_set_t setList[1];
} peq_full_set_t;

typedef enum
{
    PEQ_ELEMENT_ID_32K = 0x0,
    PEQ_ELEMENT_ID_441K,
    PEQ_ELEMENT_ID_48K,
} peq_element_id_t;

typedef struct {
    uint16_t elementID;
    uint16_t numOfParameter;
    uint16_t parameter[1];
} peq_element_t;

typedef struct {
    uint16_t numOfElement;
    uint16_t peqAlgorithmVer;
    peq_element_t elementList[1];
} peq_parameter_t;

#endif /* _NVKEY_DSPALG_H_ */
