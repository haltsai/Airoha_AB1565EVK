
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
 * File: app_ms_teams_utils.c
 *
 * Description:
 * This file is used to provide the common function for ms_teams
 *
 */

#include "app_ms_teams_utils.h"
#include "ui_shell_activity.h"
#include "ui_shell_manager.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd.h"
#include "race_cmd_relay_cmd.h"
#include "race_noti.h"
#include "race_bt.h"
#endif
#include "app_ms_teams_idle_activity.h"

#define TAG "[MS TEAMS] utils "

static void ms_teams_event_handler(ms_teams_event_t ev, uint32_t sub_event, uint8_t *data, uint32_t data_len)
{
    uint32_t shell_event = 0;
    uint8_t *data_copy = NULL;
    ui_shell_status_t ui_ret = UI_SHELL_STATUS_OK;

    APPS_LOG_MSGID_I(TAG"teams event callback: 0x%x, 0x%x, 0x%x, 0x%x.", 4, ev, sub_event, data, data_len);
#ifdef MTK_AWS_MCE_ENABLE
    /* TODO: send event and data to partner. */
#endif
    shell_event = ((ev << 16) & 0xFFFF0000) | (sub_event & 0xFFFF);
    if (data != NULL && data_len != 0) {
        data_copy = (uint8_t *)pvPortMalloc(data_len);
        if (data_copy == NULL) {
            APPS_LOG_MSGID_I(TAG"teams event handler malloc mem failed.", 0);
            return;
        }
        memcpy(data_copy, data, data_len);
    } else {
        data_len = 0;
    }

    ui_ret = ui_shell_send_event(true, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_MS_TEAMS,
                        shell_event,
                        data_copy,
                        data_len,
                        NULL, 0);
    if (ui_ret != UI_SHELL_STATUS_OK && data_len != 0) {
        APPS_LOG_MSGID_E(TAG"teams event handler send event fail.", 0);
        vPortFree(data_copy);
    }
}

void app_ms_teams_init() {
    ms_teams_error_code_t ret = MS_TEAMS_ERROR_CODE_NONE;
    ms_teams_config_t config = {
#ifndef ULL_DONGLE
        MS_TEAMS_BT_SRV,
#else
        MS_TEAMS_USB_PROXY
#endif
        ms_teams_event_handler,
        0x02
    };

    ret = ms_teams_init(&config);
    APPS_LOG_MSGID_I(TAG"init result: %d", 1, ret);
}

void app_ms_teams_set_mmi_state(ms_teams_call_state_sub_event_t ev) {
    apps_config_state_t mmi_sta = APP_TOTAL_STATE_NO;
    /* TODO: handle the state update logic with HFP */
    switch (ev) {
        case MS_TEAMS_CALL_EVENT_END_CALL:
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                            NULL, 0);
            break;
        case MS_TEAMS_CALL_EVENT_OUTGOING_CALL:
            mmi_sta = APP_HFP_OUTGOING;
            apps_config_set_background_led_pattern(LED_INDEX_OUTGOING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        case MS_TEAMS_CALL_EVENT_INCOMING_CALL:
            mmi_sta = APP_HFP_INCOMING;
            apps_config_set_background_led_pattern(LED_INDEX_INCOMING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        case MS_TEAMS_CALL_EVENT_CALL_IN_PROCESS:
            mmi_sta = APP_HFP_CALLACTIVE;
            apps_config_set_background_led_pattern(LED_INDEX_CALL_ACTIVE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        case MS_TEAMS_CALL_EVENT_CALLER_ID:
            break;
        case MS_TEAMS_CALL_EVENT_MUTE_STATE_UPDATE:
            break;
        case MS_TEAMS_CALL_EVENT_HOLD_CALL:
            mmi_sta = APP_STATE_HELD_ACTIVE;
            apps_config_set_background_led_pattern(LED_INDEX_CALL_ACTIVE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
    }

    if (mmi_sta != APP_TOTAL_STATE_NO) {
        apps_config_key_set_mmi_state(mmi_sta);
    }

    APPS_LOG_MSGID_I(TAG"mmi update to 0x%x with 0x%x", 2, mmi_sta, ev);
}

bool app_ms_teams_is_dongle_connected() {
#ifdef AIR_LE_AUDIO_ENABLE
    APPS_LOG_MSGID_I(TAG"dongle connected, le-audio case", 0);
    return true;
#else
#if defined(MTK_RACE_CMD_ENABLE)
    bt_bd_addr_t addr_list[1];
    uint32_t list_num = 1;
    list_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL),
                                                        addr_list, list_num);
    APPS_LOG_MSGID_I(TAG"dongle connected debug %d,0x%x", 2, list_num, bt_cm_get_connected_profile_services(addr_list[0]));
    if (list_num > 0 &&
        BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AIR) & bt_cm_get_connected_profile_services(addr_list[0])) {
        APPS_LOG_MSGID_I(TAG"dongle connected", 0);
        return true;
    }
#endif
    return false;
#endif
}

#if 0
void app_ms_teams_send_race_data_to_dongle(uint8_t type, uint8_t *data, uint32_t data_len) {
#if defined(MTK_RACE_CMD_ENABLE)
    uint8_t channel_id = race_get_channel_id_by_port_type(RACE_SERIAL_PORT_TYPE_SPP);
    ms_teams_race_pkg_t *pkg = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND_WITHOUT_RSP,
            (uint16_t)MS_TEAMS_RACE_SET_DATA,
            (uint16_t)(sizeof(uint8_t) + data_len),
            channel_id);
    pkg->type = type;
    if (data_len > 0) {
        memcpy(&pkg->data, data, data_len);
    }
    RACE_ERRCODE race_notify_ret = race_noti_send(pkg, channel_id, false);
    APPS_LOG_MSGID_I(TAG"set race data to dongle ret=%d", 1, race_notify_ret);
    if (race_notify_ret != RACE_ERRCODE_SUCCESS) {
        RACE_FreePacket((void *)pkg);
    }
#endif
}

static void* apps_ms_teams_race_handler(ptr_race_pkt_t p_race_package, uint16_t length, uint8_t channel_id)
{
    typedef struct
    {
        RACE_COMMON_HDR_STRU cmdhdr;
        uint8_t data[0];
    } PACKED CMD1;
    CMD1* cmd = (CMD1 *)p_race_package;
    APPS_LOG_MSGID_I(TAG"receive data from headset, type=%d, len=%d", 2, cmd->data[0], cmd->cmdhdr.length);
    if (cmd->data[0] == 0) {
        ms_teams_btn_press_type_t type = MS_TEAMS_BTN_PRESS_TYPE_LONG_PRESS;
        ms_teams_error_code_t ret = ms_teams_send_action(MS_TEAMS_ACTION_TEAMS_BTN_INVOKE, &type, sizeof(ms_teams_btn_press_type_t));
        APPS_LOG_MSGID_I(TAG"call btn invoke, ret=%d.", 1, ret);
    } else {
        ms_teams_btn_press_type_t type = MS_TEAMS_BTN_PRESS_TYPE_NONE;
        ms_teams_error_code_t ret = ms_teams_send_action(MS_TEAMS_ACTION_TEAMS_BTN_RELEASE, &type, sizeof(ms_teams_btn_press_type_t));
        APPS_LOG_MSGID_I(TAG"call btn invoke, ret=%d.", 1, ret);
    }
    return NULL;
}

void app_ms_teams_event_init()
{
#if defined(MTK_RACE_CMD_ENABLE)
    RACE_HANDLER handler = {
        .id_start = MS_TEAMS_RACE_RACE_ID_START,
        .id_end = MS_TEAMS_RACE_RACE_ID_END,
        .handler = apps_ms_teams_race_handler
    };
    race_status_t ret = RACE_Register_Handler(&handler);
    APPS_LOG_MSGID_I(TAG"init race ret=%d", 1, ret);
#endif
}
#endif

