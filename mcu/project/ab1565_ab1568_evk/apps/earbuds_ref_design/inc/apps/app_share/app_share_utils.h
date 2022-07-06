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
 /* Airoha restricted information */

/**
 * File: app_share_utils.h
 *
 * Description: This file defines the common structure and functions of share app.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifndef __APP_SHARE_UTILS_H__
#define __APP_SHARE_UTILS_H__

#include "FreeRTOS.h"
#include "apps_events_event_group.h"
#include "apps_events_interaction_event.h"
#include "apps_debug.h"
#include "ui_shell_manager.h"
#include "airo_key_event.h"
#include "apps_events_key_event.h"
#include "apps_config_event_list.h"
#include "apps_config_key_remapper.h"
#include "bt_connection_manager.h"
#include "bt_role_handover.h"
#include "bt_aws_mce_report.h"
#include "bt_device_manager.h"
#include "apps_aws_sync_event.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "app_share_idle_activity.h"
#include "app_share_activity.h"
#include "bt_mcsync_share.h"
#include "app_smcharger_utils.h"
#include "race_cmd_share_mode.h"
#include "app_power_save_utils.h"
#include "apps_config_led_index_list.h"
#include "apps_config_led_manager.h"
#include "multi_ble_adv_manager.h"
#include "apps_events_battery_event.h"
#ifdef AIR_MULTI_POINT_ENABLE
#include "bt_init.h"
#include "bt_customer_config.h"
#endif
#include "apps_customer_config.h"
#include "task.h"
#include "bt_sink_srv_ami.h"

typedef bt_mcsync_share_state_t app_share_mode_sta_t;

/* The middleware will disable page scan when enter share mode successed.
 * The middleware will also enable page scan when no SP connected and
 * disable it again when SP connected.
*/
/* #define DISABLE_PAGE_SCAN_IN_APPLAYER */

typedef struct{
    app_share_mode_sta_t current_sta;               /**<  Record current state of share mode. */
    bool is_follower;                               /**<  Indicates current role of share mode. */
    bool ble_adv_paused;                            /**<  Indicates whether the ADV is paused by share mode. */
    app_power_saving_target_mode_t target_mode;     /**<  Indicates the power saving mode in share mode. */
#ifdef MTK_AWS_MCE_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
    volatile bt_status_t rho_status;                /**<  Indicates current rho status. */
    bool rho_pending;                               /**<  Indicates whether the RHO is pended by share mode. */
    bool share_pairing_result;
#endif
#endif
#ifdef AIR_MULTI_POINT_ENABLE
    apps_config_key_action_t pending_key_action;    /**<  Indicates the key action witch pended by EMP. */
#endif
#ifdef DISABLE_PAGE_SCAN_IN_APPLAYER
    bool page_scan_off;                             /**<  Indicates whether the page scan is closed. */
#endif
} app_share_context_t;

#define KEY_CODE_INVALID 0
#define KEY_CODE_PALY_PAUSE 1

#define ACTION_VOL_UP 1
#define ACTION_VOL_DOWN 2

/**
* @brief      This function is used to get the share app's context.
* @return     If pointer of context.
*/
app_share_context_t* app_share_get_local_context(void);

/**
* @brief      This function is used to enable share mode.
* @param[in]  self, the context pointer of the activity.
* @param[in]  is_follower, indicates the role of share mode.
* @return     None.
*/
void app_share_enable_share_mode(struct _ui_shell_activity *self, bool is_follower);

/**
* @brief      This function is used to disable share mode.
* @param[in]  self, the context pointer of the activity.
* @return     None.
*/
void app_share_disable_share_mode(struct _ui_shell_activity *self);

/**
* @brief      This function is used to update current status of share mode.
* @return     None.
*/
void app_share_mode_sta_update(void);

/**
* @brief      This function is used to get the current power saving mode.
* @return     The type of power saving mode.
*/
app_power_saving_target_mode_t app_share_get_power_saving_target_mode(void);

/**
* @brief      This function is used to send key code to share agent.
* @param[in]  key_code, the key code.
*/
void app_share_send_key_code_to_agent(uint8_t key_code);

/**
* @brief      This function is used to handle share action.
* @param[in]  action, the pointer of share action.
*/
void app_share_handle_share_action(bt_mcsync_share_action_info_t* action);

/**
* @brief      This function is used to handle the volume change event.
* @param[in]  vol_action, ACTION_VOL_UP or ACTION_VOL_DOWN.
*/
void app_share_vol_change(uint8_t vol_action);

#ifdef AIR_MULTI_POINT_ENABLE
/**
* @brief      This function is used to check and exit the EMP status.
* @return     The nums of SP link.
*/
uint32_t check_and_exit_emp_mode(void);

/**
* @brief      This function is used to enable EMP status.
* @return     None.
*/
void enable_emp_mode(void);

/**
* @brief      Disable page scan to block sp connection.
* @return     None.
*/
void disable_page_scan(void);

/**
* @brief      Enable page scan to allow sp connection.
* @return     None.
*/
void enable_page_scan(void);
#endif
#endif

