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
 * File: app_hfp_utils.h
 *
 * Description: This file defines the common structure and function of hfp app.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */


#ifndef __APP_HFP_UTILS_H__
#define __APP_HFP_UTILS_H__

#include "apps_config_state_list.h"
#include "bt_sink_srv.h"
#include "ui_shell_manager.h"
#include "ui_shell_activity.h"
#include "apps_config_led_manager.h"
#include "apps_config_led_index_list.h"
#include "apps_config_vp_manager.h"
#include "apps_config_vp_index_list.h"
#include "apps_config_event_list.h"

#define APP_HFP_UTILS "[HFP_APP]utils"

#define TWC_INCOMING_CALL_VP_TIMER_INTERVAL   (4 * 1000)
#define APP_HFP_INCMOING_CALL_VP_LONG_DELAY_TIME  (2*1000)  /**< 2 seconds. */
#define APP_HFP_INCMOING_CALL_VP_SHORT_DELAY_TIME (6*100)

typedef enum {
    APP_HFP_INCOMING_CALL_VP_STOP = 0,        /**< Incoming call vp stop play. */
    APP_HFP_INCOMING_CALL_VP_LONG,           /**< Incoming call vp long delay time play. */
    APP_HFP_INCOMING_CALL_VP_SHORT          /**< Incoming call vp long delay time play. */
} app_hfp_incoming_call_vp_play_status_t;

typedef enum {
    APP_HFP_INCOMING_CALL_VP_SYNC_SUCCESS= 0,        /**< Incoming call vp sync play success. */
    APP_HFP_INCOMING_CALL_VP_SYNC_PLAY_FAIL,          /**< Incoming call vp sync play fail. */
    APP_HFP_INCOMING_CALL_VP_SYNC_STOP_FAIL          /**< Incoming call vp sync stop fail. */
} app_hfp_incoming_call_vp_sync_status_t;

/**
 *  @brief This structure to record the incoming call vp play and sync status.
 */
typedef struct {
        app_hfp_incoming_call_vp_play_status_t  vp_play_status;      /**< Record incoming call vp play status. */
        app_hfp_incoming_call_vp_sync_status_t  vp_sync_status;     /**< Record incoming call vp sync status. */
} app_hfp_incoming_call_vp_status_t;

/**
 *  @brief This structure defines the hfp app's context
 */
typedef struct {
    bt_sink_srv_state_t pre_state;      /**<  Record the previous sink_state. */
    bt_sink_srv_state_t curr_state;     /**<  Record the current sink_state. */
    int32_t battery_level;              /**<  Current battery level. */
    bool voice_assistant;               /**<  Indicates whether the voice assistant is active. */
    int8_t esco_connected;                /**<  Record the esco connection state. */
    bool hfp_connected;                 /**<  Record the hfp connection state. */
    uint8_t is_vp;                      /**<  Indicates whether the vp of incoming call is triggered. */
    bool aws_link_state;                /**<  Record aws link state for playing incoming call vp when incoming call. */
    bt_bd_addr_t activate_con_addr;     /**<  The BT address which connection in calling state.*/
    bool transient_actived;             /**<  Record whether the hfp transient activity is actived. */
    bool mute_mic;                        /**<  Record whether the mic is muted. */
#ifdef AIR_LE_AUDIO_ENABLE
    uint32_t    le_audio_srv;           /**<  Record LE Audio Connected Service. */
#endif
} hfp_context_t;

#ifdef MTK_IN_EAR_FEATURE_ENABLE
typedef enum {
    APP_HFP_AUTO_ACCEPT_NONE = 0,    /**<  None. */
    APP_HFP_AUTO_ACCEPT_ENABLE,      /**<  Indicates auto accept incoming call enable. */
    APP_HFP_AUTO_ACCEPT_DISABLE      /**<  Indicates auto accept incoming call disable. */
} app_hfp_auto_accept_t;
#endif

#ifdef MTK_AWS_MCE_ENABLE
void app_hfp_stop_vp_by_aws_role(bt_aws_mce_role_t aws_role);
#endif

/**
  * @brief     This function is used to stop vp associated with the call.
  *                 For example, incoming call or call reject vp.
  */
void app_hfp_stop_vp(void);

/**
  * @brief     This function is used to send call action to sink srv.
  *                 For example, accept call .etc.
  */
bool app_hfp_send_call_action(apps_config_key_action_t action);

/**
* @brief      This function is used to handle the event come from sink service module.
* @param[in]  self, the context pointer of the activity.
* @param[in]  now, the current sink_state.
* @param[in]  pre, the previous sink_state.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_hfp_update_led_bg_pattern(
    ui_shell_activity_t *self,
    bt_sink_srv_state_t now,
    bt_sink_srv_state_t pre);


/**
* @brief      This function is used to remap the bt_sink_srv_state_t to apps_config_state_t.
* @param[in]  state, the sink service state.
* @return     The app config state.
*/
apps_config_state_t app_get_config_status_by_state(bt_sink_srv_state_t state);


/**
* @brief          This function is used to set the "incoming call" vp play status.
* @param[in]  The status of incoming call vp play status.
* @return        None.
*/
void app_hfp_set_incoming_call_vp_flag(app_hfp_incoming_call_vp_play_status_t vp_play_status);


/**
* @brief      This function is used to get the "incoming call" vp play status.
* @return    The status of incoming call vp play status.
*/
app_hfp_incoming_call_vp_play_status_t app_hfp_get_incoming_call_vp_flag(void);

/**
* @brief          This function is used to set the "incoming call" vp sync status.
* @param[in]   The status of incoming call vp sync status.
* @return        None.
*/
void app_hfp_set_incoming_call_vp_sync_flag(app_hfp_incoming_call_vp_sync_status_t vp_sync_status);

/**
* @brief      This function is used to sync the "incoming call" vp sync flag to partner.
* @return     The state of "incoming call" vp sync, true means the vp sync success, 0 means not.
*/
bool app_hfp_sync_incoming_call_vp_flag();

/**
* @brief      This function is used to get the "incoming call" vp sync status.
* @return    The status of vp sync status.
*/
app_hfp_incoming_call_vp_sync_status_t app_hfp_get_incoming_call_vp_sync_flag();

/**
* @brief      This function is used to set the "incoming call" vp sync state.
* @return     The state of "incoming call" vp, 1 means the vp is playing, 0 means not.
*/
void app_hfp_incoming_call_vp_callback(uint32_t idx, vp_err_code err);

void app_hfp_report_battery_to_remote(int32_t bat_val, int32_t pre_val);

#ifdef MTK_IN_EAR_FEATURE_ENABLE
/**
* @brief      This function is used to get the status of in ear detection.
* @return     If return oxFF, get in ear detection state failed.
*             Others, get successfully.
*/
uint8_t app_hfp_is_auto_accept_incoming_call();

/**
* @brief      This function is used to set in ear detection enable or disable.
* @param[in]  en, 0x0 means disable, 0x1 means enable.
* @return     true is set successfully.
*/
bool app_hfp_set_auto_accept_incoming_call(uint8_t isAutoaccept);
/**
* @brief      This function is used to notify state to peer.
*/
void app_hfp_notify_state_to_peer();
#endif /*MTK_IN_EAR_FEATURE_ENABLE*/

#if defined(AIR_MULTI_POINT_ENABLE) && defined(AIR_EMP_AUDIO_INTER_STYLE_ENABLE)
void app_hfp_emp_music_process(ui_shell_activity_t *self, uint32_t event_id, void *extra_data, size_t data_len);
bool app_hfp_proc_conflict_vp_event(ui_shell_activity_t *self,
        uint32_t event_group,
        uint32_t event_id,
        void *extra_data,
        size_t data_len);
#endif /*AIR_MULTI_POINT_ENABLE && AIR_EMP_AUDIO_INTER_STYLE_ENABLE*/
/**
* @brief      This function is used to play incoming call vp.
* @param[in]  self, the context pointer of the activity.
* @param[in]  isLongVp, true is long delay time vp, false is short delay time. For conflict vp case.
* @param[in] isTwcIncoming, true is twc incoming call, false is incoming call.
*/
void app_hfp_incoming_call_vp_process(ui_shell_activity_t *self, bool isLongVp, bool isTwcIncoming);

/**
* @brief          This function is used to sync hfp sink state to peer.
* @param[in]   self, the context pointer of the hfp activity.
*/
void app_hfp_sync_sink_state_to_peer(ui_shell_activity_t *self);

/**
  * @brief                            This function is used to mute or unmute microphone.
  * @param[in]  type            True is mute mic, flase is unmute mic.
  * * @return     If return true, set mic mute status success.
  */
bool app_hfp_mute_mic(bool mute);

#ifdef MTK_AWS_MCE_ENABLE
bool app_hfp_get_aws_link_is_switching(void);
#endif //MTK_AWS_MCE_ENABLE
#endif
