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
 
/*****************************************************************************
 *
 * Description:
 * ------------
 * This file implements DOGP service service structures and functions
 *
 ****************************************************************************/

#ifndef __BLE_AIR_INTERNAL_H__
#define __BLE_AIR_INTERNAL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "bt_type.h"
#include "ble_air_interface.h"

#define AIR_RX_CHAR_VALUE_HANDLE   (0x0053)   /**< Attribute Vlaue Hanlde of RX Characteristic. */
#define AIR_TX_CHAR_VALUE_HANDLE   (0x0055)   /**< Attribute Vlaue Hanlde of RX Characteristic. */
#define BLE_AIR_CCCD_NOTIFICATION  (0x0001)

/************************************************
*   structures
*************************************************/
#ifdef BLE_AIR_LOW_POWER_CONTROL
typedef enum {
    BLE_AIR_ADV_INTERVAL_DEFAULT,
    BLE_AIR_ADV_INTERVAL_FAST_CONNECT,
    BLE_AIR_ADV_INTERVAL_LOW_POWER
} ble_air_adv_interval_enum_t;
     
typedef enum {
    BLE_AIR_CONN_PARAM_PRIORITY_DEFAULT,
    BLE_AIR_CONN_PARAM_HIGH_SPEED_ANDROID, /**< please use this High speed enum, when remote device is android. */
    BLE_AIR_CONN_PARAM_HIGH_SPEED_IOS,     /**< please use this High speed enum, when remote device is IOS. */
    BLE_AIR_CONN_PARAM_LOW_POWER
} ble_air_conn_param_priority_t;

typedef struct {
    uint16_t                        conn_interval;
    ble_air_conn_param_priority_t   conn_priority;
    ble_air_adv_interval_enum_t     adv_interval;
    uint8_t                         remote_device_type;
} ble_air_low_power_cntx_t;

#endif

/**
 * structure for main cntx.
 */
typedef struct {
    bool                            is_real_connected; //After ACL link connected and first write request had come
    bool                            need_ready2write;
    uint16_t                        notify_enabled;
    uint16_t                        conn_handle;
    bt_bd_addr_t                    peer_addr; /**< Address information of the remote device. */
    uint8_t                         *receive_buffer;
    uint32_t                        receive_buffer_start;
    uint32_t                        receive_buffer_length;   
#ifdef BLE_AIR_LOW_POWER_CONTROL
    ble_air_low_power_cntx_t        low_power_t;
#endif
#ifdef MTK_AWS_MCE_ENABLE
    uint16_t                        aws_role_cccd;
#endif
    uint16_t                        revert_interval;
    uint16_t                        revert_supervision_timeout;
    bool                            is_link_optimization;
} ble_air_cntx_t;


#ifdef BLE_AIR_LOW_POWER_CONTROL
void ble_air_set_remote_device_type(uint16_t conn_handle, ble_air_remote_device_type_t type);
bt_status_t ble_air_update_connection_interval(uint16_t conn_handle);
ble_air_conn_param_priority_t ble_air_get_current_connection_interval(uint16_t conn_handle, uint16_t conn_interval);
#endif

ble_air_cntx_t *ble_air_get_cntx_by_handle(uint16_t conn_handle);
uint16_t ble_air_get_real_connected_handle(void);

#ifdef AIR_BLE_AUDIO_DONGLE_ENABLE
bt_status_t ble_air_srv_switch_link(bt_handle_t connection_handle);
#endif


bt_status_t ble_air_link_performace_optimization(void);
bt_status_t ble_air_link_performace_optimization_revert(void);

#ifdef __cplusplus
}
#endif

#endif /**__BLE_AIR_INTERNAL_H__*/




