
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
 * File: app_ms_teams_activity.c
 *
 * Description:
 * This file is ms_teams idle activity. This activity is used for teams status management
 *
 */


#ifdef AIR_MS_TEAMS_ENABLE

#include "app_ms_teams_activity.h"
#include "app_ms_teams_utils.h"

#define TAG "[MS TEAMS] activity "

static bool app_ms_teams_idle_teams_ev_proc(struct _ui_shell_activity *self, uint32_t ev, uint8_t *data, uint32_t data_len) {
    bool ret = false;
    ms_teams_event_t event = ((ev >> 16) & 0xFFFF);

    APPS_LOG_MSGID_I(TAG"teams event proc, 0x%x.", 1, ev);
    switch (event) {
        case MS_TEAMS_EVENT_CALL_STATE: {
            ms_teams_call_state_sub_event_t sub_ev = (ms_teams_call_state_sub_event_t)(ev & 0xFFFF);
            APPS_LOG_MSGID_I(TAG"teams call sta event, 0x%x.", 1, ev);
            if (sub_ev == MS_TEAMS_CALL_EVENT_END_CALL) {
                APPS_LOG_MSGID_I(TAG"finish activity.", 0);
                ui_shell_finish_activity(self, self);
            }
            app_ms_teams_set_mmi_state(sub_ev);
            ret = true;
        }
        default:
            break;
    }

    return ret;
}

static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            /* TODO: Set led */
            APPS_LOG_MSGID_I(TAG"create", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool _handle_key_event(apps_config_key_action_t key_action) {
    bool ret = false;
    static ms_teams_action_t last_action = MS_TEAMS_ACTION_NONE;
    ms_teams_action_t ms_action = MS_TEAMS_ACTION_NONE;

    switch(key_action) {
        case KEY_CANCEL_OUT_GOING_CALL:
            ms_action = MS_TEAMS_ACTION_CALL_REJECT_OR_END;
            break;
        case KEY_REJCALL:
            ms_action = MS_TEAMS_ACTION_CALL_REJECT_OR_END;
            break;
        case KEY_REJCALL_SECOND_PHONE:
            ms_action = MS_TEAMS_ACTION_CALL_REJECT_OR_END;
            break;
        case KEY_ONHOLD_CALL:
            ms_action = last_action == MS_TEAMS_ACTION_HOLD_CALL ? MS_TEAMS_ACTION_RESUME_CALL : MS_TEAMS_ACTION_HOLD_CALL;
            break;
        case KEY_ACCEPT_CALL:
            ms_action = MS_TEAMS_ACTION_CALL_ACCEPT;
            break;
        case KEY_END_CALL:
            ms_action = MS_TEAMS_ACTION_CALL_REJECT_OR_END;
            break;
        default:
            break;
    }

    if (ms_action != MS_TEAMS_ACTION_NONE) {
        APPS_LOG_MSGID_I(TAG"ms teams receive key event: 0x%x, 0x%x", 2 , key_action, ms_action);
        ret = true;
        ms_teams_send_action(ms_action, NULL, 0);
    }

    return ret;
}

static bool _proc_key_event(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    apps_config_key_action_t action;
    uint8_t key_id;
    airo_key_event_t key_event;

    /* TODO: should not handle the key action about call when HFP is activate. */

    app_event_key_event_decode(&key_id, &key_event, event_id);
    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role;
    role = bt_device_manager_aws_local_info_get_role();
    if (role == BT_AWS_MCE_ROLE_PARTNER) {
        if (action >= KEY_REDIAL_LAST_CALL && action <= KEY_END_CALL) {
            bt_status_t ret = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action);
            if (BT_STATUS_SUCCESS != ret) {
                APPS_LOG_MSGID_I(TAG"partner sync key action to agent failed: %d.", 1, ret);
            }
            return true;
        }
    }
#endif

    return _handle_key_event(action);
}


#ifdef MTK_AWS_MCE_ENABLE
static bool _proc_aws_data(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action,
                        &p_extra_data, &extra_data_len);
        if (event_group == EVENT_GROUP_UI_SHELL_KEY && (action >= KEY_REDIAL_LAST_CALL && action <= KEY_END_CALL)) {
            /* TODO: should not handle the key action about call when HFP is activate. */
            APPS_LOG_MSGID_I(TAG"receive partner key action: %d.", 1, action);
            ret = _handle_key_event(action);
        }
    }

    /* TODO: handle the teams event come from Agent. */
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
bool app_ms_teams_activity_proc(struct _ui_shell_activity *self,
                                  uint32_t event_group,
                                  uint32_t event_id,
                                  void *extra_data,
                                  size_t data_len)
{

    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            /* UI Shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_KEY:
            /* key event. */
            ret = _proc_key_event(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_MS_TEAMS:
            ret = app_ms_teams_idle_teams_ev_proc(self, event_id, extra_data, data_len);
            break;

#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            /* The event come from partner. */
            ret = _proc_aws_data(self, event_id, extra_data, data_len);
            break;
#endif
    }
    return ret;
}

#endif /* AIR_MS_TEAMS_ENABLE */

