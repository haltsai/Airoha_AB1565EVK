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
#include "bt_connection_manager_internal.h"
#include "bt_device_manager.h"
#include "apps_debug.h"
#include "hal_gpt.h"
#include "FreeRTOS.h"

#include "bt_sink_srv.h"
#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le.h"
#endif

#include "ui_shell_manager.h"
#include "bt_gap_le.h"
#include "apps_aws_sync_event.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
extern bool g_apps_bt_event_normal_link_partner;
#endif

#define UI_SHELL_IDLE_BT_CONN_ACTIVITY "[TK_BT_CONN]app_bt_conn"
#define APP_BT_CONN_DISCONNECTED_VP_DELAY  600 * 1000

#ifdef __CONN_VP_SYNC_STYLE_TWO__
bool g_bt_conn_wait_play_vp = false;                      /**<  Flag to indicate waiting AWS connect to play sync VP. */
extern bool g_apps_bt_event_aws_connect_encryp;   /* Flag set in apps_events_bt_event.c to indicate whether AWS is connected when encryption_on. */
#endif

/**
* @brief      DISCONNECTED VP callback when it plays by GPT, should unlock GPT in BT module when sync play ends.
*/
static void app_bt_conn_disconn_vp_cb(uint32_t idx, vp_err_code err)
{
    if ((idx == VP_INDEX_DEVICE_DISCONNECTED) && ((err == VP_ERR_CODE_SUCCESS) || (err == VP_ERR_CODE_PREEMPTED))) {
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" bt_cm_unlock_bt_sleep_by_VP", 0);
        bt_cm_unlock_bt_sleep_by_VP();
    }
}

/**
* @brief      APP update MMI.
*/
static void bt_conn_component_update_mmi()
{
    //APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" bt_conn_component_update_mmi", 0);
    /* Send event to update LED background pattern and MMI state. */
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                        NULL, 0);
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                        NULL, 0);
}

/**
* @brief      Manager connected remote devices, return a bool that indicates whether need play VP.
*/
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
                /* The device to be added is already in the table. */
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
            /* Found a blank to add the device. */
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Add device to index %d", 1, blank_index);
            memcpy(&(local_ctx->conn_device[blank_index].addr), &(remote->address), sizeof(bt_bd_addr_t));
            local_ctx->conn_device[blank_index].conn_service = remote->connected_service;
            local_ctx->conn_device_num ++;
            play_vp = true;
        } else {
            APPS_LOG_MSGID_E(UI_SHELL_IDLE_BT_CONN_ACTIVITY", conn device full, something wrong", 0);
        }
    } else {
        if (match_index != 0xFF) {
            if (blank_count < APP_CONN_MAX_DEVICE_NUM) {
                /* Remove the device. */
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
            /* The device to be remove is not in the table. */
            APPS_LOG_MSGID_E(UI_SHELL_IDLE_BT_CONN_ACTIVITY", no match index found, something wrong", 0);
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

bool bt_conn_component_app_interaction_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

#ifdef AIR_MULTI_POINT_ENABLE
    if (event_id == APPS_EVENTS_INTERACTION_MULTIPOINT_SWITCH_AWS) {
        /* Only received by partner when AWS is failed to switch to another remote device in 3s.*/
        home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Partner aws switch fail", 0);
        local_ctx->partner_aws_switch = false;
        local_ctx->connection_state = false;
        memset(&(local_ctx->conn_device), 0, sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM);
        if (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE && (!local_ctx->bt_power_off)) {
            apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        }
        bt_conn_component_update_mmi();
        ret = true;
    }
#endif

#if (defined AIR_MULTI_POINT_ENABLE) || (defined __CONN_VP_SYNC_STYLE_TWO__)
    if (event_id == APPS_EVENTS_INTERACTION_PLAY_SYNC_VP) {
        home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;
#ifdef __CONN_VP_SYNC_STYLE_TWO__
        if (extra_data && g_bt_conn_wait_play_vp && local_ctx->connection_state) {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" timer expired to wait normal aws, play local connected vp", 0);
            /* Partner fail to reconnect to agent after SP connect 3s, agent play local VP. */
            apps_config_set_vp(VP_INDEX_CONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            g_bt_conn_wait_play_vp = false;
        }
#endif
#ifdef AIR_MULTI_POINT_ENABLE
        if (extra_data == NULL && local_ctx->wait_disconnect) {
            /* Partner fail to reconnect to agent after SP disconnect 3s, agent play local VP. */
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" timer expired to wait special aws, play local disconnect vp", 0);
            if (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE && (!local_ctx->bt_power_off)) {
                apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            local_ctx->wait_disconnect = 0;
        }
#endif
        ret = true;
    }
#endif

    return ret;
}

void app_bt_conn_sync_addr_list(home_screen_local_context_type_t *local_ctx)
{
    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", sync addr list to new agent", 0);

    void *data = pvPortMalloc(sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM + sizeof(uint8_t));
    if(data) {
        memcpy(data, &(local_ctx->conn_device), sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM);
        memcpy(data + sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM, &(local_ctx->conn_device_num), sizeof(uint8_t));
        apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_SYNC_ADDR_LIST,
                data, sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM + sizeof(uint8_t));
        vPortFree(data);
    }
}

uint8_t g_app_bt_conn_pro_btcm = 0;
bool bt_conn_component_bt_cm_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    g_app_bt_conn_pro_btcm = 1;
    bool ret = false;
    home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == local_ctx || NULL == remote_update) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY",there has error", 0);
                break;
            }
#ifdef MTK_AWS_MCE_ENABLE
            bt_event_suffix_data_t *suffix_data = get_bt_event_suffix_data(extra_data, sizeof(bt_cm_remote_info_update_ind_t));
            role = suffix_data->aws_role;
#endif
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", REMOTE_INFO_UPDATE, role 0x%x, addr %x:%x:%x:%x:%x:%x, acl(%x)->(%x), service(%x)->(%x)",
                    11, role, remote_update->address[5], remote_update->address[4], remote_update->address[3],
                    remote_update->address[2], remote_update->address[1], remote_update->address[0],
                    remote_update->pre_acl_state, remote_update->acl_state,
                    remote_update->pre_connected_service, remote_update->connected_service);
            //APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", role %x, conn_state %d", 2, role, local_ctx->connection_state);

#ifdef MTK_AWS_MCE_ENABLE
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role || BT_AWS_MCE_ROLE_FOLLOWER_1 == role)
#endif
            {
#ifdef __CONN_VP_SYNC_STYLE_TWO__
                if ((remote_update->connected_service & ~remote_update->pre_connected_service)
                    & (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL))) {
                    /* When A2DP or HFP connect, treat as connected to SP. */
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent Connected", 0);
                    if (!(local_ctx->connection_state)) {
                        local_ctx->connection_state = true;
                        local_ctx->bt_power_off = false;
                        bt_conn_component_update_mmi();

                        //if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service) {
                        if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                            /* Normal AWS (agent + partner + SP) already connected. */
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" AWS connected, sync play VP", 0);
                            apps_config_set_vp(VP_INDEX_CONNECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        } else {
                            if (g_apps_bt_event_aws_connect_encryp) {
                                /* Partner attached to agent before agent connect to SP, but now not attach again, set timer to wait partner re-attach. */
                                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" set timer wait aws", 0);
                                g_bt_conn_wait_play_vp = true;
                                ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_PLAY_SYNC_VP, (void *)1, 0, NULL, 3000);
                            } else {
                                /* Partner not attached to agent before agent connect to SP, so agent play local VP immediately. */
                                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" no partner, local play connected vp", 0);
                                apps_config_set_vp(VP_INDEX_CONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                            }
                        }

                        local_ctx->conn_device_num = 1;
                        memcpy(&(local_ctx->conn_device[0].addr), &(remote_update->address), sizeof(bt_bd_addr_t));
                        local_ctx->conn_device[0].conn_service = remote_update->connected_service;
#ifdef AIR_MULTI_POINT_ENABLE
                    } else if (bt_conn_component_add_device(true, local_ctx, remote_update)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent connect another device", 0);
                        /* VP is already set in apps_events_bt_event.c */
#endif
                    }
                }
#else
                if (!(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Update BT EDR connection state as TRUE when Agent connected first non-AWS profile. */
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent Connected", 0);
                    if (!(local_ctx->connection_state)) {
                        local_ctx->connection_state = true;
                        local_ctx->bt_power_off = false;
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent connection_state set true", 0);
                        bt_conn_component_update_mmi();

                        local_ctx->conn_device_num = 1;
                        memcpy(&(local_ctx->conn_device[0].addr), &(remote_update->address), sizeof(bt_bd_addr_t));
                        local_ctx->conn_device[0].conn_service = remote_update->connected_service;
#ifdef AIR_MULTI_POINT_ENABLE
                    } else if (bt_conn_component_add_device(true, local_ctx, remote_update)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent connect another device", 0);
                        /* VP is already set in apps_events_bt_event.c */
#endif
                    }
                }
#endif
                else if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                         && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state) {
                    /* BT ACL disconnection happen. */
                    if (local_ctx->connection_state) {
#ifndef AIR_MULTI_POINT_ENABLE
                        if (bt_conn_component_add_device(false, local_ctx, remote_update)) {
                            /* Update BT EDR connection state as FALSE if Agent all non-AWS profile disconnected. */
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent Disconnect", 0);
                            /* Not play disconnected VP when BT/system power off. */
                            if (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE && (!local_ctx->bt_power_off)) {
                                /* Use controller provided GPT count to ensure VP sync. */
                                uint32_t gpt = bt_cm_get_disconnected_gpt_count(remote_update->address);
                                uint32_t cur_gpt = 0;
                                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &cur_gpt);
                                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" get disconnect gpt 0x%08x, cur 0x%08x", 2, gpt, cur_gpt);
                                if (gpt != 0) {
                                    apps_config_set_vp_by_gpt(VP_INDEX_DEVICE_DISCONNECTED, gpt + APP_BT_CONN_DISCONNECTED_VP_DELAY,
                                            VOICE_PROMPT_PRIO_MEDIUM, false, app_bt_conn_disconn_vp_cb);
                                    bt_cm_clear_disconnected_gpt_count(remote_update->address);
                                } else {
                                    /* There's no GPT count when link loss or no partner attached. */
                                    apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                                }
                            }
                            if (local_ctx->conn_device_num == 0) {
                                local_ctx->connection_state = false;
                                bt_conn_component_update_mmi();
                            }
                        }
#else
                        if (bt_conn_component_add_device(false, local_ctx, remote_update)) {
                            /* One of the device disconnect. */
                            if (local_ctx->conn_device_num == 0) {
                                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Agent disconnect all", 0);
                                local_ctx->connection_state = false;
                                bt_conn_component_update_mmi();
                            }

                            bt_bd_addr_t aws_device;
                            uint32_t num = 0;
                            num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &aws_device, 1);
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" get aws addr num %d", 1, num);

                            if (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE && (!local_ctx->bt_power_off)) {
                                uint32_t gpt = bt_cm_get_disconnected_gpt_count(remote_update->address);
                                uint32_t cur_gpt = 0;
                                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &cur_gpt);
                                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" get disconnect gpt 0x%08x, cur 0x%08x", 2, gpt, cur_gpt);
                                if (gpt != 0) {
                                    apps_config_set_vp_by_gpt(VP_INDEX_DEVICE_DISCONNECTED, gpt + APP_BT_CONN_DISCONNECTED_VP_DELAY,
                                            VOICE_PROMPT_PRIO_MEDIUM, false, app_bt_conn_disconn_vp_cb);
                                    bt_cm_clear_disconnected_gpt_count(remote_update->address);
                                } else if (remote_update->reason == BT_HCI_STATUS_CONNECTION_TIMEOUT && !(local_ctx->agent_synced_disconn)) {
                                    /* Link loss on active AWS link, play local VP. Partner will receive link loss event and play local. */
                                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" link loss on active aws or none aws link", 0);
                                    apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                                } else if (remote_update->reason != BT_HCI_STATUS_CONNECTION_TIMEOUT && local_ctx->conn_device_num == 0) {
                                    /* 2 remote devices disconnect at the same time, and AWS is not attached after the 1st disconnect. */
                                    if (bt_aws_mce_srv_get_link_type() == BT_AWS_MCE_SRV_LINK_SPECIAL) {
                                        apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                                    } else if (!(local_ctx->partner_lost) && g_apps_bt_event_aws_connect_encryp) {
                                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Set timer to wait special link", 0);
                                        local_ctx->wait_disconnect = 1;
                                        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                APPS_EVENTS_INTERACTION_PLAY_SYNC_VP, (void *)0, 0, NULL, 3000);
                                    } else {
                                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" agent disconnect when no partner", 0);
                                        apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                                    }
                                } else if (local_ctx->partner_lost || !g_apps_bt_event_aws_connect_encryp) {
                                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" agent disconnect when no partner", 0);
                                    apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                                } else if (!local_ctx->agent_synced_disconn) {
                                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" agent disconnect, but VP not played", 0);
                                    apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                                }
                                local_ctx->agent_synced_disconn = false;
                            }
                        }
#endif
                    }
                }

                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Agent update AWS connection state and notify BT_VISIBLE_STATE_CHANGE when AWS connected. */
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
                    g_apps_bt_event_aws_connect_encryp = true;
#ifdef AIR_MULTI_POINT_ENABLE
                    local_ctx->partner_lost = false;
                    if (local_ctx->wait_disconnect &&
                            (memcmp(bt_device_manager_get_local_address(), &(remote_update->address), sizeof(bt_bd_addr_t)) == 0)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" special link, sync disconnect vp", 0);
                        if (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE && (!local_ctx->bt_power_off)) {
                            apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        }
                        local_ctx->wait_disconnect = 0;
                        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_PLAY_SYNC_VP);
                    }
#endif
#ifdef __CONN_VP_SYNC_STYLE_TWO__
                    if (g_bt_conn_wait_play_vp && local_ctx->connection_state && (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type())) {
                        /* Partner re-attached, play sync VP and stop timer. */
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", normal AWS connect, play sync vp.", 0);
                        apps_config_set_vp(VP_INDEX_CONNECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        g_bt_conn_wait_play_vp = false;
                        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_PLAY_SYNC_VP);
                    }
#endif
                } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Agent update AWS connection state when AWS disconnected. */
                    local_ctx->aws_connected = false;
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner Detached.", 0);
#ifdef AIR_MULTI_POINT_ENABLE
                    bt_cm_profile_service_mask_t conn_profile = bt_cm_get_connected_profile_services(remote_update->address);
                    if (conn_profile != BT_CM_PROFILE_SERVICE_MASK_NONE &&
                            (memcmp(bt_device_manager_get_local_address(), &(remote_update->address), sizeof(bt_bd_addr_t)) != 0)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Normal AWS disconnect but remote device already connect", 0);
                        local_ctx->partner_lost = true;
                    }
#endif
                }
            }
#ifdef MTK_AWS_MCE_ENABLE
            else if (role == BT_AWS_MCE_ROLE_PARTNER || role == BT_AWS_MCE_ROLE_CLINET || role == BT_AWS_MCE_ROLE_FOLLOWER_2) {
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Partner update AWS connection state when AWS connected. */
                    local_ctx->aws_connected = true;
                    /* Partner Connect SP = AWS connected + LINK_NORMAL */
                    if (g_apps_bt_event_normal_link_partner) {
                        g_apps_bt_event_normal_link_partner = false;
#ifdef AIR_MULTI_POINT_ENABLE
                        if (local_ctx->partner_aws_switch) {
                            local_ctx->partner_aws_switch = false;
                            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_MULTIPOINT_SWITCH_AWS);
                        } else
#endif
                        {
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner set connected", 0);
                            local_ctx->connection_state = true;
                            local_ctx->bt_power_off = false;
                            bt_conn_component_update_mmi();
                        }
                    }
                } else if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)
                    & (remote_update->pre_connected_service & (~remote_update->connected_service))) {
                    /* Partner update connection state when AWS profile disconnect. */
                    if (local_ctx->connection_state &&
                            (memcmp(bt_device_manager_get_local_address(), &(remote_update->address), sizeof(bt_bd_addr_t)) != 0)) {
#ifdef AIR_MULTI_POINT_ENABLE
                        if ((BT_CM_AWS_LINK_DISCONNECT_REASON_MASK & remote_update->reason) == BT_CM_AWS_LINK_DISCONNECT_REASON_MASK
                                && ((~BT_CM_AWS_LINK_DISCONNECT_REASON_MASK) & remote_update->reason) == BT_CM_AWS_LINK_DISCONNECT_BY_SWITCH_LINK) {
                            //set a 3s timer for multipoint aws switch
                            local_ctx->partner_aws_switch = true;
                            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_MULTIPOINT_SWITCH_AWS, NULL, 0, NULL, 3000);
                        } else
#endif
                        {
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Partner Disconnected, reason 0x%x", 1, remote_update->reason);
                            local_ctx->connection_state = false;
                            if (local_ctx->state == APP_HOME_SCREEN_STATE_IDLE && (!local_ctx->bt_power_off)) {
                                uint32_t gpt = bt_cm_get_disconnected_gpt_count(remote_update->address);
                                uint32_t cur_gpt = 0;
                                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &cur_gpt);
                                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" get disconnect gpt 0x%08x, cur 0x%08x", 2, gpt, cur_gpt);
                                if (gpt != 0 && remote_update->reason != BT_HCI_STATUS_CONNECTION_TIMEOUT
                                        && remote_update->reason != BT_HCI_STATUS_CONNECTION_TERMINATED_BY_LOCAL_HOST) {
                                    apps_config_set_vp_by_gpt(VP_INDEX_DEVICE_DISCONNECTED, gpt + APP_BT_CONN_DISCONNECTED_VP_DELAY,
                                            VOICE_PROMPT_PRIO_MEDIUM, false, app_bt_conn_disconn_vp_cb);
                                    //bt_cm_clear_disconnected_gpt_count(remote_update->address);
                                } else {
                                    apps_config_set_vp(VP_INDEX_DEVICE_DISCONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                                }
                            }
                            bt_conn_component_update_mmi();
#ifdef AIR_MULTI_POINT_ENABLE
                            memset(&(local_ctx->conn_device), 0, sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM);
#endif
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
                local_ctx->connection_state = false;
                local_ctx->aws_connected = false;
                local_ctx->conn_device_num = 0;
                memset(local_ctx->conn_device, 0, sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM);
#ifdef AIR_MULTI_POINT_ENABLE
                local_ctx->partner_aws_switch = false;
                local_ctx->agent_synced_disconn = false;
                local_ctx->wait_disconnect = false;
#endif
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
    g_app_bt_conn_pro_btcm = 0;
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
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
                    case APPS_EVENTS_INTERACTION_SYNC_ADDR_LIST: {
                        if (p_extra_data && (extra_data_len == (sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM + sizeof(uint8_t)))) {
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" Received addr list from old agent", 0);
                            memcpy(&(local_ctx->conn_device), p_extra_data, sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM);
                            memcpy(&(local_ctx->conn_device_num), p_extra_data + sizeof(app_conn_device_info_t) * APP_CONN_MAX_DEVICE_NUM, sizeof(uint8_t));
                            g_apps_bt_event_aws_connect_encryp = true;
                        }
                        ret =true;
                        break;
                    }
#endif
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

