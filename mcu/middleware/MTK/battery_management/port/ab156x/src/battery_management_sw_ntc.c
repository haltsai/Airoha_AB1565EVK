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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "battery_management_sw_ntc.h"
#include "nvdm.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "hal_adc.h"
#include "hal_gpt.h"
#include "hal_flash_disk_internal.h"
#include "hal_log.h"
#include "hal_pmu.h"
#include "hal_gpio.h"
#include "hal_pmu_charger_2565.h"

#include "FreeRTOS.h"
#include "timers.h"

static hal_ntc_basic_config ntc_setting = {0,0,0,0,0,0,0,0,0,0,0,0,0};
static hal_ntc_temp_config ntc_temp_setting = {0};
static hal_ntc_para ntc_para = {0};

TimerHandle_t pNTCTimer;

static bool ntc_slt_feature = false;
signed char ntc_sw_temp_global_setting[NTC_NVKEY_SIZE];
unsigned short ntc_sw_ratio_global_setting[NTC_NVKEY_SIZE];

#define BATTERY_MANAGEMENT_TABLE_DUMP true
/*-------------------------------------------------------*/

/*==========[SW NTC API]==========*/
void battery_management_sw_ntc_retrive(int *temperature, sw_ntc_jeita_status *jeita_curr,
        sw_ntc_jeita_status *jeita_next) {
#if 0 //local testing
    *temperature = 25;
    if(jeita_curr != NULL)
        *jeita_curr= sw_ntc_charger_normal;

    if(jeita_next != NULL)
        *jeita_next= sw_ntc_charger_normal;
#else
    uint32_t adc_value;
    uint32_t ratio_value;
    int temper = 0;
    bool charger_exist = false;

    battery_management_sw_ntc_enable_adc();
    battery_management_sw_ntc_get_adc_value(&adc_value);
    battery_management_sw_ntc_disable_adc();
    battery_management_sw_ntc_caculate_slope(adc_value, &ratio_value);
    battery_management_sw_ntc_rario_to_temper(ratio_value, &temper);

    if (temperature != NULL)
        *temperature = temper;

    charger_exist = pmu_charger_is_plug();

    if (jeita_curr != NULL) {
        if (charger_exist) {
            if (temper >= ntc_para.char_ratio[3]) { /*Risk High*/
                *jeita_curr = sw_ntc_charger_hot;
                log_hal_msgid_info("[battery][sw_ntc] charger[ON]temper[%d]state[hot]\n", 1, temper);
            } else if (temper >= ntc_para.char_ratio[2] && (temper < ntc_para.char_ratio[3])) { /*Abnormal High*/
                *jeita_curr = sw_ntc_charger_warm;
                log_hal_msgid_info("[battery][sw_ntc] charger[ON]temper[%d]state[warm]\n", 1, temper);
            } else if (temper <= ntc_para.char_ratio[1] && (temper > ntc_para.char_ratio[0])) { /*Abnormal Low*/
                *jeita_curr = sw_ntc_charger_cool;
                log_hal_msgid_info("[battery][sw_ntc] charger[ON]temper[%d]state[cool]\n", 1, temper);
            } else if (temper <= ntc_para.char_ratio[0]) { /*Risk Low*/
                *jeita_curr = sw_ntc_charger_cold;
                log_hal_msgid_info("[battery][sw_ntc] charger[ON]temper[%d]state[cold]\n", 1, temper);
            } else { /*Normal*/
                *jeita_curr = sw_ntc_charger_normal;
                log_hal_msgid_info("[battery][sw_ntc] charger[ON]temper[%d]state[normal]\n", 1, temper);
            }
        } else {
            if (temper >= ntc_para.unchar_ratio[3]) { /*Security High*/
                *jeita_curr = sw_ntc_nocharger_power_off_high;
                log_hal_msgid_info("[battery][sw_ntc] charger[OFF]temper[%d]state[power_off_high]\n", 1, temper);
            } else if (temper >= ntc_para.unchar_ratio[2] && (temper < ntc_para.unchar_ratio[3])) {
                *jeita_curr = sw_ntc_nocharger_keep_state_high; /*Normal High*/
                log_hal_msgid_info("[battery][sw_ntc] charger[OFF]temper[%d]state[keep_high]\n", 1, temper);
            } else if (temper <= ntc_para.unchar_ratio[1] && (temper > ntc_para.unchar_ratio[0])) {
                *jeita_curr = sw_ntc_nocharger_keep_state_low; /*Normal Low*/
                log_hal_msgid_info("[battery][sw_ntc] charger[OFF]temper[%d]state[keep_low]\n", 1, temper);
            } else if (temper <= ntc_para.unchar_ratio[0]) { /*Security Low*/
                *jeita_curr = sw_ntc_nocharger_power_off_low;
                log_hal_msgid_info("[battery][sw_ntc] charger[OFF]temper[%d]state[power_off_low]\n", 1, temper);
            } else { /*Normal*/
                *jeita_curr = sw_ntc_nocharger_normal;
                log_hal_msgid_info("[battery][sw_ntc] charger[OFF]temper[%d]state[normal]\n", 1, temper);
            }
        }
    }
#endif
}

battery_management_sw_ntc_status battery_management_sw_ntc_check_enable(void) {
    battery_management_sw_ntc_status ret = battery_sw_ntc_sucess;
    log_hal_msgid_info("[sw_ntc] battery_management_sw_ntc_check_enable [%d]", 1, ntc_para.enable);
    if (ntc_para.enable != 1)
        ret = battery_sw_ntc_not_enable;
    return ret;
}

/*==========[SW NTC detect temperature calculation function]==========*/

void battery_management_sw_ntc_rario_to_temper(int ratio, int *ret_temper) {
    int ptr;
    int index = 1;
    int max, min;
    int ntc_count = 0;
    ntc_count = ntc_sw_temp_global_setting[NTC_NVKEY_SIZE_INDEX];

    if (ratio >= ntc_sw_ratio_global_setting[NTC_NVKEY_SMALL_START_INDEX])
        index = NTC_NVKEY_SMALL_START_INDEX;
    else if (ratio <= ntc_sw_ratio_global_setting[ntc_count])
        index = ntc_count;
    else {
        for (ptr = NTC_NVKEY_SMALL_START_INDEX + 1; ptr <= ntc_count; ptr++) {
            index = ptr;
            if (ratio == ntc_sw_ratio_global_setting[ptr]) {
                break;
            } else if (ratio > ntc_sw_ratio_global_setting[ptr]) {
                max = ntc_sw_ratio_global_setting[ptr - 1] - ratio;
                min = ratio - ntc_sw_ratio_global_setting[ptr];
                if (max == min)
                    index = ptr;
                else if (max < min)
                    index = ptr - 1;
                else
                    index = ptr;
                break;
            }
        }
    }
    *ret_temper = (int) (signed char) (ntc_sw_temp_global_setting[index]);
}

void battery_management_sw_ntc_caculate_slope(uint32_t ratio, uint32_t *data)
{
    *data = (ratio * 1000 ) / 4095;
}

void battery_management_sw_ntc_enable_adc(void) {
    hal_pinmux_set_function(HAL_GPIO_12, 8);
    hal_ntc_write32_data16(0xA2070400, 0x0800);
    hal_adc_init();
}

void battery_management_sw_ntc_get_adc_value(uint32_t *ret_data)
{
    uint32_t data = 0;
    uint32_t calibration = 0;
    uint8_t count = 0;
    uint32_t total = 0;
    uint8_t avg_count;

    if(ntc_slt_feature == true){
        avg_count = 5;
    }else{
        avg_count = ntc_para.avg;
    }

    while(count < avg_count){
        hal_adc_get_data_polling(HAL_ADC_CHANNEL_0,&data);
        total += data;
        count++;
    }
    total = (total / avg_count);
    hal_adc_get_calibraton_data(total, &calibration);
    *ret_data = calibration;

 }

void battery_management_sw_ntc_disable_adc(void) {
    hal_ntc_write32_data16(0xA2070400, 0x0000);
    hal_adc_deinit();
}

int battery_management_sw_ntc_convert_temp(signed char Temp)
{
    return (int)(signed char)Temp;
}

void battery_management_sw_ntc_table_dump()
{
#if BATTERY_MANAGEMENT_TABLE_DUMP
    uint8_t battery_management_ntc_key_num = 81;
    signed char ntc_temp[battery_management_ntc_key_num];
    uint16_t ntc_ntc[battery_management_ntc_key_num];
    uint32_t ntc_R2[battery_management_ntc_key_num];
    uint16_t ntc_sw_ratio[battery_management_ntc_key_num];

    //double R2_Temp = 0;
    //double internal_r2 = 0;
    uint32_t ntc_nvkey_length;

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_table_dump\r\n", 0);

    ntc_nvkey_length = sizeof(ntc_temp);
    memset(&ntc_temp, 0, sizeof(ntc_temp));
    nvkey_read_data(NVKEYID_NTC_NTC_PARA_TOOL_TEMP_TABLE, (uint8_t *)(&ntc_temp), &ntc_nvkey_length);

    ntc_nvkey_length = sizeof(ntc_ntc);
    memset(&ntc_ntc, 0, sizeof(ntc_ntc));
    nvkey_read_data(NVKEYID_NTC_NTC_PARA_TOOL_RESISTOR_TABLE, (uint8_t *)(&ntc_ntc), &ntc_nvkey_length);

    ntc_nvkey_length = sizeof(ntc_R2);
    memset(&ntc_R2, 0, sizeof(ntc_R2));
    nvkey_read_data(NVKEYID_NTC_NTC_PARA_TEMP_TABLE, (uint8_t *)(&ntc_R2), &ntc_nvkey_length);

    ntc_nvkey_length = sizeof(ntc_sw_ratio);
    memset(&ntc_sw_ratio, 0, sizeof(ntc_sw_ratio));
    nvkey_read_data(NVKEYID_NTC_NTC_PARA_RARIO_TABLE, (uint8_t *)(&ntc_sw_ratio), &ntc_nvkey_length);

    for(int ptr = 1; ptr < battery_management_ntc_key_num; ptr++)
    {
        hal_gpt_delay_ms(10);
        //R2_Temp = ntc_R2[ptr];
        //internal_r2 = (R2_Temp / 10000);

        log_hal_msgid_info("[battery][sw_ntc]{table}index[%d]temper[%d]ntc[%d]r2[%d]ratio[%d]\n",5,
            ptr, battery_management_sw_ntc_convert_temp(ntc_temp[ptr]), ntc_ntc[ptr], ntc_R2[ptr], ntc_sw_ratio[ptr]);
    }
#endif
}

battery_management_sw_ntc_status battery_management_sw_ntc_caculted_done(void)
{
    battery_management_sw_ntc_status ret = battery_sw_ntc_sucess;
    nvkey_status_t nvkey_status = NVKEY_STATUS_OK;
    uint32_t ntc_nvkey_length = 0;

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_caculted_done\r\n", 0);

    memset(&ntc_setting, 0, sizeof(hal_ntc_basic_config));
    ntc_nvkey_length = sizeof(hal_ntc_basic_config);
    nvkey_status = nvkey_read_data(NVKEYID_NTC_NTC_PARA_CONFIG1, (uint8_t *)(&ntc_setting), &ntc_nvkey_length);

    ntc_setting.caculated_done = 1;
    nvkey_status = nvkey_write_data(NVKEYID_NTC_NTC_PARA_CONFIG1,(uint8_t *)(&ntc_setting), ntc_nvkey_length);

    if(nvkey_status != NVKEY_STATUS_OK){
        log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_caculted_done nvkey error\r\n", 0);
    }

    return ret;
}

battery_management_sw_ntc_status battery_management_sw_ntc_get_ragpio(uint16_t *ret)
{
    uint16_t data = 0;
    uint8_t efuse[4] = {0};

    memcpy(efuse, (void *)EFUSE_RAGPIO_AB1565, sizeof(efuse));

    data = (efuse[3]<<8) + efuse[2];

    if(data == 0){
        *ret = EFUSE_INTERNAL_RAGPIO_DEFULT;
    }else{
        /* bit 19 - 31 */
        *ret = ((data & 0xFFF8) >> 3);
    }

    log_hal_msgid_info("[sw_ntc] battery_management_sw_ntc_get_ragpio [%d]",1, *ret);

    return battery_sw_ntc_sucess;
}

battery_management_sw_ntc_status battery_management_sw_ntc_ratio_caculate() {
    battery_management_sw_ntc_status ret = battery_sw_ntc_ratio_error;
    unsigned short ntc_tool_resistor_setting[NTC_NVKEY_SIZE];   //NTC
    uint32_t ntc_nvkey_length;
    uint16_t RAGPIO_data = 0;
    uint16_t r2;

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_ratio_caculate\r\n", 0);

    /*Step 1: Retrieve NVKEY */
    ntc_nvkey_length = sizeof(ntc_tool_resistor_setting);
    memset(&ntc_tool_resistor_setting, 0, ntc_nvkey_length);
    nvkey_read_data(NVKEYID_NTC_NTC_PARA_TOOL_RESISTOR_TABLE, (uint8_t*) (&ntc_tool_resistor_setting),
            &ntc_nvkey_length);

    /*Step 2: Retrieve RAGPIO value */
    battery_management_sw_ntc_get_ragpio(&RAGPIO_data);
    battery_management_sw_ntc_get_r2(&r2);

    /*Step 3: Caculate Ratio value */
    double Ratio1 = 0;
    double internal_r2 = r2;
    int Temp = 0;

    for (int index = NTC_NVKEY_SMALL_START_INDEX; index <= NTC_NVKEY_LARGE_END_INDEX; index++) {
        Ratio1 = ((ntc_tool_resistor_setting[index] + RAGPIO_data)
                / (ntc_tool_resistor_setting[index] + RAGPIO_data + internal_r2));
        Temp = (Ratio1 * 1000);
        ntc_sw_ratio_global_setting[index] = Temp;
    }

    nvkey_write_data(NVKEYID_NTC_NTC_PARA_RARIO_TABLE, (uint8_t*) (&ntc_sw_ratio_global_setting),sizeof(ntc_sw_ratio_global_setting));

    ret = battery_sw_ntc_sucess;

    return ret;
}

battery_management_sw_ntc_status battery_management_sw_ntc_get_r2(uint16_t *ret)
{
    uint16_t data = 0;
    uint8_t efuse[4] = {0};
    memcpy(efuse, (void *)EFUSE_INTERNAL_R2_AB1565, sizeof(efuse));
    data = (efuse[1]<<8) + efuse[0];
    if(data == 0){
        *ret = EFUSE_INTERNAL_R2_DEFULT;
    }else{
        /* bit 0 - 15 */
        *ret = data & 0xFFFF;
    }
    log_hal_msgid_info("[sw_ntc] battery_management_sw_ntc_get_r2 [%d]", 1, *ret);
    return battery_sw_ntc_sucess;
}

battery_management_sw_ntc_status battery_management_sw_ntc_find_25degree(uint8_t *index) {
    battery_management_sw_ntc_status status = battery_sw_ntc_sucess;
    if (index != NULL) {
        uint8_t ptr = 0;
        while (ptr < NTC_NVKEY_SIZE) {
            if (ntc_sw_temp_global_setting[ptr] == 25) {
                *index = ptr;
                break;
            } else {
                ptr++;
            }
        }
        log_hal_msgid_info("[sw_ntc] battery_management_sw_ntc_find_25degree index[%d]\r\n", 1, ptr);
    } else {
        status = battery_sw_ntc_find_25_error;
        log_hal_msgid_info("[sw_ntc] battery_management_sw_ntc_find_25degree ptr null\r\n", 0);
    }

    return status;
}

battery_management_sw_ntc_status battery_management_sw_ntc_r2_caculate(void )
{
    battery_management_sw_ntc_status status = battery_sw_ntc_sucess;
    nvkey_status_t nvkey_status = NVKEY_STATUS_OK;

    double Internal_R2;
    double Last_R2;
    unsigned int ntc_sw_temp_setting[NTC_NVKEY_SIZE];
    unsigned short R2_25C = 0;
    unsigned int covert = 0;
    uint8_t temper_ptr = NTC_NVKEY_MID_INDEX;

    uint32_t ntc_nvkey_length;

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_r2_caculate\r\n", 0);

    /*Step 0: Retrieve NVKEY */
    ntc_nvkey_length = sizeof(ntc_sw_temp_setting);
    memset(&ntc_sw_temp_setting, 0, sizeof(ntc_sw_temp_setting));
    nvkey_status &= nvkey_read_data(NVKEYID_NTC_NTC_PARA_TEMP_TABLE, (uint8_t *)(&ntc_sw_temp_setting), &ntc_nvkey_length);

    if(nvkey_status != NVKEY_STATUS_OK)
        log_hal_msgid_info("battery_management_sw_ntc_r2_caculate nvkey error", 0);

    /*Step 1: Find 25 degree Ptr */
    battery_management_sw_ntc_find_25degree(&temper_ptr);
    /*Step 2: Retrieve R2 value */
    battery_management_sw_ntc_get_r2(&R2_25C);
    //R2_25C = 11693;
    ntc_sw_temp_setting[temper_ptr] = (R2_25C * NTC_INR2_POPULATION);
    Last_R2 = R2_25C;
    /*Step 3: Caculate R2 */
    for(int lindex = (temper_ptr - 1); lindex >= NTC_NVKEY_SMALL_START_INDEX; lindex--)
    {
        Internal_R2 = ((double)Last_R2) * (0.999808);
        Last_R2 = Internal_R2;
        covert = (Internal_R2 * NTC_INR2_POPULATION);
        ntc_sw_temp_setting[lindex] = covert;
    }

    Last_R2 = R2_25C;

    for(int rindex = (temper_ptr + 1); rindex <= NTC_NVKEY_LARGE_END_INDEX; rindex++)
    {
        Internal_R2 = Last_R2 * (1.000192);
        Last_R2 = Internal_R2;
        covert = (Internal_R2 * NTC_INR2_POPULATION);
        ntc_sw_temp_setting[rindex] = covert;
    }

    ntc_nvkey_length = sizeof(ntc_sw_temp_setting);
    nvkey_write_data(NVKEYID_NTC_NTC_PARA_TEMP_TABLE, (uint8_t *)(&ntc_sw_temp_setting), ntc_nvkey_length);

    return status;
}

/*==========[SW NTC basic api]==========*/
battery_management_sw_ntc_status battery_management_sw_ntc_check_efuse(void) {
    battery_management_sw_ntc_status status = battery_sw_ntc_sucess;
    uint16_t data = 0;
    uint8_t efuse[4] = { 0 };
    memcpy(efuse, (void*) EFUSE_INTERNAL_R2_AB1565, 4);
    data = (efuse[1] << 8) + efuse[0];
    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_check_efuse r2 [%d]\r\n", 1, data);
    if (data == 0) {
        //status = battery_sw_ntc_no_efuseotp;
    }
    return status;
}

battery_management_sw_ntc_status battery_management_sw_ntc_get_interval(uint8_t *time) {
    battery_management_sw_ntc_status ret = battery_sw_ntc_sucess;
    if (time != NULL) {
        *time = ntc_para.t2;
        log_hal_msgid_info("[sw_ntc] battery_management_sw_ntc_get_interval *time[%d]",1, *time);
    } else {
        log_hal_msgid_info("[sw_ntc] battery_management_sw_ntc_get_interval para is null\r\n",0);
    }
    return ret;
}

battery_management_sw_ntc_status battery_management_sw_ntc_para_init()
{
    uint8_t j;
    battery_management_sw_ntc_status ret = battery_sw_ntc_sucess;
    nvkey_status_t nvkey_status = NVKEY_STATUS_ERROR;
    uint32_t ntc_nvkey_length = 0;

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_para_init\r\n", 0);

    ntc_nvkey_length = sizeof(ntc_setting);
    memset(&ntc_setting, 0, ntc_nvkey_length);
    nvkey_status = nvkey_read_data(NVKEYID_NTC_NTC_PARA_CONFIG1, (uint8_t *)(&ntc_setting), &ntc_nvkey_length);
    if(nvkey_status != NVKEY_STATUS_OK){
        return battery_sw_ntc_readnv_fail;
    }

    ntc_nvkey_length = sizeof(ntc_temp_setting);
    memset(&ntc_temp_setting, 0, ntc_nvkey_length);
    nvkey_status = nvkey_read_data(NVKEYID_NTC_NTC_PARA_CONFIG2, (uint8_t *)(&ntc_temp_setting), &ntc_nvkey_length);
    if(nvkey_status != NVKEY_STATUS_OK){
        return battery_sw_ntc_readnv_fail;
    }

    ntc_para.enable = ntc_setting.enable;
    ntc_para.table_done = ntc_setting.caculated_done;
    ntc_para.t1 = ntc_setting.t1;
    ntc_para.t2 = ntc_setting.t2;
    ntc_para.avg = ntc_setting.avr_count;
    ntc_para.char_ratio[3] = ntc_temp_setting.rishH;
    ntc_para.char_ratio[2] = ntc_temp_setting.abnH;
    ntc_para.char_ratio[1] = ntc_temp_setting.abnL;
    ntc_para.char_ratio[0] = ntc_temp_setting.rishL;
    ntc_para.unchar_ratio[3] = ntc_temp_setting.secH;
    ntc_para.unchar_ratio[2] = ntc_temp_setting.noH;
    ntc_para.unchar_ratio[1] = ntc_temp_setting.noL;
    ntc_para.unchar_ratio[0] = ntc_temp_setting.secL;

    for (j = 0; j < 4; j++) {
        log_hal_msgid_info("[battery][sw_ntc] Charger_TEMP[%d]\n", 1, ntc_para.char_ratio[j]);
    }
    for (j = 0; j < 4; j++) {
        log_hal_msgid_info("[battery][sw_ntc] WithoutCharger_TEMP[%d]\n", 1, ntc_para.unchar_ratio[j]);
    }
    log_hal_msgid_info("[battery][sw_ntc] ntc_para.enable[%d]\n", 1, ntc_para.enable);
    log_hal_msgid_info("[battery][sw_ntc] ntc_para.table_done[%d]\n", 1, ntc_para.table_done);
    log_hal_msgid_info("[battery][sw_ntc] ntc_para.t1[%d]\n", 1, ntc_para.t1);
    log_hal_msgid_info("[battery][sw_ntc] ntc_para.t2[%d]\n", 1, ntc_para.t2);
    log_hal_msgid_info("[battery][sw_ntc] ntc_para.avg[%d]\n", 1, ntc_para.avg);
    return ret;
}

battery_management_sw_ntc_status battery_management_sw_ntc_init(void) {
    uint32_t ntc_nvkey_length;
    battery_management_sw_ntc_status ret = battery_sw_ntc_sucess;

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_init\r\n", 0);
    ntc_nvkey_length = sizeof(ntc_sw_ratio_global_setting);
    memset(&ntc_sw_ratio_global_setting, 0, ntc_nvkey_length);
    nvkey_read_data(NVKEYID_NTC_NTC_PARA_RARIO_TABLE, (uint8_t *) (&ntc_sw_ratio_global_setting), &ntc_nvkey_length);

    ntc_nvkey_length = sizeof(ntc_sw_temp_global_setting);
    memset(&ntc_sw_temp_global_setting, 0, ntc_nvkey_length);
    nvkey_read_data(NVKEYID_NTC_NTC_PARA_TOOL_TEMP_TABLE, (uint8_t *) (&ntc_sw_temp_global_setting), &ntc_nvkey_length);

    ret = battery_management_sw_ntc_para_init();
    if (ret != battery_sw_ntc_sucess) {
        log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_init efuse error [%d]\r\n", 1, ret);
        return ret;
    }

    if (!ntc_para.enable) {
        ret = battery_sw_ntc_not_enable;
        log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_init not enable [%d]\r\n", 1, ret);
        return ret;
    }

    ret = battery_management_sw_ntc_check_efuse();
    if (ret != battery_sw_ntc_sucess) {
        log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_init error [%d]\r\n", 1, ret);
        return ret;
    }
    if (!ntc_setting.caculated_done) {
        battery_management_sw_ntc_r2_caculate();
        battery_management_sw_ntc_ratio_caculate();
        battery_management_sw_ntc_caculted_done();
        battery_management_sw_ntc_table_dump();
        ret = battery_sw_ntc_sucess;
        return ret;
    } else {
        return ret;
    }
}
/*==========[self test]==========*/
#if 0
void battery_management_sw_ntc_callback( TimerHandle_t pxExpiredTimer )
{
    int temp = 0;
    sw_ntc_jeita_status current = sw_ntc_charger_cold;
    sw_ntc_jeita_status next = sw_ntc_charger_cold;

    battery_management_sw_ntc_retrive(&temp, &current, &next);

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_callback temp[%d]state[%d]\n", 2, temp, (int)current);
}

void battery_management_sw_ntc_temp(void ){
    battery_management_sw_ntc_status status;
    uint8_t intervale_time;

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_temp \n", 0);

    /* Step 1: NTC default init */
    status = battery_management_sw_ntc_init();

    /* Step 2: Get time interval */
    battery_management_sw_ntc_get_interval(&intervale_time);

    /* Step 3*/
    if(status == battery_sw_ntc_sucess)
    {
        pNTCTimer = xTimerCreate( "NTC", pdMS_TO_TICKS(intervale_time * 1000), pdTRUE, NULL, battery_management_sw_ntc_callback);
        xTimerStart(pNTCTimer, 0);
    }else{
        log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_temp error [%d]\n", 1, status);
    }
}

void get_adc_value_from_GE_and_OE(double *adc_a ,double *adc_b)
{
    uint16_t GE_data = 0;
    uint16_t OE_data = 0;

    uint32_t effuse_data=0;

    effuse_data = *(volatile uint32_t *)(0xA20A0404);
    effuse_data = effuse_data&0x7FFFF;
    GE_data = effuse_data&0x3FF;
    OE_data = (effuse_data>>10)&0x1FF;

    *adc_a = (double)(GE_data -256)/(double)4096;
    *adc_b = (double)(OE_data -128)/(double)4096;

    log_hal_msgid_info("adc ge[%d]oe[%d] adca[%lf]adc_b[%lf]\r\n",3, GE_data,OE_data, *adc_a, *adc_b);

}

bool battery_management_sw_ntc_slt(void ){
    bool ret = false;

    ntc_slt_feature = true;

    double Rrouting = 0;
    double r2 = 0;
    double vadc = 0;
    double iNTC = 0;
    double vNTC = 0;
    double RNTC = 0;
    double vadc_core = 0;
    uint32_t vadc_temp;
    uint16_t temp;

    double a, b;

    log_hal_msgid_info("[battery][sw_ntc] battery_management_sw_ntc_slt \n", 0);

    //step 0: vadc core
    battery_management_sw_ntc_enable_adc();
    battery_management_sw_ntc_get_adc_value(&vadc_temp);
    battery_management_sw_ntc_disable_adc();
    vadc_core = vadc_temp;

    //Step 1: GE/OE
    get_adc_value_from_GE_and_OE(&a,&b);

    //Step 2: R2
    battery_management_sw_ntc_get_r2(&temp);
    r2 = temp;

    //Step 3: Rrouting(Ragpio)
    battery_management_sw_ntc_get_ragpio(&temp);
    Rrouting = temp;

    //Caculate RNTC final value.
    vadc = (vadc_core/4096 - b) * 1.4/(1+a);
    iNTC = (1.4-vadc)/r2;
    vNTC = vadc - iNTC*Rrouting;
    RNTC = (vNTC/iNTC);

    if((RNTC > 9700)&&(RNTC < 10300))
        ret = true;
    else
        ret = false;

    log_hal_msgid_info("[battery][sw_ntc] vadc_core[%lf]r2[%lf]Rrouting[%lf]\n",3, vadc_core, r2, Rrouting);
    log_hal_msgid_info("[battery][sw_ntc] vadc[%lf]iNTC[%lf]vNTC[%lf]RNTC[%lf]\n",4, vadc, iNTC, vNTC, RNTC);
    log_hal_msgid_info("[battery][sw_ntc] ret[%d]",1, ret);

    return ret;
}
#endif
