/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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


#ifndef __HAL_PMU_INTEFACE_H__
#define __HAL_PMU_INTEFACE_H__
#ifdef HAL_PMU_MODULE_ENABLED
#define PMU_DEBUG_ENABLE
#ifdef  PMU_DEBUG_ENABLE
#define log_debug(_message,...) printf(_message, ##__VA_ARGS__)
#else
#define log_debug(_message,...)
#endif



#ifdef AB1565
#include "hal_pmu.h"
#include "hal_pmu_cali_2565.h"
#include "hal_pmu_internal_2565.h"
#include "hal_pmu_charger_2565.h"
#include "hal_pmu_auxadc_2565.h"
#include "hal_pmu_ab2565_platform.h"

typedef struct{
    void (*init)(void);
    uint32_t (*get_register_value)(uint32_t address, uint32_t mask, uint32_t shift);
    pmu_operate_status_t (*set_register_value)(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value);
    pmu_operate_status_t (*force_set_register_value)(uint32_t address, uint32_t value);
    pmu_power_vcore_voltage_t (*lock_vcore)(pmu_power_stage_t mode,pmu_power_vcore_voltage_t vol,pmu_lock_parameter_t lock);
    void (*charger_init)(uint16_t precc_cur,uint16_t cv_termination);
    uint8_t (*enable_charger)(uint8_t isEnableCharging);
    uint8_t (*get_bc12_charger_type)(void);
    uint8_t (*get_usb_input_status)(void);
    void (*enable_lpsd)(void);
    void (*get_lock_status)(int sta);
    bool (*get_pwrkey_state)(void);
    void (*power_off_sequence)(pmu_power_stage_t stage);
    uint8_t (*get_power_on_reason)(void);
    uint8_t (*get_power_off_reason)(void);
    pmu_operate_status_t (*pwrkey_enable)(pmu_power_operate_t oper);
    pmu_power_vcore_voltage_t (*get_vcore_voltage)(void);
    pmu_operate_status_t (*select_vcore_voltage) (pmu_power_stage_t mode, pmu_power_vcore_voltage_t vol);
    void (*set_vaud18_voltage)(pmu_power_vaud18_voltage_t oper);
}pmu_ab2565_wrap_api_struct;

#else
#include "hal_pmu_internal_2568.h"
#include "hal_pmu_charger_2568.h"
#include "hal_pmu_auxadc_2568.h"
#include "hal_pmu_ab2568_platform.h"

typedef struct{
    void (*init)(void);
    pmu_operate_status_t (*set_register_value)(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value);
    uint32_t (*get_register_value)(uint32_t address, uint32_t mask, uint32_t shift);
    void (*enable_power)(pmu_power_domain_t pmu_pdm, pmu_power_operate_t operate);
    uint8_t (*get_power_status)(pmu_power_domain_t pmu_pdm);
    pmu_power_vcore_voltage_t (*get_vcore_voltage)(void);
    pmu_power_vcore_voltage_t (*lock_vcore)(pmu_power_stage_t mode,pmu_power_vcore_voltage_t vol,pmu_lock_parameter_t lock);
    void (*charger_init)(uint16_t precc_cur,uint16_t cv_termination);
    uint8_t (*enable_charger)(uint8_t isEnableCharging);
    void (*hw_jeita_init)(void);
    uint8_t (*get_hw_jeita_status)(void);
    pmu_operate_status_t (*set_jeita_voltage)(uint32_t auxadcVolt, uint8_t JeitaThreshold);
    void (*jeita_state_setting)(uint8_t state,pmu_jc_perecnt_level_t ICC_JC,pmu_cv_voltage_t vol);
    uint8_t (*get_bc12_charger_type)(void);
    void (*set_charger_current_limit)(uint8_t port);
    void (*thermal_parameter_init)(void);
    uint32_t (*get_pmic_temperature)(void);
    void (*select_eco_option_operating)(pmu_eoc_option_t opt,pmu_eoc_operating_t oper);
    bool (*set_icl_curent_level)(uint8_t currentLevel);
    uint32_t (*get_charger_state)(void);
    bool (*set_extend_charger_time)(uint8_t timeMins);
    bool (*get_chr_detect_value)(void);
    bool (*enable_recharger)(bool isEnableRecharge);
    bool (*set_rechg_voltage)(uint8_t rechargeVoltage);
    bool (*set_hw_jeita_enable)(uint8_t value);
    uint32_t (*disable_vsys_discharge)(uint8_t value);
    void (*lock_va18)(int oper);
    void (*charger_check_faston)(void);
    bool (*select_cv_voltage)(uint8_t voltage);
    bool (*select_cc_safety_timer)(uint8_t timeMHrs);
    void (*enable_sw_lp_mode)(pmu_power_domain_t domain, pmu_control_mode_t mode);
    void (*switch_control_mode)(pmu_power_domain_t domain, pmu_control_mode_t mode);
    void (*set_charger_current)(pmu_fastcc_chrcur_t cur);
    void (*set_iterm_current_irq)(pmu_iterm_chrcur_t cur);
    pmu_operate_status_t (*select_vsram_voltage) (pmu_power_stage_t mode, pmu_power_vsram_voltage_t vol);
    pmu_operate_status_t (*select_vcore_voltage) (pmu_power_stage_t mode, pmu_power_vcore_voltage_t vol);
    uint8_t (*get_usb_input_status)(void);
    void (*set_audio_enhance)(pmu_power_operate_t oper);
    void (*set_pre_charger_current)(pmu_fastcc_chrcur_t cur);
    void (*set_iterm_current)(pmu_iterm_chrcur_t cur);
    void (*enable_ocp)(pmu_power_operate_t oper);
    void (*enable_lpsd)(void);
    void (*get_lock_status)(int sta);
    bool (*get_pwrkey_state)(void);
    void (*power_off_sequence)(pmu_power_stage_t stage);
    uint8_t (*get_power_on_reason)(void);
    uint8_t (*get_power_off_reason)(void);
    pmu_operate_status_t (*pwrkey_enable)(pmu_power_operate_t oper);
    bool (*select_precc_voltage)(uint8_t voltage);
    void (*set_vaud18_voltage)(pmu_power_vaud18_voltage_t oper);
}pmu_ab2568_wrap_api_struct;
#endif

#endif /* HAL_PMU_MODULE_ENABLED */
#endif
