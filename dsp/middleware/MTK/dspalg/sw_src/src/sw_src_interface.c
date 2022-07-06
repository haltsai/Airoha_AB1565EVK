/* Copyright Statement:
*
* (C) 2020 Airoha Technology Corp. All rights reserved.
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

#if defined(AIR_SOFTWARE_SRC_ENABLE)

/* Includes ------------------------------------------------------------------*/
#include "dsp_feature_interface.h"
#include "dsp_audio_process.h"
#include "sw_src_interface.h"
#include "dsp_dump.h"
#include "dsp_memory.h"
#include "memory_attribute.h"
#include "preloader_pisplit.h"

/* Private define ------------------------------------------------------------*/
#define SW_SRC_DEBUG_LOG                0
#define SW_SRC_DEBUG_DUMP               0

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static sw_src_port_t sw_src_port[SW_SRC_PORT_MAX];

/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
 * @brief This function is used to converter sample rate format.
 *
 * @param fs is the sample rate format in stream_samplerate_t.
 * @return uint32_t is the sample rate in HZ.
 */
static uint32_t sw_src_fs_converter(stream_samplerate_t fs)
{
    switch(fs)
    {
        case FS_RATE_44_1K:
            return 44100;

        case FS_RATE_8K:
        case FS_RATE_16K:
        case FS_RATE_24K:
        case FS_RATE_32K:
        case FS_RATE_48K:
            return fs * 1000;

        default:
            DSP_MW_LOG_E("[SW_SRC] sample rate is not supported!", 0);
            OS_ASSERT(FALSE);
            return fs;
    }
}

/* Public functions ----------------------------------------------------------*/
/**
 * @brief This function is used to get the sw_src port.
 *        If the owner does not have a sw_src port, it will malloc a port for this owner.
 *        If the owner have a sw_src port, it will return the port directly.
 *        If the owner is NULL, it will return the first unused port.
 *
 * @param owner is who want to get or query a sw_src port or NULL.
 * @return sw_src_port_t* is the result.
 */
sw_src_port_t *stream_function_sw_src_get_port(void *owner)
{
    int32_t i;
    uint32_t saved_mask;
    sw_src_port_t *port = NULL;

    /* Find out a port for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = SW_SRC_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if there is unused port */
        if (sw_src_port[i].owner == NULL)
        {
            port = &sw_src_port[i];
        }

        /* Check if this owner has already owned a sw src */
        if (sw_src_port[i].owner == owner)
        {
            port = &sw_src_port[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_SRC] Port not enough!", 0);
        assert(0);
        return port;
    }

    port->owner = owner;

    return port;
}

/**
 * @brief This function is used to configure the sw_src port.
 *
 * @param port is which port needs to be configured.
 * @param config is pointer to the configuration settings.
 */
void stream_function_sw_src_init(sw_src_port_t *port, sw_src_config_t *config)
{
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_SRC] Port is NULL!", 0);
        OS_ASSERT(FALSE);
    }

    /* Config SW SRC */
    port->status            = SW_SRC_PORT_STATUS_INIT;
    port->mode              = config->mode;
    port->channel_num       = config->channel_num;
    port->in_res            = config->in_res;
    port->in_sampling_rate  = config->in_sampling_rate;
    port->in_frame_size_max = config->in_frame_size_max;
    port->out_res           = config->out_res;
    port->out_sampling_rate = config->out_sampling_rate;
    port->out_frame_size_max= config->out_frame_size_max;
    port->work_mem_size     = 0;
    port->internal_buf_size = 0;
    port->temp_buf_size     = 0;
}

/**
 * @brief This function is used to deinit the sw_src port.
 *
 * @param port is which port needs to be freed.
 */
void stream_function_sw_src_deinit(sw_src_port_t *port)
{
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_SRC] Port is NULL!", 0);
        OS_ASSERT(FALSE);
    }

    port->status            = SW_SRC_PORT_STATUS_DEINIT;
    port->owner             = NULL;
    port->stream            = NULL;
    port->mode              = 0;
    port->channel_num       = 0;
    port->in_res            = 0;
    port->in_sampling_rate  = 0;
    port->in_frame_size_max = 0;
    port->out_res           = 0;
    port->out_sampling_rate = 0;
    port->out_frame_size_max= 0;

    if (port->work_mem_ptr != NULL)
    {
        preloader_pisplit_free_memory(port->work_mem_ptr);
        port->work_mem_ptr = NULL;
    }
    port->work_mem_size = 0;

    if (port->internal_buf_ptr != NULL)
    {
        preloader_pisplit_free_memory(port->internal_buf_ptr);
        port->internal_buf_ptr = NULL;
    }
    port->internal_buf_size = 0;

    if (port->temp_buf_ptr != NULL)
    {
        preloader_pisplit_free_memory(port->temp_buf_ptr);
        port->temp_buf_ptr = NULL;
    }
    port->temp_buf_size = 0;
}

/**
 * @brief This function is used to initialize the sw src run-time environment.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
ATTR_TEXT_IN_IRAM bool stream_function_sw_src_initialize(void *para)
{
    int32_t i;
    sw_src_port_t *port = NULL;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    uint32_t saved_mask;
    Blisrc_Handle **current_handle;
    void *current_internal_buf;

    /* Find out the sw src port of this stream */
    for (i = SW_SRC_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if this source or sink has already owned a sw src */
        if ((sw_src_port[i].owner == stream_ptr->source) ||
            (sw_src_port[i].owner == stream_ptr->sink))
        {
            port = &sw_src_port[i];
            break;
        }
    }
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_SRC] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    /* status check */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (port->status == SW_SRC_PORT_STATUS_INIT)
    {
        port->status = SW_SRC_PORT_STATUS_RUNNING;
    }
    else if (port->status == SW_SRC_PORT_STATUS_RUNNING)
    {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return FALSE;
    }
    else
    {
        DSP_MW_LOG_I("[SW_SRC] error status:%d", 1, port->status);
        assert(0);
        return TRUE;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    /* Config SW SRC */
    port->stream = stream_ptr;
    port->first_time_flag = true;
    port->blisrc_param.in_sampling_rate = port->in_sampling_rate;
    port->blisrc_param.in_channel = 1;
    port->blisrc_param.out_sampling_rate = port->out_sampling_rate;
    port->blisrc_param.out_channel = 1;
    if ((port->in_res == RESOLUTION_16BIT) && (port->out_res == RESOLUTION_16BIT))
    {
        port->blisrc_param.PCM_Format = BLISRC_IN_Q1P15_OUT_Q1P15_I;
    }
    else if ((port->in_res == RESOLUTION_16BIT) && (port->out_res == RESOLUTION_32BIT))
    {
        port->blisrc_param.PCM_Format = BLISRC_IN_Q1P15_OUT_Q1P31_I;
    }
    else if ((port->in_res == RESOLUTION_32BIT) && (port->out_res == RESOLUTION_32BIT))
    {
        port->blisrc_param.PCM_Format = BLISRC_IN_Q1P31_OUT_Q1P31_I;
    }
    else
    {
        DSP_MW_LOG_E("[SW_SRC] resolution is not supported!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    /* get working memory */
    port->work_mem_size = (port->in_frame_size_max + sizeof(Blisrc_Handle *) + 3)&0xfffffffc; //4B aligned
    port->work_mem_ptr = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, port->work_mem_size * port->channel_num);
    if (port->work_mem_ptr == NULL)
    {
        assert(0);
    }

    /* Get buffer size */
    Blisrc_GetBufferSize(&(port->internal_buf_size), &(port->temp_buf_size), &(port->blisrc_param));
    port->internal_buf_size = (port->internal_buf_size + 3)&0xfffffffc;
    port->temp_buf_size     = (port->temp_buf_size + 3)&0xfffffffc;

    #if SW_SRC_DEBUG_LOG
    DSP_MW_LOG_I("[SW_SRC][buf size]:%u, %u", 2,
                port->internal_buf_size,
                port->temp_buf_size);
    #endif /* SW_SRC_DEBUG_LOG */

    /* malloc internal buffer */
    port->internal_buf_ptr = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, port->internal_buf_size * port->channel_num);
    if (port->internal_buf_ptr == NULL)
    {
        OS_ASSERT(FALSE);
        return TRUE;
    }

    /* malloc temp buffer */
    if (port->temp_buf_size != 0)
    {
        port->temp_buf_ptr = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, port->temp_buf_size * port->channel_num);
        if (port->temp_buf_ptr == NULL)
        {
            OS_ASSERT(FALSE);
            return TRUE;
        }
    }
    else
    {
        port->temp_buf_ptr = NULL;
    }

    /* init sw_src channel one by one */
    for (i = 0; (uint32_t)i < port->channel_num; i++)
    {
        current_handle = (Blisrc_Handle **)((uint32_t)(port->work_mem_ptr) + port->work_mem_size * i);
        current_internal_buf = (void *)((uint32_t)(port->internal_buf_ptr) + port->internal_buf_size * i);

        /* open sw_src channel */
        if (Blisrc_Open(current_handle, (void *)(current_internal_buf), &(port->blisrc_param)) != 0)
        {
            DSP_MW_LOG_E("[SW_SRC] SRC open fail!", 0);
            OS_ASSERT(FALSE);
            return TRUE;
        }

        /* reset sw_src channel */
        if (Blisrc_Reset(*current_handle) != 0)
        {
            DSP_MW_LOG_E("[SW_SRC] SRC reset fail!", 0);
            OS_ASSERT(FALSE);
            return TRUE;
        }
    }

    DSP_MW_LOG_I("[SW_SRC] SRC open successfully!, %u, %u, %u, %u, %u, %u, %u, %u, %u, 0x%x, %u, 0x%x, %u, 0x%x", 14,
                port->mode,
                port->channel_num,
                port->in_res,
                port->in_sampling_rate,
                port->in_frame_size_max,
                port->out_res,
                port->out_sampling_rate,
                port->out_frame_size_max,
                port->internal_buf_size,
                port->internal_buf_ptr,
                port->temp_buf_size,
                port->temp_buf_ptr,
                port->work_mem_size,
                port->work_mem_ptr);

    return FALSE;
}

/**
 * @brief This function is used to do the sw src process.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
ATTR_TEXT_IN_IRAM bool stream_function_sw_src_process(void *para)
{
    int32_t i;
    uint32_t j;
    sw_src_port_t *port = NULL;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    uint32_t in_frame_size = stream_function_get_output_size(para);
    uint32_t in_frame_size_temp;
    // uint32_t channel_number = stream_function_get_channel_number(para);
    uint8_t in_sampling_rate = stream_function_get_samplingrate(para);
    uint32_t out_frame_size_temp;
    uint32_t out_frame_size;
    // uint8_t *in_buf = NULL;
    Blisrc_Handle **current_handle;
    void *current_internal_buf;
    char *current_temp_buf;
    void *current_in_data_buf;
    void *current_out_data_buf;
    uint32_t in_samples;
    uint32_t out_samples;
    uint32_t sample_size;

    #if SW_SRC_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SW_SRC][start]: %d, %u, %d", 3, InLengthL, current_timestamp, hal_nvic_query_exception_number());
    #endif /* SW_SRC_DEBUG_LOG */

    /* Find out the sw src port of this stream */
    for (i = 0; i < SW_SRC_PORT_MAX; i++)
    {
        /* Check if this source or sink has already owned a sw src */
        if ((sw_src_port[i].owner == stream_ptr->source) ||
            (sw_src_port[i].owner == stream_ptr->sink))
        {
            port = &sw_src_port[i];
            break;
        }
    }
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_SRC] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    if (!in_frame_size)
    {
        // DSP_MW_LOG_I("[SW_SRC] Input length is 0!", 0);
        return FALSE;
    }

    /* check if the in_sampling_rate is matched */
    if (port->in_sampling_rate != sw_src_fs_converter(in_sampling_rate))
    {
        DSP_MW_LOG_E("[SW_SRC] input sampling rate is not right, %u, %u!", 2, port->in_sampling_rate, sw_src_fs_converter(in_sampling_rate));
        OS_ASSERT(FALSE);
        return TRUE;
    }

    /* check if in_sampling_rate and out_sampling_rate are same */
    if (port->in_sampling_rate == port->out_sampling_rate)
    {
        return FALSE;
    }

    switch (port->mode)
    {
        case SW_SRC_MODE_NORMAL:
            /* calulate input samples and output samples */
            sample_size = (port->in_res == RESOLUTION_16BIT) ? sizeof(int16_t) : sizeof(int32_t);
            in_samples = in_frame_size/sample_size;
            out_samples = in_samples*port->out_sampling_rate/port->in_sampling_rate;
            sample_size = (port->out_res == RESOLUTION_16BIT) ? sizeof(int16_t) : sizeof(int32_t);
            out_frame_size = out_samples*sample_size;

            /* check if frame size is ok */
            if ((in_frame_size > port->in_frame_size_max) || (out_frame_size > port->out_frame_size_max))
            {
                DSP_MW_LOG_E("[SW_SRC] size is not right, %u, %u, %u, %u!", 4, port->in_frame_size_max, in_frame_size, port->out_frame_size_max, out_frame_size);
                OS_ASSERT(FALSE);
                return TRUE;
            }

            /* process data one by one channel */
            for (i = 0; (uint32_t)i < port->channel_num; i++)
            {
                current_handle          = (Blisrc_Handle **)((uint32_t)(port->work_mem_ptr) + port->work_mem_size * i);
                current_internal_buf    = (void *)((uint32_t)(port->internal_buf_ptr) + port->internal_buf_size * i);
                current_temp_buf        = (char *)((uint32_t)(port->temp_buf_ptr) + port->temp_buf_size * i);
                current_in_data_buf     = (void *)((uint32_t)(port->work_mem_ptr) + port->work_mem_size * i + sizeof(Blisrc_Handle *));
                current_out_data_buf    = (void *)(stream_function_get_inout_buffer(para, i+1));

                /* copy input data into the working memory */
                memcpy(current_in_data_buf, current_out_data_buf, in_frame_size);

                /* do sampling rate convert */
                in_frame_size_temp  = in_frame_size;
                out_frame_size_temp = out_frame_size;
                if (Blisrc_Process(*current_handle, current_temp_buf, current_in_data_buf, &in_frame_size_temp, current_out_data_buf, &out_frame_size_temp) < 0)
                {
                    DSP_MW_LOG_E("[SW_SRC] SRC process fail!", 0);
                    OS_ASSERT(FALSE);
                    return TRUE;
                }
                if (in_frame_size_temp != 0)
                {
                    DSP_MW_LOG_E("[SW_SRC] size is not right, %d, %d!", 2, in_frame_size, in_frame_size_temp);
                    OS_ASSERT(FALSE);
                }
                if ((out_frame_size_temp < out_frame_size) && (port->first_time_flag == true))
                {
                    /* add zero-padding in the header at the first time*/
                    for (j = 0; j < out_frame_size_temp; j++)
                    {
                        *(((uint8_t *)current_out_data_buf) + out_frame_size - 1 - j) = *(((uint8_t *)current_out_data_buf) + out_frame_size_temp - 1 - j);
                    }
                    for (j = 0; j < (out_frame_size - out_frame_size_temp); j++)
                    {
                        *((uint8_t *)current_out_data_buf + j) = 0;
                    }
                }
                else if (out_frame_size_temp == out_frame_size)
                {
                    /* do nothing */
                }
                else
                {
                    DSP_MW_LOG_E("[SW_SRC] size is not right, %d, %d!", 2, out_frame_size_temp, out_frame_size);
                    OS_ASSERT(FALSE);
                }
            }

            if (port->first_time_flag == true)
            {
                port->first_time_flag = false;
            }
            break;

        default:
            DSP_MW_LOG_E("[SW_SRC] Mode is not supported!", 0);
            OS_ASSERT(FALSE);
            return TRUE;
    }

    if (out_frame_size == 0)
    {
        DSP_MW_LOG_E("[SW_SRC] output size is zero!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    }
    stream_function_modify_output_size(para, out_frame_size);
    stream_function_modify_output_resolution(para, port->out_res);
    ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate = port->out_sampling_rate/1000;

    #if SW_SRC_DEBUG_LOG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SW_SRC][finish]:%u, %u, %u, %u, %u, %d", 6,
                port->in_sampling_rata,
                port->in_frame_size,
                port->out_sampling_rata,
                out_frame_size,
                current_timestamp,
                hal_nvic_query_exception_number());
    #endif /* SW_SRC_DEBUG_LOG */

    return FALSE;
}

#endif /* AIR_SOFTWARE_SRC_ENABLE */
