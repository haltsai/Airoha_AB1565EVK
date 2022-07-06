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

#ifndef __BATTERY_MANAGEMENT_SW_NTC_H__
#define __BATTERY_MANAGEMENT_SW_NTC_H__

#include "hal_platform.h"

#define hal_ntc_write32_data16(addr,data)   ((*(volatile uint32_t *)(addr)) = (uint16_t)(data))
#define hal_ntc_read32(addr)                (*(volatile uint32_t *)(addr))

#define NTC_NVKEY_SIZE                  81
#define NTC_NVKEY_SIZE_INDEX            0
#define NTC_NVKEY_SMALL_START_INDEX     1
#define NTC_NVKEY_SMALL_END_INDEX       35
#define NTC_NVKEY_MID_INDEX             36
#define NTC_NVKEY_LARGE_START_INDEX     37
#define NTC_NVKEY_LARGE_END_INDEX       80
#define NTC_NVKEY_SIZE                  81
#define NTC_INR2_POPULATION             10000


#define EFUSE_INTERNAL_R2_DEFULT        11693
#define EFUSE_INTERNAL_RAGPIO_DEFULT    719

#define EFUSE_INTERNAL_R2_AB1568    0xA20A0408
#define EFUSE_INTERNAL_R2_AB1565    0xA20A0408
#define EFUSE_RAGPIO_AB1568     0xA20A0404
#define EFUSE_RAGPIO_AB1565     0xA20A0404


typedef enum
{
    /* plug in charger */
    sw_ntc_charger_cold = 0,
    sw_ntc_charger_cool = 8,
    sw_ntc_charger_normal = 12,
    sw_ntc_charger_warm = 14,
    sw_ntc_charger_hot = 15,

    /* not plug in charger */
    sw_ntc_nocharger_power_off_low = 16,
    sw_ntc_nocharger_keep_state_low = 17,
    sw_ntc_nocharger_normal = 18,
    sw_ntc_nocharger_keep_state_high = 19,
    sw_ntc_nocharger_power_off_high = 20,

} sw_ntc_jeita_status;

typedef enum
{
    battery_sw_ntc_sucess = 0,
    battery_sw_ntc_init_sucess,
    battery_sw_ntc_readnv_fail,
    battery_sw_ntc_not_enable,
    battery_sw_ntc_no_efuseotp,
    battery_sw_ntc_table_not_done,
    battery_sw_ntc_ratio_error,
    battery_sw_ntc_get_time_error,
    battery_sw_ntc_find_25_error,
    
} battery_management_sw_ntc_status;


typedef struct
{
    uint8_t enable;
    uint8_t caculated_done;
    uint8_t rsv2;
    uint8_t rsv3;
    uint8_t rsv4;
    uint8_t rsv5;
    uint8_t avr_count;
    uint8_t t1;
    uint8_t rsv6;
    uint8_t t2;
    uint8_t dis_count;
    uint32_t rsv;
    uint8_t power_on_delay_time;
} hal_ntc_basic_config;

typedef struct
{
    signed char rishH;
    signed char abnH;
    signed char norH;
    signed char norL;
    signed char abnL;
    signed char rishL;
    signed char rsv1;
    signed char rsv2;
    signed char rsv3;
    signed char rsv4;
    signed char secH;
    signed char noH;
    signed char noL;
    signed char secL;
    signed char rsv5;
    signed char rsv6;
    signed char rsv7;
    signed char rsv8;
} hal_ntc_temp_config;


typedef struct
{
    uint8_t enable;
    uint8_t table_done;
    uint8_t t1;
    uint8_t t2;
    uint8_t avg;
    int16_t char_ratio[4];               
    int16_t unchar_ratio[4];             
    uint8_t power_on_delay_time;          
} hal_ntc_para;

battery_management_sw_ntc_status battery_management_sw_ntc_init(void);
battery_management_sw_ntc_status battery_management_sw_ntc_timer_start(void );
battery_management_sw_ntc_status battery_management_sw_ntc_get_interval(uint8_t *time);
battery_management_sw_ntc_status battery_management_sw_ntc_get_r2(uint16_t *ret);
bool battery_management_sw_ntc_slt(void);
void battery_management_sw_ntc_rario_to_temper(int ratio, int *ret_temper);
void battery_management_sw_ntc_caculate_slope(uint32_t ratio, uint32_t *data);
void battery_management_sw_ntc_enable_adc(void);
void battery_management_sw_ntc_get_adc_value(uint32_t *ret_data);
void battery_management_sw_ntc_disable_adc(void);
//for hqa
void battery_management_sw_ntc_temp(void );

//for battery call
void battery_management_sw_ntc_retrive(int *temperature, sw_ntc_jeita_status *jeita_curr, sw_ntc_jeita_status *jeita_next);

#endif

