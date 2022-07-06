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
 * File: app_leakage_detection_idle_activity.c
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

#include "app_leakage_detection_idle_activity.h"
#include "app_leakage_detection_utils.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "app_voice_prompt.h"
#include "bt_sink_srv.h"
#include "apps_config_event_list.h"
#include "apps_debug.h"
#include "ui_shell_manager.h"
#include "apps_config_vp_manager.h"
#include "airo_key_event.h"
#include "bt_sink_srv_ami.h"
#include "leakage_detection_control.h"

static app_leakage_detection_context_t s_app_leakage_detection_conext;  /* The variable records context. */
app_leakage_detection_context_t *get_context()
{
    return &s_app_leakage_detection_conext;
}


/**
* @brief      This function is called by the middleware at the the beginning of leakage detection.
* @param[in]  leakage_status, the leakage detection status.
* @return     None.
*/
static void leakage_detection_callback(uint16_t leakage_status)
{
    bool *p_play_flag = NULL;
    bt_status_t bt_status;
    app_leakage_detection_context_t *ctx = NULL;

    ctx = get_context();

    /* Stop A2DP before leakage detection starts. */
    bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PAUSE, NULL);
    if (bt_status != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc send pause action failed.", 0);
    }

    /* Check if the A2DP needs to be resumed at the end of leakage detection. */
    if (ctx->is_a2dp_playing) {
        ctx->need_resume_a2dp = true;
    } else {
        ctx->need_resume_a2dp = false;
    }

    /* Play leakage detection VP with 1500ms delay. */
    p_play_flag = (bool *)pvPortMalloc(sizeof(bool));
    if (p_play_flag == NULL) {
        APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc malloc mem failed.", 0);
        return;
    }
    *p_play_flag = true;
    ui_shell_status_t status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                   APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER,
                                                   p_play_flag, sizeof(bool), NULL, 1500);
    if (UI_SHELL_STATUS_OK != status) {
        vPortFree(p_play_flag);
        APPS_LOG_MSGID_I("UI_SHELL send race cmd vp trigger event failed.", 0);
    }
}

static bool _proc_ui_shell_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);


static void leakage_detection_over(app_leakage_detection_context_t *ctx)
{
    bt_status_t bt_status;

    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER);
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP);
#if 1
#ifndef APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL
    apps_config_stop_voice(true, 0, true);
#else
    apps_config_stop_voice(true, 0, false);
#endif
#endif
    ctx->vp_cnt = 0;

    if (ctx->vp_playing) {
        /* A2DP cannot be resume until both VP and seal check are finished. */
        APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, seal check over, vp still playing.", 0);
        return;
    }
    APPS_LOG_MSGID_I("[RECORD_LC]audio_anc_leakage_detection_resume_dl in stop_vp.", 0);
    audio_anc_leakage_detection_resume_dl();

    /* Try to resume A2DP. */
    if (ctx->need_resume_a2dp) {
        APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, resume a2dp after seal check finished.", 0);
        bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PLAY, NULL);
        if (bt_status != BT_STATUS_SUCCESS) {
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc send play action failed.", 0);
        }
        ctx->need_resume_a2dp = false;
    }

}


static void vp_callback(uint32_t idx, vp_err_code err)
{
    bt_status_t bt_status;
    app_leakage_detection_context_t *ctx = NULL;
    ctx = get_context();
#if 0
    if (err != VP_ERR_CODE_START_PLAY) {
        ctx->vp_playing = false;
    }
#endif
    /* Stop the leakage detection when the VP of leakage detection is interrupted by others VP. */
    if (idx == APP_LEAKAGE_DETECTION_VP_INDEX && err == VP_ERR_CODE_PREEMPTED) {
        APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, vp stoped", 0);
        audio_anc_leakage_detection_stop();
    }

    if (idx == APP_LEAKAGE_DETECTION_VP_INDEX && (err == VP_ERR_CODE_SUCCESS || err == VP_ERR_CODE_STOP)) {
        if (ctx->seal_checking) {
            /* A2DP cannot be resume until both VP and seal check are finished. */
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, vp stoped, seal check working.", 0);
            return;
        }

        APPS_LOG_MSGID_I("[RECORD_LC]audio_anc_leakage_detection_resume_dl in vp_callback.", 0);
        audio_anc_leakage_detection_resume_dl();

        /* Try to resume A2DP. */
        if (ctx->need_resume_a2dp) {
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, resume a2dp after vp finished.", 0);
            bt_status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PLAY, NULL);
            if (bt_status != BT_STATUS_SUCCESS) {
                APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc send play action failed.", 0);
            }
            ctx->need_resume_a2dp = false;
        }
    }
}


static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = true;
    app_leakage_detection_context_t *ctx = NULL;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, create", 0);
            self->local_context = &s_app_leakage_detection_conext;
            memset(self->local_context, 0, sizeof(app_leakage_detection_context_t));
            ctx = (app_leakage_detection_context_t *)self->local_context;
            ctx->in_idle = true;
            ctx->is_a2dp_playing = false;
            audio_anc_leakage_detection_register_vp_start_callback(leakage_detection_callback);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, result", 0);
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
                APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, trigger vp play, flag:%d",
                                 1, *p_play_flag);
                if (*p_play_flag) {
                    /* Play the VP of leakage detection at the beginning of leakage detection. */
                    ctx->seal_checking = true;
                    ctx->vp_playing = true;
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
                    /* The leakage detection is finished. */
                    ctx->seal_checking = false;
                    leakage_detection_over(ctx);
                }
            } else {
                audio_anc_leakage_detection_stop();
            }

            break;

        case APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP:
            ret = true;
#ifndef APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL
            leakage_detection_over(ctx);
#else
            if (ctx && ctx->in_idle && ctx->vp_cnt > 0) {
                APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, vp play, vp_cnt:%d", 1, ctx->vp_cnt);
                ctx->vp_cnt--;
                apps_config_set_voice(APP_LEAKAGE_DETECTION_VP_INDEX, true, 200, VOICE_PROMPT_PRIO_ULTRA, false, false);

                if (ctx->vp_cnt > 0) {
                    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP,
                                        NULL, 0, NULL,
                                        APP_LEAKAGE_DETECTION_VP_REPEAT_INTERVAL);
                }
            }
#endif
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
        APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, sink status changed, %d -> %d", 2,
                         param->previous, param->current);

        /* Leakage detection will stop when the HFP is active. */
        if ((param->previous < BT_SINK_SRV_STATE_INCOMING) && (param->current >= BT_SINK_SRV_STATE_INCOMING)) {
            ctx->in_idle = false;
            ctx->is_a2dp_playing = false;
            leakage_detection_over(ctx);
        }

        if (param->current < BT_SINK_SRV_STATE_INCOMING) {
            ctx->in_idle = true;
        }

        /* Check A2DP state. */
        if (param->current <= BT_SINK_SRV_STATE_CONNECTED) {
            ctx->is_a2dp_playing = false;
        } else if (param->current == BT_SINK_SRV_STATE_STREAMING) {
            if (ctx->seal_checking) {
                APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, LD terminated by A2DP", 0);
                audio_anc_leakage_detection_stop();
                ctx->need_resume_a2dp = false;
            }
            ctx->is_a2dp_playing = true;
        }
    }

    return false;
}

#if 0
static bool _proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    static bool flag = false;
    bool ret = false;
    uint8_t key_id;
    airo_key_event_t key_event;
    apps_config_key_action_t action = KEY_ACTION_INVALID;
    app_leakage_detection_context_t *ctx = (app_leakage_detection_context_t *)self->local_context;

    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }

    if (action == KEY_TEST) {
        APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc event_group key event", 0);
        if (!flag) {
            ctx->in_idle = true;
            bool *p_play_flag = (bool *)pvPortMalloc(sizeof(bool));
            *p_play_flag = true;
            ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER,
                                p_play_flag, sizeof(bool), NULL, 0);
        } else {
            ctx->in_idle = false;
            stop_vp(ctx);
        }
        flag = !flag;
    }
    return ret;

}
#endif

static bool app_leakage_detection_idle_proc_bt_cm_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    app_leakage_detection_context_t *ctx = (app_leakage_detection_context_t *)self->local_context;
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;

            if (remote_update->pre_connected_service & (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
                && !(remote_update->connected_service & (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)))) {
                APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, apps_config_stop_voice.", 0);
                /* Only stop LD VP when non-AWS profile disconnection happen. */
                if (APP_LEAKAGE_DETECTION_VP_INDEX == app_voice_prompt_get_current_index()) {
                    apps_config_stop_voice(false, 0, true);
                }
                if (ctx->seal_checking) {
                    APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, LD terminated bt disconncted", 0);
                    audio_anc_leakage_detection_stop();
               }
            }
#ifdef MTK_AWS_MCE_ENABLE
            if (BT_AWS_MCE_ROLE_PARTNER == role || BT_AWS_MCE_ROLE_CLINET == role) {
                if (remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)
                    && !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))) {
                    /* Stop LD when partner aws disconnected. */
                    APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc, partner aws disconncted", 0);
                    audio_anc_leakage_detection_stop();
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
    APPS_LOG_MSGID_I("app_leakage_detection_idle_activity_proc event_group : %x, id: %x", 2, event_group, event_id);
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

#if 0
        /* ONLY FOR DEBUG. */
        case EVENT_GROUP_UI_SHELL_KEY:
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
#endif

        default:
            break;
    }

    return ret;
}

#endif /* MTK_LEAKAGE_DETECTION_ENABLE */

