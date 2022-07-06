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
 * File: apps_events_bt_event.c
 *
 * Description: This file defines callback of BT and send events to APPs
 *
 */

#include "ui_shell_manager.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "apps_events_bt_event.h"
#include "apps_events_key_event.h"
#include "apps_events_interaction_event.h"
#include "multi_va_event_id.h"
#include "bt_gap_le.h"
#include "bt_gap.h"
#include "bt_sink_srv.h"
#include "bt_connection_manager.h"
#include "bt_device_manager_power.h"
#include "bt_aws_mce_srv.h"
#include "bt_device_manager.h"
#ifdef MTK_RACE_CMD_ENABLE
#include "race_cmd_feature.h"
#include "race_event.h"
#include "race_cmd_dsprealtime.h"
#endif
#include "bt_app_common.h"
#ifdef __BT_FAST_PAIR_ENABLE__
#include "bt_fast_pair.h"
#endif

#include "bt_callback_manager.h"
#include "app_voice_prompt.h"
#include "FreeRTOS.h"
#include "bt_power_on_config.h"
#include "nvkey_id_list.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "apps_config_led_manager.h"
bool g_apps_bt_event_normal_link_partner = false;
#endif
#include "bt_init.h"

#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif

#if (defined __CONN_VP_SYNC_STYLE_ONE__) || (defined __CONN_VP_SYNC_STYLE_TWO__)
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "app_hfp_utils.h"
#endif

#ifdef AIR_RACE_SCRIPT_ENABLE
#include "race_cmd_script.h"
#endif

#define LOG_TAG         "[bt_evt]"

#ifdef __CONN_VP_SYNC_STYLE_TWO__
bool g_apps_bt_event_aws_connect_encryp = false;    /* Flag to indicate whether AWS is connected when encryption_on. */
#ifdef AIR_MULTI_POINT_ENABLE
extern bool g_bt_conn_wait_play_vp;                 /**<  Flag to indicate waiting AWS connect to play sync VP. */
#endif
#endif

#include "app_home_screen_idle_activity.h"

bt_event_suffix_data_t *get_bt_event_suffix_data(void *event_params, size_t param_len)
{
    if (event_params) {
        /* Because the param is 4 bytes align. */
        size_t fixed_len = ((param_len + 3) >> 2) << 2;
        return (bt_event_suffix_data_t *)(event_params + fixed_len);
    } else {
        return NULL;
    }
}

/**
 * @brief      The implementation of the weak symbol bt_sink_srv_event_callback which declared in bt_sink_srv.h.
 */
void bt_sink_srv_event_callback(bt_sink_srv_event_t event_id, void *param, uint32_t param_len)
{
    void *event_params = NULL;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    APPS_LOG_MSGID_I("[UI_SHELL], Srv_event_callback :%x", 1, event_id);

    /*
        if (bt_power_on_get_config_type() == BT_POWER_ON_DUT && event_id == BT_CM_EVENT_VISIBILITY_STATE_UPDATE) {
            bt_bd_addr_t empty_addr = {0};
            bt_cm_visibility_state_update_ind_t *visible_update = (bt_cm_visibility_state_update_ind_t *)param;
            if (!memcmp(bt_device_manager_aws_local_info_get_peer_address(), &empty_addr, sizeof(bt_bd_addr_t))
                    && !visible_update->visibility_state) {
                bt_cm_discoverable(true);
            }
        }
        */

    /* bt_app_common need process the callback in BT task. */
    bt_app_common_on_bt_sink_event_callback(event_id, param, param_len);

    if (NULL != param) {
        /* Calculate the total length, it's 4 bytes align. */
        size_t total_len = (((param_len + 3) >> 2) << 2) + sizeof(bt_event_suffix_data_t);
        event_params = pvPortMalloc(total_len);
        if (NULL != event_params) {
            /* Copy the data into event_params from the data of callback. */
            memcpy(event_params, param, param_len);
#ifdef MTK_AWS_MCE_ENABLE
            /* Append the suffix data. */
            bt_event_suffix_data_t *suffix_data = get_bt_event_suffix_data(event_params, param_len);
            suffix_data->aws_role = role;
#endif
        } else {
            APPS_LOG_MSGID_I("malloc fail", 0);
        }
    }
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BT_SINK, event_id, event_params,
                        param_len, NULL, 0);
}

#if (defined AIR_MULTI_POINT_ENABLE) && (defined MTK_AWS_MCE_ENABLE)
static void apps_events_be_event_vp_callback(uint32_t idx, vp_err_code err)
{
    if ((idx == VP_INDEX_DEVICE_DISCONNECTED) && (err == VP_ERR_CODE_SYNC_FAIL)) {
        APPS_LOG_MSGID_I(LOG_TAG"app_bt_conn, disconnect vp sync fail, wait special link", 0);
        home_screen_local_context_type_t *homescreen_context = app_home_screen_idle_activity_get_context();
        homescreen_context->wait_disconnect = 1;
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
            APPS_EVENTS_INTERACTION_PLAY_SYNC_VP, (void *)0, 0, NULL, 3000);
    }
}
#endif

/**
 * @brief      The implementation of the weak symbol bt_cm_event_callback which declared in bt_connection_manager.h.
 */
void bt_cm_event_callback(bt_cm_event_t event_id, void *params, uint32_t params_len)
{
    void *event_params = NULL;
    APPS_LOG_MSGID_I(LOG_TAG"bt_cm_event_callback() id: %x", 1, event_id);

#ifdef AIR_RACE_SCRIPT_ENABLE
    if (BT_CM_EVENT_REMOTE_INFO_UPDATE == event_id) {
        bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)params;
        if (remote_update) {
            if ((BT_CM_ACL_LINK_CONNECTED > remote_update->acl_state) && (BT_CM_ACL_LINK_CONNECTED <= remote_update->pre_acl_state)) {
                   if (bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_ALL, NULL, 0) == 0){
                       race_script_remote_disconnected();
                   }
               }
        }
    }
#endif

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (BT_CM_EVENT_REMOTE_INFO_UPDATE == event_id) {
        bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)params;
        if (remote_update) {
            /* Notify config_led_manager aws status change. */
            if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                && BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service) {
                apps_config_led_manager_on_aws_attached(role, true);
            } else if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service
                       && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                apps_config_led_manager_on_aws_attached(role, false);
            }

            if ((BT_CM_ACL_LINK_ENCRYPTED == remote_update->acl_state) && (BT_CM_ACL_LINK_ENCRYPTED != remote_update->pre_acl_state)) {
#ifdef __CONN_VP_SYNC_STYLE_ONE__
                APPS_LOG_MSGID_I(LOG_TAG",app_bt_conn encryption on %x:%x:%x:%x:%x:%x", 6, remote_update->address[5], remote_update->address[4],
                                 remote_update->address[3], remote_update->address[2], remote_update->address[1], remote_update->address[0]);
                /* To play connected VP in time, process the BT_CM_ACL_LINK_ENCRYPTED to play VP. */
                bt_bd_addr_t *local_addr;
                local_addr = bt_device_manager_get_local_address();
                if ((role == BT_AWS_MCE_ROLE_AGENT) && (memcmp(local_addr, &(remote_update->address), sizeof(bt_bd_addr_t)) != 0)) {
                    APPS_LOG_MSGID_I(LOG_TAG",app_bt_conn remote device encryption on", 0);
                    /* Sync the VP packet in BT task to ensure AWS not disconnect. There's AWS link switch when connect to SP. */
                    app_vp_type cur_type = app_voice_prompt_get_current_type_unsafe();
                    uint8_t incoming_call = app_hfp_get_incoming_call_vp_flag();
                    if (!incoming_call && cur_type != VP_TYPE_VB && cur_type != VP_TYPE_RT) {
                        app_voice_prompt_set_sync_immediat();
                        app_voice_prompt_play(VP_INDEX_CONNECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL, 0);
                    }
                }
#endif
            } else if ((BT_CM_ACL_LINK_CONNECTED <= remote_update->acl_state) && (BT_CM_ACL_LINK_CONNECTED > remote_update->pre_acl_state)) {
#ifdef __CONN_VP_SYNC_STYLE_TWO__
                /* Set flag to record whether AWS is connected or not when agent connect to SP. */
                if (BT_AWS_MCE_SRV_LINK_NONE == bt_aws_mce_srv_get_link_type()) {
                    g_apps_bt_event_aws_connect_encryp = false;
                } else {
                    g_apps_bt_event_aws_connect_encryp = true;
                }
#endif
            }

#if (defined AIR_MULTI_POINT_ENABLE) && (defined __CONN_VP_SYNC_STYLE_TWO__)
            if ((!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP) & remote_update->pre_connected_service)
                && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->pre_connected_service)
                && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL) & remote_update->pre_connected_service)) &&
                ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP) & remote_update->connected_service)
                || (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->connected_service)
                || (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL) & remote_update->connected_service))) {

                bt_cm_profile_service_mask_t profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL);
                if (bt_cm_get_connected_devices(profile, NULL, 0) > 1) {
                    if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
                        APPS_LOG_MSGID_I(LOG_TAG",app_bt_conn another remote device profile connected and aws connect", 0);
                        /* Sync the VP packet in BT task to ensure AWS not disconnect. There maybe AWS link switch when connect to 2nd SP.*/
                        app_vp_type cur_type = app_voice_prompt_get_current_type_unsafe();
                        uint8_t incoming_call = app_hfp_get_incoming_call_vp_flag();
                        if (!incoming_call && cur_type != VP_TYPE_VB && cur_type != VP_TYPE_RT) {
                            app_voice_prompt_set_sync_immediat();
                            app_voice_prompt_play(VP_INDEX_CONNECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL, 0);
                        }
                    } else {
                        if (g_apps_bt_event_aws_connect_encryp) {
                            /* Partner attached to agent before agent connect to SP, but now not attach again, set timer to wait partner re-attach. */
                            APPS_LOG_MSGID_I(LOG_TAG",app_bt_conn another remote device profile connected, set timer wait aws", 0);
                            g_bt_conn_wait_play_vp = true;
                            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_PLAY_SYNC_VP, (void *)1, 0, NULL, 3000);
                        } else {
                            /* Partner not attached to agent before agent connect to SP, so agent play local VP immediately. */
                            APPS_LOG_MSGID_I(LOG_TAG",app_bt_conn another remote device profile connected, no partner", 0);
                            app_vp_type cur_type = app_voice_prompt_get_current_type_unsafe();
                            uint8_t incoming_call = app_hfp_get_incoming_call_vp_flag();
                            if (!incoming_call && cur_type != VP_TYPE_VB && cur_type != VP_TYPE_RT) {
                                app_voice_prompt_play(VP_INDEX_CONNECTED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL, 0);
                            }
                        }
                    }
                }
            }
#endif
            else if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                     && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state
                     && (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_FOLLOWER_1)) {
#ifdef AIR_MULTI_POINT_ENABLE
                bt_bd_addr_t aws_device;
                uint32_t num = 0;
                bool curr_is_connected = false;
                bool aws_device_connected = false;
                int i = 0;
                bt_bd_addr_t *local_addr;
                local_addr = bt_device_manager_get_local_address();
                home_screen_local_context_type_t *homescreen_context = app_home_screen_idle_activity_get_context();
                /* The AWS device must bt get in BT task to ensure current in the case 2SPs disconnect at the same time. */
                num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), &aws_device, 1);

                for (i = 0; i < APP_CONN_MAX_DEVICE_NUM; i++) {
                    if (memcmp(&(homescreen_context->conn_device[i].addr), &(remote_update->address), sizeof(bt_bd_addr_t)) == 0) {
                        /* Current disconnect addr is in the connected device list. */
                        curr_is_connected = true;
                    }

                    if (num != 0) {
                        /* For takeover, the AWS is attached on the 3rd SP, will play VP by disconnect gpt, can't sync. */
                        if (memcmp(&(homescreen_context->conn_device[i].addr), &aws_device, sizeof(bt_bd_addr_t)) == 0) {
                            /* Current AWS connect addr is in the connected device list. */
                            aws_device_connected = true;
                        }
                    }
                }

                if ((num == 1) && curr_is_connected && aws_device_connected
                        && (memcmp(&(remote_update->address), &aws_device, sizeof(bt_bd_addr_t)) != 0)
                        && (memcmp(local_addr, &aws_device, sizeof(bt_bd_addr_t)) != 0)
                        && (memcmp(local_addr, &(remote_update->address), sizeof(bt_bd_addr_t)) != 0)) {
                    /* The AWS link is not on the disconnect remote device, sync vp. */
                    if (homescreen_context->state == APP_HOME_SCREEN_STATE_IDLE && (!homescreen_context->bt_power_off)) {
                        APPS_LOG_MSGID_I(LOG_TAG",app_bt_conn none aws remote device disconnect, sync vp", 0);
                        homescreen_context->agent_synced_disconn = true;
                        app_vp_type cur_type = app_voice_prompt_get_current_type_unsafe();
                        uint8_t incoming_call = app_hfp_get_incoming_call_vp_flag();
                        if (!incoming_call && cur_type != VP_TYPE_VB && cur_type != VP_TYPE_RT) {
                            app_voice_prompt_play(VP_INDEX_DEVICE_DISCONNECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM,
                                    false, apps_events_be_event_vp_callback, 0);
                        }
                    }
                }
#endif
                if (bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0) == 0) {
                    /* All remote device disconnect, notify VP module do not wait exit sniff callback. */
                    app_voice_prompt_remote_disconnect();
                }
            }  else if (role == BT_AWS_MCE_ROLE_PARTNER || role == BT_AWS_MCE_ROLE_CLINET || role == BT_AWS_MCE_ROLE_FOLLOWER_2) {
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                        /* Partner Connect SP = AWS connected + LINK_NORMAL */
                        APPS_LOG_MSGID_I(LOG_TAG",app_bt_conn partner normal AWS connect", 0);
                        g_apps_bt_event_normal_link_partner = true;
                    }
                }
            }
        }
    } else if (BT_CM_EVENT_POWER_STATE_UPDATE == event_id) {
        bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)params;
        if (power_update && BT_AWS_MCE_ROLE_AGENT == role) {
            if (BT_CM_POWER_STATE_OFF == power_update->power_state) {
                /* Notify config_led_manager aws status change. */
                apps_config_led_manager_on_aws_attached(role, false);
            }
        }
    }
#endif
#ifdef AIR_MULTI_POINT_ENABLE
    if ((BT_CM_EVENT_REMOTE_INFO_UPDATE == event_id) && (params != NULL)
#ifdef MTK_AWS_MCE_ENABLE
        && (role & (BT_AWS_MCE_ROLE_AGENT | BT_AWS_MCE_ROLE_NONE))
#endif
    ) {
        bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)params;
        if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->acl_state
                 && BT_CM_ACL_LINK_DISCONNECTED == remote_update->pre_acl_state) {
            /* For issue BTA-11416, Guanglu Wu. */
            bt_gap_reset_sniff_timer(3000);
        }
    }
#endif

    if (NULL != params && params_len) {
        size_t total_len = (((params_len + 3) >> 2) << 2) + sizeof(bt_event_suffix_data_t);
        event_params = pvPortMalloc(total_len);
        if (event_params) {
            memcpy(event_params, params, params_len);
#ifdef MTK_AWS_MCE_ENABLE
            /* Append the suffix data. */
            bt_event_suffix_data_t *suffix_data = get_bt_event_suffix_data(event_params, params_len);
            suffix_data->aws_role = role;
#endif
        } else {
            APPS_LOG_MSGID_I("malloc fail", 0);
            return;
        }
    }
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER, event_id, event_params,
                        params_len, NULL, 0);
}

#ifdef MTK_AWS_MCE_ENABLE
/**
 * @brief      The implementation of the weak symbol bt_cm_event_callback which declared in bt_aws_mce_srv.h.
 */
void bt_aws_mce_srv_event_callback(bt_aws_mce_srv_event_t event_id, void *params, uint32_t params_len)
{
    void *event_params = NULL;
    if (NULL != params && params_len) {
        event_params = pvPortMalloc(params_len);
        if (event_params) {
            memcpy(event_params, params, params_len);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_AWS, event_id, event_params,
                                params_len, NULL, 0);
        } else {
            APPS_LOG_MSGID_I("bt_aws_mce_srv_event_callback malloc fail", 0);
        }
    }
}
#endif

/**
 * @brief      The implementation of the bt event callback, refer to bt_callback_manager.h.
 * @param[in]  msg, the message type of the callback event.
 * @param[in]  status, the status, refer to bt_status_t.
 * @param[in]  buffer, the data buffer.
 */
static void registered_bt_event_callback(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    apps_bt_event_data_t *bt_data = NULL;
    if (BT_GAP_LE_ADVERTISING_REPORT_IND == msg || BT_GAP_LE_EXT_ADVERTISING_REPORT_IND == msg) {
        // Because the BT module sends a large amounts of the messages, must ignore it here. If customer want to receive the message, must check it here.
    } else {
        uint32_t buffer_size;
        switch (msg) {
            /* This is an example. If the user wants to use the buffer in app, the user must copy the buffer to a heap memory; */
            case BT_GAP_LE_DISCONNECT_IND:
                buffer_size = sizeof(bt_gap_le_disconnect_ind_t);
                break;
#ifdef AIR_TILE_ENABLE
            case BT_GAP_LE_READ_RSSI_CNF:
                buffer_size = sizeof(bt_hci_evt_cc_read_rssi_t);
                break;
            case BT_GAP_LE_CONNECTION_UPDATE_IND:
                buffer_size = sizeof(bt_gap_le_connection_update_ind_t);
                break;
#endif
            case BT_GAP_LE_CONNECT_IND:
                buffer_size = sizeof(bt_gap_le_connection_ind_t);
                break;
            case BT_GAP_LE_RPA_ROTAION_IND:
                buffer_size = sizeof(bt_gap_le_rpa_rotation_ind_t);
                break;
            default:
                buffer_size = 0;
                break;
        }
        /* To decrease the times of calling malloc, append buffer after bt_data */
        bt_data = (apps_bt_event_data_t *)pvPortMalloc(sizeof(apps_bt_event_data_t) + buffer_size);
        if (bt_data) {
            bt_data->status = status;
            bt_data->buffer = NULL;
            if (buffer_size) {
                memcpy(((uint8_t *)bt_data) + sizeof(apps_bt_event_data_t), buffer, buffer_size);
                bt_data->buffer = ((uint8_t *)bt_data) + sizeof(apps_bt_event_data_t);
            }
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BT,
                                msg, bt_data, sizeof(apps_bt_event_data_t), NULL, 0);
        }

    }
}

static bt_status_t bt_device_manager_callback(bt_device_manager_power_event_t evt, bt_device_manager_power_status_t status, void *data, uint32_t data_len)
{
    void *extra_data = NULL;

    if (data && data_len > 0) {
        extra_data = pvPortMalloc(data_len);
        if (extra_data) {
            memcpy(extra_data, data, data_len);
        } else {
            APPS_LOG_MSGID_E("bt_device_manager_callback malloc fail", 0);
            return BT_STATUS_FAIL;
        }
    }
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER,
            (evt & 0xFFFF) | ((status << 16) & 0xFFFF0000), extra_data, data_len, NULL, 0);
    return BT_STATUS_SUCCESS;
}

void bt_event_get_bt_dm_event_and_status(uint32_t event_id, bt_device_manager_power_event_t *p_evt, bt_device_manager_power_status_t *p_status)
{
    if (p_evt) {
        *p_evt = event_id & 0xFFFF;
    }
    if (p_status) {
        *p_status = (event_id >> 16) & 0xFFFF;
    }
}

void apps_events_bt_event_init(void)
{
    /* Only care a part of events, so the second parameter is not all modules. */
    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                          (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_SYSTEM),
                                          (void *)registered_bt_event_callback);
    bt_device_manager_register_callback(bt_device_manager_callback);
}

#ifdef __BT_FAST_PAIR_ENABLE__
/**
 * @brief      The implementation of the weak symbol of bt_fast_pair_app_event_callback which is declared in bt_fast_pair.h.
 */
bt_fast_pair_status_t bt_fast_pair_app_event_callback(bt_fast_pair_app_event_t evt, void *buf)
{
    uint8_t *msg_buf = NULL;
    APPS_LOG_MSGID_I("[UI_SHELL], fast_pair_event_callback :%x", 1, buf);

    if (BT_FAST_PAIR_APP_EVENT_NEED_STORE_ACCOUNT_KEY == evt) {
        /* The buffer of this event is a number but on a pointer. */
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,
                            evt, buf, 0, NULL, 0);
        return BT_FAST_PAIR_STATUS_SUCCESS;
    }
    if (buf) {
        msg_buf = (uint8_t *)pvPortMalloc(BT_FAST_PAIR_EVENT_BUFFER_MAXIMUM_LENGTH);
        if (msg_buf) {
            memcpy(msg_buf, buf, BT_FAST_PAIR_EVENT_BUFFER_MAXIMUM_LENGTH);
        } else {
            APPS_LOG_MSGID_E("fast pair send event malloc fail", 0);
            return BT_FAST_PAIR_STATUS_SUCCESS;
        }
    }
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_BT_FAST_PAIR,
                        evt,
                        msg_buf,
                        BT_FAST_PAIR_EVENT_BUFFER_MAXIMUM_LENGTH,
                        NULL, 0);
    return BT_FAST_PAIR_STATUS_SUCCESS;


}
#endif

#ifdef MTK_RACE_CMD_ENABLE
/**
 * @brief      Process the fota type race cmd, send event to APPs.
 * @param[in]  event_type, the events type defined in race_event_type_enum.
 * @param[in]  param, parameter of the event.
 * @param[in]  user_data, The same user_data input in race_event_register().
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
static RACE_ERRCODE bt_race_fota_app_event_callback(race_event_type_enum event_type, void *param, void *user_data)
{
    APPS_LOG_MSGID_I("[UI_SHELL], FOTA_APP_event_callback :%x", 1, param);

    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_FOTA,
                        event_type,
                        param,
                        0,
                        NULL, 0);
    return RACE_ERRCODE_SUCCESS;
}

static RACE_ERRCODE bt_race_audio_event_callback(void *param, void *user_data)
{
    race_dsprealtime_notify_struct *audio_param = (race_dsprealtime_notify_struct *)param;

    switch (audio_param->dsp_realtime_race_id) {
#ifdef MTK_ANC_ENABLE
        case RACE_DSPREALTIME_ANC:{
            bool anc_onoff = FALSE;
            if(audio_param->dsp_realtime_race_evet == RACE_ANC_ON){
                APPS_LOG_MSGID_I("[Rdebug]Race cmd enable ANC ret(%d).", 1, audio_param->result);
                anc_onoff = TRUE;
            } else if(audio_param->dsp_realtime_race_evet == RACE_ANC_OFF){
                APPS_LOG_MSGID_I("[Rdebug]Race cmd disable ANC ret(%d).", 1, audio_param->result);
                anc_onoff = TRUE;
            }
            APPS_LOG_MSGID_I("[ANC_SRV] RaceCMD anc_onoff=%d", 1, anc_onoff);
            if (anc_onoff) {
                app_anc_service_play_vp();
            }
        }
#endif
        default:
            break;
    }

    return RACE_ERRCODE_SUCCESS;
}

#ifdef RACE_FIND_ME_ENABLE
#include "app_fm_activity.h"
/**
 * @brief      Process the find me type race cmd, send event to APPs.
 * @param[in]  event_type, the events type defined in race_event_type_enum.
 * @param[in]  param, parameter of the event.
 * @param[in]  user_data, The same user_data input in race_event_register().
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
static RACE_ERRCODE bt_race_findme_app_event_callback(race_event_type_enum event_type, void *param, void *user_data)
{
    APPS_LOG_MSGID_I("[UI_SHELL], FOTA_findme_event_callback :%x", 1, param);
    if (RACE_EVENT_TYPE_FIND_ME == event_type && param) {
        app_find_me_param_struct *find_me_param = (app_find_me_param_struct *)pvPortMalloc(sizeof(app_find_me_param_struct));
        if (find_me_param) {
            find_me_param->blink = ((race_event_find_me_param_struvt *)param)->is_blink;
            find_me_param->tone = ((race_event_find_me_param_struvt *)param)->is_tone;
            find_me_param->duration_seconds = 0;
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_FINDME,
                                APP_FIND_ME_EVENT_ID_TRIGGER,
                                find_me_param,
                                sizeof(app_find_me_param_struct),
                                NULL, 0);
        }
    }
    return RACE_ERRCODE_SUCCESS;
}

#endif

/**
 * @brief      Process the reload nvkey type race cmd.
 * @param[in]  param, parameter of the event.
 * @param[in]  user_data, The same user_data input in race_event_register().
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
static RACE_ERRCODE bt_race_reload_nvkey_event_callback(void *param, void *user_data)
{
    uint16_t nvkey_id = *(uint16_t *)param;
    APPS_LOG_MSGID_I("[UI_SHELL], bt_race_reload_nvkey_event_callback :%x", 1, nvkey_id);
    /* Need reload NVKEYID_CONFIGURABLE_KEY_ACTION_TABLE, send event to refresh key remapping table. */
    if (NVKEYID_CONFIGURABLE_KEY_ACTION_TABLE == nvkey_id) {
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_RELOAD_KEY_ACTION_FROM_NVKEY, NULL, 0,
                            NULL, 0);
        return RACE_ERRCODE_SUCCESS;
        /* Need reload NVKEYID_VA_SWITH, send event to set va type and reboot. */
    } else if (NVKEYID_VA_SWITH == nvkey_id) {
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_MULTI_VA,
                            MULTI_VA_EVENT_SET_VA, NULL, 0,
                            NULL, 1000);
        return RACE_ERRCODE_SUCCESS;
    } else {
        return RACE_ERRCODE_NOT_SUPPORT;
    }
}

/**
 * @brief      Process the simulate key event race cmd.
 * @param[in]  param, parameter of the event, means the simulated key action.
 * @param[in]  user_data, The same user_data input in race_event_register().
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
static RACE_ERRCODE bt_race_key_app_event_callback(void *param, void *user_data)
{
    uint16_t *msg_data = (uint16_t *)param;
    APPS_LOG_MSGID_I("[UI_SHELL], race_key_event_callback :%x", 1, *msg_data);
    uint16_t *p_key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
    if (p_key_action) {
        *p_key_action = *msg_data;
        /* The extra_data of the event is key action. */
        ui_shell_send_event(false,
                            EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                            INVALID_KEY_EVENT_ID,
                            p_key_action, sizeof(uint16_t), NULL, 0);
    }
    return RACE_ERRCODE_SUCCESS;
}

/**
 * @brief      Process the power testing request race cmd.
 * @param[in]  param, parameter of the event.
 * @param[in]  user_data, The same user_data input in race_event_register().
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
static RACE_ERRCODE bt_race_system_power_event_callback(void *param, void *user_data)
{
    uint8_t *msg_data = (uint8_t *)param;
    APPS_LOG_MSGID_I("[UI_SHELL], race_system_power_event_callback :%x", 1, *msg_data);

    switch (*msg_data) {
        case RACE_SYSTEM_POWER_PMUOFF:
            APPS_LOG_MSGID_I("[UI_SHELL], race_system_power_event_callback: send event OFF mode", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_SYSTEM_POWER,
                                APPS_EVENTS_SYSTEM_POWER_PMUOFF, NULL, 0,
                                NULL, 0);
            break;
        case RACE_SYSTEM_POWER_RTC:
            APPS_LOG_MSGID_I("[UI_SHELL], race_system_power_event_callback: send event RTC mode", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_SYSTEM_POWER,
                                APPS_EVENTS_SYSTEM_POWER_RTC, NULL, 0,
                                NULL, 0);
            break;
        case RACE_SYSTEM_POWER_SLEEP:
            APPS_LOG_MSGID_I("[UI_SHELL], race_system_power_event_callback: send event SLEEP mode", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_SYSTEM_POWER,
                                APPS_EVENTS_SYSTEM_POWER_SLEEP, NULL, 0,
                                NULL, 0);
            break;
        default:
            break;
    }

    return RACE_ERRCODE_SUCCESS;
}


#ifdef MTK_LEAKAGE_DETECTION_ENABLE
/**
 * @brief      Process the leakage detection voice prompt request race cmd.
 * @param[in]  param, parameter of the event.
 * @param[in]  user_data, The same user_data input in race_event_register().
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
static RACE_ERRCODE bt_race_cmd_vp_callback(void *param, void *user_data)
{
    race_cmd_vp_struct *vp_param = (race_cmd_vp_struct *)param;
    bool *p_play_flag = NULL;
    ui_shell_status_t status = UI_SHELL_STATUS_OK;

    if (vp_param == NULL) {
        APPS_LOG_MSGID_I("bt_race_cmd_vp_callback invalid param.", 0);
        return RACE_ERRCODE_FAIL;
    }

    switch (vp_param->vp_type) {
        case RACE_CMD_VP_LEAKAGE_DETECTION:
            p_play_flag = (bool *)pvPortMalloc(sizeof(bool));
            if (p_play_flag == NULL) {
                APPS_LOG_MSGID_I("bt_race_cmd_vp_callback malloc failed.", 0);
                return RACE_ERRCODE_NOT_ENOUGH_MEMORY;
            }
            *p_play_flag = vp_param->play_flag;
            status = ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                         APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_VP_TRIGGER,
                                         p_play_flag, sizeof(bool), NULL, 0);
            if (UI_SHELL_STATUS_OK != status) {
                vPortFree(p_play_flag);
                APPS_LOG_MSGID_I("UI_SHELL send race cmd vp trigger event failed.", 0);
            }
            break;
        default:
            break;
    }

    return RACE_ERRCODE_SUCCESS;
}
#endif

#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
static RACE_ERRCODE bt_race_cmd_ANC_adaptive_FF_callback(race_event_type_enum event_type, void *param, void *user_data)
{
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_ANC_FF_RACE, event_type, NULL, 0, NULL, 0);
    return RACE_ERRCODE_SUCCESS;
}
#endif

#ifdef MTK_ANC_ENABLE
static RACE_ERRCODE bt_race_cmd_anc_gain_callback(race_event_type_enum event_type, void *param, void *user_data)
{
    if (event_type == RACE_EVENT_TYPE_ANC_GAIN_CONTROL) {
        anc_gain_control_param_t *new_param = (anc_gain_control_param_t *)pvPortMalloc(sizeof(anc_gain_control_param_t));
        if (new_param != NULL) {
            memset(new_param, 0, sizeof(anc_gain_control_param_t));
            memcpy(new_param, param, sizeof(anc_gain_control_param_t));
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN_RACE,
                                event_type, new_param, sizeof(anc_gain_control_param_t), NULL, 0);
        }
    } else {
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_ANC_EXTEND_GAIN_RACE,
                            event_type, param, 0, NULL, 0);
    }
    return RACE_ERRCODE_SUCCESS;
}
#endif

/**
 * @brief      Process the app race events.
 * @param[in]  id, The same register_id obtained when calling race_event_register().
 * @param[in]  event_type, The type of the event occurs
 * @param[in]  param, The parameter of the event
 * @param[in]  user_data, The same user_data input in race_event_register().
 * @return RACE_ERRCODE_SUCCESS succeed; otherwise, fail.
 */
static RACE_ERRCODE bt_race_app_event_callback(int32_t id, race_event_type_enum event_type, void *param, void *user_data)
{
    switch (event_type) {
        case RACE_EVENT_SYSTEM_POWER:
            return bt_race_system_power_event_callback(param, user_data);
        case RACE_EVENT_RELOAD_NVKEY_TO_RAM:
            return bt_race_reload_nvkey_event_callback(param, user_data);
        case RACE_EVENT_TYPE_KEY:
            return bt_race_key_app_event_callback(param, user_data);
#ifdef RACE_FIND_ME_ENABLE
        case RACE_EVENT_TYPE_FIND_ME:
            return bt_race_findme_app_event_callback(event_type, param, user_data);
#endif
        case RACE_EVENT_TYPE_FOTA_START:
        case RACE_EVENT_TYPE_FOTA_CANCELLING:
        case RACE_EVENT_TYPE_FOTA_CANCEL:
        case RACE_EVENT_TYPE_FOTA_NEED_REBOOT:
            return bt_race_fota_app_event_callback(event_type, param, user_data);
#ifdef MTK_LEAKAGE_DETECTION_ENABLE
        case RACE_EVENT_TYPE_CMD_VP:
            return bt_race_cmd_vp_callback(param, user_data);
#endif
#ifdef MTK_USER_TRIGGER_ADAPTIVE_FF_V2
        case RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_START:
        case RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_CANCEL:
        case RACE_EVENT_TYPE_ANC_ADAPTIVE_FF_END:
            return bt_race_cmd_ANC_adaptive_FF_callback(event_type, param, user_data);
#endif
#ifdef MTK_ANC_ENABLE
        case RACE_EVENT_TYPE_ANC_GAIN_USER_UNAWARE:
        case RACE_EVENT_TYPE_ANC_GAIN_WIND_NOISE:
        case RACE_EVENT_TYPE_ANC_GAIN_ENVIRONMENT_DETECTION:
        case RACE_EVENT_TYPE_ANC_GAIN_CONTROL:
            return bt_race_cmd_anc_gain_callback(event_type, param, user_data);
#endif
        case RACE_EVENT_TYPE_AUDIO_DSP_REALTIME:
            return bt_race_audio_event_callback(param, user_data);
        default:
            break;
    }
    return RACE_ERRCODE_NOT_SUPPORT;
}

/**
 * @brief      Initialize race app callback.
 */
void bt_race_app_event_init()
{
    int32_t register_id = 0;

    APPS_LOG_MSGID_I("[UI_SHELL], bt_race_app_event_init", 0);

    race_event_register(&register_id, bt_race_app_event_callback, NULL);


}
#endif

void apps_bt_events_le_service_callback(bt_gap_le_srv_event_t event, void *data)
{
    uint32_t data_len = 0;
    void *extra_data = NULL;
    APPS_LOG_MSGID_I(LOG_TAG"apps_bt_events_le_service_callback() event: %x", 1, event);
    switch (event) {
        case BT_GAP_LE_SRV_EVENT_ADV_COMPLETE:
            data_len = sizeof(bt_gap_le_srv_adv_complete_t);
            break;
        case BT_GAP_LE_SRV_EVENT_ADV_CLEARED:
        case BT_GAP_LE_SRV_EVENT_CONN_CLEARED:
        case BT_GAP_LE_SRV_EVENT_BLE_DISABLED:
            data_len = sizeof(bt_gap_le_srv_common_result_t);
            break;
        case BT_GAP_LE_SRV_EVENT_CONN_UPDATED:
            data_len = sizeof(bt_gap_le_srv_conn_update_t);
            break;
        default:
            return;
    }
    extra_data = pvPortMalloc(data_len);
    if (extra_data) {
        memcpy(extra_data, data, data_len);
    } else {
        return;
    }
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_LE_SERVICE,
                        event,
                        extra_data,
                        data_len,
                        NULL, 0);
}

#if defined(MTK_AWS_MCE_ENABLE)
/**
 * @brief      callback of received aws data.
 * @param[in]  para, The parameter of the event.
 */
void app_aws_report_event_callback(bt_aws_mce_report_info_t *para)
{
    APPS_LOG_MSGID_I("[UI_SHELL], app_aws_report_event_callback: send aws report to app: module 0x%0x", 1, para->module_id);
    /* Malloc the bt_aws_mce_report_info_t and the param of bt_aws_mce_report_info_t in a whole memory. */
    bt_aws_mce_report_info_t *extra_data = pvPortMalloc(sizeof(bt_aws_mce_report_info_t) + para->param_len);
    if (extra_data == NULL) {
        APPS_LOG_MSGID_I("[UI_SHELL], app_aws_report_event_callback: malloc fail", 0);
        return;
    }

    memcpy(extra_data, para, sizeof(bt_aws_mce_report_info_t));
    extra_data->param = (void *)extra_data + sizeof(bt_aws_mce_report_info_t);
    /* Append the content of the para->param to extra_data->param. */
    memcpy(extra_data->param, para->param, para->param_len);

    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_AWS_DATA,
                        0, extra_data, sizeof(bt_aws_mce_report_info_t) + para->param_len,
                        NULL, 0);
}

/**
 * @brief      initialize the aws report.
 * @param[in]  para, The parameter of the event.
 */
void app_aws_report_event_init()
{
    APPS_LOG_MSGID_I("[UI_SHELL], app_aws_report_event_init", 0);

    /* Only care a part of events, so only register a part of BT_AWS_MCE_REPORT_MODULE */
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_BATTERY, app_aws_report_event_callback);
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_APP_ACTION, app_aws_report_event_callback);
#ifdef MTK_SMART_CHARGER_ENABLE
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_SMCHARGER, app_aws_report_event_callback);
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_CHARGER_CASE, app_aws_report_event_callback);
#endif
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_IN_EAR, app_aws_report_event_callback);
#endif
#ifdef AIR_MULTI_POINT_ENABLE
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_APP_EMP, app_aws_report_event_callback);
#endif
}


#endif

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
void bt_ulla_callback(bt_ull_event_t event, void *param, uint32_t param_len)
{
    void *extra_data = NULL;
    APPS_LOG_MSGID_I(LOG_TAG"bt_ulla_callback: %x", 1, event);
    if (param) {
        extra_data = pvPortMalloc(param_len);
        if (extra_data) {
            memcpy(extra_data, param, param_len);
        } else {
            APPS_LOG_MSGID_E(LOG_TAG"bt_ulla_callback: malloc fail", 0);
            return;
        }
    }
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_BT_ULTRA_LOW_LATENCY,
                    event, extra_data, param_len, NULL, 0);
}
#endif
