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

#ifndef __HAL_PMU_INTERNAL_H__
#define __HAL_PMU_INTERNAL_H__
#ifdef HAL_PMU_MODULE_ENABLED

typedef enum {
    PMU_NO_FTK = 0xFF,
    PMU_ECO1 = 1,
    PMU_ECO2 = 2,
    PMU_ECO3 = 3,
} pmu_adie_version_t;

typedef enum {
    IC_NONE,
    IC_MT2822S,
    IC_MT2822A,
    IC_AB1568,
    IC_AB1565,
    IC_AB1565A,
    IC_MT2822M,
    IC_AB1565M,
    IC_AB1565AM,
    IC_AB1568M,
    IC_MT2821,
    IC_AB1565D,
} pmu_ic_version_t;

typedef enum {
    CAP_LPSD,
    REGEN_LPSD,
    SYS_RST,
    WD_RST,
} pmu_lpsd_rst_sel_t;

typedef enum {
    PMU_PWRKEY_IRQ_RISE_FLAG,
    PMU_PWRKEY_IRQ_RISE_EN,
    PMU_PWRKEY_IRQ_FALL_FLAG,
    PMU_PWRKEY_IRQ_FALL_EN,
    PMU_REGEN_IRQ_TIME_SEL,
    PMU_CHG_PON_FLAG = 6,
    PMU_REGEN_PON_FLAG,
    PMU_RTC_ALARM_FLAG,
    PMU_CHG_ALARM_FLAG,
    PMU_REGEN_ALARM_FLAG,
    PMU_RTC_MODE_FLAG,
    PMU_CAP_LPSD_FLAG,
    PMU_REGEN_LPSD_FLAG,
    PMU_SYS_RST_FLAG,
    PMU_WD_RST_FLAG,
} pmu_pwrkey_int_mask_t;

typedef union union_pmu_power_on_reason
{
    uint8_t value;
    struct stru_pmu_power_on_reason
    {
        bool off_norm_regen_pon      : 1;
        bool rtc_norm_rtc_alarm      : 1;
        bool off_norm_chg_pon        : 1;
        bool rtc_norm_chg_alarm      : 1;
        bool rtc_norm_regen_alarm    : 1;
        bool reserved5               : 1;
        bool reserved6               : 1;
        bool reserved7               : 1;
    } field;
} PMU_POWER_ON_REASON_UNION;

enum
{
    CHG_CASE_TYPE_NONE,
    CHG_CASE_TYPE_GPIOXY,
    CHG_CASE_TYPE_SMART,
    CHG_CASE_TYPE_SMART_V2,
    CHG_CASE_TYPE_SMART_1WIRE,
    CHG_CASE_TYPE_SMART_1WIRE_3PIN,
};

/*typedef union union_pmu_power_off_reason
{
    uint8_t value;
    struct stru_pmu_power_off_reason
    {
        bool norm_rtc_rtc_mode       : 1;
        bool norm_rtc_cap_lpsd       : 1;
        bool norm_rtc_regen_lpsd     : 1;
        bool norm_excep_sys_rst      : 1;
        bool norm_excep_wd_rst       : 1;
        bool reserved5               : 1;
        bool reserved6               : 1;
        bool reserved7               : 1;
    } field;

} PMU_POWER_OFF_REASON_UNION;*/

extern uint8_t adie_version;
extern uint8_t adie_kgd;

extern pmu_function_t pmu_function_table_2565[PMU_INT_MAX_2565];

/*==========[PMIC RG]==========*/
uint32_t pmu_d2d_i2c_read(unsigned char *ptr_send, unsigned char *ptr_read, int type);
void pmic_i2c_init(void);
void pmic_i2c_deinit(void);
uint32_t pmu_get_register_value_2565(uint32_t address, uint32_t mask, uint32_t shift);
pmu_operate_status_t pmu_set_register_value_2565(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value);
pmu_operate_status_t pmu_force_set_register_value_2565(uint32_t address, uint32_t value);
void pmu_set_register_value_ddie(uint32_t address, short int mask, short int shift, short int value);
uint32_t pmu_get_register_value_ddie(uint32_t address, short int mask, short int shift);

/*==========[Basic function]==========*/
uint32_t pmu_get_ic_version(void);
uint8_t pmu_get_power_on_reason_2565(void);
uint8_t pmu_get_power_off_reason_2565(void);
void pmu_init_2565(void);
void pmu_power_off_sequence_2565(pmu_power_stage_t stage);
void hal_pmu_sleep_backup(void);
void hal_pmu_sleep_resume(void);
void pmu_uart_psw_enable(pmu_power_operate_t en);

/*==========[PMIC irq]==========*/
void pmu_eint_init(void);
void pmu_eint_handler(void *parameter);
void pmu_pwrkey_handler(uint16_t pwrkey_flag);
pmu_status_t pmu_register_callback_2565(pmu_interrupt_index_2565_t pmu_int_ch, pmu_callback_t callback, void *user_data);
pmu_status_t pmu_deregister_callback_2565(pmu_interrupt_index_2565_t pmu_int_ch);

/*==========[Power key & LPSD & USB]==========*/
void pmu_latch_power_key_for_bootloader(void);
pmu_operate_status_t pmu_pwrkey_enable_2565(pmu_power_operate_t oper);
//pmu_operate_status_t pmu_pwrkey_duration_time(pmu_pwrkey_time_t tmr);
pmu_operate_status_t pmu_pwrkey_normal_key_init(pmu_pwrkey_config_t *config);
bool pmu_get_pwrkey_state_2565(void);
void pmu_enable_lpsd_2565(void);
uint8_t pmu_get_usb_input_status_2565(void);

/*==========[Buck/Ldo voltage]==========*/
pmu_operate_status_t pmu_select_vcore_voltage_ab2565(pmu_power_stage_t mode ,pmu_power_vcore_voltage_t vol);
pmu_power_vcore_voltage_t pmu_get_vcore_voltage_ab2565(void);
pmu_power_vcore_voltage_t pmu_lock_vcore_ab2565(pmu_power_stage_t mode,pmu_power_vcore_voltage_t vol,pmu_lock_parameter_t lock);
void pmu_get_lock_status_2565(int sta);
void pmu_set_vaud18_voltage_2565(pmu_power_vaud18_voltage_t oper);
/*==========[Unused]==========*/
pmu_status_t pmu_clear_interrupt(pmu_interrupt_index_t int_channel);

#endif /* HAL_PMU_MODULE_ENABLED */
#endif
