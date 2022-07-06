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
 

#include "apps_bt_aws_mce_le_association.h"
#include "bt_gattc_discovery.h"
#include "bt_type.h"
#include "bt_callback_manager.h"
#include "bt_connection_manager.h"
#include "bt_aws_mce_srv.h"
#include "syslog.h"
#include "bt_timer_external.h"
#include "atci.h"
#include "string.h"
#include "hal_trng.h"
#include "ui_shell_manager.h"
#include "apps_events_event_group.h"
#include "bt_app_common.h"
#include "FreeRTOS.h"
#include "nvdm.h"
#include "nvdm_id_list.h"
#include "apps_aws_sync_event.h"
#include "multi_ble_adv_manager.h"
#include "bt_device_manager_internal.h"
#include "bt_hci.h"
#include "apps_events_bt_event.h"

bool g_ungroup_time_start = false;
bool g_wait_to_start_adv = false;
bool g_wait_to_start_scan = false;
bt_key_t g_app_association_aws_key = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
        0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x00};

bt_app_association_context g_app_cntx = {
    .agent_le_addr = {0xFF, {0}},
    .address = {0},
    .handle = 0xFFFF,
    .role = BT_AWS_MCE_ROLE_NONE,
    .mode = BT_AWS_MCE_SRV_MODE_NORMAL,
    .adv_enable = 0,
    .association_done = 0,
};

static bt_gattc_discovery_characteristic_t g_bt_aws_mce_le_ass_app_char[BT_AWS_MCE_LE_ASSOCIATION_MAX_NUMBER];
static bt_gattc_discovery_service_t g_bt_aws_mce_le_ass_app_srv;

static bool g_bt_aws_mce_le_ass_scan_enabled;

static const ble_uuid_t uuid_aws_le_association_service = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_SERVICE_UUID,
};

static const ble_uuid_t uuid_aws_le_association_agent_addr = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_AGENT_ADDR_CHAR_UUID,
};

static const ble_uuid_t uuid_aws_le_association_key = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_KEY_CHAR_UUID,
};

static const ble_uuid_t uuid_aws_le_association_client_addr = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_CLIENT_ADDR_CHAR_UUID,
};

static const ble_uuid_t uuid_aws_le_association_audio_lat = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_AUDIO_LAT_CHAR_UUID,
};

static const ble_uuid_t uuid_aws_le_association_voice_lat = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_VOICE_LAT_CHAR_UUID,
};

static const ble_uuid_t uuid_aws_le_association_number = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_NUMBER_CHAR_UUID,
};

static const ble_uuid_t uuid_aws_le_custom_read_data = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_CUSTOM_READ_CHAR_UUID,
};

static const ble_uuid_t uuid_aws_le_custom_write_data = {
    .type = BLE_UUID_TYPE_128BIT,
    .uuid.uuid = ASSOCIATION_CUSTOM_WRITE_CHAR_UUID,
};

bt_aws_mce_le_association_custom_data_t g_custom_data = {
    .data = NULL,
    .len = 0,
};

static void bt_aws_mce_le_association_init_register();

bt_aws_mce_le_association_pair_mode_t bt_aws_mce_le_association_get_char_type_by_uuid(ble_uuid_t *uuid)
{
    uint16_t len = sizeof(ble_uuid_t);
    bt_aws_mce_le_association_pair_mode_t type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_INVALID;

    if (memcmp(uuid, &uuid_aws_le_association_agent_addr, len) == 0) {
        type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_AGENT_ADDR;
    } else if (memcmp(uuid, &uuid_aws_le_association_key, len) == 0) {
        type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_SECRET_KEY;
    } else if (memcmp(uuid, &uuid_aws_le_association_client_addr, len) == 0) {
        type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_ADDR; 
    } else if (memcmp(uuid, &uuid_aws_le_association_audio_lat, len) == 0) {
        type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_AUDIO_LATENCY; 
    } else if (memcmp(uuid, &uuid_aws_le_association_voice_lat, len) == 0) {
        type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_VOICE_LATENCY;
    } else if (memcmp(uuid, &uuid_aws_le_association_number, len) == 0) {
        type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_CLIENT_NO; 
    } else if (memcmp(uuid, &uuid_aws_le_custom_read_data, len) == 0){
        type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_CUSTOM_READ_DATA;
    } else if (memcmp(uuid, &uuid_aws_le_custom_write_data, len) == 0) {
        type = BT_AWS_MCE_LE_ASSOCIATION_CHAR_CUSTOM_WRITE_DATA;
    } else {
        /* Ivnlid type */
    }

    return type;

}
static bt_status_t bt_aws_mce_le_association_app_compose_service(bt_aws_mce_le_association_service_t *service,
                                                     bt_gattc_discovery_service_t *origin_service)
{
    uint32_t i;
    if (service == NULL || origin_service == NULL) {
        return BT_STATUS_LE_ASSOCIATION_PARAMETER_ERR;
    }

    for (i = 0; i < origin_service->char_count_found; i++) {
        LOG_HEXDUMP_I(common, "uuid", &origin_service->charateristics[i].char_uuid, sizeof(ble_uuid_t));
        bt_aws_mce_le_association_pair_mode_t type = bt_aws_mce_le_association_get_char_type_by_uuid(&origin_service->charateristics[i].char_uuid);
        if (type == BT_AWS_MCE_LE_ASSOCIATION_CHAR_INVALID) {
            /* Error handle */
            return BT_STATUS_LE_ASSOCIATION_PARAMETER_ERR;
        }

        service->chara[i].type = type;
        service->chara[i].value_handle = origin_service->charateristics[i].value_handle;

    }

    service->char_count = origin_service->char_count_found;
    return BT_STATUS_SUCCESS;


}

static void bt_aws_mce_le_association_app_compose_client(bt_aws_mce_le_association_client_info_t *client_info)
{
    if (client_info == NULL) {
        return;
    }

    bt_bd_addr_t *addr = bt_connection_manager_device_local_info_get_local_address();
    memcpy(&client_info->address, addr, sizeof(bt_bd_addr_t));
    client_info->audio_latency = 0x96;
    client_info->voice_latency = 0x32;

}

static void bt_aws_mce_le_association_app_discovery_event_handler(bt_gattc_discovery_event_t *event)
{
    switch(event->event_type) {
        case BT_GATTC_DISCOVERY_EVENT_COMPLETE:
        {
            LOG_MSGID_I(common, "[app_speaker]BT_GATTC_DISCOVERY_EVENT_COMPLETE-end: role: %x", 1, g_app_cntx.role);
            if (g_app_cntx.role == BT_AWS_MCE_ROLE_AGENT)
            {
                break;
            }
            bt_gattc_discovery_service_t *service = event->params.discovered_db;
            if (service == NULL || service != &g_bt_aws_mce_le_ass_app_srv) {
                /* Error handle */
                LOG_MSGID_I(common, "[app_speaker]service not found", 0);
                
                break;
            }

            if (memcmp(&service->service_uuid, &uuid_aws_le_association_service, sizeof(ble_uuid_t)) != 0) {
                /* uuid is wrongly  */
                LOG_MSGID_I(common, "[app_speaker]service uuid wrongly", 0);
                break;
            }

            if (service->char_count_found > BT_AWS_MCE_LE_ASSOCIATION_MAX_NUMBER ||
                service->char_count_found == 0) {
                /* char count is wrongly*/
                
                LOG_MSGID_I(common, "[app_speaker]char count wrongly", 0);
                break;
            }
            
            bt_aws_mce_le_association_service_t aws_service;
            bt_status_t ret;

            ret = bt_aws_mce_le_association_app_compose_service(&aws_service, service);
            if (ret != BT_STATUS_SUCCESS) {
                /* Error handle */
                LOG_MSGID_I(common, "[app_speaker]compose service error", 0);
                break;
            }

            bt_aws_mce_le_association_pair_agent_req_t req;
            bt_aws_mce_le_association_client_info_t client_info;
            req.handle = event->conn_handle;
            req.mode = BT_AWS_MCE_LE_ASSOCIATION_PAIR_MODE_NUMBERED;
            req.service = &aws_service;
            req.client = &client_info;
            bt_aws_mce_le_association_app_compose_client(req.client);
            
            ret = bt_aws_mce_le_association_client_start_pairing(&req);
            LOG_MSGID_I(common, "[app_speaker]start pairing error %d",1, ret);
            if (ret != BT_STATUS_SUCCESS) {
                /* Error handle */
            }
            break;
        }
        case BT_GATTC_DISCOVERY_EVENT_FAIL:
        {
            //printf("%d\r\n", event->params.error);
            /* Didn't find target service */
            break;
        }
    }

}


static void bt_aws_mce_le_association_app_get_adv_data(bt_gap_le_set_ext_advertising_data_t* data)
{
    uint8_t data_context[31] = {0};

    /* uint16_t * of the 2nd parameters of bt_aws_mce_le_association_service_build_advertising_data
     * is just for extended adv. */
    bt_aws_mce_le_association_service_build_advertising_data((void *)data_context,
            (uint16_t *)&(data->data_length), g_app_cntx.mode);

    memcpy(data->data, data_context, data->data_length);

}

static void bt_aws_mce_le_association_app_get_adv_param(bt_hci_le_set_ext_advertising_parameters_t* adv_param)
{
    adv_param->advertising_event_properties = 0b10011;
    /* BTA-21229. */
    adv_param->primary_advertising_interval_min = 0xC0;
    adv_param->primary_advertising_interval_max = 0xC0;

    adv_param->primary_advertising_channel_map = 0b111;
    adv_param->own_address_type = BT_ADDR_RANDOM;
    memset(&(adv_param->peer_address), 0, sizeof(bt_addr_t));
    adv_param->advertising_filter_policy = 0;
    adv_param->advertising_tx_power = 0x00;
    adv_param->primary_advertising_phy = 0x01;
    adv_param->secondary_advertising_phy = 0x01;
    adv_param->secondary_advertising_max_skip = 0;
    adv_param->scan_request_notify_enable = 0;
}

static uint32_t bt_aws_mce_le_association_app_adv_info(multi_ble_adv_info_t *adv_info)
{
    if (adv_info == NULL) {
        return 0;
    }
    if (adv_info->adv_data) {
        bt_aws_mce_le_association_app_get_adv_data(adv_info->adv_data);
    }
    if (adv_info->adv_param) {
        bt_aws_mce_le_association_app_get_adv_param(adv_info->adv_param);
    }
    if (adv_info->scan_rsp) {
        adv_info->scan_rsp->data_length = 0;
    }
    return 0;
}

static void bt_aws_mce_le_association_app_timer_callback(uint32_t timer_id, uint32_t data)
{
    if (timer_id ==  BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_SCAN) {
        if (g_bt_aws_mce_le_ass_scan_enabled == true) {
            bt_hci_cmd_le_set_scan_enable_t enable = {
                .le_scan_enable = BT_HCI_DISABLE,
                .filter_duplicates = BT_HCI_DISABLE,
            };

            bt_status_t ret = bt_gap_le_set_scan((const bt_hci_cmd_le_set_scan_enable_t *)&enable, NULL);
//mark atci response
//#if 0
            atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));

            if (response != NULL) {
                uint8_t len = sizeof("Not Found the assocation adversting\r\n");
                strncpy((char*)(response->response_buf), "Not Found the assocation adversting\r\n", len);
                response->response_len = len + 1;
                response->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
                LOG_MSGID_I(common, "[app_speaker]app_timer_callback(len: %x)(ret =%x)", 2, len, ret);
                atci_send_response(response);
                vPortFree(response);
            } else {
                LOG_MSGID_E(common, "[app_speaker]app_timer_callback:malloc fail", 0);
            }
            bt_aws_mce_le_association_app_reset_contx();
//#endif
            //scan timeout callback: send event to broadcast app
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_LE_ASSOCIATION,
                    LE_ASSOCIATION_EVENT_SCAN_TIMEOUT, NULL, 0, NULL, 0);
            g_bt_aws_mce_le_ass_scan_enabled = false;
        }
    } else if (timer_id == BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_RESET) {
        bt_status_t ret = BT_STATUS_FAIL;
        bt_aws_mce_srv_mode_t mode;
        bt_aws_mce_srv_mode_switch_t param = {0};
        bt_aws_mce_srv_mode_t curr_mode = bt_aws_mce_srv_get_mode();
        bt_aws_mce_role_t curr_role = bt_connection_manager_device_local_info_get_aws_role();
        bt_bd_addr_t *pre_adr = bt_connection_manager_device_local_info_get_peer_aws_address();

        mode = g_app_cntx.mode;
        param.role = g_app_cntx.role;
        memcpy(&(param.addr), &(g_app_cntx.address), sizeof(bt_bd_addr_t));

        LOG_MSGID_I(common, "[app_speaker]time_reset:(cur_mode:%x),(cur_role: %x), (type: %x),(role: %x)", 4, curr_mode, curr_role, g_app_cntx.mode, g_app_cntx.role);

        if (curr_mode != g_app_cntx.mode || curr_role != g_app_cntx.role) {
            ret = bt_aws_mce_srv_switch_mode(mode, &param);
        } else if (curr_role == BT_AWS_MCE_ROLE_PARTNER || curr_role == BT_AWS_MCE_ROLE_CLINET) {
            if (memcmp(&(g_app_cntx.address), pre_adr, sizeof(bt_bd_addr_t))) {//change attach new agent
                LOG_MSGID_I(common, "[app_speaker]new agent addr: %x", 1, g_app_cntx.address);
                ret = bt_aws_mce_srv_switch_mode(mode, &param);
            } else {
                LOG_MSGID_I(common, "[app_speaker]old agent, reconnect", 0);
                //ret = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CONNECT, &(g_app_cntx.address));
                ret = BT_STATUS_SUCCESS;
            }
        } else { //agent
            LOG_MSGID_I(common, "[app_speaker]agent, no switch", 0);
            //ret = bt_sink_srv_send_action(BT_SINK_SRV_ACTION_CONNECT, NULL);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_AWS,
                    BT_AWS_MCE_SRV_EVENT_MODE_CHANGED_IND, NULL, 0, NULL, 0);
            ret = BT_STATUS_SUCCESS;
        }

        LOG_MSGID_I(common, "[app_speaker]timer reset: ret = (%x)", 1,ret);

        if (ret == BT_STATUS_SUCCESS)
        {
            bt_aws_mce_le_association_app_reset_contx();
        }
    } else if (timer_id == BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_UNGROUP) {
        g_ungroup_time_start = false;
        bt_status_t ret = bt_aws_mce_le_association_app_switch_to_single();
        LOG_MSGID_I(common, "[app_speaker]app_association, switch to single ret %x", 1, ret);
    } else if (timer_id == BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_ADV && g_app_cntx.association_done == 0) {
        //adv timeout, remove adv
        LOG_MSGID_I(common, "[app_speaker]BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_ADV", 0);
        multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_SPK_ASS, bt_aws_mce_le_association_app_adv_info);
        multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_SPK_ASS);
        bt_aws_mce_le_association_app_reset_contx();
        ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_LE_ASSOCIATION,
                LE_ASSOCIATION_EVENT_ADV_TIMEOUT, NULL, 0, NULL, 0);
    }
}

/*
extern uint8_t multi_adv_manager_get_adv_data_cb(multi_adv_instance_t instance, bt_gap_le_srv_adv_data_op_t op, void *data);
static uint8_t bt_aws_mce_le_association_app_adv_data_cb(bt_gap_le_srv_adv_data_op_t op, void *data)
{
    return multi_adv_manager_get_adv_data_cb(MULTI_ADV_INSTANCE_SPK_ASS, op, data);
}
*/

static bt_status_t bt_aws_mce_le_association_app_start_adv(void)
{
    if (multi_ble_adv_manager_add_ble_adv(MULTI_ADV_INSTANCE_SPK_ASS, bt_aws_mce_le_association_app_adv_info, 1)) {
        bt_timer_ext_start(BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_ADV, 0, 60000, bt_aws_mce_le_association_app_timer_callback);
        multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_SPK_ASS);
        return BT_STATUS_SUCCESS;
    } else {
        LOG_MSGID_I(common, "[app_speaker]app_association, add adv fail",0);
        return BT_STATUS_FAIL;
    }
}


static bt_status_t bt_aws_mce_le_association_app_check_and_do_adv(void)
{
    bt_status_t ret;
    LOG_MSGID_I(common, "[app_speaker]association check connection handle: %x", 1, g_app_cntx.handle);
    //if (0xFFFF == g_app_cntx.handle) {
    if(1) {
        return bt_aws_mce_le_association_app_start_adv();
    } else {
        bt_hci_cmd_disconnect_t discnn_para;
        discnn_para.reason = 0x13;
        discnn_para.connection_handle = g_app_cntx.handle;
        g_wait_to_start_adv = true;
        ret = bt_gap_le_disconnect(&discnn_para);
        LOG_MSGID_I(common, "[app_speaker]association check adv ble disconnect ret: %x", 1, ret);

        if(ret == BT_STATUS_CONNECTION_NOT_FOUND){
            g_app_cntx.handle = 0xFFFF;
            g_wait_to_start_adv = false;
            return bt_aws_mce_le_association_app_start_adv();
        }
    }
    return ret;
}


static bt_status_t bt_aws_mce_le_association_app_start_scan(void)
{
    bt_hci_cmd_le_set_scan_parameters_t param = {
        .le_scan_type = BT_HCI_SCAN_TYPE_PASSIVE,
        .le_scan_interval = 0x50,
        .le_scan_window = 0x18,
        .own_address_type = BT_HCI_SCAN_ADDR_RANDOM,
        .scanning_filter_policy = 0
    };

    bt_hci_cmd_le_set_scan_enable_t enable = {
        .le_scan_enable = BT_HCI_ENABLE,
        .filter_duplicates = BT_HCI_ENABLE,
    };

    LOG_MSGID_I(common, "[app_speaker]start scan", 0);
    bt_timer_ext_start(BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_SCAN, 0, 30000, bt_aws_mce_le_association_app_timer_callback);

    g_bt_aws_mce_le_ass_scan_enabled = true;
    bt_status_t ret = bt_gap_le_set_scan((const bt_hci_cmd_le_set_scan_enable_t *)&enable, (const bt_hci_cmd_le_set_scan_parameters_t *)&param);
    if (ret != BT_STATUS_SUCCESS) {
        g_bt_aws_mce_le_ass_scan_enabled = false;
    }

    return ret;
}

static bt_status_t bt_aws_mce_le_association_app_check_and_scan(void)
{
    bt_status_t ret;
    LOG_MSGID_I(common, "[app_speaker]association check connection handle: %x", 1, g_app_cntx.handle);

    if (0xFFFF == g_app_cntx.handle) {
        return bt_aws_mce_le_association_app_start_scan();
    } else {
        bt_hci_cmd_disconnect_t discnn_para;
        discnn_para.reason = 0x13;
        discnn_para.connection_handle = g_app_cntx.handle;
        g_wait_to_start_scan = true;
        ret = bt_gap_le_disconnect(&discnn_para);
        LOG_MSGID_I(common, "[app_speaker]association check scan ble disconnect ret: %x", 1, ret);

        if(ret == BT_STATUS_CONNECTION_NOT_FOUND){
            g_app_cntx.handle = 0xFFFF;
            g_wait_to_start_scan = false;
            return bt_aws_mce_le_association_app_start_scan();
        }
    }
    return ret;
}

static bt_status_t bt_aws_mce_le_association_app_ungroup()
{
    bt_status_t ret = BT_STATUS_FAIL;
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
    if (role == BT_AWS_MCE_ROLE_AGENT || role == BT_AWS_MCE_ROLE_PARTNER) {
        ret = apps_aws_sync_event_send(EVENT_GROUP_UI_SHELL_LE_ASSOCIATION, LE_ASSOCIATION_EVENT_AWS_MCE_UNGROUP);
        LOG_MSGID_I(common, "[app_speaker]sync ungroup: ret = (%x)", 1,ret);
    }
    if (!g_ungroup_time_start) {
        ret = bt_timer_ext_start(BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_UNGROUP, 0, 200, bt_aws_mce_le_association_app_timer_callback);
        if (ret == BT_STATUS_SUCCESS) {
            g_ungroup_time_start = true;
        }
    }
    return ret;
}


bt_status_t bt_aws_mce_le_association_app_event_handler(bt_msg_type_t msg, bt_status_t status, void *buffer)
{
    bt_status_t ret = BT_STATUS_SUCCESS;
    LOG_MSGID_I(common, "association_app_event_handler msg(%x), status(%x)",2, msg, status);
    switch (msg) {
        case BT_GAP_LE_ADVERTISING_REPORT_IND:
        {
            bt_gap_le_advertising_report_ind_t *ind = (bt_gap_le_advertising_report_ind_t *)buffer;
            if (ind->event_type == BT_GAP_LE_ADV_REPORT_EVT_TYPE_ADV_IND) {

                ret = bt_aws_mce_le_association_client_check_adv_data(ind->data, ind->data_length, g_app_cntx.mode);
                if (ret == BT_STATUS_SUCCESS) {

                    bt_hci_cmd_le_set_scan_enable_t enable = {
                        .le_scan_enable = BT_HCI_DISABLE,
                        .filter_duplicates = BT_HCI_DISABLE,
                    };

                    bt_hci_cmd_le_create_connection_t conn_para = {
                        .le_scan_interval = 0x30,
                        .le_scan_window = 0x20,
                        .initiator_filter_policy = 0x0,
                        .own_address_type = BT_ADDR_RANDOM,
                        .conn_interval_min = 0x20,
                        .conn_interval_max = 0x20,
                        .conn_latency = 0,
                        .supervision_timeout = 0x1F4,
                        .minimum_ce_length = 0x40,
                        .maximum_ce_length = 0x100,
                    };

                    memcpy(&conn_para.peer_address, &ind->address, sizeof(bt_addr_t));
                    memcpy(&g_app_cntx.agent_le_addr, &ind->address, sizeof(bt_addr_t));

                    if (g_bt_aws_mce_le_ass_scan_enabled == true) {
                        ret = bt_gap_le_set_scan((const bt_hci_cmd_le_set_scan_enable_t *)&enable, NULL);
                        g_bt_aws_mce_le_ass_scan_enabled = false;
                    }
                    LOG_MSGID_I(common, "[app_speaker]Scanner connect addr:(%02x:%02x:%02x:%02x:%02x:%02x)", 6,
                       conn_para.peer_address.addr[5],
                       conn_para.peer_address.addr[4],
                       conn_para.peer_address.addr[3],
                       conn_para.peer_address.addr[2],
                       conn_para.peer_address.addr[1],
                       conn_para.peer_address.addr[0]);
                    bt_gap_le_connect((const bt_hci_cmd_le_create_connection_t *)&conn_para);
                    bt_timer_ext_stop(BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_SCAN);
                    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_LE_ASSOCIATION,
                                    LE_ASSOCIATION_EVENT_TIMER_ASSOCIATION, NULL, 0, NULL, 60000);
                }

            }
            break;
        }
        case BT_POWER_OFF_CNF: {
            g_app_cntx.adv_enable = 0;
            break;
        }

        case BT_GAP_LE_ADVERTISING_SET_TERMINATED_IND:
        {
            bt_gap_le_advertising_set_terminated_ind_t *ind = (bt_gap_le_advertising_set_terminated_ind_t *)buffer;
            if (ind->handle == multi_ble_adv_manager_get_adv_handle_by_instance(MULTI_ADV_INSTANCE_SPK_ASS)) {
                LOG_MSGID_I(common, "[app_speaker]SPK adv teminated",0);
            }
            break;
        }

        case BT_GAP_LE_CONNECT_IND:
        {
            bt_gap_le_connection_ind_t *connection_ind = (bt_gap_le_connection_ind_t *)buffer;
            if (connection_ind) {
                LOG_MSGID_I(common, "[app_speaker]CONNECT_IND: conn_handle: %x", 1, connection_ind->connection_handle);
                g_app_cntx.handle = connection_ind->connection_handle;

                g_app_cntx.adv_enable = 0;
                LOG_MSGID_I(common, "[app_speaker]association ble conn peer:(%02x:%02x:%02x:%02x:%02x:%02x), local:(%02x:%02x:%02x:%02x:%02x:%02x)", 12,
                        connection_ind->peer_addr.addr[5],
                        connection_ind->peer_addr.addr[4],
                        connection_ind->peer_addr.addr[3],
                        connection_ind->peer_addr.addr[2],
                        connection_ind->peer_addr.addr[1],
                        connection_ind->peer_addr.addr[0],
                        connection_ind->local_addr.addr[5],
                        connection_ind->local_addr.addr[4],
                        connection_ind->local_addr.addr[3],
                        connection_ind->local_addr.addr[2],
                        connection_ind->local_addr.addr[1],
                        connection_ind->local_addr.addr[0]);
            }
            break;
        }
        case BT_GAP_LE_DISCONNECT_IND:
        {
            bt_gap_le_disconnect_ind_t *ind = (bt_gap_le_disconnect_ind_t *) buffer;
            if (ind == NULL)
            {
                LOG_MSGID_I(common, "[app_speaker]DISCONN_IND: fail", 0);
                break;
            }
            LOG_MSGID_I(common, "[app_speaker]DISCONN_IND: handle: %x, conn_handle: %x",2, g_app_cntx.handle, ind->connection_handle);
            if (g_app_cntx.handle == ind->connection_handle)
            {
                g_app_cntx.handle = 0xFFFF;
                LOG_MSGID_I(common, "[app_speaker]DISCONN_IND: association: %x, role: %x",2, g_app_cntx.association_done, g_app_cntx.role);

                if (g_wait_to_start_adv) {
                    g_wait_to_start_adv = false;
                    bt_aws_mce_le_association_app_start_adv();
                } else if(g_wait_to_start_scan) {
                    g_wait_to_start_scan = false;
                    bt_aws_mce_le_association_app_start_scan();
                } else if (g_app_cntx.association_done)
                {
                    g_app_cntx.association_done = 0;
                    //stop timer and remove adv
                    if (bt_timer_ext_stop(BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_ADV) == BT_TIMER_EXT_STATUS_SUCCESS) {
                        multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_SPK_ASS, bt_aws_mce_le_association_app_adv_info);
                        multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_SPK_ASS);
                    }
                    //send event to broadcast app
                    ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_LE_ASSOCIATION,
                            LE_ASSOCIATION_EVENT_ASSOCIATION_DONE, NULL, 0, NULL, 0);

                    bt_timer_ext_start(BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_RESET, 0, 1, bt_aws_mce_le_association_app_timer_callback);

                } else {
                    /* For 0x3e reason, retry connect. */
                    if (g_app_cntx.role == BT_AWS_MCE_ROLE_PARTNER || g_app_cntx.role == BT_AWS_MCE_ROLE_CLINET)
                    {
                        if (ind->reason == BT_HCI_STATUS_CONNECTION_FAILED_TO_BE_ESTABLISHED)
                        {
                            bt_hci_cmd_le_create_connection_t conn_para = {
                                .le_scan_interval = 0x30,
                                .le_scan_window = 0x20,
                                .initiator_filter_policy = 0x0,
                                .own_address_type = BT_ADDR_RANDOM,
                                .conn_interval_min = 0x20,
                                .conn_interval_max = 0x20,
                                .conn_latency = 0,
                                .supervision_timeout = 0x1F4,
                                .minimum_ce_length = 0x40,
                                .maximum_ce_length = 0x100,
                            };

                            memcpy(&conn_para.peer_address, &g_app_cntx.agent_le_addr, sizeof(bt_addr_t));

                            LOG_MSGID_I(common, "[app_speaker]Scanner connect retry addr:(%02x:%02x:%02x:%02x:%02x:%02x)", 6,
                               conn_para.peer_address.addr[5],
                               conn_para.peer_address.addr[4],
                               conn_para.peer_address.addr[3],
                               conn_para.peer_address.addr[2],
                               conn_para.peer_address.addr[1],
                               conn_para.peer_address.addr[0]);
                            bt_gap_le_connect((const bt_hci_cmd_le_create_connection_t *)&conn_para);
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
    }
    return ret;

}


void bt_aws_mce_le_association_app_action(bt_aws_mce_le_association_app_action_t action, void *buff)
{
    switch (action) {
        case BT_AWS_MCE_LE_ASSOCIATION_APP_START_PAIR:
        {
            /* Only one action till now */
            bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
            if (role == BT_AWS_MCE_ROLE_CLINET) {
                /* Start scan */
                bt_aws_mce_le_association_app_check_and_scan();
                
            } else if (role == BT_AWS_MCE_ROLE_AGENT) {
                /* Start advertising */
                
                bt_aws_mce_le_association_app_check_and_do_adv();

            }

            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_APP_START_SCAN:
        {
            bt_aws_mce_le_association_app_check_and_scan();
            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_APP_START_ADV:
        {
            bt_aws_mce_le_association_app_check_and_do_adv();
            break;
        }
        case BT_AWS_MCE_LE_ASSOCIATION_APP_UNGROUP:
        {
            bt_aws_mce_le_association_app_ungroup();
            break;
        }
        default:
            break;
    }

}

void bt_aws_mce_le_association_app_init()
{
    bt_aws_mce_le_association_agent_info_t info = 
    {
        .secret_key = {0},
    };
    bt_key_t *key = bt_device_manager_aws_local_info_get_key();
    if (key != NULL) {
        memcpy(&info.secret_key, key, 16);
    }
    bt_bd_addr_t *addr = bt_connection_manager_device_local_info_get_local_address();

    /* If previous mode is broadcast before reboot, set to single before BT power on. */
    bt_aws_mce_srv_mode_t curr_mode = bt_aws_mce_srv_get_mode();
    if(curr_mode == BT_AWS_MCE_SRV_MODE_BROADCAST) {
        LOG_MSGID_I(common, "[app_speaker] set to single before BT power on", 0);
        bt_device_manager_aws_local_info_store_mode(BT_AWS_MCE_SRV_MODE_SINGLE);
        bt_device_manager_aws_local_info_store_role(BT_AWS_MCE_ROLE_NONE);
        bt_bd_addr_t peer_addr = {0};
        bt_device_manager_aws_local_info_store_peer_address(&peer_addr);
    }

    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                              (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_GATT | MODULE_MASK_SYSTEM),
                                              (void *)bt_aws_mce_le_association_event_handler);

    bt_callback_manager_register_callback(bt_callback_type_app_event,
                                              (uint32_t)(MODULE_MASK_GAP | MODULE_MASK_GATT | MODULE_MASK_SYSTEM),
                                              (void *)bt_aws_mce_le_association_app_event_handler);
    bt_aws_mce_le_association_init_register();

    memcpy(&info.address, addr, sizeof(bt_bd_addr_t));

    bt_aws_mce_le_association_service_info_set(&info);

    bt_timer_ext_init();

}


static void bt_aws_mce_le_association_init_register()
{
    g_bt_aws_mce_le_ass_app_srv.characteristic_count = BT_AWS_MCE_LE_ASSOCIATION_MAX_NUMBER;
    g_bt_aws_mce_le_ass_app_srv.charateristics = &g_bt_aws_mce_le_ass_app_char[0];

    bt_gattc_discovery_service_register((ble_uuid_t *)&uuid_aws_le_association_service,&g_bt_aws_mce_le_ass_app_srv, bt_aws_mce_le_association_app_discovery_event_handler);
}


uint16_t bt_aws_mce_le_association_service_get_assign_number(void)
{
    static uint16_t i = 1;
    return i++;
}

void bt_aws_mce_le_association_event_callback(bt_aws_mce_le_association_event_t event, bt_status_t status, void *buffer, uint16_t length)
{
    LOG_MSGID_I(common, "callback error: event = %d, status:%d", 2, event, status);
    bt_status_t ret;

    if (status != BT_STATUS_SUCCESS) {
        return;
    }
    if (event == BT_AWS_MCE_LE_ASSOCIATION_EVENT_CLIENT_PAIR_IND) {//for agent
        bt_aws_mce_le_association_client_pair_ind_t *ind = (bt_aws_mce_le_association_client_pair_ind_t *) buffer;
        LOG_MSGID_I(common, "air ind, number(%d), audio_latency(%d), voice_lat(%d),handle:(%x) ",4, ind->number, ind->info.audio_latency, ind->info.voice_latency, ind->handle);
        LOG_MSGID_I(common, "address(%x-%x-%x-%x-%x-%x)",6,  ind->info.address[0], ind->info.address[1],ind->info.address[2],ind->info.address[3],ind->info.address[4],ind->info.address[5]);
        /* Report to sink and write to NVDM */
        g_app_cntx.association_done = 1;

    } else if (event == BT_AWS_MCE_LE_ASSOCIATION_EVENT_AGENT_PAIR_CNF) {// for client

            bt_aws_mce_le_association_agent_pair_cnf_t *ind = (bt_aws_mce_le_association_agent_pair_cnf_t *) buffer;
            LOG_MSGID_I(common, "pair cnf, number(%d), address(%x-%x-%x-%x-%x-%x)",7, ind->number, ind->info.address[0], ind->info.address[1],ind->info.address[2],ind->info.address[3],ind->info.address[4],ind->info.address[5]);
            LOG_HEXDUMP_I(common, "secret_key", ind->info.secret_key, 16);
            //bt_sink_srv_store_aws_peer_address(&ind->info.address);
            memcpy(&(g_app_cntx.address), &ind->info.address, sizeof(bt_bd_addr_t));
            bt_device_manager_aws_local_info_store_key(&ind->info.secret_key);
            g_app_cntx.association_done = 1;/*ble handle*/

            ret = bt_aws_mce_le_association_read_custom_data(ind->handle);
            LOG_MSGID_I(common, "pair cnf, read_custom_data: %d",1, ret);
    }
    if (event == BT_AWS_MCE_LE_ASSOCIATION_EVENT_READ_CUSTOM_DATA_CNF) {//for Client
        bt_aws_mce_le_association_read_custom_data_cnf_t *cnf =  (bt_aws_mce_le_association_read_custom_data_cnf_t *)buffer;
        if (cnf) {
            LOG_MSGID_I(common, "read_custom_data_cnf, len:%d, data[%x, %x, %x,%x]", 5, cnf->info.len, cnf->info.data[0],cnf->info.data[1],cnf->info.data[2],cnf->info.data[3]);
            uint8_t data[4] = {2,3,4,5};
            bt_aws_mce_le_association_write_custom_data(cnf->handle, data, 4);/*only For test write flow, User can choose write operation or not in this point*/
        }
    }
    if (event == BT_AWS_MCE_LE_ASSOCIATION_EVENT_WRITE_CUSTOM_DATA_CNF) {// for client
        LOG_MSGID_I(common, "write_custom_cnf, status = %x", 1, status);
        /*disconn*/
        bt_hci_cmd_disconnect_t discon;
        bt_aws_mce_le_association_write_custom_data_cnf_t *cnf = (bt_aws_mce_le_association_write_custom_data_cnf_t *)buffer;
        if (cnf == NULL) {
            return;
        }
        discon.reason = 0x13;
        discon.connection_handle = cnf->handle;
        if (g_app_cntx.handle == cnf->handle) {
            ret = bt_gap_le_disconnect(&discon);
            LOG_MSGID_I(common, "write_custom_cnf, handle = %x, status = %x, ret = %x, handle = %x", 4, status, cnf->handle, ret, g_app_cntx.handle);
        }


    } else if (event == BT_AWS_MCE_LE_ASSOCIATION_EVENT_WRITE_CUSTOM_DATA_IND) {//for server
        bt_aws_mce_le_association_write_custom_data_ind_t *ind =  (bt_aws_mce_le_association_write_custom_data_ind_t *)buffer;
        uint8_t temp_data[BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN];
        if (ind == NULL) { /*save custom data*/
            return;
        }
        if (ind->info.len >= BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN) {
            ind->info.len = BT_AWS_MCE_LE_ASSOCIATION_MAX_CUSTOM_DATA_LEN;
        }
        g_custom_data.len = ind->info.len;
        memcpy(temp_data, ind->info.data, g_custom_data.len);
        g_custom_data.data = temp_data;
        LOG_MSGID_I(common, "write_custom_ind, data[%x, %x, %x,%x]", 4, g_custom_data.data[0],g_custom_data.data[1],g_custom_data.data[2],g_custom_data.data[3]);
    }
}

bool bt_aws_mce_le_association_service_get_custom_data(bt_aws_mce_le_association_custom_data_t *data)
{
    /*for server*/
    uint8_t temp_data[4] = {6,1,6,5};
    memcpy(data->data, &temp_data, 4);
    data->len = 4;
    return true;
}

void bt_aws_mce_le_association_set_app_paramters(bt_aws_mce_role_t role, bt_aws_mce_srv_mode_t mode)
{
    g_app_cntx.role = role;
    g_app_cntx.mode = mode;
}

bt_app_association_context *bt_aws_mce_le_association_get_app_context()
{
    return &g_app_cntx;
}

bt_status_t bt_aws_mce_le_association_app_switch_to_single()
{
    bt_status_t ret = BT_STATUS_FAIL;

    bt_aws_mce_srv_mode_t mode = BT_AWS_MCE_SRV_MODE_SINGLE;
    bt_aws_mce_srv_mode_switch_t param = {0};
    param.role = BT_AWS_MCE_ROLE_NONE;

    bt_aws_mce_le_association_app_reset_contx();
    multi_ble_adv_manager_remove_ble_adv(MULTI_ADV_INSTANCE_SPK_ASS, bt_aws_mce_le_association_app_adv_info);
    multi_ble_adv_manager_notify_ble_adv_data_changed(MULTI_ADV_INSTANCE_SPK_ASS);
    if (g_bt_aws_mce_le_ass_scan_enabled == true) {
        bt_hci_cmd_le_set_scan_enable_t enable = {
            .le_scan_enable = BT_HCI_DISABLE,
            .filter_duplicates = BT_HCI_DISABLE,
        };

        bt_gap_le_set_scan((const bt_hci_cmd_le_set_scan_enable_t *)&enable, NULL);
        g_bt_aws_mce_le_ass_scan_enabled = false;
    }
    bt_timer_ext_stop(BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_SCAN);
    bt_timer_ext_stop(BT_AWS_MCE_LE_ASSOCIATION_APP_TIMER_ADV);
    ret = bt_aws_mce_srv_switch_mode(mode, &param);
    return ret;
}

void bt_aws_mce_le_association_app_reset_contx()
{
    memset(&g_app_cntx, 0x00, sizeof(bt_app_association_context));
    g_app_cntx.handle = 0xFFFF;
}

void bt_aws_mce_le_association_app_get_aws_key(bt_key_t *aws_key)
{
/*
    LOG_MSGID_I(common,"app_speaker get AWS key: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
            16, g_app_association_aws_key[15],g_app_association_aws_key[14],g_app_association_aws_key[13],g_app_association_aws_key[12],g_app_association_aws_key[11],
            g_app_association_aws_key[10],g_app_association_aws_key[9],g_app_association_aws_key[8],g_app_association_aws_key[7],g_app_association_aws_key[6],
            g_app_association_aws_key[5],g_app_association_aws_key[4],g_app_association_aws_key[3],g_app_association_aws_key[2],g_app_association_aws_key[1],g_app_association_aws_key[0]);
*/
    memcpy(aws_key, &g_app_association_aws_key, sizeof(bt_key_t));
}

void bt_aws_mce_le_association_app_set_aws_key(bt_key_t aws_key)
{
/*
    LOG_MSGID_I(common,"app_speaker set AWS key: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
            16, aws_key[15],aws_key[14],aws_key[13],aws_key[12],aws_key[11],
            aws_key[10],aws_key[9],aws_key[8],aws_key[7],aws_key[6],
            aws_key[5],aws_key[4],aws_key[3],aws_key[2],aws_key[1],aws_key[0]);
*/
    memcpy(&g_app_association_aws_key, aws_key, sizeof(bt_key_t));
}
