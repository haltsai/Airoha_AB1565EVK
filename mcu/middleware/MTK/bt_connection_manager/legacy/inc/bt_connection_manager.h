/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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
 

#ifndef __BT_CONNECTION_MANAGER_H__
#define __BT_CONNECTION_MANAGER_H__

#include "bt_system.h"
#include "bt_type.h"
#include "bt_custom_type.h"
#include "bt_gap.h"
#include "bt_aws_mce.h"
#include "bt_platform.h"

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup  BluetoothServices_CONM BT Connection Manager
 * @{
 * The bt connection manager is a Bluetooth service which integrates HFP, A2DP, AVRCP and PBAPC profiles.
 * It implements most functions of these Bluetooth profiles and provides the interface which is easier to use.
 * The Sink service works as a Bluetooth headset and contains many usual functions such as answer or reject incoming call,
 * get contact name of incoming call, play or pause music, move to previous or next song,
 * reconnection when power on or link lost.
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
* @defgroup BluetoothServices_CONM_define Define
* @{
* Define sink service data types and values.
*/

#define BT_SINK_SRV_DISCONNECT_NOTIFICATION_CALLBACK_MAX   (5)   /**< The maximum number of callbacks that can be registered for #bt_sink_srv_cm_disconnect_notification_register_callback. */
#define BT_SINK_SRV_AWS_MCE_RHO_MAX_APP_DATA_LENGTH        (20)  /**<This value is the max application data length for role handover.*/

/**
 *  @brief Define for the feature configuration of bt connnection manager.
 */
#define  BT_CONNECTION_MANAGER_FEATURE_NONE                                (0x0000)   /**< None. */
#define  BT_CONNECTION_MANAGER_FEATURE_POWER_ON_RECONNECT                  (0x0001)   /**< Device will reconnect the trusted devices when Bluetooth switch on. */
#define  BT_CONNECTION_MANAGER_FEATURE_VISIBILITY                          (0x0002)   /**< Device will be discoverable. */
#define  BT_CONNECTION_MANAGER_FEATURE_RECONNECT_ON_LINK_LOSS              (0x0008)   /**< Device will reconnect the remote device when Link lost happened. */
typedef uint16_t bt_connection_manager_configurable_feature_t;                    /**<The feature configuration of sink service. */

/**
 *  @brief Define for Bluetooth role.
 */
#define BT_SINK_SRV_ROLE_MASTER   BT_ROLE_MASTER          /**< Master. */
#define BT_SINK_SRV_ROLE_SLAVE    BT_ROLE_SLAVE           /**< Slave. */
#define BT_SINK_SRV_ROLE_UNKNOWN  0xFF                    /**< Unknown role. */
typedef uint8_t bt_sink_srv_role_t;                /**< Define the sink role type. */

/**
 *  @brief Define for Bluetooth link sniff mode types.
 */
#define BT_CONNECTION_MANAGER_SNNIFF_MODE_ACTIVE        0x00     /**< Connection active mode. */
#define BT_CONNECTION_MANAGER_SNNIFF_MODE_SNIFFED       0x02     /**< Connection sniff mode. */
#define BT_CONNECTION_MANAGER_SNNIFF_MODE_SUB_SNIFFED   0x03     /**< Connection sniff subrating mode. */
typedef uint8_t bt_connection_manager_sniff_mode_t;  /**< The connection sniff mode types. */

/**
 * @brief Define the module for the connection manager.
 */
#define BT_CONNECTION_MANAGER_MODULE_OFFSET   16            /**< Module range: 0xF8200000 ~ 0xF82F0000. The maximum number of modules: 16. */
#define BT_CONNECTION_MANAGER_MODULE_MASK     0x0000FFFFU   /**< Mask for Bluetooth custom module. 0x0000FFFFU */

#define BT_CONNECTION_MANAGER_STATUS    ((BT_MODULE_CUSTOM_CONNECTION_MANAGER) | (0x00U << BT_CONNECTION_MANAGER_MODULE_OFFSET)) /**< Prefix of the Status.  0xF8200000*/
#define BT_CONNECTION_MANAGER_ACTION    ((BT_MODULE_CUSTOM_CONNECTION_MANAGER) | (0x01U << BT_CONNECTION_MANAGER_MODULE_OFFSET)) /**< Prefix of the action.  0xF8210000*/
#define BT_CONNECTION_MANAGER_EVENT     ((BT_MODULE_CUSTOM_CONNECTION_MANAGER) | (0x02U << BT_CONNECTION_MANAGER_MODULE_OFFSET)) /**< Prefix of the event.  0xF8220000*/

/**
 *  @brief  Define for the bt connection manager status.
 */
#define BT_CONNECTION_MANAGER_STATUS_FAIL                   (BT_CONNECTION_MANAGER_STATUS | 0x0000U)  /**< The connection manager  status: fail. 0xF82100000*/
#define BT_CONNECTION_MANAGER_STATUS_PENDING                (BT_CONNECTION_MANAGER_STATUS | 0x0001U)  /**< The connection manager  status: operation is pending. 0xF82100001*/
#define BT_CONNECTION_MANAGER_STATUS_INVALID_PARAM          (BT_CONNECTION_MANAGER_STATUS | 0x0002U)  /**< The connection manager  status: invalid parameters. 0xF82100002*/
#define BT_CONNECTION_MANAGER_STATUS_DB_NOT_FOUND           (BT_CONNECTION_MANAGER_STATUS | 0x0003U)  /**< The connection manager  status: database is not found. 0xF82100003*/
#define BT_CONNECTION_MANAGER_STATUS_EVENT_STOP             (BT_CONNECTION_MANAGER_STATUS | 0x0004U)  /**< The connection manager  status: event stop looping. 0xF82100004*/
#define BT_CONNECTION_MANAGER_STATUS_NO_REQUEST             (BT_CONNECTION_MANAGER_STATUS | 0x0005U)  /**< The connection manager  status: no request is found. 0xF82100005*/
#define BT_CONNECTION_MANAGER_STATUS_LINK_EXIST             (BT_CONNECTION_MANAGER_STATUS | 0x0006U)  /**< The connection manager  status: link is already existed. 0xF82100006*/
#define BT_CONNECTION_MANAGER_STATUS_MAX_LINK               (BT_CONNECTION_MANAGER_STATUS | 0x0007U)  /**< The connection manager  status: reach the max link number. 0xF82100007*/
#define BT_CONNECTION_MANAGER_STATUS_NEED_RETRY             (BT_CONNECTION_MANAGER_STATUS | 0x0008U)  /**< The connection manager  status: the request need to be retried. 0xF82100008*/
#define BT_CONNECTION_MANAGER_STATUS_REQUEST_EXIST          (BT_CONNECTION_MANAGER_STATUS | 0x0009U)  /**< The connection manager  status: the request is already existed. 0xF82100009*/
#define BT_CONNECTION_MANAGER_STATUS_INVALID_STATUS         (BT_CONNECTION_MANAGER_STATUS | 0x000AU)  /**< The connection manager  status: invalid status. 0xF8210000A*/
#define BT_CONNECTION_MANAGER_STATUS_USER_CANCEL            (BT_CONNECTION_MANAGER_STATUS | 0x000BU)  /**< The connection manager status: user cancel the action. 0xF8210000B*/
#define BT_CONNECTION_MANAGER_STATUS_STATE_ALREADY_EXIST    (BT_CONNECTION_MANAGER_STATUS | 0x000CU)  /**< The connection manager status: the opreation state is exist already. 0xF8210000C*/


/**
 *  @brief  Define for the bt connection manager action.
 */
#define BT_SINK_SRV_ACTION_DISCOVERABLE       (BT_CONNECTION_MANAGER_ACTION | 0x0000U) /**< This action send a request to make sink service discoverable.0xF82100000 */
#define BT_SINK_SRV_ACTION_CONNECT            (BT_CONNECTION_MANAGER_ACTION | 0x0001U) /**< This action send a request to create a connection. 0xF82100001*/
#define BT_SINK_SRV_ACTION_DISCONNECT         (BT_CONNECTION_MANAGER_ACTION | 0x0002U) /**< This action send a request to disconnect a device.  0xF82100002*/
#define BT_SINK_SRV_ACTION_RESET_TRUSTED_LIST (BT_CONNECTION_MANAGER_ACTION | 0x0003U) /**< This action indicates the profile to reset trusted device list.  0xF82100003*/
#define BT_SINK_SRV_ACTION_POWER_OFF          (BT_CONNECTION_MANAGER_ACTION | 0x0004U) /**< This action send a request to power off the whole system.  0xF82100004*/
#define BT_SINK_SRV_ACTION_POWER_RESET        (BT_CONNECTION_MANAGER_ACTION | 0x0005U) /**< This action send a request to reset the whole system.  0xF82100005*/
#define BT_SINK_SRV_ACTION_START_RECONNECT    (BT_CONNECTION_MANAGER_ACTION | 0x0007U) /**< This action send a request to connect device which is latest paired. 0xF82100007*/
#define BT_SINK_SRV_ACTION_CANCEL_CONNECT     (BT_CONNECTION_MANAGER_ACTION | 0x0008U) /**< This action send a request to cancel connect. 0xF82100008*/
#define BT_SINK_SRV_ACTION_BT_STANDBY         (BT_CONNECTION_MANAGER_ACTION | 0x0009U) /**< This action send a request to let bt standby. 0xF82100009*/
#define BT_SINK_SRV_ACTION_BT_ACTIVE          (BT_CONNECTION_MANAGER_ACTION | 0x000AU) /**< This action send a request to active bt.0xF8210000A */
#define BT_SINK_SRV_ACTION_BT_RESET           (BT_CONNECTION_MANAGER_ACTION | 0x000BU) /**< This action send a request to reset bt. 0xF8210000B*/
#define BT_SINK_SRV_ACTION_EXIT_DISCOVERABLE  (BT_CONNECTION_MANAGER_ACTION | 0x000CU) /**< This action send a request to exit discoverable mode. 0xF8210000C*/
#define BT_SINK_SRV_ACTION_START_AIR_PAIRING  (BT_CONNECTION_MANAGER_ACTION | 0x000DU) /**< This action send a request to start air pairing. 0xF8210000D*/
#define BT_SINK_SRV_ACTION_NOTIFY_OUT_OF_CASE (BT_CONNECTION_MANAGER_ACTION | 0x000EU) /**< This action send indicates that earbuds take from charger case. 0xF8210000E*/
#define BT_SINK_SRV_ACTION_NOTIFY_IN_CASE     (BT_CONNECTION_MANAGER_ACTION | 0x000FU) /**< This action send indicates that earbuds put into charger case. 0xF8210000F*/
#define BT_SINK_SRV_ACTION_STOP_AIR_PAIRING   (BT_CONNECTION_MANAGER_ACTION | 0x0010U) /**< This action send a request to stop air pairing. 0xF82100010*/
#define BT_SINK_SRV_ACTION_SWITCH_MODE        (BT_CONNECTION_MANAGER_ACTION | 0x0011U) /**< This action send a request to switch mode. 0xF82100011*/
#define BT_SINK_SRV_ACTION_CLASSIC_BT_STANDBY (BT_CONNECTION_MANAGER_ACTION | 0x0012U) /**< This action send a request to switch mode. 0xF82100012*/
#define BT_SINK_SRV_ACTION_ROLE_CHANGE (BT_CONNECTION_MANAGER_ACTION | 0x0013U) /**< This action send a request to switch current role. 0xF82100013*/


/**
 *  @brief  Define for the bt connection manager event.
 */
#define BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE         (BT_CONNECTION_MANAGER_EVENT | 0x0000U)  /**< This event indicates the profile connection information of current link. 0xF82200000*/
#define BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE           (BT_CONNECTION_MANAGER_EVENT | 0x0001U)  /**< This event indicates the status of visibility. 0xF82200001*/
#define BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE            (BT_CONNECTION_MANAGER_EVENT | 0x0002U)  /**< This event indicates the connection information of current link. 0xF82200002*/
#define BT_SINK_SRV_EVENT_ROLE_CHANGED_IND                  (BT_CONNECTION_MANAGER_EVENT | 0x0003U)  /**< This event indicates the current GAP role.0xF82200003*/
#define BT_SINK_SRV_EVENT_START_RECONNECTION                (BT_CONNECTION_MANAGER_EVENT | 0x0004U)  /**< This event indicates the sink service start to reconnect the remote device. 0xF82200004*/
#define BT_SINK_SRV_EVENT_PAIRED_LIST_CHANGE                (BT_CONNECTION_MANAGER_EVENT | 0x0005U)  /**< This event indicates the paired list change. 0xF82200005*/
#define BT_CONNECTION_MANAGER_EVENT_AIR_PAIRING_COMPLETE    (BT_CONNECTION_MANAGER_EVENT | 0x0006U)  /**< This event indicates the air pairing complete. 0xF82200006*/
#define BT_CONNECTION_MANAGER_EVENT_SNIFF_MODE_UPDATE       (BT_CONNECTION_MANAGER_EVENT | 0x0007U)  /**< This event indicates the sniff mode updatae. 0xF82200007*/
#define BT_CONNECTION_MANAGER_EVENT_ENCRYPTION_ON           (BT_CONNECTION_MANAGER_EVENT | 0x0008U)  /**< This event indicates the link had encrypted. 0xF82200008*/
#define BT_CONNECTION_MANAGER_EVENT_MODE_CHANGED_IND        (BT_CONNECTION_MANAGER_EVENT | 0x0009U)  /**< This event indicates the result of mode switch. 0xF82200009*/
#define BT_CONNECTION_MANAGER_EVENT_INIT_IND                (BT_CONNECTION_MANAGER_EVENT | 0x000AU)  /**< This event indicates the connection manager had been initialted. 0xF8220000A*/
/* Notify acl link state */
#define BT_CONNECTION_MANAGER_EVENT_ACL_LINK_STATE_IND                (BT_CONNECTION_MANAGER_EVENT | 0x000BU)  /**< This event indicates the acl link disconnect reason. 0xF8220000B*/

typedef uint32_t bt_connection_manager_event_t;

/**
 *  @brief Define for the profile connection state.
 */
#define BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED  (0x00)
#define BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTING    (0x01)
#define BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED     (0x02)
#define BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTING (0x03)
typedef uint8_t bt_sink_srv_profile_connection_state_t;    /**<The profile connection state. */

/**
 *  @brief Define for the profile type.
 */
#define BT_SINK_SRV_PROFILE_NONE           (0x00)  /**< Profile type: None. */
#define BT_SINK_SRV_PROFILE_HFP            (0x01)  /**< Profile type: Hands-free Profile(HFP). */
#define BT_SINK_SRV_PROFILE_A2DP_SINK      (0x02)  /**< Profile type: Advanced Audio Distribution Profile(A2DP) as sink. */
#define BT_SINK_SRV_PROFILE_AVRCP          (0x04)  /**< Profile type: Audio/Video Remote Control Profile(AVRCP). */
#define BT_SINK_SRV_PROFILE_PBAPC          (0x08)  /**< Profile type: Audio/Video Remote Control Profile(PBAPC). */
#define BT_SINK_SRV_PROFILE_HSP            (0x10)  /**< Profile type: Advanced Audio Distribution Profile(A2DP)  as source. */
#define BT_SINK_SRV_PROFILE_AWS            (0x20)  /**< Profile type: Advanced Wireless Stero Profile(AWS). */
#define BT_SINK_SRV_PROFILE_IAP2           (0x40)  /**< Profile type: iPod Accessory Protocol(iAP2). */
typedef uint8_t bt_sink_srv_profile_type_t;     /**<The feature configuration of sink service. */

#define BT_CONNECTION_MANAGER_AWS_LS_DISABLE     (0x00)
#define BT_CONNECTION_MANAGER_AWS_LS_ENABLE      (0x01)
typedef uint8_t bt_connection_manager_aws_ls_enable_t;

/**
 * @}
 */

/**
 * @defgroup BluetoothServices_CONM_define Define Define
 * @{
 * Define Bluetooth connection manager data types and values.
 */

/**
 *  @brief Define the callback for Sink service to notify applications to disconnect their own profile connection which were not managed by Sink service before Sink srv disconnect the ACL.
 */
typedef void (*bt_sink_srv_cm_disconnect_notification_callback_t)(bt_bd_addr_t *address);



/**
 * @}
 */


/**
 * @defgroup  BluetoothServices_CONM_enum Enum
 * @{
 */

/**
 * @brief This enum is sub event for #BT_SINK_SRV_EVENT_PAIRED_LIST_CHANGE.
 */
typedef enum {
    BT_SINK_SRV_PAIRED_LIST_ADD,                  /**< The paired list had add an item. */
    BT_SINK_SRV_PAIRED_LIST_DEL,                  /**< The paired list had delete an item. */
    BT_SINK_SRV_PAIRED_LIST_CLEAR                 /**< The paired list had been cleared. */
} bt_sink_srv_paired_list_change_t;

/**
 * @}
 */

/**
 * @defgroup  BluetoothServices_CONM_struct Struct
 * @{
 * Define structures for sink service APIs and events.
 */

/**
 *  @brief This structure is the parameters of #bt_connection_manager_init which configure the features of bt connection manager.
 */
typedef struct {
    bt_connection_manager_configurable_feature_t features;    /**<  bt connection manager features. */
} bt_connection_manager_features_config_t;

/**
 *  @brief This structure is the callback parameters of event, which indicated gap role
 */
typedef struct {
    bt_bd_addr_t address;   /**<  The remote device address. */
    bt_sink_srv_role_t role;
} bt_sink_srv_gap_role_information_t;

/**
 *  @brief This structure defines the parameters for the features to initiate reconnection after powered on or link loss ouccured .
 */
typedef struct {
    uint8_t attampts;                   /** <The number of reconnect before the reconnection is established.*/
} bt_sink_srv_feature_reconnect_params_t;

/**
 *  @brief This structure defines the parameters for the features to initiate reconnection after powered on or link loss ouccured .
 */
typedef struct {
    uint32_t visibility_duration;    /**<The duration . */
    bool power_on_be_visible_once;  /**< If the device will inquiry the search for nearby devices when Bluetooth switch on. */
} bt_sink_srv_feature_visibility_params_t;

/**
 * @brief This structure defines the EIR information used in the function #bt_sink_srv_get_eir().
 */
typedef struct {
    const uint8_t*    uuid_128bit;          /**< The eir data. */
    uint8_t      rssi;                        /**< The RSSI. */
}bt_sink_srv_eir_information_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_CONNECTION_INFO_UPDATE) which indicates connection status and information.
 */
typedef struct {
    bt_bd_addr_t               bt_addr;       /**<  The remote device address. */
    bt_sink_srv_profile_type_t profile_type;  /**<  The connected profile type of this device. */
} bt_sink_srv_connection_information_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_SINK_SRV_EVENT_PROFILE_CONNECTION_UPDATE) which indicates connection status and information.
 */
typedef struct {
    bt_bd_addr_t address;                           /**<  The remote device address */
    bt_sink_srv_profile_type_t profile;             /**< Profile type*/
    bt_sink_srv_profile_connection_state_t state;   /**<Profile connection state */
    bt_status_t reason;                             /**<The disconnect or connect fail reason */
} bt_sink_srv_profile_connection_state_update_t;


/**
  *  @brief This structure is the parameters of #BT_SINK_SRV_EVENT_VISIBILITY_STATE_UPDATE  which indicates the device visibility state.
*/
typedef struct {
    bool visibility_on;                 /**<  The visibility is on or off. */
} bt_sink_srv_visibility_state_update_t;

/**
 *  @brief This structure is the parameters of #BT_SINK_SRV_ACTION_PROFILE_CONNECT or #BT_SINK_SRV_ACTION_PROFILE_DISCONNECT 
 *            which indicates the profile to create or disconnect connection.
 */
typedef struct {
    bt_bd_addr_t                address;                    /**<  The remote device address. */
    bt_sink_srv_profile_type_t  profile_connection_mask;    /**<  The mask indicates which profile the device will connect. */
} bt_sink_srv_profile_connection_action_t;

/**
 *    @brief This structure is the parameters of #BT_SINK_SRV_EVENT_PAIRED_LIST_CHANGE
 *            which indicates the paired list change has changed.
 */
typedef struct {
    bt_sink_srv_paired_list_change_t type;
    bt_bd_addr_t bt_addr;
} bt_sink_srv_cm_paired_list_changed_ind_t;

/**
 *    @brief This structure is the parameters of #BT_SINK_SRV_EVENT_AWS_ROLE_HANDOVER_UPDATE
 *            which indicates the result of aws mce role handover.
 */
typedef struct {
    bool result;              /**< The role handover result. */
    bt_aws_mce_role_t role;   /**< The current role of the device.*/
    uint8_t data[BT_SINK_SRV_AWS_MCE_RHO_MAX_APP_DATA_LENGTH]; /**< The received rho data for the partner, the data will be null for the agent. */
    uint8_t data_length;  /**< The received rho data for the partner, the length will be zero for the agent.*/
} bt_sink_srv_aws_mce_role_handover_update_t;

/**
 *    @brief This structure is the parameters of #BT_CONNECTION_MANAGER_EVENT_AIR_PAIRING_COMPLETE
 *            which indicates the result of air pairing.
 */
typedef struct {
    bool                result;         /**< The air pairing result .*/
    bt_aws_mce_role_t   cur_aws_role;   /**< The current aws role .*/
} bt_connection_manager_air_pairing_complete_ind_t;

/**
 *  @brief This structure is the parameters of #BT_SINK_SRV_ACTION_START_AIR_PAIRING
 *            which indicates start air pairing.
 */
typedef struct {
    uint32_t duration;              /**< The duration of do air pairing. */
    bt_aws_mce_role_t default_role; /**< The device default role. */
    uint8_t air_pairing_key[16];    /**< The key to encrypt aws key. */
    uint8_t air_pairing_info[16];   /**< The information of air pairing. */
    int8_t rssi_threshold;          /**< The threshold of device rssi. */
    int8_t audio_ch;                /**< The audio output channel. */
} bt_sink_srv_start_air_pairing_t;

/*
*/
typedef struct {
    bt_bd_addr_t                        address;            /**<  The connection remote device address. */
    bt_connection_manager_sniff_mode_t  sniff_mode;         /**<  The connection sniff mode. */
} bt_connection_manager_sniff_mode_update_t;

/**
 *  @brief This structure is the parameters of #BT_CONNECTION_MANAGER_EVENT_ENCRYPTION_ON
 *            which indicates the link had ecnrypted.
 */
typedef struct {
    bt_bd_addr_t                        address;            /**<  The connection remote device address. */
} bt_connection_manager_encryption_on_ind_t;

#ifdef MTK_AWS_MCE_ENABLE
#define BT_CONNECTION_MANAGER_AWS_LINK_NONE         (0x00)
#define BT_CONNECTION_MANAGER_AWS_LINK_SPECIAL      (0x01)
#define BT_CONNECTION_MANAGER_AWS_LINK_NORMAL       (0x02)
typedef uint8_t bt_connection_manager_aws_link_type_t;
#endif


/**
 *  @brief This structure is the parameters of #BT_SINK_SRV_ACTION_SWITCH_MODE 
 *            which indicates to reload n9 binary file.
 */
typedef struct {
    bt_firmware_type_t  type;        /**< The dest n9 firmware binary type. */
    bt_aws_mce_role_t   role;        /**< The dest role to switch. */
    bt_bd_addr_t        addr;        /**< The bluetooth address of when need switch to client or partner role, need set agent addr. when need to set to wpc mode agent role, need set to a fixed address. */
    bt_key_t            aws_key;     /**< Used for wpc mode.*/
} bt_connection_manager_switch_mode_params_t;

/**
   *  @brief This structure is the parameters of #BT_CONNECTION_MANAGER_EVENT_MODE_CHANGED_IND
   *            which indicates to mode was changed.
   */
typedef struct {
    bt_firmware_type_t  type;       /**< The Current N9 binary type. */
    bt_aws_mce_role_t   role;       /**< The Current AWS role. */
} bt_connection_manager_mode_changed_ind_t;

/**
  *  @brief This structure is the parameters of #BT_CONNECTION_MANAGER_EVENT_ACL_LINK_STATE_IND  which indicates the acl link state.
*/
typedef struct {
    bool                is_connected;         /**< The acl link connection result .*/
    bt_bd_addr_t address;                           /**<  The remote device address */
    bt_status_t reason;                             /**<The disconnect or connect fail reason */
} bt_connection_manager_acl_link_ind_t;

#ifdef MTK_AWS_MCE_ENABLE
#define BT_CONNECTION_MANAGER_AWS_LINK_NONE         (0x00)
#define BT_CONNECTION_MANAGER_AWS_LINK_SPECIAL      (0x01)
#define BT_CONNECTION_MANAGER_AWS_LINK_NORMAL       (0x02)
typedef uint8_t bt_connection_manager_aws_link_type_t;
#endif


/**
 * @}
 */

/**
 * @brief                             This function is to get parameters for link loss reconnection. Implementation is required.
 * @param[in] params         None.
 * @return                           A pointer to the reconnect parameters.
 */
const bt_sink_srv_feature_reconnect_params_t* bt_sink_srv_get_link_loss_reconnect_params(void);

/**
 * @brief                              This function is to get parameters for power on reconnection. Implementation is required.
 * @param[in] features         None.
 * @return                            A pointer to the reconnect parameters.
 */
const bt_sink_srv_feature_reconnect_params_t* bt_sink_srv_get_power_on_reconnect_params(void);

/**
 * @brief                              This function is to get parameters for visibility. Implementation is required.
 * @param[in] features         None.
 * @return                            A pointer to the visibility parameters.
 */
const bt_sink_srv_feature_visibility_params_t* bt_sink_srv_get_visibility_params(void);

/**
 * @brief                              This function is to get the retry times when do switch role. Implementation is required.
 * @param[in] features         None.
 * @return                            The value of retry times, 0xFF means always retry until success.
 */
uint8_t bt_sink_srv_get_role_switch_retry_times(void);

/**
 * @brief                              This function is to get the page timeout paramters. Implementation is required.
 * @param[in] features         None.
 * @return                            The value of page time out, 0x5dc0(*0.625ms = 15 seconds) is default.
 */
uint16_t bt_sink_srv_get_page_timeout_paramters(void);

/**
 * @brief                             This function is to get parameters for the EIR information. Implementation is required.
 * @param[in] features         is the initialization parameter. The value is provided once it is called by sink service.
 * @return                            A pointer to the EIR data.
 */
const bt_sink_srv_eir_information_t* bt_sink_srv_get_eir(void);

/**
 * @brief     It is used to configure the Bluetooth information.
 * @return    the default configuration information of sink service.
 */
const bt_gap_config_t *bt_sink_srv_gap_config(void);

/**
 * @brief            Gets the number of connected devices.
 * @return           The number of connected devices.
 */
uint8_t bt_sink_srv_get_connected_devices_number(void);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
void bt_connection_manager_device_local_info_store_local_address(bt_bd_addr_t *addr);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    NULL.
 */
void bt_connection_manager_device_local_info_init(void);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
bt_bd_addr_t* bt_connection_manager_device_local_info_get_local_address(void);

bt_bd_addr_t* bt_connection_manager_device_local_info_get_aws_fixed_address(void);
void bt_connection_manager_device_local_info_store_peer_aws_address(bt_bd_addr_t *addr);
bt_bd_addr_t* bt_connection_manager_device_local_info_get_peer_aws_address(void);


/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
void bt_connection_manager_device_local_info_store_aws_key(bt_key_t* aws_key);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
bt_key_t* bt_connection_manager_device_local_info_get_aws_key(void);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
void bt_connection_manager_device_local_info_store_aws_role(bt_aws_mce_role_t aws_role);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
bt_aws_mce_role_t bt_connection_manager_device_local_info_get_aws_role(void);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
bt_connection_manager_aws_ls_enable_t bt_connection_manager_device_local_info_get_aws_ls_enable(void);

/**
 * @brief                     Receive Bluetooth protocol stack events.
 * @param[in] msg             is the callback message type.
 * @param[in] status          is the status of the callback message.
 * @param[in] buffer          is the payload of the callback message.
 * @return                    The status of this operation returned from the callback.
 */
void bt_connection_manager_device_local_info_store_aws_ls_enable(bt_connection_manager_aws_ls_enable_t ls_enable);

/**
 * @brief                      Get connected profiles with the remote device.
 * @param[in] address   The remote device address. 
 * @return                    The connected profiles type with the remote device .
 */
bt_sink_srv_profile_type_t bt_sink_srv_cm_get_connected_profiles(bt_bd_addr_t *address);

/**
  * @brief                             Get the connected devices list.
  * @param[in] profile          the profile that the connected device need connected, 0 is not care profile connect state.
  * @param[out] addr_list       the list buffer of connected device address.
  * @param[in] list_size          the address number that the addr_list can fill.
  * @return                           The real count of connected device.
  */
uint32_t bt_sink_srv_cm_get_connected_device_list(bt_sink_srv_profile_type_t profile, bt_bd_addr_t *addr_list, uint32_t list_num);

bt_aws_mce_agent_state_type_t bt_sink_srv_cm_get_aws_link_state(void);

void bt_connection_manager_init(bt_connection_manager_features_config_t *features);

bt_bd_addr_t *bt_sink_srv_cm_get_aws_connected_device();

/**
 * @brief                         This function is a static callback for the application to listen to the event.  It should be implemented by the application.
 * @param[in] event_id     is the callback event ID.
 * @param[in] params       is the parameter of the callback message.
 * @param[in] params_len is the parameter length of the callback message.
 * @return                       None.
 */
void bt_connection_manager_event_callback(bt_connection_manager_event_t event_id, void *params, uint32_t params_len);

bt_aws_mce_role_t bt_connection_manager_air_pairing_get_aws_role(bt_bd_addr_t *remote_addr);

bt_status_t bt_connection_manager_exit_sniff_mode(bt_bd_addr_t *remote_addr);

#ifdef MTK_AWS_MCE_ENABLE
bt_connection_manager_aws_link_type_t bt_connection_manager_get_aws_link_type(void);
#endif


#ifdef __cplusplus
}
#endif
/**
 * @}
 * @}
 */

#endif /*__BT_CONNECTION_MANAGER_H__*/

