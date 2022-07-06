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

#ifdef MTK_NVDM_ENABLE
#include "nvkey.h"
#include "nvkey_id_list.h"
#endif /* MTK_NVDM_ENABLE */
#include "bt_type.h"
#include "bt_aws_mce.h"
#include "bt_device_manager_internal.h"
#include "bt_device_manager_db.h"
#include "bt_connection_manager_utils.h"
#include "bt_platform.h"

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

typedef struct {
    uint8_t         aws_role;
    uint16_t        supported_feature;
    bt_bd_addr_t    peer_addr;
    uint8_t         slave_retry_count;
    bt_bd_addr_t    local_aws_addr;
    uint8_t         ls_enable;
    uint8_t         aws_key[16];
    uint8_t         speaker_fixed_addr[6];
    uint8_t         aws_mode;
    uint8_t         reserved[8];
} PACKED bt_device_manager_db_aws_local_info_t;

typedef struct {
    bt_bd_addr_t sandbox_local_addr;
    bt_bd_addr_t sandbox_peer_addr;
    uint8_t aws_key[16];
    bt_aws_mce_role_t sandbox_local_role;
    uint8_t sandbox_aws_mode;
} bt_device_manager_db_aws_sandbox_env_t;

static bt_device_manager_db_aws_sandbox_env_t bt_dm_aws_sandbox_cnt;
static bt_device_manager_db_aws_local_info_t  bt_dm_aws_cnt;

bt_bd_addr_t* bt_device_manager_aws_local_info_get_speaker_fixed_address(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get speaker fixed address", 0);
    bt_device_manager_dump_bt_address(
        BT_DEVICE_MANAGER_LOCAL_INFO_ADDRESS_AWS_LOCAL, (uint8_t *)&(bt_dm_aws_cnt.speaker_fixed_addr));
    return &(bt_dm_aws_cnt.speaker_fixed_addr);
}

void          bt_device_manager_aws_local_info_store_speaker_fixed_address(bt_bd_addr_t *addr)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Set speaker fixed address", 0);
    bt_device_manager_dump_bt_address(
        BT_DEVICE_MANAGER_LOCAL_INFO_ADDRESS_AWS_LOCAL, (uint8_t *)addr);
    memcpy(&(bt_dm_aws_cnt.speaker_fixed_addr), addr, sizeof(bt_bd_addr_t));
    bt_device_manager_db_flush(BT_DEVICE_MANAGER_DB_TYPE_AWS_LOCAL_INFO, BT_DEVICE_MANAGER_DB_FLUSH_BLOCK);
}

bt_bd_addr_t* bt_device_manager_aws_local_info_get_fixed_address(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get fixed address:", 0);
    return bt_device_manager_get_local_address_internal();
}

void bt_device_manager_aws_local_info_store_local_address(bt_bd_addr_t *addr)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Store local address:", 0);
    bt_device_manager_dump_bt_address(
        BT_DEVICE_MANAGER_LOCAL_INFO_ADDRESS_AWS_LOCAL, (uint8_t *)addr);
    memcpy(&(bt_dm_aws_sandbox_cnt.sandbox_local_addr), addr, sizeof(bt_bd_addr_t));
}

bt_bd_addr_t* bt_device_manager_aws_local_info_get_local_address(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get local address:", 0);
    bt_device_manager_dump_bt_address(
        BT_DEVICE_MANAGER_LOCAL_INFO_ADDRESS_AWS_LOCAL, (uint8_t *)&(bt_dm_aws_sandbox_cnt.sandbox_local_addr));
    return &(bt_dm_aws_sandbox_cnt.sandbox_local_addr);
}

void bt_device_manager_aws_local_info_store_peer_address(bt_bd_addr_t *addr)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Store peer address:", 0);
    bt_device_manager_dump_bt_address(
        BT_DEVICE_MANAGER_LOCAL_INFO_ADDRESS_AWS_PEER, (uint8_t *)addr);
    memcpy(&(bt_dm_aws_sandbox_cnt.sandbox_peer_addr), addr, sizeof(bt_bd_addr_t));
}

bt_bd_addr_t* bt_device_manager_aws_local_info_get_peer_address(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get peer address:", 0);
    bt_device_manager_dump_bt_address(
        BT_DEVICE_MANAGER_LOCAL_INFO_ADDRESS_AWS_PEER, (uint8_t *)&(bt_dm_aws_sandbox_cnt.sandbox_peer_addr));
    return &(bt_dm_aws_sandbox_cnt.sandbox_peer_addr);
}

void bt_device_manager_aws_local_info_store_key(bt_key_t* aws_key)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Store aws key:", 0);
    bt_device_manager_dump_link_key((uint8_t *)aws_key);
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Current aws key:", 0);
    bt_device_manager_dump_link_key((uint8_t *)bt_dm_aws_sandbox_cnt.aws_key);
    if (memcmp(&bt_dm_aws_sandbox_cnt.aws_key, aws_key, sizeof(bt_key_t)) != 0) {
        memcpy(&bt_dm_aws_sandbox_cnt.aws_key, aws_key, sizeof(bt_key_t));
    }
}

bt_key_t* bt_device_manager_aws_local_info_get_key(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get current aws key:", 0);
    bt_device_manager_dump_link_key((uint8_t *)&bt_dm_aws_cnt.aws_key);
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get asndbox aws key:", 0);
    bt_device_manager_dump_link_key((uint8_t *)&bt_dm_aws_sandbox_cnt.aws_key);
    return &(bt_dm_aws_sandbox_cnt.aws_key);
}

void bt_device_manager_aws_local_info_store_role(bt_aws_mce_role_t aws_role)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Set aws role:0x%x, current aws role:0x%x, sandbox aws role:0x%x",
        3, aws_role, bt_dm_aws_cnt.aws_role, bt_dm_aws_sandbox_cnt.sandbox_local_role);
    bt_dm_aws_sandbox_cnt.sandbox_local_role = aws_role;
}

bt_aws_mce_role_t bt_device_manager_aws_local_info_get_role(void)
{
    /*bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get current aws role:0x%x, sandbox aws role:0x%x",
        2, bt_dm_aws_cnt.aws_role, bt_dm_aws_sandbox_cnt.sandbox_local_role);*/
    return bt_dm_aws_sandbox_cnt.sandbox_local_role;
}

void bt_device_manager_aws_local_info_store_ls_enable(uint8_t ls_enable)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Store ls enable:0x%x", 1, ls_enable);
    if (bt_dm_aws_cnt.ls_enable != ls_enable) {
        bt_device_manager_db_mutex_take();
        bt_dm_aws_cnt.ls_enable = ls_enable;
        bt_device_manager_db_mutex_give();
        bt_device_manager_db_update(BT_DEVICE_MANAGER_DB_TYPE_AWS_LOCAL_INFO);
    }
}

uint8_t bt_device_manager_aws_local_info_get_ls_enable(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get ls enable:0x%x", 1, bt_dm_aws_cnt.ls_enable);
    return bt_dm_aws_cnt.ls_enable;
}

bt_aws_mce_role_t bt_device_manager_aws_local_info_get_real_role(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get real role 0x%02X", 1, bt_dm_aws_cnt.aws_role);
    return bt_dm_aws_cnt.aws_role;
}

void bt_device_manager_aws_local_info_update(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Update local info from sandbox", 0);
    bool dirty_flag = false;
    bt_device_manager_db_mutex_take();
    if (memcmp(&(bt_dm_aws_cnt.local_aws_addr), &(bt_dm_aws_sandbox_cnt.sandbox_local_addr), sizeof(bt_bd_addr_t))) {
        memcpy(&(bt_dm_aws_cnt.local_aws_addr), &(bt_dm_aws_sandbox_cnt.sandbox_local_addr), sizeof(bt_bd_addr_t));
        dirty_flag = true;
    }
    if (memcmp(&(bt_dm_aws_cnt.peer_addr), &(bt_dm_aws_sandbox_cnt.sandbox_peer_addr), sizeof(bt_bd_addr_t))) {
        memcpy(&(bt_dm_aws_cnt.peer_addr), &(bt_dm_aws_sandbox_cnt.sandbox_peer_addr), sizeof(bt_bd_addr_t));
        dirty_flag = true;
    }
    if (bt_dm_aws_cnt.aws_role != bt_dm_aws_sandbox_cnt.sandbox_local_role) {
        bt_dm_aws_cnt.aws_role = bt_dm_aws_sandbox_cnt.sandbox_local_role;
        dirty_flag = true;
    }
    if (memcmp(&(bt_dm_aws_cnt.aws_key), &(bt_dm_aws_sandbox_cnt.aws_key), sizeof(bt_key_t))) {
        memcpy(&(bt_dm_aws_cnt.aws_key), &(bt_dm_aws_sandbox_cnt.aws_key), sizeof(bt_key_t));
        dirty_flag = true;
    }
    if (bt_dm_aws_cnt.aws_mode != bt_dm_aws_sandbox_cnt.sandbox_aws_mode) {
        bt_dm_aws_cnt.aws_mode = bt_dm_aws_sandbox_cnt.sandbox_aws_mode;
        dirty_flag = true;
    }
    bt_device_manager_db_mutex_give();
    if (true == dirty_flag) {
        bt_device_manager_db_update(BT_DEVICE_MANAGER_DB_TYPE_AWS_LOCAL_INFO);
        bt_device_manager_db_flush(BT_DEVICE_MANAGER_DB_TYPE_AWS_LOCAL_INFO, BT_DEVICE_MANAGER_DB_FLUSH_BLOCK);
    }
}

void bt_device_manager_aws_reset(void)
{
    bt_bd_addr_t invalid_addr = {0};
    bt_key_t invalid_key = {0};
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Aws reset", 0);
    bt_bd_addr_t *fixed_addr = bt_device_manager_aws_local_info_get_fixed_address();
    bt_device_manager_aws_local_info_store_local_address(fixed_addr);
    bt_device_manager_aws_local_info_store_peer_address(&invalid_addr);
    bt_device_manager_aws_local_info_store_key(&invalid_key);
    bt_device_manager_aws_local_info_store_role(BT_AWS_MCE_ROLE_AGENT);
    bt_device_manager_aws_local_info_update();
}

void bt_device_manager_aws_local_info_store_mode(uint8_t aws_mode)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Set aws mode:0x%x, current aws mode:0x%x, sandbox aws mode:0x%x",
        3, aws_mode, bt_dm_aws_cnt.aws_mode, bt_dm_aws_sandbox_cnt.sandbox_aws_mode);
    bt_dm_aws_sandbox_cnt.sandbox_aws_mode = aws_mode;
}

uint8_t bt_device_manager_aws_local_info_get_mode(void)
{
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] Get current aws mode:0x%x, sandbox aws mode:0x%x",
        2, bt_dm_aws_cnt.aws_mode, bt_dm_aws_sandbox_cnt.sandbox_aws_mode);
    return bt_dm_aws_sandbox_cnt.sandbox_aws_mode;
}

void bt_device_manager_aws_local_info_init(void)
{
    bt_device_manager_db_storage_t aws_storage = {
        .auto_gen = true,
        .storage_type = BT_DEVICE_MANAGER_DB_STORAGE_TYPE_NVKEY,
        .nvkey_id = NVKEYID_PROFILE_AWS_MCE_CONFIG
    };
    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] AWS local info init", 0);

    memset((void*)&bt_dm_aws_cnt, 0, sizeof(bt_dm_aws_cnt));
    bt_device_manager_db_init(BT_DEVICE_MANAGER_DB_TYPE_AWS_LOCAL_INFO,
        &aws_storage, &bt_dm_aws_cnt, sizeof(bt_dm_aws_cnt));
    memcpy(&(bt_dm_aws_sandbox_cnt.sandbox_local_addr), &bt_dm_aws_cnt.local_aws_addr, sizeof(bt_bd_addr_t));
    memcpy(&(bt_dm_aws_sandbox_cnt.sandbox_peer_addr), &bt_dm_aws_cnt.peer_addr, sizeof(bt_bd_addr_t));
    memcpy(&(bt_dm_aws_sandbox_cnt.aws_key), &bt_dm_aws_cnt.aws_key, sizeof(bt_key_t));
    bt_dm_aws_sandbox_cnt.sandbox_local_role = bt_dm_aws_cnt.aws_role;
    bt_dm_aws_sandbox_cnt.sandbox_aws_mode = bt_dm_aws_cnt.aws_mode;

    bt_dmgr_report_id("[BT_DM][AWS_LOCAL_INFO][I] AWS local role = (0x%x) mode = (0x%x)", 2, bt_dm_aws_cnt.aws_role, bt_dm_aws_cnt.aws_mode);
}

