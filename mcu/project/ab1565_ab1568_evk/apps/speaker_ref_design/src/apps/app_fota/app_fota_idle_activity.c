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
 * File: app_fota_idle_activity.c
 *
 * Description: This file is the activity to handle fota state and request reboot.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#include "app_fota_idle_activity.h"
#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "race_app_race_event_hdl.h"
#include "race_app_bt_event_hdl.h"
#include "race_app_aws_event_hdl.h"
#include "bt_device_manager.h"


#define APP_IDLE_FOTA_ACTI "app_idle_fota_activity"

/**
 *  @brief This structure defines the context of app_fota_idle_activity.
 */
typedef struct {
    app_fota_state_t pre_state;    /**<  Record the previous fota state. */
    app_fota_state_t curr_state;   /**<  Record the current fota state. */
} app_fota_context_t;

static app_fota_context_t s_app_fota_context;   /* The variable records context */
static bool s_device_busy = false;

/**
* @brief      This function gets if the device is busy,such as streaming or call active.
* @param[in]  is_busy is true : the device is busy.
*/
void race_get_device_busy_status(bool *is_busy)
{
    if (is_busy != NULL) {
        *is_busy = s_device_busy;
        APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI", current device is: %x", 1, *is_busy);
    }
}

static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len);

/**
* @brief      This function shows and updates the state of fota.
* @param[in]  self, the context pointer of the activity, can't be NULL.
* @param[in]  state, current state of fota.
*/
static void ui_shell_show_activity(struct _ui_shell_activity *self, app_fota_state_t state)
{
    APPS_LOG_MSGID_I("ui shell fota state :%d ", 1, state);
    app_fota_context_t *local_context = (app_fota_context_t *)self->local_context;
    /* Update context. */
    local_context->pre_state = local_context->curr_state;
    local_context->curr_state = state;
    switch (state) {
        case FOTA_IN_IDLE: {
            APPS_LOG_MSGID_I("FOTA IN IDLE", 0);
            break;
        }
        case  FOTA_IN_START: {
            APPS_LOG_MSGID_I("FOTA STARTING", 0);
            break;
        }
        case FOTA_IN_CANCELLING: {
            APPS_LOG_MSGID_I("FOTA CANCELLING", 0);
            break;
        }
        case FOTA_IN_CANCELLED: {
            APPS_LOG_MSGID_I("FOTA CANCELLED", 0);
            break;
        }
        case FOTA_IN_END: {
            APPS_LOG_MSGID_I("FOTA END", 0);
            break;
        }
        default:
            break;
    }
}


static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI": create", 0);
            if (self) {
                app_fota_context_t *local_context = NULL;
                self->local_context = &s_app_fota_context;
                local_context = (app_fota_context_t *)self->local_context;
                if (local_context) {
                    local_context->pre_state = FOTA_IN_IDLE;
                    local_context->curr_state = FOTA_IN_IDLE;
                }
                ui_shell_show_activity(self, FOTA_IN_IDLE);
            }
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI": destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI": resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI": pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI": refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI": result", 0);
            break;
        }
        default:
            ret = false;
            break;
    }
    return ret;
}

/**
* @brief      This function process the events of race fota.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool ui_shell_pro_fota_state(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI", event_id : %d, role = %x", 2, event_id, role);
#endif
    switch (event_id) {
        case RACE_EVENT_TYPE_FOTA_START: {
            ui_shell_show_activity(self, FOTA_IN_START);
#ifdef MTK_AWS_MCE_ENABLE
            if (role == BT_AWS_MCE_ROLE_AGENT)
#endif
            {
                /* Set VP and LED. */
                apps_config_set_vp(VP_INDEX_SUCCESSED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                apps_config_set_foreground_led_pattern(LED_INDEX_FOTA_START, 30, false);
            }
            break;
        }
        case RACE_EVENT_TYPE_FOTA_CANCELLING: {
            ui_shell_show_activity(self, FOTA_IN_CANCELLING);
            break;
        }
        case RACE_EVENT_TYPE_FOTA_CANCEL: {
            ui_shell_show_activity(self, FOTA_IN_CANCELLED);
#ifdef MTK_AWS_MCE_ENABLE
            if (role == BT_AWS_MCE_ROLE_AGENT)
#endif
            {
                /* Set LED. */
                apps_config_set_foreground_led_pattern(LED_INDEX_FOTA_CANCELLED, 30, false);
            }
            break;
        }
        case RACE_EVENT_TYPE_FOTA_NEED_REBOOT: {
            /* Send reboot request to homescreen app. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_REQUEST_REBOOT, NULL, 0,
                                NULL, 0);
            break;
        }
    }
    return ret;
}

static bool ui_shell_bt_fota_event_proc(ui_shell_activity_t *self,
                                uint32_t event_group,
                                uint32_t event_id,
                                void *extra_data,
                                size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
            bool test_device_is_busy = false;
            if (param == NULL) {
                APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI", state_change :param is null ", 0);
                return ret;
            }
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI", state_change: param_previous: %x, param_now: %x", 2, param->previous, param->current);
            /* Check the call status, if there are no HFP related states, finish current activity. */
            if ((param->current < BT_SINK_SRV_STATE_STREAMING) && (param->previous >= BT_SINK_SRV_STATE_STREAMING)) {
                /* Busy is end */
                s_device_busy = false;
                race_get_device_busy_status(&test_device_is_busy);
                APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI", test current device is: %x", 1,test_device_is_busy);
            } else {
                /*Device is busy*/
                s_device_busy = true;
                race_get_device_busy_status(&test_device_is_busy);
                APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI", test current device is: %x", 1,test_device_is_busy);
            }
            break;
        }
        default: {
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI", fota_event_proc:Don't care", 0);
            ret = false;
            break;
        }
    }
    return ret;
}

bool app_fota_idle_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            /* UI Shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_FOTA: {
            /* FOTA events sent by race_cmd_fota. */
            ret = ui_shell_pro_fota_state(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* BT sink events, notify race cmd. */
            APPS_LOG_MSGID_I(APP_IDLE_FOTA_ACTI", event_id : %x", 1, APP_FOTA_ACTI, event_id);
            ret = ui_shell_bt_fota_event_proc(self, event_group, event_id, extra_data, data_len);
            race_app_bt_sink_event_handler(event_id, extra_data);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            /* BT connection manager events, notify race cmd. */
            race_app_bt_cm_event_handler(event_id, extra_data);
#ifdef RACE_AWS_ENABLE
            race_app_aws_cm_event_handler(event_id, extra_data);
#endif
            break;
        }
        default: {
            break;
        }
    }

    return ret;
}

bool app_fota_idle_activity_is_fota_doing(void)
{
    return s_app_fota_context.curr_state != FOTA_IN_IDLE && s_app_fota_context.curr_state != FOTA_IN_END;
}

