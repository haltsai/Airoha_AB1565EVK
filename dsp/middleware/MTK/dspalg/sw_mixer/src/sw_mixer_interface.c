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
#include "sw_mixer_interface.h"
#include "dsp_audio_process.h"
#include "dsp_dump.h"
#include "memory_attribute.h"
#include "preloader_pisplit.h"
#include <xtensa/tie/xt_hifi2.h>


/* Private define ------------------------------------------------------------*/
#define SW_MIXER_DEBUG_LOG                  0
#define SW_MIXER_DEBUG_DUMP                 0


/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static sw_mixer_port_handle_t sw_mixer_port_handle[SW_MIXER_PORT_MAX];


/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
ATTR_TEXT_IN_IRAM __attribute__ ((noinline))
static void sw_mixer_data_copy(uint32_t *des, uint32_t *src, uint32_t data_size)
{
    uint32_t i, remain_size;

    remain_size = data_size%4;
    data_size = data_size/4;

    for (i=0; i < data_size; i++)
    {
        *(des+i) = *(src+i);
    }

    if (remain_size == 1)
    {
        *((uint8_t *)des + data_size*4) = *((uint8_t *)src + data_size*4);
    }
    else if (remain_size == 2)
    {
        *((uint16_t *)des + data_size*4) = *((uint16_t *)src + data_size*4);
    }
    else if (remain_size == 3)
    {
        *((uint16_t *)des + data_size*4) = *((uint16_t *)src + data_size*4);
        *((uint8_t *)des + data_size*4 + 2) = *((uint8_t *)src + data_size*4 + 2);
    }
    else
    {

    }
}

ATTR_TEXT_IN_IRAM __attribute__ ((noinline))
sw_mixer_status_t sw_mixer_copy_data(sw_mixer_input_channel_t *input_ch, stream_resolution_t in_res, uint32_t in_frame_size, uint8_t *in_buf)
{
    uint32_t remian_size;

    if ((input_ch->resolution != in_res) ||
        (input_ch->buf_size < in_frame_size))
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    switch (input_ch->input_mode)
    {
        case SW_MIXER_CHANNEL_MODE_NORMAL:
            /* copy data */
            if ((input_ch->write_to+in_frame_size) > (input_ch->in_buf+input_ch->buf_size))
            {
                /* wrapper case */
                remian_size = (uint32_t)(input_ch->in_buf+input_ch->buf_size) - (uint32_t)(input_ch->write_to);
                sw_mixer_data_copy((uint32_t *)(input_ch->write_to), (uint32_t *)in_buf, remian_size);
                input_ch->write_to = input_ch->in_buf;
                sw_mixer_data_copy((uint32_t *)(input_ch->write_to), (uint32_t *)(in_buf+remian_size), in_frame_size - remian_size);
                input_ch->write_to += in_frame_size - remian_size;
            }
            else
            {
                /* normal case */
                sw_mixer_data_copy((uint32_t *)(input_ch->write_to), (uint32_t *)in_buf, in_frame_size);
                input_ch->write_to += in_frame_size;
                if (input_ch->write_to == (input_ch->in_buf+input_ch->buf_size))
                {
                    input_ch->write_to = input_ch->in_buf;
                }
            }

            if (in_frame_size > (input_ch->buf_size - input_ch->data_size))
            {
                /* input frame size is large than empty space, the buffer will be full after copy */
                /* set read pointer == write pointer, read pointer is pointed to the oldest data */
                input_ch->read_from = input_ch->write_to;

                /* update data size, buffer is full */
                input_ch->data_size = input_ch->buf_size;
            }
            else
            {
                /* input frame size is not large than empty space */
                /* update data size */
                input_ch->data_size += in_frame_size;
            }

            break;

        case SW_MIXER_CHANNEL_MODE_OVERWRITE:
            /* copy data */
            /* read pointer is always pointed to the buffer start address */
            input_ch->read_from = input_ch->in_buf;
            input_ch->write_to  = input_ch->in_buf;
            sw_mixer_data_copy((uint32_t *)(input_ch->write_to), (uint32_t *)in_buf, in_frame_size);
            input_ch->write_to += in_frame_size;

            /* update data size */
            input_ch->data_size = in_frame_size;

            break;

        case SW_MIXER_CHANNEL_MODE_FORCED_ENOUGH:
            if (in_frame_size > (input_ch->buf_size - input_ch->data_size))
            {
                DSP_MW_LOG_E("[SW_MIXER] not enough space, %d, %d", 2, in_frame_size, (input_ch->buf_size - input_ch->data_size));
                assert(0);
            }

            /* copy data */
            if ((input_ch->write_to+in_frame_size) > (input_ch->in_buf+input_ch->buf_size))
            {
                /* wrapper case */
                remian_size = (uint32_t)(input_ch->in_buf+input_ch->buf_size) - (uint32_t)(input_ch->write_to);
                sw_mixer_data_copy((uint32_t *)(input_ch->write_to), (uint32_t *)in_buf, remian_size);
                input_ch->write_to = input_ch->in_buf;
                sw_mixer_data_copy((uint32_t *)(input_ch->write_to), (uint32_t *)(in_buf+remian_size), in_frame_size - remian_size);
                input_ch->write_to += in_frame_size - remian_size;
            }
            else
            {
                /* normal case */
                sw_mixer_data_copy((uint32_t *)(input_ch->write_to), (uint32_t *)in_buf, in_frame_size);
                input_ch->write_to += in_frame_size;
                if (input_ch->write_to == (input_ch->in_buf+input_ch->buf_size))
                {
                    input_ch->write_to = input_ch->in_buf;
                }
            }

            /* update data size */
            input_ch->data_size += in_frame_size;

            break;

        default:
            assert(0);
            break;
    }

    return SW_MIXER_STATUS_OK;
}

ATTR_TEXT_IN_IRAM void sw_mixer_data_add(uint8_t *in_buf1, uint8_t *in_buf2, uint8_t *ou_buf, stream_resolution_t res, uint32_t samples)
{
    uint32_t i;
    uint32_t samples_odd;
    ae_p24x2s p0, p1;

    switch (res)
    {
        case RESOLUTION_16BIT:
            /* check in samples if it is a odd number */
            if ((samples%2) != 0)
            {
                samples_odd = samples - 1;
            }
            else
            {
                samples_odd = samples;
            }

            for (i = 0; i < samples_odd; i += 2)
            {
                /* load two 16 bit data into AE_PR register at every time */
                p0 = *((ae_p16x2s *)((int16_t *)in_buf1 + i));
                p1 = *((ae_p16x2s *)((int16_t *)in_buf2 + i));

                /* Signed saturating 24-bit addition */
                p0 = p0 + p1;

                /* Truncate to two 16-bit data and store them into the memory at evety time */
                *((ae_p16x2s *)((int16_t *)ou_buf + i)) = p0;
            }
            if (samples_odd != samples)
            {
                /* process the last one sample if in samples is not odd */
                /* load two 16 bit data into AE_PR register at every time */
                p0 = *((ae_p16x2s *)((int16_t *)in_buf1 + samples_odd));
                p1 = *((ae_p16x2s *)((int16_t *)in_buf2 + samples_odd));

                /* Signed saturating 24-bit addition */
                p0 = p0 + p1;

                /* Truncate to one 16-bit data (p0.l) and store it into the memory at evety time */
                *((ae_p16s *)((int16_t *)ou_buf + samples_odd)) = p0;
            }

            break;

        case RESOLUTION_32BIT:
            DSP_MW_LOG_E("[SW_MIXER] 32-bit not support now", 0);
            assert(0);

            break;

        default:
            assert(0);
            break;
    }
}

ATTR_TEXT_IN_IRAM __attribute__ ((noinline))
void sw_mixer_data_ring_buffer_mix(sw_mixer_input_channel_t *input_ch, uint8_t *output_buf, uint32_t mix_data_size, uint32_t mix_data_samples)
{
    uint32_t mix_data_samples_temp, mix_data_size_temp;

    if ((input_ch->read_from+mix_data_size) > (input_ch->in_buf+input_ch->buf_size))
    {
        /* wrapper case */
        mix_data_size_temp = (uint32_t)(input_ch->in_buf+input_ch->buf_size) - (uint32_t)(input_ch->read_from);
        mix_data_samples_temp = (input_ch->resolution == RESOLUTION_16BIT)?(mix_data_size_temp/sizeof(S16)):(mix_data_size_temp/sizeof(S32));
        sw_mixer_data_add((uint8_t *)output_buf,
                          (uint8_t *)input_ch->read_from,
                          (uint8_t *)output_buf,
                          input_ch->resolution,
                          mix_data_samples_temp);
        sw_mixer_data_add((uint8_t *)((uint32_t)output_buf+mix_data_size_temp),
                          (uint8_t *)input_ch->in_buf,
                          (uint8_t *)((uint32_t)output_buf+mix_data_size_temp),
                          input_ch->resolution,
                          mix_data_samples-mix_data_samples_temp);
    }
    else
    {
        /* normal case */
        sw_mixer_data_add((uint8_t *)output_buf,
                          (uint8_t *)input_ch->read_from,
                          (uint8_t *)output_buf,
                          input_ch->resolution,
                          mix_data_samples);
    }
}

ATTR_TEXT_IN_IRAM __attribute__ ((noinline))
uint32_t sw_mixer_mix_data(sw_mixer_output_channel_t *output_ch, stream_resolution_t in_res, uint32_t in_frame_size, uint8_t *out_buf, uint32_t out_buf_size)
{
    uint32_t i;
    uint32_t output_size = 0;
    uint32_t output_samples;
    sw_mixer_input_channel_t *main_input_ch;
    sw_mixer_input_channel_t *input_ch;
    sw_mixer_list_item_t *main_item = NULL;
    sw_mixer_list_item_t *in_channel_list_item;

    UNUSED(in_res);
    UNUSED(in_frame_size);

    if (output_ch->in_channel_list.item_number == 0)
    {
        return output_size;
    }

    /* get main channel */
    main_item = output_ch->in_channel_list.first_item;
    if (main_item->attribute != SW_MIXER_CHANNEL_ATTRIBUTE_MAIN)
    {
        /* there is no main channel */
        assert(0);
        return output_size;
    }

    /* at first, copy main channel data into out buffer */
    main_input_ch = (sw_mixer_input_channel_t *)main_item->channel;
    output_size = main_input_ch->data_size;
    if (output_size == 0)
    {
        return output_size;
    }
    else if (output_size > out_buf_size)
    {
        /* output size is too large */
        assert(0);
        return 0;
    }
    if ((main_input_ch->read_from+output_size) > (main_input_ch->in_buf+main_input_ch->buf_size))
    {
        /* wrapper case */
        i = (uint32_t)(main_input_ch->in_buf+main_input_ch->buf_size) - (uint32_t)(main_input_ch->read_from);
        sw_mixer_data_copy((uint32_t *)out_buf, (uint32_t *)(main_input_ch->read_from), i);
        sw_mixer_data_copy((uint32_t *)(out_buf+i), (uint32_t *)(main_input_ch->in_buf), output_size-i);
    }
    else
    {
        /* normal case */
        sw_mixer_data_copy((uint32_t *)out_buf, (uint32_t *)(main_input_ch->read_from), output_size);
    }


    /* then, mix data one by one other input channels */
    in_channel_list_item = main_item->next_item;
    for (i=0; i < (output_ch->in_channel_list.item_number-1); i++)
    {
        /* get input channel */
        input_ch = (sw_mixer_input_channel_t *)in_channel_list_item->channel;

        /* check resoultion */
        if(input_ch->resolution != main_input_ch->resolution)
        {
            /* the resoultion difference is not support at now */
            assert(0);
        }

        /* mix data based on mode */
        if (in_channel_list_item->attribute == SW_MIXER_CHANNEL_ATTRIBUTE_FORCED_WAIT)
        {
            if (input_ch->data_size < output_size)
            {
                /* there is not enough data in FORCED_WAIT channel, so bypass all channels of this mixer */
                output_size = 0;
                break;
            }

            /* get samples */
            output_samples = (input_ch->resolution == RESOLUTION_16BIT)?(output_size/sizeof(S16)):(output_size/sizeof(S32));

            /* there is enough data in FORCED_WAIT channel, so do the mixing of this channel */
            sw_mixer_data_ring_buffer_mix(input_ch, out_buf, output_size, output_samples);
        }
        else if (in_channel_list_item->attribute == SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL)
        {
            if (input_ch->data_size < output_size)
            {
                /* get actual samples */
                output_samples = (input_ch->resolution == RESOLUTION_16BIT)?(input_ch->data_size/sizeof(S16)):(input_ch->data_size/sizeof(S32));

                /* there is not enough data in NORMAL channel, so just do the mixing (actual size of this channel) of this channel */
                sw_mixer_data_ring_buffer_mix(input_ch, out_buf, input_ch->data_size, output_samples);
            }
            else
            {
                /* get samples */
                output_samples = (input_ch->resolution == RESOLUTION_16BIT)?(output_size/sizeof(S16)):(output_size/sizeof(S32));

                /* there is enough data in NORMAL channel, so do the mixing of this channel */
                sw_mixer_data_ring_buffer_mix(input_ch, out_buf, output_size, output_samples);
            }
        }
        else if (in_channel_list_item->attribute == SW_MIXER_CHANNEL_ATTRIBUTE_NORMAL_WAIT)
        {
            if (input_ch->data_size < output_size)
            {
                /* there is not enough data in NORMAL_WAIT channel, so only bypass this channel */
            }
            else
            {
                /* get samples */
                output_samples = (main_input_ch->resolution == RESOLUTION_16BIT)?(output_size/sizeof(S16)):(output_size/sizeof(S32));

                /* there is enough data in NORMAL_WAIT channel, so do the mixing of this channel */
                sw_mixer_data_ring_buffer_mix(input_ch, out_buf, output_size, output_samples);
            }
        }
        else
        {
            assert(0);
        }

        in_channel_list_item = in_channel_list_item->next_item;
    }

    return output_size;
}


/* Public functions ----------------------------------------------------------*/
sw_mixer_status_t stream_function_sw_mixer_init(sw_mixer_port_t port)
{
    uint32_t saved_mask;

    /* check parameters */
    if (port >= SW_MIXER_PORT_MAX)
    {
        return SW_MIXER_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    assert(sw_mixer_port_handle[port].count < 0xffff);
    sw_mixer_port_handle[port].count += 1;
    if (sw_mixer_port_handle[port].count != 1)
    {
        /* someone has inited this port, so do nothing and just return ok */
        hal_nvic_restore_interrupt_mask(saved_mask);
        return SW_MIXER_STATUS_OK;
    }

    /* do initialize */
    sw_mixer_port_handle[port].status        = SW_MIXER_PORT_STATUS_INIT;
    sw_mixer_port_handle[port].total_members = 0;
    sw_mixer_port_handle[port].first_member  = NULL;
    sw_mixer_port_handle[port].last_member   = NULL;

    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_MIXER_STATUS_OK;
}

sw_mixer_status_t stream_function_sw_mixer_deinit(sw_mixer_port_t port)
{
    uint32_t saved_mask;

    /* check parameters */
    if (port >= SW_MIXER_PORT_MAX)
    {
        return SW_MIXER_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    assert(sw_mixer_port_handle[port].count != 0);
    sw_mixer_port_handle[port].count -= 1;
    if (sw_mixer_port_handle[port].count != 0)
    {
        /* someone is still using this port, so do nothing and just return ok */
        hal_nvic_restore_interrupt_mask(saved_mask);
        return SW_MIXER_STATUS_OK;
    }

    /* do deinitialize */
    sw_mixer_port_handle[port].status        = SW_MIXER_PORT_STATUS_DEINIT;
    sw_mixer_port_handle[port].total_members = 0;
    sw_mixer_port_handle[port].first_member  = NULL;
    sw_mixer_port_handle[port].last_member   = NULL;

    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_MIXER_STATUS_OK;
}

sw_mixer_member_t *stream_function_sw_mixer_member_create
(void *owner, sw_mixer_member_mode_t mode, sw_mixer_callback_config_t *callback_config, sw_mixer_input_channel_config_t *in_ch_config, sw_mixer_output_channel_config_t *out_ch_config)
{
    sw_mixer_member_t *member;
    uint32_t i;
    sw_mixer_input_channel_t  *input_ch;
    sw_mixer_output_channel_t *output_ch;

    /* check parameters */
    if ((mode >= SW_MIXER_MEMBER_MODE_MAX) ||
        (owner == NULL) ||
        (callback_config == NULL) ||
        (in_ch_config == NULL) ||
        (out_ch_config == NULL))
    {
        return NULL;
    }

    member = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, sizeof(sw_mixer_member_t));
    if (member == NULL)
    {
        assert(0);
        return NULL;
    }

    /* initialize member state */
    member->status = SW_MIXER_MEMBER_STATUS_CREATED;
    member->owner = owner;
    member->stream = NULL;
    /* it will be set after the member is registered */
    member->port = SW_MIXER_PORT_MAX;
    /* it will be set after the member is registered */
    member->number = -1;
    member->mode  = mode;
    member->preprocess_callback  = callback_config->preprocess_callback;
    member->postprocess_callback = callback_config->postprocess_callback;
    member->default_connected  = 0;
    member->total_in_channels  = in_ch_config->total_channel_number;
    member->total_out_channels = out_ch_config->total_channel_number;
    member->input_ch  = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, member->total_in_channels * sizeof(sw_mixer_input_channel_t));
    member->output_ch = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, member->total_out_channels * sizeof(sw_mixer_output_channel_t));
    if ((member->input_ch  == NULL) ||
        (member->output_ch  == NULL) ||
        (member->total_in_channels != member->total_out_channels))
    {
        assert(0);
        return NULL;
    }
    member->next_member = NULL;
    member->perv_member = NULL;

    /* initialize input channels one by one */
    for (i = 0; i < member->total_in_channels; i++)
    {
        input_ch = member->input_ch + i;
        input_ch->member     = member;
        input_ch->number     = i+1;
        input_ch->resolution = in_ch_config->resolution;
        if (in_ch_config->input_mode >= SW_MIXER_CHANNEL_MODE_MAX)
        {
            assert(0);
            return NULL;
        }
        input_ch->input_mode = in_ch_config->input_mode;
        input_ch->buf_size   = in_ch_config->buffer_size;
        input_ch->data_size  = 0;
        input_ch->in_buf     = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, input_ch->buf_size);
        if (input_ch->in_buf == NULL)
        {
            assert(0);
            return NULL;
        }
        input_ch->read_from  = input_ch->in_buf;
        input_ch->write_to   = input_ch->in_buf;
        input_ch->out_channel_list.item_number = 0;
        input_ch->out_channel_list.first_item  = NULL;
        input_ch->out_channel_list.last_item   = NULL;
    }

    /* initialize output channels one by one */
    for (i = 0; i < member->total_out_channels; i++)
    {
        output_ch = member->output_ch + i;
        output_ch->member     = member;
        output_ch->number     = i+1;
        output_ch->resolution = out_ch_config->resolution;
        /* out_buf will be pointer to stream buffer after the stream starts */
        output_ch->out_buf    = NULL;
        output_ch->data_size  = 0;
        output_ch->in_channel_list.item_number = 0;
        output_ch->in_channel_list.first_item  = NULL;
        output_ch->in_channel_list.last_item   = NULL;
    }

    return member;
}

sw_mixer_status_t stream_function_sw_mixer_member_register(sw_mixer_port_t port, sw_mixer_member_t *member, bool default_connected)
{
    uint32_t saved_mask;
    uint32_t i;
    sw_mixer_member_t *c_member;

    /* check parameters */
    if ((port >= SW_MIXER_PORT_MAX) ||
        (member == NULL))
    {
        return SW_MIXER_STATUS_ERROR;
    }

    if (sw_mixer_port_handle[port].status != SW_MIXER_PORT_STATUS_INIT)
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    if ((member->status != SW_MIXER_MEMBER_STATUS_CREATED) &&
        (member->status != SW_MIXER_MEMBER_STATUS_UNREGISTERED))
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* register member into the mix port */
    if (sw_mixer_port_handle[port].total_members == 0)
    {
        /* it is the first member in this mix port */
        sw_mixer_port_handle[port].first_member = member;
        sw_mixer_port_handle[port].last_member  = member;
    }
    else
    {
        /* check if this member has been registered */
        c_member = sw_mixer_port_handle[port].first_member;
        for (i = 0; i < sw_mixer_port_handle[port].total_members; i++)
        {
            if (c_member == member)
            {
                /* yes, this member is registered */
                hal_nvic_restore_interrupt_mask(saved_mask);
                return SW_MIXER_STATUS_OK;
            }

            c_member = c_member->next_member;
        }

        /* add this member in the tail of the mix port */
        member->next_member = NULL;
        member->perv_member = sw_mixer_port_handle[port].last_member;
        sw_mixer_port_handle[port].last_member->next_member = member;
        sw_mixer_port_handle[port].last_member = member;
    }

    /* update the total member number */
    sw_mixer_port_handle[port].total_members += 1;
    if (sw_mixer_port_handle[port].total_members == 0)
    {
        assert(0);
    }

    /* update member status */
    member->number = sw_mixer_port_handle[port].total_members;
    member->port = port;

    /* change member status to SW_MIXER_MEMBER_STATUS_REGISTERED that indicate the member has be registered into mix port */
    member->status = SW_MIXER_MEMBER_STATUS_REGISTERED;

    /* update default connection status */
    member->default_connected = default_connected;

    hal_nvic_restore_interrupt_mask(saved_mask);

    /* check if needs to do default connections */
    if (default_connected)
    {
        /* in here, it means all input channel should be connected to the corresponding output channel as default */
        for (i = 0; i < member->total_in_channels; i++)
        {
            stream_function_sw_mixer_channel_connect(member, i+1, SW_MIXER_CHANNEL_ATTRIBUTE_MAIN, member, i+1);
        }
    }

    return SW_MIXER_STATUS_OK;
}

sw_mixer_status_t stream_function_sw_mixer_member_unregister(sw_mixer_port_t port, sw_mixer_member_t *member)
{
    uint32_t saved_mask;
    uint32_t i, j;
    sw_mixer_member_t *c_member;
    sw_mixer_input_channel_t  *input_ch;
    sw_mixer_output_channel_t *output_ch;
    sw_mixer_list_item_t *item;
    sw_mixer_list_item_t *next_item;
    uint32_t item_number;

    /* check parameters */
    if ((port >= SW_MIXER_PORT_MAX) ||
        (member == NULL))
    {
        return SW_MIXER_STATUS_ERROR;
    }

    if (sw_mixer_port_handle[port].status != SW_MIXER_PORT_STATUS_INIT)
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    if (member->status != SW_MIXER_MEMBER_STATUS_REGISTERED)
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    /* disable all input channels one by one */
    for (i = 0; i < member->total_in_channels; i++)
    {
        input_ch = member->input_ch + i;
        item_number = input_ch->out_channel_list.item_number;
        item = input_ch->out_channel_list.first_item;
        for (j = 0; j < item_number; j++)
        {
            /* back up next_item because stream_function_sw_mixer_channel_disconnect() will delete item */
            next_item = item->next_item;

            /* get current channel on out_channel_list */
            output_ch = (sw_mixer_output_channel_t *)(item->channel);
            stream_function_sw_mixer_channel_disconnect(input_ch->member,
                                                        input_ch->number,
                                                        output_ch->member,
                                                        output_ch->number);

            /* change to the next item on out_channel_list */
            item = next_item;
        }
    }

    /* disable all output channels one by one */
    for (i = 0; i < member->total_out_channels; i++)
    {
        output_ch = member->output_ch + i;
        item_number = output_ch->in_channel_list.item_number;
        item = output_ch->in_channel_list.first_item;
        for (j = 0; j < item_number; j++)
        {
            /* back up next_item because stream_function_sw_mixer_channel_disconnect() will delete item */
            next_item = item->next_item;

            /* get current channel on in_channel_list */
            input_ch = (sw_mixer_input_channel_t *)(item->channel);
            stream_function_sw_mixer_channel_disconnect(input_ch->member,
                                                        input_ch->number,
                                                        output_ch->member,
                                                        output_ch->number);

            /* change to the next item on in_channel_list */
            item = next_item;
        }
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* unregister member from the mix port */
    c_member = sw_mixer_port_handle[port].first_member;
    for (i=0; i < sw_mixer_port_handle[port].total_members; i++)
    {
        /* check if it is the unregister member */
        if (c_member == member)
        {
            /* find out the unregister member */
            if ((c_member->perv_member != NULL) && (c_member->next_member != NULL))
            {
                /* this member is on the middle of the member list */
                c_member->perv_member->next_member = c_member->next_member;
                c_member->next_member->perv_member = c_member->perv_member;
            }
            else if ((c_member->perv_member == NULL) && (c_member->next_member != NULL))
            {
                /* this member is on the head of the member list, but it is not only one member on the list */
                c_member->next_member->perv_member = NULL;
                sw_mixer_port_handle[port].first_member = c_member->next_member;
            }
            else if ((c_member->perv_member != NULL) && (c_member->next_member == NULL))
            {
                /* this member is on the tail of the member list, but it is not only one member on the list */
                c_member->perv_member->next_member = NULL;
                sw_mixer_port_handle[port].last_member = c_member->perv_member;
            }
            else
            {
                /* this member is only one member on the list */
                sw_mixer_port_handle[port].first_member = NULL;
                sw_mixer_port_handle[port].last_member  = NULL;
            }

            break;
        }

        /* switch to the next member */
        c_member = c_member->next_member;
        if (c_member == NULL)
        {
            /* in here, it means the member is not in this port */
            assert(0);
        }
    }

    /* update member status */
    member->next_member = NULL;
    member->perv_member = NULL;
    member->number = -1;
    member->port = SW_MIXER_PORT_MAX;

    /* change member status to SW_MIXER_MEMBER_STATUS_UNREGISTERED that indicate the member has be unregistered into mix port */
    member->status = SW_MIXER_MEMBER_STATUS_UNREGISTERED;

    /* delete a member number */
    if (sw_mixer_port_handle[port].total_members == 0)
    {
        assert(0);
    }
    sw_mixer_port_handle[port].total_members -= 1;

    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_MIXER_STATUS_OK;
}

sw_mixer_status_t stream_function_sw_mixer_member_delete(sw_mixer_member_t *member)
{
    uint32_t saved_mask;
    uint32_t i;
    sw_mixer_input_channel_t  *input_ch;

    /* check parameters */
    if (member == NULL)
    {
        return SW_MIXER_STATUS_ERROR;
    }

    if ((member->status != SW_MIXER_MEMBER_STATUS_CREATED) &&
        (member->status != SW_MIXER_MEMBER_STATUS_UNREGISTERED))
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* update status */
    member->status = SW_MIXER_MEMBER_STATUS_DELETED;

    hal_nvic_restore_interrupt_mask(saved_mask);

    /* free the buffer of the input channel */
    for (i = 0; i < member->total_in_channels; i++)
    {
        input_ch = member->input_ch + i;
        preloader_pisplit_free_memory(input_ch->in_buf);
    }

    /* free input channels */
    preloader_pisplit_free_memory(member->input_ch);

    /* free output channels */
    preloader_pisplit_free_memory(member->output_ch);

    /* free member */
    preloader_pisplit_free_memory(member);

    return SW_MIXER_STATUS_OK;
}

ATTR_TEXT_IN_IRAM sw_mixer_status_t stream_function_sw_mixer_member_input_buffers_clean(sw_mixer_member_t *member, bool set_zeros)
{
    uint32_t saved_mask;
    uint32_t i;
    sw_mixer_input_channel_t  *input_ch;

    /* check parameters */
    if (member == NULL)
    {
        return SW_MIXER_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* clean in_buf one by one input channels */
    for(i=0; i < member->total_in_channels; i++)
    {
        /* get input channel */
        input_ch = member->input_ch + i;

        /* clean in_buf */
        input_ch->data_size = 0;
        input_ch->read_from = input_ch->in_buf;
        input_ch->write_to  = input_ch->in_buf;
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    /* check if needs to set zeros into the in buffer */
    if (set_zeros == true)
    {
        for(i=0; i < member->total_in_channels; i++)
        {
            /* get input channel */
            input_ch = member->input_ch + i;

            /* set zeros into the in buffer */
            memset(input_ch->in_buf, 0, input_ch->buf_size);
        }
    }

    return SW_MIXER_STATUS_OK;
}

sw_mixer_status_t stream_function_sw_mixer_channel_disconnect_all(sw_mixer_member_t *member)
{
    uint32_t saved_mask;
    uint32_t i, j;
    sw_mixer_input_channel_t  *input_ch;
    sw_mixer_output_channel_t *output_ch;
    sw_mixer_list_item_t *item;
    sw_mixer_list_item_t *next_item;
    uint32_t item_number;

    /* check parameters */
    if (member == NULL)
    {
        return SW_MIXER_STATUS_ERROR;
    }

    if (member->status != SW_MIXER_MEMBER_STATUS_REGISTERED)
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* disable this member's all input channels which maybe are connected to other members one by one */
    for (i = 0; i < member->total_in_channels; i++)
    {
        input_ch = member->input_ch + i;
        item_number = input_ch->out_channel_list.item_number;
        item = input_ch->out_channel_list.first_item;
        for (j = 0; j < item_number; j++)
        {
            /* back up next_item because stream_function_sw_mixer_channel_disconnect() will delete item */
            next_item = item->next_item;

            /* get current channel on out_channel_list */
            output_ch = (sw_mixer_output_channel_t *)(item->channel);
            stream_function_sw_mixer_channel_disconnect(input_ch->member,
                                                        input_ch->number,
                                                        output_ch->member,
                                                        output_ch->number);

            /* change to the next item on out_channel_list */
            item = next_item;
        }
    }

    /* disable this member's all output channels which maybe are connected to other members one by one */
    for (i = 0; i < member->total_out_channels; i++)
    {
        output_ch = member->output_ch + i;
        item_number = output_ch->in_channel_list.item_number;
        item = output_ch->in_channel_list.first_item;
        for (j = 0; j < item_number; j++)
        {
            /* back up next_item because stream_function_sw_mixer_channel_disconnect() will delete item */
            next_item = item->next_item;

            /* get current channel on in_channel_list */
            input_ch = (sw_mixer_input_channel_t *)(item->channel);
            stream_function_sw_mixer_channel_disconnect(input_ch->member,
                                                        input_ch->number,
                                                        output_ch->member,
                                                        output_ch->number);

            /* change to the next item on in_channel_list */
            item = next_item;
        }
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_MIXER_STATUS_OK;
}

sw_mixer_status_t stream_function_sw_mixer_channel_connect
(sw_mixer_member_t *in_member, uint32_t in_ch_number, sw_mixer_channel_attribute_t attribute, sw_mixer_member_t *out_member, uint32_t out_ch_number)
{
    uint32_t saved_mask;
    uint32_t i;
    sw_mixer_input_channel_t  *input_ch;
    sw_mixer_output_channel_t *output_ch;
    sw_mixer_list_item_t *c_item;
    sw_mixer_list_item_t *in_channel_list_item;
    sw_mixer_list_item_t *out_channel_list_item;

    /* check parameters */
    if ((attribute >= SW_MIXER_CHANNEL_ATTRIBUTE_MAX) ||
        (in_member == NULL) ||
        (out_member == NULL))
    {
        return SW_MIXER_STATUS_ERROR;
    }

    if (in_member->port != out_member->port)
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    /* check members' status for making sure they are in using */
    if ((in_member->status != SW_MIXER_MEMBER_STATUS_REGISTERED) ||
        (out_member->status != SW_MIXER_MEMBER_STATUS_REGISTERED))
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    if ((in_ch_number  == 0) ||
        (in_ch_number  > in_member->total_in_channels) ||
        (out_ch_number  == 0) ||
        (out_ch_number > out_member->total_out_channels))
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    /* get input channel and output channel */
    input_ch  = in_member->input_ch + (in_ch_number - 1);
    output_ch = out_member->output_ch + (out_ch_number - 1);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* insert input channel into the head or the tail on the the in_channel_list of the output channel */
    if (output_ch->in_channel_list.item_number == 0)
    {
        /* get list items of in_channel_list */
        in_channel_list_item  = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, sizeof(sw_mixer_list_item_t));
        if (in_channel_list_item == NULL)
        {
            assert(0);
        }
        in_channel_list_item->channel = (void *)input_ch;
        in_channel_list_item->attribute = attribute;
        in_channel_list_item->next_item = NULL;
        in_channel_list_item->perv_item = NULL;

        /* this input channel is the first item on in_channel_list */
        output_ch->in_channel_list.first_item = in_channel_list_item;
        output_ch->in_channel_list.last_item  = in_channel_list_item;
    }
    else
    {
        /* check if this input channel has been connected to the output channel */
        c_item = output_ch->in_channel_list.first_item;
        for (i = 0; i < output_ch->in_channel_list.item_number; i++)
        {
            if (c_item->channel == ((void *)input_ch))
            {
                /* yes, this input channel was connected to the output channel */
                hal_nvic_restore_interrupt_mask(saved_mask);
                return SW_MIXER_STATUS_OK;
            }
            c_item = c_item->next_item;
        }

        /* get list items of in_channel_list */
        in_channel_list_item  = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, sizeof(sw_mixer_list_item_t));
        if (in_channel_list_item == NULL)
        {
            assert(0);
        }
        in_channel_list_item->channel = (void *)input_ch;
        in_channel_list_item->attribute = attribute;
        in_channel_list_item->next_item = NULL;
        in_channel_list_item->perv_item = NULL;

        if (attribute == SW_MIXER_CHANNEL_ATTRIBUTE_MAIN)
        {
            /* insert this main input channel into the head of the output_ch->in_channel_list */
            if (output_ch->in_channel_list.first_item->attribute == SW_MIXER_CHANNEL_ATTRIBUTE_MAIN)
            {
                /* there is other main channel on output_ch->in_channel_list */
                assert(0);
            }
            in_channel_list_item->next_item = output_ch->in_channel_list.first_item;
            output_ch->in_channel_list.first_item->perv_item = in_channel_list_item;
            output_ch->in_channel_list.first_item = in_channel_list_item;
        }
        else
        {
            /* insert this input channel into the tail of the output_ch->in_channel_list */
            in_channel_list_item->perv_item = output_ch->in_channel_list.last_item;
            output_ch->in_channel_list.last_item->next_item = in_channel_list_item;
            output_ch->in_channel_list.last_item = in_channel_list_item;
        }
    }
    output_ch->in_channel_list.item_number += 1;
    if (output_ch->in_channel_list.item_number == 0)
    {
        assert(0);
    }

    /* insert output channel into the tail on the out_channel_list of the input channel */
    out_channel_list_item = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, sizeof(sw_mixer_list_item_t));
    if (out_channel_list_item == NULL)
    {
        assert(0);
    }
    out_channel_list_item->channel = (void *)output_ch;
    out_channel_list_item->attribute = attribute;
    out_channel_list_item->next_item = NULL;
    out_channel_list_item->perv_item = NULL;
    if (input_ch->out_channel_list.item_number == 0)
    {
        input_ch->out_channel_list.first_item = out_channel_list_item;
        input_ch->out_channel_list.last_item  = out_channel_list_item;
    }
    else
    {
        /*  do not need to check if this output channel has been connected to the input channel,
            because it has been checked at the time of insertting input channel */

        /* insert this output channel into the tail of the input_ch->out_channel_list */
        out_channel_list_item->perv_item = input_ch->out_channel_list.last_item;
        input_ch->out_channel_list.last_item->next_item = out_channel_list_item;
        input_ch->out_channel_list.last_item = out_channel_list_item;
    }
    input_ch->out_channel_list.item_number += 1;
    if (input_ch->out_channel_list.item_number == 0)
    {
        assert(0);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_MIXER_STATUS_OK;
}

sw_mixer_status_t stream_function_sw_mixer_channel_disconnect
(sw_mixer_member_t *in_member, uint32_t in_ch_number, sw_mixer_member_t *out_member, uint32_t out_ch_number)
{
    uint32_t saved_mask;
    uint32_t i;
    sw_mixer_input_channel_t  *input_ch;
    sw_mixer_output_channel_t *output_ch;
    sw_mixer_list_item_t *c_item;
    sw_mixer_list_item_t *in_channel_list_item = NULL;
    sw_mixer_list_item_t *out_channel_list_item = NULL;

    /* check parameters */
    if ((in_member == NULL) ||
        (out_member == NULL))
    {
        return SW_MIXER_STATUS_ERROR;
    }

    if (in_member->port != out_member->port)
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    if ((in_ch_number  == 0) ||
        (in_ch_number  > in_member->total_in_channels) ||
        (out_ch_number  == 0) ||
        (out_ch_number > out_member->total_out_channels))
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    /* get input channel and output channel */
    input_ch  = in_member->input_ch + (in_ch_number - 1);
    output_ch = out_member->output_ch + (out_ch_number - 1);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* check members' status fpr making sure they are in using */
    if ((in_member->status != SW_MIXER_MEMBER_STATUS_REGISTERED) ||
        (out_member->status != SW_MIXER_MEMBER_STATUS_REGISTERED))
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    /* delete input channel on the the in_channel_list of the output channel */
    in_channel_list_item = NULL;
    c_item = output_ch->in_channel_list.first_item;
    for (i = 0; i < output_ch->in_channel_list.item_number; i++)
    {
        /* check if it is the input channel */
        if ((sw_mixer_input_channel_t *)(c_item->channel) == input_ch)
        {
            /* find out the input channel */
            in_channel_list_item = c_item;

            /* update list */
            if ((in_channel_list_item->perv_item != NULL) && (in_channel_list_item->next_item != NULL))
            {
                /* this input channel is on the middle of in_channel_list */
                in_channel_list_item->perv_item->next_item = in_channel_list_item->next_item;
                in_channel_list_item->next_item->perv_item = in_channel_list_item->perv_item;
            }
            else if ((in_channel_list_item->perv_item == NULL) && (in_channel_list_item->next_item != NULL))
            {
                /* this input channel is on the head of in_channel_list, but it is not the only one item on in_channel_list */
                in_channel_list_item->next_item->perv_item = NULL;
                output_ch->in_channel_list.first_item = in_channel_list_item->next_item;
            }
            else if ((in_channel_list_item->perv_item != NULL) && (in_channel_list_item->next_item == NULL))
            {
                /* this input channel is on the tail of in_channel_list, but it is not the only one item on in_channel_list */
                in_channel_list_item->perv_item->next_item = NULL;
                output_ch->in_channel_list.last_item = in_channel_list_item->perv_item;
            }
            else
            {
                /* this input channel is the only one item on in_channel_list */
                output_ch->in_channel_list.first_item = NULL;
                output_ch->in_channel_list.last_item  = NULL;
            }

            break;
        }

        /* switch to the next item */
        c_item = c_item->next_item;
    }
    if (in_channel_list_item != NULL)
    {
        if (output_ch->in_channel_list.item_number == 0)
        {
            assert(0);
        }
        output_ch->in_channel_list.item_number -= 1;
        preloader_pisplit_free_memory(in_channel_list_item);
    }

    /* delete output channel on the the out_channel_list of the input channel */
    out_channel_list_item = NULL;
    c_item = input_ch->out_channel_list.first_item;
    for (i = 0; i < input_ch->out_channel_list.item_number; i++)
    {
        if ((sw_mixer_output_channel_t *)(c_item->channel) == output_ch)
        {
            /* find out the output channel */
            out_channel_list_item = c_item;

            /* update list */
            if ((out_channel_list_item->perv_item != NULL) && (out_channel_list_item->next_item != NULL))
            {
                /* this output channel is on the middle of out_channel_list */
                out_channel_list_item->perv_item->next_item = out_channel_list_item->next_item;
                out_channel_list_item->next_item->perv_item = out_channel_list_item->perv_item;
            }
            else if ((out_channel_list_item->perv_item == NULL) && (out_channel_list_item->next_item != NULL))
            {
                /* this output channel is on the head of out_channel_list, but it is not the only one item on out_channel_list */
                out_channel_list_item->next_item->perv_item = NULL;
                input_ch->out_channel_list.first_item = out_channel_list_item->next_item;
            }
            else if ((out_channel_list_item->perv_item != NULL) && (out_channel_list_item->next_item == NULL))
            {
                /* this output channel is on the tail of out_channel_list, but it is not the only one item on out_channel_list */
                out_channel_list_item->perv_item->next_item = NULL;
                input_ch->out_channel_list.last_item = out_channel_list_item->perv_item;
            }
            else
            {
                /* this output channel is the only one item on out_channel_list */
                input_ch->out_channel_list.first_item = NULL;
                input_ch->out_channel_list.last_item  = NULL;
            }

            break;
        }

        /* switch to the next item */
        c_item = c_item->next_item;
    }
    if (out_channel_list_item != NULL)
    {
        if (input_ch->out_channel_list.item_number == 0)
        {
            assert(0);
        }
        input_ch->out_channel_list.item_number -= 1;
        preloader_pisplit_free_memory(out_channel_list_item);
    }

    if (((in_channel_list_item == NULL) && (out_channel_list_item != NULL)) ||
        ((in_channel_list_item != NULL) && (out_channel_list_item == NULL)))
    {
        /* in here, it means input channel and output channel are not in pair on channel list */
        assert(0);
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

    return SW_MIXER_STATUS_OK;
}

sw_mixer_status_t stream_function_sw_mixer_channel_input_buffer_clean(sw_mixer_member_t *member, uint32_t in_ch_number, bool set_zeros)
{
    uint32_t saved_mask;
    sw_mixer_input_channel_t  *input_ch;

    /* check parameters */
    if (member == NULL)
    {
        return SW_MIXER_STATUS_ERROR;
    }

    if ((in_ch_number == 0) ||
        (in_ch_number > member->total_in_channels))
    {
        assert(0);
        return SW_MIXER_STATUS_ERROR;
    }

    /* get input channel */
    input_ch  = member->input_ch + (in_ch_number - 1);

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* clean in_buf */
    input_ch->data_size = 0;
    input_ch->read_from = input_ch->in_buf;
    input_ch->write_to  = input_ch->in_buf;

    hal_nvic_restore_interrupt_mask(saved_mask);

    /* check if needs to set zeros into the in buffer */
    if (set_zeros == true)
    {
        memset(input_ch->in_buf, 0, input_ch->buf_size);
    }

    return SW_MIXER_STATUS_OK;
}

ATTR_TEXT_IN_IRAM bool stream_function_sw_mixer_initialize(void *para)
{
    int32_t i, j;
    sw_mixer_port_handle_t *port = NULL;
    sw_mixer_member_t *c_member;
    sw_mixer_member_t *member;
    sw_mixer_output_channel_t *output_ch;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    uint32_t channel_number = stream_function_get_channel_number(para);

    member = NULL;
    for (i = 0; i < SW_MIXER_PORT_MAX; i++)
    {
        port = &sw_mixer_port_handle[i];
        c_member = port->first_member;
        for (j = 0; j < port->total_members; j++)
        {
            if ((c_member->owner == stream_ptr->source) ||
                (c_member->owner == stream_ptr->sink))
            {
                member = c_member;
                goto find_member;
            }

            c_member = c_member->next_member;
        }
    }

find_member:
    if (member == NULL)
    {
        assert(0);
        return true;
    }

    member->stream = stream_ptr;

    /* config out buffers */
    if (member->total_in_channels != channel_number)
    {
        assert(0);
        return true;
    }
    for (i = 0; i < member->total_out_channels; i++)
    {
        output_ch = member->output_ch + i;
        output_ch->out_buf = (uint8_t *)stream_function_get_inout_buffer(para, i+1);
    }

    #if SW_MIXER_DEBUG_LOG
    DSP_MW_LOG_I("[SW_MIXER] port %u, member = 0x%x, init done. port = %d, number = %d, mode = %d, owner = 0x%x, stream = 0x%x, in_ch = %u, out_ch = %u", 9,
                    port,
                    member,
                    member->port,
                    member->number,
                    member->mode,
                    member->owner,
                    member->stream,
                    member->total_in_channels,
                    member->total_out_channels);
    #endif /* SW_BUFFER_DEBUG_LOG */

    return false;
}

ATTR_TEXT_IN_IRAM bool stream_function_sw_mixer_process(void *para)
{
    int32_t i, j;
    sw_mixer_status_t status;
    sw_mixer_port_handle_t *port = NULL;
    sw_mixer_member_t *c_member;
    sw_mixer_member_t *member;
    sw_mixer_input_channel_t  *input_ch;
    sw_mixer_output_channel_t *output_ch;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    stream_resolution_t resolution = stream_function_get_output_resolution(para);
    uint32_t in_frame_size = stream_function_get_output_size(para);
    uint32_t out_frame_size = 0;
    uint32_t channel_number = stream_function_get_channel_number(para);

    member = NULL;
    for (i = 0; i < SW_MIXER_PORT_MAX; i++)
    {
        port = &sw_mixer_port_handle[i];
        c_member = port->first_member;
        for (j = 0; j < port->total_members; j++)
        {
            if ((c_member->owner == stream_ptr->source) ||
                (c_member->owner == stream_ptr->sink))
            {
                member = c_member;
                goto find_member;
            }

            c_member = c_member->next_member;
        }
    }

find_member:
    if (member == NULL)
    {
        assert(0);
        return true;
    }

    /* call preprocess callback */
    if (member->preprocess_callback)
    {
        member->preprocess_callback(member, para);
    }

    /* copy data from stream buffer into input channels' internal buffer */
    if (member->total_in_channels != channel_number)
    {
        assert(0);
        return true;
    }
    for (i = 0; i < member->total_in_channels; i++)
    {
        input_ch = member->input_ch + i;
        status = sw_mixer_copy_data(input_ch, resolution, in_frame_size, stream_function_get_inout_buffer(para, i+1));
    }

    /* mix data from all input channels' internal buffer into output channels's buffer(default stream buffer ) */
    for (i = 0; i < member->total_out_channels; i++)
    {
        output_ch = member->output_ch + i;
        out_frame_size = sw_mixer_mix_data( output_ch,
                                            resolution,
                                            in_frame_size,
                                            output_ch->out_buf,
                                            stream_ptr->callback.EntryPara.out_malloc_size);
        output_ch->data_size = out_frame_size;
    }

    /* call postprocess callback */
    if (member->postprocess_callback)
    {
        member->postprocess_callback(member, para, &out_frame_size);
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&(member->finish_gpt_count));

    switch (member->mode)
    {
        case SW_MIXER_MEMBER_MODE_NO_BYPASS:
            stream_function_modify_output_size(para, out_frame_size);
            return false;

        case SW_MIXER_MEMBER_MODE_BYPASS_FEATURES:
            /* for bypass all subsequent sink operations */
            stream_ptr->callback.EntryPara.encoder_out_size = 0;
            stream_ptr->callback.EntryPara.src_out_size = 0;
            stream_ptr->callback.EntryPara.codec_out_size = 0;

            /* for bypass all subsequent features */
            return true;

        case SW_MIXER_MEMBER_MODE_BYPASS_FEATURES_SINK:
            /* for bypass all subsequent sink operations */
            stream_ptr->callback.EntryPara.encoder_out_size = 0;
            stream_ptr->callback.EntryPara.src_out_size = 0;
            stream_ptr->callback.EntryPara.codec_out_size = 0;

            /* for bypass all subsequent features */
            return true;

        case SW_MIXER_MEMBER_MODE_BYPASS_ALL:
            /* for bypass all subsequent source operations */
            stream_ptr->callback.EntryPara.in_size = 0;

            /* for bypass all subsequent sink operations */
            stream_ptr->callback.EntryPara.encoder_out_size = 0;
            stream_ptr->callback.EntryPara.src_out_size = 0;
            stream_ptr->callback.EntryPara.codec_out_size = 0;

            /* for bypass all subsequent features */
            return true;

        default:
            assert(0);
            return true;
    }
}
