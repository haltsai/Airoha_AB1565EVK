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
extern battery_basic_data bm_cust;

int *voltage_table=NULL;
extern battery_managerment_control_info_t bm_ctrl_info;

battery_gauge_ctrl_stru	gGaugeCtrlInfo;

uint32_t gVBatSnS = 0;
uint32_t battery_get_linear_gauge_percent(void) {
    uint32_t interpolationVoltage = 0;
    interpolationVoltage = battery_core_gauge_function(pmu_auxadc_get_channel_value(PMU_AUX_BATSNS));
    return interpolationVoltage;
}
uint32_t battery_get_linear_gauge_percent_level(void){
    return (int)(battery_get_linear_gauge_percent()/10);
}
unsigned char battery_gauge_get_refernece_index(signed short vBatSnsValue) {
    unsigned char index;
    for (index = 0; index < BATTERY_VOLTAGE_REFERENCE_POINTS; index++) {
        if (vBatSnsValue < voltage_table[index]) {
            break;
        }
    }
    return index;
}

uint32_t battery_core_gauge_function(signed short vBatSnsValue)
{
    unsigned char index = 0;
    float slope;
    uint32_t interPolationValue = 0;
    short xAxisDiff = 0;
    voltage_table = bm_cust.check_point;
    index = battery_gauge_get_refernece_index(vBatSnsValue);
    slope = battery_core_gauge_indexCalc(index);
    if (index == 0) {
        xAxisDiff = (float) (vBatSnsValue - bm_cust.initial_bat);
        interPolationValue = 0 + slope * xAxisDiff;
    } else if (index == BATTERY_VOLTAGE_REFERENCE_POINTS) {
        xAxisDiff = (float) (vBatSnsValue - voltage_table[BATTERY_VOLTAGE_REFERENCE_POINTS - 1]);
        interPolationValue = 90 + slope * xAxisDiff;
        if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) > (bm_cust.full_bat - bm_cust.full_bat_offset)) {
            interPolationValue = GAUGE_TOLERANCE_PERCENT+bm_ctrl_info.gauge_calibration;
        }
        if(interPolationValue>100){
            interPolationValue = 100;
        }
    } else {
        xAxisDiff = (float) (vBatSnsValue - voltage_table[index - 1]);
        interPolationValue = (index * 10) + slope * xAxisDiff;
    }
    return interPolationValue;
}

float battery_core_gauge_indexCalc(unsigned char index) {
    short dataX0, dataX1 = 0;
    short dataY0, dataY1 = 0;
    float slope;

    if (index == 0) {
        dataX0 = bm_cust.initial_bat;
        dataX1 = voltage_table[0];

        dataY0 = 0;
        dataY1 = 10;
    } else if (index == BATTERY_VOLTAGE_REFERENCE_POINTS) {
        dataX0 = voltage_table[BATTERY_VOLTAGE_REFERENCE_POINTS - 1];
        dataX1 = bm_cust.full_bat;

        dataY0 = BATTERY_VOLTAGE_REFERENCE_POINTS * 10;
        dataY1 = 100;
    } else {
        dataX0 = voltage_table[index - 1];
        dataX1 = voltage_table[index];
        dataY0 = index * 10;
        dataY1 = (index + 1) * 10;
    }

    if (dataX1 == dataX0)
        return 0.0;

    slope = ((float) (dataY1 - dataY0)) / ((float) (dataX1 - dataX0));
    return slope;
}


