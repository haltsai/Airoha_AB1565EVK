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
//#include "hal_pmu_wrap_interface.h"
#ifndef __HAL_PMU_MODULE_H__

#define __HAL_PMU_MODULE_H__
#ifdef HAL_PMU_MODULE_ENABLED
#ifdef AB1568
#include "hal_pmu_ab2568_platform.h"
#else
#include "hal_pmu_ab2565_platform.h"
#endif

/*
 * HAL_KEEP_VAUD18_POWER : Keep VAUD18 power on */
#ifdef MTK_KEEP_VAUD18_POWER
#define HAL_KEEP_VAUD18_POWER
#endif

/* PMU low power setting */
#define PMU_LOW_POWER_SETTING
//#define PMU_LOW_POWER_VA18_SETTING
//#define PMU_LOW_POWER_VCORE_SETTING
#define PMU_LOW_POWER_VIO18_SETTING
//#define PMU_LOW_POWER_VRF_SETTING
//#define PMU_LOW_POWER_VAUD18_SETTING

#define PMIC_SLAVE_ADDR            0x6B
#define INVALID_INTERRUPT_VALUE    0xFF
#define PMU_PRESS_PK_TIME 100000  //100ms

#define PMU_VCORE_CP     0xA20A0204
typedef enum {
    IBAT_TUNE_TRIM = 0,
} pmu_charger_parameter_t;
typedef enum{
    PMU_LOCK=0,
    PMU_UNLOCK,
    PMU_TEMP
}pmu_lock_parameter_t;

typedef enum {
    PMU_NONE          = 0,                 /**< NONE Trigger */
    PMU_EDGE_RISING   = 1,                 /**< edge and rising trigger */
    PMU_EDGE_FALLING  = 2,                 /**< edge and falling trigger */
    PMU_EDGE_FALLING_AND_RISING = 3        /**< edge and falling or rising trigger */
} pmu_int_trigger_mode_t;

typedef enum {
    PMU_STATUS_INVALID_PARAMETER  = -1,     /**< pmu error invalid parameter */
    PMU_STATUS_ERROR              = 0,     /**< pmu undefined error */
    PMU_STATUS_SUCCESS            = 1       /**< pmu function ok */
}pmu_status_t;

typedef enum {
    PMU_INVALID = -1,
    PMU_ERROR   = 0,
    PMU_OK    = 1
} pmu_operate_status_t;

typedef enum {
    PMU_OFF    = 0,
    PMU_ON   = 1,
} pmu_power_operate_t;

typedef enum {
    PMU_SW_MODE,
    PMU_HW_MODE,
} pmu_control_mode_t;

typedef enum {
    PMU_DURATION_8S,
    PMU_DURATION_10S,
    PMU_DURATION_15S,
    PMU_DURATION_20S,
} pmu_pwrkey_time_t;

typedef enum {
    PMU_DEBOUNCE_PWRKEY,
    PMU_RELEASE_PWRKEY,
    PMU_REPRESS_PWRKEY,
    PMU_RESET_DEFAULT,
} pmu_pwrkey_scenario_t;

typedef enum {
    PMU_PK_PRESS    = 0,
    PMU_PK_RELEASE   = 1,
} pmu_pk_operate_t;

typedef enum {
    PMU_PWROFF,
    PMU_RTC,
    PMU_SLEEP,
    PMU_NORMAL,
    PMU_DVS,
} pmu_power_stage_t;

typedef enum {
    PMIC_VAUD18_1P45_V,
    PMIC_VAUD18_1P50_V,
    PMIC_VAUD18_1P55_V,
    PMIC_VAUD18_1P60_V,
    PMIC_VAUD18_1P65_V,
    PMIC_VAUD18_1P70_V,
    PMIC_VAUD18_1P71_V,
    PMIC_VAUD18_1P75_V,
    PMIC_VAUD18_1P80_V,
} pmu_power_vaud18_voltage_t;

typedef enum {
       PMIC_VCORE_0P5_V,
       PMIC_VCORE_0P6_V,
       PMIC_VCORE_0P7_V,
       PMIC_VCORE_0P73_V,
       PMIC_VCORE_0P75_V,
       PMIC_VCORE_0P8_V,
       PMIC_VCORE_0P9_V,
       PMIC_VCORE_FAIL_V,
} pmu_power_vcore_voltage_t;

typedef enum {
       PMIC_VSRAM_0P9=0,
       PMIC_VSRAM_0P85=1,
       PMIC_VSRAM_0P8=2,
} pmu_power_vsram_voltage_t;

typedef enum{
    PMU_POWER_STABLE=0,
    PMU_POWER_TRY_TO_ENABLE,
    PMU_POWER_TRY_TO_DISABLE,
}pmu_power_status_t;

typedef enum {
    PMU_BUCK_VCORE,     //0.7~13V
    PMU_BUCK_VIO18,     //1.8V
    PMU_BUCK_VRF,       //1.45V/2.2V
    PMU_BUCK_VAUD18,    //0.9V/1.8V
    PMU_LDO_VA18,       //1.8V
    PMU_LDO_VLDO33,     //3.3V
    PMU_LDO_VRF,       //1.8V
    PMU_LDO_VSRAM,
} pmu_power_domain_t;

typedef enum{
        RG_ICC_JC_20    = 0,
        RG_ICC_JC_40    = 4,
        RG_ICC_JC_60    = 5,
        RG_ICC_JC_80    = 6,
        RG_ICC_JC_100   = 7,
}pmu_jc_perecnt_level_t;

typedef enum
{
    RG_VCV_VOLTAGE_4P05V = 0,   //4.05V
    RG_VCV_VOLTAGE_4P10V = 1,   //4.10V
    RG_VCV_VOLTAGE_4P15V = 2,   //4.15V
    RG_VCV_VOLTAGE_4P20V = 3,   //4.20V
    RG_VCV_VOLTAGE_4P25V = 4,   //4.25V
    RG_VCV_VOLTAGE_4P30V = 5,   //4.30V
    RG_VCV_VOLTAGE_4P35V = 6,   //4.35V
    RG_VCV_VOLTAGE_4P40V = 7,   //4.40V
    RG_VCV_VOLTAGE_4P45V = 8,   //4.45V
    RG_VCV_VOLTAGE_4P50V = 9,   //4.50V
    RG_VCV_VOLTAGE_4P55V = 10,  //4.55V
    RG_VCV_VOLTAGE_4P60V = 11,  //4.60V
}pmu_cv_voltage_t;

typedef enum {
    ICL_ITH_10mA = 0,     //  10   mA
    ICL_ITH_90mA = 1,     //  90   mA
    ICL_ITH_200mA = 2,    //  200  mA
    ICL_ITH_300mA = 3,    //  300  mA
    ICL_ITH_400mA = 4,    //  400  mA
    ICL_ITH_500mA = 5,    //  500  mA
    ICL_ITH_600mA = 6,    //  600  mA
    ICL_ITH_700mA = 7,    //  700  mA
    ICL_ITH_800mA = 8,    //  800  mA
    ICL_ITH_900mA = 9,    //  900  mA
    ICL_ITH_1000mA = 10,  //  1000 mA
} pmu_icl_level_t;

typedef enum {
    RG_INT_PWRKEY,      //INT_CON0 index :0
    RG_INT_PWRKEY_R,
    RG_INT_VLDO33_LBAT_DET,
    RG_INT_VBAT_RECHG,
    RG_INT_JEITA_HOT,
    RG_INT_JEITA_WARM,
    RG_INT_JEITA_COOL,
    RG_INT_JEITA_COLD,
    RG_INT_BATOV,
    RG_INT_CHRDET,
    RG_INT_ChgStatInt,
    RG_INT_VBUS_OVP,
    RG_INT_VBUS_UVLO,
    RG_INT_ICHR_ITERM,
    RG_INT_ICHR_CHG_CUR,
    RG_INT_SAFETY_TIMEOUT,
    RG_INT_AD_LBAT_LV,     //INT_CON1 index :16
    RG_INT_THM_OVER40,
    RG_INT_THM_OVER55,
    RG_INT_THM_OVER110,
    RG_INT_THM_OVER125,
    RG_INT_THM_UNDER40,
    RG_INT_THM_UNDER55,
    RG_INT_THM_UNDER110,
    RG_INT_THM_UNDER125,
    RG_INT_BAT2_H_R,
    RG_INT_bat_h_lv,
    RG_INT_bat_l_lv,
    RG_INT_thr_h_r,
    RG_INT_thr_h_f,
    RG_INT_thr_l_r,
    RG_INT_thr_l_f,
    RG_INT_VCORE_OC,     //INT_CON2 index :32
    RG_INT_VIO18_OC,
    RG_INT_VRF_OC,
    RG_INT_VAUD18_OC,
    RG_INT_VLDO33_OC,
    RG_INT_VA18_OC,
    RG_INT_VRF11_OC,
    RG_INT_VSRAM_OC,
    RG_INT_ILimInt,        //INT_CON3 index :40
    RG_INT_ThermRegInt,
    RG_INT_AVDD50_OC,
    RG_INT_VSYS_DPM,
    PMU_INT_MAX,
} pmu_interrupt_index_t;

typedef enum {
    RG_INT_PWRKEY_FALL,                    //PMU_CON3,     index :0
    RG_INT_PWRKEY_RISE,                    //PMU_CON3,     index :2
    RG_INT_CHG_IN,                         //CHARGER_CON1, index :0
    RG_INT_CHG_OUT,                        //CHARGER_CON1, index :2
    RG_INT_CHG_COMPL,                      //CHARGER_CON1, index :4
    RG_INT_CHG_RECHG,                      //CHARGER_CON1, index :6
    RG_INT_ADC_SW_TRIG,                    //ADC_CON3,     index :1
    RG_INT_ADC_HW_TRIG,                    //ADC_CON3,     index :3
    PMU_INT_MAX_2565,
} pmu_interrupt_index_2565_t;

enum
{
    PMU_NOT_INIT,
    PMU_INIT,
};

enum
{
    TST_W_KEY_HW_MODE,
    TST_W_KEY_SW_MODE,
};

typedef void (*pmu_callback_t)(void);

typedef struct
{
    void (*pmu_callback)(void);
    void *user_data;
    bool init_status;
	bool isMask;
} pmu_function_t;

typedef struct
{
    pmu_callback_t callback1; //press callback
    void *user_data1;
    pmu_callback_t callback2; //release callback
    void *user_data2;
} pmu_pwrkey_config_t;

typedef enum {
    option_setting=0,
    option2_init=1,
    option2_recharger = 2,
    option2_exit = 3,
    option3_init = 4,
    option3_checking = 5,
    option3_recharger = 6,
    option3_exit = 7,
    option4_init = 8,
    option4_exit = 9,
} pmu_eoc_operating_t;

typedef enum {
    pmu_eoc_option1=1,
    pmu_eoc_option2=2,
    pmu_eoc_option3=3,
    pmu_eoc_option4=4,
} pmu_eoc_option_t;

typedef enum {
    pmu_fastcc_chrcur_0p5mA=0,
    pmu_fastcc_chrcur_1mA,
    pmu_fastcc_chrcur_1p5mA,
    pmu_fastcc_chrcur_2mA,
    pmu_fastcc_chrcur_2p5mA,
    pmu_fastcc_chrcur_3mA,
    pmu_fastcc_chrcur_3p5mA,
    pmu_fastcc_chrcur_4mA,
    pmu_fastcc_chrcur_4p5mA,
    pmu_fastcc_chrcur_5mA,
    pmu_fastcc_chrcur_10mA,
    pmu_fastcc_chrcur_15mA,
    pmu_fastcc_chrcur_20mA,
    pmu_fastcc_chrcur_25mA,
    pmu_fastcc_chrcur_30mA,
    pmu_fastcc_chrcur_35mA,
    pmu_fastcc_chrcur_40mA,
    pmu_fastcc_chrcur_45mA,
    pmu_fastcc_chrcur_50mA,
    pmu_fastcc_chrcur_55mA,
    pmu_fastcc_chrcur_60mA,
    pmu_fastcc_chrcur_65mA,
    pmu_fastcc_chrcur_70mA,
    pmu_fastcc_chrcur_75mA,
    pmu_fastcc_chrcur_80mA,
    pmu_fastcc_chrcur_90mA,
    pmu_fastcc_chrcur_100mA,
    pmu_fastcc_chrcur_110mA,
    pmu_fastcc_chrcur_120mA,
    pmu_fastcc_chrcur_130mA,
    pmu_fastcc_chrcur_140mA,
    pmu_fastcc_chrcur_150mA,
    pmu_fastcc_chrcur_160mA,
    pmu_fastcc_chrcur_170mA,
    pmu_fastcc_chrcur_180mA,
    pmu_fastcc_chrcur_190mA,
    pmu_fastcc_chrcur_200mA,
    pmu_fastcc_chrcur_210mA,
    pmu_fastcc_chrcur_220mA,
    pmu_fastcc_chrcur_230mA,
    pmu_fastcc_chrcur_240mA,
    pmu_fastcc_chrcur_250mA,
    pmu_fastcc_chrcur_260mA,
    pmu_fastcc_chrcur_270mA,
    pmu_fastcc_chrcur_280mA,
    pmu_fastcc_chrcur_290mA,
    pmu_fastcc_chrcur_300mA,
    pmu_fastcc_chrcur_310mA,
    pmu_fastcc_chrcur_320mA,
    pmu_fastcc_chrcur_330mA,
    pmu_fastcc_chrcur_340mA,
    pmu_fastcc_chrcur_350mA,
    pmu_fastcc_chrcur_360mA,
    pmu_fastcc_chrcur_370mA,
    pmu_fastcc_chrcur_380mA,
    pmu_fastcc_chrcur_390mA,
    pmu_fastcc_chrcur_400mA,
    pmu_fastcc_chrcur_410mA,
    pmu_fastcc_chrcur_420mA,
    pmu_fastcc_chrcur_430mA,
    pmu_fastcc_chrcur_440mA,
    pmu_fastcc_chrcur_450mA,
    pmu_fastcc_chrcur_460mA,
    pmu_fastcc_chrcur_470mA,
    pmu_fastcc_chrcur_480mA,
    pmu_fastcc_chrcur_490mA,
    pmu_fastcc_chrcur_500mA,
} pmu_fastcc_chrcur_t;

typedef enum {
    pmu_iterm_chrcur_0p5mA=0,
    pmu_iterm_chrcur_1mA,
    pmu_iterm_chrcur_1p5mA,
    pmu_iterm_chrcur_2mA,
    pmu_iterm_chrcur_2p5mA,
    pmu_iterm_chrcur_3mA,
    pmu_iterm_chrcur_3p5mA,
    pmu_iterm_chrcur_4mA,
    pmu_iterm_chrcur_4p5mA,
    pmu_iterm_chrcur_5mA,
    pmu_iterm_chrcur_5p5mA,
    pmu_iterm_chrcur_6mA,
    pmu_iterm_chrcur_6p5mA,
    pmu_iterm_chrcur_7mA,
    pmu_iterm_chrcur_7p5mA,
    pmu_iterm_chrcur_8mA,
    pmu_iterm_chrcur_9mA,
    pmu_iterm_chrcur_10mA,
    pmu_iterm_chrcur_11mA,
    pmu_iterm_chrcur_12mA,
    pmu_iterm_chrcur_13mA,
    pmu_iterm_chrcur_14mA,
    pmu_iterm_chrcur_15mA,
    pmu_iterm_chrcur_16mA,
    pmu_iterm_chrcur_17mA,
    pmu_iterm_chrcur_18mA,
    pmu_iterm_chrcur_19mA,
    pmu_iterm_chrcur_20mA,
    pmu_iterm_chrcur_21mA,
    pmu_iterm_chrcur_22mA,
    pmu_iterm_chrcur_23mA,
    pmu_iterm_chrcur_24mA,
    pmu_iterm_chrcur_25mA,
    pmu_iterm_chrcur_26mA,
    pmu_iterm_chrcur_27mA,
    pmu_iterm_chrcur_28mA,
    pmu_iterm_chrcur_29mA,
    pmu_iterm_chrcur_30mA,
    pmu_iterm_chrcur_31mA,
    pmu_iterm_chrcur_32mA,
    pmu_iterm_chrcur_34mA,
    pmu_iterm_chrcur_36mA,
    pmu_iterm_chrcur_38mA,
    pmu_iterm_chrcur_40mA,
    pmu_iterm_chrcur_42mA,
    pmu_iterm_chrcur_44mA,
    pmu_iterm_chrcur_46mA,
    pmu_iterm_chrcur_48mA,
    pmu_iterm_chrcur_50mA,
    pmu_iterm_chrcur_60mA,
} pmu_iterm_chrcur_t;

enum {
    SDP_CHARGER = 1,
    CDP_CHARGER,
    DCP_CHARGER,
    SS_TABLET_CHARGER,
    IPAD2_IPAD4_CHARGER,
    IPHONE_5V_1A_CHARGER,
    NON_STD_CHARGER,
    DP_DM_FLOATING,
    UNABLE_TO_IDENTIFY_CHARGER,
    INVALID_CHARGER = 0xFF,
};

/* Because of restrictions on HW
 * Charging current through matching combination on the battery pack
 * But due to the digital design of HW, list all charging current
*/

/*==========[Basic function]==========*/
void pmu_init(void);
pmu_power_vcore_voltage_t pmu_lock_vcore(pmu_power_stage_t mode,pmu_power_vcore_voltage_t vol,pmu_lock_parameter_t lock);
pmu_operate_status_t pmu_set_register_value(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value);
uint32_t pmu_get_register_value(uint32_t address, uint32_t mask, uint32_t shift);
pmu_operate_status_t pmu_force_set_register_value(uint32_t address, uint32_t value);
bool pmu_get_chr_detect_value(void);
void pmu_enable_sw_lp_mode(pmu_power_domain_t domain, pmu_control_mode_t mode);
void pmu_switch_control_mode(pmu_power_domain_t domain, pmu_control_mode_t mode);
void pmu_lock_va18(int oper);
void pmu_charger_check_faston(void);
void pmu_enable_ocp(pmu_power_operate_t oper);
void pmu_enable_lpsd(void);
pmu_operate_status_t pmu_pwrkey_enable(pmu_power_operate_t oper);
/*==========[BUCK/LDO]==========*/
void pmu_enable_power(pmu_power_domain_t pmu_pdm, pmu_power_operate_t operate);
uint32_t pmu_get_power_status(pmu_power_domain_t pmu_pdm);
pmu_power_vcore_voltage_t pmu_get_vcore_voltage(void);
pmu_operate_status_t pmu_select_vsram_voltage(pmu_power_stage_t mode, pmu_power_vsram_voltage_t vol);
pmu_operate_status_t pmu_select_vcore_voltage(pmu_power_stage_t mode, pmu_power_vcore_voltage_t vol);
void pmu_set_audio_enhance(pmu_power_operate_t oper);
void pmu_get_lock_status(int sta);
bool pmu_get_pwrkey_state(void);
void pmu_power_off_sequence(pmu_power_stage_t stage);
uint8_t pmu_get_power_on_reason(void);
uint8_t pmu_get_power_off_reason(void);
void pmu_set_vaud18_voltage(pmu_power_vaud18_voltage_t oper);
/*==========[Charger]==========*/
void pmu_charger_init(uint16_t precc_cur,uint16_t cv_termination);
uint8_t pmu_enable_charger(uint8_t isEnableCharging);
bool pmu_set_icl_curent_level(uint8_t currentLevel);
uint8_t pmu_get_bc12_charger_type(void);
void pmu_set_charger_current_limit(uint8_t port);
void pmu_select_eco_option_operating(pmu_eoc_option_t opt,pmu_eoc_operating_t oper);
uint32_t pmu_get_charger_state(void);
void pmu_set_extend_charger_time(uint8_t timeMins);
void pmu_enable_recharger(bool isEnableCharging);
bool pmu_set_rechg_voltage(uint8_t isEnableRecharge);
void pmu_set_pre_charger_current(pmu_fastcc_chrcur_t cur);
void pmu_set_charger_current(pmu_fastcc_chrcur_t cur);
void pmu_set_iterm_current_irq(pmu_iterm_chrcur_t cur);
void pmu_set_iterm_current(pmu_iterm_chrcur_t cur);
bool pmu_select_cv_voltage(uint8_t voltage);
bool pmu_select_cc_safety_timer(uint8_t timeMHrs);
uint32_t pmu_disable_vsys_discharge(uint8_t value);
uint8_t pmu_get_usb_input_status(void);
bool pmu_select_precc_voltage(uint8_t voltage);
/*==========[HW-JEITA/NTC]==========*/
void pmu_hw_jeita_init(void);
uint8_t pmu_get_hw_jeita_status(void);
pmu_operate_status_t pmu_set_jeita_voltage(uint32_t auxadcVolt, uint8_t JeitaThreshold);
void pmu_jeita_state_setting(uint8_t state,pmu_jc_perecnt_level_t ICC_JC,pmu_cv_voltage_t vol);
void pmu_thermal_parameter_init(void);
uint32_t pmu_auxadc_get_pmic_temperature(void);
bool pmu_set_hw_jeita_enable(uint8_t value);
#ifdef AB1565
void pmu_auxadc_init(void);
#endif
#endif /* HAL_PMU_MODULE_ENABLED */
#endif
