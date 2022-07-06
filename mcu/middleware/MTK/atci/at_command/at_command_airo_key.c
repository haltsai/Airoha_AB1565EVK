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

#ifdef AIRO_KEY_EVENT_ENABLE

#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include <stdio.h>
#include "syslog.h"
#include <stdlib.h>

#include "at_command.h"
#include "airo_key_event.h"
#include "airo_key_event_internal.h"
#include "hal_keypad_table.h"


log_create_module(atci_airo_Key, PRINT_LEVEL_INFO);

#define LOGE(fmt,arg...)   printf("[AIROKEY]"fmt,##arg)
#define LOGW(fmt,arg...)   printf("[AIROKEY]"fmt,##arg)
#define LOGI(fmt,arg...)   printf("[AIROKEY]"fmt,##arg)

#define LOGMSGIDI(fmt,cnt,arg...)   LOG_MSGID_I(atci_airo_Key ,"[AIROKEY]"fmt,cnt,##arg)
#define LOGMSGIDW(fmt,cnt,arg...)   LOG_MSGID_W(atci_airo_Key, "[AIROKEY]"fmt,cnt,##arg)
#define LOGMSGIDE(fmt,cnt,arg...)   LOG_MSGID_E(atci_airo_Key ,"[AIROKEY]"fmt,cnt,##arg)

#define MAX_AT_ARGS 8

typedef struct {
    uint32_t val[MAX_AT_ARGS];
    uint32_t counter;
} at_buffer_t;

typedef struct {
    char * command;
    char * opcode;
    at_buffer_t number;
    at_buffer_t string;
    at_buffer_t hexstr;
} at_content_t;

static long int str2int(char* str)
{
    char *p = str;
    while (*p == ' ')
        p++;
    if ((p[0] == '0') && ((p[1] == 'x') || (p[1] == 'X'))) {
        p += 2;
        return strtol(p, NULL, 16);
    } else {
        return strtol(p, NULL, 10);
    }
}

/**
 * @brief decode AT command
 *
 * @param str  the command string format should be <command>=<opcode>:<args ...>
 * args:
 *  number : 1234 or 0x1234
 *  string : @hello string
 *  hexstr : #12abcd344a5d8c
 * example:
 *  AT+AIROKEY=EVENT:0x21,0x18
 * @param content command content
 * @return int 0: success, less than 0: fail
 */
int decode_command(char* str, at_content_t *content)
{
    char *s;
    at_buffer_t *at_buffer;

    #define CMD_DELIMITER           "="
    #define OPCODE_DELIMITER        ":"
    #define ARGS_DELIMITER          ","
    #define STR_FLAG                '@'
    #define HEXSTR_FLAG             '#'

    content->string.counter = 0;
    content->hexstr.counter = 0;
    content->number.counter = 0;

    s = strtok(str, CMD_DELIMITER);
    if (s == NULL) {
        return -1;
    }
    content->command = s;

    s = strtok(NULL, OPCODE_DELIMITER);
    if (s == NULL) {
        return -2;
    }
    content->opcode = s;

    while(s != NULL) {
        s = strtok(NULL, ARGS_DELIMITER);
        if (s != NULL) {
            if (s[0] == STR_FLAG) { // this is string
                at_buffer = &content->string;
                if (at_buffer->counter < MAX_AT_ARGS) {
                    at_buffer->val[at_buffer->counter++] = (uint32_t)(s + 1);
                }
            } else if (s[0] == HEXSTR_FLAG) { // this is hex string
                at_buffer = &content->hexstr;
                if (at_buffer->counter < MAX_AT_ARGS) {
                    at_buffer->val[at_buffer->counter++] = (uint32_t)(s + 1);
                }
            } else { //this is number
                at_buffer = &content->number;
                if (at_buffer->counter < MAX_AT_ARGS) {
                    at_buffer->val[at_buffer->counter++] = str2int(s);
                }
            }
        }
    }

    return 0;
}

#define at_response(result, fmt, ...) \
do {\
    atci_response_t rsp = {{0},0,0};\
    sprintf((char *)rsp.response_buf, fmt, ##__VA_ARGS__);\
    rsp.response_len = strlen((char *)rsp.response_buf);\
    if (result) {\
        rsp.response_flag |= ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;\
    } else {\
        rsp.response_flag |= ATCI_RESPONSE_FLAG_APPEND_ERROR;\
    }\
    atci_send_response(&rsp);\
} while(0)

atci_status_t atci_cmd_hdlr_airo_key(atci_parse_cmd_param_t *parse_cmd)
{
    at_content_t at_content;
    memset(&at_content, 0, sizeof(at_content));

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:
        case ATCI_CMD_MODE_READ:
            at_response(true, "Usage:\r\nAT+AIROKEY=APPEVENT:<key_event>,<key_data>\r\n");
            at_response(true, "AT+AIROKEY=EVENT:<key_event>,<key_data>\r\n");
            at_response(true, "key_event:  \r\n"\
                              "  For AT+AIROKEY=APPEVENT: The value of key_event is defined in airo_key_event_t\r\n"\
                              "  For AT+AIROKEY=EVENT: 0 means release key, 1 means press key\r\n");
            at_response(true, "key_data: \r\n"\
                        "  eint_key: 0x%x <= key_data <= 0x%x\r\n"\
                        "  captouch: 0x%x <= key_data <= 0x%x\r\n"\
                        "  powerkey: key_data = 0x%x\r\n",
                        EINT_KEY_0, EINT_KEY_4,
                        DEVICE_KEY_A, DEVICE_KEY_H,
                        DEVICE_KEY_POWER);
            break;

        case ATCI_CMD_MODE_EXECUTION:
            if (parse_cmd->string_ptr != NULL) {
                if (decode_command(parse_cmd->string_ptr, &at_content) == 0) {
                    if (!strncmp(at_content.opcode, "APPEVENT", strlen("APPEVENT"))) {
                        uint32_t event = at_content.number.val[0];
                        uint32_t key_data = at_content.number.val[1];
                        airo_key_send_simulation_event((airo_key_event_t)event, key_data);
                        LOGMSGIDI("[airo_key] key_event=0x%x key_data=0x%x\r\n", 2, event, key_data);
                        at_response(true, "OK\r\n");
                    } else if (!strncmp(at_content.opcode, "EVENT", strlen("EVENT"))) {
                        airo_key_mapping_event_t key_event;
                        airo_key_source_type_t type;
                        uint32_t time;
                        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_32K, &time);
                        uint32_t event = at_content.number.val[0];
                        uint32_t key_data = at_content.number.val[1];

                        key_event.state      = (event > 0) ? AIRO_KEY_DRIVEN_PRESS: AIRO_KEY_DRIVEN_RELEASE;
                        key_event.key_data   = key_data;
                        key_event.time_stamp = time;
                        if ((EINT_KEY_0 <= key_data) && (key_data <= EINT_KEY_4)) {
                            type = AIRO_KEY_EINT_KEY;
                        } else if ((DEVICE_KEY_A <= key_data) && (key_data <= DEVICE_KEY_H)) {
                            type = AIRO_KEY_CAPTOUCH;
                        } else if (DEVICE_KEY_POWER == key_data) {
                            type = AIRO_KEY_POWERKEY;
                        } else {
                            at_response(false, "[airo_key] key_data=0x%x unsupported\r\n", (unsigned int)key_data);
                            return ATCI_STATUS_OK;
                        }
                        LOGMSGIDI("[airo_key] key_event=0x%x key_data=0x%x type=%d\r\n", 3, key_event, key_data, type);
                        at_response(true, "OK\r\n");
                        airo_key_process_key(&key_event, type);
                    }
                }
            }
            break;
        default :
            at_response(false, "\r\nERROR DATA\r\n");
    }
    return ATCI_STATUS_OK;
}

#endif

