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
#ifndef  __EC120_PORTABLE_H__
#define  __EC120_PORTABLE_H__

#ifdef AIR_EC120_ENABLE
#include "dsp_para_cpd.h"
#if defined(MTK_3RD_PARTY_NR) && (defined(AIR_IGO_NR_ENABLE) || defined(AIR_IGO_NR_PLUS_1_ENABLE))
#include "igo_nr.h"
#endif

/* function type define */
typedef int (*p_get_ec120_memsize)(void);
typedef void (*p_Voice_EC120_Init)(int NB_mode, void *p_ecnr_mem_ext, void *p_ecnr_NvKey, S16 *NR_gain);
typedef void (*p_Voice_EC120_Prcs)(S16* MIC1, S16* MIC2, S16* Ref ,S16* EC_out1, S16 *EC_out2);
typedef void (*p_Voice_PostEC120_Prcs)(S16* NR_out, S16 *out, ECNR_OUT *PAR);
typedef void (*p_EC120_REF_GAIN_READBACK)(S16 *gain);
typedef int (*p_get_EC120_SVN)(void);
typedef int (*p_get_ec120_inear_memsize)(void);
typedef void (*p_Voice_EC120_Inear_Init)(int NB_mode, void *p_ecnr_mem_ext, void *p_ecnr_NvKey, S16 *NR_gain);
typedef void (*p_Voice_EC120_Inear_Prcs)(S16* MIC1, S16* MIC2, S16* MICB, S16* REF ,S16* EC_out1, S16 *EC_out2, S16 *EC_outB);
typedef void (*p_EC120_Inear_REF_GAIN_READBACK)(S16 *gain);
typedef void (*p_EC120_PreLim_Coef_READBACK)(S16 *coef);
typedef S16 (*p_EC120_DaulMIC_power_Coef_READBACK)(void);
typedef U8 (*p_PostEC120_Info)(ECNR_OUT *PAR);


/*for import parameters*************************************************/
extern void *ec120_import_parameters[];

/***********************************************************************/

/*for export parameters*************************************************/
extern void *ec120_export_parameters[];

#define get_ec120_memsize          ((p_get_ec120_memsize)ec120_export_parameters[0])
#define Voice_EC120_Init           ((p_Voice_EC120_Init)ec120_export_parameters[1])
#define Voice_EC120_Prcs           ((p_Voice_EC120_Prcs)ec120_export_parameters[2])
#define Voice_PostEC120_Prcs       ((p_Voice_PostEC120_Prcs)ec120_export_parameters[3])
#define EC120_REF_GAIN_READBACK    ((p_EC120_REF_GAIN_READBACK)ec120_export_parameters[4])
#define get_EC120_SVN              ((p_get_EC120_SVN)ec120_export_parameters[5])
#define get_ec120_inear_memsize    ((p_get_ec120_inear_memsize)ec120_export_parameters[6])
#define Voice_EC120_Inear_Init     ((p_Voice_EC120_Inear_Init)ec120_export_parameters[7])
#define Voice_EC120_Inear_Prcs     ((p_Voice_EC120_Inear_Prcs)ec120_export_parameters[8])
#define EC120_Inear_REF_GAIN_READBACK ((p_EC120_Inear_REF_GAIN_READBACK)ec120_export_parameters[9])
#define EC120_PreLim_Coef_READBACK    ((p_EC120_PreLim_Coef_READBACK)ec120_export_parameters[10])
#define EC120_DaulMIC_power_Coef_READBACK ((p_EC120_DaulMIC_power_Coef_READBACK)ec120_export_parameters[11])
#define PostEC120_Info             ((p_PostEC120_Info)ec120_export_parameters[12])

/***********************************************************************/


#endif /*#ifdef AIR_EC120_ENABLE*/

#endif /*__EC120_PORTABLE_H__*/

