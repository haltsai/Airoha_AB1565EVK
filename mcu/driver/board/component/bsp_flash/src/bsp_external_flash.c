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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

/* hal includes */
#include "hal_spi_master.h"
#include "hal_log.h"
#include "hal_gpt.h"
#include "bsp_external_flash.h"
#include "bsp_external_flash_config.h"
#include "bsp_external_flash_internal.h"
#include "memory_attribute.h"
#if defined(AB1558) || defined(AB1556)  || defined(AB1552)
#include "ab155x.h"
#include "hal_gpio.h"
#endif

#ifdef BSP_EXTERNAL_SERIAL_FLASH_ENABLED

#if defined(AB1558) || defined(AB1556)
#define SPI_EXT_FLASH_SCK_PIN    HAL_GPIO_82
#define SPI_EXT_FLASH_MOSI_PIN   HAL_GPIO_80
#define SPI_EXT_FLASH_MISO_PIN   HAL_GPIO_83
#define SPI_EXT_FLASH_SIO2_PIN   HAL_GPIO_81
#define SPI_EXT_FLASH_SIO3_PIN   HAL_GPIO_84
#define SPI_EXT_FLASH_CS_PIN     HAL_GPIO_85

#define SPI_EXT_FLASH_SCK_GPIO_MODE    1
#define SPI_EXT_FLASH_MOSI_GPIO_MODE   1
#define SPI_EXT_FLASH_MISO_GPIO_MODE   1
#define SPI_EXT_FLASH_SIO2_GPIO_MODE   1
#define SPI_EXT_FLASH_SIO3_GPIO_MODE   1
#define SPI_EXT_FLASH_CS_GPIO_MODE     1
#elif defined(AB1552)
#define SPI_EXT_FLASH_SCK_PIN    HAL_GPIO_78
#define SPI_EXT_FLASH_MOSI_PIN   HAL_GPIO_75
#define SPI_EXT_FLASH_MISO_PIN   HAL_GPIO_74
#define SPI_EXT_FLASH_SIO2_PIN   HAL_GPIO_79
#define SPI_EXT_FLASH_SIO3_PIN   HAL_GPIO_73
#define SPI_EXT_FLASH_CS_PIN     HAL_GPIO_77

#define SPI_EXT_FLASH_SCK_GPIO_MODE    2
#define SPI_EXT_FLASH_MOSI_GPIO_MODE   2
#define SPI_EXT_FLASH_MISO_GPIO_MODE   2
#define SPI_EXT_FLASH_SIO2_GPIO_MODE   2
#define SPI_EXT_FLASH_SIO3_GPIO_MODE   2
#define SPI_EXT_FLASH_CS_GPIO_MODE     2
#endif

#ifndef min
#define min(_a, _b)    (((_a)<(_b))?(_a):(_b))
#endif

#ifndef max
#define max(_a, _b)    (((_a)>(_b))?(_a):(_b))
#endif

/* HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE includes the sent CMD size and received data size
   so it can recieve 32 - 4 (read cmd + 3 byte adress)
*/
#define SPI_EXT_FLASH_MAX_READ_SIZE_WITH_POLLING  (HAL_SPI_MAXIMUM_POLLING_TRANSACTION_SIZE - 4)
hal_spi_master_port_t spi_master_port = SPIM_PORT;
int32_t spi_flash_init = 0;
static uint32_t QE_bit_status;
static flsh_device_info_t flash_device_info;
const flsh_device_info_t *support_flash_device = NULL;

#ifdef __EXT_BOOTLOADER__
static uint8_t cmd_buffer[COMMAND_LENGTH];
static uint8_t non_cache_buffer[FLASH_DADA_MAX_LENGTH + COMMAND_LENGTH];
#else
ATTR_ZIDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN static uint8_t cmd_buffer[COMMAND_LENGTH];
ATTR_ZIDATA_IN_NONCACHED_SYSRAM_16BYTE_ALIGN static uint8_t non_cache_buffer[FLASH_DADA_MAX_LENGTH + COMMAND_LENGTH];
#endif

/* for test */
static uint32_t debug_info_enable = 0;
uint32_t gpt_count_num = 0;
uint32_t gpt_cur_count = 0;

const static flsh_device_info_t support_flash_list[] = {
    /* name         JEDEC ID  Page size       */
    {"DEF_FLASH",   0xFFFFFFFF,  256, 4096,     0x00,  0,  0x00,  0,    0, 0,    0, 0,    0,    0, 0,  0,  0},
    {"W25Q32FW",    0x00EF6016,  256, 8192 * 2, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x35, 0x31 , 2, 0,  0},
    {"W25Q32JV",    0x00EF4016,  256, 8192 * 2, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x35, 0x31 , 2, 0,  0}, /*3.3V*/
    {"W25Q80DV",    0x00EF4014,  256, 2048 * 2, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x35, 0x1 ,  2, 0,  0},
    {"W25Q64FW",    0x00EF6017,  256, 8192 * 4, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x35, 0x31 , 2, 0,  0},
    {"W25Q64JV",    0x00EF4017,  256, 8192 * 4, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x35, 0x31 , 2, 0,  0}, /*3.3V*/
    {"W25Q128FW",   0x00EF6018,  256, 8192 * 8, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x35, 0x31 , 2, 0,  0},
    {"IS25WP32",    0x009D7016,  256, 8192 * 2, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x5,  0x1 , 6,  0,  0},
    {"IS25WP64",    0x009D7017,  256, 8192 * 4, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x5,  0x1 , 6,  0,  0},
    {"IS25WP128",   0x009D7018,  256, 8192 * 8, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x5,  0x1 , 6,  0,  0},
    {"IS25LP032D",  0x009D6016,  256, 8192 * 2, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x5,  0x1 , 6,  0,  0},
    {"25LP64",      0x009D6017,  256, 8192 * 4, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x5,  0x1 , 6,  0,  0},
    {"25LP128",     0x009D6018,  256, 8192 * 8, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x5,  0x1 , 6,  0,  0},
    {"KH25L12833F", 0x00C22018,  256, 8192 * 8, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x5,  0x1 , 6,  0,  0},
    {"KH25L6436F",  0x00C22017,  256, 8192 * 4, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x5,  0x1 , 6,  0,  0},
    {"GD25Q32C",    0x00C86016,  256, 8192 * 2, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x35, 0x31 , 2, 0,  0},
    {"GD25Q32C",    0x00C84016,  256, 8192 * 2, 0x0B,  8,  0x6B,  8, 0x02, 0, 0x32, 0, 0x35, 0x31 , 2, 0,  0}
};


static void set_QE_bit_status(uint32_t status)
{
    QE_bit_status = status;
}

uint32_t get_flash_size(void)
{
    uint32_t size;
    if (support_flash_device != NULL && support_flash_device->jedec_id != 0xFFFFFFFF) {
        size = support_flash_device->page_size * support_flash_device->n_pages;
    } else {
        size = 0;
    }
    return size;
}

static void caculate_read_delay_cycle(void)
{
    if (flash_device_info.frd_delay == 8) {
        //delay 8 cycles + 1 bytes command + 3 bytes address cycle
        flash_device_info.offset = 1 + 4;
    } else {
        // 1 bytes command + 3 bytes address cycle
        flash_device_info.offset = 4;
    }


    if (flash_device_info.qfrd_delay == 8) {
        //delay 8 cycles + 1 bytes command + 3 bytes address cycle
        flash_device_info.q_offset = 1 + 4;
    } else {
        // 1 bytes command + 3 bytes address cycle
        flash_device_info.q_offset = 4;
    }
}

static flash_status_t read_status_register(uint32_t command, uint8_t *sr_status)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_buffer;
    uint8_t buff[2] = {0, 0};
    uint32_t cmd_indx;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    cmd_indx = 0;
    cmd[cmd_indx++] = command;
    spi_send_and_receive_config.receive_buffer = buff;
    spi_send_and_receive_config.send_data = cmd;
    spi_send_and_receive_config.send_length = cmd_indx;
    spi_send_and_receive_config.receive_length = spi_send_and_receive_config.send_length + 0x1;
    //should mind: status is at buffer[cmd_indx] for SPI driver
    status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config);

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_msgid_error("hal_spi_master_send_polling fail\n", 0);
        return FLASH_BUSY;
    }
    *sr_status = buff[cmd_indx];
    return status;
}

static flash_status_t write_status_register(uint32_t command, uint8_t sr)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_buffer;
    uint32_t cmd_indx;

    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    cmd_indx = 0;
    cmd[cmd_indx++] = command;
    cmd[cmd_indx++] = sr;
    status = hal_spi_master_send_polling(spi_master_port, cmd, cmd_indx);

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_msgid_error("hal_spi_master_send_polling fail\n", 0);
        return FLASH_BUSY;
    }
    return FLASH_STATUS_IDLE;
}

static flash_status_t write_status_register_2(uint32_t command, uint8_t sr, uint8_t sr2)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_buffer;
    uint32_t cmd_indx;

    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    cmd_indx = 0;
    cmd[cmd_indx++] = command;
    cmd[cmd_indx++] = sr;
    cmd[cmd_indx++] = sr2;
    status = hal_spi_master_send_polling(spi_master_port, cmd, cmd_indx);

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_msgid_error("hal_spi_master_send_polling fail\n", 0);
        return FLASH_BUSY;
    }
    return FLASH_STATUS_IDLE;
}


static flash_status_t spi_external_flash_get_status(void)
{
    uint8_t sr;

    if (spi_flash_init != FLASH_INIT) {
        return FLASH_NOT_INIT;
    }

    read_status_register(READ_SR, &sr);
    if ((sr & FLASH_BUSY) == FLASH_BUSY) {
        return FLASH_STATUS_BUSY;
    } else {
        return FLASH_STATUS_IDLE;
    }
}

static flash_status_t wait_flash_ready(uint32_t ms)
{
    uint32_t count = 0;
    int32_t status = FLASH_NOT_INIT;

    if (spi_flash_init != FLASH_INIT) {
        return status;
    }

    while (1) {
        if ((spi_external_flash_get_status() & FLASH_BUSY) == FLASH_BUSY) {
            hal_gpt_delay_ms(1);
            count++;
            if (count > ((ms + 1) * 1000)) {
                log_hal_msgid_error("\r\n Flash is busy over time!!! \r\n", 0);
                return FLASH_STATUS_BUSY;
            }
        } else {
            return FLASH_STATUS_IDLE;
        }
    }
}

static uint32_t enable_QE_bit(void)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t sr;
    uint8_t *cmd = cmd_buffer;
    uint32_t cmd_indx;

    //write enable
    cmd_indx = 0;
    cmd[cmd_indx++] = WRITE_ENABLE;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, cmd, cmd_indx)) {
        log_hal_msgid_error("\r\n hal_spi_master_send_polling fail\r\n", 0);
    }
    wait_flash_ready(1);

    if (support_flash_device != NULL) {
        if ((support_flash_device->jedec_id & 0xEF6000) == 0xEF6000) {
            read_status_register(READ_SR_2, &sr);
            if (sr & QE_ENABLE_WB) {
                //QE was enable
                set_QE_bit_status(1);
                return EXTERNAL_FLASH_STATUS_OK;
            } else {
                //Write QE bit
                status = write_status_register(WRITE_SR_2, QE_ENABLE_WB);
                //read back and check
                sr = 0;
                read_status_register(READ_SR_2, &sr);
                if ((status == FLASH_STATUS_IDLE) && (sr & QE_ENABLE_WB)) {
                    set_QE_bit_status(1);
                } else {
                    log_hal_msgid_error("\r\n set QE enable failed\r\n", 0);
                }
            }
        } else if ((support_flash_device->jedec_id & 0xEF4000) == 0xEF4000) {
            read_status_register(READ_SR_2, &sr);
            if (sr & QE_ENABLE_WB) {
                //QE was enable
                set_QE_bit_status(1);
                return EXTERNAL_FLASH_STATUS_OK;
            } else {
                uint8_t sr_1;
                read_status_register(READ_SR, &sr_1);
                //Write QE bit
                status = write_status_register_2(WRITE_SR, sr_1, QE_ENABLE_WB);
                //read back and check
                sr = 0;
                read_status_register(READ_SR_2, &sr);
                if ((status == FLASH_STATUS_IDLE) && (sr & QE_ENABLE_WB)) {
                    set_QE_bit_status(1);
                } else {
                    log_hal_msgid_error("\r\n set QE enable failed\r\n", 0);
                }
            }
        }
        else if (((support_flash_device->jedec_id & 0x9D7000) == 0x9D7000) ||
                  ((support_flash_device->jedec_id & 0x9D6000) == 0x9D6000)) {
            read_status_register(READ_SR, &sr);
            if (sr & QE_ENABLE) {
                //QE was enable
                set_QE_bit_status(1);
                return EXTERNAL_FLASH_STATUS_OK;
            } else {
                //Write QE bit
                status = write_status_register(WRITE_SR, QE_ENABLE);
                //read back and check
                sr = 0;
                read_status_register(READ_SR, &sr);
                if ((status == FLASH_STATUS_IDLE) && (sr & QE_ENABLE)) {
                    set_QE_bit_status(1);
                } else {
                    log_hal_msgid_error("\r\n set QE enable failed\r\n", 0);
                }
            }
        } else if ((support_flash_device->jedec_id & 0xC22000) == 0xC22000) {
            read_status_register(READ_SR, &sr);
            if (sr & QE_ENABLE) {
                //QE was enable
                set_QE_bit_status(1);
                return EXTERNAL_FLASH_STATUS_OK;
            } else {
                //Write QE bit
                status = write_status_register(WRITE_SR, QE_ENABLE);
                //read back and check
                sr = 0;
                read_status_register(READ_SR, &sr);
                if ((status == FLASH_STATUS_IDLE) && (sr & QE_ENABLE)) {
                    set_QE_bit_status(1);
                } else {
                    log_hal_msgid_error("\r\n set QE enable failed\r\n", 0);
                }
            }
        } else if (((support_flash_device->jedec_id & 0xC86000) == 0xC86000) ||
                   ((support_flash_device->jedec_id & 0xC84000) == 0xC84000)) {
            read_status_register(READ_SR_2, &sr);
            if (sr & QE_ENABLE_WB) {
                //QE was enable
                set_QE_bit_status(1);
                return EXTERNAL_FLASH_STATUS_OK;
            } else {
                //Write QE bit
                status = write_status_register(WRITE_SR_2, QE_ENABLE_WB);
                //read back and check
                sr = 0;
                read_status_register(READ_SR_2, &sr);
                if ((status == FLASH_STATUS_IDLE) && (sr & QE_ENABLE_WB)) {
                    set_QE_bit_status(1);
                } else {
                    log_hal_msgid_error("\r\n set QE enable failed\r\n", 0);
                }
            }
        }
    }
    return EXTERNAL_FLASH_STATUS_ERROR;
}

bsp_external_flash_status_t bsp_external_flash_get_rdid(uint8_t *buffer)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_buffer;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;
    uint32_t cmd_indx;

    if (spi_flash_init != FLASH_INIT) {
        return EXTERNAL_FLASH_STATUS_NOT_INIT;
    }

    if (wait_flash_ready(1)) {
        log_hal_msgid_info("\r\n flash is busy! \r\n", 0);
        return EXTERNAL_FLASH_STATUS_BUSY;
    }

    //write enable
    cmd_indx = 0;
    cmd[cmd_indx++] = READ_RDID;
    //log_hal_msgid_info("[Flash Status] read flash RDID start \r\n", 0);
    spi_send_and_receive_config.receive_buffer = buffer;
    spi_send_and_receive_config.send_data = cmd;
    spi_send_and_receive_config.send_length = cmd_indx;
    spi_send_and_receive_config.receive_length = spi_send_and_receive_config.send_length + 0x3;

    status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config);

    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_msgid_error("hal_spi_master_send_polling fail\n", 0);
        return EXTERNAL_FLASH_STATUS_ERROR;
    }

    if (FLASH_STATUS_BUSY == wait_flash_ready(1)) {
        log_hal_msgid_error("\r\n flash is busy before read! \r\n", 0);
        return EXTERNAL_FLASH_STATUS_BUSY;
    }
    return EXTERNAL_FLASH_STATUS_OK;
}

/**
 * @brief This function do initialization for serial flash and SPI Master.
 * @param[in] master_port:  used SPI master port.
 * @param[in] frequency: SPI Mater frequency, max is 52Mhz.
 * @param[in] length: read data length
 */
bsp_external_flash_status_t bsp_external_flash_init(hal_spi_master_port_t master_port, uint32_t frequency)
{
    //flash reset command
    uint8_t reset_cmd[2] = {EANBLE_RESET, RESET_DEVICE};
    //for 2523 max is 13Mhz
    uint32_t freq = frequency;
    hal_spi_master_config_t spi_config;
    hal_spi_master_status_t status;
    hal_spi_master_advanced_config_t advanced_config;

    if (spi_flash_init == FLASH_INIT) {
        return EXTERNAL_FLASH_STATUS_OK;
    }

#if defined(AB1558) || defined(AB1556)  || defined(AB1552)
    hal_pinmux_set_function(SPI_EXT_FLASH_SCK_PIN, SPI_EXT_FLASH_SCK_GPIO_MODE);
    hal_pinmux_set_function(SPI_EXT_FLASH_MOSI_PIN, SPI_EXT_FLASH_MOSI_GPIO_MODE);
    hal_pinmux_set_function(SPI_EXT_FLASH_MISO_PIN, SPI_EXT_FLASH_MISO_GPIO_MODE);
    hal_pinmux_set_function(SPI_EXT_FLASH_CS_PIN, SPI_EXT_FLASH_CS_GPIO_MODE);
#ifdef SPI_FLASH_QUAD_MODE
    hal_pinmux_set_function(SPI_EXT_FLASH_SIO2_PIN, SPI_EXT_FLASH_SIO2_GPIO_MODE);
    hal_pinmux_set_function(SPI_EXT_FLASH_SIO3_PIN, SPI_EXT_FLASH_SIO3_GPIO_MODE);
#endif
#endif

    //configure the right SPI
    spi_master_port = master_port;
    spi_config.bit_order = HAL_SPI_MASTER_MSB_FIRST;
    spi_config.slave_port = HAL_SPI_MASTER_SLAVE_0;
    spi_config.clock_frequency = freq;
    spi_config.phase = HAL_SPI_MASTER_CLOCK_PHASE0;
    spi_config.polarity = HAL_SPI_MASTER_CLOCK_POLARITY0;
    hal_spi_master_deinit(master_port);
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_init(spi_master_port, &spi_config)) {
        log_hal_msgid_error("\r\n hal_spi_master_init fail\r\n", 0);
        return EXTERNAL_FLASH_STATUS_ERROR;
    }

    //set tick delay with HQA result
    advanced_config.byte_order = HAL_SPI_MASTER_LITTLE_ENDIAN;       //default setting
    advanced_config.chip_polarity = HAL_SPI_MASTER_CHIP_SELECT_LOW;  //default setting
    //should set with SQA result
    advanced_config.get_tick = SPIM_TICK_DELAY;
    advanced_config.sample_select = HAL_SPI_MASTER_SAMPLE_POSITIVE;  //default setting
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_set_advanced_config(spi_master_port, &advanced_config)) {
        log_hal_msgid_error("\r\n hal_spi_master_set_advanced_config fail\r\n", 0);
        return (status = EXTERNAL_FLASH_STATUS_ERROR);
    }

    //hal_spi_master_set_macro_selection(spi_master_port,HAL_SPI_MASTER_MACRO_GROUP_B);

    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, reset_cmd, 2)) {
        log_hal_msgid_error("\r\n hal_spi_master_send_polling fail\r\n", 0);
        return (status = EXTERNAL_FLASH_STATUS_ERROR);
    }

    spi_flash_init = FLASH_INIT;

    //check supported flash exist
    {
        uint8_t flash_id[4];
        uint8_t i;
        uint32_t jedecid;

        bsp_external_flash_get_rdid(flash_id);
        log_hal_msgid_info("flash id: %x, %x, %x \r\n", 3, flash_id[1], flash_id[2], flash_id[3]);

        jedecid = (flash_id[1] << 16) | (flash_id[2] << 8) | flash_id[3];
        for (i = 1; i < (sizeof(support_flash_list) / sizeof(support_flash_list[0])); i++) {
            if (support_flash_list[i].jedec_id == jedecid) {
                support_flash_device = &support_flash_list[i];
                memcpy(&flash_device_info, &support_flash_list[i], sizeof(flsh_device_info_t));
            }
        }

        if (support_flash_device == NULL) {
            log_hal_msgid_error("\r\n Can't find supported device.\r\n", 0);
            return (status = EXTERNAL_FLASH_ERROR_NOT_FOUND_DEVICE);
        }
    }

    //enable Q bit
    enable_QE_bit();
    //caculate delay cycles for read
    caculate_read_delay_cycle();

    return (status = EXTERNAL_FLASH_STATUS_OK);
}

/*
   should do de-initiation when not use it
*/
bsp_external_flash_status_t bsp_external_flash_deinit(void)
{
    int32_t status = FLASH_NOT_INIT;
    if (spi_flash_init != FLASH_INIT) {
        return (status = EXTERNAL_FLASH_STATUS_NOT_INIT);
    }

    /* De-init spi master */
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_deinit(spi_master_port)) {
        log_hal_msgid_error("hal_spi_master_deinit fail\n", 0);
    }

    spi_master_port = HAL_SPI_MASTER_MAX;
    spi_flash_init = FLASH_NOT_INIT;
    return (status = EXTERNAL_FLASH_STATUS_OK);
}


static bsp_external_flash_status_t spi_flash_read_polling(uint32_t address, uint8_t *buffer, uint32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_buffer;
    uint32_t cmd_indx;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    cmd_indx = 0;
    /* 1,should not use 0x03 to read as max SPI frequency is 52Mhz;
       2,should not use 0xEB as SPI doesn't support 1+4+4 mode.
       3,should use 0x0B (fast read) or 0x6B (Quad read);
       4,use polling mode if read length is less than 32-4/5 bytes
       5,can't use 0xeb for winbond flash,which is 1 + 4 + 4 */
    if (0) {
        //??? maybe use spi read is better
        cmd[cmd_indx++] = SPIQ_READ;
        cmd[cmd_indx++] = address >> 16;  // 1: 3 byte address
        cmd[cmd_indx++] = address >> 8;   // 2: 3 byte address
        cmd[cmd_indx++] = address;        // 3: 3 byte address

        hal_gpt_get_free_run_count(0, &gpt_cur_count);
        gpt_count_num = gpt_cur_count;

        //log_hal_msgid_info("[Flash Read] read flash data start buffer = 0x%x, length = %d\r\n", 2, buffer, length);
        spi_send_and_receive_config.receive_buffer = buffer;
        spi_send_and_receive_config.send_data = cmd;
        spi_send_and_receive_config.send_length = cmd_indx;
        spi_send_and_receive_config.receive_length = spi_send_and_receive_config.send_length + length;

#ifdef SPI_FLASH_QUAD_MODE
        hal_spi_master_set_mode(spi_master_port, HAL_SPI_MASTER_QUAD_MODE);
#endif
        if (flash_device_info.qfrd_delay == 8) {
            //need add dummy cycle for fast read
            cmd[cmd_indx++] = 0;
        } else {
            //call hal_spi_master_set_dummy_bits to set delay bits if dummy is not byte alignement;
            //delay_bit : 0 ~ 15 bits
            hal_spi_master_set_dummy_bits(spi_master_port, flash_device_info.qfrd_delay);
        }

        hal_spi_master_set_command_bytes(spi_master_port, cmd_indx); // look command & address as command for SPI driver
        status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config);
        if (HAL_SPI_MASTER_STATUS_OK != status) {
            log_hal_msgid_error("hal_spi_master_send_polling fail -00 status = 0x%x \r\n", 1, status);
        }

        //reset to default, command is 1 byte.
        hal_spi_master_set_command_bytes(spi_master_port, 1);
#ifdef SPI_FLASH_QUAD_MODE
        hal_spi_master_set_mode(spi_master_port, HAL_SPI_MASTER_SINGLE_MODE);
#endif
        if (flash_device_info.qfrd_delay != 8) {
            //reset delay bit
            hal_spi_master_set_dummy_bits(spi_master_port, 0);
        }
    } else {
        cmd[cmd_indx++] = FAST_READ;
        cmd[cmd_indx++] = address >> 16;  // 1: 3 byte address
        cmd[cmd_indx++] = address >> 8;   // 2: 3 byte address
        cmd[cmd_indx++] = address;        // 3: 3 byte address

        if (flash_device_info.frd_delay == 8) {
            //need add dummy cycle for fast read
            cmd[cmd_indx++] = 0;
        } else {
            //call hal_spi_master_set_dummy_bits to set delay bits if dummy is not byte alignement;
            //delay_bit : 0 ~ 15 bits
            hal_spi_master_set_dummy_bits(spi_master_port, flash_device_info.frd_delay);
        }

        hal_gpt_get_free_run_count(0, &gpt_cur_count);
        gpt_count_num = gpt_cur_count;

        //log_hal_msgid_info("[Flash Read] read flash data start buffer = 0x%x, length = %d\r\n", 2, buffer, length);
        spi_send_and_receive_config.receive_buffer = buffer;
        spi_send_and_receive_config.send_data = cmd;
        spi_send_and_receive_config.send_length = cmd_indx;
        spi_send_and_receive_config.receive_length = spi_send_and_receive_config.send_length + length;

        status = hal_spi_master_send_and_receive_polling(spi_master_port, &spi_send_and_receive_config);
        if (HAL_SPI_MASTER_STATUS_OK != status) {
            log_hal_msgid_error("hal_spi_master_send_polling fail -01 status = 0x%x \r\n", 1, status);
        }

        if (flash_device_info.qfrd_delay != 8) {
            //reset delay bit
            hal_spi_master_set_dummy_bits(spi_master_port, 0);
        }
    }

    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count - gpt_count_num;
    if (debug_info_enable) {
        log_hal_msgid_info("\r\n flash read len: %d ,  tick: %d ! \r\n", 2, length, gpt_count_num);
    }
    gpt_count_num = 0;
    gpt_cur_count = 0;

    return EXTERNAL_FLASH_STATUS_OK;
}

/*
    buffer should be 4 bytes alignment and non_cacheable
*/
static bsp_external_flash_status_t spi_flash_read_dma(uint32_t address, uint8_t *buffer, uint32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_buffer;
    uint32_t cmd_indx;
    hal_spi_master_send_and_receive_config_t spi_send_and_receive_config;

    cmd_indx = 0;
#ifdef SPI_FLASH_QUAD_MODE
    cmd[cmd_indx++] = SPIQ_READ;
#else
    cmd[cmd_indx++] = FAST_READ;//SPIQ_READ;
#endif
    cmd[cmd_indx++] = address >> 16;    // 1: 3 byte address
    cmd[cmd_indx++] = address >> 8;     // 2: 3 byte address
    cmd[cmd_indx++] = address;          // 3: 3 byte address

#ifdef SPI_FLASH_QUAD_MODE
    if (flash_device_info.qfrd_delay == 8) {
        cmd[cmd_indx++] = 0x0;
    }
#else
    if (flash_device_info.frd_delay == 8) {
        cmd[cmd_indx++] = 0x0;
    } else {
        hal_spi_master_set_dummy_bits(spi_master_port, flash_device_info.frd_delay);
    }
#endif

    //hal_spi_master_set_mode(spi_master_port, HAL_SPI_MASTER_QUAD_MODE);
    //if (flash_device_info.qfrd_delay != 8) {
    //    hal_spi_master_set_dummy_bits(spi_master_port, delay_bit);
    //}
    //hal_spi_master_set_command_bytes(spi_master_port, cmd_indx); // cmd + add

    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count;
    //log_hal_msgid_info("[Flash Read] read flash data start buffer = 0x%x, length = %d\r\n", 2, buffer, length);
    spi_send_and_receive_config.receive_buffer = buffer;
    spi_send_and_receive_config.send_data = cmd;
    spi_send_and_receive_config.send_length = cmd_indx;
    spi_send_and_receive_config.receive_length = spi_send_and_receive_config.send_length + length;
#ifdef SPI_FLASH_QUAD_MODE
    hal_spi_master_set_mode(spi_master_port, HAL_SPI_MASTER_QUAD_MODE);
    if (flash_device_info.qfrd_delay != 8) {
        hal_spi_master_set_dummy_bits(spi_master_port, flash_device_info.qfrd_delay);
    }
#endif
    hal_spi_master_set_command_bytes(spi_master_port, cmd_indx);
    status = hal_spi_master_send_and_receive_dma_blocking(spi_master_port, &spi_send_and_receive_config);
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_msgid_error("[dsp_spi_burst_write] fail reason: %d\r\n", 1, status);
        return status;
    }

    //reset to default, command is 1 byte.
    hal_spi_master_set_command_bytes(spi_master_port, 1);
    //reset spi mode to single mode
    hal_spi_master_set_mode(spi_master_port, HAL_SPI_MASTER_SINGLE_MODE);
#ifndef SPI_FLASH_QUAD_MODE
    if (flash_device_info.frd_delay != 8) {
        hal_spi_master_set_dummy_bits(spi_master_port, 0); //reset delay bit to 0
    }
#endif
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_msgid_error("hal_spi_master_send_polling fail 1234 status = 0x%x \r\n", 1, status);
    }

    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count - gpt_count_num;

    if (debug_info_enable) {
        log_hal_msgid_info("\r\n flash read len: %d ,  tick: %d ! \r\n", 2, length, gpt_count_num);
    }

    gpt_count_num = 0;
    gpt_cur_count = 0;
    return EXTERNAL_FLASH_STATUS_OK;
}

/**
 * @brief This function read data from serial flash.
 * @param[in] address:  read address.
 * @param[in] buffer: buffer for read data
 * @param[in] length: read data length
 */
bsp_external_flash_status_t bsp_external_flash_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *data = non_cache_buffer;
    uint32_t read_length = 0;
    uint32_t data_length = length;
    uint32_t disksize;

    if (length == 0 || buffer == NULL) {
        return EXTERNAL_FLASH_WRONG_PARAMETER;
    }
    if (spi_flash_init != FLASH_INIT) {
        return EXTERNAL_FLASH_STATUS_NOT_INIT;
    }

    // only support 3 byte address
    address = address & ADDRESS_MASK;

    if (address & ADDRESS_CHECK_MASK) {
        return EXTERNAL_FLASH_ERROR_WRONG_ADDRESS;
    }

    disksize = get_flash_size();
    if (address >= disksize || address + length > disksize) {
        return EXTERNAL_FLASH_ERROR_WRONG_ADDRESS;
    }

    if (wait_flash_ready(1)) {
        log_hal_msgid_info("\r\n flash is busy before read! \r\n", 0);
        return EXTERNAL_FLASH_STATUS_ERROR;
    }

    if ((data_length + flash_device_info.offset) <= SPI_EXT_FLASH_MAX_READ_SIZE_WITH_POLLING) {
        status = spi_flash_read_polling(address, data, data_length);
        // please should mind: data is start from cmd+addr+1
        memcpy(buffer, &data[flash_device_info.offset], data_length);
        return status;
    }


    //while ((data_length + flash_device_info->q_offset) > FLASH_DADA_MAX_LENGTH) {
    while (data_length > FLASH_DADA_MAX_LENGTH) {
        read_length = FLASH_DADA_MAX_LENGTH;
        spi_flash_read_dma(address, data, read_length);
        memcpy(buffer, &data[flash_device_info.q_offset], read_length);
        buffer = buffer + read_length;
        address = address + read_length;
        data_length = data_length - read_length;
    }

    //log_hal_msgid_info("\r\n read addr: 0x%x  len = %d \r\n", 2, address, data_length);
    read_length = data_length;
    status = spi_flash_read_dma(address, data, data_length);
    memcpy(buffer, &data[flash_device_info.q_offset], data_length);
    return EXTERNAL_FLASH_STATUS_OK;
}


static bsp_external_flash_status_t spi_flash_write_polling(uint32_t address, uint8_t *data, int32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *buffer = non_cache_buffer;
    uint32_t cmd_indx;
    int32_t len = length;

    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count;

    //write enable
    buffer[0] = WRITE_ENABLE;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, buffer, 1)) {
        log_hal_msgid_error("\r\n hal_spi_master_send_polling fail\r\n", 0);
    }
    //wait_flash_ready(1);

    //write with polling
    cmd_indx = 0;
    buffer[cmd_indx++] = SPI_WRITE;
    buffer[cmd_indx++] = address >> 16;  //1: 3 byte address
    buffer[cmd_indx++] = address >> 8;   //2: 3 byte address
    buffer[cmd_indx++] = address;        //3: 3 byte address
    memcpy(&buffer[cmd_indx], data, len);

    //if szie is less than 32bytes, it's not neccessory to disable interrupt
    status = hal_spi_master_send_polling(spi_master_port, buffer, (len + cmd_indx));
    if (HAL_SPI_MASTER_STATUS_OK != status) {
        log_hal_msgid_info("\r\n hal_spi_master_send_polling fail\r\n", 0);
    }

    if (wait_flash_ready(1)) {
        log_hal_msgid_error("\r\n flash is busy before write! \r\n", 0);
        return EXTERNAL_FLASH_STATUS_ERROR;
    }

    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count - gpt_count_num;
    if (debug_info_enable) {
        log_hal_msgid_info("\r\n flash write len: %d ,  tick: %d ! \r\n", 2, length, gpt_count_num);
    }
    gpt_count_num = 0;
    gpt_cur_count = 0;

    return EXTERNAL_FLASH_STATUS_OK;
}

/*
    buffer should be 4 bytes alignment and non_cacheable
*/
static int32_t spi_flash_write_dma(uint32_t address, uint8_t *data, int32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *buffer = non_cache_buffer;
    uint32_t cmd_indx;

    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count;

    //write enable
    buffer[0] = WRITE_ENABLE;
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, buffer, 1)) {
        log_hal_msgid_error("\r\n hal_spi_master_send_polling fail\r\n", 0);
    }

    cmd_indx = 0;
#ifdef SPI_FLASH_QUAD_MODE
    buffer[cmd_indx++] = SPIQ_WRITE;     //quad page write
#else
    buffer[cmd_indx++] = SPI_WRITE;      //quad page write
#endif
    buffer[cmd_indx++] = address >> 16;  // 1: 3 byte address
    buffer[cmd_indx++] = address >> 8;   // 2: 3 byte address
    buffer[cmd_indx++] = address;        // 3: 3 byte address

    memcpy(&buffer[cmd_indx], data, length);
#ifdef SPI_FLASH_QUAD_MODE
    hal_spi_master_set_mode(spi_master_port, HAL_SPI_MASTER_QUAD_MODE);
#endif
    hal_spi_master_set_command_bytes(spi_master_port, cmd_indx); // cmd + add
    status = hal_spi_master_send_dma_blocking(spi_master_port, buffer, (length + cmd_indx));
    if (status != HAL_SPI_MASTER_STATUS_OK) {
        log_hal_msgid_error("[dsp_spi_burst_write] fail reason: %d\r\n", 1, status);
        return status;
    }

    //should wait device ready
    wait_flash_ready(2);

    hal_spi_master_set_command_bytes(spi_master_port, 1);
#ifdef SPI_FLASH_QUAD_MODE
    //reset spi mode to single mode
    hal_spi_master_set_mode(spi_master_port, HAL_SPI_MASTER_SINGLE_MODE);
#endif
    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count - gpt_count_num;
    if (debug_info_enable) {
        log_hal_msgid_info("\r\n flash write len: %d ,  tick: %d ! \r\n", 2, length, gpt_count_num);
    }
    gpt_count_num = 0;
    gpt_cur_count = 0;

    return EXTERNAL_FLASH_STATUS_OK;
}

/**
 * @brief This function write data to serial flash.
 * @param[in] address:  write address.
 * @param[in] data: write data
 * @param[in] length: write data length
 */
bsp_external_flash_status_t bsp_external_flash_write(uint32_t address, uint8_t *data, int32_t length)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *buffer = data;
    uint32_t write_length;
    uint32_t page_offset;
    int32_t len = length;
    uint32_t addr = address;
    uint32_t disksize;

    if (spi_flash_init != FLASH_INIT) {
        return EXTERNAL_FLASH_STATUS_NOT_INIT;
    }
    if (length == 0 || data == NULL) {
        return EXTERNAL_FLASH_WRONG_PARAMETER;
    }
    if (wait_flash_ready(2)) {
        log_hal_msgid_error("\r\n flash is busy before write! \r\n", 0);
        return status;
    }

    // only support 3 byte address
    address = address & ADDRESS_MASK;

    if (address & ADDRESS_CHECK_MASK) {
        return EXTERNAL_FLASH_ERROR_WRONG_ADDRESS;
    }

    disksize = get_flash_size();
    if (address >= disksize || address + length > disksize) {
        return EXTERNAL_FLASH_ERROR_WRONG_ADDRESS;
    }

#ifdef SPI_DEBUG_LOG
    log_hal_msgid_info("bsp_external_flash_write: addrss = 0x%x ,len= %d \r\n", 2, address, length);
#endif

    page_offset = address % WRITE_BUFFER_SIZE;
    while (len > 0) {
        write_length = min(len, (WRITE_BUFFER_SIZE - page_offset));
        page_offset = 0;
        if ((write_length + 4) <= SPI_EXT_FLASH_MAX_READ_SIZE_WITH_POLLING) {
            status = spi_flash_write_polling(addr, buffer, write_length);
            if (status != EXTERNAL_FLASH_STATUS_OK) {
                return EXTERNAL_FLASH_STATUS_ERROR;
            }
        } else {
            status = spi_flash_write_dma(addr, buffer, write_length);
            if (status != EXTERNAL_FLASH_STATUS_OK) {
                return EXTERNAL_FLASH_STATUS_ERROR;
            }
        }
        buffer += write_length;
        addr += write_length;
        len -= write_length;
    }

    return EXTERNAL_FLASH_STATUS_OK;
}

/**
 * @brief This function erase the serial flash with sector, block or chip .
 * @param[in] adress: erase start address.
 * @param[in] block_size: erase block size
 * @return
 */
bsp_external_flash_status_t bsp_external_flash_erase(uint32_t address, block_size_type_t block_size)
{
    int32_t status = FLASH_NOT_INIT;
    uint8_t *cmd = cmd_buffer;
    uint32_t cmd_indx;
    uint32_t disksize;

    if (spi_flash_init != FLASH_INIT) {
        return EXTERNAL_FLASH_STATUS_NOT_INIT;
    }
    if (block_size < FLASH_BLOCK_4K || block_size > FLASH_CHIP_ERASE) {
        return (status = EXTERNAL_FLASH_WRONG_PARAMETER);
    }
    if (wait_flash_ready(2)) {
        log_hal_msgid_error("\r\n flash is busy before erase! \r\n", 0);
        return EXTERNAL_FLASH_STATUS_BUSY;
    }
    
    // only support 3 byte address
    address = address & ADDRESS_MASK;

    if (address & ADDRESS_CHECK_MASK) {
        #ifdef SPI_DEBUG_LOG
        log_hal_msgid_info("bsp_external_flash_erase: address = %d \r\n", 1, address);
        #endif
        return EXTERNAL_FLASH_ERROR_WRONG_ADDRESS;
    }

    disksize = get_flash_size();
    if (address >= disksize) {
        #ifdef SPI_DEBUG_LOG
        log_hal_msgid_info("bsp_external_flash_erase: address = %d, disksize = 0x%x \r\n", 2, address, disksize);
        #endif
        return EXTERNAL_FLASH_ERROR_WRONG_ADDRESS;
    }

    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count;

    cmd_indx = 0;
    cmd[cmd_indx++] = WRITE_ENABLE;
    //log_hal_msgid_info("[Flash Erase] Erase flash device start \r\n", 0);
    if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, cmd, cmd_indx)) {
        log_hal_msgid_error("\r\n hal_spi_master_send_polling fail\r\n", 0);
    }

    wait_flash_ready(1);
    cmd_indx = 0;
    if (block_size == FLASH_CHIP_ERASE) {
        cmd[cmd_indx++] = CHIP_ERASE;   // CHIP ERASE  0X60 OR 0XC7
        if (HAL_SPI_MASTER_STATUS_OK != hal_spi_master_send_polling(spi_master_port, cmd, cmd_indx)) {
            log_hal_msgid_error(" \r\n hal_spi_master_send_polling fail\r\n ", 0);
            return EXTERNAL_FLASH_STATUS_ERROR;
        }
    } else {
        cmd_indx = 0;
        if (block_size == FLASH_BLOCK_4K) {
            cmd[cmd_indx++] = SECTOR_4K_ERASE;   // SECTOR 4K ERASE
        } else if (block_size == FLASH_BLOCK_32K) {
            cmd[cmd_indx++] = BLOCK_32K_ERASE;   // BLOCK 32K ERASE
        } else if (block_size == FLASH_BLOCK_64K) {
            cmd[cmd_indx++] = BLOCK_64K_ERASE;   // BLOCK 64K ERASE
        }

        cmd[cmd_indx++] = address >> 16;   // 1: 3 byte address
        cmd[cmd_indx++] = address >> 8;    // 2: 3 byte address
        cmd[cmd_indx++] = address;         // 3: 3 byte address
        //log_hal_msgid_info("\r\n flash erase address is : 0x%x! \r\n", address);
        status = hal_spi_master_send_polling(spi_master_port, cmd, cmd_indx);
        if (HAL_SPI_MASTER_STATUS_OK != status) {
            log_hal_msgid_error(" \r\n[Flash Erase] hal_spi_master_send_polling fail\r\n ", 0);
            return EXTERNAL_FLASH_STATUS_ERROR;
        }
    }

    hal_gpt_get_free_run_count(0, &gpt_cur_count);
    gpt_count_num = gpt_cur_count - gpt_count_num;
    if (debug_info_enable) {
        log_hal_msgid_info("\r\n flash erase tick: %d ! \r\n", 1, gpt_count_num);
    }
    gpt_count_num = 0;
    gpt_cur_count = 0;

    //Wait flash erase finished
    if (wait_flash_ready(1000) == FLASH_STATUS_IDLE) {
        return EXTERNAL_FLASH_STATUS_OK;
    }

    return EXTERNAL_FLASH_STATUS_ERROR;
}
#endif //BSP_EXTERNAL_SERIAL_FLASH_ENABLED

