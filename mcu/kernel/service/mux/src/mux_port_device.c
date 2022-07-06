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
#include <string.h>
#include <stdbool.h>

#include "hal.h"
#include "mux.h"
#include "mux_port.h"

#include "syslog.h"
#include "mux_port_device.h"
#include "assert.h"


void mux_common_device_r_w_point_init(virtual_read_write_point_t *p, mux_port_config_t *p_setting)
{
    p->rx_buff_start = p_setting->rx_buf_addr;
    p->rx_buff_end = p_setting->rx_buf_addr + p_setting->rx_buf_size;
    p->rx_buff_len = p_setting->rx_buf_size;
    p->rx_buff_read_point = p_setting->rx_buf_addr;
    p->rx_buff_write_point = p_setting->rx_buf_addr;
    p->rx_buff_available_len = 0;
    p->rx_receiving_write_point = 0xFFFFFFFF;

    p->tx_buff_start = p_setting->tx_buf_addr;
    p->tx_buff_end = p_setting->tx_buf_addr + p_setting->tx_buf_size;
    p->tx_buff_len = p_setting->tx_buf_size;
    p->tx_buff_read_point = p_setting->tx_buf_addr;
    p->tx_buff_write_point = p_setting->tx_buf_addr;
    p->tx_buff_available_len = 0;
    p->tx_sending_read_point = 0xFFFFFFFF;
    p->tx_send_is_running = MUX_DEVICE_HW_IDLE;

    p->tx_send_src_address    = 0;
    p->tx_send_src_length     = 0;
    p->rx_receive_src_address = 0;
    p->rx_receive_src_length  = 0;

}

bool mux_common_device_buf_is_full(virtual_read_write_point_t *p, bool is_rx)
{
    if (is_rx) {
        return p->rx_buff_available_len == p->rx_buff_len ? TRUE : FALSE;
    } else {
        return p->tx_buff_available_len == p->tx_buff_len ? TRUE : FALSE;
    }
}

uint32_t mux_common_device_get_hw_rptr(virtual_read_write_point_t *p, bool is_rx)
{
    if (is_rx) {
        return p->rx_buff_read_point - p->rx_buff_start;
    } else {
        return p->tx_buff_read_point - p->tx_buff_start;
    }
}

uint32_t mux_common_device_get_hw_wptr(virtual_read_write_point_t *p, bool is_rx)
{
    if (is_rx) {
        return p->rx_buff_write_point - p->rx_buff_start;
    } else {
        return p->tx_buff_write_point - p->tx_buff_start;
    }
}

void mux_common_device_set_rx_hw_rptr(virtual_read_write_point_t *p, uint32_t move_bytes)
{
    mux_driver_debug_for_check(p);
    if (move_bytes == 0) {
        return;
    }
    MOVE_RING_BUFF(p->rx_buff_start, p->rx_buff_read_point, move_bytes, p->rx_buff_end, p->rx_buff_read_point);
    p->rx_buff_available_len -= move_bytes;
}

/*port_mux_usb_set_rx_hw_wptr_internal_use: for Rx-HW receive data to Rx buff
    This function only need by USB / I2C slave / SPI slave.
    As UART with VFIFO, HW will move Rx write point.
    But for  USB / I2C slave / SPI slave, mux_xxx driver should to do this.
*/
void mux_common_device_set_rx_hw_wptr_internal_use(virtual_read_write_point_t *p, uint32_t move_bytes)
{
    mux_driver_debug_for_check(p);
    if (move_bytes == 0) {
        return;
    }
    MOVE_RING_BUFF(p->rx_buff_start, p->rx_buff_write_point, move_bytes, p->rx_buff_end, p->rx_buff_write_point);
    p->rx_buff_available_len += move_bytes;
    if (p->rx_buff_write_point == p->rx_buff_end) {
        assert(0);
    }
}

/*port_mux_usb_set_tx_hw_rptr_internal_use: for Tx-HW send data from Tx buff
    This function only need by USB / I2C slave / SPI slave.
    As UART with VFIFO, HW will move Rx write point.
    But for  USB / I2C slave / SPI slave, mux_xxx driver should to do this.
*/
void mux_common_device_set_tx_hw_rptr_internal_use(virtual_read_write_point_t *p, uint32_t move_bytes)
{
    mux_driver_debug_for_check(p);

    if (move_bytes == 0) {
        return;
    }
    MOVE_RING_BUFF(p->tx_buff_start, p->tx_buff_read_point, move_bytes, p->tx_buff_end, p->tx_buff_read_point);
    p->tx_buff_available_len -= move_bytes;
    if (p->tx_buff_read_point >= p->tx_buff_end) {
        assert(0);
    }
}

/*
    port_mux_usb_set_tx_hw_wptr: for Tx- MUX driver send data.
*/
void mux_common_device_set_tx_hw_wptr(virtual_read_write_point_t *p, uint32_t move_bytes)
{
    //uint32_t per_cpu_irq_mask;
    mux_driver_debug_for_check(p);

    /************************
        Step1:Move Tx write point.
            Note1:Common code for USB / I2C slave / SPI slave.
            Note2: Mux driver should query Tx buffer free size, then copy data to Tx buffer, then call this function to move Write point.
            Note3: No need to check whether overflow here, because Mux driver should do it when copy data to Tx buffer will do it firstly!!!
    ************************/
    MOVE_RING_BUFF(p->tx_buff_start, p->tx_buff_write_point, move_bytes, p->tx_buff_end, p->tx_buff_write_point);

    /************************
        Step2: update tx_buff_available_len.
            Note1:Common code for USB / I2C slave / SPI slave.
    ************************/
    p->tx_buff_available_len += move_bytes;

    return;
}

uint32_t mux_common_device_get_buf_next_free_block_len(uint32_t addr, uint32_t r_addr, uint32_t w_addr, uint32_t end, uint32_t available_len)
{
    uint32_t total_len = end - addr;

    if (total_len - available_len != 0) { // Rx still have free size
        // restart
        //step1:update to VG
        if (r_addr <= w_addr) {
            return end - w_addr;
        } else { // (r_addr > w_addr)
            return r_addr - w_addr;
        }
    } else { //Rx buff full
        return 0;
    }
}

uint32_t mux_common_device_get_buf_next_available_block_len(uint32_t addr, uint32_t r_addr, uint32_t w_addr, uint32_t end, uint32_t available_len)
{
    PORT_MUX_UNUSED(addr);
    if (available_len != 0) { // have available data
        //step1:update to VG
        if (r_addr < w_addr) {
            return w_addr - r_addr;
        } else { // (r_addr >= w_addr)
            return end - r_addr;
        }
    } else { //no available
        return 0;
    }
}

void mux_driver_debug_printf(uint32_t flag, virtual_read_write_point_t *p)
{
    //MUX_PORT_MSGID_E("mux sw r w point check error!!! flag:0x%x", 1, (int)flag);
    if ((flag & 0x7F) != 0) {
        assert(0);
        // MUX_PORT_MSGID_E("p->rx_buff_start:0x%x ", 1, (unsigned int)p->rx_buff_start);
        // MUX_PORT_MSGID_E("p->rx_buff_len:0x%x ", 1, (unsigned int)p->rx_buff_len);
        // MUX_PORT_MSGID_E("p->rx_buff_end:0x%x ", 1, (unsigned int)p->rx_buff_end);
        // MUX_PORT_MSGID_E("p->rx_buff_read_point:0x%x ", 1, (unsigned int)p->rx_buff_read_point);
        // MUX_PORT_MSGID_E("p->rx_buff_write_point:0x%x ", 1, (unsigned int)p->rx_buff_write_point);
        // MUX_PORT_MSGID_E("p->rx_buff_available_len:0x%x ", 1, (unsigned int)p->rx_buff_available_len);
        // MUX_PORT_MSGID_E("p->rx_receiving_write_point:0x%x ", 1, (unsigned int)p->rx_receiving_write_point);
    }
    if ((flag & 0x3F80) != 0) {
        assert(0);
        // MUX_PORT_MSGID_E("p->tx_buff_start:0x%x ", 1, (unsigned int)p->tx_buff_start);
        // MUX_PORT_MSGID_E("p->tx_buff_len:0x%x ", 1, (unsigned int)p->tx_buff_len);
        // MUX_PORT_MSGID_E("p->tx_buff_end:0x%x ", 1, (unsigned int)p->tx_buff_end);
        // MUX_PORT_MSGID_E("p->tx_buff_read_point:0x%x ", 1, (unsigned int)p->tx_buff_read_point);
        // MUX_PORT_MSGID_E("p->tx_buff_write_point:0x%x ", 1, (unsigned int)p->tx_buff_write_point);
        // MUX_PORT_MSGID_E("p->tx_buff_available_len:0x%x ", 1, (unsigned int)p->tx_buff_available_len);
        // MUX_PORT_MSGID_E("p->tx_sending_read_point:0x%x ", 1, (unsigned int)p->tx_sending_read_point);
    }
}

ATTR_TEXT_IN_TCM void mux_driver_debug_for_check(virtual_read_write_point_t *p)
{
    uint32_t flag = 0;
    uint32_t avaliable_len = 0;
    /*Step1: check Rx buffer*/
    if (p->rx_buff_read_point >= p->rx_buff_end) {
        flag |= (0x1 << 0) ;
    }
    if (p->rx_buff_read_point < p->rx_buff_start) {
        flag |= (0x1 << 1) ;
    }
    if (p->rx_buff_write_point >= p->rx_buff_end) {
        flag |= (0x1 << 2) ;
    }
    if (p->rx_buff_write_point < p->rx_buff_start) {
        flag |= (0x1 << 3) ;
    }

    if (p->rx_buff_read_point == p->rx_buff_write_point) {
        if ((p->rx_buff_available_len != 0) && (p->rx_buff_available_len != p->rx_buff_len)) {
            flag |= (0x1 << 4) ;
        }
    } else if (p->rx_buff_read_point < p->rx_buff_write_point) {
        if (p->rx_buff_available_len != (p->rx_buff_write_point - p->rx_buff_read_point)) {
            flag |= (0x1 << 5) ;
        }
    } else {
        avaliable_len = (p->rx_buff_end - p->rx_buff_read_point) + (p->rx_buff_write_point - p->rx_buff_start);
        if (avaliable_len != p->rx_buff_available_len) {
            flag |= (0x1 << 6) ;
        }
    }
    /*Step2: check Tx buffer*/
    if (p->tx_buff_read_point >= p->tx_buff_end) {
        flag |= (0x1 << 7) ;
    }
    if (p->tx_buff_read_point < p->tx_buff_start) {
        flag |= (0x1 << 8) ;
    }
    if (p->tx_buff_write_point >= p->tx_buff_end) {
        flag |= (0x1 << 9) ;
    }
    if (p->tx_buff_write_point < p->tx_buff_start) {
        flag |= (0x1 << 10) ;
    }

    if (p->tx_buff_read_point == p->tx_buff_write_point) {
        if ((p->tx_buff_available_len != 0) && (p->tx_buff_available_len != p->tx_buff_len)) {
            flag |= (0x1 << 11) ;
        }
    } else if (p->tx_buff_read_point < p->tx_buff_write_point) {
        if (p->tx_buff_available_len != (p->tx_buff_write_point - p->tx_buff_read_point))
            flag |= (0x1 << 12) ;
    } else {
        avaliable_len = (p->tx_buff_end - p->tx_buff_read_point) + (p->tx_buff_write_point - p->tx_buff_start);
        if (avaliable_len != p->tx_buff_available_len) {
            flag |= (0x1 << 13) ;
        }
    }

    if (flag != 0) {
        mux_driver_debug_printf(flag, p);
    }
}



