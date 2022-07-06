/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#ifndef __BATTERY_MANAGEMENT_HW_JEITA_H__
#define __BATTERY_MANAGEMENT_HW_JEITA_H__

typedef enum{
        RG_ICC_JC_20    = 0,
        RG_ICC_JC1_20   = 1,
        RG_ICC_JC2_20   = 2,
        RG_ICC_JC3_20   = 3,
        RG_ICC_JC_40    = 4,
        RG_ICC_JC_60    = 5,
        RG_ICC_JC_80    = 6,
        RG_ICC_JC_100   = 7,
}battery_managerment_jc_perecnt_level_t;

typedef enum
{
    RG_VCV_VOLTAGE_4P05V = 0,   //4.05V
    RG_VCV_VOLTAGE_4P10V = 1,   //4.10V
    RG_VCV_VOLTAGE_4P15V = 2,   //4.15V
    RG_VCV_VOLTAGE_4P20V = 3,   //4.20V
    RG_VCV_VOLTAGE_4P25V = 4,   //4.25V
    RG_VCV_VOLTAGE_4P30V = 5,   //4.30V
    RG_VCV_VOLTAGE_4P35V = 6,   //4.35V
    RG_VCV_VOLTAGE_4P40V = 7,   //4.40V
    RG_VCV_VOLTAGE_4P45V = 8,   //4.45V
    RG_VCV_VOLTAGE_4P50V = 9,   //4.50V
    RG_VCV_VOLTAGE_4P55V = 10,  //4.55V
    RG_VCV_VOLTAGE_4P60V = 11,  //4.60V
}battery_managerment_cv_voltage_t;

#define HW_JEITA_HOT_STAGE 0xF
#define HW_JEITA_WARM_STAGE 0xE
#define HW_JEITA_NORMAL_STAGE 0xC
#define HW_JEITA_COOL_STAGE 0x8
#define HW_JEITA_COLD_STAGE 0



void battery_core_hw_jeita_hot_zone(void);
void battery_core_hw_jeita_warm_zone(void);
void battery_core_hw_jeita_cool_zone(void);
void battery_core_hw_jeita_cold_zone(void);

void battery_core_hw_jeita_hot_setting(void);
void battery_core_hw_jeita_warm_setting(void);
void battery_core_hw_jeita_cold_setting(void);
void battery_core_hw_jeita_cool_setting(void);

void battery_core_hw_jeita_set_threshold(float auxadcVolt, uint8_t JeitaThreshold);
void battery_core_hw_jeita_enable_warmcool(void);

void battery_core_hw_jeita_init_threshold(void);
void battery_core_hw_jeita_init(void);
void battery_jetia_timer_callback(TimerHandle_t pxTimer);
void battery_jetia_create_check_timer(void);
uint8_t battery_get_hw_jeita_status(void);
#endif // __BATTERY_MANAGEMENT_HW_JEITA_H__
