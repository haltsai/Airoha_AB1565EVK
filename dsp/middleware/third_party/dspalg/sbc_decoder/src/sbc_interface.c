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



/**
 *
 *  Definition
 *
 */
#define SBC_USE_STATIC_MEMORY (FALSE)
#define SBC_DECODE_FRAME_SIZE (256)
#if defined(MTK_HWSRC_IN_STREAM) || defined(MTK_AUDIO_PLC_ENABLE)
#define SBC_CODECOUT_FRAME_NUM (1)
#else
#define SBC_CODECOUT_FRAME_NUM (4)
#endif

/**
 *
 *  Type Definition
 *
 */
typedef struct SBC_DEC_CTRL_s
{
    SBC_DEC_INP_BUF    *pbuf;
    SBC_DEC_MEM *pmem;
    U32 ui_init_done;
    U32 ui_exec_done;
    U32 error_code;
    S32 o_frame_size;
    S32 i_total_bytes;
    S32 frame_num;
    U16 pre_SBC_fs;
    U16 pre_SBC_samples;
    U16 SBC_decode_frame_index;
} SBC_DEC_CTRL_t;


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

/**
 *
 *  Buffer & Control
 *
 */
STATIC SBC_DEC_CTRL_t SbcDecCtrl;
#if (SBC_USE_STATIC_MEMORY)
SBC_DEC_MEM    sbc_dec_mem; // Memorys are not dynamic allocated yet
#endif
U32 sbc_in_buf_addr;

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
    U8* InBuf = stream_codec_get_1st_input_buffer(para);

    #if (SBC_USE_STATIC_MEMORY)
    SbcDecCtrl.pmem = &sbc_dec_mem;
    #else
    SbcDecCtrl.pmem = stream_codec_get_workingbuffer(para);
    #endif
    SbcDecCtrl.pbuf = &(SbcDecCtrl.pmem->inp_buf);
    SbcDecCtrl.ui_init_done = 0;
    SbcDecCtrl.error_code = SBC_Init1(SbcDecCtrl.pmem);
    SbcDecCtrl.pre_SBC_samples = SBC_DECODE_FRAME_SIZE;
    SbcDecCtrl.SBC_decode_frame_index = 0;

    if (SbcDecCtrl.error_code != 0)
    {
        /* Should not enter this */
        return TRUE;
    }
    stream_codec_modify_output_size(para, 0);
    if (stream_codec_get_input_size(para) != 0) {
        stream_codec_modify_output_samplingrate(para, SBC_GetSamplingRate(InBuf));
    }
    return FALSE;
}


/**
 * DSP_SBC_DeinterleaveStream
 *
 * This function splite a SBC frame into dual output channels
 *
 *
 *
 */

VOID DSP_SBC_DeinterleaveStream(U16* inBuf, U16* outBufL, U16* outBufR, U32 inLen)
{
    U32 i;

    for (i = 0 ; i < inLen/2 ; i++)
    {
        outBufL[i] = inBuf[2*i];
        outBufR[i] = inBuf[2*i+1];
    }
}


/**
 * stream_codec_decoder_sbc_process
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
    U16 frameLength;
    U8* InBuf = (U8*)stream_codec_get_1st_input_buffer(para);
    S32* OutBufL;
    S32* OutBufR;
    U16 InLength = stream_codec_get_input_size(para);
    U16 OutLength;
    U16 sample_size = (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT) ? 4 : 2;
    OutBufL = (S32*)((U8*)stream_codec_get_1st_output_buffer(para) + (SbcDecCtrl.SBC_decode_frame_index*SbcDecCtrl.pre_SBC_samples*sample_size)/2);
    OutBufR = (S32*)((U8*)stream_codec_get_2nd_output_buffer(para) + (SbcDecCtrl.SBC_decode_frame_index*SbcDecCtrl.pre_SBC_samples*sample_size)/2);
    OutLength = (U16)SBC_GetOutputFrameLength(InBuf); // size in bytes
    if (InLength == 0)
    {
        stream_codec_modify_output_size(para, 0);
        return FALSE;
    }
    if ((*InBuf != 0x9C)||(stream_codec_get_mute_flag(para) == TRUE)||((SbcDecCtrl.pre_SBC_samples != OutLength)&&(SbcDecCtrl.ui_init_done))) // for sync-word mismatch
    {
        //if (skipData == InLength) // can not find sync word
        {
            memset(OutBufL, 0, SBC_DECODE_FRAME_SIZE*sample_size/2);
            memset(OutBufR, 0, SBC_DECODE_FRAME_SIZE*sample_size/2);
            stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
            if(stream_codec_get_mute_flag(para) != TRUE) {
                DSP_MW_LOG_I("sync word error 0x%x or abnormal output size %d index %d", 3,*InBuf,OutLength,SbcDecCtrl.SBC_decode_frame_index);
            }
            SBC_QUEUE_OUTPUT(para,&(SbcDecCtrl.SBC_decode_frame_index),sample_size);
            return FALSE;
        }
        //InBuf += skipData;
    }
    frameLength = SBC_GetInputFrameLength(InBuf);

    while (!SbcDecCtrl.ui_init_done)
    {

        memcpy(&(SbcDecCtrl.pbuf)->pb_inp_buf[0], InBuf, InLength);

        (SbcDecCtrl.pbuf)->i_bytes_read = SBC_GetInputFrameLength(InBuf);

        SbcDecCtrl.error_code = SBC_Init2(SbcDecCtrl.pmem, &(SbcDecCtrl.ui_init_done));

        if (SbcDecCtrl.error_code != 0)
        {
            U16 temp_index = SbcDecCtrl.SBC_decode_frame_index;
            DSP_MW_LOG_E("SBC decode failed", 0);
            stream_codec_decoder_sbc_initialize(para);
            stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
            SbcDecCtrl.SBC_decode_frame_index = temp_index;
            SBC_QUEUE_OUTPUT(para,&(SbcDecCtrl.SBC_decode_frame_index),sample_size);
            return FALSE;
        }
        else
        {
            SbcDecCtrl.pre_SBC_fs = SBC_GetSamplingRate(InBuf);
        }

    }

    // SBC DECODER
    do
    {
        sbc_in_buf_addr = (U32)InBuf;
        memcpy(&(SbcDecCtrl.pbuf)->pb_inp_buf[0], InBuf, InLength);
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
        LOG_AUDIO_DUMP((U8*)InBuf, (U32)InLength, AUDIO_CODEC_IN);
        LOG_AUDIO_DUMP((U8*)&InLength, (U32)2, AUDIO_CODEC_IN_LENGTH);
#endif
        (SbcDecCtrl.pbuf)->i_bytes_read = SBC_GetInputFrameLength(InBuf);

        SbcDecCtrl.error_code = SBC_Decoder_Process(SbcDecCtrl.pmem, &(SbcDecCtrl.o_frame_size), &(SbcDecCtrl.ui_exec_done));

        if (SbcDecCtrl.error_code != 0)
        {
            OutLength = SbcDecCtrl.pre_SBC_samples;
            memset(OutBufL, 0, OutLength);
            memset(OutBufR, 0, OutLength);
            break;
        }
        else
        {
            if (SbcDecCtrl.pre_SBC_fs != SBC_GetSamplingRate(InBuf)) //decode ok but fs differs
            {
                SBC_QUEUE_OUTPUT(para,&(SbcDecCtrl.SBC_decode_frame_index),sample_size);
                stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
                stream_codec_modify_output_samplingrate(para, SBC_GetSamplingRate(InBuf));
                return FALSE;
            }

            if (2 == (SbcDecCtrl.pmem)->info.i_num_chan) //de-interleave output stream
            {
                DSP_SBC_DeinterleaveStream((U16*)(SbcDecCtrl.pmem)->inp_buf.pb_out_buf,
                                           (U16*)OutBufL,
                                           (U16*)OutBufR,
                                           OutLength*2);
            }
            else if (1 == (SbcDecCtrl.pmem)->info.i_num_chan)
            {
                memcpy(OutBufL, (SbcDecCtrl.pmem)->inp_buf.pb_out_buf, OutLength);
                memcpy(OutBufR, (SbcDecCtrl.pmem)->inp_buf.pb_out_buf, OutLength);
            }

            SbcDecCtrl.pre_SBC_fs = SBC_GetSamplingRate(InBuf);
            SbcDecCtrl.pre_SBC_samples = SBC_GetOutputFrameLength(InBuf);
        }

    } while(0);
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)OutBufL, (U32)OutLength, AUDIO_SOURCE_IN_L);
    LOG_AUDIO_DUMP((U8*)OutBufR, (U32)OutLength, AUDIO_SOURCE_IN_R);
#endif
    /*
    To do:
    We have to handle cases when output frame != 128 * 2 bytes,
    which should buffers output data and wait for data to be collected into desired size
    */

    #if 1
    if (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        dsp_converter_16bit_to_32bit(OutBufL, (S16*)OutBufL, OutLength/sizeof(S16));
        dsp_converter_16bit_to_32bit(OutBufR, (S16*)OutBufR, OutLength/sizeof(S16));
        OutLength *= 2 ;
    }

    stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
    #endif

    SBC_QUEUE_OUTPUT(para,&(SbcDecCtrl.SBC_decode_frame_index),sample_size);

    stream_codec_modify_output_samplingrate(para, SBC_GetSamplingRate(InBuf));

    return FALSE;
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
BOOL SBC_QUEUE_OUTPUT (VOID* para ,U16* queue_index , U16 sample_size)
{
    (*queue_index)++;
    if (*queue_index < SBC_CODECOUT_FRAME_NUM){
        stream_codec_modify_output_size(para, 0);
        return false;
    }
    else{
        stream_codec_modify_output_size(para, SBC_DECODE_FRAME_SIZE*SBC_CODECOUT_FRAME_NUM*sample_size/2);
        *queue_index = 0;
        return true;
    }
}


#ifdef PRELOADER_ENABLE
BOOL SBC_Decoder_Open  (VOID* para)
{
    DSP_MW_LOG_I("[PIC] SBC Decoder Open", 0);
    UNUSED(para);
    return TRUE;
}

BOOL SBC_Decoder_Close (VOID* para)
{
    DSP_MW_LOG_I("[PIC] SBC Decoder Close", 0);
    UNUSED(para);
    return TRUE;
}
#endif
