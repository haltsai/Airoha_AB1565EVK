
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

#define APP_VA_XIAOWEI_PREFIX "[VA_XIAOWEI] VA_XIAOWEI_ACTIVITY"

xiaowei_status_t va_xiaowei_status = XIAOWEI_STATUS_NONE;
xiaowei_key_function_t key_func = XIAOWEI_KEY_FUNCTION_XIAOWEI;

bool xiaowei_init_done = false;

void va_xiaowei_app_activity_status_changed(xiaowei_status_t new_status)
{
    va_xiaowei_status = new_status;
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Update current status to be : 0x%x", 1, va_xiaowei_status);

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

void va_xiaowei_app_activity_key_function_changed(xiaowei_key_function_t new_function)
{
    key_func = new_function;
    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Change key function to be : %d", 1, key_func);
}

void va_xiaowei_app_activity_avrcp_request(xiaowei_avrcp_request_code_t code)
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

void va_xiaowei_app_activity_retrieve_firmware_version(uint8_t *version)
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

void va_xiaowei_app_activity_retrieve_model_name(uint8_t **model_name, uint8_t *model_name_len)
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

void va_xiaowei_app_activity_retrieve_customer_name(uint8_t **customer_name, uint8_t *customer_name_len)
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

void va_xiaowei_app_activity_retrieve_skuid(uint8_t *skuid)
{
    /* Fill in the device SKUID. */
    *skuid = VA_XIAOWEI_CUSTOMER_CONFIG_SKUID;
}

void app_va_xiaowei_init()
{
    xiaowei_error_code_t err_code = XIAOWEI_ERROR_CODE_OK;

    xiaowei_callback_t callback;

    callback.on_xiaowei_avrcp_request = va_xiaowei_app_activity_avrcp_request;
    callback.on_xiaowei_key_function_changed = va_xiaowei_app_activity_key_function_changed;
    callback.on_xiaowei_retrieve_customer_name = va_xiaowei_app_activity_retrieve_customer_name;
    callback.on_xiaowei_retrieve_firmware_version = va_xiaowei_app_activity_retrieve_firmware_version;
    callback.on_xiaowei_retrieve_model_name = va_xiaowei_app_activity_retrieve_model_name;
    callback.on_xiaowei_retrieve_skuid = va_xiaowei_app_activity_retrieve_skuid;
    callback.on_xiaowei_status_changed = va_xiaowei_app_activity_status_changed;

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
#ifndef MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD
    param.record_type = XIAOWEI_RECORD_TYPE_TAP;
#else
    param.record_type = XIAOWEI_RECORD_TYPE_HOLD;
#endif
    param.key_func = XIAOWEI_KEY_FUNCTION_XIAOWEI;

    va_xiaowei_status = XIAOWEI_STATUS_NONE;

    err_code = xiaowei_init(&param, &callback);
    if (err_code != XIAOWEI_ERROR_CODE_OK) {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Init xiaowei failed, 0x%x", 1, err_code);
    } else {
        APPS_LOG_MSGID_E(APP_VA_XIAOWEI_PREFIX", Init xiaowei succeed", 0);
    }
}

bool va_xiaowei_app_activity_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
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

bool va_xiaowei_app_activity_proc_key_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;

    uint16_t key_id = *(uint16_t *)extra_data;

    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", key event processing, role : 0x%x, Key event ID : 0x%04x, current xiaowei status 0x%x, key function : %d", 4, role, key_id, va_xiaowei_status, key_func);

#ifndef MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD
    if (key_id != KEY_VA_XIAOWEI_START_NOTIFY && key_id != KEY_VA_XIAOWEI_START && key_id != KEY_VA_XIAOWEI_STOP_PLAY) {
        /* Current is not the xiaowei key event. */
        return false;
    }
#else
    if (key_id != KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START && key_id != KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP && key_id != KEY_VA_XIAOWEI_STOP_PLAY) {
        /* Current is not the xiaowei key event. */
        return false;
    }
#endif /* MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD */

    if ((va_xiaowei_status < XIAOWEI_STATUS_READY) || (key_func != XIAOWEI_KEY_FUNCTION_XIAOWEI)) {
        return false;
    }

    switch (key_id) {
        case KEY_VA_XIAOWEI_STOP_PLAY: {
#ifdef MTK_AWS_MCE_ENABLE
            /* If current support AWS feature, and current role is partner, need send the key event to agent. */
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, key_id)) {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Partner send stop play key event to agent failed", 0);
                } else {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Partner send stop play key event to agent succeed", 0);
                }
            } else
#endif /* MTK_AWS_MCE_ENABLE */
            {
                /* Stop the current playing (including TTS, music...). */
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Stop current play (Trigger from agent)", 0);
                xiaowei_stop_play();
            }
            ret = true;
        }
        break;

#ifndef MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD
        case KEY_VA_XIAOWEI_START_NOTIFY: {
            /* When pressed time is enough to trigger xiaowei, play the VP. */
            APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Start to notify trigger", 0);
            apps_config_set_vp(VP_INDEX_PRESS, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);

            ret = true;
        }
        break;

        case KEY_VA_XIAOWEI_START: {
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                /**
                 * @brief If support AWS, and current role is partner.
                 * Need send command to agent to execute the key event.
                 */
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, key_id)) {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (TAP) Partner send start key event to agent failed", 0);
                } else {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (TAP) Partner send start key event to agent succeed", 0);
                }
            } else
#endif /* MTK_AWS_MCE_ENABLE */
            {
                /* Start to xiaowei recognizing flow. */
                APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (TAP) Start recognizing flow (Trigger from agent)", 0);
                xiaowei_start_recognizing();
            }

            ret = true;
        }
        break;

#else /* MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD */
        case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START:
        case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP: {
            if (key_id == KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START) {
                apps_config_set_vp(VP_INDEX_PRESS, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }

#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                /**
                 * @brief If support AWS, and current role is partner
                 * Need send command to agent to execute the key event.
                 */
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, key_id)) {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (Long-press) Partner send key event 0x%x to agent failed", 1, key_id);
                } else {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (Long-press) Partner send key event 0x%x to agent succeed", 1, key_id);
                }
            } else
#endif
            {
                if (key_id == KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START) {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (Long-press) Start recognizing (Trigger from agent)", 0);
                    xiaowei_start_recognizing();
                } else {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (Long-press) Stop recognizing (Trigger from agent)", 0);
                    xiaowei_long_press_released();
                }
            }

            ret = true;
        }
        break;
#endif /* MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD */

        default: {
            ret = false;
        }
        break;
    }

    return ret;
}

bool app_va_xiaowei_bt_sink_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    return false;
}

bool app_va_xiaowei_bt_cm_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_CM_EVENT_POWER_STATE_UPDATE:
            bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)extra_data;
            if (power_update) {
                /* Init xiaowei when BT power on and deinit it after BT power off. */
                if (power_update->power_state) {
                    if (xiaowei_init_done == false) {
                        app_va_xiaowei_init();
                        xiaowei_init_done = true;
                    }
                } else {
                    if (xiaowei_init_done == true) {
                        xiaowei_deinit();
                        va_xiaowei_status = XIAOWEI_STATUS_NONE;
                        xiaowei_init_done = false;
                    }
                }
            }
            break;
        default:
            break;
    }
    return false;
}

bool app_xiaowei_activity_proc_va(ui_shell_activity_t *self,
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
        case XIAOWEI_STATUS_DISCONNECTED: {
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
bool va_xiaowei_app_activity_proc_aws_data(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
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
            switch (action) {
                case KEY_VA_XIAOWEI_STOP_PLAY: {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", Stop current play (Trigger from partner)", 0);
                    xiaowei_stop_play();

                    ret = true;
                }
                break;

#ifndef MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD
                case KEY_VA_XIAOWEI_START: {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (TAP) Start to executing recognizing (Trigger from partner)", 0);
                    xiaowei_start_recognizing();

                    ret = true;
                }
                break;
#else /* MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD */
                case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START: {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (LONG-PRESS) Start to executing recognizing (Trigger from partner)", 0);
                    xiaowei_start_recognizing();

                    ret = true;
                }
                break;

                case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP: {
                    APPS_LOG_MSGID_I(APP_VA_XIAOWEI_PREFIX", (LONG-PRESS) Stop recognizing (Trigger from partner)", 0);
                    xiaowei_long_press_released();

                    ret = true;
                }
                break;
#endif /* MTK_VA_XIAOWEI_TRIGGER_MODE_HOLD */
            }
        }
    }

    return ret;
}
#endif

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
            ret = va_xiaowei_app_activity_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_KEY:
            /* key event. */
            ret = va_xiaowei_app_activity_proc_key_event(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_BT_SINK:
            /* BT_SINK events. */
            app_va_xiaowei_bt_sink_event_proc(self, event_id, extra_data, data_len);
            return false;

        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            /* The event come from bt connection manager, indicates the power state of BT. */
            app_va_xiaowei_bt_cm_event_proc(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_VA_XIAOWEI:
            /* The event come from Xiaowei middleware, indicates the state of Xiaowei. */
            ret = app_xiaowei_activity_proc_va(self, event_id, extra_data, data_len);
            break;

#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            /* The event come from partner. */
            ret = va_xiaowei_app_activity_proc_aws_data(self, event_id, extra_data, data_len);
            break;
#endif
    }
    return ret;
}

#endif /* MTK_VA_XIAOWEI_ENABLE */
