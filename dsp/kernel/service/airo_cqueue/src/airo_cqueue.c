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

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#include "hal.h"
#include "airo_cqueue.h"

void *airo_cqueue_create(uint32_t queue_len, uint32_t item_size, void *buffer)
{
    airo_cqueue_t *pQueue = NULL;
    //pQueue = (airo_cqueue_t *)malloc(sizeof(airo_cqueue_t) + queue_len * item_size);
    pQueue = (airo_cqueue_t *)buffer;
    pQueue->head = 0;
    pQueue->tail = 0;
    pQueue->length = queue_len;
    pQueue->item_size = item_size;
    pQueue->used_item = 0;
    pQueue->is_full = false;

    return (void *)pQueue;
}

bool airo_cqueue_send(void *qHandle, const void *const pItem)
{
    uint32_t mask;
    airo_cqueue_t *pQueue = (airo_cqueue_t *)qHandle;
    uint8_t *pBuffer = (uint8_t *)qHandle + sizeof(airo_cqueue_t);
    bool ret;

    assert(pQueue);

    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (airo_cqueue_is_full((void *)pQueue)) {
        ret = false;

    } else {
        memcpy((void *)(pBuffer + pQueue->tail * pQueue->item_size), pItem, pQueue->item_size);
        pQueue->used_item += 1;

        if (pQueue->used_item == pQueue->length) {
            pQueue->is_full = true;
        }

        pQueue->tail = (pQueue->tail + 1) % pQueue->length;

        ret = true;
    }
    hal_nvic_restore_interrupt_mask(mask);

    return ret;
}


bool airo_cqueue_receive(void *qHandle, void *const pUser_buffer)
{
    uint32_t mask;
    airo_cqueue_t *pQueue = (airo_cqueue_t *)qHandle;
    uint8_t *pBuffer = (uint8_t *)qHandle + sizeof(airo_cqueue_t);
    bool ret;
    assert(pQueue);

    hal_nvic_save_and_set_interrupt_mask(&mask);
    if (airo_cqueue_is_empty((void *) pQueue)) {
        ret = false;

    } else {
        memcpy(pUser_buffer, (void *)(pBuffer + pQueue->head * pQueue->item_size), pQueue->item_size);
        pQueue->used_item -= 1;

        if (pQueue->is_full) {
            pQueue->is_full = false;
        }

        pQueue->head = (pQueue->head + 1) % pQueue->length;
        ret = true;

    }
    hal_nvic_restore_interrupt_mask(mask);

    return ret;
}


bool airo_cqueue_is_full(void *qHandle)
{
    airo_cqueue_t *pQueue = (airo_cqueue_t *)qHandle;

    assert(pQueue);
    return pQueue->is_full;
}

bool airo_cqueue_is_empty(void *qHandle)
{
    uint32_t mask;
    airo_cqueue_t *pQueue = (airo_cqueue_t *)qHandle;
    bool ret;
    assert(pQueue);

    hal_nvic_save_and_set_interrupt_mask(&mask);
    ret = ((pQueue->is_full == false) && (pQueue->head == pQueue->tail)) ? true : false;
    hal_nvic_restore_interrupt_mask(mask);

    return ret;
}

uint32_t airo_cqueue_message_waiting(void *qHandle)
{
    airo_cqueue_t *pQueue = (airo_cqueue_t *)qHandle;

    assert(pQueue);
    return pQueue->used_item;
}


