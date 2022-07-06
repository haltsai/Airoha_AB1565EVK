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

#include "dsp_audio_process.h"
#include "dsp_para_ins.h"
#include "ins_interface.h"
#include "dsp_callback.h"
#include "dsp_sdk.h"
#include "dsp_memory.h"
#include "audio_nvdm_common.h"
#include "dsp_dump.h"

#ifdef MTK_BT_A2DP_CPD_USE_PIC
#include "cpd_portable.h"
#endif

/*
 * Buffer Control
*/
INS_INSTANCE_ptr InsMemoryPtr = NULL;

#define INS_AU_FRAME_SIZE      (32)
#define INS_VALID_MEMORY_CHECK_VALUE              ((U32)0x6128800)
#define INS_RUNTIME_INVALID_MEMORY_CHECK_VALUE    ((U32)0x5A5A5A5A)
#define INS_MEMSIZE          (sizeof(INS_INSTANCE_t))


/**
 *
 * Function Prototype
 *
 */
bool INS_MemInit (void* para);
bool INS_MemCheck ();



/**
 * INS_MemInit
 *
 * This function is used to init memory space for INS process
 *
 *
 * @para : Default parameter of callback function
 * @return : Initialize result
 */
bool INS_MemInit (void* para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    if (!INS_MemCheck())
    {
        if (!(InsMemoryPtr && (InsMemoryPtr->MemoryCheck == INS_RUNTIME_INVALID_MEMORY_CHECK_VALUE))) {
           InsMemoryPtr = (INS_INSTANCE_ptr)DSPMEM_tmalloc(DAV_TASK_ID, INS_MEMSIZE+get_INS_memsize(), stream_ptr);
        }
        InsMemoryPtr->MemoryCheck = INS_VALID_MEMORY_CHECK_VALUE;
        return FALSE;
    }
    return TRUE;
}

/**
 * INS_MemCheck
 *
 * This function is used to check init memory space for INS process
 *
 *
 * @para : Default parameter of callback function
 * @return : Check result
 */
bool INS_MemCheck ()
{
    if(NULL != InsMemoryPtr) {
        if(INS_VALID_MEMORY_CHECK_VALUE == InsMemoryPtr->MemoryCheck) {
            return TRUE;
        }
    }
    return FALSE;
}

void stream_function_ins_deinitialize (void)
{
    if (InsMemoryPtr) {
        InsMemoryPtr->MemoryCheck = INS_RUNTIME_INVALID_MEMORY_CHECK_VALUE;
    }
}

/**
 * Audio_INS_Init
 *
 * This function is used to init memory space for audio INS
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_ins_audio_initialize (VOID* para)
{
    if (INS_MemInit(para))
    {
        return TRUE;
    }

    void * p_scratch = &InsMemoryPtr->ScratchMemory[0];
    void * p_nvkey = &InsMemoryPtr->NvKey;

    nvkey_read_full_key(NVKEY_DSP_PARA_INS, p_nvkey, sizeof(INS_NVKEY_STATE));

    INS_Init(p_scratch, p_nvkey);
    return FALSE;
}

bool ins_audio_process (VOID* para)
{
    S32* BufL = (S32*)stream_function_get_1st_inout_buffer(para);
    S32* BufR = (S32*)stream_function_get_2nd_inout_buffer(para);
    U16 FrameSize = stream_function_get_output_size(para);
    S16 dev_channels = (S16)stream_function_get_device_channel_number(para);
    U16 InsSampleSize;
    S16 channel_mode = 2; // 2:stereo 1:mono
    void * p_scratch = &InsMemoryPtr->ScratchMemory[0];
    void * p_nvkey = &InsMemoryPtr->NvKey;
    UNUSED(p_nvkey);
	
    if (InsMemoryPtr->NvKey.ENABLE == 0)
    {
        return FALSE;
    }

    if ((0 == FrameSize) || (FrameSize%INS_AU_FRAME_SIZE))
    {
        return FALSE;
    }

    if (dev_channels == 1)
    {
        BufR = NULL;
        channel_mode = 1;
    }

    if (stream_function_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        InsSampleSize = FrameSize >> 2;
    }
    else
    {
        InsSampleSize = FrameSize >> 1;
        dsp_converter_16bit_to_32bit(BufL, (S16*)BufL, InsSampleSize);
        dsp_converter_16bit_to_32bit(BufR, (S16*)BufR, InsSampleSize);
        stream_function_modify_output_size(para, InsSampleSize << 2);
        stream_function_modify_output_resolution(para, RESOLUTION_32BIT);
    }

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)BufL, (U32)(InsSampleSize << 2), AUDIO_INS_IN_L);
    if (BufR) {
        LOG_AUDIO_DUMP((U8*)BufR, (U32)(InsSampleSize << 2), AUDIO_INS_IN_R);
    }
#endif

    channel_mode = (BufR == NULL) ? 1 : 2;
    INS_Prcs(p_scratch, BufL, BufR, InsSampleSize, channel_mode);

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)BufL, (U32)(InsSampleSize << 2), AUDIO_INS_OUT_L);
    if (BufR) {
        LOG_AUDIO_DUMP((U8*)BufR, (U32)(InsSampleSize << 2), AUDIO_INS_OUT_R);
    }
#endif
    return FALSE;
}

/**
 * stream_function_ins_audio_process
 *
 * audio INS main process
 *
 *
 * @para : Default parameter of callback function
 *
 */
bool stream_function_ins_audio_process (void *para)
{
    return ins_audio_process(para);
}

