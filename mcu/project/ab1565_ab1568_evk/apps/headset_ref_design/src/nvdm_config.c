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
 * File: nvdm_config.c
 *
 * Description: This file implements the functions defined in the nvdm_config.h file.
 * Their function is to perform some trimming of the data items in NVDM for the first boot after FOTA.
 *
 */

#if defined(MTK_NVDM_ENABLE) && defined(MTK_FOTA_VIA_RACE_CMD)

#include "nvdm.h"
#include "nvdm_config.h"
#include "fota.h"
#include "syslog.h"
#include <string.h>

#include "nvdm_id_list.h"

#ifndef __weak
#define __weak   __attribute__((weak))
#endif


#define FOTA_MARK_ONGOING 0x55
#define FOTA_MARK_DONE    0xAA

/**
 *  @brief This structure defines enough information for NVDM to find the specified entry.
 */
typedef struct {
    char *group_name;     /**< The group name of the data item, such as "Bluetooth". */
    char *item_name;      /**< The name of the data item, such as "address" of the "Bluetooth". */
} fota_reserved_nvdm_item_t;

/* In this array, the NVDM data items that need to be deleted specified by the user are saved. */
static const fota_reserved_nvdm_item_t g_fota_clear_nvdm_items[] = FOTA_NVDM_ITEM_CLEAR_LIST;
static const uint16_t g_fota_clear_nvkey_items[] = FOTA_NVKEY_ITEM_CLEAR_LIST;


/**
 * @brief    Users can add the processing logic for the specified NVDM data items in this function.
 * @note     This function has the weak attribute. If you define other functions with the same name,
 *           this function will be masked.
 */
__weak void reserved_nvdm_item_list_modify(void)
{}


/**
 * @brief    Write a flag so that some unnecessary NVDM data items can be deleted
 *           at the first boot after FOTA.
 */
void reserved_nvdm_item_list_ask_check(void)
{
    uint8_t fota_mark;
    nvdm_status_t status;

    LOG_MSGID_W(common, "Write FOTA mark to NVDM", 0);

    fota_mark = FOTA_MARK_ONGOING;
    status = nvdm_write_data_item(NVDM_GROUP_FOTA, NVDM_FOTA_ITEM_TRIGGER_MARK,
                                  NVDM_DATA_ITEM_TYPE_RAW_DATA, &fota_mark, 1);
    if (status != NVDM_STATUS_OK) {
        LOG_MSGID_E(common, "Write FOTA mark fail %d", 1, status);
    }
}

/**
 *  @brief This API is used to check whether all non-reserved NVDM items should be deleted after FOTA done.
 *  @note  It should be called during system initial phase.
 */
void reserved_nvdm_item_list_check(void)
{
    uint32_t i, size;
    uint8_t fota_mark;
    nvdm_status_t status;
    char item_name[5];
    char group_name[64];
    char data_item_name[64];
    bool fota_upgrade_result = FALSE;

    LOG_MSGID_I(common, "Check the FOTA status", 0);

    fota_get_upgrade_result(&fota_upgrade_result);

    size = sizeof(fota_mark);
    status = nvdm_read_data_item(NVDM_GROUP_FOTA, NVDM_FOTA_ITEM_TRIGGER_MARK, &fota_mark, &size);

    /* If the FOTA upgrade is successful, and the flag previously written to NVDM is the correct value. */
    if ((status == NVDM_STATUS_OK) && fota_upgrade_result && (fota_mark == FOTA_MARK_ONGOING)) {
        LOG_MSGID_I(common, "Detect FOTA update, re-organize the NVDM region", 0);

        /* Since this is only required for the first boot after FOTA, this item needs to be deleted. */
        status = nvdm_delete_data_item(NVDM_GROUP_FOTA, NVDM_FOTA_ITEM_TRIGGER_MARK);
        if (status != NVDM_STATUS_OK) {
            LOG_MSGID_E(common, "Delete FOTA mark fail: %d", 1, status);
            return;
        }

        /* Clear nvdm item. */
        for (i = 0; i < (sizeof(g_fota_clear_nvdm_items) / sizeof(fota_reserved_nvdm_item_t)); i++) {
            status = nvdm_delete_data_item(g_fota_clear_nvdm_items[i].group_name, g_fota_clear_nvdm_items[i].item_name);
            if (status != NVDM_STATUS_OK) {
                LOG_MSGID_E(common, "nvdm_delete_data_item fail: group_name(%s), data_item_name(%s), %d", 3,
                    g_fota_clear_nvdm_items[i].group_name, g_fota_clear_nvdm_items[i].item_name, status);
            }
            LOG_MSGID_I(common, "delete item: group_name = %s, data_item_name = %s", 2,
                g_fota_clear_nvdm_items[i].group_name, g_fota_clear_nvdm_items[i].item_name);
        }

        /* Clear nvkey item. */
        for (i = 0; i < (sizeof(g_fota_clear_nvkey_items) / sizeof(uint16_t)); i++) {
            snprintf(item_name, sizeof(item_name), "%X", g_fota_clear_nvkey_items[i]);
            status = nvdm_delete_data_item("AB15", item_name);
            if (status != NVDM_STATUS_OK) {
                LOG_MSGID_E(common, "nvdm_delete_data_item fail: group_name(AB15), data_item_name(%s), %d", 2, item_name, status);
            }
            LOG_MSGID_I(common, "delete item: group_name = AB15, data_item_name = %s", 1, item_name);
        }

        if (fota_nvdm_is_incompatible_flag_set() == true) {
            LOG_MSGID_I(common, "Trigger user's customize modification for reserved item list in NVDM region after FOTA", 0);
            reserved_nvdm_item_list_modify();
            fota_nvdm_clear_incompatible_flag();
        }

        /* Dump group name and item name of all items stored in NVDM. */
        LOG_MSGID_I(common, "dump reserved item list in NVDM region after FOTA", 0);
        status = nvdm_query_begin();
        if (status != NVDM_STATUS_OK) {
            LOG_MSGID_E(common, "nvdm_query_begin fail %d", 1, status);
            return;
        }
        while (nvdm_query_next_group_name(group_name) == NVDM_STATUS_OK) {
            while (nvdm_query_next_data_item_name(data_item_name) == NVDM_STATUS_OK) {
                LOG_I(common, "group_name = %s, data_item_name = %s", group_name, data_item_name);
            }
        }
        status = nvdm_query_end();
        if (status != NVDM_STATUS_OK) {
            LOG_MSGID_E(common, "nvdm_query_end fail %d", 1, status);
            return;
        }
    }
}

#endif

