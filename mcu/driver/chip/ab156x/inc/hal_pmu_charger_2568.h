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


#ifndef __HAL_PMU_CHARGER_H__
#define __HAL_PMU_CHARGER_H__
#ifdef HAL_PMU_MODULE_ENABLED

#define DIGITAL_THERMAL_FUNCTION 1
#define HW_JEITA_HOT_STAGE 0xF
#define HW_JEITA_WARM_STAGE 0xE
#define HW_JEITA_NORMAL_STAGE 0xC
#define HW_JEITA_COOL_STAGE 0x8
#define HW_JEITA_COLD_STAGE 0
#define HW_JEITA_ERRIR_STATE 0x4

/*
 *  BC1.2 address */

#define PMU_REG_BASE_AB155X (0xA2070000)

// PMU Group
#define PMU2_ANA_CON0   (PMU_REG_BASE_AB155X+0x0500)
#define PMU2_ANA_CON1   (PMU_REG_BASE_AB155X+0x0504)
#define PMU2_ELR_0      (PMU_REG_BASE_AB155X+0x0508)
#define PMU2_ANA_RO     (PMU_REG_BASE_AB155X+0x0510)

//Control RG

#define PMU_BC12_IBIAS_EN_V12_ADDR                  PMU2_ANA_CON0
#define PMU_BC12_IBIAS_EN_V12_MASK                  0x1
#define PMU_BC12_IBIAS_EN_V12_SHIFT                 0

#define PMU_BC12_CMP_EN_V12_ADDR                    PMU2_ANA_CON0
#define PMU_BC12_CMP_EN_V12_MASK                    0x3
#define PMU_BC12_CMP_EN_V12_SHIFT                   1

#define PMU_BC12_DCD_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_DCD_EN_V12_MASK                          0x1
#define PMU_BC12_DCD_EN_V12_SHIFT                         3

#define PMU_BC12_IPDC_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_IPDC_EN_V12_MASK                          0x3
#define PMU_BC12_IPDC_EN_V12_SHIFT                         4

#define PMU_BC12_IPD_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_IPD_EN_V12_MASK                          0x3
#define PMU_BC12_IPD_EN_V12_SHIFT                         6

#define PMU_BC12_IPD_HALF_EN_V12_ADDR                       PMU2_ANA_CON0
#define PMU_BC12_IPD_HALF_EN_V12_MASK                       0x1
#define PMU_BC12_IPD_HALF_EN_V12_SHIFT                      8

#define PMU_BC12_IPU_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_IPU_EN_V12_MASK                          0x3
#define PMU_BC12_IPU_EN_V12_SHIFT                         9

#define PMU_BC12_VREF_VTH_EN_V12_ADDR                       PMU2_ANA_CON0
#define PMU_BC12_VREF_VTH_EN_V12_MASK                       0x3
#define PMU_BC12_VREF_VTH_EN_V12_SHIFT                      11

#define PMU_BC12_SRC_EN_V12_ADDR                          PMU2_ANA_CON0
#define PMU_BC12_SRC_EN_V12_MASK                          0x3
#define PMU_BC12_SRC_EN_V12_SHIFT                         13

#define PMU_BC12_IPU_TEST_EN_V12_ADDR                       PMU2_ANA_CON1
#define PMU_BC12_IPU_TEST_EN_V12_MASK                       0x1
#define PMU_BC12_IPU_TEST_EN_V12_SHIFT                      0

#define PMU_BC12_CS_TRIM_V12_ADDR                          PMU2_ELR_0
#define PMU_BC12_CS_TRIM_V12_MASK                          0x3F
#define PMU_BC12_CS_TRIM_V12_SHIFT                         0

#define PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR                       PMU2_ANA_RO
#define PMU_AQ_QI_BC12_CMP_OUT_V12_MASK                       0x1
#define PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT                      0
/*==========[Basic function]==========*/
void pmu_charger_init_2568(uint16_t precc_cur,uint16_t cv_termination);
void pmu_trim_ic_init(void);
bool pmu_get_chr_detect_value_2568(void);
uint32_t pmu_disable_vsys_discharge_2568(uint8_t value);
void pmu_charger_check_faston_2568(void);
bool pmu_select_cv_voltage_2568(uint8_t voltage);
bool pmu_select_precc_voltage_2568(uint8_t voltage);
bool pmu_select_cc_safety_timer_2568(uint8_t timeMHrs);
void pmu_set_charger_current_limit_2568(uint8_t port);
void pmu_control_pmic_protect(uint8_t tstWKeymode);
uint32_t pmu_get_charger_state_2568(void);
/*==========[charger]==========*/
uint8_t pmu_enable_charger_2568(uint8_t isEnableCharging);
bool pmu_set_icl_curent_level_2568(uint8_t currentLevel);
bool pmu_set_extend_charger_time_2568(uint8_t timeMins);
bool pmu_enable_recharger_2568(bool isEnableRecharge);
bool pmu_set_rechg_voltage_2568(uint8_t rechargeVoltage);
/*==========[HW-JEITA]==========*/
bool pmu_set_hw_jeita_enable_2568(uint8_t value);
void pmu_hw_jeita_init_2568(void);
pmu_operate_status_t pmu_set_jeita_voltage_2568(uint32_t auxadcVolt, uint8_t JeitaThreshold);
void pmu_set_jeita_state_setting_2568(uint8_t state,pmu_jc_perecnt_level_t ICC_JC,pmu_cv_voltage_t vol);
uint8_t pmu_get_hw_jeita_status_2568(void);
void pmu_set_pre_charger_current_2568(pmu_fastcc_chrcur_t cur);
void pmu_set_charger_current_2568(pmu_fastcc_chrcur_t cur);
void pmu_set_iterm_current_irq_2568(pmu_iterm_chrcur_t cur);
void pmu_set_iterm_current_2568(pmu_iterm_chrcur_t cur);
int pmu_get_charger_current_index(void);
/*==========[BC 1.2 behavior]==========*/
void pmu_bc12_init(void);
uint8_t pmu_get_bc12_charger_type_2568(void);
/*==========[ECO setting]==========*/
bool pmu_set_plugin_db_sw_sel(uint8_t value);
bool pmu_set_ppfet_lp_mode(uint8_t value);
/*==========[ECO]==========*/
void pmu_select_eco_option_operating_2568(pmu_eoc_option_t opt,pmu_eoc_operating_t oper);
void pmu_charger_eoc2_exit_setting(void);
void pmu_charger_eoc2_recharger_interrupt(void);
void pmu_charger_eoc2_setting(void);
void pmu_charger_eoc3_exit_setting(void);
void pmu_charger_eoc3_recharger_setting(void);
void pmu_charger_eoc3_checking(void);
void pmu_charger_eoc3_setting(void);
void pmu_charger_eoc4_setting(void);
void pmu_charger_eoc4_exit(void);
void pmu_eoc_option_setting(pmu_eoc_option_t opt);
bool pmu_enable_sysldo(bool isEnableSysdo);
/*==========[Other]==========*/
uint32_t pmu_set_charger_parameter_2568(pmu_charger_parameter_t index,uint32_t value);
void pmu_core_pmu_turbo_the_timer(uint8_t isEnable);
bool pmu_rstb_select(uint8_t value);
uint32_t pmu_get_faston_flag(void);
uint32_t pmu_get_vsys_dpm_status(void);
uint32_t pmu_get_vbus_dpm_status(void);

#endif /* HAL_PMU_MODULE_ENABLED */
#endif

