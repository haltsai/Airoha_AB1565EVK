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

#ifndef __HAL_I2C_SLAVE_H__
#define __HAL_I2C_SLAVE_H__

#include "hal_platform.h"

#if defined(HAL_I2C_SLAVE_MODULE_ENABLED)

/**
 * @addtogroup HAL
 * @{
 * @addtogroup I2C_SLAVE
 * @{
 * This section describes the programming interfaces of the Inter-Integrated Circuit slave(I2C_SLAVE) driver.
 *
 * @section HAL_I2C_SLAVE_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b DMA                        | Direct Memory Access. For an introduction to DMA, please refer to <a href="https://en.wikipedia.org/wiki/Direct_memory_access"> DMA in Wikipedia</a>.|
 * |\b GPIO                       | General Purpose Inputs-Outputs. For more details, please refer to the @ref GPIO module in HAL. |
 * |\b I2C                        | Inter-Integrated Circuit. I2C is typically used to attach low-speed peripheral ICs to processors and microcontrollers. For an introduction to I2C, please refer to <a href="https://en.wikipedia.org/wiki/I%C2%B2C"> I2C in Wikipedia</a>.|
 * |\b NVIC                       | Nested Vectored Interrupt Controller. NVIC is the interrupt controller of ARM Cortex-M4. For more details, please refer to <a href="http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.100166_0001_00_en/ric1417175922867.html"> NVIC introduction in ARM Cortex-M4 Processor Technical Reference Manual</a>.|
 *
 * @section HAL_I2C_SLAVE_Features_Chapter Supported features
 * - \b Supported \b transaction \b modes
 *  - \b Polling \b mode \b transaction
 *    In polling mode, the functions with the suffix "_polling" will not return any value until the transaction is complete.
 *    After the function returns, the return value should be checked and the error status should be handled.
 *
 *  - \b DMA \b mode \b transaction
 *    In DMA mode, the functions with the suffix "_dma" return a value once the I2C slave and DMA hardware registers are assigned.
 *    These functions do not wait for the transaction to finish. After the transaction finishes, an interrupt is triggered and the user callback in the I2C slave ISR
 *    routine is called. After the function returns, the return value should be checked and the error status should be handled.
 *
 * @section HAL_I2C_SLAVE_Architecture_Chapter Software architecture of the I2C Slave
 * - \b Polling \b mode \b architecture. \n
 *   Polling mode architecture is similar to the polling mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for polling mode architecture.
 * - \b DMA \b mode \b architecture. \n
 *   DMA mode architecture is similar to the interrupt mode architecture in HAL overview. See @ref HAL_Overview_3_Chapter for interrupt mode architecture.
 *
 *
 * @section HAL_I2C_Slave_Driver_Usage_Chapter How to use this driver
 * - \b Using \b I2C \b Slave \b in \b polling \b mode.  \n
 *  To use I2C slave driver in a polling mode, configure GPIO pins to pinmux to SCL and SDA, then call
 *  #hal_pinmux_set_function() to select the pinmux function. After setting the pinmux, call #hal_gpio_set_schmitt() to enable schmitt on
 *  i2c slave pins. Once the data is successfully transmitted, call #hal_i2c_slave_deinit() to release the I2C slave resource for other users.\n
 *  Steps are shown below:
 *  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about hal_gpio_init please refer to GPIO module in HAL.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux or use the EPT tool to apply the pinmux settings.
 *  - Step3: Call #hal_gpio_set_schmitt() to enable schmitt.
 *  - Step4: Call #hal_i2c_slave_init() to initialize the I2C slave.
 *  - Step5: Call #hal_i2c_slave_send_polling() to send data in a polling mode.
 *  - Step6: Call #hal_i2c_slave_receive_polling() to receive data in a polling mode.
 *  - Step7: Call #hal_i2c_slave_deinit() to de-allocate the I2C slave if it is no longer in use.
 *  - Sample code:
 *    @code
 *       int32_t success_size = 0;
 *       hal_i2c_slave_receive_config_t i2cs_receive_config;
 *       hal_i2c_slave_send_config_t i2cs_send_config;
 *
 *       i2cs_receive_config.slave_address = I2C_SLAVE_ADDRESS;
 *       i2cs_receive_config.receive_length = data_length;
 *       i2cs_receive_config.receive_buffer = receive_buff;
 *
 *       i2cs_send_config.slave_address = I2C_SLAVE_ADDRESS;
 *       i2cs_send_config.send_length = data_length;
 *       i2cs_send_config.send_buffer = send_buff;
 *
 *       hal_gpio_init(I2C_SCL_PIN_USED);
 *       hal_gpio_init(I2C_SDA_PIN_USED);

 *       hal_pinmux_set_function(I2C_SCL_PIN_USED, I2C_SCL_MODE_USED);
 *       hal_pinmux_set_function(I2C_SDA_PIN_USED, I2C_SDA_MODE_USED);
 *
 *       hal_gpio_set_schmitt(I2CS_SCL_PIN_USED);
 *       hal_gpio_set_schmitt(I2CS_SDA_PIN_USED);
 *
 *       if(hal_i2c_slave_init(HAL_I2C_SLAVE_0) != HAL_I2C_SLAVE_STATUS_OK) {
 *           //Send data
 *           if (HAL_I2C_SLAVE_STATUS_OK != hal_i2c_slave_send_polling(HAL_I2C_SLAVE_0, &i2cs_send_config, 10000, &success_size)) {
 *               //Error handler;
 *           }
 *           //Receive data
 *           if (HAL_I2C_SLAVE_STATUS_OK != hal_i2c_slave_receive_polling(HAL_I2C_SLAVE_0, &i2cs_receive_config, 10000, &success_size)) {
 *               //Error handler;
 *           }
 *           //Call this function if the I2C slave is no longer in use.
 *           hal_i2c_slave_deinit(HAL_I2C_SLAVE_0);
 *
 *       } else {
 *           //Error handler;
 *       }
 *
 *    @endcode
 *
 * - \b Using \b I2C \b Slave \b in \b DMA \b mode. \n
 *  To use I2C slave driver in DMA mode, configure GPIO pins to pinmux to SCL and SDA, then call
 *  #hal_pinmux_set_function to select the pinmux function. After setting the pinmux, call #hal_gpio_set_schmitt() to enable schmitt on
 *  i2c slave pins. Once the data transaction is complete, call #hal_i2c_slave_deinit() to release the I2C resource to other users.
 *  Steps are shown below:
 *  - Step1: Call #hal_gpio_init() to initialize the pin. For mode details about hal_gpio_init please refer to GPIO module in HAL.
 *  - Step2: Call #hal_pinmux_set_function() to set the GPIO pinmux or use the EPT tool to apply the pinmux settings.
 *  - Step3: Call #hal_gpio_set_schmitt() to enable schmitt.
 *  - Step4: Call #hal_i2c_slave_init() to initialize the I2C slave.
 *  - Step5: Call #hal_i2c_slave_send_dma() to send data in a polling mode.
 *  - Step6: Call #hal_i2c_slave_receive_dma() to receive data in a polling mode.
 *  - Step7: Call #hal_i2c_slave_deinit() to de-allocate the I2C slave if it is no longer in use.
 *  - Sample code:
 *    @code
 *       int32_t success_size = 0;
 *       hal_i2c_slave_receive_config_t i2cs_receive_config;
 *       hal_i2c_slave_send_config_t i2cs_send_config;
 *       hal_i2c_slave_callback_config_t i2cs_callback_config;
 *
 *       i2cs_receive_config.slave_address = I2C_SLAVE_ADDRESS;
 *       i2cs_receive_config.receive_length = data_length;
 *       i2cs_receive_config.receive_buffer = receive_buff;
 *
 *       i2cs_send_config.slave_address = I2C_SLAVE_ADDRESS;
 *       i2cs_send_config.send_length = data_length;
 *       i2cs_send_config.send_buffer = send_buff;
 *
 *       i2cs_callback_config.user_callback = i2c_slave_callback;
 *       i2cs_callback_config.user_data = NULL;
 *
 *       hal_gpio_init(I2C_SCL_PIN_USED);
 *       hal_gpio_init(I2C_SDA_PIN_USED);

 *       hal_pinmux_set_function(I2C_SCL_PIN_USED, I2C_SCL_MODE_USED);
 *       hal_pinmux_set_function(I2C_SDA_PIN_USED, I2C_SDA_MODE_USED);
 *
 *       hal_gpio_set_schmitt(I2CS_SCL_PIN_USED);
 *       hal_gpio_set_schmitt(I2CS_SDA_PIN_USED);
 *
 *       if(hal_i2c_slave_init(HAL_I2C_SLAVE_0) != HAL_I2C_SLAVE_STATUS_OK) {
 *           //Send data
 *           if (hal_i2c_slave_send_dma(HAL_I2C_SLAVE_0, &i2cs_send_config, 3000, &i2cs_callback_config) != HAL_I2C_SLAVE_STATUS_OK) {
 *               //Error handler;
 *           }
 *
 *           // wait i2c slave event here
 *
 *           //Receive data after send data is finished
 *           if (hal_i2c_slave_receive_dma(HAL_I2C_SLAVE_0, &i2cs_receive_config, 3000, &i2cs_callback_config) != HAL_I2C_SLAVE_STATUS_OK) {
 *               //Error handler;
 *           }
 *
 *           // wait i2c slave event here
 *
 *           //Call this function if the I2C slave is no longer in use.
 *           hal_i2c_slave_deinit(HAL_I2C_SLAVE_0);
 *
 *       } else {
 *           //Error handler;
 *       }
 *
 *    @endcode
 *    @code
 *       // Callback function sample code. This function should be passed to driver while calling the function #hal_I2C_SLAVE_register_callback().
 *       void i2c_slave_callback(uint8_t slave_address, hal_i2c_slave_event_t event, int32_t success_size, void *user_data)
 *       {
 *           if(HAL_I2C_SLAVE_EVENT_TX_LTSIZE_ERROR == event) {
 *               // The sent data size is less than the required size
 *               // Error handler;
 *           } else if(HAL_I2C_SLAVE_EVENT_RX_GTSIZE_ERROR == event) {
 *               // The received data size is greater than the required size
 *               // Error handler;
 *           } else if(HAL_I2C_SLAVE_EVENT_RX_LTSIZE_ERROR == event) {
 *               // The received data size is less than the required size
 *               // Error handler;
 *           } else if(HAL_I2C_SLAVE_EVENT_RX_LOSTDATA_ERROR == event) {
 *               // Some data are lost during transaction
 *               // Error handler;
 *           } else if(HAL_I2C_SLAVE_EVENT_TIMEOUT_ERROR == event) {
 *               // A timeout error occurred during transaction
 *               // Error handler;
 *           } else if(HAL_I2C_SLAVE_EVENT_SUCCESS == event) {
 *               // Do something;
 *               // The slave_address indicates which slave address is in using.
 *           }
 *       }
 *
 *    @endcode
 *
 * @}
 * @}
 */


/**
* @addtogroup HAL
* @{
* @addtogroup I2C_SLAVE
* @{
*/

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/

/** @defgroup hal_i2c_slave_enum Enum
  * @{
  */

/** @brief This enum defines the HAL interface return value. */
typedef enum
{
    HAL_I2C_SLAVE_STATUS_ERROR = -4,                        /**<  An error occurred and the transaction has failed. */
    HAL_I2C_SLAVE_STATUS_ERROR_BUSY = -3,                   /**<  The I2C bus is busy, an error occurred. */
    HAL_I2C_SLAVE_STATUS_INVALID_PORT_NUMBER = -2,          /**<  A wrong port number is given. */
    HAL_I2C_SLAVE_STATUS_INVALID_PARAMETER = -1,            /**<  A wrong parameter is given. */
    HAL_I2C_SLAVE_STATUS_OK = 0                             /**<  No error occurred during the function call. */
} hal_i2c_slave_status_t;

/** @brief This enum defines the transaction result event. These result is passed through a callback function. */
typedef enum {
    HAL_I2C_SLAVE_EVENT_TX_LTSIZE_ERROR = -5,               /**<  An error occurred during transaction. The sent data size is less than the required size. */
    HAL_I2C_SLAVE_EVENT_RX_GTSIZE_ERROR = -4,               /**<  An error occurred during transaction. The received data size is greater than the required size. */
    HAL_I2C_SLAVE_EVENT_RX_LTSIZE_ERROR = -3,               /**<  An error occurred during transaction. The received data size is less than the required size. */
    HAL_I2C_SLAVE_EVENT_RX_LOSTDATA_ERROR = -2,             /**<  An error occurred during transaction. Some data are lost during transaction. */
    HAL_I2C_SLAVE_EVENT_TIMEOUT_ERROR = -1,                 /**<  A timeout error occurred during transaction.*/
    HAL_I2C_SLAVE_EVENT_SUCCESS = 0                         /**<  The transaction completed wihtout any error. */
} hal_i2c_slave_event_t;

/** @brief This enum defines the I2C slave status. */
typedef enum {
    HAL_I2C_SLAVE_RUNNING_STATUS_IDLE = 0,                  /**<  The I2C bus is idle. */
    HAL_I2C_SLAVE_RUNNING_STATUS_BUSY = 1,                  /**<  The I2C bus is busy. */
    HAL_I2C_SLAVE_RUNNING_STATUS_READY = 2,                 /**<  The I2C bus is ready to use. */
    HAL_I2C_SLAVE_RUNNING_STATUS_TX_USING = 3,              /**<  The I2C bus is in sending mode. */
    HAL_I2C_SLAVE_RUNNING_STATUS_RX_USING = 4,              /**<  The I2C bus is in receiving mode. */
} hal_i2c_slave_running_status_t;

/** @brief This enum defines the I2C slave IO mode. */
typedef enum {
    HAL_I2C_SLAVE_IO_PUSH_PULL  = 0,                        /**<  push-pull. */
    HAL_I2C_SLAVE_IO_OPEN_DRAIN = 3,                        /**<  open-drain. */
    HAL_I2C_SLAVE_IO_MAX                                    /**<  The total number of supported I2C IO config.*/
} hal_i2c_slave_io_config_t;

/**
  * @}
  */

/** @defgroup hal_i2c_slave_typedef Typedef
  * @{
  */

/** @brief  This defines the callback function prototype.
 *          User should register a callback function while using DMA mode.
 *          This function is called after the transaction finishes in the I2C Slave ISR routine.
 *          For more details about callback architecture, please refer to @ref HAL_I2C_SLAVE_Architecture_Chapter.
 *  @param [in] slave_address is a user defined slave address to send or receive data.
 *  @param [in] event is the transaction event of current transaction. It also provides the transaction result.
 *              For more details about the event type, please refer to #hal_i2c_slave_event_t.
 *  @param [in] success_size is the real transaction sizes(bytes).
 *  @param [in] user_data is the user defined parameter obtained from #hal_i2c_slave_send_dma() and #hal_i2c_slave_receive_dma() function.
 *  @sa  #hal_i2c_slave_send_dma() and #hal_i2c_slave_receive_dma()
 */
typedef void (*hal_i2c_slave_callback_t)(uint8_t slave_address, hal_i2c_slave_event_t event, int32_t success_size, void *user_data);

/**
  * @}
  */

/** @defgroup hal_i2c_slave_struct Struct
  * @{
  */

/** @brief This structure defines the configuration parameters for i2c slave send transaction. */
typedef struct {
    uint8_t slave_address;                                  /**<  The slave device address. */
    uint32_t send_length;                                   /**<  The send data length. */
    uint8_t *send_buffer;                                   /**<  The send data buffer. */
} hal_i2c_slave_send_config_t;

/** @brief This structure defines the configuration parameters for i2c slave receive transaction. */
typedef struct {
    uint8_t slave_address;                                  /**<  The slave device address. */
    uint32_t receive_length;                                /**<  The receive data length. */
    uint8_t *receive_buffer;                                /**<  The receive data buffer. */
} hal_i2c_slave_receive_config_t;

/** @brief This structure defines the configuration parameters for i2c slave callback function. */
typedef struct {
    hal_i2c_slave_callback_t user_callback;                 /**<  The user callback. */
    void *user_data;                                        /**<  The user data. */
} hal_i2c_slave_callback_config_t;

/**
  * @}
  */


/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/**
 * @brief This function initializes the I2C slave before starting a transaction.
 * @param[in] slave_port is the I2C slave port number. The value is defined in #hal_i2c_slave_port_t.
 * @return #HAL_I2C_SLAVE_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *         #HAL_I2C_SLAVE_STATUS_INVALID_PARAMETER, an invalid transfer_frequency is given; \n
 *         #HAL_I2C_SLAVE_STATUS_OK, the operation completed successfully.
 *         #HAL_I2C_SLAVE_STATUS_ERROR_BUSY, the I2C slave is in use.
 * @note   #hal_i2c_slave_deinit() must be called when the I2C is no longer in use, release the I2C resource for the other users to use this I2C slave.
 *         In a multi-task applications, if #hal_i2c_slave_init() returns error #HAL_I2C_STATUS_ERROR_BUSY, it is suggested to call functions that can yield CPU and try
 *         again later.
 * @sa #hal_i2c_slave_deinit()
 */
hal_i2c_slave_status_t hal_i2c_slave_init(hal_i2c_slave_port_t slave_port);

/**
 * @brief This function releases the I2C slave after the transaction is over. Call this function, if the I2C slave is no longer in use.
 * @param[in] slave_port is the I2C slave port number. The value is defined in #hal_i2c_slave_port_t.
 * @return  #HAL_I2C_SLAVE_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *          #HAL_I2C_SLAVE_STATUS_OK, the operation completed successfully. \n
 *          #HAL_I2C_SLAVE_STATUS_ERROR_BUSY, the I2C slave is in use.
 * @note   This function must be called when the I2C slave is no longer in use, release the I2C slave resource for the other users to use this I2C slave.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Slave_Driver_Usage_Chapter
 * @sa #hal_i2c_slave_init()
 */
hal_i2c_slave_status_t hal_i2c_slave_deinit(hal_i2c_slave_port_t slave_port);

/**
 * @brief This function sends data to I2C master in polling mode.
 *  This function will not return until the transaction is complete. For more details about polling mode, see polling mode in
 *  @ref HAL_I2C_SLAVE_Features_Chapter chapter.
 * @param[in] slave_port is the I2C slave port number. The value is defined in #hal_i2c_slave_port_t.
 * @param[in] send_config is the configuration parameter of this API for send.
 * @param[in] timeout_ms is the time(ms) to wait for the master transaction. If the transaction is not complete in the time out value, a timeout error will occurs.
 * @param[in] success_size is the real transaction sizes(bytes).
 * @return   #HAL_I2C_SLAVE_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_SLAVE_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_SLAVE_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_SLAVE_STATUS_ERROR, a hardware error occurred during the transaction.\n
 *           #HAL_I2C_SLAVE_STATUS_ERROR_BUSY, the I2C slave bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Slave_Driver_Usage_Chapter
 * @sa #hal_i2c_slave_send_dma()
 */
hal_i2c_slave_status_t hal_i2c_slave_send_polling(hal_i2c_slave_port_t slave_port, hal_i2c_slave_send_config_t *send_config, uint32_t timeout_ms, int32_t *success_size);

/**
 * @brief This function receives data to I2C master in polling mode.
 *  This function will not return until the transaction is complete. For more details about polling mode, see polling mode in
 *  @ref HAL_I2C_SLAVE_Features_Chapter chapter.
 * @param[in] slave_port is the I2C slave port number. The value is defined in #hal_i2c_slave_port_t.
 * @param[in] receive_config is the configuration parameter of this API for receive.
 * @param[in] timeout_ms is the time(ms) to wait for the master transaction. If the transaction is not complete in the time out value, a timeout error will occurs.
 * @param[in] success_size is the real transaction sizes(bytes).
 * @return   #HAL_I2C_SLAVE_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_SLAVE_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_SLAVE_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_SLAVE_STATUS_ERROR, a hardware error occurred during the transaction.\n
 *           #HAL_I2C_SLAVE_STATUS_ERROR_BUSY, the I2C slave bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Slave_Driver_Usage_Chapter
 * @sa #hal_i2c_slave_receive_dma()
 */
hal_i2c_slave_status_t hal_i2c_slave_receive_polling(hal_i2c_slave_port_t slave_port, hal_i2c_slave_receive_config_t *receive_config, uint32_t timeout_ms, int32_t *success_size);

/**
 * @brief This function sends data to I2C master in dma mode.
 *  This function will not return until the transaction is complete. For more details about dma mode, see dma mode in
 *  @ref HAL_I2C_SLAVE_Features_Chapter chapter.
 * @param[in] slave_port is the I2C slave port number. The value is defined in #hal_i2c_slave_port_t.
 * @param[in] send_config is the configuration parameter of this API for send.
 * @param[in] timeout_ms is the time(ms) to wait for the master transaction. If the transaction is not complete in the time out value, a timeout error will occurs.
 * @param[in] callback_config is the configuration parameter of the callback function.
 * @return   #HAL_I2C_SLAVE_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_SLAVE_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_SLAVE_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_SLAVE_STATUS_ERROR, a hardware error occurred during the transaction.\n
 *           #HAL_I2C_SLAVE_STATUS_ERROR_BUSY, the I2C slave bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Slave_Driver_Usage_Chapter
 * @sa #hal_i2c_slave_send_polling()
 */
hal_i2c_slave_status_t hal_i2c_slave_send_dma(hal_i2c_slave_port_t slave_port, hal_i2c_slave_send_config_t *send_config, uint32_t timeout_ms, hal_i2c_slave_callback_config_t *callback_config);

/**
 * @brief This function receives data to I2C master in dma mode.
 *  This function will not return until the transaction is complete. For more details about dma mode, see dma mode in
 *  @ref HAL_I2C_SLAVE_Features_Chapter chapter.
 * @param[in] slave_port is the I2C slave port number. The value is defined in #hal_i2c_slave_port_t.
 * @param[in] receive_config is the configuration parameter of this API for receive.
 * @param[in] timeout_ms is the time(ms) to wait for the master transaction. If the transaction is not complete in the time out value, a timeout error will occurs.
 * @param[in] callback_config is the configuration parameter of the callback function.
 * @return   #HAL_I2C_SLAVE_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_SLAVE_STATUS_INVALID_PARAMETER, a NULL buffer pointer is given by user; \n
 *           #HAL_I2C_SLAVE_STATUS_OK, the operation completed successfully; \n
 *           #HAL_I2C_SLAVE_STATUS_ERROR, a hardware error occurred during the transaction.\n
 *           #HAL_I2C_SLAVE_STATUS_ERROR_BUSY, the I2C slave bus is in use.
 * @par       Example
 *    Sample code, please refer to @ref HAL_I2C_Slave_Driver_Usage_Chapter
 * @sa #hal_i2c_slave_receive_polling()
 */
hal_i2c_slave_status_t hal_i2c_slave_receive_dma(hal_i2c_slave_port_t slave_port, hal_i2c_slave_receive_config_t *receive_config, uint32_t timeout_ms, hal_i2c_slave_callback_config_t *callback_config);

/**
 * @brief This function gets running status of the I2C slave.
 *  Call this function to check if the I2C slave is in ready or not before transferring data. If it's not in ready, then the resource is currently in use,
 *  delay the operation until the I2C slave is in ready.
 * @param[in] slave_port is the I2C slave port number. The value is defined in #hal_i2c_slave_port_t.
 * @param[out] running_status
 *             #HAL_I2C_SLAVE_RUNNING_STATUS_BUSY, the I2C slave is in busy status; \n
 *             #HAL_I2C_SLAVE_RUNNING_STATUS_READY, the I2C slave is ready to use; \n
 *             #HAL_I2C_SLAVE_RUNNING_STATUS_TX_USING, the I2C slave is in sending mode; \n
 *             #HAL_I2C_SLAVE_RUNNING_STATUS_RX_USING, the I2C slave is in receiving mode; \n
 *             #HAL_I2C_SLAVE_RUNNING_STATUS_IDLE, the I2C slave is in idle status; User needs to call #hal_i2c_slave_init() now.
 * @return   #HAL_I2C_SLAVE_STATUS_INVALID_PORT_NUMBER, an invalid port number is given; \n
 *           #HAL_I2C_SLAVE_STATUS_OK, the operation completed successfully.
 */
hal_i2c_slave_status_t hal_i2c_slave_get_running_status(hal_i2c_slave_port_t slave_port, hal_i2c_slave_running_status_t *running_status);


#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif /* HAL_I2C_SLAVE_MODULE_ENABLED */

#endif /* __HAL_I2C_SLAVE_H__ */
