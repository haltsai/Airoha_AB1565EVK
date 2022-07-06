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
 * File: apps_config_audio_helper.c
 *
 * Description: This file provide some API to implement audio features, because the middleware API cannot be used directly.
 *
 */

#include "apps_config_audio_helper.h"

#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_aws_sync_event.h"

#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
#include "app_advance_passthrough.h"
#endif
#include "bt_device_manager.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_aws_mce_srv.h"
#endif
#include "nvdm.h"
#include "nvdm_id_list.h"
#include "apps_race_cmd_co_sys_event.h"

#define LOG_TAG                                             "[app_audio_helper]"

#define APP_CONFIG_AUDIO_HELPER_SIDETONE_DISABLE_VALUE      (-100)

static app_config_audio_helper_sidetone_data_t s_sidetone_data;

#ifdef MTK_AWS_MCE_ENABLE
static void apps_config_audio_helper_aws_sync_sidetone_data(void)
{
    if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
        apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_SYNC_SIDETONE,
                                       &s_sidetone_data, sizeof(s_sidetone_data));
    }
}
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
static void apps_config_audio_helper_dual_sync_sidetone_data(void)
{
    app_race_cmd_co_sys_send_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_SYNC_SIDETONE, &s_sidetone_data, sizeof(s_sidetone_data), false);
}
#endif

const app_config_audio_helper_sidetone_data_t *apps_config_audio_helper_get_sidetone_data(void)
{
    return &s_sidetone_data;
}

static bt_sink_srv_am_result_t apps_config_audio_helper_change_sidetone_enable(bool enable, bool need_sync)
{
    bt_sink_srv_am_result_t am_ret = AUD_EXECUTION_SUCCESS;
    int32_t target_value = APP_CONFIG_AUDIO_HELPER_SIDETONE_DISABLE_VALUE;
    APPS_LOG_MSGID_I(LOG_TAG "set_sidetone_enable :%d->%d", 2, s_sidetone_data.enable, enable);
    if (s_sidetone_data.enable == enable) {
        return am_ret;
    } else {
        s_sidetone_data.enable = enable;
    }
    if (s_sidetone_data.enable) {
        target_value = s_sidetone_data.value;
    }
    if (am_audio_get_test_mode_config(SIDETONE_TEST_MODE)) {
        APPS_LOG_MSGID_W(LOG_TAG "set_sidetone_enable, in am_audio_get_test_mode", 0);
    } else {
        am_ret = am_audio_side_tone_set_volume_by_scenario(SIDETONE_SCENARIO_HFP, target_value);
    }
    nvdm_write_data_item(NVDM_GROUP_APP, NVDM_APP_SIDETONE_VALUE, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&s_sidetone_data, sizeof(s_sidetone_data));
    if (need_sync) {
#ifdef MTK_AWS_MCE_ENABLE
        apps_config_audio_helper_aws_sync_sidetone_data();
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
        apps_config_audio_helper_dual_sync_sidetone_data();
#endif
    }
    return am_ret;
}

static bt_sink_srv_am_result_t apps_config_audio_helper_change_sidetone_value(int32_t value, bool need_sync)
{
    bt_sink_srv_am_result_t am_ret = AUD_EXECUTION_SUCCESS;
    APPS_LOG_MSGID_I(LOG_TAG "set_sidetone_value :%d->%d, enable?%d", 3, s_sidetone_data.value, value, s_sidetone_data.enable);
    if (s_sidetone_data.value != value) {
        s_sidetone_data.value = value;
        if (s_sidetone_data.enable) {
            if (am_audio_get_test_mode_config(SIDETONE_TEST_MODE)) {
                APPS_LOG_MSGID_W(LOG_TAG "change_sidetone_value in am_audio_get_test_mode", 0);
            } else {
                am_ret = am_audio_side_tone_set_volume_by_scenario(SIDETONE_SCENARIO_HFP, s_sidetone_data.value);
            }
        }
        nvdm_write_data_item(NVDM_GROUP_APP, NVDM_APP_SIDETONE_VALUE, NVDM_DATA_ITEM_TYPE_RAW_DATA, (uint8_t *)&s_sidetone_data, sizeof(s_sidetone_data));
        if (need_sync) {
#ifdef MTK_AWS_MCE_ENABLE
            apps_config_audio_helper_aws_sync_sidetone_data();
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
            apps_config_audio_helper_dual_sync_sidetone_data();
#endif
        }
    }
    return am_ret;
}

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE) || defined(MTK_AWS_MCE_ENABLE)
static void apps_config_audio_helper_received_sidetone_sync(app_config_audio_helper_sidetone_data_t *received_data)
{
    apps_config_audio_helper_change_sidetone_enable(received_data->enable, false);
    apps_config_audio_helper_change_sidetone_value(received_data->value, false);
}
#endif

bt_sink_srv_am_result_t apps_config_audio_helper_set_sidetone_enable(bool enable)
{
    return apps_config_audio_helper_change_sidetone_enable(enable, true);
}

bt_sink_srv_am_result_t apps_config_audio_helper_set_sidetone_value(int32_t value)
{
    return apps_config_audio_helper_change_sidetone_value(value, true);
}

void apps_config_audio_helper_sidetone_init(void)
{
    uint32_t read_nvdm_len = sizeof(s_sidetone_data);
    if (NVDM_STATUS_OK == nvdm_read_data_item(NVDM_GROUP_APP, NVDM_APP_SIDETONE_VALUE, (uint8_t *)&s_sidetone_data, &read_nvdm_len)) {
        APPS_LOG_MSGID_I(LOG_TAG "sidetone_init :%d, %d", 2, s_sidetone_data.enable, s_sidetone_data.value);
        if (read_nvdm_len <= sizeof(s_sidetone_data)) {
            /* default is true */
            s_sidetone_data.enable = true;
        }
        if (am_audio_get_test_mode_config(SIDETONE_TEST_MODE)) {
                APPS_LOG_MSGID_W(LOG_TAG "init in am_audio_get_test_mode", 0);
        } else {
            am_audio_side_tone_set_volume_by_scenario(SIDETONE_SCENARIO_HFP, s_sidetone_data.value);
        }
    } else {
        s_sidetone_data.enable = true;
        s_sidetone_data.value = am_audio_side_tone_get_volume_by_scenario(SIDETONE_SCENARIO_HFP);
    }
}

#ifdef MTK_AWS_MCE_ENABLE
static void apps_config_audio_helper_on_aws_state_change(bool connected)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    APPS_LOG_MSGID_I(LOG_TAG "[%02X] on_aws_state_change %d", 2, role, connected);
    if (BT_AWS_MCE_ROLE_AGENT == role && connected) {
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
        app_advance_passthrough_sync_to_partner();
#endif
        apps_config_audio_helper_aws_sync_sidetone_data();
    }
}

static bool apps_config_audio_helper_on_aws_data_received(uint32_t event_group, uint32_t event_id,
                                                          void *extra_data, uint32_t extra_data_len)
{
    bool ret = false;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            switch (event_id) {
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
                case APPS_EVENTS_INTERACTION_SYNC_ADVANCE_PASSTHROUGH: {
                    app_advance_passthrough_handle_aws_data(extra_data, extra_data_len);
                    ret = true;
                    break;
                }
                case APPS_EVENTS_INTERACTION_LEAKAGE_DETECTION_STOP: {
                    APPS_LOG_MSGID_I(LOG_TAG "[ADVANCE_PT] AWS Data, LEAKAGE_DETECTION_STOP event", 0);
                    app_advance_passthrough_set_ld_ongoing(FALSE);
                    break;
                }
#endif
                case APPS_EVENTS_INTERACTION_SYNC_SIDETONE: {
                    if (extra_data != NULL && extra_data_len == sizeof(app_config_audio_helper_sidetone_data_t)) {
                        app_config_audio_helper_sidetone_data_t *temp_sidetone = (app_config_audio_helper_sidetone_data_t *)extra_data;
                        APPS_LOG_MSGID_I(LOG_TAG "on_aws_data_received, SYNC_SIDETONE", 0);
                        apps_config_audio_helper_received_sidetone_sync(temp_sidetone);
                    }
                    ret = true;
                    break;
                }
                default:
                    break;
            }
            break;
        }
        default:
            break;
    }
    return ret;
}

static bool apps_config_audio_helper_proc_aws_data_event(void *extra_data, uint32_t extra_data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group = 0;
        uint32_t event_id = 0;
        void *event_data = NULL;
        uint32_t event_data_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event_id,
                                         &event_data, &event_data_len);

        ret = apps_config_audio_helper_on_aws_data_received(event_group, event_id,
                                                            event_data, event_data_len);
    }
    return ret;
}
#endif /* MTK_AWS_MCE_ENABLE */





/**================================================================================*/
/**                                  Audio Helper API                              */
/**================================================================================*/
bool apps_config_audio_helper_proc(uint32_t event_group, uint32_t event_id, void *extra_data, uint32_t extra_data_len)
{
    bool ret = false;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            switch (event_id) {
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE) || defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_SLAVE_ENABLE)
                case APPS_EVENTS_INTERACTION_SYNC_SIDETONE: {
                    if (extra_data != NULL && extra_data_len == sizeof(app_config_audio_helper_sidetone_data_t)) {
                        app_config_audio_helper_sidetone_data_t *temp_sidetone = (app_config_audio_helper_sidetone_data_t *)extra_data;
                        APPS_LOG_MSGID_I(LOG_TAG "on_event_received, SYNC_SIDETONE", 0);
                        apps_config_audio_helper_received_sidetone_sync(temp_sidetone);
                    }
                    ret = true;
                    break;
                }
#endif
                default:
                    break;
            }
            break;
        }

#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            if (event_id == BT_CM_EVENT_REMOTE_INFO_UPDATE && extra_data != NULL) {
                bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
                if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)
                    & ((~remote_update->pre_connected_service) & remote_update->connected_service)) {
                    apps_config_audio_helper_on_aws_state_change(true);
                }
            }
            break;
        }

        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            apps_config_audio_helper_proc_aws_data_event(extra_data, extra_data_len);
            break;
        }
#endif

        default:
            break;
    }

#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
    app_advance_passthrough_proc(event_group, event_id, extra_data, extra_data_len);
#endif
    return ret;
}

void apps_config_audio_helper_init(void)
{
    APPS_LOG_MSGID_I(LOG_TAG" init", 0);
#ifdef AIR_ADVANCED_PASSTHROUGH_ENABLE
    app_advance_passthrough_init();
#endif
}
