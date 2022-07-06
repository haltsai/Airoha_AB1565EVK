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
 
#ifndef _DSP_TASK_H_
#define _DSP_TASK_H_

#include "types.h"
#include "FreeRTOS.h"
#include "task.h"


/******************************************************************************
 * External Global Variables
 ******************************************************************************/


EXTERN TaskHandle_t  pDTM_TaskHandler;
EXTERN TaskHandle_t  pDAV_TaskHandler;
EXTERN TaskHandle_t  pDPR_TaskHandler;
EXTERN TaskHandle_t  pDHP_TaskHandler;
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
EXTERN TaskHandle_t  pDLL_TaskHandler;
#endif

#define	DAV_TASK_ID pDAV_TaskHandler
#define	DPR_TASK_ID pDPR_TaskHandler
#define	DHP_TASK_ID pDHP_TaskHandler
#define	DSP_TASK_ID pDTM_TaskHandler
#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
#define	DLL_TASK_ID pDLL_TaskHandler
#endif
#define	NULL_TASK_ID NULL





/******************************************************************************
 * Inline Functions
 ******************************************************************************/
#define DSP_OFFSET_OF(type,member) ((SIZE)&(((type *)0)->member))
	
	/**
	 * @brief cast a member of a structure out to the containing structure
	 * @param ptr			  the pointer to the member.
	 * @param type			  the type of the container struct this is embedded in.
	 * @param member		  the name of the member within the struct.
	 */
#define DSP_CONTAINER_OF(ptr,type,member) ((type *)((U8 *)(ptr) - DSP_OFFSET_OF(type,member)))

#define DSP_UPDATE_COMMAND_FEATURE_PARA_SEQUENCE_AUTODETECT  0xFFFFFFFF

/******************************************************************************
* Interrupt handler related
******************************************************************************/
#define INTR_ID_SRC_IN                       (0)
#define INTR_ID_IDFE_AU                      (1)
#define INTR_ID_ODFE_AU                      (2)
#define INTR_ID_ODFE_VP                      (3)
#define INTR_ID_SPDIF                        (4)

EXTERN VOID INTR_RegisterHandler( U8 IntrId, VOID* IntrHdlr, TaskHandle_t  TaskId);
EXTERN VOID INTR_CancelHandler(U8 IntrId);

typedef VOID (*CRITICALFUN)(VOID* para);
EXTERN	VOID PL_CRITICAL(CRITICALFUN pFun , VOID* para);


#endif /* _DSP_TASK_H_ */

