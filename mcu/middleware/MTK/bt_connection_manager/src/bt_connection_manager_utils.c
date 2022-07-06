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


#ifdef WIN32
#include <osapi.h>
#else
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
#include "bt_timer_external.h"
#endif
#endif /* WIN32 */
#include <string.h>
#include <stdarg.h>
#include "bt_connection_manager_utils.h"

extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);

static bt_cm_timer_record_t g_bt_sink_srv_timer_records[BT_CM_MAX_TIMER];

log_create_module(BT_CM, PRINT_LEVEL_INFO);

void    *bt_cm_memory_alloc(uint16_t size)
{
#ifdef WIN32
    void *memory = (void *)malloc(size);
#else
    void *memory = (void *)pvPortMalloc(size);
#endif /* WIN32 */
    if (NULL != memory) {
        bt_cm_memset(memory, 0, size);
    }
    return memory;
}

void    bt_cm_memory_free(void *point)
{
    if (point) {
#ifdef WIN32
        free(point);
#else
        vPortFree(point);
#endif /* WIN32 */
    }
}

void    *bt_cm_memset(void *ptr, int32_t value, uint32_t num)
{
    return memset(ptr, value, num);
}

void    *bt_cm_memcpy(void *dest, const void *src, uint32_t size)
{
    return memcpy(dest, src, size);
}

int32_t bt_cm_memcmp(const void *dest, const void *src, uint32_t count)
{
    return memcmp(dest, src, count);
}

static bt_cm_timer_record_t *bt_cm_find_timer(uint32_t user_id)
{
    uint32_t index = 0;
    for (index = 0; index < BT_CM_MAX_TIMER; index++) {
        if (user_id == g_bt_sink_srv_timer_records[index].user_id) {
            bt_cmgr_report_id("[BT_CM][TIMER][I] Find tiemr success : %p", 1, &(g_bt_sink_srv_timer_records[index]));
            return g_bt_sink_srv_timer_records + index;
        }
    }
    bt_cmgr_report_id("[BT_CM][TIMER][W] Can't find tiemr by user id : 0x%x", 1, user_id);
    return NULL;
}

void        bt_cm_timer_init(void)
{
    bt_cmgr_report_id("[BT_CM][TIMER][I] bt_cm_timer_init.", 0);
    #ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_timer_ext_init();
    #endif /*MTK_BT_TIMER_EXTERNAL_ENABLE*/
    memset(&g_bt_sink_srv_timer_records, 0, sizeof(g_bt_sink_srv_timer_records));
}

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
void        bt_cm_bt_timer_callback(uint32_t user_id, uint32_t data)
{
    bt_cm_timer_record_t *record = NULL;
    bt_cmgr_report_id("[BT_CM][TIMER][I] bt_sink_srv_timer_callback, user_id:0x%x", 1, user_id);
    if (NULL != (record = bt_cm_find_timer(user_id))) {
        record->user_id = 0;
        if (record->callback) {
            record->callback(record->parmaters);
        }
    }
}
#else /*MTK_BT_TIMER_EXTERNAL_ENABLE*/
void        bt_cm_timer_callback(TimerHandle_t xTimer)
{
    bt_cm_timer_record_t *record = NULL;
    uint32_t user_id = (uint32_t)pvTimerGetTimerID(xTimer);
    bt_cmgr_report_id("[BT_CM][TIMER][I] bt_sink_srv_timer_callback, timer_id:%x user_id:%x", 2, xTimer, user_id);
    if (NULL != (record = bt_cm_find_timer(user_id))) {
        xTimerStop(xTimer, 0);
        xTimerDelete(xTimer, 0);
        record->user_id = 0;
        if (record->callback) {
            record->callback(record->parmaters);
        }
    }
}
#endif /*MTK_BT_TIMER_EXTERNAL_ENABLE*/

void        bt_cm_timer_start(uint32_t user_id, uint32_t delay, bt_cm_timer_callback_t function_p, void *parmaters)
{
    uint32_t index = 0;
    bt_cm_timer_record_t *timer_record = NULL;
    bt_cm_timer_stop(user_id);
    for (index = 0; index < BT_CM_MAX_TIMER; index++) {
        if (0 == g_bt_sink_srv_timer_records[index].user_id) {
            timer_record = g_bt_sink_srv_timer_records + index;
            break;
        }
    }
    if (NULL == timer_record) {
        bt_cm_assert(0 && "BT cm timer buffer not enough");
        return;
    }
    timer_record->user_id = user_id;
    timer_record->callback = function_p;
    timer_record->parmaters = parmaters;

#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
    bt_timer_ext_start(user_id, 0, delay, bt_cm_bt_timer_callback);
    bt_cmgr_report_id("[BT_CM][TIMER][I] bt_cm_timer_start, timer_id:%x, delay:%d", 2, user_id, delay);
#else /*MTK_BT_TIMER_EXTERNAL_ENABLE*/
    timer_record->timer_id = (uint32_t)xTimerCreate("BT_SINK_SRV_TIMER",                /* Just a text name, not used by the kernel. */
                            (delay / portTICK_PERIOD_MS),  /* The timer period in ticks. */
                            pdFALSE,
                            (void *)user_id,
                            bt_sink_srv_timer_callback);

    xTimerStart((TimerHandle_t)timer_record->timer_id, 0);
    bt_cmgr_report_id("[BT_CM][TIMER][I] bt_cm_timer_start, timer_id:%x, user_id:%x, delay:%d", 3, timer_record->timer_id, user_id, delay);
#endif /* MTK_BT_TIMER_EXTERNAL_ENABLE */
}

void        bt_cm_timer_stop(uint32_t user_id)
{
    bt_cm_timer_record_t *record = NULL;
    bt_cmgr_report_id("[BT_CM][TIMER][I] bt_cm_timer_stop, user_id:%x", 1, user_id);
    if (NULL != (record = bt_cm_find_timer(user_id))) {
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
        bt_timer_ext_stop(user_id);
#else /*MTK_BT_TIMER_EXTERNAL_ENABLE*/
        xTimerStop((TimerHandle_t)record->timer_id, 0);
        xTimerDelete((TimerHandle_t)record->timer_id, 0);
#endif /* MTK_BT_TIMER_EXTERNAL_ENABLE*/
        record->user_id = 0;
    }
}

bool        bt_cm_timer_is_exist(uint32_t user_id)
{
    bool result;
    bt_cm_timer_record_t *record = NULL;
    if (NULL != (record = bt_cm_find_timer(user_id))) {
#ifdef MTK_BT_TIMER_EXTERNAL_ENABLE
        if (NULL != bt_timer_ext_find(user_id))
#else /*MTK_BT_TIMER_EXTERNAL_ENABLE*/
        if (xTimerIsTimerActive((TimerHandle_t)(record->timer_id)))
#endif /* MTK_BT_TIMER_EXTERNAL_ENABLE */
        {
            result = true;
        } else {
            result = false;
        }
    } else {
        result = false;
    }
    bt_cmgr_report_id("[BT_CM][TIMER][I] bt_cm_timer_is_exist, user_id:%x, result:%d", 2, user_id, result);
    return result;
}

char        *bt_cm_strfind(char *str, const char *sub)
{
    return strstr(str, sub);
}

char        *bt_cm_strcat(char *dest, const char *src)
{
    return strncat(dest, src, strlen(src));
}

char        *bt_cm_strcpy(char *dest, const char *src)
{
    return strncpy(dest, src, strlen(src) + 1);
}

char        *bt_cm_strncpy(char *dest, const char *src, uint32_t size)
{
    return strncpy(dest, src, size);
}

uint32_t    bt_cm_strlen(char *string)
{
    return strlen(string);
}

int32_t     bt_cm_strnmp(const char *dest, const char *src, uint32_t size)
{
    return strncmp(dest, src, size);
}

uint32_t    bt_cm_util_atoi(const uint8_t *a, uint8_t len)
{
    uint32_t i = 0;

    while (len-- && (*a >= '0') && (*a <= '9'))
        i = (i * 10) + (*a++ - '0');

    return i;
}

void        bt_cm_mutex_lock(void)
{
    bt_os_take_stack_mutex();
}

void        bt_cm_mutex_unlock(void)
{
    bt_os_give_stack_mutex();
}

