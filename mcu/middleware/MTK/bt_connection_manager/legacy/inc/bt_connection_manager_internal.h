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

#ifndef __BT_SINK_SRV_CONMGR_H__
#define __BT_SINK_SRV_CONMGR_H__

#include "bt_sink_srv.h"
#include "bt_connection_manager.h"


#ifdef __cplusplus
extern "C" {
#endif

#define BT_SINK_SRV_CM_END_AIR_PAIRING_TIMER_DUR (5000)
#define BT_SINK_SRV_CM_PARTNER_RECOVER_TO_AGENT_TIMER_DUR  (60000)
#define BT_SINK_SRV_CM_TO_DO_BT_POWER_OFF_TIMER_DUR        (100)

#define BT_SINK_SRV_CM_MAX_PROFILE_NUMBER         6
#ifdef MTK_AWS_MCE_ENABLE
#define BT_SINK_SRV_CM_MAX_DEVICE_NUMBER          3
#else
#define BT_SINK_SRV_CM_MAX_DEVICE_NUMBER          3
#endif
#define BT_SINK_SRV_CM_MAX_TRUSTED_DEV           BT_DEVICE_MANAGER_MAX_PAIR_NUM
#define BT_SINK_SRV_CM_REASON_AUTHENTICATION_FAILURE  0x05
#define BT_SINK_SRV_CM_REASON_CONNECTION_TIMEOUT  0x08
#define BT_SINK_SRV_CM_REASON_ROLE_SWITCH_PENDING 0x32
#define BT_SINK_SRV_CM_REASON_LMP_RESPONSE_TIMEOUT 0x22
#define BT_SINK_SRV_CM_DEVICE_ID_UNAVAILABLE      0xFF
#define BT_SINK_SRV_CM_PROFILE_NOTIFY             3000
#define BT_SINK_SRV_CM_POWER_ON_RECONNECT    5000
#define BT_SINK_SRV_REQUEST_DELAY_TIME                          (100)//(3000)
#define BT_SINK_SRV_LINK_LOST_RECONNECT_DELAY (5000)
#define BT_SINK_SRV_REQUEST_DELAY_TIME_INCREASE                 (3000)  //(15000)
#define BT_SINK_SRV_CM_POWER_ON_WAITING_REMOTE_AWS_CONNECTION   (100)//15000
#define BT_SINK_SRV_CM_PROFILE_ALREADY_EXIST_TIMEOUT 500


#define BT_SINK_SRV_FLAG_POWER_ON                      (0x01)
#define BT_SINK_SRV_FLAG_SET_SCAN_MODE                 (0x02)
#define BT_SINK_SRV_FLAG_SYSTEM_POWER_OFF              (0x04)
#define BT_SINK_SRV_FLAG_AWS_POWERING_ON               (0x08)
#define BT_SINK_SRV_FLAG_SYSTEM_POWER_RESET            (0x10)
#define BT_SINK_SRV_FLAG_POWER_OFF                     (0x20)
#define BT_SINK_SRV_FLAG_POWER_RESET                   (0x40)
typedef uint8_t bt_sink_srv_cm_flags_t;     /**<cm flag type. */

/**< All the profiles were connected with SP */
#define BT_SINK_SRV_CM_PROFILE_SP    (BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_A2DP_SINK | BT_SINK_SRV_PROFILE_AVRCP) 
#define BT_SINK_SRV_CM_PROFILE_SP_2  (BT_SINK_SRV_PROFILE_HSP | BT_SINK_SRV_PROFILE_A2DP_SINK | BT_SINK_SRV_PROFILE_AVRCP)
#define BT_SINK_SRV_CM_PROFILE_ALL   (BT_SINK_SRV_PROFILE_HFP | BT_SINK_SRV_PROFILE_HSP | BT_SINK_SRV_PROFILE_A2DP_SINK | BT_SINK_SRV_PROFILE_AVRCP)

/**< All the profiles were connected with AWS */
#define BT_SINK_SRV_CM_PROFILE_AWS_CLIENT  (BT_SINK_SRV_PROFILE_AWS)

/**< All the profiles were connected with AWS agent */
#define BT_SINK_SRV_CM_PROFILE_AWS_AGENT  (BT_SINK_SRV_PROFILE_AWS | BT_SINK_SRV_CM_PROFILE_SP)

typedef uint8_t bt_sink_srv_cm_device_type_t;     /**<type device type. */

/**< AACL link state */
typedef enum {
    BT_SINK_SRV_CM_STATE_DISCONNECTED,
    BT_SINK_SRV_CM_STATE_DISCONNECTING,
    BT_SINK_SRV_CM_STATE_CONNECTING,
    BT_SINK_SRV_CM_STATE_CONNECTED,
    BT_SINK_SRV_CM_STATE_ENCRYPTED
} bt_sink_srv_cm_state_t;

typedef enum {
    BT_SINK_SRV_CM_FIND_BY_HANDLE,
    BT_SINK_SRV_CM_FIND_BY_ADDR,
    BT_SINK_SRV_CM_FIND_BY_SET_AWS_STATE_TYPE,
} bt_sink_srv_cm_find_by_type_t;

typedef enum {
    BT_SINK_SRV_CM_REQ_TYPE_NONE,
    BT_SINK_SRV_CM_REQ_TYPE_NORMAL,
    BT_SINK_SRV_CM_REQ_TYPE_POWER_ON,
    BT_SINK_SRV_CM_REQ_TYPE_LINK_LOST,
    BT_SINK_SRV_CM_REQ_TYPE_TOTAL
} bt_sink_srv_cm_req_type_t;

#define BT_SINK_SRV_PROFILE_INDEX_HFP                (0) /**< Profile type: Hands-free Profile(HFP). */
#define BT_SINK_SRV_PROFILE_INDEX_A2DP_SINK          (1) /**< Profile type: Advanced Audio Distribution Profile(A2DP) as sink. */
#define BT_SINK_SRV_PROFILE_INDEX_AVRCP              (2) /**< Profile type: Audio/Video Remote Control Profile(AVRCP). */
#define BT_SINK_SRV_PROFILE_INDEX_PBAPC              (3) /**< Profile type: Audio/Video Remote Control Profile(PBAPC). */
#define BT_SINK_SRV_PROFILE_INDEX_AWS                (4) /**< Profile type: Advanced Wireless Stero Profile(AWS). */
#define BT_SINK_SRV_PROFILE_INDEX_HSP                (5) /**< Profile type: HandSet Profile(HSP). */
typedef uint8_t bt_sink_srv_cm_profile_index_t;   /**<The feature configuration of sink service. */

#define BT_SINK_SRV_ROLE_MASK_GAP   0x0F
#define BT_SINK_SRV_ROLE_MASK_AWS   0xF0

#define BT_SINK_SRV_CM_REMOTE_FLAG_ROLE_SWITCHING          0x01
#define BT_SINK_SRV_CM_REMOTE_FLAG_NEED_AWS_DISCOVERABLE   0x02
#define BT_SINK_SRV_CM_REMOTE_FLAG_DISCON_ESCO             0x04
#define BT_SINK_SRV_CM_REMOTE_FLAG_ESCO_CONNECTED          0x08
#define BT_SINK_SRV_CM_REMOTE_FLAG_CONNECTION_ABNORMAL     0x10
#define BT_SINK_SRV_CM_REMOTE_FLAG_DISABLE_SNIFF_PENDING   0x20
typedef uint8_t bt_sink_srv_cm_remote_flag_t;

#define BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_NONE     0x00
#define BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ACTIVE   0x01
#define BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_PASSIVE  0x02
#define BT_CONNECTION_MANAGER_POWER_ON_RECONNECT_ALLOW_ALL      0x03
typedef uint8_t bt_connection_manager_power_on_reconnect_allow_t;

typedef enum {
    BT_SINK_SRV_CM_SET_AWS_ENABLE_SUCCESS,
    BT_SINK_SRV_CM_SET_AWS_ENABLE_PENDING_TRUE,
    BT_SINK_SRV_CM_SET_AWS_ENABLE_PENDING_FALSE,
} bt_sink_srv_cm_set_aws_enable_t;

typedef struct {
    bt_bd_addr_t bt_addr;
} bt_sink_srv_cm_common_record;

typedef struct {
    bt_sink_srv_cm_state_t link_state; /*ACL link state*/
    bt_gap_connection_handle_t handle;
    bt_bd_addr_t addr;
    bt_sink_srv_profile_type_t profile_connection_mask;
    bt_sink_srv_profile_type_t profile_already_exist_mask;
    bt_role_t                  local_role;
    bt_sink_srv_cm_remote_flag_t flag;
    bt_sink_srv_state_t device_state;
    bt_aws_mce_agent_state_type_t aws_state;
	bt_sink_srv_cm_set_aws_enable_t set_aws_state;
    bt_gap_link_sniff_status_t sniff_state;
    uint8_t rho_sync_data_length;
    void* rho_sync_data;
    void* profile_info[BT_SINK_SRV_CM_MAX_PROFILE_NUMBER];
} bt_sink_srv_cm_remote_device_t;

typedef void (*bt_sink_srv_cm_req_callback)(bt_status_t result, void *parameters);

typedef struct {
    bt_sink_srv_cm_req_type_t type;
    bt_sink_srv_cm_req_callback callback;
    uint32_t delay_time;
    bt_sink_srv_profile_connection_action_t action_param;
    uint8_t attampts;
} bt_sink_srv_cm_request_t;

typedef struct _bt_sink_srv_cm_node_t {
    bt_sink_srv_cm_request_t *parameters;
    struct _bt_sink_srv_cm_node_t *prev;
    struct _bt_sink_srv_cm_node_t *next;
} bt_sink_srv_cm_node_t;

typedef struct {
    uint8_t speaker_volume;
} bt_sink_srv_hf_stored_data_t;

typedef struct {
    uint16_t music_volume;
    uint8_t music_mode;
    uint32_t play_order_count;
} bt_sink_srv_music_stored_data_t;

#define BT_SINK_SRV_PBAPC_STORED_COUNT  (1)
typedef struct {
    uint8_t  number[BT_SINK_SRV_MAX_PHONE_NUMBER + 1];
    uint8_t  name[BT_SINK_SRV_MAX_NAME + 1];
} bt_sink_srv_pbapc_record_t;

typedef bt_sink_srv_pbapc_record_t bt_sink_srv_pbapc_stored_data_t[BT_SINK_SRV_PBAPC_STORED_COUNT];

#define BT_SINK_SRV_HFP_STORAGE_SIZE sizeof(bt_sink_srv_hf_stored_data_t)
#define BT_SINK_SRV_A2DP_STORAGE_SIZE sizeof(bt_sink_srv_music_stored_data_t)
#define BT_SINK_SRV_PBAP_STORAGE_SIZE sizeof(bt_sink_srv_pbapc_stored_data_t)

typedef struct {
    bt_bd_addr_t        address;            /**< The remote device address. */
#ifdef BT_SINK_SRV_HFP_STORAGE_SIZE
    uint8_t hfp_info[BT_SINK_SRV_HFP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_SRV_A2DP_STORAGE_SIZE
    uint8_t a2dp_info[BT_SINK_SRV_A2DP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_SRV_AVRCP_STORAGE_SIZE
    uint8_t avrcp_info[BT_SINK_SRV_AVRCP_STORAGE_SIZE];
#endif
#ifdef BT_SINK_SRV_PBAP_STORAGE_SIZE
    uint8_t pbap_info[BT_SINK_SRV_PBAP_STORAGE_SIZE];
#endif
} bt_sink_srv_cm_dev_info_t;

typedef struct {
    bt_sink_srv_cm_flags_t flags;
    uint8_t connected_dev_num;
    uint8_t le_connected_dev_num;
    bt_sink_srv_cm_node_t *request_pending;
    bt_sink_srv_cm_node_t request_list;
    bt_gap_scan_mode_t scan_mode;
    bt_sink_srv_cm_remote_device_t devices[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER];
} bt_sink_srv_cm_context_t;

typedef struct {
    bt_sink_srv_cm_disconnect_notification_callback_t cb;
    bool is_cnf;
} bt_sink_srv_cm_disconnect_notification_context_t;

#define BT_SINK_SRV_CM_AWS_SYNC_PAIRED_ADD              0x00
#define BT_SINK_SRV_CM_AWS_SYNC_PAIRED_UNPAIR           0x01
#define BT_SINK_SRV_CM_AWS_SYNC_PAIRED_KEY_MISS         0x02
#define BT_SINK_SRV_CM_AWS_SYNC_PAIRED_LIST_FULL        0x03
#define BT_SINK_SRV_CM_AWS_SYNC_PAIRED_UNPAIR_ALL       0x04
#define BT_SINK_SRV_CM_AWS_SYNC_CLIENT_ADDR_TO_AGENT    0x05
#define BT_SINK_SRV_CM_AWS_SYNC_CONNECTION_CONFIRM      0x06
typedef uint8_t bt_sink_srv_cm_aws_sync_packet_t;   /**<The feature configuration of sink service. */

BT_PACKED(
typedef struct {
    bt_bd_addr_t               sp_addr;
    bt_sink_srv_profile_type_t profile_connection_mask;
})bt_sink_srv_cm_rho_context_t;

bt_status_t bt_sink_srv_cm_action_handler(bt_sink_srv_action_t action, void *param);
bool bt_sink_srv_cm_get_profile_data(bt_bd_addr_t *bt_addr, bt_sink_srv_profile_type_t profile, void *data_p);
bool bt_sink_srv_cm_set_profile_data(bt_bd_addr_t *bt_addr, bt_sink_srv_profile_type_t profile, void *data_p);
uint32_t bt_sink_srv_cm_get_connected_device(bt_sink_srv_profile_type_t profile, bt_bd_addr_t addr_list[BT_SINK_SRV_CM_MAX_DEVICE_NUMBER]);
bt_bd_addr_t *bt_sink_srv_cm_last_connected_device(void);
void bt_sink_srv_cm_profile_status_notify(bt_bd_addr_t *addr, bt_sink_srv_profile_type_t profile, bt_sink_srv_profile_connection_state_t state, bt_status_t reason);
void bt_sink_srv_cm_esco_state_notify(bt_bd_addr_t *addr, bt_sink_srv_sco_connection_state_t state);
bt_status_t bt_sink_srv_cm_read_local_address(uint8_t *local_addr);
bt_status_t bt_sink_srv_cm_gap_callback(bt_msg_type_t msg, bt_status_t status, void *param);
bt_status_t bt_sink_srv_cm_system_callback(bt_msg_type_t msg, bt_status_t status, void *buffer);
bt_sink_srv_state_t bt_sink_srv_cm_find_device_handle_by_address(bt_bd_addr_t *address);
void* bt_sink_srv_cm_get_hf_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_pbapc_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_avrcp_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_a2dp_sink_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_aws_info(uint8_t device_idx);
void* bt_sink_srv_cm_get_hsp_info(uint8_t device_idx);
void *bt_sink_srv_cm_get_profile_info(bt_bd_addr_t *address, bt_sink_srv_profile_type_t  profile);
bt_bd_addr_t *bt_sink_srv_cm_get_address_by_profile_info(void *profile_info);
uint32_t bt_sink_srv_cm_get_handle_by_profile_info(void *profile_info);
bt_gap_connection_handle_t bt_sink_srv_cm_get_gap_handle(bt_bd_addr_t *address_p);
bool bt_sink_srv_cm_if_permit_aws_connection(bt_bd_addr_t *address);
bt_bd_addr_t *bt_sink_srv_cm_get_aws_connected_device();
void bt_sink_srv_cm_ls_enable(bool enable);
bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_find_device(bt_sink_srv_cm_find_by_type_t type, void *param);

void bt_connection_manager_set_role(bt_bd_addr_t *address_p, bt_role_t role, bool force);
bt_role_t bt_connection_manager_get_gap_role(bt_bd_addr_t *address_p);
void bt_connection_manager_write_scan_enable_mode(bt_gap_scan_mode_t mode);
void bt_connection_manager_config_features(bt_connection_manager_features_config_t *features);
const bt_connection_manager_features_config_t *bt_connection_manager_get_features_config(void);
void bt_connection_manager_stop_switch_slave_retry(void);

void bt_connection_manager_set_page_scan(bool enable);
void bt_connection_manager_scan_mode_update();
bt_status_t bt_connection_manager_force_disconnect(bt_bd_addr_t *remote_dev);
void bt_connection_manager_allow_power_on_reconnect(bt_connection_manager_power_on_reconnect_allow_t allow);

//resturcture.
bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_get_special_aws_device();
bt_sink_srv_cm_remote_device_t *bt_sink_srv_cm_get_normal_aws_device();
bt_status_t bt_connection_manager_disconnect_all(bool force);
bt_bd_addr_t *bt_connection_mananger_get_addr_by_gap_handle(bt_gap_connection_handle_t handle);

// -- air pairing related
bt_status_t bt_connection_manager_aws_air_pairing_start(void *param);
bt_status_t bt_connection_manager_aws_air_pairing_cancel(void);
bt_status_t bt_connection_manager_aws_air_pairing_power_on_check();
bt_status_t bt_connection_manager_aws_air_pairing_event_handle(bt_msg_type_t msg, bt_status_t status, void *buffer);
bool bt_connection_manager_aws_pair_pairing_is_ongoing();

// -- rho related
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
void bt_connection_manager_aws_rho_init();
void bt_connection_manager_aws_rho_deinit();
#else
bt_status_t bt_connection_manager_aws_rho_start(void *param);
#endif

// -- power related
#define BT_CM_POWER_STATE_OFF               0x00
#define BT_CM_POWER_STATE_OFF_PENDING       0x01
#define BT_CM_POWER_STATE_ON                0x02
#define BT_CM_POWER_STATE_ON_PENDING        0x03
#define BT_CM_POWER_STATE_CLASSIC_BT_OFF    0x04
typedef uint8_t bt_cm_power_state_t;

bt_cm_power_state_t bt_connection_manager_power_get_state();
bt_status_t bt_connection_manager_power_on();
bt_status_t bt_connection_manager_power_off(bool force);
bt_status_t bt_connection_manager_power_reset(bool force);
bt_status_t bt_connection_manager_power_cancel_reset();
bt_status_t bt_connection_manager_power_sys_off();
bt_status_t bt_connection_manager_power_sys_reset();
bt_status_t bt_connection_manager_power_set_target_state(bt_cm_power_state_t state);
bt_status_t bt_connection_manager_power_event_handle(bt_msg_type_t msg, bt_status_t status, void *buffer);
bool bt_connection_manager_get_dut_mode();

#ifdef __cplusplus
}
#endif

#endif
