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

#include "battery_management_core.h"
#include "battery_management_customer_table.h"
const auxadc_temp_to_volt_stru gAuxadcTempVoltTable[NUM_OF_AUXADC_TEMP_TO_VOLT_DATA] =
{
#include "chargerAuxadcTable.h"
};
auxadc_efuse_data_stru gAuxadcEfuseInfo;
int32_t gChargerTemp = 0;


float battery_auxadc_volt_calc_to_temp(uint8_t smallScaleIndex, float auxadc) {
    int tempL = gAuxadcTempVoltTable[smallScaleIndex].temp;
    int tempH = gAuxadcTempVoltTable[smallScaleIndex + 1].temp;
    float auxadcL = gAuxadcTempVoltTable[smallScaleIndex].volt;
    float auxadcH = gAuxadcTempVoltTable[smallScaleIndex + 1].volt;
    float trans_temp = 0;

    float slope = 0.0;
    if (auxadcL == auxadcH) {
        slope = 0;
    } else {
        slope = ((float) (tempL - tempH)) / ((float) (auxadcL - auxadcH));
    }
    trans_temp = tempL + (auxadc - auxadcL) * slope;
    return trans_temp;
}

int battery_auxadc_voltage_to_tempature(uint32_t auxadc) //charger tempatrue
{
    int largeIndex = 0;
    int smallIndex;
    int trans_temp = 0;
    if (auxadc >= gAuxadcTempVoltTable[0].volt) {
        smallIndex = 0;
    } else {
        largeIndex = battery_auxadc_find_top_scope_index(auxadc);
        smallIndex = battery_core_auxadc_find_bottom_scope_index(auxadc, largeIndex);
    }
    if (smallIndex == NUM_OF_AUXADC_TEMP_TO_VOLT_DATA - 1) {
        trans_temp = battery_auxadc_volt_calc_to_temp(smallIndex - 1, auxadc);
    } else {
        trans_temp = battery_auxadc_volt_calc_to_temp(smallIndex, auxadc);
    }
    gChargerTemp = trans_temp;
    return trans_temp;
}

uint8_t battery_auxadc_find_top_scope_index(uint32_t auxadcValue) {
    int i = 0;
    if (auxadcValue >= battery_auxadc_temp_to_volt_scope[0].volt) {
        return 0;
    } else if (auxadcValue <= battery_auxadc_temp_to_volt_scope[(NUM_OF_SCOPE - 1)].volt) {
        return (NUM_OF_SCOPE - 1);
    }
    for (i = 0; i < (NUM_OF_SCOPE-1); i++) {
        if (auxadcValue <= battery_auxadc_temp_to_volt_scope[i].volt &&
            auxadcValue > battery_auxadc_temp_to_volt_scope[i + 1].volt) {
            return i;
        }
    }
    return BATTERY_MANAGEMENT_STATUS_OK;
}

uint8_t battery_core_auxadc_find_bottom_scope_index(float auxadc, int index) {
    int i = 0;
    uint8_t lowerBound = index * 10;
    uint8_t higherBound = (index + 1) * 10;

    if (index == NUM_OF_SCOPE - 1) {
        higherBound = NUM_OF_AUXADC_TEMP_TO_VOLT_DATA - 1;
    }

    for (i = lowerBound; i < higherBound; i++) {
        if (auxadc <= gAuxadcTempVoltTable[i].volt &&
            auxadc > gAuxadcTempVoltTable[i + 1].volt) {
            return i;
        }
    }
    return i;
}

void battery_auxadc_parameter_init(void) {
    memset(&gAuxadcEfuseInfo, 0, sizeof(auxadc_efuse_data_stru));
    battery_efuse_enable_reading();
    gAuxadcEfuseInfo.byte0 = battery_core_efuse_get_efuse_data_by_address(AUXADC_EFUSE_ADDRESS_BYTE0 / 8);
    gAuxadcEfuseInfo.byte1 = battery_core_efuse_get_efuse_data_by_address(AUXADC_EFUSE_ADDRESS_BYTE1 / 8);
#ifdef BATTERY_MANAGER_DEBUG
    LOG_MSGID_I(battery_management, "Auxadc Efuse Data B0:%x\r\n",1, gAuxadcEfuseInfo.byte0);
    LOG_MSGID_I(battery_management, "Auxadc Efuse Data B1:%x\r\n",1, gAuxadcEfuseInfo.byte1);

    LOG_MSGID_I(battery_management, "OVST Value:%x\r\n",1, battery_auxadc_get_vts());
    LOG_MSGID_I(battery_management, "DEGC CALI Value:%x\r\n",1, battery_auxadc_get_degc_cali());
    LOG_MSGID_I(battery_management, "CALI EN Value:%x\r\n",1, battery_auxadc_get_adc_cali_en());
    LOG_MSGID_I(battery_management, "O SLOPE SIGN:%x\r\n",1, battery_auxadc_get_slope_sign_value());
    LOG_MSGID_I(battery_management, "O SLOPE :%x\r\n",1, battery_auxadc_get_efuse_slope_value());
    LOG_MSGID_I(battery_management, "CHECK ID:%x\r\n",1, battery_core_auxadc_check_id());
#endif
    battery_efuse_disable_reading();
    gAuxadcEfuseInfo.isInit = 1;
}
int32_t battery_get_pmic_temp(void) {
    return battery_auxadc_get_pmic_temperature();
}

int16_t battery_auxadc_get_slope_value(void) {
    int16_t slope = 0;
    if (battery_auxadc_get_slope_sign_value() == 1) {
        slope = ((int16_t) battery_auxadc_get_efuse_slope_value()) * (-1);
    } else {
        slope = ((int16_t) battery_auxadc_get_efuse_slope_value()) * (1);
    }
    return slope;
}
// Get pmic tempautre
uint32_t battery_auxadc_get_pmic_temperature(void) {
    uint32_t adc_Out = 0;

    uint32_t vts_value = 0;

    if (!gAuxadcEfuseInfo.isInit) {
        LOG_MSGID_I(battery_management, "Efuse Data Is Not Init\r\n", 0);
    }

    if (!battery_auxadc_get_adc_cali_en()) {
        LOG_MSGID_I(battery_management, "TempValue in Efuse is Not Calibration\r\n", 0);
    }
    battery_core_auxadc_thermal_sensor_buffer_on(0);

    adc_Out = pmu_auxadc_get_channel_value(PMU_AUX_PMIC_AP);

    vts_value = battery_auxadc_voltage_transfer((int16_t) battery_auxadc_get_vts());
    battery_core_auxadc_thermal_sensor_buffer_on(1);
    return battery_auxadc_calculate_current_temperature(battery_auxadc_get_slope_value(), (int32_t) vts_value, (int32_t) adc_Out,battery_core_auxadc_get_degc_value());

}

//--------Read Efuse Value------
uint8_t battery_auxadc_get_slope_sign_value(void) {
    return ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_O_SLOPE_SIGNED_IN_B1) >> 10);
}

uint8_t battery_auxadc_get_efuse_slope_value(void) {
    return ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_O_SLOPE_IN_B1) >> 4);
}

uint8_t battery_auxadc_get_id(void) {
    return ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_ID_IN_B1) >> 11);
}

uint16_t battery_auxadc_get_vts(void) {
    return ((gAuxadcEfuseInfo.byte0 & AUXADC_EFUSE_MASK_OVTS_IN_BO) >> 7) + ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_MASK_OVTS_IN_B1) << 9);
}

uint8_t battery_auxadc_get_degc_cali(void) {
    return (gAuxadcEfuseInfo.byte0 & AUXADC_EFUSE_MASK_DEGC_CALI_IN_B0);
}

uint8_t battery_auxadc_get_adc_cali_en(void) {
    return ((gAuxadcEfuseInfo.byte0 & AUXADC_EFUSE_MASK_ADC_CALI_EN_IN_B0) >> 6);
}

uint8_t battery_core_auxadc_check_id(void) {
    return ((gAuxadcEfuseInfo.byte1 & AUXADC_EFUSE_ID_IN_B1) >> 11);
}

int8_t battery_core_auxadc_get_degc_value(void) {
    int8_t degc = 0;
    if (battery_auxadc_get_degc_cali() < 38 || battery_auxadc_get_degc_cali() > 60) {
        degc = 53;
    } else {
        degc = battery_auxadc_get_degc_cali();
    }
    return degc;
}

uint32_t battery_auxadc_voltage_transfer(int16_t value) {
    uint32_t miniVoltage = -1;
    miniVoltage = (value * 1800) / 4096;
    return miniVoltage;
}

/*
Tcurrent = DEGC / 2 + (Ycurr - VBE_T)*1e-3 / {-[0.0016754 + (1 - 2 * O_SLOPE_SIGN)*O_SLOPE / 100000]}
Note: O_SLOPE is signed integer.
*/
uint32_t battery_auxadc_calculate_current_temperature(int16_t slope_value, int32_t yBet, int32_t yCurr, int8_t degcValue) {
    int32_t PmicTemp;
    int32_t tempValue = 0;
    int32_t tempValue2 = 0;
    int32_t temperature = 0;
    int32_t tempValue3 = 0;
    int32_t tempValue4 = 0;
    tempValue3 = (1 - 2 * battery_auxadc_get_slope_sign_value()) * ((float) slope_value) / 100000;
    tempValue = (-1) * ((16754/10000000) + tempValue3);
    tempValue2 = ((float) (yCurr - yBet)) * ((float) (0.001));
    tempValue4 = tempValue2 / tempValue;
    temperature = ((float) degcValue) / 2 + tempValue4;
    PmicTemp = temperature;
    return PmicTemp;
}
