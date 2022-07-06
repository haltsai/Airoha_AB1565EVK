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
#ifdef HAL_USB_MODULE_ENABLED
#include "hal_usb.h"
#include "hal_usb_internal.h"
#endif
#ifdef MTK_USB_DEMO_ENABLED
#include "usb.h"
#endif
#include "hal_sleep_manager_internal.h"
extern battery_managerment_control_info_t bm_ctrl_info;
extern uint8_t battery_sleep_handle;
extern battery_basic_data bm_cust;
extern TimerHandle_t xbm_option_setting_timer;
void battery_management_eoc2_exit_setting(void) {
    if (battery_check_charger_ref_en() == 0) {
        hal_gpt_delay_ms(2);
        if (battery_check_vbus_gt_por() != 0) {
            LOG_MSGID_I(battery_management, "EOC2 ad_qi_vbus_gt_por has issue\r\n", 0);
        }
        battery_control_recharge_low_bat(0);
        battery_control_vbat_en_mode_sel(1);
        battery_rstb_select(0);
        battery_control_pmic_protect(TST_W_KEY_HW_MODE);
        bm_ctrl_info.feature.charger_init = 0;
    }
    battery_set_hw_jeita_enable(BATTERY_OPERATE_OFF);
    battery_set_jeita_irq_en(BATTERY_OPERATE_OFF);
}

void battery_management_eoc2_recharger_interrupt(void) {
    pmu_srclken_control_mode_6388(1);
    battery_set_ibus_low_en(0);
    hal_gpt_delay_ms(2);
    if (battery_check_vbus_gt_por() != 1) {
        LOG_MSGID_I(battery_management, "EOC2 has problem\r\n", 0);
    } else {
        battery_enable_sysldo(1);
        battery_control_recharge_low_bat(0);
        battery_control_vbat_en_mode_sel(1);
        battery_rstb_select(0);
        battery_control_pmic_protect(TST_W_KEY_HW_MODE);
    }
}

void battery_management_eoc2_setting(void)
{
    battery_control_pmic_protect(TST_W_KEY_SW_MODE);
    pmu_srclken_control_mode_6388(1);
    battery_enable_vbat_rechg(1);
    battery_set_vbat_rechg_mask(0);
    battery_set_vbat_irq_en(1);
    if (pmu_get_status_interrupt(RG_INT_VBAT_RECHG) == 1) {
        pmu_control_clear_interrupt(RG_INT_VBAT_RECHG);
    }
    battery_control_recharge_low_bat(1);
    battery_control_vbat_en_mode_sel(0);
    battery_rstb_select(1);
    battery_enable_sysldo(0);
    xTimerStartFromISR(xbm_option_setting_timer,0); //Wait 100ms for state charger machine
}

void battery_management_eoc3_exit_setting(void) {
    pmu_power_enable_6388(PMU_LDO_VA18, PMU_ON);
    pmu_power_enable_6388(PMU_LDO_VLDO33, PMU_ON);
    battery_charger_suspend_mode(0);
    battery_set_ibus_low_en(0);
    hal_gpt_delay_ms(2);
    battery_set_ppfet_lp_mode(0);
    battery_set_plugin_db_sw_sel(0);
    battery_control_pmic_protect(TST_W_KEY_HW_MODE);
    bm_ctrl_info.feature.charger_init = 0;
    battery_set_hw_jeita_enable(BATTERY_OPERATE_OFF);
    battery_set_jeita_irq_en(BATTERY_OPERATE_OFF);
    if(sleep_management_check_handle_status(battery_sleep_handle)>=1){
        hal_sleep_manager_unlock_sleep(battery_sleep_handle);
        LOG_MSGID_I(battery_management, "Battery Unlock sleep\r\n", 0);
    }
}

void battery_management_eoc3_recharger_setting(void) {
	battery_set_precc_ith(bm_cust.precc_cur);         // precc iterm setting
	battery_set_iterm_ith(bm_cust.cv_termination);    // iterm ith setting
    pmu_power_enable_6388(PMU_LDO_VA18, PMU_ON);
    pmu_power_enable_6388(PMU_LDO_VLDO33, PMU_ON);
    hal_gpt_delay_ms(1);
    battery_charger_suspend_mode(0);
    hal_gpt_delay_ms(10); // interrupt delay use gpt
    battery_set_ppfet_lp_mode(0);
    battery_set_ibus_low_en(0);
    hal_gpt_delay_ms(2);
    battery_enable_sysldo(1);
    if (battery_check_vbus_gt_por() == 1 && battery_get_vbus_uvlo_db() == 0 && battery_get_vbus_ovp_db() == 0) {
        battery_set_plugin_db_sw_sel(0);
        battery_control_pmic_protect(TST_W_KEY_HW_MODE);
    } else {
        LOG_MSGID_I(battery_management, "EOC3 recharge setting fail[%d][%d][%d]\r\n", 3,battery_check_vbus_gt_por() ,battery_get_vbus_uvlo_db(),battery_get_vbus_ovp_db());
    }
}

void battery_management_eoc3_checking(void) {
    if (!sleep_management_check_handle_status(battery_sleep_handle)) {
        hal_sleep_manager_lock_sleep(battery_sleep_handle);
    }
    pmu_power_enable_6388(PMU_LDO_VA18, PMU_ON);
    pmu_power_enable_6388(PMU_LDO_VLDO33, PMU_ON);
    hal_gpt_delay_ms(10);
    if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) < bm_cust.recharger_voltage) // recharger
    {
        battery_enable_sysldo(1);
#ifdef HAL_USB_MODULE_ENABLED
        hal_usb_phy_preinit();
#endif
        LOG_MSGID_I(battery_management, "Charger Option3 Recharge\r\n", 0);
        LOG_W(MPLOG,"Charger Option3 Recharge\r\n");/*Log output by BT*/
        battery_management_eoc3_recharger_setting();
#ifdef MTK_USB_DEMO_ENABLED
    usb_cable_detect();
#endif
    } else {
        battery_enable_sysldo(0);
        LOG_W(MPLOG,"Charger Option3 Not Recharge\r\n");/*Log output by BT*/
        LOG_MSGID_I(battery_management, "Charger Option3 Not Recharge\r\n", 0);
        //battery_auxadc_wakeup_clr(1); /*ZCV will auto re-calc when SRCLKEN high/low */
        battery_auxadc_wakeup_clr(0);
        hal_gpt_delay_ms(100);
        if ((bm_ctrl_info.chargerType != SDP_CHARGER)&&(bm_ctrl_info.chargerType != CDP_CHARGER)) {
#ifdef MTK_USB_DEMO_ENABLED
            usb_cable_detect();
#endif
        if (sleep_management_check_handle_status(battery_sleep_handle) >= 1) {
            hal_sleep_manager_unlock_sleep(battery_sleep_handle);
        }
        }
    }
}



void battery_management_eoc3_setting(void) {
    battery_control_pmic_protect(TST_W_KEY_SW_MODE);
    battery_auxadc_set_delay_cycle_timex(66);
    battery_set_plugin_db_sw_sel(1);
    //battery_auxadc_wakeup_clr(1);  /*ZCV will auto re-calc when SRCLKEN high/low */
    battery_auxadc_wakeup_clr(0);
    xTimerStartFromISR(xbm_option_setting_timer,0); //Wait 100ms for state charger machine
}


void battery_managerment_eoc_option_setting(TimerHandle_t pxTimer) {
    if (bm_ctrl_info.feature.charger_option == 2) {
        if (battery_get_faston_flag() == 1) {
            battery_set_discharger_en_sw(0);
            battery_set_discharger_en_sw_sel(0);
            battery_set_ibus_low_en(1);
            hal_gpt_delay_ms(2);
            if (battery_check_vbus_gt_por() != 0 || battery_get_vbus_uvlo_db() != 0 || battery_get_vbus_ovp_db() != 0) {
                LOG_MSGID_I(battery_management, "EOC2 EOC setting fail\r\n", 0);
            } else {
                LOG_MSGID_I(battery_management, "option2 Start To Sleep\r\n", 0);
                pmu_control_clear_interrupt(PMU_INT_MAX);
            }
        }
    } else if (bm_ctrl_info.feature.charger_option == 3) {
        if (battery_get_faston_flag() == 1) {
            battery_set_ibus_low_en(1);
        } else {
            LOG_W(MPLOG,"EOC3 EOC setting fail\r\n");/*Log output by BT*/
            LOG_MSGID_I(battery_management, "EOC3 EOC setting fail\r\n", 0);
        }
        battery_set_ppfet_lp_mode(1);
        hal_gpt_delay_ms(2);
        if (battery_check_vbus_gt_por() == 0 && battery_get_faston_flag() == 0 && battery_get_vbus_uvlo_db() == 0 && battery_get_vbus_ovp_db() == 0) {
            LOG_MSGID_I(battery_management, "Option3 Ready to Sleep", 0);
            battery_charger_suspend_mode(1);
            hal_gpt_delay_ms(1);
            LOG_MSGID_I(battery_management, "Option3 srclken", 0);
            pmu_control_clear_interrupt(PMU_INT_MAX);
            if(sleep_management_check_handle_status(battery_sleep_handle)>=1){
                hal_sleep_manager_unlock_sleep(battery_sleep_handle);
            }
        } else {
            LOG_MSGID_I(battery_management, "EOC3 EOC setting fail\r\n", 0);
        }
    } else {
        LOG_W(MPLOG,"EOC option setting fail  %d \r\n",bm_ctrl_info.feature.charger_option);/*Log output by BT*/
        LOG_MSGID_I(battery_management, "EOC option setting fail  %d \r\n",1,bm_ctrl_info.feature.charger_option);
    }

}
