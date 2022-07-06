/* Copyright Statement:
 *
 * (C) 2021  Airoha Technology Corp. All rights reserved.
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

#ifdef AIR_LE_AUDIO_ENABLE
#include "app_le_audio.h"
#include "app_le_audio_aird_client.h"

#include "bt_sink_srv_le_cap_stream.h"

#include "bt_gattc.h"
#include "bt_gattc_discovery.h"

#include "bt_le_audio_msglog.h"

/**************************************************************************************************
* Define
**************************************************************************************************/
#define APP_LE_AUDIO_DISABLE_NOTIFICATION   0
#define APP_LE_AUDIO_ENABLE_NOTIFICATION    1
#define APP_LE_AUDIO_CCCD_VALUE_LEN         2
#define APP_LE_AUDIO_ATT_HDR_SIZE           3   /* |opcode (1 bytes) | att_handle (2 bytes) | */
#define APP_LE_AUDIO_ATT_VAULE_IDX          3   /* byte_0: opcode, byte_1~2: att_handle, byte_3:att_value */

#define APP_LE_AIRD_MAX_CHARC_NUMBER        2

#define APP_LE_AUDIO_AIRD_ACTION_QUEUE_MAX_NUM  5

typedef struct {
    bt_gattc_discovery_characteristic_t charc[APP_LE_AIRD_MAX_CHARC_NUMBER];
    bt_gattc_discovery_descriptor_t descrp[APP_LE_AIRD_MAX_CHARC_NUMBER];
} app_le_audio_aird_discovery_charc_t;

typedef struct {
    struct app_le_audio_aird_action_node_t *next;
    bt_gattc_write_without_rsp_req_t write_cmd;
    uint8_t buff[1];
} app_le_audio_aird_action_node_t;

/**************************************************************************************************
* Variable
**************************************************************************************************/
static app_le_audio_aird_discovery_charc_t g_le_audio_aird_charc_discovery;
static bt_gattc_discovery_service_t g_le_audio_aird_discovery;
static const uint8_t g_le_audio_aird_charc_uuid_rx[16] = {0x41, 0x45, 0x4C, 0x61, 0x68, 0x6F, 0x72, 0x69, 0x41, 0x30, 0xAB, 0x2D, 0x52, 0x41, 0x48, 0x43};
static const uint8_t g_le_audio_aird_charc_uuid_tx[16] = {0x41, 0x45, 0x4C, 0x61, 0x68, 0x6F, 0x72, 0x69, 0x41, 0x31, 0xAB, 0x2D, 0x52, 0x41, 0x48, 0x43};

static app_le_audio_aird_client_info_t g_le_audio_aird_client_info[APP_LE_AUDIO_MAX_LINK_NUM];
bool g_le_audio_aird_device_suspend_flag = false;
static app_le_audio_aird_client_callback_t g_le_audio_aird_client_callback = NULL;

/**************************************************************************************************
* Prototype
**************************************************************************************************/

/**************************************************************************************************
* Static function
**************************************************************************************************/
static bool app_le_audio_aird_client_add_action_node(app_le_audio_aird_client_info_t *p_info, app_le_audio_aird_action_node_t *new_node)
{
    if ((NULL == p_info) || (NULL == new_node)) {
        return false;
    }

    if (NULL == p_info->action_queue) {
        /* add node to queue head */
        p_info->action_queue = (uint8_t *)new_node;
        LE_AUDIO_MSGLOG_I("[APP][AIRD] add_action_node, head node:%x", 1, new_node);
        return true;
    }

    app_le_audio_aird_action_node_t *node = NULL;
    uint8_t i = 1;

    node = (app_le_audio_aird_action_node_t *)p_info->action_queue;
    while (NULL != node->next) {
        i++;
        node = node->next;
    }

    node->next = new_node;

    LE_AUDIO_MSGLOG_I("[APP][AIRD] add_action_node, node_num:%d node:%x", 2, i, new_node);

    if (APP_LE_AUDIO_AIRD_ACTION_QUEUE_MAX_NUM <= i) {
        node = (app_le_audio_aird_action_node_t *)p_info->action_queue;
        p_info->action_queue = (uint8_t *)node->next;
        LE_AUDIO_MSGLOG_I("[APP][AIRD] add_action_node, free 1st", 0);
        vPortFree(node);
    }

    return true;
}

static bt_status_t app_le_audio_aird_cilent_do_resend_action(bt_handle_t handle, app_le_audio_aird_client_info_t *p_info)
{
    app_le_audio_aird_action_node_t *node = NULL;

    if (NULL == p_info) {
        return BT_STATUS_FAIL;
    }

    if (NULL == p_info->action_queue) {
        //LE_AUDIO_MSGLOG_I("[APP][AIRD] do_resend_action, queue empty", 0);
        return BT_STATUS_SUCCESS;
    }

    while (NULL != (node = (app_le_audio_aird_action_node_t *)p_info->action_queue)) {
        bt_status_t ret = bt_gattc_write_without_rsp(handle, false, &(node->write_cmd));
        LE_AUDIO_MSGLOG_I("[APP][AIRD] do_resend_action, handle:%x ret:%x node:%x", 3, handle, ret, node);

        if (BT_STATUS_SUCCESS != ret) {
            return BT_STATUS_FAIL;
        }

        p_info->action_queue = (uint8_t *)node->next;
        vPortFree(node);
        node = NULL;
    }

    return BT_STATUS_SUCCESS;
}

static bt_status_t app_le_audio_aird_client_resend_action(bt_handle_t handle)
{
    uint8_t i;

    if (APP_LE_AUDIO_MAX_LINK_NUM == (i = app_le_audio_get_link_index(handle))) {
        //LE_AUDIO_MSGLOG_I("[APP][AIRD] resend_action, link not found:%x", 1, handle);
        return BT_STATUS_FAIL;
    }

    return app_le_audio_aird_cilent_do_resend_action(handle, &g_le_audio_aird_client_info[i]);
}

static bt_status_t app_le_audio_aird_client_set_cccd(bt_handle_t handle, uint16_t att_handle, uint16_t cccd)
{
    bt_status_t ret;
    uint8_t p_buf[5];

    BT_GATTC_NEW_WRITE_CHARC_REQ(req, p_buf, att_handle, (uint8_t *)&cccd, APP_LE_AUDIO_CCCD_VALUE_LEN);

    if (BT_STATUS_SUCCESS != (ret = bt_gattc_write_charc(handle, &req))) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] set_cccd, fail! handle:%x att_handle:%x ret:%x", 3, handle, att_handle, ret);
    }

    return ret;
}

static bt_status_t app_le_audio_aird_client_write(bt_handle_t handle, uint16_t att_handle, uint8_t *value, uint16_t len)
{
    uint8_t *p_buf;
    bt_status_t ret;

    if ((BT_HANDLE_INVALID == handle) || (BT_HANDLE_INVALID == att_handle)) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] write, invalid handle:%x att_handle:%x", 2, handle, att_handle);
        return BT_STATUS_FAIL;
    }

    p_buf = (uint8_t *)pvPortMalloc(APP_LE_AUDIO_ATT_HDR_SIZE + len);
    if (NULL == p_buf) {
        return BT_STATUS_OUT_OF_MEMORY;
    }

    BT_GATTC_NEW_WRITE_CHARC_REQ(req, p_buf, att_handle, value, len);

    ret = bt_gattc_write_charc(handle, &req);
    LE_AUDIO_MSGLOG_I("[APP][AIRD] write, ret:%x ", 1, ret);

    vPortFree(p_buf);

    return ret;
}

static bool app_le_audio_aird_client_start_pre_action(bt_handle_t handle, app_le_audio_aird_client_info_t *p_info)
{
    bool ret = false;

    if ((BT_HANDLE_INVALID == handle) || (NULL == p_info)) {
        return false;
    }

    if ((APP_LE_AUDIO_AIRD_CLIENT_STATE_SRV_DISCOVERY_COMPLETE > p_info->state) || (APP_LE_AUDIO_AIRD_CLIENT_STATE_READY <= p_info->state)) {
        return false;
    }

    LE_AUDIO_MSGLOG_I("[APP][AIRD] pre_action, handle:%x curr_state:%x", 2, handle, p_info->state);

    switch (p_info->state) {
        case APP_LE_AUDIO_AIRD_CLIENT_STATE_SRV_DISCOVERY_COMPLETE: {
            if (BT_HANDLE_INVALID != p_info->att_handle_tx_cccd) {
                p_info->state = APP_LE_AUDIO_AIRD_CLIENT_STATE_SET_CCCD_TX;
                if (BT_STATUS_SUCCESS == app_le_audio_aird_client_set_cccd(handle, p_info->att_handle_tx_cccd, APP_LE_AUDIO_ENABLE_NOTIFICATION)) {
                    ret = true;
                    break;
                }
            }
        }
        /*Pass through*/
        case APP_LE_AUDIO_AIRD_CLIENT_STATE_SET_CCCD_TX: {
            LE_AUDIO_MSGLOG_I("[APP][AIRD] set_attribute_cb, device_suspend_flag=%d", 1, g_le_audio_aird_device_suspend_flag);
            p_info->state = APP_LE_AUDIO_AIRD_CLIENT_STATE_READY;
            if (g_le_audio_aird_device_suspend_flag) {
                app_le_audio_aird_client_send_action(handle, APP_LE_AUDIO_AIRD_ACTION_SET_DEVICE_BUSY, NULL, 0);
            }
            bt_gattc_discovery_event_response();
            ret = true;
            break;
        }

        default:
            break;
    }

    LE_AUDIO_MSGLOG_I("[APP][AIRD] pre_action, handle:%x state:%x", 2, handle, p_info->state);

    return ret;
}

static void app_le_audio_aird_client_handle_write_rsp(bt_gattc_write_rsp_t *rsp)
{
    app_le_audio_aird_client_info_t *p_info;

    if (NULL == (p_info = app_le_audio_aird_client_get_info(rsp->connection_handle))) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] pre_action, link not exist", 0);
        return;
    }

    if (app_le_audio_aird_client_start_pre_action(rsp->connection_handle, p_info)) {
        return;
    }
}

static void app_le_audio_aird_client_handle_error_rsp(bt_status_t status, bt_gattc_error_rsp_t *rsp)
{
    app_le_audio_aird_client_info_t *p_info;

    if (NULL == (p_info = app_le_audio_aird_client_get_info(rsp->connection_handle))) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] pre_action, link not exist", 0);
        return;
    }

    if (app_le_audio_aird_client_start_pre_action(rsp->connection_handle, p_info)) {
        return;
    }
}

static void app_le_audio_aird_client_handle_notification(bt_gatt_handle_value_notification_t *noti)
{
    app_le_audio_aird_client_info_t *p_info;
    bt_att_handle_value_notification_t *att_rsp = noti->att_rsp;
    uint16_t data_len;

    if (NULL == (p_info = app_le_audio_aird_client_get_info(noti->connection_handle))) {
        return;
    }

    if (att_rsp->handle != p_info->att_handle_tx) {
        return;
    }

    data_len = noti->length;
    if (APP_LE_AUDIO_ATT_HDR_SIZE >= data_len) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] noti, invalid data_len:%x", 1, data_len);
        return;
    }

    data_len -= APP_LE_AUDIO_ATT_HDR_SIZE;
    LE_AUDIO_MSGLOG_I("[APP][AIRD] noti, handle:%x data_len:%x evt:%x", 3, noti->connection_handle, data_len, att_rsp->attribute_value[0]);

    switch (att_rsp->attribute_value[0]) {
        case APP_LE_AUDIO_AIRD_EVENT_MODE_INFO: {
            if (data_len == sizeof(app_le_audio_aird_event_mode_info_ind_t)) {
                app_le_audio_aird_event_mode_info_ind_t *ind = (app_le_audio_aird_event_mode_info_ind_t *)&att_rsp->attribute_value[0];
                p_info->mode = ind->mode;
                LE_AUDIO_MSGLOG_I("[APP][AIRD] current_mode:%x", 1, p_info->mode);
            }
            break;
        }
        case APP_LE_AUDIO_AIRD_EVENT_MIC_MUTE: {
            if (data_len == sizeof(app_le_audio_aird_event_mic_mute_ind_t)) {
                app_le_audio_aird_event_mic_mute_ind_t *ind = (app_le_audio_aird_event_mic_mute_ind_t *)&att_rsp->attribute_value[0];
                p_info->mic_mute = ind->mic_mute;
                LE_AUDIO_MSGLOG_I("[APP][AIRD] mic_mute:%x", 1, p_info->mic_mute);
            }
            break;
        }
    }
}

static void app_le_audio_aird_client_discovery_callback(bt_gattc_discovery_event_t *event)
{
    app_le_audio_aird_client_info_t *p_info;
    bt_gattc_discovery_characteristic_t *p_charc = NULL;
    uint8_t i;

    if ((NULL == event) ||
        ((BT_GATTC_DISCOVERY_EVENT_FAIL == event->event_type) && (!event->last_instance)) ||
        (NULL == (p_info = app_le_audio_aird_client_get_info(event->conn_handle)))) {

        if (g_le_audio_aird_client_callback) {
            app_le_audio_aird_client_event_srv_discovery_complete_t param = {0};
            param.handle = event != NULL ? event->conn_handle : BT_HANDLE_INVALID;
            param.status = BT_STATUS_FAIL;
            g_le_audio_aird_client_callback(APP_LE_AUDIO_AIRD_CLIENT_EVENT_SRV_DISCOVERY_COMPLETE, &param);
        }

        bt_gattc_discovery_event_response();
        return;
    }

    LE_AUDIO_MSGLOG_I("[APP][AIRD] aird_client_discovery_callback, handle:%x charc_num:%d", 2, event->conn_handle, g_le_audio_aird_discovery.char_count_found);


    if (BT_GATTC_DISCOVERY_EVENT_COMPLETE == event->event_type) {

        if (0 != (i = g_le_audio_aird_discovery.char_count_found)) {
            while (i > 0) {
                i--;
                p_charc = &g_le_audio_aird_discovery.charateristics[i];

                #if 1
                LE_AUDIO_MSGLOG_I("[APP][AIRD] aird_client_discovery_callback, uuid:%x %x %x %x %x %x %x %x", 8, p_charc->char_uuid.uuid.uuid[0],
                                                                                                        p_charc->char_uuid.uuid.uuid[1],
                                                                                                        p_charc->char_uuid.uuid.uuid[2],
                                                                                                        p_charc->char_uuid.uuid.uuid[3],
                                                                                                        p_charc->char_uuid.uuid.uuid[4],
                                                                                                        p_charc->char_uuid.uuid.uuid[5],
                                                                                                        p_charc->char_uuid.uuid.uuid[6],
                                                                                                        p_charc->char_uuid.uuid.uuid[7]);

                LE_AUDIO_MSGLOG_I("[APP][AIRD] aird_client_discovery_callback, uuid:%x %x %x %x %x %x %x %x", 8, p_charc->char_uuid.uuid.uuid[8],
                                                                                                        p_charc->char_uuid.uuid.uuid[9],
                                                                                                        p_charc->char_uuid.uuid.uuid[10],
                                                                                                        p_charc->char_uuid.uuid.uuid[11],
                                                                                                        p_charc->char_uuid.uuid.uuid[12],
                                                                                                        p_charc->char_uuid.uuid.uuid[13],
                                                                                                        p_charc->char_uuid.uuid.uuid[14],
                                                                                                        p_charc->char_uuid.uuid.uuid[15]);
                #endif

                if (0 == memcmp(&p_charc->char_uuid.uuid.uuid[0], &g_le_audio_aird_charc_uuid_rx[0], 16)) {
                    /* RX */
                    p_info->att_handle_rx = p_charc->value_handle;
                    LE_AUDIO_MSGLOG_I("[APP][AIRD] aird_client_discovery_callback, att_handle_rx:%d", 1, p_info->att_handle_rx);

                } else if (0 == memcmp(&p_charc->char_uuid.uuid.uuid[0], &g_le_audio_aird_charc_uuid_tx[0], 16)) {
                    /* TX */
                    p_info->att_handle_tx = p_charc->value_handle;
                    p_info->att_handle_tx_cccd = p_charc->descriptor[0].handle;
                    LE_AUDIO_MSGLOG_I("[APP][AIRD] aird_client_discovery_callback, att_handle_tx:%d att_handle_tx_cccd:%x", 2, p_info->att_handle_rx, p_info->att_handle_tx_cccd);
                }
            }

            p_info->state = APP_LE_AUDIO_AIRD_CLIENT_STATE_SRV_DISCOVERY_COMPLETE;

            app_le_audio_aird_client_start_pre_action(event->conn_handle, p_info);

            if (g_le_audio_aird_client_callback) {
                app_le_audio_aird_client_event_srv_discovery_complete_t param = {0};
                param.handle = event->conn_handle;
                param.status = BT_STATUS_SUCCESS;
                g_le_audio_aird_client_callback(APP_LE_AUDIO_AIRD_CLIENT_EVENT_SRV_DISCOVERY_COMPLETE, &param);
            }

            return;
        }
    }

    if (g_le_audio_aird_client_callback) {
        app_le_audio_aird_client_event_srv_discovery_complete_t param = {0};
        param.handle = event->conn_handle;
        param.status = BT_STATUS_FAIL;
        g_le_audio_aird_client_callback(APP_LE_AUDIO_AIRD_CLIENT_EVENT_SRV_DISCOVERY_COMPLETE, &param);
    }

    bt_gattc_discovery_event_response();
}

/**************************************************************************************************
* Public function
**************************************************************************************************/
void app_le_audio_aird_client_event_handler(bt_msg_type_t msg, bt_status_t status, void *buff)
{
    if (NULL == buff) {
        return;
    }

    //LE_AUDIO_MSGLOG_I("[APP][AIRD] event_handler, msg:%x", 1, msg);

    switch (msg) {
        case BT_GATTC_DISCOVER_PRIMARY_SERVICE: {
            app_le_audio_aird_client_resend_action(((bt_gattc_read_by_group_type_rsp_t *)buff)->connection_handle);
            break;
        }
        case BT_GATTC_DISCOVER_PRIMARY_SERVICE_BY_UUID: {
            app_le_audio_aird_client_resend_action(((bt_gattc_find_by_type_value_rsp_t *)buff)->connection_handle);
            break;
        }
        case BT_GATTC_FIND_INCLUDED_SERVICES:
        case BT_GATTC_DISCOVER_CHARC:
        case BT_GATTC_READ_USING_CHARC_UUID: {
            app_le_audio_aird_client_resend_action(((bt_gattc_read_by_type_rsp_t *)buff)->connection_handle);
            break;
        }
        case BT_GATTC_DISCOVER_CHARC_DESCRIPTOR: {
            app_le_audio_aird_client_resend_action(((bt_gattc_find_info_rsp_t *)buff)->connection_handle);
            break;
        }
        case BT_GATTC_READ_CHARC:
        case BT_GATTC_READ_LONG_CHARC: {
            app_le_audio_aird_client_resend_action(((bt_gattc_read_rsp_t *)buff)->connection_handle);
            break;
        }
        case BT_GATTC_READ_MULTI_CHARC_VALUES: {
            app_le_audio_aird_client_resend_action(((bt_gattc_read_multiple_rsp_t *)buff)->connection_handle);
            break;
        }
        case BT_GATTC_WRITE_LONG_CHARC:
        case BT_GATTC_WRITE_CHARC: {
            if (status == BT_STATUS_SUCCESS) {
                app_le_audio_aird_client_handle_write_rsp((bt_gattc_write_rsp_t *)buff);
                app_le_audio_aird_client_resend_action(((bt_gattc_write_rsp_t *)buff)->connection_handle);
            } else {
                app_le_audio_aird_client_handle_error_rsp(status, (bt_gattc_error_rsp_t*)buff);
                app_le_audio_aird_client_resend_action(((bt_gattc_error_rsp_t *)buff)->connection_handle);
            }
            break;
        }
        case BT_GATTC_RELIABLE_WRITE_CHARC: {
            app_le_audio_aird_client_resend_action(((bt_gattc_execute_write_rsp_t *)buff)->connection_handle);
            break;
        }
        case BT_GATTC_CHARC_VALUE_NOTIFICATION: {
            app_le_audio_aird_client_handle_notification((bt_gatt_handle_value_notification_t *)buff);
            break;
        }
        default:
            break;
    }
}

void app_le_audio_aird_client_reset_info(uint8_t link_idx)
{
    if (APP_LE_AUDIO_MAX_LINK_NUM <= link_idx) {
        return;
    }

    app_le_audio_aird_action_node_t *node = NULL;
    while (NULL != (node = (app_le_audio_aird_action_node_t *)g_le_audio_aird_client_info[link_idx].action_queue)) {
        g_le_audio_aird_client_info[link_idx].action_queue = (uint8_t *)node->next;
        vPortFree(node);
        node = NULL;
    }

    memset(&g_le_audio_aird_client_info[link_idx], 0, sizeof(app_le_audio_aird_client_info_t));

    g_le_audio_aird_client_info[link_idx].att_handle_rx = BT_HANDLE_INVALID;
    g_le_audio_aird_client_info[link_idx].att_handle_tx = BT_HANDLE_INVALID;
    g_le_audio_aird_client_info[link_idx].att_handle_tx_cccd = BT_HANDLE_INVALID;
    g_le_audio_aird_client_info[link_idx].state = APP_LE_AUDIO_AIRD_CLIENT_STATE_IDLE;
}

void app_le_audio_aird_client_init(void)
{
    uint8_t i;

    /*i = APP_LE_AUDIO_MAX_LINK_NUM;

    while (i > 0) {
        i--;
        app_le_audio_aird_client_reset_info(i);
    }*/

    /* Register AIRD discovery */
    i = APP_LE_AIRD_MAX_CHARC_NUMBER;
    memset(&g_le_audio_aird_charc_discovery, 0, sizeof(app_le_audio_aird_discovery_charc_t));

    while (i > 0) {
        i--;
        g_le_audio_aird_charc_discovery.charc[i].descriptor_count = 1;
        g_le_audio_aird_charc_discovery.charc[i].descriptor = &g_le_audio_aird_charc_discovery.descrp[i];
    }

    ble_uuid_t aird_srv_uuid = {
        .type = BLE_UUID_TYPE_128BIT,
        .uuid.uuid = {0x41, 0x45, 0x4C, 0x61, 0x68, 0x6F, 0x72, 0x69, 0x41, 0x01, 0xAB, 0x2D, 0x4D, 0x49, 0x52, 0x50}
    };
    memset(&g_le_audio_aird_discovery, 0, sizeof(bt_gattc_discovery_service_t));
    g_le_audio_aird_discovery.charateristics = g_le_audio_aird_charc_discovery.charc;
    g_le_audio_aird_discovery.characteristic_count = APP_LE_AIRD_MAX_CHARC_NUMBER;
    bt_gattc_discovery_service_register(&aird_srv_uuid, &g_le_audio_aird_discovery, app_le_audio_aird_client_discovery_callback);

}

app_le_audio_aird_client_info_t *app_le_audio_aird_client_get_info(bt_handle_t handle)
{
    uint8_t link_idx;

    if (APP_LE_AUDIO_MAX_LINK_NUM == (link_idx = app_le_audio_get_link_index(handle))) {
        return NULL;
    }

    return &g_le_audio_aird_client_info[link_idx];
}

bool app_le_audio_aird_client_is_cis_streaming(bt_handle_t *acl_handle, app_le_audio_aird_mode_t *mode)
{
    app_le_audio_aird_client_info_t *p_info;
    bt_handle_t handle = BT_HANDLE_INVALID;

    *acl_handle = BT_HANDLE_INVALID;
    *mode = APP_LE_AUDIO_AIRD_MODE_NORMOL;

    if (BT_HANDLE_INVALID == (handle = bt_sink_srv_cap_stream_get_ble_link_with_cis_established())) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] is_cis_streaming, NO", 0);
        return false;
    }

    if (NULL == (p_info = app_le_audio_aird_client_get_info(handle))) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] is_cis_streaming, handle:%x not support aird", 1, handle);
        return false;
    }

    LE_AUDIO_MSGLOG_I("[APP][AIRD] is_cis_streaming, handle:%x support aird mode:%x", 2, handle, p_info->mode);

    *acl_handle = handle;
    *mode = p_info->mode;
    return true;
}

bt_status_t app_le_audio_aird_client_send_action(bt_handle_t handle, app_le_audio_aird_action_t action, void *param, uint32_t param_len)
{
    app_le_audio_aird_client_info_t *p_info;
    app_le_audio_aird_action_node_t *node = NULL;
    bt_status_t ret;
    uint16_t att_value_len;

    if (APP_LE_AUDIO_AIRD_ACTION_MAX <= action) {
        return BT_STATUS_FAIL;
    }

    if (NULL == (p_info = app_le_audio_aird_client_get_info(handle))) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] send_action, link not exist", 0);
        return BT_STATUS_FAIL;
    }

    att_value_len = (param_len + 1);    /* att_value: |action (1 byte) | param (param_len bytes) | */

    if (NULL == (node = (app_le_audio_aird_action_node_t *)pvPortMalloc(sizeof(app_le_audio_aird_action_node_t) + (APP_LE_AUDIO_ATT_HDR_SIZE + att_value_len)))) {
        LE_AUDIO_MSGLOG_E("[APP][AIRD] send_action, OOM", 0);
        return BT_STATUS_OUT_OF_MEMORY;
    }

    node->next = NULL;
    node->write_cmd.attribute_value_length = att_value_len;
    node->write_cmd.att_req = (bt_att_write_command_t *)(node->buff);
    node->write_cmd.att_req->opcode = BT_ATT_OPCODE_WRITE_COMMAND;
    node->write_cmd.att_req->attribute_handle = p_info->att_handle_rx;
    node->buff[APP_LE_AUDIO_ATT_VAULE_IDX] = action;
    if ((NULL != param) && (0 != param_len)) {
        memcpy(&node->buff[APP_LE_AUDIO_ATT_VAULE_IDX+1], param, param_len);
    }

    if (BT_STATUS_SUCCESS == app_le_audio_aird_cilent_do_resend_action(handle, p_info)) {
        ret = bt_gattc_write_without_rsp(handle, false, &(node->write_cmd));
        LE_AUDIO_MSGLOG_I("[APP][AIRD] send_action(%d), handle:%x ret:%x node:%x", 4, action, handle, ret, node);
        if (BT_STATUS_SUCCESS == ret) {
            vPortFree(node);
            return ret;
        }
    }

    if (!app_le_audio_aird_client_add_action_node(p_info, node)) {
        LE_AUDIO_MSGLOG_I("[APP][AIRD] send_action(%d), add node fail", 1, action);
        vPortFree(node);
    }

    return ret;
}

/*
void app_le_audio_aird_client_notify_dongle_media_state(bool suspend, bt_handle_t handle,
                                                        uint8_t *param, uint32_t param_len)
{
    LE_AUDIO_MSGLOG_I("[APP][AIRD][LEA] notify_dongle_media_state, suspend=%d handle=0x%04X",
                      2, suspend, handle);
    g_le_audio_aird_device_suspend_flag = suspend;

    if (handle == BT_HANDLE_INVALID) {
        LE_AUDIO_MSGLOG_E("[APP][AIRD][LEA] notify_dongle_media_state, invalid handle", 0);
        return;
    }

    uint8_t index = app_le_audio_get_link_index(handle);
    if (index >= APP_LE_AUDIO_MAX_LINK_NUM) {
        LE_AUDIO_MSGLOG_E("[APP][AIRD][LEA] notify_dongle_media_state, invalid index=%d", 1, index);
        return;
    }

    app_le_audio_aird_client_state_t state = g_le_audio_aird_client_info[index].state;
    if (state != APP_LE_AUDIO_AIRD_CLIENT_STATE_READY) {
        LE_AUDIO_MSGLOG_E("[APP][AIRD][LEA] notify_dongle_media_state, not ready state %d", 1, state);
        return;
    }

    if (suspend) {
        app_le_audio_aird_client_send_action(handle, APP_LE_AUDIO_AIRD_ACTION_SET_DEVICE_BUSY, NULL, 0);
    } else {
        app_le_audio_aird_client_send_action(handle, APP_LE_AUDIO_AIRD_ACTION_RESET_DEVICE_BUSY, NULL, 0);
    }
}*/

void app_le_audio_aird_client_infom_device_busy(bool set_device_busy)
{
    bt_handle_t handle;
    uint8_t link_idx;

    g_le_audio_aird_device_suspend_flag = set_device_busy;

    LE_AUDIO_MSGLOG_I("[APP][AIRD] infom_device_busy:%x", 1, set_device_busy);
    for (link_idx = 0; link_idx < APP_LE_AUDIO_MAX_LINK_NUM; link_idx++) {
        if ((BT_HANDLE_INVALID != (handle = app_le_audio_get_handle(link_idx))) &&
            (APP_LE_AUDIO_AIRD_CLIENT_STATE_READY == g_le_audio_aird_client_info[link_idx].state)) {
            if (set_device_busy) {
                app_le_audio_aird_client_send_action(handle, APP_LE_AUDIO_AIRD_ACTION_SET_DEVICE_BUSY, NULL, 0);
            } else {
                app_le_audio_aird_client_send_action(handle, APP_LE_AUDIO_AIRD_ACTION_RESET_DEVICE_BUSY, NULL, 0);
            }
        }
    }

}

bt_status_t app_le_audio_aird_client_register_callback(app_le_audio_aird_client_callback_t callback)
{
    if (NULL != g_le_audio_aird_client_callback) {
        return BT_STATUS_FAIL;
    }

    g_le_audio_aird_client_callback = callback;
    return BT_STATUS_SUCCESS;
}

#endif  /* AIR_LE_AUDIO_ENABLE */

