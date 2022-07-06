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
 * File: app_rho_idle_activity.c
 *
 * Description: This file could trigger RHO and send RHO event to notify other APP.
 *
 */

#include <string.h>

#include "app_rho_idle_activity.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_debug.h"
#include "ui_shell_manager.h"
#include "FreeRTOS.h"
#include "bt_sink_srv.h"
#include "bt_device_manager.h"
#include "multi_ble_adv_manager.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#endif
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "bt_role_handover.h"
#endif
#include "app_hfp_utils.h"
#include "race_cmd_hostaudio.h"
#include "race_xport.h"
#include "bt_sink_srv_ami.h"
#include "app_home_screen_idle_activity.h"

#define RHO_IDLE_ACTIVITY "[RHO_IDLE]"

/**
 *  @brief This enum defines the states of RHO APP.
 */
typedef enum {
    APP_RHO_STATE_IDLE,
    APP_RHO_STATE_DOING,
    APP_RHO_STATE_END
} app_rho_state;

/**
 *  @brief This structure defines the context of RHO APP.
 */
typedef struct {
    bool aws_state;                     /**<  AWS connection flag. */
    app_rho_state rho_state;            /**<  RHO APP state. */
} app_rho_context_t;

/* Global context for RHO APP. */
static app_rho_context_t s_app_rho_context;

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE

static void _do_rho(app_rho_context_t *local_ctx);

/**
* @brief      Notify RHO result to SP APP via RACE command.
* @param[in]  status, TRUE - RHO successfully, FALSE - RHO fail.
*/
static void _notify_rho_done_to_smart_phone(uint8_t status)
{
    typedef struct {
        race_mmi_module_t module;
        uint8_t status;
        int8_t audio_channel;
    } PACKED RACE_MMI_SET_ENUM_EVT_STRU;
    race_status_t race_ret;
    APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" RHO end, partner->agent", 0);

    RACE_MMI_SET_ENUM_EVT_STRU *pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND,
                                                        (uint16_t)RACE_MMI_SET_ENUM,
                                                        (uint16_t)sizeof(RACE_MMI_SET_ENUM_EVT_STRU),
                                                        RACE_SERIAL_PORT_TYPE_SPP);
    if (pEvt) {
        pEvt->module = RACE_MMI_MODULE_RHO_DONE;
        pEvt->status = status;
        pEvt->audio_channel = ami_get_audio_channel();
        race_ret = race_flush_packet((uint8_t *)pEvt, RACE_SERIAL_PORT_TYPE_SPP);
        APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" RHO end, send SPP notify ret=%d", 1, race_ret);
    }
    pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND,
                            (uint16_t)RACE_MMI_SET_ENUM,
                            (uint16_t)sizeof(RACE_MMI_SET_ENUM_EVT_STRU),
                            RACE_SERIAL_PORT_TYPE_BLE);
    if (pEvt) {
        pEvt->module = RACE_MMI_MODULE_RHO_DONE;
        pEvt->status = status;
        pEvt->audio_channel = ami_get_audio_channel();
        race_ret = race_flush_packet((uint8_t *)pEvt, RACE_SERIAL_PORT_TYPE_BLE);
        APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" RHO end, send BLE notify ret=%d", 1, race_ret);
    }
#ifdef MTK_GATT_OVER_BREDR_ENABLE
    pEvt = RACE_ClaimPacket((uint8_t)RACE_TYPE_COMMAND,
                            (uint16_t)RACE_MMI_SET_ENUM,
                            (uint16_t)sizeof(RACE_MMI_SET_ENUM_EVT_STRU),
                            RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR);
    if (pEvt) {
        pEvt->module = RACE_MMI_MODULE_RHO_DONE;
        pEvt->status = status;
        pEvt->audio_channel = ami_get_audio_channel();
        race_ret = race_flush_packet((uint8_t *)pEvt, RACE_SERIAL_PORT_TYPE_GATT_OVER_BREDR);
        APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" RHO end, send GATT over EDR notify ret=%d", 1, race_ret);
    }
#endif
}

static bt_status_t _battery_role_handover_allow_callback(const bt_bd_addr_t *addr)
{
    home_screen_local_context_type_t *local_ctx = app_home_screen_idle_activity_get_context();
    app_bt_conn_sync_addr_list(local_ctx);

    return BT_STATUS_SUCCESS;
}


static void _battery_role_handover_service_status_callback(const bt_bd_addr_t *addr,
                                                           bt_aws_mce_role_t role,
                                                           bt_role_handover_event_t event,
                                                           bt_status_t status)
{
    app_rho_result_t result;

    multi_ble_rho_status_callback(addr, role, event, status);
    /* Send RHO APP events when received BT RHO status callback. */
    if (BT_ROLE_HANDOVER_START_IND == event) {
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_RHO_STARTED, (void *)(uint32_t)role, 0,
                            NULL, 0);
    } else if (BT_ROLE_HANDOVER_COMPLETE_IND == event) {
        if (status == BT_STATUS_CONNECTION_IN_SNIFF || status == BT_AWS_MCE_RHO_ERROR_SNIFF_MODE) {
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" fail reason in sniff, retry", 0);
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_TRIGGER_RHO, NULL, 0,
                                NULL, 0);
            return;
        }
        if (role == BT_AWS_MCE_ROLE_AGENT) {
            if (status == BT_STATUS_SUCCESS) {
                result = APP_RHO_RESULT_SUCCESS;
                APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" RHO end, agent->partner", 0);
            } else {
                result = APP_RHO_RESULT_FAIL;
            }
            /* Old Agent send RHO_END event. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_RHO_END, (void *)result, 0,
                                NULL, 0);
        } else {
            if (status == BT_STATUS_SUCCESS) {
                APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" RHO end, partner->agent", 0);
                result = APP_RHO_RESULT_SUCCESS;
            } else {
                result = APP_RHO_RESULT_FAIL;
            }
            /* Old Partner send PARTNER_SWITCH_TO_AGENT event. */
            ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT, (void *)result, 0,
                                NULL, 0);
        }
    }
}

static void _do_rho(app_rho_context_t *local_ctx)
{
    /* Cannot do RHO if AWS isn't normal link. */
    if ((BT_AWS_MCE_SRV_LINK_NORMAL != bt_aws_mce_srv_get_link_type()
        || BT_AWS_MCE_ROLE_AGENT != bt_device_manager_aws_local_info_get_role())
        && local_ctx->rho_state != APP_RHO_STATE_DOING) {
        APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" AWS isn't LINK_NORMAL, cannot do RHO", 0);
        local_ctx->rho_state = APP_RHO_STATE_END;
        goto exit;
    }

#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    /* Call BT RHO API and update RHO state. */
    if (bt_role_handover_get_state() == BT_ROLE_HANDOVER_STATE_IDLE) {
        bt_status_t bt_status = bt_role_handover_start();
        if (bt_status != BT_STATUS_SUCCESS) {
            APPS_LOG_MSGID_E(RHO_IDLE_ACTIVITY" Start RHO failed bt_status=0x%08X", 1, bt_status);
            local_ctx->rho_state = APP_RHO_STATE_END;
        } else {
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" RHO started successfully", 0);
            local_ctx->rho_state = APP_RHO_STATE_DOING;
        }
    } else {
        APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" RHO already on going", 0);
        local_ctx->rho_state = APP_RHO_STATE_DOING;
    }
#endif

exit:
    /* Send RHO_END event with fail result if RHO state isn't APP_RHO_STATE_DOING. */
    if (APP_RHO_STATE_DOING != local_ctx->rho_state) {
        ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                            APPS_EVENTS_INTERACTION_RHO_END, (void *)APP_RHO_RESULT_FAIL, 0,
                            NULL, 0);
    }
}
#endif

static bool _proc_ui_shell_group(struct _ui_shell_activity *self,
                                 uint32_t event_id,
                                 void *extra_data,
                                 size_t data_len)
{
    bool ret = true;
    app_rho_context_t *local_ctx = (app_rho_context_t *)self->local_context;

    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" rho_idle_activity create", 0);
            /* Init APP RHO context and register RHO callback. */
            self->local_context = &s_app_rho_context;
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            bt_role_handover_callbacks_t role_callbacks = {
                _battery_role_handover_allow_callback,
                NULL,
                NULL,
                NULL,
                _battery_role_handover_service_status_callback
            };
#endif
            local_ctx = (app_rho_context_t *)self->local_context;
            memset(self->local_context, 0, sizeof(app_rho_context_t));
            local_ctx->aws_state = false;
            local_ctx->rho_state = APP_RHO_STATE_IDLE;
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
            bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_BATTERY, &role_callbacks);
#endif
            break;
        }
        case EVENT_ID_SHELL_SYSTEM_ON_DESTROY:
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" rho_idle_activity destroy", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESUME:
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" rho_idle_activity resume", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_PAUSE:
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" rho_idle_activity pause", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_REFRESH:
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" rho_idle_activity refresh", 0);
            break;
        case EVENT_ID_SHELL_SYSTEM_ON_RESULT:
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" rho_idle_activity result", 0);
            break;
        default:
            break;
    }
    return ret;
}

static bool rho_idle_bt_cm_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    app_rho_context_t *local_ctx = (app_rho_context_t *)self->local_context;

    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == local_ctx || NULL == remote_update) {
                break;
            }
#ifdef MTK_AWS_MCE_ENABLE
            /* Check Agent and Partner AWS connection state. */
            if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                local_ctx->aws_state = true;
            } else if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                       && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                local_ctx->aws_state = false;
            }
#endif
        }
        break;
    }
    return ret;
}

static bool _app_interaction_event_proc(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    app_rho_context_t *local_ctx = (app_rho_context_t *)self->local_context;
    bool ret = false;
    switch (event_id) {
        case APPS_EVENTS_INTERACTION_TRIGGER_RHO:
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" Received RHO trigger", 0);
            /* Start RHO after received TRIGGER_RHO event. */
            _do_rho(local_ctx);
            ret = true;
            break;
        case APPS_EVENTS_INTERACTION_RHO_END: {
            local_ctx->rho_state = APP_RHO_STATE_IDLE;
            app_rho_result_t result = (app_rho_result_t)extra_data;
            if (APP_RHO_RESULT_FAIL == result) {
                _notify_rho_done_to_smart_phone(result);
            }
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" Received RHO end", 0);
            break;
        }
        case APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT: {
            app_rho_result_t result = (app_rho_result_t)extra_data;
            if (APP_RHO_RESULT_SUCCESS == result) {
                _notify_rho_done_to_smart_phone(result);
            }
            break;
        }
        default:
            APPS_LOG_MSGID_I(RHO_IDLE_ACTIVITY" Not supported event id=%d", 1, event_id);
            break;
    }
    return ret;
}

bool app_rho_idle_activity_proc(struct _ui_shell_activity *self,
                                uint32_t event_group,
                                uint32_t event_id,
                                void *extra_data,
                                size_t data_len)
{
    bool ret = false;

    switch (event_group) {
        /* UI Shell internal events. */
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = _proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        /* UI Shell new BT_CM events. */
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = rho_idle_bt_cm_event_proc(self, event_id, extra_data, data_len);
            break;
        /* UI Shell APP_INTERACTION events. */
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            ret = _app_interaction_event_proc(self, event_id, extra_data, data_len);
            break;
        default:
            break;
    }
    return ret;
}
