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

#ifdef MTK_IAP2_VIA_MUX_ENABLE
#include "iAP2.h"
#include "iAP2_spp.h"
#include "syslog.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "bt_callback_manager.h"
#include "bt_connection_manager.h"
#include "bt_device_manager_internal.h"
#include "mux.h"
#include "mux_iap2.h"
#include "mux_port_device.h"

#ifdef MTK_AWS_MCE_ENABLE
#include "bt_role_handover.h"
#endif

#ifndef MTK_BT_CM_SUPPORT
#include "bt_sink_srv.h"
#endif

#define mux_iap2_assert(x) configASSERT(x)

#define MUX_IAP2_MUTEX_ENABLE   1

#define MUX_IAP2_FLAG_INIT                 0x01U
#define MUX_IAP2_FLAG_REPLY_RHO            0x02U
#define MUX_IAP2_FLAG_LINK_DISCONNECTING   0x04U
typedef uint32_t mux_iap2_flag_t;

typedef struct mux_iap2_packet {
    uint8_t *data;
    uint16_t data_length;
    struct mux_iap2_packet *next;
} mux_iap2_packet_t;

typedef struct {
    uint32_t handle;
    mux_iap2_flag_t flag;
    uint16_t max_packet_length;
    bt_bd_addr_t remote_address;
} mux_iap2_handle_t;

typedef struct {
    uint8_t is_rx_need_session_id: 1;
    uint8_t reserved: 7;
} mux_iap2_protocol_config_t;

typedef struct {
    mux_irq_handler_t handler;
    bool is_port_notifyed_con_evt;
    mux_port_config_t *config;
    virtual_read_write_point_t read_write_point;
    mux_iap2_protocol_config_t protocol_config;

    uint16_t protocol_id;
    uint16_t session_id[MAX_MUX_IAP2_SESSION_NUMBER];
    uint8_t current_session_num;

    mux_iap2_packet_t *tx_head_packet;
    mux_iap2_packet_t *tx_tail_packet;
} mux_iap2_context_t;

BT_PACKED(
typedef struct {
    uint8_t context_num;
}) mux_iap2_rho_header_t;

BT_PACKED(
typedef struct {
    uint16_t protocol_id;
    uint16_t session_id[MAX_MUX_IAP2_SESSION_NUMBER];
    uint8_t current_session_num;
}) mux_iap2_rho_context_t;

#ifdef MTK_CPU_NUMBER_0
mux_status_t mux_iap2_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler);
mux_status_t mux_iap2_deinit(uint8_t port_index);
void mux_iap2_exception_init(uint8_t port_index);
void mux_iap2_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size);
bool mux_iap2_get_buf_is_full(uint8_t port_index, bool is_rx);
#endif
uint32_t mux_iap2_get_hw_rptr(uint8_t port_index, bool is_rx);
void mux_iap2_set_hw_rptr(uint8_t port_index, uint32_t move_bytes);
uint32_t mux_iap2_get_hw_wptr(uint8_t port_index, bool is_rx);
void mux_iap2_set_hw_wptr(uint8_t port_index, uint32_t move_bytes);
void mux_iap2_phase1_send(uint8_t port_index);
void mux_iap2_phase2_send(uint8_t port_index);
mux_status_t mux_iap2_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para);
void mux_iap2_set_tx_src_address(uint8_t port_index, uint32_t  src_address, uint32_t  size);
void mux_iap2_get_rx_src_address(uint8_t port_index, uint32_t *src_address, uint32_t *size);
static void mux_iap2_reset_handle(void);
static void mux_iap2_reset_context(mux_iap2_context_t *context);
static void mux_iap2_reset_all_context(void);
static mux_iap2_packet_t *mux_iap2_alloc_packet(void);
static void mux_iap2_free_packet(mux_iap2_packet_t *packet);
static void mux_iap2_send_context_data(mux_iap2_context_t *context);
static void mux_iap2_context_callback(mux_iap2_context_t *context, mux_event_t event, void *parameter);
static mux_port_t mux_iap2_get_port_by_context(mux_iap2_context_t *context);
static mux_iap2_context_t *mux_iap2_get_context_by_port_index(uint8_t port_index);
static mux_iap2_context_t *mux_iap2_get_context_by_protocol_id(uint16_t protocol_id);
static mux_iap2_context_t *mux_iap2_get_context_by_session_id(uint16_t session_id);
static void mux_iap2_callback(iap2_event_t event, void *parameter);
static bt_status_t mux_iap2_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *parameter);
#ifdef MTK_BT_CM_SUPPORT
bt_status_t mux_iap2_cm_callback(bt_cm_profile_service_handle_t type, void *data);
#else
bt_status_t mux_iap2_action_hanler(bt_sink_srv_action_t action, void *parameter);
#endif
#if defined(MTK_AWS_MCE_ENABLE) && !defined(BT_ROLE_HANDOVER_WITH_SPP_BLE)
bt_status_t mux_iap2_role_handover_allow_execution(const bt_bd_addr_t *address);
bt_status_t mux_iap2_role_handover_update(bt_role_handover_update_info_t *info);
void mux_iap2_role_handover_status_callback(const bt_bd_addr_t *address, bt_aws_mce_role_t role, bt_role_handover_event_t event,
        bt_status_t status);
#endif

extern iap2_status_t iap2_send_app_launch_request(uint32_t handle, uint8_t *app_id, bool is_alert);
extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);

port_mux_device_ops_t g_mux_iap2_ops = {
#ifdef MTK_CPU_NUMBER_0
    .normal_init = mux_iap2_normal_init,
    .deinit = mux_iap2_deinit,
    .exception_init = mux_iap2_exception_init,
    .exception_send = mux_iap2_exception_send,
    .get_buf_is_full = mux_iap2_get_buf_is_full,
#endif
    .get_hw_rptr = mux_iap2_get_hw_rptr,
    .set_hw_rptr = mux_iap2_set_hw_rptr,
    .get_hw_wptr = mux_iap2_get_hw_wptr,
    .set_hw_wptr = mux_iap2_set_hw_wptr,
    .phase1_send = mux_iap2_phase1_send,
    .phase2_send = mux_iap2_phase2_send,
    .control = mux_iap2_control,
    .set_tx_src_address = mux_iap2_set_tx_src_address,
    .get_rx_src_address = mux_iap2_get_rx_src_address,
};

static mux_iap2_handle_t g_mux_iap2_handle;
static mux_iap2_packet_t g_mux_iap2_packet[MAX_MUX_IAP2_PACKET_NUM];
#if MUX_IAP2_MUTEX_ENABLE
static uint32_t g_mux_iap2_mutex = 0;
#endif
log_create_module(MUX_IAP2, PRINT_LEVEL_INFO);

static mux_iap2_context_t g_mux_iap2_context[MAX_MUX_IAP2_NUM] = {
    {NULL, false, NULL, {0}, {0}, MUX_IAP2_SESSION1_PROTOCOL_ID, {0}, 0, NULL, NULL},
    {NULL, false, NULL, {0}, {0}, MUX_IAP2_SESSION2_PROTOCOL_ID, {0}, 0, NULL, NULL},
    {NULL, false, NULL, {0}, {0}, MUX_IAP2_SESSION3_PROTOCOL_ID, {0}, 0, NULL, NULL},
    {NULL, false, NULL, {0}, {0}, MUX_IAP2_SESSION4_PROTOCOL_ID, {0}, 0, NULL, NULL},
};

#define MUX_IAP2_IS_PACKET_FREE(packet) \
    ((packet)->data == NULL)

#define MUX_IAP2_EXPAND_BD_ADDRESS(address) \
    ((address)[0], (address)[1], (address)[2], (address)[3], (address)[4], (address)[5])

#define MUX_IAP2_IS_SESSION_CONNECTED(context) \
    ((g_mux_iap2_handle.handle != 0) && ((context) != NULL) && ((context)->handler != NULL) && ((context)->current_session_num != 0))

#define MUX_IAP2_IS_SESSION_VALID(context, target_session_id) \
    ((((context)->session_id[0] == (target_session_id)) || ((context)->session_id[1] == (target_session_id)) || ((context)->session_id[2] == (target_session_id))) && ((target_session_id) != MUX_IAP2_INVALID_SESSION_ID))

static void mux_iap2_mutex_create()
{
#if MUX_IAP2_MUTEX_ENABLE
    if (0 == g_mux_iap2_mutex) {
        g_mux_iap2_mutex = xSemaphoreCreateMutex();
        mux_iap2_assert(g_mux_iap2_mutex);
    }
#endif
}

static void mux_iap2_mutex_lock()
{
#if MUX_IAP2_MUTEX_ENABLE
    //xSemaphoreTake(g_mux_iap2_mutex, portMAX_DELAY);
    bt_os_take_stack_mutex();
#endif
}

static void mux_iap2_mutex_unlock()
{
#if MUX_IAP2_MUTEX_ENABLE
    //xSemaphoreGive(g_mux_iap2_mutex);
    bt_os_give_stack_mutex();
#endif
}

void mux_iap2_register_callbacks(void)
{
#if defined(SUPPORT_ROLE_HANDOVER_SERVICE) && !defined(BT_ROLE_HANDOVER_WITH_SPP_BLE)
    bt_role_handover_callbacks_t callbacks = {
        .allowed_cb = mux_iap2_role_handover_allow_execution,
        .update_cb = mux_iap2_role_handover_update,
        .status_cb = mux_iap2_role_handover_status_callback
    };

    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_IAP2, &callbacks);
#endif

#ifdef MTK_BT_CM_SUPPORT
    bt_cm_profile_service_register(BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2, mux_iap2_cm_callback);
#endif
    iap2_register_callback(mux_iap2_callback);
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SYSTEM, mux_iap2_app_event_callback);
}

#ifdef MTK_CPU_NUMBER_0
mux_status_t mux_iap2_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    mux_iap2_context_t *context = NULL;
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]normal_init, port_index:0x%x p_setting:0x%x irq_handler:0x%x", 3, port_index, p_setting, irq_handler);
    mux_iap2_mutex_create();

    if ((context = mux_iap2_get_context_by_port_index(port_index)) == NULL) {
        return MUX_STATUS_ERROR_PARAMETER;
    }

    if (context->handler == NULL) {
        context->handler = irq_handler;
        context->config = p_setting;
        while (context->tx_head_packet != NULL) {
            mux_iap2_packet_t *delete_packet = context->tx_head_packet;
            context->tx_head_packet = delete_packet->next;
            mux_common_device_set_tx_hw_rptr_internal_use(&context->read_write_point, delete_packet->data_length);
            mux_iap2_free_packet(delete_packet);
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]normal_init, delete packet:0x%x, current head:0x%x tail:0x%x", 3,
                        delete_packet, context->tx_head_packet, context->tx_tail_packet);
        }
        context->tx_tail_packet = NULL;
        mux_common_device_r_w_point_init(&context->read_write_point, p_setting);
    } else {
        return MUX_STATUS_ERROR_INITIATED;
    }

    if (MUX_IAP2_IS_SESSION_CONNECTED(context)) {
        /*mux_iap2_connection_t connection = {
            .session_id = context->session_id,
            .max_packet_size = g_mux_iap2_handle.max_packet_length
        };

        memcpy(connection.remote_address, g_mux_iap2_handle.remote_address, sizeof(bt_bd_addr_t));*/
        mux_iap2_context_callback(context, MUX_EVENT_CONNECTION, NULL);
    }

    return MUX_STATUS_OK;
}

mux_status_t mux_iap2_deinit(uint8_t port_index)
{
    mux_iap2_context_t *context = NULL;
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]deinit, port_index:0x%x", 1, port_index);

    if ((context = mux_iap2_get_context_by_port_index(port_index)) != NULL) {
        mux_iap2_context_callback(context, MUX_EVENT_DISCONNECTION, NULL);
        context->handler = NULL;
        context->is_port_notifyed_con_evt = false;
    } else {
        return MUX_STATUS_ERROR_PARAMETER;
    }

    return MUX_STATUS_OK;
}

void mux_iap2_exception_init(uint8_t port_index)
{
    return;
}

void mux_iap2_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    return;
}

bool mux_iap2_get_buf_is_full(uint8_t port_index, bool is_rx)
{
    bool is_full = false;
    mux_iap2_context_t *context = mux_iap2_get_context_by_port_index(port_index);

    if (context != NULL) {
        is_full = mux_common_device_buf_is_full(&context->read_write_point, is_rx);
    }

    return is_full;
}
#endif

ATTR_TEXT_IN_TCM uint32_t mux_iap2_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    uint32_t hw_rptr = 0;
    mux_iap2_context_t *context = mux_iap2_get_context_by_port_index(port_index);

    if (context != NULL) {
        hw_rptr = mux_common_device_get_hw_rptr(&context->read_write_point, is_rx);
    }

    return hw_rptr;
}

void mux_iap2_set_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    mux_iap2_context_t *context = mux_iap2_get_context_by_port_index(port_index);

    if (context != NULL) {
        mux_common_device_set_rx_hw_rptr(&context->read_write_point, move_bytes);
    }
}

ATTR_TEXT_IN_TCM uint32_t mux_iap2_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    uint32_t hw_wptr = 0;
    mux_iap2_context_t *context = mux_iap2_get_context_by_port_index(port_index);

    if (context != NULL) {
        hw_wptr = mux_common_device_get_hw_wptr(&context->read_write_point, is_rx);
    }

    return hw_wptr;
}

ATTR_TEXT_IN_TCM void mux_iap2_set_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    mux_iap2_packet_t *packet = NULL;
    mux_iap2_context_t *context = NULL;
    //LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]set_hw_wptr, port_index:0x%x move_bytes:0x%x", 2, port_index, move_bytes);

    if ((context = mux_iap2_get_context_by_port_index(port_index)) == NULL) {
        return;
    }

    // enter critical, start process packet
    mux_common_device_set_tx_hw_wptr(&context->read_write_point, move_bytes);

    if (!MUX_IAP2_IS_SESSION_CONNECTED(context)) {
        return;
    }

    packet = mux_iap2_alloc_packet();
    if (packet == NULL) {
        //LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]set_hw_wptr, alloc packet FAILED!", 0);
        return;
    }

    packet->data = (uint8_t *)(context->read_write_point.tx_buff_read_point);
    packet->data_length = move_bytes;
    packet->next = NULL;

    if (context->tx_head_packet == NULL) {
        context->tx_head_packet = packet;
        context->tx_tail_packet = packet;
    } else {
        context->tx_tail_packet->next = packet;
        context->tx_tail_packet = packet;
    }
    // exit critial, start send packet
}

void mux_iap2_set_tx_src_address(uint8_t port_index, uint32_t src_address, uint32_t size)
{
    mux_iap2_packet_t *packet = NULL;
    mux_iap2_context_t *context = NULL;

    if ((context = mux_iap2_get_context_by_port_index(port_index)) == NULL) {
        return;
    }

    if (!MUX_IAP2_IS_SESSION_CONNECTED(context)) {
        return;
    }

    packet = mux_iap2_alloc_packet();
    if (packet == NULL) {
        return;
    }

    packet->data = src_address;
    packet->data_length = size;
    packet->next = NULL;

    if (context->tx_head_packet == NULL) {
        context->tx_head_packet = packet;
        context->tx_tail_packet = packet;
    } else {
        context->tx_tail_packet->next = packet;
        context->tx_tail_packet = packet;
    }

    context->read_write_point.tx_send_src_address = src_address;
    context->read_write_point.tx_send_src_length  = size;

    /*debug log, if use mux_tx can not mask irq*/
    //LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]mux_iap2_set_tx_src_address0x%x,0x%x", 2, context->read_write_point.tx_send_src_address,context->read_write_point.tx_send_src_length);

}

void mux_iap2_get_rx_src_address(uint8_t port_index, uint32_t *src_address, uint32_t *size)
{
    mux_iap2_packet_t *packet = NULL;
    mux_iap2_context_t *context = NULL;

    if ((context = mux_iap2_get_context_by_port_index(port_index)) == NULL) {
        return;
    }

    if (!MUX_IAP2_IS_SESSION_CONNECTED(context)) {
        return;
    }

    *src_address = context->read_write_point.rx_receive_src_address;
    *size        = context->read_write_point.rx_receive_src_length ;
}

void mux_iap2_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void mux_iap2_phase2_send(uint8_t port_index)
{
    mux_iap2_context_t *context = NULL;
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]phase2_send, port_index:0x%x", 1, port_index);
    mux_iap2_mutex_lock();
    if ((context = mux_iap2_get_context_by_port_index(port_index)) != NULL) {
        if (MUX_IAP2_IS_SESSION_CONNECTED(context)) {
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]phase2_send, current head:0x%x tail:0x%x", 2, context->tx_head_packet, context->tx_tail_packet);
            mux_iap2_send_context_data(context);
        } else {
            mux_common_device_set_tx_hw_rptr_internal_use(&context->read_write_point, context->read_write_point.tx_buff_available_len);
        }
    }
    mux_iap2_mutex_unlock();
}

mux_status_t mux_iap2_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    mux_iap2_context_t *context = NULL;

    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]control, port_index:0x%x command:0x%x para:0x%x", 3, port_index, command, para);

    if ((para == NULL) || ((context = mux_iap2_get_context_by_port_index(port_index)) == NULL)) {
        return MUX_STATUS_ERROR;
    }

    if (command == MUX_CMD_GET_CONNECTION_PARAM) {
        if (MUX_IAP2_FLAG_LINK_DISCONNECTING & g_mux_iap2_handle.flag) {
            return MUX_STATUS_ERROR;
        }
        mux_get_connection_param_t *connection_param = (mux_get_connection_param_t *)para;
        memcpy(connection_param->iap2_session_id, context->session_id, MAX_MUX_IAP2_SESSION_NUMBER * sizeof(uint16_t));
        connection_param->iap2_session_num = context->current_session_num;
        connection_param->max_packet_size = g_mux_iap2_handle.max_packet_length;
        memcpy(connection_param->remote_address, g_mux_iap2_handle.remote_address, sizeof(bt_bd_addr_t));
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]get connection param, current session number:%d, session id list:0x%x, 0x%x, 0x%x,", 4, context->current_session_num,
                    context->session_id[0], context->session_id[1], context->session_id[2]);
    } else if (command == MUX_CMD_CONNECT) {
#if 0
        if (context->current_session_num == 0 && g_mux_iap2_handle.handle == 0) {
            bt_bd_addr_t *address = (bt_bd_addr_t *)para;
            status = iap2_connect(&g_mux_iap2_handle.handle, address);
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]connect status:0x%x", 1, status);
        } else {
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]connect iap2 fail, curr_session_num:%d, g_mux_iap2_handle:0x%x", 2, context->current_session_num,
                        g_mux_iap2_handle.handle);
            return MUX_STATUS_ERROR;
        }
#endif
    } else if (command == MUX_CMD_DISCONNECT) {
#if 0
        uint16_t session_id = *(uint16_t *)para;
        if (context->current_session_num == 1 && MUX_IAP2_IS_SESSION_VALID(context, session_id) && g_mux_iap2_handle.handle != 0) {
            status = iap2_disconnect(g_mux_iap2_handle.handle);
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]disconnect status:0x%x", 1, status);
        } else {
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]disconnect iap2 fail, session id:%d, curr_session_num:%d, g_mux_iap2_handle:0x%x", 3, session_id, context->current_session_num,
                        g_mux_iap2_handle.handle);
            return MUX_STATUS_ERROR;
        }
#endif
    } else if (command == MUX_CMD_SET_RX_PARAM) {
        mux_set_config_param_t *config_param = (mux_set_config_param_t *)para;
        context->protocol_config.is_rx_need_session_id = config_param->is_rx_need_session_id;
    }

    return MUX_STATUS_OK;
}

static void mux_iap2_reset_handle(void)
{
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]reset_handle", 0);
    mux_iap2_flag_t temp_flag = g_mux_iap2_handle.flag;
    memset(&g_mux_iap2_handle, 0, sizeof(mux_iap2_handle_t));
    if (temp_flag & MUX_IAP2_FLAG_INIT) {
        g_mux_iap2_handle.flag |= MUX_IAP2_FLAG_INIT;
    }
}

static void mux_iap2_reset_context(mux_iap2_context_t *context)
{
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]reset_context, context:0x%x", 1, context);

    while (context->tx_head_packet != NULL) {
        mux_iap2_packet_t *delete_packet = context->tx_head_packet;
        context->tx_head_packet = delete_packet->next;

        mux_common_device_set_tx_hw_rptr_internal_use(&context->read_write_point, delete_packet->data_length);
        //vPortFree(delete_packet);
        mux_iap2_free_packet(delete_packet);

        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]reset_context, delete packet:0x%x, current head:0x%x tail:0x%x", 3,
                    delete_packet, context->tx_head_packet, context->tx_tail_packet);
    }

    memset(context->session_id, 0, MAX_MUX_IAP2_SESSION_NUMBER * sizeof(uint16_t));
    context->current_session_num = 0;
    context->tx_head_packet = NULL;
    context->tx_tail_packet = NULL;

    if (context->config != NULL) {
        mux_common_device_r_w_point_init(&context->read_write_point, context->config);
        mux_restore_callback(mux_iap2_get_port_by_context(context));
    } else {
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]reset_context, config is NULL!", 0);
    }
}

static void mux_iap2_reset_packet_by_session_id(mux_iap2_context_t *context, uint16_t session_id)
{
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]reset_context_by_session_id, session_id:0x%x", 1, session_id);
    //mux_iap2_context_t *context = mux_iap2_get_context_by_session_id(session_id);
    if (context != NULL) {
        mux_iap2_packet_t *head = context->tx_head_packet;  /*first node of the packet list*/
        mux_iap2_packet_t *packet = context->tx_head_packet;/*to reverse whole list*/
        while (packet != NULL) {
            if (((mux_iap2_header_t *)(packet->data))->session_id == session_id) {
                mux_iap2_packet_t *pkt_to_delete = packet;
                if (pkt_to_delete == head) {
                    head = pkt_to_delete->next;
                }
                packet = pkt_to_delete->next;
                mux_common_device_set_tx_hw_rptr_internal_use(&context->read_write_point, pkt_to_delete->data_length);
                mux_iap2_free_packet(pkt_to_delete);
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]reset_context_by_session_id, delete packet:0x%x, current head:0x%x tail:0x%x", 3,
                            pkt_to_delete, context->tx_head_packet, context->tx_tail_packet);
            } else {
                packet = packet->next;
            }
        }
        if (head != NULL) {
            context->tx_head_packet = head;
            packet = head;
            while (packet->next != NULL) {
                packet = packet->next;
            }
            context->tx_tail_packet = packet;
        } else {
            context->tx_head_packet = NULL;
            context->tx_tail_packet = NULL;
        }
    }
}

static void mux_iap2_reset_all_context(void)
{
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]reset_all_context", 0);

    for (uint8_t i = 0; i < MAX_MUX_IAP2_NUM; i++) {
        mux_iap2_reset_context(&g_mux_iap2_context[i]);
    }
}

static mux_iap2_packet_t *mux_iap2_alloc_packet(void)
{
    for (uint8_t i = 0; i < MAX_MUX_IAP2_PACKET_NUM; i++) {
        if (MUX_IAP2_IS_PACKET_FREE(&g_mux_iap2_packet[i])) {
            return &g_mux_iap2_packet[i];
        }
    }

    return NULL;
}

static void mux_iap2_free_packet(mux_iap2_packet_t *packet)
{
    //LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2] free packet:0x%x", 1, packet);
    memset(packet, 0, sizeof(mux_iap2_packet_t));
}

static void mux_iap2_send_context_data(mux_iap2_context_t *context)
{
    while ((context->tx_head_packet != NULL) &&
            (MUX_IAP2_IS_SESSION_CONNECTED(context)) &&
            (context->read_write_point.tx_send_is_running != MUX_DEVICE_HW_RUNNING)) {
        uint32_t next_read_point = 0;
        mux_iap2_packet_t *send_packet = context->tx_head_packet;
        context->read_write_point.tx_send_is_running = MUX_DEVICE_HW_RUNNING;
        mux_iap2_header_t mux_iap2_header;
        //mux_iap2_header.session_id = ((mux_iap2_header_t *)(send_packet->data))->session_id;
        //uint16_t session_id = ((mux_iap2_header_t *)(context->read_write_point.tx_buff_read_point))->session_id;//send_packet->data is not trustworthy;
        if (context->read_write_point.tx_buff_len != 0) {
            if ((context->read_write_point.tx_buff_end - context->read_write_point.tx_buff_read_point) < sizeof(mux_iap2_header_t)) {
                uint8_t low = *(uint8_t *)(context->read_write_point.tx_buff_read_point);
                uint8_t high = *(uint8_t *)(context->read_write_point.tx_buff_start);
                mux_iap2_header.session_id = (uint16_t)((high << 8) | low);
            } else {
                //send_packet->data is not trustworthy
                mux_iap2_header.session_id = ((mux_iap2_header_t *)(context->read_write_point.tx_buff_read_point))->session_id;
            }
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]mux_iap2_send_context_data, session_id:0x%x", 1, mux_iap2_header.session_id);

            if (!MUX_IAP2_IS_SESSION_CONNECTED(context)) {
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]session has closed, context:0x%x", 1, context);
                context->read_write_point.tx_send_is_running = MUX_DEVICE_HW_IDLE;
                break;
            }

            if (!MUX_IAP2_IS_SESSION_VALID(context, mux_iap2_header.session_id)) {
                /*specific ea session has closed, so abandon this packet, adjust rptr, free packet, show log*/
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]abandon invalid packet, tx_buf_rptr:0x%x, packet_session_id:%x, packet:0x%x, current head:0x%x tail:0x%x", 5,
                            context->read_write_point.tx_buff_read_point, mux_iap2_header.session_id, send_packet, context->tx_head_packet, context->tx_tail_packet);
                context->read_write_point.tx_send_is_running = MUX_DEVICE_HW_IDLE;
                mux_iap2_assert(0 && "invalid session id");
                context->tx_head_packet = send_packet->next;
                mux_common_device_set_tx_hw_rptr_internal_use(&context->read_write_point, send_packet->data_length);
                mux_iap2_free_packet(send_packet);
                continue;
            }

            if (context->read_write_point.tx_buff_read_point + send_packet->data_length <= context->read_write_point.tx_buff_end) {
                if (iap2_send_data_by_external_accessory_session(g_mux_iap2_handle.handle, mux_iap2_header.session_id,
                        (uint8_t *)(context->read_write_point.tx_buff_read_point + sizeof(mux_iap2_header_t)),
                        send_packet->data_length - sizeof(mux_iap2_header_t)) == IAP2_STATUS_SUCCESS) {
                    /*if send fail, next_read_point is 0, means there is no credit to send*/
                    next_read_point = context->read_write_point.tx_buff_read_point + send_packet->data_length;
                }
            } else {
                uint8_t *full_data = pvPortMalloc(send_packet->data_length);

                if (full_data == NULL) {
                    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]send_context_data, alloc full data FAILED!", 0);
                    break;
                }

                uint8_t *first_data = (uint8_t *)(context->read_write_point.tx_buff_read_point);
                uint16_t first_data_length = context->read_write_point.tx_buff_end - (uint32_t)context->read_write_point.tx_buff_read_point;
                memcpy(full_data, first_data, first_data_length);

                uint8_t *continue_data = (uint8_t *)(context->read_write_point.tx_buff_start);
                uint16_t continue_data_length = send_packet->data_length - first_data_length;
                memcpy(full_data + first_data_length, continue_data, continue_data_length);

                if (iap2_send_data_by_external_accessory_session(g_mux_iap2_handle.handle, mux_iap2_header.session_id,
                        full_data + sizeof(mux_iap2_header_t),
                        send_packet->data_length - sizeof(mux_iap2_header_t)) == IAP2_STATUS_SUCCESS) {
                    next_read_point = context->read_write_point.tx_buff_start + continue_data_length;
                }

                vPortFree(full_data);
            }

            if (next_read_point != 0) {
                context->tx_head_packet = send_packet->next;
                if (context->tx_head_packet == NULL) {
                    context->tx_tail_packet = NULL;
                }

                mux_common_device_set_tx_hw_rptr_internal_use(&context->read_write_point, send_packet->data_length);
                mux_iap2_free_packet(send_packet);
                //vPortFree(send_packet);
                //context->read_write_point.tx_buff_read_point = next_read_point;

                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]send_context_data, send packet:0x%x, current head:0x%x tail:0x%x", 3,
                            send_packet, context->tx_head_packet, context->tx_tail_packet);
            } else {
                context->read_write_point.tx_send_is_running = MUX_DEVICE_HW_IDLE;
                break;
            }

            context->read_write_point.tx_send_is_running = MUX_DEVICE_HW_IDLE;
        } else {
            mux_iap2_header.session_id = ((mux_iap2_header_t *)(context->read_write_point.tx_send_src_address))->session_id;
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]mux_iap2_send_context_data transparent, session_id:0x%x", 1, mux_iap2_header.session_id);
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]abandon invalid packet transparent debug, tx_send_src_address:0x%x, packet_session_id:%x, current head:0x%x tail:0x%x", 4,
                        context->read_write_point.tx_send_src_address, mux_iap2_header.session_id, context->tx_head_packet, context->tx_tail_packet);

            if (!MUX_IAP2_IS_SESSION_CONNECTED(context)) {
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]session has closed transparent, context:0x%x", 1, context);
                context->read_write_point.tx_send_is_running = MUX_DEVICE_HW_IDLE;
                break;
            }

            if (!MUX_IAP2_IS_SESSION_VALID(context, mux_iap2_header.session_id)) {
                /*specific ea session has closed, so abandon this packet, adjust rptr, free packet, show log*/
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]abandon invalid packet transparent, tx_send_src_address:0x%x, packet_session_id:%x, current head:0x%x tail:0x%x", 4,
                            context->read_write_point.tx_send_src_address, mux_iap2_header.session_id, context->tx_head_packet, context->tx_tail_packet);
                context->read_write_point.tx_send_is_running = MUX_DEVICE_HW_IDLE;
                mux_iap2_assert(0 && "invalid session id");
                context->tx_head_packet = send_packet->next;
                mux_iap2_free_packet(send_packet);
                continue;
            }

            if (iap2_send_data_by_external_accessory_session(g_mux_iap2_handle.handle, mux_iap2_header.session_id,
                    (uint8_t *)(context->read_write_point.tx_send_src_address + sizeof(mux_iap2_header_t)),
                    send_packet->data_length - sizeof(mux_iap2_header_t)) != IAP2_STATUS_SUCCESS ) {
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]iap2_send_data_by_external_accessory_session", 0);
            }

            context->tx_head_packet = send_packet->next;
            if (context->tx_head_packet == NULL) {
                context->tx_tail_packet = NULL;
            }
            mux_iap2_free_packet(send_packet);
            context->read_write_point.tx_send_is_running = MUX_DEVICE_HW_IDLE;

        }

    }
}

void mux_iap2_send_app_launch_request(uint8_t *app_id, bool is_alert)
{
    iap2_send_app_launch_request(g_mux_iap2_handle.handle, app_id, is_alert);
}

static void mux_iap2_context_callback(mux_iap2_context_t *context, mux_event_t event, void *parameter)
{
    mux_port_t port = mux_iap2_get_port_by_context(context);
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]context_callback, context:0x%x event:0x%x parameter:0x%x", 3, context, event, parameter);

    if ((port >= MUX_IAP2_BEGIN) && (port <= MUX_IAP2_END) && (context->handler != NULL)) {
        // TODO: Extra parameter for event
        context->handler(port, event, parameter);
    } else {
        mux_iap2_assert(0);
    }
}

static mux_port_t mux_iap2_get_port_by_context(mux_iap2_context_t *context)
{
    mux_port_t port = MUX_IAP2_BEGIN + (context - &g_mux_iap2_context[0]);
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]get_port_by_context, context:0x%x, port:0x%x", 2, context, port);

    return port;
}

static ATTR_TEXT_IN_TCM mux_iap2_context_t *mux_iap2_get_context_by_port_index(uint8_t port_index)
{
    mux_iap2_context_t *context = NULL;

    if (port_index < MAX_MUX_IAP2_NUM) {
        context = &g_mux_iap2_context[port_index];
    }

    //LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]get_context_by_port_index, index:0x%x, context:0x%x", 2, port_index, context);
    return context;
}

static mux_iap2_context_t *mux_iap2_get_context_by_protocol_id(uint16_t protocol_id)
{
    mux_iap2_context_t *context = NULL;

    for (uint8_t i = 0; i < MAX_MUX_IAP2_NUM; i++) {
        if (g_mux_iap2_context[i].protocol_id == protocol_id) {
            context = &g_mux_iap2_context[i];
            break;
        }
    }

    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]get_context_by_protocol_id, protocol_id:0x%x, context:0x%x", 2, protocol_id, context);
    return context;
}

static mux_iap2_context_t *mux_iap2_get_context_by_session_id(uint16_t session_id)
{
    mux_iap2_context_t *context = NULL;

    for (uint8_t i = 0; i < MAX_MUX_IAP2_NUM; i++) {
        if (MUX_IAP2_IS_SESSION_VALID(&g_mux_iap2_context[i], session_id)) {
            context = &g_mux_iap2_context[i];
            break;
        }
    }

    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]get_context_by_session_id, session_id:0x%x, context:0x%x", 2, session_id, context);
    return context;
}

static void mux_iap2_callback(iap2_event_t event, void *parameter)
{
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]callback, event:0x%x parameter:0x%x", 2, event, parameter);

    switch (event) {
    case IAP2_CONNECT_IND: {
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
        if (bt_role_handover_get_state() == BT_ROLE_HANDOVER_STATE_ONGOING) {
            bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_IAP2);
        }
#endif
        iap2_connect_ind_t *connect_ind = (iap2_connect_ind_t *)parameter;
        if (connect_ind->status == IAP2_STATUS_SUCCESS) {
            g_mux_iap2_handle.handle = connect_ind->handle;
            g_mux_iap2_handle.max_packet_length = connect_ind->max_packet_length;
            memcpy(&g_mux_iap2_handle.remote_address, connect_ind->address, sizeof(bt_bd_addr_t));

#ifdef MTK_BT_CM_SUPPORT
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2, g_mux_iap2_handle.remote_address,
                                                BT_CM_PROFILE_SERVICE_STATE_CONNECTED, BT_STATUS_SUCCESS);
#else
            bt_sink_srv_cm_profile_status_notify(&g_mux_iap2_handle.remote_address, BT_SINK_SRV_PROFILE_IAP2,
                                                 BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED, BT_STATUS_SUCCESS);
#endif

            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]callback, IAP2_CONNECT_IND max_paxket_length:0x%x remote_address:0x%x:%x:%x:%x:%x:%x", 7,
                        g_mux_iap2_handle.max_packet_length, g_mux_iap2_handle.remote_address[0], g_mux_iap2_handle.remote_address[1],
                        g_mux_iap2_handle.remote_address[2], g_mux_iap2_handle.remote_address[3], g_mux_iap2_handle.remote_address[4],
                        g_mux_iap2_handle.remote_address[5]);
        } else {
            bt_bd_addr_t remote_addr = {0};
            memcpy(&remote_addr, connect_ind->address, sizeof(bt_bd_addr_t));
#ifdef MTK_BT_CM_SUPPORT
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2, remote_addr, BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED,
                                                connect_ind->status);
#else
            bt_sink_srv_cm_profile_status_notify(&remote_addr, BT_SINK_SRV_PROFILE_IAP2, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED,
                                                 connect_ind->status);
#endif

#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
            if (bt_role_handover_get_state() == BT_ROLE_HANDOVER_STATE_ONGOING) {
                bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_IAP2);
            }
#endif
            mux_iap2_reset_handle();
        }

        break;
    }

    case IAP2_DISCONNECT_IND: {
        mux_iap2_disconnection_t disconnection = {0};
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
        if (bt_role_handover_get_state() == BT_ROLE_HANDOVER_STATE_ONGOING) {
            bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_IAP2);
        }
#endif
        if (g_mux_iap2_handle.handle != 0) {
#ifdef MTK_BT_CM_SUPPORT
            bt_cm_profile_service_status_notify(BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2, g_mux_iap2_handle.remote_address,
                                                BT_CM_PROFILE_SERVICE_STATE_DISCONNECTED, BT_STATUS_SUCCESS);
#else
            bt_sink_srv_cm_profile_status_notify(&g_mux_iap2_handle.remote_address, BT_SINK_SRV_PROFILE_IAP2,
                                                 BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, BT_STATUS_SUCCESS);
#endif
        }

        g_mux_iap2_handle.flag |= MUX_IAP2_FLAG_LINK_DISCONNECTING;
        for (uint32_t i = 0; i < MAX_MUX_IAP2_NUM; i++) {
            if (MUX_IAP2_IS_SESSION_CONNECTED(&g_mux_iap2_context[i])) {
                for (uint32_t j = 0; j < MAX_MUX_IAP2_SESSION_NUMBER; j++) {
                    if (g_mux_iap2_context[i].session_id[j] != MUX_IAP2_INVALID_SESSION_ID) {
                        disconnection.session_id = g_mux_iap2_context[i].session_id[j];
                        mux_iap2_context_callback(&g_mux_iap2_context[i], MUX_EVENT_DISCONNECTION, &disconnection);
                    }
                }
            }
        }
        mux_iap2_reset_all_context();
        mux_iap2_reset_handle();

        break;
    }

    case IAP2_EA_SESSION_OPEN_IND: {
        iap2_ea_session_open_close_t *ea_session = (iap2_ea_session_open_close_t *)parameter;
        mux_iap2_context_t *context = mux_iap2_get_context_by_protocol_id(ea_session->protocol_id);

        if (context != NULL) {
            if (context->current_session_num < MAX_MUX_IAP2_SESSION_NUMBER) {
#ifdef AMA_IAP2_APP_RELAY_ENABLE
                iap2_extA_protocol_status_change(g_mux_iap2_handle.handle, ea_session->session_id, SESSION_STATUS_OK);
#endif
                uint32_t i = 0;
                for (i = 0; i < MAX_MUX_IAP2_SESSION_NUMBER; i++) {
                    if (context->session_id[i] == MUX_IAP2_INVALID_SESSION_ID) {
                        context->session_id[i] = ea_session->session_id;
                        context->current_session_num++;
                        break;
                    }
                }
                mux_iap2_connection_t connection = {
                    .session_id = ea_session->session_id,
                    .max_packet_size = g_mux_iap2_handle.max_packet_length
                };
                memcpy(connection.remote_address, g_mux_iap2_handle.remote_address, sizeof(bt_bd_addr_t));
                mux_iap2_context_callback(context, MUX_EVENT_CONNECTION, &connection);
                if (context->is_port_notifyed_con_evt == false) {
                    context->is_port_notifyed_con_evt = true;
                }
            } else {
#ifdef AMA_IAP2_APP_RELAY_ENABLE
                iap2_extA_protocol_status_change(g_mux_iap2_handle.handle, ea_session->session_id, SESSION_CLOSE);
#endif
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]callback IAP2_EA_SESSION_OPEN_IND, reach max session number,start ea session fail! session_id:%x", 1,
                            ea_session->session_id);
            }
        }
        break;
    }

    case IAP2_EA_SESSION_CLOSE_IND: {
        mux_iap2_disconnection_t disconnection = {0};
        iap2_ea_session_open_close_t *ea_session = (iap2_ea_session_open_close_t *)parameter;
        mux_iap2_context_t *context = mux_iap2_get_context_by_session_id(ea_session->session_id);
        if (context != NULL) {
            mux_iap2_reset_packet_by_session_id(context, ea_session->session_id);

            for (uint8_t i = 0; i < MAX_MUX_IAP2_SESSION_NUMBER; i++) {
                if (context->session_id[i] == ea_session->session_id && ea_session->session_id != MUX_IAP2_INVALID_SESSION_ID) {
                    context->session_id[i] = MUX_IAP2_INVALID_SESSION_ID;
                    context->current_session_num--;
                    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]EA session close, session id:0x%x, current session num:0x%x", 2, ea_session->session_id,
                                context->current_session_num);
                    disconnection.session_id = ea_session->session_id;
                    mux_iap2_context_callback(context, MUX_EVENT_DISCONNECTION, &disconnection);
                    break;
                }
            }
        }
        break;
    }

    case IAP2_RECIEVED_DATA_IND: {
        mux_iap2_context_t *context = NULL;
        iap2_data_received_ind_t *received_data = (iap2_data_received_ind_t *)parameter;

        if (received_data->session_type == IAP2_SESSION_TYPE_EXTERNAL_ACCESSORY) {
            context = mux_iap2_get_context_by_session_id(received_data->session_id);
            if (MUX_IAP2_IS_SESSION_CONNECTED(context) && MUX_IAP2_IS_SESSION_VALID(context, received_data->session_id)) {
                virtual_read_write_point_t *rw_point = &context->read_write_point;
                if ((rw_point->rx_buff_len != 0)) {
                    if (context->protocol_config.is_rx_need_session_id) {
                        received_data->packet_length += sizeof(mux_iap2_header_t); /* Send back session id to user */
                        received_data->packet -= sizeof(mux_iap2_header_t);
                    }
                    if ((rw_point->rx_buff_end - rw_point->rx_buff_write_point) >= received_data->packet_length) {
                        memcpy((uint8_t *)(rw_point->rx_buff_write_point), received_data->packet, received_data->packet_length);
                    } else {
                        uint32_t right_length = rw_point->rx_buff_end - rw_point->rx_buff_write_point;
                        uint32_t left_length = received_data->packet_length - right_length;
                        memcpy((uint8_t *)(rw_point->rx_buff_write_point), received_data->packet, right_length);
                        memcpy((uint8_t *)(rw_point->rx_buff_start), received_data->packet + right_length, left_length);
                    }
                    mux_common_device_set_rx_hw_wptr_internal_use(rw_point, received_data->packet_length);
                    mux_iap2_context_callback(context, MUX_EVENT_READY_TO_READ, NULL);
                    if (context->protocol_config.is_rx_need_session_id) {
                        received_data->packet_length -= sizeof(mux_iap2_header_t);
                        received_data->packet += sizeof(mux_iap2_header_t);
                    }

                } else {
                    if (context->protocol_config.is_rx_need_session_id) {
                        received_data->packet_length += sizeof(mux_iap2_header_t); /* Send back session id to user */
                        received_data->packet -= sizeof(mux_iap2_header_t);
                    }
                    rw_point->rx_receive_src_address = received_data->packet;
                    rw_point->rx_receive_src_length  = received_data->packet_length;
                    mux_iap2_context_callback(context, MUX_EVENT_TRANSPARENT_READ, NULL);
                    if (context->protocol_config.is_rx_need_session_id) {
                        received_data->packet_length -= sizeof(mux_iap2_header_t);
                        received_data->packet += sizeof(mux_iap2_header_t);
                    }
                }
            }
            iap2_release_data(IAP2_SESSION_TYPE_EXTERNAL_ACCESSORY, received_data->packet);
        } else if (received_data->session_type == IAP2_SESSION_TYPE_CONTROL) {
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]callback, received control session data", 0);
            iap2_release_data(IAP2_SESSION_TYPE_CONTROL, received_data->packet);
        } else {
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]callback, received unknown session data", 0);
        }

        break;
    }

    case IAP2_READY_TO_SEND_IND: {
        for (uint8_t i = 0; i < MAX_MUX_IAP2_NUM; i++) {
            if (MUX_IAP2_IS_SESSION_CONNECTED(&g_mux_iap2_context[i])) {
                mux_iap2_send_context_data(&g_mux_iap2_context[i]);
                mux_iap2_context_callback(&g_mux_iap2_context[i], MUX_EVENT_READY_TO_WRITE, NULL);
            }
        }

        break;
    }

    default: {
        break;
    }
    }
}

static bt_status_t mux_iap2_app_event_callback(bt_msg_type_t msg, bt_status_t status, void *parameter)
{
    if ((msg == BT_POWER_ON_CNF) && ((g_mux_iap2_handle.flag & MUX_IAP2_FLAG_INIT) == 0)) {
        iap2_status_t iap2_status = iap2_init();
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2]app_event_callback, iAP2 init status:0x%x", 1, iap2_status);

        if (iap2_status == BT_STATUS_SUCCESS) {
            g_mux_iap2_handle.flag |= MUX_IAP2_FLAG_INIT;
        }
    }

    return BT_STATUS_SUCCESS;
}

#ifdef MTK_BT_CM_SUPPORT
bt_status_t mux_iap2_cm_callback(bt_cm_profile_service_handle_t type, void *data)
{
    switch (type) {
    case BT_CM_PROFILE_SERVICE_HANDLE_CONNECT: {
        bt_device_manager_db_remote_pnp_info_t pnp_info = {0};
        bt_device_manager_remote_find_pnp_info((bt_bd_addr_t *)data, &pnp_info);
        if (((g_mux_iap2_handle.handle == 0) && (pnp_info.vender_id != 0x004C) && (pnp_info.vender_id != 0x00)) ||
                ((g_mux_iap2_handle.handle) && (pnp_info.vender_id == 0x004C))) {
            /*the first connect device is non-ios device, ignore
              the second connect device is ios device, ignore due to iap2 not support multiple connect*/
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][CM]cm_callback connect ignore, handle = 0x%08x, vendor_id = 0x%08x", 2, g_mux_iap2_handle.handle, pnp_info.vender_id);
            return BT_STATUS_FAIL;
        }
        memcpy(g_mux_iap2_handle.remote_address, data, sizeof(bt_bd_addr_t));
        iap2_status_t status = iap2_connect(&g_mux_iap2_handle.handle, (const bt_bd_addr_t *)data);
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][CM]cm_callback, connect status:0x%x", 1, status);
        break;
    }

    case BT_CM_PROFILE_SERVICE_HANDLE_DISCONNECT: {
        if (memcmp(data, &g_mux_iap2_handle.remote_address, sizeof(bt_bd_addr_t)) == 0) {
            iap2_status_t status = iap2_disconnect(g_mux_iap2_handle.handle);
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][CM]callback, disconnect status:0x%x", 1, status);
        }
        break;
    }

    default: {
        break;
    }
    }

    return BT_STATUS_SUCCESS;
}
#else
bt_status_t mux_iap2_action_hanler(bt_sink_srv_action_t action, void *parameter)
{
    if (action == BT_SINK_SRV_ACTION_PROFILE_CONNECT) {
        bt_sink_srv_profile_connection_action_t *connection_action = (bt_sink_srv_profile_connection_action_t *)parameter;

        if (connection_action->profile_connection_mask & BT_SINK_SRV_PROFILE_IAP2) {
            memcpy(g_mux_iap2_handle.remote_address, connection_action->address, sizeof(bt_bd_addr_t));
            if (g_mux_iap2_handle.handle != 0) {
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][CM]action_handler connect, prevent repeat iap2 connect", 0);
                return BT_STATUS_SUCCESS;
            }
            iap2_status_t status = iap2_connect(&g_mux_iap2_handle.handle, (const bt_bd_addr_t *)&connection_action->address);
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][CM]action_handler, connect status:0x%x", 1, status);
        }
    }

    return BT_STATUS_SUCCESS;
}
#endif

#ifdef MTK_AWS_MCE_ENABLE
#ifdef BT_ROLE_HANDOVER_WITH_SPP_BLE
bt_status_t iap2_rho_allow_execution_ext(const bt_bd_addr_t *address)
{
    return BT_STATUS_SUCCESS;
}

uint8_t iap2_rho_get_data_length_ext(const bt_bd_addr_t *address)
{
    uint8_t length = 0;
    for (uint8_t i = 0; i < MAX_MUX_IAP2_NUM; i++) {
        if (g_mux_iap2_context[i].current_session_num != 0) {
            length += sizeof(mux_iap2_rho_context_t);
        }
    }
    length += sizeof(mux_iap2_rho_header_t);
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]get data length ext = %d", 1, length);
    return length;
}

bt_status_t iap2_rho_get_data_ext(const bt_bd_addr_t *address, void *data)
{
    uint32_t rho_context_num = 0;
    mux_iap2_rho_header_t *header = (mux_iap2_rho_header_t *)data;
    mux_iap2_rho_context_t *rho_context = (mux_iap2_rho_context_t *)(header + 1);
    for (uint32_t i = 0; i < MAX_MUX_IAP2_NUM; i++) {
        if (g_mux_iap2_context[i].current_session_num != 0) {
            rho_context_num++;
        }
    }
    header->context_num = rho_context_num & 0xFF;
    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]get data ext, context num = %d", 1, header->context_num);

    uint32_t local_context_idx = 0, rho_context_idx = 0;
    for (local_context_idx = 0; local_context_idx < MAX_MUX_IAP2_NUM; local_context_idx++) {
        if ((g_mux_iap2_context[local_context_idx].current_session_num != 0) && (rho_context_idx < header->context_num)) {
            (rho_context + rho_context_idx)->protocol_id = g_mux_iap2_context[local_context_idx].protocol_id;
            memcpy((rho_context + rho_context_idx)->session_id, g_mux_iap2_context[local_context_idx].session_id,
                   sizeof(g_mux_iap2_context[local_context_idx].session_id));
            (rho_context + rho_context_idx)->current_session_num = g_mux_iap2_context[local_context_idx].current_session_num;
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]get data ext, current session num = %d, session id:0x%x, 0x%x, 0x%x", 4,
                        header->context_num, (rho_context + rho_context_idx)->session_id[0], (rho_context + rho_context_idx)->session_id[1],
                        (rho_context + rho_context_idx)->session_id[2]);
            rho_context_idx++;
        }
    }

    return BT_STATUS_SUCCESS;
}

bt_status_t iap2_rho_update_ext(iap2_connect_ind_t *ind, bt_aws_mce_role_t role, void *data)
{
    switch (role) {
    case BT_AWS_MCE_ROLE_AGENT: {
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]update, Agent->Partner success", 0);

        mux_iap2_reset_all_context();
        mux_iap2_reset_handle();

        break;
    }

    case BT_AWS_MCE_ROLE_PARTNER: {
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]update, Partner->Agent success", 0);
        mux_iap2_rho_header_t *header = (mux_iap2_rho_header_t *)data;
        mux_iap2_rho_context_t *rho_context = (mux_iap2_rho_context_t *)(header + 1);

        for (uint8_t i = 0; i < header->context_num; i++) {
            mux_iap2_context_t *context = mux_iap2_get_context_by_protocol_id((rho_context + i)->protocol_id);

            if (context != NULL) {
                //context->session_id = (rho_context + i)->session_id;
                memcpy(context->session_id, (rho_context + i)->session_id, MAX_MUX_IAP2_SESSION_NUMBER * sizeof(uint16_t));
                context->current_session_num = (rho_context + i)->current_session_num;
                LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]update, protocol:0x%x, session0:0x%x, session1:0x%x, session2:0x%x", 4, context->protocol_id,
                            context->session_id[0], context->session_id[1], context->session_id[2]);
            }
        }

#if 0
        g_mux_iap2_handle.handle = bt_spp_get_handle_by_local_server_id(address, BT_SPP_IAP2_SERVER_ID);
        g_mux_iap2_handle.flag = 0;
        g_mux_iap2_handle.max_packet_length = bt_rfcomm_get_max_frame_size((void *)g_mux_iap2_handle.handle);
        memcpy(g_mux_iap2_handle.remote_address, address, sizeof(bt_bd_addr_t));
#endif

        g_mux_iap2_handle.handle = ind->handle;
        g_mux_iap2_handle.flag = MUX_IAP2_FLAG_INIT;
        g_mux_iap2_handle.max_packet_length = ind->max_packet_length;
        memcpy(&g_mux_iap2_handle.remote_address, ind->address, sizeof(bt_bd_addr_t));

        break;
    }

    default: {
        break;
    }
    }

    return BT_STATUS_SUCCESS;
}
#else
bt_status_t mux_iap2_role_handover_allow_execution(const bt_bd_addr_t *address)
{
    bt_status_t status = BT_STATUS_SUCCESS;

    if (g_mux_iap2_handle.handle != 0) {
        status = BT_STATUS_PENDING;
        //g_mux_iap2_handle.flag |= MUX_IAP2_FLAG_REPLY_RHO;
    }

    LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]allow_execution, status:0x%x", 1, status);
    return status;
}

bt_status_t mux_iap2_role_handover_update(bt_role_handover_update_info_t *info)
{
    if (info == NULL) {
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]update, NULL info return FAIL!", 0);
        return BT_STATUS_FAIL;
    }

    switch (info->role) {
    case BT_AWS_MCE_ROLE_AGENT: {
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]update, agent->partner success", 0);
        break;
    }

    case BT_AWS_MCE_ROLE_PARTNER: {
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]update, partner->agent success", 0);

        iap2_status_t status = iap2_connect(&g_mux_iap2_handle.handle, info->addr);
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]update, reconnect result:0x%x", 1, status);

        break;
    }

    default: {
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]update, unknown role!", 0);
        break;
    }
    }

    return BT_STATUS_SUCCESS;
}

void mux_iap2_role_handover_status_callback(const bt_bd_addr_t *address, bt_aws_mce_role_t role, bt_role_handover_event_t event,
        bt_status_t status)
{
    switch (event) {
    case BT_ROLE_HANDOVER_PREPARE_REQ_IND: {
        if (g_mux_iap2_handle.handle != 0) {
            iap2_status_t status = iap2_disconnect(g_mux_iap2_handle.handle);
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]status_callback, disconnect status:0x%x", 1, status);
        }

        break;
    }

    case BT_ROLE_HANDOVER_COMPLETE_IND: {
        if (role == BT_AWS_MCE_ROLE_AGENT && status != BT_STATUS_SUCCESS) {
            iap2_status_t status = iap2_connect(&g_mux_iap2_handle.handle, address);
            LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]status_callback, reconnect status:0x%x", 1, status);
        }

        break;
    }

    default: {
        LOG_MSGID_I(MUX_IAP2, "[MUX][iAP2][RHO]status_callback, role:0x%x event:0x%x status:0x%x", 3, role, event, status);
        break;
    }
    }
}
#endif
#endif
#endif
