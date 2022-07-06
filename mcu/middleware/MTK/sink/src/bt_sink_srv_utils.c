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

#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include <string.h>
#include <stdarg.h>
#include "bt_sink_srv_utils.h"

extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);

log_create_module(sink_srv, PRINT_LEVEL_INFO);
log_create_module(sink_music, PRINT_LEVEL_INFO);

void *bt_sink_srv_memory_alloc(uint16_t size)
{
    void *memory = (void *)pvPortMalloc(size);
    bt_sink_srv_memset(memory, 0, size);
    return memory;
}

void bt_sink_srv_memory_free(void *point)
{
    if (point) {
        vPortFree(point);
    }
}

void *bt_sink_srv_memset(void *ptr, int32_t value, uint32_t num)
{
    return memset(ptr, value, num);
}

void *bt_sink_srv_memcpy(void *dest, const void *src, uint32_t size)
{
    return memcpy(dest, src, size);
}

int32_t bt_sink_srv_memcmp(const void *dest, const void *src, uint32_t count)
{
    return memcmp(dest, src, count);
}

#if 0
char *bt_sink_srv_strfind(char *str, const char *sub)
{
    return strstr(str, sub);
}

char *bt_sink_srv_strcat(char *dest, const char *src)
{
    return strncat(dest, src, strlen(src));
}

char *bt_sink_srv_strcpy(char *dest, const char *src)
{
    return strncpy(dest, src, strlen(src) + 1);
}

char *bt_sink_srv_strncpy(char *dest, const char *src, uint32_t size)
{
    return strncpy(dest, src, size);
}
#endif

uint32_t bt_sink_srv_strlen(char *string)
{
    return strlen(string);
}

int32_t bt_sink_srv_strnmp(const char *dest, const char *src, uint32_t size)
{
    return strncmp(dest, src, size);
}

uint32_t bt_sink_srv_util_atoi(const uint8_t *a, uint8_t len)
{
    uint32_t i = 0;

    while (len-- && (*a >= '0') && (*a <= '9'))
        i = (i * 10) + (*a++ - '0');

    return i;
}

#if 0
void bt_sink_srv_isr_disable(void)
{
    taskDISABLE_INTERRUPTS();
}
    
void bt_sink_srv_isr_enable(void)
{
    taskENABLE_INTERRUPTS();
}
#endif

void bt_sink_srv_mutex_lock(void)
{
    bt_os_take_stack_mutex();
}

void bt_sink_srv_mutex_unlock(void)
{
    bt_os_give_stack_mutex();
}

