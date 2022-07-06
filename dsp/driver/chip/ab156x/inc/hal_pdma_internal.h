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

#ifndef __HAL_PDMA_INTERNAL_H__
#define __HAL_PDMA_INTERNAL_H__

/**
 * @ingroup HAL
 * @addtogroup Peripheral DMA
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/***************************************************************************/
/************************PDMA function start line********************************/
/***************************************************************************/

/** @brief peripheral dma channel */
typedef enum {
    PDMA_START_CHANNEL = 0,
    PDMA_I2C0_TX = PDMA_START_CHANNEL,
    PDMA_I2C0_RX,
    PDMA_I2C1_TX,
    PDMA_I2C1_RX,
    PDMA_I2C2_TX,
    PDMA_I2C2_RX,
    PDMA_END_CHANNEL
} pdma_channel_t;

/** @brief peripheral dma transfer size format */
typedef enum {
    PDMA_BYTE = 0,        /**< peripheral dma transfer size -byte format */
    PDMA_HALF_WORD,       /**<peripheral  dma transfer size -half word format */
    PDMA_WORD             /**<peripheral  dma transfer size -word  format */
} pdma_transfer_size_format_t;

/** @brief peripheral  dma transaction error */
typedef enum {
    PDMA_EVENT_TRANSACTION_ERROR = -1,         /**<peripheral  dma transaction error */
    PDMA_EVENT_TRANSACTION_SUCCESS = 0         /**<peripheral dma transaction success */
} pdma_event_t;

/** @brief peripheral dma status */
typedef enum {
    PDMA_ERROR               = -3,        /**<peripheral  dma function EEROR */
    PDMA_ERROR_CHANNEL       = -2,        /**< peripheral dma error channel */
    PDMA_INVALID_PARAMETER   = -1,        /**<peripheral  dma error invalid parameter */
    PDMA_OK                  = 0          /**<peripheral  dma function OK*/
} pdma_status_t;

/** @brief pdma running status */
typedef enum {
    PDMA_IDLE = 0,                         /**< pdma idle */
    PDMA_BUSY                              /**< pdma busy */
} pdma_running_status_t;

/** @brief pdma buffer transfer type */
typedef enum {
    PDMA_TX = 0,
    PDMA_RX,
    PDMA_TX_RINGBUFF,
    PDMA_RX_RINGBUFF
} pdma_master_type_t;

/** @brief  peripheral dma transaction mode */
typedef struct {
    bool                             burst_mode;               /**< burst mode = 0 ==> single mode */
    pdma_transfer_size_format_t      size;                     /**<transfer size type , which is valiad when burst mode enabled*/
    uint32_t                         count;                    /**<count is beat number, which is valiad when burst mode enabled,max is 0x0FFF */
    pdma_master_type_t               master_type;              /**< master is TX/RX/ring buffer TX/ring buffer RX ,should select one*/
} pdma_config_t;

/** @brief peripheral  dma callback typedef */
typedef void (*pdma_callback_t)(pdma_event_t event, void  *user_data);

/**
 * @brief       init peripheral dma base config
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @return if OK ,returns PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @par       pdma_config_t *pdma_config
 * @code      pdma_init(PDMA_I2C1_TX,&pdma_config);
 * @endcode
 */
pdma_status_t pdma_init(pdma_channel_t channel);

/**
 * @brief       reset peripheral dma register and reset state
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @return if OK ,returns PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @code      pdma_deinit(PDMA_I2C1_TX);
 * @endcode
 */
pdma_status_t pdma_deinit(pdma_channel_t channel);

/**
 * @brief        peripheral dma set ring buffer function if need
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] wppt,the transter count required to perform before the jump point,which should be less than count value in COUNT register
 * @param[in] wpto,the wrap to address, address will wrap to wpto address when jump to wppt address
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @code      pdma_configure(PDMA_I2C1_TX,wwpt,wpto);
 * @endcode
 */
pdma_status_t pdma_set_ring_buffer(pdma_channel_t channel, uint32_t wppt, uint32_t wpto);

/**
 * @brief     peripheral dma all configuration
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] config sructure pointer,dma_config
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par         PDMA_I2C1_TX,pdma_config_t *pdma_config
 * @par         pdma_config->size        = DMA_WORD;
 * @par         pdma_config->burst_mode= TRUE;
 * @par          pdma_config->count  = 4;
 * @code         pdma_configure(PDMA_I2C1_TX,&pdma_config);
 * @endcode
 */
pdma_status_t pdma_configure(pdma_channel_t channel, pdma_config_t *pdma_config);

/**
 * @brief     dma work at polling mode ,
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] address
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @code      pdma_start_polling(PDMA_I2C1_TX,address);
 * @endcode
 */
pdma_status_t pdma_start_polling(pdma_channel_t channel, uint32_t address);

/**
 * @brief     dma work at interrupt mode ,user can register callback when dma interrupt happened
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] address
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @code      pdma_start_interrupt(PDMA_I2C1_TX,address,data_length);
 * @endcode
 */
pdma_status_t pdma_start_interrupt(pdma_channel_t channel, uint32_t address);

/**
 * @brief     stop dma work
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @code      pdma_stop(PDMA_I2C1_TX);
 * @endcode
 */
pdma_status_t pdma_stop(pdma_channel_t channel);

/**
 * @brief     peripheral dma register callback,
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] function_callback,function pointer
 * @param[in] parameter,user's parameter
 * @return if set correctly ,return PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @par       void function_callback(void *parameter)
 * @code      pdma_register_callback(PDMA_I2C1_TX,callback,&user_data);
 * @endcode
 */
pdma_status_t pdma_register_callback(pdma_channel_t channel, pdma_callback_t callback, void *user_data);

/**
 * @brief     get dma  current state
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[out] running_status ,get current pdma status-->DMA_BUSY or DMA_IDLE
 * @return if  get status  successfully ,return PDMA_OK
 * @note
 * @warning
 * @par       PDMA_I2C1_TX
 * @code      pdma_get_running_status(PDMA_I2C1_TX,&running_status);
 * @endcode
 */
pdma_status_t pdma_get_running_status(pdma_channel_t channel, pdma_running_status_t *running_status);

/* Dump content of all PDMA registers for the specific channel */
pdma_status_t pdma_dump_reg_list(pdma_channel_t channel);

/***************************************************************************/
/************************VDMA function start line********************************/
/***************************************************************************/


/** @brief peripheral dma channel */
typedef enum {
    VDMA_START_CHANNEL = 0,
    VDMA_UART0TX = VDMA_START_CHANNEL,
    VDMA_UART0RX,
    VDMA_UART1TX,
    VDMA_UART1RX,
    VDMA_UART2TX,
    VDMA_UART2RX,
    VDMA_I2S0TX,
    VDMA_I2S0RX,
    VDMA_I2S1TX,
    VDMA_I2S1RX,
    VDMA_I2S2TX,
    VDMA_I2S2RX,
    VDMA_I2S3TX,
    VDMA_I2S3RX,
    VDMA_I2S4TX,
    VDMA_I2S4RX,
    VDMA_I2S5TX,
    VDMA_I2S5RX,
    VDMA_I2S6TX,
    VDMA_I2S6RX,
    VDMA_I2S7TX,
    VDMA_I2S7RX,
    VDMA_END_CHANNEL,
} vdma_channel_t;

/** @brief virtual fifo dma status */
typedef enum {
    VDMA_ERROR               = -3,        /**<virtual fifo dma function EEROR */
    VDMA_ERROR_CHANNEL       = -2,        /**<virtual fifo dma error channel */
    VDMA_INVALID_PARAMETER   = -1,        /**< virtual fifo dma error invalid parameter */
    VDMA_OK                  = 0          /**< virtual fifo dma function OK*/
} vdma_status_t;

/** @brief virtual fifo dma  running status */
typedef enum {
    VDMA_IDLE = 0,                         /**< vdma idle */
    VDMA_BUSY                              /**< vdma busy */
} vdma_running_status_t;

/** @brief virtual fifo  dma transaction error */
typedef enum {
    VDMA_EVENT_TRANSACTION_ERROR = -1,         /**<virtual fifo  dma transaction error */
    VDMA_EVENT_TRANSACTION_SUCCESS = 0         /**<virtual fifo  dma transaction success */
} vdma_event_t;

/** @brief  virtual  dma transaction mode */
typedef struct {
    uint32_t          base_address;                /**< the  virtual fifo  start address */
    uint32_t          size;                        /**< the  virtual fifo  size */
} vdma_config_t;

/** @brief virtual dma callback typedef */
typedef void (*vdma_callback_t)(vdma_event_t event, void  *user_data);

/**
 * @brief       init virtual fifo dma init
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @return if OK ,returns VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @par       pdma_config_t *pdma_config
 * @code      pdma_init(VDMA_UART1TX);
 * @endcode
 */
vdma_status_t vdma_init(vdma_channel_t channel);

/**
 * @brief     reset peripheral dma register and reset state
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @return if OK ,returns VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      pdma_deinit(VDMA_UART1TX);
 * @endcode
 */
vdma_status_t vdma_deinit(vdma_channel_t channel);

/**
 * @brief     Start the VDMA channel transfer
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @return if OK ,returns VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_start(VDMA_UART1TX);
 * @endcode
 */
vdma_status_t vdma_start(vdma_channel_t channel);

/**
 * @brief     Stop the VDMA channel transfer
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @return if OK ,returns VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_stop(VDMA_UART1TX);
 * @endcode
 */
vdma_status_t vdma_stop(vdma_channel_t channel);

/**
 * @brief     peripheral dma all configuration
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @param[in] config sructure pointer,vdma_config
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX,vdma_config_t  *vdma_config
 * @par           vdma_config->base_address  = vfifo_start_address;
 * @par       pdma_config->size= 1024;
 * @code      vdma_configure(VDMA_UART1TX,vdma_config);
 * @endcode
 */
vdma_status_t vdma_configure(vdma_channel_t channel, vdma_config_t *vdma_config);

/**
 * @brief     enable vfifo dma interrupt
 * @param[in] channel,dma master name definition in enum vdma_channel_t
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_enable_interrupt(VDMA_UART1TX,address,data_length);
 * @endcode
 */
vdma_status_t vdma_enable_interrupt(vdma_channel_t channel);

/**
 * @brief     disable vfifo dma interrupt  ,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_disable_interrupt(VDMA_UART1TX);
 * @endcode
 */
vdma_status_t vdma_disable_interrupt(vdma_channel_t channel);

/**
 * @brief     get the vfifo dma interrupt status,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *is_enabled ,return the vfifo dma interrupt status
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_get_interrupt_status(VDMA_UART1TX, &is_enabled);
 * @endcode
 */
vdma_status_t vdma_get_interrupt_status(vdma_channel_t channel, uint8_t *is_enabled);

/**
 * @brief     set vfifo dma  threshold ,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[in] threshold
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_set_threshold(VDMA_UART1TX,1024);
 * @endcode
 */
vdma_status_t vdma_set_threshold(vdma_channel_t channel, uint32_t threshold);

/**
 * @brief     set vfifo dma  alert length,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[in] alert_length,range 0~31
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_set_alert_length(VDMA_UART1TX,16);
 * @endcode
 */
vdma_status_t vdma_set_alert_length(vdma_channel_t channel, uint32_t alert_length);

/**
 * @brief     push data to virtual fifo,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[in] data
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_push_data(VDMA_UART1TX,data);
 * @endcode
 */
vdma_status_t vdma_push_data(vdma_channel_t channel, uint8_t data);
vdma_status_t vdma_push_data_4bytes(vdma_channel_t channel, uint32_t data);

/**
 * @brief    Push data to virtual fifo, user should call
 *           vdma_get_available_send_space() firstly to get the
 *           available bytes for sending.
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[in] data
 * @param[in] size, data size in byte unit that user wants to
 *       push to VFIFO.
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @endcode
 */
vdma_status_t vdma_push_data_multi_bytes(vdma_channel_t channel, uint8_t *data, uint32_t size);

/**
 * @brief     pop data from virtual fifo,
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *data,get single  byte from virtual fifo and put in data
 * @return if pop data correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_pop_data(VDMA_UART1TX,&data);
 * @endcode
 */
vdma_status_t vdma_pop_data(vdma_channel_t channel, uint8_t *data);
vdma_status_t vdma_pop_data_4bytes(vdma_channel_t channel, uint32_t *data);

/**
 * @brief    Pop data from virtual fifo, user should call
 *           vdma_get_available_receive_bytes() firstly to get
 *           the data bytes available for reading from VFIFO.
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *data, get multiple bytes from virtual fifo and
 *       put in data buffer.
 * @param[in] size, data size in byte unit that shows the
 *       available data in virtual fifo.
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @endcode
 */
vdma_status_t vdma_pop_data_multi_bytes(vdma_channel_t channel, uint8_t *data, uint32_t size);

/**
 * @brief    get the number of data stored in virtual fifo
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *receive_bytes,get available received bytes from virtual fifo and put in receive_bytes,this is for RX VFIFO
 * @return if get data correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_get_available_room(VDMA_UART1TX,&receive_bytes);
 * @endcode
 */
vdma_status_t vdma_get_available_receive_bytes(vdma_channel_t channel, uint32_t *receive_bytes);

/**
 * @brief    get the left room of   virtual fifo
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *available_space ,get available space in virtual fifo and put in  available_space,this is for TX VFIFO
 * @return if get data correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_get_available_send_space(VDMA_UART1TX,&available_space);
 * @endcode
 */
vdma_status_t vdma_get_available_send_space(vdma_channel_t channel, uint32_t *available_space);

/**
 * @brief    get the the read pointer of   virtual fifo
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *read_point ,get the read point in virtual fifo RDPTR register
 * @return if get the read point correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_get_hw_read_point(VDMA_UART1TX,&read_point);
 * @endcode
 */
vdma_status_t vdma_get_hw_read_point(vdma_channel_t channel, uint32_t *read_point);

/**
 * @brief    set the write point in  virtual fifo
 * @param[in] channel, dma master name definition in enum  vdma_channel_t
 * @param[in] sw_move_byte, set the write point in  virtual fifo SW_MV_BYTE register
 * @return if set the sw_move_byte correctly , return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_set_sw_move_byte(VDMA_UART1TX,sw_move_byte);
 * @endcode
 */
vdma_status_t vdma_set_sw_move_byte(vdma_channel_t channel, uint16_t sw_move_byte);


/**
 * @brief    get the the write pointer of virtual fifo
 * @param[in] channel,dma master name definition in enum  vdma_channel_t
 * @param[out] *write_point ,get the write point in virtual fifo WRPTR register
 * @return if get the read point correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @code      vdma_get_hw_read_point(VDMA_UART1TX,&write_point);
 * @endcode
 */
vdma_status_t vdma_get_hw_write_point(vdma_channel_t channel, uint32_t *write_point);

/**
 * @brief     virtual dma register callback,
 * @param[in] channel,dma master name definition in enum pdma_channel_t
 * @param[in] function_callback,function pointer
 * @param[in] parameter,user's parameter
 * @return if set correctly ,return VDMA_OK
 * @note
 * @warning
 * @par       VDMA_UART1TX
 * @par       void function_callback(void *parameter)
 * @code      vdma_register_callback(VDMA_UART1TX,function_callback,parameter);
 * @endcode
 */
vdma_status_t vdma_register_callback(vdma_channel_t channel, vdma_callback_t callback, void *user_data);

/* Dump content of all VDMA registers for the specific channel */
vdma_status_t vdma_dump_reg_list(vdma_channel_t channel);

#ifdef __cplusplus
}
#endif

/**@ }*/

#endif /* __HAL_PDMA_INTERNAL_H__ */

