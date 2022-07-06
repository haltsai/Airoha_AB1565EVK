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
#include "hal_pmu.h"
#ifdef HAL_PMU_MODULE_ENABLED
#include "hal_pmu_ab2565_platform.h"
#include "hal_pmu_auxadc_2565.h"
#include "hal_pmu_charger_2565.h"
#include "hal_pmu_internal_2565.h"
#include "hal_pmu_cali_2565.h"

#include "hal_usb.h"
#include "hal_usb_internal.h"
#include "hal_sleep_manager_platform.h"
#include "hal_sleep_manager_internal.h"
#include "nvkey_id_list.h"
#include "assert.h"
#include <stdio.h>
#include <string.h>

#define UNUSED(x)  ((void)(x))
#define EOC_EN         (1<<12)
#define CHGFLO_B_DEB   (1<<13)
#define CHG_PLUG_DEB   (1<<11)
#define CHGFLO_B_E2    (1<<1)
#define CHG_PLUG_E2    (1<<0)
#define CHG_DBG_CHK     1

#define CHG_GAUGE_EN    0

uint8_t pmu_charger_type; /*DATA : restore charger type*/
#ifndef AIR_PMU_DISABLE_CHARGER
uint8_t pmu_chg_intr_flag = 0;
PMU_CHG_INFO pmu_chg_info;
uint32_t chg_timer;
uint8_t pfm_flag = 0xFF;
pmu_chg_cc_rsel_t pmu_chg_cc_config;

JEITA_COOL pmu_chg_jeita_cool;
JEITA_WARM pmu_chg_jeita_warm;
INT_CHG_DAC_CONFIG pmu_chg_dac;
INT_CHG_CC1_CURRENT_CONFIG pmu_chg_cc1;
INT_CHG_CC2_CURRENT_CONFIG pmu_chg_cc2;
CHG_ADC_CONFIG pmu_chg_adc;
CHG_CV_STOP_CURRENT_ADC pmu_chg_cv_stop_curr;

extern int32_t mpk_round(int32_t val1, int32_t val2);
extern void captouch_set_vbus_flag(bool is_chgin);

/*==========[Basic function]==========*/
#if (CHG_DBG_CHK)
uint32_t chg_chk_timer;
void pmu_charger_check_timer_callback(void *user_data)
{
    UNUSED(user_data);
    pmu_auxadc_get_channel_value(PMU_AUX_VCHG);
    pmu_auxadc_get_channel_value(PMU_AUX_VBAT);
    log_hal_msgid_info("pmu_charger_check_timer_callback, rg_320[0x%X], rg_322[0x%X], rg_606[0x%X], rg_608[0x%X], rg_60A[0x%X]", 5,
    pmu_get_register_value_2565(0x320, 0xFFFF, 0), pmu_get_register_value_2565(0x322, 0xFFFF, 0), pmu_get_register_value_2565(0x606, 0xFFFF, 0),
    pmu_get_register_value_2565(0x608, 0xFFFF, 0), pmu_get_register_value_2565(0x60A, 0xFFFF, 0));
    pmu_bat_3v3_proc();

    hal_gpt_sw_start_timer_ms(chg_chk_timer, 5000,
        (hal_gpt_callback_t)pmu_charger_check_timer_callback, NULL);
}
#endif
#endif

void pmu_charger_init_2565(uint16_t precc_cur,uint16_t cv_termination){
#ifdef AIR_PMU_DISABLE_CHARGER
#ifdef AIR_USB_DONGLE_PROJECT_ENABLE
    pmu_force_set_register_value_2565(0x320, pmu_get_register_value_2565(0x320, 0xFFFF, 0) & 0x1F5A);
    log_hal_msgid_info("pmu_charger_init_2565, compl, rechg intr mask", 0);
#endif
    log_hal_msgid_info("pmu_charger_init_2565, bypass", 0);
    return;
#else
    log_hal_msgid_info("pmu_charger_init_2565 enter", 0);

    pmu_set_register_value_2565(FULL_BAT_THRESHOLD1_ADDR, FULL_BAT_THRESHOLD1_MASK, FULL_BAT_THRESHOLD1_SHIFT, 0x3FF);
    pmu_set_register_value_2565(FULL_BAT_THRESHOLD2_ADDR, FULL_BAT_THRESHOLD2_MASK, FULL_BAT_THRESHOLD2_SHIFT, 0x3FF);
    pmu_set_register_value_2565(BYP_CV_CHK_VBAT_ADDR, BYP_CV_CHK_VBAT_MASK, BYP_CV_CHK_VBAT_SHIFT, 0x1);
    pmu_set_register_value_2565(CHG_COMPLETE_CHK_NUM_ADDR, CHG_COMPLETE_CHK_NUM_MASK, CHG_COMPLETE_CHK_NUM_SHIFT, 0x9);

    pmu_chg_deb_time(PMU_CHG_OUT_DEB_10MS, PMU_CHG_IN_DEB_40MS);
    pmu_set_register_value_2565(CHG_FORCE_OFF_ADDR, CHG_FORCE_OFF_MASK, CHG_FORCE_OFF_SHIFT, PMU_OFF);

    hal_gpt_status_t gpt_status = hal_gpt_sw_get_timer(&chg_timer);
    if (gpt_status != HAL_GPT_STATUS_OK)
    {
        log_hal_msgid_error("hal_gpt_sw_get_timer fail, status[%d]", 1, gpt_status);
    }
#if (CHG_DBG_CHK)
    gpt_status = hal_gpt_sw_get_timer(&chg_chk_timer);
    if (gpt_status == HAL_GPT_STATUS_OK)
    {
        hal_gpt_sw_start_timer_ms(chg_chk_timer, 5000,
            (hal_gpt_callback_t)pmu_charger_check_timer_callback, NULL);
    }
    else
    {
        log_hal_msgid_error("hal_pmu_chg_chk_timer fail, status[%d]", 1, gpt_status);
    }
#endif
#endif
}

/*==========[charger]==========*/
uint8_t pmu_enable_charger_2565(uint8_t isEnableCharging){
    uint8_t value = isEnableCharging;
#ifdef AIR_PMU_DISABLE_CHARGER
    log_hal_msgid_info("pmu_enable_charger_2565, bypass", 0);
#else
    if (isEnableCharging)
    {
        pmu_set_register_value_2565(CHG_FORCE_OFF_ADDR, CHG_FORCE_OFF_MASK, CHG_FORCE_OFF_SHIFT, 0);
    }
    else
    {
        pmu_set_register_value_2565(CHG_FORCE_OFF_ADDR, CHG_FORCE_OFF_MASK, CHG_FORCE_OFF_SHIFT, 1);
    }
    log_hal_msgid_info("pmu_enable_charger_2565, oper[%d]", 1, isEnableCharging);
#endif
    return value;
}

#ifndef AIR_PMU_DISABLE_CHARGER
void pmu_eoc_ctrl (pmu_power_operate_t oper)
{
    uint16_t rg_320, rg_deb;
    bool vchg1, vchg2;

    vchg1 = pmu_charger_is_plug();

    rg_320 = pmu_get_register_value_2565(CHARGER_CON1, 0xFFFF, 0);

    rg_deb = rg_320 & 0xF00;
    rg_320 &= 0x1F00;
    pmu_force_set_register_value_2565(CHARGER_CON1, rg_320);

    rg_320 &= 0x1000;
    pmu_force_set_register_value_2565(CHARGER_CON1, rg_320);

    rg_320 = (oper << 12);
    pmu_force_set_register_value_2565(CHARGER_CON1, rg_320);
    hal_gpt_delay_us(250);

    rg_320 |= rg_deb;
    pmu_force_set_register_value_2565(CHARGER_CON1, rg_320);

    rg_320 |= 0xAA;
    pmu_force_set_register_value_2565(CHARGER_CON1, rg_320);

    vchg2 = pmu_charger_is_plug();

    if (vchg1 != vchg2)
    {
        if (vchg2)
        {
            log_hal_msgid_warning("pmu_eoc_ctrl, add chg in intr", 0);
            pmu_chg_intr_flag |= CHG_INTR_IN;
            pmu_charger_int_in();

        }
        else
        {
            log_hal_msgid_warning("pmu_eoc_ctrl, add chg out intr", 0);
            pmu_chg_intr_flag |= CHG_INTR_OUT;
            pmu_charger_int_out();
        }
    }

    log_hal_msgid_info("pmu_eoc_ctrl, oper[%d], rg_320[0x%X], reg_60A[0x%X], vchg1[%d], vchg2[%d]", 5,
    oper, rg_320, pmu_get_register_value_2565(0x60A, 0xFFFF, 0), vchg1, vchg2);
}

pmu_operate_status_t pmu_chg_deb_time(pmu_chg_deb_time_t out_deb_time, pmu_chg_deb_time_t in_deb_time)
{
    uint16_t reg_320, deb_time;

    deb_time = (out_deb_time << 10) + (in_deb_time << 8);
    reg_320 = pmu_get_register_value_2565(CHARGER_CON1, 0xFFFF, 0);
    reg_320 &= 0x10AA;
    reg_320 |= deb_time;
    pmu_force_set_register_value_2565(CHARGER_CON1, reg_320);

    return PMU_OK;
}
#endif
bool pmu_charger_is_plug(void)
{
    uint16_t rg_chg, result = 0;
    uint16_t rg_320 = pmu_get_register_value_2565(CHARGER_CON1, 0xFFFF, 0);

    if ((adie_version == PMU_ECO3) || (adie_version == PMU_NO_FTK))
    {
        rg_chg = pmu_get_register_value_2565(PMU_TEST5, 0xFFFF, 0);
        if (rg_320 & EOC_EN)
            result = rg_chg & CHGFLO_B_DEB;
        else
            result = rg_chg & CHG_PLUG_DEB;
    }
    else
    {
        rg_chg = pmu_get_register_value_2565(PMU_TEST2, 0xFFFF, 0);
        if (rg_320 & EOC_EN)
            result = rg_chg & CHGFLO_B_E2;
        else
            result = rg_chg & CHG_PLUG_E2;
    }

    if (result)
        return TRUE;
    else
        return FALSE;
}
#ifndef AIR_PMU_DISABLE_CHARGER
uint16_t pmu_charger_get_chg_state(void)
{
    return pmu_get_register_value_2565(CHG_STATE_ADDR, CHG_STATE_MASK, CHG_STATE_SHIFT);
}

bool pmu_charger_is_compl(void)
{
    if (pmu_charger_get_chg_state() == CHG_COMPL)
        return TRUE;
    else
        return FALSE;
}

void pmu_charger_timer_callback(void *user_data)
{
    UNUSED(user_data);
    log_hal_msgid_info("pmu_charger_timer_callback", 0);

    pmu_set_register_value_2565(CHG_FORCE_OFF_ADDR, CHG_FORCE_OFF_MASK, CHG_FORCE_OFF_SHIFT, PMU_ON);
}

void pmu_charger_timer_start(uint32_t timeout_hr)
{
    //log_hal_msgid_info("pmu_charger_timer_start", 0);

    uint32_t timeout_ms = timeout_hr * 3600000;

    hal_gpt_sw_start_timer_ms(chg_timer, timeout_ms,
        (hal_gpt_callback_t)pmu_charger_timer_callback, NULL);
}

void pmu_charger_timer_stop(void)
{
    //log_hal_msgid_info("pmu_charger_timer_stop", 0);

    hal_gpt_sw_stop_timer_ms(chg_timer);
}

void pmu_charger_int_in(void)
{
    if (pmu_chg_intr_flag & CHG_INTR_IN)
    {
#ifdef HAL_CAPTOUCH_MODULE_ENABLED
        captouch_set_vbus_flag(true);
#endif
        log_hal_msgid_info("pmu_charger_int_in, intr_flag[%x]", 1, pmu_chg_intr_flag);

        pmu_charger_pfm_config(0);

        if (pmu_function_table_2565[RG_INT_CHG_IN].pmu_callback)
            pmu_function_table_2565[RG_INT_CHG_IN].pmu_callback();

        pmu_set_register_value_2565(CHG_FORCE_OFF_ADDR, CHG_FORCE_OFF_MASK, CHG_FORCE_OFF_SHIFT, PMU_OFF);

        pmu_charger_timer_start(4);
    }

    pmu_chg_intr_flag &= (uint8_t)(~CHG_INTR_IN);

    if (pmu_chg_intr_flag == CHG_INTR_NONE)
        pmu_chg_intr_flag = 0;
    else if (pmu_chg_intr_flag & CHG_INTR_COMPL)
        pmu_charger_int_compl();
    else if (pmu_chg_intr_flag & CHG_INTR_RECHG)
        pmu_charger_int_rechg();
    else if (pmu_chg_intr_flag & CHG_INTR_OUT)
        pmu_charger_int_out();
    else
        assert(0);
}

void pmu_charger_int_out(void)
{
    if (pmu_chg_intr_flag & CHG_INTR_OUT)
    {
#ifdef HAL_CAPTOUCH_MODULE_ENABLED
        captouch_set_vbus_flag(false);
#endif
        log_hal_msgid_info("pmu_charger_int_out, intr_flag[%x]", 1, pmu_chg_intr_flag);

        pmu_charger_pfm_config(0);

        if (pmu_function_table_2565[RG_INT_CHG_OUT].pmu_callback)
            pmu_function_table_2565[RG_INT_CHG_OUT].pmu_callback();

        pmu_charger_timer_stop();
    }

    pmu_chg_intr_flag &= (uint8_t)(~CHG_INTR_OUT);

    if (pmu_chg_intr_flag == CHG_INTR_NONE)
        pmu_chg_intr_flag = 0;
    else if (pmu_chg_intr_flag & CHG_INTR_IN)
        pmu_charger_int_in();
    else
        assert(0);
}

void pmu_charger_int_compl(void)
{
    if (pmu_chg_intr_flag & CHG_INTR_COMPL)
    {
        log_hal_msgid_info("pmu_charger_int_compl, intr_flag[%x]", 1, pmu_chg_intr_flag);

        if (pmu_function_table_2565[RG_INT_CHG_COMPL].pmu_callback)
            pmu_function_table_2565[RG_INT_CHG_COMPL].pmu_callback();

        pmu_charger_timer_stop();
    }

    pmu_chg_intr_flag &= (uint8_t)(~CHG_INTR_COMPL);

    if (pmu_chg_intr_flag == CHG_INTR_NONE)
        pmu_chg_intr_flag = 0;
    else if (pmu_chg_intr_flag & CHG_INTR_RECHG)
        pmu_charger_int_rechg();
    else if (pmu_chg_intr_flag & CHG_INTR_OUT)
        pmu_charger_int_out();
    else
        assert(0);
}

void pmu_charger_int_rechg(void)
{
    if (pmu_chg_intr_flag & CHG_INTR_RECHG)
    {
        log_hal_msgid_info("pmu_charger_int_rechg, intr_flag[%x]", 1, pmu_chg_intr_flag);

        if (pmu_function_table_2565[RG_INT_CHG_RECHG].pmu_callback)
            pmu_function_table_2565[RG_INT_CHG_RECHG].pmu_callback();

        pmu_charger_timer_start(4);
    }

    pmu_chg_intr_flag &= (uint8_t)(~CHG_INTR_RECHG);

    if (pmu_chg_intr_flag == CHG_INTR_NONE)
        pmu_chg_intr_flag = 0;
    else if (pmu_chg_intr_flag & CHG_INTR_COMPL)
        pmu_charger_int_compl();
    else if (pmu_chg_intr_flag & CHG_INTR_OUT)
        pmu_charger_int_out();
    else
        assert(0);
}

void pmu_charger_handler(uint16_t chg_flag)
{
    if (chg_flag & (1 << PMU_CHG_IN_INT_FLAG))
    {
        pmu_chg_intr_flag |= CHG_INTR_IN;
    }
    if (chg_flag & (1 << PMU_CHG_OUT_INT_FLAG))
    {
        pmu_chg_intr_flag |= CHG_INTR_OUT;
    }
    if (chg_flag & (1 << PMU_CHG_COMPLETE_INT_FLAG))
    {
        pmu_chg_intr_flag |= CHG_INTR_COMPL;
    }
    if (chg_flag & (1 << PMU_CHG_RECHG_INT_FLAG))
    {
        pmu_chg_intr_flag |= CHG_INTR_RECHG;
    }
    log_hal_msgid_info("pmu_charger_handler, intr_flag[%x]", 1, pmu_chg_intr_flag);

    if(pmu_charger_is_plug())
    {
        if (pmu_chg_intr_flag & CHG_INTR_IN)
            pmu_charger_int_out();
        else
            pmu_charger_int_compl();
    }
    else
    {
        pmu_charger_int_in();
    }
}

void pmu_charger_pfm_config(uint8_t op)
{
    if (pfm_flag == 0xFF)
    {
        pfm_flag = op;
    }
    if (pfm_flag != PMU_INIT)
    {
        return;
    }

    uint16_t rg_10c = pmu_get_register_value_2565(BUCK_CON6, 0xFFFF, 0);
    if (pmu_charger_is_plug())
    {
        rg_10c |= (1<<1);
        pmu_force_set_register_value_2565(BUCK_CON6, rg_10c);
        log_hal_msgid_info("pmu_charger_pfm_config, disable", 0);
    }
    else
    {
        rg_10c &= (uint16_t)(~(1<<1));
        pmu_force_set_register_value_2565(BUCK_CON6, rg_10c);
        log_hal_msgid_info("pmu_charger_pfm_config, enable", 0);
    }
}

pmu_operate_status_t pmu_charger_cc_config(pmu_cc_type_t type, pmu_chg_cc_state_t state)
{
    if ((type > PMU_CHG_CC2) || (state >= PMU_CHG_CC_MAX))
    {
        log_hal_msgid_error("pmu_charger_cc_config fail, cc_type[%d], cc_state[%d]", 2, type, state);
        return PMU_ERROR;
    }

    uint16_t rsel = 0;

    if (type == PMU_CHG_CC1)
    {
        if (state == PMU_CHG_CC_JEITA_NORM)
            rsel = pmu_chg_cc_config.norm_cc1_rsel;
        else if (state == PMU_CHG_CC_JEITA_WARM)
            rsel = pmu_chg_cc_config.warm_cc1_rsel;
        else if (state == PMU_CHG_CC_JEITA_COOL)
            rsel = pmu_chg_cc_config.cool_cc1_rsel;
        else if (state == PMU_CHG_CC_BAT_REPORT)
            rsel = pmu_chg_cc_config.vbat_cc1_rsel;

        pmu_set_register_value_2565(CC1_RCHG_SEL_ADDR, CC1_RCHG_SEL_MASK, CC1_RCHG_SEL_SHIFT, rsel);
        pmu_set_register_value_2565(CC1_RCHG_SEL_UPDATE_ADDR, CC1_RCHG_SEL_UPDATE_MASK, CC1_RCHG_SEL_UPDATE_SHIFT, 1);
    }
    else if (type == PMU_CHG_CC2)
    {
        if (state == PMU_CHG_CC_JEITA_NORM)
            rsel = pmu_chg_cc_config.norm_cc2_rsel;
        else if (state == PMU_CHG_CC_JEITA_WARM)
            rsel = pmu_chg_cc_config.warm_cc2_rsel;
        else if (state == PMU_CHG_CC_JEITA_COOL)
            rsel = pmu_chg_cc_config.cool_cc2_rsel;
        else if (state == PMU_CHG_CC_BAT_REPORT)
            rsel = pmu_chg_cc_config.vbat_cc2_rsel;

        pmu_set_register_value_2565(CC2_RCHG_SEL_ADDR, CC2_RCHG_SEL_MASK, CC2_RCHG_SEL_SHIFT, rsel);
        pmu_set_register_value_2565(CC2_RCHG_SEL_UPDATE_ADDR, CC2_RCHG_SEL_UPDATE_MASK, CC2_RCHG_SEL_UPDATE_SHIFT, 1);
    }

    log_hal_msgid_info("pmu_charger_cc_config, cc_type[%d], cc_state[%d], old_cc_state[%d], rsel[%d]", 4,
        type, state, pmu_chg_cc_config.state, rsel);
    pmu_chg_cc_config.state = state;

    return PMU_OK;
}

uint32_t pmu_chg_vchg_to_volt(uint32_t adc)
{
    uint32_t vchg = (adc * 10580 + 500)/1000;

    //log_hal_msgid_info("pmu_chg_vchg_to_volt, vchg[%d]mV", 1, vchg);
    return vchg;
}

/*==========[jeita]==========*/
void pmu_charger_ntc_init(void)
{
    pmu_status_t status = PMU_STATUS_ERROR;

    status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_JEITA_COOL, (uint8_t *)&pmu_chg_jeita_cool);
    if (status != PMU_STATUS_SUCCESS)
        log_hal_msgid_error("pmu_charger_ntc_init fail, NVKEYID_MP_CAL_JEITA_COOL status[%d]", 1, status);

    status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_JEITA_WARM, (uint8_t *)&pmu_chg_jeita_warm);
    if (status != PMU_STATUS_SUCCESS)
        log_hal_msgid_error("pmu_charger_ntc_init fail, NVKEYID_MP_CAL_JEITA_WARM status[%d]", 1, status);

    status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_INT_CHG_DAC_CONFIG, (uint8_t *)&pmu_chg_dac);
    if (status != PMU_STATUS_SUCCESS)
        log_hal_msgid_error("pmu_charger_ntc_init fail, NVKEYID_MP_CAL_INT_CHG_DAC_CONFIG status[%d]", 1, status);

    status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_INT_CHG_CC1_CURRENT_CONFIG, (uint8_t *)&pmu_chg_cc1);
    if (status != PMU_STATUS_SUCCESS)
        log_hal_msgid_error("pmu_charger_ntc_init fail, NVKEYID_MP_CAL_INT_CHG_CC1_CURRENT_CONFIG status[%d]", 1, status);

    status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_INT_CHG_CC2_CURRENT_CONFIG, (uint8_t *)&pmu_chg_cc2);
    if (status != PMU_STATUS_SUCCESS)
        log_hal_msgid_error("pmu_charger_ntc_init fail, NVKEYID_MP_CAL_INT_CHG_CC2_CURRENT_CONFIG status[%d]", 1, status);

    status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_CHG_ADC_CONFIG, (uint8_t *)&pmu_chg_adc);
    if (status != PMU_STATUS_SUCCESS)
        log_hal_msgid_error("pmu_charger_ntc_init fail, NVKEYID_MP_CAL_CHG_ADC_CONFIG status[%d]", 1, status);

    status = hal_pmu_get_nvkey(NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC, (uint8_t *)&pmu_chg_cv_stop_curr);
    if (status != PMU_STATUS_SUCCESS)
        log_hal_msgid_error("pmu_charger_ntc_init fail, NVKEYID_MP_CAL_CV_STOP_CURRENT_ADC status[%d]", 1, status);

    pmu_chg_cc_config.norm_cc1_rsel = pmu_chg_cc1.data[pmu_chg_cc1.select - 1].rchg_sel;
    pmu_chg_cc_config.warm_cc1_rsel = pmu_chg_jeita_warm.JEITA_warm_rchg_sel_cc1_1;
    pmu_chg_cc_config.cool_cc1_rsel = pmu_chg_jeita_cool.JEITA_cool_rchg_sel_cc1_1;
    pmu_chg_cc_config.vbat_cc1_rsel = 1;
    pmu_chg_cc_config.norm_cc2_rsel = pmu_chg_cc2.data[pmu_chg_cc2.select - 1].rchg_sel;
    pmu_chg_cc_config.warm_cc2_rsel = pmu_chg_jeita_warm.JEITA_warm_rchg_sel_cc2_1;
    pmu_chg_cc_config.cool_cc2_rsel = pmu_chg_jeita_cool.JEITA_cool_rchg_sel_cc2_1;
    pmu_chg_cc_config.vbat_cc2_rsel = 1;
}

void NTC_Change_DAC(bool EnterCool)
{
    uint16_t CHG_STATE = 0;
    uint32_t setValue = 0;
    uint16_t temp = 0;
    uint32_t count = 0;

    log_hal_msgid_info("[NTC]NTC_Change_DAC, EnterCool[%d]\n", 1, EnterCool);

    /* DAC_READY */
    while(pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, DAC_READY_SHIFT) == 0)
    {
        count++;
        if(count > 10){
            log_hal_msgid_info("[NTC]NTC_Change_DAC Count[%d]Data[%d]\n", 2, count, pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0));
            break;
        }

    }

    /* Set CC1_DAC_VALUE */
    if(EnterCool == true)
        temp = pmu_chg_jeita_cool.JEITA_cool_cc1_current_DAC;
    else
        temp = pmu_chg_jeita_warm.JEITA_warm_cc1_current_DAC;

    pmu_set_register_value_2565(CC1_DAC_VALUE_ADDR, CC1_DAC_VALUE_MASK, CC1_DAC_VALUE_SHIFT, temp);
    pmu_set_register_value_2565(CC1_DAC_OUT_UPDATE_ADDR, CC1_DAC_OUT_UPDATE_MASK, CC1_DAC_OUT_UPDATE_SHIFT, 1);

    /* Set CC2_DAC_VALUE */
    //if(((pmu_get_register_value_2565(CHARGER_CON0, 0xFFFF, 0) & 0x8000)>>15) == 1)
    //{
        if(EnterCool == true)
            temp = pmu_chg_jeita_cool.JEITA_cool_cc2_current_DAC;
        else
            temp = pmu_chg_jeita_warm.JEITA_warm_cc2_current_DAC;

        pmu_set_register_value_2565(CC2_DAC_VALUE_ADDR, CC2_DAC_VALUE_MASK, CC2_DAC_VALUE_SHIFT, temp);
        pmu_set_register_value_2565(CC2_DAC_OUT_UPDATE_ADDR, CC2_DAC_OUT_UPDATE_MASK, CC2_DAC_OUT_UPDATE_SHIFT, 1);

    //}

    /* Set CV_new_value */
    //if(((pmu_get_register_value_2565(CHARGER_CON0, 0xFFFF, 0) & 0x8000)>>15) == 1)
    //{
        if(EnterCool == true)
            temp = pmu_chg_jeita_cool.JEITA_cool_cv_DAC;
        else
            temp = pmu_chg_jeita_warm.JEITA_warm_cv_DAC;

        pmu_set_register_value_2565(CV_DAC_VALUE_ADDR, CV_DAC_VALUE_MASK, CV_DAC_VALUE_SHIFT, temp);
        pmu_set_register_value_2565(CV_DAC_OUT_UPDATE_ADDR, CV_DAC_OUT_UPDATE_MASK, CV_DAC_OUT_UPDATE_SHIFT, 1);
    //}

    //Check CHG STATE
    setValue = 0;
    CHG_STATE = (pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0) & 0x3800) >> 11;

    //Set D15=1
    if(CHG_STATE == 2)      //CC1 Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_DAC1, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_DAC1, setValue);
    }
    else if(CHG_STATE == 3) //CC2 Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_DAC2, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_DAC2, setValue);
    }
    else if(CHG_STATE == 5) //CV Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_DAC2, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_DAC2, setValue);
    }
    log_hal_msgid_info("[NTC]NTC_Change_DAC State Done\n", 0);
}

void NTC_Change_DAC_To_Default()
{
    uint16_t CHG_STATE = 0;
    uint32_t setValue = 0;
    uint16_t temp = 0;
    uint32_t count = 0;

    log_hal_msgid_info("[NTC]NTC_Change_DAC_To_Default[%d]\n", 0);

    /* DAC_READY */
    while(pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, DAC_READY_SHIFT) == 0)
    {
        count++;
        if(count > 10){
            log_hal_msgid_info("[NTC]NTC_Change_DAC Count[%d]Data[%d]\n", 2, count, pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0));
            break;
        }
    }

    /* Set CC1_DAC_VALUE */
    temp = pmu_chg_dac.cc1currentDAC + pmu_chg_dac.cc1currentDACoffset;
    pmu_set_register_value_2565(CC1_DAC_VALUE_ADDR, CC1_DAC_VALUE_MASK, CC1_DAC_VALUE_SHIFT, temp);
    pmu_set_register_value_2565(CC1_DAC_OUT_UPDATE_ADDR, CC1_DAC_OUT_UPDATE_MASK, CC1_DAC_OUT_UPDATE_SHIFT, 1);

    /* Set CC2_DAC_VALUE */
    temp = pmu_chg_dac.cc2currentDAC + pmu_chg_dac.cc2currentDACoffset;
    //if(((pmu_get_register_value_2565(CHARGER_CON0, 0xFFFF, 0) & 0x8000)>>15) == 1)
    //{
        pmu_set_register_value_2565(CC2_DAC_VALUE_ADDR, CC2_DAC_VALUE_MASK, CC2_DAC_VALUE_SHIFT, temp);
        pmu_set_register_value_2565(CC2_DAC_OUT_UPDATE_ADDR, CC2_DAC_OUT_UPDATE_MASK, CC2_DAC_OUT_UPDATE_SHIFT, 1);
    //}

    /* Set CV_new_value */
    temp = pmu_chg_dac.cvDAC + pmu_chg_dac.cvDACoffset;
    //if(((pmu_get_register_value_2565(CHARGER_CON0, 0xFFFF, 0) & 0x8000)>>15) == 1)
    //{
        pmu_set_register_value_2565(CV_DAC_VALUE_ADDR, CV_DAC_VALUE_MASK, CV_DAC_VALUE_SHIFT, temp);
        pmu_set_register_value_2565(CV_DAC_OUT_UPDATE_ADDR, CV_DAC_OUT_UPDATE_MASK, CV_DAC_OUT_UPDATE_SHIFT, 1);
    //}

    //Check CHG STATE
    setValue = 0;
    CHG_STATE = (pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0) & 0x3800) >> 11;

    //Set D15=1
    if(CHG_STATE == 2)      //CC1 Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_DAC1, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_DAC1, setValue);
    }
    else if(CHG_STATE == 3) //CC2 Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_DAC2, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_DAC2, setValue);
    }
    else if(CHG_STATE == 5) //CV Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_DAC2, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_DAC2, setValue);
    }
}

void NTC_Change_Rsel(bool EnterCool)
{
    uint16_t CHG_STATE = 0;
    uint32_t setValue = 0;
    uint16_t temp = 0;
    uint32_t count = 0;

    log_hal_msgid_info("[NTC]NTC_Change_Rsel Start\n", 0);

    /* Check RSEL ready */
    while(pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, RSEL_READY_SHIFT) == 0)
    {
        count++;
        if(count > 10){
            log_hal_msgid_info("[NTC]NTC_Change_Rsel Count[%d]Data[%d]\n", 2, count, pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0));
            break;
        }
    }

    /* Set CC1_New_VALUE */
    if(EnterCool == true)
        pmu_charger_cc_config(PMU_CHG_CC1, PMU_CHG_CC_JEITA_COOL);
    else
        pmu_charger_cc_config(PMU_CHG_CC1, PMU_CHG_CC_JEITA_WARM);

    /* CC2_RCHG_SEL */
    if(((pmu_get_register_value_2565(CHARGER_CON0, 0xFFFF, 0) & 0x8000)>>15) == 1)
    {
        if(EnterCool == true)
            pmu_charger_cc_config(PMU_CHG_CC2, PMU_CHG_CC_JEITA_COOL);
        else
            pmu_charger_cc_config(PMU_CHG_CC2, PMU_CHG_CC_JEITA_WARM);
    }

    /* Set CV_new_value */
    if(((pmu_get_register_value_2565(CHARGER_CON0, 0xFFFF, 0) & 0x8000)>>15) == 1)
    {
        temp = pmu_get_register_value_2565(CHARGER_RSEL2, 0xFFFF, 0);
        pmu_set_register_value_2565(CV_RCHG_SEL_ADDR, CV_RCHG_SEL_MASK, CV_RCHG_SEL_SHIFT, temp);
        pmu_set_register_value_2565(CV_RCHG_SEL_UPDATE_ADDR, CV_RCHG_SEL_UPDATE_MASK, CV_RCHG_SEL_UPDATE_SHIFT, 1);
    }else{
        temp = pmu_get_register_value_2565(CHARGER_RSEL1, 0xFFFF, 0);
        pmu_set_register_value_2565(CV_RCHG_SEL_ADDR, CV_RCHG_SEL_MASK, CV_RCHG_SEL_SHIFT, temp);
        pmu_set_register_value_2565(CV_RCHG_SEL_UPDATE_ADDR, CV_RCHG_SEL_UPDATE_MASK, CV_RCHG_SEL_UPDATE_SHIFT, 1);
    }

    //Check CHG STATE
    CHG_STATE = (pmu_get_register_value_2565(0x0322, 0xFFFF, 0) & 0x3800) >> 11;

    //Set D15=1
    if(CHG_STATE == 2)      //CC1 Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_RSEL1, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_RSEL1, setValue);
    }
    else if(CHG_STATE == 3) //CC2 Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_RSEL2, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_RSEL2, setValue);
    }
    else if(CHG_STATE == 5) //CV Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_RSEL3, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_RSEL3, setValue);
    }
}

void NTC_Change_Rsel_To_Default()
{
    uint16_t CHG_STATE = 0;
    uint32_t setValue = 0;
    uint32_t count = 0;
    uint16_t tmpval;
    log_hal_msgid_info("[NTC]NTC_Change_Rsel_To_Default Start\n", 0);

    /* Check RSEL ready */
    while(pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, RSEL_READY_SHIFT) == 0)
    {
        count++;
        if(count > 10){
            log_hal_msgid_info("[NTC]NTC_Change_Rsel_To_Default Count[%d]Data[%d]\n", 2, count, pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0));
            break;
        }
    }

    /* Set CC1_New_VALUE */
    pmu_charger_cc_config(PMU_CHG_CC1, PMU_CHG_CC_JEITA_NORM);

    /* CC2_RCHG_SEL */
    if(((pmu_get_register_value_2565(CHARGER_CON0, 0xFFFF, 0) & 0x8000)>>15) == 1)
    {
        pmu_charger_cc_config(PMU_CHG_CC2, PMU_CHG_CC_JEITA_NORM);
    }

    /* Set CV_new_value */
    if(((pmu_get_register_value_2565(CHARGER_CON0, 0xFFFF, 0) & 0x8000) >> 15) == 1)
    {
        tmpval = pmu_chg_cc2.data[pmu_chg_cc2.select - 1].rchg_sel;
        pmu_set_register_value_2565(CV_RCHG_SEL_ADDR, CV_RCHG_SEL_MASK, CV_RCHG_SEL_SHIFT, tmpval);
        pmu_set_register_value_2565(CV_RCHG_SEL_UPDATE_ADDR, CV_RCHG_SEL_UPDATE_MASK, CV_RCHG_SEL_UPDATE_SHIFT, 1);
    }else{
        tmpval = pmu_chg_cc1.data[pmu_chg_cc1.select - 1].rchg_sel;
        pmu_set_register_value_2565(CV_RCHG_SEL_ADDR, CV_RCHG_SEL_MASK, CV_RCHG_SEL_SHIFT, tmpval);
        pmu_set_register_value_2565(CV_RCHG_SEL_UPDATE_ADDR, CV_RCHG_SEL_UPDATE_MASK, CV_RCHG_SEL_UPDATE_SHIFT, 1);
    }

    //Check CHG STATE
    CHG_STATE = (pmu_get_register_value_2565(0x0322, 0xFFFF, 0) & 0x3800) >> 11;

    //Set D15=1
    if(CHG_STATE == 2)      //CC1 Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_RSEL1, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_RSEL1, setValue);
    }
    else if(CHG_STATE == 3) //CC2 Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_RSEL2, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_RSEL2, setValue);
    }
    else if(CHG_STATE == 5) //CV Mode
    {
        setValue = pmu_get_register_value_2565(CHARGER_RSEL3, 0xFFFF, 0);
        setValue |= 0x8000;
        pmu_force_set_register_value_2565(CHARGER_RSEL3, setValue);
    }
}

void NTC_Change_Threshold(bool EnterCool)
{
    log_hal_msgid_info("[NTC]NTC_Change_Threshold\n", 0);

    uint16_t temp = 0;

    //FULL_BAT_THRESHOLD2
    /*if(EnterCool == true)
        temp = pmu_chg_jeita_cool.JEITA_cool_full_bat_ADC_2;
    else
        temp = pmu_chg_jeita_warm.JEITA_warm_full_bat_ADC_2;*/
    //pmu_set_register_value_2565(FULL_BAT_THRESHOLD2_ADDR, FULL_BAT_THRESHOLD2_MASK, FULL_BAT_THRESHOLD2_SHIFT, temp);

    //NTC_CC1_THRESHOLD
    temp = pmu_chg_adc.cc1_threshold_ADC;
    pmu_set_register_value_2565(CC1_THRESHOLD_ADDR, CC1_THRESHOLD_MASK, CC1_THRESHOLD_SHIFT, temp);


    //NTC_CC2_THRESHOLD
    if(EnterCool == true)
        temp = pmu_chg_jeita_cool.JEITA_cool_cc2_threshold_ADC;
    else
        temp = pmu_chg_jeita_warm.JEITA_warm_cc2_threshold_ADC;
    pmu_set_register_value_2565(CC2_THRESHOLD_ADDR, CC2_THRESHOLD_MASK, CC2_THRESHOLD_SHIFT, temp);

    //NTC_CV_STOP_CURRENT
    if(EnterCool == true)
        temp = pmu_chg_jeita_cool.JEITA_cool_CV_stop_current_ADC;
    else
        temp = pmu_chg_jeita_warm.JEITA_warm_CV_stop_current_ADC;
    pmu_set_register_value_2565(CV_STOP_CURRENT_ADDR, CV_STOP_CURRENT_MASK, CV_STOP_CURRENT_SHIFT, temp);

    //NTC_CV_THRESHOLD
    if(EnterCool == true)
        temp = pmu_chg_jeita_cool.JEITA_cool_cv_threshold_ADC;
    else
        temp = pmu_chg_jeita_warm.JEITA_warm_cv_threshold_ADC;
    pmu_set_register_value_2565(CV_THRESHOLD_ADDR, CV_THRESHOLD_MASK, CV_THRESHOLD_SHIFT, temp);

    //NTC_FULL_BAT_THRESHOLD1
    /*if(EnterCool == true)
        temp = pmu_chg_jeita_cool.JEITA_cool_full_bat_ADC;
    else
        temp = pmu_chg_jeita_warm.JEITA_warm_full_bat_ADC;*/
    //pmu_set_register_value_2565(FULL_BAT_THRESHOLD1_ADDR, FULL_BAT_THRESHOLD1_MASK, FULL_BAT_THRESHOLD1_SHIFT, temp);

    //NTC_RECHARGE_THRESHOLD
    if(EnterCool == true)
        temp = pmu_chg_jeita_cool.JEITA_cool_recharge_ADC;
    else
        temp = pmu_chg_jeita_warm.JEITA_warm_recharge_ADC;
    pmu_set_register_value_2565(RECHARGE_THRESHOLD_ADDR, RECHARGE_THRESHOLD_MASK, RECHARGE_THRESHOLD_SHIFT, temp);
}

void NTC_Change_Threshold_To_Default()
{
    log_hal_msgid_info("[NTC]NTC_Change_Threshold_To_Default\n", 0);

    uint16_t ADCval;

    //FULL_BAT_THRESHOLD2
    //ADCval = pmu_chg_adc.full_bat_ADC_2 + pmu_chg_adc.full_bat_ADC_2_offset;
    //pmu_set_register_value_2565(FULL_BAT_THRESHOLD2_ADDR, FULL_BAT_THRESHOLD2_MASK, FULL_BAT_THRESHOLD2_SHIFT, ADCval);

    //NTC_CC1_THRESHOLD
    ADCval = pmu_chg_adc.cc1_threshold_ADC + pmu_chg_adc.cc1_thd_ADC_offset;
    pmu_set_register_value_2565(CC1_THRESHOLD_ADDR, CC1_THRESHOLD_MASK, CC1_THRESHOLD_SHIFT, ADCval);

    //NTC_CC2_THRESHOLD
    ADCval = pmu_chg_adc.cc2_threshold_ADC + pmu_chg_adc.cc2_offset_ADC;
    pmu_set_register_value_2565(CC2_THRESHOLD_ADDR, CC2_THRESHOLD_MASK, CC2_THRESHOLD_SHIFT, ADCval);

    //NTC_CV_STOP_CURRENT
    ADCval = pmu_chg_cv_stop_curr.CV_stop_current_ADC;
    pmu_set_register_value_2565(CV_STOP_CURRENT_ADDR, CV_STOP_CURRENT_MASK, CV_STOP_CURRENT_SHIFT, ADCval);

    //NTC_CV_THRESHOLD
    ADCval = pmu_chg_adc.cv_threshold_ADC+ pmu_chg_adc.cv_thd_ADC_offset;
    pmu_set_register_value_2565(CV_THRESHOLD_ADDR, CV_THRESHOLD_MASK, CV_THRESHOLD_SHIFT, ADCval);

    //NTC_FULL_BAT_THRESHOLD1
    //ADCval = pmu_chg_adc.full_bat_ADC + pmu_chg_adc.full_bat_ADC_offset;
    //pmu_set_register_value_2565(FULL_BAT_THRESHOLD1_ADDR, FULL_BAT_THRESHOLD1_MASK, FULL_BAT_THRESHOLD1_SHIFT, ADCval);

    //NTC_RECHARGE_THRESHOLD
    ADCval = pmu_chg_adc.recharge_ADC + pmu_chg_adc.recharge_ADC_offset;
    pmu_set_register_value_2565(RECHARGE_THRESHOLD_ADDR, RECHARGE_THRESHOLD_MASK, RECHARGE_THRESHOLD_SHIFT, ADCval);
}

static void NTC_Operation(NTC_OP op)
{
    bool EnterCool = false;

    /* Loging to NVKEY while abnormal...
    */
    if (op.enable_force_chg_off)        /* Charger OFF */
    {
        log_hal_msgid_info("[NTC][OP] Any State to Cold/Hot State Start\n", 0);
        //pmu_enable_charger_2565(0);
        uint32_t bat_volt = pmu_bat_adc_to_volt(pmu_auxadc_get_channel_value(PMU_AUX_VBAT));
        if (bat_volt > 3200)
        {
            pmu_eoc_ctrl(1);
        }
        else
        {
            log_hal_msgid_error("pmu_eoc_ctrl, bypass bat_volt[%d]", 1, bat_volt);
        }
    }
    else if (op.disable_force_chg_off)  /* Charger ON */
    {
        log_hal_msgid_info("[NTC][OP] Cold/Hot State to Any State \n", 0);
        //pmu_enable_charger_2565(1);
        pmu_eoc_ctrl(0);
    }
    else if (op.enable_limit_current)   /* Normal to Cool/Warm State */
    {
        uint16_t CHG_STATE = 0;
        uint16_t Temp = 0;
        uint32_t Count = 0;

        if(op.enable_warm){
            log_hal_msgid_info("[NTC][OP] Normal to Warm State\n", 0);
            EnterCool = false;
        }
        if(op.enable_cold){
            log_hal_msgid_info("[NTC][OP] Normal to COOL State\n", 0);
            EnterCool = true;
        }


        /* Step 1: ADC_RDY_STS1 isn't ready */
        while(pmu_get_register_value_2565(ADC_DATA1, 0xFFFF, ADC_RDY_STS1_SHIFT) == 0){
            Count++;

            if(Count > 10){
                log_hal_msgid_info("[NTC][OP] enable_limit_current CntBrk[%d]Data[0x%X]\n", 2, Count, pmu_get_register_value_2565(ADC_DATA1, 0xFFFF, 0));
                break;
            }
        }
        /* Step 2: Check CHG STATE */
        CHG_STATE = (pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0) & 0x3800) >> 11;
        /* Step 3 */
        Temp = pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0);
        if(CHG_STATE == NTC_CHARGER_CON2_TRICKLE){
            Temp |= (1<<2);
        }else if(CHG_STATE == NTC_CHARGER_CON2_CC1){
            Temp |= (1<<3);
        }else if(CHG_STATE == NTC_CHARGER_CON2_CC2){
            Temp |= (1<<4);
        }else if(CHG_STATE == NTC_CHARGER_CON2_CV){
            Temp |= (1<<5);
        }else if(CHG_STATE == NTC_CHARGER_CON2_COMPL){
            Temp |= (1<<6);
        }else if(CHG_STATE == NTC_CHARGER_CON2_RECHG){
            Temp |= (1<<7);
        }
        pmu_set_register_value_2565(CHARGER_CON2, 0xFFFF, 0, Temp);

        /* Step 4 */
        NTC_Change_DAC(EnterCool);
        NTC_Change_Rsel(EnterCool);
        NTC_Change_Threshold(EnterCool);
        /* Step 7 */
        Temp = pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0);
        Temp &= 0xFF03;
        pmu_force_set_register_value_2565(CHARGER_CON2, Temp);
        log_hal_msgid_info("[NTC][OP] enable_limit_current Done\n", 1, 8);
    }
    else if(op.disable_limit_current)    /* Cool/Warm to Normal State */
    {
        uint16_t CHG_STATE = 0;
        uint16_t Temp = 0;
        uint32_t Count = 0;

        if(op.enable_warm){
            log_hal_msgid_info("[NTC][OP] COOL to Normal State\n", 0);
            EnterCool = false;
        }
        if(op.enable_cold){
            log_hal_msgid_info("[NTC][OP] Warm to Normal State\n", 0);
            EnterCool = true;
        }


        /* Step 1: Check ADC ready Chg state no change */
        while(pmu_get_register_value_2565(ADC_DATA1, 0xFFFF, ADC_RDY_STS1_SHIFT) == 0)
        {
            Count++;
            if(Count > 10){
                log_hal_msgid_info("[NTC][OP] disable_limit_current CntBrk[%d]Data[0x%X]\n", 2, Count, pmu_get_register_value_2565(ADC_DATA1, 0xFFFF, 0));
                break;
            }
        }

        /* Step 2: Check CHG STATE */
        CHG_STATE = (pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0) & 0x3800) >> 11;
        /* Step 3 */
        Temp = pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0);
        if(CHG_STATE == NTC_CHARGER_CON2_TRICKLE){
            Temp |= (1<<2);
        }else if(CHG_STATE == NTC_CHARGER_CON2_CC1){
            Temp |= (1<<3);
        }else if(CHG_STATE == NTC_CHARGER_CON2_CC2){
            Temp |= (1<<4);
        }else if(CHG_STATE == NTC_CHARGER_CON2_CV){
            Temp |= (1<<5);
        }else if(CHG_STATE == NTC_CHARGER_CON2_COMPL){
            Temp |= (1<<6);
        }else if(CHG_STATE == NTC_CHARGER_CON2_RECHG){
            Temp |= (1<<7);
        }
        pmu_set_register_value_2565(CHARGER_CON2, 0xFFFF, 0, Temp);

        /* Step 4 */
        NTC_Change_Rsel_To_Default();
        NTC_Change_DAC_To_Default();
        NTC_Change_Threshold_To_Default();

        /* Step 7 */
        Temp = pmu_get_register_value_2565(CHARGER_CON2, 0xFFFF, 0);
        Temp &= 0xFF03;
        pmu_force_set_register_value_2565(CHARGER_CON2, Temp);
        log_hal_msgid_info("[NTC][OP] disable_limit_current Done\n", 1, 8);
    }
}

void ntc_change_state(enum JEITA_CHARGER_STATUS JEITA_OP)
{
    NTC_OP OP;
    memset(&OP, 0, sizeof(NTC_OP));

    log_hal_msgid_info("[NTC] ntc_change_state JEITA_OP[%d]\n", 1, JEITA_OP);

    switch (JEITA_OP)
    {
        case JEITA_ENABLE_CHARGER:
            OP.disable_force_chg_off = 1;
            break;
        case JEITA_DISABLE_CHARGER:
            OP.enable_force_chg_off = 1;
            break;
        case JEITA_NORMAL_TO_WARM:
            OP.enable_limit_current = 1;
            OP.enable_warm = 1;
            break;
        case JEITA_NORMAL_TO_COOL:
            OP.enable_limit_current = 1;
            OP.enable_cold = 1;
            break;
        case JEITA_WARM_TO_NORMAL:
            OP.disable_limit_current = 1;
            OP.enable_cold = 1;
            break;
        case JEITA_COOL_TO_NORMAL:
            OP.disable_limit_current = 1;
            OP.enable_warm = 1;
            break;
    }

    NTC_Operation(OP);
}
#endif
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

uint8_t pmu_get_bc12_charger_type_2565(void) {
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

/*==========[battery]==========*/
#define MMI_IPHONE_BATTERY_LEVELS 9

VBAT_VOLTAGE_CONFIG* p_vbat_volt = NULL;
VBAT_ADC_CALIBRATION_TABLE* p_vbat_adc = NULL;

void pmu_bat_init(void)
{
#ifdef AIR_PMU_DISABLE_CHARGER
    log_hal_msgid_info("pmu_bat_init, bypass", 0);
    return;
#else
    pmu_charger_init_2565(0, 0);
    pmu_charger_ntc_init();
    p_vbat_volt = (VBAT_VOLTAGE_CONFIG*)get_nvkey_data(NVKEYID_MP_CAL_VBAT_VOLTAGE_CONFIG);

    if(!p_vbat_volt)
    {
        log_hal_msgid_info("pmu_bat_init, NVKEYID_MP_CAL_VBAT_VOLTAGE_CONFIG no data", 0);
        assert(0);
    }

    p_vbat_adc = (VBAT_ADC_CALIBRATION_TABLE*)get_nvkey_data(NVKEYID_MP_CAL_VBAT_ADC_CALIBRATION_TABLE);

    if(!p_vbat_adc)
    {
        log_hal_msgid_info("pmu_bat_init, NVKEYID_MP_CAL_VBAT_ADC_CALIBRATION_TABLE no data", 0);
        assert(0);
    }

    log_hal_msgid_info("pmu_bat_init, initial[%d], low[%d], shutdown[%d]", 3, p_vbat_volt->initial_bat_adc, p_vbat_volt->low_bat_adc, p_vbat_volt->shutdown_bat_adc);
    log_hal_msgid_info("pmu_bat_init, 0%[%d], 10%[%d], 20%[%d], 30%[%d], 40%[%d], 50%[%d], 60%[%d], 70%[%d], 80%[%d], 90%[%d], 100%[%d]", 11,
        p_vbat_volt->shutdown_bat_adc, p_vbat_volt->data[0].adc, p_vbat_volt->data[1].adc, p_vbat_volt->data[2].adc,
        p_vbat_volt->data[3].adc, p_vbat_volt->data[4].adc, p_vbat_volt->data[5].adc, p_vbat_volt->data[6].adc,
        p_vbat_volt->data[7].adc, p_vbat_volt->data[8].adc, p_vbat_volt->data[9].adc);
#endif
}
#ifndef AIR_PMU_DISABLE_CHARGER
static uint32_t pmu_bat_adc_to_perc_internal(uint32_t adcval, int fullscale_value)
{
    uint8_t i;
    uint16_t lowBd, highBd;
    uint32_t mul = fullscale_value / 10;
    uint32_t result = 0;

    for( i = 0; i < MMI_IPHONE_BATTERY_LEVELS; i++)
    {
        if(adcval < p_vbat_volt->data[i].adc)
            break;
    }

    if(i == 0)
    {
        lowBd = p_vbat_volt->shutdown_bat_adc;
        highBd = p_vbat_volt->data[0].adc;

        if(adcval < lowBd)
        {
            return 0;
        }
    }
    else if(i == MMI_IPHONE_BATTERY_LEVELS)
    {
        lowBd = p_vbat_volt->data[MMI_IPHONE_BATTERY_LEVELS-1].adc;
        highBd = p_vbat_volt->data[MMI_IPHONE_BATTERY_LEVELS].adc;

        if(adcval >= highBd)
        {
            log_hal_msgid_info("pmu_bat_adc_to_perc_internal, i=%d, adcval = %d, highBd = %d", 3, i, adcval, highBd);
            return fullscale_value;
        }
    }
    else
    {
        lowBd = p_vbat_volt->data[i-1].adc;
        highBd = p_vbat_volt->data[i].adc;
    }

    result = (uint32_t)(mpk_round((mul * (adcval - lowBd)), (highBd - lowBd)) + (i*mul));

    //log_hal_msgid_info("pmu_bat_adc_to_perc_internal i=%d adcval=%d lowBd=%d highBd=%d result = %d, adcval = %d", 6, i, adcval, lowBd, highBd, result, adcval);

    return result;
}

uint8_t pmu_bat_adc_to_perc(uint32_t adc)
{
    return pmu_bat_adc_to_perc_internal(adc, 100);
}

uint16_t pmu_bat_adc_to_k_perc(uint32_t adc)
{
    return pmu_bat_adc_to_perc_internal(adc, 1000);
}

uint32_t pmu_bat_volt_to_adc(uint32_t volt)
{
    uint8_t i = 0;
    uint16_t volt1, volt2, adc1, adc2;
    uint32_t result = 0;

    if (volt < p_vbat_volt->data[0].check_point)
    {
        volt1 = p_vbat_volt->shutdown_bat;
        adc1 = p_vbat_volt->shutdown_bat_adc;
        volt2 = p_vbat_volt->data[0].check_point;
        adc2 = p_vbat_volt->data[0].adc;
    }
    else if (volt > p_vbat_volt->data[8].check_point)
    {
        volt1 = p_vbat_volt->data[8].check_point;
        adc1 = p_vbat_volt->data[8].adc;
        volt2 = p_vbat_volt->data[MMI_IPHONE_BATTERY_LEVELS].check_point;
        adc2 = p_vbat_volt->data[MMI_IPHONE_BATTERY_LEVELS].adc;
    }
    else
    {
        for( i = 0; i < MMI_IPHONE_BATTERY_LEVELS; i++)
        {
            if (volt < p_vbat_volt->data[i].check_point)
            {
                break;
            }
        }
        volt1 = p_vbat_volt->data[i-1].check_point;
        adc1 = p_vbat_volt->data[i-1].adc;
        volt2 = p_vbat_volt->data[i].check_point;
        adc2 = p_vbat_volt->data[i].adc;
    }

    result = slope_calc(volt1, adc1, volt2, adc2, volt);

    log_hal_msgid_info("pmu_bat_volt_to_adc, volt[%d], adc[%d]", 2, volt, result);

    return result;

}

uint32_t pmu_bat_adc_to_volt(uint32_t adcval)
{
    uint8_t i = 0;
    uint16_t volt1, volt2, adc1, adc2;
    uint32_t result = 0;

    if (adcval < p_vbat_volt->data[0].adc)
    {
        volt1 = p_vbat_volt->shutdown_bat;
        adc1 = p_vbat_volt->shutdown_bat_adc;
        volt2 = p_vbat_volt->data[0].check_point;
        adc2 = p_vbat_volt->data[0].adc;
    }
    else if (adcval > p_vbat_volt->data[8].adc)
    {
        volt1 = p_vbat_volt->data[8].check_point;
        adc1 = p_vbat_volt->data[8].adc;
        volt2 = p_vbat_volt->data[MMI_IPHONE_BATTERY_LEVELS].check_point;
        adc2 = p_vbat_volt->data[MMI_IPHONE_BATTERY_LEVELS].adc;
    }
    else
    {
        for( i = 0; i < MMI_IPHONE_BATTERY_LEVELS; i++)
        {
            if (adcval < p_vbat_volt->data[i].adc)
            {
                break;
            }
        }
        volt1 = p_vbat_volt->data[i-1].check_point;
        adc1 = p_vbat_volt->data[i-1].adc;
        volt2 = p_vbat_volt->data[i].check_point;
        adc2 = p_vbat_volt->data[i].adc;
    }

    result = slope_calc(adc1, volt1, adc2, volt2, adcval);

    log_hal_msgid_info("pmu_bat_adc_to_volt, adc[%d], volt[%d]", 2, adcval, result);

    return result;
}

void pmu_bat_3v3_proc(void)
{
    uint32_t bat_volt = pmu_bat_adc_to_volt(pmu_auxadc_get_channel_value(PMU_AUX_VBAT));

    if(bat_volt > 3300)
    {
        if ((adie_version == PMU_ECO3) || (adie_version == PMU_NO_FTK))
            pmu_set_register_value_2565(VLDO33_CLAMP_EN_ADDR, VLDO33_CLAMP_EN_MASK, VLDO33_CLAMP_EN_SHIFT, 0);
        else
            pmu_set_register_value_2565(VLDO33_CLAMP_EN_E2_ADDR, VLDO33_CLAMP_EN_E2_MASK, VLDO33_CLAMP_EN_E2_SHIFT, 0);
    }
    else
    {
        if ((adie_version == PMU_ECO3) || (adie_version == PMU_NO_FTK))
            pmu_set_register_value_2565(VLDO33_CLAMP_EN_ADDR, VLDO33_CLAMP_EN_MASK, VLDO33_CLAMP_EN_SHIFT, 1);
        else
            pmu_set_register_value_2565(VLDO33_CLAMP_EN_E2_ADDR, VLDO33_CLAMP_EN_E2_MASK, VLDO33_CLAMP_EN_E2_SHIFT, 1);
    }

    log_hal_msgid_info("pmu_bat_3v3_proc, bat_volt[%d], rg_202[0x%X]", 2, bat_volt, pmu_get_register_value_2565(0x202, 0xFFFF, 0));
}

uint16_t pmu_bat_get_pure_adc(void)
{
    uint16_t bat = 0;

#if (CHG_GAUGE_EN)
    uint32_t chg_sta = pmu_charger_get_chg_state();

    if ((chg_sta == CHG_CC1) || (chg_sta == CHG_CC2))
    {
        pmu_charger_cc_config(PMU_CHG_CC1, PMU_CHG_CC_BAT_REPORT);
        pmu_charger_cc_config(PMU_CHG_CC2, PMU_CHG_CC_BAT_REPORT);

        hal_gpt_delay_ms(50);
        bat = pmu_auxadc_get_channel_value(PMU_AUX_VBAT);

        if (pmu_chg_cc_config.state == PMU_CHG_CC_BAT_REPORT)
        {
            pmu_charger_cc_config(PMU_CHG_CC1, PMU_CHG_CC_JEITA_NORM);
            pmu_charger_cc_config(PMU_CHG_CC2, PMU_CHG_CC_JEITA_NORM);
        }
    }
    else
    {
        bat = pmu_auxadc_get_channel_value(PMU_AUX_VBAT);
    }
    log_hal_msgid_info("pmu_bat_get_pure_adc, chg_sta[0x%X], cc_sta[%d], bat[%d], volt[%d]", 4,
        chg_sta, pmu_chg_cc_config.state, bat, pmu_bat_adc_to_volt(bat));
#else
    bat = pmu_auxadc_get_channel_value(PMU_AUX_VBAT);
    log_hal_msgid_info("pmu_bat_get_pure_adc, chg_sta[0x%X], cc_sta[%d], bat[%d], volt[%d]", 4,
        pmu_charger_get_chg_state(), pmu_chg_cc_config.state, bat, pmu_bat_adc_to_volt(bat));
#endif

    return bat;
}
#endif
#endif /* HAL_PMU_MODULE_ENABLED */
