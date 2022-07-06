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
 * File: app_ull_idle_activity.c
 *
 * Description: This file is the activity to handle ultra low latency state.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */

#include "app_ull_idle_activity.h"
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
#include "app_rho_idle_activity.h"
#endif
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_events_key_event.h"
#include "apps_events_bt_event.h"
#include "apps_config_event_list.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_key_remapper.h"
#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_customer_config.h"
#include "app_bt_state_service.h"
#include "multi_ble_adv_manager.h"
#include "bt_app_common.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#include "bt_device_manager.h"
#include "bt_device_manager_power.h"
#include "bt_ull_service.h"
#include "nvkey_id_list.h"
#ifdef __BT_FAST_PAIR_ENABLE__
#include "app_fast_pair.h"
#endif
#if defined(MTK_FOTA_ENABLE) && defined (MTK_FOTA_VIA_RACE_CMD)
#include "race_event.h"
#endif
#include "nvkey.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "apps_aws_sync_event.h"
#include "bt_aws_mce_srv.h"
#endif
#include "apps_debug.h"

#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
#include "app_dongle_service.h"
#endif
#define LOG_TAG "[ULL_activity]"

/* GAME_MAX_LEVEL --------- BALANCED_LEVEL --------- CHAT_MAX_LEVEL */
#define ULL_MIX_RATIO_GAME_MAX_LEVEL    (0)     /* Gaming is 100%, Chat is 0% */
#define ULL_MIX_RATIO_CHAT_MAX_LEVEL    (20)    /* Gaming is 0%, Chat is 100% */
#define ULL_MIX_RATIO_BALANCED_LEVEL    (10)    /* Gaming is 100%, Chat is 100% */

/* The reconnect timeout, after the time, device will reconnect both SP and dongle. */
#define ULL_SWITCH_CONNECT_TIME         (10 * 1000)

#define ULL_IS_ADDRESS_EMPTY(addr)      (0 == memcmp(addr, s_empty_address, sizeof(bt_bd_addr_t)))
#define ULL_IS_DONGLE_ADDRESS(addr)     (0 == memcmp(addr, s_ull_context.dongle_bt_address, sizeof(bt_bd_addr_t)))
#define ULL_IS_LOCAL_ADDRESS(addr)      (0 == memcmp(addr, *bt_device_manager_get_local_address(), sizeof(bt_bd_addr_t)))
#define ULL_IS_ADDRSS_THE_SAME(addr1, addr2)    (0 == memcmp(addr1, addr2, sizeof(bt_bd_addr_t)))

typedef struct {
    uint8_t link_mode;
#ifdef AIR_APP_ULL_GAMING_MODE_UI_ENABLE
    uint8_t game_mode;
#endif
} app_ull_nvdm_config_data_t;

enum {
    ULL_LINK_MODE_SINGLE,
    ULL_LINK_MODE_MULTIPLE
};

typedef enum {
    ULL_EVENTS_DONGLE_CONNECTED,
    ULL_EVENTS_DONGLE_DISCONNECTED,
    ULL_EVENTS_SP_CONNECTED,
    ULL_EVENTS_SP_DISCONNECTED,
    ULL_EVENTS_AUX_IN,
    ULL_EVENTS_AUX_OUT,
    ULL_EVENTS_USB_AUDIO_IN,
    ULL_EVENTS_USB_AUDIO_OUT,
    ULL_EVENTS_SWITCH_LINK_MODE,
    ULL_EVENTS_SWITCH_GAME_MODE,
} ull_ui_events_t;

static app_ull_context_t s_ull_context;

const static bt_bd_addr_t const s_empty_address = { 0, 0, 0, 0, 0, 0};

typedef enum {
    ULL_SINGLE_LINK_DISCONNECTING_NONE = 0,
    ULL_SINGLE_LINK_DISCONNECTING_DONGLE = 1,
    ULL_SINGLE_LINK_DISCONNECTING_SP = 2,
} ull_single_link_disconnecting_t;

static uint8_t s_single_link_disconnecting = ULL_SINGLE_LINK_DISCONNECTING_NONE;    /* Sometimes reconnect 2 devices, remember the disconnecting device and do not disconnect other SRC when this address connected. */

#ifdef MTK_AWS_MCE_ENABLE
static bool s_need_resync_ull_addr;
static bool s_ull_link_mode_synced;
static bool s_dongle_connected;     /* For partner record the ULL dongle connected status. */
#endif

#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
static bool s_reboot_trigger_reconnect = false;
#endif

static bool s_uplink_started = false;

#ifdef MTK_AWS_MCE_ENABLE
static void app_ull_partner_set_a2dp_enable(void)
{
#if ((!defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || defined (AIR_APP_ULL_GAMING_MODE_UI_ENABLE)) && !defined(AIR_ONLY_DONGLE_MODE_ENABLE)
    if (s_dongle_connected && ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode
#if defined (AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
            && s_ull_context.game_mode
#endif
    ) {
        bt_a2dp_enable_service_record(false);
        bt_avrcp_disable_sdp(true);
    } else {
        bt_a2dp_enable_service_record(true);
        bt_avrcp_disable_sdp(false);
    }
#endif
}
#endif

static void app_ull_store_dongle_address(bool from_air_pairing, bt_bd_addr_t *addr)
{
    memcpy(s_ull_context.dongle_bt_address, *addr, sizeof(bt_bd_addr_t));
    APPS_LOG_MSGID_I(LOG_TAG", dongle address is %02X:%02X:%02X:%02X:%02X:%02X", 6,
        s_ull_context.dongle_bt_address[0], s_ull_context.dongle_bt_address[1],
        s_ull_context.dongle_bt_address[2], s_ull_context.dongle_bt_address[3],
        s_ull_context.dongle_bt_address[4], s_ull_context.dongle_bt_address[5]);
    nvkey_write_data(NVKEYID_BT_ULL_PEER_ADDRESS, s_ull_context.dongle_bt_address, sizeof(s_ull_context.dongle_bt_address));
#ifdef MTK_AWS_MCE_ENABLE
    bt_status_t bt_status = BT_STATUS_PENDING;
    if (app_bt_connection_service_get_current_status()->aws_connected) {
        bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_BT_ULTRA_LOW_LATENCY, BT_ULL_EVENT_PAIRING_COMPLETE_IND,
                                                s_ull_context.dongle_bt_address, sizeof(bt_bd_addr_t));
    }
    /* Because the sending may fail when air pairing completed, need always resync when AWS connected. */
    if (from_air_pairing || bt_status != BT_STATUS_SUCCESS) {
        s_need_resync_ull_addr = true;
    } else {
        s_need_resync_ull_addr = false;
    }
    APPS_LOG_MSGID_I(LOG_TAG", sync ULL address ret = %x", 1, s_need_resync_ull_addr);
#endif
}

static bt_status_t app_ull_connect_correct_profile(bt_bd_addr_t *addr)
{
    bt_cm_connect_t param;
    bt_status_t ret;
    if (!addr || 0 == memcmp(addr, s_empty_address, sizeof(bt_bd_addr_t))) {
        return BT_STATUS_FAIL;
    }
    if (0 == memcmp(addr, s_ull_context.dongle_bt_address, sizeof(bt_bd_addr_t))) {
        param.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL);
    } else {
        param.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
#ifdef MTK_IAP2_PROFILE_ENABLE
                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2)
#endif
                        ;
    }
    memcpy(param.address, addr, sizeof(bt_bd_addr_t));
    ret = bt_cm_connect(&param);
    APPS_LOG_MSGID_I(LOG_TAG", app_ull_connect_correct_profile() profile %x, ret = %x", 2, param.profile, ret);
    return ret;
}

#if 0
static bt_status_t app_ull_single_reconnect(bt_bd_addr_t *addr)
{
    bool already_reconnect = false;
    bt_status_t sta;
    bt_bd_addr_t reconnecting_addr[2];
    uint32_t reconnecting_addr_len = 2;
    bt_cm_connect_t param;
    uint32_t i;

    if (!addr || 0 == memcmp(*addr, s_empty_address, sizeof(bt_bd_addr_t))) {
        APPS_LOG_MSGID_I(LOG_TAG", app_ull_single_reconnect() addr is empty:0x%X.", 1, addr);
        return BT_STATUS_UNSUPPORTED;
    }

    if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
        APPS_LOG_MSGID_I(LOG_TAG ", Do nothing for single reconnect when BT OFF", 0);
        return BT_STATUS_UNSUPPORTED;
    }

    reconnecting_addr_len = bt_cm_get_connecting_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS),
                                                        reconnecting_addr, reconnecting_addr_len);
    /* Cancel other connecting device. */
    param.profile = ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
    for (i = 0; i < reconnecting_addr_len; i++) {
        if (0 != memcmp(*addr, reconnecting_addr[i], sizeof(bt_bd_addr_t))) {
            memcpy(param.address, reconnecting_addr[i], sizeof(bt_bd_addr_t));
            sta = bt_cm_disconnect(&param);
            APPS_LOG_MSGID_I(LOG_TAG", app_ull_single_reconnect() disconnect before reconnect, result:%x.", 1, sta);
        } else {
            already_reconnect = true;
        }
    }

    if (!already_reconnect) {
        sta = app_ull_connect_correct_profile(addr);
    } else {
        APPS_LOG_MSGID_I(LOG_TAG", app_ull_single_reconnect() reconnect already reconnecting.", 0);
        sta = BT_STATUS_SUCCESS;
    }

    return sta;
}
#endif

static void app_ull_get_connected_devices_list(bt_bd_addr_t *conn_sp_list, uint32_t *conn_sp_count, bool *dongle_connected)
{
    bt_bd_addr_t connected_address[3];
    uint32_t connected_num = 3;
    uint32_t i;
    uint32_t sp_count = 0;
    if (dongle_connected) {
        *dongle_connected = false;
    }
    if (conn_sp_list && conn_sp_count && *conn_sp_count > 0) {
        memset(conn_sp_list, 0, (*conn_sp_count) * sizeof(bt_bd_addr_t));
    }
    connected_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE,
                                                connected_address, connected_num);
    for (i = 0; i < connected_num; i++) {
        if (ULL_IS_DONGLE_ADDRESS(connected_address[i])) {
            if (dongle_connected) {
                *dongle_connected = true;
            }
        } else if (!ULL_IS_LOCAL_ADDRESS(connected_address[i])) {
            if (conn_sp_list && conn_sp_count && *conn_sp_count > 0 && sp_count < *conn_sp_count) {
                memcpy(conn_sp_list[sp_count], connected_address[i], sizeof(bt_bd_addr_t));
                sp_count++;
            }
        }
    }
    if (conn_sp_list && conn_sp_count && *conn_sp_count > 0) {
        *conn_sp_count = sp_count;
        APPS_LOG_MSGID_I(LOG_TAG", app_ull_get_connected_devices_list(), search out %d connect SP.", 1, sp_count);
    }
}

static void app_ull_get_connecting_devices_list(bt_bd_addr_t *conn_sp_list, uint32_t *conn_sp_count, bool *dongle_connecting)
{
    bt_bd_addr_t connecting_address[3];
    uint32_t connecting_num = 3;
    uint32_t i;
    uint32_t sp_count = 0;

    if (dongle_connecting) {
        *dongle_connecting = false;
    }
    if (conn_sp_list && conn_sp_count && *conn_sp_count > 0) {
        memset(conn_sp_list, 0, (*conn_sp_count) * sizeof(bt_bd_addr_t));
    }

    connecting_num = bt_cm_get_connecting_devices(BT_CM_PROFILE_SERVICE_MASK_NONE,
                                                  connecting_address, connecting_num);

    for (i = 0; i < connecting_num; i++) {
        if (ULL_IS_DONGLE_ADDRESS(connecting_address[i])) {
            if (dongle_connecting) {
                *dongle_connecting = true;
            }
        } else if (!ULL_IS_LOCAL_ADDRESS(connecting_address[i])) {
            if (conn_sp_list && conn_sp_count && sp_count < *conn_sp_count) {
                memcpy(conn_sp_list[sp_count], connecting_address[i], sizeof(bt_bd_addr_t));
                sp_count++;
            }
        }
    }
    if (conn_sp_list && conn_sp_count && *conn_sp_count > 0) {
        *conn_sp_count = sp_count;
        APPS_LOG_MSGID_I(LOG_TAG", app_ull_get_connecting_devices_list(), search out %d connect SP.", 1, sp_count);
    }
}

static void app_ull_get_connected_connecting_devices_list(bt_bd_addr_t *conn_sp_list, uint32_t *conn_sp_count, bool *dongle_connected)
{
    bt_bd_addr_t connecting_address[2];
    uint32_t connecting_num = 3;
    uint32_t i;
    uint32_t j;
    uint32_t sp_count = 0;

    if (conn_sp_count) {
        sp_count = *conn_sp_count;
    }

    connecting_num = bt_cm_get_connecting_devices(BT_CM_PROFILE_SERVICE_MASK_NONE,
                                                  connecting_address, connecting_num);
    app_ull_get_connected_devices_list(conn_sp_list, &sp_count, dongle_connected);

    for (i = 0; i < connecting_num; i++) {
        if (ULL_IS_DONGLE_ADDRESS(connecting_address[i])) {
            if (dongle_connected) {
                *dongle_connected = true;
            }
        } else if (!ULL_IS_LOCAL_ADDRESS(connecting_address[i])) {
            bool duplicated_sp;
            if (conn_sp_list && conn_sp_count && *conn_sp_count > 0 && sp_count < *conn_sp_count) {
                duplicated_sp = false;
                for (j = 0; j < sp_count; j++) {
                    if (ULL_IS_ADDRSS_THE_SAME(connecting_address[i], conn_sp_list[j])) {
                        duplicated_sp = true;
                        APPS_LOG_MSGID_I(LOG_TAG", duplicate sp in connecting list", 0);
                        break;
                    }
                }
                if (!duplicated_sp) {
                    memcpy(conn_sp_list[sp_count], connecting_address[i], sizeof(bt_bd_addr_t));
                    sp_count++;
                }
            }
        }
    }
    if (conn_sp_list && conn_sp_count && *conn_sp_count > 0) {
        *conn_sp_count = sp_count;
        APPS_LOG_MSGID_I(LOG_TAG", app_ull_get_connected_connecting_devices_list(), search out %d connect SP.", 1, sp_count);
    }
}

static void app_ull_process_events(ull_ui_events_t event, bt_bd_addr_t *current_addr)
{
    bt_cm_connect_t cm_param;
    bt_bd_addr_t connect_sp[2];
    uint32_t connect_num = 2;
    bool dongle_connected;
    uint32_t i;

    switch (event) {
        case ULL_EVENTS_DONGLE_CONNECTED: {
            bt_bd_addr_t *connected_sp = NULL;
            bt_bd_addr_t connecting_sp[2];
            uint32_t connecting_num = 2;
            app_ull_get_connecting_devices_list(connecting_sp, &connecting_num, NULL);
            app_ull_get_connected_devices_list(connect_sp, &connect_num, NULL);
#if ((!defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)) && !defined(AIR_ONLY_DONGLE_MODE_ENABLE)
            if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode
#ifdef AIR_APP_ULL_GAMING_MODE_UI_ENABLE
                && s_ull_context.game_mode
#endif
            ) {
                bt_a2dp_enable_service_record(false);
                bt_avrcp_disable_sdp(true);
            }
#endif
#ifndef AIR_ONLY_DONGLE_MODE_ENABLE
            /* Common request, when ULL connected, disable BLE and disconnect smart phone profiles. */
            if (!s_ull_context.adv_paused) {
                multi_ble_adv_manager_pause_ble_adv();
                s_ull_context.adv_paused = true;
            }
            multi_ble_adv_manager_disconnect_ble(NULL);
#endif
#ifdef MTK_AWS_MCE_ENABLE
            bool connected = true;
            apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_ULL_DONGLE_CONNECTION_CHANGE,
                    &connected, sizeof(connected));
#endif
            /* When dongle connected,
             * if single mode, disconnect all other device.
             * if multi mode, keep HPF only.
             */
            for (i = 0; i < connect_num; i++) {
                if (ULL_LINK_MODE_SINGLE == s_ull_context.link_mode) {
                    if (s_single_link_disconnecting != ULL_SINGLE_LINK_DISCONNECTING_DONGLE) {
                        /* Disconnect SP. */
                        s_single_link_disconnecting = ULL_SINGLE_LINK_DISCONNECTING_SP;
                        cm_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                        APPS_LOG_MSGID_I(LOG_TAG ", Disconnect SP acl link.", 0);
                    } else {
                        /* This case should be actively connect both SRC successfully at the same time, so do not disconnect SP when disconnecting dongle.*/
                        APPS_LOG_MSGID_I(LOG_TAG ", don't disconnect SP because SP is connected when Dongle is disconnecting.", 0);
                        continue;
                    }
                } else {
                    if (i == 0) {
                        /* Multi link mode support the first SP connect HFP. */
                        connected_sp = &connect_sp[i];
                        cm_param.profile = ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                                            |BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS));
#if (defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                        if (!s_ull_context.game_mode)
#endif
                        {
                            cm_param.profile &= ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
                                                 | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP));
                        }
#endif
                    } else {
                        cm_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                    }
                }
                memcpy(cm_param.address, connect_sp[i], sizeof(bt_bd_addr_t));
                bt_cm_disconnect(&cm_param);
            }
            /* Because not support paging when already connected devices, cancel all other connection. */
            for (i = 0; i < connecting_num; i++) {
                if (connected_sp == NULL || !ULL_IS_ADDRSS_THE_SAME(connecting_sp[i], *connected_sp)) {
                    APPS_LOG_MSGID_I(LOG_TAG ", Disconnect SP acl link, cancel reconnect.", 0);
                    cm_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                    memcpy(cm_param.address, connecting_sp[i], sizeof(bt_bd_addr_t));
                    bt_status_t bt_status = bt_cm_disconnect(&cm_param);
                    if (bt_status == BT_STATUS_SUCCESS) {
                        if (ULL_LINK_MODE_SINGLE == s_ull_context.link_mode) {
                            s_single_link_disconnecting = ULL_SINGLE_LINK_DISCONNECTING_SP;
                        }
                    }
                }
            }

            /* Set latency. */
            if (ULL_LINK_MODE_SINGLE == s_ull_context.link_mode) {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, APPS_ULL_STREAMING_RETRY_COUNT_FOR_SINGLE_LINK);
            } else if (NULL != connected_sp) {
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                if (s_ull_context.game_mode) {
                    bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK);
                } else {
                    bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK_A2DP);
                }
#else
#ifdef AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK_A2DP);
#else
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK);
#endif
#endif
            } else {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_CONNECTING);
            }
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_ULL_RECONNECT_TIMEOUT);
            break;
        }
        case ULL_EVENTS_DONGLE_DISCONNECTED: {
#if ((!defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)) && !defined(AIR_ONLY_DONGLE_MODE_ENABLE)
            bt_a2dp_enable_service_record(true);
            bt_avrcp_disable_sdp(false);
#endif
#ifndef AIR_ONLY_DONGLE_MODE_ENABLE
            if (s_ull_context.adv_paused) {
                multi_ble_adv_manager_resume_ble_adv();
                s_ull_context.adv_paused = false;
            }
            /* To avoid smart phone reconnet A2DP when BT OFF -> ON. */
            cm_param.profile = 0
#if (!defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                    | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
                    | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2)
#endif
                    ;
            if (cm_param.profile != BT_CM_PROFILE_SERVICE_MASK_NONE
                && APP_BT_STATE_POWER_STATE_NONE_ACTION == app_bt_connection_service_get_current_status()->target_power_state
                && APP_BT_STATE_POWER_STATE_ENABLED == app_bt_connection_service_get_current_status()->current_power_state) {
                app_ull_get_connected_connecting_devices_list(connect_sp, &connect_num, NULL);
                for (i = 0; i < connect_num; i++) {
                    memcpy(cm_param.address, connect_sp[i], sizeof(bt_bd_addr_t));
                    bt_cm_connect(&cm_param);
                }
            }
#endif /* #ifndef AIR_ONLY_DONGLE_MODE_ENABLE */
#ifdef MTK_AWS_MCE_ENABLE
            bool connected = false;
            apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                    APPS_EVENTS_INTERACTION_ULL_DONGLE_CONNECTION_CHANGE,
                    &connected, sizeof(connected));
#endif
            break;
        }
        case ULL_EVENTS_SP_CONNECTED: {
            bool dongle_connecting = false;
            app_ull_get_connected_connecting_devices_list(connect_sp, &connect_num, &dongle_connecting);
            app_ull_get_connected_devices_list(NULL, NULL, &dongle_connected);
            if (ULL_LINK_MODE_SINGLE == s_ull_context.link_mode || !dongle_connected) {
                if (!ULL_IS_ADDRESS_EMPTY(s_ull_context.dongle_bt_address)) {
                    if (dongle_connected && s_single_link_disconnecting == ULL_SINGLE_LINK_DISCONNECTING_SP) {
                        APPS_LOG_MSGID_I(LOG_TAG ", don't disconnect dongle because SP is disconnecting.", 0);
                    } else if (dongle_connecting || dongle_connected) {
                        cm_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                        memcpy(cm_param.address, s_ull_context.dongle_bt_address, sizeof(bt_bd_addr_t));
                        APPS_LOG_MSGID_I(LOG_TAG ", disconnect Dongle, dongle connect: %d", 1, dongle_connected);
                        if (bt_cm_disconnect(&cm_param) == BT_STATUS_SUCCESS) {
                            s_single_link_disconnecting = ULL_SINGLE_LINK_DISCONNECTING_DONGLE;
                        }
                    }
                }
            }

            if (ULL_LINK_MODE_SINGLE == s_ull_context.link_mode) {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, APPS_ULL_STREAMING_RETRY_COUNT_FOR_SINGLE_LINK);
            } else {
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                if (s_ull_context.game_mode) {
                    bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK);
                } else {
                    bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK_A2DP);
                }
#else
#ifdef AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK_A2DP);
#else
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK);
#endif
#endif
            }
            for (i = 0; i < connect_num; i++) {
                /* take over other SP */
                if (!ULL_IS_ADDRSS_THE_SAME(connect_sp[i], *current_addr)) {
                    cm_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                    memcpy(cm_param.address, connect_sp[i], sizeof(bt_bd_addr_t));
                    APPS_LOG_MSGID_I(LOG_TAG ", takeover SP, link_mode:%d", 1, s_ull_context.link_mode);
                    bt_cm_disconnect(&cm_param);
                }
            }
            ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_ULL_RECONNECT_TIMEOUT);
            break;
        }
        case ULL_EVENTS_SP_DISCONNECTED: {
            if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode) {
#if defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE
                if (!s_ull_context.game_mode)
#endif
                {
                    bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_CONNECTING);
                }
                if (APP_BT_STATE_POWER_STATE_NONE_ACTION == app_bt_connection_service_get_current_status()->target_power_state
                    && APP_BT_STATE_POWER_STATE_ENABLED == app_bt_connection_service_get_current_status()->current_power_state
                    && ULL_AUX_USB_AUDIO_STATE_IN != s_ull_context.aux_state && ULL_AUX_USB_AUDIO_STATE_IN != s_ull_context.usb_audio_state) {
                    /* Try to reconnect dongle when SP disconnected in multi link mode. */
                    app_ull_get_connected_connecting_devices_list(connect_sp, &connect_num, &dongle_connected);
                    if (!dongle_connected) {
                        APPS_LOG_MSGID_I(LOG_TAG ", Reconnect the dongle when SP disconnected.", 0);
                        app_ull_connect_correct_profile(&s_ull_context.dongle_bt_address);
                    } else {
#if (!defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
#if defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE
                        if (s_ull_context.game_mode)
#endif
                        {
                            cm_param.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK) | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP);
                            for (i = 0; i < connect_num; i++) {
                                APPS_LOG_MSGID_I(LOG_TAG ", SP disconnected, but Dongle is connected, need cancel reconnect A2DP", 0);
                                memcpy(cm_param.address, connect_sp[i], sizeof(bt_bd_addr_t));
                                bt_cm_disconnect(&cm_param);
                            }
                        }
#endif
                    }
                }
            }
            break;
        }
#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT)
        case ULL_EVENTS_AUX_IN:
        case ULL_EVENTS_USB_AUDIO_IN: {
            if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
                APPS_LOG_MSGID_I(LOG_TAG ", Do nothing for AUX_IN when BT OFF", 0);
                break;
            }
            /* Disconnect dongle for single and multi link, connect A2DP for multi link */
            cm_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
            memcpy(cm_param.address, s_ull_context.dongle_bt_address, sizeof(bt_bd_addr_t));
            bt_cm_disconnect(&cm_param);
            if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode) {
                cm_param.profile = 0;
#if ((!defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)) && !defined(AIR_ONLY_DONGLE_MODE_ENABLE)
                cm_param.profile |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK);
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
                cm_param.profile |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2);
#endif
                if (BT_CM_PROFILE_SERVICE_MASK_NONE != cm_param.profile) {
                    app_ull_get_connected_connecting_devices_list(connect_sp, &connect_num, NULL);
                    if (connect_num > 0) {
                        /* Connect A2DP. */
                        memcpy(cm_param.address, connect_sp[0], sizeof(bt_bd_addr_t));
                        APPS_LOG_MSGID_I(LOG_TAG ", connect a2dp of connected SP", 0);
                        bt_cm_connect(&cm_param);
                        break;
                    }
                }
            }
            break;
        }
        case ULL_EVENTS_AUX_OUT:
        case ULL_EVENTS_USB_AUDIO_OUT: {
            if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
                APPS_LOG_MSGID_I(LOG_TAG ", Do nothing for AUX_OUT when BT OFF", 0);
                break;
            }
            if (ULL_AUX_USB_AUDIO_STATE_IN == s_ull_context.aux_state ||
                ULL_AUX_USB_AUDIO_STATE_IN == s_ull_context.usb_audio_state) {
                APPS_LOG_MSGID_I(LOG_TAG ", still have wired connected: aux:%d, usb:%d", 2,
                    s_ull_context.aux_state, s_ull_context.usb_audio_state);
                break;
            }
            /* Reconnect dongle if smart phone not connected. */
            app_ull_get_connected_devices_list(connect_sp, &connect_num, &dongle_connected);
            if (!dongle_connected) {
                if (0 == connect_num) {
                    APPS_LOG_MSGID_I(LOG_TAG ", Reconnect Dongle when Line out", 0);
                    app_ull_connect_correct_profile(&s_ull_context.dongle_bt_address);
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG ", Cannot reconnect when line out", 0);
                }
            }
            break;
        }
#endif
#if !defined(AIR_ONLY_DONGLE_MODE_ENABLE)
#if !(defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
        case ULL_EVENTS_SWITCH_LINK_MODE: {
            if (BT_DEVICE_MANAGER_POWER_STATE_ACTIVE != bt_device_manager_power_get_power_state(BT_DEVICE_TYPE_CLASSIC)) {
                APPS_LOG_MSGID_I(LOG_TAG ", Do nothing for SWITCH_LINK_MODE when BT OFF", 0);
                break;
            }
            APPS_LOG_MSGID_I(LOG_TAG ", link_mode switch from %d", 1, s_ull_context.link_mode);
            if (ULL_LINK_MODE_SINGLE == s_ull_context.link_mode) {
                s_ull_context.link_mode = ULL_LINK_MODE_MULTIPLE;
            } else {
                s_ull_context.link_mode = ULL_LINK_MODE_SINGLE;
            }
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                /* Notify partner mode changed. */
                app_ull_nvdm_config_data_t config_data = {
                    .link_mode = s_ull_context.link_mode,
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                    .game_mode = s_ull_context.game_mode,
#endif
                };
                bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_ULL_LINK_MODE_CHANGE,
                                                                       &(config_data), sizeof(config_data));
                APPS_LOG_MSGID_I(LOG_TAG ", Send ull link mode to partner:%d, ret = %x", 2, s_ull_context.link_mode, bt_status);
                if (BT_STATUS_SUCCESS != bt_status) {
                    s_ull_link_mode_synced = false;
                } else {
                    s_ull_link_mode_synced = true;
                }
            }
#endif
            app_ull_get_connected_devices_list(connect_sp, &connect_num, &dongle_connected);
            if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode) {
                s_ull_context.link_mode = ULL_LINK_MODE_MULTIPLE;
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_CONNECTING);
#if !defined(AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) && !defined(AIR_ONLY_DONGLE_MODE_ENABLE)
                if (dongle_connected) {
                    bt_a2dp_enable_service_record(false);
                    bt_avrcp_disable_sdp(true);
                }
#endif
                if (0 == connect_num && !dongle_connected) {
                    /* If not connected any SRC, try to connect SP and dongle. */
                    bt_bd_addr_t connecting_sp[2];
                    uint32_t connecting_num = 2;
                    app_ull_get_connected_connecting_devices_list(connecting_sp, &connecting_num, &dongle_connected);
                    if (!dongle_connected && !ULL_IS_ADDRESS_EMPTY(s_ull_context.dongle_bt_address)) {
                        app_ull_connect_correct_profile(&s_ull_context.dongle_bt_address);
                    }
                    if (connecting_num == 0) {
                        bt_device_manager_paired_infomation_t paired_info[2];
                        uint32_t paired_info_count = 2;
                        bt_device_manager_get_paired_list(paired_info, &paired_info_count);
                        for (i = 0; i < paired_info_count; i++) {
                            if (!ULL_IS_DONGLE_ADDRESS(paired_info[i].address)) {
                                APPS_LOG_MSGID_I(LOG_TAG ", Reconnect the first SP", 0);
                                app_ull_connect_correct_profile(&paired_info[i].address);
                                break;
                            }
                        }
                    }
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG ", already connected sp:%d, dongle:%d", 2, connect_num, dongle_connected);
                }
            } else {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, APPS_ULL_STREAMING_RETRY_COUNT_FOR_SINGLE_LINK);
                s_ull_context.link_mode = ULL_LINK_MODE_SINGLE;
                cm_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                if (!dongle_connected && connect_num > 0) {
                    /* When sp connected and dongle is not connected, disconnect dongle */
                    memcpy(cm_param.address, s_ull_context.dongle_bt_address, sizeof(bt_bd_addr_t));
                    APPS_LOG_MSGID_I(LOG_TAG ", disconnect Dongle when link mode change to single", 0);
                    bt_cm_disconnect(&cm_param);
                } else if (dongle_connected) {
                    connect_num = sizeof(connect_sp) / sizeof(connect_sp[0]);
                    app_ull_get_connected_connecting_devices_list(connect_sp, &connect_num, NULL);
                    for (i = 0; i < connect_num; i++) {
                        memcpy(cm_param.address, connect_sp[i], sizeof(bt_bd_addr_t));
                        APPS_LOG_MSGID_I(LOG_TAG ", disconnect SP when link mode change to single", 0);
                        bt_cm_disconnect(&cm_param);
                    }
                } else {
                    APPS_LOG_MSGID_I(LOG_TAG ", Both not connected when link mode change to single", 0);
                }
            }
            app_ull_nvdm_config_data_t config_data = {
                .link_mode = s_ull_context.link_mode,
            };
            nvkey_write_data(NVKEYID_APP_ULL_CONFIG, (uint8_t *)&config_data, sizeof(config_data));
            break;
        }
#else /* #if !(defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE) */
        case ULL_EVENTS_SWITCH_GAME_MODE: {
            bt_bd_addr_t connecting_sp[2];
            uint32_t connecting_num = 2;
            app_ull_get_connecting_devices_list(connecting_sp, &connecting_num, NULL);
            app_ull_get_connected_devices_list(connect_sp, &connect_num, &dongle_connected);
            if (dongle_connected) {
                s_ull_context.game_mode = !s_ull_context.game_mode;
                if (s_ull_context.game_mode) {
                    bt_a2dp_enable_service_record(false);
                    bt_avrcp_disable_sdp(true);
                    cm_param.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
                                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP);
                    for (i = 0; i < connect_num; i++) {
                        memcpy(cm_param.address, connect_sp[i], sizeof(bt_bd_addr_t));
                        APPS_LOG_MSGID_I(LOG_TAG ", disconnect A2DP when game mode on for %d", 1, i);
                        bt_cm_disconnect(&cm_param);
                    }
                    if (connect_num == 0) {
                        for (i = 0; i < connecting_num; i++) {
                            memcpy(cm_param.address, connecting_sp[i], sizeof(bt_bd_addr_t));
                            APPS_LOG_MSGID_I(LOG_TAG ", disconnect A2DP for conneting SP when game mode on for %d", 1, i);
                            bt_cm_disconnect(&cm_param);
                        }
                    }
                    if (connect_num > 0) {
                        bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK);
                    } else {
                        bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, APPS_ULL_STREAMING_RETRY_COUNT_FOR_SINGLE_LINK);
                    }
                } else {
                    bt_a2dp_enable_service_record(true);
                    bt_avrcp_disable_sdp(false);
                    cm_param.profile = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
                                        | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AVRCP);
                    if (connect_num > 0) {
                        memcpy(cm_param.address, connect_sp[0], sizeof(bt_bd_addr_t));
                        APPS_LOG_MSGID_I(LOG_TAG ", connect A2DP when game mode off", 0);
                        bt_cm_connect(&cm_param);
                    }
                    if (connect_num == 0) {
                        for (i = 0; i < connecting_num; i++) {
                            memcpy(cm_param.address, connecting_sp[i], sizeof(bt_bd_addr_t));
                            APPS_LOG_MSGID_I(LOG_TAG ", connect A2DP for conneting SP when game mode off for %d", 1, i);
                            bt_cm_connect(&cm_param);
                        }
                    }
                    if (connect_num > 0) {
                        bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK_A2DP);
                    } else {
                        bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_CONNECTING);
                    }
                }
                apps_config_set_vp(VP_INDEX_DOUBLE, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                app_ull_nvdm_config_data_t config_data = {
                    .link_mode = s_ull_context.link_mode,
                    .game_mode = s_ull_context.game_mode,
                };
#ifdef MTK_AWS_MCE_ENABLE
                if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_AGENT) {
                    /* Notify partner mode changed. */
                    app_ull_nvdm_config_data_t config_data = {
                        .link_mode = s_ull_context.link_mode,
                        .game_mode = s_ull_context.game_mode,
                    };
                    bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_ULL_LINK_MODE_CHANGE,
                                                                        &(config_data), sizeof(config_data));
                    APPS_LOG_MSGID_I(LOG_TAG ", Send ull game mode to partner:%d, ret = %x", 2, s_ull_context.game_mode, bt_status);
                    if (BT_STATUS_SUCCESS != bt_status) {
                        s_ull_link_mode_synced = false;
                    } else {
                        s_ull_link_mode_synced = true;
                    }
                }
#endif
                nvkey_write_data(NVKEYID_APP_ULL_CONFIG, (uint8_t *)&config_data, sizeof(config_data));
            } else {
                apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            break;
        }
#endif /* #if !(defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE) */
#endif /* #if !defined(AIR_ONLY_DONGLE_MODE_ENABLE) */
        default:
            break;
    }
}

#if !defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
static void app_ull_reconnect_request_process(void)
{
    bt_bd_addr_t addr_list[2];
    uint32_t addr_list_len = 2;
    bt_bd_addr_t connecting_list[2];
    uint32_t connecting_list_len = 2;
    bool ull_connected = false;
    bool ull_connecting = false;
    bt_status_t sta = BT_STATUS_FAIL;
    bt_cm_connect_t param;
    uint32_t i;

    if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode) {
        APPS_LOG_MSGID_I(LOG_TAG", app_ull_reconnect_request_process multi mode not need reconnect", 0);
        return;
    }

    app_ull_get_connected_devices_list(addr_list, &addr_list_len, &ull_connected);
    app_ull_get_connecting_devices_list(connecting_list, &connecting_list_len, &ull_connecting);

    APPS_LOG_MSGID_I(LOG_TAG", app_ull_reconnect_request_process connected[sp:dongle][%d:%d], connecting[sp:dongle][%d:%d]", 4,
            addr_list_len, ull_connected, connecting_list_len, ull_connecting);
    if (ull_connected && addr_list_len > 0) {
        APPS_LOG_MSGID_W(LOG_TAG", app_ull_reconnect_request_process, both connected", 0);
        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        return;
    } else if ((addr_list_len > 0 || (!ull_connected && !ull_connecting))
        && ULL_AUX_USB_AUDIO_STATE_IN != s_ull_context.usb_audio_state
        && ULL_AUX_USB_AUDIO_STATE_IN != s_ull_context.aux_state) {
        /* When SP is connected or ull is not connecting, reconnect dongle. */
        if (!ULL_IS_ADDRESS_EMPTY(s_ull_context.dongle_bt_address)) {
            param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
            /* disconnect all SP. */
            for (i = 0; i < addr_list_len; i++) {
                APPS_LOG_MSGID_I(LOG_TAG ", app_ull_reconnect_request_process, disconnect current[%x:%x:%x:%x:%x;%x]",
                                    6,
                                    addr_list[i][0], addr_list[i][1], addr_list[i][2],
                                    addr_list[i][3], addr_list[i][4], addr_list[i][5]);
                memcpy(param.address, addr_list[i], sizeof(bt_bd_addr_t));
                bt_cm_disconnect(&param);
            }
            for (i = 0; i < connecting_list_len; i++) {
                memcpy(param.address, connecting_list[i], sizeof(bt_bd_addr_t));
                bt_cm_disconnect(&param);
            }
        }
        if (ull_connecting) {
            APPS_LOG_MSGID_I(LOG_TAG", app_ull_reconnect_request_process, Already reconnecting dongle", 0);
            sta = BT_STATUS_SUCCESS;
        } else {
            if (!ULL_IS_ADDRESS_EMPTY(s_ull_context.dongle_bt_address)) {
                /* update time */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_ULL_RECONNECT_TIMEOUT);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_ULL_RECONNECT_TIMEOUT, NULL, 0,
                        NULL, ULL_SWITCH_CONNECT_TIME);
                sta = app_ull_connect_correct_profile(&s_ull_context.dongle_bt_address);
                APPS_LOG_MSGID_I(LOG_TAG", app_ull_reconnect_request_process reconnect, ULL result:%x.", 1, sta);
            } else {
                sta = BT_STATUS_FAIL;
            }
        }
        if (sta == BT_STATUS_SUCCESS) {
            apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        } else {
            apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        }
    } else if (ull_connected || (connecting_list_len == 0 && ull_connecting)) {
        /* Reconnect SP. */
        /* Disconnect dongle. */
        if (!ULL_IS_ADDRESS_EMPTY(s_ull_context.dongle_bt_address)) {
            memcpy(param.address, s_ull_context.dongle_bt_address, sizeof(bt_bd_addr_t));
            param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
            bt_cm_disconnect(&param);
        }
        if (connecting_list_len > 0) {
            APPS_LOG_MSGID_I(LOG_TAG", app_ull_reconnect_request_process, Already reconnecting sp", 0);
            sta = BT_STATUS_SUCCESS;
        } else {
            bt_device_manager_paired_infomation_t pair_info[2];
            uint32_t paired_number = 2;
            bt_bd_addr_t *sp_addr = NULL;
            bt_device_manager_get_paired_list(pair_info, &paired_number);
            for (i = 0; i < paired_number; i++) {
                if (!ULL_IS_DONGLE_ADDRESS(pair_info[i].address)) {
                    sp_addr = &pair_info[i].address;
                    break;
                }
            }
            if (sp_addr != NULL) {
                /* Update time. */
                ui_shell_remove_event(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_ULL_RECONNECT_TIMEOUT);
                ui_shell_send_event(false, EVENT_PRIORITY_HIGNEST, EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                        APPS_EVENTS_INTERACTION_ULL_RECONNECT_TIMEOUT, NULL, 0,
                        NULL, ULL_SWITCH_CONNECT_TIME);
                sta = app_ull_connect_correct_profile(sp_addr);
                APPS_LOG_MSGID_I(LOG_TAG", app_ull_reconnect_request_process result:%x.", 1, sta);
            }
        }
        if (BT_STATUS_SUCCESS == sta) {
            apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        } else {
            apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
        }
    } else if (!ull_connected && addr_list_len == 0 && ull_connecting && connecting_list_len > 0) {
        /* Keep connecting status when both is connecting. */
        APPS_LOG_MSGID_I(LOG_TAG", app_ull_reconnect_request_process both connecting, keep", 0);
        apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
    }
}
#endif

static bool app_ull_proc_ui_shell_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = true;
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            memset(&s_ull_context, 0, sizeof(s_ull_context));
            self->local_context = &s_ull_context;
            app_ull_nvdm_config_data_t config_data;
            bt_bd_addr_t dongle_addr;
            uint32_t nvdm_read_size = sizeof(bt_bd_addr_t);
#ifdef AIR_APP_ULL_GAMING_MODE_UI_ENABLE
            s_ull_context.link_mode = ULL_LINK_MODE_MULTIPLE;
#endif
            if (NVKEY_STATUS_OK == nvkey_read_data(NVKEYID_BT_ULL_PEER_ADDRESS, dongle_addr, &nvdm_read_size)) {
                memcpy(s_ull_context.dongle_bt_address, dongle_addr, sizeof(bt_bd_addr_t));
            }
            nvdm_read_size = sizeof(config_data);
            if (NVKEY_STATUS_OK == nvkey_read_data(NVKEYID_APP_ULL_CONFIG, (uint8_t *)&config_data, &nvdm_read_size)) {
                s_ull_context.link_mode = config_data.link_mode;
#ifdef AIR_APP_ULL_GAMING_MODE_UI_ENABLE
                s_ull_context.game_mode = config_data.game_mode;
#endif
            }
            if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode) {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_CONNECTING);
            } else {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, APPS_ULL_STREAMING_RETRY_COUNT_FOR_SINGLE_LINK);
            }
            bt_app_common_apply_ultra_low_latency_retry_count();
#ifdef AIR_ONLY_DONGLE_MODE_ENABLE
            bt_a2dp_enable_service_record(false);
            bt_avrcp_disable_sdp(true);
            bt_hfp_enable_service_record(false);
            multi_ble_adv_manager_pause_ble_adv();
            s_ull_context.adv_paused = true;
#endif
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
            s_reboot_trigger_reconnect = pmu_get_power_off_reason() == 0x8;
#endif
            APPS_LOG_MSGID_I(LOG_TAG": create, init link mode:%d", 1, s_ull_context.link_mode);
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
            break;
        }
        default:
            ret = false;
            break;
    }
    return ret;
}

#ifdef AIR_ROTARY_ENCODER_ENABLE
static bool app_ull_proc_rotary_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = false;

    bsp_rotary_encoder_port_t port;
    bsp_rotary_encoder_event_t event;
    bt_status_t bt_status;
    uint32_t rotary_data;
    if (!extra_data) {
        return ret;
    }
    apps_config_key_action_t key_action = *(uint16_t *)extra_data;
    app_event_rotary_event_decode(&port, &event, &rotary_data, event_id);
    switch (key_action) {
        case KEY_AUDIO_MIX_RATIO_GAME_ADD:
        case KEY_AUDIO_MIX_RATIO_CHAT_ADD: {
#ifdef AIR_BT_ULTRA_LOW_LATENCY_ENABLE
            if ((app_dongle_service_get_dongle_mode() == APP_DONGLE_SERVICE_DONGLE_MODE_XBOX)
                || (app_dongle_service_get_dongle_mode() == APP_DONGLE_SERVICE_DONGLE_MODE_PC)) {
                APPS_LOG_MSGID_I(LOG_TAG"app_ull_proc_rotary_event_group route to dongle", 0);
                apps_dongle_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, key_action);
                ret = true;
                break;
            }
#endif /* AIR_BT_ULTRA_LOW_LATENCY_ENABLE */
            uint8_t target_level = 0;
            bt_ull_mix_ratio_t mix_ratio;
            mix_ratio.num_streaming = BT_ULL_MAX_STREAMING_NUM;
            mix_ratio.streamings[0].streaming.streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER;
            mix_ratio.streamings[0].streaming.port = 0; /* gaming streaming port */
            mix_ratio.streamings[1].streaming.streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER;
            mix_ratio.streamings[1].streaming.port = 1; /* chat streaming port */
            if (KEY_AUDIO_MIX_RATIO_CHAT_ADD == key_action) {
                if (s_ull_context.current_mix_ratio_level + rotary_data < ULL_MIX_RATIO_CHAT_MAX_LEVEL) {
                    target_level = s_ull_context.current_mix_ratio_level + rotary_data;
                } else {
                    target_level = ULL_MIX_RATIO_CHAT_MAX_LEVEL;
                }
            } else {
                if (s_ull_context.current_mix_ratio_level > ULL_MIX_RATIO_GAME_MAX_LEVEL + rotary_data) {
                    target_level = s_ull_context.current_mix_ratio_level - rotary_data;
                } else {
                    target_level = ULL_MIX_RATIO_GAME_MAX_LEVEL;
                }
            }
            if (target_level != s_ull_context.current_mix_ratio_level) {
                mix_ratio.streamings[0].ratio = (target_level <= ULL_MIX_RATIO_BALANCED_LEVEL) ?
                    100 :
                    100 * (ULL_MIX_RATIO_CHAT_MAX_LEVEL - target_level) / (ULL_MIX_RATIO_CHAT_MAX_LEVEL - ULL_MIX_RATIO_BALANCED_LEVEL);
                mix_ratio.streamings[1].ratio = (target_level >= ULL_MIX_RATIO_BALANCED_LEVEL) ?
                    100 :
                    100 * (target_level - ULL_MIX_RATIO_GAME_MAX_LEVEL) / (ULL_MIX_RATIO_BALANCED_LEVEL - ULL_MIX_RATIO_GAME_MAX_LEVEL);
                bt_status = bt_ull_action(BT_ULL_ACTION_SET_STREAMING_MIX_RATIO, &mix_ratio, sizeof(mix_ratio));
                APPS_LOG_MSGID_I(LOG_TAG": set mix ratio level %d -> %d, status: 0x%x", 3,
                    s_ull_context.current_mix_ratio_level, target_level, bt_status);
                if (BT_STATUS_SUCCESS == bt_status) {
                    s_ull_context.current_mix_ratio_level = target_level;
                }
            }
            ret = true;
            break;
        }
        default:
            break;
    }

    return ret;
}
#endif

static bool app_ull_proc_interaction_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = false;
    switch (event_id) {
#ifdef APPS_LINE_IN_SUPPORT
        case APPS_EVENTS_INTERACTION_LINE_IN_PLUG_STATE: {
            bool plug_in = (bool)extra_data;
            ull_aux_or_usb_audio_in_state_t current_state = plug_in ? ULL_AUX_USB_AUDIO_STATE_IN : ULL_AUX_USB_AUDIO_STATE_OUT;
            if (current_state != s_ull_context.aux_state) {
                APPS_LOG_MSGID_I(LOG_TAG", receive LINE_IN = %d", 1, plug_in);
                if (ULL_AUX_USB_AUDIO_STATE_NONE == s_ull_context.aux_state) {
                    /* BT should have not enabled. */
                    s_ull_context.aux_state = current_state;
                } else {
                    s_ull_context.aux_state = current_state;
                    if (plug_in) {
                        app_ull_process_events(ULL_EVENTS_AUX_IN, NULL);
                    } else {
                        app_ull_process_events(ULL_EVENTS_AUX_OUT, NULL);
                    }
                }
            }
            break;
        }
#endif
#ifdef APPS_USB_AUDIO_SUPPORT
        case APPS_EVENTS_INTERACTION_USB_PLUG_STATE: {
            bool plug_in = (bool)extra_data;
            ull_aux_or_usb_audio_in_state_t current_state = plug_in ? ULL_AUX_USB_AUDIO_STATE_IN : ULL_AUX_USB_AUDIO_STATE_OUT;
            if (current_state != s_ull_context.usb_audio_state) {
                APPS_LOG_MSGID_I(LOG_TAG", receive USB AUDIO CHANGE = %d", 1, plug_in);
                s_ull_context.usb_audio_state = current_state;
                if (plug_in) {
                    app_ull_process_events(ULL_EVENTS_USB_AUDIO_IN, NULL);
                } else {
                    app_ull_process_events(ULL_EVENTS_USB_AUDIO_OUT, NULL);
                }
            }
            break;
        }
#endif
        case APPS_EVENTS_INTERACTION_ULL_RECONNECT_TIMEOUT:
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_PARTNER
                && bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
            {
                /* Reconnect both device. */
                bt_bd_addr_t reconnecting_addr[2];
                uint32_t reconnecting_addr_len = 2;
                bt_bd_addr_t connectied_addr[2];
                uint32_t connected_addr_len = 2;
                bool dongle_connecting = false;
                bool dongle_connected = false;
                uint32_t i;
                bt_status_t sta;
                app_ull_get_connecting_devices_list(reconnecting_addr, &reconnecting_addr_len, &dongle_connecting);
                app_ull_get_connected_devices_list(connectied_addr, &connected_addr_len, &dongle_connected);
                APPS_LOG_MSGID_I(LOG_TAG", ULL_RECONNECT_TIMEOUT = connected[sp:dongle], connecting[sp:dongle][%d:%d]", 4,
                        connected_addr_len > 0, dongle_connected, reconnecting_addr_len > 0, dongle_connecting);
                if (connected_addr_len > 0 || dongle_connected) {
                    /* Already connected, not need reconnect. */
                    break;
                }
                if (!dongle_connecting) {
                    sta = app_ull_connect_correct_profile(&s_ull_context.dongle_bt_address);
                    APPS_LOG_MSGID_I(LOG_TAG", ULL_RECONNECT_TIMEOUT reconnect dongle sta = %d", 1, sta);
                }
                if (reconnecting_addr_len == 0) {
                    bt_device_manager_paired_infomation_t pair_info[2];
                    uint32_t paired_number = 2;
                    bt_bd_addr_t *sp_addr = NULL;
                    bt_device_manager_get_paired_list(pair_info, &paired_number);
                    for (i = 0; i < paired_number; i++) {
                        if (!ULL_IS_DONGLE_ADDRESS(pair_info[i].address)) {
                            sp_addr = &pair_info[i].address;
                            break;
                        }
                    }
                    sta = app_ull_connect_correct_profile(sp_addr);
                    APPS_LOG_MSGID_I(LOG_TAG", ULL_RECONNECT_TIMEOUT reconnect sp sta = %d", 1, sta);
                }
            }
            break;
#ifdef MTK_AWS_MCE_ENABLE
        case APPS_EVENTS_INTERACTION_PARTNER_SWITCH_TO_AGENT: {
            if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode) {
                bt_bd_addr_t connect_sp[1];
                uint32_t connect_num = 1;
                app_ull_get_connected_devices_list(connect_sp, &connect_num, NULL);
                if (connect_num > 0) {
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                    if (s_ull_context.game_mode) {
                        bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK);
                    } else {
                        bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK_A2DP);
                    }
#else
#ifdef AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE
                    bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK_A2DP);
#else
                    bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_MULTI_LINK);
#endif
#endif /* #if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE) */
                } else {
                    bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_CONNECTING);
                }
            } else {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, APPS_ULL_STREAMING_RETRY_COUNT_FOR_SINGLE_LINK);
            }
            break;
        }
#endif
        case APPS_EVENTS_INTERACTION_SET_ULL_MIX_RATIO: {
            bt_status_t bt_status;
            uint8_t* target_level = (uint8_t*)extra_data;
            bt_ull_mix_ratio_t mix_ratio;
            mix_ratio.num_streaming = BT_ULL_MAX_STREAMING_NUM;
            mix_ratio.streamings[0].streaming.streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER;
            mix_ratio.streamings[0].streaming.port = 0; /* gaming streaming port */
            mix_ratio.streamings[1].streaming.streaming_interface = BT_ULL_STREAMING_INTERFACE_SPEAKER;
            mix_ratio.streamings[1].streaming.port = 1; /* chat streaming port */
            if (*target_level != s_ull_context.current_mix_ratio_level) {
                mix_ratio.streamings[0].ratio = (*target_level <= ULL_MIX_RATIO_BALANCED_LEVEL) ?
                    100 :
                    100 * (ULL_MIX_RATIO_CHAT_MAX_LEVEL - *target_level) / (ULL_MIX_RATIO_CHAT_MAX_LEVEL - ULL_MIX_RATIO_BALANCED_LEVEL);
                mix_ratio.streamings[1].ratio = (*target_level >= ULL_MIX_RATIO_BALANCED_LEVEL) ?
                    100 :
                    100 * (*target_level - ULL_MIX_RATIO_GAME_MAX_LEVEL) / (ULL_MIX_RATIO_BALANCED_LEVEL - ULL_MIX_RATIO_GAME_MAX_LEVEL);
                bt_status = bt_ull_action(BT_ULL_ACTION_SET_STREAMING_MIX_RATIO, &mix_ratio, sizeof(mix_ratio));
                APPS_LOG_MSGID_I(LOG_TAG": app set mix ratio level %d -> %d, status: 0x%x", 3,
                    s_ull_context.current_mix_ratio_level, *target_level, bt_status);
                if (BT_STATUS_SUCCESS == bt_status) {
                    s_ull_context.current_mix_ratio_level = *target_level;
                }
            }
            ret = true;
            break;
        }
        case APPS_EVENTS_INTERACTION_SET_ULL_MIC_VOL: {
            uint8_t* mic_vol = (uint8_t *)extra_data;
            bt_ull_streaming_info_t info = {0};
            bt_ull_streaming_t streaming = {
                .streaming_interface = BT_ULL_STREAMING_INTERFACE_MICROPHONE,
                .port = 0,
            };

            if ((BT_STATUS_SUCCESS == bt_ull_get_streaming_info(streaming,&info)) && info.is_playing) {
                bt_status_t bt_status;
                bt_ull_volume_t volume_param;
                volume_param.streaming.streaming_interface = BT_ULL_STREAMING_INTERFACE_MICROPHONE;
                volume_param.streaming.port = 0;
                volume_param.channel = BT_ULL_VOLUME_CHANNEL_DUEL;
                volume_param.volume = *mic_vol;
                volume_param.action = BT_ULL_VOLUME_ACTION_SET_ABSOLUTE_VOLUME;
                bt_status = bt_ull_action(BT_ULL_ACTION_SET_STREAMING_VOLUME, &volume_param, sizeof(volume_param));
                APPS_LOG_MSGID_I(LOG_TAG": app set mic vol %d, status: 0x%x", 2, *mic_vol, bt_status);
                if (BT_STATUS_SUCCESS == bt_status) {
                    s_ull_context.mic_vol = *mic_vol;
                }
            }
            ret = true;
            break;
        }
        default:
            break;
    }

    return ret;
}

static bool app_ull_proc_key_event_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = false;
    uint8_t key_id;
    airo_key_event_t key_event;

    app_event_key_event_decode(&key_id, &key_event, event_id);

    apps_config_key_action_t action;
    if (extra_data) {
        action = *(uint16_t *)extra_data;
    } else {
        action = apps_config_key_event_remapper_map_action(key_id, key_event);
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
        switch (action) {
            case KEY_MUTE_MIC:
            case KEY_ULL_RECONNECT:
            case KEY_ULL_SWITCH_LINK_MODE:
            case KEY_ULL_SWITCH_GAME_MODE: {
                if (bt_aws_mce_srv_get_link_type() != BT_AWS_MCE_SRV_LINK_NONE) {
                    if (BT_STATUS_SUCCESS != apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_KEY, action)) {
                        APPS_LOG_MSGID_I(LOG_TAG", Partner send key action: %x aws to agent failed", 1, action);
                        apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    } else {
                        APPS_LOG_MSGID_I(LOG_TAG", Partner send key action: %x aws to agent success", 1, action);
                    }
                } else {
                    apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
                    APPS_LOG_MSGID_I(LOG_TAG", Partner aws disconnected for key action: %x", 1, action);
                }
                ret = true;
                return ret;
                break;
            }
            default:
                break;
        }
    }
#endif

    switch (action) {
        case KEY_MUTE_MIC: {
            s_ull_context.mic_mute = !s_ull_context.mic_mute;
            bt_ull_streaming_t streaming;
            bt_ull_action_t action;
            if (s_ull_context.mic_mute) {
                action = BT_ULL_ACTION_SET_STREAMING_MUTE;
            } else {
                action = BT_ULL_ACTION_SET_STREAMING_UNMUTE;
            }
            streaming.streaming_interface = BT_ULL_STREAMING_INTERFACE_MICROPHONE;
            streaming.port = 0;
            bt_ull_action(action, &streaming, sizeof(streaming));
            ret = true;
            break;
        }
#ifndef AIR_ONLY_DONGLE_MODE_ENABLE
        case KEY_ULL_RECONNECT: {
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
            {
                app_ull_reconnect_request_process();
            }
            ret = true;
            break;
        }
        case KEY_ULL_SWITCH_LINK_MODE: {
            APPS_LOG_MSGID_I(LOG_TAG", link mode change, old:%d", 1, s_ull_context.link_mode);
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
            {
                app_ull_process_events(ULL_EVENTS_SWITCH_LINK_MODE, NULL);
                apps_config_set_vp(VP_INDEX_DOUBLE, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            break;
        }
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
        case KEY_ULL_SWITCH_GAME_MODE : {
            APPS_LOG_MSGID_I(LOG_TAG", game mode change, old:%d", 1, s_ull_context.game_mode);
#ifdef MTK_AWS_MCE_ENABLE
            if (bt_device_manager_aws_local_info_get_role() != BT_AWS_MCE_ROLE_CLINET)
#endif
            {
                app_ull_process_events(ULL_EVENTS_SWITCH_GAME_MODE, NULL);
            }
            break;
        }
#endif
#endif
        default:
            break;
    }

    return ret;
}

static bool app_ull_proc_bt_cm_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    app_ull_context_t *local_context = (app_ull_context_t *)self->local_context;
#ifdef MTK_AWS_MCE_ENABLE
    bt_status_t bt_status;
#endif
    if (NULL == local_context) {
        return ret;
    }
    switch (event_id) {
        case BT_CM_EVENT_POWER_STATE_UPDATE: {
            break;
        }
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (NULL == remote_update) {
                break;
            }
#ifdef MTK_AWS_MCE_ENABLE
            if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role() ||
                BT_AWS_MCE_ROLE_NONE == bt_device_manager_aws_local_info_get_role())
#endif
            {
                if (BT_CM_ACL_LINK_CONNECTED > remote_update->pre_acl_state && BT_CM_ACL_LINK_CONNECTED <= remote_update->acl_state) {
                    if (ULL_IS_DONGLE_ADDRESS(remote_update->address)) {
                        APPS_LOG_MSGID_I(LOG_TAG", Dongle connected, link_mode: %d", 1, s_ull_context.link_mode);
                        app_ull_process_events(ULL_EVENTS_DONGLE_CONNECTED, &remote_update->address);
                    }
#ifdef MTK_AWS_MCE_ENABLE
                    else if (ULL_IS_ADDRSS_THE_SAME(remote_update->address, *(bt_device_manager_get_local_address()))) {
                        APPS_LOG_MSGID_I(LOG_TAG", AWS connected, ignore.", 0);
                    }
#endif
                    else {
                        APPS_LOG_MSGID_I(LOG_TAG", SP connected, link_mode: %d", 1, s_ull_context.link_mode);
                        app_ull_process_events(ULL_EVENTS_SP_CONNECTED, &remote_update->address);
                    }
                } else if ((BT_CM_ACL_LINK_CONNECTED <= remote_update->pre_acl_state && BT_CM_ACL_LINK_CONNECTED > remote_update->acl_state)
                        || (BT_CM_ACL_LINK_DISCONNECTING == remote_update->pre_acl_state && BT_CM_ACL_LINK_DISCONNECTED == remote_update->acl_state)) {
                    if (ULL_IS_DONGLE_ADDRESS(remote_update->address)) {
                        APPS_LOG_MSGID_I(LOG_TAG", Dongle disconnected, link_mode: %d", 1, s_ull_context.link_mode);
                        if (ULL_SINGLE_LINK_DISCONNECTING_DONGLE == s_single_link_disconnecting) {
                            s_single_link_disconnecting = ULL_SINGLE_LINK_DISCONNECTING_NONE;
                        }
                        app_ull_process_events(ULL_EVENTS_DONGLE_DISCONNECTED, &remote_update->address);
                        s_uplink_started = false;
                    }
#ifdef MTK_AWS_MCE_ENABLE
                    else if (ULL_IS_ADDRSS_THE_SAME(remote_update->address, *(bt_device_manager_get_local_address()))) {
                        APPS_LOG_MSGID_I(LOG_TAG", AWS disconnect, ignore.", 0);
                    }
#endif
                    else {
                        APPS_LOG_MSGID_I(LOG_TAG", SP disconnected, link_mode: %d", 1, s_ull_context.link_mode);
                        if (ULL_SINGLE_LINK_DISCONNECTING_SP == s_single_link_disconnecting) {
                            s_single_link_disconnecting = ULL_SINGLE_LINK_DISCONNECTING_NONE;
                        }
                        app_ull_process_events(ULL_EVENTS_SP_DISCONNECTED, &remote_update->address);
                    }
                }
                if ((~remote_update->pre_connected_service) & remote_update->connected_service
                    & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL)) {
                    if (!ULL_IS_ADDRSS_THE_SAME(s_ull_context.dongle_bt_address, remote_update->address)) {
                        APPS_LOG_MSGID_I(LOG_TAG", dongle connected, but address has not recorded", 0);
                        bt_cm_connect_t cm_param;
                        cm_param.profile = BT_CM_PROFILE_SERVICE_MASK_ALL;
                        memcpy(cm_param.address, remote_update->address, sizeof(bt_bd_addr_t));
                        bt_cm_disconnect(&cm_param);
                        /*
                        app_ull_store_dongle_address(false, &(remote_update->address));
                        app_ull_process_events(ULL_EVENTS_DONGLE_CONNECTED, &remote_update->address);
                        */
                    }
                }
#ifdef MTK_AWS_MCE_ENABLE
                if ((~remote_update->pre_connected_service) & remote_update->connected_service
                    & BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)) {
                    if (s_need_resync_ull_addr
                        && BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                        bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_BT_ULTRA_LOW_LATENCY, BT_ULL_EVENT_PAIRING_COMPLETE_IND,
                                                            s_ull_context.dongle_bt_address, sizeof(bt_bd_addr_t));
                        if (BT_STATUS_SUCCESS == bt_status) {
                            s_need_resync_ull_addr = false;
                            bt_aws_mce_role_recovery_unlock();
                        }
                        APPS_LOG_MSGID_I(LOG_TAG", Resync dongle address ret %x", 1, bt_status);
                    }
                    if (!s_ull_link_mode_synced
                        && BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()) {
                        app_ull_nvdm_config_data_t config_data = {
                            .link_mode = s_ull_context.link_mode,
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                            .game_mode = s_ull_context.game_mode,
#endif
                        };
                        bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION, APPS_EVENTS_INTERACTION_ULL_LINK_MODE_CHANGE,
                                                                                &(config_data), sizeof(config_data));
                        if (BT_STATUS_SUCCESS == bt_status) {
                            s_ull_link_mode_synced = true;
                        }
                        APPS_LOG_MSGID_I(LOG_TAG", Resync link mode(%d) game mode(%x) ret %x", 3, s_ull_context.link_mode,
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                                        s_ull_context.game_mode,
#else
                                        0,
#endif
                                        bt_status);
                    }
                    bool connected = s_ull_context.adv_paused;
                    if (connected) {
                        apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                APPS_EVENTS_INTERACTION_ULL_DONGLE_CONNECTION_CHANGE,
                                &connected, sizeof(connected));
                    }
                }
#endif
            }
#ifdef MTK_AWS_MCE_ENABLE
            else if (BT_AWS_MCE_ROLE_PARTNER == bt_device_manager_aws_local_info_get_role()) {
                if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)
                    & (remote_update->pre_connected_service & (~remote_update->connected_service))) {
#ifndef AIR_ONLY_DONGLE_MODE_ENABLE
                    if (s_ull_context.adv_paused) {
                        multi_ble_adv_manager_resume_ble_adv();
                        s_ull_context.adv_paused = false;
                    }
#endif
                    s_dongle_connected = false;
                    s_uplink_started = false;
                    s_single_link_disconnecting = ULL_SINGLE_LINK_DISCONNECTING_NONE;
                    app_ull_partner_set_a2dp_enable();
                } else if (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS)
                    & ((~remote_update->pre_connected_service) & remote_update->connected_service)) {
                    s_single_link_disconnecting = ULL_SINGLE_LINK_DISCONNECTING_NONE;
                    if (BT_AWS_MCE_SRV_LINK_NORMAL == bt_aws_mce_srv_get_link_type()) {
                    }
                }
            }
#endif
            break;
        }
        default:
            ret = false;
            break;
    }
    return ret;
}

static bool app_ull_proc_bt_dm_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;
    bool need_reconnect = false;
    bt_status_t bt_status = BT_STATUS_FAIL;

    bt_device_manager_power_event_t evt;
    bt_device_manager_power_status_t status;
    bt_event_get_bt_dm_event_and_status(event_id, &evt, &status);

    switch (evt) {
        case BT_DEVICE_MANAGER_POWER_EVT_PREPARE_ACTIVE:
            break;
        case BT_DEVICE_MANAGER_POWER_EVT_PREPARE_STANDBY:
            break;
        case BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE:
            break;
        case BT_DEVICE_MANAGER_POWER_EVT_CLASSIC_ACTIVE_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
#if defined(AIR_DUAL_CHIP_MIXING_MODE_ROLE_MASTER_ENABLE)
                if (s_reboot_trigger_reconnect) {
                    need_reconnect = true;
                    s_reboot_trigger_reconnect = false;
                } else {
                    need_reconnect = false;
                }
#else
                need_reconnect = true;
#endif
            }
            if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode) {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, BT_APP_COMMON_ULL_STREAM_RETRY_COUNT_FOR_CONNECTING);
            } else {
                bt_app_common_pre_set_ultra_low_latency_retry_count(BT_APP_COMMON_ULL_LATENCY_MODULE_RECONNECT, APPS_ULL_STREAMING_RETRY_COUNT_FOR_SINGLE_LINK);
            }
            bt_app_common_apply_ultra_low_latency_retry_count();
            break;
        }
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
#ifdef MTK_AWS_MCE_ENABLE
                bt_aws_mce_role_recovery_unlock();
#endif
            }
            break;
        }
        default:
            break;
    }

    if (need_reconnect) {
        bool reconnect_dongle = false;
#ifndef AIR_ONLY_DONGLE_MODE_ENABLE
        bt_device_manager_paired_infomation_t paired_info[2];
        uint32_t paired_info_count = 2;
        bool reconnect_sp = false;
        bool dongle_is_first_in_paired_list = false;
        bt_bd_addr_t *sp_addr = NULL;
        uint32_t i;

        bt_device_manager_get_paired_list(paired_info, &paired_info_count);
        for (i = 0; i < paired_info_count; i++) {
            if (ULL_IS_DONGLE_ADDRESS(paired_info[i].address)) {
                if (NULL == sp_addr) {
                    dongle_is_first_in_paired_list = true;
                }
            } else if (!reconnect_sp) {
                sp_addr = &paired_info[i].address;
                reconnect_sp = true;
                break;
            }
        }

        /* Multi-link mode must reconnect dongle first. Single link mode reconnect the last paired device. */
        if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode
            || (ULL_LINK_MODE_SINGLE == s_ull_context.link_mode && dongle_is_first_in_paired_list)) {
            reconnect_dongle = true;
        }


        if (reconnect_dongle && !ULL_IS_ADDRESS_EMPTY(s_ull_context.dongle_bt_address)
#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT)
            && ULL_AUX_USB_AUDIO_STATE_IN != s_ull_context.aux_state
            && ULL_AUX_USB_AUDIO_STATE_IN != s_ull_context.usb_audio_state
#endif
        ) {
            bt_status = app_ull_connect_correct_profile(&s_ull_context.dongle_bt_address);
            APPS_LOG_MSGID_I(LOG_TAG", power on reconnect dongle:%x", 1, bt_status);
        }

        if (sp_addr) {
            bt_status = app_ull_connect_correct_profile(sp_addr);
            APPS_LOG_MSGID_I(LOG_TAG", power on reconnect SP:%x", 1, bt_status);
        }
#endif  /* #ifndef AIR_ONLY_DONGLE_MODE_ENABLE */

        /* Both mode (Single link or multi link) need reconnect dongle. */
        if (!reconnect_dongle && !ULL_IS_ADDRESS_EMPTY(s_ull_context.dongle_bt_address)
#if defined(APPS_LINE_IN_SUPPORT) || defined(APPS_USB_AUDIO_SUPPORT)
            && ULL_AUX_USB_AUDIO_STATE_IN != s_ull_context.aux_state
            && ULL_AUX_USB_AUDIO_STATE_IN != s_ull_context.usb_audio_state
#endif
        ) {
            bt_status = app_ull_connect_correct_profile(&s_ull_context.dongle_bt_address);
            APPS_LOG_MSGID_I(LOG_TAG", power on reconnect dongle:%x", 1, bt_status);
        }
    }

    return ret;
}

static bool app_ull_proc_ull_event(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    bool ret = false;

    switch (event_id) {
        case BT_ULL_EVENT_PAIRING_COMPLETE_IND: {
            bt_ull_pairing_complete_ind_t *air_pairing_ind = (bt_ull_pairing_complete_ind_t *)extra_data;
            if (air_pairing_ind->result) {
                app_ull_store_dongle_address(true, &(air_pairing_ind->remote_address));
                apps_config_set_foreground_led_pattern(LED_INDEX_AIR_PAIRING_SUCCESS, 30, false);
                apps_config_set_vp(VP_INDEX_SUCCESSED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
#ifdef MTK_AWS_MCE_ENABLE
                bt_aws_mce_role_recovery_lock();
#endif
            } else {
                apps_config_set_foreground_led_pattern(LED_INDEX_AIR_PAIRING_FAIL, 30, false);
                apps_config_set_vp(VP_INDEX_FAILED, false, 0, VOICE_PROMPT_PRIO_MEDIUM, false, NULL);
            }
            break;
        }
        case BT_ULL_EVENT_UPLINK_START_SUCCESS: {
            s_uplink_started = true;
            break;
        }
        case BT_ULL_EVENT_UPLINK_STOP_SUCCESS: {
            s_uplink_started = false;
            break;
        }
        default:
            break;
    }

    return ret;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool app_ull_proc_aws_data_event_proc(ui_shell_activity_t *self, uint32_t unused_id, void *aws_extra, size_t data_len)
{
    bool ret = false;
    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)aws_extra;
    if (NULL == aws_data_ind || aws_data_ind->module_id != BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        return ret;
    }
    uint32_t event_group;
    uint32_t event_id;
    uint8_t *extra_data;
    uint32_t extra_data_len;


    apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event_id, (void *)&extra_data, &extra_data_len);
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION:
            switch (event_id) {
                case APPS_EVENTS_INTERACTION_ULL_DONGLE_CONNECTION_CHANGE: {
                    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                        bool connected = *(bool *)extra_data;
                        APPS_LOG_MSGID_I(LOG_TAG", receive dongle connection from agent, %d", 1, connected);
#ifndef AIR_ONLY_DONGLE_MODE_ENABLE
                        if (connected && !s_ull_context.adv_paused) {
                            multi_ble_adv_manager_pause_ble_adv();
                            s_ull_context.adv_paused = true;
                        } else if (!connected && s_ull_context.adv_paused) {
                            multi_ble_adv_manager_resume_ble_adv();
                            s_ull_context.adv_paused = false;
                        }
#endif
                        s_dongle_connected = connected;
                        app_ull_partner_set_a2dp_enable();
                    }
                    break;
                }
                case APPS_EVENTS_INTERACTION_ULL_LINK_MODE_CHANGE: {
                    if (bt_device_manager_aws_local_info_get_role() == BT_AWS_MCE_ROLE_PARTNER) {
                        app_ull_nvdm_config_data_t *config_data = (app_ull_nvdm_config_data_t *)extra_data;
                        APPS_LOG_MSGID_I(LOG_TAG", receive ULL link mode from agent, %d, current : %d, game_mode %d->%d", 4, config_data->link_mode, s_ull_context.link_mode,
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                                        s_ull_context.game_mode, config_data->game_mode
#else
                                        0, 0
#endif
                                        );
                        if (config_data->link_mode != s_ull_context.link_mode
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                            || config_data->game_mode != s_ull_context.game_mode
#endif
                        ) {
                            s_ull_context.link_mode = config_data->link_mode;
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                            s_ull_context.game_mode = config_data->game_mode;
#endif
                            app_ull_partner_set_a2dp_enable();
                            nvkey_write_data(NVKEYID_APP_ULL_CONFIG, (uint8_t *)config_data, sizeof(app_ull_nvdm_config_data_t));
                        }
                    }
                    break;
                }
                default:
                    break;
            }
            break;
        case EVENT_GROUP_BT_ULTRA_LOW_LATENCY:
            switch (event_id) {
                case BT_ULL_EVENT_PAIRING_COMPLETE_IND: {
                    APPS_LOG_MSGID_I(LOG_TAG", partner receive ull address size = %d", 1, extra_data_len);
                    if (extra_data && extra_data_len == sizeof(bt_bd_addr_t)) {
                        memcpy(s_ull_context.dongle_bt_address, extra_data, sizeof(bt_bd_addr_t));
                        nvkey_write_data(NVKEYID_BT_ULL_PEER_ADDRESS, extra_data, sizeof(bt_bd_addr_t));
                    }
                    ret = true;
                    break;
                }
                default:
                    break;
            }
            break;
        case EVENT_GROUP_UI_SHELL_KEY:
            switch (event_id) {
#ifndef AIR_ONLY_DONGLE_MODE_ENABLE
                case KEY_ULL_RECONNECT:
                case KEY_ULL_SWITCH_LINK_MODE:
                case KEY_ULL_SWITCH_GAME_MODE:
#endif
                case KEY_MUTE_MIC: {
                    if (BT_AWS_MCE_ROLE_AGENT == bt_device_manager_aws_local_info_get_role()
                        || BT_AWS_MCE_ROLE_NONE == bt_device_manager_aws_local_info_get_role()) {
                        ret = app_ull_proc_key_event_group(self, 0, &event_id, 0);
                    }
                    break;
                }
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

#if defined(MTK_FOTA_ENABLE) && defined (MTK_FOTA_VIA_RACE_CMD)
#include "bt_connection_manager_internal.h"
static bool app_ull_proc_fota_group(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len)
{
    /* UI shell internal event must process by this activity, so default is true. */
    bool ret = false;

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    APPS_LOG_MSGID_I(LOG_TAG", fota event : %d, role = %x", 2, event_id, role);
#endif
    switch (event_id) {
        case RACE_EVENT_TYPE_FOTA_START: {
            bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_DISABLE, BT_CM_COMMON_TYPE_DISABLE);
            break;
        }
        case RACE_EVENT_TYPE_FOTA_CANCEL: {
            bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_ENABLE);
            break;
        }
        default:
            ret = false;
            break;
    }
    return ret;
}
#endif

bool app_ull_idle_activity_proc(
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
            ret = app_ull_proc_ui_shell_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_APP_INTERACTION: {
            /* APP interaction events. */
            ret = app_ull_proc_interaction_event_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_KEY: {
            /* key events. */
            ret = app_ull_proc_key_event_group(self, event_id, extra_data, data_len);
            break;
        }
#ifdef AIR_ROTARY_ENCODER_ENABLE
        case EVENT_GROUP_UI_SHELL_ROTARY_ENCODER: {
            /* Rotary encoder events. */
            ret = app_ull_proc_rotary_event_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER: {
            ret = app_ull_proc_bt_cm_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER: {
            ret = app_ull_proc_bt_dm_group(self, event_id, extra_data, data_len);
            break;
        }
        case EVENT_GROUP_BT_ULTRA_LOW_LATENCY: {
            ret = app_ull_proc_ull_event(self, event_id, extra_data, data_len);
            break;
        }
#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA: {
            ret = app_ull_proc_aws_data_event_proc(self, event_id, extra_data, data_len);
            break;
        }
#endif
#if defined(MTK_FOTA_ENABLE) && defined (MTK_FOTA_VIA_RACE_CMD)
        case EVENT_GROUP_UI_SHELL_FOTA: {
            ret = app_ull_proc_fota_group(self, event_id, extra_data, data_len);
            break;
        }
#endif
        default: {
            break;
        }
    }

    return ret;
}


bool bt_cm_check_connect_request(bt_bd_addr_ptr_t address, uint32_t cod)
{
    bool allow_connect = true;
    bool is_dongle = ULL_IS_DONGLE_ADDRESS(address);
    bt_bd_addr_t connected_address[3];
    uint32_t connected_num = 3;
    uint32_t i;
    connected_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE,
                                            connected_address, connected_num);
#ifdef APPS_LINE_IN_SUPPORT
    if (is_dongle && ULL_AUX_USB_AUDIO_STATE_IN == s_ull_context.aux_state) {
        APPS_LOG_MSGID_I(LOG_TAG", bt_cm_check_connect_request, dongle not allow by line in", 0);
        allow_connect = false;
    }
#endif
#ifdef APPS_USB_AUDIO_SUPPORT
    if (is_dongle && ULL_AUX_USB_AUDIO_STATE_IN == s_ull_context.usb_audio_state) {
        APPS_LOG_MSGID_I(LOG_TAG", bt_cm_check_connect_request, dongle not allow by usb audio", 0);
        allow_connect = false;
    }
#endif
    if (allow_connect && ULL_LINK_MODE_SINGLE == s_ull_context.link_mode && !app_bt_connection_service_get_current_status()->bt_visible) {
        if (!is_dongle) {
            for (i = 0; i < connected_num; i++) {
                if (ULL_IS_DONGLE_ADDRESS(connected_address[i])) {
                    bool bt_visible = app_bt_connection_service_get_current_status()->bt_visible;
                    APPS_LOG_MSGID_I(LOG_TAG", bt_cm_check_connect_request, SP connecting when Dongle is connected, visible: %d", 1, bt_visible);
                    if (!bt_visible) {
                        allow_connect = false;
                    }
                    break;
                }
            }
        } else {
            for (i = 0; i < connected_num; i++) {
                if (!ULL_IS_DONGLE_ADDRESS(connected_address[i])
#ifdef MTK_AWS_MCE_ENABLE
                    && !ULL_IS_LOCAL_ADDRESS(connected_address[i])
#endif
                ) {
                    APPS_LOG_MSGID_I(LOG_TAG", bt_cm_check_connect_request, dongle connecting when SP is connected", 0);
                    allow_connect = false;
                    break;
                }
            }
        }
    } else if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode) {
#if defined (__BT_FAST_PAIR_ENABLE__)
        if (!(is_dongle || app_fast_pair_get_is_waiting_connect())) {
            for (i = 0; i < connected_num; i++) {
                if (!ULL_IS_DONGLE_ADDRESS(connected_address[i])
#ifdef MTK_AWS_MCE_ENABLE
                    && !ULL_IS_LOCAL_ADDRESS(connected_address[i])
#endif
                ) {
                    APPS_LOG_MSGID_I(LOG_TAG", bt_cm_check_connect_request, SP already connected", 0);
                    allow_connect = false;
                    break;
                }
            }
        }
#endif
    }
    APPS_LOG_MSGID_I(LOG_TAG", bt_cm_check_connect_request, allow connect :%d for dongle?%d", 2, allow_connect, is_dongle);
    return allow_connect;
}

bool app_ull_idle_activity_allow_a2dp_connect(const bt_bd_addr_t *address)
{
#if (!defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
    if (0 < bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL), NULL, 0)
#if (defined AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
        && s_ull_context.game_mode
#endif
    ) {
        APPS_LOG_MSGID_I(LOG_TAG", bt_a2dp_allow_connection_check false due to ULL profile connected", 0);
        return false;
    } else {
        APPS_LOG_MSGID_I(LOG_TAG", bt_a2dp_allow_connection_check true", 0);
    }
#endif
    return true;
}

uint32_t bt_cm_get_reconnect_profile(bt_bd_addr_t *addr)
{
    uint32_t profiles = BT_CM_PROFILE_SERVICE_MASK_NONE;
    bool dongle_connected = false;
    bt_bd_addr_t connectied_sp[1];
    uint32_t connected_sp_len = 1;
    if (addr == NULL) {
        return BT_CM_PROFILE_SERVICE_MASK_NONE;
    }

    app_ull_get_connected_devices_list(connectied_sp, &connected_sp_len, &dongle_connected);
    if (ULL_IS_DONGLE_ADDRESS(*addr)) {
        if (connected_sp_len == 0) {
            /* Event it is multi link mode, do not reconnect link lost dongle when SP is connected. */
            profiles = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL);
        }
    } else {
        if (ULL_LINK_MODE_MULTIPLE == s_ull_context.link_mode || !dongle_connected) {
            profiles = BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP);
#if ((!defined AIR_BT_ULTRA_LOW_LATENCY_A2DP_STANDBY_ENABLE) || defined (AIR_APP_ULL_GAMING_MODE_UI_ENABLE))
            if (!dongle_connected
#if defined (AIR_APP_ULL_GAMING_MODE_UI_ENABLE)
                || !s_ull_context.game_mode
#endif
            )
#endif
            {
                profiles |= BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK);
            }
        }
    }
    return profiles;
}

const app_ull_context_t *app_ull_idle_activity_get_current_context(void)
{
    return &s_ull_context;
}

uint8_t app_ull_get_mic_vol(void)
{
     return s_ull_context.mic_vol;
}
uint8_t app_ull_get_mix_ratio(void)
{
    return s_ull_context.current_mix_ratio_level;
}

bool app_ull_is_uplink_open()
{
    return s_uplink_started;
}

