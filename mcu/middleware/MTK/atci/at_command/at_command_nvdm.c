/* Copyright Statement:
 *
 * (C) 2005-2016  MediaTek Inc. All rights reserved.
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. ("MediaTek") and/or its licensors.
 * Without the prior written permission of MediaTek and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 * You may only use, reproduce, modify, or distribute (as applicable) MediaTek Software
 * if you have agreed to and been bound by the applicable license agreement with
 * MediaTek ("License Agreement") and been granted explicit permission to do so within
 * the License Agreement ("Permitted User").  If you are not a Permitted User,
 * please cease any access or use of MediaTek Software immediately.
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT MEDIATEK SOFTWARE RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES
 * ARE PROVIDED TO RECEIVER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL
 * WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 */

// For Register AT command handler
//#include "atci.h"
// System head file
#if defined(MTK_NVDM_ENABLE)
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "syslog.h"
#include "at_command.h"
#include "nvdm_port.h"
#include "nvdm.h"
#include "portable.h"           /* add for pvPortMalloc and pvPortFree */
#ifdef SYSTEM_DAEMON_TASK_ENABLE
#include "system_daemon.h"      /* add for system_daemon_send_message */
#endif
#include "hal_gpt.h"

#if  defined ( __GNUC__ )
#ifndef __weak
#define __weak   __attribute__((weak))
#endif /* __weak */
#endif /* __GNUC__ */

#define AT_COMMAND_NVDM_HELP \
"AT+ENVDM usage:\r\n\
   Read item data: AT+ENVDM=0,<group>,<item>\r\n\
   Write value of item: AT+ENVDM=1,<group>,<item>,<length>,<data>\r\n\
   Reset value of item: AT+ENVDM=2[,<group>]\r\n\
   Read item data: AT+ENVDM=R,<group>,<item>\r\n\
   Clear one item: AT+ENVDM=CI,<group>,<item>\r\n\
   Clear items specified by group name: AT+ENVDM=CG,<group>\r\n\
   Clear all items: AT+ENVDM=CA\r\n\
   Dump all items info: AT+ENVDM=D\r\n\
   Write a string item: AT+ENVDM=WS,<group>,<item>,<length>,<string>\r\n\
   Write a raw data item: AT+ENVDM=WB,<group>,<item>,<length>,<data>\r\n\
   Query information about NVDM: AT+ENVDM=I\r\n"


static uint32_t hexstring_to_bytearray(char *s, uint8_t *bits)
{
    uint32_t i, n = 0;

    for (i = 0; s[i]; i += 2) {
        if (s[i] >= 'A' && s[i] <= 'F') {
            bits[n] = s[i] - 'A' + 10;
        } else if (s[i] >= 'a' && s[i] <= 'f') {
            bits[n] = s[i] - 'a' + 10;
        } else {
            bits[n] = s[i] - '0';
        }
        if (s[i + 1] >= 'A' && s[i + 1] <= 'F') {
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'A' + 10);
        } else if (s[i + 1] >= 'a' && s[i + 1] <= 'f') {
            bits[n] = (bits[n] << 4) | (s[i + 1] - 'a' + 10);
        } else {
            bits[n] = (bits[n] << 4) | (s[i + 1] - '0');
        }
        ++n;
    }

    return n;
}

static uint32_t hexstring_to_integer(char *s, uint32_t bits)
{
    uint32_t i, j, tmp, sum;

    sum = 0;
    for (i = 0; i < bits; i++) {
        if ((s[i] < '0') || (s[i] > '9')) {
            return 0;
        }
        tmp = s[i] - '0';
        for (j = 0; j < bits - i - 1; j++) {
            tmp *= 10;
        }
        sum += tmp;
    }

    return sum;
}

bool check_input_context_validity(char *ctx)
{
    uint32_t cxt_len = strlen(ctx);
    char temp;
    if ((cxt_len % 2) != 0) {
        /* binary input data byte count must be even */
        return false;
    }
    for (uint32_t idx = 0; idx < cxt_len; idx++) {
        /* check binary raw data input context */
        temp = ctx[idx];
        if ((temp >= '0') && (temp <= '9')) {
            continue;
        } else {
            /* not decimal digit number */
            if (((temp >= 'a') && (temp <= 'f')) || ((temp >= 'A') && (temp <= 'F'))) {
                /* hex digit number */
                continue;
            } else {
                return false;
            }
        }
    }
    return true;
}

void binary_data_to_hex_string(uint8_t *p_data, uint32_t length)
{
    uint32_t hex_string_pos = (2 * length) - 1;
    uint8_t data, div, rest;

    while (length) {
        data = p_data[length - 1];

        div = data / 16;
        rest = data % 16;

        if (rest < 10) {
            p_data[hex_string_pos] = '0' + rest;
        } else {
            p_data[hex_string_pos] = 'A' + rest - 10;
        }
        --hex_string_pos;

        if (div < 10) {
            p_data[hex_string_pos] = '0' + div;
        } else {
            p_data[hex_string_pos] = 'A' + div - 10;
        }
        --hex_string_pos;

        --length;
    }
}

extern nvdm_status_t nvdm_query_data_item_length(const char *group_name, const char *data_item_name, uint32_t *size);
extern nvdm_status_t nvdm_query_data_item_type(const char *group_name, const char *data_item_name, nvdm_data_item_type_t *type);

/* user should define this function in it's project normally. */
__weak void user_data_item_reset_to_default(char *group_name)
{
    group_name = group_name;
}

void prepare_nvdm_info(char *buff);

/*--- Function ---*/
atci_status_t atci_cmd_hdlr_nvdm(atci_parse_cmd_param_t *parse_cmd);

/* AT command handler */
atci_status_t atci_cmd_hdlr_nvdm(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t resonse = {{0}};
    atci_response_heavy_data_t heavy_response;
    uint8_t send_responese_flag = 0;
    char *group_name = NULL;
    char *data_item_name = NULL;
    char *parameter;
    char *saveptr = NULL;
    uint32_t length;
    uint8_t buffer[256];
    nvdm_status_t status;
    char delim[] = {',', 0x0D, 0x0A, 0x0};
    uint32_t begin_time, end_time;
    heavy_response.response_buf = NULL;

    LOG_MSGID_I(common, "atci_cmd_hdlr_nvdm \r\n", 0);

    resonse.response_flag = 0; // Command Execute Finish.
#ifdef ATCI_APB_PROXY_ADAPTER_ENABLE
    resonse.cmd_id = parse_cmd->cmd_id;
#endif

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
            if (strlen(AT_COMMAND_NVDM_HELP) > ATCI_UART_TX_FIFO_BUFFER_SIZE) {
                /* 4 means that the AT CMD response will auto add 4 bytes of data at the end of the data. */
                length = ATCI_UART_TX_FIFO_BUFFER_SIZE - 4;
                strncpy((char *)(resonse.response_buf), AT_COMMAND_NVDM_HELP, length);
                resonse.response_buf[length] = '\0';
            } else {
                strncpy((char *)(resonse.response_buf), AT_COMMAND_NVDM_HELP, strlen(AT_COMMAND_NVDM_HELP));
            }
            resonse.response_len = strlen((char *)(resonse.response_buf));
            resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            break;

        case ATCI_CMD_MODE_EXECUTION: // rec: AT+ENVDM=<op>  the handler need to parse the parameters
            if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=0") != NULL) {
                send_responese_flag = 0;

                /* read data item from NVDM */
                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter != NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                length = strlen(data_item_name);
                /* remove \0d\0a */
                //data_item_name[length - 2] = 0;
                //data_item_name[length - 1] = 0;
                LOG_I(common, "group_name = %s", group_name);
                LOG_I(common, "data_item_name = %s", data_item_name);
                length = ATCI_UART_TX_FIFO_BUFFER_SIZE - 2;
                status = nvdm_read_data_item(group_name,
                                             data_item_name,
                                             resonse.response_buf,
                                             &length);
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_I(common, "nvdm_read_data_item status = %d", 1, status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_MSGID_I(common, "nvdm_read_data_item length = %d", 1, length);
                /* append \0d\0a */
                resonse.response_buf[length] = '\r';
                resonse.response_buf[length + 1] = '\n';
                resonse.response_len = length + 2;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=1") != NULL) {
                send_responese_flag = 0;

                /* write data item into NVDM */
                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "group_name = %s", group_name);
                /* get data item name */
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_I(common, "data_item_name = %s", data_item_name);
                /* get length of data item */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                length = hexstring_to_integer(parameter, strlen(parameter));
                LOG_MSGID_I(common, "length = %d", 1, length);
                /* get content of data item */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                if (check_input_context_validity(parameter) == false) {
                    LOG_MSGID_W(common, "Please input hex string with even byte count.\r\n", 0);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    break;
                }
                /* remove \0d\0a */
                //resonse.response_len = strlen(parameter);
                //parameter[resonse.response_len - 2] = 0;
                //parameter[resonse.response_len - 1] = 0;
                resonse.response_len = hexstring_to_bytearray(parameter, buffer);
                LOG_MSGID_I(common, "parse length = %d", 1, resonse.response_len);
                if (length != resonse.response_len) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                {
                    uint32_t i;

                    LOG_MSGID_I(common, "data = ", 0);
                    for (i = 0; i < length; i++) {
                        LOG_MSGID_I(common, "0x%x", 1, buffer[i]);
                    }
                }
                status = nvdm_write_data_item(group_name,
                                              data_item_name,
                                              NVDM_DATA_ITEM_TYPE_STRING,
                                              buffer,
                                              resonse.response_len);
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_I(common, "nvdm_write_data_item status = %d", 1, status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_len = 0;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=2") != NULL) {
                send_responese_flag = 0;

                /* reset data items to default value by group or all */
                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name != NULL) {
                    LOG_I(common, "group_name = %s", group_name);
                }
                user_data_item_reset_to_default(group_name);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=D")) {
                /* Dump all nvdm items */
                send_responese_flag = 0;

                parameter = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter != NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    LOG_MSGID_I(common, "Input parameter ERROR, please run AT+ENVDM=? to get detail information", 0);
                    break;
                }

#ifdef SYSTEM_DAEMON_TASK_ENABLE
                system_daemon_send_message(SYSTEM_DAEMON_ID_DUMPALL_NVDM_ITEMS, NULL);

                /* must send OK response !!! */
                send_responese_flag = 0;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                resonse.response_len = 0;
#else
                LOG_MSGID_W(common, "Unsupport command, please check SYSTEM_DAEMON_TASK_ENABLE", 0);
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                resonse.response_len = 0;
#endif

            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=R")) {
                /* Read a nvdm item entry indicated by group name and data item name */
                send_responese_flag = 0;

                /* read data item from NVDM */
                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    break;
                }
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    break;
                }
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter != NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    break;
                }

                nvdm_data_item_type_t item_type;
                nvdm_status_t status;
                uint32_t item_size, calc_size;
                uint8_t *p_data;
                status = nvdm_query_data_item_type(group_name, data_item_name, &item_type);
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_W(common, "status: %d item_type: %d\r\n", 2, status, item_type);
                    send_responese_flag = 0;
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    break;
                }
                status = nvdm_query_data_item_length(group_name, data_item_name, &item_size);
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_W(common, "status: %d item_type: %d\r\n", 2, status, item_type);
                    send_responese_flag = 0;
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    break;
                }
                LOG_MSGID_W(common, "status: %d item_size: %d\r\n", 2, status, item_size);

                calc_size = ((item_type == NVDM_DATA_ITEM_TYPE_RAW_DATA) ? 2 : 1) * item_size + 20;
                if (calc_size > ATCI_UART_TX_FIFO_BUFFER_SIZE) {
                    send_responese_flag = 1;
                    heavy_response.response_buf = (uint8_t *)pvPortMalloc(calc_size);
                    p_data = heavy_response.response_buf;
                } else {
                    p_data = resonse.response_buf;
                }

                if ((send_responese_flag == 1) && (heavy_response.response_buf == NULL)) {
                    LOG_MSGID_W(common, "AT+ENVDM=R ERROR because of pvPortMalloc fail", 0);
                    send_responese_flag = 0;
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    break;
                }

                p_data[0] = '[';

                if (item_type == NVDM_DATA_ITEM_TYPE_RAW_DATA) {
                    p_data[1] = 'B';
                } else {
                    p_data[1] = 'S';
                }
                p_data[2] = ']';
                p_data[3] = ' ';

                length = calc_size - 2 - 4;
                status = nvdm_read_data_item(group_name, data_item_name, (p_data + 4), &length);
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_I(common, "nvdm_read_data_item status = %d", 1, status);
                    send_responese_flag = 0;
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    resonse.response_len = 0;
                    if (heavy_response.response_buf != NULL) {
                        vPortFree(heavy_response.response_buf);
                    }
                    break;
                }
                LOG_MSGID_I(common, "nvdm_read_data_item length = %d", 1, length);

                if (item_type == NVDM_DATA_ITEM_TYPE_RAW_DATA) {
                    binary_data_to_hex_string((p_data + 4), length);
                    length *= 2;
                }
                length += 4;
                /* append \0d\0a */
                if (send_responese_flag == 0) {
                    resonse.response_buf[length] = '\r';
                    resonse.response_buf[length + 1] = '\n';
                    resonse.response_len = length + 2;
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;

                    /* only one response, can't use URC_FORMAT */
                    /* resonse.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT; */
                } else {
                    heavy_response.response_buf[length] = '\r';
                    heavy_response.response_buf[length + 1] = '\n';
                    heavy_response.response_len = length + 2;
                    heavy_response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                    /* heavy_response.response_flag = ATCI_RESPONSE_FLAG_URC_FORMAT; */
                }
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=CI")) {
                /* clear one nvdm entry indicated by group name and data item name */
                send_responese_flag = 0;

                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter != NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                status = nvdm_delete_data_item(group_name, data_item_name);
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_I(common, "nvdm_delete_data_item status = %d", 1, status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_len = 0;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=CG")) {
                /* clear all nvdm entries specified by group name */
                send_responese_flag = 0;

                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name != NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                status = nvdm_delete_group(group_name);
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_I(common, "nvdm_delete_group status = %d", 1, status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_len = 0;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=CA")) {
                /* clear all nvdm entries */
                send_responese_flag = 0;
                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name != NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                status = nvdm_delete_all();
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_I(common, "nvdm_delete_all status = %d", 1, status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_len = 0;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=WS")) {
                /* add an entry of NVDM with parameter STRING */
                send_responese_flag = 0;
                /* write data item into NVDM */
                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                /* get data item name */
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                /* get length of data item */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                length = hexstring_to_integer(parameter, strlen(parameter));
                LOG_MSGID_I(common, "length = %d", 1, length);
                /* get content of data item */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                LOG_MSGID_I(common, "parse length = %d", 1, strlen(parameter));
                if (length != strlen(parameter)) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &begin_time);
                status = nvdm_write_data_item(group_name,
                                              data_item_name,
                                              NVDM_DATA_ITEM_TYPE_STRING,
                                              (const uint8_t *)(parameter),
                                              (uint32_t)(strlen(parameter)));  /* not write '\0', aligh with AT+ENVDM=1 */
                hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_time);
                if (status != NVDM_STATUS_OK) {
                    LOG_MSGID_I(common, "nvdm_write_data_item status = %d", 1, status);
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                resonse.response_len = 0;
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                hal_gpt_get_duration_count(begin_time, end_time, &begin_time);
                LOG_W(common, "[NVDM_WRITE_TIME] group_name: %s, data_item_name: %s, write time: %d us\n", group_name, data_item_name, begin_time);
            } else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=WB")) {
                /* add an entry of NVDM with parameter RAW_DATA */
                send_responese_flag = 0;
                /* write data item into NVDM */
                group_name = strtok_r(parse_cmd->string_ptr, delim, &saveptr);
                /* get group name */
                group_name = strtok_r(NULL, delim, &saveptr);
                if (group_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                /* get data item name */
                data_item_name = strtok_r(NULL, delim, &saveptr);
                if (data_item_name == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                /* get length of data item */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                length = hexstring_to_integer(parameter, strlen(parameter));
                LOG_MSGID_I(common, "length = %d", 1, length);
                /* get content of data item */
                parameter = strtok_r(NULL, delim, &saveptr);
                if (parameter == NULL) {
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
                if (check_input_context_validity(parameter) == true) {
                    resonse.response_len = hexstring_to_bytearray(parameter, buffer);
                    LOG_MSGID_I(common, "parse length = %d", 1, resonse.response_len);
                    if (length != resonse.response_len) {
                        resonse.response_len = 0;
                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        break;
                    }
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &begin_time);
                    status = nvdm_write_data_item(group_name,
                                                  data_item_name,
                                                  NVDM_DATA_ITEM_TYPE_RAW_DATA,
                                                  buffer,
                                                  resonse.response_len);
                    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &end_time);
                    hal_gpt_get_duration_count(begin_time, end_time, &begin_time);
                    LOG_W(common, "[NVDM_WRITE_TIME] group_name: %s, data_item_name: %s, write time: %d us\n", group_name, data_item_name, begin_time);
                    if (status != NVDM_STATUS_OK) {
                        LOG_MSGID_I(common, "nvdm_write_data_item status = %d", 1, status);
                        resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                        break;
                    }
                    resonse.response_len = 0;
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    resonse.response_len = 0;
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                    break;
                }
            }  else if (strstr((char *)parse_cmd->string_ptr, "AT+ENVDM=I")) {
                send_responese_flag = 0;

                /* After calculation, the longest response of AT+ENVDM=I does not exceed 256 bytes. */
                if (ATCI_UART_TX_FIFO_BUFFER_SIZE > 256) {
                    prepare_nvdm_info((char *)resonse.response_buf);
                    resonse.response_len = strlen((char *)(resonse.response_buf));
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
                } else {
                    strncpy((char *)(resonse.response_buf), "response length not enough\r\n", strlen("response length not enough\r\n"));
                    resonse.response_len = strlen((char *)(resonse.response_buf));
                    resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                }
            } else {
                /* invalid AT command */
                resonse.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;
                resonse.response_len = 0;
            }
            break;
        default :
            // others are invalid command format
            strncpy((char *)(resonse.response_buf), "invalid command format\r\n", strlen("invalid command format\r\n"));
            resonse.response_len = strlen((char *)(resonse.response_buf));
            break;
    }

    if (send_responese_flag == 0) {
        atci_send_response(&resonse);
    } else {
        atci_send_heavy_response(&heavy_response);
        vPortFree(heavy_response.response_buf);
    }

    return ATCI_STATUS_OK;
}


extern nvdm_status_t nvdm_query_space_info(uint32_t *total_avail_space, uint32_t *curr_used_space);
void prepare_nvdm_info(char *buff)
{
    uint32_t total_avail_space, curr_used_space;
    uint32_t max_data_item_size, max_group_name_size, max_data_item_name_size, total_data_item_count;
    uint32_t peb_count, peb_size;

    nvdm_query_space_info(&total_avail_space, &curr_used_space);
    total_data_item_count = nvdm_port_get_data_item_config(&max_data_item_size, &max_group_name_size, &max_data_item_name_size);
    peb_size = nvdm_port_get_peb_config(&peb_count);

    snprintf(buff, ATCI_UART_TX_FIFO_BUFFER_SIZE, "NVDM information:\r\nCurrentUsed: %-10u\r\nActualCapacity: %-10u\r\nConfigCapacity: %-10u\r\nMaxItemNumber: %-10u\r\nMaxDataItemSize: %-10u\r\nMaxItemNameSize: %-10u\r\nMaxGroupNameSize: %-10u\r\n",
            (unsigned int)curr_used_space, (unsigned int)total_avail_space,
            (unsigned int)(peb_size * peb_count),
            (unsigned int)total_data_item_count, (unsigned int)max_data_item_size, (unsigned int)max_data_item_name_size, (unsigned int)max_group_name_size
           );
}

#endif


#ifdef SYSTEM_DAEMON_TASK_ENABLE
extern nvdm_status_t nvdm_query_space_info(uint32_t *total_avail_space, uint32_t *curr_used_space);
void system_daemon_dump_all_nvdm_items_msg_handler(void)
{
    atci_response_heavy_data_t dumpall_response;
    uint32_t length, item_count = 0, total_ava = 0, cur_uesd = 0;
    nvdm_status_t status;
    atci_status_t send_response_result;
    uint32_t max_item_data_size, max_group_name_size, max_item_name_size, max_item_count, max_malloc_size;
    nvdm_data_item_type_t item_type;

    uint8_t *tmp_ptr = NULL;
    char group_name_buff[64];
    char data_item_name_buff[64];

    max_item_count = nvdm_port_get_data_item_config(&max_item_data_size, &max_group_name_size, &max_item_name_size);

    /* name include '\0' and '[B/S] ' and auto append '\r\n' */
    max_malloc_size = 2 * max_item_data_size + max_group_name_size + max_item_name_size + 2 + 4 + 2;

    dumpall_response.response_buf = (uint8_t *)pvPortMalloc(max_malloc_size);

    if (dumpall_response.response_buf == NULL) {
        LOG_MSGID_W(common, "pvPortMalloc return NULL", 0);
        return;
    }

    snprintf((char *)(dumpall_response.response_buf), max_malloc_size, "NVDM dump all items information:");
    dumpall_response.response_len = strlen((const char *)(dumpall_response.response_buf));
    dumpall_response.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
    send_response_result = atci_send_heavy_response(&dumpall_response);
    if (send_response_result != ATCI_STATUS_OK) {
        LOG_MSGID_W(common, "atci_send_heavy_response return %d", 1, send_response_result);
        vPortFree(dumpall_response.response_buf);
        return;
    }

    status = nvdm_query_begin();
    while (nvdm_query_next_group_name(group_name_buff) == NVDM_STATUS_OK) {
        while (nvdm_query_next_data_item_name(data_item_name_buff) == NVDM_STATUS_OK) {
            LOG_W(common, "group_name: %s, data_item_name: %s\n", group_name_buff, data_item_name_buff);

            nvdm_query_data_item_type(group_name_buff, data_item_name_buff, &item_type);

            tmp_ptr = dumpall_response.response_buf;
            tmp_ptr[0] = '[';
            if (item_type == NVDM_DATA_ITEM_TYPE_RAW_DATA) {
                tmp_ptr[1] = 'B';
            } else {
                tmp_ptr[1] = 'S';
            }
            tmp_ptr[2] = ']';
            tmp_ptr[3] = ' ';

            tmp_ptr += 4;

            tmp_ptr = memcpy(tmp_ptr, group_name_buff, strlen(group_name_buff) + 1);                /* copy '\0' */
            tmp_ptr = memcpy(tmp_ptr, data_item_name_buff, strlen(data_item_name_buff) + 1);        /* copy '\0' */

            /* type header 4 bytes */
            length = max_malloc_size - (strlen(group_name_buff) + 1) - (strlen(data_item_name_buff) + 1) - 4 - 2;
            status = nvdm_read_data_item(group_name_buff, data_item_name_buff, tmp_ptr, &length);
            LOG_MSGID_I(common, "nvdm_read_data_item length = %d", 1, length);

            ++item_count;

            if (item_type == NVDM_DATA_ITEM_TYPE_RAW_DATA) {
                binary_data_to_hex_string(tmp_ptr, length);
                length *= 2;
            }
            length += strlen(group_name_buff) + 1 + strlen(data_item_name_buff) + 1 + 4;
            dumpall_response.response_len = length;
            dumpall_response.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;

            send_response_result = atci_send_heavy_response(&dumpall_response);
            if (send_response_result != ATCI_STATUS_OK) {
                LOG_MSGID_W(common, "atci_send_heavy_response return %d", 1, send_response_result);
            }
        }
    }
    status = nvdm_query_end();
    status = nvdm_query_space_info(&total_ava, &cur_uesd);
    if (status == NVDM_STATUS_OK) {
        snprintf((char *)(dumpall_response.response_buf), max_malloc_size, "MAX Item Count: %u\r\nTotal item count: %u\r\nTotal available bytes: %u\r\nCurrent used bytes: %u\r\n", (unsigned int)max_item_count, (unsigned int)item_count, (unsigned int)total_ava, (unsigned int)cur_uesd);
        dumpall_response.response_len = strlen((const char *)(dumpall_response.response_buf));
        dumpall_response.response_flag = ATCI_RESPONSE_FLAG_APPEND_OK;
        send_response_result = atci_send_heavy_response(&dumpall_response);
        if (send_response_result != ATCI_STATUS_OK) {
            LOG_MSGID_W(common, "atci_send_heavy_response return %d", 1, send_response_result);
        }
    }

    vPortFree(dumpall_response.response_buf);
}
#endif /* SYSTEM_DAEMON_TASK_ENABLE */
