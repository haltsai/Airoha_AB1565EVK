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

#ifndef __MUX_H__
#define __MUX_H__


#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#ifdef __cplusplus
extern "C" {
#endif




/**
* @addtogroup kernel_service
* @{
* @addtogroup MUX
* @{
*/


/** @defgroup mux_macro Macro
 *  @{
 */

/** @brief This macro defines the max MUX user number. */
#ifdef MTK_SINGLE_CPU_ENV
#define MAX_MUX_USER_COUNT 80
#else
#define MAX_MUX_USER_COUNT 16
#endif/* MTK_SINGLE_CPU_ENV */


/** @brief This macro defines the max length MUX user user name. */
#define MAX_USER_NAME    16
/**
 * @}
 */

/** @defgroup mux_enum Enum
 *  @{
 */
/** @brief MUX port type. */
typedef enum {
    MUX_UART_BEGIN,             /**<  port of  MUX_UART_BEGIN*/
    MUX_UART_0 = MUX_UART_BEGIN,/**<  port of  MUX_UART_0*/
    MUX_UART_1,                 /**<  port of  MUX_UART_1*/
    MUX_UART_2,                 /**<  port of  MUX_UART_2*/
    MUX_UART_3,                 /**<  port of  MUX_UART_3*/
    MUX_UART_END = MUX_UART_3,  /**<  port of  MUX_UART_END*/

    MUX_USB_BEGIN,                  /**<  port of  MUX_USB_BEGIN*/
    MUX_USB_COM_1 = MUX_USB_BEGIN,  /**<  port of  MUX_USB_COM_1*/
    MUX_USB_COM_2,                  /**<  port of  MUX_USB_COM_2*/
    MUX_USB_END = MUX_USB_COM_2,    /**<  port of  MUX_USB_END*/

    MUX_I2C_SLAVE_BEGIN,                    /**<  port of  MUX_I2C_SLAVE_BEGIN*/
    MUX_I2C_SLAVE_0 = MUX_I2C_SLAVE_BEGIN,  /**<  port of  MUX_I2C_SLAVE_0*/
    MUX_I2C_SLAVE_END = MUX_I2C_SLAVE_0,    /**<  port of  MUX_I2C_SLAVE_END*/

    MUX_SPI_SLAVE_BEGIN,                    /**<  port of  MUX_SPI_SLAVE_BEGIN*/
    MUX_SPI_SLAVE_0 = MUX_SPI_SLAVE_BEGIN,  /**<  port of  MUX_SPI_SLAVE_0*/
    MUX_SPI_SLAVE_END = MUX_SPI_SLAVE_0,    /**<  port of  MUX_SPI_SLAVE_END*/

    MUX_AIRAPP_BEGIN,                       /**<  port of  MUX_AIRAPP_BEGIN */
    MUX_AIRAPP_0 = MUX_AIRAPP_BEGIN,        /**<  port of  MUX_AIRAPP_0 */
    MUX_AIRAPP_END = MUX_AIRAPP_0,          /**<  port of  MUX_AIRAPP_END */

#if defined(MTK_MUX_BT_ENABLE)
    MUX_BT_BEGIN,                           /**<  port of  MUX_BT_BEGIN*/
    MUX_BT_SPP = MUX_BT_BEGIN,              /**<  port of  MUX_BT_SPP*/
    MUX_BT_BLE,                             /**<  port of  MUX_BT_BLE*/
#if defined(MTK_GATT_OVER_BREDR_ENABLE)
    MUX_BT_GATT_OVER_BREDR,                 /**<  port of  MUX_BT_GATT_OVER_BREDR*/
#endif
    MUX_BT_AIRUPATE,                        /**<  port of  MUX_BT_AIRUPATE*/
    MUX_BT_END = MUX_BT_AIRUPATE,           /**<  port of  MUX_BT_END*/
#endif

#if defined(MTK_MUX_AWS_MCE_ENABLE)
    MUX_AWS_MCE_BEGIN,                      /**<  port of  MUX_AWS_MCE_BEGIN */
    MUX_AWS_MCE = MUX_AWS_MCE_BEGIN,        /**<  port of  MUX_AWS_MCE */
    MUX_AWS_MCE_URGENT,                     /**<  port of  MUX_AWS_MCE_URGENT */
    MUX_AWS_MCE_END = MUX_AWS_MCE_URGENT,   /**<  port of  MUX_AWS_MCE_END */
#endif

#if defined(MTK_IAP2_VIA_MUX_ENABLE)
    MUX_IAP2_BEGIN,                         /**<  port of  MUX_IAP2_BEGIN */
    MUX_IAP2_SESSION1 = MUX_IAP2_BEGIN,     /**<  port of  MUX_IAP2_SESSION1 */
    MUX_IAP2_SESSION2,                      /**<  port of  MUX_IAP2_SESSION2 */
    MUX_IAP2_SESSION3,                      /**<  port of  MUX_IAP2_SESSION3 */
    MUX_IAP2_END = MUX_IAP2_SESSION3,       /**<  port of  MUX_IAP2_END */
#endif

    MUX_PORT_END,   /**<  port of  MUX_PORT_END*/
} mux_port_t;

/** @brief MUX event type. */
typedef enum {
    MUX_EVENT_READY_TO_READ = 1,    /**<  event of  ready to read*/
    MUX_EVENT_READY_TO_WRITE,       /**<  event of  ready to write*/
    MUX_EVENT_CONNECTION,           /**<  event of  connection, ex. UART,USB and BT*/
    MUX_EVENT_DISCONNECTION,        /**<  event of  disconnection, ex.UART,USB and BT*/
    MUX_EVENT_WAKEUP_FROM_SLEEP,    /**<  event of  wakeup,special for uart now.*/
    MUX_EVENT_TRANSMISSION_DONE,    /**<  event of  transmission done,special for uart now.*/
} mux_event_t;

/** @brief MUX status type. */
typedef enum {
    MUX_STATUS_OK,                          /**<  status ok*/
    MUX_STATUS_ERROR,                       /**<  status error*/
    MUX_STATUS_ERROR_PARAMETER,             /**<  status error parameter*/
    MUX_STATUS_ERROR_INIT,                  /**<  status initialization*/
    MUX_STATUS_ERROR_NOT_INIT,              /**<  status uninitialized*/
    MUX_STATUS_ERROR_INITIATED,             /**<  status of have initialized*/
    MUX_STATUS_ERROR_NOT_EXIST,             /**<  status error of not exist*/
    MUX_STATUS_ERROR_INIT_FAIL,             /**<  status of initialize fail*/
    MUX_STATUS_ERROR_SOME_USER_STILL_OPEN,  /**<  status of still have some user opening this mux port, can not to close*/
    MUX_STATUS_ERROR_DEINIT_FAIL,           /**<  status of uninitialized fail*/
    MUX_STATUS_ERROR_TOO_MANY_USERS,        /**<  status of  too many users*/
    MUX_STATUS_USER_RX_QUEUE_EMPTY,         /**<  status of  port user rx package queue empty*/
    MUX_STATUS_USER_RX_BUF_SIZE_NOT_ENOUGH, /**<  status of  Rx buffer not enough*/
    MUX_STATUS_USER_TX_BUF_SIZE_NOT_ENOUGH, /**<  status of  Tx buffer not enough*/
    MUX_STATUS_USER_NOT_OPEN,               /**<  status of not open*/
    MUX_STATUS_USER_ERROR_OF_RX_TX_PROTOCOL,/**<  status of error, user Tx and Rx protocol callback error*/
    MUX_STATUS_ERROR_NVDM_INIT_FAIL,        /**<  status of NVDM initialized fail*/
    MUX_STATUS_ERROR_PORT_SETTING_CHANGED,  /**<  status of port setting changed*/
    MUX_STATUS_ERROR_NVDM_NOT_FOUND,        /**<  status of NVDM not found*/
} mux_status_t;

/** @brief MUX uart flow control type. */
typedef enum {
    MUX_UART_NONE_FLOWCONTROL,  /**<  MUX uart none flow control*/
    MUX_UART_SW_FLOWCONTROL,    /**<  MUX uart SW flow control*/
    MUX_UART_HW_FLOWCONTROL,    /**<  MUX uart HW flow control*/
} mux_uart_fc_t;
/**
 * @}
 */

/** @defgroup mux_struct struct
 *  @{
 */

/** @brief MUX handle type.\n
    bit[16:31]:magic number.\n
    bit[8:15]:user_id.\n
    bit[0:7]:mux port.\n
    */
typedef uint32_t mux_handle_t;

/** @brief MUX buffer type. */
typedef struct {
    uint8_t *p_buf;     /**<  MUX buffer start address*/
    uint32_t buf_size;  /**<  MUX buffer lenght*/
} mux_buffer_t;

/** @brief MUX port setting type. */
typedef struct {
    uint32_t tx_buffer_size;    /**<  MUX tx buffer size*/
    uint32_t rx_buffer_size;    /**<  MUX rx buffer size*/
#ifdef HAL_UART_MODULE_ENABLED
    union {
        struct {
            hal_uart_config_t uart_config;      /**<  MUX uart config*/
            mux_uart_fc_t flowcontrol_type;     /**<  MUX uart flow control setting*/
        } uart;
#endif
        struct {
        } usb;
#ifdef HAL_I2C_SLAVE_MODULE_ENABLED
        struct { /**<  For I2C slave data accuracy, support difference slave address.*/
            uint8_t cr_or_cw_slave_address;    /**<  MUX I2C slave config read or config write slave address, should use slave address when master wants to config read or write .*/
            uint8_t r_slave_address;           /**<  MUX I2C slave read slave address, should use slave address when master wants to read.*/
            uint8_t w_slave_address;           /**<  MUX I2C slave write address, should use slave address when master wants to write.*/
        } i2c_slave;
#endif
#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
        struct {
            hal_spi_slave_config_t slave_config;    /**<  MUX SPI slave config*/
        } spi_slave;
#endif
    } dev_setting;  /**<  MUX port device setting union*/
} mux_port_setting_t;

/** @brief MUX port buffer type from NVDM */
typedef struct {
    uint32_t count;     /**<  query the count of how many ports opened by a designated user by the function of mux_query_user_port_numbers_from_nvdm() */
    uint8_t buf[MUX_PORT_END];  /**<  all of the ports information of the query*/
} mux_port_buffer_t;

/** @brief MUX port user name buffer type */
typedef struct {
    uint32_t count;    /**< MUX Port Service device. */
    char name[20];     /**< Port User name. */
} mux_port_assign_t;


/** @brief MUX callback function type
    When event == MUX_EVENT_READY_TO_READ, data_len means: there have one package data, len is data_len.\n
    When event == MUX_EVENT_READY_TO_WRITE,data_len means: will be always as 0x0.
*/
typedef void (*mux_callback_t)(mux_handle_t handle, mux_event_t event,uint32_t data_len,void*user_data);

/** @brief MUX protocol Tx callback head buffer max len, user should pay attention to void memory crash when implement the function of tx_protocol_callback() to prepare header buffer .*/
#define TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN 32
/** @brief MUX protocol Tx callback tail buffer max len, user should pay attention to void memory crash when implement the function of tx_protocol_callback() to prepare tail buffer.*/
#define TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN 32

/** @brief MUX protocol callback type */
typedef struct {
    void (*tx_protocol_callback)(mux_handle_t handle, const mux_buffer_t payload[],uint32_t buffers_counter,
                mux_buffer_t *head, mux_buffer_t *tail,void *user_data); /**<  To do package, *head and *tail are the parameters out.*/
    void (*rx_protocol_callback)(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
                uint32_t *consume_len, uint32_t *package_len,void *user_data); /**<  To do unpackage, *consume_len and *package_len are the parameters out.*/
    void *user_data;    /**<  A user data.*/
} mux_protocol_t;

/** @brief
 * This structure indicates current available data received when
 * using #MUX_CMD_GET_RX_AVAIL.
 */
typedef struct {
    uint32_t ret_size;               /**< The size of the received data. */
} mux_get_rx_avail_t;

/** @brief
 * This structure indicates current available space to put the transmit data in when
 * using #MUX_CMD_GET_TX_AVAIL.
 */
typedef struct {
    uint32_t ret_size;               /**< The current available space to put the transmit data in. */
} mux_get_tx_avail_t;

/** @brief
 * This structure indicates connection parameters in when
 * using #MUX_CMD_GET_CONNECTION_PARAM.
 */
typedef struct {
    uint8_t remote_address[6];  /**< IAP2 remote address. */
    uint16_t max_packet_size;   /**< IAP2 MAX PACKET SIZE. */
    uint16_t iap2_session_id;   /**< IAP2 SESSION ID. */
} mux_get_connection_param_t;


/** @brief
 * This enum defines the commands for each parameter in #mux_ctrl_para_t.
 */
typedef enum {
    MUX_CMD_GET_TX_AVAIL,               /**< UART or USB or BT special command to get the current available space to store the transmit data. */
    MUX_CMD_GET_RX_AVAIL,               /**< UART or USB or BT special command to get the current available size for the received data. */
    MUX_CMD_GET_TX_BUFFER_STATUS,       /**< UART or USB or BT special command to get the current available space to store the transmit data. */
    MUX_CMD_GET_RX_BUFFER_STATUS,       /**< UART or USB or BT special command to get the current available size for the received data. */
    MUX_CMD_CONNECT,                    /**< special command to do connect. */
    MUX_CMD_DISCONNECT,                 /**< special command to do disconnect. */
    MUX_CMD_GET_CONNECTION_PARAM,       /**< iAP2 special command to get connection parameters. */
    MUX_CMD_CLEAN,                      /**< special command to clean tx/rx buffer data. */
    MUX_CMD_GET_VIRTUAL_TX_AVAIL_LEN,   /**< special command to get tx buffer avail data length. */
    MUX_CMD_GET_VIRTUAL_RX_AVAIL_LEN,   /**< special command to get rx buffer avail data length. */
    MUX_CMD_CLEAN_TX_VIRUTUAL,          /**< special command to clean tx buffer data. */
    MUX_CMD_CLEAN_RX_VIRUTUAL,          /**< special command to clean rx buffer data. */
    MUX_TX_BUFFER_SEND,                 /**< special command to send tx buffer data. */
    MUX_CMD_MAX,                        /**< The maximum number of commands. */
} mux_ctrl_cmd_t;

/** @brief
 * This structure defines the data structure for each command in
 * #mux_ctrl_cmd_t.
 */
typedef union {
    mux_get_tx_avail_t mux_get_tx_avail;      /**< Parameters for port tx buffer free space.  */
    mux_get_rx_avail_t mux_get_rx_avail;      /**< Parameters for port rx buffer free space.  */
    uint32_t mux_virtual_tx_avail_len;        /**< Parameters for port tx buffer avilable data length.  */
    uint32_t mux_virtual_rx_avail_len;        /**< Parameters for port rx buffer avilable data length.  */
    mux_get_connection_param_t mux_get_connection_param; /**< Parameters for iAP2 special get connection parameters command. */
} mux_ctrl_para_t;


/**
 * @}
 */

/** @brief This function is mux port initialize.
 * @param[in] port is the mux port.
 * @param[in] setting is mux port setting.
 * @param[in] protocol_callback is protocol callback function when mux to do send and receive data.
 * @return
 *         MUX_STATUS_OK, MUX port initialize success.
 * @code
 * @endcode
*/
mux_status_t mux_init(mux_port_t port, mux_port_setting_t *setting, mux_protocol_t *protocol_callback);


/** @brief This function is mux port deinitialize.
 * @param[in] port is the mux port.
 * @return
 *         MUX_STATUS_OK, MUX port deinitialize success.
 * @code
 * @endcode
*/
mux_status_t mux_deinit(mux_port_t port);


/** @brief This function is logging port exception initialize, special for mux port as logging port.
 * @param[in] handle is the mux logging user handle.
 * @return
 *         MUX_STATUS_OK, MUX logging port exception initialize success.
 * @code
 * @endcode
*/
mux_status_t mux_exception_init(mux_handle_t handle);


/** @brief This function is logging port send exception data, special for mux port as logging port.
 * @param[in] handle is the mux logging user handle.
 * @param[in] buffers is buffer list want to send.
  * @param[in] buffers_counter is buffer count.
 * @return
 *         MUX_STATUS_OK, MUX exception send data success.
 * @code
 * @endcode
*/
mux_status_t mux_exception_send(mux_handle_t handle, const mux_buffer_t *buffers,uint32_t buffers_counter);


/** @brief This function is mux port open by user.
 * @param[in] port is the mux port.
 * @param[in] user_name is the user name.
 * @param[out] p_handle is handle, user can get the handle by mux_open.
 * @param[in] callback is the callback function for ready to read and read to write.
 * @param[in] user_data the user data, as a parameter of callback.
 * @return
 *         MUX_STATUS_OK, MUX port open success.
 * @code
 * @endcode
*/
mux_status_t mux_open(mux_port_t port, const char *user_name, mux_handle_t *p_handle,mux_callback_t callback,void*user_data);


/** @brief This function is mux port close by user.
 * @param[in] handle is the user handle.
 * @return
 *         MUX_STATUS_OK, MUX port close success.
 * @code
 * @endcode
*/
mux_status_t mux_close(mux_handle_t handle);


/** @brief This function is mux receive data.
 * @param[in] handle is the mux user handle.
 * @param[in] buffer is receive buffer.
 * @param[out] receive_done_data_len is how many data received.
 * @return
 *         MUX_STATUS_OK, MUX receive data success.
 * @code
 * @endcode
*/
mux_status_t mux_rx(mux_handle_t handle, mux_buffer_t *buffer, uint32_t *receive_done_data_len);


/** @brief This function is query the user's name.
 * @param[in] handle is the mux user handle.
 * @param[out] user_name is the user's name.
 * @return
 *         MUX_STATUS_OK, Query user name success.
 * @code
 * @endcode
*/
mux_status_t mux_query_user_name(mux_handle_t handle, const char **user_name);


/** @brief This function is query the user's handle.
 * @param[in] port is the mux port.
 * @param[in] user_name is the user's user_name.
 * @param[out] p_handle is the user's handle.
 * @return
 *         MUX_STATUS_OK, Query user handle success.
 * @code
 * @endcode
*/
mux_status_t mux_query_user_handle(mux_port_t port, const char *user_name, mux_handle_t *p_handle);


/** @brief This function queries how many users are on the specified port.
 * @param[in] port is the specified port.
 * @param[out] user_count is the user quantity.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_user_number(mux_port_t port, uint32_t *user_count);


/** @brief This function queries user specified port name list.
 * @param[in] port is the user specified port.
 * @param[out] user_name is the port users name.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_user_name(mux_port_t port, mux_port_assign_t *user_name);


/** @brief This function is mux send data.
 * @param[in] handle is the mux user handle.
 * @param[in] buffers[] is send buffer list.
 * @param[in] buffers_counter is the count of buffer list which want to send.
 * @param[out] send_done_data_len is how many data sent.
 * @return
 *         MUX_STATUS_OK, MUX send success.
 * @code
 * @endcode
*/
mux_status_t mux_tx(mux_handle_t handle, const mux_buffer_t buffers[],uint32_t buffers_counter,uint32_t *send_done_data_len);


/** @brief This function is mux control operate.
 * @param[in] port is the mux port.
 * @param[in] command is the command want to do.
 * @param[out] para is the parameter which want to return from mux_control.
 * @return
 *         MUX_STATUS_OK, MUX operate success.
 * @code
 * @endcode
*/
mux_status_t mux_control(mux_port_t port, mux_ctrl_cmd_t command, mux_ctrl_para_t *para);


/** @brief This function saves the user's port information to NVDM.
 * @param[in] port is the mux user port.
 * @param[in] user_name is the user's name.
 * @return
 *         MUX_STATUS_OK, MUX save user port success.
 * @code
 * @endcode
*/
mux_status_t mux_open_save_to_nvdm(mux_port_t port, const char *user_name);


/** @brief This function deletes the user's port information from NVDM.
 * @param[in] port is the mux user port.
 * @param[in] user_name is the user's name.
 * @return
 *         MUX_STATUS_OK, MUX delete user port success.
 * @code
 * @endcode
*/
mux_status_t mux_close_delete_from_nvdm(mux_port_t port, const char *user_name);


/** @brief This function reads the port setting from NVDM.
 * @param[in] port is the mux user port.
 * @param[out] setting is the port's configuration.
 * @return
 *         MUX_STATUS_OK, MUX query port setting success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_setting_from_nvdm(mux_port_t port, mux_port_setting_t *setting);


/** @brief This function save the port setting to NVDM.
 * @param[in] port is the mux user port.
 * @param[in] setting is the port's configuration.
 * @return
 *         MUX_STATUS_OK, MUX save port setting success.
 * @code
 * @endcode
*/
mux_status_t mux_save_port_setting_to_nvdm(mux_port_t port, mux_port_setting_t *setting);


/** @brief This function queries how many users are on the specified port from NVDM.
 * @param[in] port is the specified port.
 * @param[out] user_count is the user quantity.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_user_number_form_nvdm(mux_port_t port, uint32_t *user_count);


/** @brief This function queries user specified port name list from NVDM.
 * @param[in] port is the user specified port.
 * @param[out] user_name is the port users name.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_user_name_from_nvdm(mux_port_t port, mux_port_assign_t *user_name);


/** @brief This function queries how many ports the user had opened from NVDM.
 * @param[in] user_name is the user's name.
 * @param[out] port_count is the used port quantity.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_user_port_numbers_from_nvdm(const char *user_name, uint32_t *port_count);


/** @brief This function query the used port index by the user from NVDM.
 * @param[in] user_name is the user's name.
 * @param[out] port is a point, it returns the used ports index.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_numbers_from_nvdm(const char *user_name, mux_port_buffer_t *port);

/**
* @}
* @}
*/


#ifdef __cplusplus
}
#endif


#endif//__MUX_H__

