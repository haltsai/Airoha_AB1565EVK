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

#include "bt_role_handover.h"
#include "bt_role_handover_internal.h"

BT_PACKED(
typedef struct {
    uint16_t packet_length;
    bt_bd_addr_t packet_address;
}) bt_role_handover_rho_data_t;

#define BT_ROLE_HANDOVER_SWAP_BD_ADDRESS(address1, address2) \
    bt_bd_addr_t __address = {0}; \
    memcpy(__address, (address1), sizeof(bt_bd_addr_t)); \
    memcpy((address1), (address2), sizeof(bt_bd_addr_t)); \
    memcpy((address2), __address, sizeof(bt_bd_addr_t))

#define BT_ROLE_HANDOVER_SWAP_DATA_LENGTH(array1, array2) \
    uint8_t __array[BT_ROLE_HANDOVER_MODULE_MAX] = {0}; \
    memcpy(__array, (array1), BT_ROLE_HANDOVER_MODULE_MAX * sizeof(uint8_t)); \
    memcpy((array1), (array2), BT_ROLE_HANDOVER_MODULE_MAX * sizeof(uint8_t)); \
    memcpy((array2), __array, BT_ROLE_HANDOVER_MODULE_MAX * sizeof(uint8_t))

void RHOS_MULTIPOINT(bt_role_handover_init)(void);
bt_status_t RHOS_MULTIPOINT(bt_role_handover_start_internal)(void);
bt_status_t RHOS_MULTIPOINT(bt_role_handover_is_user_allowed)(const bt_bd_addr_t *address);
uint16_t RHOS_MULTIPOINT(bt_role_handover_get_user_length)(const bt_bd_addr_t *address, uint8_t *length_array);
uint16_t RHOS_MULTIPOINT(bt_role_handover_get_user_data)(const bt_bd_addr_t *address, uint8_t *data, uint8_t *length_array);
void RHOS_MULTIPOINT(bt_role_handover_update_user)(bt_role_handover_module_type_t type, bt_role_handover_update_info_t *update_info);
void RHOS_MULTIPOINT(bt_role_handover_update_agent_user)(const bt_bd_addr_t *address);
void RHOS_MULTIPOINT(bt_role_handover_update_partner_user)(const bt_bd_addr_t *address, uint8_t *data, uint16_t length);

void bt_role_handover_notify_agent_end(bt_status_t status);
static uint32_t bt_role_handover_multipoint_get_connected_devices(bt_bd_addr_t *address_list);
static bt_status_t bt_role_handover_multipoint_allow_callback(const bt_bd_addr_t *address);
static uint8_t bt_role_handover_multipoint_get_data_length(const bt_bd_addr_t *address);
static bt_status_t bt_role_handover_multipoint_get_data(const bt_bd_addr_t *address, void *data);
static bt_status_t bt_role_handover_multipoint_update(bt_role_handover_update_info_t *info);
static void bt_role_handover_multipoint_status_callback(const bt_bd_addr_t *address, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status);

void bt_role_handover_init(void)
{
    RHOS_MULTIPOINT(bt_role_handover_init)();

    if (bt_rho_srv_callbacks[BT_ROLE_HANDOVER_MODULE_RHO_SRV].status_cb == NULL) {
        bt_role_handover_callbacks_t callbacks = {
            .allowed_cb     = bt_role_handover_multipoint_allow_callback,
            .get_len_cb     = bt_role_handover_multipoint_get_data_length,
            .get_data_cb    = bt_role_handover_multipoint_get_data,
            .update_cb      = bt_role_handover_multipoint_update,
            .status_cb      = bt_role_handover_multipoint_status_callback
        };

        memcpy(&bt_rho_srv_callbacks[BT_ROLE_HANDOVER_MODULE_RHO_SRV], &callbacks, sizeof(bt_role_handover_callbacks_t));
    }
}

bt_status_t bt_role_handover_start_internal(void)
{
    bt_status_t status = BT_STATUS_SUCCESS;

    bt_role_handover_status_callback_t cm_status_callback
        = bt_rho_srv_callbacks[BT_ROLE_HANDOVER_MODULE_SINK_CM].status_cb;
    bt_role_handover_allow_execution_callback_t cm_allow_callback
        = bt_rho_srv_callbacks[BT_ROLE_HANDOVER_MODULE_SINK_CM].allowed_cb;

    if ((cm_allow_callback != NULL) && (cm_status_callback != NULL)) {
        status = cm_allow_callback((const bt_bd_addr_t *)(&(bt_rho_srv_context.remote_addr)));

        if (status == BT_STATUS_PENDING) {
            bt_rho_srv_context.flag |= BT_ROLE_HANDOVER_FLAG_CM_PREPARE;
            cm_status_callback((const bt_bd_addr_t *)(&(bt_rho_srv_context.remote_addr)),
                BT_AWS_MCE_ROLE_AGENT, BT_ROLE_HANDOVER_PREPARE_REQ_IND, BT_STATUS_SUCCESS);
            return BT_STATUS_SUCCESS;
        } else {
            if (status != BT_STATUS_SUCCESS) {
                bt_role_handover_notify_agent_end(status);
                return status;
            }
        }
    }

    return RHOS_MULTIPOINT(bt_role_handover_start_internal)();
}

bt_status_t bt_role_handover_is_user_allowed(const bt_bd_addr_t *address)
{
    return RHOS_MULTIPOINT(bt_role_handover_is_user_allowed)(address);
}

uint16_t bt_role_handover_get_user_length(const bt_bd_addr_t *address, uint8_t *length_array)
{
    uint16_t total_length = 0;
    uint32_t get_length_number = 0;
    bt_bd_addr_t get_length_address[BT_ROLE_HANDOVER_MAX_LINK_NUM] = {{0}};

    get_length_number = bt_role_handover_multipoint_get_connected_devices(get_length_address);

    for (uint32_t i = 0; i < get_length_number; i++) {
        if (i == 0) {
            total_length += RHOS_MULTIPOINT(bt_role_handover_get_user_length)(NULL, length_array);
        } else {
            total_length += RHOS_MULTIPOINT(bt_role_handover_get_user_length)(
                (const bt_bd_addr_t *)&get_length_address[i],
                length_array + i * BT_ROLE_HANDOVER_MODULE_MAX);
        }
    }

    return total_length;
}

uint16_t bt_role_handover_get_user_data(const bt_bd_addr_t *address, uint8_t *data, uint8_t *length_array)
{
    uint16_t total_length = 0;
    uint32_t get_data_number = 0;
    bt_bd_addr_t get_data_address[BT_ROLE_HANDOVER_MAX_LINK_NUM] = {{0}};

    get_data_number = bt_role_handover_multipoint_get_connected_devices(get_data_address);

    for (uint32_t i = 0; i < get_data_number; i++) {
        uint16_t packet_length = 0;
        uint8_t *packet_data = data + total_length;

        if (i == 0) {
            packet_length = RHOS_MULTIPOINT(bt_role_handover_get_user_data)(NULL, data, length_array);
        } else {
            packet_length = RHOS_MULTIPOINT(bt_role_handover_get_user_data)(
                (const bt_bd_addr_t *)&get_data_address[i],
                packet_data,
                length_array + i * BT_ROLE_HANDOVER_MODULE_MAX);
        }

        /* Fill packet header. */
        if ((packet_data[0] != BT_ROLE_HANDOVER_MODULE_RHO_SRV) ||
            (packet_data[1] != sizeof(bt_role_handover_rho_data_t))) {
            configASSERT(0 && "Packet header error");
        } else {
            bt_role_handover_rho_data_t *rho_data = (bt_role_handover_rho_data_t *)&packet_data[2];
            rho_data->packet_length = packet_length;
            LOG_MSGID_I(BT_RHO, "get user data, packet_length:%d", 1, packet_length);
        }

        /* Move data pointer. */
        total_length += packet_length;
    }

    return total_length;
}

void bt_role_handover_update_user(bt_role_handover_module_type_t type, bt_role_handover_update_info_t *update_info)
{
    if ((update_info->role == BT_AWS_MCE_ROLE_AGENT) ||
        ((update_info->data != NULL) && (update_info->length != 0))) {
        RHOS_MULTIPOINT(bt_role_handover_update_user)(type, update_info);
    }
}

void bt_role_handover_update_agent_user(const bt_bd_addr_t *address)
{
    RHOS_MULTIPOINT(bt_role_handover_update_agent_user)(address);
}

void bt_role_handover_update_partner_user(const bt_bd_addr_t *address, uint8_t *data, uint16_t length)
{
    uint16_t parse_length = 0;
    uint8_t *parse_data = data;
    const bt_bd_addr_t null_address = {0};

    while (parse_length < length) {
        bt_role_handover_rho_data_t *rho_data = (bt_role_handover_rho_data_t *)&parse_data[2];

        if ((parse_data[0] != BT_ROLE_HANDOVER_MODULE_RHO_SRV) ||
            (parse_data[1] != sizeof(bt_role_handover_rho_data_t))) {
            configASSERT(0 && "Packet header error");
        }

        /* Get sub-packet header. */
        uint16_t packet_length = rho_data->packet_length;
        const bt_bd_addr_t *packet_address = (const bt_bd_addr_t *)(&(rho_data->packet_address));
        LOG_MSGID_I(BT_RHO, "update partner user, packet_length:%d", 1, packet_length);

        if (memcmp(packet_address, null_address, sizeof(bt_bd_addr_t)) == 0) {
            packet_address = NULL;
        } else {
            bt_aws_mce_role_handover_profile_t profile_info = {0};
            
            profile_info.gap_handle = bt_aws_mce_query_handle_by_address(BT_MODULE_GAP, packet_address);
            profile_info.hfp_handle = bt_aws_mce_query_handle_by_address(BT_MODULE_HFP, packet_address);
            profile_info.a2dp_handle = bt_aws_mce_query_handle_by_address(BT_MODULE_A2DP, packet_address);
            profile_info.avrcp_handle = bt_aws_mce_query_handle_by_address(BT_MODULE_AVRCP, packet_address);
            profile_info.hsp_handle = bt_aws_mce_query_handle_by_address(BT_MODULE_HSP, packet_address);
            profile_info.spp_handle = bt_aws_mce_query_handle_by_address(BT_MODULE_SPP, packet_address);
            profile_info.gatt_handle = bt_aws_mce_query_handle_by_address(BT_MODULE_GATT, packet_address);
            
            bt_role_handover_store_profile_info(&profile_info);

        }

        RHOS_MULTIPOINT(bt_role_handover_update_partner_user)(packet_address, parse_data, packet_length);

        /* Move data pointer. */
        parse_length += packet_length;
        parse_data += packet_length;
    }
}

static uint32_t bt_role_handover_multipoint_get_connected_devices(bt_bd_addr_t *address_list)
{
    uint32_t list_number = 2;
    uint32_t connected_number = 0;
    bt_bd_addr_t connected_address[BT_ROLE_HANDOVER_MAX_LINK_NUM] = {{0}};
    bt_bd_addr_t *local_address = bt_device_manager_aws_local_info_get_local_address();

    connected_number = bt_cm_get_connected_devices(
        BT_CM_PROFILE_SERVICE_MASK_NONE, connected_address, BT_ROLE_HANDOVER_MAX_LINK_NUM);

    for (uint32_t i = 0; i < connected_number; i++) {
        if (memcmp(&connected_address[i], local_address, sizeof(bt_bd_addr_t)) == 0) {
            // address_list[0] is used for special link/common data
            memcpy(&address_list[0], &connected_address[i], sizeof(bt_bd_addr_t));
        } else if (memcmp(&connected_address[i], &bt_rho_srv_context.remote_addr, sizeof(bt_bd_addr_t)) == 0) {
            // address_list[1] is used for active normal link
            memcpy(&address_list[1], &connected_address[i], sizeof(bt_bd_addr_t));
        } else {
            // address_list[2...MAX_LINK_NUM] are used for inactive normal link
            memcpy(&address_list[list_number], &connected_address[i], sizeof(bt_bd_addr_t));
            list_number++;
        }
    }

    return connected_number;
}

static bt_status_t bt_role_handover_multipoint_allow_callback(const bt_bd_addr_t *address)
{
    bt_status_t allow = BT_STATUS_SUCCESS;
    uint32_t connected_number = bt_cm_get_connected_devices(
        ~BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS), NULL, 0);

    if (connected_number == 0) {
        allow = BT_STATUS_FAIL;
    }

    LOG_MSGID_I(BT_RHO, "allow callback, allow:0x%x", 1, allow);
    return allow;
}

static uint8_t bt_role_handover_multipoint_get_data_length(const bt_bd_addr_t *address)
{
    return sizeof(bt_role_handover_rho_data_t);
}

static bt_status_t bt_role_handover_multipoint_get_data(const bt_bd_addr_t *address, void *data)
{
    bt_role_handover_rho_data_t *rho_data = (bt_role_handover_rho_data_t *)data;

    rho_data->packet_length = 0;

    if (address == NULL) {
        memset(&(rho_data->packet_address), 0, sizeof(bt_bd_addr_t));
    } else {
        memcpy(&(rho_data->packet_address), address, sizeof(bt_bd_addr_t));
    }
    
    return BT_STATUS_SUCCESS;
}

static bt_status_t bt_role_handover_multipoint_update(bt_role_handover_update_info_t *info)
{
    return BT_STATUS_SUCCESS;
}

static void bt_role_handover_multipoint_status_callback(const bt_bd_addr_t *address, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    return;
}

