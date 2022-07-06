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
 * File: app_hfp_activity.c
 *
 * Description:
 * This file is the activity to handle the key action or sink service event when call is active.
 * When the call is active or there is a call coming in, the app_hfp_idle_activity start this
 * activity to handle the call state change events and UI events of HFP.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "app_hfp_activity.h"
#include "app_hfp_utils.h"
#include "apps_events_event_group.h"
#include "apps_events_bt_event.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_key_remapper.h"
#include "apps_config_event_list.h"
#include "apps_config_key_remapper.h"
#include "apps_events_key_event.h"
#include "bt_aws_mce_srv.h"
#include "apps_aws_sync_event.h"
#include "bt_device_manager.h"
#include "bt_type.h"
#include "app_home_screen_idle_activity.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#include "bt_device_manager.h"

#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le.h"
#include "app_le_audio_aird_client.h"
#ifdef AIR_LE_AUDIO_MULTIPOINT_ENABLE
#include "bt_sink_srv_le_cap.h"
#endif
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_utils.h"
#endif
#include "bt_sink_srv_ami.h"
#ifdef AIR_ROTARY_ENCODER_ENABLE
#include "bsp_rotary_encoder.h"
#endif
#include "bt_hci.h"

#if defined(AIR_LE_AUDIO_ENABLE)

static void app_hfp_activity_ready_to_finish(hfp_context_t *hfp_ctx, ui_shell_activity_t *self)
{
    bt_handle_t le_handle = bt_sink_srv_cap_check_links_state(BT_SINK_SRV_CAP_STATE_CONNECTED);
    bool is_call_srv = ((hfp_ctx->le_audio_srv & BT_SINK_SRV_PROFILE_CALL) != 0);

    bool le_audio_call_ongoing = (le_handle != BT_HANDLE_INVALID && is_call_srv);
    APPS_LOG_MSGID_I(APP_HFP_ACTI" ready_to_finish, le_handle=%04X is_call_srv=%d -> le_audio_call_ongoing=%d",
                     3, le_handle, is_call_srv, le_audio_call_ongoing);
    if (le_audio_call_ongoing) {
        APPS_LOG_MSGID_I(APP_HFP_ACTI" ready_to_finish, ignore", 0);
        return;
    }
    if (hfp_ctx->transient_actived) {
        hfp_ctx->transient_actived = false;
        ui_shell_finish_activity(self, self);
    }
}

#endif

static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI", create : %x", 1, extra_data);
            self->local_context = extra_data;
            /* LED will display when call incoming or call is active, so trigger MMI updating. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI", destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {

            APPS_LOG_MSGID_I(APP_HFP_ACTI", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {

            APPS_LOG_MSGID_I(APP_HFP_ACTI", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {

            APPS_LOG_MSGID_I(APP_HFP_ACTI", result", 0);
            break;
        }
        default:
            ret = false;
            break;
    }
    return ret;
}

/**
* @brief      This function is used to handle the key action.
* @param[in]  state, the current sink_state.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_hfp_proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;

    uint8_t key_id;
    airo_key_event_t key_event;
    apps_config_key_action_t action;

    /* Decode event_id to key_id and key_event. */
    app_event_key_event_decode(&key_id, &key_event, event_id);

    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }
    APPS_LOG_MSGID_I(APP_HFP_ACTI", app_hfp_proc_key_event_group: action=0x%x", 1, action);

    /* Mute mic.*/
    if (action == KEY_MUTE_MIC) {
        hfp_context->mute_mic = !hfp_context->mute_mic;
        ret = app_hfp_mute_mic(hfp_context->mute_mic);
        if (ret == TRUE) {
            return ret;
        }
    }
    /* Send call action to sink srv.*/
    ret = app_hfp_send_call_action(action);

    return ret;
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
#ifdef AIR_LE_AUDIO_ENABLE
static void app_hfp_rotary_le_audio_volume(
        apps_config_key_action_t key_action,
        uint32_t rotary_data
        )
{
    app_le_audio_aird_client_info_t *p_info = NULL;
    bt_handle_t handle = BT_HANDLE_INVALID;

    if ((BT_HANDLE_INVALID != (handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established()))
            && (NULL != (p_info = app_le_audio_aird_client_get_info(handle)))
            && (APP_LE_AUDIO_AIRD_MODE_SUPPORT_HID_CALL == p_info->mode)) {

        app_le_audio_aird_action_set_streaming_volume_t param = {0};
        bt_status_t status = BT_STATUS_FAIL;
        app_le_audio_aird_action_t aird_action = APP_LE_AUDIO_AIRD_ACTION_MAX;

        if (KEY_VOICE_UP == key_action) {
            aird_action = APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_UP;
        } else {
            aird_action = APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_DOWN;
        }

        param.streaming_interface = APP_LE_AUDIO_AIRD_STREAMING_INTERFACE_SPEAKER;
        param.streaming_port      = APP_LE_AUDIO_AIRD_STREAMING_PORT_0;
        param.channel             = APP_LE_AUDIO_AIRD_CHANNEL_DUAL;
        param.volume              = rotary_data;   /* The delta value. */

        status = app_le_audio_aird_client_send_action(handle,
                aird_action, &param, sizeof(app_le_audio_aird_action_set_streaming_volume_t));

        APPS_LOG_MSGID_I(APP_HFP_ACTI" [LEA][CIS] rotary_le_audio_volume: status=0x%x, key_action=0x%x, rotary=%d",
                3, status, key_action, rotary_data);
    } else {
        APPS_LOG_MSGID_I(APP_HFP_ACTI" [LEA][CIS] rotary_le_audio_volume failed: handle=0x%x, p_info=0x%x, p_info->mode=%d",
                3, handle, p_info, (p_info == NULL) ? NULL : p_info->mode);
    }
}
#endif

static bool app_hfp_proc_rotary_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    bsp_rotary_encoder_port_t port;
    bsp_rotary_encoder_event_t event;
    uint32_t rotary_data;
    if (!extra_data) {
        return ret;
    }
    apps_config_key_action_t key_action = *(uint16_t *)extra_data;
    app_event_rotary_event_decode(&port, &event, &rotary_data, event_id);
    switch (key_action) {
        case KEY_VOICE_UP:
        case KEY_VOICE_DN: {
            uint32_t volume = bt_sink_srv_get_volume(NULL, BT_SINK_SRV_VOLUME_HFP);
            if (0xFFFFFFFF == volume) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI"app_hfp_proc_rotary_event_group, get hfp volume : %x", 1, volume);
            } else {
                if (KEY_VOICE_UP == key_action) {
                    if (volume + rotary_data <= AUD_VOL_OUT_MAX) {
                        volume += rotary_data;
                    } else {
                        volume = AUD_VOL_OUT_MAX - 1;
                    }
                } else {
                    if (volume > rotary_data) {
                        volume -= rotary_data;
                    } else {
                        volume = 0;
                    }
                }
                APPS_LOG_MSGID_I(APP_HFP_ACTI"app_hfp_proc_rotary_event_group, hfp target volume : %d", 1, volume);
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CALL_SET_VOLUME, &volume);
            }
#ifdef AIR_LE_AUDIO_ENABLE
            app_hfp_rotary_le_audio_volume(key_action, rotary_data);
#endif
            ret = true;
            break;
        }
        default:
            break;
    }

    return ret;
}
#endif

static bool app_hfp_proc_bt_sink_event(ui_shell_activity_t *self,
                                uint32_t event_group,
                                uint32_t event_id,
                                void *extra_data,
                                size_t data_len)
{
    bool ret = false;
    APPS_LOG_MSGID_I(APP_HFP_ACTI", event_id : %x", 1, event_id);

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t event_role = BT_AWS_MCE_ROLE_NONE;
    bt_event_suffix_data_t *suffix_data = get_bt_event_suffix_data(extra_data, data_len);
    if (suffix_data != NULL) {
        event_role = suffix_data->aws_role;
    }
#endif

    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
            hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
            if (param == NULL) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI" sink_state_change :param is null ", 0);
                return ret;
            }
            APPS_LOG_MSGID_I(APP_HFP_ACTI" sink_state_change: param_previous: %x, param_now: %x",
                    2, param->previous, param->current);
            /* Stop ringtone of incoming call and clear vp state when call up or hung up. */
#ifdef MTK_AWS_MCE_ENABLE
            if ((param->current < BT_SINK_SRV_STATE_INCOMING && hfp_context->curr_state == BT_SINK_SRV_STATE_INCOMING)
#if defined(AIR_MULTI_POINT_ENABLE)
                || (param->current < BT_SINK_SRV_STATE_INCOMING && hfp_context->curr_state == BT_SINK_SRV_STATE_TWC_INCOMING)
#endif
                ) {
                app_voice_prompt_set_voice_stopping(true);
            } else if (param->current ==  BT_SINK_SRV_STATE_INCOMING || param->current == BT_SINK_SRV_STATE_TWC_INCOMING) {
                app_voice_prompt_set_voice_stopping(false);
            }
#endif

            if (param->current != BT_SINK_SRV_STATE_INCOMING && param->current != BT_SINK_SRV_STATE_TWC_INCOMING) {
#ifdef MTK_AWS_MCE_ENABLE
                app_hfp_stop_vp_by_aws_role(event_role);
#else
                app_hfp_stop_vp();
#endif
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_AUTO_ACCEPT_INCOMING_CALL);
                /* remove event of notifying next round twc incoming call vp loop. */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP);
            }

//#if defined(AIR_MULTI_POINT_ENABLE)
//            if (param->current != BT_SINK_SRV_STATE_TWC_INCOMING) {
//#ifdef MTK_AWS_MCE_ENABLE
//                app_hfp_stop_vp_by_aws_role(event_role);
//#else
//                app_hfp_stop_vp();
//#endif
//                /* remove event of notifying next round twc incoming call vp loop. */
//                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP);
//            }
//#endif
            /* Check the call status, if there are no HFP related states, finish current activity. */
            if (param->current <= BT_SINK_SRV_STATE_STREAMING) {
                /* Call is end */
#if defined(AIR_MULTI_POINT_ENABLE) && defined(MTK_AWS_MCE_ENABLE)
                if(BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()
                        && app_hfp_get_aws_link_is_switching()) {
                    /* Partner don't finish during switch aws link. */
                    ;
                } else {
                    /* Agent send hfp state to partner before finish hfp activity,
                     * because partner don't finish activity by bt_sink_srv hfp. */
                    if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                        bt_status_t send_state = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_SYNC_HFP_STATE_TO_PARTNER, param, sizeof(bt_sink_srv_state_change_t));
                        if (BT_STATUS_SUCCESS == send_state) {
                            APPS_LOG_MSGID_I(APP_HFP_ACTI", agent send hfp_state_change to partner:param->previous: %x,param->current: %x",
                                    2, param->previous,param->current);
                        }
                    }
                    if (true == hfp_context->transient_actived) {
                        hfp_context->transient_actived = false;
                        ui_shell_finish_activity(self, self);
                    }
                }
#else
                if (true == hfp_context->transient_actived) {
                    hfp_context->transient_actived = false;
                    ui_shell_finish_activity(self, self);
                }
#endif
            } else {
                apps_config_state_t state = app_get_config_status_by_state(param->current);
                APPS_LOG_MSGID_I(APP_HFP_ACTI", state_change:is_hfp: %x", 1, state);
#if defined(AIR_MULTI_POINT_ENABLE) && defined(MTK_AWS_MCE_ENABLE)
                if(BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()
                        && app_hfp_get_aws_link_is_switching()) {
                    break;
                } else {
                    if (state != APP_TOTAL_STATE_NO) {
                        APPS_LOG_MSGID_I(APP_HFP_ACTI", enter self", 0);
                        ((hfp_context_t *)(self->local_context))->pre_state =  param->previous;
                        ((hfp_context_t *)(self->local_context))->curr_state = param->current;
                    }
                }

#else
                if (state != APP_TOTAL_STATE_NO) {
                    APPS_LOG_MSGID_I(APP_HFP_ACTI", enter self", 0);
                    ((hfp_context_t *)(self->local_context))->pre_state = param->previous;
                    ((hfp_context_t *)(self->local_context))->curr_state = param->current;
                }
#endif
            }

            /* Udpate LED display and MMI state when sink service state changed. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
            break;
        }
        case BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION: {
#if defined(AIR_MULTI_POINT_ENABLE)
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            bt_sink_srv_caller_information_t *call_info = (bt_sink_srv_caller_information_t *)extra_data;
            APPS_LOG_MSGID_I(APP_HFP_ACTI" Caller info incoming address: %02X:%02X:%02X:%02X:%02X:%02X", 6, call_info->address[5],
                    call_info->address[4], call_info->address[3], call_info->address[2], call_info->address[1], call_info->address[0]);
            APPS_LOG_MSGID_I(APP_HFP_ACTI" Caller info context address: %02X:%02X:%02X:%02X:%02X:%02X", 6, hfp_context->activate_con_addr[5],
                                hfp_context->activate_con_addr[4], hfp_context->activate_con_addr[3], hfp_context->activate_con_addr[2],
                                hfp_context->activate_con_addr[1], hfp_context->activate_con_addr[0]);
            if (0 != memcmp(call_info->address, hfp_context->activate_con_addr, sizeof(bt_bd_addr_t))) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI "Remote address don't common", 0);
                if ((hfp_context->curr_state == BT_SINK_SRV_STATE_TWC_INCOMING)
                        && (hfp_context->pre_state != BT_SINK_SRV_STATE_TWC_INCOMING)) {
                    /* Use RING_ID and TWC_RING_IND event to play incoming or twc incoming call vp.*/
                    //APPS_LOG_MSGID_I(APP_HFP_ACTI" TWC incoming call VP", 0);
#ifndef MTK_AWS_MCE_ENABLE
//                    apps_config_set_vp(VP_INDEX_TWC_INCOMING_CALL, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
//                    /* send a delay event as a timer to loop twc incoming call playback. */
//                    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
//                            APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP, NULL, 0,
//                            NULL, TWC_INCOMING_CALL_VP_TIMER_INTERVAL);
#endif
                }
            }
#endif
            break;
        }
        /* Play the ringtone of incoming call when the smartphone don't support in-band tone. */
        case BT_SINK_SRV_EVENT_HF_RING_IND: {
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            bt_sink_srv_event_hf_ring_ind_t *ring_ind = (bt_sink_srv_event_hf_ring_ind_t *)extra_data;
            memcpy(hfp_context->activate_con_addr, ring_ind->address, sizeof(bt_bd_addr_t));
            APPS_LOG_MSGID_I(APP_HFP_ACTI",[RING_IND] pre_state: %x, cur_state : %x", 2, hfp_context->pre_state, hfp_context->curr_state);
            if (hfp_context->curr_state == BT_SINK_SRV_STATE_INCOMING && hfp_context->pre_state != BT_SINK_SRV_STATE_INCOMING) {
                app_hfp_incoming_call_vp_process(self, TRUE, FALSE);
            }
            break;
        }
#if defined(AIR_MULTI_POINT_ENABLE) && defined(MTK_AWS_MCE_ENABLE)
        case BT_SINK_SRV_EVENT_HF_TWC_RING_IND: {
            hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
            bt_sink_srv_event_hf_twc_ring_ind_t *ring_ind = (bt_sink_srv_event_hf_twc_ring_ind_t *)extra_data;
            APPS_LOG_MSGID_I(APP_HFP_ACTI",[RING_IND] twc_ring_ind sink state: pre_state=%x, cur_state=%x, play_vp=%d",
                    3, hfp_context->pre_state, hfp_context->curr_state,  ring_ind->play_vp);
            if (ring_ind->play_vp == TRUE) {
                if (hfp_context->curr_state == BT_SINK_SRV_STATE_INCOMING
                        || hfp_context->curr_state == BT_SINK_SRV_STATE_TWC_INCOMING) {
                    app_hfp_incoming_call_vp_process(self, TRUE, TRUE);
                }
            } else if (ring_ind->play_vp == FALSE) {
#if (defined MTK_PROMPT_SOUND_ENABLE)
                uint8_t curr_vp_index = app_voice_prompt_get_current_index();
                if (curr_vp_index == VP_INDEX_INCOMING_CALL
                        || curr_vp_index == VP_INDEX_TWC_INCOMING_CALL) {
#ifdef MTK_AWS_MCE_ENABLE
                    app_voice_prompt_set_voice_stopping(true);
                    app_hfp_stop_vp_by_aws_role(event_role);
#else
                    app_hfp_stop_vp();
#endif
                }
#endif
            }
            break;
        }
#endif
        /* Play the conflict vp  when incoming call from sp1 and sp2 at the same time.*/
#if defined(AIR_MULTI_POINT_ENABLE)  && defined(AIR_EMP_AUDIO_INTER_STYLE_ENABLE)
        case BT_SINK_SRV_EVENT_HF_STATE_CONFLICT: {
            ret = app_hfp_proc_conflict_vp_event(self, event_group, event_id, extra_data, data_len);
            break;
        }
#endif
#if defined(AIR_LE_AUDIO_ENABLE) && defined(AIR_LE_AUDIO_CIS_ENABLE)
        case LE_SINK_SRV_EVENT_REMOTE_INFO_UPDATE: {
            hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
            bt_le_sink_srv_event_remote_info_update_t *update_ind = (bt_le_sink_srv_event_remote_info_update_t *)extra_data;
            if (update_ind == NULL || hfp_context == NULL) {
                break;
            }

            if (((update_ind->pre_connected_service & BT_SINK_SRV_PROFILE_CALL) > 0)
                    && ((update_ind->connected_service & BT_SINK_SRV_PROFILE_CALL) == 0)) {
                bt_sink_srv_state_t bt_sink_state = bt_sink_srv_get_state();
                bt_handle_t le_handle = BT_HANDLE_INVALID;
#ifdef AIR_LE_AUDIO_MULTIPOINT_ENABLE
                le_handle = bt_sink_srv_cap_check_links_state(BT_SINK_SRV_CAP_STATE_CONNECTED);
#endif
                APPS_LOG_MSGID_I(APP_HFP_ACTI"[LEA] BLE call disconnect, bt_sink_state=%d le_handle=0x%04X",
                                 2, bt_sink_state, le_handle);
                /* No any BLE link is connected. */
                if (bt_sink_state < BT_SINK_SRV_STATE_INCOMING && le_handle == BT_HANDLE_INVALID) {
#ifdef MTK_AWS_MCE_ENABLE
                    app_voice_prompt_set_voice_stopping(true);
                    app_hfp_stop_vp_by_aws_role(event_role);
#else
                    app_hfp_stop_vp();
#endif
                    if (true == hfp_context->transient_actived) {
                        hfp_context->transient_actived = false;
                        ui_shell_finish_activity(self, self);
                    }
                }
            }
        }
        break;
#endif
        default: {
            ret = false;
            break;
        }
    }
    APPS_LOG_MSGID_I(APP_HFP_ACTI" sink_srv_proc_ret = %x", 1, ret);
    return ret;
}

static bool app_hfp_proc_bt_cm_event(ui_shell_activity_t *self,
                                     uint32_t event_id,
                                     void *extra_data,
                                     size_t data_len)
{
    bool ret = false;
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
#if defined(MTK_AWS_MCE_ENABLE)
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                      && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    hfp_context->aws_link_state = TRUE;
                    if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                        /* Partner re-attached, retry to update mmi state. */
                        APPS_LOG_MSGID_I(APP_HFP_ACTI" app_hfp_proc_bt_cm_event normal AWS connect, update mmi state.", 0);
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
                        app_hfp_sync_incoming_call_vp_flag();

                        /* Play sync incoming call vp fail because of switch aws link.
                         * Retry to play sync vp  when aws link connected again.*/
                        bool is_twc_incoming_call_vp = false;
                        bool is_long_time_vp         = false;
                        if (hfp_context->curr_state == BT_SINK_SRV_STATE_TWC_INCOMING) {
                            is_twc_incoming_call_vp = true;
                        }
                        uint32_t vp_index = (is_twc_incoming_call_vp == TRUE) ? VP_INDEX_TWC_INCOMING_CALL : VP_INDEX_INCOMING_CALL;

                        app_hfp_incoming_call_vp_play_status_t vp_play_status = app_hfp_get_incoming_call_vp_flag();
                        //uint32_t vp_delay_time = (vp_play_status != APP_HFP_INCOMING_CALL_VP_SHORT) ? APP_HFP_INCMOING_CALL_VP_LONG_DELAY_TIME : APP_HFP_INCMOING_CALL_VP_SHORT_DELAY_TIME;
                        is_long_time_vp        = (vp_play_status != APP_HFP_INCOMING_CALL_VP_SHORT) ? true : false;
                        APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND][BT_CM] vp_play_status=%d, is_twc_incoming_call_vp=%d, vp_index=%d",
                                                       3, vp_play_status, is_twc_incoming_call_vp, vp_index);
                        if (BT_SINK_SRV_STATE_INCOMING == hfp_context->curr_state
                                || hfp_context->curr_state == BT_SINK_SRV_STATE_TWC_INCOMING) {
                            if ((APP_HFP_INCOMING_CALL_VP_SYNC_PLAY_FAIL == app_hfp_get_incoming_call_vp_sync_flag())
                                    || (vp_play_status != APP_HFP_INCOMING_CALL_VP_STOP)) {
                                APPS_LOG_MSGID_I(APP_HFP_ACTI" [RING_IND][BT_CM] incoming call vp sync again when aws reconnected", 0);
                                apps_config_stop_voice(true, 200, true);
                                app_hfp_incoming_call_vp_process(self, is_long_time_vp,is_twc_incoming_call_vp);
                                //apps_config_set_voice(vp_index, true, vp_delay_time, VOICE_PROMPT_PRIO_ULTRA, true, true, NULL);
                                //vp_play_status = (vp_delay_time == APP_HFP_INCMOING_CALL_VP_LONG_DELAY_TIME) ? APP_HFP_INCOMING_CALL_VP_LONG : APP_HFP_INCOMING_CALL_VP_SHORT;
                                //app_hfp_set_incoming_call_vp_flag(vp_play_status);
                                app_hfp_set_incoming_call_vp_sync_flag(APP_HFP_INCOMING_CALL_VP_SYNC_SUCCESS);
                                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_INCOMING_CALL_DELAY);
                            }
                        }
                    }
                } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                        && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    hfp_context->aws_link_state = FALSE;
                    APPS_LOG_MSGID_I(APP_HFP_ACTI" app_hfp_proc_bt_cm_event Partner Detached.", 0);
                }
            } else {
                if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    APPS_LOG_MSGID_I(APP_HFP_ACTI" app_hfp_proc_bt_cm_event Partner aws disconnected: reason=0x%02x,"
                            , 1, remote_update->reason);
                    hfp_context->aws_link_state = FALSE;
                    if (remote_update->reason == BT_HCI_STATUS_REMOTE_USER_TERMINATED_CONNECTION) {
#if defined(AIR_LE_AUDIO_ENABLE)
                        app_hfp_activity_ready_to_finish(hfp_context, self);
#else
                        if (true == hfp_context->transient_actived) {
                            hfp_context->transient_actived = false;
                            ui_shell_finish_activity(self, self);
                        }
#endif
                    }
                }
            }
#else
            /* Check the HFP connection state, if the HFP is no longer connected, finish current activity.*/
            if (remote_update
                    && (remote_update->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP))
                    && !(remote_update->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP))) {
                APPS_LOG_MSGID_I(APP_HFP_ACTI", hfp_event_proc:Don't care", 0);
                if (memcmp(&hfp_context->activate_con_addr, &remote_update->address, sizeof(bt_bd_addr_t)) == 0) {
#ifdef MTK_AWS_MCE_ENABLE
                    app_voice_prompt_set_voice_stopping(true);
#endif
                    app_hfp_stop_vp();
                    hfp_context->hfp_connected = false;
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

#ifdef MTK_AWS_MCE_ENABLE
static bool app_hfp_proc_aws_data(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t action;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        //apps_aws_sync_event_decode(aws_data_ind, &event_group, &action);
        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action,
                        &p_extra_data, &extra_data_len);
        APPS_LOG_MSGID_I("Received action = %x to paly reject call, role = %x, event_group = %x", 3, action, role, event_group);
#if defined(AIR_MULTI_POINT_ENABLE) && defined(MTK_AWS_MCE_ENABLE)
        if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && action == APPS_EVENTS_INTERACTION_SYNC_HFP_STATE_TO_PARTNER) {

            if (BT_AWS_MCE_ROLE_PARTNER == bt_connection_manager_device_local_info_get_aws_role()) {
                bt_sink_srv_state_change_t *hfp_state_change = (bt_sink_srv_state_change_t *)p_extra_data;
                hfp_context_t *hfp_context = (hfp_context_t *)self->local_context;
                if (hfp_context == NULL) {
                    return ret;
                }
                hfp_context->pre_state = hfp_state_change->previous;
                hfp_context->curr_state = hfp_state_change->current;
                APPS_LOG_MSGID_I(APP_HFP_ACTI", receive agent hfp state_change:pre_state: %x,curr_state: %x", 2, hfp_state_change->previous,hfp_state_change->current);
                if ((hfp_context->pre_state > BT_SINK_SRV_STATE_STREAMING) && (hfp_context->curr_state <= BT_SINK_SRV_STATE_STREAMING)) {
                    if (true == hfp_context->transient_actived) {
                        hfp_context->transient_actived = false;
                        ui_shell_finish_activity(self, self);
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);
                    }
                }
            }
        }
#endif
    }
    return ret;
}
#endif

/**
* @brief      This function is used to handle the app internal events.
* @param[in]  self, the context pointer of the activity.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
static bool app_hfp_proc_app_internal_events(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
    if (hfp_context == NULL) {
        return ret;
    }
    apps_config_state_t app_state = APP_TOTAL_STATE_NO;
    app_state = app_get_config_status_by_state(hfp_context->curr_state);

    switch (event_id) {
        /* Update LED display when the state of HFP updated. */
        case APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI",APP_INTERACTION, sink_state=0x%x, app_state=%d", 2, hfp_context->curr_state, app_state);
            ret = app_hfp_update_led_bg_pattern(self, hfp_context->curr_state, hfp_context->pre_state);
            break;
        }
        /* Update mmi state when the state of HFP updated. */
        case APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE: {
            if (APP_TOTAL_STATE_NO != app_state) {
#if defined(AIR_MULTI_POINT_ENABLE) && defined(MTK_AWS_MCE_ENABLE)
                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
                APPS_LOG_MSGID_I(APP_HFP_ACTI",APP_INTERACTION set_mmi_state:role=0x%x",
                        1, role);
                if (role == BT_AWS_MCE_ROLE_PARTNER && app_hfp_get_aws_link_is_switching()) {
                    ret = true;
                } else {
                    apps_config_key_set_mmi_state(app_state);
                    if (role == BT_AWS_MCE_ROLE_AGENT) {
                        /* Sync sink state to peer.*/
                        app_hfp_sync_sink_state_to_peer(self);
                    }
                    ret = true;
                }
#else
                apps_config_key_set_mmi_state(app_state);
                ret = true;
#endif
            } else {
                ret = false;
            }
            APPS_LOG_MSGID_I(APP_HFP_ACTI",APP_INTERACTION set_mmi_state: ret=%d, app_state=%d",
                    2, ret, app_state);
            break;
        }
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        case APPS_EVENTS_INTERACTION_RHO_END:
        case APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT: {
            app_rho_result_t rho_ret = (app_rho_result_t)extra_data;
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            APPS_LOG_MSGID_I(APP_HFP_ACTI" [RING_IND] Curr_Role: 0x%x, RHO result : 0x%x, cur_state : 0x%x, pre_state : 0x%x",
                    4, role, rho_ret, hfp_context->curr_state, hfp_context->pre_state);
            app_hfp_incoming_call_vp_play_status_t vp_play_status = app_hfp_get_incoming_call_vp_flag();
            if (vp_play_status == APP_HFP_INCOMING_CALL_VP_STOP) {
                break;
            }
            if (APP_RHO_RESULT_SUCCESS == rho_ret) {
                /* Play ringtone after RHO. */
                bool is_twc_incoming_call_vp = false;
                if (hfp_context->curr_state == BT_SINK_SRV_STATE_TWC_INCOMING) {
                    is_twc_incoming_call_vp = true;
                }
                uint32_t vp_index = (is_twc_incoming_call_vp == TRUE) ? VP_INDEX_TWC_INCOMING_CALL : VP_INDEX_INCOMING_CALL;

                uint32_t vp_delay_time = 0;
                if (vp_play_status == APP_HFP_INCOMING_CALL_VP_LONG) {
                    vp_delay_time = APP_HFP_INCMOING_CALL_VP_LONG_DELAY_TIME;
                } else if (vp_play_status == APP_HFP_INCOMING_CALL_VP_SHORT) {
                    vp_delay_time = APP_HFP_INCMOING_CALL_VP_SHORT_DELAY_TIME;
                }
                APPS_LOG_MSGID_I(APP_HFP_ACTI" [RING_IND] RHO: vp_play_status=%d, is_twc_incoming_call_vp=%d",
                        2, vp_play_status, is_twc_incoming_call_vp);
                if (BT_SINK_SRV_STATE_INCOMING == hfp_context->curr_state
                        || hfp_context->curr_state == BT_SINK_SRV_STATE_TWC_INCOMING) {
                    if (BT_AWS_MCE_ROLE_AGENT == role) {
                        apps_config_set_voice(vp_index, true, vp_delay_time, VOICE_PROMPT_PRIO_ULTRA, true, true, NULL);
                    } else if (BT_AWS_MCE_ROLE_PARTNER == role) {
                        apps_config_set_voice(vp_index, false, vp_delay_time, VOICE_PROMPT_PRIO_ULTRA, true, false, NULL);
                    }
                }
            }
            break;
        }
#endif
        /* Incoming call vp sync fail because switch aws link fail, play local incoming call vp*/
        case APPS_EVENTS_INTERACTION_INCOMING_CALL_DELAY: {
            if (hfp_context->curr_state == BT_SINK_SRV_STATE_INCOMING
                    && hfp_context->pre_state != BT_SINK_SRV_STATE_INCOMING) {
                app_hfp_set_incoming_call_vp_flag(APP_HFP_INCOMING_CALL_VP_LONG);
                apps_config_set_voice(VP_INDEX_INCOMING_CALL, false, APP_HFP_INCMOING_CALL_VP_LONG_DELAY_TIME, VOICE_PROMPT_PRIO_ULTRA, true, true, NULL);
                app_hfp_set_incoming_call_vp_sync_flag(APP_HFP_INCOMING_CALL_VP_SYNC_PLAY_FAIL);
                ret = true;
            }
            break;
        }
#ifdef AIR_MULTI_POINT_ENABLE
        case APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP: {
            apps_config_set_vp(VP_INDEX_TWC_INCOMING_CALL, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            /* send a delay event as a timer to loop twc incoming call vp playback. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP, NULL, 0,
                        NULL, TWC_INCOMING_CALL_VP_TIMER_INTERVAL);
            break;
        }
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        case APPS_EVENTS_INTERACTION_IN_EAR_UPDATE_STA: {
            app_in_ear_sta_info_t *sta_info = (app_in_ear_sta_info_t *)extra_data;
            APPS_LOG_MSGID_I(APP_HFP_ACTI", [IN_EAR_UPDATE] %d->%d", 2, sta_info->previous, sta_info->current);
            uint8_t hfp_in_ear_enable = app_hfp_is_auto_accept_incoming_call();
            APPS_LOG_MSGID_I(APP_HFP_ACTI"[AUTO ACCEPT] auto accept incoming call:isEnable=%d, curr_state=%d",
                                 2, hfp_in_ear_enable, app_state);
            if ((APP_HFP_AUTO_ACCEPT_ENABLE == hfp_in_ear_enable) && (sta_info->current == APP_IN_EAR_STA_BOTH_IN)
                    && (APP_HFP_INCOMING == app_state)) {
                bt_sink_srv_send_action(BT_SINK_SRV_ACTION_ANSWER, NULL);
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_AUTO_ACCEPT_INCOMING_CALL: {
            APPS_LOG_MSGID_I(APP_HFP_ACTI" [AUTO ACCEPT]: received auto accept incoming call event.", 0);
            bt_sink_srv_send_action(BT_SINK_SRV_ACTION_ANSWER, NULL);
            break;
        }
#endif
        default : {
            break;
        }
    }

    return ret;
}

bool app_hfp_activity_proc(
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
            ret = app_hfp_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef AIR_ROTARY_ENCODER_ENABLE
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER: {
            /* Rotary encoder events. */
            ret = app_hfp_proc_rotary_event_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* BT_SINK events, indicates the state of HFP. */
            ret = app_hfp_proc_bt_sink_event(self, event_group, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            /* Event come from bt connection manager, indicates the connection state of HFP. */
            ret = app_hfp_proc_bt_cm_event(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            /* Events come from app interation. */
            ret = app_hfp_proc_app_internal_events(self, event_id, extra_data, data_len);
            break;
        }
#if defined(MTK_AWS_MCE_ENABLE)
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            /* Handle the key event sync from the partner side. */
            ret = app_hfp_proc_aws_data(self, event_id, extra_data, data_len);
            break;
        }
#endif
        default:
            break;
    }

    return ret;
}

