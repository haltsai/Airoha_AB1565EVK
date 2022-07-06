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

#include "hal.h"
#include "hal_msdc.h"
#include "hal_sdio_internal.h"
#include "hal_sdio.h"
#include "hal_nvic.h"

#ifdef HAL_SDIO_MODULE_ENABLED
#include "hal_log.h"
#include "assert.h"
#include "hal_platform.h"
#include "hal_gpt.h"
#include <string.h>
#ifdef HAL_SLEEP_MANAGER_ENABLED
#include "hal_sleep_manager.h"
#include "hal_sleep_manager_internal.h"
#endif

extern volatile sdio_information_t sdio_information;
extern msdc_config_t msdc_config[];
gpd_t gpd[2];
bd_t bd;
extern uint32_t input_freq;

hal_sdio_status_t hal_sdio_register_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback, void *user_data)
{
    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    if (!sdio_callback) {
        return HAL_SDIO_STATUS_ERROR;
    }

    msdc_config[sdio_port].msdc_sdio_dma_interrupt_callback = sdio_callback;
    return HAL_SDIO_STATUS_OK;
}

hal_sdio_status_t hal_sdio_set_bus_width(hal_sdio_port_t sdio_port, hal_sdio_bus_width_t bus_width)
{
    sdio_internal_status_t status;
    hal_sdio_command52_config_t command52;
    MSDC_REGISTER_T *msdc_register_base;

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    msdc_register_base = MSDC0_REG;

    command52.direction = HAL_SDIO_DIRECTION_READ;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x07;

    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }

    command52.data = command52.data & (~(SDIO_CCCR_BUS0_BIT_MASK | SDIO_CCCR_BUS1_BIT_MASK));
    /*set bus width and disable card detection*/
    command52.data =  command52.data | SDIO_CCCR_CD_BIT_MASK |
                      ((bus_width == HAL_SDIO_BUS_WIDTH_4) ? SDIO_CCCR_BUS1_BIT_MASK : 0);

    command52.direction = HAL_SDIO_DIRECTION_WRITE;
    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }

    if (bus_width == HAL_SDIO_BUS_WIDTH_4) {
        msdc_register_base->SDC_CFG = (msdc_register_base->SDC_CFG & (~SDC_CFG_BUSWIDTH_MASK)) | (MSDC_SDIO_BUS_WIDTH_4BITS << SDC_CFG_BUSWIDTH_OFFSET);
    } else {
        msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_BUSWIDTH_MASK);
    }

    sdio_information.bus_width = bus_width;

    return HAL_SDIO_STATUS_OK;
error:

    log_hal_msgid_error("sdio error status = %d \r\n", 1, status);

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_init(hal_sdio_port_t sdio_port, hal_sdio_config_t *sdio_config)
{
    sdio_internal_status_t  status;
    uint32_t retry = 0;
    bool is_io_ready = false;
    hal_nvic_status_t irq_status;
    MSDC_REGISTER_T *msdc_register_base;
    uint32_t nvic_mask;

    input_freq = sdio_config->clock;

    msdc_register_base = MSDC0_REG;

    if (NULL == sdio_config) {
        log_hal_msgid_error("parameter error", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

    sdio_information.is_initialized = false;

    if (MSDC_INITIALIZED == msdc_init((msdc_port_t)sdio_port, sdio_config->bus_width)) {
        if (MSDC_OWNER_SD == msdc_get_owner((msdc_port_t)sdio_port)) {
            sdio_information.is_busy = false;
            return HAL_SDIO_STATUS_ERROR;
        }
    }
#ifdef HAL_SLEEP_MANAGER_ENABLED
    sleep_management_register_suspend_callback(SLEEP_BACKUP_RESTORE_MSDC, (sleep_management_suspend_callback_t)sdio_backup_all, NULL);
    sleep_management_register_resume_callback(SLEEP_BACKUP_RESTORE_MSDC, (sleep_management_resume_callback_t)sdio_restore_all, NULL);
#endif

    /*save MSDC owner*/
    msdc_set_owner((msdc_port_t)sdio_port, MSDC_OWNER_SDIO);

    msdc_nvic_set((msdc_port_t)sdio_port, false);

    msdc_reset((msdc_port_t)sdio_port);

    /*disable 4bit*/
    msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG & (~SDC_CFG_BUSWIDTH_MASK);

    msdc_set_output_clock((msdc_port_t)sdio_port, 240); /*setting bus clock to 240KHz.*/

    /*reset data timeout conter to 65536*256 cycles*/
    msdc_register_base->SDC_CFG = msdc_register_base->SDC_CFG | 0xFF000000;

    /*enable SDIO mode*/
    msdc_register_base->SDC_CFG |= SDC_CFG_SDIO_MASK;

    /*enable 74 serial clock*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG | MSDC_CFG_CKPDN_MASK;

    /*enable SDIO interrupt*/
    msdc_register_base->SDC_CFG |= SDC_CFG_SDIOIDE_MASK;

    msdc_wait(10);

    /*disable serial clock*/
    msdc_register_base->MSDC_CFG = msdc_register_base->MSDC_CFG & (~MSDC_CFG_CKPDN_MASK);

    status = sdio_send_command5(sdio_port, COMMAND_NO_ARGUMENT);
    if (status != NO_ERROR) {
        goto error;
    }

    do {
        retry ++;

        status = sdio_send_command5(sdio_port, sdio_information.ocr);

        if (100 < retry) {
            goto error;
        }

    } while (!sdio_information.is_io_ready);

    msdc_set_output_clock((msdc_port_t)sdio_port, MSDC_OUTPUT_CLOCK);

    status = sdio_get_rca(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

    status = sdio_select_card(sdio_port, sdio_information.rca);
    if (status != NO_ERROR) {
        goto error;
    }

    status = sdio_get_cccr(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

    sdio_get_capacity(sdio_port);

    sdio_get_power_control(sdio_port);

    if (!(sdio_is_support_lsc(sdio_port))) {
        if (sdio_is_support_s4mi(sdio_port)) {
            status = sdio_set_e4mi(sdio_port, true);
            if (status != NO_ERROR) {
                goto error;
            }
        }

        if (HAL_SDIO_STATUS_OK != hal_sdio_set_bus_width(sdio_port, sdio_config->bus_width)) {
            msdc_deinit((msdc_port_t)sdio_port);

            return HAL_SDIO_STATUS_ERROR;
        }

    } else if (sdio_is_support_4bls(sdio_port) && (sdio_config->bus_width == HAL_SDIO_BUS_WIDTH_4)) {
        if (HAL_SDIO_STATUS_OK != hal_sdio_set_bus_width(sdio_port, sdio_config->bus_width)) {
            msdc_deinit((msdc_port_t)sdio_port);

            return HAL_SDIO_STATUS_ERROR;
        }
    }

    status =  sdio_get_cis(sdio_port, HAL_SDIO_FUNCTION_0);
    if (status < NO_ERROR) {
        goto error;
    }

    for (retry = 0; retry < 100; retry++) {
        status = sdio_set_io(sdio_port, HAL_SDIO_FUNCTION_1, true);
        if (status < NO_ERROR) {
            goto error;
        }

        status = sdio_check_io_ready(sdio_port, HAL_SDIO_FUNCTION_1, &is_io_ready);
        if (status < NO_ERROR) {
            goto error;
        }

        if (is_io_ready) {
            break;
        }
    }

    status = sdio_get_fbr(sdio_port, HAL_SDIO_FUNCTION_1);
    if (status < NO_ERROR) {
        goto error;
    }

    if (sdio_is_support_mps(sdio_port)) {
        status = sdio_set_mps(sdio_port, true);
        if (status < NO_ERROR) {
            goto error;
        }
    }

    if (sdio_config->clock >= SDIO_DEFAULT_MAX_SPEED) {
        sdio_get_high_speed(sdio_port);
        if (sdio_is_support_shs(sdio_port)) {
            status = sdio_set_ehs(sdio_port, true);
            if (status < NO_ERROR) {
                goto error;
            }
            msdc_set_output_clock((msdc_port_t)sdio_port, sdio_config->clock);
        } else {
            msdc_set_output_clock((msdc_port_t)sdio_port, SDIO_DEFAULT_MAX_SPEED);
        }
    } else {
        msdc_set_output_clock((msdc_port_t)sdio_port, sdio_config->clock);
    }

#ifdef USE_SDIO_DTA1_INT
    /*enable sdio interrupt for function 1*/
    sdio_set_function_interrupt(sdio_port, HAL_SDIO_FUNCTION_1, true);
    /*enable sdio interrupt*/
    msdc_sdio_interrupt_set((msdc_port_t)sdio_port, true);
#endif

    status = sdio_get_cccr(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

    msdc_nvic_set((msdc_port_t)sdio_port, true);

    sdio_information.is_initialized = true ;
    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;

error:
    if (status != NO_ERROR) {
        sdio_information.is_initialized = false;
    }
    sdio_information.is_busy = false;

    log_hal_msgid_error("sdio error status = %d \r\n", 1, status);
    msdc_deinit((msdc_port_t)sdio_port);

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_deinit(hal_sdio_port_t sdio_port)
{
    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    msdc_reset((msdc_port_t)sdio_port);
    memset((void *)(&sdio_information), 0, sizeof(sdio_information_t));

    msdc_deinit((msdc_port_t)sdio_port);

    return HAL_SDIO_STATUS_OK;
}


hal_sdio_status_t hal_sdio_execute_command52(hal_sdio_port_t sdio_port, hal_sdio_command52_config_t *command52_config)
{
    hal_nvic_status_t irq_status;
    sdio_internal_status_t status;
    uint32_t nvic_mask;

    if (NULL == command52_config) {
        log_hal_msgid_error("parameter error", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    log_hal_msgid_info("command52, r/w = %d, is_stop = %x, addr = %x \r\n", 3, command52_config->direction, command52_config->stop, (unsigned int)command52_config->address);

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs before start transfer.*/
    msdc_dvfs_lock_current((msdc_port_t)sdio_port);
#endif

    status = sdio_command52(sdio_port, command52_config);

    if (NO_ERROR != status) {
        sdio_information.is_busy = false;
        log_hal_msgid_error("sdio error status = %d \r\n", 1, (unsigned int)status);

#ifdef HAL_DVFS_MODULE_ENABLED
        /*unlock dvfs before end transfer.*/
        hal_dvfs_lock_level(HAL_DVFS_UNLOCK);
#endif

        return HAL_SDIO_STATUS_ERROR;
    }

    sdio_information.is_busy = false;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs before end transfer.*/
    hal_dvfs_lock_level(HAL_DVFS_UNLOCK);
#endif

    return HAL_SDIO_STATUS_OK;
}

/*********************************************************************************************************************************/
/************************************************CMD53 ***************************************************************************/
/*********************************************************************************************************************************/
/*| 1 | 1 |       6       |  1  |           3            |      1     |    1    |         17       |         9        |  7  | 1 |*/
/*| S | D | Command index | R/W | Numberof I/O functions | Block mode | OP code | register address | byte/block count | CRC | E |*/
/*********************************************************************************************************************************/
hal_sdio_status_t hal_sdio_execute_command53(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t word_count;
    uint32_t *buffer_pointer;
    hal_nvic_status_t irq_status;
    uint32_t block_size;
    uint32_t count;
    uint32_t fifo_count;
    uint32_t interrupte_status;
    MSDC_REGISTER_T *msdc_register_base;
    uint32_t nvic_mask;

    msdc_register_base = MSDC0_REG;

    if (NULL == command53_config) {
        log_hal_msgid_error("parameter error", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs before start transfer.*/
    msdc_dvfs_lock_current((msdc_port_t)sdio_port);
#endif

    msdc_nvic_set((msdc_port_t)sdio_port, false);
    msdc_reset((msdc_port_t)sdio_port);
    msdc_clear_fifo((msdc_port_t)sdio_port);

    interrupte_status = msdc_register_base->MSDC_INT;
    if (0 != interrupte_status) {
        log_hal_msgid_info("interrupt status = %x before send cmd53 \r\n", 1, (unsigned int)interrupte_status);
        msdc_register_base->MSDC_INT |= interrupte_status;
    }

    /*use pio mode*/
    msdc_register_base->MSDC_CFG |= MSDC_CFG_PIO_MASK;

    /***************************************************************/
    /******************config command53 argument********************/
    /***************************************************************/
    /*set R/W bit*/
    argument = (command53_config->direction == HAL_SDIO_DIRECTION_WRITE) ? (argument | COMMAND53_DIRECTION_BIT_MASK) :
               (argument & (~COMMAND53_DIRECTION_BIT_MASK));
    /*set Numberof I/O functions bit*/
    argument = (argument & (~COMMAND53_FUNCTION_NUMBER_BIT_MASK)) |
               ((uint32_t)command53_config->function << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT);
    /*set block mode bit*/
    argument = command53_config->block ? (argument | COMMAND53_BLOCK_MODE_BIT_MASK) : (argument & (~COMMAND53_BLOCK_MODE_BIT_MASK));
    /*set OP code bit*/
    argument = command53_config->operation ? (argument | COMMAND53_OP_BIT_MASK) : (argument & (~COMMAND53_OP_BIT_MASK));
    /*set register address bit*/
    argument = (argument & (~COMMAND53_REGISTER_ADDRESS_BIT_MASK)) |
               ((uint32_t)command53_config->address << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT);
    /*set byte/block count bit*/
    argument = (argument & (~COMMAND53_COUNT_BIT_MASK)) |
               ((uint32_t)command53_config->count << COMMAND53_COUNT_BIT_SHIFT);


    /***************************************************************/
    /******************config command53 command********************/
    /***************************************************************/
    block_size = sdio_information.block_size[command53_config->function];

    command = MSDC_COMMAND53_SDIO;
    command = command53_config->direction ? (command | SDC_CMD_RW_MASK) : (command & (~SDC_CMD_RW_MASK));
    if (command53_config->block) {
        command = (command & (~SDC_CMD_DTYP_MASK)) | (MULTI_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN_MASK)) | (block_size << 16);
    } else {
        command = (command & (~SDC_CMD_DTYP_MASK)) | (SINGLE_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN_MASK)) | (command53_config->count << 16);
    }

    /*set block number.*/
    if (command53_config->block) {
        msdc_register_base->SDC_BLK_NUM = command53_config->count;
    } else {
        msdc_register_base->SDC_BLK_NUM = 1;
    }


    status =  sdio_send_command(sdio_port, command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = msdc_register_base->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    buffer_pointer = (uint32_t *)command53_config->buffer;

    if (command53_config->direction) {
        /*write data*/
        if (command53_config->block) {
            word_count = (uint32_t)(command53_config->count * sdio_information.block_size[command53_config->function] + 3) >> 2;
        } else {
            word_count = (uint32_t)(command53_config->count + 3) >> 2;
        }

        while (word_count) {
            if ((word_count < (MSDC_FIFO_SIZE >> 2)) && (0 == MSDC_TXFIFOCNT(msdc_register_base))) {
                do {
                    msdc_register_base->MSDC_TXDATA = *buffer_pointer++;
                } while (--word_count);
            } else if (word_count >= (MSDC_FIFO_SIZE >> 2) && (0 == MSDC_TXFIFOCNT(msdc_register_base))) {
                count = MSDC_FIFO_SIZE >> 2;
                do {
                    msdc_register_base->MSDC_TXDATA = *buffer_pointer++;
                } while (--count);
                word_count = word_count - (MSDC_FIFO_SIZE >> 2);
            }
        }

    } else {
        /*read data*/
        if (command53_config->block) {
            word_count = (uint32_t)(command53_config->count * sdio_information.block_size[command53_config->function] + 3) >> 2;
        } else {
            word_count = (uint32_t)(command53_config->count + 3) >> 2;
        }

        while (word_count) {
            fifo_count = ((msdc_register_base->MSDC_FIFOCS & MSDC_FIFOCS_RXCNT_MASK) >> 0);
            if ((word_count < (MSDC_FIFO_SIZE >> 2)) && (word_count <= (fifo_count >> 2))) {
                do {
                    *buffer_pointer++ = msdc_register_base->MSDC_RXDATA;
                } while (--word_count);
            } else if ((word_count >= (MSDC_FIFO_SIZE >> 2)) && (MSDC_FIFO_SIZE <= fifo_count)) {
                count = MSDC_FIFO_SIZE >> 2;
                do {
                    *buffer_pointer++ = msdc_register_base->MSDC_RXDATA;
                } while (--count);
                word_count = word_count - (MSDC_FIFO_SIZE >> 2);
            }
        }
    }

    status = sdio_wait_data_ready(sdio_port);
    if (status != NO_ERROR) {
        goto error;
    }

    msdc_nvic_set((msdc_port_t)sdio_port, true);
    msdc_reset((msdc_port_t)sdio_port);

    sdio_information.is_busy = false;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs before end transfer.*/
    hal_dvfs_lock_level(HAL_DVFS_UNLOCK);
#endif

    return HAL_SDIO_STATUS_OK;

error:
    *(volatile uint32_t *)0xA2010060 = *(volatile uint32_t *)0xA2010060 | 0x8000;

    log_hal_msgid_error("sdio error status = %d \r\n", 1, (unsigned int)status);
    if (command53_config->block) {
        sdio_command12_stop(sdio_port);
    }
    sdio_stop(sdio_port);
    msdc_reset((msdc_port_t)sdio_port);
    msdc_nvic_set((msdc_port_t)sdio_port, true);

    sdio_information.is_busy = false;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs before end transfer.*/
    hal_dvfs_lock_level(HAL_DVFS_UNLOCK);
#endif

    return HAL_SDIO_STATUS_ERROR;
}

hal_sdio_status_t hal_sdio_execute_command53_dma_blocking(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t *buffer_pointer;
    hal_nvic_status_t irq_status;
    uint32_t block_size;
    uint32_t interrupte_status;
    MSDC_REGISTER_T *msdc_register_base;
    uint32_t nvic_mask;

    msdc_register_base = MSDC0_REG;

    if (NULL == command53_config) {
        log_hal_msgid_error("parameter error", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    log_hal_msgid_info("command53_dma, r/w = %d, addr = %x, buffer = %x, is_block = %d, count = %x \r\n", 5,
                       command53_config->direction, (unsigned int)command53_config->address, (unsigned int)command53_config->buffer, command53_config->block, (unsigned int)command53_config->count);

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs before start transfer.*/
    msdc_dvfs_lock_current((msdc_port_t)sdio_port);
#endif

    msdc_nvic_set((msdc_port_t)sdio_port, false);
    msdc_reset((msdc_port_t)sdio_port);
    msdc_clear_fifo((msdc_port_t)sdio_port);

    interrupte_status = msdc_register_base->MSDC_INT;
    if (0 != interrupte_status) {
        log_hal_msgid_info("interrupt status = %x before send cmd53 \r\n", 1, (unsigned int)interrupte_status);
        msdc_register_base->MSDC_INT |= interrupte_status;
    }

    /*enable DMA*/
    msdc_register_base->MSDC_CFG &= ~MSDC_CFG_PIO_MASK;

    /***************************************************************/
    /******************config command53 argument********************/
    /***************************************************************/
    /*set R/W bit*/
    argument = (command53_config->direction == HAL_SDIO_DIRECTION_WRITE) ? (argument | COMMAND53_DIRECTION_BIT_MASK) :
               (argument & (~COMMAND53_DIRECTION_BIT_MASK));
    /*set Numberof I/O functions bit*/
    argument = (argument & (~COMMAND53_FUNCTION_NUMBER_BIT_MASK)) |
               ((uint32_t)command53_config->function << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT);
    /*set block mode bit*/
    argument = command53_config->block ? (argument | COMMAND53_BLOCK_MODE_BIT_MASK) : (argument & (~COMMAND53_BLOCK_MODE_BIT_MASK));
    /*set OP code bit*/
    argument = command53_config->operation ? (argument | COMMAND53_OP_BIT_MASK) : (argument & (~COMMAND53_OP_BIT_MASK));
    /*set register address bit*/
    argument = (argument & (~COMMAND53_REGISTER_ADDRESS_BIT_MASK)) |
               ((uint32_t)command53_config->address << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT);
    /*set byte/block count bit*/
    argument = (argument & (~COMMAND53_COUNT_BIT_MASK)) |
               ((uint32_t)command53_config->count << COMMAND53_COUNT_BIT_SHIFT);


    /***************************************************************/
    /******************config command53 command********************/
    /***************************************************************/
    block_size = sdio_information.block_size[command53_config->function];

    command = MSDC_COMMAND53_SDIO;
    command = command53_config->direction ? (command | SDC_CMD_RW_MASK) : (command & (~SDC_CMD_RW_MASK));
    if (command53_config->block) {
        command = (command & (~SDC_CMD_DTYP_MASK)) | (MULTI_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN_MASK)) | (block_size << 16);
    } else {
        command = (command & (~SDC_CMD_DTYP_MASK)) | (SINGLE_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN_MASK)) | (command53_config->count << 16);
    }

    /*set block number.*/
    if (command53_config->block) {
        msdc_register_base->SDC_BLK_NUM = command53_config->count;
    } else {
        msdc_register_base->SDC_BLK_NUM = 1;
    }

    buffer_pointer = (uint32_t *)command53_config->buffer;

    status =  sdio_send_command(sdio_port, command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = msdc_register_base->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    /*use basic DMA mode.*/
    msdc_register_base->DMA_CTRL &= ~MSDC_DMA_CTRL_MODE_MASK;

    msdc_register_base->DMA_CTRL = (msdc_register_base->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ_MASK)) | (MSDC_DMA_BURST_8_BYTES << 12);
    msdc_register_base->DMA_CTRL |= MSDC_DMA_CTRL_LASTBUF_MASK;

    if (command53_config->block) {
        msdc_register_base->DMA_LENGTH = command53_config->count * sdio_information.block_size[command53_config->function];
    } else {
        msdc_register_base->DMA_LENGTH = command53_config->count;
    }

    msdc_register_base->DMA_SA = (uint32_t)buffer_pointer;
    msdc_register_base->DMA_CTRL |= MSDC_DMA_CTRL_START_MASK;

    status = sdio_wait_data_ready(sdio_port);
    if (status != NO_ERROR) {
#ifdef MSDC_DEBUG_FEATURE
        msdc_dump_all_reg((msdc_port_t)sdio_port);
#endif
        goto error;
    }

    msdc_dma_stop((msdc_port_t)sdio_port);
    msdc_reset((msdc_port_t)sdio_port);

    msdc_nvic_set((msdc_port_t)sdio_port, true);
    msdc_reset((msdc_port_t)sdio_port);

    sdio_information.is_busy = false;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs before end transfer.*/
    hal_dvfs_lock_level(HAL_DVFS_UNLOCK);
#endif

    return HAL_SDIO_STATUS_OK;

error:
    *(volatile uint32_t *)0xA2010060 = *(volatile uint32_t *)0xA2010060 | 0x8000;

    log_hal_msgid_error("sdio error status = %d \r\n", 1, status);

    if (command53_config->block) {
        sdio_command12_stop(sdio_port);
    }

    sdio_stop(sdio_port);
    msdc_reset((msdc_port_t)sdio_port);
    msdc_nvic_set((msdc_port_t)sdio_port, true);

    sdio_information.is_busy = false;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs before end transfer.*/
    hal_dvfs_lock_level(HAL_DVFS_UNLOCK);
#endif

    return HAL_SDIO_STATUS_ERROR;
}


hal_sdio_status_t hal_sdio_execute_command53_dma(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    uint32_t *buffer_pointer;
    hal_nvic_status_t irq_status;
    uint32_t block_size;
    uint32_t interrupte_status;
    MSDC_REGISTER_T *msdc_register_base;
    uint32_t nvic_mask;

    msdc_register_base = MSDC0_REG;

    if (NULL == msdc_config[sdio_port].msdc_sdio_dma_interrupt_callback) {
        log_hal_msgid_error("sdio dma interrupt callback is null\r\n", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NULL == command53_config) {
        log_hal_msgid_error("parameter error\r\n", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs before start transfer.*/
    msdc_dvfs_lock_current((msdc_port_t)sdio_port);
#endif

    msdc_nvic_set((msdc_port_t)sdio_port, false);
    msdc_reset((msdc_port_t)sdio_port);
    msdc_clear_fifo((msdc_port_t)sdio_port);

    interrupte_status = msdc_register_base->MSDC_INT;
    if (0 != interrupte_status) {
        log_hal_msgid_error("error, interrupt status = %x before send cmd53 \r\n", 1, (unsigned int)interrupte_status);
        msdc_register_base->MSDC_INT |= interrupte_status;
    }

    /*enable DMA*/
    msdc_register_base->MSDC_CFG &= ~MSDC_CFG_PIO_MASK;

    /***************************************************************/
    /******************config command53 argument********************/
    /***************************************************************/
    /*set R/W bit*/
    argument = (command53_config->direction == HAL_SDIO_DIRECTION_WRITE) ? (argument | COMMAND53_DIRECTION_BIT_MASK) :
               (argument & (~COMMAND53_DIRECTION_BIT_MASK));
    /*set Numberof I/O functions bit*/
    argument = (argument & (~COMMAND53_FUNCTION_NUMBER_BIT_MASK)) |
               ((uint32_t)command53_config->function << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT);
    /*set block mode bit*/
    argument = command53_config->block ? (argument | COMMAND53_BLOCK_MODE_BIT_MASK) : (argument & (~COMMAND53_BLOCK_MODE_BIT_MASK));
    /*set OP code bit*/
    argument = command53_config->operation ? (argument | COMMAND53_OP_BIT_MASK) : (argument & (~COMMAND53_OP_BIT_MASK));
    /*set register address bit*/
    argument = (argument & (~COMMAND53_REGISTER_ADDRESS_BIT_MASK)) |
               ((uint32_t)command53_config->address << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT);
    /*set byte/block count bit*/
    argument = (argument & (~COMMAND53_COUNT_BIT_MASK)) |
               ((uint32_t)command53_config->count << COMMAND53_COUNT_BIT_SHIFT);


    /***************************************************************/
    /******************config command53 command********************/
    /***************************************************************/
    block_size = sdio_information.block_size[command53_config->function];

    command = MSDC_COMMAND53_SDIO;
    command = command53_config->direction ? (command | SDC_CMD_RW_MASK) : (command & (~SDC_CMD_RW_MASK));
    if (command53_config->block) {
        command = (command & (~SDC_CMD_DTYP_MASK)) | (MULTI_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN_MASK)) | (block_size << 16);
    } else {
        command = (command & (~SDC_CMD_DTYP_MASK)) | (SINGLE_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN_MASK)) | (command53_config->count << 16);
    }

    /*set block number.*/
    if (command53_config->block) {
        msdc_register_base->SDC_BLK_NUM = command53_config->count;
    } else {
        msdc_register_base->SDC_BLK_NUM = 1;
    }

    buffer_pointer = (uint32_t *)command53_config->buffer;

    status =  sdio_send_command(sdio_port, command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = msdc_register_base->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    /*use basic DMA mode.*/
    msdc_register_base->DMA_CTRL &= ~MSDC_DMA_CTRL_MODE_MASK;
    msdc_register_base->DMA_CTRL = (msdc_register_base->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ_MASK)) | (MSDC_DMA_BURST_8_BYTES << 12);
    msdc_register_base->DMA_CTRL |= MSDC_DMA_CTRL_LASTBUF_MASK;
    if (command53_config->block) {
        msdc_register_base->DMA_LENGTH = command53_config->count * sdio_information.block_size[command53_config->function];
    } else {
        msdc_register_base->DMA_LENGTH = command53_config->count;
    }
    msdc_register_base->DMA_SA = (uint32_t)buffer_pointer;

    /*MSDC interrupt config*/
    msdc_register_base->MSDC_INTEN |= MSDC_DAT_INTS_EN | MSDC_INTEN_DXFER_DONE_MASK;
    msdc_nvic_set((msdc_port_t)sdio_port, true);

    msdc_register_base->DMA_CTRL |= MSDC_DMA_CTRL_START_MASK;

    return HAL_SDIO_STATUS_OK;

error:

    *(volatile uint32_t *)0xA2010060 = *(volatile uint32_t *)0xA2010060 | 0x8000;

    log_hal_msgid_error("sdio error status = %d \r\n", 1, status);

    if (command53_config->block) {
        sdio_command12_stop(sdio_port);
    }

    sdio_stop(sdio_port);
    msdc_reset((msdc_port_t)sdio_port);
    msdc_nvic_set((msdc_port_t)sdio_port, true);

    sdio_information.is_busy = false;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs before end transfer.*/
    hal_dvfs_lock_level(HAL_DVFS_UNLOCK);
#endif

    return HAL_SDIO_STATUS_ERROR;

}

hal_sdio_status_t hal_sdio_execute_command53_dma_gpd(hal_sdio_port_t sdio_port, hal_sdio_command53_config_t *command53_config)
{
    sdio_internal_status_t status;
    uint32_t argument = 0;
    uint32_t command;
    uint32_t response;
    hal_nvic_status_t irq_status;
    uint32_t block_size;
    MSDC_REGISTER_T *msdc_register_base;
    uint32_t nvic_mask;

    msdc_register_base = MSDC0_REG;

    if (NULL == command53_config) {
        log_hal_msgid_error("parameter error", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    log_hal_msgid_info("command53_dma, r/w = %d, addr = %x, buffer = %x, is_block = %d, count = %x \r\n", 5,
                       command53_config->direction, (unsigned int)command53_config->address, (unsigned int)command53_config->buffer, command53_config->block, (unsigned int)command53_config->count);

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

#ifdef HAL_DVFS_MODULE_ENABLED
    /*lock dvfs before start transfer.*/
    msdc_dvfs_lock_current((msdc_port_t)sdio_port);
#endif

    msdc_nvic_set((msdc_port_t)sdio_port, false);
    msdc_reset((msdc_port_t)sdio_port);
    msdc_clear_fifo((msdc_port_t)sdio_port);

    msdc_register_base->DMA_CTRL = (msdc_register_base->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ_MASK)) | (MSDC_DMA_BURST_8_BYTES << 12);

    /*enable DMA*/
    msdc_register_base->MSDC_CFG &= ~MSDC_CFG_PIO_MASK;

    /***************************************************************/
    /******************config command53 argument********************/
    /***************************************************************/
    /*set R/W bit*/
    argument = (command53_config->direction == HAL_SDIO_DIRECTION_WRITE) ? (argument | COMMAND53_DIRECTION_BIT_MASK) :
               (argument & (~COMMAND53_DIRECTION_BIT_MASK));
    /*set Numberof I/O functions bit*/
    argument = (argument & (~COMMAND53_FUNCTION_NUMBER_BIT_MASK)) |
               ((uint32_t)command53_config->function << COMMAND53_FUNCTION_NUMBER_BIT_SHIFT);
    /*set block mode bit*/
    argument = command53_config->block ? (argument | COMMAND53_BLOCK_MODE_BIT_MASK) : (argument & (~COMMAND53_BLOCK_MODE_BIT_MASK));
    /*set OP code bit*/
    argument = command53_config->operation ? (argument | COMMAND53_OP_BIT_MASK) : (argument & (~COMMAND53_OP_BIT_MASK));
    /*set register address bit*/
    argument = (argument & (~COMMAND53_REGISTER_ADDRESS_BIT_MASK)) |
               ((uint32_t)command53_config->address << COMMAND53_REGISTER_ADDRESS_BIT_SHIFT);
    /*set byte/block count bit*/
    argument = (argument & (~COMMAND53_COUNT_BIT_MASK)) |
               ((uint32_t)command53_config->count << COMMAND53_COUNT_BIT_SHIFT);


    /***************************************************************/
    /******************config command53 command********************/
    /***************************************************************/
    block_size = sdio_information.block_size[command53_config->function];

    command = MSDC_COMMAND53_SDIO;
    command = command53_config->direction ? (command | SDC_CMD_RW_MASK) : (command & (~SDC_CMD_RW_MASK));
    if (command53_config->block) {
        command = (command & (~SDC_CMD_DTYP_MASK)) | (MULTI_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN_MASK)) | (block_size << 16);
    } else {
        command = (command & (~SDC_CMD_DTYP_MASK)) | (SINGLE_BLOCK << 11);
        command = (command & (~SDC_CMD_BLKLEN_MASK)) | (command53_config->count << 16);
    }

    /*set block number.*/
    if (command53_config->block) {
        msdc_register_base->SDC_BLK_NUM = command53_config->count;
    } else {
        msdc_register_base->SDC_BLK_NUM = 1;
    }

    status =  sdio_send_command(sdio_port, command, argument);
    if (status != NO_ERROR) {
        goto error;
    }

    response = msdc_register_base->SDC_RESP0;

    sdio_information.response = (response & SDIO_R5_RESPONSE_FLAG_BIT_MASK) >> SDIO_R5_RESPONSE_FLAG_BIT_SHIFT;

    if (sdio_information.response & SDIO_R5_RESPONSE_FLAG_ERROR_BIT_MASK) {
        goto error;
    }

    memset(&gpd[0], 0, (sizeof(gpd_t) * 2));
    memset(&bd, 0, sizeof(bd_t));

    /* Config GPD & BD descriptors */
    gpd[0].intr     = 0;   /* donnot generate interrupt */
    gpd[0].extlen   = 0;   /* ignore cmd,arg,etc. */
    gpd[0].hwo      = 1;   /* config hardward owner */
    gpd[0].bdp      = 1;   /* use buffer descriptor list */
    gpd[0].ptr      = (void *)(&bd); /* physical addr */
    gpd[0].next     = NULL;
    gpd[0].chksum   = msdc_cal_checksum(&gpd[0], 16);

    bd.eol     = 1;
    bd.next    = NULL;
    bd.ptr     = (void *)command53_config->buffer;
    bd.buflen   = command53_config->block ? (command53_config->count * block_size) : command53_config->count;
    bd.blkpad   = 0;
    bd.dwpad    = 0;
    bd.chksum    = msdc_cal_checksum(&bd, 16);

    /* enable Descriptor's checksum */
    msdc_register_base->DMA_CFG |= MSDC_DMA_CFG_DECSEN_MASK;

    /* descriptor mode and burst size=64Bytes */
    msdc_register_base->DMA_CTRL = (msdc_register_base->DMA_CTRL & (~MSDC_DMA_CTRL_BRUSTSZ_MASK)) | (MSDC_DMA_BURST_64_BYTES << 12);
    msdc_register_base->DMA_CTRL |= MSDC_DMA_CTRL_MODE_MASK;

    /* write DMA start address to GPD's physical address */
    msdc_register_base->DMA_SA = (uint32_t)(&gpd[0]);
    msdc_register_base->DMA_CTRL |= MSDC_DMA_CTRL_START_MASK;

    status = sdio_wait_data_ready(sdio_port);
    if (status != NO_ERROR) {
#ifdef MSDC_DEBUG_FEATURE
        msdc_dump_all_reg((msdc_port_t)sdio_port);
#endif
        goto error;
    }

    msdc_dma_stop((msdc_port_t)sdio_port);
    msdc_reset((msdc_port_t)sdio_port);

    msdc_nvic_set((msdc_port_t)sdio_port, true);
    msdc_reset((msdc_port_t)sdio_port);

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;

error:
    *(volatile uint32_t *)0xA2010060 = *(volatile uint32_t *)0xA2010060 | 0x8000;

    log_hal_msgid_error("sdio error status = %d \r\n", 1, status);

    if (command53_config->block) {
        sdio_command12_stop(sdio_port);
    }

    sdio_stop(sdio_port);
    msdc_reset((msdc_port_t)sdio_port);
    msdc_nvic_set((msdc_port_t)sdio_port, true);

    sdio_information.is_busy = false;

#ifdef HAL_DVFS_MODULE_ENABLED
    /*unlock dvfs before end transfer.*/
    hal_dvfs_lock_level(HAL_DVFS_UNLOCK);
#endif

    return HAL_SDIO_STATUS_ERROR;
}

hal_sdio_status_t hal_sdio_set_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t block_size)
{
    sdio_internal_status_t status;
    hal_sdio_command52_config_t command52;
    hal_nvic_status_t irq_status;
    uint32_t nvic_mask;

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

    command52.direction = HAL_SDIO_DIRECTION_WRITE;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x10 + function * 0x100;
    command52.data = block_size & 0xff;
    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }

    if (block_size >= 256) {
        command52.address = 0x11 + function * 0x100;
        command52.data = (block_size & 0xff00) >> 8;
        status = sdio_command52(sdio_port, &command52);
        if (status != NO_ERROR) {
            goto error;
        }
    }

    sdio_information.block_size[function] = block_size;

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;
error:
    sdio_information.is_busy = false;
    log_hal_msgid_error("sdio error status = %d \r\n", 1, status);

    return HAL_SDIO_STATUS_ERROR;
}

hal_sdio_status_t hal_sdio_get_block_size(hal_sdio_port_t sdio_port, hal_sdio_function_id_t function, uint32_t *block_size)
{
    sdio_internal_status_t status;
    hal_sdio_command52_config_t command52;
    uint32_t temp = 0;
    uint32_t temp1 = 0;
    hal_nvic_status_t irq_status;
    uint32_t nvic_mask;

    if (NULL == block_size) {
        log_hal_msgid_error("parameter error", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

    command52.direction = HAL_SDIO_DIRECTION_READ;
    command52.function = HAL_SDIO_FUNCTION_0;
    command52.read_after_write = false;
    command52.stop = false;
    command52.address = 0x10 + function * 0x100;
    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }

    temp = command52.data;

    command52.address = 0x11 + function * 0x100;
    status = sdio_command52(sdio_port, &command52);
    if (status != NO_ERROR) {
        goto error;
    }
    temp1 = command52.data;
    temp = temp | (temp1 << 8);

    if (temp == sdio_information.block_size[function]) {
        *block_size = temp;
    } else {
        status = ERROR_INVALID_BLOCK_LENGTH;
        goto error;
    }

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;
error:
    sdio_information.is_busy = false;
    log_hal_msgid_error("sdio error status = %d \r\n", 1, status);

    return HAL_SDIO_STATUS_ERROR;
}

hal_sdio_status_t hal_sdio_set_clock(hal_sdio_port_t sdio_port, uint32_t clock)
{
    hal_nvic_status_t irq_status;
    uint32_t nvic_mask;

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    irq_status = hal_nvic_save_and_set_interrupt_mask(&nvic_mask);
    if (sdio_information.is_busy) {
        hal_nvic_restore_interrupt_mask(nvic_mask);
        return HAL_SDIO_STATUS_BUSY;
    } else {
        sdio_information.is_busy = true;
    }
    hal_nvic_restore_interrupt_mask(nvic_mask);

    msdc_set_output_clock((msdc_port_t)sdio_port, clock);

    sdio_information.is_busy = false;

    return HAL_SDIO_STATUS_OK;
}


hal_sdio_status_t hal_sdio_get_clock(hal_sdio_port_t sdio_port, uint32_t *clock)
{
    if (NULL == clock) {
        log_hal_msgid_error("parameter error", 0);
        return HAL_SDIO_STATUS_ERROR;
    }

    if (NO_ERROR != sdio_check_msdc_port_valid(sdio_port)) {
        return HAL_SDIO_STATUS_ERROR;
    }

    *clock = msdc_get_output_clock((msdc_port_t)sdio_port);

    return HAL_SDIO_STATUS_OK;
}

#ifdef HAL_SDIO_FEATURE_DATA1_IRQ
hal_sdio_status_t hal_sdio_interrupt_register_callback(hal_sdio_port_t sdio_port, hal_sdio_callback_t sdio_callback, void *user_data)
{
    if (!sdio_callback) {
        return HAL_SDIO_STATUS_ERROR;
    }

    msdc_config[(msdc_port_t)sdio_port].msdc_sdio_interrupt_callback = sdio_callback;
    return HAL_SDIO_STATUS_OK;
}
#endif

#endif /*HAL_SDIO_MODULE_ENABLED*/

