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
#include "dsp_feature_interface.h"
#include "dsp_buffer.h"
#include "dsp_audio_ctrl.h"
#include "dsp_share_memory.h"
#include "dsp_memory.h"
#include "dsp_callback.h"
#include "audio_nvdm_common.h"
#include "tx_eq_interface.h"
#ifdef MTK_BT_A2DP_ECNR_USE_PIC
#include "tx_eq_portable.h"
#endif
#include "dsp_dump.h"

/**
 *
 *  Definition
 *
 */
#define TX_EQ_VALID_MEMORY_CHECK_VALUE              ((U32)0x12345678)
#define TX_EQ_RUNTIME_INVALID_MEMORY_CHECK_VALUE    ((U32)0x87654321)

/**
 *
 *  Buffer & Control
 *
 */
TX_EQ_INSTANCE_PTR TxEqMemoryPtr = NULL;


/**
 * stream_function_tx_eq_initialize
 *
 * This function is used to init the NVKey and memory space for voice TX_EQ
 *
 * @Author : PengTzu <PengTzu.Chen@airoha.com.tw>
 *
 */
bool stream_function_tx_eq_initialize (void *para)
{
    SCO_CODEC sco_type = gDspAlgParameter.EscoMode.Rx;

    if (tx_eq_mem_init(para))
    {
        return FALSE;
    }

    switch(sco_type)
    {
        case CVSD:
            nvkey_read_full_key(NVKEY_DSP_PARA_NB_TX_FIR_EQ, &TxEqMemoryPtr->NvKey.TxFirEq, sizeof(DSP_PARA_NB_TX_FIR_EQ_STRU));
            Voice_TxEQ_init(&TxEqMemoryPtr->ScratchMemory, &TxEqMemoryPtr->NvKey.TxFirEq); // 1568 NB's data will be upsampled, so uesed AGC_WB directly
            DSP_MW_LOG_I("NB TxEQ init done\r\n", 0);
            break;
        case mSBC:
            nvkey_read_full_key(NVKEY_DSP_PARA_WB_TX_FIR_EQ, &TxEqMemoryPtr->NvKey.TxFirEq, sizeof(DSP_PARA_WB_TX_FIR_EQ_STRU));
            Voice_TxEQ_init(&TxEqMemoryPtr->ScratchMemory, &TxEqMemoryPtr->NvKey.TxFirEq);
            DSP_MW_LOG_I("WB TxEQ init done\r\n", 0);
            break;
        default:
            return TRUE;
    }

    return FALSE;
}


/**
 * stream_function_tx_eq_deinitialize
 *
 * This function is used to check de-init memory space for TX_EQ process
 *
 *
 * @para : Default parameter of callback function
 * @return : Check result
 */
void stream_function_tx_eq_deinitialize (void)
{
    if (TxEqMemoryPtr) {
        TxEqMemoryPtr->MemoryCheck = TX_EQ_RUNTIME_INVALID_MEMORY_CHECK_VALUE;
    }
}


/**
 * TX_EQ_MemCheck
 *
 * This function is used to check init memory space for TX_EQ process
 *
 *
 * @para : Default parameter of callback function
 * @return : Check result
 */
bool tx_eq_mem_check (void)
{
    if (NULL != TxEqMemoryPtr)
    {
        if (TX_EQ_VALID_MEMORY_CHECK_VALUE == TxEqMemoryPtr->MemoryCheck)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/**
 * TX_EQ_MemInit
 *
 * This function is used to init memory space for TX_EQ process
 *
 *
 * @para : Default parameter of callback function
 * @return : Initialize result
 */
bool tx_eq_mem_init (void* para)
{
    DSP_STREAMING_PARA_PTR stream_ptr;
    stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    if (!tx_eq_mem_check())
    {
        if (!(TxEqMemoryPtr && (TxEqMemoryPtr->MemoryCheck == TX_EQ_RUNTIME_INVALID_MEMORY_CHECK_VALUE))) {
            TxEqMemoryPtr = (TX_EQ_INSTANCE_PTR)DSPMEM_tmalloc(DAV_TASK_ID, DSP_TX_EQ_INSTANCE_MEMSIZE+get_fir_wbeq_memsize(), stream_ptr);
        }
        TxEqMemoryPtr->MemoryCheck = TX_EQ_VALID_MEMORY_CHECK_VALUE;
        return FALSE;
    }
    return TRUE;
}




/**
 * stream_function_tx_eq_process
 *
 * The main process for TX_EQ
 *
 * @Author : PengTzu <PengTzu.Chen@airoha.com.tw>
 *
 */
bool stream_function_tx_eq_process (void *para)
{
    S16* Buf      = (S16*)stream_function_get_1st_inout_buffer(para);
    S16  Length    = (U16)stream_function_get_output_size(para);

    if (Length == 0) {
        DSP_MW_LOG_W("[DSP][AEC/NR] bypass the TX EQ process", 0);
        return FALSE;
    }

    //LOG_AUDIO_DUMP((U8*)Buf, Length, AUDIO_SOUNDBAR_INPUT);
    Voice_TxEQ_Prcs_Length(Buf, &TxEqMemoryPtr->ScratchMemory, Length>>1); //length unit: sample
    //LOG_AUDIO_DUMP((U8*)Buf, Length, AUDIO_SOUNDBAR_TX);
    //DSP_MW_LOG_I("TxEQ process done\r\n", 0);

    return FALSE;
}

