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
 * File: fota_util.c
 *
 * Description: This file implements some utility interfaces for FOTA.
 *
 * Note: See doc/Airoha_IoT_SDK_Firmware_Update_Developers_Guide.pdf for more detail.
 *
 */


#ifndef BL_FOTA_ENABLE
#include "FreeRTOS.h"
#endif
#include "fota_util.h"

#ifdef FOTA_USE_MBEDTLS_ENABLE
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#endif

#if defined(MBEDTLS_SHA256_C) && defined(FOTA_USE_MBEDTLS_SHA256_ENABLE)
#include "mbedtls/sha256.h"
#else
#include "hal_sha.h"
#include "hal_gpt.h"
#endif

#ifdef FOTA_RACE_CMD_CRC_SUPPORT
#include "crc32.h"
#endif
#include "hal_wdt.h"
#include "fota_flash.h"
#include "fota_multi_info.h"
#include "fota_platform.h"


void fota_device_reboot(void)
{
    hal_wdt_status_t ret;
    hal_wdt_config_t wdt_config;

    wdt_config.mode = HAL_WDT_MODE_RESET;
    wdt_config.seconds = 1;
    hal_wdt_init(&wdt_config);
    ret = hal_wdt_software_reset();
    FOTA_LOG_MSGID_I("fota_device_reboot() ret:%d", 1, ret);
}


FOTA_ERRCODE fota_crc32_generate(uint32_t *crc, uint32_t data_addr, uint32_t data_length, bool is_int)
{
#if defined(FOTA_RACE_CMD_CRC_SUPPORT)
    uint32_t crc32_init_value = 0xFFFFFFFF;
    int read_length = FOTA_LONG_DATA_READ_MAX_SIZE;
#ifdef BL_FOTA_ENABLE
    uint8_t buffer[FOTA_LONG_DATA_READ_MAX_SIZE] = {0};
#else
    uint8_t *buffer = NULL;
#endif

    if (!crc) {
        return FOTA_ERRCODE_FAIL;
    }

#ifndef BL_FOTA_ENABLE
    buffer = pvPortCalloc(1, FOTA_LONG_DATA_READ_MAX_SIZE);
    if (!buffer) {
        return FOTA_ERRCODE_READ_FOTA_DATA_FAIL;
    }
#endif

    while (data_length > 0) {
        if (data_length < FOTA_LONG_DATA_READ_MAX_SIZE) {
            read_length = data_length;
        }

        if (FOTA_ERRCODE_SUCCESS != fota_flash_read(data_addr, buffer, read_length, is_int)) {
#ifndef BL_FOTA_ENABLE
            vPortFree(buffer);
#endif
            return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
        }

        crc32_init_value = CRC32_Generate(buffer, read_length, crc32_init_value);
        data_addr += read_length;
        data_length -= read_length;
    }

#ifndef BL_FOTA_ENABLE
    vPortFree(buffer);
#endif
    *crc = crc32_init_value;
    return FOTA_ERRCODE_SUCCESS;
#else
    return FOTA_ERRCODE_UNSUPPORTED;
#endif
}


FOTA_ERRCODE fota_sha256_generate(unsigned char sha256[32], uint32_t data_addr, uint32_t data_length, bool is_int)
{
    int read_length = FOTA_LONG_DATA_READ_MAX_SIZE;
#ifdef BL_FOTA_ENABLE
    uint8_t buffer[FOTA_LONG_DATA_READ_MAX_SIZE] = {0};
#else
    uint8_t *buffer = NULL;
#endif
#if defined(MBEDTLS_SHA256_C) && defined(FOTA_USE_MBEDTLS_SHA256_ENABLE)
    mbedtls_sha256_context ctx;
#else
    hal_sha256_context_t ctx;
    hal_sha_status_t status = HAL_SHA_STATUS_ERROR;
#endif

    if (!sha256) {
        return FOTA_ERRCODE_FAIL;
    }

    memset(sha256, 0, 32);

    if (!data_length) {
        return FOTA_ERRCODE_SUCCESS;
    }

#ifndef BL_FOTA_ENABLE
    buffer = pvPortCalloc(1, FOTA_LONG_DATA_READ_MAX_SIZE);
    if (!buffer) {
        return FOTA_ERRCODE_READ_FOTA_DATA_FAIL;
    }
#endif

#if defined(MBEDTLS_SHA256_C) && defined(FOTA_USE_MBEDTLS_SHA256_ENABLE)
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);
#else
    memset(&ctx, 0, sizeof(hal_sha256_context_t));
    do {
        status = hal_sha256_init(&ctx);
        if (status == -100) {
            hal_gpt_delay_ms(1);
        }
    } while (status == -100);
#endif

    while (data_length > 0) {
        if (data_length < FOTA_LONG_DATA_READ_MAX_SIZE) {
            read_length = data_length;
        }

        if (FOTA_ERRCODE_SUCCESS != fota_flash_read(data_addr, buffer, read_length, is_int)) {
#ifndef BL_FOTA_ENABLE
            vPortFree(buffer);
#endif
            return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
        }

#if defined(MBEDTLS_SHA256_C) && defined(FOTA_USE_MBEDTLS_SHA256_ENABLE)
        mbedtls_sha256_update(&ctx, buffer, read_length);
#else
        do {
            status = hal_sha256_append(&ctx, (uint8_t *)buffer, read_length);
            if (status == -100) {
                hal_gpt_delay_ms(1);
            }
        } while (status == -100);
#endif
        data_addr += read_length;
        data_length -= read_length;
    }

#ifndef BL_FOTA_ENABLE
    vPortFree(buffer);
#endif

#if defined(MBEDTLS_SHA256_C) && defined(FOTA_USE_MBEDTLS_SHA256_ENABLE)
    mbedtls_sha256_finish(&ctx, sha256);
    mbedtls_sha256_free(&ctx);
#else
    do {
        status = hal_sha256_end(&ctx, (uint8_t *)sha256);
        if (status == -100) {
            hal_gpt_delay_ms(1);
        }
    } while (status == -100);
#endif

    return FOTA_ERRCODE_SUCCESS;
}


FOTA_ERRCODE fota_check_crc32(uint32_t signature_address, uint8_t *ptr_data_start, uint32_t data_length, bool is_int)
{
#define CRC32_LENGTH 4

    uint32_t crc_in_fota_partition;
    uint32_t crc = 0;
    FOTA_ERRCODE ret = fota_crc32_generate((uint32_t *)&crc, (uint32_t)ptr_data_start, (uint32_t)data_length, is_int);
    if (FOTA_ERRCODE_SUCCESS != ret) {
        return ret;
    }

    if (FOTA_ERRCODE_SUCCESS != fota_flash_read(signature_address, (uint8_t *)&crc_in_fota_partition, CRC32_LENGTH, is_int)) {
        return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
    } else {
        if (crc != crc_in_fota_partition) {
            return FOTA_ERRCODE_CHECK_INTEGRITY_FAIL;
        }
    }
    return FOTA_ERRCODE_SUCCESS;
}


FOTA_ERRCODE fota_check_sha256(uint32_t signature_address, uint8_t *ptr_data_start, uint32_t data_length, bool is_int)
{
#define SHA256_LENGTH 32

    unsigned char sha256[SHA256_LENGTH];

    uint8_t sha256_in_fota_partition[SHA256_LENGTH];

    /* Calculate the SHA256 value of the specified data. */
    FOTA_ERRCODE ret = fota_sha256_generate(sha256, (uint32_t)ptr_data_start, (uint32_t)data_length, is_int);
    if (FOTA_ERRCODE_SUCCESS != ret) {
        return ret;
    }

    /* Read the SHA256 value from the flash. */
    if (FOTA_ERRCODE_SUCCESS != fota_flash_read(signature_address, sha256_in_fota_partition, SHA256_LENGTH, is_int)) {
        return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
    } else {
        /* Compare the SHA256 value calculated with the SHA256 value read from the flash. Only when they are equal, the SHA256 integrity check of the
         * data passes. */
        if (strncmp((const char *)sha256_in_fota_partition, (const char *)sha256, SHA256_LENGTH) != 0) {
            return FOTA_ERRCODE_CHECK_INTEGRITY_FAIL;
        }
    }

    return FOTA_ERRCODE_SUCCESS;
}


FOTA_ERRCODE fota_check_sha256_rsa(uint32_t signature_address, uint8_t *ptr_data_start, uint32_t data_length, bool is_int)
{
#if defined(MBEDTLS_SHA256_C) && defined(FOTA_USE_MBEDTLS_SHA256_ENABLE) && defined(MBEDTLS_RSA_C) && defined(FOTA_USE_MBEDTLS_RSA_ENABLE)

    UNUSED(signature_address);
    UNUSED(ptr_data_start);
    UNUSED(data_length);

    return FOTA_ERRCODE_SUCCESS;
#else
    return FOTA_ERRCODE_SHA256_IS_NOT_SUPPORTED;
#endif
}


FOTA_ERRCODE fota_get_integrity_check_info(fota_integrity_check_type_enum *integrity_check_type,
                                           uint32_t *signature_start_address,
                                           uint32_t *data_start_address,
                                           uint32_t *data_length,
                                           FotaStorageType *storage_type)
{
#define BUFFER_SIZE 32
    uint32_t fota_header_address = 0;
    int32_t ret = FOTA_ERRCODE_FAIL;
    bool is_int = TRUE;
    uint16_t tlv_type = 0, tlv_length = 0;
    uint8_t buffer[BUFFER_SIZE] = {0};
    FOTA_BASIC_INFO *pHeader_info = NULL;

    if (!integrity_check_type || !signature_start_address || !data_start_address ||
        !data_length || !storage_type) {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    *integrity_check_type = FOTA_INTEGRITY_CHECK_TYPE_MAX;
    *storage_type = Invalid;

    ret = fota_flash_get_fota_partition_info(storage_type,
                                             signature_start_address,
                                             data_length);
    if (FOTA_ERRCODE_SUCCESS != ret) {
        return ret;
    }

    /* The structure of the FOTA package is the signature, the FOTA header and the FOTA data. */
    fota_header_address = *signature_start_address + SIGNATURE_SIZE;
    is_int = (InternalFlash == *storage_type);

    do {
        /* The format of the TLV information is type, length and value. */
        /* Read the type of the TLV information. */
        ret = fota_flash_read(fota_header_address,
                              (uint8_t *)&tlv_type,
                              sizeof(tlv_type),
                              is_int);
        if (FOTA_ERRCODE_SUCCESS != ret || INVALID_TLV_VALUE == tlv_type) {
            ret = FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
            break;
        }

        /* Read the length of the TLV information. */
        fota_header_address += sizeof(tlv_type);
        ret = fota_flash_read(fota_header_address,
                              (uint8_t *)&tlv_length,
                              sizeof(tlv_length),
                              is_int);
        if (FOTA_ERRCODE_SUCCESS != ret || 0 == tlv_length) {
            ret = FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
            break;
        }

        /* Read the value of the TLV information. */
        fota_header_address += sizeof(tlv_length);
        if (tlv_type == FOTA_HEADER_TLV_BASIC_INFO) {
            ret = fota_flash_read(fota_header_address,
                                  buffer,
                                  tlv_length,
                                  is_int);
            if (FOTA_ERRCODE_SUCCESS != ret) {
                break;
            }

            pHeader_info = (FOTA_BASIC_INFO *)buffer;
            *integrity_check_type = pHeader_info->integrity_check_type;
            *data_start_address = *signature_start_address + SIGNATURE_SIZE;
            *data_length = FOTA_HEADER_OCCUPIED_SIZE - SIGNATURE_SIZE + pHeader_info->fota_data_length;
            return FOTA_ERRCODE_SUCCESS;
        }
        fota_header_address += tlv_length;
    } while (fota_header_address < *signature_start_address + FOTA_HEADER_OCCUPIED_SIZE);

    if (fota_header_address >= *signature_start_address + FOTA_HEADER_OCCUPIED_SIZE) {
        ret = FOTA_ERRCODE_OUT_OF_RANGE;
    }

    FOTA_LOG_MSGID_E("ret:%d", 1, ret);
    return ret;
}

FOTA_ERRCODE fota_check_fota_package_integrity(FotaStorageType storage_type)
{
#define BUFFER_SIZE 32
#define BLOCK_SIZE (4*1024)
#define BASIC_BUFFER_SIZE 4

    uint32_t OTA_START_ADDR, length;
    FotaStorageType real_storage_type;
    FOTA_ERRCODE err = FOTA_ERRCODE_FAIL;
    bool is_int = TRUE;
    uint16_t tlv_type;
    uint16_t tlv_length;
    uint8_t buffer[BUFFER_SIZE];
    uint8_t basic_buffer[BASIC_BUFFER_SIZE];
    uint32_t curr_addr;
    FOTA_ERRCODE rtn;
    FOTA_BASIC_INFO *pHeader_info;

    err = fota_flash_get_fota_partition_info(&real_storage_type, &OTA_START_ADDR, &length);
    if (FOTA_ERRCODE_SUCCESS != err || storage_type != real_storage_type) {
        FOTA_LOG_MSGID_E("fota_check_fota_package_integrity:read fota partition info fail!", 0);
        return err;
    }
    /* Point to the type of the first TLV information. */
    curr_addr = OTA_START_ADDR + SIGNATURE_SIZE;

    is_int = (InternalFlash == storage_type);

    /* Parse the FOTA header to get the integrity check type and choose the corresponding method to execute the integrity check of
     * the FOTA pcakge.
     */

    /*get the basic information*/
    if (FOTA_ERRCODE_SUCCESS != fota_flash_read(curr_addr, &basic_buffer[0], sizeof(basic_buffer), is_int)) {
        FOTA_LOG_MSGID_E("fota_check_fota_package_integrity:read flash fail!", 0);
        return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
    }

    /*check the basic information first*/
    if(!(basic_buffer[0] == 0x11 && basic_buffer[1] == 0x00 && basic_buffer[2] == 0x0a && basic_buffer[3] == 0x00)) {
        FOTA_LOG_MSGID_E("fota_check_fota_package_integrity:%x,%x,%x,%x", 4, basic_buffer[0], basic_buffer[1], basic_buffer[2], basic_buffer[3]);
        return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
    }

    tlv_type = basic_buffer[0] + ((uint16_t)basic_buffer[1] << 16);
    tlv_length = basic_buffer[2] + ((uint16_t)basic_buffer[3] << 16);

    /* Point to the length of the TLV information. */
    curr_addr += sizeof(tlv_type);

    /* Point to the data of the TLV information. */
    curr_addr += sizeof(tlv_length);

    /*check the tlv type and tlv length*/
    if (tlv_type == FOTA_HEADER_TLV_BASIC_INFO) {
        if (FOTA_ERRCODE_SUCCESS != fota_flash_read(curr_addr, &buffer[0], tlv_length, is_int)) {
            FOTA_LOG_MSGID_E("fota_check_fota_package_integrity:read buffer fail!", 0);
            return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
        }

        pHeader_info = (FOTA_BASIC_INFO *)buffer;

        /*check the fota data length + package header(4k) + last 4K whether are out of range the fota partition size*/
        if ((pHeader_info->fota_data_length + 2 * BLOCK_SIZE) > length) {
            FOTA_LOG_MSGID_E("fota_data_length:address=%x,length=%x", 2, pHeader_info->fota_data_start_address, pHeader_info->fota_data_length);
            return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
        }

        /*check whether the fota start_address < header size(4k)
        * or the fota start_address + data length + last 4K are out of range the fota partition size*/
        if ((pHeader_info->fota_data_start_address < BLOCK_SIZE) ||
            (pHeader_info->fota_data_start_address + pHeader_info->fota_data_length + BLOCK_SIZE > length)) {
            FOTA_LOG_MSGID_E("fota_data_start_address:address=%x,length=%x", 2, pHeader_info->fota_data_start_address, pHeader_info->fota_data_length);
            return FOTA_ERRCODE_READ_FOTA_HEADER_FAIL;
        }

        /* Get the integrity check type from the TLV information of the basic information type. And choose the integrity check
         * method accordingly. When calculating the signature of the FOTA package, the signature included the FOTA package
         * should be skipped.
         */
        switch (pHeader_info->integrity_check_type) {
            case FOTA_INTEGRITY_CHECK_TYPE_CRC32:
                rtn = fota_check_crc32(OTA_START_ADDR, (uint8_t *)(OTA_START_ADDR + SIGNATURE_SIZE),
                                        FOTA_HEADER_OCCUPIED_SIZE - SIGNATURE_SIZE + pHeader_info->fota_data_length, is_int);
                break;
            case FOTA_INTEGRITY_CHECK_TYPE_SHA256:
                rtn = fota_check_sha256(OTA_START_ADDR, (uint8_t *)(OTA_START_ADDR + SIGNATURE_SIZE),
                                        FOTA_HEADER_OCCUPIED_SIZE - SIGNATURE_SIZE + pHeader_info->fota_data_length, is_int);

                break;
            case FOTA_INTEGRITY_CHECK_TYPE_SHA256_RSA:
                rtn = fota_check_sha256_rsa(OTA_START_ADDR, (uint8_t *)(OTA_START_ADDR + SIGNATURE_SIZE),
                                        FOTA_HEADER_OCCUPIED_SIZE - SIGNATURE_SIZE + pHeader_info->fota_data_length, is_int);
                break;

            default:
                return FOTA_ERRCODE_UNKNOWN_INTEGRITY_CHECK_TYPE;
        }

        if (FOTA_ERRCODE_SUCCESS == rtn) {
            FOTA_LOG_MSGID_I("fota_check_fota_package_integrity:package integrity pass", 0);
#ifndef BL_FOTA_ENABLE
            /* Write the integrity check result into the multiple information sector. */
            rtn = fota_dl_integrity_res_write(FOTA_DL_INTEGRITY_RES_VAL_PASS);
#endif
        } else {
                /* Do nothing. Allow to check the integrity multiple times until it passes. */
        }

            return rtn;
    }else {
            FOTA_LOG_MSGID_E("fota_check_fota_package_integrity:wrong basic header info", 0);
    }
    return FOTA_ERRCODE_FAIL;
}

FOTA_ERRCODE fota_parse_device_type_in_header(uint8_t *buffer, uint8_t buf_size)
{
#ifndef BL_FOTA_ENABLE
    uint16_t tlv_type, tlv_length;
    uint32_t fota_partition_start_address, length, curr_addr;
    FotaStorageType flash_type;

    FOTA_LOG_MSGID_I("start parser header,buf_size=%d", 1, buf_size);
    if (!buffer || !buf_size) {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    if (fota_flash_get_fota_partition_info(&flash_type, &fota_partition_start_address, &length) != FOTA_ERRCODE_SUCCESS) {
        FOTA_LOG_MSGID_E("Read fota partition info failed.", 0);
        return FOTA_ERRCODE_FAIL;
    }

    /* Point to the type of the first TLV information. */
    curr_addr = fota_partition_start_address + SIGNATURE_SIZE;
    do {
        if (fota_flash_read(curr_addr, (uint8_t *)&tlv_type, sizeof(tlv_type), InternalFlash == flash_type) != FOTA_ERRCODE_SUCCESS) {
            FOTA_LOG_MSGID_E("Read record type in Bisc info failed.", 0);
            return FOTA_ERRCODE_FAIL;
        }

        /* Point to the length of the TLV information. */
        curr_addr += sizeof(tlv_type);
        if (fota_flash_read(curr_addr, (uint8_t *)&tlv_length, sizeof(tlv_length), InternalFlash == flash_type) != FOTA_ERRCODE_SUCCESS) {
            FOTA_LOG_MSGID_E("Read record length in Bisc info failed.", 0);
            return FOTA_ERRCODE_FAIL;
        }

        FOTA_LOG_MSGID_I("cur_addr:%lx, tlv_type:%x, tlv_length:%d", 3, curr_addr, tlv_type, tlv_length);
        /* Point to the data of the TLV information. */
        curr_addr += sizeof(tlv_length);

        if (FOTA_HEADER_TLV_DEVICE_TYPE_INFO == tlv_type) {

            if (tlv_length + 1 > buf_size) {
                return FOTA_ERRCODE_FAIL;
            }

            if (fota_flash_read(curr_addr, buffer, tlv_length, InternalFlash == flash_type) != FOTA_ERRCODE_SUCCESS) {
                FOTA_LOG_MSGID_E("Read device type in Bisc info failed.", 0);
                return FOTA_ERRCODE_FAIL;
            }

            buffer[tlv_length] = '\0';

            FOTA_LOG_MSGID_I("read finish tlv_length:%x", 1, tlv_length);
            /*for (int i = 0 ; i < tlv_length; i++) {
                FOTA_LOG_MSGID_I("device type read_data: i=%d, ver = %x ", 2, i, buffer[i]);
            }*/
            return FOTA_ERRCODE_SUCCESS;
        }

        /* Point to the next TLV information. */
        curr_addr += tlv_length;
    } while (INVALID_TLV_VALUE != tlv_type);
#endif
    return FOTA_ERRCODE_FAIL;
}

static FOTA_ERRCODE fota_parse_version_in_header(uint8_t *buffer, uint8_t buf_size)
{
#ifndef BL_FOTA_ENABLE
    uint16_t tlv_type, tlv_length;
    uint32_t fota_partition_start_address, length, curr_addr;
    FotaStorageType flash_type;

    FOTA_LOG_MSGID_I("start parser header", 0);
    if (!buffer || !buf_size) {
        return FOTA_ERRCODE_INVALID_PARAMETER;
    }

    if (fota_flash_get_fota_partition_info(&flash_type, &fota_partition_start_address, &length) != FOTA_ERRCODE_SUCCESS) {
        FOTA_LOG_MSGID_E("Read record length in Bisc info failed.", 0);
        return FOTA_ERRCODE_FAIL;
    }

    /* Point to the type of the first TLV information. */
    curr_addr = fota_partition_start_address + SIGNATURE_SIZE;

    do {
        if (fota_flash_read(curr_addr, (uint8_t *)&tlv_type, 2, InternalFlash == flash_type) != FOTA_ERRCODE_SUCCESS) {
            FOTA_LOG_MSGID_E("Read record length in Bisc info failed.", 0);
            return FOTA_ERRCODE_FAIL;
        }

        /* Point to the length of the TLV information. */
        curr_addr += 2;
        if (fota_flash_read(curr_addr, (uint8_t *)&tlv_length, 2, InternalFlash == flash_type) != FOTA_ERRCODE_SUCCESS) {
            FOTA_LOG_MSGID_E("Read record length in Bisc info failed.", 0);
            return FOTA_ERRCODE_FAIL;
        }

        FOTA_LOG_MSGID_I("cur_addr:%lx, tlv_type:%x, tlv_length:%d", 3, curr_addr, tlv_type, tlv_length);
        /* Point to the data of the TLV information. */
        curr_addr += 2;

        if (FOTA_HEADER_TLV_VERSION_INFO == tlv_type) {

            if (tlv_length > FOTA_VERSION_MAX_SIZE || tlv_length > buf_size) {
                return FOTA_ERRCODE_FAIL;
            }

            if (fota_flash_read(curr_addr, buffer, tlv_length, InternalFlash == flash_type) != FOTA_ERRCODE_SUCCESS) {
                return FOTA_ERRCODE_FAIL;
            }

            buffer[tlv_length] = '\0';

            FOTA_LOG_MSGID_I("read finish tlv_length:%x", 1, tlv_length);
            for (int i = 0 ; i < tlv_length; i++) {
                FOTA_LOG_MSGID_I("read_data: i=%d, ver = %x ", 2, i, buffer[i]);
            }
            return FOTA_ERRCODE_SUCCESS;
        }

        /* Point to the next TLV information. */
        curr_addr += tlv_length;
    } while (INVALID_TLV_VALUE != tlv_type);
#endif
    return FOTA_ERRCODE_FAIL;
}


/**
 * @brief The function gets the FOTA package release version. If it fails to read the version both from the multiple information
 * sector and from NVDM, FOTA_DEFAULT_VERSION will be returned.
 * @param[in] buffer A buffer to store the version string.
 * @param[in] buf_size The size of the buffer.
 * @return FOTA_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
static FOTA_ERRCODE fota_stored_version_get(uint8_t *buffer, uint8_t buf_size)
{
    FOTA_ERRCODE ret = fota_version_read(buffer, buf_size);
    int32_t default_version_len = strlen(FOTA_DEFAULT_VERSION);

    if (FOTA_ERRCODE_UNINITIALIZED == ret) {
        if (buf_size >= default_version_len + 1) {
            memcpy(buffer, FOTA_DEFAULT_VERSION, default_version_len);
            ret = FOTA_ERRCODE_SUCCESS;
        } else {
            ret = FOTA_ERRCODE_INVALID_PARAMETER;
        }
    }

    return ret;
}


static FOTA_ERRCODE fota_stored_version_set(uint8_t *version, uint8_t version_len)
{
    return fota_version_write(version, version_len);
}


FOTA_ERRCODE fota_version_get(uint8_t *buffer, uint8_t buf_size, fota_version_type_enum version_type)
{
    if (FOTA_VERSION_TYPE_STORED == version_type) {
        return fota_stored_version_get(buffer, buf_size);
    } else if (FOTA_VERSION_TYPE_PACKAGE == version_type) {
        return fota_parse_version_in_header(buffer, buf_size);
    }

    return FOTA_ERRCODE_FAIL;
}


FOTA_ERRCODE fota_version_set(uint8_t *version, uint8_t version_len, fota_version_type_enum version_type)
{
    if (FOTA_VERSION_TYPE_STORED == version_type) {
        return fota_stored_version_set(version, version_len);
    } else if (FOTA_VERSION_TYPE_PACKAGE == version_type) {
        return FOTA_ERRCODE_UNSUPPORTED;
    }

    return FOTA_ERRCODE_FAIL;
}

