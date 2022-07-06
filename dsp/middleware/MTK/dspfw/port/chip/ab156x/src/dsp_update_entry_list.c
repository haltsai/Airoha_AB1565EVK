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

//-
#include "dsp_sdk.h"
#include "dsp_update_para.h"



////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
// Function Prototypes /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BOOL    DSP_Update_DRCGain_Entry       (VOID* FuncMemPtr, VOID* dataBeginPtr, U32 dataLength);
BOOL    DSP_Update_PEQFilter_Entry     (VOID* FuncMemPtr, VOID* dataBeginPtr, U32 dataLength);
BOOL    DSP_Update_JointCh_Entry       (VOID* FuncMemPtr, VOID* dataBeginPtr, U32 dataLength);



////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
UPDATE_PARA_ENTRY DSP_UpdateParaEntryTable[DSP_UPDATE_MAX_NUM] =
{
    DSP_Update_DRCGain_Entry,       /*DSP_UPDATE_DRC_GAIN*/
    DSP_Update_PEQFilter_Entry,     /*DSP_UPDATE_PEQ_FILTER*/
    DSP_Update_JointCh_Entry,       /*DSP_UPDATE_JOINT_CHANNEL*/
};

stream_feature_type_t DSP_UpdateCheckFeatureTypeTable[DSP_UPDATE_MAX_NUM] =
{
    FUNC_END,                       /*DSP_UPDATE_DRC_GAIN*/
    FUNC_END,                       /*DSP_UPDATE_PEQ_FILTER*/
    FUNC_JOINT,                     /*DSP_UPDATE_JOINT_CHANNEL*/
};

////////////////////////////////////////////////////////////////////////////////
// DSPMEM FUNCTION DECLARATIONS /////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/**
 * DSP_Update_DRCGain_Entry
 *
 * Update DRC digital gain value
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @FuncMemPtr   : Function memory pointer
 * @dataBeginPtr : data pointer
 * @dataLength   : data length
 *
 * @return FALSE if the streaming should not be initialized, TRUE otherwise.
 */
BOOL DSP_Update_DRCGain_Entry(VOID* FuncMemPtr, VOID* dataBeginPtr, U32 dataLength)
{
    UNUSED(FuncMemPtr);
    UNUSED(dataBeginPtr);
    UNUSED(dataLength);
    return FALSE;
}


/**
 * DSP_Update_PEQFilter_Entry
 *
 * Update PEQ Filter coefficient
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @FuncMemPtr   : Function memory pointer
 * @dataBeginPtr : data pointer
 * @dataLength   : data length
 *
 * @return FALSE if the streaming should not be initialized, TRUE otherwise.
 */
BOOL DSP_Update_PEQFilter_Entry(VOID* FuncMemPtr, VOID* dataBeginPtr, U32 dataLength)
{
    UNUSED(FuncMemPtr);
    UNUSED(dataBeginPtr);
    UNUSED(dataLength);
    return FALSE;
}


/**
 * DSP_Update_JointCh_Entry
 *
 * Joint Channel select
 *
 * @Author :  BrianChen <BrianChen@airoha.com.tw>
 *
 * @FuncMemPtr   : Function memory pointer
 * @dataBeginPtr : data pointer
 * @dataLength   : data length
 *
 * @return FALSE if the streaming should not be initialized, TRUE otherwise.
 */
BOOL DSP_Update_JointCh_Entry(VOID* FuncMemPtr, VOID* dataBeginPtr, U32 dataLength)
{
    UNUSED(FuncMemPtr);
    UNUSED(dataBeginPtr);
    UNUSED(dataLength);
    return FALSE;
}

