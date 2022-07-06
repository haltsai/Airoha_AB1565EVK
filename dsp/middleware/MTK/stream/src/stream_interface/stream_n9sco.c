/* Copyright Statement:
 *
 * (C) 2014  Airoha Technology Corp. All rights reserved.
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
#include "dsp_memory.h"
#include "stream_audio.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "transform.h"
#include "stream_n9sco.h"
#include "dsp_audio_msg.h"
#include "voice_plc_interface.h"
//-drivers
#include "audio_config.h"
#include "dsp_audio_process.h"
#include "dsp_memory.h"
#include "dsp_share_memory.h"
#include "dsp_temp.h"

U8* tempptr[6];
U32 tempnum[6];
typedef BOOL (*SCOHANDLER)(SOURCE source,SINK sink);

// #include "Drv_gpio.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




#define BTCLK_LEN 4
#define STATE_LEN 4
#define VOICE_HEADER (BTCLK_LEN + STATE_LEN)
#define ESCO_UL_ERROR_DETECT_THD (8)

////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

Stream_n9sco_Config_Ptr N9SCO_setting;
U16 escoseqn;
////////////////////////////////////////////////////////////////////////////////
// Type Defintions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EXTERN VOID StreamDSP_HWSemaphoreTake(VOID);
EXTERN VOID StreamDSP_HWSemaphoreGive(VOID);

ATTR_TEXT_IN_IRAM VOID N9ScoRx_update_from_share_information(SOURCE source)
{
    StreamDSP_HWSemaphoreTake();
    memcpy(&(source->streamBuffer.ShareBufferInfo), source->param.n9sco.share_info_base_addr, 32);/* share info fix 32 byte */
    source->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(source->streamBuffer.ShareBufferInfo.startaddr);
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9Sco_update_readoffset_share_information( SOURCE source,U32 ReadOffset)
{
    StreamDSP_HWSemaphoreTake();
    source->param.n9sco.share_info_base_addr->ReadOffset = ReadOffset;
    source->param.n9sco.share_info_base_addr->bBufferIsFull = FALSE;
    StreamDSP_HWSemaphoreGive();
}
ATTR_TEXT_IN_IRAM static VOID N9ScoTx_update_from_share_information(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    memcpy(&(sink->streamBuffer.ShareBufferInfo), sink->param.n9sco.share_info_base_addr, 32);/* share info fix 32 byte */
    sink->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.ShareBufferInfo.startaddr);
    StreamDSP_HWSemaphoreGive();
}


ATTR_TEXT_IN_IRAM_LEVEL_2 static VOID N9Sco_update_writeoffset_share_information(SINK sink,U32 WriteOffset)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.n9sco.share_info_base_addr->WriteOffset = WriteOffset;
    if (WriteOffset == sink->param.n9sco.share_info_base_addr->ReadOffset)
    {
        sink->param.n9sco.share_info_base_addr->bBufferIsFull = 1;
    }
    StreamDSP_HWSemaphoreGive();
}
static VOID N9Sco_Reset_Sinkoffset_share_information(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.n9sco.share_info_base_addr->WriteOffset = 0;
    sink->param.n9sco.share_info_base_addr->ReadOffset = 0;
    sink->param.n9sco.share_info_base_addr->length = N9SCO_setting->N9Sco_source.Buffer_Frame_Num*N9SCO_setting->N9Sco_source.Frame_Size;
    sink->param.n9sco.share_info_base_addr->bBufferIsFull = FALSE;
    StreamDSP_HWSemaphoreGive();
}


VOID N9SCO_Default_setting_init(VOID)
{
       if (N9SCO_setting != NULL)
       {return;}
       N9SCO_setting = pvPortMalloc(sizeof(Stream_n9sco_Config_t));//for rtos
       // N9SCO_setting = OSHEAP_malloc(sizeof(Stream_n9sco_Config_Ptr));
       
       memset(N9SCO_setting,0,sizeof(Stream_n9sco_Config_t));
       N9SCO_setting->N9Sco_source.Buffer_Frame_Num        = 4; // Should be setup in CCNI
       N9SCO_setting->N9Sco_source.Process_Frame_Num       = 2; // Should be setup in CCNI
       N9SCO_setting->N9Sco_source.Frame_Size              = 60 + VOICE_HEADER;// Should be setup in CCNI
       N9SCO_setting->N9Sco_source.Input_sample_rate       = 16000; 

       N9SCO_setting->N9Sco_sink.Buffer_Frame_Num          = 4; // Should be setup in CCNI
       N9SCO_setting->N9Sco_sink.Process_Frame_Num         = 2; // Should be setup in CCNI
       N9SCO_setting->N9Sco_sink.Frame_Size                = 60 + VOICE_HEADER;// Should be setup in CCNI
       N9SCO_setting->N9Sco_sink.Output_sample_rate        = 16000;
       N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt        = 0;
}




/**
 * SinkSlackSco
 *
 * Function to know the remain buffer size of SCO sink.
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 U32 SinkSlackN9Sco(SINK sink)
{
    N9ScoTx_update_from_share_information(sink);
    U32 writeOffset = sink->streamBuffer.ShareBufferInfo.WriteOffset;
    U32 readOffset  = sink->streamBuffer.ShareBufferInfo.ReadOffset;
    U32 length      = sink->streamBuffer.ShareBufferInfo.length;
    U32 ProcessFrameLen = (N9SCO_setting->N9Sco_source.Process_Frame_Num)*N9SCO_setting->N9Sco_source.Frame_Size;
    U32 RemainBuf = (readOffset >= writeOffset) ?(length + writeOffset - readOffset) : (readOffset - writeOffset - readOffset);
    //printf("SinkSlackN9Sco process_data_length : %d\r\n", sink->param.n9sco.process_data_length);
    if ((sink->streamBuffer.ShareBufferInfo.bBufferIsFull != 1)&&(RemainBuf >= ProcessFrameLen))
    {
        return sink->param.n9sco.process_data_length;
    }
    else
    {
        return sink->param.n9sco.process_data_length;
    }
}


/**
 * SinkClaimSco
 *
 * Function to ask the buffer to write data into SCO sink.
 *
 */
U32 SinkClaimN9Sco(SINK sink, U32 extra)
{
    N9ScoTx_update_from_share_information(sink);
    U32 writeOffset = sink->streamBuffer.ShareBufferInfo.WriteOffset;
    U32 readOffset  = sink->streamBuffer.ShareBufferInfo.ReadOffset;
    U32 length      = sink->streamBuffer.ShareBufferInfo.length;
    U32 RemainBuf = (readOffset >= writeOffset) ? (length - writeOffset + readOffset) : (writeOffset - readOffset);
    if((extra != 0)&&((sink->streamBuffer.ShareBufferInfo.bBufferIsFull != 1)&&(RemainBuf > extra)) && (sink->transform == NULL))
    {
        return 0;
    }
    else
    {
        return SINK_INVALID_CLAIM;
    }
}

/**
 * SinkMapSco
 *
 * Function to read the decoded data in SCO sink.
 *
 */
U8* SinkMapN9Sco(SINK sink)
{
    N9ScoTx_update_from_share_information(sink);
    //memcpy(MapAddr,sink->streamBuffer.ShareBufferInfo.startaddr + sink->streamBuffer.ShareBufferInfo.ReadOffset, sink->streamBuffer.ShareBufferInfo.length - sink->streamBuffer.ShareBufferInfo.ReadOffset);
    if (sink->streamBuffer.ShareBufferInfo.ReadOffset != 0)
    {
        memcpy(MapAddr + sink->streamBuffer.ShareBufferInfo.ReadOffset,&(sink->streamBuffer.ShareBufferInfo.startaddr), sink->streamBuffer.ShareBufferInfo.ReadOffset);    
    }

    return MapAddr;

}

/**
 * SinkFlushSco
 *
 * Function to read the decoded data in SCO sink.
 *
 * param :amount - The amount of data written into sink.
 *
*/
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL SinkFlushN9Sco(SINK sink,U32 amount)
{
    N9ScoTx_update_from_share_information(sink);
    if ((SinkSlackN9Sco(sink) == 0)||(amount != sink->param.n9sco.process_data_length))
    {
        return FALSE;
    }
    else
    {
        sink->streamBuffer.ShareBufferInfo.WriteOffset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + N9SCO_setting->N9Sco_sink.Process_Frame_Num*N9SCO_setting->N9Sco_sink.Frame_Size)%(sink->streamBuffer.ShareBufferInfo.length);
    }
    #if !DL_TRIGGER_UL
    if (sink->param.n9sco.IsFirstIRQ == TRUE)
    {
        U32 gpt_timer,relative_delay;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
        relative_delay = (gpt_timer - sink->param.n9sco.ul_play_gpt)%15000;
        /* send CCNI data transmit to N9 */
        if ((relative_delay < 8*1000)||(relative_delay > 12*1000)) //check first flush in safe zone 
        {
            U32 delay_time = (relative_delay < 8*1000) ? (8*1000 - relative_delay) : ((15+8)*1000 - relative_delay);
            hal_gpt_delay_us(delay_time);
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
            DSP_MW_LOG_I("eSCO UL flush abnormal, delay :%d GTP_N :%d, GPT_P :%d\r\n", 3, delay_time,gpt_timer,sink->param.n9sco.ul_play_gpt);
        }
        else
        {
            DSP_MW_LOG_I("eSCO UL flush time GTP_N :%d, GPT_P :%d\r\n", 2, gpt_timer,sink->param.n9sco.ul_play_gpt);
        }
        hal_ccni_message_t msg;
        memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
        msg.ccni_message[0] = MSG_DSP2N9_UL_START << 16;
        aud_msg_tx_handler(msg, 0, FALSE);
        sink->param.n9sco.IsFirstIRQ = FALSE;
    }
    #else
    if (((sink->streamBuffer.ShareBufferInfo.ReadOffset + N9SCO_setting->N9Sco_sink.Process_Frame_Num*N9SCO_setting->N9Sco_sink.Frame_Size)%sink->streamBuffer.ShareBufferInfo.length) == sink->streamBuffer.ShareBufferInfo.WriteOffset)
    {
        //DSP_MW_LOG_I("eSCO UL Ro abnormal, cnt: %d %d %d",3,N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt,sink->streamBuffer.ShareBufferInfo.WriteOffset,sink->streamBuffer.ShareBufferInfo.ReadOffset);
        if(++N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt >= ESCO_UL_ERROR_DETECT_THD)
        {
            DSP_MW_LOG_I("eSCO UL trigger ro/wo error handle cnt:%d",1,N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt);
            sink->streamBuffer.ShareBufferInfo.WriteOffset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + N9SCO_setting->N9Sco_sink.Frame_Size)%(sink->streamBuffer.ShareBufferInfo.length);
        }
    }
    else
    {
        N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt = 0;
    }
    #endif
    N9Sco_update_writeoffset_share_information(sink,sink->streamBuffer.ShareBufferInfo.WriteOffset);
    return TRUE;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL SinkBufferWriteN9Sco (SINK sink, U8 *src_addr, U32 length)
{
    U16 i;
    U8* write_ptr;
    N9ScoTx_update_from_share_information(sink);
    if (sink->param.n9sco.process_data_length != length)
    {
        return FALSE;
    }
    


    for (i = 0 ; i < N9SCO_setting->N9Sco_sink.Process_Frame_Num ; i++)
    {
        write_ptr = (U8*)(sink->streamBuffer.ShareBufferInfo.startaddr + sink->streamBuffer.ShareBufferInfo.WriteOffset);
        memcpy(write_ptr + BTCLK_LEN, src_addr + (U32)(i*(N9SCO_setting->N9Sco_sink.Frame_Size - VOICE_HEADER)) , N9SCO_setting->N9Sco_sink.Frame_Size - VOICE_HEADER );
        sink->streamBuffer.ShareBufferInfo.WriteOffset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + N9SCO_setting->N9Sco_sink.Frame_Size)%(sink->streamBuffer.ShareBufferInfo.length);
    }
    return TRUE;

}



/**
 * Sink_Sco_Buffer_Ctrl
 *
 * Function to enable/disable SCO buffer.
 *
 * param :ctrl - enable/disable SCO buffer.
 *
 */
VOID Sink_N9Sco_Buffer_Init(SINK sink)
{
    N9ScoTx_update_from_share_information(sink);
    N9Sco_Reset_Sinkoffset_share_information(sink);
	sink->streamBuffer.ShareBufferInfo.WriteOffset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + N9SCO_setting->N9Sco_sink.Frame_Size)%(sink->streamBuffer.ShareBufferInfo.length);
}


/**
 * SinkCloseSco
 *
 * Function to shutdown SCO sink.
 *
 */
BOOL SinkCloseN9Sco(SINK sink)
{
    sink->param.n9sco.process_data_length = 0;
    return TRUE;
}



/**
 * SinkInitSco
 *
 * Function to initialize SCO sink.
 *
 */
VOID SinkInitN9Sco(SINK sink)
{
    /* buffer init */
    N9SCO_Default_setting_init();
    sink->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
       sink->param.n9sco.process_data_length = N9SCO_setting->N9Sco_sink.Process_Frame_Num*(N9SCO_setting->N9Sco_sink.Frame_Size - VOICE_HEADER);
    DSP_MW_LOG_I("eSCO UL process_data_length : %d\r\n", 1, sink->param.n9sco.process_data_length);

    Sink_N9Sco_Buffer_Init(sink);


    /* interface init */
    sink->sif.SinkSlack       = SinkSlackN9Sco;
    sink->sif.SinkClaim       = SinkClaimN9Sco;
    sink->sif.SinkMap         = SinkMapN9Sco;
    sink->sif.SinkFlush       = SinkFlushN9Sco;
    sink->sif.SinkClose       = SinkCloseN9Sco;
    sink->sif.SinkWriteBuf    = SinkBufferWriteN9Sco;

    sink->param.n9sco.IsFirstIRQ = TRUE;
    escoseqn = 0;

}






/**
 * SourceSizeSco
 *
 * Function to report remaining Source buffer size.
 *
 */
ATTR_TEXT_IN_IRAM U32 SourceSizeN9Sco(SOURCE source)
{
    N9ScoRx_update_from_share_information(source);
    U32 writeOffset = source->streamBuffer.ShareBufferInfo.WriteOffset;
    U32 readOffset  = source->streamBuffer.ShareBufferInfo.ReadOffset;
    U32 length      = source->streamBuffer.ShareBufferInfo.length;
    U32 ProcessFrameLen = (N9SCO_setting->N9Sco_source.Process_Frame_Num)*N9SCO_setting->N9Sco_source.Frame_Size;
    U32 RemainLen = (readOffset > writeOffset) ? (length - readOffset + writeOffset) : (writeOffset - readOffset);
    if (((source->streamBuffer.ShareBufferInfo.bBufferIsFull)&&(ProcessFrameLen <= length))||
        (RemainLen >= ProcessFrameLen)||(source->param.n9sco.write_offset_advance != 0))
    {
        return source->param.n9sco.process_data_length;
    }
    else
    {
        //return (U32)0;
        return 0;
    }
}


/**
 * SourceMapSco
 *
 * Function to  read the received data in SCO source.
 *
 */
U8* SourceMapN9Sco(SOURCE source)
{
    N9ScoRx_update_from_share_information(source);
    //memcpy(MapAddr,source->streamBuffer.ShareBufferInfo.startaddr + source->streamBuffer.ShareBufferInfo.ReadOffset, source->streamBuffer.ShareBufferInfo.length - source->streamBuffer.ShareBufferInfo.ReadOffset);
    if (source->streamBuffer.ShareBufferInfo.ReadOffset != 0)
    {
        memcpy(MapAddr + source->streamBuffer.ShareBufferInfo.ReadOffset,&(source->streamBuffer.ShareBufferInfo.startaddr), source->streamBuffer.ShareBufferInfo.ReadOffset);    
    }

    return MapAddr;
}

/**
 * SourceDropSco
 *
 * Function to drop the data in SCO sink.
 *
 * param :amount - The amount of data to drop in sink.
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 VOID SourceDropN9Sco(SOURCE source, U32 amount)
{
    U16 i;
    U8* write_ptr;
    N9ScoRx_update_from_share_information(source);
    if (amount != source->param.n9sco.process_data_length)
    {
        return;
    }
    else
    {

        for (i = 0 ; i < N9SCO_setting->N9Sco_source.Process_Frame_Num ; i++)
        {
            write_ptr = (U8 *)(source->streamBuffer.ShareBufferInfo.startaddr + (source->streamBuffer.ShareBufferInfo.ReadOffset + i*N9SCO_setting->N9Sco_source.Frame_Size)%source->streamBuffer.ShareBufferInfo.length);
            *(write_ptr + N9SCO_setting->N9Sco_source.Frame_Size - STATE_LEN) = SCO_PKT_FREE;
        }
        source->streamBuffer.ShareBufferInfo.ReadOffset = (source->streamBuffer.ShareBufferInfo.ReadOffset + N9SCO_setting->N9Sco_source.Process_Frame_Num*N9SCO_setting->N9Sco_source.Frame_Size)%(source->streamBuffer.ShareBufferInfo.length);
        N9Sco_update_readoffset_share_information(source,source->streamBuffer.ShareBufferInfo.ReadOffset);
    }
    source->param.n9sco.write_offset_advance = 0;
    #if DL_TRIGGER_UL
    if  (((Sink_blks[SINK_TYPE_N9SCO] != NULL) && (Sink_blks[SINK_TYPE_N9SCO]->param.n9sco.IsFirstIRQ == TRUE))&& ((source->transform != NULL)&&(source->param.n9sco.dl_enable_ul == FALSE)))
    {
        volatile SINK eSCO_sink = Sink_blks[SINK_TYPE_N9SCO];
        U32 relative_delay,delay_thd;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(eSCO_sink->param.n9sco.ul_play_gpt));
        relative_delay = ((eSCO_sink->param.n9sco.ul_play_gpt - source->param.n9sco.ul_play_gpt)*1000)>>5;
        delay_thd = ((6 + ((relative_delay>>10)/(15*4)))%15); //extend THD every 15*4 ms
        if ((((relative_delay>>10)%15) < delay_thd)&&(eSCO_sink->transform != NULL))
        {
            eSCO_sink->param.n9sco.IsFirstIRQ = FALSE;
            DSP_MW_LOG_I("eSCO UL sync dl, DL time:%d thd :%d GTP_N :%d, GPT_P :%d\r\n",4, relative_delay, delay_thd,(eSCO_sink->param.n9sco.ul_play_gpt),source->param.n9sco.ul_play_gpt); 
            hal_gpt_delay_us(delay_thd*1000 - ((relative_delay*1000>>10)%15000));
            afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_VUL1, true, true);
            afe_enable_audio_irq(afe_irq_request_number(AUDIO_DIGITAL_BLOCK_MEM_VUL1), eSCO_sink->transform->source->param.audio.rate, eSCO_sink->transform->source->param.audio.count);
            if (eSCO_sink->transform->source->param.audio.echo_reference) {
                afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_AWB, true, true);
            }
            if(eSCO_sink->transform->source->param.audio.channel_num >= 3) {
                afe_set_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_VUL2, true, true);
            }
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(eSCO_sink->param.n9sco.ul_play_gpt));
            DSP_MW_LOG_I("eSCO UL start from DL drop, delay :%d GTP_N :%d",2, (delay_thd*1000 - relative_delay),eSCO_sink->param.n9sco.ul_play_gpt); 
        }
        else
        {
            N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt = 0;
            SinkFlushN9Sco(eSCO_sink,eSCO_sink->param.n9sco.process_data_length);
            DSP_MW_LOG_I("eSCO UL start from DL drop too late, delay :%d GTP_N :%d, GPT_P :%d\r\n",3, relative_delay,(eSCO_sink->param.n9sco.ul_play_gpt),source->param.n9sco.ul_play_gpt); 
        }
    }
    #endif
    escoseqn += 2;
}

/**
 * SourceConfigureSco
 *
 * Function to configure SCO source.
 *
 * param :type - The configure type.
 *
 * param :value - The configure value.
 *
 */
BOOL SourceConfigureN9Sco(SOURCE source, stream_config_type type, U32 value)
{

    switch (type)
    {
        case SCO_SOURCE_WO_ADVANCE:
            source->param.n9sco.write_offset_advance = value;
            break;
        default:
            //printf("Wrong configure type");
            return FALSE;
            break;
    }

    return TRUE;
}


/**
 * SourceReadBufSco
 *
 * Function to read data from SCO source.
 *
 * param :dst_addr - The destination buffer to write data into.
 *
 * param :length -The leng of data to read.
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL SourceReadBufN9Sco(SOURCE source, U8* dst_addr, U32 length)
{
    U16 i;
    U8* write_ptr;
    N9ScoRx_update_from_share_information(source);
    if (source->param.n9sco.process_data_length != length)
    {
        return FALSE;
    }
    //VOICE_RX_INBAND_INFO_t RxPacketInfo = {0};
    VOICE_RX_INBAND_INFO_t RxPacketInfo;
    RxPacketInfo.RxEd = 0;

    for (i = 0 ; i < N9SCO_setting->N9Sco_source.Process_Frame_Num ; i++)
    {
        write_ptr = (U8 *)(source->streamBuffer.ShareBufferInfo.startaddr + source->streamBuffer.ShareBufferInfo.ReadOffset);

        if (*(U8*)(write_ptr + N9SCO_setting->N9Sco_source.Frame_Size - STATE_LEN) == SCO_PKT_USED) // eSCO packet state 0: Free, 1: Used, 2: Lost
        {
            RxPacketInfo.RxEd = TRUE;
            memcpy(dst_addr + i*(N9SCO_setting->N9Sco_source.Frame_Size - VOICE_HEADER),write_ptr + BTCLK_LEN, N9SCO_setting->N9Sco_source.Frame_Size - VOICE_HEADER );
        }
        else
        {
            RxPacketInfo.RxEd = FALSE;
            DSP_MW_LOG_I("meet packet expired %d", 1, escoseqn + i);
            Voice_PLC_CheckAndFillZeroResponse((S16 *)(dst_addr + i*(N9SCO_setting->N9Sco_source.Frame_Size - VOICE_HEADER)),gDspAlgParameter.EscoMode.Rx);
        }
        Voice_PLC_UpdateInbandInfo(&RxPacketInfo,sizeof(VOICE_RX_INBAND_INFO_t),i);
        source->streamBuffer.ShareBufferInfo.ReadOffset = (source->streamBuffer.ShareBufferInfo.ReadOffset + N9SCO_setting->N9Sco_source.Frame_Size)%source->streamBuffer.ShareBufferInfo.length;
    }
    //N9Sco_update_readoffset_share_information(source,source->streamBuffer.ShareBufferInfo.ReadOffset);
    return TRUE;
}

/**
 * SourceCloseSco
 *
 * Function to shutdown SCO source.
 *
 */
BOOL SourceCloseN9Sco(SOURCE source)
{
    source->param.n9sco.process_data_length = 0;
    return TRUE;
}

/**
 * SourceInitSco
 *
 * Function to initialize SCO source.
 *
 */
VOID SourceInitN9Sco(SOURCE source)
{
    /* buffer init */
    N9SCO_Default_setting_init();
    source->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    Source_N9Sco_Buffer_Init(source);
    
    source->param.n9sco.process_data_length = N9SCO_setting->N9Sco_source.Process_Frame_Num*(N9SCO_setting->N9Sco_source.Frame_Size - VOICE_HEADER);
    

    /* interface init */
    source->sif.SourceSize        = SourceSizeN9Sco;
    source->sif.SourceReadBuf    = SourceReadBufN9Sco;
    source->sif.SourceMap        = SourceMapN9Sco;
    source->sif.SourceConfigure    = SourceConfigureN9Sco;
    source->sif.SourceDrop        = SourceDropN9Sco;
    source->sif.SourceClose        = SourceCloseN9Sco;
    
    /* Enable Interrupt */
    source->param.n9sco.IsFirstIRQ = TRUE;
    source->param.n9sco.dl_enable_ul = TRUE;
    source->param.n9sco.write_offset_advance = 0;
}


/**
 * Source_Sco_Buffer_Ctrl
 *
 * Function to enable/disable SCO buffer.
 *
 * param :ctrl - enable/disable SCO buffer.
 *
 */
VOID Source_N9Sco_Buffer_Init(SOURCE source)
{    
    N9ScoRx_update_from_share_information(source);
    N9Sco_update_readoffset_share_information(source,0);
}





