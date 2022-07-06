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

extern TimerHandle_t xbm_jeita_timer;
uint8_t executing_status=HW_JEITA_NORMAL_STAGE;
extern battery_managerment_control_info_t bm_ctrl_info;
extern battery_basic_data bm_cust;
void battery_core_hw_jeita_hot_zone(void) {
    LOG_MSGID_I(battery_management, "Enter HW JEITA hot Setting\r\n", 0);
    battery_core_hw_jeita_hot_setting();
}

void battery_core_hw_jeita_warm_zone(void) {
    LOG_MSGID_I(battery_management, "Enter HW JEITA warm Setting\r\n", 0);
    battery_core_hw_jeita_warm_setting();
}

void battery_core_hw_jeita_cool_zone(void) {
    LOG_MSGID_I(battery_management, "Cool Zone\r\n", 0);
    battery_core_hw_jeita_cool_setting();
}

void battery_core_hw_jeita_cold_zone(void) {
    LOG_MSGID_I(battery_management, "Enter HW JEITA cold Setting\r\n", 0);
    battery_core_hw_jeita_cold_setting();
}

void battery_core_hw_jeita_hot_setting(void) {    //stop charging
    LOG_MSGID_I(battery_management, "HW JEITA Hot Setting, Disable Charger\r\n", 0);
    battery_set_enable_charger(BATTERY_OPERATE_OFF);
}

void battery_core_hw_jeita_warm_setting(void) {
#ifndef DISABLE_WARM_COOL
    battery_set_jeita_diswarmcool(0);
    battery_set_jeita_warm_voltage(battery_get_full_battery_index(bm_cust.warm_cv));
    hal_gpt_delay_us(50);
    battery_set_jeita_warm_icc_level(bm_cust.warm_cc);
    LOG_MSGID_I(battery_management, "HW JEITA Warm Setting\r\n", 0);
#endif
}

void battery_core_hw_jeita_cool_setting(void) {
#ifndef DISABLE_WARM_COOL
    battery_set_jeita_diswarmcool(0);
    battery_set_jeita_cool_icc_level(bm_cust.cool_cc);
    hal_gpt_delay_us(50);
    battery_set_jeita_cool_voltage(battery_get_full_battery_index(bm_cust.cool_cv));
    LOG_MSGID_I(battery_management, "HW JEITA Cool Setting\r\n",0);
#endif
}

void battery_core_hw_jeita_cold_setting(void) {    //stop charging
    LOG_MSGID_I(battery_management, "HW JEITA Cold Setting, Disable Charger\r\n", 0);
    battery_set_enable_charger(BATTERY_OPERATE_OFF);
}

void battery_jetia_create_check_timer(void) {
#ifndef MTK_BATTERY_MANAGEMENT_NTC_LESS
        if (xTimerStartFromISR(xbm_jeita_timer, 0) != pdPASS) {
            LOG_MSGID_I(battery_management, "xbm_jeita_Timer xTimerStart fail\n", 0);
        }
#endif
}

static void battery_core_pmu_jeita_hot_callback(void) {
    uint32_t hwJeitaValue=0;
    hwJeitaValue = pmu_auxadc_get_channel_value(PMU_AUX_HW_JEITA);
    LOG_MSGID_I(battery_management, "Callback Hot auxadc:%d\r\n",1, hwJeitaValue);
    battery_core_hw_jeita_hot_zone();
    battery_jetia_create_check_timer();

}

static void battery_core_pmu_jeita_warm_callback(void) {
    uint32_t hwJeitaValue=0;
    hwJeitaValue = pmu_auxadc_get_channel_value(PMU_AUX_HW_JEITA);
    LOG_MSGID_I(battery_management, "Callback Warm auxadc:%d\r\n",1, hwJeitaValue);
    battery_jetia_create_check_timer();
    battery_core_hw_jeita_warm_zone();
}

static void battery_core_pmu_jeita_cool_callback(void) {
    uint32_t hwJeitaValue=0;
    hwJeitaValue = pmu_auxadc_get_channel_value(PMU_AUX_HW_JEITA);
    LOG_MSGID_I(battery_management, "Callback Cool auxadc:%d\r\n",1, hwJeitaValue);
    battery_jetia_create_check_timer();
    battery_core_hw_jeita_cool_zone();
}

static void battery_core_pmu_jeita_cold_callback(void) {
    uint32_t hwJeitaValue=0;
    hwJeitaValue = pmu_auxadc_get_channel_value(PMU_AUX_HW_JEITA);
    LOG_MSGID_I(battery_management, "Callback Cold auxadc:%d\r\n",1, hwJeitaValue);
    battery_jetia_create_check_timer();
    battery_core_hw_jeita_cold_zone();
}

void battery_jetia_timer_callback(TimerHandle_t pxTimer) {
    uint8_t jeita_status;
    jeita_status = battery_get_hw_jeita_status();
    if (jeita_status == HW_JEITA_NORMAL_STAGE) {
        if (executing_status != HW_JEITA_NORMAL_STAGE) {
            battery_set_enable_charger(1);
            battery_set_ibat_tune_trim(RG_ICC_JC_100);
            executing_status = HW_JEITA_NORMAL_STAGE;
        }
    } else if (jeita_status == HW_JEITA_HOT_STAGE) {
        if (executing_status != HW_JEITA_HOT_STAGE) {
            battery_core_hw_jeita_hot_setting();
            executing_status = HW_JEITA_HOT_STAGE;
        }
    } else if (jeita_status == HW_JEITA_COLD_STAGE) {
        if (executing_status != 0) {
            battery_core_hw_jeita_cold_setting();
            executing_status = HW_JEITA_COLD_STAGE;
        }
    }
#ifndef DISABLE_WARM_COOL
    else if (jeita_status == HW_JEITA_WARM_STAGE) {
        if (executing_status != HW_JEITA_WARM_STAGE) {
            battery_core_hw_jeita_warm_setting();
            executing_status = HW_JEITA_WARM_STAGE;
        }
    } else if (jeita_status == HW_JEITA_COOL_STAGE) {
        if (executing_status != HW_JEITA_COOL_STAGE) {
            battery_core_hw_jeita_cool_setting();
            executing_status = HW_JEITA_COOL_STAGE;
        }
    }
#else
    if (executing_status != HW_JEITA_NORMAL_STAGE) {
        battery_set_enable_charger(1);
        battery_set_ibat_tune_trim(RG_ICC_JC_100);
        executing_status = HW_JEITA_NORMAL_STAGE;
    }
#endif
    else {
#ifndef DISABLE_WARM_COOL
        LOG_MSGID_I(battery_management, "Error assert\r\n", 0);
#endif
    }
    bm_ctrl_info.jeita_state = executing_status;
}

void battery_core_hw_jeita_set_threshold(float auxadcVolt, uint8_t JeitaThreshold) {
    uint32_t jeitaVoltage = 0;
    jeitaVoltage = (uint32_t) ((auxadcVolt * 4096) / 1800);
    bm_ctrl_info.jeita_Voltage = jeitaVoltage;
    switch (JeitaThreshold)
    {
        case HW_JEITA_HOT_STAGE:
            LOG_MSGID_I(battery_management, "Set JEITA HOT:%d\r\n",1, jeitaVoltage);
            pmu_set_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_HOT_ADDR, PMU_AUXADC_JEITA_VOLT_HOT_MASK,PMU_AUXADC_JEITA_VOLT_HOT_SHIFT, jeitaVoltage);
            pmu_register_callback(RG_INT_JEITA_HOT, battery_core_pmu_jeita_hot_callback, NULL);
            break;

        case HW_JEITA_WARM_STAGE:
            LOG_MSGID_I(battery_management, "Set JEITA WARM:%d\r\n",1, jeitaVoltage);
            pmu_set_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_WARM_ADDR, PMU_AUXADC_JEITA_VOLT_WARM_MASK,PMU_AUXADC_JEITA_VOLT_WARM_SHIFT, jeitaVoltage);
            pmu_register_callback(RG_INT_JEITA_WARM, battery_core_pmu_jeita_warm_callback, NULL);
            break;

        case HW_JEITA_COOL_STAGE:
            LOG_MSGID_I(battery_management, "Set JEITA COOL:%d\r\n",1, jeitaVoltage);
            pmu_set_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_COOL_ADDR, PMU_AUXADC_JEITA_VOLT_COOL_MASK,PMU_AUXADC_JEITA_VOLT_COOL_SHIFT, jeitaVoltage);
            pmu_register_callback(RG_INT_JEITA_COOL, battery_core_pmu_jeita_cool_callback, NULL);
            break;

        case HW_JEITA_COLD_STAGE:
            LOG_MSGID_I(battery_management, "Set JEITA COLD:%d\r\n",1, jeitaVoltage);
            pmu_set_register_value_2byte_mt6388(PMU_AUXADC_JEITA_VOLT_COLD_ADDR, PMU_AUXADC_JEITA_VOLT_COLD_MASK,PMU_AUXADC_JEITA_VOLT_COLD_SHIFT, jeitaVoltage);
            pmu_register_callback(RG_INT_JEITA_COLD, battery_core_pmu_jeita_cold_callback, NULL);
            break;

    }
}

void battery_core_hw_jeita_enable_warmcool(void) {
    LOG_MSGID_I(battery_management, "HW JEITA Warm Cool Enable\r\n", 0);
    battery_set_icc_step_timing(0x2); //ICC soft start step time.default 16ms
#ifdef BATTERY_ENABLE_DIGITAL_THERMAL
    LOG_MSGID_I(battery_management, "Enable Digital Thermal Function\r\n", 0);
    battery_enable_digital_thermal(1);
#endif
}

void battery_core_hw_jeita_init_threshold(void) {
    battery_core_hw_jeita_set_threshold(bm_cust.jeita.hot, HW_JEITA_HOT_STAGE);
#ifndef DISABLE_WARM_COOL
    battery_core_hw_jeita_set_threshold(bm_cust.jeita.warm, HW_JEITA_WARM_STAGE);
    battery_core_hw_jeita_set_threshold(bm_cust.jeita.cool, HW_JEITA_COOL_STAGE);
#endif
    battery_core_hw_jeita_set_threshold(bm_cust.jeita.cold, HW_JEITA_COLD_STAGE);
}

void battery_core_hw_jeita_init(void) {
    LOG_MSGID_I(battery_management, "Enable HW JEITA function\r\n", 0);
    battery_core_hw_jeita_enable_warmcool();
    battery_core_hw_jeita_init_threshold();
    battery_set_jeita_cool_icc_level(bm_cust.cool_cc);
    battery_set_jeita_warm_icc_level(bm_cust.warm_cc);
}

