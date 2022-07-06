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
extern ATTR_TEXT_IN_IRAM void i2s_slave_0_ul_interrupt_handler(vdma_event_t event, void  *user_data);
extern ATTR_TEXT_IN_IRAM void i2s_slave_1_ul_interrupt_handler(vdma_event_t event, void  *user_data);
extern ATTR_TEXT_IN_IRAM void i2s_slave_2_ul_interrupt_handler(vdma_event_t event, void  *user_data);

#include "hal_pdma_internal.h"
#include "memory_attribute.h"
#include "hal_gpio.h"

const vdma_channel_t g_i2s_slave_vdma_channel[] = {
    VDMA_I2S3TX, VDMA_I2S3RX,//I2S0 DMA TX(VDMA7),  I2S0 DMA RX(VDMA8)
    VDMA_I2S0TX, VDMA_I2S0RX,//I2S1 DMA TX(VDMA1),  I2S1 DMA RX(VDMA2)
    VDMA_I2S4TX, VDMA_I2S4RX,//I2S2 DMA TX(VDMA9),  I2S2 DMA RX(VDMA10)
};

static const uint32_t g_i2s_slave_infrasys_baseaddr[] = {
    AFE_I2S_SLV0_BASE,
    AFE_I2S_SLV1_BASE,
    AFE_I2S_SLV2_BASE,
};

typedef struct {
    hal_gpio_pin_t clk_pin;
    uint8_t clk_func;
    hal_gpio_pin_t ws_pin;
    uint8_t ws_func;
    hal_gpio_pin_t tx_pin;
    uint8_t tx_func;
    hal_gpio_pin_t rx_pin;
    uint8_t rx_func;
} i2s_slave_pin_cfg_t;

static const i2s_slave_pin_cfg_t g_i2s_slave_pin_cfg[] = {
    /* 0 */
    {
        HAL_GPIO_9, HAL_GPIO_9_I2S_SLV0_CK,
        HAL_GPIO_10, HAL_GPIO_10_I2S_SLV0_WS,
        HAL_GPIO_8, HAL_GPIO_8_I2S_SLV0_TX,
        HAL_GPIO_11, HAL_GPIO_11_I2S_SLV0_RX,
    },
    /* 1 */
    {
        HAL_GPIO_12, HAL_GPIO_12_I2S_SLV1_CK,
        HAL_GPIO_15, HAL_GPIO_15_I2S_SLV1_WS,
        HAL_GPIO_13, HAL_GPIO_13_I2S_SLV1_TX,
        HAL_GPIO_14, HAL_GPIO_14_I2S_SLV1_RX,
    },
    /* 2 */
    {
        HAL_GPIO_17, HAL_GPIO_17_I2S_SLV2_CK,
        HAL_GPIO_16, HAL_GPIO_16_I2S_SLV2_WS,
        HAL_GPIO_19, HAL_GPIO_19_I2S_SLV2_TX,
        HAL_GPIO_18, HAL_GPIO_18_I2S_SLV2_RX,
    },
};

static const uint32_t g_i2s_slave_pin_index[] = {0, 1, 2, 3};
static bool i2s_slave_port_set_flag[3] = {false,false,false};

void print_reg_list(void)
{
    DSP_MW_LOG_I("GPIO mode setting", 0);
    DSP_MW_LOG_I("0xA20B0074 = 0x%08x", 1, *(volatile uint32_t *)0xA20B0074);
    DSP_MW_LOG_I("0xA20B0078 = 0x%08x", 1, *(volatile uint32_t *)0xA20B0078);

    DSP_MW_LOG_I("global PDN setting", 0);
    DSP_MW_LOG_I("0xA2270300 = 0x%08x", 1, *(volatile uint32_t *)0xA2270300);
    DSP_MW_LOG_I("0xA2270330 = 0x%08x", 1, *(volatile uint32_t *)0xA2270330);

    DSP_MW_LOG_I("global vdma setting", 0);
    DSP_MW_LOG_I("0xA3010000 = 0x%08x", 1, *(volatile uint32_t *)0xA3010000);
    DSP_MW_LOG_I("0xA3010070 = 0x%08x", 1, *(volatile uint32_t *)0xA3010070);
    DSP_MW_LOG_I("0xA3010028 = 0x%08x", 1, *(volatile uint32_t *)0xA3010028);
    DSP_MW_LOG_I("0xA3010030 = 0x%08x", 1, *(volatile uint32_t *)0xA3010030);
    DSP_MW_LOG_I("0xA3010020 = 0x%08x", 1, *(volatile uint32_t *)0xA3010020);
    DSP_MW_LOG_I("0xA3010040 = 0x%08x", 1, *(volatile uint32_t *)0xA3010040);
    DSP_MW_LOG_I("0xA3010044 = 0x%08x", 1, *(volatile uint32_t *)0xA3010044);

    DSP_MW_LOG_I("I2S slave 0 TX VDMA setting", 0);
    DSP_MW_LOG_I("0xA3010110 = 0x%08x", 1, *(volatile uint32_t *)0xA3010110);
    DSP_MW_LOG_I("0xA3010114 = 0x%08x", 1, *(volatile uint32_t *)0xA3010114);
    DSP_MW_LOG_I("0xA3010118 = 0x%08x", 1, *(volatile uint32_t *)0xA3010118);
    DSP_MW_LOG_I("0xA301011C = 0x%08x", 1, *(volatile uint32_t *)0xA301011C);
    DSP_MW_LOG_I("0xA3010120 = 0x%08x", 1, *(volatile uint32_t *)0xA3010120);
    DSP_MW_LOG_I("0xA3010128 = 0x%08x", 1, *(volatile uint32_t *)0xA3010128);
    DSP_MW_LOG_I("0xA301012C = 0x%08x", 1, *(volatile uint32_t *)0xA301012C);
    DSP_MW_LOG_I("0xA3010130 = 0x%08x", 1, *(volatile uint32_t *)0xA3010130);
    DSP_MW_LOG_I("0xA3010134 = 0x%08x", 1, *(volatile uint32_t *)0xA3010134);
    DSP_MW_LOG_I("0xA3010138 = 0x%08x", 1, *(volatile uint32_t *)0xA3010138);
    DSP_MW_LOG_I("0xA301013C = 0x%08x", 1, *(volatile uint32_t *)0xA301013C);
    DSP_MW_LOG_I("0xA3010140 = 0x%08x", 1, *(volatile uint32_t *)0xA3010140);
    DSP_MW_LOG_I("0xA3010144 = 0x%08x", 1, *(volatile uint32_t *)0xA3010144);
    DSP_MW_LOG_I("0xA3010160 = 0x%08x", 1, *(volatile uint32_t *)0xA3010160);
    DSP_MW_LOG_I("0xA3010164 = 0x%08x", 1, *(volatile uint32_t *)0xA3010164);
    DSP_MW_LOG_I("0xA3010168 = 0x%08x", 1, *(volatile uint32_t *)0xA3010168);
    DSP_MW_LOG_I("0xA301016C = 0x%08x", 1, *(volatile uint32_t *)0xA301016C);

    DSP_MW_LOG_I("I2S slave 0 RX VDMA setting", 0);
    DSP_MW_LOG_I("0xA3010210 = 0x%08x", 1, *(volatile uint32_t *)0xA3010210);
    DSP_MW_LOG_I("0xA3010214 = 0x%08x", 1, *(volatile uint32_t *)0xA3010214);
    DSP_MW_LOG_I("0xA3010218 = 0x%08x", 1, *(volatile uint32_t *)0xA3010218);
    DSP_MW_LOG_I("0xA301021C = 0x%08x", 1, *(volatile uint32_t *)0xA301021C);
    DSP_MW_LOG_I("0xA3010220 = 0x%08x", 1, *(volatile uint32_t *)0xA3010220);
    DSP_MW_LOG_I("0xA3010228 = 0x%08x", 1, *(volatile uint32_t *)0xA3010228);
    DSP_MW_LOG_I("0xA301022C = 0x%08x", 1, *(volatile uint32_t *)0xA301022C);
    DSP_MW_LOG_I("0xA3010230 = 0x%08x", 1, *(volatile uint32_t *)0xA3010230);
    DSP_MW_LOG_I("0xA3010234 = 0x%08x", 1, *(volatile uint32_t *)0xA3010234);
    DSP_MW_LOG_I("0xA3010238 = 0x%08x", 1, *(volatile uint32_t *)0xA3010238);
    DSP_MW_LOG_I("0xA301023C = 0x%08x", 1, *(volatile uint32_t *)0xA301023C);
    DSP_MW_LOG_I("0xA3010240 = 0x%08x", 1, *(volatile uint32_t *)0xA3010240);
    DSP_MW_LOG_I("0xA3010244 = 0x%08x", 1, *(volatile uint32_t *)0xA3010244);
    DSP_MW_LOG_I("0xA3010260 = 0x%08x", 1, *(volatile uint32_t *)0xA3010260);
    DSP_MW_LOG_I("0xA3010264 = 0x%08x", 1, *(volatile uint32_t *)0xA3010264);
    DSP_MW_LOG_I("0xA3010268 = 0x%08x", 1, *(volatile uint32_t *)0xA3010268);
    DSP_MW_LOG_I("0xA301026C = 0x%08x", 1, *(volatile uint32_t *)0xA301026C);

    DSP_MW_LOG_I("I2S slave 0 setting", 0);
    DSP_MW_LOG_I("0xA0070000 = 0x%08x", 1, *(volatile uint32_t *)0xA0070000);
    DSP_MW_LOG_I("0xA0070004 = 0x%08x", 1, *(volatile uint32_t *)0xA0070004);
    DSP_MW_LOG_I("0xA0070008 = 0x%08x", 1, *(volatile uint32_t *)0xA0070008);
    DSP_MW_LOG_I("0xA007000C = 0x%08x", 1, *(volatile uint32_t *)0xA007000C);
    DSP_MW_LOG_I("0xA0070018 = 0x%08x", 1, *(volatile uint32_t *)0xA0070018);
    DSP_MW_LOG_I("0xA007001C = 0x%08x", 1, *(volatile uint32_t *)0xA007001C);
    DSP_MW_LOG_I("0xA0070030 = 0x%08x", 1, *(volatile uint32_t *)0xA0070030);
    DSP_MW_LOG_I("0xA0070034 = 0x%08x", 1, *(volatile uint32_t *)0xA0070034);
    DSP_MW_LOG_I("0xA0070038 = 0x%08x", 1, *(volatile uint32_t *)0xA0070038);
    DSP_MW_LOG_I("0xA007003C = 0x%08x", 1, *(volatile uint32_t *)0xA007003C);
    DSP_MW_LOG_I("0xA0070040 = 0x%08x", 1, *(volatile uint32_t *)0xA0070040);
    DSP_MW_LOG_I("0xA0070044 = 0x%08x", 1, *(volatile uint32_t *)0xA0070044);
    DSP_MW_LOG_I("0xA0070048 = 0x%08x", 1, *(volatile uint32_t *)0xA0070048);
    DSP_MW_LOG_I("0xA0070050 = 0x%08x", 1, *(volatile uint32_t *)0xA0070050);
    DSP_MW_LOG_I("0xA0070054 = 0x%08x", 1, *(volatile uint32_t *)0xA0070054);
    DSP_MW_LOG_I("0xA0070058 = 0x%08x", 1, *(volatile uint32_t *)0xA0070058);
    DSP_MW_LOG_I("0xA007005C = 0x%08x", 1, *(volatile uint32_t *)0xA007005C);
}

static volatile uint32_t g_i2s_slave_port_ref_count = 0;

extern void i2s_slave_ul_interrupt_handler(vdma_event_t event, void  *user_data);
extern void i2s_slave_dl_interrupt_handler(vdma_event_t event, void  *user_data);
extern uint32_t i2s_slave_port_translate(hal_audio_interface_t audio_interface);

static void internal_i2s_slave_common_init(uint32_t port)
{
    int32_t index;
    uint32_t infrasys_baseaddr;

    /* GPIO configure */
    index = g_i2s_slave_pin_index[port];
    hal_gpio_init(g_i2s_slave_pin_cfg[index].tx_pin);
    hal_pinmux_set_function(g_i2s_slave_pin_cfg[index].tx_pin, g_i2s_slave_pin_cfg[index].tx_func);
    hal_gpio_init(g_i2s_slave_pin_cfg[index].clk_pin);
    hal_pinmux_set_function(g_i2s_slave_pin_cfg[index].clk_pin, g_i2s_slave_pin_cfg[index].clk_func);
    hal_gpio_init(g_i2s_slave_pin_cfg[index].ws_pin);
    hal_pinmux_set_function(g_i2s_slave_pin_cfg[index].ws_pin, g_i2s_slave_pin_cfg[index].ws_func);
    hal_gpio_init(g_i2s_slave_pin_cfg[index].rx_pin);
    hal_pinmux_set_function(g_i2s_slave_pin_cfg[index].rx_pin, g_i2s_slave_pin_cfg[index].rx_func);
#if 0
    /* Set I2S slave PDN on */
    *(volatile uint32_t *)(0xA2270320) |= 1 << (4 + port);

    /* Set RG_SW_I2S_DMA_CG on */
    *(volatile uint32_t *)(0xA2270350) |= 1 << 12;

    /* Set Infra I2S setting */
    infrasys_baseaddr = g_i2s_slave_infrasys_baseaddr[port];
    *(volatile uint32_t *)(infrasys_baseaddr + 0x00) = 0x20028; /* default setting */
    *(volatile uint32_t *)(infrasys_baseaddr + 0x30) &= ~(1 << 24); /* PDN clear 26M */
#endif
}

static void internal_i2s_slave_common_deinit(uint32_t port)
{
    uint32_t infrasys_baseaddr;

    infrasys_baseaddr = g_i2s_slave_infrasys_baseaddr[port];

    /* PDN clear 26M */
    *(volatile uint32_t *)(infrasys_baseaddr + 0x30) |= 1 << 24;

    /* Clear PDN on */
    *(volatile uint32_t *)(0xA2030B80) = 1 << (4 + port);
}

/***************************************************
                I2 slave RX side
***************************************************/

static void ul_global_var_init(SOURCE source)
{
    #if 0
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;

    memset(afe_block, 0, sizeof(afe_block_t));
    #else
    UNUSED(source);
    #endif
}

static int32_t i2s_slave_ul_probe(SOURCE source)
{
    hal_audio_device_t device = source->param.audio.audio_device;

    DSP_MW_LOG_I("DSP i2s_slave_ul_probe() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }

    ul_global_var_init(source);

    return 0;
}
static hal_audio_agent_t hal_audio_agent = HAL_AUDIO_AGENT_ERROR;

static int32_t i2s_slave_ul_start(SOURCE source)
{
    uint32_t port;
    vdma_config_t dma_config;
    vdma_channel_t rx_dma_channel;
    vdma_status_t i2s_rx_vdma_status;
    afe_i2s_wlen_t i2s_wlen;
    afe_i2s_format_t i2s_format;
    uint32_t infrasys_baseaddr, irq_status, rx_threshold;
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    hal_audio_device_t device = source->param.audio.audio_device;
    afe_block_t *afe_block = &source->param.audio.AfeBlkControl;
    AUDIO_PARAMETER *runtime = &source->param.audio;
    hal_audio_memory_parameter_t *mem_handle = &source->param.audio.mem_handle;//modify for ab1568
    hal_audio_memory_selection_t memory_select, fined_memory =0, memory_search;//modify for ab1568
    uint32_t channel_index = 0;
    uint32_t update_offset = 0;
    uint32_t mem_size = 0;
    U8 * mem_ptr;
    DSP_MW_LOG_I("DSP i2s_slave_ul_start() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        DSP_MW_LOG_I("i2s_slave_ul_start(): device error\r\n", 0);
        return -1;
    }
    port = i2s_slave_port_translate(audio_interface);
    if(i2s_slave_port_set_flag[port] != false)
    {
        DSP_MW_LOG_I("[i2s_slave_ul_start]i2s slave %d already config\r\n", 1,port);
        return -1;
    }

    hal_nvic_save_and_set_interrupt_mask(&irq_status);
#if 1

    internal_i2s_slave_common_init(port);
    g_i2s_slave_port_ref_count++;
    //internal_i2s_slave_common_init(port); //set i2s slave gpio
    rx_dma_channel = g_i2s_slave_vdma_channel[port * 2 + 1];
    infrasys_baseaddr = g_i2s_slave_infrasys_baseaddr[port];
    //hal_audio_afe_clock_on();
    mem_handle->audio_path_rate = runtime->rate;
    mem_handle->buffer_addr = afe_block->phys_buffer_addr;
    mem_handle->buffer_length = runtime->buffer_size;
    DSP_MW_LOG_I("DSP UL start source type:%d, memory select:0x%x\r\n", 2, source->type, mem_handle->memory_select);//modify for ab1568
    /* set irq start */
    if(!source->param.audio.AfeBlkControl.u4awsflag) {
        mem_handle->sync_status = HAL_AUDIO_MEMORY_SYNC_NONE;
    }else{
        mem_handle->sync_status = HAL_AUDIO_MEMORY_SYNC_SW_TRIGGER;
    }
    if (runtime->AfeBlkControl.u4asrcflag) {
        //asrc tracking source sel
        AFE_SET_REG (infrasys_baseaddr + 0x50,  port << 24, 0x3000000);
    }
#define ReadREG(_addr)          (*(volatile uint32_t *)(_addr))
        //hal_audio_agent_t agent = HAL_AUDIO_AGENT_ERROR;
        if (runtime->audio_interface == HAL_AUDIO_INTERFACE_1) {
            hal_audio_agent = HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE;
            vdma_register_callback(rx_dma_channel, i2s_slave_0_ul_interrupt_handler, NULL);
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
            hal_audio_agent = HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE;
            vdma_register_callback(rx_dma_channel, i2s_slave_1_ul_interrupt_handler, NULL);
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
            hal_audio_agent = HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE;
            vdma_register_callback(rx_dma_channel, i2s_slave_2_ul_interrupt_handler, NULL);
        }
        mem_size = mem_handle->buffer_length;
        if(runtime->AfeBlkControl.u4asrcflag)
        {
            mem_size += runtime->AfeBlkControl.u4asrc_buffer_size;
        }
        afe_block->phys_buffer_addr = hal_memory_allocate_sram(hal_audio_agent, mem_size);

        mem_ptr = afe_block->phys_buffer_addr;

        if (source->param.audio.AfeBlkControl.u4asrcflag) {
            mem_ptr += source->param.audio.AfeBlkControl.u4asrc_buffer_size;
        }
        source->streamBuffer.BufferInfo.startaddr[0] = mem_ptr;

        hal_audio_slave_vdma_parameter_t vdma_setting;
        vdma_setting.base_address = afe_block->phys_buffer_addr;
        vdma_setting.size = mem_handle->buffer_length>>2;
        vdma_setting.threshold = mem_handle->buffer_length>>2;
        vdma_setting.audio_interface = runtime->audio_interface;
#ifdef AIR_AUDIO_I2S_SLAVE_TDM_ENABLE
        vdma_setting.tdm_channel = HAL_AUDIO_I2S_TDM_DISABLE;
        vdma_setting.enable = true;
#endif
        if (runtime->memory == HAL_AUDIO_MEM2) {
            if (runtime->AfeBlkControl.u4asrcflag) {
                afe_asrc_config_t asrc_config;
                afe_set_asrc_ul_configuration_parameters(source, &asrc_config);
                afe_set_asrc_enable(true, AFE_MEM_ASRC_2, &asrc_config);
                source->param.audio.AfeBlkControl.u4ReadIdx = AFE_GET_REG(ASM2_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM2_IBUF_SADR);
                source->param.audio.AfeBlkControl.u4WriteIdx = AFE_GET_REG(ASM2_CH01_IBUF_WRPNT) - AFE_GET_REG(ASM2_IBUF_SADR);
                afe_mem_asrc_enable(AFE_MEM_ASRC_2, true);
                vdma_setting.size = runtime->AfeBlkControl.u4asrc_buffer_size/4;
                vdma_setting.threshold = runtime->AfeBlkControl.u4asrc_buffer_size/2;
            }
        }else{
            if (runtime->AfeBlkControl.u4asrcflag) {
                afe_asrc_config_t asrc_config;
                afe_set_asrc_ul_configuration_parameters(source, &asrc_config);
                afe_set_asrc_enable(true, AFE_MEM_ASRC_1, &asrc_config);
                source->param.audio.AfeBlkControl.u4ReadIdx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
                source->param.audio.AfeBlkControl.u4WriteIdx = AFE_GET_REG(ASM_CH01_IBUF_WRPNT) - AFE_GET_REG(ASM_IBUF_SADR);
                afe_mem_asrc_enable(AFE_MEM_ASRC_1, true);
                vdma_setting.size = runtime->AfeBlkControl.u4asrc_buffer_size/4;
                vdma_setting.threshold = runtime->AfeBlkControl.u4asrc_buffer_size/2;
            }
        }
        vdma_setting.is_ul_mode = true;
        hal_audio_set_value((hal_audio_set_value_parameter_t *)&vdma_setting, HAL_AUDIO_SET_SLAVE_VDMA);
        if (runtime->audio_interface == HAL_AUDIO_INTERFACE_1) {
            Source_Audio_BufferInfo_Rst(source, ReadREG(0xC9000830)-ReadREG(0xC900082c));
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
            Source_Audio_BufferInfo_Rst(source, ReadREG(0xC9000230)-ReadREG(0xC900022c));
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
            Source_Audio_BufferInfo_Rst(source, ReadREG(0xC9000a30)-ReadREG(0xC9000a2c));
        }

#endif
#if 0
    i2s_format = AFE_I2S_SETTING_FORMAT;

    i2s_wlen = (runtime->format_bytes == 2) ? I2S_16BIT : I2S_32BIT;
    rx_threshold = runtime->count * runtime->format_bytes * runtime->channel_num;

    DSP_MW_LOG_I("rate = %d, count = %d, phys_buffer_addr = %x, u4BufferSize = %d, rx_threshold = %d", 5,
                    runtime->rate,
                    runtime->count,
                    runtime->AfeBlkControl.phys_buffer_addr,
                    runtime->AfeBlkControl.u4BufferSize,
                    rx_threshold);

    port = i2s_slave_port_translate(audio_interface);
    infrasys_baseaddr = g_i2s_slave_infrasys_baseaddr[port];

    if (g_i2s_slave_port_ref_count == 0) {
        internal_i2s_slave_common_init(port);
    }
    g_i2s_slave_port_ref_count++;

    *(volatile uint32_t *)(0xA3010074) |= (1 << (port * 2 + 1)); /* Set Infra DMA CLK */
    *(volatile uint32_t *)(0xA301000C) |= (1 << (port * 2 + 1)); /* Set Infra DMA IRQ */

    *(volatile uint32_t *)(infrasys_baseaddr + 0x38) &= ~(1 << 16); /* pdn clear I2S in clk */

    if (i2s_format == I2S_LJ) {
        *(volatile uint32_t *)(infrasys_baseaddr + 0x08) = 0x4048007; /* UL */
    } else if (i2s_format == I2S_RJ) {
        *(volatile uint32_t *)(infrasys_baseaddr + 0x08) = 0x404E007; /* UL */
    } else {
        if (i2s_wlen == I2S_16BIT) {
            *(volatile uint32_t *)(infrasys_baseaddr + 0x08) = 0x0800D; /* UL */
        } else {
            *(volatile uint32_t *)(infrasys_baseaddr + 0x08) = 0x4800F; /* UL */
        }
    }

    *(volatile uint32_t *)(infrasys_baseaddr + 0x30) |= 1 << 16; /* UL FIFO EN */
    *(volatile uint32_t *)(infrasys_baseaddr + 0x38) |= 1 << 0; /* I2S IN enable */
    *(volatile uint32_t *)(infrasys_baseaddr + 0x30) |= 1 << 0; /* ENABLE */


    rx_dma_channel = g_i2s_slave_vdma_channel[port * 2 + 1];
    dma_config.base_address = runtime->AfeBlkControl.phys_buffer_addr;
    dma_config.size = runtime->AfeBlkControl.u4BufferSize/4;// DMA once get 4 byte

    if (runtime->AfeBlkControl.u4asrcflag) {
        afe_asrc_config_t asrc_config;
        afe_set_asrc_ul_configuration_parameters(source, &asrc_config);
        afe_set_asrc_enable(true, AFE_MEM_ASRC_1, &asrc_config);
        source->param.audio.AfeBlkControl.u4ReadIdx = AFE_GET_REG(ASM_CH01_IBUF_RDPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        source->param.audio.AfeBlkControl.u4WriteIdx = AFE_GET_REG(ASM_CH01_IBUF_WRPNT) - AFE_GET_REG(ASM_IBUF_SADR);
        afe_mem_asrc_enable(AFE_MEM_ASRC_1, true);

        dma_config.size = runtime->AfeBlkControl.u4asrc_buffer_size/4;

        if (rx_threshold > (runtime->AfeBlkControl.u4asrc_buffer_size>>1)) {
            rx_threshold = (runtime->AfeBlkControl.u4asrc_buffer_size>>1);
        }
    }

    i2s_rx_vdma_status = vdma_init(rx_dma_channel);
    if (i2s_rx_vdma_status != VDMA_OK) {
        DSP_MW_LOG_I("[I2S%d] SET RX VFIFO ERROR, vdma_init fail\r\n", 1, port);
        return -1;
    }

    i2s_rx_vdma_status = vdma_configure(rx_dma_channel, &dma_config);
    if (i2s_rx_vdma_status != VDMA_OK) {
        DSP_MW_LOG_I("[I2S%d] SET RX VFIFO ERROR, vdma_configure fail\r\n", 1, port);
        return -1;
    }

    i2s_rx_vdma_status = vdma_set_threshold(rx_dma_channel, rx_threshold);
    if (i2s_rx_vdma_status != VDMA_OK) {
        DSP_MW_LOG_I("[I2S%d] SET RX VFIFO ERROR, vdma_set_threshold fail\r\n", 1, port);
        return -1;
    }

    vdma_register_callback(rx_dma_channel, i2s_slave_ul_interrupt_handler, NULL);
    vdma_enable_interrupt(rx_dma_channel);

    i2s_rx_vdma_status = vdma_start(rx_dma_channel);
    if (i2s_rx_vdma_status != VDMA_OK) {
        log_hal_error("[I2S%d] SET TX VFIFO ERROR, vdma_start fail\r\n", port);
        return -1;
    }
#endif
    source->param.audio.is_memory_start = true;
    i2s_slave_port_set_flag[port] = true;
    hal_nvic_restore_interrupt_mask(irq_status);
    //print_reg_list();
    return 0;
}

static int32_t i2s_slave_ul_stop(SOURCE source)
{
    vdma_channel_t rx_dma_channel;
    uint32_t irq_status;
    uint32_t port;
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    hal_audio_device_t device = source->param.audio.audio_device;
    AUDIO_PARAMETER *runtime = &source->param.audio;

    source->param.audio.is_memory_start = false;
    DSP_MW_LOG_I("DSP i2s_slave_ul_stop() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }

    port = i2s_slave_port_translate(audio_interface);

    if(i2s_slave_port_set_flag[port] != true)
    {
        DSP_MW_LOG_I("[i2s_slave_ul_stop]i2s slave %d don't config\r\n", 1,port);
        return -1;
    }

    rx_dma_channel = g_i2s_slave_vdma_channel[port * 2 + 1];

    hal_nvic_save_and_set_interrupt_mask(&irq_status);

    hal_audio_device_parameter_t *device_handle = &source->param.audio.device_handle;
    DSP_MW_LOG_I("DSP i2s_slave_ul_stop() enter:device = 0x%x\r\n", 1,device);

    hal_audio_device_set_agent(device_handle, device, HAL_AUDIO_CONTROL_OFF); //close i2s
        /* Disable I2S out */
    // *(volatile uint32_t *)(infrasys_baseaddr + 0x30) &= ~(1 << 16);
    // *(volatile uint32_t *)(infrasys_baseaddr + 0x38) &= ~(1 << 0);
    // /* Clear FIFO status */
    // *(volatile uint32_t *)(infrasys_baseaddr + 0x14) |= 1 << 0;
    // *(volatile uint32_t *)(infrasys_baseaddr + 0x14) |= 1 << 16;
    // *(volatile uint32_t *)(infrasys_baseaddr + 0x14) &= ~(1 << 0);
    // *(volatile uint32_t *)(infrasys_baseaddr + 0x14) &= ~(1 << 16);
    // /* PDN clear I2S in clk */
    // *(volatile uint32_t *)(infrasys_baseaddr + 0x38) |= 1 << 16;


    // close hwsrc
    if (runtime->memory == HAL_AUDIO_MEM2) {
        if (runtime->AfeBlkControl.u4asrcflag) {
            afe_set_asrc_enable(false, AFE_MEM_ASRC_2, NULL);
            afe_mem_asrc_enable(AFE_MEM_ASRC_2, false);
        }
    }else{
        if (runtime->AfeBlkControl.u4asrcflag) {
            afe_set_asrc_enable(false, AFE_MEM_ASRC_1, NULL);
            afe_mem_asrc_enable(AFE_MEM_ASRC_1, false);
        }
    }

        /* VDMA deinit */
    vdma_disable_interrupt(rx_dma_channel);
    vdma_stop(rx_dma_channel);
    vdma_deinit(rx_dma_channel);

    //free memory
    if(hal_audio_agent != HAL_AUDIO_AGENT_ERROR){
        if (runtime->audio_interface == HAL_AUDIO_INTERFACE_1) {
            hal_audio_agent = HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE;
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_2) {
            hal_audio_agent = HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE;
        } else if (runtime->audio_interface == HAL_AUDIO_INTERFACE_3) {
            hal_audio_agent = HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE;
        }
        hal_memory_free_sram(hal_audio_agent);
        hal_audio_agent = HAL_AUDIO_AGENT_ERROR;
    }

    hal_nvic_restore_interrupt_mask(irq_status);

    return 0;
}

static int32_t i2s_slave_ul_hw_params(SOURCE source)
{
    hal_audio_device_t device = source->param.audio.audio_device;

    DSP_MW_LOG_I("DSP i2s_slave_ul_hw_params() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }

    return 0;
}

static int32_t i2s_slave_ul_open(SOURCE source)
{
    uint32_t source_ch;
    uint32_t port,irq_status;
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;
    AUDIO_PARAMETER *runtime = &source->param.audio;
    hal_audio_device_t device = source->param.audio.audio_device;

    DSP_MW_LOG_I("DSP i2s_slave_ul_open() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }
    port = i2s_slave_port_translate(audio_interface);
    if(i2s_slave_port_set_flag[port] != false)
    {
        DSP_MW_LOG_I("[i2s_slave_ul_open]i2s slave %d already config\r\n", 1,port);
        return -1;
    }
    source_ch = (runtime->channel_num > 2)
                ? 1
                : runtime->channel_num - 1;
    runtime->connect_channel_type = connect_type[source_ch][source_ch];
    hal_nvic_save_and_set_interrupt_mask(&irq_status);

    hal_audio_device_parameter_t *device_handle = &source->param.audio.device_handle;
    hal_audio_afe_clock_on();
    DSP_MW_LOG_I("DSP i2s_slave_ul_open() enter:device = 0x%x\r\n", 1,device);
    hal_audio_device_set_agent(device_handle, device, HAL_AUDIO_CONTROL_ON);

    hal_nvic_restore_interrupt_mask(irq_status);
    return 0;
}

static int32_t i2s_slave_ul_close(SOURCE source)
{
    hal_audio_device_t device = source->param.audio.audio_device;
    uint32_t port;
    hal_audio_interface_t audio_interface = source->param.audio.audio_interface;

    DSP_MW_LOG_I("DSP i2s_slave_ul_close() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }
    port = i2s_slave_port_translate(audio_interface);
    if(i2s_slave_port_set_flag[port] != true)
    {
        DSP_MW_LOG_I("[i2s_slave_ul_close]i2s slave %d don't config\r\n", 1,port);
        return -1;
    }
    hal_audio_path_parameter_t *path_handle = &source->param.audio.path_handle;
    //hal_audio_set_path(path_handle, HAL_AUDIO_CONTROL_OFF);
    i2s_slave_port_set_flag[port] = false;
    return 0;
}

static int32_t i2s_slave_ul_trigger(SOURCE source, int cmd)
{
    int32_t ret;

    DSP_MW_LOG_I("DSP i2s_slave_ul_trigger() enter\r\n", 0);

    switch (cmd) {
        case AFE_PCM_TRIGGER_START:
            ret = i2s_slave_ul_start(source);
            break;
        case AFE_PCM_TRIGGER_STOP:
            ret = i2s_slave_ul_stop(source);
            break;
        case AFE_PCM_TRIGGER_RESUME:
            ret = i2s_slave_ul_open(source);
            break;
        case AFE_PCM_TRIGGER_SUSPEND:
            ret = i2s_slave_ul_close(source);
            break;
        default:
            ret = -1;
            break;
    }

    return ret;
}

static int32_t i2s_slave_ul_copy(SOURCE source, void *dst, uint32_t count)
{
    //copy the AFE src streambuffer to sink streambuffer
    if (Source_Audio_ReadAudioBuffer(source, dst , count) == false) {
        return -1;
    }
    return 0;
}

audio_source_pcm_ops_t i2s_slave_ul_ops = {
    .probe      = i2s_slave_ul_probe,
    .open       = i2s_slave_ul_open,
    .close      = i2s_slave_ul_close,
    .hw_params  = i2s_slave_ul_hw_params,
    .trigger    = i2s_slave_ul_trigger,
    .copy       = i2s_slave_ul_copy,
};

/***************************************************
                I2 slave TX side
***************************************************/
static void dl_global_var_init(SINK sink)
{
    #if 0
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;

    memset(afe_block, 0, sizeof(afe_block_t));
    #else
    UNUSED(sink);
    #endif
}

static int32_t i2s_slave_dl_probe(SINK sink)
{
    hal_audio_device_t device = sink->param.audio.audio_device;

    DSP_MW_LOG_I("DSP i2s_slave_dl_probe() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }

    dl_global_var_init(sink);

    return 0;
}

static int32_t i2s_slave_dl_start(SINK sink)
{
    uint32_t port;
    vdma_channel_t tx_dma_channel;
    vdma_config_t dma_config;
    afe_i2s_wlen_t i2s_wlen;
    afe_i2s_format_t i2s_format;
    uint32_t infrasys_baseaddr, irq_status;
    afe_block_t *afe_block = &sink->param.audio.AfeBlkControl;
    hal_audio_interface_t audio_interface = sink->param.audio.audio_interface;
    hal_audio_device_t device = sink->param.audio.audio_device;
    AUDIO_PARAMETER *runtime = &sink->param.audio;

    DSP_MW_LOG_I("DSP i2s_slave_dl_start() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        DSP_MW_LOG_I("DSP i2s_slave_dl_start() error\r\n", 0);
        return -1;
    }

    hal_nvic_save_and_set_interrupt_mask(&irq_status);

    DSP_MW_LOG_I("rate = %d, count = %d, phys_buffer_addr = %x, u4BufferSize = %d", 4,
                    runtime->rate,
                    runtime->count,
                    runtime->AfeBlkControl.phys_buffer_addr,
                    runtime->AfeBlkControl.u4BufferSize);

    port = i2s_slave_port_translate(audio_interface);
    infrasys_baseaddr = g_i2s_slave_infrasys_baseaddr[port];

    if (g_i2s_slave_port_ref_count == 0) {
        internal_i2s_slave_common_init(port);
    }
    g_i2s_slave_port_ref_count++;

    *(volatile uint32_t *)(0xA3010074) |= (1 << (port * 2)); /* Set Infra DMA CLK */
    *(volatile uint32_t *)(0xA301000C) |= (1 << (port * 2)); /* Set Infra DMA IRQ */

    *(volatile uint32_t *)(infrasys_baseaddr + 0x34) &= ~(1 << 16); /* pdn clear I2S out clk */

    i2s_format = AFE_I2S_SETTING_FORMAT;
    i2s_wlen = AFE_I2S_SETTING_WORD_LENGTH;
    if (i2s_format == I2S_LJ) {
        *(volatile uint32_t *)(infrasys_baseaddr + 0x04) = 0x40400A7; /* DL */
    } else if (i2s_format == I2S_RJ) {
        *(volatile uint32_t *)(infrasys_baseaddr + 0x04) = 0x40460A7; /* DL */
    } else {
        if (i2s_wlen == I2S_16BIT) {
            *(volatile uint32_t *)(infrasys_baseaddr + 0x04) = 0x0000D; /* DL */
        } else {
            *(volatile uint32_t *)(infrasys_baseaddr + 0x04) = 0x4008F; /* DL */
        }
    }

    *(volatile uint32_t *)(infrasys_baseaddr + 0x30) |= 1 << 8; /* DL FIFO EN */
    *(volatile uint32_t *)(infrasys_baseaddr + 0x34) |= 1 << 0; /* I2S OUT enable */

    *(volatile uint32_t *)(infrasys_baseaddr + 0x30) |= 1 << 0; /* ENABLE */

    tx_dma_channel = g_i2s_slave_vdma_channel[port * 2];
    vdma_init(tx_dma_channel);
    dma_config.base_address = afe_block->phys_buffer_addr;
    dma_config.size = afe_block->u4BufferSize;
    vdma_configure(tx_dma_channel, &dma_config);
    vdma_set_threshold(tx_dma_channel, 0);
    vdma_register_callback(tx_dma_channel, i2s_slave_dl_interrupt_handler, NULL);
    vdma_start(tx_dma_channel);

    /* Insert dummy audio data to avoid IRQ triggered immediately after vdma_enable_interrupt() is called. */
/*
    for (i = 0; i < afe_block->u4BufferSize; i++) {
        vdma_push_data_4bytes(tx_dma_channel, 0x00000000);
    }

    vdma_enable_interrupt(tx_dma_channel);
*/
    hal_nvic_restore_interrupt_mask(irq_status);

    print_reg_list();

    return 0;
}

static int32_t i2s_slave_dl_stop(SINK sink)
{
    vdma_channel_t tx_dma_channel;
    uint32_t infrasys_baseaddr, irq_status;
    uint32_t port;
    hal_audio_interface_t audio_interface = sink->param.audio.audio_interface;
    hal_audio_device_t device = sink->param.audio.audio_device;

    DSP_MW_LOG_I("DSP i2s_slave_dl_stop() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }

    if (g_i2s_slave_port_ref_count == 0) {
        DSP_MW_LOG_I("found g_i2s_slave_port_ref_count abnormal", 0);
        return -1;
    }

    port = i2s_slave_port_translate(audio_interface);
    tx_dma_channel = g_i2s_slave_vdma_channel[port * 2];
    infrasys_baseaddr = g_i2s_slave_infrasys_baseaddr[port];

    hal_nvic_save_and_set_interrupt_mask(&irq_status);

    /* Disable I2S out */
    *(volatile uint32_t *)(infrasys_baseaddr + 0x30) &= ~(1 << 8);
    *(volatile uint32_t *)(infrasys_baseaddr + 0x34) &= ~(1 << 0);
    /* Clear FIFO status */
    *(volatile uint32_t *)(infrasys_baseaddr + 0x10) |= 1 << 0;
    *(volatile uint32_t *)(infrasys_baseaddr + 0x10) |= 1 << 16;
    *(volatile uint32_t *)(infrasys_baseaddr + 0x10) &= ~(1 << 0);
    *(volatile uint32_t *)(infrasys_baseaddr + 0x10) &= ~(1 << 16);
    /* PDN clear I2S in clk */
    *(volatile uint32_t *)(infrasys_baseaddr + 0x34) |= 1 << 16;

    g_i2s_slave_port_ref_count--;
    if (g_i2s_slave_port_ref_count == 0) {
        internal_i2s_slave_common_deinit(port);
    }

    /* VDMA deinit */
    vdma_disable_interrupt(tx_dma_channel);
    vdma_stop(tx_dma_channel);
    vdma_deinit(tx_dma_channel);

    hal_nvic_restore_interrupt_mask(irq_status);

    return 0;
}

static int32_t i2s_slave_dl_hw_params(SINK sink)
{
    hal_audio_device_t device = sink->param.audio.audio_device;

    DSP_MW_LOG_I("DSP i2s_slave_dl_hw_params() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }

    return 0;
}

static int32_t i2s_slave_dl_open(SINK sink)
{
    uint32_t source_ch;
    AUDIO_PARAMETER *runtime = &sink->param.audio;
    hal_audio_device_t device = sink->param.audio.audio_device;

    DSP_MW_LOG_I("DSP i2s_slave_dl_open() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }

    source_ch = (runtime->channel_num > 2)
                ? 1
                : runtime->channel_num - 1;
    runtime->connect_channel_type = connect_type[source_ch][source_ch];

    return 0;
}

static int32_t i2s_slave_dl_close(SINK sink)
{
    hal_audio_device_t device = sink->param.audio.audio_device;

    DSP_MW_LOG_I("DSP i2s_slave_dl_close() enter\r\n", 0);

    if (device != HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE) {
        return -1;
    }

    return 0;
}

static int32_t i2s_slave_dl_trigger(SINK sink, int cmd)
{
    DSP_MW_LOG_I("DSP i2s_slave_dl_trigger() enter\r\n", 0);

    switch (cmd) {
        case AFE_PCM_TRIGGER_START:
            return i2s_slave_dl_start(sink);
            break;
        case AFE_PCM_TRIGGER_STOP:
            return i2s_slave_dl_stop(sink);
            break;
        case AFE_PCM_TRIGGER_RESUME:
            return i2s_slave_dl_open(sink);
            break;
        case AFE_PCM_TRIGGER_SUSPEND:
            return i2s_slave_dl_close(sink);
            break;
        default:
            break;
    }

    return -1;
}

// src: Source Streambuffer not Sink Streambuffer
static int32_t i2s_slave_dl_copy(SINK sink, void *src, uint32_t count)
{
    // count: w/o channl, unit: bytes
    // copy the src's streambuffer to sink's streambuffer
    if (Sink_Audio_WriteBuffer(sink, src, count) == false) {
        return -1;
    }
    return 0;
}

audio_sink_pcm_ops_t i2s_slave_dl_ops = {
    .probe      = i2s_slave_dl_probe,
    .open       = i2s_slave_dl_open,
    .close      = i2s_slave_dl_close,
    .hw_params  = i2s_slave_dl_hw_params,
    .trigger    = i2s_slave_dl_trigger,
    .copy       = i2s_slave_dl_copy,
};
#if 0 //multi define
void i2s_slave_dl_update_wptr(SINK sink, U32 amount)
{
    uint32_t port;
    vdma_channel_t tx_dma_channel;

    port = i2s_slave_port_translate(sink->param.audio.audio_interface);
    tx_dma_channel = g_i2s_slave_vdma_channel[port * 2];
    vdma_set_sw_move_byte(tx_dma_channel, amount);
}
#endif
