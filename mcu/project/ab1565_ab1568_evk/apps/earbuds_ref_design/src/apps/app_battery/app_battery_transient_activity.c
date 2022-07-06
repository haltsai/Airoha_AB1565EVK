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
 * File: app_battery_transient_activity.c
 *
 * Description: This file could update background LED pattern for battery APP.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for Battery APP.
 *
 */

#include "app_battery_transient_activity.h"
#include "apps_events_interaction_event.h"
#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "ui_shell_manager.h"
#include "FreeRTOS.h"
#ifdef AIR_TILE_ENABLE
#include "app_bt_state_service.h"
#endif
#define LOG_TAG       "[app_battery_transient]"

static bool battery_transient_proc_ui_shell_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE:
            APPS_LOG_MSGID_I(LOG_TAG" create", 0);
            if (extra_data) {
                self->local_context = extra_data;
                APPS_LOG_MSGID_I(LOG_TAG" extra data for create", 0);
            }
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            APPS_LOG_MSGID_I(LOG_TAG" destroy", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            APPS_LOG_MSGID_I(LOG_TAG" resume", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            APPS_LOG_MSGID_I(LOG_TAG" pause", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
            APPS_LOG_MSGID_I(LOG_TAG" refresh", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
            APPS_LOG_MSGID_I(LOG_TAG" result", 0);
            break;
        default:
            break;
    }
    return ret;
}

static bool battery_transient_interaction_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    battery_local_context_type_t *local_ctx = (battery_local_context_type_t *)self->local_context;
    bool ret = false;

    switch (event_id) {
        case APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN: {
            /* Check battery state and set background LED pattern. */
            APPS_LOG_MSGID_I(LOG_TAG "Current battery state = %d", 1, local_ctx->state);
            switch (local_ctx->state) {
                case APP_BATTERY_STATE_IDLE:
                case APP_BATTERY_STATE_FULL:
                    /* No need to update Battery LED, finish activity. */
                    ui_shell_finish_activity(self, self);
                    ret = false;
                    break;
                case APP_BATTERY_STATE_CHARGING:
                    apps_config_set_background_led_pattern(LED_INDEX_CHARGING, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOW);
                    ret = true;
                    break;
                case APP_BATTERY_STATE_CHARGING_FULL:
                    /* Use "idle" pattern when charging completely. */
                    apps_config_set_background_led_pattern(LED_INDEX_IDLE, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOWEST);
                    ret = true;
                    break;
                case APP_BATTERY_STATE_LOW_CAP:
                case APP_BATTERY_STATE_SHUTDOWN: {
#ifdef AIR_TILE_ENABLE
                    const app_bt_state_service_status_t* bt_state_srv_status = app_bt_connection_service_get_current_status();
                    if (bt_state_srv_status != NULL && bt_state_srv_status->current_power_state == APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLED)
                    {
                        /* Do not display low battery LED if BT is in classic off mode */
                        apps_config_set_background_led_pattern(LED_INDEX_IDLE, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOWEST);

                    }
                    else
#endif
                    {
                        apps_config_set_background_led_pattern(LED_INDEX_LOW_BATTERY, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_HIGH);
                    }
                    ret = true;
                }
                    break;
                case APP_BATTERY_STATE_THR:
                    apps_config_set_background_led_pattern(LED_INDEX_CHARGING_ERROR, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_LOW);
                    ret = true;
                    break;
                default:
                    break;

            }
            break;
        }
        default:
            APPS_LOG_MSGID_I(LOG_TAG" Not supported event id = %d", 1, event_id);
            break;
    }
    return ret;
}

bool app_battery_transient_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = battery_transient_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        /* APP interaction events - only handle and update LED background pattern. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = battery_transient_interaction_event_proc(self, event_id, extra_data, data_len);
            break;
        default:
            break;
    }
    return ret;
}
