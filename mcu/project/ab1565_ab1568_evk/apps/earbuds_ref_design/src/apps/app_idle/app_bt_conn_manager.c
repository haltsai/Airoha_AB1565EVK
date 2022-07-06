
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
 * File: app_bt_conn_manager.c
 *
 * Description: This file provides BT Connection Manager.
 *
 */

#if defined(AIR_LE_AUDIO_ENABLE) && (defined(AIR_MULTI_POINT_ENABLE) || defined(AIR_LE_AUDIO_MULTIPOINT_ENABLE))

#include "app_bt_conn_manager.h"

#include "app_bt_state_service.h"
#include "apps_debug.h"
#include "apps_events_bt_event.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "app_le_audio.h"
#include "bt_aws_mce_srv.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#include "bt_customer_config.h"
#include "bt_device_manager.h"
#include "bt_sink_srv_le.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "apps_aws_sync_event.h"
#endif
#ifdef AIR_MULTI_POINT_ENABLE
#include "app_bt_emp_service.h"
#endif
#ifdef __BT_FAST_PAIR_ENABLE__
#include "app_fast_pair.h"
#endif

#define LOG_TAG             "[APP_CONN]"

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

typedef enum {
    APP_BT_CONN_STATE_NONE = 0,
    APP_BT_CONN_STATE_ONE_LEA,
    APP_BT_CONN_STATE_ONE_EDR,
    APP_BT_CONN_STATE_TWO_LEA,
    APP_BT_CONN_STATE_TWO_EDR,
    APP_BT_CONN_STATE_LEA_EDR
} app_bt_conn_state_t;

typedef enum {
    APP_BT_CONN_EVENT_LEA_CONNECTED = 0,
    APP_BT_CONN_EVENT_LEA_DISCONNECTED,
    APP_BT_CONN_EVENT_EDR_CONNECTED,
    APP_BT_CONN_EVENT_EDR_DISCONNECTED
} app_bt_conn_event_t;

#define APP_BT_CONN_AWS_SYNC_EVENT              0
#define APP_BT_CONN_AWS_SYNC_EDR_NUM            1

typedef struct {
    uint8_t                                type;
    uint8_t                                data;
} PACKED app_bt_conn_aws_data_t;

typedef void (*app_bt_conn_state_do_action)(void);

typedef struct {
    app_bt_conn_state_t                    cur_state;
    app_bt_conn_event_t                    event;
    app_bt_conn_state_t                    next_state;
    app_bt_conn_state_do_action            do_action;
} app_bt_conn_state_transform_t;

typedef struct {
    app_bt_conn_state_t                    state;
    uint8_t                                bt_cm_num;
#ifdef MTK_AWS_MCE_ENABLE
    app_bt_conn_state_t                    peer_state;
#endif
} app_bt_conn_state_context_t;

static void app_bt_conn_state_none_do_action();
static void app_bt_conn_state_one_lea_do_action();
static void app_bt_conn_state_one_edr_do_action();
static void app_bt_conn_state_two_lea_do_action();
static void app_bt_conn_state_two_edr_do_action();
static void app_bt_conn_state_lea_edr_do_action();

static app_bt_conn_state_transform_t    app_bt_conn_transform_table[] = {
    {APP_BT_CONN_STATE_NONE,     APP_BT_CONN_EVENT_LEA_CONNECTED,    APP_BT_CONN_STATE_ONE_LEA,  app_bt_conn_state_one_lea_do_action},
    {APP_BT_CONN_STATE_NONE,     APP_BT_CONN_EVENT_EDR_CONNECTED,    APP_BT_CONN_STATE_ONE_EDR,  app_bt_conn_state_one_edr_do_action},
    {APP_BT_CONN_STATE_ONE_LEA,  APP_BT_CONN_EVENT_LEA_CONNECTED,    APP_BT_CONN_STATE_TWO_LEA,  app_bt_conn_state_two_lea_do_action},
    {APP_BT_CONN_STATE_ONE_LEA,  APP_BT_CONN_EVENT_EDR_CONNECTED,    APP_BT_CONN_STATE_LEA_EDR,  app_bt_conn_state_lea_edr_do_action},
    {APP_BT_CONN_STATE_ONE_LEA,  APP_BT_CONN_EVENT_LEA_DISCONNECTED, APP_BT_CONN_STATE_NONE,     app_bt_conn_state_none_do_action},
    {APP_BT_CONN_STATE_ONE_EDR,  APP_BT_CONN_EVENT_LEA_CONNECTED,    APP_BT_CONN_STATE_LEA_EDR,  app_bt_conn_state_lea_edr_do_action},
    {APP_BT_CONN_STATE_ONE_EDR,  APP_BT_CONN_EVENT_EDR_CONNECTED,    APP_BT_CONN_STATE_TWO_EDR,  app_bt_conn_state_two_edr_do_action},
    {APP_BT_CONN_STATE_ONE_EDR,  APP_BT_CONN_EVENT_EDR_DISCONNECTED, APP_BT_CONN_STATE_NONE,     app_bt_conn_state_none_do_action},
    {APP_BT_CONN_STATE_TWO_LEA,  APP_BT_CONN_EVENT_LEA_DISCONNECTED, APP_BT_CONN_STATE_ONE_LEA,  app_bt_conn_state_one_lea_do_action},
    {APP_BT_CONN_STATE_TWO_EDR,  APP_BT_CONN_EVENT_EDR_DISCONNECTED, APP_BT_CONN_STATE_ONE_EDR,  app_bt_conn_state_one_edr_do_action},
    {APP_BT_CONN_STATE_LEA_EDR,  APP_BT_CONN_EVENT_LEA_DISCONNECTED, APP_BT_CONN_STATE_ONE_EDR,  app_bt_conn_state_one_edr_do_action},
    {APP_BT_CONN_STATE_LEA_EDR,  APP_BT_CONN_EVENT_EDR_DISCONNECTED, APP_BT_CONN_STATE_ONE_LEA,  app_bt_conn_state_one_lea_do_action},
};

#define APP_BT_CONN_TRANSFORM_TABLE_SIZE  (sizeof(app_bt_conn_transform_table) / sizeof(app_bt_conn_state_transform_t))

static app_bt_conn_state_context_t        app_bt_conn_manager_ctx = {0};
static bt_bd_addr_t                       app_bt_conn_manager_phone_addr[1] = {{0}};



/**============================================================================*/
/**                              Internal Function                             */
/**============================================================================*/
static void app_bt_conn_mgr_machine_run(uint8_t event);

static void app_bt_conn_mgr_cancel_connect(void)
{
    bt_bd_addr_t bd_addr[2];
    uint32_t connecting_number = 2;

    connecting_number = bt_cm_get_connecting_devices(~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS),
                                                     bd_addr, connecting_number);
    bt_cm_connect_t connect_param = {{0}, ~(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS))};
    for (int i = 0; i < connecting_number; i++) {
        memcpy(connect_param.address, bd_addr[i], sizeof(bt_bd_addr_t));
        bt_status_t bt_status = bt_cm_disconnect(&connect_param);
        uint8_t *addr = (uint8_t *)bd_addr[i];
        APPS_LOG_MSGID_I(LOG_TAG" cancel_connect, addr=%02X:%02X:%02X:%02X:%02X:%02X bt_status=0x%08X",
                         7, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], bt_status);
    }
}

static void app_bt_conn_mgr_enable_page_scan(bool enable)
{
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role != BT_AWS_MCE_ROLE_PARTNER) {
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW,
                              (enable ? BT_CM_COMMON_TYPE_ENABLE : BT_CM_COMMON_TYPE_DISABLE));
        APPS_LOG_MSGID_I(LOG_TAG" page_scan, enable=%d", 1, enable);

        if (!enable) {
            app_bt_conn_mgr_cancel_connect();
        }
    } else {
        APPS_LOG_MSGID_I(LOG_TAG" page_scan, Partner not need to enable=%d", 1, enable);
    }
}

static void app_bt_conn_mgr_disallow_edr(void)
{
    app_bt_conn_mgr_enable_page_scan(FALSE);
    bool exit_discoverable = app_bt_state_service_set_bt_visible(FALSE, FALSE, 0);
#ifdef __BT_FAST_PAIR_ENABLE__
    app_fast_pair_stop_advertising();
#endif
    APPS_LOG_MSGID_I(LOG_TAG" disallow_edr, exit discoverable/fast_pair_adv=%d",
                     1, exit_discoverable);
}

static bool app_bt_conn_mgr_is_support_emp()
{
    bool support_emp = FALSE;
#ifdef AIR_MULTI_POINT_ENABLE
    support_emp = app_bt_emp_is_enable();
#endif
    return support_emp;
}

static uint8_t app_bt_conn_mgr_get_bt_cm_num(uint8_t support_max_edr_num)
{
    uint8_t bt_cm_num = 0;
#ifdef MTK_AWS_MCE_ENABLE
    bt_cm_num += 1;
#endif

    const bt_cm_config_t *bt_cm_config = bt_customer_config_get_cm_config();
    if (support_max_edr_num > 0 && bt_cm_config->connection_takeover) {
        bt_cm_num += 1;
    }
    if (support_max_edr_num > 0) {
        bt_cm_num += support_max_edr_num;
    }

    APPS_LOG_MSGID_I(LOG_TAG" get_bt_cm_num, support_max_edr_num=%d->%d",
                     2, support_max_edr_num, bt_cm_num);
    return bt_cm_num;
}

static void app_bt_conn_state_none_do_action()
{
    APPS_LOG_MSGID_I(LOG_TAG" NONE action", 0);
    // LEA - not start/stop ADV, should restart LEA ADV if ever connected
    // EDR - Support two EDR
#ifdef MTK_AWS_MCE_ENABLE
    if (app_bt_conn_manager_ctx.peer_state == APP_BT_CONN_STATE_TWO_LEA) {
        APPS_LOG_MSGID_I(LOG_TAG" NONE action, not change bt_cm due to peer TWO_LEA", 0);
        return;
    }
#endif

    bool support_emp = app_bt_conn_mgr_is_support_emp();
    uint8_t bt_cm_num = app_bt_conn_mgr_get_bt_cm_num((support_emp ? 2 : 1));
    bt_status_t bt_status = bt_cm_set_max_connection_number(bt_cm_num, app_bt_conn_manager_phone_addr, 0, FALSE);
    if (bt_status == BT_STATUS_SUCCESS) {
        app_bt_conn_manager_ctx.bt_cm_num = bt_cm_num;
    }
    APPS_LOG_MSGID_I(LOG_TAG" NONE action, support_emp=%d bt_cm_num=%d bt_status=0x%08X",
                     3, support_emp, bt_cm_num, bt_status);
}

static void app_bt_conn_state_one_lea_do_action()
{
    APPS_LOG_MSGID_I(LOG_TAG" ONE_LEA action", 0);
    // LEA - start ADV
    // EDR - Support one EDR
    app_le_audio_start_advertising(0);

#ifdef MTK_AWS_MCE_ENABLE
    if (app_bt_conn_manager_ctx.peer_state == APP_BT_CONN_STATE_TWO_LEA) {
        APPS_LOG_MSGID_I(LOG_TAG" ONE_LEA action, start_le_adv but not change bt_cm due to peer TWO_LEA", 0);
        return;
    }
#endif

    uint8_t bt_cm_num = app_bt_conn_mgr_get_bt_cm_num(1);
    bt_status_t bt_status = bt_cm_set_max_connection_number(bt_cm_num, app_bt_conn_manager_phone_addr, 0, FALSE);
    if (bt_status == BT_STATUS_SUCCESS) {
        app_bt_conn_manager_ctx.bt_cm_num = bt_cm_num;
    }
    APPS_LOG_MSGID_I(LOG_TAG" ONE_LEA action, bt_cm_num=%d bt_status=0x%08X",
                     2, bt_cm_num, bt_status);
}

static void app_bt_conn_state_one_edr_do_action()
{
    APPS_LOG_MSGID_I(LOG_TAG" ONE_EDR action", 0);
    // LEA - start ADV
    // EDR - Support two EDR
    app_le_audio_start_advertising(0);
    bool support_emp = app_bt_conn_mgr_is_support_emp();
    uint8_t bt_cm_num = app_bt_conn_mgr_get_bt_cm_num((support_emp ? 2 : 1));
    bt_status_t bt_status = bt_cm_set_max_connection_number(bt_cm_num, app_bt_conn_manager_phone_addr, 0, FALSE);
    if (bt_status == BT_STATUS_SUCCESS) {
        app_bt_conn_manager_ctx.bt_cm_num = bt_cm_num;
    }
    APPS_LOG_MSGID_I(LOG_TAG" ONE_EDR action, support_emp=%d bt_cm_num=%d bt_status=0x%08X",
                     3, support_emp, bt_cm_num, bt_status);
}

static void app_bt_conn_state_two_lea_do_action()
{
    APPS_LOG_MSGID_I(LOG_TAG" TWO_LEA action", 0);
    // LEA - stop ADV
    // EDR - Support two EDR
    app_le_audio_stop_advertising();

    app_bt_conn_mgr_disallow_edr();
}

static void app_bt_conn_state_two_edr_do_action()
{
    APPS_LOG_MSGID_I(LOG_TAG" TWO_EDR action", 0);
    // LEA - stop ADV
    // EDR - do nothing (already support two EDR)
    app_le_audio_stop_advertising();
}

static void app_bt_conn_state_lea_edr_do_action()
{
    APPS_LOG_MSGID_I(LOG_TAG" LEA_EDR action", 0);
    // LEA - stop ADV
    // EDR - do nothing (already support two EDR)
    app_le_audio_stop_advertising();
    uint8_t bt_cm_num = app_bt_conn_mgr_get_bt_cm_num(1);
    bt_status_t bt_status = bt_cm_set_max_connection_number(bt_cm_num, app_bt_conn_manager_phone_addr, 0, FALSE);
    if (bt_status == BT_STATUS_SUCCESS) {
        app_bt_conn_manager_ctx.bt_cm_num = bt_cm_num;
    }
    APPS_LOG_MSGID_I(LOG_TAG" LEA_EDR action, bt_cm_num=%d bt_status=0x%08X", 2, bt_cm_num, bt_status);
}

static void app_bt_conn_bt_power_off_action()
{
    APPS_LOG_MSGID_I(LOG_TAG" BT_POWER_OFF action", 0);
    // LEA - stop ADV
    // EDR - bt_cm_num=0
    app_bt_conn_manager_ctx.bt_cm_num = 0;
}

static bool app_bt_conn_mgr_check_event_valid(uint8_t event)
{
    bool ret = FALSE;
    switch (event) {
        case APP_BT_CONN_EVENT_LEA_CONNECTED:
        case APP_BT_CONN_EVENT_LEA_DISCONNECTED:
        case APP_BT_CONN_EVENT_EDR_CONNECTED:
        case APP_BT_CONN_EVENT_EDR_DISCONNECTED:
            ret = TRUE;
            break;
        default:
            break;
    }
    return ret;
}

static void app_bt_conn_mgr_machine_run(uint8_t event)
{
    if (!app_bt_conn_mgr_check_event_valid(event)) {
        APPS_LOG_MSGID_E(LOG_TAG" state_machine_run, not valid event %d", 1, event);
        return;
    }

#ifdef MTK_AWS_MCE_ENABLE
    bt_aws_mce_srv_link_type_t aws_link_type = bt_aws_mce_srv_get_link_type();
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (event == APP_BT_CONN_EVENT_EDR_CONNECTED || event == APP_BT_CONN_EVENT_EDR_DISCONNECTED) {
        app_bt_conn_aws_data_t aws_data = {0};
        aws_data.type = APP_BT_CONN_AWS_SYNC_EVENT;
        aws_data.data = event;
        if (role == BT_AWS_MCE_ROLE_AGENT && aws_link_type != BT_AWS_MCE_SRV_LINK_NONE) {
            bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                                   APPS_EVENTS_INTERACTION_SYNC_APP_CONN_EVENT,
                                                                   &aws_data,
                                                                   sizeof(aws_data));
            APPS_LOG_MSGID_I(LOG_TAG" state_machine_run, [%02X] sync_to_partner event=%d bt_status=0x%08X",
                             3, role, event, bt_status);
        }
    }
#endif

    if (app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_TWO_LEA
        && event == APP_BT_CONN_EVENT_LEA_DISCONNECTED
#ifdef MTK_AWS_MCE_ENABLE
        && app_bt_conn_manager_ctx.peer_state != APP_BT_CONN_STATE_TWO_LEA
#endif
    ) {
        app_bt_conn_mgr_enable_page_scan(TRUE);
    }

    app_bt_conn_state_transform_t *transform = NULL;
    app_bt_conn_state_t state = app_bt_conn_manager_ctx.state;
    for (int i = 0; i < APP_BT_CONN_TRANSFORM_TABLE_SIZE; i++) {
        app_bt_conn_state_transform_t *temp = &app_bt_conn_transform_table[i];
        if (temp->cur_state == state && temp->event == event) {
            transform = temp;
            break;
        }
    }

    if (transform == NULL) {
        APPS_LOG_MSGID_E(LOG_TAG" state_machine_run, no transform state=%d event=%d", 2, state, event);
        return;
    }

    app_bt_conn_state_t next_state = transform->next_state;
    APPS_LOG_MSGID_I(LOG_TAG" state_machine_run, state=%d->%d event=%d", 3, state, next_state, event);
    app_bt_conn_manager_ctx.state = next_state;

    if (transform->do_action != NULL) {
        transform->do_action();
    }

#ifdef MTK_AWS_MCE_ENABLE
    if (aws_link_type != BT_AWS_MCE_SRV_LINK_NONE) {
        bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                               APPS_EVENTS_INTERACTION_SYNC_APP_CONN_STATE,
                                                               &app_bt_conn_manager_ctx.state,
                                                               sizeof(uint8_t));
        APPS_LOG_MSGID_I(LOG_TAG" state_machine_run, [%02X] sync_to_peer state=%d bt_status=0x%08X",
                         3, role, app_bt_conn_manager_ctx.state, bt_status);
    }
#endif
}

#ifdef MTK_AWS_MCE_ENABLE
static uint8_t app_bt_conn_mgr_get_conn_num(bool own, bool classic)
{
    uint8_t conn_num = 0;
    uint8_t state = (own ? app_bt_conn_manager_ctx.state : app_bt_conn_manager_ctx.peer_state);
    if (classic) {
        if (state == APP_BT_CONN_STATE_NONE
            || state == APP_BT_CONN_STATE_ONE_LEA
            || state == APP_BT_CONN_STATE_TWO_LEA) {
            conn_num = 0;
        } else if (state == APP_BT_CONN_STATE_ONE_EDR
                || state == APP_BT_CONN_STATE_LEA_EDR) {
            conn_num = 1;
        } else if (state == APP_BT_CONN_STATE_TWO_EDR) {
            conn_num = 2;
        }
    } else {
        if (state == APP_BT_CONN_STATE_NONE
            || state == APP_BT_CONN_STATE_ONE_EDR
            || state == APP_BT_CONN_STATE_TWO_EDR) {
            conn_num = 0;
        } else if (state == APP_BT_CONN_STATE_ONE_LEA
                || state == APP_BT_CONN_STATE_LEA_EDR) {
            conn_num = 1;
        } else if (state == APP_BT_CONN_STATE_TWO_LEA) {
            conn_num = 2;
        }
    }
    return conn_num;
}
#endif

/**============================================================================*/
/**                          UI_Shell EVENT Handler                            */
/**============================================================================*/
static bool app_bt_conn_mgr_activity_proc_ui_shell_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case EVENT_ID_SHELL_SYSTEM_ON_CREATE: {
            APPS_LOG_MSGID_I(LOG_TAG" CREATE", 0);
        }
        break;
    }
    return TRUE;
}

static bool app_bt_conn_mgr_activity_bt_cm_event_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)extra_data;
            if (remote_update != NULL) {
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

                uint8_t *addr = (uint8_t *)remote_update->address;
                bt_bd_addr_t* local_addr = bt_device_manager_get_local_address();
                bool phone_related = (memcmp(remote_update->address, local_addr, sizeof(bt_bd_addr_t)) != 0);
                APPS_LOG_MSGID_I(LOG_TAG" BT_CM event, [%02X] phone_conn=%d addr=%02X:%02X:%02X:%02X:%02X:%02X acl=%d->%d srv=0x%04X->0x%04X",
                                 12, role, phone_related, addr[0], addr[1], addr[2], addr[3], addr[4], addr[5],
                                 remote_update->pre_acl_state, remote_update->acl_state,
                                 remote_update->pre_connected_service, remote_update->connected_service);

#ifdef MTK_AWS_MCE_ENABLE
                bool aws_conntected = (!(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                                     && (BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service));

                // Partner need to minus EDR num when AWS disconnect at firstly, then wait next-AWS connected and sync Agent EDR num
                bool aws_disconntected = ((BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->pre_connected_service)
                                          && !(BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS) & remote_update->connected_service));
                if (role == BT_AWS_MCE_ROLE_PARTNER
                    && phone_related
                    && aws_disconntected) {
                    APPS_LOG_MSGID_I(LOG_TAG" BT_CM event, [20] AWS Disconnected -> EDR - 1", 0);
                    app_bt_conn_mgr_machine_run(APP_BT_CONN_EVENT_EDR_DISCONNECTED);
                    break;
                }
#endif

                if (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_NONE) {
                    if (remote_update->pre_acl_state == BT_CM_ACL_LINK_CONNECTING
                        && remote_update->acl_state == BT_CM_ACL_LINK_DISCONNECTED) {
                        APPS_LOG_MSGID_I(LOG_TAG" BT_CM event, ignore EDR(ACL) Connect fail", 0);
                        break;
                    }

                    // Notify Phone EDR Connection/Disconnection Event
                    if (remote_update->pre_acl_state != BT_CM_ACL_LINK_ENCRYPTED
                        && remote_update->acl_state == BT_CM_ACL_LINK_ENCRYPTED
                        && phone_related) {
                        APPS_LOG_MSGID_I(LOG_TAG" BT_CM event, phone EDR(ACL) Connected", 0);
                        app_bt_conn_mgr_machine_run(APP_BT_CONN_EVENT_EDR_CONNECTED);
                    } else if (remote_update->pre_acl_state != BT_CM_ACL_LINK_DISCONNECTED
                            && remote_update->acl_state == BT_CM_ACL_LINK_DISCONNECTED
                            && phone_related) {
                        APPS_LOG_MSGID_I(LOG_TAG" BT_CM event, phone EDR(ACL) Disconnected", 0);
                        app_bt_conn_mgr_machine_run(APP_BT_CONN_EVENT_EDR_DISCONNECTED);
                    }

                    // Notify app_conn_state to partner when AWS Connected
#ifdef MTK_AWS_MCE_ENABLE
                    if (aws_conntected) {
                        uint8_t state = app_bt_conn_manager_ctx.state;
                        uint8_t edr_connected_num = app_bt_conn_mgr_get_conn_num(TRUE, TRUE);

                        app_bt_conn_aws_data_t aws_data = {0};
                        aws_data.type = APP_BT_CONN_AWS_SYNC_EDR_NUM;
                        aws_data.data = edr_connected_num;
                        bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                                               APPS_EVENTS_INTERACTION_SYNC_APP_CONN_EVENT,
                                                                               &aws_data,
                                                                               sizeof(aws_data));
                        APPS_LOG_MSGID_I(LOG_TAG" BT_CM event, [%02X] AWS Connected, sync_to_partner state=%d edr_connected_num=%d bt_status=0x%08X",
                                         4, role, state, edr_connected_num, bt_status);
                    }
#endif
                }

#ifdef MTK_AWS_MCE_ENABLE
                if (aws_conntected) {
                    bt_status_t bt_status = apps_aws_sync_event_send_extra(EVENT_GROUP_UI_SHELL_APP_INTERACTION,
                                                                           APPS_EVENTS_INTERACTION_SYNC_APP_CONN_STATE,
                                                                           &app_bt_conn_manager_ctx.state,
                                                                           sizeof(uint8_t));
                    APPS_LOG_MSGID_I(LOG_TAG" BT_CM event, [%02X] AWS Connected, sync_to_peer state=%d bt_status=0x%08X",
                                     3, role, app_bt_conn_manager_ctx.state, bt_status);
                }
#endif
            }
            break;
        }
        default:
            break;
    }
    return FALSE;
}

static bool app_bt_conn_mgr_activity_bt_dm_event_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    bt_device_manager_power_event_t event = 0;
    bt_device_manager_power_status_t status = 0;
    bt_event_get_bt_dm_event_and_status(event_id, &event, &status);
    switch (event) {
        case BT_DEVICE_MANAGER_POWER_EVT_ACTIVE_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_STATUS_SUCCESS == status) {
                APPS_LOG_MSGID_I(LOG_TAG" BT POWER ON", 0);
                app_bt_conn_manager_ctx.state = APP_BT_CONN_STATE_NONE;
                app_bt_conn_state_none_do_action();
            }
            break;
        }
        case BT_DEVICE_MANAGER_POWER_EVT_STANDBY_COMPLETE: {
            if (BT_DEVICE_MANAGER_POWER_RESET_TYPE_NORMAL == status) {
                APPS_LOG_MSGID_I(LOG_TAG" BT POWER OFF", 0);
                app_bt_conn_manager_ctx.state = APP_BT_CONN_STATE_NONE;
                app_bt_conn_bt_power_off_action();
            }
            break;
        }
    }
    return FALSE;
}

static bool app_bt_conn_mgr_activity_bt_sink_event_group(uint32_t event_id, void *extra_data, size_t data_len)
{
    switch (event_id) {
        case LE_SINK_SRV_EVENT_REMOTE_INFO_UPDATE: {
            bt_le_sink_srv_event_remote_info_update_t *update_ind = (bt_le_sink_srv_event_remote_info_update_t *)extra_data;
            if (update_ind == NULL) {
                break;
            }
            APPS_LOG_MSGID_I(LOG_TAG" BT_SINK LEA_Link=%d->%d SRV=%d->%d", 4,
                             update_ind->pre_state, update_ind->state,
                             update_ind->pre_connected_service, update_ind->connected_service);
            if (update_ind->pre_state == BT_BLE_LINK_DISCONNECTED
                && update_ind->state == BT_BLE_LINK_CONNECTED) {
                app_bt_conn_mgr_machine_run(APP_BT_CONN_EVENT_LEA_CONNECTED);
            } else if (update_ind->pre_state == BT_BLE_LINK_CONNECTED
                    && update_ind->state == BT_BLE_LINK_DISCONNECTED) {
                app_bt_conn_mgr_machine_run(APP_BT_CONN_EVENT_LEA_DISCONNECTED);
            }
            break;
        }

        default:
            break;
    }
    return FALSE;
}

#ifdef MTK_AWS_MCE_ENABLE
static bool app_bt_conn_mgr_activity_proc_aws_data(void *extra_data, size_t data_len)
{
    bool ret = FALSE;

    bt_aws_mce_report_info_t *aws_data_ind = (bt_aws_mce_report_info_t *)extra_data;
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
    if (aws_data_ind->module_id == BT_AWS_MCE_REPORT_MODULE_APP_ACTION) {
        uint32_t event_group;
        uint32_t event_id;
        void *p_extra_data = NULL;
        uint32_t extra_data_len = 0;

        apps_aws_sync_event_decode_extra(aws_data_ind, &event_group, &event_id, &p_extra_data, &extra_data_len);
        if (role == BT_AWS_MCE_ROLE_PARTNER
            && event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
            && event_id == APPS_EVENTS_INTERACTION_SYNC_APP_CONN_EVENT) {
            app_bt_conn_aws_data_t aws_data = {0};
            memcpy(&aws_data, (uint8_t *)p_extra_data, sizeof(app_bt_conn_aws_data_t));
            uint8_t state = app_bt_conn_manager_ctx.state;
            APPS_LOG_MSGID_I(LOG_TAG" AWS_DATA Partner handle, state=%d type=%d event/edr_num=%d",
                             3, state, aws_data.type, aws_data.data);
            if (aws_data.type == APP_BT_CONN_AWS_SYNC_EVENT) {
                uint8_t event = aws_data.data;
                app_bt_conn_mgr_machine_run(event);
            } else if (aws_data.type == APP_BT_CONN_AWS_SYNC_EDR_NUM) {
                uint8_t edr_connected_num = aws_data.data;
                uint8_t cur_edr_num = app_bt_conn_mgr_get_conn_num(TRUE, TRUE);
                APPS_LOG_MSGID_I(LOG_TAG" AWS_DATA Partner handle, peer_edr_num=%d own_edr_num=%d",
                                 2, edr_connected_num, cur_edr_num);

                if (edr_connected_num == cur_edr_num) {
                    // do nothing
                    APPS_LOG_MSGID_I(LOG_TAG" AWS_DATA Partner handle, do nothing", 0);
                } else if (edr_connected_num < cur_edr_num) {
                    for (int i = 0; i < (cur_edr_num - edr_connected_num); i++) {
                        app_bt_conn_mgr_machine_run(APP_BT_CONN_EVENT_EDR_DISCONNECTED);
                    }
                } else {
                    for (int i = 0; i < (edr_connected_num - cur_edr_num); i++) {
                        app_bt_conn_mgr_machine_run(APP_BT_CONN_EVENT_EDR_CONNECTED);
                    }
                }
            }
            ret = TRUE;
        } else if (event_group == EVENT_GROUP_UI_SHELL_APP_INTERACTION
                && event_id == APPS_EVENTS_INTERACTION_SYNC_APP_CONN_STATE) {
            uint8_t peer_state = *((uint8_t *)p_extra_data);
            uint8_t old_peer_state = app_bt_conn_manager_ctx.peer_state;
            app_bt_conn_manager_ctx.peer_state = peer_state;

            uint8_t own_lea_num = app_bt_conn_mgr_get_conn_num(TRUE, FALSE);
            uint8_t peer_lea_num = app_bt_conn_mgr_get_conn_num(FALSE, FALSE);
            APPS_LOG_MSGID_I(LOG_TAG" AWS_DATA [%02X] handle, state=%d peer_state=%d->%d own_lea_num=%d peer_lea_num=%d",
                             6, role, app_bt_conn_manager_ctx.state, old_peer_state, peer_state, own_lea_num, peer_lea_num);

            // Earbuds sync EDR conn state, but not sync LEA state
            // Agent should limit EDR Discoverable & BT CM according to peer larger LEA Conn num
            if (role == BT_AWS_MCE_ROLE_AGENT
                && app_bt_conn_manager_ctx.state != peer_state
                && own_lea_num < peer_lea_num) {
                if (peer_state == APP_BT_CONN_STATE_ONE_LEA
                 || peer_state == APP_BT_CONN_STATE_LEA_EDR) {
                    uint8_t bt_cm_num = app_bt_conn_mgr_get_bt_cm_num(1);
                    bt_status_t bt_status = bt_cm_set_max_connection_number(bt_cm_num, app_bt_conn_manager_phone_addr, 0, FALSE);
                    if (bt_status == BT_STATUS_SUCCESS) {
                        app_bt_conn_manager_ctx.bt_cm_num = bt_cm_num;
                    }
                    APPS_LOG_MSGID_I(LOG_TAG" AWS_DATA [%02X] handle, peer ONE_LEA/LEA_EDR bt_cm_num=%d bt_status=0x%08X",
                                     3, role, bt_cm_num, bt_status);
                } else if (peer_state == APP_BT_CONN_STATE_TWO_LEA) {
                    app_bt_conn_mgr_disallow_edr();
                    APPS_LOG_MSGID_I(LOG_TAG" AWS_DATA [%02X] handle, peer TWO_LEA disallow edr",
                                     1, role);
                }
            }

            if (role == BT_AWS_MCE_ROLE_AGENT
                && old_peer_state == APP_BT_CONN_STATE_TWO_LEA
                && peer_state != APP_BT_CONN_STATE_TWO_LEA
                && app_bt_conn_manager_ctx.state != APP_BT_CONN_STATE_TWO_LEA) {
                app_bt_conn_mgr_enable_page_scan(TRUE);
            }
        }
    }
    return ret;
}
#endif



/**============================================================================*/
/**                                 PUBLIC API                                 */
/**============================================================================*/
uint8_t app_bt_conn_manager_get_bt_cm_num(void)
{
    return app_bt_conn_manager_ctx.bt_cm_num;
}

bool app_bt_conn_manager_exist_le_audio(void)
{
    bool exist_le_audio = FALSE;
    if (app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_ONE_LEA
        || app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_LEA_EDR
        || app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_TWO_LEA) {
        exist_le_audio = TRUE;
    }

    if (!exist_le_audio) {
#if MTK_AWS_MCE_ENABLE
        if (app_bt_conn_manager_ctx.peer_state == APP_BT_CONN_STATE_ONE_LEA
            || app_bt_conn_manager_ctx.peer_state == APP_BT_CONN_STATE_LEA_EDR
            || app_bt_conn_manager_ctx.peer_state == APP_BT_CONN_STATE_TWO_LEA) {
            exist_le_audio = TRUE;
        }
#endif
    }

#ifdef APP_CONN_MGR_RECONNECT_CONTROL
    if (!exist_le_audio) {
        if (app_bt_conn_reconnect_flag && app_bt_conn_reconnect_info.lea_num > 0) {
            exist_le_audio = TRUE;
            APPS_LOG_MSGID_I(LOG_TAG"[Reconnect] allow_le_audio, flag=%d lea_num=%d",
                             2, exist_le_audio, app_bt_conn_reconnect_info.lea_num);
        }
    }
#endif
    return exist_le_audio;
}

bool app_bt_conn_manager_allow_le_audio(void)
{
    bool ret = FALSE;
    bool support_lea_emp = FALSE;
#ifdef AIR_LE_AUDIO_MULTIPOINT_ENABLE
    if (app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_NONE
        || app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_ONE_LEA
        || app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_ONE_EDR) {
        ret = TRUE;
    }
    support_lea_emp = TRUE;
#else
    if (app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_NONE
        || app_bt_conn_manager_ctx.state == APP_BT_CONN_STATE_ONE_EDR) {
        ret = TRUE;
    }
#endif
    APPS_LOG_MSGID_I(LOG_TAG" allow_le_audio, ret=%d state=%d support_lea_emp=%d",
                     3, ret, app_bt_conn_manager_ctx.state, support_lea_emp);
    return ret;
}

bool app_bt_conn_manager_allow_edr_visible(void)
{
    bool ret = TRUE;
#ifdef MTK_AWS_MCE_ENABLE
    ret = (app_bt_conn_manager_ctx.state != APP_BT_CONN_STATE_TWO_LEA
           && app_bt_conn_manager_ctx.peer_state != APP_BT_CONN_STATE_TWO_LEA);
#else
    ret = (app_bt_conn_manager_ctx.state != APP_BT_CONN_STATE_TWO_LEA);
#endif
    if (!ret) {
        APPS_LOG_MSGID_I(LOG_TAG" disallow EDR discoverable", 0);
    }
    return ret;
}

bool app_bt_conn_manager_activity_proc(struct _ui_shell_activity *self,
                                       uint32_t event_group,
                                       uint32_t event_id,
                                       void *extra_data,
                                       size_t data_len)
{
    bool ret = FALSE;
    switch (event_group) {
        case EVENT_GROUP_UI_SHELL_SYSTEM:
            ret = app_bt_conn_mgr_activity_proc_ui_shell_group(event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_CONN_MANAGER:
            ret = app_bt_conn_mgr_activity_bt_cm_event_group(event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_DEVICE_MANAGER:
            ret = app_bt_conn_mgr_activity_bt_dm_event_group(event_id, extra_data, data_len);
            break;
        case EVENT_GROUP_UI_SHELL_BT_SINK:
            ret = app_bt_conn_mgr_activity_bt_sink_event_group(event_id, extra_data, data_len);
            break;
#ifdef MTK_AWS_MCE_ENABLE
        case EVENT_GROUP_UI_SHELL_AWS_DATA:
            ret = app_bt_conn_mgr_activity_proc_aws_data(extra_data, data_len);
            break;
#endif
    }
    return ret;
}

#endif  /* AIR_LE_AUDIO_ENABLE && (AIR_MULTI_POINT_ENABLE || AIR_LE_AUDIO_MULTIPOINT_ENABLE) */
