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
//#include "hal.h"
#ifndef __HAL_PMU_INTERNAL_H__
#define __HAL_PMU_INTERNAL_H__
#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu.h"
/*==========[Basic function]==========*/
pmu_power_vcore_voltage_t pmu_lock_vcore_ab2568(pmu_power_stage_t mode,pmu_power_vcore_voltage_t vol,pmu_lock_parameter_t lock);
pmu_status_t pmu_register_callback(pmu_interrupt_index_t pmu_int_ch, pmu_callback_t callback, void *user_data);
pmu_status_t pmu_deregister_callback(pmu_interrupt_index_t pmu_int_ch);
void pmu_power_off_sequence_2568(pmu_power_stage_t stage);

/*==========[BUCK/LDO control]==========*/
/*
 * set 1 is power on
 * set 0 is power off
 * */
void pmu_enable_power_2568(pmu_power_domain_t pmu_pdm, pmu_power_operate_t operate);
uint8_t pmu_get_power_status_2568(pmu_power_domain_t pmu_pdm);
/*
 * In normal mode , switch control mode
 * set RG_BUCK_Vxxx_ON_MODE = 0: SW mode  ; set 1: HW mode
 */
void pmu_switch_control_mode_2568(pmu_power_domain_t domain, pmu_control_mode_t mode);
void pmu_enable_sw_lp_mode_2568(pmu_power_domain_t domain, pmu_control_mode_t mode);

/*==========[Buck/Ldo voltage]==========*/
/*
 * VCORE Sleep
 * a.RG_VCORE_SLEEP_VOLTAGE:00:0.7V ; 01:0.8V; 10:0.75V; 11:0.7V
 * b.RG_BUCK_VCORE_VOSEL_SLEEP:Vout = vref_lVCORE+6.25mV*X, vref_lVCORE=0.5V  default: 0.8V
 *
 * VCORE Normal
 * RG_BUCK_VCORE_VOSEL : Vout = vref_lVCORE+6.25mV*X, vref_lVCORE=0.5V default 0.9v
 * */
pmu_operate_status_t pmu_select_vcore_voltage_ab2568(pmu_power_stage_t mode ,pmu_power_vcore_voltage_t vol);
void pmu_ignore_vcore_mode(pmu_power_operate_t operate);
void pmu_vocre_lp_mode(pmu_control_mode_t mode);
pmu_operate_status_t pmu_select_vsram_voltage_ab2568(pmu_power_stage_t mode, pmu_power_vsram_voltage_t vol);
void pmu_ddie_sram_setting(uint8_t ste);
pmu_power_vcore_voltage_t pmu_get_vcore_setting_index(uint16_t vcore);
pmu_power_vcore_voltage_t pmu_get_vcore_voltage_ab2568(void);
void pmu_vcroe_voltage_turing(int symbol, int num);
void pmu_vaud18_voltage_turing(int symbol, int num);
void pmu_set_vaud18_voltage_2568(pmu_power_vaud18_voltage_t oper);
void pmu_vio18_voltage_turing(int symbol, int num);
void pmu_selet_voltage_2568(pmu_power_stage_t mode,pmu_power_domain_t domain,uint32_t vol);

/*==========[Power key & Cap touch]==========*/
pmu_operate_status_t pmu_pwrkey_enable_2568(pmu_power_operate_t oper);
pmu_operate_status_t pmu_pwrkey_duration_time(pmu_pwrkey_time_t tmr);
pmu_operate_status_t pmu_long_press_shutdown_function_sel(pmu_pwrkey_scenario_t oper);
pmu_operate_status_t pmu_cap_touch_enable(pmu_power_operate_t oper);
pmu_operate_status_t pmu_cap_touch_shutdown_enable(pmu_power_operate_t oper);
pmu_operate_status_t pmu_pk_filter(uint8_t pk_sta);
pmu_operate_status_t pmu_pwrkey_normal_key_init(pmu_pwrkey_config_t *config);

/*==========[Get PMIC hw informantion]==========*/
/*
 * b[0] : STS_PWRKEY ;Power on for PWREKY press
 * b[1] : STS_RTCA ;Power on for RTC alarm
 * b[2] : STS_CHRIN ;Power on for charger insertion
 * b[3] : STS_SPAR ;Power on for charger out in EOC
 * b[4] : STS_RBOOT ;Power on for cold reset
 * */
uint8_t pmu_get_power_on_reason_2568(void);
/*
 *0  No Power Off Event (first power on)
 *1  PWRHOLD=0
 *2  UVLO
 *3  THRDN
 *5  SW CRST
 *8  WDT CRST & reboot
 *10  pwrkey Long press shutdown
 *11  PUPSRC
 *12  KEYPWR
 *13  SYSRSTB CRST
 *14 cap lpsd shutdown
 *15  VCORE OC
 *16  VIO18 OC
 *17  VAUD18 OC
 *18  VRF OC
 *19  VCORE PG
 *20  VIO18 PG
 *21  VAUD18 PG
 *22  VRF PG
 *23  VA18 PG
 *24 VLDO33 PG
 *25 VSRAM PG
 *26 VRF LDO PG
 **/
uint8_t pmu_get_power_off_reason_2568(void);
uint8_t pmu_get_usb_input_status_2568(void);

/*==========[Other]==========*/
void pmu_srclken_control_mode_2568(pmu_power_operate_t mode);
void pmu_latch_power_key_for_bootloader(void);
void pmu_efuse_enable_reading(void);
void pmu_efuse_disable_reading(void);
void pmu_set_audio_enhance_2568(pmu_power_operate_t oper);
bool pmu_get_efuse_status(void);
void pmu_safety_confirmation(void);
void pmu_press_pk_time(void);
/*
 * VBAT to AVDD50_CHR over current protection(OCP) enable
 * 0:disable
 * 1:enable*/
void pmu_enable_ocp_2568(pmu_power_operate_t oper);
void pmu_enable_lpsd_2568(void);
void pmu_get_lock_status_2568(int sta);
bool pmu_get_pwrkey_state_2568(void);
/*==========[PMIC irq]==========*/
void pmu_eint_handler(void *parameter);
void pmu_eint_init(void);
void pmu_get_all_int_status(void);
int pmu_get_status_interrupt(pmu_interrupt_index_t int_channel);
pmu_status_t pmu_clear_interrupt(pmu_interrupt_index_t int_channel);
pmu_status_t pmu_control_enable_interrupt(pmu_interrupt_index_t int_channel, int isEnable) ;
pmu_status_t pmu_control_mask_interrupt(pmu_interrupt_index_t int_channel, int isEnable);
void pmu_irq_count(int int_channel);
void pmu_irq_init(void);
void pmu_scan_interrupt_status(void);

/*==========[PMIC Basic setting]==========*/
void pmu_init_2568(void);
uint32_t pmu_d2d_i2c_read(unsigned char *ptr_send, unsigned char *ptr_read, int type);
uint32_t pmu_get_register_value_ab2568(uint32_t address, uint32_t mask, uint32_t shift);
pmu_operate_status_t pmu_set_register_value_ab2568(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value);
void pmic_i2c_init(void);
void pmic_i2c_deinit(void);
void hal_pmu_sleep_backup(void);
void pmu_set_register_value_ddie(uint32_t address, short int mask, short int shift, short int value);
uint32_t pmu_get_register_value_ddie(uint32_t address, short int mask, short int shift);
void pmu_lock_va18_2568(int oper);

#endif /* HAL_PMU_MODULE_ENABLED */
#endif
