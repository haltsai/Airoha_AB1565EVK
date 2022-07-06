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
#include "timers.h"
#include "hal_sleep_manager_platform.h"
#ifdef AB1565
#include "hal_pmu_charger_2565.h"
#endif
#ifdef AB1568
#include "hal_pmu_ab2568_platform.h"
#endif

int *voltage_table=NULL;
uint8_t gauge_timer_count = 0;                              /*Data : For gauge calculation*/
uint8_t gauge_regular_second_time;                          /*Data : For gauge calculation*/
extern battery_basic_data bm_cust;                          /*Data : restore battery basic data*/
extern battery_managerment_control_info_t bm_ctrl_info;     /*Data : restore battery basic data*/
int battery_linear_soc[LINEAR_GAUGE_SIZE];                  /*Data : For linear gauge soc smooth*/
int battery_linear_bat[LINEAR_GAUGE_SIZE];                  /*Data : For linear gauge soc smooth*/
uint8_t linear_gauge_array_flag =0 ;                        /*flag : For linear gauge soc array init*/

uint32_t smooth_soc=0;
uint32_t smooth_vbat_voltage=0;
int linear_array_index=0;
uint8_t linear_dischargr_step_flag =BM_DISCHARGER_NOT_INIT ;                        /*flag : For linear gauge soc array init*/
uint8_t linear_charging_step_flag =BM_CHARGING_NOT_INIT ;                        /*flag : For linear gauge soc array init*/

uint32_t older_value = 0;
#ifdef BATTERY_DECREASE_RESISTANCE
extern uint32_t remove_ir_soc;
extern uint32_t remove_ir_vbat;
int cc_value[67] ={0.5,1,1.5,2,2.5,3,3.5,4,4.5,5,
                    10,15,20,25,30,35,40,45,50,55,
                    60,65,70,75,80,90,100,110,120,130,
                    140,150,160,170,180,190,200,210,220,230,
                    240,250,260,270,280,290,300,310,320,330,
                    340,350,360,370,380,390,400,410,420,430,
                    440,450,460,470,480,490,500}; /*Index : For linear gauge*/
#endif
/*==========[Battery Management:Gague]==========*/
void battery_linear_gauge_task(void * pvParameters) {
#ifdef AB1568
    TickType_t xLastWakeTime;
    uint32_t temp_batsns=0,temp_soc=0;
    uint32_t cc_index=0;
    while (1) {
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil(&xLastWakeTime, LINEAR_REGULAR_GAUGE_TIME / portTICK_RATE_MS);
        if (battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST)) {
#ifdef DECREASE_CC_OBTAIN_VABT
            cc_index = pmu_get_charger_current_index();
            if(battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY)<BATTERY_STABLE_CHARGING_VOLTAGE){
                pmu_set_charger_current(pmu_fastcc_chrcur_2mA);
                temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
                pmu_set_charger_current(cc_index);
#ifdef BATTERY_DECREASE_RESISTANCE
                uint8_t index=0;
                LOG_MSGID_I(battery_management, "[BM]BATTERY_DECREASE_RESISTANCE enable", 0);
                index = battery_gauge_get_refernece_index(get_smooth_vbat());
                if(index > 8){
                   index = 8;
                }
                if((get_smooth_vbat() >remove_ir_vbat)&&(battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY) > remove_ir_soc)){
                    temp_soc = battery_core_gauge_function(get_smooth_vbat());
                }else{
                    temp_soc = battery_core_gauge_function(get_smooth_vbat()-(cc_value[cc_index]*bm_cust.resist_offset[index]/1000));
                }
#else
                LOG_MSGID_I(battery_management, "[BM]BATTERY_DECREASE_RESISTANCE disable", 0);
                temp_soc = battery_core_gauge_function(get_smooth_vbat());
#endif
            }else{
                temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
                temp_soc = battery_core_gauge_function(get_smooth_vbat());
            }
#else
            temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
            temp_soc = battery_core_gauge_function(temp_batsns);
#endif
            battery_smooth_charging_soc(temp_soc,temp_batsns);
            LOG_MSGID_I(battery_management, "[BM]battery_linear_gauge_task  VBUS on smooth_SOC:%d SOC%d", 2,get_smooth_soc(),temp_soc);
        }else{
            temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
            temp_soc = battery_core_gauge_function(temp_batsns);
            battery_smooth_discharger_soc(temp_soc,temp_batsns);
            LOG_MSGID_I(battery_management, "[BM]battery_linear_gauge_task  VBUS off smooth_SOC:%d SOC%d", 2,get_smooth_soc(),temp_soc);
        }
    }
#endif
#ifdef AB1565
    TickType_t xLastWakeTime;
    uint32_t temp_batsns=0,temp_soc=0;
    uint32_t temp_vbat_value =0;
    while (1) {
        xLastWakeTime = xTaskGetTickCount();
        vTaskDelayUntil(&xLastWakeTime, LINEAR_REGULAR_GAUGE_TIME / portTICK_RATE_MS);
        temp_vbat_value= pmu_bat_get_pure_adc();
        temp_batsns =pmu_bat_adc_to_volt(temp_vbat_value);
        temp_soc = pmu_bat_adc_to_perc(temp_vbat_value);
        if (battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST)) {
            battery_smooth_charging_soc(temp_soc,temp_batsns);
            LOG_MSGID_I(battery_management, "[BM]battery_linear_gauge_task  VBUS on smooth_SOC:%d SOC%d", 2,get_smooth_soc(),temp_soc);
        }else{
            battery_smooth_discharger_soc(temp_soc,temp_batsns);
            LOG_MSGID_I(battery_management, "[BM]battery_linear_gauge_task  VBUS off smooth_SOC:%d SOC%d", 2,get_smooth_soc(),temp_soc);
        }
    }
#endif
}

void battery_calibration_gauge_tolerance(void){
    if (pmu_get_charger_state() == 0x2) {
        gauge_timer_count++;
        if (gauge_timer_count == gauge_regular_second_time) {
            bm_ctrl_info.gauge_calibration += 1;
            gauge_timer_count = 0;
        }
        if (bm_ctrl_info.gauge_calibration >= 5) {
            bm_ctrl_info.gauge_calibration = 5;
        }
    } else if (pmu_get_charger_state() != 0x2 && bm_ctrl_info.gauge_calibration > 0) {
        gauge_timer_count++;
        if((bm_ctrl_info.feature.charger_option==1)&&(bm_ctrl_info.isChargerExist==1)){
            bm_ctrl_info.gauge_calibration =5;
        }else if (gauge_timer_count == gauge_regular_second_time) {
            bm_ctrl_info.gauge_calibration -= 1;
            gauge_timer_count = 0;
        }
        if (bm_ctrl_info.gauge_calibration <= 0) {
            bm_ctrl_info.gauge_calibration = 0;
        }
    }
}
/* ste 0:Vbus plug-out
 * ste 1:Vbus plug-in */
void battery_switch_calibration_state(uint8_t ste){
    if(ste){
        if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) > (bm_cust.full_bat - bm_cust.full_bat_offset)) {
            bm_ctrl_info.gauge_calibration = ((pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) - (bm_cust.full_bat - bm_cust.full_bat_offset)) / 10) + 1;
        } else {
            bm_ctrl_info.gauge_calibration = 0;
        }
    }else{
        if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) > (bm_cust.full_bat - bm_cust.full_bat_offset) && bm_ctrl_info.gauge_calibration == 0) {
            bm_ctrl_info.gauge_calibration = ((pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) - (bm_cust.full_bat - bm_cust.full_bat_offset)) / 10);
        }
        gauge_timer_count = 0;
    }
}
/*How many seconds will be divided into six times to reaching 100% */
void battery_set_calibration_time(void) {
    if((CHARGER_TOLERANCE_TIME * 60)<=CHARGER_REGULAR_TIME){
        gauge_regular_second_time = 1;
    }else{
        gauge_regular_second_time = ((CHARGER_TOLERANCE_TIME * 60) / CHARGER_REGULAR_TIME) / 6;
    }
}
/*Linear gauge use smooth SOC and VBAT*/
void battery_smooth_charging_soc(uint32_t soc_value, uint32_t batsns) {
    LOG_MSGID_I(battery_management, "charging [soc_value: %d][batsns :%d][step:%d]", 3,soc_value,batsns,linear_charging_step_flag);
    int i = 0;
    uint32_t temp_soc = 0;
    uint32_t temp_bat = 0;

        if (linear_charging_step_flag == BM_CHARGING_NOT_INIT) {
            linear_array_index = 0;
            for(i=0;i<LINEAR_GAUGE_SIZE;i++){
                battery_linear_soc[i] = soc_value;
                battery_linear_bat[i] = batsns;
            }
            linear_charging_step_flag = BM_CHARGING_START;
            linear_array_index++;
        } else if (linear_charging_step_flag == BM_CHARGING_START) {
            linear_array_index = 0;
            for (i = 0; i < LINEAR_GAUGE_SIZE; i++) {
                battery_linear_soc[i] = smooth_soc;
                battery_linear_bat[i] = smooth_vbat_voltage;
            }
            linear_charging_step_flag = BM_CHARGING_STAGE;
            linear_array_index++;
        } else if (linear_charging_step_flag == BM_CHARGING_STAGE) {
            battery_linear_soc[linear_array_index] = soc_value;
            battery_linear_bat[linear_array_index] = batsns;
            linear_array_index++;
            if (linear_array_index == LINEAR_GAUGE_SIZE) {
                linear_array_index = 0;
            }
            linear_charging_step_flag = BM_CHARGING_STAGE;
        } else {
            LOG_MSGID_I(battery_management, "Charging step error", 0);
        }

    for (i = 0; i < LINEAR_GAUGE_SIZE; i++) {
        temp_soc += battery_linear_soc[i];
        temp_bat += battery_linear_bat[i];
    }
    smooth_soc = (temp_soc/LINEAR_GAUGE_SIZE);
    smooth_vbat_voltage = (temp_bat/LINEAR_GAUGE_SIZE);
    linear_dischargr_step_flag=BM_DISCHARGER_START;
    LOG_MSGID_I(battery_management, "Charging smooth [soc: %d Vbat :%d]", 2,smooth_soc,smooth_vbat_voltage);
}
void battery_smooth_discharger_soc(uint32_t soc_value,uint32_t batsns){
    LOG_MSGID_I(battery_management, "Discharger [soc_value: %d][batsns :%d][step:%d]", 3,soc_value,batsns,linear_dischargr_step_flag);
    int i = 0;
    uint32_t temp_soc=0;
    uint32_t temp_bat=0;

    if (linear_dischargr_step_flag == BM_DISCHARGER_NOT_INIT) {
            linear_array_index = 0;
            for(i=0;i<LINEAR_GAUGE_SIZE;i++){
                battery_linear_soc[i] = soc_value;
                battery_linear_bat[i] = batsns;
            }
            linear_dischargr_step_flag = BM_DISCHARGER_START;
            linear_array_index++;
        } else if (linear_dischargr_step_flag == BM_DISCHARGER_START) {
            linear_array_index = 0;
            for (i = 0; i < LINEAR_GAUGE_SIZE; i++) {
                battery_linear_soc[i] = smooth_soc;
                battery_linear_bat[i] = smooth_vbat_voltage;
            }
            linear_dischargr_step_flag = BM_DISCHARGER_STAGE;
            linear_array_index++;
        } else if (linear_dischargr_step_flag == BM_DISCHARGER_STAGE) {
            battery_linear_soc[linear_array_index] = soc_value;
            battery_linear_bat[linear_array_index] = batsns;
            linear_array_index++;
            if (linear_array_index == LINEAR_GAUGE_SIZE) {
                linear_array_index = 0;
            }
            linear_dischargr_step_flag = BM_DISCHARGER_STAGE;
        } else {
            LOG_MSGID_I(battery_management, "Discharger step error", 0);
        }


    for (i = 0; i < LINEAR_GAUGE_SIZE; i++) {
        temp_soc += battery_linear_soc[i];
        temp_bat += battery_linear_bat[i];
    }
    smooth_soc = (temp_soc/LINEAR_GAUGE_SIZE);
    smooth_vbat_voltage = (temp_bat/LINEAR_GAUGE_SIZE);
    linear_charging_step_flag = BM_CHARGING_START;
    LOG_MSGID_I(battery_management, "Discharger smooth [soc: %d Vbat :%d]", 2,smooth_soc,smooth_vbat_voltage);
}

uint32_t get_smooth_soc(void){
    return smooth_soc;
}
uint32_t get_smooth_vbat(void){
    return smooth_vbat_voltage;
}
uint32_t battery_get_gauge_percent(void) {
    uint32_t interpolationVoltage = 0;
    interpolationVoltage = get_smooth_soc();
    if(MTK_BATTERY_ULTRA_LOW_BAT>=interpolationVoltage){
        interpolationVoltage =MTK_BATTERY_ULTRA_LOW_BAT;
    }
    return interpolationVoltage;
}
uint32_t battery_get_gauge_percent_level(void){
    return (int)(battery_get_gauge_percent()/10);
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
        xAxisDiff = (float) (vBatSnsValue - bm_cust.shutdown_bat);
        interPolationValue = 0 + slope * xAxisDiff;
    } else if (index == BATTERY_VOLTAGE_REFERENCE_POINTS) {
        xAxisDiff = (float) (vBatSnsValue - voltage_table[BATTERY_VOLTAGE_REFERENCE_POINTS - 1]);
        interPolationValue = 90 + slope * xAxisDiff;
        if (vBatSnsValue > (bm_cust.full_bat - bm_cust.full_bat_offset)) {
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
        dataX0 = bm_cust.shutdown_bat;
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

void battery_gauge_init(void) {
#ifdef AB1568
    uint32_t temp_batsns = 0, temp_soc = 0;
    uint32_t cc_index=0;
    if (battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST)) {
#ifdef DECREASE_CC_OBTAIN_VABT
        cc_index = pmu_get_charger_current_index();
        if(battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY)<BATTERY_STABLE_CHARGING_VOLTAGE){
            pmu_set_charger_current(pmu_fastcc_chrcur_2mA);
            temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
            pmu_set_charger_current(cc_index);
            temp_soc = battery_core_gauge_function(temp_batsns);
        }else{
            temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
            temp_soc = battery_core_gauge_function(temp_batsns);
        }
#else
        temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
        temp_soc = battery_core_gauge_function(temp_batsns);
#endif
        battery_smooth_charging_soc(temp_soc,temp_batsns);
        LOG_MSGID_I(battery_management, "battery_linear_gauge_task  VBUS on smooth_SOC:%d SOC%d", 2,get_smooth_soc(),temp_soc);
    }else{
        temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
        temp_soc = battery_core_gauge_function(temp_batsns);
        battery_smooth_discharger_soc(temp_soc,temp_batsns);
        LOG_MSGID_I(battery_management, "battery_linear_gauge_task  VBUS off smooth_SOC:%d SOC%d", 2,get_smooth_soc(),temp_soc);
    }
#endif
#ifdef AB1565
    uint32_t temp_batsns=0,temp_soc=0;
    uint32_t temp_vbat_value =0;
    temp_vbat_value= pmu_bat_get_pure_adc();
        temp_batsns =pmu_bat_adc_to_volt(temp_vbat_value);
        temp_soc = pmu_bat_adc_to_perc(temp_vbat_value);
        if (battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST)) {
            battery_smooth_charging_soc(temp_soc,temp_batsns);
            LOG_MSGID_I(battery_management, "battery_linear_gauge_task  VBUS on smooth_SOC:%d SOC%d", 2,get_smooth_soc(),temp_soc);
        }else{
            battery_smooth_discharger_soc(temp_soc,temp_batsns);
            LOG_MSGID_I(battery_management, "battery_linear_gauge_task  VBUS off smooth_SOC:%d SOC%d", 2,get_smooth_soc(),temp_soc);
        }
#endif
}

