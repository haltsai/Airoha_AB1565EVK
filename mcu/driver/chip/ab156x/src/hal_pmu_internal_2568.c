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
#include "hal_pmu_auxadc_2568.h"
#include "hal_pmu_charger_2568.h"
#include "hal_pmu_internal_2568.h"

#include "hal_i2c_master.h"
#include "hal_nvic_internal.h"
#include "hal_sleep_manager_platform.h"
#include "hal_sleep_manager_internal.h"
#include "hal_sleep_manager.h"
#include "assert.h"
#include "syslog.h"

#ifdef  HAL_CAPTOUCH_MODULE_ENABLED
#ifdef AB1568
#include "hal_captouch_internal.h"
#endif
#endif
uint8_t ingore_flag = 1;
typedef long BaseType_t;
extern BaseType_t xTaskResumeAll( void );
extern void vTaskSuspendAll( void );
/*
 * Basic parameters */
int old_index=0;                                                        /* DATA : restore old vcore voltage index */
int pmic_irq0=-1,pmic_irq1=-1,pmic_irq2=-1,pmic_irq3=-1;                /* DATA : restore irq status */
int first_boot_up =0;                                                   /* FLAG : PK check is turned on for the first time */
int pk_next =PMU_PK_PRESS;                                              /* DATA : power key default value*/
int pmu_switch_case = 0;                                                /* DATA : pmu lock vcore case */

uint8_t pmu_basic_index=0;                                              /* DATA : project init basic vocre index */
uint8_t pmu_lock_status=0;                                              /* DATA : lock status*/
uint8_t va18_flag=0;                                                    /* FLAG : When VA18 doesn't want to be disable */
uint8_t vaud18_flag=0;                                                  /* FLAG : When VAUD18 doesn't want to be disable */
uint8_t pmu_init_flag=0;                                                /* FLAG : check init setting is done */
uint8_t event_con0=0,event_con1=0,event_con2=0,event_con3=0;            /* DATA : restore irq times */
uint8_t pmu_charger_status;                                             /* FLAG : for vbus exist */

uint32_t pmu_register_interrupt ;                                       /* DATA : Record irq index 0~31 */
uint32_t pmu_register_interrupt_2 ;                                     /* DATA : Record irq index >31 */
uint32_t pmu_irq_enable_com0 =0;                                        /* DATA : restore irq enable status,con0 */
uint32_t pmu_irq_enable_com1 =0;                                        /* DATA : restore irq enable status,con1 */
uint32_t pmu_irq_enable_com2 =0;                                        /* DATA : restore irq enable status,con2 */
uint32_t pmu_irq_enable_com3 =0;                                        /* DATA : restore irq enable status,con3 */

uint32_t pmu_va18_state =0;                                             /* flag : VA18 implement state*/
uint32_t pmu_va18_count =0;                                             /* flag : VA18 status*/
uint32_t pmu_va18_rd=0;                                                 /* flag : VA18 status*/
uint32_t va18_count=0;                                                  /* DATA : VA18 check buck status times*/

pmu_function_t pmu_function_table[PMU_INT_MAX];                         /* DATA : restore callback function */
volatile int pmu_i2c_init_sta = 0;                                      /* FLAG : init setting flag */
static volatile unsigned char Vcore_Resource_Ctrl[7];                            /* resource control : use in pmu_lock_vcore for resource control */
extern auxadc_efuse_data_stru gAuxadcEfuseInfo;                         /* DATA : get auxadc data struct*/
#define SRAM_RF_DELSEL            *((volatile uint32_t*) (0xA2010500))  /* RG   : Setting for sram*/
#define SRAM_HDE_DELSEL           *((volatile uint32_t*) (0xA2010504))  /* RG   : Setting for sram*/
#define SRAM_UHDE_DELSEL          *((volatile uint32_t*) (0xA2010508))  /* RG   : Setting for sram*/
uint8_t pmu_dynamic_debug = 0;
/*==========[Basic function]==========*/
pmu_power_vcore_voltage_t pmu_lock_vcore_ab2568(pmu_power_stage_t mode, pmu_power_vcore_voltage_t vol, pmu_lock_parameter_t lock) {
    if(pmu_dynamic_debug){
       log_hal_msgid_info("[PMU_lock] lock vol = %d, lock = %d[0:lock 1:unlock] \r\n",2, vol, lock);
    }
    int i = 0;
    int temp = 0;
    int vol_index = 0;

    uint32_t mask_pri;
    if (vol >= PMIC_VCORE_FAIL_V) {
        return PMIC_VCORE_FAIL_V;
    }
    if (mode == PMU_SLEEP) {
    } else {
        hal_nvic_save_and_set_interrupt_mask_special(&mask_pri);
        if (lock == PMU_LOCK) {
            Vcore_Resource_Ctrl[vol]++;
        } else {
            if (Vcore_Resource_Ctrl[vol] != 0) {
                Vcore_Resource_Ctrl[vol]--;
            }
        }
        /*Find Highest Vcore Voltage*/
        for (vol_index = PMIC_VCORE_0P9_V; vol_index >= PMIC_VCORE_0P5_V; vol_index--) {
            if (Vcore_Resource_Ctrl[vol_index] != 0) {
                break;
            }
        }
        for (i = PMIC_VCORE_0P5_V; i <= PMIC_VCORE_0P9_V; i++) {
            temp += Vcore_Resource_Ctrl[i];
        }
        if(temp > 25){
            log_hal_msgid_info("ERROR!!!! PMU Lock /unlock isn't match more ten times\r\n",0);
            assert(0);
        }
        pmu_lock_status = temp;
        /*if not module lock ,return default setting*/
        if (temp == 0) {
            pmu_switch_case = 3;
            pmu_select_vcore_voltage_ab2568(PMU_DVS, pmu_basic_index);
            hal_nvic_restore_interrupt_mask_special(mask_pri);
        } else {
            if (old_index < vol_index) {
                pmu_switch_case = 2;
                pmu_select_vcore_voltage_ab2568(PMU_DVS, vol_index);
                pmu_ddie_sram_setting(vol_index);
            } else if(old_index > vol_index) {
                pmu_switch_case = 1;
               pmu_ddie_sram_setting(vol_index);
               pmu_select_vcore_voltage_ab2568(PMU_DVS, vol_index);
            } else {
                pmu_switch_case = 0;
            }
            old_index = vol_index;
            hal_nvic_restore_interrupt_mask_special(mask_pri);
        }
    }
    if(pmu_dynamic_debug){
       log_hal_msgid_info("[PMU_lock] vcore lock: %d lock state : %d pmu_switch_case = %d\r\n",3,pmu_get_vcore_voltage_ab2568(),temp,pmu_switch_case);
    }
    return vol_index;
}

pmu_status_t pmu_register_callback(pmu_interrupt_index_t pmu_int_ch, pmu_callback_t callback, void *user_data)
{
    pmu_status_t status = PMU_STATUS_ERROR;
    if(pmu_int_ch >= PMU_INT_MAX || callback == NULL)
    {
        return PMU_STATUS_INVALID_PARAMETER;
    }
    pmu_function_table[pmu_int_ch].init_status = PMU_INIT;
    pmu_function_table[pmu_int_ch].pmu_callback= callback;
    pmu_function_table[pmu_int_ch].user_data = user_data;
    pmu_function_table[pmu_int_ch].isMask= false;
    if(pmu_int_ch>31){
        pmu_register_interrupt_2 |=(1<<(pmu_int_ch-32)) ;
    }else{
        pmu_register_interrupt |=(1<<pmu_int_ch) ;
    }
    pmu_control_enable_interrupt(pmu_int_ch, 1);
    pmu_control_mask_interrupt(pmu_int_ch, 0);

    status = PMU_STATUS_SUCCESS;
    return status;
}

pmu_status_t pmu_deregister_callback(pmu_interrupt_index_t pmu_int_ch)
{
    pmu_status_t status = PMU_STATUS_ERROR;

    if(pmu_int_ch >= PMU_INT_MAX )
    {
        return PMU_STATUS_INVALID_PARAMETER;
    }
    pmu_function_table[pmu_int_ch].init_status = PMU_NOT_INIT;
    pmu_function_table[pmu_int_ch].pmu_callback= NULL;
    pmu_function_table[pmu_int_ch].user_data = NULL;
    pmu_function_table[pmu_int_ch].isMask= true;

    pmu_control_enable_interrupt(pmu_int_ch, 0);
    pmu_control_mask_interrupt(pmu_int_ch, 1);
    status = PMU_STATUS_SUCCESS;

    return status;
}

void pmu_power_off_sequence_2568(pmu_power_stage_t stage) {
    switch(stage) {
        case PMU_PWROFF:
            log_hal_msgid_info("PMU power off ", 0);
            hal_gpt_delay_ms(1);
            pmu_set_register_value_ab2568(PMU_TPO_CON0,PMU_RG_PWROFF_MODE_MASK,PMU_RG_PWROFF_MODE_SHIFT,1);//Power OFF
            break;
        case PMU_RTC:
            log_hal_msgid_info("PMU Enter RTC mode ", 0);
            hal_gpt_delay_ms(1);
            pmu_set_register_value_ab2568(PMU_VREF_Interface_ANA_CON2, PMU_RG_POFFSTS_CLR_MASK, PMU_RG_POFFSTS_CLR_SHIFT, 0x4400);
            pmu_set_register_value_ab2568(PMU_PSTSCTL2, PMU_RG_POFFSTS_EN_MASK, PMU_RG_POFFSTS_EN_SHIFT, 0x3fff);
            pmu_set_register_value_ab2568(PMU_TPO_CON0,PMU_RG_PWROFF_MODE_MASK,PMU_RG_PWROFF_MODE_SHIFT,0);//Power OFF
            pmu_set_register_value_ab2568(PMU_PWRHOLD,PMU_RG_PWRHOLD_MASK,PMU_RG_PWRHOLD_SHIFT,0);//Power hold release
        break;
        case PMU_NORMAL:
            break;
        case PMU_SLEEP:
            break;
    }
}

/*==========[BUCK/LDO control]==========*/
void pmu_enable_power_2568(pmu_power_domain_t pmu_pdm, pmu_power_operate_t operate) {
    //log_hal_msgid_info("PMU enable power [%d][%d] ", 2,pmu_pdm,operate);
    uint32_t mask_pri;
    uint8_t temp=0;
    switch (pmu_pdm)
    {
        case PMU_BUCK_VCORE:
            if (pmu_get_register_value_ab2568(PMU_BUCK_VCORE_OP_MODE, PMU_RG_BUCK_VCORE_ON_MODE_MASK, PMU_RG_BUCK_VCORE_ON_MODE_SHIFT)) {
                pmu_set_register_value_ab2568(PMU_BUCK_VCORE_EN_CFG, PMU_RG_BUCK_ACT_VCORE_EN_MASK, PMU_RG_BUCK_ACT_VCORE_EN_SHIFT, operate);
                pmu_set_register_value_ab2568(PMU_BUCK_VCORE_EN_CFG, PMU_RG_BUCK_SLP_VCORE_EN_MASK, PMU_RG_BUCK_SLP_VCORE_EN_SHIFT, operate);
            } else {
                pmu_set_register_value_ab2568(PMU_BUCK_VCORE_CON0, PMU_RG_BUCK_VCORE_EN_MASK, PMU_RG_BUCK_VCORE_EN_SHIFT, operate);

            }
            break;
        case PMU_BUCK_VIO18:
            if (pmu_get_register_value_ab2568(PMU_BUCK_VIO18_OP_MODE, PMU_RG_BUCK_VIO18_ON_MODE_MASK, PMU_RG_BUCK_VIO18_ON_MODE_SHIFT)) {
                pmu_set_register_value_ab2568(PMU_BUCK_VIO18_EN_CFG, PMU_RG_BUCK_ACT_VIO18_EN_MASK, PMU_RG_BUCK_ACT_VIO18_EN_SHIFT, operate);
                pmu_set_register_value_ab2568(PMU_BUCK_VIO18_EN_CFG, PMU_RG_BUCK_SLP_VIO18_EN_MASK, PMU_RG_BUCK_SLP_VIO18_EN_SHIFT, operate);
            } else {
                pmu_set_register_value_ab2568(PMU_BUCK_VIO18_CON0, PMU_RG_BUCK_VIO18_EN_MASK, PMU_RG_BUCK_VIO18_EN_SHIFT, operate);

            }
            break;
        case PMU_BUCK_VRF:
            if (pmu_get_register_value_ab2568(PMU_BUCK_VRF_OP_MODE, PMU_RG_BUCK_VRF_ON_MODE_MASK, PMU_RG_BUCK_VRF_ON_MODE_SHIFT)) {
                pmu_set_register_value_ab2568(PMU_BUCK_VRF_EN_CFG, PMU_RG_BUCK_ACT_VRF_EN_MASK, PMU_RG_BUCK_ACT_VRF_EN_SHIFT, operate);
                pmu_set_register_value_ab2568(PMU_BUCK_VRF_EN_CFG, PMU_RG_BUCK_SLP_VRF_EN_MASK, PMU_RG_BUCK_SLP_VRF_EN_SHIFT, operate);
            } else {
                pmu_set_register_value_ab2568(PMU_BUCK_VRF_CON0, PMU_RG_BUCK_VRF_EN_MASK, PMU_RG_BUCK_VRF_EN_SHIFT, operate);

            }
            break;
        case PMU_BUCK_VAUD18:
            if (vaud18_flag && operate == PMU_OFF) {
                log_hal_msgid_info("Warning : VAUD18 in used can't be close", 0);
            } else {
                pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_OP_MODE, PMU_RG_BUCK_VAUD18_ON_MODE_MASK, PMU_RG_BUCK_VAUD18_ON_MODE_SHIFT, 0x3);
                if (pmu_get_register_value_ab2568(PMU_BUCK_VAUD18_OP_MODE, PMU_RG_BUCK_VAUD18_ON_MODE_MASK, PMU_RG_BUCK_VAUD18_ON_MODE_SHIFT)) {
                    pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_EN_CFG, PMU_RG_BUCK_ACT_VAUD18_EN_MASK, PMU_RG_BUCK_ACT_VAUD18_EN_SHIFT, operate);
                    pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_EN_CFG, PMU_RG_BUCK_SLP_VAUD18_EN_MASK, PMU_RG_BUCK_SLP_VAUD18_EN_SHIFT, operate);
#ifdef HAL_KEEP_VAUD18_POWER
                    pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_LP_CFG, 0x3, 0, 0);
#endif
                } else {
                    pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_OP_MODE, PMU_RG_BUCK_VAUD18_ON_MODE_MASK, PMU_RG_BUCK_VAUD18_ON_MODE_SHIFT, operate);

                }
            }
            break;
        case PMU_LDO_VA18:
            if (operate == PMU_ON) {
                va18_count = 0;
                hal_nvic_save_and_set_interrupt_mask(&mask_pri);
                if(pmu_va18_count==0){
                    pmu_va18_count=1;
                    pmu_va18_state = PMU_POWER_TRY_TO_ENABLE;
                    hal_nvic_restore_interrupt_mask(mask_pri);
                    pmu_set_register_value_ab2568(PMU_LDO_VA18_EN_CFG, PMU_RG_LDO_ACT_VA18_EN_MASK, PMU_RG_LDO_ACT_VA18_EN_SHIFT, 0x1);
                    pmu_set_register_value_ab2568(PMU_LDO_VA18_EN_CFG, PMU_RG_LDO_SLP_VA18_EN_MASK, PMU_RG_LDO_SLP_VA18_EN_SHIFT, 0x1);
                }else{
                    hal_nvic_restore_interrupt_mask(mask_pri);
                }
                while (pmu_va18_state != PMU_POWER_STABLE) {
                    if (pmu_va18_state == PMU_POWER_TRY_TO_DISABLE) {
                        assert(0);
                    }
                    temp = pmu_get_register_value_ab2568(PMU_LDO_VA18_CON1, PMU_DA_QI_VA18_STB_MASK, PMU_DA_QI_VA18_STB_SHIFT);
                    if (temp) {
                        pmu_va18_state = PMU_POWER_STABLE;
                        va18_count = 0;
                        break;
                    }
                    pmu_set_register_value_ab2568(PMU_LDO_VA18_EN_CFG, PMU_RG_LDO_ACT_VA18_EN_MASK, PMU_RG_LDO_ACT_VA18_EN_SHIFT, 0x1);
                    pmu_set_register_value_ab2568(PMU_LDO_VA18_EN_CFG, PMU_RG_LDO_SLP_VA18_EN_MASK, PMU_RG_LDO_SLP_VA18_EN_SHIFT, 0x1);
                    hal_gpt_delay_ms(1);
                    va18_count++;
                    if (va18_count >= 15) {
                        log_hal_msgid_info("Warning : VA18 Can't be enable", 0);
                        assert(0);
                    }
                }
            } else {
                if (!(HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION)) {
#ifndef HAL_PMU_NO_RTOS
                    vTaskSuspendAll();
#endif
                }
                hal_nvic_save_and_set_interrupt_mask(&mask_pri);
                if ((va18_flag == 0) && (pmu_va18_count == 1)) {
                    pmu_va18_count = 0;
                    pmu_va18_state = PMU_POWER_TRY_TO_DISABLE;
                    hal_nvic_restore_interrupt_mask(mask_pri);
                    pmu_set_register_value_ab2568(PMU_LDO_VA18_EN_CFG, PMU_RG_LDO_ACT_VA18_EN_MASK, PMU_RG_LDO_ACT_VA18_EN_SHIFT, 0);
                    pmu_set_register_value_ab2568(PMU_LDO_VA18_EN_CFG, PMU_RG_LDO_SLP_VA18_EN_MASK, PMU_RG_LDO_SLP_VA18_EN_SHIFT, 0);
                    pmu_va18_state = PMU_POWER_STABLE;
                    if (!(HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION)) {
#ifndef HAL_PMU_NO_RTOS
                        xTaskResumeAll();
#endif
                    }
                } else {
                    hal_nvic_restore_interrupt_mask(mask_pri);
                    if (!(HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION)) {
#ifndef HAL_PMU_NO_RTOS
                        xTaskResumeAll();
#endif
                    }
                }
            }
            break;
        case PMU_LDO_VLDO33:
            if (pmu_get_register_value_ab2568(PMU_LDO_VLDO33_OP_MODE, PMU_RG_LDO_VLDO33_ON_MODE_MASK, PMU_RG_LDO_VLDO33_ON_MODE_SHIFT)) {
                pmu_set_register_value_ab2568(PMU_LDO_VLDO33_EN_CFG, PMU_RG_LDO_ACT_VLDO33_EN_MASK, PMU_RG_LDO_ACT_VLDO33_EN_SHIFT, operate);
                pmu_set_register_value_ab2568(PMU_LDO_VLDO33_EN_CFG, PMU_RG_LDO_SLP_VLDO33_EN_MASK, PMU_RG_LDO_SLP_VLDO33_EN_SHIFT, operate);
            } else {
                pmu_set_register_value_ab2568(PMU_LDO_VLDO33_CON0, PMU_RG_LDO_VLDO33_EN_MASK, PMU_RG_LDO_VLDO33_EN_SHIFT, operate);

            }
            break;
        case PMU_LDO_VRF:
            if (pmu_get_register_value_ab2568(PMU_LDO_VRF11_OP_MODE, PMU_RG_LDO_VRF11_ON_MODE_MASK, PMU_RG_LDO_VRF11_ON_MODE_SHIFT)) {
                pmu_set_register_value_ab2568(PMU_LDO_VRF11_EN_CFG, PMU_RG_LDO_ACT_VRF11_EN_MASK, PMU_RG_LDO_ACT_VRF11_EN_SHIFT, operate);
            } else {
                pmu_set_register_value_ab2568(PMU_LDO_VRF11_CON0, PMU_RG_LDO_VRF11_EN_MASK, PMU_RG_LDO_VRF11_EN_SHIFT, operate);

            }

            break;
        case PMU_LDO_VSRAM:
            if (pmu_get_register_value_ab2568(PMU_LDO_VSRAM_OP_MODE, PMU_RG_LDO_VSRAM_ON_MODE_MASK, PMU_RG_LDO_VSRAM_ON_MODE_SHIFT)) {
                pmu_set_register_value_ab2568(PMU_LDO_VSRAM_EN_CFG, PMU_RG_LDO_ACT_VSRAM_EN_MASK, PMU_RG_LDO_ACT_VSRAM_EN_SHIFT, operate);
                pmu_set_register_value_ab2568(PMU_LDO_VSRAM_EN_CFG, PMU_RG_LDO_SLP_VSRAM_EN_MASK, PMU_RG_LDO_SLP_VSRAM_EN_SHIFT, operate);
            } else {
                pmu_set_register_value_ab2568(PMU_LDO_VSRAM_CON0, PMU_RG_LDO_VSRAM_EN_MASK, PMU_RG_LDO_VSRAM_EN_SHIFT, operate);
            }
            break;
    }
}

uint8_t pmu_get_power_status_2568(pmu_power_domain_t pmu_pdm) {
    uint8_t sta = 0;
    switch (pmu_pdm)
    {
        case PMU_BUCK_VCORE:
            sta = pmu_get_register_value_ab2568(PMU_BUCK_VCORE_CON0, PMU_RG_BUCK_VCORE_EN_MASK, PMU_RG_BUCK_VCORE_EN_SHIFT);
            break;
        case PMU_BUCK_VIO18:
            sta = pmu_get_register_value_ab2568(PMU_BUCK_VIO18_CON0, PMU_RG_BUCK_VIO18_EN_MASK, PMU_RG_BUCK_VIO18_EN_SHIFT);
            break;
        case PMU_BUCK_VRF:
            sta = pmu_get_register_value_ab2568(PMU_BUCK_VRF_CON0, PMU_RG_BUCK_VRF_EN_MASK, PMU_RG_BUCK_VRF_EN_SHIFT);
            break;
        case PMU_BUCK_VAUD18:
            sta = pmu_get_register_value_ab2568(PMU_BUCK_VAUD18_CON0, PMU_RG_BUCK_VAUD18_EN_MASK, PMU_RG_BUCK_VAUD18_EN_SHIFT);
            break;
        case PMU_LDO_VA18:
            sta = pmu_get_register_value_ab2568(PMU_LDO_VA18_CON0, PMU_RG_LDO_VA18_EN_MASK, PMU_RG_LDO_VA18_EN_SHIFT);
            break;
        case PMU_LDO_VLDO33:
            sta = pmu_get_register_value_ab2568(PMU_LDO_VLDO33_CON0, PMU_RG_LDO_VLDO33_EN_MASK, PMU_RG_LDO_VLDO33_EN_SHIFT);
            break;
        case PMU_LDO_VRF:
            sta = pmu_get_register_value_ab2568(PMU_LDO_VRF11_CON0, PMU_RG_LDO_VRF11_EN_MASK, PMU_RG_LDO_VRF11_EN_SHIFT);
            break;
        case PMU_LDO_VSRAM:
            sta = pmu_get_register_value_ab2568(PMU_LDO_VSRAM_CON0, PMU_RG_LDO_VSRAM_EN_MASK, PMU_RG_LDO_VSRAM_EN_SHIFT);
            break;
    }
    return sta;
}

/*
 * In normal mode , switch control mode
 * set RG_BUCK_Vxxx_ON_MODE = 0: SW mode  ; set 1: HW mode
 */
void pmu_switch_control_mode_2568(pmu_power_domain_t domain, pmu_control_mode_t mode) {
    switch(domain) {
        case PMU_BUCK_VCORE:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_OP_MODE, PMU_RG_BUCK_VCORE_ON_MODE_MASK, PMU_RG_BUCK_VCORE_ON_MODE_SHIFT, mode);
            break;
        case PMU_BUCK_VIO18:
            pmu_set_register_value_ab2568(PMU_BUCK_VIO18_OP_MODE, PMU_RG_BUCK_VIO18_ON_MODE_MASK, PMU_RG_BUCK_VIO18_ON_MODE_SHIFT, mode);
            break;
        case PMU_BUCK_VRF:
            pmu_set_register_value_ab2568(PMU_BUCK_VRF_OP_MODE, PMU_RG_BUCK_VRF_ON_MODE_MASK, PMU_RG_BUCK_VRF_ON_MODE_SHIFT, mode);
            break;
        case PMU_BUCK_VAUD18:
            pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_OP_MODE, PMU_RG_BUCK_VAUD18_ON_MODE_MASK, PMU_RG_BUCK_VAUD18_ON_MODE_SHIFT, mode);
            break;
        case PMU_LDO_VA18:
            pmu_set_register_value_ab2568(PMU_LDO_VA18_OP_MODE, PMU_RG_LDO_VA18_ON_MODE_MASK, PMU_RG_LDO_VA18_ON_MODE_SHIFT, mode);
            break;
        case PMU_LDO_VLDO33:
            pmu_set_register_value_ab2568(PMU_LDO_VLDO33_OP_MODE, PMU_RG_LDO_VLDO33_ON_MODE_MASK, PMU_RG_LDO_VLDO33_ON_MODE_SHIFT, mode);
            break;
        case PMU_LDO_VRF:
            pmu_set_register_value_ab2568(PMU_LDO_VRF11_OP_MODE, PMU_RG_LDO_VRF11_ON_MODE_MASK, PMU_RG_LDO_VRF11_ON_MODE_SHIFT, mode);
            break;
        case PMU_LDO_VSRAM:
            pmu_set_register_value_ab2568(PMU_LDO_VSRAM_OP_MODE, PMU_RG_LDO_VSRAM_ON_MODE_MASK, PMU_RG_LDO_VSRAM_ON_MODE_SHIFT, mode);
            break;
    }
}

/* SW mode lp enable/disable
 * 1'b0: wo LP; 1'b1: wi LP
 * */
void pmu_enable_sw_lp_mode_2568(pmu_power_domain_t domain, pmu_control_mode_t mode) {
    switch(domain) {
        case PMU_BUCK_VCORE:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_OP_MODE, PMU_RG_BUCK_VCORE_LP_MODE_MASK, PMU_RG_BUCK_VCORE_LP_MODE_SHIFT, mode);
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_CON0, PMU_RG_BUCK_VCORE_LP_MASK, PMU_RG_BUCK_VCORE_LP_SHIFT, PMU_ON);
            break;
        case PMU_BUCK_VIO18:
            pmu_set_register_value_ab2568(PMU_BUCK_VIO18_OP_MODE, PMU_RG_BUCK_VIO18_LP_MODE_MASK, PMU_RG_BUCK_VIO18_LP_MODE_SHIFT, mode);
            pmu_set_register_value_ab2568(PMU_BUCK_VIO18_CON0, PMU_RG_BUCK_VIO18_LP_MASK, PMU_RG_BUCK_VIO18_LP_SHIFT, PMU_ON);
            break;
        case PMU_BUCK_VRF:
            pmu_set_register_value_ab2568(PMU_BUCK_VRF_OP_MODE, PMU_RG_BUCK_VRF_LP_MODE_MASK, PMU_RG_BUCK_VRF_LP_MODE_SHIFT, mode);
            pmu_set_register_value_ab2568(PMU_BUCK_VRF_CON0, PMU_RG_BUCK_VRF_LP_MODE_MASK, PMU_RG_BUCK_VRF_LP_SHIFT, PMU_ON);
            break;
        case PMU_BUCK_VAUD18:
            pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_OP_MODE, PMU_RG_BUCK_VAUD18_LP_MODE_MASK, PMU_RG_BUCK_VAUD18_LP_MODE_SHIFT, mode);
            pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_CON0, PMU_RG_BUCK_VAUD18_LP_MASK, PMU_RG_BUCK_VAUD18_LP_SHIFT, PMU_ON);
            break;
        case PMU_LDO_VA18:
            pmu_set_register_value_ab2568(PMU_LDO_VA18_OP_MODE, PMU_RG_LDO_VA18_LP_MODE_MASK, PMU_RG_LDO_VA18_LP_MODE_SHIFT, mode);
            pmu_set_register_value_ab2568(PMU_LDO_VA18_CON0, PMU_RG_LDO_VA18_LP_MASK, PMU_RG_LDO_VA18_LP_SHIFT, PMU_ON);
            break;
        case PMU_LDO_VLDO33:
            pmu_set_register_value_ab2568(PMU_LDO_VLDO33_OP_MODE, PMU_RG_LDO_VLDO33_LP_MODE_MASK, PMU_RG_LDO_VLDO33_LP_MODE_SHIFT, mode);
            pmu_set_register_value_ab2568(PMU_LDO_VLDO33_CON0, PMU_RG_LDO_VLDO33_LP_MASK, PMU_RG_LDO_VLDO33_LP_SHIFT, PMU_ON);
            break;
    }
}

void pmu_ignore_vcore_mode(pmu_power_operate_t operate){
	ingore_flag = operate;
}

void pmu_vocre_lp_mode(pmu_control_mode_t mode){
#ifdef PMU_LOW_POWER_VCORE_SETTING
    if(ingore_flag){
       pmu_switch_control_mode(PMU_BUCK_VCORE,mode);  /* VCORE Low power setting :1*/
       pmu_enable_sw_lp_mode(PMU_BUCK_VCORE,mode);    /* VCORE Low power setting :2*/
    }
#endif
}

/*==========[Buck/Ldo voltage]==========*/
pmu_operate_status_t pmu_select_vcore_voltage_ab2568(pmu_power_stage_t mode, pmu_power_vcore_voltage_t vol) {
    if ((mode > PMU_DVS) | (mode < PMU_SLEEP) | (vol > PMIC_VCORE_0P9_V) | (vol < PMIC_VCORE_0P5_V)) {
        log_hal_msgid_error("[PMU] vcore_voltage Error input", 0);
        return PMU_ERROR;
    }
    if ((mode !=PMU_DVS) && (pmu_lock_status > 0)) {
        log_hal_msgid_error("[PMU] VCORE in locked ", 0);
        return PMU_ERROR;
    }

    switch (vol)
    {
        case PMIC_VCORE_0P6_V:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x10);
            break;
        case PMIC_VCORE_0P7_V:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x20);
            pmu_vocre_lp_mode(PMU_SW_MODE);
            break;
        case PMIC_VCORE_0P73_V:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x25);
            pmu_vocre_lp_mode(PMU_SW_MODE);
            break;
        case PMIC_VCORE_0P75_V:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x28);
            pmu_vocre_lp_mode(PMU_HW_MODE);
            break;
        case PMIC_VCORE_0P8_V:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x30);
            pmu_vocre_lp_mode(PMU_HW_MODE);
            break;
        case PMIC_VCORE_0P9_V:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x40);
            pmu_vocre_lp_mode(PMU_HW_MODE);
            break;
        default:
            pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x40);
            pmu_vocre_lp_mode(PMU_HW_MODE);
    }

    return PMU_OK;
}

void pmu_ddie_sram_setting(uint8_t ste) {
    if (ste >= 6) {
        //sram 0.9V
        pmu_select_vsram_voltage_ab2568(PMU_NORMAL,PMIC_VSRAM_0P9);  //sram set 0.9V rsam
        SRAM_RF_DELSEL = 0x0000010B; //SRAM_RF_DELSEL
        SRAM_HDE_DELSEL = 0x00000102; //SRAM_HDE_DELSEL
        SRAM_UHDE_DELSEL = 0x00030102; //SRAM_UHDE_DELSEL
    } else {
        SRAM_RF_DELSEL = 0x00000304; //SRAM_RF_DELSEL
        SRAM_HDE_DELSEL = 0x00000304; //SRAM_HDE_DELSEL
        SRAM_UHDE_DELSEL = 0x00050303; //SRAM_UHDE_DELSEL
        pmu_select_vsram_voltage_ab2568(PMU_NORMAL,PMIC_VSRAM_0P8);  //sram set 0.8V rsam
    }
}

/*For DVFS select vsram voltage*/

pmu_operate_status_t pmu_select_vsram_voltage_ab2568(pmu_power_stage_t mode, pmu_power_vsram_voltage_t vol) {
    if (mode == PMU_NORMAL) {
        pmu_set_register_value_ab2568(PMU_LDO_SRAM_CON3, PMU_RG_VSRAM_VOSEL_MASK, PMU_RG_VSRAM_VOSEL_SHIFT, vol);
        return PMU_OK;
    }else{
        return PMU_ERROR;
    }
}

pmu_power_vcore_voltage_t pmu_get_vcore_setting_index(uint16_t vcore) {
    uint8_t vcbuck_voval[7] = { 0x0, 0x10, 0x20, 0x25, 0x28, 0x30, 0x40 };
    int vosel = 0;
    for (vosel = 0; vosel < 7; vosel++) {
        if (vcore == vcbuck_voval[vosel]) {
            return ((pmu_power_vcore_voltage_t) (vosel));
        }
    }
    return (PMU_ERROR);
}

pmu_power_vcore_voltage_t pmu_get_vcore_voltage_ab2568(void) {
    uint32_t temp = 0;
    temp = pmu_get_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT);
    return pmu_get_vcore_setting_index(temp);
}

void pmu_vcroe_voltage_turing(int symbol, int num) {
    uint32_t cur_v;
    if (symbol) {
        cur_v = pmu_get_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT);
        cur_v += num;
        pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, cur_v);
    } else {
        cur_v = pmu_get_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT);
        cur_v -= num;
        pmu_set_register_value_ab2568(PMU_BUCK_VCORE_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, cur_v);
    }
}

void pmu_vaud18_voltage_turing(int symbol, int num) {
    uint32_t cur_v;
    if (symbol) {
        cur_v = pmu_get_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VAUD18_VOSEL_MASK, PMU_RG_BUCK_VAUD18_VOSEL_SHIFT);
        cur_v += num;
        pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, cur_v);
    } else {
        cur_v = pmu_get_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT);
        cur_v -= num;
        pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, cur_v);
    }
}

void pmu_set_vaud18_voltage_2568(pmu_power_vaud18_voltage_t oper){
    switch(oper){
        case PMIC_VAUD18_1P45_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x2B);
        break;
        case PMIC_VAUD18_1P50_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x30);
        break;
        case PMIC_VAUD18_1P55_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x35);
        break;
        case PMIC_VAUD18_1P60_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x3B);
        break;
        case PMIC_VAUD18_1P65_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x40);
        break;
        case PMIC_VAUD18_1P70_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x45);
        break;
        case PMIC_VAUD18_1P71_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x47);
        break;
        case PMIC_VAUD18_1P75_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x4B);
        break;
        case PMIC_VAUD18_1P80_V:
             pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x50);
        break;
    }
}

void pmu_vio18_voltage_turing(int symbol, int num) {
    uint32_t cur_v;
    if (symbol) {
        cur_v = pmu_get_register_value_ab2568(PMU_BUCK_VIO18_ELR0, PMU_RG_BUCK_VIO18_VOSEL_MASK, PMU_RG_BUCK_VIO18_VOSEL_SHIFT);
        cur_v += num;
        pmu_set_register_value_ab2568(PMU_BUCK_VIO18_ELR0, PMU_RG_BUCK_VIO18_VOSEL_MASK, PMU_RG_BUCK_VIO18_VOSEL_SHIFT, cur_v);
    } else {
        cur_v = pmu_get_register_value_ab2568(PMU_BUCK_VIO18_ELR0, PMU_RG_BUCK_VIO18_VOSEL_MASK, PMU_RG_BUCK_VIO18_VOSEL_SHIFT);
        cur_v -= num;
        pmu_set_register_value_ab2568(PMU_BUCK_VIO18_ELR0, PMU_RG_BUCK_VIO18_VOSEL_MASK, PMU_RG_BUCK_VIO18_VOSEL_SHIFT, cur_v);
    }
}

void pmu_selet_voltage_2568(pmu_power_stage_t mode,pmu_power_domain_t domain,uint32_t vol) {
    switch (domain) {
        case PMU_BUCK_VIO18: //VIO18 1.12V~1.96V default setting Normal:1.8V ;Sleep :1.8V
            if (mode == PMU_NORMAL) {
                pmu_set_register_value_ab2568(PMU_BUCK_VIO18_ELR0, PMU_RG_BUCK_VIO18_VOSEL_MASK, PMU_RG_BUCK_VIO18_VOSEL_SHIFT, vol);
            }
            break;
        case PMU_BUCK_VRF: //VRF 0.7V
            pmu_set_register_value_ab2568(PMU_BUCK_VRF_ELR0, PMU_RG_BUCK_VRF_VOSEL_MASK, PMU_RG_BUCK_VRF_VOSEL_SHIFT, vol);
            pmu_set_register_value_ab2568(PMU_BUCK_VRF_CON1, PMU_RG_BUCK_VRF_VOSEL_SLEEP_MASK, PMU_RG_BUCK_VRF_VOSEL_SLEEP_SHIFT, vol);
            break;
        default:
            log_hal_msgid_info("Not support",0);
            break;
    }

}

/*==========[Power key & Cap touch]==========*/
pmu_operate_status_t pmu_pwrkey_enable_2568(pmu_power_operate_t oper) {
    return pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_PWRKEY_RST_EN_MASK, PMU_RG_PWRKEY_RST_EN_SHIFT, oper);
}

pmu_operate_status_t pmu_pwrkey_duration_time(pmu_pwrkey_time_t tmr) {
    return pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_PWRKEY_RST_TD_MASK, PMU_RG_PWRKEY_RST_TD_SHIFT, tmr);
}

pmu_operate_status_t pmu_long_press_shutdown_function_sel(pmu_pwrkey_scenario_t oper) {
    if (oper == PMU_RESET_DEFAULT) {
        pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_STRUP_LONG_PRESS_EXT_EN_MASK, PMU_RG_STRUP_LONG_PRESS_EXT_EN_SHIFT, PMU_ON);
        return pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_STRUP_LONG_PRESS_EXT_SEL_MASK, PMU_RG_STRUP_LONG_PRESS_EXT_SEL_SHIFT, 0);
    } else {
        pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_STRUP_LONG_PRESS_EXT_EN_MASK, PMU_RG_STRUP_LONG_PRESS_EXT_EN_SHIFT, PMU_ON);
        return pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_STRUP_LONG_PRESS_EXT_SEL_MASK, PMU_RG_STRUP_LONG_PRESS_EXT_SEL_SHIFT, oper);
    }
}

pmu_operate_status_t pmu_cap_touch_enable(pmu_power_operate_t oper) {
    return pmu_set_register_value_ab2568(PMU_CAP_LPSD_CON0, PMU_RG_CAP_LPSD_EN_MASK, PMU_RG_CAP_LPSD_EN_SHIFT, oper);
}

pmu_operate_status_t pmu_cap_touch_shutdown_enable(pmu_power_operate_t oper) {
    return pmu_set_register_value_ab2568(PMU_CAP_LPSD_CON1, PMU_RG_CAP_LPSD_MASK, PMU_RG_CAP_LPSD_SHIFT, oper);
}

pmu_operate_status_t pmu_pk_filter(uint8_t pk_sta) {
    if (pk_sta == 0) { //Press section : pk_next should be Press
        if (pk_next == PMU_PK_RELEASE && pmu_function_table[0].init_status != 0) {
            log_hal_msgid_info("PMIC INT[0]  [1]",0);
            pmu_function_table[0].pmu_callback();
        }
        pk_next = PMU_PK_RELEASE;
        return PMU_OK;
    } else if (pk_sta == 1) { //Release section : pk_next should be Press
        if ((first_boot_up==1) && pk_next == PMU_PK_PRESS && pmu_function_table[0].init_status != 0) {
            log_hal_msgid_info("PMIC INT[0]  [0]",0);
            pmu_function_table[0].pmu_callback();
        }
        first_boot_up=1; /*For first time boot up power key single*/
        pk_next = PMU_PK_PRESS;
        return PMU_OK;
    }else{
        log_hal_msgid_info("PMIC POWERKEY HW error [%d]", 1,pk_sta);
        return PMU_ERROR;
    }
    return PMU_OK;
}

pmu_operate_status_t pmu_pwrkey_normal_key_init(pmu_pwrkey_config_t *config)
{
    pmu_status_t status = PMU_STATUS_ERROR;
    status = pmu_register_callback(RG_INT_PWRKEY, config->callback1,config->user_data1);
    if (status != PMU_STATUS_SUCCESS) {
        return PMU_STATUS_ERROR;
    }
    return PMU_STATUS_SUCCESS;
}

/*==========[Get PMIC hw informantion]==========*/
uint8_t pmu_get_power_on_reason_2568(void) {
    uint8_t reason;
    reason = pmu_get_register_value_ab2568(PMU_PONSTS, 0x1F, 0);
    return reason;
}

uint8_t pmu_get_power_off_reason_2568(void) {
    uint8_t reason;
    reason = pmu_get_register_value_ab2568(PMU_VREF_Interface_ANA_CON3, PMU_RGS_POFFSTS_MASK, PMU_RGS_POFFSTS_SHIFT);
    return reason;
}

uint8_t pmu_get_usb_input_status_2568(void) // For usb driver get usb put in status
{
    return pmu_get_register_value_ab2568(PMU_CHR_AO_DBG0, PMU_DA_QI_CHR_REF_EN_MASK, PMU_DA_QI_CHR_REF_EN_SHIFT);
}

/*==========[Other]==========*/
void pmu_srclken_control_mode_2568(pmu_power_operate_t mode)
{
    if(mode==PMU_ON) {
        pmu_set_register_value_ab2568(PMU_TPO_CON1, PMU_RG_SRCLKEN_HW_MODE_MASK, PMU_RG_SRCLKEN_HW_MODE_SHIFT, 1);
    } else {
        pmu_set_register_value_ab2568(PMU_TPO_CON1, PMU_RG_SRCLKEN_HW_MODE_MASK, PMU_RG_SRCLKEN_HW_MODE_SHIFT, 0);
    }
}
void pmu_latch_power_key_for_bootloader(void) {
    pmu_set_register_value_ab2568(PMU_I2C_OUT_TYPE, PMU_I2C_CONFIG_MASK, PMU_I2C_CONFIG_SHIFT, 1); //D2D need to setting in PP mode, first priority, AB1555 no need, AB1558 D2D nessary
    pmu_set_register_value_ab2568(PMU_IOCFG3,PMU_RG_I2C_CLK_IC_MASK,PMU_RG_I2C_CLK_IC_SHIFT,0x4); //turn on smitt trigger for stability
    pmu_set_register_value_ab2568(PMU_IOCFG3,PMU_RG_I2C_DAT_IC_MASK,PMU_RG_I2C_DAT_IC_SHIFT,0x4); //turn on smitt trigger for stability
    pmu_set_register_value_ab2568(PMU_PWRHOLD, PMU_RG_PWRHOLD_MASK, PMU_RG_PWRHOLD_SHIFT, 1);//Power hold
    pmu_set_register_value_ab2568(PMU_RSTCFG1,PMU_RG_WDTRSTB_EN_MASK,PMU_RG_WDTRSTB_EN_SHIFT,0x1); // Enable WDT
    pmu_set_register_value_ab2568(PMU_RSTCFG1,PMU_RG_WDTRSTB_DEB_MASK,PMU_RG_WDTRSTB_DEB_SHIFT,0x1); // Enable WDT
}

void pmu_efuse_enable_reading(void) {
    //Get Original MatchKey
    gAuxadcEfuseInfo.matchKey = pmu_get_register_value_ab2568(PMU_OTP_CON7, PMU_RG_OTP_RD_PKEY_MASK, PMU_RG_OTP_RD_PKEY_SHIFT);
    // RG_EFUSE_CK_PDN
    pmu_set_register_value_ab2568(PMU_CKCFG2, PMU_RG_EFUSE_CK_PDN_MASK, PMU_RG_EFUSE_CK_PDN_SHIFT, 0);
    //RG_EFUSE_CK_PDN_HWEN
    pmu_set_register_value_ab2568(PMU_CKCFG1, PMU_RG_EFUSE_CK_PDN_HWEN_MASK, PMU_RG_EFUSE_CK_PDN_HWEN_SHIFT, 0);
    // OTP set Match Key
    pmu_set_register_value_ab2568(PMU_OTP_CON7, PMU_RG_OTP_RD_PKEY_MASK, PMU_RG_OTP_RD_PKEY_SHIFT, 0x0289);
    /*Set SW trigger read mode
     * Set HW Mode ;
     * 0: non SW trigger read mode
     * 1: SW trigger read mode*/
    pmu_set_register_value_ab2568(PMU_OTP_CON11, PMU_RG_OTP_RD_SW_MASK, PMU_RG_OTP_RD_SW_SHIFT, 1);
}

void pmu_efuse_disable_reading(void) {
    // RG_EFUSE_CK_PDN
    pmu_set_register_value_ab2568(PMU_CKCFG2, PMU_RG_EFUSE_CK_PDN_MASK, PMU_RG_EFUSE_CK_PDN_SHIFT, 1);
    //RG_EFUSE_CK_PDN_HWEN
    pmu_set_register_value_ab2568(PMU_CKCFG1, PMU_RG_EFUSE_CK_PDN_HWEN_MASK, PMU_RG_EFUSE_CK_PDN_HWEN_SHIFT, 1);
    // OTP set Original Match Key
    pmu_set_register_value_ab2568(PMU_OTP_CON7, PMU_RG_OTP_RD_PKEY_MASK, PMU_RG_OTP_RD_PKEY_SHIFT, gAuxadcEfuseInfo.matchKey);
    /*Set SW trigger read mode
     * Set HW Mode ;
     * 0: non SW trigger read mode
     * 1: SW trigger read mode*/
    pmu_set_register_value_ab2568(PMU_OTP_CON11, PMU_RG_OTP_RD_SW_MASK, PMU_RG_OTP_RD_SW_SHIFT, 0);
}

void pmu_set_audio_enhance_2568(pmu_power_operate_t oper){
    if(oper){
        pmu_set_register_value_ab2568(PMU_VAUD18DRV_MODE, 0x3, 0, 0x3);
    }else{
        pmu_set_register_value_ab2568(PMU_VAUD18DRV_MODE, 0x3, 0, 0x2);
    }

}
/* 0: efuse not ready; 1 : efuse ready*/
bool pmu_get_efuse_status(void){
    return pmu_get_register_value_ab2568(PMU_AO_EFUSE_RSV, PMU_RG_AO_EFUSE_RSV_MASK, PMU_RG_AO_EFUSE_RSV_SHIFT);
}

void pmu_safety_confirmation(void) {
    if ((pmu_get_register_value_ab2568(PMU_HWCID, PMU_HWCID_MASK, PMU_HWCID_SHIFT) != 0x8820)
            && (pmu_get_register_value_ab2568(PMU_AO_EFUSE_RSV, PMU_RG_AO_EFUSE_RSV_MASK, PMU_RG_AO_EFUSE_RSV_SHIFT) != 0x1)) {
        log_hal_msgid_info("Please Check PMIC Efuse and Version", 0);
        hal_gpt_delay_ms(1);
        hal_rtc_init();
        hal_rtc_enter_rtc_mode();
    }
    if (pmu_get_register_value_ab2568(PMU_AO_EFUSE_RSV, PMU_RG_AO_EFUSE_RSV_MASK, PMU_RG_AO_EFUSE_RSV_SHIFT) != 0x1) {
        log_hal_msgid_info("Please Check PMIC Efuse", 0);
        pmu_set_register_value(PMU_CORE_CORE_AO_ELR_2, PMU_RG_EFUSE_PREG_VTRIM_MASK, PMU_RG_EFUSE_PREG_VTRIM_SHIFT, 0x15);
    }
}

/*when boot up,press power key need more than the specific time*/
void pmu_press_pk_time(void) {
    uint8_t pk_sta;
    uint32_t pmu_gpt_start, pmu_get_press_time, pmu_get_duration_time = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &pmu_gpt_start);
    while (1) {
        pk_sta = pmu_get_register_value(PMU_PSSTS2, PMU_PWRKEY_VAL_MASK, PMU_PWRKEY_VAL_SHIFT);
        if (pk_sta == 0) {
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &pmu_get_press_time);
            hal_gpt_get_duration_count(pmu_gpt_start, pmu_get_press_time, &pmu_get_duration_time);
            pmu_set_register_value(PMU_PWRHOLD, PMU_RG_PWRHOLD_MASK, PMU_RG_PWRHOLD_SHIFT, 1);   //Power hold
        } else {
            pmu_get_press_time = 0;
            log_hal_msgid_error("ON[%x]OFF[%x]DT[%d]PT[%d]",4,pmu_get_power_on_reason(),pmu_get_power_off_reason(),pmu_get_duration_time,PMU_PRESS_PK_TIME);
            log_hal_msgid_error("Boot up fail , press pk need more than the specific time %d or PMIC OP", 1,PMU_PRESS_PK_TIME);
            hal_gpt_delay_ms(1);
            hal_rtc_init();
            hal_rtc_enter_rtc_mode();
        }
        if ((pmu_get_duration_time > PMU_PRESS_PK_TIME)) {
            log_hal_msgid_error("Boot on!! , press pk time %d", 1,pmu_get_duration_time);
            break;
        }
    }
}
/*
 * VBAT to AVDD50_CHR over current protection(OCP) enable
 * 0:disable
 * 1:enable*/
void pmu_enable_ocp_2568(pmu_power_operate_t oper){
    pmu_set_register_value(PMU_TPO_CON5, PMU_RG_OC_EN_MASK, PMU_RG_OC_EN_SHIFT, oper);
    pmu_set_register_value(PMU_TPO_CON5, PMU_RG_OC_EN_LAT_RELOAD_MASK, PMU_RG_OC_EN_LAT_RELOAD_SHIFT, 0x1);
    pmu_set_register_value(PMU_TPO_CON5, PMU_RG_OC_EN_LAT_RELOAD_MASK, PMU_RG_OC_EN_LAT_RELOAD_SHIFT, 0x0);
    if(pmu_get_register_value_ab2568(PMU_HWCID, PMU_HWCID_MASK, PMU_HWCID_SHIFT) == 0x8820){
       log_hal_msgid_info("Warning,OCP can't be close.", 0);
    }
}

/*Internal use for force LPSD*/
void pmu_enable_lpsd_2568(void){
    pmu_pwrkey_enable(PMU_ON);
#ifdef HAL_CAPTOUCH_MODULE_ENABLED
    captouch_set_vbus_flag(PMU_ON);
    pmu_set_register_value(PMU_CAP_LPSD_CON0,0x3,0,0x1);
    pmu_set_register_value(PMU_CAP_LPSD_CON1,0x3,0,0);
#endif
}

void pmu_get_lock_status_2568(int sta){
    log_hal_msgid_info("[PMU] current lock vol = %d  sta :%d[0:Before lock 1:Afeter lock]\r\n",2, pmu_get_vcore_voltage_ab2568(), sta);
    log_hal_msgid_info("[PMU] SRAM RF[0x%x] HDE[0x%x] UHDE[0x%x]\r\n",3, SRAM_RF_DELSEL,SRAM_HDE_DELSEL,SRAM_UHDE_DELSEL);
    log_hal_msgid_info("[PMU] lock state : %d pmu_switch_case = %d[2:up;1:;down;0:ignore]\r\n",2,pmu_lock_status,pmu_switch_case);
}
/* API return 1:press ; 0 :release */
bool pmu_get_pwrkey_state_2568(void){
    int status = pmu_get_register_value_ab2568(PMU_PSSTS2,PMU_PWRKEY_VAL_MASK,PMU_PWRKEY_VAL_SHIFT);
    return (!status);
}
/*==========[PMIC irq]==========*/
void pmu_eint_handler(void *parameter)
{
    uint8_t pk_sta;
    int index=0,index_shift=0;
    uint32_t unmask_index;
    hal_eint_mask(HAL_EINT_PMU);
    unmask_index = pmu_register_interrupt;
    pmu_get_all_int_status();
    pk_sta = pmu_get_register_value_ab2568(PMU_PSSTS2,PMU_PWRKEY_VAL_MASK,PMU_PWRKEY_VAL_SHIFT);
       for (index = 0; index < PMU_INT_MAX; index++) {
            if ((unmask_index >> (index - index_shift)) & (pmu_get_status_interrupt(index))) {
                if (index == 0) {
                    pmu_pk_filter(pk_sta);
                }
#ifdef PMU_WO_MPLOG
                log_hal_msgid_info("PMIC INT[%d] [%d]", 2,index,pk_sta);
#else
                LOG_W(MPLOG,"PMIC INT[%d] [%d]",index,pk_sta);/*Log output by BT*/
#endif
                if(pmu_function_table[index].init_status!=0){
                    pmu_function_table[index].pmu_callback();
                }
#ifdef  HAL_CAPTOUCH_MODULE_ENABLED
#ifdef AB1568
            if (index == RG_INT_CHRDET) {
                captouch_set_vbus_flag(pmu_get_chr_detect_value());
            }
#endif
#endif
            pmu_irq_count(index);
            }
            if (index == 31) {
                unmask_index = pmu_register_interrupt_2;
                index_shift = 32;
            }
        }
    pmu_clear_interrupt(PMU_INT_MAX);
    hal_eint_unmask(HAL_EINT_PMU);
}

void pmu_eint_init(void)
{
    hal_eint_config_t config;
    config.trigger_mode = HAL_EINT_LEVEL_LOW;
    config.debounce_time = 0;
    hal_eint_init(HAL_EINT_PMU, &config);    /*set EINT trigger mode and debounce time.*/
    hal_eint_register_callback(HAL_EINT_PMU, pmu_eint_handler, NULL);  /*register a user callback.*/
    hal_eint_unmask(HAL_EINT_PMU);
}

void pmu_get_all_int_status(void) {
    if (pmu_irq_enable_com0 != 0) {
        pmic_irq0 = pmu_get_register_value_ab2568(PMU_INT_STATUS0, 0xffff, 0);
    }
    if (pmu_irq_enable_com1 != 0) {
        pmic_irq1 = pmu_get_register_value_ab2568(PMU_INT_STATUS1, 0xffff, 0);
    }
    if (pmu_irq_enable_com2 != 0) {
        pmic_irq2 = pmu_get_register_value_ab2568(PMU_INT_STATUS2, 0xffff, 0);
    }
    if (pmu_irq_enable_com3 != 0) {
        pmic_irq3 = pmu_get_register_value_ab2568(PMU_INT_STATUS3, 0xffff, 0);
    }
}

int pmu_get_status_interrupt(pmu_interrupt_index_t int_channel)
{
    int statusValue = -1;
    if ((int_channel >= 0) && (int_channel <= 8)) {
        statusValue = pmu_get_register_value_ddie((uint32_t)&pmic_irq0, 1, int_channel);
    } else if (int_channel == 9) {
        statusValue = pmu_get_register_value_ab2568(PMU_INT_STATUS0, PMU_RG_INT_STATUS_CHRDET_MASK, PMU_RG_INT_STATUS_CHRDET_SHIFT);
    } else if ((int_channel >= 10) && (int_channel <= 15)) {
        statusValue = pmu_get_register_value_ddie((uint32_t)&pmic_irq0, 1, int_channel);
    }else if ((int_channel >= 16) && (int_channel <= 31)) {
        statusValue = pmu_get_register_value_ddie((uint32_t)&pmic_irq1, 1, (int_channel - 11));
    } else if ((int_channel >= 32) && (int_channel <= 39)) {
        statusValue = pmu_get_register_value_ddie((uint32_t)&pmic_irq2, 1, (int_channel - 24));
    } else if ((int_channel >= 40) && (int_channel <= 43)) {
        statusValue = pmu_get_register_value_ddie((uint32_t)&pmic_irq3, 1, (int_channel - 30));
    } else {
        log_hal_msgid_info("Error interrupt index", 0);
        return PMU_STATUS_INVALID_PARAMETER;
    }
    return statusValue;
}

pmu_status_t pmu_clear_interrupt(pmu_interrupt_index_t int_channel) {
    if (int_channel == PMU_INT_MAX) {
        if ((pmu_irq_enable_com0 != 0) || (event_con0 != 0)) {
            pmu_set_register_value_ab2568(PMU_INT_STATUS0, 0xffff, 0, 0xffff);
        }
        if ((pmu_irq_enable_com1 != 0) || (event_con1 != 0)) {
            pmu_set_register_value_ab2568(PMU_INT_STATUS1, 0xffff, 0, 0xffff);
        }
        if ((pmu_irq_enable_com2 != 0) || (event_con2 != 0)) {
            pmu_set_register_value_ab2568(PMU_INT_STATUS2, 0x3f, 0, 0x3f);
        }
        if ((pmu_irq_enable_com3 != 0) || (event_con3 != 0)) {
            pmu_set_register_value_ab2568(PMU_INT_STATUS3, 0xf, 0, 0xf);
        }
    } else {
        pmu_control_enable_interrupt(int_channel, 0);
        hal_gpt_delay_us(150);
        pmu_control_enable_interrupt(int_channel, 1);
    }
    return PMU_STATUS_SUCCESS;
}

pmu_status_t pmu_control_enable_interrupt(pmu_interrupt_index_t int_channel, int isEnable) {
    if ((int_channel >= 0) && (int_channel <= 15)) {
        pmu_set_register_value_ab2568(PMU_INT_CON0, 0x1, int_channel, isEnable);
        if (isEnable) {
            pmu_irq_enable_com0 |= 0x1 << int_channel;
        } else {
            pmu_set_register_value_ddie((uint32_t)&pmu_irq_enable_com0, 0x1, int_channel, 0);
        }
    } else if ((int_channel >= 16) && (int_channel <= 31)) {
        pmu_set_register_value_ab2568(PMU_INT_CON1, 0x1, (int_channel - 11), isEnable);

        if (isEnable) {
            pmu_irq_enable_com1 |= 0x1 << (int_channel - 16);
        } else {
            pmu_set_register_value_ddie((uint32_t)&pmu_irq_enable_com1, 0x1, (int_channel - 11), 0);
        }
    } else if ((int_channel >= 32) && (int_channel <= 39)) {
        pmu_set_register_value_ab2568(PMU_INT_CON2, 0x1, (int_channel - 24), isEnable);

        if (isEnable) {
            pmu_irq_enable_com2 |= 0x1 << (int_channel - 32);
        } else {
            pmu_set_register_value_ddie((uint32_t)&pmu_irq_enable_com2, 0x1, (int_channel - 24), 0);
        }
    } else if ((int_channel >= 40) && (int_channel <= 43)) {
        pmu_set_register_value_ab2568(PMU_INT_CON3, 0x1, (int_channel - 30), isEnable);
        if (isEnable) {
            pmu_irq_enable_com3 |= 0x1 << (int_channel - 40);
        } else {
            pmu_set_register_value_ddie((uint32_t)&pmu_irq_enable_com3, 0x1, (int_channel - 30), 0);
        }
    } else {
        log_hal_msgid_info("Error interrupt index", 0);
        return PMU_STATUS_ERROR;
    }

    return PMU_STATUS_SUCCESS;
}

pmu_status_t pmu_control_mask_interrupt(pmu_interrupt_index_t int_channel, int isEnable)
{
    if ((int_channel >= 0) && (int_channel <= 15)) {
        pmu_set_register_value_ab2568(PMU_INT_MASK_CON0, 0x1, int_channel, isEnable);
    } else if ((int_channel >= 16) && (int_channel <= 31)) {
        pmu_set_register_value_ab2568(PMU_INT_MASK_CON1, 0x1, (int_channel - 11), isEnable);
    } else if ((int_channel >= 32) && (int_channel <= 39)) {
        pmu_set_register_value_ab2568(PMU_INT_MASK_CON2, 0x1, (int_channel - 24), isEnable);
    } else if ((int_channel >= 40) && (int_channel <= 43)) {
        pmu_set_register_value_ab2568(PMU_INT_MASK_CON3, 0x1, (int_channel - 30), isEnable);
    } else {
        log_hal_msgid_info("Error interrupt index", 0);
        return PMU_STATUS_ERROR;
    }
    return PMU_STATUS_SUCCESS;
}

void pmu_irq_count(int int_channel) {
    if ((int_channel >= 0) && (int_channel <= 15)) {
        event_con0 = 1;
    } else if ((int_channel >= 16) && (int_channel <= 31)) {
        event_con1 = 1;
    } else if ((int_channel >= 32) && (int_channel <= 39)) {
        event_con2 = 1;
    } else if ((int_channel >= 40) && (int_channel <= 43)) {
        event_con3 = 1;
    }
}

void pmu_irq_init(void) {
    if (pmu_init_flag == 0) {
        pmu_set_register_value_ab2568(PMU_INT_CON0, 0xffff, 0, 0);
        pmu_set_register_value_ab2568(PMU_INT_CON1, 0xffff, 0, 0);
        pmu_set_register_value_ab2568(PMU_INT_CON2, 0xffff, 0, 0);
        pmu_set_register_value_ab2568(PMU_INT_CON3, 0xffff, 0, 0);
        pmu_irq_enable_com0 = 0;
        pmu_irq_enable_com1 = 0;
        pmu_irq_enable_com2 = 0;
        pmu_irq_enable_com3 = 0;
    }
}

void pmu_scan_interrupt_status(void)
{
    uint8_t index = 0xFF;
    uint8_t value = 0;
    for(index = RG_INT_PWRKEY; index < PMU_INT_MAX; index++)
    {
        value = pmu_get_status_interrupt(index);
        if(value == 1)
        {
            if (pmu_function_table[index].isMask == false) {
                if (pmu_function_table[index].pmu_callback)
                    pmu_function_table[index].pmu_callback();
                //Clear Interrupt
                pmu_control_enable_interrupt(index, 0);
                pmu_control_enable_interrupt(index, 1);
            }
        }
    }
}

/*==========[PMIC Basic setting]==========*/
void pmu_init_2568() {
    pmu_set_register_value_ab2568(PMU_IOCFG3,PMU_RG_I2C_CLK_IC_MASK,PMU_RG_I2C_CLK_IC_SHIFT,0x4); //turn on smitt trigger for stability
    pmu_set_register_value_ab2568(PMU_IOCFG3,PMU_RG_I2C_DAT_IC_MASK,PMU_RG_I2C_DAT_IC_SHIFT,0x4); //turn on smitt trigger for stability
    pmu_set_register_value_ab2568(PMU_TEST_CON0,PMU_RG_MON_GRP_SEL_MASK,PMU_RG_MON_GRP_SEL_SHIFT,0x1f); //monitor flag group select
    pmu_set_register_value_ab2568(PMU_CKCFG2,PMU_RG_CLK_TRIM_F32K_CK_PDN_MASK,PMU_RG_CLK_TRIM_F32K_CK_PDN_SHIFT,0x1);
    pmu_set_register_value_ab2568(PMU_RSTCFG1,PMU_RG_WDTRSTB_EN_MASK,PMU_RG_WDTRSTB_EN_SHIFT,0x1);
    pmu_set_register_value_ab2568(PMU_RSTCFG1,PMU_RG_WDTRSTB_DEB_MASK,PMU_RG_WDTRSTB_DEB_SHIFT,0x1);
    pmu_set_register_value_ab2568(PMU_VAUD18DRV_MODE,PMU_RG_VAUD18DRV_VOSEL_HW_MODE_MASK,PMU_RG_VAUD18DRV_VOSEL_HW_MODE_SHIFT,0x1);
    pmu_set_register_value_ab2568(PMU_CAP_LPSD_CON1,PMU_RG_CAP_LPSD_SW_MODE_MASK,PMU_RG_CAP_LPSD_SW_MODE_SHIFT,0x0);
    pmu_set_register_value_ab2568(PMU_INT_CON0,PMU_RG_INT_EN_CHGSTATINT_MASK,PMU_RG_INT_EN_CHGSTATINT_SHIFT,0x1);
    pmu_set_register_value_ab2568(PMU_INT_MASK_CON0,PMU_RG_INT_MASK_PWRKEY_MASK,PMU_RG_INT_MASK_PWRKEY_SHIFT,0x0);
    pmu_set_register_value_ab2568(PMU_INT_MASK_CON0,PMU_RG_INT_MASK_PWRKEY_R_MASK,PMU_RG_INT_MASK_PWRKEY_R_SHIFT,0x0);
    pmu_set_register_value_ab2568(PMU_INT_MASK_CON0,PMU_RG_INT_MASK_CHRDET_MASK,PMU_RG_INT_MASK_CHRDET_SHIFT,0x0);
    pmu_set_register_value_ab2568(PMU_INT_MASK_CON1,PMU_RG_INT_MASK_AD_LBAT_LV_MASK,PMU_RG_INT_MASK_AD_LBAT_LV_SHIFT,0x0);
    pmu_set_register_value_ab2568(PMU_PSTSCTL2,PMU_RG_POFFSTS_EN_MASK,PMU_RG_POFFSTS_EN_SHIFT,0x3ff8);
    pmu_set_register_value_ab2568(PMU_TPO_CON1,PMU_RG_SRCLKEN_HW_MODE_MASK,PMU_RG_SRCLKEN_HW_MODE_SHIFT,0x1);
    /* Avoid system boot that by user short touch pk or evb layout issue */
    log_hal_msgid_info("Power off reason :%d", 1,pmu_get_power_off_reason());
    log_hal_msgid_info("Power on :%d", 1,pmu_get_power_on_reason());
    if (pmu_get_power_on_reason() == 0x1 && pmu_get_power_off_reason() !=0xd && pmu_get_power_off_reason() !=0x8) {
        pmu_press_pk_time();
    }else{
        pmu_set_register_value_ab2568(PMU_PWRHOLD,PMU_RG_PWRHOLD_MASK,PMU_RG_PWRHOLD_SHIFT,0x1);//Power hold
    }
    pmu_set_register_value_ab2568(PMU_STRUP_CON6,PMU_RG_STRUP_AUXADC_RPCNT_MAX_MASK,PMU_RG_STRUP_AUXADC_RPCNT_MAX_SHIFT,0x42);
    pmu_set_register_value_ab2568(PMU_STRUP_CON6,PMU_RG_STRUP_AUXADC_RSTB_SEL_MASK,PMU_RG_STRUP_AUXADC_RSTB_SEL_SHIFT,0x1);
    pmu_set_register_value_ab2568(PMU_LDO_VA18_OP_MODE, PMU_RG_LDO_VA18_ON_MODE_MASK, PMU_RG_LDO_VA18_ON_MODE_SHIFT, 0x1);
    pmu_set_register_value_ab2568(PMU_LDO_VLDO33_OP_MODE, PMU_RG_LDO_VLDO33_ON_MODE_MASK, PMU_RG_LDO_VLDO33_ON_MODE_SHIFT, 0x1);
    pmu_set_register_value_ab2568(PMU_LDO_VLDO33_OP_MODE, PMU_RG_LDO_VLDO33_LP_MODE_MASK, PMU_RG_LDO_VLDO33_LP_MODE_SHIFT, 0x1);
    pmu_set_register_value_ab2568(PMU_LDO_VLDO33_EN_CFG, PMU_RG_LDO_SLP_VLDO33_EN_MASK, PMU_RG_LDO_SLP_VLDO33_EN_SHIFT, 0x0);
    pmu_set_register_value_ab2568(PMU_LDO_VSRAM_OP_MODE, PMU_RG_LDO_VSRAM_LP_MODE_MASK, PMU_RG_LDO_VSRAM_LP_MODE_SHIFT, 0x1);
    pmu_set_register_value_ab2568(PMU_VCORE_SRAM_SW_CON0, PMU_RG_PSW_VCORE_SRAM_ON_MODE_MASK, PMU_RG_PSW_VCORE_SRAM_ON_MODE_SHIFT, 0x0);
    pmu_set_register_value_ab2568(PMU_BUCK_VCORE_OP_MODE, PMU_RG_BUCK_VCORE_LP_MODE_MASK, PMU_RG_BUCK_VCORE_LP_MODE_SHIFT, 0x1);
    pmu_set_register_value_ab2568(PMU_BUCK_VIO18_OP_MODE, PMU_RG_BUCK_VIO18_LP_MODE_MASK, PMU_RG_BUCK_VIO18_LP_MODE_SHIFT, 0x1);
    if (pmu_get_register_value_ab2568(PMU_CPSPSA4, PMU_RG_VRF_USA_MASK, PMU_RG_VRF_USA_SHIFT) == 0x14) {
        log_hal_msgid_info("VRF source form BUCK", 0);
        pmu_set_register_value_ab2568(PMU_BUCK_VRF_OP_MODE, PMU_RG_BUCK_VRF_ON_MODE_MASK, PMU_RG_BUCK_VRF_ON_MODE_SHIFT, 0x1);
        pmu_set_register_value_ab2568(PMU_BUCK_VRF_OP_MODE, PMU_RG_BUCK_VRF_LP_MODE_MASK, PMU_RG_BUCK_VRF_LP_MODE_SHIFT, 0x1);
        pmu_set_register_value_ab2568(PMU_BUCK_VRF_EN_CFG, PMU_RG_BUCK_SLP_VRF_EN_MASK, PMU_RG_BUCK_SLP_VRF_EN_SHIFT, 0x0);
    } else {
        log_hal_msgid_info("VRF source form LDO", 0);
        pmu_set_register_value_ab2568(PMU_LDO_VRF11_OP_MODE, PMU_RG_LDO_VRF11_ON_MODE_MASK, PMU_RG_LDO_VRF11_ON_MODE_SHIFT,0x1);
        pmu_set_register_value_ab2568(PMU_LDO_VRF11_OP_MODE, PMU_RG_LDO_VRF11_LP_MODE_MASK, PMU_RG_LDO_VRF11_LP_MODE_SHIFT,0x1);
        pmu_set_register_value_ab2568(PMU_LDO_VRF11_EN_CFG, PMU_RG_LDO_SLP_VRF11_EN_MASK, PMU_RG_LDO_SLP_VRF11_EN_SHIFT, 0x0);
    }
    pmu_set_register_value_ab2568(PMU_BUCK_TOP_CLK_HWEN_CON0, PMU_RG_BUCK_DCM_MODE_MASK, PMU_RG_BUCK_DCM_MODE_SHIFT, 0x1);
    pmu_set_register_value_ab2568(PMU_BUCK_VCORE_CON1, PMU_RG_BUCK_VCORE_VOSEL_SLEEP_MASK, PMU_RG_BUCK_VCORE_VOSEL_SLEEP_SHIFT, 0x25); /*Low power setting*/
    pmu_set_register_value_ab2568(PMU_VCORE_ANA_CON3, PMU_RG_VCORE_SLEEP_VOLTAGE_MASK, PMU_RG_VCORE_SLEEP_VOLTAGE_SHIFT, 0x2); /*Low power setting*/
    pmu_set_register_value_ab2568(PMU_VAUD18_ANA_CON2, PMU_RG_VAUD18_RSV_MASK, PMU_RG_VAUD18_RSV_SHIFT, 0x60);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON0, PMU_AUXADC_CK_AON_MASK, PMU_AUXADC_CK_AON_SHIFT, 0x0);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON3, PMU_AUXADC_SPL_NUM_LARGE_MASK, PMU_AUXADC_SPL_NUM_LARGE_SHIFT, 0x1F);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON5, PMU_AUXADC_SPL_NUM_SEL_MASK, PMU_AUXADC_SPL_NUM_SEL_SHIFT, 0x4);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON6, PMU_AUXADC_TRIM_CH2_SEL_MASK, PMU_AUXADC_TRIM_CH2_SEL_SHIFT, 0x2);
    pmu_set_register_value_ab2568(PMU_AUXADC_AD_CON6, PMU_AUXADC_TRIM_CH4_SEL_MASK, PMU_AUXADC_TRIM_CH4_SEL_SHIFT, 0x1);
    pmu_set_register_value_ab2568(PMU_CORE_CORE_ANA_CON9, PMU_RG_LOOP_CHRLDO_SB_DIS_MASK, PMU_RG_LOOP_CHRLDO_SB_DIS_SHIFT, 0x80);
    pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_STRUP_LONG_PRESS_EXT_RTCA_CTRL_MASK, PMU_RG_STRUP_LONG_PRESS_EXT_RTCA_CTRL_SHIFT, 1);
    pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_STRUP_LONG_PRESS_EXT_CHR_CTRL_MASK, PMU_RG_STRUP_LONG_PRESS_EXT_CHR_CTRL_SHIFT, 1);
    pmu_set_register_value_ab2568(PMU_RSTCFG3, PMU_RG_STRUP_LONG_PRESS_EXT_PWRKEY_CTRL_MASK, PMU_RG_STRUP_LONG_PRESS_EXT_PWRKEY_CTRL_SHIFT, 1);
    /*Set VBUS_PAM*/
    if(pmu_get_register_value_ab2568(PMU_HWCID, PMU_HWCID_MASK, PMU_HWCID_SHIFT) != 0x8820){
        pmu_set_register_value_ab2568(PMU_CORE_CORE_ANA_CON8, PMU_RG_BUSDPM_DELTA_VTH_MASK, PMU_RG_BUSDPM_DELTA_VTH_SHIFT, 0x0);
    }else{
        pmu_set_register_value_ab2568(PMU_CORE_CORE_ANA_CON8, PMU_RG_BUSDPM_DELTA_VTH_MASK, PMU_RG_BUSDPM_DELTA_VTH_SHIFT, 0x1);
    }
    pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x50); //Set vaud18 1.8V
    pmu_auxadc_init();
    pmu_basic_index = pmu_get_vcore_voltage_ab2568();
    if (pmu_get_register_value_ab2568(PMU_INT_STATUS0, PMU_RG_INT_STATUS_CHRDET_MASK,PMU_RG_INT_STATUS_CHRDET_SHIFT)) {
        pmu_charger_status =pmu_get_usb_input_status();
    }
    pmu_eint_init();
    pmu_long_press_shutdown_function_sel(PMU_DEBOUNCE_PWRKEY);
    pmu_scan_interrupt_status();
    pmu_irq_init();
    pmu_init_flag=1;
#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_PMU, (sleep_management_suspend_callback_t)hal_pmu_sleep_backup, NULL);
#endif
    pmu_safety_confirmation();
#ifdef HAL_KEEP_VAUD18_POWER
    pmu_enable_power_2568(PMU_BUCK_VAUD18,PMU_ON);
    vaud18_flag =1;
#endif
    /*lOW POWER SETTING*/
#ifdef PMU_LOW_POWER_SETTING
#ifdef PMU_LOW_POWER_VA18_SETTING
    pmu_enable_power_2568(PMU_LDO_VA18,PMU_OFF);          /* Default disable VA18*/
#endif
#ifdef PMU_LOW_POWER_VIO18_SETTING
    pmu_switch_control_mode(PMU_BUCK_VIO18,PMU_SW_MODE);  /* VIO18 Low power setting :1*/
    pmu_enable_sw_lp_mode(PMU_BUCK_VIO18,PMU_SW_MODE);    /* VIO18 Low power setting :2*/
#endif
#ifdef PMU_LOW_POWER_VRF_SETTING
    pmu_switch_control_mode(PMU_BUCK_VRF,PMU_SW_MODE);    /* VRF Low power setting :1*/
    pmu_enable_sw_lp_mode(PMU_BUCK_VRF,PMU_SW_MODE);      /* VRF Low power setting :2*/
#endif
    pmu_set_register_value_ab2568(PMU_BUCK_VRF_ELR0, PMU_RG_BUCK_VRF_VOSEL_MASK, PMU_RG_BUCK_VRF_VOSEL_SHIFT, 0x38);           /* Set VRF 1.05V */
#ifdef PMU_LOW_POWER_VAUD18_SETTING
    //pmu_set_register_value_ab2568(PMU_BUCK_VAUD18_ELR0, PMU_RG_BUCK_VCORE_VOSEL_MASK, PMU_RG_BUCK_VCORE_VOSEL_SHIFT, 0x47);    /* Set vaud18 1.71V */
#endif
    //pmu_set_register_value_ab2568(PMU_BUCK_VIO18_ELR0, PMU_RG_BUCK_VIO18_VOSEL_MASK, PMU_RG_BUCK_VIO18_VOSEL_SHIFT, 0x3b);     /* Set VIO18 1.71V */
#endif
#ifdef MTK_PMIC_AVDD50_OCP
    pmu_enable_ocp_2568(PMU_ON);
    log_hal_msgid_info("AVDD50 OCP enable", 0);
#else
    log_hal_msgid_info("AVDD50 OCP disable", 0);
#endif
}

uint32_t pmu_d2d_i2c_read(unsigned char *ptr_send, unsigned char *ptr_read, int type) {
    hal_i2c_send_to_receive_config_t config;
    unsigned char retry_cnt = 0, result_read;
    if (type == 1) {
        *(ptr_send) = *(ptr_send) | 0x40;
        config.receive_length = 1;
    } else {
        config.receive_length = 2;
    }
    config.slave_address = PMIC_SLAVE_ADDR;
    config.send_data = ptr_send;
    config.send_length = 2;
    config.receive_buffer = ptr_read;
    do {
        result_read = hal_i2c_master_send_to_receive_polling(HAL_I2C_MASTER_AO, &config);
        retry_cnt++;
        if(retry_cnt==59){
            log_hal_msgid_info("ERROR I2c access PMIC fail", 0);
            assert(0);
        }
    } while ((result_read != 0) && (retry_cnt <= 60));
    return (retry_cnt);
}

uint32_t pmu_get_register_value_ab2568(uint32_t address, uint32_t mask, uint32_t shift) {
    unsigned char send_buffer[4], receive_buffer[2];
    uint32_t value;
    pmic_i2c_init();
    send_buffer[1] = address & 0x00FF; //D2D 2Byte
    send_buffer[0] = ((address >> 8) & 0x00FF) & 0x0F;
    pmu_d2d_i2c_read(send_buffer, receive_buffer, 2);
    value = (receive_buffer[1] << 8) + receive_buffer[0];
    return ((value >> shift) & mask);
    return 0;
}

pmu_operate_status_t pmu_set_register_value_ab2568(uint32_t address, uint32_t mask, uint32_t shift, uint32_t value) {
    unsigned char send_buffer[4], receive_buffer[2];
    uint32_t data;
    pmic_i2c_init();
    send_buffer[1] = address & 0x00FF;
    send_buffer[0] = ((address >> 8) & 0x00FF) & 0x0F;
    pmu_d2d_i2c_read(send_buffer, receive_buffer, 2);

    data = receive_buffer[1];
    data = (data << 8) | receive_buffer[0];
    data &= (~(mask << shift));
    data = data | (value << shift);

    send_buffer[0] = ((address >> 8) & 0x00FF) | 0x00;
    send_buffer[1] = (address) & 0x00FF;
    send_buffer[2] = (data & 0xFF);
    send_buffer[3] = ((data >> 8) & 0xFF);

    unsigned char retry_cnt = 0, result_read;
    do {
        result_read = hal_i2c_master_send_polling(HAL_I2C_MASTER_AO, PMIC_SLAVE_ADDR, send_buffer, 4);
        retry_cnt++;
        if(retry_cnt==59){
            log_hal_msgid_info("ERROR I2c access PMIC fail", 0);
            assert(0);
        }
    } while ((result_read != 0) && (retry_cnt <= 60));
    return PMU_OK;
}

void pmic_i2c_init(void) {
    uint32_t mask_pri;
    hal_nvic_save_and_set_interrupt_mask_special(&mask_pri);
    if (pmu_i2c_init_sta == 1) {
        hal_nvic_restore_interrupt_mask_special(mask_pri);
        return;
    }
    int status;
    hal_i2c_config_t config;
#ifdef AB1555
    config.frequency =HAL_I2C_FREQUENCY_400K;
#else
    config.frequency = HAL_I2C_FREQUENCY_1M;
#endif
    status = hal_i2c_master_init(HAL_I2C_MASTER_AO, &config);
    if (status != HAL_I2C_STATUS_OK) {
        assert(0);
    }
    hal_i2c_master_set_io_config(HAL_I2C_MASTER_AO, HAL_I2C_IO_PUSH_PULL);
    pmu_i2c_init_sta = 1;
    hal_nvic_restore_interrupt_mask_special(mask_pri);
}

void pmic_i2c_deinit(void) {
    uint32_t mask_pri;
    hal_nvic_save_and_set_interrupt_mask_special(&mask_pri);
    hal_i2c_master_deinit(HAL_I2C_MASTER_AO);
    pmu_i2c_init_sta=0;
    hal_nvic_restore_interrupt_mask_special(mask_pri);
}

void hal_pmu_sleep_backup(void) {
    pmic_i2c_deinit();
}

void pmu_set_register_value_ddie(uint32_t address, short int mask, short int shift, short int value) {
    uint32_t mask_buffer,target_value;
    mask_buffer = (~(mask << shift));
    target_value = *((volatile uint32_t *)(address));
    target_value &= mask_buffer;
    target_value |= (value << shift);
    *((volatile uint32_t *)(address)) = target_value;
}

uint32_t pmu_get_register_value_ddie(uint32_t address, short int mask, short int shift) {
    uint32_t change_value, mask_buffer;
    mask_buffer = (mask << shift);
    change_value = *((volatile uint32_t *)(address));
    change_value &=mask_buffer;
    change_value = (change_value>> shift);
    return change_value;
}

void pmu_lock_va18_2568(int oper){
    uint32_t mask_pri;
    hal_nvic_save_and_set_interrupt_mask(&mask_pri);
    if(oper){
        va18_flag++;
        hal_nvic_restore_interrupt_mask(mask_pri);
    }else{
        if(va18_flag!=0){
            va18_flag--;
        }
        hal_nvic_restore_interrupt_mask(mask_pri);
    }
}



#endif /* HAL_PMU_MODULE_ENABLED */
