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
#ifndef _BSP_FLASH_H_
#define _BSP_FLASH_H_

#ifdef BSP_SERIAL_FLASH_ENABLED

/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 * This section describes the programming interfaces of the BSP FLASH driver, Which provides the driver to operate SIP or external FLASH.
 *
 * @section BSP_FLASH_Terms_Chapter Terms and acronyms
 *
 * |Terms            |Details                                                                 |
 * |-----------------|------------------------------------------------------------------------|
 * |\b XIP           | eXecute In Place: A method of executing programs directly from long term storage rather than copying it into the RAM |
 * |\b SFC           | The serial flash controller (SFC) supports convenient high-speed serial NOR flash access and flexible macro command sequence read/write. |
 * |\b SIP           | System in package. It is a number of integrated circuits enclosed in a single chip carrier package. For more information, please check <a href="https://en.wikipedia.org/wiki/System_in_package"> System in package in Wikipedia</a>.|
 * |\b SPI           | Serial Peripheral Interface. The Serial Peripheral Interface bus is a synchronous serial communication interface specification used for short distance communication. For more information, please check <a href="https://en.wikipedia.org/wiki/Serial_peripheral_interface"> Serial Peripheral Interface Bus in Wikipedia</a>.|
 * |\b ESC           | The External SRAM Controller provides the platform capability to extend its memory space. |
 *
 * @section BSP_FLASH_Features_Chapter Supported features
 * The general purpose of FLASH is to save data. The features supported by this module are listed below:
 * - \b FLASH \b erase \n
 * It must be erased before data can be saved into the FLASH storage; Function #bsp_flash_erase() implements this operation.
 * - \b FLASH \b write \n
 * Function #bsp_flash_write() is called to save data into the FLASH storage after it is erased.
 * - \b FLASH \b read \n
 * Function #bsp_flash_read() is called to read data from the FLASH storage.
 * - \b High-level \b abstraction \n
 * Users can use BSP APIs to operate all types of Flash supported by the SDK. BSP flash can operate different types of Flash through different addresses specified by users.
 * @}
 * @}
 */


#ifdef AB156X
/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 *
 * @section BSP_FLASH_Information_Chapter Address and capacity information of different types of Flash
 * <table>
 * <caption id="multi_row">Address and capacity information table</caption>
 * <tr><th> \b Flash \b Type                      <th> \b Begin \b Address        <th> \b Capacity    <th> \b Maximum \b supported \b capacity
 * <tr>
 *     <td> \b SFC
 *     <td> #HAL_FLASH_BASE_ADDRESS
 *     <td>
 *         <table>
 *         <tr><td> \b ChipName <td> \b SFC \b Capacity
 *         <tr><td> AB1565 <td> 32 Mb( 4 MB )
 *         <tr><td> AB1565A <td> 32 Mb( 4 MB )
 *         <tr><td> AB1565M <td> 64 Mb( 8 MB )
 *         <tr><td> AB1565AM <td> 64Mb( 8 MB )
 *         <tr><td> AB1568 <td> 64 Mb( 8 MB )
 *         <tr><td> AB1568M <td> 128 Mb( 16 MB )
 *         </table>
 *     <td> 1024 Mb( 128 MB )
 * <tr><td> \b SPI <td> #BSP_EXTERNAL_FLASH_BASE_ADDRESS <td>SPI Flash is an external device, please refer to \n the specific external device for capacity information.<td>10240 Mb( 1280 MB )
 * <tr><td> \b ESC <td> #HAL_ESC_BASE_ADDRESS <td>ESC Flash is an external device, please refer to \n the specific external device for capacity information.<td>128 Mb( 16 MB )
 * </table>
 * @}
 * @}
 */
#endif


#ifdef AG3335
/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 *
 * @section BSP_FLASH_Information_Chapter Address and capacity information of different types of Flash
 * <table>
 * <caption id="multi_row">Address and capacity information table</caption>
 * <tr><th> \b Flash \b Type                      <th> \b Begin \b Address        <th> \b Capacity    <th> \b Maximum \b supported \b capacity
 * <tr><td> \b SFC <td> #HAL_FLASH_BASE_ADDRESS <td> 32 Mb( 4 MB ) <td> 1024 Mb( 128 MB )
 * <tr><td> \b SPI <td> #BSP_EXTERNAL_FLASH_BASE_ADDRESS <td>SPI Flash is an external device, please refer to \n the specific external device for capacity information.<td>10240 Mb( 1280 MB )
 * <tr><td> \b ESC <td> #HAL_ESC_BASE_ADDRESS <td>ESC Flash is an external device, please refer to \n the specific external device for capacity information.<td>128 Mb( 16 MB )
 * </table>
 * @}
 * @}
 */
#endif

/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 *
 * @section BSP_FLASH_High_Level_Driver_Usage_Chapter How to use the memory of FLASH
 * @}
 * @}
 */

#ifdef HAL_FLASH_MODULE_ENABLED
/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 * - \b SFC \b FLASH \b read \b and \b Write
 *   - Step 1: Call #bsp_flash_init() to initialize the SFC FLASH.
 *   - Step 2: Call #bsp_flash_erase() to erase the target block of the FLASH storage before write.
 *   - Step 3: Call #bsp_flash_write() to write data to the FLASH storage.
 *   - Step 4: Call #bsp_flash_read() to read data from the FLASH storage.
 *   - Step 5: Call #bsp_flash_deinit() to de-initialize the driver.
 *   - Sample code:
 *  @code
 *  void bsp_sfc_flash_example(void)
 *  {
 *     #define MAX_DATA (4096)
 *     uint8_t data_to_write[MAX_DATA] = {0};
 *     uint8_t data_read[MAX_DATA] = {0};
 *
 *     // For SFC Flash, the address is an offset from zero, the maximum is the flash size.
 *     // It is not necessary to do init and de-init for SFC Flash;
 *     // Erase 4k form start adress.
 *     uint32_t start_address = 0x10000 | HAL_FLASH_BASE_ADDRESS;
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_erase(start_address, BSP_FLASH_BLOCK_4K)) {
 *         //Please do error handling.
 *     }
 *     //Write data to start address
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_write(start_address, data_to_write, sizeof(data_to_write))) {
 *         //Please do error handling.
 *     }
 *     //Read data from start address.
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_read(start_address, data_read, sizeof(data_read))) {
 *         //Please do error handling.
 *     }
 *   }
 *   @endcode
 * @}
 * @}
 */
#endif /* HAL_FLASH_MODULE_ENABLED */


#ifdef HAL_SPI_MASTER_MODULE_ENABLED
/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 * - \b SPI \b FLASH \b read \b and \b Write
 *   - Step 1: Call #bsp_flash_init() to initialize the SIP FLASH or SPI FLASH module.
 *   - Step 2: Call #bsp_flash_erase() to erase the target block of the FLASH storage before write.
 *   - Step 3: Call #bsp_flash_write() to write data to the FLASH storage.
 *   - Step 4: Call #bsp_flash_read() to read data from the FLASH storage.
 *   - Step 5: Call #bsp_flash_deinit() to de-initialize the driver.
 *   - Sample code:
 *  @code
 *  void bsp_spi_flash_example(void)
 *  {
 *     #define MAX_DATA (4096)
 *     uint8_t data_to_write[MAX_DATA] = {0};
 *     uint8_t data_read[MAX_DATA] = {0};
 *     // For SPI external Flash, it assumes that the address starts at #SPI_SERIAL_FLASH_ADDRESS,
       // so the Start_address should be within BSP_EXTERNAL_FLASH_BASE_ADDRESS to BSP_EXTERNAL_FLASH_BASE_ADDRESS+flash size.
 *     uint32_t start_address = 0x10000 | BSP_EXTERNAL_FLASH_BASE_ADDRESS;
 *     // Initialize the SPI master and SPI external flash
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_init()) {
 *         //Please do error handling.
 *     }
 *     //Erase 4k form start adress
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_erase(start_address, BSP_FLASH_BLOCK_4K)) {
 *         //Please do error handling.
 *     }
 *     //Write data to start address
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_write(start_address, data_to_write, sizeof(data_to_write))) {
 *         //Please do error handling.
 *     }
 *     //Read data from start address.
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_read(start_address, data_read, sizeof(data_read))) {
 *         //Please do error handling.
 *     }
 *     // De-initialize the SPI master and external flash if these are not used any more.
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_deinit()) {
 *         //Please do error handling.
 *     }
 *   }
 *   @endcode
 *   @note Because the SPI API used does not support re-entry, it is necessary to limit the use of SPI Flash to Task level in bsp Flash.
 *   At the same time, under the SPI Flash solution, mutex is also needed to ensure that only one task is operating SPI Flash.
 * @}
 * @}
 */
#endif /* HAL_SPI_MASTER_MODULE_ENABLED */


#ifdef HAL_ESC_SUPPORT_FLASH
/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 * - \b ESC \b FLASH \b read \b and \b Write
 *   - Step 1: Call #bsp_flash_init() to initialize the ESC FLASH.
 *   - Step 2: Call #bsp_flash_erase() to erase the target block of the FLASH storage before write.
 *   - Step 3: Call #bsp_flash_write() to write data to the FLASH storage.
 *   - Step 4: Call #bsp_flash_read() to read data from the FLASH storage.
 *   - Step 5: Call #bsp_flash_deinit() to de-initialize the driver.
 *   - Sample code:
 *  @code
 *  #include "hal_esc_internal.h"
 *  void bsp_esc_flash_example(void)
 *  {
 *     #define MAX_DATA (4096)
 *     uint8_t data_to_write[MAX_DATA] = {0};
 *     uint8_t data_read[MAX_DATA] = {0};
 *
 *     // For ESC Flash, the address is an offset from zero, the maximum is the flash size.
 *     // It is not necessary to do init and de-init for ESC Flash;
 *     // Erase 4k form start adress.
 *     uint32_t start_address = 0x10000 | HAL_ESC_BASE_ADDRESS;
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_erase(start_address, BSP_FLASH_BLOCK_4K)) {
 *         //Please do error handling.
 *     }
 *     //Write data to start address
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_write(start_address, data_to_write, sizeof(data_to_write))) {
 *         //Please do error handling.
 *     }
 *     //Read data from start address.
 *     if (BSP_FLASH_STATUS_OK != bsp_flash_read(start_address, data_read, sizeof(data_read))) {
 *         //Please do error handling.
 *     }
 *   }
 *   @endcode
 * @}
 * @}
 */
#endif /* HAL_ESC_SUPPORT_FLASH */

#include "hal_spi_master.h"
#include "bsp_external_flash_config.h"

/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 */

/** @defgroup bsp_flash_define Define
 * @{
  */

/** @brief  This macro defines the BSP external Flash base address.
  */
#define BSP_EXTERNAL_FLASH_BASE_ADDRESS    (0xB0000000)
/**
  * @}
  */
/**
 * @}
 * @}
 */

/**
 * @addtogroup BSP
 * @{
 * @addtogroup SERIAL_FLASH
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup bsp_flash_enum Enums
  * @{
  */

/** @brief  This enum defines the return status of the BSP flash API.  */
typedef enum {
    BSP_FLASH_STATUS_BUSY = -10,              /**< The device is busy. */
    BSP_FLASH_ERROR_NOT_FOUND_DEVICE = -9,    /**< Did not find flash device. */
    BSP_FLASH_WRONG_PARAMETER = -8,           /**< Invalid parameter error. */
    BSP_FLASH_ERROR_ERASE_FAIL = -7,          /**< flash erase fail */
    BSP_FLASH_ERROR_PROG_FAIL = -6,           /**< flash program fail */
    BSP_FLASH_STATUS_NOT_INIT = -5,           /**< Driver is not initiazed. */
    BSP_FLASH_ERROR_NO_SPACE = -4,            /**< flash has no space to write */
    BSP_FLASH_ERROR_WRONG_ADDRESS = -3,       /**< flash invalid access address */
    BSP_FLASH_STATUS_ERROR_LOCKED = -2,       /**< flash had locked */
    BSP_FLASH_STATUS_ERROR = -1,              /**< BSP flash unspecified error. */
    BSP_FLASH_STATUS_OK,                      /**< No error occurred. */
} bsp_flash_status_t;

/** @brief flash block definition */
typedef enum {
    BSP_FLASH_BLOCK_4K,       /**< erase flash with block size 4KB */
    BSP_FLASH_BLOCK_32K,      /**< erase flash with block size 32KB */
    BSP_FLASH_BLOCK_64K,      /**< erase flash with block size 64KB */
    BSP_FLASH_CHIP_ERASE,     /**< erase flash chip */
} bsp_block_size_type_t;
/**
* @}
*/


/*****************************************************************************
* Functions
*****************************************************************************/

/**
 * @brief This function does the initialization for the serial flash and SPI Master.
 * @return    #BSP_FLASH_STATUS_BUSY means the device is busy.\n
 *            #BSP_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #BSP_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #BSP_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #BSP_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #BSP_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_flash_status_t bsp_flash_init(void);

/**
 * @brief This function reads data from the serial flash.
 * @param[in] address:  read address
 * @param[in] buffer: buffer for read data
 * @param[in] length: read data length
 * @return    #BSP_FLASH_STATUS_BUSY means the device is busy.\n
 *            #BSP_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #BSP_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #BSP_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #BSP_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #BSP_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_flash_status_t bsp_flash_read(uint32_t address, uint8_t *buffer, uint32_t length);

/**
 * @brief This function writes data to the serial flash.
 * @param[in] address:  write address
 * @param[in] data: write data
 * @param[in] length: write data length
 * @return    #BSP_FLASH_STATUS_BUSY means the device is busy.\n
 *            #BSP_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #BSP_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #BSP_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #BSP_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #BSP_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_flash_status_t bsp_flash_write(uint32_t address, uint8_t *data, int32_t length);


/**
 * @brief This function erases the serial flash.
 * @param[in] address: erase start address
 * @param[in] block_size: erase block size
 * @return    #BSP_FLASH_STATUS_BUSY means the device is busy.\n
 *            #BSP_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #BSP_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #BSP_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #BSP_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #BSP_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_flash_status_t bsp_flash_erase(uint32_t address, bsp_block_size_type_t block_size);

/**
 * @brief This function resets the serial flash and SPI Master.
 * @return    #BSP_FLASH_STATUS_BUSY means the device is busy.\n
 *            #BSP_FLASH_ERROR_NOT_FOUND_DEVICE means it did not find the device. \n
 *            #BSP_FLASH_WRONG_PARAMETER means an invalid parameter is given by the user.\n
 *            #BSP_FLASH_STATUS_NOT_INIT means the driver is not initialized.\n
 *            #BSP_FLASH_STATUS_ERROR means an error occurred during the operation. \n
 *            #BSP_FLASH_STATUS_OK means this function returns successfully.\n
 */
bsp_flash_status_t bsp_flash_deinit(void);

/**
* @}
* @}
*/

/*-----------------------------------------------------------*/
#endif //BSP_SERIAL_FLASH_ENABLED

#endif //_BSP_FLASH_H_

