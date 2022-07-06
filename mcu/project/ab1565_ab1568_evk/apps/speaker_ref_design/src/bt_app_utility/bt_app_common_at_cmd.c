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


#include <stdlib.h>
#include "FreeRTOS.h"
#include "atci.h"
#include "bt_type.h"
#include "bt_app_common.h"
#ifdef MTK_FOTA_VIA_RACE_CMD
#include "race_fota.h"
#endif
#ifdef MTK_IAP2_PROFILE_ENABLE
#include "apps_debug.h"
#include "mfi_coprocessor_api.h"
#endif

#if defined(SUPPORT_ROLE_HANDOVER_SERVICE)
#include "bt_role_handover.h"
#endif
#ifdef __BT_FAST_PAIR_ENABLE__
#include "app_fast_pair.h"
#endif
#include "bt_gap_le.h"
#include "bt_customer_config.h"
#ifdef MTK_IAP2_PROFILE_ENABLE
#include "aacp_i2c.h"
#endif
#include "apps_bt_aws_mce_le_association.h"
#include "bt_aws_mce_ls_association.h"
#include "bt_aws_mce_srv.h"
#include "apps_events_event_group.h"
#include "ui_shell_manager.h"
#include "apps_config_event_list.h"
#include "apps_events_key_event.h"
#include "multi_ble_adv_manager.h"
#include "nvdm.h"
#include "nvdm_id_list.h"
#include "bt_device_manager_le.h"

extern bt_status_t bt_app_common_advtising_stop(void);
static atci_status_t bt_app_comm_at_cmd_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_fota_hdl(atci_parse_cmd_param_t *parse_cmd);
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
static atci_status_t bt_app_comm_at_cmd_rho_hdl(atci_parse_cmd_param_t *parse_cmd);
#endif
extern void bt_app_common_start_scan();
extern void bt_app_common_stop_scan();
extern const bt_bd_addr_t *bt_app_common_get_local_random_addr(void);
static atci_status_t bt_app_comm_at_cmd_ble_scan_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ble_cancel_conn_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ble_random_addr_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_set_fast_pair_tx_power_level(atci_parse_cmd_param_t *parse_cmd);

#ifdef MTK_IAP2_PROFILE_ENABLE
static atci_status_t bt_app_comm_at_cmd_get_mfi_cert_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_get_mfi_response_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_check_mfi_chip_hdl(atci_parse_cmd_param_t *parse_cmd);
#endif
static atci_status_t bt_app_comm_at_cmd_sniff_mode(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ext_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd);

bt_status_t bt_app_common_ext_advertising_stop_test(uint8_t instance);
bt_status_t bt_app_common_ext_advertising_start_test(uint8_t instance);
static atci_status_t bt_app_comm_at_cmd_hfp_codec_set_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_bt_aws_le_associate(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_sink_app_command_get_mode();
static atci_status_t bt_sink_app_command_spk_mode(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_sink_app_command_set_mode(atci_parse_cmd_param_t *parse_cmd);
static void          bt_app_comm_at_cmd_copy_str_to_addr(uint8_t *addr, const char *str);
static atci_status_t bt_app_comm_at_cmd_spk_adv_addr(atci_parse_cmd_param_t *parse_cmd);


static atci_cmd_hdlr_item_t bt_app_comm_at_cmd[] = {
    {
        .command_head = "AT+BLEADV",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_adv_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+FOTA",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_fota_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    ,
    {
        .command_head = "AT+RHO",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_rho_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
#endif
    ,
    {
        .command_head = "AT+BLESCAN",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_scan_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BLECANCELCONN",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_cancel_conn_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BLERANDOMADDR",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_random_addr_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+FPSTXPOWER",
        .command_hdlr = bt_app_comm_at_cmd_set_fast_pair_tx_power_level,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#ifdef MTK_IAP2_PROFILE_ENABLE
    {
        .command_head = "AT+GETMFICERT",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_get_mfi_cert_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+GETMFIRESP",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_get_mfi_response_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+CHECKMFICHIP",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_check_mfi_chip_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#endif
    {
        .command_head = "AT+SNIFF",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_sniff_mode,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
    ,
    {
        .command_head = "AT+EXTBLEADV", /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ext_ble_adv_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+HFPCODEC", /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_hfp_codec_set_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BTMODESET", /**< AT command string. */
        .command_hdlr = bt_sink_app_command_set_mode,
        .hash_value1 = 0,
        .hash_value2 = 0
    },
    {
        .command_head = "AT+SPKMODE", /**< AT command string. */
        .command_hdlr = bt_sink_app_command_spk_mode,
        .hash_value1 = 0,
        .hash_value2 = 0
    },
    {
        .command_head = "AT+SPKADVADDR",    /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_spk_adv_addr,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
};


static atci_status_t bt_app_comm_at_cmd_spk_adv_addr(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    switch (parse_cmd->mode)
    {
        case ATCI_CMD_MODE_EXECUTION:
        {
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "GET", 3))
            {
                uint8_t temp_str[30] = {0};
                bt_bd_addr_t* addr = NULL;

                uint32_t nvdm_size = sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT;
                uint8_t temp_addr[sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_MAX_COUNT] = {
                    0
                };
                if (NVDM_STATUS_OK != nvdm_read_data_item(NVDM_GROUP_MMI_APP, NVDM_MULTI_ADV_LE_ADDR, temp_addr, &nvdm_size)
                        || nvdm_size < sizeof(bt_bd_addr_t) * (MULTI_ADV_INSTANCE_SPK_ASS + 1)) {
                    break;
                }

                addr = (bt_bd_addr_t *)(temp_addr + sizeof(bt_bd_addr_t) * MULTI_ADV_INSTANCE_SPK_ASS);

                snprintf((char *)temp_str, sizeof(temp_str), "0x%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                    (*addr)[5],(*addr)[4],(*addr)[3],(*addr)[2],(*addr)[1],(*addr)[0]);
                    snprintf ((char *)response.response_buf, sizeof(response.response_buf), "+ADV addrss:%s\r\n", (char *)temp_str);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static int16_t bt_app_common_at_cmd_convert_bt_data(const char *index, uint8_t *bt_data, uint32_t bt_data_len)
{
    int16_t result = 0;
    uint32_t total_num = strlen(index), bt_count = bt_data_len, bt_bit = 1;
    const char *temp_index = index;
    memset(bt_data, 0, bt_data_len);
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
        memset(bt_data, 0, bt_data_len);
        result = -1;
        LOG_MSGID_I(BT_APP,"[BTAWS_ATCMD]convert fail", 0);
    }

    if (bt_data_len == sizeof(bt_bd_addr_t)) {
        LOG_MSGID_I(BT_APP,"[BTAWS_ATCMD]LS addr: %02X:%02X:%02X:%02X:%02X:%02X", 6,
        bt_data[5],bt_data[4],bt_data[3],bt_data[2],bt_data[1],bt_data[0]);
    } else if (bt_data_len == sizeof(bt_key_t)) {
        LOG_MSGID_I(BT_APP,"[BTAWS_ATCMD]AWS key: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X", 16,
        bt_data[15],bt_data[14],bt_data[13],bt_data[12],bt_data[11],bt_data[10],
        bt_data[9],bt_data[8],bt_data[7],bt_data[6],bt_data[5],bt_data[4],bt_data[3],
        bt_data[2],bt_data[1],bt_data[0]);
   }
    return result;
}


#define STRNCPY(dest, source) strncpy(dest, source, strlen(source)+1);
static atci_status_t bt_sink_app_command_get_mode()
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    bt_aws_mce_srv_mode_t mode = bt_aws_mce_srv_get_mode();
    bt_aws_mce_role_t role = bt_connection_manager_device_local_info_get_aws_role();

    do {
        if (mode == BT_AWS_MCE_SRV_MODE_NORMAL || mode == BT_AWS_MCE_SRV_MODE_SINGLE) {
            STRNCPY((char *)response.response_buf,"+Mode: SINGLE, ");
        } else if (mode == BT_AWS_MCE_SRV_MODE_BROADCAST) {
            STRNCPY((char *)response.response_buf,"+Mode: BROADCAST, ");
        } else if (mode == BT_AWS_MCE_SRV_MODE_DOUBLE) {
            STRNCPY((char *)response.response_buf,"+Mode: DOUBLE, ");
        } else {
            STRNCPY((char *)response.response_buf,"+Mode: Error Mode!\r\n");
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            break;
        }

        if (role == BT_AWS_MCE_ROLE_NONE) {
            strcat((char *)response.response_buf,"Role: None.\r\n");
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else if (role == BT_AWS_MCE_ROLE_AGENT) {
            strcat((char *)response.response_buf,"Role: Agent.\r\n");
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else if (role == BT_AWS_MCE_ROLE_CLINET) {
            strcat((char *)response.response_buf,"Role: Client.\r\n");
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
            strcat((char *)response.response_buf,"Role: Partner.\r\n");
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        } else {
            strcat((char *)response.response_buf,"Role: error Role!\r\n");
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
    } while(0);

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t  bt_sink_app_command_set_mode(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+BTMODESET=<action>
                        * action:"mode, [role,agent address]"
                        * SINGLE
                        */
            uint32_t string_ptr_offset = parse_cmd->name_len + 1;
            if (0 == memcmp(parse_cmd->string_ptr + string_ptr_offset, "SINGLE", strlen("SINGLE"))) {
                bt_aws_mce_srv_mode_t mode = BT_AWS_MCE_SRV_MODE_SINGLE;
                bt_aws_mce_srv_mode_switch_t param = {0};
                param.role = BT_AWS_MCE_ROLE_NONE;
                if (bt_aws_mce_srv_switch_mode(mode, &param) == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + string_ptr_offset, "DOUBLE,AGENT", strlen("DOUBLE,AGENT"))) {
                bt_aws_mce_srv_mode_t mode = BT_AWS_MCE_SRV_MODE_DOUBLE;
                bt_aws_mce_srv_mode_switch_t param = {0};
                param.role = BT_AWS_MCE_ROLE_AGENT;
                if (bt_aws_mce_srv_switch_mode(mode, &param) == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + string_ptr_offset, "DOUBLE,PARTNER", strlen("DOUBLE,PARTNER"))) {
                bt_aws_mce_srv_mode_t mode = BT_AWS_MCE_SRV_MODE_DOUBLE;
                bt_aws_mce_srv_mode_switch_t param = {0};
                const char *addr_str = parse_cmd->string_ptr + string_ptr_offset + strlen("DOUBLE,PARTNER,");
                bt_app_comm_at_cmd_copy_str_to_addr(param.addr, addr_str);
                param.role = BT_AWS_MCE_ROLE_PARTNER;
                if (bt_aws_mce_srv_switch_mode(mode, &param) == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + string_ptr_offset, "BROADCAST,AGENT", strlen("BROADCAST,AGENT"))) {
                bt_aws_mce_srv_mode_t mode = BT_AWS_MCE_SRV_MODE_BROADCAST;
                bt_aws_mce_srv_mode_switch_t param = {0};
                param.role = BT_AWS_MCE_ROLE_AGENT;
                if (bt_aws_mce_srv_switch_mode(mode, &param) == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + string_ptr_offset, "BROADCAST,CLIENT", strlen("BROADCAST,CLIENT"))) {
                bt_aws_mce_srv_mode_t mode = BT_AWS_MCE_SRV_MODE_BROADCAST;
                bt_aws_mce_srv_mode_switch_t param = {0};
                const char *addr_str = parse_cmd->string_ptr + string_ptr_offset + strlen("BROADCAST,CLIENT,");
                bt_app_comm_at_cmd_copy_str_to_addr(param.addr, addr_str);
                param.role = BT_AWS_MCE_ROLE_CLINET;
                if (bt_aws_mce_srv_switch_mode(mode, &param) == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            }
            break;
        }

        default:
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


static atci_status_t bt_app_comm_at_cmd_bt_aws_le_associate(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    bt_aws_mce_srv_mode_t curr_mode = bt_aws_mce_srv_get_mode();
    bt_aws_mce_role_t curr_role = bt_connection_manager_device_local_info_get_aws_role();
    LOG_MSGID_I(BT_APP, "[app_speaker]at_cmd:curr_role: %x, curr_mode : %x", 2, curr_role, curr_mode);

    switch (parse_cmd->mode)
    {
        case ATCI_CMD_MODE_EXECUTION:
        {
            uint16_t *key_id = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
            if (key_id == NULL) {
                LOG_MSGID_E(BT_APP, "[app_speaker]at_cmd: malloc fail", 0);
                break;
            }
            memset(key_id, 0, sizeof(sizeof(uint16_t)));
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "CLWL", 4)) {
                bt_device_manager_le_clear_all_bonded_info();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "DOUBLE,", 7)) {
                if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 8, "AGENT", 5)) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    *key_id = KEY_DOUBLE_AGENT;
                } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 8, "PARTNER", 7))  {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    *key_id = KEY_DOUBLE_PARTNER;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }

            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "BROADCAST,", 10)) {
                if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 11, "AGENT", 5)) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    *key_id = KEY_BROADCAST_AGENT;
                } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 11, "CLIENT", 6)) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    *key_id = KEY_BROADCAST_CLIENT;
                } else {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }

            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "UNGROUP", 7)) {
                LOG_MSGID_I(BT_APP, "[app_speaker]at_cmd ungroup", 0);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                *key_id = KEY_SPK_UNGROUP;
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "KEY,", 4)) {
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                LOG_MSGID_I(BT_APP, "[app_speaker]at_cmd set aws key", 0);
                bt_key_t aws_key = {0};
                int16_t result = bt_app_common_at_cmd_convert_bt_data(parse_cmd->string_ptr + parse_cmd->name_len + 5,
                        (uint8_t *)&aws_key, sizeof(bt_key_t));
                if (result == 0){
                    bt_aws_mce_le_association_app_set_aws_key(aws_key);
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "LSADDR,", 7)) {
                LOG_MSGID_I(BT_APP, "[app_speaker]at_cmd set ls addr", 0);
                /*
                bt_bd_addr_t ls_addr = {0};
                int16_t result = bt_app_common_at_cmd_convert_bt_data(parse_cmd->string_ptr + parse_cmd->name_len + 8,
                        (uint8_t *)&ls_addr, sizeof(bt_bd_addr_t));
                if (result == 0){
                    //bt_aws_ls_association_write_address(&ls_addr);
                    nvdm_status_t status = nvdm_write_data_item(NVDM_GROUP_BT_APP, NVDM_BT_APP_ITEM_FIX_ADDR, NVDM_DATA_ITEM_TYPE_RAW_DATA, &ls_addr, sizeof(bt_bd_addr_t));
                    if (status != NVDM_STATUS_OK) {
                        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    } else{
                        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    }
                }
                */
            }
            if (response.response_flag == ATCI_RESPONSE_FLAG_APPEND_OK) {
                ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY, INVALID_KEY_EVENT_ID, key_id, sizeof(uint16_t), NULL, 0);
            } else {
                vPortFree(key_id);
            }
            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t  bt_sink_app_command_spk_mode(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_READ: {
            /* AT+SPKMODESET? */
            return bt_sink_app_command_get_mode();
        }
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+SPKMODESET=<mode>,<role>
                        * mode:SINGLE,DOUBLE,BROADCAST
                        * role:NONE,AGENT,PARTNER,CLIENT
                        */
            uint32_t string_ptr_offset = parse_cmd->name_len + 1;
            if (0 == memcmp(parse_cmd->string_ptr + string_ptr_offset, "SINGLE", strlen("SINGLE"))) {
                uint16_t *key_id = (uint16_t *)pvPortMalloc(sizeof(uint16_t));
                if (key_id == NULL) {
                    LOG_MSGID_E(BT_APP, "[app_speaker]at_cmd: malloc fail", 0);
                    break;
                }
                memset(key_id, 0, sizeof(sizeof(uint16_t)));
                *key_id = KEY_SPK_SINGLE;

                LOG_MSGID_I(BT_APP, "[app_speaker]at_cmd switch to single", 0);
                ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY, INVALID_KEY_EVENT_ID, key_id, sizeof(uint16_t), NULL, 0);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                return bt_app_comm_at_cmd_bt_aws_le_associate(parse_cmd);
            }
            break;
        }

        default:
            break;
    }
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


static atci_status_t bt_app_comm_at_cmd_sniff_mode(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_OK};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    bt_init_feature_mask_t bt_customer_config_feature = bt_customer_config_get_feature_mask_configuration();
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+SNIFF=<action>
                * action: "ON" / "OFF"
                */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                bt_customer_config_feature |= BT_INIT_FEATURE_MASK_DISABLE_SNIFF_MODE;
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                bt_customer_config_feature &= ~BT_INIT_FEATURE_MASK_DISABLE_SNIFF_MODE;
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


static atci_status_t bt_app_comm_at_cmd_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+BLEADV=<action>
                        * action: "ON" / "OFF"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                /* BLE ADV OFF */
                bt_status_t status = bt_app_common_advtising_stop();
                if (status == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                /* BLE ADV ON */
                bt_status_t status = bt_app_common_start_ble_adv_with_default_interval();
                if (status == BT_STATUS_SUCCESS) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


static atci_status_t bt_app_comm_at_cmd_fota_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+FOTA=<action>
                        * action: "CANCEL"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "CANCEL", 6)) {
#ifdef MTK_FOTA_VIA_RACE_CMD
                /* STOP FOTA */
                if (RACE_ERRCODE_SUCCESS == race_fota_cancel()) {
                    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
#endif
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

void bt_app_common_at_cmd_print_report(char *string)
{
    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (response != NULL) {
        memset(response, 0, sizeof(*response));
    } else {
        return;
    }
    snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
             "%s\n", (char *)string);
    response->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
    response->response_len = strlen((char *)response->response_buf);
    if (response->response_len > 0) {
        atci_send_response(response);
    }
    vPortFree(response);
    return;
}

#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
void bt_app_common_at_cmd_rho_srv_status_callback(const bt_bd_addr_t *addr, bt_aws_mce_role_t role, bt_role_handover_event_t event, bt_status_t status)
{
    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    LOG_MSGID_I(BT_APP, "RHO srv status_callback role 0x%x, event %d, status 0x%x", 3, role, event, status);
    if (response != NULL) {
        memset(response, 0, sizeof(*response));
    } else {
        return;
    }
    switch (event) {
        case BT_ROLE_HANDOVER_PREPARE_REQ_IND: {
            snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                     "Alert:RHO prepare\r\n");
            break;
        }
        case BT_ROLE_HANDOVER_START_IND: {
            snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                     "Alert:RHO start\r\n");
            break;
        }
        case BT_ROLE_HANDOVER_COMPLETE_IND: {
            uint8_t role_string[20] = {0};
            if (status == BT_STATUS_SUCCESS) {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    strcpy((char *)role_string, "(Partner now).\r\n");
                } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    strcpy((char *)role_string, "(Agent now).\r\n");
                } else {
                    strcpy((char *)role_string, "(error now)!\r\n");
                }
                snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "Alert:RHO succuss%s", (char *)role_string);
            } else {
                if (role == BT_AWS_MCE_ROLE_AGENT) {
                    strcpy((char *)role_string, "(Agent now).\r\n");
                } else if (role == BT_AWS_MCE_ROLE_PARTNER) {
                    strcpy((char *)role_string, "(Partner now).\r\n");
                } else {
                    strcpy((char *)role_string, "(error now)!\r\n");
                }
                snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                         "Alert:RHO fail%s", (char *)role_string);
            }
            break;
        }
    }

    response->response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT;
    response->response_len = strlen((char *)response->response_buf);
    if (response->response_len > 0) {
        atci_send_response(response);
    }
    vPortFree(response);
}
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#define BT_EXT_TIMER_TEST_TIMER_ID             (BT_TIMER_EXT_GROUP_SINK(9))
#define BT_EXT_TIMER_TEST_TIMER_DUR            (20 * 1000)
uint32_t g_timer_data = 0x1234;

void bt_app_common_timer_test_cb(uint32_t timer_id, uint32_t data)
{
    LOG_MSGID_I(BT_APP, "bt_app_common_timer_test_cb, id is 0x%8x, data is 0x%4x\r\n", 2, timer_id, data);
}
#endif
static atci_status_t bt_app_comm_at_cmd_rho_hdl(atci_parse_cmd_param_t *parse_cmd)
{
#if defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    uint32_t response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    LOG_MSGID_I(BT_APP, "bt_app_comm_at_cmd_rho_hdl", 0);

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE: {
            /* AT+RHO */
            bt_status_t status = bt_role_handover_start();
            if (BT_STATUS_SUCCESS == status || BT_STATUS_BUSY == status) {
                response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else {
                LOG_MSGID_I(BT_APP, "bt_app_comm_at_cmd_rho_hdl fail 0x%x", 1, status);
            }

            break;
        }

        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+RHO=<action>
                        * action: "TIMER"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "TIMER", 5)) {
                /* START A TIMER */
                LOG_MSGID_I(BT_APP, "start an 20s test timer!", 0);
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
                bt_timer_ext_status_t timer_ret = BT_TIMER_EXT_STATUS_SUCCESS;
                timer_ret = bt_timer_ext_start(BT_EXT_TIMER_TEST_TIMER_ID, (uint32_t)g_timer_data, BT_EXT_TIMER_TEST_TIMER_DUR, bt_app_common_timer_test_cb);
                if (timer_ret != BT_TIMER_EXT_STATUS_SUCCESS) {
                    LOG_MSGID_I(BT_APP, "start test timer fail!", 0);
                }
                LOG_MSGID_I(BT_APP, "start test timer success, id is 0x%8x, data is 0x%4x\r\n", 2, BT_EXT_TIMER_TEST_TIMER_ID, g_timer_data);
                response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
#endif
            }

            break;
        }

        default:
            break;
    }

    atci_response_t *response = (atci_response_t *)pvPortMalloc(sizeof(atci_response_t));
    if (response != NULL) {
        memset(response, 0, sizeof(*response));
    } else {
        return ATCI_RESPONSE_FLAG_APPEND_ERROR;
    }

    response->response_flag = response_flag;
    response->response_len = strlen((char *)response->response_buf);
    atci_send_response(response);
    vPortFree(response);
#endif /*defined(SUPPORT_ROLE_HANDOVER_SERVICE)*/
    return ATCI_STATUS_OK;
}
#endif

static atci_status_t bt_app_comm_at_cmd_ble_scan_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+BLESCAN=<action>
                        * action: "ON" / "OFF"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                /* BLE SCAN OFF */
                bt_app_common_stop_scan();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                /* BLE SCAN ON */
                bt_app_common_start_scan();
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_ble_cancel_conn_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_ACTIVE: {
            /* AT+BLECANCELCONN*/
            bt_gap_le_cancel_connection();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_ble_random_addr_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+BLERANDOMADDR=<action>
                        * action: "GET"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "GET", 3)) {
                uint8_t temp_str[30] = {0};
                bt_bd_addr_t *addr = NULL;
                addr = (bt_bd_addr_t *)bt_app_common_get_local_random_addr();
                snprintf((char *)temp_str, sizeof(temp_str), "0x%.2X:%.2X:%.2X:%.2X:%.2X:%.2X",
                         (*addr)[5], (*addr)[4], (*addr)[3], (*addr)[2], (*addr)[1], (*addr)[0]);
                snprintf((char *)response.response_buf, sizeof(response.response_buf), "+Get addrss:%s\r\n", (char *)temp_str);
                response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            }

            break;
        }

        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_set_fast_pair_tx_power_level(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

#ifdef __BT_FAST_PAIR_ENABLE__
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            int8_t tx_power_level = atoi(parse_cmd->string_ptr + parse_cmd->name_len + 1);
            LOG_MSGID_I(BT_APP, "set fast pair tx power level :%d", 1, tx_power_level);

            app_fast_pair_set_tx_power_level(tx_power_level);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;
        }
        default:
            break;
    }
#endif

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

#ifdef MTK_IAP2_PROFILE_ENABLE

#define MAX_MFI_CERT_DATA_LEN 640
static atci_status_t bt_app_comm_at_cmd_get_mfi_cert_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    int                 r;
    uint8_t             *accessory_cert;
    uint32_t            accessory_cert_len = MAX_MFI_CERT_DATA_LEN;
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    /* Get mfi cerificate data */
    /* AT+GETMFICERT= */
    /* <ACTION>: GET */

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            accessory_cert = (uint8_t *)pvPortMalloc(MAX_MFI_CERT_DATA_LEN);
            if (!accessory_cert) {
                APPS_LOG_MSGID_I("malloc mfi cert data buf failed.", 0);
                break;
            }
            memset(accessory_cert, 0, MAX_MFI_CERT_DATA_LEN);

            r = mfi_coprocessor_get_accessory_cert(&accessory_cert, &accessory_cert_len);
            if (r < 0) {
                APPS_LOG_MSGID_I("get mfi cert data failed.", 0);
            } else {
                APPS_LOG_MSGID_I("get mfi cert data len: %d.", 1, accessory_cert_len);
                APPS_LOG_DUMP_I("mfi data:", accessory_cert, accessory_cert_len, 0);
            }
            vPortFree(accessory_cert);
        }
        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


#define MAX_MFI_RESP_DATA_LEN 64
#define MAX_MFI_CHALLENGE_DATA_LEN 32
static uint8_t mfi_challenge_data[MAX_MFI_CHALLENGE_DATA_LEN] = {0};
static atci_status_t bt_app_comm_at_cmd_get_mfi_response_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    int                 r;
    uint8_t             *resp_data;
    uint32_t            resp_data_len = MAX_MFI_RESP_DATA_LEN;
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    /* Get mfi challenge response data with a empt challenge data. */
    /* AT+GETMFIRESP= */
    /* <ACTION>: GET */

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            resp_data = (uint8_t *)pvPortMalloc(MAX_MFI_RESP_DATA_LEN);
            if (!resp_data) {
                APPS_LOG_MSGID_I("malloc mfi resp data buf failed.", 0);
                break;
            }
            memset(resp_data, 0, MAX_MFI_CERT_DATA_LEN);

            r = mfi_coprocessor_gen_challenge_response(mfi_challenge_data, MAX_MFI_CHALLENGE_DATA_LEN, &resp_data, &resp_data_len);
            if (r < 0) {
                APPS_LOG_MSGID_I("get mfi resp data failed.", 0);
            } else {
                APPS_LOG_MSGID_I("get mfi resp data len: %d.", 1, resp_data_len);
                APPS_LOG_DUMP_I("mfi data:", resp_data, resp_data_len, 0);
            }
            vPortFree(resp_data);
        }
        default:
            break;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}


#define REG_DATA_LEN_1 1
#define REG_DATA_LEN_4 4
static bool check_mfi_version_data(atci_response_t *response)
{
    uint8_t temp_buf[4] = {0};
    int32_t ret = 0;
    char *ret_str = NULL;

    /* get device version */
    ret = aacp_i2c_read(AACP_DEVICE_VERSION, temp_buf, REG_DATA_LEN_1);
    if (ret != REG_DATA_LEN_1) {
        ret_str = "FAIL: get device version fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x07) {
        ret_str = "FAIL: invalid device version";
        goto func_ret;
    }

    /* get authentication revision */
    memset(temp_buf, 0x00, 4);
    ret = aacp_i2c_read(AACP_FIRMWARE_VERSION, temp_buf, REG_DATA_LEN_1);
    if (ret != REG_DATA_LEN_1) {
        ret_str = "FAIL: get authentication version fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x01) {
        ret_str = "FIAL: invalid authentication version.";
        goto func_ret;
    }

    /* get authentication protocal major version */
    memset(temp_buf, 0x00, 4);
    ret = aacp_i2c_read(AACP_AUTH_PROTO_MAJOR_VERSION, temp_buf, REG_DATA_LEN_1);
    if (ret != REG_DATA_LEN_1) {
        ret_str = "FAIL: get authentication protocal major version fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x03) {
        ret_str = "FIAL: invalid authentication protocal major version.";
        goto func_ret;
    }

    /* get authentication protocal minor version */
    memset(temp_buf, 0x00, 4);
    ret = aacp_i2c_read(AACP_AUTH_PROTO_MINOR_VERSION, temp_buf, REG_DATA_LEN_1);
    if (ret != REG_DATA_LEN_1) {
        ret_str = "FAIL: get authentication protocal minor version fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x00) {
        ret_str = "FIAL: invalid authentication protocal minor version.";
        goto func_ret;
    }

    /* get device id */
    memset(temp_buf, 0x00, 4);
    ret = aacp_i2c_read(AACP_DEVICE_ID, temp_buf, REG_DATA_LEN_4);
    if (ret != REG_DATA_LEN_4) {
        ret_str = "FAIL: get device id fail.";
        goto func_ret;
    }
    if (temp_buf[0] != 0x00 || temp_buf[1] != 0x00 ||
        temp_buf[2] != 0x03 || temp_buf[3] != 0x00) {
        ret_str = "FIAL: invalid device id.";
        goto func_ret;
    }

func_ret:
    if (ret_str != NULL) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE, ret_str);
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        return false;
    }
    return true;
}


static bool check_mfi_certifacation_data(atci_response_t *response)
{
    uint32_t idx = 0;
    bool check_result = false;
    int32_t ret = 0;
    uint8_t *accessory_cert;
    uint32_t accessory_cert_len = MAX_MFI_CERT_DATA_LEN;

    /* request certification data */
    accessory_cert = (uint8_t *)pvPortMalloc(MAX_MFI_CERT_DATA_LEN);
    if (!accessory_cert) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: malloc memory fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    memset(accessory_cert, 0x00, MAX_MFI_CERT_DATA_LEN);
    ret = mfi_coprocessor_get_accessory_cert(&accessory_cert, &accessory_cert_len);
    if (ret < 0) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: get certification data fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    /* check certification data */
    //on aacp 3.0, the certification data length is between 607 and 609, inclusive.
    if (accessory_cert_len < 607 || accessory_cert_len > 609) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: certification data length is %d.", (int)accessory_cert_len);
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    for (idx = 0; idx < accessory_cert_len; idx++) {
        if (accessory_cert[idx] != 0x00) {
            break;
        }
    }
    if (idx == accessory_cert_len) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: certification data is padding by 0x00.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    if (accessory_cert != NULL) {
        vPortFree(accessory_cert);
        accessory_cert = NULL;
    }
    check_result = true;

func_ret:
    if (accessory_cert != NULL) {
        vPortFree(accessory_cert);
        accessory_cert = NULL;
    }
    return check_result;
}


static bool check_mfi_challenge_response(atci_response_t *response)
{
    bool check_result = false;
    int32_t ret = 0;
    uint32_t idx = 0;
    uint8_t *challenge_data = NULL;
    uint8_t *challenge_response_data = NULL;
    uint32_t challenge_response_len = 0;

    /* send dummy challenge data to chip */
    challenge_data = (uint8_t *)pvPortMalloc(MAX_MFI_CHALLENGE_DATA_LEN);
    if (!challenge_data) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: malloc memory fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    } else {
        memset(challenge_data, 0x77, MAX_MFI_CHALLENGE_DATA_LEN);
    }

    challenge_response_data = (uint8_t *)pvPortMalloc(MAX_MFI_RESP_DATA_LEN);
    if (!challenge_response_data) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: malloc memory fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    } else {
        memset(challenge_response_data, 0x00, MAX_MFI_RESP_DATA_LEN);
    }

    ret = mfi_coprocessor_gen_challenge_response(challenge_data,
                                                 MAX_MFI_CHALLENGE_DATA_LEN,
                                                 &challenge_response_data,
                                                 &challenge_response_len);
    if (ret < 0) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: get challenge response data fail.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    /* check challenge response data */
    // on aacp 3.0, the challenge response data length is 64
    if (challenge_response_len != MAX_MFI_RESP_DATA_LEN) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: challenge response data length is %d.", (int)challenge_response_len);
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    for (idx = 0; idx < challenge_response_len; idx++) {
        if (challenge_response_data[idx] != 0x00) {
            break;
        }
    }
    if (idx == challenge_response_len) {
        snprintf((char *)response->response_buf, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "FAIL: challenge response data is padding by 0x00.");
        response->response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        goto func_ret;
    }

    if (challenge_data != NULL) {
        vPortFree(challenge_data);
        challenge_data = NULL;
    }

    if (challenge_response_data != NULL) {
        vPortFree(challenge_response_data);
        challenge_response_data = NULL;
    }

    check_result = true;

func_ret:
    if (challenge_data != NULL) {
        vPortFree(challenge_data);
        challenge_data = NULL;
    }
    if (challenge_response_data != NULL) {
        vPortFree(challenge_response_data);
        challenge_response_data = NULL;
    }
    return check_result;
}


static atci_status_t bt_app_comm_at_cmd_check_mfi_chip_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

    /* Check the mfi chip states */
    /* AT+CHECKMFICHIP */
    /* <ACTION>: GET */

    do {
        if (!check_mfi_version_data(&response)) {
            break;
        }

        if (!check_mfi_certifacation_data(&response)) {
            break;
        }

        if (!check_mfi_challenge_response(&response)) {
            break;
        }
    } while (0);

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

#endif

static atci_status_t bt_app_comm_at_cmd_ext_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t *response = pvPortMalloc(sizeof(atci_response_t));
    if (response == NULL) {
        return ATCI_STATUS_ERROR;
    }
    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            /* AT+EXTBLEADV=<action>,<instance>
                        * action: "ON" / "OFF"
                        */
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "OFF", 3)) {
                /* BLE ADV OFF */
                uint8_t instance = (uint8_t)strtoul(parse_cmd->string_ptr + parse_cmd->name_len + 5, NULL, 16);
                bt_status_t status = bt_app_common_ext_advertising_stop_test(instance);
                if (status == BT_STATUS_SUCCESS) {
                    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, "ON", 2)) {
                /* BLE ADV ON */
                uint8_t instance = (uint8_t)strtoul(parse_cmd->string_ptr + parse_cmd->name_len + 4, NULL, 16);
                bt_status_t status = bt_app_common_ext_advertising_start_test(instance);
                if (status == BT_STATUS_SUCCESS) {
                    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                }
            }
            break;
        }

        default:
            break;
    }

    response->response_len = strlen((char *)response->response_buf);
    atci_send_response(response);
    vPortFree(response);
    return ATCI_STATUS_OK;
}


static atci_status_t bt_app_comm_at_cmd_hfp_codec_set_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    char *token = strtok(parse_cmd->string_ptr, ",");
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR};

    if (strstr(token, "set") != NULL) {
        token = strtok(NULL, ",");

        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        if (strstr(token, "CVSD") != NULL) {
            update_hfp_audio_codec(BT_HFP_CODEC_TYPE_CVSD);
        } else if (strstr(token, "MSBC") != NULL) {
            update_hfp_audio_codec(BT_HFP_CODEC_TYPE_MSBC);
        } else {
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            char *pstr = (char *)response.response_buf;
            snprintf(pstr, ATCI_UART_TX_FIFO_BUFFER_SIZE, "unknown type, ");
        }

        response.response_len = strlen((char *)response.response_buf);
    } else {
        bt_hfp_audio_codec_type_t type = get_hfp_audio_codec();
        char *pstr = (char *)response.response_buf;
        snprintf(pstr, ATCI_UART_TX_FIFO_BUFFER_SIZE,
                 "codec mask: 0x%x", type);

        response.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
        response.response_len = strlen((char *)response.response_buf);
    }
    atci_send_response(&response);
    return ATCI_STATUS_OK;

}


static void bt_app_comm_at_cmd_copy_str_to_addr(uint8_t *addr, const char *str)
{
    unsigned int i, value;
    int using_long_format = 0;
    int using_hex_sign = 0;

    if (str[2] == ':' || str[2] == '-') {
        using_long_format = 1;
    }

    if (str[1] == 'x') {
        using_hex_sign = 2;
    }

    for (i=0; i<6; i++) {
        sscanf(str+using_hex_sign+i*(2+using_long_format), "%02x", &value);
        addr[5-i] = (uint8_t) value;
    }
}
void bt_app_comm_at_cmd_init(void)
{
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    bt_role_handover_callbacks_t callbacks = {0};
    callbacks.status_cb = bt_app_common_at_cmd_rho_srv_status_callback;
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_BT_CMD, &callbacks);
#endif

    atci_register_handler(bt_app_comm_at_cmd, sizeof(bt_app_comm_at_cmd) / sizeof(atci_cmd_hdlr_item_t));
}

