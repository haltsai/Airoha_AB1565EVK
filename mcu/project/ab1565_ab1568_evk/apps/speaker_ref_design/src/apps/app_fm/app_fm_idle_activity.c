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
 * File: app_fm_idle_activity.c
 *
 * Description: This file is the idle activity to receive find me triggered event and create app_fm_activity.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "app_fm_idle_activity.h"
#include "timers.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "bt_device_manager.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "apps_aws_sync_event.h"
#endif

#define UI_SHELL_IDLE_FM_ACTIVITY "[FIND_ME_APP]ui_shell_ut_idle_fm_activity"

static app_find_me_context_t s_app_find_me_context; /* The variable records context */

static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", create", 0);
            self->local_context = &s_app_find_me_context;
            memset(self->local_context, 0, sizeof(app_find_me_context_t));
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
bool app_find_me_idle_proc_aws_data(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = FALSE;

    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action, &p_extra_data, &extra_data_len);

        /* Partner should receive and handle XiaoAI find me event from AWS MCE.*/
         if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION) {
            if ((role == BT_AWS_MCE_ROLE_PARTNER) && action == APPS_EVENTS_INTERACTION_FIND_ME_APP_SYNC_FIND_ME_ACTION) {
                /* Received find me request from agent.*/
                if (extra_data_len == sizeof(app_find_me_param_struct)) {
                    app_find_me_param_struct *find_me_param =
                            (app_find_me_param_struct *)pvPortMalloc(sizeof(app_find_me_param_struct));
                    if (find_me_param) {
                        memcpy(find_me_param, p_extra_data, sizeof(app_find_me_param_struct));
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY" [PARTNER] AWS find me blink=%d, tone=%d, duration_seconds=%d",
                                3, find_me_param->blink, find_me_param->tone, find_me_param->duration_seconds);
                        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE,
                                EVENT_GROUP_UI_SHELL_FINDME,
                                APP_FIND_ME_EVENT_ID_TRIGGER,
                                find_me_param,
                                sizeof(app_find_me_param_struct),
                                NULL, 0);
                    }
                }
            } else if (action == APPS_EVENTS_INTERACTION_NOTIFY_FIND_ME_STATE) {
                app_find_me_context_t *context = (app_find_me_context_t *)self->local_context;
                app_find_me_notify_state_t *notify_state = (app_find_me_notify_state_t *)p_extra_data;
                if (context && notify_state) {
                    if (context->peer_blink != notify_state->blink || context->peer_tone != notify_state->tone) {
                        context->peer_blink = notify_state->blink;
                        context->peer_tone = notify_state->tone;
                        app_find_me_notify_state_change(context, false);
                    }
                }
                ret = true;
            }
        }
    }

    return ret;
}
#endif


bool app_find_me_idle_activity_proc(
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
        case EVENT_GROUP_UI_SHELL_FINDME: {
            /* Events to control find me, APP_FIND_ME_EVENT_ID_TRIGGER is sent by race cmd or other activities(app_fast_pair). */
            if (event_id == APP_FIND_ME_EVENT_ID_TRIGGER) {
                app_find_me_context_t *fm_context = (app_find_me_context_t *)self->local_context;
                app_find_me_param_struct *param = (app_find_me_param_struct *)extra_data;
                fm_context->blink = param->blink;
                fm_context->tone = param->tone;
                fm_context->duration_seconds = param->duration_seconds;
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", start_find_me: blink: %x, tone : %x, sink state %x", 3,
                                 fm_context->blink, fm_context->tone, fm_context->sink_state);
                if ((fm_context->blink || fm_context->tone) && (fm_context->sink_state < BT_SINK_SRV_STATE_INCOMING)) {
                    /* If the param indicates to blink or play ringtone, and not in call, start app_fm_activity to control LED and ringtone playback. */
                    ui_shell_start_activity(NULL, (ui_shell_proc_event_func_t)app_find_me_activity_proc, ACTIVITY_PRIORITY_MIDDLE, (void *)fm_context, 0);
                }
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* bt_sink events. */
            if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
                bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
                app_find_me_context_t *fm_context = (app_find_me_context_t *)self->local_context;
                if ((param == NULL) || (fm_context == NULL)) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", bt sink state change param null", 0);
                    return ret;
                }
                fm_context->sink_state = param->current;
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            app_find_me_context_t *fm_context = (app_find_me_context_t *)self->local_context;
            if ((extra_data == NULL) || (fm_context == NULL)) {
                break;
            }
            if (event_id == BT_CM_EVENT_REMOTE_INFO_UPDATE) {
                bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
                if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & (remote_update->pre_connected_service & ~remote_update->connected_service)) {
                    /* When AWS disconnected. */
                    if (fm_context->peer_blink || fm_context->peer_tone) {
                        fm_context->peer_blink = 0;
                        fm_context->peer_tone = 0;
                        app_find_me_notify_state_change(fm_context, false);
                    }
                }
            }
#endif
            break;
        }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            ret = app_find_me_idle_proc_aws_data(self, event_id, extra_data, data_len);
            break;
        }
#endif
        default:
            break;
    }

    /* APPS_LOG_MSGID_I(UI_SHELL_IDLE_FM_ACTIVITY", ret : %x", 1, ret); */
    return ret;
}


const app_find_me_context_t *app_find_me_idle_activity_get_context(void) {
    return &s_app_find_me_context;
}
