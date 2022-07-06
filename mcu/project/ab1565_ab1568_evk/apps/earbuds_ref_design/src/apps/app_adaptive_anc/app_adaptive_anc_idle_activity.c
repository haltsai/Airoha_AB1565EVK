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
#include "app_adaptive_anc_idle_activity.h"

#include "apps_debug.h"
#include "apps_events_event_group.h"

#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif

#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd_dsprealtime.h"
#include "race_event.h"
#endif

#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
#include "apps_events_interaction_event.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"

#include "app_voice_prompt.h"
#include "audio_src_srv.h"
#include "user_trigger_adaptive_ff.h"
#include "bt_connection_manager.h"
#include "apps_aws_sync_event.h"
#include "bt_device_manager.h"
#include "app_anc_service.h"
#endif

#define LOG_TAG           "[adaptive_anc]"

#if defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
#include "app_anc_extend_gain_activity.h"
#include "app_anc_service.h"
bool g_app_extend_gain_activity_started = false;
#endif

#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
static app_adaptive_anc_context_t s_app_adaptive_anc_ctx;

static void app_adaptive_anc_check_terminate(app_adaptive_anc_context_t *local_context, bool force);

#if defined(MTK_VENDOR_SOUND_EFFECT_ENABLE)
static void app_adaptive_anc_audio_state_callback(vendor_se_event_t event, void *arg)
{
    //APPS_LOG_MSGID_I(LOG_TAG" audio callback event %d", 1, event);
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_AUDIO_STATE_CHANGE, (void *)event, 0, NULL, 0);
}
#endif
#endif

static bool app_adaptive_anc_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(LOG_TAG" create", 0);
#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
            if (self) {
                self->local_context = (void *)&s_app_adaptive_anc_ctx;
                if (self->local_context) {
                    memset(self->local_context, 0, sizeof(app_adaptive_anc_context_t));
                }

#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
                am_vendor_se_id_t am_user_id = ami_get_vendor_se_id();
                if (am_user_id == -1) {
                    APPS_LOG_MSGID_E(LOG_TAG" ami_get_vendor_se_id fail, user id -1", 0);
                    break;
                }
                bt_sink_srv_am_result_t result = ami_register_vendor_se(am_user_id, app_adaptive_anc_audio_state_callback);
                APPS_LOG_MSGID_I(LOG_TAG" ami_register_vendor_se, id=%d result=%d", 2, am_user_id, result);
#endif
            }
#endif

#if defined(MTK_ANC_ENABLE) && (defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE))
            app_anc_extend_gain_init_control_status();
#endif
            break;
        }
    }
    return ret;
}

#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
static void app_adaptive_anc_update_reject(app_adaptive_anc_context_t *local_ctx)
{
    bool reject_pre = local_ctx->reject;
    if (local_ctx->sink_state > BT_SINK_SRV_STATE_CONNECTED || local_ctx->audio_on || local_ctx->is_rho || !local_ctx->in_ear) {
        local_ctx->reject = true;
    } else {
        local_ctx->reject = false;
    }

    if (local_ctx->reject != reject_pre) {
        APPS_LOG_MSGID_E(LOG_TAG" update_reject, %d->%d", 2, reject_pre, local_ctx->reject);
        apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                       APPS_EVENTS_INTERACTION_ADAPTIVE_ANC_STATE_CHANGE,
                                       &(local_ctx->reject), sizeof(bool));
    }
}

static bool app_adaptive_anc_proc_bt_sink_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            app_adaptive_anc_context_t *local_context = self->local_context;
            bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)extra_data;
            bt_sink_srv_state_t pre_state = event->state_change.previous;
            bt_sink_srv_state_t curr_state = event->state_change.current;
            APPS_LOG_MSGID_I(LOG_TAG" sink_state_change, %d->%d", 2, pre_state, curr_state);
            local_context->sink_state = curr_state;
            app_adaptive_anc_update_reject(local_context);
            app_adaptive_anc_check_terminate(local_context, false);
            break;
        }
        default:
            break;
    }
    return false;
}

static bool app_adaptive_anc_proc_bt_cm_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            app_adaptive_anc_context_t *local_context = self->local_context;
            if (remote_update == NULL) {
                break;
            }
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state
                && (memcmp(bt_device_manager_get_local_address(), &(remote_update->address), sizeof(bt_bd_addr_t)) != 0)) {
                if (bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0) == 0) {
                    APPS_LOG_MSGID_I(LOG_TAG" [%02X] ACL Remote_Device disconnect", 1, role);
                    app_adaptive_anc_check_terminate(local_context, true);
                }
            } else if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                       && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                APPS_LOG_MSGID_I(LOG_TAG" [%02X] AWS Connected, sync reject", 1, role);
                apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                               APPS_EVENTS_INTERACTION_ADAPTIVE_ANC_STATE_CHANGE,
                                               &(local_context->reject), sizeof(bool));
            } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                       && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                APPS_LOG_MSGID_I(LOG_TAG" [%02X] AWS Disconnected, set another_reject to 0", 1, role);
                local_context->peer_reject = false;
                local_context->aws_connect = false;
                app_adaptive_anc_check_terminate(local_context, false);
            }
            break;
        }
        default:
            break;
    }

    return false;
}

static void app_adaptive_anc_resume_anc_service(app_adaptive_anc_context_t *local_context)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG" Resume ANC, [%02X] anc_suspend=%d", 2, role, local_context->anc_suspend);
    if (local_context->anc_suspend && role == BT_AWS_MCE_ROLE_AGENT) {
#if 0
        audio_anc_control_result_t anc_result = audio_anc_control_set_runtime_gain(local_context->anc_state.anc_runtime_gain,
                                                                                   local_context->anc_state.anc_type);
        anc_result += audio_anc_control_enable(local_context->anc_state.anc_filter_id, local_context->anc_state.anc_type,
                                               &(local_context->anc_state.anc_control_misc));
        APPS_LOG_MSGID_I(LOG_TAG" Resume ANC, filter_id=%d type=%d anc_result=%d",
                         3, local_context->anc_state.anc_filter_id,
                         local_context->anc_state.anc_type, anc_result);
#endif
        local_context->anc_suspend = false;
    }

    app_anc_service_set_user_trigger_state(false);
}

static void app_adaptive_anc_check_terminate(app_adaptive_anc_context_t *local_context, bool force)
{
    if (local_context->app_state < APP_ADAPTIVE_ANC_STARTED) {
        //APPS_LOG_MSGID_E(LOG_TAG" check terminate, error state %d", 1, local_context->app_state);
        return;
    }

    if (force || local_context->reject || local_context->peer_reject || !local_context->aws_connect || !local_context->in_ear) {
        user_trigger_adaptive_ff_err_event_t terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_OTHER;

        if (local_context->sink_state == BT_SINK_SRV_STATE_STREAMING) {
            terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_A2DP;
        } else if (local_context->sink_state > BT_SINK_SRV_STATE_STREAMING) {
            terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_ESCO;
        } else if (local_context->audio_on) {
            if (local_context->audio_event == EVENT_A2DP_START) {
                terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_A2DP;
            } else if (local_context->audio_event == EVENT_HFP_START) {
                terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_ESCO;
            }
        } else if (local_context->is_rho) {
            terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_RHO;
        } else if (!local_context->in_ear) {
            terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_IN_EAR_DET;
        } else if (!local_context->aws_connect) {
            terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_PARTNER;
        } else if (force) {
            terminate_reason = USER_TRIGGER_ADAPTIVE_FF_ERR_TERMINATE_STOP;
        }

        APPS_LOG_MSGID_I(LOG_TAG" terminate, reason=%d", 1, terminate_reason);
        audio_user_trigger_adaptive_ff_terminate(terminate_reason);

        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
            apps_config_stop_voice(true, 0, true);
        } else {
            apps_config_stop_voice(false, 0, true);
        }

        local_context->app_state = APP_ADAPTIVE_ANC_IDLE;
        app_adaptive_anc_resume_anc_service(local_context);
    } else {
        APPS_LOG_MSGID_I(LOG_TAG" terminate, ignore", 0);
    }
}

static void app_adaptive_anc_play_vp()
{
    APPS_LOG_MSGID_I(LOG_TAG" anc_play_vp", 0);
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_ANC_FF_AUDIO,
                        APP_ADAPTIVE_ANC_TRIGGER_VP, NULL, 0, NULL, 0);
}

static void app_adaptive_anc_test_callback(user_trigger_adaptive_ff_event_t event)
{
    APPS_LOG_MSGID_I(LOG_TAG" anc_test_callback, event=%d", 1, event);

    if (event == USER_TRIGGER_ADAPTIVE_FF_EVENT_END) {
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_ANC_FF_AUDIO,
                            APP_ADAPTIVE_ANC_ENDED, NULL, 0, NULL, 0);
    } else if (event == USER_TRIGGER_ADAPTIVE_FF_EVENT_TERMINATED) {
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_ANC_FF_AUDIO,
                            APP_ADAPTIVE_ANC_TERMINATED, NULL, 0, NULL, 0);
    }
}

static bool app_adaptive_anc_proc_anc_ff_race_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    app_adaptive_anc_context_t *local_context = self->local_context;
    switch (event_id) {
        case RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_START: {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            APPS_LOG_MSGID_I(LOG_TAG" [RACE] ANC_ADAPTIVE_FF_START, [%2X] state=%d reject=%d another_reject=%d",
                             4, role, local_context->app_state, local_context->reject, local_context->peer_reject);
            if (app_voice_prompt_get_current_index() != 0xFFFF) {
                APPS_LOG_MSGID_E(LOG_TAG" [RACE] ANC_ADAPTIVE_FF_START, VP playing can't start", 0);
                app_adaptive_anc_resume_anc_service(local_context);
                race_dsprealtime_anc_adaptive_response(false, true);
                local_context->app_state = APP_ADAPTIVE_ANC_IDLE;
                break;
            }
            if (local_context->app_state < APP_ADAPTIVE_ANC_STARTED
                    && !local_context->reject && !local_context->peer_reject) {
                if (app_anc_service_is_enable() && role == BT_AWS_MCE_ROLE_AGENT) {
                    audio_anc_control_get_status(&(local_context->anc_state.anc_enable),
                                                 &(local_context->anc_state.anc_filter_id),
                                                 &(local_context->anc_state.anc_type),
                                                 &(local_context->anc_state.anc_runtime_gain),
                                                 &(local_context->anc_state.support_hybrid_enable),
                                                 &(local_context->anc_state.anc_control_misc));
                    APPS_LOG_MSGID_I(LOG_TAG" [RACE] ANC_ADAPTIVE_FF_START, ANC enable=%d, filter=%d, type=%d",
                                     3, local_context->anc_state.anc_enable,
                                     local_context->anc_state.anc_filter_id,
                                     local_context->anc_state.anc_type);
                    if (local_context->anc_state.anc_enable) {
                        audio_anc_control_result_t anc_result = audio_anc_control_disable(NULL);
                        APPS_LOG_MSGID_I(LOG_TAG" [RACE] ANC_ADAPTIVE_FF_START, disable anc_result=%d", 1, anc_result);
                        local_context->anc_suspend = true;
                        local_context->app_state = APP_ADAPTIVE_ANC_SUEPENDING_ANC;
                        app_anc_service_set_user_trigger_state(true);
                        break;
                    }
                }

                APPS_LOG_MSGID_I(LOG_TAG" [RACE] ANC_ADAPTIVE_FF_START, Start", 0);
                local_context->aws_connect = true;
                app_anc_service_set_user_trigger_state(true);
                audio_user_trigger_adaptive_ff_register_vp_callback(app_adaptive_anc_play_vp);
                audio_user_trigger_adaptive_ff_register_event_callback(app_adaptive_anc_test_callback, USER_TRIGGER_ADAPTIVE_FF_EVENT_ALL);
                user_trigger_adaptive_ff_result_t res = audio_user_trigger_adaptive_ff_request((anc_user_trigger_ff_callback_t)user_trigger_adaptive_ff_racecmd_response);
                if (res == USER_TRIGGER_ADAPTIVE_FF_EXECUTION_SUCCESS) {
                    local_context->app_state = APP_ADAPTIVE_ANC_STARTED;
                    race_dsprealtime_anc_adaptive_response(true, true);
                } else {
                    APPS_LOG_MSGID_E(LOG_TAG" [RACE] ANC_ADAPTIVE_FF_START, start fail %d", 1, res);
                    race_dsprealtime_anc_adaptive_response(false, true);
                    app_adaptive_anc_resume_anc_service(local_context);
                }
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" [RACE] ANC_ADAPTIVE_FF_START, abort", 0);
                race_dsprealtime_anc_adaptive_response(false, true);
                app_adaptive_anc_resume_anc_service(local_context);
            }
            break;
        }
        case RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_CANCEL: {
            APPS_LOG_MSGID_I(LOG_TAG" [RACE] ANC_ADAPTIVE_FF_CANCEL", 0);
            app_adaptive_anc_check_terminate(local_context, true);
            race_dsprealtime_anc_adaptive_response(true, false);
            break;
        }
        default:
            break;
    }
    return true;
}

static bool app_adaptive_anc_proc_anc_ff_audio_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    app_adaptive_anc_context_t *local_context = self->local_context;

    switch (event_id) {
        case APP_ADAPTIVE_ANC_TRIGGER_VP:
            if (local_context->app_state == APP_ADAPTIVE_ANC_STARTED) {
                APPS_LOG_MSGID_I(LOG_TAG" ANC_TRIGGER_VP", 0);
                apps_config_set_voice(VP_INDEX_USER_TRIGGER_FF, true, 200, VOICE_PROMPT_PRIO_ULTRA, true, false, NULL);
            }
            break;
        case APP_ADAPTIVE_ANC_TERMINATED:
            APPS_LOG_MSGID_I(LOG_TAG" ANC_TERMINATED, state=%d", 1, local_context->app_state);
            if (local_context->app_state == APP_ADAPTIVE_ANC_STARTED) {
                apps_config_stop_voice(true, 0, true);
                local_context->app_state = APP_ADAPTIVE_ANC_IDLE;
                app_adaptive_anc_resume_anc_service(local_context);
            }
            break;
        case APP_ADAPTIVE_ANC_ENDED:
            APPS_LOG_MSGID_I(LOG_TAG" ANC_ENDED, state=%d", 1, local_context->app_state);
            if (local_context->app_state == APP_ADAPTIVE_ANC_STARTED) {
                local_context->app_state = APP_ADAPTIVE_ANC_IDLE;
                app_adaptive_anc_resume_anc_service(local_context);
            }
            break;
        default:
            break;
    }

    return true;
}

static bool app_adaptive_anc_proc_interaction_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    app_adaptive_anc_context_t *local_context = self->local_context;

    switch (event_id) {
        case APPS_EVENTS_INTERACTION_AUDIO_STATE_CHANGE: {
#ifdef MTK_VENDOR_SOUND_EFFECT_ENABLE
            bool audio_update = false;
            vendor_se_event_t am_event = (vendor_se_event_t)extra_data;
            if (am_event == EVENT_A2DP_START || am_event == EVENT_HFP_START
                    || am_event == EVENT_LINEINPLAYBACK_START || am_event == EVENT_USB_AUDIO_START
                    || am_event == EVENT_BLE_START) {
                APPS_LOG_MSGID_I(LOG_TAG" INTERACTION_AUDIO_STATE_CHANGE, audio on", 0);
                local_context->audio_on = true;
                local_context->audio_event = am_event;
                audio_update = true;
            } else if (am_event == EVENT_A2DP_STOP || am_event == EVENT_HFP_STOP
                    || am_event == EVENT_LINEINPLAYBACK_STOP || am_event == EVENT_USB_AUDIO_STOP
                    || am_event == EVENT_BLE_STOP) {
                APPS_LOG_MSGID_I(LOG_TAG" INTERACTION_AUDIO_STATE_CHANGE, audio off", 0);
                local_context->audio_on = false;
                local_context->audio_event = am_event;
                audio_update = true;
            }

            if (audio_update) {
                app_adaptive_anc_update_reject(local_context);
                app_adaptive_anc_check_terminate(local_context, false);
            }
#endif
            break;
        }
        case APPS_EVENTS_INTERACTION_TRIGGER_RHO: {
            APPS_LOG_MSGID_I(LOG_TAG" APPS_EVENTS_INTERACTION_TRIGGER_RHO", 0);
            local_context->is_rho = true;
            app_adaptive_anc_update_reject(local_context);
            app_adaptive_anc_check_terminate(local_context, false);
            break;
        }
        case APPS_EVENTS_INTERACTION_RHO_END: {
            APPS_LOG_MSGID_I(LOG_TAG" APPS_EVENTS_INTERACTION_RHO_END", 0);
            local_context->is_rho = false;
            app_adaptive_anc_update_reject(local_context);
            app_adaptive_anc_check_terminate(local_context, false);
            break;
        }
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        case APPS_EVENTS_INTERACTION_UPDATE_IN_EAR_STA_EFFECT: {
            if (extra_data) {
                bool *in_ear = (bool *)extra_data;
                APPS_LOG_MSGID_I(LOG_TAG" in ear state %d", 1, *in_ear);
                local_context->in_ear = *in_ear;
                app_adaptive_anc_update_reject(local_context);
                app_adaptive_anc_check_terminate(local_context, false);
            }
            break;
        }
#endif
        default:
            break;
    }
    return false;
}

static bool app_adaptive_anc_proc_aws_data_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t event;
        bool *extra;
        uint32_t extra_len;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event, (void *)&extra, &extra_len);
        if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && event == APPS_EVENTS_INTERACTION_ADAPTIVE_ANC_STATE_CHANGE) {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            ret = true;
            bool reject = *extra;
            app_adaptive_anc_context_t *local_context = self->local_context;

            APPS_LOG_MSGID_I(LOG_TAG" [AWS][%02X] Recv reject_state=%d", 2, role, reject);
            local_context->peer_reject = reject;
            app_adaptive_anc_check_terminate(local_context, false);
        }
    }
    return ret;
}

app_adaptive_anc_state_t app_adaptive_anc_get_state()
{
    return s_app_adaptive_anc_ctx.app_state;
}

#endif

#if defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
static bool app_adaptive_anc_proc_audio_anc_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
    app_adaptive_anc_context_t *local_context = self->local_context;
    if (event_id == AUDIO_ANC_CONTROL_EVENT_OFF && local_context->app_state == APP_ADAPTIVE_ANC_SUEPENDING_ANC) {
        APPS_LOG_MSGID_I(LOG_TAG" ANC OFF, SUEPENDING_ANC resend FF_START", 0);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_ANC_FF_RACE, RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_START, NULL, 0, NULL, 0);
    }
#endif
#if defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
    if (event_id == AUDIO_ANC_CONTROL_EVENT_ON && !g_app_extend_gain_activity_started) {
        app_anc_srv_result_t *anc_result = (app_anc_srv_result_t *)pvPortMalloc(sizeof(app_anc_srv_result_t));
        if (anc_result == NULL || extra_data == NULL || data_len != sizeof(app_anc_srv_result_t)) {
            APPS_LOG_MSGID_E(LOG_TAG" ANC ON, start anc_extend_gain_activity fail", 0);
            return false;
        }
        APPS_LOG_MSGID_I(LOG_TAG" ANC ON, start anc_extend_gain_activity", 0);
        memcpy((void *)anc_result, extra_data, data_len);
        g_app_extend_gain_activity_started = true;
        ui_shell_start_activity(self, app_anc_extend_gain_activity_proc, ACTIVITY_PRIORITY_MIDDLE, anc_result, data_len);
    }
#endif
    return false;
}
#endif

bool app_adaptive_anc_idle_activity_proc(struct _ui_shell_activity *self,
                                         uint32_t event_group,
                                         uint32_t event_id,
                                         void *extra_data,
                                         size_t data_len)
{
    bool ret = false;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = app_adaptive_anc_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
        case EVENT_GROUP_UI_SHELL_ANC_FF_RACE: {
            ret = app_adaptive_anc_proc_anc_ff_race_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_ANC_FF_AUDIO: {
            ret = app_adaptive_anc_proc_anc_ff_audio_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            ret = app_adaptive_anc_proc_bt_sink_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            ret = app_adaptive_anc_proc_interaction_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            ret = app_adaptive_anc_proc_bt_cm_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            ret = app_adaptive_anc_proc_aws_data_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
#if defined(MTK_USER_TRIGGER_ADAPTIVE_FF_V2) || defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE)
        case EVENT_GROUP_UI_SHELL_AUDIO_ANC: {
            ret = app_adaptive_anc_proc_audio_anc_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN_RACE: {
#if defined(MTK_ANC_ENABLE) && (defined(AIR_ANC_WIND_DETECTION_ENABLE) || defined(AIR_ANC_USER_UNAWARE_ENABLE) || defined(AIR_ANC_ENVIRONMENT_DETECTION_ENABLE))
            bool anc_enable = app_anc_service_is_enable();
            APPS_LOG_MSGID_I(LOG_TAG" [anc_gain] ANC_EXTEND_GAIN_RACE, anc_enable=%d", 1, anc_enable);
            if (!anc_enable) {
                if (event_id == RACE_EVENT_TYPE_ANC_GAIN_USER_UNAWARE
                    || event_id == RACE_EVENT_TYPE_ANC_GAIN_WIND_NOISE
                    || event_id == RACE_EVENT_TYPE_ANC_GAIN_ENVIRONMENT_DETECTION) {
                    race_dsprealtime_anc_notify_gain_error(RACE_ANC_GAIN_STATUS_ANC_NOT_ON, event_id);
                } else if (event_id == RACE_EVENT_TYPE_ANC_GAIN_CONTROL) {
                    anc_gain_control_param_t *param = (anc_gain_control_param_t *)extra_data;
                    uint8_t type = param->type;
                    uint8_t action = param->action;
                    if (action == RACE_ANC_GAIN_CONTROL_QUERY) {
                        app_anc_extend_gain_query_status(param);
                    } else {
                        race_dsprealtime_anc_gain_control_response(RACE_ANC_GAIN_STATUS_ANC_NOT_ON, type, FALSE);
                    }
                }
            }
#endif
            break;
        }
        default:
            break;
    }
    return ret;
}
