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

#ifdef AIR_BT_CODEC_BLE_ENABLED

#include "types.h"
#include "stream_audio.h"
#include "source_inter.h"
#include "sink_inter.h"
#include "transform.h"
#include "stream_n9ble.h"
#include "dsp_audio_msg.h"
#include "audio_config.h"
#include "dsp_audio_process.h"
#include "dsp_memory.h"
#include "dsp_share_memory.h"
#include "dsp_temp.h"
#include "dsp_dump.h"
#include "clk_skew.h"
#include "source_inter.h"
#ifdef MTK_AUDIO_TRANSMITTER_ENABLE
#include "stream_audio_transmitter.h"
#endif /* MTK_AUDIO_TRANSMITTER_ENABLE */

#define BLE_AVM_INVALID_TIMESTAMP       0xFFFFFFFF
#define BLE_AVM_FRAME_HEADER_SIZE       28
#define BLE_UL_ERROR_DETECT_THD         8
#define LE_AUDIO_IRQ_INTERVAL_MS        10
#define LE_AUDIO_DL_PROCESS_TIME_MS     2
#define LE_AUDIO_THD_EXTEND_TIME_MS     50

#define LE_AUDIO_OFFSET_PROTECT         4
#define LE_TIME_BT_CLOCK                625 /* unit with 312.5us */


#define BLE_AUDIO_DL_BUF_SIZE           ((28+156)*6)

#define BLE_AVM_FRAME_NUM(frame_length)               (BLE_AUDIO_DL_BUF_SIZE/ALIGN_4((frame_length) + BLE_AVM_FRAME_HEADER_SIZE))
#define abs32(x) ( (x >= 0) ? x : (-x) )

#define N9BLE_DEBUG

#ifndef ALIGN_4
#define ALIGN_4(_value)             (((_value) + 3) & ~3u)
#endif

Stream_n9ble_Config_Ptr N9BLE_setting;
static bool g_ble_pkt_lost[4][2];
static uint32_t g_pkt_lost_count = 0;

typedef struct  {
    U16 DataOffset; /* offset of payload */
    U16 _reserved_word_02h;
    U32 TimeStamp; /* this CIS/BIS link's CLK, Unit:312.5us */
    U16 ConnEvtCnt; /* event count seem on airlog, for debug propose */
    U8 SampleSeq;  /* Sameple sequence of this SDU interval Ex:0,1,2... */
    U8 _reserved_byte_0Bh; /* valid packet: 0x01, invalid packet 0x00 */
    U8 PduHdrLo;
    U8 _reserved_byte_0Dh;
    U8 PduLen ; /* payload size */
    U8 _reserved_byte_0Fh;
    U16 DataLen;
    U16 _reserved_word_12h;
    U32 _reserved_long_0;
    U32 _reserved_long_1;
} LE_AUDIO_HEADER;

EXTERN VOID StreamDSP_HWSemaphoreTake(VOID);
EXTERN VOID StreamDSP_HWSemaphoreGive(VOID);

ATTR_TEXT_IN_IRAM static VOID N9BleRx_update_from_share_information(SOURCE source)
{
    StreamDSP_HWSemaphoreTake();
    memcpy(&(source->streamBuffer.ShareBufferInfo), source->param.n9ble.share_info_base_addr, 40);/* share info fix 40 byte */
    source->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(source->streamBuffer.ShareBufferInfo.startaddr);
    /* Refer to the hal_audio_dsp_controller.c for the limitaion of legnth */
    source->streamBuffer.ShareBufferInfo.length = ALIGN_4(source->param.n9ble.frame_length + BLE_AVM_FRAME_HEADER_SIZE) * source->param.n9ble.process_number * BLE_AVM_FRAME_NUM(source->param.n9ble.frame_length);
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM static VOID N9BleTx_update_from_share_information(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    memcpy(&(sink->streamBuffer.ShareBufferInfo), sink->param.n9ble.share_info_base_addr, 40);/* share info fix 40 byte */
    sink->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(sink->streamBuffer.ShareBufferInfo.startaddr);
    /* Refer to the hal_audio_dsp_controller.c for the limitaion of legnth */
    sink->streamBuffer.ShareBufferInfo.length = ALIGN_4(sink->param.n9ble.frame_length + BLE_AVM_FRAME_HEADER_SIZE) * BLE_AVM_FRAME_NUM(sink->param.n9ble.frame_length);
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9Ble_update_readoffset_share_information(SOURCE source, U32 ReadOffset)
{
    StreamDSP_HWSemaphoreTake();
    source->param.n9ble.share_info_base_addr->ReadOffset = ReadOffset;
    source->param.n9ble.share_info_base_addr->bBufferIsFull = FALSE;
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM_LEVEL_2 VOID N9Ble_update_writeoffset_share_information(SINK sink, U32 WriteOffset)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.n9ble.share_info_base_addr->WriteOffset = WriteOffset;
    if (WriteOffset == sink->param.n9ble.share_info_base_addr->ReadOffset) {
        sink->param.n9ble.share_info_base_addr->bBufferIsFull = TRUE;
    }
    StreamDSP_HWSemaphoreGive();
}

ATTR_TEXT_IN_IRAM_LEVEL_2 static VOID N9Ble_update_timestamp_share_information(U32 *timestamp_ptr, U32 timestamp)
{
    StreamDSP_HWSemaphoreTake();
    *timestamp_ptr = timestamp;
    StreamDSP_HWSemaphoreGive();
}

static VOID N9Ble_Reset_Sourceoffset_share_information(SOURCE source)
{
    StreamDSP_HWSemaphoreTake();
    source->param.n9ble.share_info_base_addr->WriteOffset = 0;
    source->param.n9ble.share_info_base_addr->ReadOffset = 0;
    source->param.n9ble.share_info_base_addr->length = ALIGN_4(source->param.n9ble.frame_length + BLE_AVM_FRAME_HEADER_SIZE) * source->param.n9ble.process_number * BLE_AVM_FRAME_NUM(source->param.n9ble.frame_length);
    source->param.n9ble.share_info_base_addr->bBufferIsFull = FALSE;
    StreamDSP_HWSemaphoreGive();
}

static VOID N9Ble_Reset_Sinkoffset_share_information(SINK sink)
{
    StreamDSP_HWSemaphoreTake();
    sink->param.n9ble.share_info_base_addr->WriteOffset = 0;
    sink->param.n9ble.share_info_base_addr->ReadOffset = 0;
    sink->param.n9ble.share_info_base_addr->length = ALIGN_4(sink->param.n9ble.frame_length + BLE_AVM_FRAME_HEADER_SIZE) * BLE_AVM_FRAME_NUM(sink->param.n9ble.frame_length);
    sink->param.n9ble.share_info_base_addr->bBufferIsFull = FALSE;
    StreamDSP_HWSemaphoreGive();
}

static VOID N9Ble_SinkUpdateLocalWriteOffset(SINK sink, U8 num)
{
    sink->streamBuffer.ShareBufferInfo.WriteOffset += ALIGN_4(sink->param.n9ble.frame_length + BLE_AVM_FRAME_HEADER_SIZE) * num;
    sink->streamBuffer.ShareBufferInfo.WriteOffset %= sink->streamBuffer.ShareBufferInfo.length;
}

VOID N9Ble_SourceUpdateLocalReadOffset(SOURCE source, U8 num)
{
    source->streamBuffer.ShareBufferInfo.ReadOffset += ALIGN_4(source->param.n9ble.frame_length + BLE_AVM_FRAME_HEADER_SIZE) * num;
    source->streamBuffer.ShareBufferInfo.ReadOffset %= source->streamBuffer.ShareBufferInfo.length;
}

static VOID N9Ble_Rx_Buffer_Init(SOURCE source)
{
    N9BleRx_update_from_share_information(source);
    N9Ble_Reset_Sourceoffset_share_information(source);
}

static VOID N9Ble_Tx_Buffer_Init(SINK sink)
{
    N9BleTx_update_from_share_information(sink);
    N9Ble_Reset_Sinkoffset_share_information(sink);
}

static VOID N9Ble_Default_setting_init(SOURCE source, SINK sink)
{
    if (N9BLE_setting == NULL) {
        N9BLE_setting = pvPortMalloc(sizeof(Stream_n9ble_Config_t));
        memset(N9BLE_setting, 0, sizeof(Stream_n9ble_Config_t));
    }

    if (source != NULL) {
        N9BLE_setting->N9Ble_source.Buffer_Frame_Num        = BLE_AVM_FRAME_NUM(source->param.n9ble.frame_length);
        N9BLE_setting->N9Ble_source.Process_Frame_Num       = source->param.n9ble.process_number;
        N9BLE_setting->N9Ble_source.Frame_Size              = ALIGN_4(BLE_AVM_FRAME_HEADER_SIZE + source->param.n9ble.frame_length);
        DSP_MW_LOG_I("[BLE] source Frame_Size: %d  Buffer_Frame_Num: %d", 2, N9BLE_setting->N9Ble_source.Frame_Size, N9BLE_setting->N9Ble_source.Buffer_Frame_Num);
    }

    if (sink != NULL) {
        N9BLE_setting->N9Ble_sink.Buffer_Frame_Num          = BLE_AVM_FRAME_NUM(sink->param.n9ble.frame_length);
        N9BLE_setting->N9Ble_sink.Process_Frame_Num         = sink->param.n9ble.process_number;
        N9BLE_setting->N9Ble_sink.Frame_Size                = ALIGN_4(BLE_AVM_FRAME_HEADER_SIZE + sink->param.n9ble.frame_length);
        N9BLE_setting->N9Ble_sink.N9_Ro_abnormal_cnt        = 0;
        DSP_MW_LOG_I("[BLE] sink Frame_Size: %d  Buffer_Frame_Num: %d", 2, N9BLE_setting->N9Ble_sink.Frame_Size, N9BLE_setting->N9Ble_sink.Buffer_Frame_Num);
    }
}

//#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
#ifdef AIR_FIXED_RATIO_SRC
#include "src_fixed_ratio_interface.h"

static uint32_t internal_fs_converter(stream_samplerate_t fs)
{
    switch(fs) {
        case FS_RATE_44_1K:
            return 44100;

        case FS_RATE_8K:
        case FS_RATE_16K:
        case FS_RATE_24K:
        case FS_RATE_32K:
        case FS_RATE_48K:
        case FS_RATE_96K:
            return fs * 1000;

        default:
            DSP_MW_LOG_E("[BLE] sample rate is not supported!", 0);
            OS_ASSERT(FALSE);
            return fs;
    }
}

#ifdef AIR_BLE_SWB_ENABLE
static src_fixed_ratio_port_t *g_n9ble_dl_swb_fixed_ratio_port1;
static src_fixed_ratio_port_t *g_n9ble_dl_swb_fixed_ratio_port2;
static src_fixed_ratio_port_t *g_n9ble_dl_swb_fixed_ratio_port3;


static src_fixed_ratio_port_t *g_n9ble_ul_swb_fixed_ratio_port1;
static src_fixed_ratio_port_t *g_n9ble_ul_swb_fixed_ratio_port2;
static src_fixed_ratio_port_t *g_n9ble_ul_swb_fixed_ratio_port3;

static const N9ble_Swsrc_Config_t N9Ble_Swsrc_maping_table[] = {
    /*fs_in, fs_out, fs_tmp1, fs_tmp2, swb_cnt*/
    { 8, 8, 0, 0, 0},
    { 8,16, 0, 0, 1},
    { 8,24, 0, 0, 1},
    { 8,32,16, 0, 2},
    { 8,48,16, 0, 2},
    { 8,96,16,32, 3},
    {16,16, 0, 0, 0},
    {16,24,48, 0, 2},
    {16,32, 0, 0, 1},
    {16,48, 0, 0, 1},
    {16,96,32, 0, 2},
    {24,24, 0, 0, 0},
    {24,32,48,96, 3},
    {24,48, 0, 0, 1},
    {24,96,48, 0, 2},
    {32,32, 0, 0, 0},
    {32,48,96, 0, 2},
    {32,96, 0, 0, 1},
    {48,48, 0, 0, 0},
    {48,96, 0, 0, 1},
    {96,96, 0, 0, 0},
};

void N9Ble_DL_SWB_Sample_Rate_Init(void)
{
    uint32_t channel_number;
    DSP_STREAMING_PARA_PTR dl_stream;
    src_fixed_ratio_config_t smp_config1,smp_config2, smp_config3;
    volatile SOURCE source = Source_blks[SOURCE_TYPE_N9BLE];

    dl_stream = DSP_Streaming_Get(source,source->transform->sink);
    channel_number = stream_function_get_channel_number(&(dl_stream->callback.EntryPara));
    stream_samplerate_t fs_in = dl_stream->callback.EntryPara.in_sampling_rate;
    stream_samplerate_t fs_out = dl_stream->sink->param.audio.src_rate/1000;//dl_stream->callback.EntryPara.codec_out_sampling_rate;
    stream_samplerate_t fs_temp, fs_temp2;
    uint8_t swb_cnt;
    /*fs_in = FS_RATE_32K;
    fs_temp = FS_RATE_16K;//fs_out;
    fs_temp2 = FS_RATE_48K;//fs_out;
    fs_out = FS_RATE_16K;
    swb_cnt = 1;*/

    uint32_t i;
    for(i=0;i<ARRAY_SIZE(N9Ble_Swsrc_maping_table);i++){
        if(fs_in <= fs_out){
            if((fs_in == N9Ble_Swsrc_maping_table[i].fs_in) &&(fs_out == N9Ble_Swsrc_maping_table[i].fs_out)){
                fs_temp = N9Ble_Swsrc_maping_table[i].fs_tmp1;
                fs_temp2 = N9Ble_Swsrc_maping_table[i].fs_tmp2;
                swb_cnt = N9Ble_Swsrc_maping_table[i].swb_cnt;
                break;
            }
        }else{
            if((fs_out == N9Ble_Swsrc_maping_table[i].fs_in) &&(fs_in == N9Ble_Swsrc_maping_table[i].fs_out)){
                fs_temp = N9Ble_Swsrc_maping_table[i].fs_tmp2;
                fs_temp2 = N9Ble_Swsrc_maping_table[i].fs_tmp1;
                swb_cnt = N9Ble_Swsrc_maping_table[i].swb_cnt;
                break;
            }
        }
    }

    if((!fs_temp)&&(!fs_temp2)){ /*swb_cnt=1*/
        fs_temp = fs_out;
        fs_temp2 = fs_out;
    }
    if((!fs_temp)||(!fs_temp2)){ /*swb_cnt<3*/
        if(!fs_temp){
            fs_temp = fs_temp2;
            fs_temp2 = fs_out;
        }else{
            fs_temp2 = fs_out;
        }
    }
    DSP_MW_LOG_I("[BLE] N9Ble_DL_SWB_Sample_Rate_Init %d %d %d %d %d",5,fs_in,fs_temp,fs_temp2,fs_out,swb_cnt);

    smp_config1.channel_number = channel_number;
    smp_config1.in_sampling_rate = internal_fs_converter(fs_in);
    smp_config1.out_sampling_rate = internal_fs_converter(fs_temp);
    smp_config1.resolution = dl_stream->callback.EntryPara.resolution.feature_res;
    smp_config1.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_CONSECUTIVE;
    smp_config1.cvt_num = 1;

    if(swb_cnt >= 2){
        smp_config1.cvt_num = 2;

        smp_config2.channel_number = channel_number;
        smp_config2.in_sampling_rate = internal_fs_converter(fs_temp);
        smp_config2.out_sampling_rate = internal_fs_converter(fs_temp2);
        smp_config2.resolution = dl_stream->callback.EntryPara.resolution.feature_res;
        smp_config2.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_CONSECUTIVE;
        smp_config2.cvt_num = 2;
    }

    if(swb_cnt == 3){
        smp_config1.cvt_num = 3;
        smp_config2.cvt_num = 3;

        smp_config3.channel_number = channel_number;
        smp_config3.in_sampling_rate = internal_fs_converter(fs_temp2);
        smp_config3.out_sampling_rate = internal_fs_converter(fs_out);
        smp_config3.resolution = dl_stream->callback.EntryPara.resolution.feature_res;
        smp_config3.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_CONSECUTIVE;
        smp_config3.cvt_num = 3;
    }

    g_n9ble_dl_swb_fixed_ratio_port1 = stream_function_src_fixed_ratio_get_port(source);
    stream_function_src_fixed_ratio_init(g_n9ble_dl_swb_fixed_ratio_port1, &smp_config1);

    if(swb_cnt >1){
       g_n9ble_dl_swb_fixed_ratio_port2 = stream_function_src_fixed_ratio_get_2nd_port(source);
       stream_function_src_fixed_ratio_init(g_n9ble_dl_swb_fixed_ratio_port2, &smp_config2);
    }

    if(swb_cnt >2){
        g_n9ble_dl_swb_fixed_ratio_port3 = stream_function_src_fixed_ratio_get_3rd_port(source);
        stream_function_src_fixed_ratio_init(g_n9ble_dl_swb_fixed_ratio_port3, &smp_config3);
    }

    DSP_MW_LOG_I("[BLE] N9Ble_DL_SWB_Sample_Rate_Init: channel_number %d, fs_in %d, fs_temp %d, fs_temp2 %d, fs_out %d", 5,
                smp_config1.channel_number, fs_in, fs_temp, fs_temp2, fs_out);
}

void N9Ble_DL_SWB_Sample_Rate_Deinit(void)
{

    DSP_MW_LOG_I("[BLE]N9Ble_DL_SWB_Sample_Rate_Deinit",0);

    if (g_n9ble_dl_swb_fixed_ratio_port1) {
        stream_function_src_fixed_ratio_deinit(g_n9ble_dl_swb_fixed_ratio_port1);
        g_n9ble_dl_swb_fixed_ratio_port1 = NULL;
    }

    if (g_n9ble_dl_swb_fixed_ratio_port2) {
        stream_function_src_fixed_ratio_deinit(g_n9ble_dl_swb_fixed_ratio_port2);
        g_n9ble_dl_swb_fixed_ratio_port2 = NULL;
    }

    if (g_n9ble_dl_swb_fixed_ratio_port3) {
        stream_function_src_fixed_ratio_deinit(g_n9ble_dl_swb_fixed_ratio_port3);
        g_n9ble_dl_swb_fixed_ratio_port3 = NULL;
    }
}

void N9Ble_UL_SWB_Sample_Rate_Init(void)
{
    uint32_t channel_number;
    DSP_STREAMING_PARA_PTR ul_stream;
    src_fixed_ratio_config_t smp_config1,smp_config2,smp_config3;
    volatile SINK sink = Sink_blks[SINK_TYPE_N9BLE];

    ul_stream = DSP_Streaming_Get(sink->transform->source, sink);
    channel_number = stream_function_get_channel_number(&(ul_stream->callback.EntryPara));
    stream_samplerate_t fs_in = ul_stream->callback.EntryPara.in_sampling_rate;
    stream_samplerate_t fs_out = ul_stream->callback.EntryPara.codec_out_sampling_rate;
    stream_samplerate_t fs_temp, fs_temp2;
    uint8_t swb_cnt;
    /*fs_in = FS_RATE_16K;
    fs_temp = FS_RATE_32K;//fs_out;
    fs_temp2 = FS_RATE_96K;//fs_out;
    fs_out = FS_RATE_32K;
    swb_cnt = 1;*/

    uint32_t i;
    for(i=0;i<ARRAY_SIZE(N9Ble_Swsrc_maping_table);i++){
        if(fs_in <= fs_out){
            if((fs_in == N9Ble_Swsrc_maping_table[i].fs_in) &&(fs_out == N9Ble_Swsrc_maping_table[i].fs_out)){
                fs_temp = N9Ble_Swsrc_maping_table[i].fs_tmp1;
                fs_temp2 = N9Ble_Swsrc_maping_table[i].fs_tmp2;
                swb_cnt = N9Ble_Swsrc_maping_table[i].swb_cnt;
                break;
            }
        }else{
            if((fs_out == N9Ble_Swsrc_maping_table[i].fs_in) &&(fs_in == N9Ble_Swsrc_maping_table[i].fs_out)){
                fs_temp = N9Ble_Swsrc_maping_table[i].fs_tmp2;
                fs_temp2 = N9Ble_Swsrc_maping_table[i].fs_tmp1;
                swb_cnt = N9Ble_Swsrc_maping_table[i].swb_cnt;
                break;
            }
        }
    }

    if((!fs_temp)&&(!fs_temp2)){
        fs_temp = fs_out;
        fs_temp2 = fs_out;
    }
    if((!fs_temp)||(!fs_temp2)){
        if(!fs_temp){
            fs_temp = fs_temp2;
            fs_temp2 = fs_out;
        }else{
            fs_temp2 = fs_out;
        }
    }



    DSP_MW_LOG_I("[BLE] N9Ble_UL_SWB_Sample_Rate_Init %d %d %d %d %d",5,fs_in,fs_temp,fs_temp2,fs_out,swb_cnt);

    smp_config1.channel_number = channel_number;
    smp_config1.in_sampling_rate = internal_fs_converter(fs_in);
    smp_config1.out_sampling_rate = internal_fs_converter(fs_temp);
    smp_config1.resolution = ul_stream->callback.EntryPara.resolution.feature_res;
    smp_config1.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_CONSECUTIVE;
    smp_config1.cvt_num = 1;

    if(swb_cnt >= 2){
        smp_config1.cvt_num = 2;

        smp_config2.channel_number = channel_number;
        smp_config2.in_sampling_rate = internal_fs_converter(fs_temp);
        smp_config2.out_sampling_rate = internal_fs_converter(fs_temp2);
        smp_config2.resolution = ul_stream->callback.EntryPara.resolution.feature_res;
        smp_config2.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_CONSECUTIVE;
        smp_config2.cvt_num = 2;
    }

    if(swb_cnt == 3){
        smp_config1.cvt_num = 3;
        smp_config2.cvt_num = 3;

        smp_config3.channel_number = channel_number;
        smp_config3.in_sampling_rate = internal_fs_converter(fs_temp2);
        smp_config3.out_sampling_rate = internal_fs_converter(fs_out);
        smp_config3.resolution = ul_stream->callback.EntryPara.resolution.feature_res;
        smp_config3.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_CONSECUTIVE;
        smp_config3.cvt_num = 3;
    }

    g_n9ble_ul_swb_fixed_ratio_port1 = stream_function_src_fixed_ratio_get_port(sink);
    stream_function_src_fixed_ratio_init(g_n9ble_ul_swb_fixed_ratio_port1, &smp_config1);

    if(swb_cnt >1){
        g_n9ble_ul_swb_fixed_ratio_port2 = stream_function_src_fixed_ratio_get_2nd_port(sink);
        stream_function_src_fixed_ratio_init(g_n9ble_ul_swb_fixed_ratio_port2, &smp_config2);
    }

    if(swb_cnt >2){
        g_n9ble_ul_swb_fixed_ratio_port3 = stream_function_src_fixed_ratio_get_3rd_port(sink);
        stream_function_src_fixed_ratio_init(g_n9ble_ul_swb_fixed_ratio_port3, &smp_config3);
        }

    DSP_MW_LOG_I("[BLE] N9Ble_UL_SWB_Sample_Rate_Init: channel_number %d, fs_in %d, fs_temp %d, fs_temp2 %d, fs_out %d", 5,
        smp_config1.channel_number, fs_in, fs_temp, fs_temp2, fs_out);

}

void N9Ble_UL_SWB_Sample_Rate_Deinit(void)
{
    DSP_MW_LOG_I("[BLE]N9Ble_UL_SWB_Sample_Rate_Deinit",0);

    if (g_n9ble_ul_swb_fixed_ratio_port1) {
        stream_function_src_fixed_ratio_deinit(g_n9ble_ul_swb_fixed_ratio_port1);
        g_n9ble_ul_swb_fixed_ratio_port1 = NULL;
    }

    if (g_n9ble_ul_swb_fixed_ratio_port2) {
        stream_function_src_fixed_ratio_deinit(g_n9ble_ul_swb_fixed_ratio_port2);
        g_n9ble_ul_swb_fixed_ratio_port2 = NULL;
    }

    if (g_n9ble_ul_swb_fixed_ratio_port3) {
        stream_function_src_fixed_ratio_deinit(g_n9ble_ul_swb_fixed_ratio_port3);
        g_n9ble_ul_swb_fixed_ratio_port3 = NULL;
    }

}

#endif
#ifdef AIR_UL_FIX_SAMPLING_RATE_48K
static src_fixed_ratio_port_t *g_n9ble_ul_src_fixed_ratio_port;

void N9Ble_UL_Fix_Sample_Rate_Init(void)
{
    uint32_t channel_number;
    DSP_STREAMING_PARA_PTR ul_stream;
    src_fixed_ratio_config_t smp_config;
    volatile SINK sink = Sink_blks[SINK_TYPE_N9BLE];

    ul_stream = DSP_Streaming_Get(sink->transform->source, sink);
    channel_number = stream_function_get_channel_number(&(ul_stream->callback.EntryPara));

    smp_config.channel_number = channel_number;
    smp_config.in_sampling_rate = internal_fs_converter((stream_samplerate_t)(ul_stream->callback.EntryPara.in_sampling_rate));
    smp_config.out_sampling_rate = 16000;//internal_fs_converter((stream_samplerate_t)(ul_stream->callback.EntryPara.codec_out_sampling_rate));
    smp_config.resolution = ul_stream->callback.EntryPara.resolution.feature_res;
    smp_config.multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE;
    smp_config.cvt_num = 1;
    g_n9ble_ul_src_fixed_ratio_port = stream_function_src_fixed_ratio_get_port(sink);

    DSP_MW_LOG_I("[BLE] src_fixed_ratio_init: channel_number %d, in_sampling_rate %d, out_sampling_rate %d, resolution %d", 4,
                smp_config.channel_number, smp_config.in_sampling_rate, smp_config.out_sampling_rate, smp_config.resolution);
    stream_function_src_fixed_ratio_init(g_n9ble_ul_src_fixed_ratio_port, &smp_config);
}

void N9Ble_UL_Fix_Sample_Rate_Deinit(void)
{
    if (g_n9ble_ul_src_fixed_ratio_port) {
        stream_function_src_fixed_ratio_deinit(g_n9ble_ul_src_fixed_ratio_port);
        g_n9ble_ul_src_fixed_ratio_port = NULL;
    }
}
#endif
#endif

/**
 * SinkSlackN9Ble
 *
 * Function to query the remain buffer free size of BLE sink.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 static U32 SinkSlackN9Ble(SINK sink)
{
//    DSP_MW_LOG_I("[BLE] SinkSlackN9Ble    sink frame_length: %d  process_number: %d", 2, sink->param.n9ble.frame_length, sink->param.n9ble.process_number);
    return sink->param.n9ble.frame_length *2; /* always guarantee buffer for two frames*/
}


/**
 * SinkClaimN9Ble
 *
 * Function to request the framework to write data into BLE sink.
 * Note: this function should NOT called by framework.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
static U32 SinkClaimN9Ble(SINK sink, U32 extra)
{
    UNUSED(sink);
    UNUSED(extra);

    DSP_MW_LOG_E("[BLE][sink] SinkClaimN9Ble called!!!", 0);
    configASSERT(0);

    return SINK_INVALID_CLAIM;
}

/**
 * SinkMapN9Ble
 *
 * Function to read the decoded data in BLE sink.
 * Note: this function should NOT called by framework.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
static U8 *SinkMapN9Ble(SINK sink)
{
    UNUSED(sink);

    DSP_MW_LOG_E("[BLE][sink] SinkMapN9Ble called!!!", 0);
    configASSERT(0);

    return 0;
}

/**
 * SinkFlushN9Ble
 *
 * Function to update the WPTR for BLE sink.
 *
 * param :amount - The amount of data written into sink.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
*/
ATTR_TEXT_IN_IRAM_LEVEL_2 static BOOL SinkFlushN9Ble(SINK sink, U32 amount)
{
    uint32_t ProcessFrameLen;

    ProcessFrameLen = sink->param.n9ble.frame_length * sink->param.n9ble.process_number;

    if (amount % ProcessFrameLen) {
        DSP_MW_LOG_E("[BLE][sink] flush size mismatch %d, %d!!!", 2, amount, ProcessFrameLen);
        configASSERT(0);
        return FALSE;
    }

    uint32_t flush_num = sink->param.n9ble.process_number;
    sink->param.n9ble.predict_timestamp += flush_num*((sink->param.n9ble.frame_interval<<1)/LE_TIME_BT_CLOCK);
    if(flush_num ) {
        N9BleTx_update_from_share_information(sink);
        N9Ble_SinkUpdateLocalWriteOffset(sink, flush_num);
        N9Ble_update_writeoffset_share_information(sink, sink->streamBuffer.ShareBufferInfo.WriteOffset);
    }
/*
    DSP_MW_LOG_I("[BLE][sink] SinkFlushN9Ble flush_num: %d  sink local offset:%X", 2, flush_num, sink->streamBuffer.ShareBufferInfo.WriteOffset);
*/
    return TRUE;
}

/**
 * SinkBufferWriteN9Ble
 *
 * Function to write the framwork data to BLE sink.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 static BOOL SinkBufferWriteN9Ble (SINK sink, U8 *src_addr, U32 length)
{
    U16 i;
    U8 *write_ptr;
    uint32_t ProcessFrameLen;
    LE_AUDIO_HEADER *buf_header;
    U32 timestamp = sink->param.n9ble.predict_timestamp;
    ProcessFrameLen = sink->param.n9ble.frame_length * sink->param.n9ble.process_number;
    if (length % ProcessFrameLen) {
        DSP_MW_LOG_E("[BLE][sink] write size mismatch %d, %d!!!", 2, length, ProcessFrameLen);
        configASSERT(0);
        return FALSE;
    }

    N9BleTx_update_from_share_information(sink);

    uint32_t write_num = sink->param.n9ble.process_number;

/*
    DSP_MW_LOG_I("SinkBufferWriteN9Ble src_addr:%X length:%d write_num:%d",3, src_addr, length, write_num);
*/

    for (i = 0 ; i < write_num; i++) {
        U8 *src_ptr = src_addr + (U32)(i * sink->param.n9ble.frame_length);
        write_ptr = (U8 *)(sink->streamBuffer.ShareBufferInfo.startaddr + sink->streamBuffer.ShareBufferInfo.WriteOffset);
        memcpy(write_ptr + BLE_AVM_FRAME_HEADER_SIZE, src_ptr, sink->param.n9ble.frame_length);
        buf_header = (LE_AUDIO_HEADER *)write_ptr;
        buf_header->DataOffset = BLE_AVM_FRAME_HEADER_SIZE;
        buf_header->PduLen = (U8)sink->param.n9ble.frame_length;
        buf_header->SampleSeq = 0;
        buf_header->TimeStamp = timestamp;
        timestamp += ((sink->param.n9ble.frame_interval<<1)/LE_TIME_BT_CLOCK);
        //DSP_MW_LOG_I("[BLE][sink]offset %d, ts %d!!!", 2, sink->streamBuffer.ShareBufferInfo.WriteOffset, buf_header->TimeStamp);
        N9Ble_SinkUpdateLocalWriteOffset(sink, 1);
    }
/*
    DSP_MW_LOG_I("[BLE][sink] SinkBufferWriteN9Ble sink local offset:%X", 1, sink->streamBuffer.ShareBufferInfo.WriteOffset);
*/
    return TRUE;
}

/**
 * SinkCloseN9Ble
 *
 * Function to shutdown BLE sink.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
static BOOL SinkCloseN9Ble(SINK sink)
{
    sink->param.n9ble.frame_length = 0;
    sink->param.n9ble.process_number = 0;
    sink->param.n9ble.share_info_base_addr = NULL;
#ifdef AIR_SOFTWARE_GAIN_ENABLE
    Call_UL_SW_Gain_Deinit();
#endif

#if defined(AIR_UL_FIX_SAMPLING_RATE_48K) && defined(AIR_FIXED_RATIO_SRC)
    N9Ble_UL_Fix_Sample_Rate_Deinit();
#endif

    return TRUE;
}

/**
 * SinkInitN9Ble
 *
 * Function to initialize BLE sink.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
VOID SinkInitN9Ble(SINK sink)
{
    N9Ble_Default_setting_init(NULL, sink);

    sink->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    N9Ble_Tx_Buffer_Init(sink);

    sink->sif.SinkSlack       = SinkSlackN9Ble;
    sink->sif.SinkClaim       = SinkClaimN9Ble;
    sink->sif.SinkMap         = SinkMapN9Ble;
    sink->sif.SinkFlush       = SinkFlushN9Ble;
    sink->sif.SinkClose       = SinkCloseN9Ble;
    sink->sif.SinkWriteBuf    = SinkBufferWriteN9Ble;

    sink->param.n9ble.IsFirstIRQ = TRUE;
}

/**
 * SourceSizeN9Ble
 *
 * Function to report remaining Source buffer avail size.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM static U32 SourceSizeN9Ble(SOURCE source)
{
    U32 writeOffset, readOffset, length, ProcessFrameLen, RemainLen;

    N9BleRx_update_from_share_information(source);

    writeOffset = source->streamBuffer.ShareBufferInfo.WriteOffset;
    readOffset  = source->streamBuffer.ShareBufferInfo.ReadOffset;
    length      = source->streamBuffer.ShareBufferInfo.length;
    ProcessFrameLen = source->param.n9ble.process_number * ALIGN_4(source->param.n9ble.frame_length + BLE_AVM_FRAME_HEADER_SIZE);
    RemainLen = (readOffset > writeOffset) ? (length - readOffset + writeOffset) : (writeOffset - readOffset);
    if (source->streamBuffer.ShareBufferInfo.bBufferIsFull == true) {
        RemainLen = length;
    }

    //if ((source->param.n9ble.write_offset_advance == 0) && (RemainLen < ProcessFrameLen)) {
    //    DSP_MW_LOG_E("[BLE][source] AVM buffer detect underflow!!! RO: %d, WO: %d", 2, readOffset, writeOffset);
    //}
    ProcessFrameLen = (source->param.n9ble.dual_cis_status != DUAL_CIS_DISABLED) ? source->param.n9ble.frame_length * source->param.n9ble.process_number * 2 : source->param.n9ble.frame_length * source->param.n9ble.process_number;
    return (source->param.n9ble.dl_afe_skip_time) ? 0 : ProcessFrameLen;

}

/**
 * SourceMapN9Ble
 *
 * Function to  read the received data in BLE source.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
static U8 *SourceMapN9Ble(SOURCE source)
{
    UNUSED(source);

    DSP_MW_LOG_E("[BLE][source] SourceMapN9Ble called!!!", 0);
    configASSERT(0);

    return NULL;
}

/**
 * SourceDropN9Ble
 *
 * Function to drop the data in BLE sink.
 *
 * param :amount - The amount of data to drop in sink.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 static VOID SourceDropN9Ble(SOURCE source, U32 amount)
{
    U16 i;
    U32 ProcessFrameLen;

    N9BleRx_update_from_share_information(source);

    ProcessFrameLen = (source->param.n9ble.frame_length * source->param.n9ble.process_number)<<(source->param.n9ble.dual_cis_status != DUAL_CIS_DISABLED);
    if (amount != ProcessFrameLen) {
        DSP_MW_LOG_E("[BLE][source] SourceMapN9Ble drop size mismatch %d, %d!!!", 2, amount, ProcessFrameLen);
        return;
    } else {
        for (i = 0 ; i < N9BLE_setting->N9Ble_source.Process_Frame_Num; i++)
        {
            LE_AUDIO_HEADER *buf_header;
            buf_header = (LE_AUDIO_HEADER *)(source->streamBuffer.ShareBufferInfo.startaddr + source->streamBuffer.ShareBufferInfo.ReadOffset);
            N9Ble_update_timestamp_share_information(&buf_header->TimeStamp, BLE_AVM_INVALID_TIMESTAMP); /* invalid the timestamp */
            if (source->param.n9ble.dual_cis_status != DUAL_CIS_DISABLED)
            {
                buf_header = (LE_AUDIO_HEADER *)((U32)source->param.n9ble.sub_share_info_base_addr + source->streamBuffer.ShareBufferInfo.ReadOffset + source->param.n9ble.dual_cis_buffer_offset);
                N9Ble_update_timestamp_share_information(&buf_header->TimeStamp, BLE_AVM_INVALID_TIMESTAMP); /* invalid the timestamp */
            }
            N9Ble_SourceUpdateLocalReadOffset(source, 1);
        }
        //printf("[BLE] source drop, update ro to:%d", source->streamBuffer.ShareBufferInfo.ReadOffset);
        N9Ble_update_readoffset_share_information(source, source->streamBuffer.ShareBufferInfo.ReadOffset);
    }
    source->param.n9ble.predict_frame_counter++;
    if (source->param.n9ble.predict_frame_counter == source->param.n9ble.frame_per_iso)
    {
        source->param.n9ble.predict_frame_counter = 0;
        source->param.n9ble.predict_timestamp += ((source->param.n9ble.iso_interval<<1)/LE_TIME_BT_CLOCK);
    }
    source->param.n9ble.write_offset_advance = 0;
    if (source->param.n9ble.seq_miss_cnt > LE_AUDIO_OFFSET_PROTECT)
    {
        LE_AUDIO_HEADER *buf_header;
        buf_header = (LE_AUDIO_HEADER *)(source->streamBuffer.ShareBufferInfo.startaddr + source->streamBuffer.ShareBufferInfo.ReadOffset);
        DSP_MW_LOG_E("[BLE][source] drop detect timestamp mismatch trigger offset re-sync,next TS : %d", 1,buf_header->TimeStamp);

        if (buf_header->TimeStamp <= source->param.n9ble.predict_timestamp)
        {
            N9Ble_SourceUpdateLocalReadOffset(source, (source->param.n9ble.predict_timestamp - buf_header->TimeStamp)/((source->param.n9ble.frame_interval<<1)/LE_TIME_BT_CLOCK));
            DSP_MW_LOG_E("[BLE][source] offset adjust: %d", 1,(source->param.n9ble.predict_timestamp - buf_header->TimeStamp)/((source->param.n9ble.frame_interval<<1)/LE_TIME_BT_CLOCK));
            source->param.n9ble.seq_miss_cnt = 0;
        }
        else if (buf_header->TimeStamp != BLE_AVM_INVALID_TIMESTAMP && buf_header->_reserved_byte_0Bh != 0x00)
        {
            N9Ble_SourceUpdateLocalReadOffset(source, 1);
        }
        N9Ble_update_readoffset_share_information(source, source->streamBuffer.ShareBufferInfo.ReadOffset);
    }
    source->param.n9ble.seq_num += source->param.n9ble.process_number;
    U32 mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (source->param.n9ble.skip_after_drop == TRUE)
    {
        source->param.n9ble.dl_afe_skip_time += 2;
        DSP_MW_LOG_E("[BLE] SOURCE skip after drop",0);
        source->param.n9ble.skip_after_drop = FALSE;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

/**
 * SourceConfigureN9Ble
 *
 * Function to configure BLE source.
 *
 * param :type - The configure type.
 *
 * param :value - The configure value.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
static BOOL SourceConfigureN9Ble(SOURCE source, stream_config_type type, U32 value)
{
    switch (type) {
        case SCO_SOURCE_WO_ADVANCE:
            source->param.n9ble.write_offset_advance = value;
            break;
        default:
            DSP_MW_LOG_E("[BLE][source] SourceConfigureN9Ble call with error type %d, value %d!!!", 2, type, value);
            configASSERT(0);
            return FALSE;
    }

    return TRUE;
}

/**
 * SourceReadBufN9Ble
 *
 * Function to read data from BLE source.
 *
 * param :dst_addr - The destination buffer to write data into.
 *
 * param :length -The leng of data to read.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
ATTR_TEXT_IN_IRAM_LEVEL_2 static BOOL SourceReadBufN9Ble(SOURCE source, U8 *dst_addr, U32 length)
{
    U16 i;
    U8 *read_ptr;
    U8 *write_ptr;
    U32 ProcessFrameLen;
    LE_AUDIO_HEADER *buf_header;

    ProcessFrameLen = (source->param.n9ble.frame_length * source->param.n9ble.process_number)<<(source->param.n9ble.dual_cis_status != DUAL_CIS_DISABLED); 
    if (ProcessFrameLen != length) {
        DSP_MW_LOG_E("[BLE][source] SourceReadBufN9Ble found size mismatch %d, %d!!!", 2, length, ProcessFrameLen);
        return FALSE;
    }

    N9BleRx_update_from_share_information(source);
    //Clock_Skew_Offset_Update_BLE(source);

    for (i = 0; i < source->param.n9ble.process_number; i++) {
        read_ptr = (U8 *)(source->streamBuffer.ShareBufferInfo.startaddr + source->streamBuffer.ShareBufferInfo.ReadOffset);     
        write_ptr = (source->param.n9ble.dual_cis_status == DUAL_CIS_DISABLED) ? dst_addr + i * source->param.n9ble.frame_length : dst_addr + (i * 2) * source->param.n9ble.frame_length;
        buf_header = (LE_AUDIO_HEADER *)read_ptr;
        //DSP_MW_LOG_I("[BLE][source] TimeStamp = 0x%08x, SampleSeq = %d, PduLen = %d", 3, buf_header->TimeStamp, buf_header->SampleSeq, buf_header->PduLen);
        /*
        DSP_MW_LOG_I("[BLE][source] TimeStamp = 0x%08x, RO = %d, WO = %d, EC = %d", 4,
            buf_header->TimeStamp,
            source->streamBuffer.ShareBufferInfo.ReadOffset,
            source->streamBuffer.ShareBufferInfo.WriteOffset,
            buf_header->ConnEvtCnt);
        */
        if (buf_header->TimeStamp != BLE_AVM_INVALID_TIMESTAMP && buf_header->_reserved_byte_0Bh != 0x00) {
            g_ble_pkt_lost[i][0] = false;
            memcpy(write_ptr, read_ptr + BLE_AVM_FRAME_HEADER_SIZE, source->param.n9ble.frame_length);
            if (source->param.n9ble.pkt_lost_state == TRUE)
            {
               DSP_MW_LOG_W("[BLE][source] Rx packet receive with sequence number %d", 1, source->param.n9ble.seq_num + i);
            }
            source->param.n9ble.pkt_lost_state = FALSE;
            if (((abs32((S32)(buf_header->TimeStamp - source->param.n9ble.predict_timestamp))*625)>>1) > source->param.n9ble.iso_interval)
            {
                DSP_MW_LOG_W("[BLE][source] ts info %d %d index:%d mis_cnt:%d", 4, buf_header->TimeStamp,source->param.n9ble.predict_timestamp,source->streamBuffer.ShareBufferInfo.ReadOffset,source->param.n9ble.seq_miss_cnt);
                source->param.n9ble.seq_miss_cnt++;
			}
            else
            {source->param.n9ble.seq_miss_cnt = 0;}
        } else {
            g_ble_pkt_lost[i][0] = true;
            memset(write_ptr, 0, source->param.n9ble.frame_length);
            if (source->param.n9ble.pkt_lost_state == FALSE)
            {
                DSP_MW_LOG_W("[BLE][source] Rx packet lost with sequence number %d", 1, source->param.n9ble.seq_num + i);
            }
            source->param.n9ble.pkt_lost_state = TRUE;
            g_pkt_lost_count++;
        }
        if (source->param.n9ble.dual_cis_status != DUAL_CIS_DISABLED)
        {
            read_ptr = (U8 *)((U32)source->param.n9ble.sub_share_info_base_addr + source->streamBuffer.ShareBufferInfo.ReadOffset + source->param.n9ble.dual_cis_buffer_offset);
            write_ptr = (source->param.n9ble.dual_cis_status == DUAL_CIS_DISABLED) ? dst_addr + i * source->param.n9ble.frame_length : dst_addr + (i * 2 + 1) * source->param.n9ble.frame_length;
            buf_header = (LE_AUDIO_HEADER *)read_ptr;
            if (buf_header->TimeStamp != BLE_AVM_INVALID_TIMESTAMP && buf_header->_reserved_byte_0Bh != 0x00) {
                g_ble_pkt_lost[i][1] = false;
                memcpy(write_ptr, read_ptr + BLE_AVM_FRAME_HEADER_SIZE, source->param.n9ble.frame_length);
                if (((abs32((S32)(buf_header->TimeStamp - source->param.n9ble.predict_timestamp))*625)>>1) > source->param.n9ble.iso_interval)
                {
                    DSP_MW_LOG_W("[BLE][source] sub ts info %d %d index:%d", 3, buf_header->TimeStamp,source->param.n9ble.predict_timestamp,source->streamBuffer.ShareBufferInfo.ReadOffset);
                }
            } else {
                g_ble_pkt_lost[i][1] = true;
                memset(write_ptr, 0, source->param.n9ble.frame_length);
                if (source->param.n9ble.pkt_lost_state == FALSE)
                {
                    DSP_MW_LOG_W("[BLE][source] Rx sub packet lost with sequence number %d", 1, source->param.n9ble.seq_num + i);
                }
            }
        }
        N9Ble_SourceUpdateLocalReadOffset(source, 1);
    }

    return TRUE;
}

bool ble_query_rx_packet_lost_status(uint32_t index)
{
    return g_ble_pkt_lost[index][0];
}
bool ble_query_rx_sub_cis_packet_lost_status(uint32_t index)
{
    return g_ble_pkt_lost[index][1];
}
/**
 * SourceCloseN9Ble
 *
 * Function to shutdown BLE source.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
static BOOL SourceCloseN9Ble(SOURCE source)
{
    source->param.n9ble.frame_length = 0;
    source->param.n9ble.process_number = 0;
    source->param.n9ble.share_info_base_addr = NULL;
    source->param.n9ble.seq_num = 0;

#if defined(AIR_BLE_SWB_ENABLE) && defined(AIR_FIXED_RATIO_SRC)
    N9Ble_DL_SWB_Sample_Rate_Deinit();
#endif

    DSP_MW_LOG_I("[BLE][source] total packet lost count %d", 1, g_pkt_lost_count);

    return TRUE;
}

/**
 * SourceInitBle
 *
 * Function to initialize BLE source.
 *
 * @Author : RyanHung <Ryan.Hung@airoha.com.tw>
 */
VOID SourceInitN9Ble(SOURCE source)
{
    N9Ble_Default_setting_init(source, NULL);

    source->buftype = BUFFER_TYPE_CIRCULAR_BUFFER;
    N9Ble_Rx_Buffer_Init(source);

    source->sif.SourceSize       = SourceSizeN9Ble;
    source->sif.SourceReadBuf    = SourceReadBufN9Ble;
    source->sif.SourceMap        = SourceMapN9Ble;
    source->sif.SourceConfigure  = SourceConfigureN9Ble;
    source->sif.SourceDrop       = SourceDropN9Ble;
    source->sif.SourceClose      = SourceCloseN9Ble;

    source->param.n9ble.IsFirstIRQ = TRUE;
    source->param.n9ble.write_offset_advance = 1; // force stream process to prevent first package lost
    source->param.n9ble.seq_num = 0;
    g_pkt_lost_count = 0;
}



////////////////////////////////////////////////////////////////////////////////
//              BLE Audio playback source related
////////////////////////////////////////////////////////////////////////////////

#define BLE_UL_MAX_NUM                  2

typedef struct
{
    uint32_t buffer_start[BLE_UL_MAX_NUM];
    uint32_t buffer_offset[BLE_UL_MAX_NUM];
    uint16_t buffer_index;
    uint16_t buffer_index_max;
    uint16_t buffer_size;
    uint16_t frame_head_len;
    uint16_t frame_data_len;
    uint8_t  is_source;
    uint8_t  is_playback_mode;
    uint8_t  is_1k_tone_mode;
    uint8_t  sample_rate;
    uint8_t  bitrate;
    uint8_t  num_of_sink;
    uint8_t  seq_num;
}LE_AUDIO_SOURCE_CTRL;

static LE_AUDIO_SOURCE_CTRL le_source;
static SHARE_BUFFER_INFO_PTR ul_info[BLE_UL_MAX_NUM];
static SHARE_BUFFER_INFO_PTR dl_info;

void CB_N9_BLE_UL_INFO(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);

    uint16_t index = (uint16_t)msg.ccni_message[0];

    le_source.is_source = TRUE; //this CCNI should only be used by SOURCE device
    le_source.is_playback_mode = TRUE;

    switch(index)
    {
        case 0x5A5A:
            le_source.is_playback_mode = FALSE;
            ul_info[0] = NULL;
            ul_info[1] = NULL;
            return;
        case 1:
            ul_info[0] = (msg.ccni_message[1] == 0)?NULL:(SHARE_BUFFER_INFO_PTR)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
            ul_info[1] = NULL;

            StreamDSP_HWSemaphoreTake();
            le_source.buffer_start[0]  = hal_memview_cm4_to_dsp0(ul_info[0]->startaddr);
            le_source.buffer_offset[0] = 0;
            StreamDSP_HWSemaphoreGive();

            DSP_MW_LOG_I("[le audio DSP] ul1 info = %X  ul1 buffer = %X", 2, ul_info[0],le_source.buffer_start[0]);
            break;
        case 2:
            ul_info[1] = (msg.ccni_message[1] == 0)?NULL:(SHARE_BUFFER_INFO_PTR)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);

            StreamDSP_HWSemaphoreTake();
            le_source.buffer_start[1]  = hal_memview_cm4_to_dsp0(ul_info[1]->startaddr);
            le_source.buffer_offset[1] = 0;
            StreamDSP_HWSemaphoreGive();

            DSP_MW_LOG_I("[le audio DSP] ul2 info = %X  ul2 buffer = %X", 2, ul_info[1],le_source.buffer_start[1]);
            break;
        default:
            DSP_MW_LOG_I("[le audio DSP] invalid UL info",0);
            break;
    }
}

void CB_N9_BLE_DL_INFO(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    UNUSED(ack);

    le_source.is_source = TRUE; //this CCNI should only be used by SOURCE device
    le_source.is_playback_mode = FALSE;

    dl_info = (msg.ccni_message[1] == 0)?NULL:(SHARE_BUFFER_INFO_PTR)hal_memview_cm4_to_dsp0(msg.ccni_message[1]);
    DSP_MW_LOG_I("[le audio DSP] dl buffer = %X", 1, dl_info);

    if(le_source.is_playback_mode)
        DSP_MW_LOG_I("[le audio DSP] Music mode (playback)",0);
    else
        DSP_MW_LOG_I("[le audio DSP] Voice mode (loopback)",0);
}

void CB_N9_BLE_UL_UPDATE_TIMESTAMP(hal_ccni_event_t event, void * msg)
{
    uint32_t i, bt_count, buffer_offset;
    hal_ccni_message_t *ccni_msg = msg;
    LE_AUDIO_HEADER *buf_header = NULL;
    uint8_t * avm_buffer;
    uint32_t saved_mask;
    uint32_t buf_index;
    UNUSED(event);
    UNUSED(msg);

    bt_count = ccni_msg->ccni_message[0];

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    buf_index = le_source.buffer_index;
    if ((le_source.is_source) && (le_source.is_playback_mode))
    {
        buffer_offset = (ALIGN_4(le_source.frame_head_len + le_source.frame_data_len))*le_source.buffer_index;

        for (i=0; i < BLE_UL_MAX_NUM; i++)
        {
            if (ul_info[i])
            {
                buf_header = (LE_AUDIO_HEADER *)(le_source.buffer_start[i] + buffer_offset);
                buf_header->TimeStamp = bt_count;
                buf_header->SampleSeq = le_source.seq_num;
            }
        }

        le_source.buffer_index = (le_source.buffer_index+1)%le_source.buffer_index_max;
        le_source.seq_num = (le_source.seq_num+1)&0xff;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[le audio DSP] irq update timestamp, 0x%x, 0x%x, 0x%x", 3, buf_header, buf_index, bt_count);
}

void CB_N9_BLE_UL_PLAYBACK_DATA_INFO(hal_ccni_message_t msg, hal_ccni_message_t *ack)
{
    uint32_t saved_mask;
    UNUSED(ack);

    if((uint16_t)msg.ccni_message[0] == 0xFFFF) {
        le_source.is_1k_tone_mode = !le_source.is_1k_tone_mode;
        DSP_MW_LOG_I("[le audio DSP] toggle playback source between line_in/1K tone",0);
    } else if((uint16_t)msg.ccni_message[0] == 0x9999) {
//        hal_audio_trigger_start_parameter_t sw_trigger_start;
//        sw_trigger_start.enable = true;
//        sw_trigger_start.memory_select = playback_vp_if.sink->param.audio.mem_handle.memory_select;
//        hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        DSP_MW_LOG_I("[le audio DSP] trigger line-in irq start",0);
    } else {
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        le_source.is_source = TRUE;
        le_source.is_playback_mode = TRUE;
        le_source.frame_head_len = (uint16_t)BLE_AVM_FRAME_HEADER_SIZE;
        le_source.frame_data_len = (uint16_t)msg.ccni_message[1];
        le_source.sample_rate    = (uint8_t)(msg.ccni_message[1]>>16);
        le_source.bitrate        = (uint8_t)(msg.ccni_message[1]>>24);
        le_source.num_of_sink    = (uint16_t)msg.ccni_message[0];
        le_source.buffer_size    = ALIGN_4(le_source.frame_data_len + le_source.frame_head_len)*BLE_AVM_FRAME_NUM(le_source.frame_data_len);
        le_source.buffer_index   = 0;
        le_source.buffer_index_max = BLE_AVM_FRAME_NUM(le_source.frame_data_len);
        le_source.seq_num = 0;

        #ifdef MTK_AUDIO_TRANSMITTER_ENABLE
        /* register ccni callback */
        extern void audio_transmitter_register_isr_handler(f_audio_transmitter_ccni_callback_t callback);
        audio_transmitter_register_isr_handler(CB_N9_BLE_UL_UPDATE_TIMESTAMP);
        #endif /* MTK_AUDIO_TRANSMITTER_ENABLE */

        hal_nvic_restore_interrupt_mask(saved_mask);

        DSP_MW_LOG_I("[le audio DSP] sink num: %d   frame_data_len:%d buffer_size:%d  sample_rate:%d bitrate:%d blocknum:%d", 6,
                    le_source.num_of_sink, le_source.frame_data_len, le_source.buffer_size, le_source.sample_rate, le_source.bitrate, le_source.buffer_index_max);
    }
}

bool STREAM_BLE_UL_CHECK_MODE(void)
{
    return le_source.is_1k_tone_mode;
}

bool STREAM_BLE_UL_GET_PARAM(uint32_t * sample_rate, uint32_t *bitrate)
{
    if(le_source.is_source && le_source.is_playback_mode)
    {
        if(sample_rate)
            *sample_rate = 1000*le_source.sample_rate;
        if(bitrate)
            *bitrate = 1000*le_source.bitrate;
        return TRUE;
    } else {
        return FALSE;
    }
}

bool STREAM_BLE_UL_CHECK_BUF(uint8_t index)
{
    if(le_source.is_source && le_source.is_playback_mode)
    {
        if(index < BLE_UL_MAX_NUM && ul_info[index])
            return TRUE;
    }
    return FALSE;
}

uint32_t STREAM_BLE_UL_GET_BUF_INDEX(void)
{
    return le_source.buffer_index;
}

void STREAM_BLE_UL_WRITE_BUF(uint8_t index, uint8_t * buf, uint32_t len, uint32_t buffer_index)
{
    uint32_t buffer_offset;

    if(len != le_source.frame_data_len)
        DSP_MW_LOG_I("[le audio DSP] STREAM_BLE_UL_WRITE_BUF len abnormal: %d ", 1, len);

    if(le_source.is_source && le_source.is_playback_mode && index < BLE_UL_MAX_NUM)
    {
        if(ul_info[index]) {

            buffer_offset = (ALIGN_4(le_source.frame_head_len + le_source.frame_data_len))*buffer_index;
            LE_AUDIO_HEADER *buf_header = (LE_AUDIO_HEADER *)(le_source.buffer_start[index] + buffer_offset);
            uint8_t * avm_buffer = (uint8_t *)buf_header + le_source.frame_head_len;

            if((uint32_t)avm_buffer & 0x80000000) {

                StreamDSP_HWSemaphoreTake();
                memcpy(avm_buffer, buf, len);
                buf_header->DataOffset = le_source.frame_head_len;
                buf_header->PduLen     = le_source.frame_data_len;
                StreamDSP_HWSemaphoreGive();

                // static uint32_t countttt = 0;
                // countttt++;

                // le_source.buffer_offset[index] += ALIGN_4(le_source.frame_head_len + le_source.frame_data_len);
                // if(countttt<50 && index==0)
                //     DSP_MW_LOG_I("[le audio DSP] 1.le_source.buffer_offset[index] : %d ", 1, le_source.buffer_offset[index] );
                // le_source.buffer_offset[index] %=  le_source.buffer_size;
                // if(countttt<50 && index==0)
                //     DSP_MW_LOG_I("[le audio DSP] 2.le_source.buffer_offset[index] : %d ", 1, le_source.buffer_offset[index] );
            } else {
                DSP_MW_LOG_I("[le audio DSP] UL_GET_BUF AVM abnormal    avm_buffer:%X ", 1, avm_buffer);
            }

        } else {
            DSP_MW_LOG_I("[le audio DSP] STREAM_BLE_UL_WRITE_BUF index invalid: %d",1, index);
        }
    }
}


bool N9_BLE_SOURCE_ROUTINE(void)
{
    if(le_source.is_source)
    {
//
//        static uint8_t count = 0;
//        if(count<20)
//        {
//            count++;
//            if(ul1_info)
//                DSP_MW_LOG_I("ul1_info: start %X read %X write %X next %X sample %X length%X", 6,ul1_info->startaddr,ul1_info->ReadOffset,ul1_info->WriteOffset,ul1_info->sub_info.next,ul1_info->sample_rate,ul1_info->length);
//            else
//                DSP_MW_LOG_I("ul1_info empty",0);
//            if(dl_info)
//                DSP_MW_LOG_I("dl_info: start %X read %X write %X next %X sample %X length%X", 6,dl_info->startaddr,dl_info->ReadOffset,dl_info->WriteOffset,dl_info->sub_info.next,dl_info->sample_rate,dl_info->length);
//            else
//                DSP_MW_LOG_I("dl_info empty",0);
//        }
//
//        if(le_source.is_playback_mode && le_source.is_stream_data_valid)
//        {
//            SHARE_BUFFER_INFO avm_info;
//
//
//            StreamDSP_HWSemaphoreTake();
//            memcpy((U32)&avm_info, (U32)ul1_info, 24);/* use first 24 bytes in info */
//            StreamDSP_HWSemaphoreGive();
//
//            avm_info.startaddr = hal_memview_cm4_to_dsp0(avm_info.startaddr);
//            avm_info.length = BLE_AVM_FRAME_NUM*(BLE_AVM_FRAME_HEADER_SIZE+le_source.frame_length);
//
//            if(le_source.num_of_sink >= 1)
//            {
//                memcpy((U32)(avm_info.startaddr + avm_info.WriteOffset + BLE_AVM_FRAME_HEADER_SIZE), (U32)(le_source.stream_L_addr+le_source.stream_offset),le_source.frame_length);
//                avm_info.WriteOffset = (avm_info.WriteOffset+le_source.frame_length+BLE_AVM_FRAME_HEADER_SIZE)%avm_info.length;
//            }
//            if(le_source.num_of_sink >= 2)
//            {
//                memcpy((U32)(avm_info.startaddr + avm_info.WriteOffset + BLE_AVM_FRAME_HEADER_SIZE), (U32)(le_source.stream_R_addr+le_source.stream_offset),le_source.frame_length);
//                avm_info.WriteOffset = (avm_info.WriteOffset+le_source.frame_length+BLE_AVM_FRAME_HEADER_SIZE)%avm_info.length;
//            }
//
//            le_source.stream_offset = (le_source.frame_length+le_source.stream_offset)%le_source.stream_size;
//
//            if(le_source.stream_offset == 0)
//            {
//                printf("stream_offset == 0. Repeat again");
//            }
//
//            StreamDSP_HWSemaphoreTake();
//            memcpy((U32)ul1_info, (U32)&avm_info, 24);/* use first 24 bytes in info */
//            StreamDSP_HWSemaphoreGive();
//        }
//
        return TRUE;
    }
    else// not source device
    {
        return FALSE;
    }
}

#endif

