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

#include "config.h"
#include "dsp_task.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "dsp_utilities.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_stream_connect.h"
#include "transform_inter.h"
#include "stream_audio_driver.h"
#include "stream_audio_setting.h"
#include <string.h>


/******************************************************************************
 * Function Declaration
 ******************************************************************************/



/******************************************************************************
 * Variables
 ******************************************************************************/
SINK DSP_StreamJointSinkCheck(SINK_TYPE type, DSP_STREAMING_PARA_PTR  streamPtr)
{
    SINK sink = NULL;
    if (Sink_blks[type] != NULL)
    {
        if (Sink_blks[type]->param.dsp.transform != NULL)
        {
            if ((Sink_blks[type]->param.dsp.transform->sink == streamPtr->sink) &&
                (Sink_blks[type]->param.dsp.transform->source == streamPtr->source))
            {
                sink = Sink_blks[type];
            }
        }
    }
    return sink;
}

/**
 * DSP_StreamJointInit
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_StreamJointInit(VOID* para)
{
    SINK sink = NULL;
    SINK_TYPE sinkType;
    DSP_STREAMING_PARA_PTR  stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    #if 0
    if (Sink_blks[SINK_TYPE_DSP_JOINT] != NULL)
    {
        sink = Sink_blks[SINK_TYPE_DSP_JOINT];
        if ((sink->param.dsp.transform->sink == stream_ptr->sink) &&
            (sink->param.dsp.transform->source == stream_ptr->source))
        {
            ((DSP_STREAM_JOINT_FUNC_PTR_t)stream_function_get_working_buffer(para))->sink = sink;
        }
    }
    else
        ((DSP_STREAM_JOINT_FUNC_PTR_t)stream_function_get_working_buffer(para))->sink = NULL;
    #else

    for (sinkType = SINK_TYPE_DSP_JOINT ; sinkType<= SINK_TYPE_DSP_JOINT_MAX ; sinkType++)
    {
        if ((sink = DSP_StreamJointSinkCheck(sinkType, stream_ptr)) != NULL)
        {
            break;
        }
    }

    ((DSP_STREAM_JOINT_FUNC_PTR_t)stream_function_get_working_buffer(para))->type = (sinkType<=SINK_TYPE_DSP_JOINT_MAX)
                                                                                 ? sinkType
                                                                                 : SINK_TYPE_MAX;

    #endif
    return FALSE;
}


VOID DSP_Joint_MixOut(S32 *ptr1, S32 *ptr2, U16 length)
{
    U16 i;
    S32 VOLATILE sumValue;
    for (i=0 ; i<length ; i++)
    {
         sumValue = ptr1[i] + ptr2[i];
         ptr1[i] = sumValue;
    }
}

VOID DSP_Joint_MixOut_16Bit(S16 *ptr1, S16 *ptr2, U16 length)
{
    U16 i;
    S16 VOLATILE sumValue;
    for (i=0 ; i<length ; i++)
    {
         sumValue = ptr1[i] + ptr2[i];
         ptr1[i] = sumValue;
    }
}


/**
 * DSP_StreamJoint
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_StreamJoint(VOID* para)
{

    S32 *bufTemp;
    SINK_TYPE sinkType;
    SINK sink = NULL;
    TRANSFORM transform;
    DSP_CALLBACK_PTR callback_ptr;
    U8 sinkChannel;
    U8 streamChannel;
    U32 pre_offset;
    sinkType = ((DSP_STREAM_JOINT_FUNC_PTR_t)stream_function_get_working_buffer(para))->type;
    bufTemp = ((DSP_STREAM_JOINT_FUNC_PTR_t)stream_function_get_working_buffer(para))->buftemp;
    if (sinkType != SINK_TYPE_MAX)
    {
        sink = Sink_blks[sinkType];
    }

    if (sink == NULL)
    {
        DSP_StreamJointInit(para);
        return FALSE;
    }


    configASSERT(stream_function_get_channel_number(para) >= sink->param.audio.channel_num);
    streamChannel = 0;
    for (sinkChannel = 0 ; sinkChannel < sink->param.audio.channel_num ; sinkChannel++ )
    {
        DSP_C2D_BufferCopy(bufTemp,
                           sink->streamBuffer.BufferInfo.startaddr[sinkChannel]+sink->streamBuffer.BufferInfo.ReadOffset,
                           stream_function_get_output_size(para),
                           sink->streamBuffer.BufferInfo.startaddr[sinkChannel],
                           sink->streamBuffer.BufferInfo.length);

        #if 1
        if (Audio_setting->resolution.AudioOutRes == RESOLUTION_16BIT)
            DSP_Joint_MixOut_16Bit(stream_function_get_inout_buffer(para, streamChannel+1),(S16 *)bufTemp, stream_function_get_output_size(para)/2);
        else
            DSP_Joint_MixOut(stream_function_get_inout_buffer(para, streamChannel+1), bufTemp, stream_function_get_output_size(para)/4);
        #endif


        streamChannel++;
        streamChannel%=stream_function_get_channel_number(para);
    }

    pre_offset = sink->streamBuffer.BufferInfo.ReadOffset;
    sink->streamBuffer.BufferInfo.ReadOffset = (sink->streamBuffer.BufferInfo.ReadOffset + stream_function_get_output_size(para))%
                                               (sink->streamBuffer.BufferInfo.length);
    sink->streamBuffer.BufferInfo.bBufferIsFull = FALSE;


    if (OFFSET_OVERFLOW_CHK(pre_offset, sink->streamBuffer.BufferInfo.ReadOffset, sink->streamBuffer.BufferInfo.WriteOffset))
    {
        memset(bufTemp, 0, stream_function_get_output_size(para));

        for (sinkChannel = 0 ; sinkChannel < sink->param.audio.channel_num ; sinkChannel++ )
        {
            DSP_D2C_BufferCopy(sink->streamBuffer.BufferInfo.startaddr[sinkChannel]+sink->streamBuffer.BufferInfo.WriteOffset,
                               bufTemp,
                               stream_function_get_output_size(para),
                               sink->streamBuffer.BufferInfo.startaddr[sinkChannel],
                               sink->streamBuffer.BufferInfo.length);
        }
        sink->streamBuffer.BufferInfo.WriteOffset = (sink->streamBuffer.BufferInfo.WriteOffset + stream_function_get_output_size(para))%
                                                    (sink->streamBuffer.BufferInfo.length);
    }


    transform = sink->transform;
    if (transform != NULL)
    {
        /* Transform Handler */
        transform->Handler(transform->source, transform->sink);
        /* Resume Joint Streaming */
        callback_ptr = DSP_Callback_Get(transform->source, transform->sink);
        vTaskResume(callback_ptr->EntryPara.DSPTask);
    }
    return FALSE;
}

SOURCE DSP_StreamBranchSourceCheck(SOURCE_TYPE type, DSP_STREAMING_PARA_PTR  streamPtr)
{
    SOURCE source = NULL;
    if (Source_blks[type] != NULL)
    {
        if (Source_blks[type]->param.dsp.transform != NULL)
        {
            if ((Source_blks[type]->param.dsp.transform->sink == streamPtr->sink) &&
                (Source_blks[type]->param.dsp.transform->source == streamPtr->source))
            {
                source = Source_blks[type];
            }
        }
    }
    return source;
}


/**
 * DSP_StreamBranchInit
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_StreamBranchInit(VOID* para)
{
    SOURCE source;
    SOURCE_TYPE sourceType;
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    #if 0
    if (Source_blks[SOURCE_TYPE_DSP_BRANCH] != NULL)
    {
        source = Source_blks[SOURCE_TYPE_DSP_BRANCH];
        if ((source->param.dsp.transform->source == stream_ptr->source) &&
            (source->param.dsp.transform->sink == stream_ptr->sink))
        {
            ((DSP_STREAM_BRANCH_FUNC_PTR_t)stream_function_get_working_buffer(para))->source = source;
        }
    }
    else
        ((DSP_STREAM_BRANCH_FUNC_PTR_t)stream_function_get_working_buffer(para))->source = NULL;
    #else
    for (sourceType = SOURCE_TYPE_DSP_BRANCH ; sourceType <= SOURCE_TYPE_DSP_BRANCH_MAX ; sourceType++)
    {
        if ((source = DSP_StreamBranchSourceCheck(sourceType, stream_ptr)) != NULL)
        {
            break;
        }
    }

    ((DSP_STREAM_BRANCH_FUNC_PTR_t)stream_function_get_working_buffer(para))->type   = (sourceType<=SOURCE_TYPE_DSP_BRANCH_MAX)
                                                                                     ? sourceType
                                                                                     : SOURCE_TYPE_MAX;
    #endif
    return FALSE;
}


/**
 * DSP_StreamBranch
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_StreamBranch(VOID* para)
{
    U8 sourceChannel, streamChannel;
    SOURCE_TYPE sourceType;
    SOURCE source = NULL;
    TRANSFORM transform;
    DSP_CALLBACK_PTR callback_ptr;
    U32 pre_offset;

    sourceType = ((DSP_STREAM_BRANCH_FUNC_PTR_t)stream_function_get_working_buffer(para))->type;
    if (sourceType != SOURCE_TYPE_MAX)
    {
        source = Source_blks[sourceType];
    }

    if (source == NULL)
    {
        DSP_StreamBranchInit(para);
        return FALSE;
    }

    configASSERT(stream_function_get_channel_number(para) <= source->param.audio.channel_num);
    for (sourceChannel = 0, streamChannel = 0; sourceChannel<source->param.audio.channel_num ; sourceChannel++ )
    {
        DSP_D2C_BufferCopy(source->streamBuffer.BufferInfo.startaddr[sourceChannel]+source->streamBuffer.BufferInfo.WriteOffset,
                           stream_function_get_inout_buffer(para, streamChannel+1),
                           stream_function_get_output_size(para),
                           source->streamBuffer.BufferInfo.startaddr[sourceChannel],
                           source->streamBuffer.BufferInfo.length);
        streamChannel++;
        streamChannel%=stream_function_get_channel_number(para);
    }
    pre_offset = source->streamBuffer.BufferInfo.WriteOffset;
    source->streamBuffer.BufferInfo.WriteOffset = (source->streamBuffer.BufferInfo.WriteOffset + stream_function_get_output_size(para)/*buf_size*/)%
                                                  (source->streamBuffer.BufferInfo.length);


    if(source->streamBuffer.BufferInfo.bBufferIsFull == TRUE)
    {
        source->streamBuffer.BufferInfo.ReadOffset =(source->streamBuffer.BufferInfo.ReadOffset + source->param.audio.frame_size )
                                                    %(source->streamBuffer.BufferInfo.length);
    }
    else if (OFFSET_OVERFLOW_CHK(pre_offset, source->streamBuffer.BufferInfo.WriteOffset, source->streamBuffer.BufferInfo.ReadOffset))
    {
        source->streamBuffer.BufferInfo.bBufferIsFull = TRUE;
    }


    transform = source->transform;
    if (transform != NULL)
    {
        transform->Handler(transform->source, transform->sink);
        /* Resume Branch Streaming */
        callback_ptr = DSP_Callback_Get(transform->source, transform->sink);
        vTaskResume(callback_ptr->EntryPara.DSPTask);
    }
    return FALSE;
}


