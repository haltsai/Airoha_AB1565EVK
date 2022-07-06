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

/*!
 *@file   transform.c
 *@brief  define api of transform data beteen source and sink
 *
 @verbatim
 @endverbatim
 */

//-
#include "sink_inter.h"
#include "source_inter.h"
#include "transform_inter.h"
#include "dlist.h"

//- interface
#include "stream_audio.h"
#include "davt.h"

#include "transform.h"
#include "stream.h"
#include "string.h"
#include "dtm.h"
#include "dsp_sdk.h"
#include "hal_audio_afe_control.h"
#include "dsp_callback.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
DLIST_HEAD gTransformList;


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/**
 * @brief init trasform link list
 */
VOID TransformList_Init(VOID)
{
    dlist_init(&gTransformList);
}

/**
 * @brief Start a transform
 *
 * @param transform The transform to start.
 *
 * @return FALSE on failure, TRUE on success.
 */
BOOL TransformStart(TRANSFORM transform)
{
    UNUSED(transform);
	return TRUE;
}

/**
 * @brief Stop a transform.
 *
 * @param transform The transform to stop.
 *
 * @return FALSE on failure, TRUE on success.
 */
BOOL TransformStop(TRANSFORM transform)
{
    UNUSED(transform);
	return TRUE;
}


/**
 * @brief Find the transform connected to a source.
 *
 * @param source The source to look for.
 *
 * @return Transform connected to the specified source, or zero if no transform.
 */
TRANSFORM TransformFromSource(SOURCE source)
{
    TRANSFORM transform = NULL;

    if(source)
    {
        transform = source->transform;
    }

	return transform;
}

/**
 * @brief Find the transform connected to a sink.
 *
 * @param sink The sink to look for.
 *
 * @return Transform connected to the specified sink, or zero if no transform.
 */
TRANSFORM TransformFromSink(SINK sink)
{
    TRANSFORM transform = NULL;

    if(sink)
    {
        transform = sink->transform;
    }

	return transform;
}
U32 globall;
TRANSFORM TrasformAudio2Audio(SOURCE source, SINK sink, VOID* feature_list_ptr)
{
    TRANSFORM transform = NULL;
    #if (!ForceDSPCallback)
    if ((((sink->type != SINK_TYPE_AUDIO)&&(sink->type != SINK_TYPE_VP_AUDIO)&&(sink->type != SINK_TYPE_DSP_JOINT))
         &&((source->type != SOURCE_TYPE_AUDIO)&&(source->type != SOURCE_TYPE_DSP_BRANCH)))
        ||((sink->type == SINK_TYPE_AUDIO)&&(Audio_Sink_Status.DSP_Audio_busy))
        ||((sink->type == SINK_TYPE_VP_AUDIO)&&(Audio_Sink_Status.DSP_vp_path_busy)))
    {
    }
    else
    #endif
    if ((source->transform!=NULL) || ((sink->transform!=NULL) && (sink->type!=SINK_TYPE_DSP_VIRTUAL)))
    {
        if ((source->transform == sink->transform))
        {
            transform = source->transform;
        }
    }
    else
    {
        globall=sizeof(TRANSFORM_T);
        transform = pvPortMalloc(globall);
        U32 transform_xLinkRegAddr = (U32)__builtin_return_address(0);
        UNUSED(transform_xLinkRegAddr);
        if (transform != NULL)
        {
            DSP_MW_LOG_I("[transfrom]the prev_function lr= 0x%x , the malloc_address = 0x%x , the malloc_size = 0x%x",3,transform_xLinkRegAddr,transform,globall);
            memset(transform, 0, sizeof(TRANSFORM_T));
            transform->source = source;
            transform->sink = sink;
            transform->Handler = Stream_Audio_Handler;

            dlist_init(&transform->list);
            dlist_append(&transform->list, &gTransformList);

            source->transform = transform;
            sink->transform = transform;
            TaskHandle_t  dsp_task_id = DSP_Callback_Config(source, sink, feature_list_ptr, TRUE);

            if (dsp_task_id == NULL_TASK_ID)
            {
                vPortFree(transform);
                DSP_MW_LOG_I("[transfrom]the prev_function lr= 0x%x , the free_address = 0x%x",2,transform_xLinkRegAddr,transform);
                transform = NULL;
            }


            #ifdef AIR_AUDIO_HARDWARE_ENABLE
            audio_ops_trigger(source, AFE_PCM_TRIGGER_START);
            #endif /* AIR_AUDIO_HARDWARE_ENABLE */
            if (source->type == SOURCE_TYPE_DSP_0_AUDIO_PATTERN)
            {
                //SinkConfigure(sink,AUDIO_SINK_FORCE_START,0);
                SinkFlush(sink, sink->param.audio.frame_size);
            }

            if ((sink->type == SINK_TYPE_AUDIO) || (sink->type == SINK_TYPE_AUDIO_DL3) || (sink->type == SINK_TYPE_AUDIO_DL12) || (sink->type == SINK_TYPE_ADV_PASSTRU))
            {
                Audio_Sink_Status.DSP_Audio_busy = TRUE;
                #ifdef AIR_AUDIO_HARDWARE_ENABLE
                audio_ops_trigger(sink, AFE_PCM_TRIGGER_START);
                #endif /* AIR_AUDIO_HARDWARE_ENABLE */
                if(transform != NULL) {
                    transform->TransformClose = AudioTransformClose;
                }
            }
#ifdef AIR_AUDIO_I2S_SLAVE_TDM_ENABLE
            else if (sink->type == SINK_TYPE_TDMAUDIO)
            {
                Audio_Sink_Status.DSP_Audio_busy = TRUE;
                audio_ops_trigger(sink, AFE_PCM_TRIGGER_START);
                if(transform != NULL) {
                    transform->TransformClose = AudioTransformClose;
                }
            }
#endif
            else if (sink->type == SINK_TYPE_VP_AUDIO)
            {
                Audio_Sink_Status.DSP_vp_path_busy = TRUE;
                #ifdef AIR_AUDIO_HARDWARE_ENABLE
                audio_ops_trigger(sink, AFE_PCM_TRIGGER_START);
                #endif /* AIR_AUDIO_HARDWARE_ENABLE */
                if (transform != NULL) {
                    transform->TransformClose = AudioTransformClose;
                }
            }
        }
    }
    return transform;
}



BOOL AudioTransformClose(SOURCE source, SINK sink)//also disable callback function
{
    DSP_CALLBACK_PTR callback_ptr;
    U16 length;
    if ((source == NULL) || (sink == NULL))
    {
        return FALSE;
    }
    length = SinkSlack(sink);

    if (length == 0)
    {
        return FALSE;
    }
    callback_ptr = DSP_Callback_Get(source, sink);

    if (callback_ptr->Status == CALLBACK_SUSPEND)
    {
        callback_ptr->Status = CALLBACK_ZEROPADDING;
    }

    if (callback_ptr->Status == CALLBACK_WAITEND)
    {
        StreamTransformClose(sink->transform);
        DSP_Callback_Config(source, sink, NULL, FALSE);
        callback_ptr->Status = CALLBACK_DISABLE;
        // SourceClose(source);
        // SinkClose(sink);
    }
    return TRUE;
}

VOID TransformChangeHandlerClose(VOID* transform)
{
    ((TRANSFORM)transform)->Handler= ((TRANSFORM)transform)->TransformClose;
}


