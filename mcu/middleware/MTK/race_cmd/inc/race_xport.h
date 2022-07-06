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

#ifndef RACE_XPORT_H
#define RACE_XPORT_H
#include "stdint.h"
#include "hal_uart.h"
#include "serial_port.h"
#include "memory_attribute.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include "race_cmd.h"
#include "race.h"
#include "bt_type.h"


#ifdef __cplusplus
extern "C"
{
#endif

#define RACE_LOG_F(fmt,arg...)   LOG_I(race, fmt,##arg)
#define RACE_LOG_I(fmt,arg...)   LOG_I(race, fmt,##arg)
#define RACE_LOG_W(fmt,arg...)   LOG_W(race, fmt,##arg)
#define RACE_LOG_E(fmt,arg...)   LOG_E(race, fmt,##arg)
//#define RACE_LOG_D(fmt,arg...)   LOG_E(race, fmt,##arg)
#define RACE_LOG_D(fmt,arg...)

#define RACE_LOG_MSGID_F(fmt,cnt,arg...) LOG_MSGID_I(race,fmt,cnt,##arg)
#define RACE_LOG_MSGID_I(fmt,cnt,arg...) LOG_MSGID_I(race,fmt,cnt,##arg)
#define RACE_LOG_MSGID_W(fmt,cnt,arg...) LOG_MSGID_W(race,fmt,cnt,##arg)
#define RACE_LOG_MSGID_E(fmt,cnt,arg...) LOG_MSGID_E(race,fmt,cnt,##arg)


/* This macro defines the data length of the RACE command response.
   The length defined in response_buf of #race_response_t structure cannot be larger than this macro. */

/* UART related */
//#define RACE_UART_RX_FIFO_ALERT_SIZE        (50)
//#define RACE_UART_RX_FIFO_THRESHOLD_SIZE    (128)
//#define RACE_UART_TX_FIFO_THRESHOLD_SIZE    (51)

#if defined (MTK_ATCI_VIA_PORT_SERVICE) && defined(MTK_PORT_SERVICE_ENABLE)

#define RACE_UART_RX_FIFO_BUFFER_SIZE       SERIAL_PORT_RECEIVE_BUFFER_SIZE
#define RACE_UART_TX_FIFO_BUFFER_SIZE       SERIAL_PORT_RECEIVE_BUFFER_SIZE
#define RACE_TX_BUFFER_SIZE                 SERIAL_PORT_SEND_BUFFER_SIZE
#else

/* RACE Command input data length */
#define RACE_UART_RX_FIFO_BUFFER_SIZE       (1024)
#define RACE_UART_TX_FIFO_BUFFER_SIZE       (1024)
#define RACE_TX_BUFFER_SIZE                 (1024)
#endif

#if defined(MTK_MUX_ENABLE)
#define RACE_MUX_UART_TX_BUFFER_SIZE        (1024)
#define RACE_MUX_UART_RX_BUFFER_SIZE        (1024)
#if defined(MTK_MUX_1WIRE_SLIM_ENABLE)
#define RACE_MUX_1WIRE_UART_TX_BUFFER_SIZE  (1024)
#define RACE_MUX_1WIRE_UART_RX_BUFFER_SIZE  (1024)
#else
#define RACE_MUX_1WIRE_UART_TX_BUFFER_SIZE  (1024)
#if (PRODUCT_VERSION == 2822)
#define RACE_MUX_1WIRE_UART_RX_BUFFER_SIZE  (2048)
#else
#define RACE_MUX_1WIRE_UART_RX_BUFFER_SIZE  (4096+256)
#endif
#endif
#ifdef MTK_USB_DEMO_ENABLED
#define RACE_MUX_USB_TX_BUFFER_SIZE         (2048)
#define RACE_MUX_USB_RX_BUFFER_SIZE         (1024)
#endif
#if defined(MTK_MUX_BT_ENABLE)
#define RACE_MUX_SPP_TX_BUFFER_SIZE         (1024)
#define RACE_MUX_SPP_RX_BUFFER_SIZE         ((891- 530) + 891) //891=895(SPP MTU)-4(BT header); 530 = tool max fota packet
#define RACE_MUX_BLE_TX_BUFFER_SIZE         (1024)
#define RACE_MUX_BLE_RX_BUFFER_SIZE         (1024+256)
#endif
#if defined(MTK_AIRUPDATE_ENABLE)
#define RACE_MUX_AIRUPDATE_TX_BUFFER_SIZE   (1024)
#define RACE_MUX_AIRUPDATE_RX_BUFFER_SIZE   (1024)
#endif
#if defined(MTK_IAP2_VIA_MUX_ENABLE) && defined(MTK_IAP2_PROFILE_ENABLE)
#define RACE_MUX_IAP2_TX_BUFFER_SIZE        (1024)
#define RACE_MUX_IAP2_RX_BUFFER_SIZE        (1024 * 2)
#endif
#if defined(MTK_GATT_OVER_BREDR_ENABLE)
#define RACE_MUX_GATT_OVER_BREDR_TX_BUFFER_SIZE        (1024)
#define RACE_MUX_GATT_OVER_BREDR_RX_BUFFER_SIZE        (1024 * 3)
#endif
#endif

#define RACE_DEBUG_PRINT_ENABLE             (TRUE)

#define RACE_MAX_DELAY                      (portMAX_DELAY)
#define RACE_MAX_LENGTH                     (2000)
#define RACE_LOCAL_QUEUE_LENGTH             (30)


#if (defined RACE_RELAY_CMD_ENABLE) || (defined AIR_RACE_CO_SYS_ENABLE)
#define RACE_CHANNEL_ID_RELAY_CMD_FLAG (0x80)

#define RACE_CHANNEL_ID_SET_RELAY_CMD_FLAG(channel_id) ((channel_id) | RACE_CHANNEL_ID_RELAY_CMD_FLAG)
#define RACE_CHANNEL_ID_CLEAR_RELAY_CMD_FLAG(channel_id) ((channel_id) & (~RACE_CHANNEL_ID_RELAY_CMD_FLAG))
#define RACE_CHANNEL_ID_IS_RELAY_CMD_FLAG_SET(channel_id) ((channel_id) & RACE_CHANNEL_ID_RELAY_CMD_FLAG)
#else
#define RACE_CHANNEL_ID_SET_RELAY_CMD_FLAG(channel_id) (channel_id)
#define RACE_CHANNEL_ID_CLEAR_RELAY_CMD_FLAG(channel_id) (channel_id)
#define RACE_CHANNEL_ID_IS_RELAY_CMD_FLAG_SET(channel_id) (FALSE)
#endif


typedef enum {

    RACE_RESPONSE_FLAG_AUTO_APPEND_LF_CR = 0x00000002,    /**< Auto append "\r\n" at the end of the response string. */
    RACE_RESPONSE_FLAG_URC_FORMAT = 0x00000010,           /**< The URC notification flag. */
    RACE_RESPONSE_FLAG_QUOTED_WITH_LF_CR = 0x00000020,    /**< Auto append "\r\n" at the begining and end of the response string. */
    RACE_RESPONSE_FLAG_APPEND_OK = 0x00000040,            /**< Auto append "OK\r\n" at the end of the response string. */
    RACE_RESPONSE_FLAG_APPEND_ERROR = 0x00000080          /**< Auto append "ERROR\r\n" at the end of the response string. */
} race_response_flag_t;

/** @defgroup race_enum Enums
  * @{
  */

typedef enum {
    RACE_INIT_NONE = 0,
    RACE_INIT_DONE,
} race_init_ststus_t;

typedef enum {
    MSG_ID_RACE_LOCAL_SEND_CMD_IND = 2000,
    MSG_ID_RACE_LOCAL_WRITE_CMD_IND,
    MSG_ID_RACE_LOCAL_URC_NOTIFY_IND,
    MSG_ID_RACE_LOCAL_RSP_NOTIFY_IND,
    MSG_ID_RACE_LOCAL_ERASE_PARTITION_CONTINUE_IND,
    MSG_ID_RACE_LOCAL_DELAY_NOTI_IND,
    MSG_ID_RACE_LOCAL_LPCOMM_RETRY_IND,
    MSG_ID_RACE_LOCAL_TIMER_EXPIRATION_IND,
    MSG_ID_RACE_LOCAL_LPCOMM_DATA_RECV_IND,
    MSG_ID_RACE_LOCAL_BT_ON_IND,
    MSG_ID_RACE_LOCAL_BT_EVENT_IND,
    MSG_ID_RACE_LOCAL_RHO_REQ,
    MSG_ID_RACE_LOCAL_EVENT_NOTIFY_REQ,
    MSG_ID_RACE_LOCAL_FOTA_STOP_IND,
    MSG_ID_RACE_LOCAL_STORAGE_SHA256_GENERATE_CONTINUE_IND,
    MSG_ID_RACE_LOCAL_RELAY_RACE_CMD,
    MSG_ID_RACE_LOCAL_GET_RSSI_CMD,
    MSG_ID_RACE_LOCAL_SEND_LOG_IND,
    MSG_ID_RACE_LOCAL_USB_PLUG_IN,
    MSG_ID_RACE_LOCAL_USB_PLUG_OUT,
    MSG_ID_RACE_LOCAL_1WIRE_INIT,
    MSG_ID_RACE_LOCAL_1WIRE_DEINIT,
    MSG_ID_RACE_LOCAL_WAKEUP_SLEEP_IND,
    MSG_ID_RACE_LOCAL_CFU_IND,
    MSG_ID_RACE_END
} race_msg_id_t;

typedef enum
{
    RACE_SERIAL_PORT_TYPE_NONE,
    RACE_SERIAL_PORT_TYPE_UART,
    RACE_SERIAL_PORT_TYPE_USB,
    RACE_SERIAL_PORT_TYPE_SPP,
    RACE_SERIAL_PORT_TYPE_BLE,
    RACE_SERIAL_PORT_TYPE_AIRUPDATE,
    RACE_SERIAL_PORT_TYPE_IAP2,
    RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR,
    RACE_SERIAL_PORT_TYPE_1WIRE,
#ifdef AIR_RACE_SCRIPT_ENABLE
    RACE_SERIAL_PORT_TYPE_PSEUDO,
#endif
#ifdef AIR_RACE_CO_SYS_ENABLE
    RACE_SERIAL_PORT_TYPE_COSYS,
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
    RACE_SERIAL_PORT_TYPE_EMCU,
#endif
    RACE_SERIAL_PORT_TYPE_MAX
}race_serial_port_type_enum;


/* read serial port data structure */
typedef struct {
    serial_port_dev_t dev_t;
    uint16_t uLength;
    uint16_t uLeftLength;
    bool isNewRaceCmd;
    uint8_t *msg_data;
} race_input_cmd_msg_t;

/* MSG_ID_RACE_LOCAL_SEND_CMD_IND message content structure */
typedef struct {
    char     input_buf[RACE_UART_RX_FIFO_BUFFER_SIZE];
    uint16_t input_len;
    uint32_t flag;

} race_send_input_cmd_msg_t;

/* MSG_ID_RACE_LOCAL_WRITE_CMD_IND message content structure */
typedef struct {
    uint32_t output_len;
    uint8_t buff[0];
} race_send_output_cmd_msg_t;

/* This structure defines the response structure. For more information, please refer to #race_send_response() function. */
typedef struct {
    uint8_t  response_buf[RACE_UART_TX_FIFO_BUFFER_SIZE]; /**< The response data buffer. */
    uint16_t response_len;                                /**< The actual data length of response_buf. */
    uint32_t response_flag;                               /**< For more information, please refer to #atci_response_flag_t. */

} race_response_t;

typedef struct {
    race_init_ststus_t race_init_status;
    race_init_ststus_t race_serial_port_uart;
    race_init_ststus_t race_serial_port_1wire;
    race_init_ststus_t race_serial_port_usb;
    race_init_ststus_t race_serial_port_spp;
    race_init_ststus_t race_serial_port_ble;
    race_init_ststus_t race_serial_port_airupdate;
    race_init_ststus_t race_serial_port_iap2;
    race_init_ststus_t race_serial_port_gatt_over_bredr;
} race_init_ctrl_t;

/**
 * @brief This function initializes the RACE module. It is used to set the port configuration for data transmission.
 * @param[in] port is the serial port number used to data transmission for RACE. For more details about this parameter, please refer to race_serial_port_list_init(port).
 * @return   #RACE_STATUS_OK the RACE initialized successfully. \n
 *               #RACE_STATUS_ERROR the UART initialization or the RACE local initialization failed due to the RACE initialization failure.
 * @note     The #race_init_port_service() can select the UART through HAL UART port directly, or select UART, USB or BT SPP server/client port through port service for data transmission.
 * @par       Example
 * @code
 *       ret = race_init_port_service(port);
 *       if (ret == RACE_STATUS_OK) {
 *          // The RACE initialized successfully.
 *          // Create an RACE task.
 *       } else {
 *          // The RACE initialization failed.
 *       }
 * @endcode
 */
extern race_status_t race_init_port_service(serial_port_dev_t port);


/* internal used APIs */
extern serial_port_dev_t g_race_uart_port;
extern serial_port_dev_t g_race_uart_port_default;
extern serial_port_dev_t g_race_usb_port;
extern serial_port_dev_t g_race_usb_port_default;

extern race_init_ctrl_t g_race_init_ctrl;
extern void* race_mem_alloc(uint32_t size);
extern void race_mem_free(void *buf);
extern race_status_t race_flush_packet(uint8_t *ptr, uint8_t channel_id);
extern race_status_t race_flush_packet_relay(race_pkt_t *race_pkt, uint16_t length, uint8_t channel_id, uint8_t type, uint8_t send_idx);
extern uint32_t race_get_port_handle_by_channel_id(uint8_t channel_id);
extern uint32_t race_port_send_data(uint32_t port, uint8_t* buf, uint32_t buf_len);

extern race_status_t race_read_data(race_general_msg_t *msg);
extern race_status_t race_uart_deinit(void);

bool race_is_task_existed(void);

extern void race_create_task();
extern void race_task(void *arg);

extern void race_processing(void);

#ifdef AIR_RACE_SCRIPT_ENABLE
race_status_t race_pseudo_port_init(void);
#endif

#ifdef AIR_RACE_CO_SYS_ENABLE
race_status_t race_cosys_port_init(void);
#endif

race_status_t race_serial_port_close(race_serial_port_type_enum port_type);
race_status_t race_serial_port_open(race_serial_port_type_enum port_type,
                                          serial_port_dev_t port_num);

extern void race_1wire_init(serial_port_dev_t port, uint32_t baudrate);
extern void race_1wire_deinit(void);

extern race_status_t race_serial_port_uart_init(serial_port_dev_t port);

extern hal_uart_port_t g_atci_uart_port;
extern hal_uart_port_t g_atci_uart_port_default;

race_serial_port_type_enum race_get_port_type_by_channel_id(uint8_t channel_id);
uint32_t race_get_port_handle_by_channel_id(uint8_t channel_id);

uint8_t race_get_channel_id_by_port_type(race_serial_port_type_enum port_type);

extern uint32_t race_get_serial_port_handle(serial_port_dev_t port);

extern race_serial_port_type_enum race_get_channel_id_by_port_handle(uint32_t port_handle);

extern void race_start_sleep_lock_timer();

bt_bd_addr_t *race_get_bt_connection_addr(uint8_t channel_id);

#ifdef RACE_FOTA_ADAPTIVE_MODE_ENABLE
void race_get_device_busy_status(bool *is_busy);
#endif
#ifdef __cplusplus
}
#endif


#endif
