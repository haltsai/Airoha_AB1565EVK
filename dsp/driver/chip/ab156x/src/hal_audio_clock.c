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


#include "hal_audio_clock.h"
#include "hal_audio_register.h"
#if 0//1565 clock control on CM4, modify for ab1568
#include "hal_spm.h"
#endif

#ifdef HAL_AUDIO_MODULE_ENABLED

#define HAL_AUDIO_CLOCK_CONTROL

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Variables Declaration //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int16_t afe_clock_afe_counter;
static int16_t afe_clock_i2s_slv_hclk_counter;
static int16_t afe_clock_22m_counter;
static int16_t afe_clock_24m_counter;
static int16_t afe_clock_apll2_counter;
static int16_t afe_clock_apll_counter;
static int16_t afe_clock_adc3_counter;
static int16_t afe_clock_adc2_counter;
static int16_t afe_clock_adc_counter;
static int16_t afe_clock_dac_counter;
static int16_t afe_clock_sine_counter;
static int16_t afe_clock_classg_counter;

static int16_t afe_clock_i2s0_counter;
static int16_t afe_clock_i2s1_counter;
static int16_t afe_clock_i2s2_counter;
static int16_t afe_clock_i2s3_counter;
static int16_t afe_clock_i2s_dma_counter;
static int16_t afe_clock_i2s_dma_irq_counter;
static int16_t afe_clock_adc_hires_counter;
static int16_t afe_clock_hires_sine_counter;
static int16_t afe_clock_adda2_counter;
static int16_t afe_clock_adda6_counter;
static int16_t afe_clock_adda_anc_counter;
static int16_t afe_clock_dac_hires_counter;
static int16_t afe_clock_src_counter;

static volatile int16_t afe_enable_afe_on_counter;
static int16_t afe_enable_clksq_counter;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functiion Declaration //////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void hal_audio_clock_initialize(void)
{

#if AB1568_BRING_UP_DSP_DEFAULT_HW_LOOPBACK

    // MTCMOS_AUDIO_ON TEMP!!!!
#define INFRA_MISC_CFG 0xA2200000
#define INFRA_CFG_MTCMOS3           ((volatile uint32_t*)(INFRA_MISC_CFG + 0x004C))
#define SPM_BASE    (0xA2110000)
#define SPM_AUDIO_PWR_CON                          ((volatile uint32_t*)(SPM_BASE + 0x0210))
#define SPM_AUDIO_G_CONTROL                        ((volatile uint32_t*)(SPM_BASE + 0x0498))
#define SPM_AUDIO_DFX_CH01_CONTROL                 ((volatile uint32_t*)(SPM_BASE + 0x049C))
#define SPM_AUDIO_DFX_CH23_CONTROL                 ((volatile uint32_t*)(SPM_BASE + 0x0500))
#define SPM_AUDIO_DFX_CH45_CONTROL                 ((volatile uint32_t*)(SPM_BASE + 0x0504))
#define SPM_AUDIO_UL1_CONTROL                      ((volatile uint32_t*)(SPM_BASE + 0x0508))
#define SPM_AUDIO_UL2_CONTROL                      ((volatile uint32_t*)(SPM_BASE + 0x050C))
#define SPM_AUDIO_UL3_CONTROL                      ((volatile uint32_t*)(SPM_BASE + 0x0510))
#define SPM_AUDIO_UL4_CONTROL                      ((volatile uint32_t*)(SPM_BASE + 0x0514))
#define SPM_AUDIO_DL_CONTROL                       ((volatile uint32_t*)(SPM_BASE + 0x0518))
#define SPM_AUDIO_AFE_MEM_IF_CONTROL_1             ((volatile uint32_t*)(SPM_BASE + 0x0520))
#define SPM_AUDIO_ASRC1_CONTROL_1                  ((volatile uint32_t*)(SPM_BASE + 0x530))
#define SPM_AUDIO_ASRC2_CONTROL_1                  ((volatile uint32_t*)(SPM_BASE + 0x540))

        //TINFO = " --- AUDIO power on by CM4 API "
        //*AUDIO_PWR_CON = 0x12; sleep
        *SPM_AUDIO_PWR_CON = 0x16; // [2]: pwr_on = 1
        *SPM_AUDIO_PWR_CON = 0x1E; // [3]: pwr_on_2nd = 1

        // --- AUDIO SRAM begin ------
        *SPM_AUDIO_AFE_MEM_IF_CONTROL_1 = 0x01000300;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_ASRC1_CONTROL_1      = 0x01000300;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_ASRC2_CONTROL_1      = 0x01000300;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN

        *SPM_AUDIO_G_CONTROL        = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_DFX_CH01_CONTROL     = 0x01000300;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_DFX_CH23_CONTROL     = 0x01000300;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_DFX_CH45_CONTROL     = 0x01000300;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_UL1_CONTROL      = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_UL2_CONTROL      = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_UL3_CONTROL      = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_UL4_CONTROL      = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN
        *SPM_AUDIO_DL_CONTROL       = 0x01000100;//[24]:MEM_ISO_EN_B [16]:RET2N [8]:RET1N [0]:PGEN

        // --- AUDIO SRAM end ------
        *SPM_AUDIO_PWR_CON = 0xE;   // [4]: clk_dis = 0
        *SPM_AUDIO_PWR_CON = 0x1E;  // [4]: clk_dis = 1
        *SPM_AUDIO_PWR_CON = 0x1C;  // [4]: clk_dis = 1, iso = 0
        *SPM_AUDIO_PWR_CON = 0x1D;  // [0]: rstb = 1
        *SPM_AUDIO_PWR_CON = 0xD;   // [4]: clk_dis = 0

        //turn AUDIO protect_en=0
        *INFRA_CFG_MTCMOS3 = 0x0;
        //wait AUDIO protect ready=0
        //while(*INFRA_CFG_MTCMOS3!=0x0);//For ic

        //enable apll tuner
        AFE_SET_REG(AFE_APLL1_TUNER_CFG, 1, 1);
        //enable apll tuner
        AFE_SET_REG(AFE_APLL2_TUNER_CFG, 1, 1);

    AFE_WRITE(AUDIO_TOP_CON0, AUDIO_TOP_CON0_PDN_ALL_MASK);
    AFE_WRITE(AUDIO_TOP_CON1, AUDIO_TOP_CON1_PDN_ALL_MASK);
#endif



}

int16_t hal_audio_critical_count_16bit(volatile int16_t *counter, int32_t addition)
{
    int16_t value;
    HAL_AUDIO_ENTER_CRITICAL();
    value = (*(volatile int16_t *)counter)+ addition;
    if (value < 0) {
        value = 0;
    }
    (*(volatile int16_t *)counter) = value;
    HAL_AUDIO_EXIT_CRITICAL();
    return value;
}

void hal_audio_clock_enable_afe(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_afe_counter++;
        if (afe_clock_afe_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_AFE_POS, AUDIO_TOP_CON0_PDN_AFE_MASK);
        }
    } else {
        afe_clock_afe_counter--;
        if (afe_clock_afe_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_AFE_POS, AUDIO_TOP_CON0_PDN_AFE_MASK);
        } else if (afe_clock_afe_counter < 0) {
            afe_clock_afe_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_i2s_slave_hclk(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_i2s_slv_hclk_counter++;
        if (afe_clock_i2s_slv_hclk_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_I2S_SLV_HCLK_POS, AUDIO_TOP_CON0_PDN_I2S_SLV_HCLK_MASK);
        }
    } else {
        afe_clock_i2s_slv_hclk_counter--;
        if (afe_clock_i2s_slv_hclk_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_I2S_SLV_HCLK_POS, AUDIO_TOP_CON0_PDN_I2S_SLV_HCLK_MASK);
        } else if (afe_clock_i2s_slv_hclk_counter < 0) {
            afe_clock_i2s_slv_hclk_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}


void hal_audio_clock_enable_22m(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_22m_counter++;
        if (afe_clock_22m_counter == 1) {
            //Setting audio clock, engen 22M
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_22M_POS, AUDIO_TOP_CON0_PDN_22M_MASK);
            //Enable HD engen 22m on
            AFE_SET_REG(AFE_HD_ENGEN_ENABLE, 1 << 0, 1 << 0);
        }
    } else {
        afe_clock_22m_counter--;
        if (afe_clock_22m_counter == 0) {
            AFE_SET_REG(AFE_HD_ENGEN_ENABLE, 0 << 0, 1 << 0);
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_22M_POS, AUDIO_TOP_CON0_PDN_22M_MASK);
        } else if (afe_clock_22m_counter < 0) {
            afe_clock_22m_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_24m(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_24m_counter++;
        if (afe_clock_24m_counter == 1) {
            //Setting audio clock, engen 24M
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_24M_POS, AUDIO_TOP_CON0_PDN_24M_MASK);
            //Enable HD engen 24m on
            AFE_SET_REG(AFE_HD_ENGEN_ENABLE, 1 << 1, 1 << 1);
        }
    } else {
        afe_clock_24m_counter--;
        if (afe_clock_24m_counter == 0) {
            AFE_SET_REG(AFE_HD_ENGEN_ENABLE, 0 << 1, 1 << 1);
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_24M_POS, AUDIO_TOP_CON0_PDN_24M_MASK);
        } else if (afe_clock_24m_counter < 0) {
            afe_clock_24m_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_apll2(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_apll2_counter++;
        if (afe_clock_apll2_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_APLL2_TUNER_POS, AUDIO_TOP_CON0_PDN_APLL2_TUNER_MASK);
        }
    } else {
        afe_clock_apll2_counter--;
        if (afe_clock_apll2_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_APLL2_TUNER_POS, AUDIO_TOP_CON0_PDN_APLL2_TUNER_MASK);
        } else if (afe_clock_apll2_counter < 0) {
            afe_clock_apll2_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_apll(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_apll_counter++;
        if (afe_clock_apll_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_APLL_TUNER_POS, AUDIO_TOP_CON0_PDN_APLL_TUNER_MASK);
        }
    } else {
        afe_clock_apll_counter--;
        if (afe_clock_apll_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_APLL_TUNER_POS, AUDIO_TOP_CON0_PDN_APLL_TUNER_MASK);
        } else if (afe_clock_apll_counter < 0) {
            afe_clock_apll_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_adc3(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_adc3_counter++;
        if (afe_clock_adc3_counter == 1) {
            hal_audio_clock_enable_adc(enable);
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_ADC3_POS, AUDIO_TOP_CON0_PDN_ADC3_MASK);
            hal_audio_clock_enable_adda6(enable);
        }
    } else {
        afe_clock_adc3_counter--;
        if (afe_clock_adc3_counter == 0) {
            hal_audio_clock_enable_adda6(enable);
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_ADC3_POS, AUDIO_TOP_CON0_PDN_ADC3_MASK);
            hal_audio_clock_enable_adc(enable);
        } else if (afe_clock_adc3_counter < 0) {
            afe_clock_adc3_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_adc2(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_adc2_counter++;
        if (afe_clock_adc2_counter == 1) {
            hal_audio_clock_enable_adc(enable);
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_ADC2_POS, AUDIO_TOP_CON0_PDN_ADC2_MASK);
            hal_audio_clock_enable_adda2(enable);
        }
    } else {
        afe_clock_adc2_counter--;
        if (afe_clock_adc2_counter == 0) {
            hal_audio_clock_enable_adda2(enable);
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_ADC2_POS, AUDIO_TOP_CON0_PDN_ADC2_MASK);
            hal_audio_clock_enable_adc(enable);
        } else if (afe_clock_adc2_counter < 0) {
            afe_clock_adc2_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_adc(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_adc_counter++;
        if (afe_clock_adc_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_ADC_POS, AUDIO_TOP_CON0_PDN_ADC_MASK);
        }
    } else {
        afe_clock_adc_counter--;
        if (afe_clock_adc_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_ADC_POS, AUDIO_TOP_CON0_PDN_ADC_MASK);
        } else if (afe_clock_adc_counter < 0) {
            afe_clock_adc_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_dac(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_dac_counter++;
        if (afe_clock_dac_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON0, (0<<AUDIO_TOP_CON0_PDN_DAC_POS)|(0<<AUDIO_TOP_CON0_PDN_DAC_PREDIS_POS), AUDIO_TOP_CON0_PDN_DAC_MASK|AUDIO_TOP_CON0_PDN_DAC_PREDIS_MASK);
        }
    } else {
        afe_clock_dac_counter--;
        if (afe_clock_dac_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON0, (1<<AUDIO_TOP_CON0_PDN_DAC_POS)|(1<<AUDIO_TOP_CON0_PDN_DAC_PREDIS_POS), AUDIO_TOP_CON0_PDN_DAC_MASK|AUDIO_TOP_CON0_PDN_DAC_PREDIS_MASK);
        } else if (afe_clock_dac_counter < 0) {
            afe_clock_dac_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_sine(bool enable)
{
    //Sine generator
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_sine_counter++;
        if (afe_clock_sine_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_TML_POS, AUDIO_TOP_CON0_PDN_TML_MASK);
        }
    } else {
        afe_clock_sine_counter--;
        if (afe_clock_sine_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_TML_POS, AUDIO_TOP_CON0_PDN_TML_MASK);
        } else if (afe_clock_sine_counter < 0) {
            afe_clock_sine_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_classg(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_classg_counter++;
        if (afe_clock_classg_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_CLASSG_POS, AUDIO_TOP_CON0_PDN_CLASSG_MASK);
        }
    } else {
        afe_clock_classg_counter--;
        if (afe_clock_classg_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_CLASSG_POS, AUDIO_TOP_CON0_PDN_CLASSG_MASK);
        } else if (afe_clock_classg_counter < 0) {
            afe_clock_classg_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_i2s0(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_i2s0_counter++;
        if (afe_clock_i2s0_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S0_POS, AUDIO_TOP_CON1_PDN_I2S0_MASK);
        }
    } else {
        afe_clock_i2s0_counter--;
        if (afe_clock_i2s0_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_I2S0_POS, AUDIO_TOP_CON1_PDN_I2S0_MASK);
        } else if (afe_clock_i2s0_counter < 0) {
            afe_clock_i2s0_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_i2s1(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_i2s1_counter++;
        if (afe_clock_i2s1_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S1_POS, AUDIO_TOP_CON1_PDN_I2S1_MASK);
        }
    } else {
        afe_clock_i2s1_counter--;
        if (afe_clock_i2s1_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_I2S1_POS, AUDIO_TOP_CON1_PDN_I2S1_MASK);
        } else if (afe_clock_i2s1_counter < 0) {
            afe_clock_i2s1_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_i2s2(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_i2s2_counter++;
        if (afe_clock_i2s2_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S2_POS, AUDIO_TOP_CON1_PDN_I2S2_MASK);
        }
    } else {
        afe_clock_i2s2_counter--;
        if (afe_clock_i2s2_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_I2S2_POS, AUDIO_TOP_CON1_PDN_I2S2_MASK);
        } else if (afe_clock_i2s2_counter < 0) {
            afe_clock_i2s2_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_i2s3(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_i2s3_counter++;
        if (afe_clock_i2s3_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S3_POS, AUDIO_TOP_CON1_PDN_I2S3_MASK);
        }
    } else {
        afe_clock_i2s3_counter--;
        if (afe_clock_i2s3_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_I2S3_POS, AUDIO_TOP_CON1_PDN_I2S3_MASK);
        } else if (afe_clock_i2s3_counter < 0) {
            afe_clock_i2s3_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_i2s_dma(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_i2s_dma_counter++;
        if (afe_clock_i2s_dma_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S_DMA_POS, AUDIO_TOP_CON1_PDN_I2S_DMA_MASK);
        }
    } else {
        afe_clock_i2s_dma_counter--;
        if (afe_clock_i2s_dma_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_I2S_DMA_POS, AUDIO_TOP_CON1_PDN_I2S_DMA_MASK);
        } else if (afe_clock_i2s_dma_counter < 0) {
            afe_clock_i2s_dma_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_i2s_dma_irq(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_i2s_dma_irq_counter++;
        if (afe_clock_i2s_dma_irq_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_I2S_DMA_IRQ_POS, AUDIO_TOP_CON1_PDN_I2S_DMA_IRQ_MASK);
        }
    } else {
        afe_clock_i2s_dma_irq_counter--;
        if (afe_clock_i2s_dma_irq_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_I2S_DMA_IRQ_POS, AUDIO_TOP_CON1_PDN_I2S_DMA_IRQ_MASK);
        } else if (afe_clock_i2s_dma_irq_counter < 0) {
            afe_clock_i2s_dma_irq_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_adc_hires(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_adc_hires_counter++;
        if (afe_clock_adc_hires_counter == 1) {
            #if 0//def HAL_AUDIO_CLOCK_CONTROL
            hal_clock_enable(HAL_CLOCK_CG_AUD_UPLINK);
            #endif
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_ADC_HIRES_POS, AUDIO_TOP_CON1_PDN_ADC_HIRES_MASK);
        }
    } else {
        afe_clock_adc_hires_counter--;
        if (afe_clock_adc_hires_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_ADC_HIRES_POS, AUDIO_TOP_CON1_PDN_ADC_HIRES_MASK);
            #if 0//def HAL_AUDIO_CLOCK_CONTROL
            hal_clock_disable(HAL_CLOCK_CG_AUD_UPLINK);
            #endif
        } else if (afe_clock_adc_hires_counter < 0) {
            afe_clock_adc_hires_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_sine_hire(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_hires_sine_counter++;
        if (afe_clock_hires_sine_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_POS, AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_MASK);
        }
    } else {
        afe_clock_hires_sine_counter--;
        if (afe_clock_hires_sine_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_POS, AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_MASK);
        } else if (afe_clock_hires_sine_counter < 0) {
            afe_clock_hires_sine_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_adda2(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_adda2_counter++;
        if (afe_clock_adda2_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_ADDA2_POS, AUDIO_TOP_CON1_PDN_ADDA2_MASK);
        }
    } else {
        afe_clock_adda2_counter--;
        if (afe_clock_adda2_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_ADDA2_POS, AUDIO_TOP_CON1_PDN_ADDA2_MASK);
        } else if (afe_clock_adda2_counter < 0) {
            afe_clock_adda2_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_adda6(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_adda6_counter++;
        if (afe_clock_adda6_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_ADDA6_POS, AUDIO_TOP_CON1_PDN_ADDA6_MASK);
        }
    } else {
        afe_clock_adda6_counter--;
        if (afe_clock_adda6_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_ADDA6_POS, AUDIO_TOP_CON1_PDN_ADDA6_MASK);
        } else if (afe_clock_adda6_counter < 0) {
            afe_clock_adda6_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_adda_anc(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_adda_anc_counter++;
        if (afe_clock_adda_anc_counter == 1) {
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_ADDA_ANC_POS, AUDIO_TOP_CON1_PDN_ADDA_ANC_MASK);
        }
    } else {
        afe_clock_adda_anc_counter--;
        if (afe_clock_adda_anc_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_ADDA_ANC_POS, AUDIO_TOP_CON1_PDN_ADDA_ANC_MASK);
        } else if (afe_clock_adda_anc_counter < 0) {
            afe_clock_adda_anc_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_dac_hires(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_dac_hires_counter++;
        if (afe_clock_dac_hires_counter == 1) {
            #if 0//def HAL_AUDIO_CLOCK_CONTROL
            hal_clock_enable(HAL_CLOCK_CG_AUD_DWLINK);
            #endif
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_DAC_HIRES_POS, AUDIO_TOP_CON1_PDN_DAC_HIRES_MASK);
        }
    } else {
        afe_clock_dac_hires_counter--;
        if (afe_clock_dac_hires_counter == 0) {
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_DAC_HIRES_POS, AUDIO_TOP_CON1_PDN_DAC_HIRES_MASK);
            #if 0//def HAL_AUDIO_CLOCK_CONTROL
            hal_clock_disable(HAL_CLOCK_CG_AUD_DWLINK);
            #endif
        } else if (afe_clock_dac_hires_counter < 0) {
            afe_clock_dac_hires_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_src(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_clock_src_counter++;
        if (afe_clock_src_counter == 1) {
            #if 0//def HAL_AUDIO_CLOCK_CONTROL
            hal_clock_enable(HAL_CLOCK_CG_AUD_GPSRC);
            #endif
            AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_POS, AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_MASK);
        }
    } else {
        afe_clock_src_counter--;
        if (afe_clock_src_counter == 0) {
            HAL_AUDIO_DELAY_US(10);
            AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_POS, AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_MASK);
            #if 0//def HAL_AUDIO_CLOCK_CONTROL
            hal_clock_disable(HAL_CLOCK_CG_AUD_GPSRC);
            #endif
        } else if (afe_clock_src_counter < 0) {
            afe_clock_src_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
}

void hal_audio_clock_enable_src1(bool enable)
{
    if (enable) {
        AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_ASRC1_POS, AUDIO_TOP_CON1_PDN_ASRC1_MASK);
    } else {
        U32 loop_count = 0;
        #ifndef AIR_HWSRC_RX_TRACKING_ENABLE
        while((AFE_GET_REG(ASM_OUT_BUF_MON0) & 0xFF) != 0XF1){
            if (loop_count >= 1000 && loop_count % 100 == 0){
                HAL_AUDIO_LOG_INFO("DSP - src1 28bit loop_count:%d, 0x1018:0x%x, 0x101C:0x%x,  0x1160:0x%x, 0x1170:0x%x, 0x11F8:0x%x, 0x11FC:0x%x, 0x0010:0x%x,0x0FDC:0x%x", 9, loop_count,
                AFE_GET_REG(MEM_ASRC_TOP_MON0), AFE_GET_REG(MEM_ASRC_TOP_MON1), AFE_GET_REG(ASM_CH01_OBUF_WRPNT), AFE_GET_REG(ASM_CH01_OBUF_RDPNT),
                AFE_GET_REG(ASM_OUT_BUF_MON0), AFE_GET_REG(ASM_OUT_BUF_MON1), AFE_GET_REG(AFE_DAC_CON0), AFE_GET_REG(AFE_AUDIO_BT_SYNC_MON2));
                HAL_AUDIO_ASSERT(false);
            }
            loop_count++;
            HAL_AUDIO_DELAY_US(10);
        }
        #endif
        HAL_AUDIO_LOG_INFO(" SRC1 JUMP loop_count = %d", 1, loop_count);

        AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_ASRC1_POS, AUDIO_TOP_CON1_PDN_ASRC1_MASK);
        HAL_AUDIO_DELAY_US(5);
        AFE_SET_REG(MEM_ASRC_TOP_CON0, 1<<MEM_ASRC_TOP_CON0_MASM1_RST_POS, MEM_ASRC_TOP_CON0_MASM1_RST_MASK);
        AFE_SET_REG(MEM_ASRC_TOP_CON0, 0<<MEM_ASRC_TOP_CON0_MASM1_RST_POS, MEM_ASRC_TOP_CON0_MASM1_RST_MASK);
    }
    HAL_AUDIO_DELAY_US(5);
}

void hal_audio_clock_enable_src2(bool enable)
{
    if (enable) {
        AFE_SET_REG(AUDIO_TOP_CON1, 0<<AUDIO_TOP_CON1_PDN_ASRC2_POS, AUDIO_TOP_CON1_PDN_ASRC2_MASK);
    } else {
        U32 loop_count = 0;
        #ifndef AIR_HWSRC_RX_TRACKING_ENABLE
        while((AFE_GET_REG(ASM2_OUT_BUF_MON0) & 0xFF) != 0XF1){
            if (loop_count >= 1000 && loop_count % 100 == 0){
                HAL_AUDIO_LOG_INFO("DSP - src2 28bit loop_count:%d, 0x1018:0x%x, 0x101C:0x%x,  0x1260:0x%x, 0x1270:0x%x, 0x12F8:0x%x, 0x12FC:0x%x, 0x0010:0x%x,0x0FDC:0x%x", 9, loop_count,
                AFE_GET_REG(MEM_ASRC_TOP_MON0), AFE_GET_REG(MEM_ASRC_TOP_MON1), AFE_GET_REG(ASM2_CH01_OBUF_WRPNT), AFE_GET_REG(ASM2_CH01_OBUF_RDPNT),
                AFE_GET_REG(ASM2_OUT_BUF_MON0), AFE_GET_REG(ASM2_OUT_BUF_MON1), AFE_GET_REG(AFE_DAC_CON0), AFE_GET_REG(AFE_AUDIO_BT_SYNC_MON2));
                HAL_AUDIO_ASSERT(false);
            }
            loop_count++;
            HAL_AUDIO_DELAY_US(10);
        }
        #endif
        HAL_AUDIO_LOG_INFO(" SRC2 JUMP loop_count = %d", 1, loop_count);

        AFE_SET_REG(AUDIO_TOP_CON1, 1<<AUDIO_TOP_CON1_PDN_ASRC2_POS, AUDIO_TOP_CON1_PDN_ASRC2_MASK);
        HAL_AUDIO_DELAY_US(5);
        AFE_SET_REG(MEM_ASRC_TOP_CON0, 1<<MEM_ASRC_TOP_CON0_MASM2_RST_POS, MEM_ASRC_TOP_CON0_MASM2_RST_MASK);
        AFE_SET_REG(MEM_ASRC_TOP_CON0, 0<<MEM_ASRC_TOP_CON0_MASM2_RST_POS, MEM_ASRC_TOP_CON0_MASM2_RST_MASK);
    }
        HAL_AUDIO_DELAY_US(5);
}

//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++


void hal_audio_afe_set_enable(bool enable)
{
    int16_t afe_on_cnt;

    if (enable) {
        HAL_AUDIO_ENTER_CRITICAL();
        afe_on_cnt = hal_audio_critical_count_16bit(&afe_enable_afe_on_counter, 1);
        HAL_AUDIO_EXIT_CRITICAL();
        if (afe_on_cnt == 1) {
            #if 0//def HAL_AUDIO_CLOCK_CONTROL
            hal_clock_enable(HAL_CLOCK_CG_AUD_INTBUS);
            //Enable SPM
            spm_control_mtcmos_internal(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_ENABLE);
            //Enable SPM S1
            spm_audio_lowpower_setting(SPM_STATE1,SPM_ENABLE);
            #endif
            //Default clock setting
            AFE_WRITE(AUDIO_TOP_CON0, AUDIO_TOP_CON0_PDN_ALL_MASK);
            AFE_WRITE(AUDIO_TOP_CON1, AUDIO_TOP_CON1_PDN_ALL_MASK);
            //Enable clock
            hal_audio_clock_enable_afe(enable);
            HAL_AUDIO_LOG_INFO("[AFE] enable AFE_DAC_CON0", 0);
            AFE_SET_REG(AFE_DAC_CON0, 1<<AFE_DAC_CON0_AFE_ON_POS, AFE_DAC_CON0_AFE_ON_MASK);
        }
    } else {
        HAL_AUDIO_ENTER_CRITICAL();
        afe_on_cnt = hal_audio_critical_count_16bit(&afe_enable_afe_on_counter, -1);
        HAL_AUDIO_EXIT_CRITICAL();
        if (afe_on_cnt == 0) {
            HAL_AUDIO_LOG_INFO("[AFE] disable AFE_DAC_CON0", 0);
            AFE_SET_REG(AFE_DAC_CON0, 0<<AFE_DAC_CON0_AFE_ON_POS, AFE_DAC_CON0_AFE_ON_MASK);
            //Disable clock
            hal_audio_clock_enable_afe(enable);
            #if 0
            //Disable SPM S1
            spm_audio_lowpower_setting(SPM_STATE1,SPM_DISABLE);
            //Disable SPM
            spm_control_mtcmos_internal(SPM_MTCMOS_AUDIO, SPM_MTCMOS_PWR_DISABLE);
            hal_clock_disable(HAL_CLOCK_CG_AUD_INTBUS);
            #endif
        } else if (afe_on_cnt < 0) {
            HAL_AUDIO_LOG_ERROR("DSP - Error Hal Audio AFE control cnt:%d !", 1, afe_on_cnt);
            HAL_AUDIO_ENTER_CRITICAL();
            afe_on_cnt = 0;
            afe_enable_afe_on_counter = afe_on_cnt;
            HAL_AUDIO_EXIT_CRITICAL();
        }
    }
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio AFE control:%d, cnt:%d", 2, enable, afe_on_cnt);
}

uint32_t hal_audio_afe_get_counter(void)
{
    return (uint32_t)afe_enable_afe_on_counter;
}


void hal_audio_afe_enable_clksq(bool enable)
{
    HAL_AUDIO_ENTER_CRITICAL();
    if (enable) {
        afe_enable_clksq_counter++;
        if (afe_enable_clksq_counter == 1) {
            ANA_SET_REG((AUDDEC_ANA_CON10),
                        ((1<<AUDDEC_ANA_CON10_1P5V_LDO_SEL_POS)|(2<<AUDDEC_ANA_CON10_0P8V_LDO_SEL_POS)|(3<<AUDDEC_ANA_CON10_GLB_RSV_POS)),
                        (AUDDEC_ANA_CON10_1P5V_LDO_SEL_MASK|AUDDEC_ANA_CON10_0P8V_LDO_SEL_MASK|AUDDEC_ANA_CON10_GLB_RSV_MASK));
            ANA_SET_REG((AUDDEC_ANA_CON9),
                        ((0<<AUDDEC_ANA_CON9_26M_CLK_SRC_POS)|(0<<AUDDEC_ANA_CON9_13M_DIV_POS)|(2<<AUDDEC_ANA_CON9_13M_D5NS_DELAY_POS)|(0<<AUDDEC_ANA_CON9_13M_D5NS_INV_POS)|(0<<AUDDEC_ANA_CON9_13M_DUTY_POS)|(0<<AUDDEC_ANA_CON9_CLKGEN_RSV_POS)),
                        (AUDDEC_ANA_CON9_26M_CLK_SRC_MASK|AUDDEC_ANA_CON9_13M_DIV_MASK|AUDDEC_ANA_CON9_13M_D5NS_DELAY_MASK|AUDDEC_ANA_CON9_13M_D5NS_INV_MASK|AUDDEC_ANA_CON9_13M_DUTY_MASK|AUDDEC_ANA_CON9_CLKGEN_RSV_MASK));
            ANA_SET_REG(AUDDEC_ANA_CON9, 1<<AUDDEC_ANA_CON9_DECODER_RST_POS, AUDDEC_ANA_CON9_DECODER_RST_MASK);             //Reset clock driver
        }
    } else {
        afe_enable_clksq_counter--;
        if (afe_enable_clksq_counter == 0) {
            ANA_SET_REG(AUDDEC_ANA_CON9 , 0, (AUDDEC_ANA_CON9_26M_CLK_SRC_MASK|AUDDEC_ANA_CON9_13M_DIV_MASK|AUDDEC_ANA_CON9_13M_D5NS_DELAY_MASK|AUDDEC_ANA_CON9_13M_D5NS_INV_MASK|AUDDEC_ANA_CON9_13M_DUTY_MASK|AUDDEC_ANA_CON9_CLKGEN_RSV_MASK));
            ANA_SET_REG(AUDDEC_ANA_CON9, 0<<AUDDEC_ANA_CON9_DECODER_RST_POS, AUDDEC_ANA_CON9_DECODER_RST_MASK);             //Reset clock driver
            ANA_SET_REG(AUDDEC_ANA_CON10, 0, (AUDDEC_ANA_CON10_1P5V_LDO_SEL_MASK|AUDDEC_ANA_CON10_0P8V_LDO_SEL_MASK|AUDDEC_ANA_CON10_GLB_RSV_MASK));
        } else if (afe_enable_clksq_counter < 0) {
            afe_enable_clksq_counter = 0;
        }
    }
    HAL_AUDIO_EXIT_CRITICAL();
    HAL_AUDIO_LOG_INFO("DSP - Hal Audio AFE CLKSQ control:%d, cnt:%d", 2, enable, afe_enable_clksq_counter);
}


void hal_audio_clock_set_dac(bool enable)
{
    //Workaround:Prevent state machine is unfinished
    HAL_AUDIO_DELAY_US(10);

    hal_audio_clock_enable_dac(enable);
    //hal_audio_clock_enable_adda2(enable);
    //hal_audio_clock_enable_adda6(enable);

    //Workaround:Prevent state machine is unfinished
    HAL_AUDIO_DELAY_US(10);

}



#endif /*HAL_AUDIO_MODULE_ENABLED*/
