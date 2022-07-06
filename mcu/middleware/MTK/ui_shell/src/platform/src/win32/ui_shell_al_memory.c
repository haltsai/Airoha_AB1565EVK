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

#include "ui_shell_al_memory.h"
//#include "stdafx.h"
#include "stdlib.h"
#include <string.h>

//static uint32_t g_memory_count = 0;

void* ui_shell_al_malloc(uint32_t num_bytes)
{
    void *ret;
    if (num_bytes <= 0) {
        return NULL;
    }

    //g_memory_count ++;
    ret = malloc(num_bytes);

    return ret;
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
    void *ret = realloc(org_address, num_bytes);
    return ret;
}

void ui_shell_al_free(void* memory)
{
    //g_memory_count --;
    
    free(memory);
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
    void *ret;
    if (num_bytes <= 0) {
        return NULL;
    }

    //g_memory_count ++;
    ret = calloc(1, num_bytes);

    return ret;
}

void* ui_shell_al_memcpy(void* dest, const void* src, uint32_t length)
{
    return memcpy(dest, src, length);
}

void* ui_shell_al_memset(void* dest, int32_t value, uint32_t length)
{
    return memset(dest, value, length);
}

int32_t ui_shell_al_memcmp(const void *buf1, const void *buf2, uint32_t count)
{
    return memcmp(buf1, buf2, count);
}


