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
#include "hal_pmu_ab2568_platform.h"
#include "hal_pmu_charger_2568.h"
#include "hal_pmu_auxadc_2568.h"
#include "hal_pmu_internal_2568.h"
#ifdef HAL_USB_MODULE_ENABLED
#include "hal_usb.h"
#include "hal_usb_internal.h"
#endif
#ifdef MTK_USB_DEMO_ENABLED
#include "usb.h"
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager.h"
#endif
uint8_t pmu_charger_sleep_handle;                                                                   /*DATA : sleep handle for PMIC*/
uint8_t pmu_charger_type;                                                                           /*DATA : restore charger type*/
uint16_t pmu_cc_cur[67]={0x0,0x10,0x50,0x90,0x130,0x150,0x170,0x190,0x220,0x230,                    /*DATA : charger current in CC value*/
                         0x310,0x350,0x390,0x430,0x450,0x470,0x490,0x520,0x530,0x540,
                         0x550,0x560,0x570,0x580,0x590,0x620,0x630,0x640,0x650,0x660,
                         0x670,0x680,0x690,0x712,0x720,0x722,0x730,0x732,0x740,0x742,
                         0x750,0x752,0x760,0x762,0x770,0x772,0x780,0x782,0x790,0x811,
                         0x812,0x813,0x820,0x821,0x822,0x823,0x830,0x831,0x832,0x833,
                         0x840,0x841,0x842,0x843,0x850,0x851,0x852};

uint16_t pmu_iterm[50]={0x0,0x10,0x50,0x90,0x130,0x150,0x170,0x190,0x220,0x230,                  /*DATA : charger current in iterm value*/
                           0x240,0x250,0x260,0x270,0x280,0x290,0x320,0x330,0x340,0x350,
                           0x360,0x370,0x380,0x390,0x414,0x420,0x424,0x430,0x434,0x440,
                           0x444,0x450,0x454,0x460,0x464,0x470,0x474,0x480,0x484,0x490,
                           0x514,0x520,0x524,0x530,0x534,0x540,0x544,0x550,0x554,580};
/*==========[Basic function]==========*/
void pmu_charger_init_2568(uint16_t precc_cur,uint16_t cv_termination){
    pmu_trim_ic_init();
    pmu_set_pre_charger_current_2568(precc_cur);
    pmu_set_iterm_current_2568(cv_termination);
    pmu_thermal_parameter_init();
}

void pmu_trim_ic_init(void) {
    /*check Reserved efuse bit*/
    if(!pmu_get_register_value_ab2568(PMU_AO_EFUSE_RSV, PMU_RG_AO_EFUSE_RSV_MASK,PMU_RG_AO_EFUSE_RSV_SHIFT)){
#ifndef MTK_battery_management_NTC_LESS
        pmu_set_register_value_ab2568(PMU_AUXADC_AD_JEITA_3, PMU_AUXADC_JEITA_VOLT_COOL_MASK, PMU_AUXADC_JEITA_VOLT_COOL_SHIFT, 4095);
        pmu_set_register_value_ab2568(PMU_AUXADC_AD_JEITA_4, PMU_AUXADC_JEITA_VOLT_COLD_MASK, PMU_AUXADC_JEITA_VOLT_COLD_SHIFT, 4095);
#endif
        log_hal_msgid_info("SW Trim\r\n", 0);
        pmu_set_register_value_ab2568(PMU_STRUP_CON6, PMU_RG_STRUP_AUXADC_START_SEL_MASK, PMU_RG_STRUP_AUXADC_START_SEL_SHIFT, 1);
        pmu_set_register_value_ab2568(PMU_STRUP_CON6, PMU_RG_STRUP_AUXADC_START_SW_MASK, PMU_RG_STRUP_AUXADC_START_SW_SHIFT, 0);
    } else {
        log_hal_msgid_info("IC has been Trim\r\n", 0);
    }
}

bool pmu_get_chr_detect_value_2568(void) {
    return pmu_get_register_value_ab2568(PMU_CHR_AO_DBG0, PMU_DA_QI_CHR_REF_EN_MASK, PMU_DA_QI_CHR_REF_EN_SHIFT);
}

/*Force disable VSYS discharge*/
uint32_t pmu_disable_vsys_discharge_2568(uint8_t value){
   return pmu_set_register_value_ab2568(PMU_CORE_CORE_AO_ELR_3, PMU_RG_EFUSE_PPFET_FON_RSV_TRIM_MASK, PMU_RG_EFUSE_PPFET_FON_RSV_TRIM_SHIFT, value);
}

void pmu_charger_check_faston_2568(void){
    uint32_t rg_74a = pmu_get_register_value_ab2568(0x74A, 0xFFFF, 0);
    if(rg_74a & 0x8100){
        log_hal_msgid_info("FAST_ON : %x charger state : %d", 2,rg_74a,pmu_get_charger_state_2568());
        pmu_enable_sysldo(0);
        hal_gpt_delay_ms(300);
        pmu_enable_sysldo(1);
        log_hal_msgid_info("After SYSLDO off/on charger state : %d", 1,pmu_get_charger_state_2568());
    }
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
bool pmu_select_cv_voltage_2568(uint8_t voltage) {
    return pmu_set_register_value_ab2568(PMU_CHR_AO_VCV, PMU_RG_VCV_VTH_MASK, PMU_RG_VCV_VTH_SHIFT, voltage);
}

/*Select the threshold of CC mode. (VBAT>THR enter CC mode)
 *3'b000: 2.40V
 *3'b001: 2.55V
 *3'b010: 2.70V
 *3'b011: 2.85V
 *3'b100: 3.00V (default)
 *3'b101: 3.15V
 *3'b110: 3.30V
 *3'b111: 3.45V*/
bool pmu_select_precc_voltage_2568(uint8_t voltage) {
    return pmu_set_register_value_ab2568(PMU_CORE_CORE_ANA_CON12, PMU_RG_PRECC_DET_MASK, PMU_RG_PRECC_DET_SHIFT, voltage);
}

/*CC safety timer selection, if change time out target, timer will be reset and re-count
 * 2'b00:3h
 * 2'b01:3h
 * 2'b10:4.5h
 * 2'b11:9h (default)
 */
bool pmu_select_cc_safety_timer_2568(uint8_t timeMHrs) {
    return pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON5, PMU_RG_CC_TMR_SEL_MASK, PMU_RG_CC_TMR_SEL_SHIFT, timeMHrs);
}

void pmu_set_charger_current_limit_2568(uint8_t port) {
    switch (port)
    {
        case SDP_CHARGER:
            pmu_set_register_value_ab2568(PMU_CHR_AO_ICL_0, PMU_RG_ICL_ITH_MASK, PMU_RG_ICL_ITH_SHIFT, ICL_ITH_400mA);
            break;
        case CDP_CHARGER:
            pmu_set_register_value_ab2568(PMU_CHR_AO_ICL_0, PMU_RG_ICL_ITH_MASK, PMU_RG_ICL_ITH_SHIFT, ICL_ITH_600mA);
            break;
        case DCP_CHARGER:
            pmu_set_register_value_ab2568(PMU_CHR_AO_ICL_0, PMU_RG_ICL_ITH_MASK, PMU_RG_ICL_ITH_SHIFT, ICL_ITH_800mA);
            break;
        default:
            pmu_set_register_value_ab2568(PMU_CHR_AO_ICL_0, PMU_RG_ICL_ITH_MASK, PMU_RG_ICL_ITH_SHIFT, ICL_ITH_400mA);
            break;
    }
    log_hal_msgid_info("Get ICL Current After setting:%d\r\n",1, pmu_get_register_value_ab2568(PMU_CHR_AO_ICL_0, PMU_RG_ICL_ITH_MASK,PMU_RG_ICL_ITH_SHIFT));
}

void pmu_control_pmic_protect(uint8_t tstWKeymode) {
    switch (tstWKeymode)
    {
        case TST_W_KEY_HW_MODE:
            pmu_set_register_value_ab2568(PMU_TESTKEY, PMU_TST_W_KEY_MASK,PMU_TST_W_KEY_SHIFT, 0);
            break;
        case TST_W_KEY_SW_MODE:
            pmu_set_register_value_ab2568(PMU_TESTKEY, PMU_TST_W_KEY_MASK,PMU_TST_W_KEY_SHIFT, 0x4936);
            break;
        default:
            break;
    }
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
uint32_t pmu_get_charger_state_2568(void) {
    return pmu_get_register_value_ab2568(PMU_LCHR_DIG_CON9,PMU_RGS_CHR_STATE_SW_MASK, PMU_RGS_CHR_STATE_SW_SHIFT);
}

/*==========[charger]==========*/
uint8_t pmu_enable_charger_2568(uint8_t isEnableCharging){
    uint8_t value=0;
    pmu_set_register_value_ab2568(0x730,0xffff,0,0x1);
    pmu_set_register_value_ab2568(PMU_CORE_CORE_ANA_CON9, PMU_RG_LOOP_CHRLDO_SB_DIS_MASK, PMU_RG_LOOP_CHRLDO_SB_DIS_SHIFT, 0x80);
    pmu_set_register_value_ab2568(PMU_CORE_CORE_ANA_CON8, PMU_RG_SYSDPM_STATUS_SEL_MASK, PMU_RG_SYSDPM_STATUS_SEL_SHIFT, 0);
    pmu_set_register_value_ab2568(PMU_CORE_CORE_ANA_CON8, PMU_RG_BUSDPM_DELTA_VTH_MASK, PMU_RG_BUSDPM_DELTA_VTH_SHIFT, 0x1);/*Set BUSDPM 200mV*/
    value= pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON4, PMU_RG_EN_CHR_MASK, PMU_RG_EN_CHR_SHIFT, isEnableCharging);
    return value;
}

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
bool pmu_set_icl_curent_level_2568(uint8_t currentLevel) {
    //bm_ctrl_info.icl_curent = currentLevel;
    return pmu_set_register_value_ab2568(PMU_CHR_AO_ICL_0, PMU_RG_ICL_ITH_MASK, PMU_RG_ICL_ITH_SHIFT, currentLevel);
}

/*iterm timing setting
 * 2'b00:dsiable iterm
 * 2'b01:15mins
 * 2'b10:30mins
 * 2'b11:60mins
*/
bool pmu_set_extend_charger_time_2568(uint8_t timeMins) {
    return pmu_set_register_value_ab2568(PMU_CHR_AO_ITERM, PMU_RG_T_ITERM_EXT_MASK, PMU_RG_T_ITERM_EXT_SHIFT, timeMins);
}

bool pmu_enable_recharger_2568(bool isEnableRecharge) {
    return pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON7,PMU_RG_EN_RECHG_MASK, PMU_RG_EN_RECHG_SHIFT, isEnableRecharge);
}

/*
 * recharging voltage select
 * 2'b00:VCV-50mV
 * 2'b01:VCV-100mV(default)
 * 2'b10:VCV-150mV
 * 2'b11:VCV-200mV
 * */
bool pmu_set_rechg_voltage_2568(uint8_t rechargeVoltage) {
    return pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON7, PMU_RG_VRECHG_MASK, PMU_RG_VRECHG_SHIFT, rechargeVoltage);
}

/*==========[HW-JEITA]==========*/
/* Enable the HW-JEITA hot cold control */
bool pmu_set_hw_jeita_enable_2568(uint8_t value) {
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_JEITA_0, PMU_AUXADC_JEITA_IRQ_EN_MASK, PMU_AUXADC_JEITA_IRQ_EN_SHIFT, value);
    return pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON0, PMU_RG_EN_HWJEITA_MASK, PMU_RG_EN_HWJEITA_SHIFT, value);
}

void pmu_hw_jeita_init_2568(void) {
    pmu_set_register_value_ab2568(PMU_CHR_AO_ICC_0,PMU_RG_TSTEP_ICC_MASK, PMU_RG_TSTEP_ICC_SHIFT, 0x2); //ICC soft start step time.default 16ms
    if (DIGITAL_THERMAL_FUNCTION)//digital thermal function needs to be enabled
    {
        //log_hal_msgid_info("Enable Digital Thermal Function\r\n", 0);
        /*Enable the digital thermal regulation for ICC*/
        pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON2, PMU_RG_EN_DIG_THR_MASK, PMU_RG_EN_DIG_THR_SHIFT, 0x1);
    }
}

pmu_operate_status_t pmu_set_jeita_voltage_2568(uint32_t auxadcVolt, uint8_t JeitaThreshold) {
    switch (JeitaThreshold)
    {
        case HW_JEITA_HOT_STAGE:
            return pmu_set_register_value_ab2568(PMU_AUXADC_AD_JEITA_1, PMU_AUXADC_JEITA_VOLT_HOT_MASK,PMU_AUXADC_JEITA_VOLT_HOT_SHIFT, auxadcVolt);
        break;

        case HW_JEITA_WARM_STAGE:
            return pmu_set_register_value_ab2568(PMU_AUXADC_AD_JEITA_2, PMU_AUXADC_JEITA_VOLT_WARM_MASK,PMU_AUXADC_JEITA_VOLT_WARM_SHIFT, auxadcVolt);
        break;

        case HW_JEITA_COOL_STAGE:
            return pmu_set_register_value_ab2568(PMU_AUXADC_AD_JEITA_3, PMU_AUXADC_JEITA_VOLT_COOL_MASK,PMU_AUXADC_JEITA_VOLT_COOL_SHIFT, auxadcVolt);
        break;

        case HW_JEITA_COLD_STAGE:
            return pmu_set_register_value_ab2568(PMU_AUXADC_AD_JEITA_4, PMU_AUXADC_JEITA_VOLT_COLD_MASK,PMU_AUXADC_JEITA_VOLT_COLD_SHIFT, auxadcVolt);
        break;
    }
    return PMU_OK;
}

/*WARM/COOL flag will not referenced by LCHR which decided by RG_DISWARMCOOL (default 1).
 * Set RG_DISWARMCOOL=0 when the user needs to use JEITA/WARM COOL*/
void pmu_set_jeita_state_setting_2568(uint8_t state,pmu_jc_perecnt_level_t ICC_JC,pmu_cv_voltage_t vol){
    switch(state){
        case HW_JEITA_NORMAL_STAGE:
            pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON0, PMU_RG_DISWARMCOOL_MASK, PMU_RG_DISWARMCOOL_SHIFT, 1);
            log_hal_msgid_info("Normal State will not be change\r\n", 0);
            break;
        case HW_JEITA_WARM_STAGE:
            pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON0, PMU_RG_DISWARMCOOL_MASK, PMU_RG_DISWARMCOOL_SHIFT, 0);
            pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON1, PMU_RG_VCV_JW_MASK, PMU_RG_VCV_JW_SHIFT, vol);
            hal_gpt_delay_us(50);
            pmu_set_register_value_ab2568(PMU_CHR_AO_ICC_0, PMU_RG_ICC_JW_MASK, PMU_RG_ICC_JW_SHIFT, ICC_JC);
            pmu_enable_charger_2568(PMU_ON);
            log_hal_msgid_info("HW JEITA Warm Setting, Set VCV voltage4.05V 100% ICC\r\n", 0);
            break;
        case HW_JEITA_COOL_STAGE:
            pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON0, PMU_RG_DISWARMCOOL_MASK, PMU_RG_DISWARMCOOL_SHIFT, 0);
            pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON1, PMU_RG_VCV_JC_MASK, PMU_RG_VCV_JC_SHIFT, vol);
            hal_gpt_delay_us(50);
            pmu_set_register_value_ab2568(PMU_CHR_AO_ICC_0, PMU_RG_ICC_JC_MASK, PMU_RG_ICC_JC_SHIFT, ICC_JC);
            pmu_enable_charger_2568(PMU_ON);
            log_hal_msgid_info("HW JEITA Cool Setting, Set VCV voltage4.2V 20% ICC\r\n", 0);
            break;
        case HW_JEITA_HOT_STAGE:
            log_hal_msgid_info("HW JEITA HOT Setting,Charger off\r\n", 0);
            break;
        case HW_JEITA_COLD_STAGE:
            log_hal_msgid_info("HW JEITA Cold Setting,Charger off\r\n", 0);
            break;
        default :
            log_hal_msgid_info("state error\r\n", 0);
            break;
    }
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
uint8_t pmu_get_hw_jeita_status_2568(void){
    uint8_t jeita_status=0;
    jeita_status |= (pmu_get_register_value_ab2568(PMU_AUXADC_AD_JEITA_1, PMU_AUXADC_JEITA_HOT_IRQ_MASK, PMU_AUXADC_JEITA_HOT_IRQ_SHIFT) << 0);
    jeita_status |= (pmu_get_register_value_ab2568(PMU_AUXADC_AD_JEITA_2, PMU_AUXADC_JEITA_WARM_IRQ_MASK, PMU_AUXADC_JEITA_WARM_IRQ_SHIFT) << 1);
    jeita_status |= (pmu_get_register_value_ab2568(PMU_AUXADC_AD_JEITA_3, PMU_AUXADC_JEITA_COOL_IRQ_MASK, PMU_AUXADC_JEITA_COOL_IRQ_SHIFT) << 2);
    jeita_status |= (pmu_get_register_value_ab2568(PMU_AUXADC_AD_JEITA_4, PMU_AUXADC_JEITA_COLD_IRQ_MASK, PMU_AUXADC_JEITA_COLD_IRQ_SHIFT) << 3);
    return jeita_status;
}

void pmu_set_pre_charger_current_2568(pmu_fastcc_chrcur_t cur) {
    pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON14, 0xfff, 0, pmu_cc_cur[cur]);
    log_hal_msgid_info("pree-cc charger current : %x",1,cur);
}

void pmu_set_charger_current_2568(pmu_fastcc_chrcur_t cur) {
    pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON15, 0xfff, 0, pmu_cc_cur[cur]);
    log_hal_msgid_info("fast-cc charger current : %x",1,cur);
}

void pmu_set_iterm_current_irq_2568(pmu_iterm_chrcur_t cur) {
    pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON16, 0x7ff, 0, pmu_iterm[cur]);
    log_hal_msgid_info(" iterm current irq : %x",1,cur);
}

void pmu_set_iterm_current_2568(pmu_iterm_chrcur_t cur) {
    pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON17, 0x7ff, 0, pmu_iterm[cur]);
    log_hal_msgid_info(" iterm current : %x",1,cur);
}

int pmu_get_charger_current_index(void) {
    int i =0;
    int cc_index =0;
    uint32_t temp_value =  pmu_get_register_value_ab2568(PMU_LCHR_DIG_CON15, 0xfff, 0);
    log_hal_msgid_info("Get =temp_value : %x",1,temp_value);
    for(i=0;i<67;i++){
        if(temp_value ==pmu_cc_cur[i]){
            cc_index =i;
            break;
        }
    }
    log_hal_msgid_info("confirm  index: %d temp_v:%x ,arr_v:%x",3,cc_index,temp_value,pmu_cc_cur[cc_index]);
    return cc_index;
}

/*==========[BC 1.2 behavior]==========*/

void pmu_bc12_init(void) {
    pmu_set_register_value_ddie(PMU_BC12_IBIAS_EN_V12_ADDR, PMU_BC12_IBIAS_EN_V12_MASK, PMU_BC12_IBIAS_EN_V12_SHIFT, 0x1);
    hal_gpt_delay_ms(10);
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 0);
}

void pmu_bc12_end(void) {
    pmu_set_register_value_ddie(PMU_BC12_IBIAS_EN_V12_ADDR, PMU_BC12_IBIAS_EN_V12_MASK, PMU_BC12_IBIAS_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 0);
}

uint8_t pmu_bc12_dcd(void) {
    uint8_t value = 0xFF;
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 2);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 2);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_DCD_EN_V12_ADDR, PMU_BC12_DCD_EN_V12_MASK, PMU_BC12_DCD_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 0);
    hal_gpt_delay_ms(10);
    value = pmu_get_register_value_ddie(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT);;
    hal_gpt_delay_ms(10);
    return value;
}

uint8_t pmu_bc12_primary_detction(void) {
    uint8_t compareResult = 0xFF;
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 2);
    pmu_set_register_value_ddie(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 0);
    hal_gpt_delay_ms(40);
    compareResult = pmu_get_register_value_ddie(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT);
    hal_gpt_delay_ms(20);
    return compareResult;
}

uint8_t pmu_bc12_secondary_detection(void) {
    uint8_t compareResult = 0xFF;
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 2);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 2);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 0);
    hal_gpt_delay_ms(40);
    compareResult = pmu_get_register_value_ddie(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT);
    hal_gpt_delay_ms(20);
    return compareResult;
}

uint8_t pmu_bc12_check_DCP(void) {
    uint8_t compareResult = 0xFF;
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 2);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 0);
    hal_gpt_delay_ms(10);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 0);
    hal_gpt_delay_us(200);
    compareResult = pmu_get_register_value_ddie(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT);
    return compareResult;
}

uint8_t pmu_bc12_check_dp(void) {
    uint8_t compareResult = 0xFF;
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 2);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 2);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 0);
    hal_gpt_delay_ms(10);
    compareResult = pmu_get_register_value_ddie(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT);
    hal_gpt_delay_ms(10);
    return compareResult;
}

uint8_t pmu_bc12_check_dm(void) {
    uint8_t compareResult = 0xFF;
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 0);
   // pmu_bc12_set_vref_vth_en_v12(0);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 0);
    hal_gpt_delay_ms(10);
    compareResult = pmu_get_register_value_ddie(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT);
    hal_gpt_delay_ms(10);
    return compareResult;
}

uint8_t pmu_bc12_check_floating(void) {
    uint8_t compareResult = 0xFF;
    pmu_set_register_value_ddie(PMU_BC12_SRC_EN_V12_ADDR, PMU_BC12_SRC_EN_V12_MASK, PMU_BC12_SRC_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_VREF_VTH_EN_V12_ADDR, PMU_BC12_VREF_VTH_EN_V12_MASK, PMU_BC12_VREF_VTH_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_CMP_EN_V12_ADDR, PMU_BC12_CMP_EN_V12_MASK, PMU_BC12_CMP_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_IPU_EN_V12_ADDR, PMU_BC12_IPU_EN_V12_MASK, PMU_BC12_IPU_EN_V12_SHIFT, 0);
    pmu_set_register_value_ddie(PMU_BC12_IPD_EN_V12_ADDR, PMU_BC12_IPD_EN_V12_MASK, PMU_BC12_IPD_EN_V12_SHIFT, 1);
    pmu_set_register_value_ddie(PMU_BC12_IPD_HALF_EN_V12_ADDR, PMU_BC12_IPD_HALF_EN_V12_MASK, PMU_BC12_IPD_HALF_EN_V12_SHIFT, 1);
    hal_gpt_delay_ms(10);
    compareResult = pmu_get_register_value_ddie(PMU_AQ_QI_BC12_CMP_OUT_V12_ADDR, PMU_AQ_QI_BC12_CMP_OUT_V12_MASK, PMU_AQ_QI_BC12_CMP_OUT_V12_SHIFT);
    hal_gpt_delay_ms(10);
    return compareResult;
}

uint8_t pmu_get_bc12_charger_type_2568(void) {
    pmu_bc12_init();
    if (pmu_bc12_dcd() == 0) {                       //SDP/CDP/DCP/S charger
        if (pmu_bc12_primary_detction() == 0) {      //SDP
            pmu_bc12_end();
            log_hal_msgid_info("BC12 SDP Charger\r\n", 0);
            pmu_charger_type=SDP_CHARGER;
            return SDP_CHARGER;
        } else {                                                    //CDP/DCP/S charger
            if(pmu_bc12_secondary_detection()==0){   //CDP
                pmu_bc12_end();
                log_hal_msgid_info("BC12 CDP Charger\r\n", 0);
                pmu_charger_type=CDP_CHARGER;
                return CDP_CHARGER;
            }else{
                 if(pmu_bc12_check_DCP()==0){          //DCP
                     pmu_bc12_end();
                     log_hal_msgid_info("BC12 DCP Charger\r\n", 0);
                     pmu_charger_type=DCP_CHARGER;
                     return DCP_CHARGER;
                 }else{                                             //S charger
                     pmu_bc12_end();
                     log_hal_msgid_info("BC12 SS Charger\r\n", 0);
                     pmu_charger_type=SS_TABLET_CHARGER;
                     return SS_TABLET_CHARGER;
                 }
            }
        }
    } else {                                                        //apple /non-standard/ DP&DM floating charger
        uint8_t dp_out, dm_out;
        dp_out = pmu_bc12_check_dp();
        dm_out = pmu_bc12_check_dm();
        if((dp_out==0)&&(dm_out==0)){
            if(pmu_bc12_check_floating()==0){                 //DP&DM floating charger
                pmu_bc12_end();
                log_hal_msgid_info("BC12 DP&DM Floating\r\n", 0);
                pmu_charger_type=DP_DM_FLOATING;
                return DP_DM_FLOATING;
            }else{                                                      //non-standard charger
                pmu_bc12_end();
                log_hal_msgid_info("BC12 NON-STD charger\r\n", 0);
                pmu_charger_type=NON_STD_CHARGER;
                return NON_STD_CHARGER;
            }
        }else if((dp_out==0)&&(dm_out==1)){                          //apple 5V 1A charger
                pmu_bc12_end();
                log_hal_msgid_info("BC12 IPHONE_5V_1A Charger\r\n", 0);
                pmu_charger_type=IPHONE_5V_1A_CHARGER;
                return IPHONE_5V_1A_CHARGER;
        }else{                                                      //apple ipad2/ipad4 charger
            pmu_bc12_end();
                log_hal_msgid_info("BC12 IPAD2_IPAD4 Charger\r\n", 0);
                pmu_charger_type=IPAD2_IPAD4_CHARGER;
            return IPAD2_IPAD4_CHARGER;
        }
    }
}

/*==========[ECO setting]==========*/
/*
 * Reduce IBUS
 * 0: No reduce
 * 1: disable PREG, CP, and SYS LDO
*/
bool pmu_set_ibus_low_en(uint8_t value) {
    return pmu_set_register_value_ab2568(PMU_TPO_CON5, PMU_RG_LOW_IBUS_EN_MASK, PMU_RG_LOW_IBUS_EN_SHIFT, value);
}

uint32_t pmu_check_vbus_gt_por(void) {
    return pmu_get_register_value_ab2568(PMU_CHR_AO_CON0, PMU_AD_QI_VBUS_GT_POR_MASK, PMU_AD_QI_VBUS_GT_POR_SHIFT);
}
/*check HW DA_QI_VBUS_UVLO_DB*/
uint32_t pmu_get_vbus_uvlo_db(void) {
    return pmu_get_register_value_ab2568(PMU_CHR_AO_DBG0, PMU_DA_QI_VBUS_UVLO_DB_MASK, PMU_DA_QI_VBUS_UVLO_DB_SHIFT);
}

/*check HW DA_QI_VBUS_OVP_DB*/
uint32_t pmu_get_vbus_ovp_db(void) {
    return pmu_get_register_value_ab2568(PMU_CHR_AO_DBG0, PMU_DA_QI_VBUS_OVP_DB_MASK, PMU_DA_QI_VBUS_OVP_DB_SHIFT);
}


/*Recharge low battery detection voltage detection enable
 * 0:disable
 * 1:enable */
bool pmu_control_recharge_low_bat(uint8_t value) {
    return pmu_set_register_value_ab2568(PMU_AUXADC_AD_VBAT2, PMU_AUXADC_VBAT_EN_ADC_RECHG_MASK, PMU_AUXADC_VBAT_EN_ADC_RECHG_SHIFT, value);
}

/*Recharge mode in low battery detection voltage detection enable*/
bool pmu_control_vbat_en_mode_sel(uint8_t value) {
    return pmu_set_register_value_ab2568(PMU_AUXADC_AD_VBAT2, PMU_AUXADC_VBAT_EN_MODE_SEL_MASK, PMU_AUXADC_VBAT_EN_MODE_SEL_SHIFT, value);
}

/*charger suspend mode control */
bool pmu_charger_suspend_mode(uint8_t value) {
    return pmu_set_register_value_ab2568(PMU_TPO_CON4, PMU_RG_CHR_SUSPEND_MODE_MASK, PMU_RG_CHR_SUSPEND_MODE_SHIFT, value);
}

/*clear AUXADC_ADC_RDY_WAKEUP_PCHR & AUXADC_ADC_RDY_WAKEUP_SWCHR
 * 1'b0: none
 * 1'b1: clear*/
bool pmu_auxadc_wakeup_clr(uint8_t value) {
    return pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON0,PMU_AUXADC_ADC_RDY_WAKEUP_CLR_MASK, PMU_AUXADC_ADC_RDY_WAKEUP_CLR_SHIFT, value);
}

/*
 * Rduce Iq at normal mode without BUS Plug-in
 * 0: No reduce Iq
 * 1: Disable BATOC and FastOn Comp to reduce Iq
* */
bool pmu_set_ppfet_lp_mode(uint8_t value) {
    return pmu_set_register_value_ab2568(PMU_CORE_CORE_ANA_AO_CON0, PMU_RG_PPFET_CTRL_LP_MODE_MASK, PMU_RG_PPFET_CTRL_LP_MODE_SHIFT, value);
}

bool pmu_set_plugin_db_sw_sel(uint8_t value) {
    return pmu_set_register_value_ab2568(PMU_CHR_AO_CON0, PMU_RG_CHR_PLUGIN_DB_SW_SEL_MASK, PMU_RG_CHR_PLUGIN_DB_SW_SEL_SHIFT, value);
}

/*==========[ECO operating]==========*/
void pmu_select_eco_option_operating_2568(pmu_eoc_option_t opt, pmu_eoc_operating_t oper) {
    switch (oper)
    {
        case option_setting:
            pmu_eoc_option_setting(opt);
            break;
        case option2_init:
            pmu_charger_eoc2_setting();
            break;
        case option2_recharger:
            pmu_charger_eoc2_recharger_interrupt();
            break;
        case option2_exit:
            pmu_charger_eoc2_exit_setting();
            break;
        case option3_init:
            pmu_charger_eoc3_setting();
            break;
        case option3_checking:
            pmu_charger_eoc3_checking();
            break;
        case option3_recharger:
            pmu_charger_eoc3_recharger_setting();
            break;
        case option3_exit:
            pmu_charger_eoc3_exit_setting();
            break;
        case option4_init:
            pmu_charger_eoc4_setting();
            break;
        case option4_exit:
            pmu_charger_eoc4_exit();
            break;
    }
}

void pmu_charger_eoc2_exit_setting(void) {
    if (pmu_get_register_value_ab2568(PMU_CHR_AO_CON0, PMU_DA_QI_CHR_REF_EN_MASK, PMU_DA_QI_CHR_REF_EN_SHIFT) == 0) {
        hal_gpt_delay_ms(2);
        if (pmu_get_register_value_ab2568(PMU_CHR_AO_CON0, PMU_AD_QI_VBUS_GT_POR_MASK, PMU_AD_QI_VBUS_GT_POR_SHIFT)!= 0) {
            log_hal_msgid_info("EOC2 ad_qi_vbus_gt_por has issue\r\n", 0);
        }
        pmu_set_register_value_ab2568(PMU_AUXADC_AD_VBAT2, PMU_AUXADC_VBAT_EN_ADC_RECHG_MASK, PMU_AUXADC_VBAT_EN_ADC_RECHG_SHIFT, 0);
        pmu_set_register_value_ab2568(PMU_AUXADC_AD_VBAT2, PMU_AUXADC_VBAT_EN_MODE_SEL_MASK, PMU_AUXADC_VBAT_EN_MODE_SEL_SHIFT, 1);

        pmu_rstb_select(0);
        pmu_control_pmic_protect(TST_W_KEY_HW_MODE);
    }
    pmu_set_hw_jeita_enable_2568(PMU_OFF);

}

void pmu_charger_eoc2_recharger_interrupt(void) {
    pmu_srclken_control_mode_2568(1);
    pmu_set_ibus_low_en(0);
    hal_gpt_delay_ms(2);
    if (pmu_check_vbus_gt_por() != 1) {
        log_hal_msgid_info("EOC2 has problem\r\n", 0);
    } else {
        pmu_enable_sysldo(1);
        pmu_control_recharge_low_bat(0);
        pmu_control_vbat_en_mode_sel(1);
        pmu_rstb_select(0);
        pmu_control_pmic_protect(TST_W_KEY_HW_MODE);
    }
}

void pmu_charger_eoc2_setting(void)
{
    pmu_control_pmic_protect(TST_W_KEY_SW_MODE);
    pmu_srclken_control_mode_2568(1);
    pmu_set_register_value_ab2568(PMU_INT_CON0, PMU_RG_INT_EN_VBAT_RECHG_MASK, PMU_RG_INT_EN_VBAT_RECHG_SHIFT,1);
    pmu_set_register_value_ab2568(PMU_INT_MASK_CON0, PMU_RG_INT_MASK_VBAT_RECHG_MASK, PMU_RG_INT_MASK_VBAT_RECHG_SHIFT,0);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_VBAT2, PMU_AUXADC_VBAT_IRQ_EN_MASK, PMU_AUXADC_VBAT_IRQ_EN_SHIFT, 1);

    if (pmu_get_status_interrupt(RG_INT_VBAT_RECHG) == 1) {
        pmu_clear_interrupt(RG_INT_VBAT_RECHG);
    }
    pmu_control_recharge_low_bat(1);
    pmu_control_vbat_en_mode_sel(0);
    pmu_rstb_select(1);
    pmu_enable_sysldo(0);
}

void pmu_charger_eoc3_exit_setting(void) {
    pmu_enable_power(PMU_LDO_VA18, PMU_ON);
    pmu_enable_power(PMU_LDO_VLDO33, PMU_ON);
    pmu_charger_suspend_mode(0);
    pmu_set_ibus_low_en(0);
    hal_gpt_delay_ms(2);
    pmu_set_ppfet_lp_mode(0);
    pmu_set_plugin_db_sw_sel(0);
    pmu_control_pmic_protect(TST_W_KEY_HW_MODE);
    pmu_set_hw_jeita_enable(PMU_OFF);
#ifdef HAL_SLEEP_MANAGER_ENABLED
    if(hal_sleep_manager_is_sleep_handle_alive(pmu_charger_sleep_handle)>=1){
        hal_sleep_manager_unlock_sleep(pmu_charger_sleep_handle);
        log_hal_msgid_info("Battery Unlock sleep\r\n", 0);
    }
    log_hal_msgid_info("PMU sleep handle %d\r\n", 1,hal_sleep_manager_is_sleep_handle_alive(pmu_charger_sleep_handle));
#endif
}

void pmu_charger_eoc3_recharger_setting(void) {
    pmu_enable_power(PMU_LDO_VA18, PMU_ON);
    pmu_enable_power(PMU_LDO_VLDO33, PMU_ON);
    hal_gpt_delay_ms(1);
    pmu_charger_suspend_mode(0);
    hal_gpt_delay_ms(10); // interrupt delay use gpt
    pmu_set_ppfet_lp_mode(0);
    pmu_set_ibus_low_en(0);
    hal_gpt_delay_ms(2);
    pmu_enable_sysldo(1);
    if (pmu_check_vbus_gt_por() == 1 && pmu_get_vbus_uvlo_db() == 0 && pmu_get_vbus_ovp_db() == 0) {
        pmu_set_plugin_db_sw_sel(0);
        pmu_control_pmic_protect(TST_W_KEY_HW_MODE);
    } else {
        log_hal_msgid_info("EOC3 recharge setting fail[%d][%d][%d]\r\n", 3,pmu_check_vbus_gt_por() ,pmu_get_vbus_uvlo_db(),pmu_get_vbus_ovp_db());
    }
}

void pmu_charger_eoc3_checking(void) {
#ifdef HAL_SLEEP_MANAGER_ENABLED
    if (!hal_sleep_manager_is_sleep_handle_alive(pmu_charger_sleep_handle)) {
        hal_sleep_manager_lock_sleep(pmu_charger_sleep_handle);
    }
#endif
    pmu_enable_power(PMU_LDO_VA18, PMU_ON);
    pmu_enable_power(PMU_LDO_VLDO33, PMU_ON);
    hal_gpt_delay_ms(10);
    if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) < 4100) // recharger Note
    {
        pmu_enable_sysldo(1);
#ifdef HAL_USB_MODULE_ENABLED
        hal_usb_phy_preinit();
#endif
        log_hal_msgid_info("Charger Option3 Recharge\r\n", 0);
        pmu_charger_eoc3_recharger_setting();
#ifdef MTK_USB_DEMO_ENABLED
    usb_cable_detect();
#else
#ifdef HAL_USB_MODULE_ENABLED
    hal_usb_save_current();
#endif
#endif
    } else {
        pmu_enable_sysldo(0);
        log_hal_msgid_info("Charger Option3 Not Recharge\r\n", 0);
        //pmu_auxadc_wakeup_clr(1); /*ZCV will auto re-calc when SRCLKEN high/low */
        pmu_auxadc_wakeup_clr(0);
        hal_gpt_delay_ms(100);
        if ((pmu_charger_type != SDP_CHARGER) && (pmu_charger_type != CDP_CHARGER)) {
#ifdef MTK_USB_DEMO_ENABLED
            ap_usb_deinit();
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
            if (hal_sleep_manager_is_sleep_handle_alive(pmu_charger_sleep_handle) >= 1) {
                hal_sleep_manager_unlock_sleep(pmu_charger_sleep_handle);
            }
            log_hal_msgid_info("PMU sleep handle %d\r\n", 1,hal_sleep_manager_is_sleep_handle_alive(pmu_charger_sleep_handle));
#endif
        }
    }
}

void pmu_charger_eoc3_setting(void) {
    pmu_control_pmic_protect(TST_W_KEY_SW_MODE);
    pmu_set_register_value_ab2568(PMU_STRUP_CON6,PMU_RG_STRUP_AUXADC_RPCNT_MAX_MASK, PMU_RG_STRUP_AUXADC_RPCNT_MAX_SHIFT, 66);

    pmu_set_plugin_db_sw_sel(1);
    //pmu_auxadc_wakeup_clr(1);  /*ZCV will auto re-calc when SRCLKEN high/low */
    pmu_auxadc_wakeup_clr(0);
}

void pmu_charger_eoc4_setting(void){
    pmu_control_pmic_protect(TST_W_KEY_SW_MODE);
    pmu_set_plugin_db_sw_sel(1);
}

void pmu_charger_eoc4_exit(void) {
    if (pmu_get_register_value_ab2568(PMU_PONSTS, PMU_STS_SPAR_MASK, PMU_STS_SPAR_SHIFT)) {
        pmu_set_register_value_ab2568(PMU_TPO_CON5, PMU_RG_LOW_IBUS_EN_MASK, PMU_RG_LOW_IBUS_EN_SHIFT, 0);
        pmu_set_register_value_ab2568(PMU_TPO_CON5, PMU_RG_LOW_IBUS_EN_LATCH_MASK, PMU_RG_LOW_IBUS_EN_LATCH_SHIFT, 0x1);
        pmu_set_register_value_ab2568(PMU_TPO_CON5, PMU_RG_LOW_IBUS_EN_LATCH_MASK, PMU_RG_LOW_IBUS_EN_LATCH_SHIFT, 0);
        log_hal_msgid_info("EOC RTC MODE alarm", 0);
    }
}

void pmu_eoc_option_setting(pmu_eoc_option_t opt) {
    if (opt == pmu_eoc_option2) {
        if (pmu_get_faston_flag() == 1) {
            pmu_set_register_value_ab2568(PMU_LCHR_DIG_DEBUG4,PMU_RG_SYS_DISCHR_EN_SW_MASK, PMU_RG_SYS_DISCHR_EN_SW_SHIFT, 0);
            pmu_set_register_value_ab2568(PMU_LCHR_DIG_DEBUG4,PMU_RG_SYS_DISCHR_EN_SW_SEL_MASK, PMU_RG_SYS_DISCHR_EN_SW_SEL_SHIFT, 0);

            pmu_set_ibus_low_en(1);
            hal_gpt_delay_ms(2);
            if (pmu_check_vbus_gt_por() != 0 || pmu_get_vbus_uvlo_db() != 0 || pmu_get_vbus_ovp_db() != 0) {
                log_hal_msgid_info("EOC2 EOC setting fail\r\n", 0);
            } else {
                log_hal_msgid_info("option2 Start To Sleep\r\n", 0);
                pmu_clear_interrupt(PMU_INT_MAX);
            }
        }
    } else if (opt == pmu_eoc_option3) {
        if (pmu_get_faston_flag() == 1) {
            pmu_set_ibus_low_en(1);
        } else {
            log_hal_msgid_info("3 EOC setting fail\r\n", 0);
        }
        pmu_set_ppfet_lp_mode(1);
        hal_gpt_delay_ms(2);
        if (pmu_check_vbus_gt_por() == 0 && pmu_get_faston_flag() == 0 && pmu_get_vbus_uvlo_db() == 0 && pmu_get_vbus_ovp_db() == 0) {
            log_hal_msgid_info("Option3 Ready to Sleep", 0);
            pmu_charger_suspend_mode(1);
            hal_gpt_delay_ms(1);
            log_hal_msgid_info("Option3 srclken", 0);
            pmu_clear_interrupt(PMU_INT_MAX);
#ifdef HAL_SLEEP_MANAGER_ENABLED
            if(hal_sleep_manager_is_sleep_handle_alive(pmu_charger_sleep_handle)>=1){
                hal_sleep_manager_unlock_sleep(pmu_charger_sleep_handle);
            }
            log_hal_msgid_info("PMU sleep handle %d\r\n", 1,hal_sleep_manager_is_sleep_handle_alive(pmu_charger_sleep_handle));
#endif
        } else {
            log_hal_msgid_info("EOC3 EOC setting fail\r\n", 0);
        }
    } else if (opt == pmu_eoc_option4) {
        if (pmu_get_faston_flag() == 1) {
            pmu_set_ibus_low_en(1);
            pmu_set_register_value_ab2568(PMU_TPO_CON5, PMU_RG_LOW_IBUS_EN_LATCH_MASK, PMU_RG_LOW_IBUS_EN_LATCH_SHIFT, 0x1);
            pmu_set_register_value_ab2568(PMU_TPO_CON5, PMU_RG_LOW_IBUS_EN_LATCH_MASK, PMU_RG_LOW_IBUS_EN_LATCH_SHIFT, 0x0);
        } else {
            log_hal_msgid_info("EOC3 EOC setting fail\r\n", 0);
        }
        pmu_set_ppfet_lp_mode(1);
        hal_gpt_delay_ms(2);
        pmu_set_register_value_ab2568(PMU_TPO_CON6, PMU_RG_EOC_RTC_6M_OFF_MASK, PMU_RG_EOC_RTC_6M_OFF_SHIFT, 0x1);
        pmu_set_register_value_ab2568(PMU_TPO_CON6, PMU_RG_EOC_RTC_IVGEN_OFF_MASK, PMU_RG_EOC_RTC_IVGEN_OFF_SHIFT, 0x1);
        pmu_set_register_value_ab2568(PMU_TPO_CON6, PMU_RG_EOC_RTC_EN_MASK, PMU_RG_EOC_RTC_EN_SHIFT, 0x1);
        hal_rtc_enter_rtc_mode();
    }else {
        log_hal_msgid_info("EOC option setting fail  %d \r\n",0);
    }

}
/*SYSLDO enable bit*/
bool pmu_enable_sysldo(bool isEnableSysdo) {
    return pmu_set_register_value_ab2568(PMU_CHR_AO_SYSLDO, PMU_RG_SYSLDO_EN_MASK, PMU_RG_SYSLDO_EN_SHIFT, isEnableSysdo);
}

/*==========[Other]==========*/
uint32_t pmu_set_charger_parameter_2568(pmu_charger_parameter_t index,uint32_t value){
    switch (index){
        case IBAT_TUNE_TRIM:
            if((value==0)||((value>=4)&&(value<=7))){
                return pmu_set_register_value_ab2568(PMU_LCHR_DIG_DEBUG7,PMU_RG_IBAT_TUNE_TRIM_MASK, PMU_RG_IBAT_TUNE_TRIM_SHIFT, value);
            }else{
                return PMU_ERROR;
            }
        break;
    }
    return 1;
}

void pmu_core_pmu_turbo_the_timer(uint8_t isEnable) { //LCHR 1S HIT Speed up for timer (for debug)
    if (isEnable) {
        pmu_control_pmic_protect(TST_W_KEY_SW_MODE);
        pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON10, PMU_RG_LCHR_1S_SPEEDUP_MASK, PMU_RG_LCHR_1S_SPEEDUP_SHIFT, 1);
    } else {
        pmu_control_pmic_protect(TST_W_KEY_HW_MODE);
        pmu_set_register_value_ab2568(PMU_LCHR_DIG_CON10, PMU_RG_LCHR_1S_SPEEDUP_MASK, PMU_RG_LCHR_1S_SPEEDUP_SHIFT, 0);
    }
}

bool pmu_rstb_select(uint8_t value) {
    return pmu_set_register_value_ab2568(PMU_STRUP_CON6, PMU_RG_STRUP_AUXADC_RSTB_SEL_MASK, PMU_RG_STRUP_AUXADC_RSTB_SEL_SHIFT, value);
}
/*check HW DD_QI_FASTON_FLAG_DB*/
uint32_t pmu_get_faston_flag(void) {
    return pmu_get_register_value_ab2568(PMU_LCHR_DIG_DEBUG1, PMU_DD_QI_FASTON_FLAG_DB_MASK, PMU_DD_QI_FASTON_FLAG_DB_SHIFT);
}

uint32_t pmu_get_vsys_dpm_status(void) {
    return pmu_get_register_value_ab2568(PMU_LCHR_DIG_DEBUG1, PMU_AD_QI_SYSDPM_MODE_MASK, PMU_AD_QI_SYSDPM_MODE_SHIFT);
}
uint32_t pmu_get_vbus_dpm_status(void) {
    return pmu_get_register_value_ab2568(PMU_LCHR_DIG_DEBUG1, PMU_AD_QI_VBUSDPM_MODE_MASK, PMU_AD_QI_VBUSDPM_MODE_SHIFT);
}

#endif /* HAL_PMU_MODULE_ENABLED */
