/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#include "FreeRTOS.h"
#include "timers.h"

#include "smartcharger.h"

#include "hal_eint.h"
#include "hal_gpio.h"
#include "hal_log.h"
#include "chargersmartcase.h"
#include "battery_management.h"
#include "hal.h"
#include "hal_dvfs_internal.h"
#include "hal_sleep_manager_platform.h"
#include "battery_management_core.h"
#include "battery_management.h"

#define SEMI_CHARGERCASE_TEST        false
#define DRV_SMARTCHARGER_TEST_ENABLE false
#define DRV_SMARTCHARGER_DEBUG       false

////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
smartcharger_callback_t     gMSG_SmartChargerHandler;
HandlerData gSmartChargerHandler = { DRV_SmartCharger_MsgHandler };

TimerHandle_t pSmartChargerTimer;
TimerHandle_t pSmartChargerOutTimer;

uint8_t  v2_lastEvent;
uint8_t  v2_lastCmd;
uint16_t v2_lastData_len;
uint32_t v2_lastData;

uint16_t chargerPattern;
uint16_t chargerParameter;
uint8_t  lastPattern;
uint8_t  patternIndex;
uint8_t  parameterIndex;
uint8_t  smartChargerInit;
uint8_t  chargerOutCount;
uint8_t  recognizeEnd;
uint8_t  chargerInitState;
uint8_t  adcErrorCount;

#define DRV_SMART_CHARGER_PATTERN_LENGTH            5
#define DRV_SMART_CHARGER_ADC_THRESHOLD_HIGH        520
#define DRV_SMART_CHARGER_ADC_THRESHOLD_LOW         100

#define DRV_SMART_CHARGER_PATTERN_BT_ON             0x1D
#define DRV_SMART_CHARGER_PATTERN_BT_OFF            0x1B
#define DRV_SMART_CHARGER_PATTERN_CHARGER_COMPLETE  0xF
#define DRV_SMART_CHARGER_PATTERN_CHARGER_OUT       0
#define DRV_SMART_CHARGER_PATTERN_RESERVE0          0x18
#define DRV_SMART_CHARGER_PATTERN_RESERVE1          0x1C
#define DRV_SMART_CHARGER_PATTERN_RESERVE2          0x1A
#define DRV_SMART_CHARGER_PATTERN_RESERVE3          0x1E
#define DRV_SMART_CHARGER_PATTERN_RESERVE4          0x19

uint16_t const goldenPattern[] = {DRV_SMART_CHARGER_PATTERN_BT_ON,
                             DRV_SMART_CHARGER_PATTERN_BT_OFF,
                             DRV_SMART_CHARGER_PATTERN_CHARGER_COMPLETE,
                             DRV_SMART_CHARGER_PATTERN_CHARGER_OUT
#if(DRV_SMARTCHARGER_RESERVED_PATTERN_ENABLE)
                             ,
                             DRV_SMART_CHARGER_PATTERN_RESERVE0,
                             DRV_SMART_CHARGER_PATTERN_RESERVE1,
                             DRV_SMART_CHARGER_PATTERN_RESERVE2,
                             DRV_SMART_CHARGER_PATTERN_RESERVE3,
                             DRV_SMART_CHARGER_PATTERN_RESERVE4
#endif
                             };

#define DRV_SMART_CHARGER_V2_PATTERN_LENGTH            3
#define DRV_SMART_CHARGER_V2_PATTERN_MAX_LENGTH        16

#define DRV_SMART_CHARGER_V2_PATTERN_BT_ON             0x5
#define DRV_SMART_CHARGER_V2_PATTERN_BT_OFF            0x3
#define DRV_SMART_CHARGER_V2_PATTERN_CHARGER_COMPLETE  0x7
#define DRV_SMART_CHARGER_V2_PATTERN_CHARGER_OUT       0
#define DRV_SMART_CHARGER_V2_PATTERN_CASE_KEY          0x1
#define DRV_SMART_CHARGER_V2_PATTERN_USER_1            0x4
#define DRV_SMART_CHARGER_V2_PATTERN_USER_2            0x2
#define DRV_SMART_CHARGER_V2_PATTERN_USER_3            0x6

uint16_t const goldenPattern_V2_fast[] = {
                                DRV_SMART_CHARGER_V2_PATTERN_BT_ON,
                                DRV_SMART_CHARGER_V2_PATTERN_CHARGER_OUT
                                };

uint16_t const patternTransformV2_fast[] = {
                                DRV_CHARGER_EVENT_LID_OPEN_PATTERN,
                                DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN
                                };

uint16_t const goldenPattern_V2[] = {
                                DRV_SMART_CHARGER_V2_PATTERN_BT_ON,
                                DRV_SMART_CHARGER_V2_PATTERN_CHARGER_COMPLETE,
                                DRV_SMART_CHARGER_V2_PATTERN_BT_OFF,
                                DRV_SMART_CHARGER_V2_PATTERN_CASE_KEY,
                                DRV_SMART_CHARGER_V2_PATTERN_USER_1,
                                DRV_SMART_CHARGER_V2_PATTERN_USER_2,
                                DRV_SMART_CHARGER_V2_PATTERN_USER_3
                                  };

uint16_t const patternTransformV2[] = {
                                  DRV_CHARGER_EVENT_LID_OPEN_PATTERN,
                                  DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN,
                                  DRV_CHARGER_EVENT_LID_CLOSE_PATTERN,
                                  DRV_CHARGER_EVENT_CHARGER_KEY_PATTERN,
                                  DRV_CHARGER_EVENT_USER1_PATTERN,
                                  DRV_CHARGER_EVENT_USER2_PATTERN,
                                  DRV_CHARGER_EVENT_USER3_PATTERN
                                 };
////////////////////////////////////////////////////////////////////////////////
//  Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void DRV_SmartChargerV2_CallBack( TimerHandle_t pxExpiredTimer );

#define SMARTCHARGER_GPIO   HAL_GPIO_2

uint32_t last_count = 0;
uint8_t LOCK_DVFS_SLEEP = 0;

uint32_t SC_Pattern_Time = 0;
uint32_t CC_Pattern_Time = 0;
bool FirstPatternBitHigh = false;


void smart_charger_lock_dvfs(void )
{
    log_hal_msgid_info("[SmartCharger]DVFS LOCK[%d]\r\n", 1, LOCK_DVFS_SLEEP);

    if(LOCK_DVFS_SLEEP){

    }else{
        hal_sleep_manager_lock_sleep(SLEEP_LOCK_DMA);
        //dvfs_lock_control("SmartCharger",DVFS_156M_SPEED, DVFS_LOCK);
        LOCK_DVFS_SLEEP = 1;
    }
}

void smart_charger_unlock_dvfs(void )
{
    log_hal_msgid_info("[SmartCharger]DVFS UNLOCK[%d]\r\n", 1, LOCK_DVFS_SLEEP);

    if(LOCK_DVFS_SLEEP){
        hal_sleep_manager_unlock_sleep(SLEEP_LOCK_DMA);
        //dvfs_lock_control("SmartCharger",DVFS_156M_SPEED, DVFS_UNLOCK);
        LOCK_DVFS_SLEEP = 0;
    }
}
static void smart_charger_eint_hisr(void)
{
    last_count = 0;
    //chargerPattern     = 0;
    //patternIndex       = 0;
    FirstPatternBitHigh = FALSE;

    log_hal_msgid_info("[SmartCharger]INT Pattern[%d]", 1, chargerPattern);

    smart_charger_lock_dvfs();

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&last_count);

    log_hal_msgid_info("[SmartCharger]INT, GPT[%d],TICK[%d]", 2, last_count, xTaskGetTickCount());

    hal_eint_unmask(HAL_EINT_NUMBER_2);

    DRV_SmartCharger_Handler(CHARGER_UNPLUG_INT);
}


hal_smart_charger_status_t DRV_SmartCharger_Init_GPIO(hal_gpio_pin_t gpio_index)
{
    hal_eint_config_t eint_config;
    hal_eint_status_t result;

    log_hal_msgid_info("[SmartCharger]init gpio\r\n",0);

    hal_gpio_init(gpio_index);
    hal_pinmux_set_function(gpio_index, HAL_GPIO_2_EINT2);
    hal_gpio_set_direction(gpio_index, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_pull_up(gpio_index);

    /*eint configuration*/
    eint_config.trigger_mode = HAL_EINT_EDGE_FALLING_AND_RISING;
    eint_config.debounce_time = 35;

    hal_eint_mask(HAL_EINT_NUMBER_2);

    result = hal_eint_init(HAL_EINT_NUMBER_2, &eint_config);
    if (result != HAL_EINT_STATUS_OK) {
        log_hal_msgid_info("[SmartCharger]Eint init fail: %d\r\n",1, result);
        return HAL_SMART_CHARGER_STATUS_ERROR;
    }

    result = hal_eint_register_callback((hal_eint_number_t)HAL_EINT_NUMBER_2, (hal_eint_callback_t)smart_charger_eint_hisr, NULL);
    if (result != HAL_EINT_STATUS_OK) {
        log_hal_msgid_info("[SmartCharger]Eint register fail: %d\r\n",1, result);
        return HAL_SMART_CHARGER_STATUS_ERROR;
    }

    hal_eint_unmask(HAL_EINT_NUMBER_2);

    return HAL_SMART_CHARGER_STATUS_OK;
}

void DRV_SmartCharger_Status_Update()
{
    smartcharger_callback_t callback_handler;
    callback_handler = ChargerSmartCase_GetSmartCaseHandle();

    log_hal_msgid_info("[SmartCharger]DRV_SmartCharger_Status_Update GPIO[%d]", 0);

    if(lastPattern == DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN)
    {
        callback_handler(DRV_CHARGER_EVENT_CHARGER_IN_PATTERN, 0, 0);
    }
}

uint32_t DRV_SmartCharger_MsgHandler(Handler handler, uint16_t id, void *msg, uint32_t ext_id)
{
    UNUSED(handler);
    UNUSED(msg);
    UNUSED(ext_id);

    //DRV_SmartCharger_Notify(1 << (id - MSG_MESSAGE_CHARGER_BASE));
    return 0;
}

void DRV_SmartCharger_CallBack( TimerHandle_t pxExpiredTimer )
{
    UNUSED(pxExpiredTimer);

    uint32_t pattern;
    hal_gpio_data_t adcValue = HAL_GPIO_DATA_LOW;
    hal_gpio_status_t status;

    if(patternIndex == 0)
    {
        xTimerChangePeriod( pSmartChargerTimer, pdMS_TO_TICKS(50), 0);
    }

    status = hal_gpio_get_input(SMARTCHARGER_GPIO, &adcValue);
    if(status != HAL_GPIO_STATUS_OK){
        log_hal_msgid_info("[SmartCharger]CB1 Error", 0);
    }

    log_hal_msgid_info("[SmartCharger]ADC Value = %d index = %d tick[%d]", 3, adcValue, patternIndex, xTaskGetTickCount());

    if(adcValue > DRV_SMART_CHARGER_ADC_THRESHOLD_HIGH)
    {
        chargerPattern |= (1 << patternIndex);
    }
    else if (adcValue < DRV_SMART_CHARGER_ADC_THRESHOLD_LOW)
    {
        /* do nothing */
    }
    else
    {
        log_hal_msgid_info("[SmartCharger]Read GPIO error", 0);
    }

    patternIndex++;

    for(pattern = 0; pattern < sizeof(goldenPattern)/sizeof(goldenPattern[0]); pattern++)
    {
        if(chargerPattern == (goldenPattern[pattern] & ((1 << patternIndex) - 1)))
        {
            break;
        }
    }

    if(pattern == sizeof(goldenPattern)/sizeof(goldenPattern[0]))
    {
        /* error handle */
        log_hal_msgid_info("SmartCharger Pattern Recognize error, Pattern = 0x%x", 1, chargerPattern);

        patternIndex   = 0;
        chargerPattern = 0;
        xTimerStop(pSmartChargerTimer, 0);

        if(adcValue < DRV_SMART_CHARGER_ADC_THRESHOLD_HIGH)
        {
            /* maybe out of case , read  pattern again*/
#if (DRV_SMARTCHARGER_CASE_WORKAROUND_FOR_DELAY_350MS)
            log_hal_msgid_info("SmartCharger DRV_SmartCharger_Handler 350ms enter.", 0);
            DRV_SmartCharger_Handler(CHARGER_UNPLUG_INT);
#else
            DRV_SmartCharger_Notify(CHARGER_UNPLUG_INT);
#endif
        }
    }
    else if(patternIndex == DRV_SMART_CHARGER_PATTERN_LENGTH)
    {
        log_hal_msgid_info("SmartCharger Pattern = 0x%x", 1, chargerPattern);

        if(chargerPattern == DRV_SMART_CHARGER_PATTERN_BT_OFF || chargerPattern == DRV_SMART_CHARGER_PATTERN_CHARGER_OUT)
        {
            #if defined(AB1552)
            battery_chager_ignore_detect(BATTERY_OPERATE_OFF);
            #endif
            //dvfs_lock_control("Smart_Charger",DVFS_156M_SPEED, DVFS_UNLOCK);
        }

        if(chargerPattern == DRV_SMART_CHARGER_PATTERN_CHARGER_OUT || chargerPattern == DRV_SMART_CHARGER_PATTERN_CHARGER_COMPLETE)
        {
            xTimerStop(pSmartChargerOutTimer, 0);
        }

        patternIndex   = 0;
        chargerPattern = 0;

        xTimerStop(pSmartChargerTimer, 0);

        lastPattern = pattern + 1;
    }
}

void DRV_SmartCharger_preHandler(uint16_t type)
{
    chargerInitState = type;
}
#if 0
void DRV_SmartCharger_IntrHandler(uint16_t type)
{
    DRV_SmartChargerCase_preHandler();

    if(type & CHARGER_UNPLUG_INT)
    {
        DRV_SmartCharger_Handler(CHARGER_UNPLUG_INT);
    }

    if(type & CHARGER_PLUG_INT)
    {
        DRV_SmartCharger_Handler(CHARGER_PLUG_INT);
    }

    if(type& CHARGER_COMPLETE_INT)
    {
        DRV_SmartCharger_Handler(CHARGER_COMPLETE_INT);
    }

    if(type & CHARGER_RECHARGE)
    {
        DRV_SmartCharger_Handler(CHARGER_RECHARGE);
    }
}
#endif

#if (DRV_SMARTCHARGER_CASE_WORKAROUND_FOR_DELAY_350MS)
void DRV_SmartCharger_Handler(uint16_t type)
{
    BaseType_t xHigherPriorityTaskWoken;
    smartcharger_callback_t callback_handler;
    xHigherPriorityTaskWoken = pdFALSE;

    log_hal_msgid_info("[SmartCharger] Handler Type=%d lastPattern=0x%x patternIndex=%d", 3, type, lastPattern, patternIndex);

    if(type == CHARGER_UNPLUG_INT)
    {
        if(xTimerResetFromISR(pSmartChargerOutTimer, &xHigherPriorityTaskWoken) != pdPASS )
        {
            log_hal_msgid_info("[SmartCharger] ResetFromISR Error ", 0);
        }
    }
    else
    {
        if(xTimerStopFromISR(pSmartChargerOutTimer, &xHigherPriorityTaskWoken) != pdPASS )
        {
            log_hal_msgid_info("[SmartCharger] StopFromISR Error ", 0);
        }
        smart_charger_unlock_dvfs();
    }

    if(type == CHARGER_COMPLETE_INT)
    {
        if(lastPattern != DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN && lastPattern != DRV_CHARGER_EVENT_LID_CLOSE_PATTERN)
        {
            callback_handler = ChargerSmartCase_GetSmartCaseHandle();
            hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&SC_Pattern_Time);
            callback_handler(DRV_SMARTCHARGER_EVENT_CHARGER_COMPLETE_INTERRUPT, 0xFFFFFFFF, 32);
        }
    }
    else if(patternIndex == 0)
    {
        switch (type)
        {
            case CHARGER_UNPLUG_INT:
                chargerOutCount = 0;
                xTimerChangePeriodFromISR( pSmartChargerTimer, pdMS_TO_TICKS(20), 0);
                xTimerStartFromISR(pSmartChargerTimer, 0);
                break;
        }
    }

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}
#else
void DRV_SmartCharger_Handler(void* pMsg)
{
    CHARGER_IPC* chargerIPC = pMsg;

    if(chargerIPC->type == CHARGER_COMPLETE_INT)
    {
        if(lastPattern != DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN && lastPattern != DRV_CHARGER_EVENT_LID_CLOSE_PATTERN)
        {
            MSG_MessageSend(gMSG_SmartChargerHandler, DRV_SMARTCHARGER_EVENT_CHARGER_COMPLETE_INTERRUPT, NULL);
        }
    }
    else if(patternIndex == 0)
    {
        switch (chargerIPC->type)
        {
            case CHARGER_PLUG_INT:
                if(lastPattern == DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN)
                {
                    MSG_MessageSend(gMSG_SmartChargerHandler, DRV_SMARTCHARGER_EVENT_CHARGER_IN_INTERRUPT, NULL);
                }
                break;

            case CHARGER_UNPLUG_INT:
                xTimerChangePeriod( pSmartChargerTimer, pdMS_TO_TICKS(25), 0);
                xTimerStart(pSmartChargerTimer, 0);
                break;



            case CHARGER_RECHARGE:
                MSG_MessageSend(gMSG_SmartChargerHandler, DRV_SMARTCHARGER_EVENT_CHARGER_RECHARGE_INTERRUPT, NULL);
                break;

            default :
                OS_ASSERT(FALSE);
        }
    }
    OSMEM_Put(pMsg);
}

#endif

void DRV_SmartChargerV2_CallBack( TimerHandle_t pxExpiredTimer )
{
    UNUSED(pxExpiredTimer);

    //uint16_t adcValue = 0;
    uint32_t adcHigh = 0;
    hal_gpio_data_t smartcharger_gpio_data = HAL_GPIO_DATA_LOW;
    smartcharger_callback_t callback_handler;
    hal_gpio_status_t status;
    uint32_t compensation = 0;
    uint32_t current_count = 0;

#if DRV_SMARTCHARGER_DEBUG
    uint32_t callback_s = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&callback_s);
#endif
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&current_count);

    compensation =  ((current_count - last_count) * (31.25)) / 1024;

    log_hal_msgid_info("[SmartCharger]CB Index[%d],last[%d]ns,current[%d]ns,compen[%d]", 4, patternIndex, last_count, current_count, compensation);

    last_count = current_count;

    if(compensation <= 20)
    {
        xTimerChangePeriodFromISR( pSmartChargerTimer, pdMS_TO_TICKS((uint32_t)20), 0);
    }else if (compensation > 20){
        if((compensation < 30) && (compensation > 20)){
            xTimerChangePeriodFromISR( pSmartChargerTimer, pdMS_TO_TICKS((uint32_t)(20 - (compensation - 20))), 0);
        }else if(compensation >= 30){
            if(compensation == 30)
                xTimerChangePeriodFromISR( pSmartChargerTimer, pdMS_TO_TICKS((uint32_t)(20 - (compensation - 20))), 0);
            else{
                xTimerChangePeriodFromISR( pSmartChargerTimer, pdMS_TO_TICKS((uint32_t)(5)), 0);
                log_hal_msgid_info("[SmartCharger]CB Error[%d]ms", 1, compensation);
            }
        }
    }

    status = hal_gpio_get_input((hal_gpio_pin_t)SMARTCHARGER_GPIO, &smartcharger_gpio_data);
    if(status != HAL_GPIO_STATUS_OK){
        log_hal_msgid_info("[SmartCharger]CB Error read GPIO wrong", 0);
    }

    if(smartcharger_gpio_data == HAL_GPIO_DATA_LOW)
    {
        adcHigh = 1;
        chargerOutCount = 0;
    }
    else if (smartcharger_gpio_data == HAL_GPIO_DATA_HIGH)
    {
        /* do nothing */
        chargerOutCount ++;

        if(chargerOutCount == 4)
        {
            goto CHARGER_OUT;
        }
    }
    else
    {
        adcErrorCount++;
        log_hal_msgid_info("[SmartCharger]CB Error This pattern can't be regonized!!!!\r\n", 0);
        //assert(0);
    }

    log_hal_msgid_info("[SmartCharger]GPIO Index[%d], Value[%d]", 2, patternIndex, adcHigh);



    if(recognizeEnd)
    {
        uint32_t parameter = 0xFFFFFFFF;
        uint8_t  isNormal  = TRUE;

        log_hal_msgid_info("[SmartCharger]Pattern recognized\r\n", 0);

        for(uint32_t i = 0; i < sizeof(goldenPattern_V2)/sizeof(goldenPattern_V2[0]); i++)
        {
            if(chargerPattern == goldenPattern_V2[i])
            {
                lastPattern = patternTransformV2[i];
                break;
            }
        }

        if(adcHigh || lastPattern == DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN)
        {
            /* send parameter if there is parameter */
            if(parameterIndex)
            {
                /* last bit of parameter should be remove */
                parameter = chargerParameter >> 1;
            }
            log_hal_msgid_info("[SmartCharger]Pattern = 0x%x Parameter = 0x%x Length = %d, Param = %d", 4, chargerPattern, parameter, parameterIndex, chargerParameter);

            if(lastPattern == DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN)
            {
                /* check is normal complete pattern */
                if(parameterIndex == 6 && ((chargerParameter&7) == 7))
                {
                    log_hal_msgid_info("[SmartCharger]Normal\r\n", 0);
                    /* do nothing */
                    xTimerStop(pSmartChargerOutTimer, 0);
                }
                else
                {
                    log_hal_msgid_info("[SmartCharger]Not Normal\r\n", 0);
                }
            }

            if(isNormal)
            {
                log_hal_msgid_info("[SmartCharger]chargerOutCount isNormal CMPL", 0);
                callback_handler = ChargerSmartCase_GetSmartCaseHandle();
                if(parameter == 0xFFFFFFFF)
                    parameter = 0;
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&SC_Pattern_Time);
                callback_handler(lastPattern, parameter, parameterIndex);
                smart_charger_unlock_dvfs();
            }
        }
        else
        {
            log_hal_msgid_info("[SmartCharger]Pattern Recognize error", 0);
        }

        patternIndex     = 0;
        chargerPattern   = 0;
        chargerParameter = 0;
        parameterIndex   = 0;
        recognizeEnd     = 0;
        adcErrorCount    = 0;

        xTimerStop(pSmartChargerTimer, 0);

        /* maybe abnormal charger out */
        if(!adcHigh)
        {
            DRV_SmartCharger_Handler(CHARGER_UNPLUG_INT);
        }

        return;
    }

    if(patternIndex == 0 && adcHigh == 1){
        FirstPatternBitHigh = TRUE;
    }
    if(patternIndex == 3 && FirstPatternBitHigh){
        DRV_SmartCharger_Status_Update();
    }

    if(patternIndex < DRV_SMART_CHARGER_V2_PATTERN_LENGTH)
    {
        if(adcHigh)
        {
            chargerPattern |= (1 << patternIndex);
        }
    }

    patternIndex++;

    /* check opcode */
    if(patternIndex == DRV_SMART_CHARGER_V2_PATTERN_LENGTH)
    {
        uint32_t pattern;
        uint32_t parameter = 0;

        /* check pattern */
        for(uint32_t i = 0; i < sizeof(goldenPattern_V2_fast)/sizeof(goldenPattern_V2_fast[0]); i++)
        {
            if(chargerPattern == goldenPattern_V2_fast[i])
            {
                pattern     = patternTransformV2_fast[i];
                lastPattern = pattern;

                log_hal_msgid_info("[SmartCharger] FastDetect Pattern = 0x%x", 1, chargerPattern);

                callback_handler = ChargerSmartCase_GetSmartCaseHandle();

                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&SC_Pattern_Time);
                callback_handler(pattern, parameter, 0);

                if(lastPattern == DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN)
                {
                    goto CHARGER_OUT;
                }

                break;
            }
        }

    }
    /* check parameter */
    else if(DRV_SMART_CHARGER_V2_PATTERN_LENGTH < patternIndex && patternIndex <= DRV_SMART_CHARGER_V2_PATTERN_MAX_LENGTH)
    {
        if(patternIndex % 4 == 0)
        {
            log_hal_msgid_info("[SmartCharger] Check Parameter Four", 0);
            if(adcHigh)
            {
                /* no need to do anyting */
            }
            else
            {
                recognizeEnd  = 1;
            }
        }
        else
        {
            log_hal_msgid_info("[SmartCharger] Check Parameter Not Four", 0);

            chargerParameter <<= 1;
            if(adcHigh)
            {
                chargerParameter |= 1;
            }
            parameterIndex ++;
        }
    }
    else if ( DRV_SMART_CHARGER_V2_PATTERN_MAX_LENGTH < patternIndex )
    {
        patternIndex     = 0;
        chargerPattern   = 0;
        chargerParameter = 0;
        parameterIndex   = 0;
        adcErrorCount    = 0;

        xTimerStop(pSmartChargerTimer, 0);
        log_hal_msgid_info("[SmartCharger] pattern length small than patternIndex\r\n", 0);

        /* maybe abnormal charger out */
        if(!adcHigh)
        {
            log_hal_msgid_info("[SmartCharger] maybe abnormal charger out \r\n",0);

            DRV_SmartCharger_Handler(CHARGER_UNPLUG_INT);
        }
    }

#if DRV_SMARTCHARGER_DEBUG
    uint32_t callback_e = 0;
    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&callback_e);
    log_hal_msgid_info("[SmartCharger]CB Time[%d]\r\n",1,(callback_e - callback_s));
#endif

    return;

CHARGER_OUT:
    patternIndex     = 0;
    chargerPattern   = 0;
    chargerParameter = 0;
    parameterIndex   = 0;
    recognizeEnd     = 0;
    adcErrorCount    = 0;

    callback_handler = ChargerSmartCase_GetSmartCaseHandle();

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K,&SC_Pattern_Time);
    callback_handler(DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN, 0, 0);

    lastPattern = DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN;

    smart_charger_unlock_dvfs();
    xTimerStop(pSmartChargerTimer, 0);
    xTimerStop(pSmartChargerOutTimer, 0);


}


void DRV_SmartCharger_CallBack_Out(TimerHandle_t pxExpiredTimer)
{
    log_hal_msgid_info("[SmartCharger] Workaround timeout Expired", 0);
    DRV_SmartCharger_Handler(CHARGER_PLUG_INT);
}


void DRV_SmartCharger_Init(uint32_t initType, smartcharger_callback_t handler)
{
    LOCK_DVFS_SLEEP = 0;

    gMSG_SmartChargerHandler = handler;

    log_hal_msgid_info("[SmartCharger] DRV_SmartCharger_Init init start\r\n", 0);

    if(smartChargerInit == FALSE)
    {
        chargerPattern     = 0;
        patternIndex       = 0;
        lastPattern        = DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN;

        log_hal_msgid_info("[SmartCharger] InitType:%d Start\r\n", 1, initType);

        if( initType == 1 )
        {
            pSmartChargerTimer = xTimerCreate( "Smart Charger", pdMS_TO_TICKS(25), pdTRUE, NULL, DRV_SmartCharger_CallBack );
        }
        else if ( initType == 2 )
        {
            pSmartChargerTimer = xTimerCreate( "Smart Charger", pdMS_TO_TICKS(20), pdTRUE, NULL, DRV_SmartChargerV2_CallBack );

            if(!pSmartChargerTimer)
            {
                log_hal_msgid_info("Create Period Smart ChargerTimer Fail", 0);
            }
        }

        if(!pSmartChargerTimer)
        {
            log_hal_msgid_error("ASSERT\r\n", 0);
        }else{
            log_hal_msgid_info("[SmartCharger]Create Smart ChargerTimer Sucessfully", 0);
        }

        pSmartChargerOutTimer = xTimerCreate( "Smart Charger", pdMS_TO_TICKS(1000), pdTRUE, NULL, DRV_SmartCharger_CallBack_Out );
        if(!pSmartChargerOutTimer)
        {
            log_hal_msgid_info("[SmartCharger]Create Smart ChargerTimer Fail", 0);
            log_hal_msgid_error("ASSERT\r\n", 0);
        }

        if(chargerInitState & CHARGER_PLUG_INT)
        {
            DRV_SmartCharger_Handler(CHARGER_PLUG_INT);
        }

        if(chargerInitState & CHARGER_COMPLETE_INT)
        {
            DRV_SmartCharger_Handler(CHARGER_COMPLETE_INT);
        }

        if(chargerInitState & CHARGER_RECHARGE)
        {
            DRV_SmartCharger_Handler(CHARGER_RECHARGE);
        }

        if(chargerInitState & CHARGER_UNPLUG_INT)
        {
            DRV_SmartCharger_Handler(CHARGER_UNPLUG_INT);
        }

        chargerInitState = 0;
        smartChargerInit = TRUE;

        DRV_SmartCharger_Init_GPIO(SMARTCHARGER_GPIO);

    }
}

void DRV_SmartChargerCase_Init(smartcharger_callback_t handler)
{
    DRV_SmartCharger_Init(1 , handler);
}

void DRV_SmartChargerCaseV2_Init(smartcharger_callback_t handler)
{
    DRV_SmartCharger_Init(2 ,handler);
}

static void DRV_SmartChargerCaseV2_Send_to_App(uint8_t event, uint32_t data, uint16_t data_len)
{
    smartcharger_callback_t callback_handler;
    /* send cmd to app*/
    callback_handler = ChargerSmartCase_GetSmartCaseHandle();

    v2_lastCmd = event;

    if(event != 0)
    {
        if(callback_handler)
        {
            callback_handler(event, data, data_len);
        }
        else
        {
            v2_lastEvent     = event;
            v2_lastData_len  = data_len;
            v2_lastData      = data;
        }
    }
}


void DRV_SmartChargerCaseV2_preHandler(void)
{
    if(v2_lastEvent)
    {
        DRV_SmartChargerCaseV2_Send_to_App(v2_lastEvent , v2_lastData, v2_lastData_len);
        v2_lastEvent     = 0;
        v2_lastData_len  = 0;
        v2_lastData      = 0;
    }
}

