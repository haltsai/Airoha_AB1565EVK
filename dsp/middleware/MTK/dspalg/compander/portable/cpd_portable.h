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
#ifndef  __CPD_PORTABLE_H__
#define  __CPD_PORTABLE_H__




#ifdef MTK_BT_A2DP_CPD_USE_PIC
#include "dsp_para_cpd.h"

#ifndef MTK_BT_HFP_SPE_ALG_V2

typedef int (*p_get_CPD_memsize)(int mode);
typedef int (*p_get_INS_memsize)(void);
typedef void (*p_compander_VO_init)(void *p_VO_STATE, void *p_VO_NVKEY, S32 mod);
typedef void (*p_compander_AU_init)(void *p_AU_STATE, void *p_AU_NVKEY, S32 recovery_gain, S32 band_sw);
typedef void (*p_compander_VO_proc)(void *p_VO_STATE, S16 *buf, S32 frame_size, S16 Rx_Vol, ECNR_OUT *PAR);
typedef void (*p_compander_AU_proc)(void * p_AU_STATE, S32 *buf_L, S32 *buf_R, S32 frame_size, S16 Vol, S16 channel);
typedef void (*p_INS_Init)(void *p_INS_STATE, void *p_INS_NVKEY);
typedef void (*p_INS_Prcs)(void *p_INS_STATE, S32 *buf_L, S32 *buf_R, S32 frame_size, S16 channel);


extern void *cpd_decoder_import_parameters[];


/*for export parameters*************************************************/
extern void *cpd_decoder_export_parameters[];

#define get_CPD_memsize ((p_get_CPD_memsize)cpd_decoder_export_parameters[0])
#define get_INS_memsize ((p_get_INS_memsize)cpd_decoder_export_parameters[1])
#define compander_VO_init ((p_compander_VO_init)cpd_decoder_export_parameters[2])
#define compander_AU_init ((p_compander_AU_init)cpd_decoder_export_parameters[3])
#define compander_VO_proc ((p_compander_VO_proc)cpd_decoder_export_parameters[4])
#define compander_AU_proc ((p_compander_AU_proc)cpd_decoder_export_parameters[5])
#define INS_Init ((p_INS_Init)cpd_decoder_export_parameters[6])
#define INS_Prcs ((p_INS_Prcs)cpd_decoder_export_parameters[7])

/***********************************************************************/


#else /*MTK_BT_HFP_SPE_ALG_V2*/

//get_CPD_memsize, get_INS_memsize, compander_VO_init, compander_AU_init, compander_VO_proc, compander_AU_proc, INS_Init, INS_Prcs

typedef int (*p_get_CPD_memsize)(int mode);
typedef int (*p_get_INS_memsize)(void);
typedef void (*p_compander_VO_init)(void *p_VO_STATE, void *p_VO_NVKEY, S32 mod);
typedef void (*p_compander_AU_init)(void *p_AU_STATE, void *p_AU_NVKEY, S32 recovery_gain, S32 band_sw);
typedef void (*p_compander_VO_proc)(void *p_VO_STATE, S16 *buf, S32 frame_size, S16 Rx_Vol, ECNR_OUT *PAR);
typedef void (*p_compander_AU_proc)(void *p_AU_STATE, S32 *buf_L, S32 *buf_R, S32 frame_size, S32 Vol, S16 channel);
typedef void (*p_INS_Init)(void *p_INS_STATE, void *p_INS_NVKEY);
typedef void (*p_INS_Prcs)(void *p_INS_STATE, S32 *buf_L, S32 *buf_R, S32 frame_size, S16 channel);
typedef void (*p_compander_AU_SetFrame120_init)(void *p_AU_STATE);
typedef void (*p_compander_AU_SetFrame8_init)(void *p_AU_STATE);
typedef void (*p_compander_AU_SetINS_proc)(void *p_AU_STATE, int INS_enable);
typedef int (*p_SVN_version)(void);

extern void *cpd_decoder_import_parameters[];

/*for export parameters*************************************************/
extern void *cpd_decoder_export_parameters[];

#define get_CPD_memsize   ((p_get_CPD_memsize)cpd_decoder_export_parameters[0])
#define get_INS_memsize   ((p_get_INS_memsize)cpd_decoder_export_parameters[1])
#define compander_VO_init ((p_compander_VO_init)cpd_decoder_export_parameters[2])
#define compander_AU_init ((p_compander_AU_init)cpd_decoder_export_parameters[3])
#define compander_VO_proc ((p_compander_VO_proc)cpd_decoder_export_parameters[4])
#define compander_AU_proc ((p_compander_AU_proc)cpd_decoder_export_parameters[5])
#define INS_Init          ((p_INS_Init)cpd_decoder_export_parameters[6])
#define INS_Prcs          ((p_INS_Prcs)cpd_decoder_export_parameters[7])
#define compander_AU_SetFrame120_init ((p_compander_AU_SetFrame120_init)cpd_decoder_export_parameters[8])
#define compander_AU_SetFrame8_init ((p_compander_AU_SetFrame8_init)cpd_decoder_export_parameters[9])
#define compander_AU_SetINS_proc ((p_compander_AU_SetINS_proc)cpd_decoder_export_parameters[10])
#define SVN_version ((p_SVN_version)cpd_decoder_export_parameters[11])

/***********************************************************************/


#endif /*MTK_BT_HFP_SPE_ALG_V2*/


#endif /*MTK_BT_A2DP_CPD_USE_PIC*/


#endif



