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
#ifdef MTK_SENSOR_SOURCE_ENABLE
 
#include "types.h"
#include "source_inter.h"
#include "dtm.h"
#include "sink_inter.h"

#include "transform_inter.h"
#include "dsp_callback.h"
#include "stream_audio_driver.h"

#include "bsp_multi_axis_sensor.h"
#include "bsp_multi_axis_sensor_config.h"
#include "gsensor_processing.h"

//source functions
#ifdef AXIS_SENSOR_DEBUG_ENABLE
volatile static bool gsensor_sample_period_debug = false;
#endif

uint32_t gsensor_timer_handler = 0;

U32 SourceSize_GSENSOR(SOURCE source)
{
    U32 source_size = 0;

    source = source;

    source_size = bsp_multi_axis_get_frame_cnt(BMA456);
    return source_size;
}

BOOL SourceConfigure_GSENSOR(SOURCE source, stream_config_type type, U32 value)
{
    source = source;
    type = type;
    value = value;

    return true;
}

U8*  SourceMap_GSENSOR(SOURCE source)
{
    source = source;

    return NULL;
}

VOID SourceDrop_GSENSOR(SOURCE source, U32 amount)
{
    source = source;
    amount = amount;
}

BOOL SourceClose_GSENSOR(SOURCE source)
{
    source = source;

    if (BSP_MULTI_AXIS_OK != bsp_multi_axis_disable(BMA456)) {
        DSP_MW_LOG_E("[G-sensor] bsp_multi_axis_disable error!",0);
        return false;
    }

    if (BSP_MULTI_AXIS_OK != bsp_multi_axis_deinit(BMA456)) {
        DSP_MW_LOG_E("[G-sensor] bsp_multi_axis_deinit error!",0);
        return false;
    }

    return true;
}

BOOL SourceReadBuf_GSENSOR(SOURCE source, U8 *dst_addr, U32 length)
{
    uint16_t len = 0;
    bsp_multi_axis_data_t *data = (bsp_multi_axis_data_t *)dst_addr;

    source = source;

#ifdef AXIS_SENSOR_DEBUG_ENABLE
    static uint32_t start_cnt = 0,end_cnt = 0,duration_cnt = 0;

    if (gsensor_sample_period_debug == false) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&start_cnt);
        end_cnt = start_cnt;
        gsensor_sample_period_debug = true;
    }else {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M,&end_cnt);
        hal_gpt_get_duration_count(start_cnt,end_cnt,&duration_cnt);
        DSP_MW_LOG_I("[G-sensor] sample period = %d us",1,duration_cnt);
        start_cnt = end_cnt;
    }
#endif
    len = bsp_multi_axis_get_data(BMA456,data,length);
    return true;
}

//sink functions
void SourceInit_GSENSOR(SOURCE source)
{
    source->type = SOURCE_TYPE_GSENSOR;
    /* interface init */
    source->sif.SourceSize        = SourceSize_GSENSOR;
    source->sif.SourceConfigure   = SourceConfigure_GSENSOR;
    source->sif.SourceDrop        = SourceDrop_GSENSOR;
    source->sif.SourceClose       = SourceClose_GSENSOR;
    source->sif.SourceReadBuf     = SourceReadBuf_GSENSOR;

    bsp_multi_axis_config_t acc_cfg = {BMA4_ACCEL_RANGE_4G,
                                    BSP_MULTI_AXIS_ACCEL_NORMAL_AVG4,
                                    BSP_MULTI_AXIS_OUTPUT_DATA_RATE_1600HZ};
    if (BSP_MULTI_AXIS_OK != bsp_multi_axis_initialize(BMA456,&acc_cfg)) {
        DSP_MW_LOG_E("[G-sensor] bsp_multi_axis_initialize error!",0);
        return;
    }

    if (BSP_MULTI_AXIS_OK != bsp_multi_axis_enable(BMA456)) {
        DSP_MW_LOG_E("[G-sensor] bsp_multi_axis_enable error!",0);
        return;
    }
}

#endif


