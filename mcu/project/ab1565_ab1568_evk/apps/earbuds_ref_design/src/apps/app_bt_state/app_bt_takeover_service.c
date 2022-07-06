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
#include "apps_debug.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"
#include "bt_sink_srv.h"
#include "app_bt_takeover_service.h"
#include "app_home_screen_idle_activity.h"
#include "app_bt_conn_componet_in_homescreen.h"

#define LOG_TAG     "[app_bt_takeover_service]"

static app_bt_takeover_service_allow_func_t app_bt_takeover_user[APP_BT_TAKEOVER_ID_MAX] = {0};
static bt_bd_addr_t select_addr = {0};

bool app_bt_takeover_service_user_register(app_bt_takeover_user_id_t user_id, app_bt_takeover_service_allow_func_t func)
{
    if(user_id < APP_BT_TAKEOVER_ID_MAX) {
        app_bt_takeover_user[user_id] = func;
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_takeover_service_user_register, user ID %d", 1, user_id);
        return true;
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" app_bt_takeover_service_user_register, user not found", 0);
        return false;
    }
}

bool app_bt_takeover_service_user_deregister(app_bt_takeover_user_id_t user_id)
{
    if(user_id < APP_BT_TAKEOVER_ID_MAX) {
        app_bt_takeover_user[user_id] = NULL;
        APPS_LOG_MSGID_I(LOG_TAG" app_bt_takeover_service_user_deregister, user ID %d", 1, user_id);
        return true;
    } else {
        APPS_LOG_MSGID_E(LOG_TAG" app_bt_takeover_service_user_deregister, user not found", 0);
        return false;
    }
}

static bool app_bt_takeover_service_bt_sink(const bt_bd_addr_t remote_addr)
{
    bool ret = true;
    bt_sink_srv_device_state_t state = {{0}, 0, 0, 0};

    if (bt_sink_srv_get_device_state((const bt_bd_addr_t *)remote_addr, &state, 1) == 1) {
        if (state.music_state == BT_SINK_SRV_STATE_STREAMING ||
                state.call_state > BT_SINK_SRV_STATE_STREAMING || state.sco_state == BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED) {
            APPS_LOG_MSGID_I(LOG_TAG" bt_sink not allow, a2dp %d, hfp %d, esco %d", 3, state.music_state, state.call_state, state.sco_state);
            ret = false;
        }
    }

    return ret;
}

void app_bt_takeover_service_init()
{
    app_bt_takeover_service_user_register(APP_BT_TAKEOVER_ID_BTSINK, app_bt_takeover_service_bt_sink);
}


bt_bd_addr_t *bt_cm_get_takeover_disconnect_device(void)
{
    uint32_t num = 0;
    bt_bd_addr_t zero_addr = {0};
    bt_bd_addr_t conn_addr[APP_CONN_MAX_DEVICE_NUM + 1];
    uint8_t conn_index;
#ifdef AIR_MULTI_POINT_ENABLE
    uint8_t user_index;
    app_bt_takeover_service_allow_func_t user_func = NULL;
#endif

    memset(&conn_addr, 0, sizeof(bt_bd_addr_t) * (APP_CONN_MAX_DEVICE_NUM + 1));
    num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, conn_addr, APP_CONN_MAX_DEVICE_NUM + 1);

    if (num == 0) {
        APPS_LOG_MSGID_E(LOG_TAG" bt_cm_takeover, no device connected", 0);
        return NULL;
    }
    APPS_LOG_MSGID_I(LOG_TAG" bt_cm_get_takeover_disconnect_device, conn num %d", 1, num);

    for (conn_index = 0; conn_index < num; conn_index ++) {
        APPS_LOG_MSGID_I(LOG_TAG" bt_cm_takeover check %x:%x:%x:%x:%x:%x", 6, conn_addr[conn_index][5], conn_addr[conn_index][4],
                conn_addr[conn_index][3], conn_addr[conn_index][2], conn_addr[conn_index][1], conn_addr[conn_index][0]);

        if (memcmp(bt_device_manager_get_local_address(), &(conn_addr[conn_index]), sizeof(bt_bd_addr_t)) == 0) {
            APPS_LOG_MSGID_I(LOG_TAG" bt_cm_takeover local address, phase out", 0);
            memset(conn_addr[conn_index], 0, sizeof(bt_bd_addr_t));
            continue;
        }

#ifdef AIR_MULTI_POINT_ENABLE
        for (user_index = 0; user_index < APP_BT_TAKEOVER_ID_MAX; user_index ++) {
            user_func = app_bt_takeover_user[user_index];
            if(user_func && !user_func(conn_addr[conn_index])) {
                APPS_LOG_MSGID_I(LOG_TAG" Not allow by user %d", 1, user_index);
                memset(conn_addr[conn_index], 0, sizeof(bt_bd_addr_t));
                break;
            }
        }
#endif

    }


    conn_index = 0;
    do {
        if (memcmp(conn_addr[conn_index], zero_addr, sizeof(bt_bd_addr_t)) != 0) {
            break;
        } else {
            conn_index ++;
        }
    } while(conn_index < num);

    if (conn_index == num) {
        APPS_LOG_MSGID_I(LOG_TAG" bt_cm_get_takeover_disconnect_device, not allow", 0);
        return NULL;
    } else {
        memcpy(&select_addr, conn_addr[conn_index], sizeof(bt_bd_addr_t));
        APPS_LOG_MSGID_I(LOG_TAG" bt_cm_takeover select %x:%x:%x:%x:%x:%x", 6, select_addr[5], select_addr[4],
                select_addr[3], select_addr[2], select_addr[1], select_addr[0]);
        return &select_addr;
    }
}
