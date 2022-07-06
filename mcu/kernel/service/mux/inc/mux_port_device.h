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

#ifndef __MUX_PORT_DEVICE_H__
#define __MUX_PORT_DEVICE_H__

#ifdef __cplusplus
extern "C" {
#endif



#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "mux_port_common.h"

/* Debug for mux port not include whitch port used syslog */
#define MUX_PORT_LOG_I(fmt,arg...)              LOG_I(MUX_PORT,fmt,##arg)
#define MUX_PORT_LOG_W(fmt,arg...)              LOG_W(MUX_PORT,fmt,##arg)
#define MUX_PORT_LOG_E(fmt,arg...)              LOG_E(MUX_PORT,fmt,##arg)
#define MUX_PORT_MSGID_I(fmt,cnt,arg...)        LOG_MSGID_I(MUX_PORT,fmt,cnt,##arg)
#define MUX_PORT_MSGID_W(fmt,cnt,arg...)        LOG_MSGID_W(MUX_PORT,fmt,cnt,##arg)
#define MUX_PORT_MSGID_E(fmt,cnt,arg...)        LOG_MSGID_E(MUX_PORT,fmt,cnt,##arg)

typedef struct {
    mux_port_setting_t *p_user_setting;             //  user hardware port assignment and port setting.
    uint32_t tx_buf_addr;                           //  user tx hardware buffer base address.
    uint32_t tx_buf_size;                           //  user tx hardware buffer total size.
    uint32_t rx_buf_addr;                           //  user rx hardware buffer base address.
    uint32_t rx_buf_size;                           //  user rx hardware buffer total size.
    uint32_t sw_wptr;                               //  mux ring buffer software pointer.
    uint32_t nest_count;                            //  multi-task nest counter to avoid interrupting during update write_point.
    mux_protocol_t protocol_cpu[2];                 //  user tx/rx protocol.
    uint8_t  init_status;                           //  global variable indicate mux initialized status.
    uint16_t port_is_connect;                       //  indicate the port connection and disconnection status.Mainly for the cross core scenario, cm4 can notify dsp to turn off the log.
} mux_port_config_t;


typedef enum {
    MUX_PORT_TYPE_UART = 0,
    MUX_PORT_TYPE_USB,
    MUX_PORT_TYPE_I2C_SLAVE,
    MUX_PORT_TYPE_SPI_SLAVE,
    MUX_PORT_TYPE_AIRAPP,
    MUX_PORT_TYPE_BT,
    MUX_PORT_TYPE_AWS_MCE,
    MUX_PORT_TYPE_IAP2,
    MUX_PORT_TYPE_PSEUDO,
    MUX_PORT_TYPE_MAX,
} mux_port_type_t;

#ifdef MTK_CPU_NUMBER_0

#include "serial_port_assignment.h"

#ifdef MTK_SINGLE_CPU_ENV
#ifndef CONFIG_SYSLOG_SHARE_TX_BUFFER_SIZE
#define CONFIG_SYSLOG_SHARE_TX_BUFFER_SIZE 10240
#endif
#define PORT_MUX_TX_SHARE_BUFFER_SIZE CONFIG_SYSLOG_SHARE_TX_BUFFER_SIZE
#else
#include "hal_resource_assignment.h"
#define PORT_MUX_TX_SHARE_BUFFER_SIZE  HW_SYSRAM_PRIVATE_MEMORY_MUX_TX_BUFFER_LEN
#endif

typedef void (*mux_irq_handler_t)(mux_port_t port, mux_event_t type, void *user_data);
#endif

typedef struct {
#ifdef MTK_CPU_NUMBER_0
    mux_status_t (*normal_init)(uint8_t port_index, mux_port_config_t *p_setting,mux_irq_handler_t irq_handler);
    mux_status_t (*deinit)(uint8_t port_index);
    void (*exception_init)(uint8_t port_index);
    void (*exception_send)(uint8_t port_index, uint8_t *buffer, uint32_t size);
    //void (*get_buf_size_status)(uint8_t port_index, uint32_t *tx_buf_addr, uint32_t *tx_buf_size, uint32_t *rx_buf_addr, uint32_t *rx_buf_size);
    bool (*get_buf_is_full)(uint8_t port_index, bool is_rx);
#endif
    //void (*tx_hook)(uint8_t port_index);
    uint32_t (*get_hw_rptr)(uint8_t port_index, bool is_rx);
    void (*set_hw_rptr)(uint8_t port_index, uint32_t move_bytes);
    uint32_t (*get_hw_wptr)(uint8_t port_index, bool is_rx);
    void (*set_hw_wptr)(uint8_t port_index, uint32_t move_bytes);
    void (*phase1_send)(uint8_t port_index);
    void (*phase2_send)(uint8_t port_index);
    mux_status_t (*control)(uint8_t port_index,mux_ctrl_cmd_t command, mux_ctrl_para_t *para);
    void (*set_tx_src_address)(uint8_t port_index, uint32_t src_address,uint32_t size);
    void (*get_rx_src_address)(uint8_t port_index, uint32_t src_address,uint32_t size);
} port_mux_device_ops_t;

#ifdef MTK_CPU_NUMBER_0
#define port_mux_device_normal_init(port_type, port_index,p_setting,irq_handler) \
    g_mux_device_ops_array[port_type]->normal_init(port_index, p_setting,irq_handler)
#define port_mux_device_deinit(port_type, port_index) \
    g_mux_device_ops_array[port_type]->deinit(port_index)
#define port_mux_device_exception_init(port_type, port_index) \
    g_mux_device_ops_array[port_type]->exception_init(port_index)
#define port_mux_device_exception_send(port_type, port_index, buffer, size) \
    g_mux_device_ops_array[port_type]->exception_send(port_index, buffer, size)
#define port_mux_device_buf_is_full(port_type, port_index, is_rx) \
    g_mux_device_ops_array[port_type]->get_buf_is_full(port_index, is_rx)
#endif
#define port_mux_device_get_hw_rptr(port_type, port_index, is_rx) \
    g_mux_device_ops_array[port_type]->get_hw_rptr(port_index, is_rx)
#define port_mux_device_get_hw_wptr(port_type, port_index, is_rx) \
    g_mux_device_ops_array[port_type]->get_hw_wptr(port_index, is_rx)
#define port_mux_device_set_rx_hw_rptr(port_type, port_index, move_bytes) \
    g_mux_device_ops_array[port_type]->set_hw_rptr(port_index, move_bytes)
#define port_mux_device_set_tx_hw_wptr(port_type, port_index, move_bytes) \
    g_mux_device_ops_array[port_type]->set_hw_wptr(port_index, move_bytes)
#define port_mux_device_phase1_send(port_type, port_index) \
    g_mux_device_ops_array[port_type]->phase1_send(port_index)
#define port_mux_device_phase2_send(port_type, port_index) \
    g_mux_device_ops_array[port_type]->phase2_send(port_index)
#define port_mux_device_normal_control(port_type, port_index,p_cmd,p_para) \
    g_mux_device_ops_array[port_type]->control(port_index, p_cmd,p_para)
#define port_mux_device_set_tx_src_address(port_type, port_index,src_address,size) \
    g_mux_device_ops_array[port_type]->set_tx_src_address(port_index, src_address,size)
#define port_mux_device_get_rx_src_address(port_type, port_index,src_address,size) \
    g_mux_device_ops_array[port_type]->get_rx_src_address(port_index, src_address,size)


#define MOVE_RING_BUFF(start,curren,move,end,new_point) \
    if((curren+move)>=end)\
    {\
        new_point = start+(curren+move-end);\
    }\
    else\
    {\
        new_point = curren+move;\
    }


/* for USB/SPI slave/I2C slave only, not for UART  Start****************************************************************************/

typedef struct
{
    /*use ring buffer mechanism*/
    volatile uint32_t rx_buff_read_point;
    volatile uint32_t rx_buff_write_point;
    volatile uint32_t rx_buff_len;
    volatile uint32_t rx_buff_start;
    volatile uint32_t rx_buff_end;
    volatile uint32_t rx_buff_available_len;
    volatile uint32_t rx_receiving_write_point;
    volatile uint32_t tx_buff_read_point;
    volatile uint32_t tx_buff_write_point;
    volatile uint32_t tx_buff_len;
    volatile uint32_t tx_buff_start;
    volatile uint32_t tx_buff_end;
    volatile uint32_t tx_send_is_running;
    volatile uint32_t tx_sending_read_point;
    volatile uint32_t tx_buff_available_len;
    /*use transparent send and receive mechanism*/
    volatile uint32_t tx_send_src_address;
    volatile uint32_t tx_send_src_length;
    volatile uint32_t rx_receive_src_address;
    volatile uint32_t rx_receive_src_length;
}virtual_read_write_point_t;


#define MUX_DEVICE_HW_RUNNING 0x01
#define MUX_DEVICE_HW_IDLE 0x0

/*define CONNECT & DISCONNECT status*/
#define MUX_PORT_CONNECT       0xee       //  used to restore connect status to share buffer in across core scenario
#define MUX_PORT_DISCONNECT    0xff       //  used to restore disconnect status to share buffer in across core scenario
#define MUX_PORT_INITIALIZED   0x01       //  used when user call mux_init() to set a port to initialized
#define MUX_PORT_UNINITIALIZED 0x00       //  used when user call mux_deinit() to set a port to uninitialized

extern  void mux_driver_debug_for_check(virtual_read_write_point_t *p);
extern void mux_common_device_r_w_point_init(virtual_read_write_point_t *p,mux_port_config_t *p_setting);
extern bool mux_common_device_buf_is_full(virtual_read_write_point_t *p, bool is_rx);
extern uint32_t mux_common_device_get_hw_rptr(virtual_read_write_point_t *p, bool is_rx);
extern uint32_t mux_common_device_get_hw_wptr(virtual_read_write_point_t *p, bool is_rx);
extern void mux_common_device_set_rx_hw_rptr(virtual_read_write_point_t *p, uint32_t move_bytes);
extern void mux_common_device_set_rx_hw_wptr_internal_use(virtual_read_write_point_t *p, uint32_t move_bytes);
extern void mux_common_device_set_tx_hw_rptr_internal_use(virtual_read_write_point_t *p, uint32_t move_bytes);

typedef enum{
    MUX_COMMON_DEVICE_TX_NEED_SENT_DATA,
    MUX_COMMON_DEVICE_TX_NO_NEED_SENT_DATA,
}mux_common_device_send_data_status_t;

extern void mux_common_device_set_tx_hw_wptr(virtual_read_write_point_t *p, uint32_t move_bytes);
uint32_t mux_common_device_get_buf_next_free_block_len(uint32_t addr,uint32_t r_addr,uint32_t w_addr,uint32_t end,uint32_t available_len);
uint32_t mux_common_device_get_buf_next_available_block_len(uint32_t addr,uint32_t r_addr,uint32_t w_addr,uint32_t end,uint32_t available_len);

/* for USB/SPI slave/I2C slave only, not for UART  End****************************************************************************/



#ifdef __cplusplus
}
#endif

#endif

