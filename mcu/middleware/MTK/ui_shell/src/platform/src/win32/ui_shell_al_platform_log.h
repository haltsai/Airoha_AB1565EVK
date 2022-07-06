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

#ifndef __UI_SHELL_AL_PLATFORM_LOG_H__
#define __UI_SHELL_AL_PLATFORM_LOG_H__

#include <stdint.h>
#include <stdbool.h>

#define UI_SHELL_DEBUG_LEVEL        UI_SHELL_AL_LOG_TYPE_INFO

typedef enum {
    UI_SHELL_AL_LOG_TYPE_DEBUG,
    UI_SHELL_AL_LOG_TYPE_INFO,
    UI_SHELL_AL_LOG_TYPE_WARN,
    UI_SHELL_AL_LOG_TYPE_ERROR
} ui_shell_al_log_type_t;

typedef struct {
    char *module_name;
    ui_shell_al_log_type_t   print_level;
} ui_shell_al_log_control_block_t;

#define ui_shell_al_create_log_module(_module, _level) \
ui_shell_al_log_control_block_t ui_shell_al_log_control_block_##_module = \
{ \
    #_module, \
    (_level) \
}

void ui_shell_al_printf(ui_shell_al_log_control_block_t module, ui_shell_al_log_type_t type, const  char* format, ...);
void ui_shell_al_dumpf(ui_shell_al_log_control_block_t module, ui_shell_al_log_type_t type, const  char* format,
        uint8_t *data, uint32_t size, ...);

#define ui_shell_al_print_log(_module, _type, _format, ...)\
do { \
    extern ui_shell_al_log_control_block_t ui_shell_al_log_control_block_##_module; \
    ui_shell_al_printf(ui_shell_al_log_control_block_##_module, _type, _format, ##__VA_ARGS__); \
} while(0)

#define ui_shell_al_dump_log(_module, _type, _format, ...)\
do { \
    extern ui_shell_al_log_control_block_t ui_shell_al_log_control_block_##_module; \
    ui_shell_al_dumpf(ui_shell_al_log_control_block_##_module, _type, _format, ##__VA_ARGS__); \
} while(0)

#define UI_SHELL_LOG_E(msg, ...)                    ui_shell_al_print_log(ui_shell, UI_SHELL_AL_LOG_TYPE_ERROR, (msg), ##__VA_ARGS__)
#define UI_SHELL_LOG_W(msg, ...)                    ui_shell_al_print_log(ui_shell, UI_SHELL_AL_LOG_TYPE_WARN, (msg), ##__VA_ARGS__)
#define UI_SHELL_LOG_I(msg, ...)                    ui_shell_al_print_log(ui_shell, UI_SHELL_AL_LOG_TYPE_INFO, (msg), ##__VA_ARGS__)
#define UI_SHELL_LOG_D(msg, ...)                    ui_shell_al_print_log(ui_shell, UI_SHELL_AL_LOG_TYPE_DEBUG, (msg), ##__VA_ARGS__)

#define UI_SHELL_LOG_MSGID_E(msg, param_cnt, ...)   ui_shell_al_print_log(ui_shell, UI_SHELL_AL_LOG_TYPE_ERROR, (msg), ##__VA_ARGS__)
#define UI_SHELL_LOG_MSGID_W(msg, param_cnt, ...)   ui_shell_al_print_log(ui_shell, UI_SHELL_AL_LOG_TYPE_WARN, (msg), ##__VA_ARGS__)
#define UI_SHELL_LOG_MSGID_I(msg, param_cnt, ...)   ui_shell_al_print_log(ui_shell, UI_SHELL_AL_LOG_TYPE_INFO, (msg), ##__VA_ARGS__)
#define UI_SHELL_LOG_MSGID_D(msg, param_cnt, ...)   ui_shell_al_print_log(ui_shell, UI_SHELL_AL_LOG_TYPE_DEBUG, (msg), ##__VA_ARGS__)

#define UI_SHELL_LOG_DUMP_E(msg, buffer, len, ...)  ui_shell_al_dump_log(ui_shell, UI_SHELL_AL_LOG_TYPE_ERROR, (msg), (buffer), (len), ##__VA_ARGS__)
#define UI_SHELL_LOG_DUMP_W(msg, buffer, len, ...)  ui_shell_al_dump_log(ui_shell, UI_SHELL_AL_LOG_TYPE_WARN, (msg), (buffer), (len), ##__VA_ARGS__)
#define UI_SHELL_LOG_DUMP_I(msg, buffer, len, ...)  ui_shell_al_dump_log(ui_shell, UI_SHELL_AL_LOG_TYPE_INFO, (msg), (buffer), (len), ##__VA_ARGS__)
#define UI_SHELL_LOG_DUMP_D(msg, buffer, len, ...)  ui_shell_al_dump_log(ui_shell, UI_SHELL_AL_LOG_TYPE_DEBUG,  (msg), (buffer), (len), ##__VA_ARGS__)


#endif/**__UI_SHELL_AL_PLATFORM_LOG_H__**/
