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
#include "hal_pmu_mt6388_platform.h"
#include "hal_pmu_platform.h"
#include "hal_pmu_auxadc.h"

#include "battery_management_interface.h"
#include "battery_management_charger_mt6388_api.h"
#include "battery_power_saving.h"
#include "battery_management_gauge.h"
#include "battery_management_auxadc.h"
#include "battery_management_bc12.h"
#include "battery_management_charger_api.h"
#include "battery_management_customer_file.h"
#include "battery_management_HW_JEITA.h"
#include "battery_management_BJT_charging.h"
#include "battery_management_efuse.h"

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
    battery_jeita_data jeita;
    uint16_t precc_cur;
    uint16_t cv_termination;
    uint16_t s1_voltage;
    uint16_t s1_multi_c;
    uint16_t s2_voltage;
    uint16_t s2_multi_c;
    uint16_t cool_cc;
    uint16_t cool_cv;
    uint16_t warm_cc;
    uint16_t warm_cv;
}battery_basic_data;

#define BATTERY_MANAGER_DEBUG
#define BMT_EVENT_INIT              0
#define BMT_EVENT_10_TIMEOUT           1
#define TIMEOUT_PERIOD_1S 1000/portTICK_PERIOD_MS
#define TIMEOUT_PERIOD_1MS portTICK_PERIOD_MS

//-------Feature option
#define BATTERY_MESSAGE_QUEUE_TASK FEATURE_OFF
#define BATTERY_MANAGER_JEITA_ENABLE FEATURE_ON
#define BATTERY_MANAGER_BJT_ENABLE FEATURE_OFF
#ifdef MTK_SMART_CHARGER_ENABLE
#define BATTERY_MANAGER_DEFAULT_CHARGER_OPTION 1 /*If smart charger case enable need choose option 1*/
#else
#define BATTERY_MANAGER_DEFAULT_CHARGER_OPTION 3 /*If smart charger case enable need choose option 1*/
#endif
#define BATTERY_MANAGER_CALLBACK_FUNCTION_NUMBER 8
#define BATTERY_FULLBAT_INDEX_MAX 12
#define BATTERY_RECHARGER_INDEX_MAX 4

#define BOOT_UP_BUT_DISABLE_CHARGE_BATTERY      0x01
#define POWER_SAVING_EOC_OPTION2                0x02
#define HW_JEITA_DEFAULT_VALUE                  0x04
#define POWER_SAVING_EOC_OPTION3                0x08


#define HW_JEITA_HOT_THRESHOLD  405   //45C 10k ohm
#define HW_JEITA_WARM_THRESHOLD 438   //42C 10k ohm
#define HW_JEITA_COOL_THRESHOLD 1055  //3C  10k ohm
#define HW_JEITA_COLD_THRESHOLD 1110  //0C  10k ohm

#define BATTERY_PRECC_CURRENT BM_5P
#define BATTERY_PRECC_CV_TERM BM_5P
#define BATTERY_S1_VOLTAGE 3700
#define BATTERY_S1_MULTI_C ICC_MULTIPLE_ITH_1C
#define BATTERY_S2_VOLTAGE 3900
#define BATTERY_S2_MULTI_C ICC_MULTIPLE_ITH_1C

#define BATTERY_COOL_CC RG_ICC_JC_20
#define BATTERY_COOL_CV ICC_MULTIPLE_ITH_1C
#define BATTERY_WARM_CC RG_ICC_JC_100
#define BATTERY_WARM_CV RG_VCV_VOLTAGE_4P05V
#define BATTERY_ENABLE_DIGITAL_THERMAL
#define HW_JEITA_CHECK_INTERVAL_TIME 1

typedef struct bmt_callback_context_t {
    bool callback_init;
    battery_management_callback_t func;
} bmt_callback_context_t;

typedef enum {
    BATTERY_OPERATE_OFF = 0,
    BATTERY_OPERATE_ON = 1,
} battery_managerment_operate_t;

typedef enum {
    FEATURE_OFF,
    FEATURE_ON,
} battery_managerment_feature_oper_t;

typedef enum {
    ICC_MULTIPLE_ITH_1C = 0,
    ICC_MULTIPLE_ITH_2C = 1,
    ICC_MULTIPLE_ITH_3C = 2,
    ICC_MULTIPLE_ITH_4C = 3,
    ICC_MULTIPLE_ITH_5C = 4,
} battery_managerment_charger_ITH_t;

typedef enum {
    ITERM_TIME_SETTING_DISABLE = 0, // enter EOC directly
    ITERM_TIME_SETTING_15MINS = 1,
    ITERM_TIME_SETTING_30MINS = 2,
    ITERM_TIME_SETTING_60MINS = 3,
} battery_extend_time_t;

typedef enum {
    ICL_ITH_10mA = 0,     //  10   mA
    ICL_ITH_90mA = 1,     //  90   mA
    ICL_ITH_200mA = 2,    //  200  mA
    ICL_ITH_300mA = 3,    //  300  mA
    ICL_ITH_400mA = 4,    //  400  mA
    ICL_ITH_500mA = 5,    //  500  mA
    ICL_ITH_600mA = 6,    //  600  mA
    ICL_ITH_700mA = 7,    //  700  mA
    ICL_ITH_800mA = 8,    //  800  mA
    ICL_ITH_900mA = 9,    //  900  mA
    ICL_ITH_1000mA = 10,  //  1000 mA
} battery_managerment_icl_level_t;

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

typedef enum {
    RECHARGER_MARK_50 = 0,
    RECHARGER_MARK_100 = 1,//default
    RECHARGER_MARK_150 = 2,
    RECHARGER_MARK_200 = 3,
} battery_managerment_recharger_voltage_t;

typedef enum {
    BM_5P = 0,
    BM_10P = 1,
    BM_20P = 2,
    BM_40P = 3,
} battery_managerment_percet1_t;


typedef struct {
    TaskHandle_t task_handle;
    uint32_t event;
    QueueHandle_t bmt_queue_handle;
} battery_managerment_message_queue_t;

typedef struct {
    bool feature_jeita;
    bool feature_bjt;
    bool message_task;
    uint8_t charger_option;
    bool charger_init;
} battery_charger_feature_t;

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

typedef struct {
    uint32_t chargerState;
    bool isChargerExist;  //1:true; 0:false
    battery_managerment_message_queue_t message;
    battery_charger_feature_t feature;
    bool bjt_isCharger;
    uint8_t jeita_state;
    uint8_t icl_curent;
    uint32_t jeita_Voltage;
    uint8_t chargerType;
    uint8_t charger_5c;
    uint8_t gauge_calibration;
    int8_t charger_eoc_state;
    uint8_t charger_step;
} battery_managerment_control_info_t;
battery_management_status_t battery_init_basic_data(void);
battery_management_status_t battery_init_date_form_nvdm(void);
void battery_management_trim_ic_init(void);
void battery_management_task(void);
void battery_management_interrupt_register(void);
int32_t battery_management_get_battery_property(battery_property_t property);
battery_management_status_t battery_management_init(void);
typedef void(* battery_management_callback_t)(battery_management_event_t event, const void *data);
void battery_management_enable_charger(battery_managerment_operate_t oper);
void battery_management_notification(battery_management_event_t event,uint32_t chr_exist,uint32_t state);
bool battery_set_en_recharge(bool isEnableRecharge);
uint32_t battery_management_set_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift, unsigned short int value);
uint32_t battery_management_get_register_value(unsigned short int address, unsigned short int mask, unsigned short int shift);
battery_basic_data battery_management_get_basic_data(void);
void battery_switch_charger_option(int option);
void battery_management_debug_info(void);
battery_management_status_t battery_management_register_callback(battery_management_callback_t callback);
void battery_chager_ignore_detect(battery_managerment_operate_t sta);
uint8_t battery_get_full_battery_index(uint16_t vabt);
#endif




