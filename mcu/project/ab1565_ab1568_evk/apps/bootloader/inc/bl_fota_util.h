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


#ifndef _BL_FOTA_UTIL_H_
#define _BL_FOTA_UTIL_H_

/*
 * AIROHA AB1530 FOTA upgrade loader firmware
 * COPYRIGHT (C) 2018 AIROHA TECHNOLOGY CORP. CO., LTD. ALL RIGHTS RESERVED
 */

/*!
 *@file   bl_fota_util.h
 *@brief  define
 *
 *
 */

#include "bl_fota_def.h"
#include "fota_util.h"
#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#if defined(MBEDTLS_AES_C) && defined(MBEDTLS_CIPHER_MODE_CBC)
#include "mbedtls/aes.h"
#endif

#define FOTA_UPGRADE_AES_SUPPORT

#ifdef _BL_FOTA_UTIL_C_
#define EXT
#else
#define EXT extern
#endif

typedef struct
{
 U32 number_of_movers;
 //FOTA_MOVER_TRIPLET movers[0];
} PACKED FOTA_MOVER_INFO;

EXT U32 g_fota_partition_start_address; // FOTA start address
EXT U32 g_fota_partition_length;        // FOTA length


EXT U32 g_fota_data_start_address;
EXT U32 g_fota_data_length;
EXT U32 g_number_of_movers;
EXT U32 g_sha_info_start_address;
EXT bool g_is_sha_info_found;
EXT U8  version[FOTA_VERSION_MAX_SIZE]; // fota version string
EXT U32 versionLength;                  // fota version length



EXT FOTA_MOVER_TRIPLET g_movers[319]; // 15 * 256 / 12 ~= 320 - 1(basic info.)
EXT CompressionType g_compression_type;

EXT U32 g_fota_data_4k_target_table[2048];  // Up to 2048 * 4K = 8MB
EXT U32 g_fota_data_4k_target_table_size;
EXT VOID bl_gen_each_4k_target_addr_table();
EXT bool bl_fota_is_mover_info_valid(void);

#ifdef  FOTA_SUPPORT_MEMORY_LAYOUT_MODIFICATION
EXT U8 is_fota_partition_upgrade;
#endif


EXT BOOL bl_fota_init();
#endif // _BL_FOTA_UTIL_H_
