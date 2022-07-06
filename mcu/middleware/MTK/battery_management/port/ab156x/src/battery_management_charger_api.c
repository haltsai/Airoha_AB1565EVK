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
#ifdef AB1568
#include "hal_pmu_internal_2568.h"
#endif
#include "hal_pmu.h"
#ifdef AB1565
#include "hal_pmu_internal_2565.h"
#endif

extern battery_managerment_control_info_t bm_ctrl_info;
extern uint8_t battery_VA18_flag ;
void battery_set_extend_charger_time(uint8_t timeMins) {
    pmu_set_extend_charger_time(timeMins);
}
bool battery_set_enable_charger(bool isEnableCharging) {
    if(isEnableCharging==BATTERY_OPERATE_ON){
        LOG_W(MPLOG,"[Charger] Vbus on\r\n");
    }else{
        LOG_W(MPLOG,"[Charger] Vbus off\r\n");
    }
#ifdef BATTERY_DISABLE_CHARGER
    return pmu_enable_charger(BATTERY_OPERATE_OFF);
#else
    return pmu_enable_charger(isEnableCharging);
#endif
}
void battery_check_charger_power(void){
    if((pmu_set_rechg_voltage(PMU_LDO_VLDO33)!=PMU_ON ) || (pmu_set_rechg_voltage(PMU_LDO_VA18)!=PMU_ON )){
        pmu_enable_power(PMU_LDO_VA18, PMU_ON);
        pmu_enable_power(PMU_LDO_VLDO33, PMU_ON);
    }
    battery_reguest_va18(BATTERY_OPERATE_ON);
}
/*Only when bootup_from vbus*/
void battery_clear_charger_irq(void){
    pmu_clear_interrupt(RG_INT_CHRDET);
    pmu_clear_interrupt(RG_INT_ChgStatInt);
}
void battery_reguest_va18(bool oper){
    uint32_t mask_pri;
    hal_nvic_save_and_set_interrupt_mask(&mask_pri);
    if(oper){
        pmu_lock_va18(PMU_ON);
        battery_VA18_flag=1;
    }else{
        pmu_lock_va18(PMU_OFF);
        battery_VA18_flag=0;
    }
    hal_nvic_restore_interrupt_mask(mask_pri);

}
