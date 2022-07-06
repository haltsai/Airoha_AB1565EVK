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

#include "sbc.h"
#include "msbc_dec_interface.h"
#include "dsp_feature_interface.h"
#include "dsp_audio_process.h"
#include "dsp_dump.h"

#ifdef MTK_BT_A2DP_MSBC_USE_PIC
#include "msbc_dec_portable.h"
#endif


#define MSBC_DECODED_FRAME_SIZE    (120)
#define MSBC_PKT_OFFSET_INVALID    (0xFFFFFFFF)

/**
 *
 * Function Prototype
 *
 */
bool stream_codec_decoder_msbc_initialize (void *para);
bool stream_codec_decoder_msbc_process (void *para);

#ifdef MSBC_MONITOR_PACKET_OFFSET
#define MSBC_SYCN_WORD (0x01)
#define OFFSET_RESYNC_THD (4)
static U8 mSBC_pkt_offset;
static U8 msbc_seq_set[4] = {0x08,0x38,0xc8,0xf8};
#endif
static U16 msbc_seqn;
static U8 fgMsbcAbnormalSeqn;
static U32 g_prev_mSBC_pkt_offset;
EXTERN U16 escoseqn;

#ifdef MSBC_MONITOR_PACKET_OFFSET
BOOL msbc_subseqn_check(U8* inbuf)
{
    U8 i;
    for (i = 0 ; i < 4 ; i ++)
    {
        if (*(inbuf+1) == msbc_seq_set[i])
        {
            return TRUE;
        }
    }
    return FALSE;
}
VOID msbc_offsetcheck(U8* inbuf)
{
    U8* offset_check;
    U8 j;
    offset_check = (U8*)inbuf + mSBC_pkt_offset;
    fgMsbcAbnormalSeqn ++;
    if (*offset_check == MSBC_SYCN_WORD)
    {
        fgMsbcAbnormalSeqn = msbc_subseqn_check(offset_check) ? 0 : fgMsbcAbnormalSeqn;
    }
    if (fgMsbcAbnormalSeqn >= OFFSET_RESYNC_THD)
    {
        offset_check = (U8*)inbuf;
        for  (j = 0 ; j < 60 ; j++)
        {
            //printf("0x%x",*(offset_check + j));
            if (*(offset_check + j) == MSBC_SYCN_WORD)
            {
                DSP_MW_LOG_I("[msbc] 0x%x 0x%x", 2,*(offset_check + j + 1),*(offset_check + j - 1));
                if (msbc_subseqn_check(offset_check + j))
                {
                    mSBC_pkt_offset = j;
                    break;
                }
            }
        }
        DSP_MW_LOG_I("[msbc] offset resync offset : %d", 1,mSBC_pkt_offset);
    }
}

#endif

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID msbc_seqncheck(U16* inbuf, U16 i)
{
    UNUSED(i);
    fgMsbcAbnormalSeqn = 0;
    if (msbc_seqn != *inbuf)
    {
        //DSP_MW_LOG_I("incontious packet, seq : %d msbc : 0x%x 0x%x 0x%x",4, escoseqn + i, msbc_seqn, *inbuf,*(inbuf + 10));
        DSP_MW_LOG_I("incontious packet, seq : %d msbc:0x%x 0x%x 0x%x 0x%x",5, escoseqn + i, *inbuf, *(inbuf + 1), *(inbuf + 2), *(inbuf + 3));
        msbc_seqn = *inbuf;
    } else {
        //DSP_MW_LOG_I("[msbc] packet seq:%d, msbc:0x%x 0x%x 0x%x 0x%x",5,escoseqn + i, *inbuf, *(inbuf + 1), *(inbuf + 2), *(inbuf + 3));
    }

    {
        switch (msbc_seqn){
            case  0x0801:
                msbc_seqn = 0x3801;
                break;
            case  0x3801:
                msbc_seqn = 0xc801;
                break;
            case  0xc801:
                msbc_seqn = 0xf801;
                break;
            case  0xf801:
                msbc_seqn = 0x0801;
                break;
            default :
                DSP_MW_LOG_I("abnormal msbc seqn 0x%x 0x%x", 2, msbc_seqn, *inbuf);
                msbc_seqn = 0x0801;
                fgMsbcAbnormalSeqn = 1;
                break;
        }
    }
}


/**
 * stream_codec_decoder_msbc_initialize
 *
 * This function is used to init memory space for mSBC decoder
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
bool stream_codec_decoder_msbc_initialize (void *para)
{
    msbc_dec_init(stream_codec_get_workingbuffer(para));
    stream_codec_modify_output_size(para, MSBC_DECODED_FRAME_SIZE*2);//word->byte transfer
    stream_codec_modify_output_samplingrate(para, FS_RATE_16K);
    DSP_MW_LOG_D("mSBC decode init\r\n", 0);
    #ifdef MSBC_MONITOR_PACKET_OFFSET
    mSBC_pkt_offset = 0;
    g_prev_mSBC_pkt_offset = MSBC_PKT_OFFSET_INVALID;
    #endif
    return FALSE;
}


/**
 * stream_codec_decoder_msbc_process
 *
 * This function decodes a mSBC frame into single output channel
 *
 *
 * @para : Default parameter of callback function
 * @codecMemPtr : Memory allocated by callback function
 *
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 bool stream_codec_decoder_msbc_process (void *para)
{
    U16 mSBC_status = 0;
    S16* InBuf = (S16*)stream_codec_get_1st_input_buffer(para);
    S32* OutBuf = stream_codec_get_1st_output_buffer(para);
    //U16* pOutLength = stream_codec_get_output_size_pointer(para);
    U16 DecodeTimes,i;
	U8* castIn;
    U8* Offsetbuf = (U8*)stream_codec_get_workingbuffer(para) + sizeof(SBC_DEC_STATE);
    if( (stream_codec_get_input_size(para) == 0) ||((stream_codec_get_input_size(para) %60) !=0))
    {
        DSP_MW_LOG_I("mSBC Decode size mismatch, n = %d\r\n", 1, stream_codec_get_input_size(para));
    	stream_codec_modify_output_size(para, 0);
	    return FALSE;
    }
#ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    LOG_AUDIO_DUMP((U8*)InBuf, 120, VOICE_VC_IN1);
#endif
    DecodeTimes = (stream_codec_get_input_size(para) /60);
    for (i = 0 ; i< DecodeTimes ; i++)
    {
        #ifdef MSBC_MONITOR_PACKET_OFFSET
        msbc_offsetcheck((U8*)InBuf + i*60);
        castIn = (U8*)InBuf + i*60;
        if (mSBC_pkt_offset != 0)
        {
            memcpy(Offsetbuf + 60,castIn + mSBC_pkt_offset,60 - mSBC_pkt_offset);
            memmove(castIn + (60 - mSBC_pkt_offset),castIn,mSBC_pkt_offset);
            if (g_prev_mSBC_pkt_offset == MSBC_PKT_OFFSET_INVALID) {
                memcpy(castIn,Offsetbuf,(60 - mSBC_pkt_offset));
            } else {
                memcpy(castIn,Offsetbuf,g_prev_mSBC_pkt_offset);
            }
            memmove(Offsetbuf,Offsetbuf + 60,60 - mSBC_pkt_offset);
            g_prev_mSBC_pkt_offset = 60 - mSBC_pkt_offset;
        }
        if (fgMsbcAbnormalSeqn == 0)
        #endif
        {
            msbc_seqncheck((U16*)InBuf + i*30, i);
        } else {
            DSP_MW_LOG_I("msbc fgMsbcAbnormalSeqn != 0 :%d", fgMsbcAbnormalSeqn);
        }
        if ((fgMsbcAbnormalSeqn != 0) ||(mSBC_status =  msbc_dec_process(stream_codec_get_workingbuffer(para),(S16 *)((U8*)InBuf + i*60),(S16 *)((U8*)OutBuf + i*240),0)) )
        {
            /* Warning: Need error handling here */
            DSP_MW_LOG_I("mSBC decode failed or sequence number abnormal( %d ), mSBC_status:%d", 2, fgMsbcAbnormalSeqn, mSBC_status);
            Voice_PLC_CheckAndFillZeroResponse((S16 *)((U8*)InBuf + i*60), mSBC);
            msbc_dec_process(stream_codec_get_workingbuffer(para),  (S16 *)((U8*)InBuf + i*60),  (S16 *)((U8*)OutBuf + i*240), 0);
            VOICE_RX_INBAND_INFO_t RxPacketInfo;
            RxPacketInfo.RxEd = 0;
            RxPacketInfo.HecErr = 1;
            RxPacketInfo.CrcErr = 1;
            Voice_PLC_UpdateInbandInfo(&RxPacketInfo,sizeof(VOICE_RX_INBAND_INFO_t),i);
        }
        else
        {
            //printf("mSBC decode success");
        }

    }

#if 0
    if (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        DSP_Converter_16Bit_to_24bit(OutBuf, (S16*)OutBuf, OutLength);
        OutLength*=2;
    }
    stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));
    stream_codec_modify_output_size(para, OutLength);
    stream_codec_modify_output_samplingrate(para, FS_RATE_16K);
#else // old version
    if (stream_codec_get_output_resolution(para) == RESOLUTION_32BIT)
    {
        DSP_Converter_16Bit_to_24bit(OutBuf, (S16*)OutBuf, MSBC_DECODED_FRAME_SIZE*DecodeTimes);
        DecodeTimes *=2 ;
    }
    stream_codec_modify_resolution(para, stream_codec_get_output_resolution(para));

    stream_codec_modify_output_size(para, MSBC_DECODED_FRAME_SIZE*DecodeTimes*2);
    stream_codec_modify_output_samplingrate(para, FS_RATE_16K);
#endif

    return FALSE;
}


#ifdef PRELOADER_ENABLE
BOOL mSBC_Decoder_Open  (VOID* para)
{
	DSP_MW_LOG_I("[PIC] mSBC Decoder Open", 0);
	UNUSED(para);
	return TRUE;
}

BOOL mSBC_Decoder_Close (VOID* para)
{
	DSP_MW_LOG_I("[PIC] mSBC Decoder Close", 0);
	UNUSED(para);
	return TRUE;
}
#endif
