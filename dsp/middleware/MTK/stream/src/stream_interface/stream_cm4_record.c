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
#include "types.h"
#include "source_inter.h"
#include "dsp_buffer.h"
#include "dsp_memory.h"
#include "stream_cm4_record.h"
#include "dsp_callback.h"
#include "dsp_temp.h"
#include "dsp_dump.h"
#include "dsp_audio_process.h"

////////////////////////////////////////////////////////////////////////////////
// Constant Definitions ////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////




////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
static cm4_record_pcm_ctrl_blk_t CM4_RecordCtrl;
bool CM4_Record_air_dump = false;
U8   CM4_Record_air_dump_scenario = 0;
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
bool CM4_Record_leakage_enable = false;
#endif
bool CM4_Record_flag = false;


////////////////////////////////////////////////////////////////////////////////
// Type Defintions /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////
// Function Prototypes /////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
EXTERN VOID StreamDSP_HWSemaphoreTake(VOID);
EXTERN VOID StreamDSP_HWSemaphoreGive(VOID);

////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
VOID Cm4Record_update_from_share_information(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    memcpy(&(sink->streamBuffer.ShareBufferInfo), sink->param.cm4_record.share_info_base_addr, 32);/* share info fix 32 byte */
    sink->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.ShareBufferInfo.startaddr);
    StreamDSP_HWSemaphoreGive();
}

static VOID Cm4Record_send_data_ready(VOID)
{
    hal_ccni_message_t msg;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = MSG_DSP2MCU_RECORD_DATA_NOTIFY << 16;
    aud_msg_tx_handler(msg, 0, FALSE);
}


static VOID Cm4Record_update_writeoffset_share_information(SINK sink,U32 WriteOffset)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.cm4_record.share_info_base_addr->WriteOffset = WriteOffset;
    if (WriteOffset == sink->param.cm4_record.share_info_base_addr->ReadOffset)
    {
        sink->param.cm4_record.share_info_base_addr->bBufferIsFull = 1;
    }
    StreamDSP_HWSemaphoreGive();
}
VOID Cm4Record_Reset_Sinkoffset_share_information(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.cm4_record.share_info_base_addr->WriteOffset = 0;
    sink->param.cm4_record.share_info_base_addr->ReadOffset = 0;
    sink->param.cm4_record.share_info_base_addr->bBufferIsFull = false;
    StreamDSP_HWSemaphoreGive();
}


VOID Cm4Record_Default_setting_init(VOID)
{

}



/**
 * SinkSlackCm4Record
 *
 * Function to know the remain buffer size of Cm4 Record sink.
 *
 *
 */
U32 SinkSlackCm4Record(SINK sink)
{
    Cm4Record_update_from_share_information(sink);
    U32 writeOffset = sink->streamBuffer.ShareBufferInfo.WriteOffset;
    U32 readOffset  = sink->streamBuffer.ShareBufferInfo.ReadOffset;
    U32 length      = sink->streamBuffer.ShareBufferInfo.length;
    U32 RemainBuf = (readOffset > writeOffset) ? (readOffset - writeOffset) : (length - (writeOffset - readOffset));
    U32 buffer_per_channel_shift = (( sink->param.audio.channel_num >= 2 ) && ( sink->param.cm4_record.interleave == true ))
                                     ? 1
                                     : 0;
    /*debug*/
    //DSP_MW_LOG_I("[Cm4 record] Sink_Slack RemainBuf(%d) bBufferIsFull(%d)", 2, RemainBuf, sink->streamBuffer.ShareBufferInfo.bBufferIsFull);
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    extern bool FIR_report_flag;
    extern bool cmp_filter_flag;
    extern bool cmp_filter_report_flag;
    if (FIR_report_flag && (!cmp_filter_flag) && (!cmp_filter_report_flag)) {
        //printf("[user_trigger_ff]SinkSlack,  => return 0");
        return 0;
    }
    if (FIR_report_flag && (cmp_filter_flag) && (cmp_filter_report_flag)) {
        //printf("[user_trigger_ff]SinkSlack,  => return 0");
        return 0;
    }
#endif
#endif
    if (sink->streamBuffer.ShareBufferInfo.bBufferIsFull == true)
    {
        return 0;
    }
    else
    {
        return (RemainBuf>>buffer_per_channel_shift);
    }
}


/**
 * SinkClaimCm4Record
 *
 * Function to ask the buffer to write data into Cm4 Record  sink.
 *
 *
 */
U32 SinkClaimCm4Record(SINK sink, U32 extra)
{
    Cm4Record_update_from_share_information(sink);
    U32 writeOffset = sink->streamBuffer.ShareBufferInfo.WriteOffset;
    U32 readOffset  = sink->streamBuffer.ShareBufferInfo.ReadOffset;
    U32 length      = sink->streamBuffer.ShareBufferInfo.length;
    U32 RemainBuf = (readOffset >= writeOffset) ? (length - writeOffset + readOffset) : (writeOffset - readOffset);
    /*debug*/
    //DSP_MW_LOG_I("[Cm4 record] Sink_Claim RemainBuf(%d) bBufferIsFull(%d) extra(%d)", 3, RemainBuf, sink->streamBuffer.ShareBufferInfo.bBufferIsFull, extra);
    if(((sink->streamBuffer.ShareBufferInfo.bBufferIsFull != true) && (RemainBuf >= extra)) && (sink->transform == NULL))
    {
        return sink->streamBuffer.ShareBufferInfo.startaddr + writeOffset;
    }
    else
    {
        return SINK_INVALID_CLAIM;
    }
}

/**
 * SinkMapCm4Record
 *
 * Function to read the data in Cm4 Record sink.
 *
 *
 */
U8* SinkMapCm4Record(SINK sink)
{
    Cm4Record_update_from_share_information(sink);
    memcpy(MapAddr,
           (void *)sink->streamBuffer.ShareBufferInfo.startaddr + sink->streamBuffer.ShareBufferInfo.ReadOffset,
           sink->streamBuffer.ShareBufferInfo.length - sink->streamBuffer.ShareBufferInfo.ReadOffset);
    if (sink->streamBuffer.ShareBufferInfo.ReadOffset != 0)
    {
        memcpy(MapAddr + sink->streamBuffer.ShareBufferInfo.length - sink->streamBuffer.ShareBufferInfo.ReadOffset,
               (void *)sink->streamBuffer.ShareBufferInfo.startaddr,
               sink->streamBuffer.ShareBufferInfo.ReadOffset);
    }
    return NULL;
}

/**
 * SinkFlushCm4Record
 *
 * Function to read the decoded data in Cm4 Record sink.
 *
 * param :amount - The amount of data written into sink.
 *
 *
*/
BOOL SinkFlushCm4Record(SINK sink,U32 amount)
{
    Cm4Record_update_from_share_information(sink);
    U32 buffer_per_channel_shift = (( sink->param.audio.channel_num >= 2 ) && ( sink->param.cm4_record.interleave == true ))
                                     ? 1
                                     : 0;

#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    extern bool FIR_report_flag;
    extern bool cmp_filter_flag;
    extern bool cmp_filter_report_flag;
    if(FIR_report_flag & (!(cmp_filter_flag ^ cmp_filter_report_flag))) {
        sink->streamBuffer.ShareBufferInfo.WriteOffset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + amount)%(sink->streamBuffer.ShareBufferInfo.length);
        Cm4Record_update_writeoffset_share_information(sink,sink->streamBuffer.ShareBufferInfo.WriteOffset);

    } else {
#endif
#endif
		if (SinkSlackCm4Record(sink) == 0)
	    {
	        Cm4Record_send_data_ready();

	        return FALSE;
	    }
	    else
	    {
	        sink->streamBuffer.ShareBufferInfo.WriteOffset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + (amount<<buffer_per_channel_shift))%(sink->streamBuffer.ShareBufferInfo.length);
	    }
	    Cm4Record_update_writeoffset_share_information(sink,sink->streamBuffer.ShareBufferInfo.WriteOffset);

	    CM4_RecordCtrl.frame_indx++;

	    if (CM4_RecordCtrl.frame_indx == CM4_RecordCtrl.frames_per_message)
	    {
	        CM4_RecordCtrl.frame_indx = 0;

	        Cm4Record_send_data_ready();
	    }
#ifdef MTK_USER_TRIGGER_FF_ENABLE
#ifndef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    }
#endif
#endif

    return TRUE;
}

BOOL SinkBufferWriteCm4Record (SINK sink, U8 *src_addr, U32 length)
{
    U16 i;
    U8* write_ptr;
    U8* write_ptr_2;
    U8* src_addr_2 = NULL;
    U32 copy_length = length;
    TRANSFORM transform = sink->transform;
    DSP_CALLBACK_PTR callback_ptr = NULL;
    if(transform != NULL){
        callback_ptr = DSP_Callback_Get(transform->source, sink);
        if(callback_ptr == NULL){
            DSP_MW_LOG_E("[CM4_RECORD] Get DSP callback_ptr NULL.", 0);
            return FALSE;
        }
    }else{
        DSP_MW_LOG_E("[CM4_RECORD]transform NULL.", 0);
        return FALSE;
    }
    U16 usCopySize,usCopyOffset,usUnwrapSize;
    U32 ulWriteOffset = sink->streamBuffer.ShareBufferInfo.WriteOffset;

    Cm4Record_update_from_share_information(sink);

    if (transform!=NULL && src_addr==NULL)
    {
        //src_addr = callback_ptr->EntryPara.out_ptr[0];
        if(CM4_Record_air_dump){
            switch(CM4_Record_air_dump_scenario){  // Mic_mic : 0x1 << 0,  Ref_mic: 0x1 << 1, Echo_ref: 0x1 << 2
                case 0x3:{
                    src_addr = callback_ptr->EntryPara.out_ptr[0];
                    src_addr_2 = callback_ptr->EntryPara.out_ptr[1];
                    break;
                }
                case 0x5:{
                    src_addr = callback_ptr->EntryPara.out_ptr[0];
                    src_addr_2 = callback_ptr->EntryPara.out_ptr[2];
                    break;
                }
                case 0x1:
                default:
                    src_addr = callback_ptr->EntryPara.out_ptr[0];
                    break;
            }
        } else {
            src_addr = callback_ptr->EntryPara.out_ptr[0];
        }
    }

    if(CM4_Record_air_dump){
        switch(CM4_Record_air_dump_scenario){  // Mic_mic : 0x1 << 0,  Ref_mic: 0x1 << 1, Echo_ref: 0x1 << 2
            case 0x3:{
                src_addr_2 = callback_ptr->EntryPara.out_ptr[1];
                break;
            }
            case 0x5:{
                src_addr_2 = callback_ptr->EntryPara.out_ptr[2];
                break;
            }
            case 0x1:
            default:
                break;
        }
    }

    if ((sink->param.cm4_record.interleave == true) && ( callback_ptr->EntryPara.out_channel_num > 1 )){
        src_addr_2 = callback_ptr->EntryPara.out_ptr[1];
        for (i = 0; i < callback_ptr->EntryPara.out_channel_num; i+=2) {
            usCopyOffset = 0;
            while( length > usCopyOffset )
            {
                usUnwrapSize = sink->streamBuffer.ShareBufferInfo.length - ulWriteOffset;
                usCopySize = MIN( (length-usCopyOffset) ,usUnwrapSize>>1 );

                if (sink->param.audio.format_bytes == 4){
                    DSP_D2I_BufferCopy_32bit((U32*)(sink->streamBuffer.ShareBufferInfo.startaddr + ulWriteOffset),
                                         (U32*)((U8*)src_addr + usCopyOffset),
                                         (U32*)((U8*)src_addr_2 + usCopyOffset),
                                         (U32)usCopySize>>2);
                }else{
                    DSP_D2I_BufferCopy_16bit((U16*)(sink->streamBuffer.ShareBufferInfo.startaddr + ulWriteOffset),
                                         (U16*)(src_addr + usCopyOffset),
                                         (U16*)(src_addr_2 + usCopyOffset),
                                         usCopySize>>1);
                }
#if 0
/*For debug record out data.*/
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
                LOG_AUDIO_DUMP((U8*)(src_addr + usCopyOffset), (U32)(usCopySize), SOURCE_IN3);
                LOG_AUDIO_DUMP((U8*)(src_addr_2 + usCopyOffset), (U32)(usCopySize), SOURCE_IN4);
#endif
#endif
                ulWriteOffset = (ulWriteOffset + (usCopySize<<1))%(sink->streamBuffer.ShareBufferInfo.length);
                usCopyOffset += usCopySize;
            }
        }
    } else {
        for (i = 0 ; i < length ; i+=copy_length)
        {
            write_ptr = (U8*)(sink->streamBuffer.ShareBufferInfo.startaddr + sink->streamBuffer.ShareBufferInfo.WriteOffset);
            if(CM4_Record_air_dump){
                write_ptr_2 = (U8*)(sink->streamBuffer.ShareBufferInfo.startaddr + sink->streamBuffer.ShareBufferInfo.WriteOffset + (sink->streamBuffer.ShareBufferInfo.length));
            }

            if ((length - i + sink->streamBuffer.ShareBufferInfo.WriteOffset)>sink->streamBuffer.ShareBufferInfo.length) {
                copy_length =sink->streamBuffer.ShareBufferInfo.length - sink->streamBuffer.ShareBufferInfo.WriteOffset;
            } else {
                copy_length = length-i;
            }

            memcpy(write_ptr, src_addr, copy_length);
            if(CM4_Record_air_dump){
                if(src_addr_2 != NULL){
                    memcpy(write_ptr_2, src_addr_2, copy_length);
                }
            }
#if 0
/*For debug record out data.*/
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
            LOG_AUDIO_DUMP((U8*)(write_ptr), (U32)(copy_length), SOURCE_IN3);
            LOG_AUDIO_DUMP((U8*)(write_ptr_2), (U32)(copy_length), SOURCE_IN4);
#endif
#endif

            src_addr = src_addr + copy_length;
            if(CM4_Record_air_dump){
                if(src_addr_2 != NULL){
                    src_addr_2 = src_addr_2 + copy_length;
                }
            }
            sink->streamBuffer.ShareBufferInfo.WriteOffset = (sink->streamBuffer.ShareBufferInfo.WriteOffset + copy_length)%(sink->streamBuffer.ShareBufferInfo.length);
        }
    }
    return TRUE;

}



/**
 * Sink_Cm4Record_Buffer_Init
 *
 * Function to update/reset cm4 record buffer.
 *
 * param :sink - cm4 record sink .
 *
 *
 */
VOID Sink_Cm4Record_Buffer_Init(SINK sink)
{
    Cm4Record_update_from_share_information(sink);
    Cm4Record_Reset_Sinkoffset_share_information(sink);

    CM4_RecordCtrl.frame_indx = 0;
    CM4_RecordCtrl.frames_per_message = sink->param.cm4_record.frames_per_message;
}


/**
 * SinkCloseCm4Record
 *
 * Function to shutdown cm4 record  sink.
 *
 *
 */
BOOL SinkCloseCm4Record(SINK sink)
{
    sink->param.cm4_record.process_data_length = 0;
    return TRUE;
}



/**
 * SinkInitCm4Record
 *
 * Function to initialize cm4 record sink.
 *
 *
 */
VOID SinkInitCm4Record(SINK sink)
{
    /* buffer init */
    Cm4Record_Default_setting_init();
    sink->buftype = BUFFER_TYPE_OTHER;

    Sink_Cm4Record_Buffer_Init(sink);

    /* interface init */
    sink->sif.SinkSlack       = SinkSlackCm4Record;
    sink->sif.SinkClaim       = SinkClaimCm4Record;
    sink->sif.SinkMap         = SinkMapCm4Record;
    sink->sif.SinkFlush       = SinkFlushCm4Record;
    sink->sif.SinkClose       = SinkCloseCm4Record;
    sink->sif.SinkWriteBuf    = SinkBufferWriteCm4Record;

}



