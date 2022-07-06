/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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
#include "scenario_wired_audio.h"
#include "dsp_dump.h"

#define WIRED_AUDIO_DEBUG_LOG 0

#define MIN_USB_FRAMES_IN_START                                 (11)
#define SAMPLES_IN_EACH_USB_FRAME                               (48)
#define SAMPLES_IN_EACH_AFE_FRAME                             (48*10)

#ifndef min
#define min(_a, _b)   (((_a)<(_b))?(_a):(_b))
#endif

wired_audio_handle_t *wired_audio_usb_first_dl_handle = NULL;
volatile uint32_t wired_audio_usb_music_data_in_gpt_count = 0;
volatile uint32_t wired_audio_usb_music_ccni_gpt_count = 0;
//wired_audio_handle_t *gaming_mode_dongle_first_ul_handle = NULL;

#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
wired_audio_usb_vol_balance_buffer_msg_t wire_audio_usb_vol_buffer_msg;
wired_audio_usb_vol_balance_handle_t wire_audio_usb_vol_balance_handle;
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

int8_t usb_audio_start_number = 0;
int8_t wired_audio_get_usb_audio_start_number(void){
    return usb_audio_start_number;
}
void wired_audio_add_usb_audio_start_number(void){
    usb_audio_start_number++;
}
void wired_audio_minus_usb_audio_start_number(void){
    usb_audio_start_number--;
}

CONNECTION_IF *g_application_ptr_usb_in_0 = NULL;
CONNECTION_IF *g_application_ptr_usb_in_1 = NULL;

wired_audio_handle_t *wired_audio_get_handle(void *owner)
{
    uint32_t saved_mask;
    int16_t count;
    wired_audio_handle_t *dongle_handle = NULL;
    wired_audio_handle_t *c_handle = NULL;

    dongle_handle = malloc(sizeof(wired_audio_handle_t));
    if (dongle_handle == NULL)
    {
        configASSERT(0);
    }
    memset(dongle_handle, 0, sizeof(wired_audio_handle_t));

    dongle_handle->total_number = -1;
    dongle_handle->index = -1;
    dongle_handle->owner = owner;
    dongle_handle->next_dl_handle = NULL;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (wired_audio_usb_first_dl_handle == NULL)
    {
        /* there is no item on the handle list */
        dongle_handle->total_number = 1;
        dongle_handle->index = 1;

        /* set this handle as the head of the handle list */
        wired_audio_usb_first_dl_handle = dongle_handle;
    }
    else
    {
        /* there are other items on the handle list */
        count = 1;
        c_handle = wired_audio_usb_first_dl_handle;
        while (c_handle != NULL)
        {
            count += 1;

            c_handle->total_number += 1;
            if (c_handle->total_number <= 0)
            {
                /* error status */
                configASSERT(0);
            }

            if (c_handle->next_dl_handle == NULL)
            {
                /* c_handle is the last item on the list now */
                dongle_handle->total_number = c_handle->total_number;
                dongle_handle->index = c_handle->total_number;

                /* dongle_handle is the last item on the list now */
                c_handle->next_dl_handle = dongle_handle;

                break;
            }

            c_handle = c_handle->next_dl_handle;
        }
        if ((c_handle == NULL) || (dongle_handle->total_number != count))
        {
            configASSERT(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return dongle_handle;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 bool wire_audio_source_get_avail_size(SOURCE source, uint32_t *avail_size)
{
    uint32_t frame_count = 0;
    //wired_audio_handle_t *dongle_handle = (wired_audio_handle_t *)(source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle);
    //uint32_t remain_samples = 0;
    if (source->streamBuffer.ShareBufferInfo.ReadOffset < source->streamBuffer.ShareBufferInfo.WriteOffset)
    {
        /* normal case */
        *avail_size = source->streamBuffer.ShareBufferInfo.WriteOffset - source->streamBuffer.ShareBufferInfo.ReadOffset;
    }
    else if (source->streamBuffer.ShareBufferInfo.ReadOffset == source->streamBuffer.ShareBufferInfo.WriteOffset)
    {
        if(source->streamBuffer.ShareBufferInfo.bBufferIsFull == true)
        {
            /* buffer is full, so ReadOffset == WriteOffset */
            *avail_size = source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num;
            DSP_MW_LOG_E("[audio transmitter][source]: buffer is full, index = %u", 1, source->streamBuffer.ShareBufferInfo.ReadOffset);
        }
        else
        {
            /* buffer is empty, so ReadOffset == WriteOffset */
            *avail_size = 0;

            if ((source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 == true)
                && (source->param.data_dl.scenario_param.usb_in_local_param.is_afe_irq_comming == true))
            {
                source->param.data_dl.scenario_param.usb_in_local_param.is_dummy_data = true;
                *avail_size = 1920;
                DSP_MW_LOG_D("[audio transmitter][source]: avail_size dummy 1920", 0);
            }

            #if WIRED_AUDIO_DEBUG_LOG
            uint32_t current_timestamp = 0;
            uint32_t duration = 0;
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
            hal_gpt_get_duration_count( source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.data_timestamp,
                                        current_timestamp,
                                        &duration);

            DSP_MW_LOG_E("[audio transmitter][source]: buffer is empty in a long time, index = %u", 1, source->streamBuffer.ShareBufferInfo.ReadOffset);
            DSP_MW_LOG_E("[audio transmitter][source]: duration = %u, last time = %u, current time = %u", 3,
                        duration,
                        source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.data_timestamp,
                        current_timestamp);
            #endif /* WIRED_AUDIO_DEBUG_LOG */
        }
    }
    else
    {
        /* buffer wrapper case */
        *avail_size = source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num
                    - source->streamBuffer.ShareBufferInfo.ReadOffset
                    + source->streamBuffer.ShareBufferInfo.WriteOffset;
    }

    if (*avail_size != 0)
    {
        /* if this is the first time, check if the data in usb buffer is larger than 11ms */
        if (((uint32_t)source->param.data_dl.current_notification_index == 0) &&
            (*avail_size < (MIN_USB_FRAMES_IN_START * (uint32_t)source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size)))
        {
            /* the data is not enough, so change avail_size to zero */
            *avail_size = 0;
            if ((source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 == true)
                && (source->param.data_dl.scenario_param.usb_in_local_param.is_afe_irq_comming == true))
            {
                source->param.data_dl.scenario_param.usb_in_local_param.is_dummy_data = true;
                *avail_size = 1920;
                DSP_MW_LOG_D("[audio transmitter][source]: avail_size dummy 1920", 0);
            }
            // if (hal_nvic_query_exception_number() > 0)
            // {
            //     dongle_handle->data_status = BLE_AUDIO_SOURCE_STREAM_DATA_EMPTY;
            // }
        }
        else
        {
            /* change avail_size to actual data size in the buffer */
            frame_count = *avail_size / source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size;
            *avail_size = *avail_size - frame_count * sizeof(audio_transmitter_frame_header_t);

            uint32_t remain_samples =0;
            // if (hal_nvic_query_exception_number() > 0)
            // {
                wired_audio_handle_t *dongle_handle = (wired_audio_handle_t *)(source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle);
                #ifdef AIR_SOFTWARE_BUFFER_ENABLE
                /* check if there are at least 10ms data */
                remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);
                #endif /* AIR_SOFTWARE_BUFFER_ENABLE */

                if ((frame_count*SAMPLES_IN_EACH_USB_FRAME+remain_samples) >= SAMPLES_IN_EACH_AFE_FRAME)
                {
                    /* update data status to indicate there are data in stream and the mixer needs to wait this stream */
                    dongle_handle->data_status = GAMING_MODE_STREAM_DATA_IN_STREAM;
                }
                else
                {
                    #if WIRED_AUDIO_DEBUG_LOG
                    /* When in irq level, there should be at least 9ms data in share buffer because the period of ccni is 10ms */
                    DSP_MW_LOG_E("[audio transmitter][source]: data is not enough, %u", 1, *avail_size);
                    #endif /* WIRED_AUDIO_DEBUG_LOG */

                    /* update data status to indicate there are not enough data in stream and the mixer do not need to wait this stream */
                    dongle_handle->data_status = GAMING_MODE_STREAM_DATA_EMPTY;
                    *avail_size = 0;
                    if ((source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 == true)
                        && (source->param.data_dl.scenario_param.usb_in_local_param.is_afe_irq_comming == true))
                    {
                        source->param.data_dl.scenario_param.usb_in_local_param.is_dummy_data = true;
                        *avail_size = 1920;
                        DSP_MW_LOG_D("[audio transmitter][source]: avail_size dummy 1920", 0);
                    }
                    dongle_handle->process_frames = 0;
                }
            // }
        }

        // if (hal_nvic_query_exception_number() > 0)
        // {
        //     /* update data status to indicate there are data in stream and the mixer needs to wait this stream */
        //     gaming_mode_dongle_dl_handle_t *dongle_handle = (gaming_mode_dongle_dl_handle_t *)(source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle);
        //     dongle_handle->data_status = GAMING_MODE_STREAM_DATA_IN_STREAM;
        // }
    }
    // }

    #if WIRED_AUDIO_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][get_avail_size]:%u, %u, %d, %u, %d", 5,
                source->streamBuffer.ShareBufferInfo.ReadOffset,
                source->streamBuffer.ShareBufferInfo.WriteOffset,
                *avail_size,
                current_timestamp,
                hal_nvic_query_exception_number());
    #endif /* WIRED_AUDIO_DEBUG_LOG */

    return true;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 uint32_t wire_audio_source_copy_payload(SOURCE source, uint8_t *src_buf, uint8_t *dst_buf, uint32_t length)
{
    DSP_STREAMING_PARA_PTR stream = NULL;
    audio_transmitter_frame_header_t *frame_header = NULL;
    uint32_t total_frames = 0;
    uint32_t total_samples = 0;
    uint32_t current_frame_samples = 0;
    uint32_t current_frame = 0;
    uint32_t avail_size = 0;
    uint32_t total_buffer_size = 0;
    uint32_t ReadOffset = 0;
    wired_audio_handle_t *dongle_handle = (wired_audio_handle_t *)(source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle);
    UNUSED(dongle_handle);
    UNUSED(dst_buf);

    /* update data receive timestamp */
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.data_timestamp);

    /* get the stream */
    stream = DSP_Streaming_Get(source, source->transform->sink);

    /* get the total frame number and buffer size in share buffer */
    total_frames = length / (source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size - sizeof(audio_transmitter_frame_header_t));
    total_buffer_size = source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size *
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num;

    /* copy dummy usb audio data */
    if ((source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 == true)
        && (source->param.data_dl.scenario_param.usb_in_local_param.is_dummy_data == true))
    {
        memset((U8*)(stream->callback.EntryPara.in_ptr[0]), 0, SAMPLES_IN_EACH_AFE_FRAME*sizeof(int16_t));
        memset((U8*)(stream->callback.EntryPara.in_ptr[1]), 0, SAMPLES_IN_EACH_AFE_FRAME*sizeof(int16_t));
        goto bypass_copy;
    }
    /* check if this is the first time */
    if (source->param.data_dl.current_notification_index == 0)
    {
        wire_audio_source_get_avail_size(source, &avail_size);
        if(avail_size > length)
        {
            /* at the first time, maybe the actual data size is larger than stream->callback.EntryPara.in_malloc_size */
            /* in dsp_callback.c, DSP_Callback_Processing will cut length to the stream->callback.EntryPara.in_malloc_size */
            /* in here, we need to change length back to the actual data size */
            length = avail_size;
            total_frames = length / (source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size - sizeof(audio_transmitter_frame_header_t));
        }

        /* parameters check */
        if (total_frames < MIN_USB_FRAMES_IN_START)
        {
            DSP_MW_LOG_E("[audio transmitter][source]: error parameters %d, %d", 2, total_frames, length);
            configASSERT(0);
        }

        /* Only copy the last 11ms data from the share buffer and the remian data in the head are dropped */
        total_samples = 0;
        current_frame = 0;
        ReadOffset =(source->streamBuffer.ShareBufferInfo.ReadOffset +
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * (total_frames-MIN_USB_FRAMES_IN_START)
                    ) % total_buffer_size;
        src_buf = (uint8_t *)(source->streamBuffer.ShareBufferInfo.startaddr + ReadOffset);
        while(current_frame < MIN_USB_FRAMES_IN_START)
        {
            /* get current frame info and current frame samples */
            frame_header = (audio_transmitter_frame_header_t *)src_buf;
            current_frame_samples = frame_header->payload_len / (sizeof(U16) * 2);
            // if (current_frame == 0)
            // {
            //     /* only copy 0.5ms(48K*0.5ms=24samples) data at the first time */
            //     current_frame_samples = 24;
            //     /* offset 0.5ms data in src_buf */
            //     src_buf = src_buf + 24 * (sizeof(U16) * 2);
            // }

            /* copy usb audio data from share buffer */
            DSP_I2D_BufferCopy_16bit((U16 *)(src_buf + sizeof(audio_transmitter_frame_header_t)),
                                    (U16 *)(stream->callback.EntryPara.in_ptr[0])+total_samples,
                                    (U16 *)(stream->callback.EntryPara.in_ptr[1])+total_samples,
                                    current_frame_samples);
            // if (current_frame == 0)
            // {
            //     /* offset back src_buf to the original position */
            //     src_buf = src_buf - 24 * (sizeof(U16) * 2);
            // }

            /* update total samples */
            total_samples += current_frame_samples;

            /* update copied frame number */
            current_frame += 1;

            /* change to the next frame */
            ReadOffset =(source->streamBuffer.ShareBufferInfo.ReadOffset +
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * (total_frames-4+current_frame)
                        ) % total_buffer_size;
            src_buf = (uint8_t *)(source->streamBuffer.ShareBufferInfo.startaddr + ReadOffset);

            #if WIRED_AUDIO_DEBUG_LOG
            // DSP_MW_LOG_I("[audio transmitter][source]:copy samples %u, %d, %d, %d, %d, %d", 6,
            //                 src_buf,
            //                 length,
            //                 total_frames,
            //                 current_frame,
            //                 current_frame_samples,
            //                 total_samples);
            #endif /* WIRED_AUDIO_DEBUG_LOG */
        }
        // if(total_samples != 168)
        // {
        //     DSP_MW_LOG_E("[audio transmitter][source]: error copy samples %d, %d", 2, length, total_samples);
        //     configASSERT(0);
        // }
        LOG_AUDIO_DUMP( (uint8_t *)stream->callback.EntryPara.in_ptr[0],total_samples*2, VOICE_TX_MIC_2);
        /* update processed frames */
        source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames = total_frames;
        // #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
        // /* get default capid */
        // if (dongle_handle->index == 1)
        // {
        //     dongle_handle->cap_id_default = DCXO_CAPID_EFUSE_REG.DCXO_CAPID_EFUSE;
        //     dongle_handle->cap_id_cur = dongle_handle->cap_id_default;
        // }
        // else
        // {
        //     dongle_handle->cap_id_default = wired_audio_usb_first_dl_handle->cap_id_default;
        //     dongle_handle->cap_id_cur = wired_audio_usb_first_dl_handle->cap_id_cur;
        // }
        // #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

        #if WIRED_AUDIO_DEBUG_LOG
        uint32_t current_timestamp = 0;
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
        DSP_MW_LOG_I("[audio transmitter][source][copy_payload]: get first data packet time = %u. total_samples %d", 2, current_timestamp,total_samples);
        #endif /* WIRED_AUDIO_DEBUG_LOG */
    }
    else
    {
        /* parameters check */
        if (total_frames > MIN_USB_FRAMES_IN_START)
        {
            /* in here, it means there are at least 11 packet data in share buffer. */
            DSP_MW_LOG_E("[audio transmitter][source]: too much data in share buffer %d, %d, %u, %u, 0x%x, 0x%x", 6,
                        total_frames,
                        length,
                        source->streamBuffer.ShareBufferInfo.ReadOffset,
                        source->streamBuffer.ShareBufferInfo.WriteOffset,
                        wired_audio_usb_music_data_in_gpt_count,
                        wired_audio_usb_music_ccni_gpt_count);

            // configASSERT(0);

            /* get the new read index */
            source->streamBuffer.ShareBufferInfo.ReadOffset = (source->streamBuffer.ShareBufferInfo.WriteOffset + total_buffer_size -
                                                                source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * MIN_USB_FRAMES_IN_START
                                                                ) % total_buffer_size;

            /* set WriteOffset - 11 as the new ReadOffset, it will drop the other data in the share buffer */
            extern VOID audio_transmitter_share_information_update_read_offset(SOURCE source, U32 ReadOffset);
            audio_transmitter_share_information_update_read_offset(source, source->streamBuffer.ShareBufferInfo.ReadOffset);

            /* get new share buffer info*/
            extern VOID audio_transmitter_share_information_fetch(SOURCE source, SINK sink);
            audio_transmitter_share_information_fetch(source, NULL);

            /* only process the last 3 packet data */
            total_frames = MIN_USB_FRAMES_IN_START;
            length = MIN_USB_FRAMES_IN_START * (source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size - sizeof(audio_transmitter_frame_header_t));

            DSP_MW_LOG_E("[audio transmitter][source]: change status, %d, %d, %u, %u", 4,
                        total_frames,
                        length,
                        source->streamBuffer.ShareBufferInfo.ReadOffset,
                        source->streamBuffer.ShareBufferInfo.WriteOffset);
        }

        #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
        int32_t compensatory_samples = 0;
        uint32_t remain_samples = 0;

        /* get remain samples */
        remain_samples = stream_function_sw_buffer_get_channel_used_size(dongle_handle->buffer_port, 1) / sizeof(int16_t);

        /* choose which way to do compensatory */
        if (dongle_handle->compen_method == WIRED_AUDIO_COMPENSATORY_METHOD_SW_CLK_SKEW)
        {
            /* do nothing because sw clk skew is under running */
        }
        else if ((dongle_handle->total_compen_samples != 0) &&
                (dongle_handle->compen_method == WIRED_AUDIO_COMPENSATORY_METHOD_DISABLE))
        {
            /* use sw clk skew on this stream */
            dongle_handle->compen_method = WIRED_AUDIO_COMPENSATORY_METHOD_SW_CLK_SKEW;
        }

        /* do compensatory */
        if (dongle_handle->compen_method == WIRED_AUDIO_COMPENSATORY_METHOD_SW_CLK_SKEW)
        {
            if (dongle_handle->total_compen_samples > 0)
            {
                /* buffer output 479 samples, clk skew will change them to 480 samples */
                compensatory_samples = 1;
                dongle_handle->total_compen_samples -= 1;
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, (SAMPLES_IN_EACH_AFE_FRAME-1)*sizeof(int16_t));
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, (SAMPLES_IN_EACH_AFE_FRAME-1)*sizeof(int16_t));
            }
            else if (dongle_handle->total_compen_samples < 0)
            {
                /* buffer output 481 samples, clk skew will change them to 480 samples */
                compensatory_samples = -1;
                dongle_handle->total_compen_samples += 1;
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, (SAMPLES_IN_EACH_AFE_FRAME+1)*sizeof(int16_t));
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, (SAMPLES_IN_EACH_AFE_FRAME+1)*sizeof(int16_t));
            }
            else
            {
                /* buffer output 480 samples */
                compensatory_samples = 0;
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, SAMPLES_IN_EACH_AFE_FRAME*sizeof(int16_t));
                stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, SAMPLES_IN_EACH_AFE_FRAME*sizeof(int16_t));
            }
            stream_function_sw_clk_skew_configure_compensation_samples(dongle_handle->clk_skew_port, compensatory_samples);
            source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.clk_skew.compen_samples = compensatory_samples;

            if (dongle_handle->total_compen_samples == 0)
            {
                dongle_handle->compen_method = WIRED_AUDIO_COMPENSATORY_METHOD_DISABLE;
                dongle_handle->compen_count = 0;
                dongle_handle->compen_flag = 0;
            }
        }
        else
        {
            /* calculator compensatory samples */
            if ((dongle_handle->total_compen_samples == 0) && ((remain_samples+total_frames*SAMPLES_IN_EACH_USB_FRAME-SAMPLES_IN_EACH_AFE_FRAME) <= 0))
            {
                if (dongle_handle->compen_count < 10)
                {
                    /* in here, it means there may be +1ms compensatory but its persistent period is less than 50ms */
                    dongle_handle->compen_count += 1;
                    dongle_handle->compen_flag = 1;
                }
                else
                {
                    /* in here, it means there is +1ms compensatory and its persistent period is more than 50ms */
                    dongle_handle->total_compen_samples = SAMPLES_IN_EACH_USB_FRAME;
                    dongle_handle->compen_count = 0;
                    dongle_handle->compen_flag = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && (dongle_handle->compen_flag == 1) && ((remain_samples+total_frames*SAMPLES_IN_EACH_USB_FRAME-SAMPLES_IN_EACH_AFE_FRAME) > 0))
            {
                /* in here, it means maybe the +1ms compensatory is false alarm and the usb irq is just not stable in this time */
                if (dongle_handle->compen_count != 0)
                {
                    dongle_handle->compen_count -= 1;
                }
                if (dongle_handle->compen_count == 0)
                {
                    dongle_handle->compen_flag = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && ((remain_samples+total_frames*SAMPLES_IN_EACH_USB_FRAME-SAMPLES_IN_EACH_AFE_FRAME) > SAMPLES_IN_EACH_USB_FRAME))
            {
                if (dongle_handle->compen_count < 10)
                {
                    /* in here, it means there may be -1ms compensatory but its persistent period is less than 50ms */
                    dongle_handle->compen_count += 1;
                    dongle_handle->compen_flag = -1;
                }
                else
                {
                    /* in here, it means there is -1ms compensatory and its persistent period is more than 50ms*/
                    dongle_handle->total_compen_samples = SAMPLES_IN_EACH_USB_FRAME - (remain_samples+total_frames*SAMPLES_IN_EACH_USB_FRAME-SAMPLES_IN_EACH_AFE_FRAME);
                    dongle_handle->compen_count = 0;
                }
            }
            else if ((dongle_handle->total_compen_samples == 0) && (dongle_handle->compen_flag == -1) && ((remain_samples+total_frames*SAMPLES_IN_EACH_USB_FRAME-SAMPLES_IN_EACH_AFE_FRAME) <= SAMPLES_IN_EACH_USB_FRAME))
            {
                /* in here, it means maybe the -1ms compensatory is false alarm and the usb irq is just not stable in this time */
                if (dongle_handle->compen_count != 0)
                {
                    dongle_handle->compen_count -= 1;
                }
                if (dongle_handle->compen_count == 0)
                {
                    dongle_handle->compen_flag = 0;
                }
            }

            /* always output 480 samples */
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, SAMPLES_IN_EACH_AFE_FRAME*sizeof(int16_t));
            stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, SAMPLES_IN_EACH_AFE_FRAME*sizeof(int16_t));

            /* bypass sw clk skew */
            compensatory_samples = 0;
            stream_function_sw_clk_skew_configure_compensation_samples(dongle_handle->clk_skew_port, compensatory_samples);
            source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.clk_skew.compen_samples = compensatory_samples;
        }
        #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

        /* Copy all data from the share buffer */
        total_samples = 0;
        current_frame = 0;
        ReadOffset = source->streamBuffer.ShareBufferInfo.ReadOffset;
        src_buf = (uint8_t *)(source->streamBuffer.ShareBufferInfo.startaddr + ReadOffset);
        while(current_frame < total_frames)
        {
            /* get current frame info and current frame samples */
            frame_header = (audio_transmitter_frame_header_t *)src_buf;
            current_frame_samples = frame_header->payload_len / (sizeof(U16) * 2);

            /* copy usb audio data from share buffer */
            DSP_I2D_BufferCopy_16bit((U16 *)(src_buf + sizeof(audio_transmitter_frame_header_t)),
                                    (U16 *)(stream->callback.EntryPara.in_ptr[0])+total_samples,
                                    (U16 *)(stream->callback.EntryPara.in_ptr[1])+total_samples,
                                    current_frame_samples);

            /* update total samples */
            total_samples += current_frame_samples;

            /* update copied frame number */
            current_frame += 1;

            /* change to the next frame */
            ReadOffset =(source->streamBuffer.ShareBufferInfo.ReadOffset +
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * current_frame
                        ) % total_buffer_size;
            src_buf = (uint8_t *)(source->streamBuffer.ShareBufferInfo.startaddr + ReadOffset);

            #if WIRED_AUDIO_DEBUG_LOG
            // DSP_MW_LOG_I("[audio transmitter][source]:copy samples %u, %d, %d, %d, %d, %d", 6,
            //                 src_buf,
            //                 length,
            //                 total_frames,
            //                 current_frame,
            //                 current_frame_samples,
            //                 total_samples);
            #endif /* WIRED_AUDIO_DEBUG_LOG */
        }
        if(total_samples != (length / (sizeof(S16) * 2)))
        {
            DSP_MW_LOG_E("[audio transmitter][source]: error copy samples %d, %d", 2, length, total_samples);
            configASSERT(0);
        }

        /* update processed frames */
        source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames = total_frames;
    }

bypass_copy:
    if ((source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 == true)
        && (source->param.data_dl.scenario_param.usb_in_local_param.is_dummy_data == true))
    {
        #ifdef AIR_SOFTWARE_BUFFER_ENABLE
        /* always output 480 samples */
        stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 1, SAMPLES_IN_EACH_AFE_FRAME*sizeof(int16_t));
        stream_function_sw_buffer_config_channel_output_size(dongle_handle->buffer_port, 2, SAMPLES_IN_EACH_AFE_FRAME*sizeof(int16_t));
        #endif

        #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
        /* bypass sw clk skew */
        int32_t compensatory_samples = 0;
        stream_function_sw_clk_skew_configure_compensation_samples(dongle_handle->clk_skew_port, compensatory_samples);
        source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.clk_skew.compen_samples = compensatory_samples;

        source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames = MIN_USB_FRAMES_IN_START - 1;
        #endif

        total_samples = SAMPLES_IN_EACH_AFE_FRAME;
    }

    /* update stream status */
    stream->callback.EntryPara.in_size = total_samples * sizeof(S16);
    stream->callback.EntryPara.in_channel_num = 2;
    stream->callback.EntryPara.in_sampling_rate = FS_RATE_48K;
    stream->callback.EntryPara.resolution.process_res = RESOLUTION_16BIT;

    /* update state machine */
    dongle_handle = (wired_audio_handle_t *)(source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle);
    #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    dongle_handle->compen_samples = source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.clk_skew.compen_samples;
    dongle_handle->process_frames = source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames;
    #endif

    #if WIRED_AUDIO_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][copy_payload]: %d, %d, %u, %d in_size:%d", 5, total_samples, length, current_timestamp, hal_nvic_query_exception_number(),stream->callback.EntryPara.in_size);
    #endif /* WIRED_AUDIO_DEBUG_LOG */

    /* avoid payload length check error in here */
    length = (length > source->param.data_dl.max_payload_size) ? source->param.data_dl.max_payload_size : length;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&wired_audio_usb_music_data_in_gpt_count);

    return length;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 uint32_t wire_audio_source_get_new_read_offset(SOURCE source, U32 amount)
{
    uint32_t total_buffer_size;
    SHARE_BUFFER_INFO_PTR ShareBufferInfo;
    uint32_t ReadOffset;
    UNUSED(amount);

    if ((source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 == true)
        && (source->param.data_dl.scenario_param.usb_in_local_param.is_dummy_data == true))
    {
        ShareBufferInfo = &source->streamBuffer.ShareBufferInfo;
        ReadOffset = ShareBufferInfo->ReadOffset;
    }
    else{
        /* get new read offset */
        ShareBufferInfo = &source->streamBuffer.ShareBufferInfo;
        total_buffer_size = ShareBufferInfo->sub_info.block_info.blk_size * ShareBufferInfo->sub_info.block_info.blk_num;
        ReadOffset =(ShareBufferInfo->ReadOffset +
                    ShareBufferInfo->sub_info.block_info.blk_size * source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames
                    ) % total_buffer_size;
    }

    #if WIRED_AUDIO_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][get_new_read_offset]: %u, %u, %d", 3, ReadOffset, current_timestamp, hal_nvic_query_exception_number());
    #endif /* WIRED_AUDIO_DEBUG_LOG */

    return ReadOffset;
}

ATTR_TEXT_IN_IRAM_LEVEL_1 void wire_audio_source_drop_postprocess(SOURCE source, uint32_t amount)
{
    uint32_t saved_mask = 0;
    wired_audio_handle_t *c_handle = NULL;
    extern wired_audio_handle_t *wired_audio_usb_first_dl_handle;
    c_handle = source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle;
    UNUSED(amount);

    if (source->param.data_dl.scenario_param.usb_in_local_param.is_use_afe_dl3 == true)
    {
        source->param.data_dl.scenario_param.usb_in_local_param.is_afe_irq_comming = false;
    }
    if (source->param.data_dl.scenario_param.usb_in_local_param.is_dummy_data != true)
    {
        if (source->param.data_dl.current_notification_index == 0)
        {
            source->param.data_dl.current_notification_index = 1;
        }
    }

    #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
    if(c_handle != NULL){
        if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) && (source->param.data_dl.current_notification_index != 0) && (source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames != 0))
        {
            wire_audio_usb_vol_buffer_msg.data_buf_1_cur  = (uint16_t *)stream_function_sw_buffer_get_channel_read_pointer(c_handle->buffer_port, 1);
            wire_audio_usb_vol_buffer_msg.data_buf_1_head = (uint16_t *)stream_function_sw_buffer_get_channel_start_pointer(c_handle->buffer_port, 1);
            wire_audio_usb_vol_buffer_msg.data_buf_1_tail = (uint16_t *)stream_function_sw_buffer_get_channel_end_pointer(c_handle->buffer_port, 1);
            wire_audio_usb_vol_buffer_msg.data_buf_2_cur  = (uint16_t *)stream_function_sw_buffer_get_channel_read_pointer(c_handle->buffer_port, 2);
            wire_audio_usb_vol_buffer_msg.data_buf_2_head = (uint16_t *)stream_function_sw_buffer_get_channel_start_pointer(c_handle->buffer_port, 2);
            wire_audio_usb_vol_buffer_msg.data_buf_2_tail = (uint16_t *)stream_function_sw_buffer_get_channel_end_pointer(c_handle->buffer_port, 2);
            DTM_enqueue(DTM_EVENT_ID_WIRED_AUDIO_USB_VOLUME_SMART_BALANCE_PROCESS, (uint32_t)&wire_audio_usb_vol_buffer_msg, false);
        }
        else if ((source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0) && (source->param.data_dl.current_notification_index != 0) && (source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames == 0))
        {
            wire_audio_usb_vol_buffer_msg.data_buf_1_cur = NULL;
            wire_audio_usb_vol_buffer_msg.data_buf_2_cur = NULL;
            DTM_enqueue(DTM_EVENT_ID_WIRED_AUDIO_USB_VOLUME_SMART_BALANCE_PROCESS, (uint32_t)&wire_audio_usb_vol_buffer_msg, false);
        }
    }
    #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */

    #if WIRED_AUDIO_DEBUG_LOG
    DSP_MW_LOG_I("[audio transmitter][source][drop_postprocess]: %d, %d, %d",3,
                    source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.clk_skew.compen_samples,
                    source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames,
                    amount);
    #endif /* WIRED_AUDIO_DEBUG_LOG */

    #ifdef MTK_AUDIO_DUMP_BY_CONFIGTOOL
    #if GAMING_MODE_MUSIC_DONGLE_DEBUG_DUMP
    uint32_t i, address, read_offset, total_buffer_size;
    total_buffer_size = source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size *
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_num;
    for (i=0; i < source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames; i++)
    {
        read_offset =(source->streamBuffer.ShareBufferInfo.ReadOffset +
                    source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size * i
                    ) % total_buffer_size;
        address = source->streamBuffer.ShareBufferInfo.startaddr + read_offset;
        LOG_AUDIO_DUMP( (uint8_t *)address,
                        source->streamBuffer.ShareBufferInfo.sub_info.block_info.blk_size,
                        VOICE_TX_MIC_2);
    }
    #endif /* GAMING_MODE_MUSIC_DONGLE_DEBUG_DUMP */
    #endif /* MTK_AUDIO_DUMP_BY_CONFIGTOOL */

    /* update state machine */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.fetch_flag = 0;
    source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.process_frames = 0;
    hal_nvic_restore_interrupt_mask(saved_mask);

    #if WIRED_AUDIO_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[audio transmitter][source][drop_postprocess]: %u, %d", 2, current_timestamp, hal_nvic_query_exception_number());
    #endif /* WIRED_AUDIO_DEBUG_LOG */

}

void wired_audio_release_handle(wired_audio_handle_t *handle)
{
    uint32_t saved_mask, i, count;
    wired_audio_handle_t *l_handle = NULL;
    wired_audio_handle_t *c_handle = NULL;
    wired_audio_handle_t *dongle_handle = NULL;

    if ((handle == NULL) || (wired_audio_usb_first_dl_handle == NULL))
    {
        configASSERT(0);
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    if (wired_audio_usb_first_dl_handle->total_number <= 0)
    {
        /* error status */
        configASSERT(0);
    }
    else if ((wired_audio_usb_first_dl_handle->total_number == 1) &&
            (wired_audio_usb_first_dl_handle == handle))
    {
        /* this handle is only one item on handle list */
        if (wired_audio_usb_first_dl_handle->next_dl_handle != NULL)
        {
            configASSERT(0);
        }

        /* there is no item on the handle list */
        wired_audio_usb_first_dl_handle = NULL;
    }
    else if ((wired_audio_usb_first_dl_handle->total_number > 1) &&
            (wired_audio_usb_first_dl_handle == handle))
    {
        /* this handle is the first item on handle list, but there are other handles on the list */
        c_handle = wired_audio_usb_first_dl_handle;
        count = wired_audio_usb_first_dl_handle->total_number;
        for (i=0; i < count; i++)
        {
            c_handle->total_number -= 1;
            c_handle->index -= 1;

            c_handle = c_handle->next_dl_handle;
        }
        if (c_handle != NULL)
        {
            /* error status */
            configASSERT(0);
        }

        /* change the next iten to the head */
        wired_audio_usb_first_dl_handle = handle->next_dl_handle;
    }
    else
    {
        /* this handle is the middle item on handle list */
        c_handle = wired_audio_usb_first_dl_handle;
        count = wired_audio_usb_first_dl_handle->total_number;
        if (count == 1)
        {
            /* error status */
            configASSERT(0);
        }
        for (i=0; i < count; i++)
        {
            if (c_handle == handle)
            {
                /* find out the handle on the list */
                dongle_handle = handle;
                l_handle->next_dl_handle = c_handle->next_dl_handle;
            }

            if (dongle_handle == NULL)
            {
                c_handle->total_number -= 1;
            }
            else
            {
                c_handle->total_number -= 1;
                c_handle->index -= 1;
            }

            l_handle = c_handle;
            c_handle = c_handle->next_dl_handle;
        }
        if ((c_handle != NULL) || (dongle_handle == NULL))
        {
            /* error status */
            configASSERT(0);
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    free(handle);
}

bool wire_audio_source_close(SOURCE source)
{
    uint32_t saved_mask;
    /* application deinit */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    wired_audio_handle_t *dongle_handle = (wired_audio_handle_t *)(source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle);

    #ifdef AIR_SOFTWARE_GAIN_ENABLE
    stream_function_sw_gain_deinit(dongle_handle->gain_port);
    #endif /* AIR_SOFTWARE_GAIN_ENABLE */

    #ifdef AIR_SOFTWARE_CLK_SKEW_ENABLE
    stream_function_sw_clk_skew_deinit(dongle_handle->clk_skew_port);
    #endif /* AIR_SOFTWARE_CLK_SKEW_ENABLE */

    #ifdef AIR_SOFTWARE_BUFFER_ENABLE
    stream_function_sw_buffer_deinit(dongle_handle->buffer_port);
    #endif /* AIR_SOFTWARE_BUFFER_ENABLE */

    #ifdef AIR_SOFTWARE_MIXER_ENABLE
    stream_function_sw_mixer_member_unregister(SW_MIXER_PORT_0, dongle_handle->mixer_member);
    stream_function_sw_mixer_member_delete(dongle_handle->mixer_member);
    stream_function_sw_mixer_deinit(SW_MIXER_PORT_0);
    #endif /* AIR_SOFTWARE_MIXER_ENABLE */

    /* release handle */
    wired_audio_release_handle(dongle_handle);
    source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle = NULL;
    hal_nvic_restore_interrupt_mask(saved_mask);

    wired_audio_minus_usb_audio_start_number();
    if(source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0){
        g_application_ptr_usb_in_0 = NULL;
    }
    else if(source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1){
        g_application_ptr_usb_in_1 = NULL;
    }

    return true;
}

void wire_audio_runtime_config(audio_transmitter_scenario_sub_id_wiredaudio_t sub_id, void *config_param, wired_audio_handle_t *handle)
{
    uint32_t saved_mask = 0;
    wired_audio_runtime_config_param_p p_config_param = (wired_audio_runtime_config_param_p)config_param;
    wired_audio_handle_t *dongle_handle = handle;
    #ifdef AIR_SOFTWARE_GAIN_ENABLE
    int32_t new_gain;
    int32_t new_gain2;
    sw_gain_config_t old_config;
    #endif /* AIR_SOFTWARE_GAIN_ENABLE */

    UNUSED(saved_mask);
    UNUSED(sub_id);

    switch(p_config_param->config_operation){
        case WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_L:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            /* left channel */
            new_gain = p_config_param->vol_level.gain_1;
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 1, &old_config);
            DSP_MW_LOG_I("[audio transmitter][config]wired_audio_usb_in change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                        1,
                        old_config.target_gain,
                        new_gain);

            #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            if (sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)
            {
                wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1 = p_config_param->vol_level.gain_1;
            }
            else if (sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)
            {
                wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1 = p_config_param->vol_level.gain_1;
                if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 < p_config_param->vol_level.gain_1))
                {
                    new_gain = wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1;
                }
                else if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 >= p_config_param->vol_level.gain_1))
                {
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = p_config_param->vol_level.gain_1;
                }
                else
                {
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = p_config_param->vol_level.gain_1;
                }
            }
            hal_nvic_restore_interrupt_mask(saved_mask);
            #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 1, new_gain);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;

        case WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_R:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            new_gain = p_config_param->vol_level.gain_2;
            /* right channel */
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 2, &old_config);
            DSP_MW_LOG_I("[audio transmitter][config]wired_audio_usb_in change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                        2,
                        old_config.target_gain,
                        new_gain);

            #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            if (sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)
            {
                wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2 = p_config_param->vol_level.gain_2;
            }
            else if (sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)
            {
                wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2 = p_config_param->vol_level.gain_2;
                if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 < p_config_param->vol_level.gain_2))
                {
                    new_gain = wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2;
                }
                else if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 >= p_config_param->vol_level.gain_2))
                {
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = p_config_param->vol_level.gain_2;
                }
                else
                {
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = p_config_param->vol_level.gain_2;
                }
            }
            hal_nvic_restore_interrupt_mask(saved_mask);
            #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 2, new_gain);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;

        case WIRED_AUDIO_CONFIG_OP_VOL_LEVEL_MUSIC_DUL:
            #ifdef AIR_SOFTWARE_GAIN_ENABLE
            /* left channel */
            new_gain = p_config_param->vol_level.gain_1;
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 1, &old_config);
            DSP_MW_LOG_I("[audio transmitter][config]wired_audio_usb_in change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                        1,
                        old_config.target_gain,
                        new_gain);

            /* right channel */
            stream_function_sw_gain_get_config(dongle_handle->gain_port, 2, &old_config);
            new_gain2 = p_config_param->vol_level.gain_2;
            DSP_MW_LOG_I("[audio transmitter][config]wired_audio_usb_in change channel %d gain from %d*0.01dB to %d*0.01dB\r\n", 3,
                        2,
                        old_config.target_gain,
                        new_gain2);

            #ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            if (sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)
            {
                wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1 = p_config_param->vol_level.gain_1;
                wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2 = p_config_param->vol_level.gain_2;
            }
            else if (sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)
            {
                wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1 = p_config_param->vol_level.gain_1;
                if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 < p_config_param->vol_level.gain_1))
                {
                    new_gain = wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1;
                }
                else if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 >= p_config_param->vol_level.gain_1))
                {
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = p_config_param->vol_level.gain_1;
                }
                else
                {
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = p_config_param->vol_level.gain_1;
                }
                wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2 = p_config_param->vol_level.gain_2;
                if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 < p_config_param->vol_level.gain_2))
                {
                    new_gain2 = wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2;
                }
                else if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 >= p_config_param->vol_level.gain_2))
                {
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = p_config_param->vol_level.gain_2;
                }
                else
                {
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = p_config_param->vol_level.gain_2;
                }
            }
            hal_nvic_restore_interrupt_mask(saved_mask);
            #endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 1, new_gain);
            stream_function_sw_gain_configure_gain_target(dongle_handle->gain_port, 2, new_gain2);
            #endif /* AIR_SOFTWARE_GAIN_ENABLE */
            break;
        default:
            DSP_MW_LOG_E("[audio transmitter][config]wired_audio_usb_in error config_operation: %d\r\n", 1,
                        p_config_param->config_operation);

    }
}


#ifdef AIR_SOFTWARE_MIXER_ENABLE
ATTR_TEXT_IN_IRAM_LEVEL_1 void wired_audio_mixer_postcallback(sw_mixer_member_t *member, void *para, uint32_t *out_frame_size)
{
    int32_t i;
    wired_audio_handle_t *c_handle = NULL;
    SOURCE source = NULL;
    bool all_streams_in_mixer = true;

    UNUSED(member);
    UNUSED(para);

    /* change this handle data status */
    source = (SOURCE)(member->owner);
    c_handle = (wired_audio_handle_t *)(source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle);

    all_streams_in_mixer = true;
    /* check if the output data in this time includes all stream data */
    if ((*out_frame_size != 0) && (all_streams_in_mixer == true))
    {
        /* all_streams_in_mixer is true, so all stream data is in this mixer.
           So we can clean all mixed streams' input buffers now and the mix result are sent to the sink */
        c_handle = wired_audio_usb_first_dl_handle;
        for (i = 0; i < wired_audio_usb_first_dl_handle->total_number; i++)
        {
            // if (c_handle->mixer_status != GAMING_MODE_STREAM_MIXER_UNMIX)
            // {
                /* clear this stream's input buffer */
                stream_function_sw_mixer_member_input_buffers_clean(c_handle->mixer_member, false);

                /* change this handle data status */
                // c_handle->data_status = GAMING_MODE_STREAM_DATA_EMPTY;
            // }

            /* switch to the next dl stream */
            c_handle = c_handle->next_dl_handle;
        }
    }
    else
    {
        /* all_streams_in_mixer is false, so some stream data is not in this mixer.
           So we need to bypass this mix result */
        *out_frame_size = 0;
    }
}
#endif /* AIR_SOFTWARE_MIXER_ENABLE */

#ifdef AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE
void wired_audio_usb_dl_vol_balance_init(SOURCE source, audio_transmitter_open_param_p open_param)
{
    wired_audio_handle_t *wired_audio_handle = source->param.data_dl.scenario_param.usb_in_local_param.usb_in_param.handle;
    if (open_param->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)
    {
        wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1 = open_param->scenario_param.wired_audio_param.gain_default_L;
        wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2 = open_param->scenario_param.wired_audio_param.gain_default_R;
        DTM_enqueue(DTM_EVENT_ID_WIRED_AUDIO_USB_VOLUME_SMART_BALANCE_ENABLE, (uint32_t)source, false);
    }
    else if (open_param->scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)
    {
        wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1 = open_param->scenario_param.wired_audio_param.gain_default_L;
        wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2 = open_param->scenario_param.wired_audio_param.gain_default_R;
        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = open_param->scenario_param.wired_audio_param.gain_default_L;
        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = open_param->scenario_param.wired_audio_param.gain_default_R;
        wire_audio_usb_vol_balance_handle.game_path_gain_port = wired_audio_handle->gain_port;
    }
}

void wired_audio_usb_dl_vol_balance_deinit(SOURCE source, SINK sink)
{
    UNUSED(sink);
    if (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_0)
    {
        DTM_enqueue(DTM_EVENT_ID_WIRED_AUDIO_USB_VOLUME_SMART_BALANCE_DISABLE, 0, false);
    }
    else if (source->param.data_dl.scenario_sub_id == AUDIO_TRANSMITTER_WIRED_AUDIO_USB_IN_1)
    {
        wire_audio_usb_vol_balance_handle.game_path_gain_port = NULL;
    }
}

void wired_audio_usb_game_chat_volume_smart_balance_enable(void *msg)
{
    uint32_t saved_mask;
    void *owner = msg;
    volume_estimator_port_t *port = NULL;
    volume_estimator_config_t config;
    void *data_buf;
    void *nvkey_buf;

    nvkey_buf = preloader_pisplit_malloc_memory( PRELOADER_D_HIGH_PERFORMANCE , sizeof(wired_audio_usb_vol_balance_nvkey_t));
    if (nvkey_buf == NULL)
    {
        configASSERT(0);
    }
    if (nvkey_read_full_key(NVKEY_DSP_PARA_GAME_CHAT_VOLUME_SMART_BALANCE, nvkey_buf, sizeof(wired_audio_usb_vol_balance_nvkey_t)) != NVDM_STATUS_NAT_OK)
    {
        configASSERT(0);
    }

    /* get chat volume estimator port */
    port = volume_estimator_get_port(owner);
    if (port == NULL)
    {
        configASSERT(0);
    }

    /* init chat volume estimator port */
    config.channel_num = 2;
    config.frame_size = 480; /* 2.5ms*48K*stereo*16bit */
    config.resolution = RESOLUTION_16BIT;
    config.mode = VOLUME_ESTIMATOR_CHAT_INSTANT_MODE;
    config.sample_rate = 48000;
    config.nvkey_para = (void *)&(((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->chat_vol_nvkey);
    volume_estimator_init(port, &config);

    /* malloc memory for queue stereo pcm data */
    data_buf = preloader_pisplit_malloc_memory( PRELOADER_D_HIGH_PERFORMANCE, 480);
    if (data_buf == NULL)
    {
        configASSERT(0);
    }

    /* update state machine */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (wire_audio_usb_vol_balance_handle.port != NULL)
    {
        configASSERT(0);
    }
    wire_audio_usb_vol_balance_handle.port = port;
    wire_audio_usb_vol_balance_handle.nvkey_buf = nvkey_buf;
    wire_audio_usb_vol_balance_handle.enable = (int16_t)(((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->enable);
    wire_audio_usb_vol_balance_handle.gain_setting_update_done = 0;
    wire_audio_usb_vol_balance_handle.current_db = -144*100;
    wire_audio_usb_vol_balance_handle.effective_threshold_db = ((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->effective_threshold_db;
    wire_audio_usb_vol_balance_handle.effective_delay_ms = ((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->effective_delay_ms;
    wire_audio_usb_vol_balance_handle.effective_delay_us_count = 0;
    wire_audio_usb_vol_balance_handle.failure_threshold_db = ((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->failure_threshold_db;
    wire_audio_usb_vol_balance_handle.failure_delay_ms = ((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->failure_delay_ms;
    wire_audio_usb_vol_balance_handle.failure_delay_us_count = 0;
    wire_audio_usb_vol_balance_handle.adjustment_amount_db = ((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->adjustment_amount_db;
    wire_audio_usb_vol_balance_handle.up_step_db = ((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->up_step_db;
    wire_audio_usb_vol_balance_handle.down_step_db = ((wired_audio_usb_vol_balance_nvkey_t *)nvkey_buf)->down_step_db;
    wire_audio_usb_vol_balance_handle.active_flag = false;
    wire_audio_usb_vol_balance_handle.data_size = 480;
    wire_audio_usb_vol_balance_handle.data_buf = data_buf;
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[Game/Chat balance][enable]:%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 16,
                wire_audio_usb_vol_balance_handle.enable,
                wire_audio_usb_vol_balance_handle.active_flag,
                wire_audio_usb_vol_balance_handle.current_db,
                wire_audio_usb_vol_balance_handle.effective_threshold_db,
                wire_audio_usb_vol_balance_handle.effective_delay_ms,
                wire_audio_usb_vol_balance_handle.failure_threshold_db,
                wire_audio_usb_vol_balance_handle.failure_delay_ms,
                wire_audio_usb_vol_balance_handle.adjustment_amount_db,
                wire_audio_usb_vol_balance_handle.up_step_db,
                wire_audio_usb_vol_balance_handle.down_step_db,
                wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1,
                wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2,
                wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1,
                wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2,
                wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1,
                wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2);
}

void wired_audio_usb_game_chat_volume_smart_balance_disable(void *msg)
{
    uint32_t saved_mask;
    volume_estimator_port_t *port;
    UNUSED(msg);

    /* check state machine */
    if ((wire_audio_usb_vol_balance_handle.port == NULL) || (wire_audio_usb_vol_balance_handle.data_buf == NULL) || (wire_audio_usb_vol_balance_handle.nvkey_buf == NULL) || (wire_audio_usb_vol_balance_handle.data_size != 480))
    {
        DSP_MW_LOG_I("[Game/Chat balance][disabel] error:%d,%d, %d, %d, ",4,
        wire_audio_usb_vol_balance_handle.port,wire_audio_usb_vol_balance_handle.data_buf,wire_audio_usb_vol_balance_handle.nvkey_buf,wire_audio_usb_vol_balance_handle.data_size);
        configASSERT(0);
    }
    port = wire_audio_usb_vol_balance_handle.port;

    /* deinit chat volume estimator port */
    volume_estimator_deinit(port);

    /* free memory */
    preloader_pisplit_free_memory(wire_audio_usb_vol_balance_handle.data_buf);
    preloader_pisplit_free_memory(wire_audio_usb_vol_balance_handle.nvkey_buf);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_gain_port != NULL))
    {
        /* process L-channel */
        if (wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1 != wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1)
        {
            /* recover gain to the default settings */
            stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 1, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1);
            wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1;
        }

        /* process R-channel */
        if (wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2 != wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2)
        {
            /* recover gain to the default settings */
            stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 2, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2);
            wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2;
        }

        /* restore ramp-up and ramp-down setting */
        stream_function_sw_gain_configure_gain_up(wire_audio_usb_vol_balance_handle.game_path_gain_port,   1,  1, 2);
        stream_function_sw_gain_configure_gain_up(wire_audio_usb_vol_balance_handle.game_path_gain_port,   2,  1, 2);
        stream_function_sw_gain_configure_gain_down(wire_audio_usb_vol_balance_handle.game_path_gain_port, 1, -1, 2);
        stream_function_sw_gain_configure_gain_down(wire_audio_usb_vol_balance_handle.game_path_gain_port, 2, -1, 2);
    }

    /* reset state machine */
    wire_audio_usb_vol_balance_handle.port = NULL;
    wire_audio_usb_vol_balance_handle.nvkey_buf = NULL;
    wire_audio_usb_vol_balance_handle.enable = 0;
    wire_audio_usb_vol_balance_handle.gain_setting_update_done = 0;
    wire_audio_usb_vol_balance_handle.current_db = -144*100;
    wire_audio_usb_vol_balance_handle.effective_threshold_db = 0;
    wire_audio_usb_vol_balance_handle.effective_delay_ms = 0;
    wire_audio_usb_vol_balance_handle.effective_delay_us_count = 0;
    wire_audio_usb_vol_balance_handle.failure_threshold_db = 0;
    wire_audio_usb_vol_balance_handle.failure_delay_ms = 0;
    wire_audio_usb_vol_balance_handle.failure_delay_us_count = 0;
    wire_audio_usb_vol_balance_handle.adjustment_amount_db = 0;
    wire_audio_usb_vol_balance_handle.up_step_db = 0;
    wire_audio_usb_vol_balance_handle.down_step_db = 0;
    wire_audio_usb_vol_balance_handle.active_flag = 0;
    wire_audio_usb_vol_balance_handle.data_size = 0;
    wire_audio_usb_vol_balance_handle.data_buf = NULL;
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[Game/Chat balance][disable]:%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 16,
                wire_audio_usb_vol_balance_handle.enable,
                wire_audio_usb_vol_balance_handle.active_flag,
                wire_audio_usb_vol_balance_handle.current_db,
                wire_audio_usb_vol_balance_handle.effective_threshold_db,
                wire_audio_usb_vol_balance_handle.effective_delay_ms,
                wire_audio_usb_vol_balance_handle.failure_threshold_db,
                wire_audio_usb_vol_balance_handle.failure_delay_ms,
                wire_audio_usb_vol_balance_handle.adjustment_amount_db,
                wire_audio_usb_vol_balance_handle.up_step_db,
                wire_audio_usb_vol_balance_handle.down_step_db,
                wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1,
                wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2,
                wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1,
                wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2,
                wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1,
                wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2);
}

void wired_audio_usb_game_chat_volume_smart_balance_do_process(void *msg)
{
    uint32_t saved_mask;
    uint32_t samples;
    uint32_t remain_samples;
    volume_estimator_port_t *port;
    int32_t new_gain_1, new_gain_2;
    bool gain_change = false;
    wired_audio_usb_vol_balance_buffer_msg_t buf_msg;

    /* check state machine */
    if ((wire_audio_usb_vol_balance_handle.port == NULL) || (wire_audio_usb_vol_balance_handle.data_buf == NULL) || (wire_audio_usb_vol_balance_handle.nvkey_buf == NULL) || (wire_audio_usb_vol_balance_handle.data_size != 480))
    {
        configASSERT(0);
    }
    port = wire_audio_usb_vol_balance_handle.port;

    /* check if there are new data on the chat path */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    buf_msg.data_buf_1_head = ((wired_audio_usb_vol_balance_buffer_msg_t *)msg)->data_buf_1_head;
    buf_msg.data_buf_1_tail = ((wired_audio_usb_vol_balance_buffer_msg_t *)msg)->data_buf_1_tail;
    buf_msg.data_buf_1_cur  = ((wired_audio_usb_vol_balance_buffer_msg_t *)msg)->data_buf_1_cur;
    buf_msg.data_buf_2_head = ((wired_audio_usb_vol_balance_buffer_msg_t *)msg)->data_buf_2_head;
    buf_msg.data_buf_2_tail = ((wired_audio_usb_vol_balance_buffer_msg_t *)msg)->data_buf_2_tail;
    buf_msg.data_buf_2_cur  = ((wired_audio_usb_vol_balance_buffer_msg_t *)msg)->data_buf_2_cur;
    hal_nvic_restore_interrupt_mask(saved_mask);

    for (size_t i = 0; i < 8; i++)
    {
        if ((buf_msg.data_buf_1_cur == NULL) && (buf_msg.data_buf_2_cur == NULL))
        {
            /* there are not new data on the chat path */
            /* we will set zeros into internal buffer, and the wire_audio_usb_vol_balance_handle.current_db will be very low in 200ms later. Then we recover the volume on the game path */
            memset(wire_audio_usb_vol_balance_handle.data_buf, 0, wire_audio_usb_vol_balance_handle.data_size);
        }
        else
        {
            /* there are new data on the chat path */
            /* copy data into the internal buffer */
            /* buf_msg.data_buf_1_cur & buf_msg.data_buf_2_cur is pointer the frame end address */
            samples = 480/2/sizeof(int16_t);
            if (((buf_msg.data_buf_1_cur-samples) < buf_msg.data_buf_1_head) || ((buf_msg.data_buf_2_cur-samples) < buf_msg.data_buf_2_head))
            {
                /* wrapper case */
                remain_samples = buf_msg.data_buf_1_cur - buf_msg.data_buf_1_head;
                if (remain_samples > samples)
                {
                    configASSERT(0);
                }
                DSP_D2I_BufferCopy_16bit((U16 *)(wire_audio_usb_vol_balance_handle.data_buf),
                                        (buf_msg.data_buf_1_tail-(samples-remain_samples)),
                                        (buf_msg.data_buf_2_tail-(samples-remain_samples)),
                                        samples-remain_samples);
                DSP_D2I_BufferCopy_16bit((U16 *)(wire_audio_usb_vol_balance_handle.data_buf),
                                        (buf_msg.data_buf_1_head),
                                        (buf_msg.data_buf_2_head),
                                        remain_samples);
                buf_msg.data_buf_1_cur = buf_msg.data_buf_1_tail-(samples-remain_samples);
                buf_msg.data_buf_2_cur = buf_msg.data_buf_2_tail-(samples-remain_samples);
            }
            else
            {
                /* normal case */
                DSP_D2I_BufferCopy_16bit((U16 *)(wire_audio_usb_vol_balance_handle.data_buf),
                                        (buf_msg.data_buf_1_cur-samples),
                                        (buf_msg.data_buf_2_cur-samples),
                                        samples);
                buf_msg.data_buf_1_cur -= samples;
                buf_msg.data_buf_2_cur -= samples;
            }
        }

        /* update ramp-up/ramp-down settings at the first time */
        if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_gain_port != NULL) && (wire_audio_usb_vol_balance_handle.gain_setting_update_done == 0))
        {
            /* do ramp-up and ramp-down setting */
            stream_function_sw_gain_configure_gain_up(wire_audio_usb_vol_balance_handle.game_path_gain_port,   1, wire_audio_usb_vol_balance_handle.up_step_db,   2);
            stream_function_sw_gain_configure_gain_up(wire_audio_usb_vol_balance_handle.game_path_gain_port,   2, wire_audio_usb_vol_balance_handle.up_step_db,   2);
            stream_function_sw_gain_configure_gain_down(wire_audio_usb_vol_balance_handle.game_path_gain_port, 1, wire_audio_usb_vol_balance_handle.down_step_db, 2);
            stream_function_sw_gain_configure_gain_down(wire_audio_usb_vol_balance_handle.game_path_gain_port, 2, wire_audio_usb_vol_balance_handle.down_step_db, 2);

            /* set flag */
            wire_audio_usb_vol_balance_handle.gain_setting_update_done = 1;
        }

        /* get the latest volume on chat path and the current_db will be update every 200ms */
        if (volume_estimator_process(port, wire_audio_usb_vol_balance_handle.data_buf, 480, &wire_audio_usb_vol_balance_handle.current_db) != VOLUME_ESTIMATOR_STATUS_OK)
        {
            configASSERT(0);
        }
        //LOG_AUDIO_DUMP(wire_audio_usb_vol_balance_handle.data_buf, 480, AUDIO_CODEC_IN);

#if WIRED_AUDIO_DEBUG_LOG
        DSP_MW_LOG_I("[Game/Chat balance]volume_estimator_process %d, %d, %d, %d, %d, %d, %d, %d, %d, 0x%x, gain: %d, %d, %d, buf.msg:0x%x,0x%x",15,
        wire_audio_usb_vol_balance_handle.enable,
        wire_audio_usb_vol_balance_handle.current_db,
        wire_audio_usb_vol_balance_handle.effective_threshold_db,
        wire_audio_usb_vol_balance_handle.effective_delay_us_count,
        wire_audio_usb_vol_balance_handle.failure_threshold_db,
        wire_audio_usb_vol_balance_handle.failure_delay_us_count,
        wire_audio_usb_vol_balance_handle.adjustment_amount_db,
        wire_audio_usb_vol_balance_handle.up_step_db,
        wire_audio_usb_vol_balance_handle.down_step_db,
        wire_audio_usb_vol_balance_handle.game_path_gain_port,
        wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1,
        wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1,
        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1,
        buf_msg.data_buf_1_cur,
        buf_msg.data_buf_2_cur);
#endif /* WIRED_AUDIO_DEBUG_LOG */

        /* check if needs to do the balance */
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if ((wire_audio_usb_vol_balance_handle.enable) && (wire_audio_usb_vol_balance_handle.game_path_gain_port != NULL))
        {
            if ((wire_audio_usb_vol_balance_handle.current_db >= wire_audio_usb_vol_balance_handle.effective_threshold_db) &&
                (wire_audio_usb_vol_balance_handle.effective_delay_us_count >= wire_audio_usb_vol_balance_handle.effective_delay_ms*1000))
            {
                /* there is chat voice on the chat path */
                /* process L-channel */
                new_gain_1 = (wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1 + wire_audio_usb_vol_balance_handle.adjustment_amount_db);
                new_gain_1 = min(new_gain_1, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1);
                if (wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1 != new_gain_1)
                {
                    /* the default gain is larger than the target and the current gain is not equal to the target, so we need to ramp down the volume on the game path */
                    stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 1, new_gain_1);
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = new_gain_1;
                    gain_change = true;
                }

                /* process R-channel */
                new_gain_2 = (wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2 + wire_audio_usb_vol_balance_handle.adjustment_amount_db);
                new_gain_2 = min(new_gain_2, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2);
                if (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 != new_gain_2)
                {
                    /* the default gain is larger than the target and the current gain is not equal to the target, so we need to ramp down the volume on the game path */
                    stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 2, new_gain_2);
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = new_gain_2;
                    gain_change = true;
                }

                /* set active flag */
                wire_audio_usb_vol_balance_handle.active_flag = true;

                /* reset failure_delay_us_count */
                wire_audio_usb_vol_balance_handle.failure_delay_us_count = 0;
            }
            else if ((wire_audio_usb_vol_balance_handle.current_db >= wire_audio_usb_vol_balance_handle.effective_threshold_db) &&
                    (wire_audio_usb_vol_balance_handle.effective_delay_us_count < wire_audio_usb_vol_balance_handle.effective_delay_ms*1000))
            {
                wire_audio_usb_vol_balance_handle.effective_delay_us_count += 2500;

                /* reset failure_delay_us_count */
                wire_audio_usb_vol_balance_handle.failure_delay_us_count = 0;
            }
            else if ((wire_audio_usb_vol_balance_handle.current_db <= wire_audio_usb_vol_balance_handle.failure_threshold_db) &&
                    (wire_audio_usb_vol_balance_handle.failure_delay_us_count >= wire_audio_usb_vol_balance_handle.failure_delay_ms*1000))
            {
                /* there is no chat voice on the chat path */
                /* process L-channel */
                if (wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1 != wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1)
                {
                    /* recover gain to the default settings */
                    stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 1, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1);
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1;
                    gain_change = true;
                }

                /* process R-channel */
                if (wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2 != wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2)
                {
                    /* recover gain to the default settings */
                    stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 2, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2);
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2;
                    gain_change = true;
                }

                /* set active flag */
                wire_audio_usb_vol_balance_handle.active_flag = false;

                /* reset effective_delay_us_count */
                wire_audio_usb_vol_balance_handle.effective_delay_us_count = 0;
            }
            else if ((wire_audio_usb_vol_balance_handle.current_db <= wire_audio_usb_vol_balance_handle.failure_threshold_db) &&
                    (wire_audio_usb_vol_balance_handle.failure_delay_us_count < wire_audio_usb_vol_balance_handle.failure_delay_ms*1000))
            {
                wire_audio_usb_vol_balance_handle.failure_delay_us_count += 2500;

                /* reset effective_delay_us_count */
                wire_audio_usb_vol_balance_handle.effective_delay_us_count = 0;
                
                /* process L-channel */
                if ((wire_audio_usb_vol_balance_handle.active_flag) && (wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1 < wire_audio_usb_vol_balance_handle.effective_threshold_db))
                {
                    /* In this stage, we should recover gain to the default settings because the chat gain is too less */
                    new_gain_1 = wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1;
                    if (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 != new_gain_1)
                    {
                        stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 1, new_gain_1);
                        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = new_gain_1;
                        gain_change = true;
                    }
                }
                else if (wire_audio_usb_vol_balance_handle.active_flag)
                {
                    /* In this stage, we should set gain to the minimum in default gain and smart-target gain */
                    new_gain_1 = (wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1 + wire_audio_usb_vol_balance_handle.adjustment_amount_db);
                    new_gain_1 = min(new_gain_1, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1);
                    if (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 != new_gain_1)
                    {
                        /* set gain to the minimun settings */
                        stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 1, new_gain_1);
                        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = new_gain_1;
                        gain_change = true;
                    }
                }
                else
                {
                    if (wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1 != wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1)
                    {
                        /* recover gain to the default settings */
                        stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 1, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1);
                        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1 = wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1;
                        gain_change = true;
                    }
                }

                /* process R-channel */
                if ((wire_audio_usb_vol_balance_handle.active_flag) && (wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2 < wire_audio_usb_vol_balance_handle.effective_threshold_db))
                {
                    /* In this stage, we should recover gain to the default settings because the chat gain is too less */
                    new_gain_2 = wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2;
                    if (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 != new_gain_2)
                    {
                        stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 2, new_gain_2);
                        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = new_gain_2;
                        gain_change = true;
                    }
                }
                else if (wire_audio_usb_vol_balance_handle.active_flag)
                {
                    new_gain_2 = (wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2 + wire_audio_usb_vol_balance_handle.adjustment_amount_db);
                    new_gain_2 = min(new_gain_2, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2);
                    if (wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 != new_gain_2)
                    {
                        /* recover gain to the minimun settings */
                        stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 2, new_gain_2);
                        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = new_gain_2;
                        gain_change = true;
                    }
                }
                else
                {
                    if (wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2 != wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2)
                    {
                        /* recover gain to the default settings */
                        stream_function_sw_gain_configure_gain_target(wire_audio_usb_vol_balance_handle.game_path_gain_port, 2, wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2);
                        wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2 = wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2;
                        gain_change = true;
                    }
                }
            }
        }
        hal_nvic_restore_interrupt_mask(saved_mask);
        }
    if (gain_change)
    {
        DSP_MW_LOG_I("[Game/Chat balance][change gain]:%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d\r\n", 13,
                    wire_audio_usb_vol_balance_handle.active_flag,
                    wire_audio_usb_vol_balance_handle.current_db,
                    wire_audio_usb_vol_balance_handle.effective_threshold_db,
                    wire_audio_usb_vol_balance_handle.effective_delay_us_count,
                    wire_audio_usb_vol_balance_handle.failure_threshold_db,
                    wire_audio_usb_vol_balance_handle.failure_delay_us_count,
                    wire_audio_usb_vol_balance_handle.adjustment_amount_db,
                    wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_1,
                    wire_audio_usb_vol_balance_handle.chat_path_default_vol_level.gain_2,
                    wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_1,
                    wire_audio_usb_vol_balance_handle.game_path_default_vol_level.gain_2,
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_1,
                    wire_audio_usb_vol_balance_handle.game_path_balance_vol_level.gain_2);
    }
}
#endif /* AIR_GAME_CHAT_VOLUME_SMART_BALANCE_ENABLE */
