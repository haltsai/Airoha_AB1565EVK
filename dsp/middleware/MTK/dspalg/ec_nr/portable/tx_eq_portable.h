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

#ifndef  __TX_EQ_PORTABLE_H__
#define  __TX_EQ_PORTABLE_H__


#ifdef MTK_BT_A2DP_ECNR_USE_PIC
#include "dsp_para_wb_tx_eqv2.h"

typedef int  (*p_get_fir_wbeq_memsize)(void);
typedef void (*p_Voice_TxEQ_init)(void *handle, void *eq_nvkey);
typedef void (*p_Voice_TxEQ_Prcs)(S16 *buf, void *handle);
typedef void (*p_Voice_TxEQ_Prcs_Length)(S16 *buf, void *handle, int len);

extern void *tx_eq_import_parameters[];
extern void *tx_eq_export_parameters[];

//{get_fir_wbeq_memsize, Voice_TxEQ_init, Voice_TxEQ_Prcs, Voice_TxEQ_Prcs_Length}
#define get_fir_wbeq_memsize    ((p_get_fir_wbeq_memsize)tx_eq_export_parameters[0])
#define Voice_TxEQ_init         ((p_Voice_TxEQ_init)tx_eq_export_parameters[1])
#define Voice_TxEQ_Prcs         ((p_Voice_TxEQ_Prcs)tx_eq_export_parameters[2])
#define Voice_TxEQ_Prcs_Length  ((p_Voice_TxEQ_Prcs_Length)tx_eq_export_parameters[3])


#endif /*MTK_BT_A2DP_ECNR_USE_PIC && MTK_3RD_PARTY_NR*/
#endif /*__TX_EQ_PORTABLE_H__*/
