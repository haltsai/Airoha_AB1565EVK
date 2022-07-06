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
#include "stream_n9sco.h"
#include "long_term_clk_skew.h"
#include "dsp_audio_msg.h"
#include "dsp_audio_msg_define.h"
#include "dsp_audio_ctrl.h"
#include "dsp_memory.h"
#include "bt_interface.h"
#ifdef MTK_PEQ_ENABLE
#include "peq_interface.h"
#endif
#include "bt_types.h"
#include "swla.h"
//#define REDUNDANT

log_create_module(strm_a2dp, PRINT_LEVEL_INFO);

/******************************************************************************
 * Definition
*******************************************************************************/
#define UNUSED(p) ((void)(p))
#define N9_A2DP_DEBUG_LOG   0
#define AAC_FRAME_DECODE_SAMPLE_NUM 1024
#define BUFFER_EMPTY_ASI_PADDING_NUM 1024
#define SBC_FRAME_DECODE_SAMPLE_NUM 128
#define AAC_BITRATE_REPORT_ACCUM_TIME 30
#define BUFFER_EMPTY_REINIT_THD 5
#define BUFFER_WAIT_PLR_ISR_NUM 3
#define BUFFER_WAIT_PLR_ISR_NUM_ALC 5
#define BUFFER_WAIT_PLR_ISR_NUM_GAMING_MODE 2
#define BUFFER_STATE_LOST_SKIP_MAX_NUM 1
#define ASI_MASK (0x3FFFFFF)
#define NOTIFY_LOW_BOND   80
#define NOTIFY_LOW_THD    200
#define NOTIFY_LOW_BOND_GAMING_MODE   20


#define REINIT_FLAG (0x7FFF)
#define RESEND_REINIT_CNT (30) // Resend interval is  roughtly about (20ms *  BUFFER_EMPTY_REINIT_THD * RESEND_REINIT_CNT)
#define SSRC_MAGIC_NUM 0x12345678

#define PCB_HEADER_LEN    4
#define RTP_HEADER_LEN    12
#define CP_LEN            1
#define PAYLOAD_HEADER    1
#define VEND_FRAME_HEADER 4
#define N9_A2DP_DEBUG_ASSERT (0)
#define MACRO_CHK_FIRST_PKT(param,state)                \
        {                                               \
            if(param->DspReportStartId == (0x8000 | MSG_DSP2MCU_BT_AUDIO_DL_START_ACK))\
            {                                           \
                STRM_A2DP_LOG_I("%s at first packet\n", 1, state);   \
            }                                           \
        }


#ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
#include "vendor_decoder_proting.h"
vend_bc_extern_buf_v2_t* p_vend_bc_exbuf_if;
#endif

#ifdef MTK_BT_A2DP_AIRO_CELT_ENABLE
#define AIROHA_PKT_FAKE_VERIFY          (FALSE)
#define AIROHA_GAMING_HS_DL_DBG_LOG     (TRUE)
#define AIROHA_GAMING_STD_ENC_FRM_SIZE  (80)
#define AIROHA_GAMING_FRAME_NO          (367/AIROHA_GAMING_STD_ENC_FRM_SIZE)

#if (AIROHA_PKT_FAKE_VERIFY)
#define FAKE_MEM_BLOCK_SIZE (400)
#define FAKE_MEM_BLOCK_NO   (10)

AVM_SHARE_BUF_INFO AiroFakeAvmShareBufInfo = {
    .StartAddr = 0,       // start address of share buffer
    .ReadIndex = 0,  // read pointer of share buffer  : DSP monitor
    .WriteIndex = 0, // write pointer of share buffer : Controller monitor
    .SampleRate = 48000, // sample rate for clock skew counting
    .MemBlkSize = FAKE_MEM_BLOCK_SIZE, // block length for each frame
    .MemBlkNum = FAKE_MEM_BLOCK_NO,  // number of block for frame usage
    .DbgInfoAddr = NULL, // start address of controller packet address table
    .FrameSampleNum = 0,  // DSP notify audio
    .codec_type = 0,      // Codec information
    .codec_config = 0,    // Codec config information
    .NotifyCount = 0,  // notify count of DSP notify controller not to sleep
    .ForwarderAddr = NULL, // forwarder buffer address, also used to report ASI compromise in A2DP
    .SinkLatency = 14000, // a2dp sink latency
};

U8 AiroFakeShareData[FAKE_MEM_BLOCK_SIZE*FAKE_MEM_BLOCK_NO];
U8 const AiroFakeDatabase[] = {
    #include "airo_fake_pattern.txt"
};
U32 AiroFakeDatabaseIdx;
#if 0
#define FAKE_BLOCK_SIZE                 (12+1+1+2+4+AIROHA_GAMING_FRAME_NO*(1+AIROHA_GAMING_STD_ENC_FRM_SIZE))
#else
#define FAKE_BLOCK_SIZE                 (1+1+2+4+AIROHA_GAMING_FRAME_NO*(1+AIROHA_GAMING_STD_ENC_FRM_SIZE))
#endif
#endif /* End of AIROHA_PKT_FAKE_VERIFY */

#endif

//#define A2DP_IDS_DEBUG_EN

#ifdef A2DP_IDS_DEBUG_EN

#define A2DP_IDS_DEBUG_GPIO_NO  HAL_GPIO_6
static bool is_gpio_init = false;
void gpio_init_and_set_low(void) {
    if(is_gpio_init == false) {
        hal_gpio_init(A2DP_IDS_DEBUG_GPIO_NO);
        hal_pinmux_set_function(A2DP_IDS_DEBUG_GPIO_NO, 0);
        hal_gpio_set_direction(A2DP_IDS_DEBUG_GPIO_NO, HAL_GPIO_DIRECTION_OUTPUT);
        hal_gpio_set_output(A2DP_IDS_DEBUG_GPIO_NO, 0);

        STRM_A2DP_LOG_I("[A2DP_IDS_DEBUG] GPIO init and set to low", 0);
        is_gpio_init = true;
    }
}

void gpio_revert(uint32_t revert_times) {
    for(uint32_t i=0; i<revert_times; i++) {
        hal_gpio_set_output(A2DP_IDS_DEBUG_GPIO_NO, 1);
        hal_gpio_set_output(A2DP_IDS_DEBUG_GPIO_NO, 0);
    }
    STRM_A2DP_LOG_I("[A2DP_IDS_DEBUG] GPIO revert %d times", 1, revert_times);
}

#endif

/******************************************************************************
 * Extern Functions
*******************************************************************************/
EXTERN VOID StreamDSP_HWSemaphoreTake(VOID);
EXTERN VOID StreamDSP_HWSemaphoreGive(VOID);
EXTERN uint32_t afe_get_dl1_query_data_amount(void);
EXTERN audio_dsp_ull_start_ctrl_param_t audio_headset_ull_ctrl;

/******************************************************************************
 * Static Functions
*******************************************************************************/
VOID SourceDrop_N9_a2dp(SOURCE source, U32 amount);
ATTR_TEXT_IN_IRAM_LEVEL_2 static VOID update_from_share_information(SOURCE source)
{
    #if AIROHA_PKT_FAKE_VERIFY
    if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT)
    memcpy(&(source->streamBuffer.AVMBufferInfo.ReadIndex), (void *)(&AiroFakeAvmShareBufInfo.ReadIndex), sizeof(AVM_SHARE_BUF_INFO)-sizeof(U32));
    else
    #endif
    //StreamDSP_HWSemaphoreTake();
    memcpy(&(source->streamBuffer.AVMBufferInfo.ReadIndex), (void *)(source->param.n9_a2dp.share_info_base_addr + sizeof(U32)), sizeof(AVM_SHARE_BUF_INFO)-sizeof(U32));/* share info fix 32 byte */
}
static VOID update_to_share_information(SOURCE source)
{
    //StreamDSP_HWSemaphoreTake();
    /* Put your code here, if the semaphore is taken, do something for the critical resource...... */
    ((AVM_SHARE_BUF_INFO *)source->param.n9_a2dp.share_info_base_addr)->ReadIndex = (source->streamBuffer.AVMBufferInfo.ReadIndex);
    //StreamDSP_HWSemaphoreGive();
}

static VOID share_information_read_index_update(SOURCE source)
{
    U32 mask;
    U32 readoffset = source->streamBuffer.AVMBufferInfo.MemBlkSize*source->streamBuffer.AVMBufferInfo.ReadIndex;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    ((bt_codec_a2dp_hdr_type_ptr)((U8*)source->streamBuffer.AVMBufferInfo.StartAddr + readoffset))->pcb_state = PCB_STATE_DECODED;
    source->streamBuffer.AVMBufferInfo.ReadIndex = (source->streamBuffer.AVMBufferInfo.ReadIndex + 1)%source->streamBuffer.AVMBufferInfo.MemBlkNum;
    update_to_share_information(source);
    hal_nvic_restore_interrupt_mask(mask);
}


#ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
static U8* reshape_share_buffer_for_vend_bc(SOURCE source,U16 length)
{
    return (U8*)(hal_memview_cm4_to_dsp0(((AVM_SHARE_BUF_INFO_PTR)source->param.n9_a2dp.share_info_base_addr)->StartAddr) + length);
}

#endif

ATTR_TEXT_IN_IRAM_LEVEL_1 static U32 a2dp_get_hdr_info(SOURCE source)
{
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U16 return_status = RETURN_PASS;
    bt_codec_a2dp_hdr_type_t a2dp_hdr_info;
    do {
        memcpy(&a2dp_hdr_info,(U8*)(share_buff_info->StartAddr + share_buff_info->MemBlkSize*share_buff_info->ReadIndex),sizeof(bt_codec_a2dp_hdr_type_t));
        
        /* Save global information */
        {
            n9_a2dp_param->current_frame_size = a2dp_hdr_info.frame_size;
        }

        if (a2dp_hdr_info.pcb_state == PCB_STATE_LOST)
        {
            //printf("meet state lost");
            //configASSERT(0);
            if (source->transform == NULL){
                return_status = RETURN_FAIL;
                break;
            }

            if (n9_a2dp_param->pkt_lost_report_state == PL_STATE_NONE)
            {
               n9_a2dp_param->pkt_lost_report_state = PL_STATE_REPORTPREVSEQN;
            }
            #ifdef AIR_AUDIO_HARDWARE_ENABLE
            if ((afe_get_dl1_query_data_amount() > (((source->param.n9_a2dp.latency_monitor == true) ? BUFFER_WAIT_PLR_ISR_NUM_GAMING_MODE : \
            (source->param.n9_a2dp.alc_monitor) ? BUFFER_WAIT_PLR_ISR_NUM : BUFFER_WAIT_PLR_ISR_NUM_ALC)*(source->transform->sink->param.audio.rate * source->transform->sink->param.audio.period / 1000)))||(afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) ==0)){
                S16 i;
                for (i = 1; i < (BUFFER_STATE_LOST_SKIP_MAX_NUM + 1); i++)
                {
                    memcpy(&a2dp_hdr_info,(U8*)(share_buff_info->StartAddr + share_buff_info->MemBlkSize*((source->streamBuffer.AVMBufferInfo.ReadIndex + i)%source->streamBuffer.AVMBufferInfo.MemBlkNum)),sizeof(bt_codec_a2dp_hdr_type_t));
                    if((a2dp_hdr_info.pcb_state == PCB_STATE_USED)&&(n9_a2dp_param->predict_asi == (a2dp_hdr_info.frame_asi & ASI_MASK)))
                    {
                        STRM_A2DP_LOG_I("PCB_STATE_LOST skip overpad block seqN:%d skip frame num:%d Wo %d Ro %d ", 4, n9_a2dp_param->current_seq_num,i,share_buff_info->WriteIndex,share_buff_info->ReadIndex);
                        for (;i>0;i--)
                        {
                            share_information_read_index_update(source);
                        }
                        break;
                    }
                }
                return_status = RETURN_FAIL;
                break;
            }
            else
            #endif /* AIR_AUDIO_HARDWARE_ENABLE */
            {
                U32 gpt_timer;
                if (share_buff_info->ReadIndex == share_buff_info->WriteIndex) {
                    return_status = RETURN_FAIL;
                    break;
                }
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
                share_information_read_index_update(source);
                STRM_A2DP_LOG_I("PCB_STATE_LOST SKIP seqN:%d skip size:%d Wo %d Ro %d timer %d", 5, n9_a2dp_param->current_seq_num,n9_a2dp_param->current_frame_size,share_buff_info->WriteIndex,share_buff_info->ReadIndex,gpt_timer);
                if (share_buff_info->ReadIndex == share_buff_info->WriteIndex) {
                    return_status = RETURN_FAIL;
                    break;
                }
            }
        }
        else if((a2dp_hdr_info.pcb_state == PCB_STATE_SKIP)||(a2dp_hdr_info.pcb_state == PCB_STATE_DECODED))
        {
            if ((share_buff_info->ReadIndex == share_buff_info->WriteIndex)&&(a2dp_hdr_info.pcb_state == PCB_STATE_DECODED)) {
                return_status = RETURN_FAIL;
                break;
            }
            share_information_read_index_update(source);
            STRM_A2DP_LOG_I("PCB STATE skip frame status %d ReadIndex:%d WriteIndex:%d", 3,a2dp_hdr_info.pcb_state,share_buff_info->ReadIndex,share_buff_info->WriteIndex);
        }
        else if ((a2dp_hdr_info.pcb_state == PCB_STATE_FEC)||(a2dp_hdr_info.pcb_state == PCB_STATE_FREE))
        {
            STRM_A2DP_LOG_I("Parse PCB meet rare state :%d Ro: %d  Wo: %d ",3,a2dp_hdr_info.pcb_state,share_buff_info->ReadIndex,share_buff_info->WriteIndex);
            return RETURN_FAIL;
        }
        else if (a2dp_hdr_info.pcb_state > PCB_STATE_DECODED)
        {
            if (share_buff_info->ReadIndex == share_buff_info->WriteIndex){
                return_status = RETURN_FAIL;
                break;
            }
            configASSERT(0);
        }
    } while(a2dp_hdr_info.pcb_state != PCB_STATE_USED);

    if (a2dp_hdr_info.frame_size > share_buff_info->MemBlkSize)// abnormal large packet
    {
        STRM_A2DP_LOG_E("a2dp illegal pkt size :%d Ro:%d Wo:%d", 3, a2dp_hdr_info.frame_size,share_buff_info->ReadIndex,share_buff_info->WriteIndex);
        if (share_buff_info->ReadIndex != share_buff_info->WriteIndex)
        {configASSERT(0);}
    }

    if (return_status == RETURN_PASS)
    {
        n9_a2dp_param->current_asi        =   a2dp_hdr_info.frame_asi & ASI_MASK;
        n9_a2dp_param->current_seq_num    =   a2dp_hdr_info.frame_pkt_seqn;
        n9_a2dp_param->current_frame_size =   a2dp_hdr_info.frame_size;
    }
    return return_status;
}

static VOID a2dp_skip_expired_packet(SOURCE source)
{
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);

    if ((share_buff_info->WriteIndex - share_buff_info->ReadIndex) == 0)
    {
        STRM_A2DP_LOG_E("skip expired Wi %d = Ri %d", share_buff_info->WriteIndex,share_buff_info->ReadIndex);
        return;
    }
    STRM_A2DP_LOG_I("skip expired Wi %d Ri %d asi %d pasi %d", 4, share_buff_info->WriteIndex,share_buff_info->ReadIndex,n9_a2dp_param->current_asi, n9_a2dp_param->predict_asi);

    while((((S32)n9_a2dp_param->predict_asi - (S32)n9_a2dp_param->current_asi)) > 0){
        /* Check next packet is expired or not */
        share_information_read_index_update(source);
        if (a2dp_get_hdr_info(source) == RETURN_FAIL)
        {
            break;
        }
        if ((share_buff_info->WriteIndex - share_buff_info->ReadIndex) == 0)
        {
            STRM_A2DP_LOG_E("skip expired Wi %d = Ri %d, first", share_buff_info->WriteIndex,share_buff_info->ReadIndex);
            break;
        }
    }
    STRM_A2DP_LOG_I("skip expired Wi %d Ri %d asi %d pasi %d end", 4, share_buff_info->WriteIndex,share_buff_info->ReadIndex,n9_a2dp_param->current_asi, n9_a2dp_param->predict_asi);
}


ATTR_TEXT_IN_IRAM_LEVEL_2 static VOID sbc_get_frame_information(SOURCE source, bt_codec_a2dp_sbc_frame_header_t *sbc_frame_info)
{
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);
    U32 read_offset;
    read_offset = share_buff_info->MemBlkSize*share_buff_info->ReadIndex + sizeof(bt_codec_a2dp_rtpheader_t);
    memcpy(sbc_frame_info,(U8*)(share_buff_info->StartAddr+ read_offset),sizeof(bt_codec_a2dp_sbc_frame_header_t));
    if ( sbc_frame_info->SyncWord != 0x9C ){
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
    else if (source->param.n9_a2dp.codec_info.codec_cap.type ==  BT_A2DP_CODEC_VENDOR) {
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
    else if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ) {
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
    else {
        //not support codec type
    }
    return channel;
}

static VOID vend_get_frame_information(SOURCE source, bt_codec_a2dp_vend_frame_header_t *vend_frame_info)
{
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);
    U32 read_offset;
    read_offset = share_buff_info->MemBlkSize*share_buff_info->ReadIndex + sizeof(bt_codec_a2dp_rtpheader_t);
    memcpy(vend_frame_info,(U8*)(share_buff_info->StartAddr + read_offset),sizeof(bt_codec_a2dp_vend_frame_header_t));
}


static U16 vend_get_frame_size(SOURCE source)
{
    U16 frame_size;
    switch (source->streamBuffer.AVMBufferInfo.SampleRate) {
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

#if 0
ATTR_TEXT_IN_IRAM_LEVEL_2 static U32 vend_frame_size_calc(bt_codec_a2dp_vend_frame_header_t *vend_frame_info)
{
    U32 frame_size;
    frame_size = (vend_frame_info->vend_byte2 & 0x7);
    frame_size = (frame_size<<6) + (vend_frame_info->vend_byte3>>2) + VEND_FRAME_HEADER;
    return frame_size;
}
#endif

#ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
void vend_bc_write_loop_callback(void)
{
    SOURCE source = p_vend_bc_exbuf_if->a2dp_source;
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(p_vend_bc_exbuf_if->a2dp_source->param.n9_a2dp);
    bt_codec_a2dp_vend_frame_header_t vend_frame_info;
    AVM_SHARE_BUF_INFO *share_buff_info  = &(p_vend_bc_exbuf_if->a2dp_source->streamBuffer.AVMBufferInfo);
    while (a2dp_get_hdr_info(source) == RETURN_PASS){
        if(share_buff_info->ReadIndex == share_buff_info->WriteIndex){
            break;
        }
        vend_get_frame_information(source, &vend_frame_info);
        n9_a2dp_param->current_frame_size = (vend_frame_info.vend_byte2 & 0x7);
        n9_a2dp_param->current_frame_size = (n9_a2dp_param->current_frame_size<<6) + (vend_frame_info.vend_byte3>>2) + VEND_FRAME_HEADER;
        if (vend_bc_write(p_vend_bc_exbuf_if->p_handle, (void*)(share_buff_info->StartAddr + share_buff_info->ReadIndex*share_buff_info->MemBlkSize + sizeof(bt_codec_a2dp_hdr_type_t)), n9_a2dp_param->current_frame_size) != 0x7fff)
        {
            share_information_read_index_update(source);
            update_from_share_information(source);
            if(share_buff_info->ReadIndex == share_buff_info->WriteIndex){
                break;
            }
        }
        else
        {
            break;
        }
    }
}

#if 0
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
static vend_bc_param_t bc_params = {256,1000,60,60,5,-8,1,2000,0};

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
    *(source->param.n9_a2dp.a2dp_bitrate_report.p_a2dp_bitrate_report) = 8*(sbc_frame_size_calc(&sbc_frame_info)*source->streamBuffer.ShareBufferInfo.sample_rate/(U32)(subbands*blocks));
    //STRM_A2DP_LOG_I("sbc bit rate %d", 1, *(source->param.n9_a2dp.a2dp_bitrate_report.p_a2dp_bitrate_report));
    return (sbc_frame_info.SyncWord == 0x9c);
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
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);
    uint32_t sink_buffering_time;
    uint32_t temp_read_index = share_buff_info->ReadIndex;
    uint32_t read_asi,receive_asi;
    if ((source != NULL) && (source->transform != NULL) && (source->transform->sink != NULL))
    {
        #ifdef AIR_AUDIO_HARDWARE_ENABLE
        sink_buffering_time = afe_get_dl1_query_data_amount()/(source->transform->sink->param.audio.src_rate/1000);
        #endif /* AIR_AUDIO_HARDWARE_ENABLE */
        *(U32*)source->param.n9_a2dp.readOffset = (source->transform->sink->param.audio.irq_exist) ? sink_buffering_time : (share_buff_info->SinkLatency/1000);
    }
    else
    {
        STRM_A2DP_LOG_E("[A2dp_Buffer_Notify] transform or sink is null", 0);
        return FALSE;
    }
    while (((bt_codec_a2dp_hdr_type_ptr)(share_buff_info->StartAddr + ((temp_read_index+1)%share_buff_info->MemBlkNum)*share_buff_info->MemBlkSize))->pcb_state == PCB_STATE_USED)
    {
        temp_read_index = (temp_read_index+1)%share_buff_info->MemBlkNum;
        if (temp_read_index ==  share_buff_info->ReadIndex)
        {
            ((AVM_SHARE_BUF_INFO *)source->param.n9_a2dp.share_info_base_addr)->NotifyCount = sink_buffering_time;
            return TRUE;
        }
    }
    read_asi    = ((bt_codec_a2dp_hdr_type_ptr)(share_buff_info->StartAddr + share_buff_info->ReadIndex*share_buff_info->MemBlkSize))->frame_asi;
    receive_asi = ((bt_codec_a2dp_hdr_type_ptr)(share_buff_info->StartAddr + temp_read_index*share_buff_info->MemBlkSize))->frame_asi;

    ((AVM_SHARE_BUF_INFO *)source->param.n9_a2dp.share_info_base_addr)->reserved[0] = read_asi - ((sink_buffering_time*(share_buff_info->SampleRate/1000))&0xFFFFFF00);
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    sink_buffering_time = afe_get_dl1_query_data_amount()/(source->transform->sink->param.audio.src_rate/1000) + ((receive_asi - read_asi)/(share_buff_info->SampleRate/1000));
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
    ((AVM_SHARE_BUF_INFO *)source->param.n9_a2dp.share_info_base_addr)->NotifyCount = sink_buffering_time;

    //STRM_A2DP_LOG_I("[A2dp_Buffer_Notify] dbg  %d %d %d ",3 ,read_asi,receive_asi,sink_buffering_time,(source->transform->sink->param.audio.rate/1000));
    return (sink_buffering_time < thd);
}
ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL SourceReadBuf_N9_a2dp(SOURCE source, U8 *dst_addr, U32 length)
{
    STRM_A2DP_LOG_D("SourceReadBuf_N9_a2dp++\r\n", 0);

    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);

    #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR )
    {
        if (source->param.n9_a2dp.sink_latency != 0)
        {
           memcpy(dst_addr,(U8*)(share_buff_info->StartAddr + share_buff_info->ReadIndex*share_buff_info->MemBlkSize + sizeof(bt_codec_a2dp_hdr_type_t)),length);
        }
        else 
        {
            vend_bc_read(p_vend_bc_exbuf_if->p_handle, dst_addr, length);
            if (length >  VEND_FRAME_BUF_SIZE)
            {
                STRM_A2DP_LOG_I("Vend codec meet abnormal frame size", 1, length);
            }
        }
    }
    else
    #endif
    {
        memcpy(dst_addr,(U8*)(share_buff_info->StartAddr + share_buff_info->ReadIndex*share_buff_info->MemBlkSize + sizeof(bt_codec_a2dp_hdr_type_t)),length);
    }
    return TRUE;
}



ATTR_TEXT_IN_IRAM_LEVEL_1 U32 SourceSize_N9_a2dp(SOURCE source)
{
    STRM_A2DP_LOG_D("SourceSize_N9_a2dp++\r\n", 0);

#ifdef A2DP_IDS_DEBUG_EN
    gpio_init_and_set_low();
#endif

    U32 frame_size = 0;
    ltcs_bt_type_t codec_type = LTCS_TYPE_OTHERS;
    U8 pcb_parse_state = RETURN_FAIL;
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    #ifdef AIR_BT_CLK_SKEW_ENABLE
    U32 mask;
    ltcs_anchor_info_t * pcdc_info = (ltcs_anchor_info_t *)source->param.n9_a2dp.pcdc_info_buf;
    #endif

    UNUSED(codec_type);

    /* update share information data */
    update_from_share_information(source);
    #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
    if ((n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR) && (source->param.n9_a2dp.sink_latency == 0)){
        vend_bc_write_loop_callback();
        if (source->param.n9_a2dp.DspReportStartId != 0xFFFF)
        {
            SourceDrop_N9_a2dp(source,0);
        }
        if ((source->transform != NULL)&&(SinkSlack(source->transform->sink) >= share_buff_info->FrameSampleNum*(source->transform->sink->param.audio.format_bytes))){
            A2dp_Buffer_Notify(source,0);
            return vend_bc_read(p_vend_bc_exbuf_if->p_handle, NULL, 0);
        }
        else {
            return 0;
        }
    }
    #endif

    #ifdef AIR_BT_CLK_SKEW_ENABLE/* update pcdc information data */
    hal_nvic_save_and_set_interrupt_mask(&mask);
    lt_clk_skew_set_drift_value(pcdc_info->drift_comp_val); // update drift value
    lt_clk_skew_update_base_asi(pcdc_info->asi_base, pcdc_info->asi_cur); // update anchor base asi
    hal_nvic_restore_interrupt_mask(mask);
    #endif

    #ifdef MTK_PEQ_ENABLE
    PEQ_Update_Info(pcdc_info->anchor_clk, n9_a2dp_param->predict_asi);
    #endif
    /* Check there is underflow or not */
    if ((source->transform != NULL) &&(DSP_Callback_BypassModeGet(source,source->transform->sink)==BYPASS_CODEC_MODE && n9_a2dp_param->mce_flag == TRUE)){
        return n9_a2dp_param->current_frame_size;
    }
    A2dp_Buffer_Notify(source,0);

    pcb_parse_state = a2dp_get_hdr_info(source);

    BOOL enterbcm = FALSE;
    if ( pcb_parse_state == RETURN_FAIL ) {
        #ifdef AIR_AUDIO_HARDWARE_ENABLE
         if (((source->transform != NULL) && (n9_a2dp_param->mce_flag == TRUE) && (source->transform->sink->type == SINK_TYPE_AUDIO) )
            && (afe_get_dl1_query_data_amount() <= (((source->param.n9_a2dp.latency_monitor == true) ? BUFFER_WAIT_PLR_ISR_NUM_GAMING_MODE : \
            (source->param.n9_a2dp.alc_monitor) ? BUFFER_WAIT_PLR_ISR_NUM : BUFFER_WAIT_PLR_ISR_NUM_ALC)*(source->transform->sink->param.audio.rate * source->transform->sink->param.audio.period / 1000))) && (afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) !=0)){
            if (share_buff_info->ReadIndex == share_buff_info->WriteIndex)
            {
                if (((AVM_SHARE_BUF_INFO *)source->param.n9_a2dp.share_info_base_addr)->ForwarderAddr < 8192)
                {
                    ((AVM_SHARE_BUF_INFO *)source->param.n9_a2dp.share_info_base_addr)->ForwarderAddr += BUFFER_EMPTY_ASI_PADDING_NUM;
                }
                #ifdef A2DP_IDS_DEBUG_EN
                    gpio_revert(1);
                #endif
                STRM_A2DP_LOG_I("enter BCM buffer empty , pt:%d  t:%d ro: %d padding :%d", 4, n9_a2dp_param->predict_asi, n9_a2dp_param->current_asi,share_buff_info->ReadIndex,((AVM_SHARE_BUF_INFO *)source->param.n9_a2dp.share_info_base_addr)->ForwarderAddr);
                if(n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG){
                    n9_a2dp_param->buffer_empty_cnt++;
                }
            }
            else
            {
                STRM_A2DP_LOG_I("enter BCM PCM data not enough", 0);
            }
            enterbcm = TRUE;
         }
         else
         {
             if (source->param.n9_a2dp.DspReportStartId != 0xFFFF)
             {
                 SourceDrop_N9_a2dp(source,0);
             }
             return frame_size;
         }
         #endif /* AIR_AUDIO_HARDWARE_ENABLE */
    }
    /* Check there is data in share buffer or not */
    else{
        frame_size = n9_a2dp_param->current_frame_size;
        n9_a2dp_param->predict_asi = n9_a2dp_param->predict_asi & ASI_MASK;
        if (( n9_a2dp_param->predict_asi == n9_a2dp_param->current_asi )||(!((n9_a2dp_param->mce_flag == TRUE) && (source->transform->sink->type == SINK_TYPE_AUDIO) ))) {
                //do nothing
            }
            else if ( (S32)(n9_a2dp_param->predict_asi - n9_a2dp_param->current_asi) < 0 ) {
                //packet lost
                #ifdef A2DP_IDS_DEBUG_EN
                gpio_revert(4);
                #endif
                STRM_A2DP_LOG_I("enter BCM packet loss, pasi:%d Jump to asi:%d (seqn:%d) Wo:%d Ro:%d", 5, n9_a2dp_param->predict_asi, n9_a2dp_param->current_asi,n9_a2dp_param->current_seq_num,share_buff_info->WriteIndex,share_buff_info->ReadIndex);
                if(n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG){
                    #ifdef AIR_AUDIO_HARDWARE_ENABLE
                    if (afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) !=0)
                    {
                        n9_a2dp_param->buffer_empty_cnt++;
                    }
                    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
                }
                enterbcm = TRUE;
            }
            else{
                a2dp_skip_expired_packet(source);
                if ( n9_a2dp_param->predict_asi == n9_a2dp_param->current_asi ) {
                        //do nothing
                }
                else {
                #ifdef A2DP_IDS_DEBUG_EN
                gpio_revert(3);
                #endif
                    STRM_A2DP_LOG_I("enter BCM packet loss(drop past) pt:%d  t:%d seqn:%d", 3, n9_a2dp_param->predict_asi, n9_a2dp_param->current_asi,n9_a2dp_param->current_seq_num);
                    if(n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG){
                        #ifdef AIR_AUDIO_HARDWARE_ENABLE
                        if (afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1) !=0)
                        {
                            n9_a2dp_param->buffer_empty_cnt++;
                        }
                        #endif /* AIR_AUDIO_HARDWARE_ENABLE */
                    }
                    enterbcm = TRUE;
                }
            }

    }
    if (enterbcm){
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
        n9_a2dp_param->current_frame_size = frame_size;
        //configASSERT(0);
        return frame_size;
    }
    if (n9_a2dp_param->pkt_lost_report_state == PL_STATE_REPORTPREVSEQN)
    {
        U32 gpt_timer;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
        #ifdef A2DP_DIS_DEBUG_EN
        gpio_revert(2);
        #endif
        STRM_A2DP_LOG_I("A2DP meet packet loss state,Last PKT SEQN:%d timer: %d", 2, n9_a2dp_param->current_seq_num,gpt_timer);
        n9_a2dp_param->pkt_lost_report_state = PL_STATE_REPORTNEXTSEQN;
    }
    /*Header check for SBC & Vendor decoder */
    if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
        bt_codec_a2dp_sbc_frame_header_t sbc_frame_info;
        /* Get sbc frame information */
        sbc_get_frame_information(source, &sbc_frame_info);
        if ( sbc_frame_info.SyncWord != 0x9C )
        {
             STRM_A2DP_LOG_I("A2DP meet wrong sync word,Ro : %d Syncword : %d", 2, share_buff_info->ReadIndex,sbc_frame_info.SyncWord);
             SourceDrop(source,n9_a2dp_param->current_frame_size);
             n9_a2dp_param->predict_asi -= share_buff_info->FrameSampleNum;
             return SourceSize_N9_a2dp(source);
        }
    }
    else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR )
    {
        bt_codec_a2dp_vend_frame_header_t vend_frame_info;
        vend_get_frame_information(source, &vend_frame_info);
        if ( vend_frame_info.vend_byte1 != 0xAA )
        {
            STRM_A2DP_LOG_I("A2DP meet vend wrong sync word,Ro : %d Syncword : %d frame size: %d", 2, share_buff_info->ReadIndex,vend_frame_info.vend_byte1,n9_a2dp_param->current_frame_size);
            if (n9_a2dp_param->current_frame_size != 0)
            {
            SourceDrop(source,n9_a2dp_param->current_frame_size);
            n9_a2dp_param->predict_asi -= share_buff_info->FrameSampleNum;
            }
            else
            {
                share_information_read_index_update(source);
            }
            return SourceSize_N9_a2dp(source);
        }
    }

    return frame_size;
}

U8* SourceMap_N9_a2dp(SOURCE source)
{
    UNUSED(source);
    return NULL;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 VOID SourceDrop_N9_a2dp (SOURCE source, U32 amount)
{

    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);
    N9_A2DP_PARAMETER *n9_a2dp_param    = &(source->param.n9_a2dp);
    U16 reinit_thd = (n9_a2dp_param->codec_info.codec_cap.type != BT_A2DP_CODEC_AAC) ? BUFFER_EMPTY_REINIT_THD<<3 : BUFFER_EMPTY_REINIT_THD;
    BOOL buffer_notify;
    U16 dsp_notify_thd;
    if(share_buff_info->SampleRate>48000)
    {
        reinit_thd = reinit_thd*(share_buff_info->SampleRate/48000);
    }
    if (source->param.n9_a2dp.DspReportStartId != 0xFFFF)
    {
        U32 gpt_timer;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_timer);
        //aud_msg_ack(source->param.n9_a2dp.DspReportStartId, FALSE);
        STRM_A2DP_LOG_I("[Measure DSP Callback Streaming]First decode done, Ack A2DP start ID :%d Time :%d", 2, source->param.n9_a2dp.DspReportStartId,gpt_timer);
        source->param.n9_a2dp.DspReportStartId = 0xFFFF;
        n9_a2dp_param->buffer_empty_cnt = 0;
        //n9_a2dp_param->mce_flag = FALSE;
    }
    if(amount == 0) {
        return;
    }
    if ((n9_a2dp_param->buffer_empty_cnt > (reinit_thd*(!source->param.n9_a2dp.alc_monitor)))
        &&(n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG)){
        #ifdef AIR_AUDIO_HARDWARE_ENABLE
        U32 read_reg = afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1);
        if (read_reg != 0){
            //afe_set_hardware_digital_gain(AFE_HW_DIGITAL_GAIN1, 0);
            STRM_A2DP_LOG_I("Buffer status trigger re-sync empty_cnt:%d",1, n9_a2dp_param->buffer_empty_cnt);
            #if (ADATIVE_LATENCY_CTRL)
            if (source->param.n9_a2dp.alc_monitor)
            {
                STRM_A2DP_LOG_I("ALC request Latency : %d", 1, ADATIVE_LATENCY);
                Au_DL_send_alc_request(ADATIVE_LATENCY);
            }
            else
            {   //Gaming mode no need reinit
                if(source->param.n9_a2dp.latency_monitor == false){
                    if((n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) && (n9_a2dp_param->codec_info.codec_cap.codec.sbc.max_bit_pool == 37)) {
                        STRM_A2DP_LOG_I("by pass re-sync due to codec type is sbc and max_bit_pool is 37", 0);
                    } else {
                        Au_DL_send_reinit_request(MSG2_DSP2CN4_REINIT_BUF_ABNORMAL);
                    }
                }
            }
            #else
            //Gaming mode no need reinit
            if(source->param.n9_a2dp.latency_monitor == false){
                    if((n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) && (n9_a2dp_param->codec_info.codec_cap.codec.sbc.max_bit_pool == 37)) {
                        STRM_A2DP_LOG_I("by pass re-sync due to codec type is sbc and max_bit_pool is 37", 0);
                    } else {
                        Au_DL_send_reinit_request(MSG2_DSP2CN4_REINIT_BUF_ABNORMAL);
                    }
            }
            #endif
            n9_a2dp_param->buffer_empty_cnt = REINIT_FLAG;
        }
        else
        #endif /* AIR_AUDIO_HARDWARE_ENABLE */
        {
            n9_a2dp_param->buffer_empty_cnt = 0;
        }
    }else if (n9_a2dp_param->buffer_empty_cnt >= REINIT_FLAG)
    {
        n9_a2dp_param->buffer_empty_cnt = (n9_a2dp_param->buffer_empty_cnt > (REINIT_FLAG + (reinit_thd*RESEND_REINIT_CNT))) ? 0 : n9_a2dp_param->buffer_empty_cnt + 1;
    }
    n9_a2dp_param->predict_asi += share_buff_info->FrameSampleNum;
    if ((source->transform != NULL)&&(DSP_Callback_BypassModeGet(source,source->transform->sink)==BYPASS_CODEC_MODE)) {
        DSP_Callback_BypassModeCtrl(source,source->transform->sink,STREAMING_MODE);
        return;
    }
    if((n9_a2dp_param->buffer_empty_cnt < REINIT_FLAG)&&(source->transform != NULL)){
        if ((source->param.n9_a2dp.alc_monitor == FALSE)&&(source->param.n9_a2dp.sink_latency != 0))
        {
            dsp_notify_thd = ((source->param.n9_a2dp.sink_latency/1000)  > (NOTIFY_LOW_BOND + NOTIFY_LOW_THD)) ? ((source->param.n9_a2dp.sink_latency/1000) - NOTIFY_LOW_THD) : NOTIFY_LOW_BOND;
            //set a lower notify thd for gaming mode, no need resync, for debug logging only
            if(source->param.n9_a2dp.latency_monitor) {
                dsp_notify_thd = NOTIFY_LOW_BOND_GAMING_MODE;
            }
            buffer_notify =  A2dp_Buffer_Notify(source,dsp_notify_thd);
            n9_a2dp_param->buffer_empty_cnt = (buffer_notify) ? n9_a2dp_param->buffer_empty_cnt + 1 : (n9_a2dp_param->buffer_empty_cnt != 0) ? n9_a2dp_param->buffer_empty_cnt - 1 : 0;
            if(n9_a2dp_param->buffer_empty_cnt > (reinit_thd>>1))
            {
                STRM_A2DP_LOG_I("Buffer low cnt:%d ro:%d wo:%d seqn:%d thd:%d latency:%d", 6, n9_a2dp_param->buffer_empty_cnt, share_buff_info->ReadIndex, share_buff_info->WriteIndex,n9_a2dp_param->current_seq_num,dsp_notify_thd,((AVM_SHARE_BUF_INFO *)source->param.n9_a2dp.share_info_base_addr)->NotifyCount);
            }
        }
    }

    /* SBC part */
    if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC ) {
            if ( n9_a2dp_param->current_frame_size == amount ){
                if (n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt == 0)
                {
                    n9_a2dp_param->a2dp_bitrate_report.a2dp_report_cnt = sbc_report_bitrate(source);
                }
                share_information_read_index_update(source);
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
            share_information_read_index_update(source);
        }
        else {
            //TBD
        }
    }
    /* VENDOR part */
    else if ( n9_a2dp_param->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ) {
        #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
        if ((source->param.n9_a2dp.sink_latency != 0) && ( n9_a2dp_param->current_frame_size == amount ))
        #else
        if ( n9_a2dp_param->current_frame_size == amount )
        #endif
        {
            share_information_read_index_update(source);
        }
        else {
            //TBD
        }
    }
    else {
        //Not support codec type
    }
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

#if defined(MTK_GAMING_MODE_HEADSET) || defined(AIR_GAMING_MODE_DONGLE_ENABLE)
#define AIROHA_PKT_INVALIDE_SEQ_NO      (0xFFFF)
#define AIROHA_PKT_INVALIDE_FRAME       (0xFF)
#define AIROHA_GAMING_MUTE_FRAME_SIZE   (56)
#define AIROHA_GAMING_FRAME_SIZE        (120)

AIROHA_GAMING_CTRL gA2dpGamingCtrl;
static uint8_t AiroMutePacket[] = {0x31, 0x35, 0x36, 0x35, 0x3e, 0x32, 0x38, 0x32, 0x32};

uint8_t* a2dp_get_mute_packet (void)
{
    return AiroMutePacket;
}


uint16_t a2dp_get_mute_packet_size (void)
{
    return sizeof(AiroMutePacket)/sizeof(AiroMutePacket[0]);
}


void a2dp_insert_mute_packet (uint8_t* Addr)
{
    memcpy(Addr, a2dp_get_mute_packet(), a2dp_get_mute_packet_size());
}


BOOL a2dp_is_mute_packet (uint8_t* Addr)
{
    return (memcmp(Addr, a2dp_get_mute_packet(), a2dp_get_mute_packet_size()) == 0);
}

U8 a2dp_get_airoha_pkt_framesize(AIROHA_GAMING_BUFFER_PTR pkt_ptr)
{
    return pkt_ptr->FrameSize;
}

U8 a2dp_get_airoha_pkt_frameno(AIROHA_GAMING_BUFFER_PTR pkt_ptr)
{
    return pkt_ptr->FrameNo;
}

U32 a2dp_get_airoha_pkt_anchor(AIROHA_GAMING_BUFFER_PTR pkt_ptr)
{
    return pkt_ptr->NewestAnchor;
}


AIROHA_GAMING_FRAME_PTR a2dp_get_airoha_nth_frame_ptr(AIROHA_GAMING_BUFFER_PTR pkt_ptr, U32 n)
{
    AIROHA_GAMING_FRAME_PTR firstFrame = (AIROHA_GAMING_FRAME_PTR)&pkt_ptr->FrameBegin[0];
    U8 FrameSize = a2dp_get_airoha_pkt_framesize(pkt_ptr);

    //configASSERT((n <= a2dp_get_airoha_pkt_frameno(pkt_ptr)) && (n > 0));

    AIROHA_GAMING_FRAME_PTR nthFrame = (AIROHA_GAMING_FRAME_PTR)((U8*)firstFrame + (FrameSize + 1)*(n));

    return nthFrame;
}

BOOL a2dp_verify_airoha_pkt_checksum(AIROHA_GAMING_BUFFER_PTR pkt_ptr)
{
    U8 FrameSize = a2dp_get_airoha_pkt_framesize(pkt_ptr);
    U8 FrameNo = a2dp_get_airoha_pkt_frameno(pkt_ptr);
    U16 CheckSum = 0;
    U32 NewestAnchorVal = pkt_ptr->NewestAnchor;
    U32 Index = 0;

    for (Index = 0 ; Index < 4 ; Index++) {
        CheckSum += (NewestAnchorVal & 0xFF);
        NewestAnchorVal >>= 8;
    }

    for (Index = 0 ; Index < (U32)((FrameSize + 1)*FrameNo) ; Index++) {
        CheckSum += pkt_ptr->FrameBegin[Index];
    }

    #if (AIROHA_PKT_FAKE_VERIFY)
    return (FrameNo && FrameSize); // Temp for fake pattern
    #else
    return (FrameNo && FrameSize && (CheckSum == pkt_ptr->CheckSum));
    #endif
}


U16 a2dp_get_airoha_newest_seqno(AIROHA_GAMING_BUFFER_PTR pkt_ptr)
{
    U8 FrameNo = a2dp_get_airoha_pkt_frameno(pkt_ptr);
    AIROHA_GAMING_FRAME_PTR NewestFramePtr = a2dp_get_airoha_nth_frame_ptr(pkt_ptr, FrameNo-1);

    if (a2dp_verify_airoha_pkt_checksum(pkt_ptr)) {
        return (U16)(NewestFramePtr->SeqNo);
    } else {
        return AIROHA_PKT_INVALIDE_SEQ_NO;
    }
}


VOID a2dp_drop_airoha_current_pkt(SOURCE source)
{
    update_from_share_information(source);

    #if AIROHA_PKT_FAKE_VERIFY
    U32 mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    AiroFakeAvmShareBufInfo.ReadIndex = (AiroFakeAvmShareBufInfo.ReadIndex + 1) % AiroFakeAvmShareBufInfo.MemBlkNum;
    hal_nvic_restore_interrupt_mask(mask);
    #endif

    share_information_read_index_update(source);
}


AIROHA_GAMING_BUFFER_PTR a2dp_get_airoha_pkt_ptr_with_index_offset_n(SOURCE source, U32 n)
{
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);

    U16 ReadIdx = (share_buff_info->ReadIndex + n) % share_buff_info->MemBlkNum;

    update_from_share_information(source);

    AIROHA_GAMING_BUFFER_PTR pkt_ptr
        = (AIROHA_GAMING_BUFFER_PTR)(share_buff_info->StartAddr + ReadIdx*share_buff_info->MemBlkSize);

    return pkt_ptr;
}


AIROHA_GAMING_BUFFER_PTR a2dp_get_airoha_current_pkt_ptr(SOURCE source)
{
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);

    update_from_share_information(source);

    #if 0
    AIROHA_GAMING_BUFFER_PTR pkt_ptr
        = (AIROHA_GAMING_BUFFER_PTR)(share_buff_info->StartAddr + share_buff_info->ReadIndex*share_buff_info->MemBlkSize + sizeof(bt_codec_a2dp_hdr_type_t));
    #else
    AIROHA_GAMING_BUFFER_PTR pkt_ptr
        = (AIROHA_GAMING_BUFFER_PTR)(share_buff_info->StartAddr + share_buff_info->ReadIndex*share_buff_info->MemBlkSize);
    #endif
    return pkt_ptr;
}


U16 a2dp_get_airoha_current_pkt_cnt(SOURCE source)
{
    AVM_SHARE_BUF_INFO *share_buff_info  = &(source->streamBuffer.AVMBufferInfo);
    U16 PktCnt = 0;

    update_from_share_information(source);

    PktCnt = (share_buff_info->WriteIndex + share_buff_info->MemBlkNum - share_buff_info->ReadIndex) % share_buff_info->MemBlkNum;

    while (1) {
        update_from_share_information(source);

        if (PktCnt == (share_buff_info->WriteIndex + share_buff_info->MemBlkNum - share_buff_info->ReadIndex) % share_buff_info->MemBlkNum) {
            break;
        } else {
            PktCnt = (share_buff_info->WriteIndex + share_buff_info->MemBlkNum - share_buff_info->ReadIndex) % share_buff_info->MemBlkNum;
        }
    }
    return PktCnt;
}


U8 a2dp_get_airoha_suitable_frame(SOURCE source, U8 SeqNo)
{
    if ((!source->param.n9_a2dp.FrameNo) || (AIROHA_PKT_INVALIDE_SEQ_NO == source->param.n9_a2dp.PktNewestSeqNo)) {
        return AIROHA_PKT_INVALIDE_FRAME;
    }

    if (((S8)(SeqNo - ((U8)source->param.n9_a2dp.PktNewestSeqNo - source->param.n9_a2dp.FrameNo + 1)) >= 0) && ((S8)(SeqNo - (U8)source->param.n9_a2dp.PktNewestSeqNo) <= 0)) {
        return (SeqNo - ((U8)source->param.n9_a2dp.PktNewestSeqNo - source->param.n9_a2dp.FrameNo + 1));
    } else {
        return AIROHA_PKT_INVALIDE_FRAME;
    }
}


U32 a2dp_get_afe_sink_size(void)
{
    #ifdef AIR_AUDIO_HARDWARE_ENABLE
    return afe_get_dl1_query_data_amount();
    #else
    return 0;
    #endif /* AIR_AUDIO_HARDWARE_ENABLE */
}

void a2dp_ull_get_proper_play_en(BTCLK CurrCLK, U8 CurrSeqNo, BTCLK* ProperCLK, U8* ProperSeqNo)
{
    BTCLK bt_clk;
    BTPHASE bt_phase;

    STRM_A2DP_LOG_I("[AiroGaming] Play En Btc:0x%x, Seq No:%d", 2, (uint32_t)CurrCLK, (uint32_t)CurrSeqNo);

//    MCE_GetBtClk(&bt_clk, &bt_phase,ULL_CLK_Offset);
    MCE_GetBtClk(&bt_clk, &bt_phase,BT_CLK_Offset);

    STRM_A2DP_LOG_I("[AiroGaming] Current Btc:0x%x , phase:0x%x", 2, (uint32_t)bt_clk,bt_phase);

    /* Reserve at least 5ms for a2dp transform done */
    bt_clk = ((bt_clk + 8 /* BT phase complement */ + 32 /* 10ms reserve time */) & BTCLK_27_0_MASK);

    if ((BTCLK_HALF_ROUND_TIME & bt_clk) ^ (BTCLK_HALF_ROUND_TIME & CurrCLK)) {
        /* Wrap condition */
        if (BTCLK_HALF_ROUND_TIME & CurrCLK) {
            /* Wrap @ bt_clk */
            do {
                CurrCLK = ((CurrCLK + 8) & BTCLK_27_0_MASK);
                CurrSeqNo++;
            } while ((CurrCLK < bt_clk) && (BTCLK_HALF_ROUND_TIME & CurrCLK));
        }
    } else {
        /* No Wrap condition */
        if (CurrCLK < bt_clk) {
            do {
                CurrCLK = ((CurrCLK + 8) & BTCLK_27_0_MASK);
                CurrSeqNo++;
            } while (CurrCLK < bt_clk);
        }
    }

    *ProperCLK = CurrCLK;
    *ProperSeqNo = CurrSeqNo;

    STRM_A2DP_LOG_I("[AiroGaming] Proper Btc:0x%x, Seq No:%d", 2, (uint32_t)CurrCLK, (uint32_t)CurrSeqNo);
}


void a2dp_ull_set_init_seq_no(U8 SeqNo)
{
    U32 mask;

    hal_nvic_save_and_set_interrupt_mask(&mask);
    gA2dpGamingCtrl.CurrentSeqNo = SeqNo;
    gA2dpGamingCtrl.DebugSeqNo = SeqNo;
    hal_nvic_restore_interrupt_mask(mask);
}


void a2dp_ull_manually_update_seq_no(U8 SeqNo)
{
    U32 mask;
    UNUSED(SeqNo);
    hal_nvic_save_and_set_interrupt_mask(&mask);
    gA2dpGamingCtrl.CurrentSeqNo++;
    hal_nvic_restore_interrupt_mask(mask);
}


bool a2dp_ull_drop_expired_pkt(SOURCE source)
{
    AIROHA_GAMING_BUFFER_PTR pkt_ptr = a2dp_get_airoha_current_pkt_ptr(source);
    U16 PktCnt = a2dp_get_airoha_current_pkt_cnt(source);
    U16 SeqNo = a2dp_get_airoha_newest_seqno(pkt_ptr);
    U16 DropCnt = 0;
    /* Find next suitable packet */
    for (PktCnt = a2dp_get_airoha_current_pkt_cnt(source) ; PktCnt > 0 ; PktCnt--) {
        pkt_ptr = a2dp_get_airoha_current_pkt_ptr(source);
        SeqNo = a2dp_get_airoha_newest_seqno(pkt_ptr);
        if (SeqNo == AIROHA_PKT_INVALIDE_SEQ_NO) {
            a2dp_drop_airoha_current_pkt(source);
            DropCnt++;
            continue;
        } else if ((S8)(gA2dpGamingCtrl.CurrentSeqNo - (U8)SeqNo) > 0) {
            a2dp_drop_airoha_current_pkt(source);
            DropCnt++;
            continue;
        } else {
            return TRUE;
        }
    }
    #if (AIROHA_GAMING_HS_DL_DBG_LOG)
    //STRM_A2DP_LOG_I("[AiroGaming] Dropped Expired PktNo :%d, Curr SeqNo:%d", 2, (uint32_t)DropCnt, (uint32_t)gA2dpGamingCtrl.CurrentSeqNo);
    #endif
    //SLA_CustomLogging("F10",SA_STOP);
    return FALSE;
}


void a2dp_ull_seq_no_debug (void)
{
    gA2dpGamingCtrl.DebugSeqNo++;

    if (gA2dpGamingCtrl.CurrentSeqNo != gA2dpGamingCtrl.DebugSeqNo) {
        STRM_A2DP_LOG_I("[AiroGaming] Curr SeqNo:%d, Bt SeqNo:%d", 2, gA2dpGamingCtrl.CurrentSeqNo, gA2dpGamingCtrl.DebugSeqNo);
        /* Fix abnormal frame handling, sycn BT decided time */
        gA2dpGamingCtrl.CurrentSeqNo = gA2dpGamingCtrl.DebugSeqNo;
    }
}

S8 Check_SeqAnchor_Airoha_A2DP(SOURCE source)
{
    S32 NowAnchor = gA2dpGamingCtrl.AfeAnchor - ((source->param.n9_a2dp.sink_latency - 20000)*8/2500) - 20;
    S32 PacketAnchor = a2dp_get_airoha_pkt_anchor(source->param.n9_a2dp.pkt_ptr);
    STRM_A2DP_LOG_I("[AiroGaming]NowAnchor:0x%x,PacketAnchor:0x%x",2,NowAnchor,PacketAnchor);
    if((NowAnchor - PacketAnchor) < 8 && (NowAnchor - PacketAnchor) > -8){
        return 0;
    }else{
        if(NowAnchor > PacketAnchor){
            while(source->param.n9_a2dp.PktCnt > 1){
                a2dp_drop_airoha_current_pkt(source);
                source->param.n9_a2dp.PktCnt = a2dp_get_airoha_current_pkt_cnt(source);
                source->param.n9_a2dp.pkt_ptr = a2dp_get_airoha_current_pkt_ptr(source);
                source->param.n9_a2dp.FrameNo = a2dp_get_airoha_pkt_frameno(source->param.n9_a2dp.pkt_ptr);
                source->param.n9_a2dp.PktNewestSeqNo = a2dp_get_airoha_newest_seqno(source->param.n9_a2dp.pkt_ptr);
                PacketAnchor = a2dp_get_airoha_pkt_anchor(source->param.n9_a2dp.pkt_ptr);
                STRM_A2DP_LOG_I("[AiroGaming]Search Frame => NowAnchor:0x%x, PacketAnchor:0x%x, SeqNo:%d",3,NowAnchor,PacketAnchor,source->param.n9_a2dp.PktNewestSeqNo);
                if((NowAnchor - PacketAnchor) <= 8 && (NowAnchor - PacketAnchor) >= -8){
                    gA2dpGamingCtrl.CurrentSeqNo = source->param.n9_a2dp.PktNewestSeqNo;
                    STRM_A2DP_LOG_I("[AiroGaming]Rematch correct SeqNo:%d",1,gA2dpGamingCtrl.CurrentSeqNo);
                    return 1;
                }
            }
            return -1;
        }else{
            return -1;
        }
    }
}

ATTR_TEXT_IN_IRAM_LEVEL_1 BOOL SourceReadBuf_Airoha_A2DP(SOURCE source, U8 *dst_addr, U32 length)
{
    U8 CurrentFrame = a2dp_get_airoha_suitable_frame(source, gA2dpGamingCtrl.CurrentSeqNo);

    if((length != AIROHA_GAMING_MUTE_FRAME_SIZE && CurrentFrame != AIROHA_PKT_INVALIDE_FRAME && gA2dpGamingCtrl.Initialized && gA2dpGamingCtrl.MuteCnt > 10) || (gA2dpGamingCtrl.SeqNotMatchCnt > 10)){
        if((gA2dpGamingCtrl.CurrentSeqNo == source->param.n9_a2dp.PktNewestSeqNo) || (gA2dpGamingCtrl.SeqNotMatchCnt > 10)){
            S8 CheckValue = Check_SeqAnchor_Airoha_A2DP(source);
            if(CheckValue == 1){
                CurrentFrame = a2dp_get_airoha_suitable_frame(source, gA2dpGamingCtrl.CurrentSeqNo);
            }else if(CheckValue == -1){
                length = AIROHA_GAMING_MUTE_FRAME_SIZE;
                gA2dpGamingCtrl.BypassDrop = 1;
            }
        }
    }
    
    if (length == AIROHA_GAMING_MUTE_FRAME_SIZE) {
        #if (AIROHA_GAMING_HS_DL_DBG_LOG)
        STRM_A2DP_LOG_I("[AiroGaming] Insert mute due to afe buffer is low", 0);
        #endif
        if(gA2dpGamingCtrl.MuteCnt < 12){
            gA2dpGamingCtrl.MuteCnt++;
        }
        /* Insert Mute Frame */
        a2dp_insert_mute_packet(dst_addr);
    } else if (CurrentFrame != AIROHA_PKT_INVALIDE_FRAME) {
        #if (AIROHA_GAMING_HS_DL_DBG_LOG)
        STRM_A2DP_LOG_D("[AiroGaming] Seq no:%d is correctly copied", 1, (uint32_t)gA2dpGamingCtrl.CurrentSeqNo);
        #endif
        gA2dpGamingCtrl.SeqNotMatchCnt = 0;
        gA2dpGamingCtrl.MuteCnt = 0;
        memcpy(dst_addr, ((U8*)a2dp_get_airoha_nth_frame_ptr(source->param.n9_a2dp.pkt_ptr, CurrentFrame))+1, length);
    } else {
        #if (AIROHA_GAMING_HS_DL_DBG_LOG)
        STRM_A2DP_LOG_I("[AiroGaming] Can't find suitable frame, mute seq no:%d", 1, (uint32_t)gA2dpGamingCtrl.CurrentSeqNo);
        #endif
        /* Insert Mute Frame */
        a2dp_insert_mute_packet(dst_addr);
        gA2dpGamingCtrl.SeqNotMatchCnt++;
    }

    return TRUE;
}


ATTR_TEXT_IN_IRAM_LEVEL_1 U32 SourceSize_Airoha_A2DP(SOURCE source)
{
    #ifdef MTK_PEQ_ENABLE
    BTCLK bt_clk;
    BTPHASE bt_phase;
    #endif
    
    #if AIROHA_PKT_FAKE_VERIFY
    U32 mask;
    AIROHA_GAMING_BUFFER_PTR fake_pkt_ptr = NULL;
    if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT) {
        if ((a2dp_get_afe_sink_size() < gA2dpGamingCtrl.MinAfeSinkSize) && (a2dp_get_airoha_current_pkt_cnt(source) < 5)) {
            fake_pkt_ptr
                = (AIROHA_GAMING_BUFFER_PTR)(source->streamBuffer.AVMBufferInfo.StartAddr + AiroFakeAvmShareBufInfo.WriteIndex*AiroFakeAvmShareBufInfo.MemBlkSize);

            memcpy(fake_pkt_ptr, &AiroFakeDatabase[AiroFakeDatabaseIdx], FAKE_BLOCK_SIZE);

            STRM_A2DP_LOG_I("[AiroGaming] Insert Fake Pattern with Idx:%d, Wo:%d", 2, AiroFakeDatabaseIdx, AiroFakeAvmShareBufInfo.WriteIndex);

            AiroFakeDatabaseIdx = (AiroFakeDatabaseIdx + FAKE_BLOCK_SIZE) % sizeof(AiroFakeDatabase);

            hal_nvic_save_and_set_interrupt_mask(&mask);
            AiroFakeAvmShareBufInfo.WriteIndex = (AiroFakeAvmShareBufInfo.WriteIndex + 1) % AiroFakeAvmShareBufInfo.MemBlkNum;
            hal_nvic_restore_interrupt_mask(mask);
        }
    }
    #endif
	
    update_from_share_information(source);
    
    
    if (!gA2dpGamingCtrl.Initialized && audio_headset_ull_ctrl.is_play_en_ready) {
        source->param.n9_a2dp.PktCnt = a2dp_get_airoha_current_pkt_cnt(source);
        if(source->param.n9_a2dp.PktCnt > 0){
            source->param.n9_a2dp.pkt_ptr = a2dp_get_airoha_current_pkt_ptr(source);
            source->param.n9_a2dp.FrameNo = a2dp_get_airoha_pkt_frameno(source->param.n9_a2dp.pkt_ptr);

            if (source->param.n9_a2dp.FrameNo && a2dp_verify_airoha_pkt_checksum(source->param.n9_a2dp.pkt_ptr)) {
                //gA2dpGamingCtrl.CurrentSeqNo = a2dp_get_airoha_newest_seqno(pkt_ptr);
                STRM_A2DP_LOG_I("[AiroGaming] 1st Pkt, SeqNo:%d", 1, (uint32_t)gA2dpGamingCtrl.CurrentSeqNo);
                gA2dpGamingCtrl.FirstAnchor = a2dp_get_airoha_pkt_anchor(source->param.n9_a2dp.pkt_ptr);
                gA2dpGamingCtrl.Initialized = TRUE;
            } else {
                STRM_A2DP_LOG_I("[AiroGaming] 1st Pkt is bad, should never happen", 0);
                a2dp_drop_airoha_current_pkt(source);
            }
        }
    }else if(!gA2dpGamingCtrl.Initialized && !audio_headset_ull_ctrl.is_play_en_ready){
        gA2dpGamingCtrl.SeqNotMatchCnt = 0;
        gA2dpGamingCtrl.MuteCnt = 0;
        if (a2dp_get_afe_sink_size() < AIROHA_GAMING_FRAME_SIZE) {
            return AIROHA_GAMING_MUTE_FRAME_SIZE;
        }
    }

    if (!gA2dpGamingCtrl.Initialized) {
        STRM_A2DP_LOG_I("[AiroGaming] 1st Pkt is Bad or Empty, should better not happen", 0);
        if (audio_headset_ull_ctrl.is_play_en_ready) {
            if (a2dp_get_afe_sink_size() > AIROHA_GAMING_FRAME_SIZE) {
                /* No pre-decode */
                return 0;
            } else {
                return AIROHA_GAMING_MUTE_FRAME_SIZE;
            }
        } else {
            return 0;
        }
    } else {
        #ifdef MTK_PEQ_ENABLE
        //MCE_GetBtClk(&bt_clk, &bt_phase,ULL_CLK_Offset);
        MCE_GetBtClk(&bt_clk, &bt_phase,BT_CLK_Offset);
        PEQ_Update_Info(bt_clk,bt_clk);
        #endif
        source->param.n9_a2dp.PktCnt = a2dp_get_airoha_current_pkt_cnt(source);
        if (source->param.n9_a2dp.PktCnt > 0) {

            source->param.n9_a2dp.pkt_ptr = a2dp_get_airoha_current_pkt_ptr(source);
            source->param.n9_a2dp.FrameNo = a2dp_get_airoha_pkt_frameno(source->param.n9_a2dp.pkt_ptr);
            source->param.n9_a2dp.PktNewestSeqNo = a2dp_get_airoha_newest_seqno(source->param.n9_a2dp.pkt_ptr);
           
            if (source->param.n9_a2dp.FrameNo && a2dp_verify_airoha_pkt_checksum(source->param.n9_a2dp.pkt_ptr)) {
                if (a2dp_get_afe_sink_size() > (AIROHA_GAMING_FRAME_SIZE + (AIROHA_GAMING_FRAME_SIZE >> 1))) {
                    /* No pre-decode */
                    STRM_A2DP_LOG_I("[AiroGaming] AFE data:%d, is enough, so return 0",1,a2dp_get_afe_sink_size());
                    return 0;
                } else {
                    /*U32 anchor = a2dp_get_airoha_pkt_anchor(source->param.n9_a2dp.pkt_ptr);
                    STRM_A2DP_LOG_I("[AiroGaming]BT:0x%x, phase:0x%x, NewsSeq:%d, FrameNo:%d, CurrentSeqNo:%d, anchor:0x%x,AFE:0x%x,intra:0x%x",8,bt_clk,bt_phase,source->param.n9_a2dp.PktNewestSeqNo,source->param.n9_a2dp.FrameNo,gA2dpGamingCtrl.CurrentSeqNo,anchor,gA2dpGamingCtrl.AfeAnchor,gA2dpGamingCtrl.AfeAnchorIntra);*/
                    return source->param.n9_a2dp.pkt_ptr->FrameSize;
                }
            } else {
                #if (AIROHA_GAMING_HS_DL_DBG_LOG)
                STRM_A2DP_LOG_I("[AiroGaming] SourceSize Dropped pkt seq no:%d due to fail cheksum", 1, (uint32_t)source->param.n9_a2dp.PktNewestSeqNo);
                #endif
                a2dp_drop_airoha_current_pkt(source);
                return AIROHA_GAMING_MUTE_FRAME_SIZE;
            }
        } else if (a2dp_get_afe_sink_size() <= gA2dpGamingCtrl.MinAfeSinkSize + 4) {
            return AIROHA_GAMING_MUTE_FRAME_SIZE;
        }
        return 0;
    }
}


U8* SourceMap_Airoha_A2DP(SOURCE source)
{
    UNUSED(source);

    return NULL;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 VOID SourceDrop_Airoha_A2DP (SOURCE source, U32 amount)
{
    U16 PktCnt = source->param.n9_a2dp.PktCnt;
    U16 SeqNo = source->param.n9_a2dp.PktNewestSeqNo;
    U32 RemainSamples;
    U32 mask;

    if(gA2dpGamingCtrl.BypassDrop){
        gA2dpGamingCtrl.BypassDrop = 0;
        return;
    }

    if ((SeqNo == AIROHA_PKT_INVALIDE_SEQ_NO) && (PktCnt)) {
        a2dp_drop_airoha_current_pkt(source);
        #if (AIROHA_GAMING_HS_DL_DBG_LOG)
        STRM_A2DP_LOG_D("[AiroGaming] Dropped Abnormal Pkt Condition 1", 0);
        #endif
        PktCnt = a2dp_get_airoha_current_pkt_cnt(source);
    }

    if (amount) {
        hal_nvic_save_and_set_interrupt_mask(&mask);
        gA2dpGamingCtrl.CurrentSeqNo++;
        hal_nvic_restore_interrupt_mask(mask);

        if (amount == AIROHA_GAMING_MUTE_FRAME_SIZE) {
            /* Nothing needed to be dropped in mute frame handling */
        } else if (((S8)(gA2dpGamingCtrl.CurrentSeqNo - (U8)SeqNo) > 0)
         && (SeqNo != AIROHA_PKT_INVALIDE_SEQ_NO)) {
            a2dp_drop_airoha_current_pkt(source);
            #if (AIROHA_GAMING_HS_DL_DBG_LOG)
            STRM_A2DP_LOG_D("[AiroGaming] SourceDrop Normal Dropped Pkt with SeqNo: %d", 1, SeqNo);
            #endif
        }
    }

    RemainSamples = a2dp_get_afe_sink_size();

    #if (AIROHA_GAMING_HS_DL_DBG_LOG)
    STRM_A2DP_LOG_D("[AiroGaming] Pkt Cnt:%d, afe sink size:%d, seq no:%d", 3, PktCnt, RemainSamples, gA2dpGamingCtrl.CurrentSeqNo);
    #endif

    //#if defined(MTK_GAMING_MODE_HEADSET)
    #if 0
    if((Source_blks[SOURCE_TYPE_AUDIO] != NULL) && (Source_blks[SOURCE_TYPE_AUDIO]->param.audio.irq_exist == false) && (Source_blks[SOURCE_TYPE_AUDIO]->param.audio.is_memory_start == true) && (Source_blks[SOURCE_TYPE_AUDIO]->param.audio.afe_wait_play_en_cnt == 0))
    {
        if((Source_blks[SOURCE_TYPE_AUDIO]->transform != NULL) && (Source_blks[SOURCE_TYPE_AUDIO]->transform->sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (Source_blks[SOURCE_TYPE_AUDIO]->transform->sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MAX) && (Source_blks[SOURCE_TYPE_AUDIO]->transform->sink->param.data_ul.scenario_sub_id == 0))
        {
            U32 vul_irq_time;
            bool memory_start = Source_blks[SOURCE_TYPE_AUDIO]->param.audio.is_memory_start;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &vul_irq_time);
            hal_audio_trigger_start_parameter_t start_parameter;
            //start_parameter.memory_select = (HAL_AUDIO_MEMORY_UL_VUL1|HAL_AUDIO_MEMORY_UL_AWB2);
            start_parameter.memory_select = Source_blks[SOURCE_TYPE_AUDIO]->param.audio.mem_handle.memory_select;
            start_parameter.enable = true;
            STRM_A2DP_LOG_I("[audio transmitter] Enable vul interrupt, source:0x%x, sink:0x%x, id:%d, mem:0x%x, play_en_cnt:%d, time:%d, memory_start:0x%x", 7, Source_blks[SOURCE_TYPE_AUDIO]->type, Source_blks[SOURCE_TYPE_AUDIO]->transform->sink->type, Source_blks[SOURCE_TYPE_AUDIO]->transform->sink->param.data_ul.scenario_sub_id, start_parameter.memory_select, Source_blks[SOURCE_TYPE_AUDIO]->param.audio.afe_wait_play_en_cnt, vul_irq_time, memory_start);
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&start_parameter, HAL_AUDIO_SET_TRIGGER_MEMORY_START);
            Source_blks[SOURCE_TYPE_AUDIO]->param.audio.afe_wait_play_en_cnt = 1;
        }
    }
    #endif

    /* Find next suitable packet */
    if (a2dp_ull_drop_expired_pkt(source))
    {
        return;
    }

    /* No remaining frames */
    gA2dpGamingCtrl.AccumulatedSamples += RemainSamples;
    gA2dpGamingCtrl.CntIdx++;

    if (gA2dpGamingCtrl.CntIdx == 0)
    {
        gA2dpGamingCtrl.AccumulatedSamples >>= 8;

        #if (AIROHA_GAMING_HS_DL_DBG_LOG)
        STRM_A2DP_LOG_I("[AiroGaming] Avg remain samples:%d", 1, gA2dpGamingCtrl.AccumulatedSamples);
        #endif

        gA2dpGamingCtrl.AccumulatedSamples = 0;
    }

    #if (AIROHA_GAMING_HS_DL_DBG_LOG)
    STRM_A2DP_LOG_D("[AiroGaming] SourceDrop but can not find next Pkt!", 0);
    #endif

}


BOOL SourceConfigure_Airoha_A2DP(SOURCE source, stream_config_type type, U32 value)
{
    UNUSED(source);
    UNUSED(type);
    UNUSED(value);
    return TRUE;
}


BOOL SourceClose_Airoha_A2DP(SOURCE source)
{
    ((SHARE_BUFFER_INFO *)source->param.n9_a2dp.share_info_base_addr)->notify_count = 0;
    memset(&audio_headset_ull_ctrl,0,sizeof(audio_dsp_ull_start_ctrl_param_t));
    return TRUE;
}

#endif


VOID SourceInit_N9_a2dp(SOURCE source)
{
    /* buffer init */
    #ifdef MTK_BT_A2DP_VENDOR_BC_ENABLE
    if (( source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR ) && (source->param.n9_a2dp.sink_latency == 0))
    {
        p_vend_bc_exbuf_if = DSPMEM_tmalloc(source->taskId, sizeof(vend_bc_extern_buf_v2_t), (VOID*)source);
        p_vend_bc_exbuf_if->a2dp_source = source;
        //s_set_vend_bc_params(&(p_vend_bc_exbuf_if->param));
        p_vend_bc_exbuf_if->param = bc_params;
        U8* vend_bc_buf;
        STRM_A2DP_LOG_I("Vendor BC request buffer size : %d %d", 2,VEND_BC_HANDLE_SIZE,source->streamBuffer.AVMBufferInfo.MemBlkNum*source->streamBuffer.AVMBufferInfo.MemBlkSize);
        vend_bc_buf = reshape_share_buffer_for_vend_bc(source,(source->streamBuffer.AVMBufferInfo.MemBlkNum*source->streamBuffer.AVMBufferInfo.MemBlkSize+3)&0xFFFC);
        p_vend_bc_exbuf_if->p_handle = vend_bc_open(vend_bc_buf, VEND_BC_HANDLE_SIZE, &p_vend_bc_exbuf_if->param);
        vend_bc_write_loop_callback();
    }
    #endif
    //source->type = SOURCE_TYPE_N9_A2DP;
    source->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;

    #if (AIROHA_PKT_FAKE_VERIFY)
    if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) {
        source->param.n9_a2dp.codec_info.codec_cap.type = BT_A2DP_CODEC_AIRO_CELT;
    }
    #endif

    /* interface init */
    if (source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT) {
        #ifdef MTK_BT_A2DP_AIRO_CELT_ENABLE
        /* Test Code */
        source->sif.SourceSize        = SourceSize_Airoha_A2DP;
        source->sif.SourceMap         = SourceMap_Airoha_A2DP;
        source->sif.SourceConfigure   = SourceConfigure_Airoha_A2DP;
        source->sif.SourceDrop        = SourceDrop_Airoha_A2DP;
        source->sif.SourceClose       = SourceClose_Airoha_A2DP;
        source->sif.SourceReadBuf     = SourceReadBuf_Airoha_A2DP;

        #if (AIROHA_PKT_FAKE_VERIFY)
        source->streamBuffer.AVMBufferInfo.StartAddr = &AiroFakeShareData[0];
        AiroFakeDatabaseIdx = 0;
        gA2dpGamingCtrl.MinAfeSinkSize = AIROHA_GAMING_FRAME_SIZE*10;
        #else
        gA2dpGamingCtrl.MinAfeSinkSize = AIROHA_GAMING_FRAME_SIZE;
        #endif

        gA2dpGamingCtrl.AccumulatedSamples = 0;
        gA2dpGamingCtrl.CntIdx = 0;

        gA2dpGamingCtrl.Initialized = FALSE;
        gA2dpGamingCtrl.DebugSeqNo = 0;

        #else
        STRM_A2DP_LOG_I(" Codec Type:%d not supported", 1, source->param.n9_a2dp.codec_info.codec_cap.type);
        configASSERT(0);
        #endif
    } else {
        source->sif.SourceSize        = SourceSize_N9_a2dp;
        source->sif.SourceMap         = SourceMap_N9_a2dp;
        source->sif.SourceConfigure   = SourceConfigure_N9_a2dp;
        source->sif.SourceDrop        = SourceDrop_N9_a2dp;
        source->sif.SourceClose       = SourceClose_N9_a2dp;
        source->sif.SourceReadBuf     = SourceReadBuf_N9_a2dp;
    }

}




