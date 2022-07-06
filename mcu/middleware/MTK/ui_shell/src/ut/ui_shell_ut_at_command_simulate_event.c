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
#include "ui_shell_manager.h"
#include <string.h>
#include <stdlib.h>
#include "ui_shell_al_memory.h"
#include "ut_event_group.h"
#include "airo_key_event.h"

#ifdef WIN32
#define strtok_r(X, Y, Z)       strtok_s((X), (Y), (Z))
#endif

/*--- Function ---*/
atci_status_t atci_cmd_ui_shell_sim_event(atci_parse_cmd_param_t *parse_cmd);

/*
AT+ELED=<op>                |   "OK"
AT+ELED=?                   |   "+ELED=(0,1)","OK"


*/
/* AT command handler  */
atci_status_t atci_cmd_ui_shell_sim_event(atci_parse_cmd_param_t *parse_cmd)
{
    //atci_response_t resonse = {{0}};
    char *param = NULL;
    char *param_val;

    switch (parse_cmd->mode) {
    case ATCI_CMD_MODE_EXECUTION:
        param = parse_cmd->string_ptr + parse_cmd->parse_pos;
        if (strstr(param, "key,") == param) { // start with "key,"
            uint32_t key_id;
            uint32_t key_event;
            char *string_data = NULL;
            size_t string_data_len = 0;
            char *ptr = NULL;

            param += strlen("key,");
            param_val = strtok_r(param, ",", &ptr);
            key_id = atoi(param_val);
            param_val = strtok_r(NULL, ",", &ptr);
            key_event = atoi(param_val);
            param_val = strtok_r(NULL, ",", &ptr);
            if (param_val && strlen(param_val) != 0) {
                string_data = ui_shell_al_malloc(strlen(param_val) + 1);
                string_data_len = strlen(param_val) + 1;
                ui_shell_al_memcpy(string_data, param_val, strlen(param_val) + 1);
            }
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                    (key_id & (0xFF)) | ((key_event & (0xFF)) << 8),
                    string_data, string_data_len, NULL, 0);
        }
        else if (strstr(param, "allow1,") == param) {
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                    (APP1_CARE_KEY_ID & (0xFF)) | ((AIRO_KEY_SHORT_CLICK & (0xFF)) << 8),
                    NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                    (APP2_CARE_KEY_ID & (0xFF)) | ((AIRO_KEY_SHORT_CLICK & (0xFF)) << 8),
                    NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                    (APP2_CARE_KEY_ID & (0xFF)) | ((AIRO_KEY_LONG_PRESS_1 & (0xFF)) << 8),
                    NULL, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                    (TEST_ALLOW_KEY_ID & (0xFF)) | ((AIRO_KEY_SHORT_CLICK & (0xFF)) << 8),
                    (void *)ALL_ALLOW_REQUEST_ID, 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                    (TEST_ALLOW_KEY_ID & (0xFF)) | ((AIRO_KEY_SHORT_CLICK & (0xFF)) << 8),
                    (void *)ALL_PENDING_REQUEST_ID, 0, NULL, 0);

            // Start allow
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                (TEST_ALLOW_KEY_ID & (0xFF)) | ((AIRO_KEY_SLONG & (0xFF)) << 8),
                    "2_1", 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                (TEST_ALLOW_KEY_ID & (0xFF)) | ((AIRO_KEY_SLONG & (0xFF)) << 8),
                    "2", 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                (TEST_ALLOW_KEY_ID & (0xFF)) | ((AIRO_KEY_SLONG & (0xFF)) << 8),
                    "2_2", 0, NULL, 0);
            ui_shell_send_event(false, EVENT_PRIORITY_MIDDLE, EVENT_GROUP_UI_SHELL_KEY,
                (TEST_ALLOW_KEY_ID & (0xFF)) | ((AIRO_KEY_SLONG & (0xFF)) << 8),
                    "1_1", 0, NULL, 0);
        } 
        break;
    default:
        break;
    }

    return ATCI_STATUS_OK;
}
