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
 

#ifndef __BT_AWS_MCE_SRV_H__
#define __BT_AWS_MCE_SRV_H__

#include "bt_system.h"
#include "bt_type.h"
#include "bt_gap.h"
#include "bt_aws_mce.h"

/**
 * @addtogroup Bluetooth_Services_Group Bluetooth Services
 * @{
 * @addtogroup BluetoothServices_CONM BT Connection Manager
 * @{
 * @addtogroup BTConnectionManager_AWS BT Aws Mce
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
 * @defgroup BT_Aws_Mce_define Define
 * @{ 
 * Define bt aws data types and values.
 */

/**
 * @brief Define for the aws air pairing information data maximum length.
 */
#define BT_AWS_MCE_SRV_AIR_PAIRING_INFO_MAXIMUM     (16)

/**
*  @brief Define the module for the bt aws mce aws mce service.
*/
#define BT_AWS_MCE_SRV_MODULE_OFFSET (16)   /**< Module range: 0xF8400000 ~ 0xF84F0000. The maximum number of modules: 16. */
#define BT_AWS_MCE_SRV_EVENT         ((BT_MODULE_CUSTOM_AWS_MCE_SRV) | (0x01U << BT_AWS_MCE_SRV_MODULE_OFFSET)) /**< Prefix of the event.  0xF8410000*/

/**
 * @brief Define for the aws mce profile event type.
 */
#define BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_STARTED    (BT_AWS_MCE_SRV_EVENT | 0x0001U)  /**< This event indicates air pairing started with NULL data. 0xF84100001. */
#define BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE   (BT_AWS_MCE_SRV_EVENT | 0x0002U)  /**< This event indicates air pairing completed with #bt_aws_mce_srv_air_pairing_complete_ind_t. 0xF84100002. */
#define BT_AWS_MCE_SRV_EVENT_ROLE_CHANGED_IND       (BT_AWS_MCE_SRV_EVENT | 0x0003U)  /**< This event indicates the aws mce role changed with #bt_aws_mce_srv_switch_role_complete_ind_t, 0xF84100003. */
#define BT_AWS_MCE_SRV_EVENT_MODE_CHANGED_IND       (BT_AWS_MCE_SRV_EVENT | 0x0004U)  /**< This event indicates the aws mce mode changed with #bt_aws_mce_srv_mode_changed_ind_t, 0xF84100004. */
#define BT_AWS_MCE_SRV_EVENT_CASE_PAIRING_COMPLETE  (BT_AWS_MCE_SRV_EVENT | 0x0005U)  /**< This event indicates case pairing completed with #bt_aws_mce_srv_case_pairing_complete_ind_t. 0xF84100005. */
#define BT_AWS_MCE_SRV_EVENT_UNPAIR_COMPLETE        (BT_AWS_MCE_SRV_EVENT | 0x0006U)  /**< This event indicates unpair completed with NULL data. 0xF84100006. */
typedef uint32_t bt_aws_mce_srv_event_t;    /**< The type of aws mce service event. */

/**
 * @brief Define for the aws mce mode type.
 */
#define BT_AWS_MCE_SRV_MODE_NORMAL          (0x00)  /**< The define of aws mce normal mode. */
#define BT_AWS_MCE_SRV_MODE_SINGLE          (0x01)  /**< The define of aws mce single mode. */
#define BT_AWS_MCE_SRV_MODE_DOUBLE          (0x02)  /**< The define of aws mce double mode. */
#define BT_AWS_MCE_SRV_MODE_BROADCAST       (0x03)  /**< The define of aws mce broadcast mode. */
typedef uint8_t bt_aws_mce_srv_mode_t;  /**< The type of aws mce mode. */

/**
 * @brief Define for the aws mce profile service link type.
 */
#define BT_AWS_MCE_SRV_LINK_NONE            (0x00)  /**< The type of aws profile service none, means aws profile service not connected. */
#define BT_AWS_MCE_SRV_LINK_SPECIAL         (0x01)  /**< The type of aws profile service specia, means aws profile not attached at remote device. */
#define BT_AWS_MCE_SRV_LINK_NORMAL          (0x02)  /**< The type of aws profile service normal, means aws profile attached at remote device. */
typedef uint8_t bt_aws_mce_srv_link_type_t;     /**< The type of aws profile service connection state. */

/**
 * @brief  This structure is the parameters of #BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE
 *         which indicates the result of air pairing.
 */
typedef struct {
    bool                result;                 /**< The air pairing result .*/
    bt_aws_mce_role_t   cur_aws_role;           /**< The current aws role .*/
} bt_aws_mce_srv_air_pairing_complete_ind_t;

/**
 * @brief  This structure is the parameters of #BT_AWS_MCE_SRV_EVENT_CASE_PAIRING_COMPLETE
 *         which indicates the result of case pairing.
 */
typedef bt_aws_mce_srv_air_pairing_complete_ind_t bt_aws_mce_srv_case_pairing_complete_ind_t;

/**
 * @brief Define for the aws mce mode changed indicate parameters.
 */
typedef struct {
    bt_aws_mce_srv_mode_t           mode;       /**< The current aws mce mode. */
    bt_aws_mce_srv_mode_t           pre_mode;   /**< The previous aws mce mode. */
    bt_aws_mce_role_t               role;       /**< The current aws mce role. */
    bt_aws_mce_role_t               pre_role;   /**< The previous aws mce role. */
} bt_aws_mce_srv_mode_changed_ind_t;

/**
 * @brief Define for the aws mce switch mode parameters.
 */
typedef struct {
    bt_aws_mce_role_t               role;       /**< The destination role of mode switch. */
    bt_bd_addr_t                    addr;       /**< The peer address, it's invalid when the destination role is partner or client to indicate the peer agent address, otherwise this parameter will be ignored. */
} bt_aws_mce_srv_mode_switch_t;

/**
 * @brief  This structure is the parameters of #BT_AWS_MCE_SRV_EVENT_ROLE_CHANGED_IND
 *         which indicates the result swtich role.
 */
typedef bt_aws_mce_srv_air_pairing_complete_ind_t bt_aws_mce_srv_switch_role_complete_ind_t;

/**
 * @brief Define for the aws profile service air pairint parameters.
 */
typedef struct {
    uint32_t            duration;               /**< The duration of do air pairing. */
    bt_aws_mce_role_t   default_role;           /**< The device default role. */
    bt_key_t            air_pairing_key;        /**< The key to encrypt aws key. */
    uint8_t             air_pairing_info[BT_AWS_MCE_SRV_AIR_PAIRING_INFO_MAXIMUM];   /**< The information of air pairing.  add more detail*/
    int8_t              rssi_threshold;         /**< The threshold of device rssi. */
    int8_t              audio_ch;               /**< The audio output channel. */
} bt_aws_mce_srv_air_pairing_t;

/**
 * @brief Define for the aws profile service case pairing parameters.
 */
typedef struct {
    bt_aws_mce_role_t   dest_role;              /**< The device destination role. */
    bt_bd_addr_t        peer_address;           /**< The peer device's Bluetooth address. */
    bt_key_t            aws_key;                /**< The aws secret key. */
} bt_aws_mce_srv_case_pairing_t;

/**
 * @}
 */

/**
 * @brief    This function is used to report connection manager event to application, it can be implemented by user.
 * @param[in] event_id   The aws mce srv event id with type#bt_aws_mce_srv_event_t.
 * @param[in] params     The event parameters.
 * @param[in] params_len   The event parameters' length.
 * @return   void.
 */
void            bt_aws_mce_srv_event_callback(bt_aws_mce_srv_event_t event_id, void *params, uint32_t params_len);

/**
 * @brief    This function is used to ask user to decide aws role, it can be implemented by user.
 * @param[in] remote_addr    The air pairing remote device's address.
 * @return   #bt_aws_mce_role_t.
 */
bt_aws_mce_role_t
                bt_aws_mce_srv_air_pairing_get_aws_role(const bt_bd_addr_t *remote_addr);

/**
 * @brief   This function is used to start aws air pairing to pair two earbuds.
 *          If the air pairing complete, connection manager will send #BT_AWS_MCE_SRV_EVENT_AIR_PAIRING_COMPLETE event.
 * @param[in] param     The air pairing parameters.
 * @return         #BT_STATUS_SUCCESS, the operation was successful.
 *                 #BT_CM_STATUS_INVALID_PARAM, if the param of air pairing mistake.
 */
bt_status_t     bt_aws_mce_srv_air_pairing_start(const bt_aws_mce_srv_air_pairing_t *param);

/**
 * @brief   This function is used to stop aws air pairing.
 * @return     #BT_STATUS_SUCCESS, the operation was successful.
 *             #BT_STATUS_FAIL, the operation fail.
 */
bt_status_t     bt_aws_mce_srv_air_pairing_stop(void);

/**
 * @brief   This function is used to get the aws link type.
 *          If current role is client, there is not #BT_AWS_MCE_SRV_LINK_NORMAL link type.
 * @return         #BT_AWS_MCE_SRV_LINK_NONE, if the agent or partner or client not connected.
 *                 #BT_AWS_MCE_SRV_LINK_SPECIAL, if the agent or partner or client connected in special link.
 *                 #BT_AWS_MCE_SRV_LINK_NORMAL, if the agent or parnter connected in normal link.
 */
bt_aws_mce_srv_link_type_t
                bt_aws_mce_srv_get_link_type(void);

/**
 * @brief   This function is used to enable and disable aws mce feature, the default is enabled.
 *           If this function called in partner role, then the aws wide band scan will be enabled or disabled.
 *           If this function called in agent role, the the aws link setup packets sent will be enabled or disabled.
 * @param[in] enable     The action of aws mce feature.
 * @return         #BT_STATUS_SUCCESS, the operation was successful.
 */
bt_status_t     bt_aws_mce_srv_set_aws_disable(bool enable);

/**
 * @brief   This function is used to switch aws role.
 * @param[in] dest_role     The swtich request destination role.
 * @return         #BT_STATUS_SUCCESS, the operation was successful.
 */
bt_status_t     bt_aws_mce_srv_switch_role(bt_aws_mce_role_t dest_role);

/**
 * @brief   This function is used to switch aws mce mode.
 * @param[in] mode     The aws mce mode of switch to.
 * @param[in] param     The switch mode parameters.
 * @return         #BT_STATUS_SUCCESS, the operation was successful.
 */
bt_status_t     bt_aws_mce_srv_switch_mode(bt_aws_mce_srv_mode_t mode, bt_aws_mce_srv_mode_switch_t* param);

/**
 * @brief   This function gets the current aws mce mode.
 * @return         the current aws mce mode.
 */
bt_aws_mce_srv_mode_t
                bt_aws_mce_srv_get_mode(void);


/**
 * @brief   This function is used to pair two buds in the charger case.
 *          The #BT_AWS_MCE_SRV_EVENT_CASE_PAIRING_COMPLETE event will be reported to upper layer with the paring result.
 * @param[in] param     The case pairing parameters.
 * @return         #BT_STATUS_SUCCESS, the operation was successful.
 *                 #BT_CM_STATUS_INVALID_PARAM, if there is a mistake with the param of case pairing.
 */
bt_status_t     bt_aws_mce_srv_case_pairing_start(const bt_aws_mce_srv_case_pairing_t *param);

/**
 * @brief   This function is used to unpair a couple of buds.
 *          The #BT_AWS_MCE_SRV_EVENT_UNPAIR_COMPLETE event will be reported to upper layer with the result.
 * @return         #BT_STATUS_SUCCESS, the operation was successful.
 *                 #BT_CM_STATUS_PENDING, if the aws is in case pairing state.
 */
bt_status_t     bt_aws_mce_srv_unpair(void);

#ifdef __cplusplus
}
#endif
/**
 * @}
 * @}
 * @}
 */
#endif /*__BT_AWS_MCE_SRV_H__*/

