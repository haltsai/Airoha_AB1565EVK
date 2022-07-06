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

#ifndef __BT_CONNECTION_MANAGER_UTILS_H__
#define __BT_CONNECTION_MANAGER_UTILS_H__
#include <stdint.h>
#include <stdbool.h>
#ifndef WIN32
#include <syslog.h>
#else
#include "osapi.h"
#endif
#include "FreeRTOSConfig.h"

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#define BT_CM_DEBUG_INFO
#define BT_CM_MAX_TIMER     (6)
#define bt_cm_assert        configASSERT

typedef void (*bt_cm_timer_callback_t)(void *parameter);

typedef struct {
    uint32_t                timer_id;
    uint32_t                user_id;
    bt_cm_timer_callback_t  callback;
    void                    *parmaters;
} bt_cm_timer_record_t;

#ifdef BT_CM_DEBUG_INFO
#define bt_cmgr_report(_message,...) LOG_I(BT_CM, (_message), ##__VA_ARGS__)
#define bt_cmgr_report_id(_message, arg_cnt,...) LOG_MSGID_I(BT_CM, _message, arg_cnt, ##__VA_ARGS__)
#else
#define bt_cmgr_report(_message,...);
#define bt_cmgr_report_id(_message, arg_cnt,...);
#endif

void        *bt_cm_memory_alloc(uint16_t size);

void        bt_cm_memory_free(void *point);

void        *bt_cm_memset(void *ptr, int32_t value, uint32_t num);

void        *bt_cm_memcpy(void *dest, const void *src, uint32_t size);

int32_t     bt_cm_memcmp(const void *dest, const void *src, uint32_t count);

char        *bt_cm_strfind(char *str, const char *sub);

char        *bt_cm_strcat(char *dest, const char *src);

char        *bt_cm_strcpy(char *dest, const char *src);

uint32_t    bt_cm_strlen(char *string);

char        *bt_cm_strncpy(char *dest, const char *src, uint32_t size);

int32_t     bt_cm_strnmp(const char *dest, const char *src, uint32_t size);

uint32_t    bt_cm_util_atoi(const uint8_t *a, uint8_t len);

void        bt_cm_mutex_lock(void);

void        bt_cm_mutex_unlock(void);

void        bt_cm_timer_init(void);

void        bt_cm_timer_start(uint32_t user_id, uint32_t delay, bt_cm_timer_callback_t function_p, void *parmaters);

void        bt_cm_timer_stop(uint32_t user_id);

bool        bt_cm_timer_is_exist(uint32_t user_id);

#ifdef __cplusplus
}
#endif

#endif /* __BT_CONNECTION_MANAGER_UTILS_H__ */
