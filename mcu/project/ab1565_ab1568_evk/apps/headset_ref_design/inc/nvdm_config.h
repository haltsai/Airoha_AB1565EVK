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
 * File: nvdm_config.h
 *
 * Description: This file contains various settings for the NVDM module.
 *
 */

#ifndef __NVDM_CONFIG_H__
#define __NVDM_CONFIG_H__

#include "nvdm_id_list.h"
#include "nvkey_id_list.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef MTK_NVDM_ENABLE

/* This macro defines max count of data items. */
#define NVDM_PORT_DAT_ITEM_COUNT (400)

/* This macro defines size of PEB, normally it is size of flash block. */
#define NVDM_PORT_PEB_SIZE (4096)

/* This macro defines max size of data item during all user defined data items.
 * 1. Must not define it greater than 2048 bytes.
 * 2. Define it as smaller as possible to enhance the utilization rate of NVDM region.
 * 2. Try your best to store small data less than 256 bytes.
 */
#define NVDM_PORT_MAX_DATA_ITEM_SIZE (1024)

/* This macro defines start address and PEB count of the NVDM region. */
#include "memory_map.h"
#define NVDM_PORT_REGION_ADDRESS    ROM_NVDM_BASE
#define NVDM_PORT_REGION_SIZE       ROM_NVDM_LENGTH

/* This macro defines max length of group name of data item. */
#define NVDM_PORT_GROUP_NAME_MAX_LENGTH (16)

/* This macro defines max length of data item name of data item. */
#define NVDM_PORT_DATA_ITEM_NAME_MAX_LENGTH (32)

#define FOTA_NVDM_ITEM_CLEAR_LIST \
{\
    {NVDM_GROUP_APP_DYNAMIC_FEATURE,    NVDM_APP_DYNAMIC_FEATURE_ITEM_AUTO_RHO},\
    {NVDM_GROUP_FACTORY_RESET,          NVDM_FACTORY_RESET_ITEM_FACTORY_RESET_FLAG},\
    {NVDM_GROUP_FOTA,                   NVDM_FOTA_ITEM_TRIGGER_MARK},\
    {NVDM_GROUP_FOTA,                   NVDM_FOTA_ITEM_VERSION},\
    {NVDM_GROUP_GSOUND_DATA,            NVDM_GSOUND_DATA_ITEM_DATA},\
    {NVDM_GROUP_SYSLOG,                 NVDM_SYSLOG_ITEM_MODULE_FILTER},\
};

#define FOTA_NVKEY_ITEM_CLEAR_LIST \
{\
    NVKEYID_BT_RELAY_ENABLE,\
    NVKEYID_BT_DUT_ENABLE,\
    NVKEYID_SYSTEM_MODE_SETTING,\
};

#ifdef MTK_FOTA_ENABLE

/**
 *  @brief This API is used to mark in NVDM when FOTA begins to run, and is called in FOTA side.
 */
void reserved_nvdm_item_list_ask_check(void);

/**
 *  @brief This API is used to check whether all non-reserved NVDM items should be deleted after FOTA done.
 *  @note  It should be called during system initial phase.
 */
void reserved_nvdm_item_list_check(void);

/**
 *  @brief This API is used to trigger user's customize modification for reserved item list in NVDM region after FOTA done.
 *  @note  The user should implement it when the reserved NVDM items is needed to be modified after FOTA.
 */
void reserved_nvdm_item_list_modify(void);

#endif

#endif

#ifdef __cplusplus
}
#endif

#endif /* __NVDM_CONFIG_H__ */
