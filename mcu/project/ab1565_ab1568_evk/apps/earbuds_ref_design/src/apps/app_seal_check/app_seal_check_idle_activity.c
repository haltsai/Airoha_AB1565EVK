/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
 * File: app_seal_check_idle_activity.c
 *
 * Description:
 * This file is the activity to play the VP of leakage detection. When leakage detection
 * starts, the activity will play the VP of leakage detection. And the algorithm in the
 * middleware will check the sealing state of the earbuds according to the playback
 * quality of this VP.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifdef MTK_LEAKAGE_DETECTION_ENABLE

#include "app_seal_check_idle_activity.h"
#include "app_seal_check_utils.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "app_voice_prompt.h"
#include "bt_sink_srv.h"
#include "bt_device_manager.h"
#include "apps_config_event_list.h"
#include "apps_debug.h"
#include "ui_shell_manager.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "airo_key_event.h"
#include "bt_sink_srv_ami.h"
#include "leakage_detection_control.h"
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
#include "app_advance_passthrough.h"
#endif
#ifdef MTK_AWS_MCE_ENABLE
#include "apps_aws_sync_event.h"
#endif

#define SC_TAG "app_seal_check "

static app_leakage_detection_context_t s_app_leakage_detection_conext;  /* The variable records context. */
app_leakage_detection_context_t *get_context()
{
    return &s_app_leakage_detection_conext;
}

void app_leakage_detection_broadcast_ongoing_status(bool is_from_irq, bool is_ongoing)
{
    APPS_LOG_MSGID_I(SC_TAG" app_leakage_detection_broadcast_status: is_from_irq=%d, is_ongoing=%d.",
            2, is_from_irq, is_ongoing);
    ui_shell_send_event(is_from_irq, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
            APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_STATUS, (void *)is_ongoing, 0, NULL, 0);
}

/**
* @brief      This function is called by the middleware at the the beginning of leakage detection.
* @param[in]  leakage_status, the leakage detection status.
* @return     None.
*/
static void leakage_detection_callback(uint16_t leakage_status)
{
    bool *p_play_flag = NULL;
    app_leakage_detection_context_t *ctx = NULL;

    ctx = get_context();
    ctx->seal_checking = true;

    /* Play leakage detection VP with 1500ms delay. */
    p_play_flag = (bool *)pvPortMalloc(sizeof(bool));
    if (p_play_flag == NULL) {
        APPS_LOG_MSGID_I(SC_TAG" leakage_detection_callback: malloc mem failed.", 0);
        return;
    }
    *p_play_flag = true;

    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER);
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP);
    ui_shell_status_t status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                   APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER,
                                                   p_play_flag, sizeof(bool), NULL, 200);
    if (UI_SHELL_STATUS_OK != status) {
        vPortFree(p_play_flag);
        APPS_LOG_MSGID_I(SC_TAG" leakage_detection_callback: UI_SHELL send race cmd vp trigger event failed.", 0);
    } else {
        app_leakage_detection_broadcast_ongoing_status(false, true);
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
        app_advance_passthrough_set_ld_ongoing(TRUE);
#endif
    }
}

static void leakage_detection_over(app_leakage_detection_context_t *ctx)
{
    APPS_LOG_MSGID_I(SC_TAG" leakage_detection_over.", 0);
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER);
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP);
#if 1
    if (APP_LEAKAGE_DETECTION_VP_INDEX == app_voice_prompt_get_current_index()) {
#ifndef APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL
    apps_config_stop_voice(true, 0, true);
#else
    apps_config_stop_voice(true, 0, false);
#endif
#endif
    }
    ctx->vp_cnt = 0;
    ctx->seal_checking = false;

#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
    app_advance_passthrough_set_ld_ongoing(FALSE);
#endif
    app_leakage_detection_broadcast_ongoing_status(false, false);
}

static void app_seal_check_stop()
{
    app_leakage_detection_context_t *ctx = NULL;
    ctx = get_context();
    ctx->seal_checking = false;

#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
    app_advance_passthrough_set_ld_ongoing(FALSE);
#endif
    audio_anc_leakage_detection_stop();
    app_leakage_detection_broadcast_ongoing_status(false, false);
}


static void vp_callback(uint32_t idx, vp_err_code err)
{
    /* Stop the leakage detection when the VP of leakage detection is interrupted by others VP. */
    if (idx == APP_LEAKAGE_DETECTION_VP_INDEX && err == VP_ERR_CODE_PREEMPTED) {
        APPS_LOG_MSGID_I(SC_TAG" vp stoped", 0);
        app_seal_check_stop();
    }
}

/**
* @brief      This function is called by the middleware at the the stopped of leakage detection.
* @param[in]  event_id, the event id of leakage detection.
* @param[in]  result, the result of leakage detection.
* @return     None.
*/
static void app_leakage_detection_stop_callback(
    audio_anc_leakage_detection_control_event_t event_id,
    audio_anc_leakage_detection_execution_t result)
{
    APPS_LOG_MSGID_I(SC_TAG" leakage_detection_stop_callback: event_id=0x%x, result=0x%x",
            2, event_id, result);
    if (event_id == AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_STOP
            && result ==  AUDIO_LEAKAGE_DETECTION_EXECUTION_SUCCESS) {
        app_leakage_detection_broadcast_ongoing_status(true, false);
    }
}

static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = true;
    app_leakage_detection_context_t *ctx = NULL;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(SC_TAG" create", 0);
            self->local_context = &s_app_leakage_detection_conext;
            memset(self->local_context, 0, sizeof(app_leakage_detection_context_t));
            ctx = (app_leakage_detection_context_t *)self->local_context;
            ctx->in_idle = true;
            audio_anc_leakage_detection_register_vp_start_callback(leakage_detection_callback);
            audio_anc_leakage_detection_control_register_callback(app_leakage_detection_stop_callback,
                    AUDIO_LEAKAGE_DETECTION_CONTROL_EVENT_STOP,
                    AUDIO_LEAKAGE_DETECTIONs_CONTROL_CB_LEVEL_SUCCESS_ONLY);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(SC_TAG" destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(SC_TAG" resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(SC_TAG" pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(SC_TAG" refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(SC_TAG" result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}


static bool _proc_apps_internal_events(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    app_leakage_detection_context_t *ctx = (app_leakage_detection_context_t *)self->local_context;

    switch (event_id) {
        case APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER:
            ret = true;
            if (ctx && ctx->in_idle) {
                bool *p_play_flag = (bool *)extra_data;
                APPS_LOG_MSGID_I(SC_TAG" trigger vp play, flag:%d",
                                 1, *p_play_flag);
                if (*p_play_flag) {
#ifndef APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL
                    apps_config_set_voice(APP_LEAKAGE_DETECTION_VP_INDEX, true, 200, VOICE_PROMPT_PRIO_ULTRA, true, false, vp_callback);
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP);
                    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP,
                                        NULL, 0, NULL,
                                        APP_LEAKAGE_DETECTION_VP_PLAY_TIME);
#else
                    ctx->vp_cnt = APP_LEAKAGE_DETECTION_VP_REPEAT_TIMES - 1;
                    apps_config_set_voice(APP_LEAKAGE_DETECTION_VP_INDEX, true, 200, VOICE_PROMPT_PRIO_ULTRA, false, false, vp_callback);
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP);
                    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP,
                                        NULL, 0, NULL,
                                        APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL);
#endif
                } else {
                    ctx->seal_checking = false;
                    leakage_detection_over(ctx);
                }
            } else {
                app_seal_check_stop();
            }

            break;

        case APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP: {
            ret = true;
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            bool ld_over = FALSE;

            APPS_LOG_MSGID_I(SC_TAG" [%02X] LEAKAGE_DETECTION_VP event", 1, role);
#ifndef APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL
            leakage_detection_over(ctx);
            if (role == BT_AWS_MCE_ROLE_PARTNER) {
                ld_over = TRUE;
            }
#else
            if (ctx && ctx->in_idle && ctx->vp_cnt > 0) {
                APPS_LOG_MSGID_I(SC_TAG" vp play, vp_cnt:%d", 1, ctx->vp_cnt);
                ctx->vp_cnt--;
                apps_config_set_voice(APP_LEAKAGE_DETECTION_VP_INDEX, true, 200, VOICE_PROMPT_PRIO_ULTRA, false, false);

                if (ctx->vp_cnt > 0) {
                    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP,
                                        NULL, 0, NULL,
                                        APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL);
                }
                if (ctx->vp_cnt == 0 && role == BT_AWS_MCE_ROLE_PARTNER) {
                    ld_over = TRUE;
                }
            }
#endif

            if (ld_over) {
#ifdef MTK_AWS_MCE_ENABLE
                // Notify new Agent
                APPS_LOG_MSGID_I(SC_TAG" notify new agent LD over", 0);
                apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                         APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_STOP);
#endif
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_STATUS: {
            bool is_ongoing  = (bool)extra_data;
            if (!is_ongoing) {
                ctx->seal_checking = false;
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
                app_advance_passthrough_set_ld_ongoing(FALSE);
#endif
            }
            break;
        }
        default:
            break;
    }
    return ret;
}


static bool _proc_bt_state_event(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    app_leakage_detection_context_t *ctx = (app_leakage_detection_context_t *)self->local_context;

    if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
        bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *) extra_data;
        APPS_LOG_MSGID_I(SC_TAG" sink status changed, %d -> %d", 2,
                         param->previous, param->current);

        /* Leakage detection will stop when the HFP is active. */
        if ((param->previous < BT_SINK_SRV_STATE_INCOMING) && (param->current >= BT_SINK_SRV_STATE_INCOMING)) {
            ctx->in_idle = false;
            leakage_detection_over(ctx);
        }

        if (param->current < BT_SINK_SRV_STATE_INCOMING) {
            ctx->in_idle = true;
        }
    }

    return false;
}

static bool app_leakage_detection_idle_proc_bt_cm_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    app_leakage_detection_context_t *ctx = (app_leakage_detection_context_t *)self->local_context;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;

            bt_cm_profile_service_mask_t pre_srv = remote_update->pre_connected_service;
            bt_cm_profile_service_mask_t cur_srv = remote_update->connected_service;
            bt_cm_profile_service_mask_t aws_mask = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
            if (((pre_srv & ~aws_mask) && !(cur_srv & ~aws_mask)) || ((pre_srv & aws_mask) && (cur_srv == 0))) {
                APPS_LOG_MSGID_I(SC_TAG" apps_config_stop_voice.", 0);
                /* Only stop LD VP when non-AWS profile disconnection happen. */
                if (APP_LEAKAGE_DETECTION_VP_INDEX == app_voice_prompt_get_current_index()) {
                    apps_config_stop_voice(false, 0, true);
                }
                if (ctx->seal_checking) {
                    APPS_LOG_MSGID_I(SC_TAG" LD terminated bt disconncted", 0);
                    app_seal_check_stop();
               }
            }
#ifdef MTK_AWS_MCE_ENABLE
            if (BT_AWS_MCE_ROLE_PARTNER == role || BT_AWS_MCE_ROLE_CLINET == role) {
                if ((pre_srv & aws_mask) && !(cur_srv & aws_mask)) {
                    /* Stop LD when partner aws disconnected. */
                    APPS_LOG_MSGID_I(SC_TAG" partner aws disconncted", 0);
                    app_seal_check_stop();
                }
            }
#endif
        }
        break;
        default:
            break;
    }

    return ret;
}

bool app_leakage_detection_idle_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    //APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc event_group : %x, id: %x", 2, event_group, event_id);
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            /* UI Shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            /* Interaction events. */
            ret = _proc_apps_internal_events(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_BT_SINK:
            /* BT_SINK events, indicates the state of A2DP and HFP. */
            ret = _proc_bt_state_event(self, event_id, extra_data, data_len);
            break;

        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            /* Event come from bt connection manager, indicates the connection state of HFP or AWS. */
            ret = app_leakage_detection_idle_proc_bt_cm_event_group(self, event_id, extra_data, data_len);
            break;

        default:
            break;
    }

    return ret;
}

#endif /* MTK_LEAKAGE_DETECTION_ENABLE */

