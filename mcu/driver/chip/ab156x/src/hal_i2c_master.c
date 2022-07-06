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
#if defined(HAL_I2C_MASTER_MODULE_ENABLED)

#include "hal.h"
#include "hal_i2c_master_internal.h"
#include "hal_pdma_internal.h"
#include <assert.h>
#include "string.h"
#include "memory_attribute.h"
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif

#ifdef __EXT_BOOTLOADER__
#define I2C_TEST_POLLING_CODE(en, id, subid)
#else
#define I2C_TEST_POLLING_CODE(en, id, subid)
#endif



#define     I2C_CHECK_PDMA_STATUS_ENABLED

/*Polling mode timeout*/
#ifdef I2C_ENABLE_POLLING_TIMEOUT
static i2c_transaction_status_t  hal_i2c_wait_dma_finish(hal_i2c_port_t i2c_port);
#endif

/*Extend DMA & NONE_BLOCKING enable*/
#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
typedef struct {
    void *current_user_data;
    hal_i2c_port_t current_i2c_port;
} i2c_master_non_blocking_user_data_t;

#define NB_STATUS_OF_IDLE 0
#define NB_STATUS_OF_OCCUPY_BY_OLD_USER 1
#define NB_STATUS_OF_RUNING 2
#define NB_STATUS_OF_ONE_NOTE_DONE 3

static volatile uint32_t    i2c_sw_fifo_status[HAL_I2C_MASTER_MAX] = {NB_STATUS_OF_IDLE};
static uint8_t              s_i2c_sw_fifo_init_flag[HAL_I2C_MASTER_MAX] = {0};
static hal_i2c_callback_t   g_i2c_m_nb_current_user_cb[HAL_I2C_MASTER_MAX] = {NULL};
static i2c_master_non_blocking_user_data_t g_i2c_m_nb_current_user_data[HAL_I2C_MASTER_MAX];
#endif
//static uint32_t                            s_i2c_polling_time_stamp_for_debug[HAL_I2C_MASTER_MAX] = {0};
static uint8_t                             s_i2c_debug_step_record[HAL_I2C_MASTER_MAX] = {0};

#define I2C_DEBUG_RECORD_STEP(i2c_port, x)      s_i2c_debug_step_record[i2c_port] = x
/*Sleep supported*/
#ifdef HAL_SLEEP_MANAGER_ENABLED
ATTR_ZIDATA_IN_TCM static volatile i2c_register_backup_t s_i2c_register_backup[HAL_I2C_MASTER_MAX];
static  sleep_management_lock_request_t                  s_i2c_lock_sleep_handle[HAL_I2C_MASTER_MAX] = {SLEEP_LOCK_I2C0, SLEEP_LOCK_I2C1, SLEEP_LOCK_I2C2, SLEEP_LOCK_INVALID_ID};

#define I2C_LOCK_SLEEP(i2c_port) \
do{ \
    if(s_i2c_lock_sleep_handle[i2c_port] != SLEEP_LOCK_INVALID_ID)  \
        hal_sleep_manager_lock_sleep(s_i2c_lock_sleep_handle[i2c_port]); \
}while(0)

#define I2C_UNLOCK_SLEEP(i2c_port) \
do{ \
    if(s_i2c_lock_sleep_handle[i2c_port] != SLEEP_LOCK_INVALID_ID)  \
        hal_sleep_manager_unlock_sleep(s_i2c_lock_sleep_handle[i2c_port]); \
}while(0)
#else
#define I2C_LOCK_SLEEP(i2c_port)
#define I2C_UNLOCK_SLEEP(i2c_port)
#endif

/*DVFS supported*/
#ifdef HAL_DVFS_MODULE_ENABLED
#define I2C_DVFS_UPDATE_CLOCK()
#define I2C_LOCK_DVFS()
#define I2C_UNLOCK_DVFS()
#else /* HAL_DVFS_MODULE_ENABLED */
#define I2C_DVFS_UPDATE_CLOCK()
#define I2C_LOCK_DVFS()
#define I2C_UNLOCK_DVFS()
#endif

/*Check PDMA Return value*/
#ifdef I2C_CHECK_PDMA_STATUS_ENABLED
#define     I2C_CHECK_PDMA_STATUS(status)       \
    do {                                        \
        if(PDMA_OK != status){                  \
            return HAL_I2C_STATUS_ERROR;        \
        }                                       \
    }while(0)
#else
#define     I2C_CHECK_PDMA_STATUS(status)  (void)status
#endif

/*Check PDMA Return value*/
#ifdef I2C_CHECK_PDMA_STATUS_ENABLED
#define     I2C_CHECK_PDMA_STATUS(status)       \
    do {                                        \
        if(PDMA_OK != status){                  \
            return HAL_I2C_STATUS_ERROR;        \
        }                                       \
    }while(0)
#else
#define     I2C_CHECK_PDMA_STATUS(status)  (void)status
#endif

#define     HAL_I2C_POLLING_TIMEOUT_US      (50 * 1000) //50ms

/*****************************************************************************
* Declare
*****************************************************************************/
static void hal_i2c_master_isr(hal_nvic_irq_t irq_number);
void        hal_i2c_isr_handler(hal_i2c_port_t i2c_port, hal_i2c_callback_t user_callback, void *user_data);
static void hal_i2c_master_dma_callback(pdma_event_t event, void  *user_data);
void        hal_i2c_backup_all_register(void);
void        hal_i2c_restore_all_register(void);
/*****************************************************************************
* Vairable
*****************************************************************************/
static i2c_dma_callback_parameter_t s_i2c_dma_parameter[HAL_I2C_MASTER_MAX];
static const  hal_nvic_irq_t    s_i2c_irq_table[HAL_I2C_MASTER_MAX] = {HAL_I2C_IRQ_TABLE};
extern uint32_t                 g_i2c_source_clock_table[];
static volatile  uint8_t        i2c_dma_callback_count[HAL_I2C_MASTER_MAX] = {0, 0, 0, 0};
extern I2C_REGISTER_T          *i2c_master_register[];
extern pdma_channel_t           g_i2c_tx_dma_channel[];
extern pdma_channel_t           g_i2c_rx_dma_channel[];
extern hal_clock_cg_id          g_i2c_pdn_table[];
static volatile  uint8_t        s_i2c_master_status[HAL_I2C_MASTER_MAX] = {0};
static hal_i2c_callback_t       s_i2c_callback[HAL_I2C_MASTER_MAX] = {NULL};
static void                    *s_i2c_callback_parameter[HAL_I2C_MASTER_MAX] = {NULL};
static i2c_transfer_info_t      i2c_transfer_info[HAL_I2C_MASTER_MAX];
/*****************************************************************************
* Function
*****************************************************************************/
int     i2c_wait_idle_with_timeout(hal_i2c_port_t i2c_port, uint32_t timeout)
{
    uint32_t    pre_tick = 0, cur_tick = 0, exp_tick = 0;

    hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &pre_tick);
    while (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        hal_gpt_get_free_run_count(HAL_GPT_CLOCK_SOURCE_1M, &cur_tick);
        hal_gpt_get_duration_count(pre_tick, cur_tick, &exp_tick);
        if (timeout != 0 && exp_tick > timeout) {
            return -1;
        }
    }
    return 0;
}


static hal_i2c_status_t hal_i2c_master_init_internal(hal_i2c_port_t i2c_port, hal_i2c_config_t *i2c_config)
{
    hal_i2c_status_t busy_status;

    memset(&i2c_transfer_info[i2c_port], 0, sizeof(i2c_transfer_info_t));
    I2C_DEBUG_RECORD_FLOW(i2c_port, 1);
    I2C_CHECK_AND_SET_BUSY(i2c_port, busy_status);
    if (HAL_I2C_STATUS_ERROR_BUSY == busy_status) {
        log_hal_msgid_error("[I2C%d][init]:busy\r\n", 1, i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_I2C, (sleep_management_suspend_callback_t)hal_i2c_backup_all_register, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_I2C, (sleep_management_resume_callback_t)hal_i2c_restore_all_register, NULL);
#endif

    hal_clock_enable(g_i2c_pdn_table[i2c_port]);

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

hal_i2c_status_t hal_i2c_master_init(hal_i2c_port_t i2c_port, hal_i2c_config_t *i2c_config)
{
    uint32_t saved_mask;
    hal_i2c_status_t status;

    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (HAL_I2C_FREQUENCY_MAX <= i2c_config->frequency) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (i2c_sw_fifo_status[i2c_port] != NB_STATUS_OF_IDLE) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return HAL_I2C_STATUS_BUSY_BY_NON_BLOCKING_FUNCTION;
    }
    i2c_sw_fifo_status[i2c_port] = NB_STATUS_OF_OCCUPY_BY_OLD_USER;
    hal_nvic_restore_interrupt_mask(saved_mask);
#endif
    status = hal_i2c_master_init_internal(i2c_port, i2c_config);
    if (HAL_I2C_STATUS_OK != status) {
        i2c_sw_fifo_status[i2c_port] = NB_STATUS_OF_IDLE;
    }
    return status;

}

static hal_i2c_status_t hal_i2c_master_deinit_internal(hal_i2c_port_t i2c_port)
{

    I2C_DEBUG_RECORD_FLOW(i2c_port, 2);
    /* disable I2C */
    I2C_STOP(i2c_port);

    /* disable IRQ */
    hal_nvic_disable_irq(s_i2c_irq_table[i2c_port]);

    /*mask interrupt*/
    I2C_MASK_INTERRUPT(i2c_port);

    /*clear callback*/
    s_i2c_callback[i2c_port] = NULL;

    /* set dma false */
    s_i2c_dma_parameter[i2c_port].dma_finish = false;

    hal_clock_disable(g_i2c_pdn_table[i2c_port]);

    /*unlock dvfs*/
    I2C_UNLOCK_DVFS();
    /* set to idle */
    I2C_SET_IDLE(i2c_port);

    return HAL_I2C_STATUS_OK;
}
hal_i2c_status_t hal_i2c_master_deinit(hal_i2c_port_t i2c_port)
{
    hal_i2c_status_t status;
    uint32_t saved_mask;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (i2c_sw_fifo_status[i2c_port] != NB_STATUS_OF_OCCUPY_BY_OLD_USER) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return HAL_I2C_STATUS_BUSY_BY_NON_BLOCKING_FUNCTION;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);
#endif

    status = hal_i2c_master_deinit_internal(i2c_port);
#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
    if (status == HAL_I2C_STATUS_OK) {
        //hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        i2c_sw_fifo_status[i2c_port] = NB_STATUS_OF_IDLE;
        //hal_nvic_restore_interrupt_mask(saved_mask);
    }
#endif
    return status;
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
        log_hal_msgid_error("[I2C%d][set_freq]:busy\r\n", 1, i2c_port);
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
        (HAL_I2C_IO_PUSH_PULL != io_config)) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_msgid_error("[I2C%d][set_io_config]:busy\r\n", 1, i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    i2c_set_io_config(i2c_port, io_config);

    return HAL_I2C_STATUS_OK;
}
#endif /* HAL_I2C_MASTER_FEATURE_CONFIG_IO */

hal_i2c_status_t hal_i2c_master_register_callback(hal_i2c_port_t i2c_port, hal_i2c_callback_t i2c_callback, void *user_data)
{
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
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
    i2c_transaction_status_t    transaction_status = I2C_TRANSACTION_SUCCESS;
    i2c_transaction_size_t      config_size;
    uint32_t                    saved_mask;
    i2c_transfer_info_t        *i2c_priv = NULL;
    bool                        i2c_is_preempte = false;

    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == data || size > I2C_FIFO_DEPTH) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    i2c_priv = &i2c_transfer_info[i2c_port];
    config_size.send_size = size;
    config_size.receive_size = 0;
    I2C_LOCK_SLEEP(i2c_port);       /* lock sleep */
    I2C_MASK_INTERRUPT(i2c_port);   /* mask i2c all interrupt*/

    /*step1: wait i2c hw idle*/
    while (1) {
        if (i2c_wait_idle_with_timeout(i2c_port, HAL_I2C_POLLING_TIMEOUT_US) < 0) {
            I2C_UNLOCK_SLEEP(i2c_port);
            log_hal_msgid_error("[i2c%d] send polling, wait hw idle timeout 1\r\n", 1, i2c_port);
            return HAL_I2C_STATUS_ERROR_BUSY;
        }
        /*step2: check hw is preempt by other task? if yes, then  continue wait hw idle */
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            log_hal_msgid_error("[i2c%d] send polling 1, retry", 1, i2c_port);
        } else {
            break;
        }
    }
    I2C_DEBUG_RECORD_STEP(i2c_port, 1);
    /*step3: check if current preempte the rx, if yes  then help previoud receive data*/
    if (i2c_priv->i2c_rx_cnt != 0 || i2c_priv->i2c_tx_cnt != 0) {
        *i2c_priv->i2c_intr_status = i2c_get_irq_status(i2c_port);
        *i2c_priv->i2c_is_preempte = true;
        if (i2c_priv->i2c_rx_cnt != 0) {
            if (*i2c_priv->i2c_intr_status == I2C_TRANSACTION_SUCCESS) {
                i2c_read_fifo(i2c_port, i2c_priv->i2c_rx_buff, i2c_priv->i2c_rx_size);
            }
            i2c_priv->i2c_rx_cnt--;
        }
        if (i2c_priv->i2c_tx_cnt != 0) {
            i2c_priv->i2c_tx_cnt--;
        }
    }
    /*step4: setting up i2c hw for current operator*/
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_POLLING, I2C_GET_SEND_ADDRESS(slave_address), &config_size);
    i2c_write_fifo(i2c_port, data, size);
    i2c_priv->i2c_op_state     = I2C_TX;
    i2c_priv->i2c_intr_status  = &transaction_status;
    i2c_priv->i2c_is_preempte  = &i2c_is_preempte;
    i2c_priv->i2c_tx_cnt++;
    /*start i2c*/
    I2C_START(i2c_port);
    /* enable global interrupt */
    hal_nvic_restore_interrupt_mask(saved_mask);

    /*step5: wait hw transfer done*/
    while (1) {
        if (i2c_wait_idle_with_timeout(i2c_port, HAL_I2C_POLLING_TIMEOUT_US) < 0) {
            log_hal_msgid_warning("[i2c%d] send polling, wait hw idle timeout 2\r\n", 1, i2c_port);
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            break;
        }
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            log_hal_msgid_error("[i2c] send polling 2, retry", 0);
        } else {
            break;
        }
    }
    I2C_DEBUG_RECORD_STEP(i2c_port, 2);
    /*step6: if hw idle, then judge i2c is preempte by other user*/
    if (i2c_is_preempte == false) {
        transaction_status = i2c_get_irq_status(i2c_port);
        i2c_priv->i2c_tx_cnt--;
    }
    if (i2c_transfer_info[i2c_port].i2c_op_state != I2C_IDLE) {
        i2c_transfer_info[i2c_port].i2c_op_state = I2C_IDLE;
    }
    /* enable global interrupt */
    hal_nvic_restore_interrupt_mask(saved_mask);
    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);
    if (transaction_status == I2C_TRANSACTION_SUCCESS) {
        return HAL_I2C_STATUS_OK;
    } else {
        log_hal_msgid_error("[i2c%d] send polling error, status %d", 2, i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    }
}




hal_i2c_status_t hal_i2c_master_send_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, const uint8_t *data, uint32_t size)
{
    i2c_transaction_size_t  config_size;
    pdma_status_t           ret_dma;

    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == data) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_msgid_error("[I2C%d][send_dma]:busy\r\n", 1, i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }
    I2C_DEBUG_RECORD_FLOW(i2c_port, 6);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /* config dma hardware */
    ret_dma = i2c_dma_init(i2c_port, true, size);
    I2C_CHECK_PDMA_STATUS(ret_dma);

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
    ret_dma = pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)data);
    I2C_CHECK_PDMA_STATUS(ret_dma);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_receive_polling(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t *buffer, uint32_t size)
{
    i2c_transaction_status_t    transaction_status = I2C_TRANSACTION_SUCCESS;
    i2c_transaction_size_t      config_size;
    uint32_t                    saved_mask;
    i2c_transfer_info_t         *i2c_priv = NULL;
    bool                        i2c_is_preempte = false;

    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == buffer || size > I2C_FIFO_DEPTH) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    i2c_priv = &i2c_transfer_info[i2c_port];
    config_size.send_size = 0;
    config_size.receive_size = size;
    I2C_LOCK_SLEEP(i2c_port); /* lock sleep */
    I2C_MASK_INTERRUPT(i2c_port);/*mask interrupt*/

    /*step1: wait i2c hw idle*/
    while (1) {
        if (i2c_wait_idle_with_timeout(i2c_port, HAL_I2C_POLLING_TIMEOUT_US) < 0) {
            I2C_UNLOCK_SLEEP(i2c_port);
            log_hal_msgid_error("[i2c%d] recv polling, wait hw idle timeout 1\r\n", 1, i2c_port);
            return HAL_I2C_STATUS_ERROR_BUSY;
        }
        /*step2: check hw is preempt by other task? if yes, then  continue wait hw idle */
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            log_hal_msgid_error("[i2c%d] recv polling 1, retry", 1, i2c_port);
        } else {
            break;
        }
    }
    I2C_DEBUG_RECORD_STEP(i2c_port, 1);

    /*step3: check if current preempte the rx, if yes  then help previoud receive data*/
    if (i2c_priv->i2c_rx_cnt != 0 || i2c_priv->i2c_tx_cnt != 0) {
        *i2c_priv->i2c_intr_status = i2c_get_irq_status(i2c_port);
        *i2c_priv->i2c_is_preempte = true;
        if (i2c_priv->i2c_rx_cnt != 0) {
            if (*i2c_priv->i2c_intr_status == I2C_TRANSACTION_SUCCESS) {
                i2c_read_fifo(i2c_port, i2c_priv->i2c_rx_buff, i2c_priv->i2c_rx_size);
            }
            i2c_priv->i2c_rx_cnt--;
        }
        if (i2c_priv->i2c_tx_cnt != 0) {

            i2c_priv->i2c_tx_cnt--;
        }
    }
    /*step4: setting up i2c hw for current operator*/
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_RECEIVE_POLLING, I2C_GET_RECEIVE_ADDRESS(slave_address), &config_size);
    i2c_priv->i2c_op_state     = I2C_RX;
    i2c_priv->i2c_intr_status  = &transaction_status;
    i2c_priv->i2c_is_preempte  = &i2c_is_preempte;
    i2c_priv->i2c_rx_buff      = buffer;
    i2c_priv->i2c_rx_size      = size;
    i2c_priv->i2c_rx_cnt++;
    I2C_START(i2c_port); /*start i2c*/
    hal_nvic_restore_interrupt_mask(saved_mask); /* enable global interrupt */

    /*step5: wait hw transfer done*/
    while (1) {
        if(i2c_wait_idle_with_timeout(i2c_port, HAL_I2C_POLLING_TIMEOUT_US) < 0 ){
            log_hal_msgid_error("[i2c%d] recv polling, wait hw idle timeout 2\r\n", 1, i2c_port);
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            break;
        }
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
            hal_nvic_restore_interrupt_mask(saved_mask);
        } else {
            break;
        }
    }
    I2C_DEBUG_RECORD_STEP(i2c_port, 2);
    /*step6: if hw idle, then judge i2c is preempte by other user*/
    if (i2c_is_preempte == false) {
        transaction_status = i2c_get_irq_status(i2c_port);
        if (transaction_status == I2C_TRANSACTION_SUCCESS) {
            i2c_read_fifo(i2c_port, (uint8_t *)i2c_priv->i2c_rx_buff, i2c_priv->i2c_rx_size);
        }
        i2c_priv->i2c_rx_cnt--;
    }
    if (i2c_transfer_info[i2c_port].i2c_op_state != I2C_IDLE) {
        i2c_transfer_info[i2c_port].i2c_op_state = I2C_IDLE;
    }
    hal_nvic_restore_interrupt_mask(saved_mask); /* enable global interrupt */

    I2C_UNLOCK_SLEEP(i2c_port);/* unlock sleep */
    if (transaction_status == I2C_TRANSACTION_SUCCESS) {
        return HAL_I2C_STATUS_OK;
    } else {
        log_hal_msgid_error("[i2c%d] recv polling error, status %d", 2, i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    }
}




hal_i2c_status_t hal_i2c_master_receive_dma(hal_i2c_port_t i2c_port, uint8_t slave_address, uint8_t  *buffer, uint32_t size)
{
    i2c_transaction_size_t  config_size;
    //pdma_config_t             i2c_dma_config;
    pdma_status_t           ret_dma;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_msgid_error("[I2C%d][receive_dma]:busy\r\n", 1, i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 8);
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    /* config dma hardware */
    ret_dma = i2c_dma_init(i2c_port, false, size);
    I2C_CHECK_PDMA_STATUS(ret_dma);

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
    ret_dma = pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)buffer);
    I2C_CHECK_PDMA_STATUS(ret_dma);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}



#ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE
hal_i2c_status_t hal_i2c_master_send_to_receive_polling(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_t *i2c_send_to_receive_config)
{
    i2c_transaction_status_t    transaction_status = I2C_TRANSACTION_SUCCESS;
    i2c_transaction_size_t      config_size;
    uint32_t                    saved_mask;
    i2c_transfer_info_t        *i2c_priv = NULL;
    bool                        i2c_is_preempte = false;
    //uint32_t                    tick0, tick1;

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
    i2c_priv = &i2c_transfer_info[i2c_port];
    config_size.send_size = i2c_send_to_receive_config->send_length;
    config_size.receive_size = i2c_send_to_receive_config->receive_length;
    I2C_LOCK_SLEEP(i2c_port);/* lock sleep */
    I2C_MASK_INTERRUPT(i2c_port); /*mask interrupt*/

    /*step1: wait i2c hw idle*/
    while (1) {
        if (i2c_wait_idle_with_timeout(i2c_port, HAL_I2C_POLLING_TIMEOUT_US) < 0) {
            I2C_UNLOCK_SLEEP(i2c_port);
            log_hal_msgid_error("[i2c%d] snd recv polling, wait hw idle timeout 1\r\n", 1, i2c_port);
            return HAL_I2C_STATUS_ERROR_BUSY;
        }
        /*step2: disable irq and check hw is preempt by other task? if yes, then  continue wait hw idle */
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            log_hal_msgid_error("[i2c%d] send to recv polling 1, retry", 1, i2c_port);
        } else {
            break;
        }
    }
    I2C_DEBUG_RECORD_STEP(i2c_port, 1);
    /*step3: check if current preempte the rx, if yes  then help previoud receive data*/
    if (i2c_priv->i2c_rx_cnt != 0 || i2c_priv->i2c_tx_cnt != 0) {
        *i2c_priv->i2c_intr_status = i2c_get_irq_status(i2c_port);
        *i2c_priv->i2c_is_preempte = true;
        if (i2c_priv->i2c_rx_cnt != 0) {
            if (*i2c_priv->i2c_intr_status == I2C_TRANSACTION_SUCCESS) {
                i2c_read_fifo(i2c_port, i2c_priv->i2c_rx_buff, i2c_priv->i2c_rx_size);
            }
            i2c_priv->i2c_rx_cnt--;
        }
        if (i2c_priv->i2c_tx_cnt != 0) {
            i2c_priv->i2c_tx_cnt--;
        }
    }
    /*step4: setting up i2c hw for current operator*/
    /*config i2c hardware*/
    i2c_config_hardware(i2c_port, I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_POLLING, I2C_GET_SEND_ADDRESS(i2c_send_to_receive_config->slave_address), &config_size);
    /*set data to fifo*/
    i2c_write_fifo(i2c_port, i2c_send_to_receive_config->send_data, i2c_send_to_receive_config->send_length);
    i2c_priv->i2c_op_state     = I2C_RX;
    i2c_priv->i2c_intr_status  = &transaction_status;
    i2c_priv->i2c_is_preempte  = &i2c_is_preempte;
    i2c_priv->i2c_rx_buff      = i2c_send_to_receive_config->receive_buffer;
    i2c_priv->i2c_rx_size      = i2c_send_to_receive_config->receive_length;
    i2c_priv->i2c_rx_cnt++;
    I2C_START(i2c_port); /*start i2c*/
    hal_nvic_restore_interrupt_mask(saved_mask);/* enable global interrupt */

    /*step5: wait hw transfer done*/
    while (1) {
        if (i2c_wait_idle_with_timeout(i2c_port, HAL_I2C_POLLING_TIMEOUT_US) < 0) {
            log_hal_msgid_error("[i2c%d] send to recv polling, wait hw idle timeout 2\r\n", 1, i2c_port);
            hal_nvic_save_and_set_interrupt_mask(&saved_mask);
            break;
        }
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
            hal_nvic_restore_interrupt_mask(saved_mask);
            log_hal_msgid_error("[i2c] send to recv polling 2, retry", 0);
        } else {
            break;
        }
    }
    I2C_DEBUG_RECORD_STEP(i2c_port, 2);
    /*step6: if hw idle, then judge i2c is preempte by other user*/
    if (i2c_is_preempte == false) {
        transaction_status = i2c_get_irq_status(i2c_port);
        if (transaction_status == I2C_TRANSACTION_SUCCESS) {
            i2c_read_fifo(i2c_port, (uint8_t *)i2c_priv->i2c_rx_buff, i2c_priv->i2c_rx_size);
        }
        i2c_priv->i2c_rx_cnt--;
    }
    if (i2c_transfer_info[i2c_port].i2c_op_state != I2C_IDLE) {
        i2c_transfer_info[i2c_port].i2c_op_state = I2C_IDLE;
    }
    /* enable global interrupt */
    hal_nvic_restore_interrupt_mask(saved_mask);
    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);
    if (transaction_status == I2C_TRANSACTION_SUCCESS) {
        return HAL_I2C_STATUS_OK;
    } else {
        log_hal_msgid_error("[i2c] send to recv polling error, status %d", 2, i2c_port, transaction_status);
        return HAL_I2C_STATUS_ERROR;
    }
}


hal_i2c_status_t hal_i2c_master_send_to_receive_dma(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_t *i2c_send_to_receive_config)
{
    i2c_transaction_size_t  config_size;
    pdma_config_t           i2c_dma_config;
    pdma_status_t           pdma_status;

    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_to_receive_config->send_data || NULL == i2c_send_to_receive_config->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }

    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_msgid_error("[I2C%d][send2receive_dma]:busy\r\n", 1, i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 10);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);

    i2c_dma_callback_count[i2c_port] = 0;

    /* config dma tx hardware */
    pdma_status = pdma_init(g_i2c_tx_dma_channel[i2c_port]);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = i2c_send_to_receive_config->send_length;
    i2c_dma_config.master_type = PDMA_TX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_status = pdma_configure(g_i2c_tx_dma_channel[i2c_port], &i2c_dma_config);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /* config dma rx hardware */
    pdma_status = pdma_init(g_i2c_rx_dma_channel[i2c_port]);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /* total dma length = size * count */
    i2c_dma_config.burst_mode = false;
    i2c_dma_config.count = i2c_send_to_receive_config->receive_length;
    i2c_dma_config.master_type = PDMA_RX;
    i2c_dma_config.size = PDMA_BYTE;
    pdma_status = pdma_configure(g_i2c_rx_dma_channel[i2c_port], &i2c_dma_config);
    I2C_CHECK_PDMA_STATUS(pdma_status);
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
    pdma_status = pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config->send_data);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /*start dma rx */
    pdma_status = pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config->receive_buffer);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /* start I2C */
    I2C_START(i2c_port);


    return HAL_I2C_STATUS_OK;
}
#endif /* #ifdef HAL_I2C_MASTER_FEATURE_SEND_TO_RECEIVE */


#ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA
hal_i2c_status_t hal_i2c_master_send_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_send_config_t *i2c_send_config)
{
    i2c_transaction_size_t  config_size;
    pdma_status_t           pdma_status;
    uint32_t                temp = 0;

    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_config->send_data) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_msgid_error("[I2C%d][send_dma_enhanced]:busy\r\n", 1, i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }
    I2C_DEBUG_RECORD_FLOW(i2c_port, 14);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /* config dma hardware */
    temp = (i2c_send_config->send_packet_length) * (i2c_send_config->send_bytes_in_one_packet);
    pdma_status = i2c_dma_init(i2c_port, true, temp);
    I2C_CHECK_PDMA_STATUS(pdma_status);

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
    pdma_status = pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_config->send_data);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_receive_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_receive_config_t *i2c_receive_config)
{
    i2c_transaction_size_t  config_size;
    pdma_status_t           pdma_status;
    uint32_t                temp = 0;

    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_receive_config->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_msgid_error("[I2C%d][receive_dma_enhanced]:busy\r\n", 1, i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }

    I2C_DEBUG_RECORD_FLOW(i2c_port, 15);
    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    /* config dma hardware */
    temp = (i2c_receive_config->receive_packet_length) * (i2c_receive_config->receive_bytes_in_one_packet);
    pdma_status = i2c_dma_init(i2c_port, false, temp);
    I2C_CHECK_PDMA_STATUS(pdma_status);

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
    pdma_status = pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_receive_config->receive_buffer);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}


hal_i2c_status_t hal_i2c_master_send_to_receive_dma_ex(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_ex_t *i2c_send_to_receive_config_ex)
{
    i2c_transaction_size_t  config_size;
    pdma_status_t           pdma_status;
    uint32_t                temp = 0;
    /* parameter check */
    if (HAL_I2C_MASTER_MAX <= i2c_port) {
        return HAL_I2C_STATUS_INVALID_PORT_NUMBER;
    }
    if (NULL == i2c_send_to_receive_config_ex->send_data || NULL == i2c_send_to_receive_config_ex->receive_buffer) {
        return HAL_I2C_STATUS_INVALID_PARAMETER;
    }
    if (I2C_BUSY == I2C_QUERY_RUNNING_STATUS(i2c_port)) {
        log_hal_msgid_error("[I2C%d][send2receive_dma_enhanced]:busy\r\n", 1, i2c_port);
        return HAL_I2C_STATUS_ERROR_BUSY;
    }
    I2C_DEBUG_RECORD_FLOW(i2c_port, 16);

    /* lock sleep */
    I2C_LOCK_SLEEP(i2c_port);
    i2c_dma_callback_count[i2c_port] = 0;

    /* config dma tx hardware */
    temp = i2c_send_to_receive_config_ex->send_bytes_in_one_packet;
    pdma_status = i2c_dma_init(i2c_port, true, temp);
    I2C_CHECK_PDMA_STATUS(pdma_status);

    /* register dma callback function */
    s_i2c_dma_parameter[i2c_port].transaction_type = I2C_TRANSACTION_OPEN_DRAIN_SEND_TO_RECEIVE_DMA_EXTEND;
    s_i2c_dma_parameter[i2c_port].dma_finish = false;
    s_i2c_dma_parameter[i2c_port].i2c_port = i2c_port;
    pdma_register_callback(g_i2c_tx_dma_channel[i2c_port], hal_i2c_master_dma_callback, (void *)&s_i2c_dma_parameter[i2c_port]);

    /* config dma rx hardware */
    temp = (i2c_send_to_receive_config_ex->receive_packet_length) * (i2c_send_to_receive_config_ex->receive_bytes_in_one_packet);
    pdma_status = i2c_dma_init(i2c_port, false, temp);
    I2C_CHECK_PDMA_STATUS(pdma_status);

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
    pdma_status = pdma_start_interrupt(g_i2c_tx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config_ex->send_data);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /*start dma rx */
    pdma_status = pdma_start_interrupt(g_i2c_rx_dma_channel[i2c_port], (uint32_t)i2c_send_to_receive_config_ex->receive_buffer);
    I2C_CHECK_PDMA_STATUS(pdma_status);
    /* start I2C */
    I2C_START(i2c_port);

    return HAL_I2C_STATUS_OK;
}

#endif /* #ifdef HAL_I2C_MASTER_FEATURE_EXTENDED_DMA */

#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
static hal_i2c_status_t i2c_nb_try_to_pop_and_start(hal_i2c_port_t i2c_port);


static hal_i2c_status_t    i2c_nb_start_dma_transfer(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_ex_t *config)
{
    uint32_t    tx_len = 0;
    uint32_t    rx_len = 0;
    hal_i2c_status_t status;

    tx_len = (config->send_bytes_in_one_packet    * config->send_packet_length);
    rx_len = (config->receive_bytes_in_one_packet * config->receive_packet_length);

    if (rx_len == 0 && tx_len != 0) {
        hal_i2c_send_config_t   tx_cfg;

        tx_cfg.slave_address            = config->slave_address;
        tx_cfg.send_packet_length       = config->send_packet_length;
        tx_cfg.send_bytes_in_one_packet = config->send_bytes_in_one_packet;
        tx_cfg.send_data                = config->send_data;
        status = hal_i2c_master_send_dma_ex(i2c_port, &tx_cfg);
    } else if (rx_len != 0 && tx_len == 0) {
        hal_i2c_receive_config_t rx_cfg;

        rx_cfg.slave_address                = config->slave_address;
        rx_cfg.receive_bytes_in_one_packet  = config->receive_bytes_in_one_packet;
        rx_cfg.receive_packet_length        = config->receive_packet_length;
        rx_cfg.receive_buffer               = config->receive_buffer;

        status = hal_i2c_master_receive_dma_ex(i2c_port, &rx_cfg);
    } else {
        status = hal_i2c_master_send_to_receive_dma_ex(i2c_port, config);
    }
    return status;
}


static void i2c_nb_sw_fifo_handler(uint8_t slave_address, hal_i2c_callback_event_t event, void *user_data)
{
    hal_i2c_callback_t old_i2c_callback;
    void *old_i2c_user_data;
    i2c_master_non_blocking_user_data_t *p_user_data = user_data;
    hal_i2c_port_t i2c_port;

    i2c_port = p_user_data->current_i2c_port;
    old_i2c_user_data = p_user_data->current_user_data;
    old_i2c_callback = g_i2c_m_nb_current_user_cb[i2c_port];

    hal_i2c_master_deinit_internal(i2c_port);
    i2c_sw_fifo_status[i2c_port] = NB_STATUS_OF_ONE_NOTE_DONE;
    i2c_nb_try_to_pop_and_start(i2c_port);

    /* if sw fifo not empty,when call hal_i2c_master_deinit api,
      * it will recover g_i2c_m_nb_current_user_cb,so we need to use old_i2c_callback*/
    if (*old_i2c_callback != NULL) {
        (*old_i2c_callback)(slave_address, event, old_i2c_user_data);
    }
}

static hal_i2c_status_t i2c_nb_try_to_pop_and_start(hal_i2c_port_t i2c_port)
{
    uint32_t saved_mask;
    hal_i2c_sw_fifo_node_t transfer_config;
    hal_i2c_status_t status;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if ((i2c_sw_fifo_status[i2c_port] == NB_STATUS_OF_RUNING)) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return HAL_I2C_STATUS_OK;
    } else if ((i2c_sw_fifo_status[i2c_port] == NB_STATUS_OF_IDLE) || (NB_STATUS_OF_ONE_NOTE_DONE == i2c_sw_fifo_status[i2c_port])) {
        if (0 == i2c_pop_sw_fifo(i2c_port, &transfer_config, 1)) {
            i2c_sw_fifo_status[i2c_port] = NB_STATUS_OF_IDLE;
            hal_nvic_restore_interrupt_mask(saved_mask);
            return HAL_I2C_STATUS_OK;
        }
        i2c_sw_fifo_status[i2c_port] = NB_STATUS_OF_RUNING;
        g_i2c_m_nb_current_user_cb[i2c_port] = transfer_config.i2c_callback;
        g_i2c_m_nb_current_user_data[i2c_port].current_user_data = transfer_config.user_data;
        g_i2c_m_nb_current_user_data[i2c_port].current_i2c_port = i2c_port;

        hal_nvic_restore_interrupt_mask(saved_mask);

        status = hal_i2c_master_init_internal(i2c_port, &transfer_config.i2c_config);

        if (status != HAL_I2C_STATUS_OK) {
            transfer_config.i2c_callback(transfer_config.i2c_send_to_receive_config_ex.slave_address, HAL_I2C_NON_BLOCKING_FUNCTION_ERROR, transfer_config.user_data);
            return status;
        }
        status = hal_i2c_master_register_callback(i2c_port, i2c_nb_sw_fifo_handler, &g_i2c_m_nb_current_user_data[i2c_port]);
        if (status != HAL_I2C_STATUS_OK) {
            transfer_config.i2c_callback(transfer_config.i2c_send_to_receive_config_ex.slave_address, HAL_I2C_NON_BLOCKING_FUNCTION_ERROR, transfer_config.user_data);
            return status;
        }
        status = i2c_nb_start_dma_transfer(i2c_port, &transfer_config.i2c_send_to_receive_config_ex);
        if (status != HAL_I2C_STATUS_OK) {
            transfer_config.i2c_callback(transfer_config.i2c_send_to_receive_config_ex.slave_address, HAL_I2C_NON_BLOCKING_FUNCTION_ERROR, transfer_config.user_data);
            return status;
        }
    } else {
        hal_nvic_restore_interrupt_mask(saved_mask);
    }
    return HAL_I2C_STATUS_OK;

}

hal_i2c_status_t hal_i2c_master_send_to_receive_dma_ex_none_blocking(hal_i2c_port_t i2c_port, hal_i2c_send_to_receive_config_ex_no_busy_t *i2c_send_to_receive_config_no_busy_ex)
{
    uint32_t saved_mask;
    /* s_i2c_master_status indicate  iic status , while fifo not empty and first user callback call this api,even though
        * s_i2c_master_status idle,it should be pushed in fifo.
        */
    hal_i2c_sw_fifo_node_t i2c_sw_fifo_node;

#if defined(HAL_I2C_MASTER_FRATURE_NONE_BLOCKING) && defined(HAL_I2C_MASTER_FEATURE_EXTENDED_DMA)
    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if (s_i2c_sw_fifo_init_flag[i2c_port] == 0) {
        i2c_sw_fifo_init(i2c_port);
        s_i2c_sw_fifo_init_flag[i2c_port] = 1;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);
#endif


    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if ((i2c_sw_fifo_status[i2c_port] == NB_STATUS_OF_OCCUPY_BY_OLD_USER)) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return   HAL_I2C_STATUS_ERROR_USED_BY_NORMAL_USER;
    }
    i2c_sw_fifo_node.i2c_port = i2c_port;
    i2c_sw_fifo_node.i2c_callback = i2c_send_to_receive_config_no_busy_ex->i2c_callback;
    i2c_sw_fifo_node.user_data =  i2c_send_to_receive_config_no_busy_ex->user_data;
    // setting of i2c_config copy to i2c_sw_fifo_node for push
    memcpy(&i2c_sw_fifo_node.i2c_config, &i2c_send_to_receive_config_no_busy_ex->i2c_config, sizeof(hal_i2c_config_t));
    // setting of i2c_send_to_receive_config_ex copy to i2c_sw_fifo_node for push
    memcpy(&i2c_sw_fifo_node.i2c_send_to_receive_config_ex, &i2c_send_to_receive_config_no_busy_ex->i2c_send_to_receive_config_ex, sizeof(hal_i2c_send_to_receive_config_ex_t));

    if (0 == i2c_push_sw_fifo(i2c_port, &i2c_sw_fifo_node, 1)) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return   HAL_I2C_STATUS_FULL_BY_NON_BLOCKING_FUNCTION;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    return i2c_nb_try_to_pop_and_start(i2c_port);

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
        log_hal_msgid_error("[I2C%d][isr]:port=%d\r\n", 2, i2c_port, i2c_port);
        return;
    }
    I2C_DEBUG_RECORD_FLOW(i2c_port, 11);
    hal_i2c_isr_handler(i2c_port, s_i2c_callback[i2c_port], s_i2c_callback_parameter[i2c_port]);
}


void hal_i2c_isr_handler(hal_i2c_port_t i2c_port, hal_i2c_callback_t user_callback, void *user_data)
{
    i2c_transaction_status_t transaction_status;
    //volatile uint32_t  flag = TRUE;

    /* unlock sleep */
    I2C_UNLOCK_SLEEP(i2c_port);
    /* read i2c status */
    transaction_status = i2c_get_irq_status(i2c_port);
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
        log_hal_msgid_error("[I2C%d][isr]:NO isr callback\r\n", 1, i2c_port);
    }
}


static void hal_i2c_master_dma_callback(__attribute__((unused))pdma_event_t event, void  *user_data)
{
    i2c_dma_callback_parameter_t *i2c_dma_parameter;

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
            log_hal_msgid_error("[I2C%d][isr]:dma timeout!\r\n", 1, i2c_port);
            I2C_DEBUG_RECORD_FLOW(i2c_port, 13);
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
            s_i2c_register_backup[i].clock_div  = i2c_master_register[i]->CLOCK_DIV;
            s_i2c_register_backup[i].timing     = i2c_master_register[i]->TIMING_UNION.TIMING;
            s_i2c_register_backup[i].io         = i2c_master_register[i]->IO_CONFIG;
            s_i2c_register_backup[i].control2   = i2c_master_register[i]->CONTROL2_UNION.CONTROL2;
        }
    }
}


void hal_i2c_restore_all_register(void)
{
    uint32_t i = 0;

    for (i = 0; i < HAL_I2C_MASTER_MAX; i++) {
        if (s_i2c_master_status[i] == I2C_BUSY) {
            i2c_master_register[i]->CLOCK_DIV               = s_i2c_register_backup[i].clock_div;
            i2c_master_register[i]->TIMING_UNION.TIMING     = s_i2c_register_backup[i].timing;
            i2c_master_register[i]->IO_CONFIG               = s_i2c_register_backup[i].io;
            i2c_master_register[i]->CONTROL2_UNION.CONTROL2 = s_i2c_register_backup[i].control2;
        }
    }
}
#endif /* #ifdef HAL_SLEEP_MANAGER_ENABLED */


#endif /* #if defined(HAL_I2C_MASTER_MODULE_ENABLED) */

