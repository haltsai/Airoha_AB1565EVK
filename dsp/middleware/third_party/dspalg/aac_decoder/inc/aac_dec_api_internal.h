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

#ifndef __AAC_DEC_API_INTERNAL_H__
#define __AAC_DEC_API_INTERNAL_H__

#include "xa_aac_dec_api.h"

extern xa_codec_func_t *p_xa_process_api;
extern xa_codec_handle_t p_xa_process_api_obj;


#if 0
#define AAC_API_CALL(cmd, idx, pvalue, errstr) \
    err_code = (*p_xa_process_api)(p_xa_process_api_obj, (cmd), (idx), (pvalue));\
_XA_HANDLE_ERROR(p_proc_err_info, (errstr), err_code);
#else
#define AAC_API_CALL(cmd, idx, pvalue, errstr) \
    err_code = (*p_xa_process_api)(p_xa_process_api_obj, (cmd), (idx), (pvalue));
#endif

typedef struct _inp_buffer_t
{
     signed char* pb_inp_buf;
    unsigned int ui_inp_size;
    signed int  i_bytes_consumed, i_bytes_read;
    signed int  i_buff_size;
    signed int  i_inp_over;
} inp_buffer_t;


#define GET_CONFIG(idx, pvalue, errstr) AAC_API_CALL(XA_API_CMD_GET_CONFIG_PARAM,idx, pvalue, errstr)

void AAC_DECODER_INIT1(inp_buffer_t *inp_buffer,signed char  *AAC_STUCT ,signed char  *AAC_MEMTABS,\
signed char  *AAC_INPUT_BUF,signed char  *AAC_OUTPUT_BUF,signed char  *AAC_PERRAM,signed char  *AAC_SCRAM);

int AAC_DECODER_INIT2(inp_buffer_t *inp_buffer,int a,int i_value ,signed int  ui_value) ;
extern unsigned int FILL_AAC_DATA( inp_buffer_t *inp_buffer,int len_read);
int get_params(void *p_hdl, int * p_numChannels,  int *p_channelMode,int *p_sampleRate,int *p_sbrType,  int *p_bsfmt,int *p_data_rate, int *p_pcm_sample_size);


#ifndef XT_ALIGN8
#define XT_ALIGN8 __attribute__((aligned(16)))
#endif

typedef struct AAC_HANDLE
{
	XT_ALIGN8 signed char AAC_STUCT[176];
	XT_ALIGN8 signed char AAC_MEMTABS[144];
	XT_ALIGN8 signed char AAC_INPUT_BUF[8480];
	XT_ALIGN8 signed char AAC_OUTPUT_BUF[8192];
	XT_ALIGN8 signed char AAC_PERRAM[6392];
	XT_ALIGN8 signed char AAC_SCRAM[12392];
	inp_buffer_t inp_buffer;

	int acmod;
    int i_num_chan;
    int i_samp_freq;
    int i_data_rate;
    int err_code;
    int bsfmt;
    int sbrType;
    int pcm_sample_size;
    int i_out_bytes;
    int LengthRead;
    int InitDone;
    int ExecDone;
    int FrameworkReset;
}AAC_HAD, *AAC_HAD_PTR;

#endif /* __AAC_DEC_API_INTERNAL_H__ */


