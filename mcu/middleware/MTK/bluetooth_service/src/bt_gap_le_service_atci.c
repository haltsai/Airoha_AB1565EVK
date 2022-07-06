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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "FreeRTOS.h"
#include "nvdm.h"
#include "atci.h"
#include "bt_gap_le_service.h"
#include "bt_gap_le_service_utils.h"


#define BLESRV_STRPARAM(s)              s, strlen(s)
#define BLESRV_STRCPYN(dest, source)    strncpy(dest, source, strlen(source)+1);

//static functions.
static atci_status_t bt_gap_le_srv_atci_it_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_cmd_hdlr_item_t g_ble_srv_atci_cmd[] = {
    {
        .command_head = "AT+BLESRVIT",    /* INTERNAL USE, IT TEST */
        .command_hdlr = bt_gap_le_srv_atci_it_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
};

#define BLE_GAP_SERVICE_TEST_UUID                \
        {0xAA, 0xBB, 0xCC, 0x61, 0x68, 0x6F, 0x72, 0x69,               \
         0x41, 0x03, 0xAB, 0x2D, 0x4D, 0x49, 0x52, 0xFF}
static bt_gap_le_srv_uuid_t uuid16 = {.uuid16 = 0x180A};
static bt_gap_le_srv_uuid_t uuid128 = {.uuid = BLE_GAP_SERVICE_TEST_UUID};
static uint8_t g_instance = 0;
static bt_gap_le_srv_adv_fields_t adv_fields = {
    .flags = BT_GAP_LE_AD_FLAG_BR_EDR_NOT_SUPPORTED | BT_GAP_LE_AD_FLAG_GENERAL_DISCOVERABLE,
    .uuids16 = &uuid16,
    .num_uuids16 = 1,
    .uuids16_is_complete = true,
    .name = (uint8_t *)"UPDATE_EXT_ADV",
    .name_len = 14,
    .name_is_complete = true,
};

static bt_gap_le_srv_adv_fields_t scanrsp_fields = {
    .uuids128 = &uuid128,
    .num_uuids128 = 1,
    .uuids128_is_complete = true,
    .appearance = 0x1234,
    .appearance_is_present = true,
};

static uint8_t bt_gap_le_srv_get_adv_data_func(bt_gap_le_srv_adv_data_op_t op, void *data)
{
    uint8_t gen_result = 0;
    uint8_t instance = g_instance;
    bt_hci_le_set_ext_advertising_parameters_t ext_adv_para = {
        .advertising_event_properties = BT_HCI_ADV_EVT_PROPERTIES_MASK_SCANNABLE | BT_HCI_ADV_EVT_PROPERTIES_MASK_LEGACY_PDU | BT_HCI_ADV_EVT_PROPERTIES_MASK_CONNECTABLE,
        .primary_advertising_interval_min = 0x00100,
        .primary_advertising_interval_max = 0x00400,
        .primary_advertising_channel_map = 0x07,
        .own_address_type = BT_ADDR_RANDOM,
        .peer_address = {
            .type = BT_ADDR_PUBLIC,
            .addr = {0},
         },
        .advertising_filter_policy = BT_HCI_ADV_FILTER_ACCEPT_SCAN_CONNECT_FROM_ALL,
        .advertising_tx_power = 0x0,
        .primary_advertising_phy = BT_HCI_LE_ADV_PHY_1M,
        .secondary_advertising_max_skip = 0x00,
        .secondary_advertising_phy = BT_HCI_LE_ADV_PHY_1M,
        .advertisng_SID = 0x00,
        .scan_request_notify_enable = BT_HCI_DISABLE
    };
    
    
    uint8_t adv_data[15] = {0x0E, 0x09, 'L', 'E', '_', 'E', 'X', 'T', '_', 'A', 'D', 'V', '_'};
    uint8_t scan_rsp_data[21] = {0x14, 0x09, 'L', 'E', '_', 'E', 'X', 'T', '_', 'S', 'C', 'A', 'N', '_', 'R', 'E', 'S', 'P', '_'};
    adv_data[13] = ((instance >> 4) < 0xA ? (instance >> 4) + 48 : (instance >> 4) + 55);
    adv_data[14] = ((instance & 0xF) < 0xA ? (instance & 0xF) + 48 : (instance & 0xF) + 55);
    scan_rsp_data[19] = adv_data[13];
    scan_rsp_data[20] = adv_data[14];
    
    ext_adv_para.advertisng_SID = instance;

    switch (op) {
        case BT_GAP_LE_SRV_ADV_DATA_OP_CONFIG: {
            bt_gap_le_srv_adv_config_info_t *info = (bt_gap_le_srv_adv_config_info_t *)data;  
            bt_gap_le_srv_memcpy(&info->adv_param, &ext_adv_para, sizeof(bt_hci_le_set_ext_advertising_parameters_t)); 

            bt_gap_le_srv_memcpy(info->adv_data.data, &adv_data, 15);
            info->adv_data.data_length = 15;
            bt_gap_le_srv_memcpy(info->scan_rsp.data, &scan_rsp_data, 21);
            info->scan_rsp.data_length = 21;
            gen_result = BT_GAP_LE_ADV_PARAM_GEN | BT_GAP_LE_ADV_DATA_GEN | BT_GAP_LE_ADV_SCAN_RSP_GEN;
        }
            break;

        case BT_GAP_LE_SRV_ADV_DATA_OP_UPDATE: {
            bt_gap_le_srv_adv_update_info_t *info = (bt_gap_le_srv_adv_update_info_t *)data;
            bt_gap_le_srv_generate_adv_data(&adv_fields, info->adv_data.data,(uint32_t *) &(info->adv_data.data_length)); 
            bt_gap_le_srv_generate_adv_data(&scanrsp_fields, info->scan_rsp.data, (uint32_t *)&(info->scan_rsp.data_length));
            gen_result = BT_GAP_LE_ADV_DATA_GEN | BT_GAP_LE_ADV_SCAN_RSP_GEN;
        }
            break;

        default:
            break;


    }
    return gen_result;
}

void bt_gap_le_srv_dump_adv_complete(bt_gap_le_srv_adv_complete_t *adv_cp)
{
    bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]adv cp:[instance:%d, evt:%d, result:%d, adv_events:%d, tx_p:%d, handle:0x%4x]", 6,
                    adv_cp->instance, adv_cp->adv_evt, adv_cp->result,
                    adv_cp->num_ext_adv_events, adv_cp->selected_tx_power,
                    adv_cp->conn_handle);
}

void bt_gap_le_srv_dump_conn_update(bt_gap_le_srv_conn_update_t *conn_update)
{
    bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]conn update:[result:%d, handle:0x%4x, ivl:%d, TimOut:%d, latency:%d]", 5,
                    conn_update->result, conn_update->conn_handle, conn_update->params.conn_interval, 
                    conn_update->params.supervision_timeout, conn_update->params.conn_latency);
}

static void bt_gap_le_srv_event_func (bt_gap_le_srv_event_t event, void *data)
{
    bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Srv event %d", 1, event);
    bt_gap_le_srv_event_ind_t *ind = (bt_gap_le_srv_event_ind_t *)data;
    switch (event) {
        case BT_GAP_LE_SRV_EVENT_ADV_COMPLETE: {
            bt_gap_le_srv_dump_adv_complete(&ind->adv_complete);  
        }
            break;

        case BT_GAP_LE_SRV_EVENT_ADV_CLEARED:
            bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Adv Clear result %d", 1, ind->adv_clear.result);
            break;
               
        case BT_GAP_LE_SRV_EVENT_CONN_CLEARED:  
            bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Conn Clear result %d", 1, ind->conn_clear.result);
           break;

        case BT_GAP_LE_SRV_EVENT_CONN_UPDATED:
            bt_gap_le_srv_dump_conn_update(&ind->conn_update);
           break;
           
        case BT_GAP_LE_SRV_EVENT_BLE_DISABLED: 
            bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]BLE disable result %d", 1, ind->dis_complete.result);
            break;

        default :
            break;

    }

}

int16_t bt_gap_le_srv_cmd_entry(const char *string)
{
    bt_gap_le_srv_report("[BLE_GAP_SRV][ATCI][INJECT] string:%s", string);
    /* ACTION PART */
    if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("INIT"))) {
        bt_gap_le_srv_config_t config = {4, 4};
        bt_gap_le_srv_init((const bt_gap_le_srv_config_t *)&config);
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("DEINIT"))) {
        bt_gap_le_srv_deinit();
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("GETINS"))) {
        uint8_t instance = 0;
        bt_gap_le_srv_error_t err = bt_gap_le_srv_get_available_instance(&instance);
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]GET Instance %d, err %d", 2, instance, err);
        g_instance = instance;
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("ADVSTART"))) {
        bt_gap_le_srv_error_t err;
        uint8_t instance = g_instance;
        bt_bd_addr_t random_addr = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0x00};
        bt_gap_le_srv_adv_time_params_t time_param = {0x0000, 0x00};
        random_addr[5] = instance;
        
        err = bt_gap_le_srv_start_adv(instance, (bt_bd_addr_ptr_t)&random_addr,&time_param,bt_gap_le_srv_get_adv_data_func,bt_gap_le_srv_event_func);
        
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]ADV Instance %d start, err %d", 2, instance, err);
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("ADVSTOP"))) { // DISCONNECT
        bt_gap_le_srv_error_t err;
        uint8_t instance = g_instance;

        err = bt_gap_le_srv_stop_adv(instance);
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]ADV Instance %d stop, err %d", 2, instance, err);

    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("ADVUPDATE"))) {
        bt_gap_le_srv_error_t err;
        uint8_t instance = g_instance;

        err = bt_gap_le_srv_update_adv(instance);
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]ADV Instance %d update, err %d", 2, instance, err);
    } else if (0 == bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("ADVREMOVE"))) {
        bt_gap_le_srv_error_t err;
        uint8_t instance = g_instance;

        err = bt_gap_le_srv_remove_adv(instance);
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]ADV Instance %d remove, err %d", 2, instance, err);

    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("ADVCLEAR"))) {
        bt_gap_le_srv_error_t err = bt_gap_le_srv_clear_adv(bt_gap_le_srv_event_func); 
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Clear All ADV, err %d", 1, err);
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("ADVSTATE"))) {
        bt_gap_le_srv_adv_state_t state;
        uint8_t instance = g_instance;

        state = bt_gap_le_srv_get_adv_state(instance);
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]ADV Instance %d state is %d", 2, instance, state);
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("CONNNUM"))) {
        uint8_t conn_num = bt_gap_le_srv_get_connected_dev_num();
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Conn number is %d", 1, conn_num);
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("DUMPCONNINFO"))) {
        bt_gap_le_srv_dump_conn_info_list();
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("DUMPADVINFO"))) {
        bt_gap_le_srv_dump_adv_info_list();
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("CONNCLEAR"))) {
        bt_gap_le_srv_error_t  err = bt_gap_le_srv_clear_connections(bt_gap_le_srv_event_func);
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Clear All connections, err %d", 1, err);
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("DISABLE"))) {
        bt_gap_le_srv_error_t err = bt_gap_le_srv_disable_ble(bt_gap_le_srv_event_func);
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Disable BLE, err %d", 1, err);
    } else if (0 ==  bt_gap_le_srv_memcmp(string, BLESRV_STRPARAM("CONNUPDATE"))) {
        bt_gap_le_srv_error_t err;
        uint16_t conn_handle = 0x0200;
        bt_gap_le_srv_conn_params_t params = {0x50, 0x0000, 0x0258};
        err = bt_gap_le_srv_update_conn_params(conn_handle, &params, bt_gap_le_srv_event_func);
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Conn Update, err %d", 1, err);

    } else {
        return -1;
    }
    return 0;
}

static atci_status_t bt_gap_le_srv_atci_it_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t *response = (atci_response_t *)bt_gap_le_srv_memory_alloc(sizeof(atci_response_t));
    if (NULL == response) {
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI] malloc heap memory fail.", 0);
        return ATCI_STATUS_ERROR;
    }

    bt_gap_le_srv_memset(response, 0, sizeof(atci_response_t));
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+BLESRVIT=<ACTION, PARAMS> */
            if (0 == bt_gap_le_srv_memcmp(parse_cmd->string_ptr + 6, "SRVIT", 5)) {
                int16_t result;
                result = bt_gap_le_srv_cmd_entry(parse_cmd->string_ptr + 12);
                if (result == 0) {
                    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    snprintf((char *)response->response_buf,
                             ATCI_UART_TX_FIFO_BUFFER_SIZE,
                             "command error:%d\r\n",
                             result);
                    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response->response_len = strlen((char *)response->response_buf);
            atci_send_response(response);
            break;

        default :
            /* others are invalid command format */
            response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response->response_len = strlen((char *)response->response_buf);
            atci_send_response(response);
            break;
    }
    bt_gap_le_srv_memory_free(response);
    return ATCI_STATUS_OK;
}

void bt_gap_le_srv_atci_init(void)
{
    atci_status_t ret;
    ret = atci_register_handler(g_ble_srv_atci_cmd, sizeof(g_ble_srv_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));

    if (ret != ATCI_STATUS_OK) {
        bt_gap_le_srv_report_id("[BLE_GAP_SRV][ATCI]Register fail!", 0);
    }
}


