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

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "nvkey.h"
#include "hal.h"
#include "nvkey_id_list.h"
#include "syslog.h"
#include "hal_pmu_cali_2565.h"
#include "hal_pmu_internal_2565.h"
#include "hal_pmu_charger_2565.h"
#include "assert.h"
#include "hal_flash_disk_internal.h"
/*
#include "os_memory.h"
#include "os.h"
#include "drv_charger.h"
#include "hal_pmu.h"
#include "hal_pmu_platform.h"
*/

#define UNUSED(x)  ((void)(x))

extern void vPortFree(void*);
extern void *pvPortMalloc(size_t);


uint8_t gBuckLvDslp;
VBAT_SLOPE_CONFIG vbat_slope;
DAC_SLOPE_CONFIG dac_slope;
CURR_SLOPE_CONFIG chg_curr_4v35_slope;
CURR_SLOPE_CONFIG chg_curr_4v2_slope;
CHG_CC_CURR_CONFIG chg_cc_curr;
VICHG_ADC_CONFIG chg_vichg_adc;


////////////////////////////////////////////////////////////////////////////////
// Function Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
/****************transfer function****************/
uint16_t range_protect(uint16_t val, uint16_t min, uint16_t max, uint8_t byte)
{
    uint16_t thrd = 0, result = 0;

    if (byte == 1)
        thrd = 0x80;
    else if (byte == 2)
        thrd = 0x8000;
    else
        assert(0);

    if (min & thrd || min > max)
        assert(0);

    if (val & thrd)
    {
        result = min;
    }
    else
    {
        if (val > max)
            result = max;
        else
            result = val;
    }

    return result;
}

int32_t mpk_round(int32_t val1, int32_t val2)
{
    int32_t result = 0, tmp = 0;

    if (val2)
    {
        result = val1/val2;
        tmp = (10*val1/val2)%10;

        if (tmp > 4)
            result++;
        else if (tmp < -4)
            result--;
        //log_hal_msgid_info("mpk_round, val1[%d], val2[%d], tmp[%d], result[%d]", 4, val1, val2, tmp, result);
    }
    else
    {
        log_hal_msgid_error("mpk_round fail, val1[%d], val2[%d]", 2, val1, val2);
        assert(0);
    }

    return result;
}

uint16_t slope_calc(uint16_t volt1, uint16_t adc1, uint16_t volt2, uint16_t adc2, uint16_t volt_val)
{
    int16_t adc_val = 0;
    int16_t volt_diff = 0;
    int16_t adc_diff = 0;

    if (volt1 > volt2)
    {
        volt_diff = volt1 - volt2;
        adc_diff = adc1 - adc2;
    }
    else
    {
        volt_diff = volt2 - volt1;
        adc_diff = adc2 - adc1;
    }

    if (volt_val > volt2)
    {
        adc_val = adc2 + mpk_round((adc_diff * (volt_val - volt2)), volt_diff);
    }
    else if (volt_val < volt2)
    {
        adc_val = adc2 - mpk_round((adc_diff * (volt2 - volt_val)), volt_diff);
    }
    else
    {
        adc_val = adc2;
    }
    //log_hal_msgid_info("slope_calc V1[%d] A1[%d] V2[%d] A2[%d] VC[%d] AC[%d]", 6, volt1, adc1, volt2,adc2, volt_val, adc_val);

    return adc_val;
}

uint8_t vdig_bg_calculation(uint16_t volt1, uint8_t sel1, uint16_t volt2, uint8_t sel2, uint16_t volt_val)
{
    int8_t sel_val1 = 0;
    uint8_t sel_val2 = 0;

    if (sel1 > sel2)
    {
        sel_val1 = sel1 + mpk_round(((sel1-sel2-32)*(volt_val-volt1)), (volt1-volt2));
    }
    else
    {
        sel_val1 = sel1 + mpk_round(((sel1+32-sel2)*(volt_val-volt1)), (volt1-volt2));
    }

    if (sel_val1 < 0)
    {
        sel_val2 = sel_val1 + 32;
    }
    else if (sel_val1 > 31)
    {
        sel_val2 = sel_val1 - 32;
    }
    else
        sel_val2 = sel_val1;

    log_hal_msgid_info("vdig_bg_calculation, V1[%d] S1[%d] V2[%d] S2[%d] VC[%d] sel_val1[%d], sel_val2[%d]", 7, volt1, sel1, volt2, sel2, volt_val, sel_val1, sel_val2);

    return sel_val2;
}

void* get_nvkey_data(uint16_t nvkey_id)
{
    uint8_t* nvkey_ptr = NULL;
    uint32_t nvkey_size = 0;
    nvkey_status_t status = NVKEY_STATUS_ERROR;

    status = nvkey_data_item_length(nvkey_id, &nvkey_size);
    if (status || !nvkey_size)
    {
        log_hal_msgid_error("get_nvkey_data error, id[0x%x], status[%d], size[%d]", 3, nvkey_id, status, nvkey_size);
        return NULL;
    }
    nvkey_ptr = pvPortMalloc(nvkey_size);
    if (!nvkey_ptr)
    {
        log_hal_msgid_error("get_nvkey_data error, nvkey_ptr is NULL, id[0x%x]", 1, nvkey_id);
        return NULL;
    }
    status = nvkey_read_data(nvkey_id, nvkey_ptr, &nvkey_size);

    if(status)
    {
        log_hal_msgid_error("get_nvkey_data error, id[0x%x], status[%d]", 2, nvkey_id, status);
        vPortFree(nvkey_ptr);
        return NULL;
    }
    return (void*)nvkey_ptr;
}

pmu_status_t hal_pmu_get_nvkey(uint16_t id, uint8_t *ptr)
{
    uint32_t size = 0;
    nvkey_status_t status = nvkey_data_item_length(id, &size);
    if ((status != NVKEY_STATUS_OK) || (size == 0))
    {
        log_hal_msgid_error("hal_pmu_get_nvkey fail, id[0x%x], status[%d], size[%d]", 3, id, status, size);
        return PMU_STATUS_ERROR;
    }
    status = nvkey_read_data(id, ptr, &size);
    if(status != NVKEY_STATUS_OK)
    {
        log_hal_msgid_error("hal_pmu_get_nvkey fail, id[0x%x], status[%d]", 2, id, status);
        return PMU_STATUS_ERROR;
    }
    return PMU_STATUS_SUCCESS;
}

pmu_status_t pmu_cali_get_volt_by_sel(uint16_t *volt, uint8_t volt_sel)
{
    pmu_status_t status = PMU_STATUS_ERROR;
    uint16_t value = 0;

    if (volt_sel < bat_max)
    {
        if (volt_sel == bat_4p2v)
            value = 4200;
        else if (volt_sel == bat_4p35v)
            value = 4350;
        else if (volt_sel == bat_4p05v)
            value = 4050;
        else if (volt_sel == bat_4p10v)
            value = 4100;
        else if (volt_sel == bat_4p15v)
            value = 4150;
        else if (volt_sel == bat_4p25v)
            value = 4250;
        else if (volt_sel == bat_4p3v)
            value = 4300;
        else if (volt_sel == bat_4p4v)
            value = 4400;
        else if (volt_sel == bat_4p45v)
            value = 4450;
        else
            value = 4500;
        *volt = value;
        status = PMU_STATUS_SUCCESS;
        log_hal_msgid_info("pmu_cali_get_volt_by_sel, volt_sel[%d], volt[%d]", 2, volt_sel, *volt);
    }
    else
    {
        log_hal_msgid_error("pmu_cali_get_volt_by_sel fail, volt_sel[%d]", 1, volt_sel);
    }

    return status;
}
/****************SLT function****************/
void mpk_vcore_config(uint16_t volt)
{
    BUCK_LV_CONFIG buck_lv;
    uint32_t size = sizeof(BUCK_LV_CONFIG);
    nvkey_status_t nvkey_status = NVKEY_STATUS_ERROR;

    nvkey_status = nvkey_read_data(NVKEYID_MP_CAL_BUCK_LV_CONFIG, (uint8_t*)&buck_lv, &size);
    if (nvkey_status != NVKEY_STATUS_OK)
    {
        log_hal_msgid_error("mpk_vcore_config fail, nvkey_status[%d]", 1, nvkey_status);
        return;
    }

    if (buck_lv.kflag != FT_FW_K)
    {
        log_hal_msgid_error("mpk_vcore_config fail, kflag[%d]", 1, buck_lv.kflag);
        return;
    }
    uint8_t val = slope_calc(buck_lv.adc1, buck_lv.sloep_value1, buck_lv.adc2, buck_lv.sloep_value2, volt);
    val = range_protect(val, 0, 255, 1);
    pmu_set_register_value_2565(BUCK_VSEL_LV_ADDR, BUCK_VSEL_LV_MASK, BUCK_VSEL_LV_SHIFT, val);
    log_hal_msgid_info("mpk_vcore_config, SLT VCORE[0.%dV]", 1, volt);
}

/*******************mpk_init********************/
pmu_status_t mpk_chg_config(void)
{
    CHG_CONFIG chg_config;
    uint32_t size = sizeof(CHG_CONFIG);
    nvkey_status_t nvkey_status = NVKEY_STATUS_ERROR;

    nvkey_status = nvkey_read_data(NVKEYID_MP_CAL_CHG_CONFIG, (uint8_t*)&chg_config, &size);
    if (nvkey_status != NVKEY_STATUS_OK)
    {
        log_hal_msgid_error("mpk_chg_config fail, nvkey_status[%d]", 1, nvkey_status);
        return PMU_STATUS_ERROR;
    }
    if (chg_config.kflag != FT_FW_K)
    {
        log_hal_msgid_error("mpk_chg_config fail, kflag[%d]", 1, chg_config.kflag);
        return PMU_STATUS_ERROR;
    }

#ifdef AIR_PMU_DISABLE_CHARGER
    log_hal_msgid_info("mpk_chg_config, bypass", 0);
    return PMU_STATUS_SUCCESS;
#else
    if( chg_config.two_step_sel)
    {
        pmu_set_register_value_2565(CC2_EN_ADDR, CC2_EN_MASK, CC2_EN_SHIFT, 1);
    }
    vbat_slope.two_step_sel = chg_config.two_step_sel;
    vbat_slope.voltage_sel = chg_config.bat_volt_sel;

    return PMU_STATUS_SUCCESS;
#endif
}
#ifndef AIR_PMU_DISABLE_CHARGER
void mpk_chg_adc_config(void)
{
    uint16_t ADCval;

    CHG_ADC_CONFIG* pCHG_ADC = (CHG_ADC_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_CHG_ADC_CONFIG);

    if(!pCHG_ADC)
        return;

    pmu_chg_info.cc1_thrd_volt = pCHG_ADC->cc1_threshold_voltage + pCHG_ADC->cc1_thd_voltage_offset;
    pmu_chg_info.cc2_thrd_volt = pCHG_ADC->cc2_threshold_voltage + pCHG_ADC->cc2_offset_voltage;
    pmu_chg_info.full_bat_volt = pCHG_ADC->full_bat_voltage + pCHG_ADC->full_bat_voltage_offset;
    pmu_chg_info.rechg_volt = pCHG_ADC->recharge_voltage + pCHG_ADC->recharge_voltage_offset;

    ADCval = pCHG_ADC->cc1_threshold_ADC + pCHG_ADC->cc1_thd_ADC_offset;
    pmu_set_register_value_2565(CC1_THRESHOLD_ADDR, CC1_THRESHOLD_MASK, CC1_THRESHOLD_SHIFT, ADCval);

    ADCval = pCHG_ADC->cc2_threshold_ADC + pCHG_ADC->cc2_offset_ADC;
    pmu_set_register_value_2565(CC2_THRESHOLD_ADDR, CC2_THRESHOLD_MASK, CC2_THRESHOLD_SHIFT, ADCval);

    ADCval = pCHG_ADC->cv_threshold_ADC+ pCHG_ADC->cv_thd_ADC_offset;
    pmu_set_register_value_2565(CV_THRESHOLD_ADDR, CV_THRESHOLD_MASK, CV_THRESHOLD_SHIFT, ADCval);

    ADCval = pCHG_ADC->full_bat_ADC + pCHG_ADC->full_bat_ADC_offset;
    pmu_set_register_value_2565(FULL_BAT_THRESHOLD1_ADDR, FULL_BAT_THRESHOLD1_MASK, FULL_BAT_THRESHOLD1_SHIFT, ADCval);

    ADCval = pCHG_ADC->recharge_ADC+ pCHG_ADC->recharge_ADC_offset;
    pmu_set_register_value_2565(RECHARGE_THRESHOLD_ADDR, RECHARGE_THRESHOLD_MASK, RECHARGE_THRESHOLD_SHIFT, ADCval);

    ADCval = pCHG_ADC->full_bat_ADC_2 + pCHG_ADC->full_bat_ADC_2_offset;
    pmu_set_register_value_2565(FULL_BAT_THRESHOLD2_ADDR, FULL_BAT_THRESHOLD2_MASK, FULL_BAT_THRESHOLD2_SHIFT, 0x3FF);
    pmu_set_register_value_2565(CHG_COMPLETE_CHK_NUM2_ADDR, CHG_COMPLETE_CHK_NUM2_MASK, CHG_COMPLETE_CHK_NUM2_SHIFT, 0xF);

    vPortFree(pCHG_ADC);
}

void mpk_chg_dac_config(void)
{
    uint16_t ADCval;

    INT_CHG_DAC_CONFIG* pINT_CHG_ADC = (INT_CHG_DAC_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_INT_CHG_DAC_CONFIG);

    if(!pINT_CHG_ADC)
        return;

    ADCval = pINT_CHG_ADC->tricklecurrentDAC + pINT_CHG_ADC->tricklecurrentDACoffset;
    pmu_set_register_value_2565(TRICKLE_DAC_VALUE_ADDR, TRICKLE_DAC_VALUE_MASK, TRICKLE_DAC_VALUE_SHIFT, ADCval);
    pmu_set_register_value_2565(TRICKLE_DAC_OUT_UPDATE_ADDR, TRICKLE_DAC_OUT_UPDATE_MASK, TRICKLE_DAC_OUT_UPDATE_SHIFT, 1);

    ADCval = pINT_CHG_ADC->cc1currentDAC + pINT_CHG_ADC->cc1currentDACoffset;
    pmu_set_register_value_2565(CC1_DAC_VALUE_ADDR, CC1_DAC_VALUE_MASK, CC1_DAC_VALUE_SHIFT, ADCval);
    pmu_set_register_value_2565(CC1_DAC_OUT_UPDATE_ADDR, CC1_DAC_OUT_UPDATE_MASK, CC1_DAC_OUT_UPDATE_SHIFT, 1);

    ADCval = pINT_CHG_ADC->cc2currentDAC + pINT_CHG_ADC->cc2currentDACoffset;
    pmu_set_register_value_2565(CC2_DAC_VALUE_ADDR, CC2_DAC_VALUE_MASK, CC2_DAC_VALUE_SHIFT, ADCval);
    pmu_set_register_value_2565(CC2_DAC_OUT_UPDATE_ADDR, CC2_DAC_OUT_UPDATE_MASK, CC2_DAC_OUT_UPDATE_SHIFT, 1);

    ADCval = pINT_CHG_ADC->cvDAC + pINT_CHG_ADC->cvDACoffset;
    pmu_set_register_value_2565(CV_DAC_VALUE_ADDR, CV_DAC_VALUE_MASK, CV_DAC_VALUE_SHIFT, ADCval);
    pmu_set_register_value_2565(CV_DAC_OUT_UPDATE_ADDR, CV_DAC_OUT_UPDATE_MASK, CV_DAC_OUT_UPDATE_SHIFT, 1);

    vPortFree(pINT_CHG_ADC);
}

void mpk_CHG_trickle_current_config(void)
{
    uint16_t tmpval;

    INT_CHG_TRICKLE_CURRENT_CONFIG* pINTCHG_TRICURRCONFIG = (INT_CHG_TRICKLE_CURRENT_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_INT_CHG_TRICKLE_CURRENT_CONFIG);

    if(!pINTCHG_TRICURRCONFIG)
        return;

    if(pINTCHG_TRICURRCONFIG->cal_cnt < pINTCHG_TRICURRCONFIG->select)
    {
        vPortFree(pINTCHG_TRICURRCONFIG);
        return;
    }

    tmpval = pINTCHG_TRICURRCONFIG->data[pINTCHG_TRICURRCONFIG->select - 1].rchg_sel;
    pmu_set_register_value_2565(TRICKLE_RCHG_SEL_ADDR, TRICKLE_RCHG_SEL_MASK, TRICKLE_RCHG_SEL_SHIFT, tmpval);
    pmu_set_register_value_2565(TRICKLE_RCHG_SEL_UPDATE_ADDR, TRICKLE_RCHG_SEL_UPDATE_MASK, TRICKLE_RCHG_SEL_UPDATE_SHIFT, 1);

    vPortFree(pINTCHG_TRICURRCONFIG);
}

void mpk_CHG_CC1_current_config(void)
{
    uint16_t tmpval;

    INT_CHG_CC1_CURRENT_CONFIG* pINTCHG_CC1CURRCONFIG = (INT_CHG_CC1_CURRENT_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_INT_CHG_CC1_CURRENT_CONFIG);

    if(!pINTCHG_CC1CURRCONFIG)
        return;

    if(pINTCHG_CC1CURRCONFIG->cal_cnt < pINTCHG_CC1CURRCONFIG->select)
    {
        vPortFree(pINTCHG_CC1CURRCONFIG);
        return;
    }
    pmu_chg_info.cc1_curr = pINTCHG_CC1CURRCONFIG->data[pINTCHG_CC1CURRCONFIG->select - 1].current;

    tmpval = pINTCHG_CC1CURRCONFIG->data[pINTCHG_CC1CURRCONFIG->select - 1].rchg_sel;
    pmu_chg_cc_config.norm_cc1_rsel = tmpval;
    pmu_chg_cc_config.vbat_cc1_rsel = 1;
    pmu_charger_cc_config(PMU_CHG_CC1, PMU_CHG_CC_JEITA_NORM);
    if (vbat_slope.two_step_sel == 0)
    {
        pmu_set_register_value_2565(CV_RCHG_SEL_ADDR, CV_RCHG_SEL_MASK, CV_RCHG_SEL_SHIFT, tmpval);
        pmu_set_register_value_2565(CV_RCHG_SEL_UPDATE_ADDR, CV_RCHG_SEL_UPDATE_MASK, CV_RCHG_SEL_UPDATE_SHIFT, 1);
    }

    vPortFree(pINTCHG_CC1CURRCONFIG);
}

void mpk_CHG_CC2_current_config(void)
{
    uint16_t tmpval;

    INT_CHG_CC2_CURRENT_CONFIG* pINTCHG_CC2CURRCONFIG = (INT_CHG_CC2_CURRENT_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_INT_CHG_CC2_CURRENT_CONFIG);

    if(!pINTCHG_CC2CURRCONFIG)
        return;

    if(pINTCHG_CC2CURRCONFIG->cal_cnt< pINTCHG_CC2CURRCONFIG->select)
    {
        vPortFree(pINTCHG_CC2CURRCONFIG);
        return;
    }
    pmu_chg_info.cc2_curr = pINTCHG_CC2CURRCONFIG->data[pINTCHG_CC2CURRCONFIG->select - 1].current;

    tmpval = pINTCHG_CC2CURRCONFIG->data[pINTCHG_CC2CURRCONFIG->select - 1].rchg_sel;
    pmu_chg_cc_config.norm_cc2_rsel = tmpval;
    pmu_chg_cc_config.vbat_cc2_rsel = 1;
    pmu_charger_cc_config(PMU_CHG_CC2, PMU_CHG_CC_JEITA_NORM);
    if (vbat_slope.two_step_sel == 1)
    {
        pmu_set_register_value_2565(CV_RCHG_SEL_ADDR, CV_RCHG_SEL_MASK, CV_RCHG_SEL_SHIFT, tmpval);
        pmu_set_register_value_2565(CV_RCHG_SEL_UPDATE_ADDR, CV_RCHG_SEL_UPDATE_MASK, CV_RCHG_SEL_UPDATE_SHIFT, 1);
    }

    vPortFree(pINTCHG_CC2CURRCONFIG);
}

void mpk_chg_cv_stop_current_adc(void)
{
    uint16_t tmpval;

    CHG_CV_STOP_CURRENT_ADC* pCHG_CVSTOPCURRCONFIG = (CHG_CV_STOP_CURRENT_ADC*)get_nvkey_data(NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC);

    if(!pCHG_CVSTOPCURRCONFIG)
        return;

    if(pCHG_CVSTOPCURRCONFIG->CV_stop_current_percent > 100)
    {
        vPortFree(pCHG_CVSTOPCURRCONFIG);
        return;
    }

    tmpval = pCHG_CVSTOPCURRCONFIG->CV_stop_current_ADC;
    pmu_set_register_value_2565(CV_STOP_CURRENT_ADDR, CV_STOP_CURRENT_MASK, CV_STOP_CURRENT_SHIFT, tmpval);

    vPortFree(pCHG_CVSTOPCURRCONFIG);
}
#endif
void mpk_chg_sys_ldo(void)
{
    uint16_t tmpval;

    CHG_SYS_LDO* pCHG_SYS_LDO = (CHG_SYS_LDO*)get_nvkey_data(NVKEYID_MP_CAL_SYS_LDO);

    if(!pCHG_SYS_LDO)
        return;

    if(pCHG_SYS_LDO->SYSLDO_output_voltage > 5000)
    {
        vPortFree(pCHG_SYS_LDO);
        return;
    }

    tmpval = pCHG_SYS_LDO->CHG_LDO_SEL;
    if (vbat_slope.voltage_sel >= bat_4p3v)//if CV >=4.3V, VSYSLDO output +1
    {
        tmpval++;
        tmpval = range_protect(tmpval, 0, 7, 1);
        log_hal_msgid_info("mpk_chg_sys_ldo, tmpval[%d]", 1, tmpval);
    }

    pmu_set_register_value_2565(CHG_LDO_SEL_ADDR, CHG_LDO_SEL_MASK, CHG_LDO_SEL_SHIFT, tmpval);

    vPortFree(pCHG_SYS_LDO);
}

void mpk_chg_ocp(void)
{
    CHG_OCP* pCHG_OCP = (CHG_OCP*)get_nvkey_data(NVKEYID_MP_CAL_OCP);

    if(!pCHG_OCP)
        return;

    pmu_set_register_value_2565(SW_OC_LMT_ADDR, SW_OC_LMT_MASK, SW_OC_LMT_SHIFT, pCHG_OCP->SW_OC_LMT);
    pmu_set_register_value_2565(I_LIM_TRIM_ADDR, I_LIM_TRIM_MASK, I_LIM_TRIM_SHIFT, pCHG_OCP->I_LIM_TRIM);

    vPortFree(pCHG_OCP);
}

/*******************BUCK********************/
void mpk_buck_mv(void)
{
    uint16_t tmp_val;

    BUCK_MV_CONFIG* pBUCK_MV = (BUCK_MV_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_MV_CONFIG);

    //log_hal_msgid_info("mpk_buck_mv ptr[%d], cal_cnt[%d], volselect[%d], kflag[%d]", 4, pBUCK_MV, pBUCK_MV->cal_cnt, pBUCK_MV->volselect, pBUCK_MV->kflag);

    if(!pBUCK_MV)
        return;

    if (pBUCK_MV->cal_cnt < pBUCK_MV->volselect)
    {
        vPortFree(pBUCK_MV);
        return;
    }
    if (pBUCK_MV->kflag == FT_FW_K)
    {
        tmp_val = pBUCK_MV->data[pBUCK_MV->volselect - 1].value;
        pmu_set_register_value_2565(BUCK_VSEL_MV_ADDR, BUCK_VSEL_MV_MASK, BUCK_VSEL_MV_SHIFT, tmp_val);
        //log_hal_msgid_info("mpk_buck_mv, tmp_val[%d]", 1, tmp_val);
    }
    vPortFree(pBUCK_MV);
}

void mpk_buck_mv_stb(void)
{
    uint16_t tmp_val;

    BUCK_MV_STB_CONFIG* pBUCK_MV_STB = (BUCK_MV_STB_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_MV_STB_CONFIG);

    if(!pBUCK_MV_STB)
        return;

    if (pBUCK_MV_STB->cal_cnt < pBUCK_MV_STB->volselect)
    {
        vPortFree(pBUCK_MV_STB);
        return;
    }
    if (pBUCK_MV_STB->kflag == FT_FW_K)
    {
        tmp_val = pBUCK_MV_STB->data[pBUCK_MV_STB->volselect - 1].value;
        pmu_set_register_value_2565(MV_STB_SEL_ADDR, MV_STB_SEL_MASK, MV_STB_SEL_SHIFT, tmp_val);
    }
    vPortFree(pBUCK_MV_STB);
}

void mpk_buck_mv_freq(void)
{
    BUCK_MV_FREQ* pBUCK_MV_FREQ = (BUCK_MV_FREQ*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_MV_FREQ);

    if(!pBUCK_MV_FREQ)
        return;

    if (pBUCK_MV_FREQ->kflag == FT_FW_K)
    {
        pmu_set_register_value_2565(BUCK_FREQ_MV_ADDR, BUCK_FREQ_MV_MASK, BUCK_FREQ_MV_SHIFT, pBUCK_MV_FREQ->BUCK_FREQ_MV);
        pmu_set_register_value_2565(OSC_FREQK_MV_ADDR, OSC_FREQK_MV_MASK, OSC_FREQK_MV_SHIFT, pBUCK_MV_FREQ->OSC_FREQK_MV);
    }
    vPortFree(pBUCK_MV_FREQ);
}

void mpk_buck_lv(void)
{
    uint16_t tmp_val;

    BUCK_LV_CONFIG* pBUCK_LVCONFIG = (BUCK_LV_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_LV_CONFIG);

    if(!pBUCK_LVCONFIG)
        return;

    if(pBUCK_LVCONFIG->cal_cnt < pBUCK_LVCONFIG->volselect)
    {
        vPortFree(pBUCK_LVCONFIG);
        return;
    }

#ifdef AIR_PMU_DISABLE_CHARGER
    pmu_set_register_value_2565(BUCK_LVOUTSEL_LV_NORM_ADDR, BUCK_LVOUTSEL_LV_NORM_MASK, BUCK_LVOUTSEL_LV_NORM_SHIFT, 0x0);
    log_hal_msgid_info("mpk_buck_lv, vcore 0.9V", 0);
#endif

    if (pBUCK_LVCONFIG->kflag == FT_FW_K)
    {
        tmp_val = pBUCK_LVCONFIG->data[pBUCK_LVCONFIG->volselect-1].value;
        pmu_set_register_value_2565(BUCK_VSEL_LV_ADDR, BUCK_VSEL_LV_MASK, BUCK_VSEL_LV_SHIFT, tmp_val);
        /*log_hal_msgid_info("mpk_buck_lv cnt[%d], sel[%d], flag[%d], tmp_val[0x%x]", 4, pBUCK_LVCONFIG->cal_cnt, pBUCK_LVCONFIG->volselect
            , pBUCK_LVCONFIG->kflag, tmp_val);*/
    }
    /*uint16_t tmp = pmu_get_register_value_2565(BUCK_VSEL_LV_ADDR, BUCK_VSEL_LV_MASK, BUCK_VSEL_LV_SHIFT);
    log_hal_msgid_info("mpk_buck_lv tmp_val 2[0x%x]", 1, pmu_get_register_value_2565(BUCK_VSEL_LV_ADDR, BUCK_VSEL_LV_MASK, BUCK_VSEL_LV_SHIFT));*/

    vPortFree(pBUCK_LVCONFIG);
}

void mpk_buck_lv_lpm(void)
{
    uint16_t tmp_val;

    BUCK_LV_LPM* pBUCK_LV_LPM = (BUCK_LV_LPM*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_LV_LPM);

    if(!pBUCK_LV_LPM)
        return;

    if(pBUCK_LV_LPM->cal_cnt < pBUCK_LV_LPM->volselect)
    {
        vPortFree(pBUCK_LV_LPM);
        return;
    }

    if (pBUCK_LV_LPM->kflag == FT_FW_K)
    {
        tmp_val = pBUCK_LV_LPM->data[pBUCK_LV_LPM->volselect-1].value;
        pmu_set_register_value_2565(BUCK_VSEL_LV_ADDR, BUCK_VSEL_LV_MASK, BUCK_VSEL_LV_SHIFT, tmp_val);
    }

    vPortFree(pBUCK_LV_LPM);
}

void mpk_buck_lv_freq(void)
{
    BUCK_LV_FREQ* pBUCK_LV_FREQ = (BUCK_LV_FREQ*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_LV_FREQ);

    if(!pBUCK_LV_FREQ)
        return;

    if (pBUCK_LV_FREQ->kflag == FT_FW_K)
    {
        pmu_set_register_value_2565(BUCK_FREQ_LV_ADDR, BUCK_FREQ_LV_MASK, BUCK_FREQ_LV_SHIFT, pBUCK_LV_FREQ->BUCK_FREQ_LV);
        pmu_set_register_value_2565(OSC_FREQK_LV_ADDR, OSC_FREQK_LV_MASK, OSC_FREQK_LV_SHIFT, pBUCK_LV_FREQ->OSC_FREQK_LV);
    }
    vPortFree(pBUCK_LV_FREQ);
}

/*******************LDO********************/
void mpk_ldo_vdd33_reg(void)
{
    uint16_t tmp_val;

    LDO_VDD33_REG_CONFIG* pLDO_VDD33_REG = (LDO_VDD33_REG_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VDD33_REG_CONFIG);

    if(!pLDO_VDD33_REG)
        return;

    if(pLDO_VDD33_REG->cal_cnt < pLDO_VDD33_REG->volselect)
    {
        vPortFree(pLDO_VDD33_REG);
        return;
    }
    if (pLDO_VDD33_REG->kflag == FT_FW_K)
    {
        tmp_val = pLDO_VDD33_REG->data[pLDO_VDD33_REG->volselect - 1].value;
        pmu_set_register_value_2565(REGHV_SEL_NM_ADDR, REGHV_SEL_NM_MASK, REGHV_SEL_NM_SHIFT, tmp_val);
    }
    vPortFree(pLDO_VDD33_REG);
}

void mpk_ldo_vdd33_reg_ret(void)
{
    uint16_t tmp_val;

    LDO_VDD33_REG_RET_CONFIG* pLDO_VDD33_REG_RET = (LDO_VDD33_REG_RET_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VDD33_REG_RET_CONFIG);

    if(!pLDO_VDD33_REG_RET)
        return;

    if(pLDO_VDD33_REG_RET->cal_cnt < pLDO_VDD33_REG_RET->volselect)
    {
        vPortFree(pLDO_VDD33_REG_RET);
        return;
    }
    if (pLDO_VDD33_REG_RET->kflag == FT_FW_K)
    {
        tmp_val = pLDO_VDD33_REG_RET->data[pLDO_VDD33_REG_RET->volselect - 1].value;
        pmu_set_register_value_2565(REGHV_SEL_RET_ADDR, REGHV_SEL_RET_MASK, REGHV_SEL_RET_SHIFT, tmp_val);
    }
    vPortFree(pLDO_VDD33_REG_RET);
}

void mpk_ldo_vdd33_ret(void)
{
    uint16_t tmp_val;

    LDO_VDD33_RET_CONFIG* pLDO_VDD33_RET = (LDO_VDD33_RET_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VDD33_RET_CONFIG);

    if(!pLDO_VDD33_RET)
        return;

    if(pLDO_VDD33_RET->cal_cnt < pLDO_VDD33_RET->volselect)
    {
        vPortFree(pLDO_VDD33_RET);
        return;
    }
    if (pLDO_VDD33_RET->kflag == FT_FW_K)
    {
        tmp_val = pLDO_VDD33_RET->data[pLDO_VDD33_RET->volselect - 1].value;
        pmu_set_register_value_2565(HVSTBSEL_ADDR, HVSTBSEL_MASK, HVSTBSEL_SHIFT, tmp_val);
    }
    vPortFree(pLDO_VDD33_RET);
}

void mpk_ldo_vrf_reg(void)
{
    uint16_t tmp_val;

    LDO_VRF_REG_CONFIG* pLDO_VRF_REG = (LDO_VRF_REG_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VRF_REG_CONFIG);

    if(!pLDO_VRF_REG)
        return;

    if(pLDO_VRF_REG->cal_cnt < pLDO_VRF_REG->volselect)
    {
        vPortFree(pLDO_VRF_REG);
        return;
    }
    if (pLDO_VRF_REG->kflag == FT_FW_K)
    {
        tmp_val = pLDO_VRF_REG->data[pLDO_VRF_REG->volselect - 1].value;
        pmu_set_register_value_2565(REGLV2_SEL_NM_ADDR, REGLV2_SEL_NM_MASK, REGLV2_SEL_NM_SHIFT, tmp_val);
    }
    vPortFree(pLDO_VRF_REG);
}

void mpk_ldo_vrf_reg_ret(void)
{
    uint16_t tmp_val;

    LDO_VRF_REG_RET_CONFIG* pLDO_VRF_REG_RET = (LDO_VRF_REG_RET_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VRF_REG_RET_CONFIG);

    if(!pLDO_VRF_REG_RET)
        return;

    if(pLDO_VRF_REG_RET->cal_cnt < pLDO_VRF_REG_RET->volselect)
    {
        vPortFree(pLDO_VRF_REG_RET);
        return;
    }
    if (pLDO_VRF_REG_RET->kflag == FT_FW_K)
    {
        tmp_val = pLDO_VRF_REG_RET->data[pLDO_VRF_REG_RET->volselect - 1].value;
        pmu_set_register_value_2565(REGLV2_SEL_RET_ADDR, REGLV2_SEL_RET_MASK, REGLV2_SEL_RET_SHIFT, tmp_val);
    }
    vPortFree(pLDO_VRF_REG_RET);
}

void mpk_ldo_vrf_ret(void)
{
    uint16_t tmp_val;

    LDO_VRF_RET_CONFIG* pLDO_VRF_RET = (LDO_VRF_RET_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VRF_RET_CONFIG);

    if(!pLDO_VRF_RET)
        return;

    if(pLDO_VRF_RET->cal_cnt < pLDO_VRF_RET->volselect)
    {
        vPortFree(pLDO_VRF_RET);
        return;
    }
    if (pLDO_VRF_RET->kflag == FT_FW_K)
    {
        tmp_val = pLDO_VRF_RET->data[pLDO_VRF_RET->volselect - 1].value;
        pmu_set_register_value_2565(LV3_STB_REGSEL_ADDR, LV3_STB_REGSEL_MASK, LV3_STB_REGSEL_SHIFT, tmp_val);
    }
    vPortFree(pLDO_VRF_RET);
}

void mpk_ldo_vdig18(void)
{
    uint16_t tmp_val;

    LDO_VDIG18_CONFIG* pLDO_VDIG18 = (LDO_VDIG18_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_VDIG18_CONFIG);

    if(!pLDO_VDIG18)
        return;

    if(pLDO_VDIG18->cal_cnt < pLDO_VDIG18->volselect)
    {
        vPortFree(pLDO_VDIG18);
        return;
    }
    if (pLDO_VDIG18->kflag == FT_FW_K)
    {
        tmp_val = pLDO_VDIG18->data[pLDO_VDIG18->volselect - 1].value;
        pmu_set_register_value_2565(VDIG18_SEL_ADDR, VDIG18_SEL_MASK, VDIG18_SEL_SHIFT, tmp_val);
    }
    vPortFree(pLDO_VDIG18);
}

/*******************VRF********************/
void mpk_hpbg(void)
{
    uint16_t tmp_val;

    HPBG_CONFIG* pVREF_HPBG = (HPBG_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_HPBG_CONFIG);

    if(!pVREF_HPBG)
        return;

    if(pVREF_HPBG->cal_cnt < pVREF_HPBG->volselect)
    {
        vPortFree(pVREF_HPBG);
        return;
    }
    if (pVREF_HPBG->kflag == FT_FW_K)
    {
        tmp_val = pVREF_HPBG->data[pVREF_HPBG->volselect - 1].value;
        pmu_set_register_value_2565(BGR_TRIM_ADDR, BGR_TRIM_MASK, BGR_TRIM_SHIFT, tmp_val);
    }
    vPortFree(pVREF_HPBG);
}

void mpk_lpbg(void)
{
    uint16_t tmp_val;

    LPBG_CONFIG* pVREF_LPBG = (LPBG_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LPBG_CONFIG);

    if(!pVREF_LPBG)
        return;

    if(pVREF_LPBG->cal_cnt < pVREF_LPBG->volselect)
    {
        vPortFree(pVREF_LPBG);
        return;
    }
    if (pVREF_LPBG->kflag == FT_FW_K)
    {
        tmp_val = pVREF_LPBG->data[pVREF_LPBG->volselect - 1].value;
        pmu_set_register_value_2565(BGR_TRIM_ADDR, BGR_TRIM_MASK, BGR_TRIM_SHIFT, tmp_val);
    }
    vPortFree(pVREF_LPBG);
}

void mpk_init(void)
{
    if (mpk_chg_config() != PMU_STATUS_SUCCESS)
        return;

#ifndef AIR_PMU_DISABLE_CHARGER
    mpk_chg_adc_config();
    mpk_chg_dac_config();
    mpk_CHG_trickle_current_config();
    mpk_CHG_CC1_current_config();
    mpk_CHG_CC2_current_config();
    mpk_chg_cv_stop_current_adc();
#else
    log_hal_msgid_info("mpk_init, bypass chg", 0);
#endif
    mpk_chg_sys_ldo();
    mpk_chg_ocp();

    mpk_buck_mv();
    mpk_buck_mv_stb();
    mpk_buck_mv_freq();
    mpk_buck_lv();
    mpk_buck_lv_lpm();
    mpk_buck_lv_freq();

    mpk_ldo_vdd33_reg();
    mpk_ldo_vdd33_reg_ret();
    mpk_ldo_vdd33_ret();
    mpk_ldo_vrf_reg();
    mpk_ldo_vrf_reg_ret();
    mpk_ldo_vrf_ret();
    mpk_ldo_vdig18();

    mpk_hpbg();
    //mpk_lpbg();

    log_hal_msgid_info("mpk_init, done", 0);
}

/*******************otp_calibration********************/
#ifndef AIR_PMU_DISABLE_CHARGER
void otp_cal_vbat_adc(void)
{
    uint16_t volt1 = 0;
    uint16_t volt2 = 0;
    uint16_t adc1 = 0;
    uint16_t adc2 = 0;

    VBAT_ADC_CALIBRATION_TABLE vbat_adc;
    pmu_status_t status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_VBAT_ADC_CALIBRATION_TABLE, (uint8_t*)&vbat_adc);
    if (status != PMU_STATUS_SUCCESS)
    {
        log_hal_msgid_error("otp_cal_vbat_adc fail, vbat_adc status[%d]", 1, status);
        return;
    }

    OTP_VBAT_CONFIG otp_vbat;
    hal_flash_status_t flash_status = hal_flash_otp_read(OTP_VBAT_ADDR, (uint8_t *)&otp_vbat, sizeof(OTP_VBAT_CONFIG));
    if (flash_status != HAL_FLASH_STATUS_OK)
    {
        log_hal_msgid_error("otp_cal_vbat_adc fail, flash_status[%d]", 1, flash_status);
        return;
    }
    if(otp_vbat.ft_kflag != MP_K)
    {
        log_hal_msgid_error("otp_cal_vbat_adc fail, ft_kflag[%d]", 1, otp_vbat.ft_kflag);
        return;
    }

    status = pmu_cali_get_volt_by_sel(&volt1, vbat_slope.voltage_sel);
    if (status != PMU_STATUS_SUCCESS)
    {
        log_hal_msgid_error("otp_cal_vbat_adc fail, status[%d]", 1, status);
        return;
    }
    adc1 = slope_calc(otp_vbat.data[0].volt, otp_vbat.data[0].adc,
        otp_vbat.data[1].volt, otp_vbat.data[1].adc, volt1);

    volt2 = otp_vbat.data[2].volt; //3.0v
    adc2 = otp_vbat.data[2].adc;
    log_hal_msgid_info("otp_cal_vbat_adc, volt[%d], adc[%d]", 2, volt1, adc1);

    vbat_slope.volt1 = volt1;
    vbat_slope.adc1  = adc1;
    vbat_slope.volt2 = volt2;
    vbat_slope.adc2  = adc2;

    for(uint8_t i = 0; i < vbat_adc.cal_cnt; i++)
    {
        vbat_adc.data[i].adc = slope_calc(volt1, adc1, volt2, adc2, vbat_adc.data[i].volt);
        vbat_adc.data[i].adc = range_protect(vbat_adc.data[i].adc, 0, 1023, 2);
    }
    vbat_adc.kflag = FT_FW_K;
    nvkey_write_data(NVKEYID_MP_CAL_VBAT_ADC_CALIBRATION_TABLE, (uint8_t *)&vbat_adc, sizeof(VBAT_ADC_CALIBRATION_TABLE));
}

void otp_cal_vbat_volt(void)
{
    VBAT_VOLTAGE_CONFIG vbat_volt;
    pmu_status_t status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_VBAT_VOLTAGE_CONFIG, (uint8_t*)&vbat_volt);
    if (status != PMU_STATUS_SUCCESS)
    {
        log_hal_msgid_error("otp_cal_vbat_volt fail, vbat_volt status[%d]", 1, status);
        return;
    }

    VBAT_ADC_CALIBRATION_TABLE vbat_adc;
    status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_VBAT_ADC_CALIBRATION_TABLE, (uint8_t*)&vbat_adc);
    if (status != PMU_STATUS_SUCCESS)
    {
        log_hal_msgid_error("otp_cal_vbat_volt fail, vbat_adc status[%d]", 1, status);
        return;
    }

    uint16_t volt1 = vbat_adc.data[0].volt;
    uint16_t adc1 = vbat_adc.data[0].adc;

    uint16_t volt2 = vbat_adc.data[1].volt;
    uint16_t adc2 = vbat_adc.data[1].adc;

    vbat_volt.initial_bat_adc  = slope_calc(volt1, adc1, volt2, adc2, vbat_volt.initial_bat);
    vbat_volt.low_bat_adc      = slope_calc(volt1, adc1, volt2, adc2, vbat_volt.low_bat);
    vbat_volt.shutdown_bat_adc = slope_calc(volt1, adc1, volt2, adc2, vbat_volt.shutdown_bat);

    for(uint8_t i=0 ; i<18 ;i++)
    {
        if (vbat_volt.data[i].check_point == 0)
        {
            if (i <= 9)
                assert(0);
            continue;
        }
        vbat_volt.data[i].adc = slope_calc(volt1, adc1, volt2, adc2, vbat_volt.data[i].check_point);
        vbat_volt.data[i].adc = range_protect(vbat_volt.data[i].adc, 0, 1023, 2);
    }
    vbat_volt.kflag = FT_FW_K;
    nvkey_write_data(NVKEYID_MP_CAL_VBAT_VOLTAGE_CONFIG, (uint8_t *)&vbat_volt, sizeof(VBAT_VOLTAGE_CONFIG));
}


void otp_cal_chg_adc(void)
{
    CHG_ADC_CONFIG chg_adc;
    pmu_status_t status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_CHG_ADC_CONFIG, (uint8_t*)&chg_adc);
    if (status != PMU_STATUS_SUCCESS)
    {
        log_hal_msgid_error("otp_cal_chg_adc fail, chg_adc status[%d]", 1, status);
        return;
    }

    OTP_VBAT_CONFIG otp_vbat;
    hal_flash_status_t flash_status = hal_flash_otp_read(OTP_VBAT_ADDR, (uint8_t *)&otp_vbat, sizeof(OTP_VBAT_CONFIG));
    if (flash_status != HAL_FLASH_STATUS_OK)
    {
        log_hal_msgid_error("otp_cal_chg_adc fail, flash_status[%d]", 1, flash_status);
        return;
    }
    if(otp_vbat.ft_kflag != MP_K)
    {
        log_hal_msgid_error("otp_cal_chg_adc fail, ft_kflag[%d]", 1, otp_vbat.ft_kflag);
        return;
    }

    uint16_t volt1 = vbat_slope.volt1;
    uint16_t adc1 = vbat_slope.adc1;
    uint16_t volt2 = otp_vbat.data[2].volt; //3.0v
    uint16_t adc2 = otp_vbat.data[2].adc;

    chg_adc.cc1_threshold_ADC = slope_calc(volt1, adc1, volt2, adc2, chg_adc.cc1_threshold_voltage + chg_adc.cc1_thd_voltage_offset);
    chg_adc.cc2_threshold_ADC = slope_calc(volt1, adc1, volt2, adc2, chg_adc.cc2_threshold_voltage + chg_adc.cc2_offset_voltage);
    chg_adc.cv_threshold_ADC  = slope_calc(volt1, adc1, volt2, adc2, chg_adc.cv_threshold_voltage + chg_adc.cv_thd_voltage_offset);
    chg_adc.full_bat_ADC      = slope_calc(volt1, adc1, volt2, adc2, chg_adc.full_bat_voltage + chg_adc.full_bat_voltage_offset);
    chg_adc.recharge_ADC      = slope_calc(volt1, adc1, volt2, adc2, chg_adc.recharge_voltage + chg_adc.recharge_voltage_offset);
    chg_adc.full_bat_ADC_2    = slope_calc(volt1, adc1, volt2, adc2, chg_adc.full_bat_voltage_2 + chg_adc.full_bat_voltage_2_offset);

    chg_adc.cc1_threshold_ADC = range_protect(chg_adc.cc1_threshold_ADC, 0, 1023, 2);
    chg_adc.cc2_threshold_ADC = range_protect(chg_adc.cc2_threshold_ADC, 0, 1023, 2);
    chg_adc.cv_threshold_ADC  = range_protect(chg_adc.cv_threshold_ADC, 0, 1023, 2);
    chg_adc.full_bat_ADC      = range_protect(chg_adc.full_bat_ADC, 0, 1023, 2);
    chg_adc.recharge_ADC      = range_protect(chg_adc.recharge_ADC, 0, 1023, 2);
    chg_adc.full_bat_ADC_2    = range_protect(chg_adc.full_bat_ADC_2, 0, 1023, 2);

    nvkey_write_data(NVKEYID_MP_CAL_CHG_ADC_CONFIG, (uint8_t *)&chg_adc, sizeof(CHG_ADC_CONFIG));
}
#endif
pmu_status_t otp_cal_vbat_config(void)
{
    CHG_CONFIG chg_config;
    pmu_status_t status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_CHG_CONFIG, (uint8_t *)&chg_config);
    if (status != PMU_STATUS_SUCCESS)
    {
        log_hal_msgid_error("otp_cal_vbat_config fail, chg_config status[%d]", 1, status);
        return PMU_STATUS_ERROR;
    }
    if (chg_config.kflag != DEFAULT_TAG)
    {
        log_hal_msgid_warning("otp_cal_vbat_config warning, chg_config.kflag[%d]", 1, chg_config.kflag);
        return PMU_STATUS_ERROR;
    }

#ifdef AIR_PMU_DISABLE_CHARGER
    log_hal_msgid_info("otp_cal_vbat_config, bypass", 0);
    return PMU_STATUS_SUCCESS;
#else
    vbat_slope.voltage_sel = chg_config.bat_volt_sel;

    otp_cal_vbat_adc();
    otp_cal_vbat_volt();
    otp_cal_chg_adc();

    return PMU_STATUS_SUCCESS;
#endif
}

void otp_cal_buck_mv(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    BUCK_MV_CONFIG buck_mv;
    pmu_status_t status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_BUCK_MV_CONFIG, (uint8_t *)&buck_mv);
    if (status != PMU_STATUS_SUCCESS)
    {
        log_hal_msgid_error("otp_cal_buck_mv fail, status[%d]", 1, status);
        return;
    }

    OTP_BUCK_MV_CONFIG otp_buck_mv;
    hal_flash_status_t flash_status = hal_flash_otp_read(OTP_BUCK_MV_ADDR, (uint8_t *)&otp_buck_mv, sizeof(OTP_BUCK_MV_CONFIG));
    if (flash_status != HAL_FLASH_STATUS_OK)
    {
        log_hal_msgid_error("otp_cal_buck_mv fail, flash_status[%d]", 1, flash_status);
        return;
    }
    if(otp_buck_mv.ft_kflag != MP_K)
    {
        log_hal_msgid_error("otp_cal_buck_mv fail, ft_kflag[%d]", 1, otp_buck_mv.ft_kflag);
        return;
    }

    volt1 = otp_buck_mv.data[0].volt;
    sel1  = otp_buck_mv.data[0].sel;
    volt2 = otp_buck_mv.data[1].volt;
    sel2  = otp_buck_mv.data[1].sel;

    for(i = 0; i < buck_mv.cal_cnt; i++)
    {
        buck_mv.data[i].value = slope_calc(volt1, sel1, volt2, sel2, buck_mv.data[i].volt);
        buck_mv.data[i].value = range_protect(buck_mv.data[i].value, 0, 255, 1);
    }
    buck_mv.kflag = FT_FW_K;
    buck_mv.sloep_value1 = sel1;
    buck_mv.sloep_value2 = sel2;
    buck_mv.adc1 = volt1;
    buck_mv.adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_BUCK_MV_CONFIG, (uint8_t *)&buck_mv, sizeof(BUCK_MV_CONFIG));
}

void otp_cal_buck_mv_ret(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_BUCK_MV_RET_CONFIG* pOTP_BUCK_MV_RET = (OTP_BUCK_MV_RET_CONFIG*)pvPortMalloc(sizeof(OTP_BUCK_MV_RET_CONFIG));
    if (!pOTP_BUCK_MV_RET)
        return;
    BUCK_MV_STB_CONFIG* pBUCK_MV_STB = (BUCK_MV_STB_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_MV_STB_CONFIG);
    if (!pBUCK_MV_STB)
    {
        vPortFree(pOTP_BUCK_MV_RET);
        return;
    }

    hal_flash_otp_read(OTP_BUCK_MV_RET_ADDR, (uint8_t *)pOTP_BUCK_MV_RET, sizeof(OTP_BUCK_MV_RET_CONFIG));

    if (pOTP_BUCK_MV_RET->ft_kflag != 1)
    {
        vPortFree(pOTP_BUCK_MV_RET);
        vPortFree(pBUCK_MV_STB);
        return;
    }

    volt1 = pOTP_BUCK_MV_RET->data[0].volt;
    sel1  = pOTP_BUCK_MV_RET->data[0].sel;
    volt2 = pOTP_BUCK_MV_RET->data[1].volt;
    sel2  = pOTP_BUCK_MV_RET->data[1].sel;

    for(i = 0; i < pBUCK_MV_STB->cal_cnt; i++)
    {
        pBUCK_MV_STB->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pBUCK_MV_STB->data[i].volt);
        pBUCK_MV_STB->data[i].value = range_protect(pBUCK_MV_STB->data[i].value, 0, 31, 1);
    }
    pBUCK_MV_STB->kflag = FT_FW_K;
    pBUCK_MV_STB->sloep_value1 = sel1;
    pBUCK_MV_STB->sloep_value2 = sel2;
    pBUCK_MV_STB->adc1 = volt1;
    pBUCK_MV_STB->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_BUCK_MV_STB_CONFIG, (uint8_t *)pBUCK_MV_STB, sizeof(BUCK_MV_STB_CONFIG));

    vPortFree(pOTP_BUCK_MV_RET);
    vPortFree(pBUCK_MV_STB);
}

void otp_cal_buck_mv_freq(void)
{
    OTP_BUCK_MV_FREQ_CONFIG* pOTP_BUCK_MV_FREQ = (OTP_BUCK_MV_FREQ_CONFIG*)pvPortMalloc(sizeof(OTP_BUCK_MV_FREQ_CONFIG));
    if (!pOTP_BUCK_MV_FREQ)
        return;
    BUCK_MV_FREQ* pBUCK_MV_FREQ = (BUCK_MV_FREQ*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_MV_FREQ);
    if (!pBUCK_MV_FREQ)
    {
        vPortFree(pOTP_BUCK_MV_FREQ);
        return;
    }

    hal_flash_otp_read(OTP_BUCK_MV_FREQ_ADDR, (uint8_t *)pOTP_BUCK_MV_FREQ, sizeof(OTP_BUCK_MV_FREQ_CONFIG));

    if (pOTP_BUCK_MV_FREQ->ft_kflag != 1)
    {
        vPortFree(pOTP_BUCK_MV_FREQ);
        vPortFree(pBUCK_MV_FREQ);
        return;
    }

    pBUCK_MV_FREQ->BUCK_FREQ_MV = range_protect(pOTP_BUCK_MV_FREQ->Freq_Trim, 0, 3, 1);
    pBUCK_MV_FREQ->OSC_FREQK_MV = range_protect(pOTP_BUCK_MV_FREQ->Freqk_Trim, 0, 7, 1);
    pBUCK_MV_FREQ->kflag = FT_FW_K;
    nvkey_write_data(NVKEYID_MP_CAL_BUCK_MV_FREQ, (uint8_t *)pBUCK_MV_FREQ, sizeof(BUCK_MV_FREQ));

    vPortFree(pOTP_BUCK_MV_FREQ);
    vPortFree(pBUCK_MV_FREQ);
}

void otp_cal_buck_lv(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_BUCK_LV_CONFIG* pOTP_BUCK_LV = (OTP_BUCK_LV_CONFIG*)pvPortMalloc(sizeof(OTP_BUCK_LV_CONFIG));
    if (!pOTP_BUCK_LV)
        return;
    BUCK_LV_CONFIG* pBUCK_LV = (BUCK_LV_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_LV_CONFIG);
    if (!pBUCK_LV)
    {
        vPortFree(pOTP_BUCK_LV);
        return;
    }

    hal_flash_otp_read(OTP_BUCK_LV_ADDR, (uint8_t *)pOTP_BUCK_LV, sizeof(OTP_BUCK_LV_CONFIG));

    if (pOTP_BUCK_LV->ft_kflag != 1)
    {
        vPortFree(pOTP_BUCK_LV);
        vPortFree(pBUCK_LV);
        return;
    }

    volt1 = pOTP_BUCK_LV->data[0].volt;
    sel1  = pOTP_BUCK_LV->data[0].sel;
    volt2 = pOTP_BUCK_LV->data[1].volt;
    sel2  = pOTP_BUCK_LV->data[1].sel;

    for(i = 0; i < pBUCK_LV->cal_cnt; i++)
    {
        pBUCK_LV->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pBUCK_LV->data[i].volt);
        pBUCK_LV->data[i].value = range_protect(pBUCK_LV->data[i].value, 0, 255, 1);
    }
    pBUCK_LV->kflag = FT_FW_K;
    pBUCK_LV->sloep_value1 = sel1;
    pBUCK_LV->sloep_value2 = sel2;
    pBUCK_LV->adc1 = volt1;
    pBUCK_LV->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_BUCK_LV_CONFIG, (uint8_t *)pBUCK_LV, sizeof(BUCK_LV_CONFIG));

    vPortFree(pOTP_BUCK_LV);
    vPortFree(pBUCK_LV);
}

void otp_cal_buck_lv_lpm(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_BUCK_LV_CONFIG* pOTP_BUCK_LV = (OTP_BUCK_LV_CONFIG*)pvPortMalloc(sizeof(OTP_BUCK_LV_CONFIG));
    if (!pOTP_BUCK_LV)
        return;
    BUCK_LV_LPM* pBUCK_LV_LPM = (BUCK_LV_LPM*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_LV_LPM);
    if (!pBUCK_LV_LPM)
    {
        vPortFree(pOTP_BUCK_LV);
        return;
    }

    hal_flash_otp_read(OTP_BUCK_LV_ADDR, (uint8_t *)pOTP_BUCK_LV, sizeof(OTP_BUCK_LV_CONFIG));

    if (pOTP_BUCK_LV->ft_kflag != 1)
    {
        vPortFree(pOTP_BUCK_LV);
        vPortFree(pBUCK_LV_LPM);
        return;
    }

    volt1 = pOTP_BUCK_LV->data[0].volt;
    sel1  = pOTP_BUCK_LV->data[0].sel;
    volt2 = pOTP_BUCK_LV->data[1].volt;
    sel2  = pOTP_BUCK_LV->data[1].sel;

    for(i = 0; i < pBUCK_LV_LPM->cal_cnt; i++)
    {
        pBUCK_LV_LPM->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pBUCK_LV_LPM->data[i].volt);
        pBUCK_LV_LPM->data[i].value = range_protect(pBUCK_LV_LPM->data[i].value, 0, 255, 1);
    }
    pBUCK_LV_LPM->kflag = FT_FW_K;
    pBUCK_LV_LPM->sloep_value1 = sel1;
    pBUCK_LV_LPM->sloep_value2 = sel2;
    pBUCK_LV_LPM->adc1 = volt1;
    pBUCK_LV_LPM->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_BUCK_LV_LPM, (uint8_t *)pBUCK_LV_LPM, sizeof(BUCK_LV_LPM));

    vPortFree(pOTP_BUCK_LV);
    vPortFree(pBUCK_LV_LPM);
}

void otp_cal_buck_lv_freq(void)
{
    OTP_BUCK_LV_FREQ_CONFIG* pOTP_BUCK_LV_FREQ = (OTP_BUCK_LV_FREQ_CONFIG*)pvPortMalloc(sizeof(OTP_BUCK_LV_FREQ_CONFIG));
    if (!pOTP_BUCK_LV_FREQ)
        return;
    BUCK_LV_FREQ* pBUCK_LV_FREQ = (BUCK_LV_FREQ*)get_nvkey_data(NVKEYID_MP_CAL_BUCK_LV_FREQ);
    if (!pBUCK_LV_FREQ)
    {
        vPortFree(pOTP_BUCK_LV_FREQ);
        return;
    }

    hal_flash_otp_read(OTP_BUCK_LV_FREQ_ADDR, (uint8_t *)pOTP_BUCK_LV_FREQ, sizeof(OTP_BUCK_LV_FREQ_CONFIG));

    if (pOTP_BUCK_LV_FREQ->ft_kflag != 1)
    {
        vPortFree(pOTP_BUCK_LV_FREQ);
        vPortFree(pBUCK_LV_FREQ);
        return;
    }

    pBUCK_LV_FREQ->BUCK_FREQ_LV = range_protect(pOTP_BUCK_LV_FREQ->Freq_Trim, 0, 3, 1);
    pBUCK_LV_FREQ->OSC_FREQK_LV = range_protect(pOTP_BUCK_LV_FREQ->Freqk_Trim, 0, 7, 1);
    pBUCK_LV_FREQ->kflag = FT_FW_K;
    nvkey_write_data(NVKEYID_MP_CAL_BUCK_LV_FREQ, (uint8_t *)pBUCK_LV_FREQ, sizeof(BUCK_LV_FREQ));

    vPortFree(pOTP_BUCK_LV_FREQ);
    vPortFree(pBUCK_LV_FREQ);
}

void otp_cal_ldo_vdd33_reg(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_LDO_VDD33_REG_CONFIG* pOTP_LDO_VDD33_REG = (OTP_LDO_VDD33_REG_CONFIG*)pvPortMalloc(sizeof(OTP_LDO_VDD33_REG_CONFIG));
    if (!pOTP_LDO_VDD33_REG)
        return;
    LDO_VDD33_REG_CONFIG* pLDO_VDD33_REG = (LDO_VDD33_REG_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VDD33_REG_CONFIG);
    if (!pLDO_VDD33_REG)
    {
        vPortFree(pOTP_LDO_VDD33_REG);
        return;
    }

    hal_flash_otp_read(OTP_LDO_VDD33_REG_ADDR, (uint8_t *)pOTP_LDO_VDD33_REG, sizeof(OTP_LDO_VDD33_REG_CONFIG));

    if (pOTP_LDO_VDD33_REG->ft_kflag != 1)
    {
        vPortFree(pOTP_LDO_VDD33_REG);
        vPortFree(pLDO_VDD33_REG);
        return;
    }

    volt1 = pOTP_LDO_VDD33_REG->data[0].volt;
    sel1  = pOTP_LDO_VDD33_REG->data[0].sel;
    volt2 = pOTP_LDO_VDD33_REG->data[1].volt;
    sel2  = pOTP_LDO_VDD33_REG->data[1].sel;

    for(i = 0; i < pLDO_VDD33_REG->cal_cnt; i++)
    {
        pLDO_VDD33_REG->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pLDO_VDD33_REG->data[i].volt);
        pLDO_VDD33_REG->data[i].value = range_protect(pLDO_VDD33_REG->data[i].value, 0, 127, 1);
    }
    pLDO_VDD33_REG->kflag = FT_FW_K;
    pLDO_VDD33_REG->sloep_value1 = sel1;
    pLDO_VDD33_REG->sloep_value2 = sel2;
    pLDO_VDD33_REG->adc1 = volt1;
    pLDO_VDD33_REG->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_LDO_VDD33_REG_CONFIG, (uint8_t *)pLDO_VDD33_REG, sizeof(LDO_VDD33_REG_CONFIG));

    vPortFree(pOTP_LDO_VDD33_REG);
    vPortFree(pLDO_VDD33_REG);
}

void otp_cal_ldo_vdd33_reg_ret(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_LDO_VDD33_REG_CONFIG* pOTP_LDO_VDD33_REG = (OTP_LDO_VDD33_REG_CONFIG*)pvPortMalloc(sizeof(OTP_LDO_VDD33_REG_CONFIG));
    if (!pOTP_LDO_VDD33_REG)
        return;
    LDO_VDD33_REG_RET_CONFIG* pLDO_VDD33_REG_RET = (LDO_VDD33_REG_RET_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VDD33_REG_RET_CONFIG);
    if (!pLDO_VDD33_REG_RET)
    {
        vPortFree(pOTP_LDO_VDD33_REG);
        return;
    }

    hal_flash_otp_read(OTP_LDO_VDD33_REG_ADDR, (uint8_t *)pOTP_LDO_VDD33_REG, sizeof(OTP_LDO_VDD33_REG_CONFIG));

    if (pOTP_LDO_VDD33_REG->ft_kflag != 1)
    {
        vPortFree(pOTP_LDO_VDD33_REG);
        vPortFree(pLDO_VDD33_REG_RET);
        return;
    }

    volt1 = pOTP_LDO_VDD33_REG->data[0].volt;
    sel1  = pOTP_LDO_VDD33_REG->data[0].sel;
    volt2 = pOTP_LDO_VDD33_REG->data[1].volt;
    sel2  = pOTP_LDO_VDD33_REG->data[1].sel;

    for(i = 0; i < pLDO_VDD33_REG_RET->cal_cnt; i++)
    {
        pLDO_VDD33_REG_RET->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pLDO_VDD33_REG_RET->data[i].volt);
        pLDO_VDD33_REG_RET->data[i].value = range_protect(pLDO_VDD33_REG_RET->data[i].value, 0, 127, 1);
    }
    pLDO_VDD33_REG_RET->kflag = FT_FW_K;
    pLDO_VDD33_REG_RET->sloep_value1 = sel1;
    pLDO_VDD33_REG_RET->sloep_value2 = sel2;
    pLDO_VDD33_REG_RET->adc1 = volt1;
    pLDO_VDD33_REG_RET->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_LDO_VDD33_REG_RET_CONFIG, (uint8_t *)pLDO_VDD33_REG_RET, sizeof(LDO_VDD33_REG_RET_CONFIG));

    vPortFree(pOTP_LDO_VDD33_REG);
    vPortFree(pLDO_VDD33_REG_RET);
}

void otp_cal_ldo_vdd33_ret(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_LDO_VDD33_RET_CONFIG* pOTP_LDO_VDD33_RET = (OTP_LDO_VDD33_RET_CONFIG*)pvPortMalloc(sizeof(OTP_LDO_VDD33_RET_CONFIG));
    if (!pOTP_LDO_VDD33_RET)
        return;
    LDO_VDD33_RET_CONFIG* pLDO_VDD33_RET = (LDO_VDD33_RET_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VDD33_RET_CONFIG);
    if (!pLDO_VDD33_RET)
    {
        vPortFree(pOTP_LDO_VDD33_RET);
        return;
    }

    hal_flash_otp_read(OTP_LDO_VDD33_RET_ADDR, (uint8_t *)pOTP_LDO_VDD33_RET, sizeof(OTP_LDO_VDD33_RET_CONFIG));

    if (pOTP_LDO_VDD33_RET->ft_kflag != 1)
    {
        vPortFree(pOTP_LDO_VDD33_RET);
        vPortFree(pLDO_VDD33_RET);
        return;
    }

    volt1 = pOTP_LDO_VDD33_RET->data[0].volt;
    sel1  = pOTP_LDO_VDD33_RET->data[0].sel;
    volt2 = pOTP_LDO_VDD33_RET->data[1].volt;
    sel2  = pOTP_LDO_VDD33_RET->data[1].sel;

    for(i = 0; i < pLDO_VDD33_RET->cal_cnt; i++)
    {
        pLDO_VDD33_RET->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pLDO_VDD33_RET->data[i].volt);
        pLDO_VDD33_RET->data[i].value = range_protect(pLDO_VDD33_RET->data[i].value, 0, 63, 1);
    }
    pLDO_VDD33_RET->kflag = FT_FW_K;
    pLDO_VDD33_RET->sloep_value1 = sel1;
    pLDO_VDD33_RET->sloep_value2 = sel2;
    pLDO_VDD33_RET->adc1 = volt1;
    pLDO_VDD33_RET->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_LDO_VDD33_RET_CONFIG, (uint8_t *)pLDO_VDD33_RET, sizeof(LDO_VDD33_RET_CONFIG));

    vPortFree(pOTP_LDO_VDD33_RET);
    vPortFree(pLDO_VDD33_RET);
}

void otp_cal_ldo_vrf_reg(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_LDO_VRF_REG_CONFIG* pOTP_LDO_VRF_REG = (OTP_LDO_VRF_REG_CONFIG*)pvPortMalloc(sizeof(OTP_LDO_VRF_REG_CONFIG));
    if (!pOTP_LDO_VRF_REG)
        return;
    LDO_VRF_REG_CONFIG* pLDO_VRF_REG = (LDO_VRF_REG_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VRF_REG_CONFIG);
    if (!pLDO_VRF_REG)
    {
        vPortFree(pOTP_LDO_VRF_REG);
        return;
    }

    hal_flash_otp_read(OTP_LDO_VRF_REG_ADDR, (uint8_t *)pOTP_LDO_VRF_REG, sizeof(OTP_LDO_VRF_REG_CONFIG));

    if (pOTP_LDO_VRF_REG->ft_kflag != 1)
    {
        vPortFree(pOTP_LDO_VRF_REG);
        vPortFree(pLDO_VRF_REG);
        return;
    }

    volt1 = pOTP_LDO_VRF_REG->data[0].volt;
    sel1  = pOTP_LDO_VRF_REG->data[0].sel;
    volt2 = pOTP_LDO_VRF_REG->data[1].volt;
    sel2  = pOTP_LDO_VRF_REG->data[1].sel;

    for(i = 0; i < pLDO_VRF_REG->cal_cnt; i++)
    {
        pLDO_VRF_REG->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pLDO_VRF_REG->data[i].volt);
        pLDO_VRF_REG->data[i].value = range_protect(pLDO_VRF_REG->data[i].value, 0, 63, 1);
    }
    pLDO_VRF_REG->kflag = FT_FW_K;
    pLDO_VRF_REG->sloep_value1 = sel1;
    pLDO_VRF_REG->sloep_value2 = sel2;
    pLDO_VRF_REG->adc1 = volt1;
    pLDO_VRF_REG->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_LDO_VRF_REG_CONFIG, (uint8_t *)pLDO_VRF_REG, sizeof(LDO_VRF_REG_CONFIG));

    vPortFree(pOTP_LDO_VRF_REG);
    vPortFree(pLDO_VRF_REG);
}

void otp_cal_ldo_vrf_reg_ret(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_LDO_VRF_REG_CONFIG* pOTP_LDO_VRF_REG = (OTP_LDO_VRF_REG_CONFIG*)pvPortMalloc(sizeof(OTP_LDO_VRF_REG_CONFIG));
    if (!pOTP_LDO_VRF_REG)
        return;
    LDO_VRF_REG_RET_CONFIG* pLDO_VRF_REG_RET = (LDO_VRF_REG_RET_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VRF_REG_RET_CONFIG);
    if (!pLDO_VRF_REG_RET)
    {
        vPortFree(pOTP_LDO_VRF_REG);
        return;
    }

    hal_flash_otp_read(OTP_LDO_VRF_REG_ADDR, (uint8_t *)pOTP_LDO_VRF_REG, sizeof(OTP_LDO_VRF_REG_CONFIG));

    if (pOTP_LDO_VRF_REG->ft_kflag != 1)
    {
        vPortFree(pOTP_LDO_VRF_REG);
        vPortFree(pLDO_VRF_REG_RET);
        return;
    }

    volt1 = pOTP_LDO_VRF_REG->data[0].volt;
    sel1  = pOTP_LDO_VRF_REG->data[0].sel;
    volt2 = pOTP_LDO_VRF_REG->data[1].volt;
    sel2  = pOTP_LDO_VRF_REG->data[1].sel;

    for(i = 0; i < pLDO_VRF_REG_RET->cal_cnt; i++)
    {
        pLDO_VRF_REG_RET->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pLDO_VRF_REG_RET->data[i].volt);
        pLDO_VRF_REG_RET->data[i].value = range_protect(pLDO_VRF_REG_RET->data[i].value, 0, 63, 1);
    }
    pLDO_VRF_REG_RET->kflag = FT_FW_K;
    pLDO_VRF_REG_RET->sloep_value1 = sel1;
    pLDO_VRF_REG_RET->sloep_value2 = sel2;
    pLDO_VRF_REG_RET->adc1 = volt1;
    pLDO_VRF_REG_RET->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_LDO_VRF_REG_RET_CONFIG, (uint8_t *)pLDO_VRF_REG_RET, sizeof(LDO_VRF_REG_RET_CONFIG));

    vPortFree(pOTP_LDO_VRF_REG);
    vPortFree(pLDO_VRF_REG_RET);
}

void otp_cal_ldo_vrf_ret(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_LDO_VRF_RET_CONFIG* pOTP_LDO_VRF_RET = (OTP_LDO_VRF_RET_CONFIG*)pvPortMalloc(sizeof(OTP_LDO_VRF_RET_CONFIG));
    if (!pOTP_LDO_VRF_RET)
        return;
    LDO_VRF_RET_CONFIG* pLDO_VRF_RET = (LDO_VRF_RET_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LDO_VRF_RET_CONFIG);
    if (!pLDO_VRF_RET)
    {
        vPortFree(pOTP_LDO_VRF_RET);
        return;
    }

    hal_flash_otp_read(OTP_LDO_VRF_RET_ADDR, (uint8_t *)pOTP_LDO_VRF_RET, sizeof(OTP_LDO_VRF_RET_CONFIG));

    if (pOTP_LDO_VRF_RET->ft_kflag != 1)
    {
        vPortFree(pOTP_LDO_VRF_RET);
        vPortFree(pLDO_VRF_RET);
        return;
    }

    volt1 = pOTP_LDO_VRF_RET->data[0].volt;
    sel1  = pOTP_LDO_VRF_RET->data[0].sel;
    volt2 = pOTP_LDO_VRF_RET->data[1].volt;
    sel2  = pOTP_LDO_VRF_RET->data[1].sel;

    for(i = 0; i < pLDO_VRF_RET->cal_cnt; i++)
    {
        pLDO_VRF_RET->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pLDO_VRF_RET->data[i].volt);
        pLDO_VRF_RET->data[i].value = range_protect(pLDO_VRF_RET->data[i].value, 0, 31, 1);
    }
    pLDO_VRF_RET->kflag = FT_FW_K;
    pLDO_VRF_RET->sloep_value1 = sel1;
    pLDO_VRF_RET->sloep_value2 = sel2;
    pLDO_VRF_RET->adc1 = volt1;
    pLDO_VRF_RET->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_LDO_VRF_RET_CONFIG, (uint8_t *)pLDO_VRF_RET, sizeof(LDO_VRF_RET_CONFIG));

    vPortFree(pOTP_LDO_VRF_RET);
    vPortFree(pLDO_VRF_RET);
}

void otp_cal_ldo_vdig18(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_LDO_VDIG18_CONFIG* pOTP_LDO_VDIG18 = (OTP_LDO_VDIG18_CONFIG*)pvPortMalloc(sizeof(OTP_LDO_VDIG18_CONFIG));
    if (!pOTP_LDO_VDIG18)
        return;
    LDO_VDIG18_CONFIG* pLDO_VDIG18 = (LDO_VDIG18_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_VDIG18_CONFIG);
    if (!pLDO_VDIG18)
    {
        vPortFree(pOTP_LDO_VDIG18);
        return;
    }

    hal_flash_otp_read(OTP_LDO_VDIG18_ADDR, (uint8_t *)pOTP_LDO_VDIG18, sizeof(OTP_LDO_VDIG18_CONFIG));

    if (pOTP_LDO_VDIG18->ft_kflag != 1)
    {
        vPortFree(pOTP_LDO_VDIG18);
        vPortFree(pLDO_VDIG18);
        return;
    }

    volt1 = pOTP_LDO_VDIG18->data[0].volt;
    sel1  = pOTP_LDO_VDIG18->data[0].sel;
    volt2 = pOTP_LDO_VDIG18->data[1].volt;
    sel2  = pOTP_LDO_VDIG18->data[1].sel;

    for(i = 0; i < pLDO_VDIG18->cal_cnt; i++)
    {
        if (pLDO_VDIG18->data[i].volt == 0)
        {
            pLDO_VDIG18->data[i].value = 0;
            continue;
        }
        if ((sel1 >= 16 && sel2 >= 16) || (sel1 < 16 && sel2 < 16))
        {
            /*pLDO_VDIG18->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pLDO_VDIG18->data[i].volt);
            if (pLDO_VDIG18->data[i].value & 0x80)
            {
                pLDO_VDIG18->data[i].value += 32;
            }
            else if (pLDO_VDIG18->data[i].value > 31)
            {
                pLDO_VDIG18->data[i].value -= 32;
            }*/
            assert(0);
        }
        else
        {
            pLDO_VDIG18->data[i].value = vdig_bg_calculation(volt1, sel1, volt2, sel2, pLDO_VDIG18->data[i].volt);
        }
        pLDO_VDIG18->data[i].value = (uint8_t)range_protect(pLDO_VDIG18->data[i].value, 0, 31, 1);
    }
    pLDO_VDIG18->kflag = FT_FW_K;
    pLDO_VDIG18->sloep_value1 = sel1;
    pLDO_VDIG18->sloep_value2 = sel2;
    pLDO_VDIG18->adc1 = volt1;
    pLDO_VDIG18->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_VDIG18_CONFIG, (uint8_t *)pLDO_VDIG18, sizeof(LDO_VDIG18_CONFIG));

    vPortFree(pOTP_LDO_VDIG18);
    vPortFree(pLDO_VDIG18);
}

void otp_cal_hpbg(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_HPBG_CONFIG* pOTP_HPBG = (OTP_HPBG_CONFIG*)pvPortMalloc(sizeof(OTP_HPBG_CONFIG));
    if (!pOTP_HPBG)
        return;
    HPBG_CONFIG* pHPBG = (HPBG_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_HPBG_CONFIG);
    if (!pHPBG)
    {
        vPortFree(pOTP_HPBG);
        return;
    }

    hal_flash_otp_read(OTP_HPBG_ADDR, (uint8_t *)pOTP_HPBG, sizeof(OTP_HPBG_CONFIG));

    if (pOTP_HPBG->ft_kflag != 1)
    {
        vPortFree(pOTP_HPBG);
        vPortFree(pHPBG);
        return;
    }

    volt1 = pOTP_HPBG->data[0].volt;
    sel1  = pOTP_HPBG->data[0].sel;
    volt2 = pOTP_HPBG->data[1].volt;
    sel2  = pOTP_HPBG->data[1].sel;

    for(i = 0; i < pHPBG->cal_cnt; i++)
    {
        if ((sel1 >= 16 && sel2 >= 16) || (sel1 < 16 && sel2 < 16))
        {
            /*pHPBG->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pHPBG->data[i].volt);
            if (pHPBG->data[i].value & 0x80)
            {
                pHPBG->data[i].value += 32;
            }
            else if (pHPBG->data[i].value > 31)
            {
                pHPBG->data[i].value -= 32;
            }*/
            assert(0);
        }
        else
        {
            pHPBG->data[i].value = vdig_bg_calculation(volt1, sel1, volt2, sel2, pHPBG->data[i].volt);
        }
        pHPBG->data[i].value = (uint8_t)range_protect(pHPBG->data[i].value, 0, 31, 1);
    }
    pHPBG->kflag = FT_FW_K;
    pHPBG->sloep_value1 = sel1;
    pHPBG->sloep_value2 = sel2;
    pHPBG->adc1 = volt1;
    pHPBG->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_HPBG_CONFIG, (uint8_t *)pHPBG, sizeof(HPBG_CONFIG));

    vPortFree(pOTP_HPBG);
    vPortFree(pHPBG);
}

void otp_cal_lpbg(void)
{
    uint8_t i;
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_LPBG_CONFIG* pOTP_LPBG = (OTP_LPBG_CONFIG*)pvPortMalloc(sizeof(OTP_LPBG_CONFIG));
    if (!pOTP_LPBG)
        return;
    LPBG_CONFIG* pLPBG = (LPBG_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_LPBG_CONFIG);
    if (!pLPBG)
    {
        vPortFree(pOTP_LPBG);
        return;
    }

    hal_flash_otp_read(OTP_LPBG_ADDR, (uint8_t *)pOTP_LPBG, sizeof(OTP_LPBG_CONFIG));

    if (pOTP_LPBG->ft_kflag != 1)
    {
        vPortFree(pOTP_LPBG);
        vPortFree(pLPBG);
        return;
    }

    volt1 = pOTP_LPBG->data[0].volt;
    sel1  = pOTP_LPBG->data[0].sel;
    volt2 = pOTP_LPBG->data[1].volt;
    sel2  = pOTP_LPBG->data[1].sel;

    for(i = 0; i < pLPBG->cal_cnt; i++)
    {
        if ((sel1 >= 16 && sel2 >= 16) || (sel1 < 16 && sel2 < 16))
        {
            /*pLPBG->data[i].value = slope_calc(volt1, sel1, volt2, sel2, pLPBG->data[i].volt);
            if (pLPBG->data[i].value & 0x80)
            {
                pLPBG->data[i].value += 32;
            }
            else if (pLPBG->data[i].value > 31)
            {
                pLPBG->data[i].value -= 32;
            }*/
            assert(0);
        }
        else
        {
            pLPBG->data[i].value = vdig_bg_calculation(volt1, sel1, volt2, sel2, pLPBG->data[i].volt);
        }
        pLPBG->data[i].value = (uint8_t)range_protect(pLPBG->data[i].value, 0, 31, 1);
    }
    pLPBG->kflag = FT_FW_K;
    pLPBG->sloep_value1 = sel1;
    pLPBG->sloep_value2 = sel2;
    pLPBG->adc1 = volt1;
    pLPBG->adc2 = volt2;
    nvkey_write_data(NVKEYID_MP_CAL_LPBG_CONFIG, (uint8_t *)pLPBG, sizeof(LPBG_CONFIG));

    vPortFree(pOTP_LPBG);
    vPortFree(pLPBG);
}
#ifndef AIR_PMU_DISABLE_CHARGER
void otp_cal_chg_dac(void)
{
    uint16_t chg_dac_val;

    OTP_CHG_DAC_CONFIG* pOTP_CHG_DAC = (OTP_CHG_DAC_CONFIG*)pvPortMalloc(sizeof(OTP_CHG_DAC_CONFIG));
    if (!pOTP_CHG_DAC)
        return;
    INT_CHG_DAC_CONFIG* pINT_CHG_DAC = (INT_CHG_DAC_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_INT_CHG_DAC_CONFIG);
    if (!pINT_CHG_DAC)
    {
        vPortFree(pOTP_CHG_DAC);
        return;
    }

    hal_flash_otp_read(OTP_CHG_DAC_ADDR, (uint8_t *)pOTP_CHG_DAC, sizeof(OTP_CHG_DAC_CONFIG));

    if((pOTP_CHG_DAC->ft_kflag != 1))
    {
        vPortFree(pOTP_CHG_DAC);
        vPortFree(pINT_CHG_DAC);
        return;
    }

    dac_slope.volt1 = 4350;
    dac_slope.dac1 = pOTP_CHG_DAC->DAC_4_35V;
    dac_slope.volt2 = 4200;
    dac_slope.dac2 = pOTP_CHG_DAC->DAC_4_2V;

    chg_dac_val= slope_calc(dac_slope.volt1, dac_slope.dac1, dac_slope.volt2, dac_slope.dac2, vbat_slope.volt1);
    chg_dac_val = range_protect(chg_dac_val, 0, 1023, 2);

    pINT_CHG_DAC->tricklecurrentDAC = chg_dac_val;
    pINT_CHG_DAC->cc1currentDAC = chg_dac_val;
    pINT_CHG_DAC->cc2currentDAC = chg_dac_val;
    pINT_CHG_DAC->cvDAC = chg_dac_val;
    nvkey_write_data(NVKEYID_MP_CAL_INT_CHG_DAC_CONFIG, (uint8_t *)pINT_CHG_DAC, sizeof(INT_CHG_DAC_CONFIG));

    vPortFree(pOTP_CHG_DAC);
    vPortFree(pINT_CHG_DAC);
}
#endif
void otp_cal_chg_sys_ldo(void)
{
    uint16_t volt1 = 0;
    uint16_t sel1 = 0;
    uint16_t volt2 = 0;
    uint16_t sel2 = 0;

    OTP_VSYS_LDO_CONFIG* pOTP_VSYS_LDO = (OTP_VSYS_LDO_CONFIG*)pvPortMalloc(sizeof(OTP_VSYS_LDO_CONFIG));
    if (!pOTP_VSYS_LDO)
        return;
    CHG_SYS_LDO* pSYS_LDO = (CHG_SYS_LDO*)get_nvkey_data(NVKEYID_MP_CAL_SYS_LDO);
    if (!pSYS_LDO)
    {
        vPortFree(pOTP_VSYS_LDO);
        return;
    }

    hal_flash_otp_read(OTP_VSYS_LDO_ADDR, (uint8_t *)pOTP_VSYS_LDO, sizeof(OTP_VSYS_LDO_CONFIG));

    if((pOTP_VSYS_LDO->ft_kflag != 1))
    {
        vPortFree(pOTP_VSYS_LDO);
        vPortFree(pSYS_LDO);
        return;
    }

    volt1 = pOTP_VSYS_LDO->data[0].volt;
    sel1  = pOTP_VSYS_LDO->data[0].sel;
    volt2 = pOTP_VSYS_LDO->data[1].volt;
    sel2  = pOTP_VSYS_LDO->data[1].sel;

    pSYS_LDO->CHG_LDO_SEL = slope_calc(volt1, sel1, volt2, sel2, pSYS_LDO->SYSLDO_output_voltage);
    pSYS_LDO->CHG_LDO_SEL = range_protect(pSYS_LDO->CHG_LDO_SEL, 0, 7, 1);

    nvkey_write_data(NVKEYID_MP_CAL_SYS_LDO, (uint8_t *)pSYS_LDO, sizeof(CHG_SYS_LDO));

    vPortFree(pOTP_VSYS_LDO);
    vPortFree(pSYS_LDO);
}

void otp_cal_chg_ocp(void)
{
    OTP_OCP_CONFIG* pOTP_OCP = (OTP_OCP_CONFIG*)pvPortMalloc(sizeof(OTP_OCP_CONFIG));
    if (!pOTP_OCP)
        return;
    CHG_OCP* pOCP = (CHG_OCP*)get_nvkey_data(NVKEYID_MP_CAL_OCP);
    if (!pOCP)
    {
        vPortFree(pOTP_OCP);
        return;
    }

    hal_flash_otp_read(OTP_OCP_ADDR, (uint8_t *)pOTP_OCP, sizeof(OTP_OCP_CONFIG));

    if((pOTP_OCP->ft_kflag != 1))
    {
        vPortFree(pOTP_OCP);
        vPortFree(pOCP);
        return;
    }
    pOCP->SW_OC_LMT = range_protect(pOCP->SW_OC_LMT, 0, 15, 1);
    pOCP->I_LIM_TRIM = range_protect(pOCP->I_LIM_TRIM, 0, 7, 1);
    nvkey_write_data(NVKEYID_MP_CAL_OCP, (uint8_t *)pOCP, sizeof(CHG_OCP));

    vPortFree(pOTP_OCP);
    vPortFree(pOCP);
}
#ifndef AIR_PMU_DISABLE_CHARGER
void otp_cal_chg_trickle_current(void)
{
    uint16_t i, rsel_4v35, rsel_4v2;

    INT_CHG_TRICKLE_CURRENT_CONFIG* pCHG_TRICKLE_CURRENT = (INT_CHG_TRICKLE_CURRENT_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_INT_CHG_TRICKLE_CURRENT_CONFIG);
    if(!pCHG_TRICKLE_CURRENT)
        return;

    for(i = 0; i < pCHG_TRICKLE_CURRENT->cal_cnt; i++)
    {
        rsel_4v35 = slope_calc(chg_curr_4v35_slope.curr1, chg_curr_4v35_slope.val1, chg_curr_4v35_slope.curr2, chg_curr_4v35_slope.val2, pCHG_TRICKLE_CURRENT->data[i].current);
        rsel_4v2 = slope_calc(chg_curr_4v2_slope.curr1, chg_curr_4v2_slope.val1, chg_curr_4v2_slope.curr2, chg_curr_4v2_slope.val2, pCHG_TRICKLE_CURRENT->data[i].current);

        pCHG_TRICKLE_CURRENT->data[i].rchg_sel = slope_calc(dac_slope.volt1, rsel_4v35, dac_slope.volt2, rsel_4v2, vbat_slope.volt1);
        pCHG_TRICKLE_CURRENT->data[i].rchg_sel = range_protect(pCHG_TRICKLE_CURRENT->data[i].rchg_sel, 0, 1023, 2);
    }
    nvkey_write_data(NVKEYID_MP_CAL_INT_CHG_TRICKLE_CURRENT_CONFIG, (uint8_t *)pCHG_TRICKLE_CURRENT, sizeof(INT_CHG_TRICKLE_CURRENT_CONFIG));

    vPortFree(pCHG_TRICKLE_CURRENT);
}

void otp_cal_chg_cc1_current(void)
{
    uint16_t i, rsel_4v35, rsel_4v2;

    INT_CHG_CC1_CURRENT_CONFIG* pCHG_CC1_CURRENT = (INT_CHG_CC1_CURRENT_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_INT_CHG_CC1_CURRENT_CONFIG);
    if(!pCHG_CC1_CURRENT)
        return;

    for(i = 0; i < pCHG_CC1_CURRENT->cal_cnt; i++)
    {
        rsel_4v35 = slope_calc(chg_curr_4v35_slope.curr1, chg_curr_4v35_slope.val1, chg_curr_4v35_slope.curr2, chg_curr_4v35_slope.val2, pCHG_CC1_CURRENT->data[i].current);
        rsel_4v2 = slope_calc(chg_curr_4v2_slope.curr1, chg_curr_4v2_slope.val1, chg_curr_4v2_slope.curr2, chg_curr_4v2_slope.val2, pCHG_CC1_CURRENT->data[i].current);

        pCHG_CC1_CURRENT->data[i].rchg_sel = slope_calc(dac_slope.volt1, rsel_4v35, dac_slope.volt2, rsel_4v2, vbat_slope.volt1);
        pCHG_CC1_CURRENT->data[i].rchg_sel = range_protect(pCHG_CC1_CURRENT->data[i].rchg_sel, 0, 1023, 2);
    }
    chg_cc_curr.cc1_curr = pCHG_CC1_CURRENT->data[pCHG_CC1_CURRENT->select - 1].current;
    nvkey_write_data(NVKEYID_MP_CAL_INT_CHG_CC1_CURRENT_CONFIG, (uint8_t *)pCHG_CC1_CURRENT, sizeof(INT_CHG_CC1_CURRENT_CONFIG));

    vPortFree(pCHG_CC1_CURRENT);
}

void otp_cal_chg_cc2_current(void)
{
    uint16_t i, rsel_4v35, rsel_4v2;

    INT_CHG_CC2_CURRENT_CONFIG* pCHG_CC2_CURRENT = (INT_CHG_CC2_CURRENT_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_INT_CHG_CC2_CURRENT_CONFIG);
    if(!pCHG_CC2_CURRENT)
        return;

    for(i = 0; i < pCHG_CC2_CURRENT->cal_cnt; i++)
    {
        rsel_4v35 = slope_calc(chg_curr_4v35_slope.curr1, chg_curr_4v35_slope.val1, chg_curr_4v35_slope.curr2, chg_curr_4v35_slope.val2, pCHG_CC2_CURRENT->data[i].current);
        rsel_4v2 = slope_calc(chg_curr_4v2_slope.curr1, chg_curr_4v2_slope.val1, chg_curr_4v2_slope.curr2, chg_curr_4v2_slope.val2, pCHG_CC2_CURRENT->data[i].current);

        pCHG_CC2_CURRENT->data[i].rchg_sel = slope_calc(dac_slope.volt1, rsel_4v35, dac_slope.volt2, rsel_4v2, vbat_slope.volt1);
        pCHG_CC2_CURRENT->data[i].rchg_sel = range_protect(pCHG_CC2_CURRENT->data[i].rchg_sel, 0, 1023, 2);
    }
    chg_cc_curr.cc2_curr = pCHG_CC2_CURRENT->data[pCHG_CC2_CURRENT->select - 1].current;
    nvkey_write_data(NVKEYID_MP_CAL_INT_CHG_CC2_CURRENT_CONFIG, (uint8_t *)pCHG_CC2_CURRENT, sizeof(INT_CHG_CC2_CURRENT_CONFIG));

    vPortFree(pCHG_CC2_CURRENT);
}

void otp_cal_chg_cv_stop_current(void)
{
    uint16_t val = 0;

    OTP_VICHG_ADC_CONFIG* pOTP_VICHG_ADC = (OTP_VICHG_ADC_CONFIG*)pvPortMalloc(sizeof(OTP_VICHG_ADC_CONFIG));
    if (!pOTP_VICHG_ADC)
        return;
    CHG_CV_STOP_CURRENT_ADC* pCHG_CV_STOP_CURRENT = (CHG_CV_STOP_CURRENT_ADC*)get_nvkey_data(NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC);
    if (!pCHG_CV_STOP_CURRENT)
    {
        vPortFree(pOTP_VICHG_ADC);
        return;
    }

    hal_flash_otp_read(OTP_VICHG_ADC_VALUE_ADDR, (uint8_t *)pOTP_VICHG_ADC, sizeof(OTP_VICHG_ADC_CONFIG));

    if((pOTP_VICHG_ADC->ft_kflag != 1))
    {
        vPortFree(pOTP_VICHG_ADC);
        vPortFree(pCHG_CV_STOP_CURRENT);
        return;
    }

    chg_vichg_adc.ADC_4_35V = pOTP_VICHG_ADC->ADC_4_35V;
    chg_vichg_adc.ADC_4_2V = pOTP_VICHG_ADC->ADC_4_2V;
    chg_vichg_adc.CV_stop_current_percent = pCHG_CV_STOP_CURRENT->CV_stop_current_percent;

    val = slope_calc(dac_slope.volt1, chg_vichg_adc.ADC_4_35V, dac_slope.volt2, chg_vichg_adc.ADC_4_2V, vbat_slope.volt1);

    pCHG_CV_STOP_CURRENT->CV_stop_current_ADC = mpk_round((pCHG_CV_STOP_CURRENT->CV_stop_current_percent * val), 100);
    pCHG_CV_STOP_CURRENT->CV_stop_current_ADC = range_protect(pCHG_CV_STOP_CURRENT->CV_stop_current_ADC, 0, 1023, 2);
    log_hal_msgid_info("otp_cal_chg_cv_stop_current, CV_stop_current_ADC[%d]", 1, pCHG_CV_STOP_CURRENT->CV_stop_current_ADC);

    nvkey_write_data(NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC, (uint8_t *)pCHG_CV_STOP_CURRENT, sizeof(CHG_CV_STOP_CURRENT_ADC));

    vPortFree(pOTP_VICHG_ADC);
    vPortFree(pCHG_CV_STOP_CURRENT);
}

void otp_cal_chg_current(void)
{
    OTP_CHG_4V2_CURRENT_CONFIG* pOTP_CHG_4V2 = (OTP_CHG_4V2_CURRENT_CONFIG*)pvPortMalloc(sizeof(OTP_CHG_4V2_CURRENT_CONFIG));
    if (!pOTP_CHG_4V2)
        return;
    OTP_CHG_4V35_CURRENT_CONFIG* pOTP_CHG_4V35 = (OTP_CHG_4V35_CURRENT_CONFIG*)pvPortMalloc(sizeof(OTP_CHG_4V35_CURRENT_CONFIG));
    if (!pOTP_CHG_4V35)
    {
        vPortFree(pOTP_CHG_4V2);
        return;
    }

    hal_flash_otp_read(OTP_CHG_4V2_CURRENT_ADDR, (uint8_t *)pOTP_CHG_4V2, sizeof(OTP_CHG_4V2_CURRENT_CONFIG));
    hal_flash_otp_read(OTP_CHG_4V35_CURRENT_ADDR, (uint8_t *)pOTP_CHG_4V35, sizeof(OTP_CHG_4V35_CURRENT_CONFIG));

    if((pOTP_CHG_4V2->ft_kflag != 1) || (pOTP_CHG_4V35->ft_kflag != 1))
    {
        vPortFree(pOTP_CHG_4V2);
        vPortFree(pOTP_CHG_4V35);
        return;
    }

    chg_curr_4v35_slope.curr1 = pOTP_CHG_4V35->current_value[0].dedicated_current;
    chg_curr_4v35_slope.val1  = pOTP_CHG_4V35->current_value[0].estimated_measured_value;
    chg_curr_4v35_slope.curr2 = pOTP_CHG_4V35->current_value[1].dedicated_current;
    chg_curr_4v35_slope.val2  = pOTP_CHG_4V35->current_value[1].estimated_measured_value;
    chg_curr_4v2_slope.curr1  = pOTP_CHG_4V2->current_value[0].dedicated_current;
    chg_curr_4v2_slope.val1   = pOTP_CHG_4V2->current_value[0].estimated_measured_value;
    chg_curr_4v2_slope.curr2  = pOTP_CHG_4V2->current_value[1].dedicated_current;
    chg_curr_4v2_slope.val2   = pOTP_CHG_4V2->current_value[1].estimated_measured_value;

    otp_cal_chg_trickle_current();
    otp_cal_chg_cc1_current();
    otp_cal_chg_cc2_current();
    otp_cal_chg_cv_stop_current();

    vPortFree(pOTP_CHG_4V2);
    vPortFree(pOTP_CHG_4V35);
}

void otp_cal_chg_jeita_warm(void)
{
    uint16_t sel_volt, curr_dac;
    uint16_t cc1_rsel_4v35 = 0, cc1_rsel_4v2 = 0, cc2_rsel_4v35 = 0, cc2_rsel_4v2 = 0, cv_stop_curr_adc;

    CHG_ADC_CONFIG* pCHG_ADC =(CHG_ADC_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_CHG_ADC_CONFIG);
    if (!pCHG_ADC)
        return;
    JEITA* pJEITA =(JEITA*)get_nvkey_data(NVKEYID_MP_CAL_JEITA);
    if (!pJEITA)
    {
        vPortFree(pCHG_ADC);
        return;
    }
    JEITA_WARM* pJEITA_WARM =(JEITA_WARM*)get_nvkey_data(NVKEYID_MP_CAL_JEITA_WARM);
    if (!pJEITA_WARM)
    {
        vPortFree(pCHG_ADC);
        vPortFree(pJEITA);
        return;
    }

    pJEITA_WARM->JEITA_warm_cc2_threshold_voltage = pCHG_ADC->cc2_threshold_voltage + pCHG_ADC->cc2_offset_voltage;
    pJEITA_WARM->JEITA_warm_cv_threshold_voltage  = pCHG_ADC->cv_threshold_voltage+ pCHG_ADC->cv_thd_voltage_offset - pJEITA->Warm_State_DAC_decrease;
    pJEITA_WARM->JEITA_warm_full_bat_voltage      = pCHG_ADC->full_bat_voltage + pCHG_ADC->full_bat_voltage_offset - pJEITA->Warm_State_DAC_decrease;
    pJEITA_WARM->JEITA_warm_recharge_voltage      = pCHG_ADC->recharge_voltage+ pCHG_ADC->recharge_voltage_offset - pJEITA->Warm_State_DAC_decrease;
    pJEITA_WARM->JEITA_warm_full_bat_voltage_2    = pCHG_ADC->full_bat_voltage_2 + pCHG_ADC->full_bat_voltage_2_offset - pJEITA->Warm_State_DAC_decrease;

    pJEITA_WARM->JEITA_warm_cc2_threshold_ADC = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_WARM->JEITA_warm_cc2_threshold_voltage);
    pJEITA_WARM->JEITA_warm_cv_threshold_ADC  = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_WARM->JEITA_warm_cv_threshold_voltage);
    pJEITA_WARM->JEITA_warm_full_bat_ADC      = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_WARM->JEITA_warm_full_bat_voltage);
    pJEITA_WARM->JEITA_warm_recharge_ADC      = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_WARM->JEITA_warm_recharge_voltage);
    pJEITA_WARM->JEITA_warm_full_bat_ADC_2    = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_WARM->JEITA_warm_full_bat_voltage_2);

    pJEITA_WARM->JEITA_warm_cc2_threshold_ADC = range_protect(pJEITA_WARM->JEITA_warm_cc2_threshold_ADC, 0, 1023, 2);
    pJEITA_WARM->JEITA_warm_cv_threshold_ADC  = range_protect(pJEITA_WARM->JEITA_warm_cv_threshold_ADC, 0, 1023, 2);
    pJEITA_WARM->JEITA_warm_full_bat_ADC      = range_protect(pJEITA_WARM->JEITA_warm_full_bat_ADC, 0, 1023, 2);
    pJEITA_WARM->JEITA_warm_recharge_ADC      = range_protect(pJEITA_WARM->JEITA_warm_recharge_ADC, 0, 1023, 2);
    pJEITA_WARM->JEITA_warm_full_bat_ADC_2    = range_protect(pJEITA_WARM->JEITA_warm_full_bat_ADC_2, 0, 1023, 2);

    sel_volt = vbat_slope.volt1 - pJEITA->Warm_State_DAC_decrease;
    curr_dac = slope_calc(dac_slope.volt1, dac_slope.dac1, dac_slope.volt2, dac_slope.dac2, sel_volt);
    curr_dac = range_protect(curr_dac, 0, 1023, 2);

    pJEITA_WARM->JEITA_warm_cc1_current_DAC = curr_dac;
    pJEITA_WARM->JEITA_warm_cc2_current_DAC = curr_dac;
    pJEITA_WARM->JEITA_warm_cv_DAC          = curr_dac;

    pJEITA_WARM->JEITA_warm_cc1_current_1 = mpk_round((chg_cc_curr.cc1_curr * pJEITA->Warm_State_Current_Percent), 100);
    pJEITA_WARM->JEITA_warm_cc2_current_1 = mpk_round((chg_cc_curr.cc2_curr * pJEITA->Warm_State_Current_Percent), 100);

    cc1_rsel_4v2  = slope_calc(chg_curr_4v2_slope.curr1, chg_curr_4v2_slope.val1, chg_curr_4v2_slope.curr2, chg_curr_4v2_slope.val2, pJEITA_WARM->JEITA_warm_cc1_current_1);
    cc1_rsel_4v2  = range_protect(cc1_rsel_4v2, 0, 1023, 2);
    cc1_rsel_4v35 = slope_calc(chg_curr_4v35_slope.curr1, chg_curr_4v35_slope.val1, chg_curr_4v35_slope.curr2, chg_curr_4v35_slope.val2, pJEITA_WARM->JEITA_warm_cc1_current_1);
    cc1_rsel_4v35 = range_protect(cc1_rsel_4v35, 0, 1023, 2);
    pJEITA_WARM->JEITA_warm_rchg_sel_cc1_1 = slope_calc(dac_slope.volt1, cc1_rsel_4v35, dac_slope.volt2, cc1_rsel_4v2, sel_volt);
    pJEITA_WARM->JEITA_warm_rchg_sel_cc1_1 = range_protect(pJEITA_WARM->JEITA_warm_rchg_sel_cc1_1, 0, 1023, 2);

    cc2_rsel_4v2  = slope_calc(chg_curr_4v2_slope.curr1, chg_curr_4v2_slope.val1, chg_curr_4v2_slope.curr2, chg_curr_4v2_slope.val2, pJEITA_WARM->JEITA_warm_cc2_current_1);
    cc2_rsel_4v2  = range_protect(cc2_rsel_4v2, 0, 1023, 2);
    cc2_rsel_4v35 = slope_calc(chg_curr_4v35_slope.curr1, chg_curr_4v35_slope.val1, chg_curr_4v35_slope.curr2, chg_curr_4v35_slope.val2, pJEITA_WARM->JEITA_warm_cc2_current_1);
    cc2_rsel_4v35 = range_protect(cc2_rsel_4v35, 0, 1023, 2);
    pJEITA_WARM->JEITA_warm_rchg_sel_cc2_1 = slope_calc(dac_slope.volt1, cc2_rsel_4v35, dac_slope.volt2, cc2_rsel_4v2, sel_volt);
    pJEITA_WARM->JEITA_warm_rchg_sel_cc2_1 = range_protect(pJEITA_WARM->JEITA_warm_rchg_sel_cc2_1, 0, 1023, 2);

    cv_stop_curr_adc = slope_calc(dac_slope.volt1, chg_vichg_adc.ADC_4_35V, dac_slope.volt2, chg_vichg_adc.ADC_4_2V, sel_volt);
    cv_stop_curr_adc = range_protect(cv_stop_curr_adc, 0, 1023, 2);
    pJEITA_WARM->JEITA_warm_CV_stop_current_ADC = mpk_round((cv_stop_curr_adc * chg_vichg_adc.CV_stop_current_percent), 100);

    CHG_CV_STOP_CURRENT_ADC* pCHG_CV_STOP_CURRENT =(CHG_CV_STOP_CURRENT_ADC*)get_nvkey_data(NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC);
    if (pCHG_CV_STOP_CURRENT)
        pJEITA_WARM->JEITA_warm_CV_stop_current_ADC = mpk_round((pCHG_CV_STOP_CURRENT->CV_stop_current_ADC * 100), pJEITA->Warm_State_Current_Percent);
    log_hal_msgid_info("otp_cal_chg_jeita_warm, JEITA_warm_CV_stop_current_ADC[%d]", 1, pJEITA_WARM->JEITA_warm_CV_stop_current_ADC);

    nvkey_write_data(NVKEYID_MP_CAL_JEITA_WARM, (uint8_t *)pJEITA_WARM, sizeof(JEITA_WARM));

    vPortFree(pCHG_ADC);
    vPortFree(pJEITA);
    vPortFree(pJEITA_WARM);
    vPortFree(pCHG_CV_STOP_CURRENT);
}

void otp_cal_chg_jeita_cool(void)
{
    uint16_t sel_volt, curr_dac;
    uint16_t cc1_rsel_4v35 = 0, cc1_rsel_4v2 = 0, cc2_rsel_4v35 = 0, cc2_rsel_4v2 = 0, cv_stop_curr_adc;

    CHG_ADC_CONFIG* pCHG_ADC =(CHG_ADC_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_CHG_ADC_CONFIG);
    if (!pCHG_ADC)
        return;
    JEITA* pJEITA =(JEITA*)get_nvkey_data(NVKEYID_MP_CAL_JEITA);
    if (!pJEITA)
    {
        vPortFree(pCHG_ADC);
        return;
    }
    JEITA_COOL* pJEITA_COOL =(JEITA_COOL*)get_nvkey_data(NVKEYID_MP_CAL_JEITA_COOL);
    if (!pJEITA_COOL)
    {
        vPortFree(pCHG_ADC);
        vPortFree(pJEITA);
        return;
    }

    pJEITA_COOL->JEITA_cool_cc2_threshold_voltage = pCHG_ADC->cc2_threshold_voltage + pCHG_ADC->cc2_offset_voltage;
    pJEITA_COOL->JEITA_cool_cv_threshold_voltage  = pCHG_ADC->cv_threshold_voltage+ pCHG_ADC->cv_thd_voltage_offset - pJEITA->Cool_State_DAC_decrease;
    pJEITA_COOL->JEITA_cool_full_bat_voltage      = pCHG_ADC->full_bat_voltage + pCHG_ADC->full_bat_voltage_offset - pJEITA->Cool_State_DAC_decrease;
    pJEITA_COOL->JEITA_cool_recharge_voltage      = pCHG_ADC->recharge_voltage+ pCHG_ADC->recharge_voltage_offset - pJEITA->Cool_State_DAC_decrease;
    pJEITA_COOL->JEITA_cool_full_bat_voltage_2    = pCHG_ADC->full_bat_voltage_2 + pCHG_ADC->full_bat_voltage_2_offset - pJEITA->Cool_State_DAC_decrease;

    pJEITA_COOL->JEITA_cool_cc2_threshold_ADC = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_COOL->JEITA_cool_cc2_threshold_voltage);
    pJEITA_COOL->JEITA_cool_cv_threshold_ADC  = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_COOL->JEITA_cool_cv_threshold_voltage);
    pJEITA_COOL->JEITA_cool_full_bat_ADC      = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_COOL->JEITA_cool_full_bat_voltage);
    pJEITA_COOL->JEITA_cool_recharge_ADC      = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_COOL->JEITA_cool_recharge_voltage);
    pJEITA_COOL->JEITA_cool_full_bat_ADC_2    = slope_calc(vbat_slope.volt1, vbat_slope.adc1, vbat_slope.volt2, vbat_slope.adc2, pJEITA_COOL->JEITA_cool_full_bat_voltage_2);

    pJEITA_COOL->JEITA_cool_cc2_threshold_ADC = range_protect(pJEITA_COOL->JEITA_cool_cc2_threshold_ADC, 0, 1023, 2);
    pJEITA_COOL->JEITA_cool_cv_threshold_ADC  = range_protect(pJEITA_COOL->JEITA_cool_cv_threshold_ADC, 0, 1023, 2);
    pJEITA_COOL->JEITA_cool_full_bat_ADC      = range_protect(pJEITA_COOL->JEITA_cool_full_bat_ADC, 0, 1023, 2);
    pJEITA_COOL->JEITA_cool_recharge_ADC      = range_protect(pJEITA_COOL->JEITA_cool_recharge_ADC, 0, 1023, 2);
    pJEITA_COOL->JEITA_cool_full_bat_ADC_2    = range_protect(pJEITA_COOL->JEITA_cool_full_bat_ADC_2, 0, 1023, 2);

    sel_volt = vbat_slope.volt1 - pJEITA->Cool_State_DAC_decrease;
    curr_dac = slope_calc(dac_slope.volt1, dac_slope.dac1, dac_slope.volt2, dac_slope.dac2, sel_volt);
    curr_dac = range_protect(curr_dac, 0, 1023, 2);

    pJEITA_COOL->JEITA_cool_cc1_current_DAC = curr_dac;
    pJEITA_COOL->JEITA_cool_cc2_current_DAC = curr_dac;
    pJEITA_COOL->JEITA_cool_cv_DAC          = curr_dac;

    pJEITA_COOL->JEITA_cool_cc1_current_1 = mpk_round((chg_cc_curr.cc1_curr * pJEITA->Cool_State_Current_Percent), 100);
    pJEITA_COOL->JEITA_cool_cc2_current_1 = mpk_round((chg_cc_curr.cc2_curr * pJEITA->Cool_State_Current_Percent), 100);

    cc1_rsel_4v2  = slope_calc(chg_curr_4v2_slope.curr1, chg_curr_4v2_slope.val1, chg_curr_4v2_slope.curr2, chg_curr_4v2_slope.val2, pJEITA_COOL->JEITA_cool_cc1_current_1);
    cc1_rsel_4v2  = range_protect(cc1_rsel_4v2, 0, 1023, 2);
    cc1_rsel_4v35 = slope_calc(chg_curr_4v35_slope.curr1, chg_curr_4v35_slope.val1, chg_curr_4v35_slope.curr2, chg_curr_4v35_slope.val2, pJEITA_COOL->JEITA_cool_cc1_current_1);
    cc1_rsel_4v35 = range_protect(cc1_rsel_4v35, 0, 1023, 2);
    pJEITA_COOL->JEITA_cool_rchg_sel_cc1_1 = slope_calc(dac_slope.volt1, cc1_rsel_4v35, dac_slope.volt2, cc1_rsel_4v2, sel_volt);
    pJEITA_COOL->JEITA_cool_rchg_sel_cc1_1 = range_protect(pJEITA_COOL->JEITA_cool_rchg_sel_cc1_1, 0, 1023, 2);

    cc2_rsel_4v2  = slope_calc(chg_curr_4v2_slope.curr1, chg_curr_4v2_slope.val1, chg_curr_4v2_slope.curr2, chg_curr_4v2_slope.val2, pJEITA_COOL->JEITA_cool_cc2_current_1);
    cc2_rsel_4v2  = range_protect(cc2_rsel_4v2, 0, 1023, 2);
    cc2_rsel_4v35 = slope_calc(chg_curr_4v35_slope.curr1, chg_curr_4v35_slope.val1, chg_curr_4v35_slope.curr2, chg_curr_4v35_slope.val2, pJEITA_COOL->JEITA_cool_cc2_current_1);
    cc2_rsel_4v35 = range_protect(cc2_rsel_4v35, 0, 1023, 2);
    pJEITA_COOL->JEITA_cool_rchg_sel_cc2_1 = slope_calc(dac_slope.volt1, cc2_rsel_4v35, dac_slope.volt2, cc2_rsel_4v2, sel_volt);
    pJEITA_COOL->JEITA_cool_rchg_sel_cc2_1 = range_protect(pJEITA_COOL->JEITA_cool_rchg_sel_cc2_1, 0, 1023, 2);

    cv_stop_curr_adc = slope_calc(dac_slope.volt1, chg_vichg_adc.ADC_4_35V, dac_slope.volt2, chg_vichg_adc.ADC_4_2V, sel_volt);
    cv_stop_curr_adc = range_protect(cv_stop_curr_adc, 0, 1023, 2);
    pJEITA_COOL->JEITA_cool_CV_stop_current_ADC = mpk_round((cv_stop_curr_adc * chg_vichg_adc.CV_stop_current_percent), 100);

    CHG_CV_STOP_CURRENT_ADC* pCHG_CV_STOP_CURRENT =(CHG_CV_STOP_CURRENT_ADC*)get_nvkey_data(NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC);
    if (pCHG_CV_STOP_CURRENT)
        pJEITA_COOL->JEITA_cool_CV_stop_current_ADC = mpk_round((pCHG_CV_STOP_CURRENT->CV_stop_current_ADC * 100), pJEITA->Cool_State_Current_Percent);
    log_hal_msgid_info("otp_cal_chg_jeita_cool, JEITA_cool_CV_stop_current_ADC[%d]", 1, pJEITA_COOL->JEITA_cool_CV_stop_current_ADC);

    nvkey_write_data(NVKEYID_MP_CAL_JEITA_COOL, (uint8_t *)pJEITA_COOL, sizeof(JEITA_COOL));

    vPortFree(pCHG_ADC);
    vPortFree(pJEITA);
    vPortFree(pJEITA_COOL);
    vPortFree(pCHG_CV_STOP_CURRENT);
}
#endif
void otp_cal_done(void)
{
    CHG_CONFIG chg_config;
    uint32_t size = sizeof(CHG_CONFIG);
    nvkey_status_t nvkey_status = NVKEY_STATUS_ERROR;

    nvkey_status = nvkey_read_data(NVKEYID_MP_CAL_CHG_CONFIG, (uint8_t*)&chg_config, &size);
    if (nvkey_status != NVKEY_STATUS_OK)
    {
        log_hal_msgid_error("otp_cal_vbat_config fail, chg_config nvkey_status[%d]", 1, nvkey_status);
        return;
    }
    if (chg_config.kflag != DEFAULT_TAG)
    {
        log_hal_msgid_error("otp_cal_vbat_config fail, chg_config.kflag[%d]", 1, chg_config.kflag);
        return;
    }
    chg_config.kflag = FT_FW_K;
    nvkey_write_data(NVKEYID_MP_CAL_CHG_CONFIG, (uint8_t*)&chg_config, sizeof(CHG_CONFIG));
}

void otp_calibration(void)
{
    if (otp_cal_vbat_config() != PMU_STATUS_SUCCESS)
        return;

    otp_cal_buck_mv();
    otp_cal_buck_mv_ret();
    otp_cal_buck_mv_freq();
    otp_cal_buck_lv();
    otp_cal_buck_lv_lpm();
    otp_cal_buck_lv_freq();

    otp_cal_ldo_vdd33_reg();
    otp_cal_ldo_vdd33_reg_ret();
    otp_cal_ldo_vdd33_ret();
    otp_cal_ldo_vrf_reg();
    otp_cal_ldo_vrf_reg_ret();
    otp_cal_ldo_vrf_ret();
    otp_cal_ldo_vdig18();

    otp_cal_hpbg();
    otp_cal_lpbg();

#ifndef AIR_PMU_DISABLE_CHARGER
    otp_cal_chg_dac();
    otp_cal_chg_current();
#endif
    otp_cal_chg_sys_ldo();//TBD
    otp_cal_chg_ocp();//TBD

#ifndef AIR_PMU_DISABLE_CHARGER
    otp_cal_chg_jeita_warm();
    otp_cal_chg_jeita_cool();
#endif

    otp_cal_done();
}

