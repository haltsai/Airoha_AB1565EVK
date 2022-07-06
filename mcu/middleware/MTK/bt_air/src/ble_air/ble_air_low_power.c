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
 
#ifdef MTK_PORT_SERVICE_BT_ENABLE


#include "bt_gap_le.h"
#include "ble_air_interface.h"
#include "bt_debug.h"
#include "bt_hci.h"
#include "syslog.h"
#include "FreeRTOS.h"
#include "timers.h"

#ifdef BLE_AIR_LOW_POWER_CONTROL
#include "ble_air_internal.h"

#if 0
#define BLE_AIR_TIMER_PERIOD       30000
extern TimerHandle_t g_xTimer_low_power;

static bool ble_air_is_timer_active(TimerHandle_t timer_id)
{
    if ((NULL != timer_id) && (pdFALSE != xTimerIsTimerActive(timer_id))) {
        return true;
    } 
    return false;
}

static void ble_air_stop_timer(TimerHandle_t timer_id)
{
    if (true == ble_air_is_timer_active(timer_id)) {
        xTimerStop(timer_id, 0);
    }
}

static void ble_air_start_timer(TimerHandle_t timer_id)
{
    if (NULL == timer_id) {
        return;
    }
    xTimerReset(timer_id, 0);
}

static void ble_air_update_connection_interval_timerout(TimerHandle_t xTimer)
{
    LOG_MSGID_I(AIR, "ble_air_update_connection_interval_timerout \r\n", 0);

    int32_t status = BT_STATUS_FAIL;
    ble_air_conn_param_priority_t interval = BLE_AIR_CONN_PARAM_PRIORITY_DEFAULT;
    
    uint16_t conn_handle = ble_air_get_real_connected_handle();
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);

    ble_air_stop_timer(g_xTimer_low_power);
    if (buffer_t) {   
        interval = BLE_AIR_CONN_PARAM_LOW_POWER;
        status = ble_air_update_connection_interval_internal(buffer_t->conn_handle, interval);
        if (status == BT_STATUS_SUCCESS) {
            buffer_t->low_power_t.conn_priority = interval;
        }
        LOG_MSGID_I(AIR, "AIR update_connection_interval_timerout, status = %d, interval = %d \r\n", 2, status, interval);
    }

}
#endif
/**
 * @brief Function for updating connection params to client.
 *
 * @param[in]  conn_id   connection id.
 * @param[in]  app_name  application name, who trigger the request.
 *
 * @param[out] status    result of updating connection params request.
 */
static bt_status_t ble_air_update_connection_interval_internal(uint16_t conn_handle, ble_air_conn_param_priority_t conn_priority)
{
    bt_status_t status;
    bt_hci_cmd_le_connection_update_t conn_params;
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);

    if (NULL == buffer_t) {
        return BT_STATUS_FAIL;
    }
    
    if (buffer_t->low_power_t.conn_priority == conn_priority) {
        #if 0
        if (true == ble_air_is_timer_active(g_xTimer_low_power)) {
            ble_air_start_timer(g_xTimer_low_power);
        }
        #endif
        return BT_STATUS_FAIL;
    }
    conn_params.supervision_timeout = 0x0258;            /** TBC: 6000ms : 600 * 10 ms. */
    conn_params.connection_handle = conn_handle;
    LOG_MSGID_I(AIR, "ble_air_update_connection_interval, conn_priority %d\r\n", 1, conn_priority);

    switch (conn_priority) {
        case BLE_AIR_CONN_PARAM_HIGH_SPEED_ANDROID: {
            conn_params.conn_interval_min = 0x0006;/** TBC: 7.5ms : 6 * 1.25 ms. */
            conn_params.conn_interval_max = 0x000A;/** TBC: 12.5ms : 10 * 1.25 ms. */
            conn_params.conn_latency = 0;
        }
        break;
        case BLE_AIR_CONN_PARAM_HIGH_SPEED_IOS: {
            conn_params.conn_interval_min = 0x0018;/** TBC: 30ms : 24 * 1.25 ms. */
            conn_params.conn_interval_max = 0x0018;/** TBC: 30ms : 24 * 1.25 ms. */
            conn_params.conn_latency = 0;
        }
        break;
        case BLE_AIR_CONN_PARAM_LOW_POWER:
        case BLE_AIR_CONN_PARAM_PRIORITY_DEFAULT: {
            conn_params.conn_interval_min = 0x0120;/** TBC: 360ms : 288 * 1.25 ms. */
            conn_params.conn_interval_max = 0x0130;/** TBC: 380ms : 304 * 1.25 ms. */
            conn_params.conn_latency = 4;
        }
        break;
        default: {
            conn_params.conn_interval_min = 0x0120;/*TBC: 360ms : 288 * 1.25 ms*/
            conn_params.conn_interval_max = 0x0130;/*TBC: 380ms : 304 * 1.25 ms*/
            conn_params.conn_latency = 4;
        }
        break;
    }

    status = bt_gap_le_update_connection_parameter(&conn_params);
    if (BT_STATUS_SUCCESS == status) {
        buffer_t->low_power_t.conn_priority = conn_priority;
    }
    return status;
}


/**
 * @brief Function for getting the current connection interval.
 *
 * @param[in]  conn_updated  the information of BLE_GAP_CONNECTION_PARAM_UPDATED_IND event.
 *
 * @param[out] priority      the current connection interval.
 */
ble_air_conn_param_priority_t ble_air_get_current_connection_interval(uint16_t conn_handle, uint16_t conn_interval)
{
    LOG_MSGID_I(AIR, "ble_air_get_current_connection_interval, conn_interval %d\r\n", 1, conn_interval);
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);

    if (buffer_t) {
        if (buffer_t->low_power_t.remote_device_type == BLE_AIR_REMOTE_DEVICE_ANDROID) {
            if ((conn_interval >= 0x0006) && (conn_interval <= 0x000A)) {
                return BLE_AIR_CONN_PARAM_HIGH_SPEED_ANDROID;
            } else if ((conn_interval >= 0x0120) && (conn_interval <= 0x0130)) {
                return BLE_AIR_CONN_PARAM_LOW_POWER;
            }
        } else if (buffer_t->low_power_t.remote_device_type == BLE_AIR_REMOTE_DEVICE_IOS){
            if ((conn_interval > 0x000C) && (conn_interval <= 0x0020)) {
                return BLE_AIR_CONN_PARAM_HIGH_SPEED_IOS;
            } else if ((conn_interval >= 0x0120) && (conn_interval <= 0x0130)) {
                return BLE_AIR_CONN_PARAM_LOW_POWER;
            }
        }
    }
    return BLE_AIR_CONN_PARAM_PRIORITY_DEFAULT;
}

int32_t ble_air_update_connection_interval(uint16_t conn_handle)
{
    int32_t status = BT_STATUS_FAIL;
    ble_air_conn_param_priority_t interval = BLE_AIR_CONN_PARAM_PRIORITY_DEFAULT;
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);
    
    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t)) {
        if (BLE_AIR_REMOTE_DEVICE_IOS == buffer_t->low_power_t.remote_device_type) {
            interval = BLE_AIR_CONN_PARAM_HIGH_SPEED_IOS;
        } else {
            interval = BLE_AIR_CONN_PARAM_HIGH_SPEED_ANDROID;
        }
        
        status = ble_air_update_connection_interval_internal(conn_handle, interval);
        if (status == BT_STATUS_SUCCESS) {
            buffer_t->low_power_t.conn_priority = interval;
            #if 0
            LOG_MSGID_I(AIR, "xTimer_low_power:%d\r\n", 1, g_xTimer_low_power);

            if (g_xTimer_low_power == NULL) {
                LOG_MSGID_I(AIR, "AIR init create LOW_POWER_TIMER fail!\r\n", 0);
            } else {
                xTimerStart((TimerHandle_t)g_xTimer_low_power, 0);
            }
            #endif
        }
    }
    return status;

}


/**
 * @brief Function for application to set remote device's type, android or ios device.
 */
void ble_air_set_remote_device_type(uint16_t conn_handle, ble_air_remote_device_type_t type)
{
    ble_air_cntx_t *buffer_t = ble_air_get_cntx_by_handle(conn_handle);

    if ((conn_handle != BT_HANDLE_INVALID) && (buffer_t)) {
        buffer_t->low_power_t.remote_device_type = (uint8_t)type;
    }
    LOG_MSGID_I(AIR, "ble_air_set_remote_device_type: conn_handle is [%d], master_type is [%d]\r\n", 2, conn_handle, type);
}

#if 0
TimerHandle_t ble_air_create_timer(void) 
{
    return xTimerCreate("AIR_LOW_POWER_TIMER",
                        BLE_AIR_TIMER_PERIOD / portTICK_PERIOD_MS, pdFALSE,
                        ( void *)0,
                        ble_air_update_connection_interval_timerout);
}

void ble_air_delete_timer(TimerHandle_t timer_id)
{
    if (timer_id != NULL) {
        xTimerDelete(timer_id, 0);
        timer_id = NULL;
    }
}
#endif /*#if 0*/
#endif /*#if LOW_POWER*/

#endif /*#ifdef MTK_PORT_SERVICE_BT_ENABLE*/


