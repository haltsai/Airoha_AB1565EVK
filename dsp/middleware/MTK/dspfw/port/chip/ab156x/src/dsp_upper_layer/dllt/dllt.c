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

#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)

#include "config.h"
#include "dsp_task.h"
#include "dsp_interface.h"
#include "dsp_memory.h"
#include "transform.h"
#include "audio_config.h"
#include "stream_audio.h"
#include "stream.h"
#include "dllt.h"
#include "dsp_feature_interface.h"
#include "dsp_update_para.h"



/******************************************************************************
 * Function Declaration
 ******************************************************************************/
VOID                DSP_LL_Task                     (VOID);
VOID                DLLT_DefaultEntry               (VOID);
VOID                DLLT_TestEntry                  (VOID);
VOID                DLLT_StartEntry                 (VOID);
VOID                DLLT_ProcessEntry               (VOID);
VOID                DLLT_DeInitEntry                (VOID);
#if DspIpcEnable
VOID                DLLT_CmdHdlr                    (DSP_CMD_PTR_t DspCmdPtr);
#endif
VOID                DLLT_Init                       (VOID);
VOID                DLLT_DeInit                     (VOID* para);
STATIC INLINE VOID  DLLT_Process                    (VOID);
VOID                DLLT_StreamingInit              (VOID);
BOOL                DLLT_ChkCallbackStatus          (VOID);
BOOL                DLLT_ChkCallbackStatusDisable   (VOID);
VOID                DLLT_SuspendRequest             (VOID* para);



/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/


/******************************************************************************
 * Type Definitions
 ******************************************************************************/
#if  DspIpcEnable
typedef VOID (*DLLT_CMD_HDLR) (DSP_CMD_PTR_t DspCmdPtr);
#endif
typedef VOID (*DLLT_ENTRY) (VOID);


/******************************************************************************
 * Constants
 ******************************************************************************/
#define NO_OF_LL_TASK_HANDLER (sizeof(DspLLTaskHdlr)/sizeof(DspLLTaskHdlr[0]))

#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
#define NO_OF_LL_STREAM_FOR_ADVANCED_PASSTHROUGH 1
#else
#define NO_OF_LL_STREAM_FOR_ADVANCED_PASSTHROUGH 0
#endif


#define NO_OF_LL_STREAM         (1 + NO_OF_LL_STREAM_FOR_ADVANCED_PASSTHROUGH)

#if  DspIpcEnable
DLLT_CMD_HDLR CODE DspLLTaskHdlr[] =
{
    NULL,
};
#endif
/******************************************************************************
 * Variables
 ******************************************************************************/
STATIC DLLT_ENTRY DLLT_Entry;

DSP_STREAMING_PARA LLStreaming[NO_OF_LL_STREAM];


/**
 * DSP_LL_Task
 *
 * Main Process for DSP Low Latency task application
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_LL_Task(VOID)
{
	#if  DspIpcEnable
    DSP_CMD_PTR_t DspCmdPtr;
	#endif
	/* Do Initialization */
    DLLT_Init();

    //DAVT_3wire();

    while(1)
    {
    	#if  DspIpcEnable
        if ((DspCmdPtr = osTaskGetIPCMsg()) != NULL)
        {
            DLLT_CmdHdlr(DspCmdPtr);
        }
	#endif
        DLLT_Entry();

        PL_CRITICAL(DLLT_SuspendRequest ,NULL);

        portYIELD();
    }
}

/**
 * DLLT_DefaultEntry
 *
 * Default Entry for DLLT
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DLLT_DefaultEntry(VOID)
{
    if (!DLLT_ChkCallbackStatusDisable())
        DLLT_Entry = DLLT_ProcessEntry;
}


/**
 * DLLT_TestEntry
 *
 * Test Entry for DLLT
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DLLT_TestEntry(VOID)
{
	return;
}

/**
 * DLLT_StartEntry
 *
 * Start Entry for DLLT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DLLT_StartEntry(VOID)
{
    DLLT_Process();
    if(DLLT_ChkCallbackStatus())
        DLLT_Entry = DLLT_ProcessEntry;

}


/**
 * DLLT_ProcessEntry
 *
 * Active Entry for DLLT background process
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 *
 */
VOID DLLT_ProcessEntry(VOID)
{
    #if 0
    if(DLLT_ChkCallbackStatusDisable())
        DLLT_Entry = DLLT_DeInitEntry;
    else
    #endif
        DLLT_Process();
}

/**
 * DLLT_DeInitEntry
 *
 * Initialization Entry for DLLT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DLLT_DeInitEntry(VOID)
{
    PL_CRITICAL(DLLT_DeInit ,NULL);
}

#if  DspIpcEnable

/**
 * DLLT_CmdHdlr
 *
 * DLLT Command Handler handles all Commands towards DLLT task.
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DLLT_CmdHdlr(DSP_CMD_PTR_t DspCmdPtr)
{
    DSP_CMD_MSG_t DspMsg = DspCmdPtr->DspMsg;
	switch (DspMsg)
	{
        case DSP_MSG_START_LINE:
            DLLT_Entry = DLLT_TestEntry;
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
 * DLLT_Init
 *
 * Initialization for DSP_LL_Task
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DLLT_Init(VOID)
{
    DSPMEM_Init(DLL_TASK_ID);
    DLLT_Entry = DLLT_DefaultEntry;
    DSP_Callback_StreamingInit(&LLStreaming[0], NO_OF_LL_STREAM, DLL_TASK_ID);
}

/**
 * DLLT_DeInit
 *
 * De-Initialization for LLT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DLLT_DeInit(VOID* para)
{
    UNUSED(para);
    DSPMEM_Flush(DLL_TASK_ID);
    DSP_Callback_StreamingInit(&LLStreaming[0], NO_OF_LL_STREAM, DLL_TASK_ID);
    DLLT_Entry = DLLT_DefaultEntry;
}


/**
 * DLLT_Process
 *
 * DLLT background process
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 *
 */
STATIC INLINE VOID DLLT_Process(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_LL_STREAM ; i++)
    {
        if (LLStreaming[i].streamingStatus == STREAMING_END){
            StreamCloseAll(LLStreaming[i].source->transform, InstantCloseMode);
        } else {
            DSP_Callback_Processing(&LLStreaming[i]);
        }
    }
}



/**
 * DLLT_StreamingConfig
 *
 * Configuration for DSP_LL_Task streaming
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
TaskHandle_t  DLLT_StreamingConfig(DSP_CALLBACK_STREAM_CONFIG_PTR stream_config_ptr)
{
    stream_config_ptr->stream_ptr = (DSP_STREAMING_PARA_PTR)&LLStreaming[0];
    stream_config_ptr->stream_number = NO_OF_LL_STREAM;
    stream_config_ptr->task = DLL_TASK_ID;
    return DSP_Callback_StreamConfig(stream_config_ptr);
}

/**
 * DLLT_Callback_Get
 *
 * Get DSP_LL_Task callback ptr
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
DSP_CALLBACK_PTR DLLT_Callback_Get(SOURCE source, SINK sink)
{
    U8 i;
    for (i=0 ; i<NO_OF_LL_STREAM ; i++)
    {
        if ((LLStreaming[i].streamingStatus != STREAMING_DISABLE) &&
            (LLStreaming[i].source == source) &&
            (LLStreaming[i].sink == sink))
        {
            return &LLStreaming[i].callback;
        }
    }
    return NULL;
}

/**
 * DLLT_ChkCallbackStatus
 *
 * Whether all DSP_LL_Task callback status SUSPEND/DISABLE
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
BOOL DLLT_ChkCallbackStatus(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_LL_STREAM ; i++)
    {
        if (((LLStreaming[i].callback.Status != CALLBACK_SUSPEND) &&
             (LLStreaming[i].callback.Status != CALLBACK_DISABLE) &&
             (LLStreaming[i].callback.Status != CALLBACK_WAITEND)) ||
            (LLStreaming[i].streamingStatus == STREAMING_END))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * DLLT_ChkCallbackStatusDisable
 *
 * Whether all DSP_LL_Task callback status DISABLE
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
BOOL DLLT_ChkCallbackStatusDisable(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_LL_STREAM ; i++)
    {
        if (LLStreaming[i].callback.Status != CALLBACK_DISABLE)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * DLLT_SuspendRequest
 *
 * DLLT Suspend Request depend on callback status
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
VOID DLLT_SuspendRequest(VOID* para)
{
    UNUSED(para);
    if ((DLLT_Entry != DLLT_StartEntry) && (DLLT_Entry != DLLT_DeInitEntry) &&
        DLLT_ChkCallbackStatus())
    {
        vTaskSuspend(DLL_TASK_ID);
    }
}

/**
 * DLLT_StreeamingDeinitAll
 *
 * DLLT deinit all active stream
 *
 * @Author : Machi <MachiWu@airoha.com.tw>
 */
VOID DLLT_StreeamingDeinitAll(VOID)
{
        U8 i;
    for (i=0 ; i<NO_OF_LL_STREAM ; i++)
    {
        if (LLStreaming[i].streamingStatus == STREAMING_ACTIVE)
        {
            LLStreaming[i].streamingStatus = STREAMING_DEINIT;
        }
    }
}
#endif /*#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)*/

