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
/**
 * File: bt_ancs_common.c
 *
 * Description: The file is to get ANCS information and proxy.
 *
 * Note: See doc/AB1565_AB1568_Earbuds_Reference_Design_User_Guide.pdf for more detail.
 *
 */
#include "FreeRTOS.h"
#include "bt_hci.h"
#include "ble_ancs.h"
#include "bt_gattc.h"
#include "bt_debug.h"
#include "bt_gatts.h"
#include "bt_ancs_common.h"
#include "bt_callback_manager.h"
#include "syslog.h"

#define ANCS_PROXY_FLOW_ON   0x01                                     /* ANCS proxy flow ON flag. */
#define ANCS_PROXY_ASYNC_RSP 0x02                                     /* ANCS proxy flow atomic operation flag. */

static uint8_t ancs_proxy_state = 0x00;                               /* ANCS proxy flow control variable. */
static ble_ancs_srv_content_t g_ancs_srv;                             /* ANCS common context,store the ANCS service to discovery. */
static bt_status_t ble_ancs_proxy_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff);
bt_handle_t test_conn_handle = 0;                                           /* BLE connection handle,use to ANCS test. */
const uint8_t ctrl_point_uuid[16] = {0xd9, 0xd9, 0xaa, 0xfd, 0xbd, 0x9b, 0x21, 0x98, 0xa8, 0x49, 0xe1, 0x45, 0xf3, 0xd8, 0xd1, 0x69};          /* ANCS control point UUID. */
const uint8_t noti_source_uuid[16] = {0xbd, 0x1d, 0xa2, 0x99, 0xe6, 0x25, 0x58, 0x8c, 0xd9, 0x42, 0x01, 0x63, 0x0d, 0x12, 0xbf, 0x9f};         /* ANCS notification source UUID. */
const uint8_t data_source_uuid[16] = {0xfb, 0x7b, 0x7c, 0xce, 0x6a, 0xb3, 0x44, 0xbe, 0xb5, 0x4b, 0xd6, 0x24, 0xe9, 0xc6, 0xea, 0x22};         /* ANCS data source UUID. */
static bt_gattc_discovery_characteristic_t *current_op_charc = NULL;         /* temp characteristic variable. */


/**
 * @brief   This function is a static callback for the application to listen to the event. Provide a user-defined callback.
 * @param[in] msg     is the callback message type.
 * @param[in] status  is the status of the callback message.
 * @param[in] buf     is the payload of the callback message.
 * @return            The status of this operation returned from the callback.
 */
static bt_status_t ble_ancs_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    bt_status_t result;
    /* Do not remove ble_ancs_event_callback, make sure ancs lib can receive bt stack events */
    ble_ancs_event_callback(msg, status, buff);
    if (ble_ancs_proxy_event_handler(msg, status, buff) == BT_STATUS_SUCCESS) {
        return BT_STATUS_SUCCESS;
    }
    /* BT callback event. */
    switch (msg) {
        case BT_POWER_ON_CNF:
            break;
        case BT_GAP_LE_CONNECT_IND:
            break;
        case BT_GAP_LE_BONDING_COMPLETE_IND:
            /* BLE bonding complete. */
            if (status != BT_STATUS_SUCCESS) {
                LOG_MSGID_I(ANCS_APP, "BT_GAP_LE_BONDING_COMPLETE_IND, status = %d\n", 1, status);
            }
            break;
        case BT_GATTC_READ_CHARC:
            /* GATTC read characteristic response event. */
            if (status == BT_STATUS_SUCCESS) {
                bt_gattc_read_rsp_t *rsp = (bt_gattc_read_rsp_t *)buff;
                /* GATTC read notification source. */
                if (g_ancs_srv.status == ANCS_COMM_STATUS_READ_NOTIF_SOURCE) {
                    uint8_t i;
                    LOG_MSGID_I(ANCS_APP, "notificaiton source enabled = %d\n", 1, rsp->att_rsp->attribute_value[0]);
                    /* Read data source descriptor. */
                    for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
                        if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, data_source_uuid, 16)) {
                            break;
                        }
                    }
                    if (i < BLE_ANCS_MAX_CHARC_NUMBER) {
                        BT_GATTC_NEW_READ_CHARC_REQ(req, g_ancs_srv.service.charateristics[i].descriptor[0].handle);
                        result = bt_gattc_read_charc(rsp->connection_handle, &req);
                        if (!(result == BT_STATUS_SUCCESS || result == BT_STATUS_PENDING)) {
                            LOG_MSGID_I(ANCS_APP, "read data source error (0x%x)\n", result);
                        }
                        test_conn_handle = rsp->connection_handle;
                        g_ancs_srv.status = ANCS_COMM_STATUS_READ_DATA_SOURCE;
                    }
                } else if (g_ancs_srv.status == ANCS_COMM_STATUS_READ_DATA_SOURCE) {
                    /* Notify app about the finish. */
                    ble_ancs_event_t notif_evt;
                    LOG_MSGID_I(ANCS_APP, "data source enabled = %d\n", 1, rsp->att_rsp->attribute_value[0]);
                    notif_evt.evt_type = BLE_ANCS_EVENT_CONNECTED;
                    notif_evt.connection_handle = rsp->connection_handle;
                    notif_evt.result = BT_STATUS_SUCCESS;
                    g_ancs_srv.evt_handler(&notif_evt);
                    g_ancs_srv.status = ANCS_COMM_STATUS_READY;
                } else {
                    LOG_MSGID_I(ANCS_APP, "BT_GATTC_READ_CHARC: action = %d\r\n", 1, g_ancs_srv.status);
                }
            } else {
                LOG_MSGID_I(ANCS_APP, "BT_GATTC_READ_CHARC:status = 0x%x\n", 1, status);
            }
            break;
        case BT_GATTC_WRITE_CHARC:
            /* GATTC write characteristic response event. */
            if (status == BT_STATUS_SUCCESS) {
                bt_gattc_write_rsp_t *rsp = (bt_gattc_write_rsp_t *)buff;
                ble_ancs_event_t notif_evt;
                notif_evt.evt_type = BLE_ANCS_EVENT_REQUEST_COMPLETED;
                notif_evt.result = BT_STATUS_SUCCESS;
                notif_evt.connection_handle = rsp->connection_handle;
                g_ancs_srv.evt_handler(&notif_evt);
            } else {
                LOG_MSGID_I(ANCS_APP, "BT_GATTC_WRITE_CHARC: status = %x\r\n", 1, status);
            }
            break;
        case BT_GATTC_CHARC_VALUE_NOTIFICATION:
            /* GATT notification event. */
            if (status == BT_STATUS_SUCCESS) {
                bt_gatt_handle_value_notification_t *data = (bt_gatt_handle_value_notification_t *)buff;
                ble_ancs_event_t ancs_evt;
                uint8_t i, m;
                for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
                    if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
                        break;
                    }
                }
                for (m = 0; m < BLE_ANCS_MAX_CHARC_NUMBER; m++) {
                    if (!memcmp(g_ancs_srv.service.charateristics[m].char_uuid.uuid.uuid, data_source_uuid, 16)) {
                        break;
                    }
                }
                if (data->att_rsp->handle == g_ancs_srv.service.charateristics[i].value_handle) {
                    /* Notification source data. */
                    ancs_evt.result = ble_ancs_parse_notification(&ancs_evt.data.notification, data->length - 3, data->att_rsp->attribute_value);
                    ancs_evt.connection_handle = data->connection_handle;
                    ancs_evt.evt_type = BLE_ANCS_EVENT_IOS_NOTIFICATION;
                    g_ancs_srv.evt_handler(&ancs_evt);
                } else if (data->att_rsp->handle == g_ancs_srv.service.charateristics[m].value_handle) {
#if 0
                    /* Parse attribute. */
                    bt_status_t result;
                    result = ble_ancs_parse_attributes(data->connection_handle, data->length - 3, data->att_rsp->attribute_value, &ancs_evt.data.attribute);
                    /* if status == BT_STATUS_PENDING, wait for another BT_GATTC_CHARC_VALUE_NOTIFICATION. */
                    if (BT_STATUS_SUCCESS == result || BT_STATUS_FAIL == result) {
                        ancs_evt.result = result;
                        ancs_evt.connection_handle = data->connection_handle;
                        if (ancs_evt.data.attribute.command_id == 0) {
                            ancs_evt.evt_type = BLE_ANCS_EVENT_NOTIFICATION_ATTRIBUTE;
                        } else if (ancs_evt.data.attribute.command_id == 1) {
                            ancs_evt.evt_type = BLE_ANCS_EVENT_APP_ATTRIBUTE;
                        }
                        LOG_MSGID_I(ANCS_APP, "Parse attribute done, result = %d, event = %d\r\n", 2, result, ancs_evt.evt_type);
                        g_ancs_srv.evt_handler(&ancs_evt);
                    }
#endif
                }
            } else {
                LOG_MSGID_I(ANCS_APP, "BT_GATTC_CHARC_VALUE_NOTIFICATION: status = %x\r\n", 1, status);
            }
            break;
        case BT_GAP_LE_DISCONNECT_IND:
            /* BLE disconnect event. */
            if (status == BT_STATUS_SUCCESS) {
                bt_gap_le_disconnect_ind_t *dis_ind = (bt_gap_le_disconnect_ind_t *)buff;
                ble_ancs_event_t ancs_evt;
                memset(&ancs_evt, 0, sizeof(ble_ancs_event_t));
                ancs_evt.result = BT_STATUS_SUCCESS;
                ancs_evt.connection_handle = dis_ind->connection_handle;
                ancs_evt.evt_type = BLE_ANCS_EVENT_DISCONNECTED;
                g_ancs_srv.evt_handler(&ancs_evt);
            } else {
                LOG_MSGID_I(ANCS_APP, "BT_GAP_LE_DISCONNECT_IND: status = %x\r\n", 1, status);
            }
            break;
        default:
            break;
    }
    return BT_STATUS_SUCCESS;
}


static void ble_ancs_gattc_callback(bt_gattc_discovery_event_t *event)
{
    bt_status_t result;
    switch (event->event_type) {
        case BT_GATTC_DISCOVERY_EVENT_COMPLETE: {
            /* GATT discovery complete. */
            uint8_t i;
            ble_ancs_charc_t charc[BLE_ANCS_MAX_CHARC_NUMBER];
            LOG_MSGID_I(ANCS_APP, "DISCOVER_COMPLETE: start = %d, end = %d, char_num = %d, handle = %d, %d, %d\n", 6, g_ancs_srv.service.start_handle,
                        g_ancs_srv.service.end_handle, g_ancs_srv.service.char_count_found, g_ancs_srv.service.charateristics[0].handle,
                        g_ancs_srv.service.charateristics[1].handle, g_ancs_srv.service.charateristics[2].handle);
            /* Parsing ancs characteristic. */
            for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
                if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, data_source_uuid, 16)) {
                    (charc + i)->uuid_type = BLE_ANCS_UUID_DATA_SOURCE;
                } else if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
                    (charc + i)->uuid_type = BLE_ANCS_UUID_NOTIFICATION_SOURCE;
                } else {
                    (charc + i)->uuid_type = BLE_ANCS_UUID_CONTROL_POINT;
                }
                (charc + i)->value_handle = g_ancs_srv.service.charateristics[i].value_handle;
                (charc + i)->desc_handle = g_ancs_srv.service.charateristics[i].descriptor[0].handle;
            }
            ble_ancs_init(event->conn_handle, charc);
            /* Start to read notification source descriptor. */
            for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
                LOG_MSGID_I(ANCS_APP, "read notification source, uuid_type = %d\n", 1, g_ancs_srv.service.charateristics[i].char_uuid.type);
                if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
                    break;
                }
            }
            if (i < BLE_ANCS_MAX_CHARC_NUMBER) {
                /* Read ANCS characteristic cccd status. */
                BT_GATTC_NEW_READ_CHARC_REQ(req, g_ancs_srv.service.charateristics[i].descriptor[0].handle);
                result = bt_gattc_read_charc(event->conn_handle, &req);
                if (!(result == BT_STATUS_SUCCESS || result == BT_STATUS_PENDING)) {
                    LOG_MSGID_I(ANCS_APP, "read notification source error (0x%x)\n", result);
                }
                g_ancs_srv.status = ANCS_COMM_STATUS_READ_NOTIF_SOURCE;
            }
        }
        break;
        default:
            break;
    }
}


void ble_ancs_start(ble_ancs_event_handler_t evt_handler)
{
    uint8_t i;
    /* Define ANCS service UUID. */
    ble_uuid_t ancs_srv_uuid = {
        .type = BLE_UUID_TYPE_128BIT,
        .uuid.uuid = {0xd0, 0x00, 0x2d, 0x12, 0x1e, 0x4b, 0x0f, 0xa4, 0x99, 0x4e, 0xce, 0xb5, 0x31, 0xf4, 0x05, 0x79}
    };
    memset(&g_ancs_srv, 0, sizeof(ble_ancs_srv_content_t));
    g_ancs_srv.evt_handler = evt_handler;
    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                          (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_GATT | MODULE_MASK_SYSTEM),
                                          (void *)ble_ancs_event_handler);
    /* Register ancs service discovery memory. */
    for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
        g_ancs_srv.charc[i].descriptor_count = 1;
        g_ancs_srv.charc[i].descriptor = &g_ancs_srv.descrp[i];
    }
    g_ancs_srv.service.characteristic_count = BLE_ANCS_MAX_CHARC_NUMBER;
    g_ancs_srv.service.charateristics = g_ancs_srv.charc;
    bt_gattc_discovery_service_register(&ancs_srv_uuid, &g_ancs_srv.service, ble_ancs_gattc_callback);
}


void ble_ancs_end(void)
{
    bt_callback_manager_deregister_callback(bt_callback_type_app_event, (void *)ble_ancs_event_handler);
    memset(&g_ancs_srv, 0, sizeof(ble_ancs_srv_content_t));
}


uint8_t ble_ancs_check_proxy_operation_outgoing(void)
{
    return (ancs_proxy_state & ANCS_PROXY_FLOW_ON);
}

bt_status_t ble_ancs_proxy_write_charc_cccd(bt_handle_t connection_handle, ble_ancs_proxy_charc_type_t type, uint16_t value)
{
    bt_status_t status = BT_STATUS_FAIL;
    bt_gattc_discovery_characteristic_t *charc_temp = NULL;
    /* Check ANCS proxy flow. */
    if (ble_ancs_check_proxy_operation_outgoing() == 1) {
        return status;
    }
    switch (type) {
        case BLE_ANCS_PROXY_CHARC_TYPE_NOTIFICATION_SOURCE: {
            /* ANCS notification source. */
            charc_temp = ble_ancs_proxy_find_charc_by_uuid(&noti_source_uuid);
            if (charc_temp == NULL) {
                return BT_STATUS_FAIL;
            }
            if (value == 0x0001) {
                status = ble_ancs_enable_notification_source(connection_handle);
            } else if (value == 0x0000) {
                status = ble_ancs_disable_notification_source(connection_handle);
            }
            break;
        }
        case BLE_ANCS_PROXY_CHARC_TYPE_DATA_SOURCE: {
            /* ANCS data source. */
            charc_temp = ble_ancs_proxy_find_charc_by_uuid(&data_source_uuid);
            if (charc_temp == NULL) {
                return BT_STATUS_FAIL;
            }
            if (value == 0x0001) {
                status = ble_ancs_enable_data_source(connection_handle);
            } else if (value == 0x0000) {
                status = ble_ancs_disable_data_source(connection_handle);
            }
            break;
        }
        default:
            LOG_MSGID_E(ANCS_APP, "wrong charc cccd type: %d\n", 1, type);
            break;
    }
    if (BT_STATUS_SUCCESS == status) {
        current_op_charc = charc_temp;
        ancs_proxy_state |= (ANCS_PROXY_FLOW_ON | ANCS_PROXY_ASYNC_RSP);
    } else {
        LOG_MSGID_E(ANCS_APP, "write charc cccd failed, error (0x%08x), type (%d), value (%d)", 3, status, type, value);
    }
    return status;
}

void ble_ancs_proxy_parse_command_notification_attributes(uint8_t *value, uint16_t length, ble_ancs_event_attribute_t *attr_parse)
{
    uint8_t len = 0;
    uint8_t att_num = 0;
    uint8_t *appid = 0;
    uint8_t appid_len = 0;
    memcpy(&(attr_parse->notification_uid), value + len, BLE_ANCS_NOTIFICATION_UID_LENGTH);
    len += 4;
    /* Parse notification attribute data. */
    while (len < length) {
        if (value[len] == 1 ||
            value[len] == 2 ||
            value[len] == 3) {
            attr_parse->attr_list[att_num].attribute_id =  value[len];
            len++;
            memcpy(&(attr_parse->attr_list[att_num].atrribute_len), value + len, 2);
            len += 2;
            LOG_MSGID_I(ANCS_APP, "parse notification attributes command, attribute id =%d, value len = %d\n", 2,
                        attr_parse->attr_list[att_num].attribute_id, attr_parse->attr_list[att_num].atrribute_len);
        } else {
            attr_parse->attr_list[att_num].attribute_id =  value[len];
            len++;
            LOG_MSGID_I(ANCS_APP, "parse notification attributes command , attribute id =%d\n", 1,
                        attr_parse->attr_list[att_num].attribute_id);
        }
        att_num++;
    }
    attr_parse->attr_num = att_num;
    LOG_MSGID_I(ANCS_APP, " parse notification attributes command, total number=%d\n", 1, attr_parse->attr_num);
}

void ble_ancs_proxy_parse_command_app_attributes(uint8_t *value, uint16_t length, ble_ancs_event_attribute_t *attr_parse,
                                                 uint8_t *app_id, uint16_t *app_len)
{
    uint8_t len = 0;
    uint8_t att_num = 0;
    uint8_t *appid = 0;
    uint16_t appid_len = 0;
    do {
        app_id[appid_len++] = value[len];
        len++;
    } while (value[len] != '\0');
    *app_len = appid_len;
    len++;
    /* Parse app attribute data. */
    while (len < length) {
        attr_parse->attr_list[att_num].attribute_id = value[len];
        len++;
        att_num++;
        LOG_MSGID_I(ANCS_APP, " parse app attributes command, attribute id =%d\n", 1, attr_parse->attr_list[att_num].attribute_id);
    }
    attr_parse->attr_num = att_num;
    LOG_MSGID_I(ANCS_APP, " parse app attributes command, total number=%d\n", 1, attr_parse->attr_num);
}


bt_status_t ble_ancs_proxy_write_charc(bt_handle_t connection_handle, ble_ancs_proxy_charc_type_t type, uint8_t *value, uint16_t length)
{
    ble_ancs_event_attribute_t attr_parse = {0};
    bt_status_t status = BT_STATUS_FAIL;
    bt_gattc_discovery_characteristic_t *charc_temp = NULL;
    if (ble_ancs_check_proxy_operation_outgoing() == 1) {
        return status;
    }
    if (BLE_ANCS_PROXY_CHARC_TYPE_CONTROL_POINT == type) {
        LOG_HEXDUMP_I(ANCS_APP, "write control point:", value, length);
        charc_temp = ble_ancs_proxy_find_charc_by_uuid(&ctrl_point_uuid);
        if (charc_temp == NULL) {
            return BT_STATUS_FAIL;
        }
        attr_parse.command_id = value[0];
        if (attr_parse.command_id == 0) {
            /* Get Notification Attributes. */
            ble_ancs_proxy_parse_command_notification_attributes(value + 1, length - 1, &attr_parse);
            status = ble_ancs_get_notification_attributes(connection_handle, attr_parse.notification_uid, &(attr_parse.attr_list), attr_parse.attr_num);
        } else if (attr_parse.command_id == 1) {
            /* Get app Attributes. */
            uint8_t app_id[50];
            uint16_t app_len = 0;
            ble_ancs_proxy_parse_command_app_attributes(value + 1, length - 1, &attr_parse, &app_id, &app_len);
            status = ble_ancs_get_app_attributes(connection_handle, &app_id, app_len, &(attr_parse.attr_list), attr_parse.attr_num);
        } else {
            LOG_MSGID_E(ANCS_APP, "wrong command id: %d\n", 1, attr_parse.command_id);
        }
    } else {
        LOG_MSGID_E(ANCS_APP, "wrong write charc type: %d\n", 1, type);
    }
    if (BT_STATUS_SUCCESS == status) {
        current_op_charc = charc_temp;
        ancs_proxy_state |= (ANCS_PROXY_FLOW_ON | ANCS_PROXY_ASYNC_RSP);
    } else {
        LOG_MSGID_E(ANCS_APP, "write control point  failed, error (0x%08x), type (%d)", 2, status, type);
    }
    return status;
}

/* ANCS test code. */
void ble_ancs_proxy_write_charc_test_nofity_attr()
{
    uint8_t value[50] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x0, 0x1, 0x0, 0x1, 0x2, 0x00, 0x01, 0x3, 0x0, 0x1, 0x4, 0x5};
    ble_ancs_proxy_write_charc(test_conn_handle, BLE_ANCS_PROXY_CHARC_TYPE_CONTROL_POINT, value, 17);
}
/* ANCS test code. */
void ble_ancs_proxy_write_charc_test_app_attr()
{
    uint8_t value[50] = {0x1, 0x01, 0x2, 0x3, 0x4, 0x05, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0x0, 0x0};
    ble_ancs_proxy_write_charc(test_conn_handle, BLE_ANCS_PROXY_CHARC_TYPE_CONTROL_POINT, value, 17);
}

bt_status_t ble_ancs_proxy_read_charc_cccd(bt_handle_t connection_handle, ble_ancs_proxy_charc_type_t type)
{
    uint8_t i;
    bt_status_t status = BT_STATUS_FAIL;
    if (ble_ancs_check_proxy_operation_outgoing() == 1) {
        return status;
    }
    if (type == BLE_ANCS_PROXY_CHARC_TYPE_NOTIFICATION_SOURCE) {
        /* ANCS read notification source status. */
        for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
            LOG_MSGID_I(ANCS_APP, "read notification source cccd, uuid_type = %d\n", 1, g_ancs_srv.service.charateristics[i].char_uuid.type);
            if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, noti_source_uuid, 16)) {
                break;
            }
        }
        if (i < BLE_ANCS_MAX_CHARC_NUMBER) {
            BT_GATTC_NEW_READ_CHARC_REQ(req, g_ancs_srv.service.charateristics[i].descriptor[0].handle);
            status = bt_gattc_read_charc(connection_handle, &req);
            g_ancs_srv.status = ANCS_COMM_STATUS_READ_NOTIF_SOURCE;
        }
    } else if (type == BLE_ANCS_PROXY_CHARC_TYPE_DATA_SOURCE) {
        /* ANCS read data source status. */
        for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
            if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, data_source_uuid, 16)) {
                break;
            }
        }
        if (i < BLE_ANCS_MAX_CHARC_NUMBER) {
            BT_GATTC_NEW_READ_CHARC_REQ(req, g_ancs_srv.service.charateristics[i].descriptor[0].handle);
            status = bt_gattc_read_charc(connection_handle, &req);
            g_ancs_srv.status = ANCS_COMM_STATUS_READ_DATA_SOURCE;
        }
    } else {
        LOG_MSGID_E(ANCS_APP, "wrong read charc cccde type: %d\n", 1, type);
    }
    if (BT_STATUS_SUCCESS == status) {
        ancs_proxy_state |= (ANCS_PROXY_FLOW_ON | ANCS_PROXY_ASYNC_RSP);
    } else {
        LOG_MSGID_E(ANCS_APP, "read charc cccd failed, error (0x%08x), type (%d)", 2, status, type);
    }
    return BT_STATUS_SUCCESS;
}

static bt_status_t ble_ancs_proxy_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    bt_status_t result = BT_STATUS_FAIL;
    if (!((ancs_proxy_state & ANCS_PROXY_FLOW_ON) || (ancs_proxy_state & ANCS_PROXY_ASYNC_RSP))) {
        return result;
    }
    switch (msg) {
        case BT_GATTC_READ_CHARC: {
            /* ANCS proxy gatt read characteristic response event. */
            if (status == BT_STATUS_SUCCESS) {
                bt_gattc_read_rsp_t *rsp = (bt_gattc_read_rsp_t *)buff;
                if (g_ancs_srv.status == ANCS_COMM_STATUS_READ_NOTIF_SOURCE) {
                    LOG_MSGID_I(ANCS_APP, "ble_ancs_proxy_event_handler notificaiton source enabled = %d\n", 1, rsp->att_rsp->attribute_value[0]);
                    g_ancs_srv.status = ANCS_COMM_STATUS_READY;
                } else if (g_ancs_srv.status == ANCS_COMM_STATUS_READ_DATA_SOURCE) {
                    LOG_MSGID_I(ANCS_APP, "data source enabled = %d\n", 1, rsp->att_rsp->attribute_value[0]);
                    g_ancs_srv.status = ANCS_COMM_STATUS_READY;
                }
            } else {
                LOG_MSGID_I(ANCS_APP, "ble_ancs_proxy_event_handler READ CHARC:status = 0x%x\n", 1, status);
            }
            ancs_proxy_state &= ~ANCS_PROXY_ASYNC_RSP;
            result = BT_STATUS_SUCCESS;
            break;
        }
        case BT_GATTC_WRITE_CHARC: {
            /* ANCS proxy gatt write characteristic response event. */
            LOG_MSGID_I(ANCS_APP, " ble_ancs_proxy_event_handler WRITE CHARC: status = %x\r\n", 1, status);
            bool proxy_result = false;
            if (!current_op_charc) {
                platform_assert("current op charc is NULL!", __FILE__, __LINE__);
            }
            if (!proxy_result) {
                LOG_MSGID_I(ANCS_APP, " proxy rsp for write failed.", 0);
            }
            current_op_charc = NULL;
            ancs_proxy_state &= ~ANCS_PROXY_ASYNC_RSP;
            result = BT_STATUS_SUCCESS;
            break;
        }
        default:
            break;
    }
    ancs_proxy_state &= ~ANCS_PROXY_FLOW_ON;
    return result;
}

bt_gattc_discovery_characteristic_t *ble_ancs_proxy_find_charc_by_uuid(uint8_t *uuid)
{
    uint8_t i;
    /* Poll the charc of the ANCS. */
    for (i = 0; i < BLE_ANCS_MAX_CHARC_NUMBER; i++) {
        LOG_MSGID_I(ANCS_APP, "find charc by uuid[%d], uuid_type = %d\n", 2, i, g_ancs_srv.service.charateristics[i].char_uuid.type);
        if (!memcmp(g_ancs_srv.service.charateristics[i].char_uuid.uuid.uuid, uuid, 16)) {
            break;
        }
    }
    //configASSERT(i < BLE_ANCS_MAX_CHARC_NUMBER);
    if (i >= BLE_ANCS_MAX_CHARC_NUMBER) {
        LOG_MSGID_I(ANCS_APP, "find no charc, ancs discovery failed\n", 0);
        return NULL;
    }
    LOG_MSGID_I(ANCS_APP, "find charc by uuid, charc handle = %d\n", 1, g_ancs_srv.service.charateristics[i].handle);
    return &(g_ancs_srv.service.charateristics[i]);
}
