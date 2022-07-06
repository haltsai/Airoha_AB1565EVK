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


#include "race_cmd_feature.h"
#ifdef RACE_STORAGE_CMD_ENABLE
#include "FreeRTOS.h"
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs_internal.h"
#endif
#include "bsp_flash.h"
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif
#ifdef MBEDTLS_SHA256_C
#include "mbedtls/sha256.h"
#endif
#include "race_storage_util.h"
#include "race_util.h"
#include "crc8.h"


#ifdef RACE_FOTA_STORE_IN_EXTERNAL_FLASH
    #define RACE_STORAGE_LONG_DATA_READ_MAX_SIZE      (1024)
    /* For non-block API */
    #if ((defined(MT2822)) || (defined(AB156X)))
        #define RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE   (0x100000)
    #else
        #define RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE   (0x400000)
    #endif
#else
    #define RACE_STORAGE_LONG_DATA_READ_MAX_SIZE      (256)
    /* For non-block API */
    #if ((defined(MT2822)) || (defined(AB156X)))
        #define RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE   (0x100000)
    #else
        #define RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE   (0x400000)
    #endif
#endif /* RACE_FOTA_STORE_IN_EXTERNAL_FLASH */


static uint32_t s_sha256_data_size_blocking = 0;
static uint32_t s_sha256_data_size_non_blocking = 0;

#ifdef HAL_DVFS_MODULE_ENABLED
    #if ((defined(MT2822)) || (defined(AB156X)))

        #define TARGET_CPU_CLOCK    HAL_DVFS_HIGH_SPEED_208M
        /* #define TARGET_CPU_CLOCK    HAL_DVFS_FULL_SPEED_104M */

        #define DVFS_LOCK(record_data, var_size, dvfs_condition_size)          \
                { \
                    record_data = var_size; \
                    if(var_size > (dvfs_condition_size)) { \
                        hal_dvfs_status_t dvfs_ret = hal_dvfs_lock_control(TARGET_CPU_CLOCK, HAL_DVFS_LOCK); \
                        RACE_LOG_MSGID_W("[RACE Storage] dvfs lock return %d and CPU freq is %d Hz.", 2, dvfs_ret, hal_clock_get_freq_meter(_hf_fsys_ck, 100)); \
                    } \
                }

        #define DVFS_UNLOCK(record_data, dvfs_condition_size)          \
                { \
                    if(record_data > (dvfs_condition_size)){ \
                        hal_dvfs_status_t dvfs_ret = hal_dvfs_lock_control(TARGET_CPU_CLOCK, HAL_DVFS_UNLOCK); \
                        RACE_LOG_MSGID_W("[RACE Storage] dvfs unlock return %d and CPU freq is %d Hz.", 2, dvfs_ret, hal_clock_get_freq_meter(_hf_fsys_ck, 100)); \
                    } \
                }

    #else

        #define DVFS_LOCK(record_data, var_size, dvfs_condition_size)

        #define DVFS_UNLOCK(record_data, dvfs_condition_size)

    #endif /* chip MACRO related */
#else
    #define DVFS_LOCK(record_data, var_size, dvfs_condition_size)

    #define DVFS_UNLOCK(record_data, dvfs_condition_size)
#endif /* HAL_DVFS_MODULE_ENABLED */



RACE_ERRCODE race_storage_write(uint32_t start_address,
                                uint8_t *buffer,
                                uint32_t length,
                                uint8_t storage_type)
{
    int32_t ret = RACE_ERRCODE_FAIL;

    if (0 == storage_type) {
        ret = bsp_flash_write(start_address, buffer, length);
        return BSP_FLASH_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
#ifdef RACE_STORAGE_BSP_FLASH_ENABLE
    else if (1 == storage_type) {
        /*for external flash, start address should be within SPI_SERIAL_FLASH_ADDRESS to SPI_SERIAL_FLASH_ADDRESS+flash size*/
        start_address |= SPI_SERIAL_FLASH_ADDRESS;
        ret = bsp_flash_write(start_address, buffer, length);
        //RACE_LOG_MSGID_I("bsp_flash_write ret:%d start_address:%x length:%d", 3, ret, start_address, length);
        if (BSP_FLASH_STATUS_NOT_INIT == ret) {
            /* Other task may deinit external Flash. */
            bsp_flash_init();
            ret = bsp_flash_write(start_address, buffer, length);
        }
        //RACE_LOG_MSGID_I("bsp_flash_write ret:%d", 1, ret);
        return BSP_FLASH_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
#endif

    return RACE_ERRCODE_NOT_SUPPORT;
}


RACE_ERRCODE race_storage_read(uint32_t start_address,
                               uint8_t *buffer,
                               uint32_t length,
                               uint8_t storage_type)
{
    int32_t ret = RACE_ERRCODE_FAIL;

    if (0 == storage_type) {
        /* Internal flash */
        ret = bsp_flash_read(start_address, buffer, length);
        return BSP_FLASH_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
#ifdef RACE_STORAGE_BSP_FLASH_ENABLE
    else if (1 == storage_type) {
        /* External flash */
        start_address |= SPI_SERIAL_FLASH_ADDRESS;
        ret = bsp_flash_read(start_address, buffer, length);
        if (BSP_FLASH_STATUS_NOT_INIT == ret) {
            /* Other task may deinit external Flash. */
            bsp_flash_init();
            ret = bsp_flash_read(start_address, buffer, length);
        }
        return BSP_FLASH_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
#endif

    return RACE_ERRCODE_NOT_SUPPORT;
}


RACE_ERRCODE race_storage_erase(uint32_t address,
                                race_storage_block_type_enum block_type,
                                uint8_t storage_type)
{
    uint8_t real_block_type = 0;
    int32_t ret = RACE_ERRCODE_FAIL;

    switch (block_type) {
        case RACE_STORAGE_BLOCK_TYPE_4K: {
            real_block_type = BSP_FLASH_BLOCK_4K;
            break;
        }

        case RACE_STORAGE_BLOCK_TYPE_32K: {
            real_block_type = BSP_FLASH_BLOCK_32K;
            break;
        }

        case RACE_STORAGE_BLOCK_TYPE_64K: {
            real_block_type = BSP_FLASH_BLOCK_64K;
            break;
        }

        default:
            return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (0 == storage_type) {
        /* Internal flash */
        ret = bsp_flash_erase(address, real_block_type);
        return BSP_FLASH_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
#ifdef RACE_STORAGE_BSP_FLASH_ENABLE
    else if (1 == storage_type) {
        /* External flash */
        address |= SPI_SERIAL_FLASH_ADDRESS;
        ret = bsp_flash_erase(address, real_block_type);
        if (BSP_FLASH_STATUS_NOT_INIT == ret) {
            /* Other task may deinit external Flash. */
            bsp_flash_init();
            ret = bsp_flash_erase(address, real_block_type);
        }

        return BSP_FLASH_STATUS_OK == ret ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_FAIL;
    }
#endif

    return RACE_ERRCODE_NOT_SUPPORT;
}


/* Actual erase size: block_num*4K */
RACE_ERRCODE race_storage_block_erase(uint8_t storage_type,
                                      uint32_t *address,
                                      uint32_t *block_num)
{
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;
    uint32_t block_addr;

    if (!address || !block_num || (*address & 0xfff) ||
            (0 != storage_type
#ifdef RACE_STORAGE_BSP_FLASH_ENABLE
             && 1 != storage_type
#endif
            )) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    block_addr = *address >> 12;  /*block_addr = address / 4KByte*/

    if (0 == *block_num) {
        return RACE_ERRCODE_SUCCESS;
    }

    if (!(block_addr & 0x0F) && (*block_num >= 16)) {/*if start address is 64K align and erase size >= 64K, erase with 64K */
        ret = race_storage_erase(*address,
                                 RACE_STORAGE_BLOCK_TYPE_64K,
                                 storage_type);
        if (RACE_ERRCODE_SUCCESS != ret) {
            return ret;
        }

        (*block_num) -= 16;
        (*address) += (16 << 12);
    } else if (!(block_addr & 0x07) && (*block_num >= 8)) {/*if start address is 32K align and erase size >= 32K, erase with 32K */
        ret = race_storage_erase(*address,
                                 RACE_STORAGE_BLOCK_TYPE_32K,
                                 storage_type);
        if (RACE_ERRCODE_SUCCESS != ret) {
            return ret;
        }

        (*block_num) -= 8;
        (*address) += (8 << 12);
    } else {
        ret = race_storage_erase(*address,
                                 RACE_STORAGE_BLOCK_TYPE_4K,
                                 storage_type);
        if (RACE_ERRCODE_SUCCESS != ret) {
            return ret;
        }

        (*block_num)--;
        (*address) += (1 << 12);
    }

    if (*block_num) {
        return RACE_ERRCODE_MORE_OPERATION;
    }

    return RACE_ERRCODE_SUCCESS;
}


/* Callback will only be invoked latter when the return value is RACE_ERRCODE_MORE_OPERATION.
  * It will be invoked when the partition is erased completely or any error occured after this API.
  * return value:
  * RACE_ERRCODE_SUCCESS: The partition is erased completely
  * RACE_ERRCODE_MORE_OPERATION: Partial of the partition is erased and callback will be invoked
  *                                                           when the whole partition is erased or any error occured after this API.
  * Other: Error occurs.
  */
RACE_ERRCODE race_storage_erase_partition(uint8_t storage_type,
        uint32_t partition_length,
        uint32_t partition_address,
        uint8_t app_id,
        uint8_t channel_id,
        bool noti_delay,
        race_erase_partition_done_callback callback,
        void *user_data)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    uint32_t block_num = 0, address = 0;
    race_general_msg_t msg_queue_item = {0};
    race_storage_erase_partition_continue_msg_struct *msg = NULL;

    if ((partition_length & 0xFFF) ||
            (0 != storage_type
#ifdef RACE_STORAGE_BSP_FLASH_ENABLE
             && 1 != storage_type
#endif
            )) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    if (0 == partition_length) {
        return RACE_ERRCODE_SUCCESS;
    }

    block_num = partition_length >> 12;
    address = partition_address;

    ret = race_storage_block_erase(storage_type,
                                   &address,
                                   &block_num);
    RACE_LOG_MSGID_I("race_storage_block_erase ret:%x", 1, ret);
    if (RACE_ERRCODE_MORE_OPERATION != ret) {
        return ret;
    }

    msg = race_mem_alloc(sizeof(race_storage_erase_partition_continue_msg_struct));
    if (!msg) {
        RACE_LOG_MSGID_W("Lost a ERASE_PARTITION_CONTINUE message!", 0);
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    msg->storage_type = storage_type;
    msg->partition_length = partition_length;
    msg->partition_address = partition_address;
    msg->block_num = block_num;
    msg->address = address;
    msg->app_id = app_id;
    msg->channel_id = channel_id;
    msg->callback = callback;
    msg->noti_delay = noti_delay;
    msg->user_data = user_data;

    msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_ERASE_PARTITION_CONTINUE_IND;
    msg_queue_item.msg_data = (uint8_t *)msg;
    ret = race_send_msg(&msg_queue_item);
    if (RACE_ERRCODE_SUCCESS != ret) {
        RACE_LOG_MSGID_W("Lost a ERASE_PARTITION_CONTINUE message!", 0);
        race_mem_free((void *)msg);
    }

    return ret;
}


RACE_ERRCODE race_storage_erase_partition_continue_msg_process(race_general_msg_t *msg)
{
    int32_t ret = RACE_ERRCODE_FAIL;
    race_storage_erase_partition_continue_msg_struct *continue_msg = NULL;
    race_general_msg_t msg_queue_item = {0};

    if (!msg || !msg->msg_data) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    continue_msg = (race_storage_erase_partition_continue_msg_struct *)msg->msg_data;
    /* Continue to erase the partition */
    ret = race_storage_block_erase(continue_msg->storage_type,
                                   &(continue_msg->address),
                                   &(continue_msg->block_num));
    RACE_LOG_MSGID_I("race_storage_block_erase ret:%x", 1, ret);
    if (RACE_ERRCODE_MORE_OPERATION == ret) {
        /* The partition is still not erased completely. Send MSG to continue to erase. */
        msg_queue_item.msg_id = MSG_ID_RACE_LOCAL_ERASE_PARTITION_CONTINUE_IND;
        /* Reuse the continue_msg */
        msg_queue_item.msg_data = (uint8_t *)continue_msg;
        /* msg is freed below if fail to send. */
        ret = race_send_msg(&msg_queue_item);
        if (RACE_ERRCODE_SUCCESS == ret) {
            return ret;
        }
    }

    /* Invoke the callback */
    if (continue_msg->callback) {
        continue_msg->callback(ret,
                               continue_msg->storage_type,
                               continue_msg->partition_length,
                               continue_msg->partition_address,
                               continue_msg->app_id,
                               continue_msg->channel_id,
                               continue_msg->noti_delay,
                               continue_msg->user_data);
    }

    race_mem_free(continue_msg);
    msg->msg_data = NULL;

    return ret;
}


RACE_ERRCODE race_storage_sha256_generate(unsigned char sha256[RACE_STORAGE_SHA256_SIZE],
        uint32_t data_addr,
        uint32_t data_length,
        uint8_t storage_type)
{
#if defined(MBEDTLS_SHA256_C)
    int read_length = RACE_STORAGE_LONG_DATA_READ_MAX_SIZE;
    uint8_t *buffer = NULL;
    mbedtls_sha256_context ctx;
    int32_t ret = RACE_ERRCODE_FAIL;

    if (!sha256) {
        return RACE_ERRCODE_FAIL;
    }

    memset(sha256, 0, RACE_STORAGE_SHA256_SIZE);

    if (!data_length) {
        return RACE_ERRCODE_SUCCESS;
    }

    buffer = race_mem_alloc(RACE_STORAGE_LONG_DATA_READ_MAX_SIZE);
    if (!buffer) {
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts(&ctx, 0);

#ifdef HAL_DVFS_MODULE_ENABLED
#if ((defined(AB155X)) || (defined(AM255X)))
    ret = hal_dvfs_lock_control( DVFS_156M_SPEED, HAL_DVFS_LOCK);
    if (HAL_DVFS_STATUS_OK != ret) {
        race_mem_free(buffer);
        RACE_LOG_MSGID_E("Failed to lock alone cpu frequency.", 0);
        return RACE_ERRCODE_FAIL;
    }
#endif
#endif

    DVFS_LOCK(s_sha256_data_size_blocking, data_length, RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE);

    ret = RACE_ERRCODE_SUCCESS;
    while (data_length > 0) {
        if (data_length < RACE_STORAGE_LONG_DATA_READ_MAX_SIZE) {
            read_length = data_length;
        }

        ret = race_storage_read(data_addr, buffer, read_length, storage_type);
        if (RACE_ERRCODE_SUCCESS != ret) {
            ret = RACE_ERRCODE_STORAGE_READ_FAIL;
            break;
        }

        mbedtls_sha256_update(&ctx, buffer, read_length);
        data_addr += read_length;
        data_length -= read_length;
    }

    DVFS_UNLOCK(s_sha256_data_size_blocking, RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE);

#ifdef HAL_DVFS_MODULE_ENABLED
#if ((defined(AB155X)) || (defined(AM255X)))
    hal_dvfs_lock_control( DVFS_156M_SPEED, HAL_DVFS_UNLOCK);
#endif
#endif

    race_mem_free(buffer);
    if (RACE_ERRCODE_SUCCESS == ret) {
        mbedtls_sha256_finish(&ctx, sha256);
    }
    mbedtls_sha256_free(&ctx);
    return ret;
#else
    return RACE_ERRCODE_NOT_SUPPORT;
#endif
}


// !!!Warning: Only support calculating the crc8 of 256 bytes data.
RACE_ERRCODE race_storage_crc8_generate(uint8_t *crc8,
                                        uint32_t data_addr,
                                        uint32_t data_length,
                                        uint8_t storage_type)
{
    int read_length = RACE_STORAGE_LONG_DATA_READ_MAX_SIZE;
    uint8_t *buffer = (uint8_t *)data_addr;
    int32_t ret = RACE_ERRCODE_FAIL;

    if (!crc8) {
        return RACE_ERRCODE_FAIL;
    }

    *crc8 = 0;

    if (!data_length) {
        return RACE_ERRCODE_SUCCESS;
    }

    if (!buffer) {
        return RACE_ERRCODE_FAIL;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
#if ((defined(AB155X)) || (defined(AM255X)))
    ret = hal_dvfs_lock_control( DVFS_156M_SPEED, HAL_DVFS_LOCK);
    if (HAL_DVFS_STATUS_OK != ret) {
        RACE_LOG_MSGID_E("Failed to lock alone cpu frequency.", 0);
        return RACE_ERRCODE_FAIL;
    }
#endif
#endif

    ret = RACE_ERRCODE_SUCCESS;
    while (data_length > 0) {
        if (data_length < RACE_STORAGE_LONG_DATA_READ_MAX_SIZE) {
            read_length = data_length;
        }

        *crc8 = (uint8_t)CRC8_Generate((U8 *)buffer, (U16)read_length, (U8) * crc8);

        data_addr += read_length;
        data_length -= read_length;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
#if ((defined(AB155X)) || (defined(AM255X)))
    hal_dvfs_lock_control( DVFS_156M_SPEED, HAL_DVFS_UNLOCK);
#endif
#endif

    return ret;
}

/* buffer is used to read the data from the flash. This API may be called multiple times
  * Allocating buffer inside this API is ineffective.
  */
static RACE_ERRCODE race_storage_is_4K_block_erased(bool *is_erased,
        uint32_t start_address,
        uint8_t storage_type,
        uint8_t *buffer,
        uint32_t buffer_size)
{
    uint32_t i = 0, j = 0, count = 0;
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    if (start_address & 0xFFF || !is_erased || !buffer ||
            !buffer_size || 4096 % buffer_size) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    *is_erased = FALSE;

    count = 4096 / buffer_size;
    for (i = 0; i < count; i++) {
        ret = race_storage_read(start_address, buffer, buffer_size, storage_type);
        if (RACE_ERRCODE_SUCCESS != ret) {
            return ret;
        }

        for (j = 0; j < buffer_size; j++) {
            if (0xFF != buffer[j]) {
                return RACE_ERRCODE_SUCCESS;
            }
        }

        start_address += buffer_size;
    }

    *is_erased = TRUE;
    return RACE_ERRCODE_SUCCESS;
}


/* If erase_status is NULL and erase_status_size is not, return the size only.
  * If erase_status is not NULL, originally erase_status_size is the size of erase_status buffer.
  * And it will be updated to be the actual size needed.
  */
RACE_ERRCODE race_storage_get_partition_erase_status(uint8_t *erase_status,
        uint16_t *erase_status_size,
        uint32_t start_address,
        uint32_t length,
        uint8_t storage_type)
{
    uint32_t bit_size = 0, curr_bit_pos = 0;
    bool is_erased = FALSE;
    int32_t ret = RACE_ERRCODE_FAIL;
    uint16_t erase_status_size_old = 0;
    uint8_t *buffer = NULL;

    RACE_LOG_MSGID_I("erase_status:%x,erase_status_size:%x,start_address:%x,length:%x,storage_type:%d", 5,
                     erase_status,
                     erase_status_size,
                     start_address,
                     length,
                     storage_type);

    if (!erase_status_size ||
#ifndef RACE_ERASE_VERIFY_LEN
            start_address & 0xFFF)
#else
            start_address & 0xFFF || length & 0xFFF)
#endif
    {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    erase_status_size_old = *erase_status_size;

#ifndef RACE_ERASE_VERIFY_LEN
    bit_size = ((length >> 12) + ((length & 0xFFF) > 0 ? 1 : 0));
#else
    bit_size = length >> 12;
#endif
    *erase_status_size = ((bit_size >> 3) + (bit_size % 8 ? 1 : 0));

    RACE_LOG_MSGID_I("bit_size:%d, *erase_status_size:%d", 2, bit_size, *erase_status_size);
    if (!erase_status) {
        /* Return erase_status_size only. */
        return RACE_ERRCODE_SUCCESS;
    }

    RACE_LOG_MSGID_I("erase_status_size_old:%d, *erase_status_size:%d", 2,
                     erase_status_size_old,
                     *erase_status_size);
    if (erase_status_size_old < *erase_status_size) {
        return RACE_ERRCODE_FAIL;
    }

    memset(erase_status, 0, erase_status_size_old);

    if (!(*erase_status_size)) {
        /* length is 0. No need to continue. */
        return RACE_ERRCODE_SUCCESS;
    }

    curr_bit_pos = 0;

    buffer = race_mem_alloc(RACE_STORAGE_LONG_DATA_READ_MAX_SIZE);
    if (!buffer) {
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
#if ((defined(AB155X)) || (defined(AM255X)))
    ret = hal_dvfs_lock_control( DVFS_156M_SPEED, HAL_DVFS_LOCK);
    if (HAL_DVFS_STATUS_OK != ret) {
        race_mem_free(buffer);
        RACE_LOG_MSGID_E("Failed to lock alone cpu frequency.", 0);
        return RACE_ERRCODE_FAIL;
    }
#endif
#endif

    ret = RACE_ERRCODE_SUCCESS;
    while (curr_bit_pos < bit_size) {
        ret = race_storage_is_4K_block_erased(&is_erased,
                                              start_address,
                                              storage_type,
                                              buffer,
                                              RACE_STORAGE_LONG_DATA_READ_MAX_SIZE);
        if (RACE_ERRCODE_SUCCESS != ret) {
            break;
        }

        if (is_erased) {
            erase_status[curr_bit_pos / 8] |= (0x80 >> (curr_bit_pos % 8));
        }

        curr_bit_pos++;
        start_address += 0x1000;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
#if ((defined(AB155X)) || (defined(AM255X)))
    hal_dvfs_lock_control( DVFS_156M_SPEED, HAL_DVFS_UNLOCK);
#endif
#endif

    for (int k = 0; k < *erase_status_size; k++) {
        RACE_LOG_MSGID_I("erase_status[%d]:%x", 2, k, erase_status[k]);
    }

    race_mem_free(buffer);
    return ret;
}


#ifdef MBEDTLS_SHA256_C
RACE_ERRCODE race_storage_nb_sha256_generate_continue(race_storage_nb_sha256_generate_continue_msg_struct *continue_msg,
        unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE])
{
    race_general_msg_t msg = {0};
    uint8_t *buffer = NULL;
    uint32_t read_length = RACE_STORAGE_LONG_DATA_READ_MAX_SIZE, data_length = 0;
    uint32_t data_start_address = 0;
    int32_t ret = RACE_ERRCODE_FAIL;
    uint32_t local_time[2] = { 0, 0 };


    if (!continue_msg || !continue_msg->data_length || !sha256_generated ||
            continue_msg->data_length <= continue_msg->processed_data_length) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    /* Do not carry buffer in the message. Otherwise, if race task is busy, the buffer may not be released
         * in time and the heap may run out.
         */
    buffer = race_mem_alloc(RACE_STORAGE_LONG_DATA_READ_MAX_SIZE);
    if (!buffer) {
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    data_length = continue_msg->data_length - continue_msg->processed_data_length;
    data_start_address = continue_msg->data_start_address + continue_msg->processed_data_length;
    RACE_LOG_MSGID_I("[RACE Storage] Read starts, data_length = %d", 1, data_length);

#ifdef HAL_DVFS_MODULE_ENABLED
#if ((defined(AB155X)) || (defined(AM255X)))
    ret = hal_dvfs_lock_control( DVFS_156M_SPEED, HAL_DVFS_LOCK);
    if (HAL_DVFS_STATUS_OK != ret) {
        race_mem_free(buffer);
        RACE_LOG_MSGID_E("Failed to lock alone cpu frequency.", 0);
        return RACE_ERRCODE_FAIL;
    }
#endif
#endif

    /* Only process RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE at a time to avoid block race task. */
    if (RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE < data_length) {
        data_length = RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE;
    }

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &local_time[0]);
    ret = RACE_ERRCODE_SUCCESS;
    while (0 < data_length) {
        if (RACE_STORAGE_LONG_DATA_READ_MAX_SIZE > data_length) {
            read_length = data_length;
        }

        ret = race_storage_read(data_start_address,
                                buffer,
                                read_length,
                                continue_msg->storage_type);
        if (RACE_ERRCODE_SUCCESS != ret) {
            break;
        }

        mbedtls_sha256_update(&(continue_msg->ctx), buffer, read_length);
        data_start_address += read_length;
        data_length -= read_length;
        continue_msg->processed_data_length += read_length;
    }

#ifdef HAL_DVFS_MODULE_ENABLED
#if ((defined(AB155X)) || (defined(AM255X)))
    hal_dvfs_lock_control( DVFS_156M_SPEED, HAL_DVFS_UNLOCK);
#endif
#endif

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &local_time[1]);
    hal_gpt_get_duration_count(local_time[0], local_time[1], &local_time[0]);

    RACE_LOG_MSGID_I("[RACE Storage] Read ends, ret:%d, read & sha256 %d byte spent %d ms", 3, ret, RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE, local_time[0]/1000);
    race_mem_free(buffer);

    if (RACE_ERRCODE_SUCCESS == ret) {
        if (continue_msg->data_length <= continue_msg->processed_data_length) {
            /* All data has been processed. */
            memset(sha256_generated, 0, RACE_STORAGE_SHA256_SIZE);
            mbedtls_sha256_finish(&(continue_msg->ctx), sha256_generated);
            mbedtls_sha256_free(&(continue_msg->ctx));
            ret = RACE_ERRCODE_SUCCESS;
        } else {
            /* Send the message to continue to process the data. */
            msg.msg_id = MSG_ID_RACE_LOCAL_STORAGE_SHA256_GENERATE_CONTINUE_IND;
            msg.msg_data = (uint8_t *)continue_msg;

            ret = race_send_msg(&msg);
            if (RACE_ERRCODE_SUCCESS == ret) {
                ret = RACE_ERRCODE_MORE_OPERATION;
            }
        }
    }

    return ret;
}
#endif


RACE_ERRCODE race_storage_nb_sha256_generate_continue_msg_process(void *msg_data)
{
    int32_t ret = RACE_ERRCODE_FAIL;
#ifdef MBEDTLS_SHA256_C
    race_storage_nb_sha256_generate_continue_msg_struct *continue_msg = msg_data;
    unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE] = {0};

    ret = race_storage_nb_sha256_generate_continue(continue_msg, sha256_generated);
    if (RACE_ERRCODE_MORE_OPERATION == ret) {
        return ret;
    }

    if (continue_msg && continue_msg->callback) {
        continue_msg->callback(ret,
                               continue_msg->data_start_address,
                               continue_msg->data_length,
                               continue_msg->storage_type,
                               sha256_generated,
                               continue_msg->user_data);
    }

    DVFS_UNLOCK(s_sha256_data_size_non_blocking, RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE);
#endif

    if (msg_data) {
        race_mem_free(msg_data);
    }
    return ret;
}


/* Non-blocking API */
RACE_ERRCODE race_storage_nb_sha256_generate(uint32_t data_start_address,
        uint32_t data_length,
        uint8_t storage_type,
        unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE],
        race_storage_nb_sha256_generate_callback callback,
        void *user_data)
{
#ifdef MBEDTLS_SHA256_C
    race_storage_nb_sha256_generate_continue_msg_struct *continue_msg = NULL;
#endif
    RACE_ERRCODE ret = RACE_ERRCODE_FAIL;

    if (!callback || (0 != storage_type && 1 != storage_type)) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

#ifdef MBEDTLS_SHA256_C
    continue_msg = race_mem_alloc(sizeof(race_storage_nb_sha256_generate_continue_msg_struct));
    if (!continue_msg) {
        return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
    }

    continue_msg->data_start_address = data_start_address;
    continue_msg->data_length = data_length;
    continue_msg->storage_type = storage_type;
    continue_msg->processed_data_length = 0;
    continue_msg->callback = callback;
    continue_msg->user_data = user_data;

    DVFS_LOCK(s_sha256_data_size_non_blocking, data_length, RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE);

    mbedtls_sha256_init(&(continue_msg->ctx));
    mbedtls_sha256_starts(&(continue_msg->ctx), 0);

    ret = race_storage_nb_sha256_generate_continue(continue_msg, sha256_generated);
    if (RACE_ERRCODE_MORE_OPERATION != ret) {
        race_mem_free(continue_msg);
        DVFS_UNLOCK(s_sha256_data_size_non_blocking, RACE_STORAGE_LONG_DATA_PROCESS_MAX_SIZE);
    }
#else
    ret = RACE_ERRCODE_NOT_SUPPORT;
#endif

    return ret;
}
#endif /* RACE_STORAGE_CMD_ENABLE */

