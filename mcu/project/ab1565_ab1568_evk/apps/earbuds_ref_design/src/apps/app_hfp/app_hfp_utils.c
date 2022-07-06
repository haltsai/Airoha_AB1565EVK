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
 * Description: this file provide common functions for hfp_app.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "app_hfp_utils.h"
#include "app_hfp_activity.h"
#include "apps_config_key_remapper.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_events_key_event.h"
#include "apps_aws_sync_event.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_ami.h"
#include "bt_aws_mce_srv.h"
#include "app_home_screen_idle_activity.h"
#include "app_rho_idle_activity.h"
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
#include "battery_management.h"
#include "battery_management_core.h"
#endif
#include "nvkey_id_list.h"
#include "nvkey.h"
#include "bt_sink_srv_hf.h"

#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_utils.h"
#endif
#include "bt_hci.h"
#include "app_bt_state_service.h"

#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_le_audio_util.h"
#include "app_le_audio_aird_client.h"
#include "bt_sink_srv_le_volume.h"
#endif

#ifdef MTK_IN_EAR_FEATURE_ENABLE
uint8_t g_app_hfp_auto_accept = APP_HFP_AUTO_ACCEPT_NONE;  /**<  Record the hfp auto accept incoming call config. */
#endif
app_hfp_incoming_call_vp_status_t  g_app_hfp_incoming_call_vp_status;

apps_config_state_t app_get_config_status_by_state(bt_sink_srv_state_t state)
{
    apps_config_state_t status = APP_TOTAL_STATE_NO;
    /*APPS_LOG_MSGID_I(APP_HFP_UTILS", hfp_state: %x", 1, state);*/

    switch (state) {
        case BT_SINK_SRV_STATE_INCOMING: {
            /* There is an incoming call. */
            status = APP_HFP_INCOMING;
            break;
        }
        case BT_SINK_SRV_STATE_OUTGOING: {
            /* There is an outgoing call. */
            status = APP_HFP_OUTGOING;
            break;
        }
        case BT_SINK_SRV_STATE_ACTIVE: {
            /* There is an active call only.*/
            status = APP_HFP_CALLACTIVE;
            break;
        }
        case BT_SINK_SRV_STATE_TWC_INCOMING: {
            /* There is an active call and a waiting incoming call. */
            status = APP_HFP_TWC_INCOMING;
            break;
        }
        case BT_SINK_SRV_STATE_TWC_OUTGOING: {
            /* There is a held call and a outgoing call. */
            status = APP_HFP_TWC_OUTGOING;
            break;
        }
        case BT_SINK_SRV_STATE_HELD_ACTIVE: {
            /* There is an active call and a held call. */
            status = APP_STATE_HELD_ACTIVE;
            break;
        }
        case BT_SINK_SRV_STATE_HELD_REMAINING: {
            /* There is a held call only. */
            status = APP_HFP_CALLACTIVE_WITHOUT_SCO;
            break;
        }
        case BT_SINK_SRV_STATE_MULTIPARTY: {
            /* There is a conference call. */
            status = APP_HFP_MULTITPART_CALL;
            break;
        }
    }
    /*APPS_LOG_MSGID_I(APP_HFP_UTILS", hfp_status: %x", 1, status);*/
    return status;
}

#ifdef MTK_AWS_MCE_ENABLE
void app_hfp_stop_vp_by_aws_role(bt_aws_mce_role_t aws_role)
{
    bt_aws_mce_role_t curr_role = BT_AWS_MCE_ROLE_NONE;
    curr_role = bt_device_manager_aws_local_info_get_role();

    app_hfp_set_incoming_call_vp_flag(APP_HFP_INCOMING_CALL_VP_STOP);
    app_hfp_set_incoming_call_vp_sync_flag(APP_HFP_INCOMING_CALL_VP_SYNC_SUCCESS);

    APPS_LOG_MSGID_I(APP_HFP_ACTI" stop vp by aws role: event_role=%x,curr_role=%x",
        2, aws_role, curr_role);

    if (aws_role == BT_AWS_MCE_ROLE_AGENT
        && curr_role == BT_AWS_MCE_ROLE_PARTNER) {
        /* AWS role change into partner when bt disconneced during playing incoming call vp.*/
        apps_config_stop_voice(false, 200, true);
        return;
    }
    apps_config_stop_voice(true, 200, true);
}
#endif

void app_hfp_stop_vp(void)
{
    APPS_LOG_MSGID_I(APP_HFP_ACTI" stop vp", 0);
    app_hfp_set_incoming_call_vp_flag(APP_HFP_INCOMING_CALL_VP_STOP);
    app_hfp_set_incoming_call_vp_sync_flag(APP_HFP_INCOMING_CALL_VP_SYNC_SUCCESS);
    apps_config_stop_voice(true, 200, true);
}

bool app_hfp_send_call_action(apps_config_key_action_t action)
{
    bt_sink_srv_action_t sink_action  = BT_SINK_SRV_ACTION_NONE;
    bool ret = true;

    APPS_LOG_MSGID_I(APP_HFP_UTILS", app_action : %x", 1, action);
#ifdef AIR_MCSYNC_SHARE_ENABLE
#ifdef MTK_AWS_MCE_ENABLE
    /* Share mode need to process the key event. */
    if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(APP_HFP_UTILS", set ret flag to false", 0);
        ret = false;
    }
#endif
#endif

    /* Map key action to sink service action. */
    switch (action) {
        case KEY_ACCEPT_CALL: {
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_AUTO_ACCEPT_INCOMING_CALL);
#endif
            sink_action = BT_SINK_SRV_ACTION_ANSWER;
            break;
        }
        case KEY_REJCALL: {
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_AUTO_ACCEPT_INCOMING_CALL);
#endif
            sink_action = BT_SINK_SRV_ACTION_REJECT;
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT)
#endif
            {
#ifdef MTK_AWS_MCE_ENABLE
                app_voice_prompt_set_voice_stopping(true);
#endif
                app_hfp_stop_vp();
                apps_config_set_vp(VP_INDEX_CALL_REJECTED, true, 200, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
#ifdef MTK_AWS_MCE_ENABLE
            else {
                bt_status_t ret = BT_SINK_SRV_STATUS_FAIL;
                uint32_t event_group = EVENT_GROUP_UI_SHELL_KEY;
                ret = apps_aws_sync_event_send(event_group, KEY_REJCALL);
                APPS_LOG_MSGID_I(APP_HFP_UTILS", send aws data to agent,ret=%x, event_group = %x", 2, ret, event_group);
            }
#endif
            break;
        }
        case KEY_ONHOLD_CALL: {
            sink_action = BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER;
            break;
        }
        case KEY_END_CALL: {
            sink_action = BT_SINK_SRV_ACTION_HANG_UP;
            break;
        }
        case KEY_VOICE_UP: {
#ifdef AIR_LE_AUDIO_ENABLE
            {
                app_le_audio_aird_client_info_t *p_info = NULL;
                bt_handle_t handle = BT_HANDLE_INVALID;

                if ((BT_HANDLE_INVALID != (handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established())) &&
                    (NULL != (p_info = app_le_audio_aird_client_get_info(handle))) &&
                    (APP_LE_AUDIO_AIRD_MODE_SUPPORT_HID_CALL == p_info->mode)) {

                    app_le_audio_aird_action_set_streaming_volume_t param;
                    bt_status_t status;

                    param.streaming_interface = APP_LE_AUDIO_AIRD_STREAMING_INTERFACE_SPEAKER;
                    param.streaming_port = APP_LE_AUDIO_AIRD_STREAMING_PORT_0;
                    param.channel = APP_LE_AUDIO_AIRD_CHANNEL_DUAL;
                    param.volume = 1;   /* The delta value. */

                    status = app_le_audio_aird_client_send_action(handle,
                                                                APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_UP,
                                                                &param,
                                                                sizeof(app_le_audio_aird_action_set_streaming_volume_t));

                    APPS_LOG_MSGID_I(APP_HFP_UTILS" [LEA][CIS] VOLUME UP, status:%x", 1, status);
                    ret = true;
                    break;
                } else {
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" [LEA][CIS] VOLUME UP failed, handle %x, p_info %x, p_info->mode %d", 3, handle, p_info, (p_info == NULL) ? NULL : p_info->mode);
                }
            }
#endif

            sink_action = BT_SINK_SRV_ACTION_CALL_VOLUME_UP;
            break;
        }
        case KEY_VOICE_DN: {
#ifdef AIR_LE_AUDIO_ENABLE
            {
                app_le_audio_aird_client_info_t *p_info = NULL;
                bt_handle_t handle = BT_HANDLE_INVALID;

                if ((BT_HANDLE_INVALID != (handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established())) &&
                    (NULL != (p_info = app_le_audio_aird_client_get_info(handle))) &&
                    (APP_LE_AUDIO_AIRD_MODE_SUPPORT_HID_CALL == p_info->mode)) {

                    app_le_audio_aird_action_set_streaming_volume_t param;
                    bt_status_t status;

                    param.streaming_interface = APP_LE_AUDIO_AIRD_STREAMING_INTERFACE_SPEAKER;
                    param.streaming_port = APP_LE_AUDIO_AIRD_STREAMING_PORT_0;
                    param.channel = APP_LE_AUDIO_AIRD_CHANNEL_DUAL;
                    param.volume = 1;   /* The delta value. */

                    status = app_le_audio_aird_client_send_action(handle,
                                                                APP_LE_AUDIO_AIRD_ACTION_SET_STREAMING_VOLUME_DOWN,
                                                                &param,
                                                                sizeof(app_le_audio_aird_action_set_streaming_volume_t));

                    APPS_LOG_MSGID_I(APP_HFP_UTILS" [LEA][CIS] VOLUME DOWN, status:%x", 1, status);
                    ret = true;
                    break;
                } else {
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" [LEA][CIS] VOLUME DOWN failed, handle %x, p_info %x, p_info->mode %d", 3, handle, p_info, (p_info == NULL) ? NULL : p_info->mode);
                }
            }
#endif

            sink_action = BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN;
            break;
        }
        case KEY_REJCALL_SECOND_PHONE:  {
            sink_action = BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD;
            break;
        }
        case KEY_SWITCH_AUDIO_PATH:
            sink_action = BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH;
            break;
        case KEY_3WAY_HOLD_ACTIVE_ACCEPT_OTHER: {
            sink_action = BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER;
            break;
        }
        default: {
            ret = false;
            break;
        }
    }

    APPS_LOG_MSGID_I(APP_HFP_UTILS", ret: %x, sink_action : %x", 2, ret, sink_action);

    if (BT_SINK_SRV_ACTION_NONE != sink_action) {
        bt_sink_srv_send_action(sink_action, NULL);
    }
    return ret;
}

bool app_hfp_update_led_bg_pattern(ui_shell_activity_t *self, bt_sink_srv_state_t now, bt_sink_srv_state_t pre)
{
    bool ret = true;
    APPS_LOG_MSGID_I(APP_HFP_UTILS", now=ox%x", 1, now);

    /* Update LED display by current MMI state. */
    switch (now) {
        case BT_SINK_SRV_STATE_INCOMING: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [SET_LED]: incoming", 0);
            apps_config_set_background_led_pattern(LED_INDEX_INCOMING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_OUTGOING: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [SET_LED]: outgoing", 0);
            apps_config_set_background_led_pattern(LED_INDEX_OUTGOING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_ACTIVE: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [SET_LED]: active", 0);
            apps_config_set_background_led_pattern(LED_INDEX_CALL_ACTIVE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_HELD_REMAINING: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [SET_LED]: HELD_REMAINING", 0);
            apps_config_set_background_led_pattern(LED_INDEX_HOLD_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_HELD_ACTIVE: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [SET_LED]: HELD_ACTIVE", 0);
            apps_config_set_background_led_pattern(LED_INDEX_CALL_ACTIVE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_TWC_INCOMING: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [SET_LED]: twc_incoming", 0);
            apps_config_set_background_led_pattern(LED_INDEX_INCOMING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_TWC_OUTGOING: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS"[SET_LED]: outgoing", 0);
            apps_config_set_background_led_pattern(LED_INDEX_OUTGOING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_MULTIPARTY: {
            /* There is a conference call. */
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [SET_LED]: multiparty", 0);
            apps_config_set_background_led_pattern(LED_INDEX_CALL_ACTIVE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        default: {
            ret = false;
            break;
        }
    }

    APPS_LOG_MSGID_I(APP_HFP_UTILS", update_led_bg_pattern: ret=%d", 1, ret);

    return ret;
}

void app_hfp_set_incoming_call_vp_flag(app_hfp_incoming_call_vp_play_status_t vp_play_status)
{
    APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND] set vp_play_status = %x",
            1, vp_play_status);
#ifdef MTK_AWS_MCE_ENABLE
        if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
            bt_status_t sync_vp_state = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_SYNC_INCOMING_CALL_VP, &vp_play_status, sizeof(app_hfp_incoming_call_vp_play_status_t));
            if (BT_STATUS_FAIL == sync_vp_state) {
                APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND] sync vp_play_status to peer fail!", 0);
            }
        }
#endif
    g_app_hfp_incoming_call_vp_status.vp_play_status = vp_play_status;
}

app_hfp_incoming_call_vp_play_status_t app_hfp_get_incoming_call_vp_flag()
{
    APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND] get_vp_flag: vp_play_status = %x",
            1, g_app_hfp_incoming_call_vp_status.vp_play_status);

    return g_app_hfp_incoming_call_vp_status.vp_play_status;
}

bool app_hfp_sync_incoming_call_vp_flag()
{
    bool ret = true;
#ifdef MTK_AWS_MCE_ENABLE
    app_hfp_incoming_call_vp_play_status_t vp_play_status = app_hfp_get_incoming_call_vp_flag();
    bt_status_t sync_ic_vp_state = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
            APPS_EVENTS_INTERACTION_SYNC_INCOMING_CALL_VP, &vp_play_status, sizeof(app_hfp_incoming_call_vp_play_status_t));
    APPS_LOG_MSGID_I(APP_HFP_ACTI" [RING_IND] sync incoming call vp flag state=0x%08X", 1, sync_ic_vp_state);
    if (BT_STATUS_FAIL == sync_ic_vp_state) {
        ret = false;
    }
#endif
    return ret;
}

void app_hfp_set_incoming_call_vp_sync_flag(app_hfp_incoming_call_vp_sync_status_t vp_sync_status)
{
    APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND] set_incoming_call_vp_sync_status=%x",
            1, vp_sync_status);
    g_app_hfp_incoming_call_vp_status.vp_sync_status = vp_sync_status;
}

app_hfp_incoming_call_vp_sync_status_t app_hfp_get_incoming_call_vp_sync_flag()
{
    APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND] get_incoming_call_vp_sync_status=%x",
            1, g_app_hfp_incoming_call_vp_status.vp_sync_status);

    return g_app_hfp_incoming_call_vp_status.vp_sync_status;
}

void app_hfp_incoming_call_vp_callback(uint32_t idx, vp_err_code err)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
#ifdef MTK_AWS_MCE_ENABLE
    /* Set incoming call vp flag and send incoming call delay event. */
    if (idx == VP_INDEX_INCOMING_CALL && err == VP_ERR_CODE_SYNC_FAIL) {
        APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND] incoming call vp sync fail.", 0);
        app_hfp_set_incoming_call_vp_flag(APP_HFP_INCOMING_CALL_VP_STOP);
        app_hfp_set_incoming_call_vp_sync_flag(APP_HFP_INCOMING_CALL_VP_SYNC_PLAY_FAIL);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                     APPS_EVENTS_INTERACTION_INCOMING_CALL_DELAY, NULL, 0,
                                                     NULL, 3000);
    }

    /*  Sync stop twc incoming call vp fail. */
    if ((idx == VP_INDEX_TWC_INCOMING_CALL || idx == VP_INDEX_INCOMING_CALL)
            && err ==VP_ERR_CODE_SYNC_STOP_FAIL )
    {
        APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND] twc/incoming call vp sync stop fail.", 0);
        /* Get current aws link state and connected/disconnected reason. */
        const app_bt_state_service_status_t * curr_state = app_bt_connection_service_get_current_status();
        if (!curr_state->aws_connected && curr_state->reason != BT_HCI_STATUS_SUCCESS) {
            uint16_t curr_vp_index = app_voice_prompt_get_current_index();
             if (VP_INDEX_TWC_INCOMING_CALL == curr_vp_index
                 || VP_INDEX_INCOMING_CALL == curr_vp_index) {
                 APPS_LOG_MSGID_I(APP_HFP_UTILS" [RING_IND] local stop twc/incoming call vp", 0);
                 apps_config_stop_voice(false, 200, true);
             }
        }

        app_hfp_set_incoming_call_vp_sync_flag(APP_HFP_INCOMING_CALL_VP_SYNC_STOP_FAIL);
    }
#endif
#endif
}

void app_hfp_report_battery_to_remote(int32_t bat_val, int32_t pre_val)
 {
      /* Need transfer, bt sink action level is 0~9. */
     bat_val = bat_val / 10;
     if (bat_val == 10) {
         /* bal_val is 100. */
         bat_val = 9;
     }

     pre_val = pre_val / 10;
     if (pre_val == 10) {
         /* bal_val is 100. */
         pre_val = 9;
     }

     /* Report current battery level to remote device, such as smartphone. */
     APPS_LOG_MSGID_I(APP_HFP_UTILS", bat_val : %d, pre_val : %d", 2, bat_val, pre_val);
     if (pre_val != bat_val) {
         bt_status_t status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_REPORT_BATTERY, &bat_val);
         APPS_LOG_MSGID_I(APP_HFP_UTILS", status : %x", 1, status);
     }
 }

#ifdef MTK_IN_EAR_FEATURE_ENABLE
uint8_t app_hfp_is_auto_accept_incoming_call()
{
    nvkey_status_t status = NVKEY_STATUS_OK;
    uint8_t isEnable = 0;
    uint32_t data_size = sizeof(uint8_t);
    if (APP_HFP_AUTO_ACCEPT_NONE != g_app_hfp_auto_accept) {
        APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] get in ear control state success: g_app_hfp_auto_accept=%d.", 1, g_app_hfp_auto_accept);
        return g_app_hfp_auto_accept;
    } else {
        status = nvkey_read_data(NVKEYID_APP_IN_EAR_HFP_ABILITY, &isEnable, &data_size);
        if (NVKEY_STATUS_OK == status) {
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] get in ear control state success: isEnable=%d.", 1, isEnable);
        } else if (NVKEY_STATUS_ITEM_NOT_FOUND == status) {
            APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] get in ear control state not found: %d.", 1, status);
            isEnable = APP_HFP_AUTO_ACCEPT_DISABLE;
            status = nvkey_write_data(NVKEYID_APP_IN_EAR_HFP_ABILITY, &isEnable, sizeof(uint8_t));
            if (NVKEY_STATUS_OK == status) {
                g_app_hfp_auto_accept = isEnable;
                APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] get in ear control state success: isEnable=%d.", 1, isEnable);
                return isEnable;
            } else {
                APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] set in ear control state failed again!", 0);
            }
        }
    }

    return isEnable;
}

bool app_hfp_set_auto_accept_incoming_call(uint8_t isAutoaccept)
{
    bool ret = false;
    nvkey_status_t status = NVKEY_STATUS_OK;
    status = nvkey_write_data(NVKEYID_APP_IN_EAR_HFP_ABILITY, &isAutoaccept, sizeof(uint8_t));
    if (status != NVKEY_STATUS_OK) {
        APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] set in ear control value failed: %d.", 1, status);
        return ret;
    } else {
        ret = true;
        g_app_hfp_auto_accept = isAutoaccept;
#ifdef MTK_AWS_MCE_ENABLE
        if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
            app_hfp_notify_state_to_peer();
        }
#endif
        APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] set in ear control value success: status=%d, isEnable=%d.", 2, status, isAutoaccept);
    }
    return ret;
}

void app_hfp_notify_state_to_peer()
{
    uint8_t isAutoaccept = app_hfp_is_auto_accept_incoming_call();
#ifdef MTK_AWS_MCE_ENABLE
        if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
            bt_status_t send_state = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_SYNC_AUTO_ACCEPT_STATUS, &isAutoaccept, sizeof(uint8_t));
            if (BT_STATUS_SUCCESS == send_state) {
                APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] sync auto accept status to peer success.", 0);
            } else if (BT_STATUS_FAIL == send_state) {
                APPS_LOG_MSGID_I(APP_HFP_UTILS" [AUTO ACCEPT] sync auto accept status to peer fail.", 0);
            }
        }
#endif
}

#endif

#if defined(AIR_MULTI_POINT_ENABLE) && defined(AIR_EMP_AUDIO_INTER_STYLE_ENABLE)
void app_hfp_emp_music_process(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    static bt_bd_addr_t device_in_music = {0};
    static bt_bd_addr_t empt_addr = {0};
    bt_status_t sta = BT_STATUS_SUCCESS;
    uint32_t conn_nums;
    bt_bd_addr_t conn_addrs[2];
    uint32_t idx = 0;
    uint8_t* p_t = NULL;
    if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
        bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *) extra_data;
        switch (param->current) {
            /* If music started, member the device's addr */
            case BT_SINK_SRV_STATE_STREAMING: {
                conn_nums = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), conn_addrs, 2);
                for (idx = 0; idx < conn_nums; idx++) {
                    bt_sink_srv_device_state_t dev_sta = {{0},0,0,0,0};
                    bt_sink_srv_get_device_state((const bt_bd_addr_t*)&conn_addrs[idx], &dev_sta, 1);
                    if (dev_sta.music_state == BT_SINK_SRV_STATE_STREAMING) {
                        memcpy(&device_in_music, &conn_addrs[idx], sizeof(bt_bd_addr_t));
                        p_t = (uint8_t*)&conn_addrs[idx];
                        APPS_LOG_MSGID_I(APP_HFP_UTILS" MUSIC ADDR: %x,%x,%x,%x.", 4, p_t[0], p_t[1], p_t[2], p_t[3]);
                    }
                }
                break;
            }

            /* If HFP start, try to disconnect the second phone's A2DP and AVRCP connection. */
            case BT_SINK_SRV_STATE_INCOMING:
            case BT_SINK_SRV_STATE_OUTGOING:
            case BT_SINK_SRV_STATE_ACTIVE: {
                const bt_bd_addr_t* hfp_addr = NULL;
                conn_nums = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), conn_addrs, 2);
                if (conn_nums < 2) {
                    break;
                }

                hfp_addr = bt_sink_srv_hf_get_last_operating_device();
                if (hfp_addr == NULL) {
                    break;
                }
                /* If another phone is not in playing state, disconnect it's A2DP and avrcp connection. */
                if ((memcmp(&device_in_music, &empt_addr, sizeof(bt_bd_addr_t)) == 0) ||
                    (memcmp(&device_in_music, hfp_addr, sizeof(bt_bd_addr_t)) == 0)) {
                    /* Find idle device's addr. */
                    bt_bd_addr_t* idle_addr = NULL;
                    if (memcmp(hfp_addr, &conn_addrs[0], sizeof(bt_bd_addr_t)) == 0) {
                        idle_addr = &conn_addrs[1];
                    } else {
                        idle_addr = &conn_addrs[0];
                    }
                    /* Disconnect */
                    p_t = (uint8_t*)hfp_addr;
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" HFP ADDR: %x,%x,%x,%x.", 4, p_t[0], p_t[1], p_t[2], p_t[3]);
                    p_t = (uint8_t*)idle_addr;
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" idle ADDR: %x,%x,%x,%x.", 4, p_t[0], p_t[1], p_t[2], p_t[3]);
                    bt_cm_connect_t dis_conn = {{0}, 0};
                    memcpy(&dis_conn.address, idle_addr, sizeof(bt_bd_addr_t));
                    dis_conn.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SOURCE);
                    sta = bt_cm_disconnect(&dis_conn);
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" disconnect the A2DP profile sta: %d.", 1, sta);
                    dis_conn.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP);
                    sta = bt_cm_disconnect(&dis_conn);
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" disconnect the AVRCP profile sta: %d.", 1, sta);
                    /* Clear music info. */
                    memset(&device_in_music, 0, sizeof(bt_bd_addr_t));
                }
                break;
            }

            /**/
            default: {
                /* Clear music info. */
                memset(&device_in_music, 0, sizeof(bt_bd_addr_t));
                break;
            }
        }

        /* try to resume connection */
        if (param->current <= BT_SINK_SRV_STATE_STREAMING) {
            bt_cm_profile_service_mask_t music_mask = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP);
            music_mask |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SOURCE);
            bt_cm_profile_service_mask_t conn_mask = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
            conn_nums = bt_cm_get_connected_devices(~conn_mask, conn_addrs, 2);
            for (idx = 0; idx < conn_nums; idx++) {
                bt_cm_profile_service_mask_t mask = bt_cm_get_connected_profile_services(&conn_addrs[idx]);
                if ((mask & music_mask) == 0) {
                    bt_cm_connect_t conn = {{0}, 0};
                    p_t = (uint8_t*)&conn_addrs[idx];
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" connected with out A2DP addr: %x,%x,%x,%x.", 4, p_t[0], p_t[1], p_t[2], p_t[3]);
                    memcpy(&conn.address, &conn_addrs[idx], sizeof(bt_bd_addr_t));
                    conn.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP);
                    sta = bt_cm_connect(&conn);
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" connect the AVRCP profile sta: %d.", 1, sta);
                    conn.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SOURCE);
                    sta = bt_cm_connect(&conn);
                    APPS_LOG_MSGID_I(APP_HFP_UTILS" connect the A2DP profile sta: %d.", 1, sta);
                }
            }
        }
    }
}
#endif

#if defined(AIR_MULTI_POINT_ENABLE) && defined(AIR_EMP_AUDIO_INTER_STYLE_ENABLE)
bool app_hfp_proc_conflict_vp_event(ui_shell_activity_t *self,
                                uint32_t event_group,
                                uint32_t event_id,
                                void *extra_data,
                                size_t data_len)
{
    bool ret = false;
    /* Play the conflict incoming call VP when two smart phones incoming call at the same time. */
    hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
    bt_sink_srv_event_call_state_conflict_t *update_ring_ind = (bt_sink_srv_event_call_state_conflict_t *)extra_data;
    if (update_ring_ind == NULL) {
        APPS_LOG_MSGID_I(APP_HFP_UTILS" [CONFLICT VP] update_ring_ind: NULL.", 0);
        return ret;
    }
    APPS_LOG_MSGID_I(APP_HFP_UTILS" [CONFLICT VP] update_ring_ind: call_state=%x, aws_link_state=%d",
            2, update_ring_ind->call_state, hfp_context->aws_link_state);
    if (update_ring_ind->call_state == BT_SINK_SRV_STATE_INCOMING) {
        if (APP_HFP_INCOMING_CALL_VP_LONG == app_hfp_get_incoming_call_vp_flag()) {
            APPS_LOG_MSGID_I(APP_HFP_UTILS",[CONFLICT VP] set short delay time incoming call vp.", 0);
            app_hfp_incoming_call_vp_process(self, FALSE, FALSE);
        } else {
            APPS_LOG_MSGID_I(APP_HFP_UTILS",[CONFLICT VP] set long delay time incoming call vp.", 0);
            app_hfp_incoming_call_vp_process(self, TRUE, FALSE);
        }
    } else {
        /* Two incoming call -->  one incoming call (one of incoming was rejected or accepted)*/
        APPS_LOG_MSGID_I(APP_HFP_UTILS" [CONFLICT VP] update mmi state when conflict vp state change.", 0);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0, NULL, 0);

        bt_bd_addr_t* actived_addr = NULL;
        uint8_t *p_t = NULL;
        bt_sink_srv_device_state_t actived_addr_state_list = {{0},0,0,0,0};
        uint32_t conn_nums;
        bt_bd_addr_t conn_addrs[2];
        uint32_t idx = 0;
        conn_nums = bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), conn_addrs, 2);

        for (idx = 0; idx < conn_nums; idx++) {
            if (memcmp(&update_ring_ind->address, &conn_addrs[idx], sizeof(bt_bd_addr_t)) != 0) {
                //memcpy(&device_actived, &conn_addrs[idx], sizeof(bt_bd_addr_t));
                actived_addr = &conn_addrs[idx];
            }
        }
        p_t = (uint8_t*)actived_addr;
        APPS_LOG_MSGID_I(APP_HFP_UTILS",[CONFLICT VP] actived_addr:%02x %02x %02x %02x %02x %02x.",
                6, p_t[0], p_t[1], p_t[2],p_t[3], p_t[4], p_t[5]);
        bt_sink_srv_get_device_state((const bt_bd_addr_t*)actived_addr, &actived_addr_state_list, 1);
        APPS_LOG_MSGID_I(APP_HFP_UTILS",[CONFLICT VP] is_inband_supported=%d.", 1, actived_addr_state_list.is_inband_supported);
        if (actived_addr_state_list.is_inband_supported == TRUE) {
            if (VP_INDEX_INCOMING_CALL == app_voice_prompt_get_current_index()) {
                APPS_LOG_MSGID_I(APP_HFP_UTILS",[CONFLICT VP] stop incoming call vp.", 0);
#ifdef MTK_AWS_MCE_ENABLE
                app_voice_prompt_set_voice_stopping(true);
#endif
                app_hfp_stop_vp();
            }
        } else {
            APPS_LOG_MSGID_I(APP_HFP_UTILS",[CONFLICT VP] set long delay time incoming call vp.", 0);
            app_hfp_incoming_call_vp_process(self, TRUE, FALSE);
        }
    }

    return ret;
}
#endif

void app_hfp_incoming_call_vp_process(ui_shell_activity_t *self, bool isLongVp, bool isTwcIncoming)
{
#ifdef MTK_PROMPT_SOUND_ENABLE
    hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
    if (hfp_context == NULL) {
        return;
    }
    /* Play short delay time vp when two incoming call from two smart phone at the same time(conflict vp). */
    if (isLongVp == TRUE) {
        app_hfp_set_incoming_call_vp_flag(APP_HFP_INCOMING_CALL_VP_LONG);
    } else {
        app_hfp_set_incoming_call_vp_flag(APP_HFP_INCOMING_CALL_VP_SHORT);
    }
    uint32_t vp_delay_time = (isLongVp == TRUE) ? APP_HFP_INCMOING_CALL_VP_LONG_DELAY_TIME : APP_HFP_INCMOING_CALL_VP_SHORT_DELAY_TIME;
    uint32_t vp_index = (isTwcIncoming == TRUE) ? VP_INDEX_TWC_INCOMING_CALL : VP_INDEX_INCOMING_CALL;
    APPS_LOG_MSGID_I(APP_HFP_UTILS",[RING_IND] vp_delay_time=%d, vp_index=%d, aws_link_state=%d",
            3, vp_delay_time, vp_index, hfp_context->aws_link_state);
    /* Note: the last parameter need to set into NULL when play local vp.*/
    if (TRUE == hfp_context->aws_link_state) {
        app_hfp_sync_incoming_call_vp_flag();
        APPS_LOG_MSGID_I(APP_HFP_UTILS",[RING_IND] set sync incoming call vp. ", 0);
        apps_config_set_voice(vp_index, true, vp_delay_time, VOICE_PROMPT_PRIO_ULTRA, true, true, app_hfp_incoming_call_vp_callback);
    } else {
        APPS_LOG_MSGID_I(APP_HFP_UTILS",[RING_IND] set local incoming call vp. ", 0);
        app_hfp_set_incoming_call_vp_sync_flag(APP_HFP_INCOMING_CALL_VP_SYNC_PLAY_FAIL);
        if (vp_index == VP_INDEX_INCOMING_CALL) {
            apps_config_set_voice(vp_index, false, vp_delay_time, VOICE_PROMPT_PRIO_ULTRA, true, true, NULL);
        } else if (vp_index == VP_INDEX_TWC_INCOMING_CALL) {
            apps_config_set_vp(vp_index, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            /* send a delay event as a timer to loop twc incoming call playback. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_PLAY_TWC_INCOMING_CALL_VP, NULL, 0,
                    NULL, TWC_INCOMING_CALL_VP_TIMER_INTERVAL);
        }
    }
#endif
}

#if defined(AIR_MULTI_POINT_ENABLE) && defined(MTK_AWS_MCE_ENABLE)
void app_hfp_sync_sink_state_to_peer(ui_shell_activity_t *self)
{
    hfp_context_t *hfp_context = (hfp_context_t *)(self->local_context);
    if (hfp_context == NULL) {
        return;
    }
    bt_sink_srv_state_change_t param;
    bt_status_t send_state = BT_STATUS_FAIL;

    param.current = hfp_context->curr_state;
    param.previous = hfp_context->pre_state;

    if (TRUE == hfp_context->aws_link_state) {
        send_state = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                APPS_EVENTS_INTERACTION_SYNC_HFP_STATE_TO_PARTNER, &param, sizeof(bt_sink_srv_state_change_t));
    }

    APPS_LOG_MSGID_I(APP_HFP_UTILS", agent send hfp_state_change to partner: send_state=%x, param->previous: %x,param->current: %x",
            3, send_state, hfp_context->pre_state,hfp_context->curr_state);
}
#endif

/**
  * @brief                            This function is used to mute or unmute microphone.
  * @param[in]  type            True is mute mic, flase is unmute mic.
  */
bool app_hfp_mute_mic(bool mute)
{
    bool ret = false;
    bool isNeedSync   =  FALSE;
    APPS_LOG_MSGID_I(APP_HFP_UTILS" [MUTE] mute microphone: mute=%d.", 1, mute);
        /* Mute or unmute mic.*/
    bt_status_t bt_status = BT_STATUS_FAIL;

#ifdef AIR_LE_AUDIO_ENABLE
    app_le_audio_aird_client_info_t *p_info;
    bt_handle_t handle;

    if ((BT_HANDLE_INVALID != (handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established())) &&
        (NULL != (p_info = app_le_audio_aird_client_get_info(handle))) &&
        (APP_LE_AUDIO_AIRD_MODE_SUPPORT_HID_CALL == p_info->mode)) {

        bt_status_t status = app_le_audio_aird_client_send_action(handle,
                                                    APP_LE_AUDIO_AIRD_ACTION_MUTE_MIC,
                                                    NULL,
                                                    0);

        APPS_LOG_MSGID_I(APP_HFP_UTILS" [LEA][CIS] MUTE set, status:%x", 1, status);
        return true;
    }
#endif

    bt_status = bt_sink_srv_set_mute(BT_SINK_SRV_MUTE_MICROPHONE, mute);
#ifdef AIR_LE_AUDIO_ENABLE
    bt_status = bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, mute);
#endif
    if (BT_STATUS_SUCCESS == bt_status) {
        ret = TRUE;
        /* Sync mute or unmute operation to peer when aws mce enable.*/
#ifdef MTK_AWS_MCE_ENABLE
        if (TRUE == app_home_screen_idle_activity_is_aws_connected()) {
            isNeedSync = TRUE;
            bt_status_t send_state = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_SYNC_MIC_MUTE_STATUS, &mute, sizeof(bool));
            if (BT_STATUS_SUCCESS == send_state) {
                APPS_LOG_MSGID_I(APP_HFP_UTILS" [MUTE] sync mute to peer success.", 0);
            } else if (BT_STATUS_FAIL == send_state) {
                APPS_LOG_MSGID_I(APP_HFP_UTILS" [MUTE] sync mute to peer fail.", 0);
            }
        }
#endif

#ifdef MTK_AWS_MCE_ENABLE
        if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role())
#endif
        {
            /* Play prompt voice.*/
            apps_config_set_vp(VP_INDEX_SUCCESSED, isNeedSync, 200, VOICE_PROMPT_PRIO_MEDIUM, FALSE, NULL);
        }
    }

    APPS_LOG_MSGID_I(APP_HFP_UTILS" [MUTE] mute_mic=%d, isNeedSync=%d",
            2, mute, isNeedSync);

    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
bool app_hfp_get_aws_link_is_switching(void)
{
    bool ret = false;
    /* Get current aws link state and connected/disconnected reason. */
    const app_bt_state_service_status_t * curr_state = app_bt_connection_service_get_current_status();
    if (!curr_state->aws_connected && curr_state->reason == BT_HCI_STATUS_SUCCESS) {
        ret = true;
    }
    APPS_LOG_MSGID_I(APP_HFP_UTILS" get_aws_link_is_switching: ret=%d", 1, ret);

    return ret;
}
#endif
