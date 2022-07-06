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
 
#ifndef __HAL_CLOCK_PLATFORM_AB155X_H__
#define __HAL_CLOCK_PLATFORM_AB155X_H__

#include "hal_platform.h"
#include "ab156x.h"
#ifdef HAL_CLOCK_MODULE_ENABLED
#define RF_DESENSE  1
#define STR_VALUE(arg)      #arg
#define __stringify(name) STR_VALUE(name)

//Module name: cksys_bus_clk Base address: (+0xA2150000)
#define BUS_DCM_CON_0                         ((volatile uint32_t *)0xA21E0100)
#define BUS_DCM_CON_1                         ((volatile uint32_t *)0xA21E0104)
#define CLK_FREQ_SWCH                         ((volatile uint32_t *)0xA21E0170)
#define SFC_DCM_CON_0                         ((volatile uint32_t *)0xA2020140)
#define SFC_DCM_CON_1                         ((volatile uint32_t *)0xA2020144)

#define CM4_DCM_CON_0                         ((volatile uint32_t *)0xA21E0110)
#define CM4_DCM_CON_1                         ((volatile uint32_t *)0xA21E0114)
#define SYS_FREE_DCM_CON                      ((volatile uint32_t *)0xA2020130)
#define ESC_DCM_CON_0                         ((volatile uint32_t *)0xA2020148)
#define ESC_DCM_CON_1                         ((volatile uint32_t *)0xA202014c)
#define CLOCK_SEL0                            ((volatile uint32_t *)0xA21E0200)
#define CLOCK_SEL1                            ((volatile uint32_t *)0xA21E0204)
#define PDN_PD_COND0                          ((volatile uint32_t *)0xA21E0300)
#define PDN_PD_SET0                           ((volatile uint32_t *)0xA21E0310)
#define PDN_PD_CLRD0                          ((volatile uint32_t *)0xA21E0320)
#define PDN_AO_COND0                          ((volatile uint32_t *)0xA21E0330)
#define PDN_AO_SET0                           ((volatile uint32_t *)0xA21E0340)
#define PDN_AO_CLRD0                          ((volatile uint32_t *)0xA21E0350)
#define PDN_TOP_COND0                         ((volatile uint32_t *)0xA21E0360)
#define PDN_TOP_SET0                          ((volatile uint32_t *)0xA21E0370)
#define PDN_TOP_CLRD0                         ((volatile uint32_t *)0xA21E0380)
#define DSP0_SLOW_CON0                        ((volatile uint32_t *)0xA21E0400)
#define DSP0_SLOW_CON2                        ((volatile uint32_t *)0xA21E0408)
#define DSP_DCM_CON_0                         ((volatile uint32_t *)0xA21E0410)
#define DSP_DCM_CON_1                         ((volatile uint32_t *)0xA21E0414)
#define XO_DCM_CON_0                          ((volatile uint32_t *)0xA2030C00)

//Module name: cksys Base address: (+0xA2020000)

#define CKSYS_TST_SEL_1                       ((volatile uint32_t *)0xA2020224)
#define ABIST_LMON_SEL                        ((volatile uint32_t *)0xA2040504)

#define ABIST_MON_DATA0                       ((volatile uint32_t *)0xA204050C)	 



#define CKSYS_CLK_CFG_0                       ((volatile uint32_t *)0xA2020230)
#define CKSYS_CLK_CFG_1                       ((volatile uint32_t *)0xA2020234)
#define CKSYS_CLK_CFG_2                       ((volatile uint32_t *)0xA2020238)
#define CKSYS_CLK_CFG_3                       ((volatile uint32_t *)0xA202023C)
#define CKSYS_CLK_CFG_4                       ((volatile uint32_t *)0xA2020240)

#define CKSYS_CLK_UPDATE_0                    ((volatile uint32_t *)0xA2020250)
#define CKSYS_CLK_UPDATE_1                    ((volatile uint32_t *)0xA2020254)
#define CKSYS_CLK_UPDATE_2                    ((volatile uint32_t *)0xA2020258)
#define CKSYS_CLK_UPDATE_3                    ((volatile uint32_t *)0xA202025C)

#define CKSYS_CLK_UPDATE_STATUS_0             ((volatile uint32_t *)0xA2020260)
#define CKSYS_CLK_UPDATE_STATUS_1             ((volatile uint32_t *)0xA2020264)
#define CKSYS_CLK_UPDATE_STATUS_2             ((volatile uint32_t *)0xA2020268)
#define CKSYS_CLK_UPDATE_STATUS_3             ((volatile uint32_t *)0xA202026C)

#define CKSYS_CLK_FORCE_ON_0                  ((volatile uint32_t *)0xA2020270)
#define CKSYS_CLK_FORCE_ON_1                  ((volatile uint32_t *)0xA2020274)
#define CKSYS_CLK_FORCE_ON_2                  ((volatile uint32_t *)0xA2020278)
#define CKSYS_CLK_FORCE_ON_3                  ((volatile uint32_t *)0xA202027C)

#define CKSYS_CLK_DIV_0                       ((volatile uint32_t *)0xA2020280)
#define CKSYS_CLK_DIV_1                       ((volatile uint32_t *)0xA2020284)
#define CKSYS_CLK_DIV_2                       ((volatile uint32_t *)0xA2020288)
#define CKSYS_CLK_DIV_3                       ((volatile uint32_t *)0xA202028C)

#define CKSYS_REF_CLK_SEL                     ((volatile uint32_t *)0xA20202A0)

#define CKSYS_SRC_CLK_CG0                     ((volatile uint32_t *)0xA20202C0)
#define CKSYS_SRC_CLK_CG1                     ((volatile uint32_t *)0xA20202C4)
#define CKSYS_SRC_CLK_CG2                     ((volatile uint32_t *)0xA20202C8)
#define GPIO_CLKO_CTRL_A                      ((volatile uint32_t *)0xA2020310)

#define PLL_ABIST_FQMTR_CON0                  ((volatile uint32_t *)0xA2020400)
#define PLL_ABIST_FQMTR_CON1                  ((volatile uint32_t *)0xA2020404)
#define PLL_ABIST_FQMTR_CON2                  ((volatile uint32_t *)0xA2020408)
#define PLL_ABIST_FQMTR_DATA                  ((volatile uint32_t *)0xA202040C)

#define CKSYS_MISC_0                          ((volatile uint32_t *)0xA2020500)

#define RG_AUD_CKDIV_CFG0                     ((volatile uint32_t *)0xA2020300)   // @@@@@
#define RG_AUD_CKDIV_CFG1                     ((volatile uint32_t *)0xA2020304)
#define RG_AUD_CKDIV_CFG2                     ((volatile uint32_t *)0xA2020308)
#define RG_AUD_CKDIV_CFG3                     ((volatile uint32_t *)0xA202030C)

#define XO_PDN_PD_COND0                       ((volatile uint32_t *)0xA2030B00)
#define XO_PDN_PD_SET0                        ((volatile uint32_t *)0xA2030B10)
#define XO_PDN_PD_CLRD0                       ((volatile uint32_t *)0xA2030B20)

#define XO_PDN_AO_COND0                       ((volatile uint32_t *)0xA2030B30)
#define XO_PDN_AO_SET0                        ((volatile uint32_t *)0xA2030B40)
#define XO_PDN_AO_CLRD0                       ((volatile uint32_t *)0xA2030B50)

#define XO_PDN_TOP_COND0                      ((volatile uint32_t *)0xA2030B60)
#define XO_PDN_TOP_SET0                       ((volatile uint32_t *)0xA2030B70)
#define XO_PDN_TOP_CLRD0                      ((volatile uint32_t *)0xA2030B80)

#define XO_DCM_CON_0                          ((volatile uint32_t *)0xA2030C00)
#define XO_DCM_CON_1                          ((volatile uint32_t *)0xA2030C04)
//Module name: mixedsys_d Base address: (+0xA2040000)
#define MIXEDSYSY_BASE                        ((volatile uint32_t *)0xA2040000)
#define CLKSQ_CON0                            ((volatile uint32_t *)0xA2040000)
#define CLKSQ_CON2                            ((volatile uint32_t *)0xA2040008)
#define CLKSQ_CON4                            ((volatile uint32_t *)0xA2040010)
#define LPOSC_CON0                            ((volatile uint32_t *)0xA2040200)
#define DSPCFG_STALL                          ((volatile uint32_t *)0xA21F0018)

#define XPLL_DBG_PROB                         ((volatile uint32_t *)0xA2050400)
#define XPLL_DBG_PROB_MON                     ((volatile uint32_t *)0xA2050404)

#define APLL1_CTL0                            ((volatile uint32_t *)0xA2050000)
#define APLL1_CTL1                            ((volatile uint32_t *)0xA2050004)
#define APLL1_CTL2                            ((volatile uint32_t *)0xA2050008)
#define APLL1_CTL3                            ((volatile uint32_t *)0xA205000C)
#define APLL1_CTL4                            ((volatile uint32_t *)0xA2050010)
#define APLL1_CTL5                            ((volatile uint32_t *)0xA2050014)
#define APLL1_CTL6                            ((volatile uint32_t *)0xA2050018)
#define APLL1_CTL7                            ((volatile uint32_t *)0xA205001C)
#define APLL1_CTL8                            ((volatile uint32_t *)0xA2050020)
#define APLL1_CTL9                            ((volatile uint32_t *)0xA2050024)
#define APLL1_CTL10                           ((volatile uint32_t *)0xA2050028)
#define APLL1_CTL11                           ((volatile uint32_t *)0xA205002C)
#define APLL1_CTL12                           ((volatile uint32_t *)0xA2050030)
#define APLL1_CTL13                           ((volatile uint32_t *)0xA2050034)
#define APLL1_CTL14                           ((volatile uint32_t *)0xA2050038)
#define APLL1_CTL15                           ((volatile uint32_t *)0xA205003C)
#define M_ANA_CFG_UPLL_IBANK                  ((volatile uint32_t *)0xA204011E)

#define APLL2_CTL0                            ((volatile uint32_t *)0xA2050100)
#define UPLL_CTL0                             ((volatile uint32_t *)0xA2040100)

#define SSC_CON0                              ((volatile uint32_t *)0xA2040300)
#define SSC_CON1                              ((volatile uint32_t *)0xA2040304)
#define SSC_CON2                              ((volatile uint32_t *)0xA2040308)
#define SSC_CON3                              ((volatile uint32_t *)0xA204030C)
#define SSC_CON4                              ((volatile uint32_t *)0xA2040310)
#define SW_DMA_ACT                            ((volatile uint32_t *)0xA0010000)
#define I2S_DMA_ACT                           ((volatile uint32_t *)0xC9000000)
//DCXO
#define DCXO_PCON0                            ((volatile uint32_t *)0xA2060000)
#define DCXO_DEBUG0                           ((volatile uint32_t *)0xA2060100)
#define DCXO_RSV                              ((volatile uint32_t *)0xA2060020)
#define DCXO_CTUNE_LPM                        ((volatile uint32_t *)0xA30D0008)	
#define DCXO_CORE_ISEL                        ((volatile uint32_t *)0xA30D0014)
#define DCXO_CORE_ISEL_LPM                    ((volatile uint32_t *)0xA30D0018)
#define INFRA_CG_MTCMOS3                      ((volatile uint32_t *)0xA220004C)

/*************************************************************************
 * Define clock mux register and bits structure
 *************************************************************************/


#endif /* HAL_CLOCK_MODULE_ENABLED */
#endif /* __HAL_CLOCK_PLATFORM_AB155X_H__*/
