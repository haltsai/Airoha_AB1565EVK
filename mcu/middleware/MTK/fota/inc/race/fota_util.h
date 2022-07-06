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

/**
 * File: fota_util.h
 *
 * Description: This file defines some sub-feature options, macros, types and declares some utility interfaces of fota_util.c.
 *
 * Note: See doc/Airoha_IoT_SDK_Firmware_Update_Developers_Guide.pdf for more detail.
 *
 */

#ifndef __FOTA_UTIL_H__
#define __FOTA_UTIL_H__

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "fota_platform.h"


/* For debug only. Enable this to make DUT to execute FOTA upgrade flow always. */
/* #define FOTA_UPGRADE_TEST */
/* #define __FOTA_FOR_BISTO_TEST__ */
#ifdef GSOUND_LIBRARY_ENABLE
#define __FOTA_FOR_BISTO__ /* Support the Bisto FOTA. */
#endif

#if (PRODUCT_VERSION != 3335 && PRODUCT_VERSION != 2822 && !defined(AB156X)) || !defined(BL_FOTA_ENABLE) || defined(MTK_BOOTLOADER_USE_MBEDTLS)
#define FOTA_USE_MBEDTLS_ENABLE /* Enable FOTA to ues the mbedtls algorithms. */

#ifdef FOTA_USE_MBEDTLS_ENABLE
#if (PRODUCT_VERSION != 3335 && PRODUCT_VERSION != 2822 && !defined(AB156X)) || !defined(BL_FOTA_ENABLE)
#define FOTA_USE_MBEDTLS_SHA256_ENABLE /* Enable FOTA to ues the mbedtls SHA256 algorithm. */
#endif
#define FOTA_USE_MBEDTLS_RSA_ENABLE /* Enable FOTA to ues the mbedtls RSA algorithm. */
#endif
#endif

#ifdef MTK_FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION_ENABLE
#define FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION /* Support to update the new image with the memory layout changed. */
#endif

#ifdef MTK_FOTA_EXTERNAL_FLASH_SUPPORT
#define FOTA_EXTERNAL_FLASH_SUPPORT /* Support to update the bin files stored in the external flash besides the internal flash. */
#endif

#ifdef MTK_FOTA_STORE_IN_EXTERNAL_FLASH
#define FOTA_STORE_IN_EXTERNAL_FLASH (1) /* FOTA package is stored in the external flash. */
#else
#define FOTA_STORE_IN_EXTERNAL_FLASH (0) /* FOTA package is stored in the internal flash. */
#endif

#if FOTA_STORE_IN_EXTERNAL_FLASH
#define FOTA_LONG_DATA_READ_MAX_SIZE      (1024) /* The maximum size of the data read at a time. */
#define FOTA_LONG_DATA_PROCESS_MAX_SIZE   (0x40000) /* The maximum size of the data processed at a time. */
#else
#define FOTA_LONG_DATA_READ_MAX_SIZE      (128)
#define FOTA_LONG_DATA_PROCESS_MAX_SIZE   (0x400000)
#endif

/* The TLV information types used in the FOTA header. */
#define FOTA_HEADER_TLV_BASIC_INFO (0x0011) /* The basic information type. */
#define FOTA_HEADER_TLV_MOVER_INFO (0x0012) /* The mover information type. */
#define FOTA_HEADER_TLV_VERSION_INFO (0x0013) /* The version information type. */
#define FOTA_HEADER_TLV_INTEGRITY_VERIFY_INFO (0x0014) /* The integrity verification information type. */
#define FOTA_HEADER_TLV_DEVICE_TYPE_INFO (0x0021) /* The device type verification information type. */


#define FOTA_RACE_CMD_CRC_SUPPORT /* Support the crc32 check. */

#define SIGNATURE_SIZE  (256) /* The signature size.  */
#define INVALID_TLV_VALUE (0xFFFF) /* The invalid value for the TLV information types. */


#define FOTA_HEADER_OCCUPIED_SIZE (0x1000) /* The size of the FOTA header. */


#define FOTA_VERSION_MAX_SIZE (28) /* The maximum size of the FOTA package release version string including the null-terminator. */
#define FOTA_VERSION_INTEGRITY_CHECK_VALUE_SIZE (4) /* The size of the integrity check value of the version string. */
#define FOTA_DEFAULT_VERSION    ("v1.0.0") /* The default FOTA package release version. */


typedef enum {
    FOTA_ERRCODE_SUCCESS = 0,
    FOTA_ERRCODE_READ_FOTA_HEADER_FAIL = 1,
    FOTA_ERRCODE_READ_FOTA_DATA_FAIL = 2,
    FOTA_ERRCODE_CHECK_INTEGRITY_FAIL = 3,
    FOTA_ERRCODE_UNKNOWN_STORAGE_TYPE = 4,
    FOTA_ERRCODE_UNKNOWN_INTEGRITY_CHECK_TYPE = 5,
    FOTA_ERRCODE_SHA256_IS_NOT_SUPPORTED = 6,
    FOTA_ERRCODE_COMMIT_FAIL_DUE_TO_INTEGRITY_NOT_CHECKED = 7,
    FOTA_ERRCODE_UNKNOWN_PARTITION_ID = 8,
    FOTA_ERRCODE_UNSUPPORTED_PARTITION_ID = 9,
    FOTA_ERRCODE_FOTA_RESULT_READ_FAIL = 0x10,
    FOTA_ERRCODE_FOTA_RESULT_INVALID_STATUS = 0x11,
    FOTA_ERRCODE_FOTA_RESULT_NOT_FOUND = 0x12,
    FOTA_ERRCODE_FOTA_RESULT_INCORRECT_LENGTH = 0x13,
    FOTA_ERRCODE_INVALID_PARAMETER = 0x14,
    FOTA_ERRCODE_NOT_ALLOWED = 0x15,
    FOTA_ERRCODE_NOT_ENOUGH_MEMORY = 0x16,

    FOTA_ERRCODE_WOULDBLOCK = 0xfb,
    FOTA_ERRCODE_OUT_OF_RANGE = 0xfc,
    FOTA_ERRCODE_UNINITIALIZED = 0xfd,
    FOTA_ERRCODE_UNSUPPORTED = 0xfe,
    FOTA_ERRCODE_FAIL = 0xff,
} FOTA_ERRCODE;


typedef enum {
    InternalFlash = 0,
    ExternalFlash = 1,

    Invalid = 0xFF
} FotaStorageType;


typedef enum {
    FOTA_INTEGRITY_CHECK_TYPE_CRC32 = 0,
    FOTA_INTEGRITY_CHECK_TYPE_SHA256 = 1,
    FOTA_INTEGRITY_CHECK_TYPE_SHA256_RSA = 2,

    FOTA_INTEGRITY_CHECK_TYPE_MAX = 0xFF
} fota_integrity_check_type_enum;


/**
 * @brief The FOTA package release version type.
 */
typedef enum {
    FOTA_VERSION_TYPE_NONE, /**< NONE type for initialization. */

    FOTA_VERSION_TYPE_STORED, /**< The type of the version stored in the multiple information sector. */
    FOTA_VERSION_TYPE_PACKAGE, /**< The type of the version included in the FOTA package downloaded. */

    FOTA_VERSION_TYPE_MAX /**< The maximum value of this enum. */
} fota_version_type_enum;


/**
 * @brief The structure for the basic information type.
 */
typedef struct stru_fota_basic_info {
    uint8_t compression_type; /* The compression type used by the FOTA package. */
    uint8_t integrity_check_type; /* The integrity check type used by the FOTA package. It is represented by fota_integrity_check_type_enum. */
    uint32_t fota_data_start_address; /* The start address of the FOTA data in the FOTA package. The FOTA data is the processed new firmware. */
    uint32_t fota_data_length; /* The length of the FOTA data. */
} PACKED FOTA_BASIC_INFO;


void fota_device_reboot(void);

FOTA_ERRCODE fota_crc32_generate(uint32_t *crc, uint32_t data_addr, uint32_t data_length, bool is_int);

/**
 * @brief This function generates the SHA256 value of the specified data. Be aware that it is a blocking API and when accessing the external
 * flash, it might take time to finish.
 * @param[out] sha256[32] The SHA256 value calculated.
 * @param[in] data_addr The start address of the data.
 * @param[in] data_length The length of the data.
 * @param[in] is_int It indicates if the data is stored in the internal flash or not.
 * @return FOTA_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
FOTA_ERRCODE fota_sha256_generate(unsigned char sha256[32], uint32_t data_addr, uint32_t data_length, bool is_int);

/**
 * @brief This function gets the information related to the integrity check.
 * @param[out] integrity_check_type The integrity check type.
 * @param[out] signature_start_address The start address of the signature included in the FOTA package.
 * @param[out] data_start_address The start address of the data that is used to generate the signature.
 * @param[out] data_length The length of the data.
 * @param[out] storage_type The storage type of the FOTA partition.
 * @return FOTA_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
FOTA_ERRCODE fota_get_integrity_check_info(fota_integrity_check_type_enum *integrity_check_type,
                                           uint32_t *signature_start_address,
                                           uint32_t *data_start_address,
                                           uint32_t *data_length,
                                           FotaStorageType *storage_type);

/**
 * @brief This function executes the integrity check to the FOTA package downloaded. Be aware that it is a blocking API and when accessing the external
 * flash, it might take time to finish.
 * @param[in] storage_type The storage type of the FOTA partition.
 * @return FOTA_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
FOTA_ERRCODE fota_check_fota_package_integrity(FotaStorageType storage_type);

/**
 * @brief This function gets the device type name
 * @param[out] buffer The device type name.
 * @param[in] buf_size The length of the data.
 * @return FOTA_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
FOTA_ERRCODE fota_parse_device_type_in_header(uint8_t *buffer,uint8_t buf_size);


FOTA_ERRCODE fota_version_get(uint8_t *buffer, uint8_t buf_size, fota_version_type_enum version_type);

FOTA_ERRCODE fota_version_set(uint8_t *version, uint8_t version_len, fota_version_type_enum version_type);

#endif /* __FOTA_UTIL_H__ */

