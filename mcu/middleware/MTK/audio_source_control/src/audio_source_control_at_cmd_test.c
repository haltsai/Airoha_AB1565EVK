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

#include "atci.h"
#include "audio_source_control.h"
#include "string.h"
#include "stdio.h"

#if AUDIO_SOURCE_CONTROL_TEST

#define AUDIO_SOURCE_CONTROL_HANDLE_COUNT       3
#define AUDIO_SOURCE_CONTROL_STR_REGISTER       ("REGISTER")
#define AUDIO_SOURCE_CONTROL_STR_UNREGISTER     ("UNREGISTER")
#define AUDIO_SOURCE_CONTROL_STR_TAKE_1         ("TAKE_1")
#define AUDIO_SOURCE_CONTROL_STR_TAKE_2         ("TAKE_2")
#define AUDIO_SOURCE_CONTROL_STR_TAKE_3         ("TAKE_3")
#define AUDIO_SOURCE_CONTROL_STR_GIVE_1         ("GIVE_1")
#define AUDIO_SOURCE_CONTROL_STR_GIVE_2         ("GIVE_2")
#define AUDIO_SOURCE_CONTROL_STR_GIVE_3         ("GIVE_3")

#define AUDIO_SOURCE_CONTROL_STR_REGISTER_LEN   strlen(AUDIO_SOURCE_CONTROL_STR_REGISTER)
#define AUDIO_SOURCE_CONTROL_STR_UNREGISTER_LEN strlen(AUDIO_SOURCE_CONTROL_STR_UNREGISTER)
#define AUDIO_SOURCE_CONTROL_STR_TAKE_1_LEN     strlen(AUDIO_SOURCE_CONTROL_STR_TAKE_1)
#define AUDIO_SOURCE_CONTROL_STR_TAKE_2_LEN     strlen(AUDIO_SOURCE_CONTROL_STR_TAKE_2)
#define AUDIO_SOURCE_CONTROL_STR_TAKE_3_LEN     strlen(AUDIO_SOURCE_CONTROL_STR_TAKE_3)
#define AUDIO_SOURCE_CONTROL_STR_GIVE_1_LEN     strlen(AUDIO_SOURCE_CONTROL_STR_GIVE_1)
#define AUDIO_SOURCE_CONTROL_STR_GIVE_2_LEN     strlen(AUDIO_SOURCE_CONTROL_STR_GIVE_2)
#define AUDIO_SOURCE_CONTROL_STR_GIVE_3_LEN     strlen(AUDIO_SOURCE_CONTROL_STR_GIVE_3)

void *handle_1 = NULL;
void *handle_2 = NULL;
void *handle_3 = NULL;

char *audio_source_control_match_result(audio_source_control_request_result_t result)
{
    switch (result) {
        case AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_SUCCEED:
        return "REQUEST SUCCEED";
        case AUDIO_SOURCE_CONTROL_REQUEST_RESULT_REQUEST_FAILED:
        return "REQUEST FAILED";
        case AUDIO_SOURCE_CONTROL_REQUEST_RESULT_RELEASE_SUCCEED:
        return "RELEASE SUCCEED";
        case AUDIO_SOURCE_CONTROL_REQUEST_RESULT_RELEASE_FAILED:
        return "RELEASE FAILED";
        case AUDIO_SOURCE_CONTROL_REQUEST_RESULT_SUSPEND:
        return "SUSPEND";
    }
    return "NONE";
}

void audio_source_control_request_notify_handler(void *handle, audio_source_control_request_type_t type, audio_source_control_request_result_t result)
{
    unsigned char index = 0;
    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_request_notify_handler, type : %d, result : %d", type, result);
    if (handle_1 == handle) {
        index = 1;
    } else if (handle_2 == handle) {
        index = 2;
    } else if (handle_3 == handle) {
        index = 3;
    } else {
        printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_request_notify_handler, handle is not match");
        return;
    }

    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_request_notify_handler, handle_%d result : %s", index, audio_source_control_match_result(result));

    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};
    memcpy(response.response_buf, audio_source_control_match_result(result), strlen(audio_source_control_match_result(result)));
    response.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);
}

atci_status_t audio_source_control_at_cmd_handler(atci_parse_cmd_param_t *parse_cmd)
{
    if (parse_cmd == NULL) {
        return ATCI_STATUS_ERROR;
    }

    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, received command str : %s", parse_cmd->string_ptr);

    audio_source_control_result_t result = 0;
    atci_response_t response = {{0}, 0, ATCI_RESPONSE_FLAG_APPEND_ERROR};

    response.response_flag = ATCI_RESPONSE_FLAG_APPEND_ERROR;

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_EXECUTION: {
            if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, AUDIO_SOURCE_CONTROL_STR_REGISTER, AUDIO_SOURCE_CONTROL_STR_REGISTER_LEN)) {
                if ((handle_1 != NULL) && (handle_2 != NULL) && (handle_3 != NULL)) {
                    memcpy(response.response_buf, "REG NO RESOURCE", strlen("REG NO RESOURCE"));
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, Register failed [No resource]");
                } else {
                    audio_source_control_callback_t callback = {
                        .request_notify = audio_source_control_request_notify_handler,
                    };

                    void *handle = audio_source_control_register(AUDIO_SOURCE_CONTROL_REQUEST_TYPE_MIC, callback);
                    if (handle != NULL) {
                        memcpy(response.response_buf, "REG SUCCEED", strlen("REG SUCCEED"));
                        printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, register succeed");
                    } else {
                        memcpy(response.response_buf, "REG FAILED", strlen("REG FAILED"));
                        printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, register failed");
                    }

                    if (handle_1 == NULL) {
                        handle_1 = handle;
                    } else if (handle_2 == NULL) {
                        handle_2 = handle;
                    } else {
                        handle_3 = handle;
                    }
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, AUDIO_SOURCE_CONTROL_STR_UNREGISTER, AUDIO_SOURCE_CONTROL_STR_UNREGISTER_LEN)) {

            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, AUDIO_SOURCE_CONTROL_STR_TAKE_1, AUDIO_SOURCE_CONTROL_STR_TAKE_1_LEN)) {
                if (handle_1 != NULL) {
                    result = audio_source_control_request_resource(handle_1, 1);
                    if (result == AUDIO_SOURCE_CONTROL_RESULT_OK) {
                    } else {
                        memcpy(response.response_buf, "TAKE 1 FAILED", strlen("TAKE 1 FAILED"));
                    }
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [TAKE] resource 1 result : %d", result);
                } else {
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [TAKE] resource 1 is NULL");
                    memcpy(response.response_buf, "[TAKE] HANDLE 1 NULL", strlen("[TAKE] HANDLE 1 NULL"));
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, AUDIO_SOURCE_CONTROL_STR_TAKE_2, AUDIO_SOURCE_CONTROL_STR_TAKE_2_LEN)) {
                if (handle_2 != NULL) {
                    result = audio_source_control_request_resource(handle_2, 2);
                    if (result == AUDIO_SOURCE_CONTROL_RESULT_OK) {
                    } else {
                        memcpy(response.response_buf, "TAKE 2 FAILED", strlen("TAKE 2 FAILED"));
                    }
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [TAKE] resource 2 result : %d", result);
                } else {
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [TAKE] resource 2 is NULL");
                    memcpy(response.response_buf, "[TAKE] HANDLE 2 NULL", strlen("[TAKE] HANDLE 2 NULL"));
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, AUDIO_SOURCE_CONTROL_STR_TAKE_3, AUDIO_SOURCE_CONTROL_STR_TAKE_3_LEN)) {
                if (handle_3 != NULL) {
                    result = audio_source_control_request_resource(handle_3, 3);
                    if (result == AUDIO_SOURCE_CONTROL_RESULT_OK) {
                    } else {
                        memcpy(response.response_buf, "TAKE 3 FAILED", strlen("TAKE 3 FAILED"));
                    }
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [TAKE] resource 3 result : %d", result);
                } else {
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [TAKE] resource 3 is NULL");
                    memcpy(response.response_buf, "[TAKE] HANDLE 3 NULL", strlen("[TAKE] HANDLE 3 NULL"));
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, AUDIO_SOURCE_CONTROL_STR_GIVE_1, AUDIO_SOURCE_CONTROL_STR_GIVE_1_LEN)) {
                if (handle_1 != NULL) {
                    result = audio_source_control_release_resource(handle_1);
                    if (result == AUDIO_SOURCE_CONTROL_RESULT_OK) {
                    } else {
                        memcpy(response.response_buf, "GIVE 1 FAILED", strlen("GIVE 1 FAILED"));
                    }
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [GIVE] resource 1 result : %d", result);
                } else {
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [GIVE] resource 1 is NULL");
                    memcpy(response.response_buf, "[GIVE] HANDLE 1 NULL", strlen("[GIVE] HANDLE 1 NULL"));
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, AUDIO_SOURCE_CONTROL_STR_GIVE_2, AUDIO_SOURCE_CONTROL_STR_GIVE_2_LEN)) {
                if (handle_2 != NULL) {
                    result = audio_source_control_release_resource(handle_2);
                    if (result == AUDIO_SOURCE_CONTROL_RESULT_OK) {
                    } else {
                        memcpy(response.response_buf, "GIVE 2 FAILED", strlen("GIVE 2 FAILED"));
                    }
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [GIVE] resource 2 result : %d", result);
                } else {
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [GIVE] resource 2 is NULL");
                    memcpy(response.response_buf, "[GIVE] HANDLE 2 NULL", strlen("[GIVE] HANDLE 2 NULL"));
                }
            } else if (0 == memcmp(parse_cmd->string_ptr + parse_cmd->name_len + 1, AUDIO_SOURCE_CONTROL_STR_GIVE_3, AUDIO_SOURCE_CONTROL_STR_GIVE_3_LEN)) {
                if (handle_3 != NULL) {
                    result = audio_source_control_release_resource(handle_3);
                    if (result == AUDIO_SOURCE_CONTROL_RESULT_OK) {
                    } else {
                        memcpy(response.response_buf, "GIVE 3 FAILED", strlen("GIVE 3 FAILED"));
                    }
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [GIVE] resource 3 result : %d", result);
                } else {
                    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_at_cmd_handler, [GIVE] resource 3 is NULL");
                    memcpy(response.response_buf, "[GIVE] HANDLE 3 NULL", strlen("[GIVE] HANDLE 3 NULL"));
                }
            }
        }
        break;
    }

    response.response_flag = ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
    response.response_len = strlen((char *)response.response_buf);
    atci_send_response(&response);

    return ATCI_STATUS_OK;
}

static atci_cmd_hdlr_item_t audio_source_control_at_cmd_test_table[] = {
    {
        .command_head = "AT+ASM",    /**< AT command string. */
        .command_hdlr = audio_source_control_at_cmd_handler,
        .hash_value1 = 0,
        .hash_value2 = 0,
    }
};

void audio_source_control_test_init()
{
    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_test_init enter");
    atci_status_t status = atci_register_handler(audio_source_control_at_cmd_test_table, sizeof(audio_source_control_at_cmd_test_table) / sizeof(atci_cmd_hdlr_item_t));
    handle_1 = NULL;
    handle_1 = NULL;
    handle_2 = NULL;

    printf("[AUDIO_SOURCE_CONTROL][AT_CMD_TEST] audio_source_control_test_init register result : %d", status);
}

#endif /* AUDIO_SOURCE_CONTROL_TEST */
