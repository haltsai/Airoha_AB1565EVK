/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 * File: app_advance_passthrough.c
 *
 * Description: This file provide some API to implement and limit advance passthrough feature.
 *
 */


#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE

#include "app_advance_passthrough.h"

#include "apps_aws_sync_event.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_config_key_remapper.h"

#ifdef MTK_ANC_ENABLE
#include "app_anc_service.h"
#endif
#ifdef MTK_RACE_CMD_ENABLE
#include "apps_race_cmd_event.h"
#endif
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#endif
#if defined(SUPPORT_ROLE_HANDOVER_SERVICE) && defined(BT_ROLE_HANDOVER_WITH_SPP_BLE)
#include "bt_role_handover.h"
#endif
#ifdef HAL_DVFS_MODULE_ENABLED
#include "hal_dvfs.h"
#include "hal_dvfs_internal.h"
#endif
#ifdef MTK_SMART_CHARGER_ENABLE
#include "app_smcharger_utils.h"
#endif
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
#include "apps_events_battery_event.h"
#include "battery_management.h"
#endif
#ifdef MTK_NVDM_ENABLE
#include "nvkey.h"
#include "nvkey_id_list.h"
#endif
#include "audio_src_srv.h"
#include "audio_transmitter_control.h"
#include "scenario_advanced_passthrough.h"



#define LOG_TAG                                             "[ADVANCE_PT]"

typedef struct {
    bool                enable;
    bool                is_sync;
    bt_clock_t          bt_clock;
} PACKED app_advance_pt_sync_pt_t;

typedef struct {
    bool                need_resume;
    bool                need_off_hw_pt;
    bool                ld_ongoing;
#ifdef MTK_3RD_PARTY_NR
    bool                ull_lea_ongoing;
#endif
} PACKED app_advance_pt_sync_state_t;

#define APP_ADVANCE_PASSTHROUGH_SYNC_TIME                   (500)

typedef struct {
    bool                enable;

    // only Agent saved - need to sync state when RHO
    bool                need_resume;
    bool                need_off_hw_pt;
    bool                ld_ongoing;
    bool                charger_in;
#ifdef MTK_3RD_PARTY_NR
    bool                ull_lea_ongoing;                    // 3RD_NR + ULL/LE Audio/eSCO ongoing
    bool                dvfs_lock;
#endif
} app_advance_pt_context_t;

static bool                           app_advance_pt_prepare_hw_pt = FALSE;
static app_advance_pt_context_t       app_advance_pt_ctx = {0};





/**================================================================================*/
/**                                       DVFS                                     */
/**================================================================================*/
static void app_advance_passthrough_dvfs(bool lock)
{
#if defined(MTK_3RD_PARTY_NR) && defined(HAL_DVFS_MODULE_ENABLED)
    APPS_LOG_MSGID_I(LOG_TAG" dvfs, %d->%d", 2, app_advance_pt_ctx.dvfs_lock, lock);
    hal_dvfs_status_t status = HAL_DVFS_STATUS_ERROR;
    if (!app_advance_pt_ctx.dvfs_lock && lock) {
        status = hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_LOCK);
    } else if (app_advance_pt_ctx.dvfs_lock && !lock) {
        status = hal_dvfs_lock_control(HAL_DVFS_HIGH_SPEED_208M, HAL_DVFS_UNLOCK);
    } else {
        //APPS_LOG_MSGID_E(LOG_TAG" dvfs, ignore", 0);
    }

    if (status == HAL_DVFS_STATUS_OK) {
        app_advance_pt_ctx.dvfs_lock = lock;
        APPS_LOG_MSGID_I(LOG_TAG" dvfs, pass lock=%d", 1, lock);
    }
#endif
}



/**================================================================================*/
/**                                    RHO Sync State                              */
/**================================================================================*/
#if defined(SUPPORT_ROLE_HANDOVER_SERVICE) && defined(BT_ROLE_HANDOVER_WITH_SPP_BLE)
bt_status_t app_advance_passthrough_rho_allow_callback(const bt_bd_addr_t *addr)
{
    APPS_LOG_MSGID_I(LOG_TAG" rho_allow_callback", 0);
    return BT_STATUS_SUCCESS;
}

uint8_t app_advance_passthrough_rho_get_length_callback(const bt_bd_addr_t *addr)
{
    uint8_t length = sizeof(app_advance_pt_sync_state_t);

#ifdef AIR_MULTI_POINT_ENABLE
    if (addr != NULL) {
        return 0;
    }
    APPS_LOG_MSGID_I(LOG_TAG" get_length_callback, length=%d", 1, length);
    return length;
#else
    APPS_LOG_MSGID_I(LOG_TAG" get_length_callback, length=%d", 1, length);
    return length;
#endif
}

bt_status_t app_advance_passthrough_rho_get_data_callback(const bt_bd_addr_t *addr, void *data)
{
    app_advance_pt_sync_state_t state = {0};
    state.need_resume = app_advance_pt_ctx.need_resume;
    state.need_off_hw_pt = app_advance_pt_ctx.need_off_hw_pt;
    state.ld_ongoing = app_advance_pt_ctx.ld_ongoing;
#ifdef MTK_3RD_PARTY_NR
    state.ull_lea_ongoing = app_advance_pt_ctx.ull_lea_ongoing;
#endif
    uint8_t length = sizeof(app_advance_pt_sync_state_t);

#ifdef AIR_MULTI_POINT_ENABLE
    if (addr != NULL) {
        return BT_STATUS_SUCCESS;
    }

    memcpy(data, &state, length);
    APPS_LOG_MSGID_I(LOG_TAG" get_data_callback, length=%d", 1, length);
    return BT_STATUS_SUCCESS;
#else
    memcpy(data, &state, length);
    APPS_LOG_MSGID_I(LOG_TAG" get_data_callback, length=%d", 1, length);
    return BT_STATUS_SUCCESS;
#endif
}

bt_status_t app_advance_passthrough_rho_update_callback(bt_role_handover_update_info_t *info)
{
    // Only callback once due to get length/data once
    bt_aws_mce_role_t role = info->role;
    app_advance_pt_sync_state_t *sync_state = (app_advance_pt_sync_state_t *)info->data;
    uint8_t length = info->length;

    // Cannot update charger_in flag when RHO
    // Update need_resume flag after check charger_in flag
    if (role == BT_AWS_MCE_ROLE_AGENT) {
        APPS_LOG_MSGID_I(LOG_TAG" rho_update_callback, new Partner - clear", 0);
        if (app_advance_pt_ctx.need_resume && app_advance_pt_ctx.charger_in) {
            APPS_LOG_MSGID_I(LOG_TAG" rho_update_callback, new Partner - keep need_resume due to charger_in", 0);
        } else {
            app_advance_pt_ctx.need_resume = FALSE;
        }
        app_advance_pt_ctx.need_off_hw_pt = FALSE;
        app_advance_pt_ctx.ld_ongoing = FALSE;
#ifdef MTK_3RD_PARTY_NR
        app_advance_pt_ctx.ull_lea_ongoing = FALSE;
#endif
    } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
        if (length != sizeof(app_advance_pt_sync_state_t)) {
            APPS_LOG_MSGID_E(LOG_TAG" rho_update_callback, error length=%d", 1, length);
            return BT_STATUS_SUCCESS;
        }

        if (app_advance_pt_ctx.need_resume && app_advance_pt_ctx.charger_in) {
            APPS_LOG_MSGID_I(LOG_TAG" rho_update_callback, new Agent - keep need_resume due to charger_in", 0);
        } else {
            app_advance_pt_ctx.need_resume = sync_state->need_resume;
        }
        app_advance_pt_ctx.need_off_hw_pt = sync_state->need_off_hw_pt;
        app_advance_pt_ctx.ld_ongoing = sync_state->ld_ongoing;
#ifdef MTK_3RD_PARTY_NR
        app_advance_pt_ctx.ull_lea_ongoing = sync_state->ull_lea_ongoing;
        APPS_LOG_MSGID_I(LOG_TAG" rho_update_callback, new Agent - need_resume=%d need_off_hw_pt=%d ld_ongoing=%d ull_lea_ongoing=%d",
                         4, app_advance_pt_ctx.need_resume, app_advance_pt_ctx.need_off_hw_pt,
                         app_advance_pt_ctx.ld_ongoing, app_advance_pt_ctx.ull_lea_ongoing);
#else
        APPS_LOG_MSGID_I(LOG_TAG" rho_update_callback, new Agent - need_resume=%d need_off_hw_pt=%d ld_ongoing=%d",
                         3, app_advance_pt_ctx.need_resume, app_advance_pt_ctx.need_off_hw_pt,
                         app_advance_pt_ctx.ld_ongoing);
#endif
    }
    return BT_STATUS_SUCCESS;
}

void app_advance_passthrough_rho_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    switch (event) {
        case BT_ROLE_HANDOVER_COMPLETE_IND: {
            APPS_LOG_MSGID_I(LOG_TAG" rho_status_callback, complete pre_role=%02X status=0x%08X",
                             2, role, status);
            break;
        }
    }
}
#endif



/**================================================================================*/
/**                                 Internal Function                              */
/**================================================================================*/
static bool app_advance_passthrough_check_condition()
{
    bool result = FALSE;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    uint8_t anc_state = 0;

#ifdef MTK_ANC_ENABLE
    anc_state = app_anc_server_get_state();
#endif
#ifdef MTK_3RD_PARTY_NR
    APPS_LOG_MSGID_I(LOG_TAG" check_condition, [%02X] anc_state=%d ld_ongoing=%d ull_lea=%d charger_in=%d",
                     5, role, anc_state, app_advance_pt_ctx.ld_ongoing,
                     app_advance_pt_ctx.ull_lea_ongoing, app_advance_pt_ctx.charger_in);
#else
    APPS_LOG_MSGID_I(LOG_TAG" check_condition, [%02X] anc_state=%d ld_ongoing=%d charger_in=%d",
                     4, role, anc_state, app_advance_pt_ctx.ld_ongoing, app_advance_pt_ctx.charger_in);
#endif

    if (!app_advance_pt_ctx.ld_ongoing
#ifdef MTK_ANC_ENABLE
        && anc_state != APP_ANC_STATE_PT_ENABLE
#endif
#ifdef MTK_3RD_PARTY_NR
        && !app_advance_pt_ctx.ull_lea_ongoing
#endif
        && !app_advance_pt_ctx.charger_in
       ) {
        result = TRUE;
    }
    return result;
}

static void app_advance_passthrough_sync_pt()
{
#ifdef MTK_AWS_MCE_ENABLE
    bool enable = app_advance_pt_ctx.enable;
    app_advance_pt_sync_pt_t sync_data = {
        .is_sync = FALSE,
        .enable = enable
    };
    bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                           APPS_EVENTS_INTERACTION_SYNC_ADVANCE_PASSTHROUGH,
                                                           &sync_data, sizeof(sync_data));
    APPS_LOG_MSGID_I(LOG_TAG" sync_pt, enable=%d bt_status=0x%08X", 2, enable, bt_status);
#endif
}

static void app_advance_passthrough_sync_state()
{
#ifdef MTK_AWS_MCE_ENABLE
    app_advance_pt_sync_state_t sync_data = {0};
    sync_data.need_resume = app_advance_pt_ctx.need_resume;
    bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                           APPS_EVENTS_INTERACTION_SYNC_ADVANCE_PASSTHROUGH,
                                                           &sync_data, sizeof(sync_data));
    APPS_LOG_MSGID_I(LOG_TAG" sync_state, need_resume=%d bt_status=0x%08X",
                     2, app_advance_pt_ctx.need_resume, bt_status);
#endif
}

static bool app_advance_passthrough_enable_imp(bool enable, advanced_passthrough_sync_para_t *audio_param)
{
    audio_transmitter_status_t status = AUDIO_TRANSMITTER_STATUS_FAIL;
    if (enable) {
        status = advanced_passthrough_open(audio_param);
    } else {
        status = advanced_passthrough_close(audio_param);
    }

    if (status == AUDIO_TRANSMITTER_STATUS_SUCCESS) {
        app_advance_pt_ctx.enable = enable;
        APPS_LOG_MSGID_I(LOG_TAG" enable_imp, on-off=%d successfully", 1, enable);

#ifdef MTK_3RD_PARTY_NR
        apps_config_state_t mmi_state = apps_config_key_get_mmi_state();
        audio_src_srv_pseudo_device_t audio_type = -1;
        const audio_src_srv_handle_t *audio_src = audio_src_srv_get_runing_pseudo_device();
        if (audio_src != NULL) {
            audio_type = audio_src->type;
        }
        if (audio_type == AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP
            || audio_type == AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_HFP
            || mmi_state == APP_STATE_VA) {
            app_advance_passthrough_dvfs(enable);
        }
#endif
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" enable_imp, enable=%d error status=%d", 2, enable, status);
    }
    return (status == AUDIO_TRANSMITTER_STATUS_SUCCESS);
}

static bool app_advance_passthrough_enable(bool enable, bool is_sync)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    advanced_passthrough_sync_para_t audio_param = {
        .is_sync = FALSE,
        .gpt_sync_time = 0,
    };
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_srv_link_type_t aws_link_type = bt_aws_mce_srv_get_link_type();
    APPS_LOG_MSGID_I(LOG_TAG" enable, [%02X] enable=%d is_sync=%d aws_link_type=%d",
                     4, role, enable, is_sync, aws_link_type);
#else
    APPS_LOG_MSGID_I(LOG_TAG" enable, [%02X] enable=%d is_sync=%d",
                     3, role, enable, is_sync);
#endif

#ifdef MTK_AWS_MCE_ENABLE
    if (is_sync
        && role == BT_AWS_MCE_ROLE_AGENT
        && aws_link_type != BT_AWS_MCE_SRV_LINK_NONE) {
        app_advance_pt_sync_pt_t sync_data = {0};
        bt_sink_srv_bt_clock_addition(&sync_data.bt_clock, NULL, APP_ADVANCE_PASSTHROUGH_SYNC_TIME);
        sync_data.enable = enable;
        sync_data.is_sync = TRUE;
        bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                               APPS_EVENTS_INTERACTION_SYNC_ADVANCE_PASSTHROUGH,
                                                               &sync_data, sizeof(sync_data));
        if (bt_status == BT_STATUS_SUCCESS) {
            audio_param.is_sync = TRUE;
            bt_sink_srv_convert_bt_clock_2_gpt_count(&sync_data.bt_clock, &audio_param.gpt_sync_time);
        } else {
            audio_param.is_sync = FALSE;
            APPS_LOG_MSGID_E(LOG_TAG" enable, send_aws error bt_status=0x%08X", bt_status);
        }
    }
#endif

    return app_advance_passthrough_enable_imp(enable, &audio_param);
}

static bool app_advance_passthrough_suspend(bool is_sync)
{
    if (!app_advance_pt_ctx.enable) {
        APPS_LOG_MSGID_I(LOG_TAG" suspend, error state", 0);
        return FALSE;
    }

    bool success = app_advance_passthrough_enable(FALSE, is_sync);
    APPS_LOG_MSGID_I(LOG_TAG" suspend, is_sync=%d success=%d", 2, is_sync, success);
    if (success) {
        app_advance_pt_ctx.need_resume = TRUE;
    }

#ifdef MTK_ANC_ENABLE
    if (success
        && app_anc_server_get_state() == APP_ANC_STATE_DISABLE
        && !app_advance_pt_ctx.ld_ongoing
        && !app_advance_pt_ctx.charger_in) {
        bool ret = app_anc_service_enable(AUDIO_ANC_CONTROL_PASS_THRU_FILTER_DEFAULT,
                                          AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF,
                                          AUDIO_ANC_CONTROL_UNASSIGNED_GAIN, NULL);
        APPS_LOG_MSGID_I(LOG_TAG" suspend, turn_on HW PT ret=%d", 1, ret);
        if (ret) {
            app_advance_pt_prepare_hw_pt = TRUE;
        }
    }
#endif
    return success;
}

static bool app_advance_passthrough_resume(bool is_sync)
{
    if (app_advance_pt_ctx.enable) {
        APPS_LOG_MSGID_E(LOG_TAG" resume, error state", 0);
        return FALSE;
    } else if (!app_advance_passthrough_check_condition()) {
        APPS_LOG_MSGID_E(LOG_TAG" resume, check_condition error", 0);
        return FALSE;
    }

    bool success = app_advance_passthrough_enable(TRUE, is_sync);
    APPS_LOG_MSGID_I(LOG_TAG" resume, is_sync=%d need_off_hw_pt=%d success=%d",
                     3, is_sync, app_advance_pt_ctx.need_off_hw_pt, success);
    if (success) {
        app_advance_pt_ctx.need_resume = FALSE;
#ifdef MTK_RACE_CMD_ENABLE
        uint8_t enable_status = TRUE;
        app_race_send_notify(APPS_RACE_CMD_CONFIG_TYPE_ADVANCED_PASSTHROUGH,
                             (void *)&enable_status, sizeof(uint8_t));
#endif
    }

#ifdef MTK_ANC_ENABLE
    if (app_advance_pt_ctx.need_off_hw_pt) {
        bool ret = app_anc_service_disable();
        APPS_LOG_MSGID_I(LOG_TAG" resume, turn_off HW PT ret=%d", 1, ret);
        app_advance_pt_ctx.need_off_hw_pt = FALSE;
    }
#endif
    return success;
}

static void app_advance_passthrough_swich_from_aws(app_advance_pt_sync_pt_t *sync_data)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    APPS_LOG_MSGID_I(LOG_TAG" swich_from_aws, [%02X] enable=%d->%d is_sync=%d",
                     4, role, app_advance_pt_ctx.enable, sync_data->enable, sync_data->is_sync);
    if (role == BT_AWS_MCE_ROLE_PARTNER
        && sync_data->enable != app_advance_pt_ctx.enable) {
        if (sync_data->enable && app_advance_pt_ctx.charger_in) {
            APPS_LOG_MSGID_I(LOG_TAG" swich_from_aws, [%02X] need_resume due to charger_in", 1, role);
            app_advance_pt_ctx.need_resume = TRUE;
            return;
        }

        bool enable = sync_data->enable;
        advanced_passthrough_sync_para_t audio_param = {
            .is_sync = sync_data->is_sync,
        };
        if (sync_data->is_sync) {
            bt_sink_srv_convert_bt_clock_2_gpt_count(&sync_data->bt_clock, &audio_param.gpt_sync_time);
        }
        app_advance_passthrough_enable_imp(enable, &audio_param);
    } else {
        APPS_LOG_MSGID_I(LOG_TAG" swich_from_aws, ignore", 0);
    }
}

static void app_advance_passthrough_proc_bt_sink_event(uint32_t event_id, void *extra_data, size_t data_len)
{
    if (event_id == BT_SINK_SRV_EVENT_STATE_CHANGE) {
#ifdef MTK_3RD_PARTY_NR
        bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
        audio_src_srv_pseudo_device_t audio_type = -1;
        const audio_src_srv_handle_t *audio_src = audio_src_srv_get_runing_pseudo_device();
        if (audio_src != NULL) {
            audio_type = audio_src->type;
        }
        bt_sink_srv_state_change_t *param = (bt_sink_srv_state_change_t *)extra_data;
        bt_sink_srv_state_t pre_state = param->previous;
        bt_sink_srv_state_t cur_state = param->current;
        APPS_LOG_MSGID_I(LOG_TAG" BT_SINK Event, [%02X] sink state=0x%04X->0x%04X audio_src=%d",
                         4, role, pre_state, cur_state, audio_type);

        // Need to NVFS up frequency when call ongoing (agent/partner)
        if (app_advance_pt_ctx.enable) {
            if (cur_state >= BT_SINK_SRV_STATE_INCOMING &&
                (audio_type == AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP
                || audio_type == AUDIO_SRC_SRV_PSEUDO_DEVICE_AWS_HFP)) {
                // try to lock DVFS
                app_advance_passthrough_dvfs(TRUE);
            } else {
                app_advance_passthrough_dvfs(FALSE);
            }
        }

        if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
            APPS_LOG_MSGID_I(LOG_TAG" BT_SINK Event, [%02X] ignore", 1, role);
            return;
        }

        // Need to suspend when ULL/LE Audio active
        bool old_ull_lea = app_advance_pt_ctx.ull_lea_ongoing;
        if (cur_state < BT_SINK_SRV_STATE_STREAMING) {
            app_advance_pt_ctx.ull_lea_ongoing = FALSE;
        } else if (audio_type == AUDIO_SRC_SRV_PSEUDO_DEVICE_BLE
                   || audio_type == AUDIO_SRC_SRV_PSEUDO_DEVICE_ULL_A2DP
                   || audio_type == AUDIO_SRC_SRV_PSEUDO_DEVICE_HFP) {
            app_advance_pt_ctx.ull_lea_ongoing = TRUE;
        } else {
            app_advance_pt_ctx.ull_lea_ongoing = FALSE;
        }
        APPS_LOG_MSGID_I(LOG_TAG" BT_SINK Event, [%02X] ull_lea=%d->%d enable=%d need_resume=%d",
                         5, role, old_ull_lea, app_advance_pt_ctx.ull_lea_ongoing,
                         app_advance_pt_ctx.enable, app_advance_pt_ctx.need_resume);

        if (app_advance_pt_ctx.enable) {
            if (!old_ull_lea && app_advance_pt_ctx.ull_lea_ongoing) {
                APPS_LOG_MSGID_I(LOG_TAG" BT_SINK Event, need to suspend", 0);
                app_advance_passthrough_suspend(TRUE);
            }
        } else if (app_advance_pt_ctx.need_resume) {
            if (old_ull_lea && !app_advance_pt_ctx.ull_lea_ongoing) {
                APPS_LOG_MSGID_I(LOG_TAG" BT_SINK Event, try to resume", 0);
                app_advance_passthrough_resume(TRUE);
            }
        }
#endif
    }
}

#ifdef MTK_ANC_ENABLE
static void app_advance_passthrough_proc_anc_event(uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
        APPS_LOG_MSGID_I(LOG_TAG" ANC Event, [%02X] ignore", 1, role);
        return;
    }

    switch (event_id) {
        case AUDIO_ANC_CONTROL_EVENT_ON: {
            app_anc_srv_result_t *anc_result = (app_anc_srv_result_t *)extra_data;
            if (anc_result->cur_type == AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
                APPS_LOG_MSGID_I(LOG_TAG" ANC Event, [%02X] HW PT Enable prepare_hw_pt=%d need_resume=%d",
                                 3, role, app_advance_pt_prepare_hw_pt, app_advance_pt_ctx.need_resume);
                if (app_advance_pt_prepare_hw_pt) {
                    app_advance_pt_prepare_hw_pt = FALSE;
                    app_advance_pt_ctx.need_off_hw_pt = TRUE;
                } else {
                    app_advance_pt_ctx.need_off_hw_pt = FALSE;
                    if (app_advance_pt_ctx.enable) {
                        APPS_LOG_MSGID_I(LOG_TAG" ANC Event, [%02X] HW PT Enable - need to suspend",
                                         1, role);
                        app_advance_passthrough_suspend(TRUE);
                    }
                }
            } else if (anc_result->cur_type < AUDIO_ANC_CONTROL_TYPE_PASSTHRU_FF) {
                APPS_LOG_MSGID_I(LOG_TAG" ANC Event, [%02X] HW ANC Enable", 1, role);
                app_advance_pt_prepare_hw_pt = FALSE;
                app_advance_pt_ctx.need_off_hw_pt = FALSE;
                if (app_advance_pt_ctx.need_resume) {
                    APPS_LOG_MSGID_I(LOG_TAG" ANC Event, [%02X] HW ANC Enable - try to resume", 1, role);
                    app_advance_passthrough_resume(TRUE);
                }
            }
            break;
        }

        case AUDIO_ANC_CONTROL_EVENT_OFF: {
            APPS_LOG_MSGID_I(LOG_TAG" ANC Event, [%02X] HW ANC Disable need_resume=%d",
                             2, role, app_advance_pt_ctx.need_resume);
            if (app_advance_pt_ctx.need_resume) {
                APPS_LOG_MSGID_I(LOG_TAG" ANC Event, [%02X] HW ANC/PT Disable - try to resume", 1, role);
                app_advance_passthrough_resume(TRUE);
            }
            app_advance_pt_prepare_hw_pt = FALSE;
            app_advance_pt_ctx.need_off_hw_pt = FALSE;
            break;
        }
    }
}
#endif

#ifdef MTK_SMART_CHARGER_ENABLE
static void app_advance_passthrough_proc_smcharger_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (event_id == EVENT_ID_SMCHARGER_NOTIFY_PUBLIC_EVENT) {
        app_smcharger_public_event_para_t *event_para = (app_smcharger_public_event_para_t *)extra_data;
        if (event_para->action == SMCHARGER_CHARGER_IN_ACTION) {
            APPS_LOG_MSGID_I(LOG_TAG" SMCharger event, Charger_in action", 0);
            app_advance_pt_ctx.charger_in = TRUE;
            if (app_advance_pt_ctx.enable) {
                APPS_LOG_MSGID_I(LOG_TAG" SMCharger event, [%02X] Charger_in need to suspend", 1, role);
                app_advance_passthrough_suspend(FALSE);
            }
        } else if (event_para->action == SMCHARGER_CHARGER_OUT_ACTION) {
            APPS_LOG_MSGID_I(LOG_TAG" SMCharger event, Charger_out action", 0);
            app_advance_pt_ctx.charger_in = FALSE;
            if (app_advance_pt_ctx.need_resume) {
                APPS_LOG_MSGID_I(LOG_TAG" SMCharger event, [%02X] Charger_out try to resume", 1, role);
                app_advance_passthrough_resume(FALSE);
            }
        }
    }
}
#endif

#if defined(MTK_BATTERY_MANAGEMENT_ENABLE) && !defined(MTK_SMART_CHARGER_ENABLE)
static void app_advance_passthrough_proc_battery_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (event_id == APPS_EVENTS_BATTERY_CHARGER_EXIST_CHANGE) {
        bool charger_in = (bool)extra_data;
        APPS_LOG_MSGID_I(LOG_TAG" Battery event, charger_exist change %d->%d",
                         2, app_advance_pt_ctx.charger_in, charger_in);
        if (!app_advance_pt_ctx.charger_in && charger_in) {
            app_advance_pt_ctx.charger_in = TRUE;
            if (app_advance_pt_ctx.enable) {
                APPS_LOG_MSGID_I(LOG_TAG" Battery event, [%02X] Charger_in need to suspend", 1, role);
                app_advance_passthrough_suspend(FALSE);
            }
        } else if (app_advance_pt_ctx.charger_in && !charger_in) {
            app_advance_pt_ctx.charger_in = FALSE;
            if (app_advance_pt_ctx.need_resume) {
                APPS_LOG_MSGID_I(LOG_TAG" Battery event, [%02X] Charger_out try to resume", 1, role);
                app_advance_passthrough_resume(FALSE);
            }
        }
    }
}
#endif



/**================================================================================*/
/**                               Advance Passthrough API                          */
/**================================================================================*/
void app_advance_passthrough_set_ld_ongoing(bool ld_ongoing)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    APPS_LOG_MSGID_I(LOG_TAG" set_ld_ongoing, [%02X] ld_ongoing=%d->%d enable=%d need_to_resume=%d",
                     5, role, app_advance_pt_ctx.ld_ongoing, ld_ongoing,
                     app_advance_pt_ctx.enable, app_advance_pt_ctx.need_resume);

    if ((role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE)
        && app_advance_pt_ctx.ld_ongoing != ld_ongoing) {
        app_advance_pt_ctx.ld_ongoing = ld_ongoing;

        if (app_advance_pt_ctx.enable && ld_ongoing) {
            APPS_LOG_MSGID_I(LOG_TAG" set_ld_ongoing, need to suspend", 0);
            app_advance_passthrough_suspend(TRUE);
        } else if (app_advance_pt_ctx.need_resume && !ld_ongoing) {
            APPS_LOG_MSGID_I(LOG_TAG" set_ld_ongoing, try to resume", 0);
            app_advance_passthrough_resume(TRUE);
        }
    }
}

void app_advance_passthrough_set_va_ongoing(bool va_ongoing)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    APPS_LOG_MSGID_I(LOG_TAG" set_va_ongoing, [%02X] enable=%d va_ongoing=%d",
                     3, role, app_advance_pt_ctx.enable, va_ongoing);
    if (app_advance_pt_ctx.enable) {
        if (va_ongoing) {
            app_advance_passthrough_dvfs(TRUE);
        } else {
            app_advance_passthrough_dvfs(FALSE);
        }
    }
#ifdef MTK_3RD_PARTY_NR
    if (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE) {
        app_advance_pt_ctx.ull_lea_ongoing = va_ongoing;
        if (app_advance_pt_ctx.enable && va_ongoing) {
            APPS_LOG_MSGID_I(LOG_TAG" set_va_ongoing, need to suspend", 0);
            app_advance_passthrough_suspend(TRUE);
        } else if (app_advance_pt_ctx.need_resume && !va_ongoing) {
            APPS_LOG_MSGID_I(LOG_TAG" set_va_ongoing, try to resume", 0);
            app_advance_passthrough_resume(TRUE);
        }
    }
#endif
}

bool app_advance_passthrough_switch(void)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role != BT_AWS_MCE_ROLE_AGENT && role != BT_AWS_MCE_ROLE_NONE) {
        APPS_LOG_MSGID_E(LOG_TAG" switch, error role=%02X", 1, role);
        return FALSE;
    }

    bool success = FALSE;
    bool old_enable = app_advance_pt_ctx.enable;
    bool enable = !old_enable;
    APPS_LOG_MSGID_I(LOG_TAG" switch, enable=%d->%d", 2, old_enable, enable);
    if (enable) {
        bool check_result = app_advance_passthrough_check_condition();
        if (!check_result) {
            APPS_LOG_MSGID_E(LOG_TAG" switch, check_condition error", 0);
            return success;
        }
    } else {
        APPS_LOG_MSGID_I(LOG_TAG" switch, reset need_resume", 0);
        app_advance_pt_ctx.need_resume = FALSE;
        // need to notify partner need_resume=FALSE
        app_advance_passthrough_sync_state();
    }

    success = app_advance_passthrough_enable(enable, TRUE);
//    if (success) {
//        apps_config_set_vp(VP_INDEX_SUCCESSED, TRUE, 200, VOICE_PROMPT_PRIO_MEDIUM, FALSE, NULL);
//        APPS_LOG_MSGID_I(LOG_TAG" switch, play VP", 0);
//    }
    return success;
}

bool app_advance_passthrough_is_enable(void)
{
    return app_advance_pt_ctx.enable;
}

void app_advance_passthrough_sync_to_partner(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    app_advance_passthrough_sync_pt();
#endif
}

void app_advance_passthrough_handle_aws_data(void *data, uint32_t len)
{
    if (data != NULL && len == sizeof(app_advance_pt_sync_pt_t)) {
        app_advance_pt_sync_pt_t *sync_data = (app_advance_pt_sync_pt_t *)data;
        app_advance_passthrough_swich_from_aws(sync_data);
    } else if (data != NULL && len == sizeof(app_advance_pt_sync_state_t)) {
        app_advance_pt_sync_state_t *sync_state = (app_advance_pt_sync_state_t *)data;
        app_advance_pt_ctx.need_resume = sync_state->need_resume;
        APPS_LOG_MSGID_I(LOG_TAG" handle_aws_data, sync_state need_resume=%d",
                         1, sync_state->need_resume);
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" handle_aws_data, error data=%08X len=%d", 2, data, len);
    }
}

void app_advance_passthrough_proc(uint32_t event_group, uint32_t event_id,
                                  void *extra_data, size_t data_len)
{
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_BT_SINK:
            app_advance_passthrough_proc_bt_sink_event(event_id, extra_data, data_len);
            break;
#ifdef MTK_ANC_ENABLE
        case EVENT_GROUP_UI_SHELL_AUDIO_ANC:
            app_advance_passthrough_proc_anc_event(event_id, extra_data, data_len);
            break;
#endif
#ifdef MTK_SMART_CHARGER_ENABLE
        case EVENT_GROUP_UI_SHELL_CHARGER_CASE:
            app_advance_passthrough_proc_smcharger_group(event_id, extra_data, data_len);
            break;
#endif
#if defined(MTK_BATTERY_MANAGEMENT_ENABLE) && !defined(MTK_SMART_CHARGER_ENABLE)
        case EVENT_GROUP_UI_SHELL_BATTERY: {
            app_advance_passthrough_proc_battery_group(event_id, extra_data, data_len);
            break;
        }
#endif
    }
}

void app_advance_passthrough_save_to_flash(void)
{
#ifdef MTK_NVDM_ENABLE
    uint32_t size = sizeof(uint8_t);
    nvkey_status_t status = nvkey_write_data(NVKEYID_APP_ADVANCE_PT,
                                             (const uint8_t *)&app_advance_pt_ctx.enable,
                                             size);
    APPS_LOG_MSGID_I(LOG_TAG" save_to_flash, enable=%d status=%d",
                     2, app_advance_pt_ctx.enable, status);
#endif
}

void app_advance_passthrough_init(void)
{
#if defined(SUPPORT_ROLE_HANDOVER_SERVICE) && defined(BT_ROLE_HANDOVER_WITH_SPP_BLE)
    bt_role_handover_callbacks_t rho_callback = {
        app_advance_passthrough_rho_allow_callback,
        app_advance_passthrough_rho_get_length_callback,
        app_advance_passthrough_rho_get_data_callback,
        app_advance_passthrough_rho_update_callback,
        app_advance_passthrough_rho_status_callback
    };
    bt_status_t bt_status = bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_ADVANCE_PT_APP,
                                                                &rho_callback);
    APPS_LOG_MSGID_I(LOG_TAG" init, bt_status=0x%08X", 1, bt_status);
#endif
#ifdef MTK_BATTERY_MANAGEMENT_ENABLE
    int32_t charger_exitst = battery_management_get_battery_property(BATTERY_PROPERTY_CHARGER_EXIST);
    APPS_LOG_MSGID_I(LOG_TAG" init, charger_exitst=%d", 1, charger_exitst);
    if (charger_exitst > 0) {
        app_advance_pt_ctx.charger_in = TRUE;
    }
#endif
#ifdef MTK_NVDM_ENABLE
    bool enable = FALSE;
    uint32_t size = sizeof(uint8_t);
    nvkey_status_t status = nvkey_read_data(NVKEYID_APP_ADVANCE_PT,
                                            (uint8_t *)&enable,
                                            &size);
    APPS_LOG_MSGID_I(LOG_TAG" init, read nvkey status=%d", 1, status);
    if (status == NVKEY_STATUS_ITEM_NOT_FOUND) {
        enable = FALSE;
        size = sizeof(uint8_t);
        status = nvkey_write_data(NVKEYID_APP_ADVANCE_PT, (const uint8_t *)&enable, size);
        APPS_LOG_MSGID_I(LOG_TAG" init, write nvkey status=%d", 1, status);
    }
    APPS_LOG_MSGID_I(LOG_TAG" init, read enable=%d", 1, enable);
    if (enable) {
        if (app_advance_pt_ctx.charger_in) {
            app_advance_pt_ctx.need_resume = TRUE;
            APPS_LOG_MSGID_I(LOG_TAG" init, need_resume=TRUE", 0);
        } else {
            app_advance_passthrough_resume(FALSE);
        }
    }
#endif
}

#endif   /* AIR_ADVANCED_PASSTHROUGH_ENABLE */
