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
 
#ifndef __HAL_CLOCK_PLATFORM_H__
#define __HAL_CLOCK_PLATFORM_H__

#include "hal_platform.h"
#include "ab156x.h"
#ifdef HAL_CLOCK_MODULE_ENABLED
#define RF_DESENSE  1
#define STR_VALUE(arg)      #arg
#define __stringify(name) STR_VALUE(name)

//Module name: cksys_bus_clk Base address: (+0xA2150000)
#define BUS_DCM_CON_0                         ((volatile uint32_t *)0xA21E0100)

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

#define XO_DCM_CON_0                          ((volatile uint32_t *)0xA2030C00)
#define XO_DCM_CON_1                          ((volatile uint32_t *)0xA2030C04)
#define XO_PDN_PD_COND0                       ((volatile uint32_t *)0xA2030B00)
#define XO_PDN_PD_SET0                        ((volatile uint32_t *)0xA2030B10)
#define XO_PDN_PD_CLRD0                       ((volatile uint32_t *)0xA2030B20)

#define XO_PDN_AO_COND0                       ((volatile uint32_t *)0xA2030B30)
#define XO_PDN_AO_SET0                        ((volatile uint32_t *)0xA2030B40)
#define XO_PDN_AO_CLRD0                       ((volatile uint32_t *)0xA2030B50)

#define XO_PDN_TOP_COND0                      ((volatile uint32_t *)0xA2030B60)
#define XO_PDN_TOP_SET0                       ((volatile uint32_t *)0xA2030B70)
#define XO_PDN_TOP_CLRD0                      ((volatile uint32_t *)0xA2030B80)
/*************************************************************************
 * Define clock mux register and bits structure
 *************************************************************************/


#endif /* HAL_CLOCK_MODULE_ENABLED */
#endif /* __HAL_CLOCK_PLATFORM_H__*/
