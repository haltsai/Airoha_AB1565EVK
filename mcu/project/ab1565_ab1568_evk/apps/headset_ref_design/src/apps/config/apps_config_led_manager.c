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
 * File: apps_config_led_manager.c
 *
 * Description: This file support API to set LED pattern.
 * Note: The BG pattern, short form background pattern have lower priority than the FG pattern, short form foreground pattern.
 *
 */

#include "apps_config_led_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "ui_shell_manager.h"
#include "ui_shell_activity.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "bt_sink_srv.h"
#include "bt_aws_mce_srv.h"
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
#include "apps_race_cmd_co_sys_event.h"
#endif

#include "app_led_control.h"
#include "apps_debug.h"
#include "FreeRTOS.h"
#include "timers.h"
#include "semphr.h"
#include <limits.h>

#define LOG_TAG     "[LED_MANAGER] "        /* Log tag. */
#define LED_FG_TIMER_NAME       "LED_FG"    /* The timer name. */
#define LED_FG_TIMEOUT_UNIT_MS  (100)       /* The unit value of the timeout parameter in the API apps_config_set_foreground_led_pattern(). */

#define LED_PATTERN_DEFAULT_SYNC_TIME   (600 * 1000)    /* The delay time of sync LED pattern to partner need. */
#define LED_PATTERN_INVALID_INDEX   (0xFF)              /* The invalid LED pattern index. */
#define LED_SUPPORT_TEMPORARY_DISABLE_FG    (1)         /* When it's 0, the apps_config_led_temporary_disable() only disables BG pattern. */

static bool fg_display = false;                 /* The flag to indicate FG pattern is active. */
static TimerHandle_t s_timer_handle = NULL;     /* The pointer to the time hander. */

static uint8_t s_playing_bg_index = LED_PATTERN_INVALID_INDEX;  /* Current playing BG pattern. */
static uint8_t s_playing_fg_index = LED_PATTERN_INVALID_INDEX;  /* Current playing FG pattern. */
static bool s_temp_disable_led = false;                         /* In temporary disable LED status. */

#ifdef MTK_AWS_MCE_ENABLE

static uint8_t background_self_index = LED_PATTERN_INVALID_INDEX;   /* The BG pattern set from APPs. */
static uint8_t s_background_self_priority = APPS_CONFIG_LED_AWS_SYNC_PRIO_INVALID;  /* The priority of the BG pattern set from APPs. */

/** @brief
 * This structure defines the variables for LED sync.
 */
static struct {
    bool need_sync_bg;                  /* It means the background pattern need sync to partner. */
    bool background_synced;             /* For agent, means BG sync successfully. For partner, means received BG LED sync data. */
    bool foreground_synced;             /* For agent, means FG sync successfully. For partner, means received FG LED sync data. */
    uint8_t background_sync_index;      /* It's useful when background_synced is true, means the synced background LED pattern index. */
    uint8_t background_sync_priority;   /* It's useful when background_synced is true, means the priority of the synced background LED pattern. */
} s_led_sync_info = {
    false,
    false,
    false,
    LED_PATTERN_INVALID_INDEX,
    APPS_CONFIG_LED_AWS_SYNC_PRIO_INVALID
};

/** @brief
 * This structure defines the aws format of LED sync data.
 */
typedef struct {
    bt_clock_t bt_clock;    /* Target BT clock to play LED pattern. */
    uint16_t timeout;       /* The duration to play FG pattern, only for LED_PATTERN_FG patterns. */
    uint8_t pattern_type;   /* LED_PATTERN_BG or LED_PATTERN_FG. */
    uint8_t pattern_index;  /* The index of the sync pattern. */
    uint8_t sync_priority;  /* The index of the sync pattern, only for LED_PATTERN_BG. */
} apps_config_led_manager_pattern_info_t;

/**
 * @brief      Partner send request to agent for resync BG pattern.
 */
static bt_status_t _partner_send_resync_bg_request(void)
{
    apps_config_led_manager_pattern_info_t pattern_info = {
        .pattern_type = LED_PATTERN_NONE,   /* Pattern_type is NONE means resync request */
    };
    bt_aws_mce_report_info_t mce_repo_info = {
        .module_id = BT_AWS_MCE_REPORT_MODULE_LED,
        .is_sync = false,
        .sync_time = 0,
        .param_len = sizeof(apps_config_led_manager_pattern_info_t),
        .param = &pattern_info
    };
    return bt_aws_mce_report_send_event(&mce_repo_info);
}
#endif

#ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
/**
 * @brief      Set BG LED pattern to lower layer(LED module).
 * @param[in]  index, the LED pattern index, refer to apps_config_led_index_list.h.
 * @param[in]  force_update, if it's false, do not set pattern if the LED pattern index is different from last time.
 * @param[in]  need_delay, if it's true, set pattern till gpt_time, otherwise set the pattern immediately.
 * @param[in]  gpt_time, it's useful when need_delay is true.
 * @return     If the LED pattern is set to LED module, return true.
 */
static bool _process_background_led_pattern(uint8_t index, bool force_update, bool need_delay, uint32_t gpt_time)
{
    APPS_LOG_MSGID_I(LOG_TAG"BG LED pattern = %d, need_delay = %d, gpt_time = %d", 3, index, need_delay, gpt_time);

    /* If s_playing_bg_index == index, it's the same pattern, don't update it. */
    if (force_update || s_playing_bg_index != index) {
        s_playing_bg_index = index;
#if LED_ENABLE
        if (!s_temp_disable_led) {
            return app_led_control_enable_with_sync(LED_PATTERN_BG, index, false, need_delay, gpt_time);
        } else
#endif
        {
            return true;
        }
    } else {
        return true;
    }
}
#endif /* #ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

#ifdef MTK_AWS_MCE_ENABLE
/**
 * @brief      Agent to sync BG LED pattern to partner by sending aws data.
 * @param[in]  index, the LED pattern index, refer to apps_config_led_index_list.h.
 * @param[in]  priority, the priority of the BG pattern, if is lower than the priority of partner local pattern,
 *             the synced pattern will be ignored.
 * @param[out] p_target_gpt_time, the pointer to the target gpt time.
 * @return     Send aws success or not.
 */
static bool _sync_background_led_pattern(uint8_t index, uint8_t priority, uint32_t *p_target_gpt_time)
{
    bt_status_t send_result;
    apps_config_led_manager_pattern_info_t pattern_info = {
        .pattern_type = LED_PATTERN_BG,
        .pattern_index = index,
        .sync_priority = priority,
        .timeout = 0
    };
    bt_sink_srv_bt_clock_addition(&pattern_info.bt_clock, NULL, LED_PATTERN_DEFAULT_SYNC_TIME);
    bt_aws_mce_report_info_t mce_repo_info = {
        .module_id = BT_AWS_MCE_REPORT_MODULE_LED,
        .is_sync = false,
        .sync_time = 0,
        .param_len = sizeof(apps_config_led_manager_pattern_info_t),
        .param = &pattern_info
    };

    send_result = bt_aws_mce_report_send_urgent_event(&mce_repo_info);
    if (BT_STATUS_SUCCESS != send_result) {
        APPS_LOG_MSGID_I(LOG_TAG"_sync_background_led_pattern fail, error = %x",
                         1, send_result);
        return false;
    } else {
        bt_sink_srv_convert_bt_clock_2_gpt_count(&pattern_info.bt_clock, p_target_gpt_time);
        return true;
    }
}
#endif

bool apps_config_set_background_led_pattern(uint8_t index,
                                            bool need_sync,
                                            apps_config_led_manager_aws_sync_priority_t priority)
{
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
    apps_race_cmd_co_sys_led_pattern_format_t bg_format = {
        .index = index,
        .need_sync = need_sync,
    };
    app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD, APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_UPDATE_BG_LED, &bg_format, sizeof(bg_format), false);
    return true;
#else
#ifdef MTK_AWS_MCE_ENABLE
    bool need_update_pattern = true;    /* Need update to LED module. */
#endif
    bool force_update = false;  /* Update even the last time is the same pattern. */
    bool sync_success = false;  /* Send AWS data success. */
    bool ret = true;            /* The return value of calling the LED module APIs. */
    uint32_t gpt_time = 0;      /* Play LED pattern at the target GPT time, 0 means play immediately. */

    APPS_LOG_MSGID_I(LOG_TAG"apps_config_set_background_led_pattern, index: %d, need_sync: %d",
                     2, index, need_sync);

#ifdef MTK_AWS_MCE_ENABLE
    if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
        if (!need_sync) {
            /* If last time background_synced and this time doesn't need sync any more, notify partner to cancel sync. */
            if (s_led_sync_info.background_synced) {
                apps_config_led_manager_pattern_info_t pattern_info = {
                    .pattern_type = LED_PATTERN_BG,
                    .pattern_index = LED_PATTERN_INVALID_INDEX,
                    .sync_priority = APPS_CONFIG_LED_AWS_SYNC_PRIO_INVALID,
                    .timeout = 0
                };
                bt_aws_mce_report_info_t mce_repo_info = {
                    .module_id = BT_AWS_MCE_REPORT_MODULE_LED,
                    .is_sync = false,
                    .sync_time = 0,
                    .param_len = sizeof(apps_config_led_manager_pattern_info_t),
                    .param = &pattern_info
                };
                if (BT_STATUS_FAIL == bt_aws_mce_report_send_event(&mce_repo_info)) {
                    APPS_LOG_MSGID_E(LOG_TAG"Fail to notify partner to disable sync bg pattern", 0);
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG"Success to notify partner to disable sync bg pattern", 0);
                }
            }
            s_led_sync_info.background_synced = false;
            s_led_sync_info.background_sync_index = LED_PATTERN_INVALID_INDEX;
        } else {
            if (s_led_sync_info.background_synced && index == s_led_sync_info.background_sync_index) {
                APPS_LOG_MSGID_I(LOG_TAG"The same bg index need sync, ignore", 0);
                need_update_pattern = false;
            } else {
                if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()
                    && _sync_background_led_pattern(index, priority, &gpt_time)) {
                    APPS_LOG_MSGID_I(LOG_TAG"Success to sync new bg pattern to partner", 0);
                    s_led_sync_info.background_synced = true;
                    s_led_sync_info.background_sync_index = index;
                    s_led_sync_info.background_sync_priority = priority;
                    force_update = true;
                    sync_success = true;
                } else {
                    APPS_LOG_MSGID_E(LOG_TAG"Fail to sync new bg pattern to partner", 0);
                    s_led_sync_info.background_synced = false;
                    s_led_sync_info.background_sync_index = LED_PATTERN_INVALID_INDEX;
                }
            }
        }
    } else { /* Partner */
        if (!need_sync) {
            if (s_led_sync_info.background_synced && priority <= s_led_sync_info.background_sync_priority) {
                if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
                    APPS_LOG_MSGID_I(LOG_TAG"Partner ignore new bg pattern, because background_synced", 0);
                    need_update_pattern = false;
                    /* If last time playing self LED pattern but this time need play sync pattern. */
                    if (!s_led_sync_info.need_sync_bg
                        && s_background_self_priority > s_led_sync_info.background_sync_priority) {
                        APPS_LOG_MSGID_I(LOG_TAG"partner notify re-sync when self BG changed", 0);
                        _partner_send_resync_bg_request();
                    }
                } else {
                    APPS_LOG_MSGID_E(LOG_TAG"Partner state error, why background_synced but aws not connected", 0);
                }
            }
        } else {
            /* If AWS is connected, partner will ingore the need_sync LED pattern from local. */
            if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
                if (s_led_sync_info.background_synced
                    && s_led_sync_info.background_sync_index != APPS_CONFIG_LED_AWS_SYNC_PRIO_INVALID) {
                    APPS_LOG_MSGID_I(LOG_TAG"partner ignore need sync bg pattern when attached", 0);
                    need_update_pattern = false;
                    /* If the partner is playing a high priority local pattern, it will ignore the synced pattern from agent.
                     But if the new local pattern is low priority, partner need notify agent resync the pattern.*/
                    if (!s_led_sync_info.need_sync_bg
                        && s_background_self_priority > s_led_sync_info.background_sync_priority
                        && s_led_sync_info.background_synced
                        && s_led_sync_info.background_sync_index != APPS_CONFIG_LED_AWS_SYNC_PRIO_INVALID) {
                        APPS_LOG_MSGID_I(LOG_TAG"partner notify re-sync when self BG changed nosync->sync", 0);
                        _partner_send_resync_bg_request();
                    }
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG"partner display self sync pattern when agent never sync", 0);
                }
            }
        }
    }
    s_led_sync_info.need_sync_bg = need_sync;
    s_background_self_priority = priority;
    background_self_index = index;
    if (need_update_pattern)
#endif
    {
        ret = _process_background_led_pattern(index, force_update, sync_success, gpt_time);
    }

    return ret;
#endif /* #ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
}

/**
 * @brief      Callback for FG pattern is timeout.
 * @param[in]  xTimer, the pointer to the TimerHandle_t.
 */
static void _led_fg_time_out_callback(TimerHandle_t xTimer)
{
    APPS_LOG_MSGID_I(LOG_TAG"FG LED pattern timeout", 0);
    /* Send the timout event to APPs, make it processed in ui task. */
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_LED_MANAGER,
                        APPS_EVENTS_LED_FG_PATTERN_TIMEOUT, NULL, 0, NULL, 0);
    /* Usage of fg_display: The callback is different task from UI task, and priority is high.
     Sometimes when UI task set another FG LED pattern after the callback called,
     the APPS_EVENTS_LED_FG_PATTERN_TIMEOUT will disable the new FG pattern by mistake. */
    fg_display = false;
}

void apps_config_check_foreground_led_pattern(void)
{
    if (!fg_display) {
        APPS_LOG_MSGID_I(LOG_TAG"Disable FG LED pattern", 0);
        s_playing_fg_index = LED_PATTERN_INVALID_INDEX;
#if LED_ENABLE
#if LED_SUPPORT_TEMPORARY_DISABLE_FG
        if (!s_temp_disable_led)
#endif
        {
            app_led_control_disable(LED_PATTERN_FG, false);
        }
#endif
#ifdef MTK_AWS_MCE_ENABLE
        if (BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if (BT_AWS_MCE_ROLE_AGENT == role) {
                if (s_led_sync_info.need_sync_bg) {
                    /* When fg pattern timeout, re-sync bg pattern */
                    uint32_t gpt_time = 0;
                    APPS_LOG_MSGID_I(LOG_TAG"Agent re-sync bg pattern when fg timeout", 0);
                    if (_sync_background_led_pattern(background_self_index, s_background_self_priority, &gpt_time)) {
                        _process_background_led_pattern(background_self_index, true, true, gpt_time);
                        s_led_sync_info.background_synced = true;
                        s_led_sync_info.background_sync_index = background_self_index;
                        s_led_sync_info.background_sync_priority = s_background_self_priority;
                    } else {
                        s_led_sync_info.background_synced = false;
                        s_led_sync_info.background_sync_index = LED_PATTERN_INVALID_INDEX;
                    }
                }
            } else {
                /* Send re-sync bg pattern request to agent. */
                /* If foreground_synced, consider the agent also timeout at the same time, no need send re-sync request */
                if (!s_led_sync_info.foreground_synced && s_led_sync_info.background_synced
                    && s_background_self_priority <= s_led_sync_info.background_sync_priority) {
                    APPS_LOG_MSGID_I(LOG_TAG"Partner send re-sync request when FG end", 0);
                    _partner_send_resync_bg_request();
                }
            }
        } else {
            APPS_LOG_MSGID_I(LOG_TAG"NOT attached, when fg timeout", 0);
        }
        s_led_sync_info.foreground_synced = false;
#endif
    }
}

#ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
/**
 * @brief      Set the FG pattern to low layer(LED module) and refresh the timer.
 * @param[in]  index, the LED pattern index, refer to apps_config_led_index_list.h.
 * @param[in]  timeout, the duration of the FG LED pattern playing.
 * @param[in]  need_delay, true means play the LED pattern till gpt_time.
 * @param[in]  gpt_time, the GPT time to start play the LED pattern. It's useful when need_delay is true.
 */
static bool _process_foreground_led_pattern(uint8_t index, uint16_t timeout, bool need_delay, uint32_t gpt_time)
{
    bool ret = false;
    /* Refresh the timer. */
    xTimerStop(s_timer_handle, 0);
    if (pdPASS == xTimerChangePeriod(s_timer_handle, timeout * LED_FG_TIMEOUT_UNIT_MS / portTICK_PERIOD_MS, 0)) {
        APPS_LOG_MSGID_I(LOG_TAG"FG LED pattern = %d, timeout = %d * 100ms, need_delay = %d, gpt_time = %d",
                        4, index, timeout, need_delay, gpt_time);
        fg_display = true;
#if LED_ENABLE
        s_playing_fg_index = index;
#if LED_SUPPORT_TEMPORARY_DISABLE_FG
        if (!s_temp_disable_led)
#endif
        {
            ret =  app_led_control_enable_with_sync(LED_PATTERN_FG, index, false, need_delay, gpt_time);
        } else {
            ret = true;
        }
#else
        ret = true;
#endif
        xTimerStart(s_timer_handle, 0);
    } else {
        app_led_control_disable(LED_PATTERN_FG, false);
    }

    return ret;
}
#endif /* #ifndef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */

bool apps_config_set_foreground_led_pattern(uint8_t index, uint16_t timeout, bool need_sync)
{
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE
    apps_race_cmd_co_sys_led_pattern_format_t fg_format = {
        .index = index,
        .need_sync = need_sync,
        .fg_timeout = timeout,
    };
    app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD, APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_UPDATE_FG_LED, &fg_format, sizeof(fg_format), false);
    return true;
#else
    uint32_t gpt_time = 0;
    bool sync_success = false;
    bool ret;

    APPS_LOG_MSGID_I(LOG_TAG"apps_config_set_foreground_led_pattern, index: %d, timeout: %d, need_sync: %d",
                     3, index, timeout, need_sync);

#ifdef MTK_AWS_MCE_ENABLE
    /* If need sync and AWS is connected. */
    if (need_sync
        && BT_AWS_MCE_SRV_LINK_NONE != bt_aws_mce_srv_get_link_type()) {
        bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
        if (BT_AWS_MCE_ROLE_CLINET != role && BT_AWS_MCE_ROLE_PARTNER != role) {
            apps_config_led_manager_pattern_info_t pattern_info = {
                .pattern_type = LED_PATTERN_FG,
                .pattern_index = index,
                .timeout = timeout
            };
            bt_sink_srv_bt_clock_addition(&pattern_info.bt_clock, NULL, LED_PATTERN_DEFAULT_SYNC_TIME);
            bt_aws_mce_report_info_t mce_repo_info = {
                .module_id = BT_AWS_MCE_REPORT_MODULE_LED,
                .is_sync = false,
                .sync_time = 0,
                .param_len = sizeof(apps_config_led_manager_pattern_info_t),
                .param = &pattern_info
            };
            if (BT_STATUS_SUCCESS == bt_aws_mce_report_send_urgent_event(&mce_repo_info)) {
                sync_success = true;
                bt_sink_srv_convert_bt_clock_2_gpt_count(&pattern_info.bt_clock, &gpt_time);
                s_led_sync_info.foreground_synced = true;
            } else {
                s_led_sync_info.foreground_synced = false;
                APPS_LOG_MSGID_W(LOG_TAG"apps_config_set_foreground_led_pattern, failed to sync", 0);
            }
        } else {
            /* Partner expects receiving the sync pattern from agent, so it ignores the local need_sync pattern when AWS is connected. */
            APPS_LOG_MSGID_I(LOG_TAG"apps_config_set_foreground_led_pattern, partner ignore", 0);
            return true;
        }
    } else {
        APPS_LOG_MSGID_I(LOG_TAG"apps_config_set_foreground_led_pattern, not need sync", 0);
        s_led_sync_info.foreground_synced = false;
    }
#endif
    ret = _process_foreground_led_pattern(index, timeout, sync_success, gpt_time);

    return ret;
#endif /* #ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE */
}

#ifdef MTK_AWS_MCE_ENABLE
void apps_config_led_manager_on_aws_attached(bt_aws_mce_role_t role, bool attached)
{
    APPS_LOG_MSGID_E(LOG_TAG"apps_config_led_manager_on_aws_attached, role = %x, attached = %d",
                     2, role, attached);

    if (BT_AWS_MCE_ROLE_AGENT == role && s_led_sync_info.need_sync_bg) {
        if (attached) {
            /* Agent need sync LED pattern to partner. */
            uint32_t gpt_time = 0;
            if (_sync_background_led_pattern(background_self_index, s_background_self_priority, &gpt_time)) {
                _process_background_led_pattern(background_self_index, true, true, gpt_time);
                s_led_sync_info.background_synced = true;
                s_led_sync_info.background_sync_index = background_self_index;
                s_led_sync_info.background_sync_priority = s_background_self_priority;
            } else {
                s_led_sync_info.background_synced = false;
                s_led_sync_info.background_sync_index = LED_PATTERN_INVALID_INDEX;
            }
        } else {
            s_led_sync_info.background_synced = false;
            s_led_sync_info.background_sync_index = LED_PATTERN_INVALID_INDEX;
        }
    } else if (BT_AWS_MCE_ROLE_PARTNER == role && !attached) {
        /* When aws disconnected, partner need play local LED pattern. */
        _process_background_led_pattern(background_self_index, false, false, 0);
        s_led_sync_info.background_synced = false;
        s_led_sync_info.background_sync_index = LED_PATTERN_INVALID_INDEX;
    }

}

void app_config_led_sync(void *param)
{
    bt_status_t bt_status;
    if (param) {
        apps_config_led_manager_pattern_info_t *p_pattern_info =
            (apps_config_led_manager_pattern_info_t *)param;
        if (LED_PATTERN_FG == p_pattern_info->pattern_type) {
            /* Only partner can receive aws data that pattern_type is LED_PATTERN_FG. */
            APPS_LOG_MSGID_I(LOG_TAG"app_config_led_sync, sync fg index %d from agent", 1, p_pattern_info->pattern_index);
            uint32_t target_gpt_time;
            bt_status = bt_sink_srv_convert_bt_clock_2_gpt_count(&p_pattern_info->bt_clock, &target_gpt_time);
            APPS_LOG_MSGID_I(LOG_TAG"app_config_led_sync, result %x, remain sync gpt time:%d", 2, bt_status, target_gpt_time);
            _process_foreground_led_pattern(p_pattern_info->pattern_index, p_pattern_info->timeout, BT_STATUS_SUCCESS == bt_status, target_gpt_time);
        } else if (LED_PATTERN_BG == p_pattern_info->pattern_type) {
            /* Only partner can receive aws data that pattern_type is LED_PATTERN_BG. */
            APPS_LOG_MSGID_I(LOG_TAG"app_config_led_sync, sync bg index %d from agent", 1, p_pattern_info->pattern_index);
            if (LED_PATTERN_INVALID_INDEX == p_pattern_info->pattern_index) {
                /* Partner receive sync -> unsync event, play local LED pattern. */
                _process_background_led_pattern(background_self_index, false, false, 0);
                s_led_sync_info.background_synced = false;
            } else {
                /* Partner receive sync event, play local LED pattern. */
                s_led_sync_info.background_synced = true;
                if (s_led_sync_info.need_sync_bg
                    || p_pattern_info->sync_priority >= s_background_self_priority) {
                    /* If sync priority is higher than local priority, play the sync LED parttern. */
                    uint32_t target_gpt_time;
                    bt_status = bt_sink_srv_convert_bt_clock_2_gpt_count(&p_pattern_info->bt_clock, &target_gpt_time);
                    APPS_LOG_MSGID_I(LOG_TAG"app_config_led_sync, bt_status: %x, target gpt_time:%d", 2, bt_status, target_gpt_time);
                    _process_background_led_pattern(p_pattern_info->pattern_index, true, BT_STATUS_SUCCESS == bt_status, target_gpt_time);
                } else {
                    /* If sync priority is lower than local priority, play local LED pattern. */
                    APPS_LOG_MSGID_I(LOG_TAG"app_config_led_sync, sync priority is lower, display local pattern", 0);
                    /* If current playing pattern is the same, it's not necessary to refresh, so the parameter force_update is false. */
                    _process_background_led_pattern(background_self_index, false, false, 0);
                }
            }
            s_led_sync_info.background_sync_index = p_pattern_info->pattern_index;
            s_led_sync_info.background_sync_priority = p_pattern_info->sync_priority;
        } else if (LED_PATTERN_NONE == p_pattern_info->pattern_type) {
            /* Agent receive re-sync request from partner. */
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if (BT_AWS_MCE_ROLE_AGENT == role && s_led_sync_info.need_sync_bg) {
                uint32_t gpt_time = 0;
                APPS_LOG_MSGID_I(LOG_TAG"app_config_led_sync, receive re-sync", 0);
                if (_sync_background_led_pattern(background_self_index, s_background_self_priority, &gpt_time)) {
                    s_led_sync_info.background_synced = true;
                    s_led_sync_info.background_sync_index = background_self_index;
                    s_led_sync_info.background_sync_priority = s_background_self_priority;
                    APPS_LOG_MSGID_I(LOG_TAG"success to re-sync", 0);
                    _process_background_led_pattern(background_self_index, true, true, gpt_time);
                } else {
                    s_led_sync_info.background_synced = false;
                    s_led_sync_info.background_sync_index = LED_PATTERN_INVALID_INDEX;
                    APPS_LOG_MSGID_E(LOG_TAG"fail to re-sync", 0);
                }
            }
        }
    }
}

/**
 * @brief      The callback function which need to be registered to receive aws data.
 * @param[in]  param, is the event information to handle.
 */
static void app_led_sync_callback(bt_aws_mce_report_info_t *para)
{
    APPS_LOG_MSGID_I(LOG_TAG"app_led_sync_callback: send aws report to app: module 0x%0x", 1, para->module_id);
    apps_config_led_manager_pattern_info_t *extra_data = pvPortMalloc(sizeof(apps_config_led_manager_pattern_info_t));
    if (extra_data == NULL) {
        APPS_LOG_MSGID_I(LOG_TAG"app_led_sync_callback: malloc fail", 0);
        return;
    }

    /* para is the aws data send from agentm the format of para->param must be apps_config_led_manager_pattern_info_t. */
    memcpy(extra_data, para->param, sizeof(apps_config_led_manager_pattern_info_t));

    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_LED_MANAGER,
                        APPS_EVENTS_LED_SYNC_LED_PATTERN, extra_data, sizeof(apps_config_led_manager_pattern_info_t),
                        NULL, 0);
}
#endif

void apps_config_led_manager_init(void)
{
    s_timer_handle = xTimerCreate(LED_FG_TIMER_NAME, 100, pdFALSE, 0, _led_fg_time_out_callback);
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_LED, app_led_sync_callback);
#endif
}

void apps_config_led_temporary_disable(bool disable_led)
{
    bool ret;
    APPS_LOG_MSGID_I(LOG_TAG"temporary_disable %d -> %d, bg:%d, fg:%d", 4,
                     s_temp_disable_led, disable_led, s_playing_bg_index, s_playing_fg_index);
    if (!s_temp_disable_led && disable_led) {
        /* Not disable -> disable. */
        s_temp_disable_led = disable_led;
        if (LED_PATTERN_INVALID_INDEX != s_playing_bg_index) {
            app_led_control_disable(LED_PATTERN_BG, false);
        }
#if LED_SUPPORT_TEMPORARY_DISABLE_FG
        if (LED_PATTERN_INVALID_INDEX != s_playing_fg_index) {
            app_led_control_disable(LED_PATTERN_FG, false);
        }
#endif
    } else if (s_temp_disable_led && !disable_led) {
        /* Disable -> not disable. */
        s_temp_disable_led = disable_led;
#if LED_SUPPORT_TEMPORARY_DISABLE_FG
        if (LED_PATTERN_INVALID_INDEX != s_playing_fg_index) {
            ret = app_led_control_enable_with_sync(LED_PATTERN_FG, s_playing_fg_index, false, false, 0);
            if (!ret) {
                APPS_LOG_MSGID_W(LOG_TAG"temporary_disable restart FG fail", 0);
            }
        }
#endif
        if (LED_PATTERN_INVALID_INDEX != s_playing_bg_index) {
            ret = false;
#ifdef MTK_AWS_MCE_ENABLE
            uint32_t gpt_time = 0;
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()
                && s_led_sync_info.need_sync_bg) {
                APPS_LOG_MSGID_I(LOG_TAG"temporary_disable restart BG, agent resync", 0);
                if (_sync_background_led_pattern(background_self_index, s_background_self_priority, &gpt_time)) {
                    ret = _process_background_led_pattern(background_self_index, true, true, gpt_time);
                } else {
                    APPS_LOG_MSGID_W(LOG_TAG"temporary_disable restart BG, agent resync fail", 0);
                }
            } else if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()
                       && s_led_sync_info.background_synced
                       && s_background_self_priority <= s_led_sync_info.background_sync_priority) {
                APPS_LOG_MSGID_I(LOG_TAG"temporary_disable restart BG, partner send resync", 0);
                if (BT_STATUS_SUCCESS != _partner_send_resync_bg_request()) {
                    APPS_LOG_MSGID_W(LOG_TAG"temporary_disable restart BG, partner send resync fail", 0);
                } else {
                    ret = true;
                }
            }
            if (!ret)
#endif
            {
                ret = app_led_control_enable_with_sync(LED_PATTERN_BG, s_playing_bg_index, false, false, 0);
                if (!ret) {
                    APPS_LOG_MSGID_W(LOG_TAG"temporary_disable restart BG fail", 0);
                }
            }
        }
    }
}

void apps_config_led_disable_all(void)
{
    app_led_control_disable(LED_PATTERN_BG, false);
    app_led_control_disable(LED_PATTERN_FG, false);
    xTimerStop(s_timer_handle, 0);
    fg_display = false;
    s_playing_bg_index = LED_PATTERN_INVALID_INDEX;  /* Current playing BG pattern. */
    s_playing_fg_index = LED_PATTERN_INVALID_INDEX;  /* Current playing FG pattern. */
    s_temp_disable_led = false;                         /* In temporary disable LED status. */

#ifdef MTK_AWS_MCE_ENABLE
    background_self_index = LED_PATTERN_INVALID_INDEX;   /* The BG pattern set from APPs. */
    s_background_self_priority = APPS_CONFIG_LED_AWS_SYNC_PRIO_INVALID; 
    memset(&s_led_sync_info, 0, sizeof(s_led_sync_info));
#endif
}