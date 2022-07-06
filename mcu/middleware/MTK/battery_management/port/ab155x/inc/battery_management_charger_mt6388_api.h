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

#ifndef __BATTERY_MANAGEMENT_CHARGER_MT6388_API_H__
#define __BATTERY_MANAGEMENT_CHARGER_MT6388_API_H__

enum
{
    TST_W_KEY_HW_MODE,
    TST_W_KEY_SW_MODE,
};

/*******************************************************************************
Set Register API
********************************************************************************/
bool battery_set_icl_curent_level(uint8_t currentLevel);
uint8_t battery_get_icl_curent_level(void);
void battery_control_pmic_protect(uint8_t);
bool battery_selet_auxadc_strup(uint8_t value);

bool battery_selet_auxadc_start_sw_path(uint8_t value);
bool battery_set_rg_psi1b(uint8_t value);

uint32_t battery_check_reserved_efuse_trim(void);
bool battery_select_vcv_voltage(uint8_t voltage);
bool battery_set_ibat_tune_trim(uint8_t value);
bool battery_set_jeita_warm_voltage(uint8_t value);
bool battery_set_jeita_warm_icc_level(uint8_t value);
bool battery_set_jeita_cool_icc_level(uint8_t value);
bool battery_set_jeita_cool_voltage(uint8_t value);
bool battery_set_hw_jeita_enable(uint8_t value);
bool battery_set_icc_step_timing(uint8_t value);
bool battery_set_jeita_diswarmcool(uint8_t value);
bool battery_set_jeita_irq_en(uint8_t value);
bool battery_set_jeita_cool_volt(short int value);
bool battery_enable_digital_thermal(uint8_t value);

bool battery_set_jeita_cold_volt(short int value);

//------Charger stage
bool battery_set_precc_ith(uint8_t percentageOfICC);
bool battery_set_iterm_ith(uint8_t percentageOfICC);
uint32_t battery_get_charger_state(void);

//-----BJT
bool battery_enable_fastcc_safety_timer(bool isEnable);
bool battery_select_cc_safety_timer_domain(uint8_t timeMHrs);
bool battery_switch_on_bjt_charger(uint8_t value);
bool battery_set_soft_time_step(uint8_t value);
uint32_t battery_get_soft_time_step(void);
bool battery_bjt_set_icc_ith(uint8_t value);
uint32_t battery_bjt_get_icc_ith(void);
bool battery_bjt_set_bias_icc(uint8_t value);
uint32_t battery_bjt_get_bias_icc(void);
bool battery_enable_external_bjt_charging(uint8_t value);
uint8_t battery_check_bjt_mode(void);
void battery_config_fast_charger_setting(uint8_t softStartTime,uint8_t currentLevelTarget);
uint8_t battery_get_multi_level(void);
//----auxadc
void battery_core_auxadc_thermal_sensor_buffer_on(uint8_t value);

//-----extend charging
bool battery_set_extend_charger_time(uint8_t timeMins);
uint8_t battery_get_extend_charger_time(void);
bool battery_extend_timer_clear(void);

//--------charger---
bool battery_set_vrechg(uint8_t rechargeVoltage);
bool battery_set_enable_charger(bool isEnableCharging);
bool battery_set_discharger_en_sw(uint8_t value);
bool battery_charger_suspend_mode(uint8_t value);
bool battery_set_ppfet_lp_mode(uint8_t value);
bool battery_set_ibus_low_en(uint8_t value);
uint32_t battery_check_vbus_gt_por(void);
bool battery_set_plugin_db_sw_sel(uint8_t value);
uint8_t battery_check_charger_ref_en(void);
bool battery_control_recharge_low_bat(uint8_t value);
bool battery_control_vbat_en_mode_sel(uint8_t value);
bool battery_rstb_select(uint8_t value);
bool battery_enable_vbat_rechg(uint8_t oper);
bool battery_set_vbat_rechg_mask(uint8_t oper);
bool battery_set_vbat_irq_en(uint8_t value);
bool battery_enable_sysldo(bool isEnableSysdo);
bool battery_auxadc_wakeup_clr(uint8_t value);
bool battery_auxadc_set_delay_cycle_timex(uint8_t value);
uint32_t battery_get_faston_flag(void);
uint32_t battery_get_vbus_ovp_db(void);
uint32_t battery_get_vbus_uvlo_db(void);
void battery_core_pmu_turbo_the_timer(uint8_t isEnable);
void battery_multiple_level(uint8_t currentLevelTarget);
bool battery_get_chr_detect_value(void);
bool battery_set_discharger_en_sw_sel(uint8_t value);
uint32_t battery_disable_vsys_discharge(uint8_t value);
void battery_check_charger_power(void);
void battery_clear_charger_irq(void);
void battery_charger_check_faston(void);
void battery_get_ovp_reason(void);
//---------------------------------------------------------
uint32_t battery_set_pmic_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value);
uint32_t battery_get_pmic_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift);
#endif // __BATTERY_MANAGEMENT_CHARGER_MT6388_API_H__




