/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#ifndef AUDIO_PLC_H
#define AUDIO_PLC_H


#ifdef __cplusplus
extern "C" {
#endif
#include "dsp_para_plc_nvstruc.h"

#if 0
int get_plc_memsize(void);
//int PLC_Init(void *p_plc_mem_ext, int plc_option, int framesize, int loss_count);
int PLC_Init(void *p_plc_mem_ext, DSP_PARA_PLC_STRU *plc_nvkey);
int PLC_Proc(void *p_plc_mem_ext, int *Buf_L, int *Buf_R, int BFI);
int PLC_Proc_128(void *p_plc_mem_ext, int *Buf_L, int *Buf_R, int BFI);
int get_plc_loss_count(void *p_plc_mem_ext);
#endif
#if 0
int get_plc_memsize(int channels);
int get_plc_loss_count(void *p_plc_mem_ext);
int PLC_Init(void *p_plc_mem_ext, DSP_PARA_PLC_STRU *plc_nvkey);
int PLC_Proc_512(void *p_plc_mem_ext, int *Buf_L, int *Buf_R, int BFI);
int PLC_Proc_128(void *p_plc_mem_ext, int *Buf_L, int *Buf_R, int BFI);

void Mem_Cir2Lin(int *lin_mem, short DECODE_BUFFER_SIZE, short overlap, short cirpt);
#endif
#if 1

int get_plc_memsize(int channels, int max_period);
int get_plc_loss_count(void *p_plc_mem_ext);
int PLC_Init(void *p_plc_mem_ext, DSP_PARA_AUDIO_PLC_STRU *plc_nvkey);
int PLC_Proc_512(void *p_plc_mem_ext, int *Buf_L, int *Buf_R, int BFI);
int PLC_Proc_256(void *p_plc_mem_ext, int *Buf_L, int *Buf_R, int BFI);
int PLC_Proc_128(void *p_plc_mem_ext, int *Buf_L, int *Buf_R, int BFI);

void Mem_Cir2Lin(int *lin_mem, short DECODE_BUFFER_SIZE, short overlap, short cirpt);

#endif

#ifdef __cplusplus
}
#endif

#endif /* AUDIO_PLC_H */
