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
 
#ifndef __RACE_STORAGE_UTIL_H__
#define __RACE_STORAGE_UTIL_H__

#include "race_cmd_feature.h"
#ifdef RACE_STORAGE_CMD_ENABLE
#include "race_cmd.h"
#include "race_xport.h"


#define RACE_STORAGE_SHA256_SIZE    (32)
#define RACE_STORAGE_SIGNATURE_MAX_SIZE    (RACE_STORAGE_SHA256_SIZE)


typedef enum
{
    RACE_STORAGE_BLOCK_TYPE_4K,
    RACE_STORAGE_BLOCK_TYPE_32K,
    RACE_STORAGE_BLOCK_TYPE_64K,
    
    RACE_STORAGE_BLOCK_TYPE_MAX
}race_storage_block_type_enum;


typedef RACE_ERRCODE (*race_erase_partition_done_callback)(uint8_t status, \
                                                        uint8_t storage_type, \
                                                        uint32_t partition_length, \
                                                        uint32_t partition_address, \
                                                        uint8_t app_id, \
                                                        uint8_t channel_id, \
                                                        bool noti_delay, \
                                                        void *user_data);

typedef RACE_ERRCODE (*race_storage_nb_sha256_generate_callback)(uint8_t status, \
                                                                      uint32_t data_start_address, \
                                                                      uint32_t data_length, \
                                                                      uint8_t storage_type, \
                                                                      unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE], \
                                                                      void *user_data);

typedef struct
{
    uint8_t storage_type;
    uint32_t partition_length;
    uint32_t partition_address;
    uint32_t block_num;     /* Reduced after each erase operation */
    uint32_t address;       /* Equal to flash_addr at the beginning and reduced after each erase operation */
    uint8_t app_id;
    uint8_t channel_id;
    bool noti_delay;
    race_erase_partition_done_callback callback;
    void *user_data;
}race_storage_erase_partition_continue_msg_struct;

#ifdef MBEDTLS_SHA256_C
typedef struct
{
    mbedtls_sha256_context ctx;
    uint32_t data_start_address;          /* original data start address */
    uint32_t data_length;                 /* original data length */
    uint8_t storage_type;
    uint32_t processed_data_length;       /* processed data length */
    race_storage_nb_sha256_generate_callback callback;
    void *user_data;
}race_storage_nb_sha256_generate_continue_msg_struct;
#endif


RACE_ERRCODE race_storage_write(uint32_t start_address,
                                    uint8_t *buffer,
                                    uint32_t length,
                                    uint8_t storage_type);

RACE_ERRCODE race_storage_read(uint32_t start_address,
                                    uint8_t *buffer,
                                    uint32_t length,
                                    uint8_t storage_type);

RACE_ERRCODE race_storage_erase_partition(uint8_t storage_type,
                                                 uint32_t partition_length,
                                                 uint32_t partition_address,
                                                 uint8_t app_id,
                                                 uint8_t channel_id,
                                                 bool noti_delay,
                                                 race_erase_partition_done_callback callback,
                                                 void *user_data);

RACE_ERRCODE race_storage_erase_partition_continue_msg_process(race_general_msg_t *msg);

RACE_ERRCODE race_storage_sha256_generate(unsigned char sha256[RACE_STORAGE_SHA256_SIZE],
                                                   uint32_t data_addr,
                                                   uint32_t data_length,
                                                   uint8_t storage_type);

RACE_ERRCODE race_storage_get_partition_erase_status(uint8_t *erase_status,
                                                               uint16_t *erase_status_size,
                                                               uint32_t start_address,
                                                               uint32_t length,
                                                               uint8_t storage_type);

RACE_ERRCODE race_storage_nb_sha256_generate_continue_msg_process(void *msg_data);

RACE_ERRCODE race_storage_nb_sha256_generate(uint32_t data_start_address,
                                                       uint32_t data_length,
                                                       uint8_t storage_type,
                                                       unsigned char sha256_generated[RACE_STORAGE_SHA256_SIZE],
                                                       race_storage_nb_sha256_generate_callback callback,
                                                       void *user_data);

RACE_ERRCODE race_storage_crc8_generate(uint8_t *crc8,
                                                uint32_t data_addr,
                                                uint32_t data_length,
                                                uint8_t storage_type);


#endif /* RACE_STORAGE_CMD_ENABLE */
#endif /* __RACE_STORAGE_UTIL_H__ */

