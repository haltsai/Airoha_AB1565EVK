/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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


#include "hal_audio_driver.h"
#include "hal_audio_register.h"
#include "hal_audio_clock.h"
#include "hal_audio_volume.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#define HAL_AUDIO_APLL_TUNER_NUMBER     (1)
#define HAL_AUDIO_DMIC_CLOCK_SETTING    (0)

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables Declaration //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int16_t ana_enable_micbias_LDO0_counter;
static int16_t ana_enable_capless_LDO_counter;
static int16_t ana_enable_CLK_TO_ADC01_counter;
static int16_t ana_enable_CLK_TO_ADC23_counter;
static int16_t ana_enable_CLK_TO_ADC45_counter;
extern uint16_t vow_pre_ch0_noise_msb;
extern uint16_t vow_pre_ch1_noise_msb;
#ifdef AIR_A2DP_NLE_ENABLE
static bool open_nle;
#endif
#ifdef AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE
uint32_t DRC_DGAIN;
#endif
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functiion Declaration //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint32_t afe_samplerate_get_ul_register(hal_audio_agent_t agent);


/*******************************************************************************************
*                                       SET REGISTER                                       *
********************************************************************************************/
void hal_aduio_set_register_32bit(uint32_t addr, uint32_t val, uint32_t msk)
{
    AFE_WRITE((addr), ((AFE_READ(addr) & (~(msk))) | ((val) & (msk))));
    //printf("32bitaddr:0x%x,val:0x%x\r\n",addr,AFE_READ(addr));
}

void hal_aduio_set_register_16bit(uint32_t addr, uint32_t val, uint32_t msk)
{
    ANA_WRITE((addr), ((ANA_READ(addr) & (~(msk))) | ((val) & (msk))));
    //printf("16bitaddr:0x%x,val:0x%x\r\n",addr,ANA_READ(addr));
}

void hal_aduio_set_register_8bit(uint32_t addr, uint32_t val, uint32_t msk)
{
    AFE_WRITE8((addr), ((AFE_READ8(addr) & (~(msk))) | ((val) & (msk))));
}

/*******************************************************************************************
*                                       Sample rate                                        *
********************************************************************************************/
static afe_samplerate_adda_dl_t afe_samplerate_adda_dl_convert_samplerate_to_register_value(uint32_t samplerate)
{
    switch (samplerate) {
        case 8000:
            return AFE_ADDA_DL_SAMPLERATE_8K;
        case 11025:
            return AFE_ADDA_DL_SAMPLERATE_11K;
        case 12000:
            return AFE_ADDA_DL_SAMPLERATE_12K;
        case 16000:
            return AFE_ADDA_DL_SAMPLERATE_16K;
        case 22050:
            return AFE_ADDA_DL_SAMPLERATE_22K;
        case 24000:
            return AFE_ADDA_DL_SAMPLERATE_24K;
        case 32000:
            return AFE_ADDA_DL_SAMPLERATE_32K;
        case 44100:
            return AFE_ADDA_DL_SAMPLERATE_44K;
        case 48000:
            return AFE_ADDA_DL_SAMPLERATE_48K;
        case 96000:
            return AFE_ADDA_DL_SAMPLERATE_96K;
        case 192000:
            return AFE_ADDA_DL_SAMPLERATE_192K;
        default:
            return AFE_ADDA_DL_SAMPLERATE_44K;
    }
}

static afe_samplerate_adda_ul_t afe_samplerate_adda_ul_convert_samplerate_to_register_value(uint32_t samplerate)
{
    switch (samplerate) {
        case 8000:
            return AFE_ADDA_UL_SAMPLERATE_8K;
        case 16000:
            return AFE_ADDA_UL_SAMPLERATE_16K;
        case 32000:
            return AFE_ADDA_UL_SAMPLERATE_32K;
        case 48000:
            return AFE_ADDA_UL_SAMPLERATE_48K;
        case 96000:
            return AFE_ADDA_UL_SAMPLERATE_96K;
        case 192000:
            return AFE_ADDA_UL_SAMPLERATE_192K;
        default:
            return AFE_ADDA_UL_SAMPLERATE_16K;
    }
}

afe_samplerate_general_t afe_samplerate_convert_samplerate_to_register_value(uint32_t samplerate)
{
    switch (samplerate) {
        case 8000:
            return AFE_GENERAL_SAMPLERATE_8K;
        case 11025:
            return AFE_GENERAL_SAMPLERATE_11K;
        case 12000:
            return AFE_GENERAL_SAMPLERATE_12K;
        case 16000:
            return AFE_GENERAL_SAMPLERATE_16K;
        case 22050:
            return AFE_GENERAL_SAMPLERATE_22K;
        case 24000:
            return AFE_GENERAL_SAMPLERATE_24K;
        case 32000:
            return AFE_GENERAL_SAMPLERATE_32K;
        case 44100:
            return AFE_GENERAL_SAMPLERATE_44K;
        case 48000:
            return AFE_GENERAL_SAMPLERATE_48K;
        case 88200:
            return AFE_GENERAL_SAMPLERATE_88K;
        case 96000:
            return AFE_GENERAL_SAMPLERATE_96K;
        case 176400:
            return AFE_GENERAL_SAMPLERATE_176K;
        case 192000:
            return AFE_GENERAL_SAMPLERATE_192K;
        default:
            return AFE_GENERAL_SAMPLERATE_44K;
    }
}


static uint32_t afe_samplerate_adda_dl_convert_register_value_to_samplerate(afe_samplerate_adda_dl_t register_value)
{
    switch (register_value) {
        case AFE_ADDA_DL_SAMPLERATE_8K:
            return 8000;
        case AFE_ADDA_DL_SAMPLERATE_11K:
            return 11025;
        case AFE_ADDA_DL_SAMPLERATE_12K:
            return 12000;
        case AFE_ADDA_DL_SAMPLERATE_16K:
            return 16000;
        case AFE_ADDA_DL_SAMPLERATE_22K:
            return 22050;
        case AFE_ADDA_DL_SAMPLERATE_24K:
            return 24000;
        case AFE_ADDA_DL_SAMPLERATE_32K:
            return 32000;
        case AFE_ADDA_DL_SAMPLERATE_44K:
            return 44100;
        case AFE_ADDA_DL_SAMPLERATE_48K:
            return 48000;
        case AFE_ADDA_DL_SAMPLERATE_96K:
            return 96000;
        case AFE_ADDA_DL_SAMPLERATE_192K:
            return 192000;
        default:
            return 44100;
    }
}

static uint32_t afe_samplerate_adda_ul_convert_register_value_to_samplerate(afe_samplerate_adda_ul_t register_value)
{
    switch (register_value) {
        case AFE_ADDA_UL_SAMPLERATE_8K:
            return 8000;
        case AFE_ADDA_UL_SAMPLERATE_16K:
            return 16000;
        case AFE_ADDA_UL_SAMPLERATE_32K:
            return 32000;
        case AFE_ADDA_UL_SAMPLERATE_48K:
            return 48000;
        case AFE_ADDA_UL_SAMPLERATE_96K:
            return 96000;
        case AFE_ADDA_UL_SAMPLERATE_192K:
            return 192000;
        default:
            return 16000;
    }
}

uint32_t afe_samplerate_convert_register_value_to_samplerate(afe_samplerate_general_t register_value)
{
    switch (register_value) {
        case AFE_GENERAL_SAMPLERATE_8K:
            return 8000;
        case AFE_GENERAL_SAMPLERATE_11K:
            return 11025;
        case AFE_GENERAL_SAMPLERATE_12K:
            return 12000;
        case AFE_GENERAL_SAMPLERATE_16K:
            return 16000;
        case AFE_GENERAL_SAMPLERATE_22K:
            return 22050;
        case AFE_GENERAL_SAMPLERATE_24K:
            return 24000;
        case AFE_GENERAL_SAMPLERATE_32K:
            return 32000;
        case AFE_GENERAL_SAMPLERATE_44K:
            return 44100;
        case AFE_GENERAL_SAMPLERATE_48K:
            return 48000;
        case AFE_GENERAL_SAMPLERATE_88K:
            return 88200;
        case AFE_GENERAL_SAMPLERATE_96K:
            return 96000;
        case AFE_GENERAL_SAMPLERATE_176K:
            return 176400;
        case AFE_GENERAL_SAMPLERATE_192K:
            return 192000;
        default:
            return 44100;
    }
}


/*              I2S sample rate              */
void afe_samplerate_set_i2s_master_samplerate(afe_i2s_id_t i2s_id, uint32_t samplerate)
{
    uint32_t reg_value = 0;
    uint32_t i2s_reg = hal_i2s_master_convert_i2s_register(i2s_id);
    reg_value = afe_samplerate_convert_samplerate_to_register_value(samplerate);
    AFE_SET_REG(i2s_reg, reg_value<<AFE_I2S0_CON_RATE_POS, AFE_I2S0_CON_RATE_MASK);
}

uint32_t afe_samplerate_get_i2s_master_samplerate(afe_i2s_id_t i2s_id)
{
    uint32_t reg_value = 0;
    uint32_t i2s_reg = hal_i2s_master_convert_i2s_register(i2s_id);
    reg_value = (AFE_GET_REG(i2s_reg)&AFE_I2S0_CON_RATE_MASK)>>AFE_I2S0_CON_RATE_POS;
    return afe_samplerate_convert_register_value_to_samplerate(reg_value);
}

/*              UL sample rate              */
void afe_samplerate_set_ul_samplerate(hal_audio_agent_t agent, uint32_t samplerate)
{
    uint32_t register_value = 0;
    uint32_t reg = afe_samplerate_get_ul_register(agent);
    register_value = afe_samplerate_adda_ul_convert_samplerate_to_register_value(samplerate);
    AFE_SET_REG(reg, register_value << AFE_ADDA_UL_SRC_CON0_RATE_POS, AFE_ADDA_UL_SRC_CON0_RATE_MASK);
}
uint32_t afe_samplerate_get_ul_samplerate(hal_audio_agent_t agent)
{
    uint32_t register_value;
    uint32_t reg = afe_samplerate_get_ul_register(agent);

    register_value = (AFE_GET_REG(reg)&AFE_ADDA_UL_SRC_CON0_RATE_MASK)>>AFE_ADDA_UL_SRC_CON0_RATE_POS;
    return afe_samplerate_adda_ul_convert_register_value_to_samplerate(register_value);
}

/*              DL sample rate              */
void afe_samplerate_set_dl_samplerate(uint32_t samplerate)
{
    uint32_t register_value = 0;
    register_value = afe_samplerate_adda_dl_convert_samplerate_to_register_value(samplerate);
    AFE_SET_REG(AFE_ADDA_DL_SRC2_CON0, register_value << AFE_ADDA_DL_SRC2_CON0_RATE_POS, AFE_ADDA_DL_SRC2_CON0_RATE_MASK);
}
uint32_t afe_samplerate_get_dl_samplerate(void)
{
    uint32_t register_value = 0;
    register_value = ((AFE_GET_REG(AFE_ADDA_DL_SRC2_CON0)&AFE_ADDA_DL_SRC2_CON0_RATE_MASK)>> AFE_ADDA_DL_SRC2_CON0_RATE_POS);
    return afe_samplerate_adda_dl_convert_register_value_to_samplerate(register_value);
}



uint32_t hal_samplerate_convert_to_register_value(hal_audio_agent_t agent, uint32_t samplerate)
{
    uint32_t register_value = 0;
    switch (agent) {
        case HAL_AUDIO_AGENT_DEVICE_ADDA_DL1:
            register_value = afe_samplerate_adda_dl_convert_samplerate_to_register_value(samplerate);
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1:
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL2:
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL3:
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL4:
            register_value = afe_samplerate_adda_ul_convert_samplerate_to_register_value(samplerate);
            break;
        default:
            register_value = afe_samplerate_convert_samplerate_to_register_value(samplerate);
            break;
    }
    return register_value;
}

/*******************************************************************************************
*                                       Device agent                                       *
********************************************************************************************/
hal_audio_agent_t hal_device_convert_agent(hal_audio_control_t device, hal_audio_device_interface_t device_interface)
{
    hal_audio_agent_t agent = HAL_AUDIO_AGENT_ERROR;
    HAL_AUDIO_LOG_INFO("hal_device_convert_agent %d if %d",2,device,device_interface);
    switch (device) {
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L:
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R:
        case HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL:
            agent = HAL_AUDIO_AGENT_DEVICE_ADDA_UL1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_L:
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_R:
        case HAL_AUDIO_CONTROL_DEVICE_LINE_IN_DUAL:
            agent = HAL_AUDIO_AGENT_DEVICE_ADDA_UL1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R:
        case HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL:
            agent = HAL_AUDIO_AGENT_DEVICE_ADDA_UL1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_L:
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_R:
        case HAL_AUDIO_CONTROL_DEVICE_INTERNAL_DAC_DUAL:
            agent = HAL_AUDIO_AGENT_DEVICE_ADDA_DL1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER:
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L:
        case HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R:
            agent = HAL_AUDIO_AGENT_DEVICE_I2S0_MASTER;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE:
            agent = HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_SPDIF:
            agent = HAL_AUDIO_AGENT_DEVICE_I2S0_MASTER;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_ANC:
            agent = HAL_AUDIO_AGENT_DEVICE_ANC;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_LOOPBACK:
            agent = HAL_AUDIO_AGENT_DEVICE_ADDA_UL1;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_VAD:
            agent = HAL_AUDIO_AGENT_DEVICE_VAD;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_SIDETONE:
            agent = HAL_AUDIO_AGENT_DEVICE_SIDETONE;
            break;
        case HAL_AUDIO_CONTROL_DEVICE_VOW:
            agent = HAL_AUDIO_AGENT_DEVICE_VOW;
            break;
        default:
            break;
    }


    if (device&(HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_DUAL|HAL_AUDIO_CONTROL_DEVICE_LOOPBACK|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R|HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE|HAL_AUDIO_CONTROL_DEVICE_SPDIF)) {
        if (device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
            agent += 1;
        } else if (device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
            agent += 2;
        } else if (device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_4) {
            agent += 3;
        }
    }

    if (agent == HAL_AUDIO_AGENT_ERROR) {
        HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio Device Wrong Agent :%d !", 1, device);
        HAL_AUDIO_ASSERT(false);
    }

    return agent;
}



uint32_t afe_samplerate_get_ul_register(hal_audio_agent_t agent)
{
    uint32_t reg;

    switch (agent) {
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1:
        default:
            reg = AFE_ADDA_UL_SRC_CON0;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL2:
            reg = AFE_ADDA2_UL_SRC_CON0;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL3:
            reg = AFE_ADDA6_UL_SRC_CON0;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL4:
            reg = AFE_ADDA_ANC_UL_SRC_CON0;
            break;
    }
    return reg;
}


/*******************************************************************************************
*                                       Memory agent                                       *
********************************************************************************************/
hal_audio_agent_t hal_memory_convert_agent(hal_audio_memory_selection_t memory_select)
{
    hal_audio_agent_t agent = HAL_AUDIO_AGENT_ERROR;

    switch (memory_select) {
        case HAL_AUDIO_MEMORY_DL_DL1:
            agent = HAL_AUDIO_AGENT_MEMORY_DL1;
            break;
        case HAL_AUDIO_MEMORY_DL_DL2:
            agent = HAL_AUDIO_AGENT_MEMORY_DL2;
            break;
        case HAL_AUDIO_MEMORY_DL_DL3:
            agent = HAL_AUDIO_AGENT_MEMORY_DL3;
            break;
        case HAL_AUDIO_MEMORY_DL_DL12:
            agent = HAL_AUDIO_AGENT_MEMORY_DL12;
            break;
        case HAL_AUDIO_MEMORY_DL_SRC1:
            agent = HAL_AUDIO_AGENT_MEMORY_SRC1;
            break;
        case HAL_AUDIO_MEMORY_DL_SRC2:
            agent = HAL_AUDIO_AGENT_MEMORY_SRC2;
            break;
        case HAL_AUDIO_MEMORY_UL_VUL1:
            agent = HAL_AUDIO_AGENT_MEMORY_VUL1;
            break;
        case HAL_AUDIO_MEMORY_UL_VUL2:
            agent = HAL_AUDIO_AGENT_MEMORY_VUL2;
            break;
        case HAL_AUDIO_MEMORY_UL_VUL3:
            agent = HAL_AUDIO_AGENT_MEMORY_VUL3;
            break;
        case HAL_AUDIO_MEMORY_UL_AWB:
            agent = HAL_AUDIO_AGENT_MEMORY_AWB;
            break;
        case HAL_AUDIO_MEMORY_UL_AWB2:
            agent = HAL_AUDIO_AGENT_MEMORY_AWB2;
            break;
        case HAL_AUDIO_MEMORY_UL_MASK:
            agent = HAL_AUDIO_AGENT_DEVICE_ADDA_UL1;
            break;
        default:
            break;
    }

    if (agent == HAL_AUDIO_AGENT_ERROR) {
        HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio Memory Wrong Agent memory:%d !", 1, memory_select);
        HAL_AUDIO_ASSERT(false);
    }

    return agent;
}

bool hal_memory_set_enable_by_memory_selection (hal_audio_memory_selection_t memory_select, hal_audio_control_status_t control)
{
    uint32_t rg_bit_mask = 0;

    if (memory_select & HAL_AUDIO_MEMORY_DL_DL1) {
        rg_bit_mask |= AFE_DAC_CON0_DL1_ON_MASK;
    }
    if (memory_select & HAL_AUDIO_MEMORY_DL_DL2) {
        rg_bit_mask |= AFE_DAC_CON0_DL2_ON_MASK;
    }
    if (memory_select & HAL_AUDIO_MEMORY_DL_DL3) {
        rg_bit_mask |= AFE_DAC_CON0_DL3_ON_MASK;
    }
    if (memory_select & HAL_AUDIO_MEMORY_DL_DL12) {
        rg_bit_mask |= AFE_DAC_CON0_DL12_ON_MASK;
    }
    if (memory_select & HAL_AUDIO_MEMORY_UL_VUL1) {
        rg_bit_mask |= AFE_DAC_CON0_VUL_ON_MASK;
    }
    if (memory_select & HAL_AUDIO_MEMORY_UL_VUL2) {
        rg_bit_mask |= AFE_DAC_CON0_VUL2_ON_MASK;
    }
    if (memory_select & HAL_AUDIO_MEMORY_UL_VUL3) {
        rg_bit_mask |= AFE_DAC_CON0_VUL3_ON_MASK;
    }
    if (memory_select & HAL_AUDIO_MEMORY_UL_AWB) {
        rg_bit_mask |= AFE_DAC_CON0_AWB_ON_MASK;
    }
    if (memory_select & HAL_AUDIO_MEMORY_UL_AWB2) {
        rg_bit_mask |= AFE_DAC_CON0_AWB2_ON_MASK;
    }

    if ((AFE_GET_REG(AFE_DAC_CON0) & 0x01) == 0) {
        HAL_AUDIO_LOG_ERROR("DSP - Error, please check running_flag & user conuter ! AFE_DAC_CON0 0x%x memory_select:0x%x", 2, AFE_GET_REG(AFE_DAC_CON0), memory_select);
        HAL_AUDIO_ASSERT(false);
    }

    if (control) {
        AFE_SET_REG(AFE_DAC_CON0, rg_bit_mask, rg_bit_mask);
    } else {
        AFE_SET_REG(AFE_DAC_CON0, 0, rg_bit_mask);
    }

    HAL_AUDIO_LOG_INFO("DSP - Hal memory set enable, memory_select[0x%x], RG_mask:0x%x, on/off [%d]", 3, memory_select, rg_bit_mask, control);
    return false;
}

bool hal_memory_set_enable(hal_audio_agent_t agent, hal_audio_control_status_t control)
{
    uint32_t offset_bit = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            offset_bit = AFE_DAC_CON0_DL1_ON_POS;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            offset_bit = AFE_DAC_CON0_DL2_ON_POS;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            offset_bit = AFE_DAC_CON0_DL3_ON_POS;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            offset_bit = AFE_DAC_CON0_DL12_ON_POS;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            offset_bit = AFE_DAC_CON0_VUL_ON_POS;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            offset_bit = AFE_DAC_CON0_VUL2_ON_POS;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            offset_bit = AFE_DAC_CON0_VUL3_ON_POS;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            offset_bit = AFE_DAC_CON0_AWB_ON_POS;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            offset_bit = AFE_DAC_CON0_AWB2_ON_POS;
            break;
        default:
            return true;
            break;
    }
    if((AFE_GET_REG(AFE_DAC_CON0) & 0x01) == 0){
        HAL_AUDIO_LOG_ERROR("DSP - Error, please check running_flag & user conuter ! AFE_DAC_CON0 0x%x agent %d",2,AFE_GET_REG(AFE_DAC_CON0),agent);
        HAL_AUDIO_ASSERT(false);
    }
    AFE_SET_REG(AFE_DAC_CON0, control<<offset_bit, 1<<offset_bit);
    HAL_AUDIO_LOG_INFO("DSP - Hal memory set enable, agent[%d] on/off [%d]", 2, agent, control);
    return false;
}

bool hal_memory_set_samplerate(hal_audio_agent_t agent, uint32_t samplerate)
{
    uint32_t reg, offset_bit = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            reg = AFE_DAC_CON1;
            offset_bit = 0;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            reg = AFE_DAC_CON1;
            offset_bit = 4;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            reg = AFE_DAC_CON2;
            offset_bit = 8;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            reg = AFE_DAC_CON0;
            offset_bit = AFE_DAC_CON0_DL12_RATE_POS;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            reg = AFE_DAC_CON1;
            offset_bit = 16;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            reg = AFE_DAC_CON2;
            offset_bit = 4;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            reg = AFE_DAC_CON2;
            offset_bit = 24;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            reg = AFE_DAC_CON1;
            offset_bit = 12;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            reg = AFE_DAC_CON2;
            offset_bit = 16;
            break;
        default:
            return true;
            break;
    }
    AFE_SET_REG(reg, afe_samplerate_convert_samplerate_to_register_value(samplerate)<<offset_bit, 0xF<<offset_bit);
    return false;
}

bool hal_memory_set_mono_r_channel(hal_audio_agent_t agent, bool mono_use_r_channel)
{
    uint32_t reg = 0, offset_bit = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            reg = AFE_DAC_CON1;
            offset_bit = 28;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            reg = AFE_DAC_CON2;
            offset_bit = 1;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            reg = AFE_DAC_CON2;
            offset_bit = 23;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            reg = AFE_DAC_CON1;
            offset_bit = 25;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            reg = AFE_DAC_CON2;
            offset_bit = 21;
            break;
        default:
            return true;
            break;
    }

    AFE_SET_REG(reg, mono_use_r_channel<<offset_bit, 1<<offset_bit);
    return false;
}

bool hal_memory_set_channel(hal_audio_agent_t agent, uint32_t channel)
{
    uint32_t reg = 0, offset_bit = 0;
    bool mono = (channel == 1) ? true : false;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            reg = AFE_DAC_CON1;
            offset_bit = 21;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            reg = AFE_DAC_CON1;
            offset_bit = 22;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            reg = AFE_DAC_CON1;
            offset_bit = 23;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            reg = AFE_DAC_CON1;
            offset_bit = 20;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            reg = AFE_DAC_CON1;
            offset_bit = 27;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            reg = AFE_DAC_CON2;
            offset_bit = 0;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            reg = AFE_DAC_CON2;
            offset_bit = 22;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            reg = AFE_DAC_CON1;
            offset_bit = 24;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            reg = AFE_DAC_CON2;
            offset_bit = 20;
            break;
        default:
            return true;
            break;
    }

    AFE_SET_REG(reg, mono<<offset_bit, 1<<offset_bit);
    return false;
}

bool hal_memory_set_format(hal_audio_agent_t agent, hal_audio_pcm_format_t format)
{
    uint32_t reg = AFE_MEMIF_HD_MODE, offset_bit = 0;
    bool is_hd = (format > HAL_AUDIO_PCM_FORMAT_U16_BE) ? 1 : 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            offset_bit = 0;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            offset_bit = 4;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            offset_bit = 6;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            offset_bit = 2;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            offset_bit = 10;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            offset_bit = 14;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            offset_bit = 30;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            offset_bit = 8;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            offset_bit = 28;
            break;
        default:
            return true;
            break;
    }

    AFE_SET_REG(reg, is_hd<<offset_bit, 3<<offset_bit);
    return false;
}

bool hal_memory_set_align(hal_audio_agent_t agent, afe_memory_align_t memory_align)
{
    uint32_t reg = AFE_MEMIF_HDALIGN, offset_bit = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            offset_bit = 0;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            offset_bit = 2;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            offset_bit = 3;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            offset_bit = 1;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            offset_bit = 5;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            offset_bit = 7;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            offset_bit = 15;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            offset_bit = 4;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            offset_bit = 14;
            break;
        default:
            return true;
            break;
    }

    AFE_SET_REG(reg, memory_align<<offset_bit, 1<<offset_bit);
    return false;
}

bool hal_memory_set_buffer_mode(hal_audio_agent_t agent, afe_memory_buffer_mode_t buffer_mode)
{
    uint32_t reg = AFE_MEMIF_HDALIGN, offset_bit = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            offset_bit = 16;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            offset_bit = 18;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            offset_bit = 19;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            offset_bit = 17;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            offset_bit = 21;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            offset_bit = 23;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            offset_bit = 31;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            offset_bit = 20;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            offset_bit = 30;
            break;
        default:
            return true;
            break;
    }

    AFE_SET_REG(reg, buffer_mode<<offset_bit, 1<<offset_bit);
    return false;
}

bool hal_memory_set_address(hal_audio_memory_parameter_t *handle, hal_audio_agent_t agent)
{
    uint32_t afe_base_register = 0, afe_end_register = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            afe_base_register = AFE_DL1_BASE;
            afe_end_register = AFE_DL1_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            afe_base_register = AFE_DL2_BASE;
            afe_end_register = AFE_DL2_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            afe_base_register = AFE_DL3_BASE;
            afe_end_register = AFE_DL3_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            afe_base_register = AFE_DL12_BASE;
            afe_end_register = AFE_DL12_END;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            afe_base_register = AFE_VUL_BASE;
            afe_end_register = AFE_VUL_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            afe_base_register = AFE_VUL2_BASE;
            afe_end_register = AFE_VUL2_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            afe_base_register = AFE_VUL3_BASE;
            afe_end_register = AFE_VUL3_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            afe_base_register = AFE_AWB_BASE;
            afe_end_register = AFE_AWB_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            afe_base_register = AFE_AWB2_BASE;
            afe_end_register = AFE_AWB2_END;
            break;
        default:
            break;
    }

    if ((afe_base_register!=0) && (afe_end_register!=0)) {
        AFE_SET_REG(afe_base_register, (handle->buffer_addr), 0xFFFFFFFF);
        AFE_SET_REG(afe_end_register, handle->buffer_addr + (handle->buffer_length - 1), 0xFFFFFFFF);
        HAL_AUDIO_LOG_INFO("hal_memory_set_address 0x%x,length:%d agent %d",3,handle->buffer_addr,handle->buffer_length,agent);
    }

    return false;
}

uint32_t hal_memory_get_address(hal_audio_agent_t agent)
{
    uint32_t afe_base_register = 0, base_address = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            afe_base_register = AFE_DL1_BASE;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            afe_base_register = AFE_DL2_BASE;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            afe_base_register = AFE_DL3_BASE;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            afe_base_register = AFE_DL12_BASE;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            afe_base_register = AFE_VUL_BASE;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            afe_base_register = AFE_VUL2_BASE;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            afe_base_register = AFE_VUL3_BASE;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            afe_base_register = AFE_AWB_BASE;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            afe_base_register = AFE_AWB2_BASE;
            break;
        default:
            break;
    }

    if (afe_base_register!=0) {
        base_address = AFE_GET_REG(afe_base_register);
    }
    return base_address;
}

uint32_t hal_memory_get_length(hal_audio_agent_t agent)
{
    uint32_t afe_base_register = 0, afe_end_register = 0, length = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            afe_base_register = AFE_DL1_BASE;
            afe_end_register = AFE_DL1_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            afe_base_register = AFE_DL2_BASE;
            afe_end_register = AFE_DL2_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            afe_base_register = AFE_DL3_BASE;
            afe_end_register = AFE_DL3_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            afe_base_register = AFE_DL12_BASE;
            afe_end_register = AFE_DL12_END;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            afe_base_register = AFE_VUL_BASE;
            afe_end_register = AFE_VUL_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            afe_base_register = AFE_VUL2_BASE;
            afe_end_register = AFE_VUL2_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            afe_base_register = AFE_VUL3_BASE;
            afe_end_register = AFE_VUL3_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            afe_base_register = AFE_AWB_BASE;
            afe_end_register = AFE_AWB_END;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            afe_base_register = AFE_AWB2_BASE;
            afe_end_register = AFE_AWB2_END;
            break;
        default:
            break;
    }

    if ((afe_base_register!=0) && (afe_end_register!=0)) {
        length = AFE_GET_REG(afe_end_register) - AFE_GET_REG(afe_base_register) + 1;
    }
    return length;
}

uint32_t hal_memory_get_offset(hal_audio_agent_t agent)
{
    uint32_t afe_cur_register = 0, current_offset = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            afe_cur_register = AFE_DL1_CUR;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            afe_cur_register = AFE_DL2_CUR;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            afe_cur_register = AFE_DL3_CUR;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            afe_cur_register = AFE_DL12_CUR;
            break;

        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            afe_cur_register = AFE_VUL_CUR;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            afe_cur_register = AFE_VUL2_CUR;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            afe_cur_register = AFE_VUL3_CUR;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            afe_cur_register = AFE_AWB_CUR;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            afe_cur_register = AFE_AWB2_CUR;
            break;
        default:
            break;
    }

    if (afe_cur_register!=0) {
        current_offset = AFE_GET_REG(afe_cur_register);
    }
    return current_offset;
}

hal_audio_irq_audiosys_t hal_memory_convert_audiosys_irq_number(hal_audio_agent_t agent)
{
    hal_audio_irq_audiosys_t irq;
    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            irq = AFE_AUDIOSYS_IRQ0;
            break;
       case HAL_AUDIO_AGENT_MEMORY_DL2:
            irq = AFE_AUDIOSYS_IRQ1;
            break;
       case HAL_AUDIO_AGENT_MEMORY_DL3:
            irq = AFE_AUDIOSYS_IRQ2;
            break;
       case HAL_AUDIO_AGENT_MEMORY_DL12:    //Dump Path
            irq = AFE_AUDIOSYS_IRQ3;
            break;

       case HAL_AUDIO_AGENT_MEMORY_VUL1:
            irq = AFE_AUDIOSYS_IRQ4;
            break;
       case HAL_AUDIO_AGENT_MEMORY_VUL2:
            irq = AFE_AUDIOSYS_IRQ5;
            break;
       case HAL_AUDIO_AGENT_MEMORY_VUL3:
            irq = AFE_AUDIOSYS_IRQ11;
            break;
       case HAL_AUDIO_AGENT_MEMORY_AWB:
            irq = AFE_AUDIOSYS_IRQ6;
            break;

       case HAL_AUDIO_AGENT_MEMORY_AWB2:    //Echo Path
            irq = AFE_AUDIOSYS_IRQ7;
            break;
       case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1: //dedicate for Sub source
            irq = AFE_AUDIOSYS_IRQ12;
            break;
       default:
            irq = AFE_AUDIOSYS_IRQ_NUM;
            break;
    }
    return irq;
}

hal_audio_agent_t hal_memory_convert_agent_from_audiosys_irq_number(hal_audio_irq_audiosys_t irq_number)
{
    hal_audio_agent_t agent;
    switch (irq_number) {
        case AFE_AUDIOSYS_IRQ0:
            agent = HAL_AUDIO_AGENT_MEMORY_DL1;
            break;
       case AFE_AUDIOSYS_IRQ1:
            agent = HAL_AUDIO_AGENT_MEMORY_DL2;
            break;
       case AFE_AUDIOSYS_IRQ2:
            agent = HAL_AUDIO_AGENT_MEMORY_DL3;
            break;
       case AFE_AUDIOSYS_IRQ3:    //Dump Path
            agent = HAL_AUDIO_AGENT_MEMORY_DL12;
            break;

       case AFE_AUDIOSYS_IRQ4:
            agent = HAL_AUDIO_AGENT_MEMORY_VUL1;
            break;
       case AFE_AUDIOSYS_IRQ5:
            agent = HAL_AUDIO_AGENT_MEMORY_VUL2;
            break;
       case AFE_AUDIOSYS_IRQ11:
            agent = HAL_AUDIO_AGENT_MEMORY_VUL3;
            break;
       case AFE_AUDIOSYS_IRQ6:
            agent = HAL_AUDIO_AGENT_MEMORY_AWB;
            break;

       case AFE_AUDIOSYS_IRQ7:    //Echo Path
            agent = HAL_AUDIO_AGENT_MEMORY_AWB2;
            break;
       default:
            agent = HAL_AUDIO_AGENT_ERROR;
            break;
    }
    return agent;
}

static uint32_t hal_memory_get_irq_counter_register(hal_audio_irq_audiosys_t irq)
{
    uint32_t counter_register = 0;
    switch(irq) {
        case AFE_AUDIOSYS_IRQ0:
            counter_register = AFE_IRQ_MCU_CNT0;
            break;
        case AFE_AUDIOSYS_IRQ1:
            counter_register = AFE_IRQ_MCU_CNT1;
            break;
        case AFE_AUDIOSYS_IRQ2:
            counter_register = AFE_IRQ_MCU_CNT2;
            break;
        case AFE_AUDIOSYS_IRQ3:
            counter_register = AFE_IRQ_MCU_CNT3;
            break;
        case AFE_AUDIOSYS_IRQ4:
            counter_register = AFE_IRQ_MCU_CNT4;
            break;
        case AFE_AUDIOSYS_IRQ5:
            counter_register = AFE_IRQ_MCU_CNT5;
            break;
        case AFE_AUDIOSYS_IRQ6:
            counter_register = AFE_IRQ_MCU_CNT6;
            break;
        case AFE_AUDIOSYS_IRQ7:
            counter_register = AFE_IRQ_MCU_CNT7;
            break;
        case AFE_AUDIOSYS_IRQ11:
            counter_register = AFE_IRQ_MCU_CNT11;
            break;
        case AFE_AUDIOSYS_IRQ12:
            counter_register = AFE_IRQ_MCU_CNT12;
            break;
        default:
            break;
    }
    return counter_register;
}

bool hal_memory_set_irq_period(hal_audio_agent_t agent, uint32_t samplerate, uint32_t counter)
{
    hal_audio_irq_audiosys_t irq;
    uint32_t irq_samplerate_register, irq_samplerate_shift;

    irq = hal_memory_convert_audiosys_irq_number(agent);

    if ((irq == AFE_AUDIOSYS_IRQ9) || (irq == AFE_AUDIOSYS_IRQ14)) {
        // there is no RG for irq9/irq10 irq13/irq14
        return false;
    }

    if (irq < AFE_AUDIOSYS_IRQ_NUM) {
        if (irq >= AFE_AUDIOSYS_IRQ11) {
            irq_samplerate_register = AFE_IRQ_MCU_CON2;
            irq_samplerate_shift = (irq - AFE_AUDIOSYS_IRQ11)<<2;
        } else {
            irq_samplerate_register = AFE_IRQ_MCU_CON1;
            irq_samplerate_shift = irq<<2;
        }

        //Configure sample rate
        AFE_SET_REG(irq_samplerate_register, afe_samplerate_convert_samplerate_to_register_value(samplerate)<<irq_samplerate_shift, 0xF<<irq_samplerate_shift);

        //Configure IRQ counter
        AFE_WRITE(hal_memory_get_irq_counter_register(irq), counter); //[0:17]


    } else {

        return true;
    }
    return false;
}

bool hal_memory_set_irq_enable(hal_audio_agent_t agent, hal_audio_control_status_t control)
{
    hal_audio_irq_audiosys_t irq;

    irq = hal_memory_convert_audiosys_irq_number(agent);

    if (irq < AFE_AUDIOSYS_IRQ_NUM) {

        if (control == HAL_AUDIO_CONTROL_ON) {
            //Configure en
            AFE_SET_REG(AFE_IRQ_MCU_EN1, control<<irq, 1<<irq);

            //Configure on
            AFE_SET_REG(AFE_IRQ_MCU_CON0, control<<irq, 1<<irq);
        } else {
            HAL_AUDIO_ENTER_CRITICAL();

            //Configure off
            AFE_SET_REG(AFE_IRQ_MCU_CON0, control<<irq, 1<<irq);

            HAL_AUDIO_DELAY_US(1);

            // Clear irq status
            uint32_t clear_mask = (1<<irq)|(1<<(16+irq));
            AFE_SET_REG(AFE_IRQ_MCU_CLR, clear_mask, clear_mask);

            HAL_AUDIO_EXIT_CRITICAL();

            HAL_AUDIO_DELAY_US(1);

            //Configure disable
            AFE_SET_REG(AFE_IRQ_MCU_EN1, control<<irq, 1<<irq);
        }

        HAL_AUDIO_LOG_INFO("DSP - Hal Audio irq agent:%d, Off/On:%d", 2, agent, control);

    } else {

        return true;
    }
    return false;
}

afe_audio_bt_sync_con0_t afe_get_bt_sync_enable_bit (hal_audio_agent_t agent)
{
    afe_audio_bt_sync_con0_t bt_sync_bit = 0;

    switch (agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            bt_sync_bit = AFE_AUDIO_BT_SYNC_DL1;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            bt_sync_bit = AFE_AUDIO_BT_SYNC_DL2;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            bt_sync_bit = AFE_AUDIO_BT_SYNC_DL3;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            bt_sync_bit = AFE_AUDIO_BT_SYNC_DL12;
            break;
        default:
            break;
    }
    return bt_sync_bit;
}

bool hal_memory_set_palyen_counter(hal_audio_control_status_t control)
{
    if (control == HAL_AUDIO_CONTROL_ON) {
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x1000,    0x1000);            //reset the xtal_cnt to zero
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0000,    0x1000);

        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0100,    0x0100);            //xtal_cnt powered on
    #if 1
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0000,    0x0011);            //audio_get_cnt_en & audio_play_en from BT
    #else
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0033,    0x0033);            //audio_play_en controlled by audio_play_en_int
    #endif
    } else {
        AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0000,    0x0100);            //xtal_cnt powered off
    }
    return false;
}

bool hal_memory_set_palyen(hal_audio_agent_t agent, hal_audio_control_status_t control)
{
    afe_audio_bt_sync_con0_t dl_agent_bit = afe_get_bt_sync_enable_bit(agent);

    if (dl_agent_bit != 0) {
        if (control == HAL_AUDIO_CONTROL_ON) {
            AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0x0600|dl_agent_bit, 0x0600|dl_agent_bit);
            hal_memory_set_palyen_counter(control);

        } else {
            AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  ~dl_agent_bit, dl_agent_bit);
            if (!(AFE_READ(AFE_AUDIO_BT_SYNC_CON0)&AFE_AUDIO_BT_SYNC_ENABLE)) {
                AFE_SET_REG(AFE_AUDIO_BT_SYNC_CON0,  0, 0x0600);
                hal_memory_set_palyen_counter(control);
            }

        }
    } else {
        return true;
    }
    return false;
}

uint32_t hal_memory_get_palyen_monitor(hal_audio_agent_t agent)
{
    uint32_t monitor_value, monitor_state;

    monitor_value = AFE_GET_REG(AFE_AUDIO_BT_SYNC_MON1);
    monitor_state = AFE_GET_REG(AFE_AUDIO_BT_SYNC_MON2);

    if (agent == HAL_AUDIO_AGENT_MEMORY_DL1) {
        monitor_state &= AFE_AUDIO_BT_SYNC_MON2_DL1_SETTED_MASK;
    } else if (agent == HAL_AUDIO_AGENT_MEMORY_DL2) {
        monitor_state &= AFE_AUDIO_BT_SYNC_MON2_DL2_SETTED_MASK;
    } else if (agent == HAL_AUDIO_AGENT_MEMORY_DL3) {
        monitor_state &= AFE_AUDIO_BT_SYNC_MON2_DL3_SETTED_MASK;
    } else if (agent == HAL_AUDIO_AGENT_MEMORY_DL12) {
        monitor_state &= AFE_AUDIO_BT_SYNC_MON2_DL12_SETTED_MASK;
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning invalid palyen monitor agent:%d @@", 1, agent);
    }

    if (monitor_state) {
        if (agent == HAL_AUDIO_AGENT_MEMORY_DL1) {
            monitor_value = monitor_value&AFE_AUDIO_BT_SYNC_MON1_DL1_MONITOR_MASK;
        } else if (agent == HAL_AUDIO_AGENT_MEMORY_DL2) {
            monitor_value = (monitor_value&AFE_AUDIO_BT_SYNC_MON1_DL2_MONITOR_MASK)>>AFE_AUDIO_BT_SYNC_MON1_DL2_MONITOR_POS;
        }
    } else {
        monitor_value = 0;
    }

    return monitor_value;
}

/*******************************************************************************************
*                                            SRC                                           *
********************************************************************************************/
#define MEMASRC_IIR_COEF_SIZE (48)

//IIR_COEF_384_TO_192
static const uint32_t afe_src_iir_coef_2_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x04ec93a4, 0x000a1bf6, 0x04ec93a4, 0x1a59368b, 0xc11e7000, 0x00000001, 0x0b9bf8b5, 0x00b44721,
    0x0b9bf8b5, 0x0e62af3f, 0xe1c4dfac, 0x00000002, 0x09fe478b, 0x01c85505, 0x09fe478b, 0x112b48ef,
    0xe34aeed8, 0x00000002, 0x07b86f3c, 0x02fd7211, 0x07b86f3c, 0x15cb1caf, 0xe558423d, 0x00000002,
    0x0a1e67fb, 0x0754a987, 0x0a1e67fb, 0x1c63a076, 0xe80ae60d, 0x00000002, 0x0522203a, 0x0635518b,
    0x0522203a, 0x245624f0, 0xeb304910, 0x00000002, 0x01bfb83a, 0x030c2050, 0x01bfb83a, 0x2b7a0a3d,
    0xedfa64fe, 0x00000002, 0x00000000, 0x2c9308ca, 0x2c9308ca, 0x02e75633, 0x00000000, 0x00000005
};

//IIR_COEF_384_TO_128
static const uint32_t afe_src_iir_coef_3_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x02894a52, 0xfd770f7a, 0x02894a52, 0x27008598, 0xe06656ba, 0x00000002, 0x0b4d22b9, 0xf51b5d44,
    0x0b4d22b9, 0x274e7f44, 0xe1463b51, 0x00000002, 0x0976ef3e, 0xf7ac51fb, 0x0976ef3e, 0x28672e65,
    0xe26470ac, 0x00000002, 0x06e6a0fe, 0xfb12646f, 0x06e6a0fe, 0x2a5a082f, 0xe3e6aafa, 0x00000002,
    0x081c5ea5, 0xfcccf1de, 0x081c5ea5, 0x2d20d986, 0xe5d97751, 0x00000002, 0x036a7c7a, 0x00c2723e,
    0x036a7c7a, 0x305e8d68, 0xe80a0765, 0x00000002, 0x00e763b5, 0x0123b6ba, 0x00e763b5, 0x332864f5,
    0xe9e51ce6, 0x00000002, 0x00000000, 0x38c17ba1, 0x38c17ba1, 0x068927ea, 0x00000000, 0x00000004
};

//IIR_COEF_384_TO_256
static const uint32_t afe_src_iir_coef_3_to_2[MEMASRC_IIR_COEF_SIZE] = {
    0x0539819f, 0x054951e5, 0x0539819f, 0xda57122a, 0xc12ba871, 0x00000001, 0x0cf35b0b, 0x0da330e8,
    0x0cf35b0b, 0xef0f9d73, 0xe1dc9491, 0x00000002, 0x0bb8d79a, 0x0d5e3977, 0x0bb8d79a, 0xf2db2d6f,
    0xe385013c, 0x00000002, 0x09e15952, 0x0ca91405, 0x09e15952, 0xf9444c2e, 0xe5e42680, 0x00000002,
    0x0ecd2494, 0x15e07e27, 0x0ecd2494, 0x03307784, 0xe954c12e, 0x00000002, 0x08f68ac2, 0x0f542aa7,
    0x08f68ac2, 0x10cf62d9, 0xedef5cfc, 0x00000002, 0x074829ae, 0x0df259cb, 0x074829ae, 0x3e16cf8a,
    0xe566834f, 0x00000001, 0x00000000, 0x2263d8b7, 0x2263d8b7, 0x012d626d, 0x00000000, 0x00000006
};

//IIR_COEF_384_TO_96
static const uint32_t afe_src_iir_coef_4_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x023fb27b, 0xfcd2f985, 0x023fb27b, 0x326cca6b, 0xe056495c, 0x00000002, 0x0a37a3a1, 0xf1d5e48b,
    0x0a37a3a1, 0x3265763d, 0xe110322b, 0x00000002, 0x084e4d06, 0xf4ffdd82, 0x084e4d06, 0x32d2c1e7,
    0xe1f44efd, 0x00000002, 0x05c29422, 0xf91777ea, 0x05c29422, 0x33ae7ff0, 0xe3183fb3, 0x00000002,
    0x064224ff, 0xfa1849cc, 0x064224ff, 0x34e75649, 0xe47c15ec, 0x00000002, 0x0253663f, 0xff1b8654,
    0x0253663f, 0x36483578, 0xe5f577b6, 0x00000002, 0x00816bda, 0x006b4ca3, 0x00816bda, 0x376c1778,
    0xe725c430, 0x00000002, 0x00000000, 0x28b0e793, 0x28b0e793, 0x06fbe6c3, 0x00000000, 0x00000004
};

//IIR_COEF_256_TO_192
static const uint32_t afe_src_iir_coef_4_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x02c5a70f, 0x03eb90a0, 0x02c5a70f, 0xdc8ebb5e, 0xe08256c1, 0x00000002, 0x0df345b2, 0x141cbadb,
    0x0df345b2, 0xde56eee1, 0xe1a284a2, 0x00000002, 0x0d03b121, 0x138292e8, 0x0d03b121, 0xe1c608f0,
    0xe3260cbe, 0x00000002, 0x0b8948fb, 0x1254a1d4, 0x0b8948fb, 0xe7c0c6a5, 0xe570d1c6, 0x00000002,
    0x12a7cba4, 0x1fe15ef0, 0x12a7cba4, 0xf1bd349d, 0xe911d52a, 0x00000002, 0x0c882b79, 0x1718bc3a,
    0x0c882b79, 0x013ec33c, 0xee982998, 0x00000002, 0x0b5bd89b, 0x163580e0, 0x0b5bd89b, 0x2873f220,
    0xea9edbcb, 0x00000001, 0x00000000, 0x2c155c70, 0x2c155c70, 0x00f204d6, 0x00000000, 0x00000006
};

//IIR_COEF_384_TO_64
static const uint32_t afe_src_iir_coef_6_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x029d40ba, 0xfb79d6e0, 0x029d40ba, 0x392d8096, 0xe034dd0f, 0x00000002, 0x05a2a7fa, 0xf64f6c2e,
    0x05a2a7fa, 0x1c817399, 0xf0551425, 0x00000003, 0x0953d637, 0xf036ffec, 0x0953d637, 0x38ff5268,
    0xe14354e5, 0x00000002, 0x06952665, 0xf53ce19a, 0x06952665, 0x391c1353, 0xe2158fe4, 0x00000002,
    0x072672d6, 0xf538294e, 0x072672d6, 0x39539b19, 0xe32755ee, 0x00000002, 0x027d1bb9, 0xfd10e42d,
    0x027d1bb9, 0x399a1610, 0xe45ace52, 0x00000002, 0x006a8eb3, 0xfff50a50, 0x006a8eb3, 0x39d826e9,
    0xe55db161, 0x00000002, 0x00000000, 0x1c6ede8b, 0x1c6ede8b, 0x073e2bc6, 0x00000000, 0x00000004
};

//IIR_COEF_384_TO_48
static const uint32_t afe_src_iir_coef_8_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x02d975a0, 0xfabc9d25, 0x02d975a0, 0x3bece279, 0xe024cd2f, 0x00000002, 0x05f3612a, 0xf50b0a4f,
    0x05f3612a, 0x1de062d7, 0xf03c03e5, 0x00000003, 0x04ff0085, 0xf6df7cf4, 0x04ff0085, 0x1dd063ae,
    0xf074a058, 0x00000003, 0x072ec2f0, 0xf31a41fd, 0x072ec2f0, 0x3b84724d, 0xe18bfdf7, 0x00000002,
    0x07ec0168, 0xf2572ea5, 0x07ec0168, 0x3b67118c, 0xe269bd00, 0x00000002, 0x02ba0b16, 0xfbd5e151,
    0x02ba0b16, 0x3b48b8a8, 0xe36d4fda, 0x00000002, 0x00662a40, 0xffb4d0a9, 0x00662a40, 0x3b2fa355,
    0xe44f3782, 0x00000002, 0x00000000, 0x2bc64b7d, 0x2bc64b7d, 0x0ec96668, 0x00000000, 0x00000003
};

//IIR_COEF_256_TO_96
static const uint32_t afe_src_iir_coef_8_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x02545a7f, 0xfe3ea711, 0x02545a7f, 0x2238ae5c, 0xe07931d4, 0x00000002, 0x0ac115ca, 0xf860f5e9,
    0x0ac115ca, 0x22bbb3c8, 0xe17e093d, 0x00000002, 0x08f408eb, 0xfa958f42, 0x08f408eb, 0x244477a8,
    0xe2c04629, 0x00000002, 0x0682ca04, 0xfd584bf8, 0x0682ca04, 0x26e0e9c4, 0xe463563b, 0x00000002,
    0x07baafe9, 0xff9d399b, 0x07baafe9, 0x2a7e74b7, 0xe66ef1dd, 0x00000002, 0x0362b43a, 0x01f07186,
    0x0362b43a, 0x2e9e4f86, 0xe8abd681, 0x00000002, 0x00f873cc, 0x016b4c05, 0x00f873cc, 0x321b967a,
    0xea8835e8, 0x00000002, 0x00000000, 0x1f82bf79, 0x1f82bf79, 0x03381d0d, 0x00000000, 0x00000005
};

//IIR_COEF_352_TO_32
static const uint32_t afe_src_iir_coef_11_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x0314039e, 0xfa16c22d, 0x0314039e, 0x3dc6a869, 0xe0185784, 0x00000002, 0x063c1999, 0xf40b68ae,
    0x063c1999, 0x1ed08ff8, 0xf0283846, 0x00000003, 0x0550267e, 0xf5d9545e, 0x0550267e, 0x1ebccb61,
    0xf04ff1ec, 0x00000003, 0x07cc8c28, 0xf137f60c, 0x07cc8c28, 0x3d467c42, 0xe1176705, 0x00000002,
    0x08cb7062, 0xefa566fe, 0x08cb7062, 0x3d01dd91, 0xe1c1daac, 0x00000002, 0x030c40f1, 0xfaa57571,
    0x030c40f1, 0x3cafcf6a, 0xe2923943, 0x00000002, 0x00685a9f, 0xff7ab6e7, 0x00685a9f, 0x3c665fb6,
    0xe34e3424, 0x00000002, 0x00000000, 0x2046052b, 0x2046052b, 0x0f11f8dd, 0x00000000, 0x00000003
};

//IIR_COEF_352_TO_64
static const uint32_t afe_src_iir_coef_11_to_2[MEMASRC_IIR_COEF_SIZE] = {
    0x02a6b37b, 0xfb888156, 0x02a6b37b, 0x37f859ff, 0xe0386456, 0x00000002, 0x05b20926, 0xf679dff1,
    0x05b20926, 0x1be93c6e, 0xf05adf16, 0x00000003, 0x09753fb7, 0xf07a0567, 0x09753fb7, 0x37dcb603,
    0xe15a4054, 0x00000002, 0x06b68dd9, 0xf56b6f20, 0x06b68dd9, 0x3811806b, 0xe23d69ee, 0x00000002,
    0x075ca584, 0xf5743cd8, 0x075ca584, 0x386b6017, 0xe367180a, 0x00000002, 0x029bfc8d, 0xfd367105,
    0x029bfc8d, 0x38da2ce4, 0xe4b768fc, 0x00000002, 0x007521f2, 0x0006baaa, 0x007521f2, 0x393b06d9,
    0xe5d3fa99, 0x00000002, 0x00000000, 0x1ee87e92, 0x1ee87e92, 0x072c4a47, 0x00000000, 0x00000004
};

//IIR_COEF_352_TO_96
static const uint32_t afe_src_iir_coef_11_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x0217fde7, 0xfd41917c, 0x0217fde7, 0x30c91f13, 0xe060d9b0, 0x00000002, 0x09b6b0aa, 0xf39321ee,
    0x09b6b0aa, 0x30cfc632, 0xe12f3348, 0x00000002, 0x07d62437, 0xf68e9f4a, 0x07d62437, 0x3163f76d,
    0xe2263924, 0x00000002, 0x05647e38, 0xfa4bf362, 0x05647e38, 0x327c9f18, 0xe3598145, 0x00000002,
    0x05d9333f, 0xfb86a733, 0x05d9333f, 0x3400e913, 0xe4c6093b, 0x00000002, 0x02366680, 0xffa5d34a,
    0x02366680, 0x35ada3bf, 0xe63fbbf7, 0x00000002, 0x0082c4df, 0x0084bd96, 0x0082c4df, 0x370ab423,
    0xe76b048a, 0x00000002, 0x00000000, 0x2b47dcde, 0x2b47dcde, 0x06f26612, 0x00000000, 0x00000004
};

//IIR_COEF_352_TO_128
static const uint32_t afe_src_iir_coef_11_to_4[MEMASRC_IIR_COEF_SIZE] = {
    0x02449fa8, 0xfe2712a2, 0x02449fa8, 0x24311459, 0xe0787f65, 0x00000002, 0x0a84fc5d, 0xf7e60ffe,
    0x0a84fc5d, 0x24a509fa, 0xe17ae496, 0x00000002, 0x08b52aad, 0xfa3093f6, 0x08b52aad, 0x2612ff28,
    0xe2b72e39, 0x00000002, 0x064680dc, 0xfd0ab6a4, 0x064680dc, 0x2882e116, 0xe44dae31, 0x00000002,
    0x075b08c6, 0xff2a08c3, 0x075b08c6, 0x2bdcc672, 0xe6431f3f, 0x00000002, 0x032b3b9b, 0x01a2f50e,
    0x032b3b9b, 0x2fa5cc8a, 0xe860c732, 0x00000002, 0x00e4486e, 0x01476fb8, 0x00e4486e, 0x32d1c2b2,
    0xea1e3cba, 0x00000002, 0x00000000, 0x3c8f8f79, 0x3c8f8f79, 0x0682ad88, 0x00000000, 0x00000004
};

//IIR_COEF_352_TO_192
static const uint32_t afe_src_iir_coef_11_to_6[MEMASRC_IIR_COEF_SIZE] = {
    0x04bd6a83, 0x015bba25, 0x04bd6a83, 0x0d7cdab3, 0xc137e9aa, 0x00000001, 0x0b80613f, 0x03ed2155,
    0x0b80613f, 0x0839ad63, 0xe1ea8af7, 0x00000002, 0x09f38e84, 0x049744b0, 0x09f38e84, 0x0b793c54,
    0xe38a003b, 0x00000002, 0x0f90a19e, 0x0a5f3f69, 0x0f90a19e, 0x21996102, 0xcb6599b5, 0x00000001,
    0x0a71465a, 0x0a3901b2, 0x0a71465a, 0x185f028c, 0xe8856f0e, 0x00000002, 0x057f4d69, 0x07aa76ed,
    0x057f4d69, 0x2185beab, 0xebd12f96, 0x00000002, 0x01f3ece3, 0x0389a7a8, 0x01f3ece3, 0x29ccd8d7,
    0xeec1a5bb, 0x00000002, 0x00000000, 0x30ea5629, 0x30ea5629, 0x02d46d34, 0x00000000, 0x00000005
};

//IIR_COEF_352_TO_256
static const uint32_t afe_src_iir_coef_11_to_8[MEMASRC_IIR_COEF_SIZE] = {
    0x02b9c432, 0x038d00b6, 0x02b9c432, 0xe15c8a85, 0xe0898362, 0x00000002, 0x0da7c09d, 0x12390f60,
    0x0da7c09d, 0xe3369d52, 0xe1b8417b, 0x00000002, 0x0ca047cb, 0x11b06e35, 0x0ca047cb, 0xe6d2ee1d,
    0xe34b164a, 0x00000002, 0x0b06ae31, 0x109ad63c, 0x0b06ae31, 0xed0d2221, 0xe5a278a0, 0x00000002,
    0x116c5a39, 0x1cb58b13, 0x116c5a39, 0xf7341949, 0xe93da732, 0x00000002, 0x0b5486de, 0x147ddf59,
    0x0b5486de, 0x0657d3a0, 0xee813f4a, 0x00000002, 0x09e7f722, 0x13449867, 0x09e7f722, 0x2fcbde4b,
    0xe91f9b09, 0x00000001, 0x00000000, 0x28cd51d5, 0x28cd51d5, 0x01061d85, 0x00000000, 0x00000006
};

//IIR_COEF_384_TO_32
static const uint32_t afe_src_iir_coef_12_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x032f10f1, 0xf9d95961, 0x032f10f1, 0x3e1158bc, 0xe01583e6, 0x00000002, 0x065bac8d, 0xf3bb21b9,
    0x065bac8d, 0x1ef747e9, 0xf023c270, 0x00000003, 0x0574b803, 0xf580c27d, 0x0574b803, 0x1ee3c915,
    0xf047d1e6, 0x00000003, 0x08179a23, 0xf0885da5, 0x08179a23, 0x3d927e61, 0xe0fe5dc7, 0x00000002,
    0x093e2f0c, 0xee9b6c3c, 0x093e2f0c, 0x3d48a322, 0xe19f928b, 0x00000002, 0x033d2e85, 0xfa2f0f19,
    0x033d2e85, 0x3cedff76, 0xe2689466, 0x00000002, 0x006ded19, 0xff67ddb5, 0x006ded19, 0x3c9b5197,
    0xe320f682, 0x00000002, 0x00000000, 0x1e1581e2, 0x1e1581e2, 0x0f1e283c, 0x00000000, 0x00000003
};

//IIR_COEF_384_TO_352
static const uint32_t afe_src_iir_coef_12_to_11[MEMASRC_IIR_COEF_SIZE] = {
    0x0303b6d0, 0x05cdf456, 0x0303b6d0, 0xc416d6b5, 0xe0363059, 0x00000002, 0x07cc27b7, 0x0f0ba03c,
    0x07cc27b7, 0xe25f513e, 0xf058cf70, 0x00000003, 0x07aa57c1, 0x0eda62f1, 0x07aa57c1, 0xe2f03496,
    0xf0b1a683, 0x00000003, 0x076f8c07, 0x0e7fe0ff, 0x076f8c07, 0xe3f982eb, 0xf1488afb, 0x00000003,
    0x0e01e7ef, 0x1b8868ec, 0x0e01e7ef, 0xe607f80e, 0xf26bcf29, 0x00000003, 0x0c3237da, 0x182c4952,
    0x0c3237da, 0xea8a804f, 0xf4e4c6ab, 0x00000003, 0x1082b5c7, 0x20f0495b, 0x1082b5c7, 0xe93bbad7,
    0xf4ce9040, 0x00000002, 0x00000000, 0x2b58b702, 0x2b58b702, 0xfff6882b, 0x00000000, 0x00000007
};

//IIR_COEF_384_TO_24
static const uint32_t afe_src_iir_coef_16_to_1[MEMASRC_IIR_COEF_SIZE] = {
    0x038fcda4, 0xf9036a1f, 0x038fcda4, 0x3eda69b0, 0xe00d87a5, 0x00000002, 0x06bff5be, 0xf2c44847,
    0x06bff5be, 0x1f609be0, 0xf016fc9b, 0x00000003, 0x05eecea4, 0xf46276e0, 0x05eecea4, 0x1f4fdbce,
    0xf02ffbe3, 0x00000003, 0x0490e7d9, 0xf716aef0, 0x0490e7d9, 0x1f35bb3e, 0xf059477f, 0x00000003,
    0x05790864, 0xf56662d5, 0x05790864, 0x1f0d7f0d, 0xf09a0d56, 0x00000003, 0x0405a7b9, 0xf863223b,
    0x0405a7b9, 0x3dafa23c, 0xe1e1ec18, 0x00000002, 0x0087c12c, 0xff1dcc3e, 0x0087c12c, 0x3d46e81a,
    0xe28bc951, 0x00000002, 0x00000000, 0x2e64814a, 0x2e64814a, 0x1e8c999c, 0x00000000, 0x00000002
};

//IIR_COEF_256_TO_48
static const uint32_t afe_src_iir_coef_16_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x02b72fb4, 0xfb7c5152, 0x02b72fb4, 0x374ab8ef, 0xe039095c, 0x00000002, 0x05ca62de, 0xf673171b,
    0x05ca62de, 0x1b94186a, 0xf05c2de7, 0x00000003, 0x09a9656a, 0xf05ffe29, 0x09a9656a, 0x37394e81,
    0xe1611f87, 0x00000002, 0x06e86c29, 0xf54bf713, 0x06e86c29, 0x37797f41, 0xe24ce1f6, 0x00000002,
    0x07a6b7c2, 0xf5491ea7, 0x07a6b7c2, 0x37e40444, 0xe3856d91, 0x00000002, 0x02bf8a3e, 0xfd2f5fa6,
    0x02bf8a3e, 0x38673190, 0xe4ea5a4d, 0x00000002, 0x007e1bd5, 0x000e76ca, 0x007e1bd5, 0x38da5414,
    0xe61afd77, 0x00000002, 0x00000000, 0x2038247b, 0x2038247b, 0x07212644, 0x00000000, 0x00000004
};

//IIR_COEF_352_TO_48
static const uint32_t afe_src_iir_coef_22_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x02f3c075, 0xfaa0f2bb, 0x02f3c075, 0x3b1e2ce5, 0xe0266497, 0x00000002, 0x061654a7, 0xf4f6e06e,
    0x061654a7, 0x1d7a0223, 0xf03eea63, 0x00000003, 0x0525c445, 0xf6c06638, 0x0525c445, 0x1d6cc79b,
    0xf07b5ae0, 0x00000003, 0x077bc6f3, 0xf2d1482b, 0x077bc6f3, 0x3ac6a73a, 0xe1a7aca5, 0x00000002,
    0x0861aac3, 0xf1e8c6c3, 0x0861aac3, 0x3ab6aa60, 0xe29d3957, 0x00000002, 0x02f20c88, 0xfbb246f6,
    0x02f20c88, 0x3aa813c9, 0xe3c18c32, 0x00000002, 0x0072d6df, 0xffba3768, 0x0072d6df, 0x3a9ca779,
    0xe4c37362, 0x00000002, 0x00000000, 0x2ffa2764, 0x2ffa2764, 0x0ea60a6b, 0x00000000, 0x00000003
};

//IIR_COEF_384_TO_176
static const uint32_t afe_src_iir_coef_24_to_11[MEMASRC_IIR_COEF_SIZE] = {
    0x04c0f379, 0xfedb44d2, 0x04c0f379, 0x298f4134, 0xc1174e69, 0x00000001, 0x0b2dc719, 0xfde5dcb2,
    0x0b2dc719, 0x15c4fe25, 0xe1b82c15, 0x00000002, 0x097dabd2, 0xff571c5d, 0x097dabd2, 0x182d7cb1,
    0xe32d9b4e, 0x00000002, 0x072993e5, 0x01099884, 0x072993e5, 0x1c2c2780, 0xe51a57e5, 0x00000002,
    0x090f2fa1, 0x048cdc85, 0x090f2fa1, 0x21c3b5c2, 0xe7910e77, 0x00000002, 0x04619e1a, 0x0491e3db,
    0x04619e1a, 0x28513621, 0xea59a9d0, 0x00000002, 0x016b9a38, 0x0261dc20, 0x016b9a38, 0x2e08c9d1,
    0xecbe259f, 0x00000002, 0x00000000, 0x27d8fdc3, 0x27d8fdc3, 0x03060265, 0x00000000, 0x00000005
};

//IIR_COEF_352_TO_24
static const uint32_t afe_src_iir_coef_44_to_3[MEMASRC_IIR_COEF_SIZE] = {
    0x035e047f, 0xf96b61f8, 0x035e047f, 0x3ea9e77e, 0xe0103d3f, 0x00000002, 0x068ecd54, 0xf331816d,
    0x068ecd54, 0x1f468083, 0xf01b4754, 0x00000003, 0x05b1d20a, 0xf4e638ca, 0x05b1d20a, 0x1f347549,
    0xf037d0fd, 0x00000003, 0x0899975f, 0xef4dc655, 0x0899975f, 0x3e339e50, 0xe0ca7788, 0x00000002,
    0x0506416b, 0xf657152f, 0x0506416b, 0x1ef29626, 0xf0a9d1d5, 0x00000003, 0x0397fd6f, 0xf94a745b,
    0x0397fd6f, 0x3d80a443, 0xe204ec85, 0x00000002, 0x007807fd, 0xff417ee3, 0x007807fd, 0x3d21cefe,
    0xe2aca225, 0x00000002, 0x00000000, 0x1902d021, 0x1902d021, 0x0f3e4f5a, 0x00000000, 0x00000003
};

//IIR_COEF_384_TO_88
static const uint32_t afe_src_iir_coef_48_to_11[MEMASRC_IIR_COEF_SIZE] = {
    0x02a62553, 0xfc064b8f, 0x02a62553, 0x334dac81, 0xe046a1ac, 0x00000002, 0x0b7129c3, 0xef0c6c3b,
    0x0b7129c3, 0x333f4ca6, 0xe0e333d7, 0x00000002, 0x0988a73e, 0xf251f92c, 0x0988a73e, 0x338970d0,
    0xe1afffe0, 0x00000002, 0x06d3ff07, 0xf6dd0d45, 0x06d3ff07, 0x342bbad5, 0xe2ca2e85, 0x00000002,
    0x07a6625e, 0xf756da5d, 0x07a6625e, 0x3520a02f, 0xe43bc0b9, 0x00000002, 0x02dcefe5, 0xfe24470b,
    0x02dcefe5, 0x364422e9, 0xe5ddb642, 0x00000002, 0x0095d0b1, 0x00542b8f, 0x0095d0b1, 0x374028ca,
    0xe7400a46, 0x00000002, 0x00000000, 0x27338238, 0x27338238, 0x06f4c3b3, 0x00000000, 0x00000004
};

//IIR_COEF_384_TO_44
static const uint32_t afe_src_iir_coef_96_to_11[MEMASRC_IIR_COEF_SIZE] = {
    0x03039659, 0xfa5c23d4, 0x03039659, 0x3c785635, 0xe01fb163, 0x00000002, 0x0628eac7, 0xf4805b4c,
    0x0628eac7, 0x1e27797e, 0xf03424d0, 0x00000003, 0x053a5bfc, 0xf64cf807, 0x053a5bfc, 0x1e15b5a9,
    0xf066ea58, 0x00000003, 0x07a260b2, 0xf206beb1, 0x07a260b2, 0x3c048d2d, 0xe16472aa, 0x00000002,
    0x0892f686, 0xf0ccdf29, 0x0892f686, 0x3bd501d2, 0xe23831f8, 0x00000002, 0x02fd4044, 0xfb2f4ecd,
    0x02fd4044, 0x3b9ec25d, 0xe3376e4d, 0x00000002, 0x006d4eef, 0xff9b0b4d, 0x006d4eef, 0x3b6f651a,
    0xe41af1bb, 0x00000002, 0x00000000, 0x28bbbe8a, 0x28bbbe8a, 0x0ed6fd4e, 0x00000000, 0x00000003
};

static const afe_src_iir_coefficient_t afe_src_iir_coef_maping_table[] = {
    { 2, 1, afe_src_iir_coef_2_to_1},
    { 3, 1, afe_src_iir_coef_3_to_1},
    { 3, 2, afe_src_iir_coef_3_to_2},
    { 4, 1, afe_src_iir_coef_4_to_1},
    { 4, 3, afe_src_iir_coef_4_to_3},
    { 6, 1, afe_src_iir_coef_6_to_1},
    { 8, 1, afe_src_iir_coef_8_to_1},
    { 8, 3, afe_src_iir_coef_8_to_3},
    {11, 1, afe_src_iir_coef_11_to_1},
    {11, 2, afe_src_iir_coef_11_to_2},

    {11, 3, afe_src_iir_coef_11_to_3},
    {11, 4, afe_src_iir_coef_11_to_4},
    {11, 6, afe_src_iir_coef_11_to_6},
    {11, 8, afe_src_iir_coef_11_to_8},
    {12, 1, afe_src_iir_coef_12_to_1},
    {12,11, afe_src_iir_coef_12_to_11},
    {16, 1, afe_src_iir_coef_16_to_1},
    {16, 3, afe_src_iir_coef_16_to_3},
    {22, 3, afe_src_iir_coef_22_to_3},
    {24,11, afe_src_iir_coef_24_to_11},

    {44, 3, afe_src_iir_coef_44_to_3},
    {48,11, afe_src_iir_coef_48_to_11},
    {96,11, afe_src_iir_coef_96_to_11},
};

static const int16_t afe_ul_biquad_table_100[] = {
     32304, //biquad1_a1 (negative)
    -15925, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32768, //biquad1_b1
     16384, //biquad1_b2
     32696, //biquad2_a1 (negative)
    -16314, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32768, //biquad2_b1
     16383, //biquad2_b2
};

static const int16_t afe_ul_biquad_table_125[] = {
     32188, //biquad1_a1 (negative)
    -15812, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32768, //biquad1_b1
     16383, //biquad1_b2
     32677, //biquad2_a1 (negative)
    -16296, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32767, //biquad2_b1
     16384, //biquad2_b2
};

static const int16_t afe_ul_biquad_table_150[] = {
     32072, //biquad1_a1 (negative)
    -15701, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32768, //biquad1_b1
     16383, //biquad1_b2
     32657, //biquad2_a1 (negative)
    -16279, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32766, //biquad2_b1
     16384, //biquad2_b2
};

static const int16_t afe_ul_biquad_table_175[] = {
     31956, //biquad1_a1 (negative)
    -15590, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32768, //biquad1_b1
     16383, //biquad1_b2
     32638, //biquad2_a1 (negative)
    -16262, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32765, //biquad2_b1
     16384, //biquad2_b2
};

static const int16_t afe_ul_biquad_table_200[] = {
     31840, //biquad1_a1 (negative)
    -15479, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32768, //biquad1_b1
     16383, //biquad1_b2
     32617, //biquad2_a1 (negative)
    -16244, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32765, //biquad2_b1
     16384, //biquad2_b2
};

static const int16_t afe_ul_biquad_table_225[] = {
     31725, //biquad1_a1 (negative)
    -15370, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32768, //biquad1_b1
     16383, //biquad1_b2
     32597, //biquad2_a1 (negative)
    -16227, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32763, //biquad2_b1
     16384, //biquad2_b2
};

static const int16_t afe_ul_biquad_table_250[] = {
     31609, //biquad1_a1 (negative)
    -15261, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32767, //biquad1_b1
     16384, //biquad1_b2
     32576, //biquad2_a1 (negative)
    -16210, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32762, //biquad2_b1
     16383, //biquad2_b2
};

static const int16_t afe_ul_biquad_table_275[] = {
     31493, //biquad1_a1 (negative)
    -15153, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32767, //biquad1_b1
     16383, //biquad1_b2
     32555, //biquad2_a1 (negative)
    -16193, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32761, //biquad2_b1
     16384, //biquad2_b2
};

static const int16_t afe_ul_biquad_table_300[] = {
     31378, //biquad1_a1 (negative)
    -15046, //biquad1_a2 (negative)
     16384, //biquad1_b0
    -32767, //biquad1_b1
     16383, //biquad1_b2
     32533, //biquad2_a1 (negative)
    -16175, //biquad2_a2 (negative)
     16384, //biquad2_b0
    -32760, //biquad2_b1
     16384, //biquad2_b2
};



/*Calculate greatest common factor*/
uint32_t hal_audio_get_gcd(uint32_t m, uint32_t n)
{
    while(n != 0) {
        uint32_t r = m % n;
        m = n;
        n = r;
    }
    return m;
}

static const uint32_t *hal_src_get_iir_coef(uint32_t input_rate, uint32_t output_rate, uint32_t *count)
{
    uint32_t in_ratio, out_ratio, gct, i;
    const uint32_t *coef_ptr = NULL;

    if (input_rate == 44100) {
        input_rate = 44000;
    }
    if (output_rate == 44100) {
        output_rate = 44000;
    }
    if (input_rate == 88200) {
        input_rate = 88000;
    }
    if (output_rate == 88200) {
        output_rate = 88000;
    }
    gct = hal_audio_get_gcd(input_rate, output_rate);
    in_ratio = input_rate/gct;
    out_ratio = output_rate/gct;
    if (in_ratio > out_ratio) {
        for (i = 0 ; i< ARRAY_SIZE(afe_src_iir_coef_maping_table) ; i++) {
            if ((afe_src_iir_coef_maping_table[i].in_ratio == in_ratio) && (afe_src_iir_coef_maping_table[i].out_ratio == out_ratio)) {
                coef_ptr = afe_src_iir_coef_maping_table[i].coef;
                break;
            }
        }
        if (coef_ptr != NULL) {
            *count = MEMASRC_IIR_COEF_SIZE;
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio SRC IIR in:%d, out:%d, coef:0x%x ", 3, in_ratio, out_ratio, (uint32_t)coef_ptr);

        } else {
            HAL_AUDIO_LOG_WARNING("DSP - Warning asrc unsupported ratio in:%d, out:%d @@", 2, in_ratio, out_ratio);
        }
    }

    return coef_ptr;
}


bool hal_src_set_iir(afe_asrc_id_t asrc_id, uint32_t input_rate, uint32_t output_rate)
{
    const uint32_t *coef_ptr;
    uint32_t coef_count = 0, addr_offset;
    addr_offset = asrc_id * 0x100;
    coef_ptr = hal_src_get_iir_coef(input_rate, output_rate, &coef_count);
    if (coef_ptr) {
        uint32_t i;

        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, ((coef_count/6 -1 )<<ASM_CH01_CNFG_IIR_STAGE_POS), ASM_CH01_CNFG_IIR_STAGE_MASK);    /* set IIR_stage-1 */

        /* turn on IIR coef setting path */
        AFE_SET_REG(ASM_GEN_CONF + addr_offset, 1<<ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_POS, ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_MASK);

        /* Load Coef */
        AFE_SET_REG(ASM_IIR_CRAM_ADDR + addr_offset, 0<<ASM_IIR_CRAM_ADDR_POS, ASM_IIR_CRAM_ADDR_MASK);
        for (i = 0; i < coef_count; i++) {
            //auto increase by 1 when read/write ASM_IIR_CRAM_DATA
            AFE_WRITE(ASM_IIR_CRAM_DATA + addr_offset, coef_ptr[i]<<ASM_IIR_CRAM_DATA_POS);
           // hal_gpt_delay_us(1);
        }
#if 0
        /* Read reg to verify */
        AFE_SET_REG(ASM_IIR_CRAM_ADDR + addr_offset, 0<<ASM_IIR_CRAM_ADDR_POS, ASM_IIR_CRAM_ADDR_MASK);
        AFE_GET_REG(ASM_IIR_CRAM_DATA+ addr_offset);
        for (i = 0; i < coef_count; i++) {
            uint32_t read_value = AFE_GET_REG(ASM_IIR_CRAM_DATA+ addr_offset);
            log_hal_msgid_info("DSP asrc iir coef %d:0x%x reg %d:0x%x\r\n" 4, i, coef_ptr[i], ((AFE_GET_REG(ASM_IIR_CRAM_ADDR+ addr_offset))>>ASM_IIR_CRAM_ADDR_POS), read_value);
        }
#endif

        AFE_SET_REG(ASM_IIR_CRAM_ADDR + addr_offset, 0<<ASM_IIR_CRAM_ADDR_POS, ASM_IIR_CRAM_ADDR_MASK);
        /* turn off IIR coe setting path */
        AFE_SET_REG(ASM_GEN_CONF + addr_offset, 0<<ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_POS, ASM_GEN_CONF_DSP_CTRL_COEFF_SRAM_MASK);

        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, (1<<ASM_CH01_CNFG_IIR_EN_POS), ASM_CH01_CNFG_IIR_EN_MASK);

        /*
        AFE_SET_REG(ASM_GEN_CONF + addr_offset,
                    (1<<ASM_GEN_CONF_CH_CLEAR_POS) | (1<<ASM_GEN_CONF_CH_EN_POS),
                    ASM_GEN_CONF_CH_CLEAR_MASK | ASM_GEN_CONF_CH_EN_MASK);
        */
    } else {
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0<<ASM_CH01_CNFG_IIR_EN_POS, ASM_CH01_CNFG_IIR_EN_MASK);
    }
    return false;
}

static inline uint32_t hal_src_get_samplingrate_to_palette(uint32_t rate)
{
    return rate * 4096 / 100;
}

static uint32_t hal_src_get_period_palette(uint32_t rate, bool clk45m)
{
#if 0
    switch (rate) {
        case 8000:
            return clk45m ? 0x058332 : 0x060000;
        case 12000:
            return clk45m ? 0x03accc : 0x040000;
        case 16000:
            return clk45m ? 0x02c199 : 0x030000;
        case 24000:
            return clk45m ? 0x01d666 : 0x020000;
        case 32000:
            return clk45m ? 0x0160cc : 0x018000;
        case 48000:
            return clk45m ? 0x00eb33 : 0x010000;
        case 96000:
            return clk45m ? 0x007599 : 0x008000;
        case 192000:
            return clk45m ? 0x003acd : 0x004000;
        case 384000:
            return clk45m ? 0x001d66 : 0x002000;
        case 7350:
            return clk45m ? 0x060000 : 0x0687D8;
        case 11025:
            return clk45m ? 0x040000 : 0x045A90;
        case 14700:
            return clk45m ? 0x030000 : 0x0343EC;
        case 22050:
            return clk45m ? 0x020000 : 0x022D48;
        case 29400:
            return clk45m ? 0x018000 : 0x01A1F6;
        case 44100:
            return clk45m ? 0x010000 : 0x0116A4;
        case 88200:
            return clk45m ? 0x008000 : 0x008B52;
        case 176400:
            return clk45m ? 0x004000 : 0x0045A9;
        case 352800:
            return clk45m ? 0x002000 : 0x0022D4;    /* ??? */
        default:
            return 0x0;
    }
#else
    //uint32_t divisor = (clk45m) ? 0xAC43DA80 : 0xBB800000;
    uint32_t divisor = (clk45m) ? 0x6D592A0 : 0x94C5F000;
    if(clk45m){
        rate /= 100;
    }
    return divisor/rate;

#endif
}


int32_t hal_src_set_power(afe_asrc_id_t asrc_id, bool on)
{
    uint32_t pos;

    if (asrc_id >= AFE_SRC_NUMBER) {
        HAL_AUDIO_LOG_ERROR("DSP - Error log [%s] :%d, %d !", 3, __FUNCTION__, asrc_id, on);
        return -1;
    }
    if (on) {
        /* asrc_ck select asm_h_ck(270M) */
        #if 0
        pos = asrc_id * 3;
        AFE_SET_REG(PWR2_ASM_CON2, 0x2 << pos, 0x3 << pos);
        #endif
        pos = PWR2_TOP_CON_PDN_MEM_ASRC1_POS + asrc_id;
        AFE_SET_REG(PWR2_TOP_CON, 0<<pos, 1<<pos);
        #if 0
        /* force reset */
        pos = PWR2_ASM_CON2_MEM_ASRC_1_RESET_POS + asrc_id;
        AFE_SET_REG(PWR2_ASM_CON2, 1<<pos, 1<<pos);
        AFE_SET_REG(PWR2_ASM_CON2, 0<<pos, 1<<pos);
        #endif
    } else {
        pos = PWR2_TOP_CON_PDN_MEM_ASRC1_POS + asrc_id;
        AFE_SET_REG(PWR2_TOP_CON, 0<<pos, 1<<pos);
    }
    return 0;
}

void hal_src_clear_irq(afe_asrc_id_t asrc_id, uint32_t status)
{
    uint32_t addr = ASM_IFR + asrc_id * 0x100;

    AFE_WRITE(addr, status);
}

void hal_src_set_irq_enable(afe_asrc_id_t asrc_id, bool enable)
{

    uint32_t addr = ASM_IER + asrc_id * 0x100;
    uint32_t val = (enable)
                     ? (1<<ASM_IER_IBUF_EMPTY_INTEN_POS) | (0<<ASM_IER_IBUF_AMOUNT_INTEN_POS) | (0<<ASM_IER_OBUF_OV_INTEN_POS) | (0<<ASM_IER_OBUF_AMOUNT_INTEN_POS)
                     : (0<<ASM_IER_IBUF_EMPTY_INTEN_POS) | (0<<ASM_IER_IBUF_AMOUNT_INTEN_POS) | (0<<ASM_IER_OBUF_OV_INTEN_POS) | (0<<ASM_IER_OBUF_AMOUNT_INTEN_POS);
    HAL_AUDIO_LOG_INFO("asrc_id %d,ASM_IER 0x%x\r\n",2,asrc_id,AFE_GET_REG(ASM_IER));
    AFE_WRITE(addr, val);

}
bool hal_src_set_configuration(afe_src_configuration_t *config, bool enable)
{
    uint32_t  addr_offset;
    uint32_t pos;
    addr_offset = config->id * 0x100;

    if (enable) {
#if 0
        /*We have check with ASRC HW designer. This hardware busy bit is used for monitoring. Not default to used by SW.*/
        if (AFE_GET_REG(ASM_GEN_CONF + addr_offset)&ASM_GEN_CONF_ASRC_BUSY_MASK) {
            log_hal_msgid_warning("%s() error: asrc[%d] is running\r\n", 2, __FUNCTION__, asrc_id);
            return;
        }
#endif
        AFE_SET_REG(ASM_GEN_CONF + addr_offset, 1<<ASM_GEN_CONF_CH_CLEAR_POS, ASM_GEN_CONF_CH_CLEAR_MASK);

        AFE_SET_REG(ASM_GEN_CONF + addr_offset, (config->hw_update_obuf_rdpnt)<<ASM_GEN_CONF_HW_UPDATE_OBUF_RDPNT_POS, ASM_GEN_CONF_HW_UPDATE_OBUF_RDPNT_MASK);

        /* when there is only 1 block data left in the input buffer, issue interrupt */
        /* times of 512bit. */
        AFE_SET_REG(ASM_IBUF_INTR_CNT0 + addr_offset, (config->input_buffer.size>>2)<<ASM_IBUF_INTR_CNT0_POS, ASM_IBUF_INTR_CNT0_MASK);
        /* when there is only 1 block space in the output buffer, issue interrupt */
        /* times of 512bit. 0xFF means if more than 16kB, send interrupt */
        AFE_SET_REG(ASM_OBUF_INTR_CNT0 + addr_offset, (config->output_buffer.size>>2)<<ASM_OBUF_INTR_CNT0_POS, ASM_OBUF_INTR_CNT0_MASK);

        /* clear all interrupt flag */
        hal_src_clear_irq(config->id, ASM_IFR_IBUF_EMPTY_INT_MASK | ASM_IFR_IBUF_AMOUNT_INT_MASK | ASM_IFR_OBUF_OV_INT_MASK | ASM_IFR_OBUF_AMOUNT_INT_MASK);

        /* always disable interrupt */
        if(config->id == AFE_SRC_1){
#ifdef ENABLE_HWSRC_CLKSKEW
            if(config->clkskew_mode == HAL_AUDIO_SRC_CLK_SKEW_V1) {
                hal_src_set_irq_enable(config->id, false);
            } else if (config->clkskew_mode == HAL_AUDIO_SRC_CLK_SKEW_V2){
                hal_src_set_irq_enable(config->id, true);
            } else {
                HAL_AUDIO_LOG_WARNING("No this type of SRC CLKSKEW MODE:%d", 1, config->clkskew_mode);
            }
#else
            hal_src_set_irq_enable(config->id, false);
#endif
        }else{
            hal_src_set_irq_enable(config->id, false);
        }

        /* set input buffer's base and size */
        AFE_SET_REG(ASM_IBUF_SADR + addr_offset, config->input_buffer.addr<<ASM_IBUF_SADR_POS, ASM_IBUF_SADR_MASK);
        AFE_SET_REG(ASM_IBUF_SIZE + addr_offset, config->input_buffer.size<<ASM_IBUF_SIZE_POS, ASM_IBUF_SIZE_MASK);
        /* set input buffer's rp and wp */
        if (config->ul_mode) {
            AFE_SET_REG(ASM_CH01_IBUF_WRPNT + addr_offset, (config->input_buffer.addr)<<ASM_CH01_IBUF_RDPNT_POS, ASM_CH01_IBUF_RDPNT_MASK);
            AFE_SET_REG(ASM_CH01_IBUF_RDPNT + addr_offset, (config->input_buffer.addr + config->input_buffer.size - config->input_buffer.offset)<<ASM_CH01_IBUF_WRPNT_POS, ASM_CH01_IBUF_WRPNT_MASK);
        } else {
            AFE_SET_REG(ASM_CH01_IBUF_WRPNT + addr_offset, (config->input_buffer.addr + config->input_buffer.offset)<<ASM_CH01_IBUF_RDPNT_POS, ASM_CH01_IBUF_RDPNT_MASK);
            AFE_SET_REG(ASM_CH01_IBUF_RDPNT + addr_offset, (config->input_buffer.addr)<<ASM_CH01_IBUF_WRPNT_POS, ASM_CH01_IBUF_WRPNT_MASK);
        }

        /* set output buffer's base and size */
        AFE_SET_REG(ASM_OBUF_SADR + addr_offset, config->output_buffer.addr<<ASM_OBUF_SADR_POS, ASM_OBUF_SADR_MASK);
        AFE_SET_REG(ASM_OBUF_SIZE + addr_offset, config->output_buffer.size<<ASM_OBUF_SIZE_POS, ASM_OBUF_SIZE_MASK);
        /* set output buffer's rp and wp */
        if (config->ul_mode) {
            AFE_SET_REG(ASM_CH01_OBUF_RDPNT + addr_offset, (config->output_buffer.addr + config->output_buffer.offset)<<ASM_CH01_OBUF_RDPNT_POS, ASM_CH01_OBUF_RDPNT_MASK);
            AFE_SET_REG(ASM_CH01_OBUF_WRPNT + addr_offset, (config->output_buffer.addr)<<ASM_CH01_OBUF_WRPNT_POS, ASM_CH01_OBUF_WRPNT_MASK);
        } else {
            AFE_SET_REG(ASM_CH01_OBUF_RDPNT + addr_offset, (config->output_buffer.addr)<<ASM_CH01_OBUF_RDPNT_POS, ASM_CH01_OBUF_RDPNT_MASK);
            AFE_SET_REG(ASM_CH01_OBUF_WRPNT + addr_offset, (config->output_buffer.addr + config->output_buffer.offset)<<ASM_CH01_OBUF_WRPNT_POS, ASM_CH01_OBUF_WRPNT_MASK);

        }

        /* set Bit-width Selection*/
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, ((config->input_buffer.format<=HAL_AUDIO_PCM_FORMAT_U16_BE)?true:false)<<ASM_CH01_CNFG_IBIT_WIDTH_POS, ASM_CH01_CNFG_IBIT_WIDTH_MASK);
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, ((config->output_buffer.format<=HAL_AUDIO_PCM_FORMAT_U16_BE)?true:false)<<ASM_CH01_CNFG_OBIT_WIDTH_POS, ASM_CH01_CNFG_OBIT_WIDTH_MASK);

        /* set channel number*/
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, (config->is_mono)<<ASM_CH01_CNFG_MONO_POS, ASM_CH01_CNFG_MONO_MASK);


#ifdef ENABLE_HWSRC_CLKSKEW
        if(config->clkskew_mode == HAL_AUDIO_SRC_CLK_SKEW_V2) {
             AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0x80<<ASM_CH01_CNFG_CLAC_AMOUNT_POS, ASM_CH01_CNFG_CLAC_AMOUNT_MASK);
        } else
#endif
        {
        AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 1<<ASM_CH01_CNFG_CLAC_AMOUNT_POS, ASM_CH01_CNFG_CLAC_AMOUNT_MASK);
        }
        AFE_SET_REG(ASM_MAX_OUT_PER_IN0 + addr_offset, 0<<ASM_MAX_OUT_PER_IN0_POS, ASM_MAX_OUT_PER_IN0_MASK);


        if ((config->mode == AFE_SRC_TRACKING_MODE_RX) || (config->mode == AFE_SRC_TRACKING_MODE_TX)) {
            /* check freq cali status */
            if ((AFE_READ(ASM_FREQ_CALI_CTRL + addr_offset)&ASM_FREQ_CALI_CTRL_FREQ_CALC_BUSY_MASK) ||
                (AFE_READ(ASM_FREQ_CALI_CTRL + addr_offset)&ASM_FREQ_CALI_CTRL_CALI_EN_POS)) {
                HAL_AUDIO_LOG_WARNING("DSP - Warning asrc calibration is busy 0x%x @@", 1, AFE_READ(ASM_FREQ_CALI_CTRL + addr_offset));
            }

            /* freq_mode = (denominator/period_mode)*0x800000 */
        #if 0
            AFE_WRITE(ASM_FREQ_CALI_CYC + addr_offset, 0x3F00);
            AFE_WRITE(ASM_CALI_DENOMINATOR + addr_offset, 0x3C00);//
            AFE_WRITE(ASM_FREQ_CALI_CTRL + addr_offset, 0x18500);
        #else
            AFE_WRITE(ASM_CALI_DENOMINATOR + addr_offset, 0x1FBD);
        #endif
            HAL_AUDIO_LOG_INFO("DSP - Hal Audio SRC tracking,  mode:%d, clock source:%d", 2, config->mode, config->tracking_clock);

            switch (config->tracking_clock) {
                case HAL_AUDIO_SRC_TRACKING_I2S1:
                case HAL_AUDIO_SRC_TRACKING_I2S2:
                case HAL_AUDIO_SRC_TRACKING_I2S3:
                case HAL_AUDIO_SRC_TRACKING_I2S4:
                    pos = (MEM_ASRC_TRAC_CON1_CALC_LRCK_SEL_POS + 3 * config->id);
                    //AFE_SET_REG(MEM_ASRC_TRAC_CON1,    (config->tracking_clock - HAL_AUDIO_SRC_TRACKING_I2S1 + 1)<<pos, MEM_ASRC_TRAC_CON1_CALC_LRCK_SEL_MASK<<pos);
                    AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, (config->tracking_clock-1) << ASM_FREQ_CALI_CTRL_SRC_SEL_POS, ASM_FREQ_CALI_CTRL_SRC_SEL_MASK);
                    break;
                case HAL_AUDIO_SRC_TRACKING_SPDIFIN:
                    //AFE_SET_REG(AFE_SPDIFIN_CFG1, 1<<AFE_SPDIFIN_CFG1_SEL_SPDIFIN_CLK_EN_POS, AFE_SPDIFIN_CFG1_SEL_SPDIFIN_CLK_EN_MASK);
                    //AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 0x0 << ASM_FREQ_CALI_CTRL_SRC_SEL_POS, ASM_FREQ_CALI_CTRL_SRC_SEL_MASK);
                    break;
                default:
                    break;
            }
            if (config->mode == AFE_SRC_TRACKING_MODE_RX) {
                AFE_WRITE(ASM_FREQ_CALI_CYC + addr_offset, 0x3F00);
                AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 1<<ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_POS, ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_MASK); /* Rx->FreqMode Bit9=1 */
                AFE_SET_REG(ASM_FREQUENCY_0 + addr_offset, hal_src_get_samplingrate_to_palette(config->output_buffer.rate),  0xFFFFFF);/*FrequencyPalette(output_fs)*/
                //AFE_SET_REG(ASM_FREQUENCY_2 + addr_offset, hal_src_get_samplingrate_to_palette(config->input_buffer.rate),  0xFFFFFF);/*FrequencyPalette(input_fs)*/
                //AFE_SET_REG(ASM_FREQUENCY_2 + addr_offset, 0 ,0xFFFFFF);//init rate = 0, wait i2s clock udpates it's value.

                AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 2<<ASM_CH01_CNFG_IFS_POS, ASM_CH01_CNFG_IFS_MASK);
                AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0<<ASM_CH01_CNFG_OFS_POS, ASM_CH01_CNFG_OFS_MASK);
            } else {
                bool is_44_1k_BASE = FALSE;
                is_44_1k_BASE = (config->input_buffer.rate == 44100 ||config->input_buffer.rate == 11025 ||config->input_buffer.rate == 88200 || config->input_buffer.rate == 176400); 
                AFE_WRITE(ASM_FREQ_CALI_CYC + addr_offset, is_44_1k_BASE ? 0x1B800 : 0x5F00);
                AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 0<<ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_POS, ASM_FREQ_CALI_CTRL_FREQ_UPDATE_FS2_MASK); /* Tx->PeriodMode Bit9=0 */
                AFE_SET_REG(ASM_FREQUENCY_0 + addr_offset, hal_src_get_period_palette(config->input_buffer.rate, is_44_1k_BASE), 0xFFFFFF);
                AFE_SET_REG(ASM_FREQUENCY_2 + addr_offset, hal_src_get_period_palette(config->output_buffer.rate, is_44_1k_BASE),0xFFFFFF);
                //AFE_SET_REG(ASM_FREQUENCY_2 + addr_offset, 0 ,0xFFFFFF);//init rate = 0, wait i2s clock udpates it's value.

                AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 2<<ASM_CH01_CNFG_IFS_POS, ASM_CH01_CNFG_IFS_MASK);
                AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0<<ASM_CH01_CNFG_OFS_POS, ASM_CH01_CNFG_OFS_MASK);
            }

            AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 0x1<<ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_POS | 1<<ASM_FREQ_CALI_CTRL_AUTO_RESTART_POS,  ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_MASK|ASM_FREQ_CALI_CTRL_AUTO_RESTART_MASK);
            AFE_SET_REG(ASM_FREQ_CALI_CTRL + addr_offset, 0x1<<ASM_FREQ_CALI_CTRL_CALI_EN_POS, ASM_FREQ_CALI_CTRL_CALI_EN_MASK);
        } else {
            AFE_SET_REG(ASM_FREQUENCY_0 + addr_offset, hal_src_get_samplingrate_to_palette(config->input_buffer.rate),  0xFFFFFF);
            AFE_SET_REG(ASM_FREQUENCY_1 + addr_offset, hal_src_get_samplingrate_to_palette(config->output_buffer.rate), 0xFFFFFF);
            AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 0<<ASM_CH01_CNFG_IFS_POS, ASM_CH01_CNFG_IFS_MASK);
            AFE_SET_REG(ASM_CH01_CNFG + addr_offset, 1<<ASM_CH01_CNFG_OFS_POS, ASM_CH01_CNFG_OFS_MASK);
        }

    } else {
        hal_src_set_irq_enable(config->id, false);
    }
    return false;
}

bool hal_src_set_continuous(afe_src_configuration_t *config, bool enable)
{
    uint32_t register_offset = config->id*0x100;
    uint32_t byte_per_sample ;

    if (enable) {
        if (config->mode == AFE_SRC_CONTINUOUS) {
            byte_per_sample = (config->input_buffer.format<=HAL_AUDIO_PCM_FORMAT_U16_BE) ? 2 : 4 ;

            if (!(config->is_mono)) {
                byte_per_sample <<= 1;
            }

            AFE_WRITE(ASM_SMPCNT_IRQ_VAL + register_offset, config->sample_count_threshold);
            AFE_WRITE(ASM_SMPCNT_WRAP_VAL + register_offset, (config->input_buffer.size/byte_per_sample));

            AFE_SET_REG(ASM_SMPCNT_CONF + register_offset, 1<<ASM_SMPCNT_CONF_RESET_POS, ASM_SMPCNT_CONF_RESET_MASK);
            AFE_SET_REG(ASM_SMPCNT_CONF + register_offset, 1<<ASM_SMPCNT_CONF_IRQ_CLEAR_POS, ASM_SMPCNT_CONF_IRQ_CLEAR_MASK);
            AFE_SET_REG(ASM_GEN_CONF + register_offset, enable<<ASM_GEN_CONF_ASRC_CONTINUOUS_EN_POS, ASM_GEN_CONF_ASRC_CONTINUOUS_EN_MASK);
            AFE_SET_REG(AFE_SRC_CONT_CON0, ((afe_samplerate_convert_samplerate_to_register_value(config->output_buffer.rate)<<4))<<(config->id*8), (0xF0)<<(config->id*8));
            AFE_SET_REG(ASM_SMPCNT_CONF + register_offset, ((enable^1)<<ASM_SMPCNT_CONF_IRQ_MASK_POS)|0x03, ASM_SMPCNT_CONF_IRQ_MASK_MASK|ASM_SMPCNT_CONF_SMPCNT_ENABLE2_MASK|ASM_SMPCNT_CONF_SMPCNT_ENABLE1_MASK); // Unmask irq, enable sample counter 1&2, Disable Hardware Auto Latch
        }
    } else {
        AFE_SET_REG(ASM_GEN_CONF + register_offset, enable<<ASM_GEN_CONF_ASRC_CONTINUOUS_EN_POS, ASM_GEN_CONF_ASRC_CONTINUOUS_EN_MASK);
        AFE_SET_REG(ASM_SMPCNT_CONF + register_offset, (0<<ASM_SMPCNT_CONF_SMPCNT_ENABLE2_POS)|(0<<ASM_SMPCNT_CONF_SMPCNT_ENABLE1_POS), ASM_SMPCNT_CONF_SMPCNT_ENABLE2_MASK|ASM_SMPCNT_CONF_SMPCNT_ENABLE1_MASK); // Disable sample counter 1&2
        AFE_SET_REG(ASM_SMPCNT_CONF + register_offset, ((enable^1)<<ASM_SMPCNT_CONF_IRQ_MASK_POS), ASM_SMPCNT_CONF_IRQ_MASK_MASK); // Mask irq,
        AFE_SET_REG(ASM_SMPCNT_CONF + register_offset, 1<<ASM_SMPCNT_CONF_IRQ_CLEAR_POS, ASM_SMPCNT_CONF_IRQ_CLEAR_MASK);
        AFE_SET_REG(ASM_SMPCNT_CONF + register_offset, 1<<ASM_SMPCNT_CONF_RESET_POS, ASM_SMPCNT_CONF_RESET_MASK);
    }
    return false;
}


bool hal_src_set_start(afe_asrc_id_t src_id, bool enable)
{
    if (enable) {
        AFE_SET_REG(ASM_GEN_CONF + src_id * 0x100,
                    (1<<ASM_GEN_CONF_CH_CLEAR_POS) | (1<<ASM_GEN_CONF_CH_EN_POS) | (1<<ASM_GEN_CONF_ASRC_EN_POS),
                    ASM_GEN_CONF_CH_CLEAR_MASK | ASM_GEN_CONF_CH_EN_MASK | ASM_GEN_CONF_ASRC_EN_MASK);

    } else {
        AFE_SET_REG(ASM_GEN_CONF + src_id * 0x100,
                    (0<<ASM_GEN_CONF_ASRC_EN_POS) | (0<<ASM_GEN_CONF_CH_EN_POS),
                    ASM_GEN_CONF_ASRC_EN_MASK | ASM_GEN_CONF_CH_EN_MASK);

        AFE_SET_REG(ASM_FREQ_CALI_CTRL + src_id * 0x100,
                    (0<<ASM_FREQ_CALI_CTRL_CALI_EN_POS)|(0<<ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_POS),
                    ASM_FREQ_CALI_CTRL_CALI_EN_MASK | ASM_FREQ_CALI_CTRL_AUTO_FS2_UPDATE_MASK);

    }
    return 0;
}

bool hal_src_start_continuous_mode(afe_asrc_id_t src_id, bool wait_playen, bool enable)
{
    AFE_SET_REG(AFE_SRC_CONT_CON0, ((wait_playen<<1) | enable)<<(src_id*8), (0x3)<<(src_id*8));
    return 0;
}

void hal_src_set_input_write_offset(afe_asrc_id_t src_id, uint32_t offset)
{
    uint32_t  addr_offset = src_id* 0x100;
    AFE_SET_REG(ASM_CH01_IBUF_WRPNT + addr_offset, offset<<ASM_CH01_IBUF_RDPNT_POS, ASM_CH01_IBUF_RDPNT_MASK);
}
void hal_src_set_output_read_offset(afe_asrc_id_t src_id, uint32_t offset)
{
    uint32_t  addr_offset = src_id* 0x100;
    AFE_SET_REG(ASM_CH01_OBUF_RDPNT + addr_offset, offset<<ASM_CH01_OBUF_RDPNT_POS, ASM_CH01_OBUF_RDPNT_MASK);
}
uint32_t hal_src_get_input_read_offset(afe_asrc_id_t src_id)
{
    uint32_t  addr_offset = src_id* 0x100;
    return AFE_GET_REG(ASM_CH01_IBUF_RDPNT + addr_offset);
}
uint32_t hal_src_get_output_write_offset(afe_asrc_id_t src_id)
{
    uint32_t  addr_offset = src_id* 0x100;
    return AFE_GET_REG(ASM_CH01_OBUF_WRPNT + addr_offset);
}
uint32_t hal_src_get_input_base_address(afe_asrc_id_t src_id)
{
    uint32_t  addr_offset = src_id* 0x100;
    return AFE_GET_REG(ASM_IBUF_SADR + addr_offset);
}
uint32_t hal_src_get_output_base_address(afe_asrc_id_t src_id)
{
    uint32_t  addr_offset = src_id* 0x100;
    return AFE_GET_REG(ASM_OBUF_SADR + addr_offset);
}

uint32_t hal_src_get_src_input_rate(afe_asrc_id_t src_id)
{
    uint32_t  addr_offset = src_id* 0x100;
    return AFE_GET_REG(ASM_FREQUENCY_0 + addr_offset);
}

void hal_src_set_src_input_rate(afe_asrc_id_t src_id, uint32_t register_value)
{
    uint32_t  addr_offset = src_id* 0x100;
    AFE_WRITE(ASM_FREQUENCY_0 + addr_offset, register_value);
}


bool hal_src_set_offset(afe_asrc_id_t src_id, uint32_t offset)
{
    uint32_t  addr_offset = src_id* 0x100;

    AFE_SET_REG(ASM_CH01_IBUF_WRPNT + addr_offset, offset<<ASM_CH01_IBUF_RDPNT_POS, ASM_CH01_IBUF_RDPNT_MASK);
    AFE_SET_REG(ASM_CH01_IBUF_RDPNT + addr_offset, offset<<ASM_CH01_IBUF_WRPNT_POS, ASM_CH01_IBUF_WRPNT_MASK);
    AFE_SET_REG(ASM_CH01_OBUF_RDPNT + addr_offset, offset<<ASM_CH01_OBUF_RDPNT_POS, ASM_CH01_OBUF_RDPNT_MASK);
    AFE_SET_REG(ASM_CH01_OBUF_WRPNT + addr_offset, offset<<ASM_CH01_OBUF_WRPNT_POS, ASM_CH01_OBUF_WRPNT_MASK);

    return false;
}

uint32_t hal_src_get_src_input_sample_count(afe_asrc_id_t src_id)
{
    uint32_t  addr_offset = src_id* 0x100;
    AFE_SET_REG(ASM_SMPCNT_CONF+addr_offset, 1<<ASM_SMPCNT_CONF_SW_LATCH_POS, ASM_SMPCNT_CONF_SW_LATCH_MASK);
    return AFE_GET_REG(ASM_SMPCNT1_LATCH + addr_offset);
}

/*******************************************************************************************
*                                         HW gain                                          *
********************************************************************************************/
bool hal_hw_gain_set_enable(afe_hardware_digital_gain_t gain_select, uint32_t samplerate, bool enable)
{
    uint32_t gain_register = 0, current_register;
    uint32_t sample_per_step = afe_volume_digital_get_ramp_step(gain_select);

    switch (gain_select) {
        case AFE_HW_DIGITAL_GAIN1 :
            gain_register = AFE_GAIN1_CON0;
            current_register = AFE_GAIN1_CUR;
            break;
        case AFE_HW_DIGITAL_GAIN2 :
            gain_register = AFE_GAIN2_CON0;
            current_register = AFE_GAIN2_CUR;
            break;
        case AFE_HW_DIGITAL_GAIN3 :
            gain_register = AFE_GAIN3_CON0;
            current_register = AFE_GAIN3_CUR;
            break;
        default:
            return true;
            break;

    }
    if (gain_register) {
        if (enable) {
            //AFE_WRITE(current_register, AFE_HW_DIGITAL_GAIN_MIN_REGISTER_VALUE); //Ramp from 0
            //AFE_SET_REG(gain_register, 0, 0x1);//Force toggle
            AFE_SET_REG(gain_register,
                        (true<<AFE_GAIN1_CON0_EXTEND_POS)|(sample_per_step<<AFE_GAIN1_CON0_PER_STEP_POS)|(afe_samplerate_convert_samplerate_to_register_value(samplerate)<<AFE_GAIN1_CON0_RATE_POS)|enable,
                        0x1FFF1);
        } else {
            AFE_SET_REG(gain_register, enable, 0x1);
        }
    }

    return false;
}

#ifdef AIR_A2DP_DRC_TO_USE_DGAIN_ENABLE
void hal_dgian_to_drc(uint32_t gain)
{
    DRC_DGAIN = gain*256;
}
#endif

bool hal_hw_gain_set_target(afe_hardware_digital_gain_t gain_select, uint32_t gain)
{
    uint32_t gain_register = 0, target_register;
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1 :
                gain_register = AFE_GAIN1_CON0;
                target_register = AFE_GAIN1_CON1;
                break;
            case AFE_HW_DIGITAL_GAIN2 :
                gain_register = AFE_GAIN2_CON0;
                target_register = AFE_GAIN2_CON1;
                break;
            case AFE_HW_DIGITAL_GAIN3 :
                gain_register = AFE_GAIN3_CON0;
                target_register = AFE_GAIN3_CON1;
                break;
            default:
                return true;
            break;
        }
        if (AFE_READ(gain_register)&AFE_GAIN1_CON0_EN_MASK) {
            AFE_SET_REG(target_register, gain, 0xfffff);
        } else {
            gain = 0xFFFFFFFF;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output HW gain:%d, Gain:0x%x", 2, gain_select, gain);
    }
    return false;
}
uint32_t hal_hw_gain_get_target(afe_hardware_digital_gain_t gain_select)
{
    uint32_t gain_register = 0, target_register;
    uint32_t gain = 0;
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1 :
                gain_register = AFE_GAIN1_CON0;
                target_register = AFE_GAIN1_CON1;
                break;
            case AFE_HW_DIGITAL_GAIN2 :
                gain_register = AFE_GAIN2_CON0;
                target_register = AFE_GAIN2_CON1;
                break;
            case AFE_HW_DIGITAL_GAIN3 :
                gain_register = AFE_GAIN3_CON0;
                target_register = AFE_GAIN3_CON1;
                break;
            default:
                return true;
            break;
        }
        if (AFE_READ(gain_register)&AFE_GAIN1_CON0_EN_MASK) {
            gain =  AFE_READ(target_register);
        } else {
            gain = 0xFFFFFFFF;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Get current target HW gain:%d, Gain:0x%x", 2, gain_select, gain);
    }
    return gain;
}

uint32_t hal_hw_gain_get_current_gain(afe_hardware_digital_gain_t gain_select)
{
    uint32_t gain_register = 0, current_gain_register;
    uint32_t gain = 0;
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1 :
                gain_register = AFE_GAIN1_CON0;
                current_gain_register = AFE_GAIN1_CUR;
                break;
            case AFE_HW_DIGITAL_GAIN2 :
                gain_register = AFE_GAIN2_CON0;
                current_gain_register = AFE_GAIN2_CUR;
                break;
            case AFE_HW_DIGITAL_GAIN3 :
                gain_register = AFE_GAIN3_CON0;
                current_gain_register = AFE_GAIN3_CUR;
                break;
            default:
                return true;
            break;
        }
        if (AFE_READ(gain_register)&AFE_GAIN1_CON0_EN_MASK) {
            gain =  AFE_READ(current_gain_register);
        } else {
            gain = 0xFFFFFFFF;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Get current gain HW gain:%d, Gain:0x%x", 2, gain_select, gain);
    }
    return gain;
}
bool hal_hw_gain_set_current_gain(afe_hardware_digital_gain_t gain_select, uint32_t gain)
{
    uint32_t gain_register = 0, current_gain_register;
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1 :
                gain_register = AFE_GAIN1_CON0;
                current_gain_register = AFE_GAIN1_CUR;
                break;
            case AFE_HW_DIGITAL_GAIN2 :
                gain_register = AFE_GAIN2_CON0;
                current_gain_register = AFE_GAIN2_CUR;
                break;
            case AFE_HW_DIGITAL_GAIN3 :
                gain_register = AFE_GAIN3_CON0;
                current_gain_register = AFE_GAIN3_CUR;
                break;
            default:
                return true;
            break;
        }
        if (AFE_READ(gain_register)&AFE_GAIN1_CON0_EN_MASK) {
            AFE_SET_REG(current_gain_register, gain, 0xfffff);
        } else {
            gain = 0xFFFFFFFF;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output HW gain Current:%d, Gain:0x%x", 2, gain_select, gain);
    }
    return false;
}

bool hal_hw_gain_is_running(afe_hardware_digital_gain_t gain_select)
{
    uint32_t is_running = false;
    if(hal_audio_afe_get_counter()) {
        if(hal_hw_gain_get_target(gain_select)==hal_hw_gain_get_current_gain(gain_select)){
            is_running = false;
        }else{
            is_running = true;
        }

        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Get current gain HW gain:%d, is running:0x%x", 2, gain_select, is_running);
    }
    return is_running;
}


bool hal_hw_gain_set_down_step(afe_hardware_digital_gain_t gain_select,uint32_t down_step)
{
    uint32_t gain_register = 0, down_step_register;
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1 :
                gain_register = AFE_GAIN1_CON0;
                down_step_register = AFE_GAIN1_CON2;
                break;
            case AFE_HW_DIGITAL_GAIN2 :
                gain_register = AFE_GAIN2_CON0;
                down_step_register = AFE_GAIN2_CON2;
                break;
            case AFE_HW_DIGITAL_GAIN3 :
                gain_register = AFE_GAIN3_CON0;
                down_step_register = AFE_GAIN3_CON2;
                break;
            default:
                return true;
            break;
        }
        if (AFE_READ(gain_register)&AFE_GAIN1_CON0_EN_MASK) {
            AFE_SET_REG(down_step_register, down_step, 0xfffff);
        } else {
            down_step = 0xFFFFFFFF;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Set HW gain:%d, down_step:0x%x", 2, gain_select, down_step);
    }
    return false;
}

bool hal_hw_gain_set_up_step(afe_hardware_digital_gain_t gain_select,uint32_t up_step)
{
    uint32_t gain_register = 0, up_step_register;
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1 :
                gain_register = AFE_GAIN1_CON0;
                up_step_register = AFE_GAIN1_CON3;
                break;
            case AFE_HW_DIGITAL_GAIN2 :
                gain_register = AFE_GAIN2_CON0;
                up_step_register = AFE_GAIN2_CON3;
                break;
            case AFE_HW_DIGITAL_GAIN3 :
                gain_register = AFE_GAIN3_CON0;
                up_step_register = AFE_GAIN3_CON3;
                break;
            default:
                return true;
            break;
        }
        if (AFE_READ(gain_register)&AFE_GAIN1_CON0_EN_MASK) {
            AFE_SET_REG(up_step_register, up_step, 0xfffff);
        } else {
            up_step = 0xFFFFFFFF;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Set HW gain:%d, up_step:0x%x", 2, gain_select, up_step);
    }
    return false;
}

bool hal_hw_gain_set_sample_per_step(afe_hardware_digital_gain_t gain_select,uint32_t sample_per_step)
{
    uint32_t gain_register = 0, sample_per_step_register;
    UNUSED(sample_per_step_register);
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1 :
                gain_register = AFE_GAIN1_CON0;
                break;
            case AFE_HW_DIGITAL_GAIN2 :
                gain_register = AFE_GAIN2_CON0;
                break;
            case AFE_HW_DIGITAL_GAIN3 :
                gain_register = AFE_GAIN3_CON0;
                break;
            default:
                return true;
                break;

        }

        if (AFE_READ(gain_register)&AFE_GAIN1_CON0_EN_MASK) {
            AFE_SET_REG(gain_register, sample_per_step<<AFE_GAIN1_CON0_PER_STEP_POS, AFE_GAIN1_CON0_PER_STEP_MASK);
        } else {
            sample_per_step = 0xFFFFFFFF;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Set HW gain:%d, sample_per_step:0x%x", 2, gain_select, sample_per_step);
    }
    return false;
}

uint32_t hal_hw_gain_get_sample_rate(afe_hardware_digital_gain_t gain_select)
{
    uint32_t gain_register = 0, register_value;
    uint32_t sample_rate = 0;
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_DIGITAL_GAIN1 :
                gain_register = AFE_GAIN1_CON0;
                break;
            case AFE_HW_DIGITAL_GAIN2 :
                gain_register = AFE_GAIN2_CON0;
                break;
            case AFE_HW_DIGITAL_GAIN3 :
                gain_register = AFE_GAIN3_CON0;
                break;
            default:
                return true;
            break;
        }
        if (AFE_READ(gain_register)&AFE_GAIN1_CON0_EN_MASK) {
            register_value =  ((AFE_GET_REG(gain_register)>>AFE_GAIN1_CON0_RATE_POS)&0xF);
            sample_rate = afe_samplerate_convert_register_value_to_samplerate(register_value);
        } else {
            sample_rate = 0xFFFFFFFF;
        }
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Get current gain HW gain:%d, sample_rate:%d", 2, gain_select, sample_rate);
    }
    return sample_rate;
}

void hal_gain_set_analog_output_class_ab(afe_volume_analog_output_gain_value_t gain)
{
    if(hal_audio_afe_get_counter()) {
        AFE_SET_REG(ZCD_CON2, gain, 0xFFF);
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Analog, Gain:0x%x", 1, gain);
    }
}

uint32_t hal_gain_convert_analog_output_cap_value(afe_volume_analog_output_gain_value_t gain)
{
    uint32_t convert_value;
    switch (gain) {
        case AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_NEG_6_DB:
            convert_value = AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_NEG_6_DB;
            break;
        case AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_NEG_3_DB:
            convert_value = AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_NEG_3_DB;
            break;
        case AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_0_DB:
            convert_value = AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_POS_0_DB;
            break;
        case AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_3_DB:
            convert_value = AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_POS_3_DB;
            break;
        case AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_6_DB:
            convert_value = AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_POS_6_DB;
            break;
        default:
            convert_value = AFE_VOLUME_ANALOG_OUTPUT_CLASSD_CAP_VALUE_POS_0_DB;
            break;
    }
    return convert_value;
}

void hal_gain_set_analog_output_class_d(afe_volume_analog_output_gain_value_t gain_l, afe_volume_analog_output_gain_value_t gain_r)
{
    uint32_t reg_value_l, reg_value_r;
    if(hal_audio_afe_get_counter()) {
        ANA_SET_REG((AUDDEC_ANA_CON25),
                     ((gain_l<<AUDDEC_ANA_CON25_CLD_HPL_GAIN_POS)|(gain_r<<AUDDEC_ANA_CON25_CLD_HPR_GAIN_POS)),
                     (AUDDEC_ANA_CON25_CLD_HPL_GAIN_MASK|AUDDEC_ANA_CON25_CLD_HPR_GAIN_MASK));

        reg_value_l = (gain_l==AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_6_DB) ? AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_3_DB: gain_l;
        reg_value_r = (gain_r==AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_6_DB) ? AFE_VOLUME_ANALOG_OUTPUT_GAIN_CLASSD_POS_3_DB: gain_r;
        ANA_SET_REG((AUDDEC_ANA_CON18),
                    ((reg_value_l<<AUDDEC_ANA_CON18_HPL_AUX_GAIN_POS)|(reg_value_r<<AUDDEC_ANA_CON18_HPR_AUX_GAIN_POS)),
                    (AUDDEC_ANA_CON18_HPL_AUX_GAIN_MASK|AUDDEC_ANA_CON18_HPR_AUX_GAIN_MASK));

        reg_value_l = hal_gain_convert_analog_output_cap_value(gain_l);
        reg_value_r = hal_gain_convert_analog_output_cap_value(gain_r);
        ANA_SET_REG(AUDDEC_ANA_CON23, reg_value_l<<AUDDEC_ANA_CON23_CLD_CAP_GAIN_POS, AUDDEC_ANA_CON23_CLD_CAP_GAIN_MASK);
        AFE_SET_REG(AUDDEC_ANA_CON24, reg_value_r<<AUDDEC_ANA_CON24_CLD_RCH_TRIM_POS, AUDDEC_ANA_CON24_CLD_RCH_TRIM_MASK);

        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Output Gain: CON25:0x%x, CON18:0x%x, CON23:0x%x, CON24:0x%x", 3, ANA_GET_REG(AUDDEC_ANA_CON25), ANA_GET_REG(AUDDEC_ANA_CON18), AFE_GET_REG(AUDDEC_ANA_CON23), AFE_GET_REG(AUDDEC_ANA_CON24));
    }
}

uint32_t hal_gain_get_analog_output(void)
{
    return AFE_READ(ZCD_CON2);
}

void hal_gain_set_analog_input(afe_hardware_analog_gain_t gain_select, afe_volume_analog_input_gain_value_t gain_l,  afe_volume_analog_input_gain_value_t gain_r)
{
    uint32_t gain_register_l = 0, gain_register_r = 0;
    uint32_t reg_offset=0;
    if(hal_audio_afe_get_counter()) {
        switch (gain_select) {
            case AFE_HW_ANALOG_GAIN_INPUT1:
                gain_register_l = AUDENC_ANA_CON0;
                gain_register_r = AUDENC_ANA_CON1;
                reg_offset = AUDENC_ANA_CON0_L_PREAMP_GAIN_POS;
                break;
            case AFE_HW_ANALOG_GAIN_INPUT2:
                gain_register_l = AUDENC_ANA_CON9;
                gain_register_r = AUDENC_ANA_CON10;
                reg_offset = AUDENC_ANA_CON9_RG_AUD23PREAMPLGAIN_POS;
                break;
            case AFE_HW_ANALOG_GAIN_INPUT3:
                gain_register_l = AUDENC_ANA_CON18;
                gain_register_r = AUDENC_ANA_CON19;
                reg_offset = AUDENC_ANA_CON18_RG_AUD45PREAMPLGAIN_POS;
                break;
            default:
                return;
            break;
        }

        AFE_SET_REG(gain_register_l, gain_l<<reg_offset, 0xF<<reg_offset);
        AFE_SET_REG(gain_register_r, gain_r<<reg_offset, 0xF<<reg_offset);

        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Gain Input Analog:%d, GainL:%d, GainR:%d", 3, gain_select, gain_l, gain_r);
    }
}

/*******************************************************************************************
*                                       Sine Generator                                         *
********************************************************************************************/
bool hal_sine_generator_set_samplerate(uint32_t samplerate)
{
    afe_samplerate_general_t general_rate = afe_samplerate_convert_samplerate_to_register_value(samplerate);
    AFE_SET_REG(AFE_SINEGEN_CON0, (general_rate<<8)|(general_rate<<20), (0xF<<8)|(0xF<<20));
    return false;
}

bool hal_sine_generator_set_amplitude(afe_sine_generator_amplitude_t amplitude_divider)
{
    AFE_SET_REG(AFE_SINEGEN_CON0, (amplitude_divider<<5)|(amplitude_divider<<17), (0x7<<5)|(0x7<<17));
    return false;
}

bool hal_sine_generator_set_period(uint32_t period_divider)
{
    AFE_SET_REG(AFE_SINEGEN_CON0, (period_divider<<0)|(period_divider<<12), (0x1F<<0)|(0x1F<<12));
    return false;
}

bool hal_sine_generator_set_enable(hal_audio_agent_t agent, bool is_input, bool enable)
{
    uint32_t value;
    switch(agent) {
        case HAL_AUDIO_AGENT_MEMORY_DL1:
            value = 9;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL2:
            value = 10;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL3:
            value = 11;
            break;
        case HAL_AUDIO_AGENT_MEMORY_DL12:
            value = 12;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL1:
            value = 40;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL2:
            value = 41;
            break;
        case HAL_AUDIO_AGENT_MEMORY_VUL3:
            value = 44;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB:
            value = 42;
            break;
        case HAL_AUDIO_AGENT_MEMORY_AWB2:
            value = 43;
            break;
        case HAL_AUDIO_AGENT_MEMORY_SRC1:
            value = 13;
            break;
        case HAL_AUDIO_AGENT_MEMORY_SRC2:
            value = 14;
            break;
        case HAL_AUDIO_AGENT_BLOCK_HWGAIN1:
            value = (is_input) ? 7 : 38;
            break;
        case HAL_AUDIO_AGENT_BLOCK_HWGAIN2:
            value = (is_input) ? 8 : 39;
            break;
        case HAL_AUDIO_AGENT_BLOCK_HWGAIN3:
            value = (is_input) ? 3 : 35;
            break;
        case HAL_AUDIO_AGENT_BLOCK_UP_SAMPLE01:
            value = (is_input) ? 21 : 50;
            break;
        case HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE01:
            value = (is_input) ? 19 : 48;
            break;
        case HAL_AUDIO_AGENT_BLOCK_DOWN_SAMPLE23:
            value = (is_input) ? 20 : 49;
            break;
        case HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE:
            value = (is_input) ? 15 : 45;
            break;
        case HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE:
            value = (is_input) ? 16 : 46;
            break;
        case HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE:
            value = (is_input) ? 17 : 47;
            break;
        case HAL_AUDIO_AGENT_DEVICE_I2S0_MASTER:
            value = (is_input) ? 0 : 32;
            break;
        case HAL_AUDIO_AGENT_DEVICE_I2S1_MASTER:
            value = (is_input) ? 1 : 33;
            break;
        case HAL_AUDIO_AGENT_DEVICE_I2S2_MASTER:
            value = (is_input) ? 2 : 34;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_DL1:
            value = 36;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1:
            value = 4;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL2:
            value = 5;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL3:
            value = 6;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL4:
            value = 18;
            break;
        case HAL_AUDIO_AGENT_DEVICE_SIDETONE:
            value = 37;
            break;
        default:
            return true;
            break;

    }
    AFE_WRITE(AFE_SINEGEN_CON2, value);
    hal_audio_clock_enable_sine(enable);
    AFE_SET_REG(AFE_SINEGEN_CON0, enable << 26,  0x1 << 26); //enable sine
    HAL_AUDIO_LOG_INFO("singen RG con0 0x%x con2 0x%x\r\n",2,AFE_READ(AFE_SINEGEN_CON0),AFE_READ(AFE_SINEGEN_CON2));
    return false;
}

/*******************************************************************************************
*                                       I2S master                                         *
********************************************************************************************/
afe_i2s_id_t hal_i2s_convert_id(hal_audio_control_t device, hal_audio_device_interface_t device_interface)
{
    afe_i2s_id_t i2s_id = AFE_I2S_NUMBER;

    if (device&(HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_L|HAL_AUDIO_CONTROL_DEVICE_I2S_MASTER_R|HAL_AUDIO_CONTROL_DEVICE_I2S_SLAVE)) {
        if (device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
            i2s_id = AFE_I2S0;
        } else if (device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
            i2s_id = AFE_I2S1;
        } else if (device_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_3) {
            i2s_id = AFE_I2S2;
        }
    }
    return i2s_id;
}

afe_i2s_id_t hal_i2s_convert_id_by_agent(hal_audio_agent_t agent)
{
    afe_i2s_id_t i2s_id = AFE_I2S_NUMBER;

    switch (agent) {
        default:
        case HAL_AUDIO_AGENT_DEVICE_I2S0_MASTER:
        case HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE:
            i2s_id = AFE_I2S0;
            break;
        case HAL_AUDIO_AGENT_DEVICE_I2S1_MASTER:
        case HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE:
            i2s_id = AFE_I2S1;
            break;
        case HAL_AUDIO_AGENT_DEVICE_I2S2_MASTER:
        case HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE:
            i2s_id = AFE_I2S2;
            break;

    }
    return i2s_id;
}


uint32_t hal_i2s_master_convert_i2s_register(afe_i2s_id_t i2s_id)
{
    uint32_t i2s_reg;
    switch (i2s_id) {
        case AFE_I2S0:
        default:
            i2s_reg = AFE_I2S0_CON;
            break;
        case AFE_I2S1:
            i2s_reg = AFE_I2S1_CON;
            break;
        case AFE_I2S2:
            i2s_reg = AFE_I2S2_CON;
            break;
    }
    return i2s_reg;
}

bool hal_i2s_master_set_configuration(hal_audio_device_parameter_i2s_master_t *config, afe_i2s_id_t i2s_id)
{
    uint32_t i2s_reg = hal_i2s_master_convert_i2s_register(i2s_id);
    uint32_t register_value = 0;
    bool is_i2s_format;
    is_i2s_format = (config->i2s_format == HAL_AUDIO_I2S_I2S) ? true : false;

    register_value |= (config->word_length)<<AFE_I2S0_CON_WLEN32BIT_POS;    // word length
    register_value |= 0<<AFE_I2S0_CON_SLAVE_POS;                            // master mode
    register_value |= (is_i2s_format)<<AFE_I2S0_CON_I2S_FORMAT_POS;         // setting I2S format


    if (HAL_AUDIO_I2S_LJ == config->i2s_format) {
        register_value |= 1<<AFE_I2S0_CON_WS_INV_POS;                       // WS_INV
        register_value |= 0<<AFE_I2S0_CON_OUT_RJ_POS;                       // out RJ mode enable(0:LJ 1: RJ)
        register_value |= 0<<AFE_I2S0_CON_IN_RJ_POS;                        // in  RJ mode enable(0:LJ 1: RJ)
    } else if (HAL_AUDIO_I2S_RJ == config->i2s_format) {
        register_value |= 1<<AFE_I2S0_CON_WS_INV_POS;                       // WS_INV
        register_value |= 1<<AFE_I2S0_CON_OUT_RJ_POS;                       // out RJ mode enable(0:LJ 1: RJ)
        register_value |= 1<<AFE_I2S0_CON_IN_RJ_POS;                        // in  RJ mode enable(0:LJ 1: RJ)

        // note: 24 bit data in i2s 32bit RJ mode, we need to shift 8 bit. if in 16 bit data, we have to shfit 16 bit
        register_value |= 8<<AFE_I2S0_CON_OUT_SHIFT_POS;                // mI2S_OUT_SHIFT_NUM
        register_value |= 8<<AFE_I2S0_CON_IN_SHIFT_POS;                 // mI2S_IN_SHIFT_NUM
    }
    register_value |= (afe_samplerate_convert_samplerate_to_register_value(config->rate))<<AFE_I2S0_CON_RATE_POS;                // sampling rate

    register_value |= (config->is_low_jitter)<<AFE_I2S0_CON_LOW_JITTER_POS; // LowJitterMode

    register_value |= 0<<AFE_I2S0_CON_OUT_SWAP_POS;                         //output swap
    register_value |= 0<<AFE_I2S0_CON_IN_SWAP_POS;                          //input swap

    register_value |= (config->is_recombinant)<<AFE_I2S0_CON_IN_RECOMB_POS; //for G-sensor format

    AFE_SET_REG(i2s_reg, register_value, ~AFE_I2S0_CON_ENABLE_MASK);
    return false;
}

void hal_i2s_master_enable_apll(afe_i2s_apll_t apll_source, bool enable)
{
    if (true == enable) {
#if (HAL_AUDIO_APLL_TUNER_NUMBER==2)
        // Clear upper layer audio CG
        //TEMP!!!
        AFE_SET_REG(0xA2030B20, 0x040001FC, 0xFFFFFFFF);//XO_PDN_CLRD0
        AFE_SET_REG(0xA2020238, 0x01020000, 0xFFFFFFFF);//CKSYS_CLK_CFG_2

        if (apll_source == AFE_I2S_APLL1) {
            #if 0//clock control on CM4, modify for ab1568
            clk_set_apll(CLK_APLL_45M);
            clock_mux_sel(CLK_MCLK_SEL, 0);
            #endif
            hal_audio_clock_enable_apll(true);

            //Enable tuner
            AFE_SET_REG(AFE_APLL1_TUNER_CFG, 0x00000432, 0x0000FFFF);
            AFE_SET_REG(AFE_APLL1_TUNER_CFG, 0x1, 0x1);
        } else {
            #if 0//clock control on CM4, modify for ab1568
            clk_set_apll(CLK_APLL_49M);
            clock_mux_sel(CLK_MCLK_SEL, 0);
            #endif
            hal_audio_clock_enable_apll2(true);

            //Enable tuner
            AFE_SET_REG(AFE_APLL2_TUNER_CFG, 0x00000434, 0x0000FFFF);
            AFE_SET_REG(AFE_APLL2_TUNER_CFG, 0x1, 0x1);
        }
#else

        if (apll_source == AFE_I2S_APLL1) {
            hal_audio_clock_enable_apll(true);
            //Enable tuner
            AFE_SET_REG(AFE_APLL1_TUNER_CFG, 0x00000433, 0x0000FFFF);
        } else {
            hal_audio_clock_enable_apll2(true);
            //Enable tuner
            AFE_SET_REG(AFE_APLL2_TUNER_CFG, 0x00000435, 0x0000FFFF);
        }
#endif
    } else {
#if (HAL_AUDIO_APLL_TUNER_NUMBER==2)
        if (apll_source == AFE_I2S_APLL1) {
            // Disable APLL1
            AFE_SET_REG(AFE_APLL1_TUNER_CFG, 0x0, 0x1);
            hal_audio_clock_enable_apll(false);
            #if 0//clock control on CM4, modify for ab1568
            clock_mux_sel(CLK_MCLK_SEL, 1);
            #endif
        } else {
            // Disable APLL2
            AFE_SET_REG(AFE_APLL2_TUNER_CFG, 0x0, 0x1);
            hal_audio_clock_enable_apll2(false);
            #if 0//clock control on CM4, modify for ab1568
            clock_mux_sel(CLK_MCLK_SEL, 1);
            #endif
        }
#else
        if (apll_source == AFE_I2S_APLL1) {
            AFE_SET_REG(AFE_APLL1_TUNER_CFG, 0x0, 0x1);
            hal_audio_clock_enable_apll(false);
        } else {
            AFE_SET_REG(AFE_APLL2_TUNER_CFG, 0x0, 0x1);
            hal_audio_clock_enable_apll2(false);
        }


#endif
    }
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio APLL:%d, enable:%d", 2, apll_source, enable);
}

void hal_i2s_master_enable_mclk(afe_i2s_apll_t apll_source, afe_i2s_id_t i2s_id, uint32_t mclk_divider, bool enable)
{
    uint32_t clock_divider_reg;
    uint32_t clock_divider_shift;
#if (HAL_AUDIO_APLL_TUNER_NUMBER==2)
    if ((i2s_id == AFE_I2S0) || (i2s_id == AFE_I2S1)) {
        clock_divider_reg = 0xA2020308;
    } else {
        clock_divider_reg = 0xA202030C;
    }
    if ((i2s_id == AFE_I2S0) || (i2s_id == AFE_I2S2)) {
        clock_divider_shift = 0;
    } else {
        clock_divider_shift = 16;
    }
    if (apll_source == AFE_I2S_APLL2) {
        AFE_WRITE(0xA2020304, 0x01010101); // I2S0/1/2/3 from hf_faud_2_ck MCLK_49M
    } else {
        AFE_WRITE(0xA2020304, 0x00000000); // I2S0/1/2/3 from hf_faud_1_ck MCLK_45M
    }
#else
    UNUSED(apll_source);
    clock_divider_reg = 0xA2020308;
    clock_divider_shift = 0;
    AFE_WRITE(0xA2020304, 0x00000000); // I2S0/1/2/3 from hf_faud_1_ck MCLK_45M
#endif
    if (enable) {
        AFE_SET_REG(0xA2020300, 0<<(8*i2s_id), 1<<(8*i2s_id));
        HAL_AUDIO_DELAY_US(10);
        //AFE_WRITE(0xA2020238, 0x01020000); // aud_interface1=45M/aud_interface0=49M

        /* Setting audio clock divider */  //Toggled to apply apll_ck_div bit-8 or bit-24
        AFE_WRITE(clock_divider_reg, mclk_divider<<clock_divider_shift);
        AFE_WRITE(clock_divider_reg, (mclk_divider | 0x00000100)<<clock_divider_shift);
        AFE_WRITE(clock_divider_reg, mclk_divider<<clock_divider_shift);
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio Set MClk divider:%d", 1, mclk_divider);
    } else {
        AFE_SET_REG(0xA2020300, 1<<(8*i2s_id), 1<<(8*i2s_id));
    }
}

bool hal_i2s_master_set_loopback(afe_i2s_id_t i2s_id, bool enable)
{
    //Set I2S master internal loopback(I2S out -> I2S in)
    uint32_t register_pos = 0;
    switch(i2s_id) {
        default:
        case AFE_I2S0:
            register_pos = AFE_I2S_TOP_CON_I2S0_LOOPBACK_POS;
            break;
        case AFE_I2S1:
            register_pos = AFE_I2S_TOP_CON_I2S1_LOOPBACK_POS;
            break;
        case AFE_I2S2:
            register_pos = AFE_I2S_TOP_CON_I2S2_LOOPBACK_POS;
            break;
    }
    AFE_SET_REG(AFE_I2S_TOP_CON, enable<<register_pos, 1<<register_pos);
    return false;
}

bool hal_i2s_master_enable(afe_i2s_id_t i2s_id, bool enable)
{
    uint32_t i2s_reg = hal_i2s_master_convert_i2s_register(i2s_id);
    AFE_SET_REG(i2s_reg, enable,  0x00000001);// enable
    return false;
}

void hal_i2s_master_gpio_init(afe_i2s_id_t i2s_id)
{
    uint32_t gpio_pin;
    gpio_pin = 8 + (4 * i2s_id);
    hal_gpio_init(gpio_pin);
    hal_pinmux_set_function(gpio_pin,4);

    hal_gpio_init(gpio_pin+1);
    hal_pinmux_set_function(gpio_pin+1,4);

    hal_gpio_init(gpio_pin+2);
    hal_pinmux_set_function(gpio_pin+2,4);

    hal_gpio_init(gpio_pin+3);
    hal_pinmux_set_function(gpio_pin+3,4);
}

/*******************************************************************************************
*                                       I2S slave                                         *
********************************************************************************************/
uint32_t hal_i2s_slave_convert_i2s_base(afe_i2s_id_t i2s_id)
{
    uint32_t i2s_base;
    switch (i2s_id) {
        case AFE_I2S0:
        default:
            i2s_base = AFE_I2S_SLV0_BASE;
            break;
        case AFE_I2S1:
            i2s_base = AFE_I2S_SLV1_BASE;
            break;
        case AFE_I2S2:
            i2s_base = AFE_I2S_SLV2_BASE;
            break;
    }
    return i2s_base;
}

#define WriteREG(_addr, _value) (*(volatile uint32_t *)(_addr) = (_value))
void hal_i2s_slave_set_clock()
{
    // Turning on powers
    //WriteREG(0xA2110210, 0xD);//AUDIO_PWR_CON(0xA2110210) = 0xD;
    //WriteREG(0xA2200048, 0x0);//INFRA_CFG_MTCMOS2(0xA2200048) = 0x0;

    // Turning on clocks of DMAs
    //WriteREG(0xA21E0320, 0x1);//PDN_PD_CLRD0(0xA21E0320) = 0x1;
    //WriteREG(0xA0010074, 0xFFFFFFFF);//DMA_GLB_CLK_SET(0xA0010074) = 0xFFFFFFFF;

    // Turning on clocks
    WriteREG(0xC9000074, 0xFFFF);//I2S_DMA_GLB_CLK_SET(0xC9000074) = 0xFFFF;
    WriteREG(0xC900000C, 0xFFFF);//I2S_DMA_GLB_CPU0_SET(0xC900000C) = 0xFFFF;
    //WriteREG(0xA001000C, 0xFFF);//DMA_GLB_CPU0_SET(0xA001000C) = 0xFFF;
    //WriteREG(0xA0010070, 0xFFF);//DMA_GLB_CLK_CFG(0xA0010070) = 0xFFF;
    //WriteREG(0xA2030B80, 0x60);//XO_PDN_TOP_CLRD0(0xA2030B80) = 0x60;
    //vRegSetBit(0xC0000000, 6);//AUDIO_TOP_CON0(0xC0000000) |= 0x40;//1: Power on i2s_slv_hclk clock
}

bool hal_i2s_slave_set_configuration(hal_audio_device_parameter_i2s_slave_t *config, afe_i2s_id_t i2s_id)
{
    uint32_t i2s_base = hal_i2s_slave_convert_i2s_base(i2s_id);

#if 0
    #if 1
            AFE_WRITE(i2s_base+I2S_UL_CONTROL_OFFSET, 0x402A004|(config->word_length<<I2S_UL_CONTROL_WLEN_POS)|((config->i2s_format==HAL_AUDIO_I2S_I2S)<<I2S_UL_CONTROL_FMT_POS)|(((config->i2s_format==HAL_AUDIO_I2S_RJ))<<I2S_UL_CONTROL_RJ_POS));
            AFE_WRITE(i2s_base+I2S_DL_CONTROL_OFFSET, 0x4022084|(config->word_length<<I2S_DL_CONTROL_WLEN_POS)|((config->i2s_format==HAL_AUDIO_I2S_I2S)<<I2S_UL_CONTROL_FMT_POS)|(((config->i2s_format==HAL_AUDIO_I2S_RJ))<<I2S_UL_CONTROL_RJ_POS));
    #else
            if (config->i2s_format == HAL_AUDIO_I2S_I2S) {
                AFE_WRITE(i2s_base+I2S_UL_CONTROL_OFFSET, 0x402A004|(config->word_length<<I2S_UL_CONTROL_WLEN_POS)|(1<<I2S_UL_CONTROL_FMT_POS));
                AFE_WRITE(i2s_base+I2S_DL_CONTROL_OFFSET, 0x4022084|(config->word_length<<I2S_DL_CONTROL_WLEN_POS)|(1<<I2S_DL_CONTROL_FMT_POS));
            } else if (config->i2s_format == HAL_AUDIO_I2S_RJ) {
                AFE_WRITE(i2s_base+I2S_UL_CONTROL_OFFSET, 0x402A004|(config->word_length<<I2S_UL_CONTROL_WLEN_POS)|(1<<I2S_UL_CONTROL_RJ_POS));
                AFE_WRITE(i2s_base+I2S_DL_CONTROL_OFFSET, 0x4022084|(config->word_length<<I2S_DL_CONTROL_WLEN_POS)|(1<<I2S_DL_CONTROL_RJ_POS));
            } else {
                AFE_WRITE(i2s_base+I2S_UL_CONTROL_OFFSET, 0x402A004|(config->word_length<<I2S_UL_CONTROL_WLEN_POS));
                AFE_WRITE(i2s_base+I2S_DL_CONTROL_OFFSET, 0x4022084|(config->word_length<<I2S_DL_CONTROL_WLEN_POS));
            }
    #endif
#endif

#if 0
    if (config->memory_select & HAL_AUDIO_MEMORY_UL_MASK) {
        //INTERCONN I2S UL
        //Default:[2]:I2S slave mode select, [18]:If bit 18 is 0, left & right data will be packed into a 32-bit word.
        //Setting:[1] I2S WLEN, [3] I2S FMT, [13] valid_24_16, [14] right_adjust
        AFE_WRITE(i2s_base+I2S_UL_CONTROL_OFFSET, 0x40004 | (config->word_length<<I2S_UL_CONTROL_WLEN_POS)|((config->i2s_format==HAL_AUDIO_I2S_I2S)<<I2S_UL_CONTROL_FMT_POS)|(config->word_length<<I2S_UL_CONTROL_VALID_24BIT_POS)|((config->i2s_format==HAL_AUDIO_I2S_RJ)<<I2S_UL_CONTROL_RJ_POS));
        HAL_AUDIO_LOG_INFO("DSP - Set Device Slave %d Rx", 1, i2s_base);
    }
    if (config->memory_select & HAL_AUDIO_MEMORY_DL_MASK) {
        //INTERCONN I2S DL
        //Default:[2]:I2S slave mode select, [18]:If bit 18 is 0, left & right data will be packed into a 32-bit word, [7] DL FIFIO 2D EQ Mode
        //Setting:[1] I2S WLEN, [3] I2S FMT, [13] valid_24_16, [14] right_adjust
        AFE_WRITE(i2s_base+I2S_DL_CONTROL_OFFSET, 0x40084 | (config->word_length<<I2S_DL_CONTROL_WLEN_POS)|((config->i2s_format==HAL_AUDIO_I2S_I2S)<<I2S_DL_CONTROL_FMT_POS)|(config->word_length<<I2S_DL_CONTROL_VALID_24BIT_POS)|((config->i2s_format==HAL_AUDIO_I2S_RJ)<<I2S_DL_CONTROL_RJ_POS));
        HAL_AUDIO_LOG_INFO("DSP - Set Device Slave %d Tx", 1, i2s_base);
    }

    if (config->i2s_format == HAL_AUDIO_I2S_I2S || config->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_TDM || config->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_TDM) {
        //Setting:[5] I2S ws invert=0, [26] i2s_in_ws_inv=0
    } else {
        //Setting:[5] I2S ws invert=1, [26] i2s_in_ws_inv=1
        if (config->memory_select & HAL_AUDIO_MEMORY_UL_MASK) {
            AFE_SET_REG (i2s_base+I2S_UL_CONTROL_OFFSET,  0x1 << I2S_UL_CONTROL_WS_INV_POS, I2S_UL_CONTROL_WS_INV_MASK);
            AFE_SET_REG (i2s_base+I2S_UL_CONTROL_OFFSET,  0x1 << I2S_UL_CONTROL_IN_WS_INV_POS, I2S_UL_CONTROL_IN_WS_INV_MASK);
        }
        if (config->memory_select & HAL_AUDIO_MEMORY_DL_MASK) {
            AFE_SET_REG (i2s_base+I2S_DL_CONTROL_OFFSET,  0x1 << I2S_DL_CONTROL_WS_INV_POS, I2S_DL_CONTROL_WS_INV_MASK);
            AFE_SET_REG (i2s_base+I2S_DL_CONTROL_OFFSET,  0x1 << I2S_DL_CONTROL_IN_WS_INV_POS, I2S_DL_CONTROL_IN_WS_INV_MASK);
        }
    }

    if(config->is_vdma_mode) {
    } else {
        //Set Interconn mode
        AFE_SET_REG(i2s_base+I2S_DL_CONTROL_OFFSET, 1<<I2S_DL_CONTROL_INTERCONN_POS, I2S_DL_CONTROL_INTERCONN_MASK);
    }

    if (config->memory_select & HAL_AUDIO_MEMORY_UL_MASK) {
        // Turning on FIFOs for RX
        AFE_SET_REG(i2s_base+I2S_GLOBAL_EN_CONTROL_OFFSET,
                (1<<I2S_GLOBAL_EN_CONTROL_ENABLE_POS)|(1<<I2S_GLOBAL_EN_CONTROL_UL_FIFO_EN_POS),
                 I2S_GLOBAL_EN_CONTROL_ENABLE_MASK|I2S_GLOBAL_EN_CONTROL_UL_FIFO_EN_MASK);
    }
    if (config->memory_select & HAL_AUDIO_MEMORY_DL_MASK) {
        // Turning on FIFOs for TX
        AFE_SET_REG(i2s_base+I2S_GLOBAL_EN_CONTROL_OFFSET,
                (1<<I2S_GLOBAL_EN_CONTROL_ENABLE_POS)|(1<<I2S_GLOBAL_EN_CONTROL_DL_FIFO_EN_POS),
                 I2S_GLOBAL_EN_CONTROL_ENABLE_MASK|I2S_GLOBAL_EN_CONTROL_DL_FIFO_EN_MASK);
    }
#else
    AFE_WRITE(i2s_base+I2S_UL_CONTROL_OFFSET, 0x40004 | (config->word_length<<I2S_UL_CONTROL_WLEN_POS)|((config->i2s_format==HAL_AUDIO_I2S_I2S)<<I2S_UL_CONTROL_FMT_POS)|(config->word_length<<I2S_UL_CONTROL_VALID_24BIT_POS)|((config->i2s_format==HAL_AUDIO_I2S_RJ)<<I2S_UL_CONTROL_RJ_POS));
    AFE_WRITE(i2s_base+I2S_DL_CONTROL_OFFSET, 0x40084 | (config->word_length<<I2S_DL_CONTROL_WLEN_POS)|((config->i2s_format==HAL_AUDIO_I2S_I2S)<<I2S_DL_CONTROL_FMT_POS)|(config->word_length<<I2S_DL_CONTROL_VALID_24BIT_POS)|((config->i2s_format==HAL_AUDIO_I2S_RJ)<<I2S_DL_CONTROL_RJ_POS));
#ifdef MTK_TDM_ENABLE
    if ((config->memory_select != HAL_AUDIO_MEMORY_DL_SLAVE_TDM || config->memory_select != HAL_AUDIO_MEMORY_UL_SLAVE_TDM) && (config->i2s_format == HAL_AUDIO_I2S_RJ || config->i2s_format == HAL_AUDIO_I2S_LJ)) {
        //DMA or interconn mode with LJ or RJ mode, setting ULDL[5][26]=1
        AFE_SET_REG(i2s_base+I2S_UL_CONTROL_OFFSET, (1<<I2S_UL_CONTROL_WS_INV_POS) | (1<<I2S_UL_CONTROL_IN_WS_INV_POS), I2S_UL_CONTROL_WS_INV_MASK || I2S_UL_CONTROL_IN_WS_INV_MASK);
        AFE_SET_REG(i2s_base+I2S_DL_CONTROL_OFFSET, (1<<I2S_DL_CONTROL_WS_INV_POS) | (1<<I2S_DL_CONTROL_IN_WS_INV_POS), I2S_DL_CONTROL_WS_INV_MASK || I2S_DL_CONTROL_IN_WS_INV_MASK);
    } else {
        AFE_SET_REG(i2s_base+I2S_UL_CONTROL_OFFSET, (0<<I2S_UL_CONTROL_WS_INV_POS) | (0<<I2S_UL_CONTROL_IN_WS_INV_POS), I2S_UL_CONTROL_WS_INV_MASK || I2S_UL_CONTROL_IN_WS_INV_MASK);
        AFE_SET_REG(i2s_base+I2S_DL_CONTROL_OFFSET, (0<<I2S_DL_CONTROL_WS_INV_POS) | (0<<I2S_DL_CONTROL_IN_WS_INV_POS), I2S_DL_CONTROL_WS_INV_MASK || I2S_DL_CONTROL_IN_WS_INV_MASK);
    }
#endif
    if (config->is_vdma_mode) {
        AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S_DMA_POS, AUDIO_TOP_CON1_PDN_I2S_DMA_MASK);// 0: Does not power down I2S_DMA, 1: Power down I2S_DMA
        AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S_DMA_IRQ_POS, AUDIO_TOP_CON1_PDN_I2S_DMA_IRQ_MASK);// 0: i2s_slv_irq is i2s_dma, 1: i2s_slv_irq is i2s_slv err irq
    } else {
        //Set Interconn mode
        AFE_SET_REG(i2s_base+I2S_DL_CONTROL_OFFSET, 1<<I2S_DL_CONTROL_INTERCONN_POS, I2S_DL_CONTROL_INTERCONN_MASK);
    }
#ifdef MTK_TDM_ENABLE
    if (config->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_TDM || config->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_TDM) {
        if (config->i2s_format == HAL_AUDIO_I2S_I2S) {
            AFE_WRITE(i2s_base+I2S_DL_TDM_MODE_OFFSET, 0x1);// 0: no TDM, 1: I2S format for TDM, 2: Left- or Right-justified format for TDM
            AFE_WRITE(i2s_base+I2S_UL_TDM_MODE_OFFSET, 0x1);// 0: no TDM, 1: I2S format for TDM, 2: Left- or Right-justified format for TDM
        } else {
            AFE_WRITE(i2s_base+I2S_DL_TDM_MODE_OFFSET, 0x2);// 0: no TDM, 1: I2S format for TDM, 2: Left- or Right-justified format for TDM
            AFE_WRITE(i2s_base+I2S_UL_TDM_MODE_OFFSET, 0x2);// 0: no TDM, 1: I2S format for TDM, 2: Left- or Right-justified format for TDM
        }
    }
#endif
    // Turning on FIFOs for TX & RX
    AFE_SET_REG(i2s_base+I2S_GLOBAL_EN_CONTROL_OFFSET,
                (1<<I2S_GLOBAL_EN_CONTROL_ENABLE_POS)|(1<<I2S_GLOBAL_EN_CONTROL_DL_FIFO_EN_POS)|(1<<I2S_GLOBAL_EN_CONTROL_UL_FIFO_EN_POS),
                 I2S_GLOBAL_EN_CONTROL_ENABLE_MASK|I2S_GLOBAL_EN_CONTROL_DL_FIFO_EN_MASK|I2S_GLOBAL_EN_CONTROL_UL_FIFO_EN_MASK);

#endif
    return false;
}

bool hal_i2s_slave_set_power(afe_i2s_id_t i2s_id, bool enable)
{
    uint32_t i2s_base = hal_i2s_slave_convert_i2s_base(i2s_id);
    // Turning on clock gate for the 26M clock
    AFE_SET_REG(i2s_base+I2S_GLOBAL_EN_CONTROL_OFFSET, (enable^1)<<I2S_GLOBAL_EN_CONTROL_PDN_POS, I2S_GLOBAL_EN_CONTROL_PDN_MASK);
    // Turning on clock gate for RX
    AFE_SET_REG(i2s_base+I2S_UL_SR_EN_CONTROL_OFFSET,  (enable^1)<<I2S_UL_SR_EN_CONTROL_UL_PDN_POS, I2S_UL_SR_EN_CONTROL_UL_PDN_MASK);
    // Turning on clock gate for TX
    AFE_SET_REG(i2s_base+I2S_DL_SR_EN_CONTROL_OFFSET,  (enable^1)<<I2S_DL_SR_EN_CONTROL_DL_PDN_POS, I2S_DL_SR_EN_CONTROL_DL_PDN_MASK);
    return false;
}

bool hal_i2s_slave_set_enable(afe_i2s_id_t i2s_id, bool enable)
{
    uint32_t i2s_base = hal_i2s_slave_convert_i2s_base(i2s_id);
    AFE_SET_REG(i2s_base+I2S_DL_SR_EN_CONTROL_OFFSET, enable<<I2S_DL_SR_EN_CONTROL_DL_EN_POS, I2S_DL_SR_EN_CONTROL_DL_EN_MASK);
    AFE_SET_REG(i2s_base+I2S_UL_SR_EN_CONTROL_OFFSET, enable<<I2S_UL_SR_EN_CONTROL_UL_EN_POS, I2S_UL_SR_EN_CONTROL_UL_EN_MASK);
    return false;
}

bool hal_i2s_slave_set_share_fifo()
{
    WriteREG(AFE_I2S_SLV_ENGEN_CON4, 0x1000000);//Mux select for the shared FIFO
    return false;
}

void hal_i2s_slave_gpio_init(afe_i2s_id_t i2s_id)
{
    uint32_t gpio_pin;
    gpio_pin = 8 + (4 * i2s_id);

    hal_gpio_init(gpio_pin);
    hal_pinmux_set_function(gpio_pin,5);

    hal_gpio_init(gpio_pin+1);
    hal_pinmux_set_function(gpio_pin+1,5);

    hal_gpio_init(gpio_pin+2);
    hal_pinmux_set_function(gpio_pin+2,5);

    hal_gpio_init(gpio_pin+3);
    hal_pinmux_set_function(gpio_pin+3,5);
}

#ifdef MTK_TDM_ENABLE
#define ReadREG(_addr)          (*(volatile uint32_t *)(_addr))
bool hal_i2s_slave_set_tdm_status(hal_audio_device_parameter_i2s_slave_t *config, afe_i2s_id_t i2s_id)
{
    uint32_t i2s_base = hal_i2s_slave_convert_i2s_base(i2s_id);
    if (config->memory_select == HAL_AUDIO_MEMORY_UL_SLAVE_TDM || config->memory_select == HAL_AUDIO_MEMORY_DL_SLAVE_TDM) {
        //Repeatedly reading UL_TDM_STATUS until it's not 0, then writing this value into UL_TDM_SETTING
        while(ReadREG(i2s_base + I2S_UL_TDM_STATUS_OFFSET) == 0){
            DSP_MW_LOG_I("[SLAVE TDM] UL_TDM_STATUS == 0", 0);
        }
        DSP_MW_LOG_I("[SLAVE TDM] UL_TDM_SETTING=%d, CH=%d", 2, ReadREG(i2s_base + I2S_UL_TDM_STATUS_OFFSET), config->tdm_channel);
        WriteREG(i2s_base + I2S_UL_TDM_SETTING_OFFSET, ReadREG(i2s_base + I2S_UL_TDM_STATUS_OFFSET));

        //Repeatedly reading DL_TDM_STATUS until it's not 0, then writing this value into DL_TDM_SETTING.
        while(ReadREG(i2s_base + I2S_DL_TDM_STATUS_OFFSET) == 0){
            DSP_MW_LOG_I("[SLAVE TDM] DL_TDM_STATUS == 0", 0);
        }
        DSP_MW_LOG_I("[SLAVE TDM] DL_TDM_SETTING=%d, CH=%d", 2,ReadREG(i2s_base + I2S_DL_TDM_STATUS_OFFSET), config->tdm_channel);
        WriteREG(i2s_base + I2S_DL_TDM_SETTING_OFFSET, ReadREG(i2s_base + I2S_DL_TDM_STATUS_OFFSET));
    }
    return false;
}
#endif

/*******************************************************************************************
*                                          SPDIF                                           *
********************************************************************************************/
bool hal_spdif_set_configuration(hal_audio_device_parameter_spdif_t *config)
{
    uint32_t spdif_reg_value;
    afe_spdif_clock_divider_t clock_divider;
    afe_spdif_clock_source_t clock_source;
    afe_spdif_selection_t i2s_selection;

    spdif_reg_value = AFE_GET_REG(AFE_I2S_SPDIF_CON0);

    if (config->i2s_setting.with_mclk || config->i2s_setting.is_low_jitter) {
        if (config->i2s_setting.rate == 44100) {
            clock_source = AFE_SPDIF_CLOCK_SOURCE_APLL2;//AFE_SPDIF_CLOCK_SOURCE_APLL1;
            clock_divider = AFE_SPDIF_CLOCK_DIVIDER_4;
        } else {
            clock_source = AFE_SPDIF_CLOCK_SOURCE_APLL1;//AFE_SPDIF_CLOCK_SOURCE_APLL2;
            if (config->i2s_setting.rate == 192000) {
                clock_divider = AFE_SPDIF_CLOCK_DIVIDER_1;
            } else if (config->i2s_setting.rate == 48000) {
                clock_divider = AFE_SPDIF_CLOCK_DIVIDER_4;
            } else {
                clock_divider = AFE_SPDIF_CLOCK_DIVIDER_2;
            }
        }
    } else {
        //Only 48K
        clock_source = AFE_SPDIF_CLOCK_SOURCE_DCXO48K;
        clock_divider = AFE_SPDIF_CLOCK_DIVIDER_1;

    }
    AFE_SET_REG((uint32_t)&spdif_reg_value, clock_source<<AFE_I2S_SPDIF_CON0_CLOCK_SOURCE_POS, AFE_I2S_SPDIF_CON0_CLOCK_SOURCE_MASK);
    AFE_SET_REG((uint32_t)&spdif_reg_value, clock_divider<<AFE_I2S_SPDIF_CON0_DIVIDER_POS, AFE_I2S_SPDIF_CON0_DIVIDER_MASK);

    AFE_SET_REG((uint32_t)&spdif_reg_value, true<<AFE_I2S_SPDIF_CON0_CLOCK_ON_POS, AFE_I2S_SPDIF_CON0_CLOCK_ON_MASK);

    if (config->i2s_setting.i2s_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_1) {
        i2s_selection = AFE_SPDIF_SELECTION_I2S0;
    } else if (config->i2s_setting.i2s_interface == HAL_AUDIO_CONTROL_DEVICE_INTERFACE_2) {
        i2s_selection = AFE_SPDIF_SELECTION_I2S1;
    } else {
        i2s_selection = AFE_SPDIF_SELECTION_I2S2;
    }
    AFE_SET_REG((uint32_t)&spdif_reg_value, i2s_selection<<AFE_I2S_SPDIF_CON0_SELECTION_POS, AFE_I2S_SPDIF_CON0_SELECTION_MASK);

    AFE_SET_REG((uint32_t)&spdif_reg_value, false<<AFE_I2S_SPDIF_CON0_SINETONE_POS, AFE_I2S_SPDIF_CON0_SINETONE_MASK);

    AFE_WRITE(AFE_I2S_SPDIF_CON0, spdif_reg_value);

    return false;
}

bool hal_spdif_enable(bool enable)
{
    AFE_SET_REG(AFE_I2S_SPDIF_CON0, (enable<<AFE_I2S_SPDIF_CON0_ENABLE_POS)|(enable<<AFE_I2S_SPDIF_CON0_CLOCK_ON_POS), AFE_I2S_SPDIF_CON0_ENABLE_MASK|AFE_I2S_SPDIF_CON0_CLOCK_ON_MASK);
    return false;
}



/*******************************************************************************************
*                                     Up/Down Sampler                                      *
********************************************************************************************/
afe_updown_ratio_t afe_convert_ratio_to_up_filter_value(uint32_t ratio)
{
    afe_updown_ratio_t ratio_value;
    switch (ratio)
    {
        case 1:
        default:
            ratio_value = AFE_UPDOWN_RATIO_BY1;
            break;
        case 2:
            ratio_value = AFE_UPDOWN_RATIO_BY2;
            break;
        case 3:
            ratio_value = AFE_UPDOWN_RATIO_BY3;
            break;
        case 4:
            ratio_value = AFE_UPDOWN_RATIO_BY4;
            break;
        case 6:
            ratio_value = AFE_UPDOWN_RATIO_BY6;
            break;
        case 12:
            ratio_value = AFE_UPDOWN_RATIO_BY12;
            break;
    }
    return ratio_value;
}

bool hal_updown_set_ratio(afe_updown_sampler_id_t updown_id, uint32_t input_rate, uint32_t output_rate)
{
    uint32_t register_offset;
    afe_updown_ratio_t ratio;
    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        ratio = afe_convert_ratio_to_up_filter_value(output_rate/input_rate);
        register_offset = AFE_DEC_INT_CON2_UP_CH01_RATIO_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        ratio = afe_convert_ratio_to_up_filter_value(input_rate/output_rate);
        register_offset = AFE_DEC_INT_CON2_DOWN_CH01_RATIO_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        ratio = afe_convert_ratio_to_up_filter_value(input_rate/output_rate);
        register_offset = AFE_DEC_INT_CON2_DOWN_CH23_RATIO_POS;
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning invalid ID:%d, in:%d, out:%d @@", 1, updown_id, input_rate, output_rate);
    }
    AFE_SET_REG(AFE_DEC_INT_CON2, ratio<<register_offset, 7<<register_offset);
    return false;
}

bool hal_updown_set_input_rate(afe_updown_sampler_id_t updown_id, uint32_t input_rate)
{
    uint32_t register_offset;
    uint32_t register_value;
    register_value = afe_samplerate_convert_samplerate_to_register_value(input_rate);
    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        register_offset = AFE_DEC_INT_CON1_UP_CH01_INPUT_RATE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        register_offset = AFE_DEC_INT_CON1_DOWN_CH01_INPUT_RATE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        register_offset = AFE_DEC_INT_CON1_DOWN_CH23_INPUT_RATE_POS;
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning invalid ID:%d, in rate:%d @@", 1, updown_id, input_rate);
        return true;
    }
    AFE_SET_REG(AFE_DEC_INT_CON1, register_value<<register_offset, 0xF<<register_offset);
    return false;
}

uint32_t hal_updown_get_input_rate(afe_updown_sampler_id_t updown_id)
{
    uint32_t register_offset;
    uint32_t register_value;

    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        register_offset = AFE_DEC_INT_CON1_UP_CH01_INPUT_RATE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        register_offset = AFE_DEC_INT_CON1_DOWN_CH01_INPUT_RATE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        register_offset = AFE_DEC_INT_CON1_DOWN_CH23_INPUT_RATE_POS;
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning invalid ID:%d @@", 1, updown_id);
        return 0;
    }
    register_value = (AFE_GET_REG(AFE_DEC_INT_CON1)>>register_offset)&0xF;
    return afe_samplerate_convert_register_value_to_samplerate(register_value);
}


bool hal_updown_set_output_rate(afe_updown_sampler_id_t updown_id, uint32_t output_rate)
{
    uint32_t register_offset;
    uint32_t register_value;
    register_value = afe_samplerate_convert_samplerate_to_register_value(output_rate);
    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        register_offset = AFE_DEC_INT_CON1_UP_CH01_OUTPUT_RATE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        register_offset = AFE_DEC_INT_CON1_DOWN_CH01_OUTPUT_RATE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        register_offset = AFE_DEC_INT_CON1_DOWN_CH23_OUTPUT_RATE_POS;
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning invalid ID:%d, out rate:%d @@", 1, updown_id, output_rate);
        return true;
    }
    AFE_SET_REG(AFE_DEC_INT_CON1, register_value<<register_offset, 0xF<<register_offset);
    return false;
}

uint32_t hal_updown_get_output_rate(afe_updown_sampler_id_t updown_id)
{
    uint32_t register_offset;
    uint32_t register_value;

    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        register_offset = AFE_DEC_INT_CON1_UP_CH01_OUTPUT_RATE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        register_offset = AFE_DEC_INT_CON1_DOWN_CH01_OUTPUT_RATE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        register_offset = AFE_DEC_INT_CON1_DOWN_CH23_OUTPUT_RATE_POS;
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning invalid ID:%d @@", 1, updown_id);
        return 0;
    }
    register_value = (AFE_GET_REG(AFE_DEC_INT_CON1)>>register_offset)&0xF;
    return afe_samplerate_convert_register_value_to_samplerate(register_value);
}

bool hal_updown_set_enable(afe_updown_sampler_id_t updown_id, bool enable)
{
    uint32_t register_offset;

    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        register_offset = AFE_DEC_INT_CON0_UP_CH01_ENABLE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        register_offset = AFE_DEC_INT_CON0_DOWN_CH01_ENABLE_POS;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        register_offset = AFE_DEC_INT_CON0_DOWN_CH23_ENABLE_POS;
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning invalid ID:%d  @@", 1, updown_id);
        return true;
    }
    AFE_SET_REG(AFE_DEC_INT_CON0, ((enable<<1)|enable)<<register_offset, 0x3<<register_offset);
    return false;
}


/*******************************************************************************************
*                                        tick align                                        *
********************************************************************************************/
bool hal_tick_align_set_updown(afe_updown_sampler_id_t updown_id, hal_audio_path_interconnection_tick_source_t tick_source, bool enable)
{
    uint32_t offset_in, offset_out;
    afe_tick_align_downsampler_t tick_select, tick_in, tick_out;
    switch (tick_source) {
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S0_IN:
            tick_select = AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S0_IN;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S1_IN:
            tick_select = AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S1_IN;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S2_IN:
            tick_select = AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S2_IN;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S0_OUT:
            tick_select = AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S0_OUT;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S1_OUT:
            tick_select = AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S1_OUT;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S2_OUT:
            tick_select = AFE_TICK_ALIGN_DOWNSAMPLER_SLAVE_I2S2_OUT;
            break;
        default:
            return true;
            break;
    }

    if (updown_id == AFE_UPDOWN_SAMPLER_UP_CH01) {
        offset_in = AFE_I2S_SLV_ENGEN_CON3_UP_IN_MODE_POS;
        offset_out = AFE_I2S_SLV_ENGEN_CON3_UP_OUT_MODE_POS;
        tick_in = tick_select;
        tick_out = tick_in;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH01) {
        offset_in = AFE_I2S_SLV_ENGEN_CON3_DOWN01_IN_MODE_POS;
        offset_out = AFE_I2S_SLV_ENGEN_CON3_DOWN01_OUT_MODE_POS;
        tick_in = tick_select;
        tick_out = AFE_TICK_ALIGN_DOWNSAMPLER_OUTPUT_CH0;
    } else if (updown_id == AFE_UPDOWN_SAMPLER_DOWN_CH23) {
        offset_in = AFE_I2S_SLV_ENGEN_CON3_DOWN23_IN_MODE_POS;
        offset_out = AFE_I2S_SLV_ENGEN_CON3_DOWN23_OUT_MODE_POS;
        tick_in = tick_select;
        tick_out = AFE_TICK_ALIGN_DOWNSAMPLER_OUTPUT_CH0;
    } else {
        return true;
    }

    AFE_SET_REG(AFE_I2S_SLV_ENGEN_CON3, ((enable|(tick_in<<1))<<offset_in)|((enable|(tick_out<<1))<<offset_out), (0xF<<offset_in)|(0xF<<offset_out));
    return false;
}

bool hal_tick_align_set_hw_gain(afe_hardware_digital_gain_t gain_select, hal_audio_path_interconnection_tick_source_t tick_source, bool enable)
{
    uint32_t register_offset;
    afe_tick_align_gain_t tick_select;
    switch (tick_source) {
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S0_IN:
            tick_select = AFE_TICK_ALIGN_GAIN_SLAVE_I2S0_IN;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S1_IN:
            tick_select = AFE_TICK_ALIGN_GAIN_SLAVE_I2S1_IN;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S2_IN:
            tick_select = AFE_TICK_ALIGN_GAIN_SLAVE_I2S2_IN;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S0_OUT:
            tick_select = AFE_TICK_ALIGN_GAIN_SLAVE_I2S0_OUT;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S1_OUT:
            tick_select = AFE_TICK_ALIGN_GAIN_SLAVE_I2S1_OUT;
            break;
        case HAL_AUDIO_PATH_TICK_SOURCE_I2S2_OUT:
            tick_select = AFE_TICK_ALIGN_GAIN_SLAVE_I2S2_OUT;
            break;
        default:
            return true;
            break;
    }

    if (gain_select == AFE_HW_DIGITAL_GAIN1) {
        register_offset = AFE_I2S_SLV_ENGEN_CON0_GAIN_MODE_POS;
    } else if (gain_select == AFE_HW_DIGITAL_GAIN2) {
        register_offset = AFE_I2S_SLV_ENGEN_CON0_GAIN2_MODE_POS;
    } else {
        register_offset = AFE_I2S_SLV_ENGEN_CON0_GAIN3_MODE_POS;
    }

    AFE_SET_REG(AFE_I2S_SLV_ENGEN_CON0, ((enable|(tick_select<<1))<<register_offset), (0xF<<register_offset));
    return false;
}

bool hal_tick_align_set_memory_agent(hal_audio_memory_selection_t memory_select, hal_audio_path_interconnection_tick_source_t tick_source, bool enable)
{
    uint32_t register_offset;

    switch (memory_select) {
        case HAL_AUDIO_MEMORY_DL_DL1:
            register_offset = AFE_I2S_SLV_ENGEN_CON1_DL1_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_DL_DL2:
            register_offset = AFE_I2S_SLV_ENGEN_CON1_DL2_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_DL_DL3:
            register_offset = AFE_I2S_SLV_ENGEN_CON1_DL3_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_DL_DL12:
            register_offset = AFE_I2S_SLV_ENGEN_CON1_DL12_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_DL_SRC1:
            register_offset = AFE_I2S_SLV_ENGEN_CON1_SRC1_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_DL_SRC2:
            register_offset = AFE_I2S_SLV_ENGEN_CON1_SRC2_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_UL_VUL1:
            register_offset = AFE_I2S_SLV_ENGEN_CON0_VUL_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_UL_VUL2:
            register_offset = AFE_I2S_SLV_ENGEN_CON0_VUL2_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_UL_VUL3:
            register_offset = AFE_I2S_SLV_ENGEN_CON0_VUL3_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_UL_AWB:
            register_offset = AFE_I2S_SLV_ENGEN_CON0_AWB_MODE_POS;
            break;
        case HAL_AUDIO_MEMORY_UL_AWB2:
            register_offset = AFE_I2S_SLV_ENGEN_CON0_AWB2_MODE_POS;
            break;
        default:
            return true;
            break;
    }

    if (memory_select&HAL_AUDIO_MEMORY_DL_MASK) {
        afe_tick_align_dl_t dl_tick_select;
        switch (tick_source) {
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S0_IN:
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S0_OUT:
                dl_tick_select = AFE_TICK_ALIGN_DL_SLAVE_I2S0_OUT;
                break;
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S1_IN:
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S1_OUT:
                dl_tick_select = AFE_TICK_ALIGN_DL_SLAVE_I2S1_OUT;
                break;
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S2_IN:
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S2_OUT:
                dl_tick_select = AFE_TICK_ALIGN_DL_SLAVE_I2S2_OUT;
                break;
            default:
                return true;
                break;
        }
        AFE_SET_REG(AFE_I2S_SLV_ENGEN_CON1, ((enable|(dl_tick_select<<1))<<register_offset), (0xF<<register_offset));

    } else {
        afe_tick_align_ul_t ul_tick_select;
        switch (tick_source) {
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S0_OUT:
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S0_IN:
                ul_tick_select = AFE_TICK_ALIGN_UL_SLAVE_I2S0_IN;
                break;
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S1_OUT:
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S1_IN:
                ul_tick_select = AFE_TICK_ALIGN_UL_SLAVE_I2S1_IN;
                break;
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S2_OUT:
            case HAL_AUDIO_PATH_TICK_SOURCE_I2S2_IN:
                ul_tick_select = AFE_TICK_ALIGN_UL_SLAVE_I2S2_IN;
                break;
            case HAL_AUDIO_PATH_TICK_SOURCE_DOWN_SAMPLER_OUTPUT_CH0:
                ul_tick_select = AFE_TICK_ALIGN_UL_DOWNSAMPLER_CH0;
                break;
            case HAL_AUDIO_PATH_TICK_SOURCE_DOWN_SAMPLER_OUTPUT_CH2:
                ul_tick_select = AFE_TICK_ALIGN_UL_DOWNSAMPLER_CH2;
                break;
            default:
                return true;
                break;
        }
        AFE_SET_REG(AFE_I2S_SLV_ENGEN_CON0, ((enable|(ul_tick_select<<1))<<register_offset), (0xF<<register_offset));
    }
    return false;
}

bool hal_tick_align_set_irq(hal_audio_agent_t agent, bool enable)
{
    hal_audio_irq_audiosys_t irq;
    uint32_t irq_samplerate_register, irq_samplerate_shift;

    irq = hal_memory_convert_audiosys_irq_number(agent);

    irq_samplerate_register = AFE_I2S_SLV_ENGEN_CON2;
    switch (irq) {
        case AFE_AUDIOSYS_IRQ0:
            irq_samplerate_register = AFE_I2S_SLV_ENGEN_CON1;
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON1_IRQ0_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ1:
            irq_samplerate_register = AFE_I2S_SLV_ENGEN_CON1;
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON1_IRQ1_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ2:
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON2_IRQ2_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ3:
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON2_IRQ3_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ4:
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON2_IRQ4_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ5:
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON2_IRQ5_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ6:
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON2_IRQ6_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ7:
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON2_IRQ7_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ11:
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON2_IRQ11_MODE_POS;
            break;
        case AFE_AUDIOSYS_IRQ12:
            irq_samplerate_shift = AFE_I2S_SLV_ENGEN_CON2_IRQ12_MODE_POS;
            enable = false;//Keep false to reserve AWB2 ISR for sub-source
            break;
        default:
            return true;
            break;
    }
    AFE_SET_REG(irq_samplerate_register, (enable|(AFE_TICK_ALIGN_IRQ_MEMORY_AGENT<<1))<<irq_samplerate_shift, 0xF<<irq_samplerate_shift);
    return false;
}


/*******************************************************************************************
*                                     UL/DL device                                         *
********************************************************************************************/
void hal_audio_adda_set_enable_register(bool enable)
{
    AFE_SET_REG(AFE_ADDA_UL_DL_CON0, enable<<AFE_ADDA_UL_DL_CON0_ADDA_ON_POS, AFE_ADDA_UL_DL_CON0_ADDA_ON_MASK);
}

bool hal_audio_ul_set_enable(hal_audio_agent_t agent, bool enable)
{
    uint32_t reg = afe_samplerate_get_ul_register(agent);
    //Workaround : Toggle enable to reset filter
    AFE_SET_REG(reg, 1<<AFE_ADDA_UL_SRC_CON0_ON_POS, AFE_ADDA_UL_SRC_CON0_ON_MASK);
    AFE_SET_REG(reg, 0<<AFE_ADDA_UL_SRC_CON0_ON_POS, AFE_ADDA_UL_SRC_CON0_ON_MASK);
    AFE_SET_REG(reg, enable<<AFE_ADDA_UL_SRC_CON0_ON_POS, AFE_ADDA_UL_SRC_CON0_ON_MASK);
    return false;
}

bool hal_audio_ul_iir_set_reg(int16_t *table){
    AFE_SET_REG(AFE_ADDA_IIR_COEF_02_01,table[0],0x0000FFFF);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_02_01,table[1]<<16,0xFFFF0000);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_04_03,table[2],0x0000FFFF);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_04_03,table[3]<<16,0xFFFF0000);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_06_05,table[4],0x0000FFFF);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_06_05,table[5]<<16,0xFFFF0000);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_08_07,table[6],0x0000FFFF);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_08_07,table[7]<<16,0xFFFF0000);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_10_09,table[8],0x0000FFFF);
    AFE_SET_REG(AFE_ADDA_IIR_COEF_10_09,table[9]<<16,0xFFFF0000);
    return false;
}

bool hal_audio_ul_set_iir(hal_audio_agent_t agent, hal_audio_ul_iir_t iir_filter, bool enable)
{
    uint32_t reg = afe_samplerate_get_ul_register(agent);
    if (iir_filter == HAL_AUDIO_UL_IIR_DISABLE) {
        enable = false;
        iir_filter = 0;
    }
    //iir_filter = HAL_AUDIO_UL_IIR_100HZ_AT_48KHZ;
    HAL_AUDIO_LOG_INFO("[DSP DEBUG] hal_audio_ul_set_iir = %d",1, iir_filter);
    if (iir_filter > HAL_AUDIO_UL_IIR_75HZ_AT_48KHZ){

        if(iir_filter == HAL_AUDIO_UL_IIR_100HZ_AT_48KHZ){ /*SW mode: choose biquad table according to iir filter enum*/
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            //memcpy((uint32_t *)AFE_ADDA_IIR_COEF_02_01, afe_ul_biquad_table_100, sizeof(afe_ul_biquad_table_100));
            //HAL_AUDIO_LOG_INFO("[DSP DEBUG] %4x %4x %4x %4x %4x %4x %4x %4x %4x %4x",10,afe_ul_biquad_table_100[0],afe_ul_biquad_table_100[1],afe_ul_biquad_table_100[2],afe_ul_biquad_table_100[3],afe_ul_biquad_table_100[4],afe_ul_biquad_table_100[5],afe_ul_biquad_table_100[6],afe_ul_biquad_table_100[7],afe_ul_biquad_table_100[8],afe_ul_biquad_table_100[9]);
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_100);
        }else if(iir_filter == HAL_AUDIO_UL_IIR_125HZ_AT_48KHZ){
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_125);
        }else if(iir_filter == HAL_AUDIO_UL_IIR_150HZ_AT_48KHZ){
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_150);
        }else if(iir_filter == HAL_AUDIO_UL_IIR_175HZ_AT_48KHZ){
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_175);
        }else if(iir_filter == HAL_AUDIO_UL_IIR_200HZ_AT_48KHZ){
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_200);
        }else if(iir_filter == HAL_AUDIO_UL_IIR_225HZ_AT_48KHZ){
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_225);
        }else if(iir_filter == HAL_AUDIO_UL_IIR_250HZ_AT_48KHZ){
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_250);
        }else if(iir_filter == HAL_AUDIO_UL_IIR_275HZ_AT_48KHZ){
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_275);
        }else if(iir_filter == HAL_AUDIO_UL_IIR_300HZ_AT_48KHZ){
            iir_filter = HAL_AUDIO_UL_IIR_SW;
            hal_audio_ul_iir_set_reg((int16_t*)&afe_ul_biquad_table_300);
        }
    }
    //HAL_AUDIO_LOG_INFO("[DSP DEBUG] COEF_02_01=0x%8x, COEF_04_03=0x%8x ",2, AFE_GET_REG(AFE_ADDA_IIR_COEF_02_01), AFE_GET_REG(AFE_ADDA_IIR_COEF_04_03));        
    //HAL_AUDIO_LOG_INFO("[DSP DEBUG] COEF_06_05=0x%8x, COEF_08_07=0x%8x, COEF_10_09=0x%8x ",3, AFE_GET_REG(AFE_ADDA_IIR_COEF_06_05), AFE_GET_REG(AFE_ADDA_IIR_COEF_08_07), AFE_GET_REG(AFE_ADDA_IIR_COEF_10_09));
    AFE_SET_REG(reg, (iir_filter<<AFE_ADDA_UL_SRC_CON0_IIR_MODE_POS)|(enable<<AFE_ADDA_UL_SRC_CON0_IIR_POS), AFE_ADDA_UL_SRC_CON0_IIR_MODE_MASK|AFE_ADDA_UL_SRC_CON0_IIR_MASK);
    return false;
}

uint32_t hal_audio_ul_get_iir(hal_audio_agent_t agent)
{
    uint32_t reg = afe_samplerate_get_ul_register(agent);
    if(reg) {
        reg = (AFE_GET_REG(reg)&AFE_ADDA_UL_SRC_CON0_IIR_MODE_MASK)>>AFE_ADDA_UL_SRC_CON0_IIR_MODE_POS;
    }
    return reg;
}

bool hal_audio_ul_set_hires(hal_audio_agent_t agent, bool enable)
{
    if (afe_samplerate_get_ul_samplerate(agent)>48000) {
        hal_audio_clock_enable_adc_hires(enable);
    }
    return false;
}

bool hal_audio_ul_set_dmic_bias(hal_audio_agent_t agent, bool enable)
{
    UNUSED(agent);
    AFE_SET_REG(AUDENC_ANA_CON40, (enable<<AUDENC_ANA_CON40_DMIC_BIAS_EN_POS), AUDENC_ANA_CON40_DMIC_BIAS_EN_MASK);
    return false;
}

bool hal_audio_ul_set_dmic_phase(hal_audio_agent_t agent, uint32_t phase_ch1, uint32_t phase_ch2)
{
    uint32_t reg = afe_samplerate_get_ul_register(agent);
    AFE_SET_REG(reg, (phase_ch1<<AFE_ADDA_UL_SRC_CON0_DMIC_PHASE_CH1_POS)|(phase_ch2<<AFE_ADDA_UL_SRC_CON0_DMIC_PHASE_CH2_POS), AFE_ADDA_UL_SRC_CON0_DMIC_PHASE_CH1_MASK|AFE_ADDA_UL_SRC_CON0_DMIC_PHASE_CH2_MASK);
    return false;
}

bool hal_audio_ul_set_dmic_clock(hal_audio_agent_t agent, afe_dmic_clock_rate_t clock_rate)
{
    uint32_t reg = afe_samplerate_get_ul_register(agent);
    bool is_not_3250khz = 0;
    #if (HAL_AUDIO_DMIC_CLOCK_SETTING)
    is_not_3250khz = (clock_rate != AFE_DMIC_CLOCK_3_25M);
    #endif
    AFE_SET_REG(reg, (is_not_3250khz<<AFE_ADDA_UL_SRC_CON0_DMIC_SEL_CTRL_POS)|(clock_rate<<AFE_ADDA_UL_SRC_CON0_DMIC_CLK_RATE_POS), AFE_ADDA_UL_SRC_CON0_DMIC_SEL_CTRL_MASK|AFE_ADDA_UL_SRC_CON0_DMIC_CLK_RATE_MASK);
    return false;
}

bool hal_audio_ul_set_dmic_selection (hal_audio_agent_t agent, hal_audio_dmic_selection_t dmic_selection)
{
    uint32_t clock_offset, clock_value;
    uint32_t data_offset, data_value, sel_offset, sel_value;
    switch (agent) {
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1:
            clock_value = AFE_DMIC_CLOCK_DEVICE_INTERFACE_1;
            data_offset = TOP_DMIC_DAT_SEL_UL1_POS;
            sel_offset = TOP_DMIC_DAT_SEL_UL1_SEL_POS;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL2:
            clock_value = AFE_DMIC_CLOCK_DEVICE_INTERFACE_2;
            data_offset = TOP_DMIC_DAT_SEL_UL2_POS;
            sel_offset = TOP_DMIC_DAT_SEL_UL2_SEL_POS;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL3:
            clock_value = AFE_DMIC_CLOCK_DEVICE_INTERFACE_3;
            data_offset = TOP_DMIC_DAT_SEL_UL3_POS;
            sel_offset = TOP_DMIC_DAT_SEL_UL3_SEL_POS;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL4:
            clock_value = AFE_DMIC_CLOCK_DEVICE_INTERFACE_4;
            data_offset = TOP_DMIC_DAT_SEL_UL4_POS;
            sel_offset = TOP_DMIC_DAT_SEL_UL4_SEL_POS;
            break;
        default:
            return true;
            break;
    }

    switch (dmic_selection) {
        case HAL_AUDIO_DMIC_GPIO_DMIC0:
            clock_offset = TOP_DMIC_CK_SEL_GPIO_DMIC0_POS;
            break;
        case HAL_AUDIO_DMIC_GPIO_DMIC1:
            clock_offset = TOP_DMIC_CK_SEL_GPIO_DMIC1_POS;
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC0:
            clock_offset = TOP_DMIC_CK_SEL_ANA_DMIC0_POS;
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC1:
            clock_offset = TOP_DMIC_CK_SEL_ANA_DMIC1_POS;
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC2:
            clock_offset = TOP_DMIC_CK_SEL_ANA_DMIC2_POS;
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC3:
            clock_offset = TOP_DMIC_CK_SEL_ANA_DMIC3_POS;
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC4:
            clock_offset = TOP_DMIC_CK_SEL_ANA_DMIC4_POS;
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC5:
            clock_offset = TOP_DMIC_CK_SEL_ANA_DMIC5_POS;
            break;
        default:
            return true;
            break;
    }

    if (dmic_selection <= HAL_AUDIO_DMIC_ANA_DMIC1) {
        data_value = dmic_selection;

        sel_value = 0;
    } else {
        AFE_SET_REG(TOP_DMIC_DAT_SEL, (0<<data_offset), (3<<data_offset));
        data_value = dmic_selection - HAL_AUDIO_DMIC_ANA_DMIC2;
        data_offset = data_offset + (TOP_DMIC_DAT_SEL_UL1_PRE_POS - TOP_DMIC_DAT_SEL_UL1_POS);
        sel_value = 1;
    }

    AFE_SET_REG(TOP_DMIC_CK_SEL, (clock_value<<clock_offset), (3<<clock_offset));
    AFE_SET_REG(TOP_DMIC_DAT_SEL, (sel_value<<sel_offset)|(data_value<<data_offset), (1<<sel_offset)|(3<<data_offset));
    HAL_AUDIO_LOG_INFO("agent %d dmic_selection %d",2,agent,dmic_selection);
    return false;
}

bool hal_audio_ul_set_dmic_enable(hal_audio_agent_t agent, bool enable)
{
    uint32_t reg = afe_samplerate_get_ul_register(agent);
    AFE_SET_REG(reg,
                (enable<<AFE_ADDA_UL_SRC_CON0_SDM3_FOR_DMIC_POS)|(enable<<AFE_ADDA_UL_SRC_CON0_DMIC_CH1_POS)|(enable<<AFE_ADDA_UL_SRC_CON0_DMIC_CH2_POS),
                AFE_ADDA_UL_SRC_CON0_SDM3_FOR_DMIC_MASK|AFE_ADDA_UL_SRC_CON0_DMIC_CH1_MASK|AFE_ADDA_UL_SRC_CON0_DMIC_CH2_MASK);
    return false;
}

bool hal_audio_ul_set_da_loopback_enable(hal_audio_agent_t agent, bool enable)
{
    uint32_t reg = afe_samplerate_get_ul_register(agent);
    AFE_SET_REG(reg,
                (enable<<AFE_ADDA_UL_SRC_CON0_DA_LOOPBACK_POS),
                AFE_ADDA_UL_SRC_CON0_DA_LOOPBACK_MASK);
    return false;
}

bool hal_audio_ul_get_fifo_clock_sel(hal_audio_agent_t agent)
{
    UNUSED(agent);
    //return 1 while select CH1, others is CH0
    return (AFE_GET_REG(AFUNC_AUD_CON4)&AFUNC_AUD_CON4_ANA_CLK_SEL_MASK);
}

bool hal_audio_ul_reset_fifo(hal_audio_agent_t device_agent, bool enable)
{
    uint32_t reg_offset;
    switch (device_agent) {
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1:
            reg_offset = AFE_ADDA_UL_DL_CON0_ADDA_FIFO_RST_POS;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL2:
            reg_offset = AFE_ADDA_UL_DL_CON0_ADDA2_FIFO_RST_POS;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL3:
            reg_offset = AFE_ADDA_UL_DL_CON0_ADDA6_FIFO_RST_POS;
            break;
        default:
            return true;
            break;
    }

    AFE_SET_REG(AFE_ADDA_UL_DL_CON0, (enable<<reg_offset), (1<<reg_offset));
    return false;
}

bool hal_audio_ul_set_swap(hal_audio_agent_t device_agent, bool enable)
{
    uint32_t reg_offset;
    switch (device_agent) {
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1:
            reg_offset = AFE_ADDA_UL_DL_CON0_ADDA_SWAP_POS;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL2:
            reg_offset = AFE_ADDA_UL_DL_CON0_ADDA2_SWAP_POS;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL3:
            reg_offset = AFE_ADDA_UL_DL_CON0_ADDA6_SWAP_POS;
            break;
        default:
            return true;
            break;
    }

    AFE_SET_REG(AFE_ADDA_UL_DL_CON0, (enable<<reg_offset), (1<<reg_offset));
    return false;
}

bool hal_audio_ul_set_inverse(hal_audio_agent_t device_agent, bool enable)
{
    uint32_t reg_offset;
    switch (device_agent) {
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1:
            reg_offset = AUDIO_TOP_CON2_ADC01_INV_POS;
            break;
        #if 0
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL2:
            reg_offset = AUDIO_TOP_CON2_ADC23_INV_POS;
            break;
        case HAL_AUDIO_AGENT_DEVICE_ADDA_UL3:
            reg_offset = AUDIO_TOP_CON2_ADC45_INV_POS;
            break;
        #endif
        default:
            return true;
            break;
    }

    AFE_SET_REG(AUDIO_TOP_CON2, (enable<<reg_offset), (1<<reg_offset));
    return false;
}

bool hal_audio_ul4_set_loopback(hal_audio_ul_loopback_setting_t loopback_setting, bool enable)
{
    bool bool_anc = false, bool_loopback_mux = false;
    if (enable) {
        if (loopback_setting==AFE_AUDIO_UL_LOOPBACK_FROM_ANC) {
            bool_anc = true;
        } else if (loopback_setting==AFE_AUDIO_UL_LOOPBACK_FROM_7BIT_SDM) {
            bool_anc = true;
            bool_loopback_mux = true;
        }
    }
    AFE_SET_REG(AFE_ADDA_ANC_UL_SRC_CON0,
                (bool_anc<<AFE_ADDA_ANC_UL_SRC_CON0_ANC_LOOPBACK_POS)|(bool_loopback_mux<<AFE_ADDA_ANC_UL_SRC_CON0_LOOPBACK_MUX_POS),
                AFE_ADDA_ANC_UL_SRC_CON0_ANC_LOOPBACK_MASK|AFE_ADDA_ANC_UL_SRC_CON0_LOOPBACK_MUX_MASK);
    return false;
}

bool hal_audio_dl_set_fifo_swap(bool no_swap)
{
    AFE_SET_REG(AFE_ADDA_DL_SDM_DCCOMP_CON, no_swap<<AFE_ADDA_DL_SDM_DCCOMP_CON_FIFO_SWAP_POS, AFE_ADDA_DL_SDM_DCCOMP_CON_FIFO_SWAP_MASK);
    return false;
}

bool hal_audio_dl_set_sdm(hal_audio_dl_sdm_setting_t sdm_setting, bool enable)
{
    if (sdm_setting == AFE_AUDIO_DL_SDM_5_BIT) {
        hal_audio_ul_set_da_loopback_enable(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1, enable);
    } else {
        hal_audio_ul_set_da_loopback_enable(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1, false);
        if (sdm_setting == AFE_AUDIO_DL_SDM_7_BIT) {
            AFE_SET_REG(AFE_ADDA_DL_SDM_DCCOMP_CON, (enable<<AFE_ADDA_DL_SDM_DCCOMP_CON_7BIT_POS), AFE_ADDA_DL_SDM_DCCOMP_CON_7BIT_MASK);
        }
    }
    if (hal_volume_get_analog_mode(AFE_HW_ANALOG_GAIN_OUTPUT) == HAL_AUDIO_ANALOG_OUTPUT_CLASSD)  {
        AFE_SET_REG(AFE_ADDA_DL_DITHER_CON, (enable<<AFE_ADDA_DL_DITHER_CON_SDM_DITHER_EN_POS), AFE_ADDA_DL_DITHER_CON_SDM_DITHER_EN_MASK); //enable dither
    }
    return false;
}

hal_audio_dl_sdm_setting_t hal_audio_dl_get_sdm(void)
{
    hal_audio_dl_sdm_setting_t sdm_setting = AFE_AUDIO_DL_SDM_9_BIT;
    if (AFE_GET_REG(AFE_ADDA_UL_SRC_CON0)&AFE_ADDA_UL_SRC_CON0_DA_LOOPBACK_MASK) {
        sdm_setting = AFE_AUDIO_DL_SDM_5_BIT;
    } else if (AFE_GET_REG(AFE_ADDA_DL_SDM_DCCOMP_CON)&AFE_ADDA_DL_SDM_DCCOMP_CON_7BIT_MASK) {
        sdm_setting = AFE_AUDIO_DL_SDM_7_BIT;
    }
    return sdm_setting;
}

bool hal_audio_dl_set_inverse(bool enable)
{
    //Lch only
    AFE_SET_REG(AFUNC_AUD_CON0, enable<<AFUNC_AUD_CON0_CCI_LCH_INV_POS, AFUNC_AUD_CON0_CCI_LCH_INV_MASK);
    return false;
}

bool hal_audio_dl_set_src_enable(bool enable)
{
    AFE_SET_REG(AFE_ADDA_DL_SRC2_CON0, enable<<AFE_ADDA_DL_SRC2_CON0_DL_SRC_ON_POS, AFE_ADDA_DL_SRC2_CON0_DL_SRC_ON_MASK);
    return false;
}

bool hal_audio_dl_set_hires(hal_audio_agent_t agent, bool enable)
{
    //uint32_t reg = afe_samplerate_get_ul_register(agent);
    UNUSED(agent);
    if (afe_samplerate_get_dl_samplerate()>48000) {
        hal_audio_clock_enable_dac_hires(enable);
    }
    return false;
}

bool hal_audio_dl_set_sdm_enable(bool enable)
{
    if(enable) {
        AFE_SET_REG(AFUNC_AUD_CON2, (1<<AFUNC_AUD_CON2_SDM_CLK_POS)|(1<<AFUNC_AUD_CON2_SDM_FT_TEST_POS), AFUNC_AUD_CON2_SDM_CLK_MASK|AFUNC_AUD_CON2_SDM_FT_TEST_MASK);    //sdm audio fifo clock power on
        AFE_SET_REG(AFUNC_AUD_CON0, 0xCBA1, 0xFFFF);    //scrambler clock on enable
        AFE_SET_REG(AFUNC_AUD_CON2, (1<<AFUNC_AUD_CON2_SDM_FIFO_RSTB_POS)|(0<<AFUNC_AUD_CON2_SDM_FT_TEST_POS),AFUNC_AUD_CON2_SDM_FIFO_RSTB_MASK|AFUNC_AUD_CON2_SDM_FT_TEST_MASK);    //sdm power on
        AFE_SET_REG(AFUNC_AUD_CON2, (1<<AFUNC_AUD_CON2_SDM_ENABLE_POS), AFUNC_AUD_CON2_SDM_ENABLE_MASK);    //sdm fifo enable
        AFE_SET_REG(AFE_ANA_GAIN_MUX, 0x5a, 0x005f);    //set HP gain control to ZCD module
    } else {
        AFE_SET_REG(AFUNC_AUD_CON0, 0xC820, 0xFFFF);
        AFE_SET_REG(AFUNC_AUD_CON2, (0<<AFUNC_AUD_CON2_SDM_ENABLE_POS)|(0<<AFUNC_AUD_CON2_SDM_CLK_POS), AFUNC_AUD_CON2_SDM_ENABLE_MASK|AFUNC_AUD_CON2_SDM_CLK_MASK);    //sdm fifo disable and clock off

    }
    return false;
}

bool hal_audio_dl_set_classg(bool enable)
{
    if(enable) {
        //hal_audio_clock_enable_classg(true);
        //fix class hpf mode
        //AFE_WRITE(AFE_CLASSG_CFG2, 0x0000040A);              //AFE_CLASSG_CFG3 TH ~-23.9dBV (0.25mW)
        AFE_WRITE(AFE_CLASSG_CFG2, 0x00000890);

        AFE_WRITE(AFE_CLASSG_CFG3, 0x08250825);              //classg threshold initial setting, allen cmm no this line
        AFE_WRITE(AFE_CLASSG_CFG4, 0x00000000);              //classg level up & level down transition time selection
        AFE_WRITE(AFE_CLASSG_CFG1, 0x00181831);              //set classg preview window to 500us
        AFE_WRITE(AFE_CLASSG_CFG0, 0x11004001);              //level down hold time setting and classg enable

    } else {
        //hal_audio_clock_enable_classg(false);
    }
    return false;
}

bool hal_audio_dl_set_classd(bool enable)
{
    if(enable) {
        //hal_audio_clock_enable_classg(true);
        AFE_WRITE(AFE_CLASSG_CFG5, 0x00000003);
        //printf("AFE_CLASSG_CFG5 addr 0x%x,0x%x",AFE_CLASSG_CFG5,AFE_READ(AFE_CLASSG_CFG5));
    } else {
        //hal_audio_clock_enable_classg(false);
    }
    return false;
}

bool hal_audio_dl_set_classg_enable(bool enable)
{
    #ifdef HAL_AUDIO_LEGACY_ABB_DRIVER
    ANA_SET_REG (   AUDDEC_ANA_CON11, 0<<AUDDEC_ANA_CON11_CLASSG_EN_POS, AUDDEC_ANA_CON11_CLASSG_EN_MASK);
    if (enable) {
        HAL_AUDIO_DELAY_US(20);
        ANA_SET_REG (AUDDEC_ANA_CON11, 1<<AUDDEC_ANA_CON11_CLASSG_EN_POS, AUDDEC_ANA_CON11_CLASSG_EN_MASK);
    }
    #else
    UNUSED(enable);
    #endif
    return false;
}

bool hal_audio_dl_set_classg_monitor(void)
{
    //SW workaround:HP single-ended common-mode ripple
    uint32_t afe_classg_montor = 0;
    afe_classg_montor = AFE_READ(AFE_CLASSG_MON0)&AFE_CLASSG_MON0_VAUD18SEL_MASK;
    if (!afe_classg_montor) {
        hal_audio_dl_set_classg_enable(true);
    }
    return false;
}

#ifdef AIR_A2DP_NLE_ENABLE
bool hal_audio_dl_set_nle_enable(bool enable)
{
    if(enable) {
        AFE_SET_REG(AFE_BASE, 0 << 15, 1 << 15);                            // turn on nle clock source
        AFE_SET_REG(AFE_BASE + 0xe6c, (0<<4) | (0<<6), (1<<4) | (1<<6));    // open zcd_gain_mux
        AFE_SET_REG(AFE_BASE + 0xe6c, (1<<0) | (1<<2), (3<<0) | (3<<2));    // open classg_gain_mux
        AFE_WRITE(AFE_DL_NLE_L_CFG1, 0x00006880);                           // turn on L-ch nle module
        AFE_WRITE(AFE_DL_NLE_R_CFG1, 0x00006880);                           // turn on R-ch nle module
        HAL_AUDIO_LOG_INFO("[NLE ON] turn on nle\r\n",0);
        HAL_AUDIO_LOG_INFO("[NLE ON] AFE_BASE:0x%x, 0xc0000e6c:0x%x, AFE_DL_NLE_L_CFG1:0x%x", 3,AFE_READ(AFE_BASE),AFE_READ(0xc0000e6c),AFE_READ(AFE_DL_NLE_L_CFG1));
        open_nle = true;
        hal_audio_dl_set_nle_gain(false);
    } else {
        AFE_SET_REG(AFE_BASE + 0xe6c, (1<<4) | (1<<6), (1<<4) | (1<<6));  // close zcd_gain_mux
        AFE_SET_REG(AFE_BASE + 0xe6c, (2<<0) | (2<<2), (3<<0) | (3<<2));  // close classg_gain_mux
        AFE_SET_REG(AFE_DL_NLE_L_CFG1, 0 << 7, 1 << 7);                   // turn off L-ch nle module
        AFE_SET_REG(AFE_DL_NLE_R_CFG1, 0 << 7, 1 << 7);                   // turn off R-ch nle module
        AFE_SET_REG(AFE_BASE, 1 << 15, 1 << 15);                          // turn off nle clock source
        HAL_AUDIO_LOG_INFO("[NLE OFF] turn off nle\r\n",0);
        HAL_AUDIO_LOG_INFO("[NLE OFF] AFE_BASE:0x%x, 0xc0000e6c:0x%x, AFE_DL_NLE_L_CFG1:0x%x", 3,AFE_READ(AFE_BASE),AFE_READ(0xc0000e6c),AFE_READ(AFE_DL_NLE_L_CFG1));
    }
    return false;
}
#endif

#ifdef AIR_A2DP_NLE_ENABLE
extern int32_t hal_audio_agent_user_count[HAL_AUDIO_AGENT_NUMBERS];
static S32 AFE_NLE_GAIN_LIMIT_DB = -29; // RANGE: 0~-44
bool hal_audio_dl_set_nle_gain(bool enable)
{
    if(enable) {
        if (open_nle == true) {
            return false;
        } else if (open_nle == false) {
            S32 A_GAIN_L_DB = 12 - (AFE_READ(ZCD_CON2) & 0x3f);                 // read L-ch analog initial gain
            S32 A_GAIN_R_DB = 12 - (AFE_READ(ZCD_CON2) & 0xfc0 >> 6);           // read R-ch analog initial gain
            S32 D_GAIN_L_DB = 0;
            S32 D_GAIN_R_DB = 0;

            if ((A_GAIN_L_DB + AFE_NLE_GAIN_LIMIT_DB) < -32) {
                D_GAIN_L_DB = 32 + A_GAIN_L_DB;
                A_GAIN_L_DB = -32;
            } else {
                D_GAIN_L_DB =  - AFE_NLE_GAIN_LIMIT_DB;
                A_GAIN_L_DB = A_GAIN_L_DB + AFE_NLE_GAIN_LIMIT_DB;
            }
            if ((A_GAIN_R_DB + AFE_NLE_GAIN_LIMIT_DB) < -32) {
                D_GAIN_R_DB = 32 + A_GAIN_R_DB;
                A_GAIN_R_DB = -32;
            } else {
                D_GAIN_R_DB =  - AFE_NLE_GAIN_LIMIT_DB;
                A_GAIN_R_DB = A_GAIN_R_DB + AFE_NLE_GAIN_LIMIT_DB;
            }
            if (hal_audio_agent_user_count[HAL_AUDIO_AGENT_DEVICE_ANC] > 0) {
                return false;
            }
            AFE_SET_REG(AFE_DL_NLE_L_CFG0, ((1<<0) | (12 - A_GAIN_L_DB)<<16) | (D_GAIN_L_DB<<24), (1<<0) | (0x3f<<16) | (0x3f<<24));//set L-ch digital and analog target gain (d_gain+threshold_db,a_gain-threshold_db)
            AFE_SET_REG(AFE_DL_NLE_R_CFG0, ((1<<0) | (12 - A_GAIN_R_DB)<<16) | (D_GAIN_R_DB<<24), (1<<0) | (0x3f<<16) | (0x3f<<24));//set R-ch digital and analog target gain (d_gain+threshold_db,a_gain-threshold_db)

            AFE_WRITE(AFE_DL_NLE_L_CFG1, 0x01016880);                          // toggle ready flag to update rg into nle module
            AFE_WRITE(AFE_DL_NLE_R_CFG1, 0x01016880);                          // toggle ready flag to update rg into nle module
            open_nle = true;
            HAL_AUDIO_LOG_INFO("[NLE GAIN ON] apply nle gain on\r\n",0);
            HAL_AUDIO_LOG_INFO("[NLE GAIN ON] A_GAIN_L_DB=%d,D_GAIN_L_DB=%d",2,A_GAIN_L_DB,D_GAIN_L_DB);
            HAL_AUDIO_LOG_INFO("[NLE GAIN ON] ZCD_CON2:0x%x, AFE_DL_NLE_L_CFG0:0x%x,AFE_DL_NLE_L_CFG1:0x%x", 3,AFE_READ(ZCD_CON2), AFE_READ(AFE_DL_NLE_L_CFG0), AFE_READ(AFE_DL_NLE_L_CFG1));
        }
    } else {
        if (open_nle == false) {
            return false;
        } else if (open_nle == true) {
            S32 A_GAIN_L_DB = 12 - (AFE_READ(ZCD_CON2) & 0x3f);                 // read L-ch analog initial gain
            S32 A_GAIN_R_DB = 12 - (AFE_READ(ZCD_CON2) & 0xfc0 >> 6);           // read R-ch analog initial gain
            S32 D_GAIN_L_DB = 0;
            S32 D_GAIN_R_DB = 0;

            AFE_SET_REG(AFE_DL_NLE_L_CFG0, ((1<<0) | (12 - A_GAIN_L_DB)<<16) | (D_GAIN_L_DB<<24), (1<<0) | (0x3f<<16) | (0x3f<<24));// set L-ch analog initial gain
            AFE_SET_REG(AFE_DL_NLE_R_CFG0, ((1<<0) | (12 - A_GAIN_R_DB)<<16) | (D_GAIN_R_DB<<24), (1<<0) | (0x3f<<16) | (0x3f<<24));// set R-ch analog initial gain
            AFE_WRITE(AFE_DL_NLE_L_CFG1, 0x01006880);                          // initiate L-ch analog gain
            AFE_WRITE(AFE_DL_NLE_R_CFG1, 0x01006880);                          // initiate R-ch analog gain
            open_nle = false;
            HAL_AUDIO_LOG_INFO("[NLE GAIN OFF] apply nle gain off \r\n",0);
            HAL_AUDIO_LOG_INFO("[NLE GAIN OFF] A_GAIN_L_DB=%d,D_GAIN_L_DB=%d",2,A_GAIN_L_DB,D_GAIN_L_DB);
            HAL_AUDIO_LOG_INFO("[NLE GAIN OFF] ZCD_CON2:0x%x, AFE_DL_NLE_L_CFG0:0x%x,AFE_DL_NLE_L_CFG1:0x%x", 3,AFE_READ(ZCD_CON2), AFE_READ(AFE_DL_NLE_L_CFG0), AFE_READ(AFE_DL_NLE_L_CFG1));
        }
    }
    return false;
}
#endif

void hal_audio_bubsort(uint32_t *p, uint32_t entries)
{
    uint32_t i, j, swap;

    if ((p == NULL)||(entries == 0)) {
        return;
    }

    for (i=0; i < (entries-1); i++) {
        for (j = 0; j < (entries-1)-i; j++) {
            if (p[j] > p[j+1]) {
                swap = p[j];
                p[j] = p[j+1];
                p[j+1] = swap;
            }
        }
    }
}

#define HAL_AUDIO_DL_COMP_ADC_ENABLE                        (flase)
#define HAL_AUDIO_DL_COMP_COUNT                             (100)
#define HAL_AUDIO_DL_COMP_ABANDON_HEAD                      (5)
#define HAL_AUDIO_DL_COMP_ABANDON_TAIL                      (5)
#define HAL_AUDIO_DL_COMP_COUNT_NUMBER                      (HAL_AUDIO_DL_COMP_COUNT-HAL_AUDIO_DL_COMP_ABANDON_HEAD-HAL_AUDIO_DL_COMP_ABANDON_TAIL)
#define HAL_AUDIO_DL_COMP_SHRINK_VALUE                      (0.125893)
#define HAL_AUDIO_DL_COMP_SHRINK_TRIM_STEP_KILO_VALUE       (4183)  //(0.18*4096/1400/HAL_AUDIO_DL_COMP_SHRINK_VALUE*1000) = 4.1831441893399269901537700382751*1000
#define HAL_AUDIO_DL_COMP_SHRINK_FINE_TRIM_STEP_KILO_VALUE  (1162)  //(0.05*4096/1400/HAL_AUDIO_DL_COMP_SHRINK_VALUE*1000) = 1.1619844970388686083760472328542*1000


uint32_t hal_audio_dl_get_classab_compensation_value(void)
{
    #if (HAL_AUDIO_DL_COMP_ADC_ENABLE)
    bool sign_bit_l, sign_bit_r;
    uint8_t channel, count = 0, loop = 2, trim_l = 0, trim_r = 0, fine_trim_l = 0, fine_trim_r = 0;
    uint16_t channel_sel[4]={0x0071, 0x0074, 0x0072, 0x0073};//Trimming buffer mux selection with trimming buffer gain 18dB
    uint32_t sum[4]={0}, avg_p_l = 0, avg_p_r = 0, avg_n_l = 0, avg_n_r = 0;
    int32_t value_l, value_r;
    uint16_t total=0;
    uint32_t buffer_ptr = hal_memory_allocate_sram(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1, (HAL_AUDIO_DL_COMP_COUNT+1)*loop*sizeof(uint32_t)); //pvPortMalloc(2 * sizeof(unsigned int *));
    uint32_t **data = (uint32_t **)buffer_ptr;

    AFE_WRITE(AFE_CLASSG_CFG0, 0x11004089);// Setting voltage high to 0.9v
    ANA_SET_REG(AUDDEC_ANA_CON10, 0x1<<AUDDEC_ANA_CON10_PWR_VA33_POS, AUDDEC_ANA_CON10_PWR_VA33_MASK); // Enable trim buffer's reference voltage

    ANA_SET_REG(AUDDEC_ANA_CON11, (1<<AUDDEC_ANA_CON11_GLB_VCM_EN_POS), AUDDEC_ANA_CON11_GLB_VCM_EN_MASK);
    ANA_SET_REG(AUDDEC_ANA_CON6, (1<<AUDDEC_ANA_CON6_TRIMBUF_EN_POS)|(3<<AUDDEC_ANA_CON6_TRIMBUF_GAIN_SEL_POS), AUDDEC_ANA_CON6_TRIMBUF_EN_MASK|AUDDEC_ANA_CON6_TRIMBUF_GAIN_SEL_MASK);

    if(data == NULL) {
        HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio DC compensation memory allocate fail :%d !", 1, HAL_AUDIO_DL_COMP_COUNT*loop*sizeof(uint32_t));
        HAL_AUDIO_ASSERT(false);
    } else {
        buffer_ptr += loop*sizeof(uint32_t);
        for (loop = 0; loop < 2; loop++) {
            data[loop] = (uint32_t *)buffer_ptr;
            buffer_ptr += HAL_AUDIO_DL_COMP_COUNT*sizeof(uint32_t);
        }
    }

    hal_adc_init();

    #if 0
    hal_adc_get_data_polling
    #endif
    for (channel = 0 ; channel<2 ; channel++) {
        //Auxadc measure value, trimming buffer mux selection 0x71:LP, 0x74:LN, 0x72:RP, 0x73:RN
        for (count = 0; count < HAL_AUDIO_DL_COMP_COUNT; count++) {
            for (loop = 0; loop < 2; loop++) {
                ANA_SET_REG (AUDDEC_ANA_CON6,  channel_sel[2*channel+loop],  0xFFFF);
                ADC->AUXADC_CON1 = 0;
                ADC->AUXADC_CON1 = (1<<8);
                // Wait until the module status is idle
                while (ADC->AUXADC_CON3_UNION.AUXADC_CON3_CELLS.ADC_STAT & AUXADC_CON3_ADC_STA_MASK);
                data[loop][count] = AFE_READ(AUXADC_DATA8);
            }
        }

        //Abandon critical value and calculate sum of ordered data to get average value
        hal_audio_bubsort(&data[0][0], HAL_AUDIO_DL_COMP_COUNT);
        hal_audio_bubsort(&data[1][0], HAL_AUDIO_DL_COMP_COUNT);
        for (count = 0+HAL_AUDIO_DL_COMP_ABANDON_HEAD; count < HAL_AUDIO_DL_COMP_COUNT-HAL_AUDIO_DL_COMP_ABANDON_TAIL; count++) {
            sum[2*channel+0] = sum[2*channel+0] + data[0][count];
            sum[2*channel+1] = sum[2*channel+1] + data[1][count];
        }
    }
    avg_p_l = (sum[0]*1000 + (HAL_AUDIO_DL_COMP_COUNT_NUMBER>>1)) / HAL_AUDIO_DL_COMP_COUNT_NUMBER;
    avg_n_l = (sum[1]*1000 + (HAL_AUDIO_DL_COMP_COUNT_NUMBER>>1)) / HAL_AUDIO_DL_COMP_COUNT_NUMBER;
    avg_p_r = (sum[2]*1000 + (HAL_AUDIO_DL_COMP_COUNT_NUMBER>>1)) / HAL_AUDIO_DL_COMP_COUNT_NUMBER;
    avg_n_r = (sum[3]*1000 + (HAL_AUDIO_DL_COMP_COUNT_NUMBER>>1)) / HAL_AUDIO_DL_COMP_COUNT_NUMBER;

    hal_adc_deinit();

    hal_memory_free_sram(HAL_AUDIO_AGENT_DEVICE_ADDA_DL1);

    value_l = (int32_t)(avg_p_l - avg_n_l);
    value_r = (int32_t)(avg_p_r - avg_n_r);

    HAL_AUDIO_LOG_INFO("DSP - Hal Audio dl compensation value_l %d= %d(LP) - %d(LN)", 3, value_l, avg_p_l, avg_n_l);
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio dl compensation value_r %d= %d(RP) - %d(RN)", 3, value_r, avg_p_r, avg_n_r);

    if (value_l >= 0) {
        sign_bit_l = true;
    } else {
        sign_bit_l = false;
        value_l = -value_l;
    }
    if (value_r >= 0) {
        sign_bit_r = true;
    } else {
        sign_bit_r = false;
        value_r = -value_r;
    }

    trim_l = (value_l/HAL_AUDIO_DL_COMP_SHRINK_TRIM_STEP_KILO_VALUE)&0xF;
    fine_trim_l = (value_l/HAL_AUDIO_DL_COMP_SHRINK_FINE_TRIM_STEP_KILO_VALUE)&0x3;

    trim_r = (value_r/HAL_AUDIO_DL_COMP_SHRINK_TRIM_STEP_KILO_VALUE)&0xF;
    fine_trim_r = (value_r/HAL_AUDIO_DL_COMP_SHRINK_FINE_TRIM_STEP_KILO_VALUE)&0x3;

    total = sign_bit_r<<15 | fine_trim_r<<13 | sign_bit_r<<12 | trim_r<<8 | sign_bit_l<<7 | fine_trim_l<<5 | sign_bit_l<<4 | trim_l<<0;

    HAL_AUDIO_LOG_INFO("DSP - Hal Audio dl classAB compensation Result:0x%x", 1, total);

    ANA_SET_REG(AUDDEC_ANA_CON6, (0<<AUDDEC_ANA_CON6_TRIMBUF_EN_POS), AUDDEC_ANA_CON6_TRIMBUF_EN_MASK);
    ANA_SET_REG(AUDDEC_ANA_CON11, (0<<AUDDEC_ANA_CON11_GLB_VCM_EN_POS), AUDDEC_ANA_CON11_GLB_VCM_EN_MASK);

    ANA_SET_REG(AUDDEC_ANA_CON10, 0<<AUDDEC_ANA_CON10_PWR_VA33_POS, AUDDEC_ANA_CON10_PWR_VA33_MASK); // Disable trim buffer's reference voltage
    AFE_WRITE(AFE_CLASSG_CFG0, 0x11004001);// Setting voltage low to 0.42v

    return total;
    #else
    return 0;
    #endif
}

uint32_t hal_audio_dl_get_classd_compensation_value(void)
{
    //Need to Enable AMic ADC
    uint32_t value;
    value = ANA_READ(AUDENC_ANA_CON43);
    value = value&AUDENC_ANA_CON43_AUD01_RC_READ_MASK;
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio dl classD compensation Result:0x%x", 1, value);
    return value;
}
/*******************************************************************************************
*                                         SideTone                                         *
********************************************************************************************/
static const uint16_t sidetone_coefficient_table_16k[] = { //13-tap
    0x0127, 0x027A, 0x0278, 0x0227,
    0xFFD5, 0xFD22, 0xFABF, 0xFAEB,
    0xFE90, 0x05EB, 0x0F47, 0x180A,
    0x1D4E
};

static const uint16_t sidetone_coefficient_table_32k[] = { //29-tap
    0xFF58, 0x0063, 0x0086, 0x00BF,
    0x0100, 0x013D, 0x0169, 0x0178,
    0x0160, 0x011C, 0x00AA, 0x0011,
    0xFF5D, 0xFEA1, 0xFDF6, 0xFD75,
    0xFD39, 0xFD5A, 0xFDE8, 0xFEEA,
    0x005F, 0x0237, 0x0458, 0x069F,
    0x08E2, 0x0AF7, 0x0CB2, 0x0DF0,
    0x0E96
};

static const uint16_t sidetone_coefficient_table_48k[] = { //31-tap
    0x0100, 0xFFEC, 0xFFD6, 0xFFB3,
    0xFF84, 0xFF4A, 0xFF08, 0xFEC2,
    0xFE7C, 0xFE3A, 0xFE03, 0xFDDB,
    0xFDC9, 0xFDD2, 0xFDF9, 0xFE44,
    0xFEB3, 0xFF47, 0x0000, 0x00D9,
    0x01CE, 0x02D9, 0x03F0, 0x050C,
    0x0622, 0x0729, 0x0816, 0x08DF,
    0x097E, 0x09EC, 0x0A24
};

bool hal_sidetone_set_input(hal_audio_agent_t input_agent, bool enable)
{
    uint32_t register_value, sel_ctrl = 0;
    if(enable) {
        switch (input_agent) {
            case HAL_AUDIO_AGENT_DEVICE_ADDA_UL1:
                register_value = 4;
                break;
            case HAL_AUDIO_AGENT_DEVICE_ADDA_UL2:
                register_value = 5;
                break;
            case HAL_AUDIO_AGENT_DEVICE_ADDA_UL3:
                register_value = 6;
                break;
            case HAL_AUDIO_AGENT_DEVICE_ADDA_UL4:
                register_value = 0;
                sel_ctrl = 1;
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S0_MASTER:
                register_value = 0;
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S1_MASTER:
                register_value = 1;
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S2_MASTER:
                register_value = 2;
                break;
            case HAL_AUDIO_AGENT_DEVICE_STF_TICK_ALIGN:
                register_value = 3;
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE:
                register_value = 1;
                sel_ctrl = 1;
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE:
                register_value = 2;
                sel_ctrl = 1;
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE:
                register_value = 3;
                sel_ctrl = 1;
                break;
            default:
                return true;
                break;
        }
    } else {
        register_value = 0;
    }

    AFE_SET_REG(AFE_SIDETONE_DEBUG, (register_value<<AFE_SIDETONE_DEBUG_SRC_SEL_POS)|(sel_ctrl<<AFE_SIDETONE_DEBUG_SEL_CTRL_POS), AFE_SIDETONE_DEBUG_SRC_SEL_MASK|AFE_SIDETONE_DEBUG_SEL_CTRL_MASK);
    return false;
}

bool hal_sidetone_set_output(hal_audio_agent_t output_agent, bool enable)
{
    uint32_t register_value;
    if (enable) {
        switch (output_agent) {
            case HAL_AUDIO_AGENT_DEVICE_ADDA_DL1:
                register_value = ~(enable<<AFE_SIDETONE_CON1_INTO_DAC_POS);
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S0_MASTER:
                register_value = ~(enable<<AFE_SIDETONE_CON1_INTO_I2S0_MASTER_POS);
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S1_MASTER:
                register_value = ~(enable<<AFE_SIDETONE_CON1_INTO_I2S1_MASTER_POS);
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S2_MASTER:
                register_value = ~(enable<<AFE_SIDETONE_CON1_INTO_I2S2_MASTER_POS);
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S0_SLAVE:
                register_value = ~(enable<<AFE_SIDETONE_CON1_INTO_I2S0_SLAVE_POS);
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S1_SLAVE:
                register_value = ~(enable<<AFE_SIDETONE_CON1_INTO_I2S1_SLAVE_POS);
                break;
            case HAL_AUDIO_AGENT_DEVICE_I2S2_SLAVE:
                register_value = ~(enable<<AFE_SIDETONE_CON1_INTO_I2S2_SLAVE_POS);
                break;
            default:
                return true;
                break;
        }
    } else {
        register_value = 0xFFFFFFFF;
    }
    AFE_SET_REG(AFE_SIDETONE_CON1, register_value, AFE_SIDETONE_CON1_OUTPUT_MASK);
    return false;
}

bool hal_sidetone_set_filter(uint32_t samplerate,uint16_t *p_sidetone_FIR_coef)
{
    uint32_t sidetone_half_tap_num = 0;
    uint32_t coef_addr = 0;
    const uint16_t *sidetone_coefficient_table;
    bool is_96khz = false;

    switch (samplerate) {
        case 16000:
            sidetone_half_tap_num = sizeof(sidetone_coefficient_table_16k) / sizeof(uint16_t);
            sidetone_coefficient_table = sidetone_coefficient_table_16k;
            break;
        case 32000:
            sidetone_half_tap_num = sizeof(sidetone_coefficient_table_32k) / sizeof(uint16_t);
            sidetone_coefficient_table = sidetone_coefficient_table_32k;
            break;
        case 96000:
            is_96khz = true;
            sidetone_half_tap_num = sizeof(sidetone_coefficient_table_48k) / sizeof(uint16_t);
            sidetone_coefficient_table = sidetone_coefficient_table_48k;
            break;
        case 48000:
            sidetone_half_tap_num = sizeof(sidetone_coefficient_table_48k) / sizeof(uint16_t);
            p_sidetone_FIR_coef += 1; /*config tool read nvkey shift 2 byte*/
            sidetone_coefficient_table = p_sidetone_FIR_coef;
            //HAL_AUDIO_LOG_INFO("[M DEBUG][hal_sidetone_set_filter] sidetone_coefficient_table_48k %d",1,*sidetone_coefficient_table_48k);
            //HAL_AUDIO_LOG_INFO("[M DEBUG][hal_sidetone_set_filter] sidetone_coefficient_table            %d %d %d %x",4,sidetone_coefficient_table[0],sidetone_coefficient_table[1],sidetone_coefficient_table[2],p_sidetone_FIR_coef);
            break;
        default:
            return true;
            break;
    }
    //Set sidetone half tap number and is 96kHz
    AFE_SET_REG(AFE_SIDETONE_CON1, (is_96khz<<AFE_SIDETONE_CON1_SET_96K_POS)|(sidetone_half_tap_num<<AFE_SIDETONE_CON1_HALF_TAP_NUM_POS), AFE_SIDETONE_CON1_SET_96K_MASK|AFE_SIDETONE_CON1_HALF_TAP_NUM_MASK);

    //Write coefficient
    uint32_t ready_value = AFE_GET_REG(AFE_SIDETONE_CON0);
    for (coef_addr = 0; coef_addr < sidetone_half_tap_num; coef_addr++) {
        bool old_w_rdy = (ready_value >> AFE_SIDETONE_CON0_WRITE_READY_POS) & 0x1;
        bool new_w_rdy = 0;
        uint32_t try_cnt = 0;
        bool ch = 0; //0:L ch, 1:R ch
        //enable w/r, read ops, select LCH as STF input
        AFE_SET_REG (AFE_SIDETONE_CON0,
                     (1<<AFE_SIDETONE_CON0_READ_WRITE_ENABLE_POS)|(1<<AFE_SIDETONE_CON0_READ_WRITE_SEL_POS)|(ch<<AFE_SIDETONE_CON0_CHANNEL_SEL_POS)|(coef_addr<<AFE_SIDETONE_CON0_COEFFICIENT_NUMBER_POS)|(sidetone_coefficient_table[coef_addr]<<AFE_SIDETONE_CON0_COEFFICIENT_VALUE_POS),
                     AFE_SIDETONE_CON0_READ_WRITE_ENABLE_MASK|AFE_SIDETONE_CON0_READ_WRITE_SEL_MASK|AFE_SIDETONE_CON0_CHANNEL_SEL_MASK|AFE_SIDETONE_CON0_COEFFICIENT_NUMBER_MASK|AFE_SIDETONE_CON0_COEFFICIENT_VALUE_MASK);

        /* wait until flag write_ready changed*/
        for (try_cnt = 0; try_cnt < 10; try_cnt++) { /* max try 10 times [align phone]*/
            ready_value = AFE_GET_REG(AFE_SIDETONE_CON0);
            new_w_rdy = (ready_value >> AFE_SIDETONE_CON0_WRITE_READY_POS) & 0x1;
            if (new_w_rdy == old_w_rdy) {
                HAL_AUDIO_DELAY_US(3);
                if (try_cnt == 9) {
                    HAL_AUDIO_LOG_ERROR("DSP - Error sidetone time out %d:%X !", 2, coef_addr, sidetone_coefficient_table[coef_addr]);
                }
            } else { /* state flip: ok */
                break;
            }
        }
        AFE_SET_REG (AFE_SIDETONE_CON0, (0<<AFE_SIDETONE_CON0_READ_WRITE_SEL_POS), AFE_SIDETONE_CON0_READ_WRITE_SEL_MASK);
    }
    return false;
}

bool hal_sidetone_set_enable(bool enable)
{
    AFE_SET_REG(AFE_SIDETONE_CON1, enable<<AFE_SIDETONE_CON1_ENABLE_POS , AFE_SIDETONE_CON1_ENABLE_MASK);
    return false;
}

uint32_t hal_sidetone_convert_negative_gain_value(int32_t gain)
{
    int32_t negative_gain;
    if (gain>0) {
        negative_gain = (gain%600)
                          ? (gain%600) - 600
                          : 0 ;
    } else {
        negative_gain = gain;
    }
    return afe_calculate_digital_gain_index(negative_gain, AFE_SIDETONE_0DB_REGISTER_VALUE);
}

static void hal_sidetone_set_negative_gain(int32_t negative_gain_value)
{
    AFE_SET_REG (AFE_SIDETONE_GAIN, (uint16_t)negative_gain_value, 0xFFFF);
}

/*0dB:0 6dB:1  12dB:2  18dB:3  24dB:4*/
uint32_t hal_sidetone_convert_positive_gain_value(int32_t gain)
{
    uint32_t positve_gain, pos_gain;
    if (gain>0) {
        positve_gain = gain + 599; // carry
    } else {
        positve_gain = 0;
    }

    pos_gain =  positve_gain / 600;
    if (pos_gain > 4) {
        pos_gain = 4;
    }
    return pos_gain;
}

static void hal_sidetone_set_positive_gain(int32_t positve_gain_value)
{
    AFE_SET_REG (AFE_SIDETONE_GAIN, positve_gain_value<<16, 0x7<<16);
}

void hal_sidetone_set_gain_by_register_value(int32_t positve_gain_value, int32_t negative_gain_value)
{
    hal_sidetone_set_negative_gain(negative_gain_value);
    hal_sidetone_set_positive_gain(positve_gain_value); // 0dB:0 6dB:1  12dB:2  18dB:3  24dB:4
}

void hal_sidetone_set_volume(int32_t gain)
{
    //call afe_set_sidetone_gain & afe_set_sidetone_positve_gain to adjust sidetone vol.
    //sidetone gain = gain + positive gain
    hal_sidetone_set_negative_gain(hal_sidetone_convert_negative_gain_value(gain));
    hal_sidetone_set_positive_gain(hal_sidetone_convert_positive_gain_value(gain));
}

/*******************************************************************************************
*                                         VOW                                       *
********************************************************************************************/
void spm_mtcmos_vow_on(void)
{
    /*TINFO = " --- VOW power on by CM4 API "*/
    //*SPM_VOW_PWR_CON = 0x12; sleep
    *SPM_VOW_PWR_CON = 0x16; // [2]: pwr_on = 1
    hal_gpt_delay_us(1); // 1us

    *SPM_VOW_PWR_CON = 0x1E; // [3]: pwr_on_2nd = 1
    hal_gpt_delay_us(1); // 1us

    // --- VOW SRAM begin ------
    *SPM_VOW_SRAM_CONTROL_RET = 0x00000001; //[16]:RET2N [0]:RET1N
    *SPM_VOW_SRAM_CONTROL     = 0x00000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    // --- VOW SRAM end ------

    *SPM_VOW_PWR_CON = 0xE;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x1E;  // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x1C;  // [4]: clk_dis = 1, iso = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x1D;  // [0]: rstb = 1
    hal_gpt_delay_us(1); // 1us

    // --- VOW SRAM MEM_ISO_EN_B begin ------
    *SPM_VOW_SRAM_CONTROL = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    // --- VOW SRAM end ------
    *SPM_VOW_PWR_CON = 0xD;   // [4]: clk_dis = 0
    hal_gpt_delay_us(1); // 1us

    //turn VOW protect_en=0
    *INFRA_CFG_MTCMOS5 = 0x0;
    //wait VOW protect ready=0
    while (*INFRA_CFG_MTCMOS5 != 0x0);

}

void spm_mtcmos_vow_off(void)
{
    //turn VOW protect_en=1
    *INFRA_CFG_MTCMOS5 = 0x1;
    //wait VOW protect ready=1
    HAL_AUDIO_LOG_INFO("enter while\r\n",0);

    while (*INFRA_CFG_MTCMOS5 != 0x101);//workaround needs to discuss with SPM owner
    HAL_AUDIO_LOG_INFO("exit while\r\n",0);
    /*TINFO = " --- VOW power down by CM4 API "*/
    *SPM_VOW_PWR_CON = 0x1D; // [4]: clk_dis = 1
    hal_gpt_delay_us(1); // 1us


    *SPM_VOW_PWR_CON = 0x1F; // [1]: iso = 1
    hal_gpt_delay_us(1); // 1us

    //// --- VOW SRAM Power Down begin ------
    *SPM_VOW_SRAM_CONTROL_RET = 0x00000001; //[16]:RET2N [0]:RET1N
    *SPM_VOW_SRAM_CONTROL     = 0x00000101;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
    //// --- VOW SRAM end Power Down------

    *SPM_VOW_PWR_CON = 0x1E; // [0]: rstb = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x1A; // [2]: pwr_on = 0
    hal_gpt_delay_us(1); // 1us
    *SPM_VOW_PWR_CON = 0x12; // [3]: pwr_on_2nd = 0
    hal_gpt_delay_us(1); // 1us
}


bool hal_wow_power_enable(bool enable)
{
    if(enable){
        //VOW power
        //AFE_WRITE(0xA2110560, 0x01000100);//VOW_SRAM_CONTROL
        //AFE_WRITE(0xA2111004, 0xD);//VOW_PWR_CON
        //AFE_WRITE(0xA2200054, 0x0);//INFRA_CFG_MTCMOS5
        spm_mtcmos_vow_on();
    }else{
        spm_mtcmos_vow_off();
    }
    return false;
}
bool hal_wow_set_config(bool vow_with_hpf,uint32_t snr_threshold,uint8_t alpha_rise,uint32_t mic_selection)
{
    UNUSED(alpha_rise);
    bool vow_enable_mic_ch0 = true;
    bool vow_enable_mic_ch1 = true;
    vow_enable_mic_ch0 = (mic_selection&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L)||(mic_selection&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L);
    vow_enable_mic_ch1 = (mic_selection&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R)||(mic_selection&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R);
    if(vow_with_hpf){
        //AFE_WRITE(AFE_VOW_HPF_CFG0, 0x0051);
        //AFE_WRITE(AFE_VOW_HPF_CFG1, 0x0051);
        if(vow_enable_mic_ch0){
           AFE_WRITE(AFE_VOW_HPF_CFG0, 0x0051);
        }
        if(vow_enable_mic_ch1){
           AFE_WRITE(AFE_VOW_HPF_CFG1, 0x0051);
        }
    }else{
        AFE_WRITE(AFE_VOW_HPF_CFG0, 0x0006);
        AFE_WRITE(AFE_VOW_HPF_CFG1, 0x0006);
    }
    //set default parameters to sync MTK
    AFE_WRITE(AFE_VOW_VAD_CFG0,0x0);
    AFE_WRITE(AFE_VOW_VAD_CFG1,0x0);
    AFE_WRITE(AFE_VOW_VAD_CFG2,0x0);
    AFE_WRITE(AFE_VOW_VAD_CFG3,0x0);

    //set snr threshold, Least sensitive:0x7373, Middle sensitive:0x4343,Very sensitive:0x0303
    AFE_WRITE(AFE_VOW_VAD_CFG4,snr_threshold);
    AFE_WRITE(AFE_VOW_VAD_CFG5,snr_threshold);
#if 0
    //set k_alpha_rise for singnal frame size
    ANA_SET_REG(AFE_VOW_VAD_CFG6,alpha_rise<<AFE_VOW_VAD_CFG6_K_ALPHA_RISE_CH1_POS,AFE_VOW_VAD_CFG6_K_ALPHA_RISE_CH1_MASK);
    ANA_SET_REG(AFE_VOW_VAD_CFG7,alpha_rise<<AFE_VOW_VAD_CFG7_K_ALPHA_RISE_CH2_POS,AFE_VOW_VAD_CFG7_K_ALPHA_RISE_CH2_MASK);

    //set noise min
    AFE_WRITE(AFE_VOW_VAD_CFG8,0x0050);
    AFE_WRITE(AFE_VOW_VAD_CFG9,0x0050);

    if((vow_pre_ch0_noise_msb)||(vow_pre_ch1_noise_msb))
    {
        ANA_SET_REG(AFE_VOW_VAD_CFG10,1<<15,1<<15);//enable S,N initial
        ANA_SET_REG(AFE_VOW_VAD_CFG11,1<<15,1<<15);//enable S,N initial

        AFE_WRITE(AFE_VOW_VAD_CFG10,(vow_pre_ch0_noise_msb)& 0x7FFF);//MSB of the S & N initial value
        AFE_WRITE(AFE_VOW_VAD_CFG11,(vow_pre_ch1_noise_msb)& 0x7FFF);//MSB of the S & N initial value

        AFE_WRITE(AFE_VOW_VAD_CFG10,0x0);//Finish S, N initial setting of CH1
        AFE_WRITE(AFE_VOW_VAD_CFG11,0x0);//Finish S, N initial setting of CH2
        HAL_AUDIO_LOG_INFO("vow_pre_ch0_noise_msb:0x%x,vow_pre_ch1_noise_msb:0x%x",2,vow_pre_ch0_noise_msb,vow_pre_ch1_noise_msb);
    }
#else//set default parameters to sync MTK
    AFE_WRITE(AFE_VOW_VAD_CFG6,0xa768);
    AFE_WRITE(AFE_VOW_VAD_CFG7,0xa768);
    AFE_WRITE(AFE_VOW_VAD_CFG8,0x1);
    AFE_WRITE(AFE_VOW_VAD_CFG9,0x1);
    AFE_WRITE(AFE_VOW_VAD_CFG10,0x0);
    AFE_WRITE(AFE_VOW_VAD_CFG11,0x0);
    AFE_WRITE(AFE_VOW_VAD_CFG12,0xf0f);
#endif
    HAL_AUDIO_LOG_INFO("wow_set_config mic_selection:0x%x mic_ch0 0x%x mic_ch1 0x%x",3,mic_selection,vow_enable_mic_ch0,vow_enable_mic_ch1);
    return false;

}

bool hal_wow_set_dmic(hal_audio_vow_mode_t vow_mode,hal_audio_dmic_selection_t dmic_selection,uint32_t mic_selection,hal_audio_control_status_t control)
{
    bool vow_interrupt_mode_sel = 0;
    bool vow_enable_mic_ch0 = true;
    bool vow_enable_mic_ch1 = true;
    vow_enable_mic_ch0 = (mic_selection&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_L);
    vow_enable_mic_ch1 = (mic_selection&HAL_AUDIO_CONTROL_DEVICE_DIGITAL_MIC_R);
    if(control){

        AFE_WRITE(AFE_VOW_TOP_CON0, 0x0610|(vow_interrupt_mode_sel<<2)|vow_mode);//0x0600  for phase 1 only

        if(dmic_selection == HAL_AUDIO_DMIC_GPIO_DMIC0){
          AFE_WRITE(AFE_VOW_DMIC_SEL,   (0x6-dmic_selection)<<4|(0x6-dmic_selection)<<0);//DMIC output clock source
          AFE_WRITE(AFE_VOW_DMIC_CK_CON, 1<<(0x6-dmic_selection));//DMIC output clock gating

        }else if(dmic_selection == HAL_AUDIO_DMIC_GPIO_DMIC1){
          AFE_WRITE(AFE_VOW_DMIC_SEL,   (0x8-dmic_selection)<<4|(0x8-dmic_selection)<<0);//DMIC output clock source
          AFE_WRITE(AFE_VOW_DMIC_CK_CON, 1<<(0x8-dmic_selection));//DMIC output clock gating
        }else{
          AFE_WRITE(AFE_VOW_DMIC_SEL,   (dmic_selection-2)<<4|(dmic_selection-2)<<0);//DMIC output clock source
          AFE_WRITE(AFE_VOW_DMIC_CK_CON, 1<<(dmic_selection-2));//DMIC output clock gating
        }
        AFE_WRITE(AFE_VOW_TOP_CON1, 0x6008|vow_enable_mic_ch0);
        AFE_WRITE(AFE_VOW_TOP_CON2, 0x6088|vow_enable_mic_ch1);
        #if 0
        switch(dmic_selection){
            case HAL_AUDIO_DMIC_GPIO_DMIC0:
                ANA_SET_REG(DMIC_CLK_SEL_1,1<<DMIC_CLK_SEL_1_GPIO_DMIC0_POS,DMIC_CLK_SEL_1_GPIO_DMIC0_MASK);
                ANA_WRITE(DMIC_CLK_SEL_1,0x10000);
                HAL_AUDIO_LOG_INFO("HAL_AUDIO_DMIC_GPIO_DMIC0 DMIC_CLK_SEL_1 addr:0x%x val:0x%x",2,DMIC_CLK_SEL_1,ANA_GET_REG(DMIC_CLK_SEL_1));
            break;
            case HAL_AUDIO_DMIC_GPIO_DMIC1:
                ANA_SET_REG(DMIC_CLK_SEL_1,1<<DMIC_CLK_SEL_1_GPIO_DMIC1_POS,DMIC_CLK_SEL_1_GPIO_DMIC1_MASK);
                HAL_AUDIO_LOG_INFO("HAL_AUDIO_DMIC_GPIO_DMIC1",0);
            break;
            case HAL_AUDIO_DMIC_ANA_DMIC0:
                ANA_SET_REG(DMIC_CLK_SEL_0,1<<DMIC_CLK_SEL_0_DMIC0_POS,DMIC_CLK_SEL_0_DMIC0_MASK);
                HAL_AUDIO_LOG_INFO("HAL_AUDIO_DMIC_ANA_DMIC0",0);
            break;
            case HAL_AUDIO_DMIC_ANA_DMIC1:
                ANA_SET_REG(DMIC_CLK_SEL_0,1<<DMIC_CLK_SEL_0_DMIC1_POS,DMIC_CLK_SEL_0_DMIC1_MASK);
                HAL_AUDIO_LOG_INFO("HAL_AUDIO_DMIC_ANA_DMIC1",0);
            break;
            case HAL_AUDIO_DMIC_ANA_DMIC2:
                ANA_SET_REG(DMIC_CLK_SEL_0,1<<DMIC_CLK_SEL_0_DMIC2_POS,DMIC_CLK_SEL_0_DMIC2_MASK);
                HAL_AUDIO_LOG_INFO("HAL_AUDIO_DMIC_ANA_DMIC2",0);
            break;
            case HAL_AUDIO_DMIC_ANA_DMIC3:
                ANA_SET_REG(DMIC_CLK_SEL_0,1<<DMIC_CLK_SEL_0_DMIC3_POS,DMIC_CLK_SEL_0_DMIC3_MASK);
                HAL_AUDIO_LOG_INFO("HAL_AUDIO_DMIC_ANA_DMIC3",0);
            break;
            case HAL_AUDIO_DMIC_ANA_DMIC4:
                ANA_SET_REG(DMIC_CLK_SEL_1,1<<DMIC_CLK_SEL_1_DMIC4_POS,DMIC_CLK_SEL_1_DMIC4_MASK);
                HAL_AUDIO_LOG_INFO("HAL_AUDIO_DMIC_ANA_DMIC4",0);
            break;
            case HAL_AUDIO_DMIC_ANA_DMIC5:
                ANA_SET_REG(DMIC_CLK_SEL_1,1<<DMIC_CLK_SEL_1_DMIC5_POS,DMIC_CLK_SEL_1_DMIC5_MASK);
                HAL_AUDIO_LOG_INFO("HAL_AUDIO_DMIC_ANA_DMIC5",0);
            break;
            default:
                HAL_AUDIO_LOG_ERROR("DSP - Error VOW dmic_selection %d !", 1, dmic_selection);
                HAL_AUDIO_ASSERT(false);
            break;

        }
        #else
        AFE_WRITE(0xA2010700, 0x1010101);//*DMIC_CLK_SEL_0 for ASIC
        AFE_WRITE(0xA2010704, 0x1010101);//*DMIC_CLK_SEL_1 for ASIC
        HAL_AUDIO_LOG_INFO("DMIC_CLK_SEL_0:0x%x DMIC_CLK_SEL_1:0x%x",2,ANA_GET_REG(0xA2010700),ANA_GET_REG(0xA2010704));
        #endif
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio VOW DMIC vow_mode:%d, dmic_selection:%d, ch0:%d, ch1:%d", 4, vow_mode, dmic_selection,vow_enable_mic_ch0,vow_enable_mic_ch1);
    }else{
        AFE_WRITE(DMIC_CLK_SEL_0,0x0);//Chip TOP DMIC clock mux control for DMIC3/DMIC2/DMIC1/DMIC0
        AFE_WRITE(DMIC_CLK_SEL_1,0x0);//Chip TOP DMIC clock mux control for GPIO_DMIC1/GPIO_DMIC0/DMIC5/DMIC4
        ANA_SET_REG(AFE_VOW_TOP_CON0,1<<15,1<<15);//VOW clock off
        //reset vow
        AFE_WRITE(NFRA_CFG_PERI2,0x0);
        AFE_WRITE(NFRA_CFG_PERI2,0x1);

    }
    return false;
}

bool hal_wow_set_amic(hal_audio_vow_mode_t vow_mode,hal_audio_performance_mode_t performance,afe_analog_select_t analog_select,uint32_t mic_selection,hal_audio_control_status_t control)
{
    bool vow_enable_mic_ch0 = true;
    bool vow_enable_mic_ch1 = true;
    vow_enable_mic_ch0 = (mic_selection&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_L);
    vow_enable_mic_ch1 = (mic_selection&HAL_AUDIO_CONTROL_DEVICE_ANALOG_MIC_R);
    if(control){
        AFE_WRITE(AFE_VOW_TOP_CON0, ((performance>=AFE_PEROFRMANCE_LOW_POWER_MODE)<<10)|(0<<2)|vow_mode);//0x0600  for phase 1 only ,0x0630 for AMIC1.625M, 0x230 for AMIC 6.5M
        if(analog_select == AFE_ANALOG_ADC0){
            AFE_WRITE(AFE_VOW_TOP_CON4, ((performance>=AFE_PEROFRMANCE_LOW_POWER_MODE)<<8)|(0<<4)|(1<<0));
        }else if(analog_select == AFE_ANALOG_ADC1){
            AFE_WRITE(AFE_VOW_TOP_CON4, ((performance>=AFE_PEROFRMANCE_LOW_POWER_MODE)<<8)|(2<<4)|(3<<0));
        }else if(analog_select == AFE_ANALOG_ADC2){
            AFE_WRITE(AFE_VOW_TOP_CON4, ((performance>=AFE_PEROFRMANCE_LOW_POWER_MODE)<<8)|(4<<4)|(5<<0));
        }
        AFE_WRITE(AFE_VOW_TOP_CON1, 0x4008|vow_enable_mic_ch0);
        AFE_WRITE(AFE_VOW_TOP_CON2, 0x4008|vow_enable_mic_ch1);
        HAL_AUDIO_LOG_INFO("DSP - Hal Audio VOW AMIC vow_mode:%d, performance:%d, ch0:%d, ch1:%d", 4, vow_mode, performance,vow_enable_mic_ch0,vow_enable_mic_ch1);
    }else{
        ANA_SET_REG(AFE_VOW_TOP_CON0,1<<15,1<<15);//VOW clock off
        //reset vow
        AFE_WRITE(NFRA_CFG_PERI2,0x0);
        AFE_WRITE(NFRA_CFG_PERI2,0x1);
    }
    HAL_AUDIO_LOG_INFO("wow_set_amic mic_selection:0x%x,mic_ch0:0x%x,mic_ch1:0x%x",3,mic_selection,vow_enable_mic_ch0,vow_enable_mic_ch1);
    return false;
}

void hal_vow_reset(void){
    //reset vow
    AFE_WRITE(NFRA_CFG_PERI2,0x0);
    AFE_WRITE(NFRA_CFG_PERI2,0x1);
}

bool hal_wow_set_dma_irq_threshold(uint8_t dma_irq_threshold)
{
    bool vow_fifo_tdm = false;
    AFE_WRITE(AFE_VOW_TOP_CON5, (vow_fifo_tdm<<12)|(dma_irq_threshold-1));
    return false;
}
bool hal_wow_get_signal_noise_status(hal_audio_vow_control_t* vow_control)
{
    uint32_t ch0_signal, ch0_noise = 0xFFFFF, ch1_signal, ch1_noise= 0xFFFFF;

    ch0_signal = (AFE_READ(AFE_VOW_VAD_MON4)&0xFFFF) | (AFE_READ(AFE_VOW_VAD_MON6)<<16);
    ch1_signal = (AFE_READ(AFE_VOW_VAD_MON5)&0xFFFF) | (AFE_READ(AFE_VOW_VAD_MON7)<<16);
    ch1_noise =  (AFE_READ(AFE_VOW_VAD_MON9)&0xFFFF) | (AFE_READ(AFE_VOW_VAD_MON11)<<16);
    ch0_noise =  (vow_control->vow_mode != 3) ? (AFE_READ(AFE_VOW_VAD_MON8)&0xFFFF) | (AFE_READ(AFE_VOW_VAD_MON10)<<16) : (ch1_noise);
    HAL_AUDIO_LOG_INFO("ch0_signal:0x%x, ch0_noise:0x%x , ch1_signal:0x%x, ch1_noise:0x%x, Observed_noise:0x%x \n",5, ch0_signal, ch0_noise, ch1_signal, ch1_noise, vow_control->stable_noise);

    //0xFFFFF000
    if(((vow_control->stable_noise)&(vow_control->noise_ignore_bit))==(ch0_noise&(vow_control->noise_ignore_bit))){
        vow_control->stable_noise = ch0_noise;
        return true;
    }else{
        vow_control->stable_noise = ch0_noise;
        return false;
    }
}
bool hal_wow_clear_snr_irq_status(hal_audio_vow_control_t vow_control)
{
     //Clear SNR
    if (vow_control.vow_mode == AFE_VOW_PHASE0) {
        AFE_SET_REG(AFE_VOW_TOP_CON6, 0<<0, 1<<0);
    }
    AFE_WRITE(AFE_VOW_INTR_CLR, 1<<0|1<<1);//AFE_SET_REG(AFE_VOW_INTR_CLR, 1<<0|1<<1, 1<<0|1<<1);
    if (vow_control.vow_mode == AFE_VOW_PHASE0) {
        AFE_SET_REG(AFE_VOW_TOP_CON6, 1<<0, 1<<0);
    }
    return false;
}

bool hal_wow_clear_fifo_irq_status(hal_audio_vow_control_t vow_control){
    UNUSED(vow_control);
    AFE_WRITE(AFE_VOW_INTR_CLR, 1<<8);//AFE_SET_REG(AFE_VOW_INTR_CLR, 1<<8, 1<<8);
    return false;
}


/*******************************************************************************************
*                                           ANA                                            *
********************************************************************************************/
bool hal_audio_ana_set_global_bias (bool enable)
{
    ANA_SET_REG(AUDDEC_ANA_CON10, ((enable)<<AUDDEC_ANA_CON10_PWR_VA33_POS), AUDDEC_ANA_CON10_PWR_VA33_MASK);
    return false;
}
void hal_audio_ana_micbias_LDO0_set_enable (hal_audio_bias_selection_t bias_select,hal_audio_bias_voltage_t bias_voltage, bool is_low_power, bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        ana_enable_micbias_LDO0_counter++;
        if (ana_enable_micbias_LDO0_counter == 1) {
            ANA_SET_REG(AUDENC_ANA_CON34,
                (enable<<AUDENC_ANA_CON34_BIAS0_DRIVER_EN_POS)|(0<<AUDENC_ANA_CON34_BIAS0_BYPASS_POS)|(is_low_power<<AUDENC_ANA_CON34_BIAS0_LOWPOWER_EN_POS)|(bias_voltage << AUDENC_ANA_CON34_BIAS0_VOLTAGE_SEL_POS),
                AUDENC_ANA_CON34_BIAS0_DRIVER_EN_MASK|AUDENC_ANA_CON34_BIAS0_BYPASS_MASK|AUDENC_ANA_CON34_BIAS0_LOWPOWER_EN_MASK|AUDENC_ANA_CON34_BIAS0_VOLTAGE_SEL_MASK);
            hal_audio_ana_set_bias_low_power(bias_select, is_low_power);
		}
    } else {
        ana_enable_micbias_LDO0_counter--;
        if (ana_enable_micbias_LDO0_counter == 0) {
            ANA_SET_REG(AUDENC_ANA_CON34,
                (enable<<AUDENC_ANA_CON34_BIAS0_DRIVER_EN_POS)|(0<<AUDENC_ANA_CON34_BIAS0_BYPASS_POS)|(0<<AUDENC_ANA_CON34_BIAS0_LOWPOWER_EN_POS)|(bias_voltage << AUDENC_ANA_CON34_BIAS0_VOLTAGE_SEL_POS),
                AUDENC_ANA_CON34_BIAS0_DRIVER_EN_MASK|AUDENC_ANA_CON34_BIAS0_BYPASS_MASK|AUDENC_ANA_CON34_BIAS0_LOWPOWER_EN_MASK|AUDENC_ANA_CON34_BIAS0_VOLTAGE_SEL_MASK);
        } else if (ana_enable_micbias_LDO0_counter < 0) {
            ana_enable_micbias_LDO0_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio AFE micbias LDO control:%d, cnt:%d", 2, enable, ana_enable_micbias_LDO0_counter);
}
bool hal_audio_ana_set_bias_configuration (hal_audio_bias_selection_t bias_select, hal_audio_bias_voltage_t bias_voltage, bool is_low_power,bool bias1_2_with_LDO0, bool enable)
{
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS0) {
        //Set MICBIAS Pull Low when Disable
        ANA_SET_REG(AUDENC_ANA_CON34,enable<<AUDENC_ANA_CON34_BIAS0_PULLLOW_POS,AUDENC_ANA_CON34_BIAS0_PULLLOW_MASK);
        //ANA_SET_REG(AUDENC_ANA_CON34,
        //    (enable<<AUDENC_ANA_CON34_BIAS0_DRIVER_EN_POS)|(0<<AUDENC_ANA_CON34_BIAS0_BYPASS_POS)|(0<<AUDENC_ANA_CON34_BIAS0_LOWPOWER_EN_POS)|(bias_voltage << AUDENC_ANA_CON34_BIAS0_VOLTAGE_SEL_POS),
        //    AUDENC_ANA_CON34_BIAS0_DRIVER_EN_MASK|AUDENC_ANA_CON34_BIAS0_BYPASS_MASK|AUDENC_ANA_CON34_BIAS0_LOWPOWER_EN_MASK|AUDENC_ANA_CON34_BIAS0_VOLTAGE_SEL_MASK);
        hal_audio_ana_micbias_LDO0_set_enable (bias_select,bias_voltage,is_low_power,enable);
        ANA_SET_REG(AUDENC_ANA_CON34,enable<<AUDENC_ANA_CON34_BIAS0_EN_POS,AUDENC_ANA_CON34_BIAS0_EN_MASK);
        ANA_SET_REG(AUDENC_ANA_CON40, enable<<AUDENC_ANA_CON40_BIAS0_PARALLEL_POS, AUDENC_ANA_CON40_BIAS0_PARALLEL_MASK);
        //LODO0 to MICBIAS1 disable
        ANA_SET_REG(AUDENC_ANA_CON40, 0<<AUDENC_ANA_CON40_BIAS1_EN_POS, AUDENC_ANA_CON40_BIAS1_EN_MASK);
        ANA_SET_REG(AUDENC_ANA_CON40, 0<<AUDENC_ANA_CON40_BIAS1_PARALLEL_POS, AUDENC_ANA_CON40_BIAS1_PARALLEL_MASK);
        //LODO0 to MICBIAS2 disable
        ANA_SET_REG(AUDENC_ANA_CON40, 0<<AUDENC_ANA_CON40_BIAS2_EN_POS, AUDENC_ANA_CON40_BIAS2_EN_MASK);
        ANA_SET_REG(AUDENC_ANA_CON40, 0<<AUDENC_ANA_CON40_BIAS2_PARALLEL_POS, AUDENC_ANA_CON40_BIAS2_PARALLEL_MASK);
        //printf("BIAS0 AUDENC_ANA_CON34 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON34,ANA_GET_REG(AUDENC_ANA_CON34));
        //printf("BIAS0 AUDENC_ANA_CON40 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON40,ANA_GET_REG(AUDENC_ANA_CON40));
    }

    //Bias1 modified
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS1) {
        if(bias1_2_with_LDO0){
            //Set MICBIAS Pull Low when Disable
            ANA_SET_REG(AUDENC_ANA_CON35,enable<<AUDENC_ANA_CON35_BIAS1_PULLLOW_POS,AUDENC_ANA_CON35_BIAS1_PULLLOW_MASK);
            hal_audio_ana_micbias_LDO0_set_enable (bias_select,bias_voltage,is_low_power,enable);
            //LDO0 to MICBIAS1 enable
            ANA_SET_REG(AUDENC_ANA_CON40,enable<<AUDENC_ANA_CON40_BIAS1_EN_POS,AUDENC_ANA_CON40_BIAS1_EN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON40,enable<<AUDENC_ANA_CON40_BIAS_REV_SW0_1_POS,AUDENC_ANA_CON40_BIAS_REV_SW0_1_MASK);
        }else{
            //Set MICBIAS Pull Low when Disable
            ANA_SET_REG(AUDENC_ANA_CON35,enable<<AUDENC_ANA_CON35_BIAS1_PULLLOW_POS,AUDENC_ANA_CON35_BIAS1_PULLLOW_MASK);
            ANA_SET_REG(AUDENC_ANA_CON35,enable<<AUDENC_ANA_CON35_RG_AUDPWDBMICBIAS1_3VEN_POS,AUDENC_ANA_CON35_RG_AUDPWDBMICBIAS1_3VEN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON35, 0<<AUDENC_ANA_CON35_RG_AUDMICBIAS1_3VBYPASSEN_POS, AUDENC_ANA_CON35_RG_AUDMICBIAS1_3VBYPASSEN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON35, is_low_power<<AUDENC_ANA_CON35_RG_AUDMICBIAS1_3VLOWPEN_POS, AUDENC_ANA_CON35_RG_AUDMICBIAS1_3VLOWPEN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON35, bias_voltage<<AUDENC_ANA_CON35_RG_AUDMICBIAS1_3VVREF_POS, AUDENC_ANA_CON35_RG_AUDMICBIAS1_3VVREF_MASK);
            ANA_SET_REG(AUDENC_ANA_CON35, enable<<AUDENC_ANA_CON35_BIAS1_EN_POS, AUDENC_ANA_CON35_BIAS1_EN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON42, enable<<AUDENC_ANA_CON42_RG_AUDUL_MICBIAS_REV0_SW1_POS, AUDENC_ANA_CON42_RG_AUDUL_MICBIAS_REV0_SW1_MASK);
            //printf("BIAS0 AUDENC_ANA_CON35 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON35,ANA_GET_REG(AUDENC_ANA_CON35));
            //printf("BIAS0 AUDENC_ANA_CON42 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON42,ANA_GET_REG(AUDENC_ANA_CON42));

        }
    }
    //Bias2 modified
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS2) {
        if(bias1_2_with_LDO0){
            //Set MICBIAS Pull Low when Disable
            ANA_SET_REG(AUDENC_ANA_CON36,enable<<AUDENC_ANA_CON36_BIAS2_PULLLOW_POS,AUDENC_ANA_CON36_BIAS2_PULLLOW_MASK);
            hal_audio_ana_micbias_LDO0_set_enable (bias_select,bias_voltage,is_low_power,enable);
            //LDO0 to MICBIAS1 enable
            ANA_SET_REG(AUDENC_ANA_CON40,enable<<AUDENC_ANA_CON40_BIAS2_EN_POS,AUDENC_ANA_CON40_BIAS2_EN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON40,enable<<AUDENC_ANA_CON40_BIAS_REV_SW0_2_POS,AUDENC_ANA_CON40_BIAS_REV_SW0_2_MASK);
        }else{
            //Set MICBIAS Pull Low when Disable
            ANA_SET_REG(AUDENC_ANA_CON36,enable<<AUDENC_ANA_CON36_BIAS2_PULLLOW_POS,AUDENC_ANA_CON36_BIAS2_PULLLOW_MASK);
            //LDO2 on
            ANA_SET_REG(AUDENC_ANA_CON36,enable<<AUDENC_ANA_CON36_RG_AUDPWDBMICBIAS2_3VEN_POS,AUDENC_ANA_CON36_RG_AUDPWDBMICBIAS2_3VEN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON36,0<<AUDENC_ANA_CON36_RG_AUDMICBIAS2_3VBYPASSEN_POS,AUDENC_ANA_CON36_RG_AUDMICBIAS2_3VBYPASSEN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON36,is_low_power<<AUDENC_ANA_CON36_RG_AUDMICBIAS2_3VLOWPEN_POS,AUDENC_ANA_CON36_RG_AUDMICBIAS2_3VLOWPEN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON36,bias_voltage<<AUDENC_ANA_CON36_RG_AUDMICBIAS2_3VVREF_POS,AUDENC_ANA_CON36_RG_AUDMICBIAS2_3VVREF_MASK);
            //LDO2 to MICBIAS2 enable
            ANA_SET_REG(AUDENC_ANA_CON36,enable<<AUDENC_ANA_CON36_BIAS2_EN_POS,AUDENC_ANA_CON36_BIAS2_EN_MASK);
            //ANA_SET_REG(AUDENC_ANA_CON40, enable<<AUDENC_ANA_CON40_BIAS2_EN_POS, AUDENC_ANA_CON40_BIAS2_EN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON42, enable<<AUDENC_ANA_CON42_RG_AUDUL_MICBIAS_REV0_SW2_POS, AUDENC_ANA_CON42_RG_AUDUL_MICBIAS_REV0_SW2_MASK);
            //printf("BIAS2 AUDENC_ANA_CON36 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON36,ANA_GET_REG(AUDENC_ANA_CON36));
            //printf("BIAS2 AUDENC_ANA_CON42 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON42,ANA_GET_REG(AUDENC_ANA_CON42));
        }

    }

    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS3) {
        //Set MICBIAS Pull Low when Disable
        //printf("BIAS0 AUDENC_ANA_CON37 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON37,ANA_GET_REG(AUDENC_ANA_CON37));
        ANA_SET_REG(AUDENC_ANA_CON37,enable<<AUDENC_ANA_CON37_RG_AUDACCDETMICBIAS3_PULLLOW_POS,AUDENC_ANA_CON37_RG_AUDACCDETMICBIAS3_PULLLOW_MASK);
        //LDO3 on
        //printf("BIAS0 AUDENC_ANA_CON37 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON37,ANA_GET_REG(AUDENC_ANA_CON37));
        ANA_SET_REG(AUDENC_ANA_CON37,
                    (enable<<AUDENC_ANA_CON37_RG_AUDPWDBMICBIAS3_3VEN_POS)|(0<<AUDENC_ANA_CON37_RG_AUDMICBIAS3_3VBYPASSEN_POS)|(is_low_power<<AUDENC_ANA_CON37_RG_AUDMICBIAS3_3VLOWPEN_POS)|(bias_voltage<<AUDENC_ANA_CON37_RG_AUDMICBIAS3_3VVREF_POS),
                    (AUDENC_ANA_CON37_RG_AUDPWDBMICBIAS3_3VEN_MASK|AUDENC_ANA_CON37_RG_AUDMICBIAS3_3VBYPASSEN_MASK|AUDENC_ANA_CON37_RG_AUDMICBIAS3_3VLOWPEN_MASK|AUDENC_ANA_CON37_RG_AUDMICBIAS3_3VVREF_MASK));
        //printf("BIAS0 AUDENC_ANA_CON37 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON37,ANA_GET_REG(AUDENC_ANA_CON37));
        //LDO3 to MICBIAS3 enable
        //printf("BIAS0 AUDENC_ANA_CON37 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON37,ANA_GET_REG(AUDENC_ANA_CON37));
        ANA_SET_REG(AUDENC_ANA_CON37, enable<<AUDENC_ANA_CON37_RG_AUDPWDBMICBIAS3_POS, AUDENC_ANA_CON37_RG_AUDPWDBMICBIAS3_MASK);
        ANA_SET_REG(AUDENC_ANA_CON42, enable<<AUDENC_ANA_CON42_RG_AUDUL_MICBIAS_REV0_SW3_POS, AUDENC_ANA_CON42_RG_AUDUL_MICBIAS_REV0_SW3_MASK);
        //printf("BIAS3 AUDENC_ANA_CON37 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON37,ANA_GET_REG(AUDENC_ANA_CON37));
        //printf("BIAS3 AUDENC_ANA_CON42 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON42,ANA_GET_REG(AUDENC_ANA_CON42));
    }

    //Bias4 modified
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS4) {
        //Set MICBIAS Pull Low when Disable
        //printf("BIAS0 AUDENC_ANA_CON38 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON38,ANA_GET_REG(AUDENC_ANA_CON38));
        ANA_SET_REG(AUDENC_ANA_CON38,enable<<AUDENC_ANA_CON38_RG_AUDACCDETMICBIAS4_PULLLOW_POS,AUDENC_ANA_CON38_RG_AUDACCDETMICBIAS4_PULLLOW_MASK);
        //LDO4 on
        //printf("BIAS0 AUDENC_ANA_CON38 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON38,ANA_GET_REG(AUDENC_ANA_CON38));
        ANA_SET_REG(AUDENC_ANA_CON38,
            (enable<<AUDENC_ANA_CON38_RG_AUDPWDBMICBIAS4_3VEN_POS)|(0<<AUDENC_ANA_CON38_RG_AUDMICBIAS4_3VBYPASSEN_POS)|(is_low_power<<AUDENC_ANA_CON38_RG_AUDMICBIAS4_3VLOWPEN_POS)|(bias_voltage<<AUDENC_ANA_CON38_RG_AUDMICBIAS4_3VVREF_POS),
            (AUDENC_ANA_CON38_RG_AUDPWDBMICBIAS4_3VEN_MASK|AUDENC_ANA_CON38_RG_AUDMICBIAS4_3VBYPASSEN_MASK|AUDENC_ANA_CON38_RG_AUDMICBIAS4_3VLOWPEN_MASK|AUDENC_ANA_CON38_RG_AUDMICBIAS4_3VVREF_MASK));
        //printf("BIAS0 AUDENC_ANA_CON38 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON38,ANA_GET_REG(AUDENC_ANA_CON38));
        //LDO4 to MICBIAS4 enable
        ANA_SET_REG(AUDENC_ANA_CON38, enable<<AUDENC_ANA_CON38_RG_AUDPWDBMICBIAS4_POS, AUDENC_ANA_CON38_RG_AUDPWDBMICBIAS4_MASK);
        ANA_SET_REG(AUDENC_ANA_CON42, enable<<AUDENC_ANA_CON42_RG_AUDUL_MICBIAS_REV0_SW4_POS, AUDENC_ANA_CON42_RG_AUDUL_MICBIAS_REV0_SW4_MASK);
        //printf("BIAS0 AUDENC_ANA_CON38 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON38,ANA_GET_REG(AUDENC_ANA_CON38));
        //printf("BIAS0 AUDENC_ANA_CON42 addr 0x%x val 0x%x\r\n",AUDENC_ANA_CON42,ANA_GET_REG(AUDENC_ANA_CON42));
    }
    return false;
}

bool hal_audio_ana_set_bias_low_power (hal_audio_bias_selection_t bias_select, bool is_low_power)
{
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS0){
        ANA_SET_REG(AUDENC_ANA_CON34, (is_low_power<<AUDENC_ANA_CON34_BIAS0_LOWPOWER_EN_POS), AUDENC_ANA_CON34_BIAS0_LOWPOWER_EN_MASK);
    }
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS1){
        ANA_SET_REG(AUDENC_ANA_CON35, (is_low_power<<AUDENC_ANA_CON35_RG_AUDMICBIAS1_3VLOWPEN_POS), AUDENC_ANA_CON35_RG_AUDMICBIAS1_3VLOWPEN_MASK);
    }
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS2){
        ANA_SET_REG(AUDENC_ANA_CON36, (is_low_power<<AUDENC_ANA_CON36_RG_AUDMICBIAS2_3VLOWPEN_POS), AUDENC_ANA_CON36_RG_AUDMICBIAS2_3VLOWPEN_MASK);
    }
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS3){
        ANA_SET_REG(AUDENC_ANA_CON37, (is_low_power<<AUDENC_ANA_CON37_RG_AUDMICBIAS3_3VLOWPEN_POS), AUDENC_ANA_CON37_RG_AUDMICBIAS3_3VLOWPEN_MASK);
    }
    if (bias_select&HAL_AUDIO_BIAS_SELECT_BIAS3){
        ANA_SET_REG(AUDENC_ANA_CON38, (is_low_power<<AUDENC_ANA_CON38_RG_AUDMICBIAS4_3VLOWPEN_POS), AUDENC_ANA_CON38_RG_AUDMICBIAS4_3VLOWPEN_MASK);
    }

    HAL_AUDIO_LOG_INFO("DSP - hal audio ana set bias :%d, lowpower :%d", 2, bias_select, is_low_power);

    return false;
}

bool hal_audio_ana_set_dmic_enable (hal_audio_dmic_selection_t dmic_select, bool enable)
{
    uint32_t dmic_register = 0, register_value = 0;
    switch (dmic_select) {
        case HAL_AUDIO_DMIC_GPIO_DMIC0:

            break;
        case HAL_AUDIO_DMIC_GPIO_DMIC1:

            break;
        case HAL_AUDIO_DMIC_ANA_DMIC0:
            dmic_register = AUDENC_ANA_CON28;
            register_value = (enable<<AUDENC_ANA_CON28_DMIC0_EN_POS) |
                             (enable<<AUDENC_ANA_CON28_DMIC0_DATA_EN_POS) |
                             (0<<AUDENC_ANA_CON28_CLOCK_SOURCE_POS) |
                             (1<<AUDENC_ANA_CON28_POS_DUTY_POS) |
                             (1<<AUDENC_ANA_CON28_NEG_DUTY_POS);
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC1:
            dmic_register = AUDENC_ANA_CON29;
            register_value = (enable<<AUDENC_ANA_CON29_DMIC1_EN_POS) |
                             (enable<<AUDENC_ANA_CON29_DMIC1_DATA_EN_POS) |
                             (0<<AUDENC_ANA_CON29_CLOCK_SOURCE_POS) |
                             (1<<AUDENC_ANA_CON29_POS_DUTY_POS) |
                             (1<<AUDENC_ANA_CON29_NEG_DUTY_POS);
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC2:
            dmic_register = AUDENC_ANA_CON30;
            register_value = (enable<<AUDENC_ANA_CON30_RG_AUDDIGMIC2EN_POS) |
                             (enable<<AUDENC_ANA_CON30_RG_AUDDIGMIC2DATA_EN_POS) |
                             (0<<AUDENC_ANA_CON30_RG_DMIC2HPCLKEN_POS) |
                             (1<<AUDENC_ANA_CON30_RG_AUDDIGMIC2PDUTY_POS) |
                             (1<<AUDENC_ANA_CON30_RG_AUDDIGMIC2NDUTY_POS);
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC3:
            dmic_register = AUDENC_ANA_CON31;
            register_value = (enable<<AUDENC_ANA_CON31_RG_AUDDIGMIC3EN_POS) |
                             (enable<<AUDENC_ANA_CON31_RG_AUDDIGMIC3DATA_EN_POS) |
                             (0<<AUDENC_ANA_CON31_RG_DMIC3HPCLKEN_POS) |
                             (1<<AUDENC_ANA_CON31_RG_AUDDIGMIC3PDUTY_POS) |
                             (1<<AUDENC_ANA_CON31_RG_AUDDIGMIC3NDUTY_POS);
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC4:
            dmic_register = AUDENC_ANA_CON32;
            register_value = (enable<<AUDENC_ANA_CON32_RG_AUDDIGMIC4EN_POS) |
                             (enable<<AUDENC_ANA_CON32_RG_AUDDIGMIC4DATA_EN_POS) |
                             (0<<AUDENC_ANA_CON32_RG_DMIC4HPCLKEN_POS) |
                             (1<<AUDENC_ANA_CON32_RG_AUDDIGMIC4PDUTY_POS) |
                             (1<<AUDENC_ANA_CON32_RG_AUDDIGMIC4NDUTY_POS);
            break;
        case HAL_AUDIO_DMIC_ANA_DMIC5:
            dmic_register = AUDENC_ANA_CON33;
            register_value = (enable<<AUDENC_ANA_CON33_RG_AUDDIGMIC5EN_POS) |
                             (enable<<AUDENC_ANA_CON33_RG_AUDDIGMIC5DATA_EN_POS) |
                             (0<<AUDENC_ANA_CON33_RG_DMIC5HPCLKEN_POS) |
                             (1<<AUDENC_ANA_CON33_RG_AUDDIGMIC5PDUTY_POS) |
                             (1<<AUDENC_ANA_CON33_RG_AUDDIGMIC5NDUTY_POS);
            break;
        default:
            return true;
            break;
    }
    if (dmic_register) {
        ANA_WRITE(dmic_register, register_value);
    } else {
        HAL_AUDIO_LOG_WARNING("DSP - Warning Hal Audio Dmic configure wrong %d @@", 1, dmic_select);
    }
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio Dmic configure select %d,RG:0x%x,val:0x%x,enable %d", 4, dmic_select,dmic_register,register_value,enable);
    return false;
}

void hal_audio_ana_enable_capless_LDO(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        ana_enable_capless_LDO_counter++;
        if (ana_enable_capless_LDO_counter == 1) {
            ANA_SET_REG((AUDENC_ANA_CON27),
            ((1<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_DISCHARGE_PDN_POS)|(1<<AUDENC_ANA_CON27_TBST_EN_POS)|(1<<AUDENC_ANA_CON27_REMOTE_SENSE_EN_POS)|(0<<AUDENC_ANA_CON27_PDD_TEST_POS)),
            (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_DISCHARGE_PDN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK|AUDENC_ANA_CON27_REMOTE_SENSE_EN_MASK|AUDENC_ANA_CON27_PDD_TEST_MASK));
        }
    } else {
        ana_enable_capless_LDO_counter--;
        if (ana_enable_capless_LDO_counter == 0) {
            ANA_SET_REG((AUDENC_ANA_CON27),
            ((0<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_DISCHARGE_PDN_POS)|(0<<AUDENC_ANA_CON27_TBST_EN_POS)|(0<<AUDENC_ANA_CON27_REMOTE_SENSE_EN_POS)|(0<<AUDENC_ANA_CON27_PDD_TEST_POS)),
            (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_DISCHARGE_PDN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK|AUDENC_ANA_CON27_REMOTE_SENSE_EN_MASK|AUDENC_ANA_CON27_PDD_TEST_MASK));
        } else if (ana_enable_capless_LDO_counter < 0) {
            ana_enable_capless_LDO_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio AFE capless LDO control:%d, cnt:%d", 2, enable, ana_enable_capless_LDO_counter);
}

void hal_audio_ana_enable_ADC_13MCK(afe_analog_select_t analog_select,bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        if(analog_select == AFE_ANALOG_ADC0){
            ana_enable_CLK_TO_ADC01_counter++;
        }
        if(analog_select == AFE_ANALOG_ADC1){
            ana_enable_CLK_TO_ADC01_counter++;
            ana_enable_CLK_TO_ADC23_counter++;
        }
        if(analog_select == AFE_ANALOG_ADC2){
            ana_enable_CLK_TO_ADC01_counter++;
            ana_enable_CLK_TO_ADC23_counter++;
            ana_enable_CLK_TO_ADC45_counter++;
        }
        if (ana_enable_CLK_TO_ADC01_counter == 1) {
            ANA_SET_REG(AUDENC_ANA_CON3, enable<<AUDENC_ANA_CON3_UL_CLK_FROM_CLKSQ_POS, AUDENC_ANA_CON3_UL_CLK_FROM_CLKSQ_MASK); //Enable CLK from DL to UL
            ANA_SET_REG(AUDENC_ANA_CON3, enable<<AUDENC_ANA_CON3_RG_AUD01PREAMP_LOWPEN_ORIGIN_POS, AUDENC_ANA_CON3_RG_AUD01PREAMP_LOWPEN_ORIGIN_MASK);
        }
        if (ana_enable_CLK_TO_ADC23_counter == 1) {
            ANA_SET_REG(AUDENC_ANA_CON12, enable<<AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_POS, AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_MASK); //Enable CLK from DL to UL
            ANA_SET_REG(AUDENC_ANA_CON12, enable<<AUDENC_ANA_CON12_RG_AUD23PREAMP_LOWPEN_ORIGIN_POS, AUDENC_ANA_CON12_RG_AUD23PREAMP_LOWPEN_ORIGIN_MASK);
        }
        if (ana_enable_CLK_TO_ADC45_counter == 1) {
            ANA_SET_REG(AUDENC_ANA_CON21, enable<<AUDENC_ANA_CON21_RG_AUD45ADC_13MCK_EN_POS, AUDENC_ANA_CON21_RG_AUD45ADC_13MCK_EN_MASK); //Enable CLK from DL to UL
            ANA_SET_REG(AUDENC_ANA_CON21, enable<<AUDENC_ANA_CON21_RG_AUD45PREAMP_LOWPEN_ORIGIN_POS, AUDENC_ANA_CON21_RG_AUD45PREAMP_LOWPEN_ORIGIN_MASK);
        }
    } else {
        if(analog_select == AFE_ANALOG_ADC0){
            ana_enable_CLK_TO_ADC01_counter--;
        }
        if(analog_select == AFE_ANALOG_ADC1){
            ana_enable_CLK_TO_ADC01_counter--;
            ana_enable_CLK_TO_ADC23_counter--;
        }
        if(analog_select == AFE_ANALOG_ADC2){
            ana_enable_CLK_TO_ADC01_counter--;
            ana_enable_CLK_TO_ADC23_counter--;
            ana_enable_CLK_TO_ADC45_counter--;
        }

        if (ana_enable_CLK_TO_ADC01_counter == 0) {
            ANA_SET_REG(AUDENC_ANA_CON3, enable<<AUDENC_ANA_CON3_UL_CLK_FROM_CLKSQ_POS, AUDENC_ANA_CON3_UL_CLK_FROM_CLKSQ_MASK); //Enable CLK from DL to UL
            ANA_SET_REG(AUDENC_ANA_CON3, enable<<AUDENC_ANA_CON3_RG_AUD01PREAMP_LOWPEN_ORIGIN_POS, AUDENC_ANA_CON3_RG_AUD01PREAMP_LOWPEN_ORIGIN_MASK);
        } else if (ana_enable_CLK_TO_ADC01_counter < 0) {
            ana_enable_CLK_TO_ADC01_counter = 0;
        }

        if (ana_enable_CLK_TO_ADC23_counter == 0) {
            ANA_SET_REG(AUDENC_ANA_CON12, enable<<AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_POS, AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_MASK); //Enable CLK from DL to UL
            ANA_SET_REG(AUDENC_ANA_CON12, enable<<AUDENC_ANA_CON12_RG_AUD23PREAMP_LOWPEN_ORIGIN_POS, AUDENC_ANA_CON12_RG_AUD23PREAMP_LOWPEN_ORIGIN_MASK); //Enable CLK from DL to UL
        } else if (ana_enable_CLK_TO_ADC23_counter < 0) {
            ana_enable_CLK_TO_ADC23_counter = 0;
        }

        if (ana_enable_CLK_TO_ADC45_counter == 0) {
            ANA_SET_REG(AUDENC_ANA_CON21, enable<<AUDENC_ANA_CON21_RG_AUD45ADC_13MCK_EN_POS, AUDENC_ANA_CON21_RG_AUD45ADC_13MCK_EN_MASK); //Enable CLK from DL to UL
            ANA_SET_REG(AUDENC_ANA_CON21, enable<<AUDENC_ANA_CON21_RG_AUD45PREAMP_LOWPEN_ORIGIN_POS, AUDENC_ANA_CON21_RG_AUD45PREAMP_LOWPEN_ORIGIN_MASK); //Enable CLK from DL to UL
        } else if (ana_enable_CLK_TO_ADC45_counter < 0) {
            ana_enable_CLK_TO_ADC45_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio AFE CLKSQ control:%d, adc0 cnt:%d adc1 cnt:%d adc2 cnt:%d", 2, enable, ana_enable_CLK_TO_ADC01_counter,ana_enable_CLK_TO_ADC23_counter,ana_enable_CLK_TO_ADC45_counter);
}
extern void hal_save_adc_performance_mode(afe_analog_select_t analog_select,uint8_t adc_mode);
bool hal_audio_ana_set_adc45_enable (hal_audio_device_parameter_adc_t *adc_parameter, afe_analog_control_t analog_control, bool enable)
{
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio ANA ADC45:0x%x, Mode:%d, Performance:%d, with_au_in_swap:%d enable:%d", 5, analog_control, adc_parameter->adc_mode, adc_parameter->performance,adc_parameter->with_au_in_swap, enable);
    if (enable) {
        if (analog_control&AFE_ANALOG_COMMON) {
            hal_save_adc_performance_mode(AFE_ANALOG_ADC2,adc_parameter->performance);
            hal_volume_set_analog_mode(AFE_HW_ANALOG_GAIN_INPUT3, adc_parameter->adc_mode);

            //Top
            hal_audio_afe_enable_clksq(true);       //Enable CLKSQ 26MHz
            //ANA_SET_REG(AUDENC_ANA_CON21, 1<<AUDENC_ANA_CON21_RG_AUD45ADC_13MCK_EN_POS, AUDENC_ANA_CON21_RG_AUD45ADC_13MCK_EN_MASK); //Enable CLK from DL to UL
            hal_audio_ana_enable_ADC_13MCK(AFE_ANALOG_ADC2,enable);
            HAL_AUDIO_DELAY_US(100);

            //Capless LDO
            hal_audio_ana_enable_capless_LDO(enable);
        }

            if(analog_control&AFE_ANALOG_L_CH){
                ANA_SET_REG(AUDENC_ANA_CON25,1<<AUDENC_ANA_CON25_RG_AUD45SPAREVA25_LDO_TO_AUD45_L_POS,AUDENC_ANA_CON25_RG_AUD45SPAREVA25_LDO_TO_AUD45_L_MASK);
            }
            if(analog_control&AFE_ANALOG_R_CH){
                ANA_SET_REG(AUDENC_ANA_CON25,1<<AUDENC_ANA_CON25_RG_AUD45SPAREVA25_LDO_TO_AUD45_R_POS,AUDENC_ANA_CON25_RG_AUD45SPAREVA25_LDO_TO_AUD45_R_MASK);
            }
        if (analog_control&AFE_ANALOG_COMMON) {

            #if 0
                #if 0
                ANA_SET_REG((AUDENC_ANA_CON27),
                ((1<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_DISCHARGE_PDN_POS)|(1<<AUDENC_ANA_CON27_TBST_EN_POS)|(0<<AUDENC_ANA_CON27_REMOTE_SENSE_EN_POS)|(0<<AUDENC_ANA_CON27_PDD_TEST_POS)),
                (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_DISCHARGE_PDN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK|AUDENC_ANA_CON27_REMOTE_SENSE_EN_MASK|AUDENC_ANA_CON27_PDD_TEST_MASK));
                #else//modify for ab1568
                ANA_SET_REG((AUDENC_ANA_CON27),
                ((1<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_DISCHARGE_PDN_POS)|(1<<AUDENC_ANA_CON27_TBST_EN_POS)|(1<<AUDENC_ANA_CON27_REMOTE_SENSE_EN_POS)|(0<<AUDENC_ANA_CON27_PDD_TEST_POS)),
                (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_DISCHARGE_PDN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK|AUDENC_ANA_CON27_REMOTE_SENSE_EN_MASK|AUDENC_ANA_CON27_PDD_TEST_MASK));
                #endif

                #if 1//only for ab1568
                    ANA_SET_REG(AUDENC_ANA_CON16,3<<AUDENC_ANA_CON16_RG_AUD23SPAREVA25_POS,AUDENC_ANA_CON16_RG_AUD23SPAREVA25_MASK);
                    ANA_SET_REG(AUDENC_ANA_CON25,3<<AUDENC_ANA_CON25_RG_AUD45SPAREVA25_POS,AUDENC_ANA_CON25_RG_AUD45SPAREVA25_MASK);
                #endif
            #endif
            #if 0//move to below for ab1568 flow
                //Bias temp
                ANA_SET_REG((AUDENC_ANA_CON39),
                            (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_N_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_N_EN_POS)),
                            (AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_N_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_N_EN_MASK));
            #endif
            //ADC bias
            ANA_SET_REG((AUDENC_ANA_CON20),
                        (((adc_parameter->performance >= AFE_PEROFRMANCE_LOW_POWER_MODE)<<AUDENC_ANA_CON20_RG_AUD45ULHALFBIAS_POS)|
                         ((adc_parameter->performance >= AFE_PEROFRMANCE_LOW_POWER_MODE)<<AUDENC_ANA_CON20_RG_AUD45PREAMPLP1EN_POS)),
                        (AUDENC_ANA_CON20_RG_AUD45ULHALFBIAS_MASK|
                         AUDENC_ANA_CON20_RG_AUD45PREAMPLP1EN_MASK));
            ANA_SET_REG((AUDENC_ANA_CON21),
                        (((adc_parameter->performance == AFE_PEROFRMANCE_SUPER_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON21_RG_AUD45PREAMP_LOWPEN1_POS)|
                         ((adc_parameter->performance == AFE_PEROFRMANCE_SUPER_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON21_RG_AUD45PREAMP_LOWPEN2_POS)),
                        (AUDENC_ANA_CON21_RG_AUD45PREAMP_LOWPEN1_MASK|AUDENC_ANA_CON21_RG_AUD45PREAMP_LOWPEN2_MASK));
            ANA_SET_REG((AUDENC_ANA_CON20),
                        ((((adc_parameter->performance!=AFE_PEROFRMANCE_HIGH_MODE)&&(adc_parameter->performance!=AFE_PEROFRMANCE_LOW_POWER_MODE))<<AUDENC_ANA_CON20_RG_AUD45ADC1STSTAGELPEN_POS)|
                         ((adc_parameter->performance != AFE_PEROFRMANCE_HIGH_MODE)<<AUDENC_ANA_CON20_RG_AUD45ADC2NDSTAGELPEN_POS)|
                         ((adc_parameter->performance != AFE_PEROFRMANCE_HIGH_MODE)<<AUDENC_ANA_CON20_RG_AUD45ADCFLASHLPEN_POS)),
                        (AUDENC_ANA_CON20_RG_AUD45ADC1STSTAGELPEN_MASK|
                         AUDENC_ANA_CON20_RG_AUD45ADC2NDSTAGELPEN_MASK|
                         AUDENC_ANA_CON20_RG_AUD45ADCFLASHLPEN_MASK));

         }
        //PGA

#if 0//for ab1568 flow, if set 0 DMIC using external bias
        ANA_SET_REG((AUDENC_ANA_CON40),
                    (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0P1EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0P2EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0NEN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0P1EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0P2EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0NEN_POS)),
                    (AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0P1EN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0P1EN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0NEN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0P1EN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0P2EN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0NEN_MASK));
#endif
            //ADC bias
        afe_volume_analog_update(AFE_HW_ANALOG_GAIN_INPUT3);
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG((AUDENC_ANA_CON18),
                        (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON18_RG_AUD45PREAMPLDCCEN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_ACC20K)<<AUDENC_ANA_CON18_RG_AUD45PREAMPL_ACC20K_EN_POS)|(((adc_parameter->with_au_in_swap)? 2 : 1)<<AUDENC_ANA_CON18_RG_AUD45PREAMPLINPUTSEL_POS)|(1<<AUDENC_ANA_CON18_RG_AUD45PREAMPLON_POS)),
                        (AUDENC_ANA_CON18_RG_AUD45PREAMPLDCCEN_MASK|AUDENC_ANA_CON18_RG_AUD45PREAMPL_ACC20K_EN_MASK|AUDENC_ANA_CON18_RG_AUD45PREAMPLINPUTSEL_MASK|AUDENC_ANA_CON18_RG_AUD45PREAMPLON_MASK));
            ANA_SET_REG(AUDENC_ANA_CON21, ((adc_parameter->adc_mode!=HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON21_RG_AUD45PGAL_ACCFS_POS), AUDENC_ANA_CON21_RG_AUD45PGAL_ACCFS_MASK);
            ANA_SET_REG(AUDENC_ANA_CON18, ((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON18_RG_AUD45PREAMPLDCPRECHARGE_POS), AUDENC_ANA_CON18_RG_AUD45PREAMPLDCPRECHARGE_MASK);
            if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
                AFE_SET_REG(ABB_CLK_GEN_CFG_6, (0x0103<<ABB_CLK_GEN_CFG_6_DIV_SEL_POS)|(1<<ABB_CLK_GEN_CFG_6_DIV_CHANGE_POS)|(1<<ABB_CLK_GEN_CFG_6_DIV_EN_POS), ABB_CLK_GEN_CFG_6_DIV_SEL_MASK|ABB_CLK_GEN_CFG_6_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_6_DIV_EN_MASK);
            }

        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG((AUDENC_ANA_CON19),
                        (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON19_RG_AUD45PREAMPRDCCEN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_ACC20K)<<AUDENC_ANA_CON19_RG_AUD45PREAMPR_ACC20K_EN_POS)|(((adc_parameter->with_au_in_swap)? 1 : 2)<<AUDENC_ANA_CON19_RG_AUD45PREAMPRINPUTSEL_POS)|(1<<AUDENC_ANA_CON19_RG_AUD45PREAMPRON_POS)),
                        (AUDENC_ANA_CON19_RG_AUD45PREAMPRDCCEN_MASK|AUDENC_ANA_CON19_RG_AUD45PREAMPR_ACC20K_EN_MASK|AUDENC_ANA_CON19_RG_AUD45PREAMPRINPUTSEL_MASK|AUDENC_ANA_CON19_RG_AUD45PREAMPRON_MASK));
            ANA_SET_REG(AUDENC_ANA_CON21, ((adc_parameter->adc_mode!=HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON21_RG_AUD45PGAR_ACCFS_POS), AUDENC_ANA_CON21_RG_AUD45PGAR_ACCFS_MASK);
            ANA_SET_REG(AUDENC_ANA_CON19, ((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON19_RG_AUD45PREAMPRDCPRECHARGE_POS), AUDENC_ANA_CON19_RG_AUD45PREAMPRDCPRECHARGE_MASK);
            if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
                AFE_SET_REG(ABB_CLK_GEN_CFG_7, (0x0103<<ABB_CLK_GEN_CFG_7_DIV_SEL_POS)|(1<<ABB_CLK_GEN_CFG_7_DIV_CHANGE_POS)|(1<<ABB_CLK_GEN_CFG_7_DIV_EN_POS), ABB_CLK_GEN_CFG_7_DIV_SEL_MASK|ABB_CLK_GEN_CFG_7_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_7_DIV_EN_MASK);
            }

        }
#if 0//single mic on/off
        HAL_AUDIO_DELAY_US(1000);
        ANA_SET_REG(AUDENC_ANA_CON21, (0<<AUDENC_ANA_CON21_RG_AUD45PGAL_ACCFS_POS)|(0<<AUDENC_ANA_CON21_RG_AUD45PGAR_ACCFS_POS), AUDENC_ANA_CON21_RG_AUD45PGAL_ACCFS_MASK|AUDENC_ANA_CON21_RG_AUD45PGAR_ACCFS_MASK); //Disable fast settle
        ANA_SET_REG(AUDENC_ANA_CON18, (0<<AUDENC_ANA_CON18_RG_AUD45PREAMPLDCPRECHARGE_POS), AUDENC_ANA_CON18_RG_AUD45PREAMPLDCPRECHARGE_MASK);
        ANA_SET_REG(AUDENC_ANA_CON19, (0<<AUDENC_ANA_CON19_RG_AUD45PREAMPRDCPRECHARGE_POS), AUDENC_ANA_CON19_RG_AUD45PREAMPRDCPRECHARGE_MASK);
#endif
        //here
        //ADC
        HAL_AUDIO_DELAY_US(1000);
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON21, (0<<AUDENC_ANA_CON21_RG_AUD45PGAL_ACCFS_POS), AUDENC_ANA_CON21_RG_AUD45PGAL_ACCFS_MASK); //Disable fast settle
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON21, (0<<AUDENC_ANA_CON21_RG_AUD45PGAR_ACCFS_POS), AUDENC_ANA_CON21_RG_AUD45PGAR_ACCFS_MASK); //Disable fast settle
        }
        if (analog_control&AFE_ANALOG_COMMON) {

#if 1//single mic on/off move to here
            ANA_SET_REG(AUDENC_ANA_CON18, (0<<AUDENC_ANA_CON18_RG_AUD45PREAMPLDCPRECHARGE_POS), AUDENC_ANA_CON18_RG_AUD45PREAMPLDCPRECHARGE_MASK);
            ANA_SET_REG(AUDENC_ANA_CON19, (0<<AUDENC_ANA_CON19_RG_AUD45PREAMPRDCPRECHARGE_POS), AUDENC_ANA_CON19_RG_AUD45PREAMPRDCPRECHARGE_MASK);
#endif

            ANA_SET_REG((AUDENC_ANA_CON21),
                        ((0<<AUDENC_ANA_CON21_RG_AUD45ADCCLKSEL_POS)|(0<<AUDENC_ANA_CON21_RG_AUD45ADCCLKSOURCE_POS)),
                        (AUDENC_ANA_CON21_RG_AUD45ADCCLKSEL_MASK|AUDENC_ANA_CON21_RG_AUD45ADCCLKSOURCE_MASK));
            ANA_SET_REG(AUDENC_ANA_CON25,  ((adc_parameter->performance == AFE_PEROFRMANCE_HIGH_MODE)<<AUDENC_ANA_CON25_RG_AUD45ADCCLKHALFRST_POS), AUDENC_ANA_CON25_RG_AUD45ADCCLKHALFRST_MASK);
            ANA_SET_REG((AUDENC_ANA_CON21),((adc_parameter->performance >= AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON21_RG_AUD45ADCDAC0P25FS_POS), AUDENC_ANA_CON21_RG_AUD45ADCDAC0P25FS_MASK);
            ANA_SET_REG(AUDENC_ANA_CON24,  (adc_parameter->performance >= AFE_PEROFRMANCE_LOW_POWER_MODE)<<AUDENC_ANA_CON24_RG_AUD45ADCCLKRATEQUARTER_POS, AUDENC_ANA_CON24_RG_AUD45ADCCLKRATEQUARTER_MASK);// reset configure for VOW
            ANA_SET_REG(AUDENC_ANA_CON25,
                       (((adc_parameter->performance==AFE_PEROFRMANCE_NORMAL_MODE)||(adc_parameter->performance==AFE_PEROFRMANCE_LOW_POWER_MODE))<<AUDENC_ANA_CON25_RG_AUD45ADCCLKRATEHALF_POS),
                       AUDENC_ANA_CON25_RG_AUD45ADCCLKRATEHALF_MASK);
        }

        if (analog_control&AFE_ANALOG_COMMON) {
            //For ADC R only clock select to ana ch1
            bool fifo_sync_clock = (!(analog_control&AFE_ANALOG_L_CH)) ? true : adc_parameter->is_fifo_sync_clock_ch1;
            AFE_SET_REG(AFUNC_AUD_CON4_3, fifo_sync_clock <<AFUNC_AUD_CON4_3_ANA_CLK_SEL_POS, AFUNC_AUD_CON4_3_ANA_CLK_SEL_MASK);// 0: ana_ch2 clock is selected, 1: ana_ch3 clock is selected
        }
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON18, (2<<AUDENC_ANA_CON18_RG_AUD45ADCLINPUTSEL_POS)|(1<<AUDENC_ANA_CON18_RG_AUD45ADCLPWRUP_POS), AUDENC_ANA_CON18_RG_AUD45ADCLINPUTSEL_MASK|AUDENC_ANA_CON18_RG_AUD45ADCLPWRUP_MASK);
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON19, (2<<AUDENC_ANA_CON19_RG_AUD45ADCRINPUTSEL_POS)|(1<<AUDENC_ANA_CON19_RG_AUD45ADCRPWRUP_POS), AUDENC_ANA_CON19_RG_AUD45ADCRINPUTSEL_MASK|AUDENC_ANA_CON19_RG_AUD45ADCRPWRUP_MASK);
        }

        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON18, (2<<AUDENC_ANA_CON18_RG_AUD45ADCLINPUTSEL_POS)|(1<<AUDENC_ANA_CON18_RG_AUD45ADCLPWRUP_POS), AUDENC_ANA_CON18_RG_AUD45ADCLINPUTSEL_MASK|AUDENC_ANA_CON18_RG_AUD45ADCLPWRUP_MASK);
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON19, (2<<AUDENC_ANA_CON19_RG_AUD45ADCRINPUTSEL_POS)|(1<<AUDENC_ANA_CON19_RG_AUD45ADCRPWRUP_POS), AUDENC_ANA_CON19_RG_AUD45ADCRINPUTSEL_MASK|AUDENC_ANA_CON19_RG_AUD45ADCRPWRUP_MASK);
        }
        #if 0
        //add to sync yida for gain
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON18, (0<<AUDENC_ANA_CON18_RG_AUD45PREAMPLGAIN_POS),AUDENC_ANA_CON18_RG_AUD45PREAMPLGAIN_MASK);
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON19, (0<<AUDENC_ANA_CON19_RG_AUD45PREAMPRGAIN_POS), AUDENC_ANA_CON19_RG_AUD45PREAMPRGAIN_MASK);
        }
        #endif
    } else {
        //Disable

        //Workaround: UL FIFO clock soure switch
        if (!(analog_control&AFE_ANALOG_COMMON)) {
            bool ana_clk_sel = (AFE_GET_REG(AFUNC_AUD_CON4_3)&AFUNC_AUD_CON4_3_ANA_CLK_SEL_MASK);
            if ((ana_clk_sel && (analog_control&AFE_ANALOG_R_CH) )||
                (!ana_clk_sel&&(analog_control&AFE_ANALOG_L_CH))) {
                HAL_AUDIO_LOG_WARNING("DSP - Warning Hal Audio Device ADC clock select %d :0x%x @@", 2, ana_clk_sel, AFE_GET_REG(AFUNC_AUD_CON4_3));
                //Disable UL path
                hal_audio_ul_set_enable(HAL_AUDIO_AGENT_DEVICE_ADDA_UL3, false);
                hal_audio_ul_reset_fifo(HAL_AUDIO_AGENT_DEVICE_ADDA_UL3, true);
                //Switch to ch2 while closing ch3
                AFE_SET_REG(AFUNC_AUD_CON4_3, (!ana_clk_sel)<<AFUNC_AUD_CON4_3_ANA_CLK_SEL_POS, AFUNC_AUD_CON4_3_ANA_CLK_SEL_MASK);// 0: ana_ch4 clock is selected, 1: ana_ch5 clock is selected
                //Enable UL path
                hal_audio_ul_reset_fifo(HAL_AUDIO_AGENT_DEVICE_ADDA_UL3, false);
                hal_audio_ul_set_enable(HAL_AUDIO_AGENT_DEVICE_ADDA_UL3, true);
            }
        }

        //Analog ADC
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON18, (0<<AUDENC_ANA_CON18_RG_AUD45ADCLINPUTSEL_POS)|(0<<AUDENC_ANA_CON18_RG_AUD45ADCLPWRUP_POS), AUDENC_ANA_CON18_RG_AUD45ADCLINPUTSEL_MASK|AUDENC_ANA_CON18_RG_AUD45ADCLPWRUP_MASK);
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON19, (0<<AUDENC_ANA_CON19_RG_AUD45ADCRINPUTSEL_POS)|(0<<AUDENC_ANA_CON19_RG_AUD45ADCRPWRUP_POS), AUDENC_ANA_CON19_RG_AUD45ADCRINPUTSEL_MASK|AUDENC_ANA_CON19_RG_AUD45ADCRPWRUP_MASK);
        }
        if (analog_control&AFE_ANALOG_COMMON) {
            ANA_SET_REG(AUDENC_ANA_CON21,
                        (0<<AUDENC_ANA_CON21_RG_AUD45ADCCLKSEL_POS)|(0<<AUDENC_ANA_CON21_RG_AUD45ADCCLKSOURCE_POS)|(0<<AUDENC_ANA_CON21_RG_AUD45ADCDAC0P25FS_POS),
                        AUDENC_ANA_CON21_RG_AUD45ADCCLKSEL_MASK|AUDENC_ANA_CON21_RG_AUD45ADCCLKSOURCE_MASK|AUDENC_ANA_CON21_RG_AUD45ADCDAC0P25FS_MASK);

            ANA_SET_REG(AUDENC_ANA_CON24, (0<<AUDENC_ANA_CON24_RG_AUD45ADCCLKRATEQUARTER_POS), AUDENC_ANA_CON24_RG_AUD45ADCCLKRATEQUARTER_MASK);
            ANA_SET_REG(AUDENC_ANA_CON25, (0<<AUDENC_ANA_CON25_RG_AUD45ADCCLKRATEHALF_POS)|(1<<AUDENC_ANA_CON25_RG_AUD45ADCCLKHALFRST_POS), AUDENC_ANA_CON25_RG_AUD45ADCCLKRATEHALF_MASK|AUDENC_ANA_CON25_RG_AUD45ADCCLKHALFRST_MASK);
        }

        //PGA
        if (analog_control&AFE_ANALOG_L_CH) {
           ANA_SET_REG(AUDENC_ANA_CON18, (0<<AUDENC_ANA_CON18_RG_AUD45PREAMPLINPUTSEL_POS)|(0<<AUDENC_ANA_CON18_RG_AUD45PREAMPLON_POS), AUDENC_ANA_CON18_RG_AUD45PREAMPLINPUTSEL_MASK|AUDENC_ANA_CON18_RG_AUD45PREAMPLON_MASK);
           if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
               AFE_SET_REG(ABB_CLK_GEN_CFG_6, (0<<ABB_CLK_GEN_CFG_6_DIV_CHANGE_POS)|(0<<ABB_CLK_GEN_CFG_6_DIV_EN_POS), ABB_CLK_GEN_CFG_6_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_6_DIV_EN_MASK);
           }

        }
        if (analog_control&AFE_ANALOG_R_CH) {
           ANA_SET_REG(AUDENC_ANA_CON19, (0<<AUDENC_ANA_CON19_RG_AUD45PREAMPRINPUTSEL_POS)|(0<<AUDENC_ANA_CON19_RG_AUD45PREAMPRON_POS), AUDENC_ANA_CON19_RG_AUD45PREAMPRINPUTSEL_MASK|AUDENC_ANA_CON19_RG_AUD45PREAMPRON_MASK);
           if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
               AFE_SET_REG(ABB_CLK_GEN_CFG_7, (0<<ABB_CLK_GEN_CFG_7_DIV_CHANGE_POS)|(0<<ABB_CLK_GEN_CFG_7_DIV_EN_POS), ABB_CLK_GEN_CFG_7_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_7_DIV_EN_MASK);
           }

        }

        if (analog_control&AFE_ANALOG_COMMON) {
            //Bias

            ANA_SET_REG((AUDENC_ANA_CON40),
                    0,
                    (AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0P1EN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0P1EN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS4_DCSW0NEN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0P1EN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0P2EN_MASK|AUDENC_ANA_CON40_RG_AUDMICBIAS5_DCSW0NEN_MASK));
            //LDO
            hal_audio_ana_enable_capless_LDO(enable);
         }
            if(analog_control&AFE_ANALOG_L_CH){
                ANA_SET_REG(AUDENC_ANA_CON25,0<<AUDENC_ANA_CON25_RG_AUD45SPAREVA25_LDO_TO_AUD45_L_POS,AUDENC_ANA_CON25_RG_AUD45SPAREVA25_LDO_TO_AUD45_L_MASK);
            }
            if(analog_control&AFE_ANALOG_R_CH){
                ANA_SET_REG(AUDENC_ANA_CON25,0<<AUDENC_ANA_CON25_RG_AUD45SPAREVA25_LDO_TO_AUD45_R_POS,AUDENC_ANA_CON25_RG_AUD45SPAREVA25_LDO_TO_AUD45_R_MASK);
            }
         if (analog_control&AFE_ANALOG_COMMON){
            #if 0
            ANA_SET_REG((AUDENC_ANA_CON27),
                        ((0<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_TBST_EN_POS)),
                        (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK));
            #endif
            //TOP
            //ANA_SET_REG(AUDENC_ANA_CON12, 0<<AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_POS, AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_MASK); //Disable CLK to ADC23
            hal_audio_ana_enable_ADC_13MCK(AFE_ANALOG_ADC2,enable);
            hal_audio_afe_enable_clksq(false);  //Disable CLKSQ 26MHz
        }
    }
    return false;
}

bool hal_audio_ana_set_adc23_enable (hal_audio_device_parameter_adc_t *adc_parameter, afe_analog_control_t analog_control, bool enable)
{
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio ANA ADC23:0x%x, Mode:%d, Performance:%d, with_au_in_swap:%d enable:%d", 5, analog_control, adc_parameter->adc_mode, adc_parameter->performance,adc_parameter->with_au_in_swap, enable);
    if (enable) {
        if (analog_control&AFE_ANALOG_COMMON) {
            hal_save_adc_performance_mode(AFE_ANALOG_ADC1,adc_parameter->performance);
            hal_volume_set_analog_mode(AFE_HW_ANALOG_GAIN_INPUT2, adc_parameter->adc_mode);

            //Top
            hal_audio_afe_enable_clksq(true);       //Enable CLKSQ 26MHz
            //ANA_SET_REG(AUDENC_ANA_CON12, 1<<AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_POS, AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_MASK); //Enable CLK from DL to UL
            hal_audio_ana_enable_ADC_13MCK(AFE_ANALOG_ADC1,enable);
            HAL_AUDIO_DELAY_US(100);
            //Capless LDO
            hal_audio_ana_enable_capless_LDO(enable);
            }

            if(analog_control&AFE_ANALOG_L_CH){
                ANA_SET_REG(AUDENC_ANA_CON16,1<<AUDENC_ANA_CON16_RG_AUD23SPAREVA25_LDO_TO_AUD23_L_POS,AUDENC_ANA_CON16_RG_AUD23SPAREVA25_LDO_TO_AUD23_L_MASK);
            }
            if(analog_control&AFE_ANALOG_R_CH){
                ANA_SET_REG(AUDENC_ANA_CON16,1<<AUDENC_ANA_CON16_RG_AUD23SPAREVA25_LDO_TO_AUD23_R_POS,AUDENC_ANA_CON16_RG_AUD23SPAREVA25_LDO_TO_AUD23_R_MASK);
            }
            if (analog_control&AFE_ANALOG_COMMON){
            #if 0
                #if 0
                ANA_SET_REG((AUDENC_ANA_CON27),
                ((1<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_DISCHARGE_PDN_POS)|(1<<AUDENC_ANA_CON27_TBST_EN_POS)|(0<<AUDENC_ANA_CON27_REMOTE_SENSE_EN_POS)|(0<<AUDENC_ANA_CON27_PDD_TEST_POS)),
                (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_DISCHARGE_PDN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK|AUDENC_ANA_CON27_REMOTE_SENSE_EN_MASK|AUDENC_ANA_CON27_PDD_TEST_MASK));
                #else//modify for ab1568
                ANA_SET_REG((AUDENC_ANA_CON27),
                ((1<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_DISCHARGE_PDN_POS)|(1<<AUDENC_ANA_CON27_TBST_EN_POS)|(1<<AUDENC_ANA_CON27_REMOTE_SENSE_EN_POS)|(0<<AUDENC_ANA_CON27_PDD_TEST_POS)),
                (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_DISCHARGE_PDN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK|AUDENC_ANA_CON27_REMOTE_SENSE_EN_MASK|AUDENC_ANA_CON27_PDD_TEST_MASK));
                #endif

                #if 1//only for ab1568
                    ANA_SET_REG(AUDENC_ANA_CON16,3<<AUDENC_ANA_CON16_RG_AUD23SPAREVA25_POS,AUDENC_ANA_CON16_RG_AUD23SPAREVA25_MASK);
                    ANA_SET_REG(AUDENC_ANA_CON25,3<<AUDENC_ANA_CON25_RG_AUD45SPAREVA25_POS,AUDENC_ANA_CON25_RG_AUD45SPAREVA25_MASK);
                #endif
            #endif
            #if 0//move to below for ab1568 flow
                //Bias temp
                ANA_SET_REG((AUDENC_ANA_CON39),
                            (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_N_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_N_EN_POS)),
                            (AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_N_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_N_EN_MASK));
            #endif
            //ADC bias
            ANA_SET_REG((AUDENC_ANA_CON11),
                        (((adc_parameter->performance >= AFE_PEROFRMANCE_LOW_POWER_MODE)<<AUDENC_ANA_CON11_RG_AUD23ULHALFBIAS_POS)|
                         ((adc_parameter->performance >= AFE_PEROFRMANCE_LOW_POWER_MODE)<<AUDENC_ANA_CON11_RG_AUD23PREAMPLP1EN_POS)),
                        (AUDENC_ANA_CON11_RG_AUD23ULHALFBIAS_MASK|
                         AUDENC_ANA_CON11_RG_AUD23PREAMPLP1EN_MASK));
            ANA_SET_REG((AUDENC_ANA_CON12),
                        (((adc_parameter->performance == AFE_PEROFRMANCE_SUPER_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON12_RG_AUD23PREAMP_LOWPEN1_POS)|
                         ((adc_parameter->performance == AFE_PEROFRMANCE_SUPER_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON12_RG_AUD23PREAMP_LOWPEN2_POS)),
                        (AUDENC_ANA_CON12_RG_AUD23PREAMP_LOWPEN1_MASK|AUDENC_ANA_CON12_RG_AUD23PREAMP_LOWPEN2_MASK));
            ANA_SET_REG((AUDENC_ANA_CON11),
                        ((((adc_parameter->performance!=AFE_PEROFRMANCE_HIGH_MODE)&&(adc_parameter->performance!=AFE_PEROFRMANCE_LOW_POWER_MODE))<<AUDENC_ANA_CON11_RG_AUD23ADC1STSTAGELPEN_POS)|
                         ((adc_parameter->performance != AFE_PEROFRMANCE_HIGH_MODE)<<AUDENC_ANA_CON11_RG_AUD23ADC2NDSTAGELPEN_POS)|
                         ((adc_parameter->performance != AFE_PEROFRMANCE_HIGH_MODE)<<AUDENC_ANA_CON11_RG_AUD23ADCFLASHLPEN_POS)),
                        (AUDENC_ANA_CON11_RG_AUD23ADC1STSTAGELPEN_MASK|
                         AUDENC_ANA_CON11_RG_AUD23ADC2NDSTAGELPEN_MASK|
                         AUDENC_ANA_CON11_RG_AUD23ADCFLASHLPEN_MASK));

            }
        //PGA

#if 0//for ab1568 flow, if set 0 DMIC using external bias
        ANA_SET_REG((AUDENC_ANA_CON39),
                    (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS2_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS2_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS2_DCC_N_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS3_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS3_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS3_DCC_N_EN_POS)),
                    (AUDENC_ANA_CON39_BIAS2_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS2_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS2_DCC_N_EN_MASK|AUDENC_ANA_CON39_BIAS3_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS3_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS3_DCC_N_EN_MASK));
#endif

            //ADC bias
        afe_volume_analog_update(AFE_HW_ANALOG_GAIN_INPUT2);
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG((AUDENC_ANA_CON9),
                        (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON9_RG_AUD23PREAMPLDCCEN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_ACC20K)<<AUDENC_ANA_CON9_RG_AUD23PREAMPL_ACC20K_EN_POS)|(((adc_parameter->with_au_in_swap)? 2 : 1)<<AUDENC_ANA_CON9_RG_AUD23PREAMPLINPUTSEL_POS)|(1<<AUDENC_ANA_CON9_RG_AUD23PREAMPLON_POS)),
                        (AUDENC_ANA_CON9_RG_AUD23PREAMPLDCCEN_MASK|AUDENC_ANA_CON9_RG_AUD23PREAMPL_ACC20K_EN_MASK|AUDENC_ANA_CON9_RG_AUD23PREAMPLINPUTSEL_MASK|AUDENC_ANA_CON9_RG_AUD23PREAMPLON_MASK));
            ANA_SET_REG(AUDENC_ANA_CON12, ((adc_parameter->adc_mode!=HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON12_RG_AUD23PGAL_ACCFS_POS), AUDENC_ANA_CON12_RG_AUD23PGAL_ACCFS_MASK);
            ANA_SET_REG(AUDENC_ANA_CON9, ((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON9_RG_AUD23PREAMPLDCPRECHARGE_POS), AUDENC_ANA_CON9_RG_AUD23PREAMPLDCPRECHARGE_MASK);
            if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
                AFE_SET_REG(ABB_CLK_GEN_CFG_4, (0x0103<<ABB_CLK_GEN_CFG_4_DIV_SEL_POS)|(1<<ABB_CLK_GEN_CFG_4_DIV_CHANGE_POS)|(1<<ABB_CLK_GEN_CFG_4_DIV_EN_POS), ABB_CLK_GEN_CFG_4_DIV_SEL_MASK|ABB_CLK_GEN_CFG_4_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_4_DIV_EN_MASK);
            }

        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG((AUDENC_ANA_CON10),
                        (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON10_RG_AUD23PREAMPRDCCEN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_ACC20K)<<AUDENC_ANA_CON10_RG_AUD23PREAMPR_ACC20K_EN_POS)|(((adc_parameter->with_au_in_swap)? 1 : 2)<<AUDENC_ANA_CON10_RG_AUD23PREAMPRINPUTSEL_POS)|(1<<AUDENC_ANA_CON10_RG_AUD23PREAMPRON_POS)),
                        (AUDENC_ANA_CON10_RG_AUD23PREAMPRDCCEN_MASK|AUDENC_ANA_CON10_RG_AUD23PREAMPR_ACC20K_EN_MASK|AUDENC_ANA_CON10_RG_AUD23PREAMPRINPUTSEL_MASK|AUDENC_ANA_CON10_RG_AUD23PREAMPRON_MASK));
            ANA_SET_REG(AUDENC_ANA_CON12, ((adc_parameter->adc_mode!=HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON12_RG_AUD23PGAR_ACCFS_POS), AUDENC_ANA_CON12_RG_AUD23PGAR_ACCFS_MASK);
            ANA_SET_REG(AUDENC_ANA_CON10, ((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON10_RG_AUD23PREAMPRDCPRECHARGE_POS), AUDENC_ANA_CON10_RG_AUD23PREAMPRDCPRECHARGE_MASK);
            if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
                AFE_SET_REG(ABB_CLK_GEN_CFG_5, (0x0103<<ABB_CLK_GEN_CFG_5_DIV_SEL_POS)|(1<<ABB_CLK_GEN_CFG_5_DIV_CHANGE_POS)|(1<<ABB_CLK_GEN_CFG_5_DIV_EN_POS), ABB_CLK_GEN_CFG_5_DIV_SEL_MASK|ABB_CLK_GEN_CFG_5_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_5_DIV_EN_MASK);
            }

        }
        #if 0//single mic on/off
        HAL_AUDIO_DELAY_US(1000);
        ANA_SET_REG(AUDENC_ANA_CON12, (0<<AUDENC_ANA_CON12_RG_AUD23PGAL_ACCFS_POS)|(0<<AUDENC_ANA_CON12_RG_AUD23PGAR_ACCFS_POS), AUDENC_ANA_CON12_RG_AUD23PGAL_ACCFS_MASK|AUDENC_ANA_CON12_RG_AUD23PGAR_ACCFS_MASK); //Disable fast settle
        ANA_SET_REG(AUDENC_ANA_CON9, (0<<AUDENC_ANA_CON9_RG_AUD23PREAMPLDCPRECHARGE_POS), AUDENC_ANA_CON9_RG_AUD23PREAMPLDCPRECHARGE_MASK);
        ANA_SET_REG(AUDENC_ANA_CON10, (0<<AUDENC_ANA_CON10_RG_AUD23PREAMPRDCPRECHARGE_POS), AUDENC_ANA_CON10_RG_AUD23PREAMPRDCPRECHARGE_MASK);
        #endif
        //here
        //ADC
        HAL_AUDIO_DELAY_US(1000);
        if (analog_control&AFE_ANALOG_L_CH){
            ANA_SET_REG(AUDENC_ANA_CON12, (0<<AUDENC_ANA_CON12_RG_AUD23PGAL_ACCFS_POS), AUDENC_ANA_CON12_RG_AUD23PGAL_ACCFS_MASK); //Disable fast settle
        }
        if (analog_control&AFE_ANALOG_R_CH){
            ANA_SET_REG(AUDENC_ANA_CON12, (0<<AUDENC_ANA_CON12_RG_AUD23PGAR_ACCFS_POS), AUDENC_ANA_CON12_RG_AUD23PGAR_ACCFS_MASK); //Disable fast settle
        }
        if (analog_control&AFE_ANALOG_COMMON) {
#if 1//single mic on/off move to here

            ANA_SET_REG(AUDENC_ANA_CON9, (0<<AUDENC_ANA_CON9_RG_AUD23PREAMPLDCPRECHARGE_POS), AUDENC_ANA_CON9_RG_AUD23PREAMPLDCPRECHARGE_MASK);
            ANA_SET_REG(AUDENC_ANA_CON10, (0<<AUDENC_ANA_CON10_RG_AUD23PREAMPRDCPRECHARGE_POS), AUDENC_ANA_CON10_RG_AUD23PREAMPRDCPRECHARGE_MASK);
#endif
            ANA_SET_REG((AUDENC_ANA_CON12),
                        ((0<<AUDENC_ANA_CON12_RG_AUD23ADCCLKSEL_POS)|(0<<AUDENC_ANA_CON12_RG_AUD23ADCCLKSOURCE_POS)),
                        (AUDENC_ANA_CON12_RG_AUD23ADCCLKSEL_MASK|AUDENC_ANA_CON12_RG_AUD23ADCCLKSOURCE_MASK));
            ANA_SET_REG(AUDENC_ANA_CON16,  ((adc_parameter->performance == AFE_PEROFRMANCE_HIGH_MODE)<<AUDENC_ANA_CON16_RG_AUD23ADCCLKHALFRST_POS), AUDENC_ANA_CON16_RG_AUD23ADCCLKHALFRST_MASK);
            ANA_SET_REG((AUDENC_ANA_CON12),((adc_parameter->performance >= AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON12_RG_AUD23ADCDAC0P25FS_POS), AUDENC_ANA_CON12_RG_AUD23ADCDAC0P25FS_MASK);
            ANA_SET_REG(AUDENC_ANA_CON15,  (adc_parameter->performance >= AFE_PEROFRMANCE_LOW_POWER_MODE)<<AUDENC_ANA_CON15_RG_AUD23ADCCLKRATEQUARTER_POS, AUDENC_ANA_CON15_RG_AUD23ADCCLKRATEQUARTER_MASK);// reset configure for VOW
            ANA_SET_REG(AUDENC_ANA_CON16,
                       (((adc_parameter->performance==AFE_PEROFRMANCE_NORMAL_MODE)||(adc_parameter->performance==AFE_PEROFRMANCE_LOW_POWER_MODE))<<AUDENC_ANA_CON16_RG_AUD23ADCCLKRATEHALF_POS),
                       AUDENC_ANA_CON16_RG_AUD23ADCCLKRATEHALF_MASK);
        }

        if (analog_control&AFE_ANALOG_COMMON) {
            //For ADC R only clock select to ana ch1
            bool fifo_sync_clock = (!(analog_control&AFE_ANALOG_L_CH)) ? true : adc_parameter->is_fifo_sync_clock_ch1;
            AFE_SET_REG(AFUNC_AUD_CON4_2, fifo_sync_clock <<AFUNC_AUD_CON4_2_ANA_CLK_SEL_POS, AFUNC_AUD_CON4_2_ANA_CLK_SEL_MASK);// 0: ana_ch2 clock is selected, 1: ana_ch3 clock is selected
        }
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON9, (2<<AUDENC_ANA_CON9_RG_AUD23ADCLINPUTSEL_POS)|(1<<AUDENC_ANA_CON9_RG_AUD23ADCLPWRUP_POS), AUDENC_ANA_CON9_RG_AUD23ADCLINPUTSEL_MASK|AUDENC_ANA_CON9_RG_AUD23ADCLPWRUP_MASK);
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON10, (2<<AUDENC_ANA_CON10_RG_AUD23ADCRINPUTSEL_POS)|(1<<AUDENC_ANA_CON10_RG_AUD23ADCRPWRUP_POS), AUDENC_ANA_CON10_RG_AUD23ADCRINPUTSEL_MASK|AUDENC_ANA_CON10_RG_AUD23ADCRPWRUP_MASK);
        }


    } else {
        //Disable

        //Workaround: UL FIFO clock soure switch
        if (!(analog_control&AFE_ANALOG_COMMON)) {
            bool ana_clk_sel = (AFE_GET_REG(AFUNC_AUD_CON4_2)&AFUNC_AUD_CON4_2_ANA_CLK_SEL_MASK);
            if ((ana_clk_sel && (analog_control&AFE_ANALOG_R_CH) )||
                (!ana_clk_sel&&(analog_control&AFE_ANALOG_L_CH))) {
                HAL_AUDIO_LOG_WARNING("DSP - Warning Hal Audio Device ADC clock select %d :0x%x @@", 2, ana_clk_sel, AFE_GET_REG(AFUNC_AUD_CON4_2));
                //Disable UL path
                hal_audio_ul_set_enable(HAL_AUDIO_AGENT_DEVICE_ADDA_UL2, false);
                hal_audio_ul_reset_fifo(HAL_AUDIO_AGENT_DEVICE_ADDA_UL2, true);
                //Switch to ch2 while closing ch3
                AFE_SET_REG(AFUNC_AUD_CON4_2, (!ana_clk_sel)<<AFUNC_AUD_CON4_2_ANA_CLK_SEL_POS, AFUNC_AUD_CON4_2_ANA_CLK_SEL_MASK);// 0: ana_ch2 clock is selected, 1: ana_ch3 clock is selected
                //Enable UL path
                hal_audio_ul_reset_fifo(HAL_AUDIO_AGENT_DEVICE_ADDA_UL2, false);
                hal_audio_ul_set_enable(HAL_AUDIO_AGENT_DEVICE_ADDA_UL2, true);
            }
        }

        //Analog ADC
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON9, (0<<AUDENC_ANA_CON9_RG_AUD23ADCLINPUTSEL_POS)|(0<<AUDENC_ANA_CON9_RG_AUD23ADCLPWRUP_POS), AUDENC_ANA_CON9_RG_AUD23ADCLINPUTSEL_MASK|AUDENC_ANA_CON9_RG_AUD23ADCLPWRUP_MASK);
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON10, (0<<AUDENC_ANA_CON10_RG_AUD23ADCRINPUTSEL_POS)|(0<<AUDENC_ANA_CON10_RG_AUD23ADCRPWRUP_POS), AUDENC_ANA_CON10_RG_AUD23ADCRINPUTSEL_MASK|AUDENC_ANA_CON10_RG_AUD23ADCRPWRUP_MASK);
        }
        if (analog_control&AFE_ANALOG_COMMON) {
            ANA_SET_REG(AUDENC_ANA_CON12,
                        (0<<AUDENC_ANA_CON12_RG_AUD23ADCCLKSEL_POS)|(0<<AUDENC_ANA_CON12_RG_AUD23ADCCLKSOURCE_POS)|(0<<AUDENC_ANA_CON12_RG_AUD23ADCDAC0P25FS_POS),
                        AUDENC_ANA_CON12_RG_AUD23ADCCLKSEL_MASK|AUDENC_ANA_CON12_RG_AUD23ADCCLKSOURCE_MASK|AUDENC_ANA_CON12_RG_AUD23ADCDAC0P25FS_MASK);

            ANA_SET_REG(AUDENC_ANA_CON15, (0<<AUDENC_ANA_CON15_RG_AUD23ADCCLKRATEQUARTER_POS), AUDENC_ANA_CON15_RG_AUD23ADCCLKRATEQUARTER_MASK);
            ANA_SET_REG(AUDENC_ANA_CON16, (0<<AUDENC_ANA_CON16_RG_AUD23ADCCLKRATEHALF_POS)|(1<<AUDENC_ANA_CON16_RG_AUD23ADCCLKHALFRST_POS), AUDENC_ANA_CON16_RG_AUD23ADCCLKRATEHALF_MASK|AUDENC_ANA_CON16_RG_AUD23ADCCLKHALFRST_MASK);
        }

        //PGA
        if (analog_control&AFE_ANALOG_L_CH) {
           ANA_SET_REG(AUDENC_ANA_CON9, (0<<AUDENC_ANA_CON9_RG_AUD23PREAMPLINPUTSEL_POS)|(0<<AUDENC_ANA_CON9_RG_AUD23PREAMPLON_POS), AUDENC_ANA_CON9_RG_AUD23PREAMPLINPUTSEL_MASK|AUDENC_ANA_CON9_RG_AUD23PREAMPLON_MASK);
           if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
               AFE_SET_REG(ABB_CLK_GEN_CFG_4, (0<<ABB_CLK_GEN_CFG_4_DIV_SEL_POS)|(0<<ABB_CLK_GEN_CFG_4_DIV_CHANGE_POS)|(0<<ABB_CLK_GEN_CFG_4_DIV_EN_POS), ABB_CLK_GEN_CFG_4_DIV_SEL_MASK|ABB_CLK_GEN_CFG_4_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_4_DIV_EN_MASK);
           }

        }
        if (analog_control&AFE_ANALOG_R_CH) {
           ANA_SET_REG(AUDENC_ANA_CON10, (0<<AUDENC_ANA_CON10_RG_AUD23PREAMPRINPUTSEL_POS)|(0<<AUDENC_ANA_CON10_RG_AUD23PREAMPRON_POS), AUDENC_ANA_CON10_RG_AUD23PREAMPRINPUTSEL_MASK|AUDENC_ANA_CON10_RG_AUD23PREAMPRON_MASK);
           if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
               AFE_SET_REG(ABB_CLK_GEN_CFG_5, (0<<ABB_CLK_GEN_CFG_5_DIV_SEL_POS)|(0<<ABB_CLK_GEN_CFG_5_DIV_CHANGE_POS)|(0<<ABB_CLK_GEN_CFG_5_DIV_EN_POS), ABB_CLK_GEN_CFG_5_DIV_SEL_MASK|ABB_CLK_GEN_CFG_5_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_5_DIV_EN_MASK);
           }

        }

        if (analog_control&AFE_ANALOG_COMMON) {
            //Bias

            ANA_SET_REG((AUDENC_ANA_CON39),
                        (0),
                        (AUDENC_ANA_CON39_BIAS2_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS2_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS2_DCC_N_EN_MASK|AUDENC_ANA_CON39_BIAS3_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS3_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS3_DCC_N_EN_MASK));

            //LDO
            hal_audio_ana_enable_capless_LDO(enable);
        }

            if(analog_control&AFE_ANALOG_L_CH){
                ANA_SET_REG(AUDENC_ANA_CON16,0<<AUDENC_ANA_CON16_RG_AUD23SPAREVA25_LDO_TO_AUD23_L_POS,AUDENC_ANA_CON16_RG_AUD23SPAREVA25_LDO_TO_AUD23_L_MASK);
            }
            if(analog_control&AFE_ANALOG_R_CH){
                ANA_SET_REG(AUDENC_ANA_CON16,0<<AUDENC_ANA_CON16_RG_AUD23SPAREVA25_LDO_TO_AUD23_R_POS,AUDENC_ANA_CON16_RG_AUD23SPAREVA25_LDO_TO_AUD23_R_MASK);
            }
        if (analog_control&AFE_ANALOG_COMMON){
            #if 0
            ANA_SET_REG((AUDENC_ANA_CON27),
                        ((0<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_TBST_EN_POS)),
                        (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK));
            #endif
            //TOP
            //ANA_SET_REG(AUDENC_ANA_CON12, 0<<AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_POS, AUDENC_ANA_CON12_RG_AUD23ADC_13MCK_EN_MASK); //Disable CLK to ADC23
            hal_audio_ana_enable_ADC_13MCK(AFE_ANALOG_ADC1,enable);
            hal_audio_afe_enable_clksq(false);  //Disable CLKSQ 26MHz
        }
    }
    return false;
}

bool hal_audio_ana_set_vad_irq_mask (bool mask)
{
#if (HAL_AUDIO_VAD_DRIVER)
    xthal_set_intclear(VAD_IRQn);
    AFE_SET_REG(VAD_CTL5, mask<<VAD_CTL5_MASK_IRQ_POS, VAD_CTL5_MASK_IRQ_MASK);
    HAL_AUDIO_LOG_INFO("DSP - hal_audio_ana_set_vad_irq_mask:%d", 1, mask);
#else
    UNUSED(mask);
#endif
    return false;
}

bool hal_audio_ana_set_vad_analog_enable (hal_audio_device_parameter_vad_t *vad_parameter, bool enable)
{
#if (HAL_AUDIO_VAD_DRIVER)
    uint32_t precharge_pos;
    if (enable) {
        HAL_AUDIO_LOG_INFO("hal vad setting analog input_sel:%d, amp_gain:%d", 2, vad_parameter->input_sel, vad_parameter->amp_gain);

        //AFE_WRITE(VAD_CTL1, 0x0A070000);
        //AFE_WRITE(VAD_CTL2, 0x090E0000);
        //AFE_WRITE(VAD_CTL3, 0x00011478);
        //AFE_WRITE(VAD_CTL4, 0x0000017E);
        AFE_SET_REG(VAD_CTL5, vad_parameter->threshold_0<<VAD_CTL5_MASK_THRES_POS, VAD_CTL5_MASK_THRES_MASK);
        //AFE_WRITE(VAD_CTL6, 0x00010000);
        AFE_SET_REG(VAD_CTL7, vad_parameter->threshold_1<<VAD_CTL7_MASK_THRE1_POS, VAD_CTL7_MASK_THRE1_MASK);


        ANA_SET_REG(VAD_ANA_CON0, 1<<VAD_ANA_CON0_CIRCUIT_EN_POS, VAD_ANA_CON0_CIRCUIT_EN_MASK);        //Enable VAD circuit
        ANA_SET_REG(VAD_ANA_CON0, 7<<VAD_ANA_CON0_IBAS_DAC_LV_POS, VAD_ANA_CON0_IBAS_DAC_LV_MASK);      //Trim DAC segment current default

        ANA_SET_REG(VAD_ANA_CON1, vad_parameter->amp_gain<<VAD_ANA_CON1_MIC_GAIN_LV_POS, VAD_ANA_CON1_MIC_GAIN_LV_MASK);//

        switch (vad_parameter->input_sel) {
            case HAL_AUDIO_VAD_INPUT_MIC0_P:
                precharge_pos = VAD_ANA_CON5_VIN0N_PRCH_MASK;//VAD_ANA_CON5_PRCH_MASK ^ VAD_ANA_CON5_VIN0P_PRCH_MASK;
                break;
            case HAL_AUDIO_VAD_INPUT_MIC0_N:
                precharge_pos = VAD_ANA_CON5_VIN0P_PRCH_MASK;//VAD_ANA_CON5_PRCH_MASK ^ VAD_ANA_CON5_VIN0N_PRCH_MASK;;
                break;
            case HAL_AUDIO_VAD_INPUT_MIC1_P:
                precharge_pos = VAD_ANA_CON5_VIN1N_PRCH_MASK;//VAD_ANA_CON5_PRCH_MASK ^ VAD_ANA_CON5_VIN1P_PRCH_MASK;;
                break;
            case HAL_AUDIO_VAD_INPUT_MIC1_N:
                precharge_pos = VAD_ANA_CON5_VIN1P_PRCH_MASK;//VAD_ANA_CON5_PRCH_MASK ^ VAD_ANA_CON5_VIN1N_PRCH_MASK;;
                break;
            default:
                precharge_pos = 0;
                break;
        }
        ANA_SET_REG((VAD_ANA_CON5),
                    ((3<<VAD_ANA_CON5_OP1_IDAC_LV_POS)|
                     (7<<VAD_ANA_CON5_OP2_IDAC_LV_POS)|
                     (precharge_pos)),
                    (VAD_ANA_CON5_OP1_IDAC_LV_MASK|VAD_ANA_CON5_OP2_IDAC_LV_MASK|VAD_ANA_CON5_PRCH_MASK));

        ANA_SET_REG(VAD_ANA_CON3, vad_parameter->input_sel<<VAD_ANA_CON3_VAD_MIC_SEL_LV_POS, VAD_ANA_CON3_VAD_MIC_SEL_LV_MASK);    // 0:VIN0_P , 1:VIN0_N, 2:VIN1_P, 3:VIN1_N

        #if 0
        /* For monitor*/
        ANA_SET_REG(VAD_ANA_CON0, 1<<VAD_ANA_CON0_EN_ATST_LV_POS, VAD_ANA_CON0_EN_ATST_LV_MASK);    //Enable ATST SW to bypass to VIN0_P / VIN1_N for monitor
        ANA_SET_REG(ABB_ANA_CON0, 0<<ABB_ANA_CON0_SEL_ATST0_POS, ABB_ANA_CON0_SEL_ATST0_MASK);      //RG_SEL_ATST0
        ANA_SET_REG(ABB_ANA_CON0, 0<<ABB_ANA_CON0_SEL_ATST1_POS, ABB_ANA_CON0_SEL_ATST1_MASK);      //RG_SEL_ATST1
        #endif

    } else {
        ANA_SET_REG(VAD_ANA_CON5, 0<<VAD_ANA_CON5_OP1_IDAC_LV_POS, VAD_ANA_CON5_OP1_IDAC_LV_MASK|VAD_ANA_CON5_OP2_IDAC_LV_MASK|VAD_ANA_CON5_PRCH_MASK);
        ANA_SET_REG(VAD_ANA_CON0, (0<<VAD_ANA_CON0_CIRCUIT_EN_POS)|(0<<VAD_ANA_CON0_IBAS_DAC_LV_POS), VAD_ANA_CON0_CIRCUIT_EN_MASK|VAD_ANA_CON0_IBAS_DAC_LV_MASK);
    }
#else
    UNUSED(vad_parameter);
    UNUSED(enable);
#endif
    return false;
}

bool hal_audio_ana_set_vad_digital_enable (hal_audio_device_parameter_vad_t *vad_parameter, bool enable)
{
#if (HAL_AUDIO_VAD_DRIVER)
    UNUSED(vad_parameter);
    if (enable) {
        AFE_SET_REG(VAD_CTL3, 1<<VAD_CTL3_RST_MODE_POS, VAD_CTL3_RST_MODE_MASK);//accumulator will reset while beginning of enable

        hal_audio_ana_set_vad_irq_mask(true);       // mask & clear VAD IRQ
        AFE_SET_REG(VAD_CTL0, 1<<VAD_CTL0_EN_VAD_DIG_POS, VAD_CTL0_EN_VAD_DIG_MASK);                // Enable VAD digital circuit


        AFE_SET_REG(VAD_DBG_CTL, 4<<VAD_DBG_CTL_DBG_PORT_SEL_POS, VAD_DBG_CTL_DBG_PORT_SEL_MASK);
        AFE_SET_REG(VAD_DBG_CTL, 1<<VAD_DBG_CTL_DBG_LATCH_POS, VAD_DBG_CTL_DBG_LATCH_MASK);

    } else {
        hal_audio_ana_set_vad_irq_mask(true);       // mask & clear VAD IRQ
        AFE_SET_REG(VAD_CTL0, 0<<VAD_CTL0_EN_VAD_DIG_POS, VAD_CTL0_EN_VAD_DIG_MASK);  // Disable VAD digital circuit
    }

    HAL_AUDIO_LOG_INFO("DSP - Hal Audio VAD Enable:%d ", 1, enable);
#else
    UNUSED(vad_parameter);
    UNUSED(enable);
#endif

    return false;
}

bool hal_audio_ana_set_adc0_enable (hal_audio_device_parameter_adc_t *adc_parameter, afe_analog_control_t analog_control, bool enable)
{
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio ANA ADC:0x%x, Mode:%d, Performance:%d, with_au_in_swap:%d enable:%d", 5, analog_control, adc_parameter->adc_mode, adc_parameter->performance,adc_parameter->with_au_in_swap, enable);
    if (enable) {
        if (analog_control&AFE_ANALOG_COMMON) {
            hal_save_adc_performance_mode(AFE_ANALOG_ADC0,adc_parameter->performance);
            hal_volume_set_analog_mode(AFE_HW_ANALOG_GAIN_INPUT1, adc_parameter->adc_mode);

            //Top
            hal_audio_afe_enable_clksq(true);       //Enable CLKSQ 26MHz
            //ANA_SET_REG(AUDENC_ANA_CON3, 1<<AUDENC_ANA_CON3_UL_CLK_FROM_CLKSQ_POS, AUDENC_ANA_CON3_UL_CLK_FROM_CLKSQ_MASK); //Enable CLK from DL to UL
            hal_audio_ana_enable_ADC_13MCK(AFE_ANALOG_ADC0,enable);
            HAL_AUDIO_DELAY_US(100);

            //Capless LDO
            hal_audio_ana_enable_capless_LDO(enable);
            #if 0
                #if 1
                ANA_SET_REG((AUDENC_ANA_CON27),
                ((1<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_DISCHARGE_PDN_POS)|(1<<AUDENC_ANA_CON27_TBST_EN_POS)|(0<<AUDENC_ANA_CON27_REMOTE_SENSE_EN_POS)|(0<<AUDENC_ANA_CON27_PDD_TEST_POS)),
                (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_DISCHARGE_PDN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK|AUDENC_ANA_CON27_REMOTE_SENSE_EN_MASK|AUDENC_ANA_CON27_PDD_TEST_MASK));
                #else
                ANA_SET_REG((AUDENC_ANA_CON27),
                ((1<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_DISCHARGE_PDN_POS)|(1<<AUDENC_ANA_CON27_TBST_EN_POS)|(1<<AUDENC_ANA_CON27_REMOTE_SENSE_EN_POS)|(0<<AUDENC_ANA_CON27_PDD_TEST_POS)),
                (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_DISCHARGE_PDN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK|AUDENC_ANA_CON27_REMOTE_SENSE_EN_MASK|AUDENC_ANA_CON27_PDD_TEST_MASK));
                #endif
                #if 1//only for ab1568
                ANA_SET_REG(AUDENC_ANA_CON16,3<<AUDENC_ANA_CON16_RG_AUD23SPAREVA25_POS,AUDENC_ANA_CON16_RG_AUD23SPAREVA25_MASK);
                ANA_SET_REG(AUDENC_ANA_CON25,3<<AUDENC_ANA_CON25_RG_AUD45SPAREVA25_POS,AUDENC_ANA_CON25_RG_AUD45SPAREVA25_MASK);
                #endif
            #endif
            #if 0//move to below for ab1568 flow
            //Bias temp
            ANA_SET_REG((AUDENC_ANA_CON39),
                        (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_N_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_N_EN_POS)),
                        (AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_N_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_N_EN_MASK));
            #endif
            //ADC bias
            ANA_SET_REG((AUDENC_ANA_CON2),
                        (((adc_parameter->performance >= AFE_PEROFRMANCE_LOW_POWER_MODE)<<AUDENC_ANA_CON2_HALF_BIAS_EN_POS)|
                         ((adc_parameter->performance >= AFE_PEROFRMANCE_LOW_POWER_MODE)<<AUDENC_ANA_CON2_PREAMP_LP_MODE1_EN_POS)),
                        (AUDENC_ANA_CON2_HALF_BIAS_EN_MASK|
                         AUDENC_ANA_CON2_PREAMP_LP_MODE1_EN_MASK));
            ANA_SET_REG((AUDENC_ANA_CON3),
                        (((adc_parameter->performance == AFE_PEROFRMANCE_SUPER_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON3_PREAMP_LOWPEN1_POS)|
                         ((adc_parameter->performance == AFE_PEROFRMANCE_SUPER_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON3_PREAMP_LOWPEN2_POS)),
                        (AUDENC_ANA_CON3_PREAMP_LOWPEN1_MASK|AUDENC_ANA_CON3_PREAMP_LOWPEN2_MASK));
            ANA_SET_REG((AUDENC_ANA_CON2),
                        ((((adc_parameter->performance!=AFE_PEROFRMANCE_HIGH_MODE)&&(adc_parameter->performance!=AFE_PEROFRMANCE_LOW_POWER_MODE))<<AUDENC_ANA_CON2_LP_1ST_STAGE_EN_POS)|
                         (((adc_parameter->performance!=AFE_PEROFRMANCE_HIGH_MODE)&&(adc_parameter->performance!=AFE_PEROFRMANCE_LOW_POWER_MODE))<<AUDENC_ANA_CON2_LP_2ND_STAGE_EN_POS)|
                         ((adc_parameter->performance != AFE_PEROFRMANCE_HIGH_MODE)<<AUDENC_ANA_CON2_FLASH_LP_EN_POS)),
                        (AUDENC_ANA_CON2_LP_1ST_STAGE_EN_MASK|
                         AUDENC_ANA_CON2_LP_2ND_STAGE_EN_MASK|
                         AUDENC_ANA_CON2_FLASH_LP_EN_MASK));
        }
        //PGA

        #if 0//for ab1568 flow if set 0 DMIC using external bias
        ANA_SET_REG((AUDENC_ANA_CON39),
                    (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS0_DCC_N_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON39_BIAS1_DCC_N_EN_POS)),
                    (AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_N_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_N_EN_MASK));
        #endif
            //ADC bias
        afe_volume_analog_update(AFE_HW_ANALOG_GAIN_INPUT1);
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG((AUDENC_ANA_CON0),
                        (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON0_L_DCC_MODE_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_ACC20K)<<AUDENC_ANA_CON0_L_ACC20K_EN_POS)|(((adc_parameter->with_au_in_swap)? 2 : 1)<<AUDENC_ANA_CON0_L_PREAMP_INPUT_SEL_POS)|(1<<AUDENC_ANA_CON0_L_PREAMP_EN_POS)),
                        (AUDENC_ANA_CON0_L_DCC_MODE_MASK|AUDENC_ANA_CON0_L_ACC20K_EN_MASK|AUDENC_ANA_CON0_L_PREAMP_INPUT_SEL_MASK|AUDENC_ANA_CON0_L_PREAMP_EN_MASK));
            ANA_SET_REG(AUDENC_ANA_CON3, ((adc_parameter->adc_mode!=HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON3_L_PGA_ACCFS_POS), AUDENC_ANA_CON3_L_PGA_ACCFS_MASK);
            ANA_SET_REG(AUDENC_ANA_CON0, ((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON0_L_PREAMP_PRECHARGE_DCC_POS), AUDENC_ANA_CON0_L_PREAMP_PRECHARGE_DCC_MASK);

            if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
                AFE_SET_REG(ABB_CLK_GEN_CFG_2, (0x0103<<ABB_CLK_GEN_CFG_2_DIV_SEL_POS)|(1<<ABB_CLK_GEN_CFG_2_DIV_CHANGE_POS)|(1<<ABB_CLK_GEN_CFG_2_DIV_EN_POS), ABB_CLK_GEN_CFG_2_DIV_SEL_MASK|ABB_CLK_GEN_CFG_2_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_2_DIV_EN_MASK);
            }
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG((AUDENC_ANA_CON1),
                        (((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON1_R_DCC_MODE_POS)|((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_ACC20K)<<AUDENC_ANA_CON1_R_ACC20K_EN_POS)|(((adc_parameter->with_au_in_swap)? 1 : 2)<<AUDENC_ANA_CON1_R_PREAMP_INPUT_SEL_POS)|(1<<AUDENC_ANA_CON1_R_PREAMP_EN_POS)),
                        (AUDENC_ANA_CON1_R_DCC_MODE_MASK|AUDENC_ANA_CON1_R_ACC20K_EN_MASK|AUDENC_ANA_CON1_R_PREAMP_INPUT_SEL_MASK|AUDENC_ANA_CON1_R_PREAMP_EN_MASK));
            ANA_SET_REG(AUDENC_ANA_CON3, ((adc_parameter->adc_mode!=HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON3_R_PGA_ACCFS_POS), AUDENC_ANA_CON3_R_PGA_ACCFS_MASK);
            ANA_SET_REG(AUDENC_ANA_CON1, ((adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC)<<AUDENC_ANA_CON1_R_PREAMP_PRECHARGE_DCC_POS), AUDENC_ANA_CON1_R_PREAMP_PRECHARGE_DCC_MASK);
            if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
                AFE_SET_REG(ABB_CLK_GEN_CFG_3, (0x0103<<ABB_CLK_GEN_CFG_3_DIV_SEL_POS)|(1<<ABB_CLK_GEN_CFG_3_DIV_CHANGE_POS)|(1<<ABB_CLK_GEN_CFG_3_DIV_EN_POS), ABB_CLK_GEN_CFG_3_DIV_SEL_MASK|ABB_CLK_GEN_CFG_3_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_3_DIV_EN_MASK);
            }
        }
        #if 0// single mic on/off
        HAL_AUDIO_DELAY_US(1000);
        ANA_SET_REG(AUDENC_ANA_CON3, (0<<AUDENC_ANA_CON3_L_PGA_ACCFS_POS)|(0<<AUDENC_ANA_CON3_R_PGA_ACCFS_POS), AUDENC_ANA_CON3_L_PGA_ACCFS_MASK|AUDENC_ANA_CON3_R_PGA_ACCFS_MASK); //Disable fast settle
        ANA_SET_REG(AUDENC_ANA_CON0, (0<<AUDENC_ANA_CON0_L_PREAMP_PRECHARGE_DCC_POS), AUDENC_ANA_CON0_L_PREAMP_PRECHARGE_DCC_MASK);
        ANA_SET_REG(AUDENC_ANA_CON1, (0<<AUDENC_ANA_CON1_R_PREAMP_PRECHARGE_DCC_POS), AUDENC_ANA_CON1_R_PREAMP_PRECHARGE_DCC_MASK);
        #endif
        //ADC
        HAL_AUDIO_DELAY_US(1000);
        if (analog_control&AFE_ANALOG_L_CH){
            ANA_SET_REG(AUDENC_ANA_CON3, (0<<AUDENC_ANA_CON3_L_PGA_ACCFS_POS), AUDENC_ANA_CON3_L_PGA_ACCFS_MASK); //Disable fast settle
        }
        if (analog_control&AFE_ANALOG_R_CH){
            ANA_SET_REG(AUDENC_ANA_CON3, (0<<AUDENC_ANA_CON3_R_PGA_ACCFS_POS), AUDENC_ANA_CON3_R_PGA_ACCFS_MASK); //Disable fast settle
        }
        if (analog_control&AFE_ANALOG_COMMON) {
#if 1//single mic on/off move to here
            ANA_SET_REG(AUDENC_ANA_CON0, (0<<AUDENC_ANA_CON0_L_PREAMP_PRECHARGE_DCC_POS), AUDENC_ANA_CON0_L_PREAMP_PRECHARGE_DCC_MASK);
            ANA_SET_REG(AUDENC_ANA_CON1, (0<<AUDENC_ANA_CON1_R_PREAMP_PRECHARGE_DCC_POS), AUDENC_ANA_CON1_R_PREAMP_PRECHARGE_DCC_MASK);
#endif
            ANA_SET_REG((AUDENC_ANA_CON3),
                        ((0<<AUDENC_ANA_CON3_ADC_CLK_SEL_POS)|(0<<AUDENC_ANA_CON3_ADC_CLK_SRC_POS)),
                        (AUDENC_ANA_CON3_ADC_CLK_SEL_MASK|AUDENC_ANA_CON3_ADC_CLK_SRC_MASK));
            ANA_SET_REG(AUDENC_ANA_CON7,  ((adc_parameter->performance == AFE_PEROFRMANCE_HIGH_MODE)<<AUDENC_ANA_CON7_ADC_LP_EN_POS), AUDENC_ANA_CON7_ADC_LP_EN_MASK);
            ANA_SET_REG((AUDENC_ANA_CON3),((adc_parameter->performance >= AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON3_0P25FS_EN_POS), AUDENC_ANA_CON3_0P25FS_EN_MASK);
            ANA_SET_REG(AUDENC_ANA_CON6,  (adc_parameter->performance >= AFE_PEROFRMANCE_ULTRA_LOW_POWER_MODE)<<AUDENC_ANA_CON6_ADC_CLK_RST_POS, AUDENC_ANA_CON6_ADC_CLK_RST_MASK);// reset configure for VOW
            ANA_SET_REG(AUDENC_ANA_CON7,
                       (((adc_parameter->performance==AFE_PEROFRMANCE_NORMAL_MODE)||(adc_parameter->performance==AFE_PEROFRMANCE_LOW_POWER_MODE))<<AUDENC_ANA_CON7_ADC_RST_FOR_LP_POS),
                       AUDENC_ANA_CON7_ADC_RST_FOR_LP_MASK);
        }

        if (analog_control&AFE_ANALOG_COMMON) {
            //For ADC R only clock select to ana ch1
            bool fifo_sync_clock = (!(analog_control&AFE_ANALOG_L_CH)) ? true : adc_parameter->is_fifo_sync_clock_ch1;
            AFE_SET_REG(AFUNC_AUD_CON4, fifo_sync_clock <<AFUNC_AUD_CON4_ANA_CLK_SEL_POS, AFUNC_AUD_CON4_ANA_CLK_SEL_MASK);// 0: ana_ch0 clock is selected, 1: ana_ch1 clock is selected
        }
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON0, (2<<AUDENC_ANA_CON0_L_INPUT_SEL_POS)|(1<<AUDENC_ANA_CON0_L_ADC_EN_POS), AUDENC_ANA_CON0_L_INPUT_SEL_MASK|AUDENC_ANA_CON0_L_ADC_EN_MASK);
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON1, (2<<AUDENC_ANA_CON1_R_INPUT_SEL_POS)|(1<<AUDENC_ANA_CON1_R_ADC_EN_POS), AUDENC_ANA_CON1_R_INPUT_SEL_MASK|AUDENC_ANA_CON1_R_ADC_EN_MASK);
        }


    } else {
        //Disable

        //Workaround: UL FIFO clock soure switch
        if (!(analog_control&AFE_ANALOG_COMMON)) {
            bool ana_clk_sel = (AFE_GET_REG(AFUNC_AUD_CON4)&AFUNC_AUD_CON4_ANA_CLK_SEL_MASK);
            if ((ana_clk_sel && (analog_control&AFE_ANALOG_R_CH) )||
                (!ana_clk_sel&&(analog_control&AFE_ANALOG_L_CH))) {
                HAL_AUDIO_LOG_WARNING("DSP - Warning Hal Audio Device ADC clock select %d :0x%x @@", 2, ana_clk_sel, AFE_GET_REG(AFUNC_AUD_CON4));
                //Disable UL path
                hal_audio_ul_set_enable(HAL_AUDIO_AGENT_DEVICE_ADDA_UL1, false);
                hal_audio_ul_reset_fifo(HAL_AUDIO_AGENT_DEVICE_ADDA_UL1, true);
                //Switch to ch0 while closing ch1
                AFE_SET_REG(AFUNC_AUD_CON4, (!ana_clk_sel)<<AFUNC_AUD_CON4_ANA_CLK_SEL_POS, AFUNC_AUD_CON4_ANA_CLK_SEL_MASK);// 0: ana_ch0 clock is selected, 1: ana_ch1 clock is selected
                //Enable UL path
                hal_audio_ul_reset_fifo(HAL_AUDIO_AGENT_DEVICE_ADDA_UL1, false);
                hal_audio_ul_set_enable(HAL_AUDIO_AGENT_DEVICE_ADDA_UL1, true);
            }
        }

        //Analog ADC
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON0, (0<<AUDENC_ANA_CON0_L_INPUT_SEL_POS)|(0<<AUDENC_ANA_CON0_L_ADC_EN_POS), AUDENC_ANA_CON0_L_INPUT_SEL_MASK|AUDENC_ANA_CON0_L_ADC_EN_MASK);
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON1, (0<<AUDENC_ANA_CON1_R_INPUT_SEL_POS)|(0<<AUDENC_ANA_CON1_R_ADC_EN_POS), AUDENC_ANA_CON1_R_INPUT_SEL_MASK|AUDENC_ANA_CON1_R_ADC_EN_MASK);
        }
        if (analog_control&AFE_ANALOG_COMMON) {
            ANA_SET_REG(AUDENC_ANA_CON3,
                        (0<<AUDENC_ANA_CON3_ADC_CLK_SEL_POS)|(0<<AUDENC_ANA_CON3_ADC_CLK_SRC_POS)|(0<<AUDENC_ANA_CON3_0P25FS_EN_POS),
                        AUDENC_ANA_CON3_ADC_CLK_SEL_MASK|AUDENC_ANA_CON3_ADC_CLK_SRC_MASK|AUDENC_ANA_CON3_0P25FS_EN_MASK);

            ANA_SET_REG(AUDENC_ANA_CON6, (0<<AUDENC_ANA_CON6_ADC_CLK_RST_POS), AUDENC_ANA_CON6_ADC_CLK_RST_MASK);
            ANA_SET_REG(AUDENC_ANA_CON7, (0<<AUDENC_ANA_CON7_ADC_RST_FOR_LP_POS)|(1<<AUDENC_ANA_CON7_ADC_LP_EN_POS), AUDENC_ANA_CON7_ADC_RST_FOR_LP_MASK|AUDENC_ANA_CON7_ADC_LP_EN_MASK);
        }

        //PGA
        if (analog_control&AFE_ANALOG_L_CH) {
            ANA_SET_REG(AUDENC_ANA_CON0, (0<<AUDENC_ANA_CON0_L_PREAMP_INPUT_SEL_POS)|(0<<AUDENC_ANA_CON0_L_PREAMP_EN_POS), AUDENC_ANA_CON0_L_PREAMP_INPUT_SEL_MASK|AUDENC_ANA_CON0_L_PREAMP_EN_MASK);
            if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
                AFE_SET_REG(ABB_CLK_GEN_CFG_2, (0<<ABB_CLK_GEN_CFG_2_DIV_CHANGE_POS)|(0<<ABB_CLK_GEN_CFG_2_DIV_EN_POS), ABB_CLK_GEN_CFG_2_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_2_DIV_EN_MASK);
            }
        }
        if (analog_control&AFE_ANALOG_R_CH) {
            ANA_SET_REG(AUDENC_ANA_CON1, (0<<AUDENC_ANA_CON1_R_PREAMP_INPUT_SEL_POS)|(0<<AUDENC_ANA_CON1_R_PREAMP_EN_POS), AUDENC_ANA_CON1_R_PREAMP_INPUT_SEL_MASK|AUDENC_ANA_CON1_R_PREAMP_EN_MASK);
            if (adc_parameter->adc_mode==HAL_AUDIO_ANALOG_INPUT_DCC) {
                AFE_SET_REG(ABB_CLK_GEN_CFG_3, (0<<ABB_CLK_GEN_CFG_3_DIV_CHANGE_POS)|(0<<ABB_CLK_GEN_CFG_3_DIV_EN_POS), ABB_CLK_GEN_CFG_3_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_3_DIV_EN_MASK);
            }
        }

        if (analog_control&AFE_ANALOG_COMMON) {
            //Bias

            ANA_SET_REG((AUDENC_ANA_CON39),
                        (0),
                        (AUDENC_ANA_CON39_BIAS0_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS0_DCC_N_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P1_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_P2_EN_MASK|AUDENC_ANA_CON39_BIAS1_DCC_N_EN_MASK));

            //LDO
            hal_audio_ana_enable_capless_LDO(enable);
            #if 0
            ANA_SET_REG((AUDENC_ANA_CON27),
                        ((0<<AUDENC_ANA_CON27_LCLDO_ENC_EN_POS)|(0<<AUDENC_ANA_CON27_TBST_EN_POS)),
                        (AUDENC_ANA_CON27_LCLDO_ENC_EN_MASK|AUDENC_ANA_CON27_TBST_EN_MASK));
            #endif
            //TOP
            //ANA_SET_REG(AUDENC_ANA_CON3, 0<<AUDENC_ANA_CON3_UL_CLK_FROM_CLKSQ_POS, AUDENC_ANA_CON3_UL_CLK_FROM_CLKSQ_MASK); //Disable CLK from DL to UL
            hal_audio_ana_enable_ADC_13MCK(AFE_ANALOG_ADC0,enable);
            hal_audio_afe_enable_clksq(false);  //Disable CLKSQ 26MHz
        }
    }
    return false;
}


bool hal_audio_ana_set_dac_enable (hal_audio_device_parameter_dac_t *dac_parameter, afe_analog_control_t analog_control, bool enable)
{
    UNUSED(dac_parameter);
    UNUSED(analog_control);
    UNUSED(enable);
    uint32_t value, l_mask = 0, r_mask = 0;
    bool l_ch = false;
    bool r_ch = false;
    static bool first_boot = true;

    bool with_classg = false;

    if ((dac_parameter->dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSG) ||
        (dac_parameter->dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSAB)) {
            with_classg = true;
    }

    if (analog_control&AFE_ANALOG_L_CH) {
        l_ch = true;
        l_mask = 0xFFFFFFFF;
    }
    if (analog_control&AFE_ANALOG_R_CH){
        r_ch = true;
        r_mask = 0xFFFFFFFF;
    }


    HAL_AUDIO_LOG_INFO("DSP - Hal Audio ANA DAC:0x%x, Mode:%d, Performance:%d, dc_compensation:0x%x, Enable:%d", 5, analog_control, dac_parameter->dac_mode, dac_parameter->with_high_performance, dac_parameter->dc_compensation_value, enable);

    if (enable) {
        hal_save_adc_performance_mode(AFE_ANALOG_DAC,dac_parameter->with_high_performance);
        hal_volume_set_analog_mode(AFE_HW_ANALOG_GAIN_OUTPUT, dac_parameter->dac_mode);
        //diff
        if (!with_classg){
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E021188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E021188, 0xFFFFFFFF);
        }


        //TOP
        hal_audio_dl_set_sdm_enable(true);
        hal_audio_clock_enable_classg(true);    //For ClassD ClassAB
        if (with_classg) {
            if ((dac_parameter->dc_compensation_value) || (!first_boot)) {
                if (dac_parameter->dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSG) {
                    hal_audio_dl_set_classg(true);
                } else {
                    AFE_SET_REG(AFE_CLASSG_CFG5, 0xB, 0xF);
                }
            } else {
                //class off before dc compensation for depop
                AFE_WRITE(AFE_CLASSG_CFG0, 0x11004000);
                first_boot = false;
            }
        }
        //for de pop
        if (with_classg){
            ANA_SET_REG(AUDDEC_ANA_CON3, (1<<AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_POS), AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_MASK);
            //A_SET_REG(AUDDEC_ANA_CON13, (1<<AUDDEC_ANA_CON13_PULL_HIGH_POS),AUDDEC_ANA_CON13_PULL_HIGH_MASK);
        }else{
            hal_audio_dl_set_classd(true);
        }

        hal_audio_afe_enable_clksq(true);       //Enable CLKSQ 26MHz
        //AB1568_ADVT_DL_CLG_Procedures_20210126.xlsx
        if(with_classg && (!dac_parameter->with_high_performance)){
            ANA_WRITE(AUDDEC_ANA_CON13,0x0000000);
            ANA_WRITE(AUDDEC_ANA_CON15,0x0000000);
        }

        //IDAC damage 0730
        ANA_SET_REG(AUDDEC_ANA_CON12, (1<<AUDDEC_ANA_CON12_VCMT_RSV0_R_IDAC_POS), AUDDEC_ANA_CON12_VCMT_RSV0_R_IDAC_MASK);
        ANA_SET_REG(AUDDEC_ANA_CON12, (1<<AUDDEC_ANA_CON12_VCMT_RSV0_L_IDAC_POS), AUDDEC_ANA_CON12_VCMT_RSV0_L_IDAC_MASK);

        ANA_SET_REG(AUDDEC_ANA_CON12, (0<<AUDDEC_ANA_CON12_VCMT_FROM_PAD_POS), AUDDEC_ANA_CON12_VCMT_FROM_PAD_MASK);
        ANA_SET_REG(AUDDEC_ANA_CON13, (0<<AUDDEC_ANA_CON13_HP_VCM_FROM_GLB_POS), AUDDEC_ANA_CON13_HP_VCM_FROM_GLB_MASK);
        ANA_SET_REG(AUDDEC_ANA_CON3, (with_classg<<AUDDEC_ANA_CON3_HP_ESD_RES_EN_POS), AUDDEC_ANA_CON3_HP_ESD_RES_EN_MASK);


        //VCM
        ANA_SET_REG(AUDDEC_ANA_CON10, (3<<AUDDEC_ANA_CON10_2P4V_LDO_VOL_SEL_POS), AUDDEC_ANA_CON10_2P4V_LDO_VOL_SEL_MASK);
        ANA_SET_REG(AUDDEC_ANA_CON12, (0<<AUDDEC_ANA_CON12_VCMT_CLAMPING_POS)|(0<<AUDDEC_ANA_CON12_VCMT_COMPARATOR_POS), AUDDEC_ANA_CON12_VCMT_CLAMPING_MASK|AUDDEC_ANA_CON12_VCMT_COMPARATOR_MASK);
        ANA_SET_REG(AUDDEC_ANA_CON13, (0<<AUDDEC_ANA_CON13_OFF_COMPARATOR_POS), AUDDEC_ANA_CON13_OFF_COMPARATOR_MASK);
        ANA_SET_REG(AUDDEC_ANA_CON12, (1<<AUDDEC_ANA_CON12_VCMT_CMPOUT_POLARITY_POS), AUDDEC_ANA_CON12_VCMT_CMPOUT_POLARITY_MASK);
        //IDAC damage 0730
        //ANA_SET_REG(AUDDEC_ANA_CON12, (1<<AUDDEC_ANA_CON12_VCMT_RSV0_VCMT_CMPOUT_VA18_POS), AUDDEC_ANA_CON12_VCMT_RSV0_VCMT_CMPOUT_VA18_MASK);

        ANA_SET_REG((AUDDEC_ANA_CON12),
                    ((0<<AUDDEC_ANA_CON12_VCMT_CMP_THRE_SEL_POS)|(0<<AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_POS)|(1<<AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_POS)|(0<<AUDDEC_ANA_CON12_VCMT_RDIV_EN_POS)|(0<<AUDDEC_ANA_CON12_VCMT_OP_EN_POS)),
                    (AUDDEC_ANA_CON12_VCMT_CMP_THRE_SEL_MASK|AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_MASK|AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_MASK|AUDDEC_ANA_CON12_VCMT_RDIV_EN_MASK|AUDDEC_ANA_CON12_VCMT_OP_EN_MASK));
        HAL_AUDIO_DELAY_US(50);
        ANA_SET_REG(AUDDEC_ANA_CON12, (1<<AUDDEC_ANA_CON12_VCMT_CLAMPING_POS)|(1<<AUDDEC_ANA_CON12_VCMT_COMPARATOR_POS), AUDDEC_ANA_CON12_VCMT_CLAMPING_MASK|AUDDEC_ANA_CON12_VCMT_COMPARATOR_MASK);
        ANA_SET_REG(AUDDEC_ANA_CON13, (1<<AUDDEC_ANA_CON13_OFF_COMPARATOR_POS), AUDDEC_ANA_CON13_OFF_COMPARATOR_MASK);

        ANA_SET_REG((AUDDEC_ANA_CON12),(0<<AUDDEC_ANA_CON12_VCMT_CMP_THRE_SEL_POS),AUDDEC_ANA_CON12_VCMT_CMP_THRE_SEL_MASK);
        //ANA_SET_REG(AUDDEC_ANA_CON12, (1<<AUDDEC_ANA_CON12_VCMT_CMPOUT_POLARITY_POS), AUDDEC_ANA_CON12_VCMT_CMPOUT_POLARITY_MASK);
        //ANA_SET_REG((AUDDEC_ANA_CON12),
        //            ((0<<AUDDEC_ANA_CON12_VCMT_CMP_THRE_SEL_POS)|(0<<AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_POS)|(0<<AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_POS)|(1<<AUDDEC_ANA_CON12_VCMT_RDIV_EN_POS)|(1<<AUDDEC_ANA_CON12_VCMT_OP_EN_POS)),
        //            (AUDDEC_ANA_CON12_VCMT_CMP_THRE_SEL_MASK|AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_MASK|AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_MASK|AUDDEC_ANA_CON12_VCMT_RDIV_EN_MASK|AUDDEC_ANA_CON12_VCMT_OP_EN_MASK));
        //HAL_AUDIO_DELAY_US(50);
        //ANA_SET_REG(AUDDEC_ANA_CON12, (0<<AUDDEC_ANA_CON12_VCMT_OP_EN_POS), AUDDEC_ANA_CON12_VCMT_OP_EN_MASK);
        HAL_AUDIO_DELAY_US(100);
        ANA_SET_REG((AUDDEC_ANA_CON10),
                    ((1<<AUDDEC_ANA_CON10_2P4VLDO_CORE_EN_POS)|(1<<AUDDEC_ANA_CON10_2P4VLDO_DEC_EN_POS)|(1<<AUDDEC_ANA_CON10_2P4VLDO_DAC_EN_POS)),
                    (AUDDEC_ANA_CON10_2P4VLDO_CORE_EN_MASK|AUDDEC_ANA_CON10_2P4VLDO_DEC_EN_MASK|AUDDEC_ANA_CON10_2P4VLDO_DAC_EN_MASK));
        if(!with_classg){
            ANA_SET_REG((AUDDEC_ANA_CON10),(1<<AUDDEC_ANA_CON10_1P5V_LDO_SEL_POS),AUDDEC_ANA_CON10_1P5V_LDO_SEL_MASK);
        }

        HAL_AUDIO_DELAY_US(100);
        if(with_classg)
        {
            ANA_SET_REG((AUDDEC_ANA_CON11),
                        ((1<<AUDDEC_ANA_CON11_HCLDO_EN_POS)|
                         (1<<AUDDEC_ANA_CON11_LCLDO_EN_POS)|
                         (1<<AUDDEC_ANA_CON11_LCLDO_DAC_EN_POS)|
                         (1<<AUDDEC_ANA_CON11_HCLDO_TBST_EN_POS)|
                         (1<<AUDDEC_ANA_CON11_HCLDO_PDDIS_EN_POS)|
                         (1<<AUDDEC_ANA_CON11_LCLDO_TBST_EN_POS)|
                         (1<<AUDDEC_ANA_CON11_LCLDO_PDDIS_EN_POS)|
                         (1<<AUDDEC_ANA_CON11_LCLDO_DAC_TBST_EN_POS)|
                         (1<<AUDDEC_ANA_CON11_LCLDO_DAC_PDDIS_EN_POS)),
                        (AUDDEC_ANA_CON11_HCLDO_EN_MASK|
                         AUDDEC_ANA_CON11_LCLDO_EN_MASK|
                         AUDDEC_ANA_CON11_LCLDO_DAC_EN_MASK)|
                         AUDDEC_ANA_CON11_HCLDO_TBST_EN_MASK|
                         AUDDEC_ANA_CON11_HCLDO_PDDIS_EN_MASK|
                         AUDDEC_ANA_CON11_LCLDO_TBST_EN_MASK|
                         AUDDEC_ANA_CON11_LCLDO_PDDIS_EN_MASK|
                         AUDDEC_ANA_CON11_LCLDO_DAC_TBST_EN_MASK|
                         AUDDEC_ANA_CON11_LCLDO_DAC_PDDIS_EN_MASK);
        }else{
            #if 1//1005
            ANA_SET_REG((AUDDEC_ANA_CON11),
            ((1<<AUDDEC_ANA_CON11_HCLDO_TBST_EN_POS)|
              (1<<AUDDEC_ANA_CON11_HCLDO_PDDIS_EN_POS)|
              (1<<AUDDEC_ANA_CON11_LCLDO_TBST_EN_POS)|
              (1<<AUDDEC_ANA_CON11_LCLDO_PDDIS_EN_POS)|
              (1<<AUDDEC_ANA_CON11_LCLDO_DAC_TBST_EN_POS)|
              (1<<AUDDEC_ANA_CON11_LCLDO_DAC_PDDIS_EN_POS)|
              (1<<AUDDEC_ANA_CON11_HCLDO_EN_POS)|
              (0<<AUDDEC_ANA_CON11_LCLDO_EN_POS)|
              (1<<AUDDEC_ANA_CON11_LCLDO_DAC_EN_POS)
            ),
            ((AUDDEC_ANA_CON11_HCLDO_TBST_EN_MASK)|
              (AUDDEC_ANA_CON11_HCLDO_PDDIS_EN_MASK)|
              (AUDDEC_ANA_CON11_LCLDO_TBST_EN_MASK)|
              (AUDDEC_ANA_CON11_LCLDO_PDDIS_EN_MASK)|
              (AUDDEC_ANA_CON11_LCLDO_DAC_TBST_EN_MASK)|
              (AUDDEC_ANA_CON11_LCLDO_DAC_PDDIS_EN_MASK)|
              (AUDDEC_ANA_CON11_HCLDO_EN_MASK)|
              (AUDDEC_ANA_CON11_LCLDO_EN_MASK)|
              (AUDDEC_ANA_CON11_LCLDO_DAC_EN_MASK)));
            #else

            ANA_SET_REG((AUDDEC_ANA_CON11),
            ((0<<AUDDEC_ANA_CON11_HCLDO_EN_POS)|
            (1<<AUDDEC_ANA_CON11_LCLDO_EN_POS)|
            (1<<AUDDEC_ANA_CON11_LCLDO_DAC_EN_POS)|
            (1<<AUDDEC_ANA_CON11_HCLDO_TBST_EN_POS)|
            (1<<AUDDEC_ANA_CON11_HCLDO_PDDIS_EN_POS)|
            (1<<AUDDEC_ANA_CON11_LCLDO_TBST_EN_POS)|
            (1<<AUDDEC_ANA_CON11_LCLDO_PDDIS_EN_POS)|
            (1<<AUDDEC_ANA_CON11_LCLDO_DAC_TBST_EN_POS)|
            (1<<AUDDEC_ANA_CON11_LCLDO_DAC_PDDIS_EN_POS)),
            (AUDDEC_ANA_CON11_HCLDO_EN_MASK|
            AUDDEC_ANA_CON11_LCLDO_EN_MASK|
            AUDDEC_ANA_CON11_LCLDO_DAC_EN_MASK)|
            AUDDEC_ANA_CON11_HCLDO_TBST_EN_MASK|
            AUDDEC_ANA_CON11_HCLDO_PDDIS_EN_MASK|
            AUDDEC_ANA_CON11_LCLDO_TBST_EN_MASK|
            AUDDEC_ANA_CON11_LCLDO_PDDIS_EN_MASK|
            AUDDEC_ANA_CON11_LCLDO_DAC_TBST_EN_MASK|
            AUDDEC_ANA_CON11_LCLDO_DAC_PDDIS_EN_MASK);
            #endif

        }
        ANA_SET_REG((AUDDEC_ANA_CON13),
                    ((1<<AUDDEC_ANA_CON13_LCLDO_DAC_EN_POS)|(1<<AUDDEC_ANA_CON13_LCLDO_IBAS_EN_POS)|(1<<AUDDEC_ANA_CON13_HCLDO_IBAS_EN_POS)|(1<<AUDDEC_ANA_CON13_LDO_REF_IBAS_EN_POS)),
                    (AUDDEC_ANA_CON13_LCLDO_DAC_EN_MASK|AUDDEC_ANA_CON13_LCLDO_IBAS_EN_MASK|AUDDEC_ANA_CON13_HCLDO_IBAS_EN_MASK|AUDDEC_ANA_CON13_LDO_REF_IBAS_EN_MASK));
        ANA_SET_REG((AUDDEC_ANA_CON11),
                    ((1<<AUDDEC_ANA_CON11_HCLDO_REMOTE_POS)|(1<<AUDDEC_ANA_CON11_LCLDO_REMOTE_POS)|(1<<AUDDEC_ANA_CON11_LCLDO_DAC_REMOTE_POS)),
                    (AUDDEC_ANA_CON11_HCLDO_REMOTE_MASK|AUDDEC_ANA_CON11_LCLDO_REMOTE_MASK|AUDDEC_ANA_CON11_LCLDO_DAC_REMOTE_MASK));
        HAL_AUDIO_DELAY_US(100);


        //Clock and Bias
        ANA_SET_REG((AUDDEC_ANA_CON9),
                    ((1<<AUDDEC_ANA_CON9_DAC_13M_EN_POS)|(0<<AUDDEC_ANA_CON9_DAC_26M_EN_POS)|((!with_classg)<<AUDDEC_ANA_CON9_CLD_26M_EN_POS)),
                    (AUDDEC_ANA_CON9_DAC_13M_EN_MASK|AUDDEC_ANA_CON9_DAC_26M_EN_MASK|AUDDEC_ANA_CON9_CLD_26M_EN_MASK));
        HAL_AUDIO_DELAY_US(100);
        ANA_SET_REG(AUDDEC_ANA_CON8, (0<<AUDDEC_ANA_CON8_IBIAS_PDN_POS), AUDDEC_ANA_CON8_IBIAS_PDN_MASK);               //Enable IBIST
        ANA_SET_REG(AUDDEC_ANA_CON7, (0X533<<AUDDEC_ANA_CON7_BIAS_ADJ_POS), AUDDEC_ANA_CON7_BIAS_ADJ_MASK);             //Set BUF ibias & CLD ibias & DR bias current optimization(4u/4u/5u)
        ANA_SET_REG(AUDDEC_ANA_CON8, (0xF<<AUDDEC_ANA_CON8_BIAS_ADJ_POS), AUDDEC_ANA_CON8_BIAS_ADJ_MASK);               //Set HP ZCD bias current optimization 2.5uA


        if (with_classg){
            ANA_SET_REG(AUDDEC_ANA_CON13,(0<<AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_POS), AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_MASK); //
        }else{
            //diff
            ANA_SET_REG(AUDDEC_ANA_CON13,(1<<AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_POS), AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_MASK); //
        }


        //ClassG
        if (with_classg) {
            //For ClassAB
            #if 0 //for config mono
            ANA_SET_REG((AUDDEC_ANA_CON16),
                        ((0<<AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_POS)|(0<<AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_POS)),
                        ((AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_MASK&r_mask)));
            #else
            ANA_SET_REG((AUDDEC_ANA_CON16),
                        ((0<<AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_POS)|(0<<AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_POS)),
                        ((AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_MASK)|(AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_MASK)));
            #endif
            //DAC
            ANA_SET_REG(AUDDEC_ANA_CON0, ((!dac_parameter->with_high_performance)<<AUDDEC_ANA_CON0_DAC_LPM_EN_POS), AUDDEC_ANA_CON0_DAC_LPM_EN_MASK);
            //diff
            //AFE_SET_REG(ABB_CLK_GEN_CFG_1, (0x0081<<ABB_CLK_GEN_CFG_1_DIV_SEL_POS)|(1<<ABB_CLK_GEN_CFG_1_DIV_CHANGE_POS)|(1<<ABB_CLK_GEN_CFG_1_DIV_EN_POS), ABB_CLK_GEN_CFG_1_DIV_SEL_MASK|ABB_CLK_GEN_CFG_1_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_1_DIV_EN_MASK);
            AFE_SET_REG(ABB_CLK_GEN_CFG_1, (0x0021<<ABB_CLK_GEN_CFG_1_DIV_SEL_POS)|(1<<ABB_CLK_GEN_CFG_1_DIV_CHANGE_POS)|(1<<ABB_CLK_GEN_CFG_1_DIV_EN_POS), ABB_CLK_GEN_CFG_1_DIV_SEL_MASK|ABB_CLK_GEN_CFG_1_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_1_DIV_EN_MASK);
            //------config mono

            HAL_AUDIO_DELAY_US(60);
            ANA_SET_REG((AUDDEC_ANA_CON0),
                        ((dac_parameter->with_high_performance<<AUDDEC_ANA_CON0_DAC_CHOP_EN_POS)|(1<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_POS)|(dac_parameter->with_high_performance<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_POS)),
                        (AUDDEC_ANA_CON0_DAC_CHOP_EN_MASK|AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_MASK|AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_MASK));
            ANA_SET_REG(AUDDEC_ANA_CON15, (dac_parameter->with_high_performance<<AUDDEC_ANA_CON15_DAC_CURRENT_2X_POS), AUDDEC_ANA_CON15_DAC_CURRENT_2X_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON0),
                        ((1<<AUDDEC_ANA_CON0_DAC_L_PWRUP_POS)|
                         (1<<AUDDEC_ANA_CON0_DAC_R_PWRUP_POS)|
                         (1<<AUDDEC_ANA_CON0_DAC_L_BIAS_EN_POS)|
                         (1<<AUDDEC_ANA_CON0_DAC_R_BIAS_EN_POS)),
                        (((AUDDEC_ANA_CON0_DAC_L_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_L_BIAS_EN_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON0_DAC_R_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_R_BIAS_EN_MASK)&r_mask)));
            //IDAC damage 0730
            ANA_SET_REG(AUDDEC_ANA_CON12, (0<<AUDDEC_ANA_CON12_VCMT_RSV0_R_IDAC_POS), AUDDEC_ANA_CON12_VCMT_RSV0_R_IDAC_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON12, (0<<AUDDEC_ANA_CON12_VCMT_RSV0_L_IDAC_POS), AUDDEC_ANA_CON12_VCMT_RSV0_L_IDAC_MASK);
            HAL_AUDIO_DELAY_US(100);

            //Here Set Analog - Vcm_Tracker (SH)
            ANA_SET_REG(AUDDEC_ANA_CON12, (0<<AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_POS), AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON12, (0<<AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_POS), AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON12, (1<<AUDDEC_ANA_CON12_VCMT_RDIV_EN_POS), AUDDEC_ANA_CON12_VCMT_RDIV_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON12, (1<<AUDDEC_ANA_CON12_VCMT_OP_EN_POS), AUDDEC_ANA_CON12_VCMT_OP_EN_MASK);
            HAL_AUDIO_DELAY_US(50);
            ANA_SET_REG(AUDDEC_ANA_CON12, (0<<AUDDEC_ANA_CON12_VCMT_OP_EN_POS), AUDDEC_ANA_CON12_VCMT_OP_EN_MASK);
            HAL_AUDIO_DELAY_US(1000);
            //AB1568_ADVT_DL_CLG_Procedures_20210126.xlsx
            if(with_classg && (!dac_parameter->with_high_performance)){
                ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_STB_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_STB_EN_POS), (AUDDEC_ANA_CON2_HPL_STB_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_STB_EN_MASK&r_mask));
                ANA_SET_REG(AUDDEC_ANA_CON11, (1<<AUDDEC_ANA_CON11_HPL_STBEHN_EN_POS)|(1<<AUDDEC_ANA_CON11_HPR_STBEHN_EN_POS), (AUDDEC_ANA_CON11_HPL_STBEHN_EN_MASK&l_mask)|(AUDDEC_ANA_CON11_HPR_STBEHN_EN_MASK&r_mask));
                ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), (AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK&r_mask));
                ANA_SET_REG(AUDDEC_ANA_CON11, (1<<AUDDEC_ANA_CON11_LCLDO_EN_POS), AUDDEC_ANA_CON11_LCLDO_EN_MASK);
            }

            //Here Set Analog - CLASSG Buffer (Pin-Wen)
            AFE_SET_REG(ZCD_CON0, 0<<ZCD_CON0_ZCD_EN_POS, ZCD_CON0_ZCD_EN_MASK);

            ANA_SET_REG(AUDDEC_ANA_CON3, (1<<AUDDEC_ANA_CON3_HP_TRIM_EN_POS),AUDDEC_ANA_CON3_HP_TRIM_EN_MASK);

            ANA_SET_REG((AUDDEC_ANA_CON4),
                        (dac_parameter->dc_compensation_value
                       /*(0<<AUDDEC_ANA_CON4_HPL_TRIM_VOL_SEL_POS)|
                         (0<<AUDDEC_ANA_CON4_HPL_FINE_VOL_SEL_POS)|
                         (0<<AUDDEC_ANA_CON4_HPR_TRIM_VOL_SEL_POS)|
                         (0<<AUDDEC_ANA_CON4_HPR_FINE_VOL_SEL_POS)*/),
                        (((AUDDEC_ANA_CON4_HPL_TRIM_VOL_SEL_MASK|AUDDEC_ANA_CON4_HPL_FINE_VOL_SEL_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON4_HPR_TRIM_VOL_SEL_MASK|AUDDEC_ANA_CON4_HPR_FINE_VOL_SEL_MASK)&r_mask)));

            //add for LPM
            ANA_SET_REG(AUDDEC_ANA_CON5, (0<<AUDDEC_ANA_CON5_RG_AUDHPCM3_EN_VA33_POS), AUDDEC_ANA_CON5_RG_AUDHPCM3_EN_VA33_MASK);
            //add for LPM
            ANA_SET_REG((AUDDEC_ANA_CON0),0x3<<AUDDEC_ANA_CON0_RG_AUDDAC_CLD_I_VA33_POS,AUDDEC_ANA_CON0_RG_AUDDAC_CLD_I_VA33_MASK);

            #if 0 //for config mono
            ANA_SET_REG(AUDDEC_ANA_CON1, (1<<AUDDEC_ANA_CON1_HPL_SCP_DISEN_POS)|(1<<AUDDEC_ANA_CON1_HPR_SCP_DISEN_POS), (AUDDEC_ANA_CON1_HPL_SCP_DISEN_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_SCP_DISEN_MASK&r_mask));
            #else
            ANA_SET_REG(AUDDEC_ANA_CON1, (1<<AUDDEC_ANA_CON1_HPL_SCP_DISEN_POS)|(1<<AUDDEC_ANA_CON1_HPR_SCP_DISEN_POS), (AUDDEC_ANA_CON1_HPL_SCP_DISEN_MASK)|(AUDDEC_ANA_CON1_HPR_SCP_DISEN_MASK));
            #endif
            HAL_AUDIO_LOG_INFO("0 AUDDEC_ANA_CON1:0x%x,val:0x%x\r\n",2,AUDDEC_ANA_CON1,ANA_READ(AUDDEC_ANA_CON1));
            ANA_SET_REG(AUDDEC_ANA_CON8, (0<<AUDDEC_ANA_CON8_IBIAS_PDN_POS), AUDDEC_ANA_CON8_IBIAS_PDN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON7, (0X102<<AUDDEC_ANA_CON7_BIAS_ADJ_POS), AUDDEC_ANA_CON7_BIAS_ADJ_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON13,(0<<AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_POS)|(1<<AUDDEC_ANA_CON13_HP_COM_CAP_SHORT_POS), AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_MASK|AUDDEC_ANA_CON13_HP_COM_CAP_SHORT_MASK);
            #if 0
            ANA_SET_REG(AUDDEC_ANA_CON5, (1<<AUDDEC_ANA_CON5_HP_TRACK_EN_POS)|(3<<AUDDEC_ANA_CON5_HP_TRACK_LEVEL_POS), AUDDEC_ANA_CON5_HP_TRACK_EN_MASK|AUDDEC_ANA_CON5_HP_TRACK_LEVEL_MASK);
            #else//add for Lowpower from SE
            ANA_SET_REG(AUDDEC_ANA_CON5, (1<<AUDDEC_ANA_CON5_RG_AUDHPCM3_EN_VA33_POS)|(3<<AUDDEC_ANA_CON5_HP_TRACK_LEVEL_POS), AUDDEC_ANA_CON5_RG_AUDHPCM3_EN_VA33_MASK|AUDDEC_ANA_CON5_HP_TRACK_LEVEL_MASK);
            #endif

            ANA_SET_REG(AUDDEC_ANA_CON8, (0<<AUDDEC_ANA_CON8_BIAS_ADJ_POS), AUDDEC_ANA_CON8_BIAS_ADJ_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON11, (1<<AUDDEC_ANA_CON11_HPL_STBEHN_EN_POS)|(1<<AUDDEC_ANA_CON11_HPR_STBEHN_EN_POS), AUDDEC_ANA_CON11_HPL_STBEHN_EN_MASK|AUDDEC_ANA_CON11_HPR_STBEHN_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON13, (0<<AUDDEC_ANA_CON13_PULL_HIGH_POS)|(1<<AUDDEC_ANA_CON13_GM2_BOOST_PDN_POS), AUDDEC_ANA_CON13_PULL_HIGH_MASK|AUDDEC_ANA_CON13_GM2_BOOST_PDN_MASK);
            if (dac_parameter->with_high_performance) {
                ANA_SET_REG(AUDDEC_ANA_CON5, (3<<AUDDEC_ANA_CON5_HP_BIAS_SEL_POS), AUDDEC_ANA_CON5_HP_BIAS_SEL_MASK);
            } else {
                #if 0
                ANA_SET_REG(AUDDEC_ANA_CON5, (0<<AUDDEC_ANA_CON5_HP_BIAS_SEL_POS)|(0<<AUDDEC_ANA_CON5_HP_1ST_CAP_SEL_POS), AUDDEC_ANA_CON5_HP_BIAS_SEL_MASK|AUDDEC_ANA_CON5_HP_1ST_CAP_SEL_MASK);
                #else //add for Lowpower from SE
                ANA_SET_REG(AUDDEC_ANA_CON5, (0<<AUDDEC_ANA_CON5_HP_BIAS_SEL_POS), AUDDEC_ANA_CON5_HP_BIAS_SEL_MASK);
                #endif
            }

            AFE_SET_REG(ZCD_CON2, 0x00000B2C, ZCD_CON2_L_GAIN_MASK|ZCD_CON2_R_GAIN_MASK);



            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E021188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E021188, 0xFFFFFFFF);
            ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_STB_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_STB_EN_POS), (AUDDEC_ANA_CON2_HPL_STB_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_STB_EN_MASK&r_mask));

            #if 0 //for config mono
            ANA_SET_REG((AUDDEC_ANA_CON3),
                        ((0<<AUDDEC_ANA_CON3_HPL_AUX_GAIN_POS)|(0<<AUDDEC_ANA_CON3_HPR_AUX_GAIN_POS)),
                        ((AUDDEC_ANA_CON3_HPL_AUX_GAIN_MASK&l_mask)|(AUDDEC_ANA_CON3_HPR_AUX_GAIN_MASK&r_mask)));
            #else
            ANA_SET_REG((AUDDEC_ANA_CON3),
                        ((0<<AUDDEC_ANA_CON3_HPL_AUX_GAIN_POS)|(0<<AUDDEC_ANA_CON3_HPR_AUX_GAIN_POS)),
                        ((AUDDEC_ANA_CON3_HPL_AUX_GAIN_MASK)|(AUDDEC_ANA_CON3_HPR_AUX_GAIN_MASK)));
            #endif

            ANA_SET_REG((AUDDEC_ANA_CON2),
                        ((1<<AUDDEC_ANA_CON2_HPL_AUX_FB_POS)|
                         (1<<AUDDEC_ANA_CON2_HPR_AUX_FB_POS)|
                         (1<<AUDDEC_ANA_CON2_HPL_AUX_CMFB_EN_POS)|
                         (1<<AUDDEC_ANA_CON2_HPR_AUX_CMFB_EN_POS)),
                        (((AUDDEC_ANA_CON2_HPL_AUX_FB_MASK|AUDDEC_ANA_CON2_HPL_AUX_CMFB_EN_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON2_HPR_AUX_FB_MASK|AUDDEC_ANA_CON2_HPR_AUX_CMFB_EN_MASK)&r_mask)));

            ANA_SET_REG((AUDDEC_ANA_CON1),
                        ((1<<AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_POS)|
                         (1<<AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_POS)|
                         (1<<AUDDEC_ANA_CON1_HPL_BIAS_EN_POS)|
                         (1<<AUDDEC_ANA_CON1_HPR_BIAS_EN_POS)|
                         (1<<AUDDEC_ANA_CON1_HPL_PWRUP_EN_POS)|
                         (1<<AUDDEC_ANA_CON1_HPR_PWRUP_EN_POS)),
                        (((AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_MASK|AUDDEC_ANA_CON1_HPL_BIAS_EN_MASK|AUDDEC_ANA_CON1_HPL_PWRUP_EN_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_MASK|AUDDEC_ANA_CON1_HPR_BIAS_EN_MASK|AUDDEC_ANA_CON1_HPR_PWRUP_EN_MASK)&r_mask)));

            HAL_AUDIO_DELAY_US(600);//
            ANA_SET_REG((AUDDEC_ANA_CON2),
                        ((1<<AUDDEC_ANA_CON2_HPL_SHORT_EN_POS)|
                         (1<<AUDDEC_ANA_CON2_HPR_SHORT_EN_POS)|
                         (1<<AUDDEC_ANA_CON2_HPL_MAIN_CMFB_EN_POS)|
                         (1<<AUDDEC_ANA_CON2_HPR_MAIN_CMFB_EN_POS)|
                         (0<<AUDDEC_ANA_CON2_HPL_AUX_CMFB_EN_POS)|
                         (0<<AUDDEC_ANA_CON2_HPR_AUX_CMFB_EN_POS)),
                        (((AUDDEC_ANA_CON2_HPL_SHORT_EN_MASK|AUDDEC_ANA_CON2_HPL_MAIN_CMFB_EN_MASK|AUDDEC_ANA_CON2_HPL_AUX_CMFB_EN_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON2_HPR_SHORT_EN_MASK|AUDDEC_ANA_CON2_HPR_MAIN_CMFB_EN_MASK|AUDDEC_ANA_CON2_HPR_AUX_CMFB_EN_MASK)&r_mask)));

            ANA_SET_REG(AUDDEC_ANA_CON1, (1<<AUDDEC_ANA_CON1_HPL_MAIN_STAGE_EN_POS)|(1<<AUDDEC_ANA_CON1_HPR_MAIN_STAGE_EN_POS), (AUDDEC_ANA_CON1_HPL_MAIN_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_MAIN_STAGE_EN_MASK&r_mask));

            HAL_AUDIO_DELAY_US(600);
            for (value=1 ; value<=7 ; value++) {
                AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, value<<AFE_CLASSG_LPSLCH_CFG0_STG_CTRL_POS, AFE_CLASSG_LPSLCH_CFG0_STG_CTRL_MASK);
                AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, value<<AFE_CLASSG_LPSRCH_CFG0_STG_CTRL_POS, AFE_CLASSG_LPSRCH_CFG0_STG_CTRL_MASK);
                HAL_AUDIO_DELAY_US(600);
            }

            for (value=1 ; value<=15 ; value++) {
                ANA_SET_REG(AUDDEC_ANA_CON3, (value<<AUDDEC_ANA_CON3_HPL_AUX_GAIN_POS)|(value<<AUDDEC_ANA_CON3_HPR_AUX_GAIN_POS), (AUDDEC_ANA_CON3_HPL_AUX_GAIN_MASK&l_mask)|(AUDDEC_ANA_CON3_HPR_AUX_GAIN_MASK&r_mask));
                HAL_AUDIO_DELAY_US(600);
            }
            ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPL_AUX_FB_POS)|(0<<AUDDEC_ANA_CON2_HPR_AUX_FB_POS), (AUDDEC_ANA_CON2_HPL_AUX_FB_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_AUX_FB_MASK&r_mask));

            //HP gain ramp up from -32dB to target gain, 1dB/step
            //Increase HPR/HPL gain to normal gain step by step
            afe_volume_analog_ramp_output(afe_volume_analog_get_target_register_value(AFE_HW_ANALOG_GAIN_OUTPUT));

            ANA_SET_REG(AUDDEC_ANA_CON1, (0<<AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_POS)|(0<<AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_POS), (AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_MASK&r_mask));

            ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPL_SHORT_EN_POS)|(0<<AUDDEC_ANA_CON2_HPR_SHORT_EN_POS), ((AUDDEC_ANA_CON2_HPL_SHORT_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_SHORT_EN_MASK&r_mask)));
            //AB1568_ADVT_DL_CLG_Procedures_20210126.xlsx
            if(with_classg && (!dac_parameter->with_high_performance)){
               ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS)|(0<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), ((AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK&r_mask)));
            }

            ANA_SET_REG(AUDDEC_ANA_CON11, (0<<AUDDEC_ANA_CON11_HPL_STBEHN_EN_POS)|(0<<AUDDEC_ANA_CON11_HPR_STBEHN_EN_POS), AUDDEC_ANA_CON11_HPL_STBEHN_EN_MASK|AUDDEC_ANA_CON11_HPR_STBEHN_EN_MASK);

            ANA_SET_REG(AUDDEC_ANA_CON1, (1<<AUDDEC_ANA_CON1_HPL_INPUT_SEL_POS)|(1<<AUDDEC_ANA_CON1_HPR_INPUT_SEL_POS), (AUDDEC_ANA_CON1_HPL_INPUT_SEL_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_INPUT_SEL_MASK&r_mask));

            ANA_SET_REG(AUDDEC_ANA_CON3, (0<<AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_POS), AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_MASK);

            //Not necessary to enable ClassG in DC compensation flow
            /*
            if(!(dac_parameter->dc_compensation_value)){
                //class on for dc compensation depop
                 AFE_WRITE(AFE_CLASSG_CFG0, 0x11004001);
                 printf("AFE_CLASSG_CFG0 val:0x%x\r\n",AFE_READ(AFE_CLASSG_CFG0));
            }
            */
        } else {
            //For ClassD On
            ANA_SET_REG(AUDDEC_ANA_CON1, (0<<AUDDEC_ANA_CON1_HPL_INPUT_SEL_POS)|(0<<AUDDEC_ANA_CON1_HPR_INPUT_SEL_POS), (AUDDEC_ANA_CON1_HPL_INPUT_SEL_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_INPUT_SEL_MASK&r_mask));
            ANA_SET_REG(AUDDEC_ANA_CON13,(0<<AUDDEC_ANA_CON13_HP_COM_CAP_SHORT_POS)|(1<<AUDDEC_ANA_CON13_PULL_HIGH_POS), AUDDEC_ANA_CON13_HP_COM_CAP_SHORT_MASK|AUDDEC_ANA_CON13_PULL_HIGH_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPL_AUX_FB_POS)|(0<<AUDDEC_ANA_CON2_HPR_AUX_FB_POS), (AUDDEC_ANA_CON2_HPL_AUX_FB_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_AUX_FB_MASK&r_mask));
            hal_gain_set_analog_output_class_ab(0);
#if 1//move to here from below depop 7010
            //DAC
            ANA_SET_REG((AUDDEC_ANA_CON0),
                       ((1<<AUDDEC_ANA_CON0_DAC_LPM_EN_POS)|(0<<AUDDEC_ANA_CON0_DAC_CHOP_EN_POS)|(0<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_POS)|(0<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_POS)),
                       (AUDDEC_ANA_CON0_DAC_LPM_EN_MASK|AUDDEC_ANA_CON0_DAC_CHOP_EN_MASK|AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_MASK|AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_MASK));
            //0826
            ANA_SET_REG(AUDDEC_ANA_CON15, (0<<AUDDEC_ANA_CON15_DAC_CURRENT_2X_POS)|(1<<AUDDEC_ANA_CON15_SET_LCH_INPUT_PULL_HIGH_POS), AUDDEC_ANA_CON15_DAC_CURRENT_2X_MASK|AUDDEC_ANA_CON15_SET_LCH_INPUT_PULL_HIGH_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON0),
                       ((1<<AUDDEC_ANA_CON0_DAC_L_PWRUP_POS)|(1<<AUDDEC_ANA_CON0_DAC_R_PWRUP_POS)|(1<<AUDDEC_ANA_CON0_DAC_L_BIAS_EN_POS)|(1<<AUDDEC_ANA_CON0_DAC_R_BIAS_EN_POS)),
                       (((AUDDEC_ANA_CON0_DAC_L_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_L_BIAS_EN_MASK)&l_mask)|((AUDDEC_ANA_CON0_DAC_R_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_R_BIAS_EN_MASK)&r_mask)));
            ANA_SET_REG((AUDDEC_ANA_CON12),(0<<AUDDEC_ANA_CON12_VCMT_RSV0_R_IDAC_POS)|(0<<AUDDEC_ANA_CON12_VCMT_RSV0_L_IDAC_POS),AUDDEC_ANA_CON12_VCMT_RSV0_R_IDAC_MASK|AUDDEC_ANA_CON12_VCMT_RSV0_L_IDAC_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON12),(0<<AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_POS)|(0<<AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_POS)|(1<<AUDDEC_ANA_CON12_VCMT_RDIV_EN_POS)|(1<<AUDDEC_ANA_CON12_VCMT_OP_EN_POS),
                AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_MASK|AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_MASK|AUDDEC_ANA_CON12_VCMT_RDIV_EN_MASK|AUDDEC_ANA_CON12_VCMT_OP_EN_MASK);
            HAL_AUDIO_DELAY_US(50);
            HAL_AUDIO_DELAY_US(100);
            ANA_SET_REG((AUDDEC_ANA_CON12),(0<<AUDDEC_ANA_CON12_VCMT_OP_EN_POS),
                AUDDEC_ANA_CON12_VCMT_OP_EN_MASK);
#endif
            HAL_AUDIO_DELAY_US(1000);
            //0826
            AFE_SET_REG(AUDDEC_ANA_CON11, (1<<AUDDEC_ANA_CON11_LCLDO_EN_POS), AUDDEC_ANA_CON11_LCLDO_EN_MASK);

            AFE_SET_REG(AUDDEC_ANA_CON24, (0<<AUDDEC_ANA_CON24_CLD_RST_POS), AUDDEC_ANA_CON24_CLD_RST_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON16),
                       ((0<<AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_POS)|(0<<AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_POS)),
                       ((AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_MASK&r_mask)));
            //class-d depop
            //ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPR_STB_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPR_STB_EN_MASK|AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK);
            //ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPR_STB_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS)|(0<<AUDDEC_ANA_CON2_HPL_STB_EN_POS)|(0<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPR_STB_EN_MASK|AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK|AUDDEC_ANA_CON2_HPL_STB_EN_MASK|AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK);
            //ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPR_STB_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS)|(1<<AUDDEC_ANA_CON2_HPL_STB_EN_POS)|(1<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPR_STB_EN_MASK|AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK|AUDDEC_ANA_CON2_HPL_STB_EN_MASK|AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK);
            HAL_AUDIO_DELAY_US(10);
            afe_volume_analog_update(AFE_HW_ANALOG_GAIN_OUTPUT);
            //ANA_SET_REG((AUDDEC_ANA_CON18),(4<<AUDDEC_ANA_CON18_HPL_AUX_GAIN_POS)|(4<<AUDDEC_ANA_CON18_HPR_AUX_GAIN_POS),
                //(AUDDEC_ANA_CON18_HPL_AUX_GAIN_MASK)|(AUDDEC_ANA_CON18_HPR_AUX_GAIN_MASK));
            //ANA_SET_REG((AUDDEC_ANA_CON23),(7<<AUDDEC_ANA_CON23_CLD_CAP_GAIN_POS),
                //AUDDEC_ANA_CON23_CLD_CAP_GAIN_MASK);
            //ANA_SET_REG((AUDDEC_ANA_CON24),(1<<AUDDEC_ANA_CON24_CLD_RCH_TRIM_POS),
                //(AUDDEC_ANA_CON24_CLD_RCH_TRIM_MASK));
            //ANA_SET_REG((AUDDEC_ANA_CON25),(4<<AUDDEC_ANA_CON25_CLD_HPL_GAIN_POS)|(4<<AUDDEC_ANA_CON25_CLD_HPR_GAIN_POS),
                //(AUDDEC_ANA_CON25_CLD_HPL_GAIN_MASK|AUDDEC_ANA_CON25_CLD_HPR_GAIN_MASK));
            //1005
            ANA_SET_REG(AUDDEC_ANA_CON20,(0x0<<AUDDEC_ANA_CON20_HP_DIFF_IBIAS_POS), AUDDEC_ANA_CON20_HP_DIFF_IBIAS_MASK);
            #if 0
            ANA_SET_REG((AUDDEC_ANA_CON21),
                        ((1<<AUDDEC_ANA_CON21_CLD_BIAS_EN_POS)|
                         (1<<AUDDEC_ANA_CON21_CLD_CMP_EN_POS)|
                         (1<<AUDDEC_ANA_CON21_CLD_LF_EN_POS)|
                         (1<<AUDDEC_ANA_CON21_CLD_NONOVER_EN_POS)|
                         (1<<AUDDEC_ANA_CON21_CLD_SAWGEN_EN_POS)|
                         (1<<AUDDEC_ANA_CON21_CLD_VCMINT_EN_POS)|
                         (1<<AUDDEC_ANA_CON21_CLD_VCMRED_EN_POS)|
                         (1<<AUDDEC_ANA_CON21_CLD_VCM_SEL_POS)|
                         (3<<AUDDEC_ANA_CON21_CLD_DZ_SEL_POS)|
                         (0<<AUDDEC_ANA_CON21_CLD_SR_SEL_POS)),
                        (AUDDEC_ANA_CON21_CLD_BIAS_EN_MASK|
                         AUDDEC_ANA_CON21_CLD_CMP_EN_MASK|
                         AUDDEC_ANA_CON21_CLD_LF_EN_MASK|
                         AUDDEC_ANA_CON21_CLD_NONOVER_EN_MASK|
                         AUDDEC_ANA_CON21_CLD_SAWGEN_EN_MASK|
                         AUDDEC_ANA_CON21_CLD_VCMINT_EN_MASK|
                         AUDDEC_ANA_CON21_CLD_VCMRED_EN_MASK|
                         AUDDEC_ANA_CON21_CLD_VCM_SEL_MASK|
                         AUDDEC_ANA_CON21_CLD_DZ_SEL_MASK|
                         AUDDEC_ANA_CON21_CLD_SR_SEL_MASK));
			#else
            ANA_SET_REG((AUDDEC_ANA_CON21),
            ((1<<AUDDEC_ANA_CON21_CLD_BIAS_EN_POS)|
             (1<<AUDDEC_ANA_CON21_CLD_CMP_EN_POS)|
             (1<<AUDDEC_ANA_CON21_CLD_LF_EN_POS)|
             (1<<AUDDEC_ANA_CON21_CLD_NONOVER_EN_POS)|
             (1<<AUDDEC_ANA_CON21_CLD_SAWGEN_EN_POS)|
             (1<<AUDDEC_ANA_CON21_CLD_VCMINT_EN_POS)|
             (1<<AUDDEC_ANA_CON21_CLD_VCMRED_EN_POS)|
             (1<<AUDDEC_ANA_CON21_CLD_VCM_SEL_POS)|
             (3<<AUDDEC_ANA_CON21_CLD_DZ_SEL_POS)|
             (3<<AUDDEC_ANA_CON21_CLD_SR_SEL_POS)),
            (AUDDEC_ANA_CON21_CLD_BIAS_EN_MASK|
             AUDDEC_ANA_CON21_CLD_CMP_EN_MASK|
             AUDDEC_ANA_CON21_CLD_LF_EN_MASK|
             AUDDEC_ANA_CON21_CLD_NONOVER_EN_MASK|
             AUDDEC_ANA_CON21_CLD_SAWGEN_EN_MASK|
             AUDDEC_ANA_CON21_CLD_VCMINT_EN_MASK|
             AUDDEC_ANA_CON21_CLD_VCMRED_EN_MASK|
             AUDDEC_ANA_CON21_CLD_VCM_SEL_MASK|
             AUDDEC_ANA_CON21_CLD_DZ_SEL_MASK|
             AUDDEC_ANA_CON21_CLD_SR_SEL_MASK));
            #endif
            //class-d depop
            HAL_AUDIO_DELAY_US(10);
            #if 0
            ANA_SET_REG((AUDDEC_ANA_CON22),
                        ((2<<AUDDEC_ANA_CON22_CLD_LF1_SEL_POS)|
                         (3<<AUDDEC_ANA_CON22_CLD_LF2_SEL_POS)|
                         (3<<AUDDEC_ANA_CON22_CLD_SAWOP_SEL_POS)|
                         (3<<AUDDEC_ANA_CON22_CLD_VCMINT_SEL_POS)|
                         (3<<AUDDEC_ANA_CON22_CLD_VCMRED_SEL_POS)|
                         (5<<AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_POS)),
                        (AUDDEC_ANA_CON22_CLD_LF1_SEL_MASK|
                         AUDDEC_ANA_CON22_CLD_LF2_SEL_MASK|
                         AUDDEC_ANA_CON22_CLD_SAWOP_SEL_MASK|
                         AUDDEC_ANA_CON22_CLD_VCMINT_SEL_MASK|
                         AUDDEC_ANA_CON22_CLD_VCMRED_SEL_MASK|
                         AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_MASK));
			HAL_AUDIO_DELAY_US(10);
            ANA_SET_REG((AUDDEC_ANA_CON22),(3<<AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_POS),
                (AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_MASK));
            HAL_AUDIO_DELAY_US(10);
            #else
            #if 0//SE request to set low current
            ANA_SET_REG((AUDDEC_ANA_CON22),
            ((1<<AUDDEC_ANA_CON22_CLD_LF1_SEL_POS)|
             (3<<AUDDEC_ANA_CON22_CLD_LF2_SEL_POS)|
             (3<<AUDDEC_ANA_CON22_CLD_SAWOP_SEL_POS)|
             (3<<AUDDEC_ANA_CON22_CLD_VCMINT_SEL_POS)|
             //((2+dac_parameter->with_high_performance)<<AUDDEC_ANA_CON22_CLD_VCMRED_SEL_POS)|
             ((3)<<AUDDEC_ANA_CON22_CLD_VCMRED_SEL_POS)|
             (0<<AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_POS)|
             (4<<AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_POS)),
            (AUDDEC_ANA_CON22_CLD_LF1_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_LF2_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_SAWOP_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_VCMINT_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_VCMRED_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_MASK));

            #else
            ANA_SET_REG((AUDDEC_ANA_CON22),
            ((1<<AUDDEC_ANA_CON22_CLD_LF1_SEL_POS)|
             (3<<AUDDEC_ANA_CON22_CLD_LF2_SEL_POS)|
             (3<<AUDDEC_ANA_CON22_CLD_SAWOP_SEL_POS)|
             (3<<AUDDEC_ANA_CON22_CLD_VCMINT_SEL_POS)|
             //((2+dac_parameter->with_high_performance)<<AUDDEC_ANA_CON22_CLD_VCMRED_SEL_POS)|
             ((3)<<AUDDEC_ANA_CON22_CLD_VCMRED_SEL_POS)|
             (0<<AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_POS)|
             (3<<AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_POS)),
            (AUDDEC_ANA_CON22_CLD_LF1_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_LF2_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_SAWOP_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_VCMINT_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_VCMRED_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_MASK|
             AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_MASK));
            #endif
            #endif
            #if 1//diff
            ANA_SET_REG((AUDDEC_ANA_CON23),
                        ((2<<AUDDEC_ANA_CON23_CLD_SAW_FREQ_SEL_POS)|
                         (1<<AUDDEC_ANA_CON23_CLD_CHOP_EN_POS)|
                         (1<<AUDDEC_ANA_CON23_CLD_CAPTRIM_SEL_POS)),
                        (AUDDEC_ANA_CON23_CLD_SAW_FREQ_SEL_MASK|
                         AUDDEC_ANA_CON23_CLD_CHOP_EN_MASK|
                         AUDDEC_ANA_CON23_CLD_CAPTRIM_SEL_MASK));
            //SE  request to fix cap trim
            //ANA_SET_REG(AUDDEC_ANA_CON23, dac_parameter->dc_compensation_value<<AUDDEC_ANA_CON23_CLD_CAPTRIM_POS, AUDDEC_ANA_CON23_CLD_CAPTRIM_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON23, 0x10<<AUDDEC_ANA_CON23_CLD_CAPTRIM_POS, AUDDEC_ANA_CON23_CLD_CAPTRIM_MASK);
            #else
            ANA_SET_REG((AUDDEC_ANA_CON23),
            ((2<<AUDDEC_ANA_CON23_CLD_SAW_FREQ_SEL_POS)|
             (1<<AUDDEC_ANA_CON23_CLD_CHOP_EN_POS)|
             (1<<AUDDEC_ANA_CON23_CLD_CAPTRIM_SEL_POS)|
             (0x10<<AUDDEC_ANA_CON23_CLD_CAPTRIM_POS)),
            (AUDDEC_ANA_CON23_CLD_SAW_FREQ_SEL_MASK|
             AUDDEC_ANA_CON23_CLD_CHOP_EN_MASK|
             AUDDEC_ANA_CON23_CLD_CAPTRIM_SEL_MASK|
             AUDDEC_ANA_CON23_CLD_CAPTRIM_MASK));
            #endif
            //class-d depop
            #if 1
            #if 1//SE to request AUDDEC_ANA_CON24_CLD_CHOP_MPH_EN_POS set to zero
            AFE_SET_REG((AUDDEC_ANA_CON24),
            ((0x4<<AUDDEC_ANA_CON24_CLD_SAW_GAIN_POS)|
             (0<<AUDDEC_ANA_CON24_CLD_PWM_SEL_POS)|
             (0<<AUDDEC_ANA_CON24_CLD_CHOP_MPH_EN_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_RST_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_OUTPUT_STAGE_EN_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_LOOP_EN_POS)|
             (0<<AUDDEC_ANA_CON24_CLD_ZCD_SEL_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_DEPOP_SYNC_CLK_POS)),
            (AUDDEC_ANA_CON24_CLD_SAW_GAIN_MASK|
             AUDDEC_ANA_CON24_CLD_PWM_SEL_MASK|
             AUDDEC_ANA_CON24_CLD_CHOP_MPH_EN_MASK|
             AUDDEC_ANA_CON24_CLD_RST_MASK|
             AUDDEC_ANA_CON24_CLD_OUTPUT_STAGE_EN_MASK|
             AUDDEC_ANA_CON24_CLD_LOOP_EN_MASK|
             AUDDEC_ANA_CON24_CLD_ZCD_SEL_MASK|
             AUDDEC_ANA_CON24_CLD_DEPOP_SYNC_CLK_MASK));
            #else
            //0930
            AFE_SET_REG((AUDDEC_ANA_CON24),
                        ((0x4<<AUDDEC_ANA_CON24_CLD_SAW_GAIN_POS)|
                         (0<<AUDDEC_ANA_CON24_CLD_PWM_SEL_POS)|
                         (1<<AUDDEC_ANA_CON24_CLD_CHOP_MPH_EN_POS)|
                         (1<<AUDDEC_ANA_CON24_CLD_RST_POS)|
                         (1<<AUDDEC_ANA_CON24_CLD_OUTPUT_STAGE_EN_POS)|
                         (1<<AUDDEC_ANA_CON24_CLD_LOOP_EN_POS)|
                         (0<<AUDDEC_ANA_CON24_CLD_ZCD_SEL_POS)|
                         (1<<AUDDEC_ANA_CON24_CLD_DEPOP_SYNC_CLK_POS)),
                        (AUDDEC_ANA_CON24_CLD_SAW_GAIN_MASK|
                         AUDDEC_ANA_CON24_CLD_PWM_SEL_MASK|
                         AUDDEC_ANA_CON24_CLD_CHOP_MPH_EN_MASK|
                         AUDDEC_ANA_CON24_CLD_RST_MASK|
                         AUDDEC_ANA_CON24_CLD_OUTPUT_STAGE_EN_MASK|
                         AUDDEC_ANA_CON24_CLD_LOOP_EN_MASK|
                         AUDDEC_ANA_CON24_CLD_ZCD_SEL_MASK|
                         AUDDEC_ANA_CON24_CLD_DEPOP_SYNC_CLK_MASK));
            #endif
            #else
            AFE_SET_REG((AUDDEC_ANA_CON24),
            (((4<<(!dac_parameter->with_high_performance))<<AUDDEC_ANA_CON24_CLD_SAW_GAIN_POS)|
             (0<<AUDDEC_ANA_CON24_CLD_PWM_SEL_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_CHOP_MPH_EN_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_RST_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_OUTPUT_STAGE_EN_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_LOOP_EN_POS)|
             (0<<AUDDEC_ANA_CON24_CLD_ZCD_SEL_POS)|
             (1<<AUDDEC_ANA_CON24_CLD_DEPOP_SYNC_CLK_POS)),
            (AUDDEC_ANA_CON24_CLD_SAW_GAIN_MASK|
             AUDDEC_ANA_CON24_CLD_PWM_SEL_MASK|
             AUDDEC_ANA_CON24_CLD_CHOP_MPH_EN_MASK|
             AUDDEC_ANA_CON24_CLD_RST_MASK|
             AUDDEC_ANA_CON24_CLD_OUTPUT_STAGE_EN_MASK|
             AUDDEC_ANA_CON24_CLD_LOOP_EN_MASK|
             AUDDEC_ANA_CON24_CLD_ZCD_SEL_MASK|
             AUDDEC_ANA_CON24_CLD_DEPOP_SYNC_CLK_MASK));
            #endif
            ANA_SET_REG((AUDDEC_ANA_CON25), (1<<AUDDEC_ANA_CON25_CLD_VCMRED_EN_POS), AUDDEC_ANA_CON25_CLD_VCMRED_EN_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON26),((7<<AUDDEC_ANA_CON26_HPL_OUT_STAG_POS)|(7<<AUDDEC_ANA_CON26_HPR_OUT_STAG_POS)),
                        ((AUDDEC_ANA_CON26_HPL_OUT_STAG_MASK&l_mask)|(AUDDEC_ANA_CON26_HPR_OUT_STAG_MASK&r_mask)));
            ANA_SET_REG((AUDDEC_ANA_CON17),
                        ((1<<AUDDEC_ANA_CON17_CLD_HPL_FB_RSW_EN_POS)|(1<<AUDDEC_ANA_CON17_CLD_HPR_FB_RSW_EN_POS)),
                        ((AUDDEC_ANA_CON17_CLD_HPL_FB_RSW_EN_MASK&l_mask)|(AUDDEC_ANA_CON17_CLD_HPR_FB_RSW_EN_MASK&r_mask)));
			//depop
            //HAL_AUDIO_DELAY_US(10);
            HAL_AUDIO_DELAY_US(250);
            ANA_SET_REG((AUDDEC_ANA_CON16),
                        ((1<<AUDDEC_ANA_CON16_CLD_HPL_MAIN_STAGE_EN_POS)|(1<<AUDDEC_ANA_CON16_CLD_HPR_MAIN_STAGE_EN_POS)),
                        ((AUDDEC_ANA_CON16_CLD_HPL_MAIN_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_MAIN_STAGE_EN_MASK&r_mask)));
            //depop
            //HAL_AUDIO_DELAY_US(10);
            HAL_AUDIO_DELAY_US(250);
            ANA_SET_REG((AUDDEC_ANA_CON16),
                        ((1<<AUDDEC_ANA_CON16_CLD_HPL_AUX_STAGE_EN_POS)|(1<<AUDDEC_ANA_CON16_CLD_HPR_AUX_STAGE_EN_POS)),
                        ((AUDDEC_ANA_CON16_CLD_HPL_AUX_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_AUX_STAGE_EN_MASK&r_mask)));
            //depop
            //HAL_AUDIO_DELAY_US(10);
            HAL_AUDIO_DELAY_US(250);
            ANA_SET_REG((AUDDEC_ANA_CON16),
                        ((1<<AUDDEC_ANA_CON16_CLD_HPL_PWRUP_POS)|(1<<AUDDEC_ANA_CON16_CLD_HPR_PWRUP_POS)),
                        ((AUDDEC_ANA_CON16_CLD_HPL_PWRUP_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_PWRUP_MASK&r_mask)));
            //depop
            //HAL_AUDIO_DELAY_US(10);
            HAL_AUDIO_DELAY_US(250);
            ANA_SET_REG((AUDDEC_ANA_CON16),
                        ((1<<AUDDEC_ANA_CON16_CLD_HPL_IBAS_POS)|(1<<AUDDEC_ANA_CON16_CLD_HPR_IBAS_POS)),
                        ((AUDDEC_ANA_CON16_CLD_HPL_IBAS_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_IBAS_MASK&r_mask)));
            //depop
            //HAL_AUDIO_DELAY_US(10);
            HAL_AUDIO_DELAY_US(250);
            ANA_SET_REG((AUDDEC_ANA_CON1),
                        (1<<AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_POS)|(1<<AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_POS),
                        (AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_MASK&r_mask));
            //HAL_AUDIO_DELAY_US(600);
            HAL_AUDIO_DELAY_US(1000);
            #if 0//SE request to move to above
            if(!dac_parameter->with_high_performance){
                ANA_SET_REG(AUDDEC_ANA_CON21, (0<<AUDDEC_ANA_CON21_CLD_SR_SEL_POS), AUDDEC_ANA_CON21_CLD_SR_SEL_MASK);
            }
            ANA_SET_REG(AUDDEC_ANA_CON21, (0<<AUDDEC_ANA_CON21_CLD_VCM_SEL_POS), AUDDEC_ANA_CON21_CLD_VCM_SEL_MASK);
            #endif
            ANA_SET_REG((AUDDEC_ANA_CON17),
                        ((0<<AUDDEC_ANA_CON17_CLD_HPL_FB_RSW_EN_POS)|(0<<AUDDEC_ANA_CON17_CLD_HPR_FB_RSW_EN_POS)),
                        ((AUDDEC_ANA_CON17_CLD_HPL_FB_RSW_EN_MASK&l_mask)|(AUDDEC_ANA_CON17_CLD_HPR_FB_RSW_EN_MASK&r_mask)));
            HAL_AUDIO_DELAY_US(100);
            #if 0//SE request to move to here from below
            if(!dac_parameter->with_high_performance){
                ANA_SET_REG(AUDDEC_ANA_CON21, (0<<AUDDEC_ANA_CON21_CLD_SR_SEL_POS), AUDDEC_ANA_CON21_CLD_SR_SEL_MASK);
            }
            HAL_AUDIO_DELAY_US(100);
            ANA_SET_REG(AUDDEC_ANA_CON21, (0<<AUDDEC_ANA_CON21_CLD_VCM_SEL_POS), AUDDEC_ANA_CON21_CLD_VCM_SEL_MASK);
            HAL_AUDIO_DELAY_US(100);
            #endif
            ANA_SET_REG((AUDDEC_ANA_CON16),
                        ((0<<AUDDEC_ANA_CON16_CLD_HPL_AUX_STAGE_EN_POS)|(0<<AUDDEC_ANA_CON16_CLD_HPR_AUX_STAGE_EN_POS)),
                        ((AUDDEC_ANA_CON16_CLD_HPL_AUX_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_AUX_STAGE_EN_MASK&r_mask)));
            ANA_SET_REG((AUDDEC_ANA_CON1),
                        (0<<AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_POS)|(0<<AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_POS),
                        (AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_MASK&r_mask));
            HAL_AUDIO_DELAY_US(1000);
            #if 0//SE request to move to below
            if(!dac_parameter->with_high_performance){
                ANA_SET_REG(AUDDEC_ANA_CON21, (0<<AUDDEC_ANA_CON21_CLD_SR_SEL_POS), AUDDEC_ANA_CON21_CLD_SR_SEL_MASK);
            }
            ANA_SET_REG(AUDDEC_ANA_CON21, (0<<AUDDEC_ANA_CON21_CLD_VCM_SEL_POS), AUDDEC_ANA_CON21_CLD_VCM_SEL_MASK);

            //class-d de-pop
            //ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPR_STB_EN_POS)|(0<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPR_STB_EN_MASK|AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK);
            //ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPR_STB_EN_POS), AUDDEC_ANA_CON2_HPR_STB_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK);
            #endif
            #if 0//SE request to move to here from below
            ANA_SET_REG((AUDDEC_ANA_CON22),4<<AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_POS,AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_MASK);
            HAL_AUDIO_DELAY_US(500);
            ANA_SET_REG((AUDDEC_ANA_CON22),2<<AUDDEC_ANA_CON22_CLD_LF1_SEL_POS,AUDDEC_ANA_CON22_CLD_LF1_SEL_MASK);
            HAL_AUDIO_DELAY_US(500);
            ANA_SET_REG((AUDDEC_ANA_CON22),3<<AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_POS,AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_MASK);
            HAL_AUDIO_DELAY_US(500);
            #endif
            //class-d de-pop
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E0A1188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E0A1188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E121188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E121188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E1A1188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E1A1188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E221188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E221188, 0xFFFFFFFF);
            #if 1//SE request to move to below from above
            if(!dac_parameter->with_high_performance){
                ANA_SET_REG(AUDDEC_ANA_CON21, (0<<AUDDEC_ANA_CON21_CLD_SR_SEL_POS), AUDDEC_ANA_CON21_CLD_SR_SEL_MASK);
            }
            ANA_SET_REG(AUDDEC_ANA_CON21, (0<<AUDDEC_ANA_CON21_CLD_VCM_SEL_POS), AUDDEC_ANA_CON21_CLD_VCM_SEL_MASK);

            //class-d de-pop
            //ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPR_STB_EN_POS)|(0<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPR_STB_EN_MASK|AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK);
            //ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPR_STB_EN_POS), AUDDEC_ANA_CON2_HPR_STB_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS), AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK);
            #endif
            //de-pop
            HAL_AUDIO_DELAY_US(1000);
            //AFE_SET_REG((AUDDEC_ANA_CON24),(4<<AUDDEC_ANA_CON24_CLD_SAW_GAIN_POS),AUDDEC_ANA_CON24_CLD_SAW_GAIN_MASK);

            //AFE_SET_REG((AUDDEC_ANA_CON2),(0<<AUDDEC_ANA_CON2_HPL_STB_EN_POS),AUDDEC_ANA_CON2_HPL_STB_EN_MASK);
            //AFE_SET_REG((AUDDEC_ANA_CON2),(0<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS),AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK);
            //AFE_SET_REG((AUDDEC_ANA_CON2),(0<<AUDDEC_ANA_CON2_HPR_STB_EN_POS),AUDDEC_ANA_CON2_HPR_STB_EN_MASK);
            //AFE_SET_REG((AUDDEC_ANA_CON2),(0<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS),AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK);
            //#endif
            //printf("AUDDEC_ANA_CON24 addr:0x%x,val:0x%x",AUDDEC_ANA_CON24,ANA_READ(AUDDEC_ANA_CON24));
            #if 0//move to above for depop 0710
            //DAC
            ANA_SET_REG((AUDDEC_ANA_CON0),
                       ((1<<AUDDEC_ANA_CON0_DAC_LPM_EN_POS)|(0<<AUDDEC_ANA_CON0_DAC_CHOP_EN_POS)|(0<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_POS)|(0<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_POS)),
                       (AUDDEC_ANA_CON0_DAC_LPM_EN_MASK|AUDDEC_ANA_CON0_DAC_CHOP_EN_MASK|AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_MASK|AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_MASK));
            ANA_SET_REG(AUDDEC_ANA_CON15, (0<<AUDDEC_ANA_CON15_DAC_CURRENT_2X_POS), AUDDEC_ANA_CON15_DAC_CURRENT_2X_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON0),
                       ((1<<AUDDEC_ANA_CON0_DAC_L_PWRUP_POS)|(1<<AUDDEC_ANA_CON0_DAC_R_PWRUP_POS)|(1<<AUDDEC_ANA_CON0_DAC_L_BIAS_EN_POS)|(1<<AUDDEC_ANA_CON0_DAC_R_BIAS_EN_POS)),
                       (((AUDDEC_ANA_CON0_DAC_L_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_L_BIAS_EN_MASK)&l_mask)|((AUDDEC_ANA_CON0_DAC_R_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_R_BIAS_EN_MASK)&r_mask)));
            HAL_AUDIO_DELAY_US(100);
            #endif

            ANA_SET_REG((AUDDEC_ANA_CON16),
                       ((1<<AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_POS)|(1<<AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_POS)),
                       ((AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_MASK&r_mask)));
            HAL_AUDIO_DELAY_US(100);
            #if 1//SE request to move to CM4
            ANA_SET_REG((AUDDEC_ANA_CON22),4<<AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_POS,AUDDEC_ANA_CON22_CLD_CHOP_FREQ_SEL_MASK);
            HAL_AUDIO_DELAY_US(500);
            ANA_SET_REG((AUDDEC_ANA_CON22),2<<AUDDEC_ANA_CON22_CLD_LF1_SEL_POS,AUDDEC_ANA_CON22_CLD_LF1_SEL_MASK);
            HAL_AUDIO_DELAY_US(500);
            ANA_SET_REG((AUDDEC_ANA_CON22),3<<AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_POS,AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_MASK);
            HAL_AUDIO_DELAY_US(500);
            #endif


            hal_audio_clock_enable_classg(false);    //For ClassD ClassAB add for lowpower

        }



        if (dac_parameter->with_high_performance) {
        } else {
        }

        if ((dac_parameter->dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSAB) ||
            (dac_parameter->dac_mode == HAL_AUDIO_ANALOG_OUTPUT_CLASSG)) {
            hal_audio_dl_set_classg_enable(true);
        }

    } else {
        //OFF

        if (with_classg) {

            //ClassAB off
            ANA_SET_REG(AUDDEC_ANA_CON1, (0<<AUDDEC_ANA_CON1_HPL_INPUT_SEL_POS)|(0<<AUDDEC_ANA_CON1_HPR_INPUT_SEL_POS), (AUDDEC_ANA_CON1_HPL_INPUT_SEL_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_INPUT_SEL_MASK&r_mask));
            ANA_SET_REG(AUDDEC_ANA_CON3, (1<<AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_POS), AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON11, (1<<AUDDEC_ANA_CON11_HPL_STBEHN_EN_POS)|(1<<AUDDEC_ANA_CON11_HPR_STBEHN_EN_POS), (AUDDEC_ANA_CON11_HPL_STBEHN_EN_MASK&l_mask)|(AUDDEC_ANA_CON11_HPR_STBEHN_EN_MASK&r_mask));
            //AB1568_ADVT_DL_CLG_Procedures_20210126.xlsx
            if(with_classg && (!dac_parameter->with_high_performance)){
                ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), (AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK&r_mask));
            }

            ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_SHORT_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_SHORT_EN_POS), ((AUDDEC_ANA_CON2_HPL_SHORT_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_SHORT_EN_MASK&r_mask)));

            ANA_SET_REG(AUDDEC_ANA_CON1, (1<<AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_POS)|(1<<AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_POS), (AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_MASK&r_mask));

            ANA_SET_REG((AUDDEC_ANA_CON0),
            ((0<<AUDDEC_ANA_CON0_DAC_L_PWRUP_POS)|
             (0<<AUDDEC_ANA_CON0_DAC_R_PWRUP_POS)|
             (0<<AUDDEC_ANA_CON0_DAC_L_BIAS_EN_POS)|
             (0<<AUDDEC_ANA_CON0_DAC_R_BIAS_EN_POS)),
             (((AUDDEC_ANA_CON0_DAC_L_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_L_BIAS_EN_MASK)&l_mask)|
             ((AUDDEC_ANA_CON0_DAC_R_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_R_BIAS_EN_MASK)&r_mask)));
            HAL_AUDIO_DELAY_US(50);
            //printf("classg OFF AUDDEC_ANA_CON1 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON1,ANA_READ(AUDDEC_ANA_CON1));
            //printf("classg OFF AUDDEC_ANA_CON3 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON3,ANA_READ(AUDDEC_ANA_CON3));
            //printf("classg OFF AUDDEC_ANA_CON11 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON11,ANA_READ(AUDDEC_ANA_CON11));
            //printf("classg OFF AUDDEC_ANA_CON2 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON2,ANA_READ(AUDDEC_ANA_CON2));
            //printf("classg OFF AUDDEC_ANA_CON0 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON0,ANA_READ(AUDDEC_ANA_CON0));
            afe_volume_analog_ramp_output(AFE_HW_ANALOG_OUTPUT_CLASSAB_MIN_VALUE);

            //ClassAB off
            #if 0
            ANA_SET_REG(AUDDEC_ANA_CON1, (0<<AUDDEC_ANA_CON1_HPL_INPUT_SEL_POS)|(0<<AUDDEC_ANA_CON1_HPR_INPUT_SEL_POS), (AUDDEC_ANA_CON1_HPL_INPUT_SEL_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_INPUT_SEL_MASK&r_mask));
            ANA_SET_REG(AUDDEC_ANA_CON3, (1<<AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_POS), AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON11, (1<<AUDDEC_ANA_CON11_HPL_STBEHN_EN_POS)|(1<<AUDDEC_ANA_CON11_HPR_STBEHN_EN_POS), (AUDDEC_ANA_CON11_HPL_STBEHN_EN_MASK&l_mask)|(AUDDEC_ANA_CON11_HPR_STBEHN_EN_MASK&r_mask));
            ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_SHORT_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_SHORT_EN_POS), ((AUDDEC_ANA_CON2_HPL_SHORT_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_SHORT_EN_MASK&r_mask)));

            ANA_SET_REG(AUDDEC_ANA_CON1, (1<<AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_POS)|(1<<AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_POS), (AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_MASK&r_mask));
            HAL_AUDIO_DELAY_US(50);
            #endif
            //gain Ramp
            HAL_AUDIO_DELAY_US(600);
            ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_AUX_FB_POS)|(1<<AUDDEC_ANA_CON2_HPR_AUX_FB_POS), (AUDDEC_ANA_CON2_HPL_AUX_FB_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_AUX_FB_MASK&r_mask));

            for (value=16 ; value>0 ; value--) {
                ANA_SET_REG(AUDDEC_ANA_CON3, ((value-1)<<AUDDEC_ANA_CON3_HPL_AUX_GAIN_POS)|((value-1)<<AUDDEC_ANA_CON3_HPR_AUX_GAIN_POS), (AUDDEC_ANA_CON3_HPL_AUX_GAIN_MASK&l_mask)|(AUDDEC_ANA_CON3_HPR_AUX_GAIN_MASK&r_mask));
                HAL_AUDIO_DELAY_US(600);
            }
            //if(dsp_afe_loopack_exit){
            //    return 0;
            //}

            for (value=8 ; value>0 ; value--) {
                AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, (value-1)<<AFE_CLASSG_LPSLCH_CFG0_STG_CTRL_POS, AFE_CLASSG_LPSLCH_CFG0_STG_CTRL_MASK);
                AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, (value-1)<<AFE_CLASSG_LPSRCH_CFG0_STG_CTRL_POS, AFE_CLASSG_LPSRCH_CFG0_STG_CTRL_MASK);
                HAL_AUDIO_DELAY_US(600);
            }

            ANA_SET_REG(AUDDEC_ANA_CON1, (0<<AUDDEC_ANA_CON1_HPL_MAIN_STAGE_EN_POS)|(0<<AUDDEC_ANA_CON1_HPR_MAIN_STAGE_EN_POS), (AUDDEC_ANA_CON1_HPL_MAIN_STAGE_EN_MASK&l_mask)|(AUDDEC_ANA_CON1_HPR_MAIN_STAGE_EN_MASK&r_mask));
            ANA_SET_REG(AUDDEC_ANA_CON2, (1<<AUDDEC_ANA_CON2_HPL_AUX_CMFB_EN_POS)|(1<<AUDDEC_ANA_CON2_HPR_AUX_CMFB_EN_POS), (AUDDEC_ANA_CON2_HPL_AUX_CMFB_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_AUX_CMFB_EN_MASK&r_mask));
            HAL_AUDIO_DELAY_US(50);
            ANA_SET_REG((AUDDEC_ANA_CON2),
                        ((0<<AUDDEC_ANA_CON2_HPL_MAIN_CMFB_EN_POS)|
                         (0<<AUDDEC_ANA_CON2_HPR_MAIN_CMFB_EN_POS)|
                         (0<<AUDDEC_ANA_CON2_HPL_SHORT_EN_POS)|
                         (0<<AUDDEC_ANA_CON2_HPR_SHORT_EN_POS)),
                        (((AUDDEC_ANA_CON2_HPL_MAIN_CMFB_EN_MASK|AUDDEC_ANA_CON2_HPL_SHORT_EN_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON2_HPR_MAIN_CMFB_EN_MASK|AUDDEC_ANA_CON2_HPR_SHORT_EN_MASK)&r_mask)));
            ANA_SET_REG((AUDDEC_ANA_CON1),
                        ((0<<AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_POS)|
                         (0<<AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_POS)|
                         (0<<AUDDEC_ANA_CON1_HPL_BIAS_EN_POS)|
                         (0<<AUDDEC_ANA_CON1_HPR_BIAS_EN_POS)|
                         (0<<AUDDEC_ANA_CON1_HPL_PWRUP_EN_POS)|
                         (0<<AUDDEC_ANA_CON1_HPR_PWRUP_EN_POS)),
                        (((AUDDEC_ANA_CON1_HPL_AUX_STAGE_EN_MASK|AUDDEC_ANA_CON1_HPL_BIAS_EN_MASK|AUDDEC_ANA_CON1_HPL_PWRUP_EN_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON1_HPR_AUX_STAGE_EN_MASK|AUDDEC_ANA_CON1_HPR_BIAS_EN_MASK|AUDDEC_ANA_CON1_HPR_PWRUP_EN_MASK)&r_mask)));
            ANA_SET_REG((AUDDEC_ANA_CON2),
                        ((0<<AUDDEC_ANA_CON2_HPL_AUX_CMFB_EN_POS)|
                         (0<<AUDDEC_ANA_CON2_HPR_AUX_CMFB_EN_POS)|
                         (1<<AUDDEC_ANA_CON2_HPL_MAIN_CMFB_EN_POS)|
                         (1<<AUDDEC_ANA_CON2_HPR_MAIN_CMFB_EN_POS)|
                         (0<<AUDDEC_ANA_CON2_HPL_AUX_FB_POS)|
                         (0<<AUDDEC_ANA_CON2_HPR_AUX_FB_POS)),
                        (((AUDDEC_ANA_CON2_HPL_AUX_CMFB_EN_MASK|AUDDEC_ANA_CON2_HPL_MAIN_CMFB_EN_MASK|AUDDEC_ANA_CON2_HPL_AUX_FB_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON2_HPR_AUX_CMFB_EN_MASK|AUDDEC_ANA_CON2_HPR_MAIN_CMFB_EN_MASK|AUDDEC_ANA_CON2_HPR_AUX_FB_MASK)&r_mask)));

            ANA_SET_REG(AUDDEC_ANA_CON13, (0<<AUDDEC_ANA_CON13_PULL_HIGH_POS)|(0<<AUDDEC_ANA_CON13_GM2_BOOST_PDN_POS), AUDDEC_ANA_CON13_PULL_HIGH_MASK|AUDDEC_ANA_CON13_GM2_BOOST_PDN_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON11, (0<<AUDDEC_ANA_CON11_HPL_STBEHN_EN_POS)|(0<<AUDDEC_ANA_CON11_HPR_STBEHN_EN_POS), (AUDDEC_ANA_CON11_HPL_STBEHN_EN_MASK&l_mask)|(AUDDEC_ANA_CON11_HPR_STBEHN_EN_MASK&r_mask));
            AFE_SET_REG(ZCD_CON0, 0<<ZCD_CON0_ZCD_EN_POS, ZCD_CON0_ZCD_EN_MASK);
            #if 1
            ANA_SET_REG((AUDDEC_ANA_CON0),
                        ((0<<AUDDEC_ANA_CON0_DAC_L_PWRUP_POS)|
                         (0<<AUDDEC_ANA_CON0_DAC_R_PWRUP_POS)|
                         (0<<AUDDEC_ANA_CON0_DAC_L_BIAS_EN_POS)|
                         (0<<AUDDEC_ANA_CON0_DAC_R_BIAS_EN_POS)|
                         (1<<AUDDEC_ANA_CON0_DAC_LPM_EN_POS)),
                        (AUDDEC_ANA_CON0_DAC_LPM_EN_MASK|
                         ((AUDDEC_ANA_CON0_DAC_L_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_L_BIAS_EN_MASK)&l_mask)|
                         ((AUDDEC_ANA_CON0_DAC_R_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_R_BIAS_EN_MASK)&r_mask)));
            #endif
            AFE_SET_REG(ABB_CLK_GEN_CFG_1, (0<<ABB_CLK_GEN_CFG_1_DIV_CHANGE_POS)|(0<<ABB_CLK_GEN_CFG_1_DIV_EN_POS), ABB_CLK_GEN_CFG_1_DIV_CHANGE_MASK|ABB_CLK_GEN_CFG_1_DIV_EN_MASK);
        } else {
            //ClassD Off
            hal_audio_clock_enable_classg(true);    //For ClassD ClassAB add for lowpower

            //Here Set Analog Class-D
            ANA_SET_REG((AUDDEC_ANA_CON22),3<<AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_POS,AUDDEC_ANA_CON22_CLD_CMP_BIAS_SEL_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON16),
           ((0<<AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_POS)|(0<<AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_POS)),
           ((AUDDEC_ANA_CON16_CLD_HPL_MUXIN_SEL_MASK&l_mask)|(AUDDEC_ANA_CON16_CLD_HPR_MUXIN_SEL_MASK&r_mask)));

            //IDAC
            AFE_SET_REG(AUDDEC_ANA_CON12,(1<<AUDDEC_ANA_CON12_VCMT_RSV0_R_IDAC_POS)|(1<<AUDDEC_ANA_CON12_VCMT_RSV0_L_IDAC_POS), AUDDEC_ANA_CON12_VCMT_RSV0_R_IDAC_MASK|AUDDEC_ANA_CON12_VCMT_RSV0_L_IDAC_MASK);

            ANA_SET_REG((AUDDEC_ANA_CON0),
            ((0<<AUDDEC_ANA_CON0_DAC_L_PWRUP_POS)|
             (0<<AUDDEC_ANA_CON0_DAC_R_PWRUP_POS)|
             (0<<AUDDEC_ANA_CON0_DAC_L_BIAS_EN_POS)|
             (0<<AUDDEC_ANA_CON0_DAC_R_BIAS_EN_POS)),
             (((AUDDEC_ANA_CON0_DAC_L_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_L_BIAS_EN_MASK)&l_mask)|
             ((AUDDEC_ANA_CON0_DAC_R_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_R_BIAS_EN_MASK)&r_mask)));
            ANA_SET_REG((AUDDEC_ANA_CON0),1<<AUDDEC_ANA_CON0_DAC_LPM_EN_POS,AUDDEC_ANA_CON0_DAC_LPM_EN_MASK);
            //here
            ANA_SET_REG((AUDDEC_ANA_CON0),0<<AUDDEC_ANA_CON0_DAC_CHOP_EN_POS,AUDDEC_ANA_CON0_DAC_CHOP_EN_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON0),0<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_POS,AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON0),0<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_POS,AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_MASK);
            //0826
            ANA_SET_REG((AUDDEC_ANA_CON15),1<<AUDDEC_ANA_CON15_SET_LCH_INPUT_PULL_HIGH_POS,AUDDEC_ANA_CON15_SET_LCH_INPUT_PULL_HIGH_MASK);

            #if 0
            AFE_SET_REG(AUDDEC_ANA_CON24,(4<<AUDDEC_ANA_CON24_CLD_SAW_GAIN_POS), AUDDEC_ANA_CON24_CLD_SAW_GAIN_MASK);
            HAL_AUDIO_DELAY_US(1000);
            //classd depop 0521
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E0A1188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E0A1188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E121188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E121188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E1A1188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E1A1188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E221188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E221188, 0xFFFFFFFF);
            AFE_SET_REG(AUDDEC_ANA_CON24,(2<<AUDDEC_ANA_CON24_CLD_SAW_GAIN_POS), AUDDEC_ANA_CON24_CLD_SAW_GAIN_MASK);
            HAL_AUDIO_DELAY_US(10000);
            #endif
            //add for depop
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E221188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E221188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E1A1188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E1A1188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E121188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E121188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E0A1188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E0A1188, 0xFFFFFFFF);
            HAL_AUDIO_DELAY_US(1000);
            AFE_SET_REG(AFE_CLASSG_LPSLCH_CFG0, 0x0E021188, 0xFFFFFFFF);
            AFE_SET_REG(AFE_CLASSG_LPSRCH_CFG0, 0x0E021188, 0xFFFFFFFF);


        }

        //DAC off
        ANA_SET_REG((AUDDEC_ANA_CON0),
                    ((0<<AUDDEC_ANA_CON0_DAC_L_PWRUP_POS)|
                     (0<<AUDDEC_ANA_CON0_DAC_R_PWRUP_POS)|
                     (0<<AUDDEC_ANA_CON0_DAC_L_BIAS_EN_POS)|
                     (0<<AUDDEC_ANA_CON0_DAC_R_BIAS_EN_POS)|
                     (1<<AUDDEC_ANA_CON0_DAC_LPM_EN_POS)|
                     (0<<AUDDEC_ANA_CON0_DAC_CHOP_EN_POS)|
                     (0<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_POS)|
                     (0<<AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_POS)),
                    (AUDDEC_ANA_CON0_DAC_LPM_EN_MASK|
                     AUDDEC_ANA_CON0_DAC_CHOP_EN_MASK|
                     AUDDEC_ANA_CON0_DAC_IBIAS_BST_SEL_MASK|
                     AUDDEC_ANA_CON0_DAC_IBIAS_BST_EN_MASK)|
                     ((AUDDEC_ANA_CON0_DAC_L_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_L_BIAS_EN_MASK)&l_mask)|
                     ((AUDDEC_ANA_CON0_DAC_R_PWRUP_MASK|AUDDEC_ANA_CON0_DAC_R_BIAS_EN_MASK)&r_mask));
        ANA_SET_REG(AUDDEC_ANA_CON15, (0<<AUDDEC_ANA_CON15_DAC_CURRENT_2X_POS), AUDDEC_ANA_CON15_DAC_CURRENT_2X_MASK);
        HAL_AUDIO_DELAY_US(100);

        //ClassD
        AFE_SET_REG(AUDDEC_ANA_CON24,(0<<AUDDEC_ANA_CON24_CLD_OUTPUT_STAGE_EN_POS), AUDDEC_ANA_CON24_CLD_OUTPUT_STAGE_EN_MASK);
        ANA_SET_REG((AUDDEC_ANA_CON16),
                    ((0<<AUDDEC_ANA_CON16_CLD_HPL_MAIN_STAGE_EN_POS)|
                     (0<<AUDDEC_ANA_CON16_CLD_HPR_MAIN_STAGE_EN_POS)|
                     (0<<AUDDEC_ANA_CON16_CLD_HPL_PWRUP_POS)|
                     (0<<AUDDEC_ANA_CON16_CLD_HPR_PWRUP_POS)|
                     (0<<AUDDEC_ANA_CON16_CLD_HPL_IBAS_POS)|
                     (0<<AUDDEC_ANA_CON16_CLD_HPR_IBAS_POS)),
                    (((AUDDEC_ANA_CON16_CLD_HPL_MAIN_STAGE_EN_MASK|AUDDEC_ANA_CON16_CLD_HPL_PWRUP_MASK|AUDDEC_ANA_CON16_CLD_HPL_IBAS_MASK)&l_mask)|
                     ((AUDDEC_ANA_CON16_CLD_HPR_MAIN_STAGE_EN_MASK|AUDDEC_ANA_CON16_CLD_HPR_PWRUP_MASK|AUDDEC_ANA_CON16_CLD_HPR_IBAS_MASK)&r_mask)));


        //Clock and bias
        ANA_SET_REG((AUDDEC_ANA_CON9),
                    ((0<<AUDDEC_ANA_CON9_DAC_13M_EN_POS)|(0<<AUDDEC_ANA_CON9_DAC_26M_EN_POS)|(0<<AUDDEC_ANA_CON9_CLD_26M_EN_POS)),
                    (AUDDEC_ANA_CON9_DAC_13M_EN_MASK|AUDDEC_ANA_CON9_DAC_26M_EN_MASK|AUDDEC_ANA_CON9_CLD_26M_EN_MASK));
        HAL_AUDIO_DELAY_US(100);
        ANA_SET_REG(AUDDEC_ANA_CON8, (1<<AUDDEC_ANA_CON8_IBIAS_PDN_POS), AUDDEC_ANA_CON8_IBIAS_PDN_MASK);
        HAL_AUDIO_DELAY_US(100);


        //VCM
        ANA_SET_REG((AUDDEC_ANA_CON13),
                    ((0<<AUDDEC_ANA_CON13_HP_COM_CAP_SHORT_POS)|
                     (1<<AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_POS)|
                     (0<<AUDDEC_ANA_CON13_LCLDO_DAC_EN_POS)|
                     (0<<AUDDEC_ANA_CON13_LCLDO_IBAS_EN_POS)|
                     (0<<AUDDEC_ANA_CON13_HCLDO_IBAS_EN_POS)|
                     (0<<AUDDEC_ANA_CON13_LDO_REF_IBAS_EN_POS)),
                    (AUDDEC_ANA_CON13_HP_COM_CAP_SHORT_MASK|
                     AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_MASK|
                     AUDDEC_ANA_CON13_LCLDO_DAC_EN_MASK|
                     AUDDEC_ANA_CON13_LCLDO_IBAS_EN_MASK|
                     AUDDEC_ANA_CON13_HCLDO_IBAS_EN_MASK|
                     AUDDEC_ANA_CON13_LDO_REF_IBAS_EN_MASK));
        if(!with_classg){
           ANA_SET_REG(AUDDEC_ANA_CON13, (1<<AUDDEC_ANA_CON13_OFF_COMPARATOR_POS), AUDDEC_ANA_CON13_OFF_COMPARATOR_MASK);
        }

        HAL_AUDIO_DELAY_US(600);//
        ANA_SET_REG((AUDDEC_ANA_CON11),
                    ((0<<AUDDEC_ANA_CON11_HCLDO_EN_POS)|(0<<AUDDEC_ANA_CON11_LCLDO_EN_POS)),
                    (AUDDEC_ANA_CON11_HCLDO_EN_MASK|AUDDEC_ANA_CON11_LCLDO_EN_MASK));
         //AB1568_ADVT_DL_CLG_Procedures_20210126.xlsx
        if(with_classg){
            ANA_SET_REG(AUDDEC_ANA_CON11, (0<<AUDDEC_ANA_CON11_HPL_STBEHN_EN_POS)|(0<<AUDDEC_ANA_CON11_HPR_STBEHN_EN_POS), (AUDDEC_ANA_CON11_HPL_STBEHN_EN_MASK&l_mask)|(AUDDEC_ANA_CON11_HPR_STBEHN_EN_MASK&r_mask));
            ANA_SET_REG(AUDDEC_ANA_CON2, (0<<AUDDEC_ANA_CON2_HPL_BUFSW_EN_POS)|(0<<AUDDEC_ANA_CON2_HPR_BUFSW_EN_POS), (AUDDEC_ANA_CON2_HPL_BUFSW_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_BUFSW_EN_MASK&r_mask));
        }
        ANA_SET_REG((AUDDEC_ANA_CON11),(0<<AUDDEC_ANA_CON11_LCLDO_DAC_EN_POS),AUDDEC_ANA_CON11_LCLDO_DAC_EN_MASK);


        HAL_AUDIO_DELAY_US(100);
        ANA_SET_REG((AUDDEC_ANA_CON10),
                    ((0<<AUDDEC_ANA_CON10_2P4VLDO_CORE_EN_POS)|(0<<AUDDEC_ANA_CON10_2P4VLDO_DEC_EN_POS)|(0<<AUDDEC_ANA_CON10_2P4VLDO_DAC_EN_POS)),
                    (AUDDEC_ANA_CON10_2P4VLDO_CORE_EN_MASK|AUDDEC_ANA_CON10_2P4VLDO_DEC_EN_MASK|AUDDEC_ANA_CON10_2P4VLDO_DAC_EN_MASK));
        HAL_AUDIO_DELAY_US(100);
        ANA_SET_REG((AUDDEC_ANA_CON12),
                    ((0<<AUDDEC_ANA_CON12_VCMT_CLAMPING_POS)|(0<<AUDDEC_ANA_CON12_VCMT_COMPARATOR_POS)),
                    (AUDDEC_ANA_CON12_VCMT_CLAMPING_MASK|AUDDEC_ANA_CON12_VCMT_COMPARATOR_MASK));
        ANA_SET_REG(AUDDEC_ANA_CON13, (0<<AUDDEC_ANA_CON13_OFF_COMPARATOR_POS), AUDDEC_ANA_CON13_OFF_COMPARATOR_MASK);
        ANA_SET_REG((AUDDEC_ANA_CON12),
                    ((0<<AUDDEC_ANA_CON12_VCMT_CMP_THRE_SEL_POS)|(0<<AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_POS)|(1<<AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_POS)|(0<<AUDDEC_ANA_CON12_VCMT_RDIV_EN_POS)|(0<<AUDDEC_ANA_CON12_VCMT_OP_EN_POS)),
                    (AUDDEC_ANA_CON12_VCMT_CMP_THRE_SEL_MASK|AUDDEC_ANA_CON12_VCMT_CHARGE_SEL_MASK|AUDDEC_ANA_CON12_VCMT_PAD_PULLLOW_MASK|AUDDEC_ANA_CON12_VCMT_RDIV_EN_MASK|AUDDEC_ANA_CON12_VCMT_OP_EN_MASK));

        HAL_AUDIO_DELAY_US(50);
        ANA_SET_REG(AUDDEC_ANA_CON3, (0<<AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_POS), AUDDEC_ANA_CON3_HP_OUT_SHORT_EN_MASK);

        //Reset
        if (1) {
            ANA_SET_REG(AUDDEC_ANA_CON13,(0<<AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_POS), AUDDEC_ANA_CON13_CLASSG_GAIN_MAX_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON11),
                        ((0<<AUDDEC_ANA_CON11_HCLDO_REMOTE_POS)|(0<<AUDDEC_ANA_CON11_LCLDO_REMOTE_POS)|(0<<AUDDEC_ANA_CON11_LCLDO_DAC_REMOTE_POS)),
                        (AUDDEC_ANA_CON11_HCLDO_REMOTE_MASK|AUDDEC_ANA_CON11_LCLDO_REMOTE_MASK|AUDDEC_ANA_CON11_LCLDO_DAC_REMOTE_MASK));
            ANA_SET_REG(AUDDEC_ANA_CON8, (0x3f<<AUDDEC_ANA_CON8_BIAS_ADJ_POS), AUDDEC_ANA_CON8_BIAS_ADJ_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON5, (0<<AUDDEC_ANA_CON5_HP_BIAS_SEL_POS)|(3<<AUDDEC_ANA_CON5_HP_1ST_CAP_SEL_POS), AUDDEC_ANA_CON5_HP_BIAS_SEL_MASK|AUDDEC_ANA_CON5_HP_1ST_CAP_SEL_MASK);
            ANA_SET_REG(AUDDEC_ANA_CON3, (0<<AUDDEC_ANA_CON3_HP_TRIM_EN_POS), AUDDEC_ANA_CON3_HP_TRIM_EN_MASK);
            ANA_SET_REG((AUDDEC_ANA_CON2),
                        ((0<<AUDDEC_ANA_CON2_HPL_MAIN_CMFB_EN_POS)|(0<<AUDDEC_ANA_CON2_HPR_MAIN_CMFB_EN_POS)),
                        ((AUDDEC_ANA_CON2_HPL_MAIN_CMFB_EN_MASK&l_mask)|(AUDDEC_ANA_CON2_HPR_MAIN_CMFB_EN_MASK&r_mask)));
        }
        //SET default by TY---start
        ANA_WRITE(AUDDEC_ANA_CON0,0x0);
        ANA_WRITE(AUDDEC_ANA_CON1,0x0);
        ANA_WRITE(AUDDEC_ANA_CON2,0x0);
        ANA_WRITE(AUDDEC_ANA_CON3,0x0);
        ANA_WRITE(AUDDEC_ANA_CON4,0x0);
        ANA_WRITE(AUDDEC_ANA_CON5,0x00001B08);
        ANA_WRITE(AUDDEC_ANA_CON6,0x0);
        ANA_WRITE(AUDDEC_ANA_CON7,0x00000533);
        ANA_WRITE(AUDDEC_ANA_CON8,0x0000FFFF);
        ANA_WRITE(AUDDEC_ANA_CON11,0x0);
        ANA_WRITE(AUDDEC_ANA_CON12,0x0);
        ANA_WRITE(AUDDEC_ANA_CON13,0x0);
        ANA_WRITE(AUDDEC_ANA_CON14,0x0);
        ANA_WRITE(AUDDEC_ANA_CON15,0x0);
        ANA_WRITE(AUDDEC_ANA_CON16,0x0);
        ANA_WRITE(AUDDEC_ANA_CON17,0x0);
        ANA_WRITE(AUDDEC_ANA_CON18,0x0);
        ANA_WRITE(AUDDEC_ANA_CON19,0x0);
        ANA_WRITE(AUDDEC_ANA_CON20,0x0);
        ANA_WRITE(AUDDEC_ANA_CON21,0x0);
        ANA_WRITE(AUDDEC_ANA_CON22,0x0);
        ANA_WRITE(AUDDEC_ANA_CON23,0x0);
        AFE_WRITE(AUDDEC_ANA_CON24,0x0);
        ANA_WRITE(AUDDEC_ANA_CON25,0x0);
        ANA_WRITE(AUDDEC_ANA_CON26,0x0);
        //SET default by TY---end


        hal_audio_afe_enable_clksq(false);
        if (with_classg) {
            hal_audio_dl_set_classg(false);
        }else{
            hal_audio_dl_set_classd(false);
        }
        hal_audio_clock_enable_classg(false);
        hal_audio_dl_set_sdm_enable(false);
        //printf("classg OFF END AUDDEC_ANA_CON1 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON1,ANA_READ(AUDDEC_ANA_CON1));
        //printf("classg OFF END AUDDEC_ANA_CON3 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON3,ANA_READ(AUDDEC_ANA_CON3));
        //printf("classg OFF END AUDDEC_ANA_CON11 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON11,ANA_READ(AUDDEC_ANA_CON11));
        //printf("classg OFF END AUDDEC_ANA_CON2 addr:0x%x val:0x%x\r\n",AUDDEC_ANA_CON2,ANA_READ(AUDDEC_ANA_CON2));
    }

    return false;
}


#endif /*HAL_AUDIO_MODULE_ENABLED*/

