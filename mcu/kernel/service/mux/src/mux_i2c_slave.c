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
#include "hal_feature_config.h"
#ifdef HAL_I2C_SLAVE_MODULE_ENABLED

#include "mux.h"
#include "hal_gpt.h"

#include "hal_platform.h"
#include "hal_nvic.h"

#include "mux.h"
#include "mux_port_device.h"
#include "assert.h"
#include "mux_port.h"
#include "bsp_virtual_register.h"
#include "memory_attribute.h"

virtual_read_write_point_t g_mux_i2c_slave_r_w_point[1];
mux_irq_handler_t g_mux_i2c_slave_callback;

#define I2C_SLAVE_PORT_INDEX_TO_MUX_PORT(port_index) (port_index+MUX_I2C_SLAVE_BEGIN)


void i2c_slave_virtual_register_tx_buffer_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length);
void i2c_slave_virtual_register_rx_free_size_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length);
void i2c_slave_virtual_register_rx_buffer_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length);


#define VG_I2C_SLAVE_TX_LEN_REG_OFFSET 0x08
#define VG_I2C_SLAVE_TX_BUF_REG_OFFSET 0x2000
#define VG_I2C_SLAVE_RX_LEN_REG_OFFSET 0x04
#define VG_I2C_SLAVE_RX_BUF_REG_OFFSET 0x1000

typedef enum {
    VG_I2C_SLAVE_TX_LEN_REG_INDEX,
    VG_I2C_SLAVE_TX_ADDR_REG_INDEX,
    VG_I2C_SLAVE_RX_LEN_REG_INDEX,
    VG_I2C_SLAVE_RX_ADDR_REG_INDEX,
} virtual_register_list_t;

static ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN bsp_virtual_register_t i2c_slave_virtual_register_table[] = {
    // I2C slave Tx
    //offset                                            //addr          //len_limiter       //attri                                 //op
    {VG_I2C_SLAVE_TX_LEN_REG_OFFSET, 0xFFFFFFFF, 4,            BSP_VIRTUAL_REGISTER_RO, NULL},
    {VG_I2C_SLAVE_TX_BUF_REG_OFFSET, 0xFFFFFFFF, 0xFFFFFFFF,   BSP_VIRTUAL_REGISTER_RO, i2c_slave_virtual_register_tx_buffer_callback},
    // I2C slave Rx
    {VG_I2C_SLAVE_RX_LEN_REG_OFFSET, 0xFFFFFFFF, 4,            BSP_VIRTUAL_REGISTER_RO, i2c_slave_virtual_register_rx_free_size_callback},
    {VG_I2C_SLAVE_RX_BUF_REG_OFFSET, 0xFFFFFFFF, 0xFFFFFFFF,   BSP_VIRTUAL_REGISTER_WO, i2c_slave_virtual_register_rx_buffer_callback},
    {0xdeadbeef , 0xdeadbeef, 0xFFFFFFFF,   0xFFFFFFFF             , NULL},
};


void i2c_slave_virtual_register_rx_buffer_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length)
{
    virtual_read_write_point_t *p = &g_mux_i2c_slave_r_w_point[handle];
    uint32_t next_free_block_len;
    mux_driver_debug_for_check(p);

    log_hal_msgid_info("I2C slave receive %d data\r\n", 1, length);
#if 0
    log_hal_msgid_error("p->rx_buff_start:0x%x  \r\n", 1, p->rx_buff_start);
    log_hal_msgid_error("p->rx_buff_len:0x%x  \r\n", 1, p->rx_buff_len);
    log_hal_msgid_error("p->rx_buff_end:0x%x  \r\n", 1, p->rx_buff_end);
    log_hal_msgid_error("p->rx_buff_read_point:0x%x  \r\n", 1, p->rx_buff_read_point);
    log_hal_msgid_error("p->rx_buff_write_point:0x%x  \r\n", 1, p->rx_buff_write_point);
    log_hal_msgid_error("p->rx_buff_available_len:0x%x  \r\n", 1, p->rx_buff_available_len);
    log_hal_msgid_error("p->rx_receiving_write_point:0x%x  \r\n", 1, p->rx_receiving_write_point);
#endif
    if (direction == VIRTUAL_REGISTER_READ) {
        assert(0);
    } else { //Rx
        if (length > (p->rx_receiving_write_point - p->rx_buff_write_point))
            assert(0);
        //Step1:receive done and move rx w point
        p->rx_receiving_write_point =  p->rx_buff_write_point + length;
        mux_common_device_set_rx_hw_wptr_internal_use(p, length);
        //step2: call use ready to read callback
        g_mux_i2c_slave_callback(I2C_SLAVE_PORT_INDEX_TO_MUX_PORT(0), MUX_EVENT_READY_TO_READ, NULL);
        //Step3: restart to receive
        next_free_block_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point,
                              p->rx_buff_write_point, p->rx_buff_end, p->rx_buff_available_len);
        //update VG_I2C_SLAVE_RX_ADDR_REG_INDEX.len_limiter
        //i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_ADDR_REG_INDEX].len_limiter = next_free_block_len;
        vreg->real_addr = p->rx_buff_write_point;
        vreg->len_limiter = next_free_block_len;
        p->rx_receiving_write_point = p->rx_buff_write_point + next_free_block_len;

    }
}

void i2c_slave_virtual_register_tx_buffer_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length)
{
    virtual_read_write_point_t *p = &g_mux_i2c_slave_r_w_point[handle];
    uint32_t next_available_block_len;
    mux_driver_debug_for_check(p);

    if (direction == VIRTUAL_REGISTER_READ) {

        //Step1:send done and move tx r point
        mux_common_device_set_tx_hw_rptr_internal_use(p, length);

        //step2: call use ready to read callback
        g_mux_i2c_slave_callback(I2C_SLAVE_PORT_INDEX_TO_MUX_PORT(0), MUX_EVENT_READY_TO_WRITE, NULL);
        //Step3: restart to send
        next_available_block_len = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
        vreg->real_addr = p->tx_buff_read_point;
        vreg->len_limiter = next_available_block_len;
    } else {
        log_hal_msgid_error("i2c_slave_virtual_register_tx_buffer_callback Error!!! direction:%d \r\n", 1, direction);
        assert(0);
    }
}

void i2c_slave_virtual_register_rx_free_size_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length)
{
    if (direction == VIRTUAL_REGISTER_READ) {
        //do nothing
    } else {
        //do nothing
    }
}




mux_status_t port_mux_i2c_slave_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    virtual_read_write_point_t *p = &g_mux_i2c_slave_r_w_point[port_index];
    mux_common_device_r_w_point_init(p, p_setting);
    g_mux_i2c_slave_callback = irq_handler;
    mux_driver_debug_for_check(&g_mux_i2c_slave_r_w_point[port_index]);

    bsp_slave_chip_init(BSP_VIRTUAL_REG_I2C_SLAVE_BEGIN + port_index, i2c_slave_virtual_register_table,
                        &p_setting->p_user_setting->dev_setting.i2c_slave);
    i2c_slave_virtual_register_table[VG_I2C_SLAVE_TX_ADDR_REG_INDEX].real_addr = p->tx_buff_read_point;
    i2c_slave_virtual_register_table[VG_I2C_SLAVE_TX_ADDR_REG_INDEX].len_limiter = 0;

    i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_ADDR_REG_INDEX].real_addr = p->rx_buff_write_point;
    i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_ADDR_REG_INDEX].len_limiter = p->rx_buff_len;

    i2c_slave_virtual_register_table[VG_I2C_SLAVE_TX_LEN_REG_INDEX].real_addr = (uint32_t)&i2c_slave_virtual_register_table[VG_I2C_SLAVE_TX_ADDR_REG_INDEX].len_limiter;
    i2c_slave_virtual_register_table[VG_I2C_SLAVE_TX_LEN_REG_INDEX].len_limiter = 4;

    i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_LEN_REG_INDEX].real_addr = (uint32_t)&i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_ADDR_REG_INDEX].len_limiter;
    i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_LEN_REG_INDEX].len_limiter = 4;

    return MUX_STATUS_OK;
}

mux_status_t port_mux_i2c_slave_deinit(uint8_t port_index)
{
    mux_driver_debug_for_check(&g_mux_i2c_slave_r_w_point[port_index]);

    bsp_slave_chip_deinit(port_index);
    return MUX_STATUS_OK;
}



bool port_mux_i2c_slave_buf_is_full(uint8_t port_index, bool is_rx)
{
    return mux_common_device_buf_is_full(&g_mux_i2c_slave_r_w_point[port_index], is_rx);
}

uint32_t port_mux_i2c_slave_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_rptr(&g_mux_i2c_slave_r_w_point[port_index], is_rx);
}

uint32_t port_mux_i2c_slave_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_wptr(&g_mux_i2c_slave_r_w_point[port_index], is_rx);
}

/*
     port_mux_usb_set_tx_hw_wptr: for Rx- MUX driver read data.
 */

void port_mux_i2c_slave_set_rx_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    uint32_t free_len, per_cpu_irq_mask;
    virtual_read_write_point_t *p = &g_mux_i2c_slave_r_w_point[port_index];

    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);

    mux_common_device_set_rx_hw_rptr(&g_mux_i2c_slave_r_w_point[port_index], move_bytes);

    free_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point, p->rx_buff_write_point, p->rx_buff_end, p->tx_buff_available_len);
    p->rx_receiving_write_point = p->rx_buff_write_point + free_len;

    i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_ADDR_REG_INDEX].real_addr = p->rx_buff_write_point;
    i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_ADDR_REG_INDEX].len_limiter = free_len;
    //i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_ADDR_REG_INDEX].real_addr = &p->rx_receiving_write_point;
    //i2c_slave_virtual_register_table[VG_I2C_SLAVE_RX_ADDR_REG_INDEX].len_limiter = 4;

    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);
}


void port_mux_i2c_slave_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_wptr_internal_use(&g_mux_i2c_slave_r_w_point[port_index], move_bytes);
}

/*port_mux_i2c_slave_set_tx_hw_rptr_internal_use: for Tx-HW send data from Tx buff
     This function only need by USB / I2C slave / I2C slave.
     As UART with VFIFO, HW will move Rx write point.
     But for  USB / I2C slave / SPI slave, mux_xxx driver should to do this.
*/
void port_mux_i2c_slave_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_tx_hw_rptr_internal_use(&g_mux_i2c_slave_r_w_point[port_index], move_bytes);
}


/*
     port_mux_usb_set_tx_hw_wptr: for Tx- MUX driver send data.
 */
void port_mux_i2c_slave_set_tx_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    virtual_read_write_point_t *p = &g_mux_i2c_slave_r_w_point[port_index];
    uint32_t send_len, per_cpu_irq_mask;

    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
    mux_common_device_set_tx_hw_wptr(&g_mux_i2c_slave_r_w_point[port_index], move_bytes);

    send_len = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
    //log_hal_info("port_mux_i2c_slave_set_tx_hw_wptr p->tx_buff_write_point:0x%x\r\n",p->tx_buff_write_point);


    i2c_slave_virtual_register_table[VG_I2C_SLAVE_TX_ADDR_REG_INDEX].real_addr = p->tx_buff_read_point;
    i2c_slave_virtual_register_table[VG_I2C_SLAVE_TX_ADDR_REG_INDEX].len_limiter = send_len;

    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);

}

void port_mux_i2c_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void port_mux_i2c_phase2_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

mux_status_t port_mux_i2c_slave_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    log_hal_msgid_error("ERROR!!! MUX I2C Slave can not support port_mux_i2c_slave_control!!!\r\n", 0);
    return MUX_STATUS_ERROR;
}


port_mux_device_ops_t g_port_mux_i2c_slave_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_i2c_slave_normal_init,
    port_mux_i2c_slave_deinit,
    NULL,
    NULL,
    port_mux_i2c_slave_buf_is_full,
#endif
    port_mux_i2c_slave_get_hw_rptr,
    port_mux_i2c_slave_set_rx_hw_rptr,
    port_mux_i2c_slave_get_hw_wptr,
    port_mux_i2c_slave_set_tx_hw_wptr,
    port_mux_i2c_phase1_send,
    port_mux_i2c_phase2_send,
    port_mux_i2c_slave_control,
    NULL,
    NULL,
};

#endif
