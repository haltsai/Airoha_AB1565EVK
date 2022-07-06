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
 * File:bt_at_cmd.c
 *
 * Description: The file is to parse atci cmd for bt low power test.
 *
 */

#include "bt_gap_le.h"
#include "bt_gap.h"
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "atci.h"
#include "syslog.h"

#include "FreeRTOS.h"
#include "task.h"
#include "task_def.h"
#include "bt_platform.h"

#include "bt_init.h"
#include "bt_sink_srv_am_task.h"
#include "bt_hfp_codec_internal.h"
#include "audio_middleware_api.h"
#include "bt_connection_manager_internal.h"
#include "multi_ble_adv_manager.h"

#ifndef ATCI_DEMO
#define ATCI_DEMO
#endif
#ifndef BT_SINK_DEMO
#define BT_SINK_DEMO
#endif

/**
*  @brief A structure to represent low power task information.
*/
typedef struct _lp_tasks_list_ {
    TaskFunction_t      pvTaskCode;    /**< the task function */
    char                *pcName;       /**< the name of task */
    uint16_t            usStackDepth;  /**< the stack depth of task */
    void                *pvParameters; /**< the parameters of task */
    UBaseType_t         uxPriority;    /**< the priority of task */
} lp_tasks_list_t;

/**
 * @brief     This function is bsp bt codec task.
 * @param[in] arg       parameters to pass in when start to run this task.
 * @return    void.
 */
extern void bsp_bt_codec_task_main(void *arg);

/**
 * @brief     This function is am task.
 * @param[in] arg       parameters to pass in when start to run this task.
 * @return    void.
 */
extern void am_task_main(void *arg);

/**
 * @brief     This function is bt task.
 * @param[in] arg       parameters to pass in when start to run this task.
 * @return    void.
 */
extern void bt_task(void *arg);

/**
*  @brief The defination of bt low power tasks list.
*/
static const lp_tasks_list_t bt_lp_list[] = {
    { bt_task,      BT_TASK_NAME,   BT_TASK_STACKSIZE,  NULL,   BT_TASK_PRIORITY },
#ifdef BT_SINK_DEMO
    { am_task_main, AM_TASK_NAME,   AM_TASK_STACKSIZE,  NULL,   AM_TASK_PRIO },
#endif
};

#define lp_list_count  (sizeof(bt_lp_list) / sizeof(lp_tasks_list_t))  /* Count of low power tasks. */

static TaskHandle_t     lp_handles[lp_list_count];  /* Task handles for each low power task. */

void lp_task_create(void)
{
    uint16_t            i;
    BaseType_t          x;
    const lp_tasks_list_t  *t;
    for (i = 0; i < lp_list_count; i++) {
        t = &bt_lp_list[i];
        LOG_I(common, "xCreate task %s, pri %d", t->pcName, (int)t->uxPriority);
        x = xTaskCreate(t->pvTaskCode,
                        t->pcName,
                        t->usStackDepth,
                        t->pvParameters,
                        t->uxPriority,
                        &lp_handles[i]);
        if (x != pdPASS) {
            /* Print log to notify user that create task failed. */
            LOG_MSGID_E(common, ": failed", 0);
        } else {
            /* Print log to notify user that create task successed. */
            LOG_MSGID_I(common, ": successed", 0);
        }
    }
    LOG_MSGID_I(common, "Free Heap size:%d bytes", 1, xPortGetFreeHeapSize());
}

#ifdef HAL_SLEEP_MANAGER_ENABLED
extern atci_status_t atci_cmd_hdlr_sleep_manager(atci_parse_cmd_param_t *parse_cmd);
#endif

static void bt_common_convert_str_hex(const char *str, char *output, uint8_t len)
{
    int32_t i = 0;
    char tempbuf[2];
    while (len) {
        memcpy(tempbuf, (str + (i * 2)), 2);
        output[i] = strtol(tempbuf, NULL, 16);
        len = len - 2 ;
        i++;
    }
}

static bool bt_atci_set_advertise(char *string)
{
    uint32_t adv_type;
    uint32_t min_interval;
    uint32_t max_interval;
    uint32_t own_addr_type;
    uint32_t is_enable;
    uint8_t buff[32] = {0};
    bt_hci_cmd_le_set_advertising_data_t adv_data = {0};
    char string_buff[31 * 2];
    uint8_t len;
    if (sscanf(string, "%u%*c%u%*c%u%*c%u%*c%u,%62s", (unsigned int *)&is_enable, (unsigned int *)&min_interval, (unsigned int *)&max_interval, (unsigned int *)&adv_type, (unsigned int *)&own_addr_type, (char *)string_buff) > 0) {
        /* Get value(advertising config) in target format successed. */
        LOG_MSGID_I(common, "enable: %d, adv type %d, min_interval %d, max_interval  %d, onw_addr_type %d", 5, is_enable, adv_type, min_interval, max_interval, own_addr_type);
        bt_hci_cmd_le_set_advertising_enable_t adv_enable;
        bt_hci_cmd_le_set_advertising_parameters_t adv_para = {
            .advertising_interval_min = min_interval,
            .advertising_interval_max = max_interval,
            .advertising_type = adv_type,
            .own_address_type         = own_addr_type,
            .advertising_channel_map = 7,
            .advertising_filter_policy = 1
        };
        adv_enable.advertising_enable = is_enable;
        len = strlen(string_buff);
        LOG_I(common, "adv data %s, len %d", string_buff, len);
        bt_common_convert_str_hex(string_buff, (char *)buff, len);
        adv_data.advertising_data_length = len / 2;
        memcpy(adv_data.advertising_data, buff, len / 2);
        bt_gap_le_set_advertising(&adv_enable, &adv_para, &adv_data, NULL);
        return true;
    } else {
        /* Get value in target format failed. */
        return false;
    }
}

extern bt_status_t bt_driver_set_tx_power(uint8_t *tx_power);

static bool bt_atci_parse_cmd(char *string)
{
    uint16_t addr_5, addr_4, addr_3, addr_2, addr_1, addr_0;
    bool result = true;
    if (strstr(string, "AT+EBTA2DP_LPWR=") != 0) {
        /* The string is matched with "AT+EBTA2DP_LPWR=". */
        bt_status_t bt_hci_send_vendor_cmd(uint16_t cmd_code, uint8_t *param, uint8_t param_length);
        char *value_lpr = string + strlen("AT+EBTA2DP_LPWR=");
        uint8_t lpr_flag = *value_lpr - 0x30;
        if (lpr_flag != 0) {
            lpr_flag = 1;
        }
        bt_hci_send_vendor_cmd(0xFC4E, &lpr_flag, sizeof(lpr_flag));
    } else if (strstr(string, "AT+EBTTEST_DEMO=1") != 0) {
        /* The string is matched with "AT+EBTTEST_DEMO=1" */
        LOG_MSGID_I(common, "enter low power", 0);
        bt_gap_default_sniff_params_t info = {0};
        /* Power on BT. */
        //lp_task_create();
        bt_demo_power_on();
        info.max_sniff_interval = 2048;
        info.min_sniff_interval = 2048;
        info.sniff_attempt = 4;
        info.sniff_timeout = 1;
        bt_gap_set_default_sniff_parameters(&info);
        log_config_print_switch(BTHCI, DEBUG_LOG_OFF);
        log_config_print_switch(BTL2CAP, DEBUG_LOG_OFF);
        log_config_print_switch(BTRFCOMM, DEBUG_LOG_OFF);
        log_config_print_switch(BTSPP, DEBUG_LOG_OFF);
        log_config_print_switch(BT, DEBUG_LOG_OFF);
        log_config_print_switch(hal, DEBUG_LOG_OFF);
        log_config_print_switch(atci, DEBUG_LOG_OFF);
    } else if (strstr(string, "AT+EBTTEST_DEMO=0") != 0) {
        /* The string is matched with "AT+EBTTEST_DEMO=0",to power off BT. */
        bt_demo_power_off();
    }
    /* Reture directly if not enter BT low power, avoiding exception. */
    if (strstr(string, "AT+EBLESA_DEMO=1") != 0) {
        /* The string is matched with "AT+EBLESA_DEMO=1",to start advertising. */
        bt_hci_cmd_le_set_advertising_enable_t adv_enable;
        bt_hci_cmd_le_set_advertising_parameters_t adv_para = {
            .advertising_interval_min = 0x0800,
            .advertising_interval_max = 0x0800,
            .advertising_type = BT_HCI_ADV_TYPE_CONNECTABLE_UNDIRECTED,
            .advertising_channel_map = 7,
            .advertising_filter_policy = 1
        };
        adv_enable.advertising_enable = BT_HCI_ENABLE;
        bt_gap_le_set_advertising(&adv_enable, &adv_para, NULL, NULL);
    } else if (strstr(string, "AT+EBLESA_DEMO=0") != 0) {
        /* The string is matched with "AT+EBLESA_DEMO=0",to stop advertising. */
        bt_hci_cmd_le_set_advertising_enable_t enable;
        enable.advertising_enable = BT_HCI_DISABLE;
        bt_gap_le_set_advertising(&enable, NULL, NULL, NULL);
        multi_ble_adv_manager_pause_ble_adv();
    } else if (strstr(string, "AT+EBLESEA_DEMO=") != 0) {
        /* The string is matched with "AT+EBLESEA_DEMO=",to set advertising(start/stop/config). */
        result = bt_atci_set_advertise(string + strlen("AT+EBLESEA_DEMO="));
    } else if (strstr(string, "AT+EBLESAD_DEMO=") != 0) {
        /* The string is matched with "AT+EBLESAD_DEMO=", to set random bd address, avoid memory address's end is 1. */
        bt_bd_addr_t addr;
        if (sscanf(string + strlen("AT+EBLESAD_DEMO="), "%2x%*c%2x%*c%2x%*c%2x%*c%2x%*c%2x", (unsigned int *)&addr_5,
                   (unsigned int *)&addr_4, (unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0) > 0) {
            /* Get value(random address) in target format successed. */
            addr[5] = addr_5;
            addr[4] = addr_4;
            addr[3] = addr_3;
            addr[2] = addr_2;
            addr[1] = addr_1;
            addr[0] = addr_0;
            LOG_MSGID_I(common, "addr:%x-%x-%x-%x-%x-%x", 6, addr[5],  addr[4], addr[3], addr[2], addr[1], addr[0]);
            bt_gap_le_set_random_address((bt_bd_addr_ptr_t)&addr);
        } else {
            /* Get value in target format failed. */
            result = false;
        }
    } else if (strstr(string, "AT+EBTES_DEMO=") != 0) {
        /* The string is matched with "AT+EBTES_DEMO=",to set scan mode. */
        uint16_t mode = atoi(string + strlen("AT+EBTES_DEMO="));
        LOG_MSGID_I(common, "mode %d", 1, mode);
        bt_connection_manager_write_scan_enable_mode((bt_gap_scan_mode_t)mode);
    } else if (strstr(string, "AT+EBTSI_DEMO=") != 0) {
        /* The string is matched with "AT+EBTSI_DEMO=",to set sniff mode. */
        bt_gap_default_sniff_params_t info = {0};
        if (sscanf(string + strlen("AT+EBTSI_DEMO="), "%u,%u,%u,%u", (unsigned int *)&info.max_sniff_interval,
                   (unsigned int *)&info.min_sniff_interval, (unsigned int *)&info.sniff_attempt, (unsigned int *)&info.sniff_timeout)) {
            /* Get sniff mode config value in target format successed. */
            LOG_MSGID_I(common, "max_inter %d, min_inter %d, attempt %d, to %d", 4, info.max_sniff_interval, info.min_sniff_interval, info.sniff_attempt, info.sniff_timeout);
            bt_gap_set_default_sniff_parameters(&info);
        } else {
            /* Get sniff mode config value in target format failed. */
            result = false;
        }
    } else if (strstr(string, "AT+EBLECON_DEMO=") != 0) {
        /* The string is matched with "AT+EBLECON_DEMO=",to connect ble. */
        bt_hci_cmd_le_create_connection_t conn_param;
        uint16_t addr_type;
        conn_param.le_scan_interval = 0x10;
        conn_param.le_scan_window = 0x10;
        conn_param.initiator_filter_policy = BT_HCI_CONN_FILTER_ASSIGNED_ADDRESS;
        conn_param.own_address_type = BT_ADDR_PUBLIC;
        conn_param.minimum_ce_length = 0x0000;
        conn_param.maximum_ce_length = 0x0190;
        if (sscanf(string + strlen("AT+EBLECON_DEMO="), "%2x:%2x:%2x:%2x:%2x:%2x,%u,%u,%u,%u,%u", (unsigned int *)&addr_5, (unsigned int *)&addr_4,
                   (unsigned int *)&addr_3, (unsigned int *)&addr_2, (unsigned int *)&addr_1, (unsigned int *)&addr_0, (unsigned int *)&addr_type,
                   (unsigned int *)&conn_param.conn_interval_min,
                   (unsigned int *)&conn_param.conn_interval_max,
                   (unsigned int *)&conn_param.conn_latency,
                   (unsigned int *)&conn_param.supervision_timeout) > 0) {
            /* Get ble connect config value in target format successed. */
            conn_param.peer_address.type = addr_type;
            conn_param.peer_address.addr[5] = addr_5;
            conn_param.peer_address.addr[4] = addr_4;
            conn_param.peer_address.addr[3] = addr_3;
            conn_param.peer_address.addr[2] = addr_2;
            conn_param.peer_address.addr[1] = addr_1;
            conn_param.peer_address.addr[0] = addr_0;
            LOG_MSGID_I(common, "addr:%x-%x-%x-%x-%x-%x,type:%d,min_interval:%d, max_interval:%d, latency: %d, conn_timeout: %d"
                        , 11
                        , addr_5,  addr_4, addr_3, addr_2, addr_1, addr_0, addr_type, conn_param.conn_interval_min,
                        conn_param.conn_interval_max, conn_param.conn_latency,  conn_param.supervision_timeout);
            bt_gap_le_connect(&conn_param);
        } else {
            /* Get ble connect config value in target format failed. */
            result = false;
        }
    } else if (strstr(string, "AT+EBLE_SET_TX_POWER=") != 0) {
        /* The string is matched with "AT+EBLE_SET_TX_POWER=",to config ble tx power. */
        uint32_t bt_init_tx = 0;
        uint32_t bt_max_tx = 0;
        uint32_t le_init_tx = 0;
        uint32_t tx_offset = 0;
        uint32_t fixed_tx_enable = 0;
        uint32_t fixed_tx = 0;
        if (sscanf(string + strlen("AT+EBLE_SET_TX_POWER="), "%u,%u,%u,%u,%u,%u",
                   (unsigned int *)&bt_init_tx, (unsigned int *)&tx_offset, (unsigned int *)&fixed_tx_enable,
                   (unsigned int *)&fixed_tx, (unsigned int *)&le_init_tx, (unsigned int *)&bt_max_tx) > 0) {
            /* Get ble tx power config value in target format successed. */
            bt_config_tx_power_t tx_cfg = {
                .bt_init_tx_power_level = bt_init_tx,
                .tx_power_level_offset = tx_offset,
                .fixed_tx_power_enable = fixed_tx_enable,
                .fixed_tx_power_level = fixed_tx,
                .le_init_tx_power_level = le_init_tx,
                .bt_max_tx_power_level = bt_max_tx,
            };
            bt_status_t status = bt_config_tx_power_level(&tx_cfg);
            uint8_t tx_power_level[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
            memcpy(tx_power_level, &tx_cfg, sizeof(tx_power_level));
            bt_driver_set_tx_power(tx_power_level);
            LOG_MSGID_I(common, "status:0x%08x, bt_init_tx:%d, bt_max_tx:%d, le_init_tx:%d, tx_offset:%d, fixed_tx_enable:%d, fixed_tx:%d", 7, status,
                        tx_cfg.bt_init_tx_power_level, tx_cfg.tx_power_level_offset,
                        tx_cfg.fixed_tx_power_enable, tx_cfg.fixed_tx_power_level,
                        tx_cfg.le_init_tx_power_level, tx_cfg.bt_max_tx_power_level);
        } else {
            /* Get ble tx power config value in target format failed. */
            LOG_MSGID_I(common, "send cmd fail", 0);
            result = false;
        }
    }
    return result;
}

static atci_status_t bt_atci_reg_callback(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    LOG_I(common, "[BT_ATCI] mode:%d, CMD:%s", parse_cmd->mode, parse_cmd->string_ptr);
    if (parse_cmd->mode == ATCI_CMD_MODE_EXECUTION) {
        /* Execute mode command, such as "AT+CMD=<op>. */
        bool result;
        result = bt_atci_parse_cmd(parse_cmd->string_ptr);
        if (result) {
            /* Parse atci cmd successed. */
            strncpy((char *)output.response_buf, "Parse OK\n", strlen("Parse OK\n") + 1);
        } else {
            /* Parse atci cmd failed. */
            strncpy((char *)output.response_buf, "Parse failed\n", strlen("Parse failed\n") + 1);
        }
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_OK;
        atci_send_response(&output);
    } else {
        /* Others unsupported atci cmd, such as ATCI_CMD_MODE_TESTING. */
        strncpy((char *)output.response_buf, "Not Support\n", strlen("Not Support\n") + 1);
        output.response_len = strlen((char *)output.response_buf);
        output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
        atci_send_response(&output);
    }
    return ATCI_STATUS_OK;
}

/**
*  @brief The defination of AT command handler list for low power test.For more information, please refer to #at_cmd_hdlr_fp() function.
*/
static atci_cmd_hdlr_item_t bt_at_cmd[] = {
    {"AT+EBLESA_DEMO", bt_atci_reg_callback, 0, 0},
    {"AT+EBLESEA_DEMO", bt_atci_reg_callback, 0, 0},
    {"AT+EBLESAD_DEMO", bt_atci_reg_callback, 0, 0},
    {"AT+EBLECON_DEMO", bt_atci_reg_callback, 0, 0},
    {"AT+EBTES_DEMO", bt_atci_reg_callback, 0, 0},
    {"AT+EBTSI_DEMO", bt_atci_reg_callback, 0, 0},
    {"AT+EBTA2DP_LPWR", bt_atci_reg_callback, 0, 0},
    {"AT+EBTTEST_DEMO", bt_atci_reg_callback, 0, 0},
//{"AT+EBTMP3_DEMO", bt_atci_reg_callback, 0, 0},
//{"AT+EBTMP3LP", bt_atci_reg_callback, 0, 0}
    {"AT+EBLE_SET_TX_POWER", bt_atci_reg_callback, 0, 0},
};

#define BT_ATCI_COMMAND_COUNT (sizeof(bt_at_cmd)/sizeof(atci_cmd_hdlr_item_t))  /* The count of atci cmd handlers. */

void bt_atci_init(void)
{
    atci_status_t ret = atci_register_handler(&bt_at_cmd[0], BT_ATCI_COMMAND_COUNT);
    if (ret == ATCI_STATUS_OK) {
        /* Register atci handler successed. */
        LOG_MSGID_I(common, "bt_atci register success", 0);
    } else {
        /* Register atci handler failed. */
        LOG_MSGID_W(common, "bt_atci register fail", 0);
    }
}

