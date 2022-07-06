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

#define SBC_DEC_FRAME_LEN (128)
#define SBC_TS_RATIO_MAX  (0xF)

#include "hal_audio.h"
#include "hal_audio_internal.h"
#include "bt_a2dp_codec_internal.h"
//#define BT_A2DP_BITSTREAM_DUMP_DEBUG
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
/* #define AWS_DEBUG_CODE */
//#define AWS_DEBUG_CODE
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */

#ifdef MTK_BT_A2DP_SOURCE_SUPPORT
#include "sbc_codec.h"
#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/
#if defined(MTK_AVM_DIRECT)
#include "bt_sink_srv_ami.h"
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
extern xSemaphoreHandle g_xSemaphore_Audio;
void bt_audio_mutex_lock(xSemaphoreHandle handle)
{
    if (handle != NULL) {
        xSemaphoreTake(handle, portMAX_DELAY);
    }
}
void bt_audio_mutex_unlock(xSemaphoreHandle handle)
{
    if (handle != NULL) {
        xSemaphoreGive(handle);
    }
}
#endif
#include "audio_nvdm_common.h"
#include "audio_nvdm_coef.h"

#ifdef MTK_BT_SPEAKER_ENABLE
#include "bt_aws_mce_srv.h"
#endif

#ifdef MTK_BT_A2DP_SOURCE_SUPPORT
const bt_codec_sbc_t source_capability_sbc[1] = {
    {
        25,  /* min_bit_pool       */
        75,  /* max_bit_pool       */
        0x1, /* block_len: 16 only     */
        0x1, /* subband_num: 8 only   */
        SBC_ALLOCATION_SUPPORT,
        0xf, /* sample_rate: all   */
        0xf  /* channel_mode: all  */
    }
};
#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/

const bt_codec_sbc_t sink_capability_sbc[1] = {
    {
        18,  /* min_bit_pool       */
        75,  /* max_bit_pool       */
        0xf, /* block_len: all     */
        0xf, /* subband_num: all   */
        0x3, /* both snr/loudness  */
        0xf, /* sample_rate: all   */
        0xf  /* channel_mode: all  */
    }
};

const bt_codec_aac_t sink_capability_aac[1] = {
    {
        true,    /*VBR         */
        0xc0,    /*Object type */
        0x03,    /*Channels    */
        0x0ff8,  /*Sample_rate */
        0x60000  /*bit_rate, 384 Kbps */
    }
};

static void bt_set_buffer(bt_media_handle_t *handle, uint8_t *buffer, uint32_t length)
{
    handle->buffer_info.buffer_base = buffer;
    length &= ~0x1; // make buffer size even
    handle->buffer_info.buffer_size = length;
    handle->buffer_info.write = 0;
    handle->buffer_info.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_set_get_data_function(bt_media_handle_t *handle, bt_codec_get_data func)
{
    handle->directly_access_dsp_function = func;
}

static void bt_set_get_data_count_function(bt_media_handle_t *handle, bt_codec_get_data_count func)
{
    handle->get_data_count_function = func;
}

static void bt_get_write_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->buffer_info.read > handle->buffer_info.write) {
        count = handle->buffer_info.read - handle->buffer_info.write - 1;
    } else if (handle->buffer_info.read == 0) {
        count = handle->buffer_info.buffer_size - handle->buffer_info.write - 1;
    } else {
        count = handle->buffer_info.buffer_size - handle->buffer_info.write;
    }
    *buffer = handle->buffer_info.buffer_base + handle->buffer_info.write;
    *length = count;
}

static void bt_get_read_buffer(bt_media_handle_t *handle, uint8_t **buffer, uint32_t *length)
{
    int32_t count = 0;

    if (handle->buffer_info.write >= handle->buffer_info.read) {
        count = handle->buffer_info.write - handle->buffer_info.read;
    } else {
        count = handle->buffer_info.buffer_size - handle->buffer_info.read;
    }
    *buffer = handle->buffer_info.buffer_base + handle->buffer_info.read;
    *length = count;
}

static void bt_write_data_done(bt_media_handle_t *handle, uint32_t length)
{
    handle->buffer_info.write += length;
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_MSGID_I("[A2DP]write--wr: %d, len: %d\n",2, handle->buffer_info.write, length);
#endif
    if (handle->buffer_info.write == handle->buffer_info.buffer_size) {
        handle->buffer_info.write = 0;
    }
}

static void bt_finish_write_data(bt_media_handle_t *handle)
{
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_reset_share_buffer(bt_media_handle_t *handle)
{
    handle->buffer_info.write = 0;
    handle->buffer_info.read = 0;
    handle->waiting = false;
    handle->underflow = false;
}

static void bt_read_data_done(bt_media_handle_t *handle, uint32_t length)
{
    handle->buffer_info.read += length;
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_MSGID_I("[A2DP]read--rd: %d, len: %d\n",2, handle->buffer_info.read, length);
#endif
    if (handle->buffer_info.read == handle->buffer_info.buffer_size) {
        handle->buffer_info.read = 0;
    }
}

static int32_t bt_get_free_space(bt_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->buffer_info.read - handle->buffer_info.write - 1;
    if (count < 0) {
        count += handle->buffer_info.buffer_size;
    }
    return count;
}

static int32_t bt_get_data_count(bt_media_handle_t *handle)
{
    int32_t count = 0;

    count = handle->buffer_info.write - handle->buffer_info.read;
    if (count < 0) {
        count += handle->buffer_info.buffer_size;
    }
    return count;
}

static void bt_set_aws_flag(bt_media_handle_t *handle, bool is_aws)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->is_aws = is_aws;
#endif
}
static void bt_set_gaming_mode_flag(struct _bt_media_handle_t *handle, bool is_gaming)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->is_lm_en = is_gaming;
#endif
}
static void bt_set_alc_mode_flag(struct _bt_media_handle_t *handle, bool is_alc)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->is_alc_en = is_alc;
#endif
}


#if defined(MTK_AVM_DIRECT)
void Bt_c2d_buffercopy( void* DestBuf,
                        void* SrcBuf,
                        U16   CopySize,
                        void* SrcCBufStart,
                        U16   SrcCBufSize)
{
    U8* SrcCBufEnd      = (U8*)((U8*)SrcCBufStart + SrcCBufSize);
    U16 UnwrapSize      = (U8*)SrcCBufEnd - (U8*)SrcBuf; //Remove + 1 to sync more common usage

    if (CopySize > SrcCBufSize)
    {
        TASK_LOG_MSGID_E("[A2DP Codec]Bt_c2d_buffercopy CopySize more than SrcCBufSize", 0);
        configASSERT(0);
    }
    if (CopySize > UnwrapSize)
    {
        memcpy(DestBuf, SrcBuf, UnwrapSize);
        memcpy((U8*)DestBuf + UnwrapSize, SrcCBufStart, CopySize-UnwrapSize);
    }
    else
    {
        memcpy(DestBuf, SrcBuf, CopySize);
    }
}

static uint8_t bt_sbc_get_frame_num(uint32_t ts0, bt_media_handle_t *handle, uint8_t *sample_num)
{
    uint16_t TempReadOffset = 0;
    uint32_t TempTimeStamp  = 0;
    uint16_t TempPacketSize = 0;
    uint8_t  TempFrameNum   = 0;
    uint8_t  TempBlockLen   = 0;
    uint8_t  TempSubBand    = 0;
    uint8_t  sbc_frame_info_btye2;

    uint16_t buffer_size;
    uint8_t *p_buffer_base;
    n9_dsp_share_info_t *p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);

    if(!p_share_info)
    {
        TASK_LOG_MSGID_I("[A2DP Codec]p_share_info null", 0);
        return 0;
    }

    buffer_size = p_share_info->length;
    p_buffer_base = (uint8_t *)p_share_info->start_addr;

    do {
        TempReadOffset = (TempReadOffset + TempPacketSize)%(buffer_size);
        Bt_c2d_buffercopy( (void*)      &TempPacketSize,
                           (void*)      (p_buffer_base + (TempReadOffset + 2)%(buffer_size)), //PCB header: 4 bytes: U8 state, U8 data size, U16 size
                           (uint16_t)   2,
                           (void*)      p_buffer_base,
                           (uint16_t)   buffer_size);
        TempPacketSize += 4; //include PCB header length
        Bt_c2d_buffercopy( (void*)      &TempTimeStamp,
                           (void*)      (p_buffer_base + (TempReadOffset + 8)%(buffer_size)), //RTP header (after PCB): 12 bytes: U8 version, U8 type, U16 sequence no., U32 time stamp, U32 ssrc
                           (uint16_t)   4,
                           (void*)      p_buffer_base,
                           (uint16_t)   buffer_size);
    } while(TempTimeStamp != ts0);

    if (((bt_a2dp_audio_internal_handle_t *)handle)->is_cp == TRUE)
        TempReadOffset++;
    TempFrameNum = *(p_buffer_base + (TempReadOffset + 16)%(buffer_size));
    TempFrameNum &= 0x0F; //0~3 bit

    sbc_frame_info_btye2 = *(p_buffer_base + (TempReadOffset + 17)%(buffer_size));
    if(sbc_frame_info_btye2 == 0x9C) {
        sbc_frame_info_btye2 = *(p_buffer_base + (TempReadOffset + 18)%(buffer_size)); //subbands:1bit  allocation_method:1bit  channel_mode:2bits  blocks:2bits samplerate:2bits
        TempSubBand = ((sbc_frame_info_btye2&0x1) == 0) ? (uint8_t)4:(uint8_t)8;
        TempBlockLen = (((sbc_frame_info_btye2>>4)&0x3) + 1) * (uint8_t)4;
        *sample_num = (TempSubBand * TempBlockLen);
        TASK_LOG_MSGID_I("[SBC] sbc_frame_info_btye2: 0x%02x SubBand: %d BlockLen: %d FrameNum: %d",4, sbc_frame_info_btye2, TempSubBand, TempBlockLen, TempFrameNum);
    } else {
        TASK_LOG_MSGID_E("[SBC] sync word: 0x%02x ",1, sbc_frame_info_btye2);
    }

    return TempFrameNum;
}

static uint8_t bt_get_next_pkt_info(uint32_t timestamp, uint32_t *next_timestamp)
{
    uint16_t buffer_size;
    uint8_t *p_buffer_base;
    n9_dsp_share_info_t *p_share_info = (n9_dsp_share_info_t *)hal_audio_query_share_info(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
    uint16_t TempReadOffset = 0;
    uint32_t TempTimeStamp  = 0;
    uint16_t TempPacketSize = 0;

    if(!p_share_info)
    {
        TASK_LOG_MSGID_I("[A2DP Codec]p_share_info null", 0);
        return 0;
    }

    buffer_size = p_share_info->length;
    p_buffer_base = (uint8_t *)p_share_info->start_addr;

    do {
        TempReadOffset = (TempReadOffset + TempPacketSize)%(buffer_size);
        Bt_c2d_buffercopy( (void*)      &TempPacketSize,
                           (void*)      (p_buffer_base + (TempReadOffset + 2)%(buffer_size)), //PCB header: 4 bytes: U8 state, U8 data size, U16 size
                           (uint16_t)   2,
                           (void*)      p_buffer_base,
                           (uint16_t)   buffer_size);
        TempPacketSize += 4; //include PCB header length
        Bt_c2d_buffercopy( (void*)      &TempTimeStamp,
                           (void*)      (p_buffer_base + (TempReadOffset + 8)%(buffer_size)), //RTP header (after PCB): 12 bytes: U8 version, U8 type, U16 sequence no., U32 time stamp, U32 ssrc
                           (uint16_t)   4,
                           (void*)      p_buffer_base,
                           (uint16_t)   buffer_size);
    } while(TempTimeStamp != timestamp);

    TempReadOffset = (TempReadOffset + TempPacketSize)%(buffer_size);
    if(TempReadOffset != p_share_info->write_offset) {
        Bt_c2d_buffercopy( (void*)      next_timestamp,
                           (void*)      (p_buffer_base + (TempReadOffset + 8)%(buffer_size)),
                           (uint16_t)   4,
                           (void*)      p_buffer_base,
                           (uint16_t)   buffer_size);
        return TRUE;
    }
    return FALSE;
}

#endif //MTK_AVM_DIRECT

static uint32_t bt_get_ts_ratio(bt_media_handle_t *handle, uint32_t ts0, uint32_t ts1)
{
    #if defined(MTK_AVM_DIRECT)
    U8  frame_num;
    U8  sample_num = SBC_DEC_FRAME_LEN;
    U32 ts_ratio;

    frame_num = bt_sbc_get_frame_num(ts0, handle, &sample_num);
    if ((frame_num != 0) && (sample_num != 0)) {
        ts_ratio = (ts1 - ts0)/(frame_num * sample_num);

        if( ((ts_ratio * frame_num * sample_num) != (ts1 - ts0))
         || (ts0 == ts1))
        {
            ts_ratio = frame_num*sample_num;
        } else if (ts_ratio > SBC_TS_RATIO_MAX) {
            ts_ratio = 0;
        }
    } else {
        ts_ratio = 0;
    }

    TASK_LOG_MSGID_I("[A2DP Codec]ts1:%d, ts0:%d, frame_num:%d, sample_num:%d, ret_ts_ratio:%d",5,
        ts1, ts0, frame_num, sample_num, ts_ratio);

    return ts_ratio;
    #else
    return 1;
    #endif
}

static void bt_set_ts_ratio(bt_media_handle_t *handle, uint32_t ts_ratio)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->ts_ratio = ts_ratio;
#endif
}
#ifdef AVM_SHAEE_BUF_INFO
static void bt_set_start_asi(bt_media_handle_t *handle, uint32_t asi)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->start_asi = asi;
#endif
}

static void bt_set_start_bt_clk(bt_media_handle_t *handle, uint32_t bt_clk, uint32_t bt_intra_clk)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->start_bt_clk       = bt_clk;
    internal_handle->start_bt_intra_clk = bt_intra_clk;
#endif
}
#endif


static uint32_t bt_get_sampling_rate(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    uint32_t ret;
    uint32_t sample_rate_idx = internal_handle->sample_rate;

    switch (sample_rate_idx) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
            ret = 8000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
            ret = 11025;
            break;
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
            ret = 12000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            ret = 16000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
            ret = 22050;
            break;
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
            ret = 24000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
            ret = 32000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
            ret = 44100;
            break;
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            ret = 48000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_88_2KHZ:
            ret = 88200;
            break;
        case HAL_AUDIO_SAMPLING_RATE_96KHZ:
            ret = 96000;
            break;
        case HAL_AUDIO_SAMPLING_RATE_176_4KHZ:
            ret = 176400;
            break;
        case HAL_AUDIO_SAMPLING_RATE_192KHZ:
            ret = 192000;
            break;
        default:
            ret = 48000;
    }

    return ret;
}

static void bt_set_start_time_stamp(bt_media_handle_t *handle, uint32_t time_stamp)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->time_stamp = time_stamp;
#endif
}

static void bt_set_special_device(bt_media_handle_t *handle, bool is_special)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->is_lm_en = is_special;
#endif
}


static void bt_set_content_protection(bt_media_handle_t *handle, bool is_cp)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;

    internal_handle->is_cp = is_cp;
#endif
}

static uint32_t bt_get_max_frame_decoding_time(bt_media_handle_t *handle)                                    /**< Get the max frame decoding time (unit is 0.1 ms)  */
{
    return 232; //For 1024 sample in 44.1kHz
}

static void bt_codec_buffer_function_init(bt_media_handle_t *handle)
{
    handle->set_buffer         = bt_set_buffer;
    handle->set_get_data_function = bt_set_get_data_function;
    handle->set_get_data_count_function = bt_set_get_data_count_function;
    handle->get_write_buffer   = bt_get_write_buffer;
    handle->get_read_buffer    = bt_get_read_buffer;
    handle->write_data_done    = bt_write_data_done;
    handle->finish_write_data  = bt_finish_write_data;
    handle->reset_share_buffer = bt_reset_share_buffer;
    handle->read_data_done     = bt_read_data_done;
    handle->get_free_space     = bt_get_free_space;
    handle->get_data_count     = bt_get_data_count;
    handle->set_aws_flag                = bt_set_aws_flag;
    handle->set_gaming_mode_flag        = bt_set_gaming_mode_flag;
    handle->set_alc_mode_flag           = bt_set_alc_mode_flag;
    handle->get_ts_ratio                = bt_get_ts_ratio;
    handle->set_ts_ratio                = bt_set_ts_ratio;
    handle->get_sampling_rate           = bt_get_sampling_rate;
    handle->set_start_time_stamp        = bt_set_start_time_stamp;
    handle->set_special_devicce         = bt_set_special_device;
    handle->set_content_protection      = bt_set_content_protection;
    handle->get_max_frame_decoding_time = bt_get_max_frame_decoding_time;
    #ifdef AVM_SHAEE_BUF_INFO
    handle->set_start_asi               = bt_set_start_asi;
    handle->set_start_bt_clk            = bt_set_start_bt_clk;
    #endif
}

static void bt_side_tone_enable(bt_media_handle_t *handle)
{
    mcu2dsp_sidetone_param_t sidetone;
    void *p_param_share;

    if(!handle){
        TASK_LOG_MSGID_I("[A2DP Codec]SideTone Enable No Handle\n", 0);
        return;
    }

    TASK_LOG_MSGID_I("[A2DP Codec]SideTone Enable\n", 0);

    sidetone.in_device                       = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
    sidetone.in_interface                    = HAL_AUDIO_INTERFACE_1;
    sidetone.in_misc_parms                   = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
    sidetone.out_device                      = HAL_AUDIO_DEVICE_DAC_DUAL;
    sidetone.out_interface                   = HAL_AUDIO_INTERFACE_NONE;
    sidetone.out_misc_parms                  = DOWNLINK_PERFORMANCE_NORMAL;
    sidetone.in_channel                      = HAL_AUDIO_DIRECT;
    sidetone.gain                            = 600;
    sidetone.sample_rate                     = 16000;

    p_param_share = hal_audio_dsp_controller_put_paramter( &sidetone, sizeof(mcu2dsp_sidetone_param_t), AUDIO_MESSAGE_TYPE_SIDETONE);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_START, 0, (uint32_t)p_param_share, true);
}

static void bt_side_tone_disable(bt_media_handle_t *handle)
{
    mcu2dsp_sidetone_param_t sidetone;
    void *p_param_share;

    if(!handle){
        TASK_LOG_MSGID_I("[A2DP Codec]SideTone Disable No Handle\n", 0);
        return;
    }

    TASK_LOG_MSGID_I("[A2DP Codec]SideTone Disable\n", 0);

    sidetone.in_device                       = HAL_AUDIO_DEVICE_MAIN_MIC_DUAL;
    sidetone.in_interface                    = HAL_AUDIO_INTERFACE_1;
    sidetone.in_misc_parms                   = MICBIAS_SOURCE_ALL | MICBIAS3V_OUTVOLTAGE_1p85v;
    sidetone.out_device                      = HAL_AUDIO_DEVICE_DAC_DUAL;
    sidetone.out_interface                   = HAL_AUDIO_INTERFACE_NONE;
    sidetone.out_misc_parms                  = DOWNLINK_PERFORMANCE_NORMAL;
    sidetone.in_channel                      = HAL_AUDIO_DIRECT;
    sidetone.gain                            = 0;
    sidetone.sample_rate                     = 16000;

    p_param_share = hal_audio_dsp_controller_put_paramter( &sidetone, sizeof(mcu2dsp_sidetone_param_t), AUDIO_MESSAGE_TYPE_SIDETONE);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_COMMON_SIDETONE_STOP, 0, (uint32_t)p_param_share, true);
}

#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
#define BT_A2DP_BS_LEN 160000
uint32_t bt_a2dp_ptr = 0;
uint8_t bt_a2dp_bitstream[BT_A2DP_BS_LEN];
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */

#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__) && !defined(MTK_AVM_DIRECT)
static uint32_t bt_codec_a2dp_aws_convert_sampling_rate_from_index_to_value(uint32_t sampling_rate_index)
{
    uint32_t sampling_rate_value;
    if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_8KHZ) {
        sampling_rate_value = 8000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_11_025KHZ) {
        sampling_rate_value = 11025;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_12KHZ) {
        sampling_rate_value = 12000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_16KHZ) {
        sampling_rate_value = 16000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_22_05KHZ) {
        sampling_rate_value = 22050;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_24KHZ) {
        sampling_rate_value = 24000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_32KHZ) {
        sampling_rate_value = 32000;
    } else if (sampling_rate_index == HAL_AUDIO_SAMPLING_RATE_44_1KHZ) {
        sampling_rate_value = 44100;
    } else {
        sampling_rate_value = 48000;
    }
    return sampling_rate_value;
}



static void bt_codec_a2dp_aws_callback(aws_event_t event, void *user_data)
{
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)user_data;
    bt_media_handle_t *handle = (bt_media_handle_t *)p_info;
    switch (event) {
        case CODEC_AWS_CHECK_CLOCK_SKEW:
            handle->handler(handle, BT_CODEC_MEDIA_AWS_CHECK_CLOCK_SKEW);
            break;
        case CODEC_AWS_CHECK_UNDERFLOW:
            handle->handler(handle, BT_CODEC_MEDIA_AWS_CHECK_UNDERFLOW);
            break;
        default:
            break;
    }
    return;
}

static int32_t bt_codec_a2dp_aws_open_setting(bt_a2dp_audio_internal_handle_t *p_info)
{
    int32_t result = 0;
    bt_codec_a2dp_audio_t *p_codec = &p_info->codec_info;
    uint8_t *p_aws_buf = NULL;
    int32_t aws_buf_size = 0;
    p_info->aws_flag = false;
    p_info->aws_internal_flag = false;
    p_info->aws_init_sync_flag = false;
    if (p_codec->role == BT_A2DP_SINK) {
        bt_codec_capability_t *p_cap = &p_codec->codec_cap;
        bt_a2dp_codec_type_t type = p_cap->type;
        if (type == BT_A2DP_CODEC_SBC) {
            aws_buf_size = audio_service_aws_get_buffer_size(AWS_CODEC_TYPE_SBC_FORMAT);
            if (aws_buf_size < 0) {
                TASK_LOG_MSGID_E("[AWS] Fail: Buffer size is negative\n", 0);
                return -1;
            }
            p_info->aws_working_buf_size = aws_buf_size;
            p_aws_buf = (uint8_t *)pvPortMalloc(aws_buf_size);
            if (p_aws_buf == NULL) {
                TASK_LOG_MSGID_E("[AWS] Fail: Aws buf allocation failed\n", 0);
                return -2;
            }
            p_info->aws_working_buffer = p_aws_buf;
            audio_service_aws_init(p_aws_buf, AWS_CODEC_TYPE_SBC_FORMAT, (aws_callback_t)bt_codec_a2dp_aws_callback, (void *)p_info);
        }
#if defined(MTK_BT_A2DP_AAC_ENABLE)
        else if (type == BT_A2DP_CODEC_AAC) {
            aws_buf_size = audio_service_aws_get_buffer_size(AWS_CODEC_TYPE_AAC_FORMAT);
            if (aws_buf_size < 0) {
                TASK_LOG_MSGID_E("[AWS] Fail: Buffer size is negative\n", 0);
                return -1;
            }
            p_info->aws_working_buf_size = aws_buf_size;
            p_aws_buf = (uint8_t *)pvPortMalloc(aws_buf_size);
            if (p_aws_buf == NULL) {
                TASK_LOG_MSGID_E("[AWS] Fail: Aws buf allocation failed\n", 0);
                return -2;
            }
            p_info->aws_working_buffer = p_aws_buf;
            audio_service_aws_init(p_aws_buf, AWS_CODEC_TYPE_AAC_FORMAT, (aws_callback_t)bt_codec_a2dp_aws_callback, (void *)p_info);
        }
#endif  /* defined(MTK_BT_A2DP_AAC_ENABLE) */
        else {  /* Invalid codec type */
            result = -1;
        }
    }
    return result;
}

static void bt_codec_a2dp_aws_close_setting(bt_a2dp_audio_internal_handle_t *p_info)
{
    uint8_t *p_aws_buf = p_info->aws_working_buffer;
    audio_service_aws_deinit();
    if (p_aws_buf != NULL) {
        vPortFree(p_aws_buf);
        p_info->aws_working_buffer = (uint8_t *)NULL;
    }
    return;
}

static void bt_codec_a2dp_aws_play_setting(bt_a2dp_audio_internal_handle_t *p_info)
{
    p_info->aws_internal_flag = true;
    audio_service_aws_set_flag(true);
    return;
}

static void bt_codec_a2dp_aws_stop_setting(bt_a2dp_audio_internal_handle_t *p_info)
{
    audio_service_aws_set_flag(false);
    p_info->aws_internal_flag = false;
    p_info->aws_init_sync_flag = false;
    return;
}
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */

#if !defined(MTK_AVM_DIRECT)
static void bt_write_bs_to_dsp(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;
    bt_a2dp_codec_type_t codec_type = internal_handle->codec_info.codec_cap.type;
    ring_buffer_information_t *p_ring = &internal_handle->ring_info;
    uint16_t bs_page;
    uint16_t bs_addr;
    uint16_t bs_size;
    uint16_t bs_wptr;
    uint16_t bs_rptr;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        bs_page = *DSP_SBC_DEC_DM_BS_PAGE;
        bs_addr = *DSP_SBC_DEC_DM_BS_ADDR;
        bs_size = *DSP_SBC_DEC_DM_BS_LEN;
        bs_wptr = *DSP_SBC_DEC_DM_BS_MCU_W_PTR;
        bs_rptr = *DSP_SBC_DEC_DM_BS_DSP_R_PTR;
        p_ring->write_pointer       = (uint32_t)bs_wptr;
        p_ring->read_pointer        = (uint32_t)bs_rptr;
        p_ring->buffer_byte_count   = (uint32_t)bs_size;
        p_ring->buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(bs_page, bs_addr);
    }
  #ifdef MTK_BT_A2DP_AAC_ENABLE
    else if (codec_type == BT_A2DP_CODEC_AAC) {
        bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
        bs_wptr = *DSP_AAC_DEC_DM_BS_MCU_W_PTR;    // in word
        bs_rptr = *DSP_AAC_DEC_DM_BS_DSP_R_PTR;    // in word
        p_ring->write_pointer       = (uint32_t)((bs_wptr - bs_addr) << 1); // in byte
        p_ring->read_pointer        = (uint32_t)((bs_rptr - bs_addr) << 1);
    }
  #endif /*MTK_BT_A2DP_AAC_ENABLE*/
    {
        uint32_t loop_idx;
        uint32_t loop_cnt = 4;
        for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
            uint32_t           read_byte_cnt  = 0;
            uint32_t           write_byte_cnt = 0;
            uint32_t           move_byte_cnt = 0;
            uint8_t           *p_mcu_buf      = NULL;
            volatile uint16_t *p_dsp_buf      = NULL;

            if (codec_type == BT_A2DP_CODEC_SBC) {
                ring_buffer_get_write_information(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
            }
  #ifdef MTK_BT_A2DP_AAC_ENABLE
            else if (codec_type == BT_A2DP_CODEC_AAC) {
                ring_buffer_get_write_information_non_mirroring(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);
            }
  #endif /*MTK_BT_A2DP_AAC_ENABLE*/

            write_byte_cnt &= ~0x1;     // Make it even

            if (handle->directly_access_dsp_function == NULL) {
                handle->get_read_buffer(handle, &p_mcu_buf, &read_byte_cnt);
                read_byte_cnt  &= ~0x1;     // Make it even

                move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
                {
                    // Move data
                    uint32_t move_word_cnt = move_byte_cnt >> 1;
                    if (move_word_cnt > 0) {
  #if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
                        if (bt_a2dp_ptr + move_byte_cnt <= BT_A2DP_BS_LEN) {
                            memcpy(bt_a2dp_bitstream + bt_a2dp_ptr, p_mcu_buf, move_byte_cnt);
                            bt_a2dp_ptr += move_byte_cnt;
                        }
  #endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
                        audio_idma_write_to_dsp(p_dsp_buf, (uint16_t *)p_mcu_buf, move_word_cnt);
                    } else {    // Read buffer empty or write buffer full
                        break;
                    }
                }

                handle->read_data_done(handle, move_byte_cnt);
            } else {
                uint32_t write_word_cnt = write_byte_cnt >> 1;
                uint32_t wrote_word_cnt = 0;    // how many word written by directly_access_dsp_function
                wrote_word_cnt = handle->directly_access_dsp_function(p_dsp_buf, write_word_cnt);
                move_byte_cnt = wrote_word_cnt << 1;
                if (wrote_word_cnt == 0) {
                    break;
                }
            }

            if (codec_type == BT_A2DP_CODEC_SBC) {
                ring_buffer_write_done(p_ring, move_byte_cnt);
                *DSP_SBC_DEC_DM_BS_MCU_W_PTR = (uint16_t)p_ring->write_pointer;
            }
  #ifdef MTK_BT_A2DP_AAC_ENABLE
            else if (codec_type == BT_A2DP_CODEC_AAC) {
                ring_buffer_write_done_non_mirroring(p_ring, move_byte_cnt);
                *DSP_AAC_DEC_DM_BS_MCU_W_PTR = (uint16_t)(p_ring->write_pointer >> 1) + bs_addr;
            }
  #endif /*MTK_BT_A2DP_AAC_ENABLE*/
        }
    }
    *DSP_TASK4_COSIM_HANDSHAKE = 0;
    return;
}
#endif

#if defined(MTK_AVM_DIRECT)
static void sbc_decoder_isr_handler(hal_audio_event_t event, void *data)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;

    if (event == HAL_AUDIO_EVENT_TIME_REPORT) {
        handle->handler(handle, BT_CODEC_MEDIA_TIME_REPORT);
    } else if(event == HAL_AUDIO_EVENT_LTCS_REPORT) {
        handle->handler(handle, BT_CODEC_MEDIA_LTCS_DATA_REPORT);
    } else if(event == HAL_AUDIO_EVENT_LTCS_TIMEOUT) {
        handle->handler(handle, BT_CODEC_MEDIA_LTCS_DATA_TIMEOUT);
    } else if(event == HAL_AUDIO_EVENT_DL_REINIT_REQUEST) {

        TASK_LOG_MSGID_I("[AWS]sbc_decoder_isr_handler reinitial sync\r\n", 0);
        handle->handler(handle, BT_CODEC_MEDIA_AUDIO_DL_REINIT_REQUEST);
    } else if(event == HAL_AUDIO_EVENT_ALC_REQUEST) {
        handle->handler(handle, BT_CODEC_MEDIA_AUDIO_DL_ALC_REQUEST);
    } else {
        // KH: Assume that all events are error
        handle->handler(handle, BT_CODEC_MEDIA_ERROR);
        handle->state = BT_CODEC_STATE_ERROR;
    }
}
#else
static void sbc_decoder_isr_handler(void *data)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;
#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_MSGID_I("[A2DP][SBC]ISR", 0);
#endif
    if ((GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_IDLE) || (handle->state == BT_CODEC_STATE_ERROR)) {
        return;
    }

    /* error handling */
    if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
        if ((GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_SYNC_ERR)
                || (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_CRC_ERR)
                || (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_BITPOOL_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_MAGIC_WORD_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_NON_FRAGMENTED_PAYLOAD)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_START_PAYLOAD_ERR)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAGMENT_INDEX_1)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAGMENT_INDEX_2)
                || (GET_DSP_VALUE(RG_SBC_PAR_STATUS) == DSP_SBC_PAR_INVALID_FRAME_NUMBER)
           ) {
            LISR_LOG_MSGID_E("[A2DP][SBC]DECODER ERR, PAR:%d  DEC=%d\n",2, GET_DSP_VALUE(RG_SBC_PAR_STATUS), GET_DSP_VALUE(RG_SBC_DEC_STATUS));
            handle->handler(handle, BT_CODEC_MEDIA_ERROR);
            handle->state = BT_CODEC_STATE_ERROR;
            return;
        }
    }
    internal_handle->frame_count++;
    /* fill bitstream */
    bt_write_bs_to_dsp(internal_handle);
    if (handle->directly_access_dsp_function == NULL) {//Share buffer flow
        if (!handle->waiting) {
            handle->waiting = true;
            if ((!handle->underflow) && (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_BS_UNDERFLOW)) {
                handle->underflow = true;
                LISR_LOG_MSGID_I("[A2DP][SBC] DSP underflow \n", 0);
                handle->handler(handle, BT_CODEC_MEDIA_UNDERFLOW);
            } else {
                handle->handler(handle, BT_CODEC_MEDIA_REQUEST);
            }
        }
    }
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    else { //Direct access DSP flow
        if (GET_DSP_VALUE(RG_SBC_DEC_STATUS) == DSP_SBC_DEC_BS_UNDERFLOW) {
            if (internal_handle->aws_internal_flag) {
                LISR_LOG_MSGID_I("[A2DP][SBC] DSP underflow \n", 0);
                handle->handler(handle, BT_CODEC_MEDIA_UNDERFLOW);
            }
        }
    }
  #endif
}
#endif

static bt_codec_media_status_t bt_open_sink_sbc_codec(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    mcu2dsp_start_param_t start_param;
    void *p_param_share;

    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, sbc_decoder_isr_handler, internal_handle);

    // Collect parameters
    start_param.param.stream_in     = STREAM_IN_A2DP;
    start_param.param.stream_out    = STREAM_OUT_AFE;

    start_param.stream_in_param.a2dp.content_protection_exist = internal_handle->is_cp;
    #ifdef AVM_SHAEE_BUF_INFO
    start_param.stream_in_param.a2dp.start_asi = internal_handle->start_asi;
    start_param.stream_in_param.a2dp.start_bt_clk = internal_handle->start_bt_clk;
    start_param.stream_in_param.a2dp.start_bt_intra_clk = internal_handle->start_bt_intra_clk;
    #else
    start_param.stream_in_param.a2dp.start_time_stamp = internal_handle->time_stamp;
    start_param.stream_in_param.a2dp.time_stamp_ratio = internal_handle->ts_ratio;
    #endif
    start_param.stream_in_param.a2dp.alc_enable = internal_handle->is_alc_en;
    start_param.stream_in_param.a2dp.latency_monitor_enable = internal_handle->is_lm_en;
    start_param.stream_out_param.afe.aws_flag   =  internal_handle->is_aws;

    if(internal_handle->is_lm_en == true){
        hal_dvfs_lock_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_DVFS_LOCK);
    }

    p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_START, 0, (uint32_t)p_param_share, true);

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
    ami_execute_vendor_se(EVENT_AFTER_A2DP_START);
#endif
#endif

#else
    uint16_t I = 0;
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_MSGID_CTRL("[SBC]open_codec\r\n", 0);
    audio_service_hook_isr(DSP_D2C_SBC_DEC_INT, sbc_decoder_isr_handler, internal_handle);
    audio_service_setflag(handle->audio_id);
    if (GET_DSP_VALUE(RG_SBC_DEC_FSM) != DSP_SBC_STATE_IDLE) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
  #if defined(AWS_DEBUG_CODE)
    TASK_LOG_MSGID_I("[AWS] set AWS flag\r\n", 0);
    bt_codec_a2dp_aws_set_flag(handle, true);
  #endif  /* defined(AWS_DEBUG_CODE) */
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_flag) {
        bt_codec_a2dp_aws_play_setting(internal_handle);
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    internal_handle->frame_count = 0;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    *DSP_SBC_PAR_MAGIC_WORD = 0x3453;
    SET_DSP_VALUE(RG_SBC_PARSER_EN, 1);
    SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_START);
    *DSP_SBC_DEC_DM_BS_MCU_W_PTR = 0;
    dsp_audio_fw_dynamic_download(DDID_SBC_DECODE);
    afe_set_path_type(HAL_AUDIO_PLAYBACK_MUSIC);
    audio_playback_on(ASP_TYPE_SBC_DEC, internal_handle->sample_rate);
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_internal_flag == false)
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    {
        for (I = 0; ; I++) {
            if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
                break;
            }
            if (I > 80) {
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
  #if defined(AWS_DEBUG_CODE)
    else {  /* Manually reset DSP for unit test */
        bt_codec_a2dp_aws_set_initial_sync(handle);
        for (I = 0; ; I++) {
            if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
                break;
            }
            if (I > 80) {
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
  #endif  /* defined(AWS_DEBUG_CODE) */
#endif

    handle->state = BT_CODEC_STATE_PLAY;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_close_sink_sbc_codec(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
    ami_execute_vendor_se(EVENT_BEFORE_A2DP_STOP);
#endif
#endif
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_STOP, 0, 0, true);
    hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
    if(internal_handle->is_lm_en == true){
        hal_dvfs_lock_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_DVFS_UNLOCK);
    }
#else
    uint16_t I = 0;
    TASK_LOG_MSGID_CTRL("[SBC]close_codec\r\n", 0);
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag && !internal_handle->aws_flag && !internal_handle->aws_init_sync_flag) {
            SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_IDLE);
        }
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    for (I = 0; ; I++) {
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_IDLE) {
            break;
        }
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
            SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_FLUSH);
        }
        if (I > 80) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        hal_gpt_delay_ms(9);
    }
    SET_DSP_VALUE(RG_SBC_PARSER_EN, 0);

    *DSP_AUDIO_CTRL2 &= ~DSP_PCM_R_DIS;
    *DSP_AUDIO_FLEXI_CTRL &= ~(FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    audio_playback_off();
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag) {
            bt_codec_a2dp_aws_stop_setting(internal_handle);
        }
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
  #if defined(AWS_DEBUG_CODE)
    TASK_LOG_MSGID_I("[AWS] clear AWS flag\r\n", 0);
    bt_codec_a2dp_aws_set_flag(handle, false);
  #endif  /* defined(AWS_DEBUG_CODE) */
    audio_service_unhook_isr(DSP_D2C_SBC_DEC_INT);
    audio_service_clearflag(handle->audio_id);
#endif

    handle->state = BT_CODEC_STATE_STOP;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_sbc_play(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_audio_mutex_lock(g_xSemaphore_Audio);
#endif
    bt_codec_media_status_t ret;
    TASK_LOG_MSGID_CTRL("[SBC]play\r\n", 0);
    if (handle->state != BT_CODEC_STATE_READY && handle->state != BT_CODEC_STATE_STOP) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
    bt_a2dp_ptr = 0;
    memset(bt_a2dp_bitstream, 0, BT_A2DP_BS_LEN * sizeof(uint8_t));
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
    ret = bt_open_sink_sbc_codec(handle);
#if defined(MTK_AVM_DIRECT)
    bt_audio_mutex_unlock(g_xSemaphore_Audio);
#endif
    return ret;
}

static bt_codec_media_status_t bt_a2dp_sink_sbc_stop(bt_media_handle_t *handle)
{
    bt_codec_media_status_t result;
    TASK_LOG_MSGID_CTRL("[SBC]stop--state: %d\r\n",1, handle->state);
    if (handle->state == BT_CODEC_STATE_READY) {
        handle->state = BT_CODEC_STATE_STOP;
        result = BT_CODEC_MEDIA_STATUS_OK;
    } else if (handle->state != BT_CODEC_STATE_PLAY && handle->state != BT_CODEC_STATE_ERROR) {
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    } else {
        result = bt_close_sink_sbc_codec(handle);
    }
    return result;
}

static bt_codec_media_status_t bt_a2dp_sink_sbc_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_parse_sbc_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    uint8_t channel_mode, sample_rate;
    bt_codec_a2dp_audio_t *pParam = (bt_codec_a2dp_audio_t *)&internal_handle->codec_info;

    channel_mode = pParam->codec_cap.codec.sbc.channel_mode;
    sample_rate  = pParam->codec_cap.codec.sbc.sample_rate;
    TASK_LOG_MSGID_I("[A2DP][SBC]sample rate=%d, channel=%d \n",2, sample_rate, channel_mode);
    switch (channel_mode) {
        case 8:
            internal_handle->channel_number = 1;
            break;
        case 4:
        case 2:
        case 1:
            internal_handle->channel_number = 2;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_channel_number((internal_handle->channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO);

    switch (sample_rate) {
        case 8:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 4:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 2:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 1:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_sampling_rate((hal_audio_sampling_rate_t)internal_handle->sample_rate);

    return BT_CODEC_MEDIA_STATUS_OK;
}

#ifdef MTK_BT_A2DP_AAC_ENABLE
/* aac */
#if !defined(MTK_AVM_DIRECT)
static void aac_write_silence(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    ring_buffer_information_t *p_ring = &internal_handle->ring_info;
    uint16_t bs_addr;
    uint16_t bs_wptr;
    uint16_t bs_rptr;

    bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
    bs_wptr = *DSP_AAC_DEC_DM_BS_MCU_W_PTR;    // in word
    bs_rptr = *DSP_AAC_DEC_DM_BS_DSP_R_PTR;    // in word
    p_ring->write_pointer       = (uint32_t)((bs_wptr - bs_addr) << 1); // in byte
    p_ring->read_pointer        = (uint32_t)((bs_rptr - bs_addr) << 1);

    {
        uint32_t loop_idx;
        uint32_t loop_cnt = 2;
        int32_t read_byte_cnt = SILENCE_TOTAL_LENGTH;
        for (loop_idx = 0; loop_idx < loop_cnt; loop_idx++) {
            uint32_t           write_byte_cnt = 0;
            uint32_t           move_byte_cnt;
            uint8_t            *p_mcu_buf      = internal_handle->aac_silence_pattern;
            volatile uint16_t  *p_dsp_buf      = NULL;

            p_mcu_buf += (SILENCE_TOTAL_LENGTH - read_byte_cnt);
            ring_buffer_get_write_information_non_mirroring(p_ring, (uint8_t **)&p_dsp_buf, &write_byte_cnt);

            write_byte_cnt &= ~0x1;     // Make it even
            move_byte_cnt = MINIMUM(write_byte_cnt, read_byte_cnt);
            {
                // Move data
                uint32_t move_word_cnt = move_byte_cnt >> 1;
                if (move_word_cnt > 0) {
                    audio_idma_write_to_dsp(p_dsp_buf, (uint16_t *)p_mcu_buf, move_word_cnt);
                    read_byte_cnt -= (move_word_cnt << 1);
                } else {    // Read buffer empty or write buffer full
                    break;
                }
            }
            ring_buffer_write_done_non_mirroring(p_ring, move_byte_cnt);
            *DSP_AAC_DEC_DM_BS_MCU_W_PTR = (uint16_t)(p_ring->write_pointer >> 1) + bs_addr;
        }
    }
    return;
}
#endif /*MTK_AVM_DIRECT */

#if defined(MTK_AVM_DIRECT)
static void aac_decoder_isr_handler(hal_audio_event_t event, void *data)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;

    if (event == HAL_AUDIO_EVENT_TIME_REPORT) {
        handle->handler(handle, BT_CODEC_MEDIA_TIME_REPORT);
    } else if(event == HAL_AUDIO_EVENT_LTCS_REPORT) {
        handle->handler(handle, BT_CODEC_MEDIA_LTCS_DATA_REPORT);
    } else if(event == HAL_AUDIO_EVENT_LTCS_TIMEOUT) {
        handle->handler(handle, BT_CODEC_MEDIA_LTCS_DATA_TIMEOUT);
    } else if(event == HAL_AUDIO_EVENT_DL_REINIT_REQUEST) {

        TASK_LOG_MSGID_I("[AWS]aac_decoder_isr_handler reinitial sync\r\n", 0);
        handle->handler(handle, BT_CODEC_MEDIA_AUDIO_DL_REINIT_REQUEST);
    } else if(event == HAL_AUDIO_EVENT_ALC_REQUEST) {
        handle->handler(handle, BT_CODEC_MEDIA_AUDIO_DL_ALC_REQUEST);
    } else {
        // KH: Assume that all events are error
        handle->state = BT_CODEC_STATE_ERROR;
        handle->handler(handle, BT_CODEC_MEDIA_ERROR);
    }
}
#else
static void aac_decoder_isr_handler(void *data)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)data;
    bt_media_handle_t *handle = (bt_media_handle_t *)internal_handle;

  #ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
    TASK_LOG_MSGID_I("[A2DP[AAC]ISR", 0);
  #endif

    if ((*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) || (handle->state == BT_CODEC_STATE_ERROR)) {
        return;
    }

    /* error handling, but bypass the buffer underflow warning from DSP */
    if ((*DSP_AAC_DEC_ERROR_REPORT != DSP_AAC_REPORT_NONE)
            && (*DSP_AAC_DEC_ERROR_REPORT != DSP_AAC_REPORT_UNDERFLOW)) {
        internal_handle->error_count ++;

        /* fill silence when underflow continuously */
        if (*DSP_AAC_DEC_ERROR_REPORT == DSP_AAC_REPORT_BUFFER_NOT_ENOUGH) {

            int32_t mcu_data_count;
            if (handle->get_data_count_function) {
                mcu_data_count = handle->get_data_count_function();
            } else {
                mcu_data_count = handle->get_data_count(handle);
            }

            if (mcu_data_count < AAC_FILL_SILENCE_TRHESHOLD) {
                aac_write_silence(internal_handle);
            }
        } else {
            LISR_LOG_MSGID_E("[A2DP]DECODER ERR(%x), FSM:%x REPORT=%x\n",3, (unsigned int)internal_handle->error_count, *DSP_AAC_DEC_FSM, *DSP_AAC_DEC_ERROR_REPORT);
        }

        LISR_LOG_MSGID_E("[A2DP][AAC]DECODER ERR, FSM:%x  REPORT=%x\n",2, *DSP_AAC_DEC_FSM, *DSP_AAC_DEC_ERROR_REPORT);
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
        if (internal_handle->aws_internal_flag) {
            LISR_LOG_MSGID_E("[A2DP][AAC]DECODER ERR AWS\n", 0);
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_STOP;
            handle->state = BT_CODEC_STATE_ERROR;
            handle->handler(handle, BT_CODEC_MEDIA_ERROR);
            return;
        }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
        if (internal_handle->error_count >= AAC_ERROR_FRAME_THRESHOLD) {
            LISR_LOG_MSGID_E("[A2DP][AAC]DECODER ERR OVER THRESHOLD\n", 0);
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_STOP;
            handle->state = BT_CODEC_STATE_ERROR;
            handle->handler(handle, BT_CODEC_MEDIA_ERROR);
            return;
        }
    } else { //if error is not consecutive, reset to 0
        internal_handle->error_count = 0;
    }

    /* bitstream buffer initialization */
    if (!internal_handle->ring_info.buffer_base_pointer) {
        uint16_t bs_page = DSP_AAC_PAGE_NUM;
        uint16_t bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
        uint16_t bs_size = *DSP_AAC_DEC_DM_BS_LEN << 1;
        internal_handle->ring_info.buffer_byte_count = (uint32_t)bs_size;
        internal_handle->ring_info.buffer_base_pointer = (uint8_t *)DSP_DM_ADDR(bs_page, bs_addr);
    }
    internal_handle->frame_count++;
    if (internal_handle->frame_count == 1) {
        uint16_t bs_addr = *DSP_AAC_DEC_DM_BS_ADDR;
        *DSP_AAC_DEC_DM_BS_MCU_W_PTR = bs_addr;
    }
    /* fill bitstream */
    bt_write_bs_to_dsp(internal_handle);
    if (handle->directly_access_dsp_function == NULL) {//Share buffer flow
        if (!handle->waiting) {
            handle->waiting = true;
            if ((!handle->underflow) && (*DSP_AAC_DEC_ERROR_REPORT == DSP_AAC_REPORT_UNDERFLOW)) {
                handle->underflow = true;
                LISR_LOG_MSGID_I("[A2DP][AAC]DSP underflow \n", 0);
                handle->handler(handle, BT_CODEC_MEDIA_UNDERFLOW);
            } else {
                handle->handler(handle, BT_CODEC_MEDIA_REQUEST);
            }
        }
    }
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    else {
        if (*DSP_AAC_DEC_ERROR_REPORT == DSP_AAC_REPORT_UNDERFLOW) {
            if (internal_handle->aws_internal_flag) {
                LISR_LOG_MSGID_I("[A2DP][AAC]DSP underflow \n", 0);
                handle->handler(handle, BT_CODEC_MEDIA_UNDERFLOW);
            }
        }
    }
  #endif
}
#endif

static bt_codec_media_status_t bt_open_sink_aac_codec(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    mcu2dsp_start_param_t start_param;
    void *p_param_share;

    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, aac_decoder_isr_handler, internal_handle);

    // Collect parameters
    start_param.param.stream_in     = STREAM_IN_A2DP;
    start_param.param.stream_out    = STREAM_OUT_AFE;
    #ifdef AVM_SHAEE_BUF_INFO
    start_param.stream_in_param.a2dp.start_asi = internal_handle->start_asi;
    start_param.stream_in_param.a2dp.start_bt_clk = internal_handle->start_bt_clk;
    start_param.stream_in_param.a2dp.start_bt_intra_clk = internal_handle->start_bt_intra_clk;
    #else
    start_param.stream_in_param.a2dp.start_time_stamp = internal_handle->time_stamp;
    start_param.stream_in_param.a2dp.time_stamp_ratio = internal_handle->ts_ratio;
    #endif
    start_param.stream_in_param.a2dp.content_protection_exist = internal_handle->is_cp;
    start_param.stream_in_param.a2dp.alc_enable = internal_handle->is_alc_en;
    start_param.stream_in_param.a2dp.latency_monitor_enable = internal_handle->is_lm_en;
    start_param.stream_out_param.afe.aws_flag   =  internal_handle->is_aws;

    if(internal_handle->is_lm_en == true){
        hal_dvfs_lock_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_DVFS_LOCK);
    }

    p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_START, 0, (uint32_t)p_param_share, true);

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
    ami_execute_vendor_se(EVENT_AFTER_A2DP_START);
#endif
#endif

#else
    uint16_t I = 0;
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_MSGID_CTRL("[AAC]open_codec\r\n", 0);
    internal_handle->error_count = 0;

    audio_service_hook_isr(DSP_D2C_AAC_DEC_INT, aac_decoder_isr_handler, internal_handle);
    audio_service_setflag(handle->audio_id);
    *DSP_AUDIO_ASP_COMMON_FLAG_1 = 0;    //clear dsp audio common flag

    *DSP_AAC_DEC_ALLERROR_REPORT = 0;
    *DSP_AAC_DEC_DUAL_SCE = 0;

    if (*DSP_AAC_DEC_FSM != DSP_AAC_STATE_IDLE) {
        TASK_LOG_MSGID_E("[A2DP]AAC OPEN STATE ERROR(%x)\n",1, *DSP_AAC_DEC_FSM);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
  #if defined(AWS_DEBUG_CODE)
    TASK_LOG_MSGID_I("[AWS] set AWS flag\r\n", 0);
    bt_codec_a2dp_aws_set_flag(handle, true);
  #endif  /* defined(AWS_DEBUG_CODE) */
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_flag) {
        bt_codec_a2dp_aws_play_setting(internal_handle);
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    internal_handle->frame_count = 0;
    *DSP_AAC_DEC_FSM = DSP_AAC_STATE_START;

    *DSP_AUDIO_CTRL2 |= DSP_AAC_CTRL_ASP;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);
    afe_set_path_type(HAL_AUDIO_PLAYBACK_MUSIC);
    dsp_audio_fw_dynamic_download(DDID_AAC);

    aac_get_silence_pattern(internal_handle->sample_rate, internal_handle->channel_number, internal_handle->aac_silence_pattern);
    audio_playback_on(ASP_TYPE_AAC_DEC, internal_handle->sample_rate);

  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_internal_flag == false)
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    {
        for (I = 0; ; I++) {
            if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_PLAYING) {
                break;
            }
            /* This is the case when AAC codec has started and encoutered an error,
               aacPlaybackHisr found this and set the state to STOP, then AAC codec
               set the state to IDLE. */
            if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) {
                TASK_LOG_MSGID_E("[A2DP]ERROR when AAC CODEC STARTS \n", 0);
                break;
            }
            if (I > 80) {
                TASK_LOG_MSGID_E("[A2DP][AAC] CODEC OPEN ERROR\n", 0);
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
  #if defined(AWS_DEBUG_CODE)
    else {  /* Manually reset DSP for unit test */
        bt_codec_a2dp_aws_set_initial_sync(handle);
        for (I = 0; ; I++) {
            if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_PLAYING) {
                break;
            }
            /* This is the case when AAC codec has started and encoutered an error,
               aacPlaybackHisr found this and set the state to STOP, then AAC codec
               set the state to IDLE. */
            if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) {
                TASK_LOG_MSGID_E("[A2DP]ERROR when AAC CODEC STARTS \n", 0);
                break;
            }
            if (I > 80) {
                TASK_LOG_MSGID_E("[A2DP][AAC] CODEC OPEN ERROR\n", 0);
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
  #endif  /* defined(AWS_DEBUG_CODE) */
#endif

    handle->state = BT_CODEC_STATE_PLAY;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_close_sink_aac_codec(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
    ami_execute_vendor_se(EVENT_BEFORE_A2DP_STOP);
#endif
#endif
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_STOP, 0, 0, true);
    hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
    if(internal_handle->is_lm_en == true){
        hal_dvfs_lock_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_DVFS_UNLOCK);
    }
#else
    uint16_t I = 0;
    TASK_LOG_MSGID_CTRL("[AAC]close_codec\r\n", 0);
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag && !internal_handle->aws_flag && !internal_handle->aws_init_sync_flag) {
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_IDLE;
        }
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    for (I = 0; ; I++) {
        if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_IDLE) {
            break;
        }
        if (*DSP_AAC_DEC_FSM == DSP_AAC_STATE_PLAYING) {
            *DSP_AAC_DEC_FSM = DSP_AAC_STATE_STOP;
        }
        if (I > 80) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        hal_gpt_delay_ms(9);
    }

    *DSP_AUDIO_CTRL2 &= ~(DSP_AAC_CTRL_ASP | DSP_PCM_R_DIS);
    *DSP_AUDIO_FLEXI_CTRL &= ~(FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    audio_playback_off();
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag) {
            bt_codec_a2dp_aws_stop_setting(internal_handle);
        }
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
  #if defined(AWS_DEBUG_CODE)
    TASK_LOG_MSGID_I("[AWS] clear AWS flag\r\n", 0);
    bt_codec_a2dp_aws_set_flag(handle, false);
  #endif  /* defined(AWS_DEBUG_CODE) */
    audio_service_unhook_isr(DSP_D2C_AAC_DEC_INT);
    audio_service_clearflag(handle->audio_id);
#endif

    handle->state = BT_CODEC_STATE_STOP;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_aac_play(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_audio_mutex_lock(g_xSemaphore_Audio);
#endif
    bt_codec_media_status_t ret;
    TASK_LOG_MSGID_CTRL("[AAC]play\r\n", 0);
    if (handle->state != BT_CODEC_STATE_READY && handle->state != BT_CODEC_STATE_STOP) {
        TASK_LOG_MSGID_E("[A2DP][AAC] CODEC PLAY ERROR \n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    ret = bt_open_sink_aac_codec(handle);
#if defined(MTK_AVM_DIRECT)
    bt_audio_mutex_unlock(g_xSemaphore_Audio);
#endif
    return ret;
}

static bt_codec_media_status_t bt_a2dp_sink_aac_stop(bt_media_handle_t *handle)
{
    bt_codec_media_status_t result;
    TASK_LOG_MSGID_CTRL("[AAC]stop--state: %d\r\n",1, handle->state);
    if (handle->state == BT_CODEC_STATE_READY) {
        handle->state = BT_CODEC_STATE_STOP;
        result = BT_CODEC_MEDIA_STATUS_OK;
    } else if (handle->state != BT_CODEC_STATE_PLAY && handle->state != BT_CODEC_STATE_ERROR) {
        TASK_LOG_MSGID_E("[A2DP][AAC] CODEC STOP ERROR \n", 0);
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    } else {
        result = bt_close_sink_aac_codec(handle);
    }
    return result;
}

static bt_codec_media_status_t bt_a2dp_sink_aac_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

static uint32_t bt_a2dp_sink_aac_get_ts_ratio(bt_media_handle_t *handle, uint32_t ts0, uint32_t ts1)
{
    uint32_t ts_ratio = ((ts1 - ts0) >> 10);
    uint32_t ts2;
    if(1) {
        ts_ratio = 1024;
    }
    TASK_LOG_MSGID_I("[A2DP Codec][AAC]ts1:%d, ts0:%d, ts_ratio:%d",3, ts1, ts0, ts_ratio);
#if defined(MTK_AVM_DIRECT)
    if((ts_ratio != 1024) && (bt_get_next_pkt_info(ts1, &ts2) == TRUE))
    {
        uint32_t next_ratio = ((ts2 - ts1) >> 10);
        if( ((ts2 - ts1) & 0x3FF) || (ts2 == ts1) || (next_ratio > 0xF) || (next_ratio != ts_ratio)) {
            ts_ratio = 1024;
            TASK_LOG_MSGID_I("[A2DP Codec][AAC]ts2:%d next_ts_ratio:%d ret_ts_ratio:%d",3, ts2, next_ratio, ts_ratio);
        }
    }
#endif
    return ts_ratio;
}

static bt_codec_media_status_t bt_a2dp_sink_parse_aac_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    uint8_t channel_mode;
    uint16_t sample_rate;
    bt_codec_a2dp_audio_t *pParam = (bt_codec_a2dp_audio_t *)&internal_handle->codec_info;

    channel_mode = pParam->codec_cap.codec.aac.channels;
    sample_rate  = pParam->codec_cap.codec.aac.sample_rate;
    TASK_LOG_MSGID_I("[A2DP][AAC] sample rate=%x, channel=%x \n",2, sample_rate, channel_mode);

    switch (channel_mode) {
        case 0x2:
            internal_handle->channel_number = 1;
            break;
        case 0x1:
            internal_handle->channel_number = 2;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_channel_number((internal_handle->channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO);

    switch (sample_rate) {
        case 0x800:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_8KHZ;
            break;
        case 0x400:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_11_025KHZ;
            break;
        case 0x200:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_12KHZ;
            break;
        case 0x100:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_16KHZ;
            break;
        case 0x80:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_22_05KHZ;
            break;
        case 0x40:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_24KHZ;
            break;
        case 0x20:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_32KHZ;
            break;
        case 0x10:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
            break;
        case 0x8:
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_sampling_rate((hal_audio_sampling_rate_t)internal_handle->sample_rate);

    return BT_CODEC_MEDIA_STATUS_OK;
}
#endif /*MTK_BT_A2DP_AAC_ENABLE*/

#if defined(MTK_AVM_DIRECT)
#if defined(MTK_BT_A2DP_VENDOR_ENABLE)
static void vendor_decoder_isr_handler(hal_audio_event_t event, void *data)
{
    bt_media_handle_t *handle = (bt_media_handle_t *)data;

    if (event == HAL_AUDIO_EVENT_TIME_REPORT) {
        handle->handler(handle, BT_CODEC_MEDIA_TIME_REPORT);
    } else if(event == HAL_AUDIO_EVENT_LTCS_REPORT) {
        handle->handler(handle, BT_CODEC_MEDIA_LTCS_DATA_REPORT);
    } else if(event == HAL_AUDIO_EVENT_LTCS_TIMEOUT) {
        handle->handler(handle, BT_CODEC_MEDIA_LTCS_DATA_TIMEOUT);
    } else if(event == HAL_AUDIO_EVENT_DL_REINIT_REQUEST) {
        handle->handler(handle, BT_CODEC_MEDIA_AUDIO_DL_REINIT_REQUEST);
    } else if(event == HAL_AUDIO_EVENT_ALC_REQUEST) {
        handle->handler(handle, BT_CODEC_MEDIA_AUDIO_DL_ALC_REQUEST);
    } else {
        // KH: Assume that all events are error
        handle->state = BT_CODEC_STATE_ERROR;
        handle->handler(handle, BT_CODEC_MEDIA_ERROR);
    }
}


static bt_codec_media_status_t bt_open_sink_vendor_codec(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    mcu2dsp_start_param_t start_param;
    void *p_param_share;

    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, vendor_decoder_isr_handler, internal_handle);

    // Collect parameters
    start_param.param.stream_in     = STREAM_IN_A2DP;
    start_param.param.stream_out    = STREAM_OUT_AFE;
	#ifdef AVM_SHAEE_BUF_INFO
    start_param.stream_in_param.a2dp.start_asi = internal_handle->start_asi;
    start_param.stream_in_param.a2dp.start_bt_clk = internal_handle->start_bt_clk;
    start_param.stream_in_param.a2dp.start_bt_intra_clk = internal_handle->start_bt_intra_clk;
    #else
    start_param.stream_in_param.a2dp.start_time_stamp = internal_handle->time_stamp;
    start_param.stream_in_param.a2dp.time_stamp_ratio = internal_handle->ts_ratio;
	#endif
    start_param.stream_in_param.a2dp.content_protection_exist = internal_handle->is_cp;
    start_param.stream_in_param.a2dp.alc_enable = internal_handle->is_alc_en;
    start_param.stream_in_param.a2dp.latency_monitor_enable = internal_handle->is_lm_en;
    start_param.stream_out_param.afe.aws_flag   =  internal_handle->is_aws;

    if(internal_handle->is_lm_en == true){
        hal_dvfs_lock_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_DVFS_LOCK);
    }

    p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_START, 0, (uint32_t)p_param_share, true);

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
    ami_execute_vendor_se(EVENT_AFTER_A2DP_START);
#endif
#endif

    handle->state = BT_CODEC_STATE_PLAY;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_close_sink_vendor_codec(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
#ifdef MTK_VENDOR_SOUND_EFFECT_EXTENSION_ENABLE
    ami_execute_vendor_se(EVENT_BEFORE_A2DP_STOP);
#endif
#endif
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_STOP, 0, 0, true);
    hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
    if(internal_handle->is_lm_en == true){
        hal_dvfs_lock_control(HAL_AUDIO_DVFS_MAX_SPEED, HAL_DVFS_UNLOCK);
    }

    handle->state = BT_CODEC_STATE_STOP;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_vendor_play(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_audio_mutex_lock(g_xSemaphore_Audio);
#endif
    bt_codec_media_status_t ret;
    TASK_LOG_MSGID_CTRL("[VENDOR]play\r\n", 0);
    if (handle->state != BT_CODEC_STATE_READY && handle->state != BT_CODEC_STATE_STOP) {
        TASK_LOG_MSGID_E("[A2DP][VENDOR] CODEC PLAY ERROR \n", 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    ret = bt_open_sink_vendor_codec(handle);
#if defined(MTK_AVM_DIRECT)
    bt_audio_mutex_unlock(g_xSemaphore_Audio);
#endif
    return ret;
}

static bt_codec_media_status_t bt_a2dp_sink_vendor_stop(bt_media_handle_t *handle)
{
    bt_codec_media_status_t result;
    TASK_LOG_MSGID_CTRL("[VENDOR]stop--state: %d\r\n",1, handle->state);
    if (handle->state == BT_CODEC_STATE_READY) {
        handle->state = BT_CODEC_STATE_STOP;
        result = BT_CODEC_MEDIA_STATUS_OK;
    } else if (handle->state != BT_CODEC_STATE_PLAY && handle->state != BT_CODEC_STATE_ERROR) {
        TASK_LOG_MSGID_E("[A2DP][VENDOR] CODEC STOP ERROR \n", 0);
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    } else {
        result = bt_close_sink_vendor_codec(handle);
    }
    return result;
}

static bt_codec_media_status_t bt_a2dp_sink_vendor_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

static uint32_t bt_a2dp_sink_vendor_get_ts_ratio(bt_media_handle_t *handle, uint32_t ts0, uint32_t ts1)
{
    #if 0 /*aac ts_ratio*/
    if( ((ts1 - ts0) & 0x3FF) || (ts1 == ts0) )
    {
        return 1024;
    }
    return ((ts1 - ts0) >> 10);
    #else
    return 1;
    #endif
}

static bt_codec_media_status_t bt_a2dp_sink_parse_vendor_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    uint8_t channel_mode, sample_rate;
    bt_codec_a2dp_audio_t *pParam = (bt_codec_a2dp_audio_t *)&internal_handle->codec_info;

    channel_mode = pParam->codec_cap.codec.vendor.channels;
    sample_rate  = pParam->codec_cap.codec.vendor.sample_rate;
    TASK_LOG_MSGID_I("[A2DP][VENDOR]sample rate=%d, channel=%d \n",2, sample_rate, channel_mode);

    switch (channel_mode) {
        case 0x04:    /*MONO*/
            internal_handle->channel_number = 1;
            break;
        case 0x02:    /*DUAL CHANNEL*/
            internal_handle->channel_number = 2;
            break;
        case 0x01:    /*STEREO*/
            internal_handle->channel_number = 2;
            break;
        default:
            return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
    }
    hal_audio_set_stream_out_channel_number((internal_handle->channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO);
    if (pParam->codec_cap.codec.vendor.codec_id != 0x4c33)
    {
        switch (sample_rate) {
            case 0x20:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
                break;
            case 0x10:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
                break;
            case 0x08:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_88_2KHZ;
                break;
            case 0x04:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_96KHZ;
                break;
            case 0x02:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_176_4KHZ;
                break;
            case 0x01:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_192KHZ;
                break;
            default:
                return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
        }
    }
    else
    {
        switch (sample_rate) {
            case 0x01:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_96KHZ;
                break;
            case 0x02:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_88_2KHZ;
                break;
            case 0x04:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
                break;
            case 0x08:
                internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_44_1KHZ;
                break;
            default:
                return BT_CODEC_MEDIA_STATUS_INVALID_PARAM;
        }
    }
    hal_audio_set_stream_out_sampling_rate((hal_audio_sampling_rate_t)internal_handle->sample_rate);

    return BT_CODEC_MEDIA_STATUS_OK;
}
#endif /*MTK_BT_A2DP_VENDOR_ENABLE*/
#endif /*MTK_AVM_DIRECT*/


#ifdef MTK_BT_A2DP_SOURCE_SUPPORT

// a2dp source related API
static uint32_t bt_codec_a2dp_source_query_memory_size(bt_media_handle_t *handle)
{
    return sbc_codec_get_buffer_size();
}

static uint32_t bt_codec_a2dp_source_get_payload(bt_media_handle_t *handle, uint8_t *buffer, uint32_t buffer_size, uint32_t *sample_count)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t        *encoder_handle  = internal_handle->sbc_encode_handle;
    if (encoder_handle == NULL) {
        TASK_LOG_MSGID_E("[A2DP source] NULL handle;", 0);
        return 0;
    }
    return encoder_handle->get_payload(encoder_handle, buffer, buffer_size, sample_count);
}

static void bt_codec_a2dp_source_get_payload_done(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t        *encoder_handle  = internal_handle->sbc_encode_handle;
    if (encoder_handle == NULL) {
        TASK_LOG_MSGID_E("[A2DP source] NULL handle;", 0);
    } else {
        encoder_handle->get_payload_done(internal_handle->sbc_encode_handle);
    }
}

static void bt_codec_a2dp_source_query_payload_size(bt_media_handle_t *handle, uint32_t *minimum, uint32_t *total)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t        *encoder_handle  = internal_handle->sbc_encode_handle;
    if (encoder_handle == NULL) {
        TASK_LOG_MSGID_E("[A2DP source] NULL handle;", 0);
    } else {
        encoder_handle->query_payload_size(encoder_handle, minimum, total);
    }
}

void bt_codec_a2dp_source_event_callback(void *hdl, sbc_codec_event_t event)
{
    bt_media_handle_t *handle = (bt_media_handle_t *)hdl;

    switch (event) {
        case SBC_CODEC_MEDIA_GET_PAYLOAD: {
            handle->handler(handle, BT_CODEC_MEDIA_GET_PAYLOAD);
        }
        break;
    }
}

static uint32_t bt_codec_a2dp_source_set_bit_rate(bt_media_handle_t *handle, uint32_t bit_rate)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t *encoder_handle = NULL;
    if (internal_handle->sbc_encode_handle == NULL) {
        encoder_handle = sbc_codec_open(
                             bt_codec_a2dp_source_event_callback, &(internal_handle->initial_parameter), handle);
        internal_handle->sbc_encode_handle = encoder_handle;
    } else {
        encoder_handle = internal_handle->sbc_encode_handle;
    }

    return encoder_handle->set_bit_rate(encoder_handle, bit_rate);
}

static bt_codec_media_status_t bt_open_source_sbc_codec(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t *encoder_handle = NULL;
    if (internal_handle->sbc_encode_handle == NULL) {
        encoder_handle = sbc_codec_open(
                             bt_codec_a2dp_source_event_callback, &(internal_handle->initial_parameter), handle);
        internal_handle->sbc_encode_handle = encoder_handle;
    } else {
        encoder_handle = internal_handle->sbc_encode_handle;
    }

    if (encoder_handle == NULL) {
        TASK_LOG_MSGID_E("[A2DP source][SBC] sbc encoder init fail.", 0);
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (handle->buffer_info.buffer_base == NULL) {
        TASK_LOG_MSGID_E("[A2DP source][SBC] set buffer before play.", 0);
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    encoder_handle->set_buffer(encoder_handle, handle->buffer_info.buffer_base, handle->buffer_info.buffer_size);
    encoder_handle->play(encoder_handle);

    handle->state = BT_CODEC_STATE_PLAY;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_close_source_sbc_codec(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    sbc_codec_media_handle_t *encoder_handle = internal_handle->sbc_encode_handle;

    if (encoder_handle == NULL) {
        TASK_LOG_MSGID_E("[SBC] NULL handle\r\n", 0);
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (encoder_handle->stop(encoder_handle) < 0) {
        TASK_LOG_MSGID_E("[A2DP source][SBC] sbc encoder stop fail.", 0);
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }

    if (sbc_codec_close(encoder_handle) < 0) {
        TASK_LOG_MSGID_E("[A2DP source][SBC] sbc encoder close fail.", 0);
        handle->state = BT_CODEC_STATE_ERROR;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    internal_handle->sbc_encode_handle = NULL;

    handle->state = BT_CODEC_STATE_STOP;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_source_sbc_play(bt_media_handle_t *handle)
{
    if (handle->state != BT_CODEC_STATE_READY && handle->state != BT_CODEC_STATE_STOP) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
    bt_a2dp_ptr = 0;
    memset(bt_a2dp_bitstream, 0, BT_A2DP_BS_LEN * sizeof(uint8_t));
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
    return bt_open_source_sbc_codec(handle);
}

static bt_codec_media_status_t bt_a2dp_source_sbc_stop(bt_media_handle_t *handle)
{
    bt_codec_media_status_t result;
    if (handle->state == BT_CODEC_STATE_READY) {
        handle->state = BT_CODEC_STATE_STOP;
        result = BT_CODEC_MEDIA_STATUS_OK;
    } else if (handle->state != BT_CODEC_STATE_PLAY && handle->state != BT_CODEC_STATE_ERROR) {
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    } else {
        result = bt_close_source_sbc_codec(handle);
    }
    return result;
}

static bt_codec_media_status_t bt_a2dp_source_sbc_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_source_parse_sbc_info(bt_a2dp_audio_internal_handle_t *internal_handle)
{
    sbc_codec_initial_parameter_t *pDst = (sbc_codec_initial_parameter_t *)&internal_handle->initial_parameter;

    pDst->role = SBC_CODEC_MEDIA_ENCODER;
    pDst->cap.alloc_method = internal_handle->codec_info.codec_cap.codec.sbc.alloc_method;
    pDst->cap.block_length = internal_handle->codec_info.codec_cap.codec.sbc.block_length;
    pDst->cap.channel_mode = internal_handle->codec_info.codec_cap.codec.sbc.channel_mode;
    pDst->cap.max_bit_pool = internal_handle->codec_info.codec_cap.codec.sbc.max_bit_pool;
    pDst->cap.min_bit_pool = internal_handle->codec_info.codec_cap.codec.sbc.min_bit_pool;
    pDst->cap.sample_rate  = internal_handle->codec_info.codec_cap.codec.sbc.sample_rate;
    pDst->cap.subband_num  = internal_handle->codec_info.codec_cap.codec.sbc.subband_num;

    return BT_CODEC_MEDIA_STATUS_OK;
}

#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/
#if 0
// temp before app rate change function ready
uint32_t sampling_rate_enum_to_value(hal_audio_sampling_rate_t hal_audio_sampling_rate_enum)
{
    switch (hal_audio_sampling_rate_enum) {
        case HAL_AUDIO_SAMPLING_RATE_8KHZ:
            return   8000;
        case HAL_AUDIO_SAMPLING_RATE_11_025KHZ:
            return  11025;
        case HAL_AUDIO_SAMPLING_RATE_12KHZ:
            return  12000;
        case HAL_AUDIO_SAMPLING_RATE_16KHZ:
            return  16000;
        case HAL_AUDIO_SAMPLING_RATE_22_05KHZ:
            return  22050;
        case HAL_AUDIO_SAMPLING_RATE_24KHZ:
            return  24000;
        case HAL_AUDIO_SAMPLING_RATE_32KHZ:
            return  32000;
        case HAL_AUDIO_SAMPLING_RATE_44_1KHZ:
            return  44100;
        case HAL_AUDIO_SAMPLING_RATE_48KHZ:
            return  48000;
        case HAL_AUDIO_SAMPLING_RATE_88_2KHZ:
            return  88200;
        case HAL_AUDIO_SAMPLING_RATE_96KHZ:
            return  96000;
        case HAL_AUDIO_SAMPLING_RATE_176_4KHZ:
            return 176400;
        case HAL_AUDIO_SAMPLING_RATE_192KHZ:
            return 192000;

        default:
            return 8000;
    }
}
#endif

#ifdef AVM_SHAEE_BUF_INFO
enum SBC_CHANNEL_e
{
	SBC_MONO_CHANNEL            = 8,
	SBC_DUAL_CHANNEL            = 4,
	SBC_STEREO_CHANNEL          = 2,
	SBC_JOINT_STEREO_CHANNEL    = 1,
};


uint32_t sbc_frame_size_calc( uint8_t bitpool, uint8_t nrof_subbands, uint8_t nrof_blocks, uint8_t channel_mode )
{
    U32 framesize;
    U16 channels;

    nrof_subbands = (nrof_subbands == 1) ? 8 :4;
    switch(nrof_blocks)
    {
        case 8:
            nrof_blocks = 4;
            break;
        case 4:
            nrof_blocks = 8;
            break;
        case 2:
            nrof_blocks = 12;
            break;
        default:
            nrof_blocks = 16;
            break;
    }

    channels    = (channel_mode == SBC_MONO_CHANNEL) ? (U16)1:(U16)2;

    switch(channel_mode)
    {
        case SBC_MONO_CHANNEL:
        case SBC_DUAL_CHANNEL:
            framesize = ((nrof_blocks * channels * bitpool) + 7)/8 + 4 + (4 * nrof_subbands * channels)/8;
            break;

        case SBC_STEREO_CHANNEL:
            framesize = ((nrof_blocks * bitpool) + 7)/8 + 4 + (4 * nrof_subbands * channels)/8;
            break;
        default:
            framesize  = ((nrof_subbands + (nrof_blocks * bitpool)) + 7)/8 + 4 + (4 * nrof_subbands * channels)/8;
            break;
    }
    return framesize;

}
#define AAC_FRAME_DECODE_SAMPLE_NUM 1024

#define SBC_FRAME_DECODE_SAMPLE_NUM 128

#define AAC_MAX_ENCODE_FRAME_SIZE  (1024)

#define VEND_MAX_ENCODE_FRAME_SIZE (330)
void bt_AM_set_a2dp_AVM_info(avm_share_buf_info_t *p_info, uint32_t samplerate,bt_codec_capability_t* codec_cap)
{
    uint8_t nrof_subbands,nrof_blocks;
    p_info->codec_type = (uint16_t)codec_cap->type;
    p_info->SampleRate = samplerate;
    switch ( p_info->codec_type)
    {
        case BT_A2DP_CODEC_SBC :
            nrof_subbands = (codec_cap->codec.sbc.subband_num == 1) ? 8 :4;
            switch(codec_cap->codec.sbc.block_length)
            {
                case 8:
                    nrof_blocks = 4;
                    break;
                case 4:
                    nrof_blocks = 8;
                    break;
                case 2:
                    nrof_blocks = 12;
                    break;
                default:
                    nrof_blocks = 16;
                    break;
            }
            p_info->FrameSampleNum = nrof_blocks*nrof_subbands;
            break;
        case BT_A2DP_CODEC_AAC :
            p_info->FrameSampleNum = AAC_FRAME_DECODE_SAMPLE_NUM;
            break;
        case BT_A2DP_CODEC_VENDOR :
            p_info->FrameSampleNum = 128 * (samplerate / 44100);
            break;

    }
}
#endif

#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
static bt_codec_media_status_t bt_open_sink_airo_celt_codec(bt_media_handle_t *handle)
{
#ifdef AB1568
    pmu_ignore_vcore_mode(PMU_OFF);
#endif
#if defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    mcu2dsp_start_param_t start_param;
    void *p_param_share;

    hal_audio_service_hook_callback(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, sbc_decoder_isr_handler, internal_handle);

    // Collect parameters
    start_param.param.stream_in     = STREAM_IN_A2DP;
    start_param.param.stream_out    = STREAM_OUT_AFE;

    start_param.stream_in_param.a2dp.content_protection_exist = internal_handle->is_cp;
    #ifdef AVM_SHAEE_BUF_INFO
    start_param.stream_in_param.a2dp.start_asi = internal_handle->start_asi;
    start_param.stream_in_param.a2dp.start_bt_clk = internal_handle->start_bt_clk;
    start_param.stream_in_param.a2dp.start_bt_intra_clk = internal_handle->start_bt_intra_clk;
    #else
    start_param.stream_in_param.a2dp.start_time_stamp = internal_handle->time_stamp;
    start_param.stream_in_param.a2dp.time_stamp_ratio = internal_handle->ts_ratio;
    #endif
    start_param.stream_in_param.a2dp.alc_enable = internal_handle->is_alc_en;
    start_param.stream_in_param.a2dp.latency_monitor_enable = internal_handle->is_lm_en;
    start_param.stream_out_param.afe.aws_flag   =  internal_handle->is_aws;

    p_param_share = hal_audio_dsp_controller_put_paramter( &start_param, sizeof(mcu2dsp_start_param_t), AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_START, 0, (uint32_t)p_param_share, true);


#else
    uint16_t I = 0;
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_MSGID_CTRL("[AIRO_CELT]open_codec\r\n", 0);
    audio_service_hook_isr(DSP_D2C_SBC_DEC_INT, sbc_decoder_isr_handler, internal_handle);
    audio_service_setflag(handle->audio_id);
    if (GET_DSP_VALUE(RG_SBC_DEC_FSM) != DSP_SBC_STATE_IDLE) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
  #if defined(AWS_DEBUG_CODE)
    TASK_LOG_MSGID_I("[AWS] set AWS flag\r\n", 0);
    bt_codec_a2dp_aws_set_flag(handle, true);
  #endif  /* defined(AWS_DEBUG_CODE) */
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_flag) {
        bt_codec_a2dp_aws_play_setting(internal_handle);
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    internal_handle->frame_count = 0;
    *DSP_AUDIO_FLEXI_CTRL |= (FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    *DSP_SBC_PAR_MAGIC_WORD = 0x3453;
    SET_DSP_VALUE(RG_SBC_PARSER_EN, 1);
    SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_START);
    *DSP_SBC_DEC_DM_BS_MCU_W_PTR = 0;
    dsp_audio_fw_dynamic_download(DDID_SBC_DECODE);
    afe_set_path_type(HAL_AUDIO_PLAYBACK_MUSIC);
    audio_playback_on(ASP_TYPE_SBC_DEC, internal_handle->sample_rate);
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    if (internal_handle->aws_internal_flag == false)
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    {
        for (I = 0; ; I++) {
            if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
                break;
            }
            if (I > 80) {
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
  #if defined(AWS_DEBUG_CODE)
    else {  /* Manually reset DSP for unit test */
        bt_codec_a2dp_aws_set_initial_sync(handle);
        for (I = 0; ; I++) {
            if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
                break;
            }
            if (I > 80) {
                return BT_CODEC_MEDIA_STATUS_ERROR;
            }
            hal_gpt_delay_ms(9);
        }
    }
  #endif  /* defined(AWS_DEBUG_CODE) */
#endif

    handle->state = BT_CODEC_STATE_PLAY;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_close_sink_airo_celt_codec(bt_media_handle_t *handle)
{
#ifdef AB1568
    pmu_ignore_vcore_mode(PMU_ON);
#endif
#if defined(MTK_AVM_DIRECT)
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_STOP, 0, 0, true);
    hal_audio_service_unhook_callback(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);
#else
    uint16_t I = 0;
    TASK_LOG_MSGID_CTRL("[AIRO_CELT]close_codec\r\n", 0);
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag && !internal_handle->aws_flag && !internal_handle->aws_init_sync_flag) {
            SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_IDLE);
        }
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    for (I = 0; ; I++) {
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_IDLE) {
            break;
        }
        if (GET_DSP_VALUE(RG_SBC_DEC_FSM) == DSP_SBC_STATE_RUNNING) {
            SET_DSP_VALUE(RG_SBC_DEC_FSM, DSP_SBC_STATE_FLUSH);
        }
        if (I > 80) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
        hal_gpt_delay_ms(9);
    }
    SET_DSP_VALUE(RG_SBC_PARSER_EN, 0);

    *DSP_AUDIO_CTRL2 &= ~DSP_PCM_R_DIS;
    *DSP_AUDIO_FLEXI_CTRL &= ~(FLEXI_VBI_ENABLE | FLEXI_SD_ENABLE);

    audio_playback_off();
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    {
        bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *)handle;
        if (internal_handle->aws_internal_flag) {
            bt_codec_a2dp_aws_stop_setting(internal_handle);
        }
    }
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
  #if defined(AWS_DEBUG_CODE)
    TASK_LOG_MSGID_I("[AWS] clear AWS flag\r\n", 0);
    bt_codec_a2dp_aws_set_flag(handle, false);
  #endif  /* defined(AWS_DEBUG_CODE) */
    audio_service_unhook_isr(DSP_D2C_SBC_DEC_INT);
    audio_service_clearflag(handle->audio_id);
#endif

    handle->state = BT_CODEC_STATE_STOP;
    return BT_CODEC_MEDIA_STATUS_OK;
}

static bt_codec_media_status_t bt_a2dp_sink_airo_celt_play(bt_media_handle_t *handle)
{
#if defined(MTK_AVM_DIRECT)
    bt_audio_mutex_lock(g_xSemaphore_Audio);
#endif
    bt_codec_media_status_t ret;
    TASK_LOG_MSGID_CTRL("[AIRO_CELT]play\r\n", 0);
    if (handle->state != BT_CODEC_STATE_READY && handle->state != BT_CODEC_STATE_STOP) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
#if defined(BT_A2DP_BITSTREAM_DUMP_DEBUG)
    bt_a2dp_ptr = 0;
    memset(bt_a2dp_bitstream, 0, BT_A2DP_BS_LEN * sizeof(uint8_t));
#endif /* BT_A2DP_BITSTREAM_DUMP_DEBUG */
    ret = bt_open_sink_airo_celt_codec(handle);
#if defined(MTK_AVM_DIRECT)
    bt_audio_mutex_unlock(g_xSemaphore_Audio);
#endif
    return ret;
}

static bt_codec_media_status_t bt_a2dp_sink_airo_celt_stop(bt_media_handle_t *handle)
{
    bt_codec_media_status_t result;
    TASK_LOG_MSGID_CTRL("[AIRO_CELT]stop--state: %d\r\n",1, handle->state);
    if (handle->state == BT_CODEC_STATE_READY) {
        handle->state = BT_CODEC_STATE_STOP;
        result = BT_CODEC_MEDIA_STATUS_OK;
    } else if (handle->state != BT_CODEC_STATE_PLAY && handle->state != BT_CODEC_STATE_ERROR) {
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    } else {
        result = bt_close_sink_airo_celt_codec(handle);
    }
    return result;
}

static bt_codec_media_status_t bt_a2dp_sink_airo_celt_process(bt_media_handle_t *handle, bt_codec_media_event_t event)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;
    if (internal_handle == NULL) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
}
#endif //(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)


bt_media_handle_t *bt_codec_a2dp_open(bt_codec_a2dp_callback_t bt_a2dp_callback , const bt_codec_a2dp_audio_t *param)
{
    clock_mux_sel(CLK_AUD_BUS_SEL, 1);
    bt_media_handle_t *handle;
    bt_a2dp_audio_internal_handle_t *internal_handle = NULL; /*internal handler*/

    TASK_LOG_MSGID_I("[A2DP]Open codec\n", 0);

#if !defined(MTK_AVM_DIRECT)
    uint16_t audio_id = audio_get_id();

    if (audio_id > MAX_AUDIO_FUNCTIONS) {
        return 0;
    }
#endif


    TASK_LOG_MSGID_I("[A2DP]Open codec--role: %d, type: %d, 1: %d, 2: %d, 3: %d, 4: %d, 5: %d, 6: %d, 7: %d\n",9,
               param->role, param->codec_cap.type,
               param->codec_cap.codec.sbc.alloc_method,
               param->codec_cap.codec.sbc.block_length,
               param->codec_cap.codec.sbc.channel_mode,
               param->codec_cap.codec.sbc.max_bit_pool,
               param->codec_cap.codec.sbc.min_bit_pool,
               param->codec_cap.codec.sbc.sample_rate,
               param->codec_cap.codec.sbc.subband_num);

    /* alloc internal handle space */
    internal_handle = (bt_a2dp_audio_internal_handle_t *)pvPortMalloc(sizeof(bt_a2dp_audio_internal_handle_t));
    if (internal_handle == NULL) {
        return 0;
    }

    memset(internal_handle, 0, sizeof(bt_a2dp_audio_internal_handle_t));
    handle = &internal_handle->handle;
    internal_handle->codec_info = *(bt_codec_a2dp_audio_t *)param;
#if !defined(MTK_AVM_DIRECT)
    handle->audio_id = audio_id;
#endif
    handle->handler = bt_a2dp_callback;
    handle->directly_access_dsp_function = NULL;
    handle->get_data_count_function = NULL;
    handle->buffer_info.buffer_base = NULL;
    bt_codec_buffer_function_init(handle);
    if (internal_handle->codec_info.role == BT_A2DP_SINK) {
        if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) {
            bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
            handle->play    = bt_a2dp_sink_sbc_play;
            handle->stop    = bt_a2dp_sink_sbc_stop;
            handle->process = bt_a2dp_sink_sbc_process;
            handle->side_tone_enable = bt_side_tone_enable;
            handle->side_tone_disable = bt_side_tone_disable;
            result = bt_a2dp_sink_parse_sbc_info(internal_handle);
            if (BT_CODEC_MEDIA_STATUS_OK != result) {
                if (internal_handle != NULL) {
                    vPortFree(internal_handle);
                    internal_handle = NULL;
                }
                return 0;
            }

#if defined(MTK_AVM_DIRECT)
            {
                mcu2dsp_open_param_t open_param;
                memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
                void *p_param_share;
                uint32_t sink_latency = bt_sink_srv_ami_get_a2dp_sink_latency();

                open_param.param.stream_in  = STREAM_IN_A2DP;
                open_param.param.stream_out = STREAM_OUT_AFE;

                memcpy(&open_param.stream_in_param.a2dp.codec_info, param, sizeof(bt_codec_a2dp_audio_t));
                open_param.stream_in_param.a2dp.p_current_bit_rate = hal_audio_report_bitrate_buf();
                open_param.stream_in_param.a2dp.p_lostnum_report = hal_audio_report_lostnum_buf();
                open_param.stream_in_param.a2dp.p_audio_sync_info = hal_audio_query_audio_sync_info();
                open_param.stream_in_param.a2dp.sink_latency    = sink_latency;
                #ifndef AVM_SHAEE_BUF_INFO
                open_param.stream_in_param.a2dp.p_share_info    = hal_audio_query_bt_audio_dl_share_info();
                open_param.stream_in_param.a2dp.p_asi_buf       = hal_audio_query_ltcs_asi_buf();
                open_param.stream_in_param.a2dp.p_min_gap_buf   = hal_audio_query_ltcs_min_gap_buf();
                open_param.stream_in_param.a2dp.bt_inf_address  = bt_sink_srv_ami_get_bt_inf_address();
                open_param.stream_in_param.a2dp.clk_info_address   = hal_audio_query_rcdc_share_info();
                hal_audio_a2dp_reset_share_info( open_param.stream_in_param.a2dp.p_share_info );
                ((n9_dsp_share_info_t*)(open_param.stream_in_param.a2dp.p_share_info))->next = (U32)hal_audio_query_audio_sync_info();

                open_param.stream_in_param.a2dp.p_current_bit_rate = hal_audio_report_bitrate_buf();
                open_param.stream_in_param.a2dp.p_lostnum_report = hal_audio_report_lostnum_buf();
                open_param.stream_in_param.a2dp.p_audio_sync_info = hal_audio_query_audio_sync_info();

                #else
                open_param.stream_in_param.a2dp.p_afe_buf_report   = hal_audio_afe_dl_buf_report();
                open_param.stream_in_param.a2dp.p_pcdc_anchor_info_buf   = hal_audio_query_ltcs_anchor_info_buf();
                open_param.stream_in_param.a2dp.p_share_info    = (avm_share_buf_info_t*)hal_audio_query_bt_audio_dl_share_info();
                uint32_t block_size = sbc_frame_size_calc(param->codec_cap.codec.sbc.max_bit_pool,
                                                          param->codec_cap.codec.sbc.subband_num,
                                                          param->codec_cap.codec.sbc.block_length,
                                                          param->codec_cap.codec.sbc.channel_mode);

                bt_AM_set_a2dp_AVM_info(open_param.stream_in_param.a2dp.p_share_info,
                                        hal_audio_sampling_rate_enum_to_value(internal_handle->sample_rate),
                                        &internal_handle->codec_info.codec_cap);
                hal_audio_set_avm_info(open_param.stream_in_param.a2dp.p_share_info,
                                        SHARE_BUFFER_BT_AUDIO_DL_SIZE,
                                        block_size,
                                        sink_latency);

                TASK_LOG_MSGID_I("SBC frame size = %d sample num = %d",2,block_size,((avm_share_buf_info_t*)open_param.stream_in_param.a2dp.p_share_info)->FrameSampleNum );
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_UPDATE_BUF_INFO, NULL);
                #endif
#ifdef MTK_AUDIO_PLC_ENABLE
                open_param.stream_in_param.a2dp.audio_plc.enable= true;

#endif
#if 0
                open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL;
                open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
                open_param.stream_out_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
                open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
#else
                hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
                TASK_LOG_MSGID_I("[A2DP]Open codec SBC out_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.stream_channel, open_param.stream_out_param.afe.audio_interface);
#endif
                open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM1;
                open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
                open_param.stream_out_param.afe.stream_out_sampling_rate   = hal_audio_sampling_rate_enum_to_value(internal_handle->sample_rate);
                //open_param.stream_out_param.afe.stream_out_sampling_rate  = p_hdl_effect->get_output_samplingrate(internal_handle->sample_rate);

#ifdef MTK_LEAKAGE_COMPENSATION_ENABLE
                if (audio_anc_leakage_compensation_get_status()) {
                    open_param.stream_out_param.afe.sampling_rate = 16000;
                    TASK_LOG_MSGID_I("[RECORD_LC]set a2dp sr to 16000, lc status:%d", 1, audio_anc_leakage_compensation_get_status());
                } else {
                    open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(hal_audio_get_device_out_supported_frequency(open_param.stream_out_param.afe.audio_device,internal_handle->sample_rate));
                }
#else
                open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(hal_audio_get_device_out_supported_frequency(open_param.stream_out_param.afe.audio_device,internal_handle->sample_rate));
#endif
                //open_param.stream_out_param.afe.sampling_rate   = hal_audio_get_device_out_supported_frequency(HAL_AUDIO_DEVICE_HEADSET,open_param.stream_out_param.afe.stream_out_sampling_rate)
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
                open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
                open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#endif
#if defined (MTK_FIXED_VP_A2DP_SAMPLING_RATE_TO_48KHZ)
				open_param.stream_out_param.afe.sampling_rate	= 48000;
#endif
                open_param.stream_out_param.afe.irq_period      = 8;
                open_param.stream_out_param.afe.frame_size      = 1024;
                open_param.stream_out_param.afe.frame_number    = 4;
                open_param.stream_out_param.afe.hw_gain         = true;
#ifdef ENABLE_HWSRC_CLKSKEW
#ifdef MTK_BT_SPEAKER_ENABLE
                open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
#else
                open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V2;

#endif /*MTK_BT_SPEAKER_ENABLE*/
#endif /*ENABLE_HWSRC_CLKSKEW*/
                if (open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
                    if (open_param.stream_out_param.afe.sampling_rate > 48000) {
                        open_param.stream_out_param.afe.misc_parms      |= I2S_CLK_SOURCE_APLL;
                    }
                }
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
                ami_set_afe_param(STREAM_OUT, internal_handle->sample_rate, true);
#endif
                p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);

#if defined(HAL_AUDIO_SUPPORT_APLL)
                //Open i2s master out low jitter mode
                hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_BT_CODEC_A2DP_OUT, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif

                hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, open_param.stream_out_param.afe.audio_device, MAX(open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.stream_out_sampling_rate), TRUE);
                // Notify to do dynamic download. Use async wait.
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_OPEN, AUDIO_DSP_CODEC_TYPE_SBC, (uint32_t)p_param_share, false);
            }
#endif

#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
            TASK_LOG_MSGID_I("[A2DP][SBC]Codec open", 0);
#endif
        }
#ifdef MTK_BT_A2DP_AAC_ENABLE
        else if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_AAC) {
            bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
            handle->play    = bt_a2dp_sink_aac_play;
            handle->stop    = bt_a2dp_sink_aac_stop;
            handle->process = bt_a2dp_sink_aac_process;
            handle->get_ts_ratio = bt_a2dp_sink_aac_get_ts_ratio;
            handle->side_tone_enable = bt_side_tone_enable;
            handle->side_tone_disable = bt_side_tone_disable;
            result = bt_a2dp_sink_parse_aac_info(internal_handle);
            if (BT_CODEC_MEDIA_STATUS_OK != result) {
                if (internal_handle != NULL) {
                    vPortFree(internal_handle);
                    internal_handle = NULL;
                }
                return 0;
            }

  #if defined(MTK_AVM_DIRECT)
            {
                mcu2dsp_open_param_t open_param;
                memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
                void *p_param_share;
                uint32_t sink_latency = bt_sink_srv_ami_get_a2dp_sink_latency();

                open_param.param.stream_in  = STREAM_IN_A2DP;
                open_param.param.stream_out = STREAM_OUT_AFE;

                memcpy(&open_param.stream_in_param.a2dp.codec_info, param, sizeof(bt_codec_a2dp_audio_t));
                open_param.stream_in_param.a2dp.p_current_bit_rate = hal_audio_report_bitrate_buf();
                open_param.stream_in_param.a2dp.p_lostnum_report = hal_audio_report_lostnum_buf();
                open_param.stream_in_param.a2dp.p_audio_sync_info = hal_audio_query_audio_sync_info();
                open_param.stream_in_param.a2dp.sink_latency    = sink_latency;
                #ifndef AVM_SHAEE_BUF_INFO
                open_param.stream_in_param.a2dp.p_share_info = hal_audio_query_bt_audio_dl_share_info();
                hal_audio_a2dp_reset_share_info( open_param.stream_in_param.a2dp.p_share_info );
                ((n9_dsp_share_info_t*)(open_param.stream_in_param.a2dp.p_share_info))->next = (U32)hal_audio_query_audio_sync_info();
                open_param.stream_in_param.a2dp.p_asi_buf          = hal_audio_query_ltcs_asi_buf();
                open_param.stream_in_param.a2dp.p_min_gap_buf      = hal_audio_query_ltcs_min_gap_buf();
                open_param.stream_in_param.a2dp.bt_inf_address  = bt_sink_srv_ami_get_bt_inf_address();
                open_param.stream_in_param.a2dp.clk_info_address   = hal_audio_query_rcdc_share_info();



                #else
                open_param.stream_in_param.a2dp.p_afe_buf_report   = hal_audio_afe_dl_buf_report();
                open_param.stream_in_param.a2dp.p_pcdc_anchor_info_buf   = hal_audio_query_ltcs_anchor_info_buf();
                open_param.stream_in_param.a2dp.p_share_info = (avm_share_buf_info_t*)hal_audio_query_bt_audio_dl_share_info();
                uint32_t block_size = AAC_MAX_ENCODE_FRAME_SIZE;
                //warning "should be MTU"
                bt_AM_set_a2dp_AVM_info(open_param.stream_in_param.a2dp.p_share_info,
                                        hal_audio_sampling_rate_enum_to_value(internal_handle->sample_rate),
                                        &internal_handle->codec_info.codec_cap);
                hal_audio_set_avm_info(open_param.stream_in_param.a2dp.p_share_info,
                                        SHARE_BUFFER_BT_AUDIO_DL_SIZE,
                                        block_size,
                                        sink_latency);
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_UPDATE_BUF_INFO, NULL);
                #endif
#ifdef MTK_AUDIO_PLC_ENABLE
                open_param.stream_in_param.a2dp.audio_plc.enable= true;

#endif
#if 0
                open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL;
                open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
                open_param.stream_out_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
                open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
#else
                hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
                TASK_LOG_MSGID_I("[A2DP]Open codec AAC out_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.stream_channel, open_param.stream_out_param.afe.audio_interface);
#endif
                open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM1;
                open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
                open_param.stream_out_param.afe.stream_out_sampling_rate   = hal_audio_sampling_rate_enum_to_value(internal_handle->sample_rate);

                //open_param.stream_out_param.afe.stream_out_sampling_rate  = p_hdl_effect->get_output_samplingrate(internal_handle->sample_rate);

#ifdef MTK_LEAKAGE_COMPENSATION_ENABLE
                if (audio_anc_leakage_compensation_get_status()) {
                    open_param.stream_out_param.afe.sampling_rate = 16000;
                    TASK_LOG_MSGID_I("[RECORD_LC]set a2dp sr to 16000, lc status:%d", 1, audio_anc_leakage_compensation_get_status());
                } else {
                    open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(hal_audio_get_device_out_supported_frequency(open_param.stream_out_param.afe.audio_device,internal_handle->sample_rate));
                }
#else
                open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(hal_audio_get_device_out_supported_frequency(open_param.stream_out_param.afe.audio_device,internal_handle->sample_rate));
#endif
                //open_param.stream_out_param.afe.sampling_rate   = hal_audio_get_device_out_supported_frequency(HAL_AUDIO_DEVICE_HEADSET,open_param.stream_out_param.afe.stream_out_sampling_rate)
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
                open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
                open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#endif
                open_param.stream_out_param.afe.irq_period      = 8;
                open_param.stream_out_param.afe.frame_size      = 1024;
                open_param.stream_out_param.afe.frame_number    = 4;
                open_param.stream_out_param.afe.hw_gain         = true;
#ifdef ENABLE_HWSRC_CLKSKEW
#ifdef MTK_BT_SPEAKER_ENABLE
                open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
#else
                open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V2;

#endif /*MTK_BT_SPEAKER_ENABLE*/
#endif /*ENABLE_HWSRC_CLKSKEW*/
                if (open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
                    if (open_param.stream_out_param.afe.sampling_rate > 48000) {
                        open_param.stream_out_param.afe.misc_parms      |= I2S_CLK_SOURCE_APLL;
                    }
                }
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
                ami_set_afe_param(STREAM_OUT, internal_handle->sample_rate, true);
#endif
                p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);

#if defined(HAL_AUDIO_SUPPORT_APLL)
                //Open i2s master out low jitter mode
                hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_BT_CODEC_A2DP_OUT, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif

                hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, open_param.stream_out_param.afe.audio_device, MAX(open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.stream_out_sampling_rate), TRUE);
                // Notify to do dynamic download. Use async wait.
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_OPEN, AUDIO_DSP_CODEC_TYPE_AAC, (uint32_t)p_param_share, false);
            }

  #endif

#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
            TASK_LOG_MSGID_I("[A2DP[AAC]Codec open", 0);
#endif
        }
#endif /*MTK_BT_A2DP_AAC_ENABLE*/
#if defined(MTK_BT_A2DP_VENDOR_ENABLE)
        else if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_VENDOR) {
            bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
            handle->play    = bt_a2dp_sink_vendor_play;
            handle->stop    = bt_a2dp_sink_vendor_stop;
            handle->process = bt_a2dp_sink_vendor_process;
            handle->get_ts_ratio = bt_a2dp_sink_vendor_get_ts_ratio;
            handle->side_tone_enable = bt_side_tone_enable;
            handle->side_tone_disable = bt_side_tone_disable;
            result = bt_a2dp_sink_parse_vendor_info(internal_handle);
            if (BT_CODEC_MEDIA_STATUS_OK != result) {
                if (internal_handle != NULL) {
                    vPortFree(internal_handle);
                    internal_handle = NULL;
                }
                return 0;
            }
  #if defined(MTK_AVM_DIRECT)
            {
                mcu2dsp_open_param_t open_param;
                memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
                void *p_param_share;
                uint32_t sink_latency = bt_sink_srv_ami_get_a2dp_sink_latency();

                open_param.param.stream_in  = STREAM_IN_A2DP;
                open_param.param.stream_out = STREAM_OUT_AFE;

                memcpy(&open_param.stream_in_param.a2dp.codec_info, param, sizeof(bt_codec_a2dp_audio_t));

                #ifndef AVM_SHAEE_BUF_INFO
                open_param.stream_in_param.a2dp.p_share_info = hal_audio_query_bt_audio_dl_share_info();
                #ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
                hal_audio_reset_share_info( open_param.stream_in_param.a2dp.p_share_info );
                open_param.stream_in_param.a2dp.p_share_info->length = SHARE_BUFFER_BT_AUDIO_DL_SIZE;
                #else /*MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE*/
                hal_audio_a2dp_reset_share_info( open_param.stream_in_param.a2dp.p_share_info );
                #endif /*MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE*/
                ((n9_dsp_share_info_t*)(open_param.stream_in_param.a2dp.p_share_info))->next = (U32)hal_audio_query_audio_sync_info();
                open_param.stream_in_param.a2dp.p_asi_buf          = hal_audio_query_ltcs_asi_buf();
                open_param.stream_in_param.a2dp.p_min_gap_buf      = hal_audio_query_ltcs_min_gap_buf();
                open_param.stream_in_param.a2dp.p_current_bit_rate = hal_audio_report_bitrate_buf();
                open_param.stream_in_param.a2dp.p_lostnum_report   = hal_audio_report_lostnum_buf();
                open_param.stream_in_param.a2dp.p_audio_sync_info = hal_audio_query_audio_sync_info();
                #ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
                open_param.stream_in_param.a2dp.sink_latency    = 0;
                #else /*MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE*/
                open_param.stream_in_param.a2dp.sink_latency       = sink_latency;
                #endif /*MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE*/
                open_param.stream_in_param.a2dp.bt_inf_address     = bt_sink_srv_ami_get_bt_inf_address();
                open_param.stream_in_param.a2dp.clk_info_address   = hal_audio_query_rcdc_share_info();

                #else /*AVM_SHAEE_BUF_INFO*/
                open_param.stream_in_param.a2dp.p_afe_buf_report   = hal_audio_afe_dl_buf_report();
                open_param.stream_in_param.a2dp.p_share_info    = (avm_share_buf_info_t*)hal_audio_query_bt_audio_dl_share_info();
                #ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
                open_param.stream_in_param.a2dp.sink_latency    = 0;
                #else /*MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE*/
                open_param.stream_in_param.a2dp.sink_latency    = sink_latency;
                #endif
                open_param.stream_in_param.a2dp.p_current_bit_rate = hal_audio_report_bitrate_buf();
                open_param.stream_in_param.a2dp.p_pcdc_anchor_info_buf   = hal_audio_query_ltcs_anchor_info_buf();
#ifdef MTK_AUDIO_PLC_ENABLE
                open_param.stream_in_param.a2dp.audio_plc.enable= true;

#endif
                uint32_t block_size = VEND_MAX_ENCODE_FRAME_SIZE;
                bt_AM_set_a2dp_AVM_info(open_param.stream_in_param.a2dp.p_share_info,
                                        hal_audio_sampling_rate_enum_to_value(internal_handle->sample_rate),
                                        &internal_handle->codec_info.codec_cap);
                #ifndef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
                hal_audio_set_avm_info(open_param.stream_in_param.a2dp.p_share_info,
                                        SHARE_BUFFER_BT_AUDIO_DL_SIZE,
                                        block_size,
                                        sink_latency);
                #else
                hal_audio_set_avm_info(open_param.stream_in_param.a2dp.p_share_info,
                                        SHARE_BUFFER_VENDOR_CODEC_SIZE,
                                        block_size,
                                        0);
                #endif /*MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE*/
                g_prCurrent_player->notify_cb(g_prCurrent_player->aud_id, AUD_A2DP_PROC_IND, AUD_A2DP_UPDATE_BUF_INFO, NULL);
                #endif /*AVM_SHAEE_BUF_INFO*/
#if 0
                //[TEMP]: Add AT Cmd to switch I2S mode
                if(((*((volatile uint32_t*)(0xA2120B04)) >> 2)&0x01) == 1) {
                    // I2S Mode
                    open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_I2S_MASTER;
                    open_param.stream_out_param.afe.misc_parms      = I2S_CLK_SOURCE_APLL;
                }
                else {
                    open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL;
                }

                open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
                open_param.stream_out_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
                open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
#else
                hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);

                TASK_LOG_MSGID_I("[A2DP]Open codec VENDOR out_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.stream_channel, open_param.stream_out_param.afe.audio_interface);
#endif
                open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM1;
                open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
                open_param.stream_out_param.afe.stream_out_sampling_rate   = hal_audio_sampling_rate_enum_to_value(internal_handle->sample_rate);
                //open_param.stream_out_param.afe.stream_out_sampling_rate  = p_hdl_effect->get_output_samplingrate(internal_handle->sample_rate);
                #if MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE   /* ADVDEV16IS-8657 */
                uint32_t backup_sampling_rate = open_param.stream_out_param.afe.sampling_rate;
                #endif  /* ADVDEV16IS-8657 */
#ifdef MTK_LEAKAGE_COMPENSATION_ENABLE
                if (audio_anc_leakage_compensation_get_status()) {
                    open_param.stream_out_param.afe.sampling_rate = 16000;
                    TASK_LOG_MSGID_I("[RECORD_LC]set a2dp sr to 16000, lc status:%d", 1, audio_anc_leakage_compensation_get_status());
                } else {
                    open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(hal_audio_get_device_out_supported_frequency(open_param.stream_out_param.afe.audio_device,internal_handle->sample_rate));
                }
#else
                open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(hal_audio_get_device_out_supported_frequency(open_param.stream_out_param.afe.audio_device,internal_handle->sample_rate));
#endif
                //open_param.stream_out_param.afe.sampling_rate   = hal_audio_get_device_out_supported_frequency(HAL_AUDIO_DEVICE_HEADSET,open_param.stream_out_param.afe.stream_out_sampling_rate)
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
                open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
                open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#endif
                open_param.stream_out_param.afe.irq_period      = 8;
                open_param.stream_out_param.afe.hw_gain         = true;
                #ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
                #if 1   /* ADVDEV16IS-8657 */
                if (backup_sampling_rate > 96000){
                open_param.stream_out_param.afe.frame_size      = 1024;
                }
                else if (backup_sampling_rate > 48000){
                    open_param.stream_out_param.afe.frame_size      = 768;
                }
                else
                {
                    open_param.stream_out_param.afe.frame_size      = 384;
                }
                open_param.stream_out_param.afe.frame_number    = 4;
                #else
                // To keep 2.66ms * 12 data time inside PCM buffer
                if (open_param.stream_out_param.afe.sampling_rate > 96000){
                    open_param.stream_out_param.afe.frame_size      = 512;
                }
                else if (open_param.stream_out_param.afe.sampling_rate > 48000){
                    open_param.stream_out_param.afe.frame_size      = 256;
                }
                else
                {
                    open_param.stream_out_param.afe.frame_size      = 128;
                }
                open_param.stream_out_param.afe.frame_number    = 12;
                #endif
                #else
                open_param.stream_out_param.afe.frame_size      = 1024;
                open_param.stream_out_param.afe.frame_number    = 4;
                #endif
#ifdef ENABLE_HWSRC_CLKSKEW
  #ifdef MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE
                open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
  #else /*MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE*/
    #ifdef MTK_BT_SPEAKER_ENABLE
                open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
    #else /*MTK_BT_SPEAKER_ENABLE*/
                open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V2;
    #endif /*MTK_BT_SPEAKER_ENABLE*/
  #endif /*MTK_BT_A2DP_VENDOR_CODEC_BC_ENABLE*/
#endif /*ENABLE_HWSRC_CLKSKEW*/
                if (open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
                    if (open_param.stream_out_param.afe.sampling_rate > 48000) {
                        open_param.stream_out_param.afe.misc_parms      |= I2S_CLK_SOURCE_APLL;
                    }
                }
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
                ami_set_afe_param(STREAM_OUT, internal_handle->sample_rate, true);
#endif

                p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);

                // Notify to do dynamic download. Use async wait.

#if defined(HAL_AUDIO_SUPPORT_APLL)
                //Open i2s master out low jitter mode
                hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_BT_CODEC_A2DP_OUT, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif

                hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, open_param.stream_out_param.afe.audio_device, MAX(open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.stream_out_sampling_rate), TRUE);
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_OPEN, AUDIO_DSP_CODEC_TYPE_VENDOR, (uint32_t)p_param_share, false);
            }
  #endif
        }
#endif /*MTK_BT_A2DP_VENDOR_ENABLE*/
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
        else if(internal_handle->codec_info.codec_cap.type == 0x65)//BT_A2DP_CODEC_AIRO_CELT=0x65
        {
            bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
            handle->play    = bt_a2dp_sink_airo_celt_play;
            handle->stop    = bt_a2dp_sink_airo_celt_stop;
            handle->process = bt_a2dp_sink_airo_celt_process;
            handle->side_tone_enable = bt_side_tone_enable;
            handle->side_tone_disable = bt_side_tone_disable;
            #if 0
            result = bt_a2dp_sink_parse_sbc_info(internal_handle);
            if (BT_CODEC_MEDIA_STATUS_OK != result) {
                if (internal_handle != NULL) {
                    vPortFree(internal_handle);
                    internal_handle = NULL;
                }
                return 0;
            }
            #endif
            internal_handle->channel_number = 2;
            internal_handle->sample_rate = HAL_AUDIO_SAMPLING_RATE_48KHZ;
            hal_audio_set_stream_out_channel_number((internal_handle->channel_number == 1) ? HAL_AUDIO_MONO : HAL_AUDIO_STEREO);
            hal_audio_set_stream_out_sampling_rate((hal_audio_sampling_rate_t)internal_handle->sample_rate);

#if defined(MTK_AVM_DIRECT)
            {
                mcu2dsp_open_param_t open_param;
                void *p_param_share;
                uint32_t sink_latency = bt_sink_srv_ami_get_a2dp_sink_latency();
                memset(&open_param, 0, sizeof(mcu2dsp_open_param_t));
                open_param.param.stream_in  = STREAM_IN_A2DP;
                open_param.param.stream_out = STREAM_OUT_AFE;

                memcpy(&open_param.stream_in_param.a2dp.codec_info, param, sizeof(bt_codec_a2dp_audio_t));
                open_param.stream_in_param.a2dp.p_current_bit_rate = hal_audio_report_bitrate_buf();
                open_param.stream_in_param.a2dp.p_lostnum_report = hal_audio_report_lostnum_buf();
                open_param.stream_in_param.a2dp.p_audio_sync_info = hal_audio_query_audio_sync_info();
                open_param.stream_in_param.a2dp.sink_latency    = sink_latency;
                #ifndef AVM_SHAEE_BUF_INFO
                open_param.stream_in_param.a2dp.p_share_info    = hal_audio_query_bt_audio_dl_share_info();
                open_param.stream_in_param.a2dp.p_asi_buf       = hal_audio_query_ltcs_asi_buf();
                open_param.stream_in_param.a2dp.p_min_gap_buf   = hal_audio_query_ltcs_min_gap_buf();
                open_param.stream_in_param.a2dp.bt_inf_address  = bt_sink_srv_ami_get_bt_inf_address();
                open_param.stream_in_param.a2dp.clk_info_address   = hal_audio_query_rcdc_share_info();
                hal_audio_a2dp_reset_share_info( open_param.stream_in_param.a2dp.p_share_info );
                ((n9_dsp_share_info_t*)(open_param.stream_in_param.a2dp.p_share_info))->next = (U32)hal_audio_query_audio_sync_info();

                open_param.stream_in_param.a2dp.p_current_bit_rate = hal_audio_report_bitrate_buf();
                open_param.stream_in_param.a2dp.p_lostnum_report = hal_audio_report_lostnum_buf();
                open_param.stream_in_param.a2dp.p_audio_sync_info = hal_audio_query_audio_sync_info();
#ifdef MTK_AUDIO_PLC_ENABLE
                open_param.stream_in_param.a2dp.audio_plc.enable= true;
#endif
                #else
                open_param.stream_in_param.a2dp.p_afe_buf_report   = hal_audio_afe_dl_buf_report();
                open_param.stream_in_param.a2dp.p_pcdc_anchor_info_buf   = hal_audio_query_ltcs_anchor_info_buf();
                open_param.stream_in_param.a2dp.p_share_info    = (avm_share_buf_info_t*)hal_audio_query_audio_transmitter_share_info(AUDIO_TRANSMITTER_SHARE_INFO_INDEX_GAMING_MODE_BT_RECEIVE_FROM_AIR);
                open_param.stream_in_param.a2dp.ull_clk_info_address = hal_audio_query_ull_rcdc_share_info();
                #if 0
                uint32_t block_size = sbc_frame_size_calc(param->codec_cap.codec.sbc.max_bit_pool,
                                                          param->codec_cap.codec.sbc.subband_num,
                                                          param->codec_cap.codec.sbc.block_length,
                                                          param->codec_cap.codec.sbc.channel_mode);
                block_size = 180;
                bt_AM_set_a2dp_AVM_info(open_param.stream_in_param.a2dp.p_share_info,
                                        hal_audio_sampling_rate_enum_to_value(internal_handle->sample_rate),
                                        internal_handle->codec_info.codec_cap.type);
                hal_audio_set_avm_info(open_param.stream_in_param.a2dp.p_share_info,
                                        SHARE_BUFFER_BT_AUDIO_DL_SIZE,
                                        block_size,
                                        sink_latency);
                #endif
                #endif

#if 0
                open_param.stream_out_param.afe.audio_device    = HAL_AUDIO_DEVICE_DAC_DUAL;
                open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
                open_param.stream_out_param.afe.stream_channel  = HAL_AUDIO_DIRECT;
                open_param.stream_out_param.afe.misc_parms      = DOWNLINK_PERFORMANCE_NORMAL;
#else
                hal_audio_get_stream_out_setting_config(AU_DSP_AUDIO, &open_param.stream_out_param);
                TASK_LOG_MSGID_I("[A2DP]Open codec AIRO_CELT out_device(0x%x), channel(%d), interface(%d)", 3, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.stream_channel, open_param.stream_out_param.afe.audio_interface);
#endif
                open_param.stream_out_param.afe.memory          = HAL_AUDIO_MEM1;
                open_param.stream_out_param.afe.format          = AFE_PCM_FORMAT_S32_LE;
                open_param.stream_out_param.afe.stream_out_sampling_rate   = hal_audio_sampling_rate_enum_to_value(internal_handle->sample_rate);
                //open_param.stream_out_param.afe.stream_out_sampling_rate  = p_hdl_effect->get_output_samplingrate(internal_handle->sample_rate);

#ifdef MTK_LEAKAGE_COMPENSATION_ENABLE
                if (audio_anc_leakage_compensation_get_status()) {
                    open_param.stream_out_param.afe.sampling_rate = 16000;
                    TASK_LOG_MSGID_I("[RECORD_LC]set a2dp sr to 16000, lc status:%d", 1, audio_anc_leakage_compensation_get_status());
                } else {
                    open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(hal_audio_get_device_out_supported_frequency(open_param.stream_out_param.afe.audio_device,internal_handle->sample_rate));
                }
#else
                open_param.stream_out_param.afe.sampling_rate   = hal_audio_sampling_rate_enum_to_value(hal_audio_get_device_out_supported_frequency(open_param.stream_out_param.afe.audio_device,internal_handle->sample_rate));
#endif
                //open_param.stream_out_param.afe.sampling_rate   = hal_audio_get_device_out_supported_frequency(HAL_AUDIO_DEVICE_HEADSET,open_param.stream_out_param.afe.stream_out_sampling_rate)
#if defined (FIXED_SAMPLING_RATE_TO_48KHZ)
                open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_48K_SAMPLE_RATE;
#elif defined (AIR_FIXED_DL_SAMPLING_RATE_TO_96KHZ)
                open_param.stream_out_param.afe.sampling_rate   = HAL_AUDIO_FIXED_AFE_96K_SAMPLE_RATE;
#endif
                open_param.stream_out_param.afe.irq_period      = 1;
                open_param.stream_out_param.afe.frame_size      = 1024;
                open_param.stream_out_param.afe.frame_number    = 4;
                open_param.stream_out_param.afe.hw_gain         = true;
#ifdef ENABLE_HWSRC_CLKSKEW
                open_param.stream_out_param.afe.clkskew_mode    = CLK_SKEW_V1;
#endif
                if (open_param.stream_out_param.afe.audio_device == HAL_AUDIO_DEVICE_I2S_MASTER) {
                    if (open_param.stream_out_param.afe.sampling_rate > 48000) {
                        open_param.stream_out_param.afe.misc_parms      |= I2S_CLK_SOURCE_APLL;
                    }
                }
#if defined(MTK_EXTERNAL_DSP_NEED_SUPPORT)
                ami_set_afe_param(STREAM_OUT, internal_handle->sample_rate, true);
#endif
                p_param_share = hal_audio_dsp_controller_put_paramter( &open_param, sizeof(mcu2dsp_open_param_t), AUDIO_MESSAGE_TYPE_BT_AUDIO_DL);

#if defined(HAL_AUDIO_SUPPORT_APLL)
                //Open i2s master out low jitter mode
                hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_BT_CODEC_A2DP_OUT, open_param.stream_out_param.afe.audio_device, open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.is_low_jitter, true);
#endif

                hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, open_param.stream_out_param.afe.audio_device, MAX(open_param.stream_out_param.afe.sampling_rate, open_param.stream_out_param.afe.stream_out_sampling_rate), TRUE);
                // Notify to do dynamic download. Use async wait.
                //open_param.stream_out_param.afe.sampling_rate   = 48000;
                open_param.stream_out_param.afe.audio_interface = HAL_AUDIO_INTERFACE_1;
#ifdef ENABLE_HWSRC_CLKSKEW
                open_param.stream_out_param.afe.clkskew_mode = 0;
#endif
                hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_OPEN, AUDIO_DSP_CODEC_TYPE_OPUS, (uint32_t)p_param_share, false);
            }
#endif

#ifdef BT_A2DP_BITSTREAM_DUMP_DEBUG
            TASK_LOG_MSGID_I("[A2DP][AIRO_CELT]Codec open", 0);
#endif
        }
#endif
        else {
            if (internal_handle != NULL) {
                vPortFree(internal_handle);
                internal_handle = NULL;
            }
            return 0;
        }
    } else {
#ifdef MTK_BT_A2DP_SOURCE_SUPPORT
        /* A2DP source role */
        if (internal_handle->codec_info.codec_cap.type == BT_A2DP_CODEC_SBC) {
            bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
            handle->play    = bt_a2dp_source_sbc_play;
            handle->stop    = bt_a2dp_source_sbc_stop;
            handle->process = bt_a2dp_source_sbc_process;

            handle->query_memory_size  = bt_codec_a2dp_source_query_memory_size;
            handle->set_bit_rate       = bt_codec_a2dp_source_set_bit_rate;
            handle->get_payload        = bt_codec_a2dp_source_get_payload;
            handle->get_payload_done   = bt_codec_a2dp_source_get_payload_done;
            handle->query_payload_size = bt_codec_a2dp_source_query_payload_size;

            handle->side_tone_enable = bt_side_tone_enable;
            handle->side_tone_disable = bt_side_tone_disable;
            result = bt_a2dp_source_parse_sbc_info(internal_handle);
            if (BT_CODEC_MEDIA_STATUS_OK != result) {
                if (internal_handle != NULL) {
                    vPortFree(internal_handle);
                    internal_handle = NULL;
                }
                return 0;
            }
        } else {
            if (internal_handle != NULL) {
                vPortFree(internal_handle);
                internal_handle = NULL;
            }
            return 0;
        }
#endif /*MTK_BT_A2DP_SOURCE_SUPPORT*/
    }
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__) && !defined(MTK_AVM_DIRECT)
    {
        int32_t result = bt_codec_a2dp_aws_open_setting(internal_handle);
        if (result < 0) {
            TASK_LOG_MSGID_I("[A2DP][AWS]alloc fail, result = %d\r\n",1, (int)result);
            if (internal_handle != NULL) {
                vPortFree(internal_handle);
                internal_handle = NULL;
            }
            return 0;
        }
    }
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    handle->state = BT_CODEC_STATE_READY;
    return handle;
}

bt_codec_media_status_t bt_codec_a2dp_close(bt_media_handle_t *handle)
{
    bt_a2dp_audio_internal_handle_t *internal_handle = (bt_a2dp_audio_internal_handle_t *) handle;

    TASK_LOG_MSGID_I("[A2DP]Close codec\n", 0);
    if (handle->state != BT_CODEC_STATE_STOP && handle->state != BT_CODEC_STATE_READY) {
        clock_mux_sel(CLK_AUD_BUS_SEL, 0);
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    handle->state = BT_CODEC_STATE_IDLE;

#if defined(MTK_AVM_DIRECT)
    // Notify to stop
    hal_audio_dsp_controller_send_message(MSG_MCU2DSP_BT_AUDIO_DL_CLOSE, 0, 0, true);
#else
  #if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_codec_a2dp_aws_close_setting(internal_handle);
  #endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    audio_free_id(handle->audio_id);
#endif
    if (internal_handle != NULL) {
        vPortFree(internal_handle);
        internal_handle = NULL;
    }
#if defined(HAL_AUDIO_SUPPORT_APLL)
    //Close i2s master out low jitter mode
    hal_audio_i2s_master_out_low_jitter_mode(LOW_JITTER_MODE_BT_CODEC_A2DP_OUT, 0, 0, 0, false);
#endif
    hal_audio_dsp_dl_clkmux_control(AUDIO_MESSAGE_TYPE_BT_AUDIO_DL, 0, 0, FALSE);
    clock_mux_sel(CLK_AUD_BUS_SEL, 0);
    return BT_CODEC_MEDIA_STATUS_OK;
}

bt_codec_media_status_t bt_codec_a2dp_aws_set_clock_skew(bool flag)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
  #if !defined(MTK_AVM_DIRECT)
    if (flag) {
        audio_service_aws_set_clock_skew(true);
    } else {
        audio_service_aws_set_clock_skew(false);
    }
  #endif
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}
bt_codec_media_status_t bt_codec_a2dp_aws_set_flag(bt_media_handle_t *handle, bool flag)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    p_info->aws_flag = flag;
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_set_initial_sync(bt_media_handle_t *handle)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
  #if defined(MTK_AVM_DIRECT)
    return BT_CODEC_MEDIA_STATUS_OK;
  #else
    bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    if (handle != NULL && p_info->aws_internal_flag) {
        p_info->aws_init_sync_flag = true;
        audio_service_aws_set_initial_sync();
    } else {
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return result;
  #endif
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_get_consumed_data_count(bt_media_handle_t *handle, bt_codec_a2dp_data_count_t *information)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
  #if !defined(MTK_AVM_DIRECT)
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    information->sample_count  = audio_service_aws_get_accumulated_sample_count();
    information->sampling_rate = bt_codec_a2dp_aws_convert_sampling_rate_from_index_to_value(p_info->sample_rate);
  #endif
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_get_silence_frame_information(bt_media_handle_t *handle, bt_codec_a2dp_bitstream_t *information)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__) && !defined(MTK_AVM_DIRECT)
    bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    bt_a2dp_codec_type_t codec_type = p_info->codec_info.codec_cap.type;
    uint32_t *sil_smpl_count = &information->sample_count;
    uint32_t *sil_byte_count = &information->byte_count;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        bt_codec_sbc_t *p_codec_info = &p_info->codec_info.codec_cap.codec.sbc;
        sbc_set_silence_pattern_frame_sample(p_codec_info->block_length, p_codec_info->subband_num);
        audio_service_aws_get_silence_frame_information(AWS_CODEC_TYPE_SBC_FORMAT, sil_smpl_count, sil_byte_count);
    } else if (codec_type == BT_A2DP_CODEC_AAC) {
  #ifdef MTK_BT_A2DP_AAC_ENABLE
        audio_service_aws_get_silence_frame_information(AWS_CODEC_TYPE_AAC_FORMAT, sil_smpl_count, sil_byte_count);
  #else   /* MTK_BT_A2DP_AAC_ENABLE */
        information->sample_count = 0;
        information->byte_count = 0;
        result = BT_CODEC_MEDIA_STATUS_ERROR;
  #endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else {
        information->sample_count = 0;
        information->byte_count = 0;
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return result;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_fill_silence_frame(bt_media_handle_t *handle, bt_codec_a2dp_buffer_t *data, uint32_t target_frm_cnt)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__) && !defined(MTK_AVM_DIRECT)
    int32_t result;
    uint32_t *sil_frm_cnt = &target_frm_cnt;
    uint32_t *buf_byte_cnt = &data->byte_count;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    bt_a2dp_codec_type_t codec_type = p_info->codec_info.codec_cap.type;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        bt_codec_sbc_t *p_codec_info = &p_info->codec_info.codec_cap.codec.sbc;
        sbc_set_silence_pattern_frame_sample(p_codec_info->block_length, p_codec_info->subband_num);
        result = audio_service_aws_fill_silence_frame(data->buffer, buf_byte_cnt, AWS_CODEC_TYPE_SBC_FORMAT, sil_frm_cnt);
        if (result == HAL_AUDIO_AWS_ERROR || result == HAL_AUDIO_AWS_NOT_SUPPORT) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else if (codec_type == BT_A2DP_CODEC_AAC) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        result = audio_service_aws_fill_silence_frame(data->buffer, buf_byte_cnt, AWS_CODEC_TYPE_AAC_FORMAT, sil_frm_cnt);
        if (result == HAL_AUDIO_AWS_ERROR || result == HAL_AUDIO_AWS_NOT_SUPPORT) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
#else   /* MTK_BT_A2DP_AAC_ENABLE */
        return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_parse_data_information(bt_media_handle_t *handle, bt_codec_a2dp_buffer_t *data, bt_codec_a2dp_bitstream_t *information)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__) && !defined(MTK_AVM_DIRECT)
    int32_t result;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    bt_a2dp_codec_type_t codec_type = p_info->codec_info.codec_cap.type;
    uint32_t *out_smpl_cnt = &information->sample_count;
    uint32_t *out_byte_cnt = &information->byte_count;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        result = audio_service_aws_parse_bitstream_information(data->buffer, data->byte_count, AWS_CODEC_TYPE_SBC_FORMAT, out_smpl_cnt, out_byte_cnt);
        if (result == HAL_AUDIO_AWS_ERROR || result == HAL_AUDIO_AWS_NOT_SUPPORT) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
    } else if (codec_type == BT_A2DP_CODEC_AAC) {
  #ifdef MTK_BT_A2DP_AAC_ENABLE
        result = audio_service_aws_parse_bitstream_information(data->buffer, data->byte_count, AWS_CODEC_TYPE_AAC_FORMAT, out_smpl_cnt, out_byte_cnt);
        if (result == HAL_AUDIO_AWS_ERROR || result == HAL_AUDIO_AWS_NOT_SUPPORT) {
            return BT_CODEC_MEDIA_STATUS_ERROR;
        }
  #else   /* MTK_BT_A2DP_AAC_ENABLE */
        information->sample_count = 0;
        information->byte_count = 0;
        return BT_CODEC_MEDIA_STATUS_ERROR;
  #endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else {
        information->sample_count = 0;
        information->byte_count = 0;
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_get_clock_skew_status(bt_media_handle_t *handle, bt_codec_aws_clock_skew_status_t *status)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__) && !defined(MTK_AVM_DIRECT)
    aws_clock_skew_status_t hal_aws_status;
    hal_aws_status = audio_service_aws_get_clock_skew_status();
    if (hal_aws_status == AWS_CLOCK_SKEW_STATUS_IDLE) {
        *status = BT_CODEC_AWS_CLOCK_SKEW_STATUS_IDLE;
    } else if (hal_aws_status == AWS_CLOCK_SKEW_STATUS_BUSY) {
        *status = BT_CODEC_AWS_CLOCK_SKEW_STATUS_BUSY;
    }
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_set_clock_skew_compensation_value(bt_media_handle_t *handle, int32_t sample_count)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
  #if !defined(MTK_AVM_DIRECT)
    int result;
    result = audio_service_aws_set_clock_skew_compensation_value(sample_count);
    if (result < 0) {
        return BT_CODEC_MEDIA_STATUS_ERROR;
    }
  #endif
    return BT_CODEC_MEDIA_STATUS_OK;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}

bt_codec_media_status_t bt_codec_a2dp_aws_get_audio_latency(bt_media_handle_t *handle, uint32_t sampling_rate, uint32_t *p_latency_us)
{
#if defined(__BT_A2DP_CODEC_AWS_SUPPORT__)
    bt_codec_media_status_t result = BT_CODEC_MEDIA_STATUS_OK;
    bt_a2dp_audio_internal_handle_t *p_info = (bt_a2dp_audio_internal_handle_t *)handle;
    bt_a2dp_codec_type_t codec_type = p_info->codec_info.codec_cap.type;
    uint32_t latency_us;
    if (codec_type == BT_A2DP_CODEC_SBC) {
        switch (sampling_rate) {
            case 48000:
                latency_us = 4805;
                break;
            case 44100:
                latency_us = 4805;
                break;
            case 32000:
                latency_us = 4805;
                break;
            case 16000:
                latency_us = 4805;
                break;
            default:
                latency_us = 0;
                result = BT_CODEC_MEDIA_STATUS_ERROR;
                break;
        }
    } else if (codec_type == BT_A2DP_CODEC_AAC) {
#ifdef MTK_BT_A2DP_AAC_ENABLE
        switch (sampling_rate) {
            case 48000:
                latency_us = 8604;
                break;
            case 44100:
                latency_us = 8604;
                break;
            case 32000:
                latency_us = 8604;
                break;
            case 24000:
                latency_us = 8604;
                break;
            case 22050:
                latency_us = 8604;
                break;
            case 16000:
                latency_us = 8604;
                break;
            case 12000:
                latency_us = 8604;
                break;
            case 11025:
                latency_us = 8604;
                break;
            case  8000:
                latency_us = 8604;
                break;
            default:
                latency_us = 0;
                result = BT_CODEC_MEDIA_STATUS_ERROR;
                break;
        }
#else   /* MTK_BT_A2DP_AAC_ENABLE */
        latency_us = 0;
        result = BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* MTK_BT_A2DP_AAC_ENABLE */
    } else {
        latency_us = 0;
        result = BT_CODEC_MEDIA_STATUS_ERROR;
    }
    *p_latency_us = latency_us;
    return result;
#else   /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
    return BT_CODEC_MEDIA_STATUS_ERROR;
#endif  /* defined(__BT_A2DP_CODEC_AWS_SUPPORT__) */
}


void bt_codec_a2dp_set_sw_aac(bool flag)
{
    return;
}

bt_codec_media_status_t bt_codec_a2dp_set_sw_aac_flag(bool flag)
{
    return BT_CODEC_MEDIA_STATUS_OK;
}

