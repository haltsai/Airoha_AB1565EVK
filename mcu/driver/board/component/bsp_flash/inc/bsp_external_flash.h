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

/* Includes ------------------------------------------------------------------*/
#ifndef _BSP_SPI_SERIAL_FLASH_H_
#define _BSP_SPI_SERIAL_FLASH_H_

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED

/**
 * @addtogroup BSP
 * @{
 * @addtogroup External_FLASH
 * @{
 * This section describes the programming interfaces of the external FLASH driver. The external flash driver only
 * provide the driver to operate device, it means the user controls the whole memory.
 *
 * @section BSP_EXTERNAL_FLASH_Terms_Chapter Terms and acronyms
 *
 * |Terms            |Details                                                                 |
 * |-----------------|------------------------------------------------------------------------|
 * |\b SPI           | Serial Peripheral Interface. The Serial Peripheral Interface bus is a synchronous serial communication interface specification used for short distance communication. For more information, please check <a href="https://en.wikipedia.org/wiki/Serial_peripheral_interface"> Serial Peripheral Interface Bus in Wikipedia</a>.|
 *
 * @section BSP_EXTERNAL_FLASH_Features_Chapter Supported features
 * The general purpose of External FLASH is to save data. The features supported by this module are listed below:
 * - \b FLASH \b erase \n
 * It must be erased before data can be saved into the FLASH storage; Function #bsp_external_flash_erase() implements this operation.
 * - \b FLASH \b write \n
 * Function #bsp_external_flash_write() is called to save data into the External FLASH storage after it is erased.
 * - \b FLASH \b read \n
 * Function #bsp_external_flash_read() is called to read data from the external FLASH storage.
 *
 * @section BSP_FLASH_Driver_Usage_Chapter How to use the memory of external FLASH
 * - It defines a virtual address for the SPI external flash as SPI_SERIAL_FLASH_ADDRESS.
 *     It means the flash operation address is from SPI_SERIAL_FLASH_ADDRESS to SPI_SERIAL_FLASH_ADDRESS + FLASH size.
 * - The user must pre-define the memory before using it.
 * * - \b FLASH \b read \b and \b Write
 *   - Step 1: Call #hal_gpio_init() to inite the pins, if EPT tool hasn't been used to configure the related pinmux.
 *   - Step 2: Call #hal_pinmux_set_function() to set GPIO pinmux, if EPT tool hasn't been used to configure the related pinmux.
 *             For more details about #hal_pinmux_set_function(), please refer to @ref GPIO.
 *   - Step 3: Call #bsp_external_flash_init() to initialize the external FLASH module.
 *   - Step 4: Call #bsp_external_flash_erase() to erase the target block of the external FLASH storage before write.
 *   - Step 5: Call #bsp_external_flash_write() to write data to the external FLASH storage.
 *   - Step 6: Call #bsp_external_flash_read() to read data from the external FLASH storage.
 *   - Step 7: Call #bsp_external_flash_deinit() to de-initialize the driver.
 *   - Sample code:
 *  @code
 *  void external_flash_test(void)
 *  {
 *     #define MAX_DATA (4096)
 *     uint8_t data_to_write[MAX_DATA] = {0};
 *     uint8_t data_read[MAX_DATA] = {0};
 *     // It assumes that the external flash address starts at #SPI_SERIAL_FLASH_ADDRESS,
       // so the start_address must be between 0xB0000000 and 0xB0000000+flash size.
 *     uint32_T start_address = 0xB0010000;
 *     // Initialize the SPI master and external flash
 *     if (EXTERNAL_FLASH_STATUS_OK != bsp_flash_init()) {
 *         //Please do error handling.
 *     }
 *     //Erase 4k form start adress
 *     if (EXTERNAL_FLASH_STATUS_OK != bsp_external_flash_erase(start_address, FLASH_BLOCK_4K)) {
 *         //Please do error handling.
 *     }
 *     //Write data to start address
 *     if (EXTERNAL_FLASH_STATUS_OK != bsp_external_flash_write(start_address, data_to_write, sizeof(data_to_write))) {
 *         //Please do error handling.
 *     }
 *     //Read data from start address.
 *     if (EXTERNAL_FLASH_STATUS_OK != bsp_external_flash_read(start_address, data_read, sizeof(data_read))) {
 *         //Please do error handling.
 *     }
 *     // De-initialize the SPI master and external flash if these are not used any more.
 *     if (EXTERNAL_FLASH_STATUS_OK != bsp_external_flash_deinit()) {
 *         //Please do error handling.
 *     }
 *   }
 *   @endcode
 *
 */

#include "hal_spi_master.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup bsp_external_flash_enum Enums
  * @{
  */

/** @brief  This enum defines the return status of the BSP flash API.  */
typedef enum {
    EXTERNAL_FLASH_STATUS_BUSY = -10,              /**< The device is busy. */
    EXTERNAL_FLASH_ERROR_NOT_FOUND_DEVICE = -9,    /**< Did not find flash device. */
    EXTERNAL_FLASH_WRONG_PARAMETER = -8,           /**< Invalid parameter error. */
    EXTERNAL_FLASH_ERROR_ERASE_FAIL = -7,          /**< flash erase fail */
    EXTERNAL_FLASH_ERROR_PROG_FAIL = -6,           /**< flash program fail */
    EXTERNAL_FLASH_STATUS_NOT_INIT = -5,           /**< Driver is not initiazed. */
    EXTERNAL_FLASH_ERROR_NO_SPACE = -4,            /**< flash has no space to write */
    EXTERNAL_FLASH_ERROR_WRONG_ADDRESS = -3,       /**< flash invalid access address */
    EXTERNAL_FLASH_STATUS_ERROR_LOCKED = -2,       /**< flash had locked */
    EXTERNAL_FLASH_STATUS_ERROR = -1,              /**< flash unspecified error. */
    EXTERNAL_FLASH_STATUS_OK,                      /**< No error occurred. */
} bsp_external_flash_status_t;

/** @brief flash block definition */
typedef enum {
    FLASH_BLOCK_4K,       /**< erase flash with block size 4KB */
    FLASH_BLOCK_32K,      /**< erase flash with block size 32KB */
    FLASH_BLOCK_64K,      /**< erase flash with block size 64KB */
    FLASH_CHIP_ERASE,     /**< erase flash chip */
} block_size_type_t;
/**
* @}
*/


/*****************************************************************************
* Functions
*****************************************************************************/

/**
 * @brief This function does the initialization for the serial flash and SPI Master.
 * @param[in] master_port:  used SPI master port.
 * @param[in] frequency: SPI Master frequency, Maximum is 52Mhz.
 * @return    #EXTERNAL_FLASH_STATUS_BUSY means the device is busy.\n
 *            #EXTERNAL_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #EXTERNAL_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #EXTERNAL_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #EXTERNAL_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #EXTERNAL_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_external_flash_status_t bsp_external_flash_init(hal_spi_master_port_t master_port, uint32_t frequency);

/**
 * @brief This function reads data from the serial flash.
 * @param[in] address:  read address
 * @param[in] buffer: buffer for read data
 * @param[in] length: read data length
 * @return    #EXTERNAL_FLASH_STATUS_BUSY means the device is busy.\n
 *            #EXTERNAL_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #EXTERNAL_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #EXTERNAL_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #EXTERNAL_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #EXTERNAL_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_external_flash_status_t bsp_external_flash_read(uint32_t address, uint8_t *buffer, uint32_t length);

/**
 * @brief This function writes data to the serial flash.
 * @param[in] address:  write address
 * @param[in] data: write data
 * @param[in] length: write data length
 * @return    #EXTERNAL_FLASH_STATUS_BUSY means the device is busy.\n
 *            #EXTERNAL_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #EXTERNAL_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #EXTERNAL_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #EXTERNAL_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #EXTERNAL_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_external_flash_status_t bsp_external_flash_write(uint32_t address, uint8_t *data, int32_t length);


/**
 * @brief This function erases the serial flash.
 * @param[in] address: erase start address
 * @param[in] block_size: erase block size
 * @return    #EXTERNAL_FLASH_STATUS_BUSY means the device is busy.\n
 *            #EXTERNAL_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #EXTERNAL_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #EXTERNAL_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #EXTERNAL_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #EXTERNAL_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_external_flash_status_t bsp_external_flash_erase(uint32_t address, block_size_type_t block_size);

/**
 * @brief This function resets the serial flash and SPI Master.
 * @return    #EXTERNAL_FLASH_STATUS_BUSY means the device is busy.\n
 *            #EXTERNAL_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #EXTERNAL_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #EXTERNAL_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #EXTERNAL_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #EXTERNAL_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_external_flash_status_t bsp_external_flash_deinit(void);

/**
* @}
* @}
*/

/*-----------------------------------------------------------*/
#endif //_BSP_SPI_SERIAL_FLASH_H_
#endif //BSP_EXTERNAL_SERIAL_FLASH_ENABLED

