
/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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
 * File: app_ms_cfu_idle_activity.c
 *
 * Description:
 * This file is ms_cfu idle activity. This activity is used for cfu status management
 *
 */


#ifdef AIR_CFU_ENABLE

#include "apps_debug.h"
#include "app_ms_cfu_idle_activity.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_config_event_list.h"
#include "apps_events_key_event.h"
#include "app_home_screen_idle_activity.h"
#include "app_fota_idle_activity.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "apps_aws_sync_event.h"
#endif
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"
#endif
#include "atci.h"

#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "bt_callback_manager.h"
#include "bt_sink_srv.h"
#include "bt_gap_le.h"
#include "bt_type.h"
#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "app_le_audio_aird_client.h"
#include "bt_sink_srv_le.h"
#include "bt_sink_srv_le_cap.h"
#endif

#include "cfu.h"

#define TAG "[CFU] idle_activity "

/* Init firmware version and product info.*/
#define  CFU_FW_VESION          (0x01000100)        /* 0.1.0.0.0.1.0.0 (initial version). */
#define  CFU_PRODUCT_INFO       (0x00000200)        /* 02 is headset component id.*/


typedef struct {
    bool                 speaker_mute;   /**<  Record if the speaker is muted. */
    bt_sink_srv_state_t  curr_state;     /**<  Record the current sink_state. */
    bt_bd_addr_t         dongle_addr;    /**<  Record address of dongle. */
} app_ms_cfu_context_t;

static app_ms_cfu_context_t s_app_ms_cfu_context = {0};

static void app_ms_cfu_handle_music(bool is_pause, bool is_mute)
{
    APPS_LOG_MSGID_I(TAG" app_ms_cfu_handle_music: is_pause=%d, is_mute=%d", 2, is_pause, is_mute);
    uint16_t  temp_key_action = KEY_ACTION_INVALID;
    if(is_pause) {
        temp_key_action = KEY_AVRCP_PAUSE;
    }
    uint16_t *key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
    if (key_action != NULL) {
        memcpy(key_action, &temp_key_action, sizeof(uint16_t));
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_KEY,
                INVALID_KEY_EVENT_ID, key_action, sizeof(uint16_t), NULL, 0);

        /* Mute or unmute mic.*/
        bt_status_t bt_status = bt_sink_srv_set_mute(BT_SINK_SRV_MUTE_SPEAKER, is_mute);
        if (BT_STATUS_SUCCESS == bt_status) {
            s_app_ms_cfu_context.speaker_mute = is_mute;
        /* Sync mute or unmute operation to peer when aws mce enable.*/
#ifdef MTK_AWS_MCE_ENABLE
            if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
                bt_status_t send_state = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_SYNC_SPEAKER_MUTE_STATUS, &is_mute, sizeof(bool));
                if (BT_STATUS_SUCCESS == send_state) {
                    APPS_LOG_MSGID_I(TAG" [MUTE] sync speaker to peer success.", 0);
                } else if (BT_STATUS_FAIL == send_state) {
                    APPS_LOG_MSGID_I(TAG" [MUTE] sync speaker to peer fail.", 0);
                }
            }
#endif
        }
    }
}

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
static app_power_saving_target_mode_t app_cfu_get_power_saving_target_mode(void)
{
    if (cfu_is_running()) {
        return APP_POWER_SAVING_TARGET_MODE_NORMAL;
    } else {
        return APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
    }
}
#endif

/**
 * @brief      The CFU event function is callback from CFU middleware, need to send event and run in UI Shell task.
 * @param[in]  event, CFU event types.
 * @param[in]  param, event parameter.
 */
static void app_ms_cfu_event_callback(cfu_event_type_enum event, void *param)
{
    ui_shell_status_t ret = UI_SHELL_STATUS_OK;
    switch (event) {
        case CFU_EVENT_TYPE_NEED_REBOOT: {
            uint32_t delay = (uint32_t)param;
            APPS_LOG_MSGID_I(TAG" app_ms_cfu_event_callback - reboot", 0);
            ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                      EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                      APPS_EVENTS_INTERACTION_REQUEST_REBOOT,
                                      NULL, 0, NULL, delay);
            break;
        }

        case CFU_EVENT_TYPE_OTA_START: {
            APPS_LOG_MSGID_I(TAG" app_ms_cfu_event_callback - start ota.", 0);
            if (cfu_get_component_id() != CFU_COMPONENT_ID_DONGLE) {
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                app_power_save_utils_notify_mode_changed(false, app_cfu_get_power_saving_target_mode);
#endif
#ifdef MTK_PORT_SERVICE_BT_ENABLE
                extern bt_status_t ble_air_link_performace_optimization(void);
                ble_air_link_performace_optimization();
#endif
#ifdef AIR_CFU_ACTIVE_MODE
                /* Get current streaming and call status.*/
                if (s_app_ms_cfu_context.curr_state == BT_SINK_SRV_STATE_STREAMING) {
                    app_ms_cfu_handle_music(true, true);
                }
#endif
#ifdef MTK_AWS_MCE_ENABLE
                app_fota_bt_exit_sniff_mode();
#endif
            }
            break;
        }
        case CFU_EVENT_TYPE_OTA_END: {
            APPS_LOG_MSGID_I(TAG" app_ms_cfu_event_callback - end ota.", 0);
            if (cfu_get_component_id() != CFU_COMPONENT_ID_DONGLE) {
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                app_power_save_utils_notify_mode_changed(false, app_cfu_get_power_saving_target_mode);
#endif
#ifdef MTK_PORT_SERVICE_BT_ENABLE
                extern bt_status_t ble_air_link_performace_optimization_revert(void);
                ble_air_link_performace_optimization_revert();
#endif
#ifdef AIR_CFU_ACTIVE_MODE
                if (s_app_ms_cfu_context.speaker_mute
                        && s_app_ms_cfu_context.curr_state == BT_SINK_SRV_STATE_STREAMING) {
                    app_ms_cfu_handle_music(false, false);
                }
#endif
#ifdef MTK_AWS_MCE_ENABLE
                app_fota_bt_switch_sniff_mode(true);
#endif
            }
            break;
        }

        default:
            break;
    }
    APPS_LOG_MSGID_I(TAG" app_ms_cfu_event_callback: event=%d ret=%d", 2, event, ret);
}

#ifdef AIR_LE_AUDIO_ENABLE
static void app_ms_cfu_le_audio_aird_client_callback(app_le_audio_aird_client_event_t event, void *buff)
{
    if (buff == NULL) {
        APPS_LOG_MSGID_I(TAG" le_audio_aird_client_callback buff is NULL.", 0);
        return;
    }

    app_le_audio_aird_client_event_srv_discovery_complete_t *evt =
            (app_le_audio_aird_client_event_srv_discovery_complete_t *)buff;
    APPS_LOG_MSGID_I(TAG" le_audio_aird_client_callback: event=%d, event_handle=0x%x, event_status=0x%x.",
            3, event, evt->handle, evt->status);
    switch (event) {
        case APP_LE_AUDIO_AIRD_CLIENT_EVENT_SRV_DISCOVERY_COMPLETE: {
            if (evt->status == BT_STATUS_SUCCESS) {
                APPS_LOG_MSGID_I(TAG" le_audio_callback_evt: LE-CONNECT to dongle.", 0);
#ifdef MTK_AWS_MCE_ENABLE
                if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role())
#endif
                {
                    cfu_set_sub_component_connection_state(true);
                    bt_addr_t dev_addr = bt_sink_srv_cap_get_peer_bdaddr(bt_sink_srv_cap_get_link_index(evt->handle));
                    memcpy(s_app_ms_cfu_context.dongle_addr, dev_addr.addr, sizeof(bt_bd_addr_t));
                }
            }
            break;
        }
    }

}
#endif

/**
 * @brief      The XiaoAI APP init function, this function will be called when BT power on.
 */
static void app_ms_cfu_init()
{
    APPS_LOG_MSGID_I(TAG" init cfu", 0);

    cfu_init_para_t init_para = {0};

    /* Important reminder! Customer need to modify these parameters. */
    init_para.component_id         = CFU_COMPONENT_ID_HEADSET;    /* Customer configure component id. */
    init_para.firmware_version     = CFU_FW_VESION;               /* Customer configure option: 1.0.0.0.0.1.0.0 (initial version). */
    init_para.product_info         = CFU_PRODUCT_INFO;            /* Customer configure product information. */
    init_para.event_fun            = app_ms_cfu_event_callback;
#ifdef AIR_LE_AUDIO_ENABLE
    app_le_audio_aird_client_register_callback(app_ms_cfu_le_audio_aird_client_callback);
#endif
    cfu_init(&init_para);
}


static bool _proc_ui_shell_group(struct _ui_shell_activity *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(TAG" create", 0);
            memset(&s_app_ms_cfu_context, 0, sizeof(app_ms_cfu_context_t));

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
            app_power_save_utils_register_get_mode_callback(app_cfu_get_power_saving_target_mode);
#endif
            app_ms_cfu_init();
#ifdef AIR_APP_CFU_ATCI_ENABLE
            app_cfu_atci_debug_init();
#endif
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(TAG" destroy", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool app_ms_cfu_bt_sink_event_proc(ui_shell_activity_t *self,
                                uint32_t event_group,
                                uint32_t event_id,
                                void *extra_data,
                                size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
            bool device_is_busy = false;
            if (param == NULL) {
                APPS_LOG_MSGID_I(TAG", bt_sink_state_change :param is null ", 0);
                return ret;
            }
            s_app_ms_cfu_context.curr_state = param->current;
            APPS_LOG_MSGID_I(TAG", bt_sink_state_change: param_previous: %x, param_now: %x", 2, param->previous, param->current);
            /* Check the call status, if there are no HFP related states, finish current activity. */
            if ((param->current < BT_SINK_SRV_STATE_STREAMING) && (param->previous >= BT_SINK_SRV_STATE_STREAMING)) {
                /* Busy is end */
                device_is_busy = false;
            } else {
                /*Device is busy*/
#ifndef AIR_CFU_ACTIVE_MODE
                if (param->current >= BT_SINK_SRV_STATE_STREAMING)
#else
                if (param->current > BT_SINK_SRV_STATE_STREAMING)
#endif
                {
                    device_is_busy = true;
                } else if (param->current == BT_SINK_SRV_STATE_STREAMING) {
#ifdef AIR_CFU_ACTIVE_MODE
                    if (cfu_is_running()) {
#ifdef MTK_AWS_MCE_ENABLE
                        if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role())
#endif
                        {
                            app_ms_cfu_handle_music(true, true);
                        }
                    } else {
                        if (s_app_ms_cfu_context.speaker_mute) {
                            app_ms_cfu_handle_music(false, false);
                        }
                    }
#endif
                }
            }
            APPS_LOG_MSGID_I(TAG", bt_sink_state_change: device_is_busy=%d", 1, device_is_busy);
            /* Set device state. */
            cfu_set_device_state(device_is_busy);
            break;
        }
        case LE_SINK_SRV_EVENT_REMOTE_INFO_UPDATE : {
#ifdef AIR_LE_AUDIO_CIS_ENABLE
            bt_le_sink_srv_event_remote_info_update_t *update_ind = (bt_le_sink_srv_event_remote_info_update_t *)extra_data;
            if (update_ind == NULL) {
                break;
            }

            APPS_LOG_MSGID_I(TAG" LEA_BT_SINK, Link=%d->%d srv=%d->%d",
                             4, update_ind->pre_state, update_ind->state,
                             update_ind->pre_connected_service, update_ind->connected_service);
            if (update_ind->pre_state == BT_BLE_LINK_CONNECTED
                    && update_ind->state == BT_BLE_LINK_DISCONNECTED) {
#ifdef MTK_AWS_MCE_ENABLE
                if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT)
#endif
                {
                    if (0 == memcmp(s_app_ms_cfu_context.dongle_addr, update_ind->address, sizeof(bt_bd_addr_t))) {
                        APPS_LOG_MSGID_I(TAG" le_audio_callback_evt: LE-DISCONNECT to dongle.", 0);
                        cfu_set_sub_component_connection_state(false);
                    }
                }
            }
#endif
            break;
        }
        default: {
            break;
        }
    }
    return ret;
}


static bool app_ms_cfu_bt_cm_event_proc(struct _ui_shell_activity *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bt_cm_remote_info_update_ind_t *info = (bt_cm_remote_info_update_ind_t *)extra_data;
    if (event_id == BT_CM_EVENT_REMOTE_INFO_UPDATE) {
        if (!(info->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL))
                && (info->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL))){
            APPS_LOG_MSGID_I(TAG" sub-component connected.", 0);
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                cfu_set_sub_component_connection_state(true);
            }
#else
            cfu_set_sub_component_connection_state(true);
#endif
        } else if ((info->pre_connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL))
                && (!(info->connected_service & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL)))) {
            APPS_LOG_MSGID_I(TAG" sub-component disconnected.", 0);
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                cfu_set_sub_component_connection_state(false);
            }
#else
            cfu_set_sub_component_connection_state(false);
#endif
        }
    }
    return false;
}

#if defined(MTK_AWS_MCE_ENABLE)
static bool app_ms_cfu_idle_proc_aws_data(
        ui_shell_activity_t *self,
        uint32_t event_id,
        void *extra_data,
        size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION ) {
        uint32_t event_group;
        uint32_t action;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &action, &p_extra_data, &extra_data_len);

        if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && action == APPS_EVENTS_INTERACTION_SYNC_SPEAKER_MUTE_STATUS) {
            bool mute = *(bool *)p_extra_data;
            s_app_ms_cfu_context.speaker_mute = mute;
            APPS_LOG_MSGID_I(TAG" [MUTE] received speaker mute_status=%d", 1, mute);
            bt_sink_srv_set_mute(BT_SINK_SRV_MUTE_SPEAKER, mute);
            ret = true;
        }
    }

    return ret;
}
#endif

/**
 * @brief The activity event handler
 *
 * @param self
 * @param event_group
 * @param event_id
 * @param extra_data
 * @param data_len
 */
bool app_ms_cfu_idle_activity_proc(struct _ui_shell_activity *self,
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
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            /* BT sink events, notify cfu middle. */
            ret = app_ms_cfu_bt_sink_event_proc(self, event_group, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            /* Bt connection manager event, indicates the power state of BT. */
            ret = app_ms_cfu_bt_cm_event_proc(self, event_id, extra_data, data_len);
            break;
        }
#if defined(MTK_AWS_MCE_ENABLE)
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            ret = app_ms_cfu_idle_proc_aws_data(self, event_id, extra_data, data_len);
            break;
        }
#endif
        default: {
            break;
        }
    }
    return ret;
}

#ifdef AIR_APP_CFU_ATCI_ENABLE
static atci_status_t _cfu_atci_debug(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    char *param = NULL;

    APPS_LOG_MSGID_I(TAG" [CFU ATCI] _mute_mic mode = %d", 1, parse_cmd->mode);
    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_EXECUTION:
        param = parse_cmd->string_ptr + parse_cmd->name_len + 1;
        param = strtok(param, ",\n\r");
        if(0 == memcmp("ota_start",param,strlen("ota_start")))
        {
            APPS_LOG_MSGID_I(TAG" [CFU ATCI] ota start.", 0);
#ifdef AIR_CFU_ACTIVE_MODE
            if (cfu_get_component_id() != CFU_COMPONENT_ID_DONGLE) {
                /* Get current streaming and call status.*/
                if (s_app_ms_cfu_context.curr_state == BT_SINK_SRV_STATE_STREAMING) {
                    app_ms_cfu_handle_music(true, true);
                }
            }
#endif
        } else if (0 == memcmp("ota_end",param,strlen("ota_end"))) {
            APPS_LOG_MSGID_I(TAG" [CFU ATCI] ota end.", 0);
#ifdef AIR_CFU_ACTIVE_MODE
            if (cfu_get_component_id() != CFU_COMPONENT_ID_DONGLE) {
                if (s_app_ms_cfu_context.speaker_mute
                        && s_app_ms_cfu_context.curr_state == BT_SINK_SRV_STATE_STREAMING) {
                    app_ms_cfu_handle_music(false, false);
                }
            }
#endif
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

static atci_cmd_hdlr_item_t app_cfu_atci_cmd_debug[] = {
    {
        .command_head = "AT+CFU",       /**< HFP mute mic debug. */
        .command_hdlr = _cfu_atci_debug,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
};

void app_cfu_atci_debug_init(void)
{
    atci_status_t ret;

    ret = atci_register_handler(app_cfu_atci_cmd_debug, sizeof(app_cfu_atci_cmd_debug) / sizeof(atci_cmd_hdlr_item_t));
    APPS_LOG_MSGID_I(TAG" [CFU ATCI] atci_register_handler register ret = %d", 1, ret);
}
#endif //AIR_APP_CFU_ATCI_ENABLE
#endif /* AIR_CFU_ENABLE */

