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
#include "dprt.h"
#include "dprt_rt.h"
#include "dprt_vp.h"
#include "dsp_drv_dfe.h"
#include "audio_config.h"
#include "stream.h"

#include "sink_inter.h"
#include "source_inter.h"
#include "dsp_feature_interface.h"
#include "dsp_update_para.h"
#include "rt_data.h"
//#include "os_dpc.h"
#include <string.h>
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE) & defined(AIR_LD_NR_ENABLE)
#include "ld_nr_interface.h"
#endif

#define DPRT_ZEROPADDING_COUNT       2

/******************************************************************************
* External Variables
******************************************************************************/
#ifdef MTK_PROMPT_SOUND_ENABLE
extern volatile uint32_t vp_sram_empty_flag;
extern volatile uint32_t vp_config_flag;
extern volatile uint32_t vp_data_request_flag;
#endif

#ifdef AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
U8* g_vp_memptr;   //VP Framework Data Mem Ptr
U8* g_vp_memptr2;  //VP Feature Entry Mem Ptr
U32 g_vp_featureEntrySize = 0;
extern TaskHandle_t  pDPR_TaskHandler;
extern stream_feature_list_t stream_feature_list_prompt[];
#endif

/******************************************************************************
 * Function Declaration
 ******************************************************************************/
VOID                DSP_PR_Task                     (VOID);
VOID                DPRT_DefaultEntry               (VOID);
VOID                DPRT_TestEntry                  (VOID);
VOID                DPRT_StartEntry                 (VOID);
VOID                DPRT_ActiveEntry                (VOID);
VOID                DPRT_DeInitEntry                (VOID);
#if  DspIpcEnable
VOID                DPRT_CmdHdlr                    (DSP_CMD_PTR_t DspCmdPtr);
#endif
VOID                DPRT_Init                       (VOID);
VOID                DPRT_DeInit                     (VOID* para);
VOID                DPRT_StreamingInit              (VOID);
STATIC INLINE VOID  DPRT_Process                    (VOID);
BOOL                DPRT_ChkCallbackStatus          (VOID);
BOOL                DPRT_ChkCallbackStatusDisable   (VOID);
VOID                DPRT_SuspendRequest             (VOID* para);
VOID                DSP_PRT_RT_Init                 (VOID);
VOID                DSP_PRT_PlayTone                (U32 ToneIdx);
VOID                DSP_PRT_GetData                 (U32 ToneIdx);


/******************************************************************************
 * External Function Prototypes
 ******************************************************************************/
EXTERN  VOID    DTM_ResumeDPRT              (VOID);


/******************************************************************************
 * Type Definitions
 ******************************************************************************/
#if  DspIpcEnable
typedef VOID (*DPRT_CMD_HDLR) (DSP_CMD_PTR_t DspCmdPtr);
#endif
typedef VOID (*DPRT_ENTRY) (VOID);


/******************************************************************************
 * Constants
 ******************************************************************************/
/* Number of DSP States */
#define NO_OF_PR_TASK_HANDLER (sizeof(DspPrTaskHdlr)/sizeof(DspPrTaskHdlr[0]))
#define NO_OF_PR_STREAM     (3)
#if  DspIpcEnable
/* DSP Job State Handler */
DPRT_CMD_HDLR CODE DspPrTaskHdlr[] =
{
    NULL,
};
#endif

/******************************************************************************
 * Variables
 ******************************************************************************/
STATIC DPRT_ENTRY DPRT_Entry;
DSP_STREAMING_PARA VOLATILE PRStreaming[NO_OF_PR_STREAM];
DRIVER_RINGTONE_DATA_TYPE_PTR RingPtr;
RT_CTRL_PARA_t gRTCtrl;


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

VOID DSP_PRT_RT_Init (VOID);

/**
 * DSP_VP_Premalloc
 *
 * Dedicate VP Memory in DSP_PR_Task
 * @ VP_FRAMEWORK_MEM_SIZE = VP_FrameSize(2048)*channelNum(2)
 * @ featureEntrySize = featureEntryNum*sizeof(DSP_FEATURE_TABLE)
 * @ g_vp_memptr: global memory pointer for VP data
 * @ g_vp_memptr2: global memory pointer for VP feature entry
 *
 */
#ifdef AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
VOID DSP_VP_Premalloc(VOID)
{
    VOID* stream = NULL;
    U32 featureEntryNum = 0;

    g_vp_memptr = DSPMEM_tpremalloc(pDPR_TaskHandler, VP_FRAMEWORK_MEM_SIZE, stream);//4096
    if(g_vp_memptr == NULL){
        platform_assert("[g_vp_memptr]g_vp_memptr is NULL!", __FILE__, __LINE__);
    }

    //calculate featureEntrySize based on feature Num
    while(stream_feature_list_prompt[featureEntryNum] != FUNC_END){
        featureEntryNum++;
    }
    featureEntryNum += 1;
    g_vp_featureEntrySize = featureEntryNum*sizeof(DSP_FEATURE_TABLE);

    g_vp_memptr2 = DSPMEM_tpremalloc(pDPR_TaskHandler, g_vp_featureEntrySize, stream);
    if(g_vp_memptr2 == NULL){
        platform_assert("[g_vp_memptr2]g_vp_memptr2 is NULL!", __FILE__, __LINE__);
    }
}
#endif

/**
 * DSP_PR_Task
 *
 * Main Process for DSP_PR_Task
 *
 */
VOID DSP_PR_Task(VOID)
{
	#if  DspIpcEnable
    DSP_CMD_PTR_t DspCmdPtr;
	#endif
    /* Do Initialization */
    DPRT_Init();

#ifdef AIR_PROMPT_SOUND_MEMORY_DEDICATE_ENABLE
    DSP_VP_Premalloc();
#endif
    //DPRT_Setup_RT();
    //DPRT_Setup_Test_RT();
    while(1)
    {
    	#if  DspIpcEnable
        if ((DspCmdPtr = osTaskGetIPCMsg()) != NULL)
        {
            DPRT_CmdHdlr(DspCmdPtr);
        }
		#endif
        DPRT_Entry();
#ifdef MTK_PROMPT_SOUND_ENABLE
    if (vp_data_request_flag == 1) {
        DSP_MW_LOG_I("[CM4_VP_PB] VP_Request.", 0);
        vp_data_request_flag = 0;
        hal_ccni_message_t msg;
        memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
        msg.ccni_message[0] = 0x707 << 16;
        aud_msg_tx_handler(msg, 0, FALSE);
    }
    if ((vp_config_flag == 1) && (vp_sram_empty_flag == 1)) {
        vp_config_flag = 0;
        vp_sram_empty_flag = 0;
        hal_ccni_message_t ccni_msg;
        memset((void *)&ccni_msg, 0, sizeof(hal_ccni_message_t));
        ccni_msg.ccni_message[0] = (0x8704 << 16);  //MSG_DSP2MCU_PROMPT_CONFIG_ACK
        aud_msg_tx_handler(ccni_msg, 0, FALSE);
    }
#endif
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE) & defined(AIR_LD_NR_ENABLE)
        if (ld_nr_bg_entry) {
            ld_nr_bg_entry();
        }
#endif
        PL_CRITICAL(DPRT_SuspendRequest ,NULL);

        portYIELD();
    }
}

/**
 * DPRT_DefaultEntry
 *
 * Default Entry for DPRT
 *
 */
VOID DPRT_DefaultEntry(VOID)
{
    if (!DPRT_ChkCallbackStatusDisable())
        DPRT_Entry = DPRT_ActiveEntry;
}

/**
 * DPRT_TestEntry
 *
 * Test Entry for DPRT
 *
 */
VOID DPRT_TestEntry(VOID)
{
    return;
}

/**
 * DPRT_StartEntry
 *
 * Start Entry for Prompt
 *
 */
VOID DPRT_StartEntry(VOID)
{
    DPRT_Process();
    if (DPRT_ChkCallbackStatus())
    {
        DPRT_Entry = DPRT_ActiveEntry;
    }

}

/**
 * DPRT_ActiveEntry
 *
 * Active Entry for Prompt
 *
 */
VOID DPRT_ActiveEntry(VOID)
{
    #if 0
    if (DPRT_ChkCallbackStatusDisable())
        DPRT_Entry = DPRT_DeInitEntry;
    else
    #endif
        DPRT_Process();
}

/**
 * DPRT_DeInitEntry
 *
 * DeInit Entry for Prompt
 *
 */
VOID DPRT_DeInitEntry(VOID)
{
    PL_CRITICAL(DPRT_DeInit ,NULL);
}


#if  DspIpcEnable
/**
 * DPRT_CmdHdlr
 *
 * DPRT Command Handler handles all Commands towards DPRT task.
 *
 */
VOID DPRT_CmdHdlr(DSP_CMD_PTR_t DspCmdPtr)
{
    DSP_CMD_MSG_t DspMsg = DspCmdPtr->DspMsg;
    switch (DspMsg)
    {
        case DSP_MSG_START_RT:

            break;

        case DSP_MSG_START_VP:
            DPRT_Entry = DPRT_StartEntry;
            //PRStreaming.callback.Status = CALLBACK_MALLOC;
            break;

        case DSP_MSG_STOP_PRT:
            DPRT_Entry = DPRT_TestEntry;
            break;

        case DSP_UPDATE_PARAMETER:
            DSP_UpdateStreamingPara(&DspCmdPtr->DspCmdPara.UpdatePara);
            break;

        default:
            break;
    }
}
#endif
/**
 * DPRT_Init
 *
 * Initialization for DPRT
 *
 */
VOID DPRT_Init(VOID)
{
    DSPMEM_Init(DPR_TASK_ID);
    DPRT_Entry = DPRT_DefaultEntry;
    DSP_Callback_StreamingInit((DSP_STREAMING_PARA_PTR)&PRStreaming[0], NO_OF_PR_STREAM, DPR_TASK_ID);
    //DSP_PRT_RT_Init();
}

/**
 * DPRT_DeInit
 *
 * De-Initialization for DPRT
 *
 */
VOID DPRT_DeInit(VOID* para)
{
    UNUSED(para);
    DSPMEM_Flush(DPR_TASK_ID);
    DSP_Callback_StreamingInit((DSP_STREAMING_PARA_PTR)&PRStreaming[0], NO_OF_PR_STREAM, DPR_TASK_ID);
    DPRT_Entry = DPRT_DefaultEntry;
}

/**
 * DPRT_Process
 *
 * Active Entry for Prompt background process
 *
 *
 * @Return : processing status
 */
STATIC INLINE VOID DPRT_Process(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_PR_STREAM ; i++)
    {
        if (PRStreaming[i].streamingStatus == STREAMING_END) {
#if 0
            if ((PRStreaming[i].sink->type == SINK_TYPE_AUDIO) &&
                (PRStreaming[i].sink->param.audio.irq_exist) &&
                (Audio_setting->Audio_sink.Zero_Padding_Cnt>0)) {
                //DSP_MW_LOG_E("DPRT wait zero padding:%d !!!", 1, Audio_setting->Audio_sink.Zero_Padding_Cnt);
            } else {
#endif
            StreamCloseAll(PRStreaming[i].source->transform, InstantCloseMode);
#if 0
            }
#endif
        }
        DSP_Callback_Processing((DSP_STREAMING_PARA_PTR)&PRStreaming[i]);
    }
}


/**
 * DPRT_StreamingConfig
 *
 * Configuration for DSP_PR_Task streaming
 *
 */
TaskHandle_t  DPRT_StreamingConfig(DSP_CALLBACK_STREAM_CONFIG_PTR stream_config_ptr)
{
    stream_config_ptr->stream_ptr = (DSP_STREAMING_PARA_PTR)&PRStreaming[0];
    stream_config_ptr->stream_number = NO_OF_PR_STREAM;
    stream_config_ptr->task = DPR_TASK_ID;
    return DSP_Callback_StreamConfig(stream_config_ptr);
}

/**
 * DPRT_Callback_Get
 *
 * Get DSP_PR_Task callback ptr
 *
 */
DSP_CALLBACK_PTR DPRT_Callback_Get(SOURCE source, SINK sink)
{
    U8 i;
    for (i=0 ; i<NO_OF_PR_STREAM ; i++)
    {
        if ((PRStreaming[i].streamingStatus != STREAMING_DISABLE) &&
            (PRStreaming[i].source == source) &&
            (PRStreaming[i].sink == sink))
        {
            return (DSP_CALLBACK_PTR)&PRStreaming[i].callback;
        }
    }
    return NULL;
}


/**
 * DPRT_ChkCallbackStatus
 *
 * Whether all DSP_PR_Task callback status SUSPEND/DISABLE
 *
 */
BOOL DPRT_ChkCallbackStatus(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_PR_STREAM ; i++)
    {
        if (((PRStreaming[i].callback.Status != CALLBACK_SUSPEND) &&
             (PRStreaming[i].callback.Status != CALLBACK_DISABLE) &&
             (PRStreaming[i].callback.Status != CALLBACK_WAITEND)) ||
            (PRStreaming[i].streamingStatus == STREAMING_END))
        {
            return FALSE;
        }
    }
    return TRUE;
}

/**
 * DPRT_ChkCallbackStatusDisable
 *
 * Whether all DSP_PR_Task callback status DISABLE
 *
 */
BOOL DPRT_ChkCallbackStatusDisable(VOID)
{
    U8 i;
    for (i=0 ; i<NO_OF_PR_STREAM ; i++)
    {
        if (PRStreaming[i].callback.Status != CALLBACK_DISABLE)
        {
            return FALSE;
        }
    }
    return TRUE;
}



/**
 * DPRT_SuspendRequest
 *
 * DPRT Suspend Request depend on callback status
 *
 */
VOID DPRT_SuspendRequest(VOID* para)
{
    UNUSED(para);
    if ((DPRT_Entry != DPRT_StartEntry) && (DPRT_Entry != DPRT_DeInitEntry) &&
        DPRT_ChkCallbackStatus())
        vTaskSuspend(DPR_TASK_ID);
}


/* The following RT drivers should be moved to appropriate place */


/**
 * DSP_PRT_RT_Init
 *
 * Initialization of ringtone parameter
 *
 */
VOID DSP_PRT_RT_Init (VOID)
{
    /* Change this address later to NVKEY */
    /* RingPtr = (DRIVER_RINGTONE_DATA_TYPE_PTR)&gDriver_ringtone_data_init;
    gRTCtrl.IsPlaying = FALSE; */
}

/**
 * DSP_PRT_PlayTone
 *
 * Main API for ringtone playing
 *
 */
VOID DSP_PRT_PlayTone (U32 ToneIdx)
{
    if (gRTCtrl.IsPlaying)
    {
        /* Pending */
    }
    else
    {
        configASSERT(ToneIdx < TOTAL_BEEP_PATTERNS);

        DSP_PRT_GetData(ToneIdx);

        gRTCtrl.IsPlaying = TRUE;
    }
}


/**
 * DSP_PRT_PlayTone
 *
 * Main API for ringtone playing
 *
 */
VOID DSP_PRT_GetData (U32 ToneIdx)
{
    RINGTONE_RECORD_TYPE ToneInfo;

    ToneInfo = RingPtr->ringtoneRecord[ToneIdx];

    if (ToneInfo.dataLength == 0)
    {
        return;
    }

    gRTCtrl.PlayAddr    = (U32*)(((U8*)RingPtr) + ToneInfo.offset);
    gRTCtrl.ToneLength  = ToneInfo.dataLength;
}
