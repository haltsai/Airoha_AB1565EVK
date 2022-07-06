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
#include "dsp_callback.h"
#include "audio_nvdm_common.h"
#include "agc_interface.h"
#include "aec_nr_interface.h"
#ifdef MTK_BT_AGC_USE_PIC
#include "agc_portable.h"
#endif
#include "dsp_dump.h"


/**
 * Voice_Rx_AGC_Init
 *
 * This function is used to init the NVKey and memory space for voice Rx_AGC
 *
 * @Author : PengTzu <PengTzu.Chen@airoha.com.tw>
 *
 */
bool stream_function_rx_agc_initialize (void *para)
{
    SCO_CODEC sco_type = gDspAlgParameter.EscoMode.Rx;
    AGC_VO_INSTANCE_t *agc_st_ptr = (AGC_VO_INSTANCE_t *)stream_function_get_working_buffer(para);

    switch(sco_type)
    {
        case CVSD:
            nvkey_read_full_key(NVKEY_DSP_PARA_VOICE_NB_RX_AGC, &agc_st_ptr->NvKey, sizeof(AGC_VO_NVKEY));
            AGC_VO_Init(agc_st_ptr, &agc_st_ptr->NvKey, AGC_WB); // 1568 NB's data will be upsampled, so uesed AGC_WB directly
            DSP_MW_LOG_D("CVSD Rx AGC init done\r\n", 0);
            break;
        case mSBC:
            nvkey_read_full_key(NVKEY_DSP_PARA_VOICE_WB_RX_AGC, &agc_st_ptr->NvKey, sizeof(AGC_VO_NVKEY));
            AGC_VO_Init(agc_st_ptr, &agc_st_ptr->NvKey, AGC_WB);
            DSP_MW_LOG_D("mSBC Rx AGC init done\r\n", 0);
            break;
        default:
            return TRUE;
    }

    return FALSE;
}


/**
 * Voice_Tx_AGC_Init
 *
 * This function is used to init the NVKey and memory space for voice Tx_AGC
 *
 * @Author : PengTzu <PengTzu.Chen@airoha.com.tw>
 *
 */
bool stream_function_tx_agc_initialize (void *para)
{
    AGC_VO_INSTANCE_t *agc_st_ptr = (AGC_VO_INSTANCE_t *)stream_function_get_working_buffer(para);

    nvkey_read_full_key(NVKEY_DSP_PARA_VOICE_TX_AGC, &agc_st_ptr->NvKey, sizeof(AGC_VO_NVKEY));
    AGC_VO_Init(agc_st_ptr, &agc_st_ptr->NvKey, AGC_WB);
    DSP_MW_LOG_D("Tx AGC init done\r\n", 0);

    return FALSE;
}


/**
 * Voice_Rx_AGC_Process
 *
 * The main process for Rx_AGC
 *
 * @Author : PengTzu <PengTzu.Chen@airoha.com.tw>
 *
 */
bool stream_function_rx_agc_process (void *para)
{
    S16* Buf      = (S16*)stream_function_get_1st_inout_buffer(para);
    S16  Length    = (U16)stream_function_get_output_size(para);
    U8   BytesPerSample = (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) ? 4 : 2;
    AGC_VO_INSTANCE_t *agc_st_ptr = (AGC_VO_INSTANCE_t *)stream_function_get_working_buffer(para);

    AGC_Proc(agc_st_ptr, Buf, 0, Length/BytesPerSample);

    return FALSE;
}


/**
 * Voice_Tx_AGC_Process
 *
 * The main process for Tx_AGC
 *
 * @Author : PengTzu <PengTzu.Chen@airoha.com.tw>
 *
 */
bool stream_function_tx_agc_process (void *para)
{
    S16* Buf      = (S16*)stream_function_get_1st_inout_buffer(para);
    S16  Length    = (U16)stream_function_get_output_size(para);
    U8   BytesPerSample = (stream_function_get_output_resolution(para) == RESOLUTION_32BIT) ? 4 : 2;
    AGC_VO_INSTANCE_t *agc_st_ptr = (AGC_VO_INSTANCE_t *)stream_function_get_working_buffer(para);

#ifdef TX_AGC_ECNR_PITCH_LOG
    if(gDspAlgParameter.ECNR_Pitch_Last == 1){
       DSP_MW_LOG_I("Tx_AGC process, gECNR_Pitch_Last:%d\r\n", 1, gDspAlgParameter.ECNR_Pitch_Last);
    }
#endif
    AGC_Proc(agc_st_ptr, Buf, (S16)gDspAlgParameter.ECNR_Pitch_Last, Length/BytesPerSample);

    return FALSE;
}



