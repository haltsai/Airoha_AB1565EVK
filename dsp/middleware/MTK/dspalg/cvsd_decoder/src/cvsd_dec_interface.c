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
#include "cvsd_dec_interface.h"
#include "dtm.h"
#ifdef MTK_BT_A2DP_CVSD_USE_PIC
#include "cvsd_dec_portable.h"
#endif
#include "dsp_dump.h"

int16_t dsp_updown_sample_by_2_coef[16] =
{
    0x1FCF,
    0x2752,
    0x4306,
    0xCD63,
    0x2752,
    0x2752,
    0x08F8,
    0x2752,
    0x2839,
    0xA49F,
    0x00CE,
    0x2752,
    0x19C3,
    0x893E,
    0x2752,
    0,
};

/**
 *
 * Function Prototype
 *
 */

bool stream_codec_decoder_cvsd_initialize(void *para)
{
    U16 sel_wnb = 0;    // 1(WB, 120 samples), 0(NB, 60 samples)
    CVSD_DEC_STATE_PTR  cvsd_dec_pt = (CVSD_DEC_STATE_PTR)stream_codec_get_workingbuffer(para);
    DSP_MW_LOG_I("CVSD decode init\r\n", 0);
#ifndef MTK_BT_HFP_FORWARDER_ENABLE
    _CVSD_Decoder_Init(cvsd_dec_pt,sel_wnb);
#endif
    stream_codec_modify_output_size(para, 120 + sel_wnb*120);
    stream_codec_modify_output_samplingrate(para, FS_RATE_8K);

    memcpy((U8*)&cvsd_dec_pt->cvsd_updn_sample.NvKey, dsp_updown_sample_by_2_coef, sizeof(dsp_updown_sample_by_2_coef));
    updn_sampling_by2_Init(&cvsd_dec_pt->cvsd_updn_sample);
    return FALSE;
};

volatile U16 bug_d1, bug_d2, bug_d3;
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_codec_decoder_cvsd_process(void *para)
{
    U8* InBuf = stream_codec_get_1st_input_buffer(para);
    U8* OutBuf = stream_codec_get_1st_output_buffer(para);
    U16* pOutLength = stream_codec_get_output_size_pointer(para);
    stream_samplerate_t rate;
    U16 DecodeTimes = (stream_codec_get_input_size(para) /60);
    CVSD_DEC_STATE_PTR  cvsd_dec_pt = (CVSD_DEC_STATE_PTR)stream_codec_get_workingbuffer(para);

#ifndef MTK_BT_HFP_FORWARDER_ENABLE
    if( (stream_codec_get_input_size(para) == 0) ||((stream_codec_get_input_size(para) %60) !=0))
#else
    if( (stream_codec_get_input_size(para) == 0) ||((stream_codec_get_input_size(para) %120) !=0))
#endif
    {
        DSP_MW_LOG_I("CVSD decode size mismatch, n = %d\r\n", 1, stream_codec_get_input_size(para));
        *pOutLength = 0;
    return FALSE;
    }


#ifndef MTK_BT_HFP_FORWARDER_ENABLE
    U16 i;
    for (i = 0 ; i < DecodeTimes ; i++)
    {
        *pOutLength =_CVSD_Decoder(cvsd_dec_pt, (U16 *)((U8*)InBuf + 60*i), (S16 *)((U8*)OutBuf + 120*i));
    }
    *pOutLength = *pOutLength*i;
#else
    *pOutLength = 240; //15ms * 8khz* 2(byte/sample)
#endif

    rate = FS_RATE_8K;



#ifndef MTK_BT_HFP_FORWARDER_ENABLE
    updn_sampling_by2_Proc(&cvsd_dec_pt->cvsd_updn_sample, (S16 *)OutBuf, cvsd_dec_pt->cvsd_updn_sample.buf, *pOutLength/sizeof(S16), 1, true);
#else
    updn_sampling_by2_Proc(&cvsd_dec_pt->cvsd_updn_sample, (S16 *)InBuf, cvsd_dec_pt->cvsd_updn_sample.buf, *pOutLength/sizeof(S16), 1, true);
#endif
    memcpy((U8*)OutBuf, cvsd_dec_pt->cvsd_updn_sample.buf, *pOutLength*2);
    *pOutLength = *pOutLength*2;
    rate = FS_RATE_16K;


    if (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        dsp_converter_16bit_to_32bit((S32 *)OutBuf, (S16*)OutBuf, (*pOutLength*DecodeTimes)/sizeof(U16));
        *pOutLength *=2 ;
    }
    stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
    stream_codec_modify_output_size(para,*pOutLength); // OutLength = 240 bytes(sel_wnb=1), 120 bytes(sel_wnb=0)
    stream_codec_modify_output_samplingrate(para, rate);

    return FALSE;
}


#ifdef PRELOADER_ENABLE
bool CVSD_Decoder_Open  (void *para)
{
    DSP_MW_LOG_I("[PIC] CVSD Decoder Open", 0);
    UNUSED(para);
    return TRUE;
}

bool CVSD_Decoder_Close (void *para)
{
    DSP_MW_LOG_I("[PIC] CVSD Decoder Close", 0);
    UNUSED(para);
    return TRUE;
}
#endif


