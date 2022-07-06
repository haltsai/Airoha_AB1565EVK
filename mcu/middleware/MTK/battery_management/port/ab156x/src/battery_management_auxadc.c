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


int32_t battery_get_pmic_temp(void) {
    return pmu_auxadc_get_pmic_temperature();
}
