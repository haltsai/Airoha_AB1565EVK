/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 

#include <string.h>
#include "types.h"
#include "source_inter.h"
#include "dsp_buffer.h"
#include "dtm.h"
#include "stream_n9_a2dp.h"
#include "dsp_audio_msg.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_ctrl.h"
#include "dsp_memory.h"
#ifdef MTK_PEQ_ENABLE
#include "peq_interface.h"
#endif
#if AUDIO_ASI_CNT
local_asi_compare_info_t local_asi_comp;
#endif
log_create_module(strm_a2dp, PRINT_LEVEL_INFO);

/******************************************************************************
 * Definition
*******************************************************************************/
#define UNUSED(p) ((void)(p))
#define N9_A2DP_DEBUG_LOG   0
#define AAC_FRAME_DECODE_SAMPLE_NUM 1024
#define SBC_FRAME_DECODE_SAMPLE_NUM 128
#define AAC_BITRATE_REPORT_ACCUM_TIME 30
#define BUFFER_EMPTY_REINIT_THD 5
#define RESEND_REINIT_CNT (20) // Resend interval is  roughtly about (20ms *  BUFFER_EMPTY_REINIT_THD * RESEND_REINIT_CNT) 
#define SSRC_MAGIC_NUM 0x12345678
#define NOTIFY_LOW_BOND   90
#define NOTIFY_LOW_THD    50
#define PCB_HEADER_LEN    4
#define RTP_HEADER_LEN    12
#define CP_LEN            1
#define PAYLOAD_HEADER    1
#define VEND_FRAME_HEADER 4
#define N9_A2DP_DEBUG_ASSERT (0)

#ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
#define VEND_2_FRAME_DECODE_SAMPLE_NUM 256
#define VEND_2_PAYLOAD_HEADER    2
#endif

#define MACRO_CHK_FIRST_PKT(param,state)                \
        {                                               \
            if(param->DspReportStartId == (0x8000 | MSG_DSP2MCU_BT_AUDIO_DL_START_ACK))\
            {                                           \
                STRM_A2DP_LOG_I("%s at first packet\n", 1, state);   \
            }                                           \
        }


#ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
#include "vendor_decoder_proting.h"
vend_bc_extern_buf_t* p_vend_bc_exbuf_if;
#endif


/******************************************************************************
 * Extern Functions
*******************************************************************************/
EXTERN VOID StreamDSP_HWSemaphoreTake(VOID);
EXTERN VOID StreamDSP_HWSemaphoreGive(VOID);
EXTERN uint32_t afe_get_dl1_query_data_amount(void);;

/******************************************************************************
 * Static Functions
*******************************************************************************/
static VOID sbc_get_payload_information(SOURCE source, bt_codec_a2dp_sbc_pl_header_t *sbc_pl_info);
VOID SourceDrop_N9_a2dp(SOURCE source, U32 amount);

ATTR_TEXT_IN_IRAM_LEVEL_2 static VOID update_from_share_information(SOURCE source)
{
    StreamDSP_HWSemaphoreTake();

    /* Put your code here, if the semaphore is taken, do something for the critical resource...... */
    // skip startaddr to workaround cache address issue
    memcpy(&(source->streamBuffer.ShareBufferInfo.ReadOffset), (void *)(source->param.n9_a2dp.share_info_base_addr + sizeof(U32)), sizeof(SHARE_BUFFER_INFO)-sizeof(U32));/* share info fix 32 byte */

    /* ******************************************************************************************* */

    StreamDSP_HWSemaphoreGive();
}
static VOID update_to_share_information(SOURCE source)
{
    StreamDSP_HWSemaphoreTake();

    /* Put your code here, if the semaphore is taken, do something for the critical resource...... */
    memcpy((void *)(source->param.n9_a2dp.share_info_base_addr + 4), &(source->streamBuffer.ShareBufferInfo.ReadOffset), 4);/* update ReadOffset */
    memcpy((void *)(source->param.n9_a2dp.share_info_base_addr + 22), &(source->streamBuffer.ShareBufferInfo.bBufferIsFull), 1);/* update bBufferIsFull */
    /* ******************************************************************************************* */

    StreamDSP_HWSemaphoreGive();
}
#ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
static U8* reshape_share_buffer_for_vend_bc(SOURCE source,U16 length)
{
   return (U8*)((U8*)(((SHARE_BUFFER_INFO_PTR)source->param.n9_a2dp.share_info_base_addr)->startaddr) + length);
}

#endif

ATTR_TEXT_IN_IRAM_LEVEL_1 static U32 a2dp_get_pcb_info(SOURCE source)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U16 return_status = RETURN_PASS;
    bt_codec_a2dp_pcb_type_t a2dp_pcb_info;
    bt_codec_a2dp_pcb_type_t a2dp_pcb_info_temp;
    U32 mask;
    do {
        DSP_C2D_BufferCopy((VOID*)  &a2dp_pcb_info,
                           (VOID*)  (share_buff_info->startaddr + share_buff_info->ReadOffset),
                           (U16)    sizeof(bt_codec_a2dp_pcb_type_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);

        DSP_C2D_BufferCopy((VOID*)  &a2dp_pcb_info_temp,
                           (VOID*)  (share_buff_info->startaddr + share_buff_info->ReadOffset),
                           (U16)    sizeof(bt_codec_a2dp_pcb_type_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
        if ((a2dp_pcb_info.size != a2dp_pcb_info_temp.size)||(a2dp_pcb_info.state != a2dp_pcb_info_temp.state))
        {
            STRM_A2DP_LOG_E("meet chaos pcb header read ", 0);
            return RETURN_FAIL;
        }
        if ((a2dp_pcb_info.size > 0x1000)||(a2dp_pcb_info.state > PCB_STATE_DECODED))// abnormal large packet
        {
            STRM_A2DP_LOG_E("a2dp illegal pkt size :%d Ro:%d Wo:%d status :%d", 4, a2dp_pcb_info.size,share_buff_info->ReadOffset,share_buff_info->WriteOffset,a2dp_pcb_info.state);
            STRM_A2DP_LOG_E("pkt pcb: 0x%x rtp: 0x%x", 2, a2dp_pcb_info.size,*(U32*)(share_buff_info->startaddr + share_buff_info->ReadOffset + 4));
            STRM_A2DP_LOG_E("last pkt size %d seqN %d Ts %d", 3, n9_a2dp_param->current_packet_size,n9_a2dp_param->current_seq_num,n9_a2dp_param->current_timestamp);
            configASSERT(0);
        }

        /* Save global information */
        {
            n9_a2dp_param->current_packet_size = a2dp_pcb_info.size + 4;
            n9_a2dp_param->current_bitstream_size = a2dp_pcb_info.size - a2dp_pcb_info.data_size;
            STRM_A2DP_LOG_D("curr pkt size: %d, bitstream size: %d\r\n", 2, n9_a2dp_param->current_packet_size, n9_a2dp_param->current_bitstream_size);
        }

        if (a2dp_pcb_info.state == PCB_STATE_LOST)
        {
            if (source->transform == NULL){
                return_status = RETURN_FAIL;
                break;
            }
        
            if (n9_a2dp_param->pkt_lost_report_state == PL_STATE_NONE)
            {
               n9_a2dp_param->pkt_lost_report_state = PL_STATE_REPORTPREVSEQN;
            }
            if (afe_get_dl1_query_data_amount() > (3*(source->transform->sink->param.audio.rate * source->transform->sink->param.audio.period / 1000))){
                return_status = RETURN_FAIL;
                break;
            }
            else
            {
                U32 gpt_timer;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
                hal_nvic_save_and_set_interrupt_mask(&mask);
                share_buff_info->ReadOffset = (share_buff_info->ReadOffset + n9_a2dp_param->current_packet_size)%share_buff_info->length;
                source->streamBuffer.ShareBufferInfo.bBufferIsFull = FALSE;
                update_to_share_information(source);
                hal_nvic_restore_interrupt_mask(mask);
                STRM_A2DP_LOG_I("PCB_STATE_LOST SKIP seqN:%d skip size:%d Wo %d Ro %d timer %d", 5, n9_a2dp_param->current_seq_num,n9_a2dp_param->current_packet_size,share_buff_info->WriteOffset,share_buff_info->ReadOffset,gpt_timer);
                if (share_buff_info->ReadOffset == share_buff_info->WriteOffset) {
                    return_status = RETURN_FAIL;
                    break;
                }
            }
        }
        else if(a2dp_pcb_info.state == PCB_STATE_SKIP)
        {
            hal_nvic_save_and_set_interrupt_mask(&mask);
            share_buff_info->ReadOffset = (share_buff_info->ReadOffset + n9_a2dp_param->current_packet_size)%share_buff_info->length;
            update_to_share_information(source);
            hal_nvic_restore_interrupt_mask(mask);
            STRM_A2DP_LOG_I("PCB_STATE_SKIP pkt size:%d Wo:%d Ro:%d", 3, n9_a2dp_param->current_packet_size,share_buff_info->WriteOffset,share_buff_info->ReadOffset);
            if (share_buff_info->ReadOffset == share_buff_info->WriteOffset) {
                return_status = RETURN_FAIL;
                break;
            }
        }
        if ((a2dp_pcb_info.state != PCB_STATE_LOST)&&(a2dp_pcb_info.state != PCB_STATE_USED)&&(a2dp_pcb_info.state != PCB_STATE_SKIP)){
            STRM_A2DP_LOG_E("rare pcd state :%d", 1, a2dp_pcb_info.state);
            configASSERT(0);
        }
    } while(a2dp_pcb_info.state != PCB_STATE_USED);
    return return_status;
}


ATTR_TEXT_IN_IRAM_LEVEL_1 static VOID a2dp_get_rtpheader_info(SOURCE source, bt_codec_a2dp_rtpheader_t *a2dp_rtpheader_info)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 read_offset = 0;
    U32 pre_seq_num;
    pre_seq_num = n9_a2dp_param->current_seq_num;
    read_offset = share_buff_info->ReadOffset;
    if ( (share_buff_info->ReadOffset + 4) >= share_buff_info->length ) {
        read_offset = (share_buff_info->ReadOffset + 4 - share_buff_info->length);
    }
    else {
        read_offset = (share_buff_info->ReadOffset + 4);
    }
    DSP_C2D_BufferCopy((VOID*)  a2dp_rtpheader_info,
                       (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte */
                       (U16)    sizeof(bt_codec_a2dp_rtpheader_t),
                       (VOID*)  share_buff_info->startaddr,
                       (U16)    share_buff_info->length);
    /* Save global information */
    {
        n9_a2dp_param->current_seq_num = (U32)a2dp_rtpheader_info->seqno;
        #if A2DP_DBG_PORT
        hal_gpio_set_output(HAL_GPIO_19, (n9_a2dp_param->current_seq_num>>3) & 0x01);
        hal_gpio_set_output(HAL_GPIO_17, (n9_a2dp_param->current_seq_num>>2) & 0x01);
        hal_gpio_set_output(HAL_GPIO_16, (n9_a2dp_param->current_seq_num>>1) & 0x01);
        hal_gpio_set_output(HAL_GPIO_32, n9_a2dp_param->current_seq_num & 0x01);
        #endif
        n9_a2dp_param->current_timestamp = (n9_a2dp_param->ios_aac_flag == TRUE) ? (U32)a2dp_rtpheader_info->seqno : (U32)a2dp_rtpheader_info->timestamp;
        if (n9_a2dp_param->pkt_lost_report_state == PL_STATE_REPORTNEXTSEQN)
        {
            U32 gpt_timer;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
            n9_a2dp_param->pkt_lost_report_state = PL_STATE_NONE;
            STRM_A2DP_LOG_I("PL recover seq num: %d, curr ts: %d pts: %d time :%d \r\n ", 4, n9_a2dp_param->current_seq_num, n9_a2dp_param->current_timestamp, n9_a2dp_param->predict_timestamp,gpt_timer);
        }
    }
}

static bt_codec_a2dp_rtpheader_t a2dp_get_next_rtp_info(SOURCE source)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    bt_codec_a2dp_rtpheader_t a2dp_rtp_info;
    bt_codec_a2dp_pcb_type_t a2dp_next_pcb_info;
    U32 next_read_offset = 0;
    memset((void *)&a2dp_rtp_info, 0, sizeof(bt_codec_a2dp_rtpheader_t));

    a2dp_rtp_info.ssrc = SSRC_MAGIC_NUM;

    next_read_offset = (share_buff_info->ReadOffset + n9_a2dp_param->current_packet_size) %  share_buff_info->length;
    if(next_read_offset != share_buff_info->WriteOffset)
    {
        DSP_C2D_BufferCopy((VOID*)  &a2dp_next_pcb_info,
                               (VOID*)  (share_buff_info->startaddr + next_read_offset),
                               (U16)    sizeof(bt_codec_a2dp_pcb_type_t),
                               (VOID*)  share_buff_info->startaddr,
                               (U16)    share_buff_info->length);
        if (a2dp_next_pcb_info.state == PCB_STATE_USED)
        {
            next_read_offset = (next_read_offset + 4)%share_buff_info->length;

            DSP_C2D_BufferCopy((VOID*)  &a2dp_rtp_info,
                               (VOID*)  (share_buff_info->startaddr + next_read_offset),/* pcb:4 byte */
                               (U16)    sizeof(bt_codec_a2dp_rtpheader_t),
                               (VOID*)  share_buff_info->startaddr,
                               (U16)    share_buff_info->length);
            /* Save global information */
            a2dp_rtp_info.ssrc = 0;
        }
    }
    return a2dp_rtp_info;
}

/*static U32 a2dp_skip_to_valid_packet(SOURCE source)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 temp_readoffset = share_buff_info->ReadOffset;
    U32 temp_packet_size = 0;
    bt_codec_a2dp_pcb_type_t *a2dp_pcb_info;

    do {
        //Check next packet is valid or not
        temp_readoffset += n9_a2dp_param->current_packet_size;
        if ( temp_readoffset >= share_buff_info->length ) {
            temp_readoffset -= share_buff_info->length;
        }
        if ( temp_readoffset >= share_buff_info->WriteOffset ) {
            return RETURN_FAIL;
        }
        else {
            a2dp_get_pcb_info(source, a2dp_pcb_info);
        }
    } while(a2dp_pcb_info->state == PCB_STATE_LOST);

    share_buff_info->ReadOffset = temp_readoffset;
    update_to_share_information(source);

    return RETURN_PASS;
}*/

static VOID a2dp_skip_expired_packet(SOURCE source)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    bt_codec_a2dp_rtpheader_t a2dp_rtpheader_info;
    U32 mask;

    while((((S32)n9_a2dp_param->predict_timestamp - (S32)n9_a2dp_param->current_timestamp)) > 0){
        /* Check next packet is expired or not */
        if (( share_buff_info->ReadOffset == share_buff_info->WriteOffset )&&(source->streamBuffer.ShareBufferInfo.bBufferIsFull == FALSE)) {
            break;
        }
        if(n9_a2dp_param->ios_sbc_flag == TRUE) {
            bt_codec_a2dp_sbc_pl_header_t sbc_pl_info;
            sbc_get_payload_information(source, &sbc_pl_info);
        }
        hal_nvic_save_and_set_interrupt_mask(&mask);
        STRM_A2DP_LOG_I("skip expired Wo %d Ro %d psize %d ts %d pts %d", 5, share_buff_info->WriteOffset,share_buff_info->ReadOffset,n9_a2dp_param->current_packet_size,n9_a2dp_param->current_timestamp, n9_a2dp_param->predict_timestamp);
        share_buff_info->ReadOffset += n9_a2dp_param->current_packet_size;
        if ( share_buff_info->ReadOffset >= share_buff_info->length ) {
            share_buff_info->ReadOffset -= share_buff_info->length;
        }
        source->streamBuffer.ShareBufferInfo.bBufferIsFull = FALSE;
        update_to_share_information(source);
        hal_nvic_restore_interrupt_mask(mask);
        if ( share_buff_info->ReadOffset == share_buff_info->WriteOffset ) {
            break;
        }
        else if (a2dp_get_pcb_info(source) == RETURN_PASS)
        {
            a2dp_get_rtpheader_info(source, &a2dp_rtpheader_info);
        }
        else
        {
            break;
        }
    }

    // share_buff_info->ReadOffset = temp_readoffset;
    // update_to_share_information(source);
}

ATTR_TEXT_IN_IRAM_LEVEL_2 static VOID sbc_get_payload_information(SOURCE source, bt_codec_a2dp_sbc_pl_header_t *sbc_pl_info)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 read_offset = 0;

    if ( n9_a2dp_param->cp_exist == TRUE ) {
        read_offset = share_buff_info->ReadOffset;
        if ( (share_buff_info->ReadOffset + 4 + 12 + 1) >= share_buff_info->length ) {
            read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1 - share_buff_info->length);
        }
        else {
            read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1);
        }
        DSP_C2D_BufferCopy((VOID*)  sbc_pl_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte */
                           (U16)    sizeof(bt_codec_a2dp_sbc_pl_header_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    else {
        read_offset = share_buff_info->ReadOffset;
        if ( (share_buff_info->ReadOffset + 4 + 12) >= share_buff_info->length ) {
            read_offset = (share_buff_info->ReadOffset + 4 + 12 - share_buff_info->length);
        }
        else {
            read_offset = (share_buff_info->ReadOffset + 4 + 12);
        }
        DSP_C2D_BufferCopy((VOID*)  sbc_pl_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte */
                           (U16)    sizeof(bt_codec_a2dp_sbc_pl_header_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    /* Save global information */
    {
        n9_a2dp_param->totalFrameCnt = sbc_pl_info->numOfFrames;
        n9_a2dp_param->dropFrameCnt = 0;
        /* Check fragmented or not */
        if ( sbc_pl_info->fragmented == 1 ) {
            n9_a2dp_param->fragment_flag = TRUE;
        }
        else {
            n9_a2dp_param->fragment_flag = FALSE;
        }
        STRM_A2DP_LOG_D("total frm cnt: %d, fragment flag: %d\r\n", 2, n9_a2dp_param->totalFrameCnt, n9_a2dp_param->fragment_flag);
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_2 static VOID sbc_get_frame_information(SOURCE source, bt_codec_a2dp_sbc_frame_header_t *sbc_frame_info)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 read_offset = 0;

    if( n9_a2dp_param->cp_exist == TRUE ) {

        read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1 + 1 + n9_a2dp_param->readOffset)%share_buff_info->length;
        
        DSP_C2D_BufferCopy((VOID*)  sbc_frame_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte, pl header: 1 byte*/
                           (U16)    sizeof(bt_codec_a2dp_sbc_frame_header_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    else {
        read_offset = ((share_buff_info->ReadOffset + 4 + 12 + 1 + n9_a2dp_param->readOffset)%share_buff_info->length);
        DSP_C2D_BufferCopy((VOID*)  sbc_frame_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, pl header: 1 byte*/
                           (U16)    sizeof(bt_codec_a2dp_sbc_frame_header_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    if ( sbc_frame_info->SyncWord != 0x9C ){
        //for(int i = 0; i < 20; i++){
            //printf("%d, %x\r\n", i, *(U8*)(share_buff_info->startaddr + i));
            //printf("%d %d %d %x\r\n", i, share_buff_info->length, share_buff_info->ReadOffset, *(U8*)((share_buff_info->startaddr + read_offset + i)%share_buff_info->length));
        //}
        STRM_A2DP_LOG_E("SBC wrong sync word enter BCM %d %d %d %d %d", 5, n9_a2dp_param->readOffset,n9_a2dp_param->totalFrameCnt,n9_a2dp_param->dropFrameCnt,n9_a2dp_param->current_packet_size,share_buff_info->ReadOffset);
        STRM_A2DP_LOG_D("sync word error, %x\r\n", 1, sbc_frame_info->SyncWord);
    }
}


uint32_t a2dp_get_samplingrate(SOURCE source)
{
    uint32_t samplerate = 0;
    if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
        switch (source->param.n9_a2dp.codec_info.codec_cap.codec.sbc.sample_rate) {
            case 8:
                samplerate = 16000;
                break;
            case 4:
                samplerate = 32000;
                break;
            case 2:
                samplerate = 44100;
                break;
            case 1:
                samplerate = 48000;
                break;
            default:
                STRM_A2DP_LOG_E("sample rate info error/r/n", 0);
        }
    }
    else if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AAC ) {
        switch (source->param.n9_a2dp.codec_info.codec_cap.codec.aac.sample_rate) {
            case 0x800:
                samplerate = 8000;
                break;
            case 0x400:
                samplerate = 11025;
                break;
            case 0x200:
                samplerate = 12000;
                break;
            case 0x100:
                samplerate = 16000;
                break;
            case 0x80:
                samplerate = 22050;
                break;
            case 0x40:
                samplerate = 24000;
                break;
            case 0x20:
                samplerate = 32000;
                break;
            case 0x10:
                samplerate = 44100;
                break;
            case 0x8:
                samplerate = 48000;
                break;
            default:
                STRM_A2DP_LOG_E("sample rate info error/r/n", 0);
        }
    }
    else if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id == 0x00AA) {
        switch (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.sample_rate) {
            case 0x20:
                samplerate = 44100;
                break;
            case 0x10:
                samplerate = 48000;
                break;
            case 0x08:
                samplerate = 88200;
                break;
            case 0x04:
                samplerate = 96000;
                break;
            case 0x02:
                samplerate = 176400;
                break;
            case 0x01:
                samplerate = 192000;
                break;
            default:
                STRM_A2DP_LOG_E("sample rate info error %d/r/n",1, source->param.n9_a2dp.codec_info.codec_cap.codec.aac.sample_rate);
            }
    }
    #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE 
    else if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33) {
        switch (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.sample_rate) {
            case 0x08:
                samplerate = 44100;
                break;
            case 0x04:
                samplerate = 48000;
                break;
            case 0x02:
                samplerate = 88200;
                break;
            case 0x01:
                samplerate = 96000;
                break;
            default:
                STRM_A2DP_LOG_E("sample rate info error %d/r/n",1, source->param.n9_a2dp.codec_info.codec_cap.codec.aac.sample_rate);
            }
    }
    #endif
    else {
        //not support codec type
    }
    return samplerate;
}

uint32_t a2dp_get_channel(SOURCE source)
{
    uint32_t channel = 0;
    if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
        switch (source->param.n9_a2dp.codec_info.codec_cap.codec.sbc.channel_mode) {
            case 8:
                channel = 1;
                break;
            case 4:
            case 2:
            case 1:
                channel = 2;
                break;
            default:
                STRM_A2DP_LOG_E("channel info error/r/n" , 0);
        }
    }
    else if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AAC ) {
        switch (source->param.n9_a2dp.codec_info.codec_cap.codec.aac.channels) {
            case 0x2:
                channel = 1;
                break;
            case 0x1:
                channel = 2;
                break;
            default:
                STRM_A2DP_LOG_E("channel info error/r/n", 0);
        }
    }
    else if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id == 0x00AA) {
        switch (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.channels) {
            case 0x0:
                channel = 1;
                break;
            case 0x1:
            case 0x2:
                channel = 2;
                break;
            default:
                STRM_A2DP_LOG_E("channel info error %d/r/n", 1, source->param.n9_a2dp.codec_info.codec_cap.codec.vend.channels);
        }
    }
    #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
    else if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33) {
        switch (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.channels) {
            case 0x0:
                channel = 1;
                break;
            case 0x1:
            case 0x2:
                channel = 2;
                break;
            default:
                STRM_A2DP_LOG_E("channel info error %d/r/n", 1, source->param.n9_a2dp.codec_info.codec_cap.codec.vend.channels);
        }
    }
    #endif
    else {
        //not support codec type
    }
    return channel;
}

static VOID vend_get_payload_information(SOURCE source, bt_codec_a2dp_vend_pl_header_t *vend_pl_info)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 read_offset = 0;

    if ( n9_a2dp_param->cp_exist == TRUE ) {
        read_offset = share_buff_info->ReadOffset;
        if ( (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN) >= share_buff_info->length ) {
            read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN - share_buff_info->length);
        }
        else {
            read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN);
        }
        DSP_C2D_BufferCopy((VOID*)  vend_pl_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte */
                           (U16)    sizeof(bt_codec_a2dp_vend_pl_header_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    else {
        read_offset = share_buff_info->ReadOffset;
        if ( (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN) >= share_buff_info->length ) {
            read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN - share_buff_info->length);
        }
        else {
            read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN);
        }
        DSP_C2D_BufferCopy((VOID*)  vend_pl_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte */
                           (U16)    sizeof(bt_codec_a2dp_vend_pl_header_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    /* Save global information */
    {
        n9_a2dp_param->totalFrameCnt = vend_pl_info->numOfFrames;
        n9_a2dp_param->dropFrameCnt = 0;
        /* Check fragmented or not */
        if ( vend_pl_info->F_Bit == 1 ) {
            n9_a2dp_param->fragment_flag = TRUE;
        }
        else {
            n9_a2dp_param->fragment_flag = FALSE;
        }
        STRM_A2DP_LOG_D("total frm cnt: %d, fragment flag: %d\r\n", 2, n9_a2dp_param->totalFrameCnt, n9_a2dp_param->fragment_flag);
    }
}


void vend_drop_frame(SOURCE source){
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    U32 mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    n9_a2dp_param->dropFrameCnt++;
    if ( n9_a2dp_param->dropFrameCnt == n9_a2dp_param->totalFrameCnt ) {
        /* Update share buffer read offset */
        share_buff_info->ReadOffset += n9_a2dp_param->current_packet_size;
        if ( share_buff_info->ReadOffset >= share_buff_info->length ) {
            share_buff_info->ReadOffset -= share_buff_info->length;
        }
        /* reset frame read offset */
        share_buff_info->bBufferIsFull = FALSE;
        n9_a2dp_param->readOffset = 0;
    }
    else {
        n9_a2dp_param->readOffset += n9_a2dp_param->current_frame_size;
        if (n9_a2dp_param->readOffset > n9_a2dp_param->current_packet_size){
            STRM_A2DP_LOG_I("readoffset %d cross over packet size %d Wo:%d Ro:%d", 4,n9_a2dp_param->readOffset,n9_a2dp_param->current_packet_size,share_buff_info->WriteOffset,share_buff_info->ReadOffset);
            #if N9_A2DP_DEBUG_ASSERT
            configASSERT(0);
            #endif
        }
    }
    hal_nvic_restore_interrupt_mask(mask);
}
void vend_read_frame_from_packet(SOURCE source,U8 *dst_addr, U32 length,U8 payload_header_len ){
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    U32 read_offset;
    if( n9_a2dp_param->cp_exist == TRUE ) {
                read_offset = share_buff_info->ReadOffset;
                if ( (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN + payload_header_len + n9_a2dp_param->readOffset) >= share_buff_info->length ) {
                    read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN + payload_header_len + n9_a2dp_param->readOffset - share_buff_info->length);
                }
                else {
                    read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN + payload_header_len + n9_a2dp_param->readOffset);
                }
                DSP_C2D_BufferCopy((VOID*)  dst_addr,
                                   (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte */
                                   (U16)    length,
                                   (VOID*)  share_buff_info->startaddr,
                                   (U16)    share_buff_info->length);
            }
            else {
                read_offset = share_buff_info->ReadOffset;
                if ( (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + payload_header_len + n9_a2dp_param->readOffset) >= share_buff_info->length ) {
                    read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + payload_header_len + n9_a2dp_param->readOffset - share_buff_info->length);
                }
                else {
                    read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + payload_header_len + n9_a2dp_param->readOffset);
                }
                DSP_C2D_BufferCopy((VOID*)  dst_addr,
                                   (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte */
                                   (U16)    length,
                                   (VOID*)  share_buff_info->startaddr,
                                   (U16)    share_buff_info->length);
            }

}



static VOID vend_get_frame_information(SOURCE source, VOID *vend_frame_info, U8 payload_header_len, U8 frame_info_len)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 read_offset = 0;

    if( n9_a2dp_param->cp_exist == TRUE ) {
        read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN + payload_header_len + n9_a2dp_param->readOffset)%share_buff_info->length;
        DSP_C2D_BufferCopy((VOID*)  vend_frame_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte, pl header: 1 byte*/
                           (U16)    frame_info_len,
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    else {
        read_offset = ((share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + payload_header_len + n9_a2dp_param->readOffset)%share_buff_info->length);
        DSP_C2D_BufferCopy((VOID*)  vend_frame_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, pl header: 1 byte*/
                           (U16)    frame_info_len,
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
}

static U16 vend_get_frame_size(SOURCE source)
{
    U16 frame_size;
    U32 vend_sample_rate = a2dp_get_samplingrate(source);
         switch (vend_sample_rate) {
                case 44100:
                case 48000:
                    frame_size = 128;
                    break;
                case 88200:
                case 96000:
                    frame_size = 256;
                    break;
                case 176400:
                case 192000:
                    frame_size = 512;
                    break;
                default :
                    frame_size = 0;
                    break;
            }
    return frame_size;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 static U32 vend_frame_size_calc(bt_codec_a2dp_vend_frame_header_t *vend_frame_info)
{
    U32 frame_size;
    frame_size = (vend_frame_info->vend_byte2 & 0x7);
    frame_size = (frame_size<<6) + (vend_frame_info->vend_byte3>>2) + VEND_FRAME_HEADER;
    return frame_size;
}
#ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
static VOID vend_2_get_payload_information(SOURCE source, bt_codec_a2dp_vend_2_pl_header_t *vend_pl_info)
{
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 read_offset = 0;

    if ( n9_a2dp_param->cp_exist == TRUE ) {
        read_offset = share_buff_info->ReadOffset;
        if ( (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN) >= share_buff_info->length ) {
            read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN - share_buff_info->length);
        }
        else {
            read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN + CP_LEN);
        }
        DSP_C2D_BufferCopy((VOID*)  vend_pl_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte */
                           (U16)    sizeof(bt_codec_a2dp_vend_2_pl_header_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    else {
        read_offset = share_buff_info->ReadOffset;
        if ( (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN) >= share_buff_info->length ) {
            read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN - share_buff_info->length);
        }
        else {
            read_offset = (share_buff_info->ReadOffset + PCB_HEADER_LEN + RTP_HEADER_LEN);
        }
        DSP_C2D_BufferCopy((VOID*)  vend_pl_info,
                           (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte */
                           (U16)    sizeof(bt_codec_a2dp_vend_2_pl_header_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);
    }
    /* Save global information */
    {
        /* Check fragmented or not */
        n9_a2dp_param->fragment_flag = FALSE;
        n9_a2dp_param->totalFrameCnt = vend_pl_info->numOfFrames;
        n9_a2dp_param->dropFrameCnt = 0;
        STRM_A2DP_LOG_D("total frm cnt: %d, fragment flag: %d\r\n", 2, n9_a2dp_param->totalFrameCnt, n9_a2dp_param->fragment_flag);
    }
}
ATTR_TEXT_IN_IRAM_LEVEL_2 static U32 vend_2_frame_size_calc(U32 vend_frame_info)
{
    U32 frame_size;
    frame_size = (vend_frame_info >> 8) & 0x1fff;
    return frame_size;
}
#endif

#ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
void vend_bc_write_loop_callback(void)
{
    U8 pcb_parse_state;
    SOURCE source = p_vend_bc_exbuf_if->a2dp_source;
    SHARE_BUFFER_INFO *share_buff_info  = &(p_vend_bc_exbuf_if->a2dp_source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(p_vend_bc_exbuf_if->a2dp_source->param.n9_a2dp);
    bt_codec_a2dp_vend_pl_header_t pl_info;
    bt_codec_a2dp_vend_frame_header_t vend_frame_info;
    U32 data_size;
    data_size = (share_buff_info->WriteOffset >= share_buff_info->ReadOffset) 
        ? (share_buff_info->WriteOffset - share_buff_info->ReadOffset) 
        : (share_buff_info->length - share_buff_info->ReadOffset + share_buff_info->WriteOffset);
    while ((data_size > 700)||(share_buff_info->bBufferIsFull == TRUE)){

        pcb_parse_state = a2dp_get_pcb_info(source);
        if (pcb_parse_state == RETURN_PASS)
        {
            if (n9_a2dp_param->readOffset == 0)
            {
                vend_get_payload_information(source, &pl_info);
                /* Get sbc frame information */
            }
            
            vend_get_frame_information(source, &vend_frame_info,PAYLOAD_HEADER,sizeof(bt_codec_a2dp_vend_frame_header_t));
            n9_a2dp_param->current_frame_size = (vend_frame_info.vend_byte2 & 0x7);
            n9_a2dp_param->current_frame_size = (n9_a2dp_param->current_frame_size<<6) + (vend_frame_info.vend_byte3>>2) + VEND_FRAME_HEADER;
            vend_read_frame_from_packet(source,p_vend_bc_exbuf_if->bc_temp_buf,n9_a2dp_param->current_frame_size,PAYLOAD_HEADER);
            if (vend_bc_write(p_vend_bc_exbuf_if->p_handle, p_vend_bc_exbuf_if->bc_temp_buf, n9_a2dp_param->current_frame_size) != 0x7fff)
            {
                vend_drop_frame(source);
                update_to_share_information(source);
            }
            else
            {
                break;
            }
        }
        else
        {
            break;
        }
        data_size = (share_buff_info->WriteOffset >= share_buff_info->ReadOffset) 
        ? (share_buff_info->WriteOffset - share_buff_info->ReadOffset) 
        : (share_buff_info->length - share_buff_info->ReadOffset + share_buff_info->WriteOffset);
    }
}

#if 0
static void s_set_vend_bc_params(vend_bc_param_t *params){
    params->num_observe               = 256;    /* Number of observe for average retained frames, Range is 1 - 256. */
    params->observe_interval_msec     = 1*1000; /* Observed interval for average retained frames */
    params->num_frame_start           = 64;     /* Num frame to start */
    params->num_frame_target          = 64;     /* Target frames. [ 375 frame/sec * 0.17 sec = 64 (round up 63.75) ] */
    params->num_frame_threshold_upper = 5;      /* Frame upper threshold, difference from target frames. */
    params->num_frame_threshold_lower = -20;    /* Frame lower threshold, difference from target frames. */
    params->burst_enable              = 0;      /* Enable burst. setting value is 0(disable) or other than 0(enable). */
    params->burst_msec                = 2000;   /* Set when burst is enabled. Range is 0 - 2000 msec, If exceeds the range, rounded to boundary value in the range. */
}
#endif 

extern vend_bc_param_t bc_params;void config_vend_bc_params(hal_ccni_message_t msg, hal_ccni_message_t *ack){    
    UNUSED(ack);
    switch (msg.ccni_message[0]&0xFFFF){
        case 1 :
                bc_params.num_frame_start = msg.ccni_message[1];
                break;
        case 2 :
                bc_params.num_frame_target = msg.ccni_message[1];
                break;              
        case 3 :
                bc_params.num_frame_threshold_upper = msg.ccni_message[1];
                break;
        case 4 :
                bc_params.num_frame_threshold_lower = msg.ccni_message[1]; 
                break;
        default :
                STRM_A2DP_LOG_I("BC config failed: %d",1,msg.ccni_message[1]);
                break;
        }
}

#endif


ATTR_TEXT_IN_IRAM_LEVEL_2 static U32 sbc_frame_size_calc(bt_codec_a2dp_sbc_frame_header_t *sbc_frame_info)
{
    U32 framesize;
    U16 blocks;
    U16 subbands;
    U16 bitpool;
    U16 channels;

    channels    = (sbc_frame_info->Byte1.bit_alloc.CHANNEL_MODE == SBC_MONO_CHANNEL) ? (U16)1:(U16)2;
    subbands    = (sbc_frame_info->Byte1.bit_alloc.SUBBANDS == 0) ? (U16)4:(U16)8;
    bitpool     = sbc_frame_info->Bitpool;
    blocks      = (sbc_frame_info->Byte1.bit_alloc.BLOCKS + 1) * (U16)4;

    switch(sbc_frame_info->Byte1.bit_alloc.CHANNEL_MODE)
    {
        case SBC_MONO_CHANNEL:
        case SBC_DUAL_CHANNEL:
            framesize = ((blocks * channels * bitpool) + 7)/8 + 4 + (4 * subbands * channels)/8;
            break;

        case SBC_STEREO_CHANNEL:
            framesize = ((blocks * bitpool) + 7)/8 + 4 + (4 * subbands * channels)/8;
            break;
        
        default:
            framesize  = ((subbands + (blocks * bitpool)) + 7)/8 + 4 + (4 * subbands * channels)/8;
            break;
    }
    return framesize;
}

static bool sbc_report_bitrate(SOURCE source) 
{
    U16 blocks;
    U16 subbands;
    bt_codec_a2dp_sbc_frame_header_t sbc_frame_info;
    /* Get sbc frame information */
    sbc_get_frame_information(source, &sbc_frame_info);
    /* Calculating frame size */
    subbands    = (sbc_frame_info.Byte1.bit_alloc.SUBBANDS == 0) ? (U16)4:(U16)8;
    blocks      = (sbc_frame_info.Byte1.bit_alloc.BLOCKS + 1) * (U16)4;
    *(source->param.n9_a2dp.a2dp_bitrate_report.p_a2dp_bitrate_report) = 8*(sbc_frame_size_calc(&sbc_frame_info)*a2dp_get_samplingrate(source)/(U32)(subbands*blocks));
    //STRM_A2DP_LOG_I("sbc bit rate %d", 1, *(source->param.n9_a2dp.a2dp_bitrate_report.p_a2dp_bitrate_report));
    return (sbc_frame_info.SyncWord == 0x9c);
}

//Warning : Didn't consider fragmented case.
static U16 sbc_register_lost_packet(SOURCE source)
{
    N9_A2DP_PARAMETER *n9_a2dp_param = &(source->param.n9_a2dp);
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    U16 return_status = RETURN_PASS;
    uint32_t mask;
    UNUSED(share_buff_info);

    if (n9_a2dp_param->totalFrameCnt == 0)
    {
        bt_codec_a2dp_sbc_pl_header_t sbc_pl_info;
        sbc_get_payload_information(source, &sbc_pl_info);
    }

    do {
        STRM_A2DP_LOG_I("enter BCM packet loss, expect seqn:%d jump to seqn:%d (ios sbc). Wo:%d Ro:%d cnt=%d overpad=%d\n", 6, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp, share_buff_info->WriteOffset, share_buff_info->ReadOffset, n9_a2dp_param->totalFrameCnt, n9_a2dp_param->u2OverPadFrameCnt);

        hal_nvic_save_and_set_interrupt_mask(&mask);
        n9_a2dp_param->pkt_loss_seq_no = n9_a2dp_param->predict_timestamp;
        n9_a2dp_param->padding_count = n9_a2dp_param->totalFrameCnt;
        n9_a2dp_param->predict_timestamp = (n9_a2dp_param->predict_timestamp + 1)%0x10000;

        if(n9_a2dp_param->u2OverPadFrameCnt >= n9_a2dp_param->padding_count)
        {
            STRM_A2DP_LOG_I("OverPadFrameCnt = %d -> %d\n", 3, n9_a2dp_param->u2OverPadFrameCnt,n9_a2dp_param->u2OverPadFrameCnt-n9_a2dp_param->padding_count);
            n9_a2dp_param->u2OverPadFrameCnt -= n9_a2dp_param->padding_count;
            n9_a2dp_param->padding_count = 0;
            hal_nvic_restore_interrupt_mask(mask);
        }
        else
        {
            n9_a2dp_param->padding_count -= (n9_a2dp_param->u2OverPadFrameCnt + 1);
            n9_a2dp_param->u2OverPadFrameCnt = 0;
            DSP_Callback_BypassModeCtrl(source, source->transform->sink, BYPASS_CODEC_MODE);
            n9_a2dp_param->current_frame_size = SBC_FRAME_DECODE_SAMPLE_NUM;
            return_status = RETURN_FAIL;
            hal_nvic_restore_interrupt_mask(mask);
            break;
        }
    } while((S32)(n9_a2dp_param->predict_timestamp - n9_a2dp_param->current_timestamp) < 0);

    return return_status;
}

static BOOL sbc_lost_packet_handler(SOURCE source)
{
    N9_A2DP_PARAMETER *n9_a2dp_param = &(source->param.n9_a2dp);
    uint32_t mask;

    if((n9_a2dp_param->mce_flag == TRUE) && (n9_a2dp_param->ios_sbc_flag == TRUE) && (n9_a2dp_param->padding_count > 0))
    {
        STRM_A2DP_LOG_I("enter BCM packet loss, seqn:%d in ios sbc case. cnt=%d overpad=%d\n", 3, n9_a2dp_param->pkt_loss_seq_no,n9_a2dp_param->padding_count,n9_a2dp_param->u2OverPadFrameCnt);

        hal_nvic_save_and_set_interrupt_mask(&mask);
        if(n9_a2dp_param->u2OverPadFrameCnt >= n9_a2dp_param->padding_count)
        {
            n9_a2dp_param->u2OverPadFrameCnt -= n9_a2dp_param->padding_count;
            n9_a2dp_param->padding_count = 0;
            hal_nvic_restore_interrupt_mask(mask);
            return FALSE;
        }
        else
        {
            n9_a2dp_param->padding_count -= (n9_a2dp_param->u2OverPadFrameCnt + 1);
            n9_a2dp_param->u2OverPadFrameCnt = 0;
            DSP_Callback_BypassModeCtrl(source, source->transform->sink, BYPASS_CODEC_MODE);
            n9_a2dp_param->current_frame_size = SBC_FRAME_DECODE_SAMPLE_NUM;
            hal_nvic_restore_interrupt_mask(mask);
            return TRUE;
        }
    }
    return FALSE;
}

static U16 sbc_check_overpad(SOURCE source)
{
    N9_A2DP_PARAMETER *n9_a2dp_param = &(source->param.n9_a2dp);
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    U16 return_status = RETURN_PASS;

    if((n9_a2dp_param->mce_flag == TRUE) && (n9_a2dp_param->ios_sbc_flag == TRUE) && (n9_a2dp_param->readOffset == 0))
    {
        while(n9_a2dp_param->u2OverPadFrameCnt != 0)
        {
            if(n9_a2dp_param->u2OverPadFrameCnt < n9_a2dp_param->totalFrameCnt)
            {
                bt_codec_a2dp_sbc_frame_header_t sbc_frame_info;
                U16 header = ( n9_a2dp_param->cp_exist == TRUE ) ? (4 + 12 + 1 + 1) : (4 + 12 + 1);
                U16 overPadCnt = n9_a2dp_param->u2OverPadFrameCnt;
                U32 mask;
                U8  wrong_syncword = 0;
                hal_nvic_save_and_set_interrupt_mask(&mask);
                for(U16 i=0; i<overPadCnt; i++)
                {
                    U32 read_offset = (share_buff_info->ReadOffset + (U32)header + n9_a2dp_param->readOffset)%share_buff_info->length;
                    DSP_C2D_BufferCopy((VOID*)  &sbc_frame_info,
                                       (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte, pl header: 1 byte*/
                                       (U16)    sizeof(bt_codec_a2dp_sbc_frame_header_t),
                                       (VOID*)  share_buff_info->startaddr,
                                       (U16)    share_buff_info->length);
                    if(sbc_frame_info.SyncWord != 0x9C) {
                        STRM_A2DP_LOG_E("Wrong sync word at [seq]%d [frames-th]%d\n", 3, n9_a2dp_param->current_seq_num, i);
                        wrong_syncword = 1;
                    }
                    n9_a2dp_param->current_frame_size = sbc_frame_size_calc(&sbc_frame_info);
                    SourceDrop(source,n9_a2dp_param->current_frame_size);
                    n9_a2dp_param->u2OverPadFrameCnt--;
                }
                if(wrong_syncword != 0)
                {
                    do {
                        //n9_a2dp_param->current_frame_size = sbc_frame_size_calc(&sbc_frame_info);
                        SourceDrop(source,n9_a2dp_param->current_frame_size);
                        n9_a2dp_param->pkt_loss_seq_no = n9_a2dp_param->current_seq_num;
                        n9_a2dp_param->padding_count++;
                    } while(n9_a2dp_param->dropFrameCnt < n9_a2dp_param->totalFrameCnt);
                }
                STRM_A2DP_LOG_I("drop %d frames [ori_overPadCnt]%d [readOffset]0x%x\n", 4, n9_a2dp_param->dropFrameCnt,overPadCnt,n9_a2dp_param->readOffset);
                hal_nvic_restore_interrupt_mask(mask);
                break;
            }
            else
            {
                U32 mask;
                hal_nvic_save_and_set_interrupt_mask(&mask);
                share_buff_info->ReadOffset += n9_a2dp_param->current_packet_size;
                if(share_buff_info->ReadOffset >= share_buff_info->length) {
                    share_buff_info->ReadOffset -= share_buff_info->length;
                }
                share_buff_info->bBufferIsFull = FALSE;
                update_to_share_information(source);
                n9_a2dp_param->predict_timestamp = (n9_a2dp_param->predict_timestamp + 1)%0x10000;
                n9_a2dp_param->u2OverPadFrameCnt -= n9_a2dp_param->totalFrameCnt;
                hal_nvic_restore_interrupt_mask(mask);
                STRM_A2DP_LOG_I("drop a whole packet (%d frames) [ori_overPadCnt]%d [seq]%d [ro]%d [wo]%d [pt_size]%d\n", 7, n9_a2dp_param->totalFrameCnt,n9_a2dp_param->u2OverPadFrameCnt+n9_a2dp_param->totalFrameCnt,n9_a2dp_param->current_timestamp,share_buff_info->ReadOffset,share_buff_info->WriteOffset,n9_a2dp_param->current_packet_size);
                if ( share_buff_info->ReadOffset == share_buff_info->WriteOffset ) {
                    //STRM_A2DP_LOG_I("buffer empty, overpad=%d\n", 2, n9_a2dp_param->u2OverPadFrameCnt);
                    return_status = RETURN_FAIL;
                    break;
                }
                else if (a2dp_get_pcb_info(source) == RETURN_PASS)
                {
                    bt_codec_a2dp_rtpheader_t a2dp_rtpheader_info;
                    bt_codec_a2dp_sbc_pl_header_t sbc_pl_info;
                    a2dp_get_rtpheader_info(source, &a2dp_rtpheader_info);
                    if(n9_a2dp_param->current_timestamp != n9_a2dp_param->predict_timestamp) {
                        return_status = RETURN_FAIL;
                        //STRM_A2DP_LOG_I("get future packet, pts:ts=%d:%d, overpad=%d\n", 4, n9_a2dp_param->predict_timestamp,n9_a2dp_param->current_timestamp,n9_a2dp_param->u2OverPadFrameCnt);
                        break;
                    } else {
                        sbc_get_payload_information(source, &sbc_pl_info);
                        //STRM_A2DP_LOG_I("get correct packet, pts:ts=%d:%d, overpad=%d\n", 4, n9_a2dp_param->predict_timestamp,n9_a2dp_param->current_timestamp,n9_a2dp_param->u2OverPadFrameCnt);
                    }
                }
                else
                {
                    //STRM_A2DP_LOG_I("can't get valid packet, overpad=%d\n", 2, n9_a2dp_param->u2OverPadFrameCnt);
                    return_status = RETURN_FAIL;
                    break;
                }
            }
        }
    }
    return return_status;
}

void Au_DL_send_reinit_request(DSP_REINIT_CAUSE reinit_msg)
{
    hal_ccni_message_t msg;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = MSG_DSP2MCU_BT_AUDIO_DL_REINIT_REQUEST << 16;
    msg.ccni_message[1] = reinit_msg;
    aud_msg_tx_handler(msg, 0, FALSE);
}

void Au_DL_send_alc_request(U32 alc_latency)
{
    hal_ccni_message_t msg;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = MSG_DSP2MCU_BT_AUDIO_DL_ALC_REQUEST << 16;
    msg.ccni_message[1] = alc_latency;
    aud_msg_tx_handler(msg, 0, FALSE);
}


BOOL A2dp_Buffer_Notify(SOURCE source,U16 thd)
{
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    uint32_t sink_buffering_time;
    uint32_t source_buffering_sample_num = 0;
    uint32_t temp_read_offset = share_buff_info->ReadOffset;
    uint32_t threshold;
    bt_codec_a2dp_pcb_type_t a2dp_pcb_info;
    if ((source != NULL) && (source->transform != NULL) && (source->transform->sink != NULL))
    {
        sink_buffering_time = afe_get_dl1_query_data_amount()/(source->transform->sink->param.audio.rate/1000);
    }
    else
    {
        STRM_A2DP_LOG_E("[A2dp_Buffer_Notify] transform or sink is null", 0);
        return FALSE;
    }
    if((sink_buffering_time >= thd) || (share_buff_info->bBufferIsFull == TRUE))
    {
        return FALSE;
    }
    threshold = (thd - sink_buffering_time)  * share_buff_info->sample_rate / 1000;
    //Accumulate share buffer sample count.
    while(temp_read_offset != share_buff_info->WriteOffset)
    {
        DSP_C2D_BufferCopy((VOID*)  &a2dp_pcb_info,
                           (VOID*)  (share_buff_info->startaddr + temp_read_offset),
                           (U16)    sizeof(bt_codec_a2dp_pcb_type_t),
                           (VOID*)  share_buff_info->startaddr,
                           (U16)    share_buff_info->length);

        if(a2dp_pcb_info.state == PCB_STATE_USED) {
            if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
                if (temp_read_offset != share_buff_info->ReadOffset)
                {
                    source_buffering_sample_num += SBC_FRAME_DECODE_SAMPLE_NUM * n9_a2dp_param->totalFrameCnt;
                }
                else
                {
                    source_buffering_sample_num += SBC_FRAME_DECODE_SAMPLE_NUM * (n9_a2dp_param->totalFrameCnt - n9_a2dp_param->dropFrameCnt);
                }
            } else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC ) {
                source_buffering_sample_num += AAC_FRAME_DECODE_SAMPLE_NUM;
            } else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ) {
                #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
                if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33) {
                    source_buffering_sample_num += VEND_2_FRAME_DECODE_SAMPLE_NUM * n9_a2dp_param->totalFrameCnt;
                }
                else
                {
                    uint32_t vend_frame_sample_num = vend_get_frame_size(source);
                    source_buffering_sample_num += vend_frame_sample_num * n9_a2dp_param->totalFrameCnt;
                }
                #else
                uint32_t vend_frame_sample_num = vend_get_frame_size(source);
                source_buffering_sample_num += vend_frame_sample_num * n9_a2dp_param->totalFrameCnt;
                #endif
            } else {
                source_buffering_sample_num += 512;
            }
            if(source_buffering_sample_num >= threshold) {
                return FALSE;
            }
        } else if ((a2dp_pcb_info.state != PCB_STATE_LOST)&&(a2dp_pcb_info.state != PCB_STATE_SKIP)){
            STRM_A2DP_LOG_E("meet unexpect pcd state :%d Ro :%d", 2, a2dp_pcb_info.state,share_buff_info->ReadOffset);
            return TRUE;
        }
        temp_read_offset = (temp_read_offset + a2dp_pcb_info.size + 4) % share_buff_info->length;
    }
    return TRUE;
}
#if AUDIO_ASI_CNT
S32 local_asi_compromise(SOURCE source)
{
    S32 comp_sample_num = 0;
    
    //STRM_A2DP_LOG_I("r : %d s : %d seq :%d asi :%d", 4,source->param.n9_a2dp.sync_info->aws_role,source->param.n9_a2dp.sync_info->info_status,source->param.n9_a2dp.sync_info->cur_seqn,source->param.n9_a2dp.sync_info->cur_asi);
    //STRM_A2DP_LOG_I("local s: %d a:%d relay s:%d a:%d", 4,local_asi_comp.localSEQN,local_asi_comp.localASI,local_asi_comp.relaySEQN,local_asi_comp.relayASI);
    if ((source->param.n9_a2dp.sync_info->aws_role == BT_AWS_MCE_ROLE_AGENT) || (source->param.n9_a2dp.sync_info->aws_role == BT_AWS_MCE_ROLE_NONE))
    {
        if (source->param.n9_a2dp.sync_info->cur_seqn == source->param.n9_a2dp.current_seq_num)
        {
            //do nothing
        }
        else if (source->param.n9_a2dp.sync_info->info_status == INFO_STATUS_FREE)
        {
            source->param.n9_a2dp.sync_info->cur_seqn = source->param.n9_a2dp.current_seq_num;
            source->param.n9_a2dp.sync_info->cur_asi  = lt_clk_skew_get_local_asi();
            if ((source->param.n9_a2dp.sync_info->cur_seqn == source->param.n9_a2dp.current_seq_num) && (source->param.n9_a2dp.sync_info->cur_asi == lt_clk_skew_get_local_asi()))
            {
                //double read to prevent multi core read/write timing issue
                source->param.n9_a2dp.sync_info->info_status = INFO_STATUS_USED;
            }
        }
        else if(source->param.n9_a2dp.sync_info->info_status != INFO_STATUS_USED)
        {
            STRM_A2DP_LOG_I("Agent/None reset local ASI status", 0);
            source->param.n9_a2dp.sync_info->info_status = INFO_STATUS_FREE;
        }
    }
    else
    {
        if (((U16)source->param.n9_a2dp.current_seq_num == local_asi_comp.relaySEQN)&&(source->param.n9_a2dp.current_seq_num != local_asi_comp.localSEQN))
        {
            comp_sample_num = local_asi_comp.relayASI - lt_clk_skew_get_local_asi();
        }
        else if(source->param.n9_a2dp.sync_info->info_status == INFO_STATUS_USED)
        {
            if ((U16)source->param.n9_a2dp.current_seq_num == source->param.n9_a2dp.sync_info->cur_seqn)
            {
                comp_sample_num = source->param.n9_a2dp.sync_info->cur_asi - lt_clk_skew_get_local_asi();
            }
            else if (local_asi_comp.localSEQN == source->param.n9_a2dp.sync_info->cur_seqn)
            {
                comp_sample_num = source->param.n9_a2dp.sync_info->cur_asi - local_asi_comp.localASI;
                            }
            if (((S16)((U16)source->param.n9_a2dp.current_seq_num - local_asi_comp.relaySEQN) > 0)||((S16)((U16)source->param.n9_a2dp.current_seq_num - local_asi_comp.relaySEQN) < (S16)-40))
            {
                local_asi_comp.relaySEQN = source->param.n9_a2dp.sync_info->cur_seqn;
                local_asi_comp.relayASI = source->param.n9_a2dp.sync_info->cur_asi;
                source->param.n9_a2dp.sync_info->info_status = INFO_STATUS_FREE;
            }
            if ((S16)(source->param.n9_a2dp.sync_info->cur_seqn - local_asi_comp.localSEQN) > 0)
            {
                local_asi_comp.localSEQN = (U16)source->param.n9_a2dp.current_seq_num;
                local_asi_comp.localASI = lt_clk_skew_get_local_asi();
            }
        }
        else if(source->param.n9_a2dp.sync_info->info_status != INFO_STATUS_FREE)
        {
            STRM_A2DP_LOG_I("Partner/Client reset local ASI status", 0);
            source->param.n9_a2dp.sync_info->info_status = INFO_STATUS_FREE;
        }
    }
    if (comp_sample_num != 0)
    {
        if ((comp_sample_num > 96000) || (comp_sample_num < -96000))
        {
            STRM_A2DP_LOG_I("unexpected relay ASI, comp %d", 1,comp_sample_num);
            comp_sample_num = 0;
            local_asi_comp.relaySEQN = local_asi_comp.localSEQN;
            local_asi_comp.relayASI  = local_asi_comp.relayASI;
        }
        else
        {
            STRM_A2DP_LOG_I("r : %d s : %d seq :%d asi :%d", 4,source->param.n9_a2dp.sync_info->aws_role,source->param.n9_a2dp.sync_info->info_status,source->param.n9_a2dp.sync_info->cur_seqn,source->param.n9_a2dp.sync_info->cur_asi);
            STRM_A2DP_LOG_I("local s: %d a:%d relay s:%d a:%d", 4,local_asi_comp.localSEQN,local_asi_comp.localASI,local_asi_comp.relaySEQN,local_asi_comp.relayASI);
            STRM_A2DP_LOG_I("comp = %d", 1,comp_sample_num);
            local_asi_comp.localSEQN = (U16)source->param.n9_a2dp.current_seq_num;
            local_asi_comp.localASI  = lt_clk_skew_get_local_asi() + comp_sample_num;
        }
    }
    else
    {
        comp_sample_num = 0;
    }
    return comp_sample_num;
}

#endif
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL SourceReadBuf_N9_a2dp(SOURCE source, U8 *dst_addr, U32 length)
{
    STRM_A2DP_LOG_D("SourceReadBuf_N9_a2dp++\r\n", 0);

    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 read_offset = 0;

    /* SBC part */
    if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
        /* Fragmented frame */
        if ( n9_a2dp_param->fragment_flag == TRUE ) {
            U32 temp_readoffset = share_buff_info->ReadOffset;
            U32 temp_bitstream_size = 0;
            bt_codec_a2dp_pcb_type_t sbc_last_pcb_info;

            /* Moving the fragmented-frame to destination address, except the last one */
            for ( U16 i = 0; i < (n9_a2dp_param->totalFrameCnt - 1); i++ ) {
                if( n9_a2dp_param->cp_exist == TRUE ) {
                    read_offset = temp_readoffset;
                    if ( (temp_readoffset + 4 + 12 + 1 + 1) >= share_buff_info->length ) {
                        read_offset = (temp_readoffset + 4 + 12 + 1 + 1 - share_buff_info->length);
                    }
                    else {
                        read_offset = (temp_readoffset + 4 + 12 + 1 + 1);
                    }
                    DSP_C2D_BufferCopy((VOID*)  (dst_addr + read_offset - share_buff_info->ReadOffset),
                                       (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte, pl header: 1 byte*/
                                       (U16)    (n9_a2dp_param->current_bitstream_size - 12 - 1),
                                       (VOID*)  share_buff_info->startaddr,
                                       (U16)    share_buff_info->length);
                }
                else {
                    read_offset = temp_readoffset;
                    if ( (temp_readoffset + 4 + 12 + 1) >= share_buff_info->length ) {
                        read_offset = (temp_readoffset + 4 + 12 + 1 - share_buff_info->length);
                    }
                    else {
                        read_offset = (temp_readoffset + 4 + 12 + 1);
                    }
                    DSP_C2D_BufferCopy((VOID*)  (dst_addr + read_offset - share_buff_info->ReadOffset),
                                       (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, pl header: 1 byte*/
                                       (U16)    (n9_a2dp_param->current_bitstream_size - 12),
                                       (VOID*)  share_buff_info->startaddr,
                                       (U16)    share_buff_info->length);
                }
                temp_readoffset += n9_a2dp_param->current_packet_size;
                if ( temp_readoffset >= share_buff_info->length ) {
                    temp_readoffset -= share_buff_info->length;
                }
            }
            /* Get the last one packet size */
            DSP_C2D_BufferCopy((VOID*)  &sbc_last_pcb_info,
                               (VOID*)  (share_buff_info->startaddr + temp_readoffset),
                               (U16)    sizeof(bt_codec_a2dp_pcb_type_t),
                               (VOID*)  share_buff_info->startaddr,
                               (U16)    share_buff_info->length);
            /* Save global information */
            {
                temp_bitstream_size = (U32)sbc_last_pcb_info.size - (U32)sbc_last_pcb_info.data_size;
            }
            /* Moving the last one fragmented-frame to destination address */
            if( n9_a2dp_param->cp_exist == TRUE ) {
                read_offset = temp_readoffset;
                if ( (temp_readoffset + 4 + 12 + 1 + 1) >= share_buff_info->length ) {
                    read_offset = (temp_readoffset + 4 + 12 + 1 + 1 - share_buff_info->length);
                }
                else {
                    read_offset = (temp_readoffset + 4 + 12 + 1 + 1);
                }
                DSP_C2D_BufferCopy((VOID*)  (dst_addr + read_offset - share_buff_info->ReadOffset),
                                   (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte, pl header: 1 byte */
                                   (U16)    (temp_bitstream_size - 12 - 1 ),
                                   (VOID*)  share_buff_info->startaddr,
                                   (U16)    share_buff_info->length);
            }
            else {
                read_offset = temp_readoffset;
                if ( (temp_readoffset + 4 + 12 + 1) >= share_buff_info->length ) {
                    read_offset = (temp_readoffset + 4 + 12 + 1 - share_buff_info->length);
                }
                else {
                    read_offset = (temp_readoffset + 4 + 12 + 1);
                }
                DSP_C2D_BufferCopy((VOID*)  (dst_addr + read_offset - share_buff_info->ReadOffset),
                                   (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, pl header: 1 byte */
                                   (U16)    (temp_bitstream_size - 12),
                                   (VOID*)  share_buff_info->startaddr,
                                   (U16)    share_buff_info->length);
            }
        }
        /* Non-fragmented frame */
        else {
            if( n9_a2dp_param->cp_exist == TRUE ) {
                read_offset = share_buff_info->ReadOffset;
                if ( (share_buff_info->ReadOffset + 4 + 12 + 1 + 1 + n9_a2dp_param->readOffset) >= share_buff_info->length ) {
                    read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1 + 1 + n9_a2dp_param->readOffset - share_buff_info->length);
                }
                else {
                    read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1 + 1 + n9_a2dp_param->readOffset);
                }
                DSP_C2D_BufferCopy((VOID*)  dst_addr,
                                   (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte, pl header: 1 byte */
                                   (U16)    length,
                                   (VOID*)  share_buff_info->startaddr,
                                   (U16)    share_buff_info->length);
            }
            else {
                read_offset = share_buff_info->ReadOffset;
                if ( (share_buff_info->ReadOffset + 4 + 12 + 1 + n9_a2dp_param->readOffset) >= share_buff_info->length ) {
                    read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1 + n9_a2dp_param->readOffset - share_buff_info->length);
                }
                else {
                    read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1 + n9_a2dp_param->readOffset);
                }
                DSP_C2D_BufferCopy((VOID*)  dst_addr,
                                   (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, pl header: 1 byte */
                                   (U16)    length,
                                   (VOID*)  share_buff_info->startaddr,
                                   (U16)    share_buff_info->length);
            }
        }
    }
    /* AAC part */
    else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC ) {
        if( n9_a2dp_param->cp_exist == TRUE ) {
            read_offset = share_buff_info->ReadOffset;
            if ( (share_buff_info->ReadOffset + 4 + 12 + 1) >= share_buff_info->length ) {
                read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1 - share_buff_info->length);
            }
            else {
                read_offset = (share_buff_info->ReadOffset + 4 + 12 + 1);
            }
            DSP_C2D_BufferCopy((VOID*)  dst_addr,
                               (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte, cp:1 byte */
                               (U16)    length,
                               (VOID*)  share_buff_info->startaddr,
                               (U16)    share_buff_info->length);
        }
        else {
            read_offset = share_buff_info->ReadOffset;
            if ( (share_buff_info->ReadOffset + 4 + 12) >= share_buff_info->length ) {
                read_offset = (share_buff_info->ReadOffset + 4 + 12 - share_buff_info->length);
            }
            else {
                read_offset = (share_buff_info->ReadOffset + 4 + 12);
            }
            DSP_C2D_BufferCopy((VOID*)  dst_addr,
                               (VOID*)  (share_buff_info->startaddr + read_offset),/* pcb:4 byte, rtp:12 byte */
                               (U16)    length,
                               (VOID*)  share_buff_info->startaddr,
                               (U16)    share_buff_info->length);
        }
    }
    /* VENDOR part */
    else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR )
    {
        #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
        if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33)
        {
            vend_read_frame_from_packet(source,dst_addr,length,VEND_2_PAYLOAD_HEADER);
        }
        else
        #endif
        {
	    #ifndef MTK_BT_A2DP_VENDOR_BC_ENABLE
        vend_read_frame_from_packet(source,dst_addr,length,PAYLOAD_HEADER);
        #else
        if (source->param.n9_a2dp.sink_latency != 0)
        {
            vend_read_frame_from_packet(source,dst_addr,length,PAYLOAD_HEADER);
        }
        else if (p_vend_bc_exbuf_if->read_drop_flag == FALSE)
        {
            vend_bc_read(p_vend_bc_exbuf_if->p_handle, dst_addr, length);
            memcpy(p_vend_bc_exbuf_if->bc_temp_read_buf,dst_addr,length);
            p_vend_bc_exbuf_if->read_drop_flag = TRUE;
        }
        else
        {
            memcpy(dst_addr,p_vend_bc_exbuf_if->bc_temp_read_buf,length);
        }
		#endif
        }
    }
    else {
        //Not support codec type
    }

    STRM_A2DP_LOG_D("SourceReadBuf_N9_a2dp--\r\n", 0);
    return TRUE;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 U32 SourceSize_N9_a2dp(SOURCE source)
{
    STRM_A2DP_LOG_D("SourceSize_N9_a2dp++\r\n", 0);

    U32 frame_size = 0;
    ltcs_bt_type_t codec_type = LTCS_TYPE_OTHERS;
    U16 dsp_notify_thd;
    U8 pcb_parse_state = RETURN_FAIL;
    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 mask;

    /* update share information data */
    update_from_share_information(source);

    #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
    if ((n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR) && (source->param.n9_a2dp.sink_latency == 0)){
        vend_bc_write_loop_callback();
        if (source->param.n9_a2dp.DspReportStartId != 0xFFFF)
        {
            SourceDrop_N9_a2dp(source,0);
        }
        U16 ret;
        ret = vend_bc_read(p_vend_bc_exbuf_if->p_handle, NULL, 0);//vendor BC
        if (ret == 0)
        {
            if (p_vend_bc_exbuf_if->read_drop_flag == TRUE)
            {
                ret = n9_a2dp_param->current_frame_size;
            }
            else
            {
                if (vend_bc_read(p_vend_bc_exbuf_if->p_handle, p_vend_bc_exbuf_if->bc_temp_read_buf, n9_a2dp_param->current_frame_size) >0){
                    p_vend_bc_exbuf_if->read_drop_flag = TRUE;
                }
            }
        }
        else
        {
            n9_a2dp_param->current_frame_size = ret;
        }
        return ret;
    }
    #endif


    #ifdef MTK_PEQ_ENABLE
    PEQ_Update_Info(share_buff_info->anchor_clk, n9_a2dp_param->predict_timestamp);
    #endif

    STRM_A2DP_LOG_D("bBufferIsFull: %d, ReadOffset: %d, WriteOffset: %d, readOffset: %d\r\n", 4, share_buff_info->bBufferIsFull, share_buff_info->ReadOffset, share_buff_info->WriteOffset, n9_a2dp_param->readOffset);
    /* Check there is underflow or not */
    if (DSP_Callback_BypassModeGet(source,source->transform->sink)==BYPASS_CODEC_MODE && n9_a2dp_param->mce_flag == TRUE){
        return n9_a2dp_param->current_frame_size;
    }
    #if AUDIO_ASI_CNT
    else if(local_asi_comp.comp_val > 0)
    {
        DSP_Callback_BypassModeCtrl(source,source->transform->sink,BYPASS_CODEC_MODE);
        if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
            frame_size = AAC_FRAME_DECODE_SAMPLE_NUM;//sample
        }
        else if (n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC){
            frame_size = SBC_FRAME_DECODE_SAMPLE_NUM;
        }
        else if (n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR){
            frame_size = vend_get_frame_size(source);//sample
        }
        #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
        else if (n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR_2){
            frame_size = VEND_2_FRAME_DECODE_SAMPLE_NUM;//sample
        }
        #endif
        n9_a2dp_param->current_frame_size = frame_size;
        local_asi_comp.comp_val -= frame_size;
        STRM_A2DP_LOG_I("local asi comp : %d",1,local_asi_comp.comp_val);
        if (local_asi_comp.comp_val < 0)
        {
            U32 buffer_per_channel_shift = ((source->transform->sink->param.audio.channel_num>=2) && (source->transform->sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                                     ? 1
                                     : 0;
            lt_clk_skew_accumulate_local_asi(local_asi_comp.comp_val);
            source->transform->sink->param.audio.sram_empty_fill_size -= local_asi_comp.comp_val<<(2+buffer_per_channel_shift);//temp us 2ch + 32 bit res
            local_asi_comp.comp_val = 0;
        }
        return frame_size;
    } 
    #endif
    dsp_notify_thd = ((source->param.n9_a2dp.sink_latency/1000)  > (NOTIFY_LOW_BOND + NOTIFY_LOW_THD)) ? ((source->param.n9_a2dp.sink_latency/1000) - NOTIFY_LOW_THD) : NOTIFY_LOW_BOND;
    ((SHARE_BUFFER_INFO *)source->param.n9_a2dp.share_info_base_addr)->notify_count = A2dp_Buffer_Notify(source,dsp_notify_thd);
    if(sbc_lost_packet_handler(source) == TRUE) {
        return n9_a2dp_param->current_frame_size;
    }
    if ((share_buff_info->ReadOffset != share_buff_info->WriteOffset)||(share_buff_info->bBufferIsFull == TRUE))
    {
        pcb_parse_state = a2dp_get_pcb_info(source);
    }
    if ( share_buff_info->bBufferIsFull == 0 &&
         share_buff_info->ReadOffset == share_buff_info->WriteOffset //&& (frame_size!= 0)
         /* SRAM data size < THD */ ) {
         if ((source->transform != NULL) && (n9_a2dp_param->mce_flag == TRUE) && (source->transform->sink->type == SINK_TYPE_AUDIO) ){
            MACRO_CHK_FIRST_PKT(n9_a2dp_param,"buffer empty");
            if (afe_get_dl1_query_data_amount() > (2*(source->transform->sink->param.audio.rate * source->transform->sink->param.audio.period / 1000))){
                //do nothing and wait for next IRQ
            }
            else {
                if (afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) ==0)
                {
                    STRM_A2DP_LOG_I("skip enter BCM buffer empty before play en , pt:%d  t:%d ro: %d", 3, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp,share_buff_info->ReadOffset);
                    if (n9_a2dp_param->DspReportStartId != 0xFFFF)
                    {
                        SourceDrop_N9_a2dp(source,0);
                    }
                    return 0;
                }
                STRM_A2DP_LOG_I("enter BCM buffer empty , pt:%d  t:%d ro: %d", 3, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp,share_buff_info->ReadOffset);
                if(n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG){
                        if (afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) !=0)
                        {
                            n9_a2dp_param->buffer_empty_cnt++;
                        }
                }
                DSP_Callback_BypassModeCtrl(source,source->transform->sink,BYPASS_CODEC_MODE);
                if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
                    frame_size = AAC_FRAME_DECODE_SAMPLE_NUM;//sample
                }
                else if (n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC){
                    frame_size = SBC_FRAME_DECODE_SAMPLE_NUM;
                    if(n9_a2dp_param->ios_sbc_flag == TRUE) {
                        n9_a2dp_param->u2OverPadFrameCnt++;
                    }
                }
                else if (n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR){
                    #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
                    if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33){
                        frame_size = VEND_2_FRAME_DECODE_SAMPLE_NUM;//sample
                    }
                    else
                    #endif
                    {
                        frame_size = vend_get_frame_size(source);//sample
                    }
                }

                n9_a2dp_param->current_frame_size = frame_size;
            }
            return frame_size;
         }
    }
    /* Check there is data in share buffer or not */
    else if ( share_buff_info->bBufferIsFull ||
              share_buff_info->ReadOffset != share_buff_info->WriteOffset ||
              n9_a2dp_param->readOffset != 0 ) {
        /* Get new packet */
        if ( n9_a2dp_param->readOffset == 0 ) {
            bt_codec_a2dp_rtpheader_t a2dp_rtpheader_info;
            bt_codec_a2dp_sbc_pl_header_t pl_info;

            /* Get pcb information */
            if (pcb_parse_state == RETURN_PASS)
            {
                a2dp_get_rtpheader_info(source, &a2dp_rtpheader_info);
            }
            else
            {
                MACRO_CHK_FIRST_PKT(n9_a2dp_param,"packet state LOST");
                if (n9_a2dp_param->pkt_lost_report_state == PL_STATE_REPORTPREVSEQN)
                {
                    U32 gpt_timer;
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
                    STRM_A2DP_LOG_I("A2DP meet packet loss state,Last PKT SEQN:%d timer: %d", 2, n9_a2dp_param->current_seq_num,gpt_timer);
                    n9_a2dp_param->pkt_lost_report_state = PL_STATE_REPORTNEXTSEQN;
                }
                return 0;
            }

            if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
                codec_type = LTCS_TYPE_AAC;
            } else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) {
                codec_type = LTCS_TYPE_SBC;
                if(n9_a2dp_param->ios_sbc_flag == TRUE) {
                    codec_type = LTCS_TYPE_IOS_SBC;
                    a2dp_rtpheader_info.timestamp = n9_a2dp_param->totalFrameCnt;
                }
            } else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR) {
                #if AUDIO_ASI_CNT
                #ifdef MTK_BT_A2DP_VENDOR_1_ENABLE
                codec_type = LTCS_TYPE_DSP_CNT;
                #else
                codec_type = LTCS_TYPE_VENDOR;
                #endif
                #else
                codec_type = LTCS_TYPE_VENDOR;
                #endif
            }
            if ((a2dp_rtpheader_info.timestamp == 0) && (n9_a2dp_param->ios_sbc_flag == TRUE))
            {
                STRM_A2DP_LOG_I("SBC frame number = 0 case, seqn : ", 1,n9_a2dp_param->current_seq_num,share_buff_info->ReadOffset);
            }
            lt_clk_skew_gather_pkt_info(a2dp_rtpheader_info.ssrc,
                            a2dp_rtpheader_info.timestamp,
                            (U32)a2dp_rtpheader_info.seqno,
                            codec_type);
            lt_clk_skew_set_ts_ratio(n9_a2dp_param->timestamp_ratio);

            hal_nvic_save_and_set_interrupt_mask(&mask);
            lt_clk_skew_set_drift_value(share_buff_info->drift_comp_val);
            lt_clk_skew_update_base_asi(share_buff_info->asi_base, share_buff_info->asi_current);
            hal_nvic_restore_interrupt_mask(mask);
            lt_clk_skew_update_anchor_clock(share_buff_info->anchor_clk);


            lt_clk_skew_gather_regression_info();
            #if AUDIO_ASI_CNT
            if (n9_a2dp_param->asi_sync_en == TRUE){
                //use local asi
                local_asi_comp.comp_val = local_asi_compromise(source);
                if (local_asi_comp.comp_val < 0)
                {
                    
                    U32 buffer_per_channel_shift = ((source->transform->sink->param.audio.channel_num>=2) && (source->transform->sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                                                        ? 1
                                                        : 0;
                    lt_clk_skew_accumulate_local_asi(local_asi_comp.comp_val);
                    source->transform->sink->param.audio.sram_empty_fill_size -= local_asi_comp.comp_val<<(2+buffer_per_channel_shift);//temp us 2ch + 32 bit res
                    local_asi_comp.comp_val = 0;
                }
                else if(local_asi_comp.comp_val != 0)
                {
                    return 0;
                }
            }
            if (( n9_a2dp_param->predict_timestamp == n9_a2dp_param->current_timestamp )||(!((source->transform != NULL) && (n9_a2dp_param->mce_flag == TRUE) && (source->transform->sink->type == SINK_TYPE_AUDIO) ))||(n9_a2dp_param->asi_sync_en == TRUE)) {
            #else
            if (( n9_a2dp_param->predict_timestamp == n9_a2dp_param->current_timestamp )||(!((source->transform != NULL) && (n9_a2dp_param->mce_flag == TRUE) && (source->transform->sink->type == SINK_TYPE_AUDIO) ))) {
            #endif
                //do nothing
            }
            else if ( (S32)(n9_a2dp_param->predict_timestamp - n9_a2dp_param->current_timestamp) < 0 ) {
                bt_codec_a2dp_rtpheader_t next_rtp;
                BOOL packet_skip_flag;
                MACRO_CHK_FIRST_PKT(n9_a2dp_param,"packet loss");
                next_rtp = a2dp_get_next_rtp_info(source);
                if(n9_a2dp_param->ios_sbc_flag == TRUE) {
                    if ((next_rtp.ssrc != SSRC_MAGIC_NUM) && ((next_rtp.seqno - n9_a2dp_param->current_timestamp) > BUFFER_EMPTY_REINIT_THD))
                    {
                        hal_nvic_save_and_set_interrupt_mask(&mask);
                        share_buff_info->ReadOffset = (share_buff_info->ReadOffset + n9_a2dp_param->current_packet_size)%share_buff_info->length;
                        source->streamBuffer.ShareBufferInfo.bBufferIsFull = FALSE;
                        update_to_share_information(source);
                        hal_nvic_restore_interrupt_mask(mask);
                        STRM_A2DP_LOG_W("Skip abnormal packet pt:%d  t:%d next t:%d", 3, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp, next_rtp.seqno);
                        DSP_Callback_BypassModeCtrl(source,source->transform->sink,BYPASS_CODEC_MODE);
                        n9_a2dp_param->current_frame_size = SBC_FRAME_DECODE_SAMPLE_NUM;
                        n9_a2dp_param->u2OverPadFrameCnt++;
                        return n9_a2dp_param->current_frame_size;
                    }
                    if(sbc_register_lost_packet(source) == RETURN_PASS) {
                        //do nothing
                    } else {
                        return n9_a2dp_param->current_frame_size;
                    }
                }
                else
                {
                    if (n9_a2dp_param->ios_aac_flag == TRUE){
                        packet_skip_flag = ((next_rtp.ssrc != SSRC_MAGIC_NUM) && ((next_rtp.seqno - n9_a2dp_param->current_timestamp) > BUFFER_EMPTY_REINIT_THD)) ? TRUE : FALSE; 
                    }
                    else{
                        packet_skip_flag = ((next_rtp.ssrc != SSRC_MAGIC_NUM) && ((next_rtp.timestamp - n9_a2dp_param->current_timestamp) > AAC_FRAME_DECODE_SAMPLE_NUM*BUFFER_EMPTY_REINIT_THD*n9_a2dp_param->timestamp_ratio)) ? TRUE
                                                : (((n9_a2dp_param->current_timestamp - n9_a2dp_param->predict_timestamp)%SBC_FRAME_DECODE_SAMPLE_NUM) != 0) ? TRUE : FALSE; 
                    }
                    if (packet_skip_flag){
                        hal_nvic_save_and_set_interrupt_mask(&mask);
                        share_buff_info->ReadOffset = (share_buff_info->ReadOffset + n9_a2dp_param->current_packet_size)%share_buff_info->length;
                        source->streamBuffer.ShareBufferInfo.bBufferIsFull = FALSE;
                        update_to_share_information(source);
                        hal_nvic_restore_interrupt_mask(mask);
                        STRM_A2DP_LOG_W("Skip abnormal packet pt:%d  t:%d", 2, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp);
                    }
                    STRM_A2DP_LOG_I("enter BCM packet loss, pt:%d Jump to t:%d (seqn:%d) Wo:%d Ro:%d", 5, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp,n9_a2dp_param->current_seq_num,share_buff_info->WriteOffset,share_buff_info->ReadOffset);
                    if(n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG){
                        if (afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) !=0)
                        {
                            n9_a2dp_param->buffer_empty_cnt++;
                        }
                    }
                    if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
                        if (source->param.n9_a2dp.prev_seq_num == (U32)((U16)source->param.n9_a2dp.current_seq_num -1))
                        {
                            STRM_A2DP_LOG_I("TS incontinous AAC detect when packet loss",0);
                        }
                        frame_size = AAC_FRAME_DECODE_SAMPLE_NUM;//sample -> byte
                    }
                    else if (n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC){
                        if (source->param.n9_a2dp.prev_seq_num == (U32)((U16)source->param.n9_a2dp.current_seq_num -1))
                        {
                            STRM_A2DP_LOG_I("Variant SBC detect when packet loss",0);
                        }
                        frame_size = SBC_FRAME_DECODE_SAMPLE_NUM;
                    }
                    else if (n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR){
                        #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
                        if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33){
                            frame_size = VEND_2_FRAME_DECODE_SAMPLE_NUM;//sample
                        }
                        else
                        #endif
                        {
                            frame_size = vend_get_frame_size(source);//sample
                        }
                    }
                    DSP_Callback_BypassModeCtrl(source,source->transform->sink,BYPASS_CODEC_MODE);
                    n9_a2dp_param->current_frame_size = frame_size;
                    return frame_size;
                }
            }
            else {
                a2dp_skip_expired_packet(source);
                if ( n9_a2dp_param->predict_timestamp == n9_a2dp_param->current_timestamp ) {
                        //do nothing
                }
                else {
                    if (afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) ==0)
                    {
                        STRM_A2DP_LOG_I("Skip enter BCM packet loss before play en pt:%d  t:%d",2, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp);
                         return 0;
                    }
                    STRM_A2DP_LOG_I("enter BCM packet loss(drop past) pt:%d  t:%d", 2, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp);
                    if(n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG){
                        if (afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) !=0)
                        {
                            n9_a2dp_param->buffer_empty_cnt++;
                        }
                    }
                    if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
                        frame_size = AAC_FRAME_DECODE_SAMPLE_NUM;//sample
                        n9_a2dp_param->current_frame_size = frame_size;
                    }
                    else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC){
                        frame_size = SBC_FRAME_DECODE_SAMPLE_NUM;//sample 
                        n9_a2dp_param->current_frame_size = frame_size;
                        if(n9_a2dp_param->ios_sbc_flag == TRUE) {
                            n9_a2dp_param->u2OverPadFrameCnt++;
                        }
                    }
                    else if (n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR){
                        #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
                        if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33){
                            frame_size = VEND_2_FRAME_DECODE_SAMPLE_NUM;//sample
                        }
                        else
                        #endif
                        {
                            frame_size = vend_get_frame_size(source);//sample
                        }
                        n9_a2dp_param->current_frame_size = frame_size;
                    }
                    DSP_Callback_BypassModeCtrl(source,source->transform->sink,BYPASS_CODEC_MODE);
                    return frame_size;
                }
            }

            /* Get payload header information */
            if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
                sbc_get_payload_information(source, &pl_info);
                if(sbc_check_overpad(source) == RETURN_PASS) {
                    //do nothing
                } else {
                    if (afe_get_dl1_query_data_amount() > (2*(source->transform->sink->param.audio.rate * source->transform->sink->param.audio.period / 1000))){
                        //printf("[sbc_check_overpad] sram data is enough, %d > %d samples\n",afe_get_dl1_query_data_amount(),(2*(source->transform->sink->param.audio.rate * source->transform->sink->param.audio.period / 1000)));
                        return 0;
                    }
                    else {
                        STRM_A2DP_LOG_I("enter BCM check over pad , pt:%d t:%d  wo:%d ro:%d\n", 4, n9_a2dp_param->predict_timestamp, n9_a2dp_param->current_timestamp,share_buff_info->WriteOffset,share_buff_info->ReadOffset);
                        DSP_Callback_BypassModeCtrl(source,source->transform->sink,BYPASS_CODEC_MODE);
                        n9_a2dp_param->current_frame_size = SBC_FRAME_DECODE_SAMPLE_NUM;
                        n9_a2dp_param->u2OverPadFrameCnt++;
                        return n9_a2dp_param->current_frame_size;
                    }
                }
            }
            else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ){
                #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
                if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33){
                    vend_2_get_payload_information(source, (bt_codec_a2dp_vend_2_pl_header_t *)&pl_info);
                }
                else
                #endif
                {
                    vend_get_payload_information(source, (bt_codec_a2dp_vend_pl_header_t *)&pl_info);
                }
            }
        }

        /* SBC part */
        if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
            bt_codec_a2dp_sbc_frame_header_t sbc_frame_info;
            /* Get sbc frame information */
            sbc_get_frame_information(source, &sbc_frame_info);
            if ( sbc_frame_info.SyncWord != 0x9C )
            {
                STRM_A2DP_LOG_I("Skip abnormal sbc packet Wo %d Ro %d psize %d ts %d pts %d", 5, share_buff_info->WriteOffset,share_buff_info->ReadOffset,n9_a2dp_param->current_packet_size,n9_a2dp_param->current_timestamp, n9_a2dp_param->predict_timestamp);
                hal_nvic_save_and_set_interrupt_mask(&mask);
                share_buff_info->ReadOffset = (share_buff_info->ReadOffset + n9_a2dp_param->current_packet_size)%share_buff_info->length;
                source->streamBuffer.ShareBufferInfo.bBufferIsFull = FALSE;
                n9_a2dp_param->readOffset = 0;
                update_to_share_information(source);
                if (n9_a2dp_param->ios_sbc_flag == TRUE) {
                    n9_a2dp_param->u2OverPadFrameCnt += n9_a2dp_param->dropFrameCnt;
                }
                else{
                    n9_a2dp_param->prev_seq_num--; // prevent enter sbc_variant_frame_n case
                }
                hal_nvic_restore_interrupt_mask(mask);
                return SourceSize_N9_a2dp(source);
            }
            /* Calculating frame size */
            n9_a2dp_param->current_frame_size = 0;
            frame_size = sbc_frame_size_calc(&sbc_frame_info);
            /* Fragmented frame */
            if ( n9_a2dp_param->fragment_flag == TRUE ) {
                /* Pre-read next packet(s) for checking data enough or not to form a complete frame*/
                {
                    U32 temp_readoffset = share_buff_info->ReadOffset;

                    /* all fragmented packets, except the last one, shall have the same total packet size */
                    temp_readoffset += (n9_a2dp_param->current_packet_size * (n9_a2dp_param->totalFrameCnt - 1));
                    if ( temp_readoffset >= share_buff_info->length ) {
                        temp_readoffset -= share_buff_info->length;
                    }
                    if ( temp_readoffset >= share_buff_info->WriteOffset ) {
                        frame_size = 0;
                        n9_a2dp_param->current_frame_size = frame_size;
                    }
                    else {
                        U32 temp_packet_size = 0;
                        bt_codec_a2dp_pcb_type_t sbc_last_pcb_info;

                        /* Get the last one packet size */
                        DSP_C2D_BufferCopy((VOID*)  &sbc_last_pcb_info,
                                           (VOID*)  (share_buff_info->startaddr + temp_readoffset),
                                           (U16)    sizeof(bt_codec_a2dp_pcb_type_t),
                                           (VOID*)  share_buff_info->startaddr,
                                           (U16)    share_buff_info->length);
                        /* Save global information */
                        {
                            temp_packet_size = (U32)sbc_last_pcb_info.size + 4;
                        }
                        temp_readoffset += temp_packet_size;

                        if ( temp_readoffset > share_buff_info->WriteOffset ) {
                            frame_size = 0;
                            n9_a2dp_param->current_frame_size = frame_size;
                        }
                        else {
                            n9_a2dp_param->current_frame_size = frame_size;
                        }
                    }
                }
            }
            /* Non-fragmented frame */
            else {
                n9_a2dp_param->current_frame_size = frame_size;
            }
        }
        /* AAC part */
        else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC ) {
            if( n9_a2dp_param->cp_exist == TRUE ) {
                frame_size = (n9_a2dp_param->current_bitstream_size - 12 - 1);
            }
            else {
                frame_size = (n9_a2dp_param->current_bitstream_size - 12);
            }
            n9_a2dp_param->current_frame_size = frame_size;
        }
        /* VENDOR part */
        else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ){
            #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
            if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33){
                 bt_codec_a2dp_vend_2_frame_header_t vend_2_frame_info;
                /* Get sbc frame information */
                vend_get_frame_information(source, &vend_2_frame_info.byte_alloc,VEND_2_PAYLOAD_HEADER,sizeof(bt_codec_a2dp_vend_2_frame_header_t));
                frame_size = vend_2_frame_size_calc(vend_2_frame_info.vend_header);
                if ( vend_2_frame_info.byte_alloc.vend_byte4 != 0x4C )
                {
                    STRM_A2DP_LOG_E("Vend frame syncword abnormal P:%d F:%d S:%d ", 3 ,n9_a2dp_param->current_packet_size,frame_size,vend_2_frame_info.byte_alloc.vend_byte4);
                    hal_nvic_save_and_set_interrupt_mask(&mask);
                    share_buff_info->ReadOffset = (share_buff_info->ReadOffset + n9_a2dp_param->current_packet_size)%share_buff_info->length;
                    source->streamBuffer.ShareBufferInfo.bBufferIsFull = FALSE;
                    n9_a2dp_param->readOffset = 0;
                    update_to_share_information(source);
                    hal_nvic_restore_interrupt_mask(mask);
                    return SourceSize_N9_a2dp(source);
                }
                /* Calculating frame size */
                n9_a2dp_param->current_frame_size = frame_size;
            }
            else
            #endif
            {
                bt_codec_a2dp_vend_frame_header_t vend_frame_info;
                /* Get sbc frame information */
                vend_get_frame_information(source, &vend_frame_info,PAYLOAD_HEADER,sizeof(bt_codec_a2dp_vend_frame_header_t));
                if ( vend_frame_info.vend_byte1 != 0xAA )
                {
                    STRM_A2DP_LOG_I("Skip abnormal vendor codec packet Wo %d Ro %d psize %d ts %d pts %d", 5, share_buff_info->WriteOffset,share_buff_info->ReadOffset,n9_a2dp_param->current_packet_size,n9_a2dp_param->current_timestamp, n9_a2dp_param->predict_timestamp);
                    hal_nvic_save_and_set_interrupt_mask(&mask);
                    share_buff_info->ReadOffset = (share_buff_info->ReadOffset + n9_a2dp_param->current_packet_size)%share_buff_info->length;
                    source->streamBuffer.ShareBufferInfo.bBufferIsFull = FALSE;
                    n9_a2dp_param->readOffset = 0;
                    update_to_share_information(source);
                    hal_nvic_restore_interrupt_mask(mask);
                    return SourceSize_N9_a2dp(source);
                }
                /* Calculating frame size */
                frame_size = vend_frame_size_calc(&vend_frame_info);
                n9_a2dp_param->current_frame_size = frame_size;
            }
        }
    }
    else {
        frame_size = 0;
        n9_a2dp_param->current_frame_size = frame_size;
    }
    //STRM_A2DP_LOG_I("A2DP frm sz:%d pkt sz:%d cp:%d", 3, frame_size,n9_a2dp_param->current_packet_size,n9_a2dp_param->cp_exist);
    STRM_A2DP_LOG_D("SourceSize_N9_a2dp--, frm size: %d\r\n",1, frame_size);
    return frame_size;
}

U8* SourceMap_N9_a2dp(SOURCE source)
{
    UNUSED(source);
    return NULL;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 VOID SourceDrop_N9_a2dp(SOURCE source, U32 amount)
{
    STRM_A2DP_LOG_D("SourceDrop_N9_a2dp++\r\n", 0);

    SHARE_BUFFER_INFO *share_buff_info  = &(source->streamBuffer.ShareBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U32 read_write_space = 0;
    U32 mask;
    U16 reinit_thd = (n9_a2dp_param->codec_info.codec_cap.type != BT_A2DP_CODEC_AAC) ? BUFFER_EMPTY_REINIT_THD<<3 : BUFFER_EMPTY_REINIT_THD;
    BOOL buffer_notify;
    if(share_buff_info->sample_rate>48000)
    {
        reinit_thd = reinit_thd*(share_buff_info->sample_rate/48000);
    }
    if ( share_buff_info->ReadOffset > share_buff_info->WriteOffset ) {
        read_write_space = share_buff_info->ReadOffset - share_buff_info->WriteOffset;
    }
    else {
        read_write_space = share_buff_info->length - share_buff_info->WriteOffset + share_buff_info->ReadOffset;
    }
    if (source->param.n9_a2dp.DspReportStartId != 0xFFFF)
    {
        U32 gpt_timer;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
        aud_msg_ack(source->param.n9_a2dp.DspReportStartId, FALSE);
        STRM_A2DP_LOG_I("Ack A2DP start ID :%d Time :%d", 2, source->param.n9_a2dp.DspReportStartId,gpt_timer);
        source->param.n9_a2dp.DspReportStartId = 0xFFFF;
        n9_a2dp_param->buffer_empty_cnt = REINIT_FLAG;
        n9_a2dp_param->resend_reinit_req = FALSE;
        #ifdef MTK_BT_A2DP_VENDOR_1_ENABLE
        if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR){
            n9_a2dp_param->mce_flag = FALSE;
            n9_a2dp_param->ios_sbc_flag = FALSE;
            n9_a2dp_param->ios_aac_flag = FALSE;
            n9_a2dp_param->alc_monitor = FALSE;
        }
        #endif
    }
    if(amount == 0) {
        return;
    }
    if (((n9_a2dp_param->buffer_empty_cnt > (reinit_thd*(!source->param.n9_a2dp.alc_monitor)))
        ||(share_buff_info->bBufferIsFull == TRUE))&&(n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG)){
        U32 read_reg = afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1);
        if (read_reg != 0){
            //afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN1, 0);
            STRM_A2DP_LOG_I("Buffer status trigger re-sync empty_cnt:%d bf : %d", 2, n9_a2dp_param->buffer_empty_cnt,share_buff_info->bBufferIsFull);
            #if (ADATIVE_LATENCY_CTRL)
            if (source->param.n9_a2dp.alc_monitor)
            {
                if (source->param.n9_a2dp.latency_monitor == FALSE)
                {
                    STRM_A2DP_LOG_I("ALC request Latency : %d", 1, ADATIVE_LATENCY);
                    Au_DL_send_alc_request(ADATIVE_LATENCY);
                }
                else
                {
                    STRM_A2DP_LOG_I("Gaming mode meet reinit: %d",0);
                    Au_DL_send_reinit_request(MSG2_DSP2CN4_REINIT_AFE_ABNORMAL);
                }
            }
            else
            {
                STRM_A2DP_LOG_I("Buf abnormal montior flag: %d", 1, source->param.n9_a2dp.latency_monitor);
                Au_DL_send_reinit_request(MSG2_DSP2CN4_REINIT_BUF_ABNORMAL);
            }
            #else
            if(source->param.n9_a2dp.latency_monitor){
                Au_DL_send_reinit_request(MSG2_DSP2CN4_REINIT_BUF_ABNORMAL);
            }
            #endif
            n9_a2dp_param->buffer_empty_cnt = REINIT_FLAG;
        }
        else
        {
            n9_a2dp_param->buffer_empty_cnt = REINIT_FLAG; // Ignore ALC re-init request before play en
        }
    }else if (n9_a2dp_param->buffer_empty_cnt >= REINIT_FLAG)
    {
        n9_a2dp_param->buffer_empty_cnt = (n9_a2dp_param->buffer_empty_cnt > (REINIT_FLAG + (reinit_thd*RESEND_REINIT_CNT))) ? 0 : n9_a2dp_param->buffer_empty_cnt + 1;
    }
    if ((source->transform != NULL)&&(DSP_Callback_BypassModeGet(source,source->transform->sink)==BYPASS_CODEC_MODE)) {
        *(n9_a2dp_param->a2dp_lostnum_report) = *(n9_a2dp_param->a2dp_lostnum_report) +1;
        *(n9_a2dp_param->a2dp_lostnum_report + 1) = n9_a2dp_param->predict_timestamp;
        #if AUDIO_ASI_CNT
        lt_clk_skew_accumulate_local_asi(amount);
        #endif
        if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
            //trigger flag to notify transform
                //for AAC part
            if (n9_a2dp_param->ios_aac_flag == TRUE)
            {
                n9_a2dp_param->predict_timestamp = (n9_a2dp_param->predict_timestamp + 1)%0x10000; // ios aac use sequence number instead of time stamp
            }
            else
            {
                n9_a2dp_param->predict_timestamp += AAC_FRAME_DECODE_SAMPLE_NUM*n9_a2dp_param->timestamp_ratio;//temp,need add resolution confg
            }
        }
        else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC){
            if(n9_a2dp_param->ios_sbc_flag == FALSE)
            {
                n9_a2dp_param->predict_timestamp += SBC_FRAME_DECODE_SAMPLE_NUM*n9_a2dp_param->timestamp_ratio;//temp,need add resolution confg
            }
        }
        else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR){
            #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
            if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33){
                n9_a2dp_param->predict_timestamp += VEND_2_FRAME_DECODE_SAMPLE_NUM*n9_a2dp_param->timestamp_ratio;
            }
            else
            #endif
            {
                n9_a2dp_param->predict_timestamp += vend_get_frame_size(source)*n9_a2dp_param->timestamp_ratio;
            }
        }
        DSP_Callback_BypassModeCtrl(source,source->transform->sink,STREAMING_MODE);
        return;
    }
    source->param.n9_a2dp.prev_seq_num = source->param.n9_a2dp.current_seq_num;
    if((n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG)&&(source->transform != NULL)){
        if ((source->param.n9_a2dp.alc_monitor == FALSE)&&(source->param.n9_a2dp.sink_latency != 0)&&(source->param.n9_a2dp.mce_flag == TRUE)&&(source->param.n9_a2dp.latency_monitor == FALSE))
        {
            #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
            buffer_notify =  FALSE;
            #else
            buffer_notify =  A2dp_Buffer_Notify(source,96);
            #endif
            n9_a2dp_param->buffer_empty_cnt = ((read_write_space < 0x800)||buffer_notify) ? n9_a2dp_param->buffer_empty_cnt + 1 :
            (n9_a2dp_param->buffer_empty_cnt != 0) ? n9_a2dp_param->buffer_empty_cnt - 1 : 0;
        }
        if(n9_a2dp_param->buffer_empty_cnt > (reinit_thd>>1))
        {
            STRM_A2DP_LOG_I("Buffer nearly abnormal  cnt: %d ro : %d wo: %d seqn: %d", 4, n9_a2dp_param->buffer_empty_cnt, share_buff_info->ReadOffset, share_buff_info->WriteOffset,n9_a2dp_param->current_seq_num );
            ((SHARE_BUFFER_INFO *)source->param.n9_a2dp.share_info_base_addr)->notify_count = TRUE;
        }
        if (n9_a2dp_param->resend_reinit_req == TRUE)
        {
            source->transform->sink->param.audio.afe_wait_play_en_cnt = PLAY_EN_TRIGGER_REINIT_MAGIC_NUM;
        }
    }

    /* SBC part */
    if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
        /* Fragmented frame */
        if ( n9_a2dp_param->fragment_flag == TRUE ) {
            U32 temp_packet_size = 0;
            bt_codec_a2dp_pcb_type_t sbc_last_pcb_info;

            if ( n9_a2dp_param->current_frame_size == amount ){
                if (n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt == 0)
                {
                    n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt = sbc_report_bitrate(source);
                }
                hal_nvic_save_and_set_interrupt_mask(&mask);
                /* All fragmented packets, except the last one, shall have the same total packet size */
                share_buff_info->ReadOffset += (n9_a2dp_param->current_packet_size * (n9_a2dp_param->totalFrameCnt - 1));
                if ( share_buff_info->ReadOffset >= share_buff_info->length ) {
                    share_buff_info->ReadOffset -= share_buff_info->length;
                }
                /* Get the last one packet size */
                DSP_C2D_BufferCopy((VOID*)  &sbc_last_pcb_info,
                                   (VOID*)  (share_buff_info->startaddr + share_buff_info->ReadOffset),
                                   (U16)    sizeof(bt_codec_a2dp_pcb_type_t),
                                   (VOID*)  share_buff_info->startaddr,
                                   (U16)    share_buff_info->length);
                /* Save global information */
                {
                    temp_packet_size = (U32)sbc_last_pcb_info.size + 4;
                }
                /* Update share buffer read offset */
                share_buff_info->ReadOffset += temp_packet_size;
                if ( share_buff_info->ReadOffset >= share_buff_info->length ) {
                    share_buff_info->ReadOffset -= share_buff_info->length;
                }
                /* Reset frame read offset */
                n9_a2dp_param->readOffset = 0;
                hal_nvic_restore_interrupt_mask(mask);
            }
            else {
                //TBD
            }
        }
        /* Non-fragmented frame */
        else {
            if ( n9_a2dp_param->current_frame_size == amount ){
                #if AUDIO_ASI_CNT
                lt_clk_skew_accumulate_local_asi(SBC_FRAME_DECODE_SAMPLE_NUM);
                #endif
                if (n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt == 0)
                {
                    n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt = sbc_report_bitrate(source);
                }
                hal_nvic_save_and_set_interrupt_mask(&mask);
                n9_a2dp_param->dropFrameCnt++;
                if ( n9_a2dp_param->dropFrameCnt == n9_a2dp_param->totalFrameCnt ) {
                    /* Update share buffer read offset */
                    share_buff_info->ReadOffset += n9_a2dp_param->current_packet_size;
                    if ( share_buff_info->ReadOffset >= share_buff_info->length ) {
                        share_buff_info->ReadOffset -= share_buff_info->length;
                    }
                    /* reset frame read offset */
                    n9_a2dp_param->readOffset = 0;
                    if(n9_a2dp_param->ios_sbc_flag == TRUE) {
                        n9_a2dp_param->predict_timestamp = (n9_a2dp_param->predict_timestamp + 1)%0x10000;
                    }
                }
                else {
                    n9_a2dp_param->readOffset += n9_a2dp_param->current_frame_size;
                    if (n9_a2dp_param->readOffset > n9_a2dp_param->current_packet_size){
                        STRM_A2DP_LOG_E("readoffset %d cross over packet size %d Wo:%d Ro:%d totalFrameCnt:%d curFrameSize:%d", 6, n9_a2dp_param->readOffset,n9_a2dp_param->current_packet_size,share_buff_info->WriteOffset,share_buff_info->ReadOffset, n9_a2dp_param->totalFrameCnt, n9_a2dp_param->current_frame_size);
                        #if N9_A2DP_DEBUG_ASSERT
                        configASSERT(0);
                        #endif
                    }
                }
                hal_nvic_restore_interrupt_mask(mask);
                if(n9_a2dp_param->ios_sbc_flag == FALSE) 
                {
                    n9_a2dp_param->predict_timestamp += SBC_FRAME_DECODE_SAMPLE_NUM*n9_a2dp_param->timestamp_ratio;
                }
            }
            else {
                //TBD
            }
        }
    }
    /* AAC part */
    else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC ) {
        //get packet loss status
        if ( n9_a2dp_param->current_frame_size == amount ){
            if (n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt >= AAC_BITRATE_REPORT_ACCUM_TIME){
                *(n9_a2dp_param->a2dp_bitrate_report.p_a2dp_bitrate_report) = (U32)(n9_a2dp_param->a2dp_bitrate_report.a2dp_accumulate_cnt)*a2dp_get_samplingrate(source)/(U32)(AAC_FRAME_DECODE_SAMPLE_NUM*n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt/8);
                n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt = 0;
                n9_a2dp_param->a2dp_bitrate_report.a2dp_accumulate_cnt = 0;
            }
            else{
                n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt++;
                n9_a2dp_param->a2dp_bitrate_report.a2dp_accumulate_cnt += amount;
            }
            hal_nvic_save_and_set_interrupt_mask(&mask);
            /* Update share buffer read offset */
            share_buff_info->ReadOffset += n9_a2dp_param->current_packet_size;
            if ( share_buff_info->ReadOffset >= share_buff_info->length ) {
                share_buff_info->ReadOffset -= share_buff_info->length;
            }
            if (n9_a2dp_param->ios_aac_flag == TRUE)
            {
                n9_a2dp_param->predict_timestamp = (n9_a2dp_param->predict_timestamp + 1)%0x10000; // ios aac use sequence number instead of time stamp
            }
            else
            {
                n9_a2dp_param->predict_timestamp += AAC_FRAME_DECODE_SAMPLE_NUM*n9_a2dp_param->timestamp_ratio;//temp,need add resolution confg
            }
            #if AUDIO_ASI_CNT
            lt_clk_skew_accumulate_local_asi(AAC_FRAME_DECODE_SAMPLE_NUM);
            #endif
            hal_nvic_restore_interrupt_mask(mask);
        }
        else {
            //TBD
        }
    }
    /* VENDOR part */
    else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ) {
        
        #ifdef MTK_BT_A2DP_VENDOR_2_ENABLE
        if (source->param.n9_a2dp.codec_info.codec_cap.codec.vend.codec_id== 0x4C33){
            if ( n9_a2dp_param->current_frame_size == amount ){
                vend_drop_frame(source);
                #if AUDIO_ASI_CNT
                lt_clk_skew_accumulate_local_asi(VEND_2_FRAME_DECODE_SAMPLE_NUM);
                #endif
                if(n9_a2dp_param->ios_sbc_flag == FALSE)
                {
                    n9_a2dp_param->predict_timestamp += VEND_2_FRAME_DECODE_SAMPLE_NUM*n9_a2dp_param->timestamp_ratio;
                }
            }
        }
        else
        #endif
        {
            #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
            if (source->param.n9_a2dp.sink_latency == 0)
            {
                p_vend_bc_exbuf_if->read_drop_flag = FALSE;
            }
            else if ( n9_a2dp_param->current_frame_size == amount ){
            #else
            if ( n9_a2dp_param->current_frame_size == amount ){
            #endif
                vend_drop_frame(source);
                #if AUDIO_ASI_CNT
                lt_clk_skew_accumulate_local_asi(vend_get_frame_size(source));
                #endif
                if(n9_a2dp_param->ios_sbc_flag == FALSE)
                {
                    n9_a2dp_param->predict_timestamp += vend_get_frame_size(source)*n9_a2dp_param->timestamp_ratio;
                }
            }
            else {
                //TBD
            }
        }
    }
    else {
        //Not support codec type
    }
    if ( share_buff_info->bBufferIsFull == TRUE ) {
        STRM_A2DP_LOG_W("A2DP bBufferIsFull, sync readoffset to writeoffset to reset buffer\r\n", 0);
        share_buff_info->bBufferIsFull = FALSE;
        n9_a2dp_param->readOffset = 0;
        share_buff_info->ReadOffset = share_buff_info->WriteOffset;
        n9_a2dp_param->resend_reinit_req = TRUE;
    }
    update_to_share_information(source);
    //printf("Drop buf_Roffset :%d amount:%d bF:%d",share_buff_info->ReadOffset,amount,share_buff_info->bBufferIsFull);
    STRM_A2DP_LOG_D("SourceDrop_N9_a2dp--\r\n", 0);
}

BOOL SourceConfigure_N9_a2dp(SOURCE source, stream_config_type type, U32 value)
{
    UNUSED(source);
    UNUSED(type);
    UNUSED(value);
    return TRUE;
}

BOOL SourceClose_N9_a2dp(SOURCE source)
{
    ((SHARE_BUFFER_INFO *)source->param.n9_a2dp.share_info_base_addr)->notify_count = 0;
    #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
    DSPMEM_Free(source->taskId, (VOID*)source);
    #endif
    return TRUE;
}

VOID SourceInit_N9_a2dp(SOURCE source)
{
    /* buffer init */
    #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
    if (( source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ) && (source->param.n9_a2dp.sink_latency == 0))
    {
        p_vend_bc_exbuf_if = DSPMEM_tmalloc(source->taskId, sizeof(vend_bc_extern_buf_t), (VOID*)source);
        p_vend_bc_exbuf_if->a2dp_source = source;
        //s_set_vend_bc_params(&(p_vend_bc_exbuf_if->param));
        //p_vend_bc_exbuf_if->param = bc_params;
        U8* vend_bc_buf;
        STRM_A2DP_LOG_I("Vendor BC request buffer size : %d %d", 2,VEND_BC_HANDLE_SIZE,source->streamBuffer.ShareBufferInfo.length);
        if (source->streamBuffer.ShareBufferInfo.length >= 0xA000) // if share buffer size more than 40K
        {
            vend_bc_buf = reshape_share_buffer_for_vend_bc(source,0x1C00);
            //vend_bc_write_Init();
        }
        else
        {
            vend_bc_buf = DSPMEM_tmalloc(source->taskId, VEND_BC_HANDLE_SIZE, (VOID*)source);
            STRM_A2DP_LOG_I("malloc from DSP0", 0);
        }
        p_vend_bc_exbuf_if->p_handle = vend_bc_open(vend_bc_buf, VEND_BC_HANDLE_SIZE, &p_vend_bc_exbuf_if->param);
        vend_bc_write_loop_callback();
    }
    #endif
    //source->type = SOURCE_TYPE_N9_A2DP;
    source->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    /* interface init */
    source->sif.SourceSize        = SourceSize_N9_a2dp;
    source->sif.SourceMap         = SourceMap_N9_a2dp;
    source->sif.SourceConfigure   = SourceConfigure_N9_a2dp;
    source->sif.SourceDrop        = SourceDrop_N9_a2dp;
    source->sif.SourceClose       = SourceClose_N9_a2dp;
    source->sif.SourceReadBuf     = SourceReadBuf_N9_a2dp;
}




