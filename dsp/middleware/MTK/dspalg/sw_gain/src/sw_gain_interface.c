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
#include "sw_gain_interface.h"
#include "dsp_dump.h"
#include "memory_attribute.h"
#include "preloader_pisplit.h"
#include <xtensa/tie/xt_hifi2.h>


/* Private define ------------------------------------------------------------*/
#define SW_GAIN_DEBUG_LOG               0
#define SW_GAIN_DEBUG_DUMP              0


/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static sw_gain_port_t sw_gain_port[SW_GAIN_PORT_MAX];


/* Public variables ----------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
ATTR_TEXT_IN_IRAM __attribute__ ((noinline))
sw_gain_status_t sw_gain_16bit_process(uint16_t *in_buf, uint16_t *ou_buf, uint32_t samples, sw_gain_config_t *p_config)
{
    uint32_t i;
    int32_t multiplier, decimal_x0_multiplier = 0;
    int32_t integer_db, decimal_x0_db=0;
    int32_t times_6db, remain_multiply=0;
    int32_t decimal_0x_db=0;
    int32_t decimal_0x_multiplier = 0;
    ae_p24x2s p0, p1, p2, p3;
    ae_q56s qh, ql;
#if SW_GAIN_DEBUG_LOG
    uint32_t current_timestamp = 0;
#endif /* SW_GAIN_DEBUG_LOG */

    if (p_config->current_gain == 0)
    {
        /* do not need to do gain on this channel */
        times_6db = 0;
        remain_multiply = 0;
        decimal_x0_db = 0;
        decimal_0x_db = 0;

        /* special case, in_buf and ou_buf are the same and gain is 0, so can return directly here */
        if (in_buf == ou_buf)
        {
            return SW_GAIN_STATUS_OK;
        }
    }
    else if (p_config->current_gain <= -9600)
    {
        /* special case, output all zero data here */
        for (i=0; i < (samples/2); i++)
        {
            *((uint32_t *)ou_buf+i) = 0;
        }
        if (samples%2)
        {
            *(ou_buf+samples-1) = 0;
        }
        return SW_GAIN_STATUS_OK;
    }
    else if (p_config->current_gain > 0)
    {
        integer_db = (p_config->current_gain+99)/100;
        decimal_x0_db = (integer_db*100-p_config->current_gain)/10;
        decimal_0x_db = (integer_db*100-p_config->current_gain)%10;
        times_6db = integer_db/6;
        if ((integer_db%6) != 0)
        {
            times_6db += 1;
        }
        remain_multiply = times_6db*6 - integer_db;
    }
    else
    {
        integer_db = (-p_config->current_gain)/100;
        decimal_x0_db = ((-p_config->current_gain)%100)/10;
        decimal_0x_db = ((-p_config->current_gain)%100)%10;
        times_6db = integer_db/6;
        remain_multiply = integer_db - times_6db*6;
    }

    if (remain_multiply == 1)
    {
        // -1db 0.8912509381337455299531086810783
        // 0.8912509381337455299531086810783 / (2^-23) = 0x721483
        multiplier = 0x00721483;
    }
    else if (remain_multiply == 2)
    {
        // -2db 0.79432823472428150206591828283639
        // 0.79432823472428150206591828283639 / (2^-23) = 0x65ac8c
        multiplier = 0x0065ac8c;
    }
    else if (remain_multiply == 3)
    {
        // -3db 0.70794578438413791080221494218931
        // 0.70794578438413791080221494218931 / (2^-23) = 0x5a9df8
        multiplier = 0x005a9df8;
    }
    else if (remain_multiply == 4)
    {
        // -4db 0.63095734448019324943436013662234
        // 0.63095734448019324943436013662234 / (2^-23) = 0x50c336
        multiplier = 0x0050c336;
    }
    else if (remain_multiply == 5)
    {
        // -5db 0.56234132519034908039495103977648
        // 0.56234132519034908039495103977648 / (2^-23) = 0x47facd
        multiplier = 0x0047facd;
    }

    /* process -0.x0db part */
    if (decimal_x0_db == 1)
    {
        // -0.1db 0.98855309465693884028524792978203
        // 0.98855309465693884028524792978203 / (2^-23) = 0x7e88e8
        decimal_x0_multiplier = 0x007e88e8;
    }
    else if (decimal_x0_db == 2)
    {
        // -0.2db 0.97723722095581068269707600696156
        // 0.97723722095581068269707600696156 / (2^-23) = 0x7d161c
        decimal_x0_multiplier = 0x007d161c;
    }
    else if (decimal_x0_db == 3)
    {
        // -0.3db 0.96605087898981337427673560676923
        // 0.96605087898981337427673560676923 / (2^-23) = 0x7ba78e
        decimal_x0_multiplier = 0x007ba78e;
    }
    else if (decimal_x0_db == 4)
    {
        // -0.4db 0.95499258602143594972395937950148
        // 0.95499258602143594972395937950148 / (2^-23) = 0x7a3d32
        decimal_x0_multiplier = 0x007a3d32;
    }
    else if (decimal_x0_db == 5)
    {
        // -0.5db 0.94406087628592338036438049660227
        // 0.94406087628592338036438049660227 / (2^-23) = 0x78d6fd
        decimal_x0_multiplier = 0x0078d6fd;
    }
    else if (decimal_x0_db == 6)
    {
        // -0.6db 0.93325430079699104353209661168365
        // 0.93325430079699104353209661168365 / (2^-23) = 0x7774e0
        decimal_x0_multiplier = 0x007774e0;
    }
    else if (decimal_x0_db == 7)
    {
        // -0.7db 0.92257142715476316003073802267767
        // 0.92257142715476316003073802267767 / (2^-23) = 0x7616d2
        decimal_x0_multiplier = 0x007616d2;
    }
    else if (decimal_x0_db == 8)
    {
        // -0.8db 0.91201083935590974212095940791872
        // 0.91201083935590974212095940791872 / (2^-23) = 0x74bcc5
        decimal_x0_multiplier = 0x0074bcc5;
    }
    else if (decimal_x0_db == 9)
    {
        // -0.9db 0.90157113760595688589246344194515
        // 0.90157113760595688589246344194515 / (2^-23) = 0x7366af
        decimal_x0_multiplier = 0x007366af;
    }

    /* process -0.0xdb part */
    if (decimal_0x_db == 1)
    {
        // -0.01db 0.99884936993650514951538205746463
        // 0.99884936993650514951538205746463 / (2^-23) = 0x7fda4c
        decimal_0x_multiplier = 0x007fda4c;
    }
    else if (decimal_0x_db == 2)
    {
        // -0.02db 0.99770006382255331719442194285376
        // 0.99770006382255331719442194285376 / (2^-23) = 0x7fb4a3
        decimal_0x_multiplier = 0x007fb4a3;
    }
    else if (decimal_0x_db == 3)
    {
        // -0.03db 0.9965520801347683562901517234443
        // 0.9965520801347683562901517234443 / (2^-23) = 0x7f8f04
        decimal_0x_multiplier = 0x007f8f04;
    }
    else if (decimal_0x_db == 4)
    {
        // -0.04db 0.99540541735152696244806147089511
        // 0.99540541735152696244806147089511 / (2^-23) = 0x7f6972
        decimal_0x_multiplier = 0x007f6972;
    }
    else if (decimal_0x_db == 5)
    {
        // -0.05db 0.9942600739529566568329459308278
        // 0.9942600739529566568329459308278 / (2^-23) = 0x7f43ea
        decimal_0x_multiplier = 0x007f43ea;
    }
    else if (decimal_0x_db == 6)
    {
        // -0.06db 0.99311604842093377157642607688515
        // 0.99311604842093377157642607688515 / (2^-23) = 0x7f1e6d
        decimal_0x_multiplier = 0x007f1e6d;
    }
    else if (decimal_0x_db == 7)
    {
        // -0.07db 0.99197333923908143754247531097752
        // 0.99197333923908143754247531097752 / (2^-23) = 0x7ef8fb
        decimal_0x_multiplier = 0x007ef8fb;
    }
    else if (decimal_0x_db == 8)
    {
        // -0.08db 0.99083194489276757440828314388392
        // 0.99083194489276757440828314388392 / (2^-23) = 0x7ed395
        decimal_0x_multiplier = 0x007ed395;
    }
    else if (decimal_0x_db == 9)
    {
        // -0.09db 0.9896918638691028830577922592959
        // 0.9896918638691028830577922592959 / (2^-23) = 0x7eae39
        decimal_0x_multiplier = 0x007eae39;
    }

    if (remain_multiply != 0)
    {
        /* p2.l is multiplier[23:0], p2.h is multiplier[23:0] */
        p2 = *((ae_p24s *)&multiplier);
    }

    if (decimal_x0_db != 0)
    {
        /* p1.l is decimal_x0_multiplier[23:0], p1.h is decimal_x0_multiplier[23:0] */
        p1 = *((ae_p24s *)&decimal_x0_multiplier);
    }

    if (decimal_0x_db != 0)
    {
        /* p1.l is decimal_0x_multiplier[23:0], p1.h is decimal_0x_multiplier[23:0] */
        p3 = *((ae_p24s *)&decimal_0x_multiplier);
    }

    for (i = 0; i < samples; i += 2)
    {
        /* load two 16 bit data into AE_PR register at every time */
        p0 = *((ae_p16x2s *)((uint16_t *)in_buf + i));

        if (decimal_0x_db != 0)
        {
            /* p0.l * decimal_x0_multiplier[23:0] */
            ql = AE_MULFP24S_LL(p0, p3);
            /* p0.h * decimal_x0_multiplier[23:0] */
            qh = AE_MULFP24S_HH(p0, p3);
            // ql = AE_SATQ48S(ql);
            // qh = AE_SATQ48S(qh);
            /* truncates two 9.47 variables into two 1.23 variables */
            p0 = AE_TRUNCP24Q48X2(qh, ql);
        }

        if (decimal_x0_db != 0)
        {
            /* p0.l * decimal_x0_multiplier[23:0] */
            ql = AE_MULFP24S_LL(p0, p1);
            /* p0.h * decimal_x0_multiplier[23:0] */
            qh = AE_MULFP24S_HH(p0, p1);
            // ql = AE_SATQ48S(ql);
            // qh = AE_SATQ48S(qh);
            /* truncates two 9.47 variables into two 1.23 variables */
            p0 = AE_TRUNCP24Q48X2(qh, ql);
        }

        if (remain_multiply != 0)
        {
            /* p0.l * multiplier[23:0] */
            ql = AE_MULFP24S_LL(p0, p2);
            /* p0.h * multiplier[23:0] */
            qh = AE_MULFP24S_HH(p0, p2);
            // ql = AE_SATQ48S(ql);
            // qh = AE_SATQ48S(qh);
            /* truncates two 9.47 variables into two 1.23 variables */
            p0 = AE_TRUNCP24Q48X2(qh, ql);
        }

        if (p_config->current_gain > 0)
        {
            /* Signed saturating 24-bit left */
            p0 = p0 << times_6db;
        }
        else if (p_config->current_gain < 0)
        {
            /* Signed arithmetic 24-bit right */
            p0 = p0 >> times_6db;
        }

        if ((samples - i) == 1)
        {
            /* Truncate to one 16-bit data (p0.l) and store it into the memory at evety time */
            *((ae_p16s *)((uint16_t *)ou_buf + i)) = p0;
        }
        else
        {
            /* Truncate to two 16-bit data and store them into the memory at evety time */
            *((ae_p16x2s *)((uint16_t *)ou_buf + i)) = p0;
        }
    }

    #if SW_GAIN_DEBUG_LOG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SW_GAIN][16bit_process]: %d, %d, %d, %d, %d, %d, %d, %d, %d, -%d, %u, %d", 12,
                    p_config->current_gain,
                    p_config->target_gain,
                    p_config->up_step,
                    p_config->up_samples_per_step,
                    p_config->current_up_sample,
                    p_config->down_step,
                    p_config->down_samples_per_step,
                    p_config->current_down_sample,
                    times_6db*6,
                    remain_multiply,
                    current_timestamp,
                    hal_nvic_query_exception_number());
    #endif /* SW_GAIN_DEBUG_LOG */

    return SW_GAIN_STATUS_OK;
}

ATTR_TEXT_IN_IRAM __attribute__ ((noinline))
sw_gain_status_t sw_gain_16bit_process_with_up(uint16_t *in_buf, uint16_t *ou_buf, uint32_t samples, sw_gain_config_t *p_config)
{
    uint32_t processed_samples = 0;

    if (p_config->current_up_sample + samples >= p_config->up_samples_per_step)
    {
        /* up_samples_per_step and current_up_sample shoule always be odd */
        processed_samples = p_config->up_samples_per_step - p_config->current_up_sample;
        sw_gain_16bit_process(in_buf, ou_buf, processed_samples, p_config);
        p_config->current_up_sample = 0;
        p_config->current_gain = p_config->current_gain + p_config->up_step;
        if (p_config->current_gain > p_config->target_gain)
        {
            p_config->current_gain = p_config->target_gain;
        }
        while (processed_samples != samples)
        {
            if (samples - processed_samples >= p_config->up_samples_per_step)
            {
                sw_gain_16bit_process(in_buf+processed_samples, ou_buf+processed_samples, p_config->up_samples_per_step, p_config);
                p_config->current_up_sample = 0;
                p_config->current_gain = p_config->current_gain + p_config->up_step;
                if (p_config->current_gain > p_config->target_gain)
                {
                    p_config->current_gain = p_config->target_gain;
                }
                processed_samples += p_config->up_samples_per_step;
            }
            else
            {
                sw_gain_16bit_process(in_buf+processed_samples, ou_buf+processed_samples, samples-processed_samples, p_config);
                processed_samples = ((samples-processed_samples)%2) ? (samples-processed_samples-1) : (samples-processed_samples);
                p_config->current_up_sample += processed_samples;
                processed_samples = samples;
            }
        }
    }
    else
    {
        sw_gain_16bit_process(in_buf, ou_buf, samples, p_config);
        p_config->current_up_sample += samples;
    }

    return SW_GAIN_STATUS_OK;
}

ATTR_TEXT_IN_IRAM __attribute__ ((noinline))
sw_gain_status_t sw_gain_16bit_process_with_down(uint16_t *in_buf, uint16_t *ou_buf, uint32_t samples, sw_gain_config_t *p_config)
{
    uint32_t processed_samples = 0;

    if (p_config->current_down_sample + samples >= p_config->down_samples_per_step)
    {
        /* down_samples_per_step and current_down_sample shoule always be odd */
        processed_samples = p_config->down_samples_per_step - p_config->current_down_sample;
        sw_gain_16bit_process(in_buf, ou_buf, processed_samples, p_config);
        p_config->current_down_sample = 0;
        p_config->current_gain = p_config->current_gain + p_config->down_step;
        if (p_config->current_gain < p_config->target_gain)
        {
            p_config->current_gain = p_config->target_gain;
        }
        while (processed_samples != samples)
        {
            if (samples - processed_samples >= p_config->down_samples_per_step)
            {
                sw_gain_16bit_process(in_buf+processed_samples, ou_buf+processed_samples, p_config->down_samples_per_step, p_config);
                p_config->current_down_sample = 0;
                p_config->current_gain = p_config->current_gain + p_config->down_step;
                if (p_config->current_gain < p_config->target_gain)
                {
                    p_config->current_gain = p_config->target_gain;
                }
                processed_samples += p_config->down_samples_per_step;
            }
            else
            {
                sw_gain_16bit_process(in_buf+processed_samples, ou_buf+processed_samples, samples-processed_samples, p_config);
                processed_samples = ((samples-processed_samples)%2) ? (samples-processed_samples-1) : (samples-processed_samples);
                p_config->current_down_sample += processed_samples;
                processed_samples = samples;
            }
        }
    }
    else
    {
        sw_gain_16bit_process(in_buf, ou_buf, samples, p_config);
        p_config->current_down_sample += samples;
    }

    return SW_GAIN_STATUS_OK;
}


/* Public functions ----------------------------------------------------------*/
sw_gain_port_t *stream_function_sw_gain_get_port(void *owner)
{
    int32_t i;
    sw_gain_port_t *port = NULL;
    uint32_t saved_mask;

    /* Find out a port for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = SW_GAIN_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if there is unused port */
        if (sw_gain_port[i].owner == NULL)
        {
            port = &sw_gain_port[i];
            continue;
        }

        /* Check if this owner has already owned a sw gain */
        if (sw_gain_port[i].owner == owner)
        {
            port = &sw_gain_port[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port not enough!", 0);
        return port;
    }

    port->owner = owner;

    return port;
}

sw_gain_status_t stream_function_sw_gain_init(sw_gain_port_t *port, uint16_t total_channels, sw_gain_config_t *default_config)
{
    sw_gain_config_t *config = NULL;
    sw_gain_config_t *temp_config = NULL;
    uint32_t i;

    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* parameters check */
    if ((default_config->up_step <= 0) || (default_config->down_step >= 0))
    {
        DSP_MW_LOG_E("[SW_GAIN] Parameters error!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    config = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, total_channels * sizeof(sw_gain_config_t));
    if (config == NULL)
    {
        return SW_GAIN_STATUS_ERROR;
    }

    temp_config = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, total_channels * sizeof(sw_gain_config_t));
    if (temp_config == NULL)
    {
        preloader_pisplit_free_memory(config);
        return SW_GAIN_STATUS_ERROR;
    }

    port->total_channels = total_channels;
    port->config = config;
    port->temp_config = temp_config;
    for (i=0; i < total_channels; i++)
    {
        config = port->config + i;
        config->resolution              = default_config->resolution;
        config->target_gain             = default_config->target_gain;
        config->current_gain            = default_config->target_gain;
        config->up_step                 = default_config->up_step;
        config->up_samples_per_step     = (default_config->up_samples_per_step%2)
                                          ? (default_config->up_samples_per_step+1)
                                          : (default_config->up_samples_per_step);
        config->current_up_sample       = 0;
        config->down_step               = default_config->down_step;
        config->down_samples_per_step   = (default_config->down_samples_per_step%2)
                                          ? (default_config->down_samples_per_step+1)
                                          : (default_config->down_samples_per_step);
        config->current_down_sample     = 0;
    }
    port->status = SW_GAIN_PORT_STATUS_INIT;

    return SW_GAIN_STATUS_OK;
}

sw_gain_status_t stream_function_sw_gain_deinit(sw_gain_port_t *port)
{
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    if (port->config)
    {
        preloader_pisplit_free_memory(port->config);
        port->config = NULL;
    }
    if (port->temp_config)
    {
        preloader_pisplit_free_memory(port->temp_config);
        port->temp_config = NULL;
    }
    port->owner = NULL;
    port->stream = NULL;
    port->total_channels = 0;
    port->status = SW_GAIN_PORT_STATUS_DEINIT;

    return SW_GAIN_STATUS_OK;
}

sw_gain_status_t stream_function_sw_gain_get_config(sw_gain_port_t *port, uint16_t channel, sw_gain_config_t *config)
{
    sw_gain_config_t *default_config = NULL;

    /* check port */
    if ((port == NULL) || (config == NULL))
    {
        DSP_MW_LOG_E("[SW_GAIN] Port or user config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check channel */
    if (channel > port->total_channels)
    {
        DSP_MW_LOG_E("[SW_GAIN] channel is not right!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check config */
    if (port->config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* get channel config pointer */
    default_config = port->config + channel - 1;
    if (default_config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Channel config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* get configurations */
    config->resolution              = default_config->resolution;
    config->target_gain             = default_config->target_gain;
    config->current_gain            = default_config->current_gain;
    config->up_step                 = default_config->up_step;
    config->up_samples_per_step     = default_config->up_samples_per_step;
    config->current_up_sample       = default_config->current_up_sample;
    config->down_step               = default_config->down_step;
    config->down_samples_per_step   = default_config->down_samples_per_step;
    config->current_down_sample     = default_config->current_down_sample;

    return SW_GAIN_STATUS_OK;
}

sw_gain_status_t stream_function_sw_gain_configure_resolution(sw_gain_port_t *port, uint16_t channel, uint32_t new_resolution)
{
    sw_gain_config_t *config = NULL;

    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check channel */
    if (channel > port->total_channels)
    {
        DSP_MW_LOG_E("[SW_GAIN] channel is not right!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check config */
    if (port->config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* get channel config */
    config = port->config + channel - 1;
    if (config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Channel config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* do configuration */
    config->resolution = new_resolution;

    return SW_GAIN_STATUS_OK;
}

ATTR_TEXT_IN_IRAM sw_gain_status_t stream_function_sw_gain_configure_gain_target(sw_gain_port_t *port, uint16_t channel, int32_t new_gain)
{
    sw_gain_config_t *config = NULL;

    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check channel */
    if (channel > port->total_channels)
    {
        DSP_MW_LOG_E("[SW_GAIN] channel is not right!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check config */
    if (port->config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* get channel config */
    config = port->config + channel - 1;
    if (config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Channel config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* do configuration */
    config->target_gain = new_gain;

    return SW_GAIN_STATUS_OK;
}

sw_gain_status_t stream_function_sw_gain_configure_gain_up(sw_gain_port_t *port, uint16_t channel, int32_t new_up_step, int32_t new_up_samples_per_step)
{
    sw_gain_config_t *config = NULL;

    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check channel */
    if (channel > port->total_channels)
    {
        DSP_MW_LOG_E("[SW_GAIN] channel is not right!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check config */
    if (port->config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* parameters check */
    if (new_up_step <= 0)
    {
        DSP_MW_LOG_E("[SW_GAIN] Parameters error!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* get channel config */
    config = port->config + channel - 1;
    if (config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Channel config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* do configuration */
    config->up_step                 = new_up_step;
    config->up_samples_per_step     = (new_up_samples_per_step%2)?(new_up_samples_per_step+1):(new_up_samples_per_step);

    return SW_GAIN_STATUS_OK;
}

sw_gain_status_t stream_function_sw_gain_configure_gain_down(sw_gain_port_t *port, uint16_t channel, int32_t new_down_step, int32_t new_down_samples_per_step)
{
    sw_gain_config_t *config = NULL;

    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check channel */
    if (channel > port->total_channels)
    {
        DSP_MW_LOG_E("[SW_GAIN] channel is not right!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* check config */
    if (port->config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* parameters check */
    if (new_down_step >= 0)
    {
        DSP_MW_LOG_E("[SW_GAIN] Parameters error!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* get channel config */
    config = port->config + channel - 1;
    if (config == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Channel config is NULL!", 0);
        return SW_GAIN_STATUS_ERROR;
    }

    /* do configuration */
    config->down_step               = new_down_step;
    config->down_samples_per_step   = (new_down_samples_per_step%2)?(new_down_samples_per_step+1):(new_down_samples_per_step);

    return SW_GAIN_STATUS_OK;
}

ATTR_TEXT_IN_IRAM bool stream_function_sw_gain_initialize(void *para)
{
    int32_t i;
    sw_gain_port_t *port = NULL;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);

    /* Find out the sw gain port of this stream */
    for (i = SW_GAIN_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if this source or sink has already owned a sw gain */
        if ((sw_gain_port[i].owner == stream_ptr->source) ||
            (sw_gain_port[i].owner == stream_ptr->sink))
        {
            port = &sw_gain_port[i];
            break;
        }
    }
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return true;
    }

    /* status check */
    if (port->status == SW_GAIN_PORT_STATUS_INIT)
    {
        port->status = SW_GAIN_PORT_STATUS_RUNNING;
    }
    else if (port->status == SW_GAIN_PORT_STATUS_RUNNING)
    {
        return false;
    }
    else
    {
        DSP_MW_LOG_E("[SW_GAIN] error status, %d!", 1, port->status);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    port->stream = stream_ptr;

    DSP_MW_LOG_I("[SW_GAIN] port %u init done, owner = 0x%x, stream = 0x%x", 3, port, port->owner, port->stream);

    return false;
}

ATTR_TEXT_IN_IRAM bool stream_function_sw_gain_process(void *para)
{
    int32_t i;
    uint32_t channel_number;
    sw_gain_port_t *port = NULL;
    sw_gain_config_t *p_config = NULL;
    sw_gain_config_t *p_temp_config = NULL;
    uint32_t saved_mask;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    stream_resolution_t resolution = stream_function_get_output_resolution(para);
    uint32_t in_frame_size = stream_function_get_output_size(para);
    uint8_t *in_buf = NULL;
    uint32_t samples = 0;

    channel_number = stream_function_get_channel_number(para);

    #if SW_GAIN_DEBUG_LOG
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SW_GAIN][start]: %d, %u, %u, %d", 4, in_frame_size, channel_number, current_timestamp, hal_nvic_query_exception_number());
    #endif /* SW_GAIN_DEBUG_LOG */

    /* Find out the sw gain port of this stream */
    for (i = SW_GAIN_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if this source or sink has already owned a sw gain */
        if ((sw_gain_port[i].owner == stream_ptr->source) ||
            (sw_gain_port[i].owner == stream_ptr->sink))
        {
            port = &sw_gain_port[i];
            break;
        }
    }

    /* Parameters check */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SW_GAIN] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return true;
    }
    if (channel_number != port->total_channels)
    {
        DSP_MW_LOG_E("[SW_GAIN] channel number is not right!", 0);
        OS_ASSERT(FALSE);
        return true;
    }
    if ((port->config == NULL) || (port->temp_config == NULL))
    {
        DSP_MW_LOG_E("[SW_GAIN] Port config is NULL!", 0);
        OS_ASSERT(FALSE);
        return true;
    }

    if (!in_frame_size) {
        return FALSE;
    }

    /* fetch config into temp_config */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = 0; (uint32_t)i < channel_number ; i++)
    {
        p_temp_config = port->temp_config + i;
        p_config = port->config + i;
        /* extand it for better peformance */
        p_temp_config->resolution               = p_config->resolution;
        p_temp_config->target_gain              = p_config->target_gain;
        p_temp_config->current_gain             = p_config->current_gain;
        p_temp_config->up_step                  = p_config->up_step;
        p_temp_config->up_samples_per_step      = p_config->up_samples_per_step;
        p_temp_config->current_up_sample        = p_config->current_up_sample;
        p_temp_config->down_step                = p_config->down_step;
        p_temp_config->down_samples_per_step    = p_config->down_samples_per_step;
        p_temp_config->current_down_sample      = p_config->current_down_sample;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    for (i = 1; (uint32_t)i <= channel_number ; i++)
    {
        p_temp_config = port->temp_config + i - 1;
        p_config = port->config + i - 1;

        if (resolution != p_temp_config->resolution)
        {
            DSP_MW_LOG_E("[SW_GAIN] resolution is dismatch!", 0);
            OS_ASSERT(FALSE);
            return true;
        }

        switch (resolution)
        {
            case RESOLUTION_16BIT:
                in_buf = stream_function_get_inout_buffer(para, i);
                samples = in_frame_size / sizeof(S16);

                #if SW_GAIN_DEBUG_DUMP
                // LOG_AUDIO_DUMP(in_buf, in_frame_size, 13);
                #endif /* SW_GAIN_DEBUG_DUMP */

                if (p_temp_config->current_gain == p_temp_config->target_gain)
                {
                    /* normal */
                    sw_gain_16bit_process((uint16_t *)in_buf, (uint16_t *)in_buf, samples, p_temp_config);
                    /* set new setting into config from the temp config  */
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    p_config->current_up_sample = 0;
                    p_config->current_down_sample = 0;
                    hal_nvic_restore_interrupt_mask(saved_mask);
                }
                else if (p_temp_config->current_gain > p_temp_config->target_gain)
                {
                    /* down step */
                    sw_gain_16bit_process_with_down((uint16_t *)in_buf, (uint16_t *)in_buf, samples, p_temp_config);
                    /* set new setting into config from the temp config  */
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    p_config->current_up_sample = p_temp_config->current_up_sample;
                    p_config->current_down_sample = p_temp_config->current_down_sample;
                    p_config->current_gain = p_temp_config->current_gain;
                    hal_nvic_restore_interrupt_mask(saved_mask);
                }
                else if (p_temp_config->current_gain < p_temp_config->target_gain)
                {
                    /* up step */
                    sw_gain_16bit_process_with_up((uint16_t *)in_buf, (uint16_t *)in_buf, samples, p_temp_config);
                    /* set new setting into config from the temp config  */
                    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                    p_config->current_up_sample = p_temp_config->current_up_sample;
                    p_config->current_down_sample = p_temp_config->current_down_sample;
                    p_config->current_gain = p_temp_config->current_gain;
                    hal_nvic_restore_interrupt_mask(saved_mask);
                }

                #if SW_GAIN_DEBUG_DUMP
                LOG_AUDIO_DUMP(in_buf, in_frame_size, 13);
                #endif /* SW_GAIN_DEBUG_DUMP */

                break;

            case RESOLUTION_32BIT:
                /* not support now */
                DSP_MW_LOG_E("[SW_GAIN] resolution is not support !", 0);
                OS_ASSERT(FALSE);
                return true;

                break;

            default:
                DSP_MW_LOG_E("[SW_GAIN] resolution is not support !", 0);
                OS_ASSERT(FALSE);
                return true;

                break;
        }
    }

    #if SW_GAIN_DEBUG_LOG
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SW_GAIN][finish]:%u, %u, %d", 3,
                    in_frame_size,
                    current_timestamp,
                    hal_nvic_query_exception_number());
    #endif /* SW_GAIN_DEBUG_LOG */

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, (uint32_t *)&(port->finish_gpt_count));

    return false;
}
