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
 * File: race_app_race_event_hdl.c
 *
 * Description: This file processes the RACE events.
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
#include "race_app_race_event_hdl.h"
#include "race_fota_util.h"
#include "race_xport.h"
#include "race_lpcomm_util.h"
#include "race_fota.h"
#include "race_bt.h"
#include "bt_app_common.h"
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
#include "bt_role_handover.h"
#endif
#include "race_app_bt_event_hdl.h"
#include "bt_device_manager.h"


log_create_module(race_app_race, PRINT_LEVEL_INFO);

#ifndef MTK_DEBUG_LEVEL_NONE
#define RACE_APP_RACE_LOG_E(fmt,arg...)         LOG_E(race_app_race, fmt,##arg)
#define RACE_APP_RACE_LOG_W(fmt,arg...)         LOG_W(race_app_race, fmt,##arg)
#define RACE_APP_RACE_LOG_I(fmt,arg...)         LOG_I(race_app_race, fmt,##arg)
#define RACE_APP_RACE_LOG_D(fmt,arg...)

#define RACE_APP_RACE_LOG_MSGID_E(fmt,arg...)   LOG_MSGID_E(race_app_race, fmt,##arg)
#define RACE_APP_RACE_LOG_MSGID_W(fmt,arg...)   LOG_MSGID_W(race_app_race, fmt,##arg)
#define RACE_APP_RACE_LOG_MSGID_I(fmt,arg...)   LOG_MSGID_I(race_app_race, fmt,##arg)
#define RACE_APP_RACE_LOG_MSGID_D(fmt,arg...)
#else
#define RACE_APP_RACE_LOG_E(fmt,arg...)
#define RACE_APP_RACE_LOG_W(fmt,arg...)
#define RACE_APP_RACE_LOG_I(fmt,arg...)

#define RACE_APP_RACE_LOG_MSGID_E(fmt,arg...)
#define RACE_APP_RACE_LOG_MSGID_W(fmt,arg...)
#define RACE_APP_RACE_LOG_MSGID_I(fmt,arg...)
#define RACE_APP_RACE_LOG_MSGID_D(fmt,arg...)
#endif


int32_t g_race_app_race_event_register_id; /* The register id returned by race_event_register(). */


#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
/**
* @brief      This function disconnects the [profile_type] profile connected for the active mode FOTA.
* @return     If succeed, RACE_ERRCODE_SUCCESS. Otherwise, other values.
*/
RACE_ERRCODE race_app_fota_active_mode_profile_disc_process(bt_cm_profile_service_t profile_type)
{
    bt_bd_addr_t addr_list[RACE_APP_BT_SP_CONNECTED_MAX_COUNT];
    uint32_t i = 0, count = 0;

    RACE_APP_RACE_LOG_MSGID_I("active_mode_profile_disc_process profile_type:%x", 1, profile_type);

    if (BT_CM_PROFILE_SERVICE_HFP != profile_type &&
        BT_CM_PROFILE_SERVICE_A2DP_SINK != profile_type) {
        return RACE_ERRCODE_NOT_SUPPORT;
    }

    memset(addr_list, 0, RACE_APP_BT_SP_CONNECTED_MAX_COUNT * sizeof(bt_bd_addr_t));
    /* Get the addresses of the devices with the [profile_type] profile connected. If the profile is in the connecting state or the
     * disconnecting state, its device address will not be in the addr_list.
     */
    count = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK((uint8_t)profile_type),
                                        addr_list,
                                        RACE_APP_BT_SP_CONNECTED_MAX_COUNT);
    if (RACE_APP_BT_SP_CONNECTED_MAX_COUNT < count) {
        count = RACE_APP_BT_SP_CONNECTED_MAX_COUNT;
        RACE_APP_RACE_LOG_MSGID_W("Too many SP connected. count:%d max_supported:%d", 2,
                                  count, RACE_APP_BT_SP_CONNECTED_MAX_COUNT);
    }

    for (i = 0; i < count; i++) {
        race_app_fota_active_mode_disc_profile(profile_type, &addr_list[i]);
    }

    return RACE_ERRCODE_SUCCESS;
}


/**
* @brief      This function connects the [profile_type] profile disconnected by the active mode FOTA.
* @return     If succeed, RACE_ERRCODE_SUCCESS. Otherwise, other values.
*/
RACE_ERRCODE race_app_fota_active_mode_profile_disc_process_revert(bt_cm_profile_service_t profile_type)
{
    race_app_bt_profile_info_struct *profile_info_list = race_app_fota_get_disc_profile_info_list(profile_type);
    uint32_t i = 0;

    RACE_APP_RACE_LOG_MSGID_I("active_mode_profile_disc_process_revert profile_type:%x", 1, profile_type);

    if ((BT_CM_PROFILE_SERVICE_HFP != profile_type &&
         BT_CM_PROFILE_SERVICE_A2DP_SINK != profile_type) ||
        !profile_info_list) {
        return RACE_ERRCODE_NOT_SUPPORT;
    }

    /* The A2DP is paused instead of being disconnected. Therefore, the smartphone is supposed to be connected. If not,
     * it must be disconnected by other module. In such case, FOTA will not re-connect the profile.
     */
    race_app_fota_clear_unconn_sp_disc_profile_info(profile_type);

    for (i = 0; i < RACE_APP_BT_SP_CONNECTED_MAX_COUNT; i++) {
        if (profile_info_list[i].is_used) {
            race_app_fota_active_mode_disc_profile_revert(profile_type, &(profile_info_list[i].address));
        }
    }

    return RACE_ERRCODE_SUCCESS;
}


/**
* @brief      This function executs the actions obtained from race_app_fota_active_mode_get_actions() for the active mode FOTA.
* @return     If succeed, RACE_ERRCODE_SUCCESS. Otherwise, other values.
*/
RACE_ERRCODE race_app_fota_active_mode_process(void)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    int32_t ret_val = 0;
#ifdef RACE_AWS_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    uint32_t actions = race_app_fota_active_mode_get_actions();
    bool is_profile_disconnect_needed = race_app_fota_is_profile_disconnect_needed(BT_CM_PROFILE_SERVICE_HFP);

    RACE_APP_RACE_LOG_MSGID_I("fota_active_mode_process actions:%x", 1, actions);

    if ((actions & RACE_FOTA_ACTIVE_MODE_ACTION_HFP_DISCONNECT)
        && is_profile_disconnect_needed
#ifdef RACE_AWS_ENABLE
        && BT_AWS_MCE_ROLE_AGENT == role
#endif
       ) {
        ret_val = race_app_fota_active_mode_profile_disc_process(BT_CM_PROFILE_SERVICE_HFP);
        if (RACE_ERRCODE_SUCCESS != ret_val) {
            ret = ret_val;
        }
    }

    if ((actions & RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_PAUSE)
#ifdef RACE_AWS_ENABLE
        && BT_AWS_MCE_ROLE_AGENT == role
#endif
       ) {
        ret_val = race_app_fota_active_mode_pause_a2dp();
        if (RACE_ERRCODE_SUCCESS != ret_val) {
            ret = ret_val;
        }
    }

    is_profile_disconnect_needed = race_app_fota_is_profile_disconnect_needed(BT_CM_PROFILE_SERVICE_A2DP_SINK);
    if (actions & RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_DISCONNECT
        && is_profile_disconnect_needed
#ifdef RACE_AWS_ENABLE
        && BT_AWS_MCE_ROLE_AGENT == role
#endif
       ) {
        ret_val = race_app_fota_active_mode_profile_disc_process(BT_CM_PROFILE_SERVICE_A2DP_SINK);
        if (RACE_ERRCODE_SUCCESS != ret_val) {
            ret = ret_val;
        }
    }

    if (actions & RACE_FOTA_ACTIVE_MODE_ACTION_DSP_SUSPEND) {
        /* Try to mute A2DP / HFP voice through DSP. */
        RACE_APP_RACE_LOG_MSGID_I("active_mode set_mute(TRUE)", 0);
        bt_sink_srv_music_set_mute(TRUE);
    }
#ifdef RACE_FOTA_ACTIVE_MODE_ULL_SUPPORT
    bt_ull_lock_streaming(TRUE);
#endif

    return ret;
}


/**
* @brief      This function reverts the actions obtained from race_app_fota_active_mode_get_actions() for the active mode FOTA.
* @return     If succeed, RACE_ERRCODE_SUCCESS. Otherwise, other values.
*/
RACE_ERRCODE race_app_fota_active_mode_process_revert(void)
{
    RACE_ERRCODE ret = RACE_ERRCODE_SUCCESS;
    int32_t ret_val = 0;
#ifdef RACE_AWS_ENABLE
    bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();
#endif
    uint32_t revert_actions = race_app_fota_active_mode_get_actions();
    bool is_profile_connect_needed = race_app_fota_is_profile_connect_needed(BT_CM_PROFILE_SERVICE_HFP);

    RACE_APP_RACE_LOG_MSGID_I("fota_active_mode_process_revert revert_actions:%x", 1, revert_actions);

    if (revert_actions & RACE_FOTA_ACTIVE_MODE_ACTION_HFP_DISCONNECT
        && is_profile_connect_needed
#ifdef RACE_AWS_ENABLE
        && BT_AWS_MCE_ROLE_AGENT == role
#endif
       ) {
        ret_val = race_app_fota_active_mode_profile_disc_process_revert(BT_CM_PROFILE_SERVICE_HFP);
        if (RACE_ERRCODE_SUCCESS != ret_val) {
            ret = ret_val;
        }
    }

    if (revert_actions & RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_PAUSE
#ifdef RACE_AWS_ENABLE
        && BT_AWS_MCE_ROLE_AGENT == role
#endif
       ) {
        /* Do nothing. */
    }

    is_profile_connect_needed = race_app_fota_is_profile_connect_needed(BT_CM_PROFILE_SERVICE_A2DP_SINK);
    if (revert_actions & RACE_FOTA_ACTIVE_MODE_ACTION_A2DP_DISCONNECT
        && is_profile_connect_needed
#ifdef RACE_AWS_ENABLE
        && BT_AWS_MCE_ROLE_AGENT == role
#endif
       ) {
        ret_val = race_app_fota_active_mode_profile_disc_process_revert(BT_CM_PROFILE_SERVICE_A2DP_SINK);
        if (RACE_ERRCODE_SUCCESS != ret_val) {
            ret = ret_val;
        }
    }

    if (revert_actions & RACE_FOTA_ACTIVE_MODE_ACTION_DSP_SUSPEND) {
        /* Try to un-mute A2DP / HFP voice through DSP. */
        RACE_APP_RACE_LOG_MSGID_I("active_mode set_mute(FALSE)", 0);
        bt_sink_srv_music_set_mute(FALSE);
    }
#ifdef RACE_FOTA_ACTIVE_MODE_ULL_SUPPORT
    bt_ull_lock_streaming(FALSE);
#endif

    return ret;
}
#endif


RACE_ERRCODE race_app_race_event_hdler(int32_t register_id, race_event_type_enum event_type, void *param, void *user_data)
{
    RACE_APP_RACE_LOG_MSGID_I("register_id:%d event_type:%d param:%x user_data:%x", 4, register_id, event_type, param, user_data);

    if (g_race_app_race_event_register_id != register_id) {
        RACE_APP_RACE_LOG_MSGID_E("register_id does not match! register_id:%d, g_register_id:%d", 2, register_id, g_race_app_race_event_register_id);
        return RACE_ERRCODE_PARAMETER_ERROR;
    }

    switch (event_type) {
        case RACE_EVENT_TYPE_BT_NEED_RHO: {
            /* Process the need RHO event. */
#ifdef RACE_AWS_ENABLE
            bt_aws_mce_role_t role = bt_device_manager_aws_local_info_get_role();

            /* Do the things before RHO - start */

            /* Do the things before RHO - end */

            /* Only Agent needs to call RHO API. */
            if (BT_AWS_MCE_ROLE_AGENT == role) {
#ifdef RACE_ROLE_HANDOVER_SERVICE_ENABLE
                int32_t ret = BT_STATUS_FAIL;

                ret = bt_role_handover_start();
                if (BT_STATUS_SUCCESS != ret) {
                    RACE_APP_RACE_LOG_MSGID_I("Failed to execute RHO, ret:%x.", 1, ret);
                }
#endif
            }
#endif
            break;
        }

        case RACE_EVENT_TYPE_FOTA_NEED_REBOOT: {
            /* Process the need reboot event sent by FOTA. */
            /* Do the things before reboot - start */

            /* Do the things before reboot - end */
            /* UI shell will handle all reboot requests. Do not reboot here. */
            break;
        }

        case RACE_EVENT_TYPE_FOTA_START: {
            /* Process the FOTA start event. */
            race_event_start_param_struct *start_param = (race_event_start_param_struct *)param;

            RACE_APP_RACE_LOG_MSGID_I("FOTA Start. is_dual_fota:%d is_active_fota:%d", 2,
                                      start_param ? start_param->is_dual_fota : 0xff,
                                      start_param ? start_param->is_active_fota : 0xff);

#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
            /* If FOTA is not running, race_fota_is_active_mode() will return FALSE and it will revert also. */
            if (race_fota_is_active_mode()) {
                race_app_fota_active_mode_process();
            } else {
                race_app_fota_active_mode_process_revert();
            }
#endif
            break;
        }

        case RACE_EVENT_TYPE_FOTA_CANCEL: {
            /* Process the FOTA cancel event. */
            race_event_cancel_param_struct *cancel_param = (race_event_cancel_param_struct *)param;

            if (cancel_param) {
                RACE_APP_RACE_LOG_MSGID_I("FOTA STOP result:%d origiantor:%d reason:%d.", 3,
                                          cancel_param->result,
                                          cancel_param->originator,
                                          cancel_param->reason);
            }

#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
            /* No matter if it is in the active mode or not, just revert. If it's backgroud FOTA, nothing will happen. */
            race_app_fota_active_mode_process_revert();
#endif
            break;
        }

        default: {
            break;
        }
    }

    return RACE_ERRCODE_SUCCESS;
}


RACE_ERRCODE race_app_race_event_init(void)
{
    return race_event_register(&g_race_app_race_event_register_id, race_app_race_event_hdler, NULL);
}

