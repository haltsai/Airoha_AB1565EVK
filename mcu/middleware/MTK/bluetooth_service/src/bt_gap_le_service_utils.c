/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
#endif /* WIN32 */
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "bt_os_layer_api.h"
#include "bt_gap_le_service_utils.h"

/**********************************utils************************/

log_create_module(BT_GAP_LE_SRV, PRINT_LEVEL_INFO);

/********************************Common Function*****************************/
extern void bt_os_take_stack_mutex(void);
extern void bt_os_give_stack_mutex(void);

void bt_gap_le_srv_mutex_lock(void)
{
    bt_os_take_stack_mutex();
}

void bt_gap_le_srv_mutex_unlock(void)
{
    bt_os_give_stack_mutex();
}

void *bt_gap_le_srv_memset(void *ptr, int32_t value, uint32_t num)
{
    return memset(ptr, value, num);
}

void *bt_gap_le_srv_memcpy(void *dest, const void *src, uint32_t size)
{
    return memcpy(dest, src, size);
}

int32_t bt_gap_le_srv_memcmp(const void *dest, const void *src, uint32_t count)
{
    return memcmp(dest, src, count);
}

void *bt_gap_le_srv_memory_alloc(uint32_t size)
{
#ifdef WIN32
    void *memory = (void *)malloc(size);
#else
    void *memory = (void *)pvPortMalloc(size);
#endif /* WIN32 */
    if (NULL != memory) {
        bt_gap_le_srv_memset(memory, 0, size);
    }
    return memory;
}

void bt_gap_le_srv_memory_free(void *point)
{
    if (point) {
#ifdef WIN32
        free(point);
#else
        vPortFree(point);
#endif /* WIN32 */
    }
}

void bt_gap_le_srv_u16_to_u8(void *dst, uint16_t src)
{
    uint8_t *u8ptr = dst;

    u8ptr[0] = (uint8_t)src;
    u8ptr[1] = (uint8_t)(src >> 8);
}

void bt_gap_le_srv_u32_to_u8(void *dst, uint32_t src)
{
    uint8_t *u8ptr = dst;

    u8ptr[0] = (uint8_t)src;
    u8ptr[1] = (uint8_t)(src >> 8);
    u8ptr[2] = (uint8_t)(src >> 16);
    u8ptr[3] = (uint8_t)(src >> 24);
}
void bt_gap_le_srv_linknode_insert_node(bt_gap_le_srv_linknode_t *head, bt_gap_le_srv_linknode_t *src, bt_gap_le_srv_linknode_position pos)
{
    bt_gap_le_srv_linknode_t *tmp = head;
    if (pos == BT_GAP_LE_SRV_NODE_BACK) {
        while (tmp->front != NULL) {
            tmp = tmp->front;
        }
    }
    src->front = tmp->front;
    tmp->front = src;
}

bt_gap_le_srv_linknode_t *bt_gap_le_srv_linknode_remove_node(bt_gap_le_srv_linknode_t *head, bt_gap_le_srv_linknode_t *src)
{
    bt_gap_le_srv_linknode_t *tmp;
    tmp = head;
    while (tmp) {
        if (tmp->front == src)
        {
            tmp->front = src->front;
            src->front = NULL;
            break;
        }
        tmp = tmp->front;
        if (tmp == head) {
            return NULL;
        }
    }
    return tmp;
}
