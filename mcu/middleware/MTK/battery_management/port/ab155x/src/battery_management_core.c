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
#include "hal_flash.h"
#ifdef HAL_USB_MODULE_ENABLED
#include "hal_usb.h"
#include "hal_usb_internal.h"
#endif
#ifdef MTK_USB_DEMO_ENABLED
#include "usb.h"
#endif

#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#endif
#include "hal.h"
#include "hal_sleep_manager_internal.h"
#include "hal_nvic_internal.h"
uint8_t gIntLogIndex = 0;
uint8_t gauge_timer_count =0;
uint8_t gauge_cardinal_number;

TimerHandle_t xbm_regular_t;
TimerHandle_t xbm_chr_detec_t;
TimerHandle_t xbm_jeita_timer;
TimerHandle_t xbm_eoc_timer;
TimerHandle_t xbm_option_setting_timer;
uint8_t chrdet_ignore_flag =BATTERY_OPERATE_OFF ;
const char *bm_charger_type[10] = { "", "SDP", "CDP", "DCP", "SS", "IPAD2_IPAD4", "IPHONE_5V_1A", "NON-STD", "DP/DM_Floating", "UNABLE_TO_IDENTIFY"};
const char *bm_charger_state[8] = { "CHARGER OFF", "PRECC", "CC", "EXTENSION", "EOC", "THERMAL", "VBAT_OVP", "SAFETY TIMER"};
static uint32_t battery_callback_index = 0;

battery_basic_data bm_cust;
TimerHandle_t xbm_chrdet_calibration_timer;
battery_managerment_control_info_t bm_ctrl_info;
log_create_module(battery_management, PRINT_LEVEL_INFO);
static bmt_callback_context_t bmt_callback[BATTERY_MANAGER_CALLBACK_FUNCTION_NUMBER];
int battery_basic_checkpoint[9] ={3662, 3707, 3744, 3774, 3806, 3848, 3911, 3984, 4064};
uint8_t eoc3_status=0;
uint8_t battery_sleep_handle;
static uint32_t bm_sw_gpt_handle;
uint8_t battery_management_init_setting_flag =0 ;
const battery_managerment_control_info_t *bm_ctrl_info_p;
static void battery_core_pmu_charger_state_change_callback(void) {
    uint32_t newState = battery_get_charger_state();
    if (bm_ctrl_info.bjt_isCharger==1 && newState != CHARGER_STATE_FASTCC) {
        battery_bjt_disable_charging();
    }
    switch (newState)
    {
        case CHARGER_STATE_CHR_OFF:
            LOG_W(MPLOG,"CHR OFF State %d\r\n", sleep_management_check_handle_status(battery_sleep_handle));/*Log output by BT*/
            LOG_MSGID_I(battery_management, "CHR OFF State %d\r\n", 1 ,sleep_management_check_handle_status(battery_sleep_handle));
            if (bm_ctrl_info.feature.charger_option == 3 && ((bm_ctrl_info.chargerType != SDP_CHARGER) && (bm_ctrl_info.chargerType != CDP_CHARGER))) {
                if(sleep_management_check_handle_status(battery_sleep_handle)>=1){
                    hal_sleep_manager_unlock_sleep(battery_sleep_handle);
                }
            }
            break;

        case CHARGER_STATE_EOC:
            LOG_W(MPLOG,"EOC State  %d \r\n",bm_ctrl_info.feature.charger_option);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "EOC State  %d \r\n",1,bm_ctrl_info.feature.charger_option);
            if (bm_ctrl_info.charger_eoc_state == 0 && EOC_CHECK_ON) {
                LOG_MSGID_I(battery_management,"First times enter EOC state, start timer",0);
                xTimerStartFromISR(xbm_eoc_timer,0);
            } else {
                if (bm_ctrl_info.feature.charger_option == 1) {
                    LOG_MSGID_I(battery_management, "option 1, Charger state will be waiting for recharge or other events.\r\n", 0);
#ifdef PMIC_AWAKE_TO_CHARGER
                    LOG_MSGID_I(battery_management, "option 1 , PMIC_AWAKE_TO_CHARGER enable.\r\n", 0);
                    if(sleep_management_check_handle_status(battery_sleep_handle)>=1){
                        hal_sleep_manager_unlock_sleep(battery_sleep_handle);
                    }
#endif
                } else if (bm_ctrl_info.feature.charger_option == 2) {
                    battery_management_eoc2_setting();
                } else if (bm_ctrl_info.feature.charger_option == 3) {
                    battery_management_eoc3_setting(); //eco3 init setting
                    if(sleep_management_check_handle_status(battery_sleep_handle)>=1){
                        hal_sleep_manager_unlock_sleep(battery_sleep_handle);
                    }
                } else {
                    assert(0);
                }
                bm_ctrl_info.feature.charger_init = 1;
                bm_ctrl_info.gauge_calibration = 5;
            }
            break;

        case CHARGER_STATE_SAFETY_TIMER_TIMEOUT:
            LOG_MSGID_I(battery_management, "Safety Timeout State\r\n", 0);
            break;

        case CHARGER_STATE_FASTCC:
            if (BATTERY_MANAGER_BJT_ENABLE == FEATURE_ON) {
                battery_enable_bjt();
            }
            break;
        case CHARGER_STATE_THR:
            LOG_W(MPLOG,"THR State\r\n");/*Log output by BT*/
            LOG_MSGID_I(battery_management, "THR State\r\n", 0);
            break;
        case CHARGER_STATE_VBAT_OVP:
            battery_get_ovp_reason();
            break;
        case CHARGER_STATE_PRECC:
            LOG_MSGID_I(battery_management, "Precc State\r\n", 0);
            break;
        case CHARGER_STATE_EXTENSION:
            LOG_MSGID_I(battery_management, "Extension State\r\n", 0);
            break;
        default:
            break;
    }
    bm_ctrl_info.chargerState = newState;
    battery_management_notification(BATTERY_MANAGEMENT_EVENT_CHARGER_STATE_UPDATE,battery_get_chr_detect_value(),newState);
}

uint8_t battery_charger_plugin_initial_setting(void) {
    battery_set_en_recharge(BATTERY_OPERATE_ON);
    battery_set_vrechg(bm_cust.recharger_threshold);//Setting recharger voltage
    return 1;
}
void battery_management_set_charger_step_timer(){
    xTimerStopFromISR(xbm_chr_detec_t, 0);
    if (xTimerStartFromISR(xbm_chr_detec_t, 0) != pdPASS) {
        LOG_MSGID_I(battery_management, "xbm_chr_detec_t xTimerStart fail\n", 0);
    }
}

void battery_managerment_charger_setting(TimerHandle_t pxTimer) {
    uint8_t chr_ste=0;
    uint8_t bm_lock=0;
    uint32_t mask_pri;
    uint32_t temp_batsns=0;
    switch (bm_ctrl_info.charger_step)
    {
        case BM_CHARGER_IN_CHECK_POWER:
            if(!sleep_management_check_handle_status(battery_sleep_handle)){
                hal_sleep_manager_lock_sleep(battery_sleep_handle);
            }
            battery_check_charger_power();
            bm_ctrl_info.charger_step = BM_CHARGER_IN_JEITA_INIT;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]",BM_CHARGER_IN_CHECK_POWER,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2,BM_CHARGER_IN_CHECK_POWER,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();
            break;

        case BM_CHARGER_IN_JEITA_INIT:
#ifndef MTK_BATTERY_MANAGEMENT_NTC_LESS
            battery_core_hw_jeita_init();
            battery_set_hw_jeita_enable(BATTERY_OPERATE_ON);
            battery_set_jeita_irq_en(BATTERY_OPERATE_ON); //1:enable 0:disable
#else
            battery_set_hw_jeita_enable(BATTERY_OPERATE_OFF);
            battery_set_jeita_irq_en(BATTERY_OPERATE_OFF);
#endif
            bm_ctrl_info.charger_step = BM_CHARGER_IN_JEITA_WC;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]", BM_CHARGER_IN_JEITA_INIT,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2,BM_CHARGER_IN_JEITA_INIT,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();
            break;

        case BM_CHARGER_IN_JEITA_WC:
#ifdef DISABLE_WARM_COOL
            pmu_control_enable_interrupt(RG_INT_JEITA_WARM,PMU_OFF);
            pmu_control_enable_interrupt(RG_INT_JEITA_COOL,PMU_OFF);
            battery_set_jeita_diswarmcool(1);

#else
            battery_set_jeita_diswarmcool(0);
#endif
            bm_ctrl_info.charger_step = BM_CHARGER_IN_USB;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]", BM_CHARGER_IN_JEITA_WC,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_IN_JEITA_WC,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();

            break;

        case BM_CHARGER_IN_USB:
#ifdef HAL_USB_MODULE_ENABLED
            hal_usb_phy_preinit();
#endif
            bm_ctrl_info.charger_step = BM_CHARGER_IN_PLUGIN_INIT;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]", BM_CHARGER_IN_USB,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_IN_USB,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();
            break;

        case BM_CHARGER_IN_PLUGIN_INIT:
            battery_set_icl_curent_level(ICL_ITH_90mA);
            battery_charger_plugin_initial_setting(); //init for charger
            battery_disable_vsys_discharge(0);
            bm_ctrl_info.charger_step = BM_CHARGER_IN_BC12;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]",BM_CHARGER_IN_PLUGIN_INIT,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_IN_PLUGIN_INIT,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();
            break;

        case BM_CHARGER_IN_BC12:
#ifdef BATTERY_FEATURE_BC1P2
            bm_ctrl_info.chargerType = battery_management_get_charger_type_bc12();
            battery_management_charger_type_setting(bm_ctrl_info.chargerType);
#else
            battery_management_charger_type_setting(NON_STD_CHARGER);
#endif
            bm_ctrl_info.charger_step = BM_CHARGER_IN_ENABLE;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]",BM_CHARGER_IN_BC12,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_IN_BC12,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();
            break;

        case BM_CHARGER_IN_ENABLE:
		    battery_set_precc_ith(bm_cust.precc_cur);         // precc iterm setting
			battery_set_iterm_ith(bm_cust.cv_termination);    // iterm ith setting
            temp_batsns = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
            if ((bm_ctrl_info.feature.feature_bjt == FEATURE_ON) && ((int) battery_get_charger_state() == CHARGER_STATE_FASTCC)) {
                bm_ctrl_info.bjt_isCharger = BATTERY_OPERATE_ON;
            }
            hal_nvic_save_and_set_interrupt_mask_special(&mask_pri);
            battery_set_enable_charger(BATTERY_OPERATE_ON);
            hal_nvic_restore_interrupt_mask_special(mask_pri);
            if (xTimerStartFromISR(xbm_regular_t, 0) != pdPASS) {
                LOG_MSGID_I(battery_management, "xbm_jeita_Timer xTimerStart fail\n", 0);
            }

            if ((temp_batsns >= bm_cust.s1_voltage) && (temp_batsns <= bm_cust.s2_voltage)) {
                LOG_MSGID_I(battery_management,"Battery voltage detect step charger S1 %d",1,bm_cust.s1_voltage);
                battery_multiple_level(bm_cust.s1_multi_c);
            }

            if (temp_batsns > bm_cust.s2_voltage) {
                LOG_MSGID_I(battery_management,"Battery voltage detect step charger S2 %d",1,bm_cust.s2_voltage);
                battery_multiple_level(bm_cust.s2_multi_c);
                gauge_cardinal_number = ((CHARGER_TOLERANCE_TIME * 60) / CHARGER_REGULAR_TIME) / 6;
            }

            bm_ctrl_info.charger_step = BM_CHARGER_IN_GAUGE_CALI;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]",BM_CHARGER_IN_ENABLE,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_IN_ENABLE,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();

            break;
        case BM_CHARGER_IN_GAUGE_CALI:

            if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) > (bm_cust.full_bat - bm_cust.full_bat_offset)) {
                bm_ctrl_info.gauge_calibration = ((pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) - (bm_cust.full_bat - bm_cust.full_bat_offset)) / 10) + 1;
            } else {
                bm_ctrl_info.gauge_calibration = 0;
            }
            bm_ctrl_info.charger_step = BM_CHARGER_NOTIFICATION;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]", BM_CHARGER_IN_GAUGE_CALI,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_IN_GAUGE_CALI,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();
            break;

        case BM_CHARGER_OUT_CHECK_POWER:
            battery_disable_vsys_discharge(0x2);
            bm_ctrl_info.feature.charger_init = 0;
            bm_ctrl_info.charger_step = BM_CHARGER_OUT_JEITA_OFF;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]",BM_CHARGER_OUT_CHECK_POWER,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_OUT_CHECK_POWER,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();
            break;

        case BM_CHARGER_OUT_JEITA_OFF:
#ifndef MTK_BATTERY_MANAGEMENT_NTC_LESS
            battery_set_hw_jeita_enable(BATTERY_OPERATE_OFF);
            battery_set_jeita_irq_en(BATTERY_OPERATE_OFF); //1:enable 0:disable
            xTimerStopFromISR(xbm_jeita_timer, 0);
#endif
            bm_ctrl_info.charger_step = BM_CHARGER_OUT_EOC_EXIT;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]", BM_CHARGER_OUT_JEITA_OFF,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_OUT_JEITA_OFF,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();

            break;

        case BM_CHARGER_OUT_EOC_EXIT:
            if (bm_ctrl_info.feature.charger_option == 1) {
            } else if (bm_ctrl_info.feature.charger_option == 2) {
                battery_management_eoc2_exit_setting();
            } else if (bm_ctrl_info.feature.charger_option == 3) {
                battery_management_eoc3_exit_setting();
            } else {
                assert(0);
            }
            bm_ctrl_info.charger_step = BM_CHARGER_OUT_GAUGE_CALI;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]", BM_CHARGER_OUT_EOC_EXIT,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_OUT_EOC_EXIT,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();

            break;

        case BM_CHARGER_OUT_GAUGE_CALI:
            if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) > (bm_cust.full_bat - bm_cust.full_bat_offset) && bm_ctrl_info.gauge_calibration == 0) {
                bm_ctrl_info.gauge_calibration = ((pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) - (bm_cust.full_bat - bm_cust.full_bat_offset)) / 10);
            } else if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) < (bm_cust.full_bat - bm_cust.full_bat_offset)) {
                xTimerStopFromISR(xbm_regular_t, 0);
            }
            gauge_timer_count = 0;
            if(sleep_management_check_handle_status(battery_sleep_handle)>=1){
                hal_sleep_manager_unlock_sleep(battery_sleep_handle);
            }
            pmu_lock_va18(PMU_OFF);
            bm_ctrl_info.charger_step = BM_CHARGER_NOTIFICATION;
            LOG_W(MPLOG,"BM_CHARGER_STEP[%d]->[%d]",BM_CHARGER_OUT_GAUGE_CALI,bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2, BM_CHARGER_OUT_GAUGE_CALI,bm_ctrl_info.charger_step);
            battery_management_set_charger_step_timer();

            break;

        case BM_CHARGER_NOTIFICATION:
#ifdef MTK_USB_DEMO_ENABLED
            usb_cable_detect();
#endif
#ifdef CHARGER_CALIBRATION
            xTimerStartFromISR(xbm_chrdet_calibration_timer,0);
#endif
            bm_ctrl_info.charger_step = BM_CHARGER_DONE;
            xTimerStopFromISR(xbm_chr_detec_t, 0);
            if(battery_management_init_setting_flag==0){
               battery_charger_check_faston(); /*for BAT_OVP when FAST_ON*/
            }
            /*Avoid plug in/out too fast and causing charger Confused. */
            if ((bm_ctrl_info.feature.charger_option == 3) &&
                (battery_get_charger_state() == CHARGER_STATE_CHR_OFF) &&
                (sleep_management_check_handle_status(battery_sleep_handle)>=1)&&
                (bm_ctrl_info.isChargerExist==BATTERY_OPERATE_ON)) {
                do {
                    bm_lock = sleep_management_check_handle_status(battery_sleep_handle);
                    chr_ste = bm_ctrl_info.isChargerExist;
                    if(sleep_management_check_handle_status(battery_sleep_handle)>=1){
                        hal_sleep_manager_unlock_sleep(battery_sleep_handle);
                    }
                } while ((bm_lock & chr_ste) >= 1);
            }

            LOG_W(MPLOG,"[%d]notification update", bm_ctrl_info.charger_step);/*Log output by BT*/
            LOG_MSGID_I(battery_management, "[%d]notification update",1, bm_ctrl_info.charger_step);
            battery_management_notification(BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE,battery_get_chr_detect_value(),battery_get_charger_state());
            battery_core_pmu_charger_state_change_callback();
            break;
    }
}

static void battery_charger_detect_callback(void) {
    uint32_t mask_pri;
    pmu_power_enable_6388(PMU_LDO_VA18, PMU_ON);
    hal_nvic_save_and_set_interrupt_mask_special(&mask_pri);
    bm_ctrl_info.isChargerExist = battery_get_chr_detect_value();
    if (bm_ctrl_info.isChargerExist) {
        bm_ctrl_info.charger_step = BM_CHARGER_IN_CHECK_POWER;
        if (xTimerStartFromISR(xbm_chr_detec_t, 0) != pdPASS) {
            LOG_MSGID_I(battery_management, "xbm_chr_detec_t xTimerStart fail\n", 0);
        }
        hal_nvic_restore_interrupt_mask_special(mask_pri);
    } else {
        pmu_power_enable_6388(PMU_LDO_VA18, PMU_ON);
        xTimerStopFromISR(xbm_chr_detec_t, 0);
        bm_ctrl_info.charger_step = BM_CHARGER_OUT_CHECK_POWER;
        if (xTimerStartFromISR(xbm_chr_detec_t, 0) != pdPASS) {
            LOG_MSGID_I(battery_management, "xbm_chr_detec_t xTimerStart fail\n", 0);
        }
        hal_nvic_restore_interrupt_mask_special(mask_pri);
    }

}

static void battery_core_pmu_rechg_callback(void) {
    LOG_MSGID_I(battery_management, "Recharge Interrupt Callback function\r\n", 0);
    LOG_W(MPLOG,"Recharge Interrupt Callback function\r\n");/*Log output by BT*/
    if (!sleep_management_check_handle_status(battery_sleep_handle)) {
        hal_sleep_manager_lock_sleep(battery_sleep_handle);
    }
    if(bm_ctrl_info.feature.charger_option == 1){
        LOG_MSGID_I(battery_management, "Option 1 recharger be trigged", 0);
    }else if(bm_ctrl_info.feature.charger_option == 2){
        battery_management_eoc2_recharger_interrupt();
    }else if(bm_ctrl_info.feature.charger_option == 3){
        battery_management_eoc3_checking();
    }else{
        LOG_MSGID_I(battery_management, "recharger in empty option", 0);
    }

}

static void battery_core_pmu_thm_over110_callback(void) {
    LOG_MSGID_I(battery_management, "THM_OVER 110 Trigger\r\n", 0);
    LOG_MSGID_I(battery_management, "Set ICL 500mA\r\n", 0);
    battery_set_icl_curent_level(ICL_ITH_500mA);
}

void battery_management_trim_ic_init(void) {
    if (!battery_check_reserved_efuse_trim())
    {
#ifndef MTK_BATTERY_MANAGEMENT_NTC_LESS
        battery_set_jeita_cool_volt(4095);
        battery_set_jeita_cold_volt(4095);
#endif
        LOG_MSGID_I(battery_management, "SW Trim\r\n", 0);
        battery_selet_auxadc_strup(1);
        battery_selet_auxadc_start_sw_path(0);
    } else {
        LOG_MSGID_I(battery_management, "IC has been Trim\r\n", 0);
    }
}
/*Battery Callback function template*/
static void Battery_management_monitor(battery_management_event_t event, const void *data)
{
    bm_ctrl_info_p = data;
    if (event == BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE) {
        LOG_MSGID_I(battery_management,"[BM]EVENT:[CHARGER EXIST UPDATE:%d] [Charger Exist:%x] ",2,event,bm_ctrl_info_p->isChargerExist);
    }
    if (event == BATTERY_MANAGEMENT_EVENT_CHARGER_STATE_UPDATE) {
        LOG_MSGID_I(battery_management,"[BM]EVENT:[CHARGER STATE UPDATE:%d] [Charger State:%x]",2,event,bm_ctrl_info_p->chargerState);
    }
}
void battery_management_interrupt_register(void) {
    pmu_register_callback(RG_INT_CHRDET, battery_charger_detect_callback, NULL);
    pmu_register_callback(RG_INT_ChgStatInt, battery_core_pmu_charger_state_change_callback, NULL);
    pmu_register_callback(RG_INT_VBAT_RECHG, battery_core_pmu_rechg_callback, NULL);
    pmu_register_callback(RG_INT_THM_OVER110, battery_core_pmu_thm_over110_callback, NULL);
    if (battery_management_register_callback(Battery_management_monitor) != BATTERY_MANAGEMENT_STATUS_OK) {
        LOG_MSGID_I(battery_management,"Cannot register battery callback",0);
    }
}

int32_t battery_management_get_battery_property_internal(battery_property_t property)
{
    int32_t property_value;
    switch (property)
    {
        case BATTERY_PROPERTY_CAPACITY:
            property_value = battery_get_linear_gauge_percent();
            break;
        case BATTERY_PROPERTY_CAPACITY_LEVEL :
            property_value = battery_get_linear_gauge_percent_level();
            break;
        case BATTERY_PROPERTY_CHARGER_EXIST:
            property_value = battery_get_chr_detect_value();
            break;
        case BATTERY_PROPERTY_CHARGER_TYPE:
            property_value = bm_ctrl_info.chargerType;
            break;
        case BATTERY_PROPERTY_TEMPERATURE:
            property_value = battery_auxadc_voltage_to_tempature(pmu_auxadc_get_channel_value(PMU_AUX_CHR_THM));
            break;
        case BATTERY_PROPERTY_VOLTAGE:
            property_value = pmu_auxadc_get_channel_value(PMU_AUX_BATSNS);
            break;
        case BATTERY_PROPERTY_VOLTAGE_IN_PERCENT:
            property_value = battery_get_linear_gauge_percent();
            break;
        case BATTERY_PROPERTY_PMIC_TEMPERATURE:
            property_value = battery_get_pmic_temp();
            break;
        case BATTERY_PROPERTY_CHARGER_STATE:
            property_value = battery_get_charger_state();
            break;
        default:
            property_value = BATTERY_INVALID_VALUE;
            break;
    }
    return property_value;
}

void battery_management_notification(battery_management_event_t event,uint32_t chr_exist,uint32_t state) {
    int i = 0;
    LOG_W(MPLOG,"[BM Notification : %d][Chr_exist:%d][Chr_state:%d]\r\n",event,chr_exist,state);/*Log output by BT*/
    for (i = 0; i < battery_callback_index; i++) {
         if ((bmt_callback[i].callback_init == true) && (bmt_callback[i].func != NULL)) {
         bmt_callback[i].func(event, &bm_ctrl_info);
         }
    }
}
void battery_calibration_gauge_tolerance(){
    if (battery_get_charger_state() == 0x2 || battery_get_charger_state() == 0x4) {
        gauge_timer_count++;
        if (gauge_timer_count == gauge_cardinal_number) {
            bm_ctrl_info.gauge_calibration += 1;
            gauge_timer_count = 0;
        }
        if (bm_ctrl_info.gauge_calibration >= 5) {
            bm_ctrl_info.gauge_calibration = 5;
        }
    } else if (battery_get_charger_state() != 0x2&& battery_get_chr_detect_value()!=1 && bm_ctrl_info.gauge_calibration > 0) {
        gauge_timer_count++;
        if (gauge_timer_count == gauge_cardinal_number) {
            bm_ctrl_info.gauge_calibration -= 1;
            gauge_timer_count = 0;
        }
        if (bm_ctrl_info.gauge_calibration <= 0) {
            bm_ctrl_info.gauge_calibration = 0;
        }
    }
}

void battery_init_check_charger_exist(void) {
    if (pmu_get_usb_input_status()) {
        battery_charger_detect_callback();
        battery_core_pmu_charger_state_change_callback();
        battery_clear_charger_irq();
    }
}
void  battery_managerment_eoc_timer_callback(TimerHandle_t pxTimer) {
    LOG_MSGID_I(battery_management,"First times enter EOC state, stop timer",0);
    bm_ctrl_info.charger_eoc_state = 1;
    battery_core_pmu_charger_state_change_callback();
}
void battery_managerment_regular_timer_callback(TimerHandle_t pxTimer) {
    LOG_MSGID_I(battery_management, "BM regular timer callback\r\n", 0);
    if (bm_ctrl_info.feature.message_task == FEATURE_ON) {
        battery_management_notification(BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE,battery_get_chr_detect_value(),battery_get_charger_state());
        bm_ctrl_info.message.event = BMT_EVENT_10_TIMEOUT;
        xQueueSendFromISR(bm_ctrl_info.message.bmt_queue_handle, &bm_ctrl_info.message.event, 0/portTICK_PERIOD_MS);
    }
    if (bm_ctrl_info.isChargerExist == true) {
        //EOC in option3,check battery behavior and recharger
        if (bm_ctrl_info.feature.charger_option == 3 && battery_get_charger_state() ==CHARGER_STATE_CHR_OFF) {
            battery_management_eoc3_checking();
        }
        if (bm_ctrl_info.feature.charger_option == 3 && battery_get_charger_state() ==CHARGER_STATE_EOC && bm_ctrl_info.charger_eoc_state==1) {
            battery_management_eoc3_setting();
        }
        if (bm_ctrl_info.feature.feature_bjt == FEATURE_ON) {
            battery_bjt_check_ovp(); //Protect battery voltage overlay
        }
    }
    if((pmu_auxadc_get_channel_value(PMU_AUX_BATSNS)>=bm_cust.s1_voltage) &&(pmu_auxadc_get_channel_value(PMU_AUX_BATSNS)<=bm_cust.s2_voltage)){
        LOG_MSGID_I(battery_management,"Battery voltage detect step charger S1 %d",1,bm_cust.s1_voltage);
        battery_multiple_level(bm_cust.s1_multi_c);
    }
    if(pmu_auxadc_get_channel_value(PMU_AUX_BATSNS)>bm_cust.s2_voltage){
        LOG_MSGID_I(battery_management,"Battery voltage detect step charger S2 %d",1,bm_cust.s2_voltage);
        battery_multiple_level(bm_cust.s2_multi_c);
        gauge_cardinal_number = ((CHARGER_TOLERANCE_TIME *60 )/CHARGER_REGULAR_TIME)/6;
    }
    if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) > (bm_cust.full_bat - bm_cust.full_bat_offset)){
        battery_calibration_gauge_tolerance();
    }
    LOG_W(MPLOG,"[BM]Battery temperature = %d(Celsius degrees)" ,(int)battery_management_get_battery_property(BATTERY_PROPERTY_TEMPERATURE));/*Log output by BT*/
    LOG_W(MPLOG,"[BM]Battery capacity = %d(%%) ",(int)battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY));/*Log output by BT*/
    LOG_W(MPLOG,"[BM]Charger status = %d ",(int)battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST));/*Log output by BT*/
    LOG_W(MPLOG,"[BM]Charger type = %s",bm_charger_type[battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_TYPE)]);/*Log output by BT*/
    LOG_W(MPLOG,"[BM]Battery voltage = %d(mV)\n",(int) battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE));/*Log output by BT*/
    LOG_W(MPLOG,"[BM]Battery State = %s\n",(int) bm_charger_state[battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE)]);/*Log output by BT*/
    LOG_W(MPLOG,"[BM]Multi level(CC) = %d\n",(int)(1+battery_get_multi_level()));/*Log output by BT*/
    LOG_W(MPLOG,"[BM]bm_ctrl_info.gauge_calibration = %d[%d]\n",(int)bm_ctrl_info.gauge_calibration,gauge_cardinal_number);/*Log output by BT*/
    LOG_W(MPLOG,"[BM]bm_ctrl_info.feature.charger_option = %d[%d]\n",(int)bm_ctrl_info.feature.charger_option);/*Log output by BT*/

}

#ifdef CHARGER_CALIBRATION
/*This API is for fixing human actions or environment exceptions in the charger plug-in/plug-out*/
void battery_detect_calibration_timer_callback(TimerHandle_t pxTimer){
    if ((bm_ctrl_info.isChargerExist != battery_get_chr_detect_value())) {
        LOG_MSGID_I(battery_management, "ERROR!!!!, Plug-in/out interval need 500ms\r\n", 0);
        battery_charger_detect_callback();
    }
}
#endif
void battery_management_parameter_init(){
    battery_set_icl_curent_level(ICL_ITH_90mA);
    LOG_MSGID_I(battery_management, "Change ICL Ith to 90mA\r\n", 0);
#ifndef MTK_BATTERY_MANAGEMENT_NTC_LESS
    if ( BATTERY_MANAGER_JEITA_ENABLE == FEATURE_ON) {
        LOG_MSGID_I(battery_management, "(Charger on)HW JEITA Init\r\n", 0);
        bm_ctrl_info.feature.feature_jeita = 1;
        battery_core_hw_jeita_init();
    } else {
        bm_ctrl_info.feature.feature_jeita = 0;
    }
#else
    battery_set_hw_jeita_enable(BATTERY_OPERATE_OFF);
    battery_set_jeita_irq_en(BATTERY_OPERATE_OFF);
    battery_set_enable_charger(BATTERY_OPERATE_ON);
    bm_ctrl_info.feature.feature_jeita = 0;
#endif
    bm_ctrl_info.feature.charger_option  = BATTERY_MANAGER_DEFAULT_CHARGER_OPTION;
    bm_ctrl_info.chargerState = battery_get_charger_state();

    if (BATTERY_MESSAGE_QUEUE_TASK == FEATURE_ON) {
        bm_ctrl_info.feature.message_task = 1;
    } else {
        bm_ctrl_info.feature.message_task = 0;
    }
    if (BATTERY_MANAGER_BJT_ENABLE == FEATURE_ON) {
        bm_ctrl_info.feature.feature_bjt = 1;
        battery_enable_bjt();
    } else {
        bm_ctrl_info.feature.feature_bjt = 0;
    }
    if (CHARGER_MULTIPLE_DEFAULT_LEVEL == 0) {
        bm_ctrl_info.charger_5c = 0;
    } else {
        bm_ctrl_info.charger_5c = CHARGER_MULTIPLE_DEFAULT_LEVEL;
        battery_config_fast_charger_setting(TSTEP_ICC_MULTIPLE_TIME_16uS, CHARGER_MULTIPLE_DEFAULT_LEVEL);
    }
    battery_set_extend_charger_time(EXTEND_TIME);
    gauge_cardinal_number = ((CHARGER_TOLERANCE_TIME *60 )/CHARGER_REGULAR_TIME)/6;
    if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) > (bm_cust.full_bat - bm_cust.full_bat_offset)&&battery_get_charger_state()==0x2) {
        battery_set_enable_charger(0);
        bm_ctrl_info.gauge_calibration = ((pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) - (bm_cust.full_bat - bm_cust.full_bat_offset)) / 10) + 1;
        battery_set_enable_charger(1);
    }else if (pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) > (bm_cust.full_bat - bm_cust.full_bat_offset)){
        bm_ctrl_info.gauge_calibration = ((pmu_auxadc_get_channel_value(PMU_AUX_BATSNS) - (bm_cust.full_bat - bm_cust.full_bat_offset)) / 10);
    }else{
        bm_ctrl_info.gauge_calibration = 0;
    }
    battery_sleep_handle = hal_sleep_manager_set_sleep_handle("Battery");
    bm_ctrl_info.charger_step = BM_CHARGER_IN_CHECK_POWER;
}

void battery_management_message_task_init(){
    bm_ctrl_info.message.task_handle = NULL;
    bm_ctrl_info.message.event = BMT_EVENT_INIT;
    bm_ctrl_info.message.bmt_queue_handle = xQueueCreate(1,sizeof(bm_ctrl_info.message.event));
}


void battery_management_timer_init() {
    xbm_regular_t = xTimerCreate("charger_regular_timer", (CHARGER_REGULAR_TIME * TIMEOUT_PERIOD_1S), pdTRUE, NULL, battery_managerment_regular_timer_callback);
    xbm_chr_detec_t = xTimerCreate("charger_regular_timer", 10*TIMEOUT_PERIOD_1MS, pdFALSE, NULL, battery_managerment_charger_setting);
#ifndef MTK_BATTERY_MANAGEMENT_NTC_LESS
    xbm_jeita_timer = xTimerCreate("jeita_Timer", (HW_JEITA_CHECK_INTERVAL_TIME * TIMEOUT_PERIOD_1S), pdTRUE, NULL, battery_jetia_timer_callback);
#endif
#ifdef CHARGER_CALIBRATION
    xbm_chrdet_calibration_timer = xTimerCreate("charger_detect_Timer", (CALIBRATION_TIME * portTICK_PERIOD_MS), pdFALSE, NULL, battery_detect_calibration_timer_callback);
#endif
    xbm_option_setting_timer = xTimerCreate("xbm_option_setting_timer", (100 * TIMEOUT_PERIOD_1MS), pdFALSE, NULL, battery_managerment_eoc_option_setting);
    xbm_eoc_timer = xTimerCreate("charger_eoc_timer", (BATTERY_EOC_CHECK_TIME * TIMEOUT_PERIOD_1S), pdFALSE, NULL, battery_managerment_eoc_timer_callback);
    if (hal_gpt_sw_get_timer(&bm_sw_gpt_handle) != HAL_GPT_STATUS_OK) {
        log_hal_error("ERROR : BM GPT Init Fail");
    }
}

#ifdef BATTERY_CUSTOMER_SETTING
battery_customer_setting_t bm_customer_setting[] = {
        { 0, 0 },
        { 0, 0 }
};
void battery_manager_customer_setting() {
    int custom_setting, i;
    custom_setting = sizeof(bm_customer_setting) / sizeof(bm_customer_setting[0]);
    for (i = 0; i < custom_setting; i++) {
        pmu_set_register_value_2byte_mt6388(bm_customer_setting[i].addr, 0xFFFF, 0, bm_customer_setting[i].value);
    }
}
#endif
battery_management_status_t battery_management_init_internal(void) {
    LOG_MSGID_I(battery_management, "Battery Management 1.0.2\r\n",0);
#ifdef MTK_NVDM_ENABLE
    battery_init_date_form_nvdm();
#else
    battery_init_basic_data();
#endif
    battery_management_timer_init();
    battery_management_parameter_init();
    battery_management_message_task_init();
    if (bm_ctrl_info.feature.message_task == FEATURE_ON) {
        xTimerStart(xbm_regular_t, portMAX_DELAY);
    }
    battery_management_trim_ic_init();
    battery_set_precc_ith(bm_cust.precc_cur);     //precc iterm init
    battery_set_iterm_ith(bm_cust.cv_termination);    //precc iterm init
    battery_auxadc_parameter_init();
#if BATTERY_TIMER_ACCELERATE  // LCHR 1S HIT Speed up for timer (for debug)
    battery_core_pmu_turbo_the_timer(1);
#endif
#ifdef BATTERY_CUSTOMER_SETTING
    battery_manager_customer_setting();
#endif
    battery_management_interrupt_register();
    battery_init_check_charger_exist();
    /*When no charging, battery voltage is lower than initial battery or shutdown battery ,it will enter rtc mode*/
#ifdef BATTERY_CHECK_BATTERY_VOLTAGE
    if((pmu_get_usb_input_status()!=true) &&
       (bm_cust.initial_bat  > battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE) ||
       bm_cust.shutdown_bat > battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE))){
        LOG_MSGID_I(battery_management, "Battery voltage low\r\n", 0);
        hal_rtc_enter_rtc_mode();
    }
#endif
    LOG_MSGID_I(battery_management,"Battery Capacity:%d %",1,battery_get_linear_gauge_percent());
    battery_management_init_setting_flag =1;
    return BATTERY_MANAGEMENT_STATUS_OK;
}

battery_management_status_t battery_management_register_callback_internal(battery_management_callback_t callback)
{
    bmt_callback[battery_callback_index].func = callback;
    bmt_callback[battery_callback_index].callback_init = true;
    battery_callback_index++;
    return BATTERY_MANAGEMENT_STATUS_OK;
}

bmt_function_t battery_func = {
    battery_management_init_internal,
    battery_management_get_battery_property_internal,
    battery_management_register_callback_internal,
    NULL
};

void battery_management_enable_charger(battery_managerment_operate_t oper){
    battery_set_enable_charger(oper);
}

uint32_t battery_management_set_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value){
    return battery_set_pmic_register_value(address, mask, shift, value);
}
uint32_t battery_management_get_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift){
    return battery_get_pmic_register_value(address, mask,shift);
}

uint8_t battery_get_full_battery_index(uint16_t vabt) {
    int i = 0, index = 3; //Battery default value
    int battery_full_bat_voltage[BATTERY_FULLBAT_INDEX_MAX] = { 4050, 4100, 4150, 4200, 4250, 4300, 4350, 4400, 4450, 4500, 4550, 4600 };
    for (i = 0; i < BATTERY_FULLBAT_INDEX_MAX; i++) {
        if (vabt == battery_full_bat_voltage[i]) {
            index = i;
        }
    }
    return index;
}
uint8_t battery_get_recharger_index(uint16_t vol) {
    int i = 0, index = 1; //recharger default value
    int battery_recharger_threshold[BATTERY_RECHARGER_INDEX_MAX] = { 50, 100, 150, 200 };
    for (i = 0; i < BATTERY_RECHARGER_INDEX_MAX; i++) {
        if (vol == battery_recharger_threshold[i]) {
            index = i;
        }
    }
    return index;
}
battery_management_status_t battery_init_date_form_nvdm(){
    int i = 0 ;
    uint8_t status = 0 ;
    uint8_t temp_inde =100; //decharger default value
    uint32_t leng;
    uint8_t buffer_c[152];
    uint8_t buffer_b[128];
    int checkpoint = 14;

    leng = sizeof(buffer_b);
    status = nvkey_read_data(NVKEYID_BATTERY_MANAGEMENT_BASIC, (uint8_t *) (buffer_b), &leng);
    if (status != NVDM_STATUS_OK) {
        LOG_MSGID_I(battery_management, "NVDM data error",0);
        battery_init_basic_data();
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }
    leng = sizeof(buffer_c);
    status = nvkey_read_data(NVKEYID_BATTERY_MANAGEMENT_CHR, (uint8_t *) (buffer_c), &leng);
    if (status != NVDM_STATUS_OK) {
        LOG_MSGID_I(battery_management, "NVDM data error",0);
        battery_init_basic_data();
        return BATTERY_MANAGEMENT_STATUS_ERROR;
    }

    LOG_MSGID_I(battery_management, "Get battery basic value form NVDM ", 0);
    bm_cust.initial_bat = (buffer_c[2] << 8) + buffer_c[1];
    bm_cust.shutdown_bat = (buffer_c[10] << 8) + buffer_c[9];
    bm_cust.full_bat = (buffer_b[25] << 8) + buffer_b[24];
    bm_cust.full_bat_offset = BATTERY_CAPACITY_OFFSET;
    bm_cust.recharger_voltage = (buffer_b[33] << 8) + buffer_b[32];
    bm_cust.precc_cur = (buffer_b[41] << 8) + buffer_b[40];
    bm_cust.cv_termination = (buffer_b[43] << 8) + buffer_b[42];
    bm_cust.s1_voltage = (buffer_b[45] << 8) + buffer_b[44];
    bm_cust.s1_multi_c = (buffer_b[47] << 8) + buffer_b[46];
    bm_cust.s2_voltage = (buffer_b[49] << 8) + buffer_b[48];
    bm_cust.s2_multi_c = (buffer_b[51] << 8) + buffer_b[50];
    bm_cust.cool_cc = (buffer_b[53] << 8) + buffer_b[52];
    bm_cust.cool_cv = (buffer_b[55] << 8) + buffer_b[54];
    bm_cust.warm_cc = (buffer_b[57] << 8) + buffer_b[56];
    bm_cust.warm_cv = (buffer_b[59] << 8) + buffer_b[58];
    battery_select_vcv_voltage(battery_get_full_battery_index(bm_cust.full_bat)); //set VCV_VTH
    for (i = 0; i < 9; i++) {
        bm_cust.check_point[i] = (buffer_c[checkpoint] << 8) + buffer_c[checkpoint - 1];
        LOG_MSGID_I(battery_management,"Check Point[%d][%d] \r\n",2,i,bm_cust.check_point[i]);
        checkpoint += 4;
    }
    LOG_MSGID_I(battery_management,"precc_cur[%d]cv_termination[%d]",2,bm_cust.precc_cur,bm_cust.cv_termination);
    LOG_MSGID_I(battery_management,"s1_v[%d]s1_m[%d]s2_v[%d]s2_m[%d]",4,bm_cust.s1_voltage,bm_cust.s1_multi_c,bm_cust.s2_voltage,bm_cust.s2_multi_c);
    LOG_MSGID_I(battery_management,"cool_cc[%d]cool_cv[%d]warm_cc[%d]warm_cv[%d]",4,bm_cust.cool_cc,bm_cust.cool_cv,bm_cust.warm_cc,bm_cust.warm_cv);
    LOG_MSGID_I(battery_management,"initial_bat[%d]shutdown_bat[%d]full_bat[%d]full_bat_offset[%d]recharger_voltage[%d]",5,bm_cust.initial_bat,bm_cust.shutdown_bat,bm_cust.full_bat,bm_cust.full_bat_offset,bm_cust.recharger_voltage);


    bm_cust.jeita.cold = (buffer_c[54] << 8) + buffer_c[53];
    bm_cust.jeita.cool = (buffer_c[58] << 8) + buffer_c[57];
    bm_cust.jeita.warm = (buffer_c[62] << 8) + buffer_c[61];
    bm_cust.jeita.hot = (buffer_c[66] << 8) + buffer_c[65];
    temp_inde = (buffer_c[70] << 8) + buffer_c[69];
    bm_cust.recharger_threshold = battery_get_recharger_index(temp_inde);
    LOG_MSGID_I(battery_management,"cold[%d]cool[%d]warm[%d]hot[%d]recharger_threshold[%d]",5,bm_cust.jeita.cold,bm_cust.jeita.cool,bm_cust.jeita.warm,bm_cust.jeita.hot,bm_cust.recharger_threshold);
    return BATTERY_MANAGEMENT_STATUS_OK;
}

battery_management_status_t battery_init_basic_data() {
    int i = 0;
    LOG_MSGID_I(battery_management,"Get battery default value",0);
    bm_cust.initial_bat = INITIAL_BAT;
    bm_cust.shutdown_bat = SW_SHUT_DOWN;
    bm_cust.full_bat = FULL_BATTERY;
    bm_cust.full_bat_offset = BATTERY_CAPACITY_OFFSET;
    bm_cust.recharger_voltage = EOC_RECHARGER_VOLTAGE;
    for (i = 0; i < 9; i++) {
        bm_cust.check_point[i] = battery_basic_checkpoint[i];
    }
    bm_cust.recharger_threshold = RECHARGER_VOLTAGE;
    bm_cust.jeita.cold = HW_JEITA_COLD_THRESHOLD;
    bm_cust.jeita.cool = HW_JEITA_COOL_THRESHOLD;
    bm_cust.jeita.warm = HW_JEITA_WARM_THRESHOLD;
    bm_cust.jeita.hot  = HW_JEITA_HOT_THRESHOLD;
    bm_cust.precc_cur = BATTERY_PRECC_CURRENT;
    bm_cust.cv_termination = BATTERY_PRECC_CV_TERM;
    bm_cust.s1_voltage = BATTERY_S1_VOLTAGE;
    bm_cust.s1_multi_c = BATTERY_S1_MULTI_C;
    bm_cust.s2_voltage = BATTERY_S2_VOLTAGE;
    bm_cust.s2_multi_c = BATTERY_S2_MULTI_C;
    bm_cust.cool_cc = BATTERY_COOL_CC;
    bm_cust.cool_cv = BATTERY_COOL_CV;
    bm_cust.warm_cc = BATTERY_WARM_CC;
    bm_cust.warm_cv = BATTERY_WARM_CV;
    return BATTERY_MANAGEMENT_STATUS_OK;
}

battery_basic_data battery_management_get_basic_data(){
    return bm_cust;
}
void battery_switch_charger_option(int option){
    bm_ctrl_info.feature.charger_option = option;
}
/*This feature for smart charger case
 *If chrdet_ignore_flag=1,ignore charger detect.  */
void battery_chager_ignore_detect(battery_managerment_operate_t sta){
    if(sta==BATTERY_OPERATE_OFF){
        chrdet_ignore_flag = BATTERY_OPERATE_OFF;
    }else{
        chrdet_ignore_flag = BATTERY_OPERATE_ON;
    }
}

void battery_management_debug_info(){
    int i=0;
    LOG_MSGID_I(battery_management,"===HW JEITA Check==\r\n",0);
    LOG_MSGID_I(battery_management, "EOC State  %d \r\n",1,bm_ctrl_info.feature.charger_option);
    LOG_MSGID_I(battery_management, "HOT_THRESHOLD:[%d]\r\n",1,(int)pmu_get_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_HOT_ADDR, PMU_AUXADC_JEITA_VOLT_HOT_MASK,PMU_AUXADC_JEITA_VOLT_HOT_SHIFT));
    LOG_MSGID_I(battery_management, "WARM_THRESHOLD:[%d]\r\n",1,(int)pmu_get_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_WARM_ADDR, PMU_AUXADC_JEITA_VOLT_WARM_MASK,PMU_AUXADC_JEITA_VOLT_WARM_SHIFT));
    LOG_MSGID_I(battery_management, "COOL_THRESHOLD:[%d]\r\n",1,(int)pmu_get_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_COOL_ADDR, PMU_AUXADC_JEITA_VOLT_COOL_MASK,PMU_AUXADC_JEITA_VOLT_COOL_SHIFT));
    LOG_MSGID_I(battery_management, "COLD_THRESHOLD:[%d]\r\n",1,(int)pmu_get_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_COLD_ADDR, PMU_AUXADC_JEITA_VOLT_COLD_MASK,PMU_AUXADC_JEITA_VOLT_COLD_SHIFT));
    LOG_MSGID_I(battery_management, "===BJT Check==\r\n",0);
    LOG_MSGID_I(battery_management, "=FastCC safety timer enable==%d\r\n",1,(int)pmu_get_register_value_2byte_mt6388(PMU_RG_EN_SAFETY_TMR_CC_ADDR, PMU_RG_EN_SAFETY_TMR_CC_MASK,PMU_RG_EN_SAFETY_TMR_CC_SHIFT));
    LOG_MSGID_I(battery_management, "=FastCC safety timer selection==%d\r\n",1,(int)pmu_get_register_value_2byte_mt6388(PMU_RG_CC_TMR_SEL_ADDR, PMU_RG_CC_TMR_SEL_MASK, PMU_RG_CC_TMR_SEL_SHIFT));
    LOG_MSGID_I(battery_management, "=BJT bit[1'b0: (BJT charging)] ==%lx\r\n",1,pmu_get_register_value_2byte_mt6388(PMU_RG_DIS_BJT_ADDR, PMU_RG_DIS_BJT_MASK, PMU_RG_DIS_BJT_SHIFT));
    LOG_MSGID_I(battery_management, "check 5C[3'b100~3'b111: 5C] : %d\r\n",1,(int)pmu_get_register_value_2byte_mt6388(PMU_RG_ICC_MULTIPLE_ITH_ADDR,PMU_RG_ICC_MULTIPLE_ITH_MASK, PMU_RG_ICC_MULTIPLE_ITH_SHIFT));
    LOG_MSGID_I(battery_management, "bm_ctrl_info.bjt_isCharger : %d\r\n",1, bm_ctrl_info.bjt_isCharger);
    LOG_MSGID_I(battery_management, "CAPACITY : %d\r\n",1, (int)battery_get_linear_gauge_percent());
    LOG_MSGID_I(battery_management, "CAPACITY_LEVEL : %d\r\n",1, (int)battery_get_linear_gauge_percent_level());
    LOG_MSGID_I(battery_management, "isChargerExist : %d\r\n",1, (bm_ctrl_info.isChargerExist) ? true : false);
    LOG_MSGID_I(battery_management, "chargerType : %d\r\n",1, bm_ctrl_info.chargerType);
    LOG_MSGID_I(battery_management, "TEMPERATURE : %d\r\n", 1,battery_auxadc_voltage_to_tempature(pmu_auxadc_get_channel_value(PMU_AUX_CHR_THM)));
    LOG_MSGID_I(battery_management, "VOLTAGE : %d\r\n",1, (int)pmu_auxadc_get_channel_value(PMU_AUX_BATSNS));
    LOG_MSGID_I(battery_management, "PMIC_TEMPERATURE : %d\r\n",1, (int)battery_get_pmic_temp());
    LOG_MSGID_I(battery_management, "CHARGER_STATE: %d\r\n",1, (int)battery_get_charger_state());
    LOG_MSGID_I(battery_management, "====Basic value=====\r\n",0);
    LOG_MSGID_I(battery_management, "initial_bat:[%d]\r\n",1,bm_cust.initial_bat);
    LOG_MSGID_I(battery_management, "shutdown_bat:[%d]\r\n",1,bm_cust.shutdown_bat);
    LOG_MSGID_I(battery_management, "full_bat:[%d]\r\n",1,bm_cust.full_bat);
    LOG_MSGID_I(battery_management, "full_bat_offset:[%d]\r\n",1,bm_cust.full_bat_offset);
    LOG_MSGID_I(battery_management, "recharger_voltage:[%d]\r\n",1,bm_cust.recharger_voltage);
    LOG_MSGID_I(battery_management, "recharger_threshold:[%d]\r\n",1,bm_cust.recharger_threshold);
    for (i = 0; i < 9; i++) {
        LOG_MSGID_I(battery_management, "check point:[%d][%d]\r\n",2,(i+1),bm_cust.check_point[i]);
    }
}

