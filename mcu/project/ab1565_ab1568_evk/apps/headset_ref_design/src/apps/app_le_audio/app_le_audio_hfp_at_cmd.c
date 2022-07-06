
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
 * File: app_le_audio_hfp_at_mcd.c
 *
 * Description: This file provides HFP AT CMD for LE Audio or other feature.
 *
 */

#ifdef AIR_LE_AUDIO_ENABLE

#include "app_le_audio_hfp_at_cmd.h"

#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "bt_app_common.h"
#include "bt_callback_manager.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_hf.h"
#include "ui_shell_manager.h"
#include "FreeRTOS.h"

#include "app_le_audio.h"
#ifdef AIR_MULTI_POINT_ENABLE
#include "app_bt_emp_service.h"
#endif
#ifdef MTK_SMART_CHARGER_ENABLE
#include "app_smcharger_utils.h"
#endif

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

#define LOG_TAG           "[LEA][HF]"

#define APP_LE_AUDIO_HFP_ATCMD            "+MTK="
#define APP_LE_AUDIO_HFP_ATCMD_HEADER     "FFFAFB"
#define APP_LE_AUDIO_HFP_ATCMD_LEN        (strlen(APP_LE_AUDIO_HFP_ATCMD))
#define APP_LE_AUDIO_HFP_HEADER_LEN       (strlen(APP_LE_AUDIO_HFP_ATCMD_HEADER)) // <FF><FA><FB>

#define APP_LE_AUDIO_BATTERY_TIME         (10 * 60 * 1000)

#define APP_LE_AUDIO_ATCMD_MAX_LEN        200

static uint32_t g_le_audio_hfp_handle = 0;

typedef enum {
    LE_AUDIO_HFP_ATCMD_TYPE_SWITCH = 0,
    LE_AUDIO_HFP_ATCMD_TYPE_BATTERY = 1
} le_audio_hfp_atcmd_type;

static bool app_le_audio_send_hfp_atcmd(uint8_t type, uint8_t *data, uint8_t data_len)
{
    bool ret = FALSE;
    char *atcmd = NULL;
    char *param = NULL;

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG" send_hfp_atcmd [%02X] type=%d data=0x%08X data_len=%d",
                     4, role, type, data, data_len);
    if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
        APPS_LOG_MSGID_E(LOG_TAG" send_hfp_atcmd, role fail", 0);
        goto exit;
    }
#else
    APPS_LOG_MSGID_I(LOG_TAG" send_hfp_atcmd type=%d data=0x%08X data_len=%d",
                     3, type, data, data_len);
#endif

    if ((type != LE_AUDIO_HFP_ATCMD_TYPE_SWITCH && type != LE_AUDIO_HFP_ATCMD_TYPE_BATTERY)
        || data == NULL || data_len == 0) {
        APPS_LOG_MSGID_E(LOG_TAG" send_hfp_atcmd, parameter fail", 0);
        goto exit;
    }

    atcmd = (char *)pvPortMalloc(APP_LE_AUDIO_ATCMD_MAX_LEN);
    param = (char *)pvPortMalloc(data_len * 2 + 1);
    if (atcmd == NULL || param == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG" send_hfp_atcmd, malloc fail", 0);
        goto exit;
    }
    memset(atcmd, 0, APP_LE_AUDIO_ATCMD_MAX_LEN);
    memset(param, 0, data_len * 2 + 1);

    for (int i = 0; i < data_len; i++) {
        char temp[3] = {0};
        snprintf(temp, 3, "%02X", data[i]);
        param[i * 2] = temp[0];
        param[i * 2 + 1] = temp[1];
    }

    // AT+XIAOMI=<FF> <01><02><01> <01/03> <length><type><data(LTV List)>... <FF>
    snprintf(atcmd, APP_LE_AUDIO_ATCMD_MAX_LEN, "FFFAFB%02X%02X%sFF", type, data_len, param);

    int atcmd_len = strlen(atcmd);
    APPS_LOG_I("[LEA][HF] HFP ATCMD=%s atcmd_len=%d\r\n", (char *)atcmd, atcmd_len);

    if (atcmd != NULL && atcmd_len > APP_LE_AUDIO_HFP_HEADER_LEN) {
        bt_status_t status = bt_sink_srv_hf_mtk_custom(atcmd, atcmd_len);
        ret = (status == BT_STATUS_SUCCESS);
    }

exit:
    if (atcmd != NULL) {
        vPortFree(atcmd);
    }
    if (param != NULL) {
        vPortFree(param);
    }
    return ret;
}

static void app_le_audio_switch(const char *cmd)
{
    uint8_t len = 0;
    uint8_t type = 0;
    uint8_t value = 0;
    int n = sscanf(cmd, "%02x%02x%02x", (unsigned int *)&type, (unsigned int *)&len, (unsigned int *)&value);
    APPS_LOG_MSGID_I(LOG_TAG" switch, type=%d len=%d value=%d", 3, type, len, value);

    if (n == 3 && type == LE_AUDIO_HFP_ATCMD_TYPE_SWITCH && len == 1 && (value == 0 || value == 1)) {
        bool enable = (value == 1);
        if (enable) {
#ifdef AIR_MULTI_POINT_ENABLE
            // disconnect other edr if existing
            bt_bd_addr_t *bt_bd_addr = bt_hfp_get_bd_addr_by_handle(g_le_audio_hfp_handle);
            const uint8_t *addr = (const uint8_t *)*bt_bd_addr;
            app_bt_emp_force_disable(addr);
#endif
        }
        if (enable) {
            app_le_audio_restart_advertising(APP_LE_AUDIO_ADV_TIME);
        }
        bool ret = app_le_audio_send_hfp_atcmd(LE_AUDIO_HFP_ATCMD_TYPE_SWITCH, &value, 1);
        APPS_LOG_MSGID_I(LOG_TAG" switch, send rsp ret=%d", 1, ret);
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" switch, error cmd", 0);
    }
}

static bt_status_t app_le_audio_hfp_callback(bt_msg_type_t event, bt_status_t status, void *param)
{
    switch (event) {
        case BT_HFP_CUSTOM_COMMAND_RESULT_IND: {
            bt_hfp_custom_command_result_ind_t *ind = (bt_hfp_custom_command_result_ind_t *)param;
            const char *atcmd = ind->result;
            // +MTK:<FF><FA><FB><T><L>"V"<FF>
            if (status == BT_STATUS_SUCCESS && atcmd != NULL) {
                g_le_audio_hfp_handle = ind->handle;
                int atcmd_len = strlen(atcmd) - APP_LE_AUDIO_HFP_ATCMD_LEN - APP_LE_AUDIO_HFP_HEADER_LEN;
                if (atcmd_len > 0
                    && strstr(atcmd, APP_LE_AUDIO_HFP_ATCMD) > 0
                    && strstr(atcmd, APP_LE_AUDIO_HFP_ATCMD_HEADER) > 0) {
                    atcmd += APP_LE_AUDIO_HFP_ATCMD_LEN + APP_LE_AUDIO_HFP_HEADER_LEN;
                    uint8_t type = 0;
                    uint8_t len = 0;
                    sscanf(atcmd, "%02x%02x", (unsigned int *)&type, (unsigned int *)&len);
                    APPS_LOG_MSGID_I(LOG_TAG" hfp_callback, type=%d len=%d", 2, type, len);

                    // Check suffix
                    const char *suffix = atcmd + 2 + 2 + len * 2;
                    if (suffix[0] != 'F' || suffix[1] != 'F') {
                        APPS_LOG_MSGID_E(LOG_TAG" hfp_callback, invalid suffix", 0);
                        break;
                    }

                    if (type == LE_AUDIO_HFP_ATCMD_TYPE_SWITCH) {
                        app_le_audio_switch(atcmd);
                    } else if (type == LE_AUDIO_HFP_ATCMD_TYPE_BATTERY) {
                        APPS_LOG_MSGID_E(LOG_TAG" hfp_callback, not support query battery_info", 0);
                    } else {
                        APPS_LOG_MSGID_E(LOG_TAG" hfp_callback, invalid type", 0);
                    }
                }
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" hfp_callback, error status=0x%08X atcmd=0x%08X",
                                 2, status, atcmd);
            }
            break;
        }
    }
    return BT_STATUS_SUCCESS;
}





/**================================================================================*/
/**                             HFP ATCMD PUBLIC API                               */
/**================================================================================*/
void app_le_audio_hfp_at_cmd_register(bool enable)
{
    bt_status_t status = BT_STATUS_FAIL;
    if (enable) {
        status = bt_callback_manager_register_callback(bt_callback_type_app_event,
                                    MODULE_MASK_HFP, (void *)app_le_audio_hfp_callback);
    } else {
        status = bt_callback_manager_deregister_callback(bt_callback_type_app_event,
                                    (void *)app_le_audio_hfp_callback);
    }
    APPS_LOG_MSGID_I(LOG_TAG" register HFP enable=%d status=0x%08X", 2, enable, status);
}

void app_le_audio_hfp_start_battery_timer(bool enable)
{
    uint32_t hfp_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP), NULL, 0);
    APPS_LOG_MSGID_I(LOG_TAG" start_battery_timer, enable=%d hfp_num=%d", 2, enable, hfp_num);
    if (enable && hfp_num > 0) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_NOTIFY_BATTERY);
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                            EVENT_ID_LE_AUDIO_NOTIFY_BATTERY, NULL, 0, NULL, APP_LE_AUDIO_BATTERY_TIME);
    } else if (!enable && hfp_num == 0) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_NOTIFY_BATTERY);
    }
}

void app_le_audio_hfp_send_battery_info()
{
#ifdef MTK_AWS_MCE_ENABLE
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_E(LOG_TAG" hfp_send_battery_info, not Agent", 0);
        return;
    }
#endif

    uint32_t hfp_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP), NULL, 0);
    if (hfp_num > 0) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_NOTIFY_BATTERY);
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_LE_AUDIO,
                            EVENT_ID_LE_AUDIO_NOTIFY_BATTERY, NULL, 0, NULL, APP_LE_AUDIO_BATTERY_TIME);
    } else if (hfp_num == 0) {
        APPS_LOG_MSGID_I(LOG_TAG" hfp_send_battery_info, stop timer", 0);
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_NOTIFY_BATTERY);
        return;
    }

    uint8_t report_data[3] = {0};
#ifdef MTK_SMART_CHARGER_ENABLE
    uint8_t own_battery = 0;
    uint8_t peer_battery = 0;
    uint8_t case_battery = 0;
    app_smcharger_get_battery_percent(&own_battery, &peer_battery, &case_battery);
    if (own_battery != 0xFF && app_smcharger_is_charging() == APP_SMCHARGER_IN) {
        own_battery |= 0x80;
    }
    if (peer_battery != 0xFF && app_smcharger_peer_is_charging() == APP_SMCHARGER_IN) {
        peer_battery |= 0x80;
    }

    audio_channel_t channel = ami_get_audio_channel();
    if (AUDIO_CHANNEL_L == channel) {
        report_data[0] = own_battery;
        report_data[1] = peer_battery;
    } else {
        report_data[0] = peer_battery;
        report_data[1] = own_battery;
    }
    report_data[2] = case_battery;
#endif

    bool ret = app_le_audio_send_hfp_atcmd(LE_AUDIO_HFP_ATCMD_TYPE_BATTERY, report_data, 3);
    APPS_LOG_MSGID_I(LOG_TAG" hfp_send_battery_info ret=%d", 1, ret);
}

#endif /* AIR_LE_AUDIO_ENABLE */
