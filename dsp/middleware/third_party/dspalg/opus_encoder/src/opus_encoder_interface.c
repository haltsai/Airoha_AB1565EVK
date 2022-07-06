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
#include "opus_encoder.h"
#include "opus_encoder_interface.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "dsp_callback.h"
#include "dsp_dump.h"



/**
 *
 *  Definition
 *
 */
#define OPUS_MEMORY_CHECK_VALUE     ((U32)0x03612880)
#define OPUS_ENC_IN_FRAME_NO        (3)
#define OPUS_ENC_IN_FRAME_SIZE      (640)
#define OPUS_ENC_OUT_FRAME_NO       (5)
#define OPUS_ENC_OUT_MAX_FRAME_SIZE (168)


/**
 *
 *  Type Definition
 *
 */
typedef struct stru_dsp_opus_enc_interface_u
{
    U32 MemoryCheck;
    U32 Bitrate;
    DSP_CBUF_CTRL_t InBufCtrl;
    DSP_CBUF_CTRL_t OutBufCtrl;
    U8 InCircularBuffer[OPUS_ENC_IN_FRAME_SIZE*OPUS_ENC_IN_FRAME_NO];
    //U8 OutCircularBuffer[OPUS_ENC_OUT_MAX_FRAME_SIZE*OPUS_ENC_OUT_FRAME_NO];
    S16 InBuffer[OPUS_ENC_IN_FRAME_SIZE/sizeof(S16)];
    //U8 OutBuffer[OPUS_ENC_OUT_MAX_FRAME_SIZE/sizeof(U8)];
} OPUS_ENC_INTERFACE_t;

typedef struct stru_dsp_opus_enc_para_u
{
    OPUS_ENC_INTERFACE_t Interface;
    DSP_ALIGN8 U8 ScratchMemory[1];
} OPUS_ENC_INSTANCE, *OPUS_ENC_INSTANCE_PTR;


/**
 *
 *  Buffer & Control
 *
 */
STATIC OPUS_ENC_INSTANCE_PTR OpusEncMemoryPtr = NULL;


/**
 *
 * External Symbols
 *
 */


/**
 *
 * Function Prototype
 *
 */
bool OPUS_Encoder_Init (VOID* para);
U32 OPUS_Encoder_GetCodecOutSizeByBitrate (DSP_ENC_BITRATE_t Bitrate);
VOID OPUS_Encoder_BufInit (VOID);
bool OPUS_ENC_MemCheck (VOID);
bool OPUS_ENC_MemInit (VOID* para);
bool OPUS_Encoder (VOID* para);


/**
 * OPUS_Encoder_Init
 *
 * This function is used to init memory space for OPUS Encoder process
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>, LeoBai<Leo.Bai@airoha.com.tw>
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool OPUS_Encoder_Init (VOID* para)
{
    //DSP_CBUF_CTRL_t** pOutBufCtrlPtr = (DSP_CBUF_CTRL_t**)DSP_GetEncoderBufCtrlPtr(para);

    OPUS_ENC_MemInit(para);

    #ifdef MTK_RECORD_OPUS_ENABLE
    opus_encoder_init(&OpusEncMemoryPtr->ScratchMemory[0]);
    #endif

    OPUS_Encoder_BufInit();

    //*pOutBufCtrlPtr = &OpusEncMemoryPtr->Interface.OutBufCtrl;

    OpusEncMemoryPtr->Interface.Bitrate = ENCODER_BITRATE_32KBPS;
    DSP_MW_LOG_I("OPUS INIT DONE", 0);
    return FALSE;
}


VOID OPUS_Encoder_BufInit (VOID)
{
    if (OPUS_ENC_MemCheck())
    {
        DSP_CBUF_CtrlInit(&OpusEncMemoryPtr->Interface.InBufCtrl,
                          &OpusEncMemoryPtr->Interface.InCircularBuffer[0],
                          (U32)sizeof(OpusEncMemoryPtr->Interface.InCircularBuffer));

        //DSP_CBUF_CtrlInit(&OpusEncMemoryPtr->Interface.OutBufCtrl,
        //                  &OpusEncMemoryPtr->Interface.OutCircularBuffer[0],
        //                 (U32)sizeof(OpusEncMemoryPtr->Interface.OutCircularBuffer));
        DSP_MW_LOG_I("OPUS BUF INIT DONE", 0);
    }
}


U32 OPUS_Encoder_GetCodecOutSizeByBitrate (DSP_ENC_BITRATE_t Bitrate)
{
    switch (Bitrate)
    {
        case ENCODER_BITRATE_64KBPS:
            return 160;

        case ENCODER_BITRATE_32KBPS:
            return 80;

        case ENCODER_BITRATE_16KBPS:
            return 40;

        default:
            return 0;
    }
}


/**
 * OPUS_ENC_MemCheck
 *
 * This function is used to check init memory space for OPUS ENC process
 *
 *
 * @para : Default parameter of callback function
 * @return : Check result
 */
bool OPUS_ENC_MemCheck (VOID)
{
    if (NULL != OpusEncMemoryPtr)
    {
        if (OPUS_MEMORY_CHECK_VALUE == OpusEncMemoryPtr->Interface.MemoryCheck)
        {
            return TRUE;
        }
    }
    return FALSE;
}


/**
 * OPUS_ENC_MemInit
 *
 * This function is used to init memory space for OPUS Encoder process
 *
 *
 * @para : Default parameter of callback function
 * @return : Initialize result
 */
bool OPUS_ENC_MemInit (VOID* para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    U32 InstanceSize = 0;

    if (!OPUS_ENC_MemCheck())
    {
        #ifdef MTK_RECORD_OPUS_ENABLE
        InstanceSize = opus_encoder_get_size();
        DSP_MW_LOG_I("OPUS GET MEM SIZE %d", 1,InstanceSize);
        #endif

        OpusEncMemoryPtr = (OPUS_ENC_INSTANCE_PTR)DSPMEM_tmalloc(stream_function_get_task(para), sizeof(OPUS_ENC_INTERFACE_t)+InstanceSize, stream_ptr);
        OpusEncMemoryPtr->Interface.MemoryCheck = OPUS_MEMORY_CHECK_VALUE;
        DSP_MW_LOG_I("OPUS MEM INIT DONE", 0);
    }

    return TRUE;
}


/**
 * OPUS_Encoder
 *
 * The main process for OPUS Encoder
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool OPUS_Encoder (VOID* para)
{
    S16* InBuf = stream_codec_get_1st_input_buffer(para);//DSP_GetCodecInStream1Ptr(para);
    U8* OutBuf = stream_codec_get_1st_output_buffer(para);//DSP_GetCodecOutStream1Ptr(para);
    U16 InLength = stream_codec_get_input_size(para);//(U32)DSP_GetCodecInStreamSize(para);
    //U8* CoderOutBuf = stream_codec_get_1st_output_buffer(para);

    U32 EncBitrate = stream_codec_get_encoder_bitrate(para);
    //if (!DSP_MatchIcVersion(1536))
    //{
    //     return FALSE;
    //}

    if ((EncBitrate != 0) && (EncBitrate != OpusEncMemoryPtr->Interface.Bitrate))
    {
        OpusEncMemoryPtr->Interface.Bitrate = EncBitrate;
        DSP_MW_LOG_I("OPUS change bitrate %d", 1,OpusEncMemoryPtr->Interface.Bitrate);
    }

    U32 OutLength = OPUS_Encoder_GetCodecOutSizeByBitrate(OpusEncMemoryPtr->Interface.Bitrate);

    if (!OPUS_ENC_MemCheck())
    {
        stream_feature_reinitialize(para);//DSP_SetFeatureDeinit(para);
        return FALSE;
    }
    if (DSP_CBUF_GetBufRemainSize(&OpusEncMemoryPtr->Interface.InBufCtrl) >= InLength)
    {
        DSP_CBUF_DataPush((U8*)InBuf, InLength, &OpusEncMemoryPtr->Interface.InBufCtrl);
    }

    if (DSP_CBUF_GetBufCount(&OpusEncMemoryPtr->Interface.InBufCtrl) >= OPUS_ENC_IN_FRAME_SIZE)
    {
        InBuf = OpusEncMemoryPtr->Interface.InBuffer;
        //OutBuf = OpusEncMemoryPtr->Interface.OutBuffer;

        DSP_CBUF_DataPop((U8*)InBuf, OPUS_ENC_IN_FRAME_SIZE, &OpusEncMemoryPtr->Interface.InBufCtrl);

        //DSP_DataDump_DataIn((U8*)InBuf,
        //                        OPUS_ENC_IN_FRAME_SIZE,
        //                        AUDIO_CODEC_IN);

        //LOG_AUDIO_DUMP((U8*)InBuf, (U32)OPUS_ENC_IN_FRAME_SIZE, AUDIO_SOURCE_IN_L);

        #ifdef MTK_RECORD_OPUS_ENABLE
        opus_encoder_process(&OpusEncMemoryPtr->ScratchMemory[0],
                            OutBuf,
                            InBuf,
                            OpusEncMemoryPtr->Interface.Bitrate);
        #endif

        //DSP_DataDump_DataIn((U8*)OutBuf,
        //                    OutLength,
        //                    AUDIO_SOURCE_IN_L);

        //if (DSP_CBUF_GetBufRemainSize(&OpusEncMemoryPtr->Interface.OutBufCtrl) < OutLength)
        //{
        //   DSP_CBUF_DataDrop(OutLength, &OpusEncMemoryPtr->Interface.OutBufCtrl);
        //}

        //DSP_CBUF_DataPush(OutBuf, OutLength, &OpusEncMemoryPtr->Interface.OutBufCtrl);

        //LOG_AUDIO_DUMP((U8*)OutBuf, (U32)OutLength, AUDIO_CODEC_IN);

        //memcpy(CoderOutBuf,OutBuf,OutLength);
        stream_codec_modify_output_size(para,OutLength);//DSP_ModifyCodecOutStreamSize(para, 1);

        return FALSE;
    }

    stream_codec_modify_output_size(para, 0);

    return FALSE;
}


