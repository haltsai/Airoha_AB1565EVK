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

#ifndef __HAL_PINMUX_DEFINE_H__




#define __HAL_PINMUX_DEFINE_H__

#include "hal_platform.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAL_GPIO_MODULE_ENABLED


#define HAL_GPIO_0_GPIO0    0
#define HAL_GPIO_0_UART0_TXD    1
#define HAL_GPIO_0_AUDIO_EXT_SYNC_EN    5
#define HAL_GPIO_0_SPDIF    7
#define HAL_GPIO_0_RF_TEST_P    8
#define HAL_GPIO_0_EINT0    9
#define HAL_GPIO_0_DEBUGMON0    10

#define HAL_GPIO_1_GPIO1    0
#define HAL_GPIO_1_UART0_RXD    1
#define HAL_GPIO_1_SRCLKENAI_FREF    5
#define HAL_GPIO_1_RF_TEST_N    8
#define HAL_GPIO_1_EINT1    9
#define HAL_GPIO_1_DEBUGMON1    10

#define HAL_GPIO_2_GPIO2    0
#define HAL_GPIO_2_DMIC0_CLK    3
#define HAL_GPIO_2_SPDIF    4
#define HAL_GPIO_2_RESETB_OUT    5
#define HAL_GPIO_2_PAD_DFUNC_SFCS0    6
#define HAL_GPIO_2_RF_TEST_BUS    8
#define HAL_GPIO_2_EINT2    9
#define HAL_GPIO_2_DEBUGMON2    10

#define HAL_GPIO_3_GPIO3    0
#define HAL_GPIO_3_AP_JTMS    1
#define HAL_GPIO_3_UART0_CTS    2
#define HAL_GPIO_3_DMIC0_DAT    3
#define HAL_GPIO_3_UART2_RXD    4
#define HAL_GPIO_3_SPI_MST2_CS0    5
#define HAL_GPIO_3_PAD_DFUNC_SIN    6
#define HAL_GPIO_3_AP_JTDI    7
#define HAL_GPIO_3_CTP1    8
#define HAL_GPIO_3_EINT3    9
#define HAL_GPIO_3_DEBUGMON3    10

#define HAL_GPIO_4_GPIO4    0
#define HAL_GPIO_4_I2C1_SCL    1
#define HAL_GPIO_4_UART1_TXD    2
#define HAL_GPIO_4_DMIC1_CLK    3
#define HAL_GPIO_4_PWM3    4
#define HAL_GPIO_4_CLKO1    5
#define HAL_GPIO_4_PAD_DFUNC_SOUT    6
#define HAL_GPIO_4_AP_JRSTB    7
#define HAL_GPIO_4_CTP2    8
#define HAL_GPIO_4_EINT4    9
#define HAL_GPIO_4_DEBUGMON4    10

#define HAL_GPIO_5_GPIO5    0
#define HAL_GPIO_5_I2C1_SDA    1
#define HAL_GPIO_5_UART1_RXD    2
#define HAL_GPIO_5_DMIC1_DAT    3
#define HAL_GPIO_5_PWM4    4
#define HAL_GPIO_5_CLKO3    5
#define HAL_GPIO_5_PAD_DFUNC_SFCK    6
#define HAL_GPIO_5_AP_JTDO    7
#define HAL_GPIO_5_CTP3    8
#define HAL_GPIO_5_EINT5    9
#define HAL_GPIO_5_DEBUGMON5    10

#define HAL_GPIO_6_GPIO6    0
#define HAL_GPIO_6_SPI_MST1_CS0    1
#define HAL_GPIO_6_SPI_SLV0_CS    2
#define HAL_GPIO_6_UART1_CTS    3
#define HAL_GPIO_6_I2S_MST0_MCLK    4
#define HAL_GPIO_6_PWM0    5
#define HAL_GPIO_6_I2C2_SCL    6
#define HAL_GPIO_6_AP_JTMS    7
#define HAL_GPIO_6_AUXADC6    8
#define HAL_GPIO_6_EINT6    9
#define HAL_GPIO_6_DEBUGMON6    10

#define HAL_GPIO_7_GPIO7    0
#define HAL_GPIO_7_SPI_MST1_MOSI    1
#define HAL_GPIO_7_SPI_SLV0_MOSI    2
#define HAL_GPIO_7_UART1_RTS    3
#define HAL_GPIO_7_I2S_MST1_MCLK    4
#define HAL_GPIO_7_UART2_TXD    5
#define HAL_GPIO_7_I2C2_SDA    6
#define HAL_GPIO_7_AP_JTCK    7
#define HAL_GPIO_7_AUXADC5    8
#define HAL_GPIO_7_EINT7    9
#define HAL_GPIO_7_DEBUGMON7    10

#define HAL_GPIO_8_GPIO8    0
#define HAL_GPIO_8_SPI_MST1_MISO    1
#define HAL_GPIO_8_SPI_SLV0_MISO    2
#define HAL_GPIO_8_CLKO0    3
#define HAL_GPIO_8_I2S_MST0_TX    4
#define HAL_GPIO_8_I2S_SLV0_TX    5
#define HAL_GPIO_8_I2C0_SCL    6
#define HAL_GPIO_8_DSP_JTMS    7
#define HAL_GPIO_8_AUXADC4    8
#define HAL_GPIO_8_EINT8    9
#define HAL_GPIO_8_DEBUGMON8    10

#define HAL_GPIO_9_GPIO9    0
#define HAL_GPIO_9_SPI_MST1_SIO2    1
#define HAL_GPIO_9_SPI_SLV0_SIO2    2
#define HAL_GPIO_9_UART0_CTS    3
#define HAL_GPIO_9_I2S_MST0_CK    4
#define HAL_GPIO_9_I2S_SLV0_CK    5
#define HAL_GPIO_9_I2C0_SDA    6
#define HAL_GPIO_9_DSP_JTCK    7
#define HAL_GPIO_9_AUXADC3    8
#define HAL_GPIO_9_EINT9    9
#define HAL_GPIO_9_DEBUGMON9    10

#define HAL_GPIO_10_GPIO10    0
#define HAL_GPIO_10_SPI_MST1_SIO3    1
#define HAL_GPIO_10_SPI_SLV0_SIO3    2
#define HAL_GPIO_10_UART0_RTS    3
#define HAL_GPIO_10_I2S_MST0_WS    4
#define HAL_GPIO_10_I2S_SLV0_WS    5
#define HAL_GPIO_10_I2C1_SCL    6
#define HAL_GPIO_10_DSP_JTDI    7
#define HAL_GPIO_10_AUXADC2    8
#define HAL_GPIO_10_EINT10    9
#define HAL_GPIO_10_DEBUGMON10    10

#define HAL_GPIO_11_GPIO11    0
#define HAL_GPIO_11_SPI_MST1_SCK    1
#define HAL_GPIO_11_SPI_SLV0_SCK    2
#define HAL_GPIO_11_UART1_TXD    3
#define HAL_GPIO_11_I2S_MST0_RX    4
#define HAL_GPIO_11_I2S_SLV0_RX    5
#define HAL_GPIO_11_I2C1_SDA    6
#define HAL_GPIO_11_DSP_JRST    7
#define HAL_GPIO_11_AUXADC1    8
#define HAL_GPIO_11_EINT11    9
#define HAL_GPIO_11_DEBUGMON11    10

#define HAL_GPIO_12_GPIO12    0
#define HAL_GPIO_12_AP_JTCK    1
#define HAL_GPIO_12_AUDIO_EXT_SYNC_EN    2
#define HAL_GPIO_12_UART1_RXD    3
#define HAL_GPIO_12_I2S_MST1_CK    4
#define HAL_GPIO_12_I2S_SLV1_CK    5
#define HAL_GPIO_12_UART2_RXD    6
#define HAL_GPIO_12_DSP_JTDO    7
#define HAL_GPIO_12_AUXADC0    8
#define HAL_GPIO_12_EINT12    9
#define HAL_GPIO_12_DEBUGMON12    10

#define HAL_GPIO_13_GPIO13    0
#define HAL_GPIO_13_PWM2    1
#define HAL_GPIO_13_MSDC0_RST    3
#define HAL_GPIO_13_I2S_MST1_TX    4
#define HAL_GPIO_13_I2S_SLV1_TX    5
#define HAL_GPIO_13_DMIC0_CLK    7
#define HAL_GPIO_13_EINT13    9
#define HAL_GPIO_13_DEBUGMON13    10

#define HAL_GPIO_14_GPIO14    0
#define HAL_GPIO_14_SPI_MST0_CS0    1
#define HAL_GPIO_14_ESC_CS    2
#define HAL_GPIO_14_MSDC0_CMD    3
#define HAL_GPIO_14_I2S_MST1_RX    4
#define HAL_GPIO_14_I2S_SLV1_RX    5
#define HAL_GPIO_14_UART0_RTS    6
#define HAL_GPIO_14_DMIC0_DAT    7
#define HAL_GPIO_14_PWM1    8
#define HAL_GPIO_14_EINT14    9
#define HAL_GPIO_14_DEBUGMON14    10

#define HAL_GPIO_15_GPIO15    0
#define HAL_GPIO_15_SPI_MST0_MOSI    1
#define HAL_GPIO_15_ESC_MOSI    2
#define HAL_GPIO_15_MSDC0_DAT0    3
#define HAL_GPIO_15_I2S_MST1_WS    4
#define HAL_GPIO_15_I2S_SLV1_WS    5
#define HAL_GPIO_15_I2S_MST2_MCLK    6
#define HAL_GPIO_15_DMIC1_CLK    7
#define HAL_GPIO_15_UART2_TXD    8
#define HAL_GPIO_15_EINT15    9
#define HAL_GPIO_15_DEBUGMON15    10

#define HAL_GPIO_16_GPIO16    0
#define HAL_GPIO_16_SPI_MST0_MISO    1
#define HAL_GPIO_16_ESC_MISO    2
#define HAL_GPIO_16_MSDC0_DAT1    3
#define HAL_GPIO_16_I2S_MST2_WS    4
#define HAL_GPIO_16_I2S_SLV2_WS    5
#define HAL_GPIO_16_I2C0_SCL    6
#define HAL_GPIO_16_DMIC1_DAT    7
#define HAL_GPIO_16_UART1_CTS    8
#define HAL_GPIO_16_EINT16    9
#define HAL_GPIO_16_DEBUGMON0    10

#define HAL_GPIO_17_GPIO17    0
#define HAL_GPIO_17_SPI_MST0_SIO2    1
#define HAL_GPIO_17_ESC_SIO2    2
#define HAL_GPIO_17_MSDC0_DAT2    3
#define HAL_GPIO_17_I2S_MST2_CK    4
#define HAL_GPIO_17_I2S_SLV2_CK    5
#define HAL_GPIO_17_I2C0_SDA    6
#define HAL_GPIO_17_MSDC0_CLK1    7
#define HAL_GPIO_17_UART1_RTS    8
#define HAL_GPIO_17_EINT17    9
#define HAL_GPIO_17_DEBUGMON1    10

#define HAL_GPIO_18_GPIO18    0
#define HAL_GPIO_18_SPI_MST0_SIO3    1
#define HAL_GPIO_18_ESC_SIO3    2
#define HAL_GPIO_18_MSDC0_DAT3    3
#define HAL_GPIO_18_I2S_MST2_RX    4
#define HAL_GPIO_18_I2S_SLV2_RX    5
#define HAL_GPIO_18_I2S_MST1_MCLK    6
#define HAL_GPIO_18_MSDC0_CLK2    7
#define HAL_GPIO_18_UART2_TXD    8
#define HAL_GPIO_18_EINT18    9
#define HAL_GPIO_18_DEBUGMON2    10

#define HAL_GPIO_19_GPIO19    0
#define HAL_GPIO_19_SPI_MST0_SCK    1
#define HAL_GPIO_19_ESC_SCK    2
#define HAL_GPIO_19_MSDC0_CLK0    3
#define HAL_GPIO_19_I2S_MST2_TX    4
#define HAL_GPIO_19_I2S_SLV2_TX    5
#define HAL_GPIO_19_I2S_MST0_MCLK    6
#define HAL_GPIO_19_CLKO2    7
#define HAL_GPIO_19_UART2_RXD    8
#define HAL_GPIO_19_EINT19    9
#define HAL_GPIO_19_DEBUGMON3    10

#define HAL_GPIO_20_GPIO20    0
#define HAL_GPIO_20_I2S_MST2_MCLK    1
#define HAL_GPIO_20_AUDIO_EXT_SYNC_EN    2
#define HAL_GPIO_20_COEX_W1_DAT    3
#define HAL_GPIO_20_I2C0_SCL    4
#define HAL_GPIO_20_UART1_TXD    5
#define HAL_GPIO_20_SPI_MST2_MISO    6
#define HAL_GPIO_20_AP_JTMS    7
#define HAL_GPIO_20_I2C1_SDA    8
#define HAL_GPIO_20_EINT20    9
#define HAL_GPIO_20_DEBUGMON4    10

#define HAL_GPIO_21_GPIO21    0
#define HAL_GPIO_21_SPI_MST2_CS0    1
#define HAL_GPIO_21_I2S_MST1_RX    2
#define HAL_GPIO_21_I2S_SLV1_RX    3
#define HAL_GPIO_21_I2C0_SDA    4
#define HAL_GPIO_21_UART1_RXD    5
#define HAL_GPIO_21_ESC_CS    6
#define HAL_GPIO_21_AP_JTCK    7
#define HAL_GPIO_21_I2C1_SCL    8
#define HAL_GPIO_21_EINT21    9
#define HAL_GPIO_21_DEBUGMON5    10

#define HAL_GPIO_22_GPIO22    0
#define HAL_GPIO_22_SPI_MST2_MISO    1
#define HAL_GPIO_22_I2S_MST1_TX    2
#define HAL_GPIO_22_I2S_SLV1_TX    3
#define HAL_GPIO_22_DMIC1_CLK    4
#define HAL_GPIO_22_UART1_CTS    5
#define HAL_GPIO_22_ESC_MISO    6
#define HAL_GPIO_22_DSP_JTMS    7
#define HAL_GPIO_22_AP_JTDI    8
#define HAL_GPIO_22_EINT22    9
#define HAL_GPIO_22_DEBUGMON6    10

#define HAL_GPIO_23_GPIO23    0
#define HAL_GPIO_23_SPI_MST2_MOSI    1
#define HAL_GPIO_23_I2S_MST2_WS    2
#define HAL_GPIO_23_I2S_SLV2_WS    3
#define HAL_GPIO_23_DMIC1_DAT    4
#define HAL_GPIO_23_UART1_RTS    5
#define HAL_GPIO_23_ESC_MOSI    6
#define HAL_GPIO_23_DSP_JTCK    7
#define HAL_GPIO_23_AP_JRSTB    8
#define HAL_GPIO_23_EINT23    9
#define HAL_GPIO_23_DEBUGMON7    10

#define HAL_GPIO_24_GPIO24    0
#define HAL_GPIO_24_SPI_MST2_SIO2    1
#define HAL_GPIO_24_I2S_MST2_CK    2
#define HAL_GPIO_24_I2S_SLV2_CK    3
#define HAL_GPIO_24_DMIC0_CLK    4
#define HAL_GPIO_24_SPI_MST0_CS1    5
#define HAL_GPIO_24_ESC_SIO2    6
#define HAL_GPIO_24_DSP_JTDI    7
#define HAL_GPIO_24_AP_JTDO    8
#define HAL_GPIO_24_EINT24    9
#define HAL_GPIO_24_DEBUGMON8    10

#define HAL_GPIO_25_GPIO25    0
#define HAL_GPIO_25_SPI_MST2_SIO3    1
#define HAL_GPIO_25_I2S_MST2_RX    2
#define HAL_GPIO_25_I2S_SLV2_RX    3
#define HAL_GPIO_25_DMIC0_DAT    4
#define HAL_GPIO_25_SPI_MST0_CS2    5
#define HAL_GPIO_25_ESC_SIO3    6
#define HAL_GPIO_25_DSP_JRST    7
#define HAL_GPIO_25_I2C2_SDA    8
#define HAL_GPIO_25_EINT25    9
#define HAL_GPIO_25_DEBUGMON9    10

#define HAL_GPIO_26_GPIO26    0
#define HAL_GPIO_26_SPI_MST2_SCK    1
#define HAL_GPIO_26_I2S_MST2_TX    2
#define HAL_GPIO_26_I2S_SLV2_TX    3
#define HAL_GPIO_26_PWM2    4
#define HAL_GPIO_26_SPI_MST0_CS3    5
#define HAL_GPIO_26_ESC_SCK    6
#define HAL_GPIO_26_DSP_JTDO    7
#define HAL_GPIO_26_I2C2_SCL    8
#define HAL_GPIO_26_EINT26    9
#define HAL_GPIO_26_DEBUGMON10    10

#define HAL_GPIO_27_GPIO27    0
#define HAL_GPIO_27_I2S_MST0_TX    1
#define HAL_GPIO_27_I2S_SLV0_TX    2
#define HAL_GPIO_27_DMIC0_CLK    3
#define HAL_GPIO_27_COEX_W3_BT_WLAN    4
#define HAL_GPIO_27_I2C1_SCL    5
#define HAL_GPIO_27_I2C0_SCL    6
#define HAL_GPIO_27_AUDIO_EXT_SYNC_EN    7
#define HAL_GPIO_27_GPIO_PIP_SEL_3    8
#define HAL_GPIO_27_EINT27    9
#define HAL_GPIO_27_DEBUGMON11    10

#define HAL_GPIO_28_GPIO28    0
#define HAL_GPIO_28_I2S_MST0_CK    1
#define HAL_GPIO_28_I2S_SLV0_CK    2
#define HAL_GPIO_28_DMIC0_DAT    3
#define HAL_GPIO_28_UART0_TXD    4
#define HAL_GPIO_28_I2C1_SDA    5
#define HAL_GPIO_28_I2C0_SDA    6
#define HAL_GPIO_28_SPI_MST0_CS1    7
#define HAL_GPIO_28_SPI_MST1_CS1    8
#define HAL_GPIO_28_EINT28    9
#define HAL_GPIO_28_DEBUGMON12    10

#define HAL_GPIO_29_GPIO29    0
#define HAL_GPIO_29_I2S_MST0_WS    1
#define HAL_GPIO_29_I2S_SLV0_WS    2
#define HAL_GPIO_29_SPDIF    3
#define HAL_GPIO_29_UART0_RXD    4
#define HAL_GPIO_29_I2C2_SCL    5
#define HAL_GPIO_29_CLKO0    6
#define HAL_GPIO_29_SPI_MST0_CS2    7
#define HAL_GPIO_29_SPI_MST1_CS2    8
#define HAL_GPIO_29_EINT29    9
#define HAL_GPIO_29_DEBUGMON13    10

#define HAL_GPIO_30_GPIO30    0
#define HAL_GPIO_30_I2S_MST0_RX    1
#define HAL_GPIO_30_I2S_SLV0_RX    2
#define HAL_GPIO_30_UART0_RTS    3
#define HAL_GPIO_30_DMIC1_DAT    4
#define HAL_GPIO_30_I2C2_SDA    5
#define HAL_GPIO_30_SRCLKENAI_FREF    6
#define HAL_GPIO_30_SPI_MST0_CS3    7
#define HAL_GPIO_30_SPI_MST1_CS3    8
#define HAL_GPIO_30_EINT30    9
#define HAL_GPIO_30_DEBUGMON14    10

#define HAL_GPIO_31_GPIO31    0
#define HAL_GPIO_31_I2S_MST2_TX    1
#define HAL_GPIO_31_I2S_SLV2_TX    2
#define HAL_GPIO_31_UART0_CTS    3
#define HAL_GPIO_31_DMIC1_CLK    4
#define HAL_GPIO_31_I2C0_SCL    5
#define HAL_GPIO_31_COEX_W1_DAT    6
#define HAL_GPIO_31_SPI_MST2_MOSI    7
#define HAL_GPIO_31_GPIO_PIP_SEL_0    8
#define HAL_GPIO_31_EINT31    9
#define HAL_GPIO_31_DEBUGMON15    10

#define HAL_GPIO_32_GPIO32    0
#define HAL_GPIO_32_I2S_MST2_RX    1
#define HAL_GPIO_32_I2S_SLV2_RX    2
#define HAL_GPIO_32_AP_JTMS    3
#define HAL_GPIO_32_COEX_W3_BT_ACT    4
#define HAL_GPIO_32_I2C0_SDA    5
#define HAL_GPIO_32_COEX_W3_BT_WLAN    6
#define HAL_GPIO_32_SPI_MST2_CS1    7
#define HAL_GPIO_32_GPIO_PIP_SEL_1    8
#define HAL_GPIO_32_EINT32    9
#define HAL_GPIO_32_DEBUGMON0    10

#define HAL_GPIO_33_GPIO33    0
#define HAL_GPIO_33_I2S_MST2_CK    1
#define HAL_GPIO_33_I2S_SLV2_CK    2
#define HAL_GPIO_33_AP_JTCK    3
#define HAL_GPIO_33_COEX_W3_BT_PRI    4
#define HAL_GPIO_33_UART1_TXD    5
#define HAL_GPIO_33_COEX_W3_BT_ACT    6
#define HAL_GPIO_33_SPI_MST2_CS2    7
#define HAL_GPIO_33_GPIO_PIP_SEL_2    8
#define HAL_GPIO_33_EINT33    9
#define HAL_GPIO_33_DEBUGMON1    10

#define HAL_GPIO_34_GPIO34    0
#define HAL_GPIO_34_I2S_MST2_WS    1
#define HAL_GPIO_34_I2S_SLV2_WS    2
#define HAL_GPIO_34_AP_JTDI    3
#define HAL_GPIO_34_DSP_JTMS    4
#define HAL_GPIO_34_UART1_RXD    5
#define HAL_GPIO_34_COEX_W3_BT_PRI    6
#define HAL_GPIO_34_SPI_MST2_CS3    7
#define HAL_GPIO_34_SPI_SLV0_CS    8
#define HAL_GPIO_34_EINT34    9
#define HAL_GPIO_34_DEBUGMON2    10

#define HAL_GPIO_35_GPIO35    0
#define HAL_GPIO_35_I2S_MST1_TX    1
#define HAL_GPIO_35_I2S_SLV1_TX    2
#define HAL_GPIO_35_AP_JRSTB    3
#define HAL_GPIO_35_DSP_JTCK    4
#define HAL_GPIO_35_UART1_CTS    5
#define HAL_GPIO_35_UART2_CTS    6
#define HAL_GPIO_35_PWM0    7
#define HAL_GPIO_35_SPI_SLV0_MOSI    8
#define HAL_GPIO_35_EINT35    9
#define HAL_GPIO_35_DEBUGMON3    10

#define HAL_GPIO_36_GPIO36    0
#define HAL_GPIO_36_I2S_MST1_RX    1
#define HAL_GPIO_36_I2S_SLV1_RX    2
#define HAL_GPIO_36_AP_JTDO    3
#define HAL_GPIO_36_DSP_JTDI    4
#define HAL_GPIO_36_UART1_RTS    5
#define HAL_GPIO_36_UART2_RTS    6
#define HAL_GPIO_36_PWM1    7
#define HAL_GPIO_36_SPI_SLV0_MISO    8
#define HAL_GPIO_36_EINT36    9
#define HAL_GPIO_36_DEBUGMON4    10

#define HAL_GPIO_37_GPIO37    0
#define HAL_GPIO_37_I2S_MST1_WS    1
#define HAL_GPIO_37_I2S_SLV1_WS    2
#define HAL_GPIO_37_I2C0_SCL    3
#define HAL_GPIO_37_DSP_JRST    4
#define HAL_GPIO_37_PMIC_AO_SCL    5
#define HAL_GPIO_37_UART2_TXD    6
#define HAL_GPIO_37_MSDC0_CLK1    7
#define HAL_GPIO_37_SPI_SLV0_SIO2    8
#define HAL_GPIO_37_EINT37    9
#define HAL_GPIO_37_DEBUGMON5    10

#define HAL_GPIO_38_GPIO38    0
#define HAL_GPIO_38_I2S_MST1_CK    1
#define HAL_GPIO_38_I2S_SLV1_CK    2
#define HAL_GPIO_38_I2C0_SDA    3
#define HAL_GPIO_38_DSP_JTDO    4
#define HAL_GPIO_38_PMIC_AO_SDA    5
#define HAL_GPIO_38_UART2_RXD    6
#define HAL_GPIO_38_MSDC0_CLK2    7
#define HAL_GPIO_38_SPI_SLV0_SIO3    8
#define HAL_GPIO_38_EINT38    9
#define HAL_GPIO_38_DEBUGMON6    10

#define HAL_GPIO_39_GPIO39    0
#define HAL_GPIO_39_I2S_MST0_MCLK    1
#define HAL_GPIO_39_I2S_MST1_MCLK    2
#define HAL_GPIO_39_I2S_MST2_MCLK    3
#define HAL_GPIO_39_SPDIF    4
#define HAL_GPIO_39_CLKO2    5
#define HAL_GPIO_39_SPI_MST2_SCK    6
#define HAL_GPIO_39_PWM4    7
#define HAL_GPIO_39_SPI_SLV0_SCK    8
#define HAL_GPIO_39_EINT39    9
#define HAL_GPIO_39_DEBUGMON7    10

#ifdef __cplusplus
}
#endif

#endif /*HAL_GPIO_MODULE_ENABLED*/

#endif /*__HAL_PINMUX_DEFINE_H__*/

