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

// For Register AT command handler
// System head file

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "at_command.h"
#include "syslog.h"

#if defined(MTK_MNL_ENABLE) && (PRODUCT_VERSION == 3335)
#include "gnss_dsp_type.h"

#define LOGE(fmt, arg_cnt, arg...)   LOG_MSGID_E(atcmd, "[mnl]"fmt, arg_cnt, ##arg)
#define LOGW(fmt, arg_cnt, arg...)   LOG_MSGID_W(atcmd, "[mnl]"fmt, arg_cnt, ##arg)
#define LOGI(fmt, arg_cnt, arg...)   LOG_MSGID_I(atcmd ,"[mnl]"fmt, arg_cnt, ##arg)


/*--- Function ---*/
atci_status_t atci_cmd_power_control(atci_parse_cmd_param_t *parse_cmd);

// AT command handler
atci_status_t atci_cmd_power_control(atci_parse_cmd_param_t *parse_cmd)
{
    atci_response_t output = {{0}};
    char* command_arg1;
    char* command_arg2;
    char* command_arg3;
    char* command_e;
    char buffer[32];

    switch (parse_cmd->mode) {
        case ATCI_CMD_MODE_TESTING:    // rec: AT+LPMODE=?
            strcpy((char*)output.response_buf, "+LPMODE=\"a,b,c\"\r\n"
                                               "a: (dsp_type)     0: Dual, 1: L1, 2: L5\r\n"
                                               "b: (next_lp_mode) 0: Normal, 1: Sleep, 2: Stop\r\n"
                                               "c: (duration)     units: msec\r\n"
                                               "Set next low power mode in c msec\r\n");
            output.response_len = strlen((char*)output.response_buf);
            atci_send_response(&output);
            break;
        case ATCI_CMD_MODE_EXECUTION:
        {
            uint8_t dsp_lp_mode;
            uint16_t timer_sec;
            GNSS_DSP_TYPE dsp_type;
            command_arg1 = (char *)strstr((char *)parse_cmd->string_ptr, "\"") + 1;
            command_arg2 = (char *)strstr((char *)command_arg1, ",") + 1;
            command_arg3 = (char *)strstr((char *)command_arg2, ",") + 1;
            command_e = (char *)strstr((char *)command_arg3, "\"");

            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, command_arg1, command_arg2 - command_arg1);
            dsp_type = (uint16_t)atoi(buffer);

            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, command_arg2, command_arg3 - command_arg2);
            dsp_lp_mode = (uint8_t)atoi(buffer);

            memset(buffer, 0, sizeof(buffer));
            memcpy(buffer, command_arg3, command_e - command_arg3);
            timer_sec = (uint16_t)atoi(buffer);

            sprintf((char *)output.response_buf, "+LPMODE: dsp_type: %d, mode: %d, duration: %d\r\nOK", dsp_type, dsp_lp_mode, timer_sec);
            output.response_len = strlen((char*) output.response_buf);
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_AUTO_APPEND_LF_CR;
            atci_send_response(&output);

            LOGI("dsp_type: %d, set LP mode: %d, duration: %d", 3, dsp_type, dsp_lp_mode, timer_sec);

            gnss_dsp_set_lp_mode(dsp_type, dsp_lp_mode, timer_sec);

            break;
        }
        default :
            output.response_flag = 0 | ATCI_RESPONSE_FLAG_APPEND_ERROR;
            atci_send_response(&output);
            break;
    }
    return ATCI_STATUS_OK;
}

#endif
