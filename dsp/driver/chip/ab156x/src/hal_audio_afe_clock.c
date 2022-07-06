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

#include "hal_audio.h"

#ifdef HAL_AUDIO_MODULE_ENABLED

#include "hal_audio_afe_define.h"
#include "hal_audio_afe_clock.h"
#include "hal_nvic.h"
#include "hal_log.h"

static int16_t aud_apll_1_cntr;
static int16_t aud_apll_2_cntr;
static int16_t aud_apll_1_tuner_cntr;
static int16_t aud_apll_2_tuner_cntr;
static int16_t aud_apll_1_tuner_cntr_2;
static int16_t aud_apll_2_tuner_cntr_2;
static int16_t aud_afe_clk_cntr;
static int16_t aud_i2s0_clk_cntr;
static int16_t aud_i2s1_clk_cntr;
static int16_t aud_i2s2_clk_cntr;
static int16_t aud_i2s3_clk_cntr;
static int16_t aud_adc_clk_cntr;
static int16_t aud_adc2_clk_cntr;
static int16_t aud_adc6_clk_cntr;
static int16_t aud_classg_clk_cntr;
static int16_t aud_dac_clk_cntr;
static int16_t aud_asrc_clk_cntr;

void afe_clock_variable_init(void)
{
    aud_apll_1_cntr = 0;
    aud_apll_2_cntr = 0;
    aud_apll_1_tuner_cntr = 0;
    aud_apll_2_tuner_cntr = 0;
    aud_afe_clk_cntr = 0;
    aud_i2s0_clk_cntr = 0;
    aud_i2s1_clk_cntr = 0;
    aud_i2s2_clk_cntr = 0;
    aud_i2s3_clk_cntr = 0;
    aud_adc_clk_cntr = 0;
    aud_adc2_clk_cntr = 0;
    aud_adc6_clk_cntr = 0;
    aud_classg_clk_cntr = 0;
    aud_dac_clk_cntr = 0;
    //init clk/pdn reg. should be set until correct setting with no power issue
    //AFE_WRITE(AUDIO_TOP_CON1, 0x709b000f);
    //AFE_WRITE(AUDIO_TOP_CON0, 0x3f0cc304);

}

void hal_audio_afe_clock_on(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_afe_clk_cntr++;
    if (aud_afe_clk_cntr == 1) {
        /*afe clk*/
        //AFE_SET_REG(AUDIO_TOP_CON0, 0, 0x4);
        AFE_SET_REG(AUDIO_TOP_CON0, 0<<AUDIO_TOP_CON0_PDN_AFE_POS, AUDIO_TOP_CON0_PDN_AFE_MASK);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void hal_audio_afe_clock_off(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_afe_clk_cntr--;
    if (aud_afe_clk_cntr == 0) {
        /* pdn afe clk*/
        #if 0
        // Workaround : Keep afe clock for volume setting & noise issue
        log_hal_msgid_info("disable AUDIO_TOP_CON0: PDN_AFE bit\n", 0);
        AFE_SET_REG(AUDIO_TOP_CON0, 1<<AUDIO_TOP_CON0_PDN_AFE_POS, AUDIO_TOP_CON0_PDN_AFE_MASK); // Power down all AFE clock : Main clock
        #endif
    } else if (aud_afe_clk_cntr < 0) {
        aud_afe_clk_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

/*DL*/
void afe_dac_clock_on(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_dac_clk_cntr++;
    if (aud_dac_clk_cntr == 1) {
        AFE_SET_REG(AUDIO_TOP_CON0, (0<<AUDIO_TOP_CON0_PDN_DAC_POS)|(0<<AUDIO_TOP_CON0_PDN_DAC_PREDIS_POS) , AUDIO_TOP_CON0_PDN_DAC_MASK|AUDIO_TOP_CON0_PDN_DAC_PREDIS_MASK);
        afe_adc2_clock_on();
        afe_adc6_clock_on();
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_dac_clock_off(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_dac_clk_cntr--;
    if (aud_dac_clk_cntr == 0) {
        AFE_SET_REG(AUDIO_TOP_CON0, (1<<AUDIO_TOP_CON0_PDN_DAC_POS)|(1<<AUDIO_TOP_CON0_PDN_DAC_PREDIS_POS)  , AUDIO_TOP_CON0_PDN_DAC_MASK|AUDIO_TOP_CON0_PDN_DAC_PREDIS_MASK);
        afe_adc2_clock_off();
        afe_adc6_clock_off();
    } else if (aud_dac_clk_cntr < 0) {
        aud_dac_clk_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

int16_t afe_get_dac_clock_status(void)
{
    return aud_dac_clk_cntr;
}
/*VUL1*/
void afe_adc_clock_on(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_adc_clk_cntr++;
    if (aud_adc_clk_cntr == 1) {
        AFE_SET_REG(AUDIO_TOP_CON0, 0 << AUDIO_TOP_CON0_PDN_ADC_POS , AUDIO_TOP_CON0_PDN_ADC_MASK);
        afe_adc2_clock_on();
        afe_adc6_clock_on();
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_adc_clock_off(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_adc_clk_cntr--;
    if (aud_adc_clk_cntr == 0) {
        /* pdn adc clk */
        AFE_SET_REG(AUDIO_TOP_CON0, 1 << AUDIO_TOP_CON0_PDN_ADC_POS, AUDIO_TOP_CON0_PDN_ADC_MASK);
        afe_adc2_clock_off();
        afe_adc6_clock_off();
    } else if (aud_adc_clk_cntr < 0) {
        aud_adc_clk_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

/*VUL2*/
void afe_adc2_clock_on(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_adc2_clk_cntr++;
    if (aud_adc2_clk_cntr == 1) {
        AFE_SET_REG(AUDIO_TOP_CON1, 0 << AUDIO_TOP_CON1_PDN_ADDA2_POS , AUDIO_TOP_CON1_PDN_ADDA2_MASK);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_adc2_clock_off(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_adc2_clk_cntr--;
    if (aud_adc2_clk_cntr == 0) {
        /* pdn adc2 clk */
        AFE_SET_REG(AUDIO_TOP_CON1, 1 << AUDIO_TOP_CON1_PDN_ADDA2_POS, AUDIO_TOP_CON1_PDN_ADDA2_MASK);
    } else if (aud_adc2_clk_cntr < 0) {
        aud_adc2_clk_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_adc6_clock_on(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_adc6_clk_cntr++;
    if (aud_adc6_clk_cntr == 1) {
        AFE_SET_REG(AUDIO_TOP_CON1, 0 << AUDIO_TOP_CON1_PDN_ADDA6_POS , AUDIO_TOP_CON1_PDN_ADDA6_MASK);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_adc6_clock_off(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_adc6_clk_cntr--;
    if (aud_adc6_clk_cntr == 0) {
        /* pdn adc2 clk */
        AFE_SET_REG(AUDIO_TOP_CON1, 1 << AUDIO_TOP_CON1_PDN_ADDA6_POS, AUDIO_TOP_CON1_PDN_ADDA6_MASK);
    } else if (aud_adc6_clk_cntr < 0) {
        aud_adc6_clk_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_classg_clock_on(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_classg_clk_cntr++;
    if (aud_classg_clk_cntr == 1) {
        AFE_SET_REG(AUDIO_TOP_CON0, 0 << AUDIO_TOP_CON0_PDN_CLASSG_POS , AUDIO_TOP_CON0_PDN_CLASSG_MASK);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_classg_clock_off(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_classg_clk_cntr--;
    if (aud_classg_clk_cntr == 0) {
        /* pdn adc2 clk */
        AFE_SET_REG(AUDIO_TOP_CON0, 1 << AUDIO_TOP_CON0_PDN_CLASSG_POS, AUDIO_TOP_CON0_PDN_CLASSG_MASK);
    } else if (aud_classg_clk_cntr < 0) {
        aud_classg_clk_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_dac_hires_clock(bool pdn)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    AFE_SET_REG(AUDIO_TOP_CON1, (pdn<<AUDIO_TOP_CON1_PDN_DAC_HIRES_POS), AUDIO_TOP_CON1_PDN_DAC_HIRES_MASK);
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_adc_hires_clock(bool pdn)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    AFE_SET_REG(AUDIO_TOP_CON1, (pdn<<AUDIO_TOP_CON1_PDN_ADC_HIRES_POS) | (pdn<<AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_POS), AUDIO_TOP_CON1_PDN_ADC_HIRES_MASK|AUDIO_TOP_CON1_PDN_ADC_HIRES_TML_MASK);
    hal_nvic_restore_interrupt_mask(mask);
}
#if 0
static void afe_top_pdn_i2s(bool pdn, afe_i2s_num_t i2s_num)
{
    if (pdn) {// power down
        switch(i2s_num) {
            case AFE_I2S0:
                aud_i2s0_clk_cntr--;
                if (aud_i2s0_clk_cntr == 0) {
                    AFE_SET_REG(AUDIO_TOP_CON1, 0x1<<AUDIO_TOP_CON1_PDN_I2S0_POS, AUDIO_TOP_CON1_PDN_I2S0_MASK);
                } else if (aud_i2s0_clk_cntr < 0) {
                    aud_i2s0_clk_cntr = 0;
                }
                break;
            case AFE_I2S1:
                aud_i2s1_clk_cntr--;
                if (aud_i2s1_clk_cntr == 0) {
                    AFE_SET_REG(AUDIO_TOP_CON1, 0x1<<AUDIO_TOP_CON1_PDN_I2S1_POS, AUDIO_TOP_CON1_PDN_I2S1_MASK);
                } else if (aud_i2s1_clk_cntr < 0) {
                    aud_i2s1_clk_cntr = 0;
                }
                break;
            case AFE_I2S2:
                aud_i2s2_clk_cntr--;
                if (aud_i2s2_clk_cntr == 0) {
                    AFE_SET_REG(AUDIO_TOP_CON1, 0x1<<AUDIO_TOP_CON1_PDN_I2S2_POS, AUDIO_TOP_CON1_PDN_I2S2_MASK);
                } else if (aud_i2s2_clk_cntr < 0) {
                    aud_i2s2_clk_cntr = 0;
                }
                break;
            case AFE_I2S3:
                aud_i2s3_clk_cntr--;
                if (aud_i2s3_clk_cntr == 0) {
                    AFE_SET_REG(AUDIO_TOP_CON1, 0x1<<AUDIO_TOP_CON1_PDN_I2S3_POS, AUDIO_TOP_CON1_PDN_I2S3_MASK);
                } else if (aud_i2s3_clk_cntr < 0) {
                    aud_i2s3_clk_cntr = 0;
                }
                break;
            default:
                break;
        }
    }
    else {// power on
        switch(i2s_num) {
            case AFE_I2S0:
                aud_i2s0_clk_cntr++;
                if (aud_i2s0_clk_cntr == 1) {
                    AFE_SET_REG(AUDIO_TOP_CON1, 0x0<<AUDIO_TOP_CON1_PDN_I2S0_POS, AUDIO_TOP_CON1_PDN_I2S0_MASK);
                }
                break;
            case AFE_I2S1:
                aud_i2s1_clk_cntr++;
                if (aud_i2s1_clk_cntr == 1) {
                    AFE_SET_REG(AUDIO_TOP_CON1, 0x0<<AUDIO_TOP_CON1_PDN_I2S1_POS, AUDIO_TOP_CON1_PDN_I2S1_MASK);
                }
                break;
            case AFE_I2S2:
                aud_i2s2_clk_cntr++;
                if (aud_i2s2_clk_cntr == 1) {
                    AFE_SET_REG(AUDIO_TOP_CON1, 0x0<<AUDIO_TOP_CON1_PDN_I2S2_POS, AUDIO_TOP_CON1_PDN_I2S2_MASK);
                }
                break;
            case AFE_I2S3:
                aud_i2s3_clk_cntr++;
                if (aud_i2s3_clk_cntr == 1) {
                    AFE_SET_REG(AUDIO_TOP_CON1, 0x0<<AUDIO_TOP_CON1_PDN_I2S3_POS, AUDIO_TOP_CON1_PDN_I2S3_MASK);
                }
                break;
            default:
                break;
        }
    }
}

void hal_audio_afe_i2s_clock_on(afe_i2s_num_t i2s_num)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    afe_top_pdn_i2s(false, i2s_num);
    hal_nvic_restore_interrupt_mask(mask);
}

void hal_audio_afe_i2s_clock_off(afe_i2s_num_t i2s_num)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    afe_top_pdn_i2s(true, i2s_num);
    hal_nvic_restore_interrupt_mask(mask);
}
#endif
void afe_apll1tuner_clock_on(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_apll_1_tuner_cntr++;
    if (aud_apll_1_tuner_cntr == 1) {
        //Setting audio clock, apll1 tuner
        AFE_SET_REG(AUDIO_TOP_CON0, 0 << AUDIO_TOP_CON0_PDN_APLL_TUNER_POS , AUDIO_TOP_CON0_PDN_APLL_TUNER_MASK);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_apll1tuner_clock_off(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_apll_1_tuner_cntr--;
    if (aud_apll_1_tuner_cntr == 0) {
        AFE_SET_REG(AUDIO_TOP_CON0, 1 << AUDIO_TOP_CON0_PDN_APLL_TUNER_POS, AUDIO_TOP_CON0_PDN_APLL_TUNER_MASK);
    } else if (aud_apll_1_tuner_cntr < 0) {
        aud_apll_1_tuner_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_apll2tuner_clock_on(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_apll_2_tuner_cntr++;
    if (aud_apll_2_tuner_cntr == 1) {
        //Setting audio clock, apll2 tuner
        AFE_SET_REG(AUDIO_TOP_CON0, 0 << AUDIO_TOP_CON0_PDN_APLL2_TUNER_POS, AUDIO_TOP_CON0_PDN_APLL2_TUNER_MASK);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_apll2tuner_clock_off(void)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    aud_apll_2_tuner_cntr--;
    if (aud_apll_2_tuner_cntr == 0) {
        AFE_SET_REG(AUDIO_TOP_CON0, 1 << AUDIO_TOP_CON0_PDN_APLL2_TUNER_POS, AUDIO_TOP_CON0_PDN_APLL2_TUNER_MASK);
    } else if (aud_apll_2_tuner_cntr < 0) {
        aud_apll_2_tuner_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

/*[ask apll tuning method]*/
afe_apll_source_t afe_get_apll_by_samplerate(uint32_t samplerate)
{
    if (samplerate == 176400 || samplerate == 88200 || samplerate == 44100 || samplerate == 22050 || samplerate == 11025) {
        return AFE_APLL1;
    }
    else {
        return AFE_APLL2;
    }
}

void afe_enable_apll_by_samplerate(uint32_t samplerate)
{
    switch (afe_get_apll_by_samplerate(samplerate)) {
        case AFE_APLL1:
            aud_apll_1_cntr++;
            if (aud_apll_1_cntr == 1) {
                afe_apll1_enable(true);
                afe_apll1tuner_clock_on();
            }
            break;
        case AFE_APLL2:
            aud_apll_2_cntr++;
            if (aud_apll_2_cntr == 1) {
                afe_apll2_enable(true);
                afe_apll2tuner_clock_on();
            }
            break;
        default:
            break;
    }
}

void afe_disable_apll_by_samplerate(uint32_t samplerate)
{
    switch (afe_get_apll_by_samplerate(samplerate)) {
        case AFE_APLL1:
            aud_apll_1_cntr--;
            if (aud_apll_1_cntr == 0) {
                afe_apll1tuner_clock_off();
                afe_apll1_enable(false);
            } else if (aud_apll_1_cntr < 0) {
                aud_apll_1_cntr = 0;
            }
            break;
        case AFE_APLL2:
            aud_apll_2_cntr--;
            if (aud_apll_2_cntr == 0) {
                afe_apll2tuner_clock_off();
                afe_apll2_enable(false);
            } else if (aud_apll_2_cntr < 0) {
                aud_apll_2_cntr = 0;
            }
            break;
        default:
            break;
    }
}

/*[ask apll tuning method]*/
void afe_apll1_enable(bool enable)
{
    if (enable) {
        //Setting audio clock, engen 22M
        AFE_SET_REG(AUDIO_TOP_CON0, 0 << AUDIO_TOP_CON0_PDN_22M_POS, AUDIO_TOP_CON0_PDN_22M_MASK);
        //Enable HD engen 22m on
        AFE_SET_REG(AFE_HD_ENGEN_ENABLE, 1 << 0, 1 << 0);
    } else {
        AFE_SET_REG(AUDIO_TOP_CON0, 1 << AUDIO_TOP_CON0_PDN_22M_POS, AUDIO_TOP_CON0_PDN_22M_MASK);
        AFE_SET_REG(AFE_HD_ENGEN_ENABLE, 0 << 0, 1 << 0);
    }
}

void afe_apll2_enable(bool enable)
{
    if (enable) {
        //Setting audio clock, engen 24M
        AFE_SET_REG(AUDIO_TOP_CON0, 0 << AUDIO_TOP_CON0_PDN_24M_POS, AUDIO_TOP_CON0_PDN_24M_MASK);
        //Enable HD engen 24m on
        AFE_SET_REG(AFE_HD_ENGEN_ENABLE, 1 << 1, 1 << 1);
    } else {
        AFE_SET_REG(AUDIO_TOP_CON0, 1 << AUDIO_TOP_CON0_PDN_24M_POS, AUDIO_TOP_CON0_PDN_24M_MASK);
        AFE_SET_REG(AFE_HD_ENGEN_ENABLE, 0 << 1, 1 << 1);
    }
}

void afe_enable_apll_tuner_by_samplerate(uint32_t samplerate)
{
    switch (afe_get_apll_by_samplerate(samplerate)) {
        //Enable tuner
        case AFE_APLL1:
            aud_apll_1_tuner_cntr_2++;
            if (aud_apll_1_tuner_cntr_2 == 1) {
                AFE_SET_REG(AFE_APLL1_TUNER_CFG, 0x00000432, 0x0000FFF7);
                AFE_SET_REG(AFE_APLL1_TUNER_CFG, 0x1, 0x1);
            }
            break;
        case AFE_APLL2:
            aud_apll_2_tuner_cntr_2++;
            if (aud_apll_2_tuner_cntr_2 == 1) {
                AFE_SET_REG(AFE_APLL2_TUNER_CFG, 0x00000434, 0x0000FFF7);
                AFE_SET_REG(AFE_APLL2_TUNER_CFG, 0x1, 0x1);
            }
            break;
        default:
            break;
    }
}

void afe_disable_apll_tuner_by_samplerate(uint32_t samplerate)
{
    switch (afe_get_apll_by_samplerate(samplerate)) {
        case AFE_APLL1:
            aud_apll_1_tuner_cntr_2--;
            if (aud_apll_1_tuner_cntr_2 == 0) {
               AFE_SET_REG(AFE_APLL1_TUNER_CFG, 0x0, 0x1);
            } else if (aud_apll_1_tuner_cntr_2 < 0) {
               aud_apll_1_tuner_cntr_2 = 0;
            }
            break;
        case AFE_APLL2:
            aud_apll_2_tuner_cntr_2--;
            if (aud_apll_2_tuner_cntr_2 == 0) {
                AFE_SET_REG(AFE_APLL2_TUNER_CFG, 0x0, 0x1);
            } else if (aud_apll_2_tuner_cntr_2 < 0) {
                aud_apll_2_tuner_cntr_2 = 0;
            }
            break;
        default:
            break;
    }
}

void afe_asrc_clock_on(afe_mem_asrc_id_t asrc_id)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    AFE_SET_REG(AUDIO_TOP_CON1,(0 << (AUDIO_TOP_CON1_PDN_ASRC1_POS + asrc_id)),(AUDIO_TOP_CON1_PDN_ASRC1_MASK << asrc_id));
    aud_asrc_clk_cntr++;
    if (aud_asrc_clk_cntr == 1) {
        AFE_SET_REG(AUDIO_TOP_CON1,(0 << AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_POS),AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_MASK);
    }
    hal_nvic_restore_interrupt_mask(mask);
}

void afe_asrc_clock_off(afe_mem_asrc_id_t asrc_id)
{
    uint32_t mask;
    hal_nvic_save_and_set_interrupt_mask(&mask);
    /* pdn asrc clk */
    AFE_SET_REG(AUDIO_TOP_CON1,1 << (AUDIO_TOP_CON1_PDN_ASRC1_POS + asrc_id),(AUDIO_TOP_CON1_PDN_ASRC1_MASK << asrc_id));
    /*sw reset*/
    AFE_SET_REG(MEM_ASRC_TOP_CON0,1<<(MEM_ASRC_TOP_CON0_MASM1_SFT_RST_POS + asrc_id), (MEM_ASRC_TOP_CON0_MASM1_SFT_RST_MASK<<asrc_id));
    /*reset release*/
    AFE_SET_REG(MEM_ASRC_TOP_CON0,0<<(MEM_ASRC_TOP_CON0_MASM1_SFT_RST_POS + asrc_id), (MEM_ASRC_TOP_CON0_MASM1_SFT_RST_MASK<<asrc_id));
    aud_asrc_clk_cntr--;
    if (aud_asrc_clk_cntr == 0) {
        /* pdn dram bridge */
        AFE_SET_REG(AUDIO_TOP_CON1,
                     (1 << AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_POS),
                    AUDIO_TOP_CON1_PDN_DRAM_BRIDGE_MASK);
    } else if (aud_asrc_clk_cntr < 0) {
        aud_asrc_clk_cntr = 0;
    }
    hal_nvic_restore_interrupt_mask(mask);
}

#endif //#ifdef HAL_AUDIO_MODULE_ENABLED
