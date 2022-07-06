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

#include "stdlib.h"
#include <string.h>
#include "ui_shell_al_log.h"
#include "ui_shell_al_task.h"
#include "FreeRTOS.h"
#include "task.h"
#include "portmacro.h"
#include "portable.h"

//#define HK_MEMORY_DEBUG     1

#ifdef HK_MEMORY_DEBUG
uint32_t s_current_memory = 0;
uint32_t s_max_memory = 0;
static SemaPhoreHandle_t s_almem_mutex = NULL;

#define AL_MEM_LOCK_MUTEX()                  \
    do {                                    \
        if (!s_almem_mutex) {               \
            s_almem_mutex = al_create_mutex();\
        }                                   \
        if (s_almem_mutex) {                \
            al_mutex_lock(s_almem_mutex);   \
        }                                   \
    } while (0)

#define AL_MEM_UNLOCK_MUTEX()                \
    do {                                    \
        if (s_almem_mutex) {                \
            al_mutex_unlock(s_almem_mutex); \
        }                                   \
    } while (0)


typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK *pxNextFreeBlock;   /*<< The next free block in the list. */
    size_t xBlockSize;                      /*<< The size of the free block. */
#if (MTK_SUPPORT_HEAP_DEBUG == 1)
    void* vLinkRegAddr;
#endif
} BlockLink_t;

typedef struct _memory_size {
    void *address;
    uint32_t size;
    struct _memory_size *next;
}memory_size_t;

static memory_size_t g_memory_first = { NULL, 0 , NULL};

static void register_memory_size(void *address, uint32_t size)
{
    uint32_t i;
    memory_size_t *memory = &g_memory_first;
    memory_size_t *current = NULL;
    if (address != NULL && size != 0) {
        current = pvPortMalloc(sizeof(memory_size_t));
        current->address = address;
        current->size = size;
        current->next = NULL;
        UI_SHELL_LOG_MSGID_I("register_memory_size, register = %x", 1, current);
        AL_MEM_LOCK_MUTEX();
        current->next = memory->next;
        memory->next = current;
        s_current_memory += size;
        if (s_current_memory > s_max_memory) {
            s_max_memory = s_current_memory;
        }
        AL_MEM_UNLOCK_MUTEX();
    }
}

static uint32_t release_memory_size(void *address)
{
    uint32_t i;
    uint32_t result = 0;
    memory_size_t *memory = &g_memory_first;
    memory_size_t *target = NULL;
    AL_MEM_LOCK_MUTEX();
    while (memory->next != NULL) {
        if (memory->next->address == address) {
            target = memory->next;
            memory->next = target->next;
            break;
        }
        memory = memory->next;
    }
    UI_SHELL_LOG_MSGID_I("release_memory_size, target = %x", 1, target);
    AL_MEM_UNLOCK_MUTEX();
    if (target) {
        void *register_memory = NULL;
        register_memory = target;
        result = target->size;
        s_current_memory -= target->size;
        vPortFree(target);
    }
    return result;
}

#endif

void* ui_shell_al_malloc(uint32_t num_bytes)
{
    void *result = NULL;
    if (num_bytes <= 0) {
        UI_SHELL_LOG_MSGID_I("ui_shell_al_malloc memory size is 0", 0);
        return NULL;
    }
    result = (void*)pvPortMalloc((size_t)(num_bytes));
#ifdef HK_MEMORY_DEBUG
    register_memory_size(result, num_bytes);
    UI_SHELL_LOG_MSGID_I("ui_shell_al_malloc memory(%x) size = %d, current, result, max, system current, system max = %d %d %d %d", 6,
            result, num_bytes, s_current_memory, s_max_memory, xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
#endif
    return result;
}


void* ui_shell_al_calloc(uint32_t num_bytes)
{
	void* mem = ui_shell_al_malloc(num_bytes);
	
	if (mem)
	{
		memset(mem, 0, num_bytes);
	}
	
	return mem;
}

void *ui_shell_al_realloc(void *org_address, uint32_t num_bytes)
{
    void *result = NULL;
#ifdef HK_MEMORY_DEBUG
    uint32_t org_size = release_memory_size(org_address);
#endif
    result = (void *)pvPortRealloc(org_address, num_bytes);
#ifdef HK_MEMORY_DEBUG
    register_memory_size(result, num_bytes);
    UI_SHELL_LOG_MSGID_I("ui_shell_al_realloc memory(%x)->(%x) size = (%d)->(%d), current, max, system current, system max = %d %d %d %d", 8,
            org_address, result, org_size, num_bytes, s_current_memory, s_max_memory,
            xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
#endif
    return result;
}

void ui_shell_al_free(void* memory)
{
    if (memory) {
#ifdef HK_MEMORY_DEBUG
        uint32_t size = release_memory_size(memory);
#endif
        vPortFree(memory);
#ifdef HK_MEMORY_DEBUG
        UI_SHELL_LOG_MSGID_I("ui_shell_al_free memory(%x) size = %d, current, max, system current, system max = %d %d %d %d", 6,
                memory, size, s_current_memory, s_max_memory, xPortGetFreeHeapSize(), xPortGetMinimumEverFreeHeapSize());
#endif
    }
}


/* Enlarge memory to n times of the original size to make the new buffer size no less than num_bytes. */
void* ui_shell_al_enlarge(void** memory, uint32_t num_bytes)
{
    uint32_t len = 0, i = 2;
    void* new_memory = NULL;
    
    if (!memory || !(*memory))
    {
        return NULL;
    }

    len = strlen(*memory);
    
    if (!num_bytes || len >= num_bytes)
    {
        return *memory;
    }

    while (i * len < num_bytes)
    {
        i++;
    }
    
    new_memory = ui_shell_al_calloc(i * len);
    if (!new_memory)
    {
        return NULL;
    }
    
    memcpy(new_memory, *memory, len);
    ui_shell_al_free(*memory);
    
    *memory = new_memory;
    return new_memory;
}

void *ui_shell_al_zalloc(uint32_t num_bytes)
{
    void *result = NULL;
    if (num_bytes <= 0) {
        UI_SHELL_LOG_MSGID_I("ui_shell_al_zalloc memory size is 0", 0);
    } else {
        result = ui_shell_al_malloc((size_t)num_bytes);
        if (result) {
            memset(result, 0, num_bytes);
        }
    }
    return result;
}

void* ui_shell_al_memcpy(void* dest, const void* src, uint32_t length)
{
    return memcpy(dest, src, (size_t)length);
}

void* ui_shell_al_memset(void* dest, int32_t value, uint32_t length)
{
    return memset(dest, value, (size_t)length);
}


int32_t ui_shell_al_memcmp(const void *buf1, const void *buf2, uint32_t count)
{
    return memcmp(buf1, buf2, count);
}

