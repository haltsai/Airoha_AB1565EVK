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
 * File: app_fm_activity.c
 *
 * Description: This file is the activity to handle the action of find me LED and ringtone.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#include "app_fm_activity.h"
#include "timers.h"
#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_key_remapper.h"
#include "apps_events_event_group.h"
#include "app_bt_conn_componet_in_homescreen.h"
#include "app_home_screen_idle_activity.h"
#ifdef RACE_FIND_ME_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "race_cmd_find_me.h"
#endif
#endif
#include "bt_sink_srv_ami.h"
#include "bt_connection_manager.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "apps_aws_sync_event.h"
#include "bt_device_manager.h"
#endif

#define UI_SHELL_FM_ACTIVITY "[FIND_ME_APP]ui_shell_ut_fm_activity"

#define FINDME_EVENT_TIMER_INTERVAL    (4 * 1000)  /* Interval(ms) of ringtone playback. */

#define FINDME_TOTAL_TIME              (30 * 1000)  /* Default total time(ms) of findme, used when parameter duration_seconds == 0. */

static uint16_t s_fm_vp_id = 0;

static bool _proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);
static bool _proc_ui_shell_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);

/**
* @brief      This function is to play find me ringtone and set LED.
* @param[in]  self, the context pointer of the activity, can't be NULL.
*/
static void apps_set_find_me(ui_shell_activity_t *self);

/**
* @brief      This function is to stop find me.
*/
static void apps_stop_find_me(void);

/**
* @brief      This function is to start find me.
* @param[in]  self, the context pointer of the activity, can't be NULL.
* @param[in]  extra_data, find me parameter, should point to struct app_find_me_param_struct, can't be NULL.
*/
static void apps_do_find_me(ui_shell_activity_t *self, void *extra_data);

void app_find_me_notify_state_change(app_find_me_context_t *context, bool need_aws_sync)
{
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    if (need_aws_sync) {
        app_find_me_notify_state_t state = {
            .blink = context->blink,
            .tone = context->tone,
        };
        apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_NOTIFY_FIND_ME_STATE, &state, sizeof(app_find_me_notify_state_t));
    }
    APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", notify state change blink-tone:%d-%d, peer:%d-%d", 4, context->blink, context->tone, context->peer_blink, context->peer_tone);
#endif
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_NOTIFY_FIND_ME_STATE, NULL, 0, NULL, 0);
}


static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", create", 0);
#ifdef RACE_FIND_ME_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            /* init RHO callback of race cmd, RHO is not allowed when doing find me. */
            race_cmd_find_me_init();
#endif
#endif
            self->local_context = extra_data;
            app_find_me_param_struct find_me_param = {
                .blink = ((app_find_me_context_t *)self->local_context)->blink,
                .tone = ((app_find_me_context_t *)self->local_context)->tone,
                .duration_seconds = ((app_find_me_context_t *)self->local_context)->duration_seconds
            };
            /* Do not memset sink_state. */
            ((app_find_me_context_t *)self->local_context)->blink = 0;
            ((app_find_me_context_t *)self->local_context)->tone = 0;
            ((app_find_me_context_t *)self->local_context)->duration_seconds = 0;
            /* The first find me. */
            apps_do_find_me(self, &find_me_param);

            /* LED will display in apps_do_find_me(), so only trigger MMI updating. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", destroy", 0);
#ifdef RACE_FIND_ME_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            /* Deinit RHO callback, allow RHO. */
            race_cmd_find_me_deinit();
#endif
#endif
            app_find_me_context_t *context = (app_find_me_context_t *)(self->local_context);
            context->blink = false;
            context->tone = false;
            app_find_me_notify_state_change(context, true);
            /* Notify other activities to update LED and MMI state. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);

            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
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
    /* Decode event_id to key_id and key_event. */
    app_event_key_event_decode(&key_id, &key_event, event_id);
    apps_config_key_action_t action;
    if (extra_data) {
        /* Key event send by other module, e.g.race cmd. */
        action = *(uint16_t *)extra_data;
    } else {
        /* Actual key triggered event. */
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }
    if (action == KEY_STOP_FIND_ME) {
        /* key to stop find me, refer to customerized_key_config.c. */
        apps_stop_find_me();
        ui_shell_finish_activity(self, self);
        ret = true;
    }
    return ret;
}

static void apps_do_find_me(ui_shell_activity_t *self, void *extra_data)
{
    app_find_me_param_struct *param = (app_find_me_param_struct *)extra_data;
    app_find_me_context_t *local_context = (app_find_me_context_t *)self->local_context;
    if (extra_data) {
        APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", apps_do_find_me: blink: %x, tone : %x", 2, param->blink, param->tone);
        uint8_t old_blink = local_context->blink;
        uint8_t old_tone = local_context->tone;
        APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", old state, blink: %x, tone: %x", 2, old_blink, old_tone);
        local_context->blink = param->blink;
        local_context->tone = param->tone;
        local_context->duration_seconds = param->duration_seconds;
        /* Set count to 0 as begin */
        local_context->count = 0;
        if (old_tone && !local_context->tone) {
            apps_stop_find_me();
        }
        if (!local_context->blink && !local_context->tone) {
            /* If find me need no ringtone and no LED, then no need to do, there must be some error. */
            ui_shell_finish_activity(self, self);
        } else {
            if (old_blink != local_context->blink) {
                /* If the previous(stopped by the current) find me set LED but the current no need LED, send event to update. */
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0, NULL, 0);
            }
            /* Play ringtone and/or set LED. */
            if (!old_tone && local_context->tone) {
                apps_set_find_me(self);
            }
        }
        if ((old_blink != local_context->blink || old_tone != local_context->tone) && (local_context->blink || local_context->tone)) {
            app_find_me_notify_state_change(local_context, true);
        }
    }
}

static void apps_stop_find_me(void)
{
    /* Remove event of notifying next round ringtone loop. */
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_PLAY_FIND_ME_TONE);
    if (s_fm_vp_id > 0) {
        apps_config_stop_vp(s_fm_vp_id, false, 0);
    }
}

static void apps_set_find_me(ui_shell_activity_t *self)
{
    app_find_me_context_t *local_context = (app_find_me_context_t *)self->local_context;
    APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", blink: %d, tone : %d", 2, local_context->tone, local_context->tone);
    if (local_context->tone) {
        /* Set a no repeat VP. */
        s_fm_vp_id = apps_config_set_vp(VP_INDEX_DOORBELL, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
    }
    /* Send a delay event as a timer to loop ringtone playback. */
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_PLAY_FIND_ME_TONE, NULL, 0,
                        NULL, FINDME_EVENT_TIMER_INTERVAL);
}

static void apps_fm_play_tone(ui_shell_activity_t *self)
{
    app_find_me_context_t *local_context = (app_find_me_context_t *)self->local_context;
    APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", apps_fm_play_tone: count: %x", 1, local_context->count);

    local_context->count++;
    if (local_context->count >= ((local_context->duration_seconds > 0
                                  ? (local_context->duration_seconds * 1000)
                                  : FINDME_TOTAL_TIME) / FINDME_EVENT_TIMER_INTERVAL)) {
        /* If the current count is over than the configured duration or default maximum, finish find me. */
        APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", play tone is finished", 0);
        ui_shell_finish_activity(self, self);
    } else {
        /* Start a new loop round. */
        apps_set_find_me(self);
    }
}

bool app_find_me_do_find_me_action(bool enable, app_find_me_config_t config)
{
    bool ret = TRUE;
    audio_channel_t channel = ami_get_audio_channel();
    app_find_me_param_struct *find_me_param = (app_find_me_param_struct *)pvPortMalloc(sizeof(app_find_me_param_struct));
    if (!find_me_param) {
        return ret;
    }
    find_me_param->blink = enable;
    find_me_param->tone = enable;
    find_me_param->duration_seconds = 0;

    APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY" find_me: enable=%d, channel=%d, config=%d, blink=%d, tone=%d",
            5, enable, channel, config, find_me_param->blink, find_me_param->tone);
    if ((AUDIO_CHANNEL_L == channel) || (AUDIO_CHANNEL_NONE == channel)) {
        if (FIND_ME_LEFT == config) {
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_FINDME,
                    APP_FIND_ME_EVENT_ID_TRIGGER, find_me_param, sizeof(app_find_me_param_struct), NULL, 0);
        } else if (FIND_ME_RIGHT == config) {
            /* Sent find me data to the other side. */
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
                apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_FIND_ME_APP_SYNC_FIND_ME_ACTION, find_me_param, sizeof(app_find_me_param_struct));
            }
#endif
            vPortFree(find_me_param);
        } else {
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_FINDME,
                    APP_FIND_ME_EVENT_ID_TRIGGER, find_me_param, sizeof(app_find_me_param_struct), NULL, 0);
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
                apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_FIND_ME_APP_SYNC_FIND_ME_ACTION, find_me_param, sizeof(app_find_me_param_struct));
            }
#endif
        }
    } else if (AUDIO_CHANNEL_R == channel) {
        if (FIND_ME_LEFT == config) {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            /* Sent find me data to the other side. */
            if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
                apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_FIND_ME_APP_SYNC_FIND_ME_ACTION, find_me_param, sizeof(app_find_me_param_struct));
            }
#endif
            vPortFree(find_me_param);
        } else if (FIND_ME_RIGHT == config) {
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_FINDME,
                    APP_FIND_ME_EVENT_ID_TRIGGER, find_me_param, sizeof(app_find_me_param_struct), NULL, 0);
        } else {
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_FINDME,
                    APP_FIND_ME_EVENT_ID_TRIGGER, find_me_param, sizeof(app_find_me_param_struct), NULL, 0);
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
                apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_FIND_ME_APP_SYNC_FIND_ME_ACTION, find_me_param, sizeof(app_find_me_param_struct));
            }
#endif
        }
    } else {
        ret = FALSE;
        APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY" error channel=%d ", 1, channel);
    }
    return ret;
}

bool app_find_me_activity_proc(
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
        case EVENT_GROUP_UI_SHELL_KEY: {
            /* Key event. */
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_FINDME: {
            /* Find me event */
            APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", event_id : %x", 1, event_id);
            if (event_id == APP_FIND_ME_EVENT_ID_TRIGGER) {
                apps_do_find_me(self, extra_data);
                ret = true;
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            /* APP interaction event. */
            if (event_id == APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN) {
                /* This event is send by other app, just update. */
                app_find_me_context_t *local_context = (app_find_me_context_t *)self->local_context;
                APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY",[reset_state] timer is in , find me doing", 0);
                if (local_context->blink) {
                    apps_config_set_background_led_pattern(LED_INDEX_FIND_ME, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_HIGH);
                    ret = true;
                }
            } else if (event_id == APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE) {
                /* Update MMI state to find me. */
                apps_config_key_set_mmi_state(APP_STATE_FIND_ME);
                ret = true;
            } else if (event_id == APPS_EVENTS_INTERACTION_PLAY_FIND_ME_TONE) {
                /* Ringtone loop timer expired, play vp. */
                apps_fm_play_tone(self);
                ret = true;
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* bt_sink event. */
            if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
                bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
                if (param == NULL) {
                    APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", bt sink state change param null", 0);
                    return ret;
                }
                if (param->current >= BT_SINK_SRV_STATE_INCOMING) {
                    /* Stop find me when call incomes. */
                    APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", call, stop find me", 0);
                    apps_stop_find_me();
                    ui_shell_finish_activity(self, self);
                }
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            if (event_id == BT_CM_EVENT_REMOTE_INFO_UPDATE) {
                bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
                if (remote_update->pre_acl_state && !remote_update->acl_state) {
                    if (
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
                        (bt_device_manager_aws_local_info_get_role() & ~BT_AWS_MCE_ROLE_AGENT) ||
#endif
                        bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0) == 0

                    ) {
                        APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", bt disconnected, stop find me", 0);
                        apps_stop_find_me();
                        ui_shell_finish_activity(self, self);
                    }

                }
            }
            break;
        }
        default:
            break;
    }

    /* APPS_LOG_MSGID_I(UI_SHELL_FM_ACTIVITY", ret : %x", 1, ret); */
    return ret;
}


