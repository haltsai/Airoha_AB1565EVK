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
#ifndef _DA7212_H
#define _DA7212_H

#include "hal.h"

#define DA7212_I2C_ADDR (0x1A)

/*RG map define*/
/*status RG*/
#define DA7212_STATUS1_ADDR             (0x02)
#define DA7212_PLL_STATUS_ADDR          (0x03)
#define DA7212_AUX_L_GAIN_STATUS_ADDR   (0x04)
#define DA7212_AUX_R_GAIN_STATUS_ADDR   (0x05)
#define DA7212_MIC_1_GAIN_STATUS_ADDR   (0x06)
#define DA7212_MIC_2_GAIN_STATUS_ADDR   (0x07)
#define DA7212_MIXIN_L_GAIN_STATUS_ADDR (0X08)
#define DA7212_MIXIN_R_GAIN_STATUS_ADDR (0X09)
#define DA7212_ADC_L_GAIN_STATUS_ADDR   (0x0A)
#define DA7212_ADC_R_GAIN_STATUS_ADDR   (0x0B)
#define DA7212_DAC_L_GAIN_STATUS_ADDR   (0x0C)
#define DA7212_DAC_R_GAIN_STATUS_ADDR   (0x0D)
#define DA7212_HP_L_GAIN_STATUS_ADDR    (0x0E)
#define DA7212_HP_R_GAIN_STATUS_ADDR    (0x0F)
#define DA7212_LINE_GAIN_STATUS_ADDR    (0x10)

/*system init RG*/
#define DA7212_CIF_CTRL_ADDR            (0x1D)
#define DA7212_DIG_ROUTING_DAI_ADDR     (0x21)
#define DA7212_SR_ADDR                  (0x22)
#define DA7212_REFERENCES_ADDR          (0x23)
#define DA7212_PLL_FRAC_TOP_ADDR        (0x24)
#define DA7212_PLL_FRAC_BOT_ADDR        (0x25)
#define DA7212_PLL_INTEGER_ADDR         (0x26)
#define DA7212_PLL_CTRL_ADDR            (0x27)
#define DA7212_DAI_CLK_MODE_ADDR        (0x28)
#define DA7212_DAI_CTRL_ADDR            (0x29)
#define DA7212_DIG_ROUTING_DAC_ADDR     (0x2A)
#define DA7212_ALC_CTRL1_ADDR           (0x2B)

/*input gain/select filter RG*/
#define DA7212_AUX_L_GAIN_ADDR          (0x30)
#define DA7212_AUX_R_GAIN_ADDR          (0x31)
#define DA7212_MIXIN_L_SELECT_ADDR      (0x32)
#define DA7212_MIXIN_R_SELECT_ADDR      (0x33)
#define DA7212_MIXIN_L_GAIN_ADDR        (0x34)
#define DA7212_MIXIN_R_GAIN_ADDR        (0x35)
#define DA7212_ADC_L_GAIN_ADDR          (0x36)
#define DA7212_ADC_R_GAIN_ADDR          (0x37)
#define DA7212_ADC_FILTERS1_ADDR        (0x38)
#define DA7212_MIC_1_GAIN               (0x39)
#define DA7212_MIC_2_GAIN               (0x3A)

/*output gain/select filter RG*/
#define DA7212_DAC_FILTERS5_ADDR        (0x40)
#define DA7212_DAC_FILTERS2_ADDR        (0x41)
#define DA7212_DAC_FILTERS3_ADDR        (0x42)
#define DA7212_DAC_FILTERS4_ADDR        (0x43)
#define DA7212_DAC_FILTERS1_ADDR        (0x44)
#define DA7212_DAC_L_GAIN_ADDR          (0x45)
#define DA7212_DAC_R_GAIN_ADDR          (0x46)
#define DA7212_CP_CTRL_ADDR             (0x47)
#define DA7212_HP_L_GAIN_ADDR           (0x48)
#define DA7212_HP_R_GAIN_ADDR           (0x49)
#define DA7212_LINE_GAIN_ADDR           (0x4A)
#define DA7212_MIXOUT_L_SELECT_ADDR     (0x4B)
#define DA7212_MIXOUT_R_SELECT_ADDR     (0x4C)

/*system controller RG 1*/
#define DA7212_SYSTEM_MODES_INPUT_ADDR  (0x50)
#define DA7212_SYSTEM_MODES_OUTPUT_ADDR (0x51)

/*control RG*/
#define DA7212_AUX_L_CTRL_ADDR          (0x60)
#define DA7212_AUX_R_CTRL_ADDR          (0x61)
#define DA7212_MICBIAS_CTRL_ADDR        (0x62)
#define DA7212_MIC_1_CTRL_ADDR          (0X63)
#define DA7212_MIC_2_CTRL_ADDR          (0x64)
#define DA7212_MIXIN_L_ADDR             (0x65)
#define DA7212_MIXIN_R_ADDR             (0x66)
#define DA7212_ADC_L_CTRL_ADDR          (0x67)
#define DA7212_ADC_R_CTRL_ADDR          (0x68)
#define DA7212_DAC_L_CTRL_ADDR          (0x69)
#define DA7212_DAC_R_CTRL_ADDR          (0x6A)
#define DA7212_HP_L_CTRL_ADDR           (0x6B)
#define DA7212_HP_R_CTRL_ADDR           (0x6C)
#define DA7212_LINE_CTRL_ADDR           (0x6D)
#define DA7212_MIXOUT_L_CTRL_ADDR       (0x6E)
#define DA7212_MIXOUT_R_CTRL_ADDR       (0x6F)

/*mixed sample mode RG*/
#define DA7212_MIXED_SAMPLE_MODE_ADDR   (0x84)

/*configuration RG*/
#define DA7212_LDO_CTRL_ADDR            (0x90)
#define DA7212_GAIN_RAMP_CTRL_ADDR      (0x92)
#define DA7212_MIC_CONFIG_ADDR          (0x93)
#define DA7212_PC_COUNT_ADDR            (0x94)
#define DA7212_CP_VOL_THRESHOLD1_ADDR   (0x95)
#define DA7212_CP_DELAY_ADDR            (0x96)
#define DA7212_CP_DETECTOR_ADDR         (0x97)
#define DA7212_DAI_OFFSET_ADDR          (0x98)
#define DA7212_DIG_CTRL_ADDR            (0x99)
#define DA7212_ALC_CTRL2_ADDR           (0x9A)
#define DA7212_ALC_CTRL3_ADDR           (0x9B)
#define DA7212_ALC_NOISE_ADDR           (0x9C)
#define DA7212_ALC_TARGET_MIN_ADDR      (0x9D)
#define DA7212_ALC_THRESHOLD_MAX_ADDR   (0x9E)
#define DA7212_ALC_GAIN_LIMITS_ADDR     (0x9F)
#define DA7212_ALC_ANA_GAIN_LIMITS_ADDR (0xA0)
#define DA7212_ALC_ANTICLIP_CTRL_ADDR   (0xA1)
#define DA7212_ALC_ANTICLIP_LEVEL_ADDR  (0xA2)
#define DA7212_ALC_OFFSET_AUTO_M_L_ADDR (0xA3)
#define DA7212_ALC_OFFSET_AUTO_U_L_ADDR (0xA4)
#define DA7212_ALC_OFFSET_MAN_M_L_ADDR  (0xA6)
#define DA7212_ALC_OFFSET_MAN_U_L_ADDR  (0xA7)
#define DA7212_ALC_OFFSET_AUTO_M_R_ADDR (0xA8)
#define DA7212_ALC_OFFSET_AUTO_U_R_ADDR (0xA9)
#define DA7212_ALC_OFFSET_MAN_M_R_ADDR  (0xAB)
#define DA7212_ALC_OFFSET_MAN_U_R_ADDR  (0xAC)
#define DA7212_ALC_ALC_CIC_OP_LVL_CTRL_ADDR (0xAD)
#define DA7212_ALC_CIC_OP_LVL_DATA_ADDR (0xAE)
#define DA7212_DAC_NG_SETUP_TIME_ADDR   (0xAF)
#define DA7212_DAC_NG_OFF_THRESHOLD_ADDR (0xB0)
#define DA7212_DAC_NG_ON_THRESHOLD_ADDR (0xB1)
#define DA7212_DAC_NG_CTRL_ADDR         (0xB2)

/*tone generation & beep RG*/
#define DA7212_TONE_GEN_CFG1_ADDR       (0xB4)
#define DA7212_TONE_GEN_CFG2_ADDR       (0xB5)
#define DA7212_TONE_GEN_CYCLES_ADDR     (0xB6)
#define DA7212_TONE_GEN_FREQ1_L_ADDR    (0xB7)
#define DA7212_TONE_GEN_FREQ1_U_ADDR    (0xB8)
#define DA7212_TONE_GEN_FREQ2_L_ADDR    (0xB9)
#define DA7212_TONE_GEN_FREQ2_U_ADDR    (0xBA)
#define DA7212_TONE_GEN_ON_PER_ADDR     (0xBB)
#define DA7212_TONE_GEN_OFF_PER_ADDR    (0xBC)

/*system controller RG*/
#define DA7212_SYSTEM_STATUS_ADDR       (0xE0)
#define DA7212_SYSTEM_ACTIVE_ADDR       (0xFD)

void DA7212_dac_init(void);
void DA7212_dac_enable(void);
void DA7212_dac_disable(void);
void DA7212_dac_deinit(void);
void DA7212_dump_rg_list(void);

#endif