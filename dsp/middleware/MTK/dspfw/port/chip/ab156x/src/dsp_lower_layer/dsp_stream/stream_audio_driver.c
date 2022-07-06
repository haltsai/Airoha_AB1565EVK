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

/*!
 *@file   stream_audio_driver.c
 *@brief  Defines the audio stream
 *
 @verbatim
         * Programmer : SYChiu@airoha.com.tw, Ext.3307
         * Programmer : BrianChen@airoha.com.tw, Ext.2641
         * Programmer : MachiWu@airoha.com.tw, Ext.2673
 @endverbatim
 */

//-
#include "types.h"
#include "config.h"
#include "audio_config.h"
#include "dsp_buffer.h"
#include "source.h"
#include "sink.h"
#include "stream.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "davt.h"
#include "dsp_audio_process.h"
#include "dsp_memory.h"
#include "stream_audio.h"
#include "dtm.h"
#include "stream_audio_driver.h"
#include "stream_audio_hardware.h"
#include "stream_audio_setting.h"
#include "dsp_audio_ctrl.h"
#include "dsp_drv_afe.h"
#include "dsp_dump.h"
#include <string.h>
#include "dsp_audio_process.h"
#include "silence_detection_interface.h"

//for ab1568
#include "hal_audio_control.h"
#ifdef ENABLE_HWSRC_CLKSKEW
#include "clk_skew.h"
#endif

Audiohandler Audio_handler;
#define IRQ_RATE_NUM    15
#define IRQ_COUNT_MAX   (1 << 18)
const uint32_t Irq_rate_table[IRQ_RATE_NUM] = {
    8000, 11025, 12000, 384000, 16000, 22050, 24000, 130000, 32000, 44100, 48000, 88200, 96000, 176400, 192000
};

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define RLEN(woff, roff, len) ((woff >= roff) ? (woff - roff) :(len - roff + woff))

#define AUDIO_SINK_ZERO_PADDING_FRAME_NUM 2
//#define TestReadPattern


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
BOOL Stream_Audio_Handler(SOURCE source , SINK sink);
VOID Sink_Audio_Path_Init(SINK sink);
VOID Sink_Audio_Buffer_Ctrl(SINK sink, BOOL isEnabled);
VOID Sink_Audio_UpdateReadOffset(SINK_TYPE type);
VOID Sink_Audio_SubPath_Init(SINK sink);
BOOL Sink_Audio_ZeroPadding();
VOID Sink_Audio_SRC_Ctrl(SINK sink, BOOL isEnabled, stream_samplerate_t value);
VOID Sink_Audio_Triger_SourceSRC(SOURCE source);
VOID Sink_Audio_SRC_A_CDM_SamplingRate_Reset(SINK sink, stream_samplerate_t rate);
VOID Sink_Audio_ADMAIsrHandler(VOID);
VOID Sink_Audio_SRCIsrHandler(VOID);
VOID Sink_Audio_VpRtIsrHandler(VOID);
VOID Source_Audio_Pattern_Init(SOURCE source);
VOID Source_Audio_Path_Init(SOURCE source);
VOID Source_Audio_SRC_Ctrl(SOURCE source, BOOL ctrl, stream_samplerate_t value);
VOID Source_Audio_Buffer_Ctrl(SOURCE source, BOOL ctrl);
VOID Source_Audio_Triger_SinkSRC(SINK sink);
VOID Source_Audio_IsrHandler(VOID);
VOID Source_Audio_Spdif_Rx_IsrHandler(VOID);
VOID Source_Audio_SelectInstance(audio_hardware hardware, audio_instance instance);


#if (CODE_BEGIN)
#endif

ATTR_TEXT_IN_IRAM_LEVEL_1 VOID AudioCheckTransformHandle(TRANSFORM transform)
{
    if (transform != NULL) {
        if(transform->source->type != SOURCE_TYPE_A2DP)
        {
            // printf("Audio_handler wakeup from sink buffer++\r\n");
            Audio_handler = (transform->Handler);   // Stream_Audio_Handler
            if(Audio_handler == NULL)
                DSP_MW_LOG_I("Audio_handler = NULL\r\n", 0);
            else
                Audio_handler(transform->source,transform->sink);
            // printf("Audio_handler wakeup from sink buffer--\r\n");
        }

        if (((transform->sink->taskid == DHP_TASK_ID)&&(transform->sink->type!=SINK_TYPE_DSP_VIRTUAL)) || (transform->source->taskId == DHP_TASK_ID)) {
            xTaskResumeFromISR(pDHP_TaskHandler);
        } else if (((transform->sink->taskid == DPR_TASK_ID)&&(transform->sink->type!=SINK_TYPE_DSP_VIRTUAL)) || (transform->source->taskId == DPR_TASK_ID)) {
            xTaskResumeFromISR(pDPR_TaskHandler);
        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        } else if (((transform->sink->taskid == DLL_TASK_ID)&&(transform->sink->type!=SINK_TYPE_DSP_VIRTUAL)) || (transform->source->taskId == DLL_TASK_ID)) {
            xTaskResumeFromISR(pDLL_TaskHandler);
        #endif
        } else {
            xTaskResumeFromISR(pDAV_TaskHandler);
        }
        portYIELD_FROM_ISR(pdTRUE);            // force to do context switch
    }
}

ATTR_TEXT_IN_IRAM BOOL Stream_Audio_Handler(SOURCE source , SINK sink)
{
    U16 length_src, length_snk, length;
    DSP_CALLBACK_PTR callback_ptr;
    BOOL file_done_flag = FALSE;
    DSP_STREAMING_PARA_PTR pStream;

    callback_ptr = DSP_Callback_Get(source, sink);
    pStream = DSP_Streaming_Get(source, sink);

    if ((source == NULL) || (sink == NULL) || (pStream == NULL))
    {
        return FALSE;
    }
    else if((callback_ptr != NULL)&&((callback_ptr->Status != CALLBACK_SUSPEND)&&(callback_ptr->Status != CALLBACK_WAITEND)))
    {
        return FALSE;
    }
    else if (pStream->streamingStatus == STREAMING_END)
    {
        if (sink->type == SINK_TYPE_AUDIO) {
            //DSP_MW_LOG_I("DSP Stream_Audio_Handler %d@@@@\r\n", 1, Audio_setting->Audio_sink.Zero_Padding_Cnt);
        }
        return FALSE;
    }

    if ((pStream->callback.EntryPara.force_resume) && (pStream->streamingStatus != STREAMING_START))
    {
        if (callback_ptr != NULL)
        {
            if ((callback_ptr->Status == CALLBACK_SUSPEND) || (callback_ptr->Status == CALLBACK_WAITEND))
            {
                callback_ptr->Status = CALLBACK_HANDLER;
            }
        }

        return TRUE;
    }

    #ifdef MTK_CELT_DEC_ENABLE
    if(source->type == SOURCE_TYPE_A2DP && source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT)
    {
        length = 1;
    }
    else
    #endif
    {
        length_src = SourceSize(source);
        length_snk = SinkSlack(sink);
        length = MIN(length_src, length_snk);
    }

    //printf("length_src %d, length_snk %d, ====> 0x%08x\n\r", length_src, length_snk, *(uint32_t*)(0xC0000010));

    if (length == 0)
    {
        if (source->type == SOURCE_TYPE_FILE)
        {
            if(SourceConfigure(source, FILE_EOF,0) == FALSE)
            {
                return FALSE;
            }
        }
        else if (source->type != SOURCE_TYPE_MEMORY)
        {
            return FALSE;
        }
        file_done_flag = TRUE;
    }

    #if (1)
    callback_ptr = DSP_Callback_Get(source, sink);
    // printf("callback_ptr status %d\n\r", callback_ptr->Status);
    if (callback_ptr != NULL)
    {
        if (callback_ptr->Status == CALLBACK_SUSPEND)
        {
            if (file_done_flag == TRUE)
            {
                callback_ptr->Status = CALLBACK_ZEROPADDING;
            }
            else if (DSP_Callback_BypassModeGet(source,sink) == BYPASS_CODEC_MODE)
            {
                callback_ptr->Status = CALLBACK_BYPASS_CODEC;
            }
            else
            {
                callback_ptr->Status = CALLBACK_HANDLER;
            }
        }
        else if ((callback_ptr->Status == CALLBACK_WAITEND)&&(length != 0))
        {
            callback_ptr->Status = CALLBACK_HANDLER;
        }
    }
    #else
    U16 i,j = 0;
    U16 out_channel_num = (sink->type == SINK_TYPE_AUDIO) ? sink->param.audio.channel_num : 1;
    U16 in_channel_num = (source->type == SOURCE_TYPE_AUDIO) ? source->param.audio.channel_num : 1;
    for (i = 0; i <out_channel_num; i++)
    {
        DSP_C2C_BufferCopy(sink->streamBuffer.BufferInfo.startaddr[i] + sink->streamBuffer.BufferInfo.WriteOffset,
                           source->streamBuffer.BufferInfo.startaddr[j] + source->streamBuffer.BufferInfo.ReadOffset,
                           length,
                           sink->streamBuffer.BufferInfo.startaddr[i],
                           sink->streamBuffer.BufferInfo.startaddr[i]+sink->streamBuffer.BufferInfo.length,
                           source->streamBuffer.BufferInfo.startaddr[j],
                           source->streamBuffer.BufferInfo.startaddr[j]+source->streamBuffer.BufferInfo.length);
        if (in_channel_num > j+1)
        {
            j++;
        }

    }
    if (SinkFlush(sink, (U32)length) == FALSE)
    {
        return FALSE;
    }

    SourceDrop(source, (U32)length);// Also wake up source to generate new data
    #endif
    return TRUE;
}

#ifdef AIR_AUDIO_HARDWARE_ENABLE
EXTERN U32 SinkSizeAudio(SINK sink);
hal_audio_interconn_selection_t stream_audio_convert_control_to_interconn(hal_audio_control_t audio_control, hal_audio_path_port_parameter_t port_parameter, uint32_t connection_sequence, bool is_input);

hal_audio_memory_selection_t hal_memory_convert_dl(hal_audio_memory_t memory);//for compatable ab1552
hal_audio_memory_selection_t hal_memory_convert_ul(hal_audio_memory_t memory);//for compatable ab1552
hal_audio_interconn_selection_t stream_audio_convert_control_to_interconn(hal_audio_control_t audio_control, hal_audio_path_port_parameter_t port_parameter, uint32_t connection_sequence, bool is_input)
{
    hal_audio_interconn_selection_t interconn_selection = HAL_AUDIO_INTERCONN_SEQUENCE_DUMMY;
    bool is_dual_channel = false;

    switch (audio_control) {
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL:
            is_dual_channel = true;
            interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_DAC_CH1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_L:
            interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_DAC_CH1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_R:
            interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_DAC_CH1;//HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_DAC_CH2;
            break;

        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL:
            is_dual_channel = true;
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L:
            if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH1;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL2_CH1;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL3_CH1;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_4) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL4_CH1;
            }
            break;
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R:
            if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH2;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL2_CH2;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL3_CH2;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_4) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL4_CH2;
            }
            break;


        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL:
            is_dual_channel = true;
            interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_L:
            interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_R:
            interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_UL1_CH2;
            break;

        case HAL_AUDIO_CONTROL_DEVICE_SPDIF:
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER:
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L:
            if(audio_control==HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER || audio_control==HAL_AUDIO_CONTROL_DEVICE_SPDIF)
            {
                is_dual_channel = true;
            }
            if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S0_CH1 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S0_CH1;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S1_CH1 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S1_CH1;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S2_CH1 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S2_CH1;
            }
            break;
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R:
            if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S0_CH2 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S0_CH2;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S1_CH2 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S1_CH2;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_MASTER_I2S2_CH2 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_MASTER_I2S2_CH2;
            }
            break;

        case HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE:
            is_dual_channel = true;
            if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S0_CH1 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S0_CH1;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S1_CH1 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S1_CH1;
            } else if (port_parameter.device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
                interconn_selection = (is_input) ? HAL_AUDIO_INTERCONN_SELECT_INPUT_DEVICE_SLAVE_I2S2_CH1 : HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SLAVE_I2S2_CH1;
            }
            break;

        case HAL_AUDIO_CONTROL_DEVICE_SIDETONE:
            interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_DEVICE_SIDETONE_CH1;
            break;

        case HAL_AUDIO_CONTROL_MEMORY_INTERFACE:
            is_dual_channel = true;
            if (port_parameter.memory_select & HAL_AUDIO_MEMORY_DL_DL1) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL1_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_DL_DL2) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL2_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_DL_DL3) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL3_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_DL_DL12) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_DL12_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_DL_SRC1) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC1_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_DL_SRC2) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_INPUT_MEMORY_SRC1_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_UL_VUL1) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_UL_VUL2) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_UL_VUL3) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_UL_AWB) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH1;
            } else if (port_parameter.memory_select & HAL_AUDIO_MEMORY_UL_AWB2) {
                interconn_selection = HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH1;
            }
            break;
        default:

            break;
    }
    if ((connection_sequence%2) && (is_dual_channel)) {
        //For channel 2
        interconn_selection++;
    }
    return interconn_selection;
}

hal_audio_memory_selection_t stream_audio_convert_interconn_to_memory(hal_audio_interconn_selection_t interconn_selection)
{
    hal_audio_memory_selection_t memory_select = 0;
    switch (interconn_selection) {
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL1_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL2_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_VUL2;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_VUL3_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_VUL3;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_AWB;
            break;
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH1:
        case HAL_AUDIO_INTERCONN_SELECT_OUTPUT_MEMORY_AWB2_CH2:
            memory_select = HAL_AUDIO_MEMORY_UL_AWB;
            break;
        default:
            break;
    }
    return memory_select;
}

hal_audio_memory_selection_t hal_memory_convert_dl(hal_audio_memory_t memory)
{
    hal_audio_memory_selection_t memory_select = HAL_AUDIO_MEM_DUMMY;

    switch (memory) {
        case HAL_AUDIO_MEM1://UL:DL1_data
            memory_select = HAL_AUDIO_MEMORY_DL_DL1;
            break;
        case HAL_AUDIO_MEM2://UL:DL2_data
            memory_select = HAL_AUDIO_MEMORY_DL_DL2;
            break;
        case HAL_AUDIO_MEM3://UL:DL3_data
            memory_select = HAL_AUDIO_MEMORY_DL_DL3;
            break;
        case HAL_AUDIO_MEM4://DL12_data
            memory_select = HAL_AUDIO_MEMORY_DL_DL12;
            break;
        case HAL_AUDIO_MEM6://SLAVE DMA data
            memory_select = HAL_AUDIO_MEMORY_DL_SLAVE_DMA;
            break;
        case HAL_AUDIO_MEM7://SLAVE TDM data
            memory_select = HAL_AUDIO_MEMORY_DL_SLAVE_TDM;
            break;
        default:
            break;
    }

    if (memory_select == HAL_AUDIO_MEM_DUMMY) {
        HAL_AUDIO_LOG_ERROR("DSP - Error DL memory_select:%d memory:%d !", 2, memory_select,memory);
        HAL_AUDIO_ASSERT(false);
    }

    return memory_select;
}

hal_audio_memory_selection_t hal_memory_convert_ul(hal_audio_memory_t memory)
{
    hal_audio_memory_selection_t memory_select = HAL_AUDIO_MEM_DUMMY;

    switch (memory) {
        case HAL_AUDIO_MEM1://VUL:VUL1_data
            memory_select = HAL_AUDIO_MEMORY_UL_VUL1;
            break;
        case HAL_AUDIO_MEM2://VUL:VUL2_data
            memory_select = HAL_AUDIO_MEMORY_UL_VUL2;
            break;
        case HAL_AUDIO_MEM3://VUL:AWB_data
            memory_select = HAL_AUDIO_MEMORY_UL_AWB;
            break;
        case HAL_AUDIO_MEM4://VUL:AWB2_data
            memory_select = HAL_AUDIO_MEMORY_UL_AWB2;
            break;
        case HAL_AUDIO_MEM5://VUL:VUL3_data
            memory_select = HAL_AUDIO_MEMORY_UL_VUL3;
            break;
        case HAL_AUDIO_MEM6://SLAVE DMA data
            memory_select = HAL_AUDIO_MEMORY_UL_SLAVE_DMA;
            break;
        case HAL_AUDIO_MEM7://SLAVE TDM data
            memory_select = HAL_AUDIO_MEMORY_UL_SLAVE_TDM;
            break;
        case HAL_AUDIO_MEM_SUB:
            return memory_select;
            break;
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
        case HAL_AUDIO_MEM_SUB | HAL_AUDIO_MEM4:
            return memory_select;
            break;
#endif
        default:
            break;
    }

    if (memory_select == HAL_AUDIO_MEM_DUMMY) {
        HAL_AUDIO_LOG_ERROR("DSP - Error UL memory_select:%d memory:%d !", 2, memory_select,memory);
        HAL_AUDIO_ASSERT(false);
    }

    return memory_select;
}

VOID Sink_Audio_Path_Init(SINK sink)
{
    Sink_Audio_Buffer_Ctrl(sink, TRUE);
    Audio_setting->Audio_sink.Output_Enable = FALSE;

    if (Audio_setting->Audio_sink.SRC_Out_Enable)
    {
        INTR_RegisterHandler(INTR_ID_SRC_IN,Sink_Audio_SRCIsrHandler,DAV_TASK_ID);
    }
    else
    {
        INTR_RegisterHandler(INTR_ID_ODFE_AU,Sink_Audio_ADMAIsrHandler,DAV_TASK_ID);
    }

    sink->type   = SINK_TYPE_AUDIO;
    sink->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;

    // Sink_Audio_Path_Interface_Init(sink); // Yo

    DSP_CTRL_ChangeStatus(AU_OUTPUT_DSP, AU_DSP_STATUS_ON);
}


VOID Sink_Audio_Path_Init_AFE(SINK sink)
{
    Sink_Audio_Buffer_Ctrl(sink, TRUE);

    Audio_setting->Audio_sink.Output_Enable = FALSE;

    // do .prepare() and .probe()
#if 0
    if (Audio_setting->Audio_sink.SRC_Out_Enable)
    {
        // TODO: turn on HW SRC IRQ and register HWSRC1 and 2 into OS
    }
    else
#endif
    {
#if 0
        if (sink->type == SINK_TYPE_AUDIO) {
            if (audio_sink_ops_open(sink))
                DSP_MW_LOG_I("audio sink: %x open error\r\n", 1, sink);
        }
#ifdef MTK_PROMPT_SOUND_ENABLE
        if (sink->type == SINK_TYPE_VP_AUDIO) {
            if (audio_sink_vp_ops_open(sink))
                DSP_MW_LOG_I("audio sink vp: %x open error\r\n", 1, sink);
        }
#endif
#else
    if (audio_ops_open(sink)) {
        DSP_MW_LOG_I("audio sink type : %d open error\r\n", 1, sink->type);
    }

#endif
    }

    if (sink->type == SINK_TYPE_AUDIO) {
        Audio_setting->Audio_sink.Zero_Padding_Cnt = AUDIO_SINK_ZEROPADDING_FRAME_NUM;
    }

    //sink->type   = SINK_TYPE_AUDIO;
    //sink->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;

    DSP_CTRL_ChangeStatus(AU_OUTPUT_DSP, AU_DSP_STATUS_ON);
}


VOID Sink_Audio_Buffer_Ctrl(SINK sink, BOOL isEnabled)
{
    U16 i;
    U8 * mem_ptr;
    U32 mem_size;
    audio_digital_block_t memory_block;
    uint32_t afe_number;

    if (isEnabled) {
        if ((sink->type == SINK_TYPE_AUDIO) || (sink->type == SINK_TYPE_AUDIO_DL3) || (sink->type == SINK_TYPE_AUDIO_DL12)) {
            sink->param.audio.frame_size =  Audio_setting->Audio_sink.Frame_Size*sink->param.audio.format_bytes;
            sink->streamBuffer.BufferInfo.length = sink->param.audio.frame_size*Audio_setting->Audio_sink.Buffer_Frame_Num;
            Audio_setting->Audio_sink.Output_Enable = FALSE;
        } else if (sink->type == SINK_TYPE_VP_AUDIO) {
            sink->param.audio.frame_size =  Audio_setting->Audio_VP.Frame_Size*sink->param.audio.format_bytes;
            sink->streamBuffer.BufferInfo.length = sink->param.audio.frame_size*Audio_setting->Audio_VP.Buffer_Frame_Num;
            Audio_setting->Audio_VP.Output_Enable = FALSE;
        } else if (sink->type == SINK_TYPE_DSP_JOINT) {
            sink->streamBuffer.BufferInfo.length = sink->param.audio.frame_size*Audio_setting->Audio_sink.Buffer_Frame_Num;
        }

        if (sink->buftype == BUFFER_TYPE_CIRCULAR_BUFFER) {
            mem_size = sink->streamBuffer.BufferInfo.length * sink->param.audio.channel_num;
            mem_ptr = DSPMEM_tmalloc(DSP_TASK_ID, mem_size, sink);
            memset(mem_ptr, 0, mem_size);

            for (i = 0; i<sink->param.audio.channel_num ; i++ ) {
                sink->streamBuffer.BufferInfo.startaddr[i]   = mem_ptr;
                mem_ptr += sink->streamBuffer.BufferInfo.length;
            }

        } else if (sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER) {
            memory_block =  hal_audio_afe_get_memory_digital_block (sink->param.audio.memory, true);

            sink->streamBuffer.BufferInfo.length = sink->param.audio.buffer_size;
            afe_number = ((sink->param.audio.channel_num+1)>>1);
            mem_size = sink->param.audio.buffer_size*afe_number;
            if (sink->param.audio.AfeBlkControl.u4asrcflag) {
                mem_size += sink->param.audio.AfeBlkControl.u4asrc_buffer_size;
            }
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
            else if (sink->type == SINK_TYPE_AUDIO) {
                mem_size += 4096; //for ASRC, equal  to ASRC_OUT_BUFFER_SIZE in stream.c
            }
#endif

            /* alloc memory */
            if (sink->type == SINK_TYPE_VP_AUDIO) {

                #if 0//modify for ab1568
                if(mem_size <= AFE_INTERNAL_SRAM_VP_SIZE) {
                    sink->param.audio.AfeBlkControl.phys_buffer_addr = AFE_INTERNAL_SRAM_VP_PHY_BASE;
                } else {
                    DSP_MW_LOG_I("DSP sram allocate fail sink type:%d, size:%d\r\n", 2, sink->type, mem_size);
                    configASSERT(false);
                }
                #else
                sink->param.audio.AfeBlkControl.phys_buffer_addr = hal_memory_allocate_sram(HAL_AUDIO_AGENT_MEMORY_DL2, mem_size);
                #endif
#ifdef MTK_TDM_ENABLE
            } else if (sink->type == SINK_TYPE_TDMAUDIO) {
                //I2S Slave Tdm mode
                hal_audio_agent_t agent;
                if (sink->param.audio.audio_interface == HAL_AUDIO_INTERFACE_2) {
                    agent = HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE;
                } else if (sink->param.audio.audio_interface == HAL_AUDIO_INTERFACE_3) {
                    agent = HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE;
                }
                afe_number = afe_number + sink->param.audio.device_handle.i2s_slave.tdm_channel;
                mem_size = mem_size + mem_size*sink->param.audio.device_handle.i2s_slave.tdm_channel;
                sink->param.audio.AfeBlkControl.phys_buffer_addr = hal_memory_allocate_sram(agent, mem_size);
#endif
            } else if (sink->type == SINK_TYPE_AUDIO_DL3) {
                sink->param.audio.AfeBlkControl.phys_buffer_addr = hal_memory_allocate_sram(HAL_AUDIO_AGENT_MEMORY_DL3, mem_size);
            } else if (sink->type == SINK_TYPE_AUDIO_DL12) {
                sink->param.audio.AfeBlkControl.phys_buffer_addr = hal_memory_allocate_sram(HAL_AUDIO_AGENT_MEMORY_DL12, mem_size);
            } else {

                #if 0//modify for ab1568
                if (afe_allocate_audio_sram(&sink->param.audio.AfeBlkControl, sink->param.audio.format, mem_size, false)) {
                    DSP_MW_LOG_I("DSP sram allocate fail sink type:%d, size:%d\r\n", 2, sink->type, mem_size);
                    configASSERT(false);
                }
                #else
                sink->param.audio.AfeBlkControl.phys_buffer_addr = hal_memory_allocate_sram(HAL_AUDIO_AGENT_MEMORY_DL1, mem_size);
                #endif
            }
            DSP_MW_LOG_I("audio sink buffer addr: 0x%x, sink_type:%d mem_size %d\n", 3, sink->param.audio.AfeBlkControl.phys_buffer_addr, sink->type,mem_size);

            mem_ptr = (U8*)sink->param.audio.AfeBlkControl.phys_buffer_addr;
            memset(mem_ptr, 0, mem_size);
            //modi for asrc
            /*channel = (sink->param.audio.AfeBlkControl.u4asrcflag) ? 1 : 0;
            for (i = 0; ((i<afe_number) && (i<BUFFER_INFO_CH_NUM)) ; i++ ) {
                sink->streamBuffer.BufferInfo.startaddr[channel%afe_number]   = mem_ptr;
                mem_ptr += sink->streamBuffer.BufferInfo.length;
                channel++;
            }*/
            if (sink->param.audio.AfeBlkControl.u4asrcflag) {
                sink->streamBuffer.BufferInfo.startaddr[afe_number%BUFFER_INFO_CH_NUM] = mem_ptr;
                mem_ptr += sink->param.audio.AfeBlkControl.u4asrc_buffer_size;
                DSP_MW_LOG_I("u4asrcflag mem_ptr %x  u4asrc_buffer_size %d\n", 2, mem_ptr, sink->param.audio.AfeBlkControl.u4asrc_buffer_size);
            }
            for (i = 0; ((i<afe_number) && (i<BUFFER_INFO_CH_NUM)) ; i++ ) {
                sink->streamBuffer.BufferInfo.startaddr[i] = mem_ptr;
                mem_ptr += sink->streamBuffer.BufferInfo.length;
                DSP_MW_LOG_I("afe_number %d startaddr %x mem_ptr %x BufferInfo.length %d audio.buffer_size %d\n", 5, i, sink->streamBuffer.BufferInfo.startaddr[i],mem_ptr,sink->streamBuffer.BufferInfo.length,sink->param.audio.buffer_size);
            }
            #if 0 //modify for ab1568 to slim code
            if (sink->type == SINK_TYPE_VP_AUDIO) {
                afe_set_mem_block(AFE_SINK_VP, memory_block);
            } else {
                afe_set_mem_block(AFE_SINK, memory_block);
            }
            #endif
        }
    } else {
        if (sink->buftype == BUFFER_TYPE_CIRCULAR_BUFFER) {
            DSPMEM_Free(DSP_TASK_ID, sink);
        } else if (sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER) {
            if (sink->type == SINK_TYPE_VP_AUDIO) {
                #if 1
                hal_memory_free_sram(HAL_AUDIO_AGENT_MEMORY_DL2);
                #else
                afe_free_audio_sram(AFE_SINK_VP);
                #endif
#ifdef MTK_TDM_ENABLE
            } else if (sink->type == SINK_TYPE_TDMAUDIO) {
                //I2S Slave Tdm mode
                hal_audio_agent_t agent;
                if (sink->param.audio.audio_interface == HAL_AUDIO_INTERFACE_2) {
                    agent = HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE;
                } else if (sink->param.audio.audio_interface == HAL_AUDIO_INTERFACE_3) {
                    agent = HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE;
                }
                hal_memory_free_sram(agent);
#endif
            } else if (sink->type == SINK_TYPE_AUDIO_DL3) {
                hal_memory_free_sram(HAL_AUDIO_AGENT_MEMORY_DL3);
            } else if (sink->type == SINK_TYPE_AUDIO_DL12) {
                hal_memory_free_sram(HAL_AUDIO_AGENT_MEMORY_DL12);
            } else {
                #if 1
                hal_memory_free_sram(HAL_AUDIO_AGENT_MEMORY_DL1);
                #else
                afe_free_audio_sram(AFE_SINK);
                #endif
            }
        }
    }
    Sink_Audio_BufferInfo_Rst(sink, 0);
}

#ifdef AIR_BT_CODEC_BLE_ENABLED
extern bool g_n9_ble_dl_open_flag;
extern bool g_n9_ble_dl_gaming_flag;
#endif
#ifdef MTK_BT_HFP_ENABLE
extern bool g_esco_dl_open_flag;
#endif
VOID Sink_Audio_BufferInfo_Rst(SINK sink, U32 offset)
{
    U32 TargetQnum;
    uint32_t afe_prefill;

    TargetQnum = (sink->type == SINK_TYPE_VP_AUDIO) ? (U32)Audio_setting->Audio_VP.Target_Q_Frame_Num
                                                    : (U32)Audio_setting->Audio_sink.Target_Q_Frame_Num;

    if (sink->buftype == BUFFER_TYPE_CIRCULAR_BUFFER) {
        sink->streamBuffer.BufferInfo.ReadOffset = (offset)%sink->streamBuffer.BufferInfo.length;
        sink->streamBuffer.BufferInfo.WriteOffset = (offset + sink->param.audio.frame_size*TargetQnum)%sink->streamBuffer.BufferInfo.length;
    } else if (sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER) {
        afe_prefill = (sink->type == SINK_TYPE_VP_AUDIO)
                        ? (sink->param.audio.AfeBlkControl.u4asrcflag)
                            ? ((sink->param.audio.period*sink->param.audio.format_bytes*sink->param.audio.rate*sink->param.audio.channel_num/1000)+ 32)
                            : 0x1000//(sink->param.audio.frame_size+32)*sink->param.audio.channel_num
                        : (sink->param.audio.AfeBlkControl.u4asrcflag)
                            ? (sink->param.audio.src_rate == 16000) ? ((sink->param.audio.src_rate*sink->param.audio.format_bytes*sink->param.audio.channel_num*sink->param.audio.period)/1000+64): 0x10//(uint32_t)(sink->param.audio.frame_size*2)
                            : (sink->param.audio.rate == 16000) ? (sink->param.audio.rate*sink->param.audio.format_bytes*sink->param.audio.channel_num*sink->param.audio.period)/1000 : 0x10;

#ifdef AIR_BT_CODEC_BLE_ENABLED
        if (g_n9_ble_dl_open_flag == true) {
            /* Prefill 10ms data to wait first stream handler done since maximum BLE frame interval = 10ms, for gaming mode, only prefill 5ms*/
            U16 prefill_ms = (g_n9_ble_dl_gaming_flag == true) ? 5 : (sink->param.audio.period == 10) ? 10 : 15;
            if (sink->param.audio.AfeBlkControl.u4asrcflag) {
                afe_prefill = prefill_ms * ((sink->param.audio.src_rate * sink->param.audio.format_bytes * sink->param.audio.channel_num) / 1000);
            } else {
                afe_prefill = prefill_ms * ((sink->param.audio.rate * sink->param.audio.format_bytes * sink->param.audio.channel_num) / 1000);
            }
            afe_prefill += 64; // Add 64 bytes for clock skew
            DSP_MW_LOG_I("[BLE] afe initial prefill size = %d", 1, afe_prefill);
        }
#endif
        if (sink->param.audio.linein_scenario_flag == 1) {
            afe_prefill = (sink->param.audio.count*sink->param.audio.channel_num*sink->param.audio.format_bytes*2)-32;// 2T
        }
#ifdef MTK_BT_HFP_ENABLE
        if (g_esco_dl_open_flag) {
            /* Prefill 20ms data to wait first stream handler done */
            if (sink->param.audio.AfeBlkControl.u4asrcflag) {
                afe_prefill = (sink->param.audio.src_rate * sink->param.audio.format_bytes * sink->param.audio.channel_num * (sink->param.audio.period + 5)) / 1000;
            } else {
                afe_prefill = (sink->param.audio.rate * sink->param.audio.format_bytes * sink->param.audio.channel_num * (sink->param.audio.period + 5)) / 1000;
            }
            DSP_MW_LOG_I("[eSCO] prefill size = %d", 1, afe_prefill);
        }
#endif
        DSP_MW_LOG_I("sink afe buffer prefill size: %d (samples),ch:%d", 2, (afe_prefill/sink->param.audio.format_bytes),sink->param.audio.channel_num);
        sink->streamBuffer.BufferInfo.ReadOffset = (offset)%sink->streamBuffer.BufferInfo.length;
        sink->streamBuffer.BufferInfo.WriteOffset = (offset + afe_prefill)%sink->streamBuffer.BufferInfo.length;
    } else {
        sink->streamBuffer.BufferInfo.ReadOffset  = 0;
        sink->streamBuffer.BufferInfo.WriteOffset = 0;
    }
    sink->param.audio.sram_empty_fill_size = 0;
    sink->streamBuffer.BufferInfo.bBufferIsFull = FALSE;
    //afe_sink_prefill_silence_data(sink);
}

VOID SinkBufferUpdateReadPtr(SINK sink, int32_t size)
{
    TRANSFORM transform = sink->transform;
    uint32_t pre_offset;
    BUFFER_INFO *pBufInfo = (BUFFER_INFO *) &sink->streamBuffer.BufferInfo;

    if (size != 0) {
        pBufInfo->bBufferIsFull = FALSE;
    }

    pre_offset = pBufInfo->ReadOffset;

    if (pBufInfo->length == 0) {
        configASSERT(0);
    }

    pBufInfo->ReadOffset = (pBufInfo->ReadOffset + size)%(pBufInfo->length);
//    if (Audio_setting->Audio_sink.Mute_Flag) {      // duncan, test skip the abnormal situation

#if 0   // temp. off
    if ( pAfeBlock->BufferAbnormal || (Audio_setting->Audio_sink.Mute_Flag)) {
        memset(pBufInfo->startaddr[0], 0, pBufInfo->length);
        if(sink->param.audio.channel_num > 1)
        {
             memset(pBufInfo->startaddr[1], 0, pBufInfo->length);
        }
    } else {
        memset(pBufInfo->startaddr[0] + pre_offset, 0, size);
        if(sink->param.audio.channel_num > 1)
        {
             memset(pBufInfo->startaddr[1] + pre_offset,0, size);
        }
    }
#endif

    if (transform !=NULL) {
        // printf("Audio_handler wakeup from sink buffer++\r\n");
        Audio_handler = (transform->Handler);   // Stream_Audio_Handler
        if(Audio_handler == NULL)
            DSP_MW_LOG_I("Audio_handler = NULL\r\n", 0);
        else
            Audio_handler(transform->source,transform->sink);

        // printf("Audio_handler wakeup from sink buffer--\r\n");
        xTaskResumeFromISR(pDAV_TaskHandler);
        portYIELD_FROM_ISR(pdTRUE);            // force to do context switch
    }
}

VOID SourceBufferUpdateWritePtr(SOURCE source, int32_t size)
{
    TRANSFORM transform = source->transform;
    BUFFER_INFO *pBufInfo = &source->streamBuffer.BufferInfo;
    uint32_t pre_offset;

    pre_offset = pBufInfo->WriteOffset;
    pBufInfo->WriteOffset = (pBufInfo->WriteOffset + size)%(pBufInfo->length);

    //if (pAfeBlock->BufferAbnormal) {
    if (OFFSET_OVERFLOW_CHK(pre_offset, pBufInfo->WriteOffset, pBufInfo->ReadOffset)) {
        if ((pre_offset != pBufInfo->ReadOffset)||(pBufInfo->bBufferIsFull == TRUE))
        {
           pBufInfo->bBufferIsFull = TRUE;
           //pBufInfo->ReadOffset = (pBufInfo->ReadOffset + frame_size)%(pBufInfo->length);
           pBufInfo->ReadOffset = pBufInfo->WriteOffset;
        }
    }

    if (transform !=NULL) {
        Audio_handler = (transform->Handler);           // Stream_Audio_Handler
        Audio_handler(transform->source,transform->sink);
        xTaskResumeFromISR(pDAV_TaskHandler);
        portYIELD_FROM_ISR(pdTRUE);            // force to do context switch
    }
}

VOID Sink_Audio_UpdateReadOffset(SINK_TYPE type)
{

    TRANSFORM transform = Sink_blks[type]->transform;
    BUFFER_INFO_PTR buf_inf = &Sink_blks[type]->streamBuffer.BufferInfo;
    U16 frame_size = Sink_blks[type]->param.audio.frame_size;
    U32 NEXTADDR;
    U32 pre_offset;

    #if (FEA_SUPP_DSP_SRC_VDM)
    BOOL with_vdm_src = ((Sink_blks[type]->param.audio.src != NULL) &&
                         (Sink_blks[type]->param.audio.src->src_ptr != NULL) &&
                         (type==SINK_TYPE_AUDIO || type==SINK_TYPE_VP_AUDIO));
    if (with_vdm_src)
    {
        NEXTADDR = (U32)DSP_GetSRCIn1NextPtr(Sink_blks[type]->param.audio.src->src_ptr);
    }
    else
    #endif
    if (type == SINK_TYPE_VP_AUDIO)
    {
        NEXTADDR = (U32)DSP_GetAudioOutNextPtr(ADMA_CH2);
    }
    else if (Sink_blks[type]->param.audio.channel_sel == AUDIO_CHANNEL_B)
    {
        NEXTADDR = (Audio_setting->Audio_sink.SRC_Out_Enable)
                 ? (U32)DSP_GetAudioOutNextPtr(SRCA_CH1)
                 : (U32)DSP_GetAudioOutNextPtr(ADMA_CH1);
    }
    else
    {
        NEXTADDR = (Audio_setting->Audio_sink.SRC_Out_Enable)
                 ? (U32)DSP_GetAudioOutNextPtr(SRCA_CH0)
                 : (U32)DSP_GetAudioOutNextPtr(ADMA_CH0);
    }
    buf_inf->bBufferIsFull = FALSE;

    pre_offset = buf_inf->ReadOffset;
    // buf_inf->ReadOffset = (U32)frame_size*((NEXTADDR - (U32)buf_inf->startaddr[0])/((U32)frame_size));
    buf_inf->ReadOffset = (pre_offset + frame_size)%(buf_inf->length);

    #if 0
    if ((buf_inf->WriteOffset == buf_inf->ReadOffset)||(Audio_setting->Audio_sink.Mute_Flag))//Speed up mute processure
    #else
    if (OFFSET_OVERFLOW_CHK(pre_offset, buf_inf->ReadOffset, buf_inf->WriteOffset) || (Audio_setting->Audio_sink.Mute_Flag))
    #endif
    {
        memset(buf_inf->startaddr[0], 0,buf_inf->length);
        if (Audio_setting->Audio_sink.Zero_Padding_Cnt>0)
        {
            Audio_setting->Audio_sink.Zero_Padding_Cnt--;
            if ((Audio_setting->Audio_sink.Zero_Padding_Cnt == 0)&&(Sink_blks[type]->param.audio.handler != NULL))
            {
                //MSG_MessageSend(Sink_blks[type]->param.audio.handler, MESSAGE_SINK_BUF_EMPTY, 0);
                //#warning "No message system in 155x"
            }
        }
        if(Sink_blks[type]->param.audio.channel_num>1)
        {
             memset(buf_inf->startaddr[1], 0,buf_inf->length);
        }
        buf_inf->WriteOffset = (buf_inf->WriteOffset + frame_size)%(buf_inf->length);

        #if (FEA_SUPP_DSP_SRC_VDM)
        if (with_vdm_src)
        {
            buf_inf->WriteOffset = (buf_inf->WriteOffset + frame_size)%(buf_inf->length);
        }
        #endif
    }

    if (transform !=NULL)
    {
        /* Update Source Write offset*/
        #if (FEA_SUPP_DSP_SRC_VDM)
        if ((transform->source->param.audio.src != NULL) &&
            ((transform->source->type == SOURCE_TYPE_AUDIO)||(transform->source->type == SOURCE_TYPE_AUDIO2)))
        {
            transform->source->streamBuffer.BufferInfo.WriteOffset = DSP_GetSRCOutWriteOffset(transform->source->param.audio.src->src_ptr);
        }
        #endif

        //streaming_ptr = DSP_Streaming_Get(transform->source,transform->sink);
        //if (streaming_ptr != NULL)
            //streaming_ptr->driftCtrl.SinkLatch(&(streaming_ptr->driftCtrl.para));



        #if (FEA_SUPP_DSP_SRC_VDM)
        if (with_vdm_src)
            if ((transform->sink->param.audio.src->enable_flag) &&
                (transform->source->param.audio.src == NULL))
                return;
        if (transform->source->type == SOURCE_TYPE_AUDIO || transform->source->type==SOURCE_TYPE_DSP_BRANCH || transform->source->type == SOURCE_TYPE_AUDIO2)
            if (transform->source->param.audio.src != NULL)
                if (!transform->source->param.audio.src->enable_flag)
                    return;
        #endif
        Audio_handler = (transform->Handler);
        Audio_handler(transform->source,transform->sink);
    }
}

VOID Sink_Audio_SubPath_Init(SINK sink)
{
    sink->type   = SINK_TYPE_VP_AUDIO;
    sink->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    Sink_Audio_Buffer_Ctrl(sink,TRUE);

    //OS_LV3_INTR_RegisterHandler(OS_LV3_INTR_ID_ODFE_VP, Sink_Audio_VpRtIsrHandler, sink->taskid);
    INTR_RegisterHandler(INTR_ID_ODFE_VP,Sink_Audio_VpRtIsrHandler, sink->taskid);

    #if 0 // Yo
    /* interface init */
    sink->sif.SinkSlack         = SinkSizeAudio;
    sink->sif.SinkFlush         = SinkFlushAudio;
    sink->sif.SinkConfigure     = SinkConfigureAudio;
    sink->sif.SinkClose         = SinkCloseAudio;
    sink->sif.SinkClaim         = SinkClaimAudio;
    sink->sif.SinkMap           = SinkMapAudio;
    sink->sif.SinkWriteBuf      = SinkWriteBufAudio;
    #endif

    //OS_INTR_RegisterHdlr(OS_INTR_ID_LEVEL3, OS_LV3_INTR_Hdlr);

    DSP_CTRL_ChangeStatus(AU_OUTPUT_DSP_SUBPATH, AU_DSP_STATUS_ON);
}


BOOL Sink_Audio_ZeroPadding()
{
    while (Audio_setting->Audio_sink.Zero_Padding_Cnt>0)
    {
        portYIELD();
    }
    return TRUE;
}

BOOL Sink_Audio_ClosureSmooth(SINK sink)
{
    if (sink->type == SINK_TYPE_AUDIO) {
        if (sink->param.audio.hw_gain) {
            //Set mute for ramp down
            if (sink->param.audio.memory&HAL_AUDIO_MEM1) {
                afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN1, 0);
            }
            if (sink->param.audio.memory&HAL_AUDIO_MEM2) {
                afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN2, 0);
            }
            DSP_MW_LOG_I("DSP audio pcm_dl_stop wait gain ramp down \r\n", 0);
        } else {
            //Clearup memory
            memset((U8*)sink->param.audio.AfeBlkControl.phys_buffer_addr, 0, sink->streamBuffer.BufferInfo.length);
        }
    }
    return TRUE;
}

VOID Sink_Audio_SRC_Ctrl(SINK sink, BOOL isEnabled, stream_samplerate_t value)
{
    #if (FEA_SUPP_DSP_SRC_VDM)
    U8 * mem_ptr;
    U32 mem_size;
    U32 inFrameSize;
    DSP_DRV_SRC_VDM_INIT_STRU src_setting;
    STREAM_SRC_VDM_PTR_t VOLATILE src_vdm;
    stream_samplerate_t sinkFs;
    BOOL reset_HW = FALSE;
    if (isEnabled)
    {
        if (((sink->type == SINK_TYPE_AUDIO) && (Audio_setting->Audio_sink.Output_Enable==TRUE)) ||
            ((sink->type == SINK_TYPE_VP_AUDIO) && (Audio_setting->Audio_VP.Output_Enable==TRUE)))
        {
            sink->streamBuffer.BufferInfo.bBufferIsFull = TRUE;
            AudioSinkHW_Ctrl(sink, FALSE);
            reset_HW = TRUE;
        }
        if (sink->param.audio.src == NULL)
        {
            mem_size = sink->param.audio.channel_num * sink->streamBuffer.BufferInfo.length + sizeof(STREAM_SRC_VDM_t);
            mem_ptr = DSPMEM_tmalloc(DSP_TASK_ID, mem_size, sink+DSP_MARK_SRC_PTR);
            memset(mem_ptr, 0, mem_size);
            src_vdm = (STREAM_SRC_VDM_PTR_t)((U32)mem_ptr + sink->param.audio.channel_num*sink->streamBuffer.BufferInfo.length);
            src_vdm->mem_ptr = sink->streamBuffer.BufferInfo.startaddr[0];
            sink->streamBuffer.BufferInfo.startaddr[0]   = mem_ptr;
            sink->streamBuffer.BufferInfo.startaddr[1]   = (U8*)((U32)mem_ptr + sink->streamBuffer.BufferInfo.length);
            src_vdm->original_frame_size  = sink->param.audio.frame_size;
        }
        else
        {
            src_vdm = sink->param.audio.src;
            sink->param.audio.frame_size = src_vdm->original_frame_size;
            memset(src_vdm->mem_ptr, 0, sink->param.audio.channel_num * sink->streamBuffer.BufferInfo.length);
            src_vdm->enable_flag = FALSE;
            src_vdm->accum_process_size = 0;
        }
        sinkFs = (sink->type == SINK_TYPE_VP_AUDIO)
                   ?AudioVpSinkSamplingRate_Get()
                   :AudioSinkSamplingRate_Get();


        inFrameSize = sink->param.audio.frame_size*DSP_FsChange2Value(value)/DSP_FsChange2Value(sinkFs);
        #if 1
        inFrameSize = (sink->transform != NULL)
                        ? (sink->transform->source->type==SOURCE_TYPE_AUDIO)
                            ? sink->transform->source->param.audio.frame_size
                            : (sink->transform->source->type==SOURCE_TYPE_DSP_BRANCH)
                                ? sink->transform->source->param.dsp.frame_size
                                : (sink->transform->source->type==SOURCE_TYPE_USBAUDIOCLASS)
                                    ? sink->transform->source->param.USB.frame_size
                                    : inFrameSize
                        : inFrameSize;
        #endif

        src_setting.src_ptr        = src_vdm->src_ptr;
        src_setting.mode           = SRC_IVDM;
        src_setting.radma_buf_addr = (U8*)(sink->streamBuffer.BufferInfo.startaddr[0]);
        src_setting.radma_buf_size = sink->streamBuffer.BufferInfo.length;
        src_setting.radma_THD      = inFrameSize;
        src_setting.wadma_buf_addr = (U8*)(src_vdm->mem_ptr);
        src_setting.wadma_buf_size = sink->streamBuffer.BufferInfo.length;
        src_setting.wadma_THD      = inFrameSize*DSP_FsChange2Value(sinkFs)/DSP_FsChange2Value(value);
        src_setting.fs_in          = DSP_FsChange2SRCInRate(value);
        src_setting.fs_out         = DSP_FsChange2SRCOutRate(sinkFs);
        src_setting.channel_num    = sink->param.audio.channel_num;
        src_setting.Res_In         = Audio_setting->resolution.SRCInRes;
        src_setting.Res_Out        = Audio_setting->resolution.AudioOutRes;

        src_vdm->src_ptr = DSP_DRV_SRC_VDM_INIT(&src_setting);
        sink->param.audio.frame_size = inFrameSize;
        DSP_DRV_SINK_SRC_VDM_PreTrigger(sink, src_vdm);

        if (reset_HW==TRUE)
        {
            AudioSinkHW_Ctrl(sink, TRUE);
        }

        if (sink->param.audio.src == NULL)
        {
            sink->param.audio.src = src_vdm;
        }

    }
    else
    {
        if (sink->param.audio.src != NULL)
        {
            mem_ptr = sink->streamBuffer.BufferInfo.startaddr[0];
            sink->streamBuffer.BufferInfo.startaddr[0]   = sink->param.audio.src->mem_ptr;
            sink->streamBuffer.BufferInfo.startaddr[1]   = (U8*)((U32)sink->param.audio.src->mem_ptr + sink->streamBuffer.BufferInfo.length);
            sink->param.audio.frame_size = src_vdm->original_frame_size;
            src_vdm = sink->param.audio.src;
            sink->param.audio.src = NULL;
            DSP_DRV_SRC_END(src_vdm->src_ptr);
            DSPMEM_Free(DSP_TASK_ID, sink+DSP_MARK_SRC_PTR);
        }
    }
    #else
    UNUSED(sink);
    UNUSED(isEnabled);
    UNUSED(value);
    #endif
}



VOID Sink_Audio_Triger_SourceSRC(SOURCE source)
{
    #if (FEA_SUPP_DSP_SRC_VDM)
    STREAM_SRC_VDM_PTR_t src = source->param.audio.src;
    if (src != NULL)
    {
        while(DSP_GetSRCStatus(src->src_ptr));
        if (!((U32)(DSP_GetAudioInNextPtr(ADMA_CH0) - DSP_GetSRCIn1NextPtr(src->src_ptr)) < DSP_GetSRCInFrameSize(src->src_ptr)) ||
            (source->type != SOURCE_TYPE_AUDIO) || (source->type != SOURCE_TYPE_AUDIO2))
        {
            DSP_SetSRCTrigger(src->src_ptr);
        }

        src->enable_flag = TRUE;
    }
    #else
    UNUSED(source);
    #endif
}


VOID Sink_Audio_SRC_A_CDM_SamplingRate_Reset(SINK sink, stream_samplerate_t rate)
{
    #if 0
    DSP_DA_OUT_END();

    DSP_DRV_oDFE_INT4_END();
    DSP_DRV_SRC_A_END();

    DSP_DRV_SRC_A_INIT(DSP_FsChange2SRCInRate(AudioSRCSamplingRate_Set(rate)),
                       DSP_FsChange2SRCOutRate(AudioSinkSamplingRate_Get()),
                       Audio_setting->resolution.SRCInRes,
                       Audio_setting->resolution.AudioOutRes);

    DSP_DRV_oDFE_INT4_RST(stream->sink->param.audio.channel_num);
    DSP_DRV_oDFE_INT4_EN(stream->sink->param.audio.channel_num);

    DSP_DA_OUT_RST(stream->sink->param.audio.channel_num);
    DSP_DA_OUT_EN(stream->sink->param.audio.channel_num);
    #else
    AudioSinkHW_Ctrl(sink, FALSE);
    AudioSRCSamplingRate_Set(rate);
    Sink_Audio_BufferInfo_Rst(sink, 0);
    AudioSinkHW_Ctrl(sink, TRUE);

    #endif
}


VOID Sink_Audio_ADMAIsrHandler(VOID)
{
    Sink_Audio_UpdateReadOffset(SINK_TYPE_AUDIO);
    DSP_DRV_ClearReadAdmaIsrStatus();
}


VOID Sink_Audio_SRCIsrHandler(VOID)
{
    Sink_Audio_UpdateReadOffset(SINK_TYPE_AUDIO);
    DSP_DRV_ClearSrcIsrStatus();
}


VOID Sink_Audio_VpRtIsrHandler(VOID)
{
    Sink_Audio_UpdateReadOffset(SINK_TYPE_VP_AUDIO);
    DSP_DRV_ClearVpIsrStatus();
}


VOID Source_Audio_Pattern_Init(SOURCE source)
{
    source->streamBuffer.BufferInfo.ReadOffset            = 0;
    source->streamBuffer.BufferInfo.WriteOffset           = 0;
    source->streamBuffer.BufferInfo.bBufferIsFull         = FALSE;

    #if 0 // Yo
    /* interface init */

    source->sif.SourceConfigure    = NULL;
    source->sif.SourceClose        = SourceClose_AudioPattern;

    if (source->param.VPRT.mode != RT_mode)
    {
        source->sif.SourceSize         = SourceSize_AudioPattern_VP;
        source->sif.SourceDrop         = SourceDrop_AudioPattern_VP;
        source->sif.SourceMap          = SourceMap_AudioPattern_VP;
        source->sif.SourceReadBuf      = SourceReadBuf_AudioPattern_VP;
    }
    else
    {
        source->sif.SourceSize         = SourceSize_AudioPattern_RT;
        source->sif.SourceDrop         = SourceDrop_AudioPattern_RT;
        source->sif.SourceMap          = NULL;
        source->sif.SourceReadBuf      = SourceReadBuf_AudioPattern_RT;
        #ifdef TestReadPattern
        fFlashDrv->ByteRead((FLASH_ADDR)source->streamBuffer.BufferInfo.startaddr[0], source->streamBuffer.BufferInfo.startaddr[1], 4);// Load first tone ptr
        fFlashDrv->ByteRead((FLASH_ADDR)source->streamBuffer.BufferInfo.startaddr[1], &source->param.VPRT.para.RTInfo, 6);// load first Tone
        source->streamBuffer.BufferInfo.startaddr[0] += 6; // 2 byte tone length + 4 byte  first tone ptr
        #else
        memcpy(&source->param.VPRT.para.RTInfo,source->streamBuffer.BufferInfo.startaddr[0],6);
        source->streamBuffer.BufferInfo.startaddr[0] += 6; // 2 byte tone length + 4 byte  first tone ptr
        #endif
    }
    #else

    if (source->param.VPRT.mode == RT_mode)
    {
        #ifdef TestReadPattern
        fFlashDrv->ByteRead((FLASH_ADDR)source->streamBuffer.BufferInfo.startaddr[0], source->streamBuffer.BufferInfo.startaddr[1], 4);// Load first tone ptr
        fFlashDrv->ByteRead((FLASH_ADDR)source->streamBuffer.BufferInfo.startaddr[1], &source->param.VPRT.para.RTInfo, 6);// load first Tone
        source->streamBuffer.BufferInfo.startaddr[0] += 6; // 2 byte tone length + 4 byte  first tone ptr
        #else
        memcpy(&source->param.VPRT.para.RTInfo,source->streamBuffer.BufferInfo.startaddr[0],6);
        source->streamBuffer.BufferInfo.startaddr[0] += 6; // 2 byte tone length + 4 byte  first tone ptr
        #endif
    }

    #endif
}


VOID Source_Audio_Path_Init(SOURCE source)
{
    Source_Audio_Buffer_Ctrl(source,TRUE);

   // Audio_setting->Audio_source.Input_Enable = FALSE;

    if (audio_ops_open(source))
        DSP_MW_LOG_I("audio source type : %d open error\r\n", 1, source->type);
    /* Intr Enable */
    //OS_INTR_RegisterHdlr(OS_INTR_ID_LEVEL3, OS_LV3_INTR_Hdlr);

    DSP_CTRL_ChangeStatus(AU_INPUT_DSP, AU_DSP_STATUS_ON);

}

VOID Source_Audio_SRC_Ctrl(SOURCE source, BOOL ctrl, stream_samplerate_t value)
{
    #if (FEA_SUPP_DSP_SRC_VDM)

    U8 * mem_ptr;
    U32 mem_size;
    U32 outFrameSize;
    DSP_DRV_SRC_VDM_INIT_STRU src_setting;
    STREAM_SRC_VDM_PTR_t VOLATILE src_vdm;
    BOOL reset_HW = FALSE;
    if (ctrl)
    {
        if ((source->type == SOURCE_TYPE_AUDIO)||(source->type == SOURCE_TYPE_AUDIO2))
        {
            source->streamBuffer.BufferInfo.WriteOffset = source->streamBuffer.BufferInfo.ReadOffset;
            AudioSourceHW_Ctrl(source, FALSE);
            reset_HW = TRUE;
        }

        if (source->param.audio.src == NULL)
        {
            mem_size = source->param.audio.channel_num * source->streamBuffer.BufferInfo.length + sizeof(STREAM_SRC_VDM_t);
            mem_ptr = DSPMEM_tmalloc(DSP_TASK_ID, mem_size, source+DSP_MARK_SRC_PTR);
            memset(mem_ptr, 0, mem_size);
            src_vdm = (STREAM_SRC_VDM_PTR_t)((U32)mem_ptr + source->param.audio.channel_num*source->streamBuffer.BufferInfo.length);
            src_vdm->mem_ptr = source->streamBuffer.BufferInfo.startaddr[0];
            source->streamBuffer.BufferInfo.startaddr[0]   = mem_ptr;
            source->streamBuffer.BufferInfo.startaddr[1]   = (U8*)((U32)mem_ptr + source->streamBuffer.BufferInfo.length);
            src_vdm->original_frame_size = source->param.audio.frame_size;
        }
        else
        {
            src_vdm = source->param.audio.src;
            source->param.audio.frame_size = src_vdm->original_frame_size;
            memset(src_vdm->mem_ptr, 0, source->param.audio.channel_num*source->streamBuffer.BufferInfo.length);
            src_vdm->enable_flag = FALSE;
            src_vdm->accum_process_size = 0;
        }

        outFrameSize = source->param.audio.frame_size*DSP_FsChange2Value(value)/DSP_FsChange2Value(AudioSourceSamplingRate_Get());
        #if 1
        outFrameSize = (source->transform != NULL)
                         ? (source->transform->sink->type==SINK_TYPE_AUDIO)
                             ? source->transform->sink->param.audio.frame_size
                             : (source->transform->sink->type==SINK_TYPE_DSP_JOINT)
                                 ? source->transform->sink->param.dsp.frame_size
                                 : (source->transform->sink->type == SINK_TYPE_USBAUDIOCLASS)
                                     ? source->transform->sink->param.USB.frame_size
                                     : outFrameSize
                         : outFrameSize;

        #endif
        src_setting.src_ptr        = src_vdm->src_ptr;
        src_setting.mode           = SRC_OVDM;
        src_setting.radma_buf_addr = (U8*)(src_vdm->mem_ptr);
        src_setting.radma_buf_size = source->streamBuffer.BufferInfo.length;
        src_setting.radma_THD      = outFrameSize*DSP_FsChange2Value(AudioSourceSamplingRate_Get())/DSP_FsChange2Value(value);
        src_setting.wadma_buf_addr = (U8*)(source->streamBuffer.BufferInfo.startaddr[0]);
        src_setting.wadma_buf_size = source->streamBuffer.BufferInfo.length;
        src_setting.wadma_THD      = outFrameSize;
        src_setting.fs_in          = DSP_FsChange2SRCInRate(AudioSourceSamplingRate_Get());
        src_setting.fs_out         = DSP_FsChange2SRCOutRate(value);
        src_setting.channel_num    = source->param.audio.channel_num;
        src_setting.Res_In         = Audio_setting->resolution.AudioInRes;
        src_setting.Res_Out        = (Audio_setting->Audio_sink.SRC_Out_Enable)
                                        ?Audio_setting->resolution.SRCInRes
                                        :Audio_setting->resolution.AudioOutRes;
        src_vdm->src_ptr = DSP_DRV_SRC_VDM_INIT(&src_setting);
        source->param.audio.frame_size = outFrameSize;
        DSP_DRV_SOURCE_SRC_VDM_PreTrigger(source, src_vdm);

        if (reset_HW==TRUE)
        {
            AudioSourceHW_Ctrl(source, TRUE);
        }


        if (source->param.audio.src == NULL)
        {
            source->param.audio.src = src_vdm;
        }

    }
    else
    {
        if (source->param.audio.src != NULL)
        {
            mem_ptr = source->streamBuffer.BufferInfo.startaddr[0];
            source->streamBuffer.BufferInfo.startaddr[0]   = source->param.audio.src->mem_ptr;
            source->streamBuffer.BufferInfo.startaddr[1]   = (U8*)((U32)source->param.audio.src->mem_ptr + source->streamBuffer.BufferInfo.length);
            source->param.audio.frame_size = src_vdm->original_frame_size;
            src_vdm = source->param.audio.src;
            source->param.audio.src = NULL;
            DSP_DRV_SRC_END(src_vdm->src_ptr);
            DSPMEM_Free(DSP_TASK_ID, source+DSP_MARK_SRC_PTR);
        }
    }
    #else
    UNUSED(source);
    UNUSED(ctrl);
    UNUSED(value);
    #endif
}


VOID Source_Audio_Buffer_Ctrl(SOURCE source, BOOL ctrl)
{
    U16 i;
    U8 * mem_ptr;
    U32 mem_size;

    if (audio_ops_distinguish_audio_source(source))
    {
        source->param.audio.frame_size =  Audio_setting->Audio_source.Frame_Size*source->param.audio.format_bytes;

    }

    if (ctrl) {
        if (source->buftype == BUFFER_TYPE_CIRCULAR_BUFFER) {
            source->streamBuffer.BufferInfo.length = source->param.audio.frame_size*Audio_setting->Audio_source.Buffer_Frame_Num;
            if (source->param.audio.echo_reference == true) {
                mem_size = 3 * source->streamBuffer.BufferInfo.length;
            }
            else {
                mem_size = source->param.audio.channel_num * source->streamBuffer.BufferInfo.length;
            }
            mem_ptr = DSPMEM_tmalloc(DSP_TASK_ID, mem_size, source);
            memset(mem_ptr, 0, mem_size);

            if (source->param.audio.echo_reference == true) {
                source->streamBuffer.BufferInfo.startaddr[2] = mem_ptr + source->streamBuffer.BufferInfo.length * 2;
            }

            for (i = 0; i<source->param.audio.channel_num ; i++ )
            {
                source->streamBuffer.BufferInfo.startaddr[i] = mem_ptr;
                mem_ptr = (U8*)((U32)mem_ptr + source->streamBuffer.BufferInfo.length);
            }
        } else if (source->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER) {
            source->streamBuffer.BufferInfo.length = source->param.audio.buffer_size;
            return;//allocate at ul open

            int32_t afe_number;
            audio_digital_block_t memory_block;

            memory_block =  hal_audio_afe_get_memory_digital_block (source->param.audio.memory, false);


            afe_number = ((source->param.audio.channel_num+1)>>1);
            if (source->param.audio.echo_reference) {
                afe_number++;
            }
            if (afe_number>0) {
                mem_size = source->param.audio.buffer_size*afe_number;
                source->streamBuffer.BufferInfo.length = source->param.audio.buffer_size;

                if (source->param.audio.AfeBlkControl.u4asrcflag) {
                    mem_size += source->param.audio.AfeBlkControl.u4asrc_buffer_size;
                }
                //modify for ab1568
                #if 1
                source->param.audio.AfeBlkControl.phys_buffer_addr = hal_memory_allocate_sram(HAL_AUDIO_AGENT_MEMORY_VUL1, mem_size);
                #else
                if (afe_allocate_audio_sram(&source->param.audio.AfeBlkControl, source->param.audio.format, mem_size, false)) {
                    DSP_MW_LOG_I("DSP sram allocate fail source type:%d, size:%d\r\n", 2, source->type, mem_size);
                    return;
                }
                #endif

                DSP_MW_LOG_I("audio source buffer addr: 0x%x, source_type:%d\n", 2, source->param.audio.AfeBlkControl.phys_buffer_addr, source->type);

                mem_ptr = (U8*)source->param.audio.AfeBlkControl.phys_buffer_addr;
                memset(mem_ptr, 0, mem_size);

                if (source->param.audio.AfeBlkControl.u4asrcflag) {
                    source->streamBuffer.BufferInfo.startaddr[afe_number%BUFFER_INFO_CH_NUM] = mem_ptr;
                    mem_ptr += source->param.audio.AfeBlkControl.u4asrc_buffer_size;
                }
                for (i = 0; ((i<afe_number) && (i<BUFFER_INFO_CH_NUM)) ; i++ ) {
                    if (!source->streamBuffer.BufferInfo.startaddr[i]) {
                        source->streamBuffer.BufferInfo.startaddr[i] = mem_ptr;
                        mem_ptr += source->streamBuffer.BufferInfo.length;
                        DSP_MW_LOG_I("afe_number=%d,startaddr[%d]=0x%x",3,afe_number,i,source->streamBuffer.BufferInfo.startaddr[i]);
                    }
                }
            }
#ifdef ENABLE_2A2D_TEST
            //1A1D use VUL1
            if(source->param.audio.audio_device1 != HAL_AUDIO_CONTROL_NONE) {
                memory_block = AUDIO_DIGITAL_BLOCK_MEM_VUL1;
            }
#endif
            #if 0 //modify for ab1568 to slim code
            afe_set_mem_block(AFE_SOURCE, memory_block);
            if (source->param.audio.echo_reference) {
                afe_block_t awb_block;
                awb_block.phys_buffer_addr = (U32)source->streamBuffer.BufferInfo.startaddr[i-1];
                awb_block.u4BufferSize = source->streamBuffer.BufferInfo.length;
                afe_set_mem_block_addr(AUDIO_DIGITAL_BLOCK_MEM_AWB, &awb_block);
            }
            #endif
            #if 0//modify for ab1568
            if(source->param.audio.channel_num >= 3){
                afe_block_t vul2_block;
                if(source->param.audio.echo_reference){
                    vul2_block.phys_buffer_addr = (U32)source->streamBuffer.BufferInfo.startaddr[i-2];
                }else{
                    vul2_block.phys_buffer_addr = (U32)source->streamBuffer.BufferInfo.startaddr[i-1];
                }
                vul2_block.u4asrcflag = 0;
                vul2_block.u4BufferSize = source->streamBuffer.BufferInfo.length;
                afe_set_mem_block_addr(AUDIO_DIGITAL_BLOCK_MEM_VUL2, &vul2_block);
            }
            #endif
        }
    } else {
        DSPMEM_Free(DSP_TASK_ID, source);

        if (source->buftype == BUFFER_TYPE_CIRCULAR_BUFFER) {
            DSPMEM_Free(DSP_TASK_ID, source);
        } else if (source->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER) {
            #if 1
            return;//free at ul close
            //hal_memory_free_sram(HAL_AUDIO_AGENT_MEMORY_VUL1);
            #else
            afe_free_audio_sram(AFE_SOURCE);
            #endif
        }
    }
    Source_Audio_BufferInfo_Rst(source, 0);
}

VOID Source_Audio_BufferInfo_Rst(SOURCE source, U32 offset)
{
    U32 preFillSize;
#if defined(MTK_GAMING_MODE_HEADSET)
    if((source->transform != NULL) && (source->transform->sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (source->transform->sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MAX)) {
        if(source->transform->sink->param.data_ul.is_customize == TRUE) {
            offset = 0;
        }
    }
#endif
    offset = offset&0xfffffff8;//8 bytes align
    if (source->buftype == BUFFER_TYPE_CIRCULAR_BUFFER) {
        #if 1   // more aggresive moving
            preFillSize = source->param.audio.frame_size*AUDIO_SOURCE_PREFILL_FRAME_NUM;
        #else
            preFillSize = source->param.audio.frame_size;
        #endif
        source->streamBuffer.BufferInfo.WriteOffset   = (offset)%source->streamBuffer.BufferInfo.length;
        source->streamBuffer.BufferInfo.ReadOffset    = (offset + preFillSize + 10)%source->streamBuffer.BufferInfo.length;
    } else if (source->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER) {
        preFillSize = 8;
        if (source->param.audio.linein_scenario_flag == 1) {
#ifdef MTK_TDM_ENABLE
            preFillSize = source->param.audio.count*((source->param.audio.channel_num>=2) ? 2 : 1)*source->param.audio.format_bytes;// 1T
#else
            preFillSize = source->param.audio.count*source->param.audio.channel_num*source->param.audio.format_bytes;// 1T
#endif
        }
        source->streamBuffer.BufferInfo.WriteOffset = (offset)%source->streamBuffer.BufferInfo.length;
        source->streamBuffer.BufferInfo.ReadOffset = (offset + source->streamBuffer.BufferInfo.length-preFillSize)%source->streamBuffer.BufferInfo.length;
        DSP_MW_LOG_I("AFE source prefill size:%d, wo:%d, ro:%d, len:%d, offset:%d", 5, preFillSize, source->streamBuffer.BufferInfo.WriteOffset, source->streamBuffer.BufferInfo.ReadOffset, source->streamBuffer.BufferInfo.length, offset);
    } else {
        source->streamBuffer.BufferInfo.ReadOffset  = 0;
        source->streamBuffer.BufferInfo.WriteOffset = 0;
    }

    source->streamBuffer.BufferInfo.bBufferIsFull = (source->streamBuffer.BufferInfo.ReadOffset == source->streamBuffer.BufferInfo.WriteOffset);

    // don't update the Rptr becoz the ISR trigger faster than read data from AFE SRAM
    //SourcePrefillSilenceData(source);
}



VOID Source_Audio_Triger_SinkSRC(SINK sink)
{
    #if (FEA_SUPP_DSP_SRC_VDM)
    if (sink->type!=SINK_TYPE_AUDIO &&
        sink->type!=SINK_TYPE_VP_AUDIO &&
        sink->type!=SINK_TYPE_DSP_JOINT)
        return;
    if (sink->param.audio.src == NULL)
        return;
    STREAM_SRC_VDM_PTR_t src = sink->param.audio.src;
    if ((src != NULL) && (src->src_ptr != NULL))
    {
        while(DSP_GetSRCStatus(src->src_ptr));
        if (!((U32)(DSP_GetAudioOutNextPtr(ADMA_CH0) - DSP_GetSRCOut1NextPtr(src->src_ptr)) < DSP_GetSRCOutFrameSize(src->src_ptr)) ||
            !src->enable_flag)
        {
            DSP_SetSRCTrigger(src->src_ptr);
        }
        src->enable_flag = TRUE;
    }
    #else
    UNUSED(sink);
    #endif
}


ATTR_TEXT_IN_IRAM_LEVEL_2 VOID Source_Audio_IsrHandler(VOID)
{
    TRANSFORM transform;
    Audiohandler Audio_handler;
    transform = Source_blks[SOURCE_TYPE_AUDIO]->transform;
    DSP_STREAMING_PARA_PTR streaming_ptr;


    BUFFER_INFO_PTR buf_inf = &Source_blks[SOURCE_TYPE_AUDIO]->streamBuffer.BufferInfo;
    U16 frame_size = Source_blks[SOURCE_TYPE_AUDIO]->param.audio.frame_size;
    U32 NEXTADDR;
    U32 pre_offset;

    #if (FEA_SUPP_DSP_SRC_VDM)
    if (Source_blks[SOURCE_TYPE_AUDIO]->param.audio.src != NULL)
    {
        NEXTADDR = (U32)DSP_GetSRCOut1NextPtr(Source_blks[SOURCE_TYPE_AUDIO]->param.audio.src->src_ptr);
    }
    else
    #endif
    {
        NEXTADDR = (Source_blks[SOURCE_TYPE_AUDIO]->param.audio.channel_sel == AUDIO_CHANNEL_B)
                     ? (U32)DSP_GetAudioInNextPtr(ADMA_CH1)
                     : (U32)DSP_GetAudioInNextPtr(ADMA_CH0);
    }
    pre_offset = buf_inf->WriteOffset;
    buf_inf->WriteOffset = (U32)frame_size*((NEXTADDR - (U32)buf_inf->startaddr[0])/((U32)frame_size));
    #if 0
    if (buf_inf->WriteOffset == buf_inf->ReadOffset)
    #else
    if (OFFSET_OVERFLOW_CHK(pre_offset, buf_inf->WriteOffset, buf_inf->ReadOffset))
    #endif
    {
        buf_inf->ReadOffset =
        (buf_inf->ReadOffset + frame_size)%(buf_inf->length);
        buf_inf->bBufferIsFull = TRUE;
    }

    if ((ADC_SOFTSTART == 0)&&(transform !=NULL))
    {
        /* Update Sink Read offset*/
        #if (FEA_SUPP_DSP_SRC_VDM)
        if ((transform->sink->type == SINK_TYPE_AUDIO)||(transform->sink->type == SINK_TYPE_VP_AUDIO))
        {
            if ((transform->sink->param.audio.src != NULL) &&
                (transform->sink->param.audio.src->src_ptr != NULL) &&
                (transform->sink->type == SINK_TYPE_AUDIO || transform->sink->type == SINK_TYPE_VP_AUDIO))
            {
                transform->sink->streamBuffer.BufferInfo.ReadOffset = DSP_GetSRCInReadOffset(transform->sink->param.audio.src->src_ptr);
            }
        }
        #endif

        streaming_ptr = DSP_Streaming_Get(transform->source,transform->sink);
        //if (streaming_ptr != NULL)
            //streaming_ptr->driftCtrl.SourceLatch(&(streaming_ptr->driftCtrl.para));

        #if (FEA_SUPP_DSP_SRC_VDM)
        if (Source_blks[SOURCE_TYPE_AUDIO]->param.audio.src != NULL)
            if (Source_blks[SOURCE_TYPE_AUDIO]->param.audio.src->enable_flag)
                goto CLEAR_AUDIO_SOURCE_INTR;
        #endif
        {
            Audio_handler = (transform->Handler);
            Audio_handler(transform->source,transform->sink);
        }
    }
    else
    {
        ADC_SOFTSTART--;
        /*Trigger to stagger oDFE read ptr and SRC Wadma ptr*/
        if ((ADC_SOFTSTART==0)&&(transform !=NULL))
            Source_Audio_Triger_SinkSRC(transform->sink);
    }
    #if (FEA_SUPP_DSP_SRC_VDM)
    CLEAR_AUDIO_SOURCE_INTR:
    #endif
    DSP_DRV_ClearWriteAdmaIsrStatus();
}


VOID Source_Audio_SelectInstance(audio_hardware hardware, audio_instance instance)
{
    if (AUDIO_HARDWARE_PCM == hardware)
    {
        if (INSTANCE_A == instance)
        {
            DSP_DRV_AFE_ChangeOperationMode(AU_AFE_OP_LINE_DSP_MODE);
        }
        else if (INSTANCE_B == instance)
        {
            DSP_DRV_AFE_ChangeOperationMode(AU_AFE_OP_ESCO_VOICE_MODE);
        }
    }
}



#ifdef AUTO_RATE_DET
VOID Source_Audio_RateDet_IsrHandler(VOID)
{
    if (RATE_DET.RATE_DET_CTRL1.field.RD_DIF_OR_CNT == 0)
    {
        Audio_setting->Rate.Source_Input_Sampling_Rate  = RATE_DET.RATE_DET_REPORT.field.RD_REPORT_DATA*24000/RATE_DET.RATE_DET_CTRL0.field.TIMER_CNT;//TODO now|| is for FPGA(24M)
    }
    else
    {
        Audio_setting->Rate.Source_Input_Sampling_Rate  = RATE_DET.RATE_DET_REPORT.field.RD_REPORT_DATA;
    }
    RATE_DET.RATE_DET_CTRL2.field.MIN_VALUE             = RATE_DET.RATE_DET_REPORT.field.RD_REPORT_DATA - 2;        // min value of rate det
    RATE_DET.RATE_DET_CTRL3.field.MAX_VALUE             = RATE_DET.RATE_DET_REPORT.field.RD_REPORT_DATA + 2;       // max value of rate det
    RATE_DET.RATE_DET_CTRL1.field.UPDATE_REQ            = 1;        // update min/max value
    RATE_DET.RATE_DET_CTRL1.field.RD_INTR_CLEAR         = 1;
}
#endif





VOID Source_Audio_Spdif_Rx_IsrHandler(VOID)
{
    #if 0
    #warning "no spdif SFR in 155x"

    U32 VOLATILE value;
    SOURCE source;
    source = Source_blks[SOURCE_TYPE_AUDIO];

    if (SPDIF.RX_INTR_CTRL.field.SAMPLE_RATE)
    {
        value = DSP_ChangeSpdifRate2Value(SPDIF.RX_SAMPLE.field.EST_RATE);

        //SPDIF CYCLE_NO = PLL_CLK * 192frames / Sampling rate
        SPDIF.RX_BLK_MIN_CYCLE.field.CYCLE_NO = SPDIF.LAST_RX_BLK_CYCLE.field.CYCLE_NO-20;
        SPDIF.RX_BLK_MAX_CYCLE.field.CYCLE_NO = SPDIF.LAST_RX_BLK_CYCLE.field.CYCLE_NO+20;
        if ((U32)(SPDIF.LAST_RX_BLK_CYCLE.field.CYCLE_NO - (0x990FA-23))<46)//Sampling rate 44.1kHz
        {
            value = 44100;
        }
        if (Audio_setting->Rate.Source_Input_Sampling_Rate != value/1000)
        {
            Audio_setting->Rate.Source_Input_Sampling_Rate = value/1000;
            if (source->transform != NULL)
            {
                DSP_STREAMING_PARA_PTR streaming_ptr ;
                streaming_ptr = DSP_Streaming_Get(source, source->transform->sink);
                streaming_ptr->callback.EntryPara.in_sampling_rate = (source->param.audio.src!=NULL) //Source VDM SRC
                                                          ? DSP_SRCOutRateChange2Value(DSP_GetSRCOutRate(source->param.audio.src->src_ptr))/1000
                                                          : AudioSourceSamplingRate_Get();

                PL_CRITICAL(DSP_Callback_ChangeStreaming2Deinit, &streaming_ptr->callback.EntryPara);
            }
        }

    }

    if (SPDIF.RX_INTR_CTRL.field.SIDE_BAND)
    {
    }

    SPDIF.RX_INTR_CTRL.field.SAMPLE_RATE = 1;
    SPDIF.RX_INTR_CTRL.field.SIDE_BAND = 1;
    SPDIF.RX_INTR_CTRL.field.TIME_OUT = 1;
    #endif
}

static U32 TimeCnt = 0;
static BOOL ExtAmpOff_SilenceFlag = FALSE;
void Sink_Audio_ExtAmpOff_Control_Init(void)
{
    TimeCnt = 0;
    ExtAmpOff_SilenceFlag = TRUE;
    hal_audio_set_value_parameter_t value_parameter_t;
    value_parameter_t.value = HAL_AUDIO_AMP_OUTPUT_GPIO;
    hal_audio_set_value(&value_parameter_t, HAL_AUDIO_SET_DEVICE_AMP_OUTPUT_GPIO);
}
extern bool hal_audio_device_set_output_gpio(hal_audio_control_status_t control, bool is_time_up);
extern U32 hal_audio_device_get_gpio(void);
void Sink_Audio_ExtAmpOff_Control_Callback(BOOL SilenceFlag)
{
    U32 gpio = hal_audio_device_get_gpio();
    
    if(SilenceFlag){
        TimeCnt++;
    }else{
        TimeCnt = 0;
    }

    if(gpio != 0XFF){
        U32 out = 0;
        hal_gpio_get_output(gpio,&out);
        if(out){
            ExtAmpOff_SilenceFlag = FALSE;
        }else{
            ExtAmpOff_SilenceFlag = TRUE;
        }
    }
    
    if(TimeCnt >= Get_ExtAmpOff_Time() && ExtAmpOff_SilenceFlag == FALSE){
        hal_audio_device_set_output_gpio(HAL_AUDIO_CONTROL_OFF,TRUE);// if silence, turn off amp
        DSP_MW_LOG_I("[SD]EAO Turn Off Amp",0);
        ExtAmpOff_SilenceFlag = TRUE;
    }else if(SilenceFlag == FALSE && ExtAmpOff_SilenceFlag == TRUE){
        hal_audio_device_set_output_gpio(HAL_AUDIO_CONTROL_ON,TRUE);
        DSP_MW_LOG_I("[SD]EAO Turn On Amp",0);
        ExtAmpOff_SilenceFlag = FALSE;
    }
}

/**
 * Sink_Audio_WriteBuffer
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL Sink_Audio_WriteBuffer (SINK sink, U8 *src_addr, U32 length)
{
    TRANSFORM transform = sink->transform;
    DSP_CALLBACK_PTR callback_ptr = NULL;
    U32 writeOffset = sink->streamBuffer.BufferInfo.WriteOffset;
    U8 i;
    U8 channel_num, channel_sel;
    U16 copy_size,copy_offset,unwrap_size;
    if (length==0) {
        return FALSE;
    }
    if (transform!=NULL && src_addr==NULL)
    {
        callback_ptr = DSP_Callback_Get(transform->source, sink);

        src_addr = callback_ptr->EntryPara.out_ptr[0];
        channel_num = sink->param.audio.channel_num;
        channel_sel = 0;
    }
    else
    {
        channel_num = 1;
        channel_sel = sink->streamBuffer.BufferInfo.channelSelected;
    }

    if ((sink->buftype != BUFFER_TYPE_INTERLEAVED_BUFFER) || (channel_num==1)) {
        for (i = 0; i < channel_num; i++) {
            DSP_D2C_BufferCopy(sink->streamBuffer.BufferInfo.startaddr[channel_sel] + writeOffset,
                               src_addr,
                               length,
                               sink->streamBuffer.BufferInfo.startaddr[channel_sel],
                               sink->streamBuffer.BufferInfo.length);


            #if MTK_AUDIO_DUMP_BY_CONFIGTOOL
            copy_offset = 0;
            #ifdef MTK_TDM_ENABLE
            writeOffset = sink->streamBuffer.BufferInfo.WriteOffset;
            #endif
            while(length>copy_offset)
            {
                unwrap_size = sink->streamBuffer.BufferInfo.length - writeOffset;
                copy_size = MIN((length-copy_offset),unwrap_size>>1);//unwrap_size/2

                if(sink->type == SINK_TYPE_VP_AUDIO){
                    LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.out_ptr[i] + copy_offset), (U32)(copy_size), PROMPT_VP_OUT);
                } else {
                    LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.out_ptr[i] + copy_offset), (U32)(copy_size), SINK_OUT1);
                }

                writeOffset = (writeOffset + (copy_size<<1))% sink->streamBuffer.BufferInfo.length; //copy_size*2
                copy_offset += (copy_size);
            }
            #endif

            if (callback_ptr!=NULL)
                src_addr = callback_ptr->EntryPara.out_ptr[++channel_sel];
        }
        #ifdef AIR_SILENCE_DETECTION_ENABLE
        if(Sink_Audio_SilenceDetection_Get_InitCnt()){
            Sink_Audio_SilenceDetection((void*)(callback_ptr->EntryPara.out_ptr[0]),length,sink);
        }
        #endif
        #ifdef AIR_A2DP_NLE_ENABLE
        hal_audio_dl_set_nle(sink->streamBuffer.BufferInfo.startaddr[channel_sel] + writeOffset,
                             length,
                             sink->param.audio.format_bytes,
                             sink->transform->source->type);
        #endif
    } else {
        for (i = 0; i < channel_num; i+=2) {
            #if 1
            copy_offset = 0;
#ifdef MTK_TDM_ENABLE
            writeOffset = sink->streamBuffer.BufferInfo.WriteOffset;
#endif
            while(length>copy_offset)
            {
                unwrap_size = sink->streamBuffer.BufferInfo.length - writeOffset;
                copy_size = MIN((length-copy_offset),unwrap_size>>1);
                if (sink->param.audio.format_bytes == 4){
                    DSP_D2I_BufferCopy_32bit((U32*)(sink->streamBuffer.BufferInfo.startaddr[channel_sel] + writeOffset),
                                             (U32*)((U8*)callback_ptr->EntryPara.out_ptr[i] + copy_offset),
                                             (U32*)((U8*)callback_ptr->EntryPara.out_ptr[i+1] + copy_offset),
                                             (U32)copy_size>>2);
                }else{
                    DSP_D2I_BufferCopy_16bit((U16*)(sink->streamBuffer.BufferInfo.startaddr[channel_sel] + writeOffset),
                                             (U16*)(callback_ptr->EntryPara.out_ptr[i] + copy_offset),
                                             (U16*)(callback_ptr->EntryPara.out_ptr[i+1] + copy_offset),
                                             copy_size>>1);
                }
                #ifdef AIR_SILENCE_DETECTION_ENABLE
                if(Sink_Audio_SilenceDetection_Get_InitCnt()){
                    Sink_Audio_SilenceDetection(sink->streamBuffer.BufferInfo.startaddr[channel_sel] + writeOffset,copy_size*2,sink);
                }
                #endif
                #ifdef AIR_A2DP_NLE_ENABLE
                    hal_audio_dl_set_nle(sink->streamBuffer.BufferInfo.startaddr[channel_sel] + writeOffset,
                                         copy_size*2,
                                         sink->param.audio.format_bytes,
                                         sink->transform->source->type);
                #endif
                #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                    if(sink->type == SINK_TYPE_VP_AUDIO){
                        LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.out_ptr[i] + copy_offset), (U32)(copy_size), PROMPT_VP_OUT);
                    } else if (sink->type == SINK_TYPE_AUDIO_DL3) {
                        LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.out_ptr[i] + copy_offset), (U32)(copy_size), PROMPT_RT_OUT);
                    } else {//DL1
                        LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.out_ptr[i] + copy_offset), (U32)(copy_size), SINK_OUT1);
                        LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.out_ptr[i+1] + copy_offset), (U32)(copy_size), SINK_OUT2);
                    }
                #endif

                writeOffset = (writeOffset + (copy_size<<1))% sink->streamBuffer.BufferInfo.length;
                copy_offset += copy_size;
            }
            #else
            DSP_D2I_BufferCopy(sink->streamBuffer.BufferInfo.startaddr[channel_sel] + writeOffset,
                               callback_ptr->EntryPara.out_ptr[i],
                               callback_ptr->EntryPara.out_ptr[i+1],
                               sink->streamBuffer.BufferInfo.startaddr[channel_sel],
                               sink->streamBuffer.BufferInfo.length,
                               length,
                               sink->param.audio.format_bytes);
            #endif
            channel_sel++;
        }
    }
    return TRUE;
}


/**
 * Sink_Audio_FlushBuffer
 */
#ifdef MTK_BT_HFP_ENABLE
extern bool g_ignore_next_drop_flag;
#endif
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL Sink_Audio_FlushBuffer(SINK sink, U32 amount)
{
    BOOL IsSinkAudio = (sink->type == SINK_TYPE_AUDIO)? TRUE : FALSE;
    U16 TargetQnum = IsSinkAudio ? Audio_setting->Audio_sink.Target_Q_Frame_Num : Audio_setting->Audio_VP.Target_Q_Frame_Num;
    BOOL* OutputEnable = IsSinkAudio ? &Audio_setting->Audio_sink.Output_Enable : &Audio_setting->Audio_VP.Output_Enable;
    U32 Buffer_LEN =RLEN(sink->streamBuffer.BufferInfo.WriteOffset,sink->streamBuffer.BufferInfo.ReadOffset,sink->streamBuffer.BufferInfo.length);
    U32 TargetLevel = sink->param.audio.frame_size*TargetQnum;
    U32 buffer_per_channel_shift = ((sink->param.audio.channel_num>=2) && (sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                                     ? 1
                                     : 0;

    if ((amount == 0)||(SinkSlack(sink)<amount))
    {

        return FALSE;
    }

    if (!(*OutputEnable))
    {
#if 1
        if ((Buffer_LEN >= TargetLevel) ||(sink->streamBuffer.BufferInfo.bBufferIsFull == TRUE))
#else
        if ((Buffer_LEN >= TargetLevel)
          ||(sink->streamBuffer.BufferInfo.bBufferIsFull == TRUE)
          ||(sink->param.audio.src != NULL))
#endif
        {
            //AudioSinkHW_Ctrl(sink, TRUE);
        }
        *OutputEnable = TRUE;
    }

    if (Audio_setting->Audio_sink.Mute_Flag)
    {
        memset(sink->streamBuffer.BufferInfo.startaddr[0],
               0,
               sink->streamBuffer.BufferInfo.length);

        if(sink->param.audio.channel_num>1)
        {
            memset(sink->streamBuffer.BufferInfo.startaddr[1],
                   0,
                   sink->streamBuffer.BufferInfo.length);
        }
    }
    if (sink->param.audio.sram_empty_fill_size != 0)
    {
        U32 mask;
        hal_nvic_save_and_set_interrupt_mask(&mask);
        if (sink->param.audio.sram_empty_fill_size >= (amount<<buffer_per_channel_shift))
        {
            sink->param.audio.sram_empty_fill_size -= (amount<<buffer_per_channel_shift);
            DSP_MW_LOG_I("reduce empty_fill_size %d %d", 2,sink->param.audio.sram_empty_fill_size,(amount<<buffer_per_channel_shift));
            hal_nvic_restore_interrupt_mask(mask);
            return TRUE;
        }
        else{
            DSP_MW_LOG_I("reduce empty_fill_size 2 zero %d %d", 2,sink->param.audio.sram_empty_fill_size,(sink->param.audio.sram_empty_fill_size>>buffer_per_channel_shift));
            amount -= (sink->param.audio.sram_empty_fill_size>>buffer_per_channel_shift);
            sink->param.audio.sram_empty_fill_size = 0;
        }
        hal_nvic_restore_interrupt_mask(mask);
    }
    U32 mask_temp;
    hal_nvic_save_and_set_interrupt_mask(&mask_temp); // add critical region during writing WO.
#ifdef MTK_BT_HFP_ENABLE
    if (!g_ignore_next_drop_flag) {
#endif
        sink->streamBuffer.BufferInfo.WriteOffset
            = (sink->streamBuffer.BufferInfo.WriteOffset + (amount<<buffer_per_channel_shift))
                % (sink->streamBuffer.BufferInfo.length);
        if (sink->streamBuffer.BufferInfo.WriteOffset == sink->streamBuffer.BufferInfo.ReadOffset){
            sink->streamBuffer.BufferInfo.bBufferIsFull = TRUE;
        }
#ifdef MTK_BT_HFP_ENABLE
    }
#endif

#ifdef MTK_BT_HFP_ENABLE
    if (g_ignore_next_drop_flag) {
        g_ignore_next_drop_flag = false;
    }
#endif
    #if 0
    if (sink->param.audio.AfeBlkControl.u4asrcflag) {
        if (IsSinkAudio) {
            AFE_WRITE(ASM_CH01_IBUF_WRPNT, sink->streamBuffer.BufferInfo.WriteOffset + AFE_READ(ASM_IBUF_SADR));
        } else if (sink->type == SINK_TYPE_VP_AUDIO) {
            AFE_WRITE(ASM2_CH01_IBUF_WRPNT, sink->streamBuffer.BufferInfo.WriteOffset + AFE_READ(ASM2_IBUF_SADR));
        }
    }
    #else

    //modify for ab1568
    if(sink->param.audio.mem_handle.memory_select&( HAL_AUDIO_MEMORY_DL_SRC1) ||
        sink->param.audio.mem_handle.pure_agent_with_src){
//#if (AFE_REGISTER_ASRC_IRQ)
#ifdef ENABLE_HWSRC_CLKSKEW
        if((ClkSkewMode_g == CLK_SKEW_V2) && (sink->param.audio.mem_handle.memory_select != HAL_AUDIO_MEMORY_DL_DL2)) {
            if(afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK)== false){//modify for clock skew
                afe_set_asrc_irq_enable(AFE_MEM_ASRC_1, true);
                //DSP_MW_LOG_I("asrc Sink_Audio_FlushBuffer asrc_irq_is_enabled %d",2,afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK));
            }
        }else{
            hal_audio_set_value_parameter_t set_value_parameter;
            set_value_parameter.set_current_offset.pure_agent_with_src = sink->param.audio.mem_handle.pure_agent_with_src;
            set_value_parameter.set_current_offset.memory_select = sink->param.audio.mem_handle.memory_select;
            set_value_parameter.set_current_offset.offset = sink->streamBuffer.BufferInfo.WriteOffset + (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];//(uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
            hal_audio_set_value(&set_value_parameter, HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET);

        }
#else
        hal_audio_set_value_parameter_t set_value_parameter;
        set_value_parameter.set_current_offset.pure_agent_with_src = sink->param.audio.mem_handle.pure_agent_with_src;
        set_value_parameter.set_current_offset.memory_select = sink->param.audio.mem_handle.memory_select;
        set_value_parameter.set_current_offset.offset = sink->streamBuffer.BufferInfo.WriteOffset + (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];//(uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
        hal_audio_set_value(&set_value_parameter, HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET);
#endif
    }else if(sink->param.audio.mem_handle.memory_select&( HAL_AUDIO_MEMORY_DL_SRC2) ||
        sink->param.audio.mem_handle.pure_agent_with_src){
        hal_audio_set_value_parameter_t set_value_parameter;
        set_value_parameter.set_current_offset.pure_agent_with_src = sink->param.audio.mem_handle.pure_agent_with_src;
        set_value_parameter.set_current_offset.memory_select = sink->param.audio.mem_handle.memory_select;
        set_value_parameter.set_current_offset.offset = sink->streamBuffer.BufferInfo.WriteOffset + (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];//(uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
        hal_audio_set_value(&set_value_parameter, HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET);

    }
    hal_nvic_restore_interrupt_mask(mask_temp);

    #endif
    //afe loopback smooth
    if(sink->transform && sink->transform->sink) {
        SOURCE source = sink->transform->source;
        if ((sink->type == SINK_TYPE_AUDIO) && (source->type == SOURCE_TYPE_AUDIO) && (!source->param.audio.lineout_scenario_flag))  {
            DSP_STREAMING_PARA_PTR  pStream = DSP_Streaming_Get(source,sink);
            bool mute_flag = source->param.audio.mute_flag;
            if (pStream) {
                mute_flag = (mute_flag || (pStream->streamingStatus == STREAMING_END));
            }

#if 0//modify for ab1568
            afe_mute_digital_gain(mute_flag, (sink->param.audio.memory == HAL_AUDIO_MEM1) ? AUDIO_HW_GAIN : AUDIO_HW_GAIN2);
#else
            hal_audio_volume_digital_gain_parameter_t           digital_gain;
            memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
            digital_gain.memory_select = sink->param.audio.memory;
            digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_FRAMEWORK;
            digital_gain.mute_enable = mute_flag;
            digital_gain.is_mute_control = true;
            //DSP_MW_LOG_I("Sink_Audio_FlushBuffer, sink & source = audio, memory select [%d], mute enable [%d]", 2, sink->param.audio.memory, mute_flag);
            hal_audio_set_value((hal_audio_set_value_parameter_t*)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
#endif

        }
    }
    return TRUE;
}

/**
 * Sink_Audio_CloseProcedure
 */
BOOL Sink_Audio_CloseProcedure(SINK sink)
{
#if 0
    Audio_setting->Audio_sink.Zero_Padding_Cnt = 0;
    Sink_Audio_ZeroPadding(); // be careful of task pending
    AudioSinkHW_Ctrl (sink, FALSE);
#endif

   // AudioSinkHW_Ctrl (sink, FALSE);
    Sink_Audio_SRC_Ctrl(sink, FALSE, 0);
    Sink_Audio_Buffer_Ctrl(sink, FALSE);

#if 0
    if (sink->type == SINK_TYPE_AUDIO)
    {
       //OS_LV3_INTR_CancelHandler(OS_LV3_INTR_ID_SRC_A_IN); // how to determine the enabled hardware is SRC_A?
       // OS_LV3_INTR_CancelHandler(OS_LV3_INTR_ID_IDFE_AU);
        INTR_CancelHandler(INTR_ID_SRC_IN);
        INTR_CancelHandler(INTR_ID_ODFE_AU);
        Audio_Sink_Status.DSP_Audio_busy = FALSE;
    }
    else if (sink->type == SINK_TYPE_VP_AUDIO)
    {
        //OS_LV3_INTR_CancelHandler(OS_LV3_INTR_ID_ODFE_VP);
        INTR_CancelHandler(INTR_ID_ODFE_VP);
        Audio_Sink_Status.DSP_vp_path_busy = FALSE;
    }
#endif

    DSP_CTRL_ChangeStatus(AU_OUTPUT_DSP, AU_DSP_STATUS_OFF);

    return TRUE;
}

BOOL AudioAfeConfiguration(stream_config_type type, U32 value)
{
    U32 i, found;

    switch (type)
    {
        case AUDIO_SINK_MUTE_ENABLE:
            Audio_setting->Audio_sink.Mute_Flag = (value ==0)? FALSE : TRUE;
            break;

        case AUDIO_SOURCE_MUTE_ENABLE:
            Audio_setting->Audio_source.Mute_Flag = (value ==0)? FALSE : TRUE;
            break;
        case AUDIO_SINK_DATA_FORMAT:
        case AUDIO_SOURCE_DATA_FORMAT:
            if (value < AFE_PCM_FORMAT_LAST) {
                if (type == AUDIO_SINK_DATA_FORMAT)
                    gAudioCtrl.Afe.AfeDLSetting.format = value;
                else
                    gAudioCtrl.Afe.AfeULSetting.format = value;
            }
            else
                return FALSE;
            break;
        case AUDIO_SINK_IRQ_PERIOD:
        case AUDIO_SOURCE_IRQ_PERIOD:         // 18 bits
            //if (gAudioCtrl.Afe.AfeDLSetting.rate*value < IRQ_COUNT_MAX) {
                 if (type == AUDIO_SINK_IRQ_PERIOD)
                    gAudioCtrl.Afe.AfeDLSetting.period = value;
                 else
                    gAudioCtrl.Afe.AfeULSetting.period = value;
            //}
            //else
                //return FALSE;
            break;
        case AUDIO_SINK_IRQ_RATE:
        case AUDIO_SOURCE_IRQ_RATE:
        case AUDIO_SRC_RATE:
            found = 0;
            for(i=0; i<IRQ_RATE_NUM; i++) {
                if (Irq_rate_table[i] == value ) {
                    found = 1;
                    break;
                }
            }
            if (found) {
                if (type == AUDIO_SINK_IRQ_RATE)
                {
                    gAudioCtrl.Afe.AfeDLSetting.rate = value;
                    gAudioCtrl.Afe.AfeDLSetting.src_rate = value;// sink for SRC in
                }
                if (type == AUDIO_SRC_RATE)
                {
                    gAudioCtrl.Afe.AfeDLSetting.src_rate = value;// sink for SRC in
                    #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
                    gAudioCtrl.Afe.AfeULSetting.src_rate = value; // source for SRC out
                    #endif
                }
                else
                {
                    gAudioCtrl.Afe.AfeULSetting.rate = value;
                    gAudioCtrl.Afe.AfeULSetting.src_rate = value;// source for SRC out
                }
            }
            else {
               // printf("wrong sampling rate\r\n");
               return FALSE;
            }
            break;
#if 0
        case AUDIO_SINK_IRQ_COUNT:
            if (value < IRQ_COUNT_MAX);

            break;
#endif
        case AUDIO_SINK_DEVICE:
            gAudioCtrl.Afe.AfeDLSetting.audio_device = value;
            break;
        case AUDIO_SINK_CHANNEL:
            gAudioCtrl.Afe.AfeDLSetting.stream_channel = value;
            break;
        case AUDIO_SINK_MEMORY:
            gAudioCtrl.Afe.AfeDLSetting.memory = value;
            break;
        case AUDIO_SINK_INTERFACE:
            gAudioCtrl.Afe.AfeDLSetting.audio_interface = value;
            break;
        case AUDIO_SINK_HW_GAIN:
            gAudioCtrl.Afe.AfeDLSetting.hw_gain = value;
            break;
        #ifdef HAL_AUDIO_ENABLE_PATH_MEM_DEVICE
        case AUDIO_SINK_ADC_MODE:
            gAudioCtrl.Afe.AfeDLSetting.adc_mode = value;
        break;
        case AUDIO_SINK_I2S_FORMAT:
            gAudioCtrl.Afe.AfeDLSetting.i2s_format = value;
        break;
        case AUDIO_SINK_I2S_SLAVE_TDM:
#ifdef MTK_TDM_ENABLE
            gAudioCtrl.Afe.AfeDLSetting.tdm_channel = value;
#else
            gAudioCtrl.Afe.AfeDLSetting.i2S_Slave_TDM = value;
#endif
        break;
        case AUDIO_SINK_I2S_WORD_LENGTH:
            gAudioCtrl.Afe.AfeDLSetting.i2s_word_length = value;
        break;
        case AUDIO_SINK_I2S_LOW_JITTER:
            gAudioCtrl.Afe.AfeDLSetting.is_low_jitter = value;
        break;
        case AUDIO_SINK_DAC_PERFORMANCE:
            gAudioCtrl.Afe.AfeDLSetting.performance = value;
        break;
        #endif
        #ifdef AUTO_ERROR_SUPPRESSION
        case AUDIO_SINK_MISC_PARMS_I2S_CLK:
            gAudioCtrl.Afe.AfeDLSetting.misc_parms.I2sClkSourceType = value;
            break;
        case AUDIO_SINK_MISC_PARMS_MICBIAS:
            gAudioCtrl.Afe.AfeDLSetting.misc_parms.MicbiasSourceType = value;
            break;
        #endif
        case AUDIO_SINK_ECHO_REFERENCE:
            gAudioCtrl.Afe.AfeDLSetting.echo_reference = value;
            break;
        case AUDIO_SINK_SW_CHANNELS:
            Audio_setting->Audio_sink.Software_Channel_Num = value;
            break;

        case AUDIO_SOURCE_DEVICE:
            gAudioCtrl.Afe.AfeULSetting.audio_device = value;
            break;
#ifdef ENABLE_2A2D_TEST
        case AUDIO_SOURCE_DEVICE1:
            gAudioCtrl.Afe.AfeULSetting.audio_device1 = value;
            break;
        case AUDIO_SOURCE_DEVICE2:
            gAudioCtrl.Afe.AfeULSetting.audio_device2 = value;
            break;
        case AUDIO_SOURCE_DEVICE3:
            gAudioCtrl.Afe.AfeULSetting.audio_device3 = value;
            break;
        case AUDIO_SOURCE_DEVICE4:
            gAudioCtrl.Afe.AfeULSetting.audio_device4 = value;
            break;
        case AUDIO_SOURCE_DEVICE5:
            gAudioCtrl.Afe.AfeULSetting.audio_device5 = value;
            break;
        case AUDIO_SOURCE_DEVICE6:
            gAudioCtrl.Afe.AfeULSetting.audio_device6 = value;
            break;
        case AUDIO_SOURCE_DEVICE7:
            gAudioCtrl.Afe.AfeULSetting.audio_device7 = value;
            break;
#endif
        case AUDIO_SOURCE_CHANNEL:
            gAudioCtrl.Afe.AfeULSetting.stream_channel = value;
            break;
        case AUDIO_SOURCE_MEMORY:
            gAudioCtrl.Afe.AfeULSetting.memory = value;
            break;
        case AUDIO_SOURCE_INTERFACE:
            gAudioCtrl.Afe.AfeULSetting.audio_interface = value;
            break;
#ifdef ENABLE_2A2D_TEST
        case AUDIO_SOURCE_INTERFACE1:
            gAudioCtrl.Afe.AfeULSetting.audio_interface1 = value;
            break;
        case AUDIO_SOURCE_INTERFACE2:
            gAudioCtrl.Afe.AfeULSetting.audio_interface2 = value;
            break;
        case AUDIO_SOURCE_INTERFACE3:
            gAudioCtrl.Afe.AfeULSetting.audio_interface3 = value;
            break;
        case AUDIO_SOURCE_INTERFACE4:
            gAudioCtrl.Afe.AfeULSetting.audio_interface4 = value;
            break;
        case AUDIO_SOURCE_INTERFACE5:
            gAudioCtrl.Afe.AfeULSetting.audio_interface5 = value;
            break;
        case AUDIO_SOURCE_INTERFACE6:
            gAudioCtrl.Afe.AfeULSetting.audio_interface6 = value;
            break;
        case AUDIO_SOURCE_INTERFACE7:
            gAudioCtrl.Afe.AfeULSetting.audio_interface7 = value;
            break;
#endif
        case AUDIO_SOURCE_HW_GAIN:
            gAudioCtrl.Afe.AfeULSetting.hw_gain = value;
            break;
#ifdef ENABLE_HWSRC_CLKSKEW
        case AUDIO_SINK_CLKSKEW_MODE:
            Audio_setting->Audio_sink.clkskew_mode = value;
            break;
#endif
        #ifdef AUTO_ERROR_SUPPRESSION
        case AUDIO_SOURCE_MISC_PARMS_I2S_CLK:
            gAudioCtrl.Afe.AfeULSetting.misc_parms.I2sClkSourceType = value;
            break;
        case AUDIO_SOURCE_MISC_PARMS_MICBIAS:
            gAudioCtrl.Afe.AfeULSetting.misc_parms.MicbiasSourceType = value;
            break;
        #endif
        case AUDIO_SOURCE_ECHO_REFERENCE:
            gAudioCtrl.Afe.AfeULSetting.echo_reference = value;
            break;
        case AUDIO_SOURCE_MISC_PARMS:
            value = ((value >> 14)&0x3);
            switch(value){
                case 0:
                    gAudioCtrl.Afe.AfeULSetting.adc_mode = 2;
                    break;
                case 1:
                    gAudioCtrl.Afe.AfeULSetting.adc_mode = 0;
                    break;
                case 2:
                    gAudioCtrl.Afe.AfeULSetting.adc_mode = 1;
                default:
                    break;
            }
            break;
        case AUDIO_SOURCE_ADC_MODE:
            gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[0] = value;
            break;
#ifdef ENABLE_2A2D_TEST
        case AUDIO_SOURCE_ADC_MODE1:
            gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[1] = value;
            break;
        case AUDIO_SOURCE_ADC_MODE2:
            gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[2] = value;
            break;
        case AUDIO_SOURCE_ADC_MODE3:
            gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[3] = value;
            break;
        case AUDIO_SOURCE_ADC_MODE4:
            gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[4] = value;
            break;
        case AUDIO_SOURCE_ADC_MODE5:
            gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[5] = value;
            break;
        case AUDIO_SOURCE_ADC_MODE6:
            gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[6] = value;
            break;
        case AUDIO_SOURCE_ADC_MODE7:
            gAudioCtrl.Afe.AfeULSetting.ul_adc_mode[7] = value;
            break;
#endif
        case AUDIO_SOURCE_BIAS_VOLTAGE:
            gAudioCtrl.Afe.AfeULSetting.bias_voltage[0] = value;
        break;
#ifdef ENABLE_2A2D_TEST
        case AUDIO_SOURCE_BIAS_VOLTAGE1:
            gAudioCtrl.Afe.AfeULSetting.bias_voltage[1] = value;
        break;
        case AUDIO_SOURCE_BIAS_VOLTAGE2:
            gAudioCtrl.Afe.AfeULSetting.bias_voltage[2] = value;
        break;
        case AUDIO_SOURCE_BIAS_VOLTAGE3:
            gAudioCtrl.Afe.AfeULSetting.bias_voltage[3] = value;
        break;
        case AUDIO_SOURCE_BIAS_VOLTAGE4:
            gAudioCtrl.Afe.AfeULSetting.bias_voltage[4] = value;
        break;
#endif
        case AUDIO_SOURCE_BIAS_SELECT:
            gAudioCtrl.Afe.AfeULSetting.bias_select = value;
        break;
        case AUDIO_SOURCE_WITH_EXTERNAL_BIAS:
            gAudioCtrl.Afe.AfeULSetting.with_external_bias = value;
        break;
        case AUDIO_SOURCE_WITH_BIAS_LOWPOWER:
            gAudioCtrl.Afe.AfeULSetting.with_bias_lowpower = value;
        break;
        case AUDIO_SOURCE_BIAS1_BIAS2_WITH_LDO0:
            gAudioCtrl.Afe.AfeULSetting.bias1_2_with_LDO0 = value;
        break;
        case AUDIO_SOURCE_DMIC_SELECT:
            gAudioCtrl.Afe.AfeULSetting.dmic_selection[0] = value;
        break;
#ifdef ENABLE_2A2D_TEST
        case AUDIO_SOURCE_DMIC_SELECT1:
            gAudioCtrl.Afe.AfeULSetting.dmic_selection[1] = value;
        break;
        case AUDIO_SOURCE_DMIC_SELECT2:
            gAudioCtrl.Afe.AfeULSetting.dmic_selection[2] = value;
        break;
        case AUDIO_SOURCE_DMIC_SELECT3:
            gAudioCtrl.Afe.AfeULSetting.dmic_selection[3] = value;
        break;
        case AUDIO_SOURCE_DMIC_SELECT4:
            gAudioCtrl.Afe.AfeULSetting.dmic_selection[4] = value;
        break;
        case AUDIO_SOURCE_DMIC_SELECT5:
            gAudioCtrl.Afe.AfeULSetting.dmic_selection[5] = value;
        break;
        case AUDIO_SOURCE_DMIC_SELECT6:
            gAudioCtrl.Afe.AfeULSetting.dmic_selection[6] = value;
        break;
        case AUDIO_SOURCE_DMIC_SELECT7:
            gAudioCtrl.Afe.AfeULSetting.dmic_selection[7] = value;
        break;

#endif
        case AUDIO_SOURCE_UL_IIR:
            gAudioCtrl.Afe.AfeULSetting.iir_filter[0] = value;
        break;
#ifdef ENABLE_2A2D_TEST
        case AUDIO_SOURCE_UL_IIR1:
            gAudioCtrl.Afe.AfeULSetting.iir_filter[1] = value;
        break;
        case AUDIO_SOURCE_UL_IIR2:
            gAudioCtrl.Afe.AfeULSetting.iir_filter[2] = value;
        break;
#endif
        case AUDIO_SOURCE_I2S_FORMAT:
            gAudioCtrl.Afe.AfeULSetting.i2s_format = value;
        break;
        case AUDIO_SOURCE_I2S_SLAVE_TDM:
#ifdef MTK_TDM_ENABLE
            gAudioCtrl.Afe.AfeULSetting.tdm_channel = value;
#else
            gAudioCtrl.Afe.AfeULSetting.i2S_Slave_TDM = value;
#endif
        break;
        case AUDIO_SOURCE_I2S_WORD_LENGTH:
            gAudioCtrl.Afe.AfeULSetting.i2s_word_length = value;
        break;
        case AUDIO_SOURCE_I2S_LOW_JITTER:
            gAudioCtrl.Afe.AfeULSetting.is_low_jitter = value;
        break;
        case AUDIO_SOURCE_UL_PERFORMANCE:
            gAudioCtrl.Afe.AfeULSetting.performance = value;
        break;
        case AUDIO_SOURCE_DMIC_CLOCK:
            gAudioCtrl.Afe.AfeULSetting.dmic_clock_rate[0] = value;
        break;
#ifdef ENABLE_2A2D_TEST
        case AUDIO_SOURCE_DMIC_CLOCK1:
            gAudioCtrl.Afe.AfeULSetting.dmic_clock_rate[1] = value;
        break;
        case AUDIO_SOURCE_DMIC_CLOCK2:
            gAudioCtrl.Afe.AfeULSetting.dmic_clock_rate[2] = value;
        break;
#endif
        case AUDIO_SOURCE_UPDOWN_SAMPLER_ENABLE:
            gAudioCtrl.Afe.AfeULSetting.with_upwdown_sampler = value;
            break;
        case AUDIO_SOURCE_PATH_INPUT_RATE:
            gAudioCtrl.Afe.AfeULSetting.audio_path_input_rate = value;
            break;
        case AUDIO_SOURCE_PATH_OUTPUT_RATE:
            gAudioCtrl.Afe.AfeULSetting.audio_path_output_rate = value;
            break;
        case AUDIO_SINK_UPDOWN_SAMPLER_ENABLE:
            gAudioCtrl.Afe.AfeDLSetting.with_upwdown_sampler = value;
            break;
        case AUDIO_SINK_PATH_INPUT_RATE:
            gAudioCtrl.Afe.AfeDLSetting.audio_path_input_rate = value;
            break;
        case AUDIO_SINK_PATH_OUTPUT_RATE:
            gAudioCtrl.Afe.AfeDLSetting.audio_path_output_rate = value;
            break;
#ifdef AIR_HFP_DNN_PATH_ENABLE
        case AUDIO_SOURCE_DNN_PATH_ENABLE:
        case AUDIO_SINK_DNN_PATH_ENABLE:
            if(type == AUDIO_SOURCE_DNN_PATH_ENABLE) {
                gAudioCtrl.Afe.AfeULSetting.enable_ul_dnn = value;
            } else if (type == AUDIO_SINK_DNN_PATH_ENABLE) {
                gAudioCtrl.Afe.AfeDLSetting.enable_ul_dnn = value;
            }
        break;
#endif
        default:
            return FALSE;
            break;
    }
    return TRUE;
}

/**
 * Sink_Audio_Configuration
 */
BOOL Sink_Audio_Configuration(SINK sink, stream_config_type type, U32 value)
{
    BOOL reset_HW = FALSE;
    switch (type)
    {
        case AUDIO_SINK_MUTE_ENABLE:
            Audio_setting->Audio_sink.Mute_Flag = (value ==0)? FALSE : TRUE;
            break;
#if 0   // TODO
        case AUDIO_SINK_UPSAMPLE_RATE:

            break;
        case AUDIO_SINK_RESOLUTION:
            break;
#endif
        case AUDIO_SINK_FRAME_SIZE:
            Audio_setting->Audio_sink.Frame_Size = (U16)value;
             if (((sink->type == SINK_TYPE_AUDIO) && (Audio_setting->Audio_sink.Output_Enable==TRUE)) ||
                 ((sink->type == SINK_TYPE_VP_AUDIO) && (Audio_setting->Audio_VP.Output_Enable==TRUE)))
            {
                AudioSinkHW_Ctrl(sink, FALSE);
                reset_HW = TRUE;
            }
            Sink_Audio_Buffer_Ctrl(sink,FALSE);
            Sink_Audio_Buffer_Ctrl(sink,TRUE);
            break;

        case AUDIO_SINK_FRAME_NUMBER:
            Audio_setting->Audio_sink.Buffer_Frame_Num = (U8)value;
            Sink_Audio_Buffer_Ctrl(sink,FALSE);
            Sink_Audio_Buffer_Ctrl(sink,TRUE);
            break;

        case AUDIO_VP_SINK_FRAME_SIZE:
            Audio_setting->Audio_VP.Frame_Size = (U16)value;
            Sink_Audio_Buffer_Ctrl(sink,FALSE);
            Sink_Audio_Buffer_Ctrl(sink,TRUE);
            break;

        case AUDIO_VP_SINK_FRAME_NUMBER:
            Audio_setting->Audio_VP.Buffer_Frame_Num = (U8)value;
            Sink_Audio_Buffer_Ctrl(sink,FALSE);
            Sink_Audio_Buffer_Ctrl(sink,TRUE);
            break;

        case AUDIO_SINK_SIDE_TONE_ENABLE :
            DSP_DRV_SIDETONE_INIT(value,ADMA_CH0);
            break;

        case AUDIO_SINK_SIDE_TONE_DISABLE :
            DSP_DRV_SIDETONE_END();
            break;

        case AUDIO_SINK_LR_SWITCH_ENABLE:
            break;

        case AUDIO_SINK_SRC_ENABLE:
           // Audio_setting->Audio_sink.SRC_Out_Enable = TRUE;
            Sink_Audio_SRC_Ctrl(sink, TRUE, value);
            break;

        case AUDIO_SINK_SRC_DISABLE:
            //Audio_setting->Audio_sink.SRC_Out_Enable = FALSE;
            Sink_Audio_SRC_Ctrl(sink, FALSE, value);
            break;
        case AUDIO_SINK_SRCIN_SAMPLE_RATE:
            Audio_setting->Rate.SRC_Sampling_Rate = value;
            Sink_Audio_SRC_Ctrl(sink, Audio_setting->Audio_sink.SRC_Out_Enable, Audio_setting->Rate.SRC_Sampling_Rate);
            break;
        case AUDIO_SINK_SRCIN_RESOLUTION:
            Audio_setting->resolution.SRCInRes = value;
            Sink_Audio_SRC_Ctrl(sink, Audio_setting->Audio_sink.SRC_Out_Enable, Audio_setting->Rate.SRC_Sampling_Rate);
            break;
        case AUDIO_SOURCE_CH_SELECT:
            sink->streamBuffer.BufferInfo.channelSelected
                = (value < BUFFER_INFO_CH_NUM)
                    ? value : 0;
            break;
        case AUDIO_SINK_PAUSE :
            Audio_setting->Audio_sink.Pause_Flag = TRUE;
            break;
        case AUDIO_SINK_RESUME :
            Audio_setting->Audio_sink.Pause_Flag = FALSE;
            break;

        case AUDIO_SINK_FORCE_START:
            AudioSinkHW_Ctrl(sink, TRUE);
            if (Source_blks[SOURCE_TYPE_AUDIO] != NULL || Source_blks[SOURCE_TYPE_AUDIO2] != NULL)
                ADC_SOFTSTART = 0;
            break;
        case AUDIO_SINK_SET_HANDLE:
            sink->param.audio.handler = (VOID*)value;
            break;
        default:
            //printf("Wrong configure type");
            return FALSE;
            break;
    }
    if (reset_HW)
    {
        AudioSinkHW_Ctrl(sink, TRUE);
    }
    return TRUE;
}


/**
 * Source_Audio_ReadAudioBuffer
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL Source_Audio_ReadAudioBuffer(SOURCE source, U8* dst_addr, U32 length)
{
    TRANSFORM transform =  source->transform;
    DSP_CALLBACK_PTR callback_ptr = NULL;
    U32 ReadOffset = source->streamBuffer.BufferInfo.ReadOffset;
    U8 i;
    U8 channel_num = 0, channel_sel;
    U16 unwrap_size,copy_offset,copy_size;
    U8* deChannel_ptr;
    if (transform!=NULL && dst_addr==NULL)
    {
        callback_ptr = DSP_Callback_Get(source, transform->sink);
        dst_addr = callback_ptr->EntryPara.in_ptr[0];
        channel_num = callback_ptr->EntryPara.in_channel_num;
        channel_sel = 0;
    }
    else
    {
        channel_num = 1;
        channel_sel = source->streamBuffer.BufferInfo.channelSelected;
    }
    if ((source->buftype != BUFFER_TYPE_INTERLEAVED_BUFFER) || (source->param.audio.channel_num==1) || (transform == NULL) || (callback_ptr == NULL)) {
        for (i = 0 ; i < channel_num ; i++) {
            DSP_C2D_BufferCopy(dst_addr,
                               source->streamBuffer.BufferInfo.startaddr[channel_sel]+ReadOffset,
                               length,
                               source->streamBuffer.BufferInfo.startaddr[channel_sel],
                               source->streamBuffer.BufferInfo.length);
            /* Fill zero packet to prevent UL pop noise */
            if(source->param.audio.mute_flag == TRUE) {
                memset(dst_addr, 0, length);
            }

            if (callback_ptr!=NULL) {
                dst_addr = callback_ptr->EntryPara.in_ptr[++channel_sel];
            }
        }
    } else {
        for (i = 0; i < channel_num; i+=2) {
            copy_offset = 0;
            ReadOffset = source->streamBuffer.BufferInfo.ReadOffset;
            while(length>copy_offset)
            {
                unwrap_size = source->streamBuffer.BufferInfo.length - ReadOffset; /* Remove + 1 to sync more common usage */
                copy_size = MIN((length-copy_offset),unwrap_size>>1);
                if (source->param.audio.format_bytes == 4){
                    if(!(callback_ptr->EntryPara.in_ptr[i+1] == NULL)){
                        deChannel_ptr = callback_ptr->EntryPara.in_ptr[i+1] + copy_offset; /* For 2-Mic Path: L R L R L R ... */
                    }
                    else{
                        deChannel_ptr = callback_ptr->EntryPara.in_ptr[i+1]; /* For EC Path: C X C X C X ... */
                    }
                    #if 1//modify for 32bit UL path with echo ref
                    DSP_I2D_BufferCopy_32bit_mute((U32*)(source->streamBuffer.BufferInfo.startaddr[channel_sel] + ReadOffset),
                                                  (U32*)((U8*)callback_ptr->EntryPara.in_ptr[i] + copy_offset),
                                                  (U32*)(deChannel_ptr),
                                                  copy_size>>2,
                                                  source->param.audio.mute_flag);
                    #else
                    DSP_I2D_BufferCopy_32bit_mute((U32*)(source->streamBuffer.BufferInfo.startaddr[channel_sel] + ReadOffset),
                                                  (U32*)((U8*)callback_ptr->EntryPara.in_ptr[i] + copy_offset),
                                                  (U32*)((U8*)callback_ptr->EntryPara.in_ptr[i+1] + copy_offset),
                                                  copy_size>>2,
                                                  source->param.audio.mute_flag);

                    #endif
                }else{
                    if(!(callback_ptr->EntryPara.in_ptr[i+1] == NULL)){
                        deChannel_ptr = callback_ptr->EntryPara.in_ptr[i+1] + copy_offset; /* For 2-Mic Path: L R L R L R ... */
                    }
                    else{
                        deChannel_ptr = callback_ptr->EntryPara.in_ptr[i+1]; /* For EC Path: C X C X C X ... */
                    }
                    DSP_I2D_BufferCopy_16bit_mute((U16*)(source->streamBuffer.BufferInfo.startaddr[channel_sel] + ReadOffset),
                                                  (U16*)(callback_ptr->EntryPara.in_ptr[i] + copy_offset),
                                                  (U16*)(deChannel_ptr),
                                                  copy_size>>1,
                                                  source->param.audio.mute_flag);
                }
                ReadOffset = (ReadOffset + (copy_size<<1))% source->streamBuffer.BufferInfo.length;
                copy_offset += copy_size;
            }
            channel_sel++;

#ifdef ENABLE_2A2D_TEST
            if (((i == channel_num-2) || (i == channel_num-1)) && (channel_num >= 4) && (source->param.audio.echo_reference)) { /* for 2A2D, echo ref.*/
                i--;
                //DSP_MW_LOG_I("2A2D channel_total = %d, channel cur = %d", 2, channel_num, i);
            }
#endif
        }
    }

#if 0 //dump ull headset mic raw data
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    if((source->transform->sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (source->transform->sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MAX) && (source->transform->sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)) {
       if(callback_ptr->EntryPara.in_ptr[0] != NULL){
            LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.in_ptr[0]), length, SOURCE_IN4);
       }
    }
#endif
#endif

#ifdef ENABLE_2A2D_TEST
  /*if(source->transform->sink->type == SINK_TYPE_N9SCO)
    {
        if(callback_ptr->EntryPara.in_ptr[0] != NULL)
            LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.in_ptr[0]), length, SOURCE_IN1);
        if(callback_ptr->EntryPara.in_ptr[1] != NULL)
            LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.in_ptr[1]), length, SOURCE_IN2);
        if(callback_ptr->EntryPara.in_ptr[2] != NULL)
            LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.in_ptr[2]), length, SOURCE_IN3);
        if(callback_ptr->EntryPara.in_ptr[3] != NULL)
            LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.in_ptr[3]), length, SOURCE_IN4);
        if(callback_ptr->EntryPara.in_ptr[4] != NULL)
            LOG_AUDIO_DUMP((U8*)(callback_ptr->EntryPara.in_ptr[4]), length, SOURCE_IN5);
    }*/
#endif

    return TRUE;
}


/**
 * Source_Audio_CloseProcedure
 */
BOOL Source_Audio_CloseProcedure(SOURCE source)
{
    // Disable DFE & audio HW
    AudioSourceHW_Ctrl(source, FALSE);

    // Disable ADMA
    Source_Audio_SRC_Ctrl(source, FALSE, 0);
    Source_Audio_Buffer_Ctrl(source,FALSE);
    DSP_CTRL_ChangeStatus(AU_INPUT_DSP, AU_DSP_STATUS_OFF);

    return TRUE;
}


/**
 * Source_Audio_Configuration
 */
BOOL Source_Audio_Configuration(SOURCE source, stream_config_type type, U32 value)
{

    switch (type)
    {
        case AUDIO_SOURCE_MUTE_ENABLE:
            Audio_setting->Audio_source.Mute_Flag = (value ==0) ? TRUE : FALSE;
            break;

        case AUDIO_SOURCE_DOWNSAMP_RATE:
            Audio_setting->Rate.Source_DownSampling_Ratio = value;
            //todo

            break;
        case AUDIO_SOURCE_RESOLUTION:
            Audio_setting->resolution.AudioInRes = value;
            //todo

            break;
        case AUDIO_SOURCE_FRAME_SIZE:
            Audio_setting->Audio_source.Frame_Size = value;
            Source_Audio_Buffer_Ctrl(source,FALSE);
            Source_Audio_Buffer_Ctrl(source,TRUE);
            break;

        case AUDIO_SOURCE_FRAME_NUMBER:
            Audio_setting->Audio_source.Buffer_Frame_Num = value;
            Source_Audio_Buffer_Ctrl(source,FALSE);
            Source_Audio_Buffer_Ctrl(source,TRUE);
            break;

        case AUDIO_SOURCE_SRC_ENABLE:
            //todo
            break;

        case AUDIO_SOURCE_SRC_DISABLE:
            //Todo
            break;

        case AUDIO_SOURCE_CH_SELECT:
            source->streamBuffer.BufferInfo.channelSelected
                = (value<BUFFER_INFO_CH_NUM)
                    ? value : 0;
            break;

        default:
            //printf("Wrong configure type");
            return FALSE;
            break;
    }

    return TRUE;
}


/**
 * Source_Audio_GetVoicePromptSize
 */
U32 Source_Audio_GetVoicePromptSize(SOURCE source)
{
    return source->param.VPRT.PatternNum - source->streamBuffer.BufferInfo.ReadOffset;
}


/**
 * Source_Audio_ReadVoicePromptBuffer
 */
BOOL Source_Audio_ReadVoicePromptBuffer(SOURCE source, U8 *dst_addr, U32 length)
{
/*
    U32 remainlength = SourceSize_AudioPattern_VP(source);
    U32 readOffset = source->streamBuffer.BufferInfo.ReadOffset;
    if(length <= remainlength)
    {
        fFlashDrv->ByteRead((FLASH_ADDR)source->streamBuffer.BufferInfo.startaddr[0]+readOffset, dst_addr, length);
        return TRUE;
    }
    else
    {
        fFlashDrv->ByteRead((FLASH_ADDR)source->streamBuffer.BufferInfo.startaddr[0]+readOffset, dst_addr, remainlength);
        return FALSE;
    }
    */
    UNUSED(source);
    UNUSED(dst_addr);
    UNUSED(length);
    return TRUE;
}

/**
 * Source_Audio_DropVoicePrompt
 */
VOID Source_Audio_DropVoicePrompt(SOURCE source, U32 amount)
{
    source->streamBuffer.BufferInfo.ReadOffset += amount;
    if (source->streamBuffer.BufferInfo.ReadOffset > source->param.VPRT.PatternNum)
    {
        source->streamBuffer.BufferInfo.ReadOffset = source->param.VPRT.PatternNum;
    }
}


/**
 * Source_Audio_GetRingtoneSize
 */
U32 Source_Audio_GetRingtoneSize(SOURCE source)
{
    if (source->param.VPRT.PatternNum !=0)
    {
        return 3;
    }
    else
    {
        return 0;
    }
}


/**
 * Source_Audio_ReadRingtoneBuf
 */
BOOL Source_Audio_ReadRingtoneBuf(SOURCE source, U8 *dst_addr, U32 length)
{
    UNUSED(length);
    UNUSED(source);
    UNUSED(dst_addr);
    return FALSE;
/*
    if(source->param.VPRT.PatternNum>0)
    {
        *dst_addr           = source->param.VPRT.para.RTInfo.tone;
        *(S16*)(dst_addr+1) = source->param.VPRT.para.RTInfo.volume;

        return TRUE;
    }

    return FALSE;
    */
}

/**
 * Source_Audio_DropRingtone
 */
VOID Source_Audio_DropRingtone(SOURCE source, U32 amount)
{
    UNUSED(amount);
    UNUSED(source);
/*
    if (source->param.VPRT.para.RTInfo.CycleNum == 0 )
    {
        if (source->param.VPRT.PatternNum != 0)
        {
            #ifdef TestReadPattern
                fFlashDrv->ByteRead((FLASH_ADDR)source->streamBuffer.BufferInfo.startaddr[0], source->streamBuffer.BufferInfo.startaddr[1], 4);// Load first tone ptr
                fFlashDrv->ByteRead((FLASH_ADDR)source->streamBuffer.BufferInfo.startaddr[1], &source->param.VPRT.para.RTInfo, 6);// load first Tone
                source->streamBuffer.BufferInfo.startaddr[0] += 4; // Next tone ptr
            #else
                memcpy(&source->param.VPRT.para.RTInfo,source->streamBuffer.BufferInfo.startaddr[0],6);
                source->streamBuffer.BufferInfo.startaddr[0] += 6; // 2 byte tone length + 4 byte  first tone ptr
            #endif
            source->param.VPRT.PatternNum--;
        }
        else
        {
            //Source disable process
        }
    }
    else
    {
        source->param.VPRT.para.RTInfo.CycleNum--;
        source->param.VPRT.para.RTInfo.volume += source->param.VPRT.para.RTInfo.volume_step;
    }*/
}



/**
 * Sink_VirtualSink_Configuration
 */
BOOL Sink_VirtualSink_Configuration(SINK sink, stream_config_type type, U32 value)
{
    switch (type)
    {
        case VIRTUAL_SINK_SET_HANDLE:
            sink->param.virtual_para.handler = (VOID*)value;
            break;
        case VIRTUAL_SINK_BUF_SIZE:
            DSPMEM_Free(sink->taskid, sink);
            DSPMEM_tmalloc(sink->taskid,value,sink);
            sink->param.virtual_para.mem_size = value;
        default:
            //printf("Wrong configure type");
            return FALSE;
            break;
    }

    return TRUE;
}

#endif /* AIR_AUDIO_HARDWARE_ENABLE */

