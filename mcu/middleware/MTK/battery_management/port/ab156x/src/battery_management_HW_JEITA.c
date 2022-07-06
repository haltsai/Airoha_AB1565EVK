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

/*Common*/
#include "battery_management_core.h"
#include "hal_pmu.h"
extern battery_managerment_control_info_t bm_ctrl_info;
extern battery_basic_data bm_cust;
extern TimerHandle_t xbm_jeita_timer;
void battery_jetia_create_check_timer(void) {
#ifndef BATTERY_NTC_LESS
     if (xTimerStartFromISR(xbm_jeita_timer, 0) != pdPASS) {
         LOG_MSGID_I(battery_management, "xbm_jeita_Timer xTimerStart fail\n", 0);
     }
#endif
}
#if defined(AB1565)
#include "battery_management_sw_ntc.h"
#include "hal_pmu_cali_2565.h"
#include "hal_pmu_charger_2565.h"
uint8_t executing_status=sw_ntc_charger_normal;
extern uint8_t battery_sw_ntc_feature;

void battery_jetia_timer_callback(TimerHandle_t pxTimer) {
#ifndef BATTERY_NTC_LESS
    uint8_t jeita_status ;
    int get_temp = 0;
    uint8_t temp_ste_n=0;
    battery_management_sw_ntc_retrive(&get_temp, &jeita_status, &temp_ste_n);
    if (jeita_status == sw_ntc_charger_normal) {
        if (executing_status != sw_ntc_charger_normal) {
            ntc_change_state(JEITA_WARM_TO_NORMAL);
            ntc_change_state(JEITA_ENABLE_CHARGER);
            executing_status = sw_ntc_charger_normal;
        }
    } else if (jeita_status == sw_ntc_charger_hot) {
        if (executing_status != sw_ntc_charger_hot) {
            LOG_MSGID_I(battery_management, "HW JEITA Hot Setting, Disable Charger\r\n", 0);
            ntc_change_state(JEITA_DISABLE_CHARGER);
            executing_status = sw_ntc_charger_hot;
        }
#ifndef DISABLE_WARM_COOL
    } else if (jeita_status == sw_ntc_charger_warm) {
        if (executing_status != sw_ntc_charger_warm) {
            ntc_change_state(JEITA_NORMAL_TO_WARM);
            ntc_change_state(JEITA_ENABLE_CHARGER);
            executing_status = sw_ntc_charger_warm;
        }
    } else if (jeita_status == sw_ntc_charger_cool) {
        if (executing_status != sw_ntc_charger_cool) {
            ntc_change_state(JEITA_NORMAL_TO_COOL);
            ntc_change_state(JEITA_ENABLE_CHARGER);
            executing_status = sw_ntc_charger_cool;
        }
#endif /* DISABLE_WARM_COOL */
    } else if (jeita_status == sw_ntc_charger_cold) {
        if (executing_status != sw_ntc_charger_cold) {
            LOG_MSGID_I(battery_management, "HW JEITA Cold Setting, Disable Charger\r\n", 0);
            ntc_change_state(JEITA_DISABLE_CHARGER);
            executing_status = sw_ntc_charger_cold;
        }
    }  else if ((jeita_status == sw_ntc_nocharger_power_off_low) ||(jeita_status == sw_ntc_nocharger_power_off_high)) {
        if (battery_sw_ntc_feature == battery_sw_ntc_sucess) {
        LOG_MSGID_I(battery_management, "Abnormal temperature! System will be enter rtc mode\r\n", 0);
        hal_rtc_enter_rtc_mode();
        }
    }else {
        LOG_MSGID_I(battery_management, "battery_jetia_timer_callback error\r\n", 0);
    }
    bm_ctrl_info.jeita_state = executing_status;
#else
    jeita_status = sw_ntc_charger_normal;
    LOG_MSGID_I(battery_management, "jeita_status error\r\n", 0);
#endif /* BATTERY_NTC_LESS */
}

#endif

#if defined(AB1568)
#include "hal_pmu_charger_2568.h"
uint8_t executing_status=HW_JEITA_NORMAL_STAGE;

void battery_jetia_timer_callback(TimerHandle_t pxTimer) {
#ifndef BATTERY_NTC_LESS
    uint8_t jeita_status ;
    jeita_status= pmu_get_hw_jeita_status();
    if (jeita_status == HW_JEITA_NORMAL_STAGE) {
        if (executing_status != HW_JEITA_NORMAL_STAGE) {
            pmu_jeita_state_setting(HW_JEITA_NORMAL_STAGE,RG_ICC_JC_100,RG_VCV_VOLTAGE_4P20V);/*Normal State will not be change*/
            executing_status = HW_JEITA_NORMAL_STAGE;
        }
    } else if (jeita_status == HW_JEITA_HOT_STAGE) {
        if (executing_status != HW_JEITA_HOT_STAGE) {
            LOG_MSGID_I(battery_management, "HW JEITA Hot Setting, Disable Charger\r\n", 0);
            battery_set_enable_charger(BATTERY_OPERATE_OFF);
            executing_status = HW_JEITA_HOT_STAGE;
        }
#ifndef DISABLE_WARM_COOL
    } else if (jeita_status == HW_JEITA_WARM_STAGE) {
        if (executing_status != HW_JEITA_WARM_STAGE) {
            pmu_jeita_state_setting(HW_JEITA_WARM_STAGE,bm_cust.warm_cc,battery_get_full_battery_index(bm_cust.warm_cv));
            executing_status = HW_JEITA_WARM_STAGE;
        }
    } else if (jeita_status == HW_JEITA_COOL_STAGE) {
        if (executing_status != HW_JEITA_COOL_STAGE) {
            pmu_jeita_state_setting(HW_JEITA_COOL_STAGE,bm_cust.cool_cc,battery_get_full_battery_index(bm_cust.cool_cv));
            executing_status = HW_JEITA_COOL_STAGE;
        }
#endif
    } else if (jeita_status == HW_JEITA_COLD_STAGE) {
        if (executing_status != 0) {
            LOG_MSGID_I(battery_management, "HW JEITA Cold Setting, Disable Charger\r\n", 0);
            battery_set_enable_charger(BATTERY_OPERATE_OFF);
            executing_status = HW_JEITA_COLD_STAGE;
        }
    } else {
        LOG_MSGID_I(battery_management, "Error assert\r\n", 0);
    }
    bm_ctrl_info.jeita_state = executing_status;
#else
    bm_ctrl_info.jeita_state = HW_JEITA_NORMAL_STAGE;
#endif
}

void battery_core_pmu_jeita_hot_callback(void) {
    uint32_t hwJeitaValue;
    hwJeitaValue = pmu_auxadc_get_channel_value(PMU_AUX_HW_JEITA);
    LOG_MSGID_I(battery_management, "battery hw jeita auxadc:%d\r\n",1, hwJeitaValue);
    battery_jetia_create_check_timer();
}

void battery_core_pmu_jeita_warm_callback(void) {
    uint32_t hwJeitaValue;
    hwJeitaValue = pmu_auxadc_get_channel_value(PMU_AUX_HW_JEITA);
    LOG_MSGID_I(battery_management, "battery hw jeita auxadc:%d\r\n",1, hwJeitaValue);
    battery_jetia_create_check_timer();
}

void battery_core_pmu_jeita_cool_callback(void) {
    uint32_t hwJeitaValue;
    hwJeitaValue = pmu_auxadc_get_channel_value(PMU_AUX_HW_JEITA);
    LOG_MSGID_I(battery_management, "battery hw jeita auxadc:%d\r\n",1, hwJeitaValue);
    battery_jetia_create_check_timer();
}

void battery_core_pmu_jeita_cold_callback(void) {
    uint32_t hwJeitaValue;
    hwJeitaValue = pmu_auxadc_get_channel_value(PMU_AUX_HW_JEITA);
    LOG_MSGID_I(battery_management, "battery hw jeita auxadc:%d\r\n",1, hwJeitaValue);
    battery_jetia_create_check_timer();
}
void battery_core_hw_jeita_set_threshold(uint32_t auxadcVolt, uint8_t JeitaThreshold) {
    uint32_t jeitaVoltage = 0;
    jeitaVoltage = (uint32_t) ((auxadcVolt * 4096) / 1800);
    switch (JeitaThreshold)
    {
        case HW_JEITA_HOT_STAGE:
            LOG_MSGID_I(battery_management, "Set JEITA HOT:%d\r\n",1, jeitaVoltage);
            pmu_set_jeita_voltage(jeitaVoltage,HW_JEITA_HOT_STAGE);
            pmu_register_callback(RG_INT_JEITA_HOT, battery_core_pmu_jeita_hot_callback, NULL);
            break;

        case HW_JEITA_WARM_STAGE:
            LOG_MSGID_I(battery_management, "Set JEITA WARM:%d\r\n",1, jeitaVoltage);
            pmu_set_jeita_voltage(jeitaVoltage,HW_JEITA_WARM_STAGE);
            pmu_register_callback(RG_INT_JEITA_WARM, battery_core_pmu_jeita_warm_callback, NULL);
            break;

        case HW_JEITA_COOL_STAGE:
            LOG_MSGID_I(battery_management, "Set JEITA COOL:%d\r\n",1, jeitaVoltage);
            pmu_set_jeita_voltage(jeitaVoltage,HW_JEITA_COOL_STAGE);
            pmu_register_callback(RG_INT_JEITA_COOL, battery_core_pmu_jeita_cool_callback, NULL);
            break;

        case HW_JEITA_COLD_STAGE:
            LOG_MSGID_I(battery_management, "Set JEITA COLD:%d\r\n",1, jeitaVoltage);
            pmu_set_jeita_voltage(jeitaVoltage,HW_JEITA_COLD_STAGE);
            pmu_register_callback(RG_INT_JEITA_COLD, battery_core_pmu_jeita_cold_callback, NULL);
            break;
    }
}

void battery_core_hw_jeita_init_threshold(void) {
    battery_core_hw_jeita_set_threshold(bm_cust.jeita.hot, HW_JEITA_HOT_STAGE);
    battery_core_hw_jeita_set_threshold(bm_cust.jeita.warm, HW_JEITA_WARM_STAGE);
    battery_core_hw_jeita_set_threshold(bm_cust.jeita.cool, HW_JEITA_COOL_STAGE);
    battery_core_hw_jeita_set_threshold(bm_cust.jeita.cold, HW_JEITA_COLD_STAGE);
}

void battery_core_hw_jeita_init(void) {
    LOG_MSGID_I(battery_management, "Enable HW JEITA function\r\n", 0);
    pmu_hw_jeita_init();
}

#endif
