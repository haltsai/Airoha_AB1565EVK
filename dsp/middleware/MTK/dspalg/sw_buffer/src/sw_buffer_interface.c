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
#include "sw_buffer_interface.h"
#include "dsp_audio_process.h"
#include "dsp_dump.h"
#include "memory_attribute.h"
#include "preloader_pisplit.h"

/* Private define ------------------------------------------------------------*/
#define SW_BUFFER_DEBUG_LOG                  0
#define SW_BUFFER_DEBUG_DUMP                 0

/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static sw_buffer_port_t sw_buffer_port[SW_BUFFER_PORT_MAX];
static sw_buffer_list_t sw_buffer_list[SW_BUFFER_LIST_MAX];

/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
ATTR_TEXT_IN_IRAM __attribute__ ((noinline))
static void sw_buffer_data_copy(void *des, void *src, uint32_t data_size)
{
    uint32_t i, remain_size;

    if (((((uint32_t)des)%4) == 0) && ((((uint32_t)src)%4) == 0))
    {
        /* word access for better performance */
        remain_size = data_size%4;
        data_size = data_size/4;

        for (i=0; i < data_size; i++)
        {
            *((uint32_t *)des+i) = *((uint32_t *)src+i);
        }

        if (remain_size == 1)
        {
            *((uint8_t *)((uint32_t)des + data_size*4)) = *((uint8_t *)((uint32_t)src + data_size*4));
        }
        else if (remain_size == 2)
        {
            *((uint16_t *)((uint32_t)des + data_size*4)) = *((uint16_t *)((uint32_t)src + data_size*4));
        }
        else if (remain_size == 3)
        {
            *((uint16_t *)((uint32_t)des + data_size*4)) = *((uint16_t *)((uint32_t)src + data_size*4));
            *((uint8_t *)((uint32_t)des + data_size*4 + 2)) = *((uint8_t *)((uint32_t)src + data_size*4 + 2));
        }
        else
        {
            /* do nothing */
        }
    }
    else
    {
        /* half-word access for better performance */
        remain_size = data_size%2;
        data_size = data_size/2;

        for (i=0; i < data_size; i++)
        {
            *((uint16_t *)des+i) = *((uint16_t *)src+i);
        }

        if (remain_size == 1)
        {
            *((uint8_t *)((uint32_t)des + data_size*2)) = *((uint8_t *)((uint32_t)src + data_size*2));
        }
        else
        {
            /* do nothing */
        }
    }
}

/* Public functions ----------------------------------------------------------*/
sw_buffer_port_t *stream_function_sw_buffer_get_port(void *owner)
{
    int32_t i;
    sw_buffer_port_t *port = NULL;
    uint32_t saved_mask;

    /* Find out a port for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = SW_BUFFER_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if there is unused port */
        if (sw_buffer_port[i].owner == NULL)
        {
            port = &sw_buffer_port[i];
            continue;
        }

        /* Check if this owner has already owned a sw buffer */
        if (sw_buffer_port[i].owner == owner)
        {
            port = &sw_buffer_port[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port not enough!", 0);
        assert(0);
        return port;
    }

    port->owner = owner;

    return port;
}

sw_buffer_port_t *stream_function_sw_buffer_get_unused_port(void *owner)
{
    int32_t i;
    sw_buffer_port_t *port = NULL;
    uint32_t saved_mask;

    /* Find out a unued port for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = 0; i < SW_BUFFER_PORT_MAX; i++)
    {
        /* Check if there is unused port */
        if (sw_buffer_port[i].owner == NULL)
        {
            port = &sw_buffer_port[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] All ports are used!", 0);
        return port;
    }

    port->owner = owner;

    return port;
}

sw_buffer_status_t stream_function_sw_buffer_init(sw_buffer_port_t *port, sw_buffer_config_t *config)
{
    uint32_t i;
    sw_buffer_channel_t *channel;

    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    /* get channels' setting space */
    port->total_channels = config->total_channels;
    port->channel = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, sizeof(sw_buffer_channel_t) * config->total_channels);
    if (port->channel == NULL)
    {
        assert(0);
    }

    /* config buffer settings */
    port->mode = config->mode;
    switch (config->mode)
    {
        case SW_BUFFER_MODE_FIXED_OUTPUT_LENGTH:
        case SW_BUFFER_MODE_MULTI_BUFFERS:
            for (i = 0; i < config->total_channels; i++)
            {
                channel = port->channel+i;
                channel->buf_size = config->watermark_max_size;
                channel->data_size = 0;
                channel->buf = preloader_pisplit_malloc_memory( PRELOADER_D_HIGH_PERFORMANCE , config->watermark_max_size);
                // channel->buf = malloc(config->watermark_max_size);
                if (channel->buf == NULL)
                {
                    assert(0);
                }
                channel->read_from = channel->buf;
                channel->write_to  = channel->buf;
            }

            port->watermark_max_size = config->watermark_max_size;
            port->watermark_min_size = config->watermark_min_size;
            port->output_size        = config->output_size;
            break;

        default:
            assert(0);
            break;
    }

    /* update status */
    port->status = SW_BUFFER_PORT_STATUS_INIT;

    return SW_BUFFER_STATUS_OK;
}

sw_buffer_status_t stream_function_sw_buffer_deinit(sw_buffer_port_t *port)
{
    uint32_t i;
    sw_buffer_channel_t *channel;

    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    for (i = 0; i < port->total_channels; i++)
    {
        channel = port->channel+i;

        channel->data_size = 0;
        preloader_pisplit_free_memory(channel->buf);
        // free(channel->buf);
    }
    preloader_pisplit_free_memory(port->channel);
    port->channel = NULL;

    port->owner = NULL;
    port->stream = NULL;
    port->mode = SW_BUFFER_MODE_END;
    port->total_channels = 0;
    port->status = SW_BUFFER_PORT_STATUS_DEINIT;

    return SW_BUFFER_STATUS_OK;
}

ATTR_TEXT_IN_IRAM uint32_t stream_function_sw_buffer_get_channel_used_size(sw_buffer_port_t *port, uint16_t channel_number)
{
    sw_buffer_channel_t *channel;

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    channel = port->channel+ (channel_number - 1);

    return (channel->data_size);
}

ATTR_TEXT_IN_IRAM uint32_t stream_function_sw_buffer_get_channel_free_size(sw_buffer_port_t *port, uint16_t channel_number)
{
    sw_buffer_channel_t *channel;

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    channel = port->channel+ (channel_number - 1);

    return (channel->buf_size - channel->data_size);
}

ATTR_TEXT_IN_IRAM uint8_t *stream_function_sw_buffer_get_channel_read_pointer(sw_buffer_port_t *port, uint16_t channel_number)
{
    sw_buffer_channel_t *channel;

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return NULL;
    }

    channel = port->channel+ (channel_number - 1);

    return (channel->read_from);
}

ATTR_TEXT_IN_IRAM uint8_t *stream_function_sw_buffer_get_channel_write_pointer(sw_buffer_port_t *port, uint16_t channel_number)
{
    sw_buffer_channel_t *channel;

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return NULL;
    }

    channel = port->channel+ (channel_number - 1);

    return (channel->write_to);
}

ATTR_TEXT_IN_IRAM uint8_t *stream_function_sw_buffer_get_channel_start_pointer(sw_buffer_port_t *port, uint16_t channel_number)
{
    sw_buffer_channel_t *channel;

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return NULL;
    }

    channel = port->channel+ (channel_number - 1);

    return (channel->buf);
}

ATTR_TEXT_IN_IRAM uint8_t *stream_function_sw_buffer_get_channel_end_pointer(sw_buffer_port_t *port, uint16_t channel_number)
{
    sw_buffer_channel_t *channel;

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return NULL;
    }

    channel = port->channel+ (channel_number - 1);

    return (channel->buf+channel->buf_size);
}

ATTR_TEXT_IN_IRAM sw_buffer_status_t stream_function_sw_buffer_config_channel_output_size(sw_buffer_port_t *port, uint16_t channel_number, uint16_t new_output_size)
{
    uint32_t saved_mask;

    UNUSED(channel_number);

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    port->output_size = new_output_size;

    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_BUFFER_STATUS_OK;
}

sw_buffer_status_t stream_function_sw_buffer_config_channel_prefill_size(sw_buffer_port_t *port, uint16_t channel_number, uint16_t perfill_size, bool set_zeros)
{
    sw_buffer_channel_t *channel;

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    channel = port->channel+ (channel_number - 1);

    if (set_zeros == true)
    {
        /* set zeros into the channel buffer */
        memset(channel->buf, 0, perfill_size);
    }

    channel->data_size = perfill_size;
    channel->write_to  = channel->buf + perfill_size;

    return SW_BUFFER_STATUS_OK;
}

ATTR_TEXT_IN_IRAM sw_buffer_status_t stream_function_sw_buffer_reset_channel_buffer(sw_buffer_port_t *port, uint16_t channel_number, bool set_zeros)
{
    sw_buffer_channel_t *channel;
    uint32_t saved_mask;

    /* check port and channel_number */
    if ((port == NULL) || (channel_number == 0))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is NULL or Channel is 0!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    channel = port->channel+ (channel_number - 1);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    channel->data_size = 0;
    channel->read_from = channel->buf;
    channel->write_to  = channel->buf;

    if (set_zeros == true)
    {
        /* set zeros into the channel buffer */
        memset(channel->buf, 0, channel->buf_size);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_BUFFER_STATUS_OK;
}

sw_buffer_list_t *stream_function_sw_buffer_get_list(void *owner)
{
    int32_t i;
    sw_buffer_list_t *list = NULL;
    uint32_t saved_mask;

    /* Find out a list for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = SW_BUFFER_LIST_MAX-1; i >= 0; i--)
    {
        /* Check if there is unused list */
        if (sw_buffer_list[i].owner == NULL)
        {
            list = &sw_buffer_list[i];
            continue;
        }

        /* Check if this owner has already owned a sw buffer */
        if (sw_buffer_list[i].owner == owner)
        {
            list = &sw_buffer_list[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (list == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] list not enough!", 0);
        assert(0);
        return list;
    }

    list->owner = owner;

    return list;
}

sw_buffer_status_t stream_function_sw_buffer_list_init(sw_buffer_list_t *list, uint16_t buffer_total_number)
{
    uint32_t i;

    /* check list */
    if ((list == NULL) || (buffer_total_number > SW_BUFFER_PORT_MAX))
    {
        DSP_MW_LOG_E("[SW_BUFFER] List is NULL!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    list->current_index = 0;
    list->buffer_total  = buffer_total_number;
    list->port_list     = NULL;
    list->port_list     = preloader_pisplit_malloc_memory( PRELOADER_D_HIGH_PERFORMANCE , buffer_total_number*sizeof(sw_buffer_port_t *));
    if (list->port_list == NULL)
    {
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }
    for (i = 0; i < buffer_total_number; i++)
    {
        list->port_list[i] = NULL;
    }

    return SW_BUFFER_STATUS_OK;
}

sw_buffer_status_t stream_function_sw_buffer_list_deinit(sw_buffer_list_t *list)
{
    /* check list */
    if (list == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] List is NULL!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    list->owner         = NULL;
    list->current_index = 0;
    list->buffer_total  = 0;
    preloader_pisplit_free_memory(list->port_list);
    list->port_list     = NULL;

    return SW_BUFFER_STATUS_OK;
}

ATTR_TEXT_IN_IRAM sw_buffer_status_t stream_function_sw_buffer_list_insert_buffer(sw_buffer_list_t *list, sw_buffer_port_t *port, uint16_t buffer_index)
{
    uint32_t saved_mask;

    /* check parameters */
    if ((list == NULL) || (port == NULL) || (buffer_index >= SW_BUFFER_PORT_MAX))
    {
        DSP_MW_LOG_E("[SW_BUFFER] Paramters are not right!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    if (list->owner != port->owner)
    {
        DSP_MW_LOG_E("[SW_BUFFER] Owners are not the same!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    if (list->port_list == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] List is NULL!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    if (port->mode != SW_BUFFER_MODE_MULTI_BUFFERS)
    {
        DSP_MW_LOG_E("[SW_BUFFER] Buffer mode are not right!", 0);
        assert(0);
        return SW_BUFFER_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    list->port_list[buffer_index] = port;
    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_BUFFER_STATUS_OK;
}

ATTR_TEXT_IN_IRAM bool stream_function_sw_buffer_initialize(void *para)
{
    int32_t i;
    sw_buffer_port_t *port = NULL;
    sw_buffer_list_t *list = NULL;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    /* Find out the sw buffer port of this stream */
    /* At first, check if this stream use multi-buffer list */
    for (i = 0; i < SW_BUFFER_LIST_MAX; i++)
    {
        /* Check if this source or sink has already owned a sw buffer */
        if ((sw_buffer_list[i].owner == stream_ptr->source) ||
            (sw_buffer_list[i].owner == stream_ptr->sink))
        {
            list = &sw_buffer_list[i];
            break;
        }
    }
    if (list != NULL)
    {
        port = list->port_list[list->current_index];
        list->current_index = (list->current_index + 1) % (list->buffer_total);
    }
    else
    {
        /* The stream does not use multi-buffer list, so need to find out the buffer one by one */
        for (i = 0; i < SW_BUFFER_PORT_MAX; i++)
        {
            /* Check if this source or sink has already owned a sw buffer */
            if ((sw_buffer_port[i].owner == stream_ptr->source) ||
                (sw_buffer_port[i].owner == stream_ptr->sink))
            {
                port = &sw_buffer_port[i];
                break;
            }
        }
    }
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return true;
    }

    /* status check */
    if (port->status == SW_BUFFER_PORT_STATUS_INIT)
    {
        port->status = SW_BUFFER_PORT_STATUS_RUNNING;
    }
    else if (port->status == SW_BUFFER_PORT_STATUS_RUNNING)
    {
        return false;
    }
    else
    {
        DSP_MW_LOG_E("[SW_BUFFER] error status, %d!", 1, port->status);
        OS_ASSERT(FALSE);
        return true;
    }

    port->stream = stream_ptr;

    DSP_MW_LOG_I("[SW_BUFFER] port 0x%x init done, owner = 0x%x, stream = 0x%x", 3, port, port->owner, port->stream);

    return false;
}

ATTR_TEXT_IN_IRAM bool stream_function_sw_buffer_process(void *para)
{
    int32_t i;
    sw_buffer_port_t *port = NULL;
    sw_buffer_list_t *list = NULL;
    sw_buffer_channel_t *channel;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    uint32_t channel_number = stream_function_get_channel_number(para);
    uint32_t in_frame_size = stream_function_get_output_size(para);
    uint8_t *buf = NULL;
    uint32_t out_frame_size = 0;
    uint32_t remian_size = 0;

    /* Find out the sw buffer port of this stream */
    /* At first, check if this stream use multi-buffer list */
    for (i = 0; i < SW_BUFFER_LIST_MAX; i++)
    {
        /* Check if this source or sink has already owned a sw buffer */
        if ((sw_buffer_list[i].owner == stream_ptr->source) ||
            (sw_buffer_list[i].owner == stream_ptr->sink))
        {
            list = &sw_buffer_list[i];
            break;
        }
    }
    if (list != NULL)
    {
        port = list->port_list[list->current_index];
        list->current_index = (list->current_index + 1) % (list->buffer_total);
    }
    else
    {
        /* The stream does not use multi-buffer list, so need to find out the buffer one by one */
        for (i = 0; i < SW_BUFFER_PORT_MAX; i++)
        {
            /* Check if this source or sink has already owned a sw buffer */
            if ((sw_buffer_port[i].owner == stream_ptr->source) ||
                (sw_buffer_port[i].owner == stream_ptr->sink))
            {
                port = &sw_buffer_port[i];
                break;
            }
        }
    }
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_BUFFER] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return true;
    }

    if (channel_number != port->total_channels)
    {
        DSP_MW_LOG_E("[SW_GAIN] channel number is not right!", 0);
        OS_ASSERT(FALSE);
        return true;
    }

    #if SW_BUFFER_DEBUG_DUMP
    LOG_AUDIO_DUMP( (uint8_t *)stream_function_get_inout_buffer(para, 1),
                    in_frame_size,
                    VOICE_TX_MIC_2);
    #endif /* SW_BUFFER_DEBUG_DUMP */

    switch (port->mode)
    {
        case SW_BUFFER_MODE_FIXED_OUTPUT_LENGTH:
        case SW_BUFFER_MODE_MULTI_BUFFERS:
            /* copy data into buffer */
            for (i = 0; (uint32_t)i < channel_number ; i++)
            {
                channel = port->channel+i;

                buf = (uint8_t *)stream_function_get_inout_buffer(para, i+1);

                /* write data into internal buffer */
                if (in_frame_size > (channel->buf_size - channel->data_size))
                {
                    DSP_MW_LOG_E("[SW_BUFFER] port 0x%x not enough space, %d, %d", 3, port, in_frame_size, (channel->buf_size - channel->data_size));
                    assert(0);
                }
                if ((channel->write_to+in_frame_size) > (channel->buf+channel->buf_size))
                {
                    /* wrapper case */
                    remian_size = (uint32_t)(channel->buf+channel->buf_size) - (uint32_t)(channel->write_to);
                    sw_buffer_data_copy((void *)(channel->write_to), (void *)buf, remian_size);
                    channel->write_to = channel->buf;
                    sw_buffer_data_copy((void *)(channel->write_to), (void *)(buf+remian_size), in_frame_size - remian_size);
                    channel->write_to += in_frame_size - remian_size;
                }
                else
                {
                    /* normal case */
                    sw_buffer_data_copy((void *)(channel->write_to), (void *)buf, in_frame_size);
                    channel->write_to += in_frame_size;
                    if (channel->write_to == (channel->buf+channel->buf_size))
                    {
                        channel->write_to = channel->buf;
                    }
                }
                /* update data size */
                channel->data_size += in_frame_size;
            }

            /* check if output data is enough */
            out_frame_size = port->output_size;
            for (i = 0; (uint32_t)i < channel_number ; i++)
            {
                channel = port->channel+i;

                if ((channel->data_size < port->watermark_min_size) || (channel->data_size < out_frame_size))
                {
                    // DSP_MW_LOG_E("[SW_BUFFER] port 0x%x ch%d not enough data, %d, %d, %d", 5, port, (i+1), out_frame_size, channel->data_size, port->watermark_min_size);
                    out_frame_size = 0;
                    break;
                }
            }

            /* output data into stream */
            if (out_frame_size != 0)
            {
                for (i = 0; (uint32_t)i < channel_number ; i++)
                {
                    channel = port->channel+i;

                    buf = (uint8_t *)stream_function_get_inout_buffer(para, i+1);

                    /* read data from internal buffer */
                    if ((channel->read_from+out_frame_size) > (channel->buf+channel->buf_size))
                    {
                        /* wrapper case */
                        remian_size = (uint32_t)(channel->buf+channel->buf_size) - (uint32_t)(channel->read_from);
                        sw_buffer_data_copy((void *)buf, (void *)(channel->read_from), remian_size);
                        channel->read_from = channel->buf;
                        sw_buffer_data_copy((void *)(buf+remian_size), (void *)(channel->read_from), out_frame_size - remian_size);
                        channel->read_from += out_frame_size - remian_size;
                    }
                    else
                    {
                        /* normal case */
                        sw_buffer_data_copy((void *)buf, (void *)(channel->read_from), out_frame_size);
                        channel->read_from += out_frame_size;
                        if (channel->read_from == (channel->buf+channel->buf_size))
                        {
                            channel->read_from = channel->buf;
                        }
                    }
                    /* update data size */
                    channel->data_size -= out_frame_size;
                }
            }
            break;

        default:
            OS_ASSERT(FALSE);
            break;
    }

    stream_function_modify_output_size(para, out_frame_size);

    #if SW_BUFFER_DEBUG_DUMP
    LOG_AUDIO_DUMP( (uint8_t *)stream_function_get_inout_buffer(para, 1),
                    out_frame_size,
                    VOICE_TX_MIC_3);
    #endif /* SW_BUFFER_DEBUG_DUMP */

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&(port->finish_gpt_count));

    return false;
}
