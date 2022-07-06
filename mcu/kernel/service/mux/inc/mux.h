/* Copyright Statement:
 *
 * (C) 2019  Airoha Technology Corp. All rights reserved.
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

#ifndef __MUX_H__
#define __MUX_H__


#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "mux_port_common.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup kernel_service
 * @{
 * @addtogroup MUX
 * @{
 *
 * @brief This section introduces the MUX APIs including terms and acronyms, supported features, details on how to use MUX, function groups, enums, structures, and functions.
 *
 * MUX is a multiplexer for some communication devices, like: USB/UART/SPI slave/I2C slave etc.
 *
 * @section MUX_Terms_Chapter Terms and acronyms
 *
 * |        Terms         |           Details                              |
 * |----------------------|------------------------------------------------|
 * |\b      MUX          |        Multiplexer            |
 * |\b      Virtual \b Register          |        Interface for SPI and I2C slave            |
 * |\b      NVDM          |        Non-volatile Data Management            |
 * |\b      UART          |  Universal Asynchronous Receiver/Transmitter   |
 * |\b      USB           |           Universal Serial Bus                 |
 * @section MUX_Architecture Architecture
 * @image html mux_sw_architecture_overview.png
 * @section MUX_Features_Chapter Supported features
 * - \b Ports \b management
 *   - The MUX module manages a lot of device ports to do port assignment.
 * - \b Data \b communication
 *   - The MUX module use various device ports to send and receive data.
 * - \b Support \b multiple \b users \b on \b one \b MUX \b channel.
 *   - Some user with same data package format can MUX together to use one MUX port.
 * - \b High \b performance
 *   - Just one time memory copy when MUX send data.
 * - \b Less \b resources \b requirement
 *   - No need task to do MUX send and receive data.
 *   - Use read point and write point to manage the ring buffer.
 *     @image html mux_rx_tx_ring_buffer.png
 * @section MUX_HOWTO How to use MUX Module
 * - \b How \b to \b use \b MUX \b module
 *    - UART as sample:
 *      @code
 *           mux_protocol_t mux_uart1_op;
 *           mux_handle_t uart1_user1_handle,uart1_user2_handle;
 *           mux_port_setting_t setting;
 *           setting.tx_buffer_size = 1024;
 *           setting.rx_buffer_size = 1024;
 *           setting.dev_setting.uart.uart_config.baudrate = HAL_UART_BAUDRATE_921600;
 *           setting.dev_setting.uart.uart_config.word_length = HAL_UART_WORD_LENGTH_8;
 *           setting.dev_setting.uart.uart_config.stop_bit = HAL_UART_STOP_BIT_1;
 *           setting.dev_setting.uart.uart_config.parity = HAL_UART_PARITY_NONE;
 *           setting.dev_setting.uart.flowcontrol_type = MUX_UART_SW_FLOWCONTROL;
 *            mux_uart1_op.tx_protocol_callback = mux_uart1_package_protocol_cb_for_tx;
 *            mux_uart1_op.rx_protocol_callback = mux_uart1_unpackage_protocol_cb_for_rx;
 *
 *            status = mux_init(MUX_UART_1,&setting,&mux_uart1_op);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *            status = mux_open(MUX_UART_1,"uart1_user1",&uart1_user1_handle,uart1_mux_user1_cb,NULL);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *            printf("uart1 user1 open done~~~handle:0x%x\r\n",uart1_user1_handle);
 *
 *            status = mux_open(MUX_UART_1,"uart1_user2",&uart1_user2_handle,uart1_mux_user2_cb,NULL);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *            printf("uart1 user2 open done~~~handle:0x%x\r\n",uart1_user2_handle);
 *      @endcode
 *    - USB as sample:
 *      @code
 *            mux_protocol_t mux_usb2_op;
 *            mux_handle_t usb2_user1_handle,usb2_user2_handle;
 *            mux_port_setting_t setting;
 *            setting.tx_buffer_size = 1024;
 *            setting.rx_buffer_size = 1024;
 *            mux_usb2_op.tx_protocol_callback = NULL;
 *            mux_usb2_op.rx_protocol_callback = mux_usb2_unpackage;
 *
 *            status = mux_init(MUX_USB_COM_2,&setting,&mux_usb2_op);
 *            if(MUX_STATUS_OK != status)
 *             assert(0);
 *
 *            status = mux_open(MUX_USB_COM_2,"usb2_user1",&usb2_user1_handle,usb2_mux_user1_cb,NULL);
 *            if(MUX_STATUS_OK != status)
 *             assert(0);
 *            printf("USB2 user1 open done~~~handle:0x%x\r\n",usb2_user1_handle);
 *
 *            status = mux_open(MUX_USB_COM_2,"usb2_user2",&usb2_user2_handle,usb2_mux_user2_cb,NULL);
 *            if(MUX_STATUS_OK != status)
 *             assert(0);
 *            printf("USB2 user2 open done~~~handle:0x%x\r\n",usb2_user2_handle);
 *      @endcode
 *    - SPI slave as sample:
 *      @code
 *            mux_protocol_t mux_spi_slave_op;
 *            mux_handle_t spi_slave_user1_handle,spi_slave_user2_handle;
 *            mux_port_setting_t setting;
 *            setting.tx_buffer_size = 1024;
 *            setting.rx_buffer_size = 1024;
 *            setting.dev_setting.spi_slave.slave_config.bit_order = HAL_SPI_SLAVE_LSB_FIRST;
 *            setting.dev_setting.spi_slave.slave_config.phase = HAL_SPI_SLAVE_CLOCK_PHASE0;
 *            setting.dev_setting.spi_slave.slave_config.polarity = HAL_SPI_SLAVE_CLOCK_POLARITY0;
 *            setting.dev_setting.spi_slave.slave_config.timeout_threshold = 0xffffffff;
 *            //setting.dev_setting.spi_slave.early_miso = HAL_SPI_SLAVE_EARLY_MISO_DISABLE;
 *
 *            mux_spi_slave_op.tx_protocol_callback = NULL;
 *            mux_spi_slave_op.rx_protocol_callback = mux_spi_slave_unpackage;
 *
 *            status = mux_init(MUX_SPI_SLAVE_0,&setting,&mux_spi_slave_op);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *
 *            status = mux_open(MUX_SPI_SLAVE_0,"spi_slave_user1",&spi_slave_user1_handle,spi_slave_mux_user1_cb,NULL);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *            printf("spi_slave user1 open done~~~handle:0x%x\r\n",spi_slave_user1_handle);
 *
 *            status = mux_open(MUX_SPI_SLAVE_0,"spi_slave_user2",&spi_slave_user2_handle,spi_slave_mux_user2_cb,NULL);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *            printf("spi_slave user2 open done~~~handle:0x%x\r\n",spi_slave_user2_handle);
 *      @endcode
 *    - I2C slave as sample:
 *      @code
 *             #define DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW 0x50
 *             #define DEMO_I2C_SLAVE_ADDRESS_R        0x54
 *             #define DEMO_I2C_SLAVE_ADDRESS_W        0x58
 *             mux_protocol_t mux_i2c_slave_op;
 *             mux_handle_t i2c_slave_user1_handle,i2c_slave_user2_handle;
 *             mux_port_setting_t setting;
 *             setting.tx_buffer_size = 1024;
 *             setting.rx_buffer_size = 1024;
 *             setting.dev_setting.i2c_slave.cr_or_cw_slave_address = DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW;
 *             setting.dev_setting.i2c_slave.r_slave_address = DEMO_I2C_SLAVE_ADDRESS_R;
 *             setting.dev_setting.i2c_slave.w_slave_address = DEMO_I2C_SLAVE_ADDRESS_W;
 *             mux_i2c_slave_op.tx_protocol_callback = NULL;
 *             mux_i2c_slave_op.rx_protocol_callback = mux_i2c_slave_unpackage;
 *
 *             status = mux_init(MUX_I2C_SLAVE_0,&setting,&mux_i2c_slave_op);
 *             if(MUX_STATUS_OK != status)
 *                 assert(0);
 *
 *             status = mux_open(MUX_I2C_SLAVE_0,"i2c_slave_user1",&i2c_slave_user1_handle,i2c_slave_mux_user1_cb,NULL);
 *             if(MUX_STATUS_OK != status)
 *                 assert(0);
 *             printf("i2c_slave user1 open done~~~handle:0x%x\r\n",i2c_slave_user1_handle);
 *
 *             status = mux_open(MUX_I2C_SLAVE_0,"i2c_slave_user2",&i2c_slave_user2_handle,i2c_slave_mux_user2_cb,NULL);
 *             if(MUX_STATUS_OK != status)
 *                 assert(0);
 *             printf("i2c_slave user2 open done~~~handle:0x%x\r\n",i2c_slave_user2_handle);
 *      @endcode
 * - \b How \b to \b implement \b Rx \b and \b Tx \b protocol_callback
 *     - \b About \b tx_protocol_callback \n
 *          When the user owns a mux channel, means maybe no need add package header,
 *                   then the parameter of tx_protocol_callback should be NULL.\n
 *          When multiple users want to share a mux channel, means need add package header,
 *                   then the parameter of tx_protocol_callback should implement by user.\n
 *          For example:
 *      @code
 *                                       void mux_uart1_package_protocol_cb_for_tx(mux_handle_t handle, const mux_buffer_t payload[],uint32_t buffers_counter, mux_buffer_t *head, mux_buffer_t *tail,void *user_data)
 *                                       {
 *                                           //step1:Analysis the handle to identify which one user.
 *                                                  user_id = analysis_handle(handle);
 *                                          //step2: Calculate the payload size.
 *                                                  size = calculate_size(payload,buffers_counter);
 *                                          //step3:prepare the header buffer if need.
 *                                                  //prepare header buffer
 *                                                  //User must pay attention to the buffer copy length and make sure that it does not exceed TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN
 *                                                  head->p_buf[0] = 0x11;
 *                                                  head->p_buf[1] = 0x22;
 *                                                  //....
 *                                                  head->p_buf[N] = 0xnn;
 *                                                  //User must pay attention to N must be less than TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN
 *                                                  head->buf_size = N;
 *                                         //step4:prepare the tail buffer if need.
 *                                                  //User must pay attention to the buffer copy length and make sure that it does not exceed TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN
 *                                                  tail->p_buf[0] = 0x11;
 *                                                  tail->p_buf[1] = 0x22;
 *                                                  //....
 *                                                  tail->p_buf[M] = 0xnn;
 *                                                  //User must pay attention to M must be less than TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN
 *                                                  tail->buf_size = M;
 *                                      }
 *      @endcode
 *     - \b About \b rx_protocol_callback \n
 *          When the user owns a mux channel, means maybe no need add package tail,
 *                   then the parameter of rx_protocol_callback should be NULL.\n
 *          When multiple users want to share a mux channel, user decide whether need tail in the package or not.
 *                   if need, the parameter of tx_protocol_callback should implement by user.\n
 *
 *          For example:
 *      @code
 *                                       void mux_uart1_unpackage_protocol_cb_for_rx(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
 *                                                      uint32_t *consume_len, uint32_t *package_len,void *user_data)
 *                                       {
 *                                          //step1:Analysis the handle to identify which one user.
 *                                                 user_id = analysis_handle(handle);
 *                                          //Step2:analysis data to find the abandon data in front, user should pay attention that the consume_len data will be abandoned.
 *                                              *consume_len = analysis_data_to_find_consume_len(buffers,buffers_counter);
 *                                          //Step3:analysis data to find first full package in front, if not find, please set *package_len to 0x0, means the data not enough, mux driver will call the callback again when receive more data.
 *                                              *package_len = analysis_data_to_find_consume_len(buffers,buffers_counter);
 *                                       }
 *      @endcode
 * - \b How \b to \b send \b data
 *     - Send one buffer one time:
 *      @code
 *           uint32_t return_value;
 *           mux_status_t status;
 *           mux_buffer_t temp_buff;
 *           temp_buff.p_buf  = "This is UART1 user1 Tx~~~\r\n";
 *           temp_buff.buf_size = strlen(temp_buff.p_buf);
 *           mux_tx(uart1_user1_handle,&temp_buff,1,&return_value);
 *      @endcode
 *     - Send multi buffers one time:
 *      @code
 *           uint32_t return_value;
 *           mux_status_t status;
 *           mux_buffer_t temp_buff[3];
 *           temp_buff[0].p_buf  = "This is UART1 user1 Tx~~~ one\r\n";
 *           temp_buff[0].buf_size = strlen(temp_buff[0].p_buf);
 *           temp_buff[1].p_buf  = "This is UART1 user1 Tx~~~ two\r\n";
 *           temp_buff[1].buf_size = strlen(temp_buff[1].p_buf);
 *           temp_buff[2].p_buf  = "This is UART1 user1 Tx~~~ three\r\n";
 *           temp_buff[2].buf_size = strlen(temp_buff[2].p_buf);
 *           mux_tx(uart1_user1_handle,&temp_buff,3,&return_value);
 *      @endcode
 * - \b How \b to \b receive \b data
 *    - UART as sample:
 *      @code
 *            void uart1_mux_user1_cb(mux_handle_t handle, mux_event_t event,uint32_t data_len,void*user_data)
 *            {
 *                if(MUX_EVENT_READY_TO_READ == event)
 *                {
 *                    printf("this is uart1 user1 MUX_EVENT_READY_TO_READ");
 *                    // User also can just send a message to task on here, and call mux_rx() to get data on task thread.
 *                    {
 *                        char *temp_buff[100];
 *                        mux_buffer_t buffer;
 *                        buffer.buf_size = 100;
 *                        buffer.p_buf = temp_buff;
 *                        uint32_t len;
 *                        mux_rx(uart1_user1_handle, &buffer,&len);
 *                        printf("vMuxTest_UART1_user1_Rx, get @@@@%d data\r\n",len);
 *                   }
 *
 *                }
 *                else if(MUX_EVENT_READY_TO_WRITE == event)
 *                    printf("this is uart1 user1 MUX_EVENT_READY_TO_WRITE");
 *            }
 *      @endcode
 * - \b How \b to \b get \b and \n save \b MUX \b port \b setting \b from \b NVDM
 *    - Take UART as sample:
 *      @code
 *            mux_protocol_t mux_uart1_op;
 *            mux_handle_t uart1_user1_handle,uart1_user2_handle;
 *            mux_port_setting_t setting;
 *            setting.tx_buffer_size = 1024;
 *            setting.rx_buffer_size = 1024;
 *            setting.dev_setting.uart.uart_config.baudrate = HAL_UART_BAUDRATE_921600;
 *            setting.dev_setting.uart.uart_config.word_length = HAL_UART_WORD_LENGTH_8;
 *            setting.dev_setting.uart.uart_config.stop_bit = HAL_UART_STOP_BIT_1;
 *            setting.dev_setting.uart.uart_config.parity = HAL_UART_PARITY_NONE;
 *            setting.dev_setting.uart.flowcontrol_type = MUX_UART_SW_FLOWCONTROL;
 *            mux_uart1_op.tx_protocol_callback = mux_uart1_package_protocol_cb_for_tx;
 *            mux_uart1_op.rx_protocol_callback = mux_uart1_unpackage_protocol_cb_for_rx;
 *
 *            status = mux_init(MUX_UART_1,&setting,&mux_uart1_op);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *
 *            status = mux_save_port_setting_to_nvdm(MUX_UART_1, &setting);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *
 *            status = mux_open(MUX_UART_1,"uart1_user1",&uart1_user1_handle,uart1_mux_user1_cb,NULL);
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *
 *            status = mux_open_save_to_nvdm(MUX_UART_1, "uart1_user1");
 *            if(MUX_STATUS_OK != status)
 *                assert(0);
 *
 *            //the user had opened port
 *            mux_port_buffer_t buffer;
 *            //query which port that uart1_user1 had opened
 *            status = mux_query_port_numbers_from_nvdm("uart1_user1", &buffer);
 *            if (status != MUX_STATUS_OK)
 *                assert(0);
 *            //can get the porting setting when got uart1_user1 used port
 *            status = mux_query_port_setting_from_nvdm(buffer.buf[0], &setting);
 *            if (status != MUX_STATUS_OK)
 *                assert(0);
 *
 *            //delete user from NVDM
 *            status = mux_close_delete_from_nvdm(MUX_UART_1, "uart1_user1");
 *            if(MUX_STATUS_OK != status)
 *                return status;
 *      @endcode
 * @section MUX_MASTER_HOWTO Mux support multiple devices
 * - \b MUX \b support \b UART \b and \b USB \n
 *      UART and USB are basic modules for data communication. Those serial devices are easy to use by mux_rx and mux_rx to do data communication.\n
 * - \b MUX \b support \b SPI \b slave \n
 *      The architecture of MUX SPI slave as below:
 *      @image html mux_spi_slave_architecture.png
 * - \b MUX \b support \b I2C \b slave \n
 *      The architecture of MUX I2C slave as below:
 *      @image html mux_i2c_slave_architecture.png
 * @section MUX_MASTER_DEMO The demo code of host chip
 *    Airoha SDK provide the service of MUX to support SPI slave and I2C slave. Because of only SPI master and I2C master can start a data transmission,
 *   we must make some conventions between master and slave.
 *    For example,  Master need query Slave Tx data length, then start a transmission to receive the data on master slide.
 * - \b Master \b demo \b code \b of  \b common \b MUX \b service \n
 *      @code
 * typedef enum {
 *     HOST_MUX_I2C_MASTER_BEGIN,
 *     HOST_MUX_I2C_MASTER_0 = HOST_MUX_I2C_MASTER_BEGIN,
 *     HOST_MUX_I2C_MASTER_1,
 *     HOST_MUX_I2C_MASTER_END = HOST_MUX_I2C_MASTER_1,
 *     HOST_MUX_SPI_MASTER_BEGIN,
 *     HOST_MUX_SPI_MASTER_0 = HOST_MUX_SPI_MASTER_BEGIN,
 *     HOST_MUX_SPI_MASTER_1,
 *     HOST_MUX_SPI_MASTER_END = HOST_MUX_SPI_MASTER_1,
 *
 *     HOST_MUX_PORT_END,
 * } host_mux_port_t;
 *
 *
 * typedef enum {
 *     HOST_MUX_STATUS_OK,
 *     HOST_MUX_STATUS_ERROR,
 *     HOST_MUX_STATUS_ERROR_PARAMETER,
 *     HOST_MUX_STATUS_ERROR_INIT,
 *     HOST_MUX_STATUS_ERROR_NOT_INIT,
 *     HOST_MUX_STATUS_ERROR_INITIATED,
 * } host_mux_status_t;
 *
 *#include "host_mux_portable.h"
 *#include "host_mux_spi_master_portable.h"
 *
 * host_mux_status_t host_mux_init(host_mux_port_t port)
 * {
 *     host_mux_status_t status = HOST_MUX_STATUS_OK;
 *     if((port >= HOST_MUX_I2C_MASTER_BEGIN) && (port <= HOST_MUX_I2C_MASTER_END))
 *         status = portable_HAL_I2C_MASTER_INIT(port);
 *     else if((port >= HOST_MUX_SPI_MASTER_BEGIN) && (port <= HOST_MUX_SPI_MASTER_END))
 *         status = portable_HAL_SPI_MASTER_INIT(port);
 *     return status;
 * }
 *
 * host_mux_status_t host_mux_deinit(host_mux_port_t port)
 * {
 *     host_mux_status_t status = HOST_MUX_STATUS_OK;
 *     if((port >= HOST_MUX_I2C_MASTER_BEGIN) && (port <= HOST_MUX_I2C_MASTER_END))
 *         status = portable_HAL_I2C_MASTER_DEINIT(port);
 *     else if((port >= HOST_MUX_SPI_MASTER_BEGIN) && (port <= HOST_MUX_SPI_MASTER_END))
 *         status = portable_HAL_SPI_MASTER_DEINIT(port);
 *     return status;
 * }
 * host_mux_status_t host_mux_open(host_mux_port_t port,void*user_data)
 * {
 *     return HOST_MUX_STATUS_OK;
 * }
 * host_mux_status_t host_mux_close(host_mux_port_t port)
 * {
 *     return HOST_MUX_STATUS_OK;
 * }
 * host_mux_status_t host_mux_rx(host_mux_port_t port, uint8_t*buffer, uint32_t *receive_done_data_len)
 * {
 *     host_mux_status_t status = HOST_MUX_STATUS_OK;
 *     if((port >= HOST_MUX_I2C_MASTER_BEGIN) && (port <= HOST_MUX_I2C_MASTER_END))
 *            status = portable_HAL_I2C_MASTER_RX(port,buffer, receive_done_data_len);
 *     else if((port >= HOST_MUX_SPI_MASTER_BEGIN) && (port <= HOST_MUX_SPI_MASTER_END))
 *            status = portable_HAL_SPI_MASTER_RX(port,buffer, receive_done_data_len);
 *        return status;
 * }
 * host_mux_status_t host_mux_tx(host_mux_port_t port, uint8_t*buffer,uint32_t *send_done_data_len)
 * {
 *     host_mux_status_t status = HOST_MUX_STATUS_OK;
 *     if((port >= HOST_MUX_I2C_MASTER_BEGIN) && (port <= HOST_MUX_I2C_MASTER_END))
 *         status = portable_HAL_I2C_MASTER_TX(port,buffer, send_done_data_len);
 *     if((port >= HOST_MUX_SPI_MASTER_BEGIN) && (port <= HOST_MUX_SPI_MASTER_END))
 *         status = portable_HAL_SPI_MASTER_TX(port,buffer, send_done_data_len);
 *     return status;
 * }
 *      @endcode
 * - \b Master \b demo \b code \b of  \b some \b common \b service  \b portable \b layer \n
 *      @code
 *
 *
 * //On this demo need to use some OS service of MUTX and semaphore
 * #define portable_SemaphoreHandle_t SemaphoreHandle_t
 * void portable_MUTEX_TAKE(portable_SemaphoreHandle_t *p_sema_handle);
 * void portable_MUTEX_GIVE(portable_SemaphoreHandle_t *p_sema_handle);
 * #define portable_xSemaphoreCreateMutex xSemaphoreCreateMutex
 * #define portable_xSemaphoreCreateBinary xSemaphoreCreateBinary
 * #define portable_xSemaphoreTake xSemaphoreTake
 * #define portable_xSemaphoreGive xSemaphoreGive
 * #define portable_xSemaphoreGiveFromISR xSemaphoreGiveFromISR
 *
 * //On Arioha Chip, SPI master DMA only access non-cacheable buffer, and the buffer should be 4B align
 * #define portable_Malloc_Non_cacheable pvPortMallocNC
 * #define portable_Free_Non_cacheable vPortFreeNC
 * extern void vPortFreeNC( void *pv );
 * extern void *pvPortMallocNC( size_t xWantedSize );
 *
 * //On this demo need to use busy loop delay
 * #define portable_CPU_BUSY_LOOP_DELAY_US hal_gpt_delay_us
 * #define portable_CPU_BUSY_LOOP_DELAY_MS hal_gpt_delay_ms
 *
 *  void portable_MUTEX_TAKE(portable_SemaphoreHandle_t *p_sema_handle)
 *  {
 *      printf("portable_MUTEX_TASK\r\n");
 *      if (portable_xSemaphoreTake(*p_sema_handle, portMAX_DELAY) == pdFALSE) {
 *          printf("host mux portable_MUTEX_TAKE error\r\n");
 *          {printf("Assert!!!");     while(1);    assert(0);}
 *          return;
 *      }
 *  }
 *  void portable_MUTEX_GIVE(portable_SemaphoreHandle_t *p_sema_handle)
 *  {
 *      printf("portable_MUTEX_GIVE\r\n");
 *      if (portable_xSemaphoreGive(*p_sema_handle) == pdFALSE) {
 *          printf("portable_MUTEX_GIVE error\r\n");
 *          {printf("Assert!!!");     while(1);    assert(0);}
 *          return;
 *      }
 *  }
 *      @endcode
 * - \b Master \b demo \b code \b of  \b SPI \b master  \b portable \b layer \b as \b below: \n
 *      @code
 * #define MAX_ERROR_NUMBER 1000
 * #define SPIS_CFG_RD_CMD         0x0a
 * #define SPIS_RD_CMD             0x81
 * #define SPIS_CFG_WR_CMD         0x0c
 * #define SPIS_WR_CMD             0x0e
 * #define SPIS_RS_CMD             0x06
 * #define SPIS_WS_CMD             0x08
 * #define SPIS_PWON_CMD           0x04
 * #define SPIS_PWOFF_CMD          0x02
 * #define SPIS_CT_CMD             0x10
 *
 *
 *#define VG_SPI_SLAVE_CMD_LEN 9 // The first word[31:16]: for cmd. he first word[15:0]::for reg index  The second word: for reg len
 * //matser<--> slave CR or CW data format:
 * //        Word 0:
 * //           [31:16]- VG_SPI_SLAVE_CR_CMD or VG_SPI_SLAVE_CW_CMD
 * //            [15:0]-   VG_SPI_SLAVE_xxx_xxx_REG_OFFSET
 * //        Word:
 * //            [31:0] Register len.
 *
 *#define VG_SPI_SLAVE_TX_LEN_REG_OFFSET 0x08
 *#define VG_SPI_SLAVE_TX_BUF_REG_OFFSET 0x2000
 *
 *#define VG_SPI_SLAVE_RX_LEN_REG_OFFSET 0x04
 *#define VG_SPI_SLAVE_RX_BUF_REG_OFFSET 0x1000
 *
 *
 *
 *
 *
 *
 *
 *#define HOST_MUX_SPIS_STA_SLV_ON_OFFSET           (0)
 *#define HOST_MUX_SPIS_STA_SLV_ON_MASK             (0x1<<HOST_MUX_SPIS_STA_SLV_ON_OFFSET)
 *
 *#define HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_OFFSET    (2)
 *#define HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK      (0x1<<HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_OFFSET)
 *
 *#define HOST_MUX_SPIS_STA_RDWR_FINISH_OFFSET      (5)
 *#define HOST_MUX_SPIS_STA_RDWR_FINISH_MASK        (0x1<<HOST_MUX_SPIS_STA_RDWR_FINISH_OFFSET)
 *
 *#define HOST_MUX_SPIS_STA_RD_ERR_OFFSET           (3)
 *#define HOST_MUX_SPIS_STA_RD_ERR_MASK             (0x1<<HOST_MUX_SPIS_STA_RD_ERR_OFFSET)
 *
 *#define HOST_MUX_SPIS_STA_WR_ERR_OFFSET           (4)
 *#define HOST_MUX_SPIS_STA_WR_ERR_MASK             (0x1<<HOST_MUX_SPIS_STA_WR_ERR_OFFSET)
 *
 * static volatile uint32_t g_host_mux_spi_master_power_on_counter[HAL_SPI_MASTER_MAX] = {0};
 *
 * static portable_SemaphoreHandle_t g_host_mux_spi_master_mutex = NULL;
 *
 * static portable_SemaphoreHandle_t g_host_mux_spi_master_wait_send_done_xSemaphore = NULL;
 * static portable_SemaphoreHandle_t g_host_mux_spi_master_wait_receive_done_xSemaphore = NULL;
 *
 *
 *
 * static host_mux_status_t portable_HAL_SPI_POWER_ON(hal_spi_master_port_t port);
 * static host_mux_status_t portable_HAL_SPI_POWER_OFF(hal_spi_master_port_t port);
 * static host_mux_status_t portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(hal_spi_master_port_t master_port,uint32_t bit_mask,uint32_t bit_value,uint32_t retry_counter);
 *
 * static host_mux_status_t portable_HAL_SPI_POWER_ON(hal_spi_master_port_t port)
 * {
 *     uint8_t poweron_cmd;
 *     if(g_host_mux_spi_master_power_on_counter[port] == 0){
 *         while(1){
 *             poweron_cmd = SPIS_PWON_CMD;
 *             if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(port, &poweron_cmd, 1)) {
 *                 printf("portable_HAL_SPI_POWER_ON ERROR. hal_spi_master_send_polling fail!!!\r\n");
 *                 return HOST_MUX_STATUS_ERROR;
 *             }
 *
 *             if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_SLV_ON_MASK,
 *                 HOST_MUX_SPIS_STA_SLV_ON_MASK,1000)==HOST_MUX_STATUS_OK)
 *                 break;
 *         }
 *     }
 *     g_host_mux_spi_master_power_on_counter[port] ++;
 *     printf("portable_HAL_SPI_POWER_ON success!!!");
 *     return HOST_MUX_STATUS_OK;
 * }
 *
 * static host_mux_status_t portable_HAL_SPI_POWER_OFF(hal_spi_master_port_t port)
 * {
 *     uint8_t poweron_cmd;
 *     if(g_host_mux_spi_master_power_on_counter[port] == 0)
 *         {printf("Assert!!!");     while(1);    assert(0);}
 *     if(g_host_mux_spi_master_power_on_counter[port] == 1){
 *         while(1){
 *             poweron_cmd = SPIS_PWOFF_CMD;
 *             if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(port, &poweron_cmd, 1)) {
 *             printf("portable_HAL_SPI_POWER_OFF ERROR. hal_spi_master_send_polling fail!!!\r\n");
 *             return HOST_MUX_STATUS_ERROR;
 *             }
 *             if( portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_SLV_ON_MASK,0,1000) == HOST_MUX_STATUS_OK)
 *                 break;
 *         }
 *     }
 *     g_host_mux_spi_master_power_on_counter[port] --;
 *     printf("portable_HAL_SPI_POWER_OFF success!!!");
 *     return HOST_MUX_STATUS_OK;
 * }
 *
 *
 * static host_mux_status_t portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(hal_spi_master_port_t master_port,uint32_t bit_mask,uint32_t bit_value,uint32_t retry_counter)
 * {
 *     uint8_t status_cmd = SPIS_RS_CMD;
 *     uint8_t status_receive[2];
 *     uint8_t clear_cmd_buf[2];
 *     uint8_t status;
 *     uint32_t i;
 *     hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
 *
 *
 *     // Note:
 *     // * The value of receive_length is the valid number of bytes received plus the number of bytes to send.
 *     // * For example, here the valid number of bytes received is 1 byte,
 *     // * and the number of bytes to send also is 1 byte, so the receive_length is 2.
 *
 *     for(i=0;i<retry_counter;i++)
 *     {
 *         status_receive[1] = 0;
 *         spi_send_and_receive_config.receive_length = 2;
 *         spi_send_and_receive_config.send_length = 1;
 *         spi_send_and_receive_config.send_data = &status_cmd;
 *         spi_send_and_receive_config.receive_buffer = status_receive;
 *         if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_and_receive_polling(master_port, &spi_send_and_receive_config)) {
 *             printf("portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY,SPI master query status of slave failed\r\n");
 *             return HOST_MUX_STATUS_ERROR;
 *         }
 *         printf("portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY  Status receive: 0x%x\r\n", status_receive[1]);
 *         status = status_receive[1];
 *         if(status & (SPIS_STA_RD_ERR_MASK|SPIS_STA_WR_ERR_MASK) )//|SPIS_STA_CMD_ERR_MASK
 *         {
 *             printf("portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY,Slave tansfer Error:%d, need reset Slave \r\n",status);
 *             clear_cmd_buf[0]= SPIS_WS_CMD;
 *             clear_cmd_buf[1] = status;
 *             if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(master_port,clear_cmd_buf,2)) {
 *                 printf("portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY,SPI master query status of slave failed\r\n");
 *                 {printf("Assert!!!");     while(1);    assert(0);}
 *                 return HOST_MUX_STATUS_ERROR;
 *             }
 *             printf("Transfer error, now Master reset Slave!!!\r\n");
 *             portable_CPU_BUSY_LOOP_DELAY_US(100);
 *             portable_HAL_SPI_POWER_OFF(master_port);
 *             portable_HAL_SPI_POWER_ON(master_port);
 *             return HOST_MUX_STATUS_ERROR;
 *         }
 *         else if((bit_mask & status)== bit_value)
 *         {
 *             printf("portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY OK, retry counter:%d,\r\n",(int)i);
 *             return HOST_MUX_STATUS_OK;
 *         }
 *         portable_CPU_BUSY_LOOP_DELAY_US(100);
 *     }
 *     printf("portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY, retry counter:%d,\r\n",(int)i);
 *     return HOST_MUX_STATUS_ERROR;
 * }
 *
 *
 *
 *
 * static void portable_HOST_MUX_SPI_MASTER_wait_for_send_done()
 * {
 *    if(portable_xSemaphoreTake( g_host_mux_spi_master_wait_send_done_xSemaphore, portMAX_DELAY) == pdTRUE)
 *    {
 *    }
 *    else
 *    {
 *        {printf("Assert!!!");     while(1);    assert(0);}
 *    }
 *
 * }
 * static void portable_HOST_MUX_SPI_MASTER_wait_for_receive_done()
 * {
 *    if(portable_xSemaphoreTake( g_host_mux_spi_master_wait_receive_done_xSemaphore, portMAX_DELAY) == pdTRUE)
 *    {
 *    }
 *    else
 *    {
 *        {printf("Assert!!!");     while(1);    assert(0);}
 *    }
 * }
 * static void portable_HOST_MUX_SPI_MASTER_RECEIVE_CB(hal_spi_master_callback_event_t event, void *user_data)
 * {
 *     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *     uint32_t receive_done_len = (uint32_t)user_data;
 *
 *     if(event == HAL_SPI_MASTER_EVENT_RECEIVE_FINISHED)
 *     {
 *         printf("portable_HOST_MUX_SPI_MASTER_RECEIVE_CB receive_done_len:%d\r\n",(int)receive_done_len);
 *         portable_xSemaphoreGiveFromISR( g_host_mux_spi_master_wait_receive_done_xSemaphore, &xHigherPriorityTaskWoken );
 *
 *      }
 *     else
 *     {
 *         printf("portable_HOST_MUX_SPI_MASTER_RECEIVE_CB ERROR, err event:%d\r\n",event);
 *         {printf("Assert!!!");     while(1);    assert(0);}
 *     }
 *
 * }
 * static void portable_HOST_MUX_SPI_MASTER_SEND_CB(hal_spi_master_callback_event_t event, void *user_data)
 * {
 *     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *     uint32_t send_done_len = (uint32_t)user_data;
 *     if(event == HAL_SPI_MASTER_EVENT_SEND_FINISHED)
 *     {
 *         printf("portable_HOST_MUX_SPI_MASTER_SEND_CB send_done_len:%d\r\n",(int)send_done_len);
 *         portable_xSemaphoreGiveFromISR( g_host_mux_spi_master_wait_send_done_xSemaphore, &xHigherPriorityTaskWoken );
 *     }
 *     else
 *     {
 *         printf("portable_HOST_MUX_SPI_MASTER_SEND_CB ERROR, err event:%d\r\n",event);
 *         {printf("Assert!!!");     while(1);    assert(0);}
 *     }
 * }
 *
 *
 *
 *
 *
 *
 * host_mux_status_t portable_HAL_SPI_MASTER_INIT(host_mux_port_t port)
 * {
 *
 *     hal_spi_master_config_t spi_master_config;
 *     hal_spi_master_port_t spi_port;
 *
 *     if(g_host_mux_spi_master_mutex == NULL)
 *         g_host_mux_spi_master_mutex = portable_xSemaphoreCreateMutex();
 *     if (g_host_mux_spi_master_mutex == NULL) {
 *         printf( "g_host_mux_spi_master_mutex create error\r\n");
 *         return HOST_MUX_STATUS_ERROR_INIT;
 *     }
 *     if(g_host_mux_spi_master_wait_send_done_xSemaphore == NULL)
 *         g_host_mux_spi_master_wait_send_done_xSemaphore = portable_xSemaphoreCreateBinary();
 *     if (g_host_mux_spi_master_wait_send_done_xSemaphore == NULL) {
 *         printf( "g_host_mux_spi_master_wait_send_done_xSemaphore create error\r\n");
 *         return HOST_MUX_STATUS_ERROR_INIT;
 *     }
 *     if(g_host_mux_spi_master_wait_receive_done_xSemaphore == NULL)
 *         g_host_mux_spi_master_wait_receive_done_xSemaphore = portable_xSemaphoreCreateBinary();
 *     if (g_host_mux_spi_master_wait_receive_done_xSemaphore == NULL) {
 *         printf( "g_host_mux_spi_master_wait_receive_done_xSemaphore create error\r\n");
 *         return HOST_MUX_STATUS_ERROR_INIT;
 *     }
 *
 *     if(port == HOST_MUX_SPI_MASTER_0)
 *            spi_port =  HAL_SPI_MASTER_0;
 *     if(port == HOST_MUX_SPI_MASTER_1)
 *            spi_port =  HAL_SPI_MASTER_1;
 *
 *     spi_master_config.bit_order = HAL_SPI_MASTER_LSB_FIRST;
 *     spi_master_config.clock_frequency = 1000000;
 *     spi_master_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
 *     spi_master_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
 *     spi_master_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
 *
 *     if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(spi_port, &spi_master_config)) {
 *         printf( "portable_HAL_SPI_MASTER_INIT hal_spi_master_init error!!!\r\n");
 *             return HOST_MUX_STATUS_ERROR_INIT;
 *     }
 *     hal_spi_master_set_mode(spi_port, HAL_SPI_MASTER_SINGLE_MODE);
 *     return HOST_MUX_STATUS_OK;
 * }
 *
 *
 * host_mux_status_t portable_HAL_SPI_MASTER_DEINIT(host_mux_port_t port)
 * {
 *   hal_spi_master_port_t spi_port;
 *   if(port == HOST_MUX_SPI_MASTER_0)
 *          spi_port =  HAL_SPI_MASTER_0;
 *   if(port == HOST_MUX_SPI_MASTER_1)
 *          spi_port =  HAL_SPI_MASTER_1;
 *
 *   if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(spi_port)) {
 *         printf( "mux_spi_master_demo_deinit hal_spi_master_deinit error!!!\r\n");
 *             return HOST_MUX_STATUS_ERROR_INIT;
 *     }
 *     return HOST_MUX_STATUS_OK;
 * }
 *
 *
 *
 * //     This is Master read data from slave:
 * //     Step0: Power ON SPI slave
 * //     Step1:Master read Slave the register of <VG_SPI_SLAVE_TX_LEN_REG_OFFSET> with the len of 4
 * //          Step1_a: Master send Config Read (CR) cmd to slave:
 * //                     Master send to slave 9 bytes cmd: [0]:SPIS_CFG_RD_CMD, [1:4]VG_SPI_SLAVE_TX_LEN_REG_OFFSET,[5:8] len-1
 * //          Step1_b: Wait for SPI slave CR ready,check HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK.
 * //          Step1_c: Master receive the value of <VG_SPI_SLAVE_TX_LEN_REG_OFFSET>
 * //                Master receive 4 bytes, it's the data len which Slave prepared.
 * //          Step1_d: Master query and check Slave_Tx done status,check HOST_MUX_SPIS_STA_RDWR_FINISH_MASK.
 * //     Step2:Master read Slave the register of <VG_SPI_SLAVE_TX_BUF_REG_OFFSET> with the len of data_buffer_len.
 * //             Step2_a: Master send Config Read (CR) cmd to slave:
 * //                Master send to slave 9 bytes cmd: [0]:SPIS_CFG_RD_CMD, [1:4]VG_SPI_SLAVE_TX_BUF_REG_OFFSET,[5:8] len-1
 * //             Step2_b: Wait for SPI slave CR ready,,check HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK.
 * //             Step2_c: Master receive <data_buffer_len> data.
 * //                     Master receive <data_buffer_len> bytes, it's the data which Slave prepared
 * //             Step2_d:Master and check Slave_Tx done status,check HOST_MUX_SPIS_STA_RDWR_FINISH_MASK.
 * //      Step3: Power OFF
 *
 *
 * host_mux_status_t portable_HAL_SPI_MASTER_RX(host_mux_port_t mux_port, uint8_t*buf, uint32_t *receive_done_data_len)
 * {
 *        hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
 *        hal_spi_master_status_t status;
 *        uint8_t request_cmd[9];
 *        uint32_t receive_reg_value;
 *        uint32_t length,offset;
 *        static ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t temp_host_mux_send_buf[1];//Airoha chip SPI master need the buffer 4B align
 *        static ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t temp_host_mux_receive_buf[4+1];//Airoha chip SPI master need the buffer 4B align
 *        hal_spi_master_port_t port;
 *         if(mux_port == HOST_MUX_SPI_MASTER_0)
 *             port =  HAL_SPI_MASTER_0;
 *         if(mux_port == HOST_MUX_SPI_MASTER_1)
 *             port =  HAL_SPI_MASTER_1;
 *
 *   //Step0: Power ON SPI slave
 *        portable_MUTEX_TAKE(&g_host_mux_spi_master_mutex);
 *        portable_HAL_SPI_POWER_ON(port);
 *        portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *
 * mux_spi_master_demo_receive_restart:
 *        portable_MUTEX_TAKE(&g_host_mux_spi_master_mutex);
 *     //Step1:Master read Slave the register of <VG_SPI_SLAVE_TX_LEN_REG_OFFSET> with the len of 4
 *         // Step1_a: Master send Config Read (CR) cmd to slave:
 *         //            Master send to slave 9 bytes cmd: [0]:SPIS_CFG_RD_CMD, [1:4]VG_SPI_SLAVE_TX_LEN_REG_OFFSET,[5:8] len-1
 *         length = 4;
 *         offset = VG_SPI_SLAVE_TX_LEN_REG_OFFSET;
 *         request_cmd[0] = SPIS_CFG_RD_CMD;
 *         request_cmd[1] = offset & 0xff;
 *         request_cmd[2] = (offset >> 8) & 0xff;
 *         request_cmd[3] = (offset >> 16) & 0xff;
 *         request_cmd[4] = (offset >> 24) & 0xff;
 *         request_cmd[5] = (length - 1) & 0xff;
 *         request_cmd[6] = ((length - 1) >> 8) & 0xff;
 *         request_cmd[7] = ((length - 1) >> 16) & 0xff;
 *         request_cmd[8] = ((length - 1) >> 24) & 0xff;
 *         printf("mux_spi_master_demo_receive-Step1_a: Send VG_SPI_SLAVE_CRD_CMD.\r\n");
 *         printf("mux_spi_master_demo_receive-Step1_a: Master want to send 9B\r\n");
 *         status = hal_spi_master_send_polling(port,request_cmd,VG_SPI_SLAVE_CMD_LEN);
 *         if( HAL_SPI_MASTER_STATUS_OK == status){
 *             printf("mux_spi_master_demo_receive-Step1_a: Send VG_SPI_SLAVE_CRD_CMD success!!!\r\n");
 *         }
 *         else{
 *          printf("mux_spi_master_demo_receive-Step1_a:: SPI master send err, status:%d \r\n",status);
 *            {printf("Assert!!!");     while(1);    assert(0);}
 *         }
 *
 *       // Step1_b: Wait for SPI slave CR ready,check HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK
 *         printf("mux_spi_master_demo_receive-Step1_b: wait slave CR done...\r\n");
 *         if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK,
 *             HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK,MAX_ERROR_NUMBER)==HOST_MUX_STATUS_ERROR)
 *         {
 *             printf("mux_spi_master_demo_receive-Step1_b: #### too many ERROR, now go to restart!!!!!\r\n");
 *             portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *             goto mux_spi_master_demo_receive_restart;
 *         }
 *
 *         //Step1_c: Master receive the value of <VG_SPI_SLAVE_TX_LEN_REG_OFFSET>
 *         //       Master receive 4 bytes, it's the data len which Slave prepared.
 *             printf("mux_spi_master_demo_receive-Step1_c: Receive SPI slave Tx_len Reg value. \r\n");
 *             printf("mux_spi_master_demo_receive-Step1_c: Master want to receive 4B.\r\n");
 *             {
 *                 temp_host_mux_send_buf[0] = SPIS_RD_CMD;
 *                 spi_send_and_receive_config.receive_length = 4+1;
 *                 spi_send_and_receive_config.send_length = 1;
 *                 spi_send_and_receive_config.send_data = temp_host_mux_send_buf;
 *                 spi_send_and_receive_config.receive_buffer = temp_host_mux_receive_buf;
 *                 status = hal_spi_master_send_and_receive_polling(port,&spi_send_and_receive_config);
 *                 if( HAL_SPI_MASTER_STATUS_OK == status){
 *                         // Step1_d: Master query Slave_Tx done status,check HOST_MUX_SPIS_STA_RDWR_FINISH_MASK
 *                         if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_RDWR_FINISH_MASK,
 *                         HOST_MUX_SPIS_STA_RDWR_FINISH_MASK,MAX_ERROR_NUMBER)==HOST_MUX_STATUS_ERROR)
 *                         {
 *                             printf("mux_spi_master_demo_receive-Step1_d: #### too many ERROR, now go to restart!!!!!\r\n");
 *                             portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *                             goto mux_spi_master_demo_receive_restart;
 *                         }
 *                     receive_reg_value = temp_host_mux_receive_buf[1] | (temp_host_mux_receive_buf[2]<<8) | (temp_host_mux_receive_buf[3]<<16)|(temp_host_mux_receive_buf[3]<<24);
 *                     printf("mux_spi_master_demo_receive-Step1_c: Receive SPI slave Tx_len Reg value:0x%x. success!!!\r\n",(unsigned int)receive_reg_value);
 *                 }
 *                 else{
 *                     printf("mux_spi_master_demo_receive-Step1_c : SPI master receive err,status:%d\r\n",status);
 *                     {printf("Assert!!!");     while(1);    assert(0);}
 *                 }
 *             }
 *
 *        if(receive_reg_value > *receive_done_data_len){
 *             receive_reg_value = *receive_done_data_len;
 *             printf("mux_spi_master_demo_receive-Step1_c: slave data len too big, request %d B firstly;\r\n",(int)*receive_done_data_len);
 *        }
 *        if(receive_reg_value == 0){//// receive len should not 0
 *             portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *             *receive_done_data_len = 0;
 *             return HOST_MUX_STATUS_OK;
 *        }
 *        portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *
 *
 *         portable_MUTEX_TAKE(&g_host_mux_spi_master_mutex);
 *     //Step2:Master read Slave the register of <VG_SPI_SLAVE_TX_BUF_REG_OFFSET> with the len of data_buffer_len.
 *     //        Step2_a: Master send Config Read (CR) cmd to slave:
 *     //           Master send to slave 9 bytes cmd: [0]:SPIS_CFG_RD_CMD, [1:4]VG_SPI_SLAVE_TX_BUF_REG_OFFSET,[5:8] len-1
 *         length = receive_reg_value;
 *         offset = VG_SPI_SLAVE_TX_BUF_REG_OFFSET;
 *         request_cmd[0] = SPIS_CFG_RD_CMD;
 *         request_cmd[1] = offset & 0xff;
 *         request_cmd[2] = (offset >> 8) & 0xff;
 *         request_cmd[3] = (offset >> 16) & 0xff;
 *         request_cmd[4] = (offset >> 24) & 0xff;
 *         request_cmd[5] = (length - 1) & 0xff;
 *         request_cmd[6] = ((length - 1) >> 8) & 0xff;
 *         request_cmd[7] = ((length - 1) >> 16) & 0xff;
 *         request_cmd[8] = ((length - 1) >> 24) & 0xff;
 *         printf("mux_spi_master_demo_receive-Step2_a: send VG_SPI_SLAVE_RD_CMD.\r\n");
 *         printf("mux_spi_master_demo_receive-Step2_a: Master want to send 9B cmd.\r\n");
 *         status = hal_spi_master_send_polling(port,request_cmd,VG_SPI_SLAVE_CMD_LEN);
 *         if( HAL_SPI_MASTER_STATUS_OK == status){
 *          printf("mux_spi_master_demo_receive-Step2_a: Send VG_SPI_SLAVE_RD_CMD. success!!!\r\n");
 *         }
 *         else{
 *            printf("mux_spi_master_demo_receive-Step2_a: SPI master send err, try again...status:%d \r\n",status);
 *            {printf("Assert!!!");     while(1);    assert(0);}
 *         }
 *
 *      // Step2_b: Wait for SPI slave CR ready
 *         printf("mux_spi_master_demo_receive-Step2_b: wait slave CR done...\r\n");
 *         if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK,
 *         HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK,MAX_ERROR_NUMBER)==HOST_MUX_STATUS_ERROR)
 *         {
 *             printf("mux_spi_master_demo_receive-Step2_b: #### too many ERROR, now go to restart!!!!!\r\n");
 *             portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *             goto mux_spi_master_demo_receive_restart;
 *         }
 *
 *
 *         //Step2_c: Master receive <data_buffer_len> data.
 *         //            Master receive <data_buffer_len> bytes, it's the data which Slave prepared.
 *        hal_spi_master_register_callback(port, portable_HOST_MUX_SPI_MASTER_RECEIVE_CB, (void *)receive_reg_value);//Register a user callback.
 *         temp_host_mux_send_buf[0] = SPIS_RD_CMD;
 *         spi_send_and_receive_config.receive_length = receive_reg_value+1;
 *         spi_send_and_receive_config.send_length = 1;
 *         spi_send_and_receive_config.send_data = temp_host_mux_send_buf;
 *         spi_send_and_receive_config.receive_buffer = buf;
 *         printf("mux_spi_master_demo_receive-Step2_c: Master want to receive:%d\r\n",(int)receive_reg_value);
 *
 *            status = hal_spi_master_send_and_receive_dma(port, &spi_send_and_receive_config);
 *            if (HAL_SPI_MASTER_STATUS_OK == status) {
 *                 portable_HOST_MUX_SPI_MASTER_wait_for_receive_done();
 *                 //Step2_d:Master and check Slave_Tx done status,check HOST_MUX_SPIS_STA_RDWR_FINISH_MASK.
 *                     if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_RDWR_FINISH_MASK,
 *                     HOST_MUX_SPIS_STA_RDWR_FINISH_MASK,MAX_ERROR_NUMBER)==HOST_MUX_STATUS_ERROR)
 *                     {
 *                         printf("mux_spi_master_demo_receive-Step2_d: #### too many ERROR, now go to restart!!!!!\r\n");
 *                         portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *                         goto mux_spi_master_demo_receive_restart;
 *                     }
 *                 printf("mux_spi_master_demo_receive-Step2_c: Receive Tx_buff data. success\r\n");
 *            }
 *            else{
 *              printf("mux_spi_master_demo_receive-Step2_c: hal_spi_master_send_and_receive_dma return ERROR!!! status:%d\r\n",status);
 *                {printf("Assert!!!");     while(1);    assert(0);}
 *            }
 *        portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *      *receive_done_data_len = receive_reg_value;
 *
 *    //Step3: Power OFF SPI slave
 *        portable_MUTEX_TAKE(&g_host_mux_spi_master_mutex);
 *        portable_HAL_SPI_POWER_OFF(port);
 *        portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *      return HOST_MUX_STATUS_OK;
 * }
 *
 *   //  This is Master write data to slave:
 *   //  Step1:Master read Slave the register of <VG_SPI_SLAVE_RX_LEN_REG_OFFSET> with the len of 4
 *   //          Step1_a: Master send Config Read (CR) cmd to slave:
 *   //              Master send to slave 9 bytes cmd: [0]:SPIS_CFG_RD_CMD, [1:4]VG_SPI_SLAVE_RX_LEN_REG_OFFSET,[5:8] len-1
 *   //          Step1_b: Wait for SPI slave CR ready
 *   //          Step1_c: Master receive the value of <VG_SPI_SLAVE_RX_LEN_REG_OFFSET>
 *   //                   Master receive 4 bytes, it's the data len which Slave prepared.
 *   //          Step1_d: Master query and check Slave_Tx done status,check HOST_MUX_SPIS_STA_RDWR_FINISH_MASK.
 *   //  Step2:Master read Slave the register of <VG_SPI_SLAVE_RX_BUF_REG_OFFSET> with the len of data_buffer_len which master want to write.
 *   //          Step2_a: Master send Config Write (CW) cmd to slave:
 *   //              Master send to slave9 bytes cmd: [0]:SPIS_CFG_RD_CMD, [1:4]VG_SPI_SLAVE_TX_BUF_REG_OFFSET,[5:8] len-1
 *   //          Step2_b: Wait for SPI slave CR ready
 *   //          Step2_c: Master send <data_buffer_len> data.
 *   //              Slave address is DEMO_SPI_SLAVE_ADDRESS_W
 *   //              Master send <data_buffer_len> bytes, it's the data which master want to write.
 *   //          Step2_d:Master and check Slave_Rx done status,check HOST_MUX_SPIS_STA_RDWR_FINISH_MASK.
 *
 *
 * host_mux_status_t portable_HAL_SPI_MASTER_TX(host_mux_port_t mux_port, uint8_t*buf, uint32_t *send_done_data_len)
 * {
 *        hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
 *        hal_spi_master_status_t status;
 *        uint8_t request_cmd[9];
 *        uint32_t receive_reg_value;
 *        uint32_t length,offset;
 *        uint8_t* p_temp_malloc_buff;
 *        static ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t temp_host_mux_send_buf[1];//Airoha chip SPI master need the buffer 4B align
 *        static ATTR_ZIDATA_IN_NONCACHED_SYSRAM_4BYTE_ALIGN uint8_t temp_host_mux_receive_buf[4+1];//Airoha chip SPI master need the buffer 4B align
 *         hal_spi_master_port_t port;
 *         if(mux_port == HOST_MUX_SPI_MASTER_0)
 *             port =  HAL_SPI_MASTER_0;
 *         if(mux_port == HOST_MUX_SPI_MASTER_1)
 *             port =  HAL_SPI_MASTER_1;
 *    //Step0: Power ON SPI slave
 *         portable_MUTEX_TAKE(&g_host_mux_spi_master_mutex);
 *         portable_HAL_SPI_POWER_ON(port);
 *         portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *
 * mux_spi_master_demo_send_restart:
 *
 *     portable_MUTEX_TAKE(&g_host_mux_spi_master_mutex);
 *     //Step1:Master read Slave the register of <VG_SPI_SLAVE_RX_LEN_REG_OFFSET> with the len of 4
 *     //        Step1_a: Master send Config Read (CR) cmd to slave:
 *     //            Master send to slave 9 bytes cmd: [0]:SPIS_CFG_RD_CMD, [1:4]VG_SPI_SLAVE_RX_LEN_REG_OFFSET,[5:8] len-1
 *     length = 4;
 *     offset = VG_SPI_SLAVE_RX_LEN_REG_OFFSET;
 *     request_cmd[0] = SPIS_CFG_RD_CMD;
 *     request_cmd[1] = offset & 0xff;
 *     request_cmd[2] = (offset >> 8) & 0xff;
 *     request_cmd[3] = (offset >> 16) & 0xff;
 *     request_cmd[4] = (offset >> 24) & 0xff;
 *     request_cmd[5] = (length - 1) & 0xff;
 *     request_cmd[6] = ((length - 1) >> 8) & 0xff;
 *     request_cmd[7] = ((length - 1) >> 16) & 0xff;
 *     request_cmd[8] = ((length - 1) >> 24) & 0xff;
 *     printf("mux_spi_master_demo_send-Step1_a: Send VG_SPI_SLAVE_CRD_CMD.\r\n");
 *     printf("mux_spi_master_demo_send-Step1_a: Master want to send 9B cmd.\r\n");
 *     status = hal_spi_master_send_polling(port,request_cmd,VG_SPI_SLAVE_CMD_LEN);
 *     if( HAL_SPI_MASTER_STATUS_OK == status){
 *         printf("mux_spi_master_demo_send-Step1_a: Send VG_SPI_SLAVE_CRD_CMD success!!!\r\n");
 *     }
 *     else{
 *         printf("mux_spi_master_demo_send-Step1_a: SPI master send err, try again...status:%d \r\n",status);
 *         {printf("Assert!!!");     while(1);    assert(0);}
 *     }
 *     // Step1_b: Wait for SPI slave CR ready
 *     printf("mux_spi_master_demo_send-Step1_b: wait slave CR done...\r\n");
 *     if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK,
 *     HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK,MAX_ERROR_NUMBER)== HOST_MUX_STATUS_ERROR)
 *     {
 *         printf("mux_spi_master_demo_send-Step1_b: #### too many ERROR, now go to restart!!!!!\r\n");
 *         portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *         goto mux_spi_master_demo_send_restart;
 *     }
 *     //Step1_c: Master receive the value of <VG_SPI_SLAVE_RX_LEN_REG_OFFSET>
 *     //                 Master receive 4 bytes, it's the data len which Slave prepared.
 *     printf("mux_spi_master_demo_send-Step1_c: receive SPI slave Tx_len Reg value.\r\n");
 *     printf("mux_spi_master_demo_send-Step1_c: Master want to receive 4B.\r\n");
 *     temp_host_mux_send_buf[0] = SPIS_RD_CMD;
 *     spi_send_and_receive_config.receive_length = 4+1;
 *     spi_send_and_receive_config.send_length = 1;
 *     spi_send_and_receive_config.send_data = temp_host_mux_send_buf;
 *     spi_send_and_receive_config.receive_buffer = temp_host_mux_receive_buf;
 *     status = hal_spi_master_send_and_receive_polling(port,&spi_send_and_receive_config);
 *     if( HAL_SPI_MASTER_STATUS_OK == status){
 *         //Step1_d: Master query and check Slave_Tx done status,check HOST_MUX_SPIS_STA_RDWR_FINISH_MASK.
 *         if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_RDWR_FINISH_MASK,
 *         HOST_MUX_SPIS_STA_RDWR_FINISH_MASK,MAX_ERROR_NUMBER)==HOST_MUX_STATUS_ERROR)
 *         {
 *             printf("mux_spi_master_demo_send-Step1_d: #### too many ERROR, now go to restart!!!!!\r\n");
 *             portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *             goto mux_spi_master_demo_send_restart;
 *         }
 *         receive_reg_value = temp_host_mux_receive_buf[1] | (temp_host_mux_receive_buf[2]<<8) | (temp_host_mux_receive_buf[3]<<16)|(temp_host_mux_receive_buf[3]<<24);
 *         printf("mux_spi_master_demo_send-Step1_c: Receive SPI slave Rx_len Reg value:0x%x. success!!!\r\n",(unsigned int)receive_reg_value);
 *     }
 *     else{
 *         printf("mux_spi_master_demo_send-Step1_c: SPI master receive err, try again...status:%d\r\n",status);
 *         {printf("Assert!!!");     while(1);    assert(0);}
 *     }
 *
 *    if(receive_reg_value > *send_done_data_len){
 *         printf("mux_spi_master_demo_send-Step1_c: Receive SPI slave Rx_len Reg value:%d, but master just want to send:%d\r\n",(int)receive_reg_value,(int)*send_done_data_len);
 *         receive_reg_value = *send_done_data_len;
 *     }
 *    if(receive_reg_value == 0){
 *         portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *         *send_done_data_len = 0;
 *         return HOST_MUX_STATUS_OK;
 *    }
 *    portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *
 *    portable_MUTEX_TAKE(&g_host_mux_spi_master_mutex);
 *
 *     //Step2:Master read Slave the register of <VG_SPI_SLAVE_RX_BUF_REG_OFFSET> with the len of data_buffer_len which master want to write.
 *     //        Step2_a: Master send Config Write (CW) cmd to slave:
 *    //             Master send to slave9 bytes cmd: [0]:SPIS_CFG_RD_CMD, [1:4]VG_SPI_SLAVE_TX_BUF_REG_OFFSET,[5:8] len-1
 *     length = receive_reg_value;
 *     offset = VG_SPI_SLAVE_RX_BUF_REG_OFFSET;
 *     request_cmd[0] = SPIS_CFG_WR_CMD;
 *     request_cmd[1] = offset & 0xff;
 *     request_cmd[2] = (offset >> 8) & 0xff;
 *     request_cmd[3] = (offset >> 16) & 0xff;
 *     request_cmd[4] = (offset >> 24) & 0xff;
 *     request_cmd[5] = (length - 1) & 0xff;
 *     request_cmd[6] = ((length - 1) >> 8) & 0xff;
 *     request_cmd[7] = ((length - 1) >> 16) & 0xff;
 *     request_cmd[8] = ((length - 1) >> 24) & 0xff;
 *     printf("mux_spi_master_demo_send-Step2_a: send VG_SPI_SLAVE_WD_CMD.\r\n");
 *     printf("mux_spi_master_demo_send-Step2_a:Master want to send 8B \r\n");
 *     status = hal_spi_master_send_polling(port,request_cmd,VG_SPI_SLAVE_CMD_LEN);
 *     if( HAL_SPI_MASTER_STATUS_OK == status)
 *     {
 *         printf("mux_spi_master_demo_send-Step2_a: Send VG_SPI_SLAVE_RD_CMD. success!!!\r\n");
 *     }
 *     else{
 *        printf("mux_spi_master_demo_send-Step2_a: SPI master send err, try again...status:%d \r\n",status);
 *        {printf("Assert!!!");     while(1);    assert(0);}
 *     }
 *     // Step2_b: Wait for SPI slave CR ready
 *     printf("mux_spi_master_demo_send-Step2_b: wait slave CR done...\r\n");
 *     if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK,
 *             HOST_MUX_SPIS_STA_TXRX_FIFO_RDY_MASK,MAX_ERROR_NUMBER)==HOST_MUX_STATUS_ERROR)
 *     {
 *         printf("mux_spi_master_demo_send-Step1_d: #### too many ERROR, now go to restart!!!!!\r\n");
 *         portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *         goto mux_spi_master_demo_send_restart;
 *     }
 *     //Step2_c: Master send <data_buffer_len> data.
 *    //         Slave address is DEMO_SPI_SLAVE_ADDRESS_W
 *     //        Master send <data_buffer_len> bytes, it's the data which master want to write.
 *
 *         //Just for SPI master design behavior of : the buf address must be alignment with 4B
 *         p_temp_malloc_buff = NULL;
 *         if((uint32_t)buf % 4 != 0){
 *            p_temp_malloc_buff= portable_Malloc_Non_cacheable(receive_reg_value+1);
 *            memcpy(p_temp_malloc_buff+1,buf,receive_reg_value);
 *            buf = p_temp_malloc_buff;
 *         }
 *
 *        hal_spi_master_register_callback(port, portable_HOST_MUX_SPI_MASTER_SEND_CB, (void *)receive_reg_value);//Register a user callback.
 *        printf("mux_spi_master_demo_send-Step2_c:Master want to send:%d\r\n",(int)receive_reg_value);
 *        printf("mux_spi_master_demo_send-Step2_c: master send data. buf address:0x%x\r\n",(unsigned int)buf);
 *        buf[0] = SPIS_WR_CMD;
 *        status = hal_spi_master_send_dma(port, buf, receive_reg_value+1);
 *        if (HAL_SPI_MASTER_STATUS_OK == status) {
 *             portable_HOST_MUX_SPI_MASTER_wait_for_send_done();
 *                 //Step2_d:Master and check Slave_Rx done status,check HOST_MUX_SPIS_STA_RDWR_FINISH_MASK.
 *                 if(portable_HOST_SPI_MASTER_QUERY_SLAVE_STATUS_WITH_RETRY(port,HOST_MUX_SPIS_STA_RDWR_FINISH_MASK,
 *                 HOST_MUX_SPIS_STA_RDWR_FINISH_MASK,MAX_ERROR_NUMBER)==HOST_MUX_STATUS_ERROR)
 *                 {
 *                     printf("mux_spi_master_demo_receive-Step2_d: #### too many ERROR, now go to restart!!!!!\r\n");
 *                     portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *                     goto mux_spi_master_demo_send_restart;
 *                 }
 *              printf("mux_spi_master_demo_send-Step2_c: receive Tx_buff data. success\r\n");
 *        }
 *        else{
 *             printf("mux_spi_master_demo_send-Step2_c: send fail!!!status:%d\r\n",status);
 *            {printf("Assert!!!");     while(1);    assert(0);}
 *        }
 *  if(p_temp_malloc_buff != NULL)
 *         portable_Free_Non_cacheable(p_temp_malloc_buff);
 *      portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *      *send_done_data_len = receive_reg_value;
 *
 *   //Step3: Power OFF SPI slave
 *        portable_MUTEX_TAKE(&g_host_mux_spi_master_mutex);
 *        portable_HAL_SPI_POWER_OFF(port);
 *        portable_MUTEX_GIVE(&g_host_mux_spi_master_mutex);
 *      return HOST_MUX_STATUS_OK;
 * }
 *      @endcode
 * - \b Master \b demo \b code \b of  \b I2C \b master  \b portable \b layer \b as \b below: \n
 *      @code
 *  #define ENABLE_ERROR_RETRY
 *  #ifdef ENABLE_ERROR_RETRY
 *      #define MAX_ERROR_NUMBER 1000
 *       static volatile uint32_t g_i2c_master_send_error_counter = 0;
 *       static volatile uint32_t g_i2c_master_receive_error_counter = 0;
 *  #endif
 *  #define VG_I2C_SLAVE_CR_CMD 0xaa51
 *  #define VG_I2C_SLAVE_CW_CMD 0xaa53
 *   // The first word[31:16]: for cmd. he first word[15:0]::for reg index  The second word: for reg len
 *  #define VG_I2C_SLAVE_CMD_LEN 8
 *   //matser<--> slave CR or CW data format:
 *   //        Word 0:
 *   //            [31:16]- VG_I2C_SLAVE_CR_CMD or VG_I2C_SLAVE_CW_CMD
 *   //            [15:0]-   VG_I2C_SLAVE_xxx_xxx_REG_OFFSET
 *   //        Word:
 *   //            [31:0] Register len.
 *
 *  #define VG_I2C_SLAVE_TX_LEN_REG_OFFSET 0x08
 *  #define VG_I2C_SLAVE_TX_BUF_REG_OFFSET 0x2000
 *
 *  #define VG_I2C_SLAVE_RX_LEN_REG_OFFSET 0x04
 *  #define VG_I2C_SLAVE_RX_BUF_REG_OFFSET 0x1000
 *
 *  #define DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW 0x50//(0x30)//
 *  #define DEMO_I2C_SLAVE_ADDRESS_R        0x54//(0xa1)//
 *  #define DEMO_I2C_SLAVE_ADDRESS_W        0x58//(0xa2)//
 *
 *
 *
 *
 *
 *   static volatile int32_t send_irq_err_flag = 0;
 *   static volatile int32_t receive_irq_err_flag = 0;
 *
 *   static SemaphoreHandle_t g_host_mux_i2c_master_mutex = NULL;
 *   static SemaphoreHandle_t g_host_mux_i2c_master_wait_send_done_xSemaphore = NULL;
 *   static SemaphoreHandle_t g_host_mux_i2c_master_wait_receive_done_xSemaphore = NULL;
 *
 *
 *   host_mux_status_t portable_HAL_I2C_MASTER_INIT(host_mux_port_t port)
 *   {
 *       hal_i2c_config_t i2c_config;
 *       hal_i2c_port_t i2c_port;
 *       if(g_host_mux_i2c_master_mutex == NULL)
 *           g_host_mux_i2c_master_mutex = portable_xSemaphoreCreateMutex();
 *       if (g_host_mux_i2c_master_mutex == NULL) {
 *           printf( "g_host_mux_i2c_master_mutex create error\r\n");
 *           return HOST_MUX_STATUS_ERROR_INIT;
 *       }
 *       if(g_host_mux_i2c_master_wait_send_done_xSemaphore == NULL)
 *           g_host_mux_i2c_master_wait_send_done_xSemaphore = portable_xSemaphoreCreateBinary();
 *       if (g_host_mux_i2c_master_wait_send_done_xSemaphore == NULL) {
 *           printf( "g_host_mux_i2c_master_wait_send_done_xSemaphore create error\r\n");
 *           return HOST_MUX_STATUS_ERROR_INIT;
 *       }
 *       if(g_host_mux_i2c_master_wait_receive_done_xSemaphore == NULL)
 *           g_host_mux_i2c_master_wait_receive_done_xSemaphore = portable_xSemaphoreCreateBinary();
 *       if (g_host_mux_i2c_master_wait_receive_done_xSemaphore == NULL) {
 *           printf( "g_host_mux_i2c_master_wait_receive_done_xSemaphore create error\r\n");
 *           return HOST_MUX_STATUS_ERROR_INIT;
 *       }
 *
 *       if(port == HOST_MUX_I2C_MASTER_0)
 *          i2c_port =  HAL_I2C_MASTER_0;
 *       if(port == HOST_MUX_I2C_MASTER_1)
 *          i2c_port =  HAL_I2C_MASTER_1;
 *
 *       i2c_config.frequency = HAL_I2C_FREQUENCY_50K;
 *       if(HAL_I2C_STATUS_OK == hal_i2c_master_init(i2c_port, &i2c_config)){
 *           hal_i2c_master_set_io_config(i2c_port,HAL_I2C_IO_PUSH_PULL);
 *       }
 *       else{
 *           printf("mux_i2c_master_demo_receive: open fail, try next time...\r\n");
 *           {printf("Assert!!!");     while(1);    assert(0);}
 *       }
 *      return HOST_MUX_STATUS_OK;
 *
 *   }
 *   host_mux_status_t portable_HAL_I2C_MASTER_DEINIT(host_mux_port_t port)
 *   {
 *       hal_i2c_port_t i2c_port;
 *       if(port == HOST_MUX_I2C_MASTER_0)
 *         i2c_port =  HAL_I2C_MASTER_0;
 *       if(port == HOST_MUX_I2C_MASTER_1)
 *         i2c_port =  HAL_I2C_MASTER_1;
 *       if(HAL_I2C_STATUS_OK != hal_i2c_master_deinit(i2c_port)){
 *              printf("hal_i2c_master_deinit: deinit fail!!!\r\n");
 *           }
 *       return HOST_MUX_STATUS_OK;
 *   }
 *
 *   static void portable_HOST_MUX_I2C_MASTER_wait_for_send_done()
 *   {
 *      if(portable_xSemaphoreTake( g_host_mux_i2c_master_wait_send_done_xSemaphore, portMAX_DELAY) == pdTRUE)
 *      {
 *      }
 *      else
 *      {
 *          {printf("Assert!!!");     while(1);    assert(0);}
 *      }
 *
 *   }
 *   static void portable_HOST_MUX_I2C_MASTER_wait_for_receive_done()
 *   {
 *      if(portable_xSemaphoreTake( g_host_mux_i2c_master_wait_receive_done_xSemaphore, portMAX_DELAY) == pdTRUE)
 *      {
 *      }
 *      else
 *      {
 *          {printf("Assert!!!");     while(1);    assert(0);}
 *      }
 *   }
 *
 *
 *   static void portable_HOST_MUX_I2C_MASTER_RECEIVE_CB(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data)
 *   {
 *       BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *       uint32_t receive_done_len = (uint32_t)user_data;
 *
 *       if(event == HAL_I2C_EVENT_SUCCESS)
 *       {
 *           receive_irq_err_flag = 1;
 *            //printf("receive done: <%s>\r\n",receive_data_buf);
 *        }
 *       else
 *       {
 *           receive_irq_err_flag = -1;
 *           printf("i2c_user_receive_callback ERROR, err event:%d\r\n",event);
 *       }
 *       portable_xSemaphoreGiveFromISR( g_host_mux_i2c_master_wait_receive_done_xSemaphore, &xHigherPriorityTaskWoken );
 *   }
 *
 *   static void portable_HOST_MUX_I2C_MASTER_SEND_CB(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data)
 *   {
 *       BaseType_t xHigherPriorityTaskWoken = pdFALSE;
 *       uint32_t send_done_len = (uint32_t)user_data;
 *       if(event == HAL_I2C_EVENT_SUCCESS)
 *       {
 *           send_irq_err_flag= 1;
 *       }
 *       else
 *       {
 *           send_irq_err_flag = -1;
 *           printf("i2c_user_send_callback ERROR, err event:%d\r\n",event);
 *       }
 *       portable_xSemaphoreGiveFromISR( g_host_mux_i2c_master_wait_send_done_xSemaphore, &xHigherPriorityTaskWoken );
 *   }
 *
 *    //   This is Master read data from slave:
 *    //   Step0: Init I2C master
 *    //   Step1:Master read Slave the register of <VG_I2C_SLAVE_TX_LEN_REG_OFFSET> with the len of 4
 *    //           Step1_a: Master send Config Read (CR) cmd to slave:
 *    //               Slave address is DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW.
 *    //               Master send to slave two word:{(VG_I2C_SLAVE_CR_CMD<<16)|VG_I2C_SLAVE_TX_LEN_REG_OFFSET,4}
 *    //           Step1_b: Master receive the value of <VG_I2C_SLAVE_TX_LEN_REG_OFFSET>
 *    //              Slave address is DEMO_I2C_SLAVE_ADDRESS_R
 *    //              Master receive 4 bytes, it's the data len which Slave prepared.
 *    //   Step2:Master read Slave the register of <VG_I2C_SLAVE_TX_BUF_REG_OFFSET> with the len of data_buffer_len.
 *    //           Step2_a: Master send Config Read (CR) cmd to slave:
 *    //               Slave address is DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW.
 *    //               Master send to slave two word:{(VG_I2C_SLAVE_CR_CMD<<16)|VG_I2C_SLAVE_TX_BUF_REG_OFFSET,data_buffer_len}
 *    //           Step2_b: Master receive <data_buffer_len> data.
 *    //               Slave address is DEMO_I2C_SLAVE_ADDRESS_R
 *    //               Master receive <data_buffer_len> bytes, it's the data which Slave prepared.
 *    //   Step3: Deinit I2C master
 *
 *   host_mux_status_t portable_HAL_I2C_MASTER_RX(host_mux_port_t mux_port, uint8_t*buf, uint32_t *receive_done_data_len)
 *   {
 *          hal_i2c_receive_config_t receive_config;
 *          hal_i2c_status_t status;
 *          uint32_t request_cmd[2];
 *          uint32_t receive_reg_value = 0;
 *           hal_i2c_port_t port;
 *           if(mux_port == HOST_MUX_I2C_MASTER_0)
 *               port =  HAL_I2C_MASTER_0;
 *           if(mux_port == HOST_MUX_I2C_MASTER_1)
 *               port =  HAL_I2C_MASTER_1;
 *  #ifdef ENABLE_ERROR_RETRY
 *   mux_i2c_master_demo_receive_restart:
 *  #endif
 *          portable_MUTEX_TAKE(&g_host_mux_i2c_master_mutex);
 *       //Step1:Master read Slave the register of <VG_I2C_SLAVE_TX_LEN_REG_OFFSET> with the len of 4
 *     //       Step1_a: Master send Config Read (CR) cmd to slave:
 *     //                  Slave address is DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW.
 *     //                  Master send to slave two word:{(VG_I2C_SLAVE_CR_CMD<<16)|VG_I2C_SLAVE_TX_LEN_REG_OFFSET,4}
 *  #ifdef ENABLE_ERROR_RETRY
 *       g_i2c_master_receive_error_counter = 0;
 *  #endif
 *         request_cmd[0] = (VG_I2C_SLAVE_CR_CMD<<16) | VG_I2C_SLAVE_TX_LEN_REG_OFFSET;
 *         request_cmd[1] = 4;
 *         while(1){
 *           printf("mux_i2c_master_demo_receive-Step1_a: Send VG_I2C_SLAVE_CRD_CMD.\r\n");
 *           printf("mux_i2c_master_demo_receive-Step1_a: Switch I2C Slave address to :0x%x. Master want to send 8B\r\n",DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW);
 *           status = hal_i2c_master_send_polling(port, DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW,(uint8_t *)request_cmd,VG_I2C_SLAVE_CMD_LEN);
 *           if( HAL_I2C_STATUS_OK == status){
 *               printf("mux_i2c_master_demo_receive-Step1_a: Send VG_I2C_SLAVE_CRD_CMD success!!!\r\n");
 *               break;
 *           }
 *            printf("mux_i2c_master_demo_receive-Step1_a: I2C master send err, try again...status:%d \r\n",status);
 *  #ifdef ENABLE_ERROR_RETRY
 *               g_i2c_master_receive_error_counter++;
 *               if(g_i2c_master_receive_error_counter>MAX_ERROR_NUMBER){
 *                   printf("mux_i2c_master_demo_receive-Step1_a: #### too many ERROR counter %d, now go to restart!!!!!\r\n",(int)g_i2c_master_receive_error_counter);
 *                   portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *                  goto mux_i2c_master_demo_receive_restart;
 *               }
 *  #endif
 *         }
 *           //Step1_b: Master receive the value of <VG_I2C_SLAVE_TX_LEN_REG_OFFSET>
 *           //       Slave address is DEMO_I2C_SLAVE_ADDRESS_R
 *           //       Master receive 4 bytes, it's the data len which Slave prepared.
 *  #ifdef ENABLE_ERROR_RETRY
 *          g_i2c_master_receive_error_counter = 0;
 *  #endif
 *          while(1)
 *          {
 *               printf("mux_i2c_master_demo_receive-Step1_b: Receive I2C slave Tx_len Reg value. \r\n");
 *               printf("mux_i2c_master_demo_receive-Step1_b: Switch I2C Slave address to :0x%x.Master want to receive 4B.\r\n",
 *                       DEMO_I2C_SLAVE_ADDRESS_R);
 *               status = hal_i2c_master_receive_polling(port, DEMO_I2C_SLAVE_ADDRESS_R, (uint8_t*)&receive_reg_value, 4);
 *               if( HAL_I2C_STATUS_OK == status){
 *                   printf("mux_i2c_master_demo_receive-Step1_b: Receive I2C slave Tx_len Reg value:0x%x. success!!!\r\n",(unsigned int)receive_reg_value);
 *                   break;
 *               }
 *               printf("mux_i2c_master_demo_receive-Step1_b : I2C master receive err, try again...status:%d\r\n",(int)status);
 *  #ifdef ENABLE_ERROR_RETRY
 *                           g_i2c_master_receive_error_counter++;
 *                           if(g_i2c_master_receive_error_counter>MAX_ERROR_NUMBER){
 *                               printf("mux_i2c_master_demo_receive-Step1_b: #### too many ERROR counter %d, now go to restart!!!!!\r\n",(int)g_i2c_master_receive_error_counter);
 *                               portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *                              goto mux_i2c_master_demo_receive_restart;
 *                           }
 *  #endif
 *          }
 *          if(receive_reg_value > *receive_done_data_len){
 *               receive_reg_value = *receive_done_data_len;
 *               printf("mux_i2c_master_demo_receive-Step1_b: slave data len too big, request %d B firstly;\r\n",(int)*receive_done_data_len);
 *          }
 *          if(receive_reg_value == 0){//// receive len should not 0
 *               portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *               *receive_done_data_len = 0;
 *               return HOST_MUX_STATUS_OK;
 *          }
 *          portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *
 *
 *       portable_MUTEX_TAKE(&g_host_mux_i2c_master_mutex);
 *
 *       //Step2:Master read Slave the register of <VG_I2C_SLAVE_TX_BUF_REG_OFFSET> with the len of data_buffer_len.
 *       //        Step2_a: Master send Config Read (CR) cmd to slave:
 *       //            Slave address is DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW.
 *       //            Master send to slave two word:{(VG_I2C_SLAVE_CR_CMD<<16)|VG_I2C_SLAVE_TX_BUF_REG_OFFSET,data_buffer_len}
 *  #ifdef ENABLE_ERROR_RETRY
 *              g_i2c_master_receive_error_counter = 0;
 *  #endif
 *          request_cmd[0] = (VG_I2C_SLAVE_CR_CMD<<16) | VG_I2C_SLAVE_TX_BUF_REG_OFFSET;
 *          request_cmd[1] = receive_reg_value;
 *          while(1){
 *              printf("mux_i2c_master_demo_receive-Step2_a: send VG_I2C_SLAVE_RD_CMD.\r\n");
 *              printf("mux_i2c_master_demo_receive-Step2_a: Switch I2C Slave address to :0x%x.Master want to send 8B cmd.\r\n",DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW);
 *              status = hal_i2c_master_send_polling(port, DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW,(uint8_t *)request_cmd,VG_I2C_SLAVE_CMD_LEN);
 *              if( HAL_I2C_STATUS_OK == status){
 *                printf("mux_i2c_master_demo_receive-Step2_a: Send VG_I2C_SLAVE_RD_CMD. success!!!\r\n");
 *                break;
 *              }
 *              printf("mux_i2c_master_demo_receive-Step2_a: I2C master send err, try again...status:%d \r\n",status);
 *  #ifdef ENABLE_ERROR_RETRY
 *              g_i2c_master_receive_error_counter++;
 *              if(g_i2c_master_receive_error_counter>MAX_ERROR_NUMBER){
 *                  printf("mux_i2c_master_demo_receive-Step2_a: #### too many ERROR counter %d, now go to restart!!!!!\r\n",(int)g_i2c_master_receive_error_counter);
 *                  portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *                 goto mux_i2c_master_demo_receive_restart;
 *              }
 *  #endif
 *
 *          }
 *           //Step2_b: Master receive <data_buffer_len> data.
 *           //            Slave address is DEMO_I2C_SLAVE_ADDRESS_R
 *           //            Master receive <data_buffer_len> bytes, it's the data which Slave prepared.
 *  #ifdef ENABLE_ERROR_RETRY
 *          g_i2c_master_receive_error_counter = 0;
 *  #endif
 *          hal_i2c_master_register_callback(port, portable_HOST_MUX_I2C_MASTER_RECEIVE_CB, (void *)receive_reg_value);//Register a user callback.
 *          receive_config.receive_bytes_in_one_packet = receive_reg_value;
 *          receive_config.receive_buffer = buf;
 *          receive_config.receive_packet_length = 1;
 *          receive_config.slave_address = DEMO_I2C_SLAVE_ADDRESS_R;
 *          printf("mux_i2c_master_demo_receive-Step2_b: Switch I2C Slave address to :0x%x,Master want to receive:%d\r\n",
 *           DEMO_I2C_SLAVE_ADDRESS_R,(int)receive_reg_value);
 *          while(1){
 *              receive_irq_err_flag = 0;
 *              printf("mux_i2c_master_demo_receive-Step2_b: receive Tx_buff data.  \r\n");
 *              status = hal_i2c_master_receive_dma_ex(port, &receive_config);
 *              if (HAL_I2C_STATUS_OK == status) {
 *                   portable_HOST_MUX_I2C_MASTER_wait_for_receive_done();
 *                   if(receive_irq_err_flag == 1){
 *                       printf("mux_i2c_master_demo_receive-Step2_b: Receive Tx_buff data. success\r\n");
 *                       break;
 *                   }
 *              }
 *              else{
 *                printf("mux_i2c_master_demo_receive-Step2_b: hal_i2c_master_receive_dma_ex return ERROR!!! status:%d\r\n",status);
 *                   while(1);
 *                  assert(0);
 *              }
 *              printf("mux_i2c_master_demo_receive-Step2_b: I2C master receive err, try again...\r\n");
 *  #ifdef ENABLE_ERROR_RETRY
 *             g_i2c_master_receive_error_counter++;
 *             if(g_i2c_master_receive_error_counter>MAX_ERROR_NUMBER){
 *                 printf("mux_i2c_master_demo_receive-Step2_b: #### too many ERROR counter %d, now go to restart!!!!!\r\n",(int)g_i2c_master_receive_error_counter);
 *                 portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *                goto mux_i2c_master_demo_receive_restart;
 *             }
 *  #endif
 *
 *        }
 *        portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *        *receive_done_data_len = receive_reg_value;
 *        return HOST_MUX_STATUS_OK;
 *   }
 *
 *
 *   //    This is Master write data to slave:
 *   //    Step0: Init I2C master
 *   //    Step1:Master read Slave the register of <VG_I2C_SLAVE_RX_LEN_REG_OFFSET> with the len of 4
 *   //            Step1_a: Master send Config Read (CR) cmd to slave:
 *   //                Slave address is DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW.
 *   //                Master send to slave two word:{(VG_I2C_SLAVE_CR_CMD<<16)|VG_I2C_SLAVE_RX_LEN_REG_OFFSET,4}
 *   //            Step1_b: Master receive the value of <VG_I2C_SLAVE_RX_LEN_REG_OFFSET>
 *   //               Slave address is DEMO_I2C_SLAVE_ADDRESS_R
 *   //               Master receive 4 bytes, it's the data len which Slave Rx buffer free size.
 *   //    Step2:Master read Slave the register of <VG_I2C_SLAVE_RX_BUF_REG_OFFSET> with the len of data_buffer_len which master want to write.
 *   //            Step2_a: Master send Config Write (CW) cmd to slave:
 *   //                Slave address is DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW.
 *   //                Master send to slave two word:{(VG_I2C_SLAVE_CW_CMD<<16)|VG_I2C_SLAVE_RX_BUF_REG_OFFSET,data_buffer_len}
 *   //            Step2_b: Master send <data_buffer_len> data.
 *   //                Slave address is DEMO_I2C_SLAVE_ADDRESS_W
 *   //                Master send <data_buffer_len> bytes, it's the data which master want to write.
 *   //    Step3: Deinit I2C master
 *
 *   host_mux_status_t portable_HAL_I2C_MASTER_TX(host_mux_port_t mux_port, uint8_t*buf, uint32_t *send_done_data_len)
 *   {
 *          hal_i2c_send_config_t send_config;
 *          hal_i2c_status_t status;
 *          uint32_t request_cmd[2];
 *          uint32_t receive_reg_value;
 *          hal_i2c_port_t port;
 *          if(mux_port == HOST_MUX_I2C_MASTER_0)
 *              port =  HAL_I2C_MASTER_0;
 *          if(mux_port == HOST_MUX_I2C_MASTER_1)
 *              port =  HAL_I2C_MASTER_1;
 *
 *  #ifdef ENABLE_ERROR_RETRY
 *          mux_i2c_master_demo_send_restart:
 *  #endif
 *       portable_MUTEX_TAKE(&g_host_mux_i2c_master_mutex);
 *       //Step1:Master read Slave the register of <VG_I2C_SLAVE_RX_LEN_REG_OFFSET> with the len of 4
 *       //    Step1_a: Master send Config Read (CR) cmd to slave:
 *       //        Slave address is DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW.
 *       //        Master send to slave two word:{(VG_I2C_SLAVE_CR_CMD<<16)|VG_I2C_SLAVE_RX_LEN_REG_OFFSET,4}
 *  #ifdef ENABLE_ERROR_RETRY
 *           g_i2c_master_send_error_counter = 0;
 *  #endif
 *           request_cmd[0] = (VG_I2C_SLAVE_CR_CMD<<16) | VG_I2C_SLAVE_RX_LEN_REG_OFFSET;
 *           request_cmd[1] = 4;
 *           while(1){
 *               printf("mux_i2c_master_demo_send-Step1_a: Send VG_I2C_SLAVE_CRD_CMD.\r\n");
 *               printf("mux_i2c_master_demo_send-Step1_a: Switch I2C Slave address to :0x%x. Master want to send 8B cmd.\r\n",DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW);
 *               status = hal_i2c_master_send_polling(port, DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW,(uint8_t *)request_cmd,VG_I2C_SLAVE_CMD_LEN);
 *           if( HAL_I2C_STATUS_OK == status){
 *               printf("mux_i2c_master_demo_send-Step1_a: Send VG_I2C_SLAVE_CRD_CMD success!!!\r\n");
 *               break;
 *           }
 *               printf("mux_i2c_master_demo_send-Step1_a: I2C master send err, try again...status:%d \r\n",status);
 *  #ifdef ENABLE_ERROR_RETRY
 *             g_i2c_master_receive_error_counter++;
 *             if(g_i2c_master_receive_error_counter>MAX_ERROR_NUMBER){
 *                 printf("mux_i2c_master_demo_send-Step1_a: #### too many ERROR counter %d, now go to restart!!!!!\r\n",(int)g_i2c_master_receive_error_counter);
 *                 portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *                goto mux_i2c_master_demo_send_restart;
 *             }
 *  #endif
 *           }
 *
 *       //Step1_b: Master receive the value of <VG_I2C_SLAVE_RX_LEN_REG_OFFSET>
 *       //           Slave address is DEMO_I2C_SLAVE_ADDRESS_R
 *       //           Master receive 4 bytes, it's the data len which Slave Rx buffer free size.
 *  #ifdef ENABLE_ERROR_RETRY
 *                   g_i2c_master_send_error_counter = 0;
 *  #endif
 *          while(1){
 *               printf("mux_i2c_master_demo_send-Step1_b: receive I2C slave Tx_len Reg value.\r\n");
 *               printf("mux_i2c_master_demo_send-Step1_b: Switch I2C Slave address to :0x%x.Master want to receive 4B.\r\n",DEMO_I2C_SLAVE_ADDRESS_R);
 *               status = hal_i2c_master_receive_polling(port, DEMO_I2C_SLAVE_ADDRESS_R, (uint8_t*)&receive_reg_value, 4);
 *               if( HAL_I2C_STATUS_OK == status){
 *                   printf("mux_i2c_master_demo_send-Step1_b: Receive I2C slave Rx_len Reg value:0x%x. success!!!\r\n",(int)receive_reg_value);
 *                   break;
 *               }
 *               printf("mux_i2c_master_demo_send-Step1_b: I2C master receive err, try again...status:%d\r\n",status);
 *  #ifdef ENABLE_ERROR_RETRY
 *             g_i2c_master_receive_error_counter++;
 *             if(g_i2c_master_receive_error_counter>MAX_ERROR_NUMBER){
 *                 printf("mux_i2c_master_demo_send-Step1_b: #### too many ERROR counter %d, now go to restart!!!!!\r\n",(int)g_i2c_master_receive_error_counter);
 *                 portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *                goto mux_i2c_master_demo_send_restart;
 *             }
 *  #endif
 *
 *          }
 *          if(receive_reg_value > *send_done_data_len){
 *               printf("mux_i2c_master_demo_send-Step1_b: Receive I2C slave Rx_len Reg value:%d, but master just want to send:%d\r\n",(int)receive_reg_value,(int)*send_done_data_len);
 *               receive_reg_value = *send_done_data_len;
 *           }
 *          if(receive_reg_value == 0){
 *               portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *                *send_done_data_len = 0;
 *                   return HOST_MUX_STATUS_OK;
 *          }
 *          portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *
 *          portable_MUTEX_TAKE(&g_host_mux_i2c_master_mutex);
 *       //Step2:Master read Slave the register of <VG_I2C_SLAVE_RX_BUF_REG_OFFSET> with the len of data_buffer_len which master want to write.
 *       //    Step2_a: Master send Config Write (CW) cmd to slave:
 *       //        Slave address is DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW.
 *       //        Master send to slave two word:{(VG_I2C_SLAVE_CW_CMD<<16)|VG_I2C_SLAVE_RX_BUF_REG_OFFSET,data_buffer_len}
 *  #ifdef ENABLE_ERROR_RETRY
 *          g_i2c_master_send_error_counter = 0;
 *  #endif
 *          request_cmd[0] = (VG_I2C_SLAVE_CW_CMD<<16) | VG_I2C_SLAVE_RX_BUF_REG_OFFSET;
 *          request_cmd[1] = receive_reg_value;
 *          while(1){
 *              printf("mux_i2c_master_demo_send-Step2_a: send VG_I2C_SLAVE_WD_CMD.\r\n");
 *              printf("mux_i2c_master_demo_send-Step2_a: Switch I2C Slave address to :0x%x.Master want to send 8B \r\n",DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW);
 *              status = hal_i2c_master_send_polling(port, DEMO_I2C_SLAVE_ADDRESS_CR_OR_CW,(uint8_t *)request_cmd,VG_I2C_SLAVE_CMD_LEN);
 *              if( HAL_I2C_STATUS_OK == status)
 *              {
 *                printf("mux_i2c_master_demo_send-Step2_a: Send VG_I2C_SLAVE_RD_CMD. success!!!\r\n");
 *                break;
 *              }
 *              printf("mux_i2c_master_demo_send-Step2_a: I2C master send err, try again...status:%d \r\n",status);
 *  #ifdef ENABLE_ERROR_RETRY
 *            g_i2c_master_receive_error_counter++;
 *            if(g_i2c_master_receive_error_counter>MAX_ERROR_NUMBER){
 *                printf("mux_i2c_master_demo_send-Step2_a: #### too many ERROR counter %d, now go to restart!!!!!\r\n",(int)g_i2c_master_receive_error_counter);
 *                portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *               goto mux_i2c_master_demo_send_restart;
 *            }
 *  #endif
 *          }
 *       //Step2_b: Master send <data_buffer_len> data.
 *       //        Slave address is DEMO_I2C_SLAVE_ADDRESS_W
 *       //        Master send <data_buffer_len> bytes, it's the data which master want to write.
 *  #ifdef ENABLE_ERROR_RETRY
 *          g_i2c_master_send_error_counter = 0;
 *  #endif
 *          hal_i2c_master_register_callback(port, portable_HOST_MUX_I2C_MASTER_SEND_CB, (void *)receive_reg_value);//Register a user callback.
 *          send_config.send_bytes_in_one_packet = receive_reg_value;
 *          send_config.send_data = buf;
 *          send_config.send_packet_length = 1;
 *          send_config.slave_address = DEMO_I2C_SLAVE_ADDRESS_W;
 *          printf("mux_i2c_master_demo_send-Step2_b: Switch I2C Slave address to :0x%x. Master want to send:%d\r\n",
 *                                       DEMO_I2C_SLAVE_ADDRESS_W,(int)receive_reg_value);
 *          while(1){
 *              //hal_gpt_delay_ms(10);
 *              send_irq_err_flag = 0;
 *              printf("mux_i2c_master_demo_send-Step2_b: master send data. \r\n");
 *              status = hal_i2c_master_send_dma_ex(port, &send_config);
 *              if (HAL_I2C_STATUS_OK == status) {
 *                   portable_HOST_MUX_I2C_MASTER_wait_for_send_done();
 *                   if(send_irq_err_flag == 1){
 *                       printf("mux_i2c_master_demo_send-Step2_b: receive Tx_buff data. success\r\n");
 *                       break;
 *                   }
 *              }
 *              else{
 *                   printf("mux_i2c_master_demo_send-Step2_b: send fail!!!status:%d\r\n",status);
 *                   while(1);
 *                  assert(0);
 *              }
 *              printf("mux_i2c_master_demo_send-Step2_b: I2C master receive err, try again...\r\n");
 *  #ifdef ENABLE_ERROR_RETRY
 *           g_i2c_master_receive_error_counter++;
 *           if(g_i2c_master_receive_error_counter>MAX_ERROR_NUMBER){
 *               printf("mux_i2c_master_demo_send-Step2_a: #### too many ERROR counter %d, now go to restart!!!!!\r\n",(int)g_i2c_master_receive_error_counter);
 *               portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *              goto mux_i2c_master_demo_send_restart;
 *           }
 *  #endif
 *
 *        }
 *        portable_MUTEX_GIVE(&g_host_mux_i2c_master_mutex);
 *        *send_done_data_len = receive_reg_value;
 *        return HOST_MUX_STATUS_OK;
 *   }
 *      @endcode
 * @}
 * @}
 */



/**
* @addtogroup kernel_service
* @{
* @addtogroup MUX
* @{
*/


/** @defgroup mux_macro Macro
 *  @{
 */

/** @brief This macro defines the max MUX user number. */
#ifdef MTK_SINGLE_CPU_ENV
#define MAX_MUX_USER_COUNT 30
#else
#define MAX_MUX_USER_COUNT 12
#endif/* MTK_SINGLE_CPU_ENV */


/** @brief This macro defines the max length MUX user user name. */
#define MAX_USER_NAME    16
/**
 * @}
 */

/** @defgroup mux_enum Enum
 *  @{
 */
/** @brief MUX port type. */
typedef enum {
    MUX_UART_BEGIN,             /**<  port of  MUX_UART_BEGIN*/
    MUX_UART_0 = MUX_UART_BEGIN,/**<  port of  MUX_UART_0*/
    MUX_UART_1,                 /**<  port of  MUX_UART_1*/
    MUX_UART_2,                 /**<  port of  MUX_UART_2*/
    MUX_UART_3,                 /**<  port of  MUX_UART_3*/
    MUX_UART_END = MUX_UART_3,  /**<  port of  MUX_UART_END*/

    MUX_USB_BEGIN,                  /**<  port of  MUX_USB_BEGIN*/
    MUX_USB_COM_1 = MUX_USB_BEGIN,  /**<  port of  MUX_USB_COM_1*/
    MUX_USB_COM_2,                  /**<  port of  MUX_USB_COM_2*/
    MUX_USB_END = MUX_USB_COM_2,    /**<  port of  MUX_USB_END*/

    MUX_I2C_SLAVE_BEGIN,                    /**<  port of  MUX_I2C_SLAVE_BEGIN*/
    MUX_I2C_SLAVE_0 = MUX_I2C_SLAVE_BEGIN,  /**<  port of  MUX_I2C_SLAVE_0*/
    MUX_I2C_SLAVE_END = MUX_I2C_SLAVE_0,    /**<  port of  MUX_I2C_SLAVE_END*/

    MUX_SPI_SLAVE_BEGIN,                    /**<  port of  MUX_SPI_SLAVE_BEGIN*/
    MUX_SPI_SLAVE_0 = MUX_SPI_SLAVE_BEGIN,  /**<  port of  MUX_SPI_SLAVE_0*/
    MUX_SPI_SLAVE_END = MUX_SPI_SLAVE_0,    /**<  port of  MUX_SPI_SLAVE_END*/

    MUX_AIRAPP_BEGIN,                       /**<  port of  MUX_AIRAPP_BEGIN */
    MUX_AIRAPP_0 = MUX_AIRAPP_BEGIN,        /**<  port of  MUX_AIRAPP_0 */
    MUX_AIRAPP_END = MUX_AIRAPP_0,          /**<  port of  MUX_AIRAPP_END */

#if defined(MTK_MUX_BT_ENABLE)
    MUX_BT_BEGIN,                           /**<  port of  MUX_BT_BEGIN*/
    MUX_BT_SPP = MUX_BT_BEGIN,              /**<  port of  MUX_BT_SPP*/
    MUX_BT_BLE,                             /**<  port of  MUX_BT_BLE*/
#if defined(MTK_GATT_OVER_BREDR_ENABLE)
    MUX_BT_GATT_OVER_BREDR,                 /**<  port of  MUX_BT_GATT_OVER_BREDR*/
#endif
    MUX_BT_AIRUPATE,                        /**<  port of  MUX_BT_AIRUPATE*/
    MUX_BT_END = MUX_BT_AIRUPATE,           /**<  port of  MUX_BT_END*/
#endif

#if defined(MTK_MUX_AWS_MCE_ENABLE)
    MUX_AWS_MCE_BEGIN,                      /**<  port of  MUX_AWS_MCE_BEGIN */
    MUX_AWS_MCE = MUX_AWS_MCE_BEGIN,        /**<  port of  MUX_AWS_MCE */
    MUX_AWS_MCE_URGENT,                     /**<  port of  MUX_AWS_MCE_URGENT */
    MUX_AWS_MCE_END = MUX_AWS_MCE_URGENT,   /**<  port of  MUX_AWS_MCE_END */
#endif

#if defined(MTK_IAP2_VIA_MUX_ENABLE)
    MUX_IAP2_BEGIN,                         /**<  port of  MUX_IAP2_BEGIN */
    MUX_IAP2_SESSION1 = MUX_IAP2_BEGIN,     /**<  port of  MUX_IAP2_SESSION1 */
    MUX_IAP2_SESSION2,                      /**<  port of  MUX_IAP2_SESSION2 */
    MUX_IAP2_SESSION3,                      /**<  port of  MUX_IAP2_SESSION3 */
    MUX_IAP2_SESSION4,                      /**<  port of  MUX_IAP2_SESSION4 */
    MUX_IAP2_END = MUX_IAP2_SESSION4,       /**<  port of  MUX_IAP2_END */
#endif

#if defined(AIR_RACE_SCRIPT_ENABLE)
    MUX_PORT_PSEUDO,                        /**<  port of  MUX_PORT_PSEUDO. */
#endif
    MUX_PORT_END,   /**<  port of  MUX_PORT_END*/
} mux_port_t;

/** @brief MUX event type. */
typedef enum {
    MUX_EVENT_READY_TO_READ = 1,    /**<  event of  ready to read*/
    MUX_EVENT_READY_TO_WRITE,       /**<  event of  ready to write*/
    MUX_EVENT_CONNECTION,           /**<  event of  connection, ex. UART,USB and BT*/
    MUX_EVENT_DISCONNECTION,        /**<  event of  disconnection, ex.UART,USB and BT*/
    MUX_EVENT_WAKEUP_FROM_SLEEP,    /**<  event of  wakeup,special for uart now.*/
    MUX_EVENT_TRANSMISSION_DONE,    /**<  event of  transmission done,special for uart now.*/
    MUX_EVENT_TRANSPARENT_READ,     /**<  event of  transparent read data from hardware buffer directly.The restriction is: only applicable when the data is received is a complete package*/
} mux_event_t;

/** @brief MUX status type. */
typedef enum {
    MUX_STATUS_OK,                          /**<  status ok*/
    MUX_STATUS_ERROR,                       /**<  status error*/
    MUX_STATUS_ERROR_PARAMETER,             /**<  status error parameter*/
    MUX_STATUS_ERROR_INIT,                  /**<  status initialization*/
    MUX_STATUS_ERROR_NOT_INIT,              /**<  status uninitialized*/
    MUX_STATUS_ERROR_INITIATED,             /**<  status of have initialized*/
    MUX_STATUS_ERROR_NOT_EXIST,             /**<  status error of not exist*/
    MUX_STATUS_ERROR_INIT_FAIL,             /**<  status of initialize fail*/
    MUX_STATUS_ERROR_SOME_USER_STILL_OPEN,  /**<  status of still have some user opening this mux port, can not to close*/
    MUX_STATUS_ERROR_DEINIT_FAIL,           /**<  status of uninitialized fail*/
    MUX_STATUS_ERROR_TOO_MANY_USERS,        /**<  status of  too many users*/
    MUX_STATUS_USER_RX_QUEUE_EMPTY,         /**<  status of  port user rx package queue empty*/
    MUX_STATUS_USER_RX_BUF_SIZE_NOT_ENOUGH, /**<  status of  Rx buffer not enough*/
    MUX_STATUS_USER_TX_BUF_SIZE_NOT_ENOUGH, /**<  status of  Tx buffer not enough*/
    MUX_STATUS_USER_NOT_OPEN,               /**<  status of not open*/
    MUX_STATUS_USER_ERROR_OF_RX_TX_PROTOCOL,/**<  status of error, user Tx and Rx protocol callback error*/
    MUX_STATUS_ERROR_NVDM_INIT_FAIL,        /**<  status of NVDM initialized fail*/
    MUX_STATUS_ERROR_PORT_SETTING_CHANGED,  /**<  status of port setting changed*/
    MUX_STATUS_ERROR_NVDM_NOT_FOUND,        /**<  status of NVDM not found*/
} mux_status_t;

/** @brief MUX uart flow control type. */
typedef enum {
    MUX_UART_NONE_FLOWCONTROL,  /**<  MUX uart none flow control*/
    MUX_UART_SW_FLOWCONTROL,    /**<  MUX uart SW flow control*/
    MUX_UART_HW_FLOWCONTROL,    /**<  MUX uart HW flow control*/
} mux_uart_fc_t;
/**
 * @}
 */

/** @defgroup mux_struct struct
 *  @{
 */

/** @brief MUX handle type.\n
    bit[16:31]:magic number.\n
    bit[8:15]:user_id.\n
    bit[0:7]:mux port.\n
    */
typedef uint32_t mux_handle_t;

/** @brief MUX buffer type. */
typedef struct {
    uint8_t *p_buf;     /**<  MUX buffer start address*/
    uint32_t buf_size;  /**<  MUX buffer lenght*/
} mux_buffer_t;

/** @brief MUX port setting type. */
typedef struct {
    uint32_t portLinkRegAddr;   /**<  MUX init caller linker address*/
    uint32_t tx_buffer_size;    /**<  MUX tx buffer size*/
    uint32_t rx_buffer_size;    /**<  MUX rx buffer size*/
#ifdef HAL_UART_MODULE_ENABLED
    union {
        struct {
            hal_uart_config_t uart_config;      /**<  MUX uart config*/
            mux_uart_fc_t flowcontrol_type;     /**<  MUX uart flow control setting*/
        } uart;
#endif
        struct {
        } usb;
#ifdef HAL_I2C_SLAVE_MODULE_ENABLED
        struct { /**<  For I2C slave data accuracy, support difference slave address.*/
            uint8_t cr_or_cw_slave_address;    /**<  MUX I2C slave config read or config write slave address, should use slave address when master wants to config read or write .*/
            uint8_t r_slave_address;           /**<  MUX I2C slave read slave address, should use slave address when master wants to read.*/
            uint8_t w_slave_address;           /**<  MUX I2C slave write address, should use slave address when master wants to write.*/
        } i2c_slave;
#endif
#ifdef HAL_SPI_SLAVE_MODULE_ENABLED
        struct {
            hal_spi_slave_config_t slave_config;    /**<  MUX SPI slave config*/
        } spi_slave;
#endif
    } dev_setting;  /**<  MUX port device setting union*/
} mux_port_setting_t;

/** @brief MUX port buffer type from NVDM */
typedef struct {
    uint32_t count;     /**<  query the count of how many ports opened by a designated user by the function of mux_query_user_port_numbers_from_nvdm() */
    uint8_t buf[MUX_PORT_END];  /**<  all of the ports information of the query*/
} mux_port_buffer_t;

/** @brief MUX port user name buffer type */
typedef struct {
    uint32_t count;    /**< MUX Port Service device. */
    char name[20];     /**< Port User name. */
} mux_port_assign_t;


/** @brief MUX callback function type
    When event == MUX_EVENT_READY_TO_READ, data_len means: there have one package data, len is data_len.\n
    When event == MUX_EVENT_READY_TO_WRITE,data_len means: will be always as 0x0.
*/
typedef void (*mux_callback_t)(mux_handle_t handle, mux_event_t event,uint32_t data_len,void*user_data);

/** @brief MUX protocol Tx callback head buffer max len, user should pay attention to void memory crash when implement the function of tx_protocol_callback() to prepare header buffer .*/
#define TX_PROTOCOL_CB_HEAD_BUFFER_MAX_LEN 32
/** @brief MUX protocol Tx callback tail buffer max len, user should pay attention to void memory crash when implement the function of tx_protocol_callback() to prepare tail buffer.*/
#define TX_PROTOCOL_CB_TAIL_BUFFER_MAX_LEN 32

/** @brief MUX protocol callback type */
typedef struct {
    void (*tx_protocol_callback)(mux_handle_t handle, const mux_buffer_t payload[],uint32_t buffers_counter,
                mux_buffer_t *head, mux_buffer_t *tail,void *user_data); /**<  To do package, *head and *tail are the parameters out.*/
    void (*rx_protocol_callback)(mux_handle_t *handle, mux_buffer_t buffers[],uint32_t buffers_counter,
                uint32_t *consume_len, uint32_t *package_len,void *user_data); /**<  To do unpackage, *consume_len and *package_len are the parameters out.*/
    void *user_data;    /**<  A user data.*/
} mux_protocol_t;

/** @brief
 * This structure indicates current available data received when
 * using #MUX_CMD_GET_RX_AVAIL.
 */
typedef struct {
    uint32_t ret_size;               /**< The size of the received data. */
} mux_get_rx_avail_t;

/** @brief
 * This structure indicates current available space to put the transmit data in when
 * using #MUX_CMD_GET_TX_AVAIL.
 */
typedef struct {
    uint32_t ret_size;               /**< The current available space to put the transmit data in. */
} mux_get_tx_avail_t;

/** @brief
 * This structure indicates connection parameters in when
 * using #MUX_CMD_GET_CONNECTION_PARAM.
 */
typedef struct {
    uint8_t remote_address[6];  /**< IAP2 remote address. */
    uint16_t max_packet_size;   /**< IAP2 MAX PACKET SIZE. */
    uint16_t iap2_session_id[3]; /**< IAP2 Session ID list. */
    uint8_t iap2_session_num;    /**< IAP2 Session number */
} mux_get_connection_param_t;

/** @brief
 * This structure indicates configure parameters in when
 * using #MUX_CMD_SET_RX_PARAM.
 */
typedef struct {
    uint8_t is_rx_need_session_id: 1;  /**< IAP2 is rx need session id . */
    uint8_t reserved: 7;               /**< Reserved . */
} mux_set_config_param_t;

/** @brief
 * This structure indicates configure parameters in when
 * using #MUX_CMD_GET_TX_SEND_STATUS or #MUX_CMD_GET_RX_RECEIVED_STATUS.
 */
typedef struct {
    uint16_t tx_send_status;           /**< The user can set a value to indicate the status of tx transmission. */
    uint16_t rx_receive_status;        /**< The user can set a value to indicate the status of rx transmission. */
    uint32_t transfer_completed_size;  /**< Indicate the data size for successfully completed transfer. */
} mux_get_trx_status_t;

/** @brief
 * This enum defines the commands for each parameter in #mux_ctrl_para_t.
 */
typedef enum {
    MUX_CMD_GET_TX_AVAIL,               /**< UART or USB or BT special command to get the current available space to store the transmit data. */
    MUX_CMD_GET_RX_AVAIL,               /**< UART or USB or BT special command to get the current available size for the received data. */
    MUX_CMD_GET_TX_BUFFER_STATUS,       /**< UART or USB or BT special command to get the current available space to store the transmit data. */
    MUX_CMD_GET_RX_BUFFER_STATUS,       /**< UART or USB or BT special command to get the current available size for the received data. */
    MUX_CMD_CONNECT,                    /**< special command to do connect. */
    MUX_CMD_DISCONNECT,                 /**< special command to do disconnect. */
    MUX_CMD_GET_CONNECTION_PARAM,       /**< iAP2 special command to get connection parameters. */
    MUX_CMD_SET_RX_PARAM ,              /**< iAP2 special command to set rx paramete. */
    MUX_CMD_CLEAN,                      /**< special command to clean tx/rx buffer data. */
    MUX_CMD_GET_VIRTUAL_TX_AVAIL_LEN,   /**< special command to get tx buffer avail data length. */
    MUX_CMD_GET_VIRTUAL_RX_AVAIL_LEN,   /**< special command to get rx buffer avail data length. */
    MUX_CMD_CLEAN_TX_VIRUTUAL,          /**< special command to clean tx buffer data. */
    MUX_CMD_CLEAN_RX_VIRUTUAL,          /**< special command to clean rx buffer data. */
    MUX_TX_BUFFER_SEND,                 /**< special command to send tx buffer data. */
    MUX_CMD_GET_TX_SEND_STATUS,         /**< User can get TX status. Including: The status whether the underlying hardware successfully sent data, and what is the data size of the successfully sent.*/
    MUX_CMD_GET_RX_RECEIVED_STATUS,     /**< User can get TX status. Including: The status whether the underlying hardware successfully received data, and what is the data size of the successfully received.*/
    MUX_CMD_MAX,                        /**< The maximum number of commands. */
} mux_ctrl_cmd_t;

/** @brief
 * This structure defines the data structure for each command in
 * #mux_ctrl_cmd_t.
 */
typedef union {
    mux_get_tx_avail_t mux_get_tx_avail;      /**< Parameters for port tx buffer free space.  */
    mux_get_rx_avail_t mux_get_rx_avail;      /**< Parameters for port rx buffer free space.  */
    uint32_t mux_virtual_tx_avail_len;        /**< Parameters for port tx buffer available data length.  */
    uint32_t mux_virtual_rx_avail_len;        /**< Parameters for port rx buffer available data length.  */
    mux_get_connection_param_t mux_get_connection_param; /**< Parameters for iAP2 special get connection parameters command. */
    mux_set_config_param_t mux_set_config_param;    /**< Parameters for iAP2 special set configuration parameters command. */
    mux_get_trx_status_t mux_get_trx_status;        /**< Parameters for user to get sening or receiving status and The length of the data that has been sent or received.*/
} mux_ctrl_para_t;


/**
 * @}
 */

/** @brief This function is mux port initialize.
 * @param[in] port is the mux port.
 * @param[in] setting is mux port setting.
 * @param[in] protocol_callback is protocol callback function when mux to do send and receive data.
 * @return
 *         MUX_STATUS_OK, MUX port initialize success.
 * @code
 * @endcode
*/
mux_status_t mux_init(mux_port_t port, mux_port_setting_t *setting, mux_protocol_t *protocol_callback);


/** @brief This function is mux port deinitialize.
 * @param[in] port is the mux port.
 * @return
 *         MUX_STATUS_OK, MUX port deinitialize success.
 * @code
 * @endcode
*/
mux_status_t mux_deinit(mux_port_t port);


/** @brief This function is logging port exception initialize, special for mux port as logging port.
 * @param[in] handle is the mux logging user handle.
 * @return
 *         MUX_STATUS_OK, MUX logging port exception initialize success.
 * @code
 * @endcode
*/
mux_status_t mux_exception_init(mux_handle_t handle);


/** @brief This function is logging port send exception data, special for mux port as logging port.
 * @param[in] handle is the mux logging user handle.
 * @param[in] buffers is buffer list want to send.
  * @param[in] buffers_counter is buffer count.
 * @return
 *         MUX_STATUS_OK, MUX exception send data success.
 * @code
 * @endcode
*/
mux_status_t mux_exception_send(mux_handle_t handle, const mux_buffer_t *buffers,uint32_t buffers_counter);


/** @brief This function is mux port open by user.
 * @param[in] port is the mux port.
 * @param[in] user_name is the user name.
 * @param[out] p_handle is handle, user can get the handle by mux_open.
 * @param[in] callback is the callback function for ready to read and read to write.
 * @param[in] user_data the user data, as a parameter of callback.
 * @return
 *         MUX_STATUS_OK, MUX port open success.
 * @code
 * @endcode
*/
mux_status_t mux_open(mux_port_t port, const char *user_name, mux_handle_t *p_handle,mux_callback_t callback,void*user_data);


/** @brief This function is mux port close by user.
 * @param[in] handle is the user handle.
 * @return
 *         MUX_STATUS_OK, MUX port close success.
 * @code
 * @endcode
*/
mux_status_t mux_close(mux_handle_t handle);


/** @brief This function is mux receive data.
 * @param[in] handle is the mux user handle.
 * @param[in] buffer is receive buffer.
 * @param[out] receive_done_data_len is how many data received.
 * @return
 *         MUX_STATUS_OK, MUX receive data success.
 * @code
 * @endcode
*/
mux_status_t mux_rx(mux_handle_t handle, mux_buffer_t *buffer, uint32_t *receive_done_data_len);


/** @brief This function is query the user's name.
 * @param[in] handle is the mux user handle.
 * @param[out] user_name is the user's name.
 * @return
 *         MUX_STATUS_OK, Query user name success.
 * @code
 * @endcode
*/
mux_status_t mux_query_user_name(mux_handle_t handle, const char **user_name);


/** @brief This function is query the user's handle.
 * @param[in] port is the mux port.
 * @param[in] user_name is the user's user_name.
 * @param[out] p_handle is the user's handle.
 * @return
 *         MUX_STATUS_OK, Query user handle success.
 * @code
 * @endcode
*/
mux_status_t mux_query_user_handle(mux_port_t port, const char *user_name, mux_handle_t *p_handle);


/** @brief This function queries how many users are on the specified port.
 * @param[in] port is the specified port.
 * @param[out] user_count is the user quantity.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_user_number(mux_port_t port, uint32_t *user_count);


/** @brief This function queries user specified port name list.
 * @param[in] port is the user specified port.
 * @param[out] user_name is the port users name.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_user_name(mux_port_t port, mux_port_assign_t *user_name);


/** @brief This function is mux send data.
 * @param[in] handle is the mux user handle.
 * @param[in] buffers[] is send buffer list.
 * @param[in] buffers_counter is the count of buffer list which want to send.
 * @param[out] send_done_data_len is how many data sent.
 * @return
 *         MUX_STATUS_OK, MUX send success.
 * @code
 * @endcode
*/
mux_status_t mux_tx(mux_handle_t handle, const mux_buffer_t buffers[],uint32_t buffers_counter,uint32_t *send_done_data_len);


/** @brief This function is mux control operate.
 * @param[in] port is the mux port.
 * @param[in] command is the command want to do.
 * @param[out] para is the parameter which want to return from mux_control.
 * @return
 *         MUX_STATUS_OK, MUX operate success.
 * @code
 * @endcode
*/
mux_status_t mux_control(mux_port_t port, mux_ctrl_cmd_t command, mux_ctrl_para_t *para);


/** @brief This function saves the user's port information to NVDM.
 * @param[in] port is the mux user port.
 * @param[in] user_name is the user's name.
 * @return
 *         MUX_STATUS_OK, MUX save user port success.
 * @code
 * @endcode
*/
mux_status_t mux_open_save_to_nvdm(mux_port_t port, const char *user_name);


/** @brief This function deletes the user's port information from NVDM.
 * @param[in] port is the mux user port.
 * @param[in] user_name is the user's name.
 * @return
 *         MUX_STATUS_OK, MUX delete user port success.
 * @code
 * @endcode
*/
mux_status_t mux_close_delete_from_nvdm(mux_port_t port, const char *user_name);


/** @brief This function reads the port setting from NVDM.
 * @param[in] port is the mux user port.
 * @param[out] setting is the port's configuration.
 * @return
 *         MUX_STATUS_OK, MUX query port setting success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_setting_from_nvdm(mux_port_t port, mux_port_setting_t *setting);


/** @brief This function save the port setting to NVDM.
 * @param[in] port is the mux user port.
 * @param[in] setting is the port's configuration.
 * @return
 *         MUX_STATUS_OK, MUX save port setting success.
 * @code
 * @endcode
*/
mux_status_t mux_save_port_setting_to_nvdm(mux_port_t port, mux_port_setting_t *setting);


/** @brief This function queries how many users are on the specified port from NVDM.
 * @param[in] port is the specified port.
 * @param[out] user_count is the user quantity.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_user_number_form_nvdm(mux_port_t port, uint32_t *user_count);


/** @brief This function queries user specified port name list from NVDM.
 * @param[in] port is the user specified port.
 * @param[out] user_name is the port users name.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_user_name_from_nvdm(mux_port_t port, mux_port_assign_t *user_name);


/** @brief This function queries how many ports the user had opened from NVDM.
 * @param[in] user_name is the user's name.
 * @param[out] port_count is the used port quantity.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_user_port_numbers_from_nvdm(const char *user_name, uint32_t *port_count);


/** @brief This function query the used port index by the user from NVDM.
 * @param[in] user_name is the user's name.
 * @param[out] port is a point, it returns the used ports index.
 * @return
 *         MUX_STATUS_OK, MUX query parameter success.
 * @code
 * @endcode
*/
mux_status_t mux_query_port_numbers_from_nvdm(const char *user_name, mux_port_buffer_t *port);

/**
* @}
* @}
*/


#ifdef __cplusplus
}
#endif


#endif//__MUX_H__

