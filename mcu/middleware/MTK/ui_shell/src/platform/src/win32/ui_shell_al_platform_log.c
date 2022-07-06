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

#include "ui_shell_al_log.h"
#include "ui_shell_al_platform_log.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <ui_shell_al_memory.h>
#include <windows.h>

ui_shell_al_create_log_module(ui_shell, UI_SHELL_DEBUG_LEVEL);

const char* const LOG_TYPE_STRING[] =
{
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};

void ui_shell_al_printf(ui_shell_al_log_control_block_t module, ui_shell_al_log_type_t type, const  char* format, ...)
{
    if (type >= module.print_level) {
        va_list ap;
        uint32_t type_len = strlen(LOG_TYPE_STRING[type]);
        uint32_t len = strlen(format);
        uint32_t module_len = strlen(module.module_name);
        uint32_t total_len = module_len + 2 + len + 2 + type_len + 1 + 13 + 2;
        char *new_format = (char *)ui_shell_al_malloc(total_len);
        SYSTEMTIME now;
        GetLocalTime(&now);
        sprintf_s(new_format, total_len, "%02d:%02d:%02d.%03d %s:[%s] %s\r\n",
                now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
                LOG_TYPE_STRING[type], module.module_name, format);

        va_start(ap, format);
        vprintf(new_format, ap);
        va_end(ap);
        
        ui_shell_al_free(new_format);
    }
}

void ui_shell_al_dumpf(ui_shell_al_log_control_block_t module, ui_shell_al_log_type_t type, const  char* format,
        uint8_t *data, uint32_t size, ...)
{
    if (type >= module.print_level) {
        uint32_t i;
        va_list ap;
        uint32_t type_len = strlen(LOG_TYPE_STRING[type]);
        uint32_t len = strlen(format);
        uint32_t module_len = strlen(module.module_name);
        uint32_t total_len = module_len + 2 + len + 2 + type_len + 1 + 13 + 2;
        char *new_format = (char *)ui_shell_al_malloc(total_len);
        SYSTEMTIME now;
        GetLocalTime(&now);
        sprintf_s(new_format, total_len, "%02d:%02d:%02d.%03d %s:[%s] %s\r\n",
                now.wHour, now.wMinute, now.wSecond, now.wMilliseconds,
                LOG_TYPE_STRING[type], module.module_name, format);

        va_start(ap, size);
        vprintf(new_format, ap);
        va_end(ap);

        ui_shell_al_free(new_format);
        for (i = 0; i < size; i++) {
            if (i % 32 == 0 && i != 0) {
                printf("\n");
            }
            printf("%02X ", *(data + i));
        }
    }
}
