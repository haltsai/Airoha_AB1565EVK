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

extern battery_managerment_control_info_t bm_ctrl_info;
#ifdef PMIC_AWAKE_TO_CHARGER
#include "hal_spm.h"
#endif
bool battery_set_enable_charger(bool isEnableCharging) {
       pmu_set_register_value_2byte_mt6388(PMU_RG_LOOP_CHRLDO_SB_DIS_ADDR, PMU_RG_LOOP_CHRLDO_SB_DIS_MASK, PMU_RG_LOOP_CHRLDO_SB_DIS_SHIFT, 0x80);
       pmu_set_register_value_2byte_mt6388(PMU_RG_BUSDPM_DELTA_VTH_ADDR, PMU_RG_BUSDPM_DELTA_VTH_MASK, PMU_RG_BUSDPM_DELTA_VTH_SHIFT, 0x1);
       pmu_set_register_value_2byte_mt6388(PMU_RG_SYSDPM_STATUS_SEL_ADDR, PMU_RG_SYSDPM_STATUS_SEL_MASK, PMU_RG_SYSDPM_STATUS_SEL_SHIFT, 0);
#ifdef PMIC_AWAKE_TO_CHARGER
    if(isEnableCharging) {
        spm_force_on_pmic(SPM_PMIC_REQUEST,SPM_ENABLE);
    } else {
        spm_force_on_pmic(SPM_PMIC_REQUEST,SPM_DISABLE);
    }
#endif
    return pmu_set_register_value_2byte_mt6388(PMU_RG_EN_CHR_ADDR, PMU_RG_EN_CHR_MASK, PMU_RG_EN_CHR_SHIFT, isEnableCharging);
}
/* [Battery Management HW JITE ] */

/* Enable the HW-JEITA hot cold control */
bool battery_set_hw_jeita_enable(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_EN_HWJEITA_ADDR, PMU_RG_EN_HWJEITA_MASK, PMU_RG_EN_HWJEITA_SHIFT, value);
}

/*
 * VCV voltage for JEITA WARM setting
 * 4'b0000: 4.05V(defaut)
 * 4'b0001: 4.10V
 * 4'b0010: 4.15V
 * 4'b0011: 4.20V
 * 4'b0100: 4.25V
 * 4'b0101: 4.30V
 * 4'b0110: 4.35V
 * 4'b0111: 4.40V
 * 4'b1000: 4.45V
 * 4'b1001: 4.50V
 * 4'b1010: 4.55V
 * 4'b1011: 4.60V
 * 4'b1100~4'b1111: 4.6V
*/
bool battery_set_jeita_warm_voltage(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_VCV_JW_ADDR, PMU_RG_VCV_JW_MASK, PMU_RG_VCV_JW_SHIFT, value);
}
/*
 * VCV voltage for JEITA COOL setting
 * 4'b0000: 4.05V(defaut)
 * 4'b0001: 4.10V
 * 4'b0010: 4.15V
 * 4'b0011: 4.20V
 * 4'b0100: 4.25V
 * 4'b0101: 4.30V
 * 4'b0110: 4.35V
 * 4'b0111: 4.40V
 * 4'b1000: 4.45V
 * 4'b1001: 4.50V
 * 4'b1010: 4.55V
 * 4'b1011: 4.60V
 * 4'b1100~4'b1111: 4.6V*/
bool battery_set_jeita_cool_voltage(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_VCV_JC_ADDR, PMU_RG_VCV_JC_MASK, PMU_RG_VCV_JC_SHIFT, value);
}

/*  IBAT level tune when jeita warm happened
 * 000~011: ICC 20%
 * 100: ICC 40%(Default)
 * 101: ICC 60%
 * 110: ICC 80%
 * 111: ICC 100%
 * */
bool battery_set_jeita_warm_icc_level(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_ICC_JW_ADDR, PMU_RG_ICC_JW_MASK, PMU_RG_ICC_JW_SHIFT, value);
}
/* IBAT level tune when jeita cool happened
 * 3'b000~011: ICC 20%
 * 3'b100: ICC 40%(Default)
 * 3'b101: ICC 60%
 * 3'b110: ICC 80%
 * 3'b111: ICC 100%
 * */
bool battery_set_jeita_cool_icc_level(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_ICC_JC_ADDR, PMU_RG_ICC_JC_MASK, PMU_RG_ICC_JC_SHIFT, value);
}

bool battery_set_jeita_irq_en(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_AUXADC_JEITA_IRQ_EN_ADDR, PMU_AUXADC_JEITA_IRQ_EN_MASK, PMU_AUXADC_JEITA_IRQ_EN_SHIFT, value);
}
/* Get HW JEITA stage
 *
 * HW_JEITA_HOT_STAGE 0xF
 * HW_JEITA_WARM_STAGE 0xE
 * HW_JEITA_NORMAL_STAGE 0xC
 * HW_JEITA_COOL_STAGE 0x8
 * HW_JEITA_COLD_STAGE 0
 *
 * */
uint8_t battery_get_hw_jeita_status(){
    uint8_t jeita_status=0;
    jeita_status |= (pmu_get_register_value_2byte_mt6388(PMU_AUXADC_JEITA_HOT_IRQ_ADDR, PMU_AUXADC_JEITA_HOT_IRQ_MASK, PMU_AUXADC_JEITA_HOT_IRQ_SHIFT) << 0);
    jeita_status |= (pmu_get_register_value_2byte_mt6388(PMU_AUXADC_JEITA_WARM_IRQ_ADDR, PMU_AUXADC_JEITA_WARM_IRQ_MASK, PMU_AUXADC_JEITA_WARM_IRQ_SHIFT) << 1);
    jeita_status |= (pmu_get_register_value_2byte_mt6388(PMU_AUXADC_JEITA_COOL_IRQ_ADDR, PMU_AUXADC_JEITA_COOL_IRQ_MASK, PMU_AUXADC_JEITA_COOL_IRQ_SHIFT) << 2);
    jeita_status |= (pmu_get_register_value_2byte_mt6388(PMU_AUXADC_JEITA_COLD_IRQ_ADDR, PMU_AUXADC_JEITA_COLD_IRQ_MASK, PMU_AUXADC_JEITA_COLD_IRQ_SHIFT) << 3);
    return jeita_status;
}

bool battery_set_jeita_cool_volt(short int value) {
    return pmu_set_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_COOL_ADDR, PMU_AUXADC_JEITA_VOLT_COOL_MASK, PMU_AUXADC_JEITA_VOLT_COOL_SHIFT, value);
}

bool battery_set_jeita_cold_volt(short int value) {
    return pmu_set_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_COLD_ADDR, PMU_AUXADC_JEITA_VOLT_COLD_MASK, PMU_AUXADC_JEITA_VOLT_COLD_SHIFT, value);
}

/*WARM/COOL flag will not referenced by LCHR which decided by RG_DISWARMCOOL (default 1).
 * Set RG_DISWARMCOOL=0 when the user needs to use JEITA/WARM COOL*/
bool battery_set_jeita_diswarmcool(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_DISWARMCOOL_ADDR, PMU_RG_DISWARMCOOL_MASK, PMU_RG_DISWARMCOOL_SHIFT, value);
}

//-------Battery Management Behavior

/* ICC mode current step up/down timing setting
 * 00: 16us
 * 01: 2ms
 * 10: 16ms (default)
 * 11: 128ms
*/
bool battery_set_icc_step_timing(uint8_t value) {
return pmu_set_register_value_2byte_mt6388(PMU_RG_TSTEP_ICC_ADDR,PMU_RG_TSTEP_ICC_MASK, PMU_RG_TSTEP_ICC_SHIFT, value);
}

/*set RG_IBAT_TUNE_TRIM_
 * 3'b000~011: ICC 20%
 * 3'b100: ICC 40%
 * 3'b101: ICC 60%
 * 3'b110: ICC 80%
 * 3'b111: ICC 100% (Default)*/
bool battery_set_ibat_tune_trim(uint8_t value)
{
    return pmu_set_register_value_2byte_mt6388(PMU_RG_IBAT_TUNE_TRIM_ADDR,PMU_RG_IBAT_TUNE_TRIM_MASK, PMU_RG_IBAT_TUNE_TRIM_SHIFT, value);
}

/*select vcv voltage
 * 4'b0000: 4.05V
 * 4'b0001: 4.10V
 * 4'b0010: 4.15V
 * 4'b0011: 4.20V (defaut)
 * 4'b0100: 4.25V
 * 4'b0101: 4.30V
 * 4'b0110: 4.35V
 * 4'b0111: 4.40V
 * 4'b1000: 4.45V
 * 4'b1001: 4.50V
 * 4'b1010: 4.55V
 * 4'b1011: 4.60V
 * 4'b1100~4'b1111: 4.6V*/
bool battery_select_vcv_voltage(uint8_t voltage) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_VCV_VTH_ADDR, PMU_RG_VCV_VTH_MASK, PMU_RG_VCV_VTH_SHIFT, voltage);
}

/*The charging state status of linear charger
 * 3'b000: CHR_OFF
 * 3'b001: PRECC
 * 3'b010: FASTCC
 * 3'b011: EXTENSION
 * 3'b100: EOC
 * 3'b101: THR
 * 3'b110: VBAT_OVP
 * 3'b111: PRECC or CC  SAFETY timer time out
 * */
uint32_t battery_get_charger_state(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_RGS_CHR_STATE_SW_ADDR,PMU_RGS_CHR_STATE_SW_MASK, PMU_RGS_CHR_STATE_SW_SHIFT);
}

/*CC safety timer selection, if change time out target, timer will be reset and re-count
 * 2'b00:3h
 * 2'b01:3h
 * 2'b10:4.5h
 * 2'b11:9h (default)
 */
bool battery_select_cc_safety_timer_domain(uint8_t timeMHrs) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_CC_TMR_SEL_ADDR, PMU_RG_CC_TMR_SEL_MASK, PMU_RG_CC_TMR_SEL_SHIFT, timeMHrs);
}

/* [Battery Management Charger] */

/*ICL current level control and soft start by digital (Typ. Current)
 * 4b0000: 10mA (for FT trim)
 * 4b0001: 90mA
 * 4'b0010: 200mA
 * 4'b0011: 300mA
 * 4'b0100: 400mA (Default)
 * 4'b0101: 500mA
 * 4b'0110: 600mA
 * 4'b0111: 700mA
 * 4b'1000: 800mA
 * 4b'1001: 900mA
 * 4'b1010: 1000mA */
bool battery_set_icl_curent_level(uint8_t currentLevel) {
    bm_ctrl_info.icl_curent = currentLevel;
    return pmu_set_register_value_2byte_mt6388(PMU_RG_ICL_ITH_ADDR, PMU_RG_ICL_ITH_MASK, PMU_RG_ICL_ITH_SHIFT, currentLevel);
}

/*iterm timing setting
 * 2'b00:dsiable iterm
 * 2'b01:15mins
 * 2'b10:30mins
 * 2'b11:60mins
*/
bool battery_set_extend_charger_time(uint8_t timeMins) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_T_ITERM_EXT_ADDR, PMU_RG_T_ITERM_EXT_MASK, PMU_RG_T_ITERM_EXT_SHIFT, timeMins);
}

bool battery_extend_timer_clear(void) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_EXTEND_TMR_CLR_ADDR, PMU_RG_EXTEND_TMR_CLR_MASK, PMU_RG_EXTEND_TMR_CLR_SHIFT, 0);
}

bool battery_set_en_recharge(bool isEnableRecharge) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_EN_RECHG_ADDR,PMU_RG_EN_RECHG_MASK, PMU_RG_EN_RECHG_SHIFT, isEnableRecharge);
}

/*
 * recharging voltage select
 * 2'b00:VCV-50mV
 * 2'b01:VCV-100mV(default)
 * 2'b10:VCV-150mV
 * 2'b11:VCV-200mV
 * */
bool battery_set_vrechg(uint8_t rechargeVoltage) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_VRECHG_ADDR, PMU_RG_VRECHG_MASK, PMU_RG_VRECHG_SHIFT, rechargeVoltage);
}


/*[Battery Management BJT]*/

//CC(FASTCC) safety timer enable
bool battery_enable_fastcc_safety_timer(bool isEnable) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_EN_SAFETY_TMR_CC_ADDR, PMU_RG_EN_SAFETY_TMR_CC_MASK,PMU_RG_EN_SAFETY_TMR_CC_SHIFT, isEnable);
}

/*
 * disable external BJT charging bit,will be reset when SYS_PWR_ON state
 * 1'b0: NA (BJT charging)
 * 1'b1: disable BJT charging
 * */
bool battery_bjt_enable_charging(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_DIS_BJT_ADDR, PMU_RG_DIS_BJT_MASK, PMU_RG_DIS_BJT_SHIFT, value);
}

/*
 * ICC_BJT soft start time step
 * 2'b00:0us
 * 2'b01:16us(default)
 * 2'b10:128us
 * 2'b11:256us
 * */
bool battery_set_soft_time_step(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_TSTEP_ICC_BJT_ADDR, PMU_RG_TSTEP_ICC_BJT_MASK, PMU_RG_TSTEP_ICC_BJT_SHIFT, value);
}

/*ICC_BJT current level control
 * 3'b001: 0.75A
 * 3'b010: 1.0A
 * 3'b011: 1.25A
 * 3'b100: 1.5A */
bool battery_bjt_set_icc_ith(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_ICC_BJT_ITH_ADDR, PMU_RG_ICC_BJT_ITH_MASK, PMU_RG_ICC_BJT_ITH_SHIFT, value);
}

/*for BJT charging stability*/
bool battery_bjt_set_bias_icc(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_LOOP_GM_BIAS_ICC_ADDR, PMU_RG_LOOP_GM_BIAS_ICC_MASK, PMU_RG_LOOP_GM_BIAS_ICC_SHIFT, value);
}

/*
 * enable external BJT charging,will be reset when SYS_PWR_ON state
 * 1'b0: CHRLDO charging
 * 1'b1: external BJT charging
*/
bool battery_enable_external_bjt_charging(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_CHR2BJT_EN_ADDR, PMU_RG_CHR2BJT_EN_MASK, PMU_RG_CHR2BJT_EN_SHIFT, value);
}

/*setting charger multiple level
 *level is 1~5,
 *ref : battery_managerment_charger_ITH_t
 **/
void battery_multiple_level(uint8_t currentLevelTarget){
    battery_config_fast_charger_setting(TSTEP_ICC_MULTIPLE_TIME_16uS, currentLevelTarget);
}

void battery_config_fast_charger_setting(uint8_t softStartTime, uint8_t currentLevelTarget) {
    pmu_set_register_value_2byte_mt6388(PMU_RG_EN_ICC_MULTIPLE_SFSTR_ADDR, PMU_RG_EN_ICC_MULTIPLE_SFSTR_MASK, PMU_RG_EN_ICC_MULTIPLE_SFSTR_SHIFT, 1);
    pmu_set_register_value_2byte_mt6388(PMU_RG_TSTEP_ICC_MULTIPLE_ADDR, PMU_RG_TSTEP_ICC_MULTIPLE_MASK, PMU_RG_TSTEP_ICC_MULTIPLE_SHIFT, softStartTime);
    if (currentLevelTarget >= ICC_MULTIPLE_ITH_5C) {
        currentLevelTarget = ICC_MULTIPLE_ITH_5C;
    }
    pmu_set_register_value_2byte_mt6388(PMU_RG_ICC_MULTIPLE_ITH_ADDR, PMU_RG_ICC_MULTIPLE_ITH_MASK, PMU_RG_ICC_MULTIPLE_ITH_SHIFT, currentLevelTarget);
    bm_ctrl_info.charger_5c = currentLevelTarget;
}

uint8_t battery_get_multi_level(void){
    return pmu_get_register_value_2byte_mt6388(PMU_RG_ICC_MULTIPLE_ITH_ADDR,PMU_RG_ICC_MULTIPLE_ITH_MASK, PMU_RG_ICC_MULTIPLE_ITH_SHIFT);
}

/* [ Battery Management setting API ]*/

/*
 * Reduce IBUS
 * 0: No reduce
 * 1: disable PREG, CP, and SYS LDO
*/
bool battery_set_ibus_low_en(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_LOW_IBUS_EN_ADDR, PMU_RG_LOW_IBUS_EN_MASK, PMU_RG_LOW_IBUS_EN_SHIFT, value);
}

/* set SRCLKEN status*/
bool battery_set_rg_psi1b(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_PSI1B_VAL_ADDR, PMU_PSI1B_VAL_MASK, PMU_PSI1B_VAL_SHIFT, value);
}

/*STRUP_AUXADC_START selection
 * 1'b0: HW mode
 * 1'b1: SW mode*/
bool battery_selet_auxadc_strup(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_STRUP_AUXADC_START_SEL_ADDR, PMU_RG_STRUP_AUXADC_START_SEL_MASK, PMU_RG_STRUP_AUXADC_START_SEL_SHIFT, value);
}
/*STRUP_AUXADC_START SW path
 * 1'b0 : AUXADC_START sw =0
 * 1'b1 : AUXADC_START sw =1*/
bool battery_selet_auxadc_start_sw_path(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_STRUP_AUXADC_START_SW_ADDR, PMU_RG_STRUP_AUXADC_START_SW_MASK, PMU_RG_STRUP_AUXADC_START_SW_SHIFT, value);
}

/*check Reserved efuse bit*/
uint32_t battery_check_reserved_efuse_trim(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_RG_AO_EFUSE_RSV_ADDR, PMU_RG_AO_EFUSE_RSV_MASK,PMU_RG_AO_EFUSE_RSV_SHIFT);
}

/*Enable the digital thermal regulation for ICC*/
bool battery_enable_digital_thermal(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_EN_DIG_THR_ADDR, PMU_RG_EN_DIG_THR_MASK, PMU_RG_EN_DIG_THR_SHIFT, value);
}

/*
 * PRECC % of ICC
 * 00: 5%
 * 01: 10% (Default)
 * 10: 20%
 * 11: 40%
*/
bool battery_set_precc_ith(uint8_t percentageOfICC) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_PRECC_ITH_ADDR, PMU_RG_PRECC_ITH_MASK, PMU_RG_PRECC_ITH_SHIFT, percentageOfICC);
}

/*
 * ITEM % of ICC
 * 00: 5%
 * 01: 10% (Default)
 * 10: 20%
 * 11: 40%
*/
bool battery_set_iterm_ith(uint8_t percentageOfICC) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_ITERM_ITH_ADDR, PMU_RG_ITERM_ITH_MASK, PMU_RG_ITERM_ITH_SHIFT, percentageOfICC);
}

/*1: Bypass Vbuf.
 *0: turn on Vbuf.
 * */
void battery_core_auxadc_thermal_sensor_buffer_on(uint8_t value) {
    pmu_set_register_value_2byte_mt6388(PMU_RG_VBUF_BYP_ADDR, PMU_RG_VBUF_BYP_MASK, PMU_RG_VBUF_BYP_SHIFT, value);
}

bool battery_set_discharger_en_sw(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_SYS_DISCHR_EN_SW_ADDR,PMU_RG_SYS_DISCHR_EN_SW_MASK, PMU_RG_SYS_DISCHR_EN_SW_SHIFT, value);
}

bool battery_set_discharger_en_sw_sel(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_SYS_DISCHR_EN_SW_SEL_ADDR,PMU_RG_SYS_DISCHR_EN_SW_SEL_MASK, PMU_RG_SYS_DISCHR_EN_SW_SEL_SHIFT, value);
}

/*charger suspend mode control */
bool battery_charger_suspend_mode(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_CHR_SUSPEND_MODE_ADDR, PMU_RG_CHR_SUSPEND_MODE_MASK, PMU_RG_CHR_SUSPEND_MODE_SHIFT, value);
}

uint32_t battery_check_vbus_gt_por(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_AD_QI_VBUS_GT_POR_ADDR, PMU_AD_QI_VBUS_GT_POR_MASK, PMU_AD_QI_VBUS_GT_POR_SHIFT);
}

bool battery_set_plugin_db_sw_sel(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_CHR_PLUGIN_DB_SW_SEL_ADDR, PMU_RG_CHR_PLUGIN_DB_SW_SEL_MASK, PMU_RG_CHR_PLUGIN_DB_SW_SEL_SHIFT, value);
}

uint8_t battery_check_charger_ref_en(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_DA_QI_CHR_REF_EN_ADDR, PMU_DA_QI_CHR_REF_EN_MASK, PMU_DA_QI_CHR_REF_EN_SHIFT);
}

/*Recharge low battery detection voltage detection enable
 * 0:disable
 * 1:enable */
bool battery_control_recharge_low_bat(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_AUXADC_VBAT_EN_ADC_RECHG_ADDR, PMU_AUXADC_VBAT_EN_ADC_RECHG_MASK, PMU_AUXADC_VBAT_EN_ADC_RECHG_SHIFT, value);
}

/*Recharge mode in low battery detection voltage detection enable*/
bool battery_control_vbat_en_mode_sel(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_AUXADC_VBAT_EN_MODE_SEL_ADDR, PMU_AUXADC_VBAT_EN_MODE_SEL_MASK, PMU_AUXADC_VBAT_EN_MODE_SEL_SHIFT, value);
}

/*STRUP_AUXADC_RSTB selection*/
bool battery_rstb_select(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_STRUP_AUXADC_RSTB_SEL_ADDR, PMU_RG_STRUP_AUXADC_RSTB_SEL_MASK, PMU_RG_STRUP_AUXADC_RSTB_SEL_SHIFT, value);
}

bool battery_set_vbat_rechg_mask(uint8_t oper) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_INT_MASK_VBAT_RECHG_ADDR, PMU_RG_INT_MASK_VBAT_RECHG_MASK, PMU_RG_INT_MASK_VBAT_RECHG_SHIFT,oper);
}

bool battery_set_vbat_irq_en(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_AUXADC_VBAT_IRQ_EN_ADDR, PMU_AUXADC_VBAT_IRQ_EN_MASK, PMU_AUXADC_VBAT_IRQ_EN_SHIFT, value);
}

/*SYSLDO enable bit*/
bool battery_enable_sysldo(bool isEnableSysdo) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_SYSLDO_EN_ADDR, PMU_RG_SYSLDO_EN_MASK, PMU_RG_SYSLDO_EN_SHIFT, isEnableSysdo);
}

/*clear AUXADC_ADC_RDY_WAKEUP_PCHR & AUXADC_ADC_RDY_WAKEUP_SWCHR
 * 1'b0: none
 * 1'b1: clear*/
bool battery_auxadc_wakeup_clr(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_AUXADC_ADC_RDY_WAKEUP_CLR_ADDR,PMU_AUXADC_ADC_RDY_WAKEUP_CLR_MASK, PMU_AUXADC_ADC_RDY_WAKEUP_CLR_SHIFT, value);
}
/*auxadc start delay cycle(32k), default 1.5ms*/
bool battery_auxadc_set_delay_cycle_timex(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_STRUP_AUXADC_RPCNT_MAX_ADDR,PMU_RG_STRUP_AUXADC_RPCNT_MAX_MASK, PMU_RG_STRUP_AUXADC_RPCNT_MAX_SHIFT, value);
}

/*check HW DD_QI_FASTON_FLAG_DB*/
uint32_t battery_get_faston_flag(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_DD_QI_FASTON_FLAG_DB_ADDR, PMU_DD_QI_FASTON_FLAG_DB_MASK, PMU_DD_QI_FASTON_FLAG_DB_SHIFT);
}
/*check HW DA_QI_VBUS_OVP_DB*/
uint32_t battery_get_vbus_ovp_db(void)
{
    return  pmu_get_register_value_2byte_mt6388(PMU_DA_QI_VBUS_OVP_DB_ADDR,PMU_DA_QI_VBUS_OVP_DB_MASK, PMU_DA_QI_VBUS_OVP_DB_SHIFT);
}
/*check HW AD_QI_FASTON_FLAG*/
uint32_t battery_core_pmu_get_ad_qi_faston_flag(void)
{
    return pmu_get_register_value_2byte_mt6388(PMU_AD_QI_FASTON_FLAG_ADDR,
        PMU_AD_QI_FASTON_FLAG_MASK, PMU_AD_QI_FASTON_FLAG_SHIFT);
}

/*check HW DA_QI_VBUS_UVLO_DB*/
uint32_t battery_get_vbus_uvlo_db(void)
{
    return  pmu_get_register_value_2byte_mt6388(PMU_DA_QI_VBUS_UVLO_DB_ADDR,PMU_DA_QI_VBUS_UVLO_DB_MASK, PMU_DA_QI_VBUS_UVLO_DB_SHIFT);
}

bool battery_enable_vbat_rechg(uint8_t oper) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_INT_EN_VBAT_RECHG_ADDR, PMU_RG_INT_EN_VBAT_RECHG_MASK, PMU_RG_INT_EN_VBAT_RECHG_SHIFT,
            oper);
}

/* [ Battery Management debug ] */

void battery_core_pmu_turbo_the_timer(uint8_t isEnable) { //LCHR 1S HIT Speed up for timer (for debug)
    if (isEnable) {
        battery_control_pmic_protect(TST_W_KEY_SW_MODE);
        pmu_set_register_value_2byte_mt6388(PMU_RG_LCHR_1S_SPEEDUP_ADDR, PMU_RG_LCHR_1S_SPEEDUP_MASK, PMU_RG_LCHR_1S_SPEEDUP_SHIFT, 1);
    } else {
        battery_control_pmic_protect(TST_W_KEY_HW_MODE);
        pmu_set_register_value_2byte_mt6388(PMU_RG_LCHR_1S_SPEEDUP_ADDR, PMU_RG_LCHR_1S_SPEEDUP_MASK, PMU_RG_LCHR_1S_SPEEDUP_SHIFT, 0);
    }
}


uint8_t battery_get_icl_curent_level(void)
{
    return pmu_get_register_value_2byte_mt6388(PMU_RG_ICL_ITH_ADDR, PMU_RG_ICL_ITH_MASK,PMU_RG_ICL_ITH_SHIFT);
}

uint32_t battery_get_soft_time_step(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_RG_TSTEP_ICC_BJT_ADDR, PMU_RG_TSTEP_ICC_BJT_MASK, PMU_RG_TSTEP_ICC_BJT_SHIFT);
}

/*ICC_BJT current level control
 * 3'b001: 0.75A
 * 3'b010: 1.0A
 * 3'b011: 1.25A
 * 3'b100: 1.5A */
uint32_t battery_bjt_get_icc_ith(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_RG_ICC_BJT_ITH_ADDR, PMU_RG_ICC_BJT_ITH_MASK, PMU_RG_ICC_BJT_ITH_SHIFT);
}

uint32_t battery_bjt_get_bias_icc(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_RG_LOOP_GM_BIAS_ICC_ADDR,PMU_RG_LOOP_GM_BIAS_ICC_MASK, PMU_RG_LOOP_GM_BIAS_ICC_SHIFT);
}

/*Check BJT Mode is working*/
uint8_t battery_check_bjt_mode(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_DA_QI_BJT_MODE_EN_ADDR,PMU_DA_QI_BJT_MODE_EN_MASK, PMU_DA_QI_BJT_MODE_EN_SHIFT);
}

uint8_t battery_get_extend_charger_time(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_RG_T_ITERM_EXT_ADDR, PMU_RG_T_ITERM_EXT_MASK, PMU_RG_T_ITERM_EXT_SHIFT);
}

/*
 * Rduce Iq at normal mode without BUS Plug-in
 * 0: No reduce Iq
 * 1: Disable BATOC and FastOn Comp to reduce Iq
* */
bool battery_set_ppfet_lp_mode(uint8_t value) {
    return pmu_set_register_value_2byte_mt6388(PMU_RG_PPFET_CTRL_LP_MODE_ADDR, PMU_RG_PPFET_CTRL_LP_MODE_MASK, PMU_RG_PPFET_CTRL_LP_MODE_SHIFT, value);
}

void battery_control_pmic_protect(uint8_t tstWKeymode) {
    switch (tstWKeymode)
    {
        case TST_W_KEY_HW_MODE:
            pmu_set_register_value_2byte_mt6388(PMU_TST_W_KEY_ADDR, PMU_TST_W_KEY_MASK,PMU_TST_W_KEY_SHIFT, 0);
            break;
        case TST_W_KEY_SW_MODE:
            pmu_set_register_value_2byte_mt6388(PMU_TST_W_KEY_ADDR, PMU_TST_W_KEY_MASK,PMU_TST_W_KEY_SHIFT, 0x4936);
            break;
        default:
            break;
    }
}
 /*Force disable VSYS discharge*/
uint32_t battery_disable_vsys_discharge(uint8_t value){
    return pmu_set_register_value_2byte_mt6388(PMU_RG_EFUSE_PPFET_FON_RSV_TRIM_ADDR, PMU_RG_EFUSE_PPFET_FON_RSV_TRIM_MASK, PMU_RG_EFUSE_PPFET_FON_RSV_TRIM_SHIFT, value);
}

bool battery_get_chr_detect_value(void) {
    return pmu_get_register_value_2byte_mt6388(PMU_DA_QI_CHR_REF_EN_ADDR, PMU_DA_QI_CHR_REF_EN_MASK, PMU_DA_QI_CHR_REF_EN_SHIFT);
}

void battery_get_ovp_reason(void) {
    LOG_MSGID_I(MPLOG, " Vbat OVP State 744[%x]746[%x]\r\n",pmu_get_register_value_2byte_mt6388(PMU_LCHR_DIG_DEBUG0, 0xffff,0)
	                                                       ,pmu_get_register_value_2byte_mt6388(PMU_LCHR_DIG_DEBUG1, 0xffff,0));
}
void battery_check_charger_power(void){
    uint32_t mask_pri;
    hal_nvic_save_and_set_interrupt_mask(&mask_pri);
    if((pmu_get_power_status_6388(PMU_LDO_VLDO33)!=PMU_ON ) || (pmu_get_power_status_6388(PMU_LDO_VA18)!=PMU_ON )){
        pmu_power_enable_6388(PMU_LDO_VA18, PMU_ON);
        pmu_power_enable_6388(PMU_LDO_VLDO33, PMU_ON);
    }
    pmu_lock_va18(PMU_ON);
    hal_nvic_restore_interrupt_mask(mask_pri);
}
/*Only when bootup_from vbus*/
void battery_clear_charger_irq(void){
    pmu_control_mask_interrupt(RG_INT_CHRDET,1);
    pmu_control_enable_interrupt(RG_INT_CHRDET, 0);
    pmu_control_clear_interrupt(RG_INT_CHRDET);
    pmu_control_enable_interrupt(RG_INT_CHRDET, 1);
    pmu_control_mask_interrupt(RG_INT_CHRDET, 0);

    pmu_control_mask_interrupt(RG_INT_ChgStatInt, 1);
    pmu_control_enable_interrupt(RG_INT_ChgStatInt, 0);
    pmu_control_clear_interrupt(RG_INT_ChgStatInt);
    pmu_control_enable_interrupt(RG_INT_ChgStatInt, 1);
    pmu_control_mask_interrupt(RG_INT_ChgStatInt, 0);
}
uint32_t battery_set_pmic_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value){
    pmu_set_register_value_2byte_mt6388(address, mask, shift, value);
    return pmu_get_register_value_2byte_mt6388(address, mask,shift);
}
uint32_t battery_get_pmic_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift){
    return pmu_get_register_value_2byte_mt6388(address, mask,shift);
}
void battery_charger_check_faston(void){
    if(pmu_get_register_value_2byte_mt6388(0x746, 0x1,15)==1){
        LOG_MSGID_I(battery_management, "FAST_ON", 0);
        battery_enable_sysldo(0);
        hal_gpt_delay_ms(300);
        battery_enable_sysldo(1);
    }
}
