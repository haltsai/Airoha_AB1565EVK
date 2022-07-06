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

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "source.h"
#include "stream_audio_setting.h"
#include "hal_nvic.h"
#include "FreeRTOS.h"
#include "stream_audio_driver.h"
#include "dsp_callback.h"
#include "dsp_audio_ctrl.h"
#include "hal_audio_afe_control.h"
#include "hal_audio_afe_define.h"
#include "hal_audio_afe_clock.h"
#include "audio_afe_common.h"

extern afe_stream_channel_t connect_type[2][2];
extern afe_t afe;
extern audio_source_pcm_ops_t afe_platform_awb_ops;
#define AUDIO_AFE_SOURCE_ASRC_BUFFER_SIZE 4096

static void ul1_global_var_init(SOURCE source)
{
    #if 0
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;
    memset(afe_block, 0, sizeof(afe_block_t));
    #else
    UNUSED(source);
    #endif
}

static int32_t pcm_ul1_probe(SOURCE source)
{
    ul1_global_var_init(source);
    return 0;
}

static int32_t pcm_ul1_start(SOURCE source)
{
    AUDIO_PARAMETER *runtime = &source->param.audio;
    audio_digital_block_t memory_block;
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;
    hal_audio_memory_parameter_t *mem_handle = &source->param.audio.mem_handle;//modify for ab1568
    hal_audio_memory_selection_t memory_select, fined_memory =0, memory_search;//modify for ab1568
    uint32_t channel_index = 0;
    uint32_t update_offset = 0;

    UNUSED(channel_index);
    DSP_MW_LOG_I("DSP audio pcm_ul1_start\r\n", 0);
    hal_audio_afe_clock_on();

    mem_handle->audio_path_rate = runtime->rate;
    mem_handle->buffer_addr = afe_block->phys_buffer_addr;
    mem_handle->buffer_length = runtime->buffer_size;
    if(source->param.audio.mem_handle.pure_agent_with_src){
        mem_handle->buffer_length = AUDIO_AFE_SOURCE_ASRC_BUFFER_SIZE;
    }
    DSP_MW_LOG_I("DSP UL start source type:%d, memory select:0x%x\r\n", 2, source->type, mem_handle->memory_select);//modify for ab1568
    memory_block =  hal_audio_afe_get_memory_digital_block (source->param.audio.memory, false);
    /* set irq start */
    if(!source->param.audio.AfeBlkControl.u4awsflag) {
        //afe_enable_audio_irq(afe_irq_request_number(memory_block), runtime->rate, runtime->count);
        mem_handle->sync_status = HAL_AUDIO_MEMORY_SYNC_NONE;
    }else{
        mem_handle->sync_status = HAL_AUDIO_MEMORY_SYNC_SW_TRIGGER;
    }

    /* Set Memory */
    memory_select = mem_handle->memory_select;
    for (memory_search = HAL_AUDIO_MEMORY_UL_VUL1 ; memory_search<=HAL_AUDIO_MEMORY_UL_AWB2 ; memory_search<<=1)
    {
        if(memory_search & memory_select) {
            mem_handle->buffer_addr = 0; //Clear buffer ptr to allocate new SRAM
            mem_handle->memory_select = memory_search;
            hal_audio_set_memory(mem_handle, HAL_AUDIO_CONTROL_MEMORY_INTERFACE, HAL_AUDIO_CONTROL_ON);
            if(source->param.audio.mem_handle.pure_agent_with_src){
                source->streamBuffer.BufferInfo.startaddr[channel_index++] = (void *)mem_handle->src_buffer_addr;
            }else{
                source->streamBuffer.BufferInfo.startaddr[channel_index++] = (void *)mem_handle->buffer_addr;
            }
            DSP_MW_LOG_I("DSP UL start mem_search:0x%x, mem adr:0x%x\r\n", 2,memory_search, mem_handle->buffer_addr);
            if (!fined_memory) {
                fined_memory = memory_search;
            }
        }
    }
    mem_handle->memory_select = memory_select;

#define ReadREG(_addr)          (*(volatile uint32_t *)(_addr))
    if ((runtime->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) && (runtime->memory == HAL_AUDIO_MEM6)) {
        //I2S Slave Infra mode
        hal_audio_agent_t agent = HAL_AUDIO_AGENT_ERROR;
        if(runtime->audio_interface == HAL_AUDIO_INTERFACE_1) {
            agent = HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE;
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
            agent = HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE;
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
            agent = HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE;
        }
        source->streamBuffer.BufferInfo.startaddr[0] = (void *)hal_memory_allocate_sram(agent, mem_handle->buffer_length);
        afe_block->phys_buffer_addr = (uint32_t)source->streamBuffer.BufferInfo.startaddr[0];

        hal_audio_slave_vdma_parameter_t vdma_setting;
        vdma_setting.base_address = afe_block->phys_buffer_addr;
        vdma_setting.size = mem_handle->buffer_length>>2;
        vdma_setting.threshold = mem_handle->buffer_length>>2;
        vdma_setting.audio_interface = runtime->audio_interface;
#ifdef MTK_TDM_ENABLE
        vdma_setting.tdm_channel = HAL_AUDIO_I2S_TDM_DISABLE;
#endif
        vdma_setting.is_ul_mode = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&vdma_setting, HAL_AUDIO_SET_SLAVE_VDMA);
        if (runtime->audio_interface == HAL_AUDIO_INTERFACE_1) {
            Source_Audio_BufferInfo_Rst(source, ReadREG(0xC9000830)-ReadREG(0xC900082c));
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
            Source_Audio_BufferInfo_Rst(source, ReadREG(0xC9000230)-ReadREG(0xC900022c));
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
            Source_Audio_BufferInfo_Rst(source, ReadREG(0xC9000a30)-ReadREG(0xC9000a2c));
        }
    } else if ((runtime->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) && (runtime->memory == HAL_AUDIO_MEM7)) {
        //I2S Slave Tdm mode
#ifdef MTK_TDM_ENABLE
        hal_audio_agent_t agent;
        if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
            agent = HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE;
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
            agent = HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE;
        }
        source->streamBuffer.BufferInfo.startaddr[0] = (void *)hal_memory_allocate_sram(agent, mem_handle->buffer_length + mem_handle->buffer_length*runtime->device_handle.i2s_slave.tdm_channel);
        afe_block->phys_buffer_addr = (uint32_t)source->streamBuffer.BufferInfo.startaddr[0];
        for (channel_index=1; channel_index <= runtime->device_handle.i2s_slave.tdm_channel; channel_index++)
        {
            source->streamBuffer.BufferInfo.startaddr[channel_index] = source->streamBuffer.BufferInfo.startaddr[channel_index-1] + mem_handle->buffer_length;
        }

        hal_audio_slave_vdma_parameter_t vdma_setting;
        vdma_setting.base_address = afe_block->phys_buffer_addr;
        vdma_setting.size = mem_handle->buffer_length>>2;
        vdma_setting.threshold = mem_handle->buffer_length>>2;
        vdma_setting.audio_interface = runtime->audio_interface;
        vdma_setting.tdm_channel = runtime->device_handle.i2s_slave.tdm_channel;
        vdma_setting.is_ul_mode = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&vdma_setting, HAL_AUDIO_SET_SLAVE_VDMA);
#endif
    } else {
        /* Update initial offset */
        hal_audio_current_offset_parameter_t get_current_offset;
        get_current_offset.memory_select = fined_memory;
        get_current_offset.pure_agent_with_src = false;
        hal_audio_get_value((hal_audio_get_value_parameter_t *)&get_current_offset, HAL_AUDIO_GET_MEMORY_INPUT_CURRENT_OFFSET);
        if ((get_current_offset.offset!=0) && (get_current_offset.base_address!=0) ) {
            update_offset = get_current_offset.offset - get_current_offset.base_address;
        }
        if (mem_handle->sync_status == HAL_AUDIO_MEMORY_SYNC_SW_TRIGGER) {
            update_offset = 0;
        }
        Source_Audio_BufferInfo_Rst(source, update_offset);
    }


#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    if ((source->type >= SOURCE_TYPE_SUBAUDIO_MIN) && (source->type <= SOURCE_TYPE_SUBAUDIO_MAX)) {
        hal_audio_memory_irq_enable_parameter_t irq_enable;
        irq_enable.memory_select    = HAL_AUDIO_MEMORY_UL_MASK;
        irq_enable.irq_counter      = mem_handle->irq_counter;
        irq_enable.rate             = mem_handle->audio_path_rate;
        irq_enable.enable           = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&irq_enable, HAL_AUDIO_SET_MEMORY_IRQ_ENABLE);
    }
#endif
    source->param.audio.is_memory_start = true;
    DSP_MW_LOG_I("DSP UL start is_memory_start:0x%x\r\n", 1,source->param.audio.is_memory_start);
    return 0;
}

static int32_t pcm_ul1_stop(SOURCE source)
{
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;
    UNUSED(afe_block);

    audio_digital_block_t memory_block;
    AUDIO_PARAMETER *runtime = &source->param.audio;
    UNUSED(runtime);
    //modify for ab1568 new driver
    hal_audio_memory_parameter_t *mem_handle = &source->param.audio.mem_handle;
    hal_audio_memory_selection_t memory_select, memory_search;
    UNUSED(memory_block);
    DSP_MW_LOG_I("DSP audio pcm_ul1_stop\r\n", 0);

    DSP_MW_LOG_I("DSP UL stop source type:%d, memory select:0x%x\r\n", 2, source->type, mem_handle->memory_select);//modify for ab1568

    source->param.audio.is_memory_start = false;
#if defined(MTK_MULTI_MIC_STREAM_ENABLE) || defined(MTK_ANC_SURROUND_MONITOR_ENABLE) || defined(AIR_WIRED_AUDIO_ENABLE) || defined(AIR_ADVANCED_PASSTHROUGH_ENABLE)
    if ((source->type >= SOURCE_TYPE_SUBAUDIO_MIN) && (source->type <= SOURCE_TYPE_SUBAUDIO_MAX)) {
        hal_audio_memory_irq_enable_parameter_t irq_enable;
        irq_enable.memory_select    = HAL_AUDIO_MEMORY_UL_MASK;
        irq_enable.irq_counter      = mem_handle->irq_counter;
        irq_enable.rate             = mem_handle->audio_path_rate;
        irq_enable.enable           = false;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&irq_enable, HAL_AUDIO_SET_MEMORY_IRQ_ENABLE);
    }
#endif

    /* Set Memory */
    memory_select = mem_handle->memory_select;
    for (memory_search = HAL_AUDIO_MEMORY_UL_VUL1 ; memory_search<=HAL_AUDIO_MEMORY_UL_AWB2 ; memory_search<<=1)
    {
        if(memory_search&memory_select) {
            mem_handle->memory_select = memory_search;
            hal_audio_set_memory(mem_handle, HAL_AUDIO_CONTROL_MEMORY_INTERFACE, HAL_AUDIO_CONTROL_OFF);
        }
    }
    mem_handle->memory_select = memory_select;

#ifdef MTK_TDM_ENABLE
    if ((runtime->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) && (runtime->memory == HAL_AUDIO_MEM7)) {
        //I2S Slave Tdm mode
        hal_audio_agent_t agent;
        if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
            agent = HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE;
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
            agent = HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE;
        }

        /* VDMA deinit */
        vdma_disable_interrupt(15);//need modify
        vdma_disable_interrupt(17);
        vdma_disable_interrupt(19);
        vdma_disable_interrupt(21);
        vdma_stop(15);
        vdma_stop(17);
        vdma_stop(19);
        vdma_stop(21);
        vdma_deinit(15);
        vdma_deinit(17);
        vdma_deinit(19);
        vdma_deinit(21);
    }
#endif
    return 0;
}

static int32_t pcm_ul1_hw_params(SOURCE source)
{
    UNUSED(source);
    return 0;
}

static int32_t pcm_ul1_open(SOURCE source)
{
    uint32_t source_ch;
    int32_t ret = 0;
    AUDIO_PARAMETER *runtime = &source->param.audio;
    if (runtime->is_pcm_ul1_open) {
        DSP_MW_LOG_I("DSP audio pcm_ul1_open already\r\n", 0);
        return ret;
    }
    runtime->is_pcm_ul1_open = true;
    hal_audio_device_t device = source->param.audio.audio_device;//hal_audio_get_stream_in_device();
#ifdef ENABLE_2A2D_TEST
    hal_audio_device_t device1 = source->param.audio.audio_device1;
    hal_audio_device_t device2 = source->param.audio.audio_device2;
    hal_audio_device_t device3 = source->param.audio.audio_device3;
    hal_audio_device_t device4 = source->param.audio.audio_device4;
    hal_audio_device_t device5 = source->param.audio.audio_device5;
    hal_audio_device_t device6 = source->param.audio.audio_device6;
    hal_audio_device_t device7 = source->param.audio.audio_device7;
#endif
    hal_audio_path_parameter_t *path_handle = &source->param.audio.path_handle;//modify for ab1568
    hal_audio_device_parameter_t *device_handle = &source->param.audio.device_handle;//modify for ab1568
#ifdef ENABLE_2A2D_TEST
    hal_audio_device_parameter_t *device_handle1 = &source->param.audio.device_handle1;//modify for ab1568
    hal_audio_device_parameter_t *device_handle2 = &source->param.audio.device_handle2;//modify for ab1568
    hal_audio_device_parameter_t *device_handle3 = &source->param.audio.device_handle3;//modify for ab1568
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        hal_audio_device_parameter_t *device_handle4 = &source->param.audio.device_handle4;//modify for ab1568
        hal_audio_device_parameter_t *device_handle5 = &source->param.audio.device_handle5;//modify for ab1568
        hal_audio_device_parameter_t *device_handle6 = &source->param.audio.device_handle6;//modify for ab1568
        hal_audio_device_parameter_t *device_handle7 = &source->param.audio.device_handle7;//modify for ab1568
    #endif
#endif

    DSP_MW_LOG_I("DSP audio pcm_ul1_open\r\n", 0);

    hal_audio_afe_clock_on();
    #if 0
    TRANSFORM transform = source->transform;
    DSP_CALLBACK_PTR callback_ptr = NULL;
    audio_afe_io_block_t audio_block_in, audio_block_out;
    uint32_t stream_ch, source_ch;
    callback_ptr = DSP_Callback_Get(source, transform->sink);
    stream_ch = (callback_ptr->EntryPara.in_channel_num >2)
                  ? 1
                  : callback_ptr->EntryPara.in_channel_num - 1;
    source_ch = (runtime->channel_num > 2)
                  ? 1
                  : runtime->channel_num - 1;
    runtime->connect_channel_type = connect_type[stream_ch][source_ch];
    #else
    source_ch = (runtime->channel_num > 2)
                  ? 1
                  : runtime->channel_num - 1;
    runtime->connect_channel_type = connect_type[source_ch][source_ch];
    #endif
#if 0//modify for ab1568
    /*set interconnection*/
    DSP_MW_LOG_I("DSP audio pcm_ul1_open channel_type:%d \r\n", 1, runtime->connect_channel_type);
    hal_audio_afe_set_connection(runtime, true, true);

    #if 0
    //UpLink use software gain control
    if (runtime->hw_gain) {
        afe_set_hardware_digital_gain_mode(AFE_HW_DIGITAL_GAIN2, runtime->rate, 20);
        afe_enable_hardware_digital_gain(AFE_HW_DIGITAL_GAIN2, true);
    }
    #endif


    afe_audio_device_enable(true, device, runtime->audio_interface, runtime->format, runtime->rate, runtime->misc_parms);

#ifdef ENABLE_2A2D_TEST
    if (device1 != HAL_AUDIO_DEVICE_NONE) {
        afe_audio_device_enable(true, device1, runtime->audio_interface1, runtime->memory, runtime->format, runtime->rate, runtime->misc_parms);
    }
    if (device2 != HAL_AUDIO_DEVICE_NONE) {
        afe_audio_device_enable(true, device2, runtime->audio_interface2, runtime->memory, runtime->format, runtime->rate, runtime->misc_parms);
    }
    if (device3 != HAL_AUDIO_DEVICE_NONE) {
        afe_audio_device_enable(true, device3, runtime->audio_interface3, runtime->memory, runtime->format, runtime->rate, runtime->misc_parms);
    }
#endif
    if (source->param.audio.echo_reference == true) {
        ret = afe_platform_awb_ops.open(source);
    }
#else
    if ((runtime->audio_device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) && ((runtime->memory == HAL_AUDIO_MEM6) || (runtime->memory == HAL_AUDIO_MEM7))) {
        //I2S Slave Infra mode or Tdm mode
    } else {
        hal_audio_set_path(path_handle, HAL_AUDIO_CONTROL_ON);
    }
    hal_audio_set_device(device_handle, device, HAL_AUDIO_CONTROL_ON);
#ifdef ENABLE_2A2D_TEST
    if(device1!=NULL){
        hal_audio_set_device(device_handle1, device1, HAL_AUDIO_CONTROL_ON);
    }
    if(device2!=NULL){
        hal_audio_set_device(device_handle2, device2, HAL_AUDIO_CONTROL_ON);}
    if(device3!=NULL){
        hal_audio_set_device(device_handle3, device3, HAL_AUDIO_CONTROL_ON);}
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if(device4!=NULL){
            hal_audio_set_device(device_handle4, device4, HAL_AUDIO_CONTROL_ON);}
        if(device5!=NULL){
            hal_audio_set_device(device_handle5, device5, HAL_AUDIO_CONTROL_ON);}
        if(device6!=NULL){
            hal_audio_set_device(device_handle6, device6, HAL_AUDIO_CONTROL_ON);}
        if(device7!=NULL){
            hal_audio_set_device(device_handle7, device7, HAL_AUDIO_CONTROL_ON);}
    #endif
#endif
    #if 0
        //Sine generator for FGPA verification TEMP!!!
        hal_audio_sine_generator_parameter_t sine_generator;
        sine_generator.enable = true;

        sine_generator.rate = device_handle->common.rate;
        sine_generator.audio_control = device_handle->common.audio_device;
        sine_generator.port_parameter.device_interface = device_handle->common.device_interface;
        sine_generator.is_input_port = true;
        DSP_MW_LOG_I("audio_controol %d rate %d audio_interface %d sine_generator.rate %d\r\n",4,sine_generator.audio_control,sine_generator.rate,sine_generator.port_parameter.device_interface,sine_generator.rate);
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&sine_generator, HAL_AUDIO_SET_SINE_GENERATOR);
    #endif

    #ifdef MTK_AUDIO_LOOPBACK_TEST_ENABLE
            //Audio loopback test choose pga 0/1/2/3 mux = null or normal
            if (Audio_setting->Audio_source.Pga_mux == HAL_AUDIO_LOOPBACK_TEST_PGA_NULL) {
                DSP_MW_LOG_I("[AUDIO LOOPBACK TEST] pga mux = null", 0);
                //Pga 0/1 mux = null
                ANA_SET_REG(AUDENC_ANA_CON0,  0, AUDENC_ANA_CON0_L_PREAMP_INPUT_SEL_MASK|AUDENC_ANA_CON0_L_PREAMP_GAIN_MASK);
                ANA_SET_REG(AUDENC_ANA_CON1,  0, AUDENC_ANA_CON1_R_PREAMP_INPUT_SEL_MASK|AUDENC_ANA_CON1_R_PREAMP_GAIN_MASK);
                //Pga 2/3 mux = null
                ANA_SET_REG(AUDENC_ANA_CON9,  0, AUDENC_ANA_CON9_RG_AUD23PREAMPLINPUTSEL_MASK|AUDENC_ANA_CON9_RG_AUD23PREAMPLGAIN_MASK);
                ANA_SET_REG(AUDENC_ANA_CON10, 0, AUDENC_ANA_CON10_RG_AUD23PREAMPRINPUTSEL_MASK|AUDENC_ANA_CON10_RG_AUD23PREAMPRGAIN_MASK);
                //Pga 4/5 mux = null
                ANA_SET_REG(AUDENC_ANA_CON18, 0, AUDENC_ANA_CON18_RG_AUD45PREAMPLINPUTSEL_MASK|AUDENC_ANA_CON18_RG_AUD45PREAMPLGAIN_MASK);
                ANA_SET_REG(AUDENC_ANA_CON19, 0, AUDENC_ANA_CON19_RG_AUD45PREAMPRINPUTSEL_MASK|AUDENC_ANA_CON19_RG_AUD45PREAMPRGAIN_MASK);
            } else {
                DSP_MW_LOG_I("[AUDIO LOOPBACK TEST] pga mux = normal", 0);
            }
    #endif
#endif
    return ret;
}

static int32_t pcm_ul1_close(SOURCE source)
{
    int32_t ret = 0;
    AUDIO_PARAMETER *runtime = &source->param.audio;
    hal_audio_device_t device = source->param.audio.audio_device;
#ifdef ENABLE_2A2D_TEST
    hal_audio_device_t device1 = source->param.audio.audio_device1;
    hal_audio_device_t device2 = source->param.audio.audio_device2;
    hal_audio_device_t device3 = source->param.audio.audio_device3;
    hal_audio_device_t device4 = source->param.audio.audio_device4;
    hal_audio_device_t device5 = source->param.audio.audio_device5;
    hal_audio_device_t device6 = source->param.audio.audio_device6;
    hal_audio_device_t device7 = source->param.audio.audio_device7;
#endif
    hal_audio_path_parameter_t *path_handle = &source->param.audio.path_handle;//modify for ab1568
    hal_audio_device_parameter_t *device_handle = &source->param.audio.device_handle;//modify for ab1568
#ifdef ENABLE_2A2D_TEST
    hal_audio_device_parameter_t *device_handle1 = &source->param.audio.device_handle1;//modify for ab1568
    hal_audio_device_parameter_t *device_handle2 = &source->param.audio.device_handle2;//modify for ab1568
    hal_audio_device_parameter_t *device_handle3 = &source->param.audio.device_handle3;//modify for ab1568
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        hal_audio_device_parameter_t *device_handle4 = &source->param.audio.device_handle4;//modify for ab1568
        hal_audio_device_parameter_t *device_handle5 = &source->param.audio.device_handle5;//modify for ab1568
        hal_audio_device_parameter_t *device_handle6 = &source->param.audio.device_handle6;//modify for ab1568
        hal_audio_device_parameter_t *device_handle7 = &source->param.audio.device_handle7;//modify for ab1568
    #endif
#endif
    DSP_MW_LOG_I("DSP audio pcm_ul1_close\r\n", 0);
    runtime->is_pcm_ul1_open = false;
#if 0//modify for ab1568
    afe_audio_device_enable(false, device, source->param.audio.audio_interface, source->param.audio.format, source->param.audio.rate, source->param.audio.misc_parms);
    #if 0
    //UpLink use software gain control
    if (source->param.audio.hw_gain) {
        afe_enable_hardware_digital_gain(AFE_HW_DIGITAL_GAIN2, false);
    }
    #endif

    /*clear interconnection*/
    hal_audio_afe_set_connection(&source->param.audio, true, false);



    if (source->param.audio.echo_reference == true) {
        ret = afe_platform_awb_ops.close(source);
    }

    hal_audio_afe_clock_off();
#else

#ifdef ENABLE_2A2D_TEST
    #ifdef MTK_AUDIO_HW_IO_CONFIG_ENHANCE
        if(device7!=NULL){hal_audio_set_device(device_handle7, device7, HAL_AUDIO_CONTROL_OFF);}
        if(device6!=NULL){hal_audio_set_device(device_handle6, device6, HAL_AUDIO_CONTROL_OFF);}
        if(device5!=NULL){hal_audio_set_device(device_handle5, device5, HAL_AUDIO_CONTROL_OFF);}
        if(device4!=NULL){hal_audio_set_device(device_handle4, device4, HAL_AUDIO_CONTROL_OFF);}
    #endif
    if(device3!=NULL){hal_audio_set_device(device_handle3, device3, HAL_AUDIO_CONTROL_OFF);}
    if(device2!=NULL){hal_audio_set_device(device_handle2, device2, HAL_AUDIO_CONTROL_OFF);}
    if(device1!=NULL){hal_audio_set_device(device_handle1, device1, HAL_AUDIO_CONTROL_OFF);}
#endif
    hal_audio_set_device(device_handle, device, HAL_AUDIO_CONTROL_OFF);
#ifdef MTK_TDM_ENABLE
    if ((device == HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) && ((source->param.audio.memory == HAL_AUDIO_MEM6) || (source->param.audio.memory == HAL_AUDIO_MEM7))) {
        //I2S Slave Infra mode or Tdm mode
    } else {
    hal_audio_set_path(path_handle, HAL_AUDIO_CONTROL_OFF);
    }
#else
    hal_audio_set_path(path_handle, HAL_AUDIO_CONTROL_OFF);
#endif
#endif
    return ret;
}

static int32_t pcm_ul1_trigger(SOURCE source, int cmd)
{
    int32_t ret = 0;

    switch (cmd) {
        case AFE_PCM_TRIGGER_START:
            ret = pcm_ul1_start(source);
            if ((ret == 0)&& (source->param.audio.echo_reference == true)) {
                ret = afe_platform_awb_ops.trigger(source, cmd);
            }
            break;
        case AFE_PCM_TRIGGER_STOP:
            if (source->param.audio.echo_reference == true) {
                ret = afe_platform_awb_ops.trigger(source, cmd);
            }
            if (ret == 0) {
                ret = pcm_ul1_stop(source);
            }
            break;
        case AFE_PCM_TRIGGER_RESUME:
            ret = pcm_ul1_open(source);
            if ((ret == 0)&& (source->param.audio.echo_reference == true)) {
                ret = afe_platform_awb_ops.trigger(source, cmd);
            }
            break;
        case AFE_PCM_TRIGGER_SUSPEND:
            if (source->param.audio.echo_reference == true) {
                ret = afe_platform_awb_ops.trigger(source, cmd);
            }
            if (ret == 0) {
                ret = pcm_ul1_close(source);
            }
            break;
        default:
            ret = -1;
            break;
    }
    if (ret != 0) {
        DSP_MW_LOG_I("pcm_ul1_trigger %d, error:%d\r\n", 2, cmd, ret);
    }
    return ret;
}

ATTR_TEXT_IN_IRAM_LEVEL_2 static int32_t pcm_ul1_copy(SOURCE source, void *dst, uint32_t count)
{
    //copy the AFE src streambuffer to sink streambuffer
    if (Source_Audio_ReadAudioBuffer(source, dst , count) == false) {
        return -1;
    }
    return 0;
}

audio_source_pcm_ops_t afe_platform_ul1_ops = {
    .probe      = pcm_ul1_probe,
    .open       = pcm_ul1_open,
    .close      = pcm_ul1_close,
    .hw_params  = pcm_ul1_hw_params,
    .trigger    = pcm_ul1_trigger,
    .copy       = pcm_ul1_copy,
};
