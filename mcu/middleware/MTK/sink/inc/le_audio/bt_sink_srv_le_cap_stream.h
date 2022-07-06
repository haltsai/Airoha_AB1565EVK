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


#ifndef __BT_SINK_SRV_CAP_STREAM_H__
#define __BT_SINK_SRV_CAP_STREAM_H__

#include "ble_bap.h"
#include "bt_sink_srv_le_cap_audio_manager.h"

#ifndef PACKED
#define PACKED  __attribute__((packed))
#endif

#define BIS_INDEX_INVALID   (0xFF)

typedef enum
{
    BT_SINK_SRV_CAP_STREAM_SYNC_POLICY_NONE,
    BT_SINK_SRV_CAP_STREAM_SYNC_POLICY_SELECT_BIS,
}bt_sink_srv_cap_stream_sync_policy_t;



/**
 *  @brief This structure defines the parameter data type in function #bt_sink_srv_cap_stream_get_config_info().
 */
typedef ble_bap_stream_config_info_t bt_sink_srv_cap_stream_config_info_t;

#if 0
typedef struct {
    uint8_t direction;                          /**< Direction of this ASE with respect to the server (Audio Source or Audio Sink). */
    uint8_t codec[5];                           /**< The codec id. */
    uint8_t sampling_frequency;                 /**< Sampling frequency. */
    uint8_t sdu_interval[3];                    /**< SDU interval. */
    uint8_t framing;                            /**< Framing. */
    uint8_t phy;                                /**< PHY. */
    uint8_t presentation_delay[3];              /**< Presentation delay. */
    uint8_t retransmission_number;              /**< Retransmission number. */
    uint16_t frame_payload_length;              /**< Frame payload length. */
    uint16_t transport_latency;                 /**< Transport latency. */
} PACKED bt_sink_srv_cap_stream_config_info_t;
#endif
/**
 *  @brief This structure defines the service BIG information.
 */
typedef struct {
    bt_handle_t sync_handle;                    /**< The sync handle of the periodic advertising. */
    uint8_t big_handle;                         /**< The BIG handle. */
    uint8_t num_bis;                            /**< Number of BIS to sync. */
    uint8_t *bis_indices;                       /**< BIS index list. */
    bt_sink_srv_cap_stream_config_info_t config_info;  /**< Audio configuration information. */
} PACKED bt_sink_srv_cap_stream_service_big_t;

/**
 *  @brief This structure defines the BMR scan parameters.
 */
typedef struct
{
    bt_le_audio_location_t audio_channel_allocation;    /**< The specified audio channel location. */
    bt_bd_addr_t *bms_address;                          /**< The specified device address. */
    uint16_t duration;                                  /**< The scanning duration. (uint:10ms) */
    bt_sink_srv_cap_stream_sync_policy_t sync_policy;   /**< The scanning policy */
}PACKED bt_sink_srv_cap_stream_bmr_scan_param_t;

/**
 *  @brief This structure defines the current BMR scan info.
 */
typedef struct
{
    bt_le_audio_location_t audio_channel_allocation;
    bt_bd_addr_t bms_address;
    bool specified_bms;
    bt_sink_srv_cap_stream_sync_policy_t sync_policy;
    bt_handle_t sync_handle;
}PACKED bt_sink_srv_cap_stream_bmr_scan_info_t;


/**
 * @brief                       This function is a CAP stream initialization API.
 * @param[in] max_link_num      is maximum number of BLE link.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_init(uint8_t max_link_num);

/**
 * @brief                       This function is a CAP stream deinitialization API.
 * @param[in] max_link_num      is maximum number of BLE link.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_deinit(void);

#ifdef AIR_LE_AUDIO_CIS_ENABLE
/**
 * @brief                       This function responds to the ASE enabling request sent to Audio Manager.
 * @param[in] is_accept         is accept or not.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_enabling_response(bt_handle_t handle, bool is_accept, bt_le_audio_direction_t direction);

/**
 * @brief                       This function responds to the ASE disabling request sent to Audio Manager.
 * @param[in] is_accept         is accept or not.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_disabling_response(bt_handle_t handle, bool is_accept, bt_le_audio_direction_t direction);

/**
 * @brief                       This function responds to the multiple ASEs disabling request sent to Audio Manager.
 * @param[in] is_accept         is accept or not.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_disabling_response_all(bt_handle_t handle, bool is_accept, bt_le_audio_direction_t direction);

/**
 * @brief                       This function responds restarting operation complete sent from Audio Manager.
 * @param[in] mode              is streaming mode.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
void bt_sink_srv_cap_stream_restarting_complete_response(bt_sink_srv_cap_am_mode mode);

/**
 * @brief                       This function responds to the BASE enabling request sent to Audio Manager.
 * @param[in] is_accept         is accept or not.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_broadcast_enabling_response(bool is_accept);

/**
 * @brief                       This function is used to get streaming BLE link.
 * @return                      streaming BLE connection handle.
 */
bt_handle_t bt_sink_srv_cap_stream_get_service_ble_link(void);

/**
 * @brief                       This function is used to get streaming BLE link.
 * @return                      BLE connection handle with CIS established.
 */
bt_handle_t bt_sink_srv_cap_stream_get_ble_link_with_cis_established(void);


/**
 * @brief                       This function is used to get default BMR scan information.
 * @param[in] scan_param        Scan parameters.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_get_default_bmr_scan_info(bt_sink_srv_cap_stream_bmr_scan_param_t *scan_param);

/*
 * @brief                       This function allocates ASE information.
 * @param[in] connect_handle    is BLE connection handle.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_allocate_ase_link(bt_handle_t connect_handle);

/**
 * @brief                       This function clears all ASEs information.
 * @param[in] connect_handle    is BLE connection handle.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_clear_all_ase_link(bt_handle_t connect_handle);

/**
 * @brief                       This function sets streaming BLE link.
 * @param[in] connect_handle    is BLE connection handle.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_set_service_ble_link(bt_handle_t connect_handle);

/**
 * @brief                       This function clears streaming BLE link.
 * @param[in] connect_handle    is BLE connection handle.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_clear_service_ble_link(bt_handle_t connect_handle);

/**
 * @brief                       This function start a timer for Audio Manager in order to wait for CIS established.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_am_timer_start(void);

/**
 * @brief                       This function autonomously initiate codec config operation.
 * @param[in] connect_handle    is BLE connection handle.
 * @param[in] direction         is audio direction.
 * @param[in] parm              is the parameters of codec configuration.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_config_codec_autonomously(bt_handle_t connect_handle, bt_le_audio_direction_t direction, ble_ascs_config_codec_operation_t *parm);

/**
 * @brief                       This function autonomously initiate update metadata operation.
 * @param[in] connect_handle    is BLE connection handle.
 * @param[in] parm              is the parameters of metadata.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_update_metadata_autonomously(bt_handle_t connect_handle, ble_ascs_update_metadata_operation_t *parm);

/**
 * @brief                       This function autonomously initiate disable operation.
 * @param[in] connect_handle    is BLE connection handle.
 * @param[in] ase_id            is identifier of ASE.
                                #0xFF, disable all ASEs that is in streaming state.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_disable_autonomously(bt_handle_t connect_handle, uint8_t ase_id);

/**
 * @brief                       This function autonomously initiate release operation.
 * @param[in] connect_handle    is BLE connection handle.
 * @param[in] ase_id            is identifier of ASE.
 * @param[in] resume_needed     is the option to resume streaming if these ASEs are in streaming state.
 * @param[in] resume_timeout    is the timer to cancel resuming.
                                #0xFF, disable all ASEs that is in streaming state.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_release_autonomously(bt_handle_t connect_handle, uint8_t ase_id, bool resume_needed, uint16_t resume_timeout);
#endif

/**
 * @brief                       This function sets all CIS ISO data path under specified BLE connection.
 * @param[in] connect_handle    is BLE connection handle.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_set_all_cis_data_path(bt_handle_t connect_handle);

/**
 * @brief                       This function is used to get streaming BLE link.
 * @param[in] mode              BLE audio streaming mode.
 * @return                      BLE connection handle.
 */
bt_handle_t bt_sink_srv_cap_get_ble_link_by_streaming_mode(bt_sink_srv_cap_am_mode mode);

/**
 * @brief                       This function responds to the BASE enabling request sent to Audio Manager.
 * @param[in] is_accept         is accept or not.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_broadcast_enabling_response(bool is_accept);

/**
 * @brief                       This function responds to the BASE disabling request sent to Audio Manager.
 * @param[in] is_accept         is accept or not.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_broadcast_disabling_response(bool is_accept);

/**
 * @brief                       This function is used to get configured audio information.
 * @return                      configured audio information.
 */
bt_sink_srv_cap_stream_config_info_t bt_sink_srv_cap_stream_get_config_info(bt_sink_srv_cap_am_mode mode, bt_le_audio_direction_t direction);

/**
 * @brief                       This function is used to get streaming BIG information.
 * @return                      streaming BIG information.
 */
bt_sink_srv_cap_stream_service_big_t *bt_sink_srv_cap_stream_get_service_big(void);


/**
 * @brief                       This function clears streaming BIG information.
 * @param[in] big_handle        is BIG handle.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_clear_service_big(uint8_t big_handle);

/**
 * @brief                       This function start streaming with broadcast source.
 * @param[in] connect_handle    is the sync handle of the periodic advertising.
 * @param[in] num_bis           is the number of bises.
 * @param[in] bis_indices       is the bis index list.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_start_broadcast_reception(bt_handle_t sync_handle, bt_handle_t big_handle, uint8_t num_bis, uint8_t *bis_indices);

/**
 * @brief                       This function stop streaming with broadcast source.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_stop_broadcast_reception(void);

/**
 * @brief                       This function syncs Periodic Advertising with broadcast source.
 * @param[in] addr              is the address of broadcast source.
 * @param[in] num_bis           is the Advertising SID.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_stream_broadcast_sync_periodic_advretising(bt_addr_t addr, uint8_t advertising_sid);

/**
 * @brief                       This function gets BIS index with specified audio loacation.
 * @param[in] audio_location    is the audio location.
 * @param[in] subgroup_idx      is the subgroup index.
 * @return                      the BIS index.
 */
uint8_t bt_sink_srv_cap_stream_get_bis_index_by_audio_location(bt_le_audio_location_t audio_location, uint8_t subgroup_idx);

/**
 * @brief                       This function gets BIS index with specified audio loacation in Broadcast Audio Announcements level 1.
 * @param[in] audio_location    is the audio location.
 * @return                      the BIS index.
 */
uint8_t bt_sink_srv_cap_stream_get_bis_index_in_pa_level_1(bt_le_audio_location_t audio_location);

/**
 * @brief                       This function start scanning broadcast source.
 * @param[in] bmr_scan_info     is the scan parameters.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_stream_scan_broadcast_source(bt_sink_srv_cap_stream_bmr_scan_param_t *bmr_scan_info);

/**
 * @brief                       This function stop scanning broadcast source.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_stream_stop_scanning_broadcast_source(void);

/**
 * @brief                       This function checks local device is broadcast streaming or not.
 * @return                      #true, the local device is streaming.
 *                              #false, the local device is not streaming.
 */
bool bt_sink_srv_cap_stream_is_broadcast_streaming(void);

/**
 * @brief                       This function gets broadcast streaming volume level.
 * @return                      the volume level.
 */
bt_sink_srv_am_volume_level_out_t bt_sink_srv_cap_stream_get_broadcast_volume(void);

/**
 * @brief                       This function checks the broadcast streaming mute state.
 * @return                      #true, the broadcast streaming mute state is mute.
 *                              #false, the broadcast streaming mute state is unmute.
 */
bool bt_sink_srv_cap_stream_is_broadcast_mute(void);

/**
 * @brief                       This function sets broadcast streaming volume.
 * @param[in] volume            is the broadcast streaming volume.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_set_broadcast_volume(bt_sink_srv_am_volume_level_out_t volume);

/**
 * @brief                       This function sets broadcast streaming mute state.
 * @param[in] mute              is the broadcast streaming mute state.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_set_broadcast_mute(bool mute);

/**
 * @brief                       This function gets current scanning information.
 * @return                      the current scanning information.
 */
bt_sink_srv_cap_stream_bmr_scan_info_t *bt_sink_srv_cap_stream_get_bmr_scan_info(void);

/**
 * @brief                       This function resets broadcast streaming state.
 * @return                      none.
 */
void bt_sink_srv_cap_stream_reset_broadcast_state(void);

/**
 * @brief                       This function gets current broadcast code.
 * @param[in] broadcast_code    is the broadcast code.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_stream_set_broadcast_code(uint8_t *broadcast_code);

/**
 * @brief                       This function sets current broadcast code.
 * @param[in] broadcast_code    is the broadcast code.
 * @return                      #BT_STATUS_SUCCESS, the operation completed successfully.
 *                              #BT_STATUS_FAIL, the operation has failed.
 */
bt_status_t bt_sink_srv_cap_stream_get_broadcast_code(uint8_t *broadcast_code);

/**
 * @brief                       This function checks local device is scanning broadcast source or not.
 * @return                      #true, the local device is scanning.
 *                              #false, the local device is not scanning.
 */
bool bt_sink_srv_cap_stream_is_scanning_broadcast_source(void);

/**
 * @brief                       This function request Audio manager and controller to restart streaming without sending ASE state notification.
 * @return                      #true, the operation completed successfully.
 *                              #false, the operation has failed.
 */
bool bt_sink_srv_cap_stream_restart_streaming(void);

#endif
