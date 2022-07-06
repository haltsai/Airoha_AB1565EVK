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

/* Includes ------------------------------------------------------------------*/
#include "src_fixed_ratio_interface.h"
#include "dsp_feature_interface.h"
#include "dsp_dump.h"
#include "preloader_pisplit.h"
/* Private define ------------------------------------------------------------*/
#define SRC_FIXED_RATIO_PORT_MAX    (10)
#define SRC_FIXED_RATIO_CVT_MAX     (3)
#define SRC_FIXED_RATIO_MAX         (3)
#define SRC_FIXED_RATIO_DEBUG_LOG   (0)
#define SRC_FIXED_RATIO_DEBUG_DUMP   (0)

#define ENDIAN_RVRS(A) (A)
/* Private typedef -----------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static src_fixed_ratio_port_t src_fixed_ratio_port[SRC_FIXED_RATIO_PORT_MAX];
const int16_t dsp_sampler_coef[32] =
{
    ENDIAN_RVRS((S16)0x1AC0),    // LP2_COFEB16_00
    ENDIAN_RVRS((S16)0x1D37),    // LP2_COEFB16_01
    ENDIAN_RVRS((S16)0x49A7),    // LP2_COEFB16_02
    ENDIAN_RVRS((S16)0xC906),    // LP2_COEFB16_03
    ENDIAN_RVRS((S16)0x1D37),    // LP2_COEFB16_04
    ENDIAN_RVRS((S16)0x1D37),    // LP2_COEFB16_05
    ENDIAN_RVRS((S16)0x0FC0),    // LP2_COEFB16_06
    ENDIAN_RVRS((S16)0x1D37),    // LP2_COEFB16_07
    ENDIAN_RVRS((S16)0x2CCB),    // LP2_COEFB16_08
    ENDIAN_RVRS((S16)0xA9FB),    // LP2_COEFB16_09
    ENDIAN_RVRS((S16)0x0975),    // LP2_COEFB16_10
    ENDIAN_RVRS((S16)0x1D37),    // LP2_COEFB16_11
    ENDIAN_RVRS((S16)0x1977),    // LP2_COEFB16_12
    ENDIAN_RVRS((S16)0x8CC7),    // LP2_COEFB16_13
    ENDIAN_RVRS((S16)0x1D37),    // LP2_COEFB24_14
    ENDIAN_RVRS((S16)0     ),    // LP2_Reserved

    ENDIAN_RVRS((S16)0x12CC),    // LP3_COFEB16_00
    ENDIAN_RVRS((S16)0x1D43),    // LP3_COEFB16_01
    ENDIAN_RVRS((S16)0x4B0B),    // LP3_COEFB16_02
    ENDIAN_RVRS((S16)0xCDAF),    // LP3_COEFB16_03
    ENDIAN_RVRS((S16)0x1D43),    // LP3_COEFB16_04
    ENDIAN_RVRS((S16)0x1D43),    // LP3_COEFB16_05
    ENDIAN_RVRS((S16)0xFB40),    // LP3_COEFB16_06
    ENDIAN_RVRS((S16)0x1D43),    // LP3_COEFB16_07
    ENDIAN_RVRS((S16)0x4934),    // LP3_COEFB16_08
    ENDIAN_RVRS((S16)0xACFC),    // LP3_COEFB16_09
    ENDIAN_RVRS((S16)0xF4CE),    // LP3_COEFB16_10
    ENDIAN_RVRS((S16)0x1D43),    // LP3_COEFB16_11
    ENDIAN_RVRS((S16)0x4A0F),    // LP3_COEFB16_12
    ENDIAN_RVRS((S16)0x8D79),    // LP3_COEFB16_13
    ENDIAN_RVRS((S16)0x1D43),    // LP3_COEFB24_14
    ENDIAN_RVRS((S16)0     )     // LP3_Reserved

};

/* Private functions ---------------------------------------------------------*/
ATTR_TEXT_IN_IRAM static uint32_t src_fixed_ratio_fs_converter(stream_samplerate_t fs)
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
        case FS_RATE_96K:
            return fs * 1000;

        default:
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] sample rate is not supported!", 0);
            OS_ASSERT(FALSE);
            return fs;
    }
}

/* Public functions ----------------------------------------------------------*/
/**
 * @brief This function is used to get the src port.
 *        If the owner does not have a src port, it will malloc a port for this owner.
 *        If the owner have a src port, it will return the port directly.
 *        If the owner is NULL, it will return the first unused port.
 *
 * @param owner is who want to get or query a src port or NULL.
 * @return src_fixed_ratio_port_t* is the result.
 */
src_fixed_ratio_port_t* stream_function_src_fixed_ratio_get_port(void* owner)
{
    int32_t i;
    uint32_t saved_mask;
    src_fixed_ratio_port_t* port = NULL;

    /* Find out a port for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = SRC_FIXED_RATIO_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if there is unused port */
        if (src_fixed_ratio_port[i].owner == NULL)
        {
            port = &src_fixed_ratio_port[i];
            break;
        }

        /* Check if this owner has already owned a sw src */
        if (src_fixed_ratio_port[i].owner == owner)
        {
            port = &src_fixed_ratio_port[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    DSP_MW_LOG_I("[SRC_FIXED_RATIO] get port%d, owner:0x%x", 2, i, owner);

    if (port == NULL)
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] Port not enough!", 0);
        return port;
    }

    port->owner = owner;
    return port;
}

/**
 * @brief This function is used to get the src port.
 *        If the owner does not have a src port, it will malloc a port for this owner.
 *        If the owner have a src port, it will return the port directly.
 *        If the owner is NULL, it will return the first unused port.
 *
 * @param owner is who want to get or query a src port or NULL.
 * @return src_fixed_ratio_port_t* is the result.
 */
src_fixed_ratio_port_t* stream_function_src_fixed_ratio_get_2nd_port(void* owner)
{
    int32_t i;
    int32_t j = 0;
    uint32_t saved_mask;
    src_fixed_ratio_port_t* port = NULL;

    /* Find out a port for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = SRC_FIXED_RATIO_PORT_MAX-1; i >= 0; i--)
    {
        if (src_fixed_ratio_port[i].owner == owner)
        {
            j++;
        }
        if ((src_fixed_ratio_port[i].owner == NULL) && (j == 1))
        {
            port = &src_fixed_ratio_port[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (port == NULL)
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] 2nd Port not available!", 0);
        return port;
    }
    port->owner = owner;
    DSP_MW_LOG_I("[SRC_FIXED_RATIO] get 2nd port%d, owner:0x%x", 2, i, owner);
    return port;
}

/**
 * @brief This function is used to get the src port.
 *        If the owner does not have a src port, it will malloc a port for this owner.
 *        If the owner have a src port, it will return the port directly.
 *        If the owner is NULL, it will return the first unused port.
 *
 * @param owner is who want to get or query a src port or NULL.
 * @return src_fixed_ratio_port_t* is the result.
 */
 
src_fixed_ratio_port_t* stream_function_src_fixed_ratio_get_3rd_port(void* owner)
{
    int32_t i;
    int32_t j = 0;
    uint32_t saved_mask;
    src_fixed_ratio_port_t* port = NULL;

    /* Find out a port for this owner */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    for (i = SRC_FIXED_RATIO_PORT_MAX-1; i >= 0; i--)
    {
        if (src_fixed_ratio_port[i].owner == owner)
        {
            j++;
        }
        if ((src_fixed_ratio_port[i].owner == NULL) && (j == 2))
        {
            port = &src_fixed_ratio_port[i];
            break;
        }
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (port == NULL)
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] 3rd Port not available!", 0);
        return port;
    }
    port->owner = owner;
    DSP_MW_LOG_I("[SRC_FIXED_RATIO] get 3rd port%d, owner:0x%x", 2, i, owner);
    return port;
}


/**
 * @brief This function is used to configure the src port.
 *
 * @param port is which port needs to be configured.
 * @param config is pointer to the configuration settings.
 */
void stream_function_src_fixed_ratio_init(src_fixed_ratio_port_t* port, src_fixed_ratio_config_t* config)
{
    int32_t is_upsample, factor;
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] Port is NULL!", 0);
        OS_ASSERT(FALSE);
    }
    /* check sampling rate */
    if (config->in_sampling_rate > config->out_sampling_rate) {
        is_upsample = 0;
        if (config->in_sampling_rate % config->out_sampling_rate != 0) {
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] sampling rate incorrect", 0);
            OS_ASSERT(FALSE);
        } else {
            factor = config->in_sampling_rate / config->out_sampling_rate;
            if ((factor < 2) || (factor > 3)) {
                DSP_MW_LOG_E("[SRC_FIXED_RATIO] sampling rate incorrect", 0);
                OS_ASSERT(FALSE);
            }
        }
    } else if (config->in_sampling_rate < config->out_sampling_rate) {
        is_upsample = 1;
        if (config->out_sampling_rate % config->in_sampling_rate != 0) {
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] sampling rate incorrect", 0);
            OS_ASSERT(FALSE);
        }  else {
            factor = config->out_sampling_rate / config->in_sampling_rate;
            if ((factor < 2) || (factor > 3)) {
                DSP_MW_LOG_E("[SRC_FIXED_RATIO] sampling rate incorrect", 0);
                OS_ASSERT(FALSE);
            }
        }
    } else {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] sampling rate incorrect", 0);
#ifndef AIR_BLE_SWB_ENABLE
        OS_ASSERT(FALSE);
#endif
    }
    /* config */
    port->status = SRC_FIXED_RATIO_PORT_STAT_INIT;
    port->channel_number = config->channel_number;
    port->in_sampling_rate = config->in_sampling_rate;
    port->out_sampling_rate = config->out_sampling_rate;
    port->resolution = config->resolution;
    port->is_upsample = is_upsample;
    port->factor = factor;
    port->multi_cvt_mode = SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE;
    port->cvt_num = 1;
    if ((config->multi_cvt_mode > SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_SINGLE)
        && (config->multi_cvt_mode < SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_MAX)
        && (config->cvt_num <= SRC_FIXED_RATIO_CVT_MAX))
    {
        port->multi_cvt_mode = config->multi_cvt_mode;
        port->cvt_num = config->cvt_num;
    }

    DSP_MW_LOG_I("[SRC_FIXED_RATIO] INIT, status:%d, ch:%d, in_sampling_rate:%d, out_sampling_rate:%d, is_upsample:%d, factor:%d, multi_cvt_mode:%d, cvt_num:%d", 8, port->status, port->channel_number, port->in_sampling_rate, port->out_sampling_rate, port->is_upsample, port->factor, port->multi_cvt_mode, port->cvt_num);
}

/**
 * @brief This function is used to deinit the src port.
 *
 * @param port is which port needs to be freed.
 */
void stream_function_src_fixed_ratio_deinit(src_fixed_ratio_port_t* port)
{
    int i;
    /* check port */
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] Port is NULL!", 0);
        OS_ASSERT(FALSE);
    }
    DSP_MW_LOG_I("[SRC_FIXED_RATIO]DEINIT, owner:0x%x", 1, port->owner);
    port->status = SRC_FIXED_RATIO_PORT_STAT_DEINIT;
    port->owner = NULL;
    port->channel_number = 0;
    port->in_sampling_rate = 0;
    port->out_sampling_rate = 0;
    port->resolution = 0;
    port->is_upsample = 0;
    port->factor = 0;
    port->multi_cvt_mode = 0;
    port->cvt_num = 1;
    for (i = SMP_MAX_CH_NUM-1; i>=0; i--) {
        if (port->smp_instance_ptr[i]) {
            //free memory
            preloader_pisplit_free_memory(port->smp_instance_ptr[i]);
            port->smp_instance_ptr[i] = NULL;
        }
    }
    if (port->tmp_buff) {
        preloader_pisplit_free_memory(port->tmp_buff);
        port->tmp_buff = NULL;
        port->tmp_buff_size = 0;
    }
}


/**
 * @brief This function is used to initialize the src run-time environment.
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
ATTR_TEXT_IN_IRAM bool stream_function_src_fixed_ratio_initialize(void *para)
{
    int32_t i, j, k;
    src_fixed_ratio_port_t *port = NULL;
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    uint32_t channel_number = stream_function_get_channel_number(para);
    uint32_t saved_mask;
    uint32_t instance_size = sizeof(SMP_INSTANCE_t);

    /* Find out the port of this stream */
    for (i = SRC_FIXED_RATIO_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if this source or sink has already owned a sw src */
        if ((src_fixed_ratio_port[i].owner == stream_ptr->source) ||
            (src_fixed_ratio_port[i].owner == stream_ptr->sink))
        {
            port = &src_fixed_ratio_port[i];
            break;
        }
    }
    if (port == NULL)
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    } else {
        DSP_MW_LOG_I("[SRC_FIXED_RATIO] Port %d found!", 1, i);
    }

    /* status check */
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (port->status == SRC_FIXED_RATIO_PORT_STAT_INIT)
    {
        port->status = SRC_FIXED_RATIO_PORT_STAT_RUNNING;
    }
    else if (port->status == SRC_FIXED_RATIO_PORT_STAT_RUNNING)
    {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return FALSE;
    }
    else
    {
        DSP_MW_LOG_I("[SRC_FIXED_RATIO] error status:%d", 1, port->status);
        assert(0);
        return TRUE;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    if (port->channel_number != channel_number)
    {
        if(port->channel_number == SRC_FIXED_RATIO_CH_NUM_UPDATE_LATER) {
            DSP_MW_LOG_W("[SRC_FIXED_RATIO] ch number is 0, update to the proper channel number", 0);
            port->channel_number = channel_number;
        } else if (port->channel_number == SRC_FIXED_RATIO_CH_NUM_EXCLUDE_REF) {
            DSP_MW_LOG_W("[SRC_FIXED_RATIO] ch number exclude REF", 0);
            port->channel_number = channel_number-1;
//        } else {
//                DSP_MW_LOG_E("[SRC_FIXED_RATIO] ch number is not right! %d != %d", 2, port->channel_number, channel_number);
//                OS_ASSERT(FALSE);
//                return TRUE;
        }
    }

    /*smp instance init*/
    for (k = port->channel_number-1; k>=0; k--) {
        if (port->smp_instance_ptr[k]) {
            preloader_pisplit_free_memory(port->smp_instance_ptr[k]);
        }
        port->smp_instance_ptr[k] = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, instance_size);
        if (port->smp_instance_ptr[k] == NULL) {
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] malloc fail!", 0);
            assert(0);
        }
        updn_sampling_Init((SMP_INSTANCE_t*)port->smp_instance_ptr[k], &dsp_sampler_coef);
    }

    if (port->cvt_num > 1) // initialize other ports of same owner
    {
        src_fixed_ratio_port_t *other_port = NULL;
        for (j = i-1; j >= 0; j--)
        {
            /* Check if this source or sink has already owned a sw src */
            if ((src_fixed_ratio_port[j].owner == src_fixed_ratio_port[i].owner))
            {
                other_port = &src_fixed_ratio_port[j];
                DSP_MW_LOG_I("[SRC_FIXED_RATIO] other Port %d found!", 1, j);

                /* status check */
                hal_nvic_save_and_set_interrupt_mask(&saved_mask);
                if (other_port->status == SRC_FIXED_RATIO_PORT_STAT_INIT)
                {
                    other_port->status = SRC_FIXED_RATIO_PORT_STAT_RUNNING;
                }
                else if (other_port->status == SRC_FIXED_RATIO_PORT_STAT_RUNNING)
                {

                }
                else
                {
                    DSP_MW_LOG_I("[SRC_FIXED_RATIO] error status:%d", 1, other_port->status);
                    assert(0);
                    return TRUE;
                }
                hal_nvic_restore_interrupt_mask(saved_mask);

                if (other_port->channel_number != channel_number) {
                    if((other_port->channel_number == SRC_FIXED_RATIO_CH_NUM_UPDATE_LATER) || (other_port->multi_cvt_mode == SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_CONSECUTIVE)) {
                        other_port->channel_number = channel_number;
                    } else if (other_port->channel_number == SRC_FIXED_RATIO_CH_NUM_EXCLUDE_REF) {
                        other_port->channel_number = channel_number-1;
                    }
                }

                /*smp instance init*/
                for (k = other_port->channel_number-1; k>=0; k--) {
                    if (other_port->smp_instance_ptr[k]) {
                        preloader_pisplit_free_memory(other_port->smp_instance_ptr[k]);
                    }
                    other_port->smp_instance_ptr[k] = preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, instance_size);
                    if (other_port->smp_instance_ptr[k] == NULL) {
                        DSP_MW_LOG_E("[SRC_FIXED_RATIO] malloc fail!", 0);
                        assert(0);
                    }
                    updn_sampling_Init((SMP_INSTANCE_t*)other_port->smp_instance_ptr[k], &dsp_sampler_coef);
                }
            }
        }

        if (other_port == NULL) {
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] other Ports are not found!", 0);
            assert(0);
        }
    }
    DSP_MW_LOG_I("[SRC_FIXED_RATIO] initialize, version:%d", 1, get_sampler_by_n_version());
    return 0;
}

/**
 * @brief This function is used to process the src in run-time
 *
 * @param para is the input parameters.
 * @return true means there is a error.
 * @return false means there is no error.
 */
ATTR_TEXT_IN_IRAM bool stream_function_src_fixed_ratio_process(void *para)
{
    src_fixed_ratio_port_t *port[SRC_FIXED_RATIO_CVT_MAX];
    DSP_STREAMING_PARA_PTR stream_ptr = DSP_STREAMING_GET_FROM_PRAR(para);
    int32_t i, j,res;
    int m, n = 0;
    uint32_t in_frame_size = stream_function_get_output_size(para);
    uint8_t in_sampling_rate = stream_function_get_samplingrate(para);
    uint32_t process_buff_size = in_frame_size * SRC_FIXED_RATIO_MAX;
    uint8_t* in_data_buffer;
    int16_t out_frame_cnt;
    uint32_t sample_size;

    #if SRC_FIXED_RATIO_DEBUG_LOG
    uint32_t channel_number = stream_function_get_channel_number(para);
    DSP_MW_LOG_I("[SRC_FIXED_RATIO][START]in_frame_size:%d, in_sampling_rate:%d, channel_number:%d, tmp_buff_size:%d", 4, in_frame_size, in_sampling_rate, channel_number, process_buff_size);
    uint32_t current_timestamp = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
    DSP_MW_LOG_I("[SRC_FIXED_RATIO][start]: %d, %u", 2, in_frame_size, current_timestamp);
    #endif /* SW_SRC_DEBUG_LOG */
    port[0] = NULL;
    port[1] = NULL;
    port[2] = NULL;

    /* Find out the port of this stream */
    for (i = SRC_FIXED_RATIO_PORT_MAX-1; i >= 0; i--)
    {
        /* Check if this source or sink has already owned a sw src */
        if ((src_fixed_ratio_port[i].owner == stream_ptr->source) ||
            (src_fixed_ratio_port[i].owner == stream_ptr->sink))
        {
            port[0] = &src_fixed_ratio_port[i];
            #if SRC_FIXED_RATIO_DEBUG_LOG
            DSP_MW_LOG_I("[SRC_FIXED_RATIO]port %d is found, owner:0x%x", 2, i, src_fixed_ratio_port[i].owner);
            #endif
            break;
        }
    }
    if (port[0] == NULL)
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] Port is not found!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    if (!in_frame_size)
    {
        // DSP_MW_LOG_I("[SW_SRC] Input length is 0!", 0);
        return FALSE;
    }

//    if (port[0]->channel_number != channel_number)
//    {
//        DSP_MW_LOG_E("[SRC_FIXED_RATIO] ch number is not right!", 0);
//        OS_ASSERT(FALSE);
//        return TRUE;
//    }

    if ( (port[0]->resolution != RESOLUTION_16BIT) && (port[0]->resolution != RESOLUTION_32BIT))
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] resolution is not right!", 0);
        OS_ASSERT(FALSE);
        return TRUE;
    }

    /* check if the in_sampling_rate is matched */
    if ((port[0]->in_sampling_rate != src_fixed_ratio_fs_converter(in_sampling_rate)) && (port[0]->cvt_num <= 1))
    {
        DSP_MW_LOG_E("[SRC_FIXED_RATIO] input sampling rate is not right, %u, %u!", 2, port[0]->in_sampling_rate, src_fixed_ratio_fs_converter(in_sampling_rate));
        OS_ASSERT(FALSE);
        return TRUE;
    }

    /* check if in_sampling_rate and out_sampling_rate are same */
    if (port[0]->in_sampling_rate == port[0]->out_sampling_rate)
    {
        return FALSE;
    }

    if (port[0]->tmp_buff == NULL) {
        port[0]->tmp_buff = (U8*)preloader_pisplit_malloc_memory(PRELOADER_D_HIGH_PERFORMANCE, process_buff_size);
        if (port[0]->tmp_buff == NULL) {
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] tmp buff malloc FAIL!", 0);
            OS_ASSERT(FALSE);
        }
        port[0]->tmp_buff_size = process_buff_size;

    }
    in_data_buffer = port[0]->tmp_buff;
    //check other port
    for (m = i-1; m >= 0; m--)
    {
        /* Check if this source or sink  own other sw src */
        if (src_fixed_ratio_port[m].owner == src_fixed_ratio_port[i].owner)
        {
            n++;
            port[n] = &src_fixed_ratio_port[m];
            if (port[n-1]->out_sampling_rate != port[n]->in_sampling_rate) {
                DSP_MW_LOG_E("[SRC_FIXED_RATIO] converter in/out sampling rate are not match! %d != %d", 2, port[n-1]->out_sampling_rate, port[n]->in_sampling_rate);
                assert(0);
            }
            #if SRC_FIXED_RATIO_DEBUG_LOG
            DSP_MW_LOG_I("[SRC_FIXED_RATIO]port %d is found, owner:0x%x", 2, m, src_fixed_ratio_port[m].owner);
            #endif

        }
    }

    if ((port[0]->cvt_num <= 1) || (port[0]->multi_cvt_mode == SRC_FIXED_RATIO_PORT_MUTI_CVT_MODE_ALTERNATE)) {
        int32_t port_select = -1;
        for (m = 0; m < port[0]->cvt_num; m++) {
            if (port[m]->in_sampling_rate == src_fixed_ratio_fs_converter(in_sampling_rate)) {
                port_select = m;
                break;
            }
        }

        if (port_select < 0) {
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] no port can use", 0);
            assert(0);
        }
        //DSP_MW_LOG_I("[SRC_FIXED_RATIO]port_select %d, m %d %d %d", 4, port_select, m, 59+2*m, 60+2*m);

        sample_size = (port[port_select]->resolution == RESOLUTION_16BIT) ? sizeof(int16_t) : sizeof(int32_t);

        #if SRC_FIXED_RATIO_DEBUG_DUMP
        LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, 1), in_frame_size, 59+2*m);
        #endif

        for(j=0; j<(int32_t)port[port_select]->channel_number; j++) {
            //memset(in_data_buffer, 0, port[0]->tmp_buff_size);
            memcpy(in_data_buffer, stream_function_get_inout_buffer(para, j+1), in_frame_size);

            if (port[port_select]->resolution == RESOLUTION_16BIT) {
                out_frame_cnt = updn_sampling_16bit_Proc((SMP_INSTANCE_t*)port[port_select]->smp_instance_ptr[j], port[port_select]->is_upsample, port[port_select]->factor, (S16*)in_data_buffer, (S16*)stream_function_get_inout_buffer(para, j+1), in_frame_size/sample_size);
            } else {
                out_frame_cnt = updn_sampling_32bit_Proc((SMP_INSTANCE_t*)port[port_select]->smp_instance_ptr[j], port[port_select]->is_upsample, port[port_select]->factor, (S32*)in_data_buffer, (S32*)stream_function_get_inout_buffer(para, j+1), in_frame_size/sample_size);
            }
        }
        //DSP_MW_LOG_I("[SRC_FIXED_RATIO]out_frame_cnt %d, sample_size %d", 2, out_frame_cnt, sample_size);

        res = stream_function_modify_output_size(para, out_frame_cnt*sample_size);
        ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate = port[port_select]->out_sampling_rate/1000;

        #if SRC_FIXED_RATIO_DEBUG_DUMP
        LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, 1), out_frame_cnt*sample_size, 60+2*m);
        #endif

        #if SRC_FIXED_RATIO_DEBUG_LOG
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
        DSP_MW_LOG_I("[SRC_FIXED_RATIO][finish]:%u, %u, %u", 3,
                    ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate,
                    out_frame_cnt*sample_size,
                    current_timestamp);
        #endif /* SW_SRC_DEBUG_LOG */

    } else { // consecutive converters

        if (port[1] == NULL) {
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] 2nd Port is not found!", 0);
            assert(0);
        }
        if (port[0]->cvt_num != (n+1)) {
            DSP_MW_LOG_E("[SRC_FIXED_RATIO] converter number is not match! %d != %d", 2, port[0]->cvt_num, (n+1));
            assert(0);
        }

        sample_size = (port[0]->resolution == RESOLUTION_16BIT) ? sizeof(int16_t) : sizeof(int32_t);

        for (m = 0; m < port[0]->cvt_num; m++) {
//            LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, 2), in_frame_size, 57+m);
            for(j=0; j<(int32_t)port[m]->channel_number; j++) {
                memset(in_data_buffer, 0, port[0]->tmp_buff_size);
                memcpy(in_data_buffer, stream_function_get_inout_buffer(para, j+1), in_frame_size);

                if (port[0]->resolution == RESOLUTION_16BIT) {
                    out_frame_cnt = updn_sampling_16bit_Proc((SMP_INSTANCE_t*)port[m]->smp_instance_ptr[j], port[m]->is_upsample, port[m]->factor, (S16*)in_data_buffer, (S16*)stream_function_get_inout_buffer(para, j+1), in_frame_size/sample_size);
                } else {
                    out_frame_cnt = updn_sampling_32bit_Proc((SMP_INSTANCE_t*)port[m]->smp_instance_ptr[j], port[m]->is_upsample, port[m]->factor, (S32*)in_data_buffer, (S32*)stream_function_get_inout_buffer(para, j+1), in_frame_size/sample_size);
                }
            }
            in_frame_size = out_frame_cnt * sample_size;
            #if SRC_FIXED_RATIO_DEBUG_LOG
            DSP_MW_LOG_I("[SRC_FIXED_RATIO][cvt%d]:%u, %u", 3,
                            m,
                            port[m]->out_sampling_rate,
                            in_frame_size);
            #endif
        }
//        LOG_AUDIO_DUMP((U8*)stream_function_get_inout_buffer(para, 2), in_frame_size, 60);

        res = stream_function_modify_output_size(para, in_frame_size);
        ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate = port[n]->out_sampling_rate/1000;

        #if SRC_FIXED_RATIO_DEBUG_LOG
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &current_timestamp);
        DSP_MW_LOG_I("[SRC_FIXED_RATIO][finish]:%u, %u, %u", 3,
                    ((DSP_ENTRY_PARA_PTR)para)->codec_out_sampling_rate,
                    in_frame_size,
                    current_timestamp);
        #endif /* SW_SRC_DEBUG_LOG */
    }

    return 0;
}
