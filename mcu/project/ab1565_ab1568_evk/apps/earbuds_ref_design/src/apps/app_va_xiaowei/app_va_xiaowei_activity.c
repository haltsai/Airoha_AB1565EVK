
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
 * File: app_va_xiaowei_activity.c
 *
 * Description:
 * This file is Xiaowei idle activity. This activity is used for Xiaowei's triggering and
 * state management.
 *
 */


#ifdef MTK_VA_XIAOWEI_ENABLE

#include "app_va_xiaowei_activity.h"
#include "app_va_xiaowei_transient_activity.h"
#include "app_va_xiaowei_device_control.h"
#include "apps_config_event_list.h"
#include "apps_aws_sync_event.h"
#include "apps_events_event_group.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "app_voice_prompt.h"
#include "apps_debug.h"
#include "xiaowei.h"
#include "multi_va_manager.h"
#include "bt_sink_srv.h"
#include "bt_aws_mce_report.h"
#include "va_xiaowei_customer_config.h"
#include "bt_connection_manager.h"
#include "atci.h"
#include "apps_config_key_remapper.h"
#include "bt_sink_srv_music.h"
#include "app_bt_takeover_service.h"

#if defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2) && defined(MTK_ANC_ENABLE)
#include "app_adaptive_anc_idle_activity.h"
#endif /* MTK_USER_TRIGGER_ADAPTIVE_FF_V2 */

#define APP_VA_XIAOWEI_PREFIX "[VA_XIAOWEI] VA_XIAOWEI_ACTIVITY"

xiaowei_status_t va_xiaowei_status  = XIAOWEI_STATUS_NONE;
xiaowei_key_function_t key_func     = XIAOWEI_KEY_FUNCTION_XIAOWEI;

bool xiaowei_init_done              = false;
bool play_tone_flag_enabled         = false;

/**
 * @brief For multi-point solution
 * Which can resume another Smart phone music if it's playing status
 * @param enable true : Support to play the response
 *               false : Resume the another smart phone music
 */
void app_va_xiaowei_activity_config_play_tone(bool enable)
{
    unsigned char address[6] = {0};
    bool is_ble_address = false;
    bool get_address_result = false;
    bool need_configure_play_tone_flag = false;

    get_address_result = xiaowei_get_connected_device_address(address, &is_ble_address);
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Enable : %d, play_tone_flag_enabled : %d, get_address_result : %d, is_ble_address : %d",
                4, enable, play_tone_flag_enabled, get_address_result, is_ble_address);

    if (get_address_result == false) {
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Failed to get connected device address", 0);
        return;
    }

    if ((enable == true) && (play_tone_flag_enabled == false)) {
        play_tone_flag_enabled = true;
        need_configure_play_tone_flag = true;
    }
    if ((enable == false) && (play_tone_flag_enabled == true)) {
        play_tone_flag_enabled = false;
        need_configure_play_tone_flag = true;
    }

    if (need_configure_play_tone_flag == true) {
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Set MUST play tone flag to be : %d", 1, play_tone_flag_enabled);
        if (is_ble_address == true) {
            bt_sink_srv_set_must_play_tone_flag(NULL,
                                BT_SINK_SRV_XIAOWEI_NOTIFICATION_VOICE,
                                play_tone_flag_enabled);
        } else {
            bt_sink_srv_set_must_play_tone_flag(address,
                                BT_SINK_SRV_XIAOWEI_NOTIFICATION_VOICE,
                                play_tone_flag_enabled);
        }
    }
}

/**-----------------------------------------------------------------------------**/
/**                     XIAOWEI LIB CALLBACK HANDLER                            **/
/**-----------------------------------------------------------------------------**/
/**
 * @brief The status change callback handler
 *
 * @param new_status The new status
 */
void app_va_xiaowei_activity_cb_status_changed(xiaowei_status_t new_status)
{
    va_xiaowei_status = new_status;
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Update current status to be : 0x%x", 1, va_xiaowei_status);

    /**
     * @brief If status is recording status, notify A2DP that should play the response
     */
    if (va_xiaowei_status == XIAOWEI_STATUS_RECORDING) {
        app_va_xiaowei_activity_config_play_tone(true);
    }
    /**
     * @brief If already configured play tone, but disconnected need release the tone configure.
     */
    if ((va_xiaowei_status == XIAOWEI_STATUS_DISCONNECTED)
            || (va_xiaowei_status == XIAOWEI_STATUS_PROTOCOL_DISCONNECTED)
            || (va_xiaowei_status == XIAOWEI_STATUS_RECORDER_SUSPENDED)
            || (va_xiaowei_status == XIAOWEI_STATUS_INITED)) {
        app_va_xiaowei_activity_config_play_tone(false);
    }

    /**
     * @brief
     * When received the status changed callback, need to do some task in the UI shell task.
     * Send the new status to the UI shell task to handle.
     */
    ui_shell_send_event(false,
                        EVENT_PRIORITY_HIGH,
                        EVENT_GROUP_UI_SHELL_VA_XIAOWEI,
                        va_xiaowei_status,
                        NULL,
                        0,
                        NULL,
                        0);
}

/**
 * @brief The key function changed callback handler
 *
 * @param new_function The new key function
 */
void app_va_xiaowei_activity_cb_key_function_changed(xiaowei_key_function_t new_function)
{
    key_func = new_function;
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Change key function to be : %d", 1, key_func);
}

/**
 * @brief The AVRCP request callback handler
 *
 * @param code The AVRCP request code
 */
void app_va_xiaowei_activity_cb_avrcp_request(xiaowei_avrcp_request_code_t code)
{
    bt_status_t result = BT_STATUS_FAIL;

    if (code == XIAOWEI_AVRCP_REQUEST_CODE_PAUSE) {
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Execute the AVRCP PAUSE operation", 0);
        result = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PAUSE, NULL);
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Execute the AVRCP PAUSE operation result, 0x%x", 1, result);
    } else if (code == XIAOWEI_AVRCP_REQUEST_CODE_RESUME) {
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Execute the AVRCP RESUME opeartion", 0);
        result = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PLAY, NULL);
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Execute the AVRCP RESUME operation result, 0x%x", 1, result);
    } else {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Unknown avrcp request to handle", 0);
        return;
    }
}

/**
 * @brief The retrieve fw version callback handler
 *
 * @param version [output] Fill in the version, the length is 4 bytes.
 */
void app_va_xiaowei_activity_cb_retrieve_firmware_version(uint8_t *version)
{
    /**
     * @brief Customer should fill in the firmware version from somewhere (maybe FOTA)
     * The version should be like : 0.1.4.1
     */
    version[0] = 0x00;
    version[1] = 0x01;
    version[2] = 0x04;
    version[3] = 0x01;
}

/**
 * @brief The retrieve model name callback handler
 *
 * @param model_name [output] The model name to fill in
 * @param model_name_len [output] The model name length
 */
void app_va_xiaowei_activity_cb_retrieve_model_name(uint8_t **model_name, uint8_t *model_name_len)
{
    *model_name = (uint8_t *)malloc(sizeof(uint8_t) * VA_XIAOWEI_CUSTOMER_CONFIG_MODEL_NAME_LEN);
    if (*model_name == NULL) {
        *model_name_len = 0;
        return;
    }

    memset(*model_name, 0, sizeof(uint8_t) * VA_XIAOWEI_CUSTOMER_CONFIG_MODEL_NAME_LEN);
    memcpy(*model_name, VA_XIAOWEI_CUSTOMER_CONFIG_MODEL_NAME, VA_XIAOWEI_CUSTOMER_CONFIG_MODEL_NAME_LEN);

    *model_name_len = VA_XIAOWEI_CUSTOMER_CONFIG_MODEL_NAME_LEN;
}

/**
 * @brief The retrieve custom name callback handler
 *
 * @param customer_name [output] The custom name to fill in
 * @param customer_name_len [output] The custom name length
 */
void app_va_xiaowei_activity_cb_retrieve_customer_name(uint8_t **customer_name, uint8_t *customer_name_len)
{
    *customer_name = (uint8_t *)malloc(sizeof(uint8_t) * VA_XIAOWEI_CUSTOMER_CONFIG_CUSTOMER_NAME_LEN);
    if (*customer_name == NULL) {
        *customer_name_len = 0;
        return;
    }

    memset(*customer_name, 0, sizeof(uint8_t) * VA_XIAOWEI_CUSTOMER_CONFIG_CUSTOMER_NAME_LEN);
    memcpy(*customer_name, VA_XIAOWEI_CUSTOMER_CONFIG_CUSTOMER_NAME, VA_XIAOWEI_CUSTOMER_CONFIG_CUSTOMER_NAME_LEN);

    *customer_name_len = VA_XIAOWEI_CUSTOMER_CONFIG_CUSTOMER_NAME_LEN;
}

/**
 * @brief The retrieve SKUID callback handler
 *
 * @param skuid [output] The skuid to fill in
 */
void app_va_xiaowei_activity_cb_retrieve_skuid(uint8_t *skuid)
{
    /* Fill in the device SKUID. */
    *skuid = VA_XIAOWEI_CUSTOMER_CONFIG_SKUID;
}

/**
 * @brief The retrieve serial number callback handler
 *
 * @param serial_number [output] The serail number to fill in
 * @param serial_number_len [output] The serial number length
 */
void app_va_xiaowei_activity_cb_retrieve_serial_number(uint8_t **serial_number, uint8_t *serial_number_len)
{
    *serial_number = (uint8_t *)malloc(sizeof(uint8_t) * VA_XIAOWEI_CUSTOMER_CONFIG_SERIAL_NUMBER_LEN);
    if (*serial_number == NULL) {
        *serial_number_len = 0;
        return;
    }

    memset(*serial_number, 0, sizeof(uint8_t) * VA_XIAOWEI_CUSTOMER_CONFIG_SERIAL_NUMBER_LEN);
    memcpy(*serial_number, VA_XIAOWEI_CUSTOMER_CONFIG_SERIAL_NUMBER, VA_XIAOWEI_CUSTOMER_CONFIG_SERIAL_NUMBER_LEN);

    *serial_number_len = VA_XIAOWEI_CUSTOMER_CONFIG_SERIAL_NUMBER_LEN;
}

/**
 * @brief The retrieve keep connection callback handler
 *
 * @param keep [output] The keep connection configure value
 */
void app_va_xiaowei_activity_cb_retrieve_keep_connection(bool *keep)
{
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Keep connection return true", 0);
    *keep = true;
}

/**
 * @brief The device configuration callback handler
 *
 * @param config The configuration structure.
 * @return true If execute succeed, return true
 * @return false If execute failed, return false
 */
bool app_va_xiaowei_activity_cb_handle_device_config(xiaowei_payload_general_config_request_t *config)
{
    // TODO handle the device control flow.
    if (config != NULL) {
        return app_va_xiaowei_device_control_handler(config);
    }
    return false;
}

/**
 * @brief The custom skill command execute callback handler
 *
 * @param skill The custom skill structure.
 * @return true
 * @return false
 */
bool app_va_xiaowei_activity_cb_execute_customer_skill(xiaowei_payload_custom_skill_request_t *skill)
{
    if (skill == NULL) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Skill is NULL", 0);
        return false;
    }

    if ((skill->skill_name_length == 0) || (skill->skill_name == NULL)) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Skill name error (length : %d)", 1, skill->skill_name_length);
        return false;
    }

    if ((skill->intent_name_length == 0) || (skill->intent_name == NULL)) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Skill intent error (length : %d)", 1, skill->intent_name_length);
        return false;
    }

    // TODO Handle the custom skill according to the skill information
    printf("[VA_XIAOWEI] VA_XIAOWEI_ACTIVITY, skill     name : (%d) %s", skill->skill_name_length, skill->skill_name);
    printf("[VA_XIAOWEI] VA_XIAOWEI_ACTIVITY, intent    name : (%d) %s", skill->intent_name_length, skill->intent_name);
    if ((skill->slot_list != NULL) && (skill->slot_count > 0)) {
        uint8_t slot_index = 0;
        for (slot_index = 0; slot_index < skill->slot_count; slot_index ++) {
            printf("[VA_XIAOWEI] VA_XIAOWEI_ACTIVITY, slot [%d] name : (%d) %s, value : (%d) %s",
                        slot_index, skill->slot_list[slot_index].slot_name_length, skill->slot_list[slot_index].slot_name,
                        skill->slot_list[slot_index].slot_value_length, skill->slot_list[slot_index].slot_value);
        }
    }

    /**
     * @brief Should replace the VP with customer vp
     */
    apps_config_set_vp(VP_INDEX_SUCCESSED, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);

    return true;
}

/**
 * @brief The connection control reconnect request callback handler
 * If send connect control command with reconnect request to smart phone, but smart phone
 * already connected with another device
 */
void app_va_xiaowei_activity_cb_connect_control_reconnect_app_connected_with_other_device()
{
    APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", [Connection control] APP already connected with other devices", 0);
}

/**
 * @brief The TTS play finish callback handler
 */
void app_va_xiaowei_activity_cb_tts_play_finished()
{
    app_va_xiaowei_activity_config_play_tone(false);
}

/**-----------------------------------------------------------------------------**/
/**                  XIAOWEI AT CMD CALLBACK IMPLEMENTATION                     **/
/**       This is for testing connection control flow (Should be removed)       **/
/**-----------------------------------------------------------------------------**/
#define APP_VA_XIAOWEI_DIS_PROTOCOL_AND_RELEASE_LINK    \
        ("[XiaoWei] Disconnect protocol and release link, then should connect in Xiaowei APP again")
#define APP_VA_XIAOWEI_DIS_PROTOCOL_BUT_KEEP_LINK    \
        ("[XiaoWei] Disconnect protocol but keep link, then can send reconnect command to connect protocol again")
#define APP_VA_XIAOWEI_RECONNECT_PROTOCOL   \
        ("[XiaoWei] Reconnect xiaowei protocol")
#define APP_VA_XIAOWEI_RELEASE_LINK    \
        ("[XiaoWei] Disconnect xiaowei link, then should connect in Xiaowei APP again")

static atci_status_t app_va_xiaowei_activity_at_cmd_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t *response = (atci_response_t *)malloc(sizeof(atci_response_t));

    if (parse_cmd == NULL) {
        response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        response->response_len = 0;
        atci_send_response(response);
        free(response);
        response = NULL;
        return ATCI_STATUS_OK;
    }

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            printf("[VA_XIAOWEI] VA_XIAOWEI_ACTIVITY, AT CMD : %s", parse_cmd->string_ptr);
            xiaowei_connect_control_flag_t flag = XIAOWEI_CONNECT_CONTROL_FLAG_DISCONNECT_AND_RELEASE_LINK;
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "0", 1)) {
                flag = XIAOWEI_CONNECT_CONTROL_FLAG_DISCONNECT_AND_RELEASE_LINK;
                memcpy(response->response_buf, APP_VA_XIAOWEI_DIS_PROTOCOL_AND_RELEASE_LINK, strlen(APP_VA_XIAOWEI_DIS_PROTOCOL_AND_RELEASE_LINK));
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "1", 1)) {
                flag = XIAOWEI_CONNECT_CONTROL_FLAG_DISCONNECT_BUT_KEEP_LINK;
                memcpy(response->response_buf, APP_VA_XIAOWEI_DIS_PROTOCOL_BUT_KEEP_LINK, strlen(APP_VA_XIAOWEI_DIS_PROTOCOL_BUT_KEEP_LINK));
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "2", 1)) {
                flag = XIAOWEI_CONNECT_CONTROL_FLAG_RECONNECT;
                memcpy(response->response_buf, APP_VA_XIAOWEI_RECONNECT_PROTOCOL, strlen(APP_VA_XIAOWEI_RECONNECT_PROTOCOL));
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "3", 1)) {
                flag = XIAOWEI_CONNECT_CONTROL_FLAG_RELEASE_LINK;
                memcpy(response->response_buf, APP_VA_XIAOWEI_RELEASE_LINK, strlen(APP_VA_XIAOWEI_RELEASE_LINK));
            } else {
                APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Unknown ATCI command parameter", 0);
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                response->response_len = 0;
                atci_send_response(response);
                free(response);
                response = NULL;
                return ATCI_STATUS_OK;
            }
            xiaowei_control_connection(flag);
            response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
            response->response_len = strlen((char *)response->response_buf);
            atci_send_response(response);
            free(response);
            response = NULL;
        }
        break;
        default:{
            response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response->response_len = 0;
            atci_send_response(response);
            free(response);
            response = NULL;
            return ATCI_STATUS_OK;
        }
    }
    return ATCI_STATUS_OK;
}

/**
 * @brief The at command table which used to do connect control opeartion.
 */
static atci_cmd_hdlr_item_t app_va_xiaowei_activity_at_cmd_table[] = {
    {
        .command_head = "AT+XW_CT_CONN",    /**< AT command string. */
        .command_hdlr = app_va_xiaowei_activity_at_cmd_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

/**
 * @brief Init connect control AT CMD
 *
 */
void app_va_xiaowei_activity_at_cmd_init()
{
    atci_register_handler(app_va_xiaowei_activity_at_cmd_table, sizeof(app_va_xiaowei_activity_at_cmd_table) / sizeof(atci_cmd_hdlr_item_t));
}

/**-----------------------------------------------------------------------------**/
/**                  XIAOWEI PRIVATE FUNCTION IMPLEMENTATION                    **/
/**-----------------------------------------------------------------------------**/
xiaowei_record_type_t app_va_xiaowei_activity_mapping_record_type(apps_config_configurable_table_t *table, uint8_t table_count)
{
    bool start_notify_exist = false;
    bool start_exist = false;
    bool long_press_start_exist = false;
    bool long_press_stop_exist = false;

    uint8_t table_index = 0;

    for (table_index = 0; table_index < table_count; table_index ++) {
        if ((table[table_index].app_key_event == KEY_VA_XIAOWEI_START_NOTIFY)
                && (table[table_index].key_event == APPS_CONFIG_KEY_LONG_PRESS_2)) {
            start_notify_exist = true;
        }
        if ((table[table_index].app_key_event == KEY_VA_XIAOWEI_START)
                && (table[table_index].key_event == APPS_CONFIG_KEY_LONG_PRESS_RELEASE_2)) {
            start_exist = true;
        }

        if ((table[table_index].app_key_event == KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START)
                && (table[table_index].key_event == APPS_CONFIG_KEY_LONG_PRESS_1)) {
            long_press_start_exist = true;
        }
        if ((table[table_index].app_key_event == KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP)
                && (table[table_index].key_event == APPS_CONFIG_KEY_RELEASE)) {
            long_press_stop_exist = true;
        }
    }

    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Get configure table, start_notify_exist : %d, start_exist : %d, long_press_start_exist : %d, long_press_stop_exist : %d",
                    4, start_notify_exist, start_exist, long_press_start_exist, long_press_stop_exist);

    if (start_notify_exist != start_exist) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Configure table TAP support not match, assert", 2, start_notify_exist, start_exist);
        assert(false);
    }
    if (long_press_start_exist != long_press_stop_exist) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Configure table LONG-PRESS not match, assert", 2, long_press_start_exist, long_press_stop_exist);
        assert(false);
    }
    if ((start_notify_exist == true) && (start_exist == true)
            && (long_press_start_exist == true) && (long_press_stop_exist == true)) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Configure table support both TAP and LONG-PRESS, assert", 0);
        assert(false);
    }
    if ((start_notify_exist == false) && (start_exist == false)
            && (long_press_start_exist == false) && (long_press_stop_exist == false)) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Configure table support both NOT TAP and LONG-PRESS", 0);
        return XIAOWEI_RECORD_TYPE_UNKNOWN;
    }

    if (start_notify_exist == true) {
        return XIAOWEI_RECORD_TYPE_TAP;
    }
    if (long_press_start_exist == true) {
        return XIAOWEI_RECORD_TYPE_HOLD;
    }
    return XIAOWEI_RECORD_TYPE_UNKNOWN;
}

xiaowei_record_type_t app_va_xiaowei_activity_get_record_type()
{
    uint8_t local_configure_table_len = 0;
    uint8_t peer_configure_table_len = 0;

    xiaowei_record_type_t local_support_type = XIAOWEI_RECORD_TYPE_TAP;
    xiaowei_record_type_t peer_support_type = XIAOWEI_RECORD_TYPE_TAP;
    xiaowei_record_type_t return_type = XIAOWEI_RECORD_TYPE_TAP;

    const apps_config_configurable_table_t *local_configure_table = apps_config_get_local_key_configure_table(&local_configure_table_len);
    apps_config_configurable_table_t *peer_configure_table = apps_config_get_peer_key_configure_table(&peer_configure_table_len);

    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Get configure table, local : 0x%x, local_length : %d, peer : 0x%x, peer_length : %d",
                    4, local_configure_table, local_configure_table_len, peer_configure_table, peer_configure_table_len);

    if ((local_configure_table == NULL) && (peer_configure_table == NULL)) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Both local and peer key configure table are NULL, return TAP record type", 0);
        return_type = XIAOWEI_RECORD_TYPE_TAP;
        goto final;
    }
    if ((local_configure_table_len < 2) && (peer_configure_table_len < 2)) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Both local and peer key configure table length < 2 (XIAOWEI key configure table length MUST NOT smaller than 2)", 0);
        return_type = XIAOWEI_RECORD_TYPE_TAP;
        goto final;
    }

    if ((local_configure_table != NULL) && (local_configure_table_len >= 2)) {
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Start to mapping local configure table", 0);
        local_support_type = app_va_xiaowei_activity_mapping_record_type(local_configure_table, local_configure_table_len);
    } else {
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (ERROR CONFIGURATION) Local configure table : 0x%x, length : %d",
                            2, local_configure_table, local_configure_table_len);
        local_support_type = XIAOWEI_RECORD_TYPE_UNKNOWN;
    }
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Local configured record type : %d", 1, local_support_type);

    if (local_support_type == XIAOWEI_RECORD_TYPE_UNKNOWN) {
        if (peer_configure_table != NULL && peer_configure_table_len > 2) {
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Start to mapping peer configure table", 0);
            peer_support_type = app_va_xiaowei_activity_mapping_record_type(peer_configure_table, peer_configure_table_len);
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Peer configured record type : %d", 1, peer_support_type);

            if (peer_support_type == XIAOWEI_RECORD_TYPE_UNKNOWN) {
                return_type = XIAOWEI_RECORD_TYPE_TAP;
                goto final;
            } else {
                return_type = peer_support_type;
                goto final;
            }
        } else {
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (ERROR CONFIGURATION) Peer configure table : 0x%x, length : %d",
                                2, peer_configure_table, peer_configure_table_len);
            return_type = XIAOWEI_RECORD_TYPE_TAP;
            goto final;
        }
    } else {
        return_type = local_support_type;
        goto final;
    }

final:
    if (peer_configure_table != NULL) {
        free(peer_configure_table);
        peer_configure_table = NULL;
    }
    return return_type;
}

static bool app_va_xiaowei_bt_takeover_handler(const bt_bd_addr_t remote_addr)
{
    bool ret = true;

    if (va_xiaowei_status >= XIAOWEI_STATUS_RECORDING
        && va_xiaowei_status < XIAOWEI_STATUS_SPEAKING) {
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", cur addr takeover not allow, state %d", 1, va_xiaowei_status);
        ret = false;
    }

    return ret;
}

/**
 * @brief APP level init xiaowei
 */
void app_va_xiaowei_activity_init()
{
    xiaowei_error_code_t err_code = XIAOWEI_ERROR_CODE_OK;

    xiaowei_callback_t callback;

    callback.on_xiaowei_avrcp_request = app_va_xiaowei_activity_cb_avrcp_request;
    callback.on_xiaowei_key_function_changed = app_va_xiaowei_activity_cb_key_function_changed;
    callback.on_xiaowei_retrieve_customer_name = app_va_xiaowei_activity_cb_retrieve_customer_name;
    callback.on_xiaowei_retrieve_firmware_version = app_va_xiaowei_activity_cb_retrieve_firmware_version;
    callback.on_xiaowei_retrieve_model_name = app_va_xiaowei_activity_cb_retrieve_model_name;
    callback.on_xiaowei_retrieve_skuid = app_va_xiaowei_activity_cb_retrieve_skuid;
    callback.on_xiaowei_status_changed = app_va_xiaowei_activity_cb_status_changed;
    callback.on_xiaowei_retrieve_serial_number = app_va_xiaowei_activity_cb_retrieve_serial_number;
    callback.on_xiaowei_retrieve_keep_connection = app_va_xiaowei_activity_cb_retrieve_keep_connection;
    callback.on_xiaowei_device_config = app_va_xiaowei_activity_cb_handle_device_config;
    callback.on_xiaowei_execute_custom_skill = app_va_xiaowei_activity_cb_execute_customer_skill;
    callback.on_xiaowei_connect_control_reconnect_app_connected_with_other_device = app_va_xiaowei_activity_cb_connect_control_reconnect_app_connected_with_other_device;
    callback.on_xiaowei_tts_play_finish = app_va_xiaowei_activity_cb_tts_play_finished;

    xiaowei_init_param_t param;

    bt_bd_addr_t *local_addr;
#ifdef MTK_AWS_MCE_ENABLE
    /* To fix the BT name change after RHO. */
    if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()) {
        local_addr = bt_device_manager_aws_local_info_get_peer_address();
    } else
#endif
    {
        local_addr = bt_device_manager_get_local_address();
    }

    param.company_id = VA_XIAOWEI_CUSTOMER_CONFIG_COMPANY_ID;
    param.product_id = VA_XIAOWEI_CUSTOMER_CONFIG_PRODUCT_ID;
    memcpy(param.mac_addr, *local_addr, 6);
    /**
     * @brief Configure the record type according to the key configuration
     */
    param.record_type = app_va_xiaowei_activity_get_record_type();
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Init xiaowei, configure record type to be : %d", 1, param.record_type);
    param.key_func = XIAOWEI_KEY_FUNCTION_XIAOWEI;

    va_xiaowei_status = XIAOWEI_STATUS_NONE;

    /**
     * @brief Init xiaowei library.
     */
    err_code = xiaowei_init(&param, &callback);
    if (err_code != XIAOWEI_ERROR_CODE_OK) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Init xiaowei failed, 0x%x", 1, err_code);
    } else {
        xiaowei_init_done = true;
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Init xiaowei succeed", 0);

        /**
         * @brief For testing code, should be removed
         */
        app_va_xiaowei_activity_at_cmd_init();
    }

    app_bt_takeover_service_user_register(APP_BT_TAKEOVER_ID_XIAOWEI, app_va_xiaowei_bt_takeover_handler);
}

/**
 * @brief Proc ui shell group event.
 *
 * @param self
 * @param event_id
 * @param extra_data
 * @param data_len
 * @return true
 * @return false
 */
bool app_va_xiaowei_activity_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", CREATE", 0);
        }
        break;

        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", DESTROY", 0);
            xiaowei_deinit();
        }
        break;

        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", RESUME", 0);
            break;

        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", PAUSE", 0);
            break;

        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", REFRESH", 0);
            break;

        case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", RESULT", 0);
            break;
    }

    return ret;
}

/**
 * @brief Key event handler
 *
 * @param key_id The key ID to handle
 */
bool app_va_xiaowei_activity_handle_key_event(uint16_t key_id)
{

#if defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2) && defined(MTK_ANC_ENABLE)
    if (app_adaptive_anc_get_state() != APP_ADAPTIVE_ANC_IDLE) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Key processing, recorder is using by adaptive ANC, cannot trigger xiaowei", 0);
        return false;
    }
#endif /* MTK_USER_TRIGGER_ADAPTIVE_FF_V2 */

    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", key processing, role : 0x%x, Key event ID : 0x%04x, current xiaowei status 0x%x, key function : %d",
                        4, role, key_id, va_xiaowei_status, key_func);

    if ((key_id != KEY_VA_XIAOWEI_START_NOTIFY)
            && (key_id != KEY_VA_XIAOWEI_START)
            && (key_id != KEY_VA_XIAOWEI_STOP_PLAY)
            && (key_id != KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START)
            && (key_id != KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP)) {
        /* Current is not the xiaowei key event. */
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (role == BT_AWS_MCE_ROLE_PARTNER) {
        if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, key_id)) {
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Partner key event (0x%04x) to agent failed", 1, key_id);
        } else {
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Partner key event (0x%04x) to agent succeed", 1, key_id);
        }
        return true;
    } else
#endif /* MTK_AWS_MCE_ENABLE */
    {
        /**
         * @brief If current is not ready to trigger xiaowei, play VP
         */
        uint32_t connected_count = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP), NULL, 0);
        APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", key processing, connected SP count : %d", 1, connected_count);

        /**
         * @brief If the key function is error and the key ID is start notify or long press trigger start
         */
        if (key_func != XIAOWEI_KEY_FUNCTION_XIAOWEI) {
            if ((key_id == KEY_VA_XIAOWEI_START_NOTIFY) || (key_id == KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Play xiaowei trigger failed VP (KEY_FUNC ERROR)", 0);
                apps_config_set_vp(VP_INDEX_DOORBELL, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            return true;
        }
        /**
         * @brief If the connected smart phone is 0, notify VP
         */
        if (connected_count == 0) {
            if ((key_id == KEY_VA_XIAOWEI_START_NOTIFY) || (key_id == KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Play xiaowei trigger failed VP (CONNECTED_SP_COUNT 0)", 0);
                apps_config_set_vp(VP_INDEX_DOORBELL, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            return true;
        }

        /**
         * @brief If not ready to trigger xiaowei
         */
        if (xiaowei_is_ready_to_trigger() == false) {
            if ((key_id == KEY_VA_XIAOWEI_START_NOTIFY) || (key_id == KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START)) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Play xiaowei trigger failed VP (NOT_READY_TO_TRIGGER)", 0);
                apps_config_set_vp(VP_INDEX_DOORBELL, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                return true;
            }
            if (key_id == KEY_VA_XIAOWEI_START) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Current status is not ready to trigger xiaowei", 0);
                return true;
            }
        }

        switch (key_id) {
            case KEY_VA_XIAOWEI_STOP_PLAY: {
                /* Stop the current playing (including TTS, music...). */
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Stop current play", 0);
                xiaowei_stop_play();
            }
            break;

            case KEY_VA_XIAOWEI_START_NOTIFY: {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (TAP) Start notify", 0);
                apps_config_set_vp(VP_INDEX_PRESS, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            break;

            case KEY_VA_XIAOWEI_START: {
                /* Start to xiaowei recognizing flow. */
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (TAP) Start recognizing", 0);
                xiaowei_start_recognizing();
            }
            break;

            case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START: {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (Long-press) Start recognizing", 0);
                apps_config_set_vp(VP_INDEX_PRESS, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                xiaowei_start_recognizing();
            }
            break;

            case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP: {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (Long-press) Stop recognizing", 0);
                xiaowei_long_press_released();
            }
            break;

            default: {
                return false;
            }
        }
    }

    return true;
}

/**
 * @brief Key event handler
 *
 * @param self
 * @param event_id
 * @param extra_data
 * @param data_len
 */
bool app_va_xiaowei_activity_proc_key_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;

    uint16_t key_id = *(uint16_t *)extra_data;

    return app_va_xiaowei_activity_handle_key_event(key_id);
}

bool app_va_xiaowei_activity_proc_bt_sink_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    return false;
}

/**
 * @brief BT connection manager event handler
 *
 * @param self
 * @param event_id
 * @param extra_data
 * @param data_len
 */
bool app_va_xiaowei_activity_proc_bt_cm_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_CM_EVENT_POWER_STATE_UPDATE: {
            bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)extra_data;
            if (power_update) {
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", [BT POWER STATE] Current state : 0x%x, xiaowei_init_done : %d",
                                    2, power_update->power_state, xiaowei_init_done);

                /* Init xiaowei when BT power on and deinit it after BT power off. */
                if ((power_update->power_state == BT_CM_POWER_STATE_ON)
                        && (xiaowei_init_done == false)) {

                    multi_va_type_t va_type = multi_va_manager_get_current_va_type();
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", [BT POWER ON] Current VA type : %d", 1, va_type);

#ifdef MULTI_VA_SUPPORT_COMPETITION
                    app_va_xiaowei_activity_init();
#else
                    if (va_type == MULTI_VA_TYPE_XIAOWEI) {
                        app_va_xiaowei_activity_init();
                    }
#endif /* MULTI_VA_SUPPORT_COMPETITION */
                } else if (power_update->power_state == BT_CM_POWER_STATE_OFF) {
                    if (xiaowei_init_done == true) {
                        xiaowei_deinit();
                        va_xiaowei_status = XIAOWEI_STATUS_NONE;
                        xiaowei_init_done = false;
                    }
                }
            }
        }
        break;
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (remote_update == NULL) {
                APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", [REMOTE INFO UPDATE] remote_update is null", 0);
                return false;
            }

            bool pre_hfp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP) & remote_update->pre_connected_service) > 0);
            if (!pre_hfp_conn) {
                pre_hfp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HSP) & remote_update->pre_connected_service) > 0);
            }

            bool cur_hfp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)) & remote_update->connected_service) > 0;
            if (!cur_hfp_conn) {
                cur_hfp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HSP) & remote_update->connected_service) > 0);
            }

            bool pre_a2dp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->pre_connected_service) > 0);
            bool cur_a2dp_conn = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->connected_service) > 0);

            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", [REMOTE INFO UPDATE] acl=%d->%d srv=0x%08X->0x%08X hfp=%d->%d a2dp=%d->%d", 8,
                                remote_update->pre_acl_state, remote_update->acl_state,
                                remote_update->pre_connected_service, remote_update->connected_service,
                                pre_hfp_conn, cur_hfp_conn, pre_a2dp_conn, cur_a2dp_conn);

            if (!cur_hfp_conn && !cur_a2dp_conn && (pre_hfp_conn || pre_a2dp_conn)) {
                /**
                 * @brief If A2DP/HFP disconnected, disconnect xiaowei link
                 */
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", [REMOTE INFO UPDATE] A2DP and HFP disconnected, Disconnect xiaowei link", 0);
                xiaowei_disconnect();
            }
        }
        break;
        default:
            break;
    }
    return false;
}

/**
 * @brief The xiaowei VA event handler
 *
 * @param self
 * @param event_id
 * @param extra_data
 * @param data_len
 */
bool app_va_xiaowei_activity_proc_va(ui_shell_activity_t *self,
                                  uint32_t event_id,
                                  void *extra_data,
                                  size_t data_len)
{
    static bool va_notify = false;
    switch (event_id) {
        case XIAOWEI_STATUS_READY: {
            if (va_notify == false) {
                /* When Xiaowei is ready, the multi voice assistant manager will switch context to Xiaowei. */
                multi_voice_assistant_manager_notify_va_connected(MULTI_VA_TYPE_XIAOWEI);
                va_notify = true;
            }
        }
        break;
        case XIAOWEI_STATUS_DISCONNECTED:
        case XIAOWEI_STATUS_NONE: {
            va_notify = false;
            multi_voice_assistant_manager_notify_va_disconnected(MULTI_VA_TYPE_XIAOWEI);
        }
        break;
        case XIAOWEI_STATUS_RECORDING: {
            /* Start Xiaowei transient activity when Xiaowei is recording. */
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Start Xiaowei transient activity", 0);
            ui_shell_start_activity(self, app_va_xiaowei_transient_activity_proc, ACTIVITY_PRIORITY_HIGH, NULL, 0);
        }
        break;
    }
    return true;
}

#ifdef MTK_AWS_MCE_ENABLE
/**
 * @brief The AWS MCE data event handler
 *
 * @param self
 * @param event_id
 * @param extra_data
 * @param data_len
 */
bool app_va_xiaowei_activity_proc_aws_data(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;

        apps_aws_sync_event_decode(aws_data_ind, &event_group, &action);

        /* Handle the key event come from partner. */
        if (event_group == EVENT_GROUP_UI_SHELL_KEY && role == BT_AWS_MCE_ROLE_AGENT) {
            return app_va_xiaowei_activity_handle_key_event(action);
        }
    }

    return ret;
}
#endif

/**
 * @brief The activity event handler
 *
 * @param self
 * @param event_group
 * @param event_id
 * @param extra_data
 * @param data_len
 */
bool app_va_xiaowei_activity_proc(struct _ui_shell_activity *self,
                                  uint32_t event_group,
                                  uint32_t event_id,
                                  void *extra_data,
                                  size_t data_len)
{

    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            /* UI Shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = app_va_xiaowei_activity_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_KEY:
            /* key event. */
            ret = app_va_xiaowei_activity_proc_key_event(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_BT_SINK:
            /* BT_SINK events. */
            app_va_xiaowei_activity_proc_bt_sink_event(self, event_id, extra_data, data_len);
            return false;

        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            /* The event come from bt connection manager, indicates the power state of BT. */
            app_va_xiaowei_activity_proc_bt_cm_event(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_VA_XIAOWEI:
            /* The event come from Xiaowei middleware, indicates the state of Xiaowei. */
            ret = app_va_xiaowei_activity_proc_va(self, event_id, extra_data, data_len);
            break;

#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            /* The event come from partner. */
            ret = app_va_xiaowei_activity_proc_aws_data(self, event_id, extra_data, data_len);
            break;
#endif
    }
    return ret;
}

#endif /* MTK_VA_XIAOWEI_ENABLE */
