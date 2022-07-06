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

#include "mux.h"
#include "mux_port_device.h"

#ifdef MTK_CPU_NUMBER_0

void port_mux_flash_exception_init(uint8_t port_index)
{
    ///TODO:
}

void port_mux_flash_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    ///TODO:
}

void port_mux_flash_normal_init(uint8_t port_index, uint32_t share_buffer_addr, uint32_t share_buffer_size, mux_irq_handler_t irq_handler)
{
    ///TODO:
}

void port_mux_flash_deinit(uint8_t port_index)
{
    ///TODO:
}

void port_mux_flash_device_get_buf_info(uint8_t port_index, uint32_t *tx_buf_addr, uint32_t *tx_buf_size,
                                        uint32_t *rx_buf_addr, uint32_t *rx_buf_size)
{
    ///TODO:
}

bool port_mux_flash_get_buf_full_status(uint8_t port_index, bool is_rx)
{
    ///TODO:
}

#endif

void port_mux_flash_hook(uint8_t port_index)
{
    ///TODO:
}

uint32_t port_mux_flash_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    ///TODO:
}

uint32_t port_mux_flash_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    ///TODO:
}

void port_mux_flash_set_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    ///TODO:
}

void port_mux_flash_set_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    ///TODO:
}

port_mux_device_ops_t g_port_mux_flash_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_flash_normal_init,
    port_mux_flash_deinit,
    port_mux_flash_exception_init,
    port_mux_flash_exception_send,
    port_mux_flash_device_get_buf_info,
    port_mux_flash_get_buf_full_status,
#endif
    port_mux_flash_hook,
    port_mux_flash_get_hw_rptr,
    port_mux_flash_set_hw_rptr,
    port_mux_flash_get_hw_wptr,
    port_mux_flash_set_hw_wptr,
    NULL,
    NULL,
};

