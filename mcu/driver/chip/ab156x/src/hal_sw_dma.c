/* Copyright Statement:
 *
 * (C) 2018  Airoha Technology Corp. All rights reserved.
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

#include "hal_sw_dma.h"

#ifdef HAL_SW_DMA_MODULE_ENABLED

#include <stdio.h>
#include <assert.h>
#include "hal_sw_dma_internal.h"
#include "hal_nvic.h"
#ifdef HAL_CACHE_MODULE_ENABLED
#include "hal_cache.h"
#endif
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager_internal.h"
#endif

typedef struct {
    uint8_t Read;
    uint8_t Write;
    uint8_t Length;
} sw_dma_buffer_info_t;

#define BWrite(Buffer)          (Buffer->Write)
#define BRead(Buffer)           (Buffer->Read)
#define BLength(Buffer)         (Buffer->Length)

#define Buf_init(_Buffer,_uTotalSize)   \
{                                       \
   sw_dma_buffer_info_t *_Buf=_Buffer;  \
   _Buf->Read = 0;                      \
    _Buf->Write = 0;                    \
    _Buf->Length = _uTotalSize;         \
}

#define Buf_GetRoomLeft(_Buffer,_RoomLeft)      \
{                                               \
   sw_dma_buffer_info_t *_Buf=_Buffer;          \
   if ( BRead(_Buf) <= BWrite(_Buf) )           \
    {                                           \
      _RoomLeft = BLength(_Buf) - BWrite(_Buf) + BRead(_Buf) - 1;   \
    }                                                   \
    else                                                \
    {                                                   \
        _RoomLeft = BRead(_Buf) - BWrite(_Buf) - 1;     \
    }                                                   \
}

#define Buf_Push(_Buffer)                     \
{                                             \
   sw_dma_buffer_info_t *_Buf=_Buffer;        \
   if(BWrite(_Buf) >= (BLength(_Buf) - 1))   \
   {                                           \
    BWrite(_Buf) = 0;                          \
   }                                           \
   else                                        \
   {                                           \
    BWrite(_Buf)++;                            \
   }                                           \
}

#define Buf_GetAvail(_Buffer,_Available)         \
{                                                 \
   sw_dma_buffer_info_t *_Buf = _Buffer;          \
    _Available = 0;                               \
    if (BWrite(_Buf) >= BRead(_Buf))              \
        _Available = BWrite(_Buf) - BRead(_Buf);   \
    else                                           \
        _Available = BLength(_Buf) - BRead(_Buf) + BWrite(_Buf); \
}

#define Buf_Pop(_Buffer,_pop_num)        \
{                                        \
   sw_dma_buffer_info_t *_Buf = _Buffer; \
    BRead(_Buf)+= _pop_num;              \
    if (BRead(_Buf) >= BLength(_Buf))    \
    {                                    \
        BRead(_Buf) -= BLength(_Buf);    \
    }                                    \
}

static sw_dma_buffer_info_t sw_dma_buffer[1];
static bool g_sw_dma_init_done = false;
static volatile bool g_sw_dma_is_ongoing = false;
static hal_sw_dma_config_info_t sw_dma_internal_config_info[SW_DMA_USER_NUM];
static const uint8_t sw_dma_alignment_result[2][3] = {
    {1, 2, 4},
    {4, 8, 16}
};

static hal_sw_gdma_status_t sw_gdma_parameter_check(hal_sw_dma_config_info_t *info)
{
    uint8_t length_alignment, addr_alignment, address_align, length_align;

    if (((info->length >> info->h_size) < 1) ||
        ((info->length >> info->h_size) > (GDMA_COUNT_BIT_MASK >> GDMA_COUNT_BIT_OFFSET))) {
        return HAL_SW_DMA_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable(info->destination_address)) {
        return HAL_SW_DMA_STATUS_INVALID_PARAMETER;
    }
    if (true == hal_cache_is_cacheable(info->source_address)) {
        return HAL_SW_DMA_STATUS_INVALID_PARAMETER;
    }
#endif
    addr_alignment = sw_dma_alignment_result[0][info->h_size];
    address_align = info->source_address & (addr_alignment - 1);
    length_alignment = sw_dma_alignment_result[(info->transfer_type >> 1)][info->h_size];
    length_align = info->length & (length_alignment - 1);
    if ((address_align != 0) || (length_align != 0)) {
        return HAL_SW_DMA_STATUS_INVALID_PARAMETER;
    }

    return HAL_SW_DMA_STATUS_OK;
}

void sw_dma_irq_handler(hal_nvic_irq_t irq_number)
{
    uint8_t sw_dma_read_pointer;
    uint32_t room_available;
    hal_sw_dma_callback_t callback;
    hal_sw_dma_config_info_t config_info;
    void *argument;
    uint32_t irq_status;

    if (irq_number != MCU_DMA_IRQn) {
        assert(0);
    }

    if (!SW_DMA_IS_TRIGGER_INTERRUPT()) {
        return;
    }
    sw_dma_clear_irq();

    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    sw_dma_read_pointer = BRead(sw_dma_buffer);
    callback = sw_dma_internal_config_info[sw_dma_read_pointer].func;
    argument = sw_dma_internal_config_info[sw_dma_read_pointer].argument;
    Buf_Pop(sw_dma_buffer, 1);
    g_sw_dma_is_ongoing = false;
    hal_nvic_restore_interrupt_mask(irq_status);

    if (callback != NULL) {
        callback(HAL_SW_DMA_EVENT_TRANSACTION_SUCCESS, argument);
    }

    hal_nvic_save_and_set_interrupt_mask(&irq_status);
    /* check if there has data wait for send */
    Buf_GetAvail(sw_dma_buffer, room_available);
    if (room_available >= 1) {
        /* User may start another transaction in the callback, so need check here */
        if (g_sw_dma_is_ongoing == false) {
            sw_dma_read_pointer = BRead(sw_dma_buffer);
            config_info.source_address = sw_dma_internal_config_info[sw_dma_read_pointer].source_address;
            config_info.destination_address = sw_dma_internal_config_info[sw_dma_read_pointer].destination_address;
            config_info.length = sw_dma_internal_config_info[sw_dma_read_pointer].length;
            config_info.dma_type = sw_dma_internal_config_info[sw_dma_read_pointer].dma_type;
            config_info.h_size = sw_dma_internal_config_info[sw_dma_read_pointer].h_size;
            config_info.transfer_type = sw_dma_internal_config_info[sw_dma_read_pointer].transfer_type;
            sw_dma_start_interrupt(&config_info);
            g_sw_dma_is_ongoing = true;
        }
    }
    hal_nvic_restore_interrupt_mask(irq_status);
}

hal_sw_gdma_status_t hal_sw_gdma_start(hal_sw_dma_config_info_t *info)
{
    uint8_t room_left;
    uint8_t sw_dma_read_pointer;
    uint32_t irq_status;
    hal_sw_gdma_status_t status;
    hal_sw_dma_config_info_t config_info;

    hal_nvic_save_and_set_interrupt_mask(&irq_status);

    /* init SW DMA buffer and HW DMA only once */
    if (g_sw_dma_init_done == false) {
        Buf_init(sw_dma_buffer, SW_DMA_USER_NUM);
        sw_dma_init();
        hal_nvic_register_isr_handler(SW_DMA_CHANNEL_IRQ_NUMBER, sw_dma_irq_handler);
        hal_nvic_enable_irq(SW_DMA_CHANNEL_IRQ_NUMBER);
        g_sw_dma_init_done = true;
    }

    hal_nvic_restore_interrupt_mask(irq_status);

    hal_nvic_save_and_set_interrupt_mask(&irq_status);

    /* check the buffer left room space*/
    Buf_GetRoomLeft(sw_dma_buffer, room_left);
    if (room_left >= 1)  {
        status = sw_gdma_parameter_check(info);
        if (status != HAL_SW_DMA_STATUS_OK) {
            hal_nvic_restore_interrupt_mask(irq_status);
            return status;
        }
        sw_dma_internal_config_info[BWrite(sw_dma_buffer)].source_address = info->source_address;
        sw_dma_internal_config_info[BWrite(sw_dma_buffer)].destination_address = info->destination_address;
        sw_dma_internal_config_info[BWrite(sw_dma_buffer)].length = info->length;
        sw_dma_internal_config_info[BWrite(sw_dma_buffer)].func = info->func;
        sw_dma_internal_config_info[BWrite(sw_dma_buffer)].argument = info->argument;
        sw_dma_internal_config_info[BWrite(sw_dma_buffer)].dma_type = info->dma_type;
        sw_dma_internal_config_info[BWrite(sw_dma_buffer)].h_size = info->h_size;
        sw_dma_internal_config_info[BWrite(sw_dma_buffer)].transfer_type = info->transfer_type;
        Buf_Push(sw_dma_buffer);
    } else {
        hal_nvic_restore_interrupt_mask(irq_status);
        return HAL_SW_DMA_STATUS_BUFFER_FULL;
    }

    if (g_sw_dma_is_ongoing == false) {
        sw_dma_read_pointer = BRead(sw_dma_buffer);
        config_info.source_address = sw_dma_internal_config_info[sw_dma_read_pointer].source_address;
        config_info.destination_address = sw_dma_internal_config_info[sw_dma_read_pointer].destination_address;
        config_info.length = sw_dma_internal_config_info[sw_dma_read_pointer].length;
        config_info.dma_type = sw_dma_internal_config_info[sw_dma_read_pointer].dma_type;
        config_info.h_size = sw_dma_internal_config_info[sw_dma_read_pointer].h_size;
        config_info.transfer_type = sw_dma_internal_config_info[sw_dma_read_pointer].transfer_type;
        sw_dma_start_interrupt(&config_info);
        g_sw_dma_is_ongoing = true;
    }

    hal_nvic_restore_interrupt_mask(irq_status);

    return HAL_SW_DMA_STATUS_OK;
}

#endif /*HAL_SW_DMA_MODULE_ENABLED*/

