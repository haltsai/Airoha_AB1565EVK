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
 * File: app_master_utils.c
 *
 * Description: This file is supply utils functions for dual master feature.
 *
 */

#include "app_master_utils.h"
#include "apps_debug.h"
#include "apps_config_key_remapper.h"
#include "apps_events_key_event.h"
#include "apps_race_cmd_co_sys_event.h"
#include "apps_events_event_group.h"
#include "ui_shell_manager.h"
#ifdef AIR_USB_AUDIO_OUT_ENABLE
#include "app_usb_audio_idle_activity.h"
#endif
#ifdef AIR_LINE_OUT_ENABLE
#include "app_line_in_idle_activity.h"
#endif
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "app_ull_idle_activity.h"
#endif
#define LOG_TAG "[dual_master]utils "

extern app_dual_chip_master_context_t s_dual_chip_master_context;

app_dual_chip_master_mmi_priority_t app_dual_chip_master_get_slave_mmi_priority(apps_config_state_t mmi_state)
{
    if (APP_CONNECTABLE == mmi_state) {
        return APP_DUAL_CHIP_MASTER_MMI_PRIORITY_DISCOVERABLE;
    } else if (APP_CONNECTED >= mmi_state) {
        return APP_DUAL_CHIP_MASTER_MMI_PRIORITY_IDLE;
    } else if (APP_STATE_HELD_ACTIVE >= mmi_state) {
        return APP_DUAL_CHIP_MASTER_MMI_PRIORITY_BT_ACTIVE;
    } else {
        return APP_DUAL_CHIP_MASTER_MMI_PRIORITY_SPECIAL;
    }
}

bool app_dual_chip_master_process_key_event(apps_config_state_t slave_mmi_state, uint32_t event_id, void* extra_data)
{
    bool ret = false;
    bool wired_audio_out_streaming = false;
    bool need_slave_proc = false;
    uint8_t key_id;
    airo_key_event_t key_event;
    app_event_key_event_decode(&key_id, &key_event, event_id);

    apps_config_key_action_t action;
    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
#if defined(AIR_USB_AUDIO_OUT_ENABLE)
        if (app_usb_out_is_open()) {
            wired_audio_out_streaming = true;
            APPS_LOG_MSGID_I(LOG_TAG" usb out streaming, use HFP activate state", 0);
        }
#endif
#if defined(AIR_LINE_OUT_ENABLE)
        if (app_line_in_is_plug_in()) {
            wired_audio_out_streaming = true;
            APPS_LOG_MSGID_I(LOG_TAG" line out streaming, use HFP activate state", 0);
        }
#endif
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
        if (app_ull_is_uplink_open()) {
            wired_audio_out_streaming = true;
            APPS_LOG_MSGID_I(LOG_TAG" ull uplink streaming, use HFP activate state", 0);
        }
#endif
        if (wired_audio_out_streaming) {
            if ((action >= KEY_GSOUND_ENDPOINTING && action <= KEY_GSOUND_CANCEL) ||
                (action >= KEY_AMA_START && action <= KEY_AMA_MEDIA_CONTROL) ||
                (action >= KEY_VA_XIAOAI_START && action <= KEY_VA_XIAOAI_LONG_PRESS_TRIGGER_STOP) ||
                (action >= KEY_VA_XIAOWEI_START && action <= KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP)) {
                APPS_LOG_MSGID_I(LOG_TAG"action=%d rejected during wired audio or ULL uplink working", 1, action);
                return true;
            }
        }
    }

    switch (action) {
        case KEY_DISCOVERABLE:
        case KEY_CANCEL_DISCOVERABLE:
        case KEY_WAKE_UP_VOICE_ASSISTANT:
        case KEY_WAKE_UP_VOICE_ASSISTANT_CONFIRM:
        case KEY_WAKE_UP_VOICE_ASSISTANT_NOTIFY: 
        case KEY_GSOUND_PRESS:
        case KEY_GSOUND_RELEASE:
        case KEY_GSOUND_NOTIFY:
        case KEY_GSOUND_VOICE_QUERY:
        case KEY_GSOUND_CANCEL:
        case KEY_AMA_START:
        case KEY_AMA_START_NOTIFY:
        case KEY_AMA_STOP:
        case KEY_AMA_MEDIA_CONTROL:
        case KEY_AMA_LONG_PRESS_TRIGGER_START:
        case KEY_AMA_LONG_PRESS_TRIGGER_STOP:
        case KEY_VA_XIAOAI_START:
        case KEY_VA_XIAOAI_START_NOTIFY:
        case KEY_VA_XIAOAI_STOP_PLAY:
        case KEY_VA_XIAOAI_LONG_PRESS_TRIGGER_START:
        case KEY_VA_XIAOAI_LONG_PRESS_TRIGGER_STOP:
        case KEY_VA_XIAOWEI_START:
        case KEY_VA_XIAOWEI_START_NOTIFY:
        case KEY_VA_XIAOWEI_STOP_PLAY:
        case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_START:
        case KEY_VA_XIAOWEI_LONG_PRESS_TRIGGER_STOP: {
            /* Action for any MMI status. */
            need_slave_proc = true;
            ret = true;
            break;
        }
        default:
            break;
    }

    if (slave_mmi_state == APP_CONNECTED && !need_slave_proc) {
        switch(action) {
            case KEY_VOICE_UP:
            case KEY_VOICE_DN:
            case KEY_AVRCP_PLAY:
            case KEY_REDIAL_LAST_CALL: {
                /* Action for BT connected. */
                if ((KEY_VOICE_UP != action && KEY_VOICE_DN != action) || apps_config_key_get_mmi_state() != APP_ULTRA_LOW_LATENCY_PLAYING) {
                    need_slave_proc = true;
                    ret = true;
                }
                break;
            }
            default:
                break;
        }
    }

    if (((slave_mmi_state >= APP_HFP_INCOMING && slave_mmi_state <= APP_HFP_MULTITPART_CALL) || slave_mmi_state == APP_STATE_HELD_ACTIVE) && !need_slave_proc) {
        switch(action) {
            case KEY_CANCEL_OUT_GOING_CALL:
            case KEY_REJCALL:
            case KEY_REJCALL_SECOND_PHONE:
            case KEY_ONHOLD_CALL:
            case KEY_ACCEPT_CALL:
            case KEY_END_CALL:
            case KEY_SWITCH_AUDIO_PATH:
            case KEY_MUTE_MIC:
            case KEY_3WAY_HOLD_ACTIVE_ACCEPT_OTHER: {
                /* Action for calling status. */
                need_slave_proc = true;
                ret = true;
                break;
            }
            default:
                break;
        }
    }

    if (slave_mmi_state == APP_A2DP_PLAYING && !need_slave_proc) {
        switch(action) {
            case KEY_AVRCP_PAUSE:
            case KEY_AVRCP_FORWARD:
            case KEY_AVRCP_BACKWARD:
            case KEY_AVRCP_FAST_FORWARD_PRESS:
            case KEY_AVRCP_FAST_FORWARD_RELEASE:
            case KEY_AVRCP_FAST_REWIND_PRESS:
            case KEY_AVRCP_FAST_REWIND_RELEASE:
            case KEY_AUDIO_PEQ_SWITCH: {
                /* Action for music status. */
                need_slave_proc = true;
                ret = true;
                break;
            }
            default:
                break;
        }
    }

    if (slave_mmi_state == APP_STATE_FIND_ME && !need_slave_proc) {
        switch(action) {
            case KEY_STOP_FIND_ME: {
                /* Action for find me. */
                need_slave_proc = true;
                ret = true;
                break;
            }
            default:
                break;
        }
    }

    if (need_slave_proc) {
        APPS_LOG_MSGID_I(LOG_TAG": send the action(0x%x) to slave when slave mmi state is %d", 2, action, slave_mmi_state);
        app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_KEY, event_id, &action, sizeof(action), false);
    }

    return ret;
}

apps_config_state_t app_master_utils_get_slave_mmi_state(void)
{
    return s_dual_chip_master_context.slave_mmi_state;
}


#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"

void app_dual_chip_utls_power_saving_state_change(void)
{
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_POWER_SAVING,
                    APP_POWER_SAVING_EVENT_NOTIFY_CHANGE, NULL, 0, NULL, 0);
}
#endif