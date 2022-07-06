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
#include "race_cmd_feature.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hal_uart.h"
#include "hal_uart_internal.h"
#include "syslog.h"
#include "serial_port.h"
#include "bt_callback_manager.h"
#include "atci_adapter.h"
#include "atci_main.h"
#include "race_cmd.h"
#include "race_xport.h"
#include "race_bt.h"
#include "task_def.h"
#include "race_lpcomm_packet.h"
#include "race_timer.h"
#include "race_lpcomm_util.h"
#include "race_storage_util.h"
#include "race_noti.h"
#include "race_lpcomm_recv.h"
#include "race_lpcomm_ps_list.h"
#include "race_lpcomm_retry.h"
#include "race_util.h"
#include "race_event_internal.h"
#ifdef RACE_FOTA_CMD_ENABLE
#include "fota_multi_info.h"
#include "race_fota_util.h"
#include "race_fota.h"
#endif
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
#include "bt_role_handover.h"
#endif
#ifdef RACE_FIND_ME_ENABLE
#include "race_cmd_find_me.h"
#endif
#include "race_cmd_bluetooth.h"
#include "serial_port_assignment.h"
#include "bt_type.h"
#include "bt_connection_manager_internal.h"
#include "bt_spp.h"
#ifdef AIR_RACE_CO_SYS_ENABLE
#include "race_cmd_relay_cmd_cosys.h"
#include "race_cmd_nvdm.h"
#endif

#if defined(MTK_MUX_ENABLE)
#include "mux.h"
#include "mux_port.h"
#include "mux_bt.h"
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
#include "serial_port_iap2.h"
#endif

#ifdef MTK_USB_DEMO_ENABLED
#include "usb.h"
#include "usb_case.h"
#endif
#include "timers.h"

#ifdef RACE_CFU_ENABLE
#include "race_cfu_internal.h"
#endif


#if (defined AIR_DONGLE_RELAY_RACE_PACKET_ENABLE) || defined (AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
#include "usbaudio_drv.h"

typedef enum {
    RACE_REMOTE_HID_FLAG_LOCAL,
    RACE_REMOTE_HID_FLAG_SPP,
    RACE_REMOTE_HID_FLAG_BLE,
    RACE_REMOTE_HID_FLAG_COSYS,
    RACE_REMOTE_HID_FLAG_EMCU,
} race_remote_hid_flag_t;

#define RACE_ID_NO_REMOTE_RANGE_BEGIN 0x2E00
#define RACE_ID_NO_REMOTE_RANGE_END 0x2E1F
#endif

#ifdef AIR_RACE_CO_SYS_ENABLE
#define RACE_DUAL_CHIP_COSYS_PORT MUX_UART_2
#define RACE_DUAL_CHIP_EMCU_PORT  MUX_UART_1
#define RACE_COSYS_MUX_BUFF_SIZE  (2 * 1024)
#define RACE_EMCU_MUX_BUFF_SIZE   (1 * 1024)

extern void race_cosys_tx_timer_init();
extern void race_cosys_tx_timer_start(void);
extern bool race_cosys_tx_timer_is_alive();

extern void race_cosys_rx_irq(mux_handle_t handle, uint32_t data_len);
extern bool race_relay_send_cosys(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id, uint8_t relay_type);
#endif

#if defined(MTK_MUX_ENABLE)
uint32_t race_get_port_by_channel_id(uint8_t channel_id);
static mux_port_t race_muxid_from_portid(serial_port_dev_t port_id);

typedef struct {
    mux_port_t dev;
    bt_bd_addr_t addr;
} race_bt_conn_info_t;

race_bt_conn_info_t g_race_bt_connection[] = {
        {MUX_BT_SPP,              {0}},
        {MUX_BT_BLE,              {0}},
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        {MUX_BT_GATT_OVER_BREDR,  {0}},
#endif
        {MUX_BT_AIRUPATE,         {0}},
#ifdef MTK_IAP2_VIA_MUX_ENABLE
        {MUX_IAP2_SESSION2,       {0}},
#endif
};

#define RACE_BT_CONNECTION_NUM    (sizeof(g_race_bt_connection) / sizeof(race_bt_conn_info_t))

static race_bt_conn_info_t *race_find_bt_conn_info_by_port(mux_port_t dev)
{
    uint8_t i = 0;

    for (i = 0; i < RACE_BT_CONNECTION_NUM; i ++) {
        if (g_race_bt_connection[i].dev == dev) {
            return &g_race_bt_connection[i];
        }
    }

    return NULL;
}

static void race_set_bt_connection_addr(mux_port_t dev, bt_bd_addr_t* addr)
{
    race_bt_conn_info_t *info = race_find_bt_conn_info_by_port(dev);

    if (info == NULL) {
        RACE_LOG_MSGID_E("race_set_bt_connection_addr, dev %d not support", 1, dev);
        return;
    }

    if (addr) {
        RACE_LOG_MSGID_I("race_set_bt_connection_addr, dev %d, addr %x:%x:%x:%x:%x:%x", 7, dev,
        ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5]);
        memcpy(&(info->addr), addr, sizeof(bt_bd_addr_t));
    } else {
        RACE_LOG_MSGID_I("race_set_bt_connection_addr, dev %d, set 0", 1, dev);
        memset(&(info->addr), 0, sizeof(bt_bd_addr_t));
    }
}

bt_bd_addr_t *race_get_bt_connection_addr(uint8_t channel_id)
{
    serial_port_type_t serial_port = race_get_port_by_channel_id(channel_id);
    mux_port_t dev = race_muxid_from_portid(serial_port);

    race_bt_conn_info_t *info = race_find_bt_conn_info_by_port(dev);

    if (info == NULL) {
        RACE_LOG_MSGID_E("race_get_bt_connection_addr, dev %d(channel %d) not support", 2, dev, channel_id);
        return NULL;
    }

    RACE_LOG_MSGID_I("race_get_bt_connection_addr, dev %d(channel %d)", 2, dev, channel_id);
    return &(info->addr);
}


static mux_port_t race_muxid_from_portid(serial_port_dev_t port_id)
{
    switch(port_id){
        case SERIAL_PORT_DEV_UART_0:
            return MUX_UART_0;
        case SERIAL_PORT_DEV_UART_1:
            return MUX_UART_1;
        case SERIAL_PORT_DEV_UART_2:
            return MUX_UART_2;
        case SERIAL_PORT_DEV_UART_3:
            return MUX_UART_3;
#ifdef MTK_USB_DEMO_ENABLED
        case SERIAL_PORT_DEV_USB_COM1:
            return MUX_USB_COM_1;
        case SERIAL_PORT_DEV_USB_COM2:
            return MUX_USB_COM_2;
#endif
        case SERIAL_PORT_DEV_BT_SPP:
            return MUX_BT_SPP;
        case SERIAL_PORT_DEV_BT_LE:
            return MUX_BT_BLE;
#ifdef MTK_IAP2_PROFILE_ENABLE
        case SERIAL_PORT_DEV_IAP2_SESSION2:
            return MUX_IAP2_SESSION2;
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        case SERIAL_PORT_DEV_BT_GATT_OVER_BREDR:
            return MUX_BT_GATT_OVER_BREDR;
#endif
#ifdef MTK_AIRUPDATE_ENABLE
        case SERIAL_PORT_DEV_BT_AIRUPDATE:
            return MUX_BT_AIRUPATE;
#endif
        default:
            return 0xFF;
    }

    return 0xFF;
}

static serial_port_dev_t race_portid_from_muxid(mux_port_t mux_id)
{
    RACE_LOG_MSGID_I("race_portid_from_muxid, mux_id[%d]", 1, mux_id);
    switch (mux_id) {
        case MUX_UART_0:
            return SERIAL_PORT_DEV_UART_0;

        case MUX_UART_1:
            return SERIAL_PORT_DEV_UART_1;

        case MUX_UART_2:
            return SERIAL_PORT_DEV_UART_2;

        case MUX_UART_3:
            return SERIAL_PORT_DEV_UART_3;

#ifdef MTK_USB_DEMO_ENABLED
        case MUX_USB_COM_1:
            return SERIAL_PORT_DEV_USB_COM1;

        case MUX_USB_COM_2:
            return SERIAL_PORT_DEV_USB_COM2;
#endif
        case MUX_BT_SPP:
            return SERIAL_PORT_DEV_BT_SPP;

        case MUX_BT_BLE:
            return SERIAL_PORT_DEV_BT_LE;

#ifdef MTK_AIRUPDATE_ENABLE
        case MUX_BT_AIRUPATE:
            return SERIAL_PORT_DEV_BT_AIRUPDATE;
#endif

#ifdef MTK_IAP2_PROFILE_ENABLE
        case MUX_IAP2_SESSION2:
            return SERIAL_PORT_DEV_IAP2_SESSION2;
#endif

#ifdef MTK_GATT_OVER_BREDR_ENABLE
        case MUX_BT_GATT_OVER_BREDR:
            return SERIAL_PORT_DEV_BT_GATT_OVER_BREDR;
#endif
#ifdef AIR_RACE_SCRIPT_ENABLE
        case MUX_PORT_PSEUDO:
            return SERIAL_PORT_DEV_PSEUDO;
#endif
        default:
            return 0xFF;
    }
}


static RACE_ERRCODE race_convert_mux_event(serial_port_dev_t *device,
                                                   serial_port_callback_event_t *event,
                                                   void **parameter,
                                                   mux_handle_t handle,
                                                   mux_event_t mux_event,
                                                   uint32_t data_len,
                                                   void* user_data)
{
    UNUSED(parameter);
    UNUSED(data_len);
    UNUSED(user_data);

    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    if (!device || !event) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    *device = race_portid_from_muxid(handle & 0xFF);

    if (0xFF == *device) {
        return RACE_ERRCODE_NOT_SUPPORT;
    }

    switch (mux_event) {
        case MUX_EVENT_READY_TO_READ:
        case MUX_EVENT_TRANSPARENT_READ:
        {
            *event = SERIAL_PORT_EVENT_READY_TO_READ;
            break;
        }

        case MUX_EVENT_READY_TO_WRITE: {
            *event = SERIAL_PORT_EVENT_READY_TO_WRITE;
            break;
        }

        case MUX_EVENT_CONNECTION: {
            if (SERIAL_PORT_DEV_BT_SPP == *device ||
#ifdef MTK_AIRUPDATE_ENABLE
                SERIAL_PORT_DEV_BT_AIRUPDATE == *device ||
#endif
                SERIAL_PORT_DEV_BT_LE == *device) {
                *event = SERIAL_PORT_EVENT_BT_CONNECTION;
            }
#ifdef MTK_IAP2_PROFILE_ENABLE
            else if (SERIAL_PORT_DEV_IAP2_SESSION2 == *device) {
                *event = SERIAL_PORT_EVENT_IAP2_SESSION_OPEN;
            }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
            else if (SERIAL_PORT_DEV_BT_GATT_OVER_BREDR == *device) {
                *event = SERIAL_PORT_EVENT_BT_CONNECTION;
            }
#endif
#ifdef MTK_USB_DEMO_ENABLED
            else if (SERIAL_PORT_DEV_USB_COM1 == *device ||
                     SERIAL_PORT_DEV_USB_COM2 == *device) {
                *event = SERIAL_PORT_EVENT_USB_CONNECTION;
            }
#endif
            else {
                ret = RACE_ERRCODE_NOT_SUPPORT;
            }
            break;
        }

        case MUX_EVENT_DISCONNECTION: {
            if (SERIAL_PORT_DEV_BT_SPP == *device ||
#ifdef MTK_AIRUPDATE_ENABLE
                SERIAL_PORT_DEV_BT_AIRUPDATE == *device ||
#endif
                SERIAL_PORT_DEV_BT_LE == *device) {
                *event = SERIAL_PORT_EVENT_BT_DISCONNECTION;
            }
#ifdef MTK_IAP2_PROFILE_ENABLE
            else if (SERIAL_PORT_DEV_IAP2_SESSION2 == *device) {
                *event = SERIAL_PORT_EVENT_IAP2_SESSION_CLOSE;
            }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
            else if (SERIAL_PORT_DEV_BT_GATT_OVER_BREDR == *device) {
                *event = SERIAL_PORT_EVENT_BT_DISCONNECTION;
            }
#endif
#ifdef MTK_USB_DEMO_ENABLED
            else if (SERIAL_PORT_DEV_USB_COM1 == *device ||
                     SERIAL_PORT_DEV_USB_COM2 == *device) {
                *event = SERIAL_PORT_EVENT_USB_DISCONNECTION;
            }
#endif
            else {
                ret = RACE_ERRCODE_NOT_SUPPORT;
            }
            break;
        }

        case MUX_EVENT_WAKEUP_FROM_SLEEP: {
            *event = SERIAL_PORT_EVENT_WAKEUP_SLEEP;
            break;
        }

        case MUX_EVENT_TRANSMISSION_DONE: {
#ifdef MTK_SMART_CHARGER_1WIRE_RACE_ENABLE
            if (SERIAL_PORT_DEV_UART_1 == *device){
                log_hal_msgid_info("[SmartCharger][1Wire] MUX_EVENT_TRANSMISSION_DONE", 0);
                hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_GPIO4);
                hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_UART1_RXD);
            }
#endif
            *event = 255;//SERIAL_PORT_EVENT_TRANSMISSION_DONE;
            break;
        }

        default: {
            ret = RACE_ERRCODE_NOT_SUPPORT;
            break;
        }
    }
    RACE_LOG_MSGID_I("race_convert_mux_event, device[0x%X], event[%d]", 2, *device, *event);

    return ret;
}
#endif

#ifdef MTK_ONLINE_LOG_TO_APK_ENABLE
#include "hal_nvic.h"
#endif

log_create_module(race, PRINT_LEVEL_INFO);


typedef struct
{
    serial_port_dev_t serial_port;
    uint32_t serial_port_handle;
}race_serial_port_info;

#define RACE_SERIAL_PORT_INFO_INIT(a) \
do{\
    (a).serial_port = SERIAL_PORT_DEV_UNDEFINED;\
    (a).serial_port_handle = 0;\
}while(0)

typedef struct
{
    race_serial_port_info ble_port;
    race_serial_port_info spp_port;
    race_serial_port_info airupdate_port;
    race_serial_port_info iap2_port;
    race_serial_port_info uart_port;
    race_serial_port_info one_wire_port;
    race_serial_port_info usb_port;
    race_serial_port_info gatt_over_bredr_port;
#ifdef AIR_RACE_SCRIPT_ENABLE
    race_serial_port_info pseudo_port;
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    race_serial_port_info cosys_port;
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    race_serial_port_info emcu_port;
#endif
#endif
}race_serial_port_list;

uint8_t *auReceiveBuff = NULL;
race_serial_port_list *g_race_serial_port_list = NULL;
uint32_t g_race_serial_port_queue = 0;
uint32_t g_race_send_data_cache_left_len = 0;
uint32_t g_race_send_data_cache_len = 0;
uint8_t* g_race_send_data_cache = NULL;
serial_port_dev_t g_race_uart_port = SERIAL_PORT_DEV_UART_1;
serial_port_dev_t g_race_uart_port_default = SERIAL_PORT_DEV_UART_1;
uint32_t g_race_1wire_baudrate = CONFIG_RACE_BAUDRATE;
serial_port_dev_t g_race_usb_port = SERIAL_PORT_DEV_USB_COM1;
serial_port_dev_t g_race_usb_port_default = SERIAL_PORT_DEV_USB_COM1;
#ifdef RACE_CREATE_TASK_DYNAMICALLY
TaskHandle_t g_race_task_hdl;
#endif
race_init_ctrl_t g_race_init_ctrl;
#ifdef MTK_RACE_EVENT_ID_ENABLE
int32_t g_race_event_register_id = RACE_EVENT_INVALID_REGISTER_ID;
#endif

static uint8_t race_uart_rx_sleep_handle = 0xFF;
static TimerHandle_t race_sleep_lock_timer = NULL;
static bool race_sleep_lock_timer_alive = false;
TimerHandle_t race_usb_rx_clean_timer = NULL;
#ifdef MTK_USB_AUDIO_HID_ENABLE
static bool race_usb_rx_clean_timer_started = FALSE;
#endif
void race_start_sleep_lock_timer(void);
bt_status_t spp_air_get_tx_state(void);

race_input_cmd_msg_t uart_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
race_input_cmd_msg_t usb_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
race_input_cmd_msg_t ble_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
race_input_cmd_msg_t spp_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
race_input_cmd_msg_t airupdate_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
race_input_cmd_msg_t iap2_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
race_input_cmd_msg_t gatt_over_bredr_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
#ifdef AIR_RACE_SCRIPT_ENABLE
race_input_cmd_msg_t pseudo_port_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
race_input_cmd_msg_t cosys_port_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
race_input_cmd_msg_t emcu_port_msg_item = { SERIAL_PORT_DEV_UNDEFINED, 0, 0, true, NULL };
#endif
#endif

#define RACE_COMMAND_ERROR_CHECK \
   "It is RACE command mode, please check the command format!\r\n"

#define G_SET_UART_PORT(u) (g_race_serial_port_list->uart_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_UART_PORT() (g_race_serial_port_list->uart_port.serial_port)
#define G_SET_UART_PORT_HANDLE(u) (g_race_serial_port_list->uart_port.serial_port_handle = ((uint32_t)u))
#define G_GET_UART_PORT_HANDLE() (g_race_serial_port_list->uart_port.serial_port_handle)

#define G_SET_1WIRE_PORT(u) (g_race_serial_port_list->one_wire_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_1WIRE_PORT() (g_race_serial_port_list->one_wire_port.serial_port)
#define G_SET_1WIRE_PORT_HANDLE(u) (g_race_serial_port_list->one_wire_port.serial_port_handle = ((uint32_t)u))
#define G_GET_1WIRE_PORT_HANDLE() (g_race_serial_port_list->one_wire_port.serial_port_handle)

#define G_SET_USB_PORT(u) (g_race_serial_port_list->usb_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_USB_PORT() (g_race_serial_port_list->usb_port.serial_port)
#define G_SET_USB_PORT_HANDLE(u) (g_race_serial_port_list->usb_port.serial_port_handle = ((uint32_t)u))
#define G_GET_USB_PORT_HANDLE() (g_race_serial_port_list->usb_port.serial_port_handle)

#define G_SET_BLE_PORT(u) (g_race_serial_port_list->ble_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_BLE_PORT() (g_race_serial_port_list->ble_port.serial_port)
#define G_SET_BLE_PORT_HANDLE(u) (g_race_serial_port_list->ble_port.serial_port_handle = ((uint32_t)u))
#define G_GET_BLE_PORT_HANDLE() (g_race_serial_port_list->ble_port.serial_port_handle)

#define G_SET_SPP_PORT(u) (g_race_serial_port_list->spp_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_SPP_PORT() (g_race_serial_port_list->spp_port.serial_port)
#define G_SET_SPP_PORT_HANDLE(u) (g_race_serial_port_list->spp_port.serial_port_handle = ((uint32_t)u))
#define G_GET_SPP_PORT_HANDLE() (g_race_serial_port_list->spp_port.serial_port_handle)

#define G_SET_AIRUPDATE_PORT(u) (g_race_serial_port_list->airupdate_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_AIRUPDATE_PORT() (g_race_serial_port_list->airupdate_port.serial_port)
#define G_SET_AIRUPDATE_PORT_HANDLE(u) (g_race_serial_port_list->airupdate_port.serial_port_handle = ((uint32_t)u))
#define G_GET_AIRUPDATE_PORT_HANDLE() (g_race_serial_port_list->airupdate_port.serial_port_handle)

#define G_SET_IAP2_PORT(u) (g_race_serial_port_list->iap2_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_IAP2_PORT() (g_race_serial_port_list->iap2_port.serial_port)
#define G_SET_IAP2_PORT_HANDLE(u) (g_race_serial_port_list->iap2_port.serial_port_handle = ((uint32_t)u))
#define G_GET_IAP2_PORT_HANDLE() (g_race_serial_port_list->iap2_port.serial_port_handle)

#define G_SET_GATT_OVER_BREDR_PORT(u) (g_race_serial_port_list->gatt_over_bredr_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_GATT_OVER_BREDR_PORT() (g_race_serial_port_list->gatt_over_bredr_port.serial_port)
#define G_SET_GATT_OVER_BREDR_PORT_HANDLE(u) (g_race_serial_port_list->gatt_over_bredr_port.serial_port_handle = ((uint32_t)u))
#define G_GET_GATT_OVER_BREDR_PORT_HANDLE() (g_race_serial_port_list->gatt_over_bredr_port.serial_port_handle)

#ifdef AIR_RACE_SCRIPT_ENABLE
#define G_SET_PSEUDO_PORT(u) (g_race_serial_port_list->pseudo_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_PSEUDO_PORT() (g_race_serial_port_list->pseudo_port.serial_port)
#define G_SET_PSEUDO_PORT_HANDLE(u) (g_race_serial_port_list->pseudo_port.serial_port_handle = ((uint32_t)u))
#define G_GET_PSEUDO_PORT_HANDLE() (g_race_serial_port_list->pseudo_port.serial_port_handle)
#endif

#ifdef AIR_RACE_CO_SYS_ENABLE
#define G_SET_COSYS_PORT(u) (g_race_serial_port_list->cosys_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_COSYS_PORT() (g_race_serial_port_list->cosys_port.serial_port)
#define G_SET_COSYS_PORT_HANDLE(u) (g_race_serial_port_list->cosys_port.serial_port_handle = ((uint32_t)u))
#define G_GET_COSYS_PORT_HANDLE() (g_race_serial_port_list->cosys_port.serial_port_handle)

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
#define G_SET_EMCU_PORT(u) (g_race_serial_port_list->emcu_port.serial_port = ((serial_port_dev_t)u))
#define G_GET_EMCU_PORT() (g_race_serial_port_list->emcu_port.serial_port)
#define G_SET_EMCU_PORT_HANDLE(u) (g_race_serial_port_list->emcu_port.serial_port_handle = ((uint32_t)u))
#define G_GET_EMCU_PORT_HANDLE() (g_race_serial_port_list->emcu_port.serial_port_handle)
#endif

#endif

#define STRNCPY(dest, source) strncpy(dest, source, strlen(source)+1);

race_status_t race_send_data(uint32_t port_handle, uint8_t* data, uint32_t data_len);

#ifdef RACE_FOTA_ADAPTIVE_MODE_ENABLE
void race_get_device_busy_status_default(bool *is_busy)
{
    if (is_busy) {
        *is_busy = FALSE;
    }
}

#pragma weak race_get_device_busy_status=race_get_device_busy_status_default

inline static void race_cmd_post_process(race_pkt_t *race_cmd_ptr)
{
    if (race_cmd_ptr && RACE_APP_ID_FOTA == race_cmd_ptr->hdr.pktId.field.app_id &&
        race_fota_is_adaptive_mode() &&
        (RACE_TYPE_RESPONSE == race_cmd_ptr->hdr.type ||
         RACE_TYPE_NOTIFICATION == race_cmd_ptr->hdr.type)) {
        uint8_t *status_ptr = (uint8_t *)race_cmd_ptr + 6;
        bool is_busy = FALSE;
#ifdef RACE_CMDRELAY_PASS_TO_DST
        if (RACE_TYPE_NOTIFICATION == race_cmd_ptr->hdr.type &&
            RACE_CMDRELAY_PASS_TO_DST == race_cmd_ptr->hdr.id) {
            /* Length includes ID and payload. Add 2 bytes for ID and another 2 bytes for RACE_CMDRELAY_PASS_TO_DST NOTI structure. */
            if (race_cmd_ptr->hdr.length > sizeof(race_pkt_t) + 4) {
                race_pkt_t relayed_race_cmd;

                memcpy(&relayed_race_cmd, race_cmd_ptr->payload + 2, sizeof(race_pkt_t));
                if (RACE_APP_ID_FOTA == relayed_race_cmd.hdr.pktId.field.app_id &&
                    (RACE_TYPE_RESPONSE == relayed_race_cmd.hdr.type ||
                     RACE_TYPE_NOTIFICATION == relayed_race_cmd.hdr.type)) {
                    status_ptr = race_cmd_ptr->payload + 8;
                } else {
                    return;
                }
            } else {
                return;
            }
        }
#endif
        race_get_device_busy_status(&is_busy);
        if (is_busy) {
            (*status_ptr) = (*status_ptr) | 0x80;
        }
    }
}
#endif


#if defined(MTK_RACE_CMD_ENABLE)

void* race_mem_alloc(uint32_t size)
{
    void *pvReturn = NULL;
    uint32_t  free_size;
    free_size = xPortGetFreeHeapSize();
    if (free_size > size) {
        pvReturn = pvPortMalloc(size);
    }

    return pvReturn;
}

void race_mem_free(void *buf)
{
    RACE_LOG_MSGID_I("race_mem_free, buf[0x%X]", 1, buf);
    if (buf)
    {
        vPortFree(buf);
        buf = NULL;
    }
}

uint32_t race_serial_port_send_data(uint32_t port_handle, uint8_t* buf, uint32_t buf_len)
{
    if(!port_handle)
    {
        RACE_LOG_MSGID_E("race_serial_port_send_data fail, port_handle NULL", 0);
        return 0xFFFF;
    }
    uint32_t data_len = 0;
    serial_port_status_t status = SERIAL_PORT_STATUS_FAIL;
    //RACE_LOG_MSGID_I("race_serial_port_send_data, port_handle[0x%X], buf_len[%d]", 2, port_handle, buf_len);

#if defined(MTK_MUX_ENABLE)
    mux_status_t mux_status = MUX_STATUS_ERROR;

    if((race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_UART)
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_1WIRE
#ifdef MTK_USB_DEMO_ENABLED
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_USB
#endif
#if defined(MTK_MUX_BT_ENABLE)
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_SPP
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_BLE
#ifdef MTK_AIRUPDATE_ENABLE
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_AIRUPDATE
#endif
#endif
#if defined(MTK_IAP2_VIA_MUX_ENABLE)
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_IAP2
#endif
#if defined(MTK_GATT_OVER_BREDR_ENABLE)
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR
#endif
#ifdef AIR_RACE_SCRIPT_ENABLE
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_PSEUDO
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_COSYS
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_EMCU
#endif
#endif
    )
    {
        if (race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_IAP2)
        {
            uint16_t iap2_session_id;
            mux_ctrl_para_t para;
            mux_control(G_GET_IAP2_PORT_HANDLE(), MUX_CMD_GET_CONNECTION_PARAM, &para);
            RACE_LOG_MSGID_I("race_serial_port_send_data, current_sesion_num[%d], session_id0[%X], session_id1[%X], session_id2[%X]",
                4, para.mux_get_connection_param.iap2_session_num, para.mux_get_connection_param.iap2_session_id[0],
                para.mux_get_connection_param.iap2_session_id[1], para.mux_get_connection_param.iap2_session_id[2]);
            //iap2_session_id = p_para->mux_get_connection_param.iap2_session_id;//?
            //iap2_session_id = (iap2_session_id >> 8) + ((iap2_session_id & 0xFF) << 8);
            iap2_session_id = para.mux_get_connection_param.iap2_session_id[0];
            if (iap2_session_id != 0)
            {
                uint8_t *p_malloc_buff_addr;
                p_malloc_buff_addr = pvPortMalloc(buf_len+2);
                if (p_malloc_buff_addr == NULL) {
                    assert(0);
                }
                memcpy((void*)p_malloc_buff_addr,(void*)(&iap2_session_id),2);
                memcpy((void*)p_malloc_buff_addr+2,(void*)(buf),buf_len);

                mux_buffer_t mux_buffer_iap2[] = {
                    {p_malloc_buff_addr, buf_len+2}
                };
                mux_status = mux_tx(port_handle, (mux_buffer_t *)&mux_buffer_iap2, 1, &data_len);
                data_len -= 2;

                vPortFree(p_malloc_buff_addr);
            }
        }
        else
        {
            if(race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_1WIRE)
            {
#ifdef MTK_SMART_CHARGER_1WIRE_RACE_ENABLE
                log_hal_msgid_info("[SmartCharger][1Wire] send race event", 0);
                hal_pinmux_set_function(HAL_GPIO_4, HAL_GPIO_4_UART1_TXD);
                hal_pinmux_set_function(HAL_GPIO_5, HAL_GPIO_5_GPIO5);
#endif
                mux_buffer_t mux_buffer = {buf, buf_len};
                mux_status = mux_tx(port_handle, &mux_buffer, 1, &data_len);
            }
            else if (race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_SPP)
            {
                mux_buffer_t mux_buffer = {buf, buf_len};
                mux_status = mux_tx(port_handle, &mux_buffer, 1, &data_len);
                mux_get_trx_status_t para;
                mux_status_t mux_tx_status = mux_control(MUX_BT_SPP, MUX_CMD_GET_TX_SEND_STATUS, (mux_ctrl_para_t *)&para);
                //mux_status_t mux_tx_status = port_mux_bt_control(MUX_BT_SPP, MUX_CMD_GET_TX_SEND_STATUS, &para);
                if (mux_tx_status == MUX_STATUS_OK && mux_status == MUX_STATUS_OK)
                {
                    data_len = para.transfer_completed_size;

                    if (para.tx_send_status != MUX_STATUS_OK)
                    {
                        uint32_t count = 0;
                        bool race_spp_connected = false;
                        bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
                        count = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS),
                                                            addr_list, BT_SINK_SRV_CM_MAX_DEVICE_NUMBER);
                        while(count > 0) {
                            RACE_LOG_MSGID_I("race_serial_port_send_data check remote_addr %x:%x:%x:%x:%x:%x",
                                    6, addr_list[count - 1][5], addr_list[count - 1][4], addr_list[count - 1][3],
                                    addr_list[count - 1][2], addr_list[count - 1][1], addr_list[count - 1][0]);
                            uint32_t spp_handle = bt_spp_get_handle_by_local_server_id((const bt_bd_addr_t *)&addr_list[count - 1], 0x15);
                            if (spp_handle != BT_SPP_INVALID_HANDLE) {
                                RACE_LOG_MSGID_I("race_serial_port_send_data spp handle %x", 1, spp_handle);
                                race_spp_connected = true;
                                break;
                            } else {
                                count --;
                            }
                        }
                        if (!race_spp_connected) {
                            RACE_LOG_MSGID_E("race_serial_port_send_data spp not connected", 0);
                            return 0xFFFF;
                        }

                        data_len |= (1<<31);
                    }
                    RACE_LOG_MSGID_I("race_serial_port_send_data spp, mux_tx_status[%d], data_len[%u]", 2, para.tx_send_status, data_len);
                }
                else
                {
                    RACE_LOG_MSGID_E("race_serial_port_send_data spp fail, mux_status[%d], mux_tx_status[%d]", 2, mux_status, mux_tx_status);
                }
            }
            else
            {

            //RACE_LOG_MSGID_E("race mux_tx buf_len %d, data: %x %x %x %x %x %x", 7, buf_len,buf[0], buf[1],buf[2],buf[3],buf[4],buf[5]);
            mux_buffer_t mux_buffer = {buf, buf_len};
            mux_status = mux_tx(port_handle, &mux_buffer, 1, &data_len);
        }
        }

        if(mux_status == MUX_STATUS_OK)
            status = SERIAL_PORT_STATUS_OK;
        else
            RACE_LOG_MSGID_E("race_serial_port_send_data, mux_tx fail, mux_status[%d]", 1, mux_status);
    }
    else
    {
        RACE_LOG_MSGID_E("race_serial_port_send_data fail, port_handle[0x%X]", 1, port_handle);
        return 0xFFFF;
    }
#else
    serial_port_write_data_t send_data;
    if((race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_UART)
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_USB
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_SPP
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_BLE
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_AIRUPDATE
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_IAP2
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR)
    {
        if (race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_IAP2)
        {
        #ifdef MTK_IAP2_PROFILE_ENABLE
            uint8_t *p_data;
            uint16_t iap2_session_id;
            serial_port_get_connection_param_t para_data;

            p_data = (uint8_t *)malloc(buf_len + 2);
            memcpy(p_data+2, buf, buf_len);
            status = serial_port_control(port_handle, SERIAL_PORT_CMD_GET_CONNECTION_PARAM, (serial_port_get_connection_param_t*)&para_data);
            iap2_session_id = para_data.iap2_session_id[0];
            memcpy(p_data, &iap2_session_id, 2);

            RACE_LOG_MSGID_I("race_serial_port_send_data, port service with iap2, status[%d], iap2_session_id[%d]", 2,
                status, iap2_session_id);

            if (iap2_session_id != SERIAL_PORT_IAP2_INVALID_SESSION_ID)
            {
                send_data.data = p_data;
                send_data.size = buf_len + 2;
                status = serial_port_control(port_handle, SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t*)&send_data);
                data_len = send_data.ret_size;
                data_len -= 2;
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_send_data, iap2_session_id fail, iap2_session_id[%d]", 1, iap2_session_id);
                assert(0);
            }
            free(p_data);
        #endif
        }
        else
        {
            send_data.data = buf;
            send_data.size = buf_len;
            status = serial_port_control(port_handle, SERIAL_PORT_CMD_WRITE_DATA, (serial_port_ctrl_para_t*)&send_data);
            data_len = send_data.ret_size;
        }
    }
    else
    {
        RACE_LOG_MSGID_E("race_serial_port_send_data fail, port_handle[0x%X]", 1, port_handle);
        return 0xFFFF;
    }
#endif
    if (status != SERIAL_PORT_STATUS_OK)
    {
        RACE_LOG_MSGID_E("race_serial_port_send_data fail, status[0x%X]", 1, status);
        data_len = 0;
    }

    return data_len;
}


uint32_t race_port_send_data_imp(uint32_t port_handle, uint8_t* buf, uint32_t buf_len)
{
    uint32_t send_len = 0, time_start = 0, time_curr = 0;
    uint8_t *ptr = buf;
    uint32_t left_len = buf_len, err_code = 0;

#ifdef RACE_FOTA_ADAPTIVE_MODE_ENABLE
    race_cmd_post_process((race_pkt_t *)ptr);
#endif

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_start);
    while(1)
    {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &time_curr);
#if (defined(MTK_ATCI_VIA_PORT_SERVICE) || defined(MTK_ATCI_VIA_MUX)) && defined(MTK_PORT_SERVICE_ENABLE)
        send_len = race_serial_port_send_data(port_handle, ptr, left_len);
#else
        send_len = hal_uart_send_dma((hal_uart_port_t)port_handle, ptr, left_len);
#endif
        if (send_len == 0xFFFF)
        {
            RACE_LOG_MSGID_E("race_port_send_data fail, port_handle[0x%X]", 1, port_handle);
            left_len = 0;
            break;
        }

        err_code = send_len & (1<<31);
        send_len &= ~(1<<31);

        left_len -= send_len;

        if (left_len == 0)
        {
            RACE_LOG_MSGID_I("race_port_send_data, ok", 0);
            break;
        }
        else if ((time_curr - time_start) > 100000)
        {
            RACE_LOG_MSGID_E("race_port_send_data fail timeout, left_len[%d]", 1, left_len);
            break;
        }
        else
        {
            ptr += send_len;
        }
    }

    buf_len -= left_len;
    buf_len |= err_code;

    return buf_len;
}


uint32_t race_port_send_data(uint32_t port_handle, uint8_t* buf, uint32_t buf_len)
{
#ifdef AIR_RACE_CO_SYS_ENABLE

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    if (port_handle == G_GET_COSYS_PORT_HANDLE() || port_handle == G_GET_EMCU_PORT_HANDLE()) {
#else
    if (port_handle == G_GET_COSYS_PORT_HANDLE()) {
#endif
        race_general_msg_t msg_queue_item = {0};
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_WRITE_CMD_IND;
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
        msg_queue_item.dev_t = (port_handle == G_GET_COSYS_PORT_HANDLE()) ? G_GET_COSYS_PORT(): G_GET_EMCU_PORT();
#else
        msg_queue_item.dev_t = G_GET_COSYS_PORT();
#endif
        msg_queue_item.msg_data = race_mem_alloc(sizeof(uint32_t) + buf_len);
        race_send_output_cmd_msg_t *data = (race_send_output_cmd_msg_t *)msg_queue_item.msg_data;
        if (data == NULL) {
            RACE_LOG_MSGID_E("race_port_send_data alloc fail", 0);
            return 0;
        }
        data->output_len = buf_len;
        memcpy(data->buff, buf, buf_len);
        race_send_msg(&msg_queue_item);
        return buf_len;
    }
#endif

    return race_port_send_data_imp(port_handle, buf, buf_len);
}


uint32_t race_serial_port_read_data(serial_port_handle_t port_handle, uint8_t *buf, uint32_t buf_len)
{
    //serial_port_get_read_avail_t read_data2;
    uint32_t data_len = 0;

    //status = serial_port_control(handle, SERIAL_PORT_CMD_GET_READ_AVAIL , (serial_port_ctrl_para_t *)&read_data2);
    //RACE_LOG_MSGID_I("race_serial_port_read_data, receive data Length[%ld]", 1, read_data2.ret_size);
#if defined(MTK_MUX_ENABLE)
    mux_status_t mux_status = MUX_STATUS_ERROR;

    if((race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_UART)
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_1WIRE
#ifdef MTK_USB_DEMO_ENABLED
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_USB
#endif
#if defined(MTK_MUX_BT_ENABLE)
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_SPP
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_BLE
#ifdef MTK_AIRUPDATE_ENABLE
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_AIRUPDATE
#endif
#endif
#if defined(MTK_IAP2_VIA_MUX_ENABLE)
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_IAP2
#endif
#if defined(MTK_GATT_OVER_BREDR_ENABLE)
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR
#endif
#ifdef AIR_RACE_SCRIPT_ENABLE
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_PSEUDO
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_COSYS
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
            || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_EMCU
#endif
    )
    {
        mux_buffer_t mux_buffer={buf, buf_len};
        mux_status = mux_rx(port_handle, &mux_buffer, &data_len);
        RACE_LOG_MSGID_I("race_serial_port_read_data, mux_rx, status[%d], data_len[%d]", 2, mux_status, data_len);
    }
#else
    serial_port_read_data_t read_data;
    serial_port_status_t status = SERIAL_PORT_STATUS_FAIL;
    if((race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_UART)
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_USB
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_SPP
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_BLE
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_AIRUPDATE
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_IAP2
        || race_get_channel_id_by_port_handle(port_handle) == RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR)
    {
        read_data.buffer = (uint8_t*)buf;
        read_data.size = buf_len;//read_data2.ret_size;
        status = serial_port_control(port_handle, SERIAL_PORT_CMD_READ_DATA, (serial_port_ctrl_para_t*)&read_data);

        if (status != SERIAL_PORT_STATUS_OK)
        {
            RACE_LOG_MSGID_E("race_serial_port_read_data() serial_port_control read fail", 0);
            return data_len;
        }
        data_len = read_data.ret_size;
    }
#endif
    return data_len;
}

uint32_t race_port_read_data(uint32_t port_handle, uint8_t *buf, uint32_t buf_len)
{
    uint32_t data_len = 0;

#if defined(MTK_PORT_SERVICE_ENABLE)
    data_len = race_serial_port_read_data(port_handle, buf, buf_len);
#else
    data_len = hal_uart_receive_dma((hal_uart_port_t)port_handle, buf, buf_len);
#endif
    RACE_LOG_MSGID_I("race_port_read_data, port_handle[0x%X], data_len[%d]", 2, port_handle, data_len);

    return data_len;
}

race_status_t race_serial_uart_port_check(serial_port_dev_t port)
{
#ifdef AIR_RACE_CO_SYS_ENABLE
    if (port == G_GET_COSYS_PORT()) {
        return RACE_STATUS_ERROR;
    }
#endif

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    if (port == G_GET_EMCU_PORT()) {
        return RACE_STATUS_ERROR;
    }
#endif

    if ((port >= SERIAL_PORT_DEV_UART_TYPE_BEGIN) && (port <= SERIAL_PORT_DEV_UART_TYPE_END))
    {
        return RACE_STATUS_OK;
    }
    return RACE_STATUS_ERROR;
}

race_status_t race_serial_usb_port_check(serial_port_dev_t port)
{
    if ((port >= SERIAL_PORT_DEV_USB_TYPE_BEGIN) && (port <= SERIAL_PORT_DEV_USB_TYPE_END))
    {
        return RACE_STATUS_OK;
    }
    return RACE_STATUS_ERROR;
}

void GET_INPUT_CMD_MSG_PTR(serial_port_dev_t port, race_input_cmd_msg_t **ptr)
{
    if (race_serial_uart_port_check((port)) == RACE_STATUS_OK)
    {
        (*ptr) = &uart_msg_item;
    }
    else if (race_serial_usb_port_check((port)) == RACE_STATUS_OK)
    {
        (*ptr) = &usb_msg_item;
    }
    else if (port == SERIAL_PORT_DEV_BT_LE)
    {
        (*ptr) = &ble_msg_item;
    }
    else if (port == SERIAL_PORT_DEV_BT_SPP)
    {
        (*ptr) = &spp_msg_item;
    }
#ifdef MTK_AIRUPDATE_ENABLE
    else if (port == SERIAL_PORT_DEV_BT_AIRUPDATE)
    {
        (*ptr) = &airupdate_msg_item;
    }
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
    else if (port == SERIAL_PORT_DEV_IAP2_SESSION2)
    {
        (*ptr) = &iap2_msg_item;
    }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    else if (port == SERIAL_PORT_DEV_BT_GATT_OVER_BREDR)
    {
        (*ptr) = &gatt_over_bredr_msg_item;
    }
#endif
#ifdef AIR_RACE_SCRIPT_ENABLE
    else if (port == SERIAL_PORT_DEV_PSEUDO)
    {
        (*ptr) = &pseudo_port_msg_item;
    }
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    else if (port == G_GET_COSYS_PORT())
    {
        (*ptr) = &cosys_port_msg_item;
    }
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    else if (port == G_GET_EMCU_PORT())
    {
        (*ptr) = &emcu_port_msg_item;
    }
#endif
    return;
}

uint32_t race_get_serial_port_handle(serial_port_dev_t port)
{
    if (race_serial_uart_port_check(port) == RACE_STATUS_OK)
    {
        if (port == G_GET_1WIRE_PORT())
            return G_GET_1WIRE_PORT_HANDLE();
        else
            return G_GET_UART_PORT_HANDLE();
    }
    else if (race_serial_usb_port_check(port) == RACE_STATUS_OK)
    {
        return G_GET_USB_PORT_HANDLE();
    }
    else if (port == SERIAL_PORT_DEV_BT_LE)
    {
        return G_GET_BLE_PORT_HANDLE();
    }
    else if (port == SERIAL_PORT_DEV_BT_SPP)
    {
        return G_GET_SPP_PORT_HANDLE();
    }
#ifdef MTK_AIRUPDATE_ENABLE
    else if (port == SERIAL_PORT_DEV_BT_AIRUPDATE)
    {
        return G_GET_AIRUPDATE_PORT_HANDLE();
    }
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
    else if (port == SERIAL_PORT_DEV_IAP2_SESSION2)
    {
        return G_GET_IAP2_PORT_HANDLE();
    }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    else if (port == SERIAL_PORT_DEV_BT_GATT_OVER_BREDR)
    {
        return G_GET_GATT_OVER_BREDR_PORT_HANDLE();
    }
#endif
#ifdef AIR_RACE_SCRIPT_ENABLE
    else if (port == SERIAL_PORT_DEV_PSEUDO)
    {
        return G_GET_PSEUDO_PORT_HANDLE();
    }
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    else if (port == G_GET_COSYS_PORT())
    {
        return G_GET_COSYS_PORT_HANDLE();
    }
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    else if (port == G_GET_EMCU_PORT())
    {
        return G_GET_EMCU_PORT_HANDLE();
    }
#endif
    else
        return 0;
}

void race_alloc_serial_port_list(race_serial_port_list **ptr)
{
    if (ptr)
    {
        *ptr = (race_serial_port_list *)race_mem_alloc(sizeof(race_serial_port_list));
        if (*ptr)
        {
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->ble_port);
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->spp_port);
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->airupdate_port);
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->iap2_port);
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->uart_port);
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->one_wire_port);
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->usb_port);
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->gatt_over_bredr_port);
#ifdef AIR_RACE_SCRIPT_ENABLE
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->pseudo_port);
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->cosys_port);
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
            RACE_SERIAL_PORT_INFO_INIT((*ptr)->emcu_port);
#endif
        }
    }
}

void race_serial_port_data_callback(serial_port_dev_t device, serial_port_callback_event_t event, void *parameter)
{
    race_general_msg_t msg_queue_item = {0};
    //uint16_t         queue_msg_num = 0;

    RACE_LOG_MSGID_I("race_serial_port_data_callback, device[%d], event[%d]", 2, device, event);

    switch (event)
    {
    case SERIAL_PORT_EVENT_READY_TO_READ:
    {
        RACE_LOG_MSGID_I("race_serial_port_data_callback, SERIAL_PORT_EVENT_READY_TO_READ", 0);
        race_start_sleep_lock_timer();
#ifdef AIR_DONGLE_RELAY_RACE_PACKET_ENABLE
        uint8_t usb_mux_flag = 0;
#if defined(MTK_USB_DEMO_ENABLED) && defined(MTK_USB_AUDIO_HID_ENABLE)
        extern uint8_t usb_mux_get_data_flag(void);
        usb_mux_flag = usb_mux_get_data_flag();

        if ((SERIAL_PORT_DEV_USB_TYPE_BEGIN <= device)
            && (SERIAL_PORT_DEV_USB_TYPE_END >= device)
            && (TARGET_REMOTE_DEVICE == usb_mux_flag)) {
#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
            msg_queue_item.msg_flag = RACE_REMOTE_HID_FLAG_BLE;
#else
            msg_queue_item.msg_flag = RACE_REMOTE_HID_FLAG_SPP; /* usb race cmd for remote */
#endif
        } else {
            msg_queue_item.msg_flag = RACE_REMOTE_HID_FLAG_LOCAL; /* local race cmd/response/notification */
        }
#endif
        RACE_LOG_MSGID_I("race_serial_port_data_callback, msg_flag[0x%x]", 1, msg_queue_item.msg_flag);
#elif (defined AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
        uint8_t usb_mux_flag = 0;
#if defined(MTK_USB_DEMO_ENABLED) && defined(MTK_USB_AUDIO_HID_ENABLE)
        extern uint8_t usb_mux_get_data_flag(void);
        usb_mux_flag = usb_mux_get_data_flag();

        msg_queue_item.msg_flag = RACE_REMOTE_HID_FLAG_LOCAL;
        if ((SERIAL_PORT_DEV_USB_TYPE_BEGIN <= device)
            && (SERIAL_PORT_DEV_USB_TYPE_END >= device)) {
            if (TARGET_REMOTE_DEVICE == usb_mux_flag) {
                msg_queue_item.msg_flag = RACE_REMOTE_HID_FLAG_COSYS; /* usb race cmd for remote */
            } else if (TARGET_EXTERNAL_DEVICE == usb_mux_flag) {
                msg_queue_item.msg_flag = RACE_REMOTE_HID_FLAG_EMCU; /* usb race cmd for external mcu */
            }
        }
#endif
        RACE_LOG_MSGID_I("race_serial_port_data_callback, msg_flag[0x%x]", 1, msg_queue_item.msg_flag);
#endif
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_SEND_CMD_IND;
        msg_queue_item.dev_t = device;
        msg_queue_item.msg_data = NULL;
        race_send_msg(&msg_queue_item);
        break;
    }

    case SERIAL_PORT_EVENT_READY_TO_WRITE:
    {
        RACE_LOG_MSGID_I("race_serial_port_data_callback, SERIAL_PORT_EVENT_READY_TO_WRITE", 0);
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_WRITE_CMD_IND;
        msg_queue_item.dev_t = device;
        msg_queue_item.msg_data = NULL;
        race_send_msg(&msg_queue_item);
        break;
    }

    case SERIAL_PORT_EVENT_WAKEUP_SLEEP:
    {
        RACE_LOG_MSGID_I("race_serial_port_data_callback, SERIAL_PORT_EVENT_WAKEUP_SLEEP", 0);
        race_start_sleep_lock_timer();
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_WAKEUP_SLEEP_IND;
        msg_queue_item.dev_t = device;
        msg_queue_item.msg_data = NULL;
        race_send_msg(&msg_queue_item);
        break;
    }

    case SERIAL_PORT_EVENT_BT_DISCONNECTION:
    {
        if (SERIAL_PORT_DEV_BT_LE == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_BLE_DISCONNECT, NULL);
        }

        if (SERIAL_PORT_DEV_BT_SPP == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_SPP_DISCONNECT, NULL);
        }

#ifdef MTK_AIRUPDATE_ENABLE
        if (SERIAL_PORT_DEV_BT_AIRUPDATE == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_AIRUPDATE_DISCONNECT, NULL);
        }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        if (SERIAL_PORT_DEV_BT_GATT_OVER_BREDR == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_DISCONNECT, NULL);
        }
#endif

#ifdef MTK_MUX_ENABLE
        RACE_LOG_MSGID_I("race_serial_port_data_callback, clean", 0);
        mux_control(race_muxid_from_portid(device), MUX_CMD_CLEAN, NULL);
#endif
        break;
    }

    case SERIAL_PORT_EVENT_BT_CONNECTION:
    {
        if (SERIAL_PORT_DEV_BT_LE == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_BLE_CONNECT, NULL);
        }

        if (SERIAL_PORT_DEV_BT_SPP == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_SPP_CONNECT, NULL);
        }

#ifdef MTK_AIRUPDATE_ENABLE
        if (SERIAL_PORT_DEV_BT_AIRUPDATE == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_AIRUPDATE_CONNECT, NULL);
        }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        if (SERIAL_PORT_DEV_BT_GATT_OVER_BREDR == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_CONNECT, NULL);
        }
#endif
        break;
    }

#ifdef MTK_IAP2_PROFILE_ENABLE
    case SERIAL_PORT_EVENT_IAP2_SESSION_OPEN:
    {
        if (SERIAL_PORT_DEV_IAP2_SESSION2 == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_IAP2_CONNECT, NULL);
        }

        break;
    }

    case SERIAL_PORT_EVENT_IAP2_SESSION_CLOSE:
    {
        if (SERIAL_PORT_DEV_IAP2_SESSION2 == device)
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_IAP2_DISCONNECT, NULL);
        }
#ifdef MTK_MUX_ENABLE
        mux_control(race_muxid_from_portid(device), MUX_CMD_CLEAN, NULL);
#endif
        break;
    }
#endif

#ifdef MTK_USB_DEMO_ENABLED
    case SERIAL_PORT_EVENT_USB_CONNECTION:
    {
        if ((SERIAL_PORT_DEV_USB_COM1 == device) || (SERIAL_PORT_DEV_USB_COM2 == device))
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_USB_CONNECT, NULL);
        }
        break;
    }
    case SERIAL_PORT_EVENT_USB_DISCONNECTION:
    {
        if ((SERIAL_PORT_DEV_USB_COM1 == device) || (SERIAL_PORT_DEV_USB_COM2 == device))
        {
            /* The current API is running in the port_service task. */
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_USB_DISCONNECT, NULL);
        }
        break;
    }
#endif
    default:
        break;
    }

}

#if defined(MTK_MUX_ENABLE)
typedef enum {
    mux_buffer_idx_uart,
    mux_buffer_idx_usb,
    mux_buffer_idx_spp,
    mux_buffer_idx_ble,
    mux_buffer_idx_airupdate,
    mux_buffer_idx_iap2,
    mux_buffer_idx_gatt_over_bredr,
    mux_buffer_idx_1wire,
#ifdef AIR_RACE_SCRIPT_ENABLE
    mux_buffer_idx_pseudo,
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    mux_buffer_idx_cosys,
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    mux_buffer_idx_emcu,
#endif
    mux_buffer_idx_max,
} race_mux_buffer_idx_t;

typedef struct
{
    mux_buffer_t *mux_buffer;
    uint32_t offset;
    uint32_t idx;
    uint32_t counter;
    uint32_t left_data_len;
    uint32_t total_length;
}race_mux_buffer_stru;

race_mux_buffer_stru race_mux_buffer[mux_buffer_idx_max];

static void race_multi_buffer_fetch_init(mux_buffer_t buffers[], uint32_t buffers_counter, uint8_t index)
{
    uint32_t i;

    race_mux_buffer[index].mux_buffer = buffers;
    race_mux_buffer[index].offset = 0;
    race_mux_buffer[index].idx = 0;
    race_mux_buffer[index].counter = buffers_counter;
    race_mux_buffer[index].left_data_len = 0;
    for(i=0; i<buffers_counter; i++) {
        race_mux_buffer[index].left_data_len += buffers[i].buf_size;
    }
    race_mux_buffer[index].total_length = race_mux_buffer[index].left_data_len;
    /*RACE_LOG_MSGID_I("race_multi_buffer_fetch_init, buf_size[%d], left_data_len[%d], total_length[%d]", 3,
        buffers[0].buf_size, race_mux_buffer[index].left_data_len, race_mux_buffer[index].total_length);*/
}

static bool race_protocol_header_fetch(uint8_t *out_buf, uint32_t out_len, uint8_t index)
{
    uint32_t i;

    if (race_mux_buffer[index].idx >= race_mux_buffer[index].counter) {
        return false;
    }

    if(race_mux_buffer[index].left_data_len < out_len) {
        return false;
    }
    //race_dump((uint8_t *)race_mux_buffer[index].mux_buffer[race_mux_buffer[index].idx].p_buf, RACE_DBG_CMD);

    for(i=0; i<out_len; i++, race_mux_buffer[index].left_data_len--, race_mux_buffer[index].offset++)
    {
        if(race_mux_buffer[index].offset >= race_mux_buffer[index].mux_buffer[race_mux_buffer[index].idx].buf_size)
        {
            race_mux_buffer[index].idx++;
            race_mux_buffer[index].offset = 0;
            if(race_mux_buffer[index].idx>=race_mux_buffer[index].counter) {
                assert(0);
            }
        }
        *(out_buf+i) = *(race_mux_buffer[index].mux_buffer[race_mux_buffer[index].idx].p_buf + race_mux_buffer[index].offset);
        //RACE_LOG_MSGID_I("race_protocol_header_fetch, data[0x%x]", 1, *(out_buf+i));
    }
    return true;
}

static mux_port_t race_mux_get_port_by_idx(race_mux_buffer_idx_t idx)
{
    switch (idx) {
    case mux_buffer_idx_uart:
        return G_GET_UART_PORT();
    case mux_buffer_idx_usb:
        return G_GET_USB_PORT();
    case mux_buffer_idx_spp:
        return G_GET_SPP_PORT();
    case mux_buffer_idx_ble:
        return G_GET_BLE_PORT();
    case mux_buffer_idx_airupdate:
        return G_GET_AIRUPDATE_PORT();
    case mux_buffer_idx_iap2:
        return G_GET_IAP2_PORT();
    case mux_buffer_idx_gatt_over_bredr:
        return G_GET_GATT_OVER_BREDR_PORT();
    case mux_buffer_idx_1wire:
        return G_GET_1WIRE_PORT();
#ifdef AIR_RACE_SCRIPT_ENABLE
    case mux_buffer_idx_pseudo:
        return G_GET_PSEUDO_PORT();
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    case mux_buffer_idx_cosys:
        return G_GET_COSYS_PORT();
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    case mux_buffer_idx_emcu:
        return G_GET_EMCU_PORT();
#endif
    default:
        return MUX_PORT_END;
    }
}

static mux_handle_t race_mux_get_handle_by_idx(race_mux_buffer_idx_t idx)
{
    switch (idx) {
    case mux_buffer_idx_uart:
        return G_GET_UART_PORT_HANDLE();
    case mux_buffer_idx_usb:
        return G_GET_USB_PORT_HANDLE();
    case mux_buffer_idx_spp:
        return G_GET_SPP_PORT_HANDLE();
    case mux_buffer_idx_ble:
        return G_GET_BLE_PORT_HANDLE();
    case mux_buffer_idx_airupdate:
        return G_GET_AIRUPDATE_PORT_HANDLE();
    case mux_buffer_idx_iap2:
        return G_GET_IAP2_PORT_HANDLE();
    case mux_buffer_idx_gatt_over_bredr:
        return G_GET_GATT_OVER_BREDR_PORT_HANDLE();
    case mux_buffer_idx_1wire:
        return G_GET_1WIRE_PORT_HANDLE();
#ifdef AIR_RACE_SCRIPT_ENABLE
    case mux_buffer_idx_pseudo:
        return G_GET_PSEUDO_PORT_HANDLE();
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    case mux_buffer_idx_cosys:
        return G_GET_COSYS_PORT_HANDLE();
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    case mux_buffer_idx_emcu:
        return G_GET_EMCU_PORT_HANDLE();
#endif
    default:
        return MUX_PORT_END;
    }
}

#ifdef MTK_USB_AUDIO_HID_ENABLE
static void race_rx_clean_timer_callback(TimerHandle_t xtimer)
{
    RACE_LOG_MSGID_I("race_usb_rx_clean_timer timeout, reset usb rx buff", 0);
    mux_control(G_GET_USB_PORT(), MUX_CMD_CLEAN, NULL);
}
#endif

static void race_mux_rx_protocol_timer_check(race_mux_buffer_idx_t idx)
{
    if (idx != mux_buffer_idx_usb || race_usb_rx_clean_timer == NULL) {
        return;
    }
#ifdef MTK_USB_AUDIO_HID_ENABLE
    if (!race_usb_rx_clean_timer_started) {
        RACE_LOG_MSGID_I("start race_usb_rx_clean_timer", 0);
        race_usb_rx_clean_timer_started = TRUE;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (xTimerStartFromISR(race_usb_rx_clean_timer, &xHigherPriorityTaskWoken) != pdPASS) {
            RACE_LOG_MSGID_I("start race_usb_rx_clean_timer fail", 0);
        }
        if (xHigherPriorityTaskWoken != pdFALSE) {
            // Actual macro used here is port specific.
            portYIELD_FROM_ISR(pdTRUE);
        }
    }
#endif
}

static void race_mux_rx_protocol_timer_stop(race_mux_buffer_idx_t idx)
{
    if (idx != mux_buffer_idx_usb || race_usb_rx_clean_timer == NULL) {
            return;
    }

#ifdef MTK_USB_AUDIO_HID_ENABLE
    if (race_usb_rx_clean_timer_started) {
        RACE_LOG_MSGID_I("stop race_usb_rx_clean_timer", 0);
        race_usb_rx_clean_timer_started = FALSE;
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        if (xTimerStopFromISR(race_usb_rx_clean_timer, &xHigherPriorityTaskWoken) != pdPASS) {
            RACE_LOG_MSGID_I("stop race_usb_rx_clean_timer fail", 0);
        }
        if (xHigherPriorityTaskWoken != pdFALSE) {
            // Actual macro used here is port specific.
            portYIELD_FROM_ISR(pdTRUE);
        }
    }
#endif
}

static void race_mux_rx_protocol_callback_common(race_mux_buffer_idx_t idx, mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    mux_port_t port;
    uint8_t race_channel, race_type;
    uint16_t race_length, race_id;
    mux_handle_t p_handle;
#ifdef AIR_RACE_CO_SYS_ENABLE
    uint8_t race_cosys_critical = 0;
#endif
    RACE_LOG_MSGID_I("race_mux_rx_protocol_callback_common idx[%d], buffers_counter[%d]", 2, idx, buffers_counter);

    race_length = 0;
    *package_len = 0;
    *consume_len = 0;

    port = race_mux_get_port_by_idx(idx);

    race_multi_buffer_fetch_init(buffers, buffers_counter, idx);

    if (race_protocol_header_fetch(&race_channel, 1, idx) == false) {
        *package_len = 0;
        *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
        RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common, race_channel fetch fail", 0);
        return;
    }

    if((race_channel & 0x07) == 0x05) { //RACE PACKAGE
        if (race_protocol_header_fetch(&race_type, 1, idx) == false) {
            *package_len = 0;
            *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common, race_type fetch fail", 0);
            return;
        }

        if (idx == mux_buffer_idx_spp || idx == mux_buffer_idx_ble || idx == mux_buffer_idx_iap2 ||
                idx == mux_buffer_idx_airupdate || idx == mux_buffer_idx_gatt_over_bredr
#ifdef AIR_RACE_CO_SYS_ENABLE
                || idx == mux_buffer_idx_cosys
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
                || idx == mux_buffer_idx_emcu
#endif
        ) {
            if ((race_type < RACE_TYPE_COMMAND) || (race_type > RACE_TYPE_NOTIFICATION)) {
                *package_len = 0;
                *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
                RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common fail, race_type[0x%X]", 1, race_type);
            return;
            }
        } else {
            if ((race_type != RACE_TYPE_COMMAND) && (race_type != RACE_TYPE_COMMAND_WITHOUT_RSP)) {
                *package_len = 0;
                *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
                    RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common fail, race_type[0x%X]", 1, race_type);
                return;
            }
        }

        if (race_protocol_header_fetch((uint8_t *)&race_length, 2, idx) == false) {
            *package_len = 0;
            *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common, race_length fetch fail", 0);
            return;
        }

        if (race_protocol_header_fetch((uint8_t *)&race_id, 2, idx)== false) {
            *package_len = 0;
            *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common, race_id fetch fail", 0);
            return;
        }

#ifdef AIR_RACE_CO_SYS_ENABLE
        if (idx == mux_buffer_idx_cosys && race_id == 0x110F && race_protocol_header_fetch((uint8_t *)&race_cosys_critical, 1, idx)== false) {
            *package_len = 0;
            *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common, mux_buffer_idx_cosys fetch fail", 0);
            return;
        }
#endif

        *handle = 0;
        if (race_id == 0x0F92) {
            if(mux_query_user_handle(port, "ATCI", &p_handle) == MUX_STATUS_OK) {
                /* ATCI race command */
                *handle = p_handle;
                /* RACE cmd format: 05 5D length(2 byte) 0f92 at+test=? */
                *package_len = race_length - 2;
                *consume_len = 6;
            } else {
                *package_len = 0;
                *consume_len = race_length + 4;
                //return;
            }
        }
#ifdef AIR_RACE_CO_SYS_ENABLE
        else if (idx == mux_buffer_idx_cosys && race_id == 0x110F && race_cosys_critical == 1) {
            if(mux_query_user_handle(port, "RACE_COSYS", &p_handle) == MUX_STATUS_OK) {
                /* race cosys critical data. */
                *handle = p_handle;
                if(*handle) {
                    *package_len = race_length + 6 - 2;
                    *consume_len = 0;
                } else {
                    *package_len = 0;
                    *consume_len = race_length + 6 - 2;
                }
            }
        }
#endif
        else {
            *handle = race_mux_get_handle_by_idx(idx);;
            if(*handle) {
                *package_len = race_length + 6 - 2;
                *consume_len = 0;
            } else {
                *package_len = 0;
                *consume_len = race_length + 6 - 2;
            }

            /* Timer for long length race/AT (fota) buff clean.*/
            race_mux_rx_protocol_timer_check(idx);
        }

        RACE_LOG_MSGID_I("race_mux_rx_protocol_callback_common ok, race_channel[0x%X], race_type[0x%X], race_length[%d], race_id[0x%X] handle[0x%X]", 5,
            race_channel, race_type, race_length, race_id, *handle);
    }
#if 1
    else if(race_channel == 0x01) { //CMD  bt_hci_packet_cmd_t
        RACE_LOG_MSGID_I("race_mux_rx_protocol_callback_common, hci cmd enter", 0);
        uint16_t reserved_data;
        uint8_t total_length;
        if (race_protocol_header_fetch((uint8_t *)&reserved_data, 2, idx) == false) {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci cmd, reserved_data fetch fail", 0);
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        if (race_protocol_header_fetch((uint8_t *)&total_length, 1, idx) == false) {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci cmd, total_length fetch fail", 0);
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        *handle = 0;
        if(mux_query_user_handle(port, "HCI_CMD", &p_handle) == MUX_STATUS_OK) {
            /* race command */
                *handle = p_handle;
            *package_len = total_length + 4;
                *consume_len = 0;
            } else {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci cmd fail, p_handle[0x%X]", 1, p_handle);
        *package_len = 0;
        *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
    }
    }
    else if(race_channel == 0x02) { //ACL  bt_hci_packet_acl_t
        RACE_LOG_MSGID_I("race_mux_rx_protocol_callback_common, hci acl enter", 0);
        uint16_t reserved_data, total_length;
        if (race_protocol_header_fetch((uint8_t *)&reserved_data, 2, idx) == false) {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci acl, reserved_data fetch fail", 0);
    *package_len = 0;
    *consume_len = 0;
        return;
        }
        if (race_protocol_header_fetch((uint8_t *)&total_length, 2, idx) == false) {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci acl, total_length fetch fail", 0);
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        *handle = 0;
        if(mux_query_user_handle(port, "HCI_CMD", &p_handle) == MUX_STATUS_OK) {
            /* race command */
            *handle = p_handle;
            *package_len = total_length + 5;
            *consume_len = 0;
        } else {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci acl fail, p_handle[0x%X]", 1, p_handle);
            *package_len = 0;
            *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
        }
    }
    else if(race_channel == 0x04) { //EVT  bt_hci_packet_evt_t
        RACE_LOG_MSGID_I("race_mux_rx_protocol_callback_common, hci evt enter", 0);
        uint8_t reserved_data, total_length;
        if (race_protocol_header_fetch((uint8_t *)&reserved_data, 1, idx) == false) {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci evt, reserved_data fetch fail", 0);
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        if (race_protocol_header_fetch((uint8_t *)&total_length, 1, idx) == false) {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci evt, total_length fetch fail", 0);
            *package_len = 0;
            *consume_len = 0;
            return;
        }
        *handle = 0;
        if(mux_query_user_handle(port, "HCI_CMD", &p_handle) == MUX_STATUS_OK) {
            /* race command */
                *handle = p_handle;
            *package_len = total_length + 3;
                *consume_len = 0;
            } else {
            RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common hci evt fail, p_handle[0x%X]", 1, p_handle);
                *package_len = 0;
            *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
        }
    }
#endif
    else
    {
        *package_len = 0;
        *consume_len = race_mux_buffer[idx].total_length - race_mux_buffer[idx].left_data_len;
        RACE_LOG_MSGID_E("race_mux_rx_protocol_callback_common fail, not RACE format, race_channel[0x%X]", 1, race_channel);
    }
}

static void race_mux_rx_protocol_callback_uart(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_uart, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}

#ifdef AIR_RACE_SCRIPT_ENABLE
static void race_mux_rx_protocol_callback_pseudo(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_pseudo, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}
#endif

#ifdef AIR_RACE_CO_SYS_ENABLE
static void race_mux_rx_protocol_callback_cosys(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_cosys, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}
#endif

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
static void race_mux_rx_protocol_callback_emcu(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_emcu, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}
#endif

static void race_mux_rx_protocol_callback_1wire(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_1wire, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}

#ifdef MTK_USB_DEMO_ENABLED
static void race_mux_rx_protocol_callback_usb(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_usb, handle, buffers,buffers_counter, consume_len, package_len, user_data);
}
#endif

static void race_mux_tx_protocol_callback_uart_usb(mux_handle_t handle, const mux_buffer_t payload[],uint32_t buffers_counter, mux_buffer_t *head, mux_buffer_t *tail, void *user_data)
{
    mux_status_t status;
    const char *user_name = NULL;
    uint32_t race_id = 0, total_size = 0;
    uint32_t i;

    status = mux_query_user_name(handle, &user_name);
    if((status == MUX_STATUS_OK) && (user_name != NULL)) {
        if(strcmp(user_name, "RACE_CMD") == 0 || strcmp(user_name, "HCI_CMD") == 0) {
            head->p_buf = NULL;
            tail->p_buf = NULL;
            head->buf_size = 0;
            tail->buf_size = 0;
            return;
        } else if(strcmp(user_name, "ATCI") == 0) {
            race_id = 0x0F92;
        }
    }

    total_size = 0;
    for (i = 0; i<buffers_counter; i++) {
        total_size += payload[i].buf_size;
    }

    /* Insert the race header here */
    head->p_buf[0] = 0x05;
    head->p_buf[1] = RACE_TYPE_NOTIFICATION;
    head->p_buf[2] = (uint8_t)((total_size + 2) & 0xFF);
    head->p_buf[3] = (uint8_t)(((total_size + 2) >> 8) & 0xFF);
    head->p_buf[4] = (uint8_t)(race_id & 0xFF);
    head->p_buf[5] = (uint8_t)((race_id >> 8) & 0xFF);
    head->buf_size = 6;

    tail->p_buf = NULL;
    tail->buf_size = 0;
}

#ifdef AIR_RACE_CO_SYS_ENABLE
static void race_mux_tx_protocol_callback_cosys(mux_handle_t handle, const mux_buffer_t payload[], uint32_t buffers_counter, mux_buffer_t *head, mux_buffer_t *tail, void *user_data)
{

    if (!race_cosys_tx_timer_is_alive()) {
    #if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
        extern const char BSP_DUAL_CHIP_TRIGGER_WAKEUP_PIN;
        hal_gpio_set_output(BSP_DUAL_CHIP_TRIGGER_WAKEUP_PIN, HAL_GPIO_DATA_LOW);
        hal_gpt_delay_ms(15);
        hal_gpio_set_output(BSP_DUAL_CHIP_TRIGGER_WAKEUP_PIN, HAL_GPIO_DATA_HIGH);
    #elif defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        extern const char BSP_DUAL_CHIP_TRIGGER_WAKEUP_PIN;
        hal_gpio_set_output(BSP_DUAL_CHIP_TRIGGER_WAKEUP_PIN, HAL_GPIO_DATA_LOW);
        hal_gpt_delay_ms(15);
        hal_gpio_set_output(BSP_DUAL_CHIP_TRIGGER_WAKEUP_PIN, HAL_GPIO_DATA_HIGH);
    #endif
    }

    race_cosys_tx_timer_start();
    race_mux_tx_protocol_callback_uart_usb(handle, payload, buffers_counter, head, tail, user_data);
}
#endif

/*
Complete: only used for mux spp during mux_init()
*/
static void race_mux_rx_protocol_callback_spp(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_spp, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}

static void race_mux_rx_protocol_callback_ble(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_ble, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}

static void race_mux_rx_protocol_callback_airupdate(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_airupdate, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}

#if defined(MTK_IAP2_PROFILE_ENABLE) && defined(MTK_IAP2_VIA_MUX_ENABLE)
static void race_mux_rx_protocol_callback_iap2(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_iap2, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
static void race_mux_rx_protocol_callback_gatt_over_bredr(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
    uint32_t *consume_len, uint32_t *package_len,void *user_data)
{
    return race_mux_rx_protocol_callback_common(mux_buffer_idx_gatt_over_bredr, handle, buffers,buffers_counter, consume_len, package_len, user_data);

}
#endif
#endif

/*
Complete: should be used for all mux ports during mux_open()
*/
#if defined(MTK_MUX_ENABLE)
static void race_mux_callback(mux_handle_t handle, mux_event_t event, uint32_t data_len,void *user_data)
{
    RACE_ERRCODE ret;
    serial_port_dev_t device;
    serial_port_callback_event_t port_event = 0xFF;
    void *parameter = NULL;

    RACE_LOG_MSGID_I("race_mux_callback, handle[0x%X], event[%d]", 2, handle, event);

    if (handle == G_GET_USB_PORT_HANDLE() && event == MUX_EVENT_READY_TO_READ) {
        race_mux_rx_protocol_timer_stop(mux_buffer_idx_usb);
    }

    ret = race_convert_mux_event(&device, &port_event, &parameter, handle, event, data_len, user_data);

    if (ret == RACE_ERRCODE_SUCCESS)
    {
        race_serial_port_data_callback(device, port_event, &parameter);

        if (event == MUX_EVENT_CONNECTION) {
            switch (device) {
                case SERIAL_PORT_DEV_BT_SPP: {
                    mux_bt_connect_ind_t *ind = (mux_bt_connect_ind_t *)user_data;
                    race_set_bt_connection_addr(MUX_BT_SPP, &(ind->address));
                    break;
                }
                default:
                    break;
            }
        } else if (event == MUX_EVENT_DISCONNECTION) {
            switch (device) {
                case SERIAL_PORT_DEV_BT_SPP: {
                    race_set_bt_connection_addr(MUX_BT_SPP, NULL);
                    break;
                }
                default:
                    break;
            }
        }

        RACE_LOG_MSGID_I("race_mux_callback ok", 0);
    }
    else if (ret == RACE_ERRCODE_NOT_SUPPORT)
    {
        RACE_LOG_MSGID_W("race_mux_callback, not support", 0);
    }
    else
    {
        RACE_LOG_MSGID_E("race_mux_callback fail, status[%d]", 1, ret);
    }
}
#endif

#ifdef AIR_RACE_SCRIPT_ENABLE
race_status_t race_pseudo_port_init(void)
{
    mux_port_setting_t setting;
    mux_status_t mux_status1, mux_status2;
    mux_protocol_t protocol_callback = {race_mux_tx_protocol_callback_uart_usb, race_mux_rx_protocol_callback_pseudo, NULL};

    setting.tx_buffer_size = RACE_MUX_UART_TX_BUFFER_SIZE;
    setting.rx_buffer_size = RACE_MUX_UART_RX_BUFFER_SIZE;

    G_SET_PSEUDO_PORT(MUX_PORT_PSEUDO);
    mux_status1 = mux_init(MUX_PORT_PSEUDO, &setting, &protocol_callback);
    mux_status2 = mux_open(MUX_PORT_PSEUDO, "RACE_CMD", &G_GET_PSEUDO_PORT_HANDLE(), race_mux_callback, NULL);
    RACE_LOG_MSGID_I("race_pseudo_port_init, mux_status1[0x%X], mux_status[0x%X]", 2, mux_status1, mux_status2);
    if (mux_status1 == MUX_STATUS_OK && mux_status2 == MUX_STATUS_OK) {
        return RACE_ERRCODE_SUCCESS;
    } else {
        return RACE_ERRCODE_FAIL;
    }
}
#endif

#ifdef AIR_RACE_CO_SYS_ENABLE
extern const unsigned char BSP_DUAL_CHIP_WAKEUP_EINT;

static void race_cosys_eint_callback(void *user_data)
{
    // User's handler
    printf("race_cosys im wake\r\n");
    race_start_sleep_lock_timer();
    hal_eint_unmask(BSP_DUAL_CHIP_WAKEUP_EINT);
}

static void race_cosys_mux_callback(mux_handle_t handle, mux_event_t event, uint32_t data_len,void *user_data)
{
    if (event == MUX_EVENT_READY_TO_READ) {
        race_cosys_rx_irq(handle, data_len);
    }
}

race_status_t race_cosys_port_init(void)
{
    mux_port_setting_t setting = {0};
    mux_status_t mux_status1, mux_status2, mux_status3;
    mux_handle_t race_cosys_critical_handle;
    mux_protocol_t protocol_callback = {race_mux_tx_protocol_callback_cosys, race_mux_rx_protocol_callback_cosys, NULL};

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
    extern const unsigned char BSP_DUAL_CHIP_WAKEUP_EINT;
    hal_eint_config_t config_eint;
    config_eint.trigger_mode = HAL_EINT_EDGE_FALLING;
    config_eint.debounce_time = 1;
    hal_eint_init(BSP_DUAL_CHIP_WAKEUP_EINT, &config_eint);
    hal_eint_register_callback(BSP_DUAL_CHIP_WAKEUP_EINT, race_cosys_eint_callback, NULL);
    //hal_eint_set_debounce_count(BSP_DUAL_CHIP_WAKEUP_EINT, 5); /* debounce time: (1/32k) * 5 = 165us*/
    hal_eint_unmask(BSP_DUAL_CHIP_WAKEUP_EINT);
#elif defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    extern const unsigned char BSP_DUAL_CHIP_WAKEUP_EINT;
    hal_eint_config_t config_eint;
    config_eint.trigger_mode = HAL_EINT_EDGE_FALLING;
    config_eint.debounce_time = 1;
    hal_eint_init(BSP_DUAL_CHIP_WAKEUP_EINT, &config_eint);
    hal_eint_register_callback(BSP_DUAL_CHIP_WAKEUP_EINT, race_cosys_eint_callback, NULL);
    //hal_eint_set_debounce_count(BSP_DUAL_CHIP_WAKEUP_EINT, 5); /* debounce time: (1/32k) * 5 = 165us*/
    hal_eint_unmask(BSP_DUAL_CHIP_WAKEUP_EINT);
#endif

    setting.tx_buffer_size = RACE_COSYS_MUX_BUFF_SIZE;
    setting.rx_buffer_size = RACE_COSYS_MUX_BUFF_SIZE;
    setting.dev_setting.uart.uart_config.baudrate    = CONFIG_RACE_BAUDRATE;
    setting.dev_setting.uart.uart_config.word_length = HAL_UART_WORD_LENGTH_8;
    setting.dev_setting.uart.uart_config.stop_bit    = HAL_UART_STOP_BIT_1;
    setting.dev_setting.uart.uart_config.parity      = HAL_UART_PARITY_NONE;
    setting.dev_setting.uart.flowcontrol_type        = MUX_UART_SW_FLOWCONTROL;

    G_SET_COSYS_PORT(RACE_DUAL_CHIP_COSYS_PORT);

    mux_status1 = mux_init(RACE_DUAL_CHIP_COSYS_PORT, &setting, &protocol_callback);
    mux_status2 = mux_open(RACE_DUAL_CHIP_COSYS_PORT, "RACE_CMD", &G_GET_COSYS_PORT_HANDLE(), race_mux_callback, NULL);
    mux_status3 = mux_open(RACE_DUAL_CHIP_COSYS_PORT, "RACE_COSYS", &race_cosys_critical_handle, race_cosys_mux_callback, NULL);
    RACE_LOG_MSGID_I("race_cosys_port_init, mux_status1[0x%X], mux_status2[0x%X], mux_status3[0x%X]", 3, mux_status1, mux_status2, mux_status3);

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE

    G_SET_EMCU_PORT(RACE_DUAL_CHIP_EMCU_PORT);
    setting.tx_buffer_size = RACE_EMCU_MUX_BUFF_SIZE;
    setting.rx_buffer_size = RACE_EMCU_MUX_BUFF_SIZE;
    protocol_callback.tx_protocol_callback = race_mux_tx_protocol_callback_uart_usb;
    protocol_callback.rx_protocol_callback = race_mux_rx_protocol_callback_emcu;
    mux_status1 = mux_init(RACE_DUAL_CHIP_EMCU_PORT, &setting, &protocol_callback);
    mux_status2 = mux_open(RACE_DUAL_CHIP_EMCU_PORT, "RACE_CMD", &G_GET_EMCU_PORT_HANDLE(), race_mux_callback, NULL);
    RACE_LOG_MSGID_I("race_cosys_port_init emcu, mux_status1[0x%X], mux_status2[0x%X]", 2, mux_status1, mux_status2);
#endif

    if (mux_status1 == MUX_STATUS_OK && mux_status2 == MUX_STATUS_OK) {
        return RACE_ERRCODE_SUCCESS;
    } else {
        return RACE_ERRCODE_FAIL;
    }
}
#endif

race_status_t race_serial_port_init(serial_port_dev_t port, uint32_t *serial_port_handle)
{
    serial_port_status_t status = SERIAL_PORT_STATUS_FAIL;
    serial_port_open_para_t serial_port_open_para = {0};

    RACE_LOG_MSGID_I("race_serial_port_init, port[%d]", 1, port);

#if defined(MTK_MUX_ENABLE)
    mux_status_t mux_init_status = MUX_STATUS_ERROR;
    mux_status_t mux_status = MUX_STATUS_ERROR;

    if((port >= SERIAL_PORT_DEV_UART_TYPE_BEGIN) && (port < SERIAL_PORT_DEV_UART_TYPE_END))
    {
        mux_port_setting_t setting = {0};
        mux_protocol_t protocol_callback_1wire = {race_mux_tx_protocol_callback_uart_usb, race_mux_rx_protocol_callback_1wire, NULL};
        mux_protocol_t protocol_callback = {race_mux_tx_protocol_callback_uart_usb, race_mux_rx_protocol_callback_uart, NULL};

        setting.tx_buffer_size = RACE_MUX_UART_TX_BUFFER_SIZE;
        setting.rx_buffer_size = RACE_MUX_UART_RX_BUFFER_SIZE;
        setting.dev_setting.uart.uart_config.baudrate    = CONFIG_RACE_BAUDRATE;
        setting.dev_setting.uart.uart_config.word_length = HAL_UART_WORD_LENGTH_8;
        setting.dev_setting.uart.uart_config.stop_bit    = HAL_UART_STOP_BIT_1;
        setting.dev_setting.uart.uart_config.parity      = HAL_UART_PARITY_NONE;
        setting.dev_setting.uart.flowcontrol_type        = MUX_UART_NONE_FLOWCONTROL;

        //Don't need to call mux_init(), since it's already called in syslog.
        if (port == G_GET_1WIRE_PORT())
        {
            setting.tx_buffer_size = RACE_MUX_1WIRE_UART_TX_BUFFER_SIZE;
            setting.rx_buffer_size = RACE_MUX_1WIRE_UART_RX_BUFFER_SIZE;
            setting.dev_setting.uart.uart_config.baudrate = g_race_1wire_baudrate;
            mux_init_status = mux_init(race_muxid_from_portid(port), &setting, &protocol_callback_1wire);
        }
        else
        {
            uart_disable_irq(port);
            mux_init_status = mux_init(race_muxid_from_portid(port), &setting, &protocol_callback);
        }
        if (mux_init_status == MUX_STATUS_OK || mux_init_status == MUX_STATUS_ERROR_INITIATED)
            mux_status = mux_open(race_muxid_from_portid(port), "RACE_CMD", serial_port_handle, race_mux_callback, NULL);
    }

    #ifdef MTK_USB_DEMO_ENABLED
    else if((port >= SERIAL_PORT_DEV_USB_TYPE_BEGIN) && (port <= SERIAL_PORT_DEV_USB_TYPE_END))
    {
        mux_port_setting_t setting = {0};
        mux_protocol_t protocol_callback = {race_mux_tx_protocol_callback_uart_usb, race_mux_rx_protocol_callback_usb, NULL};

        setting.tx_buffer_size = RACE_MUX_USB_TX_BUFFER_SIZE;
        setting.rx_buffer_size = RACE_MUX_USB_RX_BUFFER_SIZE;
        setting.dev_setting.uart.uart_config.baudrate    = CONFIG_RACE_BAUDRATE;
        setting.dev_setting.uart.uart_config.word_length = HAL_UART_WORD_LENGTH_8;
        setting.dev_setting.uart.uart_config.stop_bit    = HAL_UART_STOP_BIT_1;
        setting.dev_setting.uart.uart_config.parity      = HAL_UART_PARITY_NONE;
        setting.dev_setting.uart.flowcontrol_type        = MUX_UART_NONE_FLOWCONTROL;

        mux_init_status = mux_init(race_muxid_from_portid(port), &setting, &protocol_callback);
        if (mux_init_status == MUX_STATUS_OK || mux_init_status == MUX_STATUS_ERROR_INITIATED)
            mux_status = mux_open(race_muxid_from_portid(port), "RACE_CMD", serial_port_handle, race_mux_callback, NULL);
    }
    #endif

    #if defined(MTK_MUX_BT_ENABLE)
    else if(port == SERIAL_PORT_DEV_BT_SPP) // For SPP, we use mux service instead of port service.
    {
        mux_port_setting_t setting = {0};
        mux_protocol_t protocol_callback = {NULL, race_mux_rx_protocol_callback_spp, NULL};

        setting.tx_buffer_size = RACE_MUX_SPP_TX_BUFFER_SIZE;
        setting.rx_buffer_size = RACE_MUX_SPP_RX_BUFFER_SIZE;
        mux_init_status = mux_init(race_muxid_from_portid(port), &setting, &protocol_callback);
        if (mux_init_status == MUX_STATUS_OK)
            mux_status = mux_open(race_muxid_from_portid(port), "BT_SPP", serial_port_handle, race_mux_callback, NULL);
    }
    else if(port == SERIAL_PORT_DEV_BT_LE){ // For BLE, we use mux service instead of port service.
        mux_port_setting_t setting = {0};
        mux_protocol_t protocol_callback = {NULL, race_mux_rx_protocol_callback_ble, NULL};

        setting.tx_buffer_size = RACE_MUX_BLE_TX_BUFFER_SIZE;
        setting.rx_buffer_size = RACE_MUX_BLE_RX_BUFFER_SIZE;
        mux_init_status = mux_init(race_muxid_from_portid(port), &setting, &protocol_callback);
        if (mux_init_status == MUX_STATUS_OK)
            mux_status = mux_open(race_muxid_from_portid(port), "BT_BLE", serial_port_handle, race_mux_callback, NULL);
    }
    #ifdef MTK_AIRUPDATE_ENABLE
    else if(port == SERIAL_PORT_DEV_BT_AIRUPDATE){
        mux_port_setting_t setting = {0};
        mux_protocol_t protocol_callback = {NULL, race_mux_rx_protocol_callback_airupdate, NULL};

        setting.tx_buffer_size = RACE_MUX_AIRUPDATE_TX_BUFFER_SIZE;
        setting.rx_buffer_size = RACE_MUX_AIRUPDATE_RX_BUFFER_SIZE;
        mux_init_status = mux_init(race_muxid_from_portid(port), &setting, &protocol_callback);
        if (mux_init_status == MUX_STATUS_OK)
            mux_status = mux_open(race_muxid_from_portid(port), "AIRUPDATE", serial_port_handle, race_mux_callback, NULL);
    }
    #endif
    #ifdef MTK_GATT_OVER_BREDR_ENABLE
    else if(port == SERIAL_PORT_DEV_BT_GATT_OVER_BREDR){ // For GATT, we use mux service instead of port service.
        mux_port_setting_t setting = {0};
        mux_protocol_t protocol_callback = {NULL, race_mux_rx_protocol_callback_gatt_over_bredr, NULL};

        setting.tx_buffer_size = RACE_MUX_GATT_OVER_BREDR_TX_BUFFER_SIZE;
        setting.rx_buffer_size = RACE_MUX_GATT_OVER_BREDR_RX_BUFFER_SIZE;
        mux_init_status = mux_init(race_muxid_from_portid(port), &setting, &protocol_callback);
        if (mux_init_status == MUX_STATUS_OK)
            mux_status = mux_open(race_muxid_from_portid(port), "GATT", serial_port_handle, race_mux_callback, NULL);
    }
    #endif
    #endif
    #if defined(MTK_IAP2_VIA_MUX_ENABLE) && defined(MTK_IAP2_PROFILE_ENABLE)
    else if(port == SERIAL_PORT_DEV_IAP2_SESSION2){ // For SPP, we use mux service instead of port service.
        mux_port_setting_t setting = {0};
        mux_protocol_t protocol_callback = {NULL, race_mux_rx_protocol_callback_iap2, NULL};

        setting.tx_buffer_size = RACE_MUX_IAP2_TX_BUFFER_SIZE;
        setting.rx_buffer_size = RACE_MUX_IAP2_RX_BUFFER_SIZE;
        mux_init_status = mux_init(race_muxid_from_portid(port), &setting, &protocol_callback);
        if (mux_init_status == MUX_STATUS_OK)
            mux_status = mux_open(race_muxid_from_portid(port), "iAP2", serial_port_handle, race_mux_callback, NULL);
        #if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1568) || (PRODUCT_VERSION == 1565) || (PRODUCT_VERSION == 2821)
        mux_set_config_param_t config_param = {
            .is_rx_need_session_id = 0,
            .reserved = 0
        };
        mux_control(G_GET_IAP2_PORT_HANDLE(), MUX_CMD_SET_RX_PARAM, (mux_ctrl_para_t *)&config_param);
        #endif
    }
    #endif
    else
    {
        RACE_LOG_MSGID_E("race_serial_port_init fail, port is not support MUX, port[%d]", 1, port);
        // register the serial user event callback
        serial_port_open_para.callback = race_serial_port_data_callback;
        serial_port_open_para.tx_buffer_size = RACE_MUX_UART_TX_BUFFER_SIZE;
        serial_port_open_para.rx_buffer_size = RACE_MUX_UART_RX_BUFFER_SIZE;

        // for serial port configuration
        status = serial_port_open(port, &serial_port_open_para, serial_port_handle);
        if ((status == SERIAL_PORT_STATUS_OK) || (status == SERIAL_PORT_STATUS_DEV_NOT_READY))
        {
            RACE_LOG_MSGID_I("race_serial_port_init ok, port_service, port[%d], status[%d]", 2, port, status);
            return RACE_STATUS_OK;
        }
        else
        {
            RACE_LOG_MSGID_E("race_serial_port_init fail, port_service, port[%d], status[%d]", 2, port, status);
            *serial_port_handle = 0; //set 0 if fail
            return RACE_STATUS_ERROR;
        }
    }
    if(mux_status != MUX_STATUS_OK)
    {
        // When open failed, serial_port_open() will set handle value to an invalid value,
        // we reset it to zero for race_xport logic.
        *serial_port_handle = 0;//set 0 if fail
        RACE_LOG_MSGID_E("race_serial_port_init fail, mux, mux_init_status[%d], mux_status[%d]", 2, mux_init_status, mux_status);
        return RACE_STATUS_ERROR;
    }
    else
    {
        RACE_LOG_MSGID_I("race_serial_port_init ok, mux, port[%d], serial_port_handle[0x%X], *serial_port_handle[0x%X]", 3,
                port, serial_port_handle, *serial_port_handle);
        return RACE_STATUS_OK;
    }
#else
    // register the serial user event callback
    serial_port_open_para.callback = race_serial_port_data_callback;
    serial_port_open_para.tx_buffer_size = RACE_UART_TX_FIFO_BUFFER_SIZE;
    serial_port_open_para.rx_buffer_size = RACE_UART_RX_FIFO_BUFFER_SIZE;

    // for serial port configuration
    status = serial_port_open(port, &serial_port_open_para, serial_port_handle);
    #if (PRODUCT_VERSION == 2822) || (PRODUCT_VERSION == 1568) || (PRODUCT_VERSION == 1565) || (PRODUCT_VERSION == 2821)
    if(port == SERIAL_PORT_DEV_IAP2_SESSION2)
    {
        serial_port_set_config_param_t config_param = {
            .is_rx_need_session_id = 0,
            .reserved = 0
        };
        serial_port_control(G_GET_IAP2_PORT_HANDLE(), SERIAL_PORT_CMD_SET_RX_PARAM, &config_param);
    }
    #endif
    if ((status == SERIAL_PORT_STATUS_OK) || (status == SERIAL_PORT_STATUS_DEV_NOT_READY))
    {
        RACE_LOG_MSGID_I("race_serial_port_init ok, port_service, port[%d], status[%d]", 2, port, status);
        return RACE_STATUS_OK;
    }
    else
    {
        RACE_LOG_MSGID_E("race_serial_port_init fail, port_service, port[%d], status[%d]", 2, port, status);
        *serial_port_handle = 0; //set 0 if fail
        return RACE_STATUS_ERROR;
    }
#endif
}


race_serial_port_type_enum race_get_channel_id_by_port_handle(uint32_t port_handle)
{
    // TODO: return real port type by chn id

    if (port_handle == G_GET_UART_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_UART;
    else if (port_handle == G_GET_1WIRE_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_1WIRE;
    else if (port_handle == G_GET_USB_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_USB;
    else if (port_handle == G_GET_BLE_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_BLE;
    else if (port_handle == G_GET_SPP_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_SPP;
    else if (port_handle == G_GET_AIRUPDATE_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_AIRUPDATE;
    else if (port_handle == G_GET_IAP2_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_IAP2;
    else if (port_handle == G_GET_GATT_OVER_BREDR_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR;
#ifdef AIR_RACE_SCRIPT_ENABLE
    else if (port_handle == G_GET_PSEUDO_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_PSEUDO;
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    else if (port_handle == G_GET_COSYS_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_COSYS;
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    else if (port_handle == G_GET_EMCU_PORT_HANDLE())
        return RACE_SERIAL_PORT_TYPE_EMCU;
#endif
    else
        return RACE_SERIAL_PORT_TYPE_NONE;
}

uint32_t race_get_port_by_channel_id(uint8_t channel_id)
{
    race_serial_port_type_enum port_type = (race_serial_port_type_enum)channel_id;

    if (port_type == RACE_SERIAL_PORT_TYPE_UART)
        return G_GET_UART_PORT();
    else if (port_type == RACE_SERIAL_PORT_TYPE_1WIRE)
        return G_GET_1WIRE_PORT();
    else if (port_type == RACE_SERIAL_PORT_TYPE_USB)
        return G_GET_USB_PORT();
    else if (port_type == RACE_SERIAL_PORT_TYPE_BLE)
        return G_GET_BLE_PORT();
    else if (port_type == RACE_SERIAL_PORT_TYPE_SPP)
        return G_GET_SPP_PORT();
    else if (port_type == RACE_SERIAL_PORT_TYPE_AIRUPDATE)
        return G_GET_AIRUPDATE_PORT();
    else if (port_type == RACE_SERIAL_PORT_TYPE_IAP2)
        return G_GET_IAP2_PORT();
    else if (port_type == RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR)
        return G_GET_GATT_OVER_BREDR_PORT();
#ifdef AIR_RACE_SCRIPT_ENABLE
    else if (port_type == RACE_SERIAL_PORT_TYPE_PSEUDO)
        return G_GET_PSEUDO_PORT();
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    else if (port_type == RACE_SERIAL_PORT_TYPE_COSYS)
        return G_GET_COSYS_PORT();
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    else if (port_type == RACE_SERIAL_PORT_TYPE_EMCU)
        return G_GET_EMCU_PORT();
#endif
    else
        return 0;
}

uint32_t race_get_port_handle_by_channel_id(uint8_t channel_id)
{
    race_serial_port_type_enum port_type = (race_serial_port_type_enum)channel_id;

    if (port_type == RACE_SERIAL_PORT_TYPE_UART)
        return G_GET_UART_PORT_HANDLE();
    else if (port_type == RACE_SERIAL_PORT_TYPE_1WIRE)
        return G_GET_1WIRE_PORT_HANDLE();
    else if (port_type == RACE_SERIAL_PORT_TYPE_USB)
        return G_GET_USB_PORT_HANDLE();
    else if (port_type == RACE_SERIAL_PORT_TYPE_BLE)
        return G_GET_BLE_PORT_HANDLE();
    else if (port_type == RACE_SERIAL_PORT_TYPE_SPP)
        return G_GET_SPP_PORT_HANDLE();
    else if (port_type == RACE_SERIAL_PORT_TYPE_AIRUPDATE)
        return G_GET_AIRUPDATE_PORT_HANDLE();
    else if (port_type == RACE_SERIAL_PORT_TYPE_IAP2)
        return G_GET_IAP2_PORT_HANDLE();
    else if (port_type == RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR)
        return G_GET_GATT_OVER_BREDR_PORT_HANDLE();
#ifdef AIR_RACE_SCRIPT_ENABLE
    else if (port_type == RACE_SERIAL_PORT_TYPE_PSEUDO)
        return G_GET_PSEUDO_PORT_HANDLE();
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    else if (port_type == RACE_SERIAL_PORT_TYPE_COSYS)
        return G_GET_COSYS_PORT_HANDLE();
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    else if (port_type == RACE_SERIAL_PORT_TYPE_EMCU)
        return G_GET_EMCU_PORT_HANDLE();
#endif
    else
        return 0;
}

uint8_t race_get_channel_id_by_port_type(race_serial_port_type_enum port_type)
{
    if ((port_type > RACE_SERIAL_PORT_TYPE_NONE) && (port_type < RACE_SERIAL_PORT_TYPE_MAX))
        return (uint8_t)port_type;
    else
        return 0;
}

race_serial_port_type_enum race_get_port_type_by_channel_id(uint8_t channel_id)
{
#if 1
    race_serial_port_type_enum port_type = (race_serial_port_type_enum)RACE_CHANNEL_ID_CLEAR_RELAY_CMD_FLAG(channel_id);
    if ((port_type > RACE_SERIAL_PORT_TYPE_NONE) && (port_type < RACE_SERIAL_PORT_TYPE_MAX))
        return port_type;
    else
        return RACE_SERIAL_PORT_TYPE_NONE;
#else
    /* The relationship between the port_type and the channel_id may change. So use port_handle to check. */

    if (G_GET_BLE_PORT_HANDLE() &&
        G_GET_BLE_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_BLE;
    }

    if (G_GET_SPP_PORT_HANDLE() &&
        G_GET_SPP_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_SPP;
    }

    if (G_GET_AIRUPDATE_PORT_HANDLE() &&
        G_GET_AIRUPDATE_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_AIRUPDATE;
    }

    if (G_GET_IAP2_PORT_HANDLE() &&
        G_GET_IAP2_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_IAP2;
    }

    if (G_GET_UART_PORT_HANDLE() &&
        G_GET_UART_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_UART;
    }

    if (G_GET_USB_PORT_HANDLE() &&
        G_GET_USB_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_USB;
    }

    if (G_GET_GATT_OVER_BREDR_PORT_HANDLE() &&
        G_GET_GATT_OVER_BREDR_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR;
    }

#ifdef AIR_RACE_SCRIPT_ENABLE
    if (G_GET_PSEUDO_PORT_HANDLE() &&
        G_GET_PSEUDO_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_PSEUDO;
    }
#endif

#ifdef AIR_RACE_CO_SYS_ENABLE
    if (G_GET_COSYS_PORT_HANDLE() &&
        G_GET_COSYS_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_COSYS;
    }
#endif

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    if (G_GET_EMCU_PORT_HANDLE() &&
        G_GET_EMCU_PORT_HANDLE() == race_get_port_handle_by_channel_id(channel_id))
    {
        return RACE_SERIAL_PORT_TYPE_EMCU;
    }
#endif

    return RACE_SERIAL_PORT_TYPE_NONE;
#endif
}

race_status_t race_serial_port_open(race_serial_port_type_enum port_type, serial_port_dev_t port)
{
    race_status_t ret = RACE_STATUS_ERROR;

    RACE_LOG_MSGID_I("race_serial_port_open, port_type[%d], port[%d], g_race_serial_port_list[0x%X]", 3,
        port_type, port, g_race_serial_port_list);

    if (!g_race_serial_port_list)
    {
        RACE_LOG_MSGID_E("race_serial_port_open fail, g_race_serial_port_list NULL", 0);
        return ret;
    }

    switch (port_type)
    {
        case RACE_SERIAL_PORT_TYPE_UART:
            if (race_serial_uart_port_check(port) == RACE_STATUS_OK && (!G_GET_UART_PORT_HANDLE()))
            {
                G_SET_UART_PORT(port);
                ret = race_serial_port_init(port, &G_GET_UART_PORT_HANDLE());
                if (ret == RACE_STATUS_OK)
                {
                    g_race_init_ctrl.race_serial_port_uart = RACE_INIT_DONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_open, UART check fail, port_handle[0x%X]", 1, G_GET_UART_PORT_HANDLE());
            }
        break;
        case RACE_SERIAL_PORT_TYPE_1WIRE:
            if (race_serial_uart_port_check(port) == RACE_STATUS_OK && (!G_GET_1WIRE_PORT_HANDLE()))
            {
                G_SET_1WIRE_PORT(port);
                ret = race_serial_port_init(port, &G_GET_1WIRE_PORT_HANDLE());
                if (ret == RACE_STATUS_OK)
                {
                    g_race_init_ctrl.race_serial_port_1wire = RACE_INIT_DONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_open, 1WIRE check fail, port_handle[0x%X]", 1, G_GET_1WIRE_PORT_HANDLE());
            }
        break;
        case RACE_SERIAL_PORT_TYPE_USB:
            if (race_serial_usb_port_check(port) == RACE_STATUS_OK && (!G_GET_USB_PORT_HANDLE()))
            {
                G_SET_USB_PORT(port);
                ret = race_serial_port_init(port, &G_GET_USB_PORT_HANDLE());
                if (ret == RACE_STATUS_OK)
                {
                    g_race_init_ctrl.race_serial_port_usb = RACE_INIT_DONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_open, USB check fail, port_handle[0x%X]", 1, G_GET_USB_PORT_HANDLE());
            }
        break;
        case RACE_SERIAL_PORT_TYPE_SPP:
            if ((port == SERIAL_PORT_DEV_BT_SPP) && !G_GET_SPP_PORT_HANDLE())
            {
                G_SET_SPP_PORT(port);
                ret = race_serial_port_init(port, &G_GET_SPP_PORT_HANDLE());
                if (ret == RACE_STATUS_OK)
                {
                    g_race_init_ctrl.race_serial_port_spp = RACE_INIT_DONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_open, SPP check fail, port_handle[0x%X]", 1, G_GET_SPP_PORT_HANDLE());
            }
        break;
        case RACE_SERIAL_PORT_TYPE_BLE:
            if ((port == SERIAL_PORT_DEV_BT_LE) && !G_GET_BLE_PORT_HANDLE())
            {
                G_SET_BLE_PORT(port);
                ret = race_serial_port_init(port, &G_GET_BLE_PORT_HANDLE());
                if (ret == RACE_STATUS_OK)
                {
                    g_race_init_ctrl.race_serial_port_ble = RACE_INIT_DONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_open, BLE check fail, port_handle[0x%X]", 1, G_GET_BLE_PORT_HANDLE());
            }
        break;
#ifdef MTK_AIRUPDATE_ENABLE
        case RACE_SERIAL_PORT_TYPE_AIRUPDATE:
            if ((port == SERIAL_PORT_DEV_BT_AIRUPDATE) && !G_GET_AIRUPDATE_PORT_HANDLE())
            {
                G_SET_AIRUPDATE_PORT(port);
                ret = race_serial_port_init(port, &G_GET_AIRUPDATE_PORT_HANDLE());
                if (ret == RACE_STATUS_OK)
                {
                    g_race_init_ctrl.race_serial_port_airupdate = RACE_INIT_DONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_open, AIRUPDATE check fail, port_handle[0x%X]", 1, G_GET_AIRUPDATE_PORT_HANDLE());
            }
        break;
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
        case RACE_SERIAL_PORT_TYPE_IAP2:
            if ((port == SERIAL_PORT_DEV_IAP2_SESSION2) && !G_GET_IAP2_PORT_HANDLE())
            {
                G_SET_IAP2_PORT(port);
                ret = race_serial_port_init(port, &G_GET_IAP2_PORT_HANDLE());
                if (ret == RACE_STATUS_OK)
                {
                    g_race_init_ctrl.race_serial_port_iap2 = RACE_INIT_DONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_open, IAP2 check fail, port_handle[0x%X]", 1, G_GET_IAP2_PORT_HANDLE());
            }
        break;
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        case RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR:
            if ((port == SERIAL_PORT_DEV_BT_GATT_OVER_BREDR) && !G_GET_GATT_OVER_BREDR_PORT_HANDLE())
            {
                G_SET_GATT_OVER_BREDR_PORT(port);
                ret = race_serial_port_init(port, &G_GET_GATT_OVER_BREDR_PORT_HANDLE());
                if (ret == RACE_STATUS_OK)
                {
                    g_race_init_ctrl.race_serial_port_gatt_over_bredr = RACE_INIT_DONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_open, GATT check fail, port_handle[0x%X]", 1, G_GET_GATT_OVER_BREDR_PORT_HANDLE());
            }
        break;
#endif
        default:
        break;
    }

    if (ret == RACE_STATUS_OK)
    {
        RACE_LOG_MSGID_I("race_serial_port_open ok", 0);
    }
    else
    {
        RACE_LOG_MSGID_E("race_serial_port_open fail", 0);
    }

    return ret;
}

/*init spp,ble,airupdate*/
race_status_t race_serial_port_list_init(serial_port_dev_t port)
{
    race_status_t status = RACE_STATUS_ERROR;
    race_serial_port_type_enum port_type = RACE_SERIAL_PORT_TYPE_NONE;

    if (g_race_serial_port_list == NULL)
    {
        race_alloc_serial_port_list(&g_race_serial_port_list);
        if (g_race_serial_port_list == NULL)
        {
            return RACE_STATUS_ERROR;
        }
    }
    else
    {
        RACE_LOG_MSGID_E("race_serial_port_list_init fail, g_race_serial_port_list exist", 0);
        return RACE_STATUS_ERROR;
    }


#ifdef AIR_RACE_CO_SYS_ENABLE
    if (port == (serial_port_dev_t)RACE_DUAL_CHIP_COSYS_PORT) {
        RACE_LOG_MSGID_E("race_serial_port_list_init fail, cannot use COSYSPORT in co-sys", 0);
        return RACE_STATUS_ERROR;
    }
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    if (port == (serial_port_dev_t)RACE_DUAL_CHIP_EMCU_PORT) {
        RACE_LOG_MSGID_E("race_serial_port_list_init fail, cannot use EMCUPORT in master", 0);
        return RACE_STATUS_ERROR;
    }
#endif

    if ((port >= SERIAL_PORT_DEV_UART_TYPE_BEGIN) && (port <= SERIAL_PORT_DEV_UART_TYPE_END))
    {
        g_race_uart_port = port;
        port_type = RACE_SERIAL_PORT_TYPE_UART;
        status = race_serial_port_open(port_type, port);
    }
    else if ((port >= SERIAL_PORT_DEV_USB_TYPE_BEGIN) && (port <= SERIAL_PORT_DEV_USB_TYPE_END))
    {
        g_race_usb_port = port;
        port_type = RACE_SERIAL_PORT_TYPE_USB;
        //G_SET_USB_PORT(port);
        status = race_serial_port_open(port_type, port);
        status = RACE_STATUS_OK;
    }
    else
    {
        RACE_LOG_MSGID_E("race_serial_port_list_init fail, port_type[%d], port[%d]", 2, port_type, port);
    }

    if (status != RACE_STATUS_OK)
        RACE_LOG_MSGID_E("race_serial_port_list_init fail", 0);

    return status;
}

race_status_t race_local_init()
{
    /* Init Queue */
    if (NULL == (void *)g_race_serial_port_queue)
    {
        g_race_serial_port_queue = atci_queue_create(RACE_LOCAL_QUEUE_LENGTH, sizeof(race_general_msg_t));
        if ((void *)g_race_serial_port_queue == NULL)
        {
            RACE_LOG_MSGID_E("race_local_init fail, g_race_serial_port_queue is NULL", 0);
            return RACE_STATUS_ERROR;
        }
    }
    if(auReceiveBuff == NULL)
    {
        auReceiveBuff = (uint8_t *)race_mem_alloc(SERIAL_PORT_RECEIVE_BUFFER_SIZE);
        if(auReceiveBuff == NULL)
        {
            RACE_LOG_MSGID_E("race_local_init fail, auReceiveBuff is NULL", 0);
            return RACE_STATUS_ERROR;
        }
    }

    return RACE_STATUS_OK;
}

race_status_t race_serial_port_uart_init(serial_port_dev_t port)
{
    race_status_t status = RACE_STATUS_ERROR;

    RACE_LOG_MSGID_I("race_serial_port_uart_init, port[%d], race_init_status[%d], race_serial_port_uart[%d], g_race_uart_port[%d]", 4,
        port, g_race_init_ctrl.race_init_status, g_race_init_ctrl.race_serial_port_uart, g_race_uart_port);

    if (g_race_init_ctrl.race_serial_port_uart == RACE_INIT_DONE)
    {
        RACE_LOG_MSGID_W("race_serial_port_uart_init exist", 0);
        return RACE_STATUS_OK;
    }

#if defined(MTK_PORT_SERVICE_ENABLE)
    status = race_serial_port_open(RACE_SERIAL_PORT_TYPE_UART, port);
#endif

    if (status == RACE_STATUS_OK)
    {
        g_race_uart_port = port;
        g_race_init_ctrl.race_serial_port_uart = RACE_INIT_DONE;
        RACE_LOG_MSGID_I("race_serial_port_uart_init ok, port[%d]", 1, port);
    }
    else
    {
        g_race_uart_port = SERIAL_PORT_DEV_UART_1;
        g_race_init_ctrl.race_serial_port_uart = RACE_INIT_NONE;
        RACE_LOG_MSGID_E("race_serial_port_uart_init fail, port[%d]", 1, port);
    }

    return status;
}

race_status_t race_init_port_service(serial_port_dev_t port)
{
    race_status_t status = RACE_STATUS_ERROR;

    RACE_LOG_MSGID_I("race_init_port_service, port[%d]", 1, port);

    status = race_local_init();

    if (status != RACE_STATUS_OK) {
        RACE_LOG_MSGID_E("race_local_init fail", 0);
        return status;
    }

    status = race_serial_port_list_init(port);

    if (status == RACE_STATUS_OK)
        RACE_LOG_MSGID_I("race_init_port_service ok", 0);
    else
        RACE_LOG_MSGID_E("race_init_port_service fail", 0);

#ifdef MTK_USB_AUDIO_HID_ENABLE
    if ((port < SERIAL_PORT_DEV_USB_TYPE_BEGIN) || (port > SERIAL_PORT_DEV_USB_TYPE_END))
    {
        if (race_usb_rx_clean_timer == NULL) {
            race_usb_rx_clean_timer = xTimerCreate("race_rx_timer", (2 * 1000 / portTICK_PERIOD_MS), pdFALSE, NULL, race_rx_clean_timer_callback);
            RACE_LOG_MSGID_I("create race_usb_rx_clean_timer success 0X%x", 1, race_usb_rx_clean_timer);
        }
        g_race_usb_port = SERIAL_PORT_DEV_USB_COM2;
        status = race_serial_port_open(RACE_SERIAL_PORT_TYPE_USB, g_race_usb_port);
        RACE_LOG_MSGID_I("race_init_port_service, init hid usb stat %d", 1, status);
    }
#endif

#if (defined RACE_RELAY_CMD_ENABLE) || (defined AIR_RACE_CO_SYS_ENABLE)
    race_relay_cmd_init();
#endif

    return status;
}

#if defined(MTK_MUX_ENABLE)
static race_status_t race_mux_deinit(serial_port_dev_t port, uint32_t serial_port_handle)
{
    mux_status_t mux_status = MUX_STATUS_ERROR;

    RACE_LOG_MSGID_I("race_mux_deinit, port[%d], serial_port_handle[0x%X]", 2, port, serial_port_handle);

    if (!serial_port_handle)
    {
        RACE_LOG_MSGID_E("race_mux_deinit fail, port handle error", 0);
        return RACE_STATUS_ERROR;
    }

    if((port == SERIAL_PORT_DEV_UART_0) || (port == SERIAL_PORT_DEV_UART_1) ||
       (port == SERIAL_PORT_DEV_UART_2) || (port == SERIAL_PORT_DEV_UART_3))
    {
        mux_status = mux_close(serial_port_handle);
        if(mux_status != MUX_STATUS_OK)
        {
            RACE_LOG_MSGID_E("race_mux_deinit, UART mux_close fail, mux_status[%d]", 1, mux_status);
        }
        else
        {
            mux_status = mux_deinit(race_muxid_from_portid(port));
            if(mux_status != MUX_STATUS_OK)
            {
                RACE_LOG_MSGID_E("race_mux_deinit, UART mux_deinit fail, mux_status[%d]", 1, mux_status);
            }
        }
        if (port == G_GET_1WIRE_PORT())
        {
            G_SET_1WIRE_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_1WIRE_PORT_HANDLE(NULL);
            g_race_init_ctrl.race_serial_port_1wire = RACE_INIT_NONE;
        }
        else
        {
            G_SET_UART_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_UART_PORT_HANDLE(NULL);
            g_race_init_ctrl.race_serial_port_uart = RACE_INIT_NONE;
        }
    }
#ifdef MTK_USB_DEMO_ENABLED
    if((port == SERIAL_PORT_DEV_USB_COM1) || (port == SERIAL_PORT_DEV_USB_COM2))
    {
        mux_status = mux_close(serial_port_handle);
        if(mux_status != MUX_STATUS_OK)
        {
            RACE_LOG_MSGID_E("race_mux_deinit, USB mux_close fail, mux_status[%d]", 1, mux_status);
        }
        else
        {
            mux_status = mux_deinit(race_muxid_from_portid(port));
            if(mux_status != MUX_STATUS_OK)
            {
                RACE_LOG_MSGID_E("race_mux_deinit, USB mux_deinit fail, mux_status[%d]", 1, mux_status);
            }
        }
        G_SET_USB_PORT(SERIAL_PORT_DEV_UNDEFINED);
        G_SET_USB_PORT_HANDLE(NULL);
        g_race_init_ctrl.race_serial_port_usb = RACE_INIT_NONE;
    }
#endif
#if defined(MTK_MUX_BT_ENABLE)
    else if(port == SERIAL_PORT_DEV_BT_SPP)
    {
        mux_status = mux_close(serial_port_handle);
        if(mux_status != MUX_STATUS_OK)
        {
            RACE_LOG_MSGID_E("race_mux_deinit, SPP mux_close fail, mux_status[%d]", 1, mux_status);
        }
        else
        {
            mux_status = mux_deinit(race_muxid_from_portid(port));
            if(mux_status != MUX_STATUS_OK)
            {
                RACE_LOG_MSGID_E("race_mux_deinit, SPP mux_deinit fail, mux_status[%d]", 1, mux_status);
            }
        }
        G_SET_SPP_PORT(SERIAL_PORT_DEV_UNDEFINED);
        G_SET_SPP_PORT_HANDLE(NULL);
        g_race_init_ctrl.race_serial_port_spp = RACE_INIT_NONE;
    }
    else if(port == SERIAL_PORT_DEV_BT_LE)
    {
        mux_status = mux_close(serial_port_handle);
        if(mux_status != MUX_STATUS_OK)
        {
            RACE_LOG_MSGID_E("race_mux_deinit, BLE mux_close fail, mux_status[%d]", 1, mux_status);
        }
        else
        {
            mux_status = mux_deinit(race_muxid_from_portid(port));
            if(mux_status != MUX_STATUS_OK)
            {
                RACE_LOG_MSGID_E("race_mux_deinit, BLE mux_deinit fail, mux_status[%d]", 1, mux_status);
            }
        }
        G_SET_BLE_PORT(SERIAL_PORT_DEV_UNDEFINED);
        G_SET_BLE_PORT_HANDLE(NULL);
        g_race_init_ctrl.race_serial_port_ble = RACE_INIT_NONE;
    }
#ifdef MTK_AIRUPDATE_ENABLE
    else if(port == SERIAL_PORT_DEV_BT_AIRUPDATE)
    {
        mux_status = mux_close(serial_port_handle);
        if(mux_status != MUX_STATUS_OK)
        {
            RACE_LOG_MSGID_E("race_mux_deinit, AIRUPDATE mux_close fail, mux_status[%d]", 1, mux_status);
        }
        else
        {
            mux_status = mux_deinit(race_muxid_from_portid(port));
            if(mux_status != MUX_STATUS_OK)
            {
                RACE_LOG_MSGID_E("race_mux_deinit, AIRUPDATE mux_deinit fail, mux_status[%d]", 1, mux_status);
            }
        }
        G_SET_AIRUPDATE_PORT(SERIAL_PORT_DEV_UNDEFINED);
        G_SET_AIRUPDATE_PORT_HANDLE(NULL);
        g_race_init_ctrl.race_serial_port_airupdate = RACE_INIT_NONE;
    }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    else if(port == SERIAL_PORT_DEV_BT_GATT_OVER_BREDR)
    {
        mux_status = mux_close(serial_port_handle);
        if(mux_status != MUX_STATUS_OK)
        {
            RACE_LOG_MSGID_E("race_mux_deinit, GATT mux_close fail, mux_status[%d]", 1, mux_status);
        }
        else
        {
            mux_status = mux_deinit(race_muxid_from_portid(port));
            if(mux_status != MUX_STATUS_OK)
            {
                RACE_LOG_MSGID_E("race_mux_deinit, GATT mux_deinit fail, mux_status[%d]", 1, mux_status);
            }
        }
        G_SET_GATT_OVER_BREDR_PORT(SERIAL_PORT_DEV_UNDEFINED);
        G_SET_GATT_OVER_BREDR_PORT_HANDLE(NULL);
        g_race_init_ctrl.race_serial_port_gatt_over_bredr = RACE_INIT_NONE;
    }
#endif
#endif
#if defined(MTK_IAP2_VIA_MUX_ENABLE) && defined(MTK_IAP2_PROFILE_ENABLE)
    else if(port == SERIAL_PORT_DEV_IAP2_SESSION2)
    {
        mux_status = mux_close(serial_port_handle);
        if(mux_status != MUX_STATUS_OK)
        {
            RACE_LOG_MSGID_E("race_mux_deinit, IAP2 mux_close fail, mux_status[%d]", 1, mux_status);
        }
        else
        {
            mux_status = mux_deinit(race_muxid_from_portid(port));
            if(mux_status != MUX_STATUS_OK)
            {
                RACE_LOG_MSGID_E("race_mux_deinit, IAP2 mux_deinit fail, mux_status[%d]", 1, mux_status);
            }
        }
        G_SET_IAP2_PORT(SERIAL_PORT_DEV_UNDEFINED);
        G_SET_IAP2_PORT_HANDLE(NULL);
        g_race_init_ctrl.race_serial_port_iap2 = RACE_INIT_NONE;
    }
#endif
    else
    {
        RACE_LOG_MSGID_E("race_mux_deinit fail, port is not support MUX, port[%d]", 1, port);
    }

    if (mux_status == MUX_STATUS_OK)
    {
        RACE_LOG_MSGID_I("race_mux_deinit ok, port[%d]", 1, port);
        return RACE_STATUS_OK;
    }
    else
    {
        RACE_LOG_MSGID_E("race_mux_deinit fail, port[%d], mux_status[%d]", 2, port, mux_status);
        return RACE_STATUS_ERROR;
    }
}
#endif


#ifndef MTK_PORT_SERVICE_ENABLE
static race_status_t race_hal_uart_deinit(hal_uart_port_t port)
{
    hal_uart_status_t uart_ret = HAL_UART_STATUS_ERROR;
    race_status_t ret = RACE_STATUS_ERROR;

    uart_ret = hal_uart_deinit(port);

    RACE_LOG_MSGID_I("race_hal_uart_deinit, uart_ret[%d]", 1, uart_ret);

    if (uart_ret == HAL_UART_STATUS_OK) {

        ret = RACE_STATUS_OK;
    }

    return ret;
}
#endif

race_status_t race_uart_deinit(void)
{
    race_status_t ret = RACE_STATUS_ERROR;

    RACE_LOG_MSGID_I("race_uart_deinit, race_serial_port_uart[%d]", 1, g_race_init_ctrl.race_serial_port_uart);

    if (g_race_init_ctrl.race_serial_port_uart == RACE_INIT_NONE) {
        RACE_LOG_MSGID_E("race_uart_deinit fail, race_serial_port_uart is not ready", 0);
        return RACE_STATUS_ERROR;
    }

#if defined(MTK_PORT_SERVICE_ENABLE)
    serial_port_status_t status = serial_port_close(G_GET_UART_PORT_HANDLE());
    if (status == SERIAL_PORT_STATUS_OK)
    {
        G_SET_UART_PORT(SERIAL_PORT_DEV_UNDEFINED);
        G_SET_UART_PORT_HANDLE(NULL);
        ret = RACE_STATUS_OK;
    }
#else
    ret = race_hal_uart_deinit(g_race_uart_port);
#endif

    if (ret == RACE_STATUS_OK)
    {
        g_race_init_ctrl.race_serial_port_uart = RACE_INIT_NONE;
        RACE_LOG_MSGID_I("race_uart_deinit ok", 0);
    }
    else
    {
        RACE_LOG_MSGID_E("race_uart_deinit fail, ret[%d]", 1, ret);
    }

    return ret;
}

race_status_t race_serial_port_close(race_serial_port_type_enum port_type)
{
    race_status_t ret = RACE_STATUS_ERROR;
    serial_port_status_t status = SERIAL_PORT_STATUS_FAIL;
    #if defined(MTK_MUX_ENABLE)
    serial_port_dev_t port = SERIAL_PORT_DEV_UNDEFINED;
    #endif
    uint32_t port_handle = 0;

    RACE_LOG_MSGID_I("race_serial_port_close, port_type[%d], g_race_serial_port_list[0x%X]", 2,
        port_type, g_race_serial_port_list);

    if (!g_race_serial_port_list)
    {
        RACE_LOG_MSGID_I("race_serial_port_close fail, g_race_serial_port_list is NULL", 0);
        return ret;
    }

    switch (port_type)
    {
        case RACE_SERIAL_PORT_TYPE_UART:
            port_handle = G_GET_UART_PORT_HANDLE();
            #if defined(MTK_MUX_ENABLE)
            port = G_GET_UART_PORT();
            if (race_serial_uart_port_check(port) == RACE_STATUS_OK && port_handle)
            {
                ret = race_mux_deinit(port, port_handle);
                if (ret == RACE_STATUS_OK)
                {
                    G_SET_UART_PORT(SERIAL_PORT_DEV_UNDEFINED);
                    G_SET_UART_PORT_HANDLE(NULL);
                    g_race_init_ctrl.race_serial_port_uart = RACE_INIT_NONE;
                    status = SERIAL_PORT_STATUS_OK;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_close, UART check fail", 0);
            }
            #else
            status = serial_port_close(port_handle);
            G_SET_UART_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_UART_PORT_HANDLE(NULL);
            #endif
        break;

        case RACE_SERIAL_PORT_TYPE_1WIRE:
            port_handle = G_GET_1WIRE_PORT_HANDLE();
            #if defined(MTK_MUX_ENABLE)
            port = G_GET_1WIRE_PORT();
            if (race_serial_uart_port_check(port) == RACE_STATUS_OK && port_handle)
            {
                ret = race_mux_deinit(port, port_handle);
                if (ret == RACE_STATUS_OK)
                {
                    G_SET_1WIRE_PORT(SERIAL_PORT_DEV_UNDEFINED);
                    G_SET_1WIRE_PORT_HANDLE(NULL);
                    g_race_init_ctrl.race_serial_port_1wire = RACE_INIT_NONE;
                    status = SERIAL_PORT_STATUS_OK;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_close, 1WIRE check fail", 0);
            }
            #else
            status = serial_port_close(port_handle);
            G_SET_1WIRE_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_1WIRE_PORT_HANDLE(NULL);
            #endif
        break;

#ifdef MTK_USB_DEMO_ENABLED
        case RACE_SERIAL_PORT_TYPE_USB:
            port_handle = G_GET_USB_PORT_HANDLE();
            #if defined(MTK_MUX_ENABLE)
            port = G_GET_USB_PORT();
            if (race_serial_usb_port_check(port) == RACE_STATUS_OK && port_handle)
            {
                ret = race_mux_deinit(port, port_handle);
                if (ret == RACE_STATUS_OK)
                {
                    G_SET_USB_PORT(SERIAL_PORT_DEV_UNDEFINED);
                    G_SET_USB_PORT_HANDLE(NULL);
                    g_race_init_ctrl.race_serial_port_usb = RACE_INIT_NONE;
                    status = SERIAL_PORT_STATUS_OK;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_close, USB check fail", 0);
            }
            #else
            status = serial_port_close(port_handle);
            G_SET_USB_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_USB_PORT_HANDLE(NULL);
            #endif
        break;
#endif
        case RACE_SERIAL_PORT_TYPE_SPP: //SPP is not disconnected
            port_handle = G_GET_SPP_PORT_HANDLE();
            #if defined(MTK_MUX_ENABLE) && defined(MTK_MUX_BT_ENABLE)
            port = G_GET_SPP_PORT();
            if ((port == SERIAL_PORT_DEV_BT_SPP) && port_handle)
            {
                ret = race_mux_deinit(port, port_handle);
                if (ret == RACE_STATUS_OK)
                {
                    G_SET_SPP_PORT(SERIAL_PORT_DEV_UNDEFINED);
                    G_SET_SPP_PORT_HANDLE(NULL);
                    g_race_init_ctrl.race_serial_port_spp = RACE_INIT_NONE;
                    status = SERIAL_PORT_STATUS_OK;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_close, SPP check fail", 0);
            }
            #else
            status = serial_port_close(port_handle);
            G_SET_SPP_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_SPP_PORT_HANDLE(NULL);
            #endif
        break;

        case RACE_SERIAL_PORT_TYPE_BLE: //BLE is not disconnected
            port_handle = G_GET_BLE_PORT_HANDLE();
            #if defined(MTK_MUX_ENABLE) && defined(MTK_MUX_BT_ENABLE)
            port = G_GET_BLE_PORT();
            if ((port == SERIAL_PORT_DEV_BT_LE) && port_handle)
            {
                ret = race_mux_deinit(port, port_handle);
                if (ret == RACE_STATUS_OK)
                {
                    G_SET_BLE_PORT(SERIAL_PORT_DEV_UNDEFINED);
                    G_SET_BLE_PORT_HANDLE(NULL);
                    g_race_init_ctrl.race_serial_port_ble = RACE_INIT_NONE;
                    status = SERIAL_PORT_STATUS_OK;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_close, BLE check fail", 0);
            }
            #else
            status = serial_port_close(port_handle);
            G_SET_BLE_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_BLE_PORT_HANDLE(NULL);
            #endif
        break;

        case RACE_SERIAL_PORT_TYPE_AIRUPDATE: //TODO: check if AIRUPDATE is disconnected by calling this API
            port_handle = G_GET_AIRUPDATE_PORT_HANDLE();
            #if defined(MTK_MUX_ENABLE) && defined(MTK_AIRUPDATE_ENABLE)
            port = G_GET_AIRUPDATE_PORT();
            if ((port == SERIAL_PORT_DEV_BT_AIRUPDATE) && port_handle)
            {
                ret = race_mux_deinit(port, port_handle);
                if (ret == RACE_STATUS_OK)
                {
                    G_SET_AIRUPDATE_PORT(SERIAL_PORT_DEV_UNDEFINED);
                    G_SET_AIRUPDATE_PORT_HANDLE(NULL);
                    g_race_init_ctrl.race_serial_port_airupdate = RACE_INIT_NONE;
					status = SERIAL_PORT_STATUS_OK;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_close, AIRUPDATE check fail", 0);
            }
            #else
            status = serial_port_close(port_handle);
            G_SET_AIRUPDATE_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_AIRUPDATE_PORT_HANDLE(NULL);
            #endif
        break;

        case RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR: //GATT is not disconnected
            port_handle = G_GET_GATT_OVER_BREDR_PORT_HANDLE();
            #if defined(MTK_MUX_ENABLE) && defined(MTK_MUX_BT_ENABLE) && defined(MTK_GATT_OVER_BREDR_ENABLE)
            port = G_GET_GATT_OVER_BREDR_PORT();
            if ((port == SERIAL_PORT_DEV_BT_GATT_OVER_BREDR) && port_handle)
            {
                ret = race_mux_deinit(port, port_handle);
                if (ret == RACE_STATUS_OK)
                {
                    G_SET_GATT_OVER_BREDR_PORT(SERIAL_PORT_DEV_UNDEFINED);
                    G_SET_GATT_OVER_BREDR_PORT_HANDLE(NULL);
                    g_race_init_ctrl.race_serial_port_gatt_over_bredr = RACE_INIT_NONE;
                    status = SERIAL_PORT_STATUS_OK;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_close, GATT OVER BREDR check fail", 0);
            }
            #else
            status = serial_port_close(port_handle);
            G_SET_GATT_OVER_BREDR_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_GATT_OVER_BREDR_PORT_HANDLE(NULL);
            #endif
        break;

        case RACE_SERIAL_PORT_TYPE_IAP2:
            port_handle = G_GET_IAP2_PORT_HANDLE();
            #if defined(MTK_MUX_ENABLE) && defined(MTK_IAP2_VIA_MUX_ENABLE) && defined(MTK_IAP2_PROFILE_ENABLE)
            port = G_GET_IAP2_PORT();
            if ((port == SERIAL_PORT_DEV_IAP2_SESSION2) && port_handle)
            {
                ret = race_mux_deinit(port, port_handle);
                if (ret == RACE_STATUS_OK)
                {
                    G_SET_IAP2_PORT(SERIAL_PORT_DEV_UNDEFINED);
                    G_SET_IAP2_PORT_HANDLE(NULL);
                    g_race_init_ctrl.race_serial_port_iap2 = RACE_INIT_NONE;
                }
            }
            else
            {
                RACE_LOG_MSGID_E("race_serial_port_close, IAP2 check fail", 0);
            }
            #else
            status = serial_port_close(port_handle);
            G_SET_IAP2_PORT(SERIAL_PORT_DEV_UNDEFINED);
            G_SET_IAP2_PORT_HANDLE(NULL);
            #endif
        break;

        default:
        break;
    }

    if (status == SERIAL_PORT_STATUS_OK)
    {
        RACE_LOG_MSGID_I("race_serial_port_close ok, port_handle[0x%X]", 1, port_handle);
        ret = RACE_STATUS_OK;
    }
    else
    {
        RACE_LOG_MSGID_I("race_serial_port_close fail, status[%d], port_handle[0x%X]", 2, status, port_handle);
    }

    return ret;
}

race_status_t race_send_data(uint32_t port_handle, uint8_t* data, uint32_t data_len)
{
    uint8_t* send_data = NULL;
    uint32_t send_len = 0;
    uint32_t sent_len = 0;
    race_status_t ret = RACE_STATUS_OK;

    RACE_LOG_MSGID_I("race_send_data, cache_left_len[%ld]", 1, g_race_send_data_cache_left_len);
    if (port_handle == 0)
    {
        RACE_LOG_MSGID_E("race_send_data fail, port_handle 0", 0);
        return RACE_STATUS_ERROR;
    }

    if (g_race_send_data_cache_left_len == 0) {

        send_data = data;
        send_len = data_len;
    }
    else if ((data == NULL) && (g_race_send_data_cache_left_len > 0)) {

        if (g_race_send_data_cache != NULL)
        {
            send_data = &g_race_send_data_cache[g_race_send_data_cache_len - g_race_send_data_cache_left_len];
            send_len = g_race_send_data_cache_left_len;
        }
        else
            return  RACE_STATUS_ERROR;
    }
    else
        return  RACE_STATUS_ERROR;

    // send data by port
    sent_len = race_port_send_data(port_handle, send_data, send_len);
    if (sent_len == 0)
    {
        ret = RACE_STATUS_ERROR;
        RACE_LOG_MSGID_E("race_send_data fail, sent_len 0", 0);
    }

    RACE_LOG_MSGID_I("race_send_data, send_len[%ld], sent_len[%ld]", 2, send_len, sent_len);

    if (sent_len >= send_len) {
        // send all the input data
        if (g_race_send_data_cache != NULL)
        {
            race_mem_free(g_race_send_data_cache);
            g_race_send_data_cache = NULL;
            RACE_LOG_MSGID_I("race_send_data, race_mem_free", 0);
        }
        g_race_send_data_cache_len = 0;
        g_race_send_data_cache_left_len = 0;

        return RACE_STATUS_OK;
    }
    else {
        // cache the left sending data
        if (g_race_send_data_cache_len == 0) {

            g_race_send_data_cache_len = send_len - sent_len;

            if (g_race_send_data_cache == NULL)
            {
                g_race_send_data_cache = (uint8_t *)race_mem_alloc(g_race_send_data_cache_len);
                if (g_race_send_data_cache == NULL)
                {
                    RACE_LOG_MSGID_E("race_send_data, alloc memory fail, drop_cached_data_len[%ld]", 1, g_race_send_data_cache_left_len);
                    g_race_send_data_cache_len = 0;
                    g_race_send_data_cache_left_len = 0;
                    return RACE_STATUS_ERROR;
                }
            }

            memset(g_race_send_data_cache, 0, g_race_send_data_cache_len);
            memcpy(g_race_send_data_cache, &send_data[sent_len], g_race_send_data_cache_len);
        }

        g_race_send_data_cache_left_len = send_len - sent_len;

        RACE_LOG_MSGID_I("race_send_data, cached_data_len[%ld]", 1, g_race_send_data_cache_left_len);
    }

    return ret;
}

race_status_t race_header_check(uint8_t *header, uint32_t *length)
{
    RACE_LOG_MSGID_I("race_header_check, data[%X %X %X %X %X %X]", 6,
        header[0], header[1], header[2], header[3], header[4], header[5]);

    if ((0x05 == (header[0] & 0x0F)) &&
        ((0x5A == header[1]) ||
         (0x5B == header[1]) ||
         (0x5C == header[1]) ||
         (0x5D == header[1])))
    {
        *length = (uint32_t)((header[3] << 8) + header[2]);
        if (*length <= RACE_MAX_LENGTH)
        {
            return RACE_STATUS_OK;
        }
    }
    return RACE_STATUS_ERROR;
}

race_status_t race_read_data(race_general_msg_t *msg)
{
    race_status_t ret = RACE_STATUS_OK;
    RACE_ERRCODE retn = RACE_ERRCODE_SUCCESS;
    uint32_t port_handle = 0;
    uint32_t  length = 0;
    uint8_t *race_cmd_header;
    uint32_t read_len = 0, offset = 0;
    uint8_t *ptrPos = NULL;
    race_input_cmd_msg_t *msg_item = NULL;
    race_general_msg_t msg_queue_item;
    serial_port_dev_t dev_t = msg->dev_t;

    port_handle = race_get_serial_port_handle(dev_t);
    RACE_LOG_MSGID_I("race_read_data, port_handle[0x%X]", 1, port_handle);
    if (port_handle == 0)
    {
        return RACE_STATUS_ERROR;
    }

    GET_INPUT_CMD_MSG_PTR(dev_t, &msg_item);
    if (msg_item == NULL)
    {
        return RACE_STATUS_ERROR;
    }

    read_len = race_port_read_data(port_handle, auReceiveBuff, SERIAL_PORT_RECEIVE_BUFFER_SIZE);
    RACE_LOG_MSGID_I("race_read_data, read_len[%d]", 1, read_len);

    ptrPos = auReceiveBuff;
    offset = 0;
    while (read_len > 0)
    {
        if (msg_item->isNewRaceCmd)
        {
            race_cmd_header = ptrPos;
            if (read_len < 4)
            {
                RACE_LOG_MSGID_I("race_read_data fail, read_len[%d] < 4, data[0x%X 0x%X 0x%X 0x%X 0x%X 0x%X]", 7,
                    read_len, ptrPos[0], ptrPos[1], ptrPos[2], ptrPos[3], ptrPos[4], ptrPos[5]);
                ret = RACE_STATUS_ERROR;
                break;
            }
            if (race_header_check(race_cmd_header, &length) == RACE_STATUS_OK)
            {
                if (msg_item->msg_data != NULL)
                {
                    /* Should not happen actually */
                    RACE_LOG_MSGID_W("race_read_data, free last uncompleted race_cmd, msg_data[0x%X]", 1, msg_item->msg_data);
                    race_mem_free(msg_item->msg_data);
                }
                msg_item->msg_data = race_mem_alloc(length + 4);
                msg_item->uLength = 0;
                if (!msg_item->msg_data)
                {
                    RACE_LOG_MSGID_W("race_read_data, race_mem_alloc fail", 0);
                    ret = RACE_STATUS_ERROR;
                    break;
                }
            }
            else
            {
                RACE_LOG_MSGID_I("race_read_data, race_header_check fail", 0);
                ret = RACE_STATUS_ERROR;
                break;
            }
            memcpy(msg_item->msg_data, race_cmd_header, 4);
            msg_item->uLength += 4;
            msg_item->uLeftLength = length;
            msg_item->isNewRaceCmd = false;
            msg_item->dev_t = dev_t;

            offset = 4;
        }
        else
        {
            if (read_len < msg_item->uLeftLength)
            {
                memcpy((uint8_t*)(msg_item->msg_data + msg_item->uLength), ptrPos, read_len);
                msg_item->isNewRaceCmd = false;
                msg_item->uLength += read_len;
                msg_item->uLeftLength -= read_len;

                offset = read_len;
            }
            else
            {
                memcpy((uint8_t*)(msg_item->msg_data + msg_item->uLength), ptrPos, msg_item->uLeftLength);
                msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_RSP_NOTIFY_IND;
                msg_queue_item.dev_t = dev_t;
                msg_queue_item.msg_data = msg_item->msg_data;
                #if (defined AIR_DONGLE_RELAY_RACE_PACKET_ENABLE) || (defined AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
                msg_queue_item.msg_flag = msg->msg_flag;
                #endif
                RACE_LOG_MSGID_I("race_read_data done, length[%d]", 1, (msg_item->uLength + msg_item->uLeftLength));
                retn = race_send_msg(&msg_queue_item);
                if (RACE_ERRCODE_SUCCESS != retn)
                {
                    RACE_LOG_MSGID_I("race_read_data, race_send_msg fail, retn[%d]", 1, retn);
                    //race_mem_free(msg_item->msg_data);
                    /* Do not break here to process the rest data received. */
                }

                offset = msg_item->uLeftLength;

                msg_item->isNewRaceCmd = true;
                msg_item->uLeftLength = 0;
                msg_item->msg_data = NULL;
                msg_item->uLength = 0;
                msg_item->dev_t = SERIAL_PORT_DEV_UNDEFINED;
            }
        }

        read_len -= offset;
        ptrPos += offset;

    }

    //race_mem_free(auReciveBuff);

    return ret;
}

#if (defined AIR_DONGLE_RELAY_RACE_PACKET_ENABLE) || (defined AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
static void race_flush_raw_cmd(uint8_t *ptr, uint8_t channel_id)
{
    RACE_COMMON_HDR_STRU *race_header = (RACE_COMMON_HDR_STRU *)ptr;
    uint32_t length = race_header->length + 4;
    uint32_t port_handle = race_get_port_handle_by_channel_id(channel_id);
    race_port_send_data(port_handle, ptr, length);
}
#endif

race_status_t race_flush_packet_relay(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id, uint8_t type, uint8_t send_idx)
{
#ifdef RACE_RELAY_CMD_ENABLE
    bt_send_aws_mce_race_cmd_data(race_pkt, length, channel_id, type, send_idx);
#endif

    return RACE_STATUS_OK;
}


race_status_t race_flush_packet(uint8_t *ptr, uint8_t channel_id)
{
    race_status_t ret = RACE_STATUS_ERROR;
    race_send_pkt_t* pData = NULL;

    RACE_LOG_MSGID_I("race_flush_packet, ptr[0x%X] channel_id[%d]", 2, ptr, channel_id);

    if (ptr != NULL)
    {
        pData = race_pointer_cnv_pkt_to_send_pkt((void *)ptr);

#ifdef AIR_RACE_CO_SYS_ENABLE
        if (RACE_CHANNEL_ID_IS_RELAY_CMD_FLAG_SET(channel_id)) {
            bool ret_val = FALSE;

            channel_id = RACE_CHANNEL_ID_CLEAR_RELAY_CMD_FLAG(channel_id);
            ret_val = race_relay_send_cosys(&pData->race_data, pData->length, channel_id, RACE_CMD_RSP_FROM_PARTNER);
            race_mem_free(pData);
            return ret_val ? RACE_STATUS_OK : RACE_STATUS_ERROR;
        }
#elif (defined RACE_RELAY_CMD_ENABLE)
        if (RACE_CHANNEL_ID_IS_RELAY_CMD_FLAG_SET(channel_id)) {
            bt_status_t ret_val;

            channel_id = RACE_CHANNEL_ID_CLEAR_RELAY_CMD_FLAG(channel_id);
            ret_val = bt_send_aws_mce_race_cmd_data(&pData->race_data, pData->length, channel_id, RACE_CMD_RSP_FROM_PARTNER, 0);
            race_mem_free(pData);
            return (ret_val == BT_STATUS_SUCCESS) ? RACE_STATUS_OK : RACE_STATUS_ERROR;
        }
#endif

        //race_port_send_data(port_handle, (uint8_t*)&pData->race_data, pData->length);

        /* BTA-19782, fix dead lock. Change to send in race task. */
        race_general_msg_t msg_queue_item = {0};
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_WRITE_CMD_IND;
        msg_queue_item.dev_t = race_get_port_by_channel_id(channel_id);
        msg_queue_item.msg_data = race_mem_alloc(sizeof(uint32_t) + pData->length);
        race_send_output_cmd_msg_t *data = (race_send_output_cmd_msg_t *)msg_queue_item.msg_data;
        if (data == NULL) {
            RACE_LOG_MSGID_E("race_port_send_data alloc fail", 0);
            return 0;
        }
        data->output_len = pData->length;
        memcpy(data->buff, (uint8_t*)&pData->race_data, pData->length);
        race_send_msg(&msg_queue_item);

#if (RACE_DEBUG_PRINT_ENABLE)
        race_pkt_t      *pret;
        race_send_pkt_t *psend;
        psend = (race_send_pkt_t *)pData;
        pret = &psend->race_data;
        race_dump((uint8_t *)pret, RACE_DBG_FLUSH);
#endif
        ret = RACE_STATUS_OK;
        race_mem_free(pData);
    }

    return ret;
}

race_status_t race_input_command_handler(uint32_t port_handle, uint8_t* race_input_cmd)
{
    race_status_t ret = RACE_STATUS_ERROR;
    race_send_pkt_t* pEvt = NULL;
    uint8_t channel_id = 0;
    uint32_t send_length = 0;

    RACE_LOG_MSGID_I("race_input_command_handler, race_input_cmd[0x%X]", 1, race_input_cmd);

    //g_race_input_command
    race_pkt_t* pCmd = (race_pkt_t*)race_input_cmd;
    if (pCmd)
    {
        channel_id = (uint8_t)race_get_channel_id_by_port_handle(port_handle);
        if ((pEvt = RACE_CmdHandler(pCmd, channel_id)))
        {
            send_length = race_port_send_data(port_handle, (uint8_t*)&pEvt->race_data, pEvt->length);
            if (send_length == pEvt->length) {
                ret = RACE_STATUS_OK;
            } else {
                ret = RACE_STATUS_ERROR;
            }
            race_mem_free(pEvt);
        } else if (pCmd->hdr.type == RACE_TYPE_COMMAND_WITHOUT_RSP) {
            ret = RACE_STATUS_OK;
        }
    }
    return ret;
}

void race_flush_msg(uint32_t port, void* msg)
{
    uint8_t *pMsg;
    uint32_t length = (uint32_t)strlen((char *)msg)+1;


    RACE_LOG_MSGID_I("race_flush_msg length = %d",1, length);
    pMsg = (uint8_t *)race_mem_alloc(length);

    if (pMsg != NULL)
    {
        STRNCPY((char *)pMsg, msg);
        race_port_send_data(port, (uint8_t*)pMsg, length);
        race_mem_free(pMsg);
    }
}

void race_input_data_processing(race_general_msg_t *msg)
{
    race_status_t ret = RACE_STATUS_ERROR;
    ret = race_read_data(msg);
    if (ret == RACE_STATUS_OK)
    {
        RACE_LOG_MSGID_I("race_input_data_processing, ok!", 0);
    }
    else
    {
        RACE_LOG_MSGID_E("race_input_data_processing fail, ret[%d]", 1, ret);
        race_flush_msg(G_GET_UART_PORT_HANDLE(), RACE_COMMAND_ERROR_CHECK);
    }
}
void race_send_cache(race_general_msg_t *msg)
{
    uint32_t port_handle = 0;
    // send the cache output data
    RACE_LOG_MSGID_I("g_send_data_cache_left_len[%ld]", 1, g_race_send_data_cache_left_len);
    port_handle = race_get_serial_port_handle(msg->dev_t);
    if (g_race_send_data_cache_left_len > 0)
    {
        race_send_data(port_handle, NULL, 0);
    }
}
void race_handle_command(race_general_msg_t *msg)
{
    race_status_t status = RACE_STATUS_ERROR;
    uint32_t port_handle = 0;
    /* handling someone RACE command. Need to wait the response and then could handle the next RACE command */

    if (!msg)
    {
        RACE_LOG_MSGID_E("race_handle_command fail, msg NULL", 0);
        return;
    }
    else if (!msg->msg_data)
    {
        RACE_LOG_MSGID_E("race_handle_command fail, msg_data NULL", 0);
        return;
    }

    port_handle = race_get_serial_port_handle(msg->dev_t);
    if (port_handle == 0)
    {
        RACE_LOG_MSGID_E("race_handle_command fail, port_handle 0", 0);
        return;
    }
#ifdef AIR_DONGLE_RELAY_RACE_PACKET_ENABLE
    RACE_LOG_MSGID_I("msg dev_t: 0x%x, msg_flag: 0x%x", 2, msg->dev_t, msg->msg_flag);
    race_pkt_t* pCmd = (race_pkt_t*)msg->msg_data;

    if (RACE_REMOTE_HID_FLAG_SPP == msg->msg_flag) { /* usb race cmd for remote */
        /* for ull EDR dongle, relay race cmd to remote headset/earbuds */
        status = RACE_STATUS_OK;
        race_flush_raw_cmd(msg->msg_data, RACE_SERIAL_PORT_TYPE_SPP);
    } else if (RACE_REMOTE_HID_FLAG_BLE == msg->msg_flag) {
        status = RACE_STATUS_OK;
        race_flush_raw_cmd(msg->msg_data, RACE_SERIAL_PORT_TYPE_BLE);
    } else if ((msg->dev_t == G_GET_SPP_PORT() || msg->dev_t == G_GET_BLE_PORT())
            && (pCmd->hdr.type == RACE_TYPE_RESPONSE || pCmd->hdr.type == RACE_TYPE_NOTIFICATION)) { /* remote spp_air/ble race response/notification */
        status = RACE_STATUS_OK;
        race_pkt_t* pCmd = (race_pkt_t*)msg->msg_data;
        /* These commands should not be relayed to PC. */
        if (pCmd->hdr.id >= RACE_ID_NO_REMOTE_RANGE_BEGIN && pCmd->hdr.id <= RACE_ID_NO_REMOTE_RANGE_END){
            status = race_input_command_handler(port_handle, msg->msg_data);
        } else {
            /* dongle receive race packet from spp_air/ble, need relay to USB -> PC */
            race_flush_raw_cmd(msg->msg_data, RACE_SERIAL_PORT_TYPE_USB);
        }
    } else { /* local race packet handle */
        status = race_input_command_handler(port_handle, msg->msg_data);
    }
#elif (defined AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
    RACE_LOG_MSGID_I("msg dev_t: 0x%x, msg_flag: 0x%x", 2, msg->dev_t, msg->msg_flag);
    race_pkt_t* pCmd = (race_pkt_t*)msg->msg_data;

    if (RACE_REMOTE_HID_FLAG_COSYS == msg->msg_flag) { /* usb race cmd for remote */
        /* for 68 master, relay race cmd to 65 slave */
        status = RACE_STATUS_OK;
        race_flush_raw_cmd(msg->msg_data, RACE_SERIAL_PORT_TYPE_COSYS);
    } else if (RACE_REMOTE_HID_FLAG_EMCU == msg->msg_flag) {
        /* relay to ex_mcu. */
        status = RACE_STATUS_OK;
        race_flush_raw_cmd(msg->msg_data, RACE_SERIAL_PORT_TYPE_EMCU);
    } else if ((msg->dev_t == G_GET_COSYS_PORT() || msg->dev_t == G_GET_EMCU_PORT())
            && (pCmd->hdr.type == RACE_TYPE_RESPONSE || pCmd->hdr.type == RACE_TYPE_NOTIFICATION)) {
        /* for 68 master, relay race cmd response to PC.*/
        status = RACE_STATUS_OK;
        race_flush_raw_cmd(msg->msg_data, RACE_SERIAL_PORT_TYPE_USB);
    } else if (race_cmd_is_to_remote(pCmd)) {
        status = RACE_STATUS_OK;
        uint8_t channel_id = race_get_channel_id_by_port_handle(race_get_serial_port_handle(msg->dev_t));
        race_relay_send_cosys_to_remote_internal(pCmd, pCmd->hdr.length + 4, channel_id);
    } else { /* local race packet handle */
        status = race_input_command_handler(port_handle, msg->msg_data);
    }
#elif (defined AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
    race_pkt_t* pCmd = (race_pkt_t*)msg->msg_data;
    if (race_cmd_is_to_remote(pCmd)) {
        status = RACE_STATUS_OK;
        uint8_t channel_id = race_get_channel_id_by_port_handle(race_get_serial_port_handle(msg->dev_t));
        race_relay_send_cosys_to_remote_internal(pCmd, pCmd->hdr.length + 4, channel_id);
    } else {
        status = race_input_command_handler(port_handle, msg->msg_data);
    }
#else
    status = race_input_command_handler(port_handle, msg->msg_data);
#endif

    if (status == RACE_STATUS_OK)
        RACE_LOG_MSGID_I("race_handle_command ok", 0);
    else
        RACE_LOG_MSGID_E("race_handle_command fail", 0);

    race_mem_free(msg->msg_data);
}

#ifdef MTK_ONLINE_LOG_TO_APK_ENABLE
#define RACE_HEARDER_SIZE        (12)
#define RACE_DATA_SIZE           (500)
#define SEND_THREAD_HOLD_SIZE    (2)
#define SPP_BT_MTU_MAX_SIZE  (RACE_DATA_SIZE - RACE_HEARDER_SIZE)
extern void online_log_race_sent_data(uint32_t *curr_log_addr, uint32_t *curr_log_size);
extern void online_log_race_update_readp(uint32_t read_length);
volatile uint32_t retry_send;
volatile uint32_t retry_read_addr;
volatile uint32_t retry_log_size;

ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN static uint8_t g_syslog_buffer[SPP_BT_MTU_MAX_SIZE+RACE_HEARDER_SIZE];
void race_send_online_Log_data(race_general_msg_t *msg)
{
    uint32_t read_address;
    uint32_t curr_log_size;
    uint32_t length, log_length, send_len;
    uint32_t port_handle;
    uint32_t mask;
    uint32_t send_times;

    (void ) msg;

    if (retry_send == 0) {
        //HW write point is changed, if log is wrapped which will cause issue
        online_log_race_sent_data(&read_address, &curr_log_size);
    } else {
        read_address = retry_read_addr;
        curr_log_size = retry_log_size;
        if (retry_send > 100) {
            assert(0);
        }
    }

    send_times = 0;
    g_syslog_buffer[0] = 6;               //OS_OFFSET_OF(RACE_IPC_STRU, payload);
    g_syslog_buffer[1] = 0;
    //g_syslog_buffer[2] = (length+6);    //sizeof(RACE_COMMON_HDR_STRU) + dat_len;
   // g_syslog_buffer[3] = (length+6)>>8;
    g_syslog_buffer[4] = 5;               //channel_id: SERIAL_PORT_DEV_BT_SPP
    g_syslog_buffer[5] = 0xCC;
    g_syslog_buffer[6] = 0x05;
    g_syslog_buffer[7] = 0x5D;
    //g_syslog_buffer[8] = (length+2);
    //g_syslog_buffer[9] = (length+2)>>8;
    g_syslog_buffer[10] = 0xFF;
    g_syslog_buffer[11] = 0xFF;

    length = SPP_BT_MTU_MAX_SIZE;
    log_length = curr_log_size;
    port_handle = race_get_port_handle_by_channel_id(RACE_SERIAL_PORT_TYPE_SPP);

    while (log_length > 0) {
        if (log_length > SPP_BT_MTU_MAX_SIZE) {
            length = SPP_BT_MTU_MAX_SIZE;
        } else {
            length = log_length;    //packet size
        }

        //g_syslog_buffer[0] = 6;               //OS_OFFSET_OF(RACE_IPC_STRU, payload);
        //g_syslog_buffer[1] = 0;
        //g_syslog_buffer[2] = (length+6);    //sizeof(RACE_COMMON_HDR_STRU) + dat_len;
        //g_syslog_buffer[3] = (length+6)>>8;
        //g_syslog_buffer[4] = 5;               //channel_id: SERIAL_PORT_DEV_BT_SPP
        //g_syslog_buffer[5] = 0xCC;
        //g_syslog_buffer[6] = 0x05;
        //g_syslog_buffer[7] = 0x5D;
        //g_syslog_buffer[8] = (length+2);
        //g_syslog_buffer[9] = (length+2)>>8;
        //g_syslog_buffer[10] = 0xFF;
        //g_syslog_buffer[11] = 0xFF;
        g_syslog_buffer[2] = (length+6);    //sizeof(RACE_COMMON_HDR_STRU) + dat_len;
        g_syslog_buffer[3] = (length+6)>>8;
        //data is 05 5D LENGTH FF FF Datas;
        //LENGTH includes FF FF datas
        g_syslog_buffer[8] = (length+2);
        g_syslog_buffer[9] = (length+2)>>8;

        memcpy(&g_syslog_buffer[12], (uint8_t *)(read_address + send_times * SPP_BT_MTU_MAX_SIZE), length);
        send_len = race_port_send_data(port_handle, &g_syslog_buffer[6], (length+6));
        if ((send_len == 0) || (send_len != (length+6))) {
            hal_nvic_save_and_set_interrupt_mask(&mask);
            retry_send++;
            retry_read_addr = read_address + send_times * SPP_BT_MTU_MAX_SIZE;
            retry_log_size = curr_log_size - send_times * SPP_BT_MTU_MAX_SIZE;
            hal_nvic_restore_interrupt_mask(mask);
            return;
        }

        send_times++;
        retry_send = 0;
        retry_read_addr = 0;
        retry_log_size = 0;
        online_log_race_update_readp(length);
        log_length -= length;
        if (log_length > SEND_THREAD_HOLD_SIZE) {
            /* Delay 60ms to send next packege. */
            vTaskDelay(60 / portTICK_PERIOD_MS);
        }
    }
}
#endif

/* msg will be copied into message queue and should be freed after this API returns if it's allocated dynamically. */
RACE_ERRCODE race_send_msg(race_general_msg_t *msg)
{
    uint16_t race_queue_num = 0;
    atci_status_t status = ATCI_STATUS_ERROR;

    if (!msg)
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }
    race_queue_num = atci_queue_get_item_num(g_race_serial_port_queue);

    if (race_queue_num >= RACE_LOCAL_QUEUE_LENGTH)
    {
        RACE_LOG_MSGID_E("race_send_msg, race_queue_num over[%d]", 1, race_queue_num);
        assert(0);
    }

    status = atci_queue_send(g_race_serial_port_queue, (void*)msg);

    //RACE_LOG_MSGID_I("race_send_msg, status[%d], msg_id[%d]", 2, status, msg->msg_id);

    if (status == ATCI_STATUS_OK)
        return RACE_ERRCODE_SUCCESS;
    else
        return RACE_ERRCODE_FAIL;
}

#ifdef MTK_USB_DEMO_ENABLED
void race_mux_usb_callback(void *param)
{
    bool plug_in = (bool)param;
    race_general_msg_t msg_queue_item = {0};

    if (plug_in)
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_USB_PLUG_IN;
    else
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_USB_PLUG_OUT;

    race_send_msg(&msg_queue_item);
}

void race_mux_usb_plug(bool state)
{
    RACE_LOG_MSGID_I("race_mux_usb_plug, state[%d]", 1, state);
    if (state)
        race_serial_port_open(RACE_SERIAL_PORT_TYPE_USB, G_GET_USB_PORT());
    else
        race_serial_port_close(RACE_SERIAL_PORT_TYPE_USB);
}
#endif

static void race_sleep_lock_timer_callback(TimerHandle_t xtimer)
{
    race_sleep_lock_timer_alive = false;
    if(hal_sleep_manager_is_sleep_handle_alive(race_uart_rx_sleep_handle) == true)
    {
        hal_sleep_manager_unlock_sleep(race_uart_rx_sleep_handle);
        RACE_LOG_MSGID_I("race_sleep_lock_timer_callback, unlock", 0);
    }
}

static void race_sleep_timer_init()
{
    if(race_uart_rx_sleep_handle == 0xFF)
    {
        race_uart_rx_sleep_handle = hal_sleep_manager_set_sleep_handle("race");
        RACE_LOG_MSGID_I("race_sleep_timer_init, handle[%d]",1, race_uart_rx_sleep_handle);
    }

    if(race_sleep_lock_timer == NULL)
    {
        race_sleep_lock_timer = xTimerCreate("race_sleep_timer", (10 * 1000 / portTICK_PERIOD_MS), pdFALSE, NULL, race_sleep_lock_timer_callback);
        if(race_sleep_lock_timer == NULL)
        {
            if(hal_sleep_manager_is_sleep_handle_alive(race_uart_rx_sleep_handle) == true)
            {
                hal_sleep_manager_unlock_sleep(race_uart_rx_sleep_handle);
            }
            RACE_LOG_MSGID_I("race_sleep_timer_init fail", 0);
            return;
        }
        RACE_LOG_MSGID_I("race_sleep_timer_init done", 0);
    }
}

void race_start_sleep_lock_timer(void)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    BaseType_t ret;

    if(!race_sleep_lock_timer_alive) {
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0) {
            ret = xTimerStartFromISR(race_sleep_lock_timer, &xHigherPriorityTaskWoken);
        } else {
            ret = xTimerStart(race_sleep_lock_timer, 0);
        }
        //RACE_LOG_MSGID_I("race_start_sleep_lock_timer, start ret %d", 1, ret);
    } else {
        if (HAL_NVIC_QUERY_EXCEPTION_NUMBER != 0) {
            ret = xTimerResetFromISR(race_sleep_lock_timer, &xHigherPriorityTaskWoken);
        } else {
            ret = xTimerReset(race_sleep_lock_timer, 0);
        }
        //RACE_LOG_MSGID_I("race_start_sleep_lock_timer, restart ret %d", 1, ret);
    }

    if (ret == pdTRUE) {
        race_sleep_lock_timer_alive = true;
    }

    if(xHigherPriorityTaskWoken == pdTRUE){
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }

    if(hal_sleep_manager_is_sleep_handle_alive(race_uart_rx_sleep_handle) == true)
    {
        hal_sleep_manager_unlock_sleep(race_uart_rx_sleep_handle);
    }
    hal_sleep_manager_lock_sleep(race_uart_rx_sleep_handle);
}

void race_processing(void)
{
    int common_queue_msg_num = 0;
    race_general_msg_t msg_queue_data;

    RACE_LOG_MSGID_I("race_processing, enter", 0);

    while (1) {
        common_queue_msg_num = atci_queue_get_item_num(g_race_serial_port_queue);
        if(common_queue_msg_num > 0)
        {
            RACE_LOG_MSGID_I("race_processing, queue_num[%d]", 1, common_queue_msg_num);
        }
        atci_queue_receive_wait(g_race_serial_port_queue, &msg_queue_data, RACE_MAX_DELAY);
        RACE_LOG_MSGID_I("race_processing, msg_id[%d]", 1, msg_queue_data.msg_id);
        switch (msg_queue_data.msg_id)
        {
        case MSG_ID_RACE_LOCAL_SEND_CMD_IND:
            race_input_data_processing(&msg_queue_data);
            break;
        case MSG_ID_RACE_LOCAL_WRITE_CMD_IND:
            if (msg_queue_data.msg_data == NULL) {
                race_send_cache(&msg_queue_data);
            } else {
                race_send_output_cmd_msg_t *msg_data = (race_send_output_cmd_msg_t *)msg_queue_data.msg_data;
                uint32_t port_handle = race_get_serial_port_handle(msg_queue_data.dev_t);
                RACE_LOG_MSGID_I("race_processing, port [%d] write len[%d]", 2, msg_queue_data.dev_t, msg_data->output_len);
                race_port_send_data_imp(port_handle, msg_data->buff, msg_data->output_len);
                race_mem_free(msg_data);
            }
            break;
        case MSG_ID_RACE_LOCAL_URC_NOTIFY_IND:
            break;
        case MSG_ID_RACE_LOCAL_RSP_NOTIFY_IND:
            race_handle_command(&msg_queue_data);
            break;

        case MSG_ID_RACE_LOCAL_1WIRE_INIT:
            race_serial_port_open(RACE_SERIAL_PORT_TYPE_1WIRE, msg_queue_data.dev_t);
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
            atci_mux_port_reinit(msg_queue_data.dev_t, true, true);
#endif
            break;
        case MSG_ID_RACE_LOCAL_1WIRE_DEINIT:
#if defined(MTK_ATCI_VIA_MUX) && defined(MTK_MUX_ENABLE)
            atci_mux_port_resume(true, true);
#endif
            race_serial_port_close(RACE_SERIAL_PORT_TYPE_1WIRE);
            break;

#if defined(MTK_RACE_CMD_ENABLE) && defined(MTK_MUX_ENABLE) && defined(MTK_USB_DEMO_ENABLED)
        case MSG_ID_RACE_LOCAL_USB_PLUG_IN:
            race_mux_usb_plug(true);
            break;
        case MSG_ID_RACE_LOCAL_USB_PLUG_OUT:
            race_mux_usb_plug(false);
            break;
#endif

        case MSG_ID_RACE_LOCAL_DELAY_NOTI_IND:
        {
            race_noti_delay_msg_process(&msg_queue_data);
            break;
        }

        case MSG_ID_RACE_LOCAL_EVENT_NOTIFY_REQ:
        {
            race_event_notify_msg_process(&msg_queue_data);
            break;
        }

#ifdef RACE_STORAGE_CMD_ENABLE
        case MSG_ID_RACE_LOCAL_ERASE_PARTITION_CONTINUE_IND:
        {
            race_storage_erase_partition_continue_msg_process(&msg_queue_data);
            break;
        }

        case MSG_ID_RACE_LOCAL_STORAGE_SHA256_GENERATE_CONTINUE_IND:
        {
            race_storage_nb_sha256_generate_continue_msg_process((void *)msg_queue_data.msg_data);
            break;
        }
#endif

#ifdef RACE_BT_EVENT_MSG_HDL
        case MSG_ID_RACE_LOCAL_BT_EVENT_IND:
        {
            race_bt_event_ind_msg_process((race_bt_event_msg_info_struct *)msg_queue_data.msg_data);
            race_mem_free(msg_queue_data.msg_data);
            break;
        }
#endif

#ifdef RACE_LPCOMM_ENABLE
        case MSG_ID_RACE_LOCAL_LPCOMM_DATA_RECV_IND:
        {
            race_lpcomm_data_recv_msg_process(&msg_queue_data);
            race_mem_free(msg_queue_data.msg_data);
            break;
        }

        case MSG_ID_RACE_LOCAL_TIMER_EXPIRATION_IND:
        {
            race_timer_expiration_msg_process();
            break;
        }
#endif

#ifdef RACE_FOTA_CMD_ENABLE
        case MSG_ID_RACE_LOCAL_FOTA_STOP_IND:
        {
            race_fota_stop_msg_process(&msg_queue_data);
            if (msg_queue_data.msg_data)
            {
                race_mem_free(msg_queue_data.msg_data);
            }
            break;
        }
#endif

#if (defined RACE_RELAY_CMD_ENABLE) || (defined AIR_RACE_CO_SYS_ENABLE)
        case MSG_ID_RACE_LOCAL_RELAY_RACE_CMD:
        {
            race_cmd_relay_aws_mce_msg_process(&msg_queue_data);
            break;
        }
#endif
        case MSG_ID_RACE_LOCAL_GET_RSSI_CMD:{
#ifdef RACE_BT_CMD_ENABLE
            race_bt_notify_rssi();
#endif
            break;
        }
#ifdef MTK_ONLINE_LOG_TO_APK_ENABLE
        case MSG_ID_RACE_LOCAL_SEND_LOG_IND:
            race_send_online_Log_data(&msg_queue_data);
            break;
#endif
        case MSG_ID_RACE_LOCAL_WAKEUP_SLEEP_IND:
            break;

#ifdef RACE_CFU_ENABLE
        case MSG_ID_RACE_LOCAL_CFU_IND:
        {
            race_cfu_msg_processer(&msg_queue_data);
            break;
        }
#endif

        default:
            break;
        }
    }
}

void race_task(void *arg)
{
    while(1)
    {
        race_processing();
    }
}


#ifdef RACE_CREATE_TASK_DYNAMICALLY
bool race_is_task_existed(void)
{
    return g_race_task_hdl != NULL;
}


void race_create_task()
{
#ifdef MTK_AUDIO_TUNING_ENABLED
#define RACE_TASK_STACKSIZE         (1506) /*unit byte!*/
#else
#define RACE_TASK_STACKSIZE         (512) /*unit byte!*/
#endif

#define RACE_TASK_PRIO              (TASK_PRIORITY_NORMAL)
#define RACE_TASK_NAME              ("race command")

    RACE_LOG_MSGID_I("xCreate task : race command, pri[%d]", 1, RACE_TASK_PRIO);

    BaseType_t ret = xTaskCreate(race_task,
                    RACE_TASK_NAME,
                    RACE_TASK_STACKSIZE,
                    NULL,
                    RACE_TASK_PRIO,
                    &g_race_task_hdl);

    if (ret != pdPASS || !g_race_task_hdl) {
        RACE_LOG_MSGID_E("xCreate race task failed, ret[%d]", 1, ret);
    } else {
        RACE_LOG_MSGID_I("xCreate race task succeeded", 0);
    }
}
#endif


#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
static bt_status_t race_rho_srv_allowed_callback(const bt_bd_addr_t *addr)
{
    /* Always PENDING for SPP/BLE/AIRUPDATE needs be disconnected. */
    return BT_STATUS_PENDING;
}

static uint8_t race_rho_get_data_len_callback(const bt_bd_addr_t *addr)
{
    uint8_t len = (uint8_t)sizeof(g_race_bt_connection);
#ifdef AIR_MULTI_POINT_ENABLE
    if (addr != NULL) {
        len = 0;
    }
#endif

    RACE_LOG_MSGID_I("RHO, data len %d", 1, len);
    return len;
}

static bt_status_t race_rho_get_data_callback(const bt_bd_addr_t *addr, void *data)
{
#ifdef AIR_MULTI_POINT_ENABLE
    if (addr != NULL) {
        RACE_LOG_MSGID_E("RHO, addr not null!", 0);
        return BT_STATUS_FAIL;
    }
#endif

    memcpy(data, &g_race_bt_connection, sizeof(g_race_bt_connection));
    RACE_LOG_MSGID_I("RHO, get data", 0);

    return BT_STATUS_SUCCESS;
}

static bt_status_t race_rho_update_data_callback(bt_role_handover_update_info_t *info)
{
    if (info == NULL || info->data == NULL) {
        RACE_LOG_MSGID_E("RHO, update, data null", 0);
        return BT_STATUS_FAIL;
    }

    RACE_LOG_MSGID_I("RHO, update data, len %d", 1, info->length);
    memcpy(&g_race_bt_connection, info->data, info->length);

    bt_bd_addr_t* addr = &(g_race_bt_connection[0].addr);
    RACE_LOG_MSGID_I("RHO, SPP addr %x:%x:%x:%x:%x:%x", 6,
    ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5]);
    addr = &(g_race_bt_connection[1].addr);
    RACE_LOG_MSGID_I("RHO, BLE addr %x:%x:%x:%x:%x:%x", 6,
    ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5]);

    return BT_STATUS_SUCCESS;
}


static void race_rho_srv_status_callback(const bt_bd_addr_t *addr,
                                                 bt_aws_mce_role_t role,
                                                 bt_role_handover_event_t event,
                                                 bt_status_t status)
{
    if (BT_ROLE_HANDOVER_START_IND == event)
    {
        race_send_event_notify_msg(RACE_EVENT_TYPE_BT_RHO_START, NULL);
    }
    else if (BT_ROLE_HANDOVER_PREPARE_REQ_IND == event)
    {
        /* All allow RHO */
        race_send_event_notify_msg(RACE_EVENT_TYPE_BT_RHO_PREPARE, NULL);
    }
    else if (BT_ROLE_HANDOVER_COMPLETE_IND == event)
    {
        RACE_LOG_MSGID_I("RHO result:%d",1, BT_STATUS_SUCCESS == status);
        race_event_send_bt_rho_result_event(BT_STATUS_SUCCESS == status);

        if (BT_STATUS_SUCCESS == status && role == BT_AWS_MCE_ROLE_AGENT) {
            RACE_LOG_MSGID_I("RHO complete, clear g_race_bt_connection", 0);
            memset(&(g_race_bt_connection[0].addr), 0, sizeof(bt_bd_addr_t));
            memset(&(g_race_bt_connection[1].addr), 0, sizeof(bt_bd_addr_t));
        }
    }
}
#endif


#ifdef MTK_RACE_EVENT_ID_ENABLE
RACE_ERRCODE race_event_cb(int32_t register_id, race_event_type_enum event_type, void *param, void *user_data)
#else
RACE_ERRCODE race_event_cb(race_event_type_enum event_type, void *param, void *user_data)
#endif
{
#ifdef MTK_RACE_EVENT_ID_ENABLE
    RACE_LOG_MSGID_I("race_event_cb, register_id[%d] event_type[%d] param[0x%X] user_data[0x%X]", 4, register_id, event_type, param, user_data);

    if (g_race_event_register_id != register_id)
    {
        RACE_LOG_MSGID_E("race_event_cb, register_id does not match! register_id[%d], g_register_id[%d]", 2, register_id, g_race_event_register_id);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }
#else
    RACE_LOG_MSGID_I("race_event_cb, event_type[%d] param[0x%X] user_data[0x%X]", 3, event_type, param, user_data);
#endif

    switch (event_type)
    {
        case RACE_EVENT_TYPE_CONN_BLE_DISCONNECT:
        case RACE_EVENT_TYPE_CONN_SPP_DISCONNECT:
#ifdef MTK_AIRUPDATE_ENABLE
        case RACE_EVENT_TYPE_CONN_AIRUPDATE_DISCONNECT:
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
        case RACE_EVENT_TYPE_CONN_IAP2_DISCONNECT:
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        case RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_DISCONNECT:
#endif
        case RACE_EVENT_TYPE_CONN_USB_DISCONNECT:
        {
            if (RACE_EVENT_TYPE_CONN_BLE_DISCONNECT == event_type)
            {
                ble_msg_item.isNewRaceCmd = TRUE;
                if (ble_msg_item.msg_data)
                {
                    race_mem_free(ble_msg_item.msg_data);
                    ble_msg_item.msg_data = NULL;
                }
            }

            /* Normally, only Agent will receive BLE_DISC, SPP_DISC events. */
            if (RACE_EVENT_TYPE_CONN_SPP_DISCONNECT == event_type)
            {
                spp_msg_item.isNewRaceCmd = TRUE;
                if (spp_msg_item.msg_data)
                {
                    race_mem_free(spp_msg_item.msg_data);
                    spp_msg_item.msg_data = NULL;
                }
            }
#ifdef MTK_AIRUPDATE_ENABLE
            if (RACE_EVENT_TYPE_CONN_AIRUPDATE_DISCONNECT == event_type)
            {
                airupdate_msg_item.isNewRaceCmd = TRUE;
                if (airupdate_msg_item.msg_data)
                {
                    race_mem_free(airupdate_msg_item.msg_data);
                    airupdate_msg_item.msg_data = NULL;
                }
            }
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
            if (RACE_EVENT_TYPE_CONN_IAP2_DISCONNECT == event_type)
            {
                iap2_msg_item.isNewRaceCmd = TRUE;
                if (iap2_msg_item.msg_data)
                {
                    race_mem_free(iap2_msg_item.msg_data);
                    iap2_msg_item.msg_data = NULL;
                }
            }
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
            if (RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_DISCONNECT == event_type)
            {
                gatt_over_bredr_msg_item.isNewRaceCmd = TRUE;
                if (gatt_over_bredr_msg_item.msg_data)
                {
                    race_mem_free(gatt_over_bredr_msg_item.msg_data);
                    gatt_over_bredr_msg_item.msg_data = NULL;
                }
            }
#endif

            if (RACE_EVENT_TYPE_CONN_USB_DISCONNECT == event_type)
            {
                usb_msg_item.isNewRaceCmd = TRUE;
                if (usb_msg_item.msg_data)
                {
                    race_mem_free(usb_msg_item.msg_data);
                    usb_msg_item.msg_data = NULL;
                }
            }

            break;
        }

        case RACE_EVENT_TYPE_CONN_BLE_CONNECT:
        case RACE_EVENT_TYPE_CONN_SPP_CONNECT:
#ifdef MTK_AIRUPDATE_ENABLE
        case RACE_EVENT_TYPE_CONN_AIRUPDATE_CONNECT:
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
        case RACE_EVENT_TYPE_CONN_IAP2_CONNECT:
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
        case RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_CONNECT:
#endif
        {
#ifdef RACE_FIND_ME_ENABLE
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
            race_cmd_set_find_me_trans_method(event_type);
#endif
#endif
            break;
        }

#ifdef RACE_AWS_ENABLE
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
        case RACE_EVENT_TYPE_BT_RHO_START:
        {
            /* Warning: When this event is received by Partner before RHO, RHO may have already been done.
                        * This is because BT task's priority is much higher than race task's. Need add mutex protect for
                        * race timer and stop the timer in race_rho_srv_status_callback(). However, there's no problem
                        * currently because the timer's timeout value is long enough.
                        */
            /* Start RHO timer when processing Role_Switch cmd. Stop RHO timer on receiving RHO_SRV's START event,
                        * because if RHO START event is received, RHO END will be received for sure.
                        */
            race_timer_smart_stop(race_lpcomm_get_rho_timer_id(), NULL);
            race_lpcomm_set_rho_timer_id(RACE_TIMER_INVALID_TIMER_ID);
            break;
        }

        /* Only Agent will receive RHO_PREPARE Event. */
        case RACE_EVENT_TYPE_BT_RHO_PREPARE:
        {
            /* No need to receive the fake rsp because RHO may have already been done when receiving the fake rsp. */
#ifdef RACE_LPCOMM_RETRY_ENABLE
            race_lpcomm_retry_cancel(RACE_APP_ID_ALL, FALSE);
#endif
#ifndef RACE_RHO_WITHOUT_SMARTPHONE_DISCONNECT_ENABLE
            /* When the serial port is closed, DISCONNECT event triggered by BT_AIR module will not
                        * be received. Therefore, send DISCONNECT event here.
                        */
            race_serial_port_close(RACE_SERIAL_PORT_TYPE_SPP);
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_SPP_DISCONNECT, NULL);

            race_serial_port_close(RACE_SERIAL_PORT_TYPE_BLE);
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_BLE_DISCONNECT, NULL);

#ifdef MTK_AIRUPDATE_ENABLE
            race_serial_port_close(RACE_SERIAL_PORT_TYPE_AIRUPDATE);
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_AIRUPDATE_DISCONNECT, NULL);
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
            race_serial_port_close(RACE_SERIAL_PORT_TYPE_IAP2);
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_IAP2_DISCONNECT, NULL);
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
            race_serial_port_close(RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR);
            race_send_event_notify_msg(RACE_EVENT_TYPE_CONN_GATT_OVER_BREDR_DISCONNECT, NULL);
#endif
#endif
            bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_RACE_CMD);
            break;
        }
#endif

        case RACE_EVENT_TYPE_BT_RHO_RESULT:
        {
            race_lpcomm_set_role_switch_enable(FALSE);
#ifndef RACE_RHO_WITHOUT_SMARTPHONE_DISCONNECT_ENABLE
            /* Only when RACE_AWS_ENABLE is defined, will this event be received. */
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
            RACE_LOG_MSGID_I("open ports", 0);
            race_serial_port_open(RACE_SERIAL_PORT_TYPE_SPP,
                                  SERIAL_PORT_DEV_BT_SPP);
            race_serial_port_open(RACE_SERIAL_PORT_TYPE_BLE,
                                  SERIAL_PORT_DEV_BT_LE);
#ifdef MTK_AIRUPDATE_ENABLE
            race_serial_port_open(RACE_SERIAL_PORT_TYPE_AIRUPDATE,
                                  SERIAL_PORT_DEV_BT_AIRUPDATE);
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
            race_serial_port_open(RACE_SERIAL_PORT_TYPE_IAP2,
                                  SERIAL_PORT_DEV_IAP2_SESSION2);
#endif
#ifdef MTK_GATT_OVER_BREDR_ENABLE
            race_serial_port_open(RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR,
                                  SERIAL_PORT_DEV_BT_GATT_OVER_BREDR);
#endif

#endif
#endif
            break;
        }
#endif

        default:
            break;
    }

    return RACE_ERRCODE_SUCCESS;
}

void race_1wire_init(serial_port_dev_t port, uint32_t baudrate)
{
    race_general_msg_t msg_queue_item;

    g_race_1wire_baudrate = baudrate;
    RACE_LOG_MSGID_I("race_1wire_init, port[%d], baudrate[%d]", 2, port, baudrate);
    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_1WIRE_INIT;
    msg_queue_item.dev_t = port;
    msg_queue_item.msg_data = NULL;
    race_send_msg(&msg_queue_item);
}

void race_1wire_deinit(void)
{
    race_general_msg_t msg_queue_item;

    RACE_LOG_MSGID_I("race_1wire_deinit", 0);
    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_1WIRE_DEINIT;
    msg_queue_item.dev_t = G_GET_1WIRE_PORT();
    msg_queue_item.msg_data = NULL;
    race_send_msg(&msg_queue_item);
}

void race_init (void)
{
    int32_t ret = 0;
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
    bt_role_handover_callbacks_t rho_srv_callbacks = {race_rho_srv_allowed_callback,
                                                      race_rho_get_data_len_callback,
                                                      race_rho_get_data_callback,
                                                      race_rho_update_data_callback,
                                                      race_rho_srv_status_callback};
#endif

    RACE_LOG_MSGID_I("race_init", 0);

#ifdef RACE_BT_CMD_ENABLE
    /* Listen to BT Events */
    ret = bt_callback_manager_register_callback(bt_callback_type_app_event,
                                                MODULE_MASK_SYSTEM | MODULE_MASK_GAP | MODULE_MASK_HFP | MODULE_MASK_A2DP,
#ifdef RACE_BT_EVENT_MSG_HDL
                                                (void*)race_bt_app_event_handler);
#else
                                                (void*)race_bt_event_process);
#endif

    if (BT_STATUS_SUCCESS != ret)
    {
        RACE_LOG_MSGID_E("race_init fail, ret[%d]", 1, ret);
        return;
    }

#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_RACE_CMD, &rho_srv_callbacks);
#endif

    /* BT Init */
    race_bt_init();
#endif /* RACE_BT_CMD_ENABLE */

#ifdef RACE_LPCOMM_ENABLE
    race_lpcomm_ps_list_init();

#ifdef RACE_LPCOMM_RETRY_ENABLE
    race_lpcomm_retry_init();
#endif
#endif

    race_event_init();
#ifdef MTK_RACE_EVENT_ID_ENABLE
    race_event_register(&g_race_event_register_id, race_event_cb, NULL);
#else
    race_event_register(race_event_cb, NULL);
#endif

#ifdef RACE_FOTA_CMD_ENABLE
    race_fota_init();
#endif

#if defined(MTK_RACE_CMD_ENABLE) && defined(MTK_PORT_SERVICE_ENABLE)
    serial_port_dev_t race_port;
#if (PRODUCT_VERSION != 2822) && !defined(AB156X)
    serial_port_setting_uart_t race_uart_setting;
    if(serial_port_config_read_dev_number("race", &race_port) != SERIAL_PORT_STATUS_OK)
    {
        race_port = CONFIG_RACE_PORT;
        serial_port_config_write_dev_number("race", race_port);
        LOG_MSGID_W(common, "serial_port_config_write_dev_number setting uart", 0);
        race_uart_setting.baudrate = CONFIG_RACE_BAUDRATE;
        serial_port_config_write_dev_setting(race_port, (serial_port_dev_setting_t *)&race_uart_setting);
    }
#else

#if 1
    {
        mux_status_t status;
        mux_port_buffer_t query_port_buffer;
        query_port_buffer.count = 0;
        status = mux_query_port_numbers_from_nvdm("RACE_CMD", (mux_port_buffer_t*)&query_port_buffer);
        if ((status == MUX_STATUS_OK) && (query_port_buffer.count == 1)) {
            race_port = query_port_buffer.buf[0];
        } else {
            race_port = CONFIG_RACE_PORT;
        }
        mux_open_save_to_nvdm(race_port, "RACE_CMD");
    }
#else
    race_port = CONFIG_RACE_PORT;
#endif

#endif

    if ((race_port >= SERIAL_PORT_DEV_UART_TYPE_BEGIN) && (race_port <= SERIAL_PORT_DEV_UART_TYPE_END))
    {
        g_race_uart_port_default = race_port;
    }
    else if ((race_port >= SERIAL_PORT_DEV_USB_TYPE_BEGIN) && (race_port <= SERIAL_PORT_DEV_USB_TYPE_END))
    {
        g_race_usb_port_default = race_port;
#ifdef MTK_USB_DEMO_ENABLED
        usb_case_register_race_callback(race_mux_usb_callback);
#endif
    }
    else
    {
        RACE_LOG_MSGID_E("race_init fail, race_port[%d]", 1, race_port);
        return;
    }

    race_init_port_service(race_port);
    race_sleep_timer_init();
#ifdef AIR_RACE_CO_SYS_ENABLE
    race_cosys_port_init();
    race_cosys_tx_timer_init();
    race_cosys_nvkey_init();
#endif
#endif
}
#endif

