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

#include "hal_spi_master.h"

#ifdef HAL_SPI_MASTER_MODULE_ENABLED
#include <string.h>
#include "hal_spi_master_internal.h"
#include "hal_nvic.h"
#include "hal_nvic_internal.h"
#include "hal_clock.h"
#include "hal_cache.h"
#include "hal_cache_internal.h"
#include "hal_log.h"
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"

static const IRQn_Type g_spi_master_irq_code[HAL_SPI_MASTER_MAX] = {SPI_MST0_IRQn, SPI_MST1_IRQn, SPI_MST2_IRQn};
static const hal_clock_cg_id g_spi_master_cg_code[HAL_SPI_MASTER_MAX] = {HAL_CLOCK_CG_SPIMST0, HAL_CLOCK_CG_SPIMST1, HAL_CLOCK_CG_SPIMST2};
static const hal_clock_cg_id g_spi_master_bus_cg_code[HAL_SPI_MASTER_MAX] = {HAL_CLOCK_CG_SPIMST0_BUS, HAL_CLOCK_CG_SPIMST1_BUS, HAL_CLOCK_CG_SPIMST2_BUS};
static const clock_mux_sel_id g_spi_master_cg_sel[HAL_SPI_MASTER_MAX] = {CLK_SPIMST0_SEL, CLK_SPIMST1_SEL, CLK_SPIMST2_SEL};
static const uint16_t g_spi_master_pri_code[HAL_SPI_MASTER_MAX] = {SPI_MST0_IRQ_PRIORITY, SPI_MST1_IRQ_PRIORITY, SPI_MST2_IRQ_PRIORITY};
static hal_spi_master_callback_t g_spi_master_callback[HAL_SPI_MASTER_MAX] = {NULL};
static void *g_spi_master_user_data[HAL_SPI_MASTER_MAX] = {NULL};
static volatile uint8_t g_spi_master_status[HAL_SPI_MASTER_MAX] = {0};
static hal_spi_master_mode_t g_spi_master_mode[HAL_SPI_MASTER_MAX] = {HAL_SPI_MASTER_SINGLE_MODE};

spi_master_direction_t g_spi_master_direction[HAL_SPI_MASTER_MAX] = {SPI_MASTER_TX};
#ifdef HAL_SLEEP_MANAGER_ENABLED
sleep_management_lock_request_t g_spi_master_sleep_handle[HAL_SPI_MASTER_MAX] = {SLEEP_LOCK_SPI_MST0, SLEEP_LOCK_SPI_MST1, SLEEP_LOCK_SPI_MST2};
sleep_management_backup_restore_module_t g_spi_master_sleep_backup_restore_handle[HAL_SPI_MASTER_MAX] = {SLEEP_BACKUP_RESTORE_SPI_MST0, SLEEP_BACKUP_RESTORE_SPI_MST1, SLEEP_BACKUP_RESTORE_SPI_MST2};
#endif

#if defined(HAL_SPI_MASTER_FRATURE_NO_BUSY) && defined (HAL_SPI_MASTER_FEATURE_DMA_MODE)

#define NB_STATUS_OF_IDLE 0
#define NB_STATUS_OF_OCCUPY_BY_OLD_USER 1
#define NB_STATUS_OF_RUNING 2
#define NB_STATUS_OF_ONE_NOTE_DONE 3
static volatile uint32_t spi_sw_fifo_status[HAL_SPI_MASTER_MAX] = {NB_STATUS_OF_IDLE};



static uint8_t  s_spi_sw_fifo_init_flag[HAL_SPI_MASTER_MAX] = {0};
static hal_spi_master_callback_t  g_spi_m_nb_current_user_cb[HAL_SPI_MASTER_MAX] = {NULL};
typedef struct{
    void *current_user_data;
    hal_spi_master_port_t current_spi_port;
}spi_master_non_blocking_user_data_t;
static spi_master_non_blocking_user_data_t g_spi_m_nb_current_user_data[HAL_SPI_MASTER_MAX];

#endif

extern hal_clock_status_t clock_mux_sel(clock_mux_sel_id mux_id, uint32_t mux_sel);

/* internal parameter check functions */
static bool is_master_port(hal_spi_master_port_t master_port)
{
    return (master_port < HAL_SPI_MASTER_MAX);
}

static bool is_master_config(const hal_spi_master_config_t *spi_config)
{
    bool ret = true;

    ret &= (((spi_config->clock_frequency) >= HAL_SPI_MASTER_CLOCK_MIN_FREQUENCY) &&
            ((spi_config->clock_frequency) <= HAL_SPI_MASTER_CLOCK_MAX_FREQUENCY));

    ret &= (((spi_config->bit_order) == HAL_SPI_MASTER_LSB_FIRST) ||
            ((spi_config->bit_order) == HAL_SPI_MASTER_MSB_FIRST));

    ret &= (((spi_config->polarity) == HAL_SPI_MASTER_CLOCK_POLARITY0) ||
            ((spi_config->polarity) == HAL_SPI_MASTER_CLOCK_POLARITY1));

    ret &= (((spi_config->phase) == HAL_SPI_MASTER_CLOCK_PHASE0) ||
            ((spi_config->phase) == HAL_SPI_MASTER_CLOCK_PHASE1));

    ret &= ((spi_config->slave_port) < HAL_SPI_MASTER_SLAVE_MAX);

    return ret;
}

void spi_master_isr(hal_nvic_irq_t irq_number)
{
    uint32_t i;
    hal_spi_master_port_t spi_port = HAL_SPI_MASTER_0;

    for (i = 0; i < HAL_SPI_MASTER_MAX; i++) {
        if (g_spi_master_irq_code[i] == irq_number) {
            spi_port = (hal_spi_master_port_t)i;
            break;
        }
    }

    spi_master_isr_handler(spi_port, g_spi_master_callback[spi_port], g_spi_master_user_data[spi_port]);
}

static hal_spi_master_status_t hal_spi_master_clock_enable(hal_spi_master_port_t spi_port)
{
    if (hal_clock_is_enabled(g_spi_master_cg_code[spi_port]) != true) {
        if (HAL_CLOCK_STATUS_OK != hal_clock_enable(g_spi_master_cg_code[spi_port])) {
            log_hal_msgid_error("[SPIM%d] Clock enable failed!\r\n",1, spi_port);
            return HAL_SPI_MASTER_STATUS_ERROR;
        }
    }
    if (hal_clock_is_enabled(g_spi_master_bus_cg_code[spi_port]) != true) {
        if (HAL_CLOCK_STATUS_OK != hal_clock_enable(g_spi_master_bus_cg_code[spi_port])) {
            log_hal_msgid_error("[SPIM%d] Bus clock enable failed!\r\n",1, spi_port);
            return HAL_SPI_MASTER_STATUS_ERROR;
        }
    }

    return HAL_SPI_MASTER_STATUS_OK;
}

static hal_spi_master_status_t hal_spi_master_clock_disable(hal_spi_master_port_t spi_port)
{
    if (hal_clock_is_enabled(g_spi_master_cg_code[spi_port]) == true) {
        if (HAL_CLOCK_STATUS_OK != hal_clock_disable(g_spi_master_cg_code[spi_port])) {
            log_hal_msgid_error("[SPIM%d] Clock enable failed!\r\n",1, spi_port);
            return HAL_SPI_MASTER_STATUS_ERROR;
        }
    }
    if (hal_clock_is_enabled(g_spi_master_bus_cg_code[spi_port]) == true) {
        if (HAL_CLOCK_STATUS_OK != hal_clock_disable(g_spi_master_bus_cg_code[spi_port])) {
            log_hal_msgid_error("[SPIM%d] Bus clock enable failed!\r\n",1, spi_port);
            return HAL_SPI_MASTER_STATUS_ERROR;
        }
    }

    return HAL_SPI_MASTER_STATUS_OK;
}

hal_spi_master_status_t hal_spi_master_init_internal(hal_spi_master_port_t master_port,
        hal_spi_master_config_t *spi_config)
{
    hal_spi_master_status_t busy_status;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (spi_config == NULL) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (!is_master_config(spi_config)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* thread safe protect */
    SPI_CHECK_AND_SET_BUSY(master_port, busy_status);
    if (HAL_SPI_MASTER_STATUS_ERROR_BUSY == busy_status) {
        log_hal_msgid_error("[SPIM%d][init]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_clock_enable(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    if (HAL_CLOCK_STATUS_OK != clock_mux_sel(g_spi_master_cg_sel[master_port], SPI_MASTER_CLOCK_SOURCE_104MHZ)) {
        log_hal_msgid_error("[SPIM%d] Clock source select failed!\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR;
    }
    spi_master_reset_default_value(master_port);

    /* set parameter to registers */
    spi_master_init(master_port, spi_config);

#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(g_spi_master_sleep_backup_restore_handle[master_port], (sleep_management_suspend_callback_t)spi_master_backup_register_callback, NULL);
    sleep_management_register_resume_callback(g_spi_master_sleep_backup_restore_handle[master_port], (sleep_management_resume_callback_t)spi_master_restore_register_callback, NULL);
#endif

    g_spi_master_mode[master_port] = HAL_SPI_MASTER_SINGLE_MODE;

    return HAL_SPI_MASTER_STATUS_OK;
}
hal_spi_master_status_t hal_spi_master_init(hal_spi_master_port_t master_port,
        hal_spi_master_config_t *spi_config)
{
    hal_spi_master_status_t status;

#if defined(HAL_SPI_MASTER_FRATURE_NO_BUSY) && defined (HAL_SPI_MASTER_FEATURE_DMA_MODE)
    uint32_t saved_mask;
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if(spi_sw_fifo_status[master_port] != NB_STATUS_OF_IDLE)
        {
            hal_nvic_restore_interrupt_mask(saved_mask);
            return HAL_SPI_STATUS_BUSY_BY_NON_BLOCKING_FUNCTION;
        }
        spi_sw_fifo_status[master_port] = NB_STATUS_OF_OCCUPY_BY_OLD_USER;
        hal_nvic_restore_interrupt_mask(saved_mask);
#endif
    status = hal_spi_master_init_internal(master_port,spi_config);

#if defined(HAL_SPI_MASTER_FRATURE_NO_BUSY) && defined (HAL_SPI_MASTER_FEATURE_DMA_MODE)
    if(HAL_SPI_MASTER_STATUS_OK != status)
        spi_sw_fifo_status[master_port] = NB_STATUS_OF_IDLE;
#endif
    return status;
}

#ifdef HAL_SPI_MASTER_FEATURE_ADVANCED_CONFIG
static bool is_master_advanced_config(const hal_spi_master_advanced_config_t *advanced_config)
{
    bool ret = true;

    ret &= (((advanced_config->byte_order) == HAL_SPI_MASTER_LITTLE_ENDIAN) || \
            ((advanced_config->byte_order) == HAL_SPI_MASTER_BIG_ENDIAN));

    ret &= (((advanced_config->chip_polarity) == HAL_SPI_MASTER_CHIP_SELECT_LOW) || \
            ((advanced_config->chip_polarity) == HAL_SPI_MASTER_CHIP_SELECT_HIGH));

    ret &= ((advanced_config->get_tick) <= HAL_SPI_MASTER_GET_TICK_DELAY7);

    ret &= (((advanced_config->sample_select) == HAL_SPI_MASTER_SAMPLE_POSITIVE) || \
            ((advanced_config->sample_select) == HAL_SPI_MASTER_SAMPLE_NEGATIVE));

    return ret;
}

hal_spi_master_status_t hal_spi_master_set_advanced_config(hal_spi_master_port_t master_port,
        hal_spi_master_advanced_config_t *advanced_config)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (advanced_config == NULL) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (!is_master_advanced_config(advanced_config)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_advanced_config(master_port, advanced_config);

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif

hal_spi_master_status_t hal_spi_master_deinit_internal(hal_spi_master_port_t master_port)
{

    /* reset state to idle */
    SPI_SET_IDLE(master_port);

    /* disable interrupt */
    hal_nvic_disable_irq(g_spi_master_irq_code[master_port]);

    /* clear callback */
    g_spi_master_callback[master_port] = NULL;

    spi_master_reset_default_value(master_port);

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_clock_disable(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR;
    }
    return HAL_SPI_MASTER_STATUS_OK;
}

hal_spi_master_status_t hal_spi_master_deinit(hal_spi_master_port_t master_port)
{
    uint32_t saved_mask;
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }

#if defined(HAL_SPI_MASTER_FRATURE_NO_BUSY) && defined (HAL_SPI_MASTER_FEATURE_DMA_MODE)
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if(spi_sw_fifo_status[master_port] != NB_STATUS_OF_OCCUPY_BY_OLD_USER)
        {
            hal_nvic_restore_interrupt_mask(saved_mask);
            return HAL_SPI_STATUS_BUSY_BY_NON_BLOCKING_FUNCTION;
        }
        spi_sw_fifo_status[master_port] = NB_STATUS_OF_IDLE;
        hal_nvic_restore_interrupt_mask(saved_mask);
#endif

    return hal_spi_master_deinit_internal(master_port);
}


hal_spi_master_status_t hal_spi_master_send_polling(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        status = HAL_SPI_MASTER_STATUS_ERROR_PORT;
        return status;
    }
    if (NULL == data) {
        status = HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
        return status;
    }
    if ((size <= 0) || (size > HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE)) {
        status = HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
        return status;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][send_polling]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_mode(master_port, SPI_MASTER_TX, SPI_MASTER_MODE_FIFO);
    spi_master_set_mode(master_port, SPI_MASTER_RX, SPI_MASTER_MODE_FIFO);
    spi_master_set_interrupt(master_port, false);

    status = spi_master_push_data(master_port, data, size, size);
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        return status;
    }

    spi_master_start_transfer_fifo(master_port, true);

    return status;
}


hal_spi_master_status_t hal_spi_master_send_and_receive_polling(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (spi_send_and_receive_config == NULL) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (NULL == (spi_send_and_receive_config->receive_buffer)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length != 0) &&
            (NULL == (spi_send_and_receive_config->send_data))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length == 0) &&
            (NULL != (spi_send_and_receive_config->send_data))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->receive_length <= 0) ||
            (spi_send_and_receive_config->receive_length < spi_send_and_receive_config->send_length) ||
            (spi_send_and_receive_config->receive_length > HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (g_spi_master_mode[master_port] == HAL_SPI_MASTER_SINGLE_MODE) {
        if (spi_send_and_receive_config->send_length > HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE) {
            return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
        }
    } else {
        if (spi_send_and_receive_config->send_length > HAL_SPI_MAXIMUM_NON_SINGLE_MODE_TRANSACTION_SIZE) {
            return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
        }
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][send_and_receive_polling]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_mode(master_port, SPI_MASTER_TX, SPI_MASTER_MODE_FIFO);
    spi_master_set_mode(master_port, SPI_MASTER_RX, SPI_MASTER_MODE_FIFO);
    spi_master_set_interrupt(master_port, false);

    /* receive_length is the number of bytes received after sending data
       finished plus the number of bytes to send */
    status = spi_master_push_data(master_port, spi_send_and_receive_config->send_data, spi_send_and_receive_config->send_length, spi_send_and_receive_config->receive_length);
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        return status;
    }

    spi_master_start_transfer_fifo(master_port, false);

    status = spi_master_pop_data(master_port, spi_send_and_receive_config->receive_buffer, spi_send_and_receive_config->receive_length);

    return status;
}

#ifdef HAL_SPI_MASTER_FEATURE_DMA_MODE
hal_spi_master_status_t hal_spi_master_register_callback(hal_spi_master_port_t master_port,
        hal_spi_master_callback_t callback,
        void *user_data)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == callback) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    g_spi_master_callback[master_port] = callback;
    /* user_data can be as NULL */
    g_spi_master_user_data[master_port] = user_data;

    /* register NVIC */
    hal_nvic_register_isr_handler(g_spi_master_irq_code[master_port], spi_master_isr);
    hal_nvic_set_priority(g_spi_master_irq_code[master_port], g_spi_master_pri_code[master_port]);
    hal_nvic_enable_irq(g_spi_master_irq_code[master_port]);

    return HAL_SPI_MASTER_STATUS_OK;
}

hal_spi_master_status_t hal_spi_master_send_dma(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == data) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* the address for DMA buffer must be 16 bytes aligned */
    if (((uint32_t)data % 16) > 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (size <= 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#endif

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][send_dma]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_rwaddr(master_port, SPI_MASTER_TX, data);
    spi_master_set_mode(master_port, SPI_MASTER_TX, SPI_MASTER_MODE_DMA);
    spi_master_set_mode(master_port, SPI_MASTER_RX, SPI_MASTER_MODE_FIFO);
    spi_master_set_interrupt(master_port, true);
    status = spi_master_analyse_transfer_size(master_port, size);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        return status;
    } else {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_lock_sleep(g_spi_master_sleep_handle[master_port]);
#endif

        g_spi_master_direction[master_port] = SPI_MASTER_TX;
        spi_master_start_transfer_dma(master_port, false, true);
    }

    return status;
}

hal_spi_master_status_t hal_spi_master_send_dma_blocking(hal_spi_master_port_t master_port,
        uint8_t *data,
        uint32_t size)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == data) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* the address for DMA buffer must be 16 bytes aligned */
    if (((uint32_t)data % 16) > 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (size <= 0) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#endif

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][send_dma]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_rwaddr(master_port, SPI_MASTER_TX, data);
    spi_master_set_mode(master_port, SPI_MASTER_TX, SPI_MASTER_MODE_DMA);
    spi_master_set_mode(master_port, SPI_MASTER_RX, SPI_MASTER_MODE_FIFO);
    spi_master_set_interrupt(master_port, false);
    status = spi_master_analyse_transfer_size(master_port, size);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        return status;
    } else {
        spi_master_start_transfer_dma_blocking(master_port, true);
    }

    return status;
}

hal_spi_master_status_t hal_spi_master_send_and_receive_dma(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (spi_send_and_receive_config == NULL) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (NULL == (spi_send_and_receive_config->receive_buffer)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length != 0) &&
            (NULL == spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length == 0) &&
            (NULL != spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* the address for DMA buffer must be 16 bytes aligned */
    if ((((uint32_t)(spi_send_and_receive_config->receive_buffer) % 16) > 0) ||
            (((uint32_t)(spi_send_and_receive_config->send_data) % 16) > 0)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->receive_length <= 0) ||
            (spi_send_and_receive_config->receive_length < spi_send_and_receive_config->send_length)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)(spi_send_and_receive_config->receive_buffer))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_data != NULL) &&
            (true == hal_cache_is_cacheable((uint32_t)(spi_send_and_receive_config->send_data)))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#endif
    if ((g_spi_master_mode[master_port] != HAL_SPI_MASTER_SINGLE_MODE) &&
            (spi_send_and_receive_config->send_length > HAL_SPI_MAXIMUM_NON_SINGLE_MODE_TRANSACTION_SIZE)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][send_and_receive_dma]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    if (spi_send_and_receive_config->send_data != NULL) {
        spi_master_set_rwaddr(master_port, SPI_MASTER_TX, spi_send_and_receive_config->send_data);
    } else {
        spi_master_set_rwaddr(master_port, SPI_MASTER_TX, 0);
        spi_master_clear_fifo(master_port);
    }
    spi_master_set_rwaddr(master_port, SPI_MASTER_RX, spi_send_and_receive_config->receive_buffer);
    spi_master_set_mode(master_port, SPI_MASTER_RX, SPI_MASTER_MODE_DMA);
    spi_master_set_mode(master_port, SPI_MASTER_TX, SPI_MASTER_MODE_DMA);
    spi_master_set_interrupt(master_port, true);
    status = spi_master_analyse_transfer_size(master_port, spi_send_and_receive_config->receive_length);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        return status;
    } else {
#ifdef HAL_SLEEP_MANAGER_ENABLED
        hal_sleep_manager_lock_sleep(g_spi_master_sleep_handle[master_port]);
#endif
        g_spi_master_direction[master_port] = SPI_MASTER_RX;
        spi_master_start_transfer_dma(master_port, false, false);
    }

    return status;
}

hal_spi_master_status_t hal_spi_master_send_and_receive_dma_blocking(hal_spi_master_port_t master_port,
        hal_spi_master_send_and_receive_config_t *spi_send_and_receive_config)
{
    hal_spi_master_status_t status = HAL_SPI_MASTER_STATUS_OK;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (spi_send_and_receive_config == NULL) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if (NULL == (spi_send_and_receive_config->receive_buffer)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length != 0) &&
            (NULL == spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_length == 0) &&
            (NULL != spi_send_and_receive_config->send_data)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    /* the address for DMA buffer must be 16 bytes aligned */
    if ((((uint32_t)(spi_send_and_receive_config->receive_buffer) % 16) > 0) ||
            (((uint32_t)(spi_send_and_receive_config->send_data) % 16) > 0)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->receive_length <= 0) ||
            (spi_send_and_receive_config->receive_length < spi_send_and_receive_config->send_length)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#ifdef HAL_CACHE_MODULE_ENABLED
    if (true == hal_cache_is_cacheable((uint32_t)(spi_send_and_receive_config->receive_buffer))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
    if ((spi_send_and_receive_config->send_data != NULL) &&
            (true == hal_cache_is_cacheable((uint32_t)(spi_send_and_receive_config->send_data)))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }
#endif
    if ((g_spi_master_mode[master_port] != HAL_SPI_MASTER_SINGLE_MODE) &&
            (spi_send_and_receive_config->send_length > HAL_SPI_MAXIMUM_NON_SINGLE_MODE_TRANSACTION_SIZE)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][send_and_receive_dma]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    if (spi_send_and_receive_config->send_data != NULL) {
        spi_master_set_rwaddr(master_port, SPI_MASTER_TX, spi_send_and_receive_config->send_data);
    } else {
        spi_master_set_rwaddr(master_port, SPI_MASTER_TX, 0);
        spi_master_clear_fifo(master_port);
    }
    spi_master_set_rwaddr(master_port, SPI_MASTER_RX, spi_send_and_receive_config->receive_buffer);
    spi_master_set_mode(master_port, SPI_MASTER_RX, SPI_MASTER_MODE_DMA);
    spi_master_set_mode(master_port, SPI_MASTER_TX, SPI_MASTER_MODE_DMA);
    spi_master_set_interrupt(master_port, false);
    status = spi_master_analyse_transfer_size(master_port, spi_send_and_receive_config->receive_length);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        return status;
    } else {
        spi_master_start_transfer_dma_blocking(master_port, false);
    }

    return status;
}
#endif

hal_spi_master_status_t hal_spi_master_get_running_status(hal_spi_master_port_t master_port,
        hal_spi_master_running_status_t *running_status)
{
    uint32_t status = 0;

    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (NULL == running_status) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    if (hal_clock_is_enabled(g_spi_master_cg_code[master_port]) != true) {
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    /* get hardware status */
    status = spi_master_get_status(master_port);
    if (status != 0) {
        *running_status = HAL_SPI_MASTER_IDLE;
    } else {
        *running_status = HAL_SPI_MASTER_BUSY;
    }

    return HAL_SPI_MASTER_STATUS_OK;
}

#ifdef HAL_SPI_MASTER_FEATURE_CHIP_SELECT_TIMING
static bool is_master_chip_select_timing(hal_spi_master_chip_select_timing_t chip_select_timing)
{
    bool ret = true;

    ret &= ((chip_select_timing.chip_select_hold_count) <= SPIM_CFG0_CS_HOLD_COUNT_MASK);

    ret &= ((chip_select_timing.chip_select_setup_count) <= SPIM_CFG0_CS_HOLD_COUNT_MASK);

    ret &= ((chip_select_timing.chip_select_idle_count) <= SPIM_CFG2_CS_IDLE_COUNT_MASK);

    return ret;
}

hal_spi_master_status_t hal_spi_master_set_chip_select_timing(hal_spi_master_port_t master_port,
        hal_spi_master_chip_select_timing_t chip_select_timing)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (!is_master_chip_select_timing(chip_select_timing)) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][cs_timing]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_chip_select_timing(master_port, chip_select_timing);

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DEASSERT_CONFIG
hal_spi_master_status_t hal_spi_master_set_deassert(hal_spi_master_port_t master_port,
        hal_spi_master_deassert_t deassert)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (!(((deassert) == HAL_SPI_MASTER_DEASSERT_DISABLE) || \
            ((deassert) == HAL_SPI_MASTER_DEASSERT_ENABLE))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][deassert]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_deassert(master_port, deassert);

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif

#ifdef HAL_SPI_MASTER_FEATURE_DUAL_QUAD_MODE
hal_spi_master_status_t hal_spi_master_set_mode(hal_spi_master_port_t master_port,
        hal_spi_master_mode_t mode)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (((mode > HAL_SPI_MASTER_QUAD_MODE))) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][deassert]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_type(master_port, mode);

    g_spi_master_mode[master_port] = mode;

    return HAL_SPI_MASTER_STATUS_OK;
}

hal_spi_master_status_t hal_spi_master_set_dummy_bits(hal_spi_master_port_t master_port,
        uint8_t dummy_bits)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (dummy_bits > 0x0f) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][deassert]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_dummy_bits(master_port, dummy_bits);

    return HAL_SPI_MASTER_STATUS_OK;
}

hal_spi_master_status_t hal_spi_master_set_command_bytes(hal_spi_master_port_t master_port,
        uint8_t command_bytes)
{
    if (!is_master_port(master_port)) {
        return HAL_SPI_MASTER_STATUS_ERROR_PORT;
    }
    if (command_bytes > 0x0f) {
        return HAL_SPI_MASTER_STATUS_INVALID_PARAMETER;
    }

    if (g_spi_master_mode[master_port] == HAL_SPI_MASTER_SINGLE_MODE) {
        return HAL_SPI_MASTER_STATUS_ERROR;
    }

    /* check HW status before configure HW registers */
    if (HAL_SPI_MASTER_BUSY == spi_master_get_status(master_port)) {
        log_hal_msgid_error("[SPIM%d][deassert]:busy.\r\n",1, master_port);
        return HAL_SPI_MASTER_STATUS_ERROR_BUSY;
    }

    spi_master_set_command_bytes(master_port, command_bytes);

    return HAL_SPI_MASTER_STATUS_OK;
}
#endif

#if defined(HAL_SPI_MASTER_FRATURE_NO_BUSY) && defined (HAL_SPI_MASTER_FEATURE_DMA_MODE)

static hal_spi_master_status_t spi_nb_try_to_pop_and_start(hal_spi_master_port_t spi_port);

static hal_spi_master_status_t spi_nb_start_dma_transfer(hal_spi_master_port_t spi_port, hal_spi_master_send_and_receive_config_t *config)
{
    hal_spi_master_status_t status;
    uint32_t    tx_len = 0;
    uint32_t    rx_len = 0;

    tx_len = config->send_length;
    rx_len = config->receive_length;

    if ((tx_len != 0) && (rx_len == 0)) {
        status = hal_spi_master_send_dma(spi_port, config->send_data, tx_len);
    } else {
        status = hal_spi_master_send_and_receive_dma(spi_port, config);
    }
    return status;
}

static void spi_nb_sw_fifo_handler(hal_spi_master_callback_event_t event, void *user_data)
{
    hal_spi_master_callback_t old_spi_callback;
    void * old_spi_user_data;
    spi_master_non_blocking_user_data_t *p_user_data = user_data;
    hal_spi_master_port_t spi_port;

    spi_port = p_user_data->current_spi_port;
    old_spi_user_data = p_user_data->current_user_data;
    old_spi_callback = g_spi_m_nb_current_user_cb[spi_port];

    hal_spi_master_deinit_internal(spi_port);
    spi_sw_fifo_status[spi_port] = NB_STATUS_OF_ONE_NOTE_DONE;


    spi_nb_try_to_pop_and_start(spi_port);

    /* if sw fifo not empty,when call hal_spi_master_deinit api,
      * it will recover g_spi_m_nb_current_user_cb,so we need to use old_spi_callback*/
    (*old_spi_callback)(event, old_spi_user_data);

}


static hal_spi_master_status_t spi_nb_try_to_pop_and_start(hal_spi_master_port_t spi_port)
{
    uint32_t saved_mask;
    hal_spi_sw_fifo_node_t transfer_config;
    hal_spi_master_status_t status;

    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if ((spi_sw_fifo_status[spi_port] == NB_STATUS_OF_RUNING)) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return HAL_SPI_MASTER_STATUS_OK;
    } else if ((spi_sw_fifo_status[spi_port] == NB_STATUS_OF_IDLE) || (NB_STATUS_OF_ONE_NOTE_DONE == spi_sw_fifo_status[spi_port])) {
        if (0 == spi_pop_sw_fifo(spi_port, &transfer_config, 1)) {
            spi_sw_fifo_status[spi_port] = NB_STATUS_OF_IDLE;
            hal_nvic_restore_interrupt_mask(saved_mask);
            return HAL_SPI_MASTER_STATUS_OK;
        }
        spi_sw_fifo_status[spi_port] = NB_STATUS_OF_RUNING;
        g_spi_m_nb_current_user_cb[spi_port] = transfer_config.spi_callback;
        g_spi_m_nb_current_user_data[spi_port].current_user_data = transfer_config.user_data;
        g_spi_m_nb_current_user_data[spi_port].current_spi_port = spi_port;

        hal_nvic_restore_interrupt_mask(saved_mask);

        status = hal_spi_master_init_internal(spi_port, &transfer_config.spi_config);
        if (status != HAL_SPI_MASTER_STATUS_OK) {
            goto error;
        }

        status = hal_spi_master_set_advanced_config(spi_port, &transfer_config.spi_advanced_config);
        if (status != HAL_SPI_MASTER_STATUS_OK) {
            goto error;
        }

        status = hal_spi_master_set_deassert(spi_port, transfer_config.deassert);
        if (status != HAL_SPI_MASTER_STATUS_OK) {
            goto error;
        }

        status = hal_spi_master_set_chip_select_timing(spi_port, transfer_config.chip_select_timing);
        if (status != HAL_SPI_MASTER_STATUS_OK) {
            goto error;
        }

        status = hal_spi_master_set_mode(spi_port, transfer_config.spi_non_single_config.mode);
        if (status != HAL_SPI_MASTER_STATUS_OK) {
            goto error;
        }
        if (transfer_config.spi_non_single_config.mode != HAL_SPI_MASTER_SINGLE_MODE) {
            status = hal_spi_master_set_dummy_bits(spi_port, transfer_config.spi_non_single_config.dummy_bits);
            if (status != HAL_SPI_MASTER_STATUS_OK) {
                goto error;
            }
            status = hal_spi_master_set_command_bytes(spi_port, transfer_config.spi_non_single_config.command_bytes);
            if (status != HAL_SPI_MASTER_STATUS_OK) {
                goto error;
            }
        }
        status = hal_spi_master_register_callback(spi_port, spi_nb_sw_fifo_handler, &g_spi_m_nb_current_user_data[spi_port]);
        if (status != HAL_SPI_MASTER_STATUS_OK) {
            goto error;
        }

        status = spi_nb_start_dma_transfer(spi_port, &transfer_config.spi_send_and_receive_config_ex);
        if (status != HAL_SPI_MASTER_STATUS_OK) {
            goto error;
        }
    } else {
        hal_nvic_restore_interrupt_mask(saved_mask);
	}
    return HAL_SPI_MASTER_STATUS_OK;
error:
    transfer_config.spi_callback(HAL_SPI_MASTER_NO_BUSY_FUNCTION_ERROR, transfer_config.user_data);
    return status;
}

hal_spi_master_status_t hal_spi_master_send_and_receive_dma_no_busy(hal_spi_master_port_t spi_port, hal_spi_send_and_receive_config_ex_no_busy_t *spi_send_and_receive_config_no_busy_ex)
{
    uint32_t saved_mask;
    /* s_spi_master_status indicate  iic status , while fifo not empty and first user callback call this api,even though
         * s_spi_master_status idle,it should be pushed in fifo.
         */
    hal_spi_sw_fifo_node_t spi_sw_fifo_node;


#if defined(HAL_SPI_MASTER_FRATURE_NO_BUSY) && defined (HAL_SPI_MASTER_FEATURE_DMA_MODE)
        hal_nvic_save_and_set_interrupt_mask(&saved_mask);
        if (s_spi_sw_fifo_init_flag[spi_port] == 0) {
            spi_sw_fifo_init(spi_port);
            s_spi_sw_fifo_init_flag[spi_port] = 1;
        }
        hal_nvic_restore_interrupt_mask(saved_mask);
#endif


    hal_nvic_save_and_set_interrupt_mask(&saved_mask);
    if ((spi_sw_fifo_status[spi_port] == NB_STATUS_OF_OCCUPY_BY_OLD_USER)) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return   HAL_SPI_STATUS_ERROR_USED_BY_NORMAL_USER;
    }

    /* copy SPI settings to spi_sw_fifo_node */
    spi_sw_fifo_node.spi_port = spi_port;
    spi_sw_fifo_node.spi_callback = spi_send_and_receive_config_no_busy_ex->spi_callback;
    spi_sw_fifo_node.user_data =  spi_send_and_receive_config_no_busy_ex->user_data;
    memcpy(&spi_sw_fifo_node.spi_config,&spi_send_and_receive_config_no_busy_ex->spi_config, sizeof(hal_spi_master_config_t));
    memcpy(&spi_sw_fifo_node.spi_send_and_receive_config_ex,&spi_send_and_receive_config_no_busy_ex->spi_send_and_receive_config_ex, sizeof(hal_spi_master_send_and_receive_config_t));
    memcpy(&spi_sw_fifo_node.spi_advanced_config, &spi_send_and_receive_config_no_busy_ex->spi_advanced_config, sizeof(hal_spi_master_advanced_config_t));
    memcpy(&spi_sw_fifo_node.spi_non_single_config, &spi_send_and_receive_config_no_busy_ex->spi_non_single_config, sizeof(hal_spi_master_non_single_config_t));
    memcpy(&spi_sw_fifo_node.chip_select_timing, &spi_send_and_receive_config_no_busy_ex->chip_select_timing, sizeof(hal_spi_master_chip_select_timing_t));
    spi_sw_fifo_node.deassert = spi_send_and_receive_config_no_busy_ex->deassert;

    if (0 == spi_push_sw_fifo(spi_port, &spi_sw_fifo_node, 1)) {
        hal_nvic_restore_interrupt_mask(saved_mask);
        return   HAL_SPI_STATUS_FULL_BY_NON_BLOCKING_FUNCTION;
    }
    hal_nvic_restore_interrupt_mask(saved_mask);

    return spi_nb_try_to_pop_and_start(spi_port);

}

#endif

#endif /* HAL_SPI_MASTER_MODULE_ENABLED */

