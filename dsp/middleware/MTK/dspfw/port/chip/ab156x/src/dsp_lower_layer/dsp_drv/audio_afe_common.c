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

#include <stdio.h>
#include <string.h>
#include "sink.h"
#include "source.h"
#include "common.h"
#include "dsp_buffer.h"
#include "stream_audio_driver.h"
#include "hal_audio_afe_control.h"
#include "hal_audio_afe_define.h"
#include "audio_afe_common.h"
#include "clk_skew.h"
#include "ext_clk_skew.h"
#include "stream_audio_setting.h"
#include "stream_n9sco.h"
#include "dsp_callback.h"
#include "FreeRTOS.h"
#include "dsp_drv_afe.h"
#include "dsp_audio_msg.h"
#include "bt_interface.h"
#ifdef MTK_ANC_ENABLE
#include "anc_api.h"
#endif
#ifdef AIR_BT_CODEC_BLE_ENABLED
#include "stream_n9ble.h"
#endif

#include "sfr_bt.h"
#include "hal_pdma_internal.h"
#include "dsp_scenario.h"
#include "dsp_gain_control.h"
#include "hal_audio_volume.h"
#include "bt_interface.h"

#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
#define HWSRC_UNDERRUN_DETECT
#endif

#ifdef HWSRC_UNDERRUN_DETECT
U32 hwsrc_out_remain = 0;
#endif
extern afe_sram_manager_t audio_sram_manager;

#define HW_SYSRAM_PRIVATE_MEMORY_CCNI_START_ADDR *(U8*)0x8423FC00
#define AFE_OFFSET_PROTECT (16+256)

const afe_stream_channel_t connect_type[2][2] = { // [Stream][AFE]
    {STREAM_M_AFE_M, STREAM_M_AFE_S} ,
    {STREAM_S_AFE_M, STREAM_S_AFE_S}
};

#define WriteREG(_addr, _value) (*(volatile uint32_t *)(_addr) = (_value))
#define ReadREG(_addr)          (*(volatile uint32_t *)(_addr))
static volatile uint32_t AFE_CLASSG_MON0_b29 = 0;

#define ASRC_CLCOK_SKEW_DEBUG (false)

extern bool hal_src_set_start(afe_asrc_id_t src_id, bool enable);

ATTR_TEXT_IN_IRAM_LEVEL_2 BOOL chk_dl1_cur(uint32_t cur_addr)
{
#if 0
    if((AFE_GET_REG(AFE_AUDIO_BT_SYNC_CON0) & afe_get_bt_sync_enable_bit(AUDIO_DIGITAL_BLOCK_MEM_DL1)) //use sync mode
       && (afe_get_bt_sync_monitor_state(AUDIO_DIGITAL_BLOCK_MEM_DL1) == 0) //play en not start yet
       && ((cur_addr < AFE_GET_REG(AFE_DL1_BASE)) || (cur_addr > AFE_GET_REG(AFE_DL1_END)))) //cur addr isn't correct
#else
    //Keep flase before play en start
    if((AFE_GET_REG(AFE_AUDIO_BT_SYNC_CON0) & afe_get_bt_sync_enable_bit(AUDIO_DIGITAL_BLOCK_MEM_DL1)) //use sync mode
       && (afe_get_bt_sync_monitor_state(AUDIO_DIGITAL_BLOCK_MEM_DL1) == 0)) //play en not start yet
#endif
    {
        DSP_MW_LOG_W("chk_dl1_cur addr=0x%08x, base=0x%08x, end=0x%08x\n", 3,cur_addr,AFE_GET_REG(AFE_DL1_BASE),AFE_GET_REG(AFE_DL1_END));
        return false;
    }

    return true;
}

void vRegSetBit(uint32_t addr, uint32_t bit)
{
    uint32_t u4CurrValue, u4Mask;
    u4Mask = 1 << bit;
    u4CurrValue = ReadREG(addr);
    WriteREG(addr, (u4CurrValue | u4Mask));
    return;
}

void vRegResetBit(uint32_t addr, uint32_t bit)
{
    uint32_t u4CurrValue, u4Mask;
    u4Mask = 1 << bit;
    u4CurrValue = ReadREG(addr);
    WriteREG(addr, (u4CurrValue & (~u4Mask)));
    return;
}
#if 0
static void audio_split_source_to_interleaved(SOURCE source, uint8_t *afe_src, uint32_t pos, uint32_t size)
{
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    uint8_t *dst_ch0 = NULL, *dst_ch1 = NULL;
    afe_stream_channel_t channel_type = source->param.audio.connect_channel_type;
    uint32_t i, copysize, offset;

    if (size == 0) {
        return ;
    }
    if (channel_type == STREAM_S_AFE_S) {
        pos = pos>>1;
    }
    offset = (buffer_info->WriteOffset + pos) % (buffer_info->length);
    dst_ch0 = (uint8_t *)(buffer_info->startaddr[0] + offset);

    if (channel_type == STREAM_S_AFE_S) {
        copysize = size >> 1;
        dst_ch1 = (uint8_t *)(buffer_info->startaddr[1] + offset);
        for (i=0; i< copysize>>1; i++) {
            *(dst_ch0++) = *(afe_src++);
            *(dst_ch0++) = *(afe_src++);
            *(dst_ch1++) = *(afe_src++);
            *(dst_ch1++) = *(afe_src++);
            if (dst_ch0 == (uint8_t *)(buffer_info->startaddr[0] + buffer_info->length)){
                dst_ch0 = (uint8_t *)(buffer_info->startaddr[0]);
                dst_ch1 = (uint8_t *)(buffer_info->startaddr[1]);
            }
        }
    }else {
        DSP_D2C_BufferCopy(dst_ch0, afe_src, size, buffer_info->startaddr[0], buffer_info->length);
        if (channel_type == STREAM_S_AFE_M) {    // even if the stream is stereo, the SRAM updates 1 ch
            dst_ch1 = (uint8_t *)(buffer_info->startaddr[1] + offset);
            DSP_D2C_BufferCopy(dst_ch1, afe_src, size, buffer_info->startaddr[1], buffer_info->length);
        }
    }
}

static void audio_split_echo_to_interleaved(SOURCE source, uint8_t *afe_src, uint32_t pos, uint32_t size)
{
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    uint8_t *dst_ch2 = NULL;
    afe_stream_channel_t channel_type = source->param.audio.connect_channel_type;
    uint32_t i, copysize, offset;

    if (size == 0) {
        return ;
    }
    if (channel_type == STREAM_S_AFE_S) {
        pos = pos>>1;
    }
    offset = (buffer_info->WriteOffset + pos) % (buffer_info->length);
    dst_ch2 = (uint8_t *)(buffer_info->startaddr[2] + offset);

    if (channel_type == STREAM_S_AFE_S) {
        copysize = size >> 1;
        for (i=0; i< copysize>>1; i++) {
            *(dst_ch2++) = *(afe_src++);
            *(dst_ch2++) = *(afe_src++);
            afe_src++;
            afe_src++;
            if (dst_ch2 == (uint8_t *)(buffer_info->startaddr[2] + buffer_info->length)){
                dst_ch2 = (uint8_t *)(buffer_info->startaddr[2]);
            }
        }
    } else {
        DSP_D2C_BufferCopy(dst_ch2, afe_src, size, buffer_info->startaddr[2], buffer_info->length);
    }
}

static void audio_split_sink_to_interleaved(SINK sink, uint8_t *afe_dst, uint32_t pos, uint32_t size)
{
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    uint8_t *src_ch0 = NULL, *src_ch1 = NULL, *p_dst;
    afe_stream_channel_t channel_type = sink->param.audio.connect_channel_type;
    uint32_t i, offset, copysize = size;
    p_dst = afe_dst;

    if (size == 0) {
        return ;
    }
    if (channel_type == STREAM_S_AFE_S) {
        pos = pos>>1;
    }
    offset = (buffer_info->ReadOffset + pos) % (buffer_info->length);
    src_ch0 = (uint8_t *)(buffer_info->startaddr[0] + offset);

    if (channel_type == STREAM_S_AFE_S) {
        src_ch1 = (uint8_t *)(buffer_info->startaddr[1] + offset);
        copysize = copysize >> 1;  //2 channels
        for (i = 0; i < copysize >> 1; i++) {  //16-bit
            *(p_dst++) = *(src_ch0++);
            *(p_dst++) = *(src_ch0++);
            *(p_dst++) = *(src_ch1++);
            *(p_dst++) = *(src_ch1++);

            if (src_ch0 == (uint8_t *)(buffer_info->startaddr[0] + buffer_info->length)) { //wrap
                src_ch0 = (uint8_t *)(buffer_info->startaddr[0]);
                src_ch1 = (uint8_t *)(buffer_info->startaddr[1]);
            }
        }
    } else {// mono
        DSP_C2D_BufferCopy(p_dst, src_ch0, size, buffer_info->startaddr[0], buffer_info->length);
    }
}

static void audio_afe_source_deinterleaved(SOURCE source, uint32_t size)
{
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    uint8_t *src_afe, *dst_ch0, *dst_ch1, *dst_echo;
    uint32_t i, j, sample, src_offset, dst_offset;
    uint32_t rpt, wpt, toggle=0, channel;

    if (size == 0) {
        return ;
    }

    dst_offset = (buffer_info->WriteOffset) % (buffer_info->length);
    src_offset= (source->param.audio.AfeBlkControl.u4ReadIdx) % source->param.audio.AfeBlkControl.u4BufferSize;

    src_afe = (uint8_t *)source->param.audio.AfeBlkControl.phys_buffer_addr;
    dst_ch0 = buffer_info->startaddr[0];
    dst_ch1 = buffer_info->startaddr[1];
    dst_echo = buffer_info->startaddr[2];

    //format_bytes
    sample = size/source->param.audio.format_bytes;

    //Echo path
    if (source->param.audio.echo_reference == true) {
        sample = sample + (sample>>1);
        toggle = 0;
    } else {
        toggle = 1;
    }
    channel = source->param.audio.channel_num+1;

    for (i=0 ; i<sample ; i++) {
        for (j=0 ; j<source->param.audio.format_bytes ; j++ ) {
            wpt = (dst_offset + j) % buffer_info->length;
            rpt = (src_offset + j) % source->param.audio.AfeBlkControl.u4BufferSize;

            if ((toggle==0) && (source->param.audio.echo_reference == true) && (dst_echo!=NULL)) {
                *(dst_echo+wpt) = *(src_afe+rpt+source->param.audio.AfeBlkControl.u4BufferSize);
            } else if ((toggle==1) && (dst_ch0!=NULL)) {
                *(dst_ch0+wpt) = *(src_afe+rpt);
            } else if ((toggle==2)&& (dst_ch1!=NULL)) {
                *(dst_ch1+wpt) = *(src_afe+rpt);
            }
        }

        if (toggle != 0) {
            src_offset = ( src_offset + source->param.audio.format_bytes)%source->param.audio.AfeBlkControl.u4BufferSize;
        }
        toggle++;
        if (channel == toggle) {
            dst_offset = (dst_offset + source->param.audio.format_bytes) % buffer_info->length;
            toggle = (source->param.audio.echo_reference == true) ? 0 : 1;
        }
    }
}


static void audio_afe_sink_interleaved(SINK sink, uint32_t size)
{
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    uint8_t *src_ch0, *src_ch1, *dst_afe;
    uint32_t i, j, sample, src_offset, dst_offset;
    uint32_t rpt, wpt, toggle=0;

    if (size == 0) {
        return ;
    }

    src_offset = (buffer_info->ReadOffset) % (buffer_info->length);
    dst_offset = (sink->param.audio.AfeBlkControl.u4WriteIdx) % sink->param.audio.AfeBlkControl.u4BufferSize;

    src_ch0 = buffer_info->startaddr[0];
    src_ch1 = buffer_info->startaddr[1];
    //modify for asrc
    /*dst_afe = (sink->param.audio.AfeBlkControl.u4asrcflag)
                ? (uint8_t *)(sink->param.audio.AfeBlkControl.phys_buffer_addr+sink->param.audio.buffer_size)
                : (uint8_t *)(sink->param.audio.AfeBlkControl.phys_buffer_addr);*/
#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
    dst_afe = (sink->param.audio.AfeBlkControl.u4asrcflag)
                ? (uint8_t *)(sink->param.audio.AfeBlkControl.phys_buffer_addr+sink->param.audio.AfeBlkControl.u4asrc_buffer_size)
                : (uint8_t *)(sink->param.audio.AfeBlkControl.phys_buffer_addr);
#else
    dst_afe = (sink->param.audio.AfeBlkControl.u4asrcflag)
            ? (uint8_t *)(sink->param.audio.AfeBlkControl.phys_buffer_addr+sink->param.audio.buffer_size)
            : (uint8_t *)(sink->param.audio.AfeBlkControl.phys_buffer_addr);
#endif //ENABLE_HWSRC_ON_MAIN_STREAM

    //format_bytes
    sample = size/sink->param.audio.format_bytes;

    for (i=0 ; i<sample ; i++) {
        for (j=0 ; j<sink->param.audio.format_bytes ; j++ ) {
            wpt = (dst_offset + j) % sink->param.audio.AfeBlkControl.u4BufferSize;
            rpt = (src_offset + j) % buffer_info->length;

            if ((sink->param.audio.channel_num>=2) && (toggle == 1) && (src_ch1 != NULL)) {
                *(dst_afe+wpt) = *(src_ch1+rpt);
            } else {
                *(dst_afe+wpt) = *(src_ch0+rpt);
            }
        }

        dst_offset = (dst_offset + sink->param.audio.format_bytes)%sink->param.audio.AfeBlkControl.u4BufferSize;
        toggle ^= 1;
        if ((sink->param.audio.channel_num<2) || (toggle==0)) {
            src_offset = (src_offset + sink->param.audio.format_bytes) % buffer_info->length;
        }

    }
}
#endif

/**/
/////////////////////////////////////////////////////////////////////////////////////////////
bool audio_ops_distinguish_audio_sink(void *param)
{
    bool is_au_sink = FALSE;
#ifdef MTK_TDM_ENABLE
    if ((param == Sink_blks[SINK_TYPE_VP_AUDIO]) || (param == Sink_blks[SINK_TYPE_AUDIO]) || (param == Sink_blks[SINK_TYPE_AUDIO_DL3]) || (param == Sink_blks[SINK_TYPE_AUDIO_DL12]) || (param == Sink_blks[SINK_TYPE_TDMAUDIO])) {
#else
    if ((param == Sink_blks[SINK_TYPE_VP_AUDIO]) || (param == Sink_blks[SINK_TYPE_AUDIO]) || (param == Sink_blks[SINK_TYPE_AUDIO_DL3]) || (param == Sink_blks[SINK_TYPE_AUDIO_DL12])) {
#endif
        is_au_sink = TRUE;
    }
    return is_au_sink;
}
bool audio_ops_distinguish_audio_source(void *param)
{
    SOURCE source_ptr = (SOURCE)param;
    bool is_au_source = FALSE;
    if ((source_ptr) &&
#ifdef MTK_TDM_ENABLE
        ((source_ptr->type == SOURCE_TYPE_AUDIO) || (source_ptr->type == SOURCE_TYPE_TDMAUDIO) ||
#else
#ifdef AIR_I2S_SLAVE_ENABLE
        ((source_ptr->type == SOURCE_TYPE_AUDIO)|| (source_ptr->type == SOURCE_TYPE_AUDIO2) ||
#else
        ((source_ptr->type == SOURCE_TYPE_AUDIO)||
#endif
#endif
#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
         ((source_ptr->type>=SOURCE_TYPE_SUBAUDIO_MIN) && (source_ptr->type<=SOURCE_TYPE_SUBAUDIO_MAX)))){
#else
         (0))){
#endif
        is_au_source = TRUE;
    }
    return is_au_source;
}


int32_t audio_ops_probe(void *param)
{
    int ret = -1;
    if (param == NULL) {
        DSP_MW_LOG_E("DSP audio ops parametser invalid\r\n", 0);
        return ret;
    }
    if (audio_ops_distinguish_audio_sink(param)) {
        SINK sink = param;
        if (sink->param.audio.ops->probe != NULL) {
            sink->param.audio.ops->probe(param);
            ret = 0;
        }
    } else if (audio_ops_distinguish_audio_source(param)){
        SOURCE source = param;
        if (source->param.audio.ops->probe != NULL){
            source->param.audio.ops->probe(param);
            ret = 0;
        }
    }
    return ret;
}

int32_t audio_ops_hw_params(void *param)
{
    int ret = -1;
    if (param == NULL) {
        DSP_MW_LOG_E("DSP audio ops parametser invalid\r\n", 0);
        return ret;
    }
    if (audio_ops_distinguish_audio_sink(param)) {
        SINK sink = param;
        if (sink->param.audio.ops->hw_params != NULL) {
            sink->param.audio.ops->hw_params(param);
            ret = 0;
        }
    } else if (audio_ops_distinguish_audio_source(param)){
        SOURCE source = param;
        if (source->param.audio.ops->hw_params != NULL) {
            source->param.audio.ops->hw_params(param);
            ret = 0;
        }
    }
    return ret;
}

int32_t audio_ops_open(void *param)
{
    int ret = -1;
    if (param == NULL) {
        DSP_MW_LOG_E("DSP audio ops parametser invalid\r\n", 0);
        return ret;
    }
    if (audio_ops_distinguish_audio_sink(param)) {
        SINK sink = param;
        if (sink->param.audio.ops->open != NULL) {
            sink->param.audio.ops->open(param);
            ret = 0;
        }
    } else if (audio_ops_distinguish_audio_source(param)){
        SOURCE source = param;
        if (source->param.audio.ops->open != NULL) {
            source->param.audio.ops->open(param);
            ret = 0;
        }
    }
    return ret;
}

bool audio_ops_close(void *param)
{
    int ret = false;
    if (param == NULL) {
        DSP_MW_LOG_E("DSP audio ops parametser invalid\r\n", 0);
        return ret;
    }
    if (audio_ops_distinguish_audio_sink(param)) {
        SINK sink = param;
        if (sink->param.audio.ops->close != NULL) {
            sink->param.audio.ops->close(param);
            ret = true;
        }
    } else if (audio_ops_distinguish_audio_source(param)){
        SOURCE source = param;
        if (source->param.audio.ops->close != NULL) {
            source->param.audio.ops->close(param);
            ret = true;
        }
    }
    return ret;
}

int32_t audio_ops_trigger(void *param, int cmd)
{
    int ret = -1;
    if (param == NULL) {
        DSP_MW_LOG_E("DSP audio ops parametser invalid\r\n", 0);
        return ret;
    }
        if (audio_ops_distinguish_audio_sink(param) == TRUE) {
        SINK sink = param;
        if (sink->param.audio.ops->trigger != NULL) {
            sink->param.audio.ops->trigger(param, cmd);
            ret = 0;
        }
    } else if (audio_ops_distinguish_audio_source(param) == TRUE) {
        SOURCE source = param;
        if (source->param.audio.ops->trigger != NULL) {
            source->param.audio.ops->trigger(param, cmd);
            ret = 0;
        }
    }
    return ret;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 bool audio_ops_copy(void *param, void *src, uint32_t count)
{
    int ret = false;
    if (param == NULL) {
        DSP_MW_LOG_E("DSP audio ops parametser invalid\r\n", 0);
        return ret;
    }
    if (audio_ops_distinguish_audio_sink(param)) {
        SINK sink = param;
        if (sink->param.audio.ops->copy != NULL) {
            sink->param.audio.ops->copy(param, src, count);
            ret = true;
        }
    } else if (audio_ops_distinguish_audio_source(param)){
        SOURCE source = param;
        if (source->param.audio.ops->copy != NULL) {
            source->param.audio.ops->copy(param, src, count);
            ret = true;
        }
    }
    return ret;
}

extern audio_sink_pcm_ops_t afe_platform_dl1_ops;
#ifdef MTK_PROMPT_SOUND_ENABLE
extern audio_sink_pcm_ops_t afe_platform_dl2_ops;
#endif
extern audio_source_pcm_ops_t afe_platform_ul1_ops;
#if defined(AIR_I2S_SLAVE_ENABLE)
extern audio_sink_pcm_ops_t i2s_slave_dl_ops;
extern audio_source_pcm_ops_t i2s_slave_ul_ops;
#endif

void audio_afe_set_ops(void *param)
{
    if (param == NULL) {
        DSP_MW_LOG_E("DSP audio ops parametser invalid\r\n", 0);
        return;
    }
    if (audio_ops_distinguish_audio_sink(param)) {
        SINK sink = param;
#if defined(AIR_I2S_SLAVE_ENABLE)
        if (sink->param.audio.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
            sink->param.audio.ops = &i2s_slave_dl_ops;
            return;
        }
#endif
        sink->param.audio.ops = (audio_pcm_ops_p)&afe_platform_dl1_ops;
    } else if (audio_ops_distinguish_audio_source(param)) {
        SOURCE source = param;
#if defined(AIR_I2S_SLAVE_ENABLE)
        if (source->param.audio.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
            source->param.audio.ops = &i2s_slave_ul_ops;
            return;
        }
#endif
        source->param.audio.ops = (audio_pcm_ops_p)&afe_platform_ul1_ops;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////
void afe_sink_prefill_silence_data(SINK sink)
{
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;

    if (afe_block->u4BufferSize <= buffer_info->length) {
        afe_block->u4BufferSize =  buffer_info->length;
    }

    afe_block->u4DataRemained = ((buffer_info->WriteOffset >= buffer_info->ReadOffset)
                                    ? (buffer_info->WriteOffset - buffer_info->ReadOffset)
                                    :(buffer_info->length - buffer_info->ReadOffset + buffer_info->WriteOffset));

    if (sink->param.audio.channel_num == 2) {
        afe_block->u4DataRemained <<= 1;
    }

    afe_block->u4WriteIdx += afe_block->u4DataRemained;
    afe_block->u4WriteIdx %= afe_block->u4BufferSize;
}

void afe_source_prefill_silence_data(SOURCE source)
{
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;

    if (afe_block->u4BufferSize <= buffer_info->length) {
        afe_block->u4BufferSize =  buffer_info->length;
    }

    if (source->param.audio.channel_num == 2) {
        afe_block->u4ReadIdx = ( buffer_info->ReadOffset << 1) % afe_block->u4BufferSize;
    }
    else {
        afe_block->u4ReadIdx = buffer_info->ReadOffset % afe_block->u4BufferSize;
    }
}

/*
 * Get dl1 afe and sink buffer
 * Units: sample
*/
ATTR_TEXT_IN_IRAM_LEVEL_2 uint32_t afe_get_dl1_query_data_amount(void)
{
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    uint32_t afe_sram_data_count, sink_data_count;

    if (sink == NULL) {
        return 0;
    }
    //AFE DL1 SRAM data amount
    #if 0
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    int32_t hw_current_read_idx = AFE_GET_REG(AFE_DL1_CUR);
    afe_block->u4ReadIdx = hw_current_read_idx - AFE_GET_REG(AFE_DL1_BASE);
    if (afe_block->u4WriteIdx > afe_block->u4ReadIdx) {
        *afe_sram_data_count = afe_block->u4WriteIdx - afe_block->u4ReadIdx;
    } else {
        *afe_sram_data_count = afe_block->u4BufferSize + afe_block->u4WriteIdx - afe_block->u4ReadIdx;
    }
    #else
    afe_sram_data_count = 0;
    #endif
    //Sink audio data amount
    U32 buffer_per_channel_shift = ((sink->param.audio.channel_num>=2) && (sink->buftype == BUFFER_TYPE_INTERLEAVED_BUFFER ))
                                     ? 1
                                     : 0;
    sink_data_count = (sink->streamBuffer.BufferInfo.length>>buffer_per_channel_shift) - SinkSlack(sink);

    if((sink_data_count > (AFE_OFFSET_PROTECT*sink->param.audio.format_bytes)) && (sink->param.audio.AfeBlkControl.u4asrcflag == true))
    {
        sink_data_count += (sink->param.audio.AfeBlkControl.u4asrc_buffer_size>>buffer_per_channel_shift);
    }

    return ((afe_sram_data_count+sink_data_count)/sink->param.audio.format_bytes);
}

uint32_t i2s_slave_port_translate(hal_audio_interface_t audio_interface)
{
    uint32_t port;

    if (audio_interface == HAL_AUDIO_INTERFACE_1) {
        port = 0;
    } else if (audio_interface == HAL_AUDIO_INTERFACE_2) {
        port = 1;
    } else if (audio_interface == HAL_AUDIO_INTERFACE_3) {
        port = 2;
    } else {
        port = 3;
    }

    return port;
}

const vdma_channel_t g_i2s_slave_vdma_channel_infra[] = {
    VDMA_I2S3TX, VDMA_I2S3RX,//I2S0 DMA TX(VDMA7),  I2S0 DMA RX(VDMA8)
    VDMA_I2S0TX, VDMA_I2S0RX,//I2S1 DMA TX(VDMA1),  I2S1 DMA RX(VDMA2)
    VDMA_I2S4TX, VDMA_I2S4RX,//I2S2 DMA TX(VDMA9),  I2S2 DMA RX(VDMA10)
};

#ifdef MTK_TDM_ENABLE
const vdma_channel_t g_i2s_slave_vdma_channel_tdm[] = {
    VDMA_I2S0TX, VDMA_I2S0RX,//I2S1 2CH TX(VDMA1),  I2S1 2CH RX(VDMA2)
    VDMA_I2S1TX, VDMA_I2S1RX,//I2S1 4CH TX(VDMA3),  I2S1 4CH RX(VDMA4)
    VDMA_I2S2TX, VDMA_I2S2RX,//I2S1 6CH TX(VDMA5),  I2S1 6CH RX(VDMA6)
    VDMA_I2S3TX, VDMA_I2S3RX,//I2S1 8CH TX(VDMA7),  I2S1 8CH RX(VDMA8)
    VDMA_I2S4TX, VDMA_I2S4RX,//I2S2 2CH TX(VDMA9),  I2S2 2CH RX(VDMA10)
    VDMA_I2S5TX, VDMA_I2S5RX,//I2S2 4CH TX(VDMA11), I2S2 4CH RX(VDMA12)
    VDMA_I2S6TX, VDMA_I2S6RX,//I2S2 6CH TX(VDMA13), I2S2 6CH RX(VDMA14)
    VDMA_I2S7TX, VDMA_I2S7RX,//I2S2 8CH TX(VDMA15), I2S2 8CH RX(VDMA16)
};
#endif

void i2s_slave_ul_update_rptr(vdma_channel_t rx_dma_channel, U32 amount)
{
    vdma_set_sw_move_byte(rx_dma_channel, amount);
}

void i2s_slave_dl_update_wptr(vdma_channel_t tx_dma_channel, U32 amount)
{
    vdma_set_sw_move_byte(tx_dma_channel, amount);
}

ATTR_TEXT_IN_IRAM void i2s_slave_ul_interrupt_handler(void)
{
    uint32_t mask, port, hw_current_write_idx;
    SOURCE source = Source_blks[SOURCE_TYPE_AUDIO];
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    vdma_channel_t rx_dma_channel;
    uint32_t update_frame_size = Audio_setting->Audio_source.Frame_Size * source->param.audio.channel_num * source->param.audio.format_bytes;//unit:bytes
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    afe_block_t *afe_block = &Source_blks[SOURCE_TYPE_AUDIO]->param.audio.AfeBlkControl;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    port = i2s_slave_port_translate(audio_interface);
    rx_dma_channel = g_i2s_slave_vdma_channel_infra[port * 2 + 1];

    // Get last WPTR and record current WPTR
    vdma_get_hw_write_point(rx_dma_channel, &hw_current_write_idx);
    if (afe_block->u4asrcflag) {
    } else {
        buffer_info->WriteOffset = hw_current_write_idx - afe_block->phys_buffer_addr;
    }

    vdma_disable_interrupt(rx_dma_channel);
    i2s_slave_ul_update_rptr(rx_dma_channel, update_frame_size*4);
    vdma_enable_interrupt(rx_dma_channel);

    AudioCheckTransformHandle(source->transform);
    hal_nvic_restore_interrupt_mask(mask);
}

#ifdef AIR_I2S_SLAVE_ENABLE
ATTR_TEXT_IN_IRAM void i2s_slave_0_ul_interrupt_handler(vdma_event_t event, void  *user_data)
{
    //DSP_MW_LOG_I("i2s_slave_0_ul_interrupt_handler",0);
    uint32_t mask, port, hw_current_write_idx,hw_current_read_idx;
    SOURCE source = Source_blks[SOURCE_TYPE_AUDIO];
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    vdma_channel_t rx_dma_channel;
    uint32_t update_frame_size = Audio_setting->Audio_source.Frame_Size * source->param.audio.channel_num * source->param.audio.format_bytes;//unit:bytes
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    afe_block_t *afe_block = &Source_blks[SOURCE_TYPE_AUDIO]->param.audio.AfeBlkControl;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    port = i2s_slave_port_translate(audio_interface);
    rx_dma_channel = g_i2s_slave_vdma_channel_infra[port * 2 + 1];

    // Get last WPTR and record current WPTR
    vdma_get_hw_write_point(rx_dma_channel, &hw_current_write_idx);
    if (afe_block->u4asrcflag) {
        if (AFE_GET_REG(ASM_CH01_OBUF_RDPNT) == AFE_GET_REG(ASM_CH01_OBUF_WRPNT)) {
            printf("asrc out buffer RPTR=WPTR, R=0x%x, W=0x%x,     asrc in buffer, R=0x%x, W=0x%x",AFE_GET_REG(ASM_CH01_OBUF_RDPNT),AFE_GET_REG(ASM_CH01_OBUF_WRPNT),AFE_GET_REG(ASM_CH01_IBUF_RDPNT),AFE_GET_REG(ASM_CH01_IBUF_WRPNT));
            source->streamBuffer.BufferInfo.bBufferIsFull = TRUE;
        }
        #if 0
        hw_current_read_idx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        update_frame_size = (hw_current_read_idx + afe_block->u4BufferSize - afe_block->u4ReadIdx)%afe_block->u4BufferSize;
        afe_block->u4ReadIdx = hw_current_read_idx;
        #else
        afe_block->u4ReadIdx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        //DSP_MW_LOG_I("[HAS][HWSRC2] read point = 0x%x",1,afe_block->u4ReadIdx);
        vdma_get_hw_read_point(rx_dma_channel, &hw_current_read_idx);
        //update_frame_size = (hw_current_write_idx + afe_block->u4asrc_buffer_size - hw_current_read_idx - 8)%afe_block->u4asrc_buffer_size;
        update_frame_size = afe_block->u4asrc_buffer_size/2;
        #endif

        afe_block->u4WriteIdx = hw_current_write_idx - afe_block->phys_buffer_addr;
        AFE_SET_REG(ASM_CH01_IBUF_WRPNT, hw_current_write_idx<<ASM_CH01_IBUF_WRPNT_POS, ASM_CH01_IBUF_WRPNT_MASK);

        buffer_info->WriteOffset = AFE_GET_REG(ASM_CH01_OBUF_WRPNT) - AFE_GET_REG(ASM_OBUF_SADR);
    } else {
        buffer_info->WriteOffset = hw_current_write_idx - afe_block->phys_buffer_addr;
    }

    vdma_disable_interrupt(rx_dma_channel);
    i2s_slave_ul_update_rptr(rx_dma_channel, update_frame_size*4);
    vdma_enable_interrupt(rx_dma_channel);

    AudioCheckTransformHandle(source->transform);
    hal_nvic_restore_interrupt_mask(mask);
    // DSP_MW_LOG_I("[HWSRC Rx Tracking]:ASM2_CH01_CONF = 0x%x,ASM2_FREQ_CALI_CTRL = 0x%x", 2, AFE_GET_REG(ASM_CH01_CNFG),AFE_GET_REG(ASM_FREQ_CALI_CTRL));
    // DSP_MW_LOG_I("[HWSRC Rx Tracking]:ASM2_FREQUENCY_0 = 0x%x,ASM2_FREQUENCY_2 = 0x%x",2,AFE_GET_REG(ASM_FREQUENCY_0),AFE_GET_REG(ASM_FREQUENCY_2));

    // DSP_MW_LOG_I("[HWSRC Rx Tracking]:ASM2_GEN_CONF = 0x%x,ASM2_IIR_CRAM_ADDR = 0x%x", 2, AFE_GET_REG(ASM_GEN_CONF),AFE_GET_REG(ASM_IIR_CRAM_ADDR));
    // DSP_MW_LOG_I("[HWSRC Rx Tracking]:ASM2_IER = 0x%x,ASM2_IFR = 0x%x",2,AFE_GET_REG(ASM_IER),AFE_GET_REG(ASM_IFR));

    // DSP_MW_LOG_I("[HWSRC Rx Tracking]:ASM2_FREQ_CALI_CYC = 0x%x,ASM2_FREQ_CALI_CTRL = 0x%x", 2, AFE_GET_REG(ASM_FREQ_CALI_CYC),AFE_GET_REG(ASM_FREQ_CALI_CTRL));
    // DSP_MW_LOG_I("[HWSRC Rx Tracking]:ASM2_FREQ_CALI_CTRL = 0x%x,ASM2_GEN_CONF = 0x%x",2,AFE_GET_REG(ASM_FREQ_CALI_CTRL),AFE_GET_REG(ASM_GEN_CONF));
}

ATTR_TEXT_IN_IRAM void i2s_slave_1_ul_interrupt_handler(vdma_event_t event, void  *user_data)
{
    //DSP_MW_LOG_I("i2s_slave_1_ul_interrupt_handler",0);
    uint32_t mask, port, hw_current_write_idx,hw_current_read_idx;
    SOURCE source = Source_blks[SOURCE_TYPE_AUDIO];
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    vdma_channel_t rx_dma_channel;
    uint32_t update_frame_size = Audio_setting->Audio_source.Frame_Size * source->param.audio.channel_num * source->param.audio.format_bytes;//unit:bytes
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    afe_block_t *afe_block = &Source_blks[SOURCE_TYPE_AUDIO]->param.audio.AfeBlkControl;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    port = i2s_slave_port_translate(audio_interface);
    rx_dma_channel = g_i2s_slave_vdma_channel_infra[port * 2 + 1];

    // Get last WPTR and record current WPTR
    vdma_get_hw_write_point(rx_dma_channel, &hw_current_write_idx);
    if (afe_block->u4asrcflag) {
        if (AFE_GET_REG(ASM_CH01_OBUF_RDPNT) == AFE_GET_REG(ASM_CH01_OBUF_WRPNT)) {
            printf("asrc out buffer RPTR=WPTR, R=0x%x, W=0x%x,     asrc in buffer, R=0x%x, W=0x%x",AFE_GET_REG(ASM_CH01_OBUF_RDPNT),AFE_GET_REG(ASM_CH01_OBUF_WRPNT),AFE_GET_REG(ASM_CH01_IBUF_RDPNT),AFE_GET_REG(ASM_CH01_IBUF_WRPNT));
            source->streamBuffer.BufferInfo.bBufferIsFull = TRUE;
        }
        #if 0
        hw_current_read_idx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        update_frame_size = (hw_current_read_idx + afe_block->u4BufferSize - afe_block->u4ReadIdx)%afe_block->u4BufferSize;
        afe_block->u4ReadIdx = hw_current_read_idx;
        #else
        afe_block->u4ReadIdx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        //DSP_MW_LOG_I("[HAS][HWSRC2] read point = 0x%x",1,afe_block->u4ReadIdx);
        vdma_get_hw_read_point(rx_dma_channel, &hw_current_read_idx);
        //update_frame_size = (hw_current_write_idx + afe_block->u4asrc_buffer_size - hw_current_read_idx - 8)%afe_block->u4asrc_buffer_size;
        update_frame_size = afe_block->u4asrc_buffer_size/2;
        #endif

        afe_block->u4WriteIdx = hw_current_write_idx - afe_block->phys_buffer_addr;
        AFE_SET_REG(ASM_CH01_IBUF_WRPNT, hw_current_write_idx<<ASM_CH01_IBUF_WRPNT_POS, ASM_CH01_IBUF_WRPNT_MASK);

        buffer_info->WriteOffset = AFE_GET_REG(ASM_CH01_OBUF_WRPNT) - AFE_GET_REG(ASM_OBUF_SADR);
    } else {
        buffer_info->WriteOffset = hw_current_write_idx - afe_block->phys_buffer_addr;
    }
    //DSP_MW_LOG_I("1111######sona VDMA2_PGMADDR=0x%x, WRPTR=0x%x, RRPTR=0x%x, COUNT=%d, FFSIZE=%d, FFCNT=%d",ReadREG(0xC900022C),ReadREG(0xC9000230),ReadREG(0xC9000234),ReadREG(0xC9000210),ReadREG(0xC9000244),ReadREG(0xC9000238));
    vdma_disable_interrupt(rx_dma_channel);
    i2s_slave_ul_update_rptr(rx_dma_channel, update_frame_size*4);
    vdma_enable_interrupt(rx_dma_channel);

    AudioCheckTransformHandle(source->transform);
    hal_nvic_restore_interrupt_mask(mask);
    //DSP_MW_LOG_I("2222######sona VDMA2_PGMADDR=0x%x, WRPTR=0x%x, RRPTR=0x%x, COUNT=%d, FFSIZE=%d, FFCNT=%d",ReadREG(0xC900022C),ReadREG(0xC9000230),ReadREG(0xC9000234),ReadREG(0xC9000210),ReadREG(0xC9000244),ReadREG(0xC9000238));
}

ATTR_TEXT_IN_IRAM void i2s_slave_2_ul_interrupt_handler(vdma_event_t event, void  *user_data)
{
    //DSP_MW_LOG_I("i2s_slave_2_ul_interrupt_handler",0);
    uint32_t mask, port, hw_current_write_idx, hw_current_read_idx;
    SOURCE source = Source_blks[SOURCE_TYPE_AUDIO2];
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    vdma_channel_t rx_dma_channel;
    uint32_t update_frame_size = Audio_setting->Audio_source.Frame_Size * source->param.audio.channel_num * source->param.audio.format_bytes;//unit:bytes
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    afe_block_t *afe_block = &Source_blks[SOURCE_TYPE_AUDIO2]->param.audio.AfeBlkControl;
    source->param.audio.mute_flag = FALSE;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    port = i2s_slave_port_translate(audio_interface);
    rx_dma_channel = g_i2s_slave_vdma_channel_infra[port * 2 + 1];

    // Get last WPTR and record current WPTR
    vdma_get_hw_write_point(rx_dma_channel, &hw_current_write_idx);
    if (afe_block->u4asrcflag) {
        if (AFE_GET_REG(ASM2_CH01_OBUF_RDPNT) == AFE_GET_REG(ASM2_CH01_OBUF_WRPNT)) {
            printf("asrc2 out buffer RPTR=WPTR, R=0x%x, W=0x%x,     asrc in buffer, R=0x%x, W=0x%x",AFE_GET_REG(ASM2_CH01_OBUF_RDPNT),AFE_GET_REG(ASM2_CH01_OBUF_WRPNT),AFE_GET_REG(ASM2_CH01_IBUF_RDPNT),AFE_GET_REG(ASM2_CH01_IBUF_WRPNT));
            source->streamBuffer.BufferInfo.bBufferIsFull = TRUE;
        }
        #if 0
        hw_current_read_idx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        update_frame_size = (hw_current_read_idx + afe_block->u4BufferSize - afe_block->u4ReadIdx)%afe_block->u4BufferSize;
        afe_block->u4ReadIdx = hw_current_read_idx;
        #else
        afe_block->u4ReadIdx = AFE_GET_REG(ASM2_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM2_IBUF_SADR);
        //DSP_MW_LOG_I("[HAS][HWSRC2] read point = 0x%x",1,afe_block->u4ReadIdx);
        vdma_get_hw_read_point(rx_dma_channel, &hw_current_read_idx);
        //update_frame_size = (hw_current_write_idx + afe_block->u4asrc_buffer_size - hw_current_read_idx - 8)%afe_block->u4asrc_buffer_size;
        update_frame_size = afe_block->u4asrc_buffer_size/2;
        #endif

        afe_block->u4WriteIdx = hw_current_write_idx - afe_block->phys_buffer_addr;
        AFE_SET_REG(ASM2_CH01_IBUF_WRPNT, hw_current_write_idx<<ASM2_CH01_IBUF_WRPNT_POS, ASM2_CH01_IBUF_WRPNT_MASK);

        buffer_info->WriteOffset = AFE_GET_REG(ASM2_CH01_OBUF_WRPNT) - AFE_GET_REG(ASM2_OBUF_SADR);
        //DSP_MW_LOG_I("[HAS][HWSRC2] write offset = 0x%x",1,buffer_info->WriteOffset);
    } else {
        //DSP_MW_LOG_I("[HAS] enter else u4asrcflag",0);
        buffer_info->WriteOffset = hw_current_write_idx - afe_block->phys_buffer_addr;
    }
    //DSP_MW_LOG_I("1111######sona VDMA10_PGMADDR=0x%x, WRPTR=0x%x, RRPTR=0x%x, COUNT=%d, FFSIZE=%d, FFCNT=%d",ReadREG(0xC9000a2C),ReadREG(0xC9000a30),ReadREG(0xC9000a34),ReadREG(0xC9000a10),ReadREG(0xC9000a44),ReadREG(0xC9000a38));
    vdma_disable_interrupt(rx_dma_channel);
    i2s_slave_ul_update_rptr(rx_dma_channel, update_frame_size*4);
    vdma_enable_interrupt(rx_dma_channel);

    AudioCheckTransformHandle(source->transform);
    hal_nvic_restore_interrupt_mask(mask);
    //DSP_MW_LOG_I("2222######sona VDMA10_PGMADDR=0x%x, WRPTR=0x%x, RRPTR=0x%x, COUNT=%d, FFSIZE=%d, FFCNT=%d",ReadREG(0xC9000a2C),ReadREG(0xC9000a30),ReadREG(0xC9000a34),ReadREG(0xC9000a10),ReadREG(0xC9000a44),ReadREG(0xC9000a38));
}
#endif

ATTR_TEXT_IN_IRAM void i2s_slave_dl_interrupt_handler(void)
{
    uint32_t mask, port, hw_current_read_idx;
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    hal_audio_interface_t audio_interface = sink->param.audio.audio_interface;
    vdma_channel_t tx_dma_channel;
    uint32_t update_frame_size = Audio_setting->Audio_source.Frame_Size * sink->param.audio.channel_num * sink->param.audio.format_bytes;//unit:bytes
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    uint32_t dl_base_addr = (uint32_t)buffer_info->startaddr[0];
    uint32_t pre_offset, isr_interval;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    port = i2s_slave_port_translate(audio_interface);
    tx_dma_channel = g_i2s_slave_vdma_channel_infra[port * 2];

    /* Get last RPTR and record current RPTR */
    pre_offset = buffer_info->ReadOffset;
    vdma_get_hw_read_point(tx_dma_channel, &hw_current_read_idx);

    buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
    isr_interval = (pre_offset <= buffer_info->ReadOffset)
                     ? (buffer_info->ReadOffset - pre_offset)
                     : (buffer_info->length + buffer_info->ReadOffset - pre_offset);

    if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }
    /* Check whether underflow happen */
    if  (OFFSET_OVERFLOW_CHK(pre_offset,(buffer_info->ReadOffset)%buffer_info->length, buffer_info->WriteOffset )&&(buffer_info->bBufferIsFull == FALSE)) {
        DSP_MW_LOG_I("SRAM Empty play en:%d pR:%d R:%d W:%d",4,isr_interval, pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);
        buffer_info->WriteOffset = (buffer_info->ReadOffset + 2*isr_interval)%buffer_info->length;
        if (buffer_info->WriteOffset > buffer_info->ReadOffset){
            memset((void *)(dl_base_addr + buffer_info->ReadOffset), 0, buffer_info->WriteOffset - buffer_info->ReadOffset);
        } else {
            memset((void *)(dl_base_addr+ buffer_info->ReadOffset), 0, buffer_info->length - buffer_info->ReadOffset);
            memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);
        }
    } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }

    vdma_disable_interrupt(tx_dma_channel);
    i2s_slave_dl_update_wptr(tx_dma_channel, update_frame_size*4);
    vdma_enable_interrupt(tx_dma_channel);

    AudioCheckTransformHandle(sink->transform);
    hal_nvic_restore_interrupt_mask(mask);
}

#ifdef MTK_TDM_ENABLE
ATTR_TEXT_IN_IRAM void i2s_slave_ul_tdm_interrupt_handler(void)
{
    uint32_t mask, port, hw_current_write_idx;
    SOURCE source = Source_blks[SOURCE_TYPE_TDMAUDIO];
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    hal_audio_i2s_tdm_channel_setting_t tdm_channel = source->param.audio.device_handle.i2s_slave.tdm_channel;
    vdma_channel_t rx_dma_channel, dma_set_ch0, dma_set_ch1, dma_set_ch2, dma_set_ch3;
    uint8_t channel_num     = (source->param.audio.channel_num>=2) ? 2 : 1;
    uint32_t update_frame_size = Audio_setting->Audio_source.Frame_Size * channel_num * source->param.audio.format_bytes;//unit:bytes
    uint32_t setting_cnt, dma_setting_count;

    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    afe_block_t *afe_block = &Source_blks[SOURCE_TYPE_TDMAUDIO]->param.audio.AfeBlkControl;

    uint32_t volatile dma_int;
    dma_int = I2S_DMA_RG_GLB_STA;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    if (audio_interface == HAL_AUDIO_INTERFACE_2) {
        port = 0;
    } else if (audio_interface == HAL_AUDIO_INTERFACE_3) {
        port = 1;
    }

    dma_set_ch0 = g_i2s_slave_vdma_channel_tdm[port * 8 + 1];

    if (tdm_channel >= HAL_AUDIO_I2S_TDM_4CH) {
        dma_set_ch1 = g_i2s_slave_vdma_channel_tdm[port * 8 + 1 + 2];
        dma_setting_count = 2;
    }
    if (tdm_channel >= HAL_AUDIO_I2S_TDM_6CH) {
        dma_set_ch2 = g_i2s_slave_vdma_channel_tdm[port * 8 + 1 + 4];
        dma_setting_count = 3;
    }
    if (tdm_channel >= HAL_AUDIO_I2S_TDM_8CH) {
        dma_set_ch3 = g_i2s_slave_vdma_channel_tdm[port * 8 + 1 + 6];
        dma_setting_count = 4;
    }

    rx_dma_channel = dma_set_ch0;

    // Get last WPTR and record current WPTR
    vdma_get_hw_write_point(rx_dma_channel, &hw_current_write_idx);
    if (afe_block->u4asrcflag) {
    } else {
        buffer_info->WriteOffset = hw_current_write_idx - afe_block->phys_buffer_addr;
    }

    for (setting_cnt=0; setting_cnt < dma_setting_count; setting_cnt++) {
        if (setting_cnt==0) {
            rx_dma_channel = dma_set_ch0;
        } else if (setting_cnt==1) {
            rx_dma_channel = dma_set_ch1;
        } else if (setting_cnt==2) {
            rx_dma_channel = dma_set_ch2;
        } else {
            rx_dma_channel = dma_set_ch3;
        }
        vdma_disable_interrupt(rx_dma_channel);
        i2s_slave_ul_update_rptr(rx_dma_channel, update_frame_size*4);
        vdma_enable_interrupt(rx_dma_channel);
    }
    AudioCheckTransformHandle(source->transform);
    hal_nvic_restore_interrupt_mask(mask);
}

ATTR_TEXT_IN_IRAM void i2s_slave_dl_tdm_interrupt_handler(void)
{
    uint32_t mask, port, hw_current_read_idx;
    volatile SINK sink = Sink_blks[SINK_TYPE_TDMAUDIO];
    hal_audio_interface_t audio_interface = sink->param.audio.audio_interface;
    hal_audio_i2s_tdm_channel_setting_t tdm_channel = sink->param.audio.device_handle.i2s_slave.tdm_channel;
    vdma_channel_t tx_dma_channel, dma_set_ch0, dma_set_ch1, dma_set_ch2, dma_set_ch3;
    uint8_t channel_num     = (sink->param.audio.channel_num>=2) ? 2 : 1;
    uint32_t update_frame_size = Audio_setting->Audio_source.Frame_Size * channel_num * sink->param.audio.format_bytes;//unit:bytes
    uint32_t setting_cnt, dma_setting_count;

    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    uint32_t dl_base_addr = (uint32_t)buffer_info->startaddr[0];
    uint32_t pre_offset, isr_interval;

    uint32_t volatile dma_int;
    dma_int = I2S_DMA_RG_GLB_STA;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    if (audio_interface == HAL_AUDIO_INTERFACE_2) {
        port = 0;
    } else if (audio_interface == HAL_AUDIO_INTERFACE_3) {
        port = 1;
    }

    dma_set_ch0 = g_i2s_slave_vdma_channel_tdm[port * 8];

    if (tdm_channel >= HAL_AUDIO_I2S_TDM_4CH) {
        dma_set_ch1 = g_i2s_slave_vdma_channel_tdm[port * 8 + 2];
        dma_setting_count = 2;
    }
    if (tdm_channel >= HAL_AUDIO_I2S_TDM_6CH) {
        dma_set_ch2 = g_i2s_slave_vdma_channel_tdm[port * 8 + 4];
        dma_setting_count = 3;
    }
    if (tdm_channel >= HAL_AUDIO_I2S_TDM_8CH) {
        dma_set_ch3 = g_i2s_slave_vdma_channel_tdm[port * 8 + 6];
        dma_setting_count = 4;
    }

    tx_dma_channel = dma_set_ch0;

    /* Get last RPTR and record current RPTR */
    pre_offset = buffer_info->ReadOffset;
    vdma_get_hw_read_point(tx_dma_channel, &hw_current_read_idx);

    buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
    isr_interval = (pre_offset <= buffer_info->ReadOffset)
                     ? (buffer_info->ReadOffset - pre_offset)
                     : (buffer_info->length + buffer_info->ReadOffset - pre_offset);

    if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }

    /* Check whether underflow happen */
    if  (OFFSET_OVERFLOW_CHK(pre_offset,(buffer_info->ReadOffset)%buffer_info->length, buffer_info->WriteOffset )&&(buffer_info->bBufferIsFull == FALSE)) {
        DSP_MW_LOG_I("SRAM Empty play en:%d pR:%d R:%d W:%d",4,isr_interval, pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);
        buffer_info->WriteOffset = (buffer_info->ReadOffset + 2*isr_interval)%buffer_info->length;
        if (buffer_info->WriteOffset > buffer_info->ReadOffset){
            memset((void *)(dl_base_addr + buffer_info->ReadOffset), 0, buffer_info->WriteOffset - buffer_info->ReadOffset);
        } else {
            memset((void *)(dl_base_addr+ buffer_info->ReadOffset), 0, buffer_info->length - buffer_info->ReadOffset);
            memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);
        }
    } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }

    for (setting_cnt=0; setting_cnt < dma_setting_count; setting_cnt++) {
        if (setting_cnt==0) {
            tx_dma_channel = dma_set_ch0;
        } else if (setting_cnt==1) {
            tx_dma_channel = dma_set_ch1;
        } else if (setting_cnt==2) {
            tx_dma_channel = dma_set_ch2;
        } else {
            tx_dma_channel = dma_set_ch3;
        }
        vdma_disable_interrupt(tx_dma_channel);
        i2s_slave_dl_update_wptr(tx_dma_channel, update_frame_size*4);
        vdma_enable_interrupt(tx_dma_channel);
    }
    AudioCheckTransformHandle(sink->transform);
    hal_nvic_restore_interrupt_mask(mask);
}
#endif
#if 0
#if defined(AIR_I2S_SLAVE_ENABLE)
/***************************************************
               I2 slave TX / RX IRQ handler
***************************************************/
#include "hal_pdma_internal.h"
extern const vdma_channel_t g_i2s_slave_vdma_channel[];
extern uint32_t i2s_slave_port_translate(hal_audio_interface_t audio_interface);

void i2s_slave_ul_interrupt_handler(vdma_event_t event, void  *user_data)
{
    uint32_t mask;
    uint32_t port;
    vdma_channel_t rx_dma_channel;
    uint32_t hw_current_write_idx, hw_current_read_idx;
    afe_block_t *afe_block = &Source_blks[SOURCE_TYPE_AUDIO]->param.audio.AfeBlkControl;
    SOURCE source = Source_blks[SOURCE_TYPE_AUDIO];
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
    uint32_t update_frame_size = source->param.audio.frame_size * source->param.audio.channel_num;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    port = i2s_slave_port_translate(audio_interface);
    rx_dma_channel = g_i2s_slave_vdma_channel[port * 2 + 1];

    // Get last WPTR and record current WPTR
    vdma_get_hw_write_point(rx_dma_channel, &hw_current_write_idx);
    if (afe_block->u4asrcflag) {
        #if 0
        hw_current_read_idx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        update_frame_size = (hw_current_read_idx + afe_block->u4BufferSize - afe_block->u4ReadIdx)%afe_block->u4BufferSize;
        afe_block->u4ReadIdx = hw_current_read_idx;
        #else
        afe_block->u4ReadIdx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        vdma_get_hw_read_point(rx_dma_channel, &hw_current_read_idx);
        update_frame_size = (hw_current_write_idx + afe_block->u4asrc_buffer_size - hw_current_read_idx - 8)%afe_block->u4asrc_buffer_size;
        #endif

        afe_block->u4WriteIdx = hw_current_write_idx - afe_block->phys_buffer_addr;
        AFE_SET_REG(ASM_CH01_IBUF_WRPNT, hw_current_write_idx<<ASM_CH01_IBUF_WRPNT_POS, ASM_CH01_IBUF_WRPNT_MASK);

        buffer_info->WriteOffset = AFE_GET_REG(ASM_CH01_OBUF_WRPNT) - AFE_GET_REG(ASM_OBUF_SADR);

        /*
        if(OFFSET_OVERFLOW_CHK(afe_block->u4ReadIdx, hw_current_read_idx-afe_block->phys_buffer_addr, afe_block->u4WriteIdx)) {
            log_hal_error("DSP I2D UL with SRC wrap. SRC_WR:0x%x, SRC_RD:0x%x, I2S_RD:0x%x", afe_block->u4WriteIdx, afe_block->u4ReadIdx, hw_current_read_idx-afe_block->phys_buffer_addr);
        }
        */

    } else {
        buffer_info->WriteOffset = hw_current_write_idx - afe_block->phys_buffer_addr;
    }

    vdma_disable_interrupt(rx_dma_channel);
    i2s_slave_ul_update_rptr(source, update_frame_size*4);
    vdma_enable_interrupt(rx_dma_channel);

    AudioCheckTransformHandle(source->transform);

    hal_nvic_restore_interrupt_mask(mask);
}

void i2s_slave_dl_interrupt_handler(vdma_event_t event, void  *user_data)
{
    int16_t cp_samples = 0;
    uint32_t hw_current_read_idx;
    uint32_t mask,pre_offset,isr_interval;
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    AUDIO_PARAMETER *runtime = &sink->param.audio;
    uint32_t dl_base_addr = buffer_info->startaddr[0];
    vdma_channel_t tx_dma_channel;
    uint32_t port;
    hal_audio_interface_t audio_interface = sink->param.audio.audio_interface;

    hal_nvic_save_and_set_interrupt_mask(&mask);

    port = i2s_slave_port_translate(audio_interface);
    tx_dma_channel = g_i2s_slave_vdma_channel[port * 2];

    /* clock skew adjustment */
    cp_samples = clk_skew_check_dl_status();
    vdma_set_threshold(tx_dma_channel, (runtime->count + cp_samples) * 2 * 32);

    /* Get last RPTR and record current RPTR */
    pre_offset = buffer_info->ReadOffset;
    vdma_get_hw_read_point(tx_dma_channel, &hw_current_read_idx);
    buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
    isr_interval = (pre_offset <= buffer_info->ReadOffset)
                     ? (buffer_info->ReadOffset - pre_offset)
                     : (buffer_info->length + buffer_info->ReadOffset - pre_offset);

    if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }

    /* Check whether underflow happen */
    if  (OFFSET_OVERFLOW_CHK(pre_offset,(buffer_info->ReadOffset)%buffer_info->length, buffer_info->WriteOffset )&&(buffer_info->bBufferIsFull == FALSE)) {
        DSP_MW_LOG_I("SRAM Empty play en:%d pR:%d R:%d W:%d",4,isr_interval, pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);
        buffer_info->WriteOffset = (buffer_info->ReadOffset + 2*isr_interval)%buffer_info->length;
        if (buffer_info->WriteOffset > buffer_info->ReadOffset){
            memset((void *)(dl_base_addr + buffer_info->ReadOffset), 0, buffer_info->WriteOffset - buffer_info->ReadOffset);
        } else {
            memset((void *)(dl_base_addr+ buffer_info->ReadOffset), 0, buffer_info->length - buffer_info->ReadOffset);
            memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);
        }
    } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }

    AudioCheckTransformHandle(sink->transform);

    hal_nvic_restore_interrupt_mask(mask);
}
#endif
#endif
int32_t dl_irq_cnt = 0;
#ifdef MTK_BT_HFP_ENABLE
extern bool g_ignore_next_drop_flag;
#endif
#ifdef MTK_GAMING_MODE_HEADSET
extern AIROHA_GAMING_CTRL gA2dpGamingCtrl;
#endif
ATTR_TEXT_IN_IRAM void afe_dl1_interrupt_handler(void)
{
    uint32_t mask,pre_offset,isr_interval ;
    uint32_t hw_current_read_idx = 0;
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    volatile SOURCE source;
    BTCLK bt_clk;
    BTPHASE bt_phase;
    int16_t cp_samples_ext = 0;
    SOURCE ul_source = Source_blks[SOURCE_TYPE_AUDIO];

    if ((sink == NULL) || (sink->transform == NULL) || (sink->transform->source == NULL)) {
        return;
    }

    if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_N9SCO)){
        if(ul_source->param.audio.mem_handle.pure_agent_with_src){
            MCE_GetBtClk(&bt_clk, &bt_phase,BT_CLK_Offset);
            clk_skew_isr_time_update(bt_clk,bt_phase);
            S32 isr_time_samples = clk_skew_isr_time_get_dl_cp_samples();
            if(isr_time_samples){
                cp_samples_ext = isr_time_samples;
                clk_skew_isr_time_set_dl_cp_samples(0);
            }
        }
    }

#ifdef MTK_GAMING_MODE_HEADSET
    if(sink->transform->source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT){
        MCE_GetBtClk(&bt_clk, &bt_phase,BT_CLK_Offset);
        gA2dpGamingCtrl.AfeAnchor = bt_clk;
        gA2dpGamingCtrl.AfeAnchorIntra = bt_phase;
    }
#endif

#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
#endif
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    hw_current_read_idx = word_size_align(AFE_GET_REG(AFE_DL1_CUR));
    AUDIO_PARAMETER *runtime = &sink->param.audio;
    int16_t cp_samples = 0;
    uint32_t dl_base_addr = (uint32_t)buffer_info->startaddr[0];
    uint32_t pre_AFE_CLASSG_MON0_b29 = 0;
    uint32_t src_out_data = 0;

#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
    if (afe_block->u4asrcflag) {
        dl_base_addr = AFE_GET_REG(ASM_IBUF_SADR);
        hw_current_read_idx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT);
        uint32_t ibuf_addr,iwo,iro,obuf_addr,owo,oro,osize,isize;
        ibuf_addr = AFE_GET_REG(ASM_IBUF_SADR);
        obuf_addr = AFE_GET_REG(ASM_OBUF_SADR);
        iro = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        iwo = AFE_GET_REG(ASM_CH01_IBUF_WRPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        owo = AFE_GET_REG(ASM_CH01_OBUF_WRPNT) - AFE_GET_REG(ASM_OBUF_SADR);
        oro = AFE_GET_REG(ASM_CH01_OBUF_RDPNT) - AFE_GET_REG(ASM_OBUF_SADR);
        osize = AFE_GET_REG(ASM_OBUF_SIZE);
        isize = AFE_GET_REG(ASM_IBUF_SIZE);
        src_out_data = (owo >= oro) ?  (owo - oro) : (owo + osize - oro);
        //DSP_MW_LOG_I("[afe_irq] asrc in,wo=%d,ro=%d,len=%d,out,wo=%d,ro=%d,len=%d\r\n", 6,iwo ,iro,isize,owo,oro,osize);
    } else {
        dl_base_addr = AFE_GET_REG(AFE_DL1_BASE);
        hw_current_read_idx = AFE_GET_REG(AFE_DL1_CUR);
        uint32_t owo,oro,osize;
        dl_base_addr = AFE_GET_REG(AFE_DL1_BASE);
        hw_current_read_idx = AFE_GET_REG(AFE_DL1_CUR);
        oro = AFE_GET_REG(AFE_DL1_CUR) - AFE_GET_REG(AFE_DL1_BASE);
        owo = buffer_info->WriteOffset;
        osize = AFE_GET_REG(AFE_DL1_END)- AFE_GET_REG(AFE_DL1_BASE);
        //DSP_MW_LOG_I("dl out,wo=%d,ro=%d,len=%d\r\n", 3,owo,oro,osize);
    }
#endif

    /*Mce play en check*/
    if (runtime->irq_exist == false) {
        runtime->irq_exist = true;
        U32 first_dl_irq_time;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &first_dl_irq_time);
        DSP_MW_LOG_I("DSP afe dl1 interrupt exist, bt_clk 0x%x, bt_intra:0x%x, gpt_time:%d", 3, rBb->rClkCtl.rNativeClock, rBb->rClkCtl.rNativePhase, first_dl_irq_time);
        dl_irq_cnt = 1;
        source = sink->transform->source;
#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
        if (afe_block->u4asrcflag) {
            #ifdef HWSRC_UNDERRUN_DETECT
            hwsrc_out_remain = 0;
            #endif

        }
#endif
        if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_N9SCO))
        {
            if (SourceSize(source) != 0)
            {
                DSP_MW_LOG_I("eSCO DL audio irq first Wo:%d Ro:%d", 2,source->streamBuffer.AVMBufferInfo.WriteIndex, source->streamBuffer.AVMBufferInfo.ReadIndex);
            }
            else
            {
                DSP_MW_LOG_I("eSCO First IRQ meet size 0 Wo:%d Ro:%d", 2,source->streamBuffer.AVMBufferInfo.WriteIndex, source->streamBuffer.AVMBufferInfo.ReadIndex);
            }
        }
#ifdef AIR_BT_CODEC_BLE_ENABLED
        if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_N9BLE))
        {
            if (SourceSize(source) != 0)
            {
                DSP_MW_LOG_I("BLE DL audio irq first Wo:%d Ro:%d", 2,source->streamBuffer.ShareBufferInfo.WriteOffset,source->streamBuffer.ShareBufferInfo.ReadOffset);
            }
            else
            {
                DSP_MW_LOG_I("BLE First IRQ meet size 0 Wo:%d Ro:%d", 2,source->streamBuffer.ShareBufferInfo.WriteOffset,source->streamBuffer.ShareBufferInfo.ReadOffset);
            }
        }
#endif
    } else if (runtime->AfeBlkControl.u4awsflag == true) {
        uint32_t sync_reg_mon1 = afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1);
        uint32_t sync_reg_mon2 = afe_get_bt_sync_monitor_state(AUDIO_DIGITAL_BLOCK_MEM_DL1);
        if ((sync_reg_mon1 == 0) || (sync_reg_mon2 == 0)) {
            DSP_MW_LOG_I("DSP afe BT sync monitor by dl1 0x%x wait cnt: %d Wo: %d Ro: %d Bf: %d", 5,sync_reg_mon1,runtime->afe_wait_play_en_cnt,buffer_info->WriteOffset,buffer_info->ReadOffset,buffer_info->bBufferIsFull);
            if ((runtime->afe_wait_play_en_cnt != PLAY_EN_REINIT_DONE_MAGIC_NUM) && (runtime->afe_wait_play_en_cnt != PLAY_EN_TRIGGER_REINIT_MAGIC_NUM))
            {
                runtime->afe_wait_play_en_cnt++;
                if ((runtime->afe_wait_play_en_cnt * runtime->period) > PLAY_EN_DELAY_TOLERENCE)
                {
                   runtime->afe_wait_play_en_cnt = PLAY_EN_TRIGGER_REINIT_MAGIC_NUM;
                   buffer_info->bBufferIsFull = FALSE;
                }
            }
        }
    }
	dl_irq_cnt++;

    #if 0//modify for ab1568
    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL1))
    #endif
    {
        hal_nvic_save_and_set_interrupt_mask(&mask);
        if ((hw_current_read_idx == 0) || (chk_dl1_cur(hw_current_read_idx) == false)) { //should chk setting if =0
#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
            hw_current_read_idx = word_size_align((S32) dl_base_addr);
#else
            hw_current_read_idx = dl_base_addr;
#endif
        }
        pre_offset = buffer_info->ReadOffset;

        /* Update Clk Skew: clk information */
        if(afe_get_bt_sync_monitor_state(AUDIO_DIGITAL_BLOCK_MEM_DL1)){
            BT_CLOCK_OFFSET_SCENARIO CLK_Offset_Type = BT_CLK_Offset;
            #ifdef MTK_CELT_DEC_ENABLE
            if(sink->transform->source != NULL && sink != NULL){
                if(sink->transform->source->type == SOURCE_TYPE_A2DP && sink->transform->source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT){
                    CLK_Offset_Type = ULL_CLK_Offset;
                }
            }
            #endif
            Clock_Skew_Offset_Update(CLK_Offset_Type);
            #ifdef AIR_BT_CODEC_BLE_ENABLED
            if(sink->transform->source->type == SOURCE_TYPE_N9BLE) {
                Clock_Skew_UL_Offset_Update(BT_CLK_Offset);
            }
            #endif
        }

        /* For Downlink Clk Skew IRQ period control */
#ifdef ENABLE_HWSRC_CLKSKEW
        if((ClkSkewMode_g == CLK_SKEW_V1) || ((ClkSkewMode_g == CLK_SKEW_V2) && ((sink->transform != NULL) && (sink->transform->source->type == SOURCE_TYPE_N9SCO)))){
            cp_samples = clk_skew_check_dl_status();
        #ifdef AIR_BT_CODEC_BLE_ENABLED
        } else if ((sink->transform != NULL) && (sink->transform->source->type == SOURCE_TYPE_N9BLE)) {
            cp_samples = clk_skew_check_dl_status();
        #endif
        } else {
            cp_samples = 0;
        }
#else
        cp_samples = clk_skew_check_dl_status();
#endif
        if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_N9SCO)){
            if(ul_source->param.audio.mem_handle.pure_agent_with_src){
                cp_samples = cp_samples_ext;
            }
        }

        #if 0//modify for ab1568
        afe_update_audio_irq_cnt(afe_irq_request_number(AUDIO_DIGITAL_BLOCK_MEM_DL1),
                                 runtime->count + cp_samples);
        #else
        hal_audio_memory_irq_period_parameter_t irq_period;
        irq_period.memory_select= HAL_AUDIO_MEMORY_DL_DL1;
        irq_period.rate = runtime->rate;
        if(runtime->rate != runtime->src_rate) {
            irq_period.irq_counter = (uint32_t)((int32_t)runtime->count + (int32_t)((int32_t)cp_samples*(int32_t)(runtime->rate)/(int32_t)(runtime->src_rate)));
        } else {
            irq_period.irq_counter = (uint32_t)((int32_t)runtime->count + (int32_t)cp_samples);
        }
        if(cp_samples != 0) {
            DSP_MW_LOG_I("[ClkSkew] DL irq_cnt:%d, cp:%d, fs_in:%d, fs_out:%d, cnt:%d", 5, irq_period.irq_counter, cp_samples, runtime->src_rate, runtime->rate, runtime->count);
        }
        hal_audio_control_set_value((hal_audio_set_value_parameter_t *)&irq_period, HAL_AUDIO_SET_MEMORY_IRQ_PERIOD);
        #endif


        buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
        isr_interval = (pre_offset <= buffer_info->ReadOffset)
                         ? (buffer_info->ReadOffset - pre_offset)
                         : (buffer_info->length + buffer_info->ReadOffset - pre_offset);
        //printf("dl1 pre %d rpt %d wpt %d len %d\r\n",pre_offset,buffer_info->ReadOffset,buffer_info->WriteOffset,buffer_info->length);
        if (dl_base_addr != NULL){ //Prevent to access null pointer when the last isr is executed after HW is turned off and pointer is cleared
            /*Clear up last time used memory */
            if(!(sink->transform->source->type == SOURCE_TYPE_A2DP && sink->transform->source->param.n9_a2dp.codec_info.codec_cap.type == BT_A2DP_CODEC_AIRO_CELT)){
                if (buffer_info->ReadOffset >= pre_offset) {
                    memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->ReadOffset - pre_offset);
                } else {
                    memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->length - pre_offset);
                    memset((void *)dl_base_addr, 0, buffer_info->ReadOffset);
                }
            }
        }
        if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
            buffer_info->bBufferIsFull = FALSE;
        }

#ifdef AIR_BT_CODEC_BLE_ENABLED
        if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_N9BLE))
        {
            source = sink->transform->source;
            DSP_CALLBACK_PTR callback_ptr;
            callback_ptr = DSP_Callback_Get(source, sink);
            source->param.n9ble.dl_afe_skip_time = (source->param.n9ble.dl_afe_skip_time != 0) ? source->param.n9ble.dl_afe_skip_time - 1 : 0;
            if (callback_ptr->Status != CALLBACK_SUSPEND)
            {
                Sink_Audio_FlushBuffer(sink,sink->param.audio.src_rate* (source->param.n9ble.frame_interval * sink->param.audio.format_bytes/1000)  / 1000);
                SourceDrop(source, source->param.n9ble.frame_length * source->param.n9ble.process_number);
                DSP_MW_LOG_I("Callback Busy : %d, Source drop and move Sink WO, dl_irq_cnt:%d fill size = %d %d", 4, callback_ptr->Status, dl_irq_cnt,(sink->param.audio.channel_num * sink->param.audio.src_rate* (source->param.n9ble.frame_interval * sink->param.audio.format_bytes/1000) / 1000),clock_skew_asrc_get_input_sample_size());
                if (source->param.n9ble.frame_interval == 7500)
                {
                    Sink_Audio_FlushBuffer(sink,sink->param.audio.src_rate* (source->param.n9ble.frame_interval * sink->param.audio.format_bytes/1000)  / 1000);
                    SourceDrop(source, source->param.n9ble.frame_length * source->param.n9ble.process_number);
                    if(source->param.n9ble.skip_after_drop)
                    {
                        source->param.n9ble.dl_afe_skip_time += (1 + ((source->param.n9ble.dl_afe_skip_time&0x1)^1));
                    }
                    else
                    {
                        source->param.n9ble.skip_after_drop = TRUE;
                    }
                }
            }

            if ((source->param.n9ble.dl_afe_skip_time != 0)||(source->param.n9ble.skip_after_drop))
            {
                DSP_MW_LOG_I("Callback Busy when LE call, skip process time %d + 0x%x", 2, source->param.n9ble.dl_afe_skip_time,source->param.n9ble.skip_after_drop);
            }
            //uint32_t _out_read, _out_write ;
            //_out_write = AFE_READ(ASM_CH01_OBUF_WRPNT);
            //_out_read = AFE_READ(ASM_CH01_OBUF_RDPNT);
            //DSP_MW_LOG_I("Wo:%d Ro:%d oWo:%d oRo:%d iwo:%d", 5,buffer_info->WriteOffset,buffer_info->ReadOffset,_out_write- AFE_GET_REG(ASM_OBUF_SADR),_out_read- AFE_GET_REG(ASM_OBUF_SADR),AFE_GET_REG(ASM_CH01_IBUF_WRPNT) - AFE_GET_REG(ASM_IBUF_SADR));

#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
#if (AFE_REGISTER_ASRC_IRQ)
            if(ClkSkewMode_g == CLK_SKEW_V2) {
                if(afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK)== false){//modify for clock skew
                    afe_set_asrc_irq_enable(AFE_MEM_ASRC_1, true);
                    //DSP_MW_LOG_I("asrc afe_dl1_interrupt_handler asrc_irq_is_enabled %d",1,afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK));
                }
            }
#endif /*AFE_REGISTER_ASRC_IRQ*/
#endif /*ENABLE_HWSRC_ON_MAIN_STREAM*/

            //#ifndef MTK_BT_HFP_FORWARDER_ENABLE
            if (SourceSize(source) == 0)
            {
                DSP_MW_LOG_I("BLE DL audio irq no data in Wo:%d Ro:%d", 2,source->streamBuffer.ShareBufferInfo.WriteOffset,source->streamBuffer.ShareBufferInfo.ReadOffset);
                SourceConfigure(source,SCO_SOURCE_WO_ADVANCE,1); // force stream to process even there has no frame in avm buffer
            }
            //#endif
        }
#endif/*AIR_BT_CODEC_BLE_ENABLED*/
        if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_N9SCO))
        {
            source = sink->transform->source;
            DSP_CALLBACK_PTR callback_ptr;

            callback_ptr = DSP_Callback_Get(source, sink);
            #if DL_TRIGGER_UL
            if (sink->transform->source->param.n9sco.dl_enable_ul == TRUE)
            {
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &(sink->transform->source->param.n9sco.ul_play_gpt));
                sink->transform->source->param.n9sco.dl_enable_ul = FALSE;
            }
            #endif
            #if 0
            if (callback_ptr->Status != CALLBACK_SUSPEND)
            {
                //DSP_MW_LOG_I("Callback Busy : %d", 1,callback_ptr->Status);
                #if DL_TRIGGER_UL
                sink->transform->source->param.n9sco.dl_enable_ul = TRUE;
                SourceDrop(source,source->param.n9sco.process_data_length);
                sink->transform->source->param.n9sco.dl_enable_ul = FALSE;
                #else
                SourceDrop(source,source->param.n9sco.process_data_length);
                #endif
                uint32_t prewo = buffer_info->WriteOffset;
                buffer_info->WriteOffset = (buffer_info->WriteOffset + (sink->param.audio.channel_num * sink->param.audio.src_rate * sink->param.audio.period * sink->param.audio.format_bytes / 1000))%buffer_info->length;
                DSP_MW_LOG_I("Callback Busy : %d, Source drop and move Sink WO:%d, prewo:%d, buflen:%d, dl_irq_cnt:%d", 5, callback_ptr->Status, buffer_info->WriteOffset, prewo, buffer_info->length, dl_irq_cnt);
            }
            #endif

#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
//#if (AFE_REGISTER_ASRC_IRQ)
            if(ClkSkewMode_g == CLK_SKEW_V2) {
                if(afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK)== false){//modify for clock skew
                    afe_set_asrc_irq_enable(AFE_MEM_ASRC_1, true);
                    //DSP_MW_LOG_I("asrc afe_dl1_interrupt_handler asrc_irq_is_enabled %d",1,afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK));
                }
            }
//#endif /*AFE_REGISTER_ASRC_IRQ*/
#endif /*ENABLE_HWSRC_ON_MAIN_STREAM*/
            //#ifndef MTK_BT_HFP_FORWARDER_ENABLE
            if (SourceSize(source) == 0)
            {
                DSP_MW_LOG_I("eSCO DL audio irq no data in Wo:%d Ro:%d", 2,source->streamBuffer.AVMBufferInfo.WriteIndex,source->streamBuffer.AVMBufferInfo.ReadIndex);
                SourceConfigure(source,SCO_SOURCE_WO_ADVANCE,2);// add 2 frame in advance for HFP MCE
            } else {
                //DSP_MW_LOG_I("eSCO DL audio irq have data in Wo:%d Ro:%d", 2,source->streamBuffer.AVMBufferInfo.WriteIndex,source->streamBuffer.AVMBufferInfo.ReadIndex);

            }
            //#endif
        }
#ifdef HWSRC_UNDERRUN_DETECT
        #ifdef AIR_BT_CODEC_BLE_ENABLED
        if ((sink->transform != NULL)&&((sink->transform->source->type == SOURCE_TYPE_A2DP)||(sink->transform->source->type == SOURCE_TYPE_N9BLE))&&(afe_block->u4asrcflag == TRUE)&&(ClkSkewMode_g== CLK_SKEW_V2))
        #else
        if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_A2DP)&&(afe_block->u4asrcflag == TRUE)&&(ClkSkewMode_g== CLK_SKEW_V2))
        #endif
        {
            U16 channelnum_shift = (sink->param.audio.mem_handle.with_mono_channel == FALSE) ? 1 : 0;
            src_out_data = src_out_data>>channelnum_shift;
            hwsrc_out_remain = (hwsrc_out_remain >= src_out_data) ? (hwsrc_out_remain - src_out_data) : 0; // Calculate if there is remaining HWSRC out data been comsumed in this round
            if ((((isr_interval>>channelnum_shift) + (clock_skew_asrc_get_input_sample_size())*sink->param.audio.format_bytes)/((runtime->src_rate/1000)) + (hwsrc_out_remain/(afe_block->u4asrcrate/1000))) < (sink->param.audio.period*sink->param.audio.format_bytes))// Check if SRC convert enough amount during this ISR interval
            {
                DSP_MW_LOG_W("SRAM Empty with UNDERRUN_DETECT, remain_data:%d, isr_interval:%d, period_num:%d, last_remain:%d iro:%d", 5,src_out_data, (isr_interval>>channelnum_shift), (sink->param.audio.period*sink->param.audio.format_bytes), (hwsrc_out_remain/(afe_block->u4asrcrate/1000)),buffer_info->ReadOffset);
                //runtime->afe_wait_play_en_cnt = PLAY_EN_TRIGGER_REINIT_MAGIC_NUM;
            }
            hwsrc_out_remain = src_out_data;
        }
#endif

#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
        if ((OFFSET_OVERFLOW_CHK(pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset)&&(buffer_info->bBufferIsFull==FALSE))
            || (((buffer_info->WriteOffset+buffer_info->length-buffer_info->ReadOffset)%buffer_info->length < 32) && (afe_block->u4asrcflag)))
#else
        if  (OFFSET_OVERFLOW_CHK(pre_offset,(buffer_info->ReadOffset)%buffer_info->length, buffer_info->WriteOffset )&&(buffer_info->bBufferIsFull == FALSE))//Sram empty
#endif
        {

            bool empty = true;
#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
            if(afe_block->u4asrcflag) {
                uint32_t src_out_size, src_out_read, src_out_write, remain_data;
                src_out_write = AFE_READ(ASM_CH01_OBUF_WRPNT);
                src_out_read = AFE_READ(ASM_CH01_OBUF_RDPNT);
                src_out_size = AFE_READ(ASM_OBUF_SIZE);
                remain_data = (src_out_write > src_out_read) ? src_out_write-src_out_read : src_out_write+src_out_size- src_out_read;
                remain_data = (sink->param.audio.channel_num>=2) ? remain_data>>1 : remain_data;
                if (remain_data > ((afe_block->u4asrcrate*sink->param.audio.period*sink->param.audio.format_bytes/2))/1000) {
                    empty = false;
                } else {
                    DSP_MW_LOG_W("SRAM Empty play en, remain_data:%d, data_thd:%d, src_out_write:0x%x, src_out_read:0x%x, buf_size:%d", 5, remain_data, ((afe_block->u4asrcrate*sink->param.audio.period*sink->param.audio.format_bytes)/2/1000), src_out_write, src_out_read, src_out_size);
                }
            }
#endif
            if(empty){
                DSP_MW_LOG_W("SRAM Empty play en:%d pR:%d R:%d W:%d", 4,isr_interval, pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);
#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
                U16 pre_write_offset = buffer_info->WriteOffset;

                if (afe_block->u4asrcflag) {
                    #ifdef AIR_BT_CODEC_BLE_ENABLED
                    if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_N9BLE))
                    {
                        buffer_info->WriteOffset = ((U32)(buffer_info->WriteOffset + (sink->param.audio.channel_num * sink->param.audio.rate * sink->param.audio.period * sink->param.audio.format_bytes / 1000)))%buffer_info->length;
                        sink->param.audio.sram_empty_fill_size +=(sink->param.audio.channel_num * sink->param.audio.rate * sink->param.audio.period * sink->param.audio.format_bytes / 1000);
                    }
                    else
                    #endif
                    {
                    buffer_info->WriteOffset = (buffer_info->ReadOffset + buffer_info->length/2) % buffer_info->length;
                    }
                } else {
                    if (pre_offset < buffer_info->ReadOffset) {
                        buffer_info->WriteOffset = (buffer_info->ReadOffset*2 - pre_offset)% buffer_info->length;
                    } else {
                        buffer_info->WriteOffset = (buffer_info->ReadOffset*2 + (buffer_info->length - pre_offset))% buffer_info->length;
                    }
                }

//#if (AFE_REGISTER_ASRC_IRQ)
#ifdef ENABLE_HWSRC_CLKSKEW
                if(ClkSkewMode_g== CLK_SKEW_V2) {
                    if(afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK)== false){//modify for clock skew
                        afe_set_asrc_irq_enable(AFE_MEM_ASRC_1, true);
                        //DSP_MW_LOG_W("asrc afe_dl1_interrupt_handler asrc_irq_is_enabled %d",1,afe_get_asrc_irq_is_enabled(AFE_MEM_ASRC_1, ASM_IER_IBUF_EMPTY_INTEN_MASK));
                    }
                }
#endif /*ENABLE_HWSRC_CLKSKEW*/
//#endif /*AFE_REGISTER_ASRC_IRQ*/
#endif /*ENABLE_HWSRC_ON_MAIN_STREAM*/

                if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_A2DP)&&(sink->transform->source->param.n9_a2dp.mce_flag))
                {
#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
                    DSP_MW_LOG_W("add empty_fill_size %d %d", 2, sink->param.audio.sram_empty_fill_size, ((pre_write_offset >= buffer_info->WriteOffset) ? (pre_write_offset - buffer_info->WriteOffset) : (buffer_info->length - buffer_info->WriteOffset + pre_write_offset)));
                    sink->param.audio.sram_empty_fill_size += (( buffer_info->WriteOffset >= pre_write_offset) ? (buffer_info->WriteOffset - pre_write_offset ) : (buffer_info->length - pre_write_offset + buffer_info->WriteOffset));
#else
                    DSP_MW_LOG_W("add empty_fill_size %d %d",2,sink->param.audio.sram_empty_fill_size,2*isr_interval + ((buffer_info->ReadOffset >= buffer_info->WriteOffset) ? (buffer_info->ReadOffset - buffer_info->WriteOffset) : (buffer_info->length - buffer_info->WriteOffset + buffer_info->ReadOffset)));
                    sink->param.audio.sram_empty_fill_size += 2*isr_interval + ((buffer_info->ReadOffset >= buffer_info->WriteOffset) ? (buffer_info->ReadOffset - buffer_info->WriteOffset) : (buffer_info->length - buffer_info->WriteOffset + buffer_info->ReadOffset));
                    buffer_info->WriteOffset = (buffer_info->ReadOffset + 2*isr_interval)%buffer_info->length;
#endif
                }
                if (dl_base_addr != NULL){ //Prevent to access null pointer when the last isr is executed after HW is turned off and pointer is cleared
                    if (buffer_info->WriteOffset > buffer_info->ReadOffset){
                        memset((void *)(dl_base_addr + buffer_info->ReadOffset), 0, buffer_info->WriteOffset - buffer_info->ReadOffset);

                    } else {
                        memset((void *)(dl_base_addr + buffer_info->ReadOffset), 0, buffer_info->length - buffer_info->ReadOffset);
                        memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);

                    }
                }
            }
        } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
            buffer_info->bBufferIsFull = FALSE;
        }

#if 0
//#if defined(MTK_GAMING_MODE_HEADSET)
        if((Source_blks[SOURCE_TYPE_AUDIO] != NULL) && (Source_blks[SOURCE_TYPE_AUDIO]->transform != NULL) && (Source_blks[SOURCE_TYPE_AUDIO]->transform->sink != NULL)) {
            if((Source_blks[SOURCE_TYPE_AUDIO]->transform->sink->type >= SINK_TYPE_AUDIO_TRANSMITTER_MIN) && (Source_blks[SOURCE_TYPE_AUDIO]->transform->sink->type <= SINK_TYPE_AUDIO_TRANSMITTER_MAX) && (Source_blks[SOURCE_TYPE_AUDIO]->transform->sink->param.data_ul.scenario_sub_id == AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET)){
                SINK game_headset_voice_sink;
                SHARE_BUFFER_INFO_PTR ShareBufferInfo;
                game_headset_voice_param_t *voice_para;

                game_headset_voice_sink = Source_blks[SOURCE_TYPE_AUDIO]->transform->sink;
                memcpy(&(game_headset_voice_sink->streamBuffer.ShareBufferInfo), game_headset_voice_sink->param.data_ul.share_info_base_addr, 32);/* share info fix 32 byte */
                game_headset_voice_sink->streamBuffer.ShareBufferInfo.startaddr = hal_memview_cm4_to_dsp0(game_headset_voice_sink->streamBuffer.ShareBufferInfo.startaddr);
                voice_para = &(game_headset_voice_sink->param.data_ul.scenario_param.voice_param);
                ShareBufferInfo = &game_headset_voice_sink->streamBuffer.ShareBufferInfo;

                if((voice_para->ul_process_done == TRUE) && ((voice_para->dl_irq_cnt == 0 || voice_para->dl_irq_cnt == 3))) {
                    // update wo per 6 dl irq
                    uint32_t total_buffer_size = ShareBufferInfo->sub_info.block_info.blk_size * ShareBufferInfo->sub_info.block_info.blk_num;
                    ShareBufferInfo->WriteOffset = (ShareBufferInfo->WriteOffset + ShareBufferInfo->sub_info.block_info.blk_size * 1) % total_buffer_size;
                    game_headset_voice_sink->param.data_ul.share_info_base_addr->WriteOffset = ShareBufferInfo->WriteOffset;
                    //DSP_MW_LOG_I("[audio transmitter][gaming_headset ul]: Update WO:%d", 1, ShareBufferInfo->WriteOffset);
                    uint32_t bt_clk;
                    uint16_t intra_clk;
                    MCE_GetBtClk((BTCLK *)&bt_clk,(BTPHASE *)&intra_clk,ULL_CLK_Offset);

                    // CCNI to notice controller
                    if(game_headset_voice_sink->param.data_ul.current_notification_index == 0) {
                        /* prepare message */
                        hal_ccni_message_t msg;
                        msg.ccni_message[0] = AUDIO_TRANSMITTER_GAMING_MODE_VOICE_HEADSET;
                        msg.ccni_message[1] = bt_clk;
                        /* send CCNI to notify controller*/
                        hal_ccni_set_event(CCNI_DSP0_TO_CM4_EVENT2, &msg);
                        game_headset_voice_sink->param.data_ul.current_notification_index = 1;
                        DSP_MW_LOG_I("[audio transmitter][gaming_headset ul]: send CCNI to notice controller, current_notification_index:%d, bt_clk:%d", 2, game_headset_voice_sink->param.data_ul.current_notification_index, bt_clk);
                    }

                    //DSP_MW_LOG_I("[audio transmitter][gaming_headset ul]: Update dl_irq_cnt:%d (after 0 / 5)", 1, voice_para->dl_irq_cnt);
                    voice_para->dl_irq_cnt++;
                    if(voice_para->dl_irq_cnt == 6 ) {
                        voice_para->dl_irq_cnt = 0;
                    }
                }else {
                    //DSP_MW_LOG_I("[audio transmitter][gaming_headset ul]: Update dl_irq_cnt:%d", 1, voice_para->dl_irq_cnt);
                    voice_para->dl_irq_cnt++;
                    if(voice_para->dl_irq_cnt == 6 ) {
                        voice_para->dl_irq_cnt = 0;
                    }
                }
            }
        }
#endif /*defined(MTK_GAMING_MODE_HEADSET)*/



#if 0
        remain_sink_data = sink->streamBuffer.BufferInfo.length - SinkSlack(sink);
        if(remain_sink_data == 0) {
            //printf("[AUD]underflow\r\n");
            SinkBufferUpdateReadPtr(sink, 0);
            hal_nvic_restore_interrupt_mask(mask);
            return;
        }

        if (afe_block->u4ReadIdx > afe_block->u4WriteIdx) {
            sram_free_space = afe_block->u4ReadIdx - afe_block->u4WriteIdx;
        }
        else if ((afe_block->u4bufferfullflag == TRUE)&&(afe_block->u4ReadIdx == afe_block->u4WriteIdx)){ // for Play_en not active yet
            sram_free_space = 0;
        }
        else{
            sram_free_space = afe_block->u4BufferSize + afe_block->u4ReadIdx - afe_block->u4WriteIdx;
        }
        sram_free_space = word_size_align(sram_free_space);// 4-byte alignment
        if (channel_type == STREAM_S_AFE_S){
            copy_size = MINIMUM(sram_free_space >> 1, remain_sink_data);
            afe_block->u4bufferfullflag = (sram_free_space == copy_size << 1) ? TRUE : FALSE;
            sram_free_space = copy_size << 1;
        } else {
            copy_size = MINIMUM(sram_free_space, remain_sink_data);
            afe_block->u4bufferfullflag = (sram_free_space == copy_size) ? TRUE : FALSE;
            sram_free_space = copy_size;
        }

        #if 0
        if (afe_block->u4WriteIdx + sram_free_space < afe_block->u4BufferSize) {
            audio_split_sink_to_interleaved(sink, (afe_block->pSramBufAddr + afe_block->u4WriteIdx), 0, sram_free_space);
        } else {
            uint32_t size_1 = 0, size_2 = 0;
            size_1 = word_size_align((afe_block->u4BufferSize - afe_block->u4WriteIdx));
            size_2 = word_size_align((sram_free_space - size_1));
            audio_split_sink_to_interleaved(sink, (afe_block->pSramBufAddr + afe_block->u4WriteIdx), 0, size_1);
            audio_split_sink_to_interleaved(sink, (afe_block->pSramBufAddr), size_1, size_2);
        }
        #else
        audio_afe_sink_interleaved(sink, sram_free_space);
        #endif
        afe_block->u4WriteIdx = (afe_block->u4WriteIdx + sram_free_space)% afe_block->u4BufferSize;
        SinkBufferUpdateReadPtr(sink, copy_size);
#endif


        if (sink->transform != NULL) {
            DSP_STREAMING_PARA_PTR  pStream =DSP_Streaming_Get(sink->transform->source ,sink);
            if (pStream!=NULL) {
                if (pStream->streamingStatus == STREAMING_END) {
                    if (Audio_setting->Audio_sink.Zero_Padding_Cnt>0) {
                        Audio_setting->Audio_sink.Zero_Padding_Cnt--;
                        DSP_MW_LOG_I("DL zero pad %d",1,Audio_setting->Audio_sink.Zero_Padding_Cnt);
                    }
                }
            }
        }
#ifdef MTK_BT_HFP_ENABLE
        /* for eSCO Sync L & R */
        if (g_ignore_next_drop_flag) {
            g_ignore_next_drop_flag = false;
        }
#endif
        AudioCheckTransformHandle(sink->transform);
        hal_nvic_restore_interrupt_mask(mask);
    }

    pre_AFE_CLASSG_MON0_b29 = AFE_CLASSG_MON0_b29 & 0x20000000;
    AFE_CLASSG_MON0_b29 = ReadREG(0x70000EFC) & 0x20000000;

    if (((pre_AFE_CLASSG_MON0_b29 == 0x20000000) && (AFE_CLASSG_MON0_b29 == 0)) || ((pre_AFE_CLASSG_MON0_b29 == 0x0) && (AFE_CLASSG_MON0_b29 == 0))) {
        vRegResetBit(0xA207022C, 8);
        hal_gpt_delay_us(20);
        vRegSetBit(0xA207022C, 8);
    }
#ifdef ENABLE_HWSRC_ON_MAIN_STREAM
    if (afe_block->u4asrcflag) {
//#if (AFE_REGISTER_ASRC_IRQ)

//#else
#ifdef ENABLE_HWSRC_CLKSKEW
       if(ClkSkewMode_g == CLK_SKEW_V2) {

       } else
#endif
       {
          AFE_WRITE(ASM_CH01_IBUF_WRPNT, buffer_info->WriteOffset + AFE_READ(ASM_IBUF_SADR));
       }
//#endif /*AFE_REGISTER_ASRC_IRQ*/
    }
#endif /*ENABLE_HWSRC_ON_MAIN_STREAM*/

}


#ifdef ENABLE_HWSRC_CLKSKEW
void clock_skew_asrc_set_compensated_sample(afe_asrc_compensating_t cp_point){
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    afe_block->u4asrcSetCompensatedSamples = cp_point;
    //DSP_MW_LOG_W("clock_skew_asrc_set_compensated_sample: %d", 1, afe_block->u4asrcSetCompensatedSamples);
}

void clock_skew_asrc_get_compensated_sample(uint32_t *accumulate_array){
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    if(accumulate_array !=NULL && sizeof(accumulate_array[0])>=sizeof(afe_block->u4asrcGetaccumulate_array[0]) ){
        memcpy( accumulate_array, afe_block->u4asrcGetaccumulate_array, sizeof(afe_block->u4asrcGetaccumulate_array) );
    }else{
        DSP_MW_LOG_W("clock_skew_asrc_get_compensated_sample fail",0);

    }
}

uint32_t clock_skew_asrc_get_input_sample_size(void)
{
    uint32_t sample_size = 0;
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    SOURCE_TYPE source_type = sink->transform->source->type;

    switch (source_type)
    {
        case SOURCE_TYPE_A2DP:
            sample_size = 128;
            break;

        case SOURCE_TYPE_N9SCO:
            sample_size = 240;
            break;

        #ifdef AIR_BT_CODEC_BLE_ENABLED
        case SOURCE_TYPE_N9BLE:
            sample_size = sink->param.audio.count*sink->param.audio.src_rate/sink->param.audio.rate;
            break;
        #endif

        default:
            #ifndef AIR_I2S_SLAVE_ENABLE
            DSP_MW_LOG_W("clock_skew_asrc_get_input_sample_size fail type:%d", 1, source_type);
            #endif
            break;
    }

    return sample_size;
}

#endif

ATTR_TEXT_IN_IRAM void stream_audio_srcl_interrupt(void)
{
    uint32_t output_write_pointer, input_read_pointer, input_read_offset;
    hal_audio_get_value_parameter_t get_value_parameter;
    hal_audio_set_value_parameter_t set_value_parameter;
    SINK sink = Sink_blks[SINK_TYPE_AUDIO];
    uint32_t mask,pre_offset,isr_interval ;
    uint32_t hw_current_read_idx = 0;
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    AUDIO_PARAMETER *runtime = &sink->param.audio;
    uint32_t dl_base_addr = (uint32_t)buffer_info->startaddr[0];
    get_value_parameter.get_current_offset.memory_select = HAL_AUDIO_MEMORY_DL_SRC1;
    input_read_pointer = hal_audio_get_value(&get_value_parameter, HAL_AUDIO_GET_MEMORY_INPUT_CURRENT_OFFSET);
    output_write_pointer = hal_audio_get_value(&get_value_parameter, HAL_AUDIO_GET_MEMORY_OUTPUT_CURRENT_OFFSET);
    UNUSED(input_read_offset);

    if (!sink) {
        DSP_MW_LOG_I("sink == NULL\r\n", 0);
        return;
    }
    if (!sink->streamBuffer.BufferInfo.startaddr[0])
    {
        DSP_MW_LOG_I("ufferInfo.startaddr[0] == NULL\r\n", 0);
        return;
    }

    dl_base_addr = AFE_GET_REG(ASM_IBUF_SADR);
    //hw_current_read_idx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT);
    //input_read_offset = input_read_pointer - (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
    hw_current_read_idx = input_read_pointer;
    #if 1
    if (runtime->irq_exist == false) {
        runtime->irq_exist = true;
        DSP_MW_LOG_I("DSP afe dl1 interrupt exist\r\n", 0);
    }
    //stream_audio_sink_handler(sink->type, input_read_offset);
    hal_nvic_save_and_set_interrupt_mask(&mask);

    if ((hw_current_read_idx == 0)) { //should chk setting if =0

        hw_current_read_idx = (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];

    }
    pre_offset = buffer_info->ReadOffset;


    buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
    isr_interval = (pre_offset <= buffer_info->ReadOffset)
                     ? (buffer_info->ReadOffset - pre_offset)
                     : (buffer_info->length + buffer_info->ReadOffset - pre_offset);
    //printf("srcl pre %d rpt %d wpt %d len %d asrc out wo %d Ro %d\r\n",pre_offset,buffer_info->ReadOffset,buffer_info->WriteOffset,buffer_info->length,AFE_GET_REG(ASM_CH01_OBUF_WRPNT)-AFE_GET_REG(ASM_OBUF_SADR),AFE_GET_REG(ASM_CH01_OBUF_RDPNT)-AFE_GET_REG(ASM_OBUF_SADR));
    /*Clear up last time used memory */
    if (buffer_info->ReadOffset >= pre_offset) {
        memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->ReadOffset - pre_offset);
    } else {
        memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->length - pre_offset);
        memset((void *)dl_base_addr, 0, buffer_info->ReadOffset);
    }

    if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }

    if  (OFFSET_OVERFLOW_CHK(pre_offset,(buffer_info->ReadOffset)%buffer_info->length, buffer_info->WriteOffset )&&(buffer_info->bBufferIsFull == FALSE))//Sram empty

    {

        bool empty = true;

        if(empty){
            DSP_MW_LOG_W("SRAM Empty play en:%d pR:%d R:%d W:%d", 4,isr_interval, pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);
            if ((sink->transform != NULL)&&(sink->transform->source->type == SOURCE_TYPE_A2DP)&&(sink->transform->source->param.n9_a2dp.mce_flag))
            {
                DSP_MW_LOG_W("add empty_fill_size %d %d",2,sink->param.audio.sram_empty_fill_size,2*isr_interval + ((buffer_info->ReadOffset >= buffer_info->WriteOffset) ? (buffer_info->ReadOffset - buffer_info->WriteOffset) : (buffer_info->length - buffer_info->WriteOffset + buffer_info->ReadOffset)));
                sink->param.audio.sram_empty_fill_size += 2*isr_interval + ((buffer_info->ReadOffset >= buffer_info->WriteOffset) ? (buffer_info->ReadOffset - buffer_info->WriteOffset) : (buffer_info->length - buffer_info->WriteOffset + buffer_info->ReadOffset));
            }
            buffer_info->WriteOffset = (buffer_info->ReadOffset + 2*isr_interval)%buffer_info->length;
            if (buffer_info->WriteOffset > buffer_info->ReadOffset){
                memset((void *)(dl_base_addr + buffer_info->ReadOffset), 0, buffer_info->WriteOffset - buffer_info->ReadOffset);

            } else {
                memset((void *)(dl_base_addr + buffer_info->ReadOffset), 0, buffer_info->length - buffer_info->ReadOffset);
                memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);

            }
        }
    } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }


    if (sink->transform != NULL) {
        DSP_STREAMING_PARA_PTR  pStream =DSP_Streaming_Get(sink->transform->source ,sink);
        if (pStream!=NULL) {
            if (pStream->streamingStatus == STREAMING_END) {
                if (Audio_setting->Audio_sink.Zero_Padding_Cnt>0) {
                    Audio_setting->Audio_sink.Zero_Padding_Cnt--;
                    DSP_MW_LOG_W("DL zero pad %d",1,Audio_setting->Audio_sink.Zero_Padding_Cnt);//modify for ab1568
                }
            }
        }
    }


    #else
    //Verification code
    sink->streamBuffer.BufferInfo.ReadOffset = input_read_offset;
    sink->streamBuffer.BufferInfo.WriteOffset = (sink->streamBuffer.BufferInfo.length + sink->streamBuffer.BufferInfo.ReadOffset - 16)%sink->streamBuffer.BufferInfo.length;
    #endif

    set_value_parameter.set_current_offset.memory_select = HAL_AUDIO_MEMORY_DL_SRC1;
    set_value_parameter.set_current_offset.offset = sink->streamBuffer.BufferInfo.WriteOffset + AFE_GET_REG(ASM_IBUF_SADR);//(uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
    hal_audio_set_value(&set_value_parameter, HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET);
    AudioCheckTransformHandle(sink->transform);
    hal_nvic_restore_interrupt_mask(mask);
}


ATTR_TEXT_IN_IRAM void afe_vul1_interrupt_handler(void)
{
    uint32_t mask;
    uint32_t hw_current_write_idx_vul1, hw_current_write_idx_awb;
    uint32_t wptr_vul1_offset, pre_wptr_vul1_offset, pre_offset;
    U8 rcdc_ch_num = 0;
    static bool data_first_comming = false;
    SOURCE source = Source_blks[SOURCE_TYPE_AUDIO];
    int16_t cp_samples_ext = 0;
    if(source!=NULL){


        AUDIO_PARAMETER *runtime = &source->param.audio;
        BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;
        if(runtime->channel_num >= 2) {
            rcdc_ch_num = 2;
        } else {
            rcdc_ch_num = 1;
        }
        int32_t  wptr_vul1_offset_diff_defualt = (runtime->format_bytes)*(runtime->count)*(rcdc_ch_num);
        int32_t  wptr_vul1_offset_diff = 0;
        int16_t cp_samples = 0;


        /*Mce play en check*/
        if (runtime->irq_exist == false) {
            runtime->irq_exist = true;
            data_first_comming = true;
            U32 vul_irq_time;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &vul_irq_time);
            DSP_MW_LOG_I("DSP afe ul1 interrupt exist, time: %d", 1, vul_irq_time);
            runtime->pop_noise_pkt_num = 0;
            runtime->mute_flag = TRUE;
        } else if ((Sink_blks[SINK_TYPE_AUDIO] != NULL) &&
                   (Sink_blks[SINK_TYPE_AUDIO]->param.audio.AfeBlkControl.u4awsflag == true) &&
                   (Sink_blks[SINK_TYPE_AUDIO]->param.audio.irq_exist == false)) {
            uint32_t read_reg = afe_get_bt_sync_monitor(AUDIO_DIGITAL_BLOCK_MEM_DL1);
            if (read_reg == 0) {
                DSP_MW_LOG_I("DSP afe BT sync monitor by ul1 0x%x", 1, read_reg);
            }
        }

        hw_current_write_idx_vul1 = AFE_GET_REG(AFE_VUL_CUR);
        pre_wptr_vul1_offset = buffer_info->WriteOffset;

        if (source->param.audio.echo_reference == true) {
            #if 1 //modify for ab1568 workaround
            hw_current_write_idx_awb = AFE_GET_REG(AFE_AWB2_CUR);
            #else
            hw_current_write_idx_awb = AFE_GET_REG(AFE_AWB_CUR);
            #endif

        }

        #ifdef ENABLE_HWSRC_CLKSKEW
        if((ClkSkewMode_g == CLK_SKEW_V2) && (source->transform->sink->type == SINK_TYPE_N9SCO)){
            Clock_Skew_UL_Offset_Update(BT_CLK_Offset);
        }
        #endif

        #if 1//mdify for ab1568
        if(source->param.audio.mem_handle.pure_agent_with_src)
        {
            pre_wptr_vul1_offset = AFE_READ(ASM2_CH01_IBUF_WRPNT) - AFE_READ(ASM2_IBUF_SADR);
        }
        if(fgULRCDCPolling==TRUE){
            while(wptr_vul1_offset_diff < (wptr_vul1_offset_diff_defualt + 8*rcdc_ch_num)){
                 wptr_vul1_offset = AFE_GET_REG(AFE_VUL_CUR) - AFE_GET_REG(AFE_VUL_BASE);
                 if(wptr_vul1_offset >= pre_wptr_vul1_offset) {
                    wptr_vul1_offset_diff = wptr_vul1_offset - pre_wptr_vul1_offset;
                    //DSP_MW_LOG_I("[ClkSkew1] buffer_info->length:%d, vul now_wo:%d, pre_wo:%d, wo_diff:%d", 4, buffer_info->length, wptr_vul1_offset, pre_wptr_vul1_offset, wptr_vul1_offset_diff);
                 } else {
                    wptr_vul1_offset_diff = buffer_info->length + wptr_vul1_offset - pre_wptr_vul1_offset;
                    //DSP_MW_LOG_I("[ClkSkew2] buffer_info->length:%d, vul now_wo:%d, pre_wo:%d, wo_diff:%d", 4, buffer_info->length, wptr_vul1_offset, pre_wptr_vul1_offset, wptr_vul1_offset_diff);
                 }
            }
            fgULRCDCPolling = FALSE;
            wptr_vul1_offset_diff = wptr_vul1_offset_diff_defualt;
        }
        else {
            wptr_vul1_offset = hw_current_write_idx_vul1 - AFE_GET_REG(AFE_VUL_BASE);
        }
        wptr_vul1_offset = AFE_GET_REG(AFE_VUL_CUR) - AFE_GET_REG(AFE_VUL_BASE);
        #endif
        #if 0//modify for ab1568
        if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_VUL1))
        #endif

        {
            hal_nvic_save_and_set_interrupt_mask(&mask);
            if (hw_current_write_idx_vul1 == 0) {
                hal_nvic_restore_interrupt_mask(mask);
                return;
            }
            if ((source->param.audio.echo_reference == true) && (hw_current_write_idx_awb == 0)) {
                hal_nvic_restore_interrupt_mask(mask);
                return;
            }

            /* Fill zero packet to prevent UL pop noise (Units:ms) */
            if(runtime->pop_noise_pkt_num < 240) {
                runtime->mute_flag = TRUE;
                runtime->pop_noise_pkt_num += source->param.audio.period;
            } else {
                runtime->mute_flag = FALSE;
            }


            /* For Uplink Clk Skew IRQ period control */
            if(source->param.audio.enable_clk_skew){
                cp_samples = clk_skew_check_ul_status();
            }

            if(source->param.audio.mem_handle.pure_agent_with_src){
                S32 isr_time_samples = clk_skew_isr_time_get_ul_cp_samples();
                if((isr_time_samples >> 2)){
                    cp_samples_ext = isr_time_samples - (isr_time_samples % 4);
                    clk_skew_isr_time_set_ul_cp_samples(isr_time_samples - cp_samples_ext);
                    cp_samples = (cp_samples_ext*(runtime->rate)/(runtime->src_rate));
                }
            }

            #if 0//modify for ab1568
            afe_update_audio_irq_cnt(afe_irq_request_number(AUDIO_DIGITAL_BLOCK_MEM_VUL1),
                                     (uint32_t)((int32_t)runtime->count + (int32_t)cp_samples));
            #else
            hal_audio_memory_irq_period_parameter_t irq_period;
            irq_period.memory_select= HAL_AUDIO_MEMORY_UL_VUL1;
            irq_period.rate = runtime->rate;
            irq_period.irq_counter = (uint32_t)((int32_t)runtime->count + (int32_t)cp_samples);
            if(source->param.audio.mem_handle.pure_agent_with_src){
                SINK sink = source->transform->sink;
                if(sink->type == SINK_TYPE_N9SCO){
                    irq_period.irq_counter *= 3;
                }
            }
            /*if(cp_samples != 0) {
               DSP_MW_LOG_I("[ClkSkew] Vul cp_samples:%d, count:%d, irq_counter:%d", 3, cp_samples, runtime->count, irq_period.irq_counter);
            }*/
            hal_audio_control_set_value((hal_audio_set_value_parameter_t *)&irq_period, HAL_AUDIO_SET_MEMORY_IRQ_PERIOD);
            #endif

            if(source->transform && source->transform->sink)
            {
               SINK sink = source->transform->sink;

               DSP_CALLBACK_PTR callback_ptr;
               callback_ptr = DSP_Callback_Get(source, sink);
#ifdef AIR_BT_CODEC_BLE_ENABLED
                // For UL ISR interval 15ms & ISO interval 10ms case (2*15) = (3*10)
                if ((sink->type == SINK_TYPE_N9BLE && callback_ptr->Status == CALLBACK_HANDLER)||(callback_ptr->IsBusy == TRUE))
                {
                    DSP_MW_LOG_W("BLE UL Callback Busy! wo:%d ro:%d  add:%d", 3, buffer_info->WriteOffset,buffer_info->ReadOffset,(runtime->format_bytes)*(runtime->count));
                    U16 process_frame_num = sink->param.n9ble.process_number;
                    SourceDrop(source,(runtime->format_bytes)*(runtime->count));
                    DSP_MW_LOG_W("BLE UL Callback Busy drop! wo:%d ro:%d", 2, buffer_info->WriteOffset,buffer_info->ReadOffset);
                    sink->param.n9ble.process_number = 2;
                    SinkFlush(sink, sink->param.n9ble.frame_length * 2);
                    sink->param.n9ble.process_number = process_frame_num;
                    N9BLE_setting->N9Ble_sink.N9_Ro_abnormal_cnt = 0;
                }
#endif /*AIR_BT_CODEC_BLE_ENABLED*/
#ifdef MTK_BT_HFP_ENABLE
               if (sink->type == SINK_TYPE_N9SCO && callback_ptr->Status == CALLBACK_HANDLER)
               {
                   DSP_MW_LOG_W("UL Callback busy! source=%d, sink=%d", 2, source->type, sink->type);
                   if (sink->param.n9sco.IsFirstIRQ == TRUE)
                   {
                      SourceDrop(source,(runtime->format_bytes)*(runtime->count));
                   }
                   #if !DL_TRIGGER_UL
                   if ((sink->param.n9sco.IsFirstIRQ == FALSE)&&(callback_ptr->IsBusy == TRUE))
                   {
                      SourceDrop(source,(runtime->format_bytes)*(runtime->count));
                      SinkFlush(sink,sink->param.n9sco.process_data_length);
                   }
                   else
                   {
                      callback_ptr->IsBusy = TRUE;
                   }
                   #else
                   else
                   {
                        SourceDrop(source,(runtime->format_bytes)*(runtime->count));
                        SinkFlush(sink,sink->param.n9sco.process_data_length);
                        N9SCO_setting->N9Sco_sink.N9_Ro_abnormal_cnt = 0;
                   }
                   #endif
               }
#endif
            }
            pre_offset = buffer_info->WriteOffset;//check overflow
            if(source->param.audio.mem_handle.pure_agent_with_src)
            {
                if (data_first_comming){
                    uint32_t prefill_size;
                    //buffer_info->WriteOffset = wptr_vul1_offset; //first update source buffer write offset to make fw move, otherwise path will mute loop
                    prefill_size = ((buffer_info->WriteOffset + buffer_info->length) - buffer_info->ReadOffset) % buffer_info->length;
                    buffer_info->ReadOffset = ((buffer_info->WriteOffset + buffer_info->length) - (runtime->format_bytes * runtime->count) * 2 - ((prefill_size + 16) / 16) * 16) % buffer_info->length;
                    AFE_WRITE(ASM2_CH01_OBUF_RDPNT, AFE_READ(ASM2_OBUF_SADR) + buffer_info->ReadOffset);
                    data_first_comming = false;
                }else{
                    buffer_info->WriteOffset = AFE_GET_REG(ASM2_CH01_OBUF_WRPNT) - AFE_READ(ASM2_OBUF_SADR);//when path work, update source buffer wirte offset with hwsrc2 out buffer write offset
                }
                AFE_WRITE(ASM2_CH01_IBUF_WRPNT, wptr_vul1_offset + AFE_READ(ASM2_IBUF_SADR)); //update HWSRC input buffer write_offset that AFE write hwsrc2 input buffer's
#if 0
                DSP_MW_LOG_W("zlx AFE_IRQ_MCU_CON1 0x%08x, AFE_IRQ_MCU_CON2 0x%08x, AFE_IRQ_MCU_CNT4 0x%08x, AVM ReadOffset %d, WriteOffset %d, HWSRC in buffer RPTR %d WPTR %d, HWSRC out buffer RPTR %d WPTR %d", 9,
                                    AFE_READ(AFE_IRQ_MCU_CON1), AFE_READ(AFE_IRQ_MCU_CON2), AFE_READ(AFE_IRQ_MCU_CNT4),
                                    buffer_info->ReadOffset, buffer_info->WriteOffset,
                                    AFE_READ(ASM2_CH01_IBUF_RDPNT) - AFE_READ(ASM2_IBUF_SADR), AFE_READ(ASM2_CH01_IBUF_WRPNT) - AFE_READ(ASM2_IBUF_SADR),
                                    AFE_READ(ASM2_CH01_OBUF_RDPNT) - AFE_READ(ASM2_OBUF_SADR), AFE_READ(ASM2_CH01_OBUF_WRPNT) - AFE_READ(ASM2_OBUF_SADR));
#endif
            }else{
                buffer_info->WriteOffset = wptr_vul1_offset;
            }
            //printf("vul pre %d wpt %d,rpt %d len %d\r\n",pre_offset,buffer_info->WriteOffset, buffer_info->ReadOffset, buffer_info->length);
            if(runtime->irq_exist){
                if(OFFSET_OVERFLOW_CHK(pre_offset, buffer_info->WriteOffset, buffer_info->ReadOffset)){
                    DSP_MW_LOG_W("UL OFFSET_OVERFLOW ! pre %d,w %d,r %d",3,pre_offset,buffer_info->WriteOffset,buffer_info->ReadOffset);
                    #if 0
                    if (pre_offset < buffer_info->WriteOffset) {
                        buffer_info->ReadOffset = (buffer_info->WriteOffset*2 - pre_offset)% buffer_info->length;
                    } else {
                        buffer_info->ReadOffset = (buffer_info->WriteOffset*2 + (buffer_info->length - pre_offset))% buffer_info->length;
                    }
                    #else
                        buffer_info->ReadOffset = (buffer_info->ReadOffset + (buffer_info->length)/2)%buffer_info->length;
                    #endif
                }
            }

            AudioCheckTransformHandle(source->transform);
            hal_nvic_restore_interrupt_mask(mask);
        }

    }
}

ATTR_TEXT_IN_IRAM void afe_subsource_interrupt_handler(void)
{
#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE)

    SOURCE_TYPE      source_type;
    uint32_t hw_current_write_idx, pre_offset,wptr_vul3_offset;
    static bool data_first_comming = false;
    SOURCE source = NULL;

    AUDIO_PARAMETER *runtime = &source->param.audio;
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;

    for (source_type=SOURCE_TYPE_SUBAUDIO_MIN ; source_type<=SOURCE_TYPE_SUBAUDIO_MAX ; source_type++) {
        source = Source_blks[source_type];

        if ((!source)||(!source->param.audio.is_memory_start) ){
            continue;
        }

        runtime = &source->param.audio;
        buffer_info = &source->streamBuffer.BufferInfo;

        /* First handle */
        if (runtime->irq_exist == false) {
            runtime->irq_exist = true;
            data_first_comming = true;
            uint32_t vul_irq_time;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &vul_irq_time);
            DSP_MW_LOG_I("DSP afe sub-source:%d interrupt exist, time: %d", 2, source_type, vul_irq_time);
            runtime->pop_noise_pkt_num = 0;
            runtime->mute_flag = TRUE;
        }

        /* Fill zero packet to prevent UL pop noise (Units:ms) */
        if(runtime->pop_noise_pkt_num < (240*(source->param.audio.rate/1000))) {
            runtime->mute_flag = TRUE;
            runtime->pop_noise_pkt_num += source->param.audio.count;
        } else {
            runtime->mute_flag = FALSE;
        }

        /* Get current offset */
        hal_audio_memory_selection_t memory_search;
        hal_audio_current_offset_parameter_t get_current_offset;
        for (memory_search=HAL_AUDIO_MEMORY_UL_VUL1 ; memory_search<=HAL_AUDIO_MEMORY_UL_AWB2 ; memory_search<<=1) {
            if (source->param.audio.mem_handle.memory_select&memory_search) {
                break;
            }
        }
        pre_offset = buffer_info->WriteOffset;
        get_current_offset.memory_select = memory_search;
        get_current_offset.pure_agent_with_src = false;
        hw_current_write_idx = hal_audio_get_value((hal_audio_get_value_parameter_t *)&get_current_offset, HAL_AUDIO_GET_MEMORY_INPUT_CURRENT_OFFSET);
        if(hw_current_write_idx) {
            wptr_vul3_offset = hw_current_write_idx - get_current_offset.base_address;

            if(source->param.audio.mem_handle.pure_agent_with_src)
            {
#ifdef AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE
                AFE_WRITE(ASM_CH01_IBUF_WRPNT, wptr_vul3_offset + AFE_READ(ASM_IBUF_SADR)); //update HWSRC input buffer write_offset that AFE write hwsrc2 input buffer's
#else
                AFE_WRITE(ASM2_CH01_IBUF_WRPNT, wptr_vul3_offset + AFE_READ(ASM2_IBUF_SADR)); //update HWSRC input buffer write_offset that AFE write hwsrc2 input buffer's
#endif
                if (data_first_comming){
                    buffer_info->WriteOffset = wptr_vul3_offset; //first update source buffer write offset to make fw move, otherwise path will mute loop
                    data_first_comming = false;
                }else{
#ifdef AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE
                    buffer_info->WriteOffset = AFE_GET_REG(ASM_CH01_OBUF_WRPNT) - AFE_READ(ASM_OBUF_SADR);//when path work, update source buffer wirte offset with hwsrc out buffer write offset
                    if (AFE_GET_REG(ASM_CH01_OBUF_RDPNT) == AFE_GET_REG(ASM_CH01_OBUF_WRPNT)) {
                        buffer_info->bBufferIsFull = TRUE;
                    }
#else
                    buffer_info->WriteOffset = AFE_GET_REG(ASM2_CH01_OBUF_WRPNT) - AFE_READ(ASM2_OBUF_SADR);//when path work, update source buffer wirte offset with hwsrc2 out buffer write offset
                    if (AFE_GET_REG(ASM2_CH01_OBUF_RDPNT) == AFE_GET_REG(ASM2_CH01_OBUF_WRPNT)) {
                        buffer_info->bBufferIsFull = TRUE;
                    }
#endif
                }
            }else{
                buffer_info->WriteOffset = wptr_vul3_offset;
            }
        }
        #if 0
        /* Clock skew */
        hal_audio_memory_irq_period_parameter_t irq_period;
        int16_t cp_sample = 0;
        cp_sample = clk_skew_check_ul_status();

        irq_period.memory_select = HAL_AUDIO_MEMORY_UL_AWB2;//Keep at AWB2 for sub source
        irq_period.rate = Audio_setting->Audio_source.memory.audio_path_rate;
        irq_period.irq_counter = (uint32_t)(Audio_setting->Audio_source.memory.irq_counter + cp_sample);
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&irq_period , HAL_AUDIO_SET_MEMORY_IRQ_PERIOD);
        #endif
        /* overflow check */
        if(OFFSET_OVERFLOW_CHK(pre_offset, buffer_info->WriteOffset, buffer_info->ReadOffset)){
            DSP_MW_LOG_W("DSP Sub-Source:%d OFFSET_OVERFLOW ! pre:0x%x, w:0x%x, r:0x%x", 4, source_type, pre_offset, buffer_info->WriteOffset, buffer_info->ReadOffset);
        }

        /* Stream handler */
        if(source->transform && source->transform->sink)
        {
            AudioCheckTransformHandle(source->transform);
        }
    }
#endif
}

#if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
ATTR_TEXT_IN_IRAM void afe_subsource2_interrupt_handler(void)
{
    SOURCE_TYPE      source_type;
    uint32_t hw_current_write_idx, pre_offset,wptr_vul3_offset;
    static bool data_first_comming = false;
    SOURCE source = NULL;

    for (source_type=SOURCE_TYPE_SUBAUDIO_MIN ; source_type<=SOURCE_TYPE_SUBAUDIO_MAX ; source_type++) {
        source = Source_blks[source_type];
        if ((source) && (source->param.audio.mem_handle.memory_select&HAL_AUDIO_MEMORY_UL_AWB)){
            break;
        }
        source = NULL;
    }
    if (!source) {
        DSP_MW_LOG_E("subsource not found", 0);
        assert(0);
    }

    AUDIO_PARAMETER *runtime = &source->param.audio;
    BUFFER_INFO *buffer_info = &source->streamBuffer.BufferInfo;

    /* First handle */
    if (runtime->irq_exist == false) {
        runtime->irq_exist = true;
        data_first_comming = true;
        uint32_t vul_irq_time;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &vul_irq_time);
        DSP_MW_LOG_I("DSP afe sub-source2:%d interrupt exist, time: %d", 2, source_type, vul_irq_time);
        runtime->pop_noise_pkt_num = 0;
        runtime->mute_flag = TRUE;
    }

    /* Fill zero packet to prevent UL pop noise (Units:ms) */
    if(runtime->pop_noise_pkt_num < (240*(source->param.audio.rate/1000))) {
        runtime->mute_flag = TRUE;
        runtime->pop_noise_pkt_num += source->param.audio.count;
    } else {
        runtime->mute_flag = FALSE;
    }

    /* Get current offset */

    pre_offset = buffer_info->WriteOffset;

    hw_current_write_idx = AFE_GET_REG(AFE_AWB_CUR);

    if(hw_current_write_idx) {
        wptr_vul3_offset = hw_current_write_idx - AFE_GET_REG(AFE_AWB_BASE);

        if(source->param.audio.mem_handle.pure_agent_with_src)
        {
#ifdef AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE
            AFE_WRITE(ASM_CH01_IBUF_WRPNT, wptr_vul3_offset + AFE_READ(ASM_IBUF_SADR)); //update HWSRC input buffer write_offset that AFE write hwsrc2 input buffer's
#else
            AFE_WRITE(ASM2_CH01_IBUF_WRPNT, wptr_vul3_offset + AFE_READ(ASM2_IBUF_SADR)); //update HWSRC input buffer write_offset that AFE write hwsrc2 input buffer's
#endif
            if (data_first_comming){
                buffer_info->WriteOffset = wptr_vul3_offset; //first update source buffer write offset to make fw move, otherwise path will mute loop
                data_first_comming = false;
            }else{
#ifdef AIR_DUAL_CHIP_MASTER_HWSRC_RX_TRACKING_ENABLE
                buffer_info->WriteOffset = AFE_GET_REG(ASM_CH01_OBUF_WRPNT) - AFE_READ(ASM_OBUF_SADR);//when path work, update source buffer wirte offset with hwsrc out buffer write offset
                if (AFE_GET_REG(ASM_CH01_OBUF_RDPNT) == AFE_GET_REG(ASM_CH01_OBUF_WRPNT)) {
                    buffer_info->bBufferIsFull = TRUE;
                }
#else
                buffer_info->WriteOffset = AFE_GET_REG(ASM2_CH01_OBUF_WRPNT) - AFE_READ(ASM2_OBUF_SADR);//when path work, update source buffer wirte offset with hwsrc2 out buffer write offset
                if (AFE_GET_REG(ASM2_CH01_OBUF_RDPNT) == AFE_GET_REG(ASM2_CH01_OBUF_WRPNT)) {
                    buffer_info->bBufferIsFull = TRUE;
                }
#endif
            }
        }else{
            buffer_info->WriteOffset = wptr_vul3_offset;
        }
    }

    /* overflow check */
    if(OFFSET_OVERFLOW_CHK(pre_offset, buffer_info->WriteOffset, buffer_info->ReadOffset)){
        DSP_MW_LOG_W("DSP Sub-Source2 OFFSET_OVERFLOW ! pre:0x%x, w:0x%x, r:0x%x", 3, pre_offset, buffer_info->WriteOffset, buffer_info->ReadOffset);
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
        buffer_info->ReadOffset = (buffer_info->ReadOffset + (buffer_info->length)/2)%buffer_info->length;
#endif
    }

    /* Stream handler */
    if(source->transform && source->transform->sink)
    {
        AudioCheckTransformHandle(source->transform);
    }
}
#endif

#ifdef MTK_PROMPT_SOUND_ENABLE
volatile uint32_t vp_sram_empty_flag = 0;
extern volatile uint32_t vp_config_flag;
void afe_dl2_interrupt_handler(void)
{
    //printf("afe_dl2_interrupt_handler\r\n");
    uint32_t mask,pre_offset ;
    int32_t hw_current_read_idx = 0;
    volatile SINK sink = Sink_blks[SINK_TYPE_VP_AUDIO];
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    uint32_t dl_base_addr;
    AUDIO_PARAMETER *runtime = &sink->param.audio;

    #if 0//modify for ab1568
    if (afe_block->u4asrcflag) {
    #else
    if (sink->param.audio.mem_handle.pure_agent_with_src) {
    #endif
        if (runtime->irq_exist == false) {
            runtime->irq_exist = true;
        }
        dl_base_addr = AFE_GET_REG(ASM2_IBUF_SADR);
        hw_current_read_idx = AFE_GET_REG(ASM2_CH01_IBUF_RDPNT);
    } else {
        dl_base_addr = AFE_GET_REG(AFE_DL2_BASE);
        hw_current_read_idx = AFE_GET_REG(AFE_DL2_CUR);
    }


    //printf("AFE_DL2_CUR:%x\r\n", hw_current_read_idx);
    //printf("AFE_DL2_BASE:%x\r\n", AFE_GET_REG(AFE_DL2_BASE));
    //printf("addr:%x, value is %x\r\n", hw_current_read_idx, (*(volatile uint32_t *)hw_current_read_idx));

    //if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL2))//modify for ab1568
    if (1)//modify for ab1568
    {

        hal_nvic_save_and_set_interrupt_mask(&mask);
        if (hw_current_read_idx == 0) { //should chk setting if =0
            hw_current_read_idx = word_size_align((S32) dl_base_addr);
        }
        pre_offset = buffer_info->ReadOffset;
        buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
        if (dl_base_addr != NULL){ //Prevent to access null pointer when the last isr is executed after HW is turned off and pointer is cleared
            /*Clear up last time used memory */
            if (buffer_info->ReadOffset >= pre_offset) {
                memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->ReadOffset - pre_offset);
            } else {
                memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->length - pre_offset);
                memset((void *)dl_base_addr, 0, buffer_info->ReadOffset);
            }

            /*Clear up last time used memory */
            if (buffer_info->ReadOffset >= pre_offset) {
                memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->ReadOffset - pre_offset);
            } else {
                memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->length - pre_offset);
                memset((void *)(dl_base_addr), 0, buffer_info->ReadOffset);
            }
        }
        if ((OFFSET_OVERFLOW_CHK(pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset)&&(buffer_info->bBufferIsFull==FALSE))
            || (((buffer_info->WriteOffset+buffer_info->length-buffer_info->ReadOffset)%buffer_info->length < 32) && (afe_block->u4asrcflag)))
        { // SRAM Empty
            bool empty = true;
            if(afe_block->u4asrcflag) {
                uint32_t src_out_size, src_out_read, src_out_write, remain_data;
                src_out_write = AFE_READ(ASM2_CH01_OBUF_WRPNT);
                src_out_read = AFE_READ(ASM2_CH01_OBUF_RDPNT);
                src_out_size = AFE_READ(ASM2_OBUF_SIZE);
                remain_data = (src_out_write > src_out_read) ? src_out_write-src_out_read : src_out_write+src_out_size- src_out_read;
                remain_data = (sink->param.audio.channel_num>=2) ? remain_data>>1 : remain_data;
                if (remain_data > (afe_block->u4asrcrate*sink->param.audio.period*sink->param.audio.format_bytes)/1000) {
                    empty = false;
                }
            }
            if (empty) {
                DSP_MW_LOG_W("SRAM Empty play pR:%d R:%d W:%d==============", 3,pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);

                #if 0//modify for ab1568
                if (afe_block->u4asrcflag) {
                #else
                if (sink->param.audio.mem_handle.pure_agent_with_src) {
                #endif
                    buffer_info->WriteOffset = (buffer_info->ReadOffset + buffer_info->length/2) % buffer_info->length;
                } else {
                    if (pre_offset < buffer_info->ReadOffset) {
                        buffer_info->WriteOffset = (buffer_info->ReadOffset*2 - pre_offset)% buffer_info->length;
                    } else {
                        buffer_info->WriteOffset = (buffer_info->ReadOffset*2 + (buffer_info->length - pre_offset))% buffer_info->length;
                    }
                }


                if ((dl_base_addr != NULL) && (hw_current_read_idx != NULL) ){ //Prevent to access null pointer when the last isr is executed after HW is turned off and pointer is cleared
                    if (buffer_info->WriteOffset > buffer_info->ReadOffset) {
                        memset((void *)hw_current_read_idx, 0, buffer_info->WriteOffset - buffer_info->ReadOffset);
                    } else {
                        memset((void *)hw_current_read_idx, 0, buffer_info->length - buffer_info->ReadOffset);
                        memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);
                    }
                }
                if (vp_config_flag == 1) {
                    vp_sram_empty_flag = 1;
                    xTaskResumeFromISR((TaskHandle_t)pDPR_TaskHandler);
                    portYIELD_FROM_ISR(pdTRUE); // force to do context switch
                }

            }
        } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
            buffer_info->bBufferIsFull = FALSE;
        }
#if 0
        if (afe_block->u4ReadIdx > afe_block->u4WriteIdx) {
            sram_free_space = afe_block->u4ReadIdx - afe_block->u4WriteIdx;
        } else {
            sram_free_space = afe_block->u4BufferSize + afe_block->u4ReadIdx - afe_block->u4WriteIdx;
        }
        sram_free_space = word_size_align(sram_free_space);// 4-byte alignment
        if (channel_type == STREAM_S_AFE_S){
            copy_size = MINIMUM(sram_free_space >> 1, remain_sink_data);
            afe_block->u4bufferfullflag = (sram_free_space == copy_size << 1) ? TRUE : FALSE;
            sram_free_space = copy_size << 1;
        } else {
            copy_size = MINIMUM(sram_free_space, remain_sink_data);
            afe_block->u4bufferfullflag = (sram_free_space == copy_size) ? TRUE : FALSE;
            sram_free_space = copy_size;
        }


        #if 0
        if (afe_block->u4WriteIdx + sram_free_space < afe_block->u4BufferSize) {
            audio_split_sink_to_interleaved(sink, (dl_base_addr + afe_block->u4WriteIdx), 0, sram_free_space);
        } else {
            uint32_t size_1 = 0, size_2 = 0;
            size_1 = word_size_align((afe_block->u4BufferSize - afe_block->u4WriteIdx));
            size_2 = word_size_align((sram_free_space - size_1));
            audio_split_sink_to_interleaved(sink, (dl_base_addr + afe_block->u4WriteIdx), 0, size_1);
            audio_split_sink_to_interleaved(sink, (dl_base_addr), size_1, size_2);
        }
        #else
        audio_afe_sink_interleaved(sink, sram_free_space);
        #endif

        afe_block->u4WriteIdx = (afe_block->u4WriteIdx + sram_free_space)% afe_block->u4BufferSize;
        SinkBufferUpdateReadPtr(sink, copy_size);
        hal_nvic_restore_interrupt_mask(mask);
        #endif
        AudioCheckTransformHandle(sink->transform);
        hal_nvic_restore_interrupt_mask(mask);
    }
    if (afe_block->u4asrcflag) {
        AFE_WRITE(ASM2_CH01_IBUF_WRPNT, buffer_info->WriteOffset + AFE_READ(ASM2_IBUF_SADR));
    }

}

void stream_audio_src2_interrupt(void)
{
    uint32_t output_write_pointer, input_read_pointer, input_read_offset;
    hal_audio_get_value_parameter_t get_value_parameter;
    hal_audio_set_value_parameter_t set_value_parameter;
    SINK sink = Sink_blks[SINK_TYPE_VP_AUDIO];
    uint32_t mask,pre_offset ;
    int32_t hw_current_read_idx = 0;
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    uint32_t dl_base_addr;
    get_value_parameter.get_current_offset.memory_select = HAL_AUDIO_MEMORY_DL_SRC2;
    input_read_pointer = hal_audio_get_value(&get_value_parameter, HAL_AUDIO_GET_MEMORY_INPUT_CURRENT_OFFSET);
    output_write_pointer = hal_audio_get_value(&get_value_parameter, HAL_AUDIO_GET_MEMORY_OUTPUT_CURRENT_OFFSET);

    if (!sink) {
        return;
    }
    if (!sink->streamBuffer.BufferInfo.startaddr[0])
    {
        return;
    }

    input_read_offset = input_read_pointer - (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
    hw_current_read_idx = input_read_offset;
    dl_base_addr = (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
    #if 1
    //stream_audio_sink_handler(sink->type, input_read_offset);
    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (hw_current_read_idx == 0) { //should chk setting if =0
        hw_current_read_idx = word_size_align((S32) dl_base_addr);
    }
    pre_offset = buffer_info->ReadOffset;
    buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
    /*Clear up last time used memory */
    if (buffer_info->ReadOffset >= pre_offset) {
        memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->ReadOffset - pre_offset);
    } else {
        memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->length - pre_offset);
        memset((void *)dl_base_addr, 0, buffer_info->ReadOffset);
    }

    /*Clear up last time used memory */
    if (buffer_info->ReadOffset >= pre_offset) {
        memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->ReadOffset - pre_offset);
    } else {
        memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->length - pre_offset);
        memset((void *)(dl_base_addr), 0, buffer_info->ReadOffset);
    }

    if ((OFFSET_OVERFLOW_CHK(pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset)&&(buffer_info->bBufferIsFull==FALSE)))
    { // SRAM Empty
        bool empty = true;

        if (empty) {
            DSP_MW_LOG_W("SRAM Empty play pR:%d R:%d W:%d==============", 3,pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);
            if (pre_offset < buffer_info->ReadOffset) {
                buffer_info->WriteOffset = (buffer_info->ReadOffset*2 - pre_offset)% buffer_info->length;
            } else {
                buffer_info->WriteOffset = (buffer_info->ReadOffset*2 + (buffer_info->length - pre_offset))% buffer_info->length;
            }

            if (buffer_info->WriteOffset > buffer_info->ReadOffset) {
                memset((void *)hw_current_read_idx, 0, buffer_info->WriteOffset - buffer_info->ReadOffset);
            } else {
                memset((void *)hw_current_read_idx, 0, buffer_info->length - buffer_info->ReadOffset);
                memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);
            }

            if (vp_config_flag == 1) {
                vp_sram_empty_flag = 1;
                xTaskResumeFromISR((TaskHandle_t)pDPR_TaskHandler);
                portYIELD_FROM_ISR(pdTRUE); // force to do context switch
            }

        }
    } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
        buffer_info->bBufferIsFull = FALSE;
    }
    #else
    //Verification code
    sink->streamBuffer.BufferInfo.ReadOffset = input_read_offset;
    sink->streamBuffer.BufferInfo.WriteOffset = (sink->streamBuffer.BufferInfo.length + sink->streamBuffer.BufferInfo.ReadOffset - 16)%sink->streamBuffer.BufferInfo.length;
    #endif

    set_value_parameter.set_current_offset.memory_select = HAL_AUDIO_MEMORY_DL_SRC2;
    set_value_parameter.set_current_offset.offset = sink->streamBuffer.BufferInfo.WriteOffset + (uint32_t)sink->streamBuffer.BufferInfo.startaddr[0];
    hal_audio_set_value(&set_value_parameter, HAL_AUDIO_SET_SRC_INPUT_CURRENT_OFFSET);

    AudioCheckTransformHandle(sink->transform);
    hal_nvic_restore_interrupt_mask(mask);
}
#endif

void afe_asrc_interrupt_handler(afe_mem_asrc_id_t asrc_id)
{
    uint32_t irq_status = afe_get_asrc_irq_status(asrc_id);

    if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL1) && asrc_id == AFE_MEM_ASRC_1) {

        if (afe_get_asrc_irq_is_enabled(asrc_id, ASM_IER_IBUF_EMPTY_INTEN_MASK) && (irq_status&ASM_IFR_IBUF_EMPTY_INT_MASK)) {

        }

        if (afe_get_asrc_irq_is_enabled(asrc_id, ASM_IER_IBUF_AMOUNT_INTEN_MASK) && (irq_status&ASM_IFR_IBUF_AMOUNT_INT_MASK)) {

        }

        if (afe_get_asrc_irq_is_enabled(asrc_id, ASM_IER_OBUF_OV_INTEN_MASK) && (irq_status&ASM_IFR_OBUF_OV_INT_MASK)) {

        }

        if (afe_get_asrc_irq_is_enabled(asrc_id, ASM_IER_OBUF_AMOUNT_INTEN_MASK) && (irq_status&ASM_IFR_OBUF_AMOUNT_INT_MASK)) {

        }
    } else if (afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL2) && asrc_id == AFE_MEM_ASRC_2) {
        if (afe_get_asrc_irq_is_enabled(asrc_id, ASM_IER_IBUF_EMPTY_INTEN_MASK) && (irq_status&ASM_IFR_IBUF_EMPTY_INT_MASK)) {

        }

        if (afe_get_asrc_irq_is_enabled(asrc_id, ASM_IER_IBUF_AMOUNT_INTEN_MASK) && (irq_status&ASM_IFR_IBUF_AMOUNT_INT_MASK)) {

        }

        if (afe_get_asrc_irq_is_enabled(asrc_id, ASM_IER_OBUF_OV_INTEN_MASK) && (irq_status&ASM_IFR_OBUF_OV_INT_MASK)) {

        }

        if (afe_get_asrc_irq_is_enabled(asrc_id, ASM_IER_OBUF_AMOUNT_INTEN_MASK) && (irq_status&ASM_IFR_OBUF_AMOUNT_INT_MASK)) {
            #ifdef MTK_PROMPT_SOUND_ENABLE
                afe_dl2_interrupt_handler();
            #endif
        }
    }
}

void afe_dl2_query_data_amount(uint32_t *sink_data_count, uint32_t *afe_sram_data_count)
{
    volatile SINK sink = Sink_blks[SINK_TYPE_VP_AUDIO];
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    int32_t hw_current_read_idx = AFE_GET_REG(AFE_DL2_CUR);
    afe_block->u4ReadIdx = hw_current_read_idx - AFE_GET_REG(AFE_DL2_BASE);

    //AFE DL2 SRAM data amount
    if (afe_block->u4WriteIdx > afe_block->u4ReadIdx) {
        *afe_sram_data_count = afe_block->u4WriteIdx - afe_block->u4ReadIdx;
    } else {
        *afe_sram_data_count = afe_block->u4BufferSize + afe_block->u4WriteIdx - afe_block->u4ReadIdx;
    }

    //Sink audio data amount
    *sink_data_count = sink->streamBuffer.BufferInfo.length - SinkSlack(sink);
}

#if defined (AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE) || defined (AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE)
void afe_dl3_interrupt_handler(void){
    //DSP_MW_LOG_I("dl3 interrupt start\r\n", 0);
    uint32_t mask = 0;
    uint32_t pre_offset = 0;
    int32_t  hw_current_read_idx = 0;
    uint32_t dl_base_addr = 0;
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO_DL3];
    //afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    AUDIO_PARAMETER *runtime = &sink->param.audio;
    dl_base_addr = AFE_GET_REG(AFE_DL3_BASE);
    hw_current_read_idx = AFE_GET_REG(AFE_DL3_CUR);
    if (runtime->irq_exist == false) {
        runtime->irq_exist = true;
        DSP_MW_LOG_I("DSP afe dl3 interrupt start\r\n", 0);
    }
    if (1) {//afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL3)
        //DSP_MW_LOG_I("TEST TT\r\n", 0);
        hal_nvic_save_and_set_interrupt_mask(&mask);
        if (hw_current_read_idx == 0) { //should chk setting if =0
            hw_current_read_idx = word_size_align((S32) dl_base_addr);
        }
        pre_offset = buffer_info->ReadOffset;
        buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        if ((sink->param.audio.scenario_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH) && (sink->param.audio.scenario_sub_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID))
        {
            /* In this case, we do nothing to reduce mips. */
        }
        else
        #endif /* defined(AIR_ADVANCED_PASSTHROUGH_ENABLE) */
        /*Clear up last time used memory */
        {
            if (dl_base_addr != NULL) {
                if (buffer_info->ReadOffset >= pre_offset) {
                    memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->ReadOffset - pre_offset);
                } else {
                    memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->length - pre_offset);
                    memset((void *)dl_base_addr, 0, buffer_info->ReadOffset);
                }
            }
        }

        if ((OFFSET_OVERFLOW_CHK(pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset)&&(buffer_info->bBufferIsFull==FALSE)))
        { // SRAM Empty
            bool empty = true;

            if (empty) {
                DSP_MW_LOG_W("DL3 SRAM Empty play pR:%d R:%d W:%d==============", 3,pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);

                if (pre_offset < buffer_info->ReadOffset) {
                    buffer_info->WriteOffset = (buffer_info->ReadOffset*2 - pre_offset)% buffer_info->length;
                } else {
                    buffer_info->WriteOffset = (buffer_info->ReadOffset*2 + (buffer_info->length - pre_offset))% buffer_info->length;
                }
                if (buffer_info->WriteOffset > buffer_info->ReadOffset) {
                    memset((void *)hw_current_read_idx, 0, buffer_info->WriteOffset - buffer_info->ReadOffset);
                } else {
                    memset((void *)hw_current_read_idx, 0, buffer_info->length - buffer_info->ReadOffset);
                    memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);
                }
            }
        } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
            buffer_info->bBufferIsFull = FALSE;
        }

#if defined (AIR_WIRED_AUDIO_ENABLE)
        if((sink->transform != NULL)
            && (sink->transform->source->type >=SOURCE_TYPE_AUDIO_TRANSMITTER_MIN)&&(sink->transform->source->type <=SOURCE_TYPE_AUDIO_TRANSMITTER_MAX)
            && (sink->transform->source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO)
            && ((sink->transform->source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)||(sink->transform->source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))) {
            sink->transform->source->param.data_dl.scenario_param.usb_in_local_param.is_afe_irq_comming = true;
            sink->transform->source->param.data_dl.scenario_param.usb_in_local_param.is_dummy_data = false;
        }
#endif

        #if defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
        if ((sink->param.audio.scenario_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH) && (sink->param.audio.scenario_sub_id == AUDIO_TRANSMITTER_ADVANCED_PASSTHROUGH_HEARING_AID))
        {
            /* In this case, the stream must be trigger by AFE source irq. So we do nothing here. */
        }
        else
        #endif /* defined(AIR_ADVANCED_PASSTHROUGH_ENABLE) */
        {
            AudioCheckTransformHandle(sink->transform);
        }

#if defined (AIR_WIRED_AUDIO_ENABLE)
        volatile SINK sink2 = Sink_blks[SINK_TYPE_DSP_VIRTUAL];
        if((sink2 != NULL)&&(sink2->transform != NULL)
            && (sink2->transform->source->type >=SOURCE_TYPE_AUDIO_TRANSMITTER_MIN)&&(sink2->transform->source->type <=SOURCE_TYPE_AUDIO_TRANSMITTER_MAX)
            && (sink2->transform->source->param.data_dl.scenario_type == AUDIO_TRANSMITTER_WIRED_AUDIO)
            && ((sink2->transform->source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)||(sink2->transform->source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1))) {
           AudioCheckTransformHandle(sink2->transform);
        }
#endif
        hal_nvic_restore_interrupt_mask(mask);
    }
}
#endif

#if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE)
void afe_dl12_interrupt_handler(void){
    // uint32_t gpt_count;
    // hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_count);
    // DSP_MW_LOG_I("dl12 interrupt, %u", 1, gpt_count);
    uint32_t mask = 0;
    uint32_t pre_offset = 0;
    int32_t  hw_current_read_idx = 0;
    uint32_t dl_base_addr = 0;
    volatile SINK sink = Sink_blks[SINK_TYPE_AUDIO_DL12];
    //afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    BUFFER_INFO *buffer_info = &sink->streamBuffer.BufferInfo;
    AUDIO_PARAMETER *runtime = &sink->param.audio;
    dl_base_addr = AFE_GET_REG(AFE_DL12_BASE);
    hw_current_read_idx = AFE_GET_REG(AFE_DL12_CUR);
    if (runtime->irq_exist == false) {
        runtime->irq_exist = true;
        DSP_MW_LOG_I("DSP afe dl12 interrupt start\r\n", 0);
    }
    if (1) {//afe_get_memory_path_enable(AUDIO_DIGITAL_BLOCK_MEM_DL12)
        //DSP_MW_LOG_I("TEST TT\r\n", 0);
        hal_nvic_save_and_set_interrupt_mask(&mask);
        if (hw_current_read_idx == 0) { //should chk setting if =0
            hw_current_read_idx = word_size_align((S32) dl_base_addr);
        }
        pre_offset = buffer_info->ReadOffset;
        buffer_info->ReadOffset = hw_current_read_idx - dl_base_addr;
        /*Clear up last time used memory */
        if (sink->type != SINK_TYPE_AUDIO_DL12) {
            if (buffer_info->ReadOffset >= pre_offset) {
                memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->ReadOffset - pre_offset);
            } else {
                memset((void *)(dl_base_addr+pre_offset), 0, buffer_info->length - pre_offset);
                memset((void *)dl_base_addr, 0, buffer_info->ReadOffset);
            }
        }
        if ((OFFSET_OVERFLOW_CHK(pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset)&&(buffer_info->bBufferIsFull==FALSE)))
        { // SRAM Empty
            bool empty = true;

            if (empty) {
                DSP_MW_LOG_W("DL12 SRAM Empty play pR:%d R:%d W:%d==============", 3,pre_offset, buffer_info->ReadOffset, buffer_info->WriteOffset);

                if (pre_offset < buffer_info->ReadOffset) {
                    buffer_info->WriteOffset = (buffer_info->ReadOffset*2 - pre_offset)% buffer_info->length;
                } else {
                    buffer_info->WriteOffset = (buffer_info->ReadOffset*2 + (buffer_info->length - pre_offset))% buffer_info->length;
                }
                if (buffer_info->WriteOffset > buffer_info->ReadOffset) {
                    memset((void *)hw_current_read_idx, 0, buffer_info->WriteOffset - buffer_info->ReadOffset);
                } else {
                    memset((void *)hw_current_read_idx, 0, buffer_info->length - buffer_info->ReadOffset);
                    memset((void *)dl_base_addr, 0, buffer_info->WriteOffset);
                }
            }
        } else if (buffer_info->ReadOffset != buffer_info->WriteOffset) {
            buffer_info->bBufferIsFull = FALSE;
        }

        AudioCheckTransformHandle(sink->transform);
        hal_nvic_restore_interrupt_mask(mask);
    }
}
#endif

int32_t afe_set_mem_block(afe_stream_type_t type, audio_digital_block_t mem_blk)
{
    afe_block_t *afe_block;
    AUDIO_PARAMETER *audio_para;
    uint32_t write_index, read_index;
    if (type == AFE_SINK) {
        afe_block = &Sink_blks[SINK_TYPE_AUDIO]->param.audio.AfeBlkControl;
        audio_para = &Sink_blks[SINK_TYPE_AUDIO]->param.audio;
        write_index = 0x800;
        read_index = 0;
    } else if (type == AFE_SINK_VP){
        afe_block = &Sink_blks[SINK_TYPE_VP_AUDIO]->param.audio.AfeBlkControl;
        audio_para = &Sink_blks[SINK_TYPE_VP_AUDIO]->param.audio;
        write_index = audio_para->buffer_size*3/4;
        read_index = 0;
    } else {
        afe_block = &Source_blks[SOURCE_TYPE_AUDIO]->param.audio.AfeBlkControl;
        audio_para = &Source_blks[SOURCE_TYPE_AUDIO]->param.audio;
        write_index = 0;
        read_index = 960;
    }

    afe_block->u4BufferSize = audio_para->buffer_size;  // [Tochk] w/ channel, unit: bytes
    //afe_block->pSramBufAddr = (uint8_t *)((volatile uint32_t *)afe_block->phys_buffer_addr);
    afe_block->u4SampleNumMask = 0x001f; /* 32 byte align */
    afe_block->u4WriteIdx = (write_index)%afe_block->u4BufferSize;
    afe_block->u4ReadIdx = (read_index)%afe_block->u4BufferSize;

    afe_block->u4DataRemained = 0;

    afe_set_mem_block_addr(mem_blk, afe_block);
    return 0;
}

void afe_free_audio_sram(afe_pcm_format_t type)
{
    uint32_t i = 0, mask;
    afe_block_t *afe_block;
    afe_sram_block_t *sram_block = NULL;

    if (type == AFE_SINK) {
        afe_block = &Sink_blks[SINK_TYPE_AUDIO]->param.audio.AfeBlkControl;
#ifdef MTK_PROMPT_SOUND_ENABLE
    } else if (type == AFE_SINK_VP) {
        afe_block = &Sink_blks[SINK_TYPE_VP_AUDIO]->param.audio.AfeBlkControl;
        return;
#endif
    } else {
        afe_block = &Source_blks[SOURCE_TYPE_AUDIO]->param.audio.AfeBlkControl;
    }

    hal_nvic_save_and_set_interrupt_mask(&mask);

    for (i = 0; i < audio_sram_manager.mBlocknum; i++) {
        sram_block = &audio_sram_manager.mAud_sram_block[i];
        if (sram_block->mUser == afe_block) {
            sram_block->mUser = NULL;
        }
    }
    hal_nvic_restore_interrupt_mask(mask);
}

hal_audio_irq_callback_function_t afe_irq_ops = {
    .afe_dl1_interrupt_handler     = afe_dl1_interrupt_handler,
#ifdef MTK_PROMPT_SOUND_ENABLE
    .afe_dl2_interrupt_handler     = afe_dl2_interrupt_handler,
#else
    .afe_dl2_interrupt_handler     = NULL,
#endif
#if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE) || defined (AIR_PROMPT_SOUND_DUMMY_SOURCE_ENABLE)
    .afe_dl3_interrupt_handler     = afe_dl3_interrupt_handler, // ab156x won't use it, ab155x will use it
#else
    .afe_dl3_interrupt_handler     = NULL,
#endif
    .afe_vul1_interrupt_handler    = afe_vul1_interrupt_handler,
    .afe_vul2_interrupt_handler    = NULL,
    .afe_awb_interrupt_handler     = NULL,
    .afe_awb2_interrupt_handler    = NULL,
#if defined (AIR_WIRED_AUDIO_ENABLE) || defined (AIR_ADVANCED_PASSTHROUGH_ENABLE)
    .afe_dl12_interrupt_handler    = afe_dl12_interrupt_handler,
#else
    .afe_dl12_interrupt_handler    = NULL,
#endif
#ifdef MTK_ANC_ENABLE
    .afe_anc_pwrdet_interrupt_handler = afe_anc_pwrdet_interrupt_handler,
#else
    .afe_anc_pwrdet_interrupt_handler = NULL,
#endif
};

/*Hook AFE IRQ callback by user's implementation.*/
void afe_register_irq_ops(void)
{
    hal_audio_afe_register_irq_callback(&afe_irq_ops);

    afe_register_asrc_irq_callback_function(afe_asrc_interrupt_handler);

}


void afe_send_amp_status_ccni(bool enable)
{
    hal_ccni_message_t msg;
    uint32_t status;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = ((MSG_DSP2MCU_AUDIO_AMP << 16) | enable);
    status = aud_msg_tx_handler(msg, 0, FALSE);

    DSP_MW_LOG_I("DSP AMP afe_send_amp_status_ccni:%d \r\n", 1, enable);
}

#ifdef AIR_SILENCE_DETECTION_ENABLE
void afe_send_silence_status_ccni(bool SilenceFlag)
{
    hal_ccni_message_t msg;
    uint32_t status;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = ((MSG_DSP2MCU_BT_AUDIO_DL_SILENCE_DETECTION_FEEDBACK << 16) | SilenceFlag);
    status = aud_msg_tx_handler(msg, 0, FALSE);
    DSP_MW_LOG_I("[SD]Silence Detection afe_send_silence_status_ccni:%d \r\n", 1, SilenceFlag);
}
#endif

#ifdef ENABLE_AMP_TIMER

bool afe_amp_open_handler(uint32_t samplerate)
{
#if 0
    return true;
#else
    bool reboot_dac;
    reboot_dac = fw_amp_timer_stop(samplerate);
    afe_send_amp_status_ccni(true);
    return reboot_dac;
#endif
}

bool afe_amp_closure_handler(void)
{
#if 0
    return true;
#else
    if (fw_amp_get_status() == FW_AMP_TIMER_END) {
        fw_amp_set_status(FW_AMP_TIMER_STOP);
        return true;
    } else {
        //Set amp timer
        fw_amp_timer_start();
        return false;
    }
#endif
}

hal_amp_function_t afe_amp_ops = {
    .open_handler       = afe_amp_open_handler,
    .closure_handler    = afe_amp_closure_handler,
};

/*Hook AFE Amp handler.*/
void afe_register_amp_handler(void)
{
    fw_amp_init_semaphore();
    fw_amp_init_timer();
    hal_audio_afe_register_amp_handle(&afe_amp_ops);
}
#endif

/*Calculate greatest common factor*/
uint32_t audio_get_gcd(uint32_t m, uint32_t n)
{
    while(n != 0) {
        uint32_t r = m % n;
        m = n;
        n = r;
    }
    return m;
}

void afe_set_asrc_ul_configuration_parameters(SOURCE source, afe_asrc_config_p asrc_config)
{
    UNUSED(source);
    UNUSED(asrc_config);
    #if 1//modify for ab1568
    uint32_t device_rate;
    asrc_config->ul_mode = true;
    asrc_config->stereo = (source->param.audio.channel_num>=2);
    asrc_config->hw_update_obuf_rdpnt = false;

    if (source->param.audio.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE)
    {
        asrc_config->tracking_mode = MEM_ASRC_TRACKING_MODE_RX;
        asrc_config->tracking_clock = afe_set_asrc_tracking_clock(source->param.audio.audio_interface);
        device_rate = source->param.audio.rate;
    }
    else
    {
        asrc_config->tracking_mode = MEM_ASRC_NO_TRACKING;
        device_rate = afe_get_audio_device_samplerate(source->param.audio.audio_device, source->param.audio.audio_interface);
    }

    asrc_config->input_buffer.addr = source->param.audio.AfeBlkControl.phys_buffer_addr;
    asrc_config->input_buffer.size = source->param.audio.AfeBlkControl.u4asrc_buffer_size;
    asrc_config->input_buffer.rate = device_rate;
    asrc_config->input_buffer.offset = 32;////((((source->param.audio.period+5)*source->param.audio.format_bytes*asrc_config->input_buffer.rate*((asrc_config->stereo==true) ? 2 : 1)/1000)+ 7) & (~7))%asrc_config->input_buffer.size;
    asrc_config->input_buffer.format = source->param.audio.format;

    asrc_config->output_buffer.addr = source->param.audio.AfeBlkControl.phys_buffer_addr+source->param.audio.AfeBlkControl.u4asrc_buffer_size;
    asrc_config->output_buffer.size = source->param.audio.buffer_size;
    asrc_config->output_buffer.rate = source->param.audio.src_rate;
    asrc_config->output_buffer.offset = source->streamBuffer.BufferInfo.ReadOffset;
    asrc_config->output_buffer.format = source->param.audio.format;

    DSP_MW_LOG_I("DSP asrc in rate:%d, out rate:%d\r\n",2, asrc_config->input_buffer.rate, asrc_config->output_buffer.rate);
    #endif
}

void afe_set_asrc_dl_configuration_parameters(SINK sink, afe_asrc_config_p asrc_config)
{
    asrc_config->ul_mode = false;
    asrc_config->tracking_mode = MEM_ASRC_NO_TRACKING;
    asrc_config->stereo = (sink->param.audio.channel_num>=2);
    asrc_config->hw_update_obuf_rdpnt = true;


    asrc_config->input_buffer.addr = sink->param.audio.AfeBlkControl.phys_buffer_addr+sink->param.audio.AfeBlkControl.u4asrc_buffer_size;
    asrc_config->input_buffer.size = sink->param.audio.buffer_size;
    asrc_config->input_buffer.rate = sink->param.audio.src_rate;
    asrc_config->input_buffer.offset = sink->streamBuffer.BufferInfo.WriteOffset;
    asrc_config->input_buffer.format = sink->param.audio.format;

    asrc_config->output_buffer.addr = sink->param.audio.AfeBlkControl.phys_buffer_addr;
    asrc_config->output_buffer.size = sink->param.audio.AfeBlkControl.u4asrc_buffer_size;
    asrc_config->output_buffer.rate = afe_get_audio_device_samplerate(sink->param.audio.audio_device, sink->param.audio.audio_interface);
    //asrc_config->output_buffer.offset = ((((sink->param.audio.period+5)*sink->param.audio.format_bytes*asrc_config->output_buffer.rate*((asrc_config->stereo==true) ? 2 : 1)/1000)+ 16 +7)& ~7 )%asrc_config->output_buffer.size ;
    asrc_config->output_buffer.format = sink->param.audio.format;


    DSP_MW_LOG_I("DSP asrc in rate:%d, out rate:%d\r\n", 2, asrc_config->input_buffer.rate, asrc_config->output_buffer.rate);
}

void afe_send_ccni_anc_switch_filter(uint32_t id)
{
    hal_ccni_message_t msg;
    uint32_t status;
    memset((void *)&msg, 0, sizeof(hal_ccni_message_t));
    msg.ccni_message[0] = ((MSG_DSP2MCU_COMMON_AUDIO_ANC_SWITCH << 16));
    msg.ccni_message[1] = id;
    status = aud_msg_tx_handler(msg, 0, FALSE);

    DSP_MW_LOG_I("DSP send ANC switch ccni:%d \r\n", 1, id);
}


//dsp audio request sync
void dsp_sync_callback_dl2(dsp_sync_request_action_id_t request_action_id, void *user_data)
{
    vol_gain_info_t *gain_info = (vol_gain_info_t *)user_data;
    DSP_MW_LOG_I("DSP sync callback dl2 action:%d \r\n", 1, request_action_id);
    if (request_action_id == SUBMSG_MCU2DSP_SYNC_START) {
        if (Sink_blks[SINK_TYPE_VP_AUDIO] != NULL) {
            hal_audio_trigger_start_parameter_t sw_trigger_start;
            sw_trigger_start.enable = true;
            sw_trigger_start.memory_select = Sink_blks[SINK_TYPE_VP_AUDIO]->param.audio.mem_handle.memory_select;
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        } else {
            DSP_MW_LOG_I("DSP DL2 sync start is invalid \r\n", 0);
            assert(0);
        }
    } else if (request_action_id == SUBMSG_MCU2DSP_SYNC_SET_VOLUME) {
        hal_ccni_message_t msg;
        msg.ccni_message[0] = 1;//For DL2
        msg.ccni_message[1] = ((gain_info->gain)&0xFFFF)|(HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX<<16);
        DSP_GC_SetOutputVolume(msg, NULL);

    } else if (request_action_id == SUBMSG_MCU2DSP_SYNC_SET_MUTE) {
        hal_audio_volume_digital_gain_parameter_t digital_gain;
        memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
        digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL2;
        digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_FRAMEWORK;
        digital_gain.mute_enable = gain_info->gain;
        digital_gain.is_mute_control = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    }
}
void dsp_sync_callback_dl1(dsp_sync_request_action_id_t request_action_id, void *user_data)
{
    vol_gain_info_t *gain_info = (vol_gain_info_t *)user_data;
    if (request_action_id == SUBMSG_MCU2DSP_SYNC_START) {
#if 0
        if (Sink_blks[SINK_TYPE_AUDIO] != NULL) {
            hal_audio_trigger_start_parameter_t sw_trigger_start;
            sw_trigger_start.enable = true;
            sw_trigger_start.memory_select = Sink_blks[SINK_TYPE_AUDIO]->param.audio.mem_handle.memory_select;
            hal_audio_set_value((hal_audio_set_value_parameter_t *)&sw_trigger_start,HAL_AUDIO_SET_TRIGGER_MEMORY_START);
        } else {
            DSP_MW_LOG_I("DSP DL1 sync start is invalid \r\n", 0);
            assert(0);
        }
#endif
    } else if (request_action_id == SUBMSG_MCU2DSP_SYNC_STOP) {
        //Ramp down
        // GPIO
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG
    hal_gpio_set_output(HAL_GPIO_23, 1);
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG */
        hal_ccni_message_t msg;
        msg.ccni_message[0] = 0;//For DL1
        msg.ccni_message[1] = (0xD120)|(HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX<<16); //-120dB
        DSP_GC_SetOutputVolume(msg, NULL);
#ifdef MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG
    hal_gpio_set_output(HAL_GPIO_23, 0);
#endif /* MTK_AUDIO_DSP_SYNC_REQUEST_GPIO_DEBUG */

    } else if (request_action_id == SUBMSG_MCU2DSP_SYNC_SET_VOLUME) {
        hal_ccni_message_t msg;
        msg.ccni_message[0] = 0;//For DL1
        msg.ccni_message[1] = ((gain_info->gain)&0xFFFF)|(HAL_AUDIO_INVALID_ANALOG_GAIN_INDEX<<16);
        DSP_GC_SetOutputVolume(msg, NULL);

    } else if (request_action_id == SUBMSG_MCU2DSP_SYNC_SET_MUTE) {
        hal_audio_volume_digital_gain_parameter_t digital_gain;
        memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
        digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
        digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_ZERO_PADDING;
        digital_gain.mute_enable = gain_info->gain;
        digital_gain.is_mute_control = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    } else if (request_action_id == SUBMSG_MCU2DSP_SYNC_FADE_OUT) {
        DSP_MW_LOG_I("[TEST] FADE OUT", 0);
        hal_audio_volume_digital_gain_parameter_t digital_gain;
        memset(&digital_gain,0,sizeof(hal_audio_volume_digital_gain_parameter_t));
        digital_gain.memory_select = HAL_AUDIO_MEMORY_DL_DL1;
        digital_gain.mute_control = HAL_AUDIO_VOLUME_MUTE_FRAMEWORK;
        digital_gain.mute_enable = true;
        digital_gain.is_mute_control = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&digital_gain, HAL_AUDIO_SET_VOLUME_HW_DIGITAL_GAIN);
    }
    DSP_MW_LOG_I("DSP sync callback dl1 action:%d \r\n", 1, request_action_id);
}
extern CONNECTION_IF n9_a2dp_if, n9_sco_dl_if;
#ifdef MTK_PROMPT_SOUND_ENABLE
extern CONNECTION_IF playback_vp_if;
#endif

void dsp_sync_callback_a2dp(dsp_sync_request_action_id_t request_action_id, void *user_data)
{
    if (n9_a2dp_if.sink) {
        dsp_sync_callback_dl1(request_action_id, user_data);
    } else {
        DSP_MW_LOG_I("DSP sync callback A2DP is closed. action:%d \r\n", 1, request_action_id);
    }
}

void dsp_sync_callback_hfp(dsp_sync_request_action_id_t request_action_id, void *user_data)
{
    if (n9_sco_dl_if.sink) {
        dsp_sync_callback_dl1(request_action_id, user_data);
    } else {
        DSP_MW_LOG_I("DSP sync callback HFP is closed. action:%d \r\n", 1, request_action_id);
    }
}

void dsp_sync_callback_vp(dsp_sync_request_action_id_t request_action_id, void *user_data)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
    if (playback_vp_if.sink) {
        dsp_sync_callback_dl2(request_action_id, user_data);
    } else {
        DSP_MW_LOG_I("DSP sync callback VP is closed. action:%d \r\n", 1, request_action_id);
    }
#endif
}

bool afe_audio_device_ready(SOURCE_TYPE source_type,SINK_TYPE sink_type)
{
    if(source_type >= SOURCE_TYPE_MAX || sink_type >= SINK_TYPE_MAX){
        return false;
    }
    volatile SINK sink = Sink_blks[sink_type];
    if(source_type == SOURCE_TYPE_N9SCO && sink_type == SINK_TYPE_AUDIO){//esco DL
        //check hwsrc2 rx tracking ready
        if(sink->param.audio.audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE){
            #ifdef AIR_HWSRC_RX_TRACKING_ENABLE
            DSP_MW_LOG_I("[HWSRC]: check ASM2_FREQUENCY_2 = 0x%x \r\n", 1, AFE_READ(ASM2_FREQUENCY_2));
            if(AFE_READ(ASM2_FREQUENCY_2) == 0xa00000 || AFE_READ(ASM2_FREQUENCY_2) == 0x0){
                return false;
            }
            #endif
        }
    }else{
        //for other device extend
    }
    return true;
}
