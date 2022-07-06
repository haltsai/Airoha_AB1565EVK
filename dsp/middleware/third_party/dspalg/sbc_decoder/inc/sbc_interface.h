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
 #ifndef _SBC_INTERFACE_H_
#define _SBC_INTERFACE_H_

// #include "Os.h"
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "sbc.h"
#include "dsp_feature_interface.h"
#include "sbc_dec_api.h"

typedef enum SBC_FS_e
{
	SBC_FS_16K                  = 0,
	SBC_FS_32K                  = 1,
	SBC_FS_44_1K                = 2,
	SBC_FS_48K                  = 3,
	SBC_FS_INVALID              = 4,
} SBC_FS_t;

typedef enum
{
    SBC_PARSE_SUCCESS,
    SBC_PARSE_PARTIAL_SUCCESS,
    SBC_PARSE_FAIL,
    SBC_FRAME_LENGTH_CHANGED,
} SBC_PARSE_STAT_t;

typedef struct SBC_BUFFER_CTRL_s
{
	U32     Wo;
	U32     Ro;
    U32     Count;
    U32     Threshold;
    U32     Enable;
} SBC_BUFFER_CTRL_t;

typedef struct AUDIO_CTRL_s
{
    SBC_BUFFER_CTRL_t InBufCtrl;
    U8 InBuf[10000];
    DSP_ALIGN4 S32 OutBufL[1024];
    DSP_ALIGN4 S32 OutBufR[1024];
} AUDIO_CTRL_t, * AUDIO_CTRL_PTR_t;

#define DSP_SBC_CODEC_MEMSIZE  (sizeof(SBC_DEC_MEM))

extern bool stream_codec_decoder_sbc_initialize (void *para);
extern bool stream_codec_decoder_sbc_process (void * para);
extern U32 SBC_GetInputFrameLength (VOID* HeaderPtr);
extern U32 SBC_GetOutputFrameLength (VOID* HeaderPtr);
extern stream_samplerate_t SBC_GetSamplingRate (VOID* HeaderPtr);
#ifdef PRELOADER_ENABLE
extern BOOL SBC_Decoder_Open  (VOID* para);
extern BOOL SBC_Decoder_Close (VOID* para);
#endif


#endif /* _SBC_INTERFACE_H_ */
