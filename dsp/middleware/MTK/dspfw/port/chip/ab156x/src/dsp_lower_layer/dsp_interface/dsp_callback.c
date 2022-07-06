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
#include "types.h"
#include "dlist.h"
#include "dsp_memory.h"
#include "source.h"
#include "sink.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "dprt.h"
#include "davt.h"
#include "dhpt.h"
#include "dllt.h"
#include "dsp_drv_dfe.h"
#include "stream_audio.h"
#include "stream.h"
#include "audio_config.h"
#include "dsp_audio_ctrl.h"

#ifdef MTK_BT_A2DP_SBC_ENABLE
#include "sbc_interface.h"
#endif /* MTK_BT_A2DP_SBC_ENABLE */

#include "dsp_sdk.h"
#include "dsp_audio_process.h"
#include "dsp_feature_interface.h"
#include "dsp_callback.h"
#include "dsp_audio_msg_define.h"
//#include "sbc_interface.h"
//#include "mp3_dec_interface.h"
//#include "aac_dec_interface.h"
#include <string.h>
#include "FreeRTOS.h"
#include "stream_audio_afe.h"
#ifdef AIR_SOFTWARE_MIXER_ENABLE
#include "sw_mixer_interface.h"
#endif /* AIR_SOFTWARE_MIXER_ENABLE */

#ifdef MTK_CELT_DEC_ENABLE
#include "celt_dec_interface.h"
#endif

#ifdef AIR_BT_CODEC_BLE_V2_ENABLED
#include "lc3_dec_interface_v2.h"
#endif /* AIR_BT_CODEC_BLE_V2_ENABLED */

#if defined(AIR_SOFTWARE_GAIN_ENABLE) || defined(AIR_UL_FIX_SAMPLING_RATE_48K)
#include "stream_n9sco.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define DSP_CALLBACK_SKIP_ALL_PROCESS   (0xFF)


////////////////////////////////////////////////////////////////////////////////
// External Function Prototypes/////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EXTERN VOID DSPMEM_CheckFeatureMemory   (VOID* VOLATILE para, DSP_MEMORY_CHECK_TIMING timing);
extern bool ScoDlStopFlag;
extern bool BleDlStopFlag;


#ifdef AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
extern U8* g_vp_memptr;   //Framework Data Mem Ptr
extern U8* g_vp_memptr2;  //Feature Entry Ptr
extern U32 g_vp_featureEntrySize;
#endif

////////////////////////////////////////////////////////////////////////////////
BOOL DSP_Callback_Undo          (DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr);
BOOL DSP_Callback_Malloc        (DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr);
BOOL DSP_Callback_Init          (DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr);
BOOL DSP_Callback_Handler       (DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr);
BOOL DSP_Callback_ZeroPadding   (DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr);
VOID DSP_Callback_StreamingRateConfig(SOURCE source, SINK sink);


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CALLBACK_STATE_ENTRY DSP_CallbackEntryTable[] =
{
    DSP_Callback_Undo,              /*CALLBACK_DISABLE*/
    DSP_Callback_Malloc,            /*CALLBACK_MALLOC*/
    DSP_Callback_Init,              /*CALLBACK_INIT*/
    DSP_Callback_Undo,              /*CALLBACK_SUSPEND*/
    DSP_Callback_Handler,           /*CALLBACK_HANDLER*/
    DSP_Callback_ZeroPadding,       /*CALLBACK_BYPASSHANDLER*/
    DSP_Callback_ZeroPadding,       /*CALLBACK_ZEROPADDING*/
    DSP_Callback_Undo,              /*CALLBACK_WAITEND*/
};



////////////////////////////////////////////////////////////////////////////////
// DSP FUNCTION DECLARATIONS ///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * DSP_Callback_Undo
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_Callback_Undo(DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr)
{
    UNUSED(entry_para);
    UNUSED(feature_table_ptr);
    return FALSE;
}

/**
 * DSP_Callback_Malloc
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_Callback_Malloc(DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr)
{
    DSP_STREAMING_PARA_PTR  stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(entry_para);
    entry_para->number.field.process_sequence = 1;
    if (feature_table_ptr != NULL)
    {
        while (feature_table_ptr->ProcessEntry != stream_function_end_process && feature_table_ptr->ProcessEntry != NULL)
        {
            DSP_MW_LOG_I("[DSP_RESOURCE] Feature ID : %d, working buffer malloc size %d", 2, feature_table_ptr->FeatureType,feature_table_ptr->MemSize);
            feature_table_ptr->MemPtr = DSPMEM_tmalloc(entry_para->DSPTask, feature_table_ptr->MemSize, stream_ptr);
            feature_table_ptr++;
            entry_para->number.field.process_sequence++;
        }
    }
    return FALSE;
}

/**
 * DSP_CleanUpCallbackOutBuf
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
void DSP_CleanUpCallbackOutBuf(DSP_ENTRY_PARA_PTR entry_para)
{
    U32 i;
    for (i=0 ; i<entry_para->out_channel_num ; i++)
    {
        memset(entry_para->out_ptr[i],
               0,
               entry_para->out_malloc_size);
    }
    if ((entry_para->out_channel_num == 1) && (entry_para->out_ptr[1] != NULL)) //Also clean up codec out buffer
    {
        memset(entry_para->out_ptr[1],
               0,
               entry_para->out_malloc_size);
    }
}

/**
 * DSP_CallbackCodecRecord
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 void DSP_CallbackCodecRecord(DSP_ENTRY_PARA_PTR entry_para)
{
    if(entry_para->number.field.process_sequence == CODEC_ALLOW_SEQUENCE)
    {
        entry_para->pre_codec_out_sampling_rate = entry_para->codec_out_sampling_rate;
        entry_para->pre_codec_out_size = entry_para->codec_out_size;
    }
}

/**
 * DSP_CallbackCheckResolution
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 void DSP_CallbackCheckResolution(DSP_ENTRY_PARA_PTR entry_para)
{
    if(entry_para->number.field.process_sequence == CODEC_ALLOW_SEQUENCE)
    {
        if(entry_para->resolution.process_res != entry_para->resolution.feature_res)
        {
            //Warning MSG
        }
    }
}


/**
 * DSP_Callback_Init
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_Callback_Init(DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr)
{
    entry_para->number.field.process_sequence = 1;
    entry_para->resolution.process_res = entry_para->resolution.feature_res;

    if (feature_table_ptr != NULL)
    {
        while (feature_table_ptr->ProcessEntry != NULL)
        {
            entry_para->mem_ptr = feature_table_ptr->MemPtr;
            if((feature_table_ptr->InitialEntry(entry_para)))
            {
                DSP_CleanUpCallbackOutBuf(entry_para);
                return TRUE;
            }
            if(feature_table_ptr->ProcessEntry == stream_function_end_process)
            {
                break;
            }
            DSP_CallbackCodecRecord(entry_para);
            entry_para->number.field.process_sequence++;
            feature_table_ptr++;
        }
    }
    return FALSE;
}

/**
 * DSP_Callback_Handler
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL DSP_Callback_Handler(DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr)
{
    DSP_STREAMING_PARA_PTR  stream_ptr = DSP_STREAMING_GET_FROM_PRAR(entry_para);
    BOOL result = FALSE;
    entry_para->number.field.process_sequence = 1;
    entry_para->codec_out_sampling_rate = entry_para->pre_codec_out_sampling_rate;
    entry_para->codec_out_size = entry_para->pre_codec_out_size;
    entry_para->resolution.process_res = entry_para->resolution.source_in_res;

    if (feature_table_ptr != NULL)
    {
        while (feature_table_ptr->ProcessEntry != NULL)
        {
            entry_para->mem_ptr = feature_table_ptr->MemPtr;
            if (DSP_Callback_SRC_Triger_Chk(&stream_ptr->callback))
            {
                if (!(entry_para->skip_process == entry_para->number.field.process_sequence))
                {
                    if((feature_table_ptr->ProcessEntry(entry_para)))
                    {
                        #ifdef AIR_SOFTWARE_MIXER_ENABLE
                        if(feature_table_ptr->ProcessEntry == stream_function_sw_mixer_process)
                        {
                            /* in here, it means software mixer wants to bypass all subsequent features */
                            result = FALSE;
                            break;
                        }
                        #endif /* AIR_SOFTWARE_MIXER_ENABLE */

                        DSP_CleanUpCallbackOutBuf(entry_para);
                        DSP_MW_LOG_I("handler return true", 0);
                        result = TRUE;
                        break;
                    }
                    DSP_CallbackCodecRecord(entry_para);
                }
                else
                {
                    DSP_CleanUpCallbackOutBuf(entry_para);
                }
            }
            DSP_CallbackCheckResolution(entry_para);
            if(feature_table_ptr->ProcessEntry == stream_function_end_process)
            {
                break;
            }
            entry_para->number.field.process_sequence++;
            feature_table_ptr++;
        }
    }
    else
    {
        stream_pcm_copy_process(entry_para);
        result = TRUE;
    }
    return result;
}

/**
 * DSP_Callback_ZeroPadding
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_Callback_ZeroPadding(DSP_ENTRY_PARA_PTR entry_para, DSP_FEATURE_TABLE_PTR feature_table_ptr)
{
    DSP_STREAMING_PARA_PTR  stream_ptr = DSP_STREAMING_GET_FROM_PRAR(entry_para);
    BOOL result = FALSE;

    entry_para->number.field.process_sequence = 1;
    entry_para->codec_out_sampling_rate = entry_para->pre_codec_out_sampling_rate;
    entry_para->codec_out_size = entry_para->pre_codec_out_size;
    entry_para->resolution.process_res = entry_para->resolution.feature_res;

    DSP_CleanUpCallbackOutBuf(entry_para);

    if (feature_table_ptr != NULL)
    {
        while (feature_table_ptr->ProcessEntry != NULL)
        {
            entry_para->mem_ptr = feature_table_ptr->MemPtr;

            if (DSP_Callback_SRC_Triger_Chk(&stream_ptr->callback))
            {
                if (!(entry_para->skip_process == entry_para->number.field.process_sequence))
                {
                    if(feature_table_ptr->ProcessEntry(entry_para))
                    {
                        if (entry_para->number.field.process_sequence != 1)
                        {
                            DSP_CleanUpCallbackOutBuf(entry_para);
                            break;
                        }
                        else
                            entry_para->codec_out_size = 0;
                    }
                    DSP_CallbackCodecRecord(entry_para);
                    if ((entry_para->codec_out_size==0)&&(entry_para->bypass_mode != BYPASS_CODEC_MODE))
                    {
                        DSP_CleanUpCallbackOutBuf(entry_para);
                        entry_para->codec_out_size = entry_para->pre_codec_out_size;
                        result = TRUE;
                    }
                }
                else
                {
                    result = TRUE;
                }
            }
            DSP_CallbackCheckResolution(entry_para);
            if(feature_table_ptr->ProcessEntry == stream_function_end_process)
            {
                break;
            }
            entry_para->number.field.process_sequence++;
            feature_table_ptr++;
        }
    }
    return result;
}

/**
 * DSP_Callback_Config
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
TaskHandle_t  DSP_Callback_Config(SOURCE source, SINK sink, VOID* feature_list_ptr, BOOL isEnable)
{
    TaskHandle_t  dsp_task_id = NULL_TASK_ID;
    #if (ForceDSPCallback)

    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    Audio_Default_setting_init();
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */

    BOOL IsSourceAudio = TRUE;
    BOOL IsSinkAudio = TRUE;
	BOOL IsBranchJoint = FALSE;

    if (sink->type == SINK_TYPE_VP_AUDIO){
       source->taskId = DPR_TASK_ID;
       sink->taskid = DPR_TASK_ID;
    } else if ((sink->type == SINK_TYPE_AUDIO_DL3)) {
        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        if (source->param.audio.scenario_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH) {
            source->taskId = DLL_TASK_ID;
            sink->taskid = DLL_TASK_ID;
        } else
        #endif
        {
            source->taskId = DHP_TASK_ID;
            sink->taskid = DHP_TASK_ID;
        }
#ifndef MTK_BT_A2DP_AIRO_CELT_ENABLE
    } else if (sink->type == SINK_TYPE_CM4RECORD) {
        source->taskId = DHP_TASK_ID;
        sink->taskid = DHP_TASK_ID;
#endif
#ifdef MTK_MULTI_MIC_STREAM_ENABLE
    } else if ((source->type >= SOURCE_TYPE_SUBAUDIO_MIN) && (source->type <= SOURCE_TYPE_SUBAUDIO_MAX)) {
        //source->taskId = DHP_TASK_ID;
        //sink->taskid = DHP_TASK_ID;
#endif
#ifdef MTK_BT_A2DP_AIRO_CELT_ENABLE
    } else if ((source->type == SOURCE_TYPE_A2DP)
            && (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT)) {
        source->taskId = DHP_TASK_ID;
        sink->taskid = DHP_TASK_ID;
#endif
    }

    /* check if both of source task id and sink task id are not configured */
    if ((source->taskId != DPR_TASK_ID) &&
        (sink->taskid   != DPR_TASK_ID) &&
        (source->taskId != DAV_TASK_ID) &&
        (sink->taskid   != DAV_TASK_ID) &&
        (source->taskId != DHP_TASK_ID) &&
        (sink->taskid   != DHP_TASK_ID)
        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        && (source->taskId != DLL_TASK_ID) &&
        (sink->taskid   != DLL_TASK_ID)
        #endif
        )
    {
        /* if yes, set them to the default AV task ID */
        source->taskId = DAV_TASK_ID;
        sink->taskid   = DAV_TASK_ID;
    }

    /* check task id status */ 
    if ((source->taskId == DHP_TASK_ID) || (sink->taskid == DHP_TASK_ID))
    {
        /* if anyone is DHP_TASK_ID, this stream should be run on HP Task */
        source->taskId = DHP_TASK_ID;
        sink->taskid   = DHP_TASK_ID;
    }
    else if ((source->taskId == DPR_TASK_ID) || (sink->taskid == DPR_TASK_ID))
    {
        /* if anyone is DPR_TASK_ID, this stream should be run on PR Task */
        source->taskId = DPR_TASK_ID;
        sink->taskid   = DPR_TASK_ID;
    }
    else if ((source->taskId == DAV_TASK_ID) || (sink->taskid == DAV_TASK_ID))
    {
        /* other case, this stream should be run on AV Task */
        source->taskId = DAV_TASK_ID;
        sink->taskid   = DAV_TASK_ID;
    }
    #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    else if ((source->taskId == DLL_TASK_ID) || (sink->taskid == DLL_TASK_ID))
    {
        /* if anyone is DLL_TASK_ID, this stream should be run on LL Task */
        source->taskId = DLL_TASK_ID;
        sink->taskid   = DLL_TASK_ID;
    }
    #endif

    #else

    BOOL IsSourceAudio = ((source->type == SOURCE_TYPE_AUDIO))
                            ? TRUE
                            : FALSE;
    BOOL IsSinkAudio   = ((sink->type == SINK_TYPE_AUDIO)||
                          (sink->type == SINK_TYPE_VP_AUDIO)||
                          (sink->type == SINK_TYPE_AUDIO_DL3)||
                          (sink->type == SINK_TYPE_AUDIO_DL12))
                             ? TRUE
                             : FALSE;
    BOOL IsBranchJoint = ((sink->type==SINK_TYPE_DSP_JOINT)||(source->type==SOURCE_TYPE_DSP_BRANCH));
	#endif
    DSP_CALLBACK_STREAM_CONFIG stream_config;
    stream_config.is_enable = isEnable;
    stream_config.source= source;
    stream_config.sink = sink;
    stream_config.feature_list_ptr = feature_list_ptr;
    #if (ForceDSPCallback)
        dsp_task_id = NULL_TASK_ID;
    #else
    if(( IsSourceAudio && IsSinkAudio && (source->taskId != sink->taskid))||
       ((!IsSourceAudio)&&(!IsSinkAudio)&&(!IsBranchJoint)))
    {
        dsp_task_id = NULL_TASK_ID;
    }
    else
    #endif
    if ((source->taskId == DHP_TASK_ID)||(sink->taskid == DHP_TASK_ID)) // Low lantency
    {
        dsp_task_id = DHPT_StreamingConfig(&stream_config);
    }
    else if ((IsSourceAudio && (source->taskId == DPR_TASK_ID))||//(source->type == SOURCE_TYPE_RINGTONE) //VP RT
             (IsSinkAudio   && (sink->taskid   == DPR_TASK_ID)))
    {
        dsp_task_id = DPRT_StreamingConfig(&stream_config);
    }
    else if ((IsSourceAudio && (source->taskId == DAV_TASK_ID))||
             (IsSinkAudio   && (sink->taskid   == DAV_TASK_ID))||
             (IsBranchJoint))
    {
        dsp_task_id = DAVT_StreamingConfig(&stream_config);
    }
    #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    else if ((source->taskId == DLL_TASK_ID)||(sink->taskid == DLL_TASK_ID))
    {
        dsp_task_id = DLLT_StreamingConfig(&stream_config);
    }
    #endif

    return dsp_task_id;
}

VOID DSP_Callback_StreamingInit(DSP_STREAMING_PARA_PTR stream_ptr, U8 stream_number, TaskHandle_t  task)
{
	U8 i, j;
    for (i=0 ; i<stream_number ; i++)
    {
        stream_ptr[i].source                                   = NULL;
        stream_ptr[i].sink                                     = NULL;
        stream_ptr[i].callback.Status                          = CALLBACK_DISABLE;
        stream_ptr[i].callback.EntryPara.DSPTask               = task;
        stream_ptr[i].callback.EntryPara.with_encoder          = FALSE;
        stream_ptr[i].callback.EntryPara.with_src              = FALSE;
        stream_ptr[i].callback.EntryPara.in_malloc_size        = 0;
        stream_ptr[i].callback.EntryPara.out_malloc_size       = 0;
        stream_ptr[i].callback.EntryPara.bypass_mode           = 0;
        for (j=0 ; j<CALLBACK_INPUT_PORT_MAX_NUM ; j++)
        {
            stream_ptr[i].callback.EntryPara.in_ptr[j]         = NULL;
        }
        for (j=0 ; j<CALLBACK_OUTPUT_PORT_MAX_NUM ; j++)
        {
            stream_ptr[i].callback.EntryPara.out_ptr[j]        = NULL;
        }
        stream_ptr[i].callback.FeatureTablePtr                 = NULL;
        stream_ptr[i].callback.Src.src_ptr                     = NULL;
        /*
        stream_ptr[i].driftCtrl.para.comp_mode                 = DISABLE_COMPENSATION;
        stream_ptr[i].driftCtrl.para.src_ptr                   = NULL;
        stream_ptr[i].driftCtrl.SourceLatch                    = NonLatch;
        stream_ptr[i].driftCtrl.SinkLatch                      = NonLatch;
        */
        stream_ptr[i].DspReportEndId                           = MSG_DSP_NULL_REPORT;
        stream_ptr[i].streamingStatus                          = STREAMING_DISABLE;
    }
}

VOID DSP_Callback_StreamClean(VOID *ptr)
{
    DSP_CALLBACK_STREAM_CLEAN_PTR clean_ptr     = ptr;

    if ((clean_ptr->stream_ptr->callback.Status != CALLBACK_SUSPEND) &&
        (clean_ptr->stream_ptr->callback.Status != CALLBACK_DISABLE) &&
        (clean_ptr->stream_ptr->callback.Status != CALLBACK_WAITEND))
    {
        clean_ptr->is_clean                         = FALSE;
    }
    else
    {
        clean_ptr->is_clean                         = TRUE;
        clean_ptr->stream_ptr->source               = NULL;
        clean_ptr->stream_ptr->sink                 = NULL;
        clean_ptr->stream_ptr->callback.Status      = CALLBACK_DISABLE;
        clean_ptr->stream_ptr->streamingStatus      = STREAMING_END;
    }
}

TaskHandle_t  DSP_Callback_StreamConfig(DSP_CALLBACK_STREAM_CONFIG_PTR stream_config_ptr)
{
    U8 i;
    stream_feature_list_ptr_t featureListPtr = stream_config_ptr->feature_list_ptr;
    if (stream_config_ptr->is_enable)
    {
        for (i=0 ; i<stream_config_ptr->stream_number ; i++)
        {
            if ((stream_config_ptr->stream_ptr[i].source == stream_config_ptr->source) &&
                (stream_config_ptr->stream_ptr[i].sink == stream_config_ptr->sink))
            {
                return stream_config_ptr->task;
            }
        }
        for (i=0 ; i<stream_config_ptr->stream_number ; i++)
        {
            if (stream_config_ptr->feature_list_ptr == NULL)
            {
                return NULL_TASK_ID;
            }
            if (stream_config_ptr->stream_ptr[i].streamingStatus == STREAMING_DISABLE)
            {
                DSP_Callback_StreamingRateConfig(stream_config_ptr->source,stream_config_ptr->sink);// Sink rate follows only now
                stream_config_ptr->stream_ptr[i].source                      = stream_config_ptr->source;
                stream_config_ptr->stream_ptr[i].sink                        = stream_config_ptr->sink;
                stream_config_ptr->stream_ptr[i].streamingStatus             = STREAMING_START;
                DSP_MW_LOG_I("DSP - Create Callback Stream, Source:%d, Sink:%d, Codec:%x", 3,stream_config_ptr->source->type, stream_config_ptr->sink->type, (*featureListPtr)&0xFF);
                DSP_Callback_FeatureConfig((DSP_STREAMING_PARA_PTR)&stream_config_ptr->stream_ptr[i], stream_config_ptr->feature_list_ptr);
                return stream_config_ptr->task;
            }
        }
        DSP_MW_LOG_I("DSP - Callback Stream Fail, Source:%d, Sink:%d, stream_number:%d ", 3,stream_config_ptr->source->type, stream_config_ptr->sink->type, stream_config_ptr->stream_number);
        configASSERT(false);
    }
    else
    {
        for (i=0 ; i<stream_config_ptr->stream_number ; i++)
        {
            if ((stream_config_ptr->stream_ptr[i].streamingStatus != STREAMING_DISABLE) &&
                (stream_config_ptr->stream_ptr[i].source == stream_config_ptr->source) &&
                (stream_config_ptr->stream_ptr[i].sink == stream_config_ptr->sink))
            {
                /*
                DSP_CALLBACK_STREAM_CLEAN clean_stru;
                clean_stru.is_clean = FALSE;
                clean_stru.stream_ptr = &(stream_config_ptr->stream_ptr[i]);
                stream_config_ptr->stream_ptr[i].streamingStatus = STREAMING_END;


                do
                {
                    OS_CRITICAL(DSP_Callback_StreamClean, &clean_stru);
                    if (!clean_stru.is_clean)
                    {
                        osTaskTaskingRequest();
                        DSP_LOG_WarningPrint(DSP_WARNING_STREAMING_DISABLE,
                                             2,
                                             stream_config_ptr->source->type,
                                             stream_config_ptr->sink->type);
                    }
                } while (!clean_stru.is_clean);
                */
                DSP_MW_LOG_I("DSP - Close Callback Stream, Source:%d, Sink:%d", 2, stream_config_ptr->source->type, stream_config_ptr->sink->type);
                #ifdef AIR_AUDIO_HARDWARE_ENABLE
                DSP_DRV_SRC_END(stream_config_ptr->stream_ptr[i].callback.Src.src_ptr);
                #endif /* AIR_AUDIO_HARDWARE_ENABLE */
                DSPMEM_Free(stream_config_ptr->task, (DSP_STREAMING_PARA_PTR)&stream_config_ptr->stream_ptr[i]);
                DSP_Callback_StreamingInit((DSP_STREAMING_PARA_PTR)&stream_config_ptr->stream_ptr[i],
                                           1,
                                           stream_config_ptr->stream_ptr[i].callback.EntryPara.DSPTask);
                return stream_config_ptr->task;
            }
        }
    }
    return NULL_TASK_ID;
}
VOID DSP_Callback_StreamingRateConfig(SOURCE source, SINK sink)// Sink rate follows only now
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    //U8 Output_Rate = PeripheralOutputSamplingRate_Get(sink);
    U8 Output_Rate =Audio_setting->Rate.Sink_Output_Sampling_Rate;


    switch (source->type)
    {
        case SOURCE_TYPE_USBAUDIOCLASS :
            if (Output_Rate > source->param.USB.sampling_rate)//Cant be 44.1 Hz
            {
                SinkConfigure(sink,AUDIO_SINK_UPSAMPLE_RATE,DSP_UpValue2Rate(Output_Rate/source->param.USB.sampling_rate));
            }
            else
            {
                SinkConfigure(sink,AUDIO_SINK_UPSAMPLE_RATE,UPSAMPLE_BY1);
            }
            if (Audio_setting->Audio_sink.Frame_Size != (source->param.USB.frame_size*2/3))
            {
                SinkConfigure(sink,AUDIO_SINK_FRAME_SIZE,(source->param.USB.frame_size*2/3));
            }
            SinkConfigure(sink,AUDIO_SINK_FORCE_START,0);
        break;
        default:
        break;
    }
    switch (sink->type)
    {
        //U8 Input_Rate = PeripheralInputSamplingRate_Get(source);
        /*
        U8 Input_Rate = Audio_setting->Rate.Source_Input_Sampling_Rate;
        case SINK_TYPE_USBAUDIOCLASS :
            if (Input_Rate > source->param.USB.sampling_rate)//Cant be 44.1 Hz
            {
                SourceConfigure(source,AUDIO_SOURCE_DOWNSAMP_RATE,DSP_DownValue2Rate(sink->param.USB.sampling_rate/Input_Rate));
            }
            else
            {
                SourceConfigure(source,AUDIO_SOURCE_DOWNSAMP_RATE,DOWNSAMPLE_BY1);
            }
            if (Audio_setting->Audio_source.Frame_Size != (source->param.USB.frame_size*2/3))
            {
                SourceConfigure(source,AUDIO_SOURCE_FRAME_SIZE,(source->param.USB.frame_size*2/3));
            }

        break;
            */
        default:
        break;
    }
#else
    UNUSED(source);
    UNUSED(sink);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

/**
 * DSP_Callback_FeatureConfig
 *
 * Get memory and copy feature function entry
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
VOID DSP_Callback_FeatureConfig(DSP_STREAMING_PARA_PTR stream, stream_feature_list_ptr_t feature_list_ptr)
{
    U32  featureEntrySize;
    stream_feature_type_ptr_t  featureTypePtr;
    DSP_FEATURE_TABLE_PTR featurePtr;
    VOID*  mem_ptr;
    U32 i, featureEntryNum = 0;
    U32 codecOutSize, codecOutResolution;

    #ifdef PRELOADER_ENABLE

    if ((featureTypePtr = stream->callback.EntryPara.feature_ptr = feature_list_ptr)!= NULL)
    #else
    featureTypePtr = feature_list_ptr;

    if (featureTypePtr != NULL)
    #endif
    {
        for(featureEntryNum=1 ; *(featureTypePtr)!=FUNC_END ; ++featureTypePtr)
        {

            if ((*(featureTypePtr)&0xFF) == DSP_SRC)
            {
                DSP_Callback_SRC_Config(stream, featureTypePtr, featureEntryNum);
            }
            featureEntryNum++;
        }
    }
    featureEntrySize = featureEntryNum*sizeof(DSP_FEATURE_TABLE);
    stream->callback.EntryPara.number.field.feature_number = featureEntryNum;

#ifdef AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
    if(stream->source->type == SOURCE_TYPE_CM4_VP_PLAYBACK){
        mem_ptr = g_vp_memptr2;
        memset(mem_ptr, 0, g_vp_featureEntrySize);
    }else{
        mem_ptr = DSPMEM_tmalloc(stream->callback.EntryPara.DSPTask, featureEntrySize, stream);
    }
#else
    mem_ptr = DSPMEM_tmalloc(stream->callback.EntryPara.DSPTask, featureEntrySize, stream);
#endif

    if (featureEntrySize>0)
    {
        stream->callback.FeatureTablePtr  = mem_ptr;
        featureTypePtr = feature_list_ptr;
        for(i=0 ; i<featureEntryNum ; i++)
        {
            featurePtr = (DSP_FEATURE_TABLE_PTR)&stream_feature_table[((U32)(*(featureTypePtr)&0xFF))];
            *(stream->callback.FeatureTablePtr + i) = *featurePtr;
            if (i==0 || i==featureEntryNum-2)
            {
                if ((*(featureTypePtr)&0xFFFF0000) != 0)
                {
                    codecOutSize = (*(featureTypePtr))>>16;
                    ((stream->callback.FeatureTablePtr + i)->MemPtr) = (VOID*)codecOutSize;
                }
                else
                {
                    codecOutSize = (U32)((stream->callback.FeatureTablePtr + i)->MemPtr);
                }
            }
            if (i==0)
            {
                codecOutResolution = (*(featureTypePtr)&0xFF00)>>8;
                stream->callback.EntryPara.resolution.feature_res = ((codecOutResolution==RESOLUTION_16BIT) || (codecOutResolution==RESOLUTION_32BIT))
                                                                      ? codecOutResolution
                                                                      : RESOLUTION_16BIT;
            }
            featureTypePtr++;
        }

        DSP_Callback_ParaSetup(stream);

        if (stream->callback.EntryPara.with_src)
        {
             (stream->callback.FeatureTablePtr + (U32)(stream->callback.EntryPara.with_src - 1))->MemSize +=
                        2*(stream->callback.EntryPara.out_malloc_size*(DSP_CALLBACK_SRC_BUF_FRAME+DSP_CALLBACK_SRC_IN_FRAME) +
                           stream->callback.Src.out_frame_size*(DSP_CALLBACK_SRC_OUT_FRAME));
#ifdef MTK_HWSRC_IN_STREAM
             (stream->callback.FeatureTablePtr + (U32)(stream->callback.EntryPara.with_src - 1))->MemSize += 64;//modify for asrc, for src_ptr+16, inSRC_mem_ptr+16, outSRC_mem_ptr+16, buf_mem_ptr+16;
#endif

        }


        stream->callback.Status = CALLBACK_MALLOC;
        vTaskResume(stream->callback.EntryPara.DSPTask);
    // while (stream->callback.Status!=CALLBACK_SUSPEND)
        // portYIELD();
    }
    else
    {
        stream->callback.FeatureTablePtr  = NULL;
        DSP_MW_LOG_I("DSP - Warning:Feature Ptr Null. Source:%d, Sink:%d", 2, stream->source->type, stream->sink->type);
    }
}
#ifdef PRELOADER_ENABLE
VOID DSP_Callback_PreloaderConfig(stream_feature_list_ptr_t feature_list_ptr)
{
    stream_feature_ctrl_entry featureOpenPtr;
    U16 feature_cnt = 0;
    DSP_MW_LOG_D("feature list ptr 0x%x", 1,feature_list_ptr);
    if (feature_list_ptr != NULL)
    {
        /* If there is CPD feautre, load the CPD PIC LIB at first */
        for( ; *(feature_list_ptr)!=FUNC_END ; ++feature_list_ptr)
        {
            feature_cnt++;
            if((*(feature_list_ptr) == FUNC_RX_WB_DRC) || (*(feature_list_ptr) == FUNC_RX_NB_DRC) || (*(feature_list_ptr) == FUNC_TX_WB_DRC) || (*(feature_list_ptr) == FUNC_TX_NB_DRC) || (*(feature_list_ptr) == FUNC_DRC) || (*(feature_list_ptr) == FUNC_DRC2)|| (*(feature_list_ptr) == FUNC_DRC3) || (*(feature_list_ptr) == FUNC_INS))
            {
                DSP_MW_LOG_I("[DSP_RESOURCE][DSP_Callback_PreloaderConfig] Feature ID(DRC): 0x%x, feature_cnt:%d", 2, *(feature_list_ptr), feature_cnt);
                if ((featureOpenPtr = DSP_FeatureControl[((U32)(*(feature_list_ptr)&0xFF))].open_entry) != NULL)
                {
                    featureOpenPtr(); //load CPD PIC LIB
                }
            }
        }

        /* Reset feature_list_ptr to the front */
        for(int i = 0; i<feature_cnt; i++) {
            --feature_list_ptr;
        }
        feature_cnt = 0;

        /* Load PIC Library according to the feature_list_ptr */
        for( ; *(feature_list_ptr)!=FUNC_END ; ++feature_list_ptr)
        {
            feature_cnt++;
            if((*(feature_list_ptr) == FUNC_RX_WB_DRC) || (*(feature_list_ptr) == FUNC_RX_NB_DRC) || (*(feature_list_ptr) == FUNC_TX_WB_DRC) || (*(feature_list_ptr) == FUNC_TX_NB_DRC) || (*(feature_list_ptr) == FUNC_DRC) || (*(feature_list_ptr) == FUNC_DRC2)|| (*(feature_list_ptr) == FUNC_DRC3) || (*(feature_list_ptr) == FUNC_INS))
            {
                // no need to load CPD PIC LIB again
            } else {
               if ((featureOpenPtr = DSP_FeatureControl[((U32)(*(feature_list_ptr)&0xFF))].open_entry) != NULL)
               {
                   DSP_MW_LOG_I("[DSP_RESOURCE][DSP_Callback_PreloaderConfig] Feature ID: 0x%x, feature_cnt:%d", 2, *(feature_list_ptr), feature_cnt);
                   featureOpenPtr();
               }
            }
        }
        DSP_MW_LOG_I("[DSP_RESOURCE] Feature PIC load end", 0);
    }
}
#endif

/*
 * DSP_Callback_ResolutionConfig
 *
 * Configure Callback parametsers resolution
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
VOID DSP_Callback_ResolutionConfig(DSP_STREAMING_PARA_PTR stream)
{
    stream->callback.EntryPara.resolution.source_in_res = RESOLUTION_16BIT;
    stream->callback.EntryPara.resolution.sink_out_res = RESOLUTION_16BIT;
    stream->callback.EntryPara.resolution.process_res = RESOLUTION_16BIT;

    /* Configure Callback in resolution */
#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    if (stream->source->type == SOURCE_TYPE_AUDIO || (stream->source->type>= SOURCE_TYPE_SUBAUDIO_MIN && stream->source->type<= SOURCE_TYPE_SUBAUDIO_MAX))
    {
#else
    if (stream->source->type == SOURCE_TYPE_AUDIO || stream->source->type == SOURCE_TYPE_AUDIO2)
    {
#endif
        //stream->callback.EntryPara.resolution.source_in_res = Audio_setting->resolution.AudioInRes;
        stream->callback.EntryPara.resolution.source_in_res = (stream->source->param.audio.format<=AFE_PCM_FORMAT_U16_BE)
                                                                ? RESOLUTION_16BIT
                                                                : RESOLUTION_32BIT;
    }
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
    else if (stream->source->type == SOURCE_TYPE_CM4_VP_DUMMY_SOURCE_PLAYBACK){
        audio_bits_per_sample_t bit_type = stream->source->param.cm4_playback.info.bit_type;
        if (AUDIO_BITS_PER_SAMPLING_24 == bit_type){
            stream->callback.EntryPara.resolution.source_in_res = RESOLUTION_32BIT;
        } else {
            stream->callback.EntryPara.resolution.source_in_res = RESOLUTION_16BIT;
        }
    }
#endif

    /* Configure Callback out resolution */
    #if 0
    if (stream->sink->type == SINK_TYPE_AUDIO)
    {
        stream->callback.EntryPara.resolution.sink_out_res = (stream->sink->param.audio.AfeBlkControl.u4asrcflag)
                                                               ? Audio_setting->resolution.SRCInRes
                                                               : Audio_setting->resolution.AudioOutRes;
    }
    else if (stream->sink->type == SINK_TYPE_VP_AUDIO)
    {
        //stream->callback.EntryPara.resolution.sink_out_res = Audio_setting->Audio_VP.Fade.Resolution;//Audio_setting->resolution.AudioOutRes;
        stream->callback.EntryPara.resolution.sink_out_res = (stream->sink->param.audio.AfeBlkControl.u4asrcflag)
                                                               ? Audio_setting->resolution.SRCInRes
                                                               : Audio_setting->resolution.AudioOutRes;
    }
    #else
    if ((stream->sink->type == SINK_TYPE_AUDIO) || (stream->sink->type == SINK_TYPE_VP_AUDIO) || (stream->sink->type == SINK_TYPE_AUDIO_DL3) || (stream->sink->type == SINK_TYPE_AUDIO_DL12) )
    {
        stream->callback.EntryPara.resolution.sink_out_res = (stream->sink->param.audio.format<=AFE_PCM_FORMAT_U16_BE)
                                                                ? RESOLUTION_16BIT
                                                                : RESOLUTION_32BIT;
    }
    #endif

    stream->callback.EntryPara.resolution.process_res = stream->callback.EntryPara.resolution.source_in_res;

}
#ifdef PRELOADER_ENABLE
/**
 * DSP_Callback_FeatureDeinit
 *
 * Deinit entry of stream
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
VOID DSP_Callback_FeatureDeinit(DSP_STREAMING_PARA_PTR stream)
{
    stream_feature_type_ptr_t  featureTypePtr;
    stream_feature_ctrl_entry featureClosePtr;

    if ((featureTypePtr = stream->callback.EntryPara.feature_ptr) != NULL)
    {
        for( ; *(featureTypePtr)!=FUNC_END ; ++featureTypePtr)
        {
            if ((featureClosePtr = DSP_FeatureControl[((U32)(*(featureTypePtr)&0xFF))].close_entry) != NULL)
            {
                featureClosePtr();
            }
        }
    }
}

VOID DSP_PIC_FeatureDeinit(stream_feature_type_ptr_t featureTypePtr)
{
    stream_feature_ctrl_entry featureClosePtr;

    if (featureTypePtr != NULL)
    {
        for( ; *featureTypePtr != FUNC_END ; ++featureTypePtr)
        {
            if ((featureClosePtr = DSP_FeatureControl[((U32)(*(featureTypePtr)&0xFF))].close_entry) != NULL)
            {
                featureClosePtr();
            }
        }
    }
}

#endif

/**
 * DSP_CallbackTask_Get
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */

TaskHandle_t  DSP_CallbackTask_Get(SOURCE source, SINK sink)
{
    return DSP_Callback_Config(source, sink, NULL, TRUE);
}

/**
 * DSP_Callback_Get
 *
 * Get DSP callback ptr
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 DSP_CALLBACK_PTR DSP_Callback_Get(SOURCE source, SINK sink)
{
    DSP_CALLBACK_PTR callback_ptr = NULL;
    if (callback_ptr == NULL)
    {
        callback_ptr = DHPT_Callback_Get(source, sink);
    }
    if (callback_ptr == NULL)
        callback_ptr = DAVT_Callback_Get(source, sink);
    if (callback_ptr == NULL)
        callback_ptr = DPRT_Callback_Get(source, sink);
    #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    if (callback_ptr == NULL)
    {
        callback_ptr = DLLT_Callback_Get(source, sink);
    }
    #endif
    return callback_ptr;
}

/**
 * DSP_Callback_BypassModeCtrl
 *
 * Get DSP callback ptr
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DSP_Callback_BypassModeCtrl(SOURCE source, SINK sink , DSP_CALLBACK_BYPASS_CODEC_MODE mode)
{
    DSP_CALLBACK_PTR callback_ptr = NULL;
    if (callback_ptr == NULL)
        callback_ptr = DHPT_Callback_Get(source, sink);
    if (callback_ptr == NULL)
        callback_ptr = DAVT_Callback_Get(source, sink);
    if (callback_ptr == NULL)
        callback_ptr = DPRT_Callback_Get(source, sink);

    if  (callback_ptr != NULL)
    {
        callback_ptr->EntryPara.bypass_mode = mode;
    }
}

/**
 * DSP_Callback_BypassModeCtrl
 *
 * Get DSP callback ptr
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 DSP_CALLBACK_BYPASS_CODEC_MODE DSP_Callback_BypassModeGet(SOURCE source, SINK sink)
{
    DSP_CALLBACK_BYPASS_CODEC_MODE reportmode = 0;
    DSP_CALLBACK_PTR callback_ptr = NULL;
    if (callback_ptr == NULL)
        callback_ptr = DHPT_Callback_Get(source, sink);
    if (callback_ptr == NULL)
        callback_ptr = DAVT_Callback_Get(source, sink);
    if (callback_ptr == NULL)
        callback_ptr = DPRT_Callback_Get(source, sink);

    if  (callback_ptr != NULL)
    {
        reportmode = callback_ptr->EntryPara.bypass_mode;
    }
    return reportmode;
}


/**
 * DSP_Callback_ChangeStreaming2Deinit
 *
 * Set streaming to de-initial status
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
VOID DSP_Callback_ChangeStreaming2Deinit(VOID* para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    //stream_ptr->callback.Status = CALLBACK_INIT;
    if (stream_ptr->streamingStatus == STREAMING_ACTIVE)
    {
        stream_ptr->streamingStatus = STREAMING_DEINIT;
    }

}

/**
 * DSP_ChangeStreaming2Deinit
 *
 * Set streaming to de-initial status
 *
 * @Author : Machi <MachiWu@airoha.com.tw>
 */
VOID DSP_ChangeStreaming2Deinit(TRANSFORM transform)
{
	DSP_STREAMING_PARA_PTR stream_ptr;
	stream_ptr = DSP_Streaming_Get(transform->source,transform->sink);
	if (stream_ptr != NULL)
	{
	    if (stream_ptr->streamingStatus == STREAMING_ACTIVE)
	    {
	        stream_ptr->streamingStatus = STREAMING_DEINIT;
	    }
	}

}


/**
 * DSP_Streaming_Get
 *
 * Get DSP Streaming ptr
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 DSP_STREAMING_PARA_PTR DSP_Streaming_Get(SOURCE source, SINK sink)
{
    DSP_STREAMING_PARA_PTR streaming_ptr = NULL;
    DSP_CALLBACK_PTR callback_ptr;
    callback_ptr = DSP_Callback_Get(source, sink);

    if (callback_ptr != NULL)
        streaming_ptr = DSP_CONTAINER_OF(callback_ptr, DSP_STREAMING_PARA, callback);

    return streaming_ptr;
}


/**
 * DSP_FuncMemPtr_Get
 *
 * Get DSP function memory ptr
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
VOID* DSP_FuncMemPtr_Get(DSP_CALLBACK_PTR callback, stream_feature_function_entry_t entry, U32 feature_seq)
{
    VOID* funcMemPtr = NULL;
    DSP_FEATURE_TABLE_PTR featureTablePtr;

    if ((callback != NULL) && (entry != NULL))
    {
        featureTablePtr = callback->FeatureTablePtr;
        if(feature_seq != DSP_UPDATE_COMMAND_FEATURE_PARA_SEQUENCE_AUTODETECT)
            featureTablePtr += feature_seq;
        if (featureTablePtr != NULL)
        {
            while (featureTablePtr->ProcessEntry != stream_function_end_process && featureTablePtr->ProcessEntry != NULL)
            {
                if(featureTablePtr->ProcessEntry == entry)
                {
                    funcMemPtr = featureTablePtr->MemPtr;
                    break;
                }
                else if(feature_seq != DSP_UPDATE_COMMAND_FEATURE_PARA_SEQUENCE_AUTODETECT)
                {
                    break;
                }
                featureTablePtr++;
            }
        }
    }
    return funcMemPtr;
}

/**
 * DSP_Callback_ParaSetup
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
VOID DSP_Callback_ParaSetup(DSP_STREAMING_PARA_PTR stream)
{
    U8 i;
    SOURCE source = stream->source;
    SINK sink  = stream->sink;
    U8* mem_ptr;
    U32 mallocSize, chNum, frameSize = 0;

    /* Configure Callback para  by Source type */
    if (source->type == SOURCE_TYPE_DSP_0_AUDIO_PATTERN)
    {
        stream->callback.EntryPara.in_size        = 20;
        stream->callback.EntryPara.in_channel_num = 1;

    }
    else if (source->type == SOURCE_TYPE_USBAUDIOCLASS)
    {
        stream->callback.EntryPara.in_size        = source->param.USB.frame_size;
        stream->callback.EntryPara.in_channel_num = 1;
    }
#ifdef MTK_BT_HFP_ENABLE
    else if (source->type == SOURCE_TYPE_N9SCO)
    {
       stream->callback.EntryPara.in_size        = 240;
       stream->callback.EntryPara.in_channel_num = 1;
       stream->callback.EntryPara.in_sampling_rate	= FS_RATE_16K;
    }
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
    else if (source->type == SOURCE_TYPE_N9BLE)
    {
        if(source->streamBuffer.ShareBufferInfo.sample_rate == 16000)
        {
            stream->callback.EntryPara.in_size        = 160;
            stream->callback.EntryPara.in_sampling_rate	= FS_RATE_16K;
        }
        else if(source->streamBuffer.ShareBufferInfo.sample_rate == 24000)
        {
            stream->callback.EntryPara.in_size        = 240;
            stream->callback.EntryPara.in_sampling_rate	= FS_RATE_24K;
        }
        else if(source->streamBuffer.ShareBufferInfo.sample_rate == 32000)
        {
            stream->callback.EntryPara.in_size        = 320;
            stream->callback.EntryPara.in_sampling_rate	= FS_RATE_32K;
        }
        else if(source->streamBuffer.ShareBufferInfo.sample_rate == 44100)
        {
            stream->callback.EntryPara.in_size        = 441;
            stream->callback.EntryPara.in_sampling_rate	= FS_RATE_44_1K;
        }
        else if(source->streamBuffer.ShareBufferInfo.sample_rate == 48000)
        {
            stream->callback.EntryPara.in_size        = 480;
            stream->callback.EntryPara.in_sampling_rate	= FS_RATE_48K;
        }
        stream->callback.EntryPara.in_channel_num = 1;
    }
#endif
    else if((source->type == SOURCE_TYPE_FILE)||(source->type == SOURCE_TYPE_USBCDCCLASS)||(source->type == SOURCE_TYPE_MEMORY))
    {
        stream->callback.EntryPara.in_size        = 512;
        stream->callback.EntryPara.in_channel_num = 1;
        stream->callback.EntryPara.in_sampling_rate = FS_RATE_16K;
    }
    else if (source->type == SOURCE_TYPE_A2DP)
    {
        stream->callback.EntryPara.in_size          = 1024;
        stream->callback.EntryPara.in_channel_num   = 1;
        stream->callback.EntryPara.in_sampling_rate = source->streamBuffer.AVMBufferInfo.SampleRate/1000;;
    }
    else if (source->type == SOURCE_TYPE_N9_A2DP )
    {
        stream->callback.EntryPara.in_size          = 1024;
        stream->callback.EntryPara.in_channel_num   = 1;
        stream->callback.EntryPara.in_sampling_rate = FS_RATE_48K;
    }
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
    else if ((source->type == SOURCE_TYPE_CM4_PLAYBACK) ||(source->type == SOURCE_TYPE_CM4_VP_PLAYBACK) ||(source->type == SOURCE_TYPE_CM4_VP_DUMMY_SOURCE_PLAYBACK))
#else
    else if ((source->type == SOURCE_TYPE_CM4_PLAYBACK) ||(source->type == SOURCE_TYPE_CM4_VP_PLAYBACK))
#endif
    {
        stream->callback.EntryPara.in_size          = 2048;
        stream->callback.EntryPara.in_channel_num   = source->param.cm4_playback.info.source_channels;
        stream->callback.EntryPara.in_sampling_rate = source->param.cm4_playback.info.sampling_rate/1000;
    }
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    else if ((source->type >= SOURCE_TYPE_AUDIO_TRANSMITTER_MIN) && (source->type <= SOURCE_TYPE_AUDIO_TRANSMITTER_MAX))
    {
        if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE)
        {

        }
        #if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE)
        {
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                // TODO: usb in
                stream->callback.EntryPara.in_size        = 6*48*2*2;
                stream->callback.EntryPara.in_channel_num = 2;
                stream->callback.EntryPara.in_sampling_rate = FS_RATE_48K;
            }
        }
        #endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
        #if defined(AIR_WIRED_AUDIO_ENABLE)
        else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO)
        {
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
            {
                // TODO: usb in
                stream->callback.EntryPara.in_size        = 48*10*2*2;
                stream->callback.EntryPara.in_channel_num = 2;
                stream->callback.EntryPara.in_sampling_rate = FS_RATE_48K;
                // stream->callback.EntryPara.out_channel_num          = 2;
                // stream->callback.EntryPara.src_out_size             = 2048;
                // stream->callback.EntryPara.codec_out_size           = 2048;
                /* set codec_out_sampling_rate by opus decoder for SRC */
                stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_48K;
            }
        }
        #endif /* AIR_WIRED_AUDIO_ENABLE */
        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE)
        {
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                stream->callback.EntryPara.in_size          = 20*48*2*2;
                stream->callback.EntryPara.in_channel_num   = 2;
                stream->callback.EntryPara.in_sampling_rate = source->param.data_dl.scenario_param.usb_in_broadcast_param.usb_in_param.codec_param.pcm.sample_rate/1000;
            }
        }
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
        else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_GSENSOR)
        {

        }
        else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_MULTI_MIC_STREAM)
        {

        }
        else
        {

        }
    }
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */
#ifdef MTK_AUDIO_BT_COMMON_ENABLE
    else if ((source->type >= SOURCE_TYPE_BT_COMMON_MIN) && (source->type <= SOURCE_TYPE_BT_COMMON_MAX))
    {
        if (source->param.bt_dl.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE)
        {

        }
        #if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        else if (source->param.bt_dl.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE)
        {
            if (source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                // TODO: bt in
                stream->callback.EntryPara.in_size        = 50;
                stream->callback.EntryPara.in_channel_num = 1;
                stream->callback.EntryPara.in_sampling_rate = FS_RATE_16K;
            }
        }
        #endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        else if (source->param.bt_dl.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE)
        {
            if (source->param.bt_dl.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                stream->callback.EntryPara.in_size          = 4+(source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.codec_param.lc3.frame_size+3)/4*4; //payload size + frame status 4B
                stream->callback.EntryPara.in_channel_num   = 2;
                stream->callback.EntryPara.in_sampling_rate = source->param.bt_dl.scenario_param.usb_out_broadcast_param.bt_in_param.codec_param.lc3.sample_rate/1000;
            }
        }
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
        else
        {

        }
    }
#endif /* MTK_AUDIO_BT_COMMON_ENABLE */
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    else if (source->param.audio.channel_num>0)
    {
        stream->callback.EntryPara.in_size            = source->param.audio.frame_size*2;
        stream->callback.EntryPara.in_channel_num     = (source->param.audio.echo_reference)
                                                        ? source->param.audio.channel_num+1
                                                        : source->param.audio.channel_num;
        if (source->param.audio.mem_handle.memory_select==HAL_AUDIO_MEMORY_UL_AWB2) { // Echo path only
            stream->callback.EntryPara.in_channel_num = 2;
        }
        stream->callback.EntryPara.in_sampling_rate   = (source->param.audio.src!=NULL) //Source VDM SRC
                                                          ? DSP_SRCOutRateChange2Value(DSP_GetSRCOutRate(source->param.audio.src->src_ptr))/1000
                                                          : source->param.audio.rate/1000;//AudioSourceSamplingRate_Get();
    }
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
    /*///////////////////////////////////////////*/


    /* Configure Callback para  by Sink type */
    if(sink->type == SINK_TYPE_N9SCO)
    {
#ifdef MTK_BT_HFP_ENABLE
        stream->callback.EntryPara.out_channel_num         = stream->callback.EntryPara.in_channel_num;
        stream->callback.EntryPara.codec_out_size          = 1000; //480; Clk skew temp
        stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_16K;
#ifdef AIR_SOFTWARE_GAIN_ENABLE
        Call_UL_SW_Gain_Init(sink);
#endif
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
        SCO_UL_Fix_Sample_Rate_Init();
#endif
#endif
    }
#ifdef AIR_BT_CODEC_BLE_ENABLED
    else if (sink->type == SINK_TYPE_N9BLE)
    {
        stream->callback.EntryPara.out_channel_num         = stream->callback.EntryPara.in_channel_num;
        stream->callback.EntryPara.codec_out_size          = 1000; //480; Clk skew temp
        if(sink->param.n9ble.share_info_base_addr->sample_rate == 16000)
        {
            stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_16K;
        }
        else if(sink->param.n9ble.share_info_base_addr->sample_rate == 24000)
        {
            stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_24K;
        }
        else if(sink->param.n9ble.share_info_base_addr->sample_rate == 32000)
        {
            stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_32K;
        }
        else if(sink->param.n9ble.share_info_base_addr->sample_rate == 44100)
        {
            stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_44_1K;
        }
        else if(sink->param.n9ble.share_info_base_addr->sample_rate == 48000)
        {
            stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_48K;
        }
#ifdef AIR_SOFTWARE_GAIN_ENABLE
    Call_UL_SW_Gain_Init(sink);
#endif
#if defined(AIR_BLE_SWB_ENABLE) && defined(AIR_FIXED_RATIO_SRC)
        N9Ble_UL_SWB_Sample_Rate_Init();
#endif
#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
        N9Ble_UL_Fix_Sample_Rate_Init();
#endif
    }
#endif
    else if (sink->type == SINK_TYPE_DSP_VIRTUAL)
    {
        stream->callback.EntryPara.out_channel_num         = 1;
        stream->callback.EntryPara.codec_out_size          = sink->param.virtual_para.mem_size;
        stream->callback.EntryPara.codec_out_sampling_rate = FS_RATE_16K;
    }
    else if (sink->type == SINK_TYPE_MEMORY)
    {
        stream->callback.EntryPara.out_channel_num     = 1;
        stream->callback.EntryPara.codec_out_size          = 512;
        stream->callback.EntryPara.codec_out_sampling_rate = FS_RATE_16K;
    }
    else if (sink->type == SINK_TYPE_CM4RECORD)
    {
        stream->callback.EntryPara.out_channel_num         = stream->callback.EntryPara.in_channel_num;
        stream->callback.EntryPara.codec_out_size          = MAX(stream->callback.EntryPara.in_size,512);
        stream->callback.EntryPara.codec_out_sampling_rate = stream->callback.EntryPara.in_sampling_rate;
    }
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    else if ((sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MAX))
    {
        if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE)
        {

    }
        #if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE)
        {
            if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_DONGLE_USB_OUT)
            {
                // TODO: usb out
                stream->callback.EntryPara.out_channel_num          = 2;
                stream->callback.EntryPara.codec_out_size           = 720;
                /* set codec_out_sampling_rate by opus decoder for SRC */
                stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_16K;
            } else if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)
            {
                stream->callback.EntryPara.out_channel_num         = stream->callback.EntryPara.in_channel_num;
                stream->callback.EntryPara.codec_out_size          = 1000; //480; Clk skew temp
                stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_16K;
            }
        }
        #endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE)
        {
            if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_VOICE_USB_OUT)
            {
                stream->callback.EntryPara.out_channel_num          = 2;
                stream->callback.EntryPara.codec_out_size           = 2*20*48*sizeof(int16_t);
                stream->callback.EntryPara.codec_out_sampling_rate  = sink->param.data_ul.scenario_param.usb_out_broadcast_param.usb_out_param.codec_param.pcm.sample_rate/1000;
            }
        }
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
        else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_GSENSOR)
        {

        }
        else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_MULTI_MIC_STREAM)
        {

        }
        #if defined(AIR_WIRED_AUDIO_ENABLE)
        else if (sink->param.data_ul.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO)
        {
            if (sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_OUT)
            {
                frameSize = sink->param.data_ul.max_payload_size;
            }
        }
        #endif
        else
        {

        }
    }
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */
#ifdef MTK_AUDIO_BT_COMMON_ENABLE
    else if ((sink->type >= SINK_TYPE_BT_COMMON_MIN) && (sink->type <= SINK_TYPE_BT_COMMON_MAX))
    {
        if (sink->param.bt_ul.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE)
        {

        }
        #if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
        else if (sink->param.bt_ul.scenario_type == AUDIO_TRANSMITTER_GAMING_MODE)
        {
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_MUSIC_DONGLE_USB_IN_1))
            {
                // TODO: bt out
                stream->callback.EntryPara.out_channel_num          = 2;
                stream->callback.EntryPara.codec_out_size           = 6*48*2*2;
                /* set codec_out_sampling_rate to usb in sampling rate for SRC */
                stream->callback.EntryPara.codec_out_sampling_rate  = FS_RATE_48K;
            }
        }
        #endif /* MTK_GAMING_MODE_HEADSET || AIR_GAMING_MODE_DONGLE_ENABLE */
        #if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
        else if (sink->param.bt_ul.scenario_type == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE)
        {
            if ((sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_0) ||
                (sink->param.bt_ul.scenario_sub_id == AUDIO_TRANSMITTER_BLE_AUDIO_DONGLE_MUSIC_USB_IN_1))
            {
                stream->callback.EntryPara.out_channel_num          = 2;
                stream->callback.EntryPara.codec_out_size           = sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.codec_param.lc3.frame_size;
                stream->callback.EntryPara.codec_out_sampling_rate  = sink->param.bt_ul.scenario_param.usb_in_broadcast_param.bt_out_param.codec_param.lc3.sample_rate/1000;
            }
        }
        #endif /* AIR_BLE_AUDIO_DONGLE_ENABLE */
        else
        {

        }
    }
#endif /* MTK_AUDIO_BT_COMMON_ENABLE */
    else if ((U32)(stream->callback.FeatureTablePtr->MemPtr) != 0)
    {
        stream->callback.EntryPara.codec_out_size             = (U32)(stream->callback.FeatureTablePtr->MemPtr);
        stream->callback.EntryPara.codec_out_sampling_rate    = (source->type == SOURCE_TYPE_AUDIO || source->type == SOURCE_TYPE_AUDIO2)
                                                                    ? stream->callback.EntryPara.in_sampling_rate *
                                                                      stream->callback.EntryPara.codec_out_size /
                                                                      stream->callback.EntryPara.in_size
                                                                    : stream->callback.EntryPara.in_sampling_rate;

        if((sink->type == SINK_TYPE_AUDIO) || (sink->type == SINK_TYPE_VP_AUDIO) || (sink->type == SINK_TYPE_DSP_JOINT) || (sink->type == SINK_TYPE_AUDIO_DL3) || (sink->type == SINK_TYPE_AUDIO_DL12)) {
            stream->callback.EntryPara.out_channel_num = sink->param.audio.channel_num;
            frameSize = sink->param.audio.frame_size;
#if defined(AIR_BLE_SWB_ENABLE) && defined(AIR_FIXED_RATIO_SRC)
#include "common.h"
             if(source->param.n9ble.context_type == BLE_CONTEXT_CONVERSATIONAL){ //for call mode only
                 N9Ble_DL_SWB_Sample_Rate_Init();
             }
#endif

#ifdef MTK_TDM_ENABLE
        } else if(sink->type == SINK_TYPE_TDMAUDIO) {
            stream->callback.EntryPara.out_channel_num = sink->param.audio.channel_sel;
            frameSize = sink->param.audio.frame_size;
#endif
        } else {
            stream->callback.EntryPara.out_channel_num = 2;

        }

    }
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    else if (sink->param.audio.channel_num>0)
    {
        stream->callback.EntryPara.out_channel_num            = sink->param.audio.channel_num;

        //setting by codec and application
        stream->callback.EntryPara.codec_out_size             = sink->param.audio.frame_size;//////
        stream->callback.EntryPara.codec_out_sampling_rate    = stream->callback.EntryPara.in_sampling_rate; /////

    }
#endif /* AIR_AUDIO_HARDWARE_ENABLE */

    if(((sink->type == SINK_TYPE_AUDIO) || (sink->type == SINK_TYPE_VP_AUDIO) || (sink->type == SINK_TYPE_AUDIO_DL3) || (sink->type == SINK_TYPE_AUDIO_DL12))
        && ((sink->param.audio.audio_device == HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_L)
         || (sink->param.audio.audio_device == HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_R))) {
        stream->callback.EntryPara.device_out_channel_num = 1;

    } else {
        stream->callback.EntryPara.device_out_channel_num = stream->callback.EntryPara.out_channel_num;

    }

    if ((sink->type == SINK_TYPE_AUDIO) || (sink->type == SINK_TYPE_VP_AUDIO) || (sink->type == SINK_TYPE_AUDIO_DL3) || (sink->type == SINK_TYPE_AUDIO_DL12)) {
        stream->callback.EntryPara.software_handled_channel_num = sink->param.audio.sw_channels;

    } else {
        stream->callback.EntryPara.software_handled_channel_num = stream->callback.EntryPara.out_channel_num;

    }

    stream->callback.EntryPara.encoder_out_size               = (stream->callback.EntryPara.number.field.feature_number>=2)
        ?((stream_feature_ptr_t)(stream->callback.FeatureTablePtr+stream->callback.EntryPara.number.field.feature_number - 2))->codec_output_size
        :((stream_feature_ptr_t)(stream->callback.FeatureTablePtr))->codec_output_size;

    stream->callback.Src.out_frame_size  = (stream->callback.EntryPara.with_src==0)
                                                ? 0
                                                : stream->callback.Src.out_frame_size;

    /*///////////////////////////////////////////*/

    stream->callback.EntryPara.out_malloc_size = MAX(MAX(MAX(stream->callback.EntryPara.codec_out_size,
                                                             stream->callback.EntryPara.encoder_out_size),
                                                             stream->callback.Src.out_frame_size),
                                                             frameSize);

    //Source VDM SRC
    if (source->type==SOURCE_TYPE_AUDIO || source->type==SOURCE_TYPE_DSP_BRANCH || source->type==SOURCE_TYPE_AUDIO2)
        if (source->param.audio.src!=NULL)
            stream->callback.EntryPara.in_size = stream->callback.EntryPara.out_malloc_size;
    #ifdef MTK_SENSOR_SOURCE_ENABLE
        if (source->type==SOURCE_TYPE_GSENSOR) {
            stream->callback.EntryPara.in_channel_num = 1;
            stream->callback.EntryPara.in_size = 384;
        }
    #endif
    stream->callback.EntryPara.in_malloc_size = stream->callback.EntryPara.in_size;
    DSP_MW_LOG_I("[DSP_RESOURCE] DSP stream in/out buf malloc", 0);
    if (stream->callback.FeatureTablePtr->FeatureType != CODEC_PCM_COPY) {
        /* malloc in_ptr */
        configASSERT(stream->callback.EntryPara.in_channel_num<=CALLBACK_INPUT_PORT_MAX_NUM);
        mallocSize = stream->callback.EntryPara.in_malloc_size*stream->callback.EntryPara.in_channel_num;
        mem_ptr = DSPMEM_tmalloc(stream->callback.EntryPara.DSPTask, mallocSize, stream);
        memset(mem_ptr, 0, mallocSize);
        for (i=0 ; i<stream->callback.EntryPara.in_channel_num ; i++)
        {
            stream->callback.EntryPara.in_ptr[i] = mem_ptr;
            mem_ptr += stream->callback.EntryPara.in_malloc_size;
        }


        /* malloc out_ptr */
        chNum = MAX(stream->callback.EntryPara.out_channel_num, 2);
        configASSERT(chNum<=CALLBACK_OUTPUT_PORT_MAX_NUM);
        mallocSize = stream->callback.EntryPara.out_malloc_size*chNum;
        mem_ptr = DSPMEM_tmalloc(stream->callback.EntryPara.DSPTask, mallocSize, stream);
        memset(mem_ptr, 0, mallocSize);
        for (i=0 ; i<chNum ; i++)
        {
            stream->callback.EntryPara.out_ptr[i]= mem_ptr;
            mem_ptr += stream->callback.EntryPara.out_malloc_size;
        }
     } else {
        //CODEC_PCM_COPY
        chNum = MAX(MAX(stream->callback.EntryPara.out_channel_num, 1), stream->callback.EntryPara.in_channel_num);
        configASSERT(chNum<=MIN(CALLBACK_INPUT_PORT_MAX_NUM, CALLBACK_OUTPUT_PORT_MAX_NUM));
        frameSize = MAX(stream->callback.EntryPara.in_malloc_size, stream->callback.EntryPara.out_malloc_size);
        stream->callback.EntryPara.in_malloc_size = stream->callback.EntryPara.out_malloc_size = frameSize;
        stream->callback.EntryPara.out_channel_num = chNum;

        mallocSize = frameSize*chNum;
        if ((frameSize & 3) && (chNum > 1))
        {
            DSP_MW_LOG_I("[DSP] Unaligned Callback Frame Size:%d!!", 1, frameSize);
        }

#ifdef AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
        if(source->type == SOURCE_TYPE_CM4_VP_PLAYBACK){
            mem_ptr = g_vp_memptr;
            memset(mem_ptr, 0, VP_FRAMEWORK_MEM_SIZE);
        }else{
            mem_ptr = DSPMEM_tmalloc(stream->callback.EntryPara.DSPTask, mallocSize, stream);
            memset(mem_ptr, 0, mallocSize);
        }
#else
        mem_ptr = DSPMEM_tmalloc(stream->callback.EntryPara.DSPTask, mallocSize, stream);
        memset(mem_ptr, 0, mallocSize);
#endif
        for (i=0 ; i<chNum ; i++)
        {
            stream->callback.EntryPara.out_ptr[i]= mem_ptr;
            stream->callback.EntryPara.in_ptr[i] = mem_ptr;
            mem_ptr += frameSize;
        }
        DSP_MW_LOG_I("[DSP] Callback stream Setup codec is CODEC_PCM_COPY, Frame Size:%d, channel_num:%d", 2, frameSize, chNum);
    }
    #ifdef MTK_AUDIO_TRANSMITTER_ENABLE
    if ((source->type >= SOURCE_TYPE_AUDIO_TRANSMITTER_MIN) && (source->type <= SOURCE_TYPE_AUDIO_TRANSMITTER_MAX))
    {
        if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_A2DP_SOURCE)
        {

        }
        #if defined(AIR_WIRED_AUDIO_ENABLE)
        else if (source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO)
        {
            if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) ||
                (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))
            {
                stream->callback.EntryPara.in_malloc_size = stream->callback.EntryPara.in_size;
            }
        }
        #endif /* AIR_WIRED_AUDIO_ENABLE */
    }
    #endif /* MTK_AUDIO_TRANSMITTER_ENABLE */

    stream->callback.EntryPara.number.field.process_sequence  = 0;
    stream->callback.EntryPara.number.field.source_type       = (U8)stream->source->type;
    stream->callback.EntryPara.number.field.sink_type         = (U8)stream->sink->type;
    stream->callback.EntryPara.with_encoder                   = FALSE;

    //stream->callback.EntryPara.src_out_size = stream->callback.EntryPara.codec_out_size;
    stream->callback.EntryPara.src_out_sampling_rate =  stream->callback.EntryPara.codec_out_sampling_rate;
}

VOID DSP_Callback_TrigerSourceSRC(SOURCE source, U32 process_length)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    U16 thd_size;
    if (source->type!=SOURCE_TYPE_AUDIO && source->type!=SOURCE_TYPE_DSP_BRANCH && source->type!=SOURCE_TYPE_AUDIO2)
        return;
    if (source->param.audio.src == NULL)
        return;
    thd_size = (U16)DSP_GetSRCOutFrameSize(source->param.audio.src->src_ptr);
    source->param.audio.src->accum_process_size += process_length;
    while (source->param.audio.src->accum_process_size >= thd_size)
    {
        Sink_Audio_Triger_SourceSRC(source);
        source->param.audio.src->accum_process_size -= thd_size;
    }
#else
    UNUSED(source);
    UNUSED(process_length);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

VOID DSP_Callback_TrigerSinkSRC(SINK sink, U32 process_length)
{
#ifdef AIR_AUDIO_HARDWARE_ENABLE
    U16 thd_size;
    if (sink->type!=SINK_TYPE_AUDIO &&
        sink->type!=SINK_TYPE_VP_AUDIO &&
        sink->type!=SINK_TYPE_DSP_JOINT)
        return;
    if (sink->param.audio.src == NULL)
        return;
    thd_size = (U16)DSP_GetSRCInFrameSize(sink->param.audio.src->src_ptr);
    sink->param.audio.src->accum_process_size += process_length;
    while (sink->param.audio.src->accum_process_size >= thd_size)
    {
        Source_Audio_Triger_SinkSRC(sink);
        sink->param.audio.src->accum_process_size -= thd_size;
    }
#else
    UNUSED(sink);
    UNUSED(process_length);
#endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DSP_Callback_CheckSkipProcess(VOID *ptr)
{
    DSP_CALLBACK_HANDLER_PTR handler = ptr;
    handler->stream->callback.EntryPara.skip_process = 0;

    U16 *callbackOutSizePtr;
    callbackOutSizePtr = (handler->stream->callback.EntryPara.with_encoder==TRUE)
                           ? &(handler->stream->callback.EntryPara.encoder_out_size)
                           : (handler->stream->callback.EntryPara.with_src)
                               ? &(handler->stream->callback.EntryPara.src_out_size)
                               : &(handler->stream->callback.EntryPara.codec_out_size);


    if ((handler->handlingStatus == CALLBACK_HANDLER)||
        (handler->handlingStatus == CALLBACK_ZEROPADDING))
    {
        if (*callbackOutSizePtr > (U16)SinkSlack(handler->stream->sink))
        {
            handler->stream->callback.EntryPara.in_size = 0;
            *callbackOutSizePtr = 0;
            handler->stream->callback.EntryPara.skip_process = DSP_CALLBACK_SKIP_ALL_PROCESS;
        }
        else if (handler->stream->callback.EntryPara.in_size==0)
        {
            //if ((handler->stream->callback.FeatureTablePtr->ProcessEntry!=MP3_Decoder) &&
            //    (handler->stream->callback.FeatureTablePtr->ProcessEntry!=stream_codec_decoder_sbc_process) &&
            //    (handler->stream->callback.FeatureTablePtr->ProcessEntry!=stream_codec_decoder_aac_process))
            if (handler->stream->callback.FeatureTablePtr->ProcessEntry!=stream_pcm_copy_process)
            {
                #ifdef MTK_CELT_DEC_ENABLE
                if(handler->stream->callback.FeatureTablePtr->ProcessEntry!=stream_codec_decoder_celt_process)
                #endif
                    handler->stream->callback.EntryPara.skip_process = CODEC_ALLOW_SEQUENCE;
            }
            #ifdef AIR_BT_CODEC_BLE_V2_ENABLED
            if (handler->stream->callback.FeatureTablePtr->ProcessEntry == stream_codec_decoder_lc3_v2_process)
            {
                handler->stream->callback.EntryPara.skip_process = 0;
            }
            #endif /* AIR_BT_CODEC_BLE_V2_ENABLED */
        }
    }
    else if (handler->handlingStatus == CALLBACK_BYPASS_CODEC)
    {
        U32 byte_per_sample;

        byte_per_sample = (handler->stream->callback.EntryPara.resolution.feature_res == RESOLUTION_32BIT)
                            ? 4
                            : 2;
        //if (handler->stream->callback.FeatureTablePtr->ProcessEntry !=stream_codec_decoder_sbc_process)
        {
    	    handler->stream->callback.EntryPara.skip_process = CODEC_ALLOW_SEQUENCE;
            *callbackOutSizePtr = SourceSize(handler->stream->source)*byte_per_sample;
            handler->stream->callback.EntryPara.pre_codec_out_size = *callbackOutSizePtr;
        }
    }

    handler->stream->callback.EntryPara.force_resume = FALSE;
}


ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DSP_Callback_CheckForceResumeValid(VOID *ptr)
{
    DSP_CALLBACK_HANDLER_PTR handler = ptr;
    U16 *callbackOutSizePtr;
    callbackOutSizePtr = (handler->stream->callback.EntryPara.with_encoder==TRUE)
                           ? &(handler->stream->callback.EntryPara.encoder_out_size)
                           : (handler->stream->callback.EntryPara.with_src)
                               ? &(handler->stream->callback.EntryPara.src_out_size)
                               : &(handler->stream->callback.EntryPara.codec_out_size);

    if (handler->stream->callback.EntryPara.force_resume)
    {
        if ((handler->handlingStatus == CALLBACK_HANDLER)||
            (handler->handlingStatus == CALLBACK_ZEROPADDING))
        {
            if (*callbackOutSizePtr > (U16)SinkSlack(handler->stream->sink))
            {
                handler->stream->callback.EntryPara.force_resume = FALSE;
            }
        }
    }
}


/**
 * DSP_Callback_DropFlushData
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DSP_Callback_DropFlushData(DSP_STREAMING_PARA_PTR stream)
{
    U8 *bufptr;
    U16 callbackOutSize;
    U16 sinkSize, remainSize;
    U16 i;
    callbackOutSize = (stream->callback.EntryPara.with_encoder==TRUE)
                           ? (stream->callback.EntryPara.encoder_out_size)
                           : (stream->callback.EntryPara.with_src)
                               ? (stream->callback.EntryPara.src_out_size)
                               : (stream->callback.EntryPara.codec_out_size);

#ifdef MTK_LEAKAGE_DETECTION_ENABLE
    extern bool CM4_Record_leakage_enable;
    if (CM4_Record_leakage_enable && (stream->sink->type == SINK_TYPE_CM4RECORD)) {
        callbackOutSize = 0;
    }
#endif
#ifdef MTK_USER_TRIGGER_FF_ENABLE
    extern bool utff_enable;
    if (utff_enable && (stream->sink->type == SINK_TYPE_CM4RECORD)) {
        callbackOutSize = 0;
    }
#endif

    //Sink Flush
    // printf("callbackOutSize: %d\r\n", callbackOutSize);
    if (callbackOutSize != 0)
    {
        #if 0
        #if 1
        configASSERT(((U16)callbackOutSize <= stream->callback.EntryPara.out_malloc_size));
        while((U16)callbackOutSize > (U16)SinkSlack(stream->sink))
        {
            vTaskSuspend(stream->callback.EntryPara.DSPTask);
            portYIELD();
        }
        #endif

        bufptr = ((stream->callback.EntryPara.out_channel_num==1) ||
                  (stream->callback.EntryPara.with_encoder==TRUE))
                   ? stream->callback.EntryPara.out_ptr[0]
                   : NULL;
        // printf("SinkWriteBuf++\r\n");
        SinkWriteBuf(stream->sink, bufptr, (U32)callbackOutSize);
        // printf("SinkWriteBuf--\r\n");
        SinkFlush(stream->sink, (U32)callbackOutSize);
        #else
        //partially flush
        configASSERT(((U16)callbackOutSize <= stream->callback.EntryPara.out_malloc_size));
        remainSize = callbackOutSize;
        #if 1
        while(remainSize>0)
        {
            while((U16)SinkSlack(stream->sink)==0)
            {
                if ((stream->streamingStatus == STREAMING_END)
#ifdef MTK_BT_HFP_ENABLE
                    || ((ScoDlStopFlag == TRUE)&&(stream->source->type == SOURCE_TYPE_N9SCO))
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
                    || ((BleDlStopFlag == TRUE)&&(stream->source->type == SOURCE_TYPE_N9BLE))
#endif
                    || (stream->sink->type == SINK_TYPE_CM4RECORD)
                    )
                {
                    if(stream->sink->type == SINK_TYPE_CM4RECORD){
                        DSP_MW_LOG_I("Callback_DropFlush sink_type(%d) flush again", 1, stream->sink->type);
                        SinkFlush(stream->sink, 0);
                        if (stream->callback.EntryPara.in_size != 0)
                        {
                            DSP_MW_LOG_I("Callback_DropFlush source drop size(%d)", 1, stream->callback.EntryPara.in_size);
                            SourceDrop(stream->source, stream->callback.EntryPara.in_size);
                        }
                    }
                    return;
                }
                vTaskSuspend(stream->callback.EntryPara.DSPTask);
            }

         #else//Machi test code
         if(remainSize>0)
         {
         #endif
            sinkSize = (U16)SinkSlack(stream->sink);
            if (remainSize<=(U16)sinkSize)
            {
                sinkSize = remainSize;
            }

            #if 0
            bufptr = ((stream->callback.EntryPara.out_channel_num==1) ||
                      (stream->callback.EntryPara.with_encoder==TRUE))
                       ? stream->callback.EntryPara.out_ptr[0]
                       : NULL;
            #else
            bufptr = ((stream->sink->type == SINK_TYPE_AUDIO) ||
                      (stream->sink->type == SINK_TYPE_VP_AUDIO) ||
#ifdef MTK_TDM_ENABLE
                      (stream->sink->type == SINK_TYPE_TDMAUDIO) ||
#endif
                      (stream->sink->type == SINK_TYPE_DSP_JOINT) ||
                      (stream->sink->type == SINK_TYPE_AUDIO_DL3) ||
                      (stream->sink->type == SINK_TYPE_AUDIO_DL12))
                       ? NULL
                       : stream->callback.EntryPara.out_ptr[0];
            #endif

            SinkWriteBuf(stream->sink, bufptr, (U32)sinkSize);
            SinkFlush(stream->sink, (U32)sinkSize);

            //SRC vector mode trigger
            DSP_Callback_TrigerSinkSRC(stream->sink, (U32)sinkSize);

            remainSize-=sinkSize;
            if (remainSize>0)
            {
                for(i=0 ; i<stream->callback.EntryPara.out_channel_num ; i++)
                {
                    memcpy(stream->callback.EntryPara.out_ptr[i],
                           (U8*)stream->callback.EntryPara.out_ptr[i]+sinkSize,
                           remainSize);
                }
            }
        }
        #endif
    }
    //Source Drop
    if (stream->callback.EntryPara.in_size != 0)
    {
        SourceDrop(stream->source, stream->callback.EntryPara.in_size);
        //SRC vector mode trigger
        DSP_Callback_TrigerSourceSRC(stream->source, (U32)stream->callback.EntryPara.in_size);
    }
}

/**
 * DSP_Callback_ChangeStatus
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DSP_Callback_ChangeStatus(VOID *ptr)
{
    DSP_CALLBACK_HANDLER_PTR handler = ptr;
    if (handler->handlingStatus == handler->stream->callback.Status)
    {
        handler->stream->callback.Status = handler->nextStatus;
    }
}

/**
 * DSP_Callback_TransformHandle
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DSP_Callback_TransformHandle(VOID *ptr)
{
    DSP_STREAMING_PARA_PTR stream = ptr;
    #if 0
    U16 sinkFlushSize;
    sinkFlushSize = (stream->callback.EntryPara.with_encoder==TRUE)
                      ? stream->callback.EntryPara.encoder_out_size
                      : (stream->callback.EntryPara.with_src)
                          ? stream->callback.EntryPara.src_out_size
                          : stream->callback.EntryPara.codec_out_size;
    if ((stream->callback.Status==CALLBACK_SUSPEND) &&
        (stream->callback.EntryPara.in_size!=0) &&
        (sinkFlushSize!=0))
    #endif
    stream->source->transform->Handler(stream->source, stream->sink);

    if (stream->source->type == SOURCE_TYPE_DSP_BRANCH)
    {
        stream->source->param.dsp.transform->Handler(stream->source->param.dsp.transform->source,
                                                     stream->source->param.dsp.transform->sink);
    }
    if (stream->sink->type == SINK_TYPE_DSP_JOINT)
    {
        stream->sink->param.dsp.transform->Handler(stream->sink->param.dsp.transform->source,
                                                   stream->sink->param.dsp.transform->sink);
    }
}

/**
 * DSP_Callback_Processing
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM VOID DSP_Callback_Processing(DSP_STREAMING_PARA_PTR stream)
{
    SOURCE source = stream->source;
    //SINK sink  = stream->sink;
    DSP_CALLBACK_HANDLER handler;
    U8 *bufptr;
    U16 *callbackOutSizePtr;
    BOOL callbackResult=FALSE;
    handler.stream = stream;
    handler.handlingStatus = stream->callback.Status;

    callbackOutSizePtr = (stream->callback.EntryPara.with_encoder==TRUE)
                           ? &(stream->callback.EntryPara.encoder_out_size)
                           : (stream->callback.EntryPara.with_src)
                               ? &(stream->callback.EntryPara.src_out_size)
                               : &(stream->callback.EntryPara.codec_out_size);

    if (handler.handlingStatus == CALLBACK_HANDLER)
    {
        stream->callback.EntryPara.in_size = SourceSize(source);

        if ((stream->callback.EntryPara.in_size > stream->callback.EntryPara.in_malloc_size))
            stream->callback.EntryPara.in_size = stream->callback.EntryPara.in_malloc_size;

        if (stream->callback.EntryPara.in_size != 0)
        {
            bufptr = (stream->callback.EntryPara.in_channel_num==1)
                       ? stream->callback.EntryPara.in_ptr[0]
                       : NULL;
            SourceReadBuf(source, bufptr, stream->callback.EntryPara.in_size);
        }
        else
        {
            DSP_MW_LOG_I("Callback meet source size 0", 0);
        }

#ifdef MTK_GAMING_MODE_HEADSET
        if (! ((stream->sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (stream->sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MAX) && (stream->sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)))
        {
#endif
            if (stream->streamingStatus == STREAMING_START || stream->streamingStatus == STREAMING_DEINIT)
            {
                stream->streamingStatus = STREAMING_ACTIVE;
                handler.handlingStatus = CALLBACK_INIT;
            }
#ifdef MTK_GAMING_MODE_HEADSET
        }
#endif
        if ((stream->streamingStatus == STREAMING_END) &&
            (stream->sink->type != SINK_TYPE_AUDIO)){
            handler.handlingStatus = CALLBACK_INIT;
        }
    }
    else if (handler.handlingStatus == CALLBACK_INIT)
    {
        stream->callback.EntryPara.in_size = 0;
    }
    else if (handler.handlingStatus == CALLBACK_ZEROPADDING)
    {
        stream->callback.EntryPara.in_size = 0;
    }
    else if (handler.handlingStatus == CALLBACK_BYPASS_CODEC)
    {
        #if A2DP_DBG_PORT
        hal_gpio_set_output(HAL_GPIO_30, 1);
        #endif
        stream->callback.EntryPara.in_size = SourceSize(source);
        if (stream->streamingStatus == STREAMING_START)
        {
            stream->streamingStatus = STREAMING_ACTIVE;
        }
        configASSERT(stream->callback.EntryPara.out_malloc_size >= SourceSize(source));

    }
    DSP_Callback_CheckSkipProcess(&handler);

    if (!(stream->callback.EntryPara.skip_process==DSP_CALLBACK_SKIP_ALL_PROCESS))
    {
        callbackResult = DSP_CallbackEntryTable[handler.handlingStatus](&(stream->callback.EntryPara), stream->callback.FeatureTablePtr);
    }

    switch (handler.handlingStatus)
    {
        case CALLBACK_MALLOC:
            handler.nextStatus = CALLBACK_INIT;
            break;
        case CALLBACK_INIT:
            DSP_Callback_ResolutionConfig(stream);
            //DriftConfiguration(stream, TRUE);
            handler.nextStatus = CALLBACK_SUSPEND;
            break;
        case CALLBACK_BYPASS_CODEC:
		case CALLBACK_ZEROPADDING:
            //DriftConfiguration(stream, FALSE);
        case CALLBACK_HANDLER:
            // printf("DSP_Callback_DropFlushData++\r\n");
            DSP_Callback_DropFlushData(stream);
            // printf("DSP_Callback_DropFlushData--\r\n");
            handler.nextStatus = CALLBACK_SUSPEND;

            if (callbackResult)
            {
                if (handler.handlingStatus == CALLBACK_ZEROPADDING)
                {
                    handler.nextStatus = CALLBACK_WAITEND;
                }
            }
            //DriftCompensation(&(stream->driftCtrl.para));
            break;
        case CALLBACK_SUSPEND:
        case CALLBACK_DISABLE:
        case CALLBACK_WAITEND:
            return;
    }

    DSP_Callback_CheckForceResumeValid(&handler);

    #if A2DP_DBG_PORT
    hal_gpio_set_output(HAL_GPIO_30, 0);
    #endif
    U32 mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    DSP_Callback_ChangeStatus(&handler);
    #if 1
    if ((((stream->callback.Status==CALLBACK_SUSPEND) &&
          ((stream->callback.EntryPara.in_size!=0) || (*callbackOutSizePtr!=0) || (stream->callback.EntryPara.force_resume)) &&
          (stream->source->type != SOURCE_TYPE_N9SCO) &&
          (stream->sink->type != SINK_TYPE_N9SCO) &&
#ifdef AIR_BT_CODEC_BLE_ENABLED
          (stream->source->type != SOURCE_TYPE_N9BLE) &&
          (stream->sink->type != SINK_TYPE_N9BLE) &&
#endif
#ifdef MTK_GAMING_MODE_HEADSET
          (!((stream->source->type == SOURCE_TYPE_AUDIO) && (stream->sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (stream->sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MAX) && (stream->sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET))) &&
          (!(stream->source->type == SOURCE_TYPE_A2DP && stream->source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT && (afe_get_dl1_query_data_amount()>=124))) &&
#endif

          ((stream->source->type != SOURCE_TYPE_A2DP)||(stream->sink->type != SINK_TYPE_AUDIO)||((Sink_blks[SINK_TYPE_VP_AUDIO]==NULL)
          #ifdef AIR_AUDIO_HARDWARE_ENABLE
          || (!stream_audio_check_sink_remain_enough(stream->sink))
          #endif
          ))) ||
#ifdef AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE
         ((handler.handlingStatus==CALLBACK_INIT) && ((stream->source->type == SOURCE_TYPE_A2DP) || ((stream->source->type == SOURCE_TYPE_CM4_VP_PLAYBACK)||(stream->source->type == SOURCE_TYPE_CM4_VP_DUMMY_SOURCE_PLAYBACK))))) &&
#else
         ((handler.handlingStatus==CALLBACK_INIT) && ((stream->source->type == SOURCE_TYPE_A2DP) || (stream->source->type == SOURCE_TYPE_CM4_VP_PLAYBACK)))) &&
#endif
        (stream->streamingStatus != STREAMING_END))
        {
            DSP_Callback_TransformHandle(stream);
        }
    #endif
    hal_nvic_restore_interrupt_mask(mask);
}

