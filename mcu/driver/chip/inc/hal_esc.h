/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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

#ifndef __HAL_ESC_H__
#define __HAL_ESC_H__

#include "hal_platform.h"

#ifdef HAL_ESC_MODULE_ENABLED

/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 * This section introduces the External SRAM Controller(ESC) driver APIs including terms, supported features, details on how to use this driver, ESC function groups, enums, structures and functions.
 *
 * @section HAL_ESC_Terms_Chapter Terms and acronyms
 *
 * The following provides descriptions to the terms commonly used in the ESC driver and how to use its various functions.
 *
 * |Terms                   |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b ESC                        | The External SRAM Controller provides the platform capability to extend its memory space. |
 *
 * @section HAL_ESC_Features_Chapter Supported features
 * The general purpose of ESC is to provide the platform capability to extend its memory space.
 *
 * The features supported by this module are listed below:
 * @}
 * @}
 */

#ifdef HAL_ESC_SUPPORT_FLASH
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 * - \b Support \b ESC \b with \b Flash \b erase \n
 * It has to be erased before data can be saved into the ESC FLASH storage, function  #hal_esc_erase_flash_block() implements that operation.
 * - \b Support \b ESC \b with \b Flash \b write \n
 * Function #hal_esc_write_flash_data() is called to save data into the ESC FLASH storage after it's erased.
 * - \b Support \b ESC \b with \b Flash \b read \n
 * Function #hal_esc_read_flash_data() is called to read data from the ESC FLASH storage.
 * @}
 * @}
 */
#endif /* HAL_ESC_SUPPORT_FLASH */

#ifdef HAL_ESC_SUPPORT_PSRAM
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 * - \b Support \b ESC \b with \b PSRAM \b read \n
 * Data can be directly read from the specified address using pointers.
 * - \b Support \b ESC \b with \b PSRAM \b write \n
 * Data can be directly written to the specified address using pointers.
 * @}
 * @}
 */
#endif /* HAL_ESC_SUPPORT_PSRAM */

/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 * @section HAL_ESC_Driver_Usage_Chapter How to use this driver
 * @}
 * @}
 */

#ifdef HAL_ESC_SUPPORT_FLASH
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 * - \b Configurate \b the \b driver \b when \b using \b ESC \b with \b flash.\n
 *   For using ESC with Flash, ESC_FLASH_ENABLE macro should be defined at the header file named esc_device_config.
 * @}
 * @}
 */
#endif /* HAL_ESC_SUPPORT_FLASH */

#ifdef HAL_ESC_SUPPORT_PSRAM
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 * - \b Configurate \b the \b driver \b when \b using \b ESC \b with \b psram.\n
 *   For using ESC with PSRAM, ESC_PSRAM_ENABLE macro should be defined at the header file named esc_device_config.
 * @}
 * @}
 */
#endif /* HAL_ESC_SUPPORT_PSRAM */


#ifdef HAL_ESC_SUPPORT_FLASH
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 *
 * - \b ESC \b with \b Flash \b read \b and \b Write
 *   - step1: call #hal_esc_init() to initialize the ESC FLASH module.
 *   - step2: call #hal_esc_erase_flash_block() to erase the target block of the ESC FLASH storage.
 *   - step3: call #hal_esc_write_flash_data() to write data to the ESC FLASH storage.
 *   - step4: call #hal_esc_read_flash_data() to read data from the ESC FLASH storage.
 *   - Sample code:
 *   @code
 *  if (HAL_ESC_STATUS_OK != hal_esc_init()) {
 *      //error handling
 *  }
 *  uint32_t start_address = ESC_GENERIC_SRAM_BANK_MASK + 0xA000;  // start_address should be within flash size
 *  if (HAL_ESC_FLASH_STATUS_OK != hal_esc_erase_flash_block(start_address, HAL_ESC_FLASH_ERASE_4K)) {
 *      //error handling
 *  }
 *  #define MAX_DATA (16)
 *  uint8_t data_to_write[MAX_DATA] = {0};
 *  if (HAL_ESC_FLASH_STATUS_OK != hal_esc_write_flash_data(start_address, data_to_write, sizeof(data_to_write))) {
 *      //error handling
 *  }
 *  uint8_t data_read[MAX_DATA] = {0};
 *  if (HAL_ESC_FLASH_STATUS_OK != hal_esc_read_flash_data(start_address, data_read, sizeof(data_read))) {
 *      //error handling
 *  }
 *  //data handling
 *   @endcode
 * @}
 * @}
 */
#endif /* HAL_ESC_SUPPORT_FLASH */

#ifdef HAL_ESC_SUPPORT_PSRAM
/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 *
 * - \b ESC \b with \b PSRAM \b read \b and \b write
 *   - step1: call #hal_esc_init() to initialize the ESC PSRAM module.
 *   - step2: Directly use pointers to read and write data to the specified address.
 *   @code
 *  if (HAL_ESC_STATUS_OK != hal_esc_init()) {
 *      //error handling
 *  }
 *  uint32_t start_address = ESC_GENERIC_SRAM_BANK_MASK + 0xA000;  // start_address should be within PSRAM size
 *  *(volatile uint32_t*)start_address = 0x12345678; // write data througn pointer
 *
 *  uint32_t read_data = *(volatile uint32_t*)start_address;
 *  uint8_t data[10] = { 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA };
 *  memcpy((void*)start_address, (void*)data, sizeof(data));
 *  if(memcmp((void*)start_address, (void*)data, sizeof(data))){
 *      //error handling
 *  }
 *   @endcode
 * @}
 * @}
 */
#endif /* HAL_ESC_SUPPORT_PSRAM */

/**
 * @addtogroup HAL
 * @{
 * @addtogroup ESC
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************
 * Enums
 *****************************************************************************/

/** @defgroup hal_esc_enum Enum
 *  @{
 */
#if defined (HAL_ESC_SUPPORT_FLASH) || defined (HAL_ESC_SUPPORT_PSRAM)

/** @brief This enum defines return type of the common APIs of ESC with PSRAM and ESC with Flash. */
typedef enum{
    HAL_ESC_STATUS_ERROR = -1,                        /**< An unknown error occurred. */
    HAL_ESC_STATUS_OK = 0                             /**< The operation was successful. */
} hal_esc_status_t;
#endif  /* (HAL_ESC_SUPPORT_FLASH) || (HAL_ESC_SUPPORT_PSRAM) */

#ifdef HAL_ESC_SUPPORT_FLASH

/** @brief ESC flash block definition */
typedef enum{
    HAL_ESC_FLASH_ERASE_4K = 0,                       /**< The flash erase block size is 4k. */
    HAL_ESC_FLASH_ERASE_32K = 1,                      /**< The flash erase block size is 32k. */
    HAL_ESC_FLASH_ERASE_64K = 2,                      /**< The flash erase block size 64k. */
    HAL_ESC_FLASH_ERASE_CHIP = 3,                     /**< The flash erase block size all chip. */
} hal_esc_erase_type_t;

/** @brief flash API return status definition */
typedef enum {
    HAL_ESC_FLASH_NOT_ENABLE = -10,                   /**< ESC support flash but ESC_WITH_FLASH was not defined. */
    HAL_ESC_FLASH_ERROR_NOT_FOUND_DEVICE = -9,        /**< ESC flash did not find flash device. */
    HAL_ESC_FLASH_WRONG_PARAMETER = -8,               /**< ESC flash invalid parameter error. */
    HAL_ESC_FLASH_STATUS_ERROR_ERASE_FAIL = -7,       /**< ESC flash erase fail. */
    HAL_ESC_FLASH_STATUS_ERROR_PROG_FAIL = -6,        /**< ESC flash program fail. */
    HAL_ESC_FLASH_STATUS_ERROR_NO_INIT = -5,          /**< ESC flash driver don't initiate. */
    HAL_ESC_FLASH_STATUS_ERROR_NO_SPACE = -4,         /**< ESC flash has no space to write. */
    HAL_ESC_FLASH_STATUS_ERROR_WRONG_ADDRESS = -3,    /**< ESC flash invalid access address. */
    HAL_ESC_FLASH_STATUS_ERROR_LOCKED = -2,           /**< ESC flash had locked. */
    HAL_ESC_FLASH_STATUS_ERROR = -1,                  /**< ESC flash function error. */
    HAL_ESC_FLASH_STATUS_OK = 0                       /**< ESC flash operation was successful. */
} hal_esc_flash_status_t;

#endif /* HAL_ESC_SUPPORT_FLASH */

/**
  * @}
  */


/*****************************************************************************
 * Structures
 *****************************************************************************/


/*****************************************************************************
 * Functions
 *****************************************************************************/
#if defined (HAL_ESC_SUPPORT_FLASH) || defined (HAL_ESC_SUPPORT_PSRAM)
/**
 * @brief     This function initializes the ESC module to enable the ESC services.
 *            Call #hal_esc_init() only once during the initialization.
 * @return
 *                #HAL_ESC_STATUS_OK, if the operation completed successfully. \n
 *                #HAL_ESC_STATUS_ERROR, if an unknown error occurred. \n
 */
hal_esc_status_t hal_esc_init(void);

/**
 * @brief     ESC driver deinitializes.
 * @return
 *                #HAL_ESC_STATUS_OK on success.
 */
hal_esc_status_t hal_esc_deinit(void);
#endif /* (HAL_ESC_SUPPORT_FLASH) || (HAL_ESC_SUPPORT_PSRAM) */

#ifdef HAL_ESC_SUPPORT_FLASH
/**
 * @brief     This function is used to read data from ESC flash.
 * @param[in]  address is starting address to read the data from
 * @param[out]  buffer is place to hold the incoming data
 * @param[in]  len is the length of the data content
 * @return
 *                #HAL_ESC_FLASH_STATUS_OK on success
 */
hal_esc_flash_status_t hal_esc_read_flash_data(uint32_t address, uint8_t *buffer, uint32_t len);

/**
 * @brief     This function is used to write data to ESC flash.
 * @param[in]  address is starting address to write from. Before the address can be written to for the first time,
 *            the address located sector or block must first be erased.
 * @param[in]  len is data length
 * @param[in]  data is source data to be written
 * @return
 *                #HAL_ESC_FLASH_STATUS_OK on success
 */
hal_esc_flash_status_t hal_esc_write_flash_data(uint32_t address, const uint8_t *data, uint32_t len);

/**
 * @brief     This function is used to erase ESC flash.
 * @param[in]  address is starting address to erase from
 * @param[in]  type is the size of block to be erased
 * @return
 *                #HAL_ESC_FLASH_STATUS_OK on success
 * @note
 *  The address should be align with the erase type.
 */
hal_esc_flash_status_t hal_esc_erase_flash_block(uint32_t address, hal_esc_erase_type_t type);

#endif /* HAL_ESC_SUPPORT_FLASH */

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/

#endif /* HAL_ESC_MODULE_ENABLED */

#endif /* __HAL_ESC_H__ */

