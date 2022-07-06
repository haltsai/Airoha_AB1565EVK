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
 * File: app_master_transient_activity.c
 *
 * Description: This file is the activity to handle the dual master events.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#include "app_master_transient_activity.h"
#include "app_master_utils.h"
#include "apps_debug.h"
#include "apps_race_cmd_co_sys_event.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_config_state_list.h"
#include "apps_config_led_index_list.h"
#include "apps_config_led_manager.h"
#include "apps_config_key_remapper.h"

#define LOG_TAG "[dual_master]transient "

static bool app_dual_chip_master_transient_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(LOG_TAG": create", 0);
            self->local_context = extra_data;
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                NULL, 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(LOG_TAG": destroy", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(LOG_TAG": resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(LOG_TAG": pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(LOG_TAG": refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(LOG_TAG": result", 0);
            break;
        }
        default:
            ret = false;
            break;
    }
    return ret;
}

static bool app_dual_chip_master_transient_proc_interaction_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    if (!self || !self->local_context) {
        return ret;
    }

    app_dual_chip_master_context_t *local_context = (app_dual_chip_master_context_t *)self->local_context;
    switch (event_id) {
        case APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN: {
            if (LED_INDEX_IDLE == local_context->slave_bg_led_pattern || LED_INDEX_DISCONNECTED == local_context->slave_bg_led_pattern) {
                APPS_LOG_MSGID_I(LOG_TAG"ignore when current slave LED pattern is :%d", 1, local_context->slave_bg_led_pattern);
            } else {
                APPS_LOG_MSGID_I(LOG_TAG"set slave LED pattern is :%d", 1, local_context->slave_bg_led_pattern);
                apps_config_set_background_led_pattern(local_context->slave_bg_led_pattern, false, APPS_CONFIG_LED_AWS_SYNC_PRIO_MIDDLE);
                ret = true;
            }
            break;
        }
        case APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE: {
            apps_config_key_set_mmi_state(local_context->slave_mmi_state);
            ret = true;
            break;
        }
    }

    return ret;
}

static bool app_dual_chip_master_transient_proc_dual_chip_cmd_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    if (!self || !self->local_context) {
        return ret;
    }

    app_dual_chip_master_context_t *local_context = (app_dual_chip_master_context_t *)self->local_context;
    switch (event_id) {
        case APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_UPDATE_MMI_STATE: {
            if (extra_data) {
                uint8_t new_state = *(uint8_t *)extra_data;
                app_dual_chip_master_mmi_priority_t old_mmi_state_level = app_dual_chip_master_get_slave_mmi_priority(local_context->slave_mmi_state);
                app_dual_chip_master_mmi_priority_t new_mmi_state_level = app_dual_chip_master_get_slave_mmi_priority(new_state);;
                if (old_mmi_state_level != new_mmi_state_level) {
                    ui_shell_finish_activity(self, self);
                    if (APP_DUAL_CHIP_MASTER_MMI_PRIORITY_IDLE == new_mmi_state_level) {
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                            NULL, 0);
                        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                            APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                            NULL, 0);
                    } else {
                        ui_shell_activity_priority_t activity_priority = ACTIVITY_PRIORITY_MIDDLE;
                        if (APP_DUAL_CHIP_MASTER_MMI_PRIORITY_DISCOVERABLE == new_mmi_state_level) {
                            activity_priority = ACTIVITY_PRIORITY_LOW;
                        } else if (APP_DUAL_CHIP_MASTER_MMI_PRIORITY_BT_ACTIVE == new_mmi_state_level) {
                            activity_priority = ACTIVITY_PRIORITY_HIGH;
                        } else if (APP_DUAL_CHIP_MASTER_MMI_PRIORITY_SPECIAL == new_mmi_state_level) {
                            activity_priority = ACTIVITY_PRIORITY_HIGH;
                        }
                        ui_shell_start_activity(self, app_master_transient_activity_proc, activity_priority, local_context, 0);
                    }
                } else if (local_context->slave_mmi_state != new_state) {
                    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                        NULL, 0);
                    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_UPDATE_MMI_STATE, NULL, 0,
                                        NULL, 0);
                }
                if (local_context->slave_mmi_state != new_state) {
                    local_context->slave_mmi_state = new_state;
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                    app_dual_chip_utls_power_saving_state_change();
#endif
                }
            }
            ret = true;
            break;
        }
        case APPS_RACE_CMD_CO_SYS_DUAL_CHIP_EVENT_UPDATE_BG_LED: {
            if (extra_data) {
                apps_race_cmd_co_sys_led_pattern_format_t *bg_format = (apps_race_cmd_co_sys_led_pattern_format_t *)extra_data;
                local_context->slave_bg_led_pattern = bg_format->index;
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                    APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, NULL, 0,
                                    NULL, 0);
            }
            break;
        }
        default:
            break;
    }

    return ret;
}

bool app_master_transient_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM: {
            /* UI Shell internal events, please refer to doc/Airoha_IoT_SDK_UI_Framework_Developers_Guide.pdf. */
            ret = app_dual_chip_master_transient_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            /* App interaction event. */
            ret = app_dual_chip_master_transient_proc_interaction_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_KEY : {
            /* Key events */
            if (self && self->local_context) {
                ret = app_dual_chip_master_process_key_event(((app_dual_chip_master_context_t *)(self->local_context))->slave_mmi_state, event_id, extra_data);
            }
            break;
        }
        case EVENT_GROUP_UI_SHELL_DUAL_CHIP_CMD: {
            /* Dual chip race events. */
            ret = app_dual_chip_master_transient_proc_dual_chip_cmd_group(self, event_id, extra_data, data_len);
        }
        default: {
            break;
        }
    }

    return ret;
}

