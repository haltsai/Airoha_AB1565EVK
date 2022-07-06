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


#ifndef __LE_SINK_SRV_H__
#define __LE_SINK_SRV_H__

#include "bt_avrcp.h"
#include "bt_sink_srv.h"
#include "bt_sink_srv_le_call.h"
#include "bt_sink_srv_common.h"
#include "bt_sink_srv_le_music.h"
#include "bt_sink_srv_utils.h"
#include "bt_connection_manager.h"

#ifdef AIR_LE_AUDIO_CIS_ENABLE
/**
 *  @brief Define for the BLE Audio link state.
 */
#define BT_BLE_LINK_DISCONNECTED         (0x00)
#define BT_BLE_LINK_DISCONNECTING        (0x01)
#define BT_BLE_LINK_CONNECTING           (0x02)
#define BT_BLE_LINK_CONNECTED            (0x03)
typedef uint8_t bt_le_link_state_t;

/**
 *  @brief Define for the BLE Audio profile type.
 */
#define BT_SINK_SRV_PROFILE_NONE           (0x00)  /**< Profile type: None. */
#define BT_SINK_SRV_PROFILE_CALL           (0x01)  /**< Profile type: Call, BT_SINK_SRV_PROFILE_HFP. */
#define BT_SINK_SRV_PROFILE_MUSIC          (0x02)  /**< Profile type: Music, BT_SINK_SRV_PROFILE_A2DP_SINK. */
typedef uint8_t bt_le_service_mask_t;     /**<The feature configuration of sink service. */

typedef struct {
    bt_bd_addr_t address;                           /**< The remote device address. */
    bt_le_link_state_t pre_state;                  /**< The previous BLE link state. */
    bt_le_link_state_t state;                      /**< The current BLE link state. */
    bt_le_service_mask_t pre_connected_service;    /**< The previous BLE service mask. */
    bt_le_service_mask_t connected_service;        /**< The current BLE service mask. */
    bt_status_t reason;                             /**< The BLE link or profile service disconnect or connect fail reason. */
} bt_le_sink_srv_event_remote_info_update_t;

typedef struct {
    uint8_t state;
    uint8_t music_state;
    bt_addr_t *dev_addr;
    bt_avrcp_operation_id_t last_play_pause_action;//for avrcp
    uint32_t conn_mask;//Profile
} le_sink_srv_context_t;

typedef struct {
    bt_handle_t handle;
} bt_le_sink_srv_music_active_handle;

typedef struct {
    bt_handle_t accept_handle;
    bt_handle_t reject_handle;
    bt_handle_t dial_handle;
} bt_le_sink_srv_call_active_handle;

le_sink_srv_context_t *le_sink_srv_get_context(bt_handle_t handle);

/**
 * @brief                       This function sends an action to the Sink Service,
 *                              The Sink service executes a Sink operation to the remote device.
 * @param[in] action            is the Sink action.
 * @param[in] params            is the parameter of the Sink action.
 * @return                      #BT_STATUS_SUCCESS, the action is successfully sent.
 *                              The operation failed with the Sink status.
 */
bt_status_t le_sink_srv_send_action(uint32_t action, void *params);

/**
 * @brief                       This function is a Sink Service initialization API for LE audio.
 * @param[in] max_link_num      is maximum number of BLE link.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
#endif

void le_sink_srv_init(uint8_t max_link_num);


#endif

