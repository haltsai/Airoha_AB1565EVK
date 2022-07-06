/* Copyright Statement:
 *
 * (C) 2020  Airoha Technology Corp. All rights reserved.
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
#include "DA7212.h"
#include "memory_attribute.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "bsp_audio_ext_codec_config.h"
#include "assert.h"

//RG dump list
const uint8_t RG_Dump_list[] = {0x1D, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37,
                                0x38, 0x39, 0x3A, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x50, 0x51, 0x60, 0x61,
                                0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x84, 0x90, 0x92, 0x93, 0x94, 0x95,
                                0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA6, 0xA7, 0xA8, 0xA9, 0xAB,
                                0xAC, 0xAD, 0xAE, 0xAF, 0xB0, 0xB1, 0xB2, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xE0, 0xFD
                               };

static ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN uint8_t i2c_read_data = 0;
static ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char g_i2c_send_data[1] = {0x00};
static ATTR_ZIDATA_IN_NONCACHED_RAM_4BYTE_ALIGN unsigned char g_i2c_fifo[9] = {0x00};
/*I2C DMA transfer semaphore*/
static SemaphoreHandle_t I2C_transfer_Semaphore = NULL;
static bool g_bsp_i2c_inited_flg = false;

/**
 * @brief i2c_dma_callback() function is use for I2C DMA transfer data callback
 *
 * @param slave_addr
 * @param event
 * @param user_data
 */
static void i2c_dma_callback(unsigned char slave_addr, hal_i2c_callback_event_t event, void *user_data)
{
    BaseType_t xHigherPriorityTaskWoken;

    //walk around build warning,no use
    slave_addr = slave_addr;
    user_data = user_data;

    if (event != HAL_I2C_EVENT_SUCCESS) {
        log_hal_msgid_error("[EX_CODEC_I2C] i2c_dma_callback err event = %d", 1, event);
        assert(0);
    }

    if (xSemaphoreGiveFromISR(I2C_transfer_Semaphore, &xHigherPriorityTaskWoken) == pdFALSE) {
        log_hal_msgid_error("cannot give I2C semaphore", 0);
        assert(0);
    }
}


/*BSP porting layer*/
/**
 * @brief bsp_i2c_read_data() function is use for I2C bus read data
 *
 * @param dev_addr
 * @param reg_addr
 * @param read_data
 * @param len
 * @return uint16_t
 */
static uint16_t bsp_i2c_read_data(uint8_t dev_addr, uint8_t reg_addr, uint8_t *read_data, uint16_t len)
{
    hal_i2c_config_t i2c;
    hal_i2c_status_t sta;
    hal_i2c_send_to_receive_config_ex_t i2c_send_to_receive_config_ex;

    if (false == g_bsp_i2c_inited_flg) {
        I2C_transfer_Semaphore = xSemaphoreCreateBinary();
        if (I2C_transfer_Semaphore == NULL) {
            log_hal_msgid_error("[EX_CODEC_I2C] create I2C semaphore fail", 0);
            assert(0);
        }
        /*init i2c hw*/
        /*gpio config*/
        //hal_pinmux_set_function(HAL_GPIO_20, 4);
        //hal_pinmux_set_function(HAL_GPIO_21, 4);

        /*i2c hw init*/
        i2c.frequency = HAL_I2C_FREQUENCY_400K;
        if (hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c)) {
            log_hal_msgid_error("[EX_CODEC_I2C]g_sensor i2c hw init fail", 0);
            assert(0);
        }

        /*register callback*/
        hal_i2c_master_register_callback(HAL_I2C_MASTER_0, i2c_dma_callback, NULL);
        hal_i2c_master_set_io_config(HAL_I2C_MASTER_0, HAL_I2C_IO_OPEN_DRAIN);

        g_bsp_i2c_inited_flg = true;
    }
    /*read data from g-sensor*/
    g_i2c_send_data[0] = reg_addr;
    i2c_send_to_receive_config_ex.receive_buffer = read_data;
    i2c_send_to_receive_config_ex.receive_bytes_in_one_packet = len;
    i2c_send_to_receive_config_ex.receive_packet_length = 1;
    i2c_send_to_receive_config_ex.send_packet_length = 1;
    i2c_send_to_receive_config_ex.send_bytes_in_one_packet = 1;
    i2c_send_to_receive_config_ex.send_data = g_i2c_send_data;
    i2c_send_to_receive_config_ex.slave_address = dev_addr;

    sta = hal_i2c_master_send_to_receive_dma_ex(HAL_I2C_MASTER_0, &i2c_send_to_receive_config_ex);
    if (HAL_I2C_STATUS_OK != sta ) {
        log_hal_msgid_error("[EX_CODEC_I2C] g_sensor i2c driver read data fail,err sta = %d,dev_addr = 0x%08x,reg_addr = 0x%08x", 3, sta, dev_addr, reg_addr);
        assert(0);
    }
    //wait transfer done
    if (xSemaphoreTake(I2C_transfer_Semaphore, portMAX_DELAY) == pdFALSE) {
        log_hal_msgid_error("[EX_CODEC_I2C]bsp_i2c_read_data cannot take I2C semaphore!", 0);
        assert(0);
    }

    return 0;
}

/**
 * @brief bsp_i2c_write_data() function is use for I2C bus write data
 *
 * @param dev_addr
 * @param reg_addr
 * @param write_data
 * @param len
 * @return uint16_t
 */
static uint16_t bsp_i2c_write_data(uint8_t dev_addr, uint8_t reg_addr, uint8_t *write_data, uint16_t len)
{
    hal_i2c_config_t i2c;
    hal_i2c_status_t sta;
    hal_i2c_send_config_t send_cfg;
    unsigned char i;

    if (false == g_bsp_i2c_inited_flg) {
        I2C_transfer_Semaphore = xSemaphoreCreateBinary();
        if (I2C_transfer_Semaphore == NULL) {
            log_hal_msgid_error("[EX_CODEC_I2C] create I2C semaphore fail", 0);
            assert(0);
        }
        /*init i2c hw*/
        /*gpio config*/
        //hal_pinmux_set_function(HAL_GPIO_20, 4);
        //hal_pinmux_set_function(HAL_GPIO_21, 4);

        /*i2c hw init*/
        i2c.frequency = HAL_I2C_FREQUENCY_400K;
        if (hal_i2c_master_init(HAL_I2C_MASTER_0, &i2c)) {
            log_hal_msgid_error("[EX_CODEC_I2C]g_sensor i2c hw init fail", 0);
            assert(0);
        }
        /*register callback*/
        hal_i2c_master_register_callback(HAL_I2C_MASTER_0, i2c_dma_callback, NULL);
        hal_i2c_master_set_io_config(HAL_I2C_MASTER_0, HAL_I2C_IO_OPEN_DRAIN);

        g_bsp_i2c_inited_flg = true;
    }

    /*write data from g-sensor*/
    g_i2c_fifo[0] = reg_addr;
    for (i = 0; i < len; i++) {
        g_i2c_fifo[i + 1] = *(write_data + i);
    }

    send_cfg.send_bytes_in_one_packet = len + 1;
    send_cfg.slave_address = dev_addr;
    send_cfg.send_packet_length = 1;
    send_cfg.send_data = g_i2c_fifo;

    sta = hal_i2c_master_send_dma_ex(HAL_I2C_MASTER_0, &send_cfg);
    if (HAL_I2C_STATUS_OK != sta) {
        log_hal_msgid_error("[EX_CODEC_I2C] g_sensor i2c driver write data fail,err sta = %d,dev_addr = 0x%08x,reg_addr = 0x%08x", 3, sta, dev_addr, reg_addr);
        assert (0);
    }

    //wait transfer done
    if (xSemaphoreTake(I2C_transfer_Semaphore, portMAX_DELAY) == pdFALSE) {
        log_hal_msgid_error("[EX_CODEC_I2C] bsp_i2c_write_data cannot take I2C semaphore!", 0);
        assert(0);
    }

    return 0;
}

/**
 * @brief DA7212_dac_init() API is use for DA7212 init flow
 *
 */
void DA7212_dac_init(void)
{
    uint8_t write_data = 0;
    //WRITE DA7212 0x92	0x00 //Set Ramp rate to default
    write_data = 0x00;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_GAIN_RAMP_CTRL_ADDR, &write_data, 1);
    //delay 40ms
    hal_gpt_delay_ms(40);
    //Enable Digital LDO
    write_data = 0x80;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_LDO_CTRL_ADDR, &write_data, 1);
    //Enable AIF 32bit I2S mode
    //write_data = 0xCC;
    //bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_DAI_CTRL_ADDR, &write_data, 1);
    //Set incoming sample rate to 48kHz
    //write_data = 0x0B;
    //bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_SR_ADDR, &write_data, 1);

    //Enable AIF 16bit I2S mode
    write_data = 0xC0;
    bsp_i2c_write_data(DA7212_I2C_ADDR,DA7212_DAI_CTRL_ADDR,&write_data,1);
    //Set incoming sample rate to 16kHz
    write_data = 0x05;
    bsp_i2c_write_data(DA7212_I2C_ADDR,DA7212_SR_ADDR,&write_data,1);

    //Set PC sync to resync
    write_data = 0x02;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_PC_COUNT_ADDR, &write_data, 1);
    //PLL_CTRL PLL disabled
    write_data = 0x04;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_PLL_CTRL_ADDR, &write_data, 1);
    //Enable Digital LDO
    write_data = 0x80;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_LDO_CTRL_ADDR, &write_data, 1);
    //Set Ramp rate to 1 second
    write_data = 0x02;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_GAIN_RAMP_CTRL_ADDR, &write_data, 1);
    //Enable AIF 32bit I2S mode
    //write_data = 0xCC;
    //bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_DAI_CTRL_ADDR, &write_data, 1);
    //Set incoming sample rate to 48kHz
    //write_data = 0x0B;
    //bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_SR_ADDR, &write_data, 1);

    //Enable AIF 16bit I2S mode
    write_data = 0xC0;
    bsp_i2c_write_data(DA7212_I2C_ADDR,DA7212_DAI_CTRL_ADDR,&write_data,1);
    //Set incoming sample rate to 16kHz
    write_data = 0x05;
    bsp_i2c_write_data(DA7212_I2C_ADDR,DA7212_SR_ADDR,&write_data,1);

    //Set PC sync to resync
    write_data = 0x02;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_PC_COUNT_ADDR, &write_data, 1);
    //PLL_INTEGER for 12.288MHz MCLK
    write_data = 0x20;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_PLL_INTEGER_ADDR, &write_data, 1);
    //PLL_CTRL SRM & PLL enabled
    write_data = 0xC4;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_PLL_CTRL_ADDR, &write_data, 1);
    //Route DAI to Outputs
    write_data = 0x32;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_DIG_ROUTING_DAC_ADDR, &write_data, 1);
    //MIXOUT_L input from DACL
    write_data = 0x08;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_MIXOUT_L_SELECT_ADDR, &write_data, 1);
    //MIXOUT_R input from DACR
    write_data = 0x08;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_MIXOUT_R_SELECT_ADDR, &write_data, 1);
    //Enable charge pump, CP_MOD mode, CPVDD/1 and Boost CP
    write_data = 0xCD;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_CP_CTRL_ADDR, &write_data, 1);
    //Set CP threshold to 0x36
    write_data = 0x36;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_CP_VOL_THRESHOLD1_ADDR, &write_data, 1);
    //Set CP Tau DELAY to 64ms
    write_data = 0xA5;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_CP_DELAY_ADDR, &write_data, 1);
    //Set volume to -12dB
    write_data = 0x2D;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_HP_L_GAIN_ADDR, &write_data, 1);
    //Set volume to -12dB
    write_data = 0x2D;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_HP_R_GAIN_ADDR, &write_data, 1);
    //Enable DAC, Mix and HP amplifiers
    write_data = 0xF1;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_SYSTEM_MODES_OUTPUT_ADDR, &write_data, 1);
    //Set tone gen gain to -18dB
    write_data = 0x60;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_TONE_GEN_CFG2_ADDR, &write_data, 1);
    //Set tone gen gain to -18dB
    write_data = 0x00;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_DAI_CLK_MODE_ADDR, &write_data, 1);

    log_hal_msgid_info("[EX_CODEC_I2C] DA7212_config done.", 0);
}

/**
 * @brief DA7212_dac_enable() API is use for DAC enable I2S MCLK
 *
 */
void DA7212_dac_enable(void)
{
    //enable I2SM MClk
    if (HAL_AUDIO_STATUS_OK != hal_audio_mclk_enable(true, AFE_MCLK_PIN_FROM_I2S0, AFE_APLL2, 3)) {
        log_hal_msgid_error("[EX_CODEC_I2C] DA7212_enable error.", 0);    
        assert(0);
    }
    log_hal_msgid_info("[EX_CODEC_I2C] DA7212_enable done.", 0);
}

/**
 * @brief DA7212_dac_disable() API is use for DAC disable I2S MCLK
 *
 */
void DA7212_dac_disable(void)
{
    //disable I2SM MClk
    if (HAL_AUDIO_STATUS_OK != hal_audio_mclk_enable(false, AFE_MCLK_PIN_FROM_I2S0, AFE_APLL2, 3)) {
        log_hal_msgid_error("[EX_CODEC_I2C] DA7212_dac_disable error.", 0);    
        assert(0);
    }
    log_hal_msgid_info("[EX_CODEC_I2C] DA7212_disable done.", 0);
}

/**
 * @brief DA7212_dac_deinit() API is use for DA7212 disable and reset
 *
 */
void DA7212_dac_deinit(void)
{
    uint8_t write_data = 0;

    write_data = 0x00;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_SYSTEM_MODES_INPUT_ADDR, &write_data, 1);
    write_data = 0x01;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_SYSTEM_MODES_OUTPUT_ADDR, &write_data, 1);
    //delay 500ms
    hal_gpt_delay_ms(500);
    write_data = 0x80;
    bsp_i2c_write_data(DA7212_I2C_ADDR, DA7212_CIF_CTRL_ADDR, &write_data, 1);

    log_hal_msgid_info("[EX_CODEC_I2C] DA7212_deinit done.", 0);
}

/**
 * @brief DA7212_dump_rg_list() API is use for dump DA7212 internal RG settings
 *
 */
void DA7212_dump_rg_list(void)
{
    uint32_t i = 0;

    for (i = 0; i < sizeof(RG_Dump_list); i++) {
        bsp_i2c_read_data(0x1A, RG_Dump_list[i], &(i2c_read_data), 1);
        log_hal_msgid_info("[EX_CODEC_I2C] DA7212 dump RG addr = 0x%02x,rg_read_back_data = 0x%02x\r\n", RG_Dump_list[i], i2c_read_data, 2);
    }
}