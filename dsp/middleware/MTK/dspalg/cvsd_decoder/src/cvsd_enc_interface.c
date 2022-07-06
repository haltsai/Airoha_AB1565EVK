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

#include "dsp_feature_interface.h"
#include "dsp_audio_process.h"
#include "cvsd_enc_interface.h"
#ifdef MTK_BT_A2DP_CVSD_USE_PIC
#include "cvsd_enc_portable.h"
#endif
#include "dsp_dump.h"

/**
 *
 * Function Prototype
 *
 */

bool stream_codec_encoder_cvsd_initialize(void *para)
{
    CVSD_ENC_STATE_PTR  cvsd_enc_pt = (CVSD_ENC_STATE_PTR)stream_codec_get_workingbuffer(para);
#ifndef MTK_BT_HFP_FORWARDER_ENABLE
    U16 sel_wnb = 0;    // 1(WB, 120 samples), 0(NB, 60 samples)
    _CVSD_Encoder_Init(cvsd_enc_pt,sel_wnb);
#endif
    memcpy((U8*)&cvsd_enc_pt->cvsd_updn_sample.NvKey, dsp_updown_sample_by_2_coef, sizeof(dsp_updown_sample_by_2_coef));
    updn_sampling_by2_Init(&cvsd_enc_pt->cvsd_updn_sample);
#ifndef MTK_BT_HFP_FORWARDER_ENABLE
	stream_codec_modify_output_size(para, 120);
#else
	stream_codec_modify_output_size(para, 240);
#endif

    return FALSE;
};

//volatile U16 bug_e1, bug_e2, bug_e3;
bool stream_codec_encoder_cvsd_process(void *para)
{
    S32* OutBuf = stream_codec_get_1st_output_buffer(para);
#ifndef MTK_BT_HFP_FORWARDER_ENABLE
    U8* InBuf = stream_codec_get_1st_input_buffer(para);
    U16* OutLength = stream_codec_get_output_size_pointer(para);
    U8 i,EncodeTimes;
#endif
    CVSD_ENC_STATE_PTR  cvsd_enc_pt = (CVSD_ENC_STATE_PTR)stream_codec_get_workingbuffer(para);

    /*
    if( (stream_codec_get_input_size(para) == 0) ||((stream_codec_get_input_size(para) %120) !=0))
    {
        *OutLength = 0;
        DSP_MW_LOG_I("CVSD Encode size mismatch, n = %d\r\n", 1, stream_codec_get_input_size(para));
        return FALSE;
    }
    EncodeTimes = (stream_codec_get_input_size(para) /120);
    */
    if (stream_codec_get_resolution(para) == RESOLUTION_32BIT)
    {
        dsp_converter_32bit_to_16bit(stream_codec_get_1st_input_buffer(para), (S32*)stream_codec_get_1st_input_buffer(para), stream_codec_get_input_size(para)/sizeof(U32));
        stream_codec_modify_input_size(para, stream_codec_get_input_size(para)/2);
    }

    if (stream_codec_get_input_samplingrate(para) == FS_RATE_16K)
    {
        //16-bit Resolution
        S16* InCodecBuf = stream_codec_get_1st_input_buffer(para);
        U16 InSampleNumber = stream_codec_get_input_size(para)/2;

        updn_sampling_by2_Proc(&cvsd_enc_pt->cvsd_updn_sample, InCodecBuf, cvsd_enc_pt->cvsd_updn_sample.buf, InSampleNumber, 0, true);
#ifndef MTK_BT_HFP_FORWARDER_ENABLE
        memcpy((U8*)InCodecBuf, cvsd_enc_pt->cvsd_updn_sample.buf, InSampleNumber);
        stream_codec_modify_input_size(para, stream_codec_get_input_size(para)/2);
#else
        memcpy((U8*)OutBuf, cvsd_enc_pt->cvsd_updn_sample.buf, InSampleNumber);
#endif
    }

#ifndef MTK_BT_HFP_FORWARDER_ENABLE
    EncodeTimes = 2;
    for (i = 0; i <stream_codec_get_input_size(para)/120; i++)
    {
        *OutLength = _CVSD_Encoder(cvsd_enc_pt, (U16 *)((U8*)InBuf+i*120), (S16 *)((U8*)OutBuf +i*60) ); // OutLength = 120 bytes(sel_wnb=1), 60 bytes(sel_wnb=0)
    }
    *OutLength = *OutLength*EncodeTimes;
    stream_codec_modify_input_size(para, cvsd_enc_pt->frm_len * sizeof(S16)* EncodeTimes );
#else
    stream_codec_modify_output_size(para, 240);
#endif

    return FALSE;
}

#ifdef PRELOADER_ENABLE
BOOL CVSD_Encoder_Open  (VOID* para)
{
    DSP_MW_LOG_I("[PIC] CVSD Encoder Open", 0);
    UNUSED(para);
    return TRUE;
}

BOOL CVSD_Encoder_Close (VOID* para)
{
    DSP_MW_LOG_I("[PIC] CVSD Encoder Close", 0);
    UNUSED(para);
    return TRUE;
}
#endif
