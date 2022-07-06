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
 * File: app_smcharger_idle_activity.c
 *
 * Description: This file could sync BT/Battery state, check RHO for SmartCharger APP.
 *
 * Note: See doc/Airoha_IoT_SDK_Application_Developers_Guide.pdf for SmartCharger state machine.
 *
 */


#ifdef MTK_SMART_CHARGER_ENABLE

#include "app_smcharger_idle_activity.h"

#include "app_smcharger_startup_activity.h"

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#include "apps_config_features_dynamic_setting.h"
#include "apps_events_bt_event.h"
#include "battery_management.h"
#include "battery_management_core.h"
#include "bt_power_on_config.h"
#include "bt_aws_mce_srv.h"
#include "apps_customer_config.h"
#include "bt_aws_mce_report.h"
#include "bt_device_manager_power.h"
#ifdef MTK_FOTA_ENABLE
#include "app_fota_idle_activity.h"
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
#include "app_in_ear_utils.h"
#endif

#define SMCHARGER_IDLE_ACTIVITY "[SMCharger]idle_activity"

app_smcharger_context_t s_app_smcharger_context;    /* The variable records context */

static bool s_app_case_battery_sync_flag = FALSE;

/**
* @brief      Print app_smcharger_context information.
* @param[in]  ctx, global app_smcharger_context.
*/
static void app_smcharger_context_print(app_smcharger_context_t *ctx)
{
    APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", Print_SMCharger_Context role=0x%02X charger_exist=%d, charger_state=%d shutdown=%d "
                     "battery_percent=%d [partner=%d] smcharger_state=%d [peer=%d] "
                     "aws_state=0x%02X battery_state=%d", 10,
                     bt_device_manager_aws_local_info_get_role(),
                     ctx->charger_exist_state, ctx->charging_state, ctx->shutdown_state,
                     ctx->battery_percent, ctx->peer_battery_percent,
                     ctx->smcharger_state, ctx->peer_smcharger_state,
                     ctx->aws_state, ctx->battery_state);
}

/**
* @brief      Get app_smcharger_context->app_smcharger_battery_state_t.
* @param[in]  smcharger_ctx, global app_smcharger_context.
* @return     Return app_smcharger_battery_state_t.
*/
static app_smcharger_battery_state_t app_smcharger_get_battery_state(app_smcharger_context_t *smcharger_ctx)
{
    app_smcharger_battery_state_t new_state;
    app_smcharger_context_print(smcharger_ctx);
    if (smcharger_ctx->charger_exist_state) {
        if (smcharger_ctx->battery_percent == 100) {
            /* Only use APP_BATTERY_STATE_CHARGING_FULL to show Charging_Full LED (LED_INDEX_IDLE). */
            /* No need to send charger_complete event (EVENT_ID_SMCHARGER_CHARGER_COMPLETE_INTERRUPT). */
            new_state = SMCHARGER_BATTERY_STATE_CHARGING_FULL;
        } else if (smcharger_ctx->charging_state == CHARGER_STATE_THR) {
            new_state = SMCHARGER_BATTERY_STATE_THR;
        } else {
            new_state = SMCHARGER_BATTERY_STATE_CHARGING;
        }
    } else {
        if (smcharger_ctx->shutdown_state == APPS_EVENTS_BATTERY_SHUTDOWN_STATE_VOLTAGE_LOW) {
            new_state = SMCHARGER_BATTERY_STATE_SHUTDOWN;
        } else if (smcharger_ctx->battery_percent < APPS_BATTERY_LOW_THRESHOLD) {
            new_state = SMCHARGER_BATTERY_STATE_LOW_CAP;
        } else if (smcharger_ctx->battery_percent >= APPS_BATTERY_FULL_THRESHOLD) {
            new_state = SMCHARGER_BATTERY_STATE_FULL;
        } else {
            new_state = SMCHARGER_BATTERY_STATE_IDLE;
        }
    }

    if (smcharger_ctx->battery_state != new_state) {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", battery_state %d->%d", 2, smcharger_ctx->battery_state, new_state);
        /* Need to update background led pattern when app_battery_state changed. */
        /* Delay 50ms to wait SmartCharger activity switch. */
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN);
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_UPDATE_LED_BG_PATTERN, (void *)NULL, 0,
                            NULL, 50);
    } else {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", same battery_state=%d", 1, new_state);
    }
    return new_state;
}

/**
* @brief      SmartCharger APP check and do RHO.
*             Pre-condition: Agent role & AWS Attached & BT EDR Connected & not OTA ongoing
*             Case 1: Agent is in the case, but partner is out of case
*             Case 2: The battery percent of partner is 30% greater than agent (both out_of_case)
* @param[in]  smcharger_ctx, global app_smcharger_context.
*/
static void smcharger_agent_check_and_do_rho(app_smcharger_context_t *smcharger_ctx)
{
#ifdef APPS_AUTO_TRIGGER_RHO
    if (!apps_config_features_is_auto_rho_enabled()) {
        APPS_LOG_MSGID_E(SMCHARGER_IDLE_ACTIVITY", No Enable config_features_is_auto_rho", 0);
        return;
    }

#ifdef MTK_FOTA_ENABLE
     bool ota_ongoing = app_fota_get_ota_ongoing();
     if (ota_ongoing) {
         APPS_LOG_MSGID_E(SMCHARGER_IDLE_ACTIVITY", check_and_do_rho, OTA ongoing", 0);
         return;
     }
#endif

    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_srv_link_type_t aws_link_type = bt_aws_mce_srv_get_link_type();
    if (role == BT_AWS_MCE_ROLE_AGENT && smcharger_ctx->aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED
        && aws_link_type == BT_AWS_MCE_SRV_LINK_NORMAL) {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", AGENT & AWS ATTACHED & NORMAL Link, check RHO: ", 0);
        app_smcharger_context_print(smcharger_ctx);
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        app_in_ear_state_t in_ear_state = app_in_ear_get_state();
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", check RHO in_ear_state=%d", 1, in_ear_state);
#endif
        if (smcharger_ctx->battery_state != SMCHARGER_BATTERY_STATE_SHUTDOWN  /* SHUTDOWN battery state will notify HomeScreen to do power off and RHO. */
            && smcharger_ctx->battery_state < SMCHARGER_BATTERY_STATE_CHARGING                /* Agent not charging. */
            && smcharger_ctx->smcharger_state == STATE_SMCHARGER_OUT_OF_CASE            /* Agent out_of_case. */
            && smcharger_ctx->peer_smcharger_state == STATE_SMCHARGER_OUT_OF_CASE    /* Partner out_of_case. */
#ifdef MTK_IN_EAR_FEATURE_ENABLE
            && (in_ear_state == APP_IN_EAR_STA_BOTH_IN || in_ear_state == APP_IN_EAR_STA_BOTH_OUT) /* Both in/out ear. */
#endif
            && smcharger_ctx->peer_battery_percent < PARTNER_BATTERY_CHARGING        /* Partner not charging. */
            && smcharger_ctx->battery_percent + APPS_DIFFERENCE_BATTERY_VALUE_FOR_RHO   /* 30% difference. */
            < smcharger_ctx->peer_battery_percent) {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", trigger RHO due to low_battery", 0);
            /* Send TRIGGER_RHO event to notify HomeScreen APP do RHO. */
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_TRIGGER_RHO);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_TRIGGER_RHO, NULL, 0, NULL, 200);
        } else {
            bool in_case = (smcharger_ctx->smcharger_state == STATE_SMCHARGER_LID_OPEN
                             || smcharger_ctx->smcharger_state == STATE_SMCHARGER_LID_CLOSE);
            bool partner_out_case = (smcharger_ctx->peer_smcharger_state == STATE_SMCHARGER_OUT_OF_CASE);
            if (in_case && partner_out_case) {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", PREPARE_RHO due to in/out case, flag=%d",
                                 1, smcharger_ctx->agent_prepare_rho_flag);
                if (!smcharger_ctx->agent_prepare_rho_flag) {
                    smcharger_ctx->agent_prepare_rho_flag = TRUE;
                    /* For debounce, send SMCHARGER PREPARE_RHO event after 1000ms. */
                    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_CHARGER_CASE, EVENT_ID_SMCHARGER_PREPARE_RHO);
                    ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_CHARGER_CASE,
                                        EVENT_ID_SMCHARGER_PREPARE_RHO, NULL, 0, NULL, 1000);
                }
            } else {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", cancel PREPARE_RHO due to in/out case", 0);
                smcharger_ctx->agent_prepare_rho_flag = FALSE;
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_CHARGER_CASE, EVENT_ID_SMCHARGER_PREPARE_RHO);
            }
        }
    } else {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", check RHO condition fail: role=0x%02X aws=0x%02X aws_link_type=%d",
                         3, role, smcharger_ctx->aws_state, aws_link_type);
    }
#endif
}


/**
* @brief      Notify battery percent to Peer.
* @param[in]  smcharger_ctx, global app_smcharger_context.
*/
static void smcharger_notify_battery_to_peer(app_smcharger_context_t *smcharger_ctx)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (smcharger_ctx->aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [%02X] AWS not connected, no need notify battery",
                         1, role);
    } else {
        uint8_t info_array[sizeof(bt_aws_mce_report_info_t)];
        uint8_t data_array[sizeof(smcharger_ctx->battery_percent)];
        bt_aws_mce_report_info_t *aws_data = (bt_aws_mce_report_info_t *)&info_array;
        aws_data->module_id = BT_AWS_MCE_REPORT_MODULE_BATTERY;
        aws_data->is_sync = FALSE;
        aws_data->sync_time = 0;
        aws_data->param_len = sizeof(smcharger_ctx->battery_percent);
        if (SMCHARGER_BATTERY_STATE_CHARGING > smcharger_ctx->battery_state) {
            *(data_array) = smcharger_ctx->battery_percent;
        } else {
            *(data_array) = PARTNER_BATTERY_CHARGING | smcharger_ctx->battery_percent;
        }
        aws_data->param = (void *)data_array;
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [%02X] Send battery(%d) to peer",
                         2, role, *((uint8_t *)aws_data->param));
        bt_aws_mce_report_send_event(aws_data);
    }
}

static void partner_notify_case_battery_level_to_agent(app_smcharger_context_t *smcharger_ctx)
{
    if (smcharger_ctx->aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [Partner] AWS not connected, no need notify case_battery_level", 0);
        s_app_case_battery_sync_flag = TRUE;
    } else {
        bt_aws_mce_report_info_t aws_data;
        aws_data.module_id = BT_AWS_MCE_REPORT_MODULE_CHARGER_CASE;
        aws_data.is_sync = FALSE;
        aws_data.sync_time = 0;
        aws_data.param_len = sizeof(smcharger_ctx->case_battery_percent);
        aws_data.param = (void *)&smcharger_ctx->case_battery_percent;
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [Partner] Send case battery(%d) to agent",
                         1, smcharger_ctx->case_battery_percent);
        bt_aws_mce_report_send_event(&aws_data);
    }
}

/**
* @brief      Notify SmartCharger state to peer.
* @param[in]  smcharger_ctx, global app_smcharger_context.
*/
static void smcharger_notify_state_to_peer(app_smcharger_context_t *smcharger_ctx)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (smcharger_ctx->aws_state != BT_AWS_MCE_AGENT_STATE_ATTACHED) {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [%02X] No AWS connected, notify smcharger_state fail",
                         1, role);
        return;
    }
    if (smcharger_ctx->aws_state == BT_AWS_MCE_AGENT_STATE_ATTACHED
        && (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_PARTNER)) {
        uint8_t info_array[sizeof(bt_aws_mce_report_info_t)] = {0};
        uint8_t data_array[sizeof(smcharger_ctx->smcharger_state)] = {0};
        bt_aws_mce_report_info_t *aws_data = (bt_aws_mce_report_info_t *)&info_array;
        aws_data->module_id = BT_AWS_MCE_REPORT_MODULE_SMCHARGER;
        aws_data->is_sync = FALSE;
        aws_data->sync_time = 0;
        aws_data->param_len = sizeof(smcharger_ctx->smcharger_state);
        memcpy((uint8_t *)data_array, &smcharger_ctx->smcharger_state, sizeof(smcharger_ctx->smcharger_state));
        aws_data->param = (void *)data_array;
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [%02X] Send smcharger_state(%d) to peer",
                         2, role, smcharger_ctx->smcharger_state);
        bt_aws_mce_report_send_event(aws_data);
    }
}

static bool smcharger_idle_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = TRUE;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", create", 0);
            /* Init SmartCharger APP. */
            app_smcharger_init();

            /* Init SmartCharger APP Context. */
            self->local_context = &s_app_smcharger_context;
            {
                app_smcharger_context_t *smcharger_ctx = (app_smcharger_context_t *)self->local_context;
                memset(self->local_context, 0, sizeof(app_smcharger_context_t));
                smcharger_ctx->battery_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
                smcharger_ctx->charging_state = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_STATE);
                smcharger_ctx->charger_exist_state = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
                smcharger_ctx->shutdown_state = calculate_shutdown_state(battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE));
                smcharger_ctx->aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;
                smcharger_ctx->peer_battery_percent = SMCHARGER_BATTERY_INVALID;
                smcharger_ctx->battery_state = SMCHARGER_BATTERY_STATE_IDLE;
                smcharger_ctx->smcharger_state = STATE_SMCHARGER_NONE;
                smcharger_ctx->peer_smcharger_state = STATE_SMCHARGER_NONE;
                smcharger_ctx->case_battery_percent = app_smcharger_read_case_battery_nvkey();
                smcharger_ctx->agent_prepare_rho_flag = FALSE;
                smcharger_ctx->case_lid_close_flag = FALSE;
                smcharger_ctx->bt_clear_ongoing = FALSE;

                smcharger_ctx->battery_state = app_smcharger_get_battery_state(smcharger_ctx);
                /* Set g_smcharger_context in app_smcharger_utils.c. */
                app_smcharger_set_context(smcharger_ctx);
                app_smcharger_update_bat();
            }

            /* Start STARTUP activity (start SmartCharger APP state machine). */
            ui_shell_start_activity(self, (ui_shell_proc_event_func_t)app_smcharger_startup_activity_proc,
                                    ACTIVITY_PRIORITY_MIDDLE, (void *)NULL, 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", destroy", 0);
            app_smcharger_set_context(NULL);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", resume", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", pause", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", refresh", 0);
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", result", 0);
            break;
        }
        default:
            break;
    }
    return ret;
}

/**
* @brief      Update battery state and Handle RHO.
* @param[in]  smcharger_ctx, global app_smcharger_context.
* @param[in]  need_rho, set TRUE when charger_in/charger_out or battery_percent changed.
*/
static void smcharger_idle_update_battery_state(app_smcharger_context_t *smcharger_ctx, bool need_rho)
{
    app_smcharger_battery_state_t new_state = app_smcharger_get_battery_state(smcharger_ctx);
    app_smcharger_battery_state_t old_state = smcharger_ctx->battery_state;
    if (new_state == old_state) {
        /* Do nothing when app_battery_state not changed. */
    } else if (new_state == SMCHARGER_BATTERY_STATE_SHUTDOWN
               && old_state != SMCHARGER_BATTERY_STATE_SHUTDOWN) {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", Start Power Off", 0);
    } else if (new_state == SMCHARGER_BATTERY_STATE_LOW_CAP
               && old_state >= SMCHARGER_BATTERY_STATE_IDLE) {
        APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", Start Low_Battery VP", 0);
        /* Play "no battery" VP when APP_BATTERY_STATE_LOW_CAP. */
        apps_config_set_vp(VP_INDEX_LOW_BATTERY, FALSE, 0, VOICE_PROMPT_PRIO_MEDIUM, FALSE, NULL);
    }
    smcharger_ctx->battery_state = new_state;

    if (SMCHARGER_BATTERY_STATE_SHUTDOWN == smcharger_ctx->battery_state) {
        /* Power off device when battery state is APP_BATTERY_STATE_SHUTDOWN. */
        app_smcharger_power_off(TRUE);
    } else {
        /* Partner notify its battery_percent to Agent, and Agent check and do RHO when need_rho is TRUE. */
        bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
        if (need_rho) {
            if (role == BT_AWS_MCE_ROLE_AGENT) {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [Agent] check_and_do_rho due to battery_event", 0);
                smcharger_agent_check_and_do_rho(smcharger_ctx);
                smcharger_notify_battery_to_peer(smcharger_ctx);
            } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                smcharger_notify_battery_to_peer(smcharger_ctx);
            }
        }
    }
}

static bool smcharger_idle_battery_event_group(
    struct _ui_shell_activity *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = FALSE;
    bool update_battery_state = TRUE;
    bool need_rho = FALSE;
    app_smcharger_context_t *smcharger_ctx = (app_smcharger_context_t *)self->local_context;
    switch (event_id) {
        /* Handle battery_percent changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_PERCENT_CHANGE: {
            uint8_t old_battery = smcharger_ctx->battery_percent;
            smcharger_ctx->battery_percent = (int32_t)extra_data;
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [DRV]Current battery_percent=%d->%d",
                             2, old_battery, (int32_t)extra_data);
            app_smcharger_update_bat();
            need_rho = TRUE;

            // Notify other APP when own battery changed
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if (role == BT_AWS_MCE_ROLE_AGENT && old_battery != smcharger_ctx->battery_percent) {
                app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_CHANGED_EVENT, NULL, 0);
            }
            break;
        }
        /* Handle charger_state changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_CHARGER_STATE_CHANGE:
            smcharger_ctx->charging_state = (int32_t)extra_data;
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [DRV]Current charging_state=%d", 1, smcharger_ctx->charging_state);
            break;
        /* Handle shutdown_state changed, event from apps_events_battery_event.c. */
        case APPS_EVENTS_BATTERY_SHUTDOWN_STATE_CHANGE:
            smcharger_ctx->shutdown_state = (battery_event_shutdown_state_t)extra_data;
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [DRV]Current shutdown_state=%d", 1, smcharger_ctx->shutdown_state);
            break;
        default:
            // APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [DRV]Doesn't care battery event: %d", 1, event_id);
            /* Ignore other battery event. */
            update_battery_state = FALSE;
            break;
    }

    if (update_battery_state) {
        smcharger_idle_update_battery_state(smcharger_ctx, need_rho);
    }
    return ret;
}

static bool smcharger_idle_bt_cm_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = FALSE;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    app_smcharger_context_t *smcharger_ctx = (app_smcharger_context_t *)self->local_context;

    /* Handle new BT_CM Event. */
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == remote_update || NULL == smcharger_ctx) {
                break;
            }

            bool need_check_rho = FALSE;
            uint8_t *addr = (uint8_t *)remote_update->address;
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", bt_info_update addr=%02X:%02X:%02X:%02X:%02X:%02X acl=%d->%d srv=0x%04X->0x%04X",
                             10, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
                             remote_update->pre_acl_state, remote_update->acl_state,
                             remote_update->pre_connected_service, remote_update->connected_service);
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role) {
                /* Check Agent AWS connection and set AWS state. */
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    smcharger_ctx->aws_state = BT_AWS_MCE_AGENT_STATE_ATTACHED;
                    smcharger_notify_battery_to_peer(smcharger_ctx);
                    smcharger_notify_state_to_peer(smcharger_ctx);
                    need_check_rho = TRUE;
                } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    smcharger_ctx->peer_battery_percent = SMCHARGER_BATTERY_INVALID;
                    smcharger_ctx->peer_smcharger_state = STATE_SMCHARGER_NONE;
                    smcharger_ctx->aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                    /* Notify Power_saving APP mode changed, then determine "is_need_power_saving". */
                    app_power_save_utils_notify_mode_changed(FALSE, app_smcharger_get_power_saving_target_mode);
#endif
                    // Notify other APP when AWS disconnected (peer battery/state will be INVALID)
                    app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_CHANGED_EVENT, NULL, 0);
                }

                /* Check RHO if BT_CM connected_service add new profile connection. */
                if (need_check_rho) {
                    APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [Agent] check_and_do_rho due to AWS connected", 0);
                    smcharger_agent_check_and_do_rho(smcharger_ctx);
                }
            } else if (BT_AWS_MCE_ROLE_PARTNER == role) {
                /* Check Partner AWS connection, then notify battery and SmartCharger state to Agent. */
                if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    smcharger_ctx->aws_state = BT_AWS_MCE_AGENT_STATE_ATTACHED;
                    smcharger_notify_battery_to_peer(smcharger_ctx);
                    smcharger_notify_state_to_peer(smcharger_ctx);
                    if (s_app_case_battery_sync_flag) {
                        partner_notify_case_battery_level_to_agent(smcharger_ctx);
                        s_app_case_battery_sync_flag = FALSE;
                    }
                } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    smcharger_ctx->peer_battery_percent = SMCHARGER_BATTERY_INVALID;
                    smcharger_ctx->peer_smcharger_state = STATE_SMCHARGER_NONE;
                    smcharger_ctx->aws_state = BT_AWS_MCE_AGENT_STATE_INACTIVE;

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                    /* Notify Power_saving APP mode changed, then determine "is_need_power_saving". */
                    app_power_save_utils_notify_mode_changed(FALSE, app_smcharger_get_power_saving_target_mode);
#endif
                }
            }
            /* Only AWS connected after BT Clear->On. */
            if (remote_update->pre_connected_service == 0
                    && remote_update->connected_service == BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)) {
                app_smcharger_bt_clear_enter_discoverable(APP_SMCHARGER_REQUEST_BT_DISCOVERABLE);
            }
        }
        break;
        default:
            break;
    }
    return ret;
}

static bool smcharger_idle_bt_dm_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_device_manager_power_event_t event = 0;
    bt_device_manager_power_status_t status = 0;
    bt_event_get_bt_dm_event_and_status(event_id, &event, &status);
    APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", BT DM event=%d status=%d", 2, event, status);
    switch (event) {
        case BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_STANDBY_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", smcharger BT classic power off", 0);
            }
            break;
        }
        case BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_ACTIVE_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", smcharger BT classic power on", 0);
            }
            break;
        }
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", smcharger BT power off", 0);
                app_smcharger_bt_clear_enter_discoverable(APP_SMCHARGER_REQUEST_BT_CLEAR_ALL);
            }
            break;
        }
        case BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", smcharger BT power on", 0);
            }
            break;
        }
    }
    return FALSE;
}

static bool smcharger_idle_aws_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = FALSE;
    bool need_check_rho = FALSE;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    app_smcharger_context_t *smcharger_ctx = (app_smcharger_context_t *)self->local_context;
    /* Agent received Partner battery and SmartCharger state via AWS_MCE, then update battery. */
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (role == BT_AWS_MCE_ROLE_AGENT) {
        if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_CHARGER_CASE) {
            configASSERT(aws_data_ind->param_len == sizeof(smcharger_ctx->case_battery_percent));
            memcpy(&smcharger_ctx->case_battery_percent, aws_data_ind->param, sizeof(smcharger_ctx->case_battery_percent));
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [Agent] Received partner case_battery=%d", 1, smcharger_ctx->case_battery_percent);
            app_smcharger_update_case_battery_nvkey(smcharger_ctx->case_battery_percent);
            app_smcharger_update_bat();
        }
    }
    if (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_PARTNER) {
        if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_BATTERY) {
            configASSERT(aws_data_ind->param_len == sizeof(smcharger_ctx->peer_battery_percent));
            uint8_t old_peer_battery = smcharger_ctx->peer_battery_percent;
            memcpy(&smcharger_ctx->peer_battery_percent, aws_data_ind->param, sizeof(smcharger_ctx->peer_battery_percent));
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [%02X] Received peer battery=%d->%d",
                             3, role, old_peer_battery, smcharger_ctx->peer_battery_percent);
            app_smcharger_update_bat();
            if (role == BT_AWS_MCE_ROLE_AGENT) {
                need_check_rho = TRUE;

                // Notify other APP when peer battery changed
                if (old_peer_battery != smcharger_ctx->peer_battery_percent) {
                    app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_CHANGED_EVENT, NULL, 0);
                }
            }
        } else if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_SMCHARGER) {
            configASSERT(aws_data_ind->param_len == sizeof(smcharger_ctx->peer_smcharger_state));
            int old_state = smcharger_ctx->peer_smcharger_state;
            memcpy(&smcharger_ctx->peer_smcharger_state, aws_data_ind->param, sizeof(smcharger_ctx->peer_smcharger_state));
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [%02X] Received peer smcharger_state=%d->%d",
                             3, role, old_state, smcharger_ctx->peer_smcharger_state);

            int state = smcharger_ctx->peer_smcharger_state;
            /* SmartCharger APP send IN_OUT_EVENT. */
            if (state == STATE_SMCHARGER_OUT_OF_CASE
                    || (old_state != STATE_SMCHARGER_NONE &&
                        old_state != STATE_SMCHARGER_LID_CLOSE
                        && state == STATE_SMCHARGER_LID_OPEN)) {
                app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_IN_OUT_EVENT, NULL, 0);
            }

            if (role == BT_AWS_MCE_ROLE_AGENT) {
                need_check_rho = TRUE;

                // Notify other APP when peer smcharger_state changed
                app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_CHANGED_EVENT, NULL, 0);

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                /* Notify Power_saving APP mode changed, then determine "is_need_power_saving". */
                app_power_save_utils_notify_mode_changed(FALSE, app_smcharger_get_power_saving_target_mode);
#endif
            }
        }
    }

    if (need_check_rho) {
        /* Check RHO when Agent received Partner battery & SmartCharger state. */
        smcharger_agent_check_and_do_rho(smcharger_ctx);
    }
    return ret;
}

static bool smcharger_idle_interaction_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = FALSE;

    switch (event_id) {
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
        /* The old Agent will switch to new Partner if RHO successfully. */
        case APPS_EVENTS_INTERACTION_RHO_END: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [new Partner] Received RHO end", 0);
            app_smcharger_context_t *smcharger_ctx = (app_smcharger_context_t *)self->local_context;
            smcharger_ctx->agent_prepare_rho_flag = FALSE;
            app_rho_result_t rho_ret = (app_rho_result_t)extra_data;
            if (APP_RHO_RESULT_SUCCESS == rho_ret) {
                /* Set Partner battery percent to invalid value when Agent switched to Partner. */
                smcharger_notify_battery_to_peer(smcharger_ctx);
                partner_notify_case_battery_level_to_agent(smcharger_ctx);
                smcharger_notify_state_to_peer(smcharger_ctx);
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [new Partner] RHO success", 0);
            } else {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [new Partner] RHO fail, agent retry check", 0);
                /* Retry check RHO when old Agent RHO fail. */
                smcharger_agent_check_and_do_rho(smcharger_ctx);
            }
            break;
        }
        /* The old Partner will switch to new Agent if RHO successfully. */
        case APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [new Agent] Received RHO end", 0);
            app_smcharger_context_t *smcharger_ctx = (app_smcharger_context_t *)self->local_context;
            smcharger_ctx->agent_prepare_rho_flag = FALSE;
            app_rho_result_t rho_ret = (app_rho_result_t)extra_data;
            if (APP_RHO_RESULT_SUCCESS == rho_ret) {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [new Agent] RHO success", 0);
                smcharger_notify_battery_to_peer(smcharger_ctx);
                smcharger_notify_state_to_peer(smcharger_ctx);
            } else {
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [new Agent] RHO fail", 0);
            }
            break;
        }
#endif
#ifdef MTK_IN_EAR_FEATURE_ENABLE
        case APPS_EVENTS_INTERACTION_IN_EAR_UPDATE_STA: {
            app_in_ear_sta_info_t *sta_info = (app_in_ear_sta_info_t *)extra_data;
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [IN_EAR_UPDATE] %d->%d", 2, sta_info->previous, sta_info->current);
            if (sta_info->current == APP_IN_EAR_STA_BOTH_IN
                    || sta_info->current == APP_IN_EAR_STA_BOTH_OUT) {
                app_smcharger_context_t *smcharger_ctx = (app_smcharger_context_t *)self->local_context;
                smcharger_agent_check_and_do_rho(smcharger_ctx);
            }
            break;
        }
#endif
        default:
            // APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", Not supported event id = %d", 1, event_id);
            break;
    }
    return ret;
}

static bool smcharger_idle_proc_key_event_group(
    ui_shell_activity_t *self,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = FALSE;
    return ret;
}

bool app_smcharger_idle_activity_proc(
    struct _ui_shell_activity *self,
    uint32_t event_group,
    uint32_t event_id,
    void *extra_data,
    size_t data_len)
{
    bool ret = FALSE;
    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = smcharger_idle_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell key events - ignore all key events when device in the SmartCharger case. */
        case EVENT_GROUP_UI_SHELL_KEY:
            ret = smcharger_idle_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell battery events - handle battery_change/charger_state/shutdown_state from battery event. */
        case EVENT_GROUP_UI_SHELL_BATTERY:
            ret = smcharger_idle_battery_event_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell APP_INTERACTION events. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = smcharger_idle_interaction_event_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell BT Connection Manager events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = smcharger_idle_bt_cm_event_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell BT Device Manager events. */
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER:
            ret = smcharger_idle_bt_dm_event_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell BT AWS_DATA events. */
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = smcharger_idle_aws_event_group(self, event_id, extra_data, data_len);
            break;
        /* APP SmartCharger events. */
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE: {
            APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", CHARGER_CASE group, event_id=%d", 1, event_id);
            app_smcharger_context_t *smcharger_ctx = (app_smcharger_context_t *)self->local_context;
            ret = TRUE;
            if (event_id == EVENT_ID_SMCHARGER_SYNC_STATE) {
                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
                if (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_PARTNER) {
                    if (role == BT_AWS_MCE_ROLE_AGENT) {
                        smcharger_agent_check_and_do_rho(smcharger_ctx);
                        // Notify other APP when own smcharger_state changed
                        app_smcharger_ui_shell_event(EVENT_ID_SMCHARGER_NOTIFY_CHANGED_EVENT, NULL, 0);
                    }
                    smcharger_notify_state_to_peer(smcharger_ctx);
                }
            } else if (event_id == EVENT_ID_SMCHARGER_CASE_BATTERY_REPORT) {
                app_smcharger_update_bat();
                app_smcharger_update_case_battery_nvkey((int)extra_data);
                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
                if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    partner_notify_case_battery_level_to_agent(smcharger_ctx);
                }
            } else if (event_id == EVENT_ID_SMCHARGER_PREPARE_RHO) {
                /* Handle PREPARE_RHO event - send event to HomeScreen. */
                bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                        APPS_EVENTS_INTERACTION_TRIGGER_RHO, NULL, 0, NULL, 0);
                }
            } else if (event_id == EVENT_ID_SMCHARGER_CHARGER_OUT_PATTERN) {
                /* Update charger_exist_state/battery_percent and update battery_state when receive CHARGER_OUT event. */
                smcharger_ctx->charger_exist_state = 0;
                smcharger_ctx->battery_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", CHARGER_OUT = Charger_exist(0) battery_percent=%d", 1, smcharger_ctx->battery_percent);
                smcharger_idle_update_battery_state(smcharger_ctx, TRUE);
            } else if (event_id == EVENT_ID_SMCHARGER_CHARGER_IN_INTERRUPT) {
                /* Update charger_exist_state/battery_percent/shutdonw_state and update battery_state when receive CHARGER_IN event. */
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", CHARGER_IN = Charger_exist(1)", 0);
                smcharger_ctx->charger_exist_state = 1;
                uint8_t battery_percent = battery_management_get_battery_property(BATTERY_PROPERTY_CAPACITY);
                smcharger_ctx->shutdown_state = calculate_shutdown_state(battery_management_get_battery_property(BATTERY_PROPERTY_VOLTAGE));
                APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", [DRV]Current charger_exist=1 battery_percent=%d (%d) shutdown_state=%d",
                                 3, smcharger_ctx->battery_percent, battery_percent, smcharger_ctx->shutdown_state);
                smcharger_idle_update_battery_state(smcharger_ctx, TRUE);
            } else if (event_id == EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT
                    || event_id == EVENT_ID_SMCHARGER_NOTIFY_IN_OUT_EVENT
                    || event_id == EVENT_ID_SMCHARGER_NOTIFY_CHANGED_EVENT) {
                /* Other APP should only receive and use public event. */
                ret = FALSE;
            } else {
                // APPS_LOG_MSGID_I(SMCHARGER_IDLE_ACTIVITY", unexpected smcharger event", 0);
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

#endif
