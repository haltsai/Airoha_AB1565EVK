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
 * File: race_app_bt_event_hdl.c
 *
 * Description: This file processes the RHO events and other BT events that FOTA concerns.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#include "race_cmd_feature.h"
#include "bt_aws_mce.h"
#include "bt_sink_srv.h"
#include "bt_avrcp.h"
#include "syslog.h"
#include "race_event.h"
#include "race_app_bt_event_hdl.h"
#include "race_fota_util.h"
#include "race_xport.h"
#include "race_lpcomm_util.h"
#include "race_fota.h"
#include "race_bt.h"
#include "bt_app_common.h"
#include "bt_device_manager.h"
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
#include "bt_role_handover.h"
#endif
#include "race_app_bt_event_hdl.h"
#include "race_app_aws_event_hdl.h"
#include "race_app_race_event_hdl.h"
#include "race_app_cosys_hdl.h"


log_create_module(race_app_bt, PRINT_LEVEL_INFO);

#ifndef MTK_DEBUG_LEVEL_NONE
#define RACE_APP_BT_LOG_E(fmt,arg...)         LOG_E(race_app_bt, fmt,##arg)
#define RACE_APP_BT_LOG_W(fmt,arg...)         LOG_W(race_app_bt, fmt,##arg)
#define RACE_APP_BT_LOG_I(fmt,arg...)         LOG_I(race_app_bt, fmt,##arg)
#define RACE_APP_BT_LOG_D(fmt,arg...)

#define RACE_APP_BT_LOG_MSGID_E(fmt,arg...)   LOG_MSGID_E(race_app_bt, fmt,##arg)
#define RACE_APP_BT_LOG_MSGID_W(fmt,arg...)   LOG_MSGID_W(race_app_bt, fmt,##arg)
#define RACE_APP_BT_LOG_MSGID_I(fmt,arg...)   LOG_MSGID_I(race_app_bt, fmt,##arg)
#define RACE_APP_BT_LOG_MSGID_D(fmt,arg...)
#else
#define RACE_APP_BT_LOG_E(fmt,arg...)
#define RACE_APP_BT_LOG_W(fmt,arg...)
#define RACE_APP_BT_LOG_I(fmt,arg...)

#define RACE_APP_BT_LOG_MSGID_E(fmt,arg...)
#define RACE_APP_BT_LOG_MSGID_W(fmt,arg...)
#define RACE_APP_BT_LOG_MSGID_I(fmt,arg...)
#define RACE_APP_BT_LOG_MSGID_D(fmt,arg...)
#endif


typedef enum {
    RACE_APP_BT_CM_PROFILE_EVENT_NONE,

    RACE_APP_BT_CM_PROFILE_EVENT_CONNECTED,
    RACE_APP_BT_CM_PROFILE_EVENT_DISCONNECTED,

    RACE_APP_BT_CM_PROFILE_EVENT_MAX
} race_app_bt_cm_profile_event_enum;


#if defined(RACE_ROLE_HANDOVER_SERVICE_ENABLE) && defined(RACE_LPCOMM_ENABLE)
bt_status_t race_app_rho_srv_allow_execution_callback(const bt_bd_addr_t *addr);

bt_status_t race_app_rho_srv_get_data_callback(const bt_bd_addr_t *addr, void *data);

uint8_t race_app_rho_srv_get_length_callback(const bt_bd_addr_t *addr);

bt_status_t race_app_rho_srv_update_callback(bt_role_handover_update_info_t *info);

void race_app_rho_srv_status_callback(const bt_bd_addr_t *addr,
                                      bt_aws_mce_role_t role,
                                      bt_role_handover_event_t event,
                                      bt_status_t status);
#endif /* RACE_ROLE_HANDOVER_SERVICE_ENABLE && RACE_LPCOMM_ENABLE */


race_app_bt_cntx_struct g_race_app_bt_cntx; /* The context variable. */


#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
uint32_t race_app_fota_active_mode_get_actions(void)
{
	uint32_t action = RACE_FOTA_ACTIVE_MODE_ACTION_DSP_SUSPEND;

#ifndef RACE_FOTA_ACTIVE_MODE_KEEP_HFP
	action |= RACE_FOTA_ACTIVE_MODE_ACTION_HFP_DISCONNECT;
#endif

#ifndef RACE_FOTA_ACTIVE_MODE_KEEP_A2DP
	action |= RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_DISCONNECT;
#else
	action |= RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_PAUSE;
#endif

	RACE_LOG_MSGID_I("active FOTA action:%x", 1, action);

	return action;
}


bool race_app_fota_is_profile_disconnect_needed(bt_cm_profile_service_t profile_type)
{
    uint32_t actions = race_app_fota_active_mode_get_actions();

    if ((BT_CM_PROFILE_SERVICE_HFP == profile_type &&
         !(RACE_FOTA_ACTIVE_MODE_ACTION_HFP_DISCONNECT & actions)) ||
        (BT_CM_PROFILE_SERVICE_A2DP_SINK == profile_type &&
         !(RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_DISCONNECT & actions)) ||
        (BT_CM_PROFILE_SERVICE_HFP != profile_type &&
         BT_CM_PROFILE_SERVICE_A2DP_SINK != profile_type) ||
        g_race_app_bt_cntx.is_rho_on_going) {
        return FALSE;
    }

    if (!race_fota_is_running(TRUE)) {
        return FALSE;
    }

    if (race_fota_is_active_mode()) {
        RACE_LOG_MSGID_I("disconnect needed. profile_type:%x", 1, profile_type);
        return TRUE;
    }

    return FALSE;
}


bool race_app_fota_is_profile_connect_needed(bt_cm_profile_service_t profile_type)
{
    uint32_t actions = race_app_fota_active_mode_get_actions();

    /* If the HFP or the A2DP is not disconnected by the active mode FOTA, there is no need to connect it. */
    if ((BT_CM_PROFILE_SERVICE_HFP == profile_type &&
         !(RACE_FOTA_ACTIVE_MODE_ACTION_HFP_DISCONNECT & actions)) ||
        (BT_CM_PROFILE_SERVICE_A2DP_SINK == profile_type &&
         !(RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_DISCONNECT & actions)) ||
        (BT_CM_PROFILE_SERVICE_HFP != profile_type &&
         BT_CM_PROFILE_SERVICE_A2DP_SINK != profile_type) ||
        g_race_app_bt_cntx.is_rho_on_going) {
        return FALSE;
    }

    if (!race_fota_is_running(TRUE)) {
        RACE_LOG_MSGID_I("connect needed. profile_type:%x", 1, profile_type);
        return TRUE;
    }

    if (!race_fota_is_active_mode()) {
        RACE_LOG_MSGID_I("connect needed. profile_type:%x", 1, profile_type);
        return TRUE;
    }

    return FALSE;
}
#endif


#if defined(RACE_ROLE_HANDOVER_SERVICE_ENABLE) && defined(RACE_LPCOMM_ENABLE)
bt_role_handover_callbacks_t race_app_rho_callbacks = {race_app_rho_srv_allow_execution_callback,
                                                       race_app_rho_srv_get_length_callback,
                                                       race_app_rho_srv_get_data_callback,
                                                       race_app_rho_srv_update_callback,
                                                       race_app_rho_srv_status_callback
                                                      }; /* The variable for the RHO callbacks. */
#endif


bt_sink_srv_state_t race_app_bt_get_a2dp_state(void)
{
    return g_race_app_bt_cntx.a2dp_state;
}


bt_avrcp_status_t race_app_bt_get_avrcp_state(void)
{
    return g_race_app_bt_cntx.avrcp_state;
}


#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
bool race_app_fota_is_device_connected(bt_bd_addr_t *bt_address)
{
    bt_bd_addr_t addr_list[RACE_APP_BT_SP_CONNECTED_MAX_COUNT];
    uint32_t i = 0, count = 0;

    if (!bt_address) {
        return FALSE;
    }

    memset(addr_list, 0, RACE_APP_BT_SP_CONNECTED_MAX_COUNT * sizeof(bt_bd_addr_t));
    /* Get the address list of the devices connected. */
    count = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE,
                                        addr_list,
                                        RACE_APP_BT_SP_CONNECTED_MAX_COUNT);
    if (RACE_APP_BT_SP_CONNECTED_MAX_COUNT < count) {
        count = RACE_APP_BT_SP_CONNECTED_MAX_COUNT;
        RACE_APP_BT_LOG_MSGID_W("Too many SP connected. count:%d", 1, count);
    }

    for (i = 0; i < count; i++) {
        if (0 == memcmp(bt_address, &addr_list[i], sizeof(bt_bd_addr_t))) {
            RACE_APP_BT_LOG_MSGID_I("Device is connected. bt_address:%x %x %x %x %x %x", 6,
                                    bt_address[0], bt_address[1], bt_address[2],
                                    bt_address[3], bt_address[4], bt_address[5]);
            return TRUE;
        }
    }

    return FALSE;
}


race_app_bt_profile_info_struct *race_app_fota_get_disc_profile_info_list(bt_cm_profile_service_t profile_type)
{
    switch (profile_type) {
#ifndef RACE_FOTA_ACTIVE_MODE_KEEP_HFP
        case BT_CM_PROFILE_SERVICE_HFP: {
            /* The HFP profile. */
            return &g_race_app_bt_cntx.fota_disc_hfp_info[0];
        }
#endif

#ifndef RACE_FOTA_ACTIVE_MODE_KEEP_A2DP
        case BT_CM_PROFILE_SERVICE_A2DP_SINK: {
            /* The A2DP profile. */
            return &g_race_app_bt_cntx.fota_disc_a2dp_info[0];
        }
#endif

        default: {
            break;
        }
    }

    return NULL;
}


RACE_ERRCODE race_app_fota_find_disc_profile_info_by_address(race_app_bt_profile_info_struct **disc_profile_info,
                                                             bt_cm_profile_service_t profile_type,
                                                             bt_bd_addr_t *bt_address)
{
    uint32_t i = 0;
    race_app_bt_profile_info_struct *profile_info = NULL;
    race_app_bt_profile_info_struct *profile_info_list = race_app_fota_get_disc_profile_info_list(profile_type);

    if (!disc_profile_info || *disc_profile_info || !bt_address ||
        !profile_info_list) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    for (i = 0; i < RACE_APP_BT_SP_CONNECTED_MAX_COUNT; i++) {
        if (profile_info_list[i].is_used &&
            0 == memcmp(&profile_info_list[i].address, bt_address, sizeof(bt_bd_addr_t))) {
            profile_info = &profile_info_list[i];
            break;
        }
    }

    if (!profile_info) {
        RACE_APP_BT_LOG_MSGID_W("Not Found. bt_address:%x %x %x %x %x %x", 6,
                                bt_address[0], bt_address[1], bt_address[2],
                                bt_address[3], bt_address[4], bt_address[5]);
    } else {
        RACE_APP_BT_LOG_MSGID_I("Found. profile_state:%d bt_address:%x %x %x %x %x %x", 7,
                                profile_info->profile_state,
                                bt_address[0], bt_address[1], bt_address[2],
                                bt_address[3], bt_address[4], bt_address[5]);
    }

    *disc_profile_info = profile_info;

    return profile_info ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_NOT_ENOUGH_MEMORY;
}


RACE_ERRCODE race_app_fota_remove_disc_profile_info(race_app_bt_profile_info_struct *disc_profile_info)
{
    if (!disc_profile_info) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    RACE_APP_BT_LOG_MSGID_I("Remove profile info. profile_state:%d bt_address:%x %x %x %x %x %x", 7,
                            disc_profile_info->profile_state,
                            disc_profile_info->address[0], disc_profile_info->address[1], disc_profile_info->address[2],
                            disc_profile_info->address[3], disc_profile_info->address[4], disc_profile_info->address[5]);

    disc_profile_info->is_used = FALSE;

    return RACE_ERRCODE_SUCCESS;
}


/**
* @brief      This function inserts the information of the profile disconnected by the active mode FOTA into the fota_disc_[profile_type]_info
* of g_race_app_bt_cntx.
* @param[out] disc_profile_info, if succeed, the newly inserted profile information. Otherwise, NULL.
* @param[in] profile_type, the profile type.
* @param[in] bt_address, the BR/EDR address of the SP that the profile disconnected with.
* @return     If succeed, RACE_ERRCODE_SUCCESS. Otherwise, other values.
*/
RACE_ERRCODE race_app_fota_insert_disc_profile_info(race_app_bt_profile_info_struct **disc_profile_info,
                                                    bt_cm_profile_service_t profile_type,
                                                    bt_bd_addr_t *bt_address)
{
    uint32_t i = 0, first_free_slot = RACE_APP_BT_SP_CONNECTED_MAX_COUNT;
    race_app_bt_profile_info_struct *profile_info = NULL;
    race_app_bt_profile_info_struct *profile_info_list = race_app_fota_get_disc_profile_info_list(profile_type);

    if (!disc_profile_info || *disc_profile_info || !bt_address ||
        !profile_info_list) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    for (i = 0; i < RACE_APP_BT_SP_CONNECTED_MAX_COUNT; i++) {
        if (!profile_info_list[i].is_used &&
            RACE_APP_BT_SP_CONNECTED_MAX_COUNT == first_free_slot) {
            first_free_slot = i;
            continue;
        }

        if (profile_info_list[i].is_used &&
            0 == memcmp(&profile_info_list[i].address, bt_address, sizeof(bt_bd_addr_t))) {
            RACE_APP_BT_LOG_MSGID_W("Already exists. idx:%d", 1, i);
            profile_info = &profile_info_list[i];
            break;
        }
    }

    if (i >= RACE_APP_BT_SP_CONNECTED_MAX_COUNT) {
        if (RACE_APP_BT_SP_CONNECTED_MAX_COUNT > first_free_slot) {
            memset(&profile_info_list[first_free_slot], 0, sizeof(race_app_bt_profile_info_struct));
            memcpy(&profile_info_list[first_free_slot].address, bt_address, sizeof(bt_bd_addr_t));
            profile_info_list[first_free_slot].profile_state = RACE_APP_BT_PROFILE_STATE_NONE;
            profile_info_list[first_free_slot].is_used = TRUE;
            profile_info = &profile_info_list[first_free_slot];
        }
    }

    *disc_profile_info = profile_info;

    return profile_info ? RACE_ERRCODE_SUCCESS : RACE_ERRCODE_NOT_ENOUGH_MEMORY;
}


void race_app_fota_clear_unconn_sp_disc_profile_info(bt_cm_profile_service_t profile_type)
{
    bt_bd_addr_t addr_list[RACE_APP_BT_SP_CONNECTED_MAX_COUNT];
    uint32_t i = 0, j = 0, count = 0;
    race_app_bt_profile_info_struct *profile_info_list = race_app_fota_get_disc_profile_info_list(profile_type);

    RACE_APP_BT_LOG_MSGID_I("race_app_fota_clear_unconn_sp_disc_profile_info profile_type:%x", 1, profile_type);
    if (!profile_info_list) {
        return;
    }

    memset(addr_list, 0, RACE_APP_BT_SP_CONNECTED_MAX_COUNT * sizeof(bt_bd_addr_t));
    /* Get the address list of the devices connected. */
    count = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE,
                                        addr_list,
                                        RACE_APP_BT_SP_CONNECTED_MAX_COUNT);
    if (RACE_APP_BT_SP_CONNECTED_MAX_COUNT < count) {
        count = RACE_APP_BT_SP_CONNECTED_MAX_COUNT;
        RACE_APP_BT_LOG_MSGID_W("Too many SP connected. count:%d", 1, count);
    }

    /* Remove the profile information the device of which is not connected. */
    for (i = 0; i < RACE_APP_BT_SP_CONNECTED_MAX_COUNT; i++) {
        if (profile_info_list[i].is_used) {
            for (j = 0; j < count; j++) {
                if (0 == memcmp(&profile_info_list[i].address, &addr_list[j], sizeof(bt_bd_addr_t))) {
                    break;
                }
            }

            if (j >= count) {
                RACE_APP_BT_LOG_MSGID_I("SP is not connected. Remove the profile info", 0);
                race_app_fota_remove_disc_profile_info(&profile_info_list[i]);
            }
        }
    }
}


RACE_ERRCODE race_app_fota_smart_insert_disc_profile_info(race_app_bt_profile_info_struct **disc_profile_info,
                                                          bt_cm_profile_service_t profile_type,
                                                          bt_bd_addr_t *bt_address)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    RACE_APP_BT_LOG_MSGID_I("smart_insert profile_type:%x bt_address: %x %x %x %x %x %x", 7,
                            profile_type,
                            bt_address[0], bt_address[1], bt_address[2],
                            bt_address[3], bt_address[4], bt_address[5]);

    ret = race_app_fota_insert_disc_profile_info(disc_profile_info, profile_type, bt_address);
    /* If the insertion fails, remove the profile information the device of which is not connected and try again.  */
    if (RACE_ERRCODE_SUCCESS != ret || !disc_profile_info) {
        race_app_fota_clear_unconn_sp_disc_profile_info(profile_type);
        ret = race_app_fota_insert_disc_profile_info(disc_profile_info, profile_type, bt_address);
    }

    RACE_APP_BT_LOG_MSGID_I("Insert profile info ret:%d profile_type:%x bt_address: %x %x %x %x %x %x", 8,
                            ret, profile_type,
                            bt_address[0], bt_address[1], bt_address[2],
                            bt_address[3], bt_address[4], bt_address[5]);
    return ret;
}


/**
* @brief      This function can only be used on receiving BT_ROLE_HANDOVER_PREPARE_REQ_IND event. It changes the
* ING unstable state to the stable state and removes the invalid profile information.
* @param[in] profile_type, the profile type.
*/
void race_app_fota_update_disc_profile_info(bt_cm_profile_service_t profile_type)
{
    uint32_t i = 0;
    race_app_bt_profile_info_struct *profile_info_list = race_app_fota_get_disc_profile_info_list(profile_type);
    bt_cm_profile_service_mask_t connected_profile_types = BT_CM_PROFILE_SERVICE_MASK_NONE;

    RACE_APP_BT_LOG_MSGID_I("update_disc_profile_info profile_type:%x", 1, profile_type);
    if (!profile_info_list) {
        return;
    }

    race_app_fota_clear_unconn_sp_disc_profile_info(profile_type);

    for (i = 0; i < RACE_APP_BT_SP_CONNECTED_MAX_COUNT; i++) {
        if (profile_info_list[i].is_used) {
            /* profile_info_list[i].profile_state:
             * RACE_APP_BT_PROFILE_STATE_NONE: Remove the profile info. Invalid state
             * RACE_APP_BT_PROFILE_STATE_CONNECTING: Remove the profile info. If it's CONNECTED now,
             *   remove it. If it's DISCONNECTED now, it's not disconnected by FOTA app and also remove it.
             * RACE_APP_BT_PROFILE_STATE_CONNECTED: Remove the profile info. Invalid state
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTING: If it's DISCONNECTED now, update the state to
             *   DISCONNECTED. If it's CONNECTED now, disconnection failed or it's not re-connected by FOTA app.
             *   Therefore, remove it.
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTED: If it's DISCONNECTED now, do nothing. If it's
             *   CONNECTED now, disconnection failed or it's not re-connected by FOTA app. Therefore, remove it.
             * RACE_APP_BT_PROFILE_STATE_MAX: Remove the profile info. Invalid state
             */
            if (RACE_APP_BT_PROFILE_STATE_DISCONNECTING == profile_info_list[i].profile_state ||
                RACE_APP_BT_PROFILE_STATE_DISCONNECTED == profile_info_list[i].profile_state) {
                connected_profile_types = bt_cm_get_connected_profile_services(profile_info_list[i].address);
                RACE_APP_BT_LOG_MSGID_I("update_disc_profile_info connected_profile_types:%x", 1, connected_profile_types);

                if (connected_profile_types & BT_CM_PROFILE_SERVICE_MASK(profile_type)) {
                    race_app_fota_remove_disc_profile_info(&profile_info_list[i]);
                } else {
                    profile_info_list[i].profile_state = RACE_APP_BT_PROFILE_STATE_DISCONNECTED;
                }
            } else {
                race_app_fota_remove_disc_profile_info(&profile_info_list[i]);
            }
        }
    }
}


RACE_ERRCODE race_app_fota_active_mode_disc_profile(bt_cm_profile_service_t profile_type,
                                                    bt_bd_addr_t *bt_address)
{
    race_app_bt_profile_info_struct *disc_profile_info = NULL;
    bt_cm_connect_t parameters;
    bt_status_t bt_ret = BT_STATUS_SUCCESS;
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    bt_cm_profile_service_mask_t connected_profile_types = BT_CM_PROFILE_SERVICE_MASK_NONE;

    if (!bt_address) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    connected_profile_types = bt_cm_get_connected_profile_services(*bt_address);
    RACE_APP_BT_LOG_MSGID_I("active_mode_disc_profile_revert profile_type:%x connected_profile_types:%x", 2,
                            profile_type, connected_profile_types);

    if (g_race_app_bt_cntx.is_rho_on_going) {
        RACE_APP_BT_LOG_MSGID_W("Do not disconnect profile_type:%x for RHO is on-going.", 1, profile_type);
        return RACE_ERRCODE_FAIL;
    }

    race_app_fota_find_disc_profile_info_by_address(&disc_profile_info,
                                                    profile_type,
                                                    bt_address);

    if (!(connected_profile_types & BT_CM_PROFILE_SERVICE_MASK(profile_type))) {
        /* Profile is disconnected. */
        if (disc_profile_info) {
            /* disc_profile_info->profile_state:
             * RACE_APP_BT_PROFILE_STATE_NONE: Remove the profile info. Invalid state.
             * RACE_APP_BT_PROFILE_STATE_CONNECTING: Do nothing. Process in the SINK SRV CONNECTED event latter.
             * RACE_APP_BT_PROFILE_STATE_CONNECTED: Remove the profile info. Invalid state.
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTING: Do nothing. Process in the SINK SRV DISCONNECTED event latter.
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTED: Do nothing.
             * RACE_APP_BT_PROFILE_STATE_MAX: Remove the profile info. Invalid state.
             */
            if (RACE_APP_BT_PROFILE_STATE_NONE == disc_profile_info->profile_state ||
                RACE_APP_BT_PROFILE_STATE_MAX == disc_profile_info->profile_state ||
                RACE_APP_BT_PROFILE_STATE_CONNECTED == disc_profile_info->profile_state) {
                race_app_fota_remove_disc_profile_info(disc_profile_info);
            }
        }
        /*else Do nothing. */
    } else {
        /* Profile is connected. */
        if (!disc_profile_info) {
            race_app_fota_smart_insert_disc_profile_info(&disc_profile_info,
                                                         profile_type,
                                                         bt_address);
        }

        memcpy(&parameters.address, bt_address, sizeof(bt_bd_addr_t));
        parameters.profile = BT_CM_PROFILE_SERVICE_MASK((uint8_t)profile_type);

        if (disc_profile_info) {
            RACE_APP_BT_LOG_MSGID_I("disc_profile_info:%x profile_state:%d", 2, disc_profile_info, disc_profile_info->profile_state);
            /* disc_profile_info->profile_state:
             * RACE_APP_BT_PROFILE_STATE_NONE: Newly inserted. Disconnect the profile and change state to Disconnecting.
             * RACE_APP_BT_PROFILE_STATE_CONNECTING: Do nothing. Process in the SINK SRV CONNECTED event latter.
             * RACE_APP_BT_PROFILE_STATE_CONNECTED: Disconnect the profile and change state to Disconnecting. Invalid state.
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTING: Do nothing. Process in the SINK SRV DISCONNECTED event latter.
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTED: Do nothing.
             * RACE_APP_BT_PROFILE_STATE_MAX: Disconnect the profile and change state to Disconnecting. Invalid state.
             */
            if (RACE_APP_BT_PROFILE_STATE_NONE == disc_profile_info->profile_state ||
                RACE_APP_BT_PROFILE_STATE_MAX == disc_profile_info->profile_state ||
                RACE_APP_BT_PROFILE_STATE_CONNECTED == disc_profile_info->profile_state) {
                bt_ret = bt_cm_disconnect((const bt_cm_connect_t *)&parameters);
                if (BT_STATUS_SUCCESS != bt_ret) {
                    ret = RACE_ERRCODE_FAIL;
                    race_app_fota_remove_disc_profile_info(disc_profile_info);
                    RACE_APP_BT_LOG_MSGID_W("Fail to disconnect profile_type:%x. bt_ret:%d", 2, profile_type, bt_ret);
                } else {
                    disc_profile_info->profile_state = RACE_APP_BT_PROFILE_STATE_DISCONNECTING;
                    RACE_APP_BT_LOG_MSGID_I("Disconnect profile_type:%x", 1, profile_type);
                }
            }
            /*else Do nothing. */
        } else {
            /* It will not be recovered if FOTA fails or is cancelled. */
            RACE_APP_BT_LOG_MSGID_W("Fail to insert the SP address into the array.", 0);

            bt_ret = bt_cm_disconnect((const bt_cm_connect_t *)&parameters);
            if (BT_STATUS_SUCCESS != bt_ret) {
                ret = RACE_ERRCODE_FAIL;
                RACE_APP_BT_LOG_MSGID_W("Fail to disconnect profile_type:%x. bt_ret:%d", 2, profile_type, bt_ret);
            } else {
                RACE_APP_BT_LOG_MSGID_I("Disconnect profile_type:%x", 1, profile_type);
            }
        }
    }

    return ret;
}


RACE_ERRCODE race_app_fota_active_mode_disc_profile_revert(bt_cm_profile_service_t profile_type,
                                                           bt_bd_addr_t *bt_address)
{
    race_app_bt_profile_info_struct *disc_profile_info = NULL;
    bt_cm_profile_service_mask_t connected_profile_types = BT_CM_PROFILE_SERVICE_MASK_NONE;
    bt_status_t bt_ret = BT_STATUS_SUCCESS;
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    if (!bt_address) {
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    RACE_APP_BT_LOG_MSGID_I("active_mode_disc_profile_revert profile_type:%x", 1,
                            profile_type);

    if (g_race_app_bt_cntx.is_rho_on_going) {
        RACE_APP_BT_LOG_MSGID_W("Do not connect profile_type:%x for RHO is on-going.", 1, profile_type);
        return RACE_ERRCODE_FAIL;
    }

    race_app_fota_find_disc_profile_info_by_address(&disc_profile_info,
                                                    profile_type,
                                                    bt_address);
    if (disc_profile_info) {
        connected_profile_types = bt_cm_get_connected_profile_services(disc_profile_info->address);

        RACE_APP_BT_LOG_MSGID_I("active_mode_disc_profile_revert connected_profile_types:%x", 1, connected_profile_types);
        /* Not sure if the profile_type is connected or not because the first input parameter of bt_cm_get_connected_devices() is 0.
         * disc_profile_info->profile_state:
         * RACE_APP_BT_PROFILE_STATE_NONE: Remove the profile info. Invalid state. Only when it's just inserted, the state will be NONE.
         * RACE_APP_BT_PROFILE_STATE_CONNECTING: Do nothing.
         * RACE_APP_BT_PROFILE_STATE_CONNECTED: Remove the profile info. Invalid state.
         * RACE_APP_BT_PROFILE_STATE_DISCONNECTING: Do nothing. Process on receiving SINK SRV DISC event.
         * RACE_APP_BT_PROFILE_STATE_DISCONNECTED: If it's connected, remove the profile. If not, connect it.
         * RACE_APP_BT_PROFILE_STATE_MAX: Remove the profile info. Invalid state.
         */
        if (RACE_APP_BT_PROFILE_STATE_CONNECTING != disc_profile_info->profile_state &&
            RACE_APP_BT_PROFILE_STATE_DISCONNECTING != disc_profile_info->profile_state) {
            if (RACE_APP_BT_PROFILE_STATE_DISCONNECTED == disc_profile_info->profile_state &&
                !(connected_profile_types & BT_CM_PROFILE_SERVICE_MASK(profile_type))) {
                bt_cm_connect_t param;

                memcpy(&param.address, bt_address, sizeof(bt_bd_addr_t));
                param.profile = BT_CM_PROFILE_SERVICE_MASK((uint8_t)profile_type);

                /* Only connect the indicated profile even if BREDR is not connected. */
                bt_ret = bt_cm_connect(&param);
                if (BT_STATUS_SUCCESS != bt_ret) {
                    ret = RACE_ERRCODE_FAIL;
                    race_app_fota_remove_disc_profile_info(disc_profile_info);
                    RACE_APP_BT_LOG_MSGID_W("Fail to connect profile_type:%x. bt_ret:%d", 2, profile_type, bt_ret);
                } else {
                    disc_profile_info->profile_state = RACE_APP_BT_PROFILE_STATE_CONNECTING;
                    RACE_APP_BT_LOG_MSGID_I("Connect profile_type:%x", 1, profile_type);
                }
            } else {
                race_app_fota_remove_disc_profile_info(disc_profile_info);
            }
        }
    } else {
        /* Do nothing, because it's not disconnected by race(FOTA) app. */
    }

    return ret;
}


/**
* @brief      This function tries to pause the A2DP. However, it may fail. When playing video, A2DP may not be able to be
* paused. When playing music, if user plays/pauses the music in the SP side frequently, A2DP may not be able to be paused either.
* @return     If succeed, RACE_ERRCODE_SUCCESS. Otherwise, other values.
*/
RACE_ERRCODE race_app_fota_active_mode_pause_a2dp(void)
{
    bt_status_t bt_ret = BT_STATUS_SUCCESS;
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

    if (g_race_app_bt_cntx.is_rho_on_going) {
        RACE_APP_BT_LOG_MSGID_W("Do not pause a2dp for RHO is on-going.", 0);
        return RACE_ERRCODE_FAIL;
    }

    /* Do not care about the A2DP or AVRCP states, because AVRCP's state machine will drop the
     * Pause action if it's not in A2DP streaming or AVRCP playing state.
     * Each time, there's only one device playing. Therefore, set the second parameter to be NULL.
     */
    bt_ret = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_PAUSE, NULL);
    if (BT_STATUS_SUCCESS != bt_ret) {
        RACE_APP_BT_LOG_MSGID_E("Fail to pause A2DP for active FOTA. bt_ret:%x", 1, bt_ret);
        ret = RACE_ERRCODE_FAIL;
    }

    RACE_APP_BT_LOG_MSGID_W("Pause a2dp ret:%d.", 1, ret);
    return ret;
}


void race_app_fota_bt_profile_connected_event_hdl(bt_bd_addr_t address,
                                                  bt_cm_profile_service_t profile_type)
{
    race_app_bt_profile_info_struct *disc_profile_info = NULL;

    if (BT_CM_PROFILE_SERVICE_HFP == profile_type ||
        BT_CM_PROFILE_SERVICE_A2DP_SINK == profile_type) {
        race_app_fota_find_disc_profile_info_by_address(&disc_profile_info,
                                                        profile_type,
                                                        (bt_bd_addr_t *)&address);
        if (disc_profile_info) {
            /* disc_profile_info->profile_state:
             * RACE_APP_BT_PROFILE_STATE_NONE: Disconnect if needed. Otherwise remove the profile info. Invalid state
             * RACE_APP_BT_PROFILE_STATE_CONNECTING: Disconnect if needed. Otherwise remove the profile info. FOTA app has
             *   recovered the connection.
             * RACE_APP_BT_PROFILE_STATE_CONNECTED: Disconnect if needed. Otherwise remove the profile info. Invalid state
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTING: Remove the profile info. Other module must
             *   try to connect the profile.
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTED: Remove the profile info. Other module must
             *   try to connect the profile.
             * RACE_APP_BT_PROFILE_STATE_MAX: Disconnect if needed. Otherwise remove the profile info. Invalid state
             */
            if (RACE_APP_BT_PROFILE_STATE_DISCONNECTING == disc_profile_info->profile_state ||
                RACE_APP_BT_PROFILE_STATE_DISCONNECTED == disc_profile_info->profile_state) {
                race_app_fota_remove_disc_profile_info(disc_profile_info);
                return;
            }

            /* Remove the disc_profile_info also for it is in the CONNECTED state. */
            race_app_fota_remove_disc_profile_info(disc_profile_info);
            disc_profile_info = NULL;
        }

        if (race_app_fota_is_profile_disconnect_needed(profile_type)) {
            race_app_fota_active_mode_disc_profile(profile_type, (bt_bd_addr_t *)&address);
        }
    }
}


void race_app_fota_bt_profile_disconnected_event_hdl(bt_bd_addr_t address,
                                                     bt_cm_profile_service_t profile_type)
{
    race_app_bt_profile_info_struct *disc_profile_info = NULL;

    if (BT_CM_PROFILE_SERVICE_HFP == profile_type ||
        BT_CM_PROFILE_SERVICE_A2DP_SINK == profile_type) {
        race_app_fota_find_disc_profile_info_by_address(&disc_profile_info,
                                                        profile_type,
                                                        (bt_bd_addr_t *)&address);
        if (disc_profile_info) {
            /* disc_profile_info->profile_state:
             * RACE_APP_BT_PROFILE_STATE_NONE: Remove the profile info. Invalid state
             * RACE_APP_BT_PROFILE_STATE_CONNECTING: Remove the profile info. The profile is not disconnected by FOTA for sure.
             * RACE_APP_BT_PROFILE_STATE_CONNECTED: Remove the profile info. Invalid state
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTING: Update the state to DISCONNECTED. Connect the profile if need.
             *   FOTA app just sents a disconnection action.
             * RACE_APP_BT_PROFILE_STATE_DISCONNECTED: Connect the profile if need. Duplicate DISCONNECTED event.
             * RACE_APP_BT_PROFILE_STATE_MAX: Remove the profile info. Invalid state
             */
            if (RACE_APP_BT_PROFILE_STATE_DISCONNECTING != disc_profile_info->profile_state &&
                RACE_APP_BT_PROFILE_STATE_DISCONNECTED != disc_profile_info->profile_state) {
                race_app_fota_remove_disc_profile_info(disc_profile_info);
            } else {
                /* If SP is disconnected, connecting HFP / A2DP will trigger the reconnection with SP which is unexpected when LPCOMM
                                * is enabled. Do not remove disconnected SP info when LPCOMM is not enabled, both HFP and A2DP are disconnected
                                * and BR/EDR will disconnected automatically for BLE FOTA and for SPP FOTA failure.
                                */
#ifdef RACE_LPCOMM_ENABLE
                if (race_app_fota_is_device_connected((bt_bd_addr_t *)&address))
#endif
                {
                    disc_profile_info->profile_state = RACE_APP_BT_PROFILE_STATE_DISCONNECTED;
                    if (race_app_fota_is_profile_connect_needed(profile_type)) {
                        /* Only reconnect the profile which is disconnected by the FOTA app. */
                        race_app_fota_active_mode_disc_profile_revert(profile_type, (bt_bd_addr_t *)&address);
                    }
                }
#ifdef RACE_LPCOMM_ENABLE
                else {
                    race_app_fota_remove_disc_profile_info(disc_profile_info);
                }
#endif
            }
        } else {
            /* Do nothing because the profile is not disconnected by the FOTA app. */
        }
    }
}


void race_app_fota_bt_profile_connection_update_hdl(bt_bd_addr_t address,
                                                    bt_cm_profile_service_t profile_type,
                                                    race_app_bt_cm_profile_event_enum connection_state,
                                                    bt_status_t reason)
{
    RACE_APP_BT_LOG_MSGID_I("connection event:%x profile_type:%x bt_address:%x %x %x %x %x %x", 8,
                            connection_state, profile_type,
                            address[0], address[1], address[2],
                            address[3], address[4], address[5]);

    if (!g_race_app_bt_cntx.is_rho_on_going &&
        (BT_CM_PROFILE_SERVICE_HFP == profile_type ||
         BT_CM_PROFILE_SERVICE_A2DP_SINK == profile_type)) {
        race_app_bt_profile_info_struct *disc_profile_info = NULL;

        race_app_fota_find_disc_profile_info_by_address(&disc_profile_info,
                                                        profile_type,
                                                        (bt_bd_addr_t *)&address);

        switch (connection_state) {
            case RACE_APP_BT_CM_PROFILE_EVENT_CONNECTED: {
                /* Process the profile connected event. */
                return race_app_fota_bt_profile_connected_event_hdl(address, profile_type);
            }

            case RACE_APP_BT_CM_PROFILE_EVENT_DISCONNECTED: {
                /* Process the profile disconnected event. */
                return race_app_fota_bt_profile_disconnected_event_hdl(address, profile_type);
            }

            default: {
                break;
            }
        }
    }
}
#endif


#if defined(RACE_ROLE_HANDOVER_SERVICE_ENABLE) && defined(RACE_LPCOMM_ENABLE)
bt_status_t race_app_rho_srv_allow_execution_callback(const bt_bd_addr_t *addr)
{
    return BT_STATUS_SUCCESS;
}


bt_status_t race_app_rho_srv_get_data_callback(const bt_bd_addr_t *addr, void *data)
{
#ifdef AIR_MULTI_POINT_ENABLE
    if (NULL == addr)
#endif
    {
#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
        memcpy(data, &g_race_app_bt_cntx, sizeof(race_app_bt_cntx_struct));
#endif
    }

    return BT_STATUS_SUCCESS;
}


uint8_t race_app_rho_srv_get_length_callback(const bt_bd_addr_t *addr)
{
    uint8_t data_len = 0;

#ifdef AIR_MULTI_POINT_ENABLE
    if (NULL == addr)
#endif
    {
#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
        data_len += sizeof(race_app_bt_cntx_struct);
#endif
    }

    return data_len;
}


bt_status_t race_app_rho_srv_update_callback(bt_role_handover_update_info_t *info)
{
#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
    if (info && BT_AWS_MCE_ROLE_PARTNER == info->role && info->data && info->length &&
        sizeof(race_app_bt_cntx_struct) <= info->length) {
        memcpy(&g_race_app_bt_cntx, info->data, sizeof(race_app_bt_cntx_struct));
    }
#endif

    return BT_STATUS_SUCCESS;
}


void race_app_rho_srv_status_callback(const bt_bd_addr_t *addr,
                                      bt_aws_mce_role_t role,
                                      bt_role_handover_event_t event,
                                      bt_status_t status)
{
#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
    if (BT_ROLE_HANDOVER_START_IND == event) {
        g_race_app_bt_cntx.is_rho_on_going = TRUE;
    } else if (BT_ROLE_HANDOVER_PREPARE_REQ_IND == event) {
        /* At this time, all profile connection or disconnection requests are supposed to be handled. However the
         * profile CONNECTED or DISCONNECTED events may be lost because of RHO. Therefore change the ING state to
         *  the stable state and remove the invalid profile info here.
         */
        if (BT_AWS_MCE_ROLE_AGENT == role) {
            race_app_fota_update_disc_profile_info(BT_CM_PROFILE_SERVICE_HFP);
            race_app_fota_update_disc_profile_info(BT_CM_PROFILE_SERVICE_A2DP_SINK);
        }
    } else if (BT_ROLE_HANDOVER_COMPLETE_IND == event) {
        g_race_app_bt_cntx.is_rho_on_going = FALSE;
        if (BT_STATUS_SUCCESS == status) {
            /* The role is still the role before RHO. */
            if (BT_AWS_MCE_ROLE_AGENT == role) {
                memset(&g_race_app_bt_cntx, 0, sizeof(race_app_bt_cntx_struct));
                g_race_app_bt_cntx.is_rho_on_going = FALSE;
            }
        }
    }
#endif
}
#endif /* RACE_ROLE_HANDOVER_SERVICE_ENABLE && RACE_LPCOMM_ENABLE */


bt_status_t race_app_bt_sink_event_handler(bt_sink_srv_event_t event_id, void *parameters)
{
#ifdef RACE_AWS_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    bt_sink_srv_event_param_t *event = (bt_sink_srv_event_param_t *)parameters;
#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
    uint32_t actions = race_app_fota_active_mode_get_actions();
#endif

    RACE_APP_BT_LOG_MSGID_I("race_app_bt_sink_event_handler event_id:%x", 1, event_id);
    switch (event_id) {
        case BT_SINK_SRV_EVENT_STATE_CHANGE: {
            /* Process the state change event. */
#ifdef RACE_AWS_ENABLE
            /* Do not process the profile event if it's Partner. */
            if (BT_AWS_MCE_ROLE_AGENT != role) {
                return BT_STATUS_SUCCESS;
            }
#endif

            RACE_APP_BT_LOG_MSGID_I("state change. pre state:0x%x cur state:0x%x", 2,
                                    event->state_change.previous,
                                    event->state_change.current);
#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
            if (BT_SINK_SRV_STATE_STREAMING == event->state_change.current) {
                g_race_app_bt_cntx.a2dp_state = event->state_change.current;
                if (race_fota_is_running(TRUE) &&
                    race_fota_is_active_mode() &&
                    (RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_PAUSE & actions)) {
                    /* Pause the A2DP if the active mode FOTA is running. */
                    race_app_fota_active_mode_pause_a2dp();
                }
            }
#ifdef RACE_FOTA_ACTIVE_MODE_KEEP_HFP
            else if (BT_SINK_SRV_STATE_INCOMING == event->state_change.current ||
                     BT_SINK_SRV_STATE_OUTGOING == event->state_change.current ||
                     BT_SINK_SRV_STATE_ACTIVE == event->state_change.current ||
                     BT_SINK_SRV_STATE_TWC_INCOMING == event->state_change.current ||
                     BT_SINK_SRV_STATE_TWC_OUTGOING == event->state_change.current ||
                     BT_SINK_SRV_STATE_HELD_ACTIVE == event->state_change.current ||
                     BT_SINK_SRV_STATE_HELD_REMAINING == event->state_change.current ||
                     BT_SINK_SRV_STATE_MULTIPARTY == event->state_change.current) {
                if (race_fota_is_running(TRUE) &&
                    race_fota_is_active_mode()) {
                    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;

                    /* Cancel the active mode FOTA if there is a phone call on-going. */
                    ret = race_fota_cancel();
                    RACE_APP_BT_LOG_MSGID_I("race_fota_cancel() ret:%d", 1, ret);
                }
            }
#endif /* RACE_FOTA_ACTIVE_MODE_KEEP_HFP */
#endif
            break;
        }

        case BT_SINK_SRV_EVENT_AVRCP_STATUS_CHANGE: {
            /* Process the AVRCP status change event. */
#ifdef RACE_AWS_ENABLE
            /* Do not process the profile event if it's Partner. */
            if (BT_AWS_MCE_ROLE_AGENT != role) {
                return BT_STATUS_SUCCESS;
            }
#endif
            g_race_app_bt_cntx.avrcp_state = event->avrcp_status_change.avrcp_status;

            RACE_APP_BT_LOG_MSGID_I("AVRCP state change. state:%x", 1, g_race_app_bt_cntx.avrcp_state);
#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
            if (BT_AVRCP_STATUS_PLAY_PLAYING == event->avrcp_status_change.avrcp_status) {
                if (race_fota_is_running(TRUE) &&
                    race_fota_is_active_mode() &&
                    (RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_PAUSE & actions)) {
                    /* Pause the A2DP if the active mode FOTA is running. */
                    race_app_fota_active_mode_pause_a2dp();
                }
            }
#endif
            break;
        }

        default: {
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}


bt_status_t race_app_bt_cm_event_handler(bt_cm_event_t event_id, void *parameters)
{
#ifdef RACE_AWS_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif

    RACE_APP_BT_LOG_MSGID_I("race_app_bt_cm_event_handler event_id:%x", 1, event_id);

    /* AWS related CM events are processed in race_app_aws_cm_event_handler() */
    switch (event_id) {
        case BT_CM_EVENT_REMOTE_INFO_UPDATE: {
            /* Process the remote information update event. */
            bt_cm_remote_info_update_ind_t *remote_update = (bt_cm_remote_info_update_ind_t *)parameters;

#ifdef RACE_AWS_ENABLE
            /* Do not process the profile event if it's Partner. */
            if (BT_AWS_MCE_ROLE_AGENT != role) {
                return BT_STATUS_SUCCESS;
            }
#endif

#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
            if (remote_update) {
                RACE_APP_BT_LOG_MSGID_I("profile:0x%x -> 0x%x, reason :0x%x", 3,
                                        remote_update->pre_connected_service, remote_update->connected_service,
                                        remote_update->reason);
                if (!(BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->pre_connected_service) &&
                    (BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->connected_service)) {
                    g_race_app_bt_cntx.a2dp_state = BT_SINK_SRV_STATE_CONNECTED;
                    race_app_fota_bt_profile_connection_update_hdl(remote_update->address,
                                                                   BT_CM_PROFILE_SERVICE_A2DP_SINK,
                                                                   RACE_APP_BT_CM_PROFILE_EVENT_CONNECTED,
                                                                   remote_update->reason);
                } else if ((BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->pre_connected_service) &&
                           !(BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_A2DP_SINK) & remote_update->connected_service)) {
                    g_race_app_bt_cntx.a2dp_state = BT_SINK_SRV_STATE_NONE;
                    race_app_fota_bt_profile_connection_update_hdl(remote_update->address,
                                                                   BT_CM_PROFILE_SERVICE_A2DP_SINK,
                                                                   RACE_APP_BT_CM_PROFILE_EVENT_DISCONNECTED,
                                                                   remote_update->reason);
                }

                if (!(BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_HFP) & remote_update->pre_connected_service) &&
                    (BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_HFP) & remote_update->connected_service)) {
                    race_app_fota_bt_profile_connection_update_hdl(remote_update->address,
                                                                   BT_CM_PROFILE_SERVICE_HFP,
                                                                   RACE_APP_BT_CM_PROFILE_EVENT_CONNECTED,
                                                                   remote_update->reason);
                } else if ((BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_HFP) & remote_update->pre_connected_service) &&
                           !(BT_CM_PROFILE_SERVICE_MASK((uint8_t)BT_CM_PROFILE_SERVICE_HFP) & remote_update->connected_service)) {
                    race_app_fota_bt_profile_connection_update_hdl(remote_update->address,
                                                                   BT_CM_PROFILE_SERVICE_HFP,
                                                                   RACE_APP_BT_CM_PROFILE_EVENT_DISCONNECTED,
                                                                   remote_update->reason);
                }
            }
#endif
            break;
        }

        default: {
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}


void race_app_init(void)
{
#if defined(RACE_ROLE_HANDOVER_SERVICE_ENABLE) && defined(RACE_LPCOMM_ENABLE)
    bt_status_t ret = bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_RACE_APP,
                                                          &race_app_rho_callbacks);
    if (BT_STATUS_SUCCESS != ret) {
        RACE_APP_BT_LOG_MSGID_E("Failed to register rho callback. ret:%d", 1, ret);
    }
#endif /* RACE_ROLE_HANDOVER_SERVICE_ENABLE && RACE_LPCOMM_ENABLE */

#ifdef RACE_AWS_ENABLE
    bt_aws_mce_report_register_callback(BT_AWS_MCE_REPORT_MODULE_FOTA, race_app_aws_mce_report_handler);
#endif

    race_app_race_event_init();

#ifdef RACE_COSYS_ENABLE
    race_app_cosys_init();
#endif
}

