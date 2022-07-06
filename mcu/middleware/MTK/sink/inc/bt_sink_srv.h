/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

#ifndef __BT_SINK_SRV_H__
#define __BT_SINK_SRV_H__

#include "bt_system.h"
#include "bt_type.h"
#include "bt_custom_type.h"
#include "bt_hfp.h"
#include "bt_a2dp.h"
#include "bt_aws_mce.h"
#include "bt_avrcp.h"
#include "bt_device_manager_config.h"
#include "bt_connection_manager.h"

/**
  * @addtogroup Bluetooth_Services_Group Bluetooth Services
  * @{
  * @addtogroup BluetoothServices_Sink BT Sink
  * @{
  * The Sink is a Bluetooth service which integrates HFP, HSP, A2DP, AVRCP and PBAPC, AWS MCE profiles.
  * It implements most functions of these Bluetooth profiles and provides the interface which is easier to use.
  * The Sink service works as a Bluetooth Sink device and contains many usual functions such as answer or reject
  * incoming call, get contact name of incoming call, play or pause music, move to previous or next song.
  * This section defines the Bluetooth Sink service API to use all Bluetooth Sink functions.
  * @{
 *
 * Terms and Acronyms
 * ======
 * |Terms                         |Details                                                                 |
 * |------------------------------|------------------------------------------------------------------------|
 * |\b SRV                        |Service, a common service that is based on Bluetooth profile. |
 * |\b TWC                       | Three-way call, a state of sink status. |
 * |\b AG                          | Audio Gateway, the remote device.|
 * |\b DTMF                      | Dual Tone Multi-Frequency, is an in-band telecommunication signaling system using the voice-frequency band over telephone lines. For more information, please refer to <a href="https://en.wikipedia.org/wiki/Dual-tone_multi-frequency_signaling">Dual tone multi-frequency signaling</a> in Wikipedia. |
 *
 * @section bt_sink_srv_api_usage How to use this module
 *  - Step1: Mandatory, initialize Bluetooth sink service during system initialization.
 *   - Sample code:
 *    @code
 *           bt_sink_configurable_feature_t config;
 *           config.features = BT_SINK_CONFIGURABLE_FEATURE_AUTO_CONNECT_PBAPC; 
 *           bt_sink_srv_init(&config);
 *    @endcode
 *  - Step2: Mandatory, implement #bt_sink_srv_event_callback() to handle the sink events, such as status changed, connection information, caller information, etc.
 *   - Sample code:
 *    @code
 *       void bt_sink_srv_event_callback(bt_sink_srv_event_t event_id, void *params, uint32_t params_len);
 *       {
 *           switch (event_id)
 *           {
 *              case BT_SINK_SRV_EVENT_STATE_CHANGE:
 *                  printf("State changed, previous:0x%x, current:0x%x", param->state_change.previous, param->state_change.current);
 *                  break;
 *              case BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION:
 *                  bt_sink_app_report("Caller information, number:%s, name:%s",
 *                              event->caller_info.number,
 *                              event->caller_info.name);
 *                  break;
 *
 *              case BT_SINK_SRV_EVENT_HF_MISSED_CALL:
 *                  bt_sink_app_report("Missed call, number:%s, name:%s",
 *                              event->caller_info.number,
 *                              event->caller_info.name);
 *                  break;
 *
 *              case BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE:
 *                  bt_sink_app_report("Speaker volume, %d", event->call_volume.current_volume);
 *                  break;
 *
 *              default:
 *                  break;
 *            }
 *        }
 *    @endcode
 *  - Step3: Optional, implement #bt_sink_srv_get_hfp_custom_command_xapl_params() to enable and configure Apple-specific custom HFP AT commands.
 *               If users do not implement the function, the Sink service will use the default parameters to configure Apple-specific commands.
 *   - Sample code:
 *    @code
 *        const static bt_sink_srv_hf_custom_command_xapl_params_t default_xapl_command_params = {
 *            .vendor_infomation = "ABCD-EFGH-1234",  //ABCD is the vendor ID, EFGH is the product ID, 1234is the software version.
 *            .features = BT_SINK_SRV_HF_CUSTOM_FEATURE_BATTERY_REPORT
 *        };
 *        const bt_sink_srv_hf_custom_command_xapl_params_t *bt_sink_srv_get_hfp_custom_command_xapl_params(void)
 *        {
 *            return &default_xapl_command_params;
 *        }
 *    @endcode
 *
 *  - Step4: Optional, Call the function #bt_sink_srv_send_action() to execute a Sink operation, such as answer an incoming call.
 *   - Sample code:
 *    @code
 *           bt_status_t status = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_ANSWER, NULL);
 *    @endcode
 *
 *  - Step4: Optional, Call the function #bt_sink_srv_get_state() to get current sink state.
 *   - Sample code:
 *    @code
 *           bt_sink_srv_state_t state = bt_sink_srv_get_state();
 *    @endcode
 */

/**
 * @defgroup BluetoothServices_Sink_define Define
 * @{
 * Define Bluetooth Sink service data types and values.
 */
#define BT_SINK_SRV_MAX_PHONE_NUMBER BT_HFP_PHONE_NUMBER_MAX_LEN /**<This value is the max phone number size.*/
#define BT_SINK_SRV_MAX_NAME         (64)                        /**<This value is the max contact name size.*/

/**
 * @brief Define the Sink module ID for the Sink service.
 */
#define BT_SINK_MODULE_OFFSET   16            /**< Module range: 0xF8100000 ~ 0xF81F0000. The maximum number of modules: 16. */
#define BT_SINK_MODULE_MASK     0x000FFFFFU   /**< Mask for Bluetooth custom module. 0x000FFFFFU */
#define BT_SINK_STATUS         ((BT_MODULE_CUSTOM_SINK) | (0x0U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the Sink Status. 0xF8100000*/
#define BT_SINK_MODULE_COMMON  ((BT_MODULE_CUSTOM_SINK) | (0x1U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the Common module.  0xF8110000*/
#define BT_SINK_MODULE_HFP     ((BT_MODULE_CUSTOM_SINK) | (0x2U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the HFP module.  0xF8120000*/
#define BT_SINK_MODULE_HSP     ((BT_MODULE_CUSTOM_SINK) | (0x3U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the HSP module.  0xF8130000*/
#define BT_SINK_MODULE_A2DP    ((BT_MODULE_CUSTOM_SINK) | (0x4U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the A2DP module.  0xF8140000*/
#define BT_SINK_MODULE_AVRCP   ((BT_MODULE_CUSTOM_SINK) | (0x5U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the AVRCP module.  0xF8150000*/
#define BT_SINK_MODULE_PBAPC   ((BT_MODULE_CUSTOM_SINK) | (0x6U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the PBAPC module.  0xF8160000*/
#define BT_SINK_MODULE_AWS_MCE ((BT_MODULE_CUSTOM_SINK) | (0x7U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the AWS MCE module. 0xF8170000*/
#define BT_SINK_MODULE_LE      ((BT_MODULE_CUSTOM_SINK) | (0x8U << BT_SINK_MODULE_OFFSET)) /**< Prefix of the LE module. 0xF8180000*/
#define BT_SINK_MODULE_USER    ((BT_MODULE_CUSTOM_SINK) | (0xFU << BT_SINK_MODULE_OFFSET)) /**< Prefix of the User module for the User to customize. 0xF81F0000*/

/**
 * @brief Define the status for the Sink service. It's the customized status of the Bluetooth status #bt_status_t .
 */
#define BT_SINK_SRV_STATUS_FAIL           (BT_SINK_STATUS&0x00)  /**< The Sink Service status: fail.  0xF8100000*/
#define BT_SINK_SRV_STATUS_PENDING        (BT_SINK_STATUS&0x01)  /**< The Sink Service status: operation is pending.  0xF8100001*/
#define BT_SINK_SRV_STATUS_INVALID_PARAM  (BT_SINK_STATUS&0x02)  /**< The Sink Service status: inavlid parameters.  0xF8100002*/
#define BT_SINK_SRV_STATUS_INVALID_STATUS (BT_SINK_STATUS&0x03)  /**< The Sink Service status: invalid status.  0xF8100003*/

/**
 *  @brief  Define the avrcp operation state type.
 */
typedef uint8_t bt_sink_srv_avrcp_operation_state_t;
#define BT_SINK_SRV_AVRCP_OPERATION_PRESS       0x00/**< The avrcp operation press action*/
#define BT_SINK_SRV_AVRCP_OPERATION_RELEASE    0x01/**< The avrcp operation release action*/

/**
 * @brief Define the sub type action and event for the Sink Module ID.
 * +----------+--------------+--------- +---------- +
 * |Sink  ID  |   Module ID  | Sub Type |   Vaule   |
 * +----------+--------------+--------- +---------- +
 */
#define BT_SINK_MODULE_SUB_TYPE_OFFSET     13   /**< Sub type range: 0xF8X00000 ~ 0xF8X8000. The maximum number of type: 8*/

/**
 *  @brief  Define the Sink action sub type.
 */
typedef uint32_t bt_sink_srv_action_t;
#define BT_SINK_MODULE_COMMON_ACTION  ((BT_SINK_MODULE_COMMON) | (0x0U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the Common action.  0xF8110000*/ 
#define BT_SINK_MODULE_HFP_ACTION     ((BT_SINK_MODULE_HFP)    | (0x0U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the HFP action.  0xF8120000*/
#define BT_SINK_MODULE_HSP_ACTION     ((BT_SINK_MODULE_HSP)    | (0x0U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the HSP action.  0xF8130000*/
#define BT_SINK_MODULE_A2DP_ACTION    ((BT_SINK_MODULE_A2DP)   | (0x0U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the A2DP action.  0xF8140000*/
#define BT_SINK_MODULE_AVRCP_ACTION   ((BT_SINK_MODULE_AVRCP)  | (0x0U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the AVRCP action.  0xF8150000*/
#define BT_SINK_MODULE_PBAPC_ACTION   ((BT_SINK_MODULE_PBAPC)  | (0x0U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**<Prefix of the  PBAPC action.  0xF8160000*/
#define BT_SINK_MODULE_AWS_MCE_ACTION ((BT_SINK_MODULE_AWS_MCE)| (0x0U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the AWS MCE action.  0xF8170000*/
/* Common action*/
#define BT_SINK_SRV_ACTION_PROFILE_INIT       (BT_SINK_MODULE_COMMON_ACTION | 0x000U) /**< This action indicates the profile to initialize. 0xF8110000*/
#define BT_SINK_SRV_ACTION_PROFILE_CONNECT    (BT_SINK_MODULE_COMMON_ACTION | 0x001U) /**< This action indicates the profile to create connection. 0xF8110001*/
#define BT_SINK_SRV_ACTION_PROFILE_DISCONNECT (BT_SINK_MODULE_COMMON_ACTION | 0x002U) /**< This action indicates the profile to disconnect remote device. 0xF8110002*/
#define BT_SINK_SRV_ACTION_PROFILE_DEINIT     (BT_SINK_MODULE_COMMON_ACTION | 0x003U) /**< This action indicates the profile to de-initialize. 0xF8110003*/
/* HF action*/
#define BT_SINK_SRV_ACTION_ANSWER                             (BT_SINK_MODULE_HFP_ACTION | 0x001U)   /**< This action sends a request to answer an incoming call. 0xF8120001*/
#define BT_SINK_SRV_ACTION_REJECT                             (BT_SINK_MODULE_HFP_ACTION | 0x002U)   /**< This action sends a request to reject an incoming call. 0xF8120002*/
#define BT_SINK_SRV_ACTION_HANG_UP                            (BT_SINK_MODULE_HFP_ACTION | 0x003U)   /**< This action sends a request to hang up an outgoing call or an active call. 0xF8120003*/
#define BT_SINK_SRV_ACTION_DIAL_LAST                          (BT_SINK_MODULE_HFP_ACTION | 0x004U)   /**< This action sends a request to dial the last dialed number. 0xF8120004*/
#define BT_SINK_SRV_ACTION_DIAL_MISSED                        (BT_SINK_MODULE_HFP_ACTION | 0x005U)   /**< This action sends a request to dial the missed call number. 0xF8120005*/
#define BT_SINK_SRV_ACTION_QUERY_CALL_LIST                    (BT_SINK_MODULE_HFP_ACTION | 0x006U)   /**< This action sends a request to query the current call list. 0xF8120006*/
#define BT_SINK_SRV_ACTION_SWITCH_AUDIO_PATH                  (BT_SINK_MODULE_HFP_ACTION | 0x007U)   /**< This action sends a request to switch audio path. 0xF8120007*/
#define BT_SINK_SRV_ACTION_3WAY_RELEASE_ALL_HELD              (BT_SINK_MODULE_HFP_ACTION | 0x008U)   /**< This action sends a request to release all held calls or sets User Determined User Busy (UDUB) for a waiting call. 0xF8120008*/
#define BT_SINK_SRV_ACTION_3WAY_RELEASE_ACTIVE_ACCEPT_OTHER   (BT_SINK_MODULE_HFP_ACTION | 0x009U)   /**< This action sends a request to release all active calls and accept the other held or waiting call. 0xF8120009*/
#define BT_SINK_SRV_ACTION_3WAY_RELEASE_SPECIAL               (BT_SINK_MODULE_HFP_ACTION | 0x00AU)   /**< This action sends a request to release a specific call. 0xF812000A*/
#define BT_SINK_SRV_ACTION_3WAY_HOLD_ACTIVE_ACCEPT_OTHER      (BT_SINK_MODULE_HFP_ACTION | 0x00BU)   /**< This action sends a request to place all active calls on hold and accept the other held or waiting call. 0xF812000B*/
#define BT_SINK_SRV_ACTION_3WAY_HOLD_SPECIAL                  (BT_SINK_MODULE_HFP_ACTION | 0x00CU)   /**< This action sends a request to place a specific call on hold. 0xF812000C*/
#define BT_SINK_SRV_ACTION_3WAY_ADD_HELD_CALL_TO_CONVERSATION (BT_SINK_MODULE_HFP_ACTION | 0x00DU)   /**< This action sends a request to add a held call to the conversation. 0xF812000D*/
#define BT_SINK_SRV_ACTION_3WAY_EXPLICIT_CALL_TRANSFER        (BT_SINK_MODULE_HFP_ACTION | 0x00EU)   /**< This action sends a request to connect the two calls and disconnect the AG from both calls. 0xF812000E*/
#define BT_SINK_SRV_ACTION_SWITCH_AUDIO_DEVICE                (BT_SINK_MODULE_HFP_ACTION | 0x00FU)   /**< This action sends a request to switch audio between two devices. 0xF812000F*/
#define BT_SINK_SRV_ACTION_VOICE_RECOGNITION_ACTIVATE         (BT_SINK_MODULE_HFP_ACTION | 0x010U)   /**< This action sends a request to activate voice recognition. 0xF8120010*/
#define BT_SINK_SRV_ACTION_DTMF                               (BT_SINK_MODULE_HFP_ACTION | 0x011U)   /**< This action sends a request to instruct the AG to transmit a specific DTMF code to its network connection. 0xF8120011*/
#define BT_SINK_SRV_ACTION_DIAL_NUMBER                        (BT_SINK_MODULE_HFP_ACTION | 0x012U)   /**< This action sends a request to instruct the AG to dial a number. 0xF8120012*/
#define BT_SINK_SRV_ACTION_REPORT_BATTERY                     (BT_SINK_MODULE_HFP_ACTION | 0x013U)   /**< This action sends a request to report a battery level (0~9) to Apple-specific devices. 0xF8120013*/
#define BT_SINK_SRV_ACTION_CALL_VOLUME_UP                     (BT_SINK_MODULE_HFP_ACTION | 0x014U)   /**< This action sends a request to increase the call volume one level. 0xF8120014*/
#define BT_SINK_SRV_ACTION_CALL_VOLUME_DOWN                   (BT_SINK_MODULE_HFP_ACTION | 0x015U)   /**< This action sends a request to decrease the call volume one level. 0xF8120015*/
#define BT_SINK_SRV_ACTION_CALL_VOLUME_MAX                    (BT_SINK_MODULE_HFP_ACTION | 0x016U)   /**< This action sends a request to increase the call volume to the MAX. 0xF8120016*/
#define BT_SINK_SRV_ACTION_CALL_VOLUME_MIN                    (BT_SINK_MODULE_HFP_ACTION | 0x017U)   /**< This action sends a request to decrease the call volume to the MIN. 0xF8120017*/
#define BT_SINK_SRV_ACTION_HF_GET_SIRI_STATE                  (BT_SINK_MODULE_HFP_ACTION | 0x018U)   /**< This action sends a request to get the SIRI state from Apple-specific devices. 0xF8120018*/
#define BT_SINK_SRV_ACTION_HF_ECNR_ACTIVATE                   (BT_SINK_MODULE_HFP_ACTION | 0x01AU)   /**< This action sends a request to activate the echo canceling and noise reduction function. 0xF8120019*/
#define BT_SINK_SRV_ACTION_CALL_SET_VOLUME                    (BT_SINK_MODULE_HFP_ACTION | 0x01BU)   /**< This action sends a request to set the call volume. 0xF812001B*/
/* A2DP action*/
#define BT_SINK_SRV_ACTION_SET_LATENCY    (BT_SINK_MODULE_A2DP_ACTION | 0x000U) /**< This action sends a request to set sink latency. 0xF8140000*/
/* AVRCP action*/
#define BT_SINK_SRV_ACTION_PLAY           (BT_SINK_MODULE_AVRCP_ACTION | 0x000U)  /**< This action sends a request to play BT music. 0xF8150000*/
#define BT_SINK_SRV_ACTION_PAUSE          (BT_SINK_MODULE_AVRCP_ACTION | 0x001U)  /**< This action sends a request to pause BT music. 0xF8150001*/
#define BT_SINK_SRV_ACTION_NEXT_TRACK     (BT_SINK_MODULE_AVRCP_ACTION | 0x002U)  /**< This action sends a request to switch to the next song. 0xF8150002*/
#define BT_SINK_SRV_ACTION_PREV_TRACK     (BT_SINK_MODULE_AVRCP_ACTION | 0x003U) /**< This action sends a request to switch to the previous song. 0xF8150003*/
#define BT_SINK_SRV_ACTION_VOLUME_UP      (BT_SINK_MODULE_AVRCP_ACTION | 0x004U)  /**< This action sends a request to increase the music volume one level. 0xF8150004*/
#define BT_SINK_SRV_ACTION_VOLUME_DOWN    (BT_SINK_MODULE_AVRCP_ACTION | 0x005U)  /**< This action sends a request to decrease the music volume one level. 0xF8150005*/
#define BT_SINK_SRV_ACTION_VOLUME_MAX     (BT_SINK_MODULE_AVRCP_ACTION | 0x006U)  /**< This action sends a request to increase the music volume one level. 0xF8150006*/
#define BT_SINK_SRV_ACTION_VOLUME_MIN     (BT_SINK_MODULE_AVRCP_ACTION | 0x007U)  /**< This action sends a request to decrease the music volume one level. 0xF8150007*/
#define BT_SINK_SRV_ACTION_PLAY_PAUSE     (BT_SINK_MODULE_AVRCP_ACTION | 0x008U)  /**< This action sends a request to decrease the music volume one level. 0xF8150008*/
#define BT_SINK_SRV_ACTION_SET_VOLUME     (BT_SINK_MODULE_AVRCP_ACTION | 0x009U)  /**< This action sends a request to set the music volume one level. 0xF8150009*/
#define BT_SINK_SRV_ACTION_FAST_FORWARD             (BT_SINK_MODULE_AVRCP_ACTION | 0x00aU) /**< This action sends a request to fast forward with the parameter #bt_sink_srv_avrcp_operation_state_t. */
#define BT_SINK_SRV_ACTION_REWIND                   (BT_SINK_MODULE_AVRCP_ACTION | 0x00cU) /**< This action sends a request to rewind with the parameter #bt_sink_srv_avrcp_operation_state_t. */
#define BT_SINK_SRV_ACTION_GET_PLAY_STATUS          (BT_SINK_MODULE_AVRCP_ACTION | 0x00eU) /**< This action sends a request to get the playing status with the remote device`s address parameter, if the bt
                                                                                                                                                         address is added  in parameter, this action is for the the device of the remote address,
                                                                                                                                                         if the parameter is NULL, this action is for the default device. */
#define BT_SINK_SRV_ACTION_GET_CAPABILITY           (BT_SINK_MODULE_AVRCP_ACTION | 0x00fU) /**< This action sends a request to get remote device`s capability with the
                                                                                                                                                                parameter #bt_sink_srv_avrcp_get_capability_parameter_t. */
#define BT_SINK_SRV_ACTION_GET_ELEMENT_ATTRIBUTE    (BT_SINK_MODULE_AVRCP_ACTION | 0x010U) /**< This action sends a request to get remote device`s element attributes with
                                                                                                                                                                the parameter #bt_sink_srv_avrcp_get_element_attributes_parameter_t. */
#define BT_SINK_SRV_ACTION_GET_FOLDER_ITEM          (BT_SINK_MODULE_AVRCP_ACTION | 0x011U) /**< This action sends a request to get remote device`s folder item with the
                                                                                                                                                                parameter #bt_sink_srv_avrcp_get_folder_items_parameter_t*/
/* The invalid action */
#define BT_SINK_SRV_ACTION_NONE  (0x0) /**< This action is invalid for initialized. */

/**
 *  @brief Define the Sink action sub type.
 */
typedef uint32_t bt_sink_srv_event_t;
#define BT_SINK_MODULE_COMMON_EVENT   ((BT_SINK_MODULE_COMMON) | (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the Common event. 0xF8111000*/ 
#define BT_SINK_MODULE_HFP_EVENT      ((BT_SINK_MODULE_HFP)    | (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the HFP event.  0xF8121000*/
#define BT_SINK_MODULE_HSP_EVENT      ((BT_SINK_MODULE_HSP)    | (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the HSP event.  0xF8131000*/
#define BT_SINK_MODULE_A2DP_EVENT     ((BT_SINK_MODULE_A2DP)   | (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the A2DP event. 0xF8141000*/
#define BT_SINK_MODULE_AVRCP_EVENT    ((BT_SINK_MODULE_AVRCP)  | (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the AVRCP event. 0xF8151000*/
#define BT_SINK_MODULE_PBAPC_EVENT    ((BT_SINK_MODULE_PBAPC)  | (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the  PBAPC event. 0xF8161000*/
#define BT_SINK_MODULE_AWS_MCE_EVENT  ((BT_SINK_MODULE_AWS_MCE)| (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the AWS MCE event. 0xF8171000*/
#define BT_SINK_MODULE_LE_EVENT       ((BT_SINK_MODULE_LE)     | (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the LE event. 0xF8181000*/
#define BT_SINK_MODULE_USER_EVENT     ((BT_SINK_MODULE_USER)   | (0x1U << BT_SINK_MODULE_SUB_TYPE_OFFSET))  /**< Prefix of the User custom event. 0xF81F1000*/

/* Common event */
#define BT_SINK_SRV_EVENT_STATE_CHANGE     (BT_SINK_MODULE_COMMON_EVENT | 0x000U)  /**< This event indicates the status of sink service has changed. 0xF8111001*/
#define LE_SINK_SRV_EVENT_REMOTE_INFO_UPDATE     (BT_SINK_MODULE_COMMON_EVENT | 0x001U) /**< This event indicates the status of le audio . 0xF8111002*/

/* HFP event */
#define BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION         (BT_SINK_MODULE_HFP_EVENT | 0x000U)   /**< This event indicates the caller information of current incoming call. 0xF8121000*/
#define BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE      (BT_SINK_MODULE_HFP_EVENT | 0x001U)   /**< This event indicates the in-call volume has changed. 0xF8121000 */
#define BT_SINK_SRV_EVENT_HF_MISSED_CALL                (BT_SINK_MODULE_HFP_EVENT | 0x002U)   /**< This event indicates a missed call. 0xF8121002*/
#define BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED  (BT_SINK_MODULE_HFP_EVENT | 0x003U)   /**< This event indicates the status of voice recognition. 0xF8121003*/
#define BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION      (BT_SINK_MODULE_HFP_EVENT | 0x004U)   /**< This event indicates the call list information. 0xF8121004*/
#define BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE           (BT_SINK_MODULE_HFP_EVENT | 0x005U)   /**< This event indicates the state changed of the SCO/eSCO link. 0xF8121005*/
#define BT_SINK_SRV_EVENT_HF_SIRI_STATE_UPDATE          (BT_SINK_MODULE_HFP_EVENT | 0x006U)   /**< This event indicates the state of the SIRI. 0xF8121006*/
#define BT_SINK_SRV_EVENT_HF_RING_IND                   (BT_SINK_MODULE_HFP_EVENT | 0x007U)   /**< This event indicates the ring tone can be played by application. 0xF8121007*/
#define BT_SINK_SRV_EVENT_HF_TWC_RING_IND               (BT_SINK_MODULE_HFP_EVENT | 0x008U)   /**< This event indicates the three-way ring tone can be played by application. 0xF8121008*/
/* AVRCP event*/
#define BT_SINK_SRV_EVENT_AVRCP_STATUS_CHANGE           (BT_SINK_MODULE_AVRCP_EVENT | 0x000U) /**<AVRCP status change*/

#define BT_SINK_SRV_EVENT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF     (BT_SINK_MODULE_AVRCP_EVENT | 0x001U)    /**< This event indicates the result of getting element attributes with the parameter #bt_sink_srv_avrcp_get_element_attributes_cnf_t. */
#define BT_SINK_SRV_EVENT_AVRCP_GET_PLAY_STATUS_CNF            (BT_SINK_MODULE_AVRCP_EVENT | 0x002U)    /**< This event indicates the result of getting element attributes with the parameter #bt_sink_srv_avrcp_get_play_status_cnf_t. */
#define BT_SINK_SRV_EVENT_AVRCP_GET_CAPABILITY_CNF             (BT_SINK_MODULE_AVRCP_EVENT | 0x003U)    /**< This event indicates the result of getting element attributes with the parameter #bt_sink_srv_avrcp_get_capability_cnf_t. */
#define BT_SINK_SRV_EVENT_AVRCP_GET_FOLDER_ITEM_CNF            (BT_SINK_MODULE_AVRCP_EVENT | 0x004U)    /**< This event indicates the result of getting element attributes with the parameter #bt_sink_srv_avrcp_get_folder_items_cnf_t. */

/* AWS MCE event */
#define BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE         (BT_SINK_MODULE_AWS_MCE_EVENT | 0x000U)   /**< This event indicates the state change of aws mce on the Agent. 0xF8171000 */

/* LE event, the event type of the partner LE advertising feature. */
#define BT_SINK_SRV_EVENT_LE_DISCONNECT                        (BT_SINK_MODULE_LE_EVENT | 0x000U)    /**< This action indicates the application to disconnect the LE link. */
#define BT_SINK_SRV_EVENT_LE_START_CONNECTABLE_ADV             (BT_SINK_MODULE_LE_EVENT | 0x001U)    /**< This action indicates the application to enable a connectable LE advertising. */
#define BT_SINK_SRV_EVENT_LE_START_SCANNABLE_ADV               (BT_SINK_MODULE_LE_EVENT | 0x002U)    /**< This action indicates the application to enable a scannable LE advertising. */

/**
 *  @brief Define the feature configuration of the Sink.
 */
#define  BT_SINK_CONFIGURABLE_FEATURE_NONE                   (0x00000000)   /**< None. */
#define  BT_SINK_CONFIGURABLE_FEATURE_AUTO_CONNECT_PBAPC     (0x00000001)   /**< Device will connect pabpc to the remote device when HFP is connected.*/
typedef uint32_t bt_sink_configurable_feature_t;                         /**<The feature configuration of sink service. */

/**
 *  @brief Define the Apple-specific AT commands feature.
 */
#define BT_SINK_SRV_HF_CUSTOM_FEATURE_NONE              (0x00) /**< No features. */
#define BT_SINK_SRV_HF_CUSTOM_FEATURE_RESERVED          (0x01) /**< Reserved. */
#define BT_SINK_SRV_HF_CUSTOM_FEATURE_BATTERY_REPORT    (0x02) /**< Battery reporting. */
#define BT_SINK_SRV_HF_CUSTOM_FEATURE_DOCKED            (0x04) /**< Is docked or powered. */
#define BT_SINK_SRV_HF_CUSTOM_FEATURE_SIRI_REPORT       (0x08) /**< Siri status reporting. */
#define BT_SINK_SRV_HF_CUSTOM_FEATURE_NR_REPORT         (0x10) /**< Noise reduction(NR) status reporting. */
typedef uint8_t bt_sink_srv_hf_custom_feature_t;       /**< The supported features of the Apple-specific device. */

/**
 *  @brief Define for the sco connection state.
 */
#define BT_SINK_SRV_SCO_CONNECTION_STATE_DISCONNECTED  (0x00)  /**<  The disconnected state. */
#define BT_SINK_SRV_SCO_CONNECTION_STATE_CONNECTED     (0x01)  /**<  The connected state. */
typedef uint8_t bt_sink_srv_sco_connection_state_t;     /**<  The SCO connection state. */

/**
 *  @brief Define the state of the sink service.
 */
#define BT_SINK_SRV_STATE_NONE              (0x0000) /**< Bluetooth is idle. */
#define BT_SINK_SRV_STATE_POWER_ON          (0x0001) /**< Bluetooth is powered on. */
#define BT_SINK_SRV_STATE_CONNECTED         (0x0002) /**< Device is connected.*/
#define BT_SINK_SRV_STATE_STREAMING         (0x0004) /**< BT music is playing.  */
#define BT_SINK_SRV_STATE_INCOMING          (0x0010) /**< There is an incoming call. */
#define BT_SINK_SRV_STATE_OUTGOING          (0x0020) /**< There is an outgoing call. */
#define BT_SINK_SRV_STATE_ACTIVE            (0x0040) /**< There is an active call only.*/
#define BT_SINK_SRV_STATE_TWC_INCOMING      (0x0080) /**< There is an active call and a waiting incoming call. */
#define BT_SINK_SRV_STATE_TWC_OUTGOING      (0x0100) /**< There is a held call and a outgoing call. */
#define BT_SINK_SRV_STATE_HELD_ACTIVE       (0x0200) /**< There is an active call and a held call. */
#define BT_SINK_SRV_STATE_HELD_REMAINING    (0x0400) /**< There is a held call only. */
#define BT_SINK_SRV_STATE_MULTIPARTY        (0x0800) /**< There is a conference call. */
typedef uint16_t bt_sink_srv_state_t;     /**< The state of the sink service. */


/**
 *  @brief Define the volume type.
 */
#define BT_SINK_SRV_VOLUME_HFP              (0x01)  /**< The volume type of hfp. */
#define BT_SINK_SRV_VOLUME_A2DP             (0x02)  /**< The volume type of a2dp. */
typedef uint8_t bt_sink_srv_volume_type_t;       /**< The supported volume type. */

/**
 *  @brief Define the mute type.
 */
#define BT_SINK_SRV_MUTE_MICROPHONE         (0x01)  /**< The mute type of microphone. */
#define BT_SINK_SRV_MUTE_SPEAKER            (0x02)  /**< The mute type of speaker. */
typedef uint8_t bt_sink_srv_mute_t;                 /**< The supported mute type of sink service. */

/**
 * @}
 */

/**
 * @defgroup BluetoothServices_Sink_struct Struct
 * @{
 * Define structures for the Sink service.
 */

/**
 *  @brief This structure defines the parameters for the HFP Apple-specific custom command "AT+XAPL".
 */
typedef struct {
    const char* vendor_infomation;                 /**<  A string representation of the vendor ID, the product ID and the software version from the manufacturer. The format is VendorID-ProductID-Version, ASCII format.*/
    bt_sink_srv_hf_custom_feature_t features;      /**<  The supported features. */
} bt_sink_srv_hf_custom_command_xapl_params_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_STATE_CHANGE) which indicates sink service state is changed.
 */
typedef struct {
    bt_sink_srv_state_t previous;        /**<  The previous sink service state. */
    bt_sink_srv_state_t current;         /**<  The current sink service state. */
} bt_sink_srv_state_change_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_SCO_STATE_UPDATE) which indicates SCO link state.
 */
typedef struct {
    bt_bd_addr_t                       address;  /**<  The remote device address. */
    bt_sink_srv_sco_connection_state_t state;    /**<  The sco connection state. */
    bt_hfp_audio_codec_type_t          codec;    /**<  The codec type. */
} bt_sink_srv_sco_state_update_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_CALLER_INFORMATION) which indicates caller information of the incoming call.
 */
typedef struct {
    bt_bd_addr_t          address;                                  /**<  The remote device address. */
    uint16_t              num_size;                                 /**<  The phone number size of the call. */
    uint8_t               number[BT_SINK_SRV_MAX_PHONE_NUMBER + 1]; /**<  The phone number of the call. */
    uint8_t               name[BT_SINK_SRV_MAX_NAME + 1];           /**<  The contact name of the call. */
    uint8_t               type;                                     /**<  The type of the call. */
    bool                  waiting;                                  /**<  Waiting incoming or not. */
} bt_sink_srv_caller_information_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_SPEAKER_VOLUME_CHANGE) which indicates in-call volume is changed.
 */
typedef struct {
    bt_bd_addr_t        address;             /**<  The remote device address. */
    uint8_t             current_volume;      /**<  The current speech volume. */
} bt_sink_srv_call_volume_change_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_VOICE_RECOGNITION_CHANGED) which indicates the status of voice recognition is changed.
 */
typedef struct {
    bt_bd_addr_t        address;      /**<  The remote device address. */
    bool                enable;       /**<  The voice recognition status is enabled or not. */
} bt_sink_srv_voice_recognition_status_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_HF_CALL_LIST_INFORMATION) which indicates the status of voice recognition is changed.
 */
typedef struct {
    bt_bd_addr_t          address;                                  /**<  The remote device address. */
    uint8_t               index;                                    /**<  Index of the call on the audio gateway. */
    uint8_t               director;                                 /**<  The director of the call, 0 - Mobile Originated, 1 - Mobile Terminated. */
    bt_hfp_call_status_t  state;                                    /**<  The call state of the call. */
    bt_hfp_call_mode_t    mode;                                     /**<  The call mode of the call. */
    uint8_t               multiple_party;                           /**<  If the call is multiple party, 0 - Not Multiparty, 1 - Multiparty. */
    uint16_t              num_size;                                 /**<  The phone number size of the call. */
    uint8_t               number[BT_SINK_SRV_MAX_PHONE_NUMBER];     /**<  The phone number of the call. */
    bt_hfp_phone_num_format_t   type;                               /**<  The address type of the call. */
    bool                  final;                                    /**<  The end of the call information. */
} bt_sink_srv_call_list_information_t;

/**
 *    @brief This structure is the parameters of #BT_SINK_SRV_EVENT_AWS_MCE_STATE_CHANGE
 *            which indicates the aws mce state has changed only on the  Agent side.
 */
typedef struct
{
    bt_bd_addr_t bt_addr;                     /**<  The remote device address. */
    bt_aws_mce_agent_state_type_t aws_state;  /**<  The AWS MCE state. */
} bt_sink_srv_aws_mce_state_changed_ind_t;

/**
   *  @brief This structure is the parameters of #BT_SINK_SRV_EVENT_HF_SIRI_STATE_UPDATE
   *            which indicates the Siri state.
   */
typedef struct {
    bt_bd_addr_t     address;   /**<  The remote device address. */
    uint8_t          state;    /**<  0, Not available; 1, Available but disabled; 2, Available and enabled*/
} bt_sink_srv_event_siri_state_update_t;

/**
   *  @brief This structure is the parameters of #BT_SINK_SRV_EVENT_HF_RING_IND
   *            which indicates to play local ring tone when the incoming call comes.
   */
typedef struct {
    bt_bd_addr_t     address;   /**<  The remote device address. */
} bt_sink_srv_event_hf_ring_ind_t;

/**
 *  @brief This structure is the parameters of #BT_SINK_SRV_EVENT_HF_TWC_RING_IND,
 *         which indicates to play local ring tone when the three-way incoming call comes.
 */
typedef struct {
    bool             play_vp;   /**< Three-way ring tone should be played or stopped. */
    bt_bd_addr_t     address;   /**< The remote device address. */
} bt_sink_srv_event_hf_twc_ring_ind_t;

/**
 *  @brief This structure is the parameters to send the DTMF.
 */
typedef struct {
    bt_bd_addr_t          address;   /**<  The remote device address. */
    uint8_t               code;      /**<  The DTMF number ('0' ~ '9') or '*' or '#'. */
} bt_sink_srv_send_dtmf_t;

/**
 *  @brief This structure is the parameters of #bt_sink_srv_init() which configure the features of sink service.
 */
typedef struct {
    bt_sink_configurable_feature_t features;    /**<  Sink service features. */
} bt_sink_feature_config_t;

/**
   *  @brief This structure is the parameters of #BT_SINK_SRV_EVENT_AVRCP_STATUS_CHANGE
   *            which indicates to current avrcp status.
   */
typedef struct {
    bt_bd_addr_t   address; /**< The Current N9 binary type. */
    bt_avrcp_status_t    avrcp_status; /**< The Current avrcp status. */
} bt_sink_srv_event_avrcp_status_changed_ind_t;

/**
 *  @brief This structure is the callback parameters of #bt_sink_srv_event_callback, it is the union of all the events.
 */
typedef union {
    bt_sink_srv_state_change_t                state_change;       /**< State change. */
    bt_sink_srv_event_avrcp_status_changed_ind_t avrcp_status_change;/**< Avrcp status change*/
    bt_sink_srv_caller_information_t          caller_info;        /**< Caller information. */
    bt_sink_srv_call_volume_change_t          call_volume;        /**< Call volume change. */
    bt_sink_srv_voice_recognition_status_t    voice_recognition;  /**< Voice recognition status. */
    bt_sink_srv_call_list_information_t       call_list;          /**< Call list information. */
    bt_sink_srv_sco_state_update_t            sco_state;          /**< The SCO/eSCO connection state. */
    bt_sink_srv_aws_mce_state_changed_ind_t   aws_state_change;   /**< The AWS MCE state change. */
    bt_sink_srv_event_siri_state_update_t     siri;               /**< The SIRI status update. */
    bt_sink_srv_event_hf_ring_ind_t           ring_ind;           /**< The local ring tone indication. */
    bt_sink_srv_event_hf_twc_ring_ind_t       twc_ring_ind;       /**< The local 3-way ring tone indication. */
} bt_sink_srv_event_param_t;

/**
 *  @brief This structure is the retern value of #bt_sink_srv_music_get_played_device_list.
 */
typedef struct {
    uint8_t number;             /**< The device count that connected and played devices. */
    bt_bd_addr_t device_list[BT_DEVICE_MANAGER_MAX_PAIR_NUM];  /**< The connected and played device, and first device is lasted played device. */
}bt_sink_srv_music_device_list_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_AVRCP_GET_PLAY_STATUS_CNF) which indicates play status.
 */
typedef struct {
    bt_status_t status;                                     /**< The status of getting play status*/
    bt_bd_addr_t address;                                   /**< The bt address of remote device*/
    uint32_t song_length;                                   /**< The total length (in milliseconds) of the song that is currently playing. */
    uint32_t song_position;                                 /**< The current position (in milliseconds) of the currently playing song (i.e. time elapsed). */
    bt_avrcp_media_play_status_event_t play_status;         /**< Current Status of playing media. */
} bt_sink_srv_avrcp_get_play_status_cnf_t;


#define BT_SINK_SRV_MAX_CAPABILITY_VALUE_LENGTH 0x30 /**< The max capability value count*/
/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_AVRCP_GET_CAPABILITY_CNF) which contains capability info.
 */
typedef struct {
    bt_status_t status;                                               /**< The status of getting capability*/
    bt_bd_addr_t address;                                             /**< The bt address of remote device*/
    uint8_t number;                                                   /**< The number for the capability value. */
    uint8_t capability_value[BT_SINK_SRV_MAX_CAPABILITY_VALUE_LENGTH];/**< The supported events of remote device. */
} bt_sink_srv_avrcp_get_capability_cnf_t;


/**
* @brief The struct of #BT_SINK_SRV_EVENT_AVRCP_GET_ELEMENT_ATTRIBUTES_CNF.
*/
typedef struct{
    bt_status_t status;                                                       /**< The status of getting element attribute*/
    bt_bd_addr_t address;                                                     /**< The bt address of remote device. */
    bt_avrcp_metadata_packet_type_t packet_type;                              /**< The packet type indicates if it is fragmented. */
    uint16_t length;                                                          /**< The total length of attribute_list. */
    union{
        struct{
            uint8_t number;                                                   /**< The number of members in the attribute_list. */
            bt_avrcp_get_element_attributes_response_value_t *attribute_list; /**< The list of response values for requested media attribute ID. */
        };
        uint8_t *data;                                                        /**< The fragmented data. If the packet type is BT_AVRCP_METADATA_PACKET_TYPE_CONTINUE or
                                                                                                                                    BT_AVRCP_METADATA_PACKET_TYPE_END, the application should use the data to compose a complete attribute list. */ 
    };
}bt_sink_srv_avrcp_get_element_attributes_cnf_t;

/**
*@brief The struct of #BT_SINK_SRV_EVENT_AVRCP_GET_FOLDER_ITEM_CNF.
*/
typedef struct{
    bt_status_t status;                                                 /**< The status of getting element attribute*/
    bt_bd_addr_t address;                                              /**< The bt address of remote device, if address is NULL, this action is for
                                                                                                                                default remote device. */
    uint16_t uid_counter;                                               /**< For database aware players, TG maintains a non-zero UID counter that is incremented whenever the database changes. For database unaware players uid_counter=0. */
    uint16_t number_of_items;                                           /**< The number of items returned in this listing. */
    uint16_t size_of_item_list;                                         /**< The size of the item list. */
    bt_avrcp_get_folder_items_response_value_t *items_list;             /**< The attributes returned with each item are the supported attributes from the list provided in the attribute list parameter of the request. */
}bt_sink_srv_avrcp_get_folder_items_cnf_t;

/**
* @brief The struct of #.BT_SINK_SRV_ACTION_GET_CAPABILITY
*/
typedef struct{
    bt_bd_addr_t *address;                                                     /**< The bt address of remote device, if address is NULL, this action is for 
                                                                                                                                            default remote device. */
    bt_avrcp_capability_types_t type;                                          /**< The value of attribute_list. */
}bt_sink_srv_avrcp_get_capability_parameter_t;

/**
* @brief The struct of #.BT_SINK_SRV_ACTION_GET_ELEMENT_ATTRIBUTE
*/
typedef struct{
    bt_bd_addr_t *address;                                                     /**< The bt address of remote device, if address is NULL, this action is for 
                                                                                                                                            default remote device. */
    bool accept_fragment;                                                      /**< This parameter used to indecate if user acctpts fragment packet.*/
    uint16_t attribute_size;                                                   /**< The size of attribute_list. */
    bt_avrcp_get_element_attributes_t *attribute_list;                         /**< The value of attribute_list. */
}bt_sink_srv_avrcp_get_element_attributes_parameter_t;

/**
*@brief The struct of #.BT_SINK_SRV_ACTION_GET_FOLDER_ITEM
*/
typedef struct{
    bt_bd_addr_t *address;                                             /**< The bt address of remote device, if address is NULL, this action is for 
                                                                                                                               default remote device. */
    bt_avrcp_scope_t scope;                                            /**< The scope in which the UID of the media element item or folder item is valid. */
    uint32_t start_item;                                               /**< The offset within the listing of the item, which should be the first returned item. The first element in the listing is at offset 0. */
    uint32_t end_item;                                                 /**< The offset within the listing of the item which should be the final returned item. If this is set to a value beyond what is available, the TG returns items from the given Start Item index to the index of the final item. If the End Item index is smaller than the Start Item index, the TG returns an error. If CT requests too many items, TG can respond with a sub-set of the requested items. */
}bt_sink_srv_avrcp_get_folder_items_parameter_t;

/**
 *  @brief This structure is the out parameter of #bt_sink_srv_get_device_state, which indicates the state of specified device.
 */
typedef struct {
    bt_bd_addr_t address;                                               /**< The bt address of the remote deivce. */
    bt_sink_srv_state_t music_state;                                    /**< The music state of this device. */
    bt_sink_srv_state_t call_state;                                     /**< The call state of this device. */
    bt_sink_srv_sco_connection_state_t sco_state;                       /**< The SCO state of this device. */
} bt_sink_srv_device_state_t;

/**
 * @}
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief                       This function initializes the Sink service.
 * @param[in] features          is the feature configuration of the Sink service.
 * @return                      None.
 */
void bt_sink_srv_init(bt_sink_feature_config_t *features);

/**
 * @brief                       This function is a static callback for the application to listen to the event.  It should be implemented by the application.
 * @param[in] event_id          is the callback event ID.
 * @param[in] params            is the parameter of the callback message.
 * @param[in] params_len        is the parameter length of the callback message.
 * @return                      None.
 */
void bt_sink_srv_event_callback(bt_sink_srv_event_t event_id, void *params, uint32_t params_len);


/**
 * @brief                       This function is to get the Apple-specific parameters for HFP AT commands. This API invoked by the Sink service when the HFP connection was established.
 *                              It should be implemented by the application.
 * @return                      The ios hf features parameters.
 */
const bt_sink_srv_hf_custom_command_xapl_params_t* bt_sink_srv_get_hfp_custom_command_xapl_params(void);

/**
 * @brief                       This function sends an action to the Sink service,
 *                              The Sink service executes a Sink operation to the remote device.
 * @param[in] action            is the Sink action.
 * @param[in] params            is the parameter of the Sink action.
 * @return                      #BT_STATUS_SUCCESS, the action is successfully sent.
 *                              The operation failed with the Sink status.
 */
bt_status_t bt_sink_srv_send_action(bt_sink_srv_action_t action, void *params);

/**
 * @brief                       This function get the current Sink state.
 * @return                      The current Sink state.
 */
bt_sink_srv_state_t bt_sink_srv_get_state(void);

/**
 * @brief                       This function get the default bt music volume  level, should be implemened by app, the scrop of volume level should be 0-15.
 * @param[in] support           If true, should return the default volume level when smart support absolute volume, or return the volume  level that smart phone don`t support absolute volume.
 * @return                      volume level.
 */
uint8_t bt_sink_srv_get_default_volume_level(bool support);

/**
 * @brief                       This function is used to mute bt music.
 * @param[in] is_mute           If true, mute the bt music, or resume bt music.
 * @return                      the current sink statue.
 */
bt_status_t bt_sink_srv_music_set_mute(bool is_mute);

/**
 * @brief                       Function to get ordered played device list.
 * @param[in] is_connected      If true, would retern played and connected ordered devices list, or return played ordered devices list.
 * @return                      Return the played ordered devices list.
 */
bt_sink_srv_music_device_list_t *bt_sink_srv_music_get_played_device_list(bool is_connected);

/**
 * @brief                       Function to get volume.
 * @param[in] type              The volume type.
 * @param[in] bd_addr           The remote device`s bt address, if this value is NULL, means get the volume of current playing device`s volume. 
 * @return                      Reture volume value, if this value is 0xffffffff means get volume fail.
 */
uint32_t bt_sink_srv_get_volume(bt_bd_addr_t *bd_addr, bt_sink_srv_volume_type_t type);

/**
  * @brief                      Get the target bt clock with a base bt clock and a duration.
  * @param[in] base_clk         the base bt clock, if base_clk is NULL, it means that user wants to use the current bt clock as base, or base_clk should be used as base.
  * @param[in] duration         the duration of target clock and base clock, it`s unit is us.
  * @param[out] target_clk      used to save the target bt clock after add a duration base on base clock.
  * @return                     #BT_STATUS_SUCCESS, calculate target bt clock success.
  *                             #BT_STATUS_FAIL, partner is not attached or parameters error.
  */
bt_status_t bt_sink_srv_bt_clock_addition(bt_clock_t *target_clk, bt_clock_t *base_clk, 
    uint32_t duration);

/**
  * @brief                      Switch target bt clock to target gpt count.
  * @param[in] bt_clock         the target bt clock.
  * @param[out] gpt_count       used to save the target gpt count switch from bt clock, it uses 1M gpt count.
  * @return                     #BT_STATUS_SUCCESS, switch gpt count successfully
  *                             #BT_STATUS_FAIL, bt clock is not ready, or parameter error.
  */
bt_status_t bt_sink_srv_convert_bt_clock_2_gpt_count(const bt_clock_t *bt_clock, uint32_t *gpt_count);

/**
  * @brief                      This function is used to get the state of the connected device.
  * @param[in]  device_address  is the address of the connected device, NULL means all connected devices.
  * @param[out] state_list      is an array which contains the corresponding states.
  * @param[in]  list_number     is the number of connected devices that user requests.
  * @return                     the number of connected devices returned. It is not larger than the list_number passed from user.
  */
uint32_t bt_sink_srv_get_device_state(const bt_bd_addr_t *device_address, bt_sink_srv_device_state_t *state_list, uint32_t list_number);

/**
  * @brief                      This function is used to mute microphone or speaker.
  * @param[in]  type            is the mute type.
  * @param[in]  mute            is mute or unmute.
  * @return                     #BT_STATUS_SUCCESS, mute or unmute successfully, other is failed.
  */
bt_status_t bt_sink_srv_set_mute(bt_sink_srv_mute_t type, bool mute);

#ifdef __cplusplus
}
#endif
/**
 * @}
 * @}
 * @}
 */
#endif /* __BT_SINK_SRV_H__ */

