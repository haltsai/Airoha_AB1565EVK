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
 * File: app_power_save_utils.c
 *
 * Description: This file provides some API for Power Saving APP.
 *
 */

#include "app_power_save_utils.h"
#include "apps_customer_config.h"
#include "apps_debug.h"
#include "apps_config_features_dynamic_setting.h"
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
#include "app_master_utils.h"
#endif
#include "bt_power_on_config.h"
#include "bt_device_manager.h"
#include "bt_device_manager_test_mode.h"
#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "FreeRTOS.h"

#ifdef AIR_LE_AUDIO_ENABLE
#include "bt_sink_srv_le.h"
#include "app_le_audio.h"
#ifdef AIR_LE_AUDIO_MULTIPOINT_ENABLE
#include "bt_sink_srv_le_cap.h"
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
#include "app_le_audio_bis.h"
#endif
#endif
#ifdef AIR_SILENCE_DETECTION_ENABLE
#include "apps_events_audio_event.h"
#endif

#include "nvkey_id_list.h"
#include "nvkey.h"

#define LOG_TAG     "[POWER_SAVING][UTILS] "

/**
 *  @brief This structure defines the link-list of get_mode_func.
 */
typedef struct _app_power_save_utils_modules {
    get_power_saving_target_mode_func_t get_mode_func;
    struct _app_power_save_utils_modules *next;
} app_power_save_utils_modules_t;


app_power_save_utils_modules_t *s_top_module = NULL; /* The top node of get_power_saving_target_mode link-list. */

static bool s_have_got_mode_flag = false; /* TRUE means Power Saving APP has called "app_power_save_utils_get_target_mode", need to NOTIFY_CHANGE when register get_mode function. */

static app_power_saving_cfg s_cfg = {0, 0, 0, 0}; /* The power saving configuration. */

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(AIR_SILENCE_DETECTION_ENABLE)
static bool s_slave_audio_silence = true;
#endif

bool app_power_save_utils_add_new_callback_func(get_power_saving_target_mode_func_t callback_func)
{
    bool ret = true;
    app_power_save_utils_modules_t *power_saving_module;
    app_power_save_utils_modules_t **p_new_module = NULL;

    if (NULL == s_top_module) {
        p_new_module = &s_top_module;
    }
    for (power_saving_module = s_top_module;
         power_saving_module != NULL;
         power_saving_module = power_saving_module->next) {
        /* Reject duplicated power_saving_module. */
        if (callback_func == power_saving_module->get_mode_func) {
            APPS_LOG_MSGID_I(LOG_TAG" Already register get_mode_callback=%x",
                             1, callback_func);
            break;
        } else if (NULL == power_saving_module->next) {
            /* Find last empty power_saving_module. */
            p_new_module = &(power_saving_module->next);
        }
    }
    /* Malloc new power_saving_module and set callback_func. */
    if (p_new_module) {
        *p_new_module = (app_power_save_utils_modules_t *)
                        pvPortMalloc(sizeof(app_power_save_utils_modules_t));
        if (*p_new_module) {
            memset(*p_new_module, 0, sizeof(app_power_save_utils_modules_t));
            (*p_new_module)->get_mode_func = callback_func;
        } else {
            ret = false;
            APPS_LOG_MSGID_I(LOG_TAG" Fail to register get_mode_callback, =%x",
                             1, callback_func);
        }
    }
    return ret;
}

app_power_saving_target_mode_t app_power_save_utils_get_target_mode(ui_shell_activity_t *self, app_power_saving_type_t *type)
{
    /* When all of the below status is true, need do power off:
    1. Device is out of case;
    2. Power on and not connected;
    3. ANC/PassThrough is off;
    */
    app_power_saving_target_mode_t target_mode = APP_POWER_SAVING_TARGET_MODE_NORMAL;
    app_power_saving_target_mode_t temp_mode = APP_POWER_SAVING_TARGET_MODE_NORMAL;
    app_power_saving_context_t *local_context = NULL;
    app_power_save_utils_modules_t *power_saving_module;

    s_have_got_mode_flag = true;
    *type = APP_POWER_SAVING_TYPE_NO_CONNECTION;

    if (BT_DEVICE_MANAGER_TEST_MODE_NONE != bt_device_manager_get_test_mode()
        && !(BT_DEVICE_MANAGER_TEST_MODE_DUT_MIX == bt_device_manager_get_test_mode()
            && BT_DEVICE_MANAGER_TEST_MODE_DUT_STATE_DUT_MIX_ENABLED == bt_device_manager_test_mode_get_dut_state())) {
        return APP_POWER_SAVING_TARGET_MODE_NORMAL;
    }

    if (!app_power_save_utils_is_enabled()) {
        return APP_POWER_SAVING_TARGET_MODE_NORMAL;
    }

#if (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_SYSTEM_OFF) || (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_DISABLE_BT)
    if (self && self->local_context) {
        local_context = self->local_context;
        /* Default Power saving mode is POWER_OFF_SYSTEM, see inc/boards/<board_type>/apps_customer_config.h. */

        if (APP_POWER_SAVING_BT_DISCONNECTED == local_context->bt_sink_srv_state
#ifdef AIR_LE_AUDIO_ENABLE
            && !app_le_audio_is_connected()
#endif
#ifdef AIR_LE_AUDIO_BIS_ENABLE
            && !app_le_audio_bis_is_streaming()
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
            && app_master_utils_get_slave_mmi_state() < APP_CONNECTED
#endif
        ) {
            target_mode = APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
        }

#ifdef AIR_SILENCE_DETECTION_ENABLE
        /* When audio is silence, do power off. */
        if (target_mode == APP_POWER_SAVING_TARGET_MODE_NORMAL && app_events_audio_event_get_silence_detect_flag()
#if defined(MTK_AWS_MCE_ENABLE)
            && 0 == (bt_device_manager_aws_local_info_get_role() & (BT_AWS_MCE_ROLE_CLINET | BT_AWS_MCE_ROLE_PARTNER))
#endif
#ifdef AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE
            && s_slave_audio_silence
#endif
            && app_power_saving_get_cfg()->silence_detect_enable) {
            *type = APP_POWER_SAVING_TYPE_AUDIO_SILENCE;
            target_mode = APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF;
        }
#endif

        /* Do system off or BT off by config. */
        if (target_mode == APP_POWER_SAVING_TARGET_MODE_SYSTEM_OFF
#if (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_SYSTEM_OFF)
           && apps_config_features_is_no_connection_sleep_mode_bt_off()
#endif
        ) {
            target_mode = APP_POWER_SAVING_TARGET_MODE_BT_OFF;
        }

        /* Update target_mode by running whole power_saving_module link-list. */
        for (power_saving_module = s_top_module;
             power_saving_module != NULL && target_mode > APP_POWER_SAVING_TARGET_MODE_NORMAL;
             power_saving_module = power_saving_module->next) {
            if (power_saving_module->get_mode_func) {
                temp_mode = power_saving_module->get_mode_func();
            } else {
                APPS_LOG_MSGID_E(LOG_TAG" get_tagert_mode, power_saving_module->get_mode_func = NULL", 0);
            }
            /* Select "lower" target_mode. */
            if (target_mode > temp_mode) {
                target_mode = temp_mode;
            }
        }

        if (target_mode == APP_POWER_SAVING_TARGET_MODE_BT_OFF && APP_POWER_SAVING_BT_OFF == local_context->bt_sink_srv_state) {
            /* Since target mode is BT OFF and current mode is BT OFF, do nothing. */
            APPS_LOG_MSGID_I(LOG_TAG" BT is already OFF, target is also BT OFF", 0);
            target_mode = APP_POWER_SAVING_TARGET_MODE_NORMAL;
        }
    }

    if (target_mode == APP_POWER_SAVING_TARGET_MODE_BT_OFF && APP_POWER_SAVING_BT_OFF == local_context->bt_sink_srv_state) {
        /* Already BT OFF, don't need change. */
        target_mode = APP_POWER_SAVING_TARGET_MODE_NORMAL;
    }
#endif /* #if (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_SYSTEM_OFF) || (APPS_POWER_SAVING_MODE == APPS_POWER_SAVING_DISABLE_BT) */
    return target_mode;
}

bool app_power_save_utils_notify_mode_changed(bool from_isr, get_power_saving_target_mode_func_t callback_func)
{
    bool ret = true;
    /* Send NOTIFY_CHANGE event to update and process. */
    ui_shell_status_t send_ret = ui_shell_send_event(from_isr,
                                                     EVENT_PRIORITY_HIGNEST,
                                                     EVENT_GROUP_UI_SHELL_POWER_SAVING,
                                                     APP_POWER_SAVING_EVENT_NOTIFY_CHANGE,
                                                     NULL, 0, NULL, 0);
    if (UI_SHELL_STATUS_OK != send_ret) {
        APPS_LOG_MSGID_E(LOG_TAG" _notify_mode_changed, Fail(%d) to send POWER_SAVING_ALLOWANCE_CHANGE to =%x",
                         2, send_ret, callback_func);
        ret = false;
    }
    return ret;
}

bool app_power_save_utils_register_get_mode_callback(get_power_saving_target_mode_func_t callback_func)
{
    bool ret = true;

    ui_shell_status_t send_ret = UI_SHELL_STATUS_INVALID_STATE;
    /* Need to NOTIFY_CHANGE with callback_func when register get_mode function. */
    if (s_have_got_mode_flag) {
        send_ret = ui_shell_send_event(false,
                                       EVENT_PRIORITY_HIGNEST,
                                       EVENT_GROUP_UI_SHELL_POWER_SAVING,
                                       APP_POWER_SAVING_EVENT_NOTIFY_CHANGE,
                                       callback_func, 0, NULL, 0);
    }

    if (UI_SHELL_STATUS_INVALID_STATE == send_ret) {
        APPS_LOG_MSGID_I(LOG_TAG" Register get_mode_callback in main=%x", 1, callback_func);
        ret = app_power_save_utils_add_new_callback_func(callback_func);
    } else if (UI_SHELL_STATUS_OK != send_ret) {
        APPS_LOG_MSGID_E(LOG_TAG" register_get_mode_callback, Fail(%d) to send POWER_SAVING_ALLOWANCE_CHANGE to =%x",
                         2, send_ret, callback_func);
        ret = false;
    } else {
        APPS_LOG_MSGID_I(LOG_TAG" register_get_mode_callback, Send POWER_SAVING_ALLOWANCE_CHANGE to =%x", 1, callback_func);
    }
    return ret;
}

bool app_power_save_utils_update_bt_state(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    app_power_saving_context_t *local_context = (app_power_saving_context_t *)self->local_context;
    bool bt_changed = FALSE;
    if (local_context == NULL) {
        return FALSE;
    }
#if defined(MTK_AWS_MCE_ENABLE)
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    app_power_saving_bt_state_t old_bt_state = local_context->bt_sink_srv_state;

    switch (event_id) {
        case BT_CM_EVENT_POWER_STATE_UPDATE: {
            bt_cm_power_state_update_ind_t *power_update = (bt_cm_power_state_update_ind_t *)extra_data;
            if (!power_update || !local_context) {
                break;
            }

            /* Update bt_state when BT power_state changed. */
            if (APP_POWER_SAVING_BT_OFF != local_context->bt_sink_srv_state
                && BT_CM_POWER_STATE_OFF == power_update->power_state) {
                local_context->bt_sink_srv_state = APP_POWER_SAVING_BT_OFF;
                APPS_LOG_MSGID_I(LOG_TAG"bt_state = %d", 1, local_context->bt_sink_srv_state);
            } else if (APP_POWER_SAVING_BT_OFF == local_context->bt_sink_srv_state
                       && BT_CM_POWER_STATE_ON == power_update->power_state) {
                local_context->bt_sink_srv_state = APP_POWER_SAVING_BT_DISCONNECTED;
                APPS_LOG_MSGID_I(LOG_TAG"bt_state = %d", 1, local_context->bt_sink_srv_state);
            }
        }
        break;
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (!remote_update || !local_context) {
                break;
            }

#if defined(MTK_AWS_MCE_ENABLE)
            if (BT_AWS_MCE_ROLE_AGENT == role || BT_AWS_MCE_ROLE_NONE == role)
#endif
            {
                /* For Agent, bt_state will be BT_CONNECTED (no need power saving) when non-AWS profile connected. */
                if (BT_CM_ACL_LINK_DISCONNECTED != remote_update->pre_acl_state
                    && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state) {
                    if (APP_POWER_SAVING_BT_DISCONNECTED != local_context->bt_sink_srv_state
                        && 0 == bt_cm_get_connected_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0)) {
                        local_context->bt_sink_srv_state = APP_POWER_SAVING_BT_DISCONNECTED;
                    }
                } else if (!((~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)) & remote_update->pre_connected_service)
                           && ((~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)) & remote_update->connected_service)) {
                    if (APP_POWER_SAVING_BT_CONNECTED != local_context->bt_sink_srv_state) {
                        local_context->bt_sink_srv_state = APP_POWER_SAVING_BT_CONNECTED;
                    }
                }
            }
#if defined(MTK_AWS_MCE_ENABLE)
            else {
                /* For Partner, bt_state will be BT_CONNECTED (no need power saving) when AWS attached. */
                APPS_LOG_MSGID_I(LOG_TAG"connected_service %x -> %x", 2, remote_update->pre_connected_service, remote_update->connected_service);
                if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                    && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    local_context->bt_sink_srv_state = APP_POWER_SAVING_BT_DISCONNECTED;
                } else if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                           && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                    local_context->bt_sink_srv_state = APP_POWER_SAVING_BT_CONNECTED;
                }
            }
#endif
        }
        break;
        default:
            break;
    }

    bt_changed = (old_bt_state != local_context->bt_sink_srv_state);
    APPS_LOG_MSGID_I(LOG_TAG"bt_state change = %d->%d", 2, old_bt_state, local_context->bt_sink_srv_state);
    return bt_changed;
}

void app_power_save_utils_refresh_waiting_time(void)
{
    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_POWER_SAVING,
                    APP_POWER_SAVING_EVENT_REFRESH_TIME, NULL, 0, NULL, 0);
}

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) && defined(AIR_SILENCE_DETECTION_ENABLE)
void app_power_save_utils_slave_silence_detect(bool silence)
{
    s_slave_audio_silence = silence;
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_POWER_SAVING,
                    APP_POWER_SAVING_EVENT_NOTIFY_CHANGE, NULL, 0, NULL, 0);
}
#endif

#define SECOND_TO_MS(a) (a*1000)
#define MS_TO_SECOND(a) (a/1000)

bool app_power_save_utils_is_enabled() {
    uint16_t enabled = true;
    enabled = app_power_saving_get_cfg()->power_saving_enable;
    APPS_LOG_MSGID_I(LOG_TAG", power saving cfg en: %d.", 1, enabled);
    return enabled == APP_POWER_SAVING_ENABLED ? true : false;
}

uint32_t app_power_save_utils_get_timeout(app_power_saving_type_t type) {
    uint16_t timeout = 0;
    if (APP_POWER_SAVING_TYPE_NO_CONNECTION == type) {
        timeout = app_power_saving_get_cfg()->timeout;
    } else {
        timeout = app_power_saving_get_cfg()->silence_detect_timeout;
    }
    
    if (timeout < MS_TO_SECOND(APPS_MIN_TIMEOUT_OF_SLEEP_AFTER_NO_CONNECTEION)) {
        timeout = MS_TO_SECOND(APPS_MIN_TIMEOUT_OF_SLEEP_AFTER_NO_CONNECTEION);
    }
    APPS_LOG_MSGID_I(LOG_TAG", power saving cfg timeout: %d.", 1, SECOND_TO_MS(timeout));
    return SECOND_TO_MS(timeout);
}

app_power_saving_cfg* app_power_saving_get_cfg(void) {
    return &s_cfg;
}

void app_power_save_utils_load_cfg(void)
{
    nvkey_status_t sta = NVKEY_STATUS_OK;
    app_power_saving_cfg* p_cfg = NULL;
    uint32_t size = sizeof(app_power_saving_cfg);

    p_cfg = app_power_saving_get_cfg();
    sta = nvkey_read_data(NVKEYID_APP_POWER_SAVING_CFG, (uint8_t*)p_cfg, &size);
    if (sta != NVKEY_STATUS_OK) {
        p_cfg->power_saving_enable = APP_POWER_SAVING_ENABLED;
        p_cfg->timeout = MS_TO_SECOND(APPS_TIMEOUT_OF_SLEEP_AFTER_NO_CONNECTION);
#ifdef AIR_SILENCE_DETECTION_ENABLE
        p_cfg->silence_detect_enable = APP_POWER_SAVING_ENABLED;
        p_cfg->silence_detect_timeout = MS_TO_SECOND(APPS_TIMEOUT_OF_SLEEP_OF_SILENCE_DETECT);
#else
        p_cfg->silence_detect_enable = APP_POWER_SAVING_DISABLED;
        p_cfg->silence_detect_timeout = 0;
#endif
        if (sta == NVKEY_STATUS_ITEM_NOT_FOUND) {
            app_power_save_utils_set_cfg(p_cfg);
        } else {
            APPS_LOG_MSGID_I(LOG_TAG", load power saving cfg failed: %d.", 1, sta);
        }
    }
    APPS_LOG_MSGID_I(LOG_TAG", load power saving cfg: %d, %d, %d, %d.", 4, p_cfg->power_saving_enable, p_cfg->timeout, p_cfg->silence_detect_enable, p_cfg->silence_detect_timeout);
}

int32_t app_power_save_utils_set_cfg(app_power_saving_cfg* config) {
    nvkey_status_t sta = NVKEY_STATUS_OK;
    app_power_saving_cfg* p_cfg = NULL;

    p_cfg = app_power_saving_get_cfg();
    p_cfg->power_saving_enable = config->power_saving_enable;
    p_cfg->timeout = config->timeout;
    p_cfg->silence_detect_enable = config->silence_detect_enable;
    p_cfg->silence_detect_timeout = config->silence_detect_timeout;
    sta = nvkey_write_data(NVKEYID_APP_POWER_SAVING_CFG, (uint8_t*)p_cfg, sizeof(app_power_saving_cfg));
    if (sta != NVKEY_STATUS_OK) {
        APPS_LOG_MSGID_I(LOG_TAG", set power saving cfg failed: %d.", 1, sta);
        return -1;
    } else {
        APPS_LOG_MSGID_I(LOG_TAG", set power saving cfg: %d, %d. %d, %d", 4, p_cfg->power_saving_enable, p_cfg->timeout, p_cfg->silence_detect_enable, p_cfg->silence_detect_timeout);
    }
    return 0;
}


void app_power_save_utils_cfg_updated_notify() {
    ui_shell_remove_event(EVENT_GROUP_UI_SHELL_POWER_SAVING, APP_POWER_SAVING_EVENT_TIMEOUT);
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_POWER_SAVING,
                        APP_POWER_SAVING_EVENT_NOTIFY_CHANGE, NULL, 0, NULL, 0);
    ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_POWER_SAVING,
                        APP_POWER_SAVING_EVENT_REFRESH_TIME, NULL, 0, NULL, 0);
}

void app_power_save_utils_init(void)
{
    app_power_save_utils_load_cfg();

}
