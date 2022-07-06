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
#ifdef HAL_SPI_SLAVE_MODULE_ENABLED

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

virtual_read_write_point_t g_mux_spi_slave_r_w_point[1];
mux_irq_handler_t g_mux_spi_slave_callback;

#define SPI_SLAVE_PORT_INDEX_TO_MUX_PORT(port_index) (port_index+MUX_SPI_SLAVE_BEGIN)


void spi_slave_virtual_register_tx_buffer_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length);
void spi_slave_virtual_register_rx_free_size_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length);
void spi_slave_virtual_register_rx_buffer_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length);

typedef enum {
    VG_SPI_SLAVE_TX_LEN_REG_INDEX,
    VG_SPI_SLAVE_TX_ADDR_REG_INDEX,
    VG_SPI_SLAVE_RX_LEN_REG_INDEX,
    VG_SPI_SLAVE_RX_ADDR_REG_INDEX,
} virtual_register_list_t;

static ATTR_RWDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN bsp_virtual_register_t spi_slave_virtual_register_table[] = {
    // SPI slave Tx
    //offset            //addr          //len_limiter       //attri                                 //op
    {0x08       , 0xFFFFFFFF, 4,            BSP_VIRTUAL_REGISTER_RO, NULL},
    {0x2000     , 0xFFFFFFFF, 0xFFFFFFFF,   BSP_VIRTUAL_REGISTER_RO, spi_slave_virtual_register_tx_buffer_callback},
    // SPI slave Rx
    {0x04       , 0xFFFFFFFF, 4,            BSP_VIRTUAL_REGISTER_RO, spi_slave_virtual_register_rx_free_size_callback},
    {0x1000     , 0xFFFFFFFF, 0xFFFFFFFF,   BSP_VIRTUAL_REGISTER_WO, spi_slave_virtual_register_rx_buffer_callback},
    {0xdeadbeef , 0xdeadbeef, 0xFFFFFFFF,   0xFFFFFFFF             , NULL},
};


void spi_slave_virtual_register_rx_buffer_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length)
{
    virtual_read_write_point_t *p = &g_mux_spi_slave_r_w_point[handle];
    uint32_t next_free_block_len;
    mux_driver_debug_for_check(p);

    LOG_MSGID_I(common, "SPI slave receive %d data\r\n", 1, (int)length);
    LOG_MSGID_I(common, "p->rx_buff_start:0x%x  \r\n", 1, (unsigned int)p->rx_buff_start);
    LOG_MSGID_I(common, "p->rx_buff_len:0x%x  \r\n", 1, (unsigned int)p->rx_buff_len);
    LOG_MSGID_I(common, "p->rx_buff_end:0x%x  \r\n", 1, (unsigned int)p->rx_buff_end);
    LOG_MSGID_I(common, "p->rx_buff_read_point:0x%x  \r\n", 1, (unsigned int)p->rx_buff_read_point);
    LOG_MSGID_I(common, "p->rx_buff_write_point:0x%x  \r\n", 1, (unsigned int)p->rx_buff_write_point);
    LOG_MSGID_I(common, "p->rx_buff_available_len:0x%x  \r\n", 1, (unsigned int)p->rx_buff_available_len);
    LOG_MSGID_I(common, "p->rx_receiving_write_point:0x%x  \r\n", 1, (unsigned int)p->rx_receiving_write_point);
    if (direction == VIRTUAL_REGISTER_READ) {
        assert(0);
    } else { //Rx
        if (length > (p->rx_receiving_write_point - p->rx_buff_write_point))
            assert(0);
        //Step1:receive done and move rx w point
        p->rx_receiving_write_point =  p->rx_buff_write_point + length;
        mux_common_device_set_rx_hw_wptr_internal_use(p, length);
        //step2: call use ready to read callback
        g_mux_spi_slave_callback(SPI_SLAVE_PORT_INDEX_TO_MUX_PORT(0), MUX_EVENT_READY_TO_READ, NULL);
        //Step3: restart to receive
        next_free_block_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point,
                              p->rx_buff_write_point, p->rx_buff_end, p->rx_buff_available_len);
        //update VG_SPI_SLAVE_RX_ADDR_REG_INDEX.len_limiter
        //spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_ADDR_REG_INDEX].len_limiter = next_free_block_len;
        vreg->real_addr = p->rx_buff_write_point;
        vreg->len_limiter = next_free_block_len;
        p->rx_receiving_write_point = p->rx_buff_write_point + next_free_block_len;

    }
}

void spi_slave_virtual_register_tx_buffer_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length)
{
    virtual_read_write_point_t *p = &g_mux_spi_slave_r_w_point[handle];
    uint32_t next_available_block_len;
    LOG_MSGID_I(common, "this is function of spi_slave_virtual_register_tx_buffer_callback()\r\n", 0);
    mux_driver_debug_for_check(p);

    if (direction == VIRTUAL_REGISTER_READ) {
#if 0
        if ( p->tx_sending_read_point - p->tx_buff_read_point < length) {
            LOG_MSGID_I(common, "spi_slave_virtual_register_tx_buffer_callback: tx_sending_read_point:0x%x,tx_buff_read_point:0x%x,length:%d\r\n", 3,
                        p->tx_sending_read_point, p->tx_buff_read_point, length);
            while (1);
            assert(0);
        }
#endif
        //if(p->tx_send_is_running != MUX_DEVICE_HW_RUNNING){
        //        assert(0);
        // }

        //Step1:send done and move tx r point
        //p->tx_sending_read_point =  p->tx_buff_read_point +length;
        mux_common_device_set_tx_hw_rptr_internal_use(p, length);

        //step2: call use ready to read callback
        g_mux_spi_slave_callback(SPI_SLAVE_PORT_INDEX_TO_MUX_PORT(0), MUX_EVENT_READY_TO_WRITE, NULL);
        //Step3: restart to send
        next_available_block_len = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
        vreg->real_addr = p->tx_buff_read_point;
        vreg->len_limiter = next_available_block_len;
#if 0
        if (next_available_block_len == 0) {
            //p->tx_send_is_running = MUX_DEVICE_HW_IDLE;
        } else {
            //update VG_SPI_SLAVE_TX_ADDR_REG_INDEX.len_limiter
            //spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_ADDR_REG_INDEX].len_limiter = next_available_block_len;
            vreg->len_limiter = next_available_block_len;
            p->tx_sending_read_point = p->tx_buff_read_point + next_available_block_len;
            LOG_MSGID_I(common, "spi_slave_virtual_register_tx_buffer_callback p->tx_sending_read_point:0x%x\r\n", 1, p->tx_sending_read_point);
        }
#endif
    } else {
        assert(0);
    }
}

void spi_slave_virtual_register_rx_free_size_callback(uint32_t handle, virtual_register_transfer_direction_t direction, bsp_virtual_register_t *vreg, uint32_t length)
{
    LOG_MSGID_I(common, "this is function of spi_slave_virtual_register_rx_free_size_callback()\r\n", 0);

    if (direction == VIRTUAL_REGISTER_READ) {
        //do nothing
        LOG_MSGID_I(common, "Master read Slave Virtual REG of Rx_Free_Len reg, value is :%d\r\n", 1, (int) * (volatile uint32_t *)vreg->real_addr);
    } else {
        //do nothing
    }
}




mux_status_t port_mux_spi_slave_normal_init(uint8_t port_index, mux_port_config_t *p_setting, mux_irq_handler_t irq_handler)
{
    virtual_read_write_point_t *p = &g_mux_spi_slave_r_w_point[port_index];
    mux_common_device_r_w_point_init(p, p_setting);
    g_mux_spi_slave_callback = irq_handler;
    mux_driver_debug_for_check(&g_mux_spi_slave_r_w_point[port_index]);

    bsp_slave_chip_init(BSP_VIRTUAL_REG_SPI_SLAVE_BEGIN + port_index, spi_slave_virtual_register_table,
                        &p_setting->p_user_setting->dev_setting.spi_slave.slave_config);
    spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_ADDR_REG_INDEX].real_addr = p->tx_buff_read_point;
    spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_ADDR_REG_INDEX].len_limiter = 0;

    spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_ADDR_REG_INDEX].real_addr = p->rx_buff_write_point;
    spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_ADDR_REG_INDEX].len_limiter = p->rx_buff_len;

    spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_LEN_REG_INDEX].real_addr = (uint32_t)&spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_ADDR_REG_INDEX].len_limiter;
    spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_LEN_REG_INDEX].len_limiter = 4;

    spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_LEN_REG_INDEX].real_addr = (uint32_t)&spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_ADDR_REG_INDEX].len_limiter;
    spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_LEN_REG_INDEX].len_limiter = 4;

    return MUX_STATUS_OK;
}

mux_status_t port_mux_spi_slave_deinit(uint8_t port_index)
{
    virtual_register_status_t status;

    mux_driver_debug_for_check(&g_mux_spi_slave_r_w_point[port_index]);

    status = bsp_slave_chip_deinit(port_index + BSP_VIRTUAL_REG_SPI_SLAVE_BEGIN);
    if (VIRTUAL_REGISTER_STATUS_OK != status) {
        LOG_MSGID_I(common, "bsp_slave_chip_deinit fail!! error_code=%d\r\n", 1, status);
        return MUX_STATUS_ERROR_DEINIT_FAIL;
    }
    return MUX_STATUS_OK;
}



bool port_mux_spi_slave_buf_is_full(uint8_t port_index, bool is_rx)
{
    return mux_common_device_buf_is_full(&g_mux_spi_slave_r_w_point[port_index], is_rx);
}

uint32_t port_mux_spi_slave_get_hw_rptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_rptr(&g_mux_spi_slave_r_w_point[port_index], is_rx);
}

uint32_t port_mux_spi_slave_get_hw_wptr(uint8_t port_index, bool is_rx)
{
    return mux_common_device_get_hw_wptr(&g_mux_spi_slave_r_w_point[port_index], is_rx);
}

/*
    port_mux_usb_set_tx_hw_wptr: for Rx- MUX driver read data.
*/

void port_mux_spi_slave_set_rx_hw_rptr(uint8_t port_index, uint32_t move_bytes)
{
    uint32_t free_len, per_cpu_irq_mask;
    virtual_read_write_point_t *p = &g_mux_spi_slave_r_w_point[port_index];

    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);

    mux_common_device_set_rx_hw_rptr(&g_mux_spi_slave_r_w_point[port_index], move_bytes);

    free_len = mux_common_device_get_buf_next_free_block_len(p->rx_buff_start, p->rx_buff_read_point, p->rx_buff_write_point, p->rx_buff_end, p->tx_buff_available_len);
    p->rx_receiving_write_point = p->rx_buff_write_point + free_len;

    spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_ADDR_REG_INDEX].real_addr = p->rx_buff_write_point;
    spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_ADDR_REG_INDEX].len_limiter = free_len;
    //spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_ADDR_REG_INDEX].real_addr = &p->rx_receiving_write_point;
    //spi_slave_virtual_register_table[VG_SPI_SLAVE_RX_ADDR_REG_INDEX].len_limiter = 4;

    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);
}


void port_mux_spi_slave_set_rx_hw_wptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_rx_hw_wptr_internal_use(&g_mux_spi_slave_r_w_point[port_index], move_bytes);
}

/*port_mux_spi_slave_set_tx_hw_rptr_internal_use: for Tx-HW send data from Tx buff
    This function only need by USB / I2C slave / SPI slave.
    As UART with VFIFO, HW will move Rx write point.
    But for  USB / I2C slave / SPI slave, mux_xxx driver should to do this.
*/
void port_mux_spi_slave_set_tx_hw_rptr_internal_use(uint8_t port_index, uint32_t move_bytes)
{
    mux_common_device_set_tx_hw_rptr_internal_use(&g_mux_spi_slave_r_w_point[port_index], move_bytes);
}


/*
    port_mux_usb_set_tx_hw_wptr: for Tx- MUX driver send data.
*/
void port_mux_spi_slave_set_tx_hw_wptr(uint8_t port_index, uint32_t move_bytes)
{
    virtual_read_write_point_t *p = &g_mux_spi_slave_r_w_point[port_index];
    uint32_t send_len, per_cpu_irq_mask;

    port_mux_local_cpu_enter_critical(&per_cpu_irq_mask);
    mux_common_device_set_tx_hw_wptr(&g_mux_spi_slave_r_w_point[port_index], move_bytes);

    send_len = mux_common_device_get_buf_next_available_block_len(p->tx_buff_start, p->tx_buff_read_point, p->tx_buff_write_point, p->tx_buff_end, p->tx_buff_available_len);
    //p->tx_sending_read_point = p->tx_buff_read_point+send_len;
    //p->tx_send_is_running = MUX_DEVICE_HW_RUNNING;

    //spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_ADDR_REG_INDEX].real_addr = send_addr_from_tx_buff;
    spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_ADDR_REG_INDEX].real_addr = p->tx_buff_read_point;
    spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_ADDR_REG_INDEX].len_limiter = send_len;
    //spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_LEN_REG_INDEX].real_addr = &p->tx_sending_read_point;
    //spi_slave_virtual_register_table[VG_SPI_SLAVE_TX_LEN_REG_INDEX].len_limiter = 4;

    port_mux_local_cpu_exit_critical(per_cpu_irq_mask);

}

void port_mux_spi_phase1_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

void port_mux_spi_phase2_send(uint8_t port_index)
{
    PORT_MUX_UNUSED(port_index);
    return;
}

mux_status_t port_mux_spi_slave_control(uint8_t port_index, mux_ctrl_cmd_t command, mux_ctrl_para_t *para)
{
    LOG_MSGID_I(common, "ERROR!!! MUX SPI Slave can not support port_mux_spi_slave_control!!!\r\n", 0);
    return MUX_STATUS_ERROR;
}


port_mux_device_ops_t g_port_mux_spi_slave_ops = {
#ifdef MTK_CPU_NUMBER_0
    port_mux_spi_slave_normal_init,
    port_mux_spi_slave_deinit,
    NULL,
    NULL,
    port_mux_spi_slave_buf_is_full,
#endif
    port_mux_spi_slave_get_hw_rptr,
    port_mux_spi_slave_set_rx_hw_rptr,
    port_mux_spi_slave_get_hw_wptr,
    port_mux_spi_slave_set_tx_hw_wptr,
    port_mux_spi_phase1_send,
    port_mux_spi_phase2_send,
    port_mux_spi_slave_control,
    NULL,
    NULL,
};
#endif
