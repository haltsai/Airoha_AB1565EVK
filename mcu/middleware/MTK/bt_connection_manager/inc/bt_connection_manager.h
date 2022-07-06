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

#include "bt_gap.h"
#include "bt_type.h"
#include "bt_system.h"
#include "bt_platform.h"
#include "bt_custom_type.h"
#include "bt_connection_manager_adapt.h"

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_CONM BT Connection Manager
 * @{
 * @addtogroup BTConnectionManager_CMCM Connection Manager
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
* @defgroup Bluetooth_CM_CONM_define Define
* @{
* Define bt bonnection manager data types and values.
*/

/**
 *  @brief Define for the profile service type.
 */
typedef enum {
    BT_CM_PROFILE_SERVICE_NONE,         /**< Profile service type: none. */
    BT_CM_PROFILE_SERVICE_HFP,          /**< Profile service type: Hands-free Profile(HFP). */
    BT_CM_PROFILE_SERVICE_HSP,          /**< Profile service type: HandSet Profile(HSP). */
    BT_CM_PROFILE_SERVICE_PBAPC,        /**< Profile service type: Audio/Video Remote Control Profile(PBAPC). */
    BT_CM_PROFILE_SERVICE_A2DP_SINK,    /**< Profile service type: Advanced Audio Distribution Profile(A2DP) as sink. */
    BT_CM_PROFILE_SERVICE_A2DP_SOURCE,  /**< Profile service type: Advanced Audio Distribution Profile(A2DP) as source. */
    BT_CM_PROFILE_SERVICE_AVRCP,        /**< Profile service type: Audio/Video Remote Control Profile(AVRCP). */
    BT_CM_PROFILE_SERVICE_AIR,          /**< Profile service type: Airoha customized profile service. */
    BT_CM_PROFILE_SERVICE_AWS,          /**< Profile service type: Advanced Wireless Stero Profile(AWS). */
    BT_CM_PROFILE_SERVICE_GATT_OVER_BREDR_AIR,          /**< Profile service type: Advanced Wireless Stero Profile(AWS). */


    BT_CM_PROFILE_SERVICE_CUSTOMIZED_BEGIN,     /**< Customized profile service type start value. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_GSOUND_CONTROL = BT_CM_PROFILE_SERVICE_CUSTOMIZED_BEGIN,   /**< Customized profile service type: Gsound control. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_GSOUND_AUDIO,  /**< Customized profile service type: Gsound audio. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_AMA,           /**< Customized profile service type: AMA. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_FAST_PAIR,     /**< Customized profile service type: Fast pair. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2,          /**< Customized profile service type: IAP2. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_XIAOAI,        /**< Customized profile service type: XIAOAI. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_XIAOWEI,       /**< Customized profile service type: XIAOWEI. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_ULL,           /**< Customized profile service type: ULL. */
    BT_CM_PROFILE_SERVICE_CUSTOMIZED_END,           /**< Customized profile service type end value. */

    BT_CM_PROFILE_SERVICE_MAX = 31      /**< Profile service type maximum, should not large than 31. */
} bt_cm_profile_service_t;  /**< The profile service type. */

/**
 *  @brief Define for the profile service mask type.
 */
#define BT_CM_PROFILE_SERVICE_MASK(profile_service_type)    (0x01U << (profile_service_type))   /**< The profile_service_type shoud be #bt_cm_profile_service_t. */
#define BT_CM_PROFILE_SERVICE_MASK_ALL                      (0xFFFFFFFF)                        /**<  Masked all the registered profile service type.  */
#define BT_CM_PROFILE_SERVICE_MASK_NONE                     (0x00000000)                        /**< No profile service. */
typedef uint32_t bt_cm_profile_service_mask_t;  /**< The profile service mask type. */

/**
 *  @brief Define for the profile service connection state.
 */
#define BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED    (0x00)  /**< The profile service connection state disconnected. */
#define BT_CM_PROFILE_SERVICE_STATE_DISCONNECTING   (0x01)  /**< The profile service connection state disconnecting. */
#define BT_CM_PROFILE_SERVICE_STATE_CONNECTING      (0x02)  /**< The profile service connection state connecting. */
#define BT_CM_PROFILE_SERVICE_STATE_CONNECTED       (0x03)  /**< The profile service connection state connected. */
typedef uint8_t bt_cm_profile_service_state_t;      /**< The profile service connection state type. */

/**
 *  @brief Define for the bluetooth power state.
 */
#define BT_CM_POWER_STATE_OFF           (0x00)  /**< The bluetooth power off state. */
#define BT_CM_POWER_STATE_OFF_PENDING   (0x01)  /**< The bluetooth power off is ongoing. */
#define BT_CM_POWER_STATE_ON            (0x02)  /**< The bluetooth power on state. */
#define BT_CM_POWER_STATE_ON_PENDING    (0x03)  /**< The bluetooth power on is ongoing. */
#define BT_CM_POWER_STATE_RESETING      (0x04)  /**< The bluetooth power is reseting. */
typedef uint8_t bt_cm_power_state_t;    /**< The bluetooth power state type. */

/**
 *  @brief Define for the profile service handler type for bt connection manager.
 */
#define BT_CM_PROFILE_SERVICE_HANDLE_POWER_ON   (0x01)  /**< The type of profile service bt power on handler, with NULL data. */
#define BT_CM_PROFILE_SERVICE_HANDLE_POWER_OFF  (0x02)  /**< The type of profile service bt power off handler, with NULL data. */
#define BT_CM_PROFILE_SERVICE_HANDLE_CONNECT    (0x03)  /**< The type of profile service connect handler, with #bt_bd_addr_t data. */
#define BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT (0x04)  /**< The type of profile service disconnect handler, with #bt_bd_addr_t data. */
typedef uint8_t bt_cm_profile_service_handle_t; /**< The type of profile service handler type for bt connection manager. */

/**
 *  @brief Define for the aws link disconnect reason.
 */
#define BT_CM_AWS_LINK_DISCONNECT_REASON_MASK       (0xE0)  /**< Partner receive aws disconnect reason mask, bit0~bit2 indicate the detail reason. */
#define BT_CM_AWS_LINK_DISCONNECT_BY_SWITCH_LINK    (0x01)  /**< The aws link disconnect by switch another aws link. */
#define BT_CM_AWS_LINK_DISCONNECT_NORMAL            (0x02)  /**< The agent request disconnect aws link. */
typedef uint8_t bt_cm_aws_link_disconnect_reason_t;     /**< The aws link disconnect reason. */

/**
 *  @brief Define the callback type of profile service handler for bt connection manager.
 *  return #BT_STATUS_SUCCESS, if one profile service can deal with this handler.
 *  return #BT_STATUS_FAIL, if one profile service can't deal with this handler.
 */
typedef bt_status_t (*bt_cm_profile_service_handle_callback_t)(bt_cm_profile_service_handle_t type, void *data);

/**
 *  @brief Define for Bluetooth role.
 */
#define BT_CM_ROLE_MASTER   BT_ROLE_MASTER  /**< Master. */
#define BT_CM_ROLE_SLAVE    BT_ROLE_SLAVE   /**< Slave. */
#define BT_CM_ROLE_UNKNOWN  0xFF            /**< Unknown role. */
typedef uint8_t bt_cm_role_t;   /**< The role of bt conneciton. */

/**
 *  @brief Define the module for the bt connection manager.
 */
#define BT_CM_MODULE_OFFSET (16)            /**< Module range: 0xF8200000 ~ 0xF82F0000. The maximum number of modules: 16. */
#define BT_CM_STATUS        ((BT_MODULE_CUSTOM_CM) | (0x00U << BT_CM_MODULE_OFFSET)) /**< Prefix of the Status.  0xF8200000*/
#define BT_CM_EVENT         ((BT_MODULE_CUSTOM_CM) | (0x01U << BT_CM_MODULE_OFFSET)) /**< Prefix of the event.  0xF8210000*/

/**
 *  @brief Define for the bt connection manager status.
 */
#define BT_CM_STATUS_FAIL                   (BT_CM_STATUS | 0x0000U)  /**< The connection manager status: fail. 0xF82000000*/
#define BT_CM_STATUS_PENDING                (BT_CM_STATUS | 0x0001U)  /**< The connection manager status: operation is pending. 0xF82000001*/
#define BT_CM_STATUS_INVALID_PARAM          (BT_CM_STATUS | 0x0002U)  /**< The connection manager status: invalid parameters. 0xF82000002*/
#define BT_CM_STATUS_DB_NOT_FOUND           (BT_CM_STATUS | 0x0003U)  /**< The connection manager status: database is not found. 0xF82000003*/
#define BT_CM_STATUS_EVENT_STOP             (BT_CM_STATUS | 0x0004U)  /**< The connection manager status: event stop looping. 0xF82000004*/
#define BT_CM_STATUS_NO_REQUEST             (BT_CM_STATUS | 0x0005U)  /**< The connection manager status: no request is found. 0xF82000005*/
#define BT_CM_STATUS_LINK_EXIST             (BT_CM_STATUS | 0x0006U)  /**< The connection manager status: link is already existed. 0xF82000006*/
#define BT_CM_STATUS_MAX_LINK               (BT_CM_STATUS | 0x0007U)  /**< The connection manager status: reach the max link number. 0xF82000007*/
#define BT_CM_STATUS_NEED_RETRY             (BT_CM_STATUS | 0x0008U)  /**< The connection manager status: the request need to be retried. 0xF82000008*/
#define BT_CM_STATUS_REQUEST_EXIST          (BT_CM_STATUS | 0x0009U)  /**< The connection manager status: the request is already existed. 0xF82000009*/
#define BT_CM_STATUS_INVALID_STATUS         (BT_CM_STATUS | 0x000AU)  /**< The connection manager status: invalid status. 0xF8200000A*/
#define BT_CM_STATUS_USER_CANCEL            (BT_CM_STATUS | 0x000BU)  /**< The connection manager status: user cancel the action. 0xF8200000B*/
#define BT_CM_STATUS_STATE_ALREADY_EXIST    (BT_CM_STATUS | 0x000CU)  /**< The connection manager status: the opreation state is exist already. 0xF8200000C*/
#define BT_CM_STATUS_NOT_FOUND              (BT_CM_STATUS | 0x000DU)  /**< The connection manager status: not find the connection or remote device. 0xF8200000D*/
#define BT_CM_STATUS_CONNECTION_TAKEOVER    (BT_CM_STATUS | 0x000EU)  /**< The connection manager status: connection disconnected due to connection preemption. 0xF8200000E*/
#define BT_CM_STATUS_ROLE_RECOVERY          (BT_CM_STATUS | 0x000FU)  /**< The connection manager status: the connection was disconnected due to the recovery of the role. 0xF8200000F*/


/**
 *  @brief Define for the bt connection acl link state.
 */
#define BT_CM_ACL_LINK_DISCONNECTED         (0x00)  /**< The acl link had disconnected. */
#define BT_CM_ACL_LINK_DISCONNECTING        (0x01)  /**< The acl link is disconnecting. */
#define BT_CM_ACL_LINK_PENDING_CONNECT      (0x02)  /**< The acl link is pendig to connect. */
#define BT_CM_ACL_LINK_CONNECTING           (0x03)  /**< The acl link is connecting. */
#define BT_CM_ACL_LINK_CONNECTED            (0x04)  /**< The acl link had connected. */
#define BT_CM_ACL_LINK_ENCRYPTED            (0x05)  /**< The acl link had encrypted. */
typedef uint8_t bt_cm_acl_link_state_t; /**< The remote device address. */

/**
 *  @brief Define for the bt connection manager event.
 */
#define BT_CM_EVENT_POWER_STATE_UPDATE          (BT_CM_EVENT | 0x0000U)  /**< This event indicates the local info update with #bt_cm_power_state_update_ind_t. 0xF82100000*/
#define BT_CM_EVENT_VISIBILITY_STATE_UPDATE     (BT_CM_EVENT | 0x0001U)  /**< This event indicates the local info update with #bt_cm_visibility_state_update_ind_t. 0xF82100001*/
#define BT_CM_EVENT_REMOTE_INFO_UPDATE          (BT_CM_EVENT | 0x0002U)  /**< This event indicates the remote info update with #bt_cm_remote_info_update_ind_t. 0xF82100002*/
#define BT_CM_EVENT_PRE_CONNECT                 (BT_CM_EVENT | 0x0003U)  /**< This event indicates the connection process will be excuting with#bt_bd_addr_t. 0xF82100003*/
#define BT_CM_EVENT_RHO_STATE_UPDATE            (BT_CM_EVENT | 0x0004U)  /**< This event indicates the rho state with#bt_cm_rho_state_update_ind_t. 0xF82100004*/
typedef uint32_t bt_cm_event_t; /**< The bt connectioin manager event type. */

/**
 * @brief This structure defines the connect request parameter type.
 */
typedef struct {
    bt_bd_addr_t                    address;        /**< The connect request remote device's bluetooth address. */
    bt_cm_profile_service_mask_t    profile;        /**< The connect request profile service. */
} bt_cm_connect_t;

/**
 * @brief This structure defines the EIR information data.
 */
typedef struct {
    const uint8_t*                  data;           /**< The eir data context. */
    uint8_t                         length;         /**< The eir data length. */
} bt_cm_eir_data_t;

/**
 * @brief Define for bt connection manager configure parameters.
 */
typedef struct {
    uint8_t     max_connection_num;                 /**< The maximum connection number. */
    bool        connection_takeover;                /**< The swtich of connection takeover. */
    bt_cm_role_t    request_role;                   /**< The request role master or slave. */
    uint8_t     request_role_retry_times;           /**< The retry switch role times after switch role fail. */
    uint16_t    page_timeout;                       /**< The page timeout timer, 625us as unit.*/
    bt_cm_profile_service_mask_t power_on_reconnect_profile;    /**< The profile service mask of power on reconnect. */
    uint32_t    power_on_reconnect_duration;        /**< The power on reconnect duration, 1s as unit. */
    bt_cm_profile_service_mask_t link_loss_reconnect_profile;   /**< The profile service mask of link lost reconnect. */
    uint32_t    link_loss_reconnect_duration;       /**< The link lost reconnect duration, 1s as unit. */
    bt_cm_eir_data_t eir_data;                      /**< The extended inquiry response data. */
} bt_cm_config_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_CM_EVENT_POWER_STATE_UPDATE) which indicates power state update.
 */
typedef struct {
    bt_cm_power_state_t     power_state;            /**< The current bt power state. */
} bt_cm_power_state_update_ind_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_CM_EVENT_VISIBILITY_STATE_UPDATE) which indicates visibility state update.
 */
typedef struct {
    bool                    visibility_state;       /**< The current bt visibility state. */
} bt_cm_visibility_state_update_ind_t;

/**
 *  @brief This structure is the callback parameters type of event(#BT_CM_EVENT_REMOTE_INFO_UPDATE) which indicates remote information update.
 */
typedef struct {
    bt_bd_addr_t                    address;                /**< The remote device address. */
    bt_cm_acl_link_state_t          pre_acl_state;          /**< The previous acl link state. */
    bt_cm_acl_link_state_t          acl_state;              /**< The current acl link state. */
    bt_cm_profile_service_mask_t    pre_connected_service;  /**< The previous connected profile service mask. */
    bt_cm_profile_service_mask_t    connected_service;      /**< The current connected profile service mask. */
    bt_status_t                     reason;                 /**< The acl link or profile service disconnect or connect fail reason. */
} bt_cm_remote_info_update_ind_t;

/**
 *  @brief This structure define the detail infomation of the link.
 */
typedef struct {
    bt_gap_connection_handle_t      handle;                 /**< The link's gap handle. */
    bt_bd_addr_t                    addr;                   /**< The link's remote address. */
    bt_cm_acl_link_state_t          link_state;             /**< The link's state. */
    bt_cm_role_t                    local_role;             /**< The link's local role. */
    bt_gap_link_sniff_status_t      sniff_state;            /**< The link's sniff state. */
    bt_cm_profile_service_mask_t    connecting_mask;        /**< The connecting profile service mask on link. */
    bt_cm_profile_service_mask_t    disconnecting_mask;     /**< The disconnecting profile service mask on link. */
    bt_cm_profile_service_mask_t    connected_mask;         /**< The connected profile service mask on link. */
} bt_cm_link_info_t;


/**
 *  @brief This structure is the callback parameters type of event(#BT_CM_EVENT_RHO_STATE_UPDATE) which indicates rho state update.
 */
typedef struct {
    uint32_t            event;                              /**< The current bt visibility state. */
    bt_status_t         status;                             /**< The rho status. */
} bt_cm_rho_state_update_ind_t;

/**
 * @}
 */

/**
* @brief   This function is a static callback for the application to listen to the connection manager moudle's event. Provide a user-defined callback.
* @param[in] event_id     the event type id.
* @param[in] params       the parameters of related event..
* @param[in] params_len   the parameters's length.
* @return                       void.
*/
void            bt_cm_event_callback(bt_cm_event_t event_id, void *params, uint32_t params_len);

/**
* @brief    This function used to get takeover disconnect device, It can be implemented by user.
*           This function be called when #bt_cm_config_t connection_takeover be set to true and connected links number equal to max_connnection_num.
*           If this function not be implemented by user. It wihle disconnect the firstly connected device default when takeover occur.
*           If User return NULL as takeover disconnect device, the new connection while be disconnected.
* @return   the takeover disconnect address.
*/
bt_bd_addr_t   *bt_cm_get_takeover_disconnect_device(void);

/**
 * @brief   This function used to register profile callback which hope to managed by connection manager.
 *          If profile service registered it callback to connection manager, user can connect or disconnect connection manager common API
 *          #bt_cm_connect and #bt_cm_disconnect, and connection manager will also disconnect profile service 
 *          when user triggered bt power off.
 * @param[in] profile   is the profile type.
 * @param[in] cb        is the callback sets registered to bt connection manager.
 * @return              void.
 */
void            bt_cm_profile_service_register(bt_cm_profile_service_t profile,
                    bt_cm_profile_service_handle_callback_t cb);

/**
 * @brief   This function called by profile service used to notify connection manager itself status change.
 *          connection manager will save all registered profile services's status and update to user.
 * @param[in] profile   is the profile type.
 * @param[in] addr      the connection remote address.
 * @param[in] state     the profile service's current state.
 * @param[in] reason    the profile service disconnected reason.
 * @return              void.
 */
void            bt_cm_profile_service_status_notify(bt_cm_profile_service_t profile, bt_bd_addr_t addr,
                    bt_cm_profile_service_state_t state, bt_status_t reason);

/**
 * @brief   This function used to initiate connection manager.
 *          It call be called only in bt power off state.
 * @param[in] config    the connection manager configure.
 * @return                  void.
 */
void            bt_cm_init(const bt_cm_config_t *config);

/**
 * @brief   This function used to deinitiate connection manager.
 *          It call be called only in bt power off state.
 *          It must be called before call #bt_cm_init again.
 * @return                void.
 */
void            bt_cm_deinit(void);

/**
 * @brief   This function used to connect profile services.
 * @param[in] param    the connect parameters.
 * @return             #BT_STATUS_SUCCESS , the operation success.            
 *                     #BT_CM_STATUS_INVALID_PARAM the connect parameter is mistake.
 */
bt_status_t     bt_cm_connect(const bt_cm_connect_t *param);

/**
 * @brief   This function used to disconnect profile services.
 *            Disconnect all connection with param address as 0XFF:0xFF:0xFF:0XFF:0xFF:0xFF
 *            Disconnect all profile service with param profile as#BT_CM_PROFILE_SERVICE_MASK_ALL.
 * @param[in] param    the disconnect parameters.
 * @return             #BT_STATUS_SUCCESS , the operation success.
 *                     #BT_CM_STATUS_NOT_FOUND, the connection not found with parameter address.
 *                     #BT_CM_STATUS_INVALID_PARAM, the connect parameter is mistake.
 */
bt_status_t     bt_cm_disconnect(const bt_cm_connect_t *param);

/**
 * @brief   This function used to open or close bt visibility.
 *          If the visibility state change the #BT_CM_EVENT_VISIBILITY_STATE_UPDATE event while send to user.
 * @param[in] discoverable    open or close the visibility.
 * @return                    #BT_STATUS_SUCCESS , the operation success.            
 *                            #BT_CM_STATUS_FAIL, the operation fail.
 */
bt_status_t     bt_cm_discoverable(bool discoverable);

/**
 * @brief   This function used to get gap handle.
 * @param[in] addr   is the address.
 * @return           the gap handle.
 */
uint32_t        bt_cm_get_gap_handle(bt_bd_addr_t addr);

/**
 * @brief   This function used to get profile services connection state.
 * @param[in] addr      is the address.
 * @param[in] profile   is the profile service type.
 * @return              the profile service state, please refer to #bt_cm_profile_service_state_t.
 */
bt_cm_profile_service_state_t
                bt_cm_get_profile_service_state(bt_bd_addr_t addr, bt_cm_profile_service_t profile);

/**
 * @brief   This function used to get the connected devices list.
 *              The sequence while sorted as connection order.
 * @param[in]   profiles      is the profile masks user want to check connected.
                              if the one of the profile in the mask had connected, the result address list will add this device.
 * @param[out]  addr_list     is the buffer of connection address list.
 * @param[in]   list_num      is the addr_list number.
 * @return      the return device number.
 */
uint32_t        bt_cm_get_connected_devices(bt_cm_profile_service_mask_t profiles, bt_bd_addr_t *addr_list, uint32_t list_num);

/**
 * @brief   This function used to get connected profile services.
 * @param[in] addr  is the addr of connection.
 * @return          the connected profile service of the indicated connection by address.
 */
bt_cm_profile_service_mask_t
                bt_cm_get_connected_profile_services(bt_bd_addr_t addr);

/**
 * @brief   This function used to power on the bt.
 * @return      #BT_STATUS_SUCCESS , the operation success.
 */
bt_status_t     bt_cm_power_active(void);

/**
 * @brief   This function used to power off the bt.
 * @param[in] force     if indicated to false, connection manager while disconnect all the connection before bt power off, otherwise it will not.
 * @return              #BT_STATUS_SUCCESS , the operation success.
 */
bt_status_t     bt_cm_power_standby(bool force);

/**
 * @brief   This function used to power reset the bt.
 * @param[in] force     if indicated to false, connection manager while disconnect all the connection before bt power reset, otherwise it will not.
 * @return              #BT_STATUS_SUCCESS , the operation success.
 */
bt_status_t     bt_cm_power_reset(bool force);

/**
 * @brief   This function used to get the bt power state.
 * @return  the bt power state, please refer to #bt_cm_power_state_t.
 */
bt_cm_power_state_t
                bt_cm_power_get_state(void);

/**
 * @brief   This function used to get the detail information of the indicated link.
 * @param[in] addr   is the address.
 * @return           the link information, please refer to #bt_cm_link_info_t.
 */
bt_cm_link_info_t *
                bt_cm_get_link_information(bt_bd_addr_t addr);
/**
 * @brief   This function used to get gpt count for disconnected vp sync.
 * @param[in] addr   is the address.
 * @return           the gpt count.
 */
uint32_t bt_cm_get_disconnected_gpt_count(bt_bd_addr_t addr);

#ifdef __cplusplus
}
#endif
/**
 * @}
 * @}
 * @}
 */

#endif /*__BT_CONNECTION_MANAGER_H__*/

