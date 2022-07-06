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
#ifndef _TX_EQ_INTERFACE_H_
#define _TX_EQ_INTERFACE_H_

#include "dsp_para_wb_tx_eqv2.h"
#include "dsp_para_nb_tx_eqv2.h"
#include "dsp_utilities.h"

/**
 *
 *  Definition
 *
 */
#define DSP_TX_EQ_SCRATCH_MEMSIZE   (1)
#define DSP_TX_EQ_INSTANCE_MEMSIZE  (sizeof(TX_EQ_INSTANCE))

/**
 *
 *  Type Definition
 *
 */
 typedef struct stru_dsp_tx_fir_eq_nvkey_u
{
    DSP_ALIGN4 DSP_PARA_WB_TX_FIR_EQ_STRU TxFirEq;
} TX_EQ_NVKEY_STATE;
 
 
typedef struct stru_dsp_tx_eq_para_u
{
    U32 MemoryCheck;
	DSP_ALIGN4 TX_EQ_NVKEY_STATE NvKey;
	DSP_ALIGN8 U8 ScratchMemory[DSP_TX_EQ_SCRATCH_MEMSIZE]; //Set at the end of structure for dynamic size
} TX_EQ_INSTANCE, *TX_EQ_INSTANCE_PTR;


/**
 *
 * Function Prototype
 *
 */
bool stream_function_tx_eq_initialize (void* para);
EXTERN void stream_function_tx_eq_deinitialize (void);
bool tx_eq_mem_check (void);
bool tx_eq_mem_init (void* para);
bool stream_function_tx_eq_process (void *para);


#endif /* _TX_EQ_INTERFACE_H_ */

