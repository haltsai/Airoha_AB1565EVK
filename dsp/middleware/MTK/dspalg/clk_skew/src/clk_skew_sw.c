/* Copyright Statement:
*
* (C) 2020  Airoha Technology Corp. All rights reserved.
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

/* Includes ------------------------------------------------------------------*/
#include "dsp_feature_interface.h"
#include "dsp_audio_process.h"
#include "clk_skew_sw.h"
#include "skew_ctrl.h"
#include "dsp_dump.h"
#include "dsp_memory.h"
#include "clk_skew_portable.h"
#include "memory_attribute.h"
#include "preloader_pisplit.h"


/* Private define ------------------------------------------------------------*/
#define SW_CLK_SKEW_DEBUG_LOG               0
#define SW_CLK_SKEW_DEBUG_DUMP              0

#define SW_CLK_SKEW_NORMAL                  0
#define SW_CLK_SKEW_SPECIAL                 1


/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static sw_clk_skew_port_t sw_clk_skew_port[SW_CLK_SKEW_PORT_MAX];


/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
sw_clk_skew_port_t *stream_function_sw_clk_skew_get_port(void *owner)
{
    int32_t i;
    uint32_t saved_mask;
    sw_clk_skew_port_t *port = NULL;

    /* Find out a port for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = SW_CLK_SKEW_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if there is unused port */
        if (sw_clk_skew_port[i].owner == NULL)
        {
            port = &sw_clk_skew_port[i];
            continue;
        }

        /* Check if this owner has already owned a sw clk skew */
        if (sw_clk_skew_port[i].owner == owner)
        {
            port = &sw_clk_skew_port[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Port not enough!", 0);
    }

    port->owner = owner;

    return port;
}

void stream_function_sw_clk_skew_init(sw_clk_skew_port_t *port, sw_clk_skew_config_t *config)
{
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Port is NULL!", 0);
        OS_ASSERT(FALSE);
    }

    port->status = SW_CLK_SKEW_STATUS_INIT;
    port->channel = config->channel;
    port->bits = config->bits;
    port->order = config->order;
    port->max_output_size = config->max_output_size;
    port->skew_io_mode = config->skew_io_mode;
    port->skew_compensation_mode = config->skew_compensation_mode;
    port->skew_work_mode = config->skew_work_mode;
    port->compen_samples_in_each_frame = 0;
    port->continuous_frame_size = config->continuous_frame_size;
    port->channel_config = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, config->channel * sizeof(sw_clk_skew_channel_config_t));
    if (port->channel_config == NULL)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Channel is NULL!", 0);
        OS_ASSERT(FALSE);
    }
    memset(port->channel_config, 0, config->channel * sizeof(sw_clk_skew_channel_config_t));
}

void stream_function_sw_clk_skew_deinit(sw_clk_skew_port_t *port)
{
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Port is NULL!", 0);
        OS_ASSERT(FALSE);
    }

    port->status = SW_CLK_SKEW_STATUS_DEINIT;
    port->owner = NULL;
    port->stream = NULL;
    port->channel = 0;
    port->bits = 0;
    port->order = 0;
    port->max_output_size = 0;
    port->skew_io_mode = 0;
    port->skew_compensation_mode = 0;
    port->skew_work_mode = 0;
    port->compen_samples_in_each_frame = 0;
    port->continuous_frame_size = 0;
    preloader_pisplit_free_memory(port->channel_config);
}

ATTR_TEXT_IN_IRAM void stream_function_sw_clk_skew_configure_compensation_samples(sw_clk_skew_port_t *port, S32 compen_samples_in_each_frame)
{
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Port is NULL!", 0);
        OS_ASSERT(FALSE);
    }

    port->compen_samples_in_each_frame = compen_samples_in_each_frame;

    #if SW_CLK_SKEW_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SW_CLK_SKEW][configure_compensation_samples]: %d, %u, %d", 3, port->compen_samples_in_each_frame, current_timestamp, hal_nvic_query_exception_number());
    #endif /* SW_CLK_SKEW_DEBUG_LOG */
}

ATTR_TEXT_IN_IRAM void stream_function_sw_clk_skew_configure_compensation_mode(sw_clk_skew_port_t *port, sw_clk_skew_compensation_mode_t compensation_mode)
{
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Port is NULL!", 0);
        OS_ASSERT(FALSE);
    }

    port->skew_compensation_mode = compensation_mode;
}

ATTR_TEXT_IN_IRAM bool stream_function_sw_clk_skew_initialize(void *para)
{
    int32_t i;
    sw_clk_skew_port_t *port = NULL;
    sw_clk_skew_channel_config_t *channel_config;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    /* Find out the sw clk skew port of this stream */
    for (i = SW_CLK_SKEW_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if this source or sink has already owned a sw clk skew */
        if ((sw_clk_skew_port[i].owner == stream_ptr->source) ||
            (sw_clk_skew_port[i].owner == stream_ptr->sink))
        {
            port = &sw_clk_skew_port[i];
            break;
        }
    }
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    /* status check */
    if (port->status == SW_CLK_SKEW_STATUS_INIT)
    {
        port->status = SW_CLK_SKEW_STATUS_RUNNING;
    }
    else if (port->status == SW_CLK_SKEW_STATUS_RUNNING)
    {
        return false;
    }
    else
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] error status, %d!", 1, port->status);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    port->stream = stream_ptr;

    /* init skew ctrl one by one channel */
    for (i = 0; i < port->channel; i++)
    {
        channel_config = port->channel_config + i;

        /* init clk skew */
        #ifdef MTK_BT_HFP_SPE_ALG_V2
        skew_ctrl_init(&(channel_config->skew_ctrl), port->bits, port->skew_io_mode, port->order);
        #else
        skew_ctrl_init(&(channel_config->skew_ctrl), 1, port->bits, port->skew_io_mode, port->order);
        #endif

        channel_config->p_internal_buffer = (U8 *)DSPMEM_tmalloc(stream_function_get_task(para), port->max_output_size+(port->bits/8), port->stream);

        /* if clk skew is continuous mode */
        if (port->skew_work_mode == SW_CLK_SKEW_CONTINUOUS)
        {
            skew_ctrl_set_input_framesize(&(channel_config->skew_ctrl), port->continuous_frame_size);
        }
    }

    DSP_MW_LOG_I("[SW_CLK_SKEW] CLK SKEW open successfully!, %u, %u, %u, %u, %u, %u, %u", 7,
                 port->channel,
                 port->bits,
                 port->skew_io_mode,
                 port->order,
                 port->max_output_size,
                 port->skew_work_mode,
                 port->continuous_frame_size);

    return false;
}

ATTR_TEXT_IN_IRAM bool stream_function_sw_clk_skew_process(void *para)
{
    int32_t i;
    uint32_t j;
    sw_clk_skew_port_t *port = NULL;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    #if SW_CLK_SKEW_DEBUG_DUMP
    S8* buf1 = (S8*)stream_function_get_1st_inout_buffer(para);
    S8* buf2 = (S8*)stream_function_get_2nd_inout_buffer(para);
    #endif /* SW_CLK_SKEW_DEBUG_DUMP */
    U16 in_frame_size = stream_function_get_output_size(para);
    U16 sample_bytes = (stream_function_get_output_resolution(para) == RESOLUTION_16BIT) ? sizeof(S16) : sizeof(S32);
    U16 channels = stream_function_get_channel_number(para);
    U32 total_in_bytes, total_ou_bytes;
    U32 process_in_bytes, process_ou_bytes;
    S8* inbuf = NULL;
    S8* oubuf = NULL;
    skew_ctrl_t *p_skew_ctrl;
    S32 compen_samples = 0;
    S32 compen_samples_1 = 0;
    S32 compen_samples_2 = 0;
    U16 in_frame_size_1 = 0;
    U16 count = 0;
    U32 process_mode = SW_CLK_SKEW_NORMAL;
    sw_clk_skew_channel_config_t *channel_config;

    #if SW_CLK_SKEW_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SW_CLK_SKEW][start]: %d, %u, %u, %d", 4, in_frame_size, channels, current_timestamp, hal_nvic_query_exception_number());
    #endif /* SW_CLK_SKEW_DEBUG_LOG */

    #if SW_CLK_SKEW_DEBUG_DUMP
    static U16 *temp_buf = NULL;
    if (temp_buf == NULL)
    {
        temp_buf = malloc(4*48*2*2*2 + 4*3);
    }
    DSP_D2I_BufferCopy_16bit((U16 *)((U32)temp_buf+4*3), buf1, buf2, in_frame_size/2);
    #endif /* SW_CLK_SKEW_DEBUG_DUMP */

    /* Find out the sw clk skew port of this stream */
    for (i = SW_CLK_SKEW_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if this source or sink has already owned a sw clk skew */
        if ((sw_clk_skew_port[i].owner == stream_ptr->source) ||
            (sw_clk_skew_port[i].owner == stream_ptr->sink))
        {
            port = &sw_clk_skew_port[i];
            break;
        }
    }
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    if (!in_frame_size) {
        // DSP_MW_LOG_I("[SW_CLK_SKEW] Input length is 0!", 0);
        return FALSE;
    }

    /* check channels */
    if (channels != port->channel)
    {
        DSP_MW_LOG_E("[SW_CLK_SKEW] Channel Number is dismatch, %u, %u!", 2, channels, port->channel);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    if (port->skew_work_mode == SW_CLK_SKEW_CONTINUOUS)
    {
        for(i = 1; i <= port->channel; i++)
        {
            /* parameters init */
            compen_samples = port->compen_samples_in_each_frame;
            process_in_bytes = in_frame_size;
            process_ou_bytes = port->max_output_size;
            channel_config = port->channel_config + (i-1);
            p_skew_ctrl = &(channel_config->skew_ctrl);
            inbuf = (S8 *)(channel_config->p_internal_buffer);
            oubuf = stream_function_get_inout_buffer(para, i);

            /* check if the input size is matched */
            if (port->skew_io_mode == C_Skew_Oup)
            {
                /* the clk skew is continuous mode , the input size is fixed, the output size is input size +1 or -1 sample */
                if (in_frame_size != port->continuous_frame_size)
                {
                    DSP_MW_LOG_E("[SW_CLK_SKEW] input size is dismatch, %u, %u!", 2, in_frame_size, port->continuous_frame_size);
                    OS_ASSERT(FALSE);
                    return TRUE;
                }
            }
            else if (port->skew_io_mode == C_Skew_Inp)
            {
                /* the clk skew is continuous mode , the output size is fixed, the input size is output size +1 or -1 sample */
                if ((in_frame_size < (port->continuous_frame_size - port->bits/8)) ||
                    (in_frame_size > (port->continuous_frame_size + port->bits/8)))
                {
                    DSP_MW_LOG_E("[SW_CLK_SKEW] input size is dismatch, %u, %u!", 2, in_frame_size, port->continuous_frame_size);
                    OS_ASSERT(FALSE);
                    return TRUE;
                }
            }
            else
            {
                DSP_MW_LOG_E("[SW_CLK_SKEW] io mode is not supported, %d!", 1, port->skew_io_mode);
                OS_ASSERT(FALSE);
                return TRUE;
            }

            /* copy original data into internal buffer */
            // memcpy(inbuf, oubuf, in_frame_size);
            if (port->bits == 16)
            {
                *((uint16_t *)inbuf + 0) = (uint16_t)(channel_config->last_sample_value);
                for (j = 0; j < (in_frame_size/2); j++)
                {
                    *((uint16_t *)inbuf + j + 1) = *((uint16_t *)oubuf + j);
                }
                channel_config->last_sample_value = *((uint16_t *)oubuf + (in_frame_size/2) - 1);
            }
            else
            {
                *((uint32_t *)inbuf + 0) = (uint32_t)(channel_config->last_sample_value);
                for (j = 0; j < (in_frame_size/4); j++)
                {
                    *((uint32_t *)inbuf + j + 1) = *((uint32_t *)oubuf + j);
                }
                channel_config->last_sample_value = *((uint32_t *)oubuf + (in_frame_size/4) - 1);
            }

            switch (port->skew_compensation_mode)
            {
                case SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_1_FRAME:
                    /* compensate one sample in every input frame */
                    if (compen_samples == 0)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Pass);
                    }
                    else if (compen_samples == 1)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Inc);
                    }
                    else if (compen_samples == -1)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Dec);
                    }
                    else
                    {
                        DSP_MW_LOG_E("[SW_CLK_SKEW] compen_samples is not support in CONTINUOUS mode, %d!", 1, compen_samples);
                        OS_ASSERT(FALSE);
                        return TRUE;
                    }

                    /* check if the output size is matched */
                    if (port->skew_io_mode == C_Skew_Oup)
                    {
                        /* the clk skew is continuous mode , the input size is fixed, the output size is input size +1 or -1 sample */
                        if (compen_samples == 0)
                        {
                            if (in_frame_size != process_ou_bytes)
                            {
                                DSP_MW_LOG_E("[SW_CLK_SKEW] output size is dismatched, %d, %d!", 2, in_frame_size, process_ou_bytes);
                                OS_ASSERT(FALSE);
                                return TRUE;
                            }
                        }
                        else if (compen_samples == 1)
                        {
                            if ((in_frame_size+port->bits/8) != process_ou_bytes)
                            {
                                DSP_MW_LOG_E("[SW_CLK_SKEW] output size is dismatched, %d, %d!", 2, in_frame_size, process_ou_bytes);
                                OS_ASSERT(FALSE);
                                return TRUE;
                            }
                        }
                        else if (compen_samples == -1)
                        {
                            if ((U32)(in_frame_size-port->bits/8) != process_ou_bytes)
                            {
                                DSP_MW_LOG_E("[SW_CLK_SKEW] output size is dismatched, %d, %d!", 2, in_frame_size, process_ou_bytes);
                                OS_ASSERT(FALSE);
                                return TRUE;
                            }
                        }
                    }
                    else if (port->skew_io_mode == C_Skew_Inp)
                    {
                        /* the clk skew is continuous mode , the output size is fixed, the input size is output size +1 or -1 sample */
                        if (port->continuous_frame_size != process_ou_bytes)
                        {
                            DSP_MW_LOG_E("[SW_CLK_SKEW] output size is dismatched, %d, %d!", 2, port->continuous_frame_size, process_ou_bytes);
                            OS_ASSERT(FALSE);
                            return TRUE;
                        }
                    }
                    break;

                case SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_4_FRAME:
                    /* compensate one sample in every input frame */
                    if (compen_samples == 0)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Pass_p25);
                    }
                    else if (compen_samples == 1)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Inc_p25);
                    }
                    else if (compen_samples == -1)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Dec_p25);
                    }
                    else
                    {
                        DSP_MW_LOG_E("[SW_CLK_SKEW] compen_samples is not support in CONTINUOUS mode, %d!", 1, compen_samples);
                        OS_ASSERT(FALSE);
                        return TRUE;
                    }

                    /* check if the output size is matched */
                    if (port->skew_io_mode == C_Skew_Oup)
                    {
                    }
                    else if (port->skew_io_mode == C_Skew_Inp)
                    {
                        DSP_MW_LOG_E("[SW_CLK_SKEW] io_mode and compensation_mode is not right, %d, %d!", 2, port->skew_io_mode, port->skew_compensation_mode);
                        OS_ASSERT(FALSE);
                        return TRUE;
                    }
                    break;

                case SW_CLK_SKEW_COMPENSATION_1_SAMPLE_IN_8_FRAME:
                    /* compensate one sample in every input frame */
                    if (compen_samples == 0)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Pass_p125);
                    }
                    else if (compen_samples == 1)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Inc_p125);
                    }
                    else if (compen_samples == -1)
                    {
                        skew_ctrl_process(p_skew_ctrl, inbuf, (U16 *)&process_in_bytes, oubuf, (U16 *)&process_ou_bytes, C_Skew_Dec_p125);
                    }
                    else
                    {
                        DSP_MW_LOG_E("[SW_CLK_SKEW] compen_samples is not support in CONTINUOUS mode, %d!", 1, compen_samples);
                        OS_ASSERT(FALSE);
                        return TRUE;
                    }

                    /* check if the output size is matched */
                    if (port->skew_io_mode == C_Skew_Oup)
                    {
                    }
                    else if (port->skew_io_mode == C_Skew_Inp)
                    {
                        DSP_MW_LOG_E("[SW_CLK_SKEW] io_mode and compensation_mode is not right, %d, %d!", 2, port->skew_io_mode, port->skew_compensation_mode);
                        OS_ASSERT(FALSE);
                        return TRUE;
                    }
                    break;

                default:
                    DSP_MW_LOG_E("[SW_CLK_SKEW] compensation mode is not supported, %d!", 1, port->skew_compensation_mode);
                    OS_ASSERT(FALSE);
                    return TRUE;
            }
        }

        /* configure output size */
        stream_function_modify_output_size(para, process_ou_bytes);

        #if SW_CLK_SKEW_DEBUG_DUMP
        *((S32 *)temp_buf+0) = in_frame_size;
        *((S32 *)temp_buf+1) = process_ou_bytes;
        *((S32 *)temp_buf+2) = port->compen_samples_in_each_frame;
        DSP_D2I_BufferCopy_16bit((U16 *)((U32)temp_buf+4*3+in_frame_size/2*2*2),
                                buf1,
                                buf2,
                                process_ou_bytes/2);
        LOG_AUDIO_DUMP(temp_buf, 4*3+in_frame_size/2*2*2+process_ou_bytes/2*2*2, 13);
        #endif /* SW_CLK_SKEW_DEBUG_DUMP */
    }
    else
    {
        /* check if the compen_samples is too large beacuse clock skew input frame size is at least 2 samples */
        if (((port->compen_samples_in_each_frame > 0) && (((in_frame_size / sample_bytes) / port->compen_samples_in_each_frame) <= 1)) ||
            ((port->compen_samples_in_each_frame < 0) && (((in_frame_size / sample_bytes) / -port->compen_samples_in_each_frame) <= 1)))
        {
            process_mode = SW_CLK_SKEW_SPECIAL;
        }
        else
        {
            process_mode = SW_CLK_SKEW_NORMAL;
        }

        switch(process_mode)
        {
            /* In this mode, every compensate sample wil be allocated equally to the in_frame */
            /* For example, input frame samples is 48 and compensate sample is +2,
                so the output will be 24+1(compensate)+24+1(compensate) = 50 samples*/
            /* Other example, input frame samples is 48 and compensate sample is -2,
                so the output will be 24-1(compensate)+24-1(compensate) = 46 samples*/
            case SW_CLK_SKEW_NORMAL:
                /* compensate data in each frame one by one channel */
                for(i = 1; i <= port->channel; i++)
                {
                    /* parameters init */
                    compen_samples = port->compen_samples_in_each_frame;
                    if (compen_samples > 0)
                    {
                        process_in_bytes = ((in_frame_size / sample_bytes) / (compen_samples)) * sample_bytes;
                    }
                    else if (compen_samples < 0)
                    {
                        process_in_bytes = ((in_frame_size / sample_bytes) / (-compen_samples)) * sample_bytes;
                    }
                    else
                    {
                        process_in_bytes = 0;
                        break;
                    }
                    process_ou_bytes = 0;
                    total_in_bytes = 0;
                    total_ou_bytes = 0;
                    channel_config = port->channel_config + (i-1);
                    p_skew_ctrl = &(channel_config->skew_ctrl);
                    inbuf = (S8 *)(channel_config->p_internal_buffer);
                    oubuf = stream_function_get_inout_buffer(para, i);

                    /* copy original data into internal buffer */
                    // memcpy(inbuf, oubuf, in_frame_size);
                    for (j = 0; j < (in_frame_size/2); j++)
                    {
                        *((uint16_t *)inbuf + j )= *((uint16_t *)oubuf + j);
                    }

                    /* compensate one sample on every loop */
                    while (compen_samples != 0)
                    {
                        /* check output size */
                        if(total_ou_bytes >= port->max_output_size)
                        {
                            DSP_MW_LOG_E("[SW_CLK_SKEW] output size is larger than max, %d, %d!", 2, total_ou_bytes, port->max_output_size);
                            OS_ASSERT(FALSE);
                            break;
                        }

                        process_ou_bytes = port->max_output_size - total_ou_bytes;

                        /* In this case, the output frame size will be the input frame size + 1 in every loop */
                        if(compen_samples > 0)
                        {
                            skew_ctrl_set_input_framesize(p_skew_ctrl, process_in_bytes);
                            skew_ctrl_process(p_skew_ctrl, inbuf+total_in_bytes, (U16 *)&process_in_bytes, oubuf+total_ou_bytes, (U16 *)&process_ou_bytes, C_Skew_Inc);
                            if ((process_ou_bytes - process_in_bytes) != (port->bits/8))
                            {
                                DSP_MW_LOG_E("[SW_CLK_SKEW] clk skew error, %d, %d!", 2, total_in_bytes, total_ou_bytes);
                                OS_ASSERT(FALSE);
                                return TRUE;
                            }
                            total_in_bytes += process_in_bytes;
                            total_ou_bytes += process_ou_bytes;

                            compen_samples -= 1;
                            if(compen_samples == 0)
                            {
                                if(total_in_bytes < in_frame_size)
                                {
                                    // memcpy(oubuf+total_ou_bytes, inbuf+total_in_bytes, in_frame_size-total_in_bytes);
                                    for (j = 0; j < ((in_frame_size-total_in_bytes)/2); j++)
                                    {
                                        *((uint16_t *)(oubuf+total_ou_bytes) + j )= *((uint16_t *)(inbuf+total_in_bytes) + j);
                                    }
                                    total_ou_bytes += in_frame_size-total_in_bytes;
                                    total_in_bytes += in_frame_size-total_in_bytes;
                                }
                                else if(total_in_bytes > in_frame_size)
                                {
                                    DSP_MW_LOG_E("[SW_CLK_SKEW] clk skew error, %d, %d!", 2, total_in_bytes, in_frame_size);
                                    OS_ASSERT(FALSE);
                                    return TRUE;
                                }
                                break;
                            }
                        }
                        /* In this case, the output frame size will be the input frame size - 1 in every loop */
                        else if(compen_samples < 0)
                        {
                            skew_ctrl_set_input_framesize(p_skew_ctrl, process_in_bytes);
                            skew_ctrl_process(p_skew_ctrl, inbuf+total_in_bytes, (U16 *)&process_in_bytes, oubuf+total_ou_bytes, (U16 *)&process_ou_bytes, C_Skew_Dec);
                            if ((process_in_bytes - process_ou_bytes) != (port->bits/8))
                            {
                                DSP_MW_LOG_E("[SW_CLK_SKEW] clk skew error, %d, %d!", 2, total_in_bytes, total_ou_bytes);
                                OS_ASSERT(FALSE);
                                return TRUE;
                            }
                            total_in_bytes += process_in_bytes;
                            total_ou_bytes += process_ou_bytes;

                            compen_samples += 1;
                            if(compen_samples == 0)
                            {
                                if(total_in_bytes < in_frame_size)
                                {
                                    // memcpy(oubuf+total_ou_bytes, inbuf+total_in_bytes, in_frame_size-total_in_bytes);
                                    for (j = 0; j < ((in_frame_size-total_in_bytes)/2); j++)
                                    {
                                        *((uint16_t *)(oubuf+total_ou_bytes) + j )= *((uint16_t *)(inbuf+total_in_bytes) + j);
                                    }
                                    total_ou_bytes += in_frame_size-total_in_bytes;
                                    total_in_bytes += in_frame_size-total_in_bytes;
                                }
                                else if(total_in_bytes > in_frame_size)
                                {
                                    DSP_MW_LOG_E("[SW_CLK_SKEW] clk skew error, %d, %d!", 2, total_in_bytes, in_frame_size);
                                    OS_ASSERT(FALSE);
                                    return TRUE;
                                }
                                break;
                            }
                        }
                    }

                    /* check size */
                    if ((port->compen_samples_in_each_frame != 0) &&
                        (total_ou_bytes != (U32)(in_frame_size + port->compen_samples_in_each_frame * port->bits/8)))
                    {
                        DSP_MW_LOG_E("[SW_CLK_SKEW] output size is unexcepted, %d, %d, %d, %d!", 4,
                                    total_in_bytes,
                                    total_ou_bytes,
                                    in_frame_size,
                                    in_frame_size + port->compen_samples_in_each_frame * port->bits/8);
                    }
                }

                break;

            /* In this mode, if the compensate sample interval is less than 2 samples, the compensate sample interval will be set 2 samples at this time,
            then the remain compensate wil be allocated equally to the result of the output till the compensate sample interval is larger than 1 samples */
            /* For example, input frame samples is 48 and compensate sample is +48,
                so the first output will be 2+1(compensate)+2+1(compensate)+...+ (total repeated 24 times) +...+2+1(compensate) = 72 samples,
                the second output will be 3+1(compensate)+3+1(compensate)+...+ (total repeated 24 times) +...+3+1(compensate) = 96 samples, */
            /* For example, input frame samples is 48 and compensate sample is -50,
                so the output will be 48-48(compensate) = 0 samples*/
            case SW_CLK_SKEW_SPECIAL:
                /* compensate data in each frame one by one channel */
                for(i = 1; i <= port->channel; i++)
                {
                    /* parameters init */
                    compen_samples = port->compen_samples_in_each_frame;
                    channel_config = port->channel_config + (i-1);
                    p_skew_ctrl = &(channel_config->skew_ctrl);
                    inbuf = (S8 *)(channel_config->p_internal_buffer);
                    oubuf = stream_function_get_inout_buffer(para, i);

                    if (compen_samples > 0)
                    {
                        count = 0;
                        in_frame_size_1 = in_frame_size;
                        while(compen_samples != 0)
                        {
                            process_in_bytes = ((in_frame_size_1 / sample_bytes) / (compen_samples)) * sample_bytes;
                            if (process_in_bytes < (U32)(2*(port->bits/8)))
                            {
                                /* at least 2 samples */
                                process_in_bytes = 2 * (port->bits/8);
                            }
                            compen_samples_1 = in_frame_size_1 / process_in_bytes;
                            compen_samples_2 = in_frame_size_1 / process_in_bytes;
                            process_ou_bytes = port->max_output_size;
                            total_in_bytes = 0;
                            total_ou_bytes = 0;
                            while (compen_samples_1 != 0)
                            {
                                skew_ctrl_set_input_framesize(p_skew_ctrl, process_in_bytes);
                                skew_ctrl_process(p_skew_ctrl, inbuf+total_in_bytes, (U16 *)&process_in_bytes, oubuf+total_ou_bytes, (U16 *)&process_ou_bytes, C_Skew_Inc);
                                if ((process_ou_bytes - process_in_bytes) != (port->bits/8))
                                {
                                    DSP_MW_LOG_E("[SW_CLK_SKEW] clk skew error, %d, %d!", 2, total_in_bytes, total_ou_bytes);
                                    OS_ASSERT(FALSE);
                                    return TRUE;
                                }
                                total_in_bytes += process_in_bytes;
                                total_ou_bytes += process_ou_bytes;

                                compen_samples_1 -= 1;
                                if(compen_samples_1 == 0)
                                {
                                    if(total_in_bytes < in_frame_size_1)
                                    {
                                        // memcpy(oubuf+total_ou_bytes, inbuf+total_in_bytes, in_frame_size_1-total_in_bytes);
                                        for (j = 0; j < ((in_frame_size_1-total_in_bytes)/2); j++)
                                        {
                                            *((uint16_t *)(oubuf+total_ou_bytes) + j )= *((uint16_t *)(inbuf+total_in_bytes) + j);
                                        }
                                        total_ou_bytes += in_frame_size_1-total_in_bytes;
                                        total_in_bytes += in_frame_size_1-total_in_bytes;
                                    }
                                    else if(total_in_bytes > in_frame_size_1)
                                    {
                                        DSP_MW_LOG_E("[SW_CLK_SKEW] clk skew error, %d, %d!", 2, total_in_bytes, in_frame_size_1);
                                        OS_ASSERT(FALSE);
                                        return TRUE;
                                    }

                                    if(total_ou_bytes != (U32)(in_frame_size_1 + compen_samples_2 * port->bits/8))
                                    {
                                        DSP_MW_LOG_E("[SW_CLK_SKEW] clk skew error, %d, %d, %d, %u, %u, %u, %u, %u!", 8,
                                                        compen_samples_1,
                                                        compen_samples_2,
                                                        in_frame_size_1,
                                                        total_in_bytes,
                                                        total_ou_bytes,
                                                        in_frame_size,
                                                        process_in_bytes,
                                                        process_ou_bytes);
                                        OS_ASSERT(FALSE);
                                        return TRUE;
                                    }
                                }
                            }

                            /* wrapper buffer pointer */
                            inbuf = (S8*)((U32)inbuf ^ (U32)oubuf);
                            oubuf = (S8*)((U32)inbuf ^ (U32)oubuf);
                            inbuf = (S8*)((U32)inbuf ^ (U32)oubuf);

                            count += 1;

                            in_frame_size_1 = total_ou_bytes;
                            compen_samples = compen_samples - compen_samples_2;
                            if (compen_samples < 0)
                            {
                                DSP_MW_LOG_E("[SW_CLK_SKEW] clk skew error, %d, %d, %d, %u, %u, %u, %u, %u!", 8,
                                                compen_samples_1,
                                                compen_samples_2,
                                                in_frame_size_1,
                                                total_in_bytes,
                                                total_ou_bytes,
                                                in_frame_size,
                                                process_in_bytes,
                                                process_ou_bytes);
                                OS_ASSERT(FALSE);
                                return TRUE;
                            }
                        }

                        /* check size */
                        if ((port->compen_samples_in_each_frame != 0) &&
                            (total_ou_bytes != (U32)(in_frame_size + port->compen_samples_in_each_frame * port->bits/8)))
                        {
                            DSP_MW_LOG_E("[SW_CLK_SKEW] output size is unexcepted, %d, %d, %d, %d!", 4,
                                        total_in_bytes,
                                        total_ou_bytes,
                                        in_frame_size,
                                        in_frame_size + port->compen_samples_in_each_frame * port->bits/8);
                        }

                        /* check result if in internal buf */
                        if (count%2 == 1)
                        {
                            /* in here, it means inbuf is pointer to internal buf, outbuf is pointer to stream buf */
                            /* at now, the result is in internal buf and do not needs do the second compensate,
                            so we need to copy them into the stream buffer */
                            // memcpy(oubuf, inbuf, total_ou_bytes);
                            for (j = 0; j < (total_ou_bytes/2); j++)
                            {
                                *((uint16_t *)oubuf + j )= *((uint16_t *)inbuf + j);
                            }
                        }
                    }
                    else if (compen_samples < 0)
                    {
                        total_ou_bytes = 0;
                    }
                    else
                    {
                    }
                }

                break;

            default:
                DSP_MW_LOG_E("[SW_CLK_SKEW] Unknow mode, %u, %u!", 2, process_mode, process_in_bytes);
                OS_ASSERT(FALSE);
        }

        /* configure output size */
        if (port->compen_samples_in_each_frame != 0)
        {
            stream_function_modify_output_size(para, total_ou_bytes);

            #if SW_CLK_SKEW_DEBUG_DUMP
            *((S32 *)temp_buf+0) = in_frame_size;
            *((S32 *)temp_buf+1) = total_ou_bytes;
            *((S32 *)temp_buf+2) = port->compen_samples_in_each_frame;
            DSP_D2I_BufferCopy_16bit((U16 *)((U32)temp_buf+4*3+in_frame_size/2*2*2),
                                    buf1,
                                    buf2,
                                    total_ou_bytes/2);
            LOG_AUDIO_DUMP(temp_buf, 4*3+in_frame_size/2*2*2+total_ou_bytes/2*2*2, 13);
            #endif /* SW_CLK_SKEW_DEBUG_DUMP */
        }
        else
        {
            stream_function_modify_output_size(para, in_frame_size);

            #if SW_CLK_SKEW_DEBUG_DUMP
            // *((S32 *)temp_buf+0) = in_frame_size;
            // *((S32 *)temp_buf+1) = total_ou_bytes;
            // *((S32 *)temp_buf+2) = port->compen_samples_in_each_frame;
            // DSP_D2I_BufferCopy_16bit((U16 *)((U32)temp_buf+4*3+in_frame_size/2*2*2),
            //                          buf1,
            //                          buf2,
            //                          in_frame_size/2);
            // LOG_AUDIO_DUMP(temp_buf, 4*3+in_frame_size/2*2*2+in_frame_size/2*2*2, 13);
            #endif /* SW_CLK_SKEW_DEBUG_DUMP */
        }
    }

    #if SW_CLK_SKEW_DEBUG_LOG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SW_CLK_SKEW][finish]:%u, %u, %u, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %u, %d", 16,
                    inbuf,
                    oubuf,
                    process_mode,
                    sample_bytes,
                    in_frame_size,
                    count,
                    compen_samples_1,
                    compen_samples_2,
                    in_frame_size_1,
                    process_in_bytes,
                    total_in_bytes,
                    process_ou_bytes,
                    total_ou_bytes,
                    port->compen_samples_in_each_frame,
                    port->channel,
                    current_timestamp,
                    hal_nvic_query_exception_number());
    #endif /* SW_CLK_SKEW_DEBUG_LOG */

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&(port->finish_gpt_count));

    return false;
}
