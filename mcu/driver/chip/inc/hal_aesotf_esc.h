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

#ifndef __HAL_AESOTF_ESC_H__
#define __HAL_AESOTF_ESC_H__
#include "hal_platform.h"

#ifdef HAL_AESOTF_ESC_MODULE_ENABLED
 /**
 * @addtogroup HAL
 * @{
 * @addtogroup AESOTF_ESC
 * @{
 * This section introduces the AESOTF_ESC driver APIs including terms and acronyms,
 * supported features, software architecture, details on how to use this driver, AESOTF_ESC function groups, enums, structures and functions.
 *
 * @section HAL_AESOTF_Terms_Chapter Terms and acronyms
 *
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b AES                        | The Advanced Encryption Standard. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard"> introduction to the AES in Wikipedia</a>.|
 * |\b AESOTF                     | AESOTF (On-The-Fly) means zero latency of encryption and decryption
 * |\b AESOTF_ESC                 | AESOTF_ESC (External SRAM Controller) used to communicate with peripheral
 *
 * @section HAL_AESOTF_ESC_Features_Chapter Supported features
 *
 *
 * @section HAL_AESOTF_ESC_Driver_Usage_Chapter How to use this driver
 *
 * - Use the AESOTF_ESC to encrypt/decrypt. \n
 *   To use the AESOTF_ESC driver, the caller should allocate a buffer in physical memory region, e.g. TCM, SYSRAM and PSRAM.
 *  - Step 1. Call #hal_aesotf_esc_enable() to encrypt/decrypt.
 *  - sample code:
 *    @code
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t hardware_id[16] = {
 *           0x4d, 0x54, 0x4b, 0x30, 0x30, 0x30, 0x30, 0x30,
 *           0x32, 0x30, 0x31, 0x34, 0x30, 0x38, 0x31, 0x35
 *       };
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t nonce[HAL_AESOTF_ESC_NONCE_LENGTH] = {
 *           0x61, 0x33, 0x46, 0x68, 0x55, 0x38, 0x31, 0x43,
 *           0x77, 0x68, 0x36, 0x33, 0x50, 0x76, 0x33, 0x46
 *       };
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t decrypted_buffer[16] = {0};
 *
 *       ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN hal_aesotf_esc_buffer_t key = {
 *           .buffer = hardware_id,
 *           .length = sizeof(hardware_id)
 *       };
 *
 *       hal_aesotf_esc_init();
 *
 *       hal_aesotf_esc_set_region(BL_BASE, BL_LENGTH, 0);
 *
 *       hal_aesotf_esc_status_t hal_aesotf_esc_enable(&key, &nonce, HAL_AESOTF_ESC_KEY_SOURCE_SWKEY);
 *
 *       memcpy(decrypted_buffer, BL_BASE, 16);
 *
 *       hal_aesotf_esc_disable();
 *
 *    @endcode
 *
 */

#include "hal_define.h"

#ifdef __cplusplus
extern "C" {
#endif


/** @defgroup hal_aesotf_esc_define Define
  * @{
  */

/** @brief This macro defines the block size of AESOTF_ESC operation. The unit is in bytes.
  */
#define HAL_AESOTF_ESC_BLOCK_SIZES        (16)

/** @brief This macro defines the key length of AESOTF_ESC 128 bits. The unit is in bytes.
  */
#define HAL_AESOTF_ESC_KEY_LENGTH_128    (16)

/** @brief This macro defines the key length of AESOTF_ESC 192 bits. The unit is in bytes.
  */
#define HAL_AESOTF_ESC_KEY_LENGTH_192    (24)

/** @brief This macro defines the key length of AESOTF_ESC 256 bits. The unit is in bytes.
  */
#define HAL_AESOTF_ESC_KEY_LENGTH_256    (32)

/** @brief This macro defines the size of initial vector. The unit is in bytes.
  */
#define HAL_AESOTF_ESC_NONCE_LENGTH     (16)

// This macro defines the key source.
#define HAL_AESOTF_ESC_KEY_SOURCE_EFUSE1   (0)
#define HAL_AESOTF_ESC_KEY_SOURCE_EFUSE2   (1)
#define HAL_AESOTF_ESC_KEY_SOURCE_SWKEY    (2)
#define HAL_AESOTF_ESC_KEY_SOURCE_CKDF1    (3)
#define HAL_AESOTF_ESC_KEY_SOURCE_CKDF2    (4)
#define HAL_AESOTF_ESC_KEY_SOURCE_NUM      (5)

/**
  * @}
  */


/** @defgroup hal_aesotf_esc_enum Enum
  * @{
  */

/** @brief This enum defines the HAL interface return value.
  */
typedef enum {
    HAL_AESOTF_ESC_STATUS_BUSY  = -100,    /**< crypto hw is busy. */
    HAL_AESOTF_ESC_STATUS_ERROR = -1,      /**< An error occurred. */
    HAL_AESOTF_ESC_STATUS_OK = 0           /**< No error occurred. */
} hal_aesotf_esc_status_t;


/**
  * @}
  */

/** @defgroup hal_aes_struct Struct
  * @{
  */

/** @brief This structure defines the buffer used in the operation. */
typedef struct {
    uint8_t *buffer;  /**< Data buffer. */
    uint32_t length;  /**< Data length. */
} hal_aesotf_esc_buffer_t;

/**
  * @}
  */

/**
 * @brief      This function initializes the AESOTF engine.
 * @return
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_ESC_Driver_Usage_Chapter
 *
 */
hal_aesotf_esc_status_t hal_aesotf_esc_init(void);


/**
 * @brief      This function turns on the AESOTF engine.
 *
 * @param[in]  key is used to encrypt/decrypt the data.
 * @param[in]  nonce is the element of AESOTF engine.
 * @param[in]  key_src chooses the key source for AESOTF engine.
 * @return
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_ESC_Driver_Usage_Chapter
 *
 */
hal_aesotf_esc_status_t hal_aesotf_esc_enable(hal_aesotf_esc_buffer_t *key, uint8_t nonce[HAL_AESOTF_ESC_NONCE_LENGTH], uint8_t key_src);


/**
 * @brief  This function turns off the AESOTF engine.
 *
 * @return
 * @par    Example
 * Sample code, please refer to @ref HAL_AES_ESC_Driver_Usage_Chapter
 *
 */
hal_aesotf_esc_status_t hal_aesotf_esc_disable(void);

#ifdef __cplusplus
}
#endif

/**
* @}
* @}
*/
#endif /* HAL_AESOTF_ESC_MODULE_ENABLED */

#endif /* __HAL_AESOTF_ESC_H__ */



