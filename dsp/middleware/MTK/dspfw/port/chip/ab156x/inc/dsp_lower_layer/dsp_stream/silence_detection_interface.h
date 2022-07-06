/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#include "types.h"
#include "sink_inter.h"
#include "sink.h"
#include "stream.h"

#define Silence_DetectTime_s 1 //defult setting: 1s

typedef void (*FuncPtr)(BOOL flag);

typedef struct STRU_SD_PARA_s
{
    S32 AutoPowerOff_TH_dB;
    U32 AutoPowerOff_Time;
    S32 NLE_TH_dB;
    U32 NLE_Time;
    S32 ExtAmpOff_TH_dB;
    U32 ExtAmpOff_Time;
    BOOL APO_isEnable;
    BOOL NLE_isEnable;    
    BOOL EAO_isEnable;
}PACKED SD_NVKEY_STATE;

typedef struct SD_INSTANCE_s
{
    S32 AutoPowerOff_TH_32;
    S32 NLE_TH_32;
    S32 ExtAmpOff_TH_32;
    S32 AutoPowerOff_TH_16;
    S32 NLE_TH_16;
    S32 ExtAmpOff_TH_16;
    U32 DetectTime_s; 
    S32 APO_MaxValue_16;
    S32 NLE_MaxValue_16;
    S32 EAO_MaxValue_16;
    S32 APO_MaxValue_32;
    S32 NLE_MaxValue_32;
    S32 EAO_MaxValue_32;
    U64 SampleCntAccumulate;
    U32 StartCnt;
    BOOL APO_SilenceFlag;
    BOOL NLE_SilenceFlag;
    BOOL EAO_SilenceFlag;
    BOOL RegisteredSink[SINK_TYPE_MAX];
    FuncPtr APO_FunPtr;
    FuncPtr NLE_FunPtr;
    FuncPtr EAO_FunPtr;
    BOOL Mutex;
    SD_NVKEY_STATE NvKey;    
} SD_INSTANCE_t, *SD_INSTANCE_ptr;

EXTERN void Sink_Audio_SilenceDetection_Register(SINK sink);
EXTERN void Sink_Audio_SilenceDetection_Unregister(SINK sink);
EXTERN void Sink_Audio_SilenceDetection(VOID* SrcBuf, U16 CopySize, SINK sink);
EXTERN U8   Sink_Audio_SilenceDetection_Get_InitCnt(void);
EXTERN void Sink_Audio_SilenceDetection_Init(VOID *APO_FunPtr,VOID *NLE_FunPtr,VOID *EAO_FunPtr);
EXTERN void Sink_Audio_SilenceDetection_Load_Nvkey(void *nvkey);

EXTERN U32 Get_AutoPowerOff_Time(void);
EXTERN U32 Get_NLE_Time(void);
EXTERN U32 Get_ExtAmpOff_Time(void);

