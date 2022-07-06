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
#include "syslog.h"

#if defined(MTK_CPU_NUMBER_0) && defined(MTK_RACE_CMD_ENABLE) && !defined(AG3335)
#include "race_cmd.h"
#include "race_xport.h"
#endif /* MTK_CPU_NUMBER_0 */


#ifdef MTK_SINGLE_CPU_ENV
static volatile virtual_read_write_point_t mux_airapp_r_w_point;
static volatile virtual_read_write_point_t *g_mux_airapp_r_w_point = &mux_airapp_r_w_point;
static volatile uint32_t g_online_log_start;
static volatile uint32_t *online_log_start = &g_online_log_start;
#else
static volatile virtual_read_write_point_t *g_mux_airapp_r_w_point = (volatile virtual_read_write_point_t *)(HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_ONLINE_VAR_START);
static volatile uint32_t *online_log_start = (volatile uint32_t *)(HW_SYSRAM_PRIVATE_MEMORY_SYSLOG_ONLINE_VAR_START + sizeof(virtual_read_write_point_t));
#endif /* MTK_SINGLE_CPU_ENV */


#ifdef MTK_CPU_NUMBER_0

#define MAX_TX_LEN 512

uint32_t airapp_get_race_port_handle(void)
{
    uint32_t channelNum = 0;
#if defined(MTK_RACE_CMD_ENABLE) && !defined(AG3335)
    channelNum = race_get_port_handle_by_channel_id(RACE_SERIAL_PORT_TYPE_SPP);
#endif
    return channelNum;
}

static void airapp_send_data(uint8_t port_index, mux_ctrl_para_t *para)
{
    uint32_t channelNum;
    uint32_t total_size, send_addr, send_len;
    mux_status_t status;
    mux_buffer_t mux_buffer[2];
    virtual_read_write_point_t *p = (virtual_read_write_point_t *)&g_mux_airapp_r_w_point[port_index];

    PORT_MUX_UNUSED(para);
    channelNum = airapp_get_race_port_handle();
    if (channelNum) {
        send_len  = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
        send_addr = p->tx_buff_read_point;
        if (send_len > MAX_TX_LEN) {
            send_len = MAX_TX_LEN;
        }
        mux_buffer[0].p_buf = (uint8_t *)send_addr;
        mux_buffer[0].buf_size = send_len;
        status = mux_tx(channelNum, mux_buffer, 1, &total_size);
        if (status != MUX_STATUS_OK) {
            //error handle
        }
        mux_common_device_set_tx_hw_rptr_internal_use(p, send_len);
    }
}

mux_status_t port_mux_airapp_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    PORT_MUX_UNUSED(port_index);
    PORT_MUX_UNUSED(irq_handler);

    virtual_read_write_point_t *p = (virtual_read_write_point_t *)&g_mux_airapp_r_w_point[port_index];

    mux_common_device_r_w_point_init(p, p_setting);
    *online_log_start = 0;
    return MUX_STATUS_OK;
}

mux_status_t port_mux_airapp_deinit(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return MUX_STATUS_OK;
}

void port_mux_airapp_exception_init(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
}

void port_mux_airapp_exception_send(uint8_t port_index, uint8_t *buffer, uint32_t size)
{
    PORT_MUX_UNUSED(port_index);
    PORT_MUX_UNUSED(buffer);
    PORT_MUX_UNUSED(size);
}

bool port_mux_airapp_get_buf_is_full(uint8_t port_index, bool is_rx)
{
    PORT_MUX_UNUSED(port_index);
    PORT_MUX_UNUSED(is_rx);

    return 0;
}

#endif /* MTK_CPU_NUMBER_0 */


ATTR_TEXT_IN_TCM uint32_t port_mux_airapp_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    PORT_MUX_UNUSED(port_index);
    PORT_MUX_UNUSED(is_rx);
    return mux_common_device_get_hw_rptr((virtual_read_write_point_t *)&g_mux_airapp_r_w_point[port_index], is_rx);
}

ATTR_TEXT_IN_TCM uint32_t port_mux_airapp_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    PORT_MUX_UNUSED(port_index);
    PORT_MUX_UNUSED(is_rx);
    return mux_common_device_get_hw_wptr((virtual_read_write_point_t *)&g_mux_airapp_r_w_point[port_index], is_rx);
}

ATTR_TEXT_IN_TCM void port_mux_airapp_set_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    PORT_MUX_UNUSED(port_index);
    PORT_MUX_UNUSED(move_bytes);
    mux_common_device_set_rx_hw_rptr((virtual_read_write_point_t *)&g_mux_airapp_r_w_point[port_index], move_bytes);
}

void port_mux_airapp_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void port_mux_airapp_phase2_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

ATTR_TEXT_IN_TCM void port_mux_airapp_set_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    virtual_read_write_point_t *p = (virtual_read_write_point_t *)&g_mux_airapp_r_w_point[port_index];
    PORT_MUX_UNUSED(port_index);

    mux_common_device_set_tx_hw_wptr(p, move_bytes);

    if (*online_log_start == 0) {
        mux_common_device_set_tx_hw_rptr_internal_use(p, move_bytes);
        return;
    }
}

mux_status_t port_mux_airapp_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    virtual_read_write_point_t *p = (virtual_read_write_point_t *)&g_mux_airapp_r_w_point[port_index];
    PORT_MUX_UNUSED(para);

    if (port_index > (MUX_AIRAPP_END - MUX_AIRAPP_BEGIN)) {
        return MUX_STATUS_ERROR;
    }

    switch (command) {
    case MUX_CMD_CONNECT: {
        *online_log_start = 0x01;
    } break;

    case MUX_CMD_DISCONNECT: {
        *online_log_start = 0;
    } break;

    case MUX_CMD_CLEAN: {
    } break;

    case MUX_CMD_GET_VIRTUAL_TX_AVAIL_LEN: {
        para->mux_get_tx_avail.ret_size = p->tx_buff_available_len;
    } break;

    case MUX_CMD_GET_VIRTUAL_RX_AVAIL_LEN: {
        para->mux_get_rx_avail.ret_size = p->rx_buff_available_len;
    } break;

    case MUX_TX_BUFFER_SEND: {
        airapp_send_data(port_index, para);
    } break;

    default:
        break;
    }

    return MUX_STATUS_OK;
}

port_mux_device_ops_t g_port_mux_airapp_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_airapp_normal_init,
    port_mux_airapp_deinit,
    NULL,
    NULL,
    port_mux_airapp_get_buf_is_full,
#endif
    port_mux_airapp_get_hw_rptr,
    port_mux_airapp_set_hw_rptr,
    port_mux_airapp_get_hw_wptr,
    port_mux_airapp_set_hw_wptr,
    port_mux_airapp_phase1_send,
    port_mux_airapp_phase2_send,
    port_mux_airapp_control,
    NULL,
    NULL,
};





