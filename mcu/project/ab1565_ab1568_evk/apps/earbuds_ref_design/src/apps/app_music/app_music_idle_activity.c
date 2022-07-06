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
 * File: app_music_idle_activity.c
 *
 * Description:
 * This file is the activity to handle the key action or sink service event when music is not playing.
 * When the music starts playing, This activity will start the app_music_activity. In addition, this
 * activity manage the partner connection status, music mixing mode, and data synchronization during RHO.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "FreeRTOS.h"
#include "app_music_idle_activity.h"
#include "app_music_utils.h"
#include "app_music_activity.h"
#include "apps_events_interaction_event.h"
#include "bt_sink_srv_ami.h"
#include <stdlib.h>
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "bt_sink_srv.h"

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
#include "nvkey_dspalg.h"
#include "apps_aws_sync_event.h"
#include "bt_device_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "race_cmd.h"
extern void race_mmi_set_peq_group_id(uint8_t peq_group_id, uint8_t *status, am_feature_type_t audio_path_id);
#endif

#include "apps_config_key_remapper.h"
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_utils.h"
#endif
#include "bt_role_handover.h"

static apps_music_local_context_t s_app_music_context;  /* The variable records context */

static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len);
static bool _proc_key_event_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len);

#ifdef MTK_IN_EAR_FEATURE_ENABLE

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE

typedef struct
{
    uint8_t flag;
    bt_bd_addr_t actived_addr;
} app_music_rho_data_t;

static uint8_t _role_handover_service_get_data_len_callback(const bt_bd_addr_t *addr)
{
    uint8_t data_len = 0;
#ifdef AIR_MULTI_POINT_ENABLE
    if (NULL == addr)
#endif
    {
        data_len = sizeof(app_music_rho_data_t);
        APPS_LOG_MSGID_I(APP_MUSIC_TAG" [RHO] get_data_len_callback: %d", 1, data_len);

    }
    return data_len;
}

static bt_status_t _role_handover_get_data_callback(const bt_bd_addr_t *addr, void *data)
{
#ifdef AIR_MULTI_POINT_ENABLE
    if (NULL == addr)
#endif
    {
        app_music_rho_data_t rho_data = {};
        //uint8_t flag_data = 0;
        if (s_app_music_context.isAutoPaused) {
            rho_data.flag = rho_data.flag | 0x1;
        }

        if (s_app_music_context.music_playing) {
            rho_data.flag = rho_data.flag | 0x2;
        }
        memcpy(rho_data.actived_addr, s_app_music_context.activate_con_addr, sizeof(bt_bd_addr_t));
        memcpy(data, &rho_data, sizeof(app_music_rho_data_t));
        APPS_LOG_MSGID_I(APP_MUSIC_TAG" [RHO] get_data_callback isAutoPaused: %d", 1, s_app_music_context.isAutoPaused);
    }
    return BT_STATUS_SUCCESS;
}

static bt_status_t _role_handover_update_data_callback(bt_role_handover_update_info_t *info)
{
    APPS_LOG_MSGID_I(APP_MUSIC_TAG" [RHO] update_data_callback, recv role=0x%02X", 1, info->role);
    /* When RHO start, the data of the agent needs to be synchronized to the partner. */
#ifdef AIR_MULTI_POINT_ENABLE
    if (NULL == info->addr)
#endif
    {
        if (info->role == BT_AWS_MCE_ROLE_PARTNER) {
            app_music_rho_data_t data = *(app_music_rho_data_t *)info->data;
            bool isAutoPaused = (data.flag & 0x1) == 0x1 ? true : false;
            bool music_playing = (data.flag & 0x2) == 0x2 ? true : false;
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" [RHO] update_data_callback, recv role=0x%02X, isAutoPaused=%d, isPlaying=%d",
                    3, info->role, isAutoPaused, s_app_music_context.music_playing);
            if (s_app_music_context.isAutoPaused != isAutoPaused) {
                APPS_LOG_MSGID_I(APP_MUSIC_TAG" [RHO] update_data_callback rho status, isAutoPaused: old=%d, new=%d",
                        2, s_app_music_context.isAutoPaused, isAutoPaused);
                s_app_music_context.isAutoPaused = isAutoPaused;
            }
            if (s_app_music_context.music_playing != music_playing) {
                APPS_LOG_MSGID_I(APP_MUSIC_TAG" [RHO] update_data_callback rho status, isPlaying: old=%d, new=%d",
                        2, s_app_music_context.music_playing, music_playing);
                s_app_music_context.music_playing = music_playing;
            }
            if (0 != memcmp(s_app_music_context.activate_con_addr, data.actived_addr, sizeof(bt_bd_addr_t))) {
                memcpy(s_app_music_context.activate_con_addr, data.actived_addr, sizeof(bt_bd_addr_t));
            }
        }
    }

    return BT_STATUS_SUCCESS;
}

static void _role_handover_service_status_callback(
    const bt_bd_addr_t *addr,
    bt_aws_mce_role_t role,
    bt_role_handover_event_t event,
    bt_status_t status)
{
    if (BT_ROLE_HANDOVER_COMPLETE_IND == event) {
        APPS_LOG_MSGID_I(APP_MUSIC_TAG" [RHO] service_status_callback: status=%d, role=0x%x",
                2, status, role);
        if (BT_STATUS_SUCCESS == status) {
            /* allways set partner connecting flag to true while rho success. */
            s_app_music_context.isPartnerConnected = true;
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            if ( role == BT_AWS_MCE_ROLE_AGENT) {
                am_dynamic_change_channel(AUDIO_CHANNEL_SELECTION_STEREO);
                s_app_music_context.currMixState = MUSIC_STEREO;
            }
#endif
        }
    }
}

#endif
#endif /* MTK_IN_EAR_FEATURE_ENABLE */

static bool _proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" create current activity : 0x%x", 1, (uint32_t)self);
            self->local_context = &s_app_music_context;

            ((apps_music_local_context_t *)self->local_context)->music_playing = false;
#ifdef MTK_AWS_MCE_ENABLE
            ((apps_music_local_context_t *)self->local_context)->isPartnerCharging = false;
            ((apps_music_local_context_t *)self->local_context)->isPartnerConnected = false;
            ((apps_music_local_context_t *)self->local_context)->isBoothInEar = false;
            ((apps_music_local_context_t *)self->local_context)->currMixState = MUSIC_MONO;
            am_dynamic_change_channel(AUDIO_CHANNEL_SELECTION_MONO);
#endif

#ifdef MTK_IN_EAR_FEATURE_ENABLE
            ((apps_music_local_context_t *)self->local_context)->isAutoPaused = false;
#ifdef AIR_APP_MUSIC_DEBUG_ENABLE
            app_music_in_ear_atci_debug_init();
#endif
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            bt_role_handover_callbacks_t role_callbacks = {
                NULL,
                _role_handover_service_get_data_len_callback,
                _role_handover_get_data_callback,
                _role_handover_update_data_callback,
                _role_handover_service_status_callback
            };
            bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_MUSIC, &role_callbacks);
#endif
#endif /* MTK_IN_EAR_FEATURE_ENABLE */
            ((apps_music_local_context_t *)self->local_context)->avrcp_op_sta = AVRCP_OPERATION_STA_IDLE;
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

/**
 * @brief Do PEQ swtich operation
 */
static void app_music_idle_activity_handle_peq_switch()
{
#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)

    apps_config_set_vp(VP_INDEX_DOUBLE, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);

#ifdef MTK_AWS_MCE_ENABLE
    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
        bt_status_t status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_KEY, KEY_AUDIO_PEQ_SWITCH, NULL, 0);
        if (BT_STATUS_SUCCESS != status) {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" app_music_idle_activity_handle_peq_switch, Partner send KEY_AUDIO_PEQ_SWITCH aws to agent failed", 0);
            apps_config_set_vp(VP_INDEX_FAILED, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        } else {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" app_music_idle_activity_handle_peq_switch, Partner send KEY_AUDIO_PEQ_SWITCH aws to agent success", 0);
        }
        return;
    }
#endif /* MTK_AWS_MCE_ENABLE */

    uint8_t status = aud_peq_get_peq_status(PEQ_AUDIO_PATH_A2DP, 0);
    APPS_LOG_MSGID_I(APP_MUSIC_TAG" app_music_idle_activity_handle_peq_switch, get peq status result : %d", 1, status);

    if (status == 0) {
        apps_config_set_vp(VP_INDEX_FAILED, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        return;
    }

    uint8_t current_peq_id = aud_peq_get_current_sound_mode(PEQ_AUDIO_PATH_A2DP, 0);
    int32_t total_peq_num = aud_peq_get_total_mode(PEQ_AUDIO_PATH_A2DP, 0);
    APPS_LOG_MSGID_I(APP_MUSIC_TAG" app_music_idle_activity_handle_peq_switch, total peq num : %d, current peq ID : %d",
                                2, total_peq_num, current_peq_id);

    if (current_peq_id > total_peq_num) {
        apps_config_set_vp(VP_INDEX_FAILED, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        return;
    }

    uint8_t next_peq_id = 0;
    if (current_peq_id < total_peq_num) {
        next_peq_id = (current_peq_id + 1);
    } else if (current_peq_id == total_peq_num) {
        next_peq_id = 1;
    }

    race_mmi_set_peq_group_id(next_peq_id, &status, AM_A2DP_PEQ);

    APPS_LOG_MSGID_I(APP_MUSIC_TAG" app_music_idle_activity_handle_peq_switch, configure next peq ID : %d, result : %d (SUCCESS on 0)",
                            2, next_peq_id, status);

    if (status == RACE_ERRCODE_SUCCESS) {
        apps_config_set_vp(VP_INDEX_SUCCESSED, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
    } else {
        apps_config_set_vp(VP_INDEX_FAILED, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
    }
#endif /* PEQ_ENABLE || LINE_IN_PEQ_ENABLE */
}

static bool _proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;
    apps_config_key_action_t action = KEY_ACTION_INVALID;
    apps_music_local_context_t *local_ctx = (apps_music_local_context_t *)self->local_context;

    if (*(uint16_t *)extra_data == KEY_AUDIO_PEQ_SWITCH) {
        /**
         * @brief For PEQ switch operation
         */
        app_music_idle_activity_handle_peq_switch();
        return true;
    }

    if (local_ctx) {
        action = app_music_utils_proc_key_events(self, event_id, extra_data, data_len);
    }

    if (action != KEY_ACTION_INVALID) {
        if (action == KEY_AVRCP_PLAY) {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG "music_playing=0x%x", 1, local_ctx->music_playing);
        }
        ret = true;
    }

    return ret;
}

bool app_music_idle_activity_proc(
    ui_shell_activity_t *self,
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
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* BT_SINK events, indicates the state of music. */
            ret = app_music_idle_proc_bt_sink_events(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            /* The event come from bt connection manager. */
            ret = app_music_idle_proc_bt_cm_events(self, event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            /* Interaction events. */
            ret = app_music_idle_proc_apps_internal_events(self, event_id, extra_data, data_len);
            break;
#if defined(MTK_AWS_MCE_ENABLE)
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {

#if defined(MTK_PEQ_ENABLE) || defined(MTK_LINEIN_PEQ_ENABLE)
            /**
             * @brief Handle the PEQ switch key event from partner.
             */
            bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

            if ((aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) && (role == BT_AWS_MCE_ROLE_AGENT)) {
                //uint32_t event_group;
                uint32_t action;
                void *p_extra_data = NULL;
                uint32_t extra_data_len = 0;

                apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action, &p_extra_data, &extra_data_len);

                if (action == KEY_AUDIO_PEQ_SWITCH) {
                    app_music_idle_activity_handle_peq_switch();
                    return true;
                }
            }
#endif

            /* The event come from peer. */
            ret = app_music_idle_proc_aws_data_events(self, event_id, extra_data, data_len);
        }
            break;
#endif
        default:
            break;
    }
    return ret;
}

#ifdef AIR_APP_MUSIC_DEBUG_ENABLE
#ifdef MTK_IN_EAR_FEATURE_ENABLE
static atci_status_t _music_in_ear_atci_update_cfg(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *param = NULL;

    APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] _music_in_ear_atci_update_cfg mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_EXECUTION:
        param = parse_cmd->string_ptr + parse_cmd->name_len + 1;
        param = strtok(param, ",\n\r");
        if(0 == memcmp("0",param,1))
        {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] set music_in_ear_cfg into APP_MUSIC_IN_EAR_NONE.", 0);
            app_music_set_in_ear_control(APP_MUSIC_IN_EAR_NONE, true);
        }
        if(0 == memcmp("1",param,1))
        {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] set music_in_ear_cfg into APP_MUSIC_IN_EAR_DISABLE.", 0);
            app_music_set_in_ear_control(APP_MUSIC_IN_EAR_DISABLE, true);
        }
        if(0 == memcmp("2",param,1))
        {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] set music_in_ear_cfg into APP_MUSIC_IN_EAR_ONLY_AUTO_PAUSE.", 0);
            app_music_set_in_ear_control(APP_MUSIC_IN_EAR_ONLY_AUTO_PAUSE, true);
        }
        if(0 == memcmp("3",param,1))
        {
            APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] set music_in_ear_cfg into APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME.", 0);
            app_music_set_in_ear_control(APP_MUSIC_IN_EAR_AUTO_PAUSE_RESUME, true);
        }
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        break;
    default:
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t _music_volume_up_set(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    uint32_t value = 0;
    bool ret = FALSE;
    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_EXECUTION: {
        value = atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
        APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] _music_volume_up_set mode = %d, value=%d", 2, parse_cmd->mode, value);
        ret = app_music_set_volume_value(true, value);
        if (ret == TRUE) {
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else {
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }
    default:
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t _music_volume_down_set(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    uint32_t value = 0;
    bool ret = FALSE;
    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_EXECUTION: {
        value= atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
        APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] _music_volume_up_set mode = %d, value=%d", 2, parse_cmd->mode, value);
        ret = app_music_set_volume_value(false, value);
        if (ret == TRUE) {
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else {
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
        break;
    }
    default:
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_cmd_hdlr_item_t app_music_in_ear_atci_cmd_debug[] = {
    {
        .command_head = "AT+MUSIC_IN_EAR_CFG",    /**< MUSIC_IN_EAR_CFG debug. */
        .command_hdlr = _music_in_ear_atci_update_cfg,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+MUSIC_VOLUME_UP",    /**< MUSIC_VOLUME_UP. */
        .command_hdlr = _music_volume_up_set,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+MUSIC_VOLUME_DOWN",    /**< MUSIC_VOLUME_DOWN. */
        .command_hdlr = _music_volume_down_set,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

void app_music_in_ear_atci_debug_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(app_music_in_ear_atci_cmd_debug, sizeof(app_music_in_ear_atci_cmd_debug) / sizeof(atci_cmd_hdlr_item_t));
    if (ret == ATCI_STATUS_OK) {
        APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] atci_register_handler register success ret = %d", 1, ret);
    } else {
        APPS_LOG_MSGID_I(APP_MUSIC_TAG" [MUSIC ATCI] atci_register_handler  register fail ret = %d", 1, ret);
    }
}
#endif
#endif
