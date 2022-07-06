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

#include "task.h"
#include "task_def.h"
#include "hal_flash.h"
#include "hal_nvic_internal.h"
#include "hal_sleep_manager_internal.h"

#ifdef HAL_USB_MODULE_ENABLED
#include "hal_usb.h"
#include "hal_usb_internal.h"
#endif
#ifdef MTK_USB_DEMO_ENABLED
#include "usb.h"
#endif
#ifdef MTK_NVDM_ENABLE
#include "nvdm.h"
#endif
#ifdef AB1565
#include "hal_pmu_charger_2565.h"
#include "battery_management_sw_ntc.h"
#include "hal_pmu_cali_2565.h"
#endif
uint8_t battery_sleep_handle;                 /*Data : Battery management handle for lock sleep */
uint8_t gauge_cardinal_number;                /*Data : For gauge calculation*/
uint8_t battery_align_flag = 0;               /*flag : Avoid VBUS shaking, ensure the Irq data is consistent this time */
uint8_t battery_init_setting_flag = 0;        /*flag : Check battery init setting is finish or not*/
uint8_t battery_sw_ntc_feature = 0;           /*flag : SW ntc feature is enable or not*/
static uint32_t battery_callback_index = 0;   /*Data : Restore register callback function number */
TimerHandle_t xbm_chr_detec_t;                /*Timer : When PMIC irq"CHRDET" is Triggered, this timer will receive from pmic irq regiseter callbcck for processing*/
TimerHandle_t xbm_jeita_timer;                /*Timer : When With NTC and HW-JEITA is enable ,This timer will check JEITA status*/
TimerHandle_t xbm_eoc_timer;                  /*Timer : 1st enter the EOC will implement , to ensure that EOC verification can be performed without battery*/
TimerHandle_t xbm_option_setting_timer;       /*Timer : EOC setting will used, divided into multiple segments increase system resource allocation */
TimerHandle_t xbm_chrdet_calibration_timer;   /*Timer : Avoid unexpected problems caused by irq shake*/
TimerHandle_t xbm_chager_detect_timer;        /*Timer : one shot for charger detect*/

battery_basic_data bm_cust;                                                                      /*Data : restore battery basic data*/
battery_managerment_control_info_t bm_ctrl_info;                                                 /*Data : restore battery info*/
static bmt_callback_context_t bmt_callback[BATTERY_MANAGER_CALLBACK_FUNCTION_NUMBER];            /*Data : callback function*/
const battery_managerment_control_info_t *bm_ctrl_info_p;                                        /*Data : used for battery monitor callback function */
TaskHandle_t battery_regular_task_t = NULL;                                                      /*Task : create regular task for eco option 3 and gauge*/
TaskHandle_t battery_lineat_task_t = NULL;                                                       /*Task : create regular task for get_smooth_vbat*/
log_create_module(battery_management, PRINT_LEVEL_INFO);                                         /*Syslog create*/
extern hal_nvic_status_t hal_nvic_save_and_set_interrupt_mask_special(uint32_t *mask);
extern hal_nvic_status_t hal_nvic_restore_interrupt_mask_special(uint32_t mask);
extern pmu_status_t pmu_register_callback_2565(pmu_interrupt_index_2565_t pmu_int_ch, pmu_callback_t callback, void *user_data);

/*AB1565*/
extern PMU_CHG_INFO pmu_chg_info;
extern VBAT_VOLTAGE_CONFIG* p_vbat_volt;
/*==========[Battery management API]=========*/
int32_t battery_management_get_battery_property_internal(battery_property_t property)
{
    int32_t property_value=0;
    int get_temp = 0;
    uint8_t temp_ste_c=0;
    uint8_t temp_ste_n=0;
    switch (property)
    {
        case BATTERY_PROPERTY_CAPACITY:
            property_value = battery_get_gauge_percent();
            break;
        case BATTERY_PROPERTY_CAPACITY_LEVEL :
            property_value = (battery_get_gauge_percent()/10);
            break;
        case BATTERY_PROPERTY_CHARGER_EXIST:
            property_value = pmu_charger_is_plug();
            break;
        case BATTERY_PROPERTY_CHARGER_TYPE:
            property_value = bm_ctrl_info.chargerType;
            break;
        case BATTERY_PROPERTY_TEMPERATURE:
            if (battery_sw_ntc_feature == battery_sw_ntc_sucess) {
                battery_management_sw_ntc_retrive(&get_temp, &temp_ste_c, &temp_ste_n);
                property_value = get_temp;
            } else {
                property_value = 25;
                LOG_MSGID_I(battery_management, "SW NTC not enable\r\n", 0);
            }
            break;
        case BATTERY_PROPERTY_VOLTAGE:
            property_value = get_smooth_vbat();
            break;
        case BATTERY_PROPERTY_VOLTAGE_IN_PERCENT:
            property_value = battery_get_gauge_percent();
            break;
        case BATTERY_PROPERTY_PMIC_TEMPERATURE:
            property_value=-1;
            break;
        case BATTERY_PROPERTY_CHARGER_STATE:
            property_value = pmu_charger_get_chg_state();
            break;
        default:
            break;
    }
    LOG_MSGID_I(battery_management, "property_value : %d %d\r\n", 2,property,property_value);
    return property_value;
}

/*==========[Battery Management Callback Function]==========*/
void  battery_eoc_timer_callback(TimerHandle_t pxTimer) {
    LOG_MSGID_I(battery_management,"First times enter EOC state, stop timer",0);
    bm_ctrl_info.charger_eoc_state = 1;
    battery_core_pmu_charger_state_change_callback();
}
void battery_core_pmu_charger_state_change_callback(void) {
    uint32_t newState = pmu_charger_get_chg_state();
    switch (newState)
    {
        case CHARGER_STATE_CHR_OFF:
            break;
        case CHARGER_STATE_FASTCC_P:
        case CHARGER_STATE_FASTCC_B:
        case CHARGER_STATE_FASTCC:
        case CHARGER_STATE_FASTCC_V1:
        case CHARGER_STATE_FASTCC_V2:
            LOG_MSGID_I(battery_management, "FASTCC State\r\n", 0);
            break;

        case CHARGER_STATE_EOC:
            LOG_MSGID_I(battery_management, "EOC\r\n", 0);
#ifdef MTK_SMART_CHARGER_ENABLE
            LOG_MSGID_I(battery_management, "Samrt charger on ; BM EOC enter rtc off\r\n", 0);
#else
    #ifdef MTK_BATTERY_EOC_ENTER_RTC

            if (bm_ctrl_info.charger_eoc_state == 0) {
                LOG_MSGID_I(battery_management,"First times enter EOC state, start timer",0);
                xTimerStartFromISR(xbm_eoc_timer, 0);
                break;
            }
                hal_rtc_enter_rtc_mode();
    #endif
#endif
            break;

        case CHARGER_STATE_RECHG:
          LOG_MSGID_I(battery_management, "RECHG State\r\n", 0);
            break;
        default:
            break;
    }
    bm_ctrl_info.chargerState = newState;
    battery_notification(BATTERY_MANAGEMENT_EVENT_CHARGER_STATE_UPDATE,pmu_charger_is_plug(),newState);
}

battery_basic_data battery_management_get_basic_data(){
    return bm_cust;
}
void battery_set_charger_step_timer(uint8_t cur,uint8_t next){
    bm_ctrl_info.charger_step =next;
    LOG_MSGID_I(battery_management, "BM_CHARGER_STEP[%d]->[%d]",2,cur,bm_ctrl_info.charger_step);
    xTimerStopFromISR(xbm_chr_detec_t, 0);
    if (xTimerStartFromISR(xbm_chr_detec_t, 0) != pdPASS) {
        LOG_MSGID_I(battery_management, "xbm_chr_detec_t xTimerStart fail\n", 0);
    }
}
void battery_charger_setting(TimerHandle_t pxTimer) {
    switch (bm_ctrl_info.charger_step)
    {
        case BM_CHARGER_IN_BC12:
#ifdef BATTERY_FEATURE_BC1P2
            bm_ctrl_info.chargerType = pmu_get_bc12_charger_type();
#else
            bm_ctrl_info.chargerType = NON_STD_CHARGER;
#endif
            battery_set_charger_step_timer(BM_CHARGER_IN_BC12, BM_CHARGER_NOTIFICATION);
            break;
        case BM_CHARGER_NOTIFICATION:
    if(battery_sw_ntc_feature==battery_sw_ntc_sucess){
            battery_jetia_create_check_timer();
    }
#ifdef MTK_USB_DEMO_ENABLED
            usb_cable_detect();
#endif
#ifdef CHARGER_CALIBRATION
            xTimerStartFromISR(xbm_chrdet_calibration_timer, 0);
#endif
            bm_ctrl_info.charger_step = BM_CHARGER_DONE;
            xTimerStopFromISR(xbm_chr_detec_t, 0);
            LOG_MSGID_I(battery_management, "PMU sleep handle %d\n", 1,sleep_management_check_handle_status(battery_sleep_handle));
            battery_notification(BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE, pmu_charger_is_plug(), pmu_charger_get_chg_state());
            battery_core_pmu_charger_state_change_callback();
            break;
        default:
            LOG_MSGID_I(battery_management, "AB1565 charger step fail\n", 0);
            break;
    }
}

void battery_one_shot_detect_callback(TimerHandle_t pxTimer) {
    LOG_MSGID_I(battery_management, "battery_one_shot_detect_callback\r\n", 0);
    battery_notification(BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE, pmu_charger_is_plug(), pmu_charger_get_chg_state());
    battery_notification(BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE, pmu_charger_is_plug(), pmu_charger_get_chg_state());
}

/*==========[Battery Management init]==========*/
bmt_function_t battery_func = {
    battery_management_init_internal,
    battery_management_get_battery_property_internal,
    battery_management_register_callback_internal,
    NULL
};


void battery_charger_in_callback() {
    LOG_MSGID_I(battery_management, "Vbus on\r\n", 0);
    bm_ctrl_info.isChargerExist = pmu_charger_is_plug();
    bm_ctrl_info.charger_step = BM_CHARGER_IN_BC12;
    if (xTimerStartFromISR(xbm_chr_detec_t, 0) != pdPASS) {
        LOG_MSGID_I(battery_management, "xbm_chr_detec_t xTimerStart fail\n", 0);
    }

    if (HAL_NVIC_QUERY_EXCEPTION_NUMBER > HAL_NVIC_NOT_EXCEPTION) {
        xTaskResumeFromISR(battery_regular_task_t);
    }
    //battery_notification(BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE, pmu_charger_is_plug(), pmu_charger_get_chg_state());
}

void battery_charger_out_callback() {
    LOG_MSGID_I(battery_management, "Vbus off\r\n", 0);
    uint32_t mask_pri;
    hal_nvic_save_and_set_interrupt_mask_special(&mask_pri);
    bm_ctrl_info.isChargerExist = pmu_charger_is_plug();
    xTimerStopFromISR(xbm_chr_detec_t, 0);
   // xTimerStopFromISR(xbm_jeita_timer, 0);
    bm_ctrl_info.charger_step = BM_CHARGER_NOTIFICATION;
    if (xTimerStartFromISR(xbm_chr_detec_t, 0) != pdPASS) {
        hal_nvic_restore_interrupt_mask_special(mask_pri);
        LOG_MSGID_I(battery_management, "xbm_chr_detec_t xTimerStart fail\n", 0);
    }else{
        hal_nvic_restore_interrupt_mask_special(mask_pri);
    }


    //battery_notification(BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE, pmu_charger_is_plug(), pmu_charger_get_chg_state());
}

void battery_charger_compl_callback(){
    battery_core_pmu_charger_state_change_callback();
}
void battery_charger_rechg_callback(){
    battery_core_pmu_charger_state_change_callback();
}
void battery_monitor(battery_management_event_t event, const void *data)
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
    pmu_register_callback_2565(RG_INT_CHG_IN, (pmu_callback_t)battery_charger_in_callback, NULL);
    pmu_register_callback_2565(RG_INT_CHG_OUT, (pmu_callback_t)battery_charger_out_callback, NULL);
    pmu_register_callback_2565(RG_INT_CHG_COMPL, (pmu_callback_t)battery_charger_compl_callback, NULL);
    pmu_register_callback_2565(RG_INT_CHG_RECHG, (pmu_callback_t)battery_charger_rechg_callback, NULL);

    if (battery_management_register_callback(battery_monitor) != BATTERY_MANAGEMENT_STATUS_OK) {
        LOG_MSGID_I(battery_management,"Cannot register battery callback",0);
    }
}

void battery_charger_task(void * pvParameters) {

    const TickType_t xDelay = (CHARGER_REGULAR_TIME * TIMEOUT_PERIOD_1S) / portTICK_PERIOD_MS;
    while (1) {
        vTaskDelay( xDelay );
        LOG_MSGID_I(battery_management, "BM regular timer check\r\n", 0);
        LOG_W(MPLOG,"[BM]Battery temperature = %d(Celsius degrees)" ,(int)battery_management_get_battery_property(BATTERY_PROPERTY_TEMPERATURE));/*Log output by BT*/
        LOG_W(MPLOG,"[BM]Battery capacity = %d(%%) ",(int)battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY));/*Log output by BT*/
        LOG_W(MPLOG,"[BM]Charger status = %d ",(int)battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST));/*Log output by BT*/
        LOG_W(MPLOG,"[BM]Battery voltage = %d(mV)\n",(int) battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE));/*Log output by BT*/
        LOG_W(MPLOG,"[BM]Battery State = %d\n",(int) battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE));/*Log output by BT*/
    }
}

void battery_timer_init(void) {
    uint8_t intervale_time;
    xTaskCreate(battery_charger_task, "charger_task", 384, NULL, TASK_PRIORITY_NORMAL, &battery_regular_task_t);
#ifdef BATTERY_LINEAR_GAUGE
    xTaskCreate(battery_linear_gauge_task, "Linear_task", 384, NULL, TASK_PRIORITY_NORMAL, &battery_lineat_task_t);
#endif
    xbm_chr_detec_t = xTimerCreate("charger_regular_timer", 10*TIMEOUT_PERIOD_1MS, pdFALSE, NULL, battery_charger_setting);
    battery_sw_ntc_feature = battery_management_sw_ntc_init();
    if(battery_sw_ntc_feature==battery_sw_ntc_sucess){
        battery_management_sw_ntc_get_interval(&intervale_time);
        xbm_jeita_timer = xTimerCreate("jeita_Timer", (intervale_time * TIMEOUT_PERIOD_1S), pdTRUE, NULL, battery_jetia_timer_callback);
        if (xTimerStart(xbm_jeita_timer, 0) != pdPASS) {
            LOG_MSGID_I(battery_management, "xbm_jeita_Timer xTimerStart fail\n", 0);
        }
		bm_ctrl_info.jeita_state = sw_ntc_charger_normal;
    }else{
        bm_ctrl_info.jeita_state = sw_ntc_charger_normal;
    LOG_MSGID_I(battery_management, "battery_sw_ntc_feature fail %d \n", 1,battery_sw_ntc_feature);
    }
#ifdef CHARGER_CALIBRATION
    xbm_chrdet_calibration_timer = xTimerCreate("charger_detect_Timer", (CALIBRATION_TIME * portTICK_PERIOD_MS), pdFALSE, NULL, battery_detect_calibration_timer_callback);
#endif
    xbm_chager_detect_timer = xTimerCreate("charger_detect", (1 * TIMEOUT_PERIOD_1S), pdFALSE, NULL, battery_one_shot_detect_callback);
    xTimerStart(xbm_chager_detect_timer, 0);
    xbm_eoc_timer = xTimerCreate("charger_eoc_timer", (BATTERY_EOC_CHECK_TIME * TIMEOUT_PERIOD_1S), pdFALSE, NULL, battery_eoc_timer_callback);
}

battery_management_status_t battery_management_init_internal(void) {
    LOG_MSGID_I(battery_management,"Battery Management V3.27.0",0);
    battery_init_basic_data();
    battery_timer_init();
    battery_gauge_init();
#ifdef BATTERY_CHECK_BATTERY_VOLTAGE
    if((pmu_get_usb_input_status()!=true) &&
       (bm_cust.initial_bat  > battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE) ||
       bm_cust.shutdown_bat > battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE))){
        LOG_MSGID_I(battery_management, "Battery voltage low\r\n", 0);
        hal_rtc_enter_rtc_mode();
    }
#endif
    battery_management_interrupt_register();
    battery_init_check_charger_exist();
    LOG_MSGID_I(battery_management,"Battery Management init done",0);
    return BATTERY_MANAGEMENT_STATUS_OK;
}

battery_management_status_t battery_management_register_callback_internal(battery_management_callback_t callback)
{
    bmt_callback[battery_callback_index].func = callback;
    bmt_callback[battery_callback_index].callback_init = true;
    battery_callback_index++;
    return BATTERY_MANAGEMENT_STATUS_OK;
}

void battery_init_check_charger_exist(void) {
    bm_ctrl_info.isChargerExist = pmu_charger_is_plug();
    bm_ctrl_info.chargerState = pmu_charger_get_chg_state();
    if (bm_ctrl_info.isChargerExist) {
        battery_charger_in_callback();
        battery_core_pmu_charger_state_change_callback();
    }else{
        //vTaskSuspend(battery_regular_task_t);
        battery_notification(BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE, pmu_charger_is_plug(), pmu_charger_get_chg_state());
        battery_notification(BATTERY_MANAGEMENT_EVENT_BATTERY_UPDATE, pmu_charger_is_plug(), pmu_charger_get_chg_state());
    }
}

/*==========[Other : Basic internal function]==========*/
battery_management_status_t battery_init_basic_data() {
    bm_cust.s1_voltage = pmu_chg_info.cc1_thrd_volt;
    bm_cust.s1_chr_cur = pmu_chg_info.cc1_curr;
    bm_cust.s2_voltage = pmu_chg_info.cc2_thrd_volt;
    bm_cust.s2_chr_cur = pmu_chg_info.cc2_curr;
    bm_cust.full_bat = pmu_chg_info.full_bat_volt;
    bm_cust.recharger_voltage = pmu_chg_info.rechg_volt;
    bm_cust.initial_bat = p_vbat_volt->initial_bat;
    bm_cust.shutdown_bat = p_vbat_volt->shutdown_bat;
    LOG_MSGID_I(battery_management, "full_bat[%d]recharger_voltage[%d]s1_voltage[%d]s1_chr_cur[%d]s2_voltage[%d]s2_chr_cur[%d]", 6,
                                      bm_cust.full_bat,bm_cust.recharger_voltage,bm_cust.s1_voltage,bm_cust.s1_chr_cur,bm_cust.s2_voltage,bm_cust.s2_chr_cur);
    return BATTERY_MANAGEMENT_STATUS_OK;
}

void battery_notification(battery_management_event_t event,uint32_t chr_exist,uint32_t state) {
    int i = 0;
    LOG_MSGID_I(battery_management, "[BM Notification : %d][Chr_exist:%d][Chr_state:%d]\r\n",3,event,(int)chr_exist,(int)state);/*Log output by BT*/
    for (i = 0; i < battery_callback_index; i++) {
         if ((bmt_callback[i].callback_init == true) && (bmt_callback[i].func != NULL)) {
             bmt_callback[i].func(event, &bm_ctrl_info);
         }
    }
}
void battery_unlock_sleep(void) {
    uint8_t bm_lock=0;
    do {
        bm_lock = sleep_management_check_handle_status(battery_sleep_handle);
        if (sleep_management_check_handle_status(battery_sleep_handle) >= 1) {
            hal_sleep_manager_unlock_sleep(battery_sleep_handle);
        }
    } while (bm_lock >= 1);
    LOG_MSGID_I(battery_management, "battery unlock sleep: %d\n", 1,sleep_management_check_handle_status(battery_sleep_handle));
}

/*==========[Other : Additional features]==========*/
#ifdef CHARGER_CALIBRATION
/*This API is for fixing human actions or environment exceptions in the charger plug-in/plug-out*/
void battery_detect_calibration_timer_callback(TimerHandle_t pxTimer){
    uint32_t mask_pri;
    uint8_t temp_state=0;
    hal_nvic_save_and_set_interrupt_mask_special(&mask_pri);
    temp_state = pmu_charger_is_plug();
    if ((bm_ctrl_info.isChargerExist != temp_state)) {
        hal_nvic_restore_interrupt_mask_special(mask_pri);
        LOG_MSGID_I(battery_management, "ERROR!!!!, Plug-in/out is not staggered\r\n", 0);
        if(temp_state){
            battery_charger_in_callback();
        }else{
            battery_charger_out_callback();
        }
    }else{
        hal_nvic_restore_interrupt_mask_special(mask_pri);
    }

}
#endif
