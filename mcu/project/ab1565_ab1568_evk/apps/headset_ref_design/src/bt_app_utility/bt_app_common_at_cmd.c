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

#ifdef AIR_LE_AUDIO_ENABLE
#include "nvkey.h"
#include "nvkey_id_list.h"
#include "multi_ble_adv_manager.h"
#include "bt_device_manager_le.h"
#include "bt_sink_srv_le_cap_stream.h"
#include "bt_sink_srv_le_cap.h"
#include "ble_csis.h"
#include "bt_le_audio_sink.h"
#include "app_le_audio.h"
#include "bt_sink_srv_le_volume.h"
#endif

#ifdef MTK_IAP2_PROFILE_ENABLE
#include "aacp_i2c.h"
#endif


extern bt_status_t bt_app_common_advtising_stop(void);
static atci_status_t bt_app_comm_at_cmd_ble_adv_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_fota_hdl(atci_parse_cmd_param_t *parse_cmd);
#if defined(MTK_AWS_MCE_ENABLE)
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

#if defined(AIR_LE_AUDIO_ENABLE)
#define BLE_CCP_PARM_MAX_LEN     20

extern bt_status_t ble_csis_get_sirk(bt_key_t *sirk);
extern void ble_csis_write_nvkey_sirk(bt_key_t *sirk);
static atci_status_t bt_app_comm_at_cmd_le_audio_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ble_remove_bonded_device_hdl(atci_parse_cmd_param_t *parse_cmd);
static atci_status_t bt_app_comm_at_cmd_ble_random_addr_hdl(atci_parse_cmd_param_t *parse_cmd);
#endif /* AIR_LE_AUDIO_ENABLE */

static atci_status_t bt_app_comm_at_cmd_hfp_codec_set_hdl(atci_parse_cmd_param_t *parse_cmd);


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
    }
#ifdef AIR_LE_AUDIO_ENABLE
    ,
    {
        .command_head = "AT+LEAUDIO",       /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_le_audio_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
    {
        .command_head = "AT+BLERMBOND",     /**< AT command string. */
        .command_hdlr = bt_app_comm_at_cmd_ble_remove_bonded_device_hdl,
        .hash_value1 = 0,
        .hash_value2 = 0,
    },
#endif
};
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
                APPS_LOG_DUMP_I("mfi data:", accessory_cert, accessory_cert_len);
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
                APPS_LOG_DUMP_I("mfi data:", resp_data, resp_data_len);
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

#ifdef AIR_LE_AUDIO_ENABLE
static atci_status_t bt_app_comm_at_cmd_ble_remove_bonded_device_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    char *pChar = NULL;
    uint8_t buf[BT_BD_ADDR_LEN+1] = {0};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        pChar = parse_cmd->string_ptr + parse_cmd->name_len + 1;

        if (0 == memcmp(pChar, "ADDR", 4)) {
            uint8_t i;

            /* AT+BLERMBOND=ADDR,<addrType>,<addr0, addr1, addr2, addr3, addr4, addr5> */
            pChar = strtok(pChar, ",");
            for (i = 0; i < (BT_BD_ADDR_LEN+1); i++) {
                pChar = strtok(NULL, ",");
                buf[i] = atoi(pChar);
            }
            LOG_MSGID_I(BT_APP, "[AT_CMD] BLE Remove bonded device, type:%x Addr:%x %x %x %x %x %x", 7, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6]);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

        } else if (0 == memcmp(pChar, "ALL", 3)){
            bt_device_manager_le_clear_all_bonded_info();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

        } else if (0 == memcmp(pChar, "GET", 3)){
            uint8_t count = bt_device_manager_le_get_bonded_number();
            bt_bd_addr_t *peer_addr = (bt_bd_addr_t *)pvPortMalloc(count * sizeof(bt_bd_addr_t));
            if (peer_addr) {
                bt_device_manager_le_get_bonded_list(peer_addr, &count);
            } else {
                count = 0;
            }
            vPortFree(peer_addr);
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        }
        if (response.response_flag == ATCI_RESPONSE_FLAG_APPEND_OK) {
            bt_device_manager_le_remove_bonded_device((bt_addr_t *)buf);
        }
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_le_audio_feature_hdl(char *pChar)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    /* AT+LEAUDIO=FEATURE,<ACTION> */
    /* <ACTION>: ON, OFF */
    if (0 == memcmp(pChar, "ON", 2)) {
        app_le_audio_enable(true);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

    } else if (0 == memcmp(pChar, "OFF", 3)) {
        app_le_audio_enable(false);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_le_audio_adv_hdl(char *pChar)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    uint8_t low_power_mode = 0;
    uint32_t size = sizeof(low_power_mode);

    /* Read system mode is in low power mode or not. */
    if ((NVKEY_STATUS_OK != nvkey_read_data(NVKEYID_SYSTEM_MODE_SETTING, &low_power_mode, &size)) || (0 == low_power_mode)) {
        LOG_MSGID_I(BT_APP, "[AT_CMD] NOT in low power mode! AT+LEAUDIO=ADV can only be used in low power mode", 0);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    } else {
        /* AT+LEAUDIO=ADV,<ACTION> */
        /* <ACTION>: ON, OFF, RESUME */
        if (0 == memcmp(pChar, "ON", 2)) {
            multi_ble_adv_manager_start_ble_adv();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

        } else if (0 == memcmp(pChar, "OFF", 3)) {
            multi_ble_adv_manager_pause_ble_adv();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

        } else if (0 == memcmp(pChar, "RESUME", 6)) {
            multi_ble_adv_manager_resume_ble_adv();
            response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        }
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static atci_status_t bt_app_comm_at_cmd_le_audio_sirk_hdl(char *pChar)
{
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    bt_key_t sirk = {0};

    /* SIRK */
    /* AT+LEAUDIO=SIRK,<ACTION> */
    /* <ACTION>: SET, GET */

    if (0 == memcmp(pChar, "GET", 3)) {
        uint8_t temp_str[50] = {0};
        ble_csis_get_sirk(&sirk);
        snprintf((char *)temp_str, sizeof(temp_str), "%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X,%.2X",
                    sirk[0], sirk[1], sirk[2], sirk[3], sirk[4], sirk[5], sirk[6], sirk[7],
                    sirk[8], sirk[9], sirk[10], sirk[11], sirk[12], sirk[13], sirk[14], sirk[15]);
        snprintf((char *)response.response_buf, sizeof(response.response_buf), "SIRK:%s\r\n", (char *)temp_str);
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

    } else if (0 == memcmp(pChar, "SET", 3)) {
        /* AT+LEAUDIO=SIRK,SET,<B0>,<B1>,<B2>,<B3>,<B4>,<B5>,<B6>,<B7>,<B8>,<B9>,<B10>,<B11>,<B12>,<B13>,<B14>,<B15> */
        pChar = strchr(pChar, ',');
        pChar++;

        if (sscanf(pChar, "%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x,%2x",
            (unsigned int *)&sirk[0], (unsigned int *)&sirk[1], (unsigned int *)&sirk[2], (unsigned int *)&sirk[3],
            (unsigned int *)&sirk[4], (unsigned int *)&sirk[5], (unsigned int *)&sirk[6], (unsigned int *)&sirk[7],
            (unsigned int *)&sirk[8], (unsigned int *)&sirk[9], (unsigned int *)&sirk[10], (unsigned int *)&sirk[11],
            (unsigned int *)&sirk[12], (unsigned int *)&sirk[13], (unsigned int *)&sirk[14], (unsigned int *)&sirk[15]) > 0) {

            LOG_MSGID_I(common, "[SIRK] SET:%x-%x-%x-%x-%x-%x-%x-%x" , 8, sirk[0], sirk[1], sirk[2], sirk[3], sirk[4], sirk[5], sirk[6], sirk[7]);
            LOG_MSGID_I(common, "[SIRK]:%x-%x-%x-%x-%x-%x-%x-%x" , 8, sirk[8], sirk[9], sirk[10], sirk[11], sirk[12], sirk[13], sirk[14], sirk[15]);
            ble_csis_write_nvkey_sirk(&sirk);
        }
        response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
    }

    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
    return ATCI_STATUS_OK;
}

static bt_status_t bt_app_comm_at_cmd_le_audio_media_hdl(char *pChar)
{
    bt_status_t result = BT_STATUS_UNSUPPORTED;
    bt_handle_t handle = bt_sink_srv_cap_get_link_handle(0xFF);
    bt_le_audio_sink_action_param_t le_param = {
        .service_idx = BLE_MCP_GMCS_INDEX,
    };

    /* AT+LEAUDIO=MEDIA,<ACTION> */
    /* <ACTION>: PLAY, PAUSE, NEXT, PREV */

    if (0 == memcmp(pChar, "PLAY", 4)) {
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PLAY, &le_param);

    } else if (0 == memcmp(pChar, "PAUSE", 5)) {
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PAUSE, &le_param);

    } else if (0 == memcmp(pChar, "NEXT", 4)) {
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_NEXT_TRACK, &le_param);

    } else if (0 == memcmp(pChar, "PREV", 4)) {
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_MEDIA_PREVIOUS_TRACK, &le_param);
    }

    return result;
}

static bt_status_t bt_app_comm_at_cmd_le_audio_call_hdl(char *pChar)
{
    bt_status_t result = BT_STATUS_UNSUPPORTED;
    bt_handle_t handle = bt_sink_srv_cap_get_link_handle(0xFF);
    uint8_t buf[BLE_CCP_PARM_MAX_LEN] = {0};

    /* AT+LEAUDIO=CALL,<ACTION> */
    /* <ACTION>: ACCEPT, TERMINATE, PLACE */
    if (0 == memcmp(pChar, "ACCEPT", 6)) {
        bt_le_audio_sink_call_action_param_t le_param = {
            .service_idx = BLE_CCP_GTBS_INDEX,
            .length = 2,
        };
        le_param.call_control_point = (ble_tbs_call_control_point_t*)buf;
        buf[0] = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_ACCEPT;
        buf[1] = 1; /* call index */
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_CALL_ACCEPT, &le_param);

    } else if (0 == memcmp(pChar, "TERMINATE", 9)) {
        bt_le_audio_sink_call_action_param_t le_param = {
            .service_idx = BLE_CCP_GTBS_INDEX,
            .length = 2,
        };
        le_param.call_control_point = (ble_tbs_call_control_point_t*)buf;
        buf[0] = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_TERMINATE;
        buf[1] = 1; /* call index */
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_CALL_TERMINATE, &le_param);

    } else if (0 == memcmp(pChar, "PLACE", 5)) {
        bt_le_audio_sink_call_action_param_t le_param = {
            .service_idx = BLE_CCP_GTBS_INDEX,
            .length = 5,
        };
        le_param.call_control_point = (ble_tbs_call_control_point_t*)buf;
        buf[0] = BLE_TBS_CALL_CONTROL_OPCODE_TYPE_ORIGINATE;
        buf[1] = 3; /* uri length */
        buf[2] = 1; /* uri */
        buf[3] = 2; /* uri */
        buf[4] = 3; /* uri */
        result = bt_le_audio_sink_send_action(handle, BT_LE_AUDIO_SINK_ACTION_CALL_ORIGINATE, &le_param);

    }
    LOG_MSGID_I(BT_APP, "[AT_CMD] Call result:%x \r\n", 1, result);

    return result;
}

static bool bt_app_comm_at_cmd_le_audio_volume_hdl(char *pChar)
{
    bt_handle_t handle = bt_sink_srv_cap_get_link_handle(0xFF);

    /* AT+LEAUDIO=VOLUME,<ACTION> */
    /* <ACTION>: MUTE, UNMUTE, UP, DOWN, SET */
    if (0 == memcmp(pChar, "MUTE", 4)) {  
	   bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_MUTE, NULL);
	} else if (0 == memcmp(pChar, "UNMUTE", 6)) {
		bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_UNMUTE, NULL);
	} else if (0 == memcmp(pChar, "UP", 2)) {
		bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_UP, NULL);
	} else if (0 == memcmp(pChar, "DOWN", 4)) {
		bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_UNMUTE_RELATIVE_VOLUME_DOWN, NULL);
    } else if (0 == memcmp(pChar, "SET", 3)) {
         /* AT+LEAUDIO=VOLUME,SET,<volume> */
        bt_sink_srv_le_set_absolute_volume_t param;
        char *buf = (pChar + 4);
        param.volume = (uint8_t)strtoul(buf, NULL, 10);
        if (param.volume > 15) {
            return false;
        }
        param.volume *= BT_SINK_LE_VOLUME_VALUE_STEP;
        if (BT_STATUS_SUCCESS != bt_sink_srv_le_volume_vcp_send_action(handle, BT_SINK_SRV_LE_VCS_ACTION_SET_ABSOLUTE_VOLUME, &param)) {
            return false;
        }
    }
    return true;
}

static bool bt_app_comm_at_cmd_le_audio_broadcast_hdl(char *pChar)
{
    /* AT+LEAUDIO=BROADCAST,<ACTION> */
    /* <ACTION>: SCAN,L  STOP */
    bt_status_t status = BT_STATUS_SUCCESS;
    if (0 == memcmp(pChar, "SCAN,L", 6)) {
        bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
        uint8_t bis_indices[2] = {0};

        scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_LEFT;
        scan_param.bms_address = NULL;
        bt_sink_srv_cap_stream_set_big_sync_info(1, 2, bis_indices);

        status = bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
    } else if(0 == memcmp(pChar, "L,", 2)) {
        bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
        bt_bd_addr_t addr = {0};
        uint16_t addr_5,addr_4,addr_3,addr_2,addr_1,addr_0;
        pChar = strchr(pChar, ',');
        pChar++;

        if (sscanf(pChar, "%2x:%2x:%2x:%2x:%2x:%2x", (unsigned int *)&addr_5, (unsigned int *)&addr_4,
                (unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0) > 0) {

            scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_LEFT;
            addr[5] = addr_5;
            addr[4] = addr_4;
            addr[3] = addr_3;
            addr[2] = addr_2;
            addr[1] = addr_1;
            addr[0] = addr_0;
            scan_param.bms_address = (bt_bd_addr_t*)&addr[0];

            LOG_MSGID_I(common, "addr:%x-%x-%x-%x-%x-%x" , 6,
                        addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

            status = bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
        }
    } else if(0 == memcmp(pChar, "R,", 2)) {
        bt_sink_srv_cap_stream_bmr_scan_param_t scan_param = {0};
        bt_bd_addr_t addr = {0};
        uint16_t addr_5,addr_4,addr_3,addr_2,addr_1,addr_0;
        pChar = strchr(pChar, ',');
        pChar++;

        if (sscanf(pChar, "%2x:%2x:%2x:%2x:%2x:%2x", (unsigned int *)&addr_5, (unsigned int *)&addr_4,
                (unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0) > 0) {

            scan_param.audio_channel_allocation = AUDIO_LOCATION_FRONT_RIGHT;
            addr[5] = addr_5;
            addr[4] = addr_4;
            addr[3] = addr_3;
            addr[2] = addr_2;
            addr[1] = addr_1;
            addr[0] = addr_0;
            scan_param.bms_address = (bt_bd_addr_t*)&addr[0];

            LOG_MSGID_I(common, "addr:%x-%x-%x-%x-%x-%x" , 6,
                        addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

            status = bt_sink_srv_cap_stream_scan_broadcast_source(&scan_param);
        }
    } else if (0 == memcmp(pChar, "STOP", 4)) {
        uint8_t bis_indices[2] = {0};

        bt_sink_srv_cap_stream_stop_broadcast_reception();

        bt_sink_srv_cap_stream_set_big_sync_info(1, 2, bis_indices);

    } else if (0 == memcmp(pChar, "CONFIG", 6)) {
        uint8_t num_bis, big_handle;
        uint8_t bis_indices[2] = {0};

        pChar = strchr(pChar, ',');
        pChar++;

        num_bis = atoi(pChar);

        if(num_bis > 2)
            return false;

        pChar = strchr(pChar, ',');
        pChar++;

        bis_indices[0] = atoi(pChar);

        if(num_bis == 2) {
            pChar = strchr(pChar, ',');
            pChar++;

            bis_indices[1] = atoi(pChar);
        }

        LOG_MSGID_I(common, "[BIS] config sync big info big_handle[%d] bis_indices[%d %d]", 3, big_handle, bis_indices[0], bis_indices[1]);

        bt_sink_srv_cap_stream_set_big_sync_info(1, num_bis, bis_indices);
    } else
        return false;

    if(status != BT_STATUS_SUCCESS)
        return false;
    else
        return true;
}

static atci_status_t bt_app_comm_at_cmd_le_audio_hdl(atci_parse_cmd_param_t *parse_cmd)
{
    atci_status_t ret_at = ATCI_STATUS_ERROR;
    char *pChar = NULL;
    atci_response_t *response = NULL;
    response = (atci_response_t*)pvPortMalloc(sizeof(atci_response_t));
    if (response == NULL) {
        return ATCI_STATUS_ERROR;
    }
    memset(response, 0, sizeof(atci_response_t));
    response->response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;


    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        pChar = parse_cmd->string_ptr + parse_cmd->name_len + 1;

        if (0 == memcmp(pChar, "MEDIA", 5)) {
            /* Media */
            /* AT+LEAUDIO=MEDIA,<ACTION> */
            /* <ACTION>: PLAY, PAUSE, NEXT, PREV */
            pChar = strchr(pChar, ',');
            pChar++;
            if (BT_STATUS_SUCCESS == bt_app_comm_at_cmd_le_audio_media_hdl(pChar)) {
                ret_at = ATCI_STATUS_OK;
            }

        } else if (0 == memcmp(pChar, "VOLUME", 6)) {
            /* Volume */
            /* AT+LEAUDIO=VOLUME,<ACTION> */
            /* <ACTION>: MUTE, UNMUTE, UP, DOWN, SET */
            pChar = strchr(pChar, ',');
            pChar++;
            if (!bt_app_comm_at_cmd_le_audio_volume_hdl(pChar)) {
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            } else {
                ret_at = ATCI_STATUS_OK;
            }
        } else if (0 == memcmp(pChar, "CALL", 4)) {
            /* Call */
            /* AT+LEAUDIO=CALL,<ACTION> */
            /* <ACTION>: ACCEPT, TERMINATE, PLACE */
            pChar = strchr(pChar, ',');
            pChar++;
            if (BT_STATUS_SUCCESS == bt_app_comm_at_cmd_le_audio_call_hdl(pChar)) {
                ret_at = ATCI_STATUS_OK;
            }

        } else if (0 == memcmp(pChar, "SIRK", 4)) {
            /* SIRK */
            /* AT+LEAUDIO=SIRK,<ACTION> */
            /* <ACTION>: SET, GET */
            pChar = strchr(pChar, ',');
            pChar++;
            ret_at = bt_app_comm_at_cmd_le_audio_sirk_hdl(pChar);

        } else if (0 == memcmp(pChar, "BROADCAST", 9)) {
            /* Broadcast */
            /* AT+LEAUDIO=BROADCAST,<ACTION> */
            /* <ACTION>: SCAN, STOP */
            pChar = strchr(pChar, ',');
            pChar++;
            if (!bt_app_comm_at_cmd_le_audio_broadcast_hdl(pChar)) {
                response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
            } else {
                ret_at = ATCI_STATUS_OK;
            }
        } else if (0 == memcmp(pChar, "ADV", 3)) {
            /* Enalbe LE Audio ADV, low power mode use only */
            /* AT+LEAUDIO=ADV,<ACTION> */
            /* <ACTION>: ON, OFF, RESUME */
            pChar = strchr(pChar, ',');
            pChar++;
            ret_at = bt_app_comm_at_cmd_le_audio_adv_hdl(pChar);

        } else if (0 == memcmp(pChar, "FEATURE", 7)) {
            /* LE Audio feature on/off */
            /* AT+LEAUDIO=FEATURE,<ACTION> */
            /* <ACTION>: ON, OFF */
            pChar = strchr(pChar, ',');
            pChar++;
            ret_at = bt_app_comm_at_cmd_le_audio_feature_hdl(pChar);

        } else if (0 == memcmp(pChar, "RESYNC", 6)) {
            /* Resync operation*/
            /* AT+LEAUDIO=RESYNC */
            bt_sink_srv_cap_stream_restart_streaming();

        } else {
            response->response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
        }
    }

    response->response_len = strlen((char *)response->response_buf);
    atci_send_response(response);
    vPortFree(response);
    return ret_at;
}
#endif  /* AIR_LE_AUDIO_ENABLE */

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


void bt_app_comm_at_cmd_init(void)
{
#if defined(MTK_AWS_MCE_ENABLE) && defined(SUPPORT_ROLE_HANDOVER_SERVICE)
    bt_role_handover_callbacks_t callbacks = {0};
    callbacks.status_cb = bt_app_common_at_cmd_rho_srv_status_callback;
    bt_role_handover_register_callbacks(BT_ROLE_HANDOVER_MODULE_BT_CMD, &callbacks);
#endif

    atci_register_handler(bt_app_comm_at_cmd, sizeof(bt_app_comm_at_cmd) / sizeof(atci_cmd_hdlr_item_t));
}

