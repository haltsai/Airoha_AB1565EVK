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

#ifdef AIR_LE_AUDIO_BIS_ENABLE

#include "app_le_audio_bis.h"
#include "app_le_audio.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "apps_aws_sync_event.h"
#include "bt_aws_mce_report.h"
#include "bt_aws_mce_srv.h"
#include "bt_device_manager.h"
#endif

#include "apps_debug.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_events_bt_event.h"
#include "apps_events_key_event.h"
#include "apps_config_key_remapper.h"
#include "bt_connection_manager.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_le.h"
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "ui_shell_manager.h"

#include "app_le_audio_bis_activity.h"

#include "bt_sink_srv_ami.h"
#include "bt_sink_srv_le_cap.h"
#include "bt_sink_srv_le_cap_stream.h"
#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
#include "app_power_save_utils.h"
#endif



#define LOG_TAG                                         "[LEA][BIS]"

#ifndef PACKED
#define PACKED __attribute__((packed))
#endif

#define BIS_INVALID_PEER_VALUE                          0xFF
#define BIS_MIN_SCAN_TIME                               (5 * 1000)               /** 5 sec, scan with addr but peer not streaming. */
#define BIS_DEFAULT_SCAN_TIME                           (30 * 1000)              /** 30 sec, scan without addr. */
#define BIS_MAX_SCAN_TIME                               (10 * 60 * 1000)         /** 10 min, scan with addr but peer streaming. */
#define BIS_CODE_SIZE                                   (16)                     /** broadcast code size. */

typedef enum {
    APP_LEA_BIS_STATE_IDLE                              = 0,
    APP_LEA_BIS_STATE_SCANNING,
    APP_LEA_BIS_STATE_STREAMING
} app_le_audio_bis_state_t;

#ifdef MTK_AWS_MCE_ENABLE
typedef enum {
    APP_LEA_BIS_SYNC_ONLY_STATE                         = 0,
    APP_LEA_BIS_SYNC_ONLY_SRC_ADDR,
    APP_LEA_BIS_SYNC_ONLY_VOLUME,
    APP_LEA_BIS_SYNC_ALL
} app_le_audio_bis_sync_t;

typedef struct {
    app_le_audio_bis_sync_t                             sync_type;
    bool                                                sync_feature;
    app_le_audio_bis_state_t                            state;
    uint8_t                                             volume;
    uint8_t                                             src_addr[BT_BD_ADDR_LEN];
    uint8_t                                             broadcast_code[BIS_CODE_SIZE];
} PACKED app_le_audio_bis_aws_data_t;
#endif

typedef struct {
    app_le_audio_bis_state_t                            state;
    uint8_t                                             src_addr[BT_BD_ADDR_LEN];
    bool                                                stopping_scan_then_retry;
    bool                                                stopping_streaming_flag;
    bool                                                retry_scan_when_timeout;
#ifdef MTK_AWS_MCE_ENABLE
    app_le_audio_bis_state_t                            peer_state;
    uint8_t                                             peer_volume;
    uint8_t                                             peer_src_addr[BT_BD_ADDR_LEN];
#endif
} app_le_audio_bis_context_t;

static app_le_audio_bis_context_t                       app_bis_ctx = {0};

#ifdef MTK_AWS_MCE_ENABLE
// For BSA control single headset, BSA need to disable the SYNC feature (sync state/volume, but not sync to BIS start/stop action)
bool                                                    app_bis_sync_feature = TRUE;

bool                                                    app_bis_temp_cancel_sync = FALSE;
#endif


/**============================================================================*/
/**                            Internal Function                               */
/**============================================================================*/
static bool app_le_audio_bis_start(bool start);

static void app_le_audio_bis_print_context(void)
{
    uint8_t *addr = app_bis_ctx.src_addr;
    bt_sink_srv_am_volume_level_out_t volume = bt_sink_srv_cap_stream_get_broadcast_volume();
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    uint8_t *addr1 = app_bis_ctx.peer_src_addr;
    APPS_LOG_MSGID_I(LOG_TAG" Context, [%02X] state=%X-%X volume=%d-%d stopping_scan_then_retry=%d src_addr=%02X:%02X:%02X:%02X:%02X:%02X peer_src_addr=%02X:%02X:%02X:%02X:%02X:%02X",
                     18, role, app_bis_ctx.state, app_bis_ctx.peer_state, volume, app_bis_ctx.peer_volume,
                     app_bis_ctx.stopping_scan_then_retry, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
                     addr1[0], addr1[1], addr1[2], addr1[3], addr1[4], addr1[5]);
#else
    APPS_LOG_MSGID_I(LOG_TAG" Context, state=%d volume=%d stopping_scan_then_retry=%d src_addr=%02X:%02X:%02X:%02X:%02X:%02X",
                     9, app_bis_ctx.state, volume, app_bis_ctx.stopping_scan_then_retry,
                     addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
#endif
}

#ifdef MTK_AWS_MCE_ENABLE
static void app_le_audio_bis_sync_to_peer(app_le_audio_bis_sync_t sync_type)
{
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_srv_link_type_t aws_link_type = bt_aws_mce_srv_get_link_type();
    bt_status_t bt_status = BT_STATUS_SUCCESS;

    if (app_bis_temp_cancel_sync) {
        app_bis_temp_cancel_sync = FALSE;
        APPS_LOG_MSGID_I(LOG_TAG" sync_to_peer, temp_cancel_sync=TRUE", 0);
        return;
    }

    if (aws_link_type == BT_AWS_MCE_SRV_LINK_NONE) {
        APPS_LOG_MSGID_I(LOG_TAG" sync_to_peer, fail aws_link_type=0x%02X", 1, aws_link_type);
        return;
    }

    app_le_audio_bis_aws_data_t aws_data = {0};
    aws_data.sync_type = sync_type;
    aws_data.sync_feature = app_bis_sync_feature;

    uint8_t volume = bt_sink_srv_cap_stream_get_broadcast_volume();
    if (sync_type == APP_LEA_BIS_SYNC_ONLY_STATE) {
        aws_data.state = app_bis_ctx.state;
    } else if (sync_type == APP_LEA_BIS_SYNC_ONLY_SRC_ADDR) {
        memcpy(aws_data.src_addr, app_bis_ctx.src_addr, BT_BD_ADDR_LEN);
    } else if (sync_type == APP_LEA_BIS_SYNC_ONLY_VOLUME) {
        aws_data.volume = volume;
    } else {
        aws_data.state = app_bis_ctx.state;
        aws_data.volume = volume;
        memcpy(aws_data.src_addr, app_bis_ctx.src_addr, BT_BD_ADDR_LEN);

        uint8_t broadcast_code[BIS_CODE_SIZE] = {0};
        bt_status = bt_sink_srv_cap_stream_get_broadcast_code(broadcast_code);
        APPS_LOG_MSGID_I(LOG_TAG" sync_to_peer, [%2X] bt_status=%08X broadcast_code=%02X:%02X:%02X",
                         5, role, bt_status, broadcast_code[0], broadcast_code[1], broadcast_code[2]);
        memcpy(aws_data.broadcast_code, broadcast_code, BIS_CODE_SIZE);
    }

    bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                               EVENT_ID_LE_AUDIO_BIS_SYNC_TO_PEER,
                                               &aws_data,
                                               sizeof(app_le_audio_bis_aws_data_t));

    uint8_t *addr = aws_data.src_addr;
    APPS_LOG_MSGID_I(LOG_TAG" sync_to_peer, [%2X] sync_type=%d bt_status=%08X state=%X volume=%d src_addr=%02X:%02X:%02X:%02X:%02X:%02X",
                     11, role, aws_data.sync_type, bt_status, aws_data.state, aws_data.volume,
                     addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}
#endif

static void app_le_audio_bis_set_state(uint8_t state)
{
    APPS_LOG_MSGID_I(LOG_TAG" set_state, state=%X->%X", 2, app_bis_ctx.state, state);
    if (app_bis_ctx.state != state) {
        app_bis_ctx.state = state;

#ifdef MTK_AWS_MCE_ENABLE
        if (state == APP_LEA_BIS_STATE_STREAMING) {
            // Sync state/volume/src_addr to peer
            app_le_audio_bis_sync_to_peer(APP_LEA_BIS_SYNC_ALL);
        } else {
            // Only sync state
            app_le_audio_bis_sync_to_peer(APP_LEA_BIS_SYNC_ONLY_STATE);
        }
#endif
    }
}

static bool app_le_audio_bis_scan()
{
    uint8_t empty_addr[BT_BD_ADDR_LEN] = {0};

#ifdef MTK_AWS_MCE_ENABLE
    if (memcmp(app_bis_ctx.src_addr, empty_addr, BT_BD_ADDR_LEN) == 0
        && app_bis_ctx.peer_state == APP_LEA_BIS_STATE_STREAMING
        && memcmp(app_bis_ctx.peer_src_addr, empty_addr, BT_BD_ADDR_LEN) != 0) {
        memcpy(app_bis_ctx.src_addr, app_bis_ctx.peer_src_addr, BT_BD_ADDR_LEN);
        uint8_t *addr = app_bis_ctx.src_addr;
        APPS_LOG_MSGID_I(LOG_TAG" SCAN, sync peer src_addr %02X:%02X:%02X:%02X:%02X:%02X",
                         6, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
    }
#endif

    uint32_t scan_duration = 0;
    bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
    scan_param.duration = 0;
    if (memcmp(app_bis_ctx.src_addr, empty_addr, BT_BD_ADDR_LEN) == 0) {
        scan_param.bms_address = NULL;
        scan_duration = BIS_DEFAULT_SCAN_TIME;
    } else {
        bt_bd_addr_t bd_addr;
        memcpy(bd_addr, app_bis_ctx.src_addr, BT_BD_ADDR_LEN);
        scan_param.bms_address = &bd_addr;

#ifdef MTK_AWS_MCE_ENABLE
        if (app_bis_ctx.peer_state == APP_LEA_BIS_STATE_STREAMING) {
            scan_duration = BIS_MAX_SCAN_TIME;
        } else {
            scan_duration = BIS_MIN_SCAN_TIME;
        }
#else
        scan_duration = BIS_MIN_SCAN_TIME;
#endif
    }

#ifdef MTK_AWS_MCE_ENABLE
    audio_channel_t channel = ami_get_audio_channel();
    if (channel == AUDIO_CHANNEL_L) {
        scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_LEFT;
    } else {
        scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_RIGHT;
    }
#else
    scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_LEFT | AUDIO_LOCATION_FRONT_RIGHT;
#endif

    bt_status_t bt_status = BT_STATUS_FAIL;
    if (bt_sink_srv_cap_stream_is_scanning_broadcast_source()) {
        bt_status = bt_sink_srv_cap_stream_stop_scanning_broadcast_source();
        APPS_LOG_MSGID_I(LOG_TAG" SCAN, need to stop bt_status=0x%08X", 1, bt_status);
        if (bt_status == BT_STATUS_SUCCESS) {
            app_bis_ctx.stopping_scan_then_retry = TRUE;
        }
    } else {
        bt_status = bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
        APPS_LOG_MSGID_I(LOG_TAG" SCAN, bms_address=0x%08X duration=%d channel=0x%08X bt_status=0x%08X",
                         4, scan_param.bms_address, scan_duration,
                         scan_param.audio_channel_allocation, bt_status);
        if (bt_status == BT_STATUS_SUCCESS) {
            app_le_audio_bis_set_state(APP_LEA_BIS_STATE_SCANNING);
            app_bis_ctx.retry_scan_when_timeout = (scan_duration == BIS_MIN_SCAN_TIME);
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_BIS_SCAN_TIMEOUT);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                EVENT_ID_LE_AUDIO_BIS_SCAN_TIMEOUT,
                                NULL, 0, NULL, scan_duration);
        }
        app_bis_ctx.stopping_scan_then_retry = FALSE;
    }
    return (bt_status == BT_STATUS_SUCCESS);
}

// event -> bt_sink_srv_cap_event_id_t
static void app_le_audio_bis_callback(bt_le_audio_sink_event_t event, void *msg)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG" bis_callback, [%02X] event=0x%04X", 2, role, event);
#else
    APPS_LOG_MSGID_I(LOG_TAG" bis_callback, event=0x%04X", 1, event);
#endif

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
    if (event == BT_SINK_SRV_CAP_EVENT_BASE_BIG_SYNC_ESTABLISHED
            || event == BT_SINK_SRV_CAP_EVENT_BASE_BIG_TERMINATE_CFM) {
        app_power_save_utils_notify_mode_changed(FALSE, NULL);
    }
#endif

    if (event == BT_SINK_SRV_CAP_EVENT_BASE_BIG_SYNC_ESTABLISHED) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_BIS_SCAN_TIMEOUT);
        bt_sink_srv_cap_event_base_big_sync_established_t *ind = (bt_sink_srv_cap_event_base_big_sync_established_t *)msg;
        if (ind != NULL && ind->status == BT_STATUS_SUCCESS) {
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                EVENT_ID_LE_AUDIO_BIS_START_STREAMING,
                                NULL, 0, NULL, 0);
        } else {
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                EVENT_ID_LE_AUDIO_BIS_ERROR,
                                NULL, 0, NULL, 0);
        }
    } else if (event == BT_SINK_SRV_CAP_EVENT_BASE_BIG_TERMINATE_CFM) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_BIS_SCAN_TIMEOUT);
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                            EVENT_GROUP_UI_SHELL_LE_AUDIO,
                            EVENT_ID_LE_AUDIO_BIS_STOP_STREAMING,
                            NULL, 0, NULL, 0);
        if (app_bis_ctx.stopping_streaming_flag) {
            app_bis_ctx.stopping_streaming_flag = FALSE;
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_BT_RETRY_POWER_ON_OFF, NULL, 0,
                                NULL, 0);
        }
    } else if (event == BT_SINK_SRV_CAP_EVENT_BASE_PERIODIC_ADV_SYNC_ESTABLISHED) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_BIS_SCAN_TIMEOUT);
        bt_sink_srv_cap_event_base_periodic_adv_sync_established_t *ind = (bt_sink_srv_cap_event_base_periodic_adv_sync_established_t *) msg;
        if (ind != NULL && ind->status == BT_STATUS_SUCCESS) {
            uint8_t *addr = (uint8_t *)pvPortMalloc(BT_BD_ADDR_LEN);
            if (addr == NULL) {
                return;
            }
            memcpy(addr, ind->advertiser_addr.addr, BT_BD_ADDR_LEN);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                EVENT_ID_LE_AUDIO_BIS_SYNC_SRC_ADDR,
                                addr, BT_BD_ADDR_LEN, NULL, 0);
        } else {
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                EVENT_ID_LE_AUDIO_BIS_ERROR,
                                NULL, 0, NULL, 0);
        }
    } else if (event == BT_SINK_SRV_CAP_EVENT_BASE_SCAN_TIMEOUT) {
        APPS_LOG_MSGID_E(LOG_TAG" bis_callback, ignore SCAN_TIMEOUT", 0);
    } else if (event == BT_SINK_SRV_CAP_EVENT_BASE_SCAN_STOPPED) {
        ui_shell_remove_event(EVENT_GROUP_UI_SHELL_LE_AUDIO, EVENT_ID_LE_AUDIO_BIS_SCAN_TIMEOUT);
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                            EVENT_GROUP_UI_SHELL_LE_AUDIO,
                            EVENT_ID_LE_AUDIO_BIS_SCAN_STOPPED,
                            NULL, 0, NULL, 0);
    }
#ifdef MTK_AWS_MCE_ENABLE
    else if (event == BT_SINK_SRV_CAP_EVENT_BASE_BASS_ADD_SOURCE) {
        bt_sink_srv_cap_event_base_bass_add_source_t *ind = (bt_sink_srv_cap_event_base_bass_add_source_t *) msg;
        uint32_t pa_sync = (uint32_t)ind->param->pa_sync;
        APPS_LOG_MSGID_E(LOG_TAG" bis_callback, BASE_BASS_ADD_SOURCE pa_sync=%d", 1, pa_sync);
        ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                            EVENT_GROUP_UI_SHELL_LE_AUDIO,
                            EVENT_ID_LE_AUDIO_BIS_SYNC_FEATURE,
                            (void *)pa_sync, 0, NULL, 0);
    }
#endif
}

static bool app_le_audio_bis_start(bool start)
{
    bool success = FALSE;
    bt_sink_srv_state_t bt_sink_state = bt_sink_srv_get_state();
    if (start && bt_sink_state >= BT_SINK_SRV_STATE_INCOMING) {
        APPS_LOG_MSGID_E(LOG_TAG" SCAN, fail HFP ongoing %d", 1, bt_sink_state);
        return success;
    }

    bool is_bis_streaming = bt_sink_srv_cap_stream_is_broadcast_streaming();
    if (!is_bis_streaming && start) {
        success = app_le_audio_bis_scan();
        APPS_LOG_MSGID_I(LOG_TAG" SCAN, success=%d", 1, success);
    } else if (is_bis_streaming && !start) {
        success = bt_sink_srv_cap_stream_stop_broadcast_reception();
        APPS_LOG_MSGID_I(LOG_TAG" STOP, success=%d", 1, success);
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" start error, is_bis_streaming=%d start=%d",
                         2, is_bis_streaming, start);
    }
    return success;
}



/**============================================================================*/
/**                               Event Handler                                */
/**============================================================================*/
static void app_le_audio_bis_proc_bt_cm_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;

            if (NULL == remote_update) {
                APPS_LOG_MSGID_E(LOG_TAG" BT CM Event, NULL remote_update", 0);
                break;
            }

#ifdef MTK_AWS_MCE_ENABLE
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            if (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service) &&
                (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                bool is_bis_streaming = bt_sink_srv_cap_stream_is_broadcast_streaming();
                app_le_audio_bis_state_t bis_state = app_bis_ctx.state;
                APPS_LOG_MSGID_I(LOG_TAG" BT CM Event, [%02X] AWS Connected, is_bis_streaming=%d bis_state=%d bis_sync_feature=%d",
                                 4, role, is_bis_streaming, bis_state, app_bis_sync_feature);
                if (is_bis_streaming) {
                    if (bis_state == APP_LEA_BIS_STATE_STREAMING) {
                        app_le_audio_bis_sync_to_peer(APP_LEA_BIS_SYNC_ALL);
                    } else {
                        // Need to wait EVENT_ID_LE_AUDIO_BIS_START_STREAMING, then set state and sync all
                        APPS_LOG_MSGID_E(LOG_TAG" BT CM Event, [%02X] AWS Connected, BIS streaming/state not match", 0);
                    }
                }
            }

            if ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service) &&
                !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service)) {
                APPS_LOG_MSGID_I(LOG_TAG" [LEA_Power_Saving] BT CM Event, [%02X] AWS Disconnected", 1, role);
                app_bis_ctx.peer_state = BIS_INVALID_PEER_VALUE;
                app_bis_ctx.peer_volume = BIS_INVALID_PEER_VALUE;
                memset(app_bis_ctx.peer_src_addr, 0, BT_BD_ADDR_LEN);

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                app_power_save_utils_notify_mode_changed(FALSE, NULL);
#endif
            }
#endif
            break;
        }
    }
}

static void app_le_audio_bis_proc_bt_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_POWER_OFF_CNF: {
            APPS_LOG_MSGID_I(LOG_TAG" BT Event, BT Power OFF CNF", 0);
            ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGNEST,
                                EVENT_GROUP_UI_SHELL_LE_AUDIO,
                                EVENT_ID_LE_AUDIO_BIS_STOP_STREAMING,
                                NULL, 0, NULL, 0);
            break;
        }
    }
}

static void app_le_audio_bis_proc_key_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    uint16_t key_action = 0;
    if (extra_data != NULL) {
        key_action = *(uint16_t *)extra_data;
    }

    if (key_action != KEY_LE_AUDIO_BIS_SCAN && key_action != KEY_LE_AUDIO_BIS_STOP) {
        return;
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG" KEY Event, [%02X] action=0x%04X", 2, role, key_action);
#else
    APPS_LOG_MSGID_I(LOG_TAG" KEY Event, action=0x%04X", 1, key_action);
#endif

    app_le_audio_bis_print_context();

    if (app_bis_ctx.state == APP_LEA_BIS_STATE_SCANNING
        && key_action == KEY_LE_AUDIO_BIS_SCAN) {
        APPS_LOG_MSGID_I(LOG_TAG" KEY Event, ignore SCAN Key_Action", 0);
        // play Fail VP
        return;
    }

    if (key_action == KEY_LE_AUDIO_BIS_SCAN) {
        app_le_audio_bis_start(TRUE);
    } else if (key_action == KEY_LE_AUDIO_BIS_STOP) {
        app_le_audio_bis_start(FALSE);
    }
}

#ifdef MTK_AWS_MCE_ENABLE
static void app_le_audio_bis_action_by_peer(void)
{
    if (!app_bis_sync_feature) {
        APPS_LOG_MSGID_I(LOG_TAG" bis_action_by_peer, sync_feature=DISABLE", 0);
        return;
    }

    uint8_t empty_addr[BT_BD_ADDR_LEN] = {0};
    uint8_t *peer_addr = app_bis_ctx.peer_src_addr;

    // Scan action by peer streaming state
    if (app_bis_ctx.peer_state == APP_LEA_BIS_STATE_STREAMING) {
        if (app_bis_ctx.state != APP_LEA_BIS_STATE_STREAMING
            && memcmp(peer_addr, empty_addr, BT_BD_ADDR_LEN) != 0) {
            memcpy(app_bis_ctx.src_addr, peer_addr, BT_BD_ADDR_LEN);
            APPS_LOG_MSGID_I(LOG_TAG" AWS Data Event, sync and start %02X:%02X:%02X:%02X:%02X:%02X",
                             6, peer_addr[0], peer_addr[1], peer_addr[2],
                             peer_addr[3], peer_addr[4], peer_addr[5]);
            app_le_audio_bis_start(TRUE);
        } else {
            APPS_LOG_MSGID_I(LOG_TAG" AWS Data Event, not sync and start", 0);
        }
    }
    // Stop action by peer idle state
    if (app_bis_ctx.peer_state == APP_LEA_BIS_STATE_IDLE) {
        if (app_bis_ctx.state == APP_LEA_BIS_STATE_SCANNING) {
            bt_status_t bt_status = bt_sink_srv_cap_stream_stop_scanning_broadcast_source();
            APPS_LOG_MSGID_I(LOG_TAG" AWS Data Event, stop scanning bt_statue=0x%08X", 1, bt_status);
            app_le_audio_bis_set_state(APP_LEA_BIS_STATE_IDLE);
        } else if (app_bis_ctx.state == APP_LEA_BIS_STATE_STREAMING) {
            APPS_LOG_MSGID_I(LOG_TAG" AWS Data Event, stop streaming", 0);
            app_le_audio_bis_start(FALSE);
        }
    }
}

static void app_le_audio_bis_proc_aws_data_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    uint32_t aws_event_group;
    uint32_t aws_event_id;
    void *p_extra_data = NULL;
    uint32_t extra_data_len = 0;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;

    if (!aws_data_ind || aws_data_ind->module_id != BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        return;
    }

    apps_aws_sync_event_decode_extra(aws_data_ind, &aws_event_group, &aws_event_id,
                                     &p_extra_data, &extra_data_len);

    if (aws_event_group == EVENT_GROUP_UI_SHELL_LE_AUDIO
        && aws_event_id == EVENT_ID_LE_AUDIO_BIS_SYNC_TO_PEER) {
        app_le_audio_bis_aws_data_t *aws_data = (app_le_audio_bis_aws_data_t *)p_extra_data;
        uint8_t sync_type = aws_data->sync_type;
        bool peer_sync_feature = aws_data->sync_feature;
        uint8_t state = aws_data->state;;
        uint8_t volume = aws_data->volume;
        uint8_t *addr = aws_data->src_addr;
        uint8_t *broadcast_code = aws_data->broadcast_code;
        APPS_LOG_MSGID_I(LOG_TAG" AWS Data Event, [%02X] sync_type=%d peer_sync_feature=%d peer_state=%X peer_volume=%d peer_src_addr=%02X:%02X:%02X:%02X:%02X:%02X broadcast_code=%02X:%02X:%02X",
                         14, role, sync_type, peer_sync_feature, state, volume, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
                         broadcast_code[0], broadcast_code[1], broadcast_code[2]);
        app_le_audio_bis_print_context();

        // Handle AWS Data -> src_addr
        if (sync_type == APP_LEA_BIS_SYNC_ONLY_SRC_ADDR || sync_type == APP_LEA_BIS_SYNC_ALL) {
            memcpy(app_bis_ctx.peer_src_addr, addr, BT_BD_ADDR_LEN);
        }
        // Handle AWS Data -> volume
        if (sync_type == APP_LEA_BIS_SYNC_ONLY_VOLUME || sync_type == APP_LEA_BIS_SYNC_ALL) {
            app_bis_ctx.peer_volume = volume;
        }
        // Handle AWS Data -> broadcast_code
        if (sync_type == APP_LEA_BIS_SYNC_ALL) {
            ble_bap_set_broadcast_code(broadcast_code);
        }
        // Handle AWS Data -> state
        if (sync_type == APP_LEA_BIS_SYNC_ONLY_STATE || sync_type == APP_LEA_BIS_SYNC_ALL) {
            if (app_bis_ctx.peer_state != state) {
                app_bis_ctx.peer_state = state;

                if (peer_sync_feature) {
                    app_le_audio_bis_action_by_peer();
                }

#ifdef APPS_SLEEP_AFTER_NO_CONNECTION
                if (state == APP_LEA_BIS_STATE_IDLE
                    || state == APP_LEA_BIS_STATE_STREAMING) {
                    app_power_save_utils_notify_mode_changed(FALSE, NULL);
                }
#endif

                bt_sink_srv_am_volume_level_out_t volume = bt_sink_srv_cap_stream_get_broadcast_volume();
                if (app_bis_ctx.state != APP_LEA_BIS_STATE_STREAMING
                    && app_bis_ctx.peer_state == APP_LEA_BIS_STATE_STREAMING
                    && app_bis_ctx.peer_volume != BIS_INVALID_PEER_VALUE
                    && app_bis_ctx.peer_volume != volume) {
                    bool ret = bt_sink_srv_cap_stream_set_broadcast_volume(app_bis_ctx.peer_volume);
                    APPS_LOG_MSGID_I(LOG_TAG" AWS Data Event, set peer_volume=%d ret=%d volume=%d",
                                     3, app_bis_ctx.peer_volume, ret, volume);
                    if (ret) {
                        app_le_audio_bis_sync_to_peer(APP_LEA_BIS_SYNC_ONLY_VOLUME);
                    }
                }
            }
        }
        app_le_audio_bis_print_context();
    }
}
#endif

static void app_le_audio_bis_proc_le_audio_group(struct _ui_shell_activity *self, uint32_t event_id,
                                                 void *extra_data, size_t data_len)
{
    APPS_LOG_MSGID_I(LOG_TAG" LE AUDIO Event, event_id=%d", 1, event_id);

    switch (event_id) {
        case EVENT_ID_LE_AUDIO_BIS_START_STREAMING: {
            APPS_LOG_MSGID_I(LOG_TAG" START_STREAMING event", 0);
            app_le_audio_bis_set_state(APP_LEA_BIS_STATE_STREAMING);
#ifdef MTK_AWS_MCE_ENABLE
            if (app_bis_ctx.peer_volume != BIS_INVALID_PEER_VALUE
                && app_bis_ctx.peer_state == APP_LEA_BIS_STATE_STREAMING) {
                bool ret = bt_sink_srv_cap_stream_set_broadcast_volume(app_bis_ctx.peer_volume);
                APPS_LOG_MSGID_I(LOG_TAG" START_STREAMING event, set volume=%d ret=%d",
                                 2, app_bis_ctx.peer_volume, ret);
                if (ret) {
                    app_le_audio_bis_sync_to_peer(APP_LEA_BIS_SYNC_ONLY_VOLUME);
                }
            }
#endif

            ui_shell_start_activity(self, app_le_audio_bis_activity_proc,
                                    ACTIVITY_PRIORITY_MIDDLE, NULL, 0);
            break;
        }

        case EVENT_ID_LE_AUDIO_BIS_STOP_STREAMING: {
            APPS_LOG_MSGID_I(LOG_TAG" STOP_STREAMING event, enable sync_feature", 0);
#ifdef MTK_AWS_MCE_ENABLE
            app_bis_sync_feature = TRUE;
#endif
            app_le_audio_bis_set_state(APP_LEA_BIS_STATE_IDLE);
            break;
        }

        case EVENT_ID_LE_AUDIO_BIS_SYNC_SRC_ADDR: {
            uint8_t *addr = (uint8_t *)extra_data;
            memcpy(app_bis_ctx.src_addr, addr, BT_BD_ADDR_LEN);
            APPS_LOG_MSGID_I(LOG_TAG" SYNC_SRC_ADDR event, src_addr=%02X:%02X:%02X:%02X:%02X:%02X",
                             6, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
            // Sync src_addr when streaming
            // app_le_audio_bis_sync_to_peer(APP_LEA_BIS_SYNC_ONLY_SRC_ADDR);
            break;
        }

        case EVENT_ID_LE_AUDIO_BIS_SCAN_TIMEOUT: {
            // APP stop BIS scan actively when scan timeout
            bt_status_t bt_status = bt_sink_srv_cap_stream_stop_scanning_broadcast_source();

            uint8_t *addr = app_bis_ctx.src_addr;
#ifdef MTK_AWS_MCE_ENABLE
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
            APPS_LOG_MSGID_I(LOG_TAG" SCAN_TIMEOUT event, [%02X] bt_status=0x%08X peer_state=%X clear src_addr=%02X:%02X:%02X:%02X:%02X:%02X retry_scan_when_timeout=%d",
                             10, role, bt_status, app_bis_ctx.peer_state, addr[0], addr[1], addr[2],
                             addr[3], addr[4], addr[5], app_bis_ctx.retry_scan_when_timeout);
#else
            APPS_LOG_MSGID_I(LOG_TAG" SCAN_TIMEOUT event, bt_status=0x%08X clear src_addr=%02X:%02X:%02X:%02X:%02X:%02X retry_scan_when_timeout=%d",
                             8, bt_status, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], app_bis_ctx.retry_scan_when_timeout);
#endif
            memset(app_bis_ctx.src_addr, 0, BT_BD_ADDR_LEN);
            // Local set IDLE to avoiding scanning->idle (sync stop state)
            app_bis_ctx.state = APP_LEA_BIS_STATE_IDLE;
            // 30sec/10min, not retry scan
            bool retry_scan_when_timeout = app_bis_ctx.retry_scan_when_timeout;
            app_bis_ctx.retry_scan_when_timeout = FALSE;
            app_bis_ctx.stopping_scan_then_retry = FALSE;

            if (retry_scan_when_timeout) {
                APPS_LOG_MSGID_I(LOG_TAG" SCAN_TIMEOUT event, will retry scan without addr", 0);
                app_bis_ctx.retry_scan_when_timeout = FALSE;
                // Local re-scan without addr when scan DEFAULT_TIME & time out
                app_bis_ctx.stopping_scan_then_retry = (bt_status == BT_STATUS_SUCCESS);
            }
            break;
        }

        case EVENT_ID_LE_AUDIO_BIS_SCAN_STOPPED: {
            APPS_LOG_MSGID_I(LOG_TAG" SCAN_STOPPED event, stopping_scan_then_retry=%d state=%d",
                             2, app_bis_ctx.stopping_scan_then_retry, app_bis_ctx.state);
            app_bis_ctx.state = APP_LEA_BIS_STATE_IDLE;
            if (app_bis_ctx.stopping_scan_then_retry) {
                app_bis_ctx.stopping_scan_then_retry = FALSE;
                app_le_audio_bis_start(TRUE);
            }
            break;
        }

        case EVENT_ID_LE_AUDIO_BIS_SYNC_FEATURE: {
#ifdef MTK_AWS_MCE_ENABLE
            uint32_t pa_sync = (uint32_t)extra_data;
            app_bis_sync_feature = (pa_sync == 0);
            APPS_LOG_MSGID_I(LOG_TAG" SYNC_FEATURE event, enable_sync_feature=%d", 1, app_bis_sync_feature);
#endif
            break;
        }

        case EVENT_ID_LE_AUDIO_BIS_ERROR: {
            APPS_LOG_MSGID_I(LOG_TAG" BIS_ERROR event", 0);
#ifdef MTK_AWS_MCE_ENABLE
            app_bis_sync_feature = TRUE;
            app_le_audio_bis_action_by_peer();
#endif
            break;
        }

        default:
            break;
    }
}





/**============================================================================*/
/**                                 PUBLIC API                                 */
/**============================================================================*/
void app_le_audio_bis_init(void)
{
    bt_status_t status = bt_sink_srv_cap_register_broadcast_callback(app_le_audio_bis_callback);
    APPS_LOG_MSGID_I(LOG_TAG" register, status=0x%08X", 1, status);

    memset(&app_bis_ctx, 0, sizeof(app_le_audio_bis_context_t));
#ifdef MTK_AWS_MCE_ENABLE
    app_bis_ctx.peer_state = BIS_INVALID_PEER_VALUE;
    app_bis_ctx.peer_volume = BIS_INVALID_PEER_VALUE;
#endif
}

bool app_le_audio_bis_is_streaming(void)
{
    bool is_bis_streaming = bt_sink_srv_cap_stream_is_broadcast_streaming();
#ifdef MTK_AWS_MCE_ENABLE
    if (!is_bis_streaming) {
        is_bis_streaming = (app_bis_ctx.peer_state == APP_LEA_BIS_STATE_STREAMING);
    }
#endif
    APPS_LOG_MSGID_I(LOG_TAG" [LEA_Power_Saving], is_bis_streaming=%d", 1, is_bis_streaming);
    return is_bis_streaming;
}

bool app_le_audio_bis_stop_streaming(bool sync_stop_state)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_sink_srv_cap_am_disable_waiting_list();
#endif
    bool success = bt_sink_srv_cap_stream_stop_broadcast_reception();
    APPS_LOG_MSGID_I(LOG_TAG" stop_streaming, success=%d sync_stop_state=%d", 2, success, sync_stop_state);
    if (success) {
        app_bis_ctx.stopping_streaming_flag = TRUE;
#ifdef MTK_AWS_MCE_ENABLE
        if (!sync_stop_state) {
            app_bis_temp_cancel_sync = TRUE;
        }
#endif
    }
    return success;
}

void app_le_audio_forward_scan_action(void)
{
    uint16_t *key_action = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
    if (key_action != NULL) {
        *key_action = KEY_LE_AUDIO_BIS_SCAN;
        ui_shell_status_t ret = ui_shell_send_event(FALSE, EVENT_PRIORITY_HIGH,
                                                    EVENT_GROUP_UI_SHELL_KEY,
                                                    INVALID_KEY_EVENT_ID,
                                                    key_action, sizeof(uint16_t),
                                                    NULL, 0);
        APPS_LOG_MSGID_I(LOG_TAG" send scan_action, ret=%d", 1, ret);
    } else {
        APPS_LOG_MSGID_I(LOG_TAG" send scan_action, malloc fail", 0);
    }
}

void app_le_audio_bis_proc_ui_shell_event(struct _ui_shell_activity *self,
                                          uint32_t event_group, uint32_t event_id,
                                          void *extra_data, size_t data_len)
{
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            app_le_audio_bis_proc_bt_cm_group(event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT: {
            app_le_audio_bis_proc_bt_group(event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_KEY: {
            app_le_audio_bis_proc_key_group(event_id, extra_data, data_len);
            break;
        }
#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            app_le_audio_bis_proc_aws_data_group(event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_LE_AUDIO: {
            app_le_audio_bis_proc_le_audio_group(self, event_id, extra_data, data_len);
            break;
        }
        default:
            break;
    }
}

#endif  /* AIR_LE_AUDIO_BIS_ENABLE */
