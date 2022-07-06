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

#ifndef __HAL_CLOCK_INTERNAL_H__
#define __HAL_CLOCK_INTERNAL_H__

#include "hal_platform.h"
#include "hal_clock.h"
#ifdef HAL_CLOCK_MODULE_ENABLED
#define BUS_DCM_CON_REG                  ((bus_dcm_con*)BUS_DCM_CON_0)->field
#include "memory_attribute.h"
#include "hal_clock_platform.h"

typedef enum {
    clk_dsp_dcm,
}hal_clk_dcm_id;

typedef union {
    uint64_t value;
    struct {
        __IO uint8_t RG_BUS_SFSEL;
        __IO uint8_t RG_BUS_DCM_DBC_NUM;
        __IO uint8_t RG_BUS_DCM_DBC_EN;
        __IO uint8_t RG_BUS_DCM_EN;
        __IO uint8_t RG_BUS_CLKOFF_EN;
        __IO uint8_t RG_BUS_CLKSLOW_EN;
        __IO uint8_t RG_BUS_FORCE_CLK_OFF;
        __IO uint8_t RG_BUS_FORCE_CLKSLOW;
    } field;
}bus_dcm_con;
void hal_clock_dcm_enable(hal_clk_dcm_id dcm_id);
void hal_clock_dcm_disable(hal_clk_dcm_id dcm_id);

#endif //HAL_CLOCK_MODULE_ENABLED

#endif //__HAL_CLOCK_INTERNAL_H__

