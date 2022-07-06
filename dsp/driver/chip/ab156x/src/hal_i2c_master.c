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

/* Includes ------------------------------------------------------------------*/
#include "hal_i2c_master.h"
#include "hal_platform.h"
#if defined(HAL_I2C_MASTER_MODULE_ENABLED)

#include "hal_i2c_master_internal.h"
#include "hal_pdma_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_log.h"
#include <assert.h>
#include "string.h"
#include "memory_attribute.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif
#include "hal_gpt.h"

static void hal_i2c_master_isr(hal_nvic_irq_t irq_number);
void        hal_i2c_isr_handler(hal_i2c_port_t i2c_port, hal_i2c_callback_t user_callback, void *user_data);
static void hal_i2c_master_dma_callback(pdma_event_t event, void  *user_data);
#ifdef I2C_ENABLE_POLLING_TIMEOUT
static void hal_i2c_master_wait_transaction_finish(hal_i2c_port_t i2c_port, i2c_transaction_type_t transaction_type);
static i2c_transaction_status_t  hal_i2c_wait_dma_finish(hal_i2c_port_t i2c_port);
#endif
static volatile  uint8_t s_i2c_master_status[HAL_I2C_MASTER_MAX] = {0};
static hal_i2c_callback_t s_i2c_callback[HAL_I2C_MASTER_MAX] = {NULL};
static void *s_i2c_callback_parameter[HAL_I2C_MASTER_MAX] = {NULL};

#ifdef HAL_I2C_MASTER_FRATURE_SW_FIFO
static uint8_t  s_i2c_sw_fifo_init_flag[HAL_I2C_MASTER_MAX] = {0};
#endif

static const  hal_nvic_irq_t s_i2c_irq_table[HAL_I2C_MASTER_MAX] = {HAL_I2C_IRQ_TABLE};
extern uint32_t g_i2c_source_clock_table[HAL_I2C_MASTER_MAX];

static volatile i2c_dma_callback_parameter_t s_i2c_dma_parameter[HAL_I2C_MASTER_MAX];
static volatile uint8_t i2c_dma_callback_count[HAL_I2C_MASTER_MAX] = {0, 0, 0, 0};

//static hal_i2c_callback_t  current_i2c_callback = NULL;
//static void *current_user_data = NULL;
//static hal_i2c_port_t current_i2c_port;

extern I2C_REGISTER_T *i2c_master_register[];
extern pdma_channel_t g_i2c_tx_dma_channel[];
extern pdma_channel_t g_i2c_rx_dma_channel[];

#ifdef HAL_SLEEP_MANAGER_ENABLED
static volatile i2c_register_backup_t i2c_register_backup[HAL_I2C_MASTER_MAX];
/* Note: there is no SLEEP_LOCK_I2C_AO enum, so use SLEEP_LOCK_I2C2 for I2C_AO port.
 * BTW: I2C_AO port only support polling mode, so the system will not enter sleep when I2C_AO port is in using.
 */
static sleep_management_lock_request_t i2c_lock_sleep_handle[HAL_I2C_MASTER_MAX] = {SLEEP_LOCK_I2C0, SLEEP_LOCK_I2C1, SLEEP_LOCK_I2C2, SLEEP_LOCK_I2C2};

#define I2C_LOCK_SLEEP(i2c_port) \
do{ \
    hal_sleep_manager_lock_sleep(i2c_lock_sleep_handle[i2c_port]); \
}while(0)

#define I2C_UNLOCK_SLEEP(i2c_port) \
do{ \
    hal_sleep_manager_unlock_sleep(i2c_lock_sleep_handle[i2c_port]); \
}while(0)

#else
#define I2C_LOCK_SLEEP(i2c_port)
#define I2C_UNLOCK_SLEEP(i2c_port)
#endif

#ifdef HAL_DVFS_MODULE_ENABLED
#define I2C_DVFS_UPDATE_CLOCK()
#define I2C_LOCK_DVFS()
#define I2C_UNLOCK_DVFS()
#else /* HAL_DVFS_MODULE_ENABLED */
#define I2C_DVFS_UPDATE_CLOCK()
#define I2C_LOCK_DVFS()
#define I2C_UNLOCK_DVFS()
#endif
/*****************************************************************************
* Function
*****************************************************************************/
void hal_i2c_backup_all_register(void);
void hal_i2c_restore_all_register(void);


hal_i2c_status_t hal_i2c_master_init(hal_i2c_port_t i2c_port, hal_i2c_config_t *i2c_config)
{
    hal_i2c_status_t busy_status;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

    if (HAL_I2C_FREQUENCY_MAX <= i2c_config->frequency) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 1);
    I2C_CHECK_AND_SET_BUSY(i2c_port, busy_status);
    if (HAL_I2C_STATUS_ERROR_BUSY == busy_status) {
        log_hal_error("[I2C%d][init]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

#ifdef HAL_I2C_MASTER_FRATURE_SW_FIFO
    if (s_i2c_sw_fifo_init_flag[i2c_port] == 0) {
        i2c_sw_fifo_init(i2c_port);
        s_i2c_sw_fifo_init_flag[i2c_port] = 1;
    }
#endif

#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_I2C, (sleep_management_suspend_callback_t)hal_i2c_backup_all_register, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_I2C, (sleep_management_resume_callback_t)hal_i2c_restore_all_register, NULL);
#endif

    i2c_clock_control(i2c_port, true);
    //hal_clock_enable(g_i2c_pdn_table[i2c_port]);

    /* i2c lock dvfs */
    I2C_LOCK_DVFS();
    /*update source clock*/
    I2C_DVFS_UPDATE_CLOCK();

    /* change i2c port to open-drain mode */
    i2c_set_io_config(i2c_port, I2C_IO_OPEN_DRAIN);

    /* calculate speed to register value */
    i2c_set_frequency(i2c_port, i2c_config->frequency);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_deinit(hal_i2c_port_t i2c_port)
{
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

#ifdef HAL_I2C_MASTER_FRATURE_SW_FIFO
    hal_i2c_sw_fifo_node_t transfer_config;
    hal_i2c_send_to_receive_config_ex_no_busy_t send_to_receive_config;
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* Get how many transfer cnt in fifo */
    uint32_t i2c_sw_fifo_cnt = i2c_get_sw_fifo_available_transfer_cnt(i2c_port);
    if (i2c_sw_fifo_cnt != 0) {
        /* set to idle */
        I2C_SET_IDLE(i2c_port);

        /* pop data from i2c_sw_fifo */
        i2c_pop_sw_fifo(i2c_port, &transfer_config, 1);
        send_to_receive_config.i2c_callback = transfer_config.i2c_callback;
        send_to_receive_config.user_data = transfer_config.user_data;
        send_to_receive_config.i2c_config = transfer_config.i2c_config;
        send_to_receive_config.i2c_send_to_receive_config_ex = transfer_config.i2c_send_to_receive_config_ex;

        /* transfer mode is dma ex */
        hal_i2c_master_send_to_receive_dma_ex_none_blocking(i2c_port, &send_to_receive_config);

        hal_nvic_restore_interrupt_mask(saved_mask);

        return HAL_I2C_STATUS_OK;
    }

    hal_nvic_restore_interrupt_mask(saved_mask);

#endif
    I2C_DEBUG_RECORD_FLOW(i2c_port, 2);
    /* disable I2C */
    I2C_STOP(i2c_port);

    if (HAL_I2C_MASTER_AO > i2c_port)
    {
        /* disable IRQ */
        hal_nvic_disable_irq(s_i2c_irq_table[i2c_port]);

        /*mask interrupt*/
        I2C_MASK_INTERRUPT(i2c_port);
    }

    /*clear callback*/
    s_i2c_callback[i2c_port] = NULL;

    /* set dma false */
    s_i2c_dma_parameter[i2c_port].dma_finish = false;

    i2c_clock_control(i2c_port, false);
    //hal_clock_disable(g_i2c_pdn_table[i2c_port]);

    /*unlock dvfs*/
    I2C_UNLOCK_DVFS();
    /* set to idle */
    I2C_SET_IDLE(i2c_port);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_set_frequency(hal_i2c_port_t i2c_port, hal_i2c_frequency_t frequency)
{
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (HAL_I2C_FREQUENCY_MAX <= frequency) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][set_freq]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 3);
    /* calculate speed to register value */
    i2c_set_frequency(i2c_port, frequency);

    return HAL_I2C_STATUS_OK;
}

#ifdef HAL_I2C_MASTER_FEATURE_CONFIG_IO
hal_i2c_status_t hal_i2c_master_set_io_config(hal_i2c_port_t i2c_port, hal_i2c_io_config_t io_config)
{
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if ((HAL_I2C_IO_OPEN_DRAIN != io_config) &&
        (HAL_I2C_IO_PUSH_PULL != io_config))
    {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][set_io_config]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    i2c_set_io_config(i2c_port, io_config);

    return HAL_I2C_STATUS_OK;
}
#endif /* HAL_I2C_MASTER_FEATURE_CONFIG_IO */

hal_i2c_status_t hal_i2c_master_register_callback(hal_i2c_port_t i2c_port, hal_i2c_callback_t i2c_callback, void *user_data)
{
    /* parameter check */
    /* 155x I2C AO Port does not support DMA mode */
    if (HAL_I2C_MASTER_AO <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_callback) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 4);
    s_i2c_callback[i2c_port] = i2c_callback;
    s_i2c_callback_parameter[i2c_port] = user_data;

    /* register NVIC */
    hal_nvic_register_isr_handler(s_i2c_irq_table[i2c_port], hal_i2c_master_isr);
    hal_nvic_enable_irq(s_i2c_irq_table[i2c_port]);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_send_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size)
{
    i2c_transaction_status_t transaction_status;
    i2c_transaction_size_t config_size;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == data || size > I2C_FIFO_DEPTH) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send_polling]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 5);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /*mask interrupt*/
    I2C_MASK_INTERRUPT(i2c_port);

    /*config i2c hardware*/
    config_size.send_size = size;
    config_size.receive_size = 0;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING, I2C_GET_SEND_ADDRESS(slave_address), &config_size);

    /*set data to fifo*/
    i2c_write_fifo(i2c_port, data, size);

    /*start i2c*/
    I2C_START(i2c_port);
#ifndef I2C_ENABLE_POLLING_TIMEOUT
    /* polling till transaction finish */
    while (0 == I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port)) {
    }
#else
    hal_i2c_master_wait_transaction_finish(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING);
#endif

    /* read and status */
    transaction_status = i2c_get_transaction_status(i2c_port);

    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);


    if (I2C_TRANSACTION_SUCCESS != transaction_status) {
        log_hal_error("[I2C%d][send_polling]:status=%d\r\n", i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    } else {
        return HAL_I2C_STATUS_OK;
    }
}


hal_i2c_status_t hal_i2c_master_send_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    /* 155x I2C AO Port does not support DMA mode */
    if (HAL_I2C_MASTER_AO <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == data) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send_dma]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 6);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /* config dma hardware */
    pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = size;
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);


    /*config i2c hardware*/
    config_size.send_size = size;
    config_size.receive_size = 0;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA, I2C_GET_SEND_ADDRESS(slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start DMA */
    pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)data);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_receive_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t *buffer, uint32_t size)
{
    i2c_transaction_status_t transaction_status;
    i2c_transaction_size_t config_size;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == buffer || size > I2C_FIFO_DEPTH) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][receive_polling]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }
    I2C_DEBUG_RECORD_FLOW(i2c_port, 7);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /*mask interrupt*/
    I2C_MASK_INTERRUPT(i2c_port);

    /*config i2c hardware*/
    config_size.send_size = 0;
    config_size.receive_size = size;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING, I2C_GET_RECEIVE_ADDRESS(slave_address), &config_size);

    /*start i2c*/
    I2C_START(i2c_port);

#ifndef I2C_ENABLE_POLLING_TIMEOUT
    /* polling till transaction finish */
    while (0 == I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port)) {
    }
#else
    hal_i2c_master_wait_transaction_finish(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING);
#endif

    /* read and status */
    transaction_status = i2c_get_transaction_status(i2c_port);
    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);

    if (I2C_TRANSACTION_SUCCESS != transaction_status) {
        log_hal_error("[I2C%d][receive_polling]:status=%d\r\n", i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    } else {
        i2c_read_fifo(i2c_port, buffer, size);
        return HAL_I2C_STATUS_OK;
    }
}


hal_i2c_status_t hal_i2c_master_receive_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t  *buffer, uint32_t size)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    /* 155x I2C AO Port does not support DMA mode */
    if (HAL_I2C_MASTER_AO <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][receive_dma]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 8);
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /* config dma hardware */
    pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = size;
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);


    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_rx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /*config i2c hardware*/
    config_size.send_size = 0;
    config_size.receive_size = size;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA, I2C_GET_RECEIVE_ADDRESS(slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start DMA */
    pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)buffer);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}



#ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE
hal_i2c_status_t hal_i2c_master_send_to_receive_polling(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_t *i2c_send_to_receive_config)
{
    i2c_transaction_status_t transaction_status;
    i2c_transaction_size_t config_size;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_to_receive_config->send_data || NULL == i2c_send_to_receive_config->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (i2c_send_to_receive_config->send_length > I2C_FIFO_DEPTH || i2c_send_to_receive_config->receive_length > I2C_FIFO_DEPTH) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }


    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send2receive_polling]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 9);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /*mask interrupt*/
    I2C_MASK_INTERRUPT(i2c_port);

    /*config i2c hardware*/
    config_size.send_size = i2c_send_to_receive_config->send_length;
    config_size.receive_size = i2c_send_to_receive_config->receive_length;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING, I2C_GET_SEND_ADDRESS(i2c_send_to_receive_config->slave_address), &config_size);

    /*set data to fifo*/
    i2c_write_fifo(i2c_port, i2c_send_to_receive_config->send_data, i2c_send_to_receive_config->send_length);

    /*start i2c*/
    I2C_START(i2c_port);
#ifndef I2C_ENABLE_POLLING_TIMEOUT
    /* polling till transaction finish */
    while (0 == I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port)) {
    }
#else
    hal_i2c_master_wait_transaction_finish(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING);
#endif
    /* read and status */
    transaction_status = i2c_get_transaction_status(i2c_port);
    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);

    if (I2C_TRANSACTION_SUCCESS != transaction_status) {
        log_hal_error("[I2C%d][send2receive_polling]:status=%d\r\n", i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    } else {
        i2c_read_fifo(i2c_port, i2c_send_to_receive_config->receive_buffer, i2c_send_to_receive_config->receive_length);
        return HAL_I2C_STATUS_OK;
    }
}


hal_i2c_status_t hal_i2c_master_send_to_receive_dma(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_t *i2c_send_to_receive_config)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    /* 155x I2C AO Port does not support DMA mode */
    if (HAL_I2C_MASTER_AO <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_to_receive_config->send_data || NULL == i2c_send_to_receive_config->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send2receive_dma]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 10);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    i2c_dma_callback_count[i2c_port] = 0;

    /* config dma tx hardware */
    pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = i2c_send_to_receive_config->send_length;
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /* config dma rx hardware */
    pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = i2c_send_to_receive_config->receive_length;
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_rx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /*config i2c hardware*/
    config_size.send_size = i2c_send_to_receive_config->send_length;
    config_size.receive_size = i2c_send_to_receive_config->receive_length;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA, I2C_GET_SEND_ADDRESS(i2c_send_to_receive_config->slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start dma tx */
    pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config->send_data);
    /*start dma rx */
    pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config->receive_buffer);
    /* start I2C */
    I2C_START(i2c_port);


    return HAL_I2C_STATUS_OK;
}
#endif /* #ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE */


#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
hal_i2c_status_t hal_i2c_master_send_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_send_config_t *i2c_send_config)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    /* 155x I2C AO Port does not support DMA mode */
    if (HAL_I2C_MASTER_AO <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_config->send_data) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send_dma_enhanced]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 14);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /* config dma hardware */
    pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = (i2c_send_config->send_packet_length) * (i2c_send_config->send_bytes_in_one_packet);
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);


    /*config i2c hardware*/
    config_size.transfer_length = i2c_send_config->send_bytes_in_one_packet;
    config_size.transaction_length = i2c_send_config->send_packet_length;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND, I2C_GET_SEND_ADDRESS(i2c_send_config->slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start DMA */
    pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_config->send_data);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_receive_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_receive_config_t *i2c_receive_config)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    /* 155x I2C AO Port does not support DMA mode */
    if (HAL_I2C_MASTER_AO <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }

    if (NULL == i2c_receive_config->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][receive_dma_enhanced]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 15);
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /* config dma hardware */
    pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = (i2c_receive_config->receive_packet_length) * (i2c_receive_config->receive_bytes_in_one_packet);
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);


    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_rx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /*config i2c hardware*/
    config_size.transfer_length = i2c_receive_config->receive_bytes_in_one_packet;
    config_size.transaction_length = i2c_receive_config->receive_packet_length;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND, I2C_GET_RECEIVE_ADDRESS(i2c_receive_config->slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start DMA */
    pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_receive_config->receive_buffer);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_send_to_receive_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_ex_t *i2c_send_to_receive_config_ex)
{
    i2c_transaction_size_t config_size;
    pdma_config_t  i2c_dma_config;
    /* parameter check */
    /* 155x I2C AO Port does not support DMA mode */
    if (HAL_I2C_MASTER_AO <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_to_receive_config_ex->send_data || NULL == i2c_send_to_receive_config_ex->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_error("[I2C%d][send2receive_dma_enhanced]:busy\r\n", i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 16);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    i2c_dma_callback_count[i2c_port] = 0;

    /* config dma tx hardware */
    pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = i2c_send_to_receive_config_ex->send_bytes_in_one_packet;
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /* config dma rx hardware */
    pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = (i2c_send_to_receive_config_ex->receive_packet_length) * (i2c_send_to_receive_config_ex->receive_bytes_in_one_packet);
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_rx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /*config i2c hardware*/
    config_size.transfer_length = i2c_send_to_receive_config_ex->send_bytes_in_one_packet;
    config_size.transfer_length_aux = i2c_send_to_receive_config_ex->receive_bytes_in_one_packet;
    config_size.transaction_length = i2c_send_to_receive_config_ex->receive_packet_length + 1;
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND, I2C_GET_SEND_ADDRESS(i2c_send_to_receive_config_ex->slave_address), &config_size);
    /* unmask i2c interrupt */
    I2C_UNMASK_INTERRUPT(i2c_port);

    /* start dma tx */
    pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config_ex->send_data);
    /*start dma rx */
    pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config_ex->receive_buffer);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}
#endif /* #ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA */

#ifdef HAL_I2C_MASTER_FRATURE_SW_FIFO
void sw_fifo_handler(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data)
{

    hal_i2c_callback_t old_i2c_callback = current_i2c_callback;
    void *old_user_data = current_user_data;

    hal_i2c_master_deinit(current_i2c_port);

    /* if sw fifo not empty,when call hal_i2c_master_deinit api,
      * it will recover current_i2c_callback,so we need to use old_i2c_callback*/
    (*old_i2c_callback)(slave_address, event, old_user_data);

}

hal_i2c_status_t hal_i2c_master_send_to_receive_dma_ex_none_blocking(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_ex_no_busy_t *i2c_send_to_receive_config_no_busy_ex)
{
    uint32_t saved_mask;
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);

    /* s_i2c_master_status indicate  iic status , while fifo not empty and first user callback call this api,even though 
      * s_i2c_master_status idle,it should be pushed in fifo.
      */
    if ((s_i2c_master_status[i2c_port] == I2C_BUSY) || (0 != i2c_get_sw_fifo_available_transfer_cnt(i2c_port))) {

        hal_i2c_sw_fifo_node_t i2c_sw_fifo_node;

        i2c_sw_fifo_node.i2c_config.frequency = i2c_send_to_receive_config_no_busy_ex->i2c_config.frequency;
        i2c_sw_fifo_node.i2c_port = i2c_port;
        i2c_sw_fifo_node.i2c_callback = i2c_send_to_receive_config_no_busy_ex->i2c_callback;
        i2c_sw_fifo_node.user_data =  i2c_send_to_receive_config_no_busy_ex->user_data;
        i2c_sw_fifo_node.i2c_send_to_receive_config_ex = i2c_send_to_receive_config_no_busy_ex->i2c_send_to_receive_config_ex;

        hal_nvic_restore_interrupt_mask(saved_mask);

        /* fifo is full */
        if(0 == i2c_push_sw_fifo(i2c_port, &i2c_sw_fifo_node, 1)) {           
            return HAL_I2C_STATUS_ERROR;
        }
        else{
            return HAL_I2C_STATUS_OK;
            }
    } else {
        hal_i2c_master_init(i2c_port, &i2c_send_to_receive_config_no_busy_ex->i2c_config);
        hal_nvic_restore_interrupt_mask(saved_mask);
    }

    current_i2c_callback = i2c_send_to_receive_config_no_busy_ex->i2c_callback;
    current_user_data = i2c_send_to_receive_config_no_busy_ex->user_data;
    current_i2c_port = i2c_port;

    hal_i2c_master_register_callback(i2c_port, sw_fifo_handler, NULL);

    hal_i2c_master_send_to_receive_dma_ex(i2c_port, &i2c_send_to_receive_config_no_busy_ex->i2c_send_to_receive_config_ex);

    return HAL_I2C_STATUS_OK;
}
#endif

hal_i2c_status_t hal_i2c_master_get_running_status(hal_i2c_port_t i2c_port, hal_i2c_running_status_t *running_status)
{
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    running_status->running_status = (hal_i2c_running_type_t)I2C_QUERY_RUNNING_STATUS(i2c_port);
    return HAL_I2C_STATUS_OK;
}


static void hal_i2c_master_isr(hal_nvic_irq_t irq_number)
{
    hal_i2c_port_t i2c_port = HAL_I2C_MASTER_MAX;
    uint32_t i = 0;

    for (i = 0; i < HAL_I2C_MASTER_MAX; i++) {
        if (s_i2c_irq_table[i] == irq_number) {
            i2c_port = (hal_i2c_port_t)i;
            break;
        }
    }

    if (HAL_I2C_MASTER_MAX == i2c_port) {
        log_hal_error("[I2C%d][isr]:port=%d\r\n", i2c_port, i2c_port);
        return;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 11);
    hal_i2c_isr_handler(i2c_port, s_i2c_callback[i2c_port], s_i2c_callback_parameter[i2c_port]);

}


void hal_i2c_isr_handler(hal_i2c_port_t i2c_port, hal_i2c_callback_t user_callback, void *user_data)
{
    i2c_transaction_status_t transaction_status;

    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);

    /* read i2c status */
    transaction_status = i2c_get_transaction_status(i2c_port);

    if (I2C_TRANSACTION_SUCCESS != transaction_status) {
        i2c_dma_deinit(i2c_port, false);
        i2c_dma_deinit(i2c_port, true);
        if (NULL != user_callback) {
            user_callback(I2C_GET_SLAVE_ADDRESS(i2c_port), (hal_i2c_callback_event_t)transaction_status, user_data);
        }
        log_hal_msgid_error("[i2c%d][isr]:error status=%d\r\n", 2, i2c_port, transaction_status);
        /* In case of dummy interrupt */
        return;
    }

#ifndef I2C_ENABLE_POLLING_TIMEOUT
    /* wait until dma finish  */
    while (1) {
        if (I2C_IF_DMA_FINISH(i2c_port)) {
            break;
        }
    }
#else
    transaction_status = hal_i2c_wait_dma_finish(i2c_port);
#endif

    /* call user callback */
    if (NULL != user_callback) {
        I2C_DEBUG_RECORD_FLOW(i2c_port, 12);
        user_callback(I2C_GET_SLAVE_ADDRESS(i2c_port), (hal_i2c_callback_event_t)transaction_status, user_data);
    } else {
        log_hal_error("[I2C%d][isr]:NO isr callback\r\n", i2c_port);
    }

}


static void hal_i2c_master_dma_callback(pdma_event_t event, void  *user_data)
{
    i2c_dma_callback_parameter_t *i2c_dma_parameter;

    (void)event;
    i2c_dma_parameter = (i2c_dma_callback_parameter_t *)user_data;
    if (I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA == i2c_dma_parameter->transaction_type ||
            I2C_TRANSACTION_OPEN_DRAIN_SEND_DMA_EXTEND == i2c_dma_parameter->transaction_type) {
        pdma_stop(g_i2c_tx_dma_channel[i2c_dma_parameter->i2c_port]);
        pdma_deinit(g_i2c_tx_dma_channel[i2c_dma_parameter->i2c_port]);
        i2c_dma_parameter->dma_finish = true;
    } else if (I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA == i2c_dma_parameter->transaction_type ||
               I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_DMA_EXTEND == i2c_dma_parameter->transaction_type) {
        pdma_stop(g_i2c_rx_dma_channel[i2c_dma_parameter->i2c_port]);
        pdma_deinit(g_i2c_rx_dma_channel[i2c_dma_parameter->i2c_port]);
        i2c_dma_parameter->dma_finish = true;
    } else if (I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA == i2c_dma_parameter->transaction_type ||
               I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND == i2c_dma_parameter->transaction_type) {
        i2c_dma_callback_count[i2c_dma_parameter->i2c_port]++;
        /* i2c_dma_callback_count is only for hal_i2c_master_send_to_receive_dma
           when use this function, two DMA interrupt will come:Tx and Rx. Only the second
           DMA interrupt(Rx) means the transaction complete,so we don't change dma_finish to true
           until the sencond interrupt comes.
        */
        if (I2C_DMA_FINISH_CALLBACK_COUNT == i2c_dma_callback_count[i2c_dma_parameter->i2c_port]) {
            pdma_stop(g_i2c_tx_dma_channel[i2c_dma_parameter->i2c_port]);
            pdma_stop(g_i2c_rx_dma_channel[i2c_dma_parameter->i2c_port]);
            pdma_deinit(g_i2c_tx_dma_channel[i2c_dma_parameter->i2c_port]);
            pdma_deinit(g_i2c_rx_dma_channel[i2c_dma_parameter->i2c_port]);
            i2c_dma_parameter->dma_finish = true;
            i2c_dma_callback_count[i2c_dma_parameter->i2c_port] = 0;
        }
    }
}


#ifdef I2C_ENABLE_POLLING_TIMEOUT
static void hal_i2c_master_wait_transaction_finish(hal_i2c_port_t i2c_port, i2c_transaction_type_t transaction_type)
{
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
    /* polling till transaction finish */
    while (1) {
        uint32_t saved_mask;
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (0 != I2C_QUERY_TRANSACTION_FINISH_STATUS(i2c_port)) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            break;
        }
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
        hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
        if (I2C_POLLING_TIMEOUT_VALUE < gpt_duration_count) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            log_hal_error("[I2C%d][type=%d]:timeout!\r\n", i2c_port, transaction_type);
            //assert(0);
            break;
        }
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
}


static i2c_transaction_status_t  hal_i2c_wait_dma_finish(hal_i2c_port_t i2c_port)
{
    uint32_t gpt_start_count, gpt_current_count, gpt_duration_count;
    i2c_transaction_status_t  status =  I2C_TRANSACTION_SUCCESS;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_start_count);
    /* polling till dma transaction finish */
    while (1) {
        uint32_t saved_mask;
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (I2C_IF_DMA_FINISH(i2c_port)) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            break;
        }
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &gpt_current_count);
        hal_gpt_get_duration_count(gpt_start_count, gpt_current_count, &gpt_duration_count);
        if (I2C_INTERRUPT_TIMEOUT_VALUE < gpt_duration_count) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            log_hal_error("[I2C%d][isr]:dma timeout!\r\n", i2c_port);
            I2C_DEBUG_RECORD_FLOW(i2c_port, 13);
            //assert(0);
            hal_i2c_master_dma_callback(PDMA_EVENT_TRANSACTION_SUCCESS, (void *)&s_i2c_dma_parameter[i2c_port]);
            if ((I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA == s_i2c_dma_parameter[i2c_port].transaction_type ||
                    I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND == s_i2c_dma_parameter[i2c_port].transaction_type) && (i2c_dma_callback_count[i2c_port] == 1)) {
                hal_i2c_master_dma_callback(PDMA_EVENT_TRANSACTION_SUCCESS, (void *)&s_i2c_dma_parameter[i2c_port]);
            }
            status = I2C_TRANSACTION_TIMEOUT_ERROR;
            break;
        }
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
    return status;
}
#endif /* #ifdef I2C_ENABLE_POLLING_TIMEOUT */


#ifdef HAL_SLEEP_MANAGER_ENABLED
void hal_i2c_backup_all_register(void)
{
    uint32_t i = 0;

    for (i = 0; i < HAL_I2C_MASTER_MAX; i++) {
        if (s_i2c_master_status[i] == I2C_BUSY) {
            i2c_register_backup[i].clock_div = i2c_master_register[i]->CLOCK_DIV;
            i2c_register_backup[i].timing = i2c_master_register[i]->TIMING_UNION.TIMING;
            i2c_register_backup[i].io = i2c_master_register[i]->IO_CONFIG;
            i2c_register_backup[i].control2 = i2c_master_register[i]->CONTROL2_UNION.CONTROL2;
        }
    }
}


void hal_i2c_restore_all_register(void)
{
    uint32_t i = 0;

    for (i = 0; i < HAL_I2C_MASTER_MAX; i++) {
        if (s_i2c_master_status[i] == I2C_BUSY) {
            i2c_master_register[i]->CLOCK_DIV = i2c_register_backup[i].clock_div;
            i2c_master_register[i]->TIMING_UNION.TIMING = i2c_register_backup[i].timing;
            i2c_master_register[i]->IO_CONFIG = i2c_register_backup[i].io;
            i2c_master_register[i]->CONTROL2_UNION.CONTROL2 = i2c_register_backup[i].control2;
        }
    }
}
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */


#endif /* #if defined(HAL_I2C_MASTER_MODULE_ENABLED) */

