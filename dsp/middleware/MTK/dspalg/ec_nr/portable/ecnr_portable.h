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
#ifndef  __ECNR_PORTABLE_H__
#define  __ECNR_PORTABLE_H__


#ifdef MTK_BT_A2DP_ECNR_USE_PIC
#include "dsp_para_aec_nr.h"
#include "dsp_para_cpd.h"
#if defined(MTK_3RD_PARTY_NR) && (defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE))
#include "igo_nr.h"
#endif

/* function type define */
      typedef int  (*p_get_ECNR_SVN)(void);

      typedef int (*p_get_aec_nr_memsize)(void);

      typedef void (*p_EC_PreLim_Coef_READBACK)(S16 *coef);
      typedef S16  (*p_DaulMIC_power_Coef_READBACK)(void);

#ifdef MTK_3RD_PARTY_NR
      typedef void (*p_Voice_EC_Init)(int NB_mode, void *p_ecnr_mem_ext, void *p_ecnr_NvKey, S16 *NR_gain);
      typedef void (*p_Voice_EC_Prcs)(S16* MIC1, S16* MIC2, S16* Ref ,S16* EC_out1, S16 *EC_out2);
      typedef void (*p_Voice_PostEC_Prcs)(S16* NR_out, S16 *out, ECNR_OUT *PAR);

      typedef int  (*p_get_aec_nr_inear_memsize)(void);
      typedef void (*p_Voice_EC_Inear_Init)(int NB_mode, void *p_ecnr_mem_ext, void *p_ecnr_NvKey, S16 *NR_gain);
      typedef void (*p_Voice_EC_Inear_Prcs)(S16* MIC1, S16* MIC2, S16* MICB, S16* REF ,S16* EC_out1, S16 *EC_out2, S16 *EC_outB);
      typedef void (*p_EC_Inear_REF_GAIN_READBACK)(S16 *gain);
#else
   #ifdef MTK_INEAR_ENHANCEMENT
      typedef void (*p_Voice_WB_TX_Inear_Init)(int NB_mode, S16 *bufin, void *p_ecnr_mem_ext, void *p_ecnr_NvKey, void *p_txeq_NvKey, void *p_InEarEQ_NvKey, void *p_AstEQ_NvKey);
      typedef void (*p_Voice_WB_TX_Inear_Prcs)(S16* MIC1, S16* MIC2, S16* REF, S16* NR, ECNR_OUT* PAR, S16 *g_f_wind, S16 *mic_status_o);
   #else /*#ifdef MTK_INEAR_ENHANCEMENT*/
      typedef void (*p_Voice_WB_TX_Init)(int NB_mode, S16 *bufin, void *p_ecnr_mem_ext, void *p_ecnr_NvKey, void *p_txeq_NvKey);
      typedef void (*p_Voice_WB_TX_Prcs)(S16* MIC1, S16* MIC2, S16* REF, S16* NR, ECNR_OUT* PAR);
   #endif /*#ifdef MTK_INEAR_ENHANCEMENT*/
#endif /*#ifndef MTK_3RD_PARTY_NR*/

      typedef void (*p_EC_REF_GAIN_READBACK)(S16 *gain);
      typedef void (*p_NB_RX_NR_init)(void *p_ecnr_mem_ext, void *p_ecnr_NvKey, void *p_rxeq_NvKey);
      typedef void (*p_Voice_NB_RX_Prcs)(S16* NR);
      typedef void (*p_WB_RX_NR_init)(void *p_ecnr_mem_ext, void *p_ecnr_NvKey, void *p_rxeq_NvKey);
      typedef void (*p_Voice_WB_RX_Prcs)(S16* NR);

#ifndef MTK_3RD_PARTY_NR
      typedef int  (*p_AEC_NR_Write_FLASH)(S16 *bufou);
      typedef void (*p_EQ_update)(void *p_ecnr_mem_ext, void *p_rxeq_NvKey);
      typedef int  (*p_get_txnr_pitch_last)(void);
      typedef int  (*p_TWO_MIC_WB_Write_FLASH)(S16*);
      typedef void (*p_IE_EC_REF_GAIN_READBACK)(S16 *gain);
#endif

#ifdef MTK_INEAR_ENHANCEMENT
      typedef void (*p_Get_AirDump_Inear)(S16 *out);
#endif

#ifdef MTK_DUALMIC_INEAR
      typedef void (*P_Voice_WB_TX_Inear_Init_V2)(int NB_mode, S16 *bufin, void *p_ecnr_mem_ext, void *p_ecnr_NvKey, void *p_txeq_NvKey, void *p_InEarEQ_NvKey, void *p_ast_eq_nvkey);
      typedef void (*p_Voice_WB_TX_Inear_Prcs_V2)(S16 *MIC1, S16 *MIC2, S16 *MICB, S16 *REF, S16 *NR, ECNR_OUT *PAR);
#endif


extern void *ecnr_decoder_import_parameters[];

/*for export parameters*************************************************/
extern void *ecnr_decoder_export_parameters[];

/* 
 * void * export_parameter_array[15] = {
 * get_aec_nr_memsize, Voice_WB_TX_Inear_Init, Voice_WB_TX_Inear_Prcs, EC_REF_GAIN_READBACK,
 * NB_RX_NR_init, Voice_NB_RX_Prcs, WB_RX_NR_init, Voice_WB_RX_Prcs, EC_PreLim_Coef_READBACK,
 * AEC_NR_Write_FLASH, EQ_update, get_txnr_pitch_last, DaulMIC_power_Coef_READBACK,
 * TWO_MIC_WB_Write_FLASH, IE_EC_REF_GAIN_READBACK}; *
 * //MTK_DUALMIC_INEAR
 * void * export_parameter_array[17] = {
 * get_aec_nr_memsize, Voice_WB_TX_Init, Voice_WB_TX_Prcs, EC_REF_GAIN_READBACK, NB_RX_NR_init,
 * Voice_NB_RX_Prcs, WB_RX_NR_init, Voice_WB_RX_Prcs, EC_PreLim_Coef_READBACK, AEC_NR_Write_FLASH, EQ_update, 
 * get_txnr_pitch_last, DaulMIC_power_Coef_READBACK, TWO_MIC_WB_Write_FLASH, IE_EC_REF_GAIN_READBACK,
 * Voice_WB_TX_Inear_Init_V2, Voice_WB_TX_Inear_Prcs_V2};
 */

#if defined(MTK_INEAR_ENHANCEMENT)

#define get_ECNR_SVN                ((p_get_ECNR_SVN)ecnr_decoder_export_parameters[0])
#define get_aec_nr_memsize          ((p_get_aec_nr_memsize)ecnr_decoder_export_parameters[1])
#define Voice_WB_TX_Inear_Init      ((p_Voice_WB_TX_Inear_Init)ecnr_decoder_export_parameters[2])
#define Voice_WB_TX_Inear_Prcs      ((p_Voice_WB_TX_Inear_Prcs)ecnr_decoder_export_parameters[3])
#define EC_REF_GAIN_READBACK        ((p_EC_REF_GAIN_READBACK)ecnr_decoder_export_parameters[4])
#define NB_RX_NR_init               ((p_NB_RX_NR_init)ecnr_decoder_export_parameters[5])
#define Voice_NB_RX_Prcs            ((p_Voice_NB_RX_Prcs)ecnr_decoder_export_parameters[6])
#define WB_RX_NR_init               ((p_WB_RX_NR_init)ecnr_decoder_export_parameters[7])
#define Voice_WB_RX_Prcs            ((p_Voice_WB_RX_Prcs)ecnr_decoder_export_parameters[8])
#define EC_PreLim_Coef_READBACK     ((p_EC_PreLim_Coef_READBACK)ecnr_decoder_export_parameters[9])
#define AEC_NR_Write_FLASH          ((p_AEC_NR_Write_FLASH)ecnr_decoder_export_parameters[10])
#define EQ_update                   ((p_EQ_update)ecnr_decoder_export_parameters[11])
#define get_txnr_pitch_last         ((p_get_txnr_pitch_last)ecnr_decoder_export_parameters[12])
#define DaulMIC_power_Coef_READBACK ((p_DaulMIC_power_Coef_READBACK)ecnr_decoder_export_parameters[13])
#define TWO_MIC_WB_Write_FLASH      ((p_TWO_MIC_WB_Write_FLASH)ecnr_decoder_export_parameters[14])
#define IE_EC_REF_GAIN_READBACK     ((p_IE_EC_REF_GAIN_READBACK)ecnr_decoder_export_parameters[15])
#define Get_AirDump_Inear           ((p_Get_AirDump_Inear)ecnr_decoder_export_parameters[16])

#elif defined(MTK_DUALMIC_INEAR)

#define get_aec_nr_memsize          ((p_get_aec_nr_memsize)ecnr_decoder_export_parameters[0])
#define Voice_WB_TX_Init            ((p_Voice_WB_TX_Init)ecnr_decoder_export_parameters[1])
#define Voice_WB_TX_Prcs            ((p_Voice_WB_TX_Prcs)ecnr_decoder_export_parameters[2])
#define EC_REF_GAIN_READBACK        ((p_EC_REF_GAIN_READBACK)ecnr_decoder_export_parameters[3])
#define NB_RX_NR_init               ((p_NB_RX_NR_init)ecnr_decoder_export_parameters[4])
#define Voice_NB_RX_Prcs            ((p_Voice_NB_RX_Prcs)ecnr_decoder_export_parameters[5])
#define WB_RX_NR_init               ((p_WB_RX_NR_init)ecnr_decoder_export_parameters[6])
#define Voice_WB_RX_Prcs            ((p_Voice_WB_RX_Prcs)ecnr_decoder_export_parameters[7])
#define EC_PreLim_Coef_READBACK     ((p_EC_PreLim_Coef_READBACK)ecnr_decoder_export_parameters[8])
#define AEC_NR_Write_FLASH          ((p_AEC_NR_Write_FLASH)ecnr_decoder_export_parameters[9])
#define EQ_update                   ((p_EQ_update)ecnr_decoder_export_parameters[10])
#define get_txnr_pitch_last         ((p_get_txnr_pitch_last)ecnr_decoder_export_parameters[11])
#define DaulMIC_power_Coef_READBACK ((p_DaulMIC_power_Coef_READBACK)ecnr_decoder_export_parameters[12])
#define TWO_MIC_WB_Write_FLASH      ((p_TWO_MIC_WB_Write_FLASH)ecnr_decoder_export_parameters[13])
#define IE_EC_REF_GAIN_READBACK     ((p_IE_EC_REF_GAIN_READBACK)ecnr_decoder_export_parameters[14])
#define Voice_WB_TX_Inear_Init_V2   ((P_Voice_WB_TX_Inear_Init_V2)ecnr_decoder_export_parameters[15])
#define Voice_WB_TX_Inear_Prcs_V2   ((p_Voice_WB_TX_Inear_Prcs_V2)ecnr_decoder_export_parameters[16])
#define get_ECNR_SVN                ((p_get_ECNR_SVN)ecnr_decoder_export_parameters[17])

#elif defined(MTK_3RD_PARTY_NR)

#define get_aec_nr_memsize          ((p_get_aec_nr_memsize)ecnr_decoder_export_parameters[0])
#define Voice_EC_Init               ((p_Voice_EC_Init)ecnr_decoder_export_parameters[1])
#define Voice_EC_Prcs               ((p_Voice_EC_Prcs)ecnr_decoder_export_parameters[2])
#define Voice_PostEC_Prcs           ((p_Voice_PostEC_Prcs)ecnr_decoder_export_parameters[3])
#define EC_REF_GAIN_READBACK        ((p_EC_REF_GAIN_READBACK)ecnr_decoder_export_parameters[4])
#define NB_RX_NR_init               ((p_NB_RX_NR_init)ecnr_decoder_export_parameters[5])
#define Voice_NB_RX_Prcs            ((p_Voice_NB_RX_Prcs)ecnr_decoder_export_parameters[6])
#define WB_RX_NR_init               ((p_WB_RX_NR_init)ecnr_decoder_export_parameters[7])
#define Voice_WB_RX_Prcs            ((p_Voice_WB_RX_Prcs)ecnr_decoder_export_parameters[8])
#define get_aec_nr_inear_memsize    ((p_get_aec_nr_inear_memsize)ecnr_decoder_export_parameters[9])
#define Voice_EC_Inear_Init         ((p_Voice_EC_Inear_Init)ecnr_decoder_export_parameters[10])
#define Voice_EC_Inear_Prcs         ((p_Voice_EC_Inear_Prcs)ecnr_decoder_export_parameters[11])
#define EC_Inear_REF_GAIN_READBACK  ((p_EC_Inear_REF_GAIN_READBACK)ecnr_decoder_export_parameters[12])
#define get_ECNR_SVN                ((p_get_ECNR_SVN)ecnr_decoder_export_parameters[13])
#define EC_PreLim_Coef_READBACK     ((p_EC_PreLim_Coef_READBACK)ecnr_decoder_export_parameters[14])
#define DaulMIC_power_Coef_READBACK ((p_DaulMIC_power_Coef_READBACK)ecnr_decoder_export_parameters[15])

#else

#define get_aec_nr_memsize          ((p_get_aec_nr_memsize)ecnr_decoder_export_parameters[0])
#define Voice_WB_TX_Init            ((p_Voice_WB_TX_Init)ecnr_decoder_export_parameters[1])
#define Voice_WB_TX_Prcs            ((p_Voice_WB_TX_Prcs)ecnr_decoder_export_parameters[2])
#define EC_REF_GAIN_READBACK        ((p_EC_REF_GAIN_READBACK)ecnr_decoder_export_parameters[3])
#define NB_RX_NR_init               ((p_NB_RX_NR_init)ecnr_decoder_export_parameters[4])
#define Voice_NB_RX_Prcs            ((p_Voice_NB_RX_Prcs)ecnr_decoder_export_parameters[5])
#define WB_RX_NR_init               ((p_WB_RX_NR_init)ecnr_decoder_export_parameters[6])
#define Voice_WB_RX_Prcs            ((p_Voice_WB_RX_Prcs)ecnr_decoder_export_parameters[7])
#define EC_PreLim_Coef_READBACK     ((p_EC_PreLim_Coef_READBACK)ecnr_decoder_export_parameters[8])
#define AEC_NR_Write_FLASH          ((p_AEC_NR_Write_FLASH)ecnr_decoder_export_parameters[9])
#define EQ_update                   ((p_EQ_update)ecnr_decoder_export_parameters[10])
#define get_txnr_pitch_last         ((p_get_txnr_pitch_last)ecnr_decoder_export_parameters[11])
#define DaulMIC_power_Coef_READBACK ((p_DaulMIC_power_Coef_READBACK)ecnr_decoder_export_parameters[12])
#define get_ECNR_SVN                ((p_get_ECNR_SVN)ecnr_decoder_export_parameters[13])

#endif

#ifdef AIR_ECNR_POST_PART_ENABLE

typedef int (*p_get_post_ec_memsize)(void);
typedef void (*p_EXT_POST_EC_Init)(void *pec_handle, void *p_ecnr_NvKey);
typedef void (*p_EXT_POST_EC_PRCS)(void *pec_handle, S16* NR_out, S16 *out, U8 PEC_GAIN, ECNR_OUT *PAR);

#define get_post_ec_memsize         ((p_get_post_ec_memsize)ecnr_decoder_export_parameters[0])
#define EXT_POST_EC_Init            ((p_EXT_POST_EC_Init)ecnr_decoder_export_parameters[1])
#define EXT_POST_EC_PRCS            ((p_EXT_POST_EC_PRCS)ecnr_decoder_export_parameters[2])

#endif

/***********************************************************************/


#endif /*#ifdef MTK_BT_A2DP_ECNR_USE_PIC*/

#endif /*__ECNR_PORTABLE_H__*/

