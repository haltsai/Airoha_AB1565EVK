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

#include "FreeRTOS.h"
#include "bt_hci.h"
#include "ble_ancs.h"
#include "bt_gattc.h"
#include "bt_debug.h"
#include "bt_gatts.h"
#include "bt_ancs_common.h"
#include "bt_callback_manager.h"
#include "bt_role_handover.h"
#include "syslog.h"

#if defined(MTK_AWS_MCE_ENABLE) && defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
static uint8_t ble_ancs_rho_prepare_request_needed = 0;
BT_PACKED (
typedef struct {
    uint16_t       conn_handle;             /**connection id*/
})ble_ancs_rho_context_t;

uint8_t ble_ancs_rho_need_prepare_request()
{
    return ble_ancs_rho_prepare_request_needed;
}

bt_status_t ble_ancs_rho_send_prepare_request()
{
    if (bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_ANCS_APP) == BT_STATUS_SUCCESS) {
        ble_ancs_rho_prepare_request_needed = 0;
        LOG_MSGID_I(ANCS_APP, "[ANCS_RHO] rho prepare request send success.\n", 0);
        return BT_STATUS_SUCCESS;
    } else {
        LOG_MSGID_I(ANCS_APP, "[ANCS_RHO] rho prepare request send failed.\n", 0);
        return BT_STATUS_FAIL;
    }
}

static bt_status_t ble_ancs_rho_allowed_cb(const bt_bd_addr_t *addr)
{
    if (ble_ancs_check_request_outgoing()) {
        LOG_MSGID_W(ANCS_APP, "[ANCS_RHO] ancs rho is not ready, need prepare request later.\n", 0);
        ble_ancs_rho_prepare_request_needed = 1;
        return BT_STATUS_PENDING;
    } else {
        return BT_STATUS_SUCCESS;
    }
}
static uint8_t ble_ancs_rho_get_data_length_cb(const bt_bd_addr_t *addr)
{
    if((ble_ancs_get_current_connection_handle() != 0)
#ifdef AIR_MULTI_POINT_ENABLE
		&& (addr == NULL)
#endif
    ) {
        LOG_MSGID_I(ANCS_APP, "[ANCS_RHO] get data length, current conn handle = 0x%02x.\n", 1, ble_ancs_get_current_connection_handle());
        return sizeof(ble_ancs_rho_context_t);
    }
    LOG_MSGID_I(ANCS_APP, "[ANCS_RHO] get data length is 0.\n", 0);
    return 0;
}
static bt_status_t ble_ancs_rho_get_data_cb(const bt_bd_addr_t *addr,void * data)
{
    if(data == NULL) {
        LOG_MSGID_I(ANCS_APP, "[ANCS_RHO] get data fail,data = NULL.\n", 0);
        return BT_STATUS_FAIL;
    }
    if((ble_ancs_get_current_connection_handle() != 0)
#ifdef AIR_MULTI_POINT_ENABLE
    && (addr == NULL)
#endif
    ) {
        ble_ancs_rho_context_t *context = (ble_ancs_rho_context_t *)data;
        context->conn_handle = ble_ancs_get_current_connection_handle();
        LOG_MSGID_I(ANCS_APP, "[ANCS_RHO] get data.\n", 0);
        return BT_STATUS_SUCCESS;
    }
    LOG_MSGID_I(ANCS_APP, "[ANCS_RHO] get data fail.\n", 0);
    return BT_STATUS_FAIL;
}
static bt_status_t ble_ancs_rho_update_cb(bt_role_handover_update_info_t *info)
{
    if(info == NULL) {
        LOG_MSGID_I(ANCS_APP,"[ANCS_RHO] rho update context info == NULL",0);
        return BT_STATUS_FAIL;
    }
    if(BT_AWS_MCE_ROLE_PARTNER == info->role) {
        if((info->length > 0) && (info->data)) {
            ble_ancs_rho_context_t *context = (ble_ancs_rho_context_t *)info->data;
#ifdef MTK_BLE_GAP_SRV_ENABLE
            /* Get new handle after RHO */
            bt_handle_t new_handle = bt_gap_le_srv_get_handle_by_old_handle(context->conn_handle);
            LOG_MSGID_I(BT_PORT, "[ANCS_RHO] rho update old_handle = %02x,new_handle = %02x!\r\n", 2, context->conn_handle, new_handle);
            ble_ancs_set_current_connection_handle(new_handle);
#else
            ble_ancs_set_current_connection_handle(context->conn_handle);
#endif
            LOG_MSGID_I(ANCS_APP,"[ANCS_RHO] partner rho update context,conenction_handle = %02x",1,ble_ancs_get_current_connection_handle());
        } else {
            LOG_MSGID_I(ANCS_APP,"[ANCS_RHO] partner rho update context data length == 0",0);
        }

    } else if(BT_AWS_MCE_ROLE_AGENT == info->role) {
        ble_ancs_deinit(ble_ancs_get_current_connection_handle());
        ble_ancs_set_current_connection_handle(0);
        LOG_MSGID_I(ANCS_APP,"[ANCS_RHO] Agent rho update context,conenction_handle = %02x",1,ble_ancs_get_current_connection_handle());
    }

    return BT_STATUS_SUCCESS;
}
static void ble_ancs_rho_status_cb(const bt_bd_addr_t *addr, bt_aws_mce_role_t role,
                                           bt_role_handover_event_t event, bt_status_t status)
{
    if(event == BT_ROLE_HANDOVER_COMPLETE_IND) {
        LOG_MSGID_I(ANCS_APP,"[ANCS_RHO] rho complete,status = %02x",1,status);
        if((status == BT_STATUS_SUCCESS) && (ble_ancs_get_current_connection_handle() != 0)) {
            bt_gattc_discovery_start(ble_ancs_get_current_connection_handle());
        }
    }
}
bt_role_handover_callbacks_t ble_ancs_rho_callbacks = {
    .allowed_cb = ble_ancs_rho_allowed_cb,          /*optional if always allowed*/
    .get_len_cb = ble_ancs_rho_get_data_length_cb,  /*optional if no RHO data to partner*/
    .get_data_cb = ble_ancs_rho_get_data_cb,        /*optional if no RHO data to partner*/
    .update_cb = ble_ancs_rho_update_cb,            /*optional if no RHO data to partner*/
    .status_cb = ble_ancs_rho_status_cb,            /*Mandatory for all users.*/
};
void ble_ancs_rho_register_callbacks(void)
{
#if defined(MTK_AWS_MCE_ENABLE) && defined (BT_ROLE_HANDOVER_WITH_SPP_BLE)
    LOG_MSGID_I(ANCS_APP,"[ANCS_RHO] rho init",0);
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_ANCS_APP, &ble_ancs_rho_callbacks);
#endif
}

#endif
