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
 *@file   stream_audio_afe.c
 *@brief  Defines the audio stream
 *
 @verbatim
         * Programmer :
 @endverbatim
 */

#include <string.h>
#include "types.h"
#include "dsp_drv_dfe.h"
#include "stream_audio.h"
#include "audio_config.h"
#include "dsp_buffer.h"
#include "source.h"
#include "sink.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "davt.h"
#include "dsp_memory.h"
#include "hal_audio_afe_control.h"
#include "audio_afe_common.h"
#include "clk_skew.h"
#ifdef MTK_ANC_ENABLE
#include "anc_api.h"
#endif
#ifdef MTK_GAMING_MODE_HEADSET
#include "dsp_audio_msg_define.h"
#endif
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
#include "dsp_audio_msg_define.h"
#endif

////////////////////////////////////////////////////////////////////////////////
// Macro Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define AUDIO_AFE_QUEUE_SIZE(ptr)       (((AUDIO_QUEUE_STRU_PTR)ptr)->length)//AUDIO_SINK_DEFAULT_FRAME_SIZE//
#define AUDIO_AFE_QUEUE_DATAPTR(ptr)    (((AUDIO_QUEUE_STRU_PTR)ptr)->data_space)//ptr//

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////
// Type Defintions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
VOID Sink_Audio_Afe_Path_Interface_Init(SINK sink);
VOID Source_Audio_Afe_Path_Interface_Init(SOURCE source);
VOID Source_Audio_Afe_SubPath_Interface_Init(SOURCE source);

U32  SinkSizeAudioAfe(SINK sink);
BOOL SinkWriteBufAudioAfe (SINK sink, U8 *src_addr, U32 length);
BOOL SinkFlushAudioAfe(SINK sink, U32 amount);
BOOL SinkCloseAudioAfe(SINK sink);
BOOL SinkConfigureAudioAfe(SINK sink, stream_config_type type, U32 value);
U32  SinkClaimAudioAfe(SINK sink, U32 extra);
U8*  SinkMapAudioAfe(SINK sink);

U32  SourceSizeAudioAfe(SOURCE source);
BOOL SourceReadBufAudioAfe(SOURCE source, U8* dst_addr, U32 length);
VOID SourceDropAudioAfe(SOURCE source, U32 amount);
BOOL SourceCloseAudioAfe(SOURCE source);
BOOL SourceConfigureAudioAfe(SOURCE source, stream_config_type type, U32 value);
U8*  SourceMapAudioAfe(SOURCE source);

U32  SourceSize_AudioAfePattern_VP(SOURCE source);
BOOL SourceReadBuf_AudioAfePattern_VP(SOURCE source, U8 *dst_addr, U32 length);
VOID SourceDrop_AudioAfePattern_VP(SOURCE source, U32 amount) ;
U8*  SourceMap_AudioAfePattern_VP(SOURCE source);
U32  SourceSize_AudioAfePattern_RT(SOURCE source);
BOOL SourceReadBuf_AudioAfePattern_RT(SOURCE source, U8 *dst_addr, U32 length);
VOID SourceDrop_AudioAfePattern_RT(SOURCE source, U32 amount);
BOOL SourceClose_AudioAfePattern(SOURCE source);


/**
 * @brief Hook the callback function of the Sink_Audio
 * @param SINK instance
 *
 * @return None
 */
VOID Sink_Audio_Afe_Path_Interface_Init(SINK sink)
{
    // TODO:  Replace all callback func.
    /* interface init */
    sink->sif.SinkSlack         = SinkSizeAudioAfe;
    sink->sif.SinkFlush         = SinkFlushAudioAfe;

    sink->sif.SinkConfigure     = SinkConfigureAudioAfe;
    sink->sif.SinkClaim         = SinkClaimAudioAfe;
    sink->sif.SinkMap           = SinkMapAudioAfe;

    #if 0
    if (sink->type == SINK_TYPE_AUDIO) {
        sink->sif.SinkClose         = audio_sink_ops_close; // SinkCloseAudioAfe;        // ready to replace, TODO:
        sink->sif.SinkWriteBuf      = audio_sink_ops_copy;  //SinkWriteBufAudioAfe;
    }
    #ifdef MTK_PROMPT_SOUND_ENABLE
    if (sink->type == SINK_TYPE_VP_AUDIO) {
        sink->sif.SinkClose         = audio_sink_vp_ops_close;
        sink->sif.SinkWriteBuf      = audio_sink_vp_ops_copy;
    }
    #endif
    #else
    sink->sif.SinkClose         = SinkCloseAudioAfe;
    sink->sif.SinkWriteBuf      = (sink_write_buffer_entry)audio_ops_copy;
    #endif
}

/**
 * @brief Hook the callback function of the Source_Audio
 * @param Source instance
 *
 * @return None
 */
VOID Source_Audio_Afe_Path_Interface_Init(SOURCE source)
{
    /* interface init */
    source->sif.SourceSize         = SourceSizeAudioAfe;
    source->sif.SourceDrop         = SourceDropAudioAfe;
    source->sif.SourceConfigure    = SourceConfigureAudioAfe;
    source->sif.SourceClose        = SourceCloseAudioAfe;   //SourceCloseAudio;
    source->sif.SourceReadBuf      = (source_read_buffer_entry)audio_ops_copy; //SourceReadBufAudioAfe
}


VOID Source_Audio_Afe_SubPath_Interface_Init(SOURCE source)
{
    /* interface init */
    if (source->param.VPRT.mode != RT_mode)
    {
        source->sif.SourceConfigure    = NULL;
        source->sif.SourceClose        = SourceClose_AudioAfePattern;
        source->sif.SourceSize         = SourceSize_AudioAfePattern_VP;
        source->sif.SourceDrop         = SourceDrop_AudioAfePattern_VP;
        source->sif.SourceMap          = SourceMap_AudioAfePattern_VP;
        source->sif.SourceReadBuf      = SourceReadBuf_AudioAfePattern_VP;
    }
    else
    {
        source->sif.SourceConfigure    = NULL;
        source->sif.SourceClose        = SourceClose_AudioAfePattern;
        source->sif.SourceSize         = SourceSize_AudioAfePattern_RT;
        source->sif.SourceDrop         = SourceDrop_AudioAfePattern_RT;
        source->sif.SourceMap          = NULL;
        source->sif.SourceReadBuf      = SourceReadBuf_AudioAfePattern_RT;
    }

}

/**
 * @brief Query the available size of the sink buffer
 * @param SINK instance
 *
 * @return The free size of the sink buffer
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 U32 SinkSizeAudioAfe(SINK sink)
{
    U32 writeOffset = sink->streamBuffer.BufferInfo.WriteOffset;
    U32 readOffset  = sink->streamBuffer.BufferInfo.ReadOffset;
    U32 length      = sink->streamBuffer.BufferInfo.length;
    U32 buffer_per_channel_shift = ((sink->param.audio.channel_num>=2) && (sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                                     ? 1
                                     : 0;
    U32 available_length = (writeOffset >= readOffset) ? ((length + readOffset - writeOffset)>>buffer_per_channel_shift) : ((readOffset - writeOffset)>>buffer_per_channel_shift);

    #if defined(MTK_ANC_ENABLE) && defined(MTK_ANC_CHANGE_DL_SRC_RATE)
    U32 dl_rate;
    if ((sink->type == SINK_TYPE_VP_AUDIO) && dsp_anc_get_dl_src_rate(&dl_rate)) {
        Sink_Audio_BufferInfo_Rst(sink, sink->streamBuffer.BufferInfo.ReadOffset);
        return 0;
    }
    #endif

    if (sink->param.audio.AfeBlkControl.u4asrcflag) {
        if (available_length>=4) {
            available_length -= 4;
        }
        available_length &= ~0x00000003;
    }

    if ((sink->streamBuffer.BufferInfo.bBufferIsFull)||(Audio_setting->Audio_sink.Pause_Flag == TRUE))
    {
        return 0;
    }
    else
    {
        return available_length ;
    }
}

/**
 * @brief Query the available size of the sink buffer
 * @param SINK instance
 * @param amount The write offset is upated based on this amount
 *
 * @return The result of the flush operation
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL SinkFlushAudioAfe(SINK sink, U32 amount)
{
	#if 0
    if (amount > 0)
    {
        clk_skew_finish_dl_compensation();
    }
	#endif

    return Sink_Audio_FlushBuffer(sink, amount);
}


BOOL SinkCloseAudioAfe(SINK sink)
{
#if defined(AIR_BLE_SWB_ENABLE) && defined(AIR_FIXED_RATIO_SRC)
    if(sink->type == SINK_TYPE_AUDIO){
        N9Ble_UL_SWB_Sample_Rate_Deinit();
    }
#endif

    audio_ops_close(sink);
    return Sink_Audio_CloseProcedure(sink);
}


BOOL SinkConfigureAudioAfe(SINK sink, stream_config_type type, U32 value)
{
    return Sink_Audio_Configuration(sink, type, value);
//    return SinkAudioAfeConfiguration(sink, type, value);
}


U32 SinkClaimAudioAfe(SINK sink, U32 extra)
{
    if ((sink->param.audio.frame_size == (U16)extra)
      &&(sink->streamBuffer.BufferInfo.bBufferIsFull != TRUE)
      &&(sink->transform == NULL))
    {
        return 0;
    }
    else
    {
        return SINK_INVALID_CLAIM;
    }
}


U8* SinkMapAudioAfe(SINK sink)
{
    return
        (sink->streamBuffer.BufferInfo.startaddr[sink->streamBuffer.BufferInfo.channelSelected]
            + sink->streamBuffer.BufferInfo.WriteOffset);
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_audio_check_sink_remain_enough (SINK sink)
{
    if ((sink) && (sink->type == SINK_TYPE_AUDIO)) {
        uint32_t writeOffset = sink->streamBuffer.BufferInfo.WriteOffset;
        uint32_t readOffset  = sink->streamBuffer.BufferInfo.ReadOffset;
        uint32_t length      = sink->streamBuffer.BufferInfo.length;
        uint32_t buffer_per_channel_shift = ((sink->param.audio.channel_num>=2) && (sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                                              ? 1
                                              : 0;
        uint32_t remain_buffer = (writeOffset >= readOffset) ? ((writeOffset - readOffset)>>buffer_per_channel_shift) : ((length + writeOffset - readOffset)>>buffer_per_channel_shift);
        uint32_t dl_rate;
        if ((sink->streamBuffer.BufferInfo.bBufferIsFull) ||
            (Audio_setting->Audio_sink.Pause_Flag == TRUE))
        {
            return true;
        }
        else
        {
            dl_rate = (sink->param.audio.AfeBlkControl.u4asrcflag) ? sink->param.audio.src_rate : sink->param.audio.rate;
            return (remain_buffer >= ((4*dl_rate*sink->param.audio.period*sink->param.audio.format_bytes)/1000))
                       ? true : false ;
        }
    }
    return false;
}

ATTR_TEXT_IN_IRAM U32 SourceSizeAudioAfe(SOURCE source)
{
    U32 writeOffset = source->streamBuffer.BufferInfo.WriteOffset;
    U32 readOffset  = source->streamBuffer.BufferInfo.ReadOffset;
    U32 length      = source->streamBuffer.BufferInfo.length;
    U16 bytePerSample = source->param.audio.format_bytes;
    U32 data_size;
    U32 buffer_per_channel_shift = ((source->param.audio.channel_num>=2) && (source->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                                     ? 1
                                     : 0;
    U32 audio_frame_size = (source->param.audio.enable_clk_skew == TRUE) ? source->param.audio.frame_size - clk_skew_get_ul_comp_samples()*bytePerSample : source->param.audio.frame_size;
    data_size = (writeOffset >= readOffset) ? (writeOffset - readOffset)>>buffer_per_channel_shift : (length - readOffset + writeOffset)>>buffer_per_channel_shift;
    if (source->param.audio.AfeBlkControl.u4asrcflag) {
        if (data_size>=4) {
            data_size -= 4;
        }
        data_size &= ~0x00000003;
    }
    if ((source->streamBuffer.BufferInfo.bBufferIsFull) ||(data_size >= audio_frame_size )) { //[CHK this part]
#if defined(MTK_GAMING_MODE_HEADSET)
        if((source->transform->sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (source->transform->sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MAX) && (source->transform->sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)) {
            if(data_size >= (audio_frame_size*2)) {
                source->streamBuffer.BufferInfo.ReadOffset += source->param.audio.frame_size;
                source->streamBuffer.BufferInfo.ReadOffset = (source->streamBuffer.BufferInfo.ReadOffset %length);
                DSP_MW_LOG_I("[SourceSizeAudioAfe] fail data_size:%d > audio_frame_size:%d, wo:%d, ro:%d, length:%d", 5, data_size, audio_frame_size, writeOffset, readOffset, length);
            }
        }
        //DSP_MW_LOG_I("[SourceSizeAudioAfe] pass data_size:%d, audio_frame_size:%d, wo:%d, ro:%d, length:%d", 5, data_size, audio_frame_size, writeOffset, readOffset, length);
#endif

        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        /* check if needs to drop redundant data for latency */
        if ((source->param.audio.scenario_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH) && (source->param.audio.scenario_sub_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID))
        {
            if (source->param.audio.drop_redundant_data_at_first_time == true)
            {
                if (data_size > audio_frame_size)
                {
                    source->streamBuffer.BufferInfo.ReadOffset += (data_size - audio_frame_size);
                    source->streamBuffer.BufferInfo.ReadOffset = (source->streamBuffer.BufferInfo.ReadOffset %length);
                }
            }
        }
        #endif /* defined(AIR_ADVANCED_PASSTHROUGH_ENABLE) */

        return (U32)audio_frame_size;
    } else {
        if(source->transform->sink->type == SINK_TYPE_N9SCO) {
            DSP_MW_LOG_I("[SourceSizeAudioAfe] afe no data, audio_frame_size%d=frame_size:%d-clkskewbyte:%d, data_size:%d, wo:%d, ro:%d, length:%d, buffer_per_channel_shift:%d", 8, audio_frame_size, source->param.audio.frame_size, clk_skew_get_ul_comp_bytes(), data_size, writeOffset, readOffset, length, buffer_per_channel_shift);
        }
#if defined(MTK_GAMING_MODE_HEADSET)
        if((source->transform->sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (source->transform->sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MAX) && (source->transform->sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)) {
            DSP_MW_LOG_W("[SourceSizeAudioAfe] afe no data, bBufferIsFull:%d, audio_frame_size%d, data_size:%d, wo:%d, ro:%d, length:%d, buffer_per_channel_shift:%d", 7, source->streamBuffer.BufferInfo.bBufferIsFull, audio_frame_size, data_size, writeOffset, readOffset, length, buffer_per_channel_shift);
        }
#endif
        #ifdef AIR_BT_CODEC_BLE_ENABLED
        if((source->transform != NULL) && (source->transform->sink->type == SINK_TYPE_N9BLE)) {
            DSP_MW_LOG_I("[SourceSizeAudioAfe] AFE no data with N9BLE case, report current data_size %d",1,data_size);
            return data_size;
        }
        #endif
        return 0;
    }
}


BOOL SourceReadBufAudioAfe(SOURCE source, U8* dst_addr, U32 length)
{
    return Source_Audio_ReadAudioBuffer(source, dst_addr, length);
}


ATTR_TEXT_IN_IRAM_LEVEL_2 VOID SourceDropAudioAfe(SOURCE source, U32 amount)
{
    U32 buffer_per_channel_shift = ((source->param.audio.channel_num>=2) && (source->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                                     ? 1
                                     : 0;
    if (amount == 0)
    {
        return;
    }
    else if (source->streamBuffer.BufferInfo.bBufferIsFull == TRUE)
    {
        source->streamBuffer.BufferInfo.bBufferIsFull = FALSE;
    }

    amount = (amount<<buffer_per_channel_shift);

    clk_skew_finish_ul_compensation();

    source->streamBuffer.BufferInfo.ReadOffset
        = (source->streamBuffer.BufferInfo.ReadOffset + amount)
            %(source->streamBuffer.BufferInfo.length);
    if (source->param.audio.AfeBlkControl.u4asrcflag) {
        #ifdef AIR_I2S_SLAVE_ENABLE
        if (source->param.audio.memory == HAL_AUDIO_MEM2) {
            AFE_WRITE(ASM2_CH01_OBUF_RDPNT, source->streamBuffer.BufferInfo.ReadOffset + AFE_READ(ASM2_OBUF_SADR));
        }else if(source->param.audio.memory == HAL_AUDIO_MEM1){
            AFE_WRITE(ASM_CH01_OBUF_RDPNT, source->streamBuffer.BufferInfo.ReadOffset + AFE_READ(ASM_OBUF_SADR));
        }
        #else
        AFE_WRITE(ASM_CH01_OBUF_RDPNT, source->streamBuffer.BufferInfo.ReadOffset + AFE_READ(ASM_OBUF_SADR));
        #endif
    }
    if (source->param.audio.mem_handle.pure_agent_with_src) {
#ifdef AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE
        AFE_WRITE(ASM_CH01_OBUF_RDPNT, source->streamBuffer.BufferInfo.ReadOffset + AFE_READ(ASM_OBUF_SADR));//source start buffer read offset update to HWSRC Out buffer Read point reg
#else
        AFE_WRITE(ASM2_CH01_OBUF_RDPNT, source->streamBuffer.BufferInfo.ReadOffset + AFE_READ(ASM2_OBUF_SADR));//source start buffer read offset update to HWSRC2 Out buffer Read point reg
#endif
    }
#if 0
    if (source->transform->sink->type == SINK_TYPE_N9SCO) {
        DSP_MW_LOG_W("zlx SourceDropAudioAfe, AVM ReadOffset %d, WriteOffset %d, HWSRC in buffer RPTR %d WPTR %d, HWSRC out buffer RPTR %d WPTR %d", 6,
                    source->streamBuffer.BufferInfo.ReadOffset, source->streamBuffer.BufferInfo.WriteOffset,
                    AFE_READ(ASM2_CH01_IBUF_RDPNT) - AFE_READ(ASM2_IBUF_SADR), AFE_READ(ASM2_CH01_IBUF_WRPNT) - AFE_READ(ASM2_IBUF_SADR),
                    AFE_READ(ASM2_CH01_OBUF_RDPNT) - AFE_READ(ASM2_OBUF_SADR), AFE_READ(ASM2_CH01_OBUF_WRPNT) - AFE_READ(ASM2_OBUF_SADR));
    }
#endif
    #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    /* check if needs to clean drop redundant data flag */
    if ((source->param.audio.scenario_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH) && (source->param.audio.scenario_sub_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID))
    {
        if (source->param.audio.drop_redundant_data_at_first_time == true)
        {
            source->param.audio.drop_redundant_data_at_first_time = false;
        }
    }
    #endif /* defined(AIR_ADVANCED_PASSTHROUGH_ENABLE) */
}


BOOL SourceCloseAudioAfe(SOURCE source)
{
    audio_ops_close(source);
    return Source_Audio_CloseProcedure(source);
}


BOOL SourceConfigureAudioAfe(SOURCE source, stream_config_type type, U32 value)
{
    return Source_Audio_Configuration(source, type, value);
}


U8* SourceMapAudioAfe(SOURCE source)
{
    U32 writeOffset = source->streamBuffer.BufferInfo.WriteOffset;
    U32 readOffset  = source->streamBuffer.BufferInfo.ReadOffset;
    U32 length      = source->streamBuffer.BufferInfo.length;

    U32 MoveLength  = writeOffset >= readOffset
                        ? (writeOffset - readOffset)
                            : (length - readOffset + writeOffset);
    #if 0
    U32 residue     = length - readOffset;

    /* source buffer wrap */
    if(MoveLength > residue)
    {
        U32 len1 = residue;
        U32 len2 = MoveLength - len1;
        memcpy(MapAddr,
               source->streamBuffer.BufferInfo.startaddr[source->streamBuffer.BufferInfo.channelSelected]+readOffset,
               len1);

        memcpy(MapAddr+len1,
               source->streamBuffer.BufferInfo.startaddr[source->streamBuffer.BufferInfo.channelSelected],
               len2);
    }
    /* source buffer no wrap */
    else
    {
        memcpy(MapAddr,
               source->streamBuffer.BufferInfo.startaddr[source->streamBuffer.BufferInfo.channelSelected]+readOffset,
               MoveLength);
    }
    #else // Yo
    DSP_C2D_BufferCopy(MapAddr,
                       source->streamBuffer.BufferInfo.startaddr[source->streamBuffer.BufferInfo.channelSelected]+readOffset,
                       MoveLength,
                       source->streamBuffer.BufferInfo.startaddr[source->streamBuffer.BufferInfo.channelSelected],
                       length);
    #endif

    return MapAddr;
}


U32 SourceSize_AudioAfePattern_VP(SOURCE source)
{
    return Source_Audio_GetVoicePromptSize(source);
}


BOOL SourceReadBuf_AudioAfePattern_VP(SOURCE source, U8 *dst_addr, U32 length)
{
    return Source_Audio_ReadVoicePromptBuffer(source, dst_addr, length);
}


VOID SourceDrop_AudioAfePattern_VP(SOURCE source, U32 amount)
{
    Source_Audio_DropVoicePrompt(source, amount);
}


U8* SourceMap_AudioAfePattern_VP(SOURCE source)
{
    U32 readOffset = source->streamBuffer.BufferInfo.ReadOffset;
    U32 remainlength = SourceSize_AudioPattern_VP(source);

    if (remainlength)
    {
        memcpy(MapAddr,source->streamBuffer.BufferInfo.startaddr[0]+readOffset,remainlength);
    }
    return MapAddr;
}


U32 SourceSize_AudioAfePattern_RT(SOURCE source)
{
    return Source_Audio_GetRingtoneSize(source);
}


BOOL SourceReadBuf_AudioAfePattern_RT(SOURCE source, U8 *dst_addr, U32 length)
{
    return Source_Audio_ReadRingtoneBuf(source, dst_addr, length);
}


VOID SourceDrop_AudioAfePattern_RT(SOURCE source, U32 amount)
{
    Source_Audio_DropRingtone(source, amount);
}


BOOL SourceClose_AudioAfePattern(SOURCE source)
{
    if (source)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL SinkFlushVirtualSinkAfe(SINK sink ,U32 amount)
{

    if (sink->param.virtual_para.entry != NULL)
    {
        sink->param.virtual_para.entry((VOID*)sink->streamBuffer.BufferInfo.startaddr[0], amount);
    }
    if (sink->param.virtual_para.handler != NULL)
    {
        //MSG_MessageSend(sink->param.virtual_para.handler, MESSAGE_SINK_NEW_DATA_IN, 0);
//        #warning "155x not support messagesend"
    }
    return TRUE;

}

BOOL SinkCloseVirtualSinkAfe(SINK sink)
{
    DSPMEM_Free(sink->taskid, sink);
    return TRUE;
}

U32 SinkSlackVirtualSinkAfe(SINK sink)
{
    return sink->param.virtual_para.mem_size;
}

U32 SinkClaimVirtualSinkAfe(SINK sink, U32 len)
{
    return (len>sink->param.virtual_para.mem_size)
               ? 0xffffffff
               : 0;
}

U8 *SinkMapVirtualSinkAfe(SINK sink)
{
    return (U8 *)sink->streamBuffer.BufferInfo.startaddr[0];
}

BOOL SinkWriteBufVirtualSinkAfe (SINK sink, U8 *src_addr, U32 length)
{
    TRANSFORM transform = sink->transform;
    DSP_CALLBACK_PTR callback_ptr;

    if (transform!=NULL && src_addr==NULL)
    {
        callback_ptr = DSP_Callback_Get(transform->source, sink);
        src_addr = callback_ptr->EntryPara.out_ptr[0];
    }

    if (length<=sink->param.virtual_para.mem_size)
    {
        memcpy(sink->streamBuffer.BufferInfo.startaddr[0],
               src_addr,
               length);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

BOOL SinkConfigureVirtualSinkAfe(SINK sink, stream_config_type type, U32 value)
{
    return Sink_VirtualSink_Configuration(sink, type, value);
}


VOID Sink_VirtualSink_Afe_Interface_Init(SINK sink)
{
    /* interface init */
    sink->sif.SinkSlack         = SinkSlackVirtualSinkAfe;
    sink->sif.SinkFlush         = SinkFlushVirtualSinkAfe;
    sink->sif.SinkConfigure     = SinkConfigureVirtualSinkAfe;
    sink->sif.SinkClose         = SinkCloseVirtualSinkAfe;
    sink->sif.SinkClaim         = SinkClaimVirtualSinkAfe;
    sink->sif.SinkMap           = SinkMapVirtualSinkAfe;
    sink->sif.SinkWriteBuf      = SinkWriteBufVirtualSinkAfe;
}

