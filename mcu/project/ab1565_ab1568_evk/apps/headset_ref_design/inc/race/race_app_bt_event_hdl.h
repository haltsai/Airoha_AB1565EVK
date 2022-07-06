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
 * File: race_app_bt_event_hdl.h
 *
 * Description: This file defines some macros and some types and declares the interfaces of race_app_bt_event_hdl.c.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifndef __RACE_APP_BT_EVENT_HDL_H__
#define __RACE_APP_BT_EVENT_HDL_H__

#include "race_cmd_feature.h"
#include "bt_sink_srv.h"
#include "race_event.h"


#define RACE_APP_BT_SP_CONNECTED_MAX_COUNT  (1)


typedef enum {
    RACE_APP_BT_PROFILE_ACTION_NONE,

    RACE_APP_BT_PROFILE_ACTION_CONNECTED,
    RACE_APP_BT_PROFILE_ACTION_DISCONNECTED,

    RACE_APP_BT_PROFILE_ACTION_MAX
} race_app_bt_profile_action_enum;


typedef enum {
    RACE_APP_BT_PROFILE_STATE_NONE,

    RACE_APP_BT_PROFILE_STATE_CONNECTING,
    RACE_APP_BT_PROFILE_STATE_CONNECTED,

    RACE_APP_BT_PROFILE_STATE_DISCONNECTING,
    RACE_APP_BT_PROFILE_STATE_DISCONNECTED,

    RACE_APP_BT_PROFILE_STATE_MAX
} race_app_bt_profile_state_enum;


typedef struct {
    bt_bd_addr_t address;
    race_app_bt_profile_state_enum profile_state;
    bool is_used;
} race_app_bt_profile_info_struct;


typedef struct {
    bt_avrcp_status_t avrcp_state;
    bt_sink_srv_state_t a2dp_state;
    bool is_rho_on_going;
#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
#ifndef RACE_FOTA_ACTIVE_MODE_KEEP_HFP
    race_app_bt_profile_info_struct fota_disc_hfp_info[RACE_APP_BT_SP_CONNECTED_MAX_COUNT]; /* HFP profiles that are disconnected by FOTA app. */
#endif
#ifndef RACE_FOTA_ACTIVE_MODE_KEEP_A2DP
    race_app_bt_profile_info_struct fota_disc_a2dp_info[RACE_APP_BT_SP_CONNECTED_MAX_COUNT]; /* A2DP profiles that are disconnected by FOTA app. */
#endif
#endif
} race_app_bt_cntx_struct;


bt_sink_srv_state_t race_app_bt_get_a2dp_state(void);

bt_avrcp_status_t race_app_bt_get_avrcp_state(void);

#ifdef RACE_FOTA_ACTIVE_MODE_ENABLE
/**
* @brief      This function gets the profile information of the profiles with a specified type which are disconnected by the FOTA in the active mode.
* @param[in]  profile_type, the profile type.
* @return  If it is NULL, the profile informatin is not found. Otherwise, return the profile information.
*/
race_app_bt_profile_info_struct *race_app_fota_get_disc_profile_info_list(bt_cm_profile_service_t profile_type);

/**
* @brief      This function clears the recorded profile information of the profiles with a specified type the devices of which are not connected any more.
* @param[in]  profile_type, the profile type.
*/
void race_app_fota_clear_unconn_sp_disc_profile_info(bt_cm_profile_service_t profile_type);

/**
* @brief      This function disconnects the profile specified.
* @param[in]  profile_type, the profile type.
* @param[in]  bt_address, the BR/EDR address of the device the profile of which will be disconnected..
*/
RACE_ERRCODE race_app_fota_active_mode_disc_profile(bt_cm_profile_service_t profile_type,
                                                    bt_bd_addr_t *bt_address);

/**
* @brief      This function connects the profile that is disconnected by the FOTA in the active mode..
* @param[in]  profile_type, the profile type.
* @param[in]  bt_address, the BR/EDR address of the device the profile of which will be connected..
*/
RACE_ERRCODE race_app_fota_active_mode_disc_profile_revert(bt_cm_profile_service_t profile_type,
                                                           bt_bd_addr_t *bt_address);

RACE_ERRCODE race_app_fota_active_mode_pause_a2dp(void);

/**
* @brief      This function gets the actions for HFP/A2DP/DSP when FOTA is in the active mode.
* @return     The actions for HFP/A2DP/DSP.
*/
uint32_t race_app_fota_active_mode_get_actions(void);

/**
* @brief      This function checks if the profile need be disconnected according to the actions of the FOTA in the active mode.
* @param[in]  profile_type, the profile type to be checked.
* @return  True, need be disconnected. False, need not be disconnected.
*/
bool race_app_fota_is_profile_disconnect_needed(bt_cm_profile_service_t profile_type);

bool race_app_fota_is_profile_connect_needed(bt_cm_profile_service_t profile_type);
#endif

bt_status_t race_app_bt_sink_event_handler(bt_sink_srv_event_t event_id, void *parameters);

bt_status_t race_app_bt_cm_event_handler(bt_cm_event_t event_id, void *parameters);

void race_app_init(void);
#endif /* __RACE_APP_BT_EVENT_HDL_H__ */

