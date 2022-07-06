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

#include "hal.h"

#ifdef HAL_PMU_MODULE_ENABLED

#ifdef AB1568
#include "hal_pmu_wrap_interface.h"
#include "hal_pmu_auxadc_2568.h"
#include "hal_pmu_charger_2568.h"
#include "hal_pmu_internal_2568.h"

pmu_ab2568_wrap_api_struct hal_pmu_st;
/*==========[Basic function]==========*/
void pmu_init(void) {
    hal_pmu_st.init();
}
pmu_power_vcore_voltage_t pmu_lock_vcore(pmu_power_stage_t mode,pmu_power_vcore_voltage_t vol,pmu_lock_parameter_t lock) {
    return hal_pmu_st.lock_vcore(mode,vol,lock);
}
pmu_operate_status_t pmu_set_register_value(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value) {
    return hal_pmu_st.set_register_value(address,mask,shift,value);
}
uint32_t pmu_get_register_value(uint32_t address, uint32_t mask, uint32_t shift) {
    return hal_pmu_st.get_register_value(address,mask,shift);
}
bool pmu_get_chr_detect_value(void){
    return hal_pmu_st.get_chr_detect_value();
}
void pmu_enable_sw_lp_mode(pmu_power_domain_t domain, pmu_control_mode_t mode){
    hal_pmu_st.enable_sw_lp_mode(domain,mode);
}
void pmu_switch_control_mode(pmu_power_domain_t domain, pmu_control_mode_t mode){
    hal_pmu_st.switch_control_mode(domain,mode);
}
void pmu_lock_va18(int oper){
    hal_pmu_st.lock_va18(oper);
}
void pmu_charger_check_faston(void){
    hal_pmu_st.charger_check_faston();
}
void pmu_enable_ocp(pmu_power_operate_t oper){
    hal_pmu_st.enable_ocp(oper);
}
void pmu_enable_lpsd(void){
    hal_pmu_st.enable_lpsd();
}
/*==========[BUCK/LDO]==========*/

void pmu_enable_power(pmu_power_domain_t pmu_pdm, pmu_power_operate_t operate){
    hal_pmu_st.enable_power(pmu_pdm,operate);
}
uint32_t pmu_get_power_status(pmu_power_domain_t pmu_pdm) {
    return hal_pmu_st.get_power_status(pmu_pdm);
}
pmu_power_vcore_voltage_t pmu_get_vcore_voltage(void) {
    return hal_pmu_st.get_vcore_voltage();
}
pmu_operate_status_t pmu_select_vsram_voltage(pmu_power_stage_t mode, pmu_power_vsram_voltage_t vol){
    return hal_pmu_st.select_vsram_voltage(mode,vol);
}

pmu_operate_status_t pmu_select_vcore_voltage(pmu_power_stage_t mode, pmu_power_vcore_voltage_t vol){
    return hal_pmu_st.select_vcore_voltage(mode,vol);
}
/*==========[Charger]==========*/
void pmu_charger_init(uint16_t precc_cur,uint16_t cv_termination){
    hal_pmu_st.charger_init(precc_cur,cv_termination);
}
uint8_t pmu_enable_charger(uint8_t isEnableCharging){
    return hal_pmu_st.enable_charger(isEnableCharging);
}
bool pmu_set_icl_curent_level(uint8_t currentLevel){
    return hal_pmu_st.set_icl_curent_level(currentLevel);
}
uint8_t pmu_get_bc12_charger_type(void) {
    return hal_pmu_st.get_bc12_charger_type();
}
void pmu_set_charger_current_limit(uint8_t port) {
    hal_pmu_st.set_charger_current_limit(port);
}
void pmu_select_eco_option_operating(pmu_eoc_option_t opt,pmu_eoc_operating_t oper) {
    hal_pmu_st.select_eco_option_operating(opt,oper);
}
uint32_t pmu_get_charger_state(void){
    return hal_pmu_st.get_charger_state();
}

void pmu_set_extend_charger_time(uint8_t timeMins){
    hal_pmu_st.set_extend_charger_time(timeMins);
}

void pmu_enable_recharger(bool isEnableRecharge){
    hal_pmu_st.enable_recharger(isEnableRecharge);
}
bool pmu_set_rechg_voltage(uint8_t isEnableRecharge){
    return hal_pmu_st.set_rechg_voltage(isEnableRecharge);
}
void pmu_set_pre_charger_current(pmu_fastcc_chrcur_t cur){
    hal_pmu_st.set_pre_charger_current(cur);
}

void pmu_set_charger_current(pmu_fastcc_chrcur_t cur){
    hal_pmu_st.set_charger_current(cur);
}

void pmu_set_iterm_current_irq(pmu_iterm_chrcur_t cur){
    hal_pmu_st.set_iterm_current_irq(cur);
}

void pmu_set_iterm_current(pmu_iterm_chrcur_t cur){
    hal_pmu_st.set_iterm_current(cur);
}

bool pmu_select_cv_voltage(uint8_t voltage){
    return hal_pmu_st.select_cv_voltage(voltage);
}

bool pmu_select_cc_safety_timer(uint8_t timeMHrs){
    return hal_pmu_st.select_cc_safety_timer(timeMHrs);
}
uint32_t pmu_disable_vsys_discharge(uint8_t value){
    return hal_pmu_st.disable_vsys_discharge(value);
}

uint8_t pmu_get_usb_input_status(void){
    return hal_pmu_st.get_usb_input_status();
}

/*==========[HW-JEITA/NTC]==========*/

void pmu_hw_jeita_init(void) {
    hal_pmu_st.hw_jeita_init();
}

uint8_t pmu_get_hw_jeita_status(void){
    return hal_pmu_st.get_hw_jeita_status();
}

pmu_operate_status_t pmu_set_jeita_voltage(uint32_t auxadcVolt, uint8_t JeitaThreshold){
    return hal_pmu_st.set_jeita_voltage(auxadcVolt,JeitaThreshold);
}

void pmu_jeita_state_setting(uint8_t state,pmu_jc_perecnt_level_t ICC_JC,pmu_cv_voltage_t vol) {
    hal_pmu_st.jeita_state_setting(state,ICC_JC,vol);
}

void pmu_thermal_parameter_init(void) {
    hal_pmu_st.thermal_parameter_init();
}

uint32_t pmu_auxadc_get_pmic_temperature(void) {
    return hal_pmu_st.get_pmic_temperature();
}
bool pmu_set_hw_jeita_enable(uint8_t value){
    return hal_pmu_st.set_hw_jeita_enable(value);
}

void pmu_set_audio_enhance(pmu_power_operate_t oper){
    hal_pmu_st.set_audio_enhance(oper);
}

void pmu_get_lock_status(int sta){
    hal_pmu_st.get_lock_status(sta);
}

bool pmu_get_pwrkey_state(void){
    return hal_pmu_st.get_pwrkey_state();
}

void pmu_power_off_sequence(pmu_power_stage_t stage){
    hal_pmu_st.power_off_sequence(stage);
}

uint8_t pmu_get_power_on_reason(void) {
    return hal_pmu_st.get_power_on_reason();
}

uint8_t pmu_get_power_off_reason(void) {
    return hal_pmu_st.get_power_off_reason();
}

pmu_operate_status_t pmu_pwrkey_enable(pmu_power_operate_t oper) {
    return hal_pmu_st.pwrkey_enable(oper);
}

bool pmu_select_precc_voltage(uint8_t voltage) {
    return hal_pmu_st.select_precc_voltage(voltage);
}

void pmu_set_vaud18_voltage(pmu_power_vaud18_voltage_t oper){
    hal_pmu_st.set_vaud18_voltage(oper);
}

pmu_ab2568_wrap_api_struct hal_pmu_st = {
        pmu_init_2568,
        pmu_set_register_value_ab2568,
        pmu_get_register_value_ab2568,
        pmu_enable_power_2568,
        pmu_get_power_status_2568,
        pmu_get_vcore_voltage_ab2568,
        pmu_lock_vcore_ab2568,
        pmu_charger_init_2568,
        pmu_enable_charger_2568,
        pmu_hw_jeita_init_2568,
        pmu_get_hw_jeita_status_2568,
        pmu_set_jeita_voltage_2568,
        pmu_set_jeita_state_setting_2568,
        pmu_get_bc12_charger_type_2568,
        pmu_set_charger_current_limit_2568,
        pmu_thermal_parameter_init_2568,
        pmu_get_auxadc_pmic_temperature_2568,
        pmu_select_eco_option_operating_2568,
        pmu_set_icl_curent_level_2568,
        pmu_get_charger_state_2568,
        pmu_set_extend_charger_time_2568,
        pmu_get_chr_detect_value_2568,
        pmu_enable_recharger_2568,
        pmu_set_rechg_voltage_2568,
        pmu_set_hw_jeita_enable_2568,
        pmu_disable_vsys_discharge_2568,
        pmu_lock_va18_2568,
        pmu_charger_check_faston_2568,
        pmu_select_cv_voltage_2568,
        pmu_select_cc_safety_timer_2568,
        pmu_enable_sw_lp_mode_2568,
        pmu_switch_control_mode_2568,
        pmu_set_charger_current_2568,
        pmu_set_iterm_current_irq_2568,
        pmu_select_vsram_voltage_ab2568,
        pmu_select_vcore_voltage_ab2568,
        pmu_get_usb_input_status_2568,
        pmu_set_audio_enhance_2568,
        pmu_set_pre_charger_current_2568,
        pmu_set_iterm_current_2568,
        pmu_enable_ocp_2568,
        pmu_enable_lpsd_2568,
        pmu_get_lock_status_2568,
        pmu_get_pwrkey_state_2568,
        pmu_power_off_sequence_2568,
        pmu_get_power_on_reason_2568,
        pmu_get_power_off_reason_2568,
        pmu_pwrkey_enable_2568,
        pmu_select_precc_voltage_2568,
        pmu_set_vaud18_voltage_2568,
};
#else
#include "hal_pmu_wrap_interface.h"
pmu_ab2565_wrap_api_struct hal_pmu_st;
/*==========[Basic function]==========*/
void pmu_init(void) {
    hal_pmu_st.init();
}
pmu_power_vcore_voltage_t pmu_lock_vcore(pmu_power_stage_t mode,pmu_power_vcore_voltage_t vol,pmu_lock_parameter_t lock) {
    return hal_pmu_st.lock_vcore(mode,vol,lock);
}
pmu_operate_status_t pmu_set_register_value(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value) {
    return hal_pmu_st.set_register_value(address,mask,shift,value);
}
pmu_operate_status_t pmu_force_set_register_value(uint32_t address, uint32_t value) {
    return hal_pmu_st.force_set_register_value(address,value);
}
uint32_t pmu_get_register_value(uint32_t address, uint32_t mask, uint32_t shift) {
    return hal_pmu_st.get_register_value(address,mask,shift);
}
void pmu_enable_lpsd(void){
    hal_pmu_st.enable_lpsd();
}

bool pmu_get_chr_detect_value(void){
    return pmu_charger_is_plug();
}
void pmu_enable_sw_lp_mode(pmu_power_domain_t domain, pmu_control_mode_t mode){
    log_hal_msgid_warning("pmu_enable_sw_lp_mode not support", 0);
}
void pmu_switch_control_mode(pmu_power_domain_t domain, pmu_control_mode_t mode){
    log_hal_msgid_warning("pmu_switch_control_mode not support", 0);
}
void pmu_lock_va18(int oper){
    log_hal_msgid_warning("pmu_lock_va18 not support", 0);
}
void pmu_charger_check_faston(void){
    log_hal_msgid_warning("pmu_charger_check_faston not support", 0);
}
/*==========[BUCK/LDO]==========*/
void pmu_enable_power(pmu_power_domain_t pmu_pdm, pmu_power_operate_t operate){
    log_hal_msgid_warning("pmu_enable_power not support", 0);
}
uint32_t pmu_get_power_status(pmu_power_domain_t pmu_pdm) {
    log_hal_msgid_warning("pmu_get_power_status not support", 0);
    return PMU_INVALID;
}
pmu_power_vcore_voltage_t pmu_get_vcore_voltage(void) {
    return hal_pmu_st.get_vcore_voltage();
}
pmu_operate_status_t pmu_select_vsram_voltage(pmu_power_stage_t mode, pmu_power_vsram_voltage_t vol){
    log_hal_msgid_warning("pmu_select_vsram_voltage not support", 0);
    return PMU_INVALID;
}
pmu_operate_status_t pmu_select_vcore_voltage(pmu_power_stage_t mode, pmu_power_vcore_voltage_t vol){
    return hal_pmu_st.select_vcore_voltage(mode,vol);
}
/*==========[Charger]==========*/
void pmu_charger_init(uint16_t precc_cur,uint16_t cv_termination){
    hal_pmu_st.charger_init(precc_cur,cv_termination);
}
uint8_t pmu_enable_charger(uint8_t isEnableCharging){
    return hal_pmu_st.enable_charger(isEnableCharging);
}
bool pmu_set_icl_curent_level(uint8_t currentLevel){
    log_hal_msgid_warning("pmu_set_icl_curent_level not support", 0);
    return PMU_INVALID;
}
uint8_t pmu_get_bc12_charger_type(void) {
    return hal_pmu_st.get_bc12_charger_type();
}
void pmu_set_charger_current_limit(uint8_t port) {
    log_hal_msgid_warning("pmu_set_charger_current_limit not support", 0);
}
void pmu_select_eco_option_operating(pmu_eoc_option_t opt,pmu_eoc_operating_t oper) {
    log_hal_msgid_warning("pmu_select_eco_option_operating not support", 0);
}
uint32_t pmu_get_charger_state(void){
    log_hal_msgid_warning("pmu_get_charger_state not support", 0);
    return PMU_INVALID;
}

void pmu_set_extend_charger_time(uint8_t timeMins){
    log_hal_msgid_warning("pmu_set_extend_charger_time not support", 0);
}

void pmu_enable_recharger(bool isEnableRecharge){
    log_hal_msgid_warning("pmu_enable_recharger not support", 0);
}
bool pmu_set_rechg_voltage(uint8_t isEnableRecharge){
    log_hal_msgid_warning("pmu_set_rechg_voltage not support", 0);
    return PMU_INVALID;
}

void pmu_set_charger_current(pmu_fastcc_chrcur_t cur){
    log_hal_msgid_warning("pmu_set_charger_current not support", 0);
}

void set_cv_iterm_current(pmu_iterm_chrcur_t cur){
    log_hal_msgid_warning("set_cv_iterm_current not support", 0);
}

bool pmu_select_cv_voltage(uint8_t voltage){
    log_hal_msgid_warning("pmu_select_cv_voltage not support", 0);
    return PMU_INVALID;
}

bool pmu_select_cc_safety_timer(uint8_t timeMHrs){
    log_hal_msgid_warning("pmu_select_cc_safety_timer not support", 0);
    return PMU_INVALID;

}
uint32_t pmu_disable_vsys_discharge(uint8_t value){
    log_hal_msgid_warning("pmu_disable_vsys_discharge not support", 0);
    return PMU_INVALID;
}

/*==========[HW-JEITA/NTC]==========*/

void pmu_hw_jeita_init(void) {
    log_hal_msgid_warning("pmu_hw_jeita_init not support", 0);
}

uint8_t pmu_get_hw_jeita_status(void){
    log_hal_msgid_warning("pmu_get_hw_jeita_status not support", 0);
    return PMU_INVALID;
}

pmu_operate_status_t pmu_set_jeita_voltage(uint32_t auxadcVolt, uint8_t JeitaThreshold){
    log_hal_msgid_warning("pmu_set_jeita_voltage not support", 0);
    return PMU_INVALID;
}

void pmu_jeita_state_setting(uint8_t state,pmu_jc_perecnt_level_t ICC_JC,pmu_cv_voltage_t vol) {
    log_hal_msgid_warning("pmu_jeita_state_setting not support", 0);
    return;

}

void pmu_thermal_parameter_init(void) {
    log_hal_msgid_warning("pmu_thermal_parameter_init not support", 0);
}

uint32_t pmu_auxadc_get_pmic_temperature(void) {
    log_hal_msgid_warning("pmu_auxadc_get_pmic_temperature not support", 0);
    return PMU_INVALID;
}

bool pmu_set_hw_jeita_enable(uint8_t value){
    log_hal_msgid_warning("pmu_set_hw_jeita_enable not support", 0);
    return PMU_INVALID;
}
void pmu_set_audio_enhance(pmu_power_operate_t oper){
    log_hal_msgid_warning("pmu_set_hw_jeita_enable not support, need to fix", 0);
}
void pmu_get_lock_status(int sta){
    hal_pmu_st.get_lock_status(sta);
}
/*==========[PMU 2565]==========*/
uint8_t pmu_get_usb_input_status(void) {
    return hal_pmu_st.get_usb_input_status();
}

bool pmu_get_pwrkey_state(void){
    return hal_pmu_st.get_pwrkey_state();
}

void pmu_power_off_sequence(pmu_power_stage_t stage){
    hal_pmu_st.power_off_sequence(stage);
}

uint8_t pmu_get_power_on_reason(void) {
    return hal_pmu_st.get_power_on_reason();
}

uint8_t pmu_get_power_off_reason(void) {
    return hal_pmu_st.get_power_off_reason();
}

pmu_operate_status_t pmu_pwrkey_enable(pmu_power_operate_t oper) {
    return hal_pmu_st.pwrkey_enable(oper);
}

void pmu_set_vaud18_voltage(pmu_power_vaud18_voltage_t oper){
    hal_pmu_st.set_vaud18_voltage(oper);
}

pmu_ab2565_wrap_api_struct hal_pmu_st = {
        pmu_init_2565,
        pmu_get_register_value_2565,
        pmu_set_register_value_2565,
        pmu_force_set_register_value_2565,
        pmu_lock_vcore_ab2565,
        pmu_charger_init_2565,
        pmu_enable_charger_2565,
        pmu_get_bc12_charger_type_2565,
        pmu_get_usb_input_status_2565,
        pmu_enable_lpsd_2565,
        pmu_get_lock_status_2565,
        pmu_get_pwrkey_state_2565,
        pmu_power_off_sequence_2565,
        pmu_get_power_on_reason_2565,
        pmu_get_power_off_reason_2565,
        pmu_pwrkey_enable_2565,
        pmu_get_vcore_voltage_ab2565,
        pmu_select_vcore_voltage_ab2565,
        pmu_set_vaud18_voltage_2565,
};

#endif
#endif /* HAL_PMU_MODULE_ENABLED */
