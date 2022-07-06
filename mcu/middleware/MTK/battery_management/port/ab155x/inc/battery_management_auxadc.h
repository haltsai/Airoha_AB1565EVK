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

#ifndef __BATTERY_MANAGEMENT_AUXADC_H__
#define __BATTERY_MANAGEMENT_AUXADC_H__


#define NUM_OF_AUXADC_TEMP_TO_VOLT_DATA 166
#define NUM_OF_SCOPE 17

#define AUXADC_EFUSE_ADDRESS_BYTE0 848
#define AUXADC_EFUSE_ADDRESS_BYTE1 864

#define AUXADC_EFUSE_MASK_DEGC_CALI_IN_B0   0x003F
#define AUXADC_EFUSE_MASK_ADC_CALI_EN_IN_B0 0x0040
#define AUXADC_EFUSE_MASK_OVTS_IN_BO        0xFF80
#define AUXADC_EFUSE_MASK_OVTS_IN_B1        0x000F
#define AUXADC_EFUSE_O_SLOPE_IN_B1          0x03F0
#define AUXADC_EFUSE_O_SLOPE_SIGNED_IN_B1   0x0400
#define AUXADC_EFUSE_ID_IN_B1               0x0800

typedef struct
{
    uint8_t isInit:1;
    uint8_t reserved:7;
    uint16_t byte0;
    uint16_t byte1;
    uint16_t matchKey;
}auxadc_efuse_data_stru;


typedef struct {
    int temp;
    int volt;
} auxadc_temp_to_volt_stru;


void battery_auxadc_parameter_init(void);
int battery_auxadc_voltage_to_tempature(uint32_t auxadc);
uint8_t battery_auxadc_find_top_scope_index(uint32_t auxadcValue);
uint8_t battery_core_auxadc_find_bottom_scope_index(float auxadc, int index);
float battery_auxadc_volt_calc_to_temp(uint8_t smallScaleIndex, float auxadc);
int16_t battery_auxadc_get_slope_value(void);
uint8_t battery_auxadc_get_slope_sign_value(void);
uint8_t battery_auxadc_get_efuse_slope_value(void);
uint8_t battery_auxadc_get_id(void);
uint16_t battery_auxadc_get_vts(void);
uint8_t battery_auxadc_get_degc_cali(void);
uint32_t battery_auxadc_voltage_transfer(int16_t value);
uint32_t battery_auxadc_calculate_current_temperature(int16_t oSlopeValue, int32_t yBet, int32_t yCurr, int8_t degcValue);
uint32_t battery_auxadc_get_pmic_temperature(void);  // Get PMIC temperature
int8_t battery_core_auxadc_get_degc_value(void);
uint8_t battery_auxadc_get_adc_cali_en(void);
int32_t battery_get_pmic_temp(void);
uint8_t battery_core_auxadc_check_id(void);

#endif // __BATTERY_MANAGEMENT_AUXADC_H__
