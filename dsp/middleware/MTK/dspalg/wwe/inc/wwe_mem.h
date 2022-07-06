/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#ifndef WWE_MEM_H
#define WWE_MEM_H

//#include "airoha_vt.h"
//#include "dsp_nvkey_vad_comm.h"
//#include "dsp_nvkey_vad_para.h"
//#include "xt_memory.h"
//#include "uart.h"

#define C_WWE_Version		0x19112400	//YYMMDDVV

#define C_RingBuf_Len	(16-2)	

#define C_MIC_1				1
#define C_MIC_2				2
#define C_VAD_MODE			1
#define C_CON_MODE			0

#define	C_CON			    0

#define C_init_environment   0
#define C_noisy_environment  1
#define C_silent_environment 2

#define false	0
#define true	1
//#define bool unsigned int

#if 0
typedef struct 
{
  air_vt_pp				my_vt_pp;
  DSP_NVKEY_VAD_COMM	nvkey_comm;
  DSP_NVKEY_VAD_PARA	nvkey_para; 
  short					DumpBuf[C_Dump_Len];
  short					HM_is_noisy;
  short					HM_silent_debcnt, HM_noisy_debcnt;  
} WWE_PP_MEM;

typedef struct 
{
  air_vt				my_vt;
  DSP_NVKEY_VAD_COMM	nvkey_comm;
  DSP_NVKEY_VAD_PARA	nvkey_para; 
  short					final_ans;  
} WWE_MEM;
#endif

int get_wwe_preprocessing_memsize(short mic_num);
int get_wwe_memsize(short mic_num);
unsigned int get_wwe_version(void);

int WWE_PreProcessing_Init(void *p_wwe_pp_mem_ext, void *p_comm_nvkey, void *p_nvkey, short mic_num);
int WWE_Init(void *p_wwe_mem_ext, void *p_wwe_dbase, void *p_comm_nvkey, void *p_nvkey, short mic_num);

int WWE_PreProcessing(short *mic_buf_1, short *mic_buf_2, short* echo_buf, int f_length, void *p_wwe_pp_mem_ext, bool sw_vad_en, short *pcm_out, short *is_noisy);
int WWE_Processing(short *mic_buf_1, short *mic_buf_2, short* echo_buf, int f_length, void *p_wwe_mem_ext, void *p_wwe_pp_mem_ext, short *kw_fr_no);
int WWE_Debug(void* p_wwe_pp_mem_ext , void *p_wwe_mem_ext, void **p_rd_dumpbuf);

#endif	/* WWE_MEM_H */
