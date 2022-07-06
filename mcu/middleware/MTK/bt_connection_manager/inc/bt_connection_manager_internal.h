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

#include "bt_gap.h"
#include "bt_sink_srv.h"
#include "bt_connection_manager.h"
#include "bt_device_manager_power.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************** start restucture **********************/
#define BT_CM_END_AIR_PAIRING_TIMER_DUR         (3000)
#define BT_CM_PROFILE_NOTIFY_DUR                (3000)
#define BT_CM_POWER_ON_RECONNECT_DUR            (5000)
#define BT_CM_REQUEST_DELAY_TIME_DUR            (100)//(3000)
#define BT_CM_LINK_LOST_RECONNECT_DELAY_DUR     (5000)
#define BT_CM_REQUEST_DELAY_TIME_INCREASE_DUR   (3000)  //(15000)
#define BT_CM_POWER_ON_WAITING_REMOTE_AWS_CONNECTION_DUR    (100)//15000
#define BT_CM_PROFILE_ALREADY_EXIST_TIMEOUT_DUR (500)

#define BT_CM_MAX_PROFILE_NUMBER    (6)
#define BT_CM_MAX_TRUSTED_DEV       (BT_DEVICE_MANAGER_MAX_PAIR_NUM)

#define BT_CM_MAX_CALLBACK_NUMBER (5)

/**< All the profiles were connected with AWS agent */
#define BT_CM_REMOTE_FLAG_ROLE_SWITCHING        (0x01)
#define BT_CM_REMOTE_FLAG_LOCK_DISCONNECT       (0x02)
#define BT_CM_REMOTE_FLAG_CONNECT_CONFLICT      (0x04)
#define BT_CM_REMOTE_FLAG_PENDING_DISCONNECT    (0x08)
#define BT_CM_REMOTE_FLAG_RESERVE_DUE_TO_ROLE_RECOVERY  (0x10)
#define BT_CM_REMOTE_FLAG_PASSIV_CONNECTING     (0x20)
typedef uint8_t bt_cm_remote_flag_t;

#define BT_CM_FLAG_PENDING_DEINIT               (0x01)
#define BT_CM_FLAG_PENDING_SET_SCAN_MODE        (0x02)
#define BT_CM_FLAG_INITIATED                    (0x04)
typedef uint8_t bt_cm_flags_t;

#define BT_CM_POWER_TEST_SYS_OFF                (0x01)
#define BT_CM_POWER_TEST_SYS_RESET              (0x02)
typedef uint8_t bt_cm_power_test_sys_t;

#define BT_CM_FIND_BY_HANDLE                    (0x00)
#define BT_CM_FIND_BY_ADDR                      (0x01)
#define BT_CM_FIND_BY_REMOTE_FLAG               (0x02)
#define BT_CM_FIND_BY_LINK_STATE                (0x04)
typedef uint8_t bt_cm_find_t;

#define BT_CM_COMMON_TYPE_DISABLE               (0x00)
#define BT_CM_COMMON_TYPE_ENABLE                (0x01)
#define BT_CM_COMMON_TYPE_UNKNOW                (0xFF)
typedef uint8_t bt_cm_common_type_t;

#define BT_CM_RHO_PREPARE_WAIT_FLAG_SCAN_MODE           (0x01)
#define BT_CM_RHO_PREPARE_WAIT_FLAG_LINK_POLICY         (0x02)
#define BT_CM_RHO_PREPARE_WAIT_FLAG_EXIT_SNIFF          (0x04)
#define BT_CM_RHO_PREPARE_WAIT_FLAG_CANCEL_CONNECTION   (0x08)
typedef uint8_t bt_cm_rho_prepare_wait_flag_t;

extern bt_cm_rho_prepare_wait_flag_t g_bt_cm_rho_flags_t;

typedef bt_status_t (*bt_cm_event_handle_callback_t)(bt_cm_event_t event_id, void *params, uint32_t params_len);

#define     BT_CM_POWER_RESET_PROGRESS_MEDIUM       (0x00)  /**< The progress of power reset in power off complete. */
#define     BT_CM_POWER_RESET_PROGRESS_COMPLETE     (0x01)  /**< The progress of power reset in power on complete. */
typedef uint8_t bt_cm_power_reset_progress_t; /**< The progress of power reset type. */

typedef bt_status_t (*bt_cm_power_reset_callback_t)(bt_cm_power_reset_progress_t type, void *user_data);

typedef enum {
    BT_CM_LIST_CONNECTING = 0x00,
    BT_CM_LIST_CONNECTED =  0x01,

    BT_CM_LIST_TYPE_MAX
} bt_cm_list_t;

typedef struct _bt_cm_remote_device_t {
    struct _bt_cm_remote_device_t   *next[BT_CM_LIST_TYPE_MAX];
    bt_cm_link_info_t               link_info;
    bt_cm_remote_flag_t             flags;
    uint8_t                         retry_times;
    bt_cm_profile_service_mask_t    expected_connect_mask;
    bt_cm_profile_service_mask_t    request_connect_mask;
    bt_cm_profile_service_mask_t    request_disconnect_mask;
} bt_cm_remote_device_t;

typedef struct {
    uint8_t                         max_connection_num;
    uint8_t                         devices_buffer_num;
    uint8_t                         connected_dev_num;
    bt_gap_scan_mode_t              scan_mode;
    bt_cm_flags_t                   flags;
    bt_cm_remote_device_t           *handle_list[BT_CM_LIST_TYPE_MAX];
    bt_cm_profile_service_handle_callback_t profile_service_cb[BT_CM_PROFILE_SERVICE_MAX + 1];
    bt_cm_event_handle_callback_t   callback_list[BT_CM_MAX_CALLBACK_NUMBER];
    bt_cm_remote_device_t           devices_list[1];
} bt_cm_cnt_t;

#define BT_CM_LIST_ADD_FRONT    (0x01)
#define BT_CM_LIST_ADD_BACK     (0x02)
typedef uint8_t bt_cm_list_add_t;
void        bt_cm_atci_init();
void        bt_cm_switch_role(bt_bd_addr_t address, bt_role_t role);
void        bt_cm_power_init(void);
void        bt_cm_power_deinit(void);
bt_cm_remote_device_t *
            bt_cm_find_device(bt_cm_find_t find_type, void *param);
bt_status_t bt_cm_write_scan_mode_internal(bt_cm_common_type_t inquiry_scan, bt_cm_common_type_t page_scan);
void        bt_cm_power_update(void *params);
void        bt_cm_power_on_cnf(bt_device_manager_power_status_t status);
void        bt_cm_power_off_cnf(bt_device_manager_power_status_t status);
bt_status_t bt_cm_prepare_power_deinit(bool force, bt_device_manager_power_status_t reason);
void        bt_cm_power_test_sys(bt_cm_power_test_sys_t type);
bt_bd_addr_t *
            bt_cm_get_last_connected_device(void);
bt_cm_remote_device_t* bt_cm_list_get_last(bt_cm_list_t list_type);


#ifdef MTK_AWS_MCE_ENABLE
#ifdef SUPPORT_ROLE_HANDOVER_SERVICE
void        bt_cm_rho_init();
void        bt_cm_rho_deinit();
void        bt_cm_rho_gap_event_handle(bt_msg_type_t msg, bt_status_t status, void *buffer);
void        bt_aws_mce_srv_rho_complete(bt_bd_addr_t remote_addr, bool active, uint8_t aws_ready);
uint8_t     bt_aws_mce_srv_rho_get_aws_ready(bt_bd_addr_t *remote_addr);
#endif
void        bt_aws_mce_srv_init(void);
uint32_t    bt_aws_mce_srv_get_aws_handle(bt_bd_addr_t *addr);
#endif

bt_status_t bt_cm_register_event_callback(bt_cm_event_handle_callback_t cb);
void        bt_cm_register_callback_notify(bt_cm_event_t event_id, void *params, uint32_t params_len);

/* Add for temp */
void        bt_cm_power_standby_ready();
bt_status_t bt_cm_prepare_standby_callback();
bt_status_t bt_cm_power_reset_ext(bool force, bt_cm_power_reset_callback_t cb, void *user_data);
void        bt_cm_write_scan_mode(bt_cm_common_type_t discoveralbe, bt_cm_common_type_t connectable);
void        bt_cm_clear_disconnected_gpt_count(bt_bd_addr_t bd_addr);
void        bt_cm_unlock_bt_sleep_by_VP(void);

bt_status_t bt_cm_set_sniff_parameters(const bt_gap_default_sniff_params_t* params);

bt_status_t bt_cm_reset_sniff_parameters();

uint32_t    bt_cm_get_connecting_devices(bt_cm_profile_service_mask_t profiles, bt_bd_addr_t *addr_list, uint32_t list_num);


bt_status_t bt_cm_set_max_connection_number(uint8_t number, bt_bd_addr_t *keep_list, uint8_t list_size, bool if_recon);

bool bt_cm_is_disconnecting_aws_device(bt_bd_addr_t device_addr);

void bt_cm_cancel_connect_timeout_callback(void *param);



/* End */

/**
 * @brief   This function used to cancel the link connect requirement.
 * @param[in] addr    the remote device's bluetooth address, if it's set to NULL means cancel the all connect requirement.
 * @return             #BT_STATUS_SUCCESS , the operation success.            
 *                     #BT_CM_STATUS_INVALID_PARAM the connect parameter is mistake.
 */
bt_status_t bt_cm_cancel_connect(bt_bd_addr_t *addr);


bt_status_t bt_cm_disconnect_normal_first();

bt_status_t bt_cm_reconn_is_allow(bool is_allow, bool is_initiate_connect);

/**
 * @brief     This is a user defined callback to check whether to accept the connecting request or reject it.
 * @param[in] address     is the address of a connecting device.
 * @param[in] cod         is the class of a device.
 * @return    the user allow or disallow
 */
bool bt_cm_check_connect_request(bt_bd_addr_ptr_t address, uint32_t cod);



/******************** end restucture **********************/

#ifdef __cplusplus
}
#endif

#endif
