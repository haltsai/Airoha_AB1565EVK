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
 
 #if 0
#include "config.h"
//#include "os.h"
//#include "os_memory.h"
#include "dsp_task.h"
//#include "rc.h"
#include "dsp_command_dispatcher.h"


#define DSP_UPDATE_COMMAND_STRUCTURE_SIZE   sizeof(DSP_COMMAND_t)-1

/******************************************************************************
 * Function Declaration
 ******************************************************************************/
VOID                DSP_CommandInit                         (VOID);
DSP_CMD_PTR_t       DSP_CommandFetch                        (VOID);
VOID                DSP_CommandDispatch                     (DSP_CMD_PTR_t DspCmdPtr);
VOID                DSP_CommandChangeID                     (DSP_CMD_PTR_t DspCmdPtr, U8 NewTaskID);
VOID                DSP_CommandForward                      (DSP_CMD_PTR_t DspCmdPtr, U8 FwdTaskID);
VOID                DSP_CommandDelivery                     (DSP_CMD_PTR_t DspCmdPtr, U8 TaskID);
DSP_CMD_PTR_t       DSP_CommandDuplicate                    (DSP_CMD_PTR_t DspCmdPtr);
DSP_CMD_PTR_t       DSP_CommandConstruction                 (DSP_CMD_MSG_t DspMsg, U8 TaskID, DSP_CMD_PARA_PTR_t DspCmdParaPtr);
BOOL                DSP_CommandConstructAndDelivery         (DSP_CMD_MSG_t DspMsg, U8 TaskID, DSP_CMD_PARA_PTR_t DspCmdParaPtr);
VOID                DSP_CommandDestructAll                  (U8 TaskID);
VOID                DSP_CommandKeepNewest                   (U8 TaskID);
VOID                DSP_CommandKeepActiveGroup              (U8 TaskID);
BOOL                DSP_IsCommandWithStopAttribute          (DSP_CMD_PTR_t DspCmdPtr);
VOID                DSP_CommandParameterConfig              (DSP_CMD_PTR_t DspCmdPtr, DSP_CMD_PARA_PTR_t DspCmdParaPtr);


/******************************************************************************
 * Variables
 ******************************************************************************/
STATIC U8 DspCmdSeqNo;


/**
 * DSP_CommandInit
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandInit(VOID)
{
    #if 0
    OS_TCB_STRU_PTR pTCB;

    pTCB = osTaskQueryTCB(DSP_TASK_ID);
    PLMQ_Init(&PLMQ_ToDSP_Command, pTCB);

    pTCB = osTaskQueryTCB(DAV_TASK_ID);
    PLMQ_Init(&PLMQ_ToDAVT_Command, pTCB);

    pTCB = osTaskQueryTCB(DPR_TASK_ID);
    PLMQ_Init(&PLMQ_ToDPRT_Command, pTCB);
    #endif

    DspCmdSeqNo = 0;
}


/**
 * DSP_CommandFetch
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
DSP_CMD_PTR_t DSP_CommandFetch(VOID)
{
    DSP_CMD_PTR_t DspCmdPtr;

	if ((DspCmdPtr = osTaskGetIPCMsg()) != NULL)
	{
        return DspCmdPtr;
	}
	else
	{
        return NULL;
	}
}


/**
 * DSP_CommandDispatch
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandDispatch(DSP_CMD_PTR_t DspCmdPtr)
{
	U16 DspCmdSize  = sizeof(DSP_CMD_PARA_t)/sizeof(U8);
	DSP_CMD_PARA_t DspCmdPara;

	memcpy(&DspCmdPara,
	          &DspCmdPtr->DspCmdPara,
	          DspCmdSize);
}


/**
 * DSP_CommandChangeID
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandChangeID(DSP_CMD_PTR_t DspCmdPtr, U8 NewTaskID)
{
    DspCmdPtr->TaskID = NewTaskID;
}


/**
 * DSP_CommandForward
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandForward(DSP_CMD_PTR_t DspCmdPtr, U8 FwdTaskID)
{
    DSP_CommandDelivery(DspCmdPtr, FwdTaskID);
}


/**
 * DSP_CommandDelivery
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandDelivery(DSP_CMD_PTR_t DspCmdPtr, U8 TaskID)
{
    #if 0
    PLMQ_PTR TargetQueue;

    TargetQueue = DSP_AcquireTargetQueue(TaskID);

    PLMQ_Put(TargetQueue,DspCmdPtr);
    #else
    osTaskPutIPCMsg(TaskID, DspCmdPtr);
    #endif
}


/**
 * DSP_CommandDuplicate
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
DSP_CMD_PTR_t DSP_CommandDuplicate(DSP_CMD_PTR_t DspCmdPtr)
{
    DSP_CMD_PTR_t DuplicateDspCmdPtr;
    U16 DspCmdSize  = sizeof(DSP_COMMAND_t)/sizeof(U8);

    if ((DuplicateDspCmdPtr = OS_malloc(DspCmdSize)) != NULL)
    {
        memcpy(DuplicateDspCmdPtr,
                  DspCmdPtr,
                  DspCmdSize);

        return DuplicateDspCmdPtr;
    }
    else
    {
        /* False condition */
        return NULL;
    }
}


/**
 * DSP_CommandConstruction
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
DSP_CMD_PTR_t DSP_CommandConstruction(DSP_CMD_MSG_t DspMsg, U8 TaskID, DSP_CMD_PARA_PTR_t DspCmdParaPtr)
{
    DSP_CMD_PTR_t VOLATILE DspCmdPtr;
    U16 DspCmdSize  = sizeof(DSP_COMMAND_t)/sizeof(U8);

    if ((DspCmdPtr = OS_malloc(DspCmdSize)) != NULL)
    {
        memset(DspCmdPtr, 0, DspCmdSize);
        DspCmdPtr->DspMsg       = DspMsg;
        DspCmdPtr->TaskID       = TaskID;
        DspCmdPtr->CmdSeqNo     = (++DspCmdSeqNo);
        DSP_CommandParameterConfig(DspCmdPtr, DspCmdParaPtr);
    }

    return DspCmdPtr;
}

/**
 * LM2DSP_CommandConstruction
 *
 * @Author : MachiWu <Machiwu@airoha.com.tw>
 * @brief Request to create an LM to DSP command
 * @param DspMsg            The command message
 * @param TxTaskID          The current running task to send the command
 * @param DspCmdParaPtr  The pointer of command parameter
 * @return The Source ID.
 */
DSP_CMD_PTR_t LM2DSP_CommandConstruction(DSP_CMD_MSG_t DspMsg, U8 TxTaskID, DSP_CMD_PARA_PTR_t DspCmdParaPtr)
{
    DSP_CMD_PTR_t VOLATILE DspCmdPtr;
    U16 DspCmdSize  = sizeof(DSP_COMMAND_t)/sizeof(U8);
    configASSERT((DspMsg > DSP_MSG_LM_ENUM_BEGIN)
        && (DspMsg < DSP_MSG_TM_ENUM_END));


    if ((DspCmdPtr = OS_malloc(DspCmdSize)) != NULL)
    {
        memset(DspCmdPtr, 0, DspCmdSize);
        DspCmdPtr->DspMsg       = DspMsg;
        DspCmdPtr->TaskID       = DSP_TASK_ID;
        DspCmdPtr->TxTaskID     = TxTaskID;
        DspCmdPtr->CmdSeqNo     = (++DspCmdSeqNo);
        if (DspCmdParaPtr != NULL)
        {
            DSP_CommandParameterConfig(DspCmdPtr, DspCmdParaPtr);
        }
    }

    return DspCmdPtr;
}



/**
 * DSP_CommandConstructAndDelivery
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
BOOL DSP_CommandConstructAndDelivery(DSP_CMD_MSG_t DspMsg, U8 TaskID, DSP_CMD_PARA_PTR_t DspCmdParaPtr)
{
    DSP_CMD_PTR_t DspCmdPtr;

    if ((DspCmdPtr = DSP_CommandConstruction(DspMsg, TaskID, DspCmdParaPtr)) != NULL)
    {
        DSP_CommandDelivery(DspCmdPtr, DSP_TASK_ID);
        return TRUE;
    }
    else
    {
        /* False condition */
        return FALSE;
    }
}


/**
 * DSP_CommandDestructAll
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandDestructAll(U8 TaskID)
{
    DSP_CMD_PTR_t DspCmdPtr;

    while ((DspCmdPtr = osTaskGetIPCMsg()) != NULL)
    {
        if (TaskID == DspCmdPtr->TaskID)
        {
            OSMEM_Put(DspCmdPtr);
        }
    }
}


/**
 * DSP_CommandKeepNewest
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandKeepNewest(U8 TaskID)
{
    DSP_CMD_PTR_t CmdPtr, NewestCmdPtr;
    U8 QueueNo, SeqNo, NewestSeqNo;
    BOOL FirstQ = TRUE;

    OS_PS ps = OS_ENTER_CRITICAL();

    QueueNo = osTaskIPCMsgNum();

    while (QueueNo)
    {
        configASSERT((CmdPtr = osTaskGetIPCMsg()) != NULL);

        if (TaskID == CmdPtr->TaskID)
        {
            SeqNo = CmdPtr->CmdSeqNo;
            if (FirstQ)
            {
                NewestSeqNo = SeqNo;
                NewestCmdPtr = CmdPtr;
                FirstQ = FALSE;
            }
            else
            {
                if ((S8)(SeqNo - NewestSeqNo) > 0)
                {
                    OSMEM_Put(NewestCmdPtr);
                    NewestSeqNo = SeqNo;
                    NewestCmdPtr = CmdPtr;
                }
                else
                {
                    OSMEM_Put(CmdPtr);
                }
            }
        }
        else
        {
            osTaskPutIPCMsg(DSP_TASK_ID, CmdPtr);
        }

        QueueNo--;
    }

    if (!FirstQ)
    {
        osTaskPutIPCMsg(DSP_TASK_ID, NewestCmdPtr);
    }

    OS_EXIT_CRITICAL(ps); 
}


/**
 * DSP_CommandKeepActiveGroup
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandKeepActiveGroup(U8 TaskID)
{
    DSP_CMD_PTR_t CmdPtr;
    U8 QueueNo, SeqNo, NewestStopCmdSeqNo;
    BOOL FirstQ = TRUE;

    OS_PS ps = OS_ENTER_CRITICAL();

    /* Find Newest Stop Command */
    QueueNo = osTaskIPCMsgNum();

    while (QueueNo)
    {
        configASSERT((CmdPtr = osTaskGetIPCMsg()) != NULL);

        if (DSP_IsCommandWithStopAttribute(CmdPtr)
         && (TaskID == CmdPtr->TaskID)
         && (HOST_TASK_ID == CmdPtr->TxTaskID))
        {
            SeqNo = CmdPtr->CmdSeqNo;

            if (FirstQ)
            {
                NewestStopCmdSeqNo = SeqNo;
                FirstQ = FALSE;
            }
            else
            {
                if ((S8)(SeqNo - NewestStopCmdSeqNo) > 0)
                {
                    NewestStopCmdSeqNo = SeqNo;
                }
            }
        }

        osTaskPutIPCMsg(DSP_TASK_ID, CmdPtr);

        QueueNo--;
    }

    /* Clear Commands Before Newest Stop Command */
    if (!FirstQ)
    {
        QueueNo = osTaskIPCMsgNum();

        while (QueueNo)
        {
            configASSERT((CmdPtr = osTaskGetIPCMsg()) != NULL);

            if (TaskID == CmdPtr->TaskID)
            {
                SeqNo = CmdPtr->CmdSeqNo;

                if (((S8)(SeqNo - NewestStopCmdSeqNo) > 0)
                 && (HOST_TASK_ID == CmdPtr->TxTaskID))
                {
                    osTaskPutIPCMsg(DSP_TASK_ID, CmdPtr);
                }
                else
                {
                    OSMEM_Put(CmdPtr);
                }
            }
            else
            {
                osTaskPutIPCMsg(DSP_TASK_ID, CmdPtr);
            }

            QueueNo--;
        }
    }

    OS_EXIT_CRITICAL(ps); 
}


/**
 * DSP_IsCommandWithStopAttribute
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
BOOL DSP_IsCommandWithStopAttribute(DSP_CMD_PTR_t DspCmdPtr)
{
    DSP_CMD_MSG_t DspMsg = DspCmdPtr->DspMsg;

    switch (DspMsg)
    {
        case DSP_MSG_STOP_PRT:
        case DSP_MSG_STOP_AVT:
            return TRUE;
        default:
            return FALSE;
    }
}



/**
 * DSP_CommandParameterConfig
 *
 * @Author : Yoyo <SYChiu@airoha.com.tw>
 */
VOID DSP_CommandParameterConfig(DSP_CMD_PTR_t DspCmdPtr, DSP_CMD_PARA_PTR_t DspCmdParaPtr)
{
    U16 DspCmdParaSize = sizeof(DSP_CMD_PARA_t)/sizeof(U8);
    memcpy(&DspCmdPtr->DspCmdPara, DspCmdParaPtr, DspCmdParaSize);
}

/**
 * DSP_CommandUpdateFeaturePara
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_CommandUpdateFeaturePara(TRANSFORM transform, DSP_UPDATE_TYPE_t type, U8* dataPtr, U32 dataLength)
{
    DSP_CMD_PTR_t DuplicateDspCmdPtr;

    U16 DspCmdSize  = (DSP_UPDATE_COMMAND_STRUCTURE_SIZE+dataLength)/sizeof(U8);


    if ((DuplicateDspCmdPtr = OS_malloc(DspCmdSize)) != NULL)
    {
        DuplicateDspCmdPtr->DspMsg = DSP_UPDATE_PARAMETER;
        DuplicateDspCmdPtr->TaskID = transform->source->taskId;
        DuplicateDspCmdPtr->DspCmdPara.UpdatePara.transform  = transform;
        DuplicateDspCmdPtr->DspCmdPara.UpdatePara.type       = type;
        DuplicateDspCmdPtr->DspCmdPara.UpdatePara.sequence   = DSP_UPDATE_COMMAND_FEATURE_PARA_SEQUENCE_AUTODETECT;
        DuplicateDspCmdPtr->DspCmdPara.UpdatePara.dataLength = dataLength;

        memcpy(&DuplicateDspCmdPtr->DspCmdPara.UpdatePara.dataBegin,
               dataPtr,
               dataLength);

        osTaskPutIPCMsg(DuplicateDspCmdPtr->TaskID, DuplicateDspCmdPtr);
        return TRUE;
    }

    return FALSE;
}


/**
 * DSP_CommandUpdateFeatureParaAssign
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 */
BOOL DSP_CommandUpdateFeatureParaAssign(TRANSFORM transform, DSP_UPDATE_TYPE_t type, U32 sequence, U8* dataPtr, U32 dataLength)
{
    DSP_CMD_PTR_t DuplicateDspCmdPtr;

    U16 DspCmdSize  = (DSP_UPDATE_COMMAND_STRUCTURE_SIZE+dataLength)/sizeof(U8);

    if ((DuplicateDspCmdPtr = OS_malloc(DspCmdSize)) != NULL)
    {
        DuplicateDspCmdPtr->DspMsg = DSP_UPDATE_PARAMETER;
        DuplicateDspCmdPtr->TaskID = transform->source->taskId;
        DuplicateDspCmdPtr->DspCmdPara.UpdatePara.transform  = transform;
        DuplicateDspCmdPtr->DspCmdPara.UpdatePara.type       = type;
        DuplicateDspCmdPtr->DspCmdPara.UpdatePara.sequence   = sequence;
        DuplicateDspCmdPtr->DspCmdPara.UpdatePara.dataLength = dataLength;

        memcpy(&DuplicateDspCmdPtr->DspCmdPara.UpdatePara.dataBegin,
               dataPtr,
               dataLength);

        osTaskPutIPCMsg(DuplicateDspCmdPtr->TaskID, DuplicateDspCmdPtr);
        return TRUE;
    }

    return FALSE;
}
#endif

