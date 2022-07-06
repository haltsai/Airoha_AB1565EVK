
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

/**
 * File: app_bt_emp_service.c
 *
 * Description: This file provides BT EMP switch API.
 *
 */

#ifdef AIR_MULTI_POINT_ENABLE

#include "app_bt_emp_service.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#endif
#ifdef AIR_LE_AUDIO_ENABLE
#include "app_le_audio.h"
#endif
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#include "bt_customer_config.h"
#include "bt_device_manager.h"
#include "FreeRTOS.h"
#include "nvkey.h"
#include "nvkey_id_list.h"

#if defined(AIR_LE_AUDIO_ENABLE) && (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))
#include "app_bt_conn_manager.h"
#endif
#ifdef MTK_VA_XIAOAI_ENABLE
#include "xiaoai.h"
#endif

#define APP_BT_EMP_LOG_TAG     "[EMP]"

static void app_bt_emp_srv_init();
static void app_bt_emp_srv_update_nvkey(bool enable);

static app_bt_emp_switch_allow_cb_t g_bt_emp_srv_user_list[APP_BT_EMP_SRV_USER_ID_MAX] = {0};
static bool                         g_bt_emp_enable = FALSE;
#ifdef MTK_AWS_MCE_ENABLE
static bool                         g_bt_emp_srv_aws_connected = FALSE;
static bool app_bt_emp_srv_sync_to_peer(uint8_t enable);
#endif

static uint8_t app_bt_emp_srv_get_bt_cm_num(bool enable)
{
    uint8_t bt_cm_num = 0;
#ifdef MTK_AWS_MCE_ENABLE
    bt_cm_num += 1 + 1; // one EDR + AWS
#else
    bt_cm_num += 1;     // only one EDR
#endif
    const bt_cm_config_t *bt_cm_config = bt_customer_config_get_cm_config();
    if (bt_cm_config->connection_takeover) {
        bt_cm_num += 1;
    }
    if (enable) {
        bt_cm_num += 1;
    }

#if defined(AIR_LE_AUDIO_ENABLE) && (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))
    uint8_t app_bt_conn_bt_cm_num = app_bt_conn_manager_get_bt_cm_num();
    bool exist_le_audio = app_bt_conn_manager_exist_le_audio();
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" [APP_CONN] exist_le_audio=%d bt_cm_num=%d-%d",
                    3, exist_le_audio, bt_cm_num, app_bt_conn_bt_cm_num);
    if (exist_le_audio) {
        if (bt_cm_num < app_bt_conn_bt_cm_num) {
            APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" [APP_CONN] use app_bt_emp_service->bt_cm_num", 0);
        } else {
            bt_cm_num = 0;
            APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" [APP_CONN] keep app_bt_conn_manager->bt_cm_num", 0);
        }
    }
#endif

    return bt_cm_num;
}

bool app_bt_emp_srv_activity_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" CREATE", 0);
            app_bt_emp_srv_init();
        }
        break;
    }
    return TRUE;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool app_bt_emp_srv_activity_bt_cm_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == remote_update) {
                break;
            }
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_PARTNER == role) {
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" [%02X] AWS Connected", 1, role);
                    g_bt_emp_srv_aws_connected = TRUE;
//#ifdef MTK_AWS_MCE_ENABLE
//                    app_bt_emp_srv_sync_to_partner(g_bt_emp_enable);
//#endif
                } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" [%02X] AWS Disconnected", 1, role);
                    g_bt_emp_srv_aws_connected = FALSE;
                }
            }
            break;
        }
    }
    return FALSE;
}

static bool app_bt_emp_srv_partner_enable(bool enable)
{
    if (g_bt_emp_enable == enable) {
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" partner_enable fail, already enable=%d", 1, enable);
        return TRUE;
    }
    uint8_t bt_cm_num = app_bt_emp_srv_get_bt_cm_num(enable);
    bt_bd_addr_t keep_phone_addr[1] = {{0}};
    bt_status_t status = bt_cm_set_max_connection_number(bt_cm_num, keep_phone_addr, 0, FALSE);
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" partner AWS Data, enable=%d bt_cm_num=%d status=0x%08X",
                     3, enable, bt_cm_num, status);
    if (status == BT_STATUS_SUCCESS) {
        app_bt_emp_srv_update_nvkey(enable);
    }

#ifdef AIR_LE_AUDIO_ENABLE
    //app_le_audio_enable_multi_conn(enable);
#endif
    return TRUE;
}

bool app_bt_emp_srv_activity_proc_aws_data(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role == BT_AWS_MCE_ROLE_PARTNER) {
        if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_EMP) {
            configASSERT(aws_data_ind->param_len == sizeof(uint8_t));
            uint8_t *enable = aws_data_ind->param;
            app_bt_emp_srv_partner_enable((bool)*enable);
        }
    } else if (role == BT_AWS_MCE_ROLE_AGENT) {
        if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_EMP) {
            configASSERT(aws_data_ind->param_len == sizeof(uint8_t));
            uint8_t *enable = aws_data_ind->param;
            app_bt_emp_temp_enable((bool)*enable);
        }
    }
    return FALSE;
}

static bool app_bt_emp_srv_sync_to_peer(uint8_t enable)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (g_bt_emp_srv_aws_connected) {
        uint8_t info_array[sizeof(bt_aws_mce_report_info_t)] = {0};
        uint8_t data_array[sizeof(uint8_t)] = {0};
        bt_aws_mce_report_info_t *aws_data = (bt_aws_mce_report_info_t *)&info_array;
        aws_data->module_id = BT_AWS_MCE_REPORT_MODULE_APP_EMP;
        aws_data->is_sync = FALSE;
        aws_data->sync_time = 0;
        aws_data->param_len = sizeof(uint8_t);
        memcpy((uint8_t *)data_array, &enable, sizeof(uint8_t));
        aws_data->param = (void *)data_array;
        bt_status_t status = bt_aws_mce_report_send_event(aws_data);
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" [%02X] sync_to_peer 0x%02X enable=%d status=0x%08X",
                         4, role, BT_AWS_MCE_REPORT_MODULE_APP_EMP, enable, status);
        return (status == BT_STATUS_SUCCESS);
    } else {
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" [%02X] sync_to_peer fail", 1, role);
        return FALSE;
    }
}

#endif

static void app_bt_emp_srv_update_nvkey(bool enable)
{
    uint32_t size = sizeof(uint8_t);
    nvkey_status_t status = nvkey_write_data(NVKEYID_APP_MULTI_POINT_ENABLE, (const uint8_t *)&enable, size);
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_srv update_nvkey, enable=%d status=%d",
                     2, enable, status);
    if (status == NVKEY_STATUS_OK) {
        g_bt_emp_enable = enable;
    }
}

static void app_bt_emp_srv_init()
{
    uint32_t size = sizeof(uint8_t);
    nvkey_status_t status = nvkey_read_data(NVKEYID_APP_MULTI_POINT_ENABLE, (uint8_t *)&g_bt_emp_enable, &size);
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_srv init, read status=%d", 1, status);
    if (status == NVKEY_STATUS_ITEM_NOT_FOUND) {
        bool emp_enable = APP_BT_EMP_SWITCH_INIT_STATE;
        size = sizeof(uint8_t);
        status = nvkey_write_data(NVKEYID_APP_MULTI_POINT_ENABLE, (const uint8_t *)&emp_enable, size);
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_srv init, write status=%d", 1, status);
        if (status == NVKEY_STATUS_OK) {
            g_bt_emp_enable = emp_enable;
        }
    }
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_srv init %d", 1, g_bt_emp_enable);
}

bool app_bt_emp_service_activity_proc(struct _ui_shell_activity *self,
                                      uint32_t event_group,
                                      uint32_t event_id,
                                      void *extra_data,
                                      size_t data_len)
{
    bool ret = FALSE;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = app_bt_emp_srv_activity_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = app_bt_emp_srv_activity_bt_cm_event_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = app_bt_emp_srv_activity_proc_aws_data(self, event_id, extra_data, data_len);
            break;
#endif
    }
    return ret;
}

bool app_bt_emp_is_enable()
{
    return g_bt_emp_enable;
}

bool app_bt_emp_enable(bool enable)
{
    if (g_bt_emp_enable == enable) {
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_enable fail, already enable=%d", 1, enable);
        return TRUE;
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_E(APP_BT_EMP_LOG_TAG" app_bt_emp_enable fail, not agent role=%02X", 1, role);
        return FALSE;
    } else if (!g_bt_emp_srv_aws_connected) {
        APPS_LOG_MSGID_E(APP_BT_EMP_LOG_TAG" app_bt_emp_enable fail, must AWS connected", 0);
        return FALSE;
    }
#endif

#ifdef AIR_LE_AUDIO_ENABLE
    //app_le_audio_enable_multi_conn(enable);
#endif

    bool success = FALSE;
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_enable, enable=%d", 1, enable);

    // Check keep_phone_addr (now only keep one SP connection)
    bt_bd_addr_t keep_phone_addr[1] = {{0}};
    uint8_t list_size = 0;
    if (!enable) {
#ifdef MTK_VA_XIAOAI_ENABLE
        xiaoai_conn_info_t conn_info = xiaoai_get_connection_info();
        xiaoai_connection_state xiaoai_state = conn_info.conn_state;
        bool is_ble = conn_info.is_ble;
        if (xiaoai_state == XIAOAI_STATE_CONNECTED && !is_ble) {
            uint8_t *addr = conn_info.phone_addr;
            memcpy(keep_phone_addr[0], addr, 6);
            APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_enable, XiaoAI state=%d is_ble=%d %02X:%02X:%02X:%02X:%02X:%02X",
                             8, xiaoai_state, is_ble, addr[0], addr[1], addr[2],
                             addr[3],addr[4],addr[5]);
            list_size = 1;
        } else {
            APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_enable, no keep_phone_addr, xiaoai_state=%d is_ble=%d",
                             2, xiaoai_state, is_ble);
        }
#else
    // Other VA or APP to determine the keep_phone_addr
#endif
    }

    // Check whether allow
    bool allow = TRUE;
    for (uint8_t user_id = 0; user_id < APP_BT_EMP_SRV_USER_ID_MAX; user_id++) {
        app_bt_emp_switch_allow_cb_t allow_cb = g_bt_emp_srv_user_list[user_id];
        if (allow_cb != NULL && !allow_cb(enable, keep_phone_addr)) {
            APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_enable, user_id=%d disallow", 1, user_id);
            allow = FALSE;
            break;
        }
    }
    if (!allow) {
        goto exit;
    }

    // Count bt_cm_number
    uint8_t bt_cm_num = app_bt_emp_srv_get_bt_cm_num(enable);
    if (bt_cm_num == 0) {
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_enable, return when bt_cm_num=0", 0);
        return TRUE;
    }

    // BT CM set max connection_number
    bt_status_t status = bt_cm_set_max_connection_number(bt_cm_num, keep_phone_addr, list_size, TRUE);
    uint8_t *addr = (uint8_t *)keep_phone_addr;
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_enable, bt_cm_num=%d %02X:%02X:%02X:%02X:%02X:%02X list_size=%d status=0x%08X",
                     9, bt_cm_num, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
                     list_size, status);
    if (status != BT_STATUS_SUCCESS) {
        goto exit;
    }

    // Update NVKEY and send to partner
    app_bt_emp_srv_update_nvkey(enable);
#ifdef MTK_AWS_MCE_ENABLE
    app_bt_emp_srv_sync_to_peer(enable);
#endif
    success = TRUE;

exit:
    if (!success) {
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" app_bt_emp_enable, fail", 0);
    }
    return success;
}

void app_bt_emp_reset_max_conn_num()
{
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" reinit %d", 1, g_bt_emp_enable);
    uint8_t bt_cm_num = 0;
#ifdef MTK_AWS_MCE_ENABLE
    bt_cm_num += 1 + 1; // one EDR + AWS
#else
    bt_cm_num += 1;     // only one EDR
#endif
    const bt_cm_config_t *bt_cm_config = bt_customer_config_get_cm_config();
    if (bt_cm_config->connection_takeover) {
        bt_cm_num += 1;
    }
    if (g_bt_emp_enable) {
        bt_cm_num += 1;
    }

    bt_bd_addr_t keep_phone_addr[1] = {{0}};
    bt_status_t status = bt_cm_set_max_connection_number(bt_cm_num, keep_phone_addr, 0, TRUE);
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" reinit, bt_cm num=%d status=0x%08X",
                     2, bt_cm_num, status);
}

bool app_bt_emp_temp_enable(bool enable)
{
    if (enable && !g_bt_emp_enable) {
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" temp_enable, want to enable but EMP OFF", 0);
        return TRUE;
    }

    uint8_t bt_cm_num = app_bt_emp_srv_get_bt_cm_num(enable);
    if (bt_cm_num == 0) {
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" temp_enable, return when bt_cm_num=0", 0);
        return TRUE;
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role == BT_AWS_MCE_ROLE_PARTNER) {
        bool ret = app_bt_emp_srv_sync_to_peer(enable);
        return ret;
    }
#endif

    bt_bd_addr_t keep_phone_addr[1] = {{0}};
    bt_status_t status = bt_cm_set_max_connection_number(bt_cm_num, keep_phone_addr, 0, TRUE);
    APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" temp_enable, bt_cm num=%d status=0x%08X",
                     2, bt_cm_num, status);
    return (status == BT_STATUS_SUCCESS);
}

bool app_bt_emp_force_disable(const uint8_t *addr)
{
    if (g_bt_emp_enable) {
#ifdef MTK_AWS_MCE_ENABLE
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
        if (role != BT_AWS_MCE_ROLE_AGENT) {
            APPS_LOG_MSGID_E(APP_BT_EMP_LOG_TAG" force_disable fail, not agent role=%02X", 1, role);
            return FALSE;
        } else if (!g_bt_emp_srv_aws_connected) {
            APPS_LOG_MSGID_E(APP_BT_EMP_LOG_TAG" force_disable fail, must AWS connected", 0);
            return FALSE;
        }
#endif

        uint8_t bt_cm_num = app_bt_emp_srv_get_bt_cm_num(FALSE);
        if (bt_cm_num == 0) {
            APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" force_disable, return when bt_cm_num=0", 0);
            return TRUE;
        }

        bt_bd_addr_t keep_phone_addr[1] = {{0}};
        memcpy(keep_phone_addr[0], addr, 6);
        bt_status_t status = bt_cm_set_max_connection_number(bt_cm_num, keep_phone_addr, 1, FALSE);
        if (status != BT_STATUS_SUCCESS) {
            APPS_LOG_MSGID_E(APP_BT_EMP_LOG_TAG" force_disable fail, bt_cm_num=%d status=0x%08X",
                             2, bt_cm_num, status);
            return FALSE;
        }

        // Update NVKEY and send to peer
        app_bt_emp_srv_update_nvkey(FALSE);
#ifdef MTK_AWS_MCE_ENABLE
        app_bt_emp_srv_sync_to_peer(FALSE);
#endif

        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" force_disable pass, bt_cm_num=%d addr=%02X:%02X:%02X:%02X:%02X:%02X",
                         7, bt_cm_num, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
        return TRUE;
    } else {
        APPS_LOG_MSGID_E(APP_BT_EMP_LOG_TAG" force_disable, already disable", 0);
        return TRUE;
    }
}

bool app_bt_emp_srv_user_register(app_bt_emp_srv_user_id user_id, app_bt_emp_switch_allow_cb_t func)
{
    if (user_id < APP_BT_EMP_SRV_USER_ID_MAX) {
        g_bt_emp_srv_user_list[user_id] = func;
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" user_register, user_id=%d", 1, user_id);
        return TRUE;
    } else {
        APPS_LOG_MSGID_E(APP_BT_EMP_LOG_TAG" user_register, user not found", 0);
        return FALSE;
    }
}

bool app_bt_emp_srv_user_deregister(app_bt_emp_srv_user_id user_id)
{
    if (user_id < APP_BT_EMP_SRV_USER_ID_MAX) {
        g_bt_emp_srv_user_list[user_id] = NULL;
        APPS_LOG_MSGID_I(APP_BT_EMP_LOG_TAG" user_deregister, user_id=%d", 1, user_id);
        return TRUE;
    } else {
        APPS_LOG_MSGID_E(APP_BT_EMP_LOG_TAG" user_deregister, user_id not found", 0);
        return FALSE;
    }
}

#endif  /* AIR_MULTI_POINT_ENABLE */
