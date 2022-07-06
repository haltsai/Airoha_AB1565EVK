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
 
#ifndef __HAL_UART_H__
#define __HAL_UART_H__

#include "hal_platform.h"

#ifdef HAL_UART_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup UART
 * @{
 * This section introduces the Universal Asynchronous Receiver/Transmitter (UART) enums and structures.
 *
 * @defgroup hal_uart_enum Enum
 * @{
 */

/** @brief This enum defines return status of the UART HAL public API. User should check return value after calling these APIs. */
typedef enum {
    HAL_UART_STATUS_ERROR_PARAMETER = -4,      /**< Invalid user input parameter. */
    HAL_UART_STATUS_ERROR_BUSY = -3,           /**< UART port is currently in use. */
    HAL_UART_STATUS_ERROR_UNINITIALIZED = -2,  /**< UART port has not been initialized. */
    HAL_UART_STATUS_ERROR = -1,                /**< UART driver detected a common error. */
    HAL_UART_STATUS_OK = 0                     /**< UART function executed successfully. */
} hal_uart_status_t;

/** @brief This enum defines baud rate of the UART frame. */
typedef enum {
    HAL_UART_BAUDRATE_110 = 0,           /**< Defines UART baudrate as 110 bps. */
    HAL_UART_BAUDRATE_300 = 1,           /**< Defines UART baudrate as 300 bps. */
    HAL_UART_BAUDRATE_1200 = 2,          /**< Defines UART baudrate as 1200 bps. */
    HAL_UART_BAUDRATE_2400 = 3,          /**< Defines UART baudrate as 2400 bps. */
    HAL_UART_BAUDRATE_4800 = 4,          /**< Defines UART baudrate as 4800 bps. */
    HAL_UART_BAUDRATE_9600 = 5,          /**< Defines UART baudrate as 9600 bps. */
    HAL_UART_BAUDRATE_19200 = 6,         /**< Defines UART baudrate as 19200 bps. */
    HAL_UART_BAUDRATE_38400 = 7,         /**< Defines UART baudrate as 38400 bps. */
    HAL_UART_BAUDRATE_57600 = 8,         /**< Defines UART baudrate as 57600 bps. */
    HAL_UART_BAUDRATE_115200 = 9,        /**< Defines UART baudrate as 115200 bps. */
    HAL_UART_BAUDRATE_230400 = 10,       /**< Defines UART baudrate as 230400 bps. */
    HAL_UART_BAUDRATE_460800 = 11,       /**< Defines UART baudrate as 460800 bps. */
    HAL_UART_BAUDRATE_921600 = 12,       /**< Defines UART baudrate as 921600 bps. */
#ifdef HAL_UART_FEATURE_3M_BAUDRATE
    HAL_UART_BAUDRATE_3000000 = 13,      /**< Defines UART baudrate as 3000000 bps. */
#endif
    HAL_UART_BAUDRATE_MAX                /**< Defines maximum enum value of UART baudrate. */
} hal_uart_baudrate_t;

/** @brief This enum defines word length of the UART frame. */
typedef enum {
    HAL_UART_WORD_LENGTH_5 = 0,           /**< Defines UART word length as 5 bits per frame. */
    HAL_UART_WORD_LENGTH_6 = 1,           /**< Defines UART word length as 6 bits per frame. */
    HAL_UART_WORD_LENGTH_7 = 2,           /**< Defines UART word length as 7 bits per frame. */
    HAL_UART_WORD_LENGTH_8 = 3            /**< Defines UART word length as 8 bits per frame. */
} hal_uart_word_length_t;

/** @brief This enum defines stop bit of the UART frame. */
typedef enum {
    HAL_UART_STOP_BIT_1 = 0,              /**< Defines UART stop bit as 1 bit per frame. */
    HAL_UART_STOP_BIT_2 = 1,              /**< Defines UART stop bit as 2 bits per frame. */
} hal_uart_stop_bit_t;

/** @brief This enum defines parity of the UART frame. */
typedef enum {
    HAL_UART_PARITY_NONE = 0,            /**< Defines UART parity as none. */
    HAL_UART_PARITY_ODD = 1,             /**< Defines UART parity as odd. */
    HAL_UART_PARITY_EVEN = 2             /**< Defines UART parity as even. */
} hal_uart_parity_t;

/** @brief This enum defines the UART event when an interrupt occurs. */
typedef enum {
    HAL_UART_EVENT_TRANSACTION_ERROR = -1,          /**< Indicates if there is a transaction error when receiving data. */
    HAL_UART_EVENT_READY_TO_READ = 1,               /**< Indicates if there is enough data available in the RX buffer for the user to read from. */
    HAL_UART_EVENT_READY_TO_WRITE = 2               /**< Indicates if there is enough free space available in the TX buffer for the user to write into. */
} hal_uart_callback_event_t;

/**
  * @}
  */


/** @defgroup hal_uart_struct Struct
  * @{
  */

/** @brief This struct defines UART configure parameters. */
typedef struct {
    hal_uart_baudrate_t baudrate;              /**< This field represents the baudrate of the UART frame. */
    hal_uart_word_length_t word_length;        /**< This field represents the word length of the UART frame. */
    hal_uart_stop_bit_t stop_bit;              /**< This field represents the stop bit of the UART frame. */
    hal_uart_parity_t parity;                  /**< This field represents the parity of the UART frame. */
} hal_uart_config_t;

/** @brief This struct defines configuration parameters and TX/RX buffers for the VFIFO DMA associated with a specific UART channel. */
typedef struct {
    uint8_t *send_vfifo_buffer;                /**< This field represents the transmitting user allocated VFIFO buffer. It will only be used by the UART driver to send data, must be non-cacheable and aligned to 4bytes. */
    uint32_t send_vfifo_buffer_size;           /**< This field represents the size of the transmitting VFIFO buffer. */
    uint32_t send_vfifo_threshold_size;        /**< This field represents the threshold of the transmitting VFIFO buffer. VFIFO DMA will trigger an interrupt when the available bytes in VFIFO buffer are lower than the threshold. */
    uint8_t *receive_vfifo_buffer;             /**< This field represents the receiving user allocated VFIFO buffer. It will only be used by the UART driver for receiving data, and must be non-cacheable and align to 4bytes. */
    uint32_t receive_vfifo_buffer_size;        /**< This field represents size of the receiving VFIFO buffer. */
    uint32_t receive_vfifo_threshold_size;     /**< This field represents the threshold of the receiving VFIFO buffer. VFIFO DMA will trigger receive interrupt when available bytes in VFIFO buffer are more than the threshold. */
    uint32_t receive_vfifo_alert_size;         /**< This field represents the threshold size of free space left in the VFIFO buffer that activates the UART's flow control system. */
} hal_uart_dma_config_t;

/**
  * @}
  */


/** @defgroup hal_uart_typedef Typedef
  * @{
  */

/** @brief This typedef defines user's callback function prototype.
 *             This callback function will be called in UART interrupt handler when UART interrupt is raised.
 *             User should call hal_uart_register_callback() to register callbacks to UART driver explicitly.
 *             Note, that the callback function is not appropriate for time-consuming operations. \n
 *             parameter "event" : for more information, please refer to description of #hal_uart_callback_event_t.
 *             parameter "user_data" : a user defined data used in the callback function.
 */
typedef void (*hal_uart_callback_t)(hal_uart_callback_event_t event, void *user_data);


/**
 * @}
 * @}
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif


#ifdef __cplusplus
}
#endif


#endif /* HAL_UART_MODULE_ENABLED */
#endif /* __HAL_UART_H__ */

