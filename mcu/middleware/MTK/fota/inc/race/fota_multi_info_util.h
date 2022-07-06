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
 * File: fota_multi_info_util.h
 *
 * Description: This file defines the multiple information needed macros and types and the interfaces of fota_multi_info_util.c.
 *
 * Note: See doc/Airoha_IoT_SDK_Firmware_Update_Developers_Guide.pdf for more detail.
 *
 */


#ifndef __FOTA_MULTI_INFO_UTIL_H__
#define __FOTA_MULTI_INFO_UTIL_H__

#include "fota_util.h"


#define FOTA_MULTI_INFO_STATE_RECORD_LEN               (2) /* The length of the FOTA download state. */
#define FOTA_MULTI_INFO_UPGRADE_FLAG_RECORD_LEN        (2) /* The length of the FOTA upgrade flag. */
#define FOTA_MULTI_INFO_DL_INTEGRITY_RES_RECORD_LEN    (2) /* The length of the integrity check result of the downloaded FOTA package. */
#define FOTA_MULTI_INFO_VERSION_RECORD_LEN             (FOTA_VERSION_MAX_SIZE + FOTA_VERSION_INTEGRITY_CHECK_VALUE_SIZE) /* The length of the FOTA package release version. */
#define FOTA_MULTI_INFO_NVDM_INCOMPATIBLE_FLAG_RECORD_LEN (2) /* The length of the NVDM incompatible flag. */
#define FOTA_MULTI_INFO_RECORD_MAX_LEN    (FOTA_MULTI_INFO_VERSION_RECORD_LEN) /* The maximum length of the records stored in the last 4K of the FOTA partition. */
#define FOTA_MULTI_INFO_RECORD_LEN_LCM    (FOTA_MULTI_INFO_VERSION_RECORD_LEN) /* The least common multiple of the lengths of all records. */


/**
 * @brief The types of the FOTA multiple information.
 */
typedef enum {
    FOTA_MULTI_INFO_TYPE_NONE = 0x00, /**< NONE type for initialization. */

    FOTA_MULTI_INFO_TYPE_STATE = 0x01, /**< The type for the FOTA download state. */
    FOTA_MULTI_INFO_TYPE_UPGRADE_FLAG = 0x02, /**< The type for the FOTA upgrade flag. */
    FOTA_MULTI_INFO_TYPE_DL_INTEGRITY_RES = 0x04, /**< The type for the integrity check result of the downloaded FOTA package. */
    FOTA_MULTI_INFO_TYPE_VERSION = 0x08, /**< The type for the FOTA package release version. */
    FOTA_MULTI_INFO_TYPE_NVDM_INCOMPATIBLE_FLAG = 0x10, /**< The type for the NVDM incompatible flag. */

    /* Add a new type before this line and update the value of the NULL type if needed. */
    FOTA_MULTI_INFO_TYPE_NULL = 0x20, /**< The NULL type. It is an invalid type and a new type must be added before it. */

    FOTA_MULTI_INFO_TYPE_ALL = 0xFFFF /**< The maximum value of this enum. */
} fota_multi_info_type_enum;


/**
 * @brief This function initializes the multiple information sector which is the last 4K of the FOTA partition. Multiple information will be stored
 * in this sector such as the FOTA download state, the FOTA upgrade flag and so on.
 */
void fota_multi_info_sector_init(void);

/**
 * @brief This function erases the multiple information sector and writes the last valid records back for all multiple information types.
 * @return FOTA_ERRCODE_SUCCESS, succeed. Other values, fail.
 */
FOTA_ERRCODE fota_multi_info_sector_reset(void);

/**
 * @brief This function erases the multiple information sector and writes the last valid records back for all multiple information types
 * except the ones indicated in not_wb_info_types.
 * @param[in] not_wb_info_types The multiple information types that do not want to write their last valid records back.
 * @param[out] erase_done A boolean to indicate if the sector is erased or not. Event if the return value is not
 * FOTA_ERRCODE_SUCCESS, it is valid.
 * @return FOTA_ERRCODE_SUCCESS, succeed. Other values, fail.
 */
FOTA_ERRCODE fota_multi_info_sector_clean(uint16_t not_wb_info_types, bool *erase_done);


/**
 * @brief This function reads the data of a certain multiple information record.
 * @param[in] info_type The multiple information type.
 * @param[out] record_data The data of the multiple information record.
 * @param[in] record_len The length of the multiple information record.
 * @return FOTA_ERRCODE_SUCCESS, succeed. Other values, fail.
 */
FOTA_ERRCODE fota_multi_info_read(fota_multi_info_type_enum info_type,
                                  uint8_t *record_data,
                                  uint8_t record_len);


FOTA_ERRCODE fota_multi_info_write(fota_multi_info_type_enum info_type,
                                   uint8_t *record_data,
                                   uint8_t record_len);

#endif /* __FOTA_MULTI_INFO_UTIL_H__ */

