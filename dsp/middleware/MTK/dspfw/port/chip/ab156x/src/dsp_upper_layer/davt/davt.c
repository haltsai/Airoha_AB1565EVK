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
#include "dsp_interface.h"
#include "dsp_memory.h"
#include "transform.h"
#include "audio_config.h"
#include "stream_audio.h"
#include "stream.h"
#include "davt.h"
#include "dsp_feature_interface.h"
#include "dsp_drv_dfe.h"
#include "dsp_update_para.h"
#include "hal_gpt.h"
#include "long_term_clk_skew.h"
#include "audio_afe_common.h"
#include "stream_n9_a2dp.h"

#if 0 /* Yo Eliminated */
U32 CODE array32_96k2k[96] =
{
	0x00000000,0x10B5150F,0x2120FB83,0x30FBC54D,0x40000000,0x4DEBE4FE,0x5A82799A,0x658C9A2D,0x6ED9EBA1,0x7641AF3D,0x7BA3751D,0x7EE7AA4C,0x7FFFFFFF,0x7EE7AA4C,0x7BA3751D,0x7641AF3D,
    0x6ED9EBA1,0x658C9A2D,0x5A82799A,0x4DEBE4FE,0x40000000,0x30FBC54D,0x2120FB83,0x10B5150F,0x00000000,0xEF4AEAF1,0xDEDF047D,0xCF043AB3,0xC0000000,0xB2141B02,0xA57D8666,0x9A7365D3,
    0x9126145F,0x89BE50C3,0x845C8AE3,0x811855B4,0x80000000,0x811855B4,0x845C8AE3,0x89BE50C3,0x9126145F,0x9A7365D3,0xA57D8666,0xB2141B02,0xC0000000,0xCF043AB3,0xDEDF047D,0xEF4AEAF1,
    0x00000000,0x10B5150F,0x2120FB83,0x30FBC54D,0x40000000,0x4DEBE4FE,0x5A82799A,0x658C9A2D,0x6ED9EBA1,0x7641AF3D,0x7BA3751D,0x7EE7AA4C,0x7FFFFFFF,0x7EE7AA4C,0x7BA3751D,0x7641AF3D,
    0x6ED9EBA1,0x658C9A2D,0x5A82799A,0x4DEBE4FE,0x40000000,0x30FBC54D,0x2120FB83,0x10B5150F,0x00000000,0xEF4AEAF1,0xDEDF047D,0xCF043AB3,0xC0000000,0xB2141B02,0xA57D8666,0x9A7365D3,
    0x9126145F,0x89BE50C3,0x845C8AE3,0x811855B4,0x80000000,0x811855B4,0x845C8AE3,0x89BE50C3,0x9126145F,0x9A7365D3,0xA57D8666,0xB2141B02,0xC0000000,0xCF043AB3,0xDEDF047D,0xEF4AEAF1,
};
#endif


/******************************************************************************
 * Function Declaration
 ******************************************************************************/
VOID                DSP_AV_Task                     (VOID);
VOID                DAVT_DefaultEntry               (VOID);
VOID                DAVT_TestEntry                  (VOID);
VOID                DAVT_StartEntry                 (VOID);
VOID                DAVT_ProcessEntry               (VOID);
VOID                DAVT_DeInitEntry                (VOID);
#if  DspIpcEnable
VOID                DAVT_CmdHdlr                    (DSP_CMD_PTR_t DspCmdPtr);
#endif
VOID                DAVT_Init                       (VOID);
VOID                DAVT_DeInit                     (VOID* para);
STATIC INLINE VOID  DAVT_Process                    (VOID);
VOID                DAVT_StreamingInit              (VOID);
BOOL                DAVT_ChkCallbackStatus          (VOID);
BOOL                DAVT_ChkCallbackStatusDisable   (VOID);
VOID                DAVT_SuspendRequest             (VOID* para);
VOID                DAVT_StreamingRateConfig        (SOURCE source, SINK sink);// Sink rate follows only now



/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/


/******************************************************************************
* External Variables
******************************************************************************/


/******************************************************************************
 * Type Definitions
 ******************************************************************************/
#if  DspIpcEnable
typedef VOID (*DAVT_CMD_HDLR) (DSP_CMD_PTR_t DspCmdPtr);
#endif
typedef VOID (*DAVT_ENTRY) (VOID);

/******************************************************************************
 * Constants
 ******************************************************************************/
#define NO_OF_AV_TASK_HANDLER (sizeof(DspAvTaskHdlr)/sizeof(DspAvTaskHdlr[0]))

#define NO_OF_AV_STREAM         3
#if  DspIpcEnable

DAVT_CMD_HDLR CODE DspAvTaskHdlr[] =
{
    NULL,
};
#endif
/******************************************************************************
 * Variables
 ******************************************************************************/
STATIC DAVT_ENTRY DAVT_Entry;

DSP_STREAMING_PARA VOLATILE AVStreaming[NO_OF_AV_STREAM];

// extern U32 globa2[8];

/**
 * DSP_AV_Task
 *
 * Main Process for DSP_AV_Task
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DSP_AV_Task(VOID)
{
	#if  DspIpcEnable
    DSP_CMD_PTR_t DspCmdPtr;
	#endif

	/* Do Initialization */
    DAVT_Init();
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    Audio_Default_setting_init();
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */

    while(1)
    {
    #if  DspIpcEnable
        if ((DspCmdPtr = osTaskGetIPCMsg()) != NULL)
        {
            DAVT_CmdHdlr(DspCmdPtr);
        }
	#endif
// hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &globa2[2]);
        DAVT_Entry();
// hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &globa2[3]);
        #ifdef AIR_BT_CLK_SKEW_ENABLE
        lt_clk_skew_notify_cm4();
        #endif

        PL_CRITICAL(DAVT_SuspendRequest ,NULL);

        //portYIELD();
    }
}

/**
 * DAVT_DefaultEntry
 *
 * Default Entry for DAVT
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DAVT_DefaultEntry(VOID)
{
    if (!DAVT_ChkCallbackStatusDisable())
        DAVT_Entry = DAVT_ProcessEntry;
}


/**
 * DAVT_TestEntry
 *
 * Test Entry for DAVT
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DAVT_TestEntry(VOID)
{
	return;
}

/**
 * DAVT_StartEntry
 *
 * Start Entry for DAVT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DAVT_StartEntry(VOID)
{
    DAVT_Process();
    if(DAVT_ChkCallbackStatus())
        DAVT_Entry = DAVT_ProcessEntry;
}

/**
 * DAVT_ProcessEntry
 *
 * Active Entry for DAVT background process
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DAVT_ProcessEntry(VOID)
{
    #if 0
    if (DAVT_ChkCallbackStatusDisable())
        DAVT_Entry = DAVT_DeInitEntry;
    else
    #endif
        DAVT_Process();
}

/**
 * DAVT_DeInitEntry
 *
 * Initialization Entry for DAVT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DAVT_DeInitEntry(VOID)
{
    PL_CRITICAL(DAVT_DeInit ,NULL);
}
#if  DspIpcEnable

/**
 * DAVT_CmdHdlr
 *
 * DAVT Command Handler handles all Commands towards DAVT task.
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DAVT_CmdHdlr(DSP_CMD_PTR_t DspCmdPtr)
{
    DSP_CMD_MSG_t DspMsg = DspCmdPtr->DspMsg;
	switch (DspMsg)
	{
        case DSP_MSG_START_LINE:
            DAVT_Entry = DAVT_TestEntry;
            break;
        case DSP_UPDATE_PARAMETER:
            DSP_UpdateStreamingPara(&DspCmdPtr->DspCmdPara.UpdatePara);
            break;
        default:
            break;
	}

	OSMEM_Put(DspCmdPtr);
}
#endif

/**
 * DAVT_Init
 *
 * Initialization for DSP_AV_Task
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DAVT_Init(VOID)
{
    DSPMEM_Init(DAV_TASK_ID);
    DAVT_Entry = DAVT_DefaultEntry;

    DSP_Callback_StreamingInit((DSP_STREAMING_PARA_PTR)&AVStreaming[0], NO_OF_AV_STREAM, DAV_TASK_ID);
}

/**
 * DAVT_DeInit
 *
 * De-Initialization for DAVT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DAVT_DeInit(VOID* para)
{
    UNUSED(para);
    DSPMEM_Flush(DAV_TASK_ID);
    DSP_Callback_StreamingInit((DSP_STREAMING_PARA_PTR)&AVStreaming[0], NO_OF_AV_STREAM, DAV_TASK_ID);
    DAVT_Entry = DAVT_DefaultEntry;
}


/**
 * DAVT_Process
 *
 * DAVT background process
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 *
 */
STATIC INLINE VOID DAVT_Process(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_AV_STREAM ; i++)
    {
        if (AVStreaming[i].streamingStatus == STREAMING_END){
            #ifdef AIR_AUDIO_HARDWARE_ENABLE
            if ((AVStreaming[i].sink->type == SINK_TYPE_AUDIO) &&
                (AVStreaming[i].sink->param.audio.irq_exist) &&
                (Audio_setting->Audio_sink.Zero_Padding_Cnt>0)) {
                //DSP_MW_LOG_E("DAVT wait zero padding:%d !!!", 1, Audio_setting->Audio_sink.Zero_Padding_Cnt);
            }
            else
            #endif /* AIR_AUDIO_HARDWARE_ENABLE */
            {
                StreamCloseAll(AVStreaming[i].source->transform, InstantCloseMode);
            }
        }
        else if(AVStreaming[i].source != NULL)
        {
            if (AVStreaming[i].source->type == SOURCE_TYPE_A2DP)
            {
                if ((AVStreaming[i].sink->param.audio.afe_wait_play_en_cnt == PLAY_EN_TRIGGER_REINIT_MAGIC_NUM))
                {
                    AVStreaming[i].sink->param.audio.afe_wait_play_en_cnt = PLAY_EN_REINIT_DONE_MAGIC_NUM;
                    #ifdef AIR_AUDIO_HARDWARE_ENABLE
                    afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN1, 0);
                    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
                    DSP_MW_LOG_E("AFE wait play en trigger re-sync", 0);
                    #ifdef MTK_BT_A2DP_ENABLE
                    Au_DL_send_reinit_request(MSG2_DSP2CN4_REINIT_AFE_ABNORMAL);
                    #endif 
                }
                if ((AVStreaming[i].callback.Status == CALLBACK_SUSPEND) || (AVStreaming[i].callback.Status == CALLBACK_WAITEND))
                {
                    AVStreaming[i].source->transform->Handler(AVStreaming[i].source, AVStreaming[i].sink);
                }
            }
        }
        DSP_Callback_Processing((DSP_STREAMING_PARA_PTR)&AVStreaming[i]);
    }
}



/**
 * DAVT_StreamingInit
 *
 * Initialization for DSP_AV_Task streaming
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
TaskHandle_t  DAVT_StreamingConfig(DSP_CALLBACK_STREAM_CONFIG_PTR stream_config_ptr)
{
    #if 0
    U8 i;
    if (isEnable)
    {
        for (i=0 ; i<NO_OF_AV_STREAM ; i++)
        {
            if ((AVStreaming[i].source == source) &&
                (AVStreaming[i].sink == sink))
            {
                return DAV_TASK_ID;
            }
        }
        for (i=0 ; i<NO_OF_AV_STREAM ; i++)
        {
            if (AVStreaming[i].streamingStatus == STREAMING_DISABLE)
            {
                DAVT_StreamingRateConfig(source,sink);
                AVStreaming[i].source                      = source;
                AVStreaming[i].sink                        = sink;
                AVStreaming[i].streamingStatus             = STREAMING_START;
                DSP_Callback_FeatureConfig((DSP_STREAMING_PARA_PTR)&AVStreaming[i], feature_list_ptr);
                return DAV_TASK_ID;
            }
        }
    }
    else
    {
        for (i=0 ; i<NO_OF_AV_STREAM ; i++)
        {
            if ((AVStreaming[i].streamingStatus != STREAMING_DISABLE) &&
                (AVStreaming[i].source == source) &&
                (AVStreaming[i].sink == sink))
            {
                AVStreaming[i].source                      = NULL;
                AVStreaming[i].sink                        = NULL;
                AVStreaming[i].callback.FeatureTablePtr    = NULL;
                AVStreaming[i].callback.Status             = CALLBACK_DISABLE;
                DSP_DRV_SRC_END(AVStreaming[i].callback.Src.src_ptr);
                AVStreaming[i].callback.Src.src_ptr        = NULL;
                AVStreaming[i].callback.EntryPara.with_src = FALSE;

                DSPMEM_Free(DAV_TASK_ID,(DSP_STREAMING_PARA_PTR)&AVStreaming[i]);
                AVStreaming[i].streamingStatus             = STREAMING_DISABLE;
                return DAV_TASK_ID;
            }
        }
    }
    return NULL_TASK_ID;
    #else
    stream_config_ptr->stream_ptr = (DSP_STREAMING_PARA_PTR)&AVStreaming[0];
    stream_config_ptr->stream_number = NO_OF_AV_STREAM;
    stream_config_ptr->task = DAV_TASK_ID;
    return DSP_Callback_StreamConfig(stream_config_ptr);
    #endif
}

/**
 * DAVT_Callback_Get
 *
 * Get DSP_AV_Task callback ptr
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
DSP_CALLBACK_PTR DAVT_Callback_Get(SOURCE source, SINK sink)
{
    U8 i;
    for (i=0 ; i<NO_OF_AV_STREAM ; i++)
    {
        if ((AVStreaming[i].streamingStatus != STREAMING_DISABLE) &&
            (AVStreaming[i].source == source) &&
            (AVStreaming[i].sink == sink))
        {
            return (DSP_CALLBACK_PTR)&AVStreaming[i].callback;
        }
    }
    return NULL;
}

/**
 * DAVT_ChkCallbackStatus
 *
 * Whether all DSP_AV_Task callback status SUSPEND/DISABLE
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL DAVT_ChkCallbackStatus(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_AV_STREAM ; i++)
    {
        if (((AVStreaming[i].callback.Status != CALLBACK_SUSPEND) &&
             (AVStreaming[i].callback.Status != CALLBACK_DISABLE) &&
             (AVStreaming[i].callback.Status != CALLBACK_WAITEND)) ||
            (AVStreaming[i].streamingStatus == STREAMING_END))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * DAVT_ChkCallbackStatusDisable
 *
 * Whether all DSP_AV_Task callback status DISABLE
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
BOOL DAVT_ChkCallbackStatusDisable(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_AV_STREAM ; i++)
    {
        if (AVStreaming[i].callback.Status != CALLBACK_DISABLE)
        {
            return FALSE;
        }
    }
    return TRUE;
}



/**
 * DAVT_SuspendRequest
 *
 * DAVT Suspend Request depend on callback status
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_1 VOID DAVT_SuspendRequest(VOID* para)
{
    UNUSED(para);
    if ((DAVT_Entry != DAVT_StartEntry) && (DAVT_Entry != DAVT_DeInitEntry) &&
        DAVT_ChkCallbackStatus())
    {
        vTaskSuspend(DAV_TASK_ID);
    }
}
/**
 * DAVT_StreeamingDeinitAll
 *
 * DAVT deinit all active stream
 *
 * @Author : Machi <MachiWu@airoha.com.tw>
 */
VOID DAVT_StreeamingDeinitAll(VOID)
{
        U8 i;
    for (i=0 ; i<NO_OF_AV_STREAM ; i++)
    {
        if (AVStreaming[i].streamingStatus == STREAMING_ACTIVE)
        {
            AVStreaming[i].streamingStatus = STREAMING_DEINIT;
        }
    }
}

