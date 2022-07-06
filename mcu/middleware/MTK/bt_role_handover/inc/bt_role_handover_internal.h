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

#ifndef __BT_ROLE_HANDOVER_INTERNAL_H__
#define __BT_ROLE_HANDOVER_INTERNAL_H__

#include "FreeRTOS.h"
#include "timers.h"

#include "bt_type.h"

#include "syslog.h"
#include "bt_connection_manager.h"
#include "bt_device_manager.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

#ifdef AIR_MULTI_POINT_ENABLE
#define BT_ROLE_HANDOVER_MAX_LINK_NUM       3
#define RHOS_MULTIPOINT(function)           function##_int
#else
#define BT_ROLE_HANDOVER_MAX_LINK_NUM       1
#define RHOS_MULTIPOINT(function)           function
#endif /* AIR_MULTI_POINT_ENABLE */

#define BT_ROLE_HANDOVER_FLAG_CM_PREPARE    (0x01U)
typedef uint32_t bt_role_handover_flag_t;

typedef struct {
    bt_bd_addr_t remote_addr;       /*addr needs to be changed after rho.*/
    bt_aws_mce_role_t role;
    bt_role_handover_state_t state; /*RHO is ongoing or in idle state.*/
    uint32_t aws_handle;            /*handle won't be changed after rho.*/
    uint32_t task_events;
    uint32_t rho_time;              /*for rho performance debug.*/
    bt_aws_mce_agent_state_type_t aws_state;
    bt_role_handover_flag_t flag;
    //for agent
    uint32_t prepare_pending_flag;  /*use each bit to represent each user*/
    uint32_t application_pending_flag;
    TimerHandle_t prepare_timer_handle;
    TimerHandle_t retry_timer_handle;
    //for partner
    uint8_t *rho_data;
    uint16_t total_len;
    uint8_t rho_data_len[BT_ROLE_HANDOVER_MAX_LINK_NUM * BT_ROLE_HANDOVER_MODULE_MAX];
    bt_aws_mce_role_handover_profile_t* profile_info;
}bt_role_handover_context_t;

LOG_CONTROL_BLOCK_DECLARE(BT_RHO);

extern bt_role_handover_context_t bt_rho_srv_context;
extern bt_role_handover_callbacks_t bt_rho_srv_callbacks[BT_ROLE_HANDOVER_MODULE_MAX];

void bt_role_handover_init(void);
bt_status_t bt_role_handover_start_internal(void);
bt_status_t bt_role_handover_is_user_allowed(const bt_bd_addr_t *address);
uint16_t bt_role_handover_get_user_length(const bt_bd_addr_t *address, uint8_t *length_array);
uint16_t bt_role_handover_get_user_data(const bt_bd_addr_t *address, uint8_t *data, uint8_t *length_array);
void bt_role_handover_store_profile_info(bt_aws_mce_role_handover_profile_t *profile_info);
void bt_role_handover_update_user(bt_role_handover_module_type_t type, bt_role_handover_update_info_t *update_info);
void bt_role_handover_update_agent_user(const bt_bd_addr_t *address);
void bt_role_handover_update_partner_user(const bt_bd_addr_t *address, uint8_t *data, uint16_t length);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __BT_ROLE_HANDOVER_INTERNAL_H__ */

