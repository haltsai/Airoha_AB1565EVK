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
#include "apps_events_key_event.h"
#include "apps_aws_sync_event.h"
#include "app_home_screen_idle_activity.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_ami.h"

apps_config_state_t app_get_config_status_by_state(bt_sink_srv_state_t state)
{
    apps_config_state_t status = APP_TOTAL_STATE_NO;
    APPS_LOG_MSGID_I(APP_HFP_UTILS", hfp_state: %x", 1, state);

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
    APPS_LOG_MSGID_I(APP_HFP_UTILS", hfp_status: %x", 1, status);
    return status;
}


static bool app_do_hfp_action(apps_config_key_action_t action)
{
    bt_sink_srv_action_t sink_action  = BT_SINK_SRV_ACTION_NONE;

    bool ret = true;

    APPS_LOG_MSGID_I(APP_HFP_UTILS", app_action : %x", 1, action);

    /* Map key action to sink service action. */
    switch (action) {
        case KEY_ACCEPT_CALL: {
            sink_action = BT_SINK_SRV_ACTION_ANSWER;
            break;
        }
        case KEY_REJCALL: {
            sink_action = BT_SINK_SRV_ACTION_REJECT;
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT ||
                bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_NONE)
#endif
            {
                apps_config_stop_voice(true, 200, true);
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
            sink_action = BT_SINK_SRV_ACTION_CALL_VOLUME_UP;
            break;
        }
        case KEY_VOICE_DN: {
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

    if (ret) {
        /* For emp. */
#if defined(MTK_AWS_MCE_ENABLE) && defined(AIR_MULTI_POINT_ENABLE)
        audio_channel_t channel = ami_get_audio_channel();
        bool aws_link_state = false;
        aws_link_state = app_home_screen_idle_activity_is_aws_connected();
        if ((AUDIO_CHANNEL_L == channel) && (true == aws_link_state)) {
            APPS_LOG_MSGID_I(APP_HFP_UTILS", L can not operation call: channel=%d, aws_link_state=%d",
                    2, channel, aws_link_state);
            return ret;
        } else {
            bt_sink_srv_send_action(sink_action, NULL);
        }
#else
        bt_sink_srv_send_action(sink_action, NULL);
#endif
    }
    return ret;
}


bool app_hfp_proc_key_event(
    bt_sink_srv_state_t state,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    uint8_t key_id;
    airo_key_event_t key_event;

    bool ret = false;

#ifdef MTK_BT_SPEAKER_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (role == BT_AWS_MCE_ROLE_PARTNER || role == BT_AWS_MCE_ROLE_CLIENT) {
        return ret;
    }
#endif

    /* Decode event_id to key_id and key_event. */
    app_event_key_event_decode(&key_id, &key_event, event_id);
    APPS_LOG_MSGID_I(APP_HFP_UTILS", hfp_proc_key_event: key_id: %x, key_event: %x", 2, key_id, key_event);
    apps_config_key_action_t action;
    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }
    APPS_LOG_MSGID_I(APP_HFP_UTILS", action : %x", 1, action);

    ret = app_do_hfp_action(action);

    return ret;
}

bool app_hfp_update_led_bg_pattern(ui_shell_activity_t *self, bt_sink_srv_state_t now, bt_sink_srv_state_t pre)
{
    bool ret = true;
    APPS_LOG_MSGID_I(APP_HFP_UTILS", now = %x", 1, now);

    /* Update LED display by current MMI state. */
    switch (now) {
        case BT_SINK_SRV_STATE_INCOMING: {
            APPS_LOG_MSGID_I("incoming", 0);
            apps_config_set_background_led_pattern(LED_INDEX_INCOMING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_OUTGOING: {
            APPS_LOG_MSGID_I("outgoing", 0);
            apps_config_set_background_led_pattern(LED_INDEX_OUTGOING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_ACTIVE: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS", active", 0);
            apps_config_set_background_led_pattern(LED_INDEX_CALL_ACTIVE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_HELD_REMAINING: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS", HELD_REMAINING", 0);
            apps_config_set_background_led_pattern(LED_INDEX_HOLD_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_HELD_ACTIVE: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS", HELD_ACTIVE", 0);
            apps_config_set_background_led_pattern(LED_INDEX_CALL_ACTIVE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_TWC_INCOMING: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS", twc_incoming", 0);
            apps_config_set_background_led_pattern(LED_INDEX_INCOMING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_TWC_OUTGOING: {
            APPS_LOG_MSGID_I(APP_HFP_UTILS", outgoing", 0);
            apps_config_set_background_led_pattern(LED_INDEX_OUTGOING_CALL, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        case BT_SINK_SRV_STATE_MULTIPARTY: {
            /* There is a conference call. */
            APPS_LOG_MSGID_I("multiparty", 0);
            apps_config_set_background_led_pattern(LED_INDEX_CALL_ACTIVE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            break;
        }
        default: {
            ret = false;
            break;
        }
    }

    APPS_LOG_MSGID_I(APP_HFP_UTILS", proc_other_event: ret = %x", 1, ret);

    return ret;
}

uint8_t g_incoming_call_vp_flag;
void app_hfp_set_incoming_call_vp_flag(uint8_t is_vp)
{

    APPS_LOG_MSGID_I(APP_HFP_UTILS",set_vp_flag: is_vp = %x", 1, is_vp);
    g_incoming_call_vp_flag = is_vp;
}

uint8_t app_hfp_get_incoming_call_vp_flag()
{
    APPS_LOG_MSGID_I(APP_HFP_UTILS",get_vp_flag: is_vp = %x", 1, g_incoming_call_vp_flag);

    return g_incoming_call_vp_flag;
}

