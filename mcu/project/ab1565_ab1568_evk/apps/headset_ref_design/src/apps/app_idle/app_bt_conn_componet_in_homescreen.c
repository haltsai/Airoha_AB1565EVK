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
 * File: app_bt_conn_componet_in_homescreen.c
 *
 * Description: This file provides utility function to handle BT event for Homescreen APP.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for Homescreen APP.
 *
 */

#include "app_bt_conn_componet_in_homescreen.h"
#include "apps_config_key_remapper.h"
#include "apps_config_features_dynamic_setting.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_events_bt_event.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "bt_app_common.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "apps_debug.h"

#include "bt_sink_srv.h"
#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le.h"
#endif
#include "ui_shell_manager.h"
#include "bt_gap_le.h"
#include "apps_aws_sync_event.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#endif

#define UI_SHELL_IDLE_BT_CONN_ACTIVITY "[TK_BT_CONN]app_bt_conn_componet_in_homescreen"


/**
* @brief      APP update MMI.
*/
static void bt_conn_component_update_mmi()
{
    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" bt_conn_component_update_mmi", 0);
    /* Send event to update LED background pattern and MMI state. */
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                        NULL, 0);
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                        NULL, 0);
}

static bool bt_conn_component_add_device(bool add, home_screen_local_context_type_t *local_ctx, bt_cm_remote_info_update_ind_t *remote)
{
    bool play_vp = false;
    uint8_t i = 0;
    uint8_t blank_index = 0xFF;
    uint8_t blank_count = 0;
    uint8_t match_index = 0xFF;
    bt_bd_addr_t zero_bt_addr;
    memset(zero_bt_addr, 0 ,sizeof(bt_bd_addr_t));

    for (i = 0; i < APP_CONN_MAX_DEVICE_NUM; i++) {
        if (memcmp(&(local_ctx->conn_device[i].addr), &(remote->address), sizeof(bt_bd_addr_t)) == 0) {
            if (add) {
                return play_vp;
            }
            match_index = i;
        } else if ((memcmp(&(local_ctx->conn_device[i].addr), zero_bt_addr, sizeof(bt_bd_addr_t)) == 0)) {
            blank_count ++;
            if (blank_index == 0xFF) {
                blank_index = i;
            }
        }
    }

    if (add) {
        if (blank_index != 0xFF) {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Add device to index %d", 1, blank_index);
            memcpy(&(local_ctx->conn_device[blank_index].addr), &(remote->address), sizeof(bt_bd_addr_t));
            local_ctx->conn_device[blank_index].conn_service = remote->connected_service;
            local_ctx->conn_device_num ++;
            if (blank_count != APP_CONN_MAX_DEVICE_NUM) {
                play_vp = true;
            }
        } else {
            APPS_LOG_MSGID_E(UI_SHELL_IDLE_BT_CONN_ACTIVITY", conn device full, something wrong", 0);
        }
    } else {
        if (match_index != 0xFF) {
            if (blank_count < APP_CONN_MAX_DEVICE_NUM) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", remove device index %d", 1, match_index);
                //memset(&(local_ctx->conn_device[match_index]), 0, sizeof(app_conn_device_info_t));
                memcpy(&(local_ctx->conn_device[match_index].addr), &zero_bt_addr, sizeof(bt_bd_addr_t));
                local_ctx->conn_device[match_index].conn_service = BT_CM_PROFILE_SERVICE_MASK_NONE;
                local_ctx->conn_device_num --;
                play_vp = true;
            } else {
                APPS_LOG_MSGID_W(UI_SHELL_IDLE_BT_CONN_ACTIVITY", the last deivce, blank_count%d", 1, blank_count);
            }
        } else {
            APPS_LOG_MSGID_W(UI_SHELL_IDLE_BT_CONN_ACTIVITY", no match index found", 0);
        }
    }

    return play_vp;
}

/**
* @brief      Receive the connection related events of LE audio and play VP.
*/
bool bt_conn_component_bt_sink_event_proc(ui_shell_activity_t *self,
        uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
#if defined (AIR_LE_AUDIO_ENABLE) && defined (AIR_LE_AUDIO_CIS_ENABLE)
    home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *) self->local_context;
    switch (event_id) {
        case LE_SINK_SRV_EVENT_REMOTE_INFO_UPDATE: {
            bt_le_sink_srv_event_remote_info_update_t *update_ind = (bt_le_sink_srv_event_remote_info_update_t *) extra_data;
            if (update_ind == NULL || local_ctx == NULL) {
                break;
            }
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"[LEA] Link=%d->%d bt_power_off=%d state=%d",
                            4, update_ind->pre_state, update_ind->state,
                            local_ctx->bt_power_off, local_ctx->state);
            if (update_ind->pre_state == BT_BLE_LINK_DISCONNECTED
                    && update_ind->state == BT_BLE_LINK_CONNECTED) {
                local_ctx->bt_power_off = false;
                apps_config_set_vp(VP_INDEX_CONNECTED, FALSE, 0, VOICE_PROMPT_PRIO_MEDIUM, FALSE, NULL);
                bt_conn_component_update_mmi();
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"[LEA] Connected VP", 0);
            } else if (update_ind->pre_state == BT_BLE_LINK_CONNECTED
                    && update_ind->state == BT_BLE_LINK_DISCONNECTED) {
                if (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE && (!local_ctx->bt_power_off)) {
                    apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, FALSE, 0, VOICE_PROMPT_PRIO_MEDIUM, FALSE, NULL);
                    bt_conn_component_update_mmi();
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"[LEA] Disconnected VP", 0);
                }
            }
        }
            break;
        default:
            break;
    }
#endif
    return ret;
}

bool bt_conn_component_bt_cm_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role;
    role = bt_device_manager_aws_local_info_get_role();
#endif
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == local_ctx || NULL == remote_update) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY",there has error", 0);
                break;
            }

            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", REMOTE_INFO_UPDATE, remote_addr %x:%x:%x:%x:%x:%x",
                    6, remote_update->address[5], remote_update->address[4], remote_update->address[3],
                    remote_update->address[2], remote_update->address[1], remote_update->address[0]);
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", REMOTE_INFO_UPDATE, acl_state(%x)->(%x), connected_service(%x)->(%x)",
                             4, remote_update->pre_acl_state, remote_update->acl_state,
                             remote_update->pre_connected_service, remote_update->connected_service);
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", REMOTE_INFO_UPDATE, (%x), aws_connected(%x), bt_power_state(%x)",
                             3, local_ctx->connection_state, local_ctx->aws_connected, local_ctx->bt_power_state);

#ifdef MTK_AWS_MCE_ENABLE
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
            {
                if ((remote_update->connected_service & ~remote_update->pre_connected_service)
                    & (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL))) {
                //if (!(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                //    && (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    // When agent connect the first profile, set state to CONNECTED
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent Connected", 0);
                    if (!(local_ctx->connection_state)) {
                        local_ctx->connection_state = true;
                        local_ctx->bt_power_off = false;
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent connection_state set true", 0);
                        apps_config_set_vp(VP_INDEX_CONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        bt_conn_component_update_mmi();
                        local_ctx->conn_device_num = 1;
                        memcpy(&(local_ctx->conn_device[0].addr), &(remote_update->address), sizeof(bt_bd_addr_t));
                        local_ctx->conn_device[0].conn_service = remote_update->connected_service;
                    } else if (bt_conn_component_add_device(true, local_ctx, remote_update)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent connect another device", 0);
                        if (local_ctx->conn_device_num == APP_CONN_MAX_DEVICE_NUM) {
                            //this will take over another
                            apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        }
                        apps_config_set_vp(VP_INDEX_CONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    }
                } else if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                           && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state) {
                    /* When disconnect happen */
                    if (local_ctx->connection_state && bt_conn_component_add_device(false, local_ctx, remote_update)) {
                        // one of the device disconnect
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent disconnect to one", 0);
                        if (local_ctx->conn_device_num < (APP_CONN_MAX_DEVICE_NUM - 1)
                                && (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE)  && (!local_ctx->bt_power_off)) {
                                //if conn_device_num == APP_CONN_MAX_DEVICE_NUM - 1, that's a take over, vp already played
                            if (local_ctx->conn_device_num == 0) {
                                local_ctx->connection_state = false;
                                memset(&(local_ctx->conn_device), 0, sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM);
                                bt_conn_component_update_mmi();
                                apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                            } else {
                                apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                            }
                        }
                    }
                }
#ifdef MTK_AWS_MCE_ENABLE
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* AWS connected */
                    local_ctx->aws_connected = true;
                    if (local_ctx->is_bt_visiable) {
                        bt_status_t send_aws_status = apps_aws_sync_event_send_extra(
                                                          EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                          APPS_EVENTS_INTERACTION_BT_VISIBLE_STATE_CHANGE,
                                                          &local_ctx->is_bt_visiable,
                                                          sizeof(local_ctx->is_bt_visiable));
                        if (BT_STATUS_SUCCESS != send_aws_status) {
                            APPS_LOG_MSGID_I("Fail to send bt visible change to partner when aws connected", 0);
                        }
                    }
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner Attached.", 0);
                } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Agent update AWS connection state when AWS disconnected. */
                    local_ctx->aws_connected = false;
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner Detached.", 0);
                }
#endif
            }
#ifdef MTK_AWS_MCE_ENABLE
            else if (role == BT_AWS_MCE_ROLE_PARTNER || role == BT_AWS_MCE_ROLE_CLINET) {
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Partner AWS connected */
                    local_ctx->aws_connected = true;
                    if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                        /* Partner Connect SP = AWS connected + LINK_NORMAL */
                        local_ctx->connection_state = true;
                        bt_conn_componet_play_vp(VP_INDEX_CONNECTED);
                    }
                } else if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)
                    & (remote_update->pre_connected_service & (~remote_update->connected_service))) {
                    /* Partner connected -> disconnected */
                    if (local_ctx->connection_state) {
                        local_ctx->connection_state = false;
                        if (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE) {
                            bt_conn_componet_play_vp(VP_INDEX_DEVICE_DISCONNECTED);
                        }
                    }
                    local_ctx->aws_connected = false;
                    local_ctx->is_bt_visiable = false;
                }
            }
#endif
        }
        break;
        case BT_CM_EVENT_VISIBILITY_STATE_UPDATE: {
            bt_cm_visibility_state_update_ind_t *visible_update = (bt_cm_visibility_state_update_ind_t *)extra_data;
            if (NULL == local_ctx || NULL == visible_update) {
                break;
            }
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" visibility_state: %d", 1, visible_update->visibility_state);

            /* Update BT visibility state for Homescreen APP. */
            local_ctx->is_bt_visiable = visible_update->visibility_state;
#ifdef MTK_AWS_MCE_ENABLE
            bt_status_t send_aws_status;
            send_aws_status = apps_aws_sync_event_send_extra(
                                  EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                  APPS_EVENTS_INTERACTION_BT_VISIBLE_STATE_CHANGE,
                                  &local_ctx->is_bt_visiable,
                                  sizeof(local_ctx->is_bt_visiable));
            if (BT_STATUS_SUCCESS != send_aws_status) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Fail to send bt visible change to partner : %d", 1, visible_update->visibility_state);
            }
#endif
            /* Send event to update LED background pattern and MMI state when BT visibility changed. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                NULL, 0);
        }
        break;
        case BT_CM_EVENT_POWER_STATE_UPDATE: {
            bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)extra_data;
            if (NULL == local_ctx || NULL == power_update) {
                break;
            }

            if (BT_CM_POWER_STATE_ON == power_update->power_state) {
                /* Switch BT state from off to on. */
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Power ON", 0);
                local_ctx->bt_power_state = APP_HOME_SCREEN_BT_POWER_STATE_ENABLED;
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                    NULL, 0);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                    NULL, 0);
            } else if (BT_CM_POWER_STATE_OFF == power_update->power_state) {
                /* Switch BT state from on to off. */
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Power OFF", 0);
                local_ctx->is_bt_visiable = false;
                local_ctx->bt_power_state = APP_HOME_SCREEN_BT_POWER_STATE_DISABLED;
                local_ctx->connection_state = false;
                local_ctx->aws_connected = false;
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                    NULL, 0);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                    NULL, 0);
            }
        }
        break;
        default:
            break;
    }
    return ret;
}

bool bt_conn_component_aws_data_proc(ui_shell_activity_t *self, uint32_t unused_id, void *extra_data, size_t data_len)
{
    bool ret = false;
#ifdef MTK_AWS_MCE_ENABLE
    home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t event_id;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event_id,
                                         &p_extra_data, &extra_data_len);
        switch (event_group) {
            case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
                switch (event_id) {
                    case APPS_EVENTS_INTERACTION_BT_VISIBLE_STATE_CHANGE: {
                        bool bt_visible = false;
                        /* Partner update BT visibility state and MMI when received BT_VISIBLE_STATE_CHANGE event. */
                        if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()
                            && p_extra_data && extra_data_len == sizeof(bt_visible)) {
                            bt_visible = *(bool *)p_extra_data;
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"Received bt_visible from agent : %d", 1,
                                             bt_visible);
                            if (local_ctx->is_bt_visiable != bt_visible) {
                                local_ctx->is_bt_visiable = bt_visible;
                                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                    APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                                    NULL, 0);
                                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                    APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                                    NULL, 0);
                            }
                        }
                        ret = true;
                    }
                    break;
                    default:
                        break;
                }
                break;

            default:
                break;
        }
    }
#endif
    return ret;
}
