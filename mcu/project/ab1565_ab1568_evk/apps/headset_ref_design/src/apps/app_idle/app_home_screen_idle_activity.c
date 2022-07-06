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
 * File: app_home_screen_idle_activity.c
 *
 * Description: This file could control system power on/off, trigger BT on/off etc.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for Homescreen APP.
 *
 */

#include "app_home_screen_idle_activity.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#include "app_bt_conn_componet_in_homescreen.h"
#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_key_remapper.h"
#include "apps_config_features_dynamic_setting.h"
#if defined(AIR_APP_SYSTEM_ON_BY_LINE_IN_ENABLE)
#include "app_ull_idle_activity.h"
#endif
#include "battery_management.h"
#include "apps_aws_sync_event.h"
#include "multi_va_manager.h"
#include "project_config.h"
#include "apps_customer_config.h"

#include "FreeRTOS.h"
#include "timers.h"
#include "ui_shell_manager.h"
#include "apps_atcmd.h"
#include "apps_events_event_group.h"
#include "apps_events_key_event.h"
#include "apps_events_bt_event.h"
#include "apps_config_event_list.h"
#include "app_bt_state_service.h"
#include "bt_customer_config.h"
#include "nvdm_config_factory_reset.h"
#include "at_command_bt.h"
#include "bt_sink_srv.h"
#include "bt_app_common.h"
#include "bt_power_on_config.h"
#include "apps_events_interaction_event.h"
#include "bt_sink_srv_ami.h"
#include "bt_device_manager.h"
#include "bt_device_manager_test_mode.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#include "nvdm.h"
#include "nvdm_id_list.h"
#include "nvkey.h"
#include "nvkey_id_list.h"
#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif
#include "apps_config_audio_helper.h"
#ifdef MTK_VA_XIAOAI_ENABLE
#include "xiaoai.h"
#endif
#include "hal_rtc.h"
#include "hal_wdt.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_srv.h"
#endif
#include "fota.h"
#include "nvdm_config.h"
#include "apps_debug.h"
#if defined(AIR_APP_SYSTEM_ON_BY_LINE_IN_ENABLE)
#include "app_line_in_idle_activity.h"
#endif
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
#include "app_advance_passthrough.h"
#endif
#ifdef AIR_LE_AUDIO_ENABLE
#include "app_le_audio.h"
#include "bt_le_audio_util.h"
#ifdef AIR_LE_AUDIO_BIS_ENABLE
#include "app_le_audio_bis.h"
#endif
#if (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))
#include "app_bt_conn_manager.h"
#endif
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
#include "apps_race_cmd_co_sys_event.h"
#endif
#ifdef AIR_TILE_ENABLE
#include "app_tile.h"
#endif

#ifdef AIR_SWIFT_PAIR_ENABLE
#include "app_swift_pair_idle_activity.h"
#endif
#include "bt_sink_srv_le_volume.h"

#define UI_SHELL_IDLE_BT_CONN_ACTIVITY  "[TK_Home]app_home_screen_idle_activity"

#define POWER_OFF_TIMER_NAME       "POWER_OFF"              /* Use a timeout before power off, to show LED and play VP. */
#define WAIT_TIME_BEFORE_POWER_OFF  (4 * 1000)              /* The delay time to do system power off for playing VP and LED. */
#define RECONNECT_DEVICE_NUMS       (1)                     /* The max device nums of try to reconnected after power on.  */
#define TIME_TO_RECONNECT_DEVICE    (10 * 1000)             /* The time of reconnect the connected device. */
#define TIME_TO_START_VISIBLE_AFTER_POWER_ON   (RECONNECT_DEVICE_NUMS * TIME_TO_RECONNECT_DEVICE)
                                                            /* The delay time to start BT visible after BT power on. */
#define TIME_TO_STOP_RECONNECTION   (2 * 60 * 1000)         /* The delay time to stop reconnection. */
#define VISIBLE_TIMEOUT             (2 * 60 * 1000)         /* The timeout of BT visibility. */

/* Global context for Homescreen APP. */
static home_screen_local_context_type_t s_app_homescreen_context;

static uint16_t s_factory_reset_pending_event = KEY_ACTION_INVALID;
static uint16_t s_factory_reset_key_action = KEY_ACTION_INVALID;
static uint16_t s_factory_reset_doing = false;
static bool s_ready_to_off = false;

static uint16_t s_reconnect_device_idx = 0;

#ifdef AIR_LE_AUDIO_ENABLE
static bool s_le_audio_muted = false;
#endif
#ifdef MTK_AWS_MCE_ENABLE
static bool s_sync_reboot_waiting = false;
#endif

#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
static bool s_reboot_trigger_sp_reconnect = false;
#endif

#ifdef AIR_TILE_ENABLE
static bool s_fake_off = false;
#endif
#define RETRY_ENTER_RTC_MODE_TIMES      (10)

void app_home_screen_check_power_off_and_reboot(void)
{
    if (s_ready_to_off) {
        if (APP_HOME_SCREEN_STATE_POWERING_OFF == s_app_homescreen_context.state) {
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
            if (s_app_homescreen_context.master_waiting_power_off_resp) {
                s_app_homescreen_context.master_waiting_power_off_resp = false;
                app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD, APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_POWER_OFF_READY,
                                               NULL, 0, false);
                return;
            }
#endif
            /* Enter RTC mode as power off action when no waiting and power_key released. */
            uint32_t i;
            /* Because some chip can display LED in RTC mode, must clear it before enter RTC mode. */
            apps_config_led_disable_all();
            for (i = 0; ; i++) {
                if (HAL_RTC_STATUS_ERROR == hal_rtc_enter_rtc_mode()) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Enter RTC mode fail !!!", 0);
                    if (i > RETRY_ENTER_RTC_MODE_TIMES) {
                        configASSERT(0 && "Enter RTC mode fail !!!");
                    }
                } else {
                    break;
                }
            }
        } else if (APP_HOME_SCREEN_STATE_REBOOT == s_app_homescreen_context.state) {
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
            if (s_app_homescreen_context.master_waiting_power_off_resp) {
                s_app_homescreen_context.master_waiting_power_off_resp = false;
                app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD, APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_REBOOT_READY,
                                               NULL, 0, false);
                return;
            }
#endif
            /* Reset WDT as reboot action. */
            hal_wdt_status_t ret;
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_home_screen_check_and_do_power_off reboot", 0);
            ret = hal_wdt_software_reset();
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_home_screen_check_and_do_power_off reboot ret = %d", 1, ret);
        }
    }
}

static void app_home_screen_check_and_do_power_off(home_screen_local_context_type_t *local_context)
{
    /* Must confirm BT off before system power off and reboot. */
    if (APP_BT_STATE_POWER_STATE_DISABLED == app_bt_connection_service_get_current_status()->current_power_state) {
        if (APP_HOME_SCREEN_STATE_POWERING_OFF == local_context->state
            && !local_context->power_off_waiting_time_out
            && local_context->power_off_waiting_release_key == DEVICE_KEY_NONE
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
            && local_context->waiting_dual_slave_state == APP_HOME_SCREEN_STATE_IDLE
#endif
            ) {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", ready to system off", 0);
            s_ready_to_off = true;
        } else if (APP_HOME_SCREEN_STATE_REBOOT == local_context->state
                   && !local_context->power_off_waiting_time_out
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
                   && local_context->waiting_dual_slave_state == APP_HOME_SCREEN_STATE_IDLE
#endif
        ) {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", ready to reboot", 0);
            s_ready_to_off = true;
        }
    }
}
#ifdef AIR_TILE_ENABLE
static void _tile_trigger_classic_bt_power_off_flow(void)
{
    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Tile has been activated, classic BT power off", 0);
    ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF, NULL, 0, NULL, 0);
    /* Notify fast pair to stop ble adv after 3 sec */
    ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                APPS_EVENTS_INTERACTION_STOP_FAST_PAIR_ADV, NULL, 0, NULL, 3000);

    /* !!! Should align _trigger_power_off_flow() some conditions */
#ifdef MTK_VA_XIAOAI_ENABLE
    xiaoai_app_save_into_flash();
#endif
    /* ANC suspend before power off. */
#ifdef MTK_ANC_ENABLE
    app_anc_service_save_into_flash();
    bool anc_ret = app_anc_service_suspend();
    APPS_LOG_MSGID_I("audio_anc_suspend after when power off in ret = %d", 1, anc_ret);
#endif
    /* Disable Audio before power off to avoid pop sound. */
    ami_audio_power_off_flow();
#ifdef AIR_LE_AUDIO_ENABLE
    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"[LEA] mute LE Audio", 0);
    bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, true);
    s_le_audio_muted = true;
#endif

    /* You don't need to stop and start ble advertising again */
    /* LE service will not switch connection handle when "earbuds" start RHO */
    /* For Headset project, You don't need to handle RHO in headset project either since RHO is only for earbuds */
    s_fake_off = true;
    apps_config_key_set_mmi_state(APP_BT_OFF);
}
#endif
static void _trigger_power_off_flow(struct _ui_shell_activity *self, bool need_wait, bool need_rho)
{
    home_screen_local_context_type_t *local_context = (home_screen_local_context_type_t *)(self->local_context);

    /* Should return if Homescreen APP is powering off or reboot state. */
    if (local_context->state != APP_HOME_SCREEN_STATE_IDLE) {
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", _trigger_power_off_flow, already prepared to power off, just wait", 0);
        return;
    } else {
        local_context->state = APP_HOME_SCREEN_STATE_POWERING_OFF;
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
        local_context->waiting_dual_slave_state = APP_HOME_SCREEN_STATE_POWERING_OFF;
        app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_REQUEST_IMMEDIATELY_POWER_OFF,
                                       NULL, 0, false);
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
        local_context->master_waiting_power_off_resp = true;
#endif
    }

#ifdef MTK_VA_XIAOAI_ENABLE
    xiaoai_app_save_into_flash();
#endif
    /* ANC suspend before power off. */
#ifdef MTK_ANC_ENABLE
    app_anc_service_save_into_flash();
    bool anc_ret = app_anc_service_suspend();
    APPS_LOG_MSGID_I("audio_anc_suspend when power off in ret = %d", 1, anc_ret);
#endif
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
    app_advance_passthrough_save_to_flash();
    APPS_LOG_MSGID_I("[ADVANCE_PT] save to flash when power off", 0);
#endif
    /* Disable Audio before power off to avoid pop sound. */
    ami_audio_power_off_flow();
#ifdef AIR_LE_AUDIO_ENABLE
    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"[LEA] mute LE Audio", 0);
    bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, true);
    s_le_audio_muted = true;
#endif
    /* Disable BT with for_system_off by using BT state service API. */
    app_bt_state_service_set_bt_on_off(false, false, need_rho, true);

    /* Send delay event to power off directly if need_wait. */
    if (need_wait) {
        local_context->power_off_waiting_time_out = true;
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_POWER_OFF_WAIT_TIMEOUT, NULL, 0,
                            NULL, WAIT_TIME_BEFORE_POWER_OFF);
    } else {
        /* Check and do power off. */
        local_context->power_off_waiting_time_out = false;
        app_home_screen_check_and_do_power_off(local_context);
    }
}

static void _trigger_reboot_flow(struct _ui_shell_activity *self, uint32_t wait_time)
{
    home_screen_local_context_type_t *local_context = (home_screen_local_context_type_t *)(self->local_context);

    /* Should return if Homescreen APP is powering off or reboot state. */
    if (local_context->state != APP_HOME_SCREEN_STATE_IDLE) {
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", reboot flow, already prepared to power off when request reboot, just wait", 0);
        return;
    } else {
        local_context->state = APP_HOME_SCREEN_STATE_REBOOT;
#ifdef MTK_VA_XIAOAI_ENABLE
        xiaoai_app_save_into_flash();
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
        local_context->waiting_dual_slave_state = APP_HOME_SCREEN_STATE_REBOOT;
        app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_REQUEST_REBOOT,
                                       NULL, 0, false);
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
        local_context->master_waiting_power_off_resp = true;
#endif
        /* ANC suspend before power off. */
#ifdef MTK_ANC_ENABLE
        app_anc_service_save_into_flash();
        bool anc_ret = app_anc_service_suspend();
        APPS_LOG_MSGID_I("audio_anc_suspend after when reboot in ret = %d", 1, anc_ret);
#endif
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
        app_advance_passthrough_save_to_flash();
        APPS_LOG_MSGID_I("[ADVANCE_PT] save to flash when reboot", 0);
#endif
        /* Disable Audio before power off to avoid pop sound. */
        ami_audio_power_off_flow();
        /* Disable BT with for_system_off by using BT state service API. */
#ifdef MTK_AWS_MCE_ENABLE
        if (!s_sync_reboot_waiting)
#endif
        {
            app_bt_state_service_set_bt_on_off(false, false, false, true);
        }
    }
    if (wait_time > 0) {
        local_context->power_off_waiting_time_out = true;
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_POWER_OFF_WAIT_TIMEOUT, NULL, 0,
                            NULL, wait_time);
    } else {
        app_home_screen_check_and_do_power_off(local_context);
    }
}

#ifdef MTK_ANC_ENABLE

#ifdef MTK_ANC_HOWLING_TURN_OFF_ANC
static void app_home_screen_disable_anc_when_howling(ui_shell_activity_t *self)
{
    home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;
    bool control_ret = FALSE;

    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER
        && local_ctx && local_ctx->aws_connected) {
        if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_AUDIO_ANC, ANC_CONTROL_EVENT_HOWLING)) {
            control_ret = app_anc_service_disable();
            apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send ANC_CONTROL_EVENT_HOWLING to agent failed, disable self ret : %d",
                             1, control_ret);
        } else {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send ANC_CONTROL_EVENT_HOWLING to agent success", 0);
        }
    } else {
        control_ret = app_anc_service_disable();
        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", disable anc filter when ANC_CONTROL_EVENT_HOWLING, ret: %d",
                         1, control_ret);
    }
}
#endif

static bool app_home_screen_process_anc_and_pass_through(ui_shell_activity_t *self, apps_config_key_action_t key_action)
{
    bool ret = false;
    audio_anc_control_filter_id_t target_filter_id;
    audio_anc_control_type_t target_anc_type;
    home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;

    if (!local_ctx) {
        return ret;
    }

    uint8_t anc_enable;
    audio_anc_control_filter_id_t anc_current_filter_id;
    audio_anc_control_type_t anc_current_type;
    int16_t anc_runtime_gain;
    uint8_t support_hybrid_enable;
    audio_anc_control_get_status(&anc_enable, &anc_current_filter_id, &anc_current_type, &anc_runtime_gain, &support_hybrid_enable, NULL);

    if (KEY_PASS_THROUGH == key_action) {
        target_anc_type = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
        target_filter_id = AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT;
        ret = true;
    } else if (KEY_ANC == key_action) {
        if (support_hybrid_enable) {
            target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID; /* See request: hybrid, ff or fb.*/
        } else {
            target_anc_type = AUDIO_ANC_CONTROL_TYPE_FF;
        }
        target_filter_id = AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT;
        ret = true;
    } else if (KEY_SWITCH_ANC_AND_PASSTHROUGH == key_action) {
        /* Switch loop is OFF->PassThrough->ANC->OFF. */
        if (!anc_enable) {
            /* When last is OFF, next state is PassThrough. */
            target_anc_type = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
            target_filter_id = AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT;
        } else {
            /* If current filter is ANC_FILTER, must set to OFF, target is ANC_FILTER
            and if current filter is PassThrough filter, target is ANC filter. */
            if (support_hybrid_enable) {
                target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
            } else {
                target_anc_type = AUDIO_ANC_CONTROL_TYPE_FF;
            }
            target_filter_id = AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT;
        }
        ret = true;
    } else if (KEY_BETWEEN_ANC_PASSTHROUGH == key_action) {
        if (anc_enable && (AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT == anc_current_filter_id)) {
            /* Set as ANC. */
            if (support_hybrid_enable) {
                /* See request: hybrid, ff or fb.*/
                target_anc_type = AUDIO_ANC_CONTROL_TYPE_HYBRID;
            } else {
                target_anc_type = AUDIO_ANC_CONTROL_TYPE_FF;
            }
            target_filter_id = AUDIO_ANC_CONTROL_ANC_FILTER_DEFAULT;
        } else {
            /* Set as passthrough. */
            target_anc_type = AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF;
            target_filter_id = AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT;
        }
        ret = true;
    } else {
        return false;
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (local_ctx && local_ctx->aws_connected) {
        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
            if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, key_action)) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_ANC or KEY_PASS_THROUGH aws to agent failed : %d", 1, key_action);
                apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            } else {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_ANC or KEY_PASS_THROUGH aws to agent success : %d", 1, key_action);
            }
            return ret;
        }
    } else {
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_home_screen_process_anc_and_pass_through, aws_connected is false", 0);
        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        return ret;
    }
#endif

    bool control_ret = FALSE;
    if (anc_enable && (target_filter_id == anc_current_filter_id)) {
        control_ret = app_anc_service_disable();
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity, disable anc filter : %d ret: %d", 2,
                         target_filter_id, control_ret);
    } else {
        control_ret = app_anc_service_enable(target_filter_id, target_anc_type, AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, NULL);
        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity, enable anc filter %d ret: %d", 2,
                         target_filter_id, control_ret);
    }
    return ret;
}
#endif

static nvdm_status_t app_home_screen_fact_rst_nvdm_flag(void)
{
    uint8_t factrst_flag;
    nvdm_status_t status;

    APPS_LOG_MSGID_I("Write Factory reset flag to NVDM", 0);

    factrst_flag = FACTORY_RESET_FLAG;
    status = nvdm_write_data_item(NVDM_GROUP_FACTORY_RESET, NVDM_FACTORY_RESET_ITEM_FACTORY_RESET_FLAG,
                                  NVDM_DATA_ITEM_TYPE_RAW_DATA, &factrst_flag, 1);

    return status;
}

static nvdm_status_t app_home_screen_fact_rst_link_key_nvdm_flag(void)
{
    uint8_t factrst_flag;
    nvdm_status_t status;

    APPS_LOG_MSGID_I("Write Factory reset flag to NVDM", 0);

    factrst_flag = FACTORY_RESET_LINK_KEY;
    status = nvdm_write_data_item(NVDM_GROUP_FACTORY_RESET, NVDM_FACTORY_RESET_ITEM_FACTORY_RESET_FLAG,
                                  NVDM_DATA_ITEM_TYPE_RAW_DATA, &factrst_flag, 1);

    return status;
}

static bool _proc_ui_shell_group(struct _ui_shell_activity *self,
                                 uint32_t event_id,
                                 void *extra_data,
                                 size_t data_len)
{
    bool ret = true;
    home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", home_screen create", 0);
            /* Init context of Homescreen APP. */
            self->local_context = &s_app_homescreen_context;
            memset(self->local_context, 0, sizeof(home_screen_local_context_type_t));
            local_ctx = (home_screen_local_context_type_t *)self->local_context;
            local_ctx->state = APP_HOME_SCREEN_STATE_IDLE;
            local_ctx->bt_power_state = APP_HOME_SCREEN_BT_POWER_STATE_DISABLED;
            local_ctx->target_bt_power_state = APP_HOME_SCREEN_BT_POWER_STATE_DISABLED;
            local_ctx->connection_state = false;
            local_ctx->power_off_waiting_release_key = DEVICE_KEY_NONE;
            local_ctx->is_bt_visiable = false;
            app_atcmd_init();
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
            app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD, APPS_RECE_CMD_CO_SYS_DUAL_CHIP_EVENT_SLAVE_POWER_ON,
                                            NULL, 0, false);
            uint8_t off_reason = pmu_get_power_off_reason();
            s_reboot_trigger_sp_reconnect = off_reason & 0x08;
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", home_screen create:power off reason=%d, is_reconneced", 2, off_reason, s_reboot_trigger_sp_reconnect);
#endif
#ifdef MTK_ANC_ENABLE
            app_anc_service_init();
#endif
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity destroy", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity resume", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity pause", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity refresh", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity result", 0);
            if (extra_data) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", extra data for app_homescreen_idle_activity result", 0);
            }
            break;
        default:
            break;
    }
    return ret;
}

static bool _proc_key_event_group(ui_shell_activity_t *self,
                                  uint32_t event_id,
                                  void *extra_data,
                                  size_t data_len)
{
    bool ret = false;
    home_screen_local_context_type_t *local_context = (home_screen_local_context_type_t *)(self->local_context);

    uint8_t key_id;
    airo_key_event_t key_event;
    app_event_key_event_decode(&key_id, &key_event, event_id);

    /* Do not power off before key released. */
    if (local_context->power_off_waiting_release_key == key_id && key_event == AIRO_KEY_RELEASE) {
        local_context->power_off_waiting_release_key = DEVICE_KEY_NONE;
        app_home_screen_check_and_do_power_off(local_context);
    }

    /* Ignore other key event if Homescreen APP will power off. */
    if (APP_HOME_SCREEN_STATE_POWERING_OFF == local_context->state) {
        return true;
    }

    apps_config_key_action_t action;
    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }

    switch (action) {
        case KEY_BT_OFF:{
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Send REQUEST_ON_OFF_BT", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT,
                                (void *)false, 0,
                                NULL, 0);
            ret = true;
            break;
        }
        case KEY_POWER_OFF:
#ifdef AIR_TILE_ENABLE
            if (!app_tile_toa_waiting_authentication()) {
#endif
                /* Apply "power off" VP and foreground LED pattern. */
                apps_config_set_foreground_led_pattern(LED_INDEX_POWER_OFF, 30, false);
                if (battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST)
#if defined(AIR_APP_SYSTEM_ON_BY_LINE_IN_ENABLE)
                    || app_line_in_is_plug_in()
#endif
                ) {
                    apps_config_set_vp(VP_INDEX_POWER_OFF, false, 0, VOICE_PROMPT_PRIO_HIGH, false, NULL);
#ifdef AIR_TILE_ENABLE
                    if (!app_tile_tmd_is_active()) {
#endif
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Send REQUEST_ON_OFF_BT when Aux/USB in", 0);
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST,
                                            EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT,
                                            (void *)false, 0,
                                            NULL, 0);
#ifdef AIR_TILE_ENABLE
                    } else {
                        _tile_trigger_classic_bt_power_off_flow();
                    }
#endif
                } else {
#ifdef AIR_TILE_ENABLE
                    if (!app_tile_tmd_is_active() || app_tile_get_battery_state() <= APP_BATTERY_STATE_LOW_CAP) {
#endif
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGH, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_POWER_OFF, NULL, 0, NULL, 0);
                        apps_config_set_vp(VP_INDEX_POWER_OFF, false, 0, VOICE_PROMPT_PRIO_EXTREME, true, NULL);
                        _trigger_power_off_flow(self, true, true);
#ifdef AIR_TILE_ENABLE
                    } else {
                        apps_config_set_vp(VP_INDEX_POWER_OFF, false, 0, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
                        _tile_trigger_classic_bt_power_off_flow();
                    }
#endif
#if (defined(AB1565))
#else
                    /* When use long press to power off, must wait the key release. */
                    if (key_event >= AIRO_KEY_LONG_PRESS_1 && key_event <= AIRO_KEY_SLONG) {
                        local_context->power_off_waiting_release_key = key_id;
                    }
#endif
                }
#ifdef AIR_TILE_ENABLE
            } else {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", allowing Tile association.", 0);
                app_tile_toa_allow_association();
            }
#endif
            ret = true;
            break;
        case KEY_POWER_ON:
            /* Power on BT if current bt_power_state is disabled. */
            if (local_context->bt_power_state == APP_HOME_SCREEN_BT_POWER_STATE_DISABLED) {
                app_bt_state_service_set_bt_on_off(true, false, false, false);
                apps_config_set_vp(VP_INDEX_POWER_ON, false, 0, VOICE_PROMPT_PRIO_HIGH, false, NULL);
                apps_config_set_foreground_led_pattern(LED_INDEX_POWER_ON, 30, false);
#ifdef AIR_TILE_ENABLE
                if (s_fake_off) {
                    /* You don't need to stop and start ble advertising again */
                    /* LE service will not switch connection handle when "earbuds" start RHO */
                    /* For Headset project, You don't need to handle RHO in headset project either since RHO is only for earbuds */
                    s_fake_off = false;
                }
#endif
            }
            break;
        case KEY_SYSTEM_REBOOT:
            _trigger_reboot_flow(self, 0);
            ret = true;
            break;
        case KEY_DISCOVERABLE:
#if defined(AIR_LE_AUDIO_ENABLE) && (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))
            if (!app_bt_conn_manager_allow_edr_visible()) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", disallow KEY_DISCOVERABLE", 0);
                apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                ret = true;
                break;
            }
#endif

#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (local_context->aws_connected) {
                    if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, KEY_DISCOVERABLE)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_DISCOVERABLE aws to agent failed", 0);
                        apps_config_set_vp(VP_INDEX_FAILED, false, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    }
                } else {
                    app_bt_state_service_set_bt_visible(true, false, VISIBLE_TIMEOUT);
                    apps_config_set_vp(VP_INDEX_PAIRING, false, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner aws disconnected for KEY_DISCOVERABLE", 0);
                }
            } else if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
            {
#ifdef AIR_SWIFT_PAIR_ENABLE
                app_swift_pair_start_adv();
#endif
                app_bt_state_service_set_bt_visible(true, false, VISIBLE_TIMEOUT);
                apps_config_set_vp(VP_INDEX_PAIRING, true, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            ret = true;
            break;
        case KEY_CANCEL_DISCOVERABLE:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", set visible to false.", 0);
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (local_context->aws_connected) {
                    if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, KEY_CANCEL_DISCOVERABLE)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_CANCEL_DISCOVERABLE aws to agent failed", 0);
                    } else {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_CANCEL_DISCOVERABLE aws to agent success", 0);
                    }
                }
            } else {
                app_bt_state_service_set_bt_visible(false, false, 0);
            }
#else
            app_bt_state_service_set_bt_visible(false, false, 0);
#endif
            break;
        case KEY_RECONNECT_LAST_DEVICE:
            /* Send AWS sync event to Agent for Partner role and AWS connected. */
            /* Play "fail" VP if Partner send fail or AWS not connected. */
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (local_context->aws_connected) {
                    if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, KEY_RECONNECT_LAST_DEVICE)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_RECONNECT_LAST_DEVICE aws to agent failed", 0);
                        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    } else {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_RECONNECT_LAST_DEVICE aws to agent success", 0);
                    }
                } else {
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner aws disconnected for KEY_RECONNECT_LAST_DEVICE", 0);
                }
            } else if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
            {
                bt_bd_addr_t *p_bd_addr = bt_device_manager_remote_get_dev_by_seq_num(1);
                bt_cm_connect_t connect_param;
                connect_param.profile = bt_customer_config_app_get_cm_config()->power_on_reconnect_profile
                                        & ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
                if (p_bd_addr) {
                    memcpy(connect_param.address, *p_bd_addr, sizeof(bt_bd_addr_t));
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity receive KEY_RECONNECT_LAST_DEVICE", 0);
                    bt_cm_connect(&connect_param);
                    apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                }
            }
            ret = true;
            break;
        case KEY_RESET_PAIRED_DEVICES:
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (local_context->aws_connected) {
                    if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, KEY_RESET_PAIRED_DEVICES)) {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_RESET_PAIRED_DEVICES aws to agent failed", 0);
                        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    } else {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_RESET_PAIRED_DEVICES aws to agent success", 0);
                    }
                } else {
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner aws disconnected for KEY_RESET_PAIRED_DEVICES", 0);
                }
            } else if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
            {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", app_homescreen_idle_activity receive KEY_RESET_PAIRED_DEVICES", 0);
                bt_device_manager_unpair_all();
                apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            ret = true;
            break;
#ifdef MTK_ANC_ENABLE
        case KEY_PASS_THROUGH:
        case KEY_ANC:
        case KEY_SWITCH_ANC_AND_PASSTHROUGH:
        case KEY_BETWEEN_ANC_PASSTHROUGH:
            /* Handle ANC key event. */
            ret = app_home_screen_process_anc_and_pass_through(self, action);
            break;
#endif
        case KEY_FACTORY_RESET:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive key to do factory reset", 0);
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action)) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", sync KEY_FACTORY_RESET_AND_POWEROFF fail.", 0);
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                }
                break;
            }
            apps_config_set_vp(VP_INDEX_SUCCESSED, true, 100, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
#else
            apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
#endif
            s_factory_reset_key_action = KEY_FACTORY_RESET;
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_FACTORY_RESET_REQUEST, NULL, 0,
                                NULL, 0);
            ret = true;
            break;

        case KEY_FACTORY_RESET_AND_POWEROFF:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive key to do factory reset and power off", 0);
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action)) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", sync KEY_FACTORY_RESET_AND_POWEROFF fail.", 0);
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                }
                break;
            }
            apps_config_set_vp(VP_INDEX_SUCCESSED, true, 100, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
#else
            apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
#endif
            s_factory_reset_key_action = KEY_FACTORY_RESET_AND_POWEROFF;
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_FACTORY_RESET_REQUEST, NULL, 0,
                                NULL, 0);
            ret = true;
            break;

        case KEY_RESET_LINK_KEY:
            /* Clear link key then reboot, only for Agent. */
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive key to do reset link key", 0);
            app_home_screen_fact_rst_link_key_nvdm_flag();
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action)) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Fail to send KEY_RESET_LINK_KEY to partner", 0);
                }
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_REQUEST_REBOOT, NULL, 0,
                                    NULL, 100);
            } else
#endif
            {
                _trigger_reboot_flow(self, 0);
            }
            ret = true;
            break;
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    case KEY_ULL_AIR_PAIRING: {
#ifdef MTK_AWS_MCE_ENABLE
        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
            if (local_context->aws_connected) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, KEY_ULL_AIR_PAIRING)) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_ULL_AIR_PAIRING aws to agent failed", 0);
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                } else {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner send KEY_ULL_AIR_PAIRING aws to agent success", 0);
                }
            } else {
                apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Partner aws disconnected for KEY_ULL_AIR_PAIRING", 0);
            }
        } else if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
        {
            bt_ull_pairing_info_t air_pairing_data = {
                    .duration = APPS_AIR_PAIRING_DURATION,
                    .role = BT_ULL_ROLE_CLIENT,
                    .key = APPS_AIR_PAIRING_KEY,
                    .info = APPS_AIR_PAIRING_INFO,
                    .rssi_threshold = AIR_PAIRING_RSSI_THRESHOLD,
            };
            if (BT_STATUS_SUCCESS == bt_ull_action(BT_ULL_ACTION_START_PAIRING, &air_pairing_data, sizeof(air_pairing_data))) {
                app_bt_state_service_set_ull_air_pairing_doing(true);
                apps_config_set_vp(VP_INDEX_PAIRING, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                apps_config_set_foreground_led_pattern(LED_INDEX_AIR_PAIRING, APPS_AIR_PAIRING_DURATION * 10, false);
            } else {
                apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
        }
        ret = true;
        break;
    }
#endif
#ifdef MTK_AWS_MCE_ENABLE
        case KEY_AIR_PAIRING: {
#ifdef AIR_LE_AUDIO_BIS_ENABLE
            uint8_t zero_addr[6] = {0};
            bt_bd_addr_t *bd_addr = bt_device_manager_aws_local_info_get_peer_address();
            uint8_t *peer_addr = (uint8_t *)(*bd_addr);
            if (peer_addr != NULL && memcmp(peer_addr, zero_addr, 6) != 0) {
                app_le_audio_forward_scan_action();
                ret = true;
                break;
            } else {
                APPS_LOG_MSGID_I("[LEA][BIS] continue air_pairing", 0);
            }
#endif
            if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
                && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
                    && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Do not trigger air pairing when test mode", 0);
            } else {
                /* Start AIR pairing and play "pairing" VP and "AIR pairing" LED. */
                bt_aws_mce_srv_air_pairing_t air_pairing_data = {
                    .duration = APPS_AIR_PAIRING_DURATION,
                    .default_role = 0,
                    .air_pairing_key = APPS_AIR_PAIRING_KEY,
                    .air_pairing_info = APPS_AIR_PAIRING_INFO,
                    .rssi_threshold = AIR_PAIRING_RSSI_THRESHOLD,
                    .audio_ch = ami_get_audio_channel(),
                };
                if (BT_STATUS_SUCCESS == bt_aws_mce_srv_air_pairing_start(&air_pairing_data)) {
                    app_bt_state_service_set_air_pairing_doing(true);
                    apps_config_set_vp(VP_INDEX_PAIRING, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    apps_config_set_foreground_led_pattern(LED_INDEX_AIR_PAIRING, APPS_AIR_PAIRING_DURATION * 10, false);
                } else {
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                }
            }
            ret = true;
            break;
        }
#endif
#ifdef APPS_TRIGGER_RHO_BY_KEY
        case KEY_RHO_TO_AGENT: {
            /* If support TRIGGER_RHO_BY_KEY and disable auto RHO feature, Partner could send "KEY_RHO_TO_AGENT" event to Agent. */
            if (!apps_config_features_is_auto_rho_enabled()) {
                if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                    if (local_context->aws_connected
                        && BT_STATUS_SUCCESS == apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, KEY_RHO_TO_AGENT)) {
                        apps_config_set_vp(VP_INDEX_PRESS, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        apps_config_set_foreground_led_pattern(LED_INDEX_TRIGGER_RHO, 30, false);
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Send RHO request to agent", 0);
                    } else {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Current AWS not connected", 0);
                        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    }
                    local_context->key_trigger_waiting_rho = true;
                } else {
                    apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    apps_config_set_foreground_led_pattern(LED_INDEX_TRIGGER_RHO, 30, false);
                }
                ret = true;
            }
            break;
        }
#endif
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
        case KEY_ADVANCED_PASSTHROUGH_SWITCH: {
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" [ADVANCE_PT] [%02X] switch via KEY", 1, role);
#ifdef MTK_AWS_MCE_ENABLE
            if (role == BT_AWS_MCE_ROLE_PARTNER) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action)) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", [ADVANCE_PT] sync KEY_ADVANCED_PASSTHROUGH_SWITCH fail.", 0);
                    //apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                }
            } else
#endif
            {
                app_advance_passthrough_switch();
            }
            break;
        }
#endif
        case KEY_TEST_MODE_ENTER_DUT_MODE: {
            /* Read BT_DUT_ENABLE flag from NVKEY -> inverse value -> write to NVKEY -> reboot. */
            bool dut_config = false;
            uint32_t dut_size = sizeof(dut_config);
            nvkey_status_t nvkey_ret = nvkey_read_data(NVKEYID_BT_DUT_ENABLE, (uint8_t *)(&dut_config), &dut_size);
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"ENTER_DUT_MODE nvkey_read:%d, dut_config:%d\r\n", 2, nvkey_ret, dut_config);
            if (NVKEY_STATUS_OK == nvkey_ret) {
                dut_config = !dut_config;
            } else {
                dut_config = true;
            }
            dut_size = sizeof(dut_config);
            nvkey_ret = nvkey_write_data(NVKEYID_BT_DUT_ENABLE, (uint8_t *)(&dut_config), dut_size);
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"ENTER_DUT_MODE nvkey_write:%d", 1, nvkey_ret);
            _trigger_reboot_flow(self, 0);
        }
        break;
        case KEY_TEST_MODE_ENTER_RELAY_MODE: {
            /* Read BT_RELAY_ENABLE flag from NVKEY -> inverse value -> write to NVKEY -> reboot. */
            bt_power_on_relay_config_t relay_config = {
                .relay_enable = false,
                .port_number = 0,
            };
            uint32_t relay_size = sizeof(relay_config);
            nvkey_status_t nvkey_ret = nvkey_read_data(NVKEYID_BT_RELAY_ENABLE, (uint8_t *)(&relay_config), &relay_size);
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"ENTER_RELAY_MODE nvkey_read:%d, relay_enable:%d, port_number:%d\r\n", 3, nvkey_ret, relay_config.relay_enable, relay_config.port_number);
            if (NVKEY_STATUS_OK == nvkey_ret) {
                relay_config.relay_enable = !relay_config.relay_enable;
            } else {
                relay_config.relay_enable = true;
                relay_config.port_number = 0;
            }
            relay_size = sizeof(relay_config);
            nvkey_ret = nvkey_write_data(NVKEYID_BT_RELAY_ENABLE, (uint8_t *)(&relay_config), relay_size);
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"ENTER_RELAY_MODE nvkey_write:%d", 3, nvkey_ret);
            _trigger_reboot_flow(self, 0);
        }
        break;
    }
    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool homescreen_app_aws_data_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    ret = bt_conn_component_aws_data_proc(self, event_id, extra_data, data_len);

    if (ret) {
        return ret;
    }

    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t aws_event_id;
        apps_aws_sync_event_decode(aws_data_ind, &event_group, &aws_event_id);
        switch (event_group) {
            case EVENT_GROUP_UI_SHELL_KEY:
                switch (aws_event_id) {
#ifdef APPS_TRIGGER_RHO_BY_KEY
                    case KEY_RHO_TO_AGENT:
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Received event = %x to do RHO", 1, aws_event_id);
                            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                APPS_EVENTS_INTERACTION_TRIGGER_RHO, NULL, 0,
                                                NULL, 0);
                        }
                        ret = true;
                        break;
#endif
                    case KEY_DISCOVERABLE:
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Received event = %x to do discover", 1, aws_event_id);
#ifdef AIR_SWIFT_PAIR_ENABLE
                            app_swift_pair_start_adv();
#endif
                            app_bt_state_service_set_bt_visible(true, false, VISIBLE_TIMEOUT);
                            apps_config_set_vp(VP_INDEX_PAIRING, true, 100,
                                               VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        }
                        ret = true;
                        break;
                    case KEY_CANCEL_DISCOVERABLE:
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", set visible to false.", 0);
                        app_bt_state_service_set_bt_visible(false, false, 0);
                        break;
                    case KEY_RECONNECT_LAST_DEVICE:
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                            bt_bd_addr_t *p_bd_addr = bt_device_manager_remote_get_dev_by_seq_num(1);
                            bt_cm_connect_t connect_param;
                            connect_param.profile = bt_customer_config_app_get_cm_config()->power_on_reconnect_profile
                                                    & ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
                            if (p_bd_addr) {
                                memcpy(connect_param.address, *p_bd_addr, sizeof(bt_bd_addr_t));
                                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Agent received KEY_RECONNECT_LAST_DEVICE from Partner", 0);
                                bt_cm_connect(&connect_param);
                                apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0,
                                                   VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                            }
                        }
                        ret = true;
                        break;
                    case KEY_RESET_PAIRED_DEVICES:
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Agent received KEY_RESET_PAIRED_DEVICES from Partner", 0);
                            bt_device_manager_unpair_all();
                            apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0,
                                               VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        }
                        ret = true;
                        break;
#ifdef MTK_ANC_ENABLE
                    case KEY_PASS_THROUGH:
                    case KEY_ANC:
                    case KEY_SWITCH_ANC_AND_PASSTHROUGH:
                    case KEY_BETWEEN_ANC_PASSTHROUGH:
                        ret = app_home_screen_process_anc_and_pass_through(self, aws_event_id);
                        break;
#endif
                    case KEY_FACTORY_RESET:
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive aws KEY_FACTORY_RESET", 0);
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                            apps_config_set_vp(VP_INDEX_SUCCESSED, true, 100, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
                        }
                        s_factory_reset_key_action = KEY_FACTORY_RESET;
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_FACTORY_RESET_REQUEST, NULL, 0,
                                            NULL, 0);
                        ret = true;
                        break;
                    case KEY_FACTORY_RESET_AND_POWEROFF:
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive aws KEY_FACTORY_RESET_AND_POWEROFF", 0);
                        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                            apps_config_set_vp(VP_INDEX_SUCCESSED, true, 100, VOICE_PROMPT_PRIO_EXTREME, false, NULL);
                        }
                        s_factory_reset_key_action = KEY_FACTORY_RESET_AND_POWEROFF;
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_FACTORY_RESET_REQUEST, NULL, 0,
                                            NULL, 0);
                        ret = true;
                        break;
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
                    case KEY_ULL_AIR_PAIRING: {
                        bt_ull_pairing_info_t air_pairing_data = {
                                .duration = APPS_AIR_PAIRING_DURATION,
                                .role = BT_ULL_ROLE_CLIENT,
                                .key = APPS_AIR_PAIRING_KEY,
                                .info = APPS_AIR_PAIRING_INFO,
                                .rssi_threshold = AIR_PAIRING_RSSI_THRESHOLD,
                        };
                        if (BT_STATUS_SUCCESS == bt_ull_action(BT_ULL_ACTION_START_PAIRING, &air_pairing_data, sizeof(air_pairing_data))) {
                            app_bt_state_service_set_ull_air_pairing_doing(true);
                            apps_config_set_vp(VP_INDEX_PAIRING, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                            apps_config_set_foreground_led_pattern(LED_INDEX_AIR_PAIRING, APPS_AIR_PAIRING_DURATION * 10, false);
                        } else {
                            apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        }
                        break;
                    }
#endif
                    case KEY_RESET_LINK_KEY:
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive aws KEY_RESET_LINK_KEY", 0);
                        app_home_screen_fact_rst_link_key_nvdm_flag();
                        _trigger_reboot_flow(self, 0);
                        ret = true;
                        break;
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
                    case KEY_ADVANCED_PASSTHROUGH_SWITCH: {
                        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY" [ADVANCE_PT] [%02X] switch via AWS-KEY",
                                         1, role);
                        if (role == BT_AWS_MCE_ROLE_AGENT) {
                            app_advance_passthrough_switch();
                        }
                        break;
                    }
#endif
                    default:
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Received unprocessed event = %x=%x", 2, event_group, aws_event_id);
                        break;
                }
                break;
#ifdef MTK_ANC_ENABLE
            case EVENT_GROUP_UI_SHELL_AUDIO_ANC:
                switch (aws_event_id) {
                    case AUDIO_ANC_CONTROL_EVENT_HOWLING:
#ifdef MTK_ANC_HOWLING_TURN_OFF_ANC
                        app_home_screen_disable_anc_when_howling(self);
#endif
                        break;
                    default:
                        break;
                }
                break;
#endif
            case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
                switch (aws_event_id) {
                    case APPS_EVENTS_INTERACTION_AWS_SYNC_REBOOT: {
                        if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT &&
                            BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
                            s_sync_reboot_waiting = true;
                        }
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Received AWS data, AWS_SYNC_REBOOT, s_sync_reboot_waiting = %d", 1, s_sync_reboot_waiting);
                        _trigger_reboot_flow(self, 0);
                        break;
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    return ret;
}
#endif

static bool homescreen_app_bt_sink_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", BT event = %x", 1, event_id);

    ret = bt_conn_component_bt_sink_event_proc(self, event_id, extra_data, data_len);

    return ret;
}

static apps_config_state_t homescreen_app_get_mmi_state(ui_shell_activity_t *self)
{
    home_screen_local_context_type_t *local_context = (home_screen_local_context_type_t *)(self->local_context);
    apps_config_state_t mmi_state = APP_BT_OFF;

    if (!local_context) {
        return mmi_state;
    }
    if (APP_HOME_SCREEN_BT_POWER_STATE_DISABLED == local_context->bt_power_state) {
        mmi_state = APP_BT_OFF;
    } else {
#ifdef AIR_LE_AUDIO_ENABLE
        const app_bt_state_service_status_t *bt_service = app_bt_connection_service_get_current_status();
        if (local_context->connection_state || bt_service->ble_audio_state >= APP_BLE_AUDIO_STATE_CONNECTED) {
#else
        if (local_context->connection_state) {
#endif
            mmi_state = APP_CONNECTED;
        } else {
            mmi_state = local_context->is_bt_visiable ? APP_CONNECTABLE : APP_DISCONNECTED;
        }
    }

    return mmi_state;
}

static bool homescreen_app_bt_connection_manager_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    home_screen_local_context_type_t *local_context = (home_screen_local_context_type_t *)(self->local_context);
    if (!local_context) {
        return ret;
    }

#if defined(APPS_AUTO_SET_BT_DISCOVERABLE) && defined (MTK_AWS_MCE_ENABLE)
    apps_config_state_t old_mmi_state;
    old_mmi_state = homescreen_app_get_mmi_state(self);
#endif

    ret = bt_conn_component_bt_cm_event_proc(self, event_id, extra_data, data_len);

#if APPS_AUTO_SET_BT_DISCOVERABLE
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    bool disconnect = false;
    bool connect = false;
    bool bt_off = false;
    bool bt_on = false;
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (!remote_update) {
                break;
            }
#ifdef MTK_AWS_MCE_ENABLE
            if (role & (BT_AWS_MCE_ROLE_AGENT | BT_AWS_MCE_ROLE_NONE)) {
#endif
                if (0 == memcmp(remote_update->address, bt_device_manager_get_local_address(), sizeof(bt_bd_addr_t))) {
                    break;
                }
                if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                    && BT_CM_ACL_LINK_PENDING_CONNECT != remote_update->pre_acl_state
                    && BT_CM_ACL_LINK_CONNECTING != remote_update->pre_acl_state
                    && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state) {
                    bt_bd_addr_t connected_devices[2];
                    uint32_t connected_devices_count = 2;
                    uint32_t i;
                    bool still_connected = false;
                    connected_devices_count = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, connected_devices, connected_devices_count);
                    for (i = 0; i < connected_devices_count; i++) {
                        if (0 != memcmp(connected_devices[i], bt_device_manager_get_local_address(), sizeof(bt_bd_addr_t))) {
                            still_connected = true;
                            break;
                        }
                    }
                    if (!still_connected) {
                        /* Check all Smart phones are disconnected */
                        disconnect = true;
                    } else {
                        APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", ACL disconnected but still have SP connected", 0);
                    }
                } else if (BT_CM_ACL_LINK_CONNECTED != remote_update->pre_acl_state
                    && BT_CM_ACL_LINK_CONNECTED == remote_update->acl_state) {
                    connect = true;
                }

#ifdef MTK_AWS_MCE_ENABLE
            } else {
                /* For partner */
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                        /* Partner Connect SP = AWS connected + LINK_NORMAL */
                        connect = true;
                    }
                } else if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)
                    & (remote_update->pre_connected_service & (~remote_update->connected_service))) {
                    if (APP_CONNECTED == old_mmi_state) {
                        disconnect = true;
                    }
                    if (s_sync_reboot_waiting) {
                        /* Sync reboot feature, partner must wait agent power off first. */
                        s_sync_reboot_waiting = false;
                        app_bt_state_service_set_bt_on_off(false, false, false, true);
                    }
                }
            }
#endif
            break;
        }
        case BT_CM_EVENT_POWER_STATE_UPDATE: {
            bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)extra_data;
            if (!power_update) {
                break;
            }
            if (power_update && BT_CM_POWER_STATE_ON == power_update->power_state) {
                bt_on = true;
#ifdef AIR_LE_AUDIO_ENABLE
                if (s_le_audio_muted) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY"[LEA] unmute LE Audio", 0);
                    bt_sink_srv_le_volume_set_mute(BT_SINK_SRV_LE_STREAM_TYPE_OUT, false);
                    s_le_audio_muted = false;
                }
#endif
            } else if (power_update && BT_CM_POWER_STATE_OFF == power_update->power_state) {
                bt_off = true;
            }
            break;
        }
        default:
            break;
    }
    /* Disconnected from Smart phone, set the flag to prepare start BT discoverable. */
    /* If user refused pairing on Smart phone, must restart discoverable. */
    if (bt_on || disconnect) {
        if (bt_device_manager_remote_get_paired_num() == 0) {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", start visible when power on if paired_num is 0", 0);
            app_bt_state_service_set_bt_visible(true, true, VISIBLE_TIMEOUT);
        } else if (bt_on) {
            s_reconnect_device_idx = 1;
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", delay to start visible if paired_num > 0", 0);
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_RECONNECT_DEVICE);
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_AUTO_START_BT_VISIBLE);
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_RECONNECT_TIMEOUT);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_BT_RECONNECT_DEVICE, NULL, 0,
                                NULL, TIME_TO_RECONNECT_DEVICE);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_AUTO_START_BT_VISIBLE, NULL, 0,
                                NULL, TIME_TO_START_VISIBLE_AFTER_POWER_ON);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_BT_RECONNECT_TIMEOUT, NULL, 0,
                                NULL, TIME_TO_STOP_RECONNECTION + TIME_TO_START_VISIBLE_AFTER_POWER_ON);
        } else {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", start reconnect timer", 0);
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_RECONNECT_TIMEOUT);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_BT_RECONNECT_TIMEOUT, NULL, 0,
                                NULL, TIME_TO_STOP_RECONNECTION);
        }
    } else if (bt_off || connect) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_RECONNECT_DEVICE);
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_AUTO_START_BT_VISIBLE);
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_RECONNECT_TIMEOUT);
    }
#endif

    app_home_screen_check_and_do_power_off(local_context);
    return ret;
}

static bool homescreen_app_bt_dm_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bt_device_manager_power_event_t evt;
    bt_device_manager_power_status_t status;
    bt_event_get_bt_dm_event_and_status(event_id, &evt, &status);

    switch (evt) {
        case BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE:
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
                if (s_reboot_trigger_sp_reconnect) {
                    bt_cm_connect_t param;
                    bt_device_manager_paired_infomation_t paired_info[1];
                    uint32_t paired_info_count = 1;
                    bt_device_manager_get_paired_list(paired_info, &paired_info_count);
                    if (paired_info_count > 0) {
                        param.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP) | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK);
                        memcpy(param.address, paired_info[0].address, sizeof(bt_bd_addr_t));
                        bt_cm_connect(&param);
                    }
                    s_reboot_trigger_sp_reconnect = false;
                }
#endif
            }
            break;
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                app_home_screen_check_and_do_power_off(self->local_context);
            }
            break;
        }
        default:
            break;
    }

    return ret;
}

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
static bool homescreen_app_ull_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    switch (event_id) {
        case BT_ULL_EVENT_PAIRING_COMPLETE_IND:
            app_bt_state_service_set_ull_air_pairing_doing(false);
            break;
        default:
            break;
    }
    return ret;
}
#endif

static bool homescreen_app_aws_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    switch (event_id) {
#ifdef MTK_AWS_MCE_ENABLE
        case BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE: {
            if (extra_data) {
                bt_aws_mce_srv_air_pairing_complete_ind_t *air_pairing_ind = (bt_aws_mce_srv_air_pairing_complete_ind_t *)extra_data;
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Received BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE result= %d", 1, air_pairing_ind->result);
                app_bt_state_service_set_air_pairing_doing(false);
                if (air_pairing_ind->result) {
#if APPS_AUTO_SET_BT_DISCOVERABLE
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", set flag auto_start_visiable when air pairing successfully", 0);
                    app_bt_state_service_set_bt_visible(true, true, VISIBLE_TIMEOUT);
#endif
                    apps_config_set_foreground_led_pattern(LED_INDEX_AIR_PAIRING_SUCCESS, 30, false);
                    apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                } else {
                    apps_config_set_foreground_led_pattern(LED_INDEX_AIR_PAIRING_FAIL, 30, false);
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                }
            }
            break;
        }
#endif
        default:
            break;
    }
    return ret;
}

static bool _app_interaction_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    home_screen_local_context_type_t *local_ctx = (home_screen_local_context_type_t *)self->local_context;
    bool ret = false;
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    switch (event_id) {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        case APPS_EVENTS_INTERACTION_RHO_STARTED: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", homescreen received RHO started, role=0x%02X", 1, role);
        }
        break;
        case APPS_EVENTS_INTERACTION_RHO_END:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", homescreen received RHO end", 0);
            break;
        case APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT:
#ifdef APPS_TRIGGER_RHO_BY_KEY
            if (!apps_config_features_is_auto_rho_enabled()) {
                /* Partner should play VP when switch to Agent via KEY_RHO_TO_AGENT. */
                if (local_ctx->key_trigger_waiting_rho) {
                    app_rho_result_t result = (app_rho_result_t)extra_data;
                    if (APP_RHO_RESULT_SUCCESS == result) {
                        apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    } else {
                        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    }
                }
            }
#endif
            break;
#endif
        case APPS_EVENTS_INTERACTION_POWER_OFF_WAIT_TIMEOUT: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Timeout before power off", 0);
            /* Check and do power off or reboot if WAIT_TIME_BEFORE_POWER_OFF timeout. */
            if (APP_HOME_SCREEN_STATE_IDLE != local_ctx->state) {
                local_ctx->power_off_waiting_time_out = false;
                app_home_screen_check_and_do_power_off(local_ctx);
                ret = true;
            }
        }
        break;
        case APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF: {
            bool need_rho = true;
            if (extra_data) {
                need_rho = *((bool *)extra_data);
            }
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive power off request", 0);
            if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
                && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
                    && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Do not trigger power off when test mode", 0);
            } else {
                /* Should wait power off timeout for INTERACTION_REQUEST_POWER_OFF event. */
                _trigger_power_off_flow(self, true, need_rho);
            }
            ret = true;
            break;
        }
        case APPS_EVENTS_INTERACTION_REQUEST_IMMEDIATELY_POWER_OFF: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive immediately power off request", 0);
            if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
                && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
                    && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Do not trigger power off when test mode", 0);
            } else {
                _trigger_power_off_flow(self, false, true);
            }
            ret = true;
            break;
        }
        case APPS_EVENTS_INTERACTION_REQUEST_REBOOT: {
            uint32_t delay_time = (uint32_t)extra_data;
            bool fota_flag = false;
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive sys reboot request after:%d", 1, delay_time);
            if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
                && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
                    && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Do not trigger power off when test mode", 0);
            } else {
                fota_check_upgrade_flag(&fota_flag);
                if (fota_flag) {
                    reserved_nvdm_item_list_ask_check();
                }
                _trigger_reboot_flow(self, delay_time);
            }
            ret = true;
            break;
        }
        case APPS_EVENTS_INTERACTION_REQUEST_ON_OFF_BT: {
            /* Enable or disable BT via bt_state_service API. */
            bool enable_bt = (bool)extra_data;
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive BT on/off request : %d", 1, enable_bt);
            if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
                && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
                    && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Do not trigger BT on/off when test mode", 0);
            } else {
                if (!enable_bt) {
                    local_ctx->bt_power_off = true;
                }
                app_bt_state_service_set_bt_on_off(enable_bt, false, true, false);
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_REQUEST_CLASSIC_BT_OFF: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive Classic off", 0);
            if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
                && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
                    && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Do not trigger BT on/off when test mode", 0);
            } else {
                local_ctx->bt_power_off = true;
                app_bt_state_service_set_bt_on_off(false, true, true, false);
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_CLASSIC_OFF_TO_BT_OFF: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", CLASSIC_OFF_TO_BT_OFF request : %d", 1, local_ctx->target_bt_power_state);
            if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
                && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
                    && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Do not trigger BT on/off when test mode", 0);
            } else {
                local_ctx->bt_power_off = true;
                app_bt_state_service_set_bt_on_off(false, false, true, false);
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN:
            ret = true;
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Connection state = %x, is_bt_visiable = %d", 2, local_ctx->connection_state, local_ctx->is_bt_visiable);
            if (local_ctx->is_bt_visiable) {
                apps_config_set_background_led_pattern(LED_INDEX_CONNECTABLE, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
            } else {
                apps_config_state_t app_state = homescreen_app_get_mmi_state(self);
                switch (app_state) {
                    case APP_BT_OFF:
                        apps_config_set_background_led_pattern(LED_INDEX_IDLE, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOWEST);
                        break;
                    case APP_DISCONNECTED:
                        apps_config_set_background_led_pattern(LED_INDEX_DISCONNECTED, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOW);
                        break;
                    case APP_CONNECTED:
#ifdef MTK_AWS_MCE_ENABLE
                        if (role == BT_AWS_MCE_ROLE_PARTNER || role == BT_AWS_MCE_ROLE_CLINET) {
                            if (local_ctx->aws_connected) {
                                apps_config_set_background_led_pattern(LED_INDEX_IDLE, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOWEST);
                            } else {
                                apps_config_set_background_led_pattern(LED_INDEX_DISCONNECTED, true, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOW);
                            }
                        } else
#endif
                        {
                            apps_config_set_background_led_pattern(LED_INDEX_IDLE, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOWEST);
                        }
                        break;
                    default:
                        ret = false;
                        APPS_LOG_MSGID_W(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Not supported state = %x", 1, local_ctx->connection_state);
                        break;
                }
            }
            break;
        case APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE:
            ret = true;
            apps_config_key_set_mmi_state(homescreen_app_get_mmi_state(self));
            break;

        case APPS_EVENTS_INTERACTION_AUTO_START_BT_VISIBLE:
#if defined(AIR_LE_AUDIO_ENABLE) && (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))
            if (!app_bt_conn_manager_allow_edr_visible()) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", disallow KEY_DISCOVERABLE", 0);
                ret = true;
                break;
            }
#endif
            /* Update BT visibility via bt_state_service API. */
            app_bt_state_service_set_bt_visible(true, true, VISIBLE_TIMEOUT);
            ret = true;
            break;
        case APPS_EVENTS_INTERACTION_MULTI_VA_REMOVE_PAIRING_DONE:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", MULTI_VA_REMOVE_PAIRING_DONE, next = %x", 1, s_factory_reset_pending_event);
            if (s_factory_reset_pending_event != KEY_ACTION_INVALID) {
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_FACTORY_RESET_REQUEST, NULL, 0,
                                    NULL, 0);
            }
            break;
        case APPS_EVENTS_INTERACTION_BT_RECONNECT_DEVICE: {
            // get the addr and try to reconnect it
#ifdef MTK_AWS_MCE_ENABLE
            if((!local_ctx->aws_connected) || (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT)) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", try reconnect paired device failed, aws:%d, role:%d", 2,
                    local_ctx->aws_connected, bt_device_manager_aws_local_info_get_role());
                break;
            }
#endif
            uint32_t read_nums = RECONNECT_DEVICE_NUMS;
            static bt_device_manager_paired_infomation_t info[RECONNECT_DEVICE_NUMS];
            bt_device_manager_get_paired_list(info, &read_nums);
            if (s_reconnect_device_idx < read_nums && s_reconnect_device_idx > 0) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", try reconnect the device: %d", 1, s_reconnect_device_idx);
                bt_cm_connect_t param;
                memcpy(&param.address, &info[s_reconnect_device_idx - 1].address, sizeof(bt_bd_addr_t));
                param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                bt_status_t sta = bt_cm_disconnect(&param);
                if (sta != BT_STATUS_SUCCESS) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", cancle reconnection with device: %d failed:%d.", 2,
                        s_reconnect_device_idx - 1, sta);
                }
                memcpy(&param.address, &info[s_reconnect_device_idx].address, sizeof(bt_bd_addr_t));
                param.profile = bt_customer_config_app_get_cm_config()->power_on_reconnect_profile
                                & ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
                sta = bt_cm_connect(&param);
                if (sta != BT_STATUS_SUCCESS) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", try reconnect the device: %d failed.", 1, s_reconnect_device_idx);
                }
            } else {
                break;
            }

            s_reconnect_device_idx++;
            if (s_reconnect_device_idx < RECONNECT_DEVICE_NUMS) {
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_RECONNECT_DEVICE);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_BT_RECONNECT_DEVICE, NULL, 0,
                                    NULL, TIME_TO_RECONNECT_DEVICE);
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_BT_RECONNECT_TIMEOUT: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", stop reconnect when time out.", 0);
            bt_bd_addr_t p_bd_addr[2];
            uint32_t connecting_number = 2;
            uint32_t i;
            connecting_number = bt_cm_get_connecting_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), p_bd_addr, connecting_number);
            bt_cm_connect_t connect_param = { {0},
                ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))
            };
            for (i = 0; i < connecting_number; i++) {
                if (0 == bt_cm_get_gap_handle(p_bd_addr[i])) {
                    memcpy(connect_param.address, p_bd_addr[i], sizeof(bt_bd_addr_t));
                    bt_cm_disconnect(&connect_param);
                }
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_FACTORY_RESET_REQUEST: {
            uint32_t action_after_factory_reset;
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive factory reset request, is_doing:%d", 1, s_factory_reset_doing);
            if (MULTI_VA_SWITCH_OFF_WAIT_INACTIVE == multi_voice_assistant_manager_va_remove_pairing()) {
                s_factory_reset_pending_event = s_factory_reset_key_action;
                break;
            } else {
                s_factory_reset_pending_event = KEY_ACTION_INVALID;
            }

            if (s_factory_reset_doing) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", factory reset already started.", 0);
                break;
            }
            s_factory_reset_doing = true;

            if (s_factory_reset_key_action == KEY_FACTORY_RESET) {
                action_after_factory_reset = APPS_EVENTS_INTERACTION_REQUEST_REBOOT;
            } else {
                action_after_factory_reset = APPS_EVENTS_INTERACTION_REQUEST_POWER_OFF;
            }

#ifdef MTK_AWS_MCE_ENABLE
            if (!local_ctx->aws_connected) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", factory reset failed, aws not connected.", 0);
                apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                s_factory_reset_doing = false;
                break;
            }

            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, s_factory_reset_key_action)) {
                    APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Fail to send KEY_FACTORY_RESET_AND_POWEROFF to partner", 0);
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    s_factory_reset_doing = false;
                    break;
                }
            }

            app_home_screen_fact_rst_nvdm_flag();
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                apps_config_set_vp(VP_INDEX_POWER_OFF, true, 1000, VOICE_PROMPT_PRIO_EXTREME, false, s_factory_reset_key_action == KEY_FACTORY_RESET ? NULL: app_home_screen_power_off_vp_play_callback);
            }
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                action_after_factory_reset, NULL, 0,
                                NULL, s_factory_reset_key_action == KEY_FACTORY_RESET ? 4500 : 3500);
#else
            app_home_screen_fact_rst_nvdm_flag();
            apps_config_set_vp(VP_INDEX_POWER_OFF, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, app_home_screen_power_off_vp_play_callback);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                action_after_factory_reset, NULL, 0,
                                NULL, s_factory_reset_key_action == KEY_FACTORY_RESET ? 1500 : 0);
#endif
        }
        break;
        case APPS_EVENTS_INTERACTION_AWS_SYNC_REBOOT: {
            uint32_t delay_time = 0;
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive _AWS_SYNC_REBOOT", 0);
#if defined(MTK_AWS_MCE_ENABLE)
            bt_status_t send_ret = BT_STATUS_SUCCESS;
            if (BT_AWS_MCE_ROLE_AGENT == role) {
                send_ret = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                        APPS_EVENTS_INTERACTION_AWS_SYNC_REBOOT, NULL, 0);
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive _AWS_SYNC_REBOOT, aws send_ret = %x", 1, send_ret);
            } else if ((BT_AWS_MCE_ROLE_PARTNER | BT_AWS_MCE_ROLE_CLINET) & role) {
                APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Receive _AWS_SYNC_REBOOT, ignore when role = %d", 1, role);
                break;
            }
            if (BT_STATUS_SUCCESS == send_ret) {
                delay_time = 100;
            }
#endif
            _trigger_reboot_flow(self, delay_time);
            ret = true;
            break;
        }
        default:
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Not supported event id = %d", 1, event_id);
            break;
    }

    return ret;
}

static bool homescreen_app_audio_anc_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
#ifdef MTK_ANC_HOWLING_TURN_OFF_ANC
        case ANC_CONTROL_EVENT_HOWLING:
            app_home_screen_disable_anc_when_howling(self);
            break;
#endif
        default:
            break;
    }
    return ret;
}

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
static bool homescreen_app_dual_chip_race_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
        case APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_POWER_OFF_READY: 
        case APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_REBOOT_READY: {
            APPS_LOG_MSGID_I(UI_SHELL_IDLE_BT_CONN_ACTIVITY", Received DUAL_CHIP_POWER_OFF_READY : event: 0x%x, current state: 0x%x", 2,
                             event_id, s_app_homescreen_context.waiting_dual_slave_state);
            if (s_app_homescreen_context.waiting_dual_slave_state == APP_HOME_SCREEN_STATE_POWERING_OFF
                || s_app_homescreen_context.waiting_dual_slave_state == APP_HOME_SCREEN_STATE_REBOOT) {
                s_app_homescreen_context.waiting_dual_slave_state = APP_HOME_SCREEN_STATE_IDLE;
            }
            app_home_screen_check_and_do_power_off(&s_app_homescreen_context);
            break;
        }
#endif
        default:
            break;
    }
    return ret;
}
#endif /* #if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE) */

bool app_home_screen_idle_activity_proc(ui_shell_activity_t *self,
                                        uint32_t event_group,
                                        uint32_t event_id,
                                        void *extra_data,
                                        size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell key events. */
        case EVENT_GROUP_UI_SHELL_KEY: {
            ret = _proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_SINK: {
            ret = homescreen_app_bt_sink_event_proc(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell BT Connection Manager events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            ret = homescreen_app_bt_connection_manager_event_proc(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell BT DM events. */
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER: {
            ret = homescreen_app_bt_dm_event_proc(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell ultra low latency events. */
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
        case EVENT_GROUP_BT_ULTRA_LOW_LATENCY:
            ret = homescreen_app_ull_event_proc(self, event_id, extra_data, data_len);
            break;
#endif
        /* UI Shell BT AWS events. */
        case EVENT_GROUP_UI_SHELL_AWS: {
            ret = homescreen_app_aws_event_proc(self, event_id, extra_data, data_len);
            break;
        }
        /* UI Shell APP_INTERACTION events. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = _app_interaction_event_proc(self, event_id, extra_data, data_len);
            break;
#if defined(MTK_AWS_MCE_ENABLE)
        /* UI Shell AWS DATA events. */
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = homescreen_app_aws_data_proc(self, event_id, extra_data, data_len);
            break;
#endif
        /* UI Shell AUDIO ANC events. */
        case EVENT_GROUP_UI_SHELL_AUDIO_ANC:
            ret = homescreen_app_audio_anc_event_proc(self, event_id, extra_data, data_len);
            break;
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        case EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD:
            ret = homescreen_app_dual_chip_race_event_proc(self, event_id, extra_data, data_len);
            break;
#endif
        default:
            break;
    }
    return ret;
}

bool app_home_screen_idle_activity_is_aws_connected(void)
{
    return s_app_homescreen_context.aws_connected;
}
void app_home_screen_power_off_vp_play_callback(uint32_t idx, vp_err_code err)
{
    if (VP_ERR_CODE_SUCCESS == err && s_app_homescreen_context.power_off_waiting_time_out) {
        bt_sink_srv_ami_audio_power_off_dac_immediately();
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_POWER_OFF_WAIT_TIMEOUT);
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_POWER_OFF_WAIT_TIMEOUT, NULL, 0,
                            NULL, 0);
    }
}
