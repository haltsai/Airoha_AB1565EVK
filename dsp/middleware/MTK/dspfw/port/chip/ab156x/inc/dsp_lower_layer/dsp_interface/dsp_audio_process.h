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

#ifndef _DSP_AUDIOPROCESS_H_
#define _DSP_AUDIOPROCESS_H_

#include "types.h"
#include "dsp_feature_interface.h"
#include "sfr_au_src.h"
#include "stream_audio_setting.h"
#include "dsp_callback.h"


/******************************************************************************
 * Constants
 ******************************************************************************/
#define DSP_SRC_MEMSIZE  (sizeof(DSP_SRC_FEATURE))


#ifdef MTK_HWSRC_IN_STREAM
#define DSP_CALLBACK_SRC_IN_FRAME    3
#else
#define DSP_CALLBACK_SRC_IN_FRAME    4
#endif

#ifdef MTK_HWSRC_IN_STREAM
#define DSP_CALLBACK_SRC_OUT_FRAME   3
#else
#define DSP_CALLBACK_SRC_OUT_FRAME   2
#endif

#ifdef MTK_HWSRC_IN_STREAM
#define DSP_CALLBACK_SRC_BUF_FRAME   0
#else
#define DSP_CALLBACK_SRC_BUF_FRAME   3
#endif


#define DSP_CALLBACK_SRC_OUT_SAMPLING_RATE  96


#define VAL_1_IN_Q15FORMAT           (0x7FFF)
#define DSP_FADE_FRAME_SIZE          (32)


/******************************************************************************
 * DSP Command Structure
 ******************************************************************************/
typedef struct DSP_SRC_Feature_s
{
    SRC_PTR_s VOLATILE  src_ptr;
    VOID*               inSRC_mem_ptr;
    VOID*               outSRC_mem_ptr;
    VOID*               buf_mem_ptr;

    U16                 inSRC_wo;
    U16                 inSRC_ro;
    U16                 inSRC_count;
    U16                 inSRC_extract_length;
    U16                 inSRC_mem_size;

    U16                 outSRC_wo;
    U16                 outSRC_ro;
    U16                 outSRC_count;
    U16                 outSRC_mem_size;

    U16                 buf_wo;
    U16                 buf_ro;
    U16                 buf_count;
    U16                 buf_mem_size;

    U8                  in_sampling_rate;
    U8                  channel_num;
    U16                 in_max_size;

    TaskHandle_t       task_id;
    int                InitDone;

    U8*                 mem_begin;
} DSP_SRC_FEATURE, *DSP_SRC_FEATURE_PTR;

/******************************************************************************
 * External Functions
 ******************************************************************************/
EXTERN VOID     DSP_Converter_16Bit_to_24bit(S32* des, S16* src, U32  sample);
EXTERN VOID     dsp_converter_16bit_to_32bit(S32* des, S16* src, U32  sample);
EXTERN VOID     dsp_converter_32bit_to_16bit(S16* des, S32* src, U32  sample);

EXTERN U32      dsp_count_bit(U32 value);

EXTERN VOID     DSP_Fade_Process(Audio_Fade_Ctrl_Ptr fade_ctrl_ptr, U8 *src_addr, U32 length);

EXTERN bool     stream_pcm_copy_process(void* para);

EXTERN BOOL     UART2AudioInit(VOID* para);
EXTERN BOOL     UART2AudioCodec(VOID* para);
EXTERN BOOL     UART2AudioCodec_16bit(VOID* para);

EXTERN bool     stream_function_src_initialize                (void *para);
EXTERN bool     stream_function_src_process             (void *para);
EXTERN VOID     DSP_Callback_SRC_Config     (DSP_STREAMING_PARA_PTR stream, stream_feature_type_ptr_t feature_type_ptr, U32 feature_entry_num);
EXTERN BOOL     DSP_Callback_SRC_Triger_Chk (DSP_CALLBACK_PTR callback_ptr);

EXTERN bool     stream_function_gain_initialize(void *para);
EXTERN bool     stream_function_gain_process(void *para);


#ifdef MTK_SPECIAL_FUNCTIONS_ENABLE
bool stream_function_fixed_gain_initialize(void *para);
bool stream_function_fixed_gain_process(void *para);
#endif

EXTERN bool     stream_function_size_converter_initialize(void *para);
EXTERN bool     stream_function_size_converter_process(void *para);
EXTERN VOID DSP_D2I_BufferCopy_32bit(U32* DestBuf,
                              U32* SrcBuf1,
                              U32* SrcBuf2,
                              U32  SampleSize);

EXTERN VOID DSP_D2I_BufferCopy_16bit(U16* DestBuf,
                              U16* SrcBuf1,
                              U16* SrcBuf2,
                              U16  SampleSize);

EXTERN VOID DSP_I2D_BufferCopy_16bit(U16* SrcBuf,
                              U16* DestBuf1,
                              U16* DestBuf2,
                              U16  SampleSize);
EXTERN VOID DSP_I2D_BufferCopy_16bit_mute(U16* SrcBuf,
                              U16* DestBuf1,
                              U16* DestBuf2,
                              U16  SampleSize,
                              BOOL muteflag);
EXTERN VOID DSP_I2D_BufferCopy_32bit(U32* SrcBuf,
                              U32* DestBuf1,
                              U32* DestBuf2,
                              U16  SampleSize);
EXTERN VOID DSP_I2D_BufferCopy_32bit_mute(U32* SrcBuf,
                                     U32* DestBuf1,
                                     U32* DestBuf2,
                                     U16  SampleSize,
                                     BOOL muteflag);




#endif /* _DSP_AUDIOPROCESS_H_ */

