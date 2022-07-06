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
#include "dhpt.h"
#include "dsp_feature_interface.h"
#include "dsp_update_para.h"



/******************************************************************************
 * Function Declaration
 ******************************************************************************/
VOID                DSP_HP_Task                     (VOID);
VOID                DHPT_DefaultEntry               (VOID);
VOID                DHPT_TestEntry                  (VOID);
VOID                DHPT_StartEntry                 (VOID);
VOID                DHPT_ProcessEntry               (VOID);
VOID                DHPT_DeInitEntry                (VOID);
#if DspIpcEnable
VOID                DHPT_CmdHdlr                    (DSP_CMD_PTR_t DspCmdPtr);
#endif
VOID                DHPT_Init                       (VOID);
VOID                DHPT_DeInit                     (VOID* para);
STATIC INLINE VOID  DHPT_Process                    (VOID);
VOID                DHPT_StreamingInit              (VOID);
BOOL                DHPT_ChkCallbackStatus          (VOID);
BOOL                DHPT_ChkCallbackStatusDisable   (VOID);
VOID                DHPT_SuspendRequest             (VOID* para);



/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/


/******************************************************************************
 * Type Definitions
 ******************************************************************************/
#if  DspIpcEnable
typedef VOID (*DHPT_CMD_HDLR) (DSP_CMD_PTR_t DspCmdPtr);
#endif
typedef VOID (*DHPT_ENTRY) (VOID);


/******************************************************************************
 * Constants
 ******************************************************************************/
#define NO_OF_HP_TASK_HANDLER (sizeof(DspHPTaskHdlr)/sizeof(DspHPTaskHdlr[0]))

#ifdef MTK_MULTI_MIC_STREAM_ENABLE
#define NO_OF_HP_STREAM_FOR_MULTI_MIC_STREAM 4
#else
#define NO_OF_HP_STREAM_FOR_MULTI_MIC_STREAM 0
#endif

#if defined(AIR_GAMING_MODE_DONGLE_ENABLE)
#define NO_OF_HP_STREAM_FOR_GAMING_MODE_DONGLE 2
#else
#define NO_OF_HP_STREAM_FOR_GAMING_MODE_DONGLE 0
#endif

#ifdef AIR_WIRED_AUDIO_ENABLE
#define NO_OF_HP_STREAM_For_WIRED_AUDIO 2 ////line in(1)/usb-in(2)
#else
#define NO_OF_HP_STREAM_For_WIRED_AUDIO 0
#endif

#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
#define NO_OF_HP_STREAM_FOR_ADVANCED_PASSTHROUGH 1
#else
#define NO_OF_HP_STREAM_FOR_ADVANCED_PASSTHROUGH 0
#endif

#if defined(AIR_BLE_AUDIO_DONGLE_ENABLE)
#define NO_OF_HP_STREAM_FOR_BLE_AUDIO_DONGLE 2
#else
#define NO_OF_HP_STREAM_FOR_BLE_AUDIO_DONGLE 0
#endif

#define NO_OF_HP_STREAM         (1 + NO_OF_HP_STREAM_FOR_MULTI_MIC_STREAM + NO_OF_HP_STREAM_FOR_GAMING_MODE_DONGLE + NO_OF_HP_STREAM_For_WIRED_AUDIO + NO_OF_HP_STREAM_FOR_ADVANCED_PASSTHROUGH + NO_OF_HP_STREAM_FOR_BLE_AUDIO_DONGLE)

#if  DspIpcEnable
DHPT_CMD_HDLR CODE DspHPTaskHdlr[] =
{
    NULL,
};
#endif
/******************************************************************************
 * Variables
 ******************************************************************************/
STATIC DHPT_ENTRY DHPT_Entry;

DSP_STREAMING_PARA HPStreaming[NO_OF_HP_STREAM];


/**
 * DSP_HP_Task
 *
 * Main Process for DSP High Priority task application
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_HP_Task(VOID)
{
	#if  DspIpcEnable
    DSP_CMD_PTR_t DspCmdPtr;
	#endif
	/* Do Initialization */
    DHPT_Init();

    //DAVT_3wire();

    while(1)
    {
    	#if  DspIpcEnable
        if ((DspCmdPtr = osTaskGetIPCMsg()) != NULL)
        {
            DHPT_CmdHdlr(DspCmdPtr);
        }
		#endif
        DHPT_Entry();

        PL_CRITICAL(DHPT_SuspendRequest ,NULL);

        portYIELD();
    }
}

/**
 * DHPT_DefaultEntry
 *
 * Default Entry for DHPT
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DHPT_DefaultEntry(VOID)
{
    if (!DHPT_ChkCallbackStatusDisable())
        DHPT_Entry = DHPT_ProcessEntry;
}


/**
 * DHPT_TestEntry
 *
 * Test Entry for DHPT
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DHPT_TestEntry(VOID)
{
	return;
}

/**
 * DHPT_StartEntry
 *
 * Start Entry for DHPT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DHPT_StartEntry(VOID)
{
    DHPT_Process();
    if(DHPT_ChkCallbackStatus())
        DHPT_Entry = DHPT_ProcessEntry;

}


/**
 * DHPT_ProcessEntry
 *
 * Active Entry for DHPT background process
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 *
 */
VOID DHPT_ProcessEntry(VOID)
{
    #if 0
    if(DHPT_ChkCallbackStatusDisable())
        DHPT_Entry = DHPT_DeInitEntry;
    else
    #endif
        DHPT_Process();
}

/**
 * DHPT_DeInitEntry
 *
 * Initialization Entry for DHPT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DHPT_DeInitEntry(VOID)
{
    PL_CRITICAL(DHPT_DeInit ,NULL);
}

#if  DspIpcEnable

/**
 * DHPT_CmdHdlr
 *
 * DHPT Command Handler handles all Commands towards DHPT task.
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DHPT_CmdHdlr(DSP_CMD_PTR_t DspCmdPtr)
{
    DSP_CMD_MSG_t DspMsg = DspCmdPtr->DspMsg;
	switch (DspMsg)
	{
        case DSP_MSG_START_LINE:
            DHPT_Entry = DHPT_TestEntry;
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
 * DHPT_Init
 *
 * Initialization for DSP_HP_Task
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DHPT_Init(VOID)
{
    DSPMEM_Init(DHP_TASK_ID);
    DHPT_Entry = DHPT_DefaultEntry;
    DSP_Callback_StreamingInit(&HPStreaming[0], NO_OF_HP_STREAM, DHP_TASK_ID);
}

/**
 * DHPT_DeInit
 *
 * De-Initialization for HPT
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 */
VOID DHPT_DeInit(VOID* para)
{
    UNUSED(para);
    DSPMEM_Flush(DHP_TASK_ID);
    DSP_Callback_StreamingInit(&HPStreaming[0], NO_OF_HP_STREAM, DHP_TASK_ID);
    DHPT_Entry = DHPT_DefaultEntry;
}


/**
 * DHPT_Process
 *
 * DHPT background process
 *
 * @Author : BrianChen <BrianChen@airoha.com.tw>
 *
 */
#ifdef MTK_BT_A2DP_AIRO_CELT_ENABLE
STATIC INLINE VOID DHPT_Process(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_HP_STREAM ; i++)
    {
        if (HPStreaming[i].streamingStatus == STREAMING_END){
            #ifdef AIR_AUDIO_HARDWARE_ENABLE
            if ((HPStreaming[i].sink->type == SINK_TYPE_AUDIO) &&
                (HPStreaming[i].sink->param.audio.irq_exist) &&
                (Audio_setting->Audio_sink.Zero_Padding_Cnt>0)) {
            }
            else
            #endif /* AIR_AUDIO_HARDWARE_ENABLE */
            {
                StreamCloseAll(HPStreaming[i].source->transform, InstantCloseMode);
            }
        }
        else if(HPStreaming[i].source != NULL)
        {
            if (HPStreaming[i].source->type == SOURCE_TYPE_A2DP)
            {
                if ((HPStreaming[i].callback.Status == CALLBACK_SUSPEND) || (HPStreaming[i].callback.Status == CALLBACK_WAITEND))
                {
                    HPStreaming[i].source->transform->Handler(HPStreaming[i].source, HPStreaming[i].sink);
                }
            }
        }
        DSP_Callback_Processing((DSP_STREAMING_PARA_PTR)&HPStreaming[i]);
    }

}
#else
STATIC INLINE VOID DHPT_Process(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_HP_STREAM ; i++)
    {
        if (HPStreaming[i].streamingStatus == STREAMING_END){
            StreamCloseAll(HPStreaming[i].source->transform, InstantCloseMode);
        } else {
            DSP_Callback_Processing(&HPStreaming[i]);
        }
    }
}
#endif


/**
 * DHPT_StreamingConfig
 *
 * Configuration for DSP_HP_Task streaming
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
TaskHandle_t  DHPT_StreamingConfig(DSP_CALLBACK_STREAM_CONFIG_PTR stream_config_ptr)
{
    stream_config_ptr->stream_ptr = (DSP_STREAMING_PARA_PTR)&HPStreaming[0];
    stream_config_ptr->stream_number = NO_OF_HP_STREAM;
    stream_config_ptr->task = DHP_TASK_ID;
    return DSP_Callback_StreamConfig(stream_config_ptr);
}

/**
 * DHPT_Callback_Get
 *
 * Get DSP_HP_Task callback ptr
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
DSP_CALLBACK_PTR DHPT_Callback_Get(SOURCE source, SINK sink)
{
    U8 i;
    for (i=0 ; i<NO_OF_HP_STREAM ; i++)
    {
        if ((HPStreaming[i].streamingStatus != STREAMING_DISABLE) &&
            (HPStreaming[i].source == source) &&
            (HPStreaming[i].sink == sink))
        {
            return &HPStreaming[i].callback;
        }
    }
    return NULL;
}

/**
 * DHPT_ChkCallbackStatus
 *
 * Whether all DSP_HP_Task callback status SUSPEND/DISABLE
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
BOOL DHPT_ChkCallbackStatus(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_HP_STREAM ; i++)
    {
        if (((HPStreaming[i].callback.Status != CALLBACK_SUSPEND) &&
             (HPStreaming[i].callback.Status != CALLBACK_DISABLE) &&
             (HPStreaming[i].callback.Status != CALLBACK_WAITEND)) ||
            (HPStreaming[i].streamingStatus == STREAMING_END))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * DHPT_ChkCallbackStatusDisable
 *
 * Whether all DSP_HP_Task callback status DISABLE
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
BOOL DHPT_ChkCallbackStatusDisable(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_HP_STREAM ; i++)
    {
        if (HPStreaming[i].callback.Status != CALLBACK_DISABLE)
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * DHPT_SuspendRequest
 *
 * DHPT Suspend Request depend on callback status
 *
 * @Author : Brian <BrianChen@airoha.com.tw>
 */
VOID DHPT_SuspendRequest(VOID* para)
{
    UNUSED(para);
    if ((DHPT_Entry != DHPT_StartEntry) && (DHPT_Entry != DHPT_DeInitEntry) &&
        DHPT_ChkCallbackStatus())
    {
        vTaskSuspend(DHP_TASK_ID);
    }
}

/**
 * DHPT_StreeamingDeinitAll
 *
 * DHPT deinit all active stream
 *
 * @Author : Machi <MachiWu@airoha.com.tw>
 */
VOID DHPT_StreeamingDeinitAll(VOID)
{
        U8 i;
    for (i=0 ; i<NO_OF_HP_STREAM ; i++)
    {
        if (HPStreaming[i].streamingStatus == STREAMING_ACTIVE)
        {
            HPStreaming[i].streamingStatus = STREAMING_DEINIT;
        }
    }
}
