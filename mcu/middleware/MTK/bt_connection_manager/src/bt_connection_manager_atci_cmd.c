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


#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

//#include "hal_gpt.h"
#include "FreeRTOS.h"
#include "serial_port.h"
#include "nvdm.h"
#include "atci.h"
#include "bt_connection_manager_internal.h"
#include "bt_connection_manager_utils.h"
#include "bt_device_manager_internal.h"

//BT releated
#include "bt_os_layer_api.h"
#include "bt_hci.h"
#include "bt_gap_le.h"

//BT Sink releated
#include "bt_sink_srv.h"

#include "bt_aws_mce_srv.h"
#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
#include "bt_aws_mce_role_recovery.h"
#endif

#include "bt_device_manager_power.h"

//defined.
#define LOCAL_ADDR                  (0)
#define AWS_PEER_ADDR               (1)
#define CM_STRPARAM(s)              s, bt_cm_strlen(s)
#define CM_STRCPYN(dest, source)    strncpy(dest, source, strlen(source)+1);

//extern functions.
extern void bt_gap_dump(void);

//static functions.
static atci_status_t bt_cm_atci_local_addr_handler(atci_parse_cmd_param_t *parse_cmd);
#ifdef MTK_AWS_MCE_ENABLE
static atci_status_t bt_cm_atci_agent_addr_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_cm_atci_aws_key_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_cm_atci_aws_ls_enable_handler(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_cm_atci_aws_role_handler(atci_parse_cmd_param_t *parse_cmd);
#endif
static atci_status_t bt_cm_atci_it_handler(atci_parse_cmd_param_t *parse_cmd);

static atci_cmd_hdlr_item_t g_bt_cm_atci_cmd[] = {
    {
        .command_head = "AT+BTCMIT",    /* INTERNAL USE, IT TEST */
        .command_hdlr = bt_cm_atci_it_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTLOCALADDR",    /**< AT command string. */
        .command_hdlr = bt_cm_atci_local_addr_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#ifdef MTK_AWS_MCE_ENABLE
    {
        .command_head = "AT+BTAWSPEERADDR",    /**< AT command string. */
        .command_hdlr = bt_cm_atci_agent_addr_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTAWSROLE",    /**< AT command string. */
        .command_hdlr = bt_cm_atci_aws_role_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTAWSKEY",    /**< AT command string. */
        .command_hdlr = bt_cm_atci_aws_key_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTLSENABLE",    /**< AT command string. */
        .command_hdlr = bt_cm_atci_aws_ls_enable_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#endif
};

static int16_t bt_cm_atci_convert_bt_data(const char *index, uint8_t *bt_data, uint32_t bt_data_len)
{
    int16_t result = 0;
    uint32_t total_num = strlen(index), bt_count = bt_data_len, bt_bit = 1;
    const char *temp_index = index;
    bt_cm_memset(bt_data, 0, bt_data_len);
    bt_cmgr_report_id("[BT_CM][ATCI] total_num:%d,bt_count:%d", 2,total_num, bt_count);
    while (total_num) {
        if (*temp_index <= '9' && *temp_index >= '0') {
            bt_data[bt_count - 1] += ((*temp_index - '0') * (bt_bit * 15 + 1));
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }

        } else if (*temp_index <= 'F' && *temp_index >= 'A') {
            bt_data[bt_count - 1] += ((*temp_index - 'A' + 10) * (bt_bit * 15 + 1));
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }
        } else if (*temp_index <= 'f' && *temp_index >= 'a') {
            bt_data[bt_count - 1] += ((*temp_index - 'a' + 10) * (bt_bit * 15 + 1));
            if (bt_bit == 0) {
                bt_count--;
                bt_bit = 1;
            } else {
                bt_bit--;
            }
        }
        if (!bt_count) {
            break;
        }
        total_num--;
        temp_index++;
    }

    if (bt_count) {
        bt_cm_memset(bt_data, 0, bt_data_len);
        result = -1;
        bt_cmgr_report_id("[BT_CM][ATCI]convert fail:%d", 1,result);
    }

    if (bt_data_len == sizeof(bt_bd_addr_t)) {
        bt_cmgr_report_id("[BT_CM][ATCI]BT addr: %02X:%02X:%02X:%02X:%02X:%02X", 6,
        bt_data[5],bt_data[4],bt_data[3],bt_data[2],bt_data[1],bt_data[0]);
    } else if (bt_data_len == sizeof(bt_key_t)) {
        bt_cmgr_report_id("[BT_CM][ATCI]BT key: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 16,
        bt_data[15],bt_data[14],bt_data[13],bt_data[12],bt_data[11],bt_data[10],
        bt_data[9],bt_data[8],bt_data[7],bt_data[6],bt_data[5],bt_data[4],bt_data[3],
        bt_data[2],bt_data[1],bt_data[0]);
   }
    return result;
}

static atci_status_t bt_cm_atci_addr_internal_handler(atci_parse_cmd_param_t *parse_cmd, uint8_t type)
{
    atci_response_t response = {{0}, 0};
    bt_bd_addr_t address = {0};
    int16_t cmd_str_len = 0;
    int16_t result = -1;
    
    bt_cmgr_report("[BT_CM][ATCI][INJECT] string:%s", parse_cmd->string_ptr);
    response.response_flag = 0; /* Command Execute Finish. */
    cmd_str_len = (LOCAL_ADDR == type ? 15 :17);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+BT<LOCAL,AGENT>ADDR=<action>,<value>  */
            /* rec: <action> = SET/GET/RESET,<value> is address XXXXXXXXXXXX(Hex, 12bytes)  */
            if (0 == bt_cm_memcmp(parse_cmd->string_ptr + cmd_str_len, CM_STRPARAM("SET,"))) {
                result = bt_cm_atci_convert_bt_data(parse_cmd->string_ptr + cmd_str_len + 4,(uint8_t *)&address, sizeof(bt_bd_addr_t));
                if (result == 0) {
                    if (type == LOCAL_ADDR) {
                        bt_device_manager_store_local_address(&address);
                #ifdef MTK_AWS_MCE_ENABLE    
                    } else if (type == AWS_PEER_ADDR){
                        bt_device_manager_aws_local_info_store_peer_address(&address);
                #endif
                    } else {
                        result = -2;
                    }
                    if (type == LOCAL_ADDR || type == AWS_PEER_ADDR) {
                    #ifdef MTK_AWS_MCE_ENABLE
                        bt_device_manager_aws_local_info_update();
                    #endif
                        bt_device_manager_db_flush_all(BT_DEVICE_MANAGER_DB_FLUSH_BLOCK);
                    }
                }
            } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + cmd_str_len, CM_STRPARAM("RESET"))){
                if (type == LOCAL_ADDR) {
                    bt_device_manager_store_local_address(&address);
                    result = 0;
            #ifdef MTK_AWS_MCE_ENABLE
                } else if (type == AWS_PEER_ADDR){
                    bt_device_manager_aws_local_info_store_peer_address(&address);
                    result = 0;
            #endif
                }
                if (type == LOCAL_ADDR || type == AWS_PEER_ADDR) {
            #ifdef MTK_AWS_MCE_ENABLE
                    bt_device_manager_aws_local_info_update();
            #endif
                    bt_device_manager_db_flush_all(BT_DEVICE_MANAGER_DB_FLUSH_BLOCK);
                }
            } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + cmd_str_len, CM_STRPARAM("GET"))){
                uint8_t temp_str[30] = {0};
                bt_bd_addr_t* addr = NULL;
                if (type == LOCAL_ADDR) {
                    addr = bt_device_manager_get_local_address();
            #ifdef MTK_AWS_MCE_ENABLE
                } else if (type == AWS_PEER_ADDR){
                    addr = bt_device_manager_aws_local_info_get_peer_address();
            #endif
                } 
                if (addr) {
                    snprintf((char *)temp_str, sizeof(temp_str), "0x%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                    (*addr)[5],(*addr)[4],(*addr)[3],(*addr)[2],(*addr)[1],(*addr)[0]);
                    snprintf ((char *)response.response_buf, sizeof(response.response_buf), "+Get addrss:%s\r\n", (char *)temp_str);
                    response.response_len = strlen((char *)response.response_buf);
                    result = 0;
                }
            } else {
                result = -1;
            }
            
            if (result == 0) {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                CM_STRCPYN((char *)response.response_buf, "command error.\r\n");
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
  
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

static atci_status_t bt_cm_atci_local_addr_handler(atci_parse_cmd_param_t *parse_cmd)
{
    return bt_cm_atci_addr_internal_handler(parse_cmd, LOCAL_ADDR);
}

#ifdef MTK_AWS_MCE_ENABLE
static atci_status_t bt_cm_atci_agent_addr_handler(atci_parse_cmd_param_t *parse_cmd)
{
    return bt_cm_atci_addr_internal_handler(parse_cmd, AWS_PEER_ADDR);
}

static atci_status_t bt_cm_atci_aws_role_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    response.response_flag = 0; /* Command Execute Finish. */
    bt_cmgr_report("[BT_CM][ATCI][INJECT] string:%s", parse_cmd->string_ptr);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+BTAWSROLE=<action>,<value> */
            /* rec: <action> is GET or SET, <value> is NONE, AGENT,CLIENT */
            if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 13, CM_STRPARAM("GET"))){
                bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();
                if (role == BT_AWS_MCE_ROLE_NONE) {
                    CM_STRCPYN((char *)response.response_buf, "+AWS Role:NONE.\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (role == BT_AWS_MCE_ROLE_CLINET) {
                    CM_STRCPYN((char *)response.response_buf, "+AWS Role:CLIENT.\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (role == BT_AWS_MCE_ROLE_AGENT) {
                    CM_STRCPYN((char *)response.response_buf, "+AWS Role:AGENT.\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                 } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    CM_STRCPYN((char *)response.response_buf, "+AWS Role:PARTNER.\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    CM_STRCPYN((char *)response.response_buf, "+AWS Role:Error role!\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 13, CM_STRPARAM("SET,"))){
                if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 17, CM_STRPARAM("NONE"))) {
                    bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_NONE);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 17, CM_STRPARAM("CLIENT"))) {
                    bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_CLINET);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 17, CM_STRPARAM("AGENT"))) {
                    bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_AGENT);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 17, CM_STRPARAM("PARTNER"))) {
                    bt_connection_manager_device_local_info_store_aws_role(BT_AWS_MCE_ROLE_PARTNER);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
                if (response.response_flag == ATCI_RESPONSE_FLAG_APPEND_OK) {
                #ifdef MTK_AWS_MCE_ENABLE
                    bt_device_manager_aws_local_info_update();
                #endif
                    bt_device_manager_db_flush_all(BT_DEVICE_MANAGER_DB_FLUSH_BLOCK);
                }
            } else {
                CM_STRCPYN((char *)response.response_buf, "Wrong action.\n");
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

static atci_status_t bt_cm_atci_aws_ls_enable_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    response.response_flag = 0; /* Command Execute Finish. */

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+BTAWSROLE=<action>,<value> */
            /* rec: <action> is GET or SET, <value> is NONE, AGENT,CLIENT */
            if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 14, CM_STRPARAM("GET"))){
                bt_connection_manager_aws_ls_enable_t ls_enalbe = bt_connection_manager_device_local_info_get_aws_ls_enable();
                if (ls_enalbe == BT_CONNECTION_MANAGER_AWS_LS_DISABLE) {
                    CM_STRCPYN((char *)response.response_buf, "+AWS LS:DISABLE.\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (ls_enalbe == BT_CONNECTION_MANAGER_AWS_LS_ENABLE) {
                    CM_STRCPYN((char *)response.response_buf, "+AWS LS:ENABLE.\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    CM_STRCPYN((char *)response.response_buf, "+AWS LS enable:Error!\r\n");
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 14, CM_STRPARAM("SET,"))){
                if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 18, CM_STRPARAM("DISABLE"))) {
                    bt_connection_manager_device_local_info_store_aws_ls_enable(BT_CONNECTION_MANAGER_AWS_LS_DISABLE);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 18, CM_STRPARAM("ENABLE"))) {
                    bt_connection_manager_device_local_info_store_aws_ls_enable(BT_CONNECTION_MANAGER_AWS_LS_ENABLE);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                CM_STRCPYN((char *)response.response_buf, "Wrong action.\n");
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

static atci_status_t bt_cm_atci_aws_key_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0};
    bt_key_t aws_key = {0};

    response.response_flag = 0; /* Command Execute Finish. */
    bt_cmgr_report("[BT_CM][ATCI][INJECT] string:%s", parse_cmd->string_ptr);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+BTAWSKEY=<> */
            if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 12, CM_STRPARAM("GET"))){
                uint8_t temp_str[80] = {0};
                bt_key_t* key = NULL;
            #ifdef MTK_AWS_MCE_ENABLE 
                key = bt_device_manager_aws_local_info_get_key();
            #endif
                snprintf((char *)temp_str, sizeof(temp_str), "0x%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X",
                (*key)[15],(*key)[14],(*key)[13],(*key)[12],(*key)[11],
                (*key)[10],(*key)[9], (*key)[8], (*key)[7], (*key)[6],
                (*key)[5], (*key)[4], (*key)[3], (*key)[2], (*key)[1],(*key)[0]);
                snprintf ((char *)response.response_buf, sizeof(response.response_buf), "+Get key:%s\r\n", (char *)temp_str);
                response.response_len = strlen((char *)response.response_buf);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 12, CM_STRPARAM("RESET"))){
            #ifdef MTK_AWS_MCE_ENABLE
                bt_device_manager_aws_local_info_store_key(&aws_key);
                bt_device_manager_aws_local_info_update();
            #endif
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 12, CM_STRPARAM("SET,"))){
                int16_t result = bt_cm_atci_convert_bt_data(parse_cmd->string_ptr + 12 + 4,(uint8_t *)&aws_key, sizeof(bt_key_t));
                if (result == 0) {
                #ifdef MTK_AWS_MCE_ENABLE
                    bt_device_manager_aws_local_info_store_key(&aws_key);
                    bt_device_manager_aws_local_info_update();
                #endif
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                   CM_STRCPYN((char *)response.response_buf, "Wrong parameters.\n");
                   response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            }
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;

        default :
            /* others are invalid command format */
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            response.response_len = strlen((char *)response.response_buf);
            atci_send_response(&response);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif //#ifdef MTK_AWS_MCE_ENAB

int16_t bt_cm_cmd_entry(const char *string)
{
    bt_cmgr_report("[BT_CM][ATCI][INJECT] string:%s", string);
    /* ACTION PART */
    if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("DSC"))) { //DISCOVERABLE
        bt_cm_discoverable(true);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("EXIT-DSC"))) {
        bt_cm_discoverable(false);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("CLD"))) { //CONNECT LAST DEVICE
        bt_aws_mce_role_t role = BT_AWS_MCE_ROLE_NONE;
        bt_bd_addr_t *address_p = NULL;
        bt_cm_connect_t conn_cntx;
    #ifdef MTK_AWS_MCE_ENABLE
        role = bt_device_manager_aws_local_info_get_role();
    #endif
        if (role == BT_AWS_MCE_ROLE_NONE || role == BT_AWS_MCE_ROLE_AGENT) {
            address_p = bt_cm_get_last_connected_device();
            conn_cntx.profile =  BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_HFP)
                | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_A2DP_SINK)
                | BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_CUSTOMIZED_IAP2);
        } else {
    #ifdef MTK_AWS_MCE_ENABLE
            address_p = bt_device_manager_aws_local_info_get_peer_address();
            conn_cntx.profile =  BT_CM_PROFILE_SERVICE_MASK(BT_CM_PROFILE_SERVICE_AWS);
    #endif
        }
        if (NULL != address_p) {
            bt_cm_memcpy(&conn_cntx.address, address_p, sizeof(bt_bd_addr_t));
            bt_cm_connect(&conn_cntx);
        } else {
            bt_cmgr_report_id("[BT_CM][ATCI]No trusted devices", 0);
        }
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("CON"))) { //CONNECT remote DEVICE
        bt_cm_connect_t connect_cntx;
        connect_cntx.profile = 0x32;
        int16_t result = bt_cm_atci_convert_bt_data(string + 4,(uint8_t *)&(connect_cntx.address), sizeof(bt_bd_addr_t));
        if (result == 0) {
            bt_cm_connect(&connect_cntx);
        }
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("DIS"))) { // DISCONNECT
        uint32_t conn_num = 0;
        bt_bd_addr_t *local_addr = bt_device_manager_get_local_address();
        bt_bd_addr_t addr_list[3];
        bt_cm_connect_t dis_conn = {
            .profile = BT_CM_PROFILE_SERVICE_MASK_ALL
        };
        conn_num = bt_cm_get_connected_devices(BT_CM_PROFILE_SERVICE_MASK_NONE, addr_list, sizeof(addr_list) / sizeof(bt_bd_addr_t));
        bt_cmgr_report_id("[BT_CM][ATCI] Disconnect all connected device num %d!", 1, conn_num);
        for (uint32_t i = 0; i < conn_num; i++) {
            if (!bt_cm_memcmp(local_addr, &addr_list[i], sizeof(bt_bd_addr_t))) {
                continue;
            }
            bt_cmgr_report_id("[BT_CM][ATCI] To disconnect 0x%x!", 1, *(uint32_t *)(&addr_list[i]));
            bt_cm_memcpy(&(dis_conn.address), &(addr_list[i]), sizeof(bt_bd_addr_t));
            bt_cm_disconnect(&dis_conn);
        }
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("RESET"))) {
        bt_device_manager_unpair_all();
    } else if (0 == bt_cm_memcmp(string, CM_STRPARAM("DUMP"))) {
        bt_cmgr_report_id("[BT_CM][ATCI]DUMP", 0);
        bt_gap_dump();
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("GRRSSI"))) {//GET RAW RSSI
        bt_bd_addr_t address = {0};
        bt_cm_atci_convert_bt_data(string + 7,(uint8_t *)&address, sizeof(bt_bd_addr_t));
        bt_gap_connection_handle_t handle = bt_sink_srv_cm_get_gap_handle(&address);
        if (handle != 0) {
            bt_status_t status = bt_gap_read_raw_rssi(handle);
            bt_cmgr_report_id("[BT_CM][ATCI]Get raw rssi 0x%x", 1, status);
        }
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("GRSSI"))) {//GET RSSI
        bt_bd_addr_t address = {0};
        bt_cm_atci_convert_bt_data(string + 6,(uint8_t *)&address, sizeof(bt_bd_addr_t));
        bt_gap_connection_handle_t handle = bt_sink_srv_cm_get_gap_handle(&address);
        if (handle != 0) {
            bt_status_t status = bt_gap_read_rssi(handle);
            bt_cmgr_report_id("[BT_CM][ATCI]Get rssi 0x%x", 1, status);
        }
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("POWEROFF"))) { //Power off
        bt_os_layer_sleep_task(1000);
        bt_cm_power_test_sys(BT_CM_POWER_TEST_SYS_OFF);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("POWERRESET"))) { //Power reset
        bt_os_layer_sleep_task(1000);
        bt_cm_power_test_sys(BT_CM_POWER_TEST_SYS_RESET);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("BT-POWER-OFF"))) {
        bt_status_t status = BT_STATUS_SUCCESS;
        status = bt_power_off();
        if(status != BT_STATUS_SUCCESS) {
           bt_cmgr_report_id("[BT_CM][ATCI]power off fail !", 0);
        } else {
            bt_cmgr_report_id("[BT_CM][ATCI]power off success!", 0);
        }
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("OFF-PAGE-SCAN"))) {
        bt_cmgr_report_id("[BT_CM][ATCI]to disable page scan!", 0);
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_DISABLE);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("ON-PAGE-SCAN"))) {
        bt_cmgr_report_id("[BT_CM][ATCI]to enable page scan!", 0);
        bt_cm_write_scan_mode(BT_CM_COMMON_TYPE_UNKNOW, BT_CM_COMMON_TYPE_ENABLE);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("OFF-LE-ADV"))) {
        bt_cmgr_report_id("[BT_CM][ATCI]to disable le adv!", 0);
        bt_hci_cmd_le_set_advertising_enable_t enable;
        enable.advertising_enable = BT_HCI_DISABLE;
        bt_gap_le_set_advertising(&enable, NULL, NULL, NULL);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("SET_LINKLOSS_RETRY"))) {
        uint8_t retry_num = atoi(string + 19);
        bt_cmgr_report_id("[BT_CM][ATCI]to set link loss retry numbers : %d", 1, retry_num);
        //....
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("BT_STANDBY"))) {
        bt_device_manager_power_standby(BT_DEVICE_TYPE_LE | BT_DEVICE_TYPE_CLASSIC);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("BT_ACTIVE"))) {
        bt_device_manager_power_active(BT_DEVICE_TYPE_LE | BT_DEVICE_TYPE_CLASSIC);
#ifdef MTK_AWS_MCE_ENABLE
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("LS_DIS"))) {
        bt_sink_srv_cm_ls_enable(false);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("LS_EN"))) {
        bt_sink_srv_cm_ls_enable(true); 
#endif
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("inquiry_test0"))) {
        bt_gap_inquiry_ext(20, 0, 0x9E8B11);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("inquiry_test1"))) {
        bt_gap_inquiry(20, 0);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("inquiry_test2"))) {
        bt_gap_inquiry_ext(20, 0, BT_HCI_IAC_LAP_TYPE_LIAC);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("set_iac_lap"))) {
        bt_hci_iac_lap_t iac = 0x9E8B11;
        bt_gap_write_inquiry_access_code(&iac, 1);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("set_scan_mode"))) {
        bt_gap_scan_mode_t scan_mode = atoi(string + 14);
        bt_cmgr_report_id("[BT_CM][ATCI] To set scan mode : %d", 1, scan_mode);
        bt_gap_set_scan_mode(scan_mode);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("bt_reset"))) {
        bt_device_manager_power_reset_t reset_type = atoi(string + 9);
        bt_device_manager_power_reset(reset_type, NULL, NULL);
#ifdef MTK_AWS_MCE_ENABLE
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("start_air_pair"))) {
        bt_aws_mce_role_t aws_role = bt_connection_manager_device_local_info_get_aws_role();
        bt_aws_mce_srv_air_pairing_t air_cnt = {
            .duration = 20,
            .air_pairing_key = {0},
            .air_pairing_info = {0},
            .rssi_threshold = -120
        };
        air_cnt.default_role = aws_role;
        air_cnt.air_pairing_key[0] = 11;
        air_cnt.air_pairing_key[1] = 22;
        air_cnt.air_pairing_key[2] = 33;
        bt_aws_mce_srv_air_pairing_start(&air_cnt);
    } else if (0 ==  bt_cm_memcmp(string, CM_STRPARAM("stop_air_pair"))) {
        bt_aws_mce_srv_air_pairing_stop();
#endif
    } else if (0 == bt_cm_memcmp(string, CM_STRPARAM("reset_sniff_timer"))) {
        uint32_t sniff_timer = atoi(string + 18);
        bt_cmgr_report_id("[BT_CM][ATCI]To reset sniff timer to : %d ms", 1, sniff_timer);
        bt_gap_reset_sniff_timer(sniff_timer);
    } else if (0 == bt_cm_memcmp(string, CM_STRPARAM("STOP_SLAVE_SWITCH"))) {
#ifdef MTK_AWS_MCE_ROLE_RECOVERY_ENABLE
    } else if (0 == bt_cm_memcmp(string, CM_STRPARAM("lock_role_recovery"))) {
        bt_aws_mce_role_recovery_lock();
    } else if (0 == bt_cm_memcmp(string, CM_STRPARAM("unlock_role_recovery"))) {
        bt_aws_mce_role_recovery_unlock();
#endif
    } else if (0 == bt_cm_memcmp(string, CM_STRPARAM("sniff_param"))){
        bt_gap_default_sniff_params_t sniff_params;
        sniff_params.max_sniff_interval = strtol(string + 12, NULL, 16);
        sniff_params.min_sniff_interval = strtol(string + 17, NULL, 16);
        sniff_params.sniff_attempt = strtol(string + 22, NULL, 16);
        sniff_params.sniff_timeout = strtol(string + 27, NULL, 16);
        bt_cmgr_report_id("[BT_CM][ATCI] sniff_param: max: %x, min: %x, attempt: %x, timeout:%x ", 4, 
                          sniff_params.max_sniff_interval, sniff_params.min_sniff_interval, sniff_params.sniff_attempt, sniff_params.sniff_timeout);
        bt_gap_set_default_sniff_parameters(&sniff_params);
    } else if(0 == bt_cm_memcmp(string, CM_STRPARAM("low_power_enable"))){
        uint16_t opcode = 0xFC10;
        uint8_t enable = strtol(string + 17, NULL, 16);
        bt_hci_send_vendor_cmd(opcode,&enable,1);
#ifdef MTK_AWS_MCE_ENABLE
    } else if (0 == bt_cm_memcmp(string, CM_STRPARAM("spkmode_reset"))) {
         bt_bd_addr_t peer_addr = {0};
         bt_device_manager_aws_local_info_store_mode(BT_AWS_MCE_SRV_MODE_SINGLE);
         bt_device_manager_aws_local_info_store_role(BT_AWS_MCE_ROLE_NONE);
         bt_device_manager_aws_local_info_store_peer_address(&peer_addr);
         bt_device_manager_aws_local_info_update();
#endif
    } else {
        return -1;
    }
    return 0;
}

static atci_status_t bt_cm_atci_it_handler(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t *response = (atci_response_t *)bt_cm_memory_alloc(sizeof(atci_response_t));
    if (NULL == response) {
        bt_cmgr_report_id("[BT_CM][ATCI] malloc heap memory fail.", 0);
        return ATCI_STATUS_ERROR;
    }

    bt_cm_memset(response, 0, sizeof(atci_response_t));
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: /* rec: AT+BTCMIT=<ACTION, PARAMS> */
            if (0 == bt_cm_memcmp(parse_cmd->string_ptr + 5, "CMIT", 4)) {
                int16_t result;
                result = bt_cm_cmd_entry(parse_cmd->string_ptr + 10);
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
    bt_cm_memory_free(response);
    return ATCI_STATUS_OK;
}

void bt_cm_atci_init(void)
{
    atci_status_t ret;
    ret = atci_register_handler(g_bt_cm_atci_cmd, sizeof(g_bt_cm_atci_cmd) / sizeof(atci_cmd_hdlr_item_t));

    if (ret != ATCI_STATUS_OK) {
        bt_cmgr_report_id("[BT_CM][ATCI]Register fail!", 0);
    }
}

