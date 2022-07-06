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

#include "igo_nr_interface.h"
#include "igo_nr_portable.h"
#include "dsp_dump.h"
#include "dsp_para_reserved.h"

void* p_igo_NvKey;

bool stream_function_igo_nr_initialize (void *para)
{
    void *tx_nr_ptr = stream_function_get_working_buffer(para);

    if (tx_nr_ptr != NULL) {
        IGO_1MIC_NR_PARAMS nr_params;
        DSP_PARA_RESERVED_STRU ResKey;
        p_igo_NvKey = (void *)&ResKey;
        nvkey_read_full_key(NVKEY_DSP_PARA_RESERVED, p_igo_NvKey, sizeof(DSP_PARA_RESERVED_STRU));
        memcpy(&nr_params, p_igo_NvKey, sizeof(IGO_1MIC_NR_PARAMS));

        IGO_NR_Init(tx_nr_ptr, &nr_params);
    }
    return false;
}


ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_function_igo_nr_process (void *para)
{
    U16  Length      = (U16)stream_function_get_output_size(para);
    void*prWorkBuf   = stream_function_get_working_buffer(para);
    S16* prInBuf     = (S16*)stream_function_get_1st_inout_buffer(para);
    S16  NrOutBuf[Length];

#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP(prInBuf, Length, AUDIO_WOOFER_RX);
#endif
    IGO_NR_Prcs(prInBuf, NrOutBuf, prWorkBuf);
    memcpy(prInBuf, NrOutBuf, Length);
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP(prInBuf, Length, AUDIO_WOOFER_UPSAMPLE_8K);
#endif
    return false;
}

