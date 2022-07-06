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

#ifdef MTK_PORT_SERVICE_ENABLE
#include "syslog.h"
#include "serial_port.h"
#include "serial_port_internal.h"
#ifdef MTK_IAP2_PROFILE_ENABLE
#include "FreeRTOS.h"
#include "iAP2.h"
#include "serial_port_iap2.h"
#include "bt_sink_srv.h"
#include "bt_callback_manager.h"
#include "bt_connection_manager.h"
#include "bt_connection_manager_internal.h"
#ifdef MTK_AWS_MCE_ENABLE
#include "bt_role_handover.h"
#endif

#define SERIAL_PORT_IAP2_IS_SESSION_CONNECTED(context) \
    ((g_iap2_handle != 0) && ((context) != NULL) && ((context)->callback != NULL) && ((context)->current_session_num != 0))

#define SERIAL_PORT_IAP2_IS_READ_DATA_INVALID(read_data) \
    (((read_data) != NULL) && ((read_data)->buffer != NULL))

#define SERIAL_PORT_IAP2_IS_WRITE_DATA_INVALID(write_data) \
    (((write_data) != NULL) && ((write_data)->data != NULL) && ((write_data)->size <= g_iap2_max_packet_length))

#define SERIAL_PORT_IAP2_IS_SESSION_VALID(context, target_session_id) \
    ((((context)->session_id[0] == (target_session_id)) || ((context)->session_id[1] == (target_session_id)) || ((context)->session_id[2] == (target_session_id))) && ((target_session_id) != SERIAL_PORT_IAP2_INVALID_SESSION_ID))


typedef struct {
    uint32_t handle;
    uint16_t max_packet_length;
    bt_bd_addr_t remote_address;
} serial_port_iap2_handle_t;

typedef struct serial_port_iap2_packet {
    uint8_t *data;
    uint16_t data_length;
    struct serial_port_iap2_packet *next;
} serial_port_iap2_packet_t;

typedef struct {
    uint8_t is_rx_need_session_id: 1;
    uint8_t reserved: 7;
} serial_port_iap2_protocol_config_t;

typedef struct {
    serial_port_register_callback_t callback;
    serial_port_iap2_protocol_config_t protocol_config;
    uint16_t protocol_id;
    uint16_t session_id[MAX_SERIAL_PORT_IAP2_SESSION_NUMBER];
    uint8_t current_session_num;

    //uint8_t *packet;
    //uint16_t packet_length;

    serial_port_iap2_packet_t *head_packet;
    serial_port_iap2_packet_t *tail_packet;
} serial_port_iap2_context_t;

static uint32_t g_iap2_handle;
static uint16_t g_iap2_max_packet_length;
static bt_bd_addr_t g_iap2_remote_address;
//static bool g_iap2_need_reconnect;
static bool g_iap2_reply_rho;

static serial_port_iap2_context_t g_serial_port_iap2_context[MAX_IAP2_PORT_NUM] = {
    {NULL, {0}, SERIAL_PORT_IAP2_SESSION1_PROTOCOL_ID, {0}, 0, NULL, NULL},
    {NULL, {0}, SERIAL_PORT_IAP2_SESSION2_PROTOCOL_ID, {0}, 0, NULL, NULL},
    {NULL, {0}, SERIAL_PORT_IAP2_SESSION3_PROTOCOL_ID, {0}, 0, NULL, NULL},
    {NULL, {0}, SERIAL_PORT_IAP2_SESSION4_PROTOCOL_ID, {0}, 0, NULL, NULL}
};

log_create_module(IAP2_PORT, PRINT_LEVEL_INFO);

extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);
static void serial_port_iap2_reset_handle(void);
static void serial_port_iap2_reset_context(serial_port_iap2_context_t *context);
static void serial_port_iap2_reset_all_context(void);
static serial_port_dev_t serial_port_iap2_get_device_by_context(serial_port_iap2_context_t *context);
static serial_port_iap2_context_t *serial_port_iap2_get_context_by_device(serial_port_dev_t device);
static serial_port_iap2_context_t *serial_port_iap2_get_context_by_protocol_id(uint16_t protocol_id);
static serial_port_iap2_context_t *serial_port_iap2_get_context_by_session_id(uint16_t session_id);
static void serial_port_iap2_device_callback(serial_port_iap2_context_t *context, serial_port_callback_event_t event, void *parameter);
static bt_status_t serial_port_iap2_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff);
static void serial_port_iap2_common_callback(iap2_event_t event, void *para);

#ifdef MTK_AWS_MCE_ENABLE
static bt_status_t serial_port_iap2_role_handover_allow_execution(const bt_bd_addr_t *address);
static bt_status_t serial_port_iap2_role_handover_update(bt_role_handover_update_info_t *info);
static void serial_port_iap2_role_handover_status_callback(const bt_bd_addr_t *address, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status);
#endif

extern iap2_status_t iap2_send_app_launch_request(uint32_t handle, uint8_t *app_id, bool is_alert);

void serial_port_iap2_register_callbacks(void)
{
#ifdef MTK_AWS_MCE_ENABLE
    bt_role_handover_callbacks_t rho_callbacks = {
        .allowed_cb = serial_port_iap2_role_handover_allow_execution,
        .update_cb = serial_port_iap2_role_handover_update,
        .status_cb = serial_port_iap2_role_handover_status_callback
    };

    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_IAP2, &rho_callbacks);
#endif
    iap2_register_callback(serial_port_iap2_common_callback);
    bt_callback_manager_register_callback(bt_callback_type_app_event, MODULE_MASK_SYSTEM, serial_port_iap2_event_callback);
}

serial_port_status_t serial_port_iap2_init(serial_port_dev_t device, serial_port_open_para_t *para, void *priv_data)
{
    serial_port_iap2_context_t *context = NULL;
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]init device:0x%x, callback:0x%x", 2, device, para->callback);

    if ((context = serial_port_iap2_get_context_by_device(device)) == NULL)
    {
        return SERIAL_PORT_STATUS_INVALID_DEVICE;
    }

    if (context->callback == NULL)
    {
        context->callback = para->callback;
    }
    else
    {
        return SERIAL_PORT_STATUS_FAIL;
    }

    if (SERIAL_PORT_IAP2_IS_SESSION_CONNECTED(context))
    {
        serial_port_iap2_session_open_t session_open = {
            .max_packet_length = g_iap2_max_packet_length,
            //.remote_address = (const bt_bd_addr_t *)&g_iap2_remote_address
        };
        memcpy(&session_open.remote_address, &g_iap2_remote_address, sizeof(bt_bd_addr_t));
        
        for (uint8_t i = 0; i < MAX_SERIAL_PORT_IAP2_SESSION_NUMBER; i++) {
            if (context->session_id[i] != SERIAL_PORT_IAP2_INVALID_SESSION_ID) {
                session_open.session_id = context->session_id[i];
                serial_port_iap2_device_callback(context, SERIAL_PORT_EVENT_IAP2_SESSION_OPEN, &session_open);
            }
        }
    }

    return SERIAL_PORT_STATUS_OK;
}

serial_port_status_t serial_port_iap2_control(serial_port_dev_t device, serial_port_ctrl_cmd_t cmd, serial_port_ctrl_para_t *para)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]control device:0x%x, cmd:0x%x, para:0x%x", 3, device, cmd, para);
    serial_port_iap2_context_t *context = NULL;
    switch (cmd)
    {
        case SERIAL_PORT_CMD_READ_DATA:
        case SERIAL_PORT_CMD_READ_DATA_BLOCKING:
        {
            serial_port_read_data_t *read_data = (serial_port_read_data_t *)para;
            context = serial_port_iap2_get_context_by_device(device);

            if (!SERIAL_PORT_IAP2_IS_SESSION_CONNECTED(context))
            {
                return SERIAL_PORT_STATUS_INVALID_DEVICE;
            }

            if (!SERIAL_PORT_IAP2_IS_READ_DATA_INVALID(read_data))
            {
                return SERIAL_PORT_STATUS_INVALID_PARAMETER;
            }
#if 0
            if (context->packet != NULL && context->packet_length > 0)
            {
                memcpy(read_data->buffer, context->packet, context->packet_length);
                read_data->ret_size = context->packet_length;

                iap2_release_data(IAP2_SESSION_TYPE_EXTERNAL_ACCESSORY, context->packet);

                context->packet = NULL;
                context->packet_length = 0;
            }
            else
            {
                read_data->ret_size = 0;
                return SERIAL_PORT_STATUS_FAIL;
            }
#endif
            if (context->head_packet != NULL && read_data->size >= context->head_packet->data_length)
            {
                memcpy(read_data->buffer, context->head_packet->data, context->head_packet->data_length);
                read_data->ret_size = context->head_packet->data_length;

                serial_port_iap2_packet_t *delete_packet = context->head_packet;
                if (context->protocol_config.is_rx_need_session_id) {
                    delete_packet->data += sizeof(serial_port_iap2_header_t);
                }
                iap2_release_data(IAP2_SESSION_TYPE_EXTERNAL_ACCESSORY, delete_packet->data);

                bt_os_take_stack_mutex();
                if ((context->head_packet = delete_packet->next) == NULL)
                {
                    context->tail_packet = NULL;
                }
                bt_os_give_stack_mutex();

                vPortFree(delete_packet);
                LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]control Deleted packet:0x%x, current head:0x%x tail:0x%x", 3,
                    delete_packet, context->head_packet, context->tail_packet);
            }
            else
            {
                read_data->ret_size = 0;
                return SERIAL_PORT_STATUS_FAIL;
            }

            break;
        }

        case SERIAL_PORT_CMD_WRITE_DATA:
        case SERIAL_PORT_CMD_WRITE_DATA_BLOCKING:
        {
            serial_port_write_data_t *write_data = (serial_port_write_data_t *)para;
            context = serial_port_iap2_get_context_by_device(device);

            if (!SERIAL_PORT_IAP2_IS_SESSION_CONNECTED(context))
            {
                return SERIAL_PORT_STATUS_INVALID_DEVICE;
            }

            if (!SERIAL_PORT_IAP2_IS_WRITE_DATA_INVALID(write_data))
            {
                return SERIAL_PORT_STATUS_INVALID_PARAMETER;
            }

            serial_port_iap2_header_t serial_port_iap2_header;
            serial_port_iap2_header.session_id = ((serial_port_iap2_header_t *)(write_data->data))->session_id;
            if (SERIAL_PORT_IAP2_IS_SESSION_VALID(context, serial_port_iap2_header.session_id)) {
                if (iap2_send_data_by_external_accessory_session(g_iap2_handle, serial_port_iap2_header.session_id,
                    write_data->data + sizeof(serial_port_iap2_header_t), write_data->size - sizeof(serial_port_iap2_header_t)) == IAP2_STATUS_SUCCESS) {
                    write_data->ret_size = write_data->size;
                } else {
                    write_data->ret_size = 0;
                    return SERIAL_PORT_STATUS_FAIL;
                }
            } else {
                write_data->ret_size = 0;
                return SERIAL_PORT_STATUS_FAIL;
            }
            break;
        }

        case SERIAL_PORT_CMD_GET_CONNECTION_PARAM:
        {
            serial_port_get_connection_param_t *get_con_param = (serial_port_get_connection_param_t *)para;
            context = serial_port_iap2_get_context_by_device(device);
            if (context == NULL || para == NULL) {
                return SERIAL_PORT_STATUS_FAIL;
            }
            get_con_param->max_packet_size = g_iap2_max_packet_length;
            memcpy(&get_con_param->remote_address, &g_iap2_remote_address, sizeof(bt_bd_addr_t));
            get_con_param->iap2_session_num = context->current_session_num;
            memcpy(get_con_param->iap2_session_id, context->session_id, MAX_SERIAL_PORT_IAP2_SESSION_NUMBER * sizeof(uint16_t));
            break;
        }

        case SERIAL_PORT_CMD_SET_RX_PARAM:
        {
            serial_port_set_config_param_t *config_param = (serial_port_set_config_param_t *)para;
            context = serial_port_iap2_get_context_by_device(device);
            if (context == NULL || para == NULL) {
                return SERIAL_PORT_STATUS_FAIL;
            }
            context->protocol_config.is_rx_need_session_id = config_param->is_rx_need_session_id;
            break;
        }

        default:
        {
            return SERIAL_PORT_STATUS_UNSUPPORTED;
        }
    }

    return SERIAL_PORT_STATUS_OK;
}

serial_port_status_t serial_port_iap2_deinit(serial_port_dev_t device)
{
    serial_port_iap2_context_t *context = NULL;
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]deinit device:0x%x", 1, device);

    if ((context = serial_port_iap2_get_context_by_device(device)) != NULL)
    {
        serial_port_iap2_device_callback(context, SERIAL_PORT_EVENT_IAP2_SESSION_CLOSE, NULL);
        context->callback = NULL;
    }
    else
    {
        return SERIAL_PORT_STATUS_INVALID_DEVICE;
    }

    return SERIAL_PORT_STATUS_OK;
}

static void serial_port_iap2_reset_handle(void)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]reset_handle", 0);

    g_iap2_handle = 0;
    g_iap2_max_packet_length = 0;
    memset(&g_iap2_remote_address, 0, sizeof(bt_bd_addr_t));
}

static void serial_port_iap2_reset_context(serial_port_iap2_context_t * context)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]reset_context context:0x%4x", 1, context);

    while (context->head_packet != NULL)
    {
        serial_port_iap2_packet_t *delete_packet = context->head_packet;
        context->head_packet = delete_packet->next;

        iap2_release_data(IAP2_SESSION_TYPE_EXTERNAL_ACCESSORY, delete_packet->data);
        vPortFree(delete_packet);
        LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]reset_context Delete packet:0x%x, current head:0x%x tail:0x%x", 3,
            delete_packet, context->head_packet, context->tail_packet);
    }

    //context->callback = NULL;
    //context->session_id = 0;
    memset(context->session_id, 0, MAX_SERIAL_PORT_IAP2_SESSION_NUMBER * sizeof(uint16_t));
    context->current_session_num = 0;
    context->head_packet = NULL;
    context->tail_packet = NULL;
}

static void serial_port_iap2_reset_context_by_session_id(uint16_t session_id)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]reset_context_by_session_id, session_id:0x%x", 1, session_id);
    serial_port_iap2_context_t *context = serial_port_iap2_get_context_by_session_id(session_id);
    if (context != NULL) {
        serial_port_iap2_packet_t *head = context->head_packet;  /*first node of the packet list*/
        serial_port_iap2_packet_t *packet = context->head_packet;/*to reverse whole list*/
        while (packet != NULL)
        {
            if (((serial_port_iap2_header_t *)(packet->data))->session_id == session_id) {
                serial_port_iap2_packet_t *pkt_to_delete = packet;
                if (pkt_to_delete == head) {
                    head = pkt_to_delete->next;
                }
                packet = pkt_to_delete->next;
                iap2_release_data(IAP2_SESSION_TYPE_EXTERNAL_ACCESSORY, pkt_to_delete->data);
                vPortFree(pkt_to_delete);
                LOG_MSGID_I(IAP2_PORT, "[PORT][iAP2]reset_context_by_session_id, delete packet:0x%x, current head:0x%x tail:0x%x", 3,
                    pkt_to_delete, context->head_packet, context->tail_packet);
            } else {
                packet = packet->next;
            }
        }
        if (head != NULL) {
            context->head_packet = head;
            packet = head;
            while (packet->next != NULL) {
                packet = packet->next;
            }
            context->tail_packet = packet;
        } else {
                context->head_packet = NULL;
                context->tail_packet = NULL;
        }
    }
}

static void serial_port_iap2_reset_all_context(void)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]reset_all_context", 0);

    for (uint8_t i = 0; i < MAX_IAP2_PORT_NUM; i++)
    {
        serial_port_iap2_reset_context(&g_serial_port_iap2_context[i]);
    }
}

static serial_port_dev_t serial_port_iap2_get_device_by_context(serial_port_iap2_context_t * context)
{
    serial_port_dev_t device = SERIAL_PORT_DEV_UNDEFINED;

    if (context == &g_serial_port_iap2_context[0])
    {
        device = SERIAL_PORT_DEV_IAP2_SESSION1;
    }
    else if (context == &g_serial_port_iap2_context[1])
    {
        device = SERIAL_PORT_DEV_IAP2_SESSION2;
    }
    else if (context == &g_serial_port_iap2_context[2])
    {
        device = SERIAL_PORT_DEV_IAP2_SESSION3;
    }
    else
    {
        device = SERIAL_PORT_DEV_UNDEFINED;
    }

    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]get_device_by_context context:0x%x, device:0x%x", 2, context, device);

    return device;
}

static serial_port_iap2_context_t *serial_port_iap2_get_context_by_device(serial_port_dev_t device)
{
    serial_port_iap2_context_t *context = NULL;

    switch (device)
    {
        case SERIAL_PORT_DEV_IAP2_SESSION1:
        {
            context = &g_serial_port_iap2_context[0];
            break;
        }

        case SERIAL_PORT_DEV_IAP2_SESSION2:
        {
            context = &g_serial_port_iap2_context[1];
            break;
        }

        case SERIAL_PORT_DEV_IAP2_SESSION3:
        {
            context = &g_serial_port_iap2_context[2];
            break;
        }

        case SERIAL_PORT_DEV_IAP2_SESSION4:
        {
            context = &g_serial_port_iap2_context[3];
            break;
        }

        default:
        {
            context = NULL;
            break;
        }
    }

    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]get_context_by_device device:0x%x, context:0x%x", 2, device, context);

    return context;
}

static serial_port_iap2_context_t *serial_port_iap2_get_context_by_protocol_id(uint16_t protocol_id)
{
    serial_port_iap2_context_t *context = NULL;

    for (uint8_t i = 0; i < MAX_IAP2_PORT_NUM; i++)
    {
        if (g_serial_port_iap2_context[i].protocol_id == protocol_id)
        {
            context = &g_serial_port_iap2_context[i];
            break;
        }
    }

    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]get_context_by_protocol_id protocol_id:0x%x, context:0x%x", 2, protocol_id, context);

    return context;
}

static serial_port_iap2_context_t *serial_port_iap2_get_context_by_session_id(uint16_t session_id)
{
    serial_port_iap2_context_t *context = NULL;

    for (uint8_t i = 0; i < MAX_IAP2_PORT_NUM; i++)
    {
        if (SERIAL_PORT_IAP2_IS_SESSION_VALID(&g_serial_port_iap2_context[i], session_id)) {
            context = &g_serial_port_iap2_context[i];
            break;
        }
    }

    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]get_context_by_session_id session_id:0x%x, context:0x%x", 2, session_id, context);

    return context;
}

static void serial_port_iap2_device_callback(serial_port_iap2_context_t *context, serial_port_callback_event_t event, void * parameter)
{
    serial_port_dev_t device = serial_port_iap2_get_device_by_context(context);

    if (event == SERIAL_PORT_EVENT_IAP2_SESSION_OPEN && parameter != NULL)
    {
        serial_port_iap2_session_open_t *session_open = (serial_port_iap2_session_open_t *)parameter;
        LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]device_callback IAP2_SESSION_OPEN session_id:0x%x, MTU:0x%x, address:0x%x:%x:%x:%x:%x:%x", 8,
            session_open->session_id, session_open->max_packet_length, session_open->remote_address[0], session_open->remote_address[1],
            session_open->remote_address[2], session_open->remote_address[3], session_open->remote_address[4], session_open->remote_address[5]);
    } else if (event == SERIAL_PORT_EVENT_IAP2_SESSION_CLOSE && parameter != NULL) {
        serial_port_iap2_session_close_t *session_close = (serial_port_iap2_session_close_t *)parameter;
        LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]device_callback IAP2_SESSION_CLOSE session_id:0x%x", 1, session_close->session_id);
    }

    if (device != SERIAL_PORT_DEV_UNDEFINED && context->callback != NULL)
    {
        context->callback(device, event, parameter);
    }
}

bt_status_t bt_sink_srv_iap2_action_handler(bt_sink_srv_action_t action, void *parameter)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]action_handler action:0x%x, parameter:0x%x", 2, action, parameter);

    switch (action)
    {
        case BT_SINK_SRV_ACTION_PROFILE_CONNECT:
        {
            bt_sink_srv_profile_connection_action_t *connection_action = (bt_sink_srv_profile_connection_action_t *)parameter;

            if (connection_action->profile_connection_mask & BT_SINK_SRV_PROFILE_IAP2)
            {
                memcpy(g_iap2_remote_address, connection_action->address, sizeof(bt_bd_addr_t));
                iap2_status_t status = iap2_connect(&g_iap2_handle, (const bt_bd_addr_t *)&connection_action->address);
                LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]action_handler Connect status:0x%x", 1, status);
            }

            break;
        }

        default:
        {
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}

void serial_port_iap2_send_app_launch_request(uint8_t *app_id, bool is_alert)
{
    iap2_send_app_launch_request(g_iap2_handle, app_id, is_alert);
}

static bt_status_t serial_port_iap2_event_callback(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]event_callback msg:0x%x, status:0x%x, buff:0x%x", 3, msg, status, buff);

    switch (msg)
    {
        case BT_POWER_ON_CNF:
        {
            iap2_init();
            break;
        }
#if 0
        case BT_GAP_LINK_STATUS_UPDATED_IND:
        {
            bt_gap_link_status_updated_ind_t *link_status = (bt_gap_link_status_updated_ind_t *)buff;

            if (link_status->link_status == BT_GAP_LINK_STATUS_DISCONNECTED)
            {
                if ((status == BT_SINK_SRV_CM_REASON_CONNECTION_TIMEOUT) ||
                    (status == BT_SINK_SRV_CM_REASON_LMP_RESPONSE_TIMEOUT) ||
                    (status == BT_SINK_SRV_CM_REASON_ROLE_SWITCH_PENDING))
                {
                    g_iap2_need_reconnect = true;
                    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]event callback, link loss and will reconnect next time.", 0);
                }
            }
            else if (link_status->link_status >= BT_GAP_LINK_STATUS_CONNECTED_0)
            {
                if (g_iap2_need_reconnect && (bt_connection_manager_device_local_info_get_aws_role() == BT_AWS_MCE_ROLE_AGENT))
                {
                    g_iap2_need_reconnect = false;
                    iap2_status_t status = iap2_connect(&g_iap2_handle, link_status->address);
                    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]event callback, reconnect status=0x%x.", 1, status);
                }
            }

            break;
        }
#endif
        default:
        {
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}

void serial_port_iap2_common_callback(iap2_event_t event, void *para)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]common_callback event:0x%2x, param:0x%4x", 2, event, para);

    switch (event)
    {
        case IAP2_CONNECT_IND:
        {
            iap2_connect_ind_t *connect_ind = (iap2_connect_ind_t *)para;

            if (connect_ind->status == IAP2_STATUS_SUCCESS)
            {
                g_iap2_handle = connect_ind->handle;
                g_iap2_max_packet_length = connect_ind->max_packet_length;
                memcpy(&g_iap2_remote_address, connect_ind->address, sizeof(bt_bd_addr_t));

                bt_sink_srv_cm_profile_status_notify(connect_ind->address, BT_SINK_SRV_PROFILE_IAP2, BT_SINK_SRV_PROFILE_CONNECTION_STATE_CONNECTED, BT_STATUS_SUCCESS);
                LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]common_callback IAP2_CONNECT_IND remote address:0x%x:%x:%x:%x:%x:%x", 6,
                    (*(connect_ind->address))[0], (*(connect_ind->address))[1], (*(connect_ind->address))[2],
                    (*(connect_ind->address))[3], (*(connect_ind->address))[4], (*(connect_ind->address))[5]);
            }
            else
            {
                bt_sink_srv_cm_profile_status_notify(&g_iap2_remote_address, BT_SINK_SRV_PROFILE_IAP2, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, BT_STATUS_UNSUPPORTED);
                serial_port_iap2_reset_handle();
            }

            break;
        }

        case IAP2_DISCONNECT_IND:
        {
#ifdef MTK_AWS_MCE_ENABLE
            if (g_iap2_reply_rho)
            {
                bt_role_handover_reply_prepare_request(BT_ROLE_HANDOVER_MODULE_IAP2);
                g_iap2_reply_rho = false;
            }
#endif
            for (uint8_t i = 0; i < MAX_IAP2_PORT_NUM; i++)
            {
                if (SERIAL_PORT_IAP2_IS_SESSION_CONNECTED(&g_serial_port_iap2_context[i]))
                {
                    serial_port_iap2_device_callback(&g_serial_port_iap2_context[i], SERIAL_PORT_EVENT_IAP2_SESSION_CLOSE, NULL);
                }
            }

            bt_sink_srv_cm_profile_status_notify(&g_iap2_remote_address, BT_SINK_SRV_PROFILE_IAP2, BT_SINK_SRV_PROFILE_CONNECTION_STATE_DISCONNECTED, BT_STATUS_SUCCESS);
            serial_port_iap2_reset_handle();
            serial_port_iap2_reset_all_context();

            break;
        }

        case IAP2_EA_SESSION_OPEN_IND:
        {
            serial_port_iap2_context_t *context = NULL;
            iap2_ea_session_open_close_t *ea_session = (iap2_ea_session_open_close_t *)para;

            if ((context = serial_port_iap2_get_context_by_protocol_id(ea_session->protocol_id)) != NULL)
            {
                serial_port_iap2_session_open_t session_open = {
                    .session_id = ea_session->session_id,
                    .max_packet_length = g_iap2_max_packet_length,
                    //.remote_address = (const bt_bd_addr_t *)&g_iap2_remote_address
                };
                if (context->current_session_num < MAX_SERIAL_PORT_IAP2_SESSION_NUMBER) {
                    context->session_id[context->current_session_num++] = ea_session->session_id;
                    memcpy(&session_open.remote_address, &g_iap2_remote_address, sizeof(bt_bd_addr_t));
                    serial_port_iap2_device_callback(context, SERIAL_PORT_EVENT_IAP2_SESSION_OPEN, &session_open);
                }
            }
            break;
        }

        case IAP2_EA_SESSION_CLOSE_IND:
        {
            serial_port_iap2_context_t *context = NULL;
            iap2_ea_session_open_close_t *ea_session = (iap2_ea_session_open_close_t *)para;

            if ((context = serial_port_iap2_get_context_by_session_id(ea_session->session_id)) != NULL)
            {
                for (uint8_t i = 0; i < MAX_SERIAL_PORT_IAP2_SESSION_NUMBER; i++) {
                    if (context->session_id[i] == ea_session->session_id && ea_session->session_id != SERIAL_PORT_IAP2_INVALID_SESSION_ID) {
                        context->session_id[i] = SERIAL_PORT_IAP2_INVALID_SESSION_ID;
                        context->current_session_num--;
                        break;
                    }
                }
                serial_port_iap2_session_close_t session_close = {
                    .session_id = ea_session->session_id
                };
                serial_port_iap2_device_callback(context, SERIAL_PORT_EVENT_IAP2_SESSION_CLOSE, &session_close);
                serial_port_iap2_reset_context_by_session_id(ea_session->session_id);
            }

            break;
        }

        case IAP2_RECIEVED_DATA_IND:
        {
            serial_port_iap2_context_t *context = NULL;
            iap2_data_received_ind_t *received_data = (iap2_data_received_ind_t *)para;

            if (received_data->session_type == IAP2_SESSION_TYPE_EXTERNAL_ACCESSORY &&
                (context = serial_port_iap2_get_context_by_session_id(received_data->session_id)) != NULL)
            {
#if 0
                if (context->packet != NULL)
                {
                    iap2_release_data(IAP2_SESSION_TYPE_EXTERNAL_ACCESSORY, context->packet);
                }

                context->packet = received_data->packet;
                context->packet_length = received_data->packet_length;
                serial_port_iap2_device_callback(context, SERIAL_PORT_EVENT_READY_TO_READ, NULL);
#endif
                serial_port_iap2_packet_t *add_packet = pvPortMalloc(sizeof(serial_port_iap2_packet_t));

                if (add_packet == NULL)
                {
                    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]common_callback Alloc packet FAILED!", 0);
                    break;
                }
                /* Send back session id to user */
                if (context->protocol_config.is_rx_need_session_id) {
                    received_data->packet -= sizeof(serial_port_iap2_header_t);
                    received_data->packet_length += sizeof(serial_port_iap2_header_t);
                }
                add_packet->data = received_data->packet;
                add_packet->data_length = received_data->packet_length;
                add_packet->next = NULL;
#if 0
                add_packet->data = received_data->packet - sizeof(serial_port_iap2_header_t);
                add_packet->data_length = received_data->packet_length + sizeof(serial_port_iap2_header_t);
                add_packet->next = NULL;
#endif
                bt_os_take_stack_mutex();
                if (context->head_packet == NULL)
                {
                    context->head_packet = add_packet;
                    context->tail_packet = add_packet;
                }
                else
                {
                    context->tail_packet->next = add_packet;
                    context->tail_packet = add_packet;
                }
                bt_os_give_stack_mutex();

                serial_port_iap2_device_callback(context, SERIAL_PORT_EVENT_READY_TO_READ, NULL);
                LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]common_callback Add packet:0x%x, current head:0x%x tail:0x%x", 3,
                    add_packet, context->head_packet, context->tail_packet);
            }
            else if (received_data->session_type == IAP2_SESSION_TYPE_CONTROL)
            {
                LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]common_callback Received control session data", 0);
                iap2_release_data(IAP2_SESSION_TYPE_CONTROL, received_data->packet);
            }
            else
            {
                LOG_MSGID_I(IAP2_PORT, "[Port][iAP2]common_callback Received unknown session data", 0);
            }

            break;
        }

        case IAP2_READY_TO_SEND_IND:
        {
            for (uint8_t i = 0; i < MAX_IAP2_PORT_NUM; i++)
            {
                if (SERIAL_PORT_IAP2_IS_SESSION_CONNECTED(&g_serial_port_iap2_context[i]))
                {
                    serial_port_iap2_device_callback(&g_serial_port_iap2_context[i], SERIAL_PORT_EVENT_READY_TO_WRITE, NULL);
                }
            }

            break;
        }

        default:
        {
            break;
        }
    }
}
#ifdef MTK_AWS_MCE_ENABLE
static bt_status_t serial_port_iap2_role_handover_allow_execution(const bt_bd_addr_t * address)
{
    bt_status_t status = BT_STATUS_SUCCESS;

    if (g_iap2_handle != 0)
    {
        g_iap2_reply_rho = true;
        status = BT_STATUS_PENDING;
    }

    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2][RHO]allow_execution status:0x%x", 1, status);
    return status;
}

static bt_status_t serial_port_iap2_role_handover_update(bt_role_handover_update_info_t *info)
{
    switch (info->role)
    {
        case BT_AWS_MCE_ROLE_AGENT:
        {
            LOG_MSGID_I(IAP2_PORT, "[Port][iAP2][RHO]update Success, agent->partner", 0);
            break;
        }

        case BT_AWS_MCE_ROLE_PARTNER:
        {
            LOG_MSGID_I(IAP2_PORT, "[Port][iAP2][RHO]update Success, partner->agent", 0);

            iap2_status_t status = iap2_connect(&g_iap2_handle, info->addr);
            LOG_MSGID_I(IAP2_PORT, "[Port][iAP2][RHO]update Reconnect result:0x%x", 1, status);

            break;
        }

        default:
        {
            LOG_MSGID_I(IAP2_PORT, "[Port][iAP2][RHO]update Unknown role!", 0);
            break;
        }
    }

    return BT_STATUS_SUCCESS;
}

static void serial_port_iap2_role_handover_status_callback(const bt_bd_addr_t * address, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    LOG_MSGID_I(IAP2_PORT, "[Port][iAP2][RHO]status_callback role:0x%x, event:0x%x, status:0x%x", 3, role, event, status);

    switch (event)
    {
        case BT_ROLE_HANDOVER_PREPARE_REQ_IND:
        {
            if (g_iap2_handle != 0)
            {
                iap2_status_t status = iap2_disconnect(g_iap2_handle);
                LOG_MSGID_I(IAP2_PORT, "[Port][iAP2][RHO]status_callback Disconnect status:0x%x", 1, status);
            }

            break;
        }

        case BT_ROLE_HANDOVER_COMPLETE_IND:
        {
            if (role == BT_AWS_MCE_ROLE_AGENT && status != BT_STATUS_SUCCESS)
            {
                iap2_status_t status = iap2_connect(&g_iap2_handle, address);
                LOG_MSGID_I(IAP2_PORT, "[Port][iAP2][RHO]status_callback Reconnect result:0x%x", 1, status);
            }

            break;
        }

        default:
        {
            break;
        }
    }
}
#endif
#else
serial_port_status_t serial_port_iap2_init(serial_port_dev_t device, serial_port_open_para_t *para, void *priv_data)
{
    return SERIAL_PORT_STATUS_UNSUPPORTED;
}

serial_port_status_t serial_port_iap2_control(serial_port_dev_t device, serial_port_ctrl_cmd_t cmd, serial_port_ctrl_para_t *para)
{
    return SERIAL_PORT_STATUS_UNSUPPORTED;
}

serial_port_status_t serial_port_iap2_deinit(serial_port_dev_t device)
{
    return SERIAL_PORT_STATUS_UNSUPPORTED;
}
#endif
#endif
