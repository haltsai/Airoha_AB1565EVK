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

#include "app_speaker_mode_idle_activity.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "bt_app_common.h"
#include "apps_debug.h"
#include "apps_config_key_remapper.h"
#include "apps_events_key_event.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_bt_aws_mce_le_association.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#include "bt_device_manager.h"
#include "FreeRTOS.h"
#include "apps_aws_sync_event.h"
#include "ui_shell_manager.h"
#include "multi_ble_adv_manager.h"
#include "apps_config_key_remapper.h"
#include "atci.h"
#ifdef APPS_LINE_IN_SUPPORT
#include "app_line_in_idle_activity.h"
#endif
#ifdef APPS_USB_AUDIO_SUPPORT
#include "app_usb_audio_idle_activity.h"
#endif

#define STRNCPY(dest, source) strncpy(dest, source, strlen(source)+1);

bool g_app_wait_to_scan = false;
static app_speaker_mode_local_context_type_t app_speaker_mode_context = {0};

static bool _proc_ui_shell_group(
        struct _ui_shell_activity *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = true; // UI shell internal event must process by this activity, so default is true

    switch (event_id) {
    case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
        APPS_LOG_MSGID_I("app_speaker_mode_activity create", 0);
        self->local_context = (void *)&app_speaker_mode_context;
        if (self->local_context) {
            memset(self->local_context, 0, sizeof(app_speaker_mode_local_context_type_t));
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            bt_aws_mce_srv_mode_t mode = bt_aws_mce_srv_get_mode();
            APPS_LOG_MSGID_I("app_speaker_mode_activity create, mode %x, role %x", 2, mode, role);
        }
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
        APPS_LOG_MSGID_I("app_speaker_mode_activity destroy", 0);
        if (self->local_context) {
            vPortFree(self->local_context);
        }
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
        APPS_LOG_MSGID_I("app_speaker_mode_activity resume", 0);
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
        APPS_LOG_MSGID_I("app_speaker_mode_activity pause", 0);
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
        APPS_LOG_MSGID_I("app_speaker_mode_activity refresh", 0);
        break;
    case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
        APPS_LOG_MSGID_I("app_speaker_mode_activity result", 0);
        break;
    default:
        break;
    }
    return ret;
}

static bool app_speaker_mode_check_key_ignore(app_speaker_mode_local_context_type_t *local_context)
{
    bool pair_ignored = false;

    if (local_context->status != SPEAKER_STATUS_NONE) {
        APPS_LOG_MSGID_I("app_speaker_mode_activity: is pairing, ignore key", 0);
        pair_ignored = true;
    }
#ifdef APPS_LINE_IN_SUPPORT
    else if(app_line_in_activity_get_current_audio_path() == APP_AUDIO_PATH_LINE_IN){
        APPS_LOG_MSGID_I("app_speaker_mode_activity: line in, ignore key", 0);
        pair_ignored = true;
    }
#endif
#ifdef APPS_USB_AUDIO_SUPPORT
    else if(app_usb_audio_is_open()){
        APPS_LOG_MSGID_I("app_speaker_mode_activity: usb in, ignore key", 0);
        pair_ignored = true;
    }
#endif

    if (pair_ignored) {
        atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_URC_FORMAT};
        STRNCPY((char *)response.response_buf,"Is paring, ignore cmd.\r\n");
        response.response_len = strlen((char *)response.response_buf);
        atci_send_response(&response);
    } else {
        apps_config_state_t mmi_state = apps_config_key_get_mmi_state();
        if (mmi_state >= APP_HFP_INCOMING && mmi_state <= APP_HFP_MULTITPART_CALL) {
            atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_URC_FORMAT};
            STRNCPY((char *)response.response_buf,"Is in call, ignore cmd.\r\n");
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            pair_ignored = true;
        }
    }

    return pair_ignored;
}

static void app_speaker_mode_print_ignore()
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_URC_FORMAT};
    STRNCPY((char *)response.response_buf,"Already in the target mode, ignore cmd.\r\n");
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
}

static bool app_speaker_mode_disconnect_all()
{
    bt_cm_connect_t disconn = {{0}, 0};
    bt_bd_addr_t addr = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    memcpy(disconn.address, &addr, sizeof(bt_bd_addr_t));
    disconn.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;

    APPS_LOG_MSGID_I("app_speaker_mode_activity: disconnect all", 0);
    if (bt_cm_disconnect(&disconn) != BT_STATUS_SUCCESS) {
        APPS_LOG_MSGID_W("app_speaker_mode_activity: disconnect fail", 0);
        return false;
    } else {
        return true;
    }
}

static bool app_speaker_check_single(bt_aws_mce_srv_mode_t mode)
{
    if (mode == BT_AWS_MCE_SRV_MODE_NORMAL || mode == BT_AWS_MCE_SRV_MODE_SINGLE) {
        APPS_LOG_MSGID_I("app_speaker_mode_activity: already in single mode", 0);
        app_speaker_mode_print_ignore();
        return true;
    } else {
        return false;
    }
}

static bool _proc_key_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    app_speaker_mode_local_context_type_t *local_context = (app_speaker_mode_local_context_type_t *)(self->local_context);
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_aws_mce_srv_mode_t mode = bt_aws_mce_srv_get_mode();

    uint8_t key_id;
    airo_key_event_t key_event;
    apps_config_key_action_t action;

    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        app_event_key_event_decode(&key_id, &key_event, event_id);
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }

    switch (action) {
    case KEY_BROADCAST_AGENT:
        ret = true;
        APPS_LOG_MSGID_I("app_speaker_mode_activity: broadcast mode agent", 0);
        if (app_speaker_mode_check_key_ignore(local_context)) {
            break;
        } else {
            apps_config_set_vp(VP_INDEX_PAIRING, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            local_context->target_mode = BT_AWS_MCE_SRV_MODE_BROADCAST;
            local_context->target_role = BT_AWS_MCE_ROLE_AGENT;
            local_context->status = SPEAKER_STATUS_PAIRING;

            if ((role == BT_AWS_MCE_ROLE_AGENT && mode == BT_AWS_MCE_SRV_MODE_BROADCAST)
                    || (role == BT_AWS_MCE_ROLE_NONE && apps_config_key_get_mmi_state() < APP_CONNECTED)) {

            } else {
                if(app_speaker_mode_disconnect_all()) {
                    local_context->status = SPEAKER_STATUS_DISCONNECTING;
                    break;
                }
            }

            bt_aws_mce_le_association_set_app_paramters(BT_AWS_MCE_ROLE_AGENT, BT_AWS_MCE_SRV_MODE_BROADCAST);
            bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_START_ADV, NULL);
        }
        break;
    case KEY_BROADCAST_CLIENT:
        ret = true;
        APPS_LOG_MSGID_I("app_speaker_mode_activity: broadcast mode client", 0);
        if (app_speaker_mode_check_key_ignore(local_context)) {
            break;
        } else if (mode == BT_AWS_MCE_SRV_MODE_BROADCAST && role == BT_AWS_MCE_ROLE_CLINET) {
            APPS_LOG_MSGID_I("app_speaker_mode_activity: in target mode, ignore key", 0);
            app_speaker_mode_print_ignore();
            break;
        } else {
            apps_config_set_vp(VP_INDEX_PAIRING, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            local_context->target_mode = BT_AWS_MCE_SRV_MODE_BROADCAST;
            local_context->target_role = BT_AWS_MCE_ROLE_CLINET;
            local_context->status = SPEAKER_STATUS_PAIRING;

            if (role != BT_AWS_MCE_ROLE_NONE || apps_config_key_get_mmi_state() >= APP_CONNECTED) {
                if (app_speaker_mode_disconnect_all()){
                    local_context->status = SPEAKER_STATUS_DISCONNECTING;
                    break;
                }
            }

            bt_aws_mce_le_association_set_app_paramters(BT_AWS_MCE_ROLE_CLINET, BT_AWS_MCE_SRV_MODE_BROADCAST);
            bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_START_SCAN, NULL);
        }
        break;
    case KEY_DOUBLE_AGENT:
        ret = true;
        APPS_LOG_MSGID_I("app_speaker_mode_activity: double mode agent", 0);
        if (app_speaker_mode_check_key_ignore(local_context)) {
            break;
        } else if (mode == BT_AWS_MCE_SRV_MODE_DOUBLE && role == BT_AWS_MCE_ROLE_AGENT) {
            APPS_LOG_MSGID_I("app_speaker_mode_activity: in target mode, ignore key", 0);
            app_speaker_mode_print_ignore();
            break;
        } else {
            apps_config_set_vp(VP_INDEX_PAIRING, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            local_context->target_mode = BT_AWS_MCE_SRV_MODE_DOUBLE;
            local_context->target_role = BT_AWS_MCE_ROLE_AGENT;
            local_context->status = SPEAKER_STATUS_PAIRING;

            if (role != BT_AWS_MCE_ROLE_NONE || apps_config_key_get_mmi_state() >= APP_CONNECTED) {
                if (app_speaker_mode_disconnect_all()){
                    local_context->status = SPEAKER_STATUS_DISCONNECTING;
                    break;
                }
            }

            bt_aws_mce_le_association_set_app_paramters(BT_AWS_MCE_ROLE_AGENT, BT_AWS_MCE_SRV_MODE_DOUBLE);
            bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_START_ADV, NULL);
        }
        break;
    case KEY_DOUBLE_PARTNER:
        APPS_LOG_MSGID_I("app_speaker_mode_activity: double mode partner", 0);
        ret = true;
        if (app_speaker_mode_check_key_ignore(local_context)) {
            break;
        } else if (mode == BT_AWS_MCE_SRV_MODE_DOUBLE && role == BT_AWS_MCE_ROLE_PARTNER) {
            APPS_LOG_MSGID_I("app_speaker_mode_activity: in target mode, ignore key", 0);
            app_speaker_mode_print_ignore();
            break;
        } else {
            apps_config_set_vp(VP_INDEX_PAIRING, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            local_context->target_mode = BT_AWS_MCE_SRV_MODE_DOUBLE;
            local_context->target_role = BT_AWS_MCE_ROLE_PARTNER;
            local_context->status = SPEAKER_STATUS_PAIRING;

            if (role != BT_AWS_MCE_ROLE_NONE || apps_config_key_get_mmi_state() >= APP_CONNECTED) {
                if (app_speaker_mode_disconnect_all()){
                    local_context->status = SPEAKER_STATUS_DISCONNECTING;
                    break;
                }
            }

            bt_aws_mce_le_association_set_app_paramters(BT_AWS_MCE_ROLE_PARTNER, BT_AWS_MCE_SRV_MODE_DOUBLE);
            bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_START_SCAN, NULL);
        }
        break;
    case KEY_SPK_SINGLE:
        APPS_LOG_MSGID_I("app_speaker_mode_activity: key single", 0);
        if (app_speaker_check_single(mode)) {
            break;
        }
        bt_aws_mce_le_association_app_switch_to_single();

        local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
        local_context->target_role = BT_AWS_MCE_ROLE_NONE;
        ret = true;
        break;
    case KEY_SPK_UNGROUP:
        APPS_LOG_MSGID_I("app_speaker_mode_activity: key ungroup", 0);
        if (app_speaker_check_single(mode)) {
            break;
        }
        bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_UNGROUP, NULL);

        local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
        local_context->target_role = BT_AWS_MCE_ROLE_NONE;
        ret = true;
        break;
    default:
        break;
    }

    return ret;
}


static bool _proc_aws_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bool need_reconnect = false;
    app_speaker_mode_local_context_type_t *local_context = (app_speaker_mode_local_context_type_t *)(self->local_context);

    if (BT_AWS_MCE_SRV_EVENT_MODE_CHANGED_IND == event_id) {
        bt_aws_mce_srv_mode_changed_ind_t *param = (bt_aws_mce_srv_mode_changed_ind_t *)extra_data;

        if (param == NULL) {
            APPS_LOG_MSGID_I("app_speaker_mode_activity mode switched, no switch.", 0);
            local_context->status = SPEAKER_STATUS_NONE;
            return ret;
        } else {

            APPS_LOG_MSGID_I("app_speaker_mode_activity mode switched: mode %d->%d, role %x->%x", 4,
                    param->pre_mode, param->mode, param->pre_role, param->role);

            if ((param->mode == BT_AWS_MCE_SRV_MODE_SINGLE) && (local_context->target_mode == BT_AWS_MCE_SRV_MODE_SINGLE)) {
                if (local_context->status != SPEAKER_STATUS_FAIL) {
                    apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                }
                local_context->status = SPEAKER_STATUS_NONE;
                need_reconnect = true;
            } else if((param->mode == BT_AWS_MCE_SRV_MODE_SINGLE) && (local_context->target_mode != BT_AWS_MCE_SRV_MODE_SINGLE)){
                bt_aws_mce_le_association_set_app_paramters(local_context->target_role, local_context->target_mode);
                if ((local_context->target_role == BT_AWS_MCE_ROLE_PARTNER) || (local_context->target_role == BT_AWS_MCE_ROLE_CLINET)){
                    bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_START_SCAN, NULL);
                } else {
                    bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_START_ADV, NULL);
                }
            } else {
                need_reconnect = true;
                if (param->role == BT_AWS_MCE_ROLE_AGENT) {
                    local_context->status = SPEAKER_STATUS_NONE;
                } else {
                    local_context->status = SPEAKER_STATUS_MODE_SWITCHED;
                }
            }

        }

        if (need_reconnect && (param->role == BT_AWS_MCE_ROLE_AGENT || param->role == BT_AWS_MCE_ROLE_NONE)) {
            bt_bd_addr_t *p_bd_addr = bt_device_manager_remote_get_dev_by_seq_num(1);
            bt_cm_connect_t connect_param = { {0},
                BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
            };
            if (p_bd_addr) {
                memcpy(connect_param.address, *p_bd_addr, sizeof(bt_bd_addr_t));
                APPS_LOG_MSGID_I("app_speaker_mode_activity reconnect SP", 0);
                bt_cm_connect(&connect_param);
            }
        }

        multi_ble_adv_manager_start_ble_adv();

        atci_response_t *response = pvPortMalloc(sizeof(atci_response_t));
        if (response == NULL) {
            return ret;
        }
        memset(response, 0, sizeof(atci_response_t));
        response->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;

        if (param->mode == BT_AWS_MCE_SRV_MODE_SINGLE) {
            STRNCPY((char *)response->response_buf,"\r\n+Mode Changed: Single mode, ");
        } else if (param->mode == BT_AWS_MCE_SRV_MODE_BROADCAST) {
            STRNCPY((char *)response->response_buf,"\r\n+Mode Changed: Broadcast mode, ");
        } else if (param->mode == BT_AWS_MCE_SRV_MODE_DOUBLE) {
            STRNCPY((char *)response->response_buf,"\r\n+Mode Changed: Double mode, ");
        }

        if (param->role == BT_AWS_MCE_ROLE_NONE) {
            strcat((char *)response->response_buf,"None role.\r\n");
        } else if (param->role == BT_AWS_MCE_ROLE_AGENT) {
            strcat((char *)response->response_buf,"Agent role.\r\n");
        } else if (param->role == BT_AWS_MCE_ROLE_CLINET) {
            strcat((char *)response->response_buf,"Client role.\r\n");
        } else if (param->role == BT_AWS_MCE_ROLE_PARTNER) {
            strcat((char *)response->response_buf,"Partner role.\r\n");
        }

        response->response_len = strlen((char *)response->response_buf);
        atci_send_response(response);
        vPortFree(response);
    }

    return ret;
}

static bool _proc_le_association_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    app_speaker_mode_local_context_type_t *local_context = (app_speaker_mode_local_context_type_t *)(self->local_context);
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    bt_aws_mce_srv_mode_t mode = bt_aws_mce_srv_get_mode();

    switch (event_id) {
    case LE_ASSOCIATION_EVENT_ASSOCIATION_DONE:
        APPS_LOG_MSGID_I("app_speaker_mode_activity: association done", 0);
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_ASSOCIATION, LE_ASSOCIATION_EVENT_TIMER_ASSOCIATION);
        local_context->status = SPEAKER_STATUS_ASSOCIATION_DONE;
        break;
    case LE_ASSOCIATION_EVENT_SCAN_TIMEOUT:
        APPS_LOG_MSGID_I("app_speaker_mode_activity: scan timeout", 0);
        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        local_context->status = SPEAKER_STATUS_FAIL;
        bt_aws_mce_le_association_app_switch_to_single();
        local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
        local_context->target_role = BT_AWS_MCE_ROLE_NONE;
        break;
    case LE_ASSOCIATION_EVENT_ADV_TIMEOUT:
        APPS_LOG_MSGID_I("app_speaker_mode_activity: adv timeout", 0);
        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        if (role != BT_AWS_MCE_ROLE_AGENT || mode != BT_AWS_MCE_SRV_MODE_BROADCAST) {
            local_context->status = SPEAKER_STATUS_FAIL;
            bt_aws_mce_le_association_app_switch_to_single();
            local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
            local_context->target_role = BT_AWS_MCE_ROLE_NONE;
        } else {
            local_context->status = SPEAKER_STATUS_NONE;
        }
        break;
    case LE_ASSOCIATION_EVENT_TIMER_ASSOCIATION:
        APPS_LOG_MSGID_I("app_speaker_mode_activity: association timeout", 0);
        if (local_context->target_role != BT_AWS_MCE_ROLE_AGENT) {
            apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            if (role != BT_AWS_MCE_ROLE_AGENT || mode != BT_AWS_MCE_SRV_MODE_BROADCAST) {
                local_context->status = SPEAKER_STATUS_FAIL;
                bt_aws_mce_le_association_app_switch_to_single();
                local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
                local_context->target_role = BT_AWS_MCE_ROLE_NONE;
            } else {
                local_context->status = SPEAKER_STATUS_NONE;
            }
        }
        break;
/*
    case LE_ASSOCIATION_EVENT_TIMER_AWS_CONNECT://client
        if(local_context->status == SPEAKER_STATUS_PAIRING) {
            APPS_LOG_MSGID_I("app_speaker_mode_activity: client aws connect timeout", 0);
            apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            local_context->status = SPEAKER_STATUS_FAIL;
            bt_aws_mce_le_association_app_switch_to_single();

            local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
            local_context->target_role = BT_AWS_MCE_ROLE_NONE;
        }
        break;
*/
    default:
        break;
    }

    return ret;
}

static bool _proc_bt_cm_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    app_speaker_mode_local_context_type_t *local_context = (app_speaker_mode_local_context_type_t *)(self->local_context);

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == local_context || NULL == remote_update) {
                APPS_LOG_MSGID_E("app_speaker_mode_activity, BT_CM_EVENT_REMOTE_INFO_UPDATE param null", 0);
                break;
            }

            if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state) {
                if (bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, NULL, 0) == 0
                        && local_context->status == SPEAKER_STATUS_DISCONNECTING) {
                    APPS_LOG_MSGID_I("app_speaker_mode_activity, all link disconnect, start ass, target mode:0x%2x, target role:0x%2x", 2, local_context->target_mode, local_context->target_role);
                    local_context->status = SPEAKER_STATUS_PAIRING;
                    bt_aws_mce_le_association_set_app_paramters(local_context->target_role, local_context->target_mode);
                    if ((local_context->target_role == BT_AWS_MCE_ROLE_PARTNER) || (local_context->target_role == BT_AWS_MCE_ROLE_CLINET)){
                        bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_START_SCAN, NULL);
                    } else {
                        bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_START_ADV, NULL);
                    }
                }
            } else if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                if (local_context->status == SPEAKER_STATUS_MODE_SWITCHED) {
                    /* Only partner & client.*/
                    APPS_LOG_MSGID_I("app_speaker_mode_activity, AWS connect", 0);
                    apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    local_context->status = SPEAKER_STATUS_NONE;
                }
            }

            break;
        }
        default:
            break;
    }

    return ret;
}

static bool _proc_aws_data_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    app_speaker_mode_local_context_type_t *local_context = (app_speaker_mode_local_context_type_t *)(self->local_context);

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t event_id;
        apps_aws_sync_event_decode(aws_data_ind, &event_group, &event_id);
        switch (event_group)
        {
        case EVENT_GROUP_UI_SHELL_LE_ASSOCIATION:
            switch (event_id)
            {
            case LE_ASSOCIATION_EVENT_AWS_MCE_UNGROUP:
                APPS_LOG_MSGID_I("app_speaker_mode_activity: aws mce report ungroup", 0);
                local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
                local_context->target_role = BT_AWS_MCE_ROLE_NONE;
                if(bt_aws_mce_le_association_app_switch_to_single() == BT_STATUS_SUCCESS) {
                    APPS_LOG_MSGID_I("app_speaker_mode_activity: ungroup", 0);
                }
                break;
            default:
                break;
            }
            ret = true;
            break;
        default:
            break;
        }
    }
    return ret;
}

bool app_speaker_mode_idle_activity_proc(
            ui_shell_activity_t *self,
            uint32_t event_group,
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;

    APPS_LOG_MSGID_I("pre proc receive event_group : %d, id : %x", 2, event_group, event_id);

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_LE_ASSOCIATION:
            ret = _proc_le_association_event_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_KEY:
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_AWS:
            ret = _proc_aws_event_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = _proc_aws_data_group(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = _proc_bt_cm_group(self, event_id, extra_data, data_len);
#ifdef APPS_LINE_IN_SUPPORT
        case EVENT_GROUP_SWITCH_AUDIO_PATH:
            if (event_id == APPS_EVENTS_INTERACTION_SWITCH_AUDIO_PATH) {
                if((app_line_in_activity_get_current_audio_path() == APP_AUDIO_PATH_LINE_IN)
                        && (bt_aws_mce_srv_get_mode() != BT_AWS_MCE_SRV_MODE_SINGLE)
                        && (bt_aws_mce_srv_get_mode() != BT_AWS_MCE_SRV_MODE_NORMAL)){
                    APPS_LOG_MSGID_I("app_speaker_mode_activity: line in, switch to single", 0);
                    bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_UNGROUP, NULL);

                    app_speaker_mode_local_context_type_t *local_context = (app_speaker_mode_local_context_type_t *)(self->local_context);
                    local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
                    local_context->target_role = BT_AWS_MCE_ROLE_NONE;
                }
            }
            break;
#endif
#ifdef APPS_USB_AUDIO_SUPPORT
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            if (event_id == APPS_EVENTS_INTERACTION_USB_AUDIO_OP) {
                if(app_usb_audio_is_open() && (bt_aws_mce_srv_get_mode() != BT_AWS_MCE_SRV_MODE_SINGLE)
                    && (bt_aws_mce_srv_get_mode() != BT_AWS_MCE_SRV_MODE_NORMAL)){
                    APPS_LOG_MSGID_I("app_speaker_mode_activity: usb in, switch to single", 0);
                    bt_aws_mce_le_association_app_action(BT_AWS_MCE_LE_ASSOCIATION_APP_UNGROUP, NULL);

                    app_speaker_mode_local_context_type_t *local_context = (app_speaker_mode_local_context_type_t *)(self->local_context);
                    local_context->target_mode = BT_AWS_MCE_SRV_MODE_SINGLE;
                    local_context->target_role = BT_AWS_MCE_ROLE_NONE;
                }
            }
            break;
#endif
        default:
            break;
    }
    return ret;
}
