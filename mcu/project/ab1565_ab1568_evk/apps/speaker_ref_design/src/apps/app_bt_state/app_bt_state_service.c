/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
 * File: app_bt_state_service.c
 *
 * Description: This file provides many utility function to control BT switch and visibility.
 *
 */

#include "app_bt_state_service.h"
#include "apps_events_event_group.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "bt_device_manager_power.h"
#include "bt_app_common.h"
#include "ui_shell_manager.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_events_bt_event.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_srv.h"
#include "apps_aws_sync_event.h"
#include "apps_config_features_dynamic_setting.h"
#endif
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "bt_ull_service.h"
#endif
#ifdef MTK_RACE_CMD_ENABLE
#include "race_bt.h"
#endif
#include "app_rho_idle_activity.h"
#include "apps_events_interaction_event.h"
#include "apps_debug.h"
#ifdef MTK_BT_SPEAKER_ENABLE
#include "apps_bt_aws_mce_le_association.h"
#endif

#define LOG_TAG     "[app_bt_state_service]"

#define RETRY_TIMEOUT_BT_POWER_OFF_FAILED       (500)

/**
 *  @brief This structure defines the pending request of BT visibility.
 */
typedef struct {
    bool bt_visible;                        /**<  Request enable/disable BT visibility. */
    bool need_wait_aws;                     /**<  Whether must wait AWS. */
    uint32_t timeout;                       /**<  BT visibility duration time. */
} app_bt_state_service_bt_visible_request_t;

/* Current BT visibility pending request. */
app_bt_state_service_bt_visible_request_t s_visible_pending_request = {
    false,
    false,
    0
};

/* The flag for system off. */
static bool s_for_system_off = false;

/* Current status/context of BT state service. */
app_bt_state_service_status_t s_current_status = {
    .connection_state = APP_BT_CONNECTION_SERVICE_BT_STATE_BT_OFF,
    .current_power_state = APP_BT_STATE_POWER_STATE_DISABLED,
    .target_power_state = APP_BT_STATE_POWER_STATE_NONE_ACTION,
    .aws_connected = false,
    .bt_visible = false,
#ifdef MTK_AWS_MCE_ENABLE
    .in_air_pairing = false,
#endif
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
    .in_ull_pairing = false,
#endif
};

static bool app_bt_state_service_pending_bt_on_off(void)
{
#if defined(MTK_AWS_MCE_ENABLE)
    if (s_current_status.in_air_pairing) {
        return true;
    }
#endif
#if defined(AIR_BT_ULTRA_LOW_LATENCY_ENABLE)
    if (s_current_status.in_ull_pairing) {
        return true;
    }
#endif
    return false;
}

/**
* @brief      This function is used to control BT on/off via BT power API.
* @param[in]  try_rho, whether need to trigger RHO before turn off BT.
*/
static void app_bt_state_service_check_and_do_bt_enable_disable(bool try_rho)
{
    APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_enable_disable target status ? %d, current statue: %d",
                     2, s_current_status.target_power_state, s_current_status.current_power_state);

    if (app_bt_state_service_pending_bt_on_off()) {
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_enable_disable s_current_status.in_air_pairing is true", 0);
        return;
    }

    if (APP_BT_STATE_POWER_STATE_DISABLING == s_current_status.current_power_state
        || APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLING == s_current_status.current_power_state
        || APP_BT_STATE_POWER_STATE_ENABLING == s_current_status.current_power_state) {
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_enable_disable changing, so need wait: %d", 1, s_current_status.current_power_state);
        return;
    } else if (s_current_status.current_power_state == s_current_status.target_power_state) {
        /* Enable target state is the same as s_current_status.current_power_state, do not call bt function. */
        s_current_status.target_power_state = APP_BT_STATE_POWER_STATE_NONE_ACTION;
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_enable_disable Already at correct state: %d", 1, s_current_status.current_power_state);
        return;
    } else if (APP_BT_STATE_POWER_STATE_ENABLED == s_current_status.target_power_state) {
        /* Enable BT when s_current_status.target_power_state is APP_BT_STATE_POWER_STATE_ENABLED. */
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_enable_disable start set BT enable: %d", 1, s_current_status.target_power_state);
        bt_device_manager_power_active(BT_DEVICE_TYPE_LE | BT_DEVICE_TYPE_CLASSIC);
        s_current_status.current_power_state = APP_BT_STATE_POWER_STATE_ENABLING;
        s_current_status.target_power_state = APP_BT_STATE_POWER_STATE_NONE_ACTION;
    } else if (APP_BT_STATE_POWER_STATE_DISABLED == s_current_status.target_power_state
            || APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLED == s_current_status.target_power_state) {
#ifdef APPS_AUTO_TRIGGER_RHO
        /* Need to check RHO before disable BT. */
        if (apps_config_features_is_auto_rho_enabled()
            && bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT
            && s_current_status.aws_connected
            && try_rho) {
            APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_enable_disable do RHO before disable bt", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_TRIGGER_RHO, NULL, 0,
                                NULL, 0);
        } else
#endif
        {
            APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_enable_disable start set BT enable: %d", 1, s_current_status.target_power_state);
#ifdef MTK_BT_SPEAKER_ENABLE
            if (bt_aws_mce_srv_get_mode() == BT_AWS_MCE_SRV_MODE_BROADCAST &&
                    bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT) {
                /* Notify client to switch to single through urgent channel. */
                apps_aws_sync_event_send_urgent(EVENT_GROUP_UI_SHELL_LE_ASSOCIATION, LE_ASSOCIATION_EVENT_AWS_MCE_UNGROUP, NULL, 0);
            }
#endif
            /* Disable BT directly when role is partner, no need RHO or AWS not connected. */
            bt_status_t bt_state;
            if (APP_BT_STATE_POWER_STATE_DISABLED == s_current_status.target_power_state) {
                bt_state = bt_device_manager_power_standby(BT_DEVICE_TYPE_LE | BT_DEVICE_TYPE_CLASSIC);
            } else {
                bt_state = bt_device_manager_power_standby(BT_DEVICE_TYPE_CLASSIC);
            }
            if (BT_STATUS_SUCCESS == bt_state) {
                if (APP_BT_STATE_POWER_STATE_DISABLED == s_current_status.target_power_state) {
                    s_current_status.current_power_state = APP_BT_STATE_POWER_STATE_DISABLING;
                } else {
                    s_current_status.current_power_state = APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLING;
                }
                s_current_status.target_power_state = APP_BT_STATE_POWER_STATE_NONE_ACTION;
            } else {
                APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_enable_disable need retry standby: %x", 1, bt_state);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_BT_RETRY_POWER_ON_OFF, NULL, 0,
                        NULL, RETRY_TIMEOUT_BT_POWER_OFF_FAILED);
            }
        }
    }
}

/**
* @brief      This function is used to refresh BT visibility duration time.
* @param[in]  timeout, updated BT visibility duration time.
*/
static void app_bt_state_service_refresh_visible_timeout(uint32_t timeout)
{
    APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_refresh_visible_timeout: %d", 1, timeout);
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
    if (timeout > BT_VISIBLE_TIMEOUT_INVALID) {
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT, NULL, 0,
                            NULL, timeout);
    }
}

/**
* @brief      This function is used to check and turn on BT visibility after BT enabled or AWS connected.
*/
static void app_bt_state_service_check_and_do_bt_visible(void)
{
    if (s_visible_pending_request.bt_visible
#ifdef MTK_AWS_MCE_ENABLE
        && bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT
#endif
       ) {
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_check_and_do_bt_visible s_current_status.current_power_state: %d, wait_aws: %d - %d", 3,
                         s_current_status.current_power_state, s_visible_pending_request.need_wait_aws, s_current_status.aws_connected);
        /* Enable BT visibility when BT enabled and AW connected or no need wait AWS.*/
        if (s_current_status.current_power_state == APP_BT_STATE_POWER_STATE_ENABLED) {
            if (!s_visible_pending_request.need_wait_aws || s_current_status.aws_connected) {
                s_visible_pending_request.bt_visible = false;
                bt_cm_discoverable(true);
            }
        }
    }
}

/******************************************************************************
******************************* Public APIs ***********************************
******************************************************************************/
void app_bt_state_service_set_bt_on_off(bool on, bool classic_off, bool need_do_rho, bool for_system_off)
{
    APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_set_bt_on_off ? %d, classic_off: %d, try_rho : %d, for_system_off: %d", 4,
                     on, classic_off, need_do_rho, for_system_off);

    /* Should turn off BT and power off if current s_for_system_off is TRUE. */
    if (for_system_off) {
        s_for_system_off = for_system_off;
    }
    if (s_for_system_off) {
        s_current_status.target_power_state = APP_BT_STATE_POWER_STATE_DISABLED;
        /* No need to enable BT visibility if target BT state is off. */
        s_visible_pending_request.bt_visible = false;
    } else if (!on) {
        if (classic_off) {
            s_current_status.target_power_state = APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLED;
        } else {
            s_current_status.target_power_state = APP_BT_STATE_POWER_STATE_DISABLED;
        }
    } else {
        s_current_status.target_power_state = APP_BT_STATE_POWER_STATE_ENABLED;
        /* No need RHO if target BT state is on. */
        need_do_rho = false;
    }
#ifdef MTK_AWS_MCE_ENABLE
    if (!on && s_current_status.in_air_pairing) {
        bt_aws_mce_srv_air_pairing_stop();
    }
#endif
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_RETRY_POWER_ON_OFF);
    app_bt_state_service_check_and_do_bt_enable_disable(need_do_rho);
}

bool app_bt_state_service_set_bt_visible(bool enable_visible, bool wait_aws_connect, uint32_t timeout)
{
    bool ret = true;
    APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_set_bt_visible enable: %d, need_wait_aws: %d", 2,
                     enable_visible, wait_aws_connect);
#ifdef MTK_AWS_MCE_ENABLE
    if (s_current_status.in_air_pairing) {
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_set_bt_visible but in air pairing", 0);
        s_visible_pending_request.bt_visible = false;
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
        return ret;
    }
#endif
    /* No action if wanted BT visibility and current BT status is same. */
    if (enable_visible == s_current_status.bt_visible
#ifdef MTK_AWS_MCE_ENABLE
            && (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT
            || bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_NONE)
#endif
    ) {
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_set_bt_visible already %d", 1, enable_visible);
        if (enable_visible) {
            /* Refresh BT visibility duration if enable_visible is TRUE. */
            app_bt_state_service_refresh_visible_timeout(timeout);
        }
        return true;
    }
    if (enable_visible) {
        /* No action if current BT is off and target BT state isn't on or no need to wait AWS connection. */
        if (APP_BT_STATE_POWER_STATE_DISABLING >= s_current_status.current_power_state
            && (APP_BT_STATE_POWER_STATE_ENABLED != s_current_status.target_power_state || !wait_aws_connect)) {
            APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_set_bt_visible fail, because BT is OFF: %d, %d", 2,
                             s_current_status.current_power_state, s_current_status.target_power_state);
            return false;
        } else if (APP_BT_STATE_POWER_STATE_ENABLED == s_current_status.current_power_state) {
#ifdef MTK_AWS_MCE_ENABLE
            /* Partner send BT visibility request to Agent when BT on and AWS connected. */
            if ((!wait_aws_connect) && s_current_status.aws_connected && bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                app_bt_state_service_bt_visible_request_t aws_data = {
                    .bt_visible = TRUE,
                    .need_wait_aws = wait_aws_connect,
                    .timeout = timeout
                };
                if (BT_STATUS_SUCCESS != apps_aws_sync_event_send_extra(
                        EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_SET_BT_VISIBLE,
                        &aws_data,
                        sizeof(aws_data))) {
                    APPS_LOG_MSGID_E(LOG_TAG" Partner send discover request to agent fail", 0);
                    ret = false;
                } else {
                    s_visible_pending_request.bt_visible = FALSE;
                    app_bt_state_service_refresh_visible_timeout(timeout);
                    ret = true;
                }
            } else if (wait_aws_connect && !s_current_status.aws_connected) {
                /* Enable BT visibility pending request when BT on and AWS not connected. */
                APPS_LOG_MSGID_I(LOG_TAG" discoverable flag when AWS is not connected", 0);
                s_visible_pending_request.bt_visible = TRUE;
                s_visible_pending_request.need_wait_aws = wait_aws_connect;
                s_visible_pending_request.timeout = timeout;
                app_bt_state_service_refresh_visible_timeout(timeout);
            } else if (wait_aws_connect
                && bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_AGENT
                && bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_NONE) {
                APPS_LOG_MSGID_I(LOG_TAG" AWS connected, ignore the partner start visible request", 0);
                s_visible_pending_request.bt_visible = FALSE;
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
            } else
#endif
            {
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
                bt_app_common_pre_set_ultra_low_latency(BT_APP_COMMON_ULL_LATENCY_MODULE_DISCOVERABLE, 60);
                if (!wait_aws_connect) {
                    bt_app_common_apply_ultra_low_latency();
                }
#endif
                /* Enable BT visibility, disable BT visibility pending request. */
                bt_cm_discoverable(true);
                app_bt_state_service_refresh_visible_timeout(timeout);
                s_visible_pending_request.bt_visible = FALSE;
                APPS_LOG_MSGID_I(LOG_TAG" Set discoverable", 0);
            }
        } else {
            /* Enable BT visibility pending request when BT off. */
            APPS_LOG_MSGID_I(LOG_TAG" discoverable flag when BT is not enable", 0);
            s_visible_pending_request.bt_visible = TRUE;
            s_visible_pending_request.need_wait_aws = wait_aws_connect;
            app_bt_state_service_refresh_visible_timeout(timeout);
        }
    } else {
#ifdef MTK_AWS_MCE_ENABLE
        /* Partner send BT visibility(off) request to Agent. */
        if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
            app_bt_state_service_bt_visible_request_t aws_data = {
                .bt_visible = FALSE,
                .need_wait_aws = 0,
                .timeout = 0
            };
            if (BT_STATUS_SUCCESS != apps_aws_sync_event_send_extra(
                    EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_SET_BT_VISIBLE,
                    &aws_data,
                    sizeof(aws_data))) {
                APPS_LOG_MSGID_E(LOG_TAG" Partner send not discover request to agent fail", 0);
                ret = false;
            }
        } else if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
        {
            /* Agent stopped BT visibility. */
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
            bt_app_common_pre_set_ultra_low_latency(BT_APP_COMMON_ULL_LATENCY_MODULE_DISCOVERABLE, 20);
#endif
            bt_cm_discoverable(false);
        }
        s_visible_pending_request.bt_visible = false;
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
    }

    return ret;
}

const app_bt_state_service_status_t *app_bt_connection_service_get_current_status(void)
{
    return &s_current_status;
}

#ifdef MTK_AWS_MCE_ENABLE
void app_bt_state_service_set_air_pairing_doing(bool doing)
{
    APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_set_air_pairing_doing %d -> %d", 2, s_current_status.in_air_pairing, doing);
    if (s_current_status.in_air_pairing && !doing) {
        s_current_status.in_air_pairing = doing;
        app_bt_state_service_check_and_do_bt_enable_disable(false);
    } else {
        if (doing) {
            s_visible_pending_request.bt_visible = false;
        }
        s_current_status.in_air_pairing = doing;
    }
}
#endif

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
void app_bt_state_service_set_ull_air_pairing_doing(bool doing)
{
    APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_set_ull_air_pairing_doing %d -> %d", 2, s_current_status.in_ull_pairing, doing);
    if (s_current_status.in_ull_pairing && !doing) {
        s_current_status.in_ull_pairing = doing;
        app_bt_state_service_check_and_do_bt_enable_disable(false);
    } else {
        s_current_status.in_ull_pairing = doing;
    }
}
#endif

/******************************************************************************
****************************** Process events *********************************
******************************************************************************/
static bool app_bt_state_service_process_interaction_events(uint32_t event_id,
                                                            void *extra_data,
                                                            size_t data_len)
{
    bool ret = false;
    switch (event_id) {
        /* The old Agent will switch to new Partner if RHO successfully. */
        case APPS_EVENTS_INTERACTION_RHO_END:
            APPS_LOG_MSGID_I(LOG_TAG" RHO done", 0);
            /* The new partner need to check and turn off BT (no need try to trigger RHO). */
            app_bt_state_service_check_and_do_bt_enable_disable(false);
            break;
        case APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT:
            APPS_LOG_MSGID_I(LOG_TAG" received BT visible timeout", 0);
#ifdef MTK_AWS_MCE_ENABLE
            /* The Agent should disable BT visibility when BT visibility duration timeout. */
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT
                || bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_NONE)
#endif
            {
                s_visible_pending_request.bt_visible = false;
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
                bt_app_common_pre_set_ultra_low_latency(BT_APP_COMMON_ULL_LATENCY_MODULE_DISCOVERABLE, 20);
                apps_config_set_vp(VP_INDEX_PAIRING_END, false, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
#endif
                if (s_current_status.bt_visible) {
                    bt_cm_discoverable(false);
                }
            }
            ret = true;
            break;
        case APPS_EVENTS_INTERACTION_BT_RETRY_POWER_ON_OFF:
            APPS_LOG_MSGID_I(LOG_TAG" retry bt on/off", 0);
            app_bt_state_service_check_and_do_bt_enable_disable(false);
            ret = true;
            break;
        default:
            break;
    }

    return ret;
}

static bool app_bt_state_service_process_bt_cm_events(uint32_t event_id,
                                                      void *extra_data,
                                                      size_t data_len)
{
    bool ret = false;
    switch (event_id) {
        case BT_CM_EVENT_POWER_STATE_UPDATE: {
            bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)extra_data;
            if (NULL == power_update) {
                break;
            }

            if (BT_CM_POWER_STATE_ON == power_update->power_state) {
                /* BT off switch to on. */
                APPS_LOG_MSGID_I(LOG_TAG" power_update Power ON %x", 1, power_update->power_state);
                s_current_status.connection_state = APP_BT_CONNECTION_SERVICE_BT_STATE_DISCONNECTED;
                app_bt_state_service_check_and_do_bt_visible();
            } else if (BT_CM_POWER_STATE_OFF == power_update->power_state) {
                /* BT on switch to off. */
                APPS_LOG_MSGID_I(LOG_TAG" power_update Power OFF %x", 1, power_update->power_state);
                s_current_status.bt_visible = false;
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
                s_current_status.connection_state = APP_BT_CONNECTION_SERVICE_BT_STATE_BT_OFF;
                s_current_status.aws_connected = false;
            }
            break;
        }
        case BT_CM_EVENT_VISIBILITY_STATE_UPDATE: {
            bt_cm_visibility_state_update_ind_t *visible_update = (bt_cm_visibility_state_update_ind_t *)extra_data;
            if (NULL == visible_update) {
                break;
            }
            APPS_LOG_MSGID_I(LOG_TAG" visibility_state: %d", 1, visible_update->visibility_state);
            /* Update BT visibility state. */
            s_current_status.bt_visible = visible_update->visibility_state;
            if (!s_current_status.bt_visible) {
                /* No need visibility_timeout event if BT visibility disabled. */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
            } else {
                /* Disable BT visibility pending request if BT visibility enabled.*/
                s_visible_pending_request.bt_visible = false;
            }
#ifdef MTK_AWS_MCE_ENABLE
            bt_status_t send_aws_status;
            send_aws_status = apps_aws_sync_event_send_extra(
                                  EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                  APPS_EVENTS_INTERACTION_BT_VISIBLE_STATE_CHANGE,
                                  &s_current_status.bt_visible,
                                  sizeof(s_current_status.bt_visible));
            if (BT_STATUS_SUCCESS != send_aws_status) {
                APPS_LOG_MSGID_I(LOG_TAG"Fail to send bt visible change to partner : %d", 1, visible_update->visibility_state);
            }
#endif
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
            uint32_t latency = 20;
            if (visible_update->visibility_state) {
                latency = 60;
            }
            bt_app_common_pre_set_ultra_low_latency(BT_APP_COMMON_ULL_LATENCY_MODULE_DISCOVERABLE, latency);
#endif
            break;
        }
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
#ifdef MTK_AWS_MCE_ENABLE
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
            if (NULL == remote_update) {
                APPS_LOG_MSGID_E(LOG_TAG"remote_update is NULL", 0);
                break;
            }

            APPS_LOG_MSGID_I(LOG_TAG" REMOTE_INFO_UPDATE, acl_state(0x%x)->(0x%x), connected_service(0x%x)->(0x%x), reason:0x%x",
                             5, remote_update->pre_acl_state, remote_update->acl_state,
                             remote_update->pre_connected_service, remote_update->connected_service,
                             remote_update->reason);
            APPS_LOG_MSGID_I(LOG_TAG" REMOTE_INFO_UPDATE, (%x), aws_connected(%x), connection_state(%x)",
                             3, s_current_status.connection_state, s_current_status.aws_connected, s_current_status.connection_state);

#ifdef MTK_AWS_MCE_ENABLE
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
            {
                if (BT_CM_ACL_LINK_ENCRYPTED != remote_update->pre_acl_state
                    && BT_CM_ACL_LINK_ENCRYPTED == remote_update->acl_state
                    && 0 != memcmp(remote_update->address, bt_device_manager_get_local_address(), sizeof(bt_bd_addr_t))) {
                    APPS_LOG_MSGID_I(LOG_TAG" Disable pairing when encrypted", 0);
                    bt_cm_discoverable(false);
                    s_visible_pending_request.bt_visible = false;
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
                }
                if (!(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Update BT state as PROFILE_CONNECTED when Agent connected first non-AWS profile. */
                    APPS_LOG_MSGID_I(LOG_TAG" Agent Connected", 0);
                    s_current_status.connection_state = APP_BT_CONNECTION_SERVICE_BT_STATE_PROFILE_CONNECTED;
                } else if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                           && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state) {
                    /* BT ACL disconnection happen. */
                    if (APP_BT_CONNECTION_SERVICE_BT_STATE_ACL_CONNECTED <= s_current_status.connection_state
                        && bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0) == 0) {
                        /* Update BT state as DISCONNECTED if Agent all non-AWS profile disconnected. */
                        APPS_LOG_MSGID_I(LOG_TAG" Agent Disconnected", 0);
                        s_current_status.connection_state = APP_BT_CONNECTION_SERVICE_BT_STATE_DISCONNECTED;
                    }
                } else if (BT_CM_ACL_LINK_CONNECTED != remote_update->pre_acl_state
                           && BT_CM_ACL_LINK_CONNECTED == remote_update->acl_state
                           && 0 != memcmp(remote_update->address, bt_device_manager_get_local_address(), sizeof(bt_bd_addr_t))) {
                    APPS_LOG_MSGID_I(LOG_TAG" Agent ACL Connected", 0);
                    /* Update BT state as ACL_CONNECTED if Agent ACL connected. */
                    if (s_current_status.connection_state < APP_BT_CONNECTION_SERVICE_BT_STATE_ACL_CONNECTED) {
                        s_current_status.connection_state = APP_BT_CONNECTION_SERVICE_BT_STATE_ACL_CONNECTED;
                    }
                }

#ifdef MTK_AWS_MCE_ENABLE
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Agent update AWS connection state when AWS connected. */
                    s_current_status.aws_connected = true;
                    if (s_current_status.bt_visible) {
                        bt_status_t send_aws_status = apps_aws_sync_event_send_extra(
                                                          EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                          APPS_EVENTS_INTERACTION_BT_VISIBLE_STATE_CHANGE,
                                                          &s_current_status.bt_visible,
                                                          sizeof(s_current_status.bt_visible));
                        if (BT_STATUS_SUCCESS != send_aws_status) {
                            APPS_LOG_MSGID_I(LOG_TAG"Fail to send bt visible change to partner when aws connected", 0);
                        }
                    }
                    /* Check and enable BT visibility when Agent AWS connected. */
                    app_bt_state_service_check_and_do_bt_visible();
                    APPS_LOG_MSGID_I(LOG_TAG" Partner Attached.", 0);
#ifdef MTK_RACE_CMD_ENABLE
                    /* Agent send AWS state to APP via RACE command. */
                    race_bt_notify_aws_state(1);
#endif
                } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Agent update AWS connection state when AWS disconnected. */
                    s_current_status.aws_connected = false;
                    APPS_LOG_MSGID_I(LOG_TAG" Partner Detached.", 0);
#ifdef MTK_RACE_CMD_ENABLE
                    race_bt_notify_aws_state(0);
#endif
                }
#endif
            }
#ifdef MTK_AWS_MCE_ENABLE
            else if (role == BT_AWS_MCE_ROLE_PARTNER || role == BT_AWS_MCE_ROLE_CLINET) {
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    /* Partner update AWS connection state when AWS connected. */
                    s_current_status.aws_connected = true;
                    s_visible_pending_request.bt_visible = false;
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
                    if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                        /* Partner connected SP if AWS connected and link type is normal. */
                        s_current_status.connection_state = APP_BT_CONNECTION_SERVICE_BT_STATE_PROFILE_CONNECTED;
                    }
                } else if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                           && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state) {
                    /* Partner update connection state when ACL disconnected. */
                    if (APP_BT_CONNECTION_SERVICE_BT_STATE_ACL_CONNECTED <= s_current_status.connection_state) {
                        s_current_status.connection_state = APP_BT_CONNECTION_SERVICE_BT_STATE_DISCONNECTED;
                    }
                    s_current_status.aws_connected = false;
                    s_current_status.bt_visible = false;
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
                }
            }
#endif
        }
        break;

        default:
            break;
    }
    return ret;
}

static bool app_bt_state_service_process_bt_device_manager_events(
            uint32_t event_id,
            void *extra_data,
            size_t data_len)
{
    bool ret = false;
    bt_device_manager_power_event_t evt;
    bt_device_manager_power_status_t status;
    bt_event_get_bt_dm_event_and_status(event_id, &evt, &status);
    switch (evt) {
        case BT_DEVICE_MANAGER_POWER_EVT_PREPARE_ACTIVE:
            break;
        case BT_DEVICE_MANAGER_POWER_EVT_PREPARE_STANDBY:
            break;
        case BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE:
            APPS_LOG_MSGID_I(LOG_TAG" BT_DM Power ON. status: %x, current : %x", 2, status, s_current_status.current_power_state);
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                if (APP_BT_STATE_POWER_STATE_ENABLING == s_current_status.current_power_state) {
                    s_current_status.current_power_state = APP_BT_STATE_POWER_STATE_ENABLED;
                }
                app_bt_state_service_check_and_do_bt_enable_disable(false);
            }
            break;
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE:
            APPS_LOG_MSGID_I(LOG_TAG" BT_DM POWER OFF. status: %x, current : %x", 2, status, s_current_status.current_power_state);
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                if (APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLING == s_current_status.current_power_state) {
                    s_current_status.current_power_state = APP_HOME_SCREEN_BT_POWER_CLASSIC_DISABLED;
                } else if (APP_BT_STATE_POWER_STATE_DISABLING == s_current_status.current_power_state) {
                    s_current_status.current_power_state = APP_BT_STATE_POWER_STATE_DISABLED;
                }
                app_bt_state_service_check_and_do_bt_enable_disable(false);
                s_visible_pending_request.bt_visible = false;
            }
            break;
    }
    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool app_bt_state_service_process_aws_data_events(uint32_t event_id,
                                                         void *extra_data,
                                                         size_t data_len)
{
    bool ret = false;
    uint32_t aws_event_group;
    uint32_t aws_event_id;
    void *p_extra_data = NULL;
    uint32_t extra_data_len = 0;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (!aws_data_ind || aws_data_ind->module_id != BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        return ret;
    }

    apps_aws_sync_event_decode_extra(aws_data_ind, &aws_event_group, &aws_event_id,
                                     &p_extra_data, &extra_data_len);
    switch (aws_event_group) {
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            switch (aws_event_id) {
                /* Update Partner BT visibility when Agent visibility changed or AWS connected. */
                case APPS_EVENTS_INTERACTION_BT_VISIBLE_STATE_CHANGE: {
                    bool bt_visible = false;
                    if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()
                        && p_extra_data && extra_data_len == sizeof(bt_visible)) {
                        bt_visible = *(bool *)p_extra_data;
                        APPS_LOG_MSGID_I(LOG_TAG"Received bt_visible from agent : %d", 1,
                                         bt_visible);
                        if (s_current_status.bt_visible != bt_visible) {
                            s_current_status.bt_visible = bt_visible;
                        }
                    }
                    ret = true;
                }
                break;
                /* Agent trigger BT visibility on/off when Agent received SET_BT_VISIBLE event. */
                case APPS_EVENTS_INTERACTION_SET_BT_VISIBLE: {
                    app_bt_state_service_bt_visible_request_t *aws_data;
                    if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()
                        && p_extra_data && extra_data_len == sizeof(app_bt_state_service_bt_visible_request_t)) {
#ifdef MTK_AWS_MCE_ENABLE
                        if (s_current_status.in_air_pairing) {
                            APPS_LOG_MSGID_I(LOG_TAG" app_bt_state_service_set_bt_visible but in air pairing", 0);
                            s_visible_pending_request.bt_visible = false;
                            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_BT_VISIBLE_TIMEOUT);
                            ret = true;
                            break;
                        }
#endif
                        aws_data = (app_bt_state_service_bt_visible_request_t *)p_extra_data;
                        APPS_LOG_MSGID_I(LOG_TAG"Received set bt_visible from partner, enable: %d - timeout: %d", 2,
                                         aws_data->bt_visible, aws_data->timeout);
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
                        bt_app_common_pre_set_ultra_low_latency(BT_APP_COMMON_ULL_LATENCY_MODULE_DISCOVERABLE, 60);
#endif
                        apps_config_set_vp(VP_INDEX_PAIRING, false, 100, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                        bt_cm_discoverable(aws_data->bt_visible);
                        app_bt_state_service_refresh_visible_timeout(aws_data->timeout);
                        s_visible_pending_request.bt_visible = false;
                    }
                    ret = true;
                }
                break;
                default:
                    break;
            }
            break;

        default:
            break;
    }
    return ret;
}
#endif

bool app_bt_state_service_process_events(uint32_t event_group,
                                         uint32_t event_id,
                                         void *extra_data,
                                         size_t data_len)
{
    bool ret = false;
    switch (event_group) {
        /* UI Shell APP_INTERACTION events. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = app_bt_state_service_process_interaction_events(event_id, extra_data, data_len);
            break;
        /* UI Shell BT Connection Manager events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = app_bt_state_service_process_bt_cm_events(event_id, extra_data, data_len);
            break;
        /* UI Shell BT device Manager events. */
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER:
            ret = app_bt_state_service_process_bt_device_manager_events(event_id, extra_data, data_len);
            break;
#ifdef MTK_AWS_MCE_ENABLE
        /* UI Shell BT AWS_DATA events. */
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = app_bt_state_service_process_aws_data_events(event_id, extra_data, data_len);
            break;
#endif
        default:
            break;
    }
    return ret;
}
