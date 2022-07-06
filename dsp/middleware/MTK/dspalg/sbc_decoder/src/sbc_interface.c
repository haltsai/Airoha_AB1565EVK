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

#include "sbc.h"
#include "sbc_interface.h"
#include "sbc_header_parse.h"
#include "dsp_feature_interface.h"
#include "dsp_audio_process.h"
#include "dsp_drv_dfe.h"
#include "dsp_dump.h"

#ifdef MTK_BT_A2DP_SBC_USE_PIC
#include "sbc_decoder_portable.h"
#endif


#define SBC_DEC_USE_INTERNAL_PATTERN    (FALSE)
#define SBC_SYNCWORD_SEARCH             (0)
#define SBC_DECODE_FRAME_SIZE (256)


/**
 *
 * Function Prototype
 *
 */
bool stream_codec_decoder_sbc_initialize (void *para);
bool stream_codec_decoder_sbc_process (void *para);
U32 SBC_GetInputFrameLength (VOID* HeaderPtr);
U32 SBC_GetOutputFrameLength (VOID* HeaderPtr);
SBC_FS_t SBC_GetSamplingFreq (VOID* HeaderPtr);
SBC_PARSE_STAT_t SBC_PacketParse(U8* PacketPtr, U32 PacketSize, U32* FrameNo, U16* FrameLength);
BOOL SBC_QUEUE_OUTPUT (VOID* para ,U16* queue_index , U16 sample_size);

DSP_ALIGN4 SBC_DEC_STATE sbc_dec_mem;

/**
 *
 *  Type Definition
 *
 */
typedef struct INHOUSE_SBC_DEC_CTRL_s
{
    U16 pre_SBC_fs;
    U16 pre_SBC_out_len;
} INHOUSE_SBC_DEC_CTRL_t;
STATIC INHOUSE_SBC_DEC_CTRL_t inHouseSbcDecCtrl;

/**
 * stream_codec_decoder_sbc_initialize
 *
 * This function is used to init memory space for sbc decoder
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_codec_decoder_sbc_initialize (void *para)
{
    U16 is_mSBC_frame;
    #if (SBC_DEC_USE_INTERNAL_PATTERN)
    U8* InBuf = sbc_pattern;
    #else
    U8* InBuf = stream_codec_get_1st_input_buffer(para);
    #endif
    SBC_DEC_STATE* sbc_dec_memPtr = (SBC_DEC_STATE*)stream_codec_get_workingbuffer(para);

    sbc_init(sbc_dec_memPtr);

    sbc_dec_memPtr->SBC_PACKET_BUF_PTR = InBuf;
    sbc_dec_memPtr->channelmode = 0; //[2018.10.31] add, 0 for L+R, 1 for L, 2 for R
    //printf("sbc init");
    if (0 != SBC_GetInputFrameLength(InBuf))
    {
        sbc_frame_header_decode(&is_mSBC_frame, sbc_dec_memPtr);

        stream_codec_modify_output_size(para, SBC_GetOutputFrameLength(InBuf)); // U16 size 
        //DSP_ModifyCodecOutStreamSamplingRate(para, SBC_GetSamplingRate(InBuf));
    }
    //printf("sbc init done");

    inHouseSbcDecCtrl.pre_SBC_fs = 0;
    inHouseSbcDecCtrl.pre_SBC_out_len = SBC_DECODE_FRAME_SIZE;

    return FALSE;
}


/**
 * SBC_Decoder
 *
 * This function decodes a SBC frame into dual output channels
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_codec_decoder_sbc_process (void *para)
{
    U16 SBC_status, is_mSBC_frame, pre_SBC_fs;
    U16 frameLength;
    #if (SBC_DEC_USE_INTERNAL_PATTERN)
    U8* InBuf = sbc_pattern;
    #else
    U8* InBuf = stream_codec_get_1st_input_buffer(para);
    #endif
    S32* OutBufL = stream_codec_get_1st_output_buffer(para);
    S32* OutBufR = stream_codec_get_2nd_output_buffer(para);

    U16 InLength = stream_codec_get_input_size(para);

    U16 OutLength;
    SBC_DEC_STATE* sbc_dec_memPtr = stream_codec_get_workingbuffer(para);
    //U8 OutSampleRate = stream_codec_get_output_samplerate(para);

    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
        LOG_AUDIO_DUMP((U8*)InBuf, (U32)InLength, AUDIO_CODEC_IN);
        LOG_AUDIO_DUMP((U8*)&InLength, (U32)2, AUDIO_CODEC_IN_LENGTH);
    #endif

    #if 1
    /*Workaround for sync-word mismatch*/


    sbc_dec_memPtr->SBC_PACKET_BUF_PTR = InBuf;
    pre_SBC_fs = sbc_dec_memPtr->SBC_FS;


    SBC_status = sbc_frame_header_decode(&is_mSBC_frame, sbc_dec_memPtr);
    frameLength = SBC_GetInputFrameLength(InBuf);
    OutLength = inHouseSbcDecCtrl.pre_SBC_out_len;
    stream_codec_modify_output_samplingrate(para, SBC_GetSamplingRate(InBuf));

    if (SBC_status == SBC_NO_ERROR)
    {
/*
        if (SBC_GetSamplingRate(InBuf) != OutSampleRate)
        {
            stream_codec_modify_output_samplingrate(para, SBC_GetSamplingRate(InBuf)/1000);
            stream_codec_modify_input_size(para, 0);
            stream_codec_modify_output_size(para, 0);
            stream_feature_reinitialize(para);
            DSP_MW_LOG_I("SBC Decode sample rate changed %d %d", 2 ,OutSampleRate,SBC_GetSamplingRate(InBuf)/1000);
            return TRUE;
        }
*/
        if ((InLength!=frameLength))
        {
            DSP_MW_LOG_I("SBC Decode in len mismatch %d %d", 2 ,InLength,frameLength);
        }

        SBC_status = sbc_frame_decode(sbc_dec_memPtr, OutBufL, OutBufR);
        if (inHouseSbcDecCtrl.pre_SBC_fs == 0)// First decode
        {
            inHouseSbcDecCtrl.pre_SBC_out_len = SBC_GetOutputFrameLength(InBuf);
            inHouseSbcDecCtrl.pre_SBC_fs = SBC_GetSamplingRate(InBuf);
            OutLength = inHouseSbcDecCtrl.pre_SBC_out_len;
            stream_codec_modify_output_samplingrate(para, inHouseSbcDecCtrl.pre_SBC_fs);
            DSP_MW_LOG_I("[SBC_DEBUG]SBC Decode first frame out_len:%d fs:%d decode status :%d", 3 ,inHouseSbcDecCtrl.pre_SBC_out_len,inHouseSbcDecCtrl.pre_SBC_fs,SBC_status);
        }
        else if ((inHouseSbcDecCtrl.pre_SBC_out_len != SBC_GetOutputFrameLength(InBuf))||(inHouseSbcDecCtrl.pre_SBC_fs != SBC_GetSamplingRate(InBuf)))
        {
            DSP_MW_LOG_I("[SBC_DEBUG] SBC Decode frame mismatch out_len:%d fs:%d", 2 ,SBC_GetOutputFrameLength(InBuf),SBC_GetSamplingRate(InBuf));
            DSP_MW_LOG_I("[SBC_DEBUG] %02x %02x %02x %02x", 4 ,InBuf[0], InBuf[1], InBuf[2], InBuf[3]);
            stream_codec_modify_output_samplingrate(para, inHouseSbcDecCtrl.pre_SBC_fs);
            memset(OutBufL, 0, OutLength);
            memset(OutBufR, 0, OutLength);
        }
        if (sbc_dec_memPtr->SBC_CHANNELS == 1)
        {
            memcpy(OutBufR, OutBufL, OutLength*sizeof(U16));
        }
    }
    else
    {
        /* Warning: Need error handling here */
        DSP_MW_LOG_I("SBC header decode failed %d", 1 ,SBC_status);
        memset(OutBufL, 0, OutLength);
        memset(OutBufR, 0, OutLength);
    }

    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)OutBufL, (U32)OutLength, AUDIO_SOURCE_IN_L);
    LOG_AUDIO_DUMP((U8*)OutBufR, (U32)OutLength, AUDIO_SOURCE_IN_R);
    #endif

    if (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        dsp_converter_16bit_to_32bit(OutBufL, (S16*)OutBufL, OutLength/sizeof(S16));
        dsp_converter_16bit_to_32bit(OutBufR, (S16*)OutBufR, OutLength/sizeof(S16));
        OutLength *= 2 ;
    }

    stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
    stream_codec_modify_output_size(para, OutLength);
    stream_codec_modify_output_samplingrate(para, SBC_GetSamplingRate(InBuf));
    return FALSE;
    #else
    // *InLength = 105*4;
    // *OutLength = 256*4;
    DRV_USB_CDC_ACM1_Send(InBuf, *InLength);
    #endif
}


/**
 * SBC_GetInputFrameLength
 *
 * This function returns the encoded frame length of input SBC frame
 *
 *
 * @HeaderPtr : Head pointer of a SBC frame
 *
 */
U32 SBC_GetInputFrameLength (VOID* HeaderPtr)
{
    return (U32)SBC_CalculateInFrameSize(HeaderPtr);
}


/**
 * SBC_GetOutputFrameLength
 *
 * This function returns the decoded frame length of input SBC frame
 *
 *
 * @HeaderPtr : Head pointer of a SBC frame
 *
 */
U32 SBC_GetOutputFrameLength (VOID* HeaderPtr)
{
    return (U32)SBC_CalculateOutFrameSize(HeaderPtr);
}


/**
 * SBC_GetSamplingFreq
 *
 * This function returns the decoded sampling frequency of input SBC frame
 *
 *
 * @HeaderPtr : Head pointer of a SBC frame
 *
 */
SBC_FS_t SBC_GetSamplingFreq (VOID* HeaderPtr)
{
    return SBC_CalculateSampleFrequency(HeaderPtr);
}


/**
 * SBC_GetSamplingRate
 *
 * This function returns the sampling rate which convert from SBC frequency
 *
 *
 * @HeaderPtr : Head pointer of a SBC frame
 *
 */
stream_samplerate_t SBC_GetSamplingRate (VOID* HeaderPtr)
{
    stream_samplerate_t samplingRate;
    switch(SBC_CalculateSampleFrequency(HeaderPtr))
    {
        case SBC_FS_16K:
            samplingRate = FS_RATE_16K;
            break;
        case SBC_FS_32K:
            samplingRate = FS_RATE_32K;
            break;
        case SBC_FS_44_1K:
            samplingRate = FS_RATE_44_1K;
            break;
        case SBC_FS_48K:
        default:
            samplingRate = FS_RATE_48K;
            break;
    }
    return samplingRate;
}

/**
 * SBC_QUEUE_OUTPUT
 *
 * This function queue the output data until gather given frame number
 *
 *
 *
 */
SBC_PARSE_STAT_t SBC_PacketParse(U8* PacketPtr, U32 PacketSize, U32* FrameNo, U16* FrameLength)
{
    U32 fNumber = 0;
    U32 fLength = 0;
    SBC_PARSE_STAT_t status;

    if ((fLength = SBC_GetInputFrameLength(PacketPtr)) != 0)
    {
        *FrameLength = fLength;
        fNumber++;

        while (PacketSize > fLength)
        {
            PacketSize -= fLength;
            PacketPtr += fLength;

            if ((fLength = SBC_GetInputFrameLength(PacketPtr)) != 0)
            {
                if (*FrameLength != fLength)
                {
                    /* Frame length changed, not reasonable in actual case */
                    status = SBC_FRAME_LENGTH_CHANGED;
                    break;
                }
                else
                {
                    fNumber++;
                }
            }
            else
            {
                status = SBC_PARSE_PARTIAL_SUCCESS;
                break;
            }
        }
    }
    else
    {
        status = SBC_PARSE_FAIL;
    }

    *FrameNo = fNumber;
    return SBC_PARSE_SUCCESS;
}

