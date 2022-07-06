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




#ifndef __BT_SINK_SRV_CAP_H__
#define __BT_SINK_SRV_CAP_H__

#include "bt_le_audio_sink.h"
#include "bt_sink_srv_le_cap_stream.h"
#include "bt_sink_srv_le_cap_audio_manager.h"
#include "ble_bap.h"
#include "bt_type.h"
#include "bt_gap_le_service.h"

/**
 * @brief The CAP device number.
 */
#define CAP_UNICAST_DEVICE_NUM       (2)
#define CAP_BROADCAST_DEVICE_NUM     (1)
#define CAP_MAX_DEVICE_NUM           (2/*LE Music*/ + 2/*LE Call*/ + 1/*LE Broadcast*/)

/**
 * @brief The CAP events report to user.
 */
#ifdef AIR_LE_AUDIO_CIS_ENABLE
#define BT_SINK_SRV_CAP_EVENT_ASE_STATE                             (BT_LE_AUDIO_MODULE_ASE|0x0001U)       /**< The notification when ASE state change, with #bt_sink_srv_cap_event_ase_state_t as the payload in the callback function. */
#define BT_SINK_SRV_CAP_EVENT_ASE_UPDATE_METADATA                   (BT_LE_AUDIO_MODULE_ASE|0x0002U)       /**< The notification when metadata of ASE updated, with #bt_sink_srv_cap_event_ase_update_metadata_t as the payload in the callback function. */
#endif
#define BT_SINK_SRV_CAP_EVENT_BASE_BROADCAST_AUDIO_ANNOUNCEMENTS    (BT_LE_AUDIO_MODULE_BASE|0x0001U)      /**< The notification when receiving Broadcast Audio Announcements, with #bt_sink_srv_cap_event_base_broadcast_audio_announcements_t as the payload in the callback function. */
#define BT_SINK_SRV_CAP_EVENT_BASE_PERIODIC_ADV_SYNC_ESTABLISHED    (BT_LE_AUDIO_MODULE_BASE|0x0002U)      /**< The notification when Periodic Advertising Sync established, with #bt_sink_srv_cap_event_base_periodic_adv_sync_established_t as the payload in the callback function. */
#define BT_SINK_SRV_CAP_EVENT_BASE_BASIC_AUDIO_ANNOUNCEMENTS        (BT_LE_AUDIO_MODULE_BASE|0x0003U)      /**< The notification when receiving Basic Audio Announcements, with #bt_sink_srv_cap_event_base_basic_audio_announcements_t as the payload in the callback function. */
#define BT_SINK_SRV_CAP_EVENT_BASE_BIGINFO_ADV_REPORT               (BT_LE_AUDIO_MODULE_BASE|0x0004U)
#define BT_SINK_SRV_CAP_EVENT_BASE_BIG_SYNC_IND                     (BT_LE_AUDIO_MODULE_BASE|0x0005U)
#define BT_SINK_SRV_CAP_EVENT_BASE_BIG_SYNC_ESTABLISHED             (BT_LE_AUDIO_MODULE_BASE|0x0006U)
#define BT_SINK_SRV_CAP_EVENT_BASE_PERIODIC_ADV_TERMINATE           (BT_LE_AUDIO_MODULE_BASE|0x0007U)
#define BT_SINK_SRV_CAP_EVENT_BASE_BIG_TERMINATE_IND                (BT_LE_AUDIO_MODULE_BASE|0x0008U)
#define BT_SINK_SRV_CAP_EVENT_BASE_BIG_TERMINATE_CFM                (BT_LE_AUDIO_MODULE_BASE|0x0009U)
#define BT_SINK_SRV_CAP_EVENT_BASE_SCAN_TIMEOUT                     (BT_LE_AUDIO_MODULE_BASE|0x0010U)
#define BT_SINK_SRV_CAP_EVENT_BASE_SCAN_STOPPED                     (BT_LE_AUDIO_MODULE_BASE|0x0011U)
#define BT_SINK_SRV_CAP_EVENT_BASE_BASS_ADD_SOURCE                  (BT_LE_AUDIO_MODULE_BASE|0x0012U)
typedef uint16_t bt_sink_srv_cap_event_id_t;

#ifdef AIR_LE_AUDIO_CIS_ENABLE
/**
 * @brief The ASE State.
 */
typedef enum {
    BT_SINK_SRV_CAP_STATE_IDLE,                             /**< Idle, no BLE connection. */
    BT_SINK_SRV_CAP_STATE_CONNECTED,                        /**< BLE Connected. */
    BT_SINK_SRV_CAP_STATE_ASE_STREAMING,                    /**< Genaral ASE streaming. */
    BT_SINK_SRV_CAP_STATE_ASE_STREAMING_MUSIC,              /**< Music ASE streaming. */
    BT_SINK_SRV_CAP_STATE_ASE_STREAMING_CALL,               /**< Call ASE streaming. */

    BT_SINK_SRV_CAP_STATE_INVALID = 0xFF,
} bt_sink_srv_cap_state;

/**
 *  @brief This structure defines the ASE sub state details.
 */
enum {
    BT_SINK_SRV_CAP_SUB_STATE_ASE_MUSIC_ENABLING,                       /**< Only one ASE is in enabling state for music. */
    BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_ENABLING_2_ASE,                  /**< Two ASEs are in enabling state for call and wait for handshaking. */
    BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_ENABLING_3_ASE,                  /**< Three ASEs are in enabling state for call and wait for handshaking. */
    BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_2_ASE_1,         /**< One ASE is in streaming state, the other one is preparing. */
    BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_3_ASE_1,         /**< One ASE is in streaming state, other two ASEs are preparing. */
    BT_SINK_SRV_CAP_SUB_STATE_ASE_CALL_PREPARE_STREAMING_3_ASE_2,         /**< Two ASEs are in streaming state, other one is preparing. */
};

/**
 *  @brief This structure defines the parameter data type for event #BT_SINK_SRV_CAP_EVENT_ASE_STATE.
 */
typedef struct {
    bt_handle_t connect_handle;                 /**< Connection handle. */
    bt_sink_srv_cap_state pre_state;                          /**< Previous state. */
    bt_sink_srv_cap_state current_state;                      /**< Current state. */
} bt_sink_srv_cap_event_ase_state_t;

/**
 *  @brief This structure defines the parameter data type for event #BT_SINK_SRV_CAP_EVENT_ASE_UPDATE_METADATA.
 */
typedef struct
{
    bt_handle_t connect_handle;                 /**< Connection handle. */
    uint8_t  metadata_length;                   /**< Length of the Metadata parameter. */
    uint8_t  metadata[1];                       /**< LTV-formatted Metadata. */
} bt_sink_srv_cap_event_ase_update_metadata_t;
#endif

/**
*   @brief This structure defines the parameter data type for event #BT_LE_AUDIO_SINK_EVENT_MEDIA_SUSPEND.
*/
typedef struct
{
    bt_handle_t connect_handle;                 /**< Connection handle. */
    bool resume;                                /**< need resume.*/
} bt_sink_srv_cap_event_media_change_state_t;

/**
 *  @brief This structure defines the parameter data type for event #BT_SINK_SRV_CAP_EVENT_BASE_BROADCAST_AUDIO_ANNOUNCEMENTS.
 */
typedef ble_bap_broadcast_audio_announcements_ind_t bt_sink_srv_cap_event_base_broadcast_audio_announcements_t;

/**
 *  @brief This structure defines the parameter data type for event #BT_SINK_SRV_CAP_EVENT_BASE_BASIC_AUDIO_ANNOUNCEMENTS.
 */

typedef ble_bap_periodic_adv_sync_established_notify_t bt_sink_srv_cap_event_base_periodic_adv_sync_established_t;
typedef ble_bap_basic_audio_announcements_ind_t bt_sink_srv_cap_event_base_basic_audio_announcements_t;
typedef ble_bap_big_info_adv_report_ind_t bt_sink_srv_cap_event_base_biginfo_adv_report_t;
typedef ble_bap_big_sync_ind_t bt_sink_srv_cap_event_base_big_sync_ind_t;
typedef ble_bap_big_sync_established_notify_t bt_sink_srv_cap_event_base_big_sync_established_t;
typedef ble_bap_bass_add_source_ind_t bt_sink_srv_cap_event_base_bass_add_source_t;

/**
 *  @brief This structure defines the parameter data type for event #BT_SINK_SRV_CAP_EVENT_BASE_PERIODIC_ADV_SYNC_ESTABLISHED.
 */
/*typedef struct
{
    bt_handle_t sync_handle;
    bt_addr_t advertiser_addr;
    uint16_t pa_interval;
} bt_sink_srv_cap_event_base_periodic_adv_sync_established_t;*/

typedef void (*bt_sink_srv_cap_callback_t)(bt_le_audio_sink_event_t event, void *msg);



/**
 * @brief                       This function is a Sink Service CAP initialization API.
 * @param[in] app_callback      is callback function used to handle  Sink Service CAP events.
 * @param[in] max_link_num      is maximum number of BLE link.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_init(bt_sink_srv_cap_callback_t app_callback, uint8_t max_link_num);

/**
 * @brief                       This function is a Sink Service CAP deinitialization API.
 * @param[in] app_callback      is callback function used to handle Sink Service CAP events.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_deinit(bt_sink_srv_cap_callback_t app_callback);

#ifdef AIR_LE_AUDIO_CIS_ENABLE
/**
 * @brief                       This function is a Sink Service CAP Broadcast registration API.
 * @param[in] app_callback      is callback function used to handle Sink Service CAP events.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */

bt_status_t bt_sink_srv_cap_register_broadcast_callback(bt_sink_srv_cap_callback_t callback);

/**
 * @brief                       This function is a Sink Service CAP Broadcast deregistration API.
 * @param[in] callback          is callback function used to handle Sink Service CAP broadcast events.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_deregister_broadcast_callback(void);

/**
 * @brief                       This function is used to set BLE Audio connection handle.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_set_link(bt_handle_t connect_handle);

/**
 * @brief                       This function is used to clear BLE Audio connection handle.
 * @param[in] connect_handle    is BLE connection handle.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_clear_link(bt_handle_t connect_handle);


/**
 * @brief                       This function is used to get BLE connection handle.
 * @param[in] index             is link index.
 * @return                      BLE connection handle.
 */
bt_handle_t bt_sink_srv_cap_get_link_handle(uint8_t index);

/**
 * @brief                       This function is used to get BLE connection handle.
 * @param[in] index             is BLE connection handle.
 * @return                      Link index.
 */
uint8_t bt_sink_srv_cap_get_link_index(bt_handle_t connect_handle);

/**
 * @brief                       This function is used to get the BLE connection handle which satisfied specified state.
 * @param[in] state             is specified state.
 * @return                      BLE connection handle.
 */
bt_handle_t bt_sink_srv_cap_check_links_state(bt_sink_srv_cap_state state);

/**
 * @brief                       This function is used to get BLE address.
 * @param[in] index             is link index.
 * @return                      BLE address.
 */
bt_addr_t bt_sink_srv_cap_get_peer_bdaddr(uint8_t index);

/**
 * @brief                       This function convert Sink action to LE audio action.
 * @param[in] sink_action       is sink action.
 * @return                      LE audio action.
 */
uint32_t bt_sink_srv_cap_get_le_audio_action(uint32_t sink_action);

/**
 * @brief                       This function set state for Sink Service CAP.
 * @param[in] connect_handle    is BLE connection handle.
 * @param[in] state             is  Sink Service CAP state.
 * @return                      none.
 */
void bt_sink_srv_cap_set_state(bt_handle_t connect_handle, bt_sink_srv_cap_state state);

/**
 * @brief                       This function updates BLE connection interval.
 * @param[in] handle            is BLE connection handle.
 * @param[in] state             is BLE connection interval.
 * @return                      none.
 */
void bt_sink_srv_cap_update_connection_interval(bt_handle_t handle, uint16_t conn_interval);

/**
 * @brief                       This function set sub state for Sink Service CAP.
 * @param[in] connect_handle    is BLE connection handle.
 * @param[in] state             is Sink Service CAP sub state.
 * @return                      none.
 */
void bt_sink_srv_cap_set_sub_state(bt_handle_t connect_handle, uint8_t sub_state);
#endif

#endif
