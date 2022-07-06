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

#ifndef __BATTERY_MANAGEMENT_CORE_H__
#define __BATTERY_MANAGEMENT_CORE_H__

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "queue.h"
#include "hal.h"
#include "battery_management.h"
#ifdef AB1568
#include "hal_pmu_ab2568_platform.h"
#include "hal_pmu_auxadc_2568.h"
#include "hal_pmu_charger_2568.h"
#include "hal_pmu_internal_2568.h"
#else
#include "hal_pmu_ab2565_platform.h"
#include "hal_pmu_auxadc_2565.h"
#endif
#include "battery_management_interface.h"
#include "battery_management_gauge.h"
#include "battery_management_auxadc.h"
#include "battery_management_customer_file.h"
#include "battery_management_HW_JEITA.h"
#include "battery_management_charger_api.h"

/*==========[Data : Battery management behavior parameter]==========*/
#define TIMEOUT_PERIOD_1S 1000/portTICK_PERIOD_MS                    /*Data : 1s */
#define TIMEOUT_PERIOD_1MS portTICK_PERIOD_MS                        /*Data : 1ms */
#define BATTERY_MANAGER_CALLBACK_FUNCTION_NUMBER 8                   /*Data : Max index number for register callback number*/
#define BATTERY_FULLBAT_INDEX_MAX 12                                 /*Data : Max index number for full battery voltage*/
#define BATTERY_RECHARGER_INDEX_MAX 4                                /*Data : Max index number for re-charger voltage*/
#define HW_JEITA_CHECK_INTERVAL_TIME 1                               /*Data : Set jeita callback function interval time*/
#define BMT_EVENT_INIT              0                                /*Data : for battery management task default value*/
#define BMT_EVENT_10_TIMEOUT           1                             /*Data : for battery management task value*/

/*==========[Struct]==========*/
typedef struct {
    uint16_t cold;
    uint16_t cool;
    uint16_t warm;
    uint16_t hot;
}battery_jeita_data;

typedef struct {
    uint16_t full_bat;
    uint16_t full_bat_offset;
    uint16_t recharger_voltage;
    uint8_t recharger_threshold;
    uint16_t initial_bat;
    uint16_t shutdown_bat;
    int check_point[9];
    int resist_offset[9];
    battery_jeita_data jeita;
    uint16_t precc_cur;
    uint16_t cv_termination;
    uint16_t s0_voltage_i;
    uint16_t s0_chr_cur;
    uint16_t s1_voltage;
    uint16_t s1_chr_cur;
    uint16_t s2_voltage;
    uint16_t s2_chr_cur;
    uint16_t cool_cc;
    uint16_t cool_cv;
    uint16_t warm_cc;
    uint16_t warm_cv;
    uint16_t iterm_irq;
}battery_basic_data;

typedef struct bmt_callback_context_t {
    bool callback_init;
    battery_management_callback_t func;
} bmt_callback_context_t;

typedef struct {
    TaskHandle_t task_handle;
    uint32_t event;
    QueueHandle_t bmt_queue_handle;
} battery_managerment_message_queue_t;

typedef struct {
    bool feature_jeita;
    bool message_task;
    uint8_t charger_option;
    bool charger_init;
} battery_charger_feature_t;

typedef enum {
    BM_CHARGING_NOT_INIT = 0,
    BM_CHARGING_START = 1,
    BM_CHARGING_STAGE = 2,
    BM_DISCHARGER_NOT_INIT = 3,
    BM_DISCHARGER_START = 4,
    BM_DISCHARGER_STAGE = 5,
} battery_linear_gauge_state_t;

typedef struct {
    uint32_t chargerState;
    bool isChargerExist;  //1:true; 0:false
    battery_managerment_message_queue_t message;
    battery_charger_feature_t feature;
    uint8_t jeita_state;
    uint8_t icl_curent;
    uint32_t jeita_Voltage;
    uint8_t chargerType;
    uint8_t gauge_calibration;
    int8_t charger_eoc_state;
    uint8_t charger_step;
} battery_managerment_control_info_t;

/*==========[enum ]==========*/
typedef enum {
    BATTERY_OPERATE_OFF = 0,
    BATTERY_OPERATE_ON = 1,
} battery_managerment_operate_t;

typedef enum {
    FEATURE_OFF,
    FEATURE_ON,
} battery_managerment_feature_oper_t;


typedef enum {
    ITERM_TIME_SETTING_DISABLE = 0, // enter EOC directly
    ITERM_TIME_SETTING_15MINS = 1,
    ITERM_TIME_SETTING_30MINS = 2,
    ITERM_TIME_SETTING_60MINS = 3,
} battery_extend_time_t;
#if defined(AB1568)
typedef enum {
    CHARGER_STATE_CHR_OFF = 0,
    CHARGER_STATE_PRECC = 1,
    CHARGER_STATE_FASTCC = 2,
    CHARGER_STATE_EXTENSION = 3,
    CHARGER_STATE_EOC = 4,
    CHARGER_STATE_THR = 5,
    CHARGER_STATE_VBAT_OVP = 6,
    CHARGER_STATE_SAFETY_TIMER_TIMEOUT = 7,
} battery_managerment_charger_state_t;
#else
typedef enum {
    CHARGER_STATE_CHR_OFF = 0,
    CHARGER_STATE_FASTCC_P = 1,
    CHARGER_STATE_FASTCC_B = 2,
    CHARGER_STATE_FASTCC   = 3,
    CHARGER_STATE_FASTCC_V1 = 4,
    CHARGER_STATE_FASTCC_V2 = 5,
    CHARGER_STATE_EOC = 6,
    CHARGER_STATE_RECHG = 7,
    CHARGER_STATE_THR = 8,
} battery_managerment_charger_state_t;
#endif
typedef enum {
    RECHARGER_MARK_50 = 0,
    RECHARGER_MARK_100 = 1,//default
    RECHARGER_MARK_150 = 2,
    RECHARGER_MARK_200 = 3,
} battery_managerment_recharger_voltage_t;

typedef enum {
    BM_CHARGER_IN_CHECK_POWER=0,
    BM_CHARGER_IN_JEITA_INIT=1,
    BM_CHARGER_IN_JEITA_WC=2,
    BM_CHARGER_IN_USB=3,
    BM_CHARGER_IN_PLUGIN_INIT=4,
    BM_CHARGER_IN_BC12=5,
    BM_CHARGER_IN_ENABLE=6,
    BM_CHARGER_IN_GAUGE_CALI=7,
    BM_CHARGER_OUT_CHECK_POWER=10,
    BM_CHARGER_OUT_JEITA_OFF=11,
    BM_CHARGER_OUT_EOC_EXIT=12,
    BM_CHARGER_OUT_GAUGE_CALI=13,
    BM_CHARGER_NOTIFICATION=20,
    BM_CHARGER_DONE=21,
} battery_managerment_charger_detect_step;

typedef void(* battery_management_callback_t)(battery_management_event_t event, const void *data);

/*==========[Battery management API]=========*/
int32_t battery_management_get_battery_property_internal(battery_property_t property);
void battery_switch_charger_option(int option);
void battery_enable_charger(battery_managerment_operate_t oper);
battery_basic_data battery_management_get_basic_data(void);
/*==========[Battery Management Callback Function]==========*/
void battery_core_pmu_charger_state_change_callback(void);
void battery_charger_setting(TimerHandle_t pxTimer);
void battery_charger_detect_callback(void);
void battery_monitor(battery_management_event_t event, const void *data);
void battery_core_pmu_rechg_callback(void);
void battery_eoc_timer_callback(TimerHandle_t pxTimer);
void battery_core_pmu_thm_over110_callback(void);
/*==========[Battery Management init]==========*/
void battery_timer_init(void);
void battery_parameter_init(void);
battery_management_status_t battery_management_init_internal(void);
void battery_message_task_init(void);
void battery_iterm_callback(void);
void battery_iterm_current_callback(void);
void battery_management_interrupt_register(void);
void battery_init_check_charger_exist(void);
/*==========[Other : Basic internal function]==========*/
battery_management_status_t battery_init_date_form_nvdm(void);
battery_management_status_t battery_init_basic_data(void);
void battery_notification(battery_management_event_t event,uint32_t chr_exist,uint32_t state);
uint8_t battery_get_full_battery_index(uint16_t vabt);
uint8_t battery_get_recharger_index(uint16_t vol);
void battery_charger_plugin_initial_setting(void);
battery_management_status_t battery_management_register_callback_internal(battery_management_callback_t callback);
void battery_eoc_option_setting(TimerHandle_t pxTimer);
void battery_unlock_sleep(void);
void battery_set_charger_step(void);
void battery_set_charger_step_timer(uint8_t cur,uint8_t next);
void battery_charger_task(void * pvParameters);
void battery_one_shot_detect_callback(TimerHandle_t pxTimer);
/*==========[Other : Additional features]==========*/
void battery_basic_ovp_debug(void);
void battery_detect_calibration_timer_callback(TimerHandle_t pxTimer);
uint32_t battery_management_set_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value);
uint32_t battery_management_get_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift);
#endif
