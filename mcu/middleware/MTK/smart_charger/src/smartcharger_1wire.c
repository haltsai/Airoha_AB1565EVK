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

#include "hal_uart_internal.h"
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
#include "memory_attribute.h"
#include "bt_sink_srv_ami.h"
#if defined(AB1565)
#include "hal_pmu_charger_2565.h"
#endif
#include "serial_port.h"
#include "race_xport.h"

#include "nvkey.h"
#include "nvkey_id_list.h"

#define BIT_FIELD_CLEAR16(r,p,l)    (((U16)(r)) & ~((((U16)1 << (l)) - 1) << (p)))
#define BIT_FIELD_CLEAR32(r,p,l)    (((U32)(r)) & ~((((U32)1 << (l)) - 1) << (p)))
#define BIT_FIELD_EXTRACT8(r,p,l)   (((U8)(r)  >> (p)) & (((U8)1  << (l)) - 1))
#define BIT_FIELD_EXTRACT16(r,p,l)  (((U16)(r) >> (p)) & (((U16)1 << (l)) - 1))
#define BIT_FIELD_EXTRACT32(r,p,l)  (((U32)(r) >> (p)) & (((U32)1 << (l)) - 1))
#define BIT_FIELD_INSERT8(r,p,l,v)  (BIT_FIELD_CLEAR8((r),(p),(l))  | ((U8)(v)  << (p)))
#define BIT_FIELD_INSERT16(r,p,l,v) (BIT_FIELD_CLEAR16((r),(p),(l)) | ((U16)(v) << (p)))
#define BIT_FIELD_INSERT32(r,p,l,v) (BIT_FIELD_CLEAR32((r),(p),(l)) | ((U32)(v) << (p)))


////////////////////////////////////////////////////////////////////////////////
// Global Variables ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
#define SMCHG_1WIRE_UART_BUFF_SIZE 32
ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static uint8_t oneWire_tx[SMCHG_1WIRE_UART_BUFF_SIZE];
ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static uint8_t oneWire_rx[SMCHG_1WIRE_UART_BUFF_SIZE];

uint8_t pre_cmd;
uint32_t pre_data;
uint16_t pre_data_len;
uint8_t last_cmd;
uint8_t boot_Flag;
uint8_t isTimerStart;
uint8_t race_mode_flag = 0;
uint8_t is1WireLoggingMode;
uint8_t isLogging[5];

ATTR_RWDATA_IN_TCM static uint8_t chk_cnt = 0;
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
extern battery_managerment_control_info_t bm_ctrl_info;
#endif

#if (PRODUCT_VERSION == 2822 || defined(AB1568))
#define CHARGER_CASE_1WIRE_1568
#elif defined(AB1565)
#define CHARGER_CASE_1WIRE_1565
#else
#define CHARGER_CASE_1WIRE_155x
#endif

#ifdef CHARGER_CASE_1WIRE_1568 //1568
#ifdef MTK_SMART_CHARGER_1WIRE_SAMPLE
#define SMCHG_1WIRE_COM_MODE_PIN    HAL_GPIO_10
#elif defined(MTK_SMART_CHARGER_1WIRE_EVK)
#define SMCHG_1WIRE_COM_MODE_PIN    HAL_GPIO_6
#else
#define SMCHG_1WIRE_COM_MODE_PIN    HAL_GPIO_3
#endif
#define SMCHG_1WIRE_TX_PIN          HAL_GPIO_4
#define SMCHG_1WIRE_RX_PIN          HAL_GPIO_5
#define SMCHG_1WIRE_TX_GPIO_MODE    HAL_GPIO_4_GPIO4
#define SMCHG_1WIRE_TX_UART_MODE    HAL_GPIO_4_UART1_TXD
#define SMCHG_1WIRE_RX_GPIO_MODE    HAL_GPIO_5_GPIO5
#define SMCHG_1WIRE_RX_UART_MODE    HAL_GPIO_5_UART1_RXD
#define OUT_OF_CASE_THRD            1000

#elif defined(CHARGER_CASE_1WIRE_1565) //1565
#define SMCHG_1WIRE_TX_PIN          HAL_GPIO_4
#define SMCHG_1WIRE_RX_PIN          HAL_GPIO_5
#define SMCHG_1WIRE_TX_GPIO_MODE    HAL_GPIO_4_GPIO4
#define SMCHG_1WIRE_TX_UART_MODE    HAL_GPIO_4_UART1_TXD
#define SMCHG_1WIRE_RX_GPIO_MODE    HAL_GPIO_5_GPIO5
#define SMCHG_1WIRE_RX_UART_MODE    HAL_GPIO_5_UART1_RXD
#define OUT_OF_CASE_THRD            140
#define VBAT_THRD                   3200

#else //155x
#define SMCHG_1WIRE_COM_MODE_PIN    HAL_GPIO_1
#define SMCHG_1WIRE_TX_PIN          HAL_GPIO_10
#define SMCHG_1WIRE_RX_PIN          HAL_GPIO_9
#define SMCHG_1WIRE_TX_GPIO_MODE    HAL_GPIO_10_GPIO10
#define SMCHG_1WIRE_TX_UART_MODE    HAL_GPIO_10_UART1_TXD
#define SMCHG_1WIRE_RX_GPIO_MODE    HAL_GPIO_9_GPIO9
#define SMCHG_1WIRE_RX_UART_MODE    HAL_GPIO_9_UART1_RXD
#define OUT_OF_CASE_THRD            500
#endif

extern TimerHandle_t pSmartChargerTimer;
extern TimerHandle_t pSmartChargerOutTimer;
extern uint8_t LOCK_DVFS_SLEEP;

static uint8_t raceEvt[] = {
    0x05, //race header
    0x5D, //race type
    0x05, //lengthL
    0x00, //lengthH
    0x00, //cmd type
    0x20, //cmd tpye
    0x00, //event
    0x00, //R or L
    0x00, //parameter
};

#define RACE_CH            0
#define RACE_TYPE          1
#define LENGTH_L           2
#define LENGTH_H           3
#define CMD_ID             6
#define EARBUD_ID          7
#define DATA               8
#define BAT_LV             8
#define BAUDRATE           8
#define KEY_ID             8

#define CASE_LID_OPEN      2
#define CASE_LID_CLOSE     3
#define CASE_CHARGER_OFF   4
#define CASE_CHARGER_KEY   5
#define CASE_BATTER_LEVEL  6
#define CASE_CHARGER_STATE 7
#define CASE_LID_PRE_CLOSE 8
#define CASE_RACE_ENABLE   0xF


#define UART_PORT_1WIRE         SERIAL_PORT_DEV_UART_1
#ifdef MTK_SMART_CHARGER_1WIRE_9600
#define RX_HANDLE_TIMER         8
#define UART_PORT_BAUDRATE      HAL_UART_BAUDRATE_9600;
#else
#define RX_HANDLE_TIMER         2
#define UART_PORT_BAUDRATE      HAL_UART_BAUDRATE_115200
#endif
#define CHECK_TIMER_MS          200
#define RACE_CHECK_TIMER_MS     1000


uint8_t LOCK_DVFS = false;

const uint8_t patternTable[] = {
    0,
    0,
    DRV_CHARGER_EVENT_LID_OPEN_PATTERN,
    DRV_CHARGER_EVENT_LID_CLOSE_PATTERN,
    DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN,
    DRV_CHARGER_EVENT_CHARGER_KEY_PATTERN,
    0,
    0,
    DRV_CHARGER_EVENT_USER1_PATTERN
};


////////////////////////////////////////////////////////////////////////////////
//  Declarations ///////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void smchg_1wire_app_cmd(uint8_t cmd)
{
    switch (cmd)
    {
        case DRV_CHARGER_EVENT_LID_OPEN_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[lid_open]", 0);
            break;
        case DRV_CHARGER_EVENT_LID_CLOSE_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[lid_close]", 0);
            break;
        case DRV_CHARGER_EVENT_CHARGER_OFF_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[chg_off]", 0);
            break;
        case DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[case_out]", 0);
            break;
#if (DRV_SMARTCHARGER_RESERVED_PATTERN_ENABLE)
        case DRV_SMARTCHARGER_EVENT_RESERVE0_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[reserve0]", 0);
            break;
        case DRV_SMARTCHARGER_EVENT_RESERVE1_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[reserve1]", 0);
            break;
        case DRV_SMARTCHARGER_EVENT_RESERVE2_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[reserve2]", 0);
            break;
        case DRV_SMARTCHARGER_EVENT_RESERVE3_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[reserve3]", 0);
            break;
        case DRV_SMARTCHARGER_EVENT_RESERVE4_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[reserve4]", 0);
            break;
#endif
        case DRV_SMARTCHARGER_EVENT_CHARGER_IN_INTERRUPT:
            log_hal_msgid_info("1wire, app_cmd[chg_in_intr]", 0);
            break;
        case DRV_SMARTCHARGER_EVENT_CHARGER_COMPLETE_INTERRUPT:
            log_hal_msgid_info("1wire, app_cmd[chg_compl_intr]", 0);
            break;
        case DRV_SMARTCHARGER_EVENT_CHARGER_RECHARGE_INTERRUPT:
            log_hal_msgid_info("1wire, app_cmd[chg_rechg_intr]", 0);
            break;
        case DRV_CHARGER_EVENT_CHARGER_KEY_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[chg_key]", 0);
            break;
        case DRV_CHARGER_EVENT_USER1_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[lid_pre_close]", 0);
            break;
        case DRV_CHARGER_EVENT_USER2_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[user2]", 0);
            break;
        case DRV_CHARGER_EVENT_USER3_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[user3]", 0);
            break;
        case DRV_CHARGER_EVENT_CHARGER_IN_PATTERN:
            log_hal_msgid_info("1wire, app_cmd[case_in]", 0);
            break;

        default:
            log_hal_msgid_error("1wire, app_cmd[unknow]", 0);
            break;
    }
}

uint32_t smchg_1wire_baudrate_report(hal_uart_baudrate_t baudrate)
{
    uint32_t ret;

    switch (baudrate)
    {
        case HAL_UART_BAUDRATE_9600:
            ret = 9600;
            break;
        case HAL_UART_BAUDRATE_115200:
            ret = 115200;
            break;
        case HAL_UART_BAUDRATE_921600:
            ret = 921600;
            break;
        case HAL_UART_BAUDRATE_2000000:
            ret = 2000000;
            break;
        case HAL_UART_BAUDRATE_3000000:
            ret = 3000000;
            break;

        default:
            ret = 0;
            break;
    }

    return ret;
}

static void smart_charger_lock_dvfs_1wire(void)
{
    if (LOCK_DVFS == FALSE)
    {
        //log_hal_msgid_info("1wire, dvfs_lock", 0);
        //dvfs_lock_control("SmartCharger",DVFS_156M_SPEED, DVFS_LOCK);
        LOCK_DVFS = TRUE;
    }
}

static void smart_charger_unlock_dvfs_1wire(void)
{
    if (LOCK_DVFS == TRUE)
    {
        //log_hal_msgid_info("1wire, dvfs_unlock", 0);
        //dvfs_lock_control("SmartCharger",DVFS_156M_SPEED, DVFS_UNLOCK);
        LOCK_DVFS = FALSE;
    }
}

static void smchg_1wire_uart_cb(hal_uart_callback_event_t status, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (status == HAL_UART_EVENT_READY_TO_READ)
    {
        log_hal_msgid_warning("1wire, uart_cb, rx_done", 0);
        chk_cnt++;
        if (isTimerStart == FALSE)
        {
            isTimerStart = TRUE;
            if (xTimerStartFromISR(pSmartChargerTimer, &xHigherPriorityTaskWoken ) != pdPASS)
            {
                log_hal_msgid_error("1wire, uart_cb, rx_handle timer fail", 0);
            }
        }
    }
    else if (status == HAL_UART_EVENT_WAKEUP_SLEEP)
    {
    }
    else if (status == HAL_UART_EVENT_TRANSMISSION_DONE)
    {
        //log_hal_msgid_warning("1wire, uart_cb, tx_done", 0);
    }

    if (xHigherPriorityTaskWoken != pdFALSE) {
        // Actual macro used here is port specific.
        portYIELD_FROM_ISR(pdTRUE);
    }
}

static void smchg_1wire_uart_init(void)
{
    /* init 1 wire uart */
    hal_uart_config_t uart_config;
    hal_uart_status_t status;
    hal_uart_dma_config_t dma_config;

    uart_config.baudrate = UART_PORT_BAUDRATE;
    uart_config.parity = HAL_UART_PARITY_NONE;
    uart_config.stop_bit = HAL_UART_STOP_BIT_1;
    uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    hal_uart_deinit(UART_PORT_1WIRE);
    status = hal_uart_init(UART_PORT_1WIRE, &uart_config);

    if (status != HAL_UART_STATUS_OK)
    {
        log_hal_msgid_error("1wire, uart_init fail[%d]", 1, status);
    }

    log_hal_msgid_info("1wire, uart_init, baudrate[%d]", 1, smchg_1wire_baudrate_report(uart_config.baudrate));

    if (boot_Flag == 0)
    {
        log_hal_msgid_info("1wire, boot_evt send", 0);
#ifndef CHARGER_CASE_1WIRE_1565
        hal_gpio_set_output(SMCHG_1WIRE_COM_MODE_PIN, HAL_GPIO_DATA_LOW);
#endif
        hal_pinmux_set_function(SMCHG_1WIRE_TX_PIN, SMCHG_1WIRE_TX_UART_MODE);
        uart_send_polling(UART_PORT_1WIRE , raceEvt, sizeof(raceEvt));
        uart_wait_empty(UART_PORT_1WIRE);
        hal_pinmux_set_function(SMCHG_1WIRE_TX_PIN, SMCHG_1WIRE_TX_GPIO_MODE);

#ifndef CHARGER_CASE_1WIRE_1565
        hal_gpio_set_output(SMCHG_1WIRE_COM_MODE_PIN, HAL_GPIO_DATA_HIGH);
#endif
        boot_Flag = 1;
    }

    dma_config.receive_vfifo_alert_size = 8;
    dma_config.receive_vfifo_buffer = oneWire_rx;
    dma_config.receive_vfifo_buffer_size = SMCHG_1WIRE_UART_BUFF_SIZE;
    dma_config.receive_vfifo_threshold_size = 5;
    dma_config.send_vfifo_buffer = oneWire_tx;
    dma_config.send_vfifo_buffer_size = SMCHG_1WIRE_UART_BUFF_SIZE;
    dma_config.send_vfifo_threshold_size = 5;
    status = hal_uart_set_dma(UART_PORT_1WIRE, (const hal_uart_dma_config_t *)&dma_config);

    if (status != HAL_UART_STATUS_OK)
    {
        log_hal_msgid_error("1wire, uart_init, set_dma fail, status[%d]", 1, status);
    }

    status = hal_uart_register_callback(UART_PORT_1WIRE, smchg_1wire_uart_cb, NULL);
    if (status != HAL_UART_STATUS_OK)
    {
        log_hal_msgid_error("1wire, uart_init, uart_cb fail, status[%d]", 1, status);
    }
}

static void smchg_1wire_race_mode(hal_uart_baudrate_t baudrate)
{
#ifdef MTK_SMART_CHARGER_1WIRE_RACE_ENABLE
    log_hal_msgid_info("1wire, race_mode, baudrate[%d]", 1, smchg_1wire_baudrate_report(baudrate));
    xTimerChangePeriod(pSmartChargerOutTimer, pdMS_TO_TICKS(RACE_CHECK_TIMER_MS), 0);//todo

    hal_uart_deinit(UART_PORT_1WIRE);
    log_hal_msgid_info("1wire, race_mode, uart_deinit", 0);
    race_1wire_init(UART_PORT_1WIRE, baudrate);
#endif
}

static void smchg_1wire_leave_race_mode(void)
{
#ifdef MTK_SMART_CHARGER_1WIRE_RACE_ENABLE
    log_hal_msgid_info("1wire, leave_race_mode", 0);
    race_1wire_deinit(); //todo
    //smchg_1wire_uart_init();
    xTimerChangePeriod(pSmartChargerOutTimer, pdMS_TO_TICKS(CHECK_TIMER_MS), 0);
#endif
}

static void smchg_1wire_chg_mode(void)
{
    log_hal_msgid_info("1wire, chg_mode", 0);

    if (xTimerStop(pSmartChargerOutTimer, 0) == pdFAIL)
    {
        log_hal_msgid_error("1wire, chg_mode, stop out_of_case_timer fail", 0);
    }

    hal_pinmux_set_function(SMCHG_1WIRE_RX_PIN, SMCHG_1WIRE_RX_GPIO_MODE);

#ifndef CHARGER_CASE_1WIRE_1565
    hal_gpio_set_output(SMCHG_1WIRE_COM_MODE_PIN, HAL_GPIO_DATA_HIGH);
#endif

    if (LOCK_DVFS_SLEEP)
    {
        hal_sleep_manager_unlock_sleep(SLEEP_LOCK_CHARGER_CASE);
        LOCK_DVFS_SLEEP = 0;
    }
}

static void smchg_1wire_com_mode(void)
{
    log_hal_msgid_info("1wire, com_mode", 0);

    hal_pinmux_set_function(SMCHG_1WIRE_RX_PIN, SMCHG_1WIRE_RX_UART_MODE);
    hal_pinmux_set_function(SMCHG_1WIRE_TX_PIN, SMCHG_1WIRE_TX_UART_MODE);
    hal_gpt_delay_ms(1);
    hal_pinmux_set_function(SMCHG_1WIRE_TX_PIN, SMCHG_1WIRE_TX_GPIO_MODE);

#ifndef CHARGER_CASE_1WIRE_1565
    hal_gpio_set_output(SMCHG_1WIRE_COM_MODE_PIN, HAL_GPIO_DATA_LOW);
#endif

    if (!LOCK_DVFS_SLEEP)
    {
        smchg_1wire_uart_init();
        hal_sleep_manager_lock_sleep(SLEEP_LOCK_CHARGER_CASE);
        LOCK_DVFS_SLEEP = 1;
    }
}

static void smchg_1wire_send_to_app(uint8_t cmd, uint32_t data, uint16_t data_len)
{
    if (last_cmd == cmd)
    {
        if (cmd == DRV_CHARGER_EVENT_LID_OPEN_PATTERN ||  //todo check what cmd need process
           cmd == DRV_CHARGER_EVENT_LID_CLOSE_PATTERN ||
           cmd == DRV_CHARGER_EVENT_USER1_PATTERN)
        {
            log_hal_msgid_warning("1wire, repeat app_cmd[%d]", 1, cmd);
            return;
        }
    }

    last_cmd = cmd;

    if (cmd)
    {
        smartcharger_callback_t callback_handler = ChargerSmartCase_GetSmartCaseHandle();
        if (callback_handler)
        {
            smchg_1wire_app_cmd(cmd);
            callback_handler(cmd, data, data_len);
        }
        else
        {
            pre_cmd = cmd;
            pre_data = data;
            pre_data_len = data_len;
        }
    }
}

void smchg_1wire_pre_handle(void)
{
    if (pre_cmd)
    {
        smchg_1wire_send_to_app(pre_cmd , pre_data, pre_data_len);
        pre_cmd = 0;
        pre_data = 0;
        pre_data_len = 0;
    }
}

static void smchg_1wire_rx_handle(TimerHandle_t pxExpiredTimer)
{
    UNUSED(pxExpiredTimer);

    uint8_t raceCmd[SMCHG_1WIRE_UART_BUFF_SIZE] = {0};

    isTimerStart = FALSE;

    uint32_t data_size = hal_uart_get_available_receive_bytes(UART_PORT_1WIRE);
    if (data_size)
        hal_uart_receive_dma(UART_PORT_1WIRE, raceCmd, data_size);

    if ((data_size >= 8) && (raceCmd[RACE_TYPE] == raceEvt[RACE_TYPE]))
        return;

    log_hal_msgid_info("1wire, rx_handle, data_size[%d]", 1, data_size);

#ifndef MTK_CHARGER_CASE_1WIRE_3PIN
    if (xTimerReset(pSmartChargerOutTimer, 0) == pdFAIL)
    {
        log_hal_msgid_error("1wire, rx_handle, reset out_of_case_timer fail", 0);
    }
#endif

    if (data_size >= 8)
    {
        audio_channel_t earbud_ch = ami_get_audio_channel();

        log_hal_msgid_info("1wire, rx_handle, race_ch[0x%X], race_type[0x%X], cmd_id[0x%X], earbud_id[%d], data[%d], earbud_ch[%d]", 6,
        raceCmd[RACE_CH], raceCmd[RACE_TYPE], raceCmd[CMD_ID], raceCmd[EARBUD_ID], raceCmd[DATA], earbud_ch);

        raceEvt[EARBUD_ID] = earbud_ch;

        if ((raceCmd[RACE_CH] == 0x5) && (raceCmd[RACE_TYPE] == 0x5A) && (raceCmd[CMD_ID]))
        {
            uint32_t data = 0;
            uint16_t data_len = 0;

            if (raceCmd[CMD_ID] == CASE_LID_OPEN || raceCmd[CMD_ID] == CASE_CHARGER_OFF)
            {
                data = raceCmd[BAT_LV];
                data_len = 3;
            }
            else if (raceCmd[CMD_ID] == CASE_LID_CLOSE)
            {
                /* down frequncy */
                smart_charger_unlock_dvfs_1wire();
            }
            else if (raceCmd[CMD_ID] == CASE_CHARGER_KEY)
            {
                data = raceCmd[KEY_ID];
                data_len = 8;
            }

            if (raceCmd[CMD_ID] <= CASE_LID_PRE_CLOSE)
            {
                smchg_1wire_send_to_app(patternTable[raceCmd[CMD_ID]], data, data_len);
            }

            if (raceCmd[EARBUD_ID] == earbud_ch)/* return event */
            {
                raceEvt[CMD_ID] = raceCmd[CMD_ID];
                raceEvt[EARBUD_ID] = earbud_ch;
                if (raceCmd[CMD_ID] == CASE_CHARGER_STATE)
                {
                    if (battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY) == 100)
                    {
                        raceEvt[DATA] = 1;
                    }
                    else
                    {
                        raceEvt[DATA] = 0;
                    }
                }
                else
                {
                    raceEvt[BAT_LV] = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
                }

                hal_pinmux_set_function(SMCHG_1WIRE_TX_PIN, SMCHG_1WIRE_TX_UART_MODE);
                uart_send_polling(UART_PORT_1WIRE , (const uint8_t *)raceEvt, sizeof(raceEvt));
                uart_wait_empty(UART_PORT_1WIRE);
                //uint32_t send_len = hal_uart_send_dma((hal_uart_port_t)UART_PORT_1WIRE, (const uint8_t *)raceEvt, sizeof(raceEvt));
                log_hal_msgid_info("1wire, rx_handle, response, evt_id[0x%X], earbud_id[%d], bat_lv[%d]", 3,
                    raceEvt[CMD_ID], raceEvt[EARBUD_ID], raceEvt[BAT_LV]);

                if (raceCmd[CMD_ID] == CASE_CHARGER_OFF)
                {
                    hal_pinmux_set_function(SMCHG_1WIRE_RX_PIN, SMCHG_1WIRE_RX_GPIO_MODE);
                }

                hal_pinmux_set_function(SMCHG_1WIRE_TX_PIN, SMCHG_1WIRE_TX_GPIO_MODE);

                if (raceCmd[CMD_ID] == CASE_RACE_ENABLE)
                {
                    race_mode_flag = 1;
                    smchg_1wire_race_mode(raceCmd[BAUDRATE]);
                }
            }
        }
    }
    else if (data_size > 0)
    {
        log_hal_msgid_error("1wire, rx_handle, unknown_data[0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X]", 8,
            raceCmd[0], raceCmd[1], raceCmd[2], raceCmd[3], raceCmd[4], raceCmd[5], raceCmd[6], raceCmd[7]);
    }
}

static void smchg_1wire_out_of_case_timer_cb(TimerHandle_t pxExpiredTimer)
{
    uint32_t data;
#if defined(CHARGER_CASE_1WIRE_1565)
    uint32_t pmuData;
#endif
    int cnt;
    uint32_t tx_idle, rx_idle, tx_len, rx_len;
    uint8_t ref_cnt = chk_cnt ;
    tx_idle = uart_query_tx_empty(UART_PORT_1WIRE);
    rx_idle = uart_query_rx_empty(UART_PORT_1WIRE);
    tx_len = hal_uart_get_available_send_space(UART_PORT_1WIRE);
    rx_len = hal_uart_get_available_receive_bytes(UART_PORT_1WIRE);
    if ((!tx_idle) || (!rx_idle) || rx_len)
    {
        log_hal_msgid_warning("1wire, uart_busy_1, tx_idle[%d], rx_idle[%d], tx_len[%d], rx_len[%d]", 4,
            tx_idle, rx_idle, tx_len, rx_len);
        return;
    }
    for(cnt = 4; cnt; cnt--){
#if defined(CHARGER_CASE_1WIRE_1568)
        data = pmu_auxadc_get_channel_value(PMU_AUX_VBUS);
#elif defined(CHARGER_CASE_1WIRE_1565)
        data = pmu_auxadc_get_channel_value(PMU_AUX_VCHG);
#else
        hal_adc_get_data_polling(HAL_ADC_CHANNEL_6 , &data);
#endif
        if (data >= OUT_OF_CASE_THRD)
        {
            chk_cnt = 0;
            //log_hal_msgid_info("1wire, keep_com_mode, vbus_uart[%d]", 1, data);
            return ;
        }
        else if (chk_cnt != ref_cnt)
        {
            log_hal_msgid_info("1wire, keep_com_mode, get_new_rx",0);
            return ;
        }
    }
    tx_idle = uart_query_tx_empty(UART_PORT_1WIRE);
    rx_idle = uart_query_rx_empty(UART_PORT_1WIRE);
    tx_len = hal_uart_get_available_send_space(UART_PORT_1WIRE);
    rx_len = hal_uart_get_available_receive_bytes(UART_PORT_1WIRE);
    if ((!tx_idle) || (!rx_idle) || rx_len)
    {
        log_hal_msgid_warning("1wire, uart_busy_2, tx_idle[%d], rx_idle[%d], tx_len[%d], rx_len[%d]", 4,
            tx_idle, rx_idle, tx_len, rx_len);
        return;
    }

    log_hal_msgid_info("1wire, out_of_case, vbus[%d]", 1, data);

    /* if adc less than 500 means real charger out */

    if (race_mode_flag)
    {
        smchg_1wire_leave_race_mode();
        race_mode_flag = 0;
    }

    /* real charger out */
    smchg_1wire_chg_mode();
    hal_gpio_pull_down(SMCHG_1WIRE_RX_PIN);

    /* send charger out to app */
    smchg_1wire_send_to_app(DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN, 0, 0);

    /* down frequncy */
    smart_charger_unlock_dvfs_1wire();

#if defined(CHARGER_CASE_1WIRE_1565)
    /* Dsiable VIO18 pullup */
    pmuData  = pmu_get_register_value(0x2, 0xFFFF, 0);
    pmuData  = BIT_FIELD_INSERT32(pmuData, 3, 1, 1);
    pmu_force_set_register_value(0x2, pmuData);

    /* VBUS discharge on */
    pmuData  = pmu_get_register_value(0x32A, 0xFFFF, 0);
    pmuData = BIT_FIELD_INSERT16(pmuData, 11, 1, 1);
    pmu_force_set_register_value(0x32A, pmuData);

    /* EOC */
    pmuData  = pmu_get_register_value(0x1E, 0xFFFF, 0);
    pmuData = BIT_FIELD_INSERT16(pmuData, 5, 3, 1);
    pmu_force_set_register_value(0x1E, pmuData);

    uint32_t bat_volt = battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE);
    if (bat_volt > VBAT_THRD)
    {
        pmu_eoc_ctrl(1);
    }
    else
    {
        log_hal_msgid_warning("1wire, out_of_case, pmu_eoc_ctrl ignore, rg_320[0x%X]", 1, pmu_get_register_value(0x320, 0xFFFF, 0));
    }

    /* PSW_MAIN_CL = ON PSW_MAIN = OFF */
    pmuData  = pmu_get_register_value(0x2, 0xFFFF, 0);
    pmuData  = BIT_FIELD_INSERT32(pmuData, 2, 1, 1);
    if (bat_volt > VBAT_THRD)
    {
        pmuData  = BIT_FIELD_INSERT32(pmuData, 0, 1, 1);
    }
    pmu_force_set_register_value(0x2, pmuData);

    /* VBUS discharge off */
    pmuData  = pmu_get_register_value(0x32A, 0xFFFF, 0);
    pmuData = BIT_FIELD_INSERT16(pmuData, 11, 1, 0);
    pmu_force_set_register_value(0x32A, pmuData);

#endif
}

static void smchg_1wire_chg_isr_cb(battery_management_event_t event, const void *data)
{
    if (event != BATTERY_MANAGEMENT_EVENT_CHARGER_EXIST_UPDATE)
        return;

    int32_t chg_exist = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
    log_hal_msgid_info("1wire, chg_isr_cb, chg_exist[%d]", 1, chg_exist);

    nvkey_status_t nvkey_status;
    uint32_t onewire_nvkey_item_size = sizeof(isLogging);

    if (is1WireLoggingMode)
    {
        nvkey_status = nvkey_read_data(NVKEYID_1WIRE_LOGGING_SETTING, (uint8_t *)(&isLogging), &onewire_nvkey_item_size);
        if ((nvkey_status == NVKEY_STATUS_OK) && (onewire_nvkey_item_size == 5))
        {
            if (isLogging[4] != NVKEY_STATUS_OK){
                log_hal_msgid_info("1wire, chg_isr_cb, logging times[%d]", 1, isLogging[4]);
                isLogging[4]--;
                is1WireLoggingMode = TRUE;
                nvkey_write_data(NVKEYID_1WIRE_LOGGING_SETTING, isLogging, sizeof(isLogging));
                return;
            }
            else
            {
                hal_wdt_software_reset();
            }
        }
    }

#ifdef CHARGER_CASE_1WIRE_1565
    uint32_t pmuData;

    if (chg_exist)
    {
    #ifndef MTK_CHARGER_CASE_1WIRE_3PIN
        if (race_mode_flag)
        {
            smchg_1wire_leave_race_mode();
            race_mode_flag = 0;
        }

        if (last_cmd == DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN)
        {
            /* up frequncy */
            smart_charger_lock_dvfs_1wire();
        }

        pmuData  = pmu_get_register_value(0x2, 0xFFFF, 0);

        /* Enable VIO18 pullup */
        pmuData  = BIT_FIELD_INSERT32(pmuData, 3, 1, 0);

        /* PSW_MAIN ON */
        pmuData  = BIT_FIELD_INSERT32(pmuData, 0, 1, 0);
        pmu_force_set_register_value(0x2, pmuData);
        hal_gpt_delay_us(100);

        /* PSW_MAIN_CL OFF */
        pmuData  = BIT_FIELD_INSERT32(pmuData, 2, 1, 0);
        pmu_force_set_register_value(0x2, pmuData);

        /* EOC = CHG*/
        pmuData  = pmu_get_register_value(0x1E, 0xFFFF, 0);
        pmuData = BIT_FIELD_INSERT16(pmuData, 5, 3, 0);
        pmu_force_set_register_value(0x1E, pmuData);

        #ifdef MTK_BATTERY_MANAGEMENT_ENABLE
        if ((bm_ctrl_info.jeita_state ==HW_JEITA_COLD_STAGE)||
           (bm_ctrl_info.jeita_state ==HW_JEITA_HOT_STAGE) ||
           (battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE)==CHARGER_STATE_THR)){
              log_hal_msgid_error("1wire, chg_isr_cb, abnormal temperature", 0);
        }else{
              pmu_eoc_ctrl(0);
        }
        #else
        pmu_eoc_ctrl(0);
        #endif

        pmuData  = pmu_get_register_value(0x32A, 0xFFFF, 0);
        pmuData = BIT_FIELD_INSERT16(pmuData, 11, 1, 1);
        pmu_force_set_register_value(0x32A, pmuData);

        /*charger mode first when charger in */
        smchg_1wire_chg_mode();
        smchg_1wire_send_to_app(DRV_CHARGER_EVENT_CHARGER_IN_PATTERN, 0, 0);
    #else
        smchg_1wire_com_mode();
    #endif
    }
    else
    {
    #ifndef MTK_CHARGER_CASE_1WIRE_3PIN
        /* Dsiable VIO18 pullup */
        pmuData  = pmu_get_register_value(0x2, 0xFFFF, 0);
        pmuData  = BIT_FIELD_INSERT32(pmuData, 3, 1, 1);
        pmu_force_set_register_value(0x2, pmuData);

        /* VBUS discharge on */
        pmuData  = pmu_get_register_value(0x32A, 0xFFFF, 0);
        pmuData = BIT_FIELD_INSERT16(pmuData, 11, 1, 1);
        pmu_force_set_register_value(0x32A, pmuData);

        /* EOC */
        pmuData  = pmu_get_register_value(0x1E, 0xFFFF, 0);
        pmuData = BIT_FIELD_INSERT16(pmuData, 5, 3, 1);
        pmu_force_set_register_value(0x1E, pmuData);

        uint32_t bat_volt = battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE);
        if (bat_volt > VBAT_THRD)
        {
            pmu_eoc_ctrl(1);
        }
        else
        {
            log_hal_msgid_warning("1wire, chg_isr_cb, pmu_eoc_ctrl ignore, rg_320[0x%X]", 1, pmu_get_register_value(0x320, 0xFFFF, 0));
        }

        /* always comm mode */
        /* PSW_MAIN = OFF , ENABLE PULL UP */
        pmuData  = pmu_get_register_value(0x2, 0xFFFF, 0);
        if (bat_volt > VBAT_THRD)
        {
            pmuData  = BIT_FIELD_INSERT32(pmuData, 0, 1, 1);
        }
        else
        {
            pmuData  = BIT_FIELD_INSERT32(pmuData, 2, 1, 1);
        }
        pmuData  = BIT_FIELD_INSERT32(pmuData, 3, 1, 0);
        pmu_force_set_register_value(0x2, pmuData);

        /* VBUS discharge off */
        pmuData  = pmu_get_register_value(0x32A, 0xFFFF, 0);
        pmuData = BIT_FIELD_INSERT16(pmuData, 11, 1, 0);
        pmu_force_set_register_value(0x32A, pmuData);

        smchg_1wire_com_mode();
        xTimerStart(pSmartChargerOutTimer, 0);
    #else
        smchg_1wire_chg_mode();
    #endif
    }

#else
    uint32_t vbus;
    #ifdef CHARGER_CASE_1WIRE_1568
    vbus = pmu_auxadc_get_channel_value(PMU_AUX_VBUS);
    #else
    hal_adc_get_data_polling(HAL_ADC_CHANNEL_6 , &vbus);
    #endif
    log_hal_msgid_info("1wire, chg_isr_cb, vbus[%d]", 1, vbus);

    if (chg_exist == 0)
    {
        /* comm mode */
        if (vbus > 700 && vbus < 3000)
        {
            smchg_1wire_com_mode();
            xTimerStart(pSmartChargerOutTimer, 0);
        }
        else
        {
            /* real charger out */
            /* send charger out to app */
            smchg_1wire_send_to_app(DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN, 0, 0);
            smchg_1wire_chg_mode();
            hal_gpio_pull_down(SMCHG_1WIRE_RX_PIN);

            /* down frequncy */
            smart_charger_unlock_dvfs_1wire();
        }
    }
    else if (chg_exist == 1)
    {
        if (race_mode_flag)
        {
            smchg_1wire_leave_race_mode();
            race_mode_flag = 0;
        }

        if (last_cmd == DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN)
        {
            /* up frequncy */
            smart_charger_lock_dvfs_1wire();
        }

        /*charger mode first when charger in */
        smchg_1wire_chg_mode();
        hal_gpio_pull_up(SMCHG_1WIRE_RX_PIN);
        smchg_1wire_send_to_app(DRV_CHARGER_EVENT_CHARGER_IN_PATTERN, 0, 0);
    }
#endif
}

void smchg_1wire_init(smartcharger_callback_t handler)
{
    uint32_t onewire_nvkey_item_size = sizeof(isLogging);
    nvkey_status_t nvkey_status;

    log_hal_msgid_info("1wire, init enter", 0);

    last_cmd = DRV_CHARGER_EVENT_CHARGER_OUT_PATTERN;

    pSmartChargerTimer = xTimerCreate("Smart Charger", pdMS_TO_TICKS(RX_HANDLE_TIMER), pdFALSE, NULL, smchg_1wire_rx_handle);
    if (!pSmartChargerTimer)
    {
        log_hal_msgid_error("1wire, init rx_handle_timer_cb fail", 0);
        assert(0);
    }

    pSmartChargerOutTimer = xTimerCreate("Smart Charger", pdMS_TO_TICKS(CHECK_TIMER_MS), pdTRUE, NULL, smchg_1wire_out_of_case_timer_cb);
    if (!pSmartChargerOutTimer)
    {
        log_hal_msgid_error("1wire, init out_of_case_timer_cb fail", 0);
        assert(0);
    }

    /* 0xF023 get special syslog configuartion */
    nvkey_status = nvkey_read_data(NVKEYID_1WIRE_LOGGING_SETTING, (uint8_t *)(&isLogging), &onewire_nvkey_item_size);
    if ((nvkey_status == NVKEY_STATUS_OK) && (onewire_nvkey_item_size == 5))
    {
        if (isLogging[4] != 0){
            log_hal_msgid_info("1wire, init logging_times[%d]", 1, isLogging[4]);
            isLogging[4]--;
            is1WireLoggingMode = TRUE;
            nvkey_write_data(NVKEYID_1WIRE_LOGGING_SETTING, (uint8_t *)(&isLogging), onewire_nvkey_item_size);
            return;
        }
    }

#ifdef CHARGER_CASE_1WIRE_1565
    #ifdef MTK_CHARGER_CASE_1WIRE_3PIN
    uint32_t pmuData;

    pmuData  = pmu_get_register_value(0x2, 0xFFFF, 0);
    pmuData  = BIT_FIELD_INSERT32(pmuData, 0, 1, 1);
    pmuData  = BIT_FIELD_INSERT32(pmuData, 3, 1, 1);
    pmu_force_set_register_value(0x2, pmuData);
    #endif

    //TX->GPIO, input no pull
    hal_pinmux_set_function(SMCHG_1WIRE_TX_PIN, SMCHG_1WIRE_TX_GPIO_MODE);
    hal_gpio_set_direction(SMCHG_1WIRE_TX_PIN, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_disable_pull(SMCHG_1WIRE_TX_PIN);

    //RX->GPIO, input pull down
    hal_pinmux_set_function(SMCHG_1WIRE_RX_PIN, SMCHG_1WIRE_RX_GPIO_MODE);
    hal_gpio_set_direction(SMCHG_1WIRE_RX_PIN, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_pull_down(SMCHG_1WIRE_RX_PIN);

#else //68, 5x
    #ifdef CHARGER_CASE_1WIRE_155x
    //GPIO0 -> ADC mode
    hal_adc_init();
    hal_pinmux_set_function(HAL_GPIO_0 , HAL_GPIO_0_AUXADC6);
    #endif

    //TX->GPIO, input no pull
    hal_pinmux_set_function(SMCHG_1WIRE_TX_PIN, SMCHG_1WIRE_TX_GPIO_MODE);
    hal_gpio_set_direction(SMCHG_1WIRE_TX_PIN, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_disable_pull(SMCHG_1WIRE_TX_PIN);

    //RX->GPIO, input pull down
    hal_pinmux_set_function(SMCHG_1WIRE_RX_PIN, SMCHG_1WIRE_RX_GPIO_MODE);
    hal_gpio_set_direction(SMCHG_1WIRE_RX_PIN, HAL_GPIO_DIRECTION_INPUT);
    hal_gpio_pull_down(SMCHG_1WIRE_RX_PIN);

    /* chg/comm gpio init */
    hal_pinmux_set_function(SMCHG_1WIRE_COM_MODE_PIN, HAL_GPIO_3_GPIO3);
    hal_gpio_set_direction(SMCHG_1WIRE_COM_MODE_PIN, HAL_GPIO_DIRECTION_OUTPUT);
    hal_gpio_set_output(SMCHG_1WIRE_COM_MODE_PIN, HAL_GPIO_DATA_HIGH);
#endif

    smchg_1wire_uart_init();

    if (battery_management_register_callback(smchg_1wire_chg_isr_cb) != BATTERY_MANAGEMENT_STATUS_OK) {
        log_hal_msgid_error("1wire, init chg_isr_cb fail", 0);
        assert(0);
    }
    log_hal_msgid_info("1wire, init done", 0);
}
