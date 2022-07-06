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

#include "app_ama_idle_activity.h"
#include "apps_config_event_list.h"
#include "apps_aws_sync_event.h"
#include "apps_events_event_group.h"
#include "apps_config_key_remapper.h"
#include "apps_events_key_event.h"
#include "bt_init.h"
#include "apps_config_vp_index_list.h"
#include "app_voice_prompt.h"
#include "multi_va_manager.h"
#include "app_ama_activity.h"

#ifdef MTK_VA_MODEL_MANAGER_ENABLE
#include "va_model_manager.h"
#endif /* MTK_VA_MODEL_MANAGER_ENABLE */

/* Alexa APP Launch Feature. */
#if defined(AMA_IAP2_VIA_MUX_ENABLE) && defined(MTK_MUX_ENABLE)
#include "mux_iap2.h"
#define AMA_IAP2_LAUNCH_APP_ID      "com.amazon.echo"
#endif

#ifdef MTK_AMA_ENABLE

bool app_start_ama_activity()
{
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;
    ret = ui_shell_start_activity(NULL, app_ama_activity_proc, ACTIVITY_PRIORITY_MIDDLE, NULL, 0);
    return ((ret == UI_SHELL_STATUS_OK) ? true : false);
}

/* Alexa APP Launch Feature. */
static void app_ama_idle_launch_app()
{
#if defined(AMA_IAP2_VIA_MUX_ENABLE) && defined(MTK_MUX_ENABLE)

#ifdef MTK_AWS_MCE_ENABLE
        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT)
#endif /* MTK_AWS_MCE_ENABLE */
        {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_launch_app, Send app launch request", 0);
            /* Only launch Alexa app without alert. */
            mux_iap2_send_app_launch_request(AMA_IAP2_LAUNCH_APP_ID, FALSE);
        }
#endif
}

static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true; // UI shell internal event must process by this activity, so default is true
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", create", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

/**
 * @brief Fix issue : BTA-8751
 * Add a new function to handle the event from key_proc and the AWS
 * @param key_event The key event ID.
 * @return true
 * @return false
 */
static bool app_ama_idle_activity_process_key_event(apps_config_key_action_t key_event)
{
    bool ret = false;

    APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_process_key_event, key_event : 0x%04x", 1, key_event);
    if ((key_event != KEY_AMA_START_NOTIFY) && (key_event != KEY_AMA_START)
            && (key_event != KEY_AMA_LONG_PRESS_TRIGGER_START) && (key_event != KEY_AMA_LONG_PRESS_TRIGGER_STOP)) {
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
        if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, key_event)) {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_process_key_event, Partner send key event to agent failed", 0);
        } else {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_process_key_event, Partner send key event to agent success", 0);
        }
        return true;
    }
#endif /* MTK_AWS_MCE_ENABLE */

    if ((key_event == KEY_AMA_LONG_PRESS_TRIGGER_START || key_event == KEY_AMA_START_NOTIFY)
            && (app_ama_is_connected() == false)) {
        APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_process_key_event, AMA not connected, play VP", 0);
        apps_config_set_vp(VP_INDEX_DOORBELL, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        app_ama_idle_launch_app();
    }

    return true;
}

static bool _proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    uint8_t key_id;
    airo_key_event_t key_event;
    apps_config_key_action_t action;

    app_event_key_event_decode(&key_id, &key_event, event_id);

    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }
    APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", key_id : 0x%x, key_event : 0x%x, event_id : 0x%x, action : 0x%x", 4, key_id, key_event, event_id, action);

    ret = app_ama_idle_activity_process_key_event(action);

    return ret;
}

#if defined(MTK_AWS_MCE_ENABLE)
bool app_ama_idle_activity_process_aws_data_proc(ui_shell_activity_t *self, uint32_t ori_event_id, void *extra_data, size_t ori_data_len)
{
    bool ret = false;

    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;


    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group = 0;
        uint32_t event_id = 0;
        void *data = NULL;
        uint32_t data_len = 0;
        uint8_t expected_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event_id, &data, &data_len);

        if (event_group == EVENT_GROUP_UI_SHELL_KEY
                    && ((event_id == KEY_AMA_START_NOTIFY)
                            || (event_id == KEY_AMA_START)
                            || (event_id == KEY_AMA_LONG_PRESS_TRIGGER_START)
                            || (event_id == KEY_AMA_LONG_PRESS_TRIGGER_STOP))) {
            ret = app_ama_idle_activity_process_key_event(event_id);
        }
    }
    return ret;
}
#endif

void app_ama_idle_activity_handle_ama_event(uint32_t event_id, void *extra_data)
{
    if (extra_data == NULL) {
        APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, Extra_data is NULL", 0);
        return;
    }

    switch (event_id) {
        case AMA_START_SERVICE_CFM:
        {
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, Start service success", 0);
        }
        break;
        case AMA_CONNECT_CFM:
        case AMA_CONNECT_CFM_WITH_LOCALE_IND:
        {
            uint8_t addr[6] = {0};

            if (event_id == AMA_CONNECT_CFM) {
                AMA_CONNECT_CFM_T *cnf = (AMA_CONNECT_CFM_T *)extra_data;
                memcpy(addr, cnf->bdAddr.addr, 6);
            } else {
                AMA_CONNECT_CFM_WITH_LOCALE_IND_T *ind = (AMA_CONNECT_CFM_WITH_LOCALE_IND_T *)extra_data;
                memcpy(addr, ind->bdAddr.addr, 6);
            }

            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_CONNECT_CFM/WITH_LOCALE_IND, Connected address : 0x%x:%x:%x:%x:%x:%x", 6,
                            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

            if (app_ama_is_valid_address(addr) == false) {
                APPS_LOG_MSGID_E(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_CONNECT_CFM/WITH_LOCALE_IND, address is INVALID", 0);
                return;
            }

            app_ama_set_connected_address(addr);

            /**
             * @brief For RHO case, which AMA lib notify that the connected cfm message
             * with locale information, can should set to activity.
             */
            if (event_id == AMA_CONNECT_CFM_WITH_LOCALE_IND) {
                APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_CONNECT_CFM_WITH_LOCALE_IND, Configure WWE locale", 0);
                AMA_CONNECT_CFM_WITH_LOCALE_IND_T *ind = (AMA_CONNECT_CFM_WITH_LOCALE_IND_T *)extra_data;
                app_ama_configure_wwe_parameter(ind->locale.name);
            }

            bool start_activity_result = app_start_ama_activity();
            if (start_activity_result == false) {
                APPS_LOG_MSGID_E(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_CONNECT_CFM, Start activity failed", 0);
                app_ama_reset_connected_address();
            }
        }
        break;
        case AMA_GET_LOCALES_IND:
        {
            AMA_GET_LOCALES_IND_T *get_locale = (AMA_GET_LOCALES_IND_T *)extra_data;
            if (get_locale == NULL) {
                APPS_LOG_MSGID_E(APP_IDLE_AMA_ACTIVITY",app_ama_idle_activity_handle_ama_event AMA_GET_LOCALES_IND, get_locale is NULL", 0);
                return;
            }
            APPS_LOG_MSGID_I(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_GET_LOCALES_IND enter", 0);
            app_ama_handle_get_locales_ind(get_locale);
        }
        break;
        case AMA_SET_LOCALE_IND:
        {
            APPS_LOG_MSGID_E(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_SET_LOCALE_IND", 0);
            AMA_SET_LOCALE_IND_T *locale = (AMA_SET_LOCALE_IND_T *)extra_data;
            if (locale == NULL) {
                APPS_LOG_MSGID_E(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_SET_LOCALE_IND, locale is NULL", 0);
                return;
            }
            app_ama_handle_set_locale_ind(locale);
        }
        break;
        case AMA_GET_STATE_IND:
        {
            APPS_LOG_MSGID_E(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_GET_STATE_IND", 0);
            app_ama_get_state_ind_handler((AMA_GET_STATE_IND_T *)extra_data);
        }
        break;
        case AMA_SYNCHRONIZE_STATE_IND:
        {
            APPS_LOG_MSGID_E(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, AMA_SYNCHRONIZE_STATE_IND", 0);
            AMA_SYNCHRONIZE_STATE_IND_T * ind = (AMA_SYNCHRONIZE_STATE_IND_T *)extra_data;
            AMA_Target_SynchronizeStateResponse(&ind->bdAddr, AMA_ERROR_CODE_SUCCESS);
        }
        break;
        default:
        {
            APPS_LOG_MSGID_E(APP_IDLE_AMA_ACTIVITY", app_ama_idle_activity_handle_ama_event, DEFAULT, no need to handle", 0);
        }
        break;
    }
}

bool app_ama_idle_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_AMA: {
            app_ama_idle_activity_handle_ama_event(event_id, extra_data);
            ret = true;
        }
        break;

#if defined(MTK_AWS_MCE_ENABLE)
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            ret = app_ama_idle_activity_process_aws_data_proc(self, event_id, extra_data, data_len);
            break;
        }
#endif
        default:
            break;
    }

    return ret;
}

#endif
