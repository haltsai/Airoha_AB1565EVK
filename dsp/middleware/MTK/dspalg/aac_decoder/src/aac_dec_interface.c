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
#include "aac_dec_interface.h"
#include "dsp_feature_interface.h"
#include "dsp_utilities.h"
#include "dsp_buffer.h"
#include "dsp_dump.h"
#include "dsp_callback.h"
#include "dsp_memory.h"
#include "assert.h"

#ifdef MTK_BT_A2DP_AAC_USE_PIC
#include "aac_decoder_portable.h"
#endif

/**
 *
 *  Definition
 *
 */
#define AAC_IN_SIZE_WORKAROUND  (0)
#define AAC_DECODER_OUT_SAMPLES ((U16)1024)
#define AAC_USB_IN_FRAME_SIZE   ((U32)(512))
#define AAC_IN_CBUF_SIZE        ((U32)(4096))
#define AAC_OUT_CBUF_SIZE       ((U32)(8192))

#define AAC_DEC_VALID_MEMORY_CHECK_VALUE   ((U32)0x414143)

/**
 *
 *  Type Definition
 *
 */
typedef struct AAC_IN_BUF_s
{
    U16 wo;
    U16 ro;
    U16 count;
    BOOL enable;
    U16 threshold;
} AAC_IN_BUF_t;

typedef struct AAC_HEADER_STRUCT_s
{
    U32 Header1:    12;
    U32 Header2:    12;
} AAC_HEADER_STRUCT_t;


/**
 *
 *  Buffer & Control
 *
 */

AAC_DEC_INSTANCE_PTR AacDecMemoryPtr = NULL;


/**
 *
 * External Function Prototype
 *
 */
#ifndef MTK_BT_A2DP_AAC_USE_PIC
int get_AACdec_memsize(void);
int AIR_AAC_init(void *AAC_INSTANCE, uint8_t *input_buf, int aac_packet_len , int16_t *ch, uint32_t *srate, int16_t *brate);
int AIR_AAC_decoder(void *AAC_INSTANCE, uint8_t *input_buf, int aac_packet_len , int32_t *output_L, int32_t *output_R, int *decoded_len);
#endif
/**
 *
 * Function Prototype
 *
 */
BOOL AAC_Decoder_MemCheck (VOID);
BOOL AAC_Decoder_MemInit (VOID* para);
//bool stream_codec_decoder_aac_initialize (void *para);
//bool stream_codec_decoder_aac_process (void *para);
VOID DSP_AAC_DeinterleaveStream(U8* inBuf, U8* splitBuf, U32 inLen, VOID* para);


BOOL AAC_Decoder_MemCheck (VOID)
{
    if (NULL != AacDecMemoryPtr)
    {
        if (AAC_DEC_VALID_MEMORY_CHECK_VALUE == AacDecMemoryPtr->MemoryCheck)
        {
            return TRUE;
        }
    }
    return FALSE;
}


BOOL AAC_Decoder_MemInit (VOID* para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    U32 AacDecInstanceSize = 0;

    if (!AAC_Decoder_MemCheck())
    {
        AacDecInstanceSize = get_AACdec_memsize();
        AacDecMemoryPtr = (AAC_DEC_INSTANCE_PTR)DSPMEM_tmalloc(stream_function_get_task(para), DSP_AAC_DECODER_MEMSIZE+AacDecInstanceSize, stream_ptr);
        AacDecMemoryPtr->MemoryCheck = AAC_DEC_VALID_MEMORY_CHECK_VALUE;
        AacDecMemoryPtr->InitDone = 0;
        return FALSE;
    }
    return TRUE;
}


/**
 * stream_codec_decoder_aac_initialize
 *
 * This function is used to init memory space for AAC decoder
 *
 * @Author : Kylie <kylie-ky.Chen@airoha.com.tw>
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_codec_decoder_aac_initialize (void *para)
{
    AAC_Decoder_MemInit(para);

    return FALSE;
}


/**
 * stream_codec_decoder_aac_process
 *
 * This function decodes a AAC frame into dual output channels
 *
 * @Author : Kylie <kylie-ky.Chen@airoha.com.tw>
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_codec_decoder_aac_process (void *para)
{
    U8* InBuf = stream_codec_get_1st_input_buffer(para);
    U8* OutBufL = stream_codec_get_1st_output_buffer(para);
    U8* OutBufR = stream_codec_get_2nd_output_buffer(para);
    U16 OutLength;
    S32 DecOutLength;
    U16 InLength = stream_codec_get_input_size(para);
    //U8  CodecOutSR = stream_codec_get_output_samplerate(para);
    //U8* pCodecOutSR = stream_codec_get_output_samplerate_pointer(para);

    S16 Channel;
    U32 Fs,decode_result;
    S16 Bitrate;
    
    assert(OutBufR != NULL);
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP(InBuf, (InLength + 1) & (~1), AUDIO_CODEC_IN);
    LOG_AUDIO_DUMP((U8*)&InLength, (U32)2, AUDIO_CODEC_IN_LENGTH);
#endif

    if (stream_codec_get_output_resolution(para) == RESOLUTION_16BIT)
    {
        stream_codec_modify_output_size(para, AAC_DECODER_OUT_SAMPLES * sizeof(U16));
    }
    else if (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        stream_codec_modify_output_size(para, AAC_DECODER_OUT_SAMPLES * sizeof(U32));
    }
    stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));

    OutLength = stream_codec_get_output_size(para);

    if (InLength > 0)
    {
        if (AacDecMemoryPtr->InitDone == 0) {
            AacDecMemoryPtr->InitDone
                = AIR_AAC_init(&AacDecMemoryPtr->ScratchMemory[0], InBuf, InLength, &Channel, &Fs, &Bitrate);
        } 

        if (AacDecMemoryPtr->InitDone != 0) {
            
            decode_result = AIR_AAC_decoder(&AacDecMemoryPtr->ScratchMemory[0],
                            InBuf,
                            (S32)InLength,
                            (S32*)OutBufL,
                            (S32*)OutBufR,
                            &DecOutLength);
            if(decode_result != 0)
            {
                DSP_MW_LOG_I("AAC Decode failed id: %d", 1,decode_result);
                OutLength = stream_codec_get_output_size(para); 
            }
            else
            {
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                LOG_AUDIO_DUMP((U8*)OutBufL,
                                OutLength,
                                AUDIO_SOURCE_IN_L);

                LOG_AUDIO_DUMP((U8*)OutBufR,
                                OutLength,
                                AUDIO_SOURCE_IN_R);
#endif

                return FALSE;
            }
        }
    }

    memset(OutBufL, 0, OutLength);
    memset(OutBufR, 0, OutLength);

    return FALSE;
}


VOID DSP_AAC_DeinterleaveStream(U8* inBuf, U8* splitBuf, U32 inLen, VOID* para)
{
    U32 i;
    if (stream_codec_get_output_resolution(para) == RESOLUTION_16BIT)
    {
        for (i = 0 ; i < inLen/(2*sizeof(U32)) ; i++)
        {
            inBuf[2*i]          =   inBuf[8*i+2];
            inBuf[2*i+1]        =   inBuf[8*i+3];

            /* Should always have enough buffer size in splitBuf to do so */
            #if 1
            splitBuf[2*i]       =   inBuf[8*i+4+2];
            splitBuf[2*i+1]     =   inBuf[8*i+4+3];
            #else
            UNUSED(splitBuf);
            #endif
        }
    }
    else
    {
        for (i = 0 ; i < inLen/(2*sizeof(U32)) ; i++)
        {
            *(U32*)&(inBuf[4*i]) = *(U32*)&(inBuf[8*i]);
            /* Should always have enough buffer size in splitBuf to do so */
            #if 1
            *(U32*)&(splitBuf[4*i]) = *(U32*)&(inBuf[8*i+4]);
            #else
            UNUSED(splitBuf);
            #endif
        }
    }
}

#ifdef PRELOADER_ENABLE
BOOL AAC_Decoder_Open  (VOID* para)
{
	DSP_MW_LOG_I("[PIC] AAC Decoder Open", 0);
	UNUSED(para);
	return TRUE;
}

BOOL AAC_Decoder_Close (VOID* para)
{
	DSP_MW_LOG_I("[PIC] AAC Decoder Close", 0);
	UNUSED(para);
	return TRUE;
}
#endif
