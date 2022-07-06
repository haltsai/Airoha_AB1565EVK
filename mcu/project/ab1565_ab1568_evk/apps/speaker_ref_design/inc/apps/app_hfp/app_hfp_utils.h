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

#define APP_HFP_UTILS "[TK_HFP]app_hfp_utils"

/**
 *  @brief This structure defines the hfp app's context
 */
typedef struct {
    bt_sink_srv_state_t pre_state;      /**<  Record the previous sink_state. */
    bt_sink_srv_state_t curr_state;     /**<  Record the current sink_state. */
    int32_t battery_level;              /**<  Current battery level. */
    bool voice_assistant;               /**<  Indicates whether the voice assistant is active. */
    bool esco_connected;                /**<  Record the esco connection state. */
    bool hfp_connected;                 /**<  Record the hfp connection state. */
    uint8_t is_vp;                      /**<  Indicates whether the vp of incoming call is triggered. */
    bt_bd_addr_t activate_con_addr;     /**<  The BT address which connection in calling state.*/
} hfp_context_t;


/**
* @brief      This function is used to handle the key action.
* @param[in]  state, the current sink_state.
* @param[in]  event_id, the current event ID to be handled.
* @param[in]  extra_data, extra data pointer of the current event, NULL means there is no extra data.
* @param[in]  data_len, the length of the extra data. 0 means extra_data is NULL.
* @return     If return true, the current event cannot be handle by the next activity.
*/
bool app_hfp_proc_key_event(
    bt_sink_srv_state_t state,
    uint32_t event_id,
    void *extra_data,
    size_t data_len);


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
* @brief      This function is used to set the "incoming call" vp state.
* @param[in]  is_vp, 1 means the vp already triggered, 0 means not.
* @return     None.
*/
void app_hfp_set_incoming_call_vp_flag(uint8_t is_vp);


/**
* @brief      This function is used to get the "incoming call" vp state.
* @return     The state of "incoming call" vp, 1 means the vp is playing, 0 means not.
*/
uint8_t app_hfp_get_incoming_call_vp_flag(void);

#endif

