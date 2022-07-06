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

#ifndef _HAL_WDT_INTERNAL_H_
#define _HAL_WDT_INTERNAL_H_

#include "hal_wdt.h"

#define WDT_1_TICK_LENGTH                   (156)   /* 15.6ms */

typedef enum {
    SYSTEM_RESET,
    DSPSYS_DEBUG_RESET,
    DSPSYS_CORE_RESET,
    CONNSYS_RESET,
    AUDIOSYS_RESET,
    USB_RESET,
    USBIF_RESET,
} wdt_reset_type_t;

void    wdt_set_length(uint32_t channel, uint32_t seconds);
void    wdt_set_mode(uint32_t channel, uint32_t value);
void    wdt_set_restart(uint32_t channel);
void    wdt_set_sw_rst(uint32_t channel);
void    wdt_set_enable(uint32_t channel, uint32_t enable);
bool    wdt_get_enable_status(uint32_t channel);
bool    wdt_clear_irq(uint32_t channel);
void    wdt_reset_default_value(uint32_t channel);
void    wdt_set_pmu_mask(uint32_t enable);
void    wdt_set_pmu_reset_polarity(uint32_t pmu_rst_inv_en);
void    wdt_clear_reset_status(wdt_reset_type_t type);
void    wdt_dump_register(uint32_t channel);
hal_wdt_reset_status_t  wdt_get_reset_status(uint32_t channel);
hal_wdt_mode_t          wdt_get_mode_status(uint32_t channel);

#endif

