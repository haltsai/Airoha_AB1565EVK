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

#ifndef _BL_FOTA_DEF_H_
#define _BL_FOTA_DEF_H_

/*
 * AIROHA AB1530 FOTA upgrade loader firmware
 * COPYRIGHT (C) 2018 AIROHA TECHNOLOGY CORP. CO., LTD. ALL RIGHTS RESERVED
 */

/*!
 *@file   fota_def.h
 *@brief  define
 *        1. FOTA error code
 *        2. flash read back check option
 */

#include <stdbool.h>

typedef unsigned int          U32;
typedef unsigned short int    U16;
typedef unsigned char         U8;
typedef bool BOOL;
typedef void VOID;

#define FLASH_PAGE_SIZE    (256)
#define FLASH_SUCCESS      (0)

#define PACKED  __attribute__((packed))


typedef U8 DFU_ERRCODE;
#define DFU_ERR_SUCCESS                     0x00
#define DFU_ERR_0x01_0x0F_ARE_RSVD          "reference to the following error code defined by the Ex/internal flash driver"
#define DFU_ERR_INVALID_FOTA_ADDR           0xA0
#define DFU_ERR_FOTA_INIT_FAIL              0xA1
#define DFU_ERR_MOVE_DATA_PAGE_WRITE_FAIL   0xA2
#define DFU_ERR_MOVE_DATA_ERASE_FAIL        0xA3
#define DFU_ERR_AES_DECRYPT_FAIL            0xA4
#define DFU_ERR_SHA_MATCH_FAIL              0xA5
#define DFU_ERR_AES_IV_NOT_MATCH            0xA6


#define DFU_ERR_FLASH_CTRL_NVRAM_PAGE_READ_FAIL     0xB0
#define DFU_ERR_FLASH_CTRL_NVRAM_PAGE_WRITE_FAIL    0xB1
#define DFU_ERR_FLASH_CTRL_VERIFY_ERASE_FAIL        0xB2
#define DFU_ERR_FLASH_CTRL_VERIFY_DATA_FAIL         0xB3
#define DFU_ERR_FLASH_CTRL_4K_ERASE_FAIL            0xB4

#define DFU_ERR_FAIL    0xFF

#define DFU_ERR_INTEGRITY_NOT_CHECKED   0xC0

/* define flash type */
#ifdef MTK_FOTA_STORE_IN_EXTERNAL_FLASH
#define FOTA_FLASH_TYPE (FALSE)
#else
#define FOTA_FLASH_TYPE (TRUE)
#endif

typedef enum
{
    None = 0,
    Lzma = 1,
    Lzma_Aes = 2
} CompressionType;

typedef enum
{
    FOTA_STATE_LOADER_START_DATA_MOVING    = 0x0100,
    FOTA_STATE_LOADER_COMPLETE_DATA_MOVING = 0x0101,
    FOTA_STATE_LOADER_DATA_MOVING_FAIL     = 0x0102,
    FOTA_STATE_LOADER_ERASE_PACKAGE        = 0x0103,
    FOTA_STATE_LOADER_PACKAGE_NG           = 0x0104,
} FotaState;

typedef struct stru_fota_mover_triplet
{
    U32 source_address;                 /* It's the offset based on the start address of FOTA package actually. */
    U32 length;                         /* It's the length of decompression data. */
    U32 destination_address;
} PACKED FOTA_MOVER_TRIPLET;

#define FOTA_BL_FLASH_ADDRESS        0x08012000
#define FOTA_BL_PHYSICAL_ADDRESS     0x00012000
#define FOTA_BL_SYNCWORD_FLASH_ADDRESS     0x08012C00
#define FOTA_BL_SYNCWORD_PHYSICAL_ADDRESS  0x00012C00

#endif // _BL_FOTA_DEF_H_

