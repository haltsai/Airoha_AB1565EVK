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
 
extern battery_managerment_control_info_t bm_ctrl_info;


void battery_bjt_check_ovp(void) {
    if (pmu_auxadc_get_channel_value(PMU_AUX_VBUS) > MAX_BATTERY_VOLTAGE_OVERLAY) {
        battery_bjt_disable_charging();
    }
}

void battery_bjt_disable_charging(void) {
    battery_bjt_enable_charging(1);
    battery_bjt_set_bias_icc(4);
    bm_ctrl_info.bjt_isCharger = BATTERY_OPERATE_OFF;
}

/*
 * Enable BJT charger need close bjt and set safety timer ,and re-enable
 * */
void battery_enable_bjt(void) {
    LOG_MSGID_I(battery_management, "BJT Charging Enable\r\n", 0);
    battery_enable_fastcc_safety_timer(0);
    battery_select_cc_safety_timer_domain(BJT_SAFETY_TIME); //set 9h ; Check with sa
    battery_enable_fastcc_safety_timer(1); // start timer
    battery_bjt_charging_setting();
}

void battery_bjt_charging_setting(void) {
    if (battery_get_charger_state() == CHARGER_STATE_FASTCC) {
        battery_bjt_enable_charging(0); // enable BJT charging bit
        battery_set_soft_time_step(TSTEP_ICC_MULTIPLE_TIME_16uS);
        battery_set_icl_curent_level(ICL_ITH_400mA);
        battery_bjt_set_icc_ith(RG_TSTEP_ICC_BJT_CURRENT_LEVEL_1p5A);
        battery_bjt_set_bias_icc(5);
        battery_enable_external_bjt_charging(1);
        hal_gpt_delay_ms(4);
        if (battery_check_bjt_mode() == 1) {
            bm_ctrl_info.bjt_isCharger = BATTERY_OPERATE_ON;
            return;
        } else { //enable fail , revert bjt setting
            battery_bjt_enable_charging(1);
            battery_bjt_set_bias_icc(4);
            goto BJT_FAILED_TO_ENTER_BJT_MODE;
        }
    } else {
        LOG_MSGID_I(battery_management, "BJT State Error, Not Start\r\n", 0);
        goto BJT_FAILED_TO_ENTER_BJT_MODE;
    }

    BJT_FAILED_TO_ENTER_BJT_MODE:
    battery_bjt_enable_charging(1); // disable BJT charging bit
    return;
}
