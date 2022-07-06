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

#ifndef __BT_DEVICE_MANAGER_DB_H__
#define __BT_DEVICE_MANAGER_DB_H__

#include "bt_type.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_config.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

typedef struct {
    bt_bd_addr_t local_address;
#ifdef MTK_BT_DEVICE_MANAGER_DB_EXTENSION
    uint8_t      reserved[64];
#endif
} bt_device_manager_db_local_info_t;

// restructure
typedef enum {
    BT_DEVICE_MANAGER_DB_TYPE_LOCAL_INFO,
    BT_DEVICE_MANAGER_DB_TYPE_REMOTE_SEQUENCE_INFO,
    BT_DEVICE_MANAGER_DB_TYPE_REMOTE_DEVICE0_INFO,
    BT_DEVICE_MANAGER_DB_TYPE_REMOTE_DEVICE_MAX = BT_DEVICE_MANAGER_MAX_PAIR_NUM + 1,
#ifdef MTK_AWS_MCE_ENABLE
    BT_DEVICE_MANAGER_DB_TYPE_AWS_LOCAL_INFO,
#endif

    BT_DEVICE_MANAGER_DB_TYPE_MAX
} bt_device_manager_db_type_t;

#define BT_DEVICE_MANAGER_DB_STORAGE_TYPE_NVDM      (0x00)
#define BT_DEVICE_MANAGER_DB_STORAGE_TYPE_NVKEY     (0x01)
typedef uint8_t bt_device_manager_db_storage_type_t;

typedef struct {
    bool auto_gen;
    bt_device_manager_db_storage_type_t storage_type;
    union {
        uint32_t nvkey_id;
        struct {
            char *nvdm_group_str;
            char *nvdm_item_str;
        };
    };
} bt_device_manager_db_storage_t;

void bt_device_manager_db_init(bt_device_manager_db_type_t db_type,
    bt_device_manager_db_storage_t *storage, void *db_buffer, uint32_t buffer_size);
void bt_device_manager_db_open(bt_device_manager_db_type_t db_type);
void bt_device_manager_db_close(bt_device_manager_db_type_t db_type);
void bt_device_manager_db_update(bt_device_manager_db_type_t db_type);
void bt_device_manager_db_flush(bt_device_manager_db_type_t db_type, bt_device_manager_db_flush_t block);
void bt_device_manager_db_mutex_take();
void bt_device_manager_db_mutex_give();

#ifdef __cplusplus
}
#endif

#endif /* __BT_DEVICE_MANAGER_DB_H__ */

